/* UNKNOWN_2C0D00.CPP: a list of up to 64 circular obstacles on the ground
   plane that a path is steered around: adding them, finding the ones a point
   or a ray hits, and grouping the ones that overlap */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_2c0d00.h"
#include "unknown_11cc90.h"
#include "vector2f_products.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

#include "ai_actor.h"
#include "path.h"
#include "object_markers.h"
#include "props.h"
#include "object_queries.h"
#include "unknown_1428b0.h"

inline void point2f_set(point2f *point, real x, real y);

real function_30bf0(vector3f *vector);
point3f *function_b9dd0(long object_index, point3f *result);
bool object_or_parent_hidden(long object_index);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_11bed0(s_location *location, point3f const *point);
short __stdcall function_bb050(long mask, dword type_mask, void const *location,
    point3f const *position, real radius, long *objects, short maximum_count);
short function_26f930(long object_index, s_object_marker *markers, short capacity, word flags);
short function_26fbf0(long object_index, s_object_marker *markers, short capacity, long flags);
short function_26fe20(long object_index, s_object_marker *markers, short capacity, long flags);
bool function_26f8f0(s_object_marker const *marker);
long function_25d810(long object_index, long actor_index, bool create);

struct s_obstacle_object_flags_2c
{
    dword : 22;
    dword restricted_spheres : 1;
    dword no_obstacle : 1;
    dword : 8;
};

struct s_obstacle_unit_flags_2c
{
    byte : 2;
    byte no_obstacle : 1;
    byte : 5;
};

struct s_obstacle_definition_flags_2c
{
    byte : 3;
    byte no_obstacle : 1;
    byte : 4;
};

struct s_obstacle_motion_definition_2c
{
    word flags;
    byte field_2[0xc - 2];
    short priority;
};

struct s_obstacle_sphere_2c
{
    short node;
    word flags;
    point3f center;
    real radius;
};

struct s_obstacle_physics_definition_2c
{
    byte field_0[0x24];
    long count;
    s_obstacle_sphere_2c *spheres;
};


/* the nearest obstacle a ray hits */
struct s_obstacle_hit
{
	real distance;
	short index;
	short group;
};

static inline real dot_product2d(point2f const *a, point2f const *b)
{
	return a->x * b->x + a->y * b->y;
}

static inline real length_sq2f(point2f const *v)
{
	return v->x * v->x + v->y * v->y;
}

static inline bool bit_vector_test(dword const *bits, long index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

static inline void bit_vector_set(dword *bits, long index)
{
	bits[index >> 5] |= 1 << (index & 31);
}

/* the two directions, turned either way from the direction, that just touch
   a circle of the radius at the distance, and how far along them it is
   touched */
// @retail 0x2c0c40
void tangent_directions(point2f const *direction, point2f *left, point2f *right, real distance, real radius, real *tangent_length)
{
	real sine = 1.0f;
	real cosine;
	real negative_sine;

	if (distance > 0.0f)
	{
		sine = radius / distance;
		if (sine > 1.0f)
		{
			sine = 1.0f;
		}
	}
	cosine = (real)sqrt(1.0f - sine * sine);
	negative_sine = 0.0f - sine;
	point2f_set(left, direction->x * cosine - negative_sine * direction->y,
		direction->x * negative_sine + cosine * direction->y);
	point2f_set(right, cosine * direction->x - sine * direction->y,
		direction->x * sine + cosine * direction->y);
	*tangent_length = cosine * distance;
}

static inline real normalize_local2d_2c(point2f *v)
{
	real magnitude = (real)sqrt(length_sq2f(v));

	if (fabs(magnitude) < 0.0001f)
	{
		magnitude = 0.0f;
	}
	else
	{
		real inverse = 1.0f / magnitude;

		v->x *= inverse;
		v->y *= inverse;
	}
	return magnitude;
}

/* the directions from the point that pass the obstacle, grown by the radius,
   on either side */
// @retail 0x2c1d90
void obstacle_tangent_directions(point2f const *point, s_obstacle_list const *list, short index, real radius,
	point2f *left, point2f *right, real *tangent_length)
{
	s_obstacle const *obstacle = &list->obstacles[index];
	point2f direction;
	real distance;

	vector2d_from_points2d(point, &obstacle->center, &direction);
	distance = normalize_local2d_2c(&direction);
	if (distance > 0.0f)
	{
		tangent_directions(&direction, left, right, distance, obstacle->radius + radius + 0.00390625f, tangent_length);
	}
	else
	{
		left->x = 0.0f;
		left->y = -1.0f;
		right->x = 0.0f;
		right->y = 1.0f;
		*tangent_length = 0.0f;
	}
}

// @retail 0x2c0d00
bool obstacle_list_add(s_obstacle_list *list, word flags, point2f const *center, long object_index, real radius)
{
	real volatile const *radius_reference = &radius;
    s_obstacle *obstacle;

	if (list->count == 64)
	{
		return false;
	}
	obstacle = &list->obstacles[list->count++];
	if (flags & 1)
	{
		list->flag0_count++;
	}
	if (flags & 8)
	{
		list->flag3_count++;
	}
	obstacle->flags = flags;
	obstacle->group = NONE;
	obstacle->object_index = object_index;
	real y = *(real volatile const *)&center->y;
	memcpy(&obstacle->center.x, &center->x, sizeof(real));
	obstacle->center.y = y;
    obstacle->radius = *radius_reference;
	return true;
}

void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
    s_path_settings const *settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
    point3f const *position, real radius, vector3f const *direction, long unit_index, long ignore_index);

