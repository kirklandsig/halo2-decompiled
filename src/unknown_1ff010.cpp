#include <string.h>
// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_1e3920.h"
#include "unknown_2551c0.h"
#include "object_markers.h"
#include "effects.h"

/* the actor's props as targets */

void *function_1e5380(long actor_index);
void *function_1e5280(long actor_index, long key);
bool function_25d970(s_prop_datum *datum);

bool weapon_barrel_aim(long weapon_index, short barrel_index, point3f const *origin, point3f const *target,
	real const *unknown3, bool unknown5, vector3f *direction, real *time, real *distance, real *speed_out, bool *linear);

struct s_weapon_aim_ranges
{
	byte unknown00[0x38];
	real minimum_distance;
	real maximum_distance;
	real minimum_fraction;
	real maximum_fraction;
};

/* a weapon barrel definition (0xec bytes), as the aiming code reads it */
struct s_barrel_aim_view
{
	byte unknown00[0x90];
	long projectile_index;
	byte unknown94[0xec - 0x94];
};

/* a projectile definition, as the aiming code reads it */
struct s_projectile_aim_view
{
	byte unknown00[0xbc];
	dword flag0 : 1;
	dword flag1 : 1;
	dword unknownbc : 30;
	byte unknownc0[0x164 - 0xc0];
	real unknown164;
};

/* aims the weapon's barrel at the target for the actor: for a projectile barrel at a fraction
   of the actor's ranges by distance, otherwise (with no actor field 0x26c) from the actor's
   position at the origin's height */
// @retail 0x1ffbd0
bool function_1ffbd0(long actor_index, long weapon_index, short barrel_index,
	point3f const *origin, point3f const *target, bool flag, vector3f *direction,
	void *unknown0, void *unknown1, void *unknown2, void *unknown3)
{
	long definition_index = moving_object_get(weapon_index)->tag_index;
	s_weapon_aim_ranges *ranges = (s_weapon_aim_ranges *)function_1e5280(actor_index, definition_index);
	bool result = false;
	if (ranges)
	{
		real fraction = 0.0f;
		bool use_fraction = false;
		byte *definition = g_4e3b44[definition_index & 0xffff].bytes;
		if (barrel_index >= 0 && barrel_index < *(long *)(definition + 0x2d0))
		{
			s_barrel_aim_view *barrel = &(*(s_barrel_aim_view **)(definition + 0x2d4))[barrel_index];
			s_actor_view *actor = actor_get(actor_index);
			if (barrel->projectile_index != NONE &&
				TEST_FIELD_BIT(TAG_GET(s_projectile_aim_view, barrel->projectile_index)->flag1) &&
				TAG_GET(s_projectile_aim_view, barrel->projectile_index)->unknown164 > 0.0f)
			{
				real distance = distance3d(origin, target);
				real upper = 1.0f - (0.0f > ranges->maximum_fraction ? 0.0f : ranges->maximum_fraction > 1.0f ? 1.0f : ranges->maximum_fraction);
				real lower = 1.0f - (0.0f > ranges->minimum_fraction ? 0.0f : ranges->minimum_fraction > 1.0f ? 1.0f : ranges->minimum_fraction);
				if (distance >= ranges->maximum_distance)
					fraction = upper;
				else if (ranges->minimum_distance >= distance)
					fraction = lower;
				else
					fraction = (distance - ranges->minimum_distance) * (upper - lower) /
						(ranges->maximum_distance - ranges->minimum_distance) + lower;
				use_fraction = true;
			}
			else if (actor->unknown26c == NONE)
			{
				point3f adjusted_origin = actor->position;
				adjusted_origin.z = origin->z;
				return weapon_barrel_aim(weapon_index, barrel_index, &adjusted_origin, target, NULL, flag,
					direction, (real *)unknown0, (real *)unknown1, (real *)unknown2, (bool *)unknown3);
			}
			result = weapon_barrel_aim(weapon_index, barrel_index, origin, target, use_fraction ? &fraction : NULL, flag,
				direction, (real *)unknown0, (real *)unknown1, (real *)unknown2, (bool *)unknown3);
		}
	}
	return result;
}

