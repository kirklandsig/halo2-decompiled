// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <math.h>
#include <string.h>

struct s_portal_path_portal
{
	short clusters[2];
	long plane_index;
	point3f center;
	real radius;
	dword flags;
	byte unknown1c[8];
};

struct s_portal_path_cluster
{
	byte unknown00[0x72];
	short sound_index;
	short environment_index;
	byte unknown76[2];
	short sound_path_index;
	short environment_path_index;
	byte unknown7c[0xb0 - 0x7c];
};

struct s_portal_path_list
{
	long unknown00;
	long count;
	short *portals;
	byte unknown0c[8];
};

struct s_portal_path_sound
{
	byte unknown00[0x44];
	real range;
	byte unknown48[0x64 - 0x48];
};

struct s_portal_path_environment
{
	byte unknown00[0x28];
	real range;
	byte unknown2c[0x48 - 0x2c];
};

struct s_portal_path_bsp
{
	byte unknown00[0x60];
	s_portal_path_portal *portals;
	byte unknown64[0xa0 - 0x64];
	s_portal_path_cluster *clusters;
	byte unknowna4[0xd4 - 0xa4];
	long sound_count;
	s_portal_path_sound *sounds;
	long environment_count;
	s_portal_path_environment *environments;
	byte unknowne4[0x14c - 0xe4];
	s_portal_path_list *sound_paths;
	long environment_path_count;
	s_portal_path_list *environment_paths;
};

struct s_structure_bsp_view;
struct s_14b240_owner;
struct s_bsp3d_disk;
real function_249d60(s_structure_bsp_view *bsp, long cluster_a, long cluster_b);
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point);
long function_2197f0(real gain);
long __stdcall function_18ae50(long a, long b, real const *values);
typedef bool (__stdcall *t_portal_path_compare)(long, long, void const *);
void sort_4byte(void *unused, long *elements, unsigned long count, t_portal_path_compare compare, void const *context);
extern real g_44a0b4;

static inline void portal_path_set_bit(dword *bits, long index)
{
	dword *word = &bits[index >> 5];
	*word |= 1 << (index & 31);
}

static inline real portal_path_pin(real value)
{
	return 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
}

__forceinline real portal_path_linear_gain(real decibels)
{
	long bits;
	if (decibels < g_44a0b4)
		bits = 0xc2800000;
	else if (decibels > 0.0f)
		bits = 0;
	else
		bits = *(long *)&decibels;
	real scaled = *(real *)&bits * 0.05f;
	return (real)exp(scaled * 2.3025851f);
}

__forceinline long portal_path_gain(real distance, real range)
{
	range = range > 0.001f ? range : 0.001f;
	real t;
	if (fabs(range) < 0.0001f)
		t = distance < range ? 1.0f : 0.0f;
	else
		t = portal_path_pin((distance - range) / (0.0f - range));
	t = portal_path_pin(t);
	real lower = portal_path_linear_gain(-64.0f);
	real upper = portal_path_linear_gain(0.0f);
	real fraction = t * t * 3.0f - t * t * t * 2.0f;
	fraction = portal_path_pin(fraction);
	return function_2197f0((upper - lower) * fraction + lower);
}