// score 882 (was 882): frame 0x2cb8 versus 0x2cac; actor/unit retention and priority/query/marker homes remain.
// point workspace names 866; initial actor value 869; query index value 916;
// conditional self-unit 911; wide initial predicates 1011; native location copy 911.
// Frame pointer 1207; size preference 1436; native object fields 866;
// direct restricted predicate 911; direct device predicate 912.
// frame 0x2cb8 versus 0x2cac; actor/unit and workspace homes differ.
// Classification order 898; common center 887; common offset 867; marker count 866; inline depth two 866;
// paired object centers 907; first/sphere centers 907; vehicle-check/sphere centers 907.
// frame 0x2cb8 versus 0x2cac and actor/unit retention;
// inline depth zero 1008; global optimization off 3195;
// long blocking 912; long classification 949; long enabled 906 (plain);
// fast control 916, trials 1008/3199/910/947/916.
// Native category on retained homes 910 (frame 0x2cb4); direct query
// members 911; current priority long 894; relationship results long 893;
// sphere flag byte 917 (frame 0x2cb4). Original body kept.
// Frame 0x2cb8 versus 0x2cac; actor/unit and query/marker homes remain.
// category switch 948; shared marker workspace 914; object matrix across iterations 866; sphere radius
// before transform 878; flags at ordinary insertion 910; motion enabled guard 866.
// frame 0x2cb8 versus 0x2cac and actor/unit homes remain. Plain direct sphere differences
// 881; direct object differences 866; both 881; shared raised-point setter 866; ordered sphere-filter join
// 866. Fast scores 927/916/927/916/916; full control 866.
// Separate first/second/third marker ordinals long 913f/934f/916f; object ordinal alone
// 897f/924 plain; only marker counts long 914f (control 916f). Original bounded traversals restored.
// Direction dot operand order 916f; single expression 928f; both 928f;
// range read order 916f; alignment test order 916f; predicate declarations 916f.
// Round 385: Word obstacle mask with native sphere/motion combinations, reduced
// 916/894/914/897/914/897/897/897; word+byte sphere 917 plain, plus byte motion 919,
// plus separate location/raised workspaces 898. Original masks and query storage restored.
// Individual full-width ignore/block/minimum/maximum priorities 897/897/897/900;
// ignore-block/minimum-maximum/ignore-minimum/block-maximum pairs 894/920/894/894 (fast).
// Original signed-short limits, query storage, qualifiers and complete body restored.
// Round 176: Immutable declaration-produced actor with original/ordinary settings/ordinary unit/both
// input homes 957/991/978/989; with independent location and raised point 973/993/949/982.
// All original actor publication, query storage, qualifiers, operands and complete body restored.
// Round 37: normalized/object, normalized/sphere and all relative-position workspaces shared per
// object 866/867/867, at entry 866/867/867; all with independent query storage 889, ordinary actor
// pointer 959. Original vectors, query record, qualifiers, operands and complete body restored.
// Round 79 (plain): no unit_index volatile 917 (fast 867: fast misleads here); unit no-obstacle bit
// tested as a word `(bool)((w >> 2) & 1)` 919 (retail's `mov dl, byte; shr dl, 2; test dl, 1`);
// location and raised point as separate locals 889; both 918; no actor volatile 959; no settings
// volatile 931. Retail holds actor_index/unit_index/self_unit in ebx/edi/ebp (reloaded on the
// object loop's back edge) with its zero in ebp.
// Round 220: Paired exhaustive confidence results bool/byte/word/long 914/914/928/942f; shared real dot
// then both tests 870/870/909/892f (unchanged reduced control 866, full 899). All regress against
// the calibrated control; original confidence defaults, query storage, qualifiers and body restored.
// Round 321: Mask-owned output selection with word obstacle/byte sphere/Boolean moving subsets 0-7: 948/947/960/959/949/948/963/962f (control 866, full 899).
// Round 321: Frame/actor/unit homes remain; all original Boolean, masks, qualifiers and body restored.
// Round 35 (lane 11): location and raised point as plain locals instead of the grouping struct 888 fast
// but 906 plain (fast misleads here; frame 0x2cb4). All three volatiles dropped 1014f (aligned 630 -> 583;
// settings in EBX, zero in EDI), that with the settings volatile kept 1014f (same code), plus self_unit
// declared first 1014f. On the plain-local form (fast): all volatiles dropped 1005 (frame 0x2cb0, aligned 525); no unit_index volatile 924; no actor
// volatile 974 (aligned 524); no settings volatile 904; raised at function scope 888; location declared
// before the actor block 888. Ours hoists the object-table global load to the entry for both lookups;
// retail loads it inside each block (four loads against three).
// Round 27 (m3 lane 15, plain): retail reads `settings` once into ECX, keeps `unit_index` in EDI and reads
// the actor's fields from the computed actor address (none of the three volatiles is retail's), and EBP is
// self_unit (its NULL is the entry's zero, so the priority/range zeros come from it). The settings
// volatile dropped 882 (kept, 3705 bytes); on 899: the location/raised grouping struct as plain locals
// 906 (3718 bytes), no unit_index volatile 917, no actor volatile 959 (3680 bytes); on 882: no unit_index
// volatile 938, plain locals 925 (3702 bytes), no actor volatile 996 (3648 bytes), both 998 (3646 bytes),
// self_unit declared first 882 (same code); the previous-direction dot and the other-actor offset and dot
// through dot3f/vector3d_from_points3d 893 (3694 bytes), with the object offset and its squared length
// through vector3d_from_points3d/length_sq3f too 899 (3690 bytes). Retail lays the type-0 arm's
// other-actor branch out before the join (the offset/radius test) and its `else if` branch and types
// 1/7/11 after it; ours keeps source order.
// Round 121 (m3 lane 5, plain): aligned by opcode, the bodies differ in one moved block: retail's
// other-actor branch falls into the join (offset/radius test, no-obstacle bit, classification start)
// and the second type-0 branch and the type 1/7/11 arms sit after the classification's first `jmp`;
// ours ends each inflation path with `sete bl; jmp` (skip flag in BL, tail-duplicated) where retail
// keeps the flag at [esp+0x12]. Retail also re-reads the type byte at each later use (range test,
// `1 << type`, sphere loop). The type read at each use, no type local 903 (3695 bytes); that without
// the unit_index volatile 938, without the actor volatile 978, without both 999; that with the skip
// flag assigned before each inflation block 949.
// Post-admission restricted/classification/device default subsets 1..7:
// 882/906/906/914/914/906/906f (control 931); all plus late inflation 906f.
// Restricted/classification/device alone 918/932/918 plain. Frame 0x2cb8 remains;
// all original constant lifetimes, qualifiers, buffers and complete body restored.
// Round 23 (lane 18, plain; fast misleads here, 931 fast against 882 plain): aligned before straight 882;
// the 0x26c flag before the dot 883; self unit chosen right after its fetch 881; the range pair before
// the ignore/block priorities 881; range maximum read before the minimum 882 (one-point moves, not kept).
// @retail 0x2c0d60
void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
    s_path_settings const *settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
    point3f const *position, real radius, vector3f const *direction, volatile long unit_index, long ignore_index)
{
    short ignore_priority = 0;
    short block_priority = 0;
    short range_min = 0;
    short range_max = 0;
    byte *self_unit = NULL;
    s_actor_view *volatile actor = NULL;
    bool straight = true;
    bool aligned = true;
    bool local_4d3867 = false;
    if (actor_index != NONE)
    {
        actor = actor_get(actor_index);
        byte *unit = (byte *)ai_object_get(actor->unknown018);
        if (previous_direction)
        {
            real dot = direction->k * previous_direction->k;
            dot += direction->j * previous_direction->j;
            dot += direction->i * previous_direction->i;
            straight = dot > 0.95f;
            aligned = dot > 0.8660253882408142f;
        }
        local_4d3867 = actor->unknown26c != NONE;
        if (unit[0xaa] == 0)
            self_unit = unit;
        if (settings)
        {
            if (flags & 1) ignore_priority = settings->unknown0e;
            if (flags & 4) block_priority = settings->unknown10;
            if (flags & 2)
            {
                range_min = settings->unknown12;
                range_max = settings->unknown14;
            }
        }
    }
    struct { s_location location; point3f raised; } query;
    s_location &location = query.location;
    if (unit_index != NONE)
        location = *(s_location *)((byte *)ai_object_get(unit_index) + 0x28);
    else
    {
        point3f &raised = query.raised;
        raised.x = g_4687b0->i * 0.05f + position->x;
        raised.y = g_4687b0->j * 0.05f + position->y;
        raised.z = g_4687b0->k * 0.05f + position->z;
        function_11bed0(&location, &raised);
    }
    long object_indices[128];
    short object_count = function_bb050(1, 0x8c3, &location, position, radius, object_indices, 128);
    s_object_marker first_markers[32];
    s_object_marker second_markers[32];
    s_object_marker third_markers[32];
    for (short i = 0; i < object_count; ++i)
    {
        long object_index = object_indices[i];
        byte *object = (byte *)ai_object_get(object_index);
        real inflation = 0.0f;
        bool skip_classification = false;
        bool device_spheres = false;
        bool restricted_spheres = false;
        if (object_index == unit_index || object_index == ignore_index)
            continue;
        s_obstacle_object_flags_2c *object_flags = (s_obstacle_object_flags_2c *)(object + 4);
        if (TEST_FIELD_BIT(object_flags->restricted_spheres))
            restricted_spheres = true;
        else if (TEST_FIELD_BIT(object_flags->no_obstacle))
            continue;
        if (object_or_parent_hidden(object_index))
            continue;
        if (self_unit && self_unit[0x3dc] == 1 && object_index == *(long *)(self_unit + 0x448))
            continue;
        long type = (char)object[0xaa];
        if (type == 0)
        {
            if (TEST_FIELD_BIT(((s_obstacle_unit_flags_2c *)(object + 0x10a))->no_obstacle))
                continue;
            if (actor_index != NONE)
            {
                long reference_index = function_25d810(object_index, actor_index, false);
                if (reference_index != NONE && prop_ref_get(reference_index)->state < 1)
                    continue;
            }
            long other_actor_index = *(long *)(object + 0x12c);
            if (other_actor_index != NONE)
            {
                s_actor_view *other = actor_get(other_actor_index);
                bool enemy = actor && function_1df560(other->unknown024, actor->unknown024);
                if (*((byte *)other + 0x5d0) && (!actor || actor->unknown26c == NONE))
                {
                    point3f center;
                    function_b9dd0(object_index, &center);
                    vector3f offset;
                    offset.i = center.x - position->x;
                    offset.j = center.y - position->y;
                    offset.k = center.z - position->z;
                    if (function_30bf0(&offset) > 0.0f)
                    {
                        vector3f const *other_direction = (vector3f const *)((byte *)other + 0x5ec);
                        real dot = offset.k * other_direction->k + offset.j * other_direction->j + offset.i * other_direction->i;
                        if (dot > magnitude3d(other_direction) * 0.7071067690849304f)
                            continue;
                    }
                }
                if (actor)
                {
                    if (actor->unknown26c != NONE && !enemy)
                        inflation = 1.0f;
                    else if (enemy && actor->unknown223)
                        inflation = 0.5f;
                }
                skip_classification = !enemy;
            }
            else if (actor && *(long *)(object + 0x13c) != NONE)
            {
                bool enemy = team_is_enemy(actor->unknown024, 1);
                if (actor->unknown26c != NONE && !enemy)
                    inflation = 1.0f;
                else if (enemy && actor->unknown223)
                    inflation = 0.5f;
                skip_classification = !enemy;
            }
        }
        else if (type == 1)
        {
            if (actor && actor->unknown26c == NONE)
            {
                short team = *(short *)(object + 0x138);
                skip_classification = team == actor->unknown024 || !function_1df560(team, actor->unknown024);
            }
        }
        else if (type == 7)
        {
            byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            word device_flags = *(word *)(definition + 0x11e);
            if (!(device_flags & 1) || ((device_flags & 2) && (*(real *)(object + 0x140) == 1.0f || !(object[0x1cc] & 1))))
                device_spheres = true;
        }
        else if (type == 11)
        {
            long component_index = *(long *)(object + 0xb4);
            if (component_index != NONE && *(char *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0 + 0x1c) <= 1)
                continue;
        }
        vector3f offset;
        offset.i = *(real *)(object + 0x30) - position->x;
        offset.j = *(real *)(object + 0x34) - position->y;
        offset.k = *(real *)(object + 0x38) - position->z;
        real object_radius = *(real *)(object + 0x3c) + radius;
        real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
        if (!(object_radius * object_radius >= distance_squared))
            continue;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        if (TEST_FIELD_BIT(((s_obstacle_definition_flags_2c *)(definition + 2))->no_obstacle))
            continue;
        s_obstacle_motion_definition_2c *motion = NULL;
        bool blocking = false;
        dword obstacle_flags = 0;
        dword sphere_flags = 0;
        if (settings->unknown18 != object_index && !skip_classification)
        {
            short priority = 0;
            bool enabled = true;
            if (*(long *)(definition + 0x5c) > 0)
            {
                motion = *(s_obstacle_motion_definition_2c **)(definition + 0x60);
                if ((motion->flags & 2) && *(real *)(object + 0xec) <= 0.0f)
                    enabled = false;
                else
                    priority = motion->priority;
            }
            if (priority == 0)
            {
                long component_index = *(long *)(object + 0xb4);
                if (component_index != NONE)
                    priority = *(char *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0 + 0x1c);
                if (!enabled)
                    continue;
            }
            if (priority > 0)
            {
                if (ignore_priority >= priority)
                    continue;
                if (block_priority >= priority && aligned)
                {
                    obstacle_flags = 0x10;
                    blocking = true;
                }
                else if (range_min <= priority && range_max >= priority && type != 0)
                {
                    obstacle_flags = 0x20;
                    blocking = true;
                }
            }
            if ((flags & 8) && straight)
            {
                short count = function_26f930(object_index, first_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    s_object_marker const *marker = &first_markers[m];
                    if (function_26f8f0(marker))
                    {
                        real dot = marker->matrix.up.k * (position->z - marker->matrix.position.z);
                        dot += marker->matrix.up.j * (position->y - marker->matrix.position.y);
                        dot += (position->x - marker->matrix.position.x) * marker->matrix.up.i;
                        if (dot >= -0.2f && dot < marker->unknown6c + 0.2f &&
                            fabs(marker->matrix.left.k * direction->k + marker->matrix.left.j * direction->j + marker->matrix.left.i * direction->i) >= 0.9f)
                        {
                            obstacle_flags |= 0x40;
                            blocking = true;
                            break;
                        }
                    }
                }
            }
            if ((flags & 0x10) && aligned)
            {
                short count = function_26fbf0(object_index, second_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    s_object_marker const *marker = &second_markers[m];
                    if (function_26f8f0(marker))
                    {
                        real dot = marker->matrix.up.k * (position->z - marker->matrix.position.z);
                        dot += marker->matrix.up.j * (position->y - marker->matrix.position.y);
                        dot += marker->matrix.up.i * (position->x - marker->matrix.position.x);
                        if (dot >= 0.1f && dot < marker->unknown6c - 0.1f &&
                            fabs(marker->matrix.left.k * direction->k + marker->matrix.left.j * direction->j + direction->i * marker->matrix.left.i) >= 0.9f)
                        {
                            obstacle_flags |= 0x100;
                            blocking = true;
                            break;
                        }
                    }
                }
            }
            if ((flags & 0x20) && aligned)
            {
                short count = function_26fe20(object_index, third_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    if (function_26f8f0(&third_markers[m]))
                    {
                        obstacle_flags |= 0x80;
                        blocking = true;
                        break;
                    }
                }
            }
            if (actor && actor->unknown26c != NONE && ((1 << type) & 3))
            {
                point3f center;
                function_b9dd0(object_index, &center);
                real dot = (center.z - position->z) * direction->k;
                dot += (center.x - position->x) * direction->i;
                dot += direction->j * (center.y - position->y);
                long vehicle_index = *(long *)(object + 0x248);
                if (dot > 0.0f && vehicle_index != NONE)
                {
                    short team = *(short *)(object + 0x138);
                    if (team == actor->unknown024 || !function_1df560(team, actor->unknown024))
                    {
                        byte *vehicle = (byte *)ai_object_get(vehicle_index);
                        long vehicle_actor_index = *(long *)(vehicle + 0x12c);
                        byte *vehicle_actor = NULL;
                        if (vehicle_actor_index != NONE)
                            vehicle_actor = (byte *)actor_get(vehicle_actor_index);
                        vector3f const *velocity = (vector3f const *)(object + 0x88);
                        if ((vehicle_actor_index != NONE && vehicle_actor[0x5d0] &&
                                *(real *)(vehicle_actor + 0x5f4) * direction->k +
                                *(real *)(vehicle_actor + 0x5f0) * direction->j +
                                direction->i * *(real *)(vehicle_actor + 0x5ec) > 0.0f) ||
                            velocity->k * direction->k + velocity->j * direction->j + direction->i * velocity->i > 0.0f)
                        {
                            obstacle_flags |= 0x200;
                            blocking = true;
                            inflation = 4.0f;
                        }
                    }
                }
            }
            if (motion && (flags & 0x40) && (motion->flags & 2))
                sphere_flags = 8;
        }
        long model_index = *(long *)(definition + 0x38);
        if (model_index == NONE)
            continue;
        long physics_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0xc);
        if (physics_index == NONE)
            continue;
        s_obstacle_physics_definition_2c *physics = (s_obstacle_physics_definition_2c *)g_4e3b44[physics_index & 0xffff].bytes;
        if (physics->count <= 0)
            continue;
        transform4x3f object_matrix;
        function_ba160(object_index, &object_matrix);
        for (short s = 0; s < physics->count; ++s)
        {
            s_obstacle_sphere_2c const *sphere = &physics->spheres[s];
            if (device_spheres && !(sphere->flags & 1))
                continue;
            if (restricted_spheres && !(sphere->flags & 4))
                continue;
            if (!local_4d3867 && (sphere->flags & 2))
                continue;
            point3f center;
            real local_a47c5e;
            if (sphere->node != NONE)
            {
                transform4x3f *matrix = function_b8bd0(object_index, sphere->node);
                transform4x3f_apply_point(matrix, &sphere->center, &center);
                local_a47c5e = sphere->radius * matrix->scale;
            }
            else
            {
                transform4x3f_apply_point(&object_matrix, &sphere->center, &center);
                local_a47c5e = sphere->radius * object_matrix.scale;
            }
            if (position->z > center.z + local_a47c5e + 1.0f && direction->k > -0.2f)
                continue;
            if (center.z - local_a47c5e - 1.0f > position->z && direction->k < 0.2f)
                continue;
            vector3f difference;
            difference.k = center.z - position->z;
            difference.j = center.y - position->y;
            difference.i = center.x - position->x;
            real squared = difference.k * difference.k * 4.0f;
            squared += difference.j * difference.j;
            squared += difference.i * difference.i;
            real combined_radius = local_a47c5e + radius;
            if (!(combined_radius * combined_radius >= squared))
                continue;
            long moving = 0;
            if (type == 0)
            {
                real dot = direction->j * difference.j + difference.k * direction->k + direction->i * difference.i;
                if (dot > 0.0f)
                {
                    vector3f const *velocity = (vector3f const *)(object + 0x88);
                    if (velocity->k * direction->k + velocity->j * direction->j + velocity->i * direction->i > 2.0f)
                        moving = 1;
                }
            }
            word combined_flags = (word)(moving | sphere_flags | obstacle_flags);
            if (blocking)
            {
                if (blocking_obstacles)
                    obstacle_list_add(blocking_obstacles, (word)obstacle_flags, (point2f const *)&center, object_index, local_a47c5e + inflation);
            }
            else
                obstacle_list_add(obstacles, combined_flags, (point2f const *)&center, object_index, local_a47c5e + inflation);
        }
    }
}

