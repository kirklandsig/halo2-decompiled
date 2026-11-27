/* UNKNOWN_2C0D00.CPP: a list of up to 64 circular obstacles on the ground
   plane that a path is steered around: adding them, finding the ones a point
   or a ray hits, and grouping the ones that overlap */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_2c0d00.h"
#include "unknown_11cc90.h"
#include "vector2f_products.h"
#include "vector_products.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

#include "ai_actor.h"
#include "path.h"
#include "object_markers.h"
#include "object_tag_definitions.h"
#include "props.h"
#include "object_queries.h"
#include "unknown_1428b0.h"
#include "unknown_1cec30.h"

inline void point2f_set(point2f *point, real x, real y);

real function_30bf0(vector3f *vector);
point3f *function_b9dd0(long object_index, point3f *result);
bool object_or_parent_hidden(long object_index);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_11bed0(s_location *location, point3f const *point);
short __stdcall function_bb050(long mask, dword type_mask, void const *location,
    point3f const *position, real radius, long *objects, short maximum_count);
short function_26f930(long object_index, s_object_marker *markers, short capacity, word flags);
short function_26fbf0(long object_index, s_object_marker *markers, short capacity, word flags);
short function_26fe20(long object_index, s_object_marker *markers, short capacity, word flags);
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
    word : 2;
    word no_obstacle : 1;
    word : 13;
};