real function_fa170(long definition_index);
bool function_fa6a0(long definition_index, real const *speed_override, point3f const *origin, point3f const *target,
	real *unknown2, real const *unknown3, real const *unknown4, bool unknown5, vector3f *direction, real *speed_out,
	real *time, real *distance, bool *linear);

struct s_projectile_choice
{
	byte unknown00[0x28];
	long definition_index;
};

struct s_projectile_choices
{
	byte unknown00[0x100];
	long count;
	s_projectile_choice *choices;
};

PRIVATE inline s_projectile_choice *projectile_choice_get(short type)
{
	s_projectile_choice *result;
	if (((s_projectile_choices *)g_4e034c)->count)
		result = &((s_projectile_choices *)g_4e034c)->choices[type];
	else
		result = NULL;
	return result;
}

/* the choice's definition, or NONE without a choice */
PRIVATE inline long projectile_choice_definition(s_projectile_choice const *choice)
{
	long definition_index = NONE;
	if (choice && choice->definition_index != NONE)
		definition_index = choice->definition_index;
	return definition_index;
}

/* aims a projectile of the choice at the target, again just above its adjusted speed
   when the first try fails; the velocity and gravity are optional outputs */
// @retail 0x1ff200
bool function_1ff200(short type, point3f const *origin, real speed, point3f const *target,
	real const *limit, bool alternate, vector3f *direction, real *speed_out, real *time,
	vector3f *velocity, real *gravity)
{
	bool result = false;
	s_projectile_choice *choice = projectile_choice_get(type);
	long definition_index = projectile_choice_definition(choice);
	if (definition_index != NONE)
	{
		real adjusted_speed;
		bool linear = false;
		if (function_fa6a0(definition_index, &speed, origin, target, &adjusted_speed, limit, NULL,
			alternate, direction, speed_out, time, NULL, &linear) ||
			(adjusted_speed > 0.0f && speed > adjusted_speed &&
			function_fa6a0(definition_index, &(adjusted_speed += 0.01f), origin, target, NULL, limit, NULL,
			alternate, direction, speed_out, time, NULL, &linear)))
		{
			result = true;
			if (velocity)
			{
				real scale = *speed_out;
				velocity->i = direction->i * scale;
				velocity->j = direction->j * scale;
				velocity->k = direction->k * scale;
			}
			if (gravity)
				*gravity = linear ? 0.0f : function_fa170(definition_index);
		}
	}
	return result;
}

/* the block function_1e5380 returns */
struct s_character_variant_ranges
{
	byte unknown00[0x18];
	real minimum_distance;
	real maximum_distance;
};

/* the prop's state as these functions read it */
struct s_prop_state_1ff
{
	byte unknown00[0x3c];
	long unknown3c;
	byte unknown40[0x5e - 0x40];
	bool unknown5e;
};

/* the elements of g_50241c (0xc4 bytes) */
struct s_50241c_element_1ff
{
	byte unknown00[0x23];
	bool unknown23;
	byte unknown24[0xc4 - 0x24];
};

/* whether the point is in the range the actor's variant fights at, and the
   prop's target there */
// @retail 0x1ff6c0
bool function_1ff6c0(long actor_index, long prop_index, point3f const *point, long *target)
{
	/* retail squares i, then k, then j here */
	struct distance_1ff6c0
	{
		static inline vector3f *sub(point3f const *a, point3f const *b, vector3f *result)
		{
			result->i = b->x - a->x;
			result->j = b->y - a->y;
			result->k = b->z - a->z;
			return result;
		}
		static inline real root(real x)
		{
			return (real)sqrt(x);
		}
		static inline real get(point3f const *a, point3f const *b)
		{
			vector3f v;
			sub(a, b, &v);
			return root(v.j*v.j + v.k*v.k + v.i*v.i);
		}
	};
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	s_character_variant_ranges *ranges = (s_character_variant_ranges *)function_1e5380(actor_index);

	if (ranges)
	{
		real distance = distance_1ff6c0::get(&actor->position, point);

		if (distance > ranges->minimum_distance && ranges->maximum_distance > distance)
		{
			if (prop_index != NONE)
			{
				s_prop_node_view *node = prop_node_get(prop_index);
				s_prop_state_1ff *state = (s_prop_state_1ff *)function_25d690((s_prop_datum *)node);
				s_50241c_element_1ff *element = (s_50241c_element_1ff *)(g_50241c->data + (node->unknown08 & 0xffff) * sizeof(s_50241c_element_1ff));

				if (element->unknown23 && !state->unknown5e &&
					((node->unknown24 >= 1 && node->unknown24 <= 2) || function_25d970((s_prop_datum *)node)))
				{
					*target = state->unknown3c;
					result = true;
				}
			}
			else
			{
				result = true;
			}
		}
	}
	return result;
}