/* the first obstacle other than the one ignored whose circle, grown by the
   radius, holds the point */
// @retail 0x2c1c00
short obstacle_list_find_containing(s_obstacle_list const *list, short ignore_index, point2f const *point, real radius)
{
	short count = list->count;

	for (short i = 0; i < count; i++)
	{
		if (i != ignore_index)
		{
			s_obstacle const *obstacle = &list->obstacles[i];
			point2f offset;
			real distance;

			real distance_squared;

			distance = obstacle->radius + radius;
			vector2d_from_points2d(point, &obstacle->center, &offset);
			distance_squared = length_sq2f(&offset);
			if (distance_squared <= distance * distance)
			{
				return i;
			}
		}
	}
	return NONE;
}

/* the nearest obstacle, grown by the radius, that the ray from the origin
   hits within the maximum distance */
// @retail 0x2c1c70
bool obstacle_list_cast_ray(s_obstacle_list const *list, short ignore_index, point2f const *origin, point2f const *direction,
	real radius, real maximum_distance, bool ignore_flagged, s_obstacle_hit *hit)
{
	hit->distance = maximum_distance;
	hit->index = NONE;
	hit->group = NONE;
	for (short i = 0; i < list->count; i++)
	{
		if (i != ignore_index)
		{
			s_obstacle const *obstacle = &list->obstacles[i];

			if (!ignore_flagged || !(obstacle->flags & 1))
			{
				point2f offset;
				real along;
				real distance;

				vector2d_from_points2d(origin, &obstacle->center, &offset);
				distance = obstacle->radius + radius;
				along = dot_product2d(direction, &offset);
				if (along > 0.0f)
				{
					real outside = length_sq2f(&offset) - distance * distance;
					real hit_distance;

					if (outside <= 0.0f)
					{
						hit_distance = 0.0f;
					}
					else
					{
						real discriminant = along * along - outside;

						if (discriminant >= 0.0f)
						{
							hit_distance = (real)(along - sqrt(discriminant));
						}
						else
						{
							continue;
						}
					}
					if (hit->distance > hit_distance)
					{
						hit->distance = hit_distance;
						hit->index = i;
						hit->group = obstacle->group;
					}
				}
			}
		}
	}
	return hit->index != NONE;
}

/* marks the obstacles that overlap the first one, directly or through
   others, each grown by the radius */