struct s_obstacle_definition_flags_2c
{
    word : 3;
    word no_obstacle : 1;
    word : 12;
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

// score 718 (was 838): the type-7 device flags read from the definition at each use (m3 lane 11 r172). Left: retail's
// second `straight = 1` store on the no-actor path (two rows that shift every later row of ours), the loop-head reload
// order and the XMM colouring of the object and sphere distance tests.
// Earlier at 838 (was 925): Gemini lane GB r8 (2026-11-25, 29 trials, plain-confirmed 838 at exact 3731/3731 B, +0x4a, exact retail frame 0x2cac, aligned 152, masked 109; all 15 file peers/callers MATCH, 0x29cbb0 378 and 0x2c41b0 675 unchanged): type-1 arm written as single-store `if (team == actor->unknown024 || !function_1df560(team, actor->unknown024)) skip_classification = true;` (retail 0x2c13a9 stores 1 only; alone 903), both one-use `velocity` field pointer locals dropped (together 851, flipping `physics` above `skip_classification` and recovering 0x2cac without moving `bool enabled`), `long type = (char)object[0xaa];` across the 4 type arms (retail 0x2c1020 movsx eax), `word` bitfield on `s_obstacle_definition_flags_2c` (matching `s_object_definition::flags` and retail `shr cl, 3; test cl, 1`), all 5 hand-written dot/sq sums replaced by existing `dot3f`/`length_sq3f` helpers. Ablations: without `long type` 877 (3697 B), without `word` 858 (3713 B), without 4 inner helpers 856 (3713 B), without dropping `velocity` 847 (3734 B), the vehicle index read at both uses (a reread, kept only with a byte match) gives nothing (838 either way at m3 review), so it stays a local, without entry `dot3f` 804 (3740 B, +9 B past retail so not kept), + `object_radius` before `offset` 837 (statement order, not kept), + `basis_dot_xzy` vehicle dot 855 (3713 B), `short sphere_count` 968 (3671 B), uninit `straight` + else 808 (3740 B). Match-only leads on GB6's 528: + `object_radius` before `offset` 509 (3731 B, al 130, mk 46; fixes all 24 object-distance insns), + reversed vel1 dot + `dot3f(&difference, direction)` + `(sphere_flags | obstacle_flags) | moving` 509 (3731 B, al 126, mk 42), + `basis_dot_xzy` 468 (3731 B, al 153, mk 67). Earlier (925): m3 lane 15 r144 unchanged (m3 review: its 859 needs `word filter_flags = sphere->flags;`, a three-use copy of a field retail tests in memory at each of the three sites, a form kept only with a byte match; without it 934, the existing-helper calls alone 933; code back to main, readings at the end of this note). r130, m3 lane 8 r220, m3 lane 11 r151, m3 lane 1 r154 and m3 lane 13 r149 unchanged (r220, r151, r154 and r149 at the end, lead below). After later merges the plain check reads 925 at 3725 bytes with frame 0x2cb0 (retail 0x2cac). Earlier 917 (was 920): the frame was retail's 0x2cac after m3 lane 4 r229 (below), the body 3676 bytes against 3731. Left: the entry's
// GB r10 lead 791 HELD as a patch at m3 review (`previous_direction` reaches 0x2c0d60 in ECX, so
// Earlier note (merged from stacked score line): GB r10 lead 791 HELD as a patch at m3 review: `previous_direction` reaches 0x2c0d60 in ECX, so
// its slot in the parameter list is not shown by retail's pushes and was picked by score among slots 8/10/11, a
// parameter order kept only with a byte match; the nested radius gates alone give 832, under the 8-point bar): Gemini lane
// GB r10 (2026-11-25, 25 fast trials + plain confirmation; 0x2c0d60 838 -> 791 at 3733/3731 B, +0x4a, 921/921 insns, fr=0x2cac, mk 41 -> 33; 0x2c41b0 675 -> 661; all 14 peers/callers MATCH, 0x29cbb0 378 unchanged): moved `previous_direction` right after `direction` across definition, 4 declarations and 3 call sites (retail 0x2c41b0 sets up ecx between unit_index and direction; fixes entry dot3f loads; alone 797, 0x2c41b0 661; pos 8/10/11 840/797/874) and wrote both radius gates as nested `if (d <= r * r)` matching siblings 0x2c1c00/0x2c1e70 (nested sphere alone 832, nested obj alone 838, both 832, with prevdir 791). Other trials restored: + `object_radius` first 838 / 791 / 790; moving `dot3f(&difference, direction)` + `(sphere_flags | obstacle_flags) | moving` 838 / 797; inline `*(long *)(object + 0x248)` 838; combo 838; match-only `else straight = true;` 405 (3731 B) / + prevdir 447 (3733 B, +0x157); full combo + early `enabled` + `else straight = true;` 524 (+0x157, mk=26) / + `object_radius` first 505 (al=94, mk=26) / + sph `vector3d_from_points3d` 466 / without prevdir 528 (3731 B, mk=24). Earlier (838, was 925): Gemini lane GB r8 (2026-11-25, 29 trials, plain-confirmed 838 at exact 3731/3731 B, +0x4a, exact retail frame 0x2cac, aligned 152, masked 109; all 15 file peers/callers MATCH, 0x29cbb0 378 and 0x2c41b0 675 unchanged): type-1 arm written as single-store `if (team == actor->unknown024 || !function_1df560(team, actor->unknown024)) skip_classification = true;` (retail 0x2c13a9 stores 1 only; alone 903), both one-use `velocity` field pointer locals dropped (together 851, flipping `physics` above `skip_classification` and recovering 0x2cac without moving `bool enabled`), `long type = (char)object[0xaa];` across the 4 type arms (retail 0x2c1020 movsx eax), `word` bitfield on `s_obstacle_definition_flags_2c` (matching `s_object_definition::flags` and retail `shr cl, 3; test cl, 1`), all 5 hand-written dot/sq sums replaced by existing `dot3f`/`length_sq3f` helpers. Ablations: without `long type` 877 (3697 B), without `word` 858 (3713 B), without 4 inner helpers 856 (3713 B), without dropping `velocity` 847 (3734 B), the vehicle index read at both uses (a reread, kept only with a byte match) gives nothing (838 either way at m3 review), so it stays a local, without entry `dot3f` 804 (3740 B, +9 B past retail so not kept), + `object_radius` before `offset` 837 (statement order, not kept), + `basis_dot_xzy` vehicle dot 855 (3713 B), `short sphere_count` 968 (3671 B), uninit `straight` + else 808 (3740 B). Match-only leads on GB6's 528: + `object_radius` before `offset` 509 (3731 B, al 130, mk 46; fixes all 24 object-distance insns), + reversed vel1 dot + `dot3f(&difference, direction)` + `(sphere_flags | obstacle_flags) | moving` 509 (3731 B, al 126, mk 42), + `basis_dot_xzy` 468 (3731 B, al 153, mk 67). Earlier (925): m3 lane 15 r144 unchanged (m3 review: its 859 needs `word filter_flags = sphere->flags;`, a three-use copy of a field retail tests in memory at each of the three sites, a form kept only with a byte match; without it 934, the existing-helper calls alone 933; code back to main, readings at the end of this note). r130, m3 lane 8 r220, m3 lane 11 r151, m3 lane 1 r154 and m3 lane 13 r149 unchanged (r220, r151, r154 and r149 at the end, lead below). After later merges the plain check reads 925 at 3725 bytes with frame 0x2cb0 (retail 0x2cac). Earlier 917 (was 920): the frame was retail's 0x2cac after m3 lane 4 r229 (below), the body 3676 bytes against 3731. Left: the entry's
// Earlier first line at 925: r130, m3 lane 8 r220, m3 lane 11 r151, m3 lane 1 r154 and m3 lane 13 r149 unchanged (r220, r151, r154 and r149 at the end, lead below). After later merges the plain check reads 925 at 3725 bytes with frame 0x2cb0 (retail 0x2cac). Earlier 917 (was 920): the frame was retail's 0x2cac after m3 lane 4 r229 (below), the body 3676 bytes against 3731. Left: the entry's
// r132 (m3 lane 15, lead below), r130, m3 lane 8 r220 and m3 lane 11 r151 unchanged (r220 and r151 at the end, lead below). After later merges the plain check reads 925 at 3725 bytes with frame 0x2cb0 (retail 0x2cac). Earlier 917 (was 920): the frame was retail's 0x2cac after m3 lane 4 r229 (below), the body 3676 bytes against 3731. Left: the entry's
// MiniMax E (11-20): 849 (from 925, not kept at review): the straight flag's initialiser moved into the two else arms (the actor block's
// previous-direction else and the no-actor else), which gives retail's frame 0x2cac and retail's homes for the four priority
// shorts, the self-unit pointer and the actor pointer; the body is 3708 bytes against retail's 3731. Left: the entry's
// registers (retail keeps the actor index in EBX and the zero in EBP, this build the other way round), the byte homes of the
// two direction flags (this build's aligned and local_4d3867 sit at +0x12 and +0x23 against retail's +0x11 and +0x21), the
// order of the three head byte stores, and the deferred type-0 second branch with the type 1/7/11 arms.
// M3 review (11-20, MiniMax E): 849 came from `straight` declared without a value and set true in two added else arms (aligned,
// beside it, keeps its initializer); statement placement is match-only, so code is back to main (925).
// MiniMax S (11-20): no new form, no lower score (925): retail's prologue reads the first stack argument into EBX and compares it with -1 where this build reads it into EBP, and the frame is 0x2cb0 against retail's 0x2cac; the parameter order may not change, so retail's order is reported instead.
// MiniMax R (11-20): no new form, no lower score (925 fast, 917 plain): the frame is 0x2cb0 against retail's 0x2cac and
// the function-scope homes are not one uniform shift (retail's actor pointer at F+0x14, the range_min short at F+0x30,
// the self-unit pointer at F+0x34, the range_max short at F+0x38, the block_priority short at F+0x48 and the ignore_priority
// short at F+0x50 against ours four bytes further out), so dropping the one extra dword would not line the rest up; the
// entry also keeps the actor index in EBP where retail keeps it in EBX and uses EBX for the zero that retail keeps in EBP.
// Earlier at 917 (was 920): the frame is retail's 0x2cac since m3 lane 4 r229 (below); the body is 3676 bytes against 3731. Left: the entry's
// registers (retail actor_index EBX, self_unit and its zero EBP, unit_index EDI, reloaded at the loop head), the skip flag's home, the
// deferred type-0 second branch and the type 1/7/11 arms. Earlier first line (920): r121, r123, r125 unchanged. frame 0x2cb4 versus 0x2cac; actor/unit retention, the deferred type-0 second branch and
// r121, r123, r125 unchanged. frame 0x2cb4 versus 0x2cac; actor/unit retention, the deferred type-0 second branch and
// MiniMax D (11-19): no new form, no lower score (920 plain, 926 fast): the frame is still 0x2cb4 against retail's 0x2cac, the pool globals are still hoisted above the actor test and the actor address is still folded instead of being kept in EDX, so the actor, unit and byte homes stay where they are.
// Earlier 920 (was 920): r123, r125 unchanged. frame 0x2cb4 versus 0x2cac; actor/unit retention, the deferred type-0 second branch and
// r118 (lane 14) unchanged; r123 unchanged. frame 0x2cb4 versus 0x2cac; actor/unit retention, the deferred type-0 second branch and
// type 1/7/11 arms, and the priority/query/marker homes remain (rounds below).
// MiniMax S (11-15): no new form kept, no lower score (920 plain, 926 fast): the frame map was rebuilt this round
// from both listings.  Retail's function-scope homes are the raised point at F+0x24, the range_min short at F+0x30,
// the self-unit pointer at F+0x34, the range_max short at F+0x38, the location at F+0x40, the block_priority short at
// F+0x48, the ignore_priority short at F+0x50, the actor pointer at F+0x14 and the two direction flags at F+0x01 and
// F+0x03; ours puts the actor at F+0x04, the flags at F+0x03 and F+0x0b, the raised point at F+0x30, range_min at
// F+0x40, the self-unit pointer at F+0x44, the location at F+0x48 and range_max at F+0x54.  The prologue clears the
// same nine homes in the same order as retail (ignore, block, range_min, range_max, self-unit, actor, straight,
// aligned, local_4d3867), so the declarations and their initialisation are already retail's; only the allocator's
// placement differs, our scalar area running eight bytes longer (frame 0x2cb4 against 0x2cac), which carries the
// query array and the marker arrays eight bytes up.  The permuter cannot search this one in the reduced image (916
// reduced against 910 full) and a full build per trial is too slow for the time allowed, so the body is unchanged.
// Earlier 920 (was 920): earlier 920 (was 923), 923 (was 923), 901 (was 901), 882 (was 882); frame 0x2cb4 versus 0x2cac; actor/unit retention and priority/query/marker homes remain.
// frame 0x2cb8 versus 0x2cac; actor/unit retention and priority/query/marker homes remain.
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
// volatile dropped 882 (kept, 3705 bytes); on 899: the location/raised grouping aggregate as plain locals
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
// Round 57 (m3 lane 1, fast): retail shares the raised point's slot with the sphere-loop center and
// reuses location's slot for the object count. Location in its own block ending at the query 925,
// with the count declared first 925; type-0 center/offset at the other-actor block scope 882;
// object matrix at the loop-body top 882; sphere center at the object-loop scope 882.
// Round 479: positive index/visibility/self-vehicle admission subsets 1/2/4/3/5/6/7
// all 931f (control 931); actual returned object transform 940f. Original gates,
// defaults, matrix consumers and complete body restored.
// Round 40 (m3 lane 15, plain; fast misleads): other-actor test inverted (NONE branch first) 886; type-1 skip flag as a
// conditional set (retail's `mov byte [esp+0x12], 1`) 936; enabled declared with the per-object defaults (retail sets
// it in the settings-test block) 918; unit/definition no-obstacle bits as `(bool)((*(word *)p >> n) & 1)` 917 (retail's
// byte shift form; aligned 710 -> 699); that with the inverted test 938. Retail also stores straight = 1 again on the
// actor == NONE path.
// Round 23 (lane 18, plain; fast misleads here, 931 fast against 882 plain): aligned before straight 882;
// the 0x26c flag before the dot 883; self unit chosen right after its fetch 881; the range pair before
// the ignore/block priorities 881; range maximum read before the minimum 882 (one-point moves, not kept).
// Round 234: Native header/sphere/both/filter-word/header-filter/sphere-filter/all observations 888/880/946/925/930/880/946; all with library sphere copy 946. Sphere forms reach retail length but frame 0x2cc4 versus 0x2cac and gain only two; rejected. Header frame 0x2ce4. Original complete body restored; actor/unit and priority/query/marker homes remain.
// N303: Distinct first-second/first-third/second-third/all marker batches grouped at collection
// or object scope: 908f each (control 931f, baseline 882 plain). Exact-only forms restored;
// original separate buffers, query storage, qualifiers and full body retained.
// Round 317: Native first/second/vehicle relative-vector subsets 1..7, all with shared marker dots
// 916/916/911/921/913/913/929/917 fast. Original full body restored.
// Paired-marker ordinary 918; original body restored.
// Actor/unit and priority/query/marker homes and excess twelve frame bytes remain.
// M315: native returned object count with complete priority/ordinary actor-unit/independent-query
// home subsets 0..7: 915/919/993/1011/903/943/1000/1013 fast (control 931).
// Count plus independent query, count alone and count plus priorities: 903/938/923 plain.
// Original body restored; frame, actor/unit and priority/query/marker homes remain.
// F336: Native rejection continues in first/second/third marker scans, seven subsets:
// 931/931/931/931/931/931/931; all scans with direct first/second array consumers 981.
// Fast control 931, full 882. Original guards, receivers, buffers and complete body restored.
// M332: Actual marker result/ordinal/receiver lexical identities, seven scan subsets,
// then distinct object/sphere ordinals: 931/931/931/931/931/931/931/931 reduced, tying control 931 (full 882).
// Original full body restored; frame, actor/unit and priority/query/marker homes remain.
// Round 84 (m3 lane 12, plain): previous-direction dot with the k and j terms' operands swapped 882
// (same code); range bounds renamed to sort before block/ignore (ours lays the four priorities out
// in name order, retail min/max/block/ignore) 882 (same homes); `else straight = true;` for retail's
// second store of 1 on the actor NONE path (0x2c0e9a) 912 (3721 bytes); local_4d3867 declared before
// the other two bools 882 (store order only); the object-distance offset in its own block 882.
// F349: Physical motion/sphere/device flag fields, subsets 1..7, then immutable
// motion/physics receivers: 924/933/923/882/925/900/925/925 fast (control 931).
// Device fields alone plain 896 versus 882; NOT kept: byte/shift instead of retail
// word/masks and no full gain. Original types and body restored; excess frame word homes remain.
// B387: Direct location/raised members and library location copy on the unqualified-settings
// graph, seven subsets, then actual count declaration before the query: 931 each
// (reduced control 931, full 882). Original complete body restored; frame/homes remain.
// Round 125 (m3 lane 7, fast; positional/aligned/masked, fast base 931/460/184): no unit volatile 918/462/185; no actor
// volatile 991/419/171; both 1014/438/171; both with plain location/raised locals 1005/419/167; plus predicates first
// 1005/419/170; plus one-expression direction dot 1020/421/166; plus self_unit first 1005; plus inflation last 1005;
// plus one object-scope center 1030/425/167; plus one object-scope offset 1005; both 1029/428/172.
// Round 143 (m3 lane 5; fast control 931, full 882): priority/range/self-unit names chosen to sort
// in retail's slot order 931f (no slot moves); self unit by a ternary 916f, 899 plain (3721 bytes);
// enemy as `false` plus `if (actor)` 900f, 934 plain; object/marker buffers at entry 931f; combined
// flags as `moving | (sphere_flags | obstacle_flags)` (retail's or order) 931f.
// Lane 18 r41 (plain; fast 931 misleads): straight left uninitialized and set true at the top of the actor block and in
// an `else` 882 (VC merges both stores ahead of the branch; retail keeps the second); the or order above 882 plain; the
// previous-direction dot as one expression 893 (3694 bytes); the object lookup moved after the unit/ignore skip 918;
// plain location with the raised point in the else block 925 again (3702 bytes). Retail keeps actor_index/unit_index/
// self_unit in EBX/EDI/EBP and reloads them at the object loop's back edge; the count reuses location's slot.
// J246: Complete paired vertical bounds, whole velocity projection and expanded-radius
// producer subsets 1/2/4/3/5/6/7, then all with radius square:
// 882/882/924/882/924/924/924/924. NOT kept: shared radius grows frame to 0x2cbc
// versus 0x2cac; other complete expression values tie. Original whole body restored.
// B406: Required/excluded sphere-admission masks byte/word/long/dword at filter
// 928/893/897/897f; per-object mask producers 946/892/888/888f (control 931f, full 882).
// NOT kept: retail has separate Boolean/byte gates; frame remains 0x2cb8 or grows to
// 0x2cbc versus 0x2cac. Original filters, qualifiers, buffers and complete body restored.
// B408: Raised query point reused by other-actor/vehicle/sphere center lifetime subsets
// 1..7: 915/917/926/923/932/897/938f; original query record with shared sphere center 905f
// (control 931f, full 882). NOT kept: frames 0x2cb4/0x2ca8/0x2cb8 miss retail 0x2cac
// and lengths 3680..3708 move away. Original query, point lifetimes and full body restored.
// M353: Motion admission produces enabled/disabled priority truth, bool/byte/short/long:
// 926/926/885/885; complements 882/882/932/882 fast (control 931; full 882).
// NOT kept: test-produced flags miss byte equality and native frame 0x2cac.
// Original priority/enabled graph, qualifiers and complete body restored.
// DH332: Standard coordinate differences, seven subsets then reverse sphere traversal; reduced control 931:
// 935/935/917/896/904/959/920/1022. NOT kept: lower previews grow frames away from 0x2cac; one joint gain has regressing parts. Full 882 retained.
// Original complete body and contracts restored.
// M371: Paired object/sphere ordered-radius overlap/exclusion truth in
// bool/byte/short/long: eight 931 (reduced control 931; full 882). NOT kept:
// byte-neutral decisions; original tests, qualifiers and complete body restored.
// Round 212 (m3 lane 3, fast control 931, plain 882; aligned 460 at control): marker arrays at the top of the function 931; object matrix at
// the loop-body top 931; object offset at the loop-body top 931; sphere centre and difference at the loop-body top 931 (identical code each);
// unit volatile dropped 918; actor volatile dropped 991 (3648); both 1014 (3632, frame 0x2cb4); both plus location and raised as separate
// locals 1005 (3643, frame 0x2cb0, aligned 419); the query aggregate alone dropped 904 fast (3702, frame 0x2cb4).
// Y591: Positive complete classification in bool/byte/short/long, entry/admitted defaults:
// 939/939/919/933/936/936/919/933 reduced (control 931, full 882). NOT kept:
// no byte match or native frame repair; original decisions, qualifiers and body restored.
// F383: Paired normalized enemy results byte/short/word 931/902/902 fast;
// complementary bool/byte/short/word/long 937/914/920/920/915 (control 931; full 882).
// NOT kept: result domains/polarity do not recover native frame or byte-result transport;
// original complete body, query storage, qualifiers and all consumers restored.
// F385: Persistent actor-mode presence byte/char/word/short/long, reduced
// 931/931/954/954/955; absence bool/byte/long 931/931/942 (control 931, full 882).
// NOT kept: frames 0x2cb8/0x2cbc miss 0x2cac; original complete body/qualifiers restored.
// P343: Actual first/second marker and sphere element lvalues, seven subsets then mutable records:
// 931/931/931/931/931/931/931/931 reduced (unchanged control 931; full 882). NOT kept: no equality;
// original buffers, observations, query, qualifiers, helpers and full body restored; native
// frame/actor/unit and priority/query/marker homes remain.
// Round 74 (m3 lane 11, plain): the raised point through the existing real_point3d_set 882 (same code); diagnostic,
// the type classification (type 0/1/7/11 arms) in a file-local forceinline helper returning false for each skip
// 1014 (3634 bytes, masked 409; the layout stays in source order, so the depth-first placement is not an inlined
// helper's); settings read once through its address in the actor block 882 (masked 420); the previous-direction
// dot through dot3f alone 893 (3694 bytes); the computed actor pointer used inside the actor block, stored once
// into the volatile actor 884 (masked 412).
// Round 152 (m3 lane 5; plain unless marked, fast misleads here: fast base 931 against plain 882; [m, x] masked and exact aligned counts, body [185, 475]):
// the unit no-obstacle bit as a word bitfield 917 [183, 464] (retail's `shr dl, 2; test dl, 1`); the unit looked up through a second
// actor_get 918 [183, 475]; both 936 [181, 464]; self_unit declared first 931f; the actor flag after self_unit 933f; the continues
// in the type arms as a flag tested after them, in a do-while 939f (3743 B) [206, 477], the arms do not sink. Retail's type 1/7/11 arms
// and the unit arm's other branches sit past the function's `ret`, each jumping back to the distance test; ours keeps them in place.
// Walk r715 (s1D), statement by statement against retail 0x2c0d60-0x2c1bf0: no logic, field, offset, constant or condition-direction
// difference found (all constants read from the XBE; team_is_enemy matches retail's inlined 0x1df560 body at 0x2c12c4-0x2c1318). What differs is
// layout only: retail places the second type-0 branch and the type 1/7/11 arms after the join (0x2c12a3-0x2c1441, jumping back into it) and keeps
// the skip flag at [esp+0x12], a profile-style block order that source order has not reproduced. The unit_index and actor volatiles carry no
// retail meaning beyond forcing the reloads retail shows at the loop back edge (0x2c0f68 edi from [esp+0x2ce0], 0x2c0f6f ebp from [esp+0x44]).
// Round 68 (m3 lane 2, plain; fast misleads here, 919 fast at the same source): the start count is 901 now. The two marker loops' up
// dots through the existing vector3d_from_points3d and dot3f (round 317's first+second subset, fast-only there, 921f; first plain
// measurement) 901 (kept: masked aligned 317 -> 301, the first loop's dot is then
// retail's; it removes two hand-written copies of those helpers), the sphere loop's direction dot through dot3f 901 (identical code,
// kept). Not kept: the left-axis dots through dot3f too 900; the vehicle actor's direction as a typed vector pointer 901 (masked 297);
// the vehicle index read at each use 947 (3753 bytes). Diagnostics: retail lays the marker buffers out second, first, third (low to
// high; ours first, second, third); renaming the second to sort first or declaring it first moves nothing (same offsets).
// M3 lane 3 r225 (plain; base 901 after the merge, aligned [exact, stack-masked, register-masked] [750, 662, 363]; fast 919 with
// CAVEATs): `else straight = true;` after the actor block (retail stores 1 into straight a second time at 0x2c0e9a) 912 [750, 665, 361];
// that plus no unit/actor volatiles and plain location/raised locals 986 (3655 bytes, frame 0x2cb0) [584, 630, 327]; unit volatile
// dropped alone 928 [749, 661, 362]; actor volatile dropped alone 982 (3662, frame 0x2cb4) [581, 628, 334]; the query grouping
// dropped alone (plain location and raised) 922 (3708, frame 0x2cb4) [753, 665, 357]. The volatile-free forms align far better
// but lose the positional count; retail reloads actor_index/unit_index at the loop back edge and keeps actor at [esp+0x24].
// m3 lane 12 r106 (fast, control 901 = plain baseline): the type-1 arm as `if (team == ... || !function_1df560(...))
// skip_classification = true;` (retail 0x2c13a9 stores only the 1) 924; the type test inverted, `if (type != 0) { 1/7/11
// chain } else { type 0 }` 915 (3737 bytes; the chain just moves ahead of type 0, nothing goes past the ret); both 927;
// the 1/7/11 arms as separate `if`s 901 (same code); that with the type-1 conditional set 924.
// MiniMax D (11-10): no new form, no lower score: retail and this build agree on the arguments (the previous
// direction arrives in ECX and ten arguments are pushed), so only the frame (0x2cb8 against 0x2cac) and the local
// homes still differ.
// M3 lane 9 r109 (plain; start 923: the call to 0x26fbf0 now passes `word flags` as retail does, which moved homes since 901):
// the raised point through the existing vector3f_add_scaled 923 (same code, kept); the definition no-obstacle bit in a word
// bitfield 916 (retail's `mov cl, byte; shr cl, 3; test cl, 1`; aligned 619 -> 620; a 7-point gain, not kept); on that: the unit
// bit in a word bitfield too 929 (both bit tests then retail's, aligned 603), the unit bit as `(object[0x10a] >> 2) & 1` 916 (same
// code: the byte shift folds to `test byte`), the other actor index read at both uses 949 (3753 bytes), the other actor's offset and
// dot through vector3d_from_points3d and dot3f 924 (3737 bytes). Retail keeps actor_index in EBX through the object loop (reloaded
// at its top), so the type-0 arm's `push eax` reuses the zero type for 0x25d810's false; ours reads actor_index from memory.
// M3 lane 11 r89 (plain; base 923; [aligned, register-masked]; fast misleads here, 902 at 923): kept: the grouping aggregate of
// location and the raised point dropped for two plain locals (raised in the else block; re-measure of r35/r212/r225 on this base)
// 920 (3718 bytes, frame 0x2cb4; [619, 455] -> [680, 528], so positional; the object count then shares location's slot as in
// retail); same code with location in its own block ending at the query; the vehicle-actor, vehicle velocity and sphere-loop
// velocity dots through the existing dot3f 920 (same code, kept). Tried on 920: the type-1 arm's team as a sign-extended long
// (retail's movsx there; on 923) 926 (3737 bytes); the object offset and its squared length through vector3d_from_points3d and
// length_sq3f alone 927 (3724 bytes); the other actor's facing dot through dot3f alone 932; the other actor's offset through
// vector3d_from_points3d alone 927; the type local kept for the arm chain with the range test, `1 << type` and the sphere loop
// reading the type byte (retail re-reads it at those three) 912 (3724 bytes, [658, 534]; a local/field mix, not kept); no type
// local, the byte read at every test and use 936 (3740 bytes, a re-measure of r121's 903); the arms as a switch on the type byte
// with the later uses reading the byte 947 (3772 bytes); `object_index != settings->unknown18` 920 (cmp operands flip only); the
// vehicle actor through actor_get in the condition with no NULL-initialised pointer 933 ([664, 513]); the combined flags computed
// in the call 927 (3724 bytes). Diagnostic: local_4d3867 renamed to sort before the sphere flags (retail's byte order) is the same
// code; the byte homes do not follow names. Ours keeps skip_classification in BL (shared with enemy) and straight in its own
// dword; retail keeps skip at [esp+0x12] beside blocking/aligned/straight, with actor_index and self_unit in EBX and EBP.
// T97: Nested identity/visibility/selected-carrier object admissions, seven subsets, then all with distance:
// eight at 926 reduced (control 926; full 920). NOT kept: byte-neutral scope graphs; original
// guards, qualifiers, marker/query storage and complete body restored. Frame 0x2cb4 still exceeds 0x2cac.
// T100: Existing object-header flag/property/model fields 926 fast, 920 plain, kept
// with unchanged compiled code from the opening body. Completed marker-query result sets
// 1/2/3/4/5/6/7: 926/926/926/926/926/926/924 fast (control 926). NOT kept: the
// two-point conditional flag lead is below the gate and not exact; original scans
// restored. Frame 0x2cb4 versus 0x2cac and actor/unit/priority homes remain.
// P444: read-only byte receivers 926f; blocking-priority producer 926f; paired side-projection producers 926f; facing-limit producer 926f; node-presence producer 926f; motion-sphere result 942f; selected-carrier producer 926f; exhaustive priority result 924f.
// NOT kept: no byte match or supported native frame/result recovery; trial forms restored.
// Existing object-header fields used for the flag, property and model reads.
// P447: Visibility/reference/property/model/physics/count decisions; lower/upper vertical exclusions at original producers:
// 926/926/926/926/926/926/926/926. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P450: Original-site first/second/third marker eligibility in bool/long 926 each; lazy motion eligibility 916/916
// (fast control 926, full 920). NOT kept: one-consumer decisions lack byte equality; motion shortens
// to 3702/3731 and misses native frame 0x2cac. Original complete body restored.
// MiniMax B (11-13): no new form, no lower score (920): the frame is 0x2cb4 against retail's 0x2cac, and the actor,
// unit, priority and query homes still differ; the byte homes do not follow the order of the source's object scan.
// P463: Complementary device-sphere admission bool/byte/short/long 917/917/918/920;
// complementary restricted-sphere admission 926/926/924/924 reduced (control 926, full 920).
// NOT kept: native positive-byte results and frame 0x2cac are unrecovered;
// all original classifications, buffers, qualifiers and complete body restored.
// P465: Existing actor/query-buffer/query-count/location/motion/physics/transform/radius
// identities independently named by role on the separate-location graph: eight 926 reduced
// (control 926, full 920). NOT kept: frame remains 0x2cb4 versus 0x2cac; original full body restored.
// T106: Common unit-relationship completion with bool/byte/short/long presence
// and absence results: eight 1001 reduced (control 926, full 920).
// NOT kept: test-owned completion misses equality, frame and native classification
// tail placement; original complete body, qualifiers and call tuples restored.
// MiniMax C (11-14): no new form, no lower score (920); the frame still reserves 0x2cb8 against
// retail's 0x2cac.
// M3 lane 14 r94 (plain; base 920): one `enemy` local at the type-0 arm's scope, assigned in both branches, 927 (3724 bytes);
// the skip flag declared at function scope 920 (same code); the other-actor branch's `if (actor)` wrapper folded into its first
// inflation test 920 (one jump target moves); a `char` type local 949 (3692 bytes); the second branch's condition nested as
// `else if (actor) { if (...) }` 920 (same code); the actor read once into a local in the other-actor branch 958 (3683 bytes;
// aligned exact 680 -> 637, retail too reads the actor's slot once per branch, into EBP), in every arm 973 (3672 bytes,
// aligned 619). Retail's other-actor branch falls into the distance test through one shared inflation/skip tail (skip at
// [esp+0x12]); ours ends each inflation path with `sete bl; jmp` and keeps the remaining arms in source order.
// MiniMax I (11-14): no new form, no lower score (926): the frame is still twelve bytes larger than retail's (0x2cb4
// against 0x2cac), and the locals are still cleared at the frame's later offsets where retail clears them earlier.
// P506: Input priority defaults before actor lookup, ignore/block/range subsets 1/2/4/3/5/6/7 and
// guarded all: 918/918/958/907/958/979/973/973 reduced (control 926, full 920). NOT kept: native
// later input reads/frame not recovered; original complete body restored.
// M3 lane 15 r86: no new form run. The leads in the listing are the second `straight = 1` store on the actor NONE path,
// the type byte re-read at its three later uses, the shared inflation/skip tail and the actor/unit/self-unit registers.
// The note above already lists each of them with its score (rounds 84, 89, 94 and 225). No existing accessor for the type
// byte exists to give its re-reads naturally. Body unchanged at 920.
// M3 lane 8 r169 (plain unless marked f; fast misleads here, fast control 926): an opcode alignment puts most of the structural
// gap in one moved block: retail lays the object distance test straight after the other-actor inflation tail (shared store of
// xmm0, skip in [esp+0x12]) and moves the 0x13c branch and the type 1/7/11 arms after the motion block's `jmp`; ours keeps them in
// source order with each inflation path ending `sete bl; jmp`. LEAD, not kept (a bool started false and set by an if; the direct
// form is main's 920): the type-1 arm as `if (team == actor->unknown024 || !function_1df560(...)) skip_classification = true;`
// 909 (3718 B; retail's type-1 arm only ever stores 1, `mov byte ptr [esp+0x12], 1`). The unit no-obstacle bit through the shared
// `s_object_view` word bitfield `flag2` (as 0x20ba00 reads it; gives retail's `mov; shr 2; test 1`) 930 (934f); the definition
// bit as `(bool)((definition->flags >> 3) & 1)` (retail's `mov; shr 3; test 1`) 933 (916f); all three 930; the squared length
// computed before the combined radius 926f (same code); the other-actor skip as `if (!enemy) skip_classification = true;` 950f.
// Body unchanged.
// P535: Complete relationship inflation results, conditional/exhaustive/actor-inclusive producers; reduced control 926, full 920. New zero-result publication is absent from native retained-default paths; frame 0x2cb4 still exceeds 0x2cac.
// 916/916/916/916/916/916/916/916. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// M3 lane 4 r203 (plain): constant-left float compares, `straight = 0.95f < dot` with `aligned = 0.866f < dot`, `0.0f <
// function_30bf0(&offset)`, `0.0f < dot && vehicle_index != NONE`, the sphere loop's `0.0f < dot` and `2.0f < dot3f(...)`
// 920 each; own dead locals n = 1..7 920/920/932/938/933/933/929. Frame map (frame offsets, ours/retail): raised point
// 0x30/0x24 (shared with the sphere-loop centre in both), location 0x48/0x40, type-0 offset 0x5c/0x54 and centre 0x68/0x60,
// vehicle centre 0x74/0x6c, object matrix 0x80/0x78, object indices 0xb4/0xac, markers 8 higher each: the scalar area
// under the raised point is 0xc larger here and the gap between raised and location 4 smaller. Aligned slot map (esp
// offsets after the four pushes, retail -> ours): bools 0x10/0x11 aligned/0x12 skip/0x13 straight -> 0x12/0x13/BL/0x1b
// (straight takes a dword of its own here), 0x14 -> 0x1c, 0x1c -> 0x24, bools 0x21/0x22/0x23 -> 0x22/0x21/0x23, actor 0x24
// -> 0x14, 0x28 -> 0x28, 0x2c -> 0x30, 0x30 -> 0x34, raised 0x34 -> 0x40, range_min 0x40 -> 0x50, self_unit 0x44 -> 0x54,
// range_max 0x48 -> 0x64, location 0x50 -> 0x58, block 0x58 -> 0x4c, ignore 0x60 -> 0x60. Body unchanged.
// M3 lane 15 r93 (plain, 0x2c0d00 matched each time): the object radius test as `distance_squared > object_radius *
// object_radius` 920; the combined flags computed inside the non-blocking arm 919; the object radius sum written radius
// first 920; the sphere counter `long` 919 (3723 bytes; retail's counter is a short, `movsx eax, ax`); both of the 919 forms
// together 919 (3721 bytes); the block-priority test with `aligned` first 920; the range test with `type != 0` first 920.
// Both 919s are under the keep bar. Retail tail-merges the two 0x2c0d00 calls as ours does. Body unchanged.
// M3 lane 15 r95 (plain): the two component reads through the existing havok_component_get (as matched 0x22a110
// reads its component) 920 (same code, kept); the unit and definition no-obstacle bitfields with word storage (a
// compile-only screen: `(bool)` on a word-storage bitfield gives retail's `mov al, byte; shr al, n; test al, 1`, byte
// storage gives `test byte`) 937 (3708 bytes: a loop pad and a jmp drop); that with both volatiles dropped 958 (3687
// bytes; register-masked aligned 172 -> 152); `if (object_count > 0)` around the object loop 920 (aligned 173);
// `enabled` declared after the four per-object defaults (retail sets `dl = 1` ahead of the settings test) 920 (aligned
// 171). Retail reads the actor's team once per arm (`xor esi, esi; mov si, [actor + 0x24]`, compared `cmp cx, si`)
// where the volatile actor makes ours read the pointer and the field twice.
// M3 lane 13 r103 (plain, the file's other matched functions stay matched; [exact, register/stack-masked] aligned counts by
// opcode, base 920 [480, 172]): a redundant `if (object_count > 0)` guard around the object loop 920 [480, 173] (match-only
// steering form, no change); skip_classification as `byte` 920 (same code); the skip flag declared after the two sphere flags
// 920 [479, 170]; re-measured on this base: no unit_index volatile 936 (3708 bytes) [473, 166], both volatiles dropped 972 (3671
// bytes, frame 0x2cb0) [423, 152] (diagnostic), `else straight = true;` 929 (3722 bytes) [480, 172]. The last tested whether
// retail's second straight store (an else block on the actor NONE path) is what stops ours hoisting the object-table global
// load to the entry (ours `mov edx, [g_4e0300]` before the actor test; retail loads it in the actor block and again in the
// location block): it is not, the hoist stays. Body unchanged.
// M3 lane 13 r107 (fast, control 926f, 0x2c0d00 MATCH in each; aligned/masked by difflib 655/298): retail's frame has three
// separate 32-marker buffers and loads position/direction into EBX/EBP in each marker loop's preheader, so the three marker scans
// were tried as file-local helpers returning bool (each with its own buffer; three new one-caller helpers, match-only): declared
// `inline` 1404f (3241 B: called out of line), `__forceinline` 924f (3724 B, frame still 0x2cb4, aligned 656, masked 307: the same
// code as the written-out loops, direction still loaded at the prologue); with both volatiles dropped 984f (3659 B; the volatile-
// free body alone re-measured 975f, 3671 B, frame 0x2cb0, aligned 512, masked 267). The outer counter declared at function scope,
// the motion pointer declared at function scope, both: 926f each (same code), with both volatiles dropped 975f (same code as that
// form): scope and declaration position place no home. Retail's cells from the listing (esp offsets after the four pushes): the
// outer counter 0x4c and the object count 0x50 (the dead location's slot) sit among the function-level shorts (range_min 0x40,
// self_unit 0x44, range_max 0x48, block 0x58, ignore 0x60) with the motion pointer at 0x5c; the definition pointer 0x2c later
// holds the sphere counter, and the sphere flags 0x30 reuse the x87 dot temporary; retail has no home for the type (it re-reads the
// byte) and packs straight with three other bools at 0x10-0x13, which is where ours spends its extra cells. The loop head
// (0x2c0f68) reloads unit_index, self_unit and actor_index into EDI/EBP/EBX and the first entry jumps over it. Body unchanged.
// Typed position and definition index 926, index only 926, position only
// 926, both velocity reads 926, all named fields 926, shared point difference
// 924, distance-local scope 926, typed selected unit 926 (reduced control 926).
// NOT kept: shared difference gains two in the preview and leaves frame 0x2cb4.
// Straight/aligned/both assigned in complete threshold arms 926/926/926 in the reduced image;
// clear their prior true values on a failed threshold, straight/aligned/both 947/947/922;
// start both false and set each on success 926; strict threshold first for both results 925.
// NOT kept: the four-point reduced gain (926 to 922) shortens the body to 3702/3731 bytes
// and retains frame 0x2cb4. The straight flag's dword-sized slot and actor and unit homes remain.
// M3 lane 8 r184 (fast with 0x2c3550/0x29c860, both matched; fast control 926, plain 920): self_unit declared after the actor
// pointer 926 (same code); `location` declared at the function top 926 (same code); the previous-direction dot through the
// existing dot3f 932 (3707 B); self_unit set by a ternary on unit[0xaa] 929 (3722 B); object_indices declared first 926 (same
// code). Body unchanged.
// Earlier stacked first line (925, was 925), merged here: the entry keeps actor_index in EBP and a zero in EBX (retail actor_index EBX, self_unit and
// Earlier header (925, was 925; merged into the note above in r130): the entry keeps actor_index in EBP and a zero in EBX (retail actor_index EBX, self_unit and
// Earlier 925 (was 925): the entry keeps actor_index in EBP and a zero in EBX (retail actor_index EBX, self_unit and
// Earlier at 925 (was 925): the entry keeps actor_index in EBP and a zero in EBX (retail actor_index EBX, self_unit and
// its zero in EBP), the frame is 0x2cb0 against retail's 0x2cac, the byte homes differ and the join is laid out in
// source order; the rounds below. MiniMax B (11-18) at 920: no new form, no lower score (920 plain, 926 fast). The permutation
// MiniMax B (11-18): 920 then (was 920): no new form, no lower score (920 plain, 926 fast). The permutation
// search over the body (300 s, one job, the full image because the reduced image misleads here) made two tries and
// never beat its baseline of 910 differing instructions; read from the listing again: retail takes the actor's unit
// field with the pool's data base and the scaled index kept apart ([edx + esi + 0x18] against our folded address),
// holds the actor pointer in edx instead of the volatile home at +0x14, zeroes the cells at 0x40, 0x48, 0x58, 0x60
// and 0x24 and packs its four bools at 0x11, 0x13 and 0x21 where ours spends 0x13, 0x1b and 0x22, so the frame is
// still 0x2cb4 against retail's 0x2cac and the body is 3718 bytes against 3731.
// The excess frame word pair and the actor/unit and priority/query homes remain.
// Actor publication after unit lookup, confidence tests, self-unit test, and settings, each with opening then ordinary unit index: 902/877, 922/932, 902/935, 924/924 reduced.
// NOT kept: 902 and 877 shorten the body to 3702 or 3708 against retail 3731, and the matched caller scores 1 in those previews.
// M3 lane 15 r101 (plain unless marked; control 920 plain, 926 fast): `unit_index` without its volatile 936 (3708 B; fast 910
// misleads again); the actor pointer without its volatile 969 fast (3671 B); the object type byte read at each of its seven
// uses with no type local 936 (3740 B); the previous-direction dot as one expression 928 (3707 B); its products written
// previous-direction first 920. Retail's entry keeps actor_index in EBX, the zero (self_unit's NULL) in EBP and unit_index in
// EDI; ours keeps direction in EBX, settings in ESI and the zero in EDI. Body unchanged.
// Typed object fields with a long/signed-byte category snapshot 920/949; category read
// at each use 936, with switch dispatch 947, with the typed selected unit 936.
// Direct category with ordinary actor/index/both 945/937/968; cached category with ordinary
// actor 974; direct category with conditional ordinary actor initialization 944.
// NOT kept: the 945, 968 and 944 forms recover frame 0x2cac but shorten the body and
// still carry the incoming actor and unit in different registers.
// Settings after actor evaluation by index or selected actor 920/919, per-action optional
// settings tests 920, complete priority or range defaults within the gate 931/916,
// nullable settings after actor evaluation 927, immutable limits with ordinary/scoped query 960/960.
// NOT kept: the four-point preview improvement leaves the eight-byte frame difference.
// N421 previews: slot-object accessor with the existing field view 926; slot-object type field observed
// through a signed byte reference 944; actor team observed once for the type-one comparison and call 910;
// actor team observed once for the vehicle comparison and call 932; signed actor team observed once in each
// comparison and call 913; word team pair observed once in each comparison and call 913; typed facing flags
// and vectors on both actor paths 926; typed object fields with stable team pairs 913.
// NOT kept: the reduced 910 and 913 forms retain the larger frame and shorten the body away from retail.
// The type reference closes four frame bytes and regresses in score and length. Reduced control 926.
// Opening body restored.
// Chained limits together or in pairs, chained direction defaults, both combinations and
// default assignments after declarations 920 each; settings-action mask with paired/original
// defaults 918/918. NOT kept: two points below the retention threshold and frame 0x2cb4 persists.
// M3 lane 10 r123 (fast with 0x2c2060 at 400 and 0x2c3030 MATCH; fast control 926, aligned 655): retail forms the actor
// address once and reads its unit field from that address before storing the pointer to the actor home; ours stores
// into the volatile home and reads it back. The actor fetched into a plain local that the head reads (unit field and the
// target field) and that is stored into the volatile home 923 (3702 B, aligned 612: retail's address fold, `mov reg,
// [index + data + 0x18]` then `add`); with only the unit field read through it 945; with the dot's products written
// previous-first 923 (same code); with the home stored after the unit read 923 (same code); with the home not volatile 980
// (3657 B, aligned 577). Not kept: a gain of 3.
// Actor and unit registers and frame 0x2cac still differ. Typed actor datum for
// unit alone or unit and state, pointer published before/after lookup:
// existing pointer 945/923 and 919/923, ordinary pointer 980/980 and 987/987.
// After lookup with ordinary incoming unit index, same pairs 879/880 and
// 986/986. Reduced control 926. NOT kept: the delayed typed read recovers
// the scaled unit-field access, but the 879 form retains frame 0x2cb4
// and shortens the body to 3708 bytes against retail 3731.
// M3 lane 10 r125 (fast with 0x2c2060 at 400 and 0x2c3030 MATCH; fast control 926, plain 920): retail stores straight = 1
// a second time on the actor_index == NONE path, in a block just ahead of the join (0x2c0e9a), besides the head's store.
// `else straight = true;` after the actor block gives that block and store: fast 919, plain 929 (3722 B); with the actor
// fetched into a plain local first (r123's 923 form) fast 924; `else { straight = true; aligned = true; }` fast 936 (3708
// B); straight left uninitialised and set true at the top of the actor arm and in the else arm fast 926; the NONE arm
// written first (`if (actor_index == NONE) straight = true; else {...}`) fast 916 but plain 926 (3722 B; the store block
// is then laid out at the head). The fast preview misleads here by up to 13. Body unchanged.
// M3 lane 14 r118 (fast with 0x29a190, 0x2c0d00 and 0x2c3550, all matched; fast control 920 = plain; difflib-aligned in
// brackets, control [687]): the type arms as `switch (type)` on the type local 926 [700] (3740 B); `long object_count` 925
// [636] (3722 B); the combined flags as `moving | (sphere_flags | obstacle_flags)` 920 (same code, first plain-equivalent
// measurement); the object fetched after the unit/ignore skip test, defaults kept ahead of it, 919 [673]; the defaults and
// the fetch both after that test 926 [673]. Retail's layout puts the join (distance test, definition bit, classification
// start up to its first `jmp`) right after the type-0 other-actor branch, with the type-0 `else if` branch and the type
// 1/7/11 arms after it jumping back; every form here keeps source order.
// M3 lane 5 r212 (fast with 0x2c0d00/0x2c3550/0x29c860, plus 0x29a190/0x29cbb0/0x2c41b0 for the signature trials, all stayed
// as they were; fast control 926 [masked 171, exact 465]): the previous direction as the ninth parameter (definition, three
// declarations, three calls) 928f (3707 B); both volatiles dropped 975f (3671 B, frame 0x2cb0); that with the ninth-parameter
// order 983f, with `else straight = true;` 955f, with both 963f; the marker query word read once per object into a named local
// for the three queries 932f (3702 B). Diagnostics on the volatile-free body (reverted): no second object lookup for the
// location 982f; the settings block removed 1064f (actor_index then EBP, unit_index EDI at entry); the loop's four settings
// reads removed 1024f, only unknown18 970f, only the three marker words 1021f: the loop's settings reads are what put settings
// in EBX at entry here, where retail reads it into ECX in the settings block only and keeps actor_index EBX, unit_index EDI,
// self_unit EBP. The object table global made address-taken (file-scope pointer, diagnostic) 966f main body, 926f volatile-free
// (3734 B): its entry hoist stays, so that is not what keeps retail's per-block table loads. Body unchanged.
// M3 lane 13 r121 (fast with the three callers 0x2c41b0, 0x29a190, 0x29cbb0 and 0x2c0d00, 0x2c3030, 0x2c2060, 0x2c3550,
// 0x29c860; this set's control is 920 = plain; [difflib-aligned] in brackets, control [680]): retail's call in 0x2c41b0
// computes the previous-direction argument right after it pushes the ignore and unit indices and before the direction,
// so the declared order is (actor_index, flags, settings, obstacles, blocking_obstacles, position, radius, direction,
// previous_direction, unit_index, ignore_index). With that order at the definition, the four declarations and the three
// calls, 0x2c41b0 goes 675 -> 661 and the two matched callers stay matched, but this body reads 928 [678] (3707 B), so
// it is not kept: build/r121/lead-2c0d60-param-order.patch. On the present order: both qualifiers dropped 972 [541]
// (3671 B, frame 0x2cb0); the type byte read at each use 936 [667]; both 968 [502] (3676 B, frame 0x2cac, 925
// instructions against retail's 921). What is left on that base is the entry's registers (ours settings EBX, the object
// table ESI, zero EDI; retail actor_index EBX, self_unit and its zero EBP, unit_index EDI, held up to the loop head and
// re-read on its back edge), the skip flag's home (ours BL; retail's byte at +0x12, packed with blocking, aligned and
// straight) and the moved join block. On that base: the skip flag forced into memory with a qualifier (diagnostic) 889
// [489] (3708 B; the four bools then pack as retail's, but its stores go through a register where retail has `sete byte
// ptr`); the entry dot through dot3f in either order 975 [500]; the two type-0 inflation tails as one forceinline helper
// with the inflation and the skip flag passed by address 968 (same code as written out; the first tail only 947 [496]);
// the settings block as a forceinline helper with the four priorities by address 991 [487]; the new order 975 [493]. A
// switch over the type local 926 [697], over the type byte 947 [683]; with the new order 889 [696] (3732 B: position
// only, the aligned count is worse) and 937 [678]. Retail reads the type byte four times in the loop: once sign-extended
// for the four arm compares (`movsx eax`, then `cmp eax, 1`, `7`, `0xb`), and as a byte at the range test, at the
// `1 << type` and in the sphere loop.
// MiniMax H (11-19): no new form, no lower score (920): retail lays the join out of source order, with the type 0
// other-actor branch followed by the join and the remaining arms jumping back to it, and no form tried here changes
// that layout; the plain and the fast score are the same (920), so the body is as it stands.
// M3 lane 4 r229 (fast with the three callers 0x2c41b0, 0x29a190, 0x29cbb0 and 0x2c0d00, 0x2c3030, 0x2c2060, 0x2c3550,
// 0x29c860, unchanged in every run; control 920 = plain, aligned 480, register-masked 172). KEPT, plain 917 (3676 B, frame
// 0x2cac as retail, aligned 366, masked 139): both qualifiers dropped (the actor pointer a plain NULL-initialised local,
// unit_index a plain parameter), the unit's flag byte at +0x10a viewed as a word bitfield (retail's `mov dl; shr dl, 2; test
// dl, 1`) and the object type byte read at each of its seven uses; the entry then reads the actor's unit field as retail
// does (`mov eax, [index + data + 0x18]; add index, data`) and stores the actor home once. Steps (fast): the actor through a
// file-local inline getter with a NULL-initialised result variable, called at the declaration 965 (3675 B, aligned 417), in
// the block 980, with the unit field through its own lookup 932, with unit_index plain 945 (aligned 424, masked 153); the
// plain actor local alone 974 (aligned 414, masked 147), with unit_index plain 972; the word bitfield alone on main's body
// 930 (masked 170); getter + plain unit_index + word bitfield 925 (3687 B); that with the type byte at each use 910 (3692 B,
// aligned 378, masked 150; the getter is a one-caller helper, so not kept), with the getter called after the three bools 918
// (aligned 365, masked 142). On the kept form: self_unit declared ahead of the four shorts 917 (same code); the unit and
// ignore index tests as two statements 917 (same code). Retail's type-0 arm passes `false` to 0x25d810 from the register
// that holds the type (`movsx eax; test eax, eax; ... push eax`): the four arm compares share one sign-extended load. That
// form (a long type local for the four arm tests, the byte read at the three later uses) 949 (3692 B); the kept form with the
// flag byte back as a byte bitfield 968 (the word bitfield is what holds 917).
// Scoped motion 926 reduced, scoped priority and enabled 926 reduced, scoped physics indices 926 reduced, scoped
// object flags 926 reduced, scoped motion and priority 926 reduced, scoped motion and flags 926 reduced, scoped
// early classification 926 reduced, all separate scopes 926 reduced. NOT kept: All eight scope forms tie the
// reduced control at 926, with the same 0x2cb4 frame and 3718-byte extent.
// M3 lane 5 r216 (plain; [masked, exact] aligned, base 925 [141, 403]): lane 13 r121's declared order (previous_direction
// after direction) re-measured 933 [136, 394] (3714 B; 0x2c41b0 675 -> 661); `else straight = true;` on the no-actor path
// (retail's second store of 1 at 0x2c0e9a) 906 [137, 325] (3724 B; lead, a redundant store); both 914 [132, 319]; one `bool
// enemy` for both type-0 arms 925 (same code); straight with no initialiser: set first in the actor block plus the no-actor
// `else` 921 [137, 325], set just before the previous-direction test plus that `else` 921, set in an `else` of the
// previous-direction block and in the no-actor `else` 849 [134, 322] (3708 B, frame 0x2cac and retail's homes for the four
// shorts, self_unit and actor; lead only: the initialiser moved into else arms). Retail's type-0 `else if` arm and the type-1
// arm end in their own `sete`/`mov byte ptr [esp+0x12], 1` (skip flag in memory), where ours keeps it in BL.
// M3 lane 5 r218 (fast with the three callers 0x2c41b0, 0x29a190, 0x29cbb0 and 0x2c0d00, 0x2c3550, 0x29c860, 0x2c3030, 0x2c2060,
// none of which moved; with that set the fast control is the plain 925, with this function alone the image differs, 920 at 3740
// bytes; [aligned, masked] = difflib counts with addresses masked / registers and stack offsets masked too, control [519, 236]).
// Read from retail: 0x2c0e9a is a second store of 1 to `straight`, reached only when actor_index is NONE; the type-1 arm stores the
// skip flag on the passing path only (`je` / `call 0x1df560` / `jne` around one `mov byte ptr [esp+0x12], 1`, no store on the
// other path); the definition's no-obstacle bit is tested as `mov cl, [eax+2]; shr cl, 3; test cl, 1`; the vehicle block compares
// `[object+0x248]` with NONE in memory and loads it again for the lookup, and tests the vehicle's actor for NONE once, reading its
// +0x5d0 byte through the folded address before it forms the pointer; the byte slots follow reference counts, with the skip flag
// in memory at +0x12 between `aligned` and `straight`.
// Not kept (m3 review: a field reread at each use and a repeated lookup are match-only; code back to main), 859: the definition's flag view as a word bitfield (the field's declared width; retail's shift-and-test), the vehicle index
// read from the object at each use, the vehicle's actor through actor_get at both field reads (no NULL-initialised pointer, one
// NONE test). Those three with the vehicle's actor as a named pointer used twice 927; the first two alone 923.
// LEAD 514 (m3 lane 5 r218): the 859 form plus `else straight = true;` after the actor block and the type-1 arm's skip flag set
// by an `if`, both match-only; with all five the frame (0x2cac), the entry registers and the byte homes are retail's. Left on it:
// each lookup's scaled index (retail takes a fresh register), the i-term operand orders of the marker, vehicle and velocity dots,
// the sphere loop's XMM numbering and loop padding.
// The lead, step by step: `else straight = true;` 906 [419, 231] at 3724 B (frame 0x2cac and retail's homes for the four shorts,
// self_unit, actor and straight); on it the type-1 arm as `if (team == ... || !function_1df560(...)) skip_classification = true;`
// 708 [203, 84] at 3772 B, first difference +0x91 (the entry registers and the skip flag's home at +0x12 are then retail's); with
// the word bitfield 568; with the vehicle index at each use 684; with the vehicle's actor through actor_get 692; those two 661;
// all five 514 [170, 77] at 3740 B, plain-confirmed 514. The five without the `else` store 847 (first difference +0x4a); without
// the type-1 `if` 914; the `else` store replaced by an `else` arm of the previous-direction test plus the no-actor `else`, no
// initialiser, 884; by `straight = true;` at the top of the actor block plus the no-actor `else`, no initialiser, 851 (without
// the type-1 `if` 940 and 859); the vehicle's actor as a named pointer on the five 537.
// On the lead: a long type local for the four arm tests (retail's `movsx eax` and the `push eax` that passes false to 0x25d810)
// 520 [154, 67] at 3756 B (it also turns the second marker dot's i-term to retail's unfolded form); `bool enabled = true;` with the
// per-object defaults (retail's `mov dl, 1` ahead of the settings branch) 507; the object distance test distance-first
// (`!(distance_squared <= r * r)`, retail squares the differences before it adds the radius) 520, the sphere test the same way
// 505, both 511 [164, 63]; type + enabled 513; type + enabled + both tests 519 [146, 51] (the closest structure found), with one,
// two or three redundant pairs around the entry dot's first product (diagnostic) 514 / 514 / 519; type + sphere test 520; type +
// both tests 526; enabled + sphere test 498; the object offset through vector3d_from_points3d 559 (it loads i, j, k, retail k, j,
// i), with the type 543, with enabled 552, with both 536; the offset assigned k, j, i 514 (emitted i, j, k); the distance declared
// ahead of the radius 514 (same code); the combined flags as `moving | (sphere_flags | obstacle_flags)` or with moving last 514
// (same code); the loop-top object lookup with the masked index in a local, through a header pointer, or as byte arithmetic in
// either operand order 519 on the type + enabled + tests form (same code each). On the `else` store alone: the long type 911, with
// the type-1 `if` 739. Retail's scaled lookup indices go to a fresh register each time (`lea edx, [eax+eax*2]` at the loop top,
// `lea edx, [eax+eax*4]` for the component, `lea ecx, [eax+eax*4]` for the sphere) with the table loaded after them; its sphere
// loop keeps the centre's z in XMM4 and 1.0 in XMM0 and subtracts k, j, i in that order.
// Actor unit through the object header 925; query unit through header 926;
// all object reads through headers 926; typed location 926; typed actor unit
// category 925; direction defaults in branch arms 945 or at actor-block entry
// 928; confidence and classification names 925. Both defaults forms recover
// frame 0x2cac but regress, still using EBX for the entry zero.
// Memberwise query location copy after the unit lookup 901, Whole query location through memcpy 925.
// NOT kept: the memberwise copy uses two word transfers where retail uses one dword.
// M3 lane 3 r287 (plain each; control 925, difflib-aligned 519 / register-masked 243): the definition's no-obstacle view as
// a word bitfield (the flags field's declared width) 924 (aligned 519, same layout); with it: both radius tests written
// distance-first (`!(distance_squared <= r * r)`) 924 (masked 331); one to three redundant paren pairs around the entry
// dot's first product 927 / 927 / 924; the two one-use `velocity` pointer locals dropped (the field cast passed to dot3f)
// 938 at 3692 B, aligned 393, masked 242: frame 0x2cac and retail's homes for the four shorts, self_unit and the actor
// (aligned 519 -> 393, a positional rise, so not kept); on that, the vehicle's actor tested through its pointer
// (`vehicle_actor && ...`) 937 (aligned 396). Retail reloads actor_index into EBX, self_unit into EBP and unit_index
// into EDI at the loop's back edge and reuses EBP as NONE inside the type-0 arm.
// M3 lane 11 r143 (plain checks; this function builds differently in the reduced image; control 925, difflib-aligned
// 519, register/offset-masked 286). Retail evidence: on the no-actor path retail stores straight = 1 again (0x2c0e9a,
// `mov byte [esp+0x13], 1`, falling into the join at 0x2c0e9f) although the prologue already stored it, so its source
// has an `else straight = true;` on the actor test with the initialiser kept. That else alone 906 at 3724 B (frame
// 0x2cac and the priority shorts, self-unit and actor homes all retail's; aligned 419, masked 281), a redundant store,
// so match-only and not kept; with the skip flag declared at function scope as well 906 (same code); with a second
// `else straight = true;` on the previous-direction test as well 849 at 3708 B (aligned 424, MiniMax E's code). The
// self-unit test written as `self_unit = unit[0xaa] == 0 ? unit : NULL;` 890 at 3741 B (aligned 510), but it compiles
// branch-free (setne/dec/and) where retail branches, so not kept; with the no-actor else 870 (aligned 415). Left on the
// 906 form: actor_index in EBP for retail's EBX with the zero in EBX for EBP (ours hoists the object-table global into
// ESI at the entry, retail loads it in each block), the skip flag in BL where retail keeps it at [esp+0x12] (sete to
// memory), and the other-actor branch's layout.
// Entry pool acquisition is still hoisted and the 0x2cb0 frame exceeds 0x2cac.
// Havok and movement pool accessors at the actor-unit lookup, location lookup,
// or both 925 each. Complete conditional unit selection with explicit or
// conditional-expression type arms 942 each at 3741 bytes.
// NOT kept: the accessor forms tie and the complete selections grow past retail.
// M3 lane 2 r128 (H2M_FAST with 0x29a190/0x29cbb0/0x2c41b0/0x2c0d00, which did not move; control 925 = plain; [aligned,
// masked] difflib counts): lane 5 r218's 514 lead rebuilt on today's file 514 [170, 77] at 3740 B (all five forms
// match-only). On it: the entry dot's k and j products written previous-direction first 514 (same code); the entry dot
// through the existing dot3f 503 [170, 74] at retail's 3731 B (only the sphere loop's XMM numbers and the padding move);
// on that the object's squared distance through length_sq3f 503 (same code); `bool enabled = true;` with the per-object
// defaults plus the sphere test written `!(squared <= r * r)` 498 [168, 64]; on that a long type local for the four arm
// tests 495 [150, 52] at 3745 B (retail's `movsx eax`/`push eax`). Left on 495: the object-table and component-table
// globals loaded ahead of the index where retail scales the index first (loop head, type 11, sphere node), the object
// distance radius computed first (retail after the three differences), the marker-loop and sphere-loop dot term orders,
// the location copy's scratch registers and the loop-entry `jmp` over padding. Body unchanged.
// M3 lane 3 r292 (H2M_FAST with 0x2c41b0, 0x29a190, 0x29cbb0 and 0x2c0d00, which did not move; control 925 = plain):
// the type-1 skip as `!(team != actor_team && function_1df560(...))` 925 (same code); the self-unit test with the
// vehicle field compared first 925 (same code); self_unit declared first 925 (same code); the type-1 `if (team ==
// actor_team || !function_1df560(...)) skip_classification = true;` alone 903 (3760 B). Diagnostics (semantics changed,
// reverted at once): the type-1 arm without its 0x1df560 call (`if (team == actor_team) skip = true;`, or `skip = team
// == actor_team;`) 883 at 3692 B, and the head is then retail's line for line up to +0x4a (frame 0x2cac, actor_index
// EBX, the zero and self_unit EBP, unit_index EDI, the object-table global loaded in each block instead of hoisted to
// the entry); the call alone (`skip = !function_1df560(team, actor_team)`) 879 and the call with (team, team) arguments
// 868, first difference +0xb each: the call in the type-1 arm is what moves the entry allocation, not the skip flag's
// store count. New form, match-only (a named bool used once): `bool enemy = team != actor_team && function_1df560(team,
// actor_team); skip_classification = !enemy;` (the type-0 arms' spelling) 873 at 3740 B, first difference +0x46 (left
// in the head: the 0x26c flag's byte home and the entry dot's operand order); with `else straight = true;` 755 (3756
// B); with that and a long type local for the four arm tests 750; with the entry dot through dot3f(direction,
// previous_direction) 741 (3745 B; previous first 743); all four 736 (3745 B). Left on 736: retail lays the shared
// distance tail out right after the other-actor branch and defers the type-0 `else if` branch and the type 1/7/11 arms
// behind it, the object pointer and index homes (+0x18/+0x1c), and scratch rotations. On the else-store base the listed
// `if` form (708) still beats this one (755). Body unchanged.
// M3 lane 2 r130 (h2m fast with 0x29a190, 0x29cbb0, 0x2c41b0 and 0x2c0d00; control 925 = plain): one `bool enemy = true;`
// ahead of the type-0 branches, assigned in each, with one `skip_classification = !enemy;` after them 924 (3741 B);
// `long object_count` 921 (3731 B); diagnostic (wrong code), the actor's unit read without the object table: still zero
// EBX / actor_index EBP, settings then in ESI, 943 (so the hoisted table is not the cause). LEAD, not kept (declarations
// moved, match-only): the four per-object defaults (inflation, skip and the two sphere flags) declared after the
// unit/ignore `continue` 875 at 3776 B: retail's entry (actor_index EBX, zero EBP, no hoisted object table, the six
// zero stores from EBP) with frame 0x2cb0 and bytes at +0x11/0x12/0x13/0x1b; with `else straight = true;` on the
// no-actor path 788 (3772 B, first difference +0x41); with `long object_count` 956; with the shared skip 924. On last
// round's 495 lead (all match-only) the moved defaults give 507.
// Role names for the separate actor/vehicle/sphere centers 925, actor/candidate displacements 925,
// marker offsets 925, sphere-size workspace 925, object/sphere indices 925, inflation and sphere radius
// 925, marker counts 925, and object distance/radius 925. Frame 0x2cb0 and the entry register choice stay.
// M3 lane 8 r220 (h2m fast with 0x29a190 and 0x2c0d00, both MATCH; control 925): reading of retail's head: EBP is the
// self-unit pointer (NULL, so the six zero stores come from it, and it holds self_unit at the loop's 0x3dc test), EBX the
// actor index and in the other-actor branch the `enemy` byte, with the skip flag in a stack byte; ours keeps skip and
// enemy together in BL, the actor index in EBP and a separate zero in EBX. Forms: the type-1 arm and the vehicle block's
// identical team test through one file-local inline friendliness helper (two callers) 963 (3676 B); the other actor's
// 0x5d0 flag and 0x5ec facing through the typed actor-view fields 925 (same code); every `actor` null test in the loop as
// `actor_index != NONE` 947 (3772 B); the reference test as `if (actor)` 921 (retail compares the index with NONE there:
// `cmp ebx, ebp` with EBP = -1, so not retail's form); the actor and self-unit pointers declared ahead of the four
// priorities 925 (same code). Body unchanged.
// M3 lane 11 r151 (H2M_FAST with 0x29a190 and 0x2c0d00, both MATCH; control 925): in the loop retail gives EBX to
// actor_index and EBP to self_unit (both reloaded on the back edge, EBP then reused for NONE in the type-0 block) and
// keeps the skip flag in a stack byte; ours gives BL to the skip flag (also the zero for the device/restricted defaults)
// and EBP to actor_index. Forms: the skip flag declared at function scope and reset per object 925 (same code); declared
// just before the type tests 922 (3725 B; skip leaves BL and EBX goes to object_index); declared right after the
// unit/ignore continue 917 (3760 B); inflation, skip and device defaults declared at the type tests 919 (all three are
// declaration moves, match-only); the type-1 arm with a named `short actor_team` 925 (same code); the self-unit vehicle
// compare with the field first 925 (same code); `bool enemy = false; if (actor) enemy = ...` 932 (3741 B). Body unchanged.
// More r151 forms (same control): the skip flag declared last of the four defaults 921, first 925, just before the
// hidden-object test 917 (3760 B; with the declaration after the unit/ignore continue the entry is retail's actor_index
// EBX and zero EBP, frame 0x2cb0); the type-0 other-actor `if (!enemy) skip = true;` 932 (3741 B); the type-1 arm as
// `if (team == actor_team || !function_1df560(...)) skip = true;` with the late declaration 903 (3760 B, the type-1 form
// alone also scores 903). All declaration moves or a flag set in a test: match-only.
// Three-level direction result, strict-first, broad-first, conditional and reversed levels
// 909/903/909/898. NOT kept: retail stores two separate bytes and the best form shortens to 3712.
// Both dot tests after location/query 934/931; direction scope after settings 928,
// optional prior-direction pointer 940. The frame and actor/unit homes still differ.
// Frame 0x2cb0 versus 0x2cac; actor index and self-unit zero still exchange registers.
// Raw tracked-unit index with entry default, two complete arms, or conditional selection after actor setup:
// 914/921/874 in both filter orders. Early raw capture with a later Boolean result: 921 in both orders.
// NOT kept: the raw forms compare a dword with NONE at the sphere filter where retail tests a saved byte.
// The 874 form grows to 3744 bytes. Late Boolean materialization grows the frame to 0x2cb4.
// M3 lane 1 r154 (H2M_FAST with 0x2c0d00 and 0x29a190, both MATCH; control 925): the definition no-obstacle view as a word
// bitfield alone 924 (retail's shift-and-test, re-measured); the type-0 other-actor test inverted (NONE arm first, the
// other-actor branch in the else) 914 (masked aligned 125 -> 179: not retail's layout; an arm swap, match-only); the vehicle
// block's dot as one expression 924 (3741 B); the vehicle dot through vector3d_from_points3d and dot3f 922 (3740 B); both
// marker-loop up dots as dot3f(&offset, &up) 923 (3741 B). Body unchanged.
// M3 lane 15 r132 (H2M_FAST with the three callers and 0x2c0d00/0x2c3550/0x29c860/0x2c3030/0x2c2060; control 925 = plain;
// [aligned, register-masked] difflib counts, control [519, 491]): LEAD, not kept (a local copy at the four arm tests with the
// field read directly at the three later uses is the reload mix, match-only): the four type tests through one `long type`
// local (retail 0x2c1020 `movsx eax, byte ptr [esi+0xaa]; test eax, eax`, the chain `cmp eax, 1 / 7 / 0xb`, and `push eax`
// passing the known-zero type as 0x25d810's false; retail re-reads the field at the later uses) 916 [507, 491], plain 916.
// Tried: the type local at the three later uses too 920 [484, 455] (under the gate), with the object-flags casts 920, with
// the velocity pointers dropped 923; self_unit as `unit[0xaa] == 0 ? unit : NULL` (redundant
// NULL arm, match-only) 890 on main, 888 on 916 [508, 409] (self_unit then gets a register for the loop as in retail's EBP);
// the two one-use velocity pointers dropped 940 on main (3692 B), 918 on 916; with the ternary 868 / 856; the object-flags
// pointer as the cast at both uses 916 on the 916 lead (tie); the entry dot through dot3f(direction, previous_direction) 924
// (3714 B) on it; the type-1 `if (...) skip_classification = true;` 903 (3760 B) on it, with the ternary 934. Retail lays
// the type-0 `else if` branch and the 1/7/11 chain after the motion-disabled `jmp` (0x2c129e), each jumping back to the
// distance test 0x2c11ad. Body unchanged.
// M3 lane 15 r134 (H2M_FAST with the three callers 0x29a190/0x29cbb0/0x2c41b0 and the five callees above: without the
// callers it builds 3741 B; control 925 = plain, [aligned, masked] [519, 236]): `straight` set true in the two else arms
// (MiniMax E) re-measured 849 [424, 234]; the r132 `long type` local re-measured 916 [507, 236]; both together 887 (3724
// B) [413, 235]; both with `straight = true` also kept at the declaration 849/887 (same code: that store is dead). 1, 2
// or 3 unused locals at the top (diagnostic) 925/925/934: the actor index keeps EBP and the zero EBX (not a symbol-count
// tie). Retail also reloads the object-table global in the actor and location blocks, where ours loads it once into ESI
// at the entry. Body unchanged.
// M3 lane 13 r147 (H2M_FAST with the three callers and 0x2c0d00/0x2c3550/0x29c860/0x2c3030/0x2c2060, unchanged in every
// run; control 925, difflib-aligned 172 groups): the entry dot written previous-direction first in all three terms 925
// (only the i term's loads swap, to previous first; the k and j terms load the previous direction first in both
// spellings, where retail loads the direction first in all three); local_4d3867's store moved ahead of the
// previous-direction block 931, after the self-unit test 928; both of those ahead of the previous-direction block 935;
// the actor-index test read once through the parameter's address 925 (two other dots swap operand loads: the
// load sides follow the symbol count, not the written order). Re-measured on today's body, lane 13 r121's retail-evidenced
// parameter order (previous_direction after direction; definition, four declarations and three calls): 933 (3714 B),
// 0x2c41b0 675 -> 661, the matched callers unchanged; the entry dot then loads the direction first in all three terms
// as retail, but the zero/actor-index registers and the frame stay ours, so the body still rises. Body unchanged.
// M3 lane 16 r178 (H2M_FAST with the three callers and five callees as above, fast control 925 = plain): retail stores 1
// to `straight` three times: the entry store, the dot test's own `mov byte ptr [esp+0x13], 1` ahead of its `ja`, and the
// no-actor store at 0x2c0e9a. The 849 form (no initialiser, set in the previous-direction else and the no-actor else,
// re-measured 849, frame 0x2cac) has the entry store (hoisted from the previous-direction else) and the no-actor store, but
// its dot test stores only the 0. On that 849 base (match-only): the type-1 arm as `if (team == actor team ||
// !function_1df560(...)) skip_classification = true;` 888 (3756 B); self_unit as `unit[0xaa] == 0 ? unit : NULL` 878 (3740
// B); the skip flag declared just before the type tests 941; both of those skip forms 888; local_4d3867 also without an
// initialiser, cleared in the no-actor else 922 (3724 B).
// M3 lane 13 r149 (H2M_FAST with the three callers and five callees as above, unchanged in every run; control 925): retail
// computes the object offset k, j, i and the radius last, ours the radius first. The radius declared after the distance
// sum 925 (same code); the offset through the existing vector3d_from_points3d(position, center, &offset) 925 (then i, j,
// k, further from retail); the actor's unit read straight into self_unit and set to NULL when its type byte is not 0 932
// (3740 B: frame 0x2cac, retail's straight/aligned byte homes and the 0x26c field loaded into a register as retail, but
// actor_index in EDX and the zero in EDI); the entry declarations as actor, self_unit, the three flags, then the four
// shorts 925 (same code but two rows); both 932. Body unchanged.
// M3 lane 12 r183 (h2m fast with the three callers and five callees as above; control 925, aligned distance 657; body
// unchanged). Frame reading: retail's frame is four bytes smaller because self_unit lives in EBP through the loop and the
// skip flag in a stack byte that packs with the other six flags; ours keeps the skip flag in BL and self_unit in a
// four-byte home. LEAD, not kept (retail stores 0 to the restricted flag ahead of the unit/ignore test and 1 under the
// bit test, so this is not its source, and the declaration moves): `bool restricted_spheres = TEST_FIELD_BIT(...)`
// after the unit/ignore `continue` with `if (!restricted_spheres && <no-obstacle bit>) continue;` 886 at 3744 B (aligned
// 597): the entry is then retail's instruction for instruction (actor_index EBX, zero EBP, six stores from EBP, three
// flag bytes) with frame 0x2cb4. Forms: the device flag as one boolean expression 915 at 3760 B (aligned 592; retail has
// the conditional store there, so not kept), both 920; the self-unit test as a nested `if (self_unit) { if (...)
// continue; }` 925 (same code); the self-unit test ahead of the hidden-object test 923; the actor's unit type test with a
// `(char)` cast 925.
// The entry still reserves an extra dword and the optional unit competes with the classification byte.
// Other-actor classification saved before geometry 928, both type-zero results saved early 946,
// explicit actor guard 945, separate same-team branch 946, late classification declaration with both early
// stores 901, conditional actor relationship 936, optional unit with the first or both early stores 916/929.
// NOT kept: every frame remains 0x2cb0. The 901 form lengthens to 3760 bytes and moves the declaration and stores.
// One three-state direction result, strict-first, wide-first, conditional or default-preserving:
// enum 909/903/909/923, byte 913/905/913/912. NOT kept: the best enum retains 0x2cb0.
// The byte forms recover 0x2cac but combine two independent retail byte stores and leave the entry register exchange.
// The 0x2cb0 frame and exchanged actor-index and zero registers remain.
// Separate marker arrays at the first, second, first/second, third, first/third, second/third
// or all three query scopes, and all three at object scope: 925 each. NOT kept: the frame
// and score are identical to the control.
// M3 lane 15 r144 (plain-confirmed; screens with the three callers and five callees in the image, control 925). Reading of the
// allocator's ranking: on main's body the whole-function pass ranks the skip flag at -62, two points above the sphere-physics
// pointer at -64, so the flag is coloured first and takes EBX for the loop body; the actor index then falls to EBP, the self-unit
// pointer stays in memory and the zero takes EBX. Once the physics pointer ranks above the flag, the flag gets no register
// (retail's stack byte), the actor index takes EBX and the self-unit pointer and the entry zero EBP, and the frame is retail's
// 0x2cac. Retail's block order follows from the same residence: with the flag in memory the unit arm's and the type-1 arm's
// exits end in one shared "reload the object" stub in front of the distance test, and that stub and the test are laid out behind
// the first unit branch (the remaining arms follow the test's first unconditional jump). NOT KEPT (m3 review: the flags local copies a field retail tests in memory at each site, so it stays a byte-match-only lead; code back to main), 859 (3732 B, first difference
// +0x41): `word filter_flags = sphere->flags;` for the three sphere filter tests (a local used three times; it lifts the physics pointer
// to -56; alone 916), the entry dot through the existing dot3f(direction, previous_direction) (on it 902), the vehicle index read
// from the object at both uses (retail `cmp dword ptr [object+0x248], -1`, then `mov eax, [object+0x248]`; with the flags local
// 874, all three 859), and four hand-written helper bodies replaced by the existing helpers at the same score (both marker
// left-axis dots and the other actor's facing dot through dot3f, the object's squared distance through length_sq3f). Without the
// flags local the other two give 935 and all of them 933 (the ranking does not flip); the vehicle index alone 927. Ties on the
// kept body, not kept: the definition flag view with word storage, the sphere test as `!(squared <= r * r)`, `bool enabled` with
// the per-object defaults, `object_index != settings->unknown18`, 859 each; one to eight unused locals 859 each (same code).
// Tried on main's body: the type-1 arm's team read at both uses 925 (same code), in the vehicle block too 934; the object-flags
// pointer dropped, the type-7 arm without its definition local, the type-11 component index at both uses, the model index at
// both uses, the self-unit compare field-first, 925 each (same code); the object centre through a named point pointer 925; `long
// sphere_count` for the two count tests 922. Tried on the kept body: a `long type` local for all seven type reads 884; the two
// velocity pointers dropped 868; dot3f(previous_direction, direction) 861; `long object_count` 912; the vehicle dot through
// vector3d_from_points3d and dot3f 881; the other actor's offset through vector3d_from_points3d 867. The location copy through a
// named pointer, a by-value inline getter, memcpy, a temporary aggregate or a named unit pointer (on the old 495 lead) 837, 495,
// 495, 495, 495: retail's copy keeps the object in EDX and the two dwords in EAX and ECX, ours EAX and EDX, EAX. LEADS on the
// kept body (forms kept only with a byte match, so not kept): `else straight = true;` on the no-actor path 746; the type-1 arm
// as `if (...) skip_classification = true;` 846 (the flag's byte home then drops under `aligned`, retail's +0x11/+0x12 order, and
// the distance test moves up as in retail); both 674; a `long type` local for the four arm tests 855; the old 495 lead's eight
// forms with the flags local 496.
// Complete per-type skip result with direct or separate team case 931/899; positive classification
// result 865/875. Positive entry default with direct or separate team case 843/925. Complete byte
// skip/positive results 931/865. NOT kept: the positive default grows to 3744 bytes. The 3728-byte
// positive per-type form changes the retail skip stores. The frame remains 0x2cb0.
// The actor and optional unit still compete with the classification byte in the 0x2cb0 frame.
// Read-only physics pointer/reference, positive count guard and that guard on the reference
// 925/925/925/925. Existing tag block as members/reference/positive members/positive reference
// 925/919/925/919. NOT kept: both block references move the actor to EBX but grow to 3762 bytes
// and retain the extra frame word.
// The optional unit and classification flag still compete for a register, leaving a 0x2cb0 frame.
// Keep center/query/both heights at the vertical tests 924/903/903, or both in reverse
// declaration order 903. Named bounds with no height/center/query/both 923/913/877/877.
// Plain query height 910, center with bounds 934, query with bounds 888.
// NOT kept: both plain gains move farther from retail length and retain the extra frame word.
// Marker query admissions computed before object collection (entry/side/extra/entry+side/entry+extra/
// side+extra/all) 927/934/938/949/935/945/951, prefiltered marker bits 942. NOT kept: entry,
// side and masked forms recover 0x2cac with higher scores. Actor/zero registers remain exchanged.
// Gemini lane GB r6 (2026-11-25, 46 trials, control 925 = plain, aligned 415, masked 262): 858 at 3724 B (retail's frame
// 0x2cac, first diff +0x41, aligned 313, masked 191) from `bool enabled = true;` declared ahead of the classification test
// (retail stores 1 in dl before that branch, at 0x2c120d) and both radius checks written distance-first (`!(d <= r*r)`), with
// both one-use `velocity` pointers dropped, s_obstacle_definition_flags_2c as a word bitfield (its declared width in
// s_object_definition) and the four inner dots/length_sq through dot3f/length_sq3f. NOT kept (coordinator review): the
// moved declaration and the flipped compares are kept only with a byte match, and the other changes alone score 927.
// Lane readings: `enabled` alone 917 (0x2cb0, aligned 311), + entry `dot3f` 903 (3749 B, 0x2cb0), + first `velocity`
// dropped 857 (3731 B, 0x2cac), + both `velocity` dropped 858 (3724 B), + entry `dot3f` 868 (3713 B); direct
// `physics->spheres[s]` 931 (3786 B, 0x2cac, +0x41), + `dot3f` 917, + clean combo 893 (3759 B, 0x2cac); `switch (type)` 899
// (3760 B, jump table); sphere flags bitfield 924 (3731 B); hybrid direct/local sphere 944. Match-only leads: the 858 form
// + entry `dot3f` + type-1 `if` + `else straight = true;` 389 (3731/3731 B, +0x91, aligned 219, masked 69); that + the
// vehicle index re-read + `long type` at the four arms + inner helpers 528 (3731/3731 B, 921/921 insns, aligned 146,
// masked 46). Left on that path: retail's no-actor `straight = true;` store (0x2c0e9a), the type-1 single-store `if`,
// `long type` at the four arm compares, the `vehicle_index` re-read.
// Reduced control 845 at 3731 bytes. Immutable direction dot, immutable typed actor unit,
// typed candidate position, earlier-center difference helper, immutable query count/index,
// shared candidate getter, and separate short ordinal 845 each. Candidate difference helper 841.
// Straight default from heading availability 842, from actor absence 778, from heading availability
// inside the actor arm 834, both defaults from heading availability 936. NOT kept: the candidate
// helper gains four against the reduced control. The actor-absence form retains frame 0x2cac
// but grows to 3736 bytes and exchanges the actor/unit registers. The other default forms grow
// to 3752 bytes. The no-actor byte store and early dot operand loads still differ.
// GB r13 (2026-11-25, 21 real trials + 3 dead-local diagnostics, fast with 0x29a190/0x29cbb0/0x2c41b0/0x2c0d00, control 838 at 3731 B, fr=0x2cac, al=180, mk=72):
// clean `dot3f(direction, vel)` in vehicle block 838 (mk 66); + `!(d <= r*r)` both distance gates, `dot3f(&difference, direction)`, sphere `dot3f(direction, vel)` and `(sphere_flags | obstacle_flags) | moving` 832 (al 178, mk 52; NOT kept: -6 under gate); vehicle dot via `vector3d_from_points3d` 846.
// Match-pursuit forms (with `else straight = true;`, early `bool enabled = true;`, dropped `vehicle_index` reread, `!(d <= r*r)` gates, direction-first dots): without `previous_direction` move 528 at exact 3731/3731 B (921/921 insns, al 133, mk 38; all 3 marker loops, vehicle tail and object-distance gate 100% byte-exact); with `previous_direction` at slot 9 (`0x2c41b0` 675 -> 661) 524 (+0x157, first 343 B byte-exact), + `moving |= (sphere_flags | obstacle_flags)` 523 (mk 42, fixes sphere moving dot), + `basis_dot_xzy` 390 (921/921 insns, 3738 B), + sphere `vector3d_from_points3d` 485 / flat squared 483, + explicit `dx/dz/dy` 501, + split `s_location` fields 842, + inverted `unit_index == NONE` 548; 1..3 dead locals diagnostic all 523 (confirms `+0x157` scratch `eax/edx` rotation is not symbol-count phase). Body unchanged.
// M3 lane GA r11 (fast with 0x29a190/0x29cbb0/0x2c41b0 and 11 file peers, all matched peers MATCH; control 838 at
// 3731/3731 B, +0x4a, fr 0x2cac; 18 compiled trials): nested object/sphere `d <= r*r` gates + `dot3f(&difference,
// direction)` + `(sphere_flags | obstacle_flags) | moving` 832 (al 196, mk 60); + vehicle x-z-y dot 832 (mk 58),
// + `direction`-first vehicle velocity dots 832 (al 194, mk 52; NOT kept: -6 under bar), + combined height `||` 832 /
// nested height + swapped center/radius decl + explicit `(j+k)+i` moving dot 835; + reversed entry dot 834; + `basis_dot_xzy`
// vehicle dot 859 (3713 B); + sphere `vector3d_from_points3d` 862 (3713 B); vehicle x-z-y alone 838 (mk 98); velocity
// dir-first alone 838 (mk 94). Match-only lead (`else straight = true;` + early `enabled` + inline `object+0x248` +
// `object_radius` first + nested gates + x-z-y + dir-first vel + moving dot/flags) 509 (3731 B, +0x91, al 126, mk 37),
// + one-expr vehicle dot 509 (al 125, mk 42), + `basis_dot_xzy(direction, ...)` 468 (mk 64), + height `||` / nested 509,
// + split `combined_flags` / swapped decl / nested type-11 510 (mk 38). Body unchanged.
// M3 lane 13 r159 (H2M_FAST, control 845 there: the reduced image builds this function differently from the full build,
// 838): slot map: ours keeps the actor pointer at +0x20 and the three entry flag bytes at +0x25..+0x27, retail a one-use
// dword at +0x20 sharing with flag bytes at +0x21..+0x23 and the actor at +0x24. The previous-target flag renamed
// (eight names sorting before and after the other locals) 845 each (same code: names do not place these homes); the three
// entry flags declared first 852; that flag declared ahead of the self-unit and actor pointers 848; the actor pointer
// declared ahead of the self-unit pointer 846; the flag left uninitialised and set false in an else of the actor test 819
// (3745 B).
// M3 lane 16 r192 (H2M_FAST with 0x2c41b0 and 0x29cbb0; control 838, difflib-aligned 180, registers and stack
// offsets masked 72: the positional count is mostly the one-row shift of the missing no-actor store): the object's
// position (+0x30) and velocity (+0x88) read through the existing s_ai_object view 838 (same code); `dword moving` 838
// (same code); the combined flags as `moving | (sphere_flags | obstacle_flags)` 838 (same code); the object gate as
// `if (distance_squared > object_radius * object_radius) continue;` 838 (aligned 188, masked 67); the sphere gate as
// `if (squared > combined_radius * combined_radius) continue;` 833 (masked 65), both gates 833 (masked 60); either gate
// as `r * r < d` 838. Diagnostics: 1..7 unused function-scope locals 717/717/862/862/862/862/843 (masked
// 75/77/83/83/76/76/66, no phase near retail); 1..3 redundant float parentheses on the entry dot or on the type-0 offset
// change nothing. Body unchanged.
// M3 lane 11 r172 (reduced image with the callers 0x29a190, 0x29cbb0 and 0x2c41b0, control 838 as the plain check): the
// device flags read at each use 718 (kept; aligned 180 -> 165; with one unused local added 720, and another local dropped
// instead of it 838, so the gain is the form, not the local count); the vehicle dot as one x-first expression 838; the
// sphere squared distance as one expression 836 (718 on the kept form); the combined flags passed directly 834 (863 at
// 3713 B on the kept form); the moving test as one condition 838. On the kept form, the other-actor index, the type-1
// team, both component indices, the model index or the vehicle team read at each use 863 each (3713 B, one code for all:
// a local-count phase); the vehicle index at both uses 857 (3713 B, aligned 140); the vehicle actor index at each use 813
// (the frame changes). The match-only lead (no-actor `else straight = true;`, early `enabled`, the vehicle index at both
// uses, direction-first dots, `!(d <= r*r)` gates) rebuilt on main 528.
// GB22 (22 fast trials with 0x29a190/0x2c0d00 MATCH, control 838 at 3731/3731 B, +0x4a, body unchanged): uninitialized
// `bool straight;` with inner & outer `else straight = true;` 756 (3731 B, +0x3c, mk 111; NOT kept: drops retail's
// `0x2c0e19` store; outer both 743, both uninit 921 at 3745 B); init + `else { straight = true; aligned = true; }` 808
// (3745 B, +0x91); clean retail bundle (early `bool enabled = true;`, direct `*(long *)(object + 0x248)`, `!(d <= r*r)`
// gates, dir-first vel dots, `dot3f(&difference, direction)`, `(sphere_flags | obstacle_flags) | moving`) 838 (3731 B,
// mk 72 -> 44; early `enabled` + direct `+0x248` alone 838, mk 88; + uninit `straight` 882/880/919); on clean bundle:
// `vector3d_from_points3d` obj/veh/sphere+flat/all3 856 each (3713 B), `switch (type)` 868 (3748 B), one-expr x-z-y
// vehicle dot 840 (mk 44). On `528` (`else straight = true;`, 3731 B, 921/921 insns, al 142, mk 37): `vector3d_from_points3d`
// obj+sphere flat 529, veh+sphere ijk 527, one-expr x-z-y + obj + flat kji 490 (921/921), + `bool moving` 488 (920/921).
// M3 lane 2 r156 (h2m fast with 0x29a190 and 0x2c0d00 MATCH, 0x2c41b0 675; control 718, 165 of 921 rows differ after
// alignment): the entry dot's operands follow rank (the ECX parameter's loads outrank the direction's). Forms: the
// object's squared distance named ahead of its radius sum 718; the classification test as `object_index !=
// settings->unknown18` 718 (both the same code); the sphere add with the non-blocking arm first and the combined flags
// declared in it 755 (3724 B); the combined flags declared in the non-blocking arm only 748; the self-unit pointer chosen
// by a ternary 887. LEAD (a redundant store, kept only with a byte match): `else straight = true;` on the no-actor path
// re-measured on this base 544 (+0x91, retail's store and jump). All restored.
// M3 lane 14 r163 (plain check unless marked; the reduced image builds 0x25d810 differently and drops the NONE register,
// so its scores here run far off: 675 fast against 405 plain for one form). With the no-actor `else straight = true;`
// (match-only): `straight` declared without a value and set true first in the actor arm and in the else 847 (fast); the
// actor pointer declared after the three flags 784 (fast); the type-7 device flags in one named two-use local again 405
// (3731 B, first +0x91: the head is retail's; left the unit location copy's registers, retail object pointer EDX and
// halves EAX/ECX, the object query's argument registers, the loop head, and the first marker loop, where retail swaps
// position/direction EBX/EBP and jumps over the loop-head pad); that plus the object radius ahead of the offset 411.
// All restored.
// GB26 (24 fast trials with 15 peers/callers, all 12 matched peers MATCH, control 718 at 3731/3731 B, +0x4a, mk=104):
// nested object/sphere `d <= r*r` gates 718 (sphere alone 718 mk=86, obj alone 718 mk=82, both 718 mk=64); retail
// direction-first vel dots + `(sphere_flags | obstacle_flags) | moving` 715 (mk=100); both together 715 (3731 B,
// 921/921 insns, mk=60, keeping `dot3f(direction, &difference)` 715, + `object_radius` first 715; NOT kept: -3 under
// bar); uninit `straight` + top/inner + outer `else straight = true;` 722 (+0x3c, VC hoists store ahead of branch and
// drops 0x2c0e9a; both elses 865); direct marker index 899 (3793 B); marker dir-first 718 (mk=246); combined/nested
// sphere height and sphere radius decl 718 (same code); one-expr vehicle dot 720; `s_ai_object` pos 720; `bool moving`
// 751; direct `object+0x248` + nested/dots 857 (3713 B, local-count phase; + uninit `straight` + early `enabled` 860).
// Match-pursuit on 718: nested/dots + `else straight = true;` 550 (3731 B, +0x91, mk=54); + early `enabled` 849 (+0x1,
// 3750 B); + direct `object+0x248` 816/818 (+0x1, 3732 B). Body unchanged at 718.
// M3 lane GA r28 (22 fast trials with 0x29a190/0x29cbb0/0x2c41b0 and 12 file peers, all 11 fast-matched peers MATCH;
// control 718 at 3731/3731 B, +0x4a, frame 0x2cac, aligned 159, masked 68): on 718 base, early `bool enabled = true;`
// 717 (mk 64); vehicle dot x-z-y 720 (mk 66); vehicle dot nested `if (dot > 0.0f)` 887 (fr 0x2cb0, al 142, mk 64);
// vehicle vel `dot3f(direction, vel)` 715 (mk 64); sphere vel `dot3f(vel, direction)` 716 (mk 66); sphere
// `dot3f(&difference, direction)` 718 (same code); clean retail bundle (x-z-y + dir-first vel + sphere diff-first +
// nested `d <= r*r` gates) 715 (mk 50), + early `enabled` 714 (-4 under keep bar, al 155, mk 46), + nested vehicle dot
// 884 (fr 0x2cb0, mk 46); delayed `ai_object_get` 852 (+0xe, 920/921 insns); location via `self_unit` 718 (same code);
// `sphere_node == NONE` 786 (3727 B); reversed entry dot 720; nested vehicle dot + direct `+0x248` reread 857 (3713 B).
// Match-pursuit (`else straight = true;` + `word device_flags`): r163 405 check 405 (mk 65); + clean retail bundle 408
// (mk 47); + early `enabled` 401 (3731/3731 B, +0x91, al 166, mk 45); + nested vehicle dot 543 (mk 40); + direct
// `+0x248` reread 528 (3731/3731 B, +0x91, 921/921 insns, al 131, mk 36: only 7 small local sites remain); `>` gates on
// bundle 408 (mk 49); `else straight = true;` alone on 718 / on bundle 544 / 550 (mk 62 / 44). Body unchanged at 718.
// M3 lane 6 r253 (plain check with 0x29a190 and 0x2c0d00 MATCH, 0x29cbb0 378 and 0x2c41b0 675 unchanged in every run;
// control 718, of which 165 of 921 rows differ once the listing is aligned, in 72 places; the rest is the two-row shift
// of the missing no-actor store). Forms: the no-actor store spelled after the actor block, `if (!actor) straight =
// true;`, 544 (aligned 158; the same code as the listed `else` form, a redundant store, match-only); the unit's
// location through a new file-local inline getter with an out parameter 718, through a new inline accessor returning
// the field's address 718 (both the same code: the copy still has the object in EAX and the halves in EDX/EAX, retail
// EDX and EAX/ECX); re-measured on today's tree, the `else` store with the type-7 device flags in one two-use local
// 405 (aligned 172), that with the object radius ahead of the offset 411 (aligned 172). Back-end reading: the location
// copy's three values and the object query's argument loads are scratch temporaries handed out by a cursor that cycles
// EAX, ECX, EDX; ours stands one or two steps off retail's at the location block although every earlier instruction
// agrees, so the difference is in which earlier temporaries moved the cursor, not in these statements.
// @retail 0x2c0d60
void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
    s_path_settings const *settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
    point3f const *position, real radius, vector3f const *direction, long unit_index, long ignore_index)
{
    short ignore_priority = 0;
    short block_priority = 0;
    short range_min = 0;
    short range_max = 0;
    byte *self_unit = NULL;
    s_actor_view *actor = NULL;
    bool straight = true;
    bool aligned = true;
    bool local_4d3867 = false;
    if (actor_index != NONE)
    {
        actor = actor_get(actor_index);
        byte *unit = (byte *)ai_object_get(actor->unknown018);
        if (previous_direction)
        {
            real dot = dot3f(direction, previous_direction);
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
    s_location location;
    if (unit_index != NONE)
        location = *(s_location *)((byte *)ai_object_get(unit_index) + 0x28);
    else
    {
        point3f raised;
        vector3f_add_scaled((vector3f const *)position, g_4687b0, 0.05f, (vector3f *)&raised);
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
                        real dot = dot3f(&offset, other_direction);
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
                if (team == actor->unknown024 || !function_1df560(team, actor->unknown024))
                    skip_classification = true;
            }
        }
        else if (type == 7)
        {
            byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            if (!(*(word *)(definition + 0x11e) & 1) ||
                ((*(word *)(definition + 0x11e) & 2) && (*(real *)(object + 0x140) == 1.0f || !(object[0x1cc] & 1))))
                device_spheres = true;
        }
        else if (type == 11)
        {
            long component_index = *(long *)(object + 0xb4);
            if (component_index != NONE && havok_component_get(component_index)->unknown1c <= 1)
                continue;
        }
        vector3f offset;
        offset.i = *(real *)(object + 0x30) - position->x;
        offset.j = *(real *)(object + 0x34) - position->y;
        offset.k = *(real *)(object + 0x38) - position->z;
        real object_radius = *(real *)(object + 0x3c) + radius;
        real distance_squared = length_sq3f(&offset);
        if (!(object_radius * object_radius >= distance_squared))
            continue;
        s_object_definition const *definition = (s_object_definition const *)g_4e3b44[*(long *)object & 0xffff].bytes;
        if (TEST_FIELD_BIT(((s_obstacle_definition_flags_2c const *)&definition->flags)->no_obstacle))
            continue;
        s_obstacle_motion_definition_2c *motion = NULL;
        bool blocking = false;
        dword obstacle_flags = 0;
        dword sphere_flags = 0;
        if (settings->unknown18 != object_index && !skip_classification)
        {
            short priority = 0;
            bool enabled = true;
            if (definition->ai_properties.count > 0)
            {
                motion = (s_obstacle_motion_definition_2c *)definition->ai_properties.address;
                if ((motion->flags & 2) && *(real *)(object + 0xec) <= 0.0f)
                    enabled = false;
                else
                    priority = motion->priority;
            }
            if (priority == 0)
            {
                long component_index = *(long *)(object + 0xb4);
                if (component_index != NONE)
                    priority = havok_component_get(component_index)->unknown1c;
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
                else if (range_min <= priority && range_max >= priority && (char)object[0xaa] != 0)
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
                        vector3f offset;
                        vector3d_from_points3d(&marker->matrix.position, position, &offset);
                        real dot = dot3f(&marker->matrix.up, &offset);
                        if (dot >= -0.2f && dot < marker->unknown6c + 0.2f &&
                            fabs(dot3f(&marker->matrix.left, direction)) >= 0.9f)
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
                        vector3f offset;
                        vector3d_from_points3d(&marker->matrix.position, position, &offset);
                        real dot = dot3f(&marker->matrix.up, &offset);
                        if (dot >= 0.1f && dot < marker->unknown6c - 0.1f &&
                            fabs(dot3f(&marker->matrix.left, direction)) >= 0.9f)
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
            if (actor && actor->unknown26c != NONE && ((1 << (char)object[0xaa]) & 3))
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
                        if ((vehicle_actor_index != NONE && vehicle_actor[0x5d0] &&
                                dot3f((vector3f const *)(vehicle_actor + 0x5ec), direction) > 0.0f) ||
                            dot3f((vector3f const *)(object + 0x88), direction) > 0.0f)
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
        long model_index = definition->model_tag_index;
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
            if ((char)object[0xaa] == 0)
            {
                real dot = dot3f(direction, &difference);
                if (dot > 0.0f)
                {
                    if (dot3f((vector3f const *)(object + 0x88), direction) > 2.0f)
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
void function_2c2910(point2f const *points, point2f const *center, point2f const *previous, real radius, point2f *result)
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
    return (data->nodes[index].flags & 1) && (!(data->nodes[index].flags & 2) || !function_1fa6b0(data, &data->nodes[index]));
}

/* the same test of the node on one side of an edge, read as the edge's word */
PRIVATE __forceinline bool edge_node_open_2c(s_pathfinding_data const *data, s_pathfinding_edge const *edge,
    long side)
{
    word index = edge->nodes[side];
    if (index == NONE || index == (word)NONE)
        return false;
    s_pathfinding_node const *node = &data->nodes[index];
    return (node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(data, node));
}
/* the second parameter is never read: retail drops it from the call (six stack arguments), and its one
   caller still forms the argument (0x2c22ed) */
// @retail 0x2c3030
bool function_2c3030(s_pathfinding_data const *data, byte const *unread_source_field, point2f const *origin, real radius,
    long edge_index, bool side, point2f *result)
{
    long first_vertex = NONE;
    long previous_vertex = NONE;
    long current_edge = edge_index;
    for (;;)
    {
        s_pathfinding_edge const *edge = &data->edges[current_edge];
        bool first_open = edge_node_open_2c(data, edge, 0);
        long node_index = edge->nodes[!first_open];
        if (!path_node_open_2c(data, node_index))
            return false;
        s_pathfinding_node const *node = &data->nodes[node_index];
        if (previous_vertex != NONE)
        {
            short surface_index = *(short const *)(edge->unknown04 + 2);
            if (surface_index != NONE && data->surface_count > 0)
            {
                s_pathfinding_edge const *portal = &data->edges[data->surfaces[surface_index].index];
                long opposite = portal->nodes[portal->surface != node_index];
                if (path_node_open_2c(data, opposite))
                {
                    *result = *(point2f const *)&data->vertices[previous_vertex];
                    return true;
                }
            }
        }
        long from_vertex, to_vertex;
        if (first_open)
        {
            from_vertex = edge->vertices[1];
            to_vertex = edge->vertices[0];
        }
        else
        {
            from_vertex = edge->vertices[0];
            to_vertex = edge->vertices[1];
        }
        point2f const *start = (point2f const *)&data->vertices[from_vertex];
        point2f const *end = (point2f const *)&data->vertices[to_vertex];
        bool crossing = false;
        point2f direction;
        vector2d_from_points2d(start, end, &direction);
        point2f normal = { direction.y, 0.0f - direction.x };
        normalize_offset_direction_2c(&normal);
        point2f offset, other_offset;
        offset.x = origin->x + normal.x * radius;
        offset.y = origin->y + normal.y * radius;
        other_offset.x = origin->x + normal.x * (0.0f - radius);
        other_offset.y = origin->y + normal.y * (0.0f - radius);
        point2f first, second;
        vector2d_from_points2d(&offset, start, &first);
        vector2d_from_points2d(&other_offset, start, &second);
        if ((dot_product2d(&first, &direction) < 0.0f ? 1 : 0) == side &&
            cross_product2d((vector2f const *)&first, (vector2f const *)&direction) < 0.0f)
            crossing = true;
        if (cross_product2d((vector2f const *)&second, (vector2f const *)&direction) < 0.0f)
            crossing = true;
        if (first_vertex == NONE)
            crossing = true;
        long vertex;
        if ((crossing != first_open) == side)
            vertex = edge->vertices[1];
        else
            vertex = edge->vertices[0];
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
            short count = 0;
            long first_edge = current_edge;
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
long function_1fa7f0(void);
/* smooths a path: from each step it takes the furthest later step the source reaches in a straight line, or a
   point round the corner that blocks it. Retail evidence for the forms below: the corner query's second
   parameter is never read (0x2c3030 has six stack arguments), yet the caller forms query + 4 for it once and
   keeps it across the first query (0x2c22ed, 0x2c22f7); the edge is stored again where the scan finds a typed
   step (0x2c219e); the step's pointer is formed at the loop top and only read back from its stack home at the
   emit (0x2c2120, 0x2c255b), so the loop top reads the step through the array; the two corners are separate
   8-byte homes (0x2c2302, 0x2c2326); and the distance test is 0x210a30's body with 0x210850's inside it, which
   retail expands here (two point copies per conversion, one shared sum, 0x2c25b9-0x2c269d). */
// @retail 0x2c2060
void __stdcall function_2c2060(s_type_f17a25 *query, short count, s_path_step_view const *points,
    short *result_count, s_path_step_view *result, bool *complete)
{
    real source_radius = query->source.radius;
    real radius = source_radius + 0.05;
    s_pathfinding_data *data = (s_pathfinding_data *)function_1fa7f0();
    if (data)
    {
        if (count > 1)
        {
            s_type_c3b527 const *source = (s_type_c3b527 *)&query->source.point;
            short index = 0;
            short used = 0;
            bool copied_last = false;
            point2f origin = *(point2f *)&source->point;
            long sector = points[0].node_index;
            while (index < count)
            {
                long edge = 0xffff;
                short next = NONE;
                bool found = false;
                short link = NONE;
                short type = NONE;
                short output_index;
                s_path_step_view const *step = &points[index];
                if (points[index].type != NONE)
                {
                    next = index;
                    link = points[index].link_index;
                    type = points[index].type;
                }
                else
                {
                    short end = index;
                    while (end < count)
                    {
                        if (points[end].type != NONE)
                        {
                            next = end;
                            edge = 0xffff;
                            found = true;
                            break;
                        }
                        ++end;
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
                            &direction, source_radius, distance, false, candidate == count - 1, false, &query->location, &trace))
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
                    if (edge != 0xffff)
                    {
                        s_path_step_view const *next_step = &points[next];
                        if (next_step->type == NONE)
                        {
                            point2f left_corner, right_corner;
                            bool left = function_2c3030((s_pathfinding_data *)query->pathfinding, &query->source.unknown04,
                                &origin, source_radius, edge, true, &left_corner);
                            bool right = function_2c3030((s_pathfinding_data *)query->pathfinding, &query->source.unknown04,
                                &origin, source_radius, edge, false, &right_corner);
                            if (right && left)
                            {
                                point2f const *previous = next > 0 ? (point2f *)&points[next - 1].point.point :
                                    (point2f *)&source->point;
                                point2f const *endpoint = (point2f *)&next_step->point.point;
                                bool side = function_2c2a70(previous, endpoint, &left_corner, &right_corner, &origin);
                                short attempt = 0;
                                point3f target;
                                target.z = 0.0f;
                                point3f start = { origin.x, origin.y, 0.0f };
                                long location = (long)&query->location;
                                for (; attempt < 2; ++attempt)
                                {
                                    point2f center;
                                    if (side)
                                        center = left_corner;
                                    else
                                        center = right_corner;
                                    point2f tangents[2], candidate;
                                    function_2c2740(&origin, &center, radius, side, &tangents[0]);
                                    function_2c2740(endpoint, &center, radius, !side, &tangents[1]);
                                    function_2c2910(tangents, &center, &origin, radius, &candidate);
                                    target.x = candidate.x;
                                    target.y = candidate.y;
                                    s_path_trace_result first, second;
                                    function_26c4e0(data, (s_type_c3b527 *)&start, sector, (s_type_c3b527 *)&target,
                                        NONE, location, &first);
                                    if (!((s_sector_trace_result *)&first)->blocked &&
                                        !function_26c4e0(data, (s_type_c3b527 *)&target, ((s_sector_trace_result *)&first)->sector_index,
                                            &next_step->point, next_step->node_index, location, &second))
                                    {
                                        sector = ((s_sector_trace_result *)&first)->sector_index;
                                        origin = candidate;
                                        output_index = next_step->point.output_index;
                                        goto emit;
                                    }
                                    side = !side;
                                }
                            }
                        }
                    }
                }
                if (next != 0 && next != index)
                {
                    origin = *(point2f *)&points[next - 1].point.point;
                    output_index = points[next - 1].point.output_index;
                    sector = points[next - 1].node_index;
                }
                else
                {
                    origin = *(point2f *)&points[next].point.point;
                    sector = points[next].node_index;
                    output_index = points[next].point.output_index;
                    next = index + 1;
                }
            emit:
                s_path_step_view *out = &result[used++];
                out->node_index = sector;
                s_type_c3b527 *out_point = &out->point;
                real_point3d_set(&out_point->point, origin.x, origin.y, step->point.point.z);
                out->point.output_index = output_index;
                out->unknown02 = step->unknown02;
                if (type != NONE)
                {
                    out->type = step->type;
                    out->link_index = link;
                }
                else
                {
                    out->type = NONE;
                    out->link_index = NONE;
                }
                if (used >= 4)
                    break;
                real distance_squared;
                if (source->output_index == out_point->output_index)
                {
                    vector3f delta;
                    vector3d_from_points3d(&source->point, &out_point->point, &delta);
                    distance_squared = (delta.j * delta.j + delta.k * delta.k) + delta.i * delta.i;
                }
                else
                {
                    vector3f delta;
                    point3f start;
                    point3f end;
                    if (source->output_index != NONE)
                    {
                        if (!function_2104b0(source->output_index, &source->point, &start))
                            start = source->point;
                    }
                    else
                    {
                        start = source->point;
                    }
                    if (out_point->output_index != NONE)
                    {
                        if (!function_2104b0(out_point->output_index, &out_point->point, &end))
                            end = out_point->point;
                    }
                    else
                    {
                        end = out_point->point;
                    }
                    vector3d_from_points3d(&start, &end, &delta);
                    distance_squared = (delta.j * delta.j + delta.k * delta.k) + delta.i * delta.i;
                }
                if (distance_squared > 400.0f)
                    break;
                index = next;
            }
            *result_count = used;
            if (!copied_last)
                *complete = false;
        }
        else
        {
            *result_count = 1;
            result[0] = points[0];
        }
    }
    else
    {
        *result_count = 0;
        *complete = false;
    }
}