/* Shared request layout used by the point/radius actor iterator. */
struct s_actor_point_request
{
	byte field_0[0x14];
	s_record_pool *pool;
	long iterator_index;
	long datum_index;
	long group_index;
	long actor_index;
	long next_actor_index;
	short type;
	short mode;
	real radius;
	real radius_squared;
	long index;
	point3f point;
	long result_index;
	real squared_distance;
};


void function_1e4770(s_actor_point_request *request, short mode, point3f const *point, short type, real radius);
s_actor_moving *function_1e47d0(s_actor_point_request *request);

// @retail 0x1ff7d0
bool function_1ff7d0(long actor_index, point3f const *point, real enemy_radius, real friendly_radius, short *count_out)
{
	s_actor_view *actor = actor_get(actor_index);
	real radius = enemy_radius > friendly_radius ? enemy_radius : friendly_radius;
	real enemy_squared = enemy_radius * enemy_radius;
	real friendly_squared = friendly_radius * friendly_radius;
	bool result = true;
	short count = 0;
	s_actor_point_request request;
#pragma inline_depth(0)
	function_1e4770(&request, actor->unknown024, point, 2, radius);
#pragma inline_depth()
	s_actor_view *other = (s_actor_view *)function_1e47d0(&request);
	while (other)
	{
		if (actor->unknown024 != other->unknown024 && function_1df560(actor->unknown024, other->unknown024))
		{
			if (enemy_squared > request.squared_distance)
			{
				long perception_index = *(long *)other->unknown01c;
				if (perception_index != NONE)
					count += *(short *)((byte *)perception_get(perception_index) + 0x14);
				else
					count++;
			}
		}
		else if (friendly_squared > request.squared_distance)
		{
			result = false;
			break;
		}
		other = (s_actor_view *)function_1e47d0(&request);
	}
	if (result)
	{
		s_record_pool_iterator player_cursor;
		player_cursor.data = g_4e8c24;
		player_cursor.index = NONE;
		player_cursor.datum_index = NONE;
		byte *player;
		while ((player = data_iterator_next_inlined(&player_cursor)) != NULL)
		{
			long object_index = *(long *)(player + 0x2c);
			if (object_index != NONE)
			{
				s_moving_object *object = moving_object_get(object_index);
				vector3f delta;
				vector3d_from_points3d(point, (point3f const *)((byte *)object + 0x30), &delta);
				real distance_i = delta.i;
				real distance_j = delta.j;
				real distance_k = delta.k;
				real squared_distance = distance_k * distance_k + distance_j * distance_j + distance_i * distance_i;
				if (actor->unknown024 != 1 && team_is_enemy(actor->unknown024, 1))
				{
					if (enemy_squared > squared_distance)
						count++;
				}
				else if (friendly_squared > squared_distance)
				{
					result = false;
					break;
				}
			}
		}
	}
	if (count_out)
		*count_out = count;
	return result;
}

long function_1e1f20(long actor_index);
void *function_1e5240(long actor_index);
point3f *function_b9dd0(long object_index, point3f *result);
void function_1fc710(long actor_index, point3f *position);
real function_30bf0(vector3f *v);
real function_11ce20(vector3f const *a, vector3f const *b);

/* an object's per-region state: 8 bytes each, +0x120 bytes of them at the
   object's +0x122 offset; the byte at +2 is 0xff for a region that is gone */
struct s_object_region_1ff
{
    byte unknown00[2];
    byte unknown02;
    byte unknown03[5];
};

