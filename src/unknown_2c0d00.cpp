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

// score 920 (was 923): earlier 923 (was 923), 901 (was 901), 882 (was 882); frame 0x2cb4 versus 0x2cac; actor/unit retention and priority/query/marker homes remain.
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
                        vector3f offset;
                        vector3d_from_points3d(&marker->matrix.position, position, &offset);
                        real dot = dot3f(&marker->matrix.up, &offset);
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
                        vector3f offset;
                        vector3d_from_points3d(&marker->matrix.position, position, &offset);
                        real dot = dot3f(&marker->matrix.up, &offset);
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
                                dot3f((vector3f const *)(vehicle_actor + 0x5ec), direction) > 0.0f) ||
                            dot3f(velocity, direction) > 0.0f)
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
            if (type == 0)
            {
                real dot = dot3f(direction, &difference);
                if (dot > 0.0f)
                {
                    vector3f const *velocity = (vector3f const *)(object + 0x88);
                    if (dot3f(velocity, direction) > 2.0f)
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
// @retail 0x2c3030
bool function_2c3030(s_pathfinding_data const *data, point2f const *origin, real radius,
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
// MiniMax A (11-13): no new form, no lower score (400); the frame is still 0x114 against retail's 0x10c, the
// source radius still sits at +0x30 against retail's +0x2c, the expanded radius at +0x44 against retail's
// +0x34, and retail keeps the count in ebx where this build holds it in esi.
// score 400 (was 400): earlier 418; the shape is retail's (see round 27 below); registers and frame homes differ, 1752 bytes against 1746.
// the shape is retail's (see round 27 below); registers and frame homes differ, 1752 bytes against 1746.
// MiniMax E (11-10): no new form, no lower score (400); the frame is 0x114 here and 0x10c there, because the edge value and the other-side flag are kept in stack slots here and in ebx and bl there, which moves every other home by eight bytes.
// MiniMax lane E (11-01): reading the step through a pointer instead of copying it, as retail does,
// scores 490 (490 as well when the pointer is assigned in a second statement); no form tried
// lowered the 444.
// Earlier 444: frame 0x11c versus 0x10c, 1740 bytes against 1746. Ours copies the whole step
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
// Round 443: zero-extended word type with short/word/long/dword link results,
// at typed selection 493/493/492/492; at native emission with common stores
// 517/520/517/517. Complete record, signed domains and body restored.
// Round 479: native positioned-point subobject with short/long type-link
// combinations 477/488/477/476; native position subobject 474/471/473/471
// (fast control 444). Original complete record snapshot and body restored.
// Round 27 (lane 17, fast; aligned = difflib over the listing): body rewritten to retail's map, 444 -> 420 (aligned 442 ->
// 238, 1737 bytes, frame 0x10c; both volatiles gone: the source radius's alone cost 9, 411 with it, its `mov; mov; fld` needs
// another form): count > 1 arm first, a step pointer in place of the record copy, type/link locals set in the
// typed branch, the corner logic inside the untyped branch, a plain output index (its volatile removed), the corner chosen by
// if/else, target.z set once outside the attempts, a named location, the next side in its own bool assigned at the loop end,
// sector read before the output index. On the way: the rewrite alone 453 (1696, aligned 289), plus corner/z/location 429 (1727),
// no second `edge = 0xffff` in the scan 429 (aligned 268; retail has that store), plus next side 412, plus sector order 411. The
// first conversion nested as 0x210850's body 408 (1747, aligned 244; retail's two copies merge in the second, not kept: that
// helper's shape written out); without the type/link locals 482-492, without one of them 480/482 (with nesting 433/470); the
// scan as the compound `while (end < count && type == NONE)` with a test after it 456 (hence the break form). Registers
// left: retail zero EBX (ours EDX, 0xc4 count in ESI where retail EDX), y read first through the query (`mov edx,[ecx+0x18]`),
// points EDX, index EDI, step ECX then +0x84, NONE ESI, edge EBX, count never enregistered in the loop; ours points ECX, index
// EDX, step EDI, NONE EBX, edge in memory. Retail also stores query+4 at +0x64 (never read). Retail inlines existing 0x210a30
// (and 0x210850) here: a call to 0x210a30 stays out of line even with an inline or __forceinline declaration (697, 1495 bytes;
// its file has /Ob1). Tried on these: the origin as two statements, y first or x first, 491/492; sector read before the origin 443
// (1746 bytes, aligned 273); origin through the query, source after it, zero-init first 428/429; data as an if or if/else 429;
// the count/data locals named 451; used shared with the no-data arm 469; scan as a for 548 (1858); type as long 487; out point pointer
// 450-481 (aligned down to 245, 1757-1770); both conversions nested 410; z in a local +4; second edge store 442-449 (1770-1777).
// DL237: Complete native query-source mutable/immutable/memcpy/memmove copies 495/495/495/504;
// with existing origin setter 524/524/524/529. All restored; source-radius staging, persistent
// zero and metadata homes remain.
// Round 316: Eager classification short/word/long/dword with short/long links
// 427/420/415/422/473/420/473/420; eager word with paired corner results
// short/word/byte/long 451/451/415/424. Five-point 415 lead rejected above 30;
// original classification, Boolean corners and complete body restored.
// Round 331: Consumed trace/world start and destination positions shared separately/both 435/438/405;
// in-place second-tangent output alone/with start/with destination/with both 424/440/445/444; all with
// paired short corner results 454. Best: both positions shared (start/target hoisted to the step scope,
// the else arm's start/end reusing them) 405 plain, frame 0xf4 versus 0x10c, 1709 bytes. NOT kept: locals
// shared across arms to steer the frame are kept only with a match; body restored at 420.
// Round 529: successive source/expanded-radius assignments crossed with candidate-direction,
// circle-center and tangent-point workspace identities: 420/420/420/420/420/420/420/420.
// Frame agrees; radius staging folds and zero/metadata homes remain. Original complete body restored.
// DH302: Actual typed-step scan cursor with short/short, long/short, short/long, long/long
// scan/candidate ordinals 477/455/496/434; also walking the reverse candidates from one-past
// 485/460/488/458. Original indexed traversals restored; radius staging and zero/metadata homes remain.
// Round 371: Native selected successor/type/link widths, subsets 1/2/4/3/5/6/7,
// 455/424/463/462/456/424/458; all with full output count 503. Original short
// carriers and complete pointer body restored; radius/zero/metadata homes remain.
// Typed-selection, trace-admission and found-result branch orientations and their pairs/all
// 493/420/420/493/493/420/493; all with metadata-arm orientation 493. Radius staging, persistent zero
// and metadata homes remain; original body restored.
// M326: Native data-availability bool/byte/short/long predicates, conditional/default-null
// pointer production, 420/420/420/420/420/420/420/420. All canonicalize to the original
// radius stage and zero/metadata homes; complete original body restored.
// N327: Actual avoidance, tangent and predecessor last-consumer scope subsets on the retained
// step-pointer graph, then all with trace-result scope: 420/420/420/420/420/420/420/420. Original
// scopes/body restored; source-radius staging, persistent zero and metadata homes remain.
// Round 345: existing direction-difference/normalizer/squared-length forceinline declarations, seven
// subsets; all with dot product: 420/420/420/420/420/420/420/420. Original body and contracts restored.
// Radius staging, persistent zero and metadata homes remain.
// Round 380: Complete positive-distance/avoidance admission, bool/byte/short/long
// acceptance then rejection complement: 420/420/420/420/420/420/420/420. All canonicalize; radius staging,
// persistent zero and metadata homes remain. Original separate guards/body restored.
// P288: Whole-function completion publication, preservation/clear-required results in
// bool/byte/word/long: 420/420/405/429/428/428/458/435. Word preservation 405 at 1755/1746,
// frame 0x114 versus 0x10c. NOT kept: unsupported flag width/frame growth; no byte match.
// Original full body restored; radius staging, persistent zero and metadata homes remain.
// Round 64 (lane 10; aligned = difflib, masked = registers and stack offsets ignored): value fix from retail's two slots: 0x26ccb0 and 0x2c3030 take the unexpanded
// radius (E+0x2c, through EDI), 0x2c2740 and 0x2c2910 the expanded one (E+0x30, through XMM6/XMM7), which gives retail's `mov; mov; fld`: 420 -> 418 (kept as a value correction).
// Not kept: the first corner result in the scan's flag (retail's one byte slot) 418; that with a second `edge = 0xffff` at the typed scan hit 380 plain (masked 62, 1752, frame 0x114): retail has
// that `mov ebx, 0xffff`, but it stores the value edge already holds; on the 380 form z in a local 416 (1751). Further forms on the 380 body, none kept: side flipped
// in place frame 0x10c, 436 (1715, masked 70); with an out-point pointer 457 (1725, masked 54); with both conversions nested 440 (1752, masked
// 55, retail's tail copies; a helper's shape written out); z local 442 (masked 53); origin y through the query first 487 (masked 68); used long
// 482; long candidate 431 (1758, exact aligned 210: entry zero EBX, count SI, y first as retail, but candidate/NONE registers differ); long end 484,
// both long 476; count through its address, const reference or local copy 440 (same code); edge outside the loop, dword edge, order of the top
// assignments, candidate while/decl forms, scan compare against type/next/step 440 (same code). Root left: our count CSE holds EBX through the
// scan where retail's edge lives in EBX and count is read from memory; retail's dead `query+4` store shares the location's slot.
// Round 125 (m3 lane 7, fast; positional/aligned/masked, base 418/275): `edge = 0xffff` again at the scan hit 380/276/62
// (dead store, not kept); side flipped in place 419/222/58; both 419/224/58; long candidate 463/240/75; radius read
// from the query at each use 491/259/72; location at each use 419; edge declared after type 418/224/58; target.z per
// attempt 412/226/61; out-point pointer 439/226/49; plus named z 435/224/47; plus word NONE test 431/244/55; plus word
// output index 431; 0x210a30 called 680 (1512 bytes), inline or forceinline declaration 680; with the others 694-699;
// on the flip: edge first 419, type/link first 419, step last 418/222/57, source first 430, sector/origin last 419,
// origin per field 418/224/60, both 418, start per field 419; all together 460.
// Round 143 (m3 lane 5, fast = plain here): avoidance trace declared at the untyped branch scope
// 418 (same code); edge declared and initialised at the untyped branch scope 439 (1754 bytes);
// copied_last/used/index declarations reversed 418 (same code); edge renamed (name-order tip) 418
// (same code); emitted type from the type local instead of the step 421 (1755 bytes).
// Lane 18 r41 (fast; frame 0x114 against 0x10c: ours keeps edge in a stack slot where retail has it in EBX, and our
// byte flags sit four bytes higher): loop-top declarations in retail's store order (edge, next, found, link, type,
// then the step) 418; origin declared at function scope 418; output_index declared before the loop-top locals 418;
// the outer loop as `for (...; index < count; index = next)` 418; the typed branch storing link before type 418.
// Corrected two-radius graph: native first/second trace and whole-source
// receiver subsets 1/2/4/3/5/6/7, 418/418/429/418/429/429/429; native
// positioned-point receiver 418. All restored; no frame/zero/home recovery.
// Original radius values, native call roles and complete body retained.
// B397: Correct original-radius live interval with long type/link/successor subsets,
// then all with zero-extended selected type 423/418/476/409/475/476/475/475. NOT kept: 409
// needs both width changes; either alone regresses/ties, and native link stores stay
// word-sized. Original storage restored; frame 0x114/0x10c and zero homes remain.
// Round 404: Trace start/target/both shared setters 426/378/370; direct-start stores 418, with target
// setter 378; const start 418/378; attempt-owned start 412. NOT kept: target setter adds an in-loop z
// store; retail initializes z before the loop. Existing planar target setter alone/with start setter
// 427/435. Original endpoints/body restored; frame/zero/count homes remain.
// P308: On the retained two-radius lifetime, existing final difference/length/start-setter
// subsets 1/2/3/4/5/6/7 score 437/418/436/426/429/426/428; complete self-dot 428.
// NOT kept: ties/regressions; original point values, both radii, helpers and full body restored.
// P310: Joined forward-scan/reverse-candidate cursor, while-for/for-for/while-while/for-while:
// short 438/621/438/438; long 456/649/456/456. NOT kept: all regress; original separate
// scan/candidate ordinals, both radius values, geometry and complete body restored.
// M351: Immutable original/expanded radius and data binding, all seven subsets, then all
// with fixed source/current/selected/output receiver bindings: 418 each. Original body restored;
// frame 0x114 versus 0x10c, persistent zero and metadata homes remain.
// DH330: On the original-radius graph, array center pointer/reference 490/490; separate
// corners with copy/pointer/reference center 418/490/490; direct trace-target output
// alone/separate corners/center pointer 439/439/520. Original workspaces and body restored.
// DH338: Existing normalizer early-zero/positive-first/zero-result/magnitude-result
// 418/418/441/418; each with the planar setter 434. Setter forms raise the other
// normalizer user 68 to 98/91; all restored, frame/radius/zero/metadata homes remain.
// F370: Actual outer input/output/both cursors 448/516/487; input span advance 439;
// output publication advance without/with input cursor 558/558; input for-latch
// without/with output cursor 448/487. NOT kept: all regress; output cursors also
// regress the checked caller. Original ordinal addressing and full body restored.
// P315: Retained two-radius/pointer graph, existing complete data getter with all entry ordinal/emitted-count/fallback-result zero-lifetime subsets: 418/401/401/401/430/401/401/438.
// P315: NOT kept: no byte match or supported gain; original full body restored.
// P315: NOT kept: 401 places loop-zero stores above native availability/count guards; frame 0x114 and count BX remain.
// P317: Joint unconditional corner success/rejection accumulators bool/byte/short/long
// 426/426/406/435/473/473/452/452. NOT kept: no match; short success 406 grows to 1754 bytes and
// uses a wider result than native tests. Original two results and complete body restored.
// DH345: Native component-array displacement loops in direct/converted/both phases,
// long then dword, complete short/word: 428/418/429/428/418/429/451/451. No gain;
// original scalar producers, both radii, signed traversal and complete body restored.
// F372: Existing two-use location address at entry/data/multi-step/traversal/step/
// selected-step/corner-pair/pre-endpoint phases: 470/431/463/470/465/416/414/418.
// NOT kept: two-/four-point leads below gate; original address phase and full body restored.
// F379: Actual emission/continuation departure-limit outcomes, bool/byte/short/long,
// positive/complementary 456/456/456/456/456/456/456/456. NOT kept: regressions; original
// whole body restored, frame 0x114 versus 0x10c and zero/radius/metadata homes remain.
// P325: Existing normalizer mutable/readonly squared-length results crossed with
// magnitude/inverse entry declaration lifetimes all tie 418 (eight forms); every user stays calibrated.
// NOT kept: no byte match for the one-use call-result intermediate. Original complete helper
// and body restored; frame 0x114/0x10c and zero/count/metadata homes remain.
// P328: Complete final squared-distance expression/compound/separate/partial reductions,
// point then vector displacement: 418/418/418/418/418/418/418/418. Original body restored;
// frame 0x114/0x10c, radius/zero/count and metadata/trace homes remain.
// Round 586: Two-corner side-table traversal, short/word/long/dword ordinal with
// indexed/body-advanced corner output: 491/491/484/484/491/491/484/484. NOT kept: nonexact
// loop/result-array residence; original direct calls, radii and complete body restored.
// J255: Scan-result scope ending before corner results, second-trace admission scope,
// and complete Boolean role names, subsets 1/2/4/3/5/6/7 then alternate complete names:
// 439/418/418/439/439/418/439/439. NOT kept: ties/regressions; scan scope also grows
// to 1754 bytes. Original full body restored; frame 0x114/0x10c and radius/zero/count homes remain.
// P335: Actual corner/tangent/trace eight-byte alignment, seven subsets and complete
// graph with direction alignment: 652/652/655/652/652/652/652/652. NOT kept: prologue/extent
// diverge and three exact providers regress. Original objects, alignment and complete body restored.
// P342: Standard record copies at fallback/singleton, forward/backward/lifetime single sites then paired
// forward/backward: 403/423/429/429/427/425/428/440. NOT kept: fallback forward reaches 403 and native
// length but outlines its copy, keeps frame 0x114 and loses exact caller; all algorithms lose that
// caller. Native copy-then-count commit 418. Original full body restored; frame 0x114/0x10c and
// radius/zero/count/metadata homes remain.
// P344: Actual current/selected/output array-element lvalues on the corrected two-radius graph, seven
// subsets then actual source subobject: 418/418/418/418/418/418/418/418. NOT kept: no byte match;
// original complete body restored. Native frame, radius/zero/count and metadata/trace homes remain.
// M3 lane 8 r135 (fast; raw/register-masked/stack-masked aligned, base 352/265/105 at 418): tuple phases through a no-op chain 418
// x3 (no effect here; the difference is integer homes). Diagnostics, removed: count read volatile at the outer test 399
// (355/267/106), scan 489, both 456; with the second edge store (380, 352/262/103) 453/458; one real extra use, at the loop
// top, of next/type/link 419, step/points 421, edge 464 (2 and 4 uses 464: edge never leaves its stack slot), found/copied
// 477, index/result 484, count 483 (336/236/114: the only one that moves roles toward retail), source 436, sector/data 492,
// query 493, used 519. No form tried recovers the 0x10c frame.
// Round 74 (m3 lane 11, fast and plain): the emitted point through the existing real_point3d_set 400 (kept: retail
// stores x, y and z as floats through one point pointer). The 8 extra frame bytes are two homes: ours keeps edge and
// other_side in stack slots where retail keeps them in EBX and BL (retail shares distance's slot with endpoint). On
// 400: the distance block reading the output point through a `s_type_c3b527 const *` 478 (1765 bytes, masked 90: gives
// retail's zero-extended index test, the reads through ESI and the untailmerged link stores, but swaps ESI/EDI
// throughout); `!side` passed and flipped in place 434 (1715); the second edge store at the scan hit 433 (diagnostic);
// endpoint declared after next_step 463; the start fallback written in both arms of an if/else (retail has two
// copies) 452.
// T63: Sector/avoidance/conversion inline call contracts, seven subsets and all with
// target inline metadata: eight 400. NOT kept: no equality or gain; every original
// body, declaration and call restored. Frame 0x114/0x10c and zero/edge/count homes remain.
// P361: Existing point-setter return retained at all later point reads through mutable/readonly/fixed pointers, references and outer pointer declarations:
// 400/400/400/400/400/400/400/400. NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// Frame 0x114/0x10c, edge/orientation residence, zero/count and metadata homes remain.
// P374: Inclusive final-ordinal outer/scan/terminal consumers, seven long-bound subsets
// then complete short bound: 499/516/484/521/485/502/460/467. NOT kept: all regress; original
// whole body restored; native frame, persistent zero/count and edge/orientation homes remain.
// P376: Scan-end scope before reverse candidate, short/long for/while, original/ended avoidance-output scope:
// 400/456/400/456/400/456/400/456. NOT kept: ties/regressions; frame 0x114 versus 0x10c persists.
// Original signed scans and whole body restored; zero/count/edge/orientation homes remain.
// P378: Output-capacity ownership of complete conversion/distance continuation, positive/equality/
// terminal/complementary gates with short/long emitted ordinals: 400/495/400/495/400/495/400/495.
// NOT kept: ties/regressions; frame 0x114 versus 0x10c and edge/orientation spills remain.
// T69: Opposite-orientation value before center selection in bool/byte/short/long
// 435/454/401/402; after center selection before the first tangent 456/455/457/456.
// NOT kept: all regress; original phases/storage and full body restored. Frame/edge/count homes remain.
// P380: Actual corner-attempt completion owns shared fallback admission, positive/complementary
// bool/byte/short/long outcomes with a successful attempt break.
// Scores 400/400/400/400/400/400/400/400. NOT kept: all tie 400 at 1752/1746; no exact flag-result form
// or native frame recovery.
// Original complete body restored.
// P383: Complete complementary candidate-missing outcomes in bool/char/signed-
// char/byte/short/word/long/dword: 400/400/400/400/471/471/460/460. NOT kept: no native frame/edge/count-
// home equality; original forward scan, reverse avoidance scan and whole body restored.
// T71: Current output-setter graph edge/selection/output lifetime regions, seven subsets and
// complete role names: 481/481/478/400/481/481/478/478. NOT kept: ties/regressions and no
// native frame repair; original full body restored.
// P392: Complete bitwise corner-query admission, both original results retained, success/failure in
// bool/byte/short/long: 477/477/477/477/488/488/502/499. NOT kept: no native frame/edge/orientation
// equality; one-use outcomes require a match. Original two-query admission and whole body restored.
// MiniMax R (11-11): no new form, no lower score (400); a permuter run over this body (556 tries in 1210 s)
// reached 386 on the reduced image and on the full image, but its best rewrite swaps the order of the sector
// and node-index arguments of the trace call and of the two point arguments of the tangent call, and it
// writes the link index through a volatile byte, so it changes what the function computes and is not kept;
// the part of that rewrite that keeps the behaviour (the declaration order inside the loop, the reversed
// comparison operands and the inverted type test) compiles to the same bytes as the body below, so the score
// stays at 400 with the same 1752 bytes.
// MiniMax R (11-11): no new form, no lower score (400); the prologue differs: retail pushes only esi and edi
// and keeps the loop's zero in EBX and the count in SI, while ours pushes ebx and esi and keeps the zero in
// EDX and the count in BX; retail reads the radius from the stack slot [esp+0x2c] and stores the expanded
// radius at [esp+0x34] where ours reads [esp+0x30] and stores at [esp+0x44]; our locals sit at 0x10-0x58
// where retail's sit at 0x6c-0x84, so the frame is 0x114 against retail's 0x10c and the two loops still
// choose different registers; 1752 bytes against retail's 1746.
// MiniMax S (11-09): no new form, no lower score; the frame is still eight bytes larger than retail's
// (0x114 against 0x10c) and the first locals sit at higher offsets, so every stack slot and the
// register choices in the two loops still differ; no form tried recovers retail's frame.
// M3 lane 8 r144 (fast; control 400, frame 0x114): retail returns with `ret 0x18` as ours, so the six stack arguments
// agree. The output element taken with a post-increment of the emitted count 400 (same code); the previous point chosen by
// an if/else in place of the conditional expression 400 (same code); edge declared at the top of the scan arm 481 (1769
// bytes); the next step's pointer declared inside the type test 400 (same code); the previous point's test as `next != 0`
// 401; the forward scan as a for with the end declared outside 636 (1911 bytes, frame 0x124); the emitted type from the
// local 403 (1755); the trace target initialised whole 400 (same code). Body restored.
// P401: Forward scan ordinal word/dword/unsigned-int64 with short reverse candidate:
// 488/501/535; with signed-int64 reverse candidate 591/572/664; signed-int64 forward
// with short/int64 reverse 535/664. NOT kept: wider/unsigned scan interference grows
// the body and misses native frame/edge homes; original signed scans and full body restored.
// Round 85 (m3 lane 10, all plain with 0x270930 and 0x2c2a70, which stay matched; positional/register-masked aligned, base 400/58): edge as a `word` 462/63
// (1759 bytes); `++used` after the type/link stores 400/61; `unknown02` copied before the output index 400/59; `*complete` cleared before the count is
// stored 464/70 (1753); the typed branch setting type and link before next 400/57. No form moves count out of BX or edge into EBX; frame 0x114 stays.
// Round 66 (m3 lane 2, fast; 0x270930 and 0x2c2a70 stay matched): retail's EBX is the zero only up to the loop, then edge
// (`mov ebx, 0xffff`); ours gives EBX to the NONE constant and EDI to the step pointer, retail EDI to index. Named step
// pointers for the fallback's points[next - 1] and points[next] reads 426 (1761 bytes); the step's fields read through
// points[index] at each use 473 (1758 bytes); the scan's trace declared before the candidate loop, the two trace results
// before the attempt loop, the delta block's start/end beside delta: 400 each (same code, frame 0x114 unchanged).
// Diagnostic, not kept: the distance test as a call to 0x210a30 with src/unknown_20fe20.cpp's /Ob1 dropped 657 (1554 bytes):
// the call still stays out of line (and 0x270930 moves to 5), so that file's /Ob1 is not what keeps it out.
// Round 68 (m3 lane 2, fast; 0x270930, 0x2c2a70 matched): retail's EBX holds in turn the entry zero, edge, side/other_side and used,
// ESI holds NONE as a register constant in the loop and count stays in memory (`cmp di, [ebp+0xc]`); ours reloads count into BX at
// the loop bottom and keeps it through the scan. The emit's type and link stored unconditionally from the locals 415 (1751 bytes); as
// two conditional expressions 521 (1795); the else arm as `out->type = out->link_index = NONE;` 400 (same code); the corner block's
// tests joined (`edge != 0xffff && next_step->type == NONE`) 399; the delta test's operands swapped 400 (same code).
// M3 lane 9 r109 (fast with 0x270930 and 0x2c2a70, base 400): the data fetched through the existing getter 0x1fa7f0 in place of its
// body written out 400 (same code, kept). Retail stores a query+4 pointer it never reads (the trace an inlined helper's parameter
// leaves), so helper boundaries (forceinline, one caller, kept only with a match): the forward scan and reverse candidate search as one helper returning found with next/edge
// out-parameters 482 (1769 bytes; `static inline` stays a call, 718 at 1492), returning edge with next/found out-parameters 482
// (edge's 0xffff moves into the scan, index to BX); the corner block (both corner queries, side, both attempts) as a helper with
// origin/sector in-out pointers 400 (same code); the emit as a helper 400 (same code). Re-measured: `!side` to the second tangent
// call with `side = !side` in the attempt for's increment 434 (1715 bytes, frame 0x10c: the attempt loop is then retail's, other
// side in BL and location in EDI); on it the first conversion in 0x210850's nested shape 426 (1725 bytes, masked aligned 108 against
// 94). The scan is what is left there: retail compares count from memory and keeps edge in EBX, ours keeps count in BX.
// M3 lane 11 r89 (fast with 0x270930 and 0x2c2a70, same code as the full check; base 400, aligned 350, stack+register masked 97):
// other_side declared at the top of the attempt body, ahead of the center choice and the first tangent call, 435 (1746 bytes,
// retail's length, but frame still 0x114 and aligned 388, masked 94). Same code: other_side declared at the corner block and
// assigned in the attempt body; the trace start through the existing real_point3d_set; end declared, then assigned. Code differs,
// 400 each: the last-candidate flag as `candidate + 1 == count`; edge stored after next/found in the candidate loop; found set
// before next at the scan hit; type/link declared ahead of next/edge/found. Retail's second `mov ebx, 0xffff` at
// the scan hit (0x2c219e) reads as an inlined helper's edge out-parameter; the listed helper forms (482) did not give it.
// M3 lane 13 r81 (fast with 0x270930 and 0x2c2a70, which stay matched; aligned exact/fully masked, base 335/57): the masked
// diff is register and slot choice only (retail: edge in EBX and a second `mov ebx, 0xffff` where the scan finds a typed
// step, count compared from [ebp+0xc], NONE in SI; ours: count in BX, edge at a stack slot). Tried: a named
// `short last = count - 1` for the last-candidate test and the copy of the last step 469 (1765 bytes); the corner test as
// `left && right` 401; type and link without initializers, set to NONE in the untyped arm 476 (1762 bytes); the corner as
// `side ? corners[0] : corners[1]` 473 (1710 bytes); the other side passed as `!side` with `side = !side` at the end of the
// attempt body 434 (1715 bytes, frame 0x10c; the listed for-increment form's code), plus the named last index 455 (1743 bytes).
// m3 lane 4 r191 (fast with 0x270930/0x2c2a70; aligned/masked 349/95): if/else arms swapped (match-only forms): the typed
// and untyped step arms 458 (1746 B), the fallback's `next == 0 || next == index` first 400 (2 lines differ), the emit's
// `type == NONE` first 400 (aligned 361), the distance block's `!=` arm first 441, the corner choice as
// `if (!side) corners[1]` 405.
// P437: Fixed actual reverse distance/location/left/right query results, predecessor/endpoint
// receivers, opposite orientation and signed count input: 400 each. NOT kept: frame 0x114 versus
// 0x10c and edge/orientation/count homes unchanged; original complete body and helpers restored.
// M3 lane 1 r99 (fast with 0x270930 and 0x2c2a70, both MATCH throughout; base 400 at 1752 bytes, frame 0x114). Reading: retail
// holds no local in a register across the loop; EBX is the entry zero, then edge from the loop top to the corner calls, then the
// side, then used. Ours caches count in BX from the loop bottom through the scan (retail reads [ebp+0xc] at each compare), so the
// zero goes to EDX, edge to a stack slot and the step pointer to EDI. Retail's side is one value flipped in place between the two
// tangent calls (`test bl, bl; sete bl`), with a copy in a stack byte for the first call. The second tangent call taking the side
// flipped in place (`side = !side;` ahead of it, no other-side local) 440 (1727 bytes, frame 0x10c); that with `edge = 0xffff`
// again at the scan hit (retail's second `mov ebx, 0xffff`) 369 (1740 bytes, frame 0x10c, first difference +0x14; not kept: the
// store repeats the value edge holds, and the flip alone raises the score); the second store alone 433; the step, its edge and
// the found flag set through one inline setter at the scan hit and at a candidate's success 432 (the setter gives that store too),
// with the in-place flip 410; edge set at each choice with no loop-top initialiser, with the flip, 471; an other-side local kept
// across the attempts (started as the side, flipped before the second tangent, copied back) 438, with the second edge store 424;
// the origin read y then x on the 369 form 467. Diagnostics: count declared volatile 474, with the second edge store 420, with
// that and the side passed as `!side` and flipped at the loop end 433 (frame 0x10c): keeping count out of BX alone does not give
// EBX to edge. Body unchanged at 400.
// P466: Next-orientation commit at producer/second tangent/candidate/target X/target Y/
// first trace: 440/441/441/441/441/441; early producer with first-tangent commit
// 441/443. NOT kept: frame remains 0x114 and all regress; original side ownership,
// both radii, calls and full body restored. Edge/count homes remain.
// T107: First tangent orientation retained while persistent side advances before the
// first call, bool/byte/short/long before center 452/465/466/469; before first tangent
// 442/449/446/448. NOT kept: no native frame/extent/home equality; original body restored.
// @retail 0x2c2060
void __stdcall function_2c2060(s_type_f17a25 *query, short count, s_path_step_view const *points,
    short *result_count, s_path_step_view *result, bool *complete)
{
    real source_radius = query->source.radius;
    real radius = (real)(source_radius + 0.05);
    s_pathfinding_data *data = (s_pathfinding_data *)function_1fa7f0();
    if (data)
    {
        if (count > 1)
        {
            s_type_c3b527 const *source = (s_type_c3b527 *)&query->source.point;
            point2f origin = *(point2f *)&source->point;
            long sector = points[0].node_index;
            short index = 0;
            short used = 0;
            bool copied_last = false;
            while (index < count)
            {
                s_path_step_view const *step = &points[index];
                short next = NONE;
                long edge = 0xffff;
                bool found = false;
                short type = NONE;
                short link = NONE;
                short output_index;
                if (step->type != NONE)
                {
                    next = index;
                    type = step->type;
                    link = step->link_index;
                }
                else
                {
                    short end = index;
                    while (end < count)
                    {
                        if (points[end].type != NONE)
                        {
                            next = end;
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
                            point2f corners[2];
                            bool left = function_2c3030((s_pathfinding_data *)query->pathfinding,
                                &origin, source_radius, edge, true, &corners[0]);
                            bool right = function_2c3030((s_pathfinding_data *)query->pathfinding,
                                &origin, source_radius, edge, false, &corners[1]);
                            if (right && left)
                            {
                                point2f const *previous = next > 0 ? (point2f *)&points[next - 1].point.point :
                                    (point2f *)&source->point;
                                point2f const *endpoint = (point2f *)&next_step->point.point;
                                bool side = function_2c2a70(previous, endpoint, &corners[0], &corners[1], &origin);
                                point3f start = { origin.x, origin.y, 0.0f };
                                point3f target;
                                target.z = 0.0f;
                                long location = (long)&query->location;
                                for (short attempt = 0; attempt < 2; ++attempt)
                                {
                                    point2f center;
                                    if (side)
                                        center = corners[0];
                                    else
                                        center = corners[1];
                                    point2f tangents[2], candidate;
                                    function_2c2740(&origin, &center, radius, side, &tangents[0]);
                                    bool other_side = !side;
                                    function_2c2740(endpoint, &center, radius, other_side, &tangents[1]);
                                    function_2c2910(&center, tangents, &origin, radius, &candidate);
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
                                    side = other_side;
                                }
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
                    origin = *(point2f *)&points[next].point.point;
                    sector = points[next].node_index;
                    output_index = points[next].point.output_index;
                    next = index + 1;
                }
            emit:
                s_path_step_view *out = &result[used];
                out->node_index = sector;
                real_point3d_set(&out->point.point, origin.x, origin.y, step->point.point.z);
                out->point.output_index = output_index;
                out->unknown02 = step->unknown02;
                ++used;
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
                point3f delta;
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