// score 287 (was 296): Advancing the required index output restores gain-based output addressing.
// The 0x428 frame agrees. The starting-cluster register, table loads, validity words
// and sort scratch offset differ.
// Before the sort scope, the frame was 0x42c.
// Tried: const portal 371; const views 371; endpoint mask before shift 371; const portal and mask 371; shared
// seed array index with separate bit read 373; validity memset 311 kept (plain); path ordinal in while 371;
// index test before closest distance 371.
// Round 605. The sort scratch scoped to its call scores 296 and restores the four 64-entry array offsets (plain
// confirmed). Bitmap clear before cluster 312. Closest distance after seed 308. Search and sort scopes 296.
// Early scratch 311. Named ordinals 311. Bounded output count 311. Shared output element 311. Explicit gain
// arms 302 (NOT kept: frame 0x434). Search scope 311. Sort scope with gain arms 302 (NOT kept: frame 0x430).
// Round 628: signed portal 363, both ordinals signed short 363, const path-list reference 296,
// integer sort scratch 296, aggregate validity clear 368, shared destination sound index 355,
// shared seed sound index 367, long packed link 317, sort validity before ordinal 299.
// The entry still uses EBX for the starting cluster and spills both table bases.
// Round 630: unsigned packed link 296, named endpoint 296, high-bit endpoint selection 358,
// signed seed index with fresh bitmap read 361, distance accumulator 296, scoped path walk 296,
// nested closest-distance admission 296, ordered range rejection guards 296.
// The starting-cluster register and both table-base reloads still differ.
// The starting cluster still occupies EBX, and both table bases are kept across the portal loop.
// Shared bitmap setter/range test/order-output tests/all operations 296/317/296/317.
// Seed-and-walk cluster scope 298; early closest distance 296; signed starting sound at the
// closest test 296; signed destination through range/bitmap 317, with shared test 317.
// Closest distance, valid bitmap and starting distance names 296/296/296.
// Round 422: Explicit portal table / cluster table / both / pointer addition 296/296/296/296.
// Sort-preparation count snapshot 296, capacity-first limit 296, selected comparison distance 296,
// default range with an override 351. The table spills, starting-cluster and validity homes remain.
// Portal/destination record references, separately and together, 296/296/296. Workspace before starting-
// cluster lookup 296, early path-list lookup 358, packed-entry reference 341, nested flag and sound
// gates 296, destination lookup inside the flag gate 328. The early list and late destination forms
// recover EBP at entry, with diffs at +0x28 and higher scores.
// M3 lane 13 r110 (fast and plain agree; aligned = difflib count of the listing rows; body unchanged, the full
// diff is HELD as build/r110/held-0x18dc90.patch): cause found. Retail compiles every caller of 0x14b240 as if that
// call may write memory: this loop reloads both table bases for every path entry, and 0x222150 keeps a value it read
// before the call in a register across it. Link-time code generation summarises a callee that way when it stores
// through a pointer or reference variable, even into its own local (scratch tests in build/r110/micro). With
// 0x14b240's in-plane offset subtracted through a reference-bound local (its bytes unchanged) this body is 373
// (1349 B, aligned 161 against 172); rewritten on that base it reaches 249 at retail's 1364 B (aligned 23), 0x18dc90
// 29 at 1363 B, and 0x222150 matches without its two volatile reads. Steps on that base, each measured:
// `long encoded`, `short portal_index` and the side bit as `((dword)encoded >> 15) & 1` 366 (1354 B, aligned 148);
// the valid test through an inline test helper (retail reuses the widened index) aligned 143; the seed stores as
// clusters, portals, distances aligned 142; the decibel helper on the long bit pattern with an if/else result
// (0x2195f0's shape) and the range clamp written at the call 366 (aligned 82); the output limit declared in the
// for-init after the counter aligned 75; the output range through a named element pointer aligned 72; the fraction
// as a (value, lower, upper) helper testing fabs(lower - upper), pinned arm first 365 (1357 B, aligned 47); the valid
// words cleared before the cluster lookup 253 (1373 B, aligned 43); the fraction helper pinning after both arms plus
// a curve helper pinning its scale again 249 (1364 B, aligned 28); the degenerate arm as
// `!(value < lower) ? 0.0f : 1.0f` 249 (aligned 23). HELD, not kept: without a match the review list reverts the
// clear moved ahead of the cluster lookup (373 without it), the arms' order (254), the for-init limit (245) and the
// two one-caller helpers, and 0x18ae80 rises 846 -> 911 on that base. Left on it: the cluster pointer sits in EBX and
// the widened portal index in EBP (retail the other way round; about sixty forms and a 600 s permuter run did not
// flip them: declaration order, names, one use more or fewer, helper shapes), and `fmul [0.05]` is scheduled after
// the next clamp's store (retail before it). Not better on that base: calling 0x2195f0 501 (1222 B, it stays a
// call); the decibel helper on a real constant 465 (1257 B, it folds); the valid words as an initializer or two
// assignments 377 (aligned 89, a shared zero register appears); one helper with the first pin in the normal arm 243;
// macro pins in one helper 254; the exponent as one expression 262 (1352 B). With main's 0x14b240: the test helper
// alone 317; the long-bits decibel helper alone 323; the path entry types alone 357; all three 359 (none kept).
// Capped bound in loop declaration 297; sorted source cursor 313; index cursor 287 (kept);
// gain cursor 290; index and gain cursors 296; common index store 316; common index/gain 303;
// common output stores 324. NOT kept: gain cursor reaches 1364 bytes but gains six.
// The starting-cluster register and both table reloads still differ.
// M3 lane 13 r111 (body unchanged; see 0x18dc90's note): the twin has the same two leftovers. On
// build/r111/held-0x18dc90.patch (the r110 patch with the aggregate-returning pin helper) the fmul schedule is retail's
// at both sites: harness 244 positional / 17 aligned at 1364 B, fast 250 (the r110 patch 249, aligned 23). Left: the
// cluster pointer in EBX and the widened portal index in EBP, and the padding that follows from it.
// Keep the table reloads and starting cluster in EBP together on the 0x428 frame.
// Path-list ordinal before seed 357; separate seed and search lookup 365; seed
// validation before bitmap clear 296; walk count snapshot 364; link cursor 355;
// sound comparison before portal flags 322; nearest distance after path-list
// lookup 293; signed admission index through range and bitmap 317.
// NOT kept: 293 gains three and moves the first difference earlier.
// Portal-table, destination-table, both, or seed-through-destination table references 296 each;
// packed link base or address 296; path ordinal advanced at the link read 367;
// starting-cluster and decoded portal names 296. NOT kept: the starting cluster and both
// live BSP table bases retain their opening allocations.
// @retail 0x18d730
void __stdcall function_18d730(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
	real maximum_range, long *indices, long *gains, long *portals, long *clusters, long *count)
{
	s_portal_path_cluster *cluster = &bsp->clusters[cluster_index];
	dword valid[2];
	real distances[64];
	long nearest_portals[64];
	long order[64];
	long nearest_clusters[64];
	real nearest_distance = FLT_MAX;
	memset(valid, 0, sizeof(valid));
	if (cluster->sound_index != NONE)
	{
		nearest_clusters[cluster->sound_index] = cluster_index;
		distances[cluster->sound_index] = 0.0f;
		nearest_portals[cluster->sound_index] = NONE;
		portal_path_set_bit(valid, cluster->sound_index);
	}
	s_portal_path_list *paths = &bsp->sound_paths[cluster->sound_path_index];
	for (long i = 0; i < paths->count; i++)
	{
		short encoded = paths->portals[i];
		long portal_index = (short)(encoded & 0x7fff);
		s_portal_path_portal *portal = &bsp->portals[portal_index];
		long other_index = portal->clusters[(encoded >> 15) & 1];
		s_portal_path_cluster *other = &bsp->clusters[other_index];
		if (!(portal->flags & 0x28) && other->sound_index != cluster->sound_index)
		{
			real travel = function_249d60((s_structure_bsp_view *)bsp, cluster_index, other_index);
			real distance = function_14b240((s_14b240_owner *)bsp, (s_bsp3d_disk *)portal, point) + travel;
			if (nearest_distance >= distance && cluster->sound_index != NONE)
			{
				nearest_portals[cluster->sound_index] = portal_index;
				nearest_distance = distance;
			}
			if (other->sound_index != NONE)
			{
				s_portal_path_sound *sound = &bsp->sounds[other->sound_index];
				real limit = sound ? sound->range : maximum_range;
				if (limit >= travel)
				{
					if (valid[other->sound_index >> 5] & (1 << (other->sound_index & 31)))
						limit = distances[other->sound_index];
					if (limit >= distance)
					{
						distances[other->sound_index] = distance;
						nearest_clusters[other->sound_index] = other_index;
						nearest_portals[other->sound_index] = portal_index;
						portal_path_set_bit(valid, other->sound_index);
					}
				}
			}
		}
	}
	for (long i = 0; i < bsp->sound_count; i++)
	{
		order[i] = i;
		if (!(valid[i >> 5] & (1 << (i & 31))))
			distances[i] = FLT_MAX;
	}
	{
		real scratch;
		sort_4byte(&scratch, order, bsp->sound_count, (t_portal_path_compare)function_18ae50, distances);
	}
	if (count)
		*count = 0;
	long limit = bsp->sound_count < 64 ? bsp->sound_count : 64;
	for (long i = 0; i < limit; i++)
	{
		long index = order[i];
		if (valid[index >> 5] & (1 << (index & 31)))
		{
			*indices++ = index;
			if (gains)
				gains[i] = index != NONE ? portal_path_gain(distances[index], bsp->sounds[index].range) : 0xc2800000;
			if (portals)
				portals[i] = nearest_portals[index];
			if (clusters)
				clusters[i] = nearest_clusters[index];
			if (count)
				(*count)++;
		}
		else
		{
			if (count)
				break;
			*indices++ = NONE;
			if (gains)
				gains[i] = 0xc2800000;
			if (portals)
				portals[i] = NONE;
			if (clusters)
				clusters[i] = NONE;
		}
	}
}