// @retail 0x2c1e70
void obstacle_list_flood(short first_index, dword *bits, real radius, s_obstacle_list const *list)
{
	memset(bits, 0, ((list->count + 31) >> 5) * sizeof(dword));
	if (first_index != NONE)
	{
		short stack[64];
		short stack_count;

		bit_vector_set(bits, first_index);
		stack[0] = first_index;
		stack_count = 1;
		do
		{
			s_obstacle const *current = &list->obstacles[stack[--stack_count]];

			for (short i = 0; i < list->count; i++)
			{
				if (!bit_vector_test(bits, i))
				{
					point2f offset;
					s_obstacle const *other = &list->obstacles[i];
					real distance;

					real other_radius = other->radius;
					distance = (current->radius + radius) + (other_radius + radius);
					vector2d_from_points2d(&current->center, &other->center, &offset);
					distance *= distance;
					real length = offset.x * offset.x;
					length += offset.y * offset.y;
					if (length <= distance)
					{
						bit_vector_set(bits, i);
						stack[stack_count++] = i;
					}
				}
			}
		}
		while (stack_count > 0);
	}
}

/* numbers the groups of overlapping obstacles */
// @retail 0x2c1fb0
void obstacle_list_group(s_obstacle_list *list, real radius)
{
	short i;

	list->group_count = 0;
	for (i = 0; i < list->count; i++)
	{
		list->obstacles[i].group = NONE;
	}
	for (i = 0; i < list->count; i++)
	{
		if (list->obstacles[i].group == NONE)
		{
			dword bits[2];
			short group = list->group_count++;

			obstacle_list_flood(i, bits, radius, list);
			for (short j = 0; j < list->count; j++)
			{
				if (bit_vector_test(bits, j))
				{
					list->obstacles[j].group = group;
				}
			}
		}
	}
}

point2f const g_440b3c = { 0.0f, 1.0f };
point2f const *g_46877c = &g_440b3c;

PRIVATE __forceinline void scale_offset_direction_2c(real scale, point2f const *v, point2f *out)
{
    out->x = scale * v->x;
    out->y = v->y * scale;
}

PRIVATE __forceinline real normalize_offset_direction_2c(point2f *v)
{
    real magnitude = (real)sqrt(length_sq2f(v));
    if (fabs(magnitude) < 0.0001f)
        magnitude = 0.0f;
    else
        scale_offset_direction_2c(1.0f / magnitude, v, v);
    return magnitude;
}

// @retail 0x2c2910
void function_2c2910(point2f const *center, point2f const *points, point2f const *previous, real radius, point2f *result)
{
    struct s_candidate_x
    {
        static __forceinline real calculate(point2f const *center, point2f const *first,
            point2f const *second, real factor)
        {
            return center->x - (first->y - second->y) * factor;
        }
    };
    point2f first, second;
    vector2d_from_points2d(center, &points[0], &first);
    vector2d_from_points2d(center, &points[1], &second);
    real cross = first.x * second.y - second.x * first.y;
    if (!(fabs(cross) < 0.0001f))
    {
        real factor = radius * radius / cross;
        real y = center->y + (first.x - second.x) * factor;
        real x = s_candidate_x::calculate(center, &first, &second, factor);
        result->x = x;
        result->y = y;
        point2f delta;
        vector2d_from_points2d(center, result, &delta);
        if (!(length_sq2f(&delta) > radius * radius * 4.0f))
            return;
    }
    point2f direction;
    point2f normal;
    vector2d_from_points2d(previous, points, &direction);
    real magnitude = (real)sqrt(length_sq2f(&direction));
    if (fabs(magnitude) < 0.0001f)
        magnitude = 0.0f;
    else
        scale_offset_direction_2c(1.0f / magnitude, &direction, &normal);
    if (magnitude == 0.0f)
        normal = *g_46877c;
    result->x = points[0].x + normal.x * radius;
    result->y = points[0].y + normal.y * radius;
}

// @retail 0x2c2740
void function_2c2740(point2f const *point, point2f const *center, real radius, bool side, point2f *result)
{
    point2f direction;
    vector2d_from_points2d(center, point, &direction);
    real distance_squared = length_sq2f(&direction);
    real factor = distance_squared > 0.0f ? radius / distance_squared : 0.0f;
    point2f candidates[2];
    real tangent_squared = distance_squared - radius * radius;
    if (tangent_squared > 0.0f)
    {
        real tangent = (real)sqrt(tangent_squared);
        candidates[0].x = center->x + (direction.y * tangent + direction.x * radius) * factor;
        candidates[0].y = center->y + (direction.y * radius - direction.x * tangent) * factor;
        candidates[1].x = center->x + (direction.x * radius - direction.y * tangent) * factor;
        candidates[1].y = center->y + (direction.x * tangent + direction.y * radius) * factor;
        point2f first, second;
        vector2d_from_points2d(point, &candidates[0], &first);
        vector2d_from_points2d(point, &candidates[1], &second);
        long orientation = first.x * second.y - first.y * second.x > 0.0f ? 1 : 0;
        *result = candidates[orientation != side];
    }
    else
    {
        point2f offset;
        vector2d_from_points2d(center, point, &offset);
        real magnitude = (real)sqrt(length_sq2f(&offset));
        if (fabs(magnitude) < 0.0001f)
            magnitude = 0.0f;
        else
            scale_offset_direction_2c(1.0f / magnitude, &offset, &candidates[0]);
        if (magnitude == 0.0f)
            candidates[0] = *g_46877c;
        result->x = center->x + candidates[0].x * radius;
        result->y = center->y + candidates[0].y * radius;
    }
}

#include "unknown_1fa590.h"

PRIVATE __forceinline bool path_node_open_2c(s_pathfinding_data const *data, long index)
{
    if (index == NONE || index == 0xffff)
        return false;
    s_pathfinding_node const *node = &data->nodes[index];
    return (node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(node, data));
}