// @retail 0x1ffe00
bool __stdcall function_1ffe00(long actor_index, long object_index)
{
    s_actor_view *actor = actor_get(actor_index);
    s_slot_object_view *object = object_get(object_index);
    byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
    bool result = false;
    byte *weapon_settings = (byte *)function_1e5240(actor_index);
    real best_score = 0.0f;
    long best = NONE;
    if (object->type != 0 || object->player_index == NONE)
    {
        point3f center;
        function_b9dd0(object_index, &center);
        if (weapon_settings)
        {
            real x = center.x - actor->position.x;
            real y = center.y - actor->position.y;
            real z = center.z - actor->position.z;
            real range = *(real *)(weapon_settings + 0x20);
            real distance_squared = z * z;
            distance_squared += x * x;
            distance_squared += y * y;
            if (distance_squared < range * range)
            {
                long model_index = *(long *)(definition + 0x38);
                if (model_index != NONE)
                {
                    byte *model = g_4e3b44[model_index & 0xffff].bytes;
                    if (*(long *)(model + 0x68) > 0)
                    {
                        point3f origin;
                        function_1fc710(actor_index, &origin);
                        for (short i = 0; i < *(long *)(model + 0x68); i++)
                        {
                            byte *entry = *(byte **)(model + 0x6c) + i * 0x1c;
                            long name = *(long *)entry;
                            short region = *(short *)(entry + 0xc);
                            if (region >= 0)
                            {
                                byte *current = (byte *)object_get(object_index);
                                if (region >= (long)(*(short *)(current + 0x120) / sizeof(s_object_region_1ff)))
                                    continue;
                                s_object_region_1ff *regions = (s_object_region_1ff *)(current + *(short *)(current + 0x122));
                                if (regions[region].unknown02 == 0xff)
                                    continue;
                            }
                            s_object_marker marker;
                            if (function_b8d30(object_index, name, &marker, 1, false))
                            {
                                vector3f direction;
                                direction.i = origin.x - marker.matrix.position.x;
                                direction.j = origin.y - marker.matrix.position.y;
                                direction.k = origin.z - marker.matrix.position.z;
                                if (function_30bf0(&direction) != 0.0f)
                                {
                                    real angle = function_11ce20(&marker.matrix.forward, &direction);
                                    if (angle < 1.5707963705062866f && angle < *(real *)(entry + 8))
                                    {
                                        real score = 1.0f - angle * 0.6366197466850281f;
                                        if (name == *(long *)((byte *)actor + 0x33c))
                                            score += 0.3f;
                                        if (score > best_score)
                                        {
                                            best_score = score;
                                            best = name;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (best != NONE)
    {
        *(long *)((byte *)actor + 0x33c) = best;
        result = true;
    }
    else
        *(long *)((byte *)actor + 0x33c) = 0;
    return result;
}

bool function_1c9290(long arg_0, point3f const *arg_1, vector3f const *arg_2,
    real arg_3, real arg_4, long arg_5, bool arg_6);
real normalize2d(point2f *arg_0);

// @retail 0x1ff010
bool __stdcall function_1ff010(long arg_0, point3f const *arg_1)
{
    bool local_0 = false;
    s_actor_view *local_3 = actor_get(arg_0);
    byte *local_4 = (byte *)function_1e5380(arg_0);
    /* without a weapon the shared exit returns the false result */
    if (!local_4)
        goto done;
    {
        point3f local_5;
        function_210850((s_type_c3b527 const *)((byte *)local_3 + 0x7d0), &local_5);
        s_projectile_choice *local_6 = projectile_choice_get(*(short *)(local_4 + 4));
        long local_7 = projectile_choice_definition(local_6);
        vector3f local_8;
        real local_9;
        real local_10;
        bool local_11;
        if (function_fa6a0(local_7, NULL, arg_1, &local_5, NULL, NULL,
            (real const *)((byte *)local_3 + 0x7f4), *(bool *)((byte *)local_3 + 0x7c6),
            &local_8, &local_9, &local_10, NULL, &local_11))
        {
            point2f local_13 = *(point2f *)&local_8;
            point2f *local_13_ptr = &local_13;
            vector3f *unknown290_ptr = &local_3->unknown290;
            if (normalize2d(&local_13) > 0.0f &&
                local_13_ptr->x * unknown290_ptr->i + unknown290_ptr->j * local_13_ptr->y > 0.8660253882408142f)
            {
                vector3f local_14;
                local_14.i = local_8.i * local_9;
                local_14.j = local_8.j * local_9;
                local_14.k = local_8.k * local_9;
                real local_12;
                if (local_11)
                    local_12 = 0.0f;
                else
                    local_12 = function_fa170(local_7);
                if (function_1c9290(arg_0, arg_1, &local_14, local_10, local_12,
                    *(long *)((byte *)local_3 + 0x7e4), local_3->unknown26c != NONE))
                {
                    *(vector3f *)((byte *)local_3 + 0x7e8) = local_8;
                    *(real *)((byte *)local_3 + 0x7f4) = local_9;
                    local_0 = true;
                }
            }
        }
    }
done:
    return local_0;
}

#include "props.h"
#include "unknown_11cc90.h"
#include "vector2f_products.h"

void __stdcall function_26c2d0(long prop_index);
bool function_1fef40(s_type_c3b527 const *target, long actor_index);
extern vector3f *g_4687b0;
void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* forced inline in src/unknown_1eb8a0.cpp */
// score 77 (was 77): frame 0x20 agrees. Retail tests the planar length after the scale products
// Round 28: the existing rotate_vector_170d70 (src/unknown_170d70.cpp) for rotate_direction 130 at retail's 850
// bytes (aligned 106 -> 88; lead), with the output through vector3f_scale 147 (830); vector3f_scale for the output
// alone 147 (828); rotate_direction with scalar cross terms (dot first, or (1 - cosine) first) 134/134 (0x1ecf50
// 120 -> 148). No existing 2D dot or perp-dot helper exists for the facing/side tests.
// with the zero in xmm0 and does not carry that zero into the inlined rotation (memory 0.0 for i, a
// fresh xorps for j/k); ours zeroes xmm6 before the divide and keeps it through the rotation, whose
// loads, dot order and registers then differ. Retail multiplies the facing components as registers
// (`movss reg, [field]; mulss reg, y`) and stores the velocity directly (speed * each component):
// with direct stores the tail matches but the body is 828 bytes. Round 42: retail inlines the same
// rotation helper at about 20 sites; the matched 0x1055b0 and 0x177d10 write it with axis-first
// products and (1 - cosine) first; a file-local copy in that form (not keepable: a twin of
// rotate_direction, which lives in another lane's file) gives aligned 83 against 106 (134), and with
// direct stores aligned 70 (147, 830 bytes), so the rotation's shape is most of what is left.
// Fast checks build this one differently unless 0x1ecf50 and 0x2b400 are checked with it.
// Tried: planar scale loop 153; direction branch update 204; horizontal native copy 142; rotation
// native copy 153; real signed sine 170; horizontal shared facing base 195; horizontal facing rotation
// copy 195; horizontal native vector copy 142; horizontal rotation copy 142; direct products 173;
// native side predicate 152; nested guards 153; entry magnitude 153; both planar normalizations
// inline 153; with the native horizontal copy, direct final products 154; shared direct scale 154;
// depth one 142; size policy 366; shared output-vector scale 142; native turn side 145; length in
// predicate 142; adjustment test result 142; direct side expression 142; turn-test result 139;
// original direction object 185; shared output in place 206; native output in place 206; facing
// object reference 153; turn completion copy 142; output memcpy 142; named turn length 139; result
// declared first 142; native motion view 142; read-only actor 142; both completion copies 142; word
// side 146; named length with word side 143, direct output 146, shared scale 146; forced rotation
// declaration 142; inline declaration 142; planar facing snapshot 220; named dot 142; named
// determinant 142; combined planar result lifetimes 142; planar scale operand order 142; planar
// completion copy 146; native turn initializer 152; turn before side 136; initialized turn before
// side 151; output product input order 142; shared double output 137; shared double direct output
// 167; horizontal copy observation 144; native input observations 154; existing length inline 142;
// earlier turn length result 136; earlier turn product input order 136; earlier turn shared result
// scale 136; earlier turn shared direct scale 160; combined memmove 191; immutable real results 136;
// byte turn truth 136; horizontal memmove 184; turn memmove 168; output memmove 140; external speed
// binding 147; external side components 136; native planar initializer 161; early turn memcpy 136;
// completed output memcpy 136; Y/X horizontal initialization 161; factor entry lifetime 136; native
// output loop 137; turn input before normalization 171; turn workspace at entry 136; output after
// target update 136; normalization failure first 131; Y/X input with output loop 157; native
// horizontal on early turn copy 136; axis reference/snapshot 152/190; native horizontal with either
// axis form 152/190; vertical input 171, with native horizontal 171, with axis snapshot 214; direct
// planar commit 182; planar memcpy 152; original horizontal fallback 162; fallback plus planar copy
// 157; native turn plus fallback 162; normalized point 237; both points 253; normalized point plus
// direct commit 166; named normalized x/y 130 (kept: retail's square order in the planar
// normalization); y declared first 136; named dot 130; direct output stores 147 (828 bytes); side
// before the turned copy 136 (862 bytes); sine as a ternary of constants 149; inline sine with the
// side bool declared before the turned copy 140 (838 bytes); side bool from an if/else 147; vector2f
// horizontal 147; turned declared at the block top 147; no named x/y 160 (screens: speed read at
// each use or the k copy moved before the second normalization shorten the body to 765-788 bytes);
// normalized scalar declaration priority 130; immutable normalized pair 130; planar pair names 130;
// length/pair names 130; second length in normalized-pair context 131; named integer sign 130;
// declaration priority plus planar names 130; sine result arms 149.
// Round 148: Complete helper-context preview agrees at 130. Complete planar facing capture 177; both
// dot/determinant product pairs 141; both input/product forms 177; facing/magnitude/side/all zero-first
// comparisons 130/130/130/130; native output copy then component scales 145. Magnitude/zero, facing, rotation
// and output schedules remain.
// Round 317: Guard-scoped normalized scalars 130; native planar input 130; both 130; all speed-field reads 140; scalar declarations at planar entry 130; adjusted turn name 130; requested direction name 130; native guarded input plus field reads 131.
// Round 262: complete vector setter at requested direction/turn input/both
// 176/147/168; completed output workspace 130; workspace plus requested/turn/both
// 176/147/168; direct final setter 147 (828 bytes). Original body restored.
// Round 77: horizontal array view 130; facing array fields 130; turned array stores 130; direct array output 147;
// combined array views 130; horizontal pointer 137; immutable magnitude 130; immutable normalized scalars with
// direct output 147. Original body restored.
// declaration priority plus planar names 130; sine result arms 149; complete planar facing capture
// 177; both dot/determinant product pairs 141; both input/product forms 177; facing/magnitude/side/
// all zero-first comparisons 130/130/130/130; native output copy then component scales 145;
// guard-scoped normalized scalars 130; native planar input 130; both 130; all speed-field reads 140;
// scalar declarations at planar entry 130; adjusted turn name 130; requested direction name 130;
// native guarded input plus field reads 131; complete vector setter at requested direction/turn
// input/both 176/147/168; completed output workspace 130; workspace plus requested/turn/both
// 176/147/168; direct final setter 147 (828 bytes); round 42: turned, horizontal or the planar
// magnitude/x/y at function scope 130 each (identical). Round 19: the inline planar normalisation
// still compares the length before the divide where retail compares it after, and carries the
// magnitude and the reciprocal in different registers. Round 8 (lane 2): the cause is the 0.0f
// constant: ours zeroes xmm6 once and keeps it from the length test through the rotation's cosine
// products, retail keeps the zero in xmm0 for the three tests and rematerialises it (memory, then
// xorps) in the rotation, so it follows the rotation helper's shape (src/unknown_1eb8a0.cpp, another
// lane's file this round). Horizontal pair as an aggregate of the two components 166 (835 bytes);
// only the normalized turned pair stored back into the direction 176 (801); the k copy dropped
// (pair stored back) 159 (818); integer-zero magnitude test 130; speed-first output products 130.
// magnitude/x/y at function scope 130 each (identical).
// Round 231: facing-first positive / both positive predicates / facing-first nonzero /
// both nonzero predicates 205/231/211/237; with direct component output
// 175/193/181/199. Original admission and aggregate output restored.
// Round 28 (lane 10): with direct output stores the aligned count drops 106 -> 93 (the tail and the
// turned copy then follow retail); the four missing instructions are the facing products, which ours
// write as `movaps reg, x; mulss reg, [field]` where retail loads the field and multiplies by x.
// Normalization products as scale * x, y * scale 130 (identical); direct stores 147 (828 bytes, aligned
// 93); output through vector3f_scale 147 (same code); direct stores without named x/y 160 (821 bytes);
// normalization as if (!(fabs < eps)) { scale } else zero 131 (845 bytes).
// BLOCKED-BY 0x1eb8a0 (the form of its inline rotation helper, src/unknown_1eb8a0.cpp).
// Round 231: facing-first positive / both positive predicates / facing-first nonzero /
// both nonzero predicates 205/231/211/237; with direct component output
// 175/193/181/199. Original admission and aggregate output restored.
// Round 179: the facing and side tests through the shared two-pointer dot_product2d/cross_product2d
// (include/vector2f_products.h) 106 (865 bytes; the side test now matches), with the output stored
// straight into velocity as speed-first products 77 (846 bytes), kept, with `horizontal` a vector2f
// 77. Only the dot helper 146, only the cross helper 150; the side bool before the turned copy 130;
// rotate_vector_170d70 or a local copy in 0x1055b0's axis-first form for rotate_direction 77 each
// (identical code: the rotation's source form is canonicalised, so the BLOCKED-BY above no longer
// holds). Left: ours zeroes XMM6 before the divide and carries that zero into the rotation's first
// cosine product, so the turned components colour XMM4/XMM7/XMM5 as k/j/i where retail has
// XMM4/XMM6/XMM5 as i/j/k; retail's test zero is XMM0 after the scale products, and it multiplies x
// as `movaps xmm1, scale; mulss xmm1, [x]`.
// Round 69 (m3 lane 9, fast with 0x1ecf50 and 0x2b400): the magnitude and facing tests nested 77 (same code);
// the facing vector as a named const 2D pointer for both tests 148 (857 bytes); the turned pair written
// straight into direction on success (no k or fallback copy) 167 (790 bytes); integer-zero literals in both
// tests 77; the rotation's cosine through a named zero local 77 (diagnostic only). The first difference is
// still the magnitude compare and its XMM6 zero scheduled above the divide.
// Round 234: Eager geometric admission, direct/length-first/facing-first/separate predicates
// 180/177/190/174; with native planar component input 199/196/198/193. Original short-circuit admission
// and library copy restored.
// Round 230: native unsigned object/prop handles and signed-byte side truth, seven subsets; all with
// unsigned-byte truth, scores [77, 77, 77, 77, 77, 77, 77, 77].
// Original executable source restored. Zero-test scheduling and rotation register homes remain.
// @retail 0x1ff360
long function_1ff360(long actor_index, point3f const *target, vector3f *velocity)
{
	s_actor_view *actor = actor_get(actor_index);
	long object_index = NONE;
	long prop_index = *(long *)((byte *)actor + 0x7e0);
	if (prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(prop_index);
		if (node->unknown24 >= 1)
			object_index = node->object_index;
		if (node->unknown24 > 0 && *(bool *)((byte *)actor + 0x7cc))
		{
			s_type_5cfb45 *state = function_25d690((s_prop_datum *)node);
			function_26c2d0(prop_index);
			s_type_c3b527 point = *(s_type_c3b527 const *)&state->unknown48;
			point.point.z += 0.2f;
			function_1fef40(&point, actor_index);
		}
	}
	function_1ff010(actor_index, target);
	vector3f direction = *(vector3f *)((byte *)actor + 0x7e8);
	if (actor->unknown26c == NONE)
	{
		vector2f horizontal;
		memcpy(&horizontal, &direction, sizeof(horizontal));
		real magnitude = normalize2d_fast(&horizontal);
		if (magnitude > 0.0f && dot_product2d((vector2f const *)&actor->unknown290, &horizontal) < 0.0f)
		{
			vector3f turned = actor->unknown290;
			bool left = cross_product2d((vector2f const *)&actor->unknown290, &horizontal) > 0.0f;
			real sine = (left ? 1 : -1) * 0.5f;
			rotate_direction(&turned, g_4687b0, sine, 0.0f);
			if (normalize2d((point2f *)&turned) > 0.0f)
			{
				turned.i *= magnitude;
				turned.j *= magnitude;
			}
			else
			{
				turned.i = direction.i;
				turned.j = direction.j;
			}
			turned.k = direction.k;
			direction = turned;
		}
	}
	real speed = *(real *)((byte *)actor + 0x7f4);
	velocity->i = speed * direction.i;
	velocity->j = speed * direction.j;
	velocity->k = speed * direction.k;
	return object_index;
}