// score 354 (was 354): The entry frame is 0x430 versus retail's 0x428. Retail keeps the starting cluster in
// EBP, reloads both BSP table bases per path entry and resolves the destination before testing flags. The
// extra table-base spills persist in the build.
// Earlier score 354 (was 354): Frame 0x430 versus 0x428. Retail keeps the starting cluster in EBP and reloads
// both BSP table bases inside the portal loop. Tried: const portal 354; const views 354; endpoint mask
// before shift 354; const portal and mask 354; shared seed array index with separate bit read 373; validity memset 364; short endpoint with early
// path increment 371; index test before closest distance 354.
// F417: Invalid output first 353; count-stop return 354; explicit gain arms 365;
// sort cursor with ordinal bound 376; default two-entry limit 354; shared valid row 374;
// sort ordinal after distance write 354; separate output bounds 362; index with sort cursor 360;
// bounded end cursor 332. NOT kept: the end cursor recovers the 0x428 frame but shrinks to
// 1346/1363 bytes and keeps the table bases live. Original indexed output and body restored.
// AC571: Combined rejecting guard / combined accepting guard / same-environment guard within the open-portal
// block / eligible path inside the flag guard 368/368/368/354. Signed endpoint test 373, with a short cluster
// index 354 at retail length. One destination index through the cap with fresh publication 333, and a separate
// read at the cap 354. NOT kept: 333 shortens to 1347 and retains frame 0x430; the mixed reads do not match.
// The short cluster form reaches frame 0x42c. Both BSP table reloads and the 0x428 frame still differ.
// Path ordinal, destination ordinal, path list and minimum named separately and together 354 each.
// Uninitialized arrays before the valid bits, minimum between the bits and arrays, or bit clear
// before the initial cluster lookup 354 each. The 0x430 frame and cached table bases remain.
// Round 623: narrow destination before flags 374, in nested gates 374; unsigned packed link and
// cluster before flags 373; one destination environment after distance calls 363; rejecting range gates
// 354, selected distance 354, comparison selected by the validity bit 356; one starting environment 326.
// NOT kept: retail rereads the starting environment, and the 326 form retains the 0x430 frame.
// Round 625: one path count 323, one entry array 356, both 323, packed-entry range 356;
// portal distance before addition 354, total in the travel result carrier 352 at 1363 bytes,
// nearest distance before portal 354, late order array and integer sort scratch 354.
// NOT kept: both 323 forms keep the count snapshot where retail reloads the count and retain the 0x430 frame.
// The 352 form gains two. The path table spills and starting-cluster register still differ.
// Round 628: scoped sort scratch 354, scoped integer scratch 354, scoped scratch with validity
// memset 365, and the paired clear/sort/positive guard 364 with the 0x428 frame.
// Separate traversal scope 354, chained validity words 354, long packed link 351,
// late distance initialization 358, scoped scratch and explicit output cap 363.
// NOT kept: the 351 form gains three, with the 0x430 frame still present.
// Advance the gain, portal or cluster output pointer at each row store 368/355/357.
// Cursor pairs 350/350/365, all optional cursors 359, every output cursor 359.
// NOT kept: the two 350 forms recover the 0x428 frame at 1368 bytes and gain four.
// Both BSP table bases still spill and the starting cluster remains in EDI.
// portal element reference 354; destination element reference 354; destination reference before portal flags
// 368; advance starting cluster table base 354; advance reloaded portal table base 354; advance destination
// table base before flags 368; signed extended packed link receipt 351; distance before nested starting
// environment check 354. NOT kept: the signed packed-link receipt reaches 1363 bytes and frame 0x42c but
// gains only three points. One table base still spills and the other remains hoisted. Resolving the
// destination before the flags with the reference or advanced base shortens the body to 1347 bytes and
// regresses. The opening body is restored.
// M3 lane 13 r110 (body unchanged; the full diff is HELD as build/r110/held-0x18dc90.patch, see 0x18d730's note for
// the cause and each step): with 0x14b240 summarised as a call that may write memory this body is 377 (1343 B) and,
// rewritten as its twin, 29 at retail's 1363 B (aligned 22; plain check). What is left there is the twin's: cluster
// pointer in EBX and widened portal index in EBP (retail the other way round), and the `fmul [0.05]` scheduling.
// Measured on that form: the degenerate arm written directly 32; the seed stores in main's order 33; the limit
// declared before the loop 225 (1355 B); the fraction's arms in the other order 181 (1370 B); the cluster lookup
// ahead of the clear 371 (1347 B). With main's 0x14b240: the test helper alone 333 (1347 B); the long-bits decibel
// helper alone 357; the path entry types alone 351; all three 377 (none kept).
// Tried flags before endpoint 322, long masked ordinal 351, high-bit predicate 351, short endpoint local 354,
// packed cursor with live count 354, explicit range cases 354, initial maximum range 365,
// shared fill and sort count 354, available-first output cap 363. NOT kept: early flags skip retail's
// pretest endpoint decode and the frame remains larger.
// M3 lane 13 r111 (one-file harness with the callee files, exact; counts positional/aligned, `--fast` where marked; body
// unchanged, the full diff is HELD as build/r111/held-0x18dc90.patch): on the r110 patch the fmul schedule is solved.
// Every retail copy of the decibel-to-gain code (here, 0x18d730, 0x18ae80, 0x21fa80) has `fld; fmul 0.05; <others>;
// fmul ln10`, a block boundary inside the helper; the pin helper returning the level as an aggregate (a real and its
// bits sharing storage) instead of a long gives it at both sites: 23/22 -> 15/16, fast 21 (the r110 patch 29), 0x18d730
// fast 250; the existing s_decibels as that aggregate 19/20 (it also swaps base and index in the output loop's stores).
// Not it: early returns in the pin helper 195/70; an out pointer or reference for the pinned value 23/22; a named real
// for the load 93/96; a double scaled 24/24; (double) casts 24-25; an exponent helper 119/22 (one multiply); ln10 as a
// global or const 23-25; exp(y * log(10)) 125/28; pow 232/219; an empty inline call, an early-return helper, a constant
// ternary or a bare scope between the multiplies 23/22; the real chain copied with a real parameter 154/92, with the
// curve switch 154/92, three levels 190/106.
// Left: only the cluster pointer in EBX and the widened portal index in EBP (retail the other way), 1363 B. Measured:
// callee-saved registers go ESI, EDI, EBP, EBX by priority and the two tie; one more read through the cluster pointer
// before the loop or in its guarded block (diagnostic stores) gives retail's pair, a read after the loop does not. No
// natural form adds that reference: accessor helpers for cluster/portal/other/environment (alone, pairs, all) 15/16;
// reference-typed cluster/portal/other/paths (15 combinations) 15/16; a bit-vector type for the valid words 19/20; 80
// first-loop shapes (guard x nearest x bit test x limit x nesting) 15-17 or worse, the two-continue guard with the other
// cluster resolved after the flag test flips the pair at 1347 B; 15 decodes of the packed reference 15-16, the 16-bit
// shift forms flip the pair but emit movzx + shr (aligned 15); a named side bit 19/20; declaration orders and scopes (9)
// 15/16; the valid words as an initializer or assignments flip the pair but share a zero register (1339 B, 348/34); the
// 24 orders of the deep block's statements 15 or worse; a separate stored-index copy (9 forms) 19/20; an index local in
// the nearest block 19/20; the nearest test with the NONE test first flips the pair but changes the block order (322/30).
// Shared bitmap setters at the seed, destination or both 354/354/354. Shared tests for distance,
// sort fill or output 333/354/354, all three 333, complete bitmap API 333.
// NOT kept: the distance test shortens the function to 1347 bytes and leaves both BSP table spills.
// @retail 0x18dc90
void __stdcall function_18dc90(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
	real maximum_range, long *indices, long *gains, long *portals, long *clusters, long *count)
{
	s_portal_path_cluster *cluster = &bsp->clusters[cluster_index];
	dword valid[2] = { 0, 0 };
	real distances[64];
	long nearest_portals[64];
	long order[64];
	long nearest_clusters[64];
	real nearest_distance = FLT_MAX;
	if (cluster->environment_index != NONE)
	{
		nearest_clusters[cluster->environment_index] = cluster_index;
		distances[cluster->environment_index] = 0.0f;
		nearest_portals[cluster->environment_index] = NONE;
		portal_path_set_bit(valid, cluster->environment_index);
	}
	s_portal_path_list *paths = &bsp->environment_paths[cluster->environment_path_index];
	for (long i = 0; i < paths->count; i++)
	{
		short encoded = paths->portals[i];
		short portal_index = (short)(encoded & 0x7fff);
		s_portal_path_portal *portal = &bsp->portals[portal_index];
		long other_index = portal->clusters[(encoded >> 15) & 1];
		if (portal->flags & 0x28) continue;
		s_portal_path_cluster *other = &bsp->clusters[other_index];
		if (other->environment_index == cluster->environment_index) continue;
		real travel = function_249d60((s_structure_bsp_view *)bsp, cluster_index, other_index);
		real distance = function_14b240((s_14b240_owner *)bsp, (s_bsp3d_disk *)portal, point) + travel;
		if (nearest_distance >= distance && cluster->environment_index != NONE)
		{
			nearest_portals[cluster->environment_index] = portal_index;
			nearest_distance = distance;
		}
		if (other->environment_index != NONE)
		{
			s_portal_path_environment *environment = &bsp->environments[other->environment_index];
			real limit = environment ? environment->range : maximum_range;
			if (limit >= travel)
			{
				if (valid[other->environment_index >> 5] & (1 << (other->environment_index & 31)))
					limit = distances[other->environment_index];
				if (limit >= distance)
				{
					distances[other->environment_index] = distance;
					nearest_clusters[other->environment_index] = other_index;
					nearest_portals[other->environment_index] = portal_index;
					portal_path_set_bit(valid, other->environment_index);
				}
			}
		}
	}
	for (long i = 0; i < bsp->environment_count; i++)
	{
		order[i] = i;
		if (!(valid[i >> 5] & (1 << (i & 31))))
			distances[i] = FLT_MAX;
	}
	real scratch;
	sort_4byte(&scratch, order, bsp->environment_count, (t_portal_path_compare)function_18ae50, distances);
	if (count)
		*count = 0;
	long limit = bsp->environment_count < 2 ? bsp->environment_count : 2;
	for (long i = 0; i < limit; i++)
	{
		long index = order[i];
		if (valid[index >> 5] & (1 << (index & 31)))
		{
			indices[i] = index;
			if (gains)
				gains[i] = index != NONE ? portal_path_gain(distances[index], bsp->environments[index].range) : 0xc2800000;
			if (portals)
				portals[i] = nearest_portals[index];
			if (clusters)
				clusters[i] = nearest_clusters[index];
			if (count)
				(*count)++;
		}
		else
		{
			if (count)
				break;
			indices[i] = NONE;
			if (gains)
				gains[i] = 0xc2800000;
			if (portals)
				portals[i] = NONE;
			if (clusters)
				clusters[i] = NONE;
		}
	}
}