/* the same test of the node on one side of an edge, read as the edge's word */
PRIVATE __forceinline bool edge_node_open_2c(s_pathfinding_data const *data, s_pathfinding_edge const *edge,
    long side)
{
    word index = edge->nodes[side];
    if (index == NONE || index == (word)NONE)
        return false;
    s_pathfinding_node const *node = &data->nodes[index];
    return (node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(node, data));
}
// score 324 (was 324): frame 0x30 versus 0x28. This 324 (round 27) comes from the file's dot_product2d
// and the shared cross_product2d of vector2f_products.h, not from the reverted two-scalar copy: the
// zero is now made again before the compares as retail does (ours xmm7, retail xmm0), but ours still
// copies it for 0 - radius after the first offset where retail subtracts in place before both
// offsets, so ours keeps radius in xmm6 (retail xmm4) and spills direction.x and the first offset's y
// (the 8 extra frame bytes). Earlier: ours kept 0.0 in xmm6 for the whole loop. The edge pointer sits
// in ebx (retail esi) and the vertex in memory (retail ebx). Retail's crossing test is one byte set
// three times (`xor dl, dl` before the normalisation, `mov dl, 1` per test): the flag form
// `crossing = false; if (..) crossing = true` x3 reached 318 in earlier rounds (kept out: flag rule);
// on the 347 and 324 bodies of round 27 it is 371 (1331 bytes).
// Round 29 (lane A): the direction as two scalars (no address-taken struct) leaves it in registers
// as retail does: 347 -> 324, 1312 bytes. The frame is still 0x30 against retail's 0x28: ours keeps
// the normal's x at esp+0x18 and its y at esp+0x34 where retail has the y at 0x2c and the x at
// 0x34, and ours still spends a slot on the spilled direction.x.
// Review (coordinator): the two-scalar direction (324) is a hand copy of vector2d_from_points2d's body
// with no match, so the code stays at main's helper call (347); body copies are kept only with a match.
// Round 79 kept: the edge's node pair as a real array, the fixed-side tests on the edge's word
// (edge_node_open_2c: retail's `mov cx, word; movzx eax, cx; cmp cx, 0xffff`), the long test on
// the indexed ones, start/end vertices by if/else, complete geometry before the tests, the ring
// direction as `bool forward`, the portal loop stepping through the portal pointer with the source
// edge formed first, the previous vertex by if/else, no volatile on first_vertex and the walking
// edge in its own local (retail stores the edge index back to its own slot at entry) 347.
// Round 79 tried on 347/318: flag 318; offsets summed product-first/direct products/second difference
// first/both scales first 318/338/343/354; `|` of the crossing parts 371; the `||` crossing in one
// expression 350; vertex-select polarity flip no change; on 347: offset and difference interleaved
// 403, one offset reused 403, direction from the vertex indices before the start pointer 394.
// Data must stay in EBP: forms that move it let LTCG pass it in a register (ret 0x14).
// Earlier: frame 0x28 agrees; zero/radius schedule, history homes and indexed node loads remain.
// Earlier: Frame 0x28 and length agree; zero/radius schedule, history homes and indexed node loads differ.
// frame 0x28 agrees; zero/radius schedule, history homes and indexed node loads remain.
// Earlier: frame 0x30 versus 0x28; zero register, history homes and adjacent-node access differ.
// Ordinary edge with shared scaling 401; short surface 405; portal source at use 405; depth one 405; unsigned count 405.
// Earlier: ordinary previous/first/both history 399/401/400;
// word node index 408; alternate normalizer 401; indexed node pair 428;
// indexed pair with ordinary previous history 438. Frame and edge/history homes remain.
// Earlier: frame 0x30 versus 0x28; zero register, edge/history homes and indexed node loads differ.
// Frame 0x28 agrees; radius/zero schedule, edge/history homes and indexed node selection remain.
// Radius/zero schedule, edge/history homes and node selection remain.
// frame 0x30 versus 0x28; zero register, history homes and adjacent-node access differ.
// frame 0x28 and length agree; zero register, edge/history homes and indexed node loads differ.
// Earlier context: frame 0x30 versus 0x28; zero register, history homes and adjacent-node access differ.
// ordinary previous/first/both history 399/401/400;
// frame 0x30 versus 0x28; zero register, edge/history homes and indexed node loads differ.
// Earlier context: frame 0x30 versus 0x28; zero register, edge/history homes and indexed node loads differ.
// Reduced trials: direct dot predicate 401; shared dot product 401; immutable predicates 401; geometry
// names 401; output memcpy 401; earlier offset workspaces 401; native first-open 416; word selected vertex
// 404.
// j91: frame 0x30 versus 0x28; edge/history homes and indexed node selection differ; origin snapshots gain only one and grow frame to 0x38.
// first offset in place 401; second offset in place 401; both offsets in place 401; first offset origin
// snapshot 400; second offset origin snapshot 400
// Earlier: frame 0x38 versus 0x28; zero register, history homes and indexed node loads differ.
// frame 0x38 versus 0x28; zero register, history homes and indexed node loads differ.
// Earlier context: frame 0x38 versus 0x28; zero register, history homes and indexed node loads differ.
// Node fields at use 420; vertex words at use 414; endpoint values 432; normal assignments 401;
// split crossing phases 399; late second workspace 399; split phases/ordinary previous history 397;
// Boolean ring direction 399; vertex words/ordinary previous, first, both history 414,415,415;
// vertex words/Boolean ring direction 408. Retained code 1307/1312, checker extent 1312/1312.
// Earlier: frame 0x28 and length agree; zero register,
// edge/history homes and indexed node loads differ. Later first/second/both
// differences in place 397/397/397; one displacement 397; one radial offset
// 385; offset with shared displacement 385; phase scopes 397; offset/scopes 385.
// Complete geometry before predicates 397; with shared dot 397, symmetric cross parts 402,
// named projection 397, both named products 402, equivalent orientation 421, postincrement count 396,
// separate ring guards 397. Original single radial workspace and predicate phases restored.
// Native/promoted cached flags 385/401; explicit flag guards 414/409;
// node fields at use 384; native/promoted flags with receiver at use 384/383; flag-word single result 406.
// frame 0x28 and length agree; zero register,
// DL131: Radius/zero schedule, edge/history homes and node selection remain; result-first query
// 441; nested query result 448; overall single result 394; overall/query result 414; indexed nodes
// 430; indexed/result query 396; indexed/nested query 427; overall/indexed/query result 450.
// Original body, both private helpers and complete shared header restored.
// Predicate inline declarations 433; ordinary ring history 404; ring/incoming/both history names 385/385/385; declaration order 385; ordinary names/order 404/404; names plus order 385.
// Native helper side/promoted node/both 385/385/385; word/unsigned node input 409/385; byte/long openness result 417/410; native side and word node 409.
// Round 187: indexed node array plus Boolean side, native/promoted return and direct/cached flags
// all 462 with force-inline openness or 429 with ordinary-inline openness. All eight forms and shared type restored.
// Round 234: native current edge/node/portal-surface records and combinations
// 395/437/429/392/478/411/486; native portal query edges 392. Original receivers restored.
// Round 197: Cached selected/portal/both node words 442/385/442; selected/portal/both with Boolean ring direction 414/399/414; both words with word first-openness 410, plus Boolean ring 396. All restored; frame 0x28 remains.
// Round 255: complete geometric truth carriers byte/short/word/long 385/398/397/401;
// the same four with native byte side 385/398/397/401. Original Boolean results
// and side parameter restored; frame 0x28 and zero/radius schedule remain.
// Round 379: End/start/both endpoint addresses at use 385/462/451; traversal-lifetime edge alone/with end/start/both 385/385/462/451; complete receiver graph with node fields 421. Original body and helpers restored.
// Round 385: Uniform edge/surface/vertex table roots, singly and in pairs/all,
// 418/410/432/407/403/412/409; all roots per iteration 463. Original observations restored.
// Round 388: Shared scale/normalizer ordinary inline singly/both 385/385/385;
// scale noinline 425; normalizer noinline 451; noinline scale/ordinary normalizer 425;
// ordinary scale/noinline normalizer 451; both noinline 415. Original declarations restored.
// Round 172 DL: Complete first-node rejection ownership with bool/byte/signed-byte/
// short storage 427/427/427/413; the same four with native complete geometric
// vector storage 427/427/427/413. Boolean and byte forms reserve 0x24 versus
// retail 0x28; short restores extent but regresses. Original acceptance,
// types, helper calls, history qualifier and complete traversal restored.
// Round 175: Full-width unsigned incoming/ring/complete selected-vertex domains and all
// subsets 385 each; all with unsigned equality-only ring-edge anchor 385. Native word values
// and distinct NONE bit patterns preserved. All original domains, histories and body restored.
// Round 91: node pair indexed through the edge's real word pair 434 (data to eax, frame 0x2c);
// word index in the open test 409; portal loop stepping through the portal pointer, source edge
// before the side test, vertex selected by value 432 (data to ebx); first_vertex without volatile
// 404 (aligned diff 500 -> 461, convention kept); that with the indexed pair 433.
// Round 420: Incoming edge used as mutable working index, with iteration/traversal receiver and short/word bounded
// ring counts: 415 each; the same four with early ring anchor 361 each (1295 bytes). Original input copy,
// receiver/count and anchor lifetimes restored; frame remains 0x30 versus 0x28.
// Round 194: Shared point-setter normal/positive/negative producers and subsets 422/470/447/479/468/508/535; all plus successful output setters 513. All grow away from retail and restore the original helpers, geometry and complete traversal.
// Round 317: Complete negative crossing result with factored rejection, second-test order
// and sentinel priority, 369/369/413/413/411/411/411/411. Frames 0x30/0x38/0x28;
// sentinel-first recovers retail frame but regresses. Original body restored.
// Round 27 (m3 lane 15, fast): on 347, the two cross tests through cross_product2d alone 347 (same
// code), the dot test through dot_product2d alone 347, both together 324 (kept, plain 324), the dot
// through the vector2f dot_product2d as well 324; the flag form on 347/324 371/371 (1331 bytes);
// on 324: first_vertex declared before previous_vertex 322 (retail's entry store order); with it the
// normal by two assignments, normalize_local2d_2c, normalize2d_fast, the ring exits as two statements
// 322 each; a negative_radius local before the first offset 398, before the normal 424 (1281 bytes);
// the offsets written per coordinate with that local, origin first/product first 360/360 (1268
// bytes). Compile-only: unary minus in place of `0.0f -` gives identical code.
// Round 252: Common successful output across all three success paths: long/dword/word/native point at entry
// and after working-edge declaration 459/459/456/401/459/459/456/401. Original complete executable source
// restored; Frame 0x30 versus 0x28, zero/radius ownership, history homes and crossing allocation remain.
// Round 47 (m3 lane 1, fast; positional count, aligned diff tracked too: baseline 283): the -radius scale
// before the +radius one, each with its origin add, 353 (1281 bytes, aligned 234: frame 0x28, radius in XMM4
// and retail's in-place `0 - radius`); both scales first then the adds 351 (aligned 233), with the other
// offset's y add first 410; the adds as `offset.x += origin->x` 324 (same code); the offsets per coordinate
// as `normal.x * radius + origin->x` (and `* (0.0f - radius)`) 360 (1268 bytes, aligned 172: the whole
// normalisation/offset/difference block then matches retail), origin first 360, -radius pair first 368;
// on that per-coordinate form the crossing as three `crossing = true` sets 347 (1312 bytes, aligned 161) or
// as `first | second | (first_vertex == NONE)` 347 (1312 bytes, aligned 137). Retail evaluates all three
// crossing tests into one flag (DL), so the || form keeps it 44 bytes short.
// Round 121 (m3 lane 5, fast): the offsets per coordinate as the matched 0x2c2910/0x2c2740 write them
// (`origin->x + normal.x * radius`, `* (0.0f - radius)` for the other side, no negative-radius local)
// 360 (1268 bytes), but the whole float block is then retail's (frame 0x28, the zero in xmm0 consumed by
// `0 - radius`, radius xmm4, direction xmm7/xmm6, no spills); with the crossing as one flag set by three
// ifs 347 (1312), plus first_vertex declared first 345 (1312); per-coordinate offsets plus first_vertex
// first 358, plus the flag declared before the normal (retail's `xor dl, dl` inside the x87 length) 361
// (edge slot 0x18 as retail); that flag at the loop top 341, before the vertex loads 304 (frame 0x2c),
// before the start pointer 347; vertex by if/else 361; edge, vertex0/1 or vertex declared at the loop
// top or function scope 361 (same code). Left on the matched float block: ours re-reads vertex1 from
// the edge at the select where retail keeps it in EBX, so ours holds the widened side in ESI (retail
// spills it to the magnitude slot) and the edge pointer takes EBX (retail ESI). Also: direction renamed
// segment 324 (same code); edge declared at function scope 324; the portal search as a while with
// nested ifs and the step at the bottom 379 (1288).
// Round 123 (lane 6, fast): the offsets as direct products (`origin->x + normal.x * radius`, `* (0.0f - radius)`),
// as 0x2c2910/0x2c2740 write theirs, 360 (1268 bytes); the dot test's operands swapped 324 and both cross tests as
// `cross_product2d(direction, x) > 0.0f` 324 (same code); `first_vertex == NONE` first in the crossing `||` 348;
// the second offset's vector from the start computed first 349.
// @retail 0x2c3030
bool function_2c3030(s_pathfinding_data const *data, point2f const *origin, real radius,
    long edge_index, bool side, point2f *result)
{
    long previous_vertex = NONE;
    long first_vertex = NONE;
    long current_edge = edge_index;
    for (;;)
    {
        s_pathfinding_edge const *edge = &data->edges[current_edge];
        bool first_open = edge_node_open_2c(data, edge, 0);
        long node_index = edge->nodes[!first_open];
        if (!path_node_open_2c(data, node_index))
            return false;
        s_pathfinding_node const *node = &data->nodes[node_index];
        short surface_index = *(short const *)(edge->unknown04 + 2);
        if (previous_vertex != NONE && surface_index != NONE && data->surface_count > 0)
        {
            s_pathfinding_edge const *portal = &data->edges[data->surfaces[surface_index].index];
            long opposite = portal->nodes[portal->surface != node_index];
            if (path_node_open_2c(data, opposite))
            {
                *result = *(point2f const *)&data->vertices[previous_vertex];
                return true;
            }
        }
        long vertex0 = edge->vertices[0];
        long vertex1 = edge->vertices[1];
        long from_vertex, to_vertex;
        if (first_open)
        {
            from_vertex = vertex1;
            to_vertex = vertex0;
        }
        else
        {
            from_vertex = vertex0;
            to_vertex = vertex1;
        }
        point2f const *start = (point2f const *)&data->vertices[from_vertex];
        point2f const *end = (point2f const *)&data->vertices[to_vertex];
        point2f direction;
        vector2d_from_points2d(start, end, &direction);
        point2f normal = { direction.y, 0.0f - direction.x };
        normalize_offset_direction_2c(&normal);
        point2f offset, other_offset;
        scale_offset_direction_2c(radius, &normal, &offset);
        offset.x = origin->x + offset.x;
        offset.y = origin->y + offset.y;
        scale_offset_direction_2c(0.0f - radius, &normal, &other_offset);
        other_offset.x = origin->x + other_offset.x;
        other_offset.y = origin->y + other_offset.y;
        point2f first, second;
        vector2d_from_points2d(&offset, start, &first);
        vector2d_from_points2d(&other_offset, start, &second);
        bool first_crossing = (dot_product2d(&first, &direction) < 0.0f ? 1 : 0) == side &&
            cross_product2d((vector2f const *)&first, (vector2f const *)&direction) < 0.0f;
        bool second_crossing = cross_product2d((vector2f const *)&second, (vector2f const *)&direction) < 0.0f;
        bool crossing = first_crossing || second_crossing || first_vertex == NONE;
        long vertex = ((crossing != first_open) == side) ? vertex1 : vertex0;
        if (vertex == previous_vertex)
        {
            *result = *(point2f const *)&data->vertices[vertex];
            return true;
        }
        if (vertex == first_vertex)
            return false;
        if (first_vertex == NONE)
        {
            if (edge->unknown04[0] & 0x40)
            {
                *result = *(point2f const *)&data->vertices[vertex];
                return true;
            }
            first_vertex = vertex;
        }
        if (!(node->flags & 8))
        {
            s_pathfinding_surface const *portal;
            for (long surface = node->first_surface; surface != NONE && data->surface_count > 0;
                surface = portal->next)
            {
                portal = &data->surfaces[surface];
                if (portal->type == 0)
                {
                    s_pathfinding_edge const *source = &data->edges[portal->index];
                    if (((long const *)portal->unknown0c)[!side] != current_edge)
                        continue;
                    s_pathfinding_edge const *target = &data->edges[portal->object_index];
                    if (edge_node_open_2c(data, target, 0) || edge_node_open_2c(data, target, 1))
                    {
                        current_edge = portal->object_index;
                        if (side)
                            previous_vertex = source->vertices[0];
                        else
                            previous_vertex = source->vertices[1];
                        first_vertex = NONE;
                        goto next_edge;
                    }
                }
            }
        }
        {
            long first_edge = current_edge;
            short count = 0;
            for (;;)
            {
                bool forward = vertex == edge->vertices[1];
                if (path_node_open_2c(data, edge->nodes[!forward]) == side)
                    break;
                current_edge = edge->next_edges[!forward];
                if (current_edge == 0xffff)
                    return false;
                edge = &data->edges[current_edge];
                if (current_edge == first_edge || ++count > 50)
                    return false;
            }
        }
        previous_vertex = vertex;
next_edge:;
    }
}

__declspec(noinline) real normalize2d(point2f *v);

real function_11cc90(vector2f const *a, vector2f const *b);

/* true when the first corner is the better way round from the origin to the
   endpoint (always when the two coincide): by the angles turned when both
   corners lie on one side of the line, by the distance through each otherwise */
// @retail 0x2c2a70
bool function_2c2a70(point2f const *origin, point2f const *endpoint,
    point2f const *first, point2f const *second, point2f const *target)
{
    point2f direction;
    vector2d_from_points2d(origin, endpoint, &direction);
    bool result;
    if (normalize_offset_direction_2c(&direction) <= 0.0f)
        result = true;
    else
    {
        point2f normal = { 0.0f - direction.y, direction.x };
        point2f first_direction, second_direction;
        vector2d_from_points2d(first, origin, &first_direction);
        normalize_offset_direction_2c(&first_direction);
        vector2d_from_points2d(second, origin, &second_direction);
        normalize_offset_direction_2c(&second_direction);
        real second_side = dot_product2d(&second_direction, &normal);
        real first_side = dot_product2d(&first_direction, &normal);
        if (fabs(second_side) < 0.001)
            second_side = 0.0f;
        if (fabs(first_side) < 0.001)
            first_side = 0.0f;
        if (first_side * second_side >= 0.0f)
        {
            point2f target_first, target_second, endpoint_first, endpoint_second;
            vector2d_from_points2d(first, target, &target_first);
            normalize2d(&target_first);
            vector2d_from_points2d(second, target, &target_second);
            normalize2d(&target_second);
            vector2d_from_points2d(first, endpoint, &endpoint_first);
            normalize2d(&endpoint_first);
            vector2d_from_points2d(second, endpoint, &endpoint_second);
            normalize2d(&endpoint_second);
            real first_target_angle = function_11cc90((vector2f const *)&target_first, (vector2f const *)&first_direction);
            real first_endpoint_angle = function_11cc90((vector2f const *)&first_direction, (vector2f const *)&endpoint_first);
            real second_target_angle = function_11cc90((vector2f const *)&target_second, (vector2f const *)&second_direction);
            real second_endpoint_angle = function_11cc90((vector2f const *)&second_direction, (vector2f const *)&endpoint_second);
            if (second_endpoint_angle > 0.0f)
                second_endpoint_angle -= 6.2831854820251465f;
            if (first_endpoint_angle < 0.0f)
                first_endpoint_angle += 6.2831854820251465f;
            result = first_target_angle + first_endpoint_angle < 0.0f - (second_target_angle + second_endpoint_angle);
        }
        else
        {
            point2f target_first, endpoint_first, target_second, endpoint_second;
            vector2d_from_points2d(target, first, &target_first);
            vector2d_from_points2d(endpoint, first, &endpoint_first);
            vector2d_from_points2d(target, second, &target_second);
            vector2d_from_points2d(endpoint, second, &endpoint_second);
            real first_distance = (real)sqrt(length_sq2f(&target_first)) + (real)sqrt(length_sq2f(&endpoint_first));
            real second_distance = (real)sqrt(length_sq2f(&target_second)) + (real)sqrt(length_sq2f(&endpoint_second));
            result = first_distance < second_distance;
        }
    }
    return result;
}

#include "globals.h"
#include "path.h"
#include "unknown_20fe20.h"
#include "unknown_26c380.h"

struct s_path_step_view
{
    short type;
    short unknown02;
    long node_index;
    short link_index;
    short unknown0a;
    s_type_c3b527 point;
};
struct s_avoidance_trace
{
    real distance;
    long sector;
    long edge;
    short obstacle;
    short group;
};
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
    short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
    real radius, real distance, bool first, bool stop_at_goal, bool ignore_flagged,
    s_path_location const *location, s_avoidance_trace *trace);
bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long sector_index,
    s_type_c3b527 const *end, long target_sector_index, long location, s_path_trace_result *trace);
// score 444 (was 444): frame 0x11c versus 0x10c, 1740 bytes against 1746. Ours copies the whole step
// (`rep movsd`) where retail reads it through a pointer and keeps only its type and link in stack slots;
// retail places the count <= 1 arm at the end, keeps 0 in EBX for the whole function, compares the
// loop indexes against the count on the stack (ours holds it in bx) and forms the output element at
// the emit point. The distance test is retail's inlined expansion (one shared (z*z + y*y) + x*x sum),
// not 0x210a30, whose two branches sum differently.
// Frame 0x11c versus 0x10c; radius, zero and metadata/endpoint homes still differ.
// Earlier at 453: Frame 0x10c and exact extent agree; radius, persistent zero and metadata/endpoint homes remain.
// Earlier: Frame 0x10c and 1746-byte extent agree; source-radius, metadata/index and endpoint homes remain.
// Earlier unsigned outer word 493; default data result 478; expanded-radius name 478; native origin fields 493; early
// Earlier 469 (was 469): Frame 0x104 versus 0x10c, radius/metadata/index homes still differ.
// Earlier at 469: Frame 0x104 versus 0x10c, radius/metadata/index homes still differ.
// Earlier: Frame 0x104 versus 0x10c and radius/metadata homes remain. Unsigned outer
// Frame 0x104 versus 0x10c and radius/metadata homes remain. Unsigned outer
// word 493; default data result 478; expanded-radius name 478; native origin fields 493; early
// output slot 469; with unsigned outer 470 (frame 0x10c); unsigned next 493.
// frame 0x104 versus 0x10c; radius/index and metadata homes remain.
// frame 0x104 versus 0x10c; metadata result homes recover 0x10c at 482 but radius, index and branch allocation still differ.
// metadata common emission 482; metadata long type 482; metadata long link 483; metadata both long 482;
// metadata outer index 482; metadata radius home 482; metadata ordinary output 513; metadata index register
// 522
// frame 0x104 versus 0x10c; radius and emitted-index homes differ.
// Short outer index with original qualifiers 513; broader trace outputs 478; native copied-last 504;
// native found-step 494; current-step fallback first 478.
// Round 310: terminal fallback return 526; double rounded radius 489; long next 519;
// unsigned edge 478; shared point setters 478. Double radius recovers 0x10c but worsens scalar homes.
// frame 0x104 versus 0x10c and radius/index homes differ; trace continues 478;
// unsigned output word 478; byte attempt 495; distinct trace-origin name 478; outer world endpoints
// 478.
// Trace endpoint names 478; first trace at entry 478; target at entry 478;
// outer candidate point 478; complete path-record memcpy 478.
// direct entry 489; direct output 471.
// native source 470; entry/output 483.
// entry/source 485; output/source 477.
// all three accesses 475; direct endpoints 469.
// Target Z once/shared endpoint zero initialization 470/470; one sector-trace output 466; avoidance at entry/iteration 469/469; sector pair before attempts 469; outer direction 469; outer tangents 469.
// Round 318, early-output-pointer body: signed-word outer 485; unsigned/native output count
// 469/522; native attempt 470; native end/candidate/both 500/472/516;
// word outer with native attempt 486. All bounded domains and trace inputs retained; all restored.
// Round 129: common corner phase 469; with corner-data/location/both receivers 476/460/464;
// with next-entry receiver 453 kept plain, plus corner-data/location/both 452/470/468
// (one-point extra rejected). Nested next receiver 460; role-based radius names 453; earlier
// source address 456; plus names 456; consistent source receiver with/without names 456/456.
// Round 132 on the shared next-entry graph: entry fallback/used/both zero lifetimes 453/453/453;
// native word outer 490, with entry fallback/used/both 485/491/491; dword fallback state 448;
// with entry fallback/used/both 451/453/451. Five-point reduction rejected above 30.
// Round 187 selected/emitted point receivers and both 469/472/530; existing world-point
// conversion helper alone/selected/emitted/both 515/526/492/505; full form with
// actual selected-element reference 505. All original receivers and conversions restored.
// Existing tangent/candidate/corner inline declarations, their pairs/all, and all
// three noinline declarations 453 each. Original declarations and body restored.
// Current/selected/both native record values 461/464/469; with word outer index
// 444/461/454; ordinary output index with current/selected value 462/461. Current
// record value and word outer index 444 plain kept, 1740/1746 bytes; other forms restored.
// frame 0x10c and exact extent agree; radius, persistent zero and
// metadata/endpoint homes remain. Retail places the count <= 1 arm at the end, keeps 0 in EBX for
// the whole function, compares a 16-bit loop index and forms the output element at the emit point.
// Earlier (frame 0x104 era): unsigned outer word 493; default data result 478; expanded-radius name
// 478; native origin fields 493; early output slot 469; with unsigned outer 470 (frame 0x10c);
// unsigned next 493; metadata common emission/long type/long link/both long/outer index/radius home
// 482 each; metadata ordinary output 513; metadata index register 522; short outer index with
// original qualifiers 513; broader trace outputs 478; native copied-last 504; native found-step 494;
// current-step fallback first 478. Round 310: terminal fallback return 526; double rounded radius
// 489; long next 519; unsigned edge 478; shared point setters 478 (double radius recovers 0x10c but
// worsens scalar homes); trace continues 478; unsigned output word 478; byte attempt 495; distinct
// trace-origin name 478; outer world endpoints 478; trace endpoint names 478; first trace at entry
// 478; target at entry 478; outer candidate point 478; complete path-record memcpy 478; direct
// entry 489; direct output 471; native source 470; entry/output 483; entry/source 485; output/source
// 477; all three accesses 475; direct endpoints 469; target Z once/shared endpoint zero
// initialization 470/470; one sector-trace output 466; avoidance at entry/iteration 469/469; sector
// pair before attempts 469; outer direction 469; outer tangents 469. Round 318 (early-output-pointer
// body): signed-word outer 485; unsigned/native output count 469/522; native attempt 470; native
// end/candidate/both 500/472/516; word outer with native attempt 486. Round 129: common corner phase
// 469; with corner-data/location/both receivers 476/460/464; with next-entry receiver 453 kept plain,
// plus corner-data/location/both 452/470/468; nested next receiver 460; role-based radius names 453;
// earlier source address 456; plus names 456; consistent source receiver with/without names
// 456/456. Round 132 on the shared next-entry graph: entry fallback/used/both zero lifetimes
// 453/453/453; native word outer 490, with entry fallback/used/both 485/491/491; dword fallback state
// 448; with entry fallback/used/both 451/453/451.
// Round 93 (fast base 453): `count > 1` arm first 466 (retail's branch placement); with a short loop
// index 475; with the source pointer formed before the loop 466; all three 475; output element
// formed at the emit point 518 (1692 bytes; aligned differences 604 -> 532).
// Round 62: the distance test through 0x210a30 730 (called, not inlined: 1478 bytes); with a
// forceinline declaration here 730; with `inline` on its definition 541 (inlined, but 0x1f45c0 then
// inlines it too and stops matching; 0x210850 stays called); data as default plus if 444; the step
// as a const reference 490 (aligned 443 -> 342, query in ecx and frame 0x104 as retail's entry); plus
// the count > 1 arm first 475 (aligned 339); plus the output element at the emit point 506 (aligned
// 318); plus a plain output index 551 (aligned 316). The reference form is the lead: it needs retail's
// saved type/link slots and the stack-compared count to recover the length.
// Round 227: Mutable current record, iteration assignment/CRT, traversal assignment/CRT,
// immutable current-step home and traversal current-step assignment/CRT 444/444/444/444/444/444/444/444. All restored.
// Final source/emitted/both native coordinate values 476/484/557;
// with existing point difference 492/484/557; both values emitted first
// without/with point difference 557/557. All restored; original current record retained.
// Round 194: native output-count/copied-result/found-result widths 505/468/476;
// both result words 479; wide count with copied/found/both 492/478/473;
// all native words with found from its bound test 500. Original carriers restored.
// Lane 9 r25: retail keeps two NONE-initialised shorts at +0x6c/+0x74 (set to the step's type and link
// when its type is not NONE, tested at the emit, which reads the type itself through the step pointer).
// Step pointer plus those two locals 461 (frame 0x10c as retail's); plus the count > 1 arm first 483;
// plus the output element at the emit point 505; used/copied-last at function scope 476; the two
// locals on the copied step 493. Retail's zero in EBX from the data test is still missing.
// Round 10 (lane 10): retail reads the step through a pointer (it loads the step's unknown02 after
// storing the output element) and keeps type/link values, NONE unless the step has a type, in two
// stack slots for the emit test. Step pointer with type/link locals set in the typed-step branch 463
// (frame 0x10c as retail, 1752 bytes); with the emit copying the type through the pointer 461 (1749
// bytes); plus the output count declared at the top 473; plus the count > 1 arm first 478; count at
// the top alone 444 (same code); plus the output element formed at the emit point 509/505 (with arm).
// Retail's EBX is a zero register from entry to the loop (data tests, index/count/flag stores).
// Round 234: entry-snapshot/displacement/conversion scope endings and combinations
// 442/444/442/444/442/444/442; tangent consumption scope 444. Two-point gains rejected; original body restored.
// Round 255: entry ordinal/count/fallback zero lifetimes 444, 444, 444;
// ordinal-count/ordinal-fallback/count-fallback/all 444, 471, 471, 472;
// all with byte fallback 472. Original lifetimes restored; frame remains 0x11c.
// Round 379: Data/single-step/both terminal exits 484/444/484; data-first acquisition alone/with data/single/both exits 470/478/470/478; traversal-only radius with both exits 485. Original body restored.
// Round 165: Early actual data-result NULL lifetime coupled with source-radius/output-
// index/traversal store subsets 469/486/491/466/471/487/496; combined existing endpoint setters
// 496. Original data producer, both inherited local qualifiers, arm order and endpoint
// construction restored. Frame 0x11c versus 0x10c, persistent zero and metadata/radius homes
// remain.
// Round 385: Natural tangent-search attempt result, short/word/long/dword
// at search eligibility 445/445/447/447, at iteration entry 445/445/447/447. Original jump restored.
// Round 388: Both corner-success carriers byte/short/word/long 444/440/440/450,
// with the same domains declared at selection 444/440/440/450. Four-point gains
// above 30 rejected; original Boolean carriers and declarations restored.
// Round 172 DL: Selected successor in the ordinary for increment 444; its
// entry declaration 444; ordinary output-index/radius/both storage 486/469/491;
// paired word corner results 440; current-record lifetime ending at metadata
// publication 494, with paired word results 485. The four-count gain fails
// the eight-count gate. Original successor lifetime, both qualifiers,
// Boolean corner results, snapshot and full body restored.
// Round 175: Existing selector inline/noinline declarations 444/444; output receiver formed
// after successful selection alone/inline/noinline 502/502/502; the same three with the immutable
// current type retained across all uses 502/502/502. All original declarations, address and
// metadata lifetimes, native record snapshot, qualifiers and complete body restored.
// Round 37: native member-produced record, const/mutable and before/after the pure output address,
// 494/493/494/493; complete coordinate member producers 469/464/469/464. Query/radius homes recover,
// but the 0x11c frame remains; original complete record copy and body restored.
// Round 79: candidate direction by direct initialiser (retail forwards y, `fst`) 458; step through a
// pointer with type/link in two short locals and the output element at the emit 509 (1686 bytes,
// frame 0x104, aligned unmatched 442 -> 317); both 517; no output_index volatile (retail: di) 486,
// with the step pointer 541 (aligned 308); no source-radius volatile 469/498; source pointer before
// the loop identical code.
// Round 301: Full count / named terminal ordinal / both 496/474/500;
// with paired word corner results 490/472/491; complete bound graph
// with ordinary radius, without/with word corners 500/491. Original count,
// ordinal expressions, Boolean corner carriers, radius qualification and body restored.
// Round 229: attempt-owned tangent orientation with complement passed to the second call and
// advance after unsuccessful attempts; bool/byte/word/long with indexed centers 446/459/444/448;
// branch-selected centers 446/451/442/439. Original orientation phases and full body restored.
// Coupled paired word/short corner results on long orientation 444/444; word orientation
// 439/439. Best 439 is a five-point gain above 30, rejected; complete original body restored.
// Round 45: complete four-field scalar input snapshots, short/long metadata
// subsets 0/1/2/4/3/5/6/7, 459/461/470/470/461/461/470/461. Frame 0x10c
// recovered, radius/zero homes still differ; match-only forms and full body restored.
// Round 35 (lane 11): the file's direction normalizer now passes `1.0f / magnitude` straight to the
// scale helper as 0x2c2910/0x2c2740 do (that matched 0x2c2a70); here it gives an in-place y product
// (444 -> 458), so this body calls the file's other normalizer, normalize_local2d_2c (444, identical code).
// Also (fast): normalize2d_fast 444 (same code); out-of-line normalize2d 562; step through a pointer with
// short type/link locals set for a typed step, type re-read through it at the emit, output element at
// the emit 509 (frame 0x104, 1686 bytes); that with a long type local 515.
// Round 27 (m3 lane 15, plain): both point differences through vector3d_from_points3d with length_sq3f
// 501 (1707 bytes), the differences alone 501; delta as a vector3f with length_sq3f 444 (same code); no
// output_index volatile 486, no source_radius volatile 469 (round 79's numbers after this round's
// 0x2c3030/0x2c0d60 changes).
// Round 121 (m3 lane 5, fast = full here): retail sets next/edge/found together at the loop top, at
// the scan's typed step (a second `mov ebx, 0xffff`) and at a candidate's success. `edge = 0xffff`
// also in the scan's `end < count` block 471 (gives that second store); edge assigned only where next
// is chosen, no initialiser 469 (1757 bytes); the one-step arm with its own `return` 444 (same code);
// the copy-last block after the loop through two gotos 444 (same code: its placement is not source
// order); the next-step pointer only inside the corner branch, the fallback indexing points[next] 478.
// @retail 0x2c2060
void __stdcall function_2c2060(s_type_f17a25 *query, short count, s_path_step_view const *points,
    short *result_count, s_path_step_view *result, bool *complete)
{
    volatile real source_radius = query->source.radius;
    real radius = (real)(source_radius + 0.05);
    s_pathfinding_data *data = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ?
        *(s_pathfinding_data **)((byte *)g_4e0348 + 0xc8) : 0;
    if (data)
    {
        if (count <= 1)
        {
            *result_count = 1;
            result[0] = points[0];
        }
        else
        {
            point2f origin = *(point2f *)&query->source.point.point;
            long sector = points[0].node_index;
            short used = 0;
            bool copied_last = false;
            for (short index = 0; index < count; )
            {
                s_path_step_view const entry = points[index];
                s_path_step_view *out = &result[used];
                short next = NONE;
                long edge = 0xffff;
                bool found = false;
                volatile short output_index;
                if (entry.type != NONE)
                    next = index;
                else
                {
                    short end = index;
                    while (end < count && points[end].type == NONE)
                        ++end;
                    if (end < count)
                    {
                        next = end;
                        found = true;
                    }
                    for (short candidate = end - 1; candidate >= index; --candidate)
                    {
                        point2f direction;
                        vector2d_from_points2d(&origin, (point2f *)&points[candidate].point.point, &direction);
                        real distance = normalize_local2d_2c(&direction);
                        if (!(distance > 0.0f))
                            break;
                        s_avoidance_trace trace;
                        if (!function_26ccb0(data, 0, NONE, &origin, sector, points[candidate].node_index,
                            &direction, radius, distance, false, candidate == count - 1, false, &query->location, &trace))
                            break;
                        edge = trace.edge;
                        next = candidate;
                        found = true;
                    }
                    if (!found)
                    {
                        result[used++] = points[count - 1];
                        copied_last = true;
                        break;
                    }
                }
                s_path_step_view const *next_step = &points[next];
                if (edge != 0xffff && next_step->type == NONE)
                {
                    point2f corners[2];
                    bool left = function_2c3030((s_pathfinding_data *)query->pathfinding,
                        &origin, radius, edge, true, &corners[0]);
                    bool right = function_2c3030((s_pathfinding_data *)query->pathfinding,
                        &origin, radius, edge, false, &corners[1]);
                    if (right && left)
                    {
                        point2f const *previous = next > 0 ? (point2f *)&points[next - 1].point.point :
                            (point2f *)&query->source.point.point;
                        point2f const *endpoint = (point2f *)&next_step->point.point;
                        bool side = function_2c2a70(previous, endpoint, &corners[0], &corners[1], &origin);
                        point3f start = { origin.x, origin.y, 0.0f };
                        for (short attempt = 0; attempt < 2; ++attempt)
                        {
                            point2f center = corners[side ? 0 : 1];
                            point2f tangents[2], candidate;
                            function_2c2740(&origin, &center, radius, side, &tangents[0]);
                            side = !side;
                            function_2c2740(endpoint, &center, radius, side, &tangents[1]);
                            function_2c2910(&center, tangents, &origin, radius, &candidate);
                            point3f target = { candidate.x, candidate.y, 0.0f };
                            s_path_trace_result first, second;
                            function_26c4e0(data, (s_type_c3b527 *)&start, sector, (s_type_c3b527 *)&target,
                                NONE, (long)&query->location, &first);
                            if (!((s_sector_trace_result *)&first)->blocked &&
                                !function_26c4e0(data, (s_type_c3b527 *)&target, ((s_sector_trace_result *)&first)->sector_index,
                                    &next_step->point, next_step->node_index, (long)&query->location, &second))
                            {
                                sector = ((s_sector_trace_result *)&first)->sector_index;
                                origin = candidate;
                                output_index = next_step->point.output_index;
                                goto emit;
                            }
                        }
                    }
                }
                if (next != 0 && next != index)
                {
                    origin = *(point2f *)&points[next - 1].point.point;
                    sector = points[next - 1].node_index;
                    output_index = points[next - 1].point.output_index;
                }
                else
                {
                    origin = *(point2f *)&next_step->point.point;
                    sector = next_step->node_index;
                    output_index = next_step->point.output_index;
                    next = index + 1;
                }
            emit:
                out->node_index = sector;
                out->point.point.x = origin.x;
                out->point.point.y = origin.y;
                out->point.point.z = entry.point.point.z;
                out->point.output_index = output_index;
                out->unknown02 = entry.unknown02;
                ++used;
                if (entry.type != NONE)
                {
                    out->type = entry.type;
                    out->link_index = entry.link_index;
                }
                else
                {
                    out->type = NONE;
                    out->link_index = NONE;
                }
                if (used >= 4)
                    break;
                point3f delta;
                s_type_c3b527 const *source = (s_type_c3b527 *)&query->source.point;
                if (source->output_index == out->point.output_index)
                {
                    delta.x = out->point.point.x - source->point.x;
                    delta.y = out->point.point.y - source->point.y;
                    delta.z = out->point.point.z - source->point.z;
                }
                else
                {
                    point3f start, end;
                    if (source->output_index == NONE || !function_2104b0(source->output_index, &source->point, &start))
                        start = source->point;
                    if (out->point.output_index == NONE || !function_2104b0(out->point.output_index, &out->point.point, &end))
                        end = out->point.point;
                    delta.x = end.x - start.x;
                    delta.y = end.y - start.y;
                    delta.z = end.z - start.z;
                }
                if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x > 400.0f)
                    break;
                index = next;
            }
            *result_count = used;
            if (!copied_last)
                *complete = false;
        }
    }
    else
    {
        *result_count = 0;
        *complete = false;
    }
}
