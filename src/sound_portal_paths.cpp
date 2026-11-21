// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "bit_vector.h"
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

// score 287 (was 287): The 0x428 frame agrees. The starting-cluster register and both BSP base reloads still differ.
// Advancing the required index output restores gain-based output addressing.
// Frame 0x428 agrees. The starting cluster stays in EBX and both BSP table bases are cached across the portal walk.
// Earlier: Advancing the required index output restores gain-based output addressing.
// Round 192 (m3 lane 8; one-file harness, see 0x18dc90's note): on the r111 held form 244 positional, 17 aligned at
// 1364 B, the same cluster/portal-index EBX/EBP swap as the twin plus the padding it shifts; the twin's ten forms
// applied to both bodies give 244 here each, except a long portal index 323 and the cluster through a reference 244
// (aligned 21).
// Advancing the required index output restores gain-based output addressing.
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
// Direct list/portal/both indexed records/direct list and portal 358/374/392/329.
// Live-length link cursor/positive end cursor/consumed count/empty-range cursor 374/359/373/364.
// The direct list form recovers the initial EBP save; both table reloads remain unresolved.
// M3 lane 2 r106 (one-file LTCG harness with the callee files; main 272 positional for this note's 287): lane 13's r111
// held form rebuilt on this body 244 (1364 B, aligned 17); without the forms the review list reverts unless a match (the
// clear moved, the seed order, the arm order, the for-init limit) 353 (1355 B); main's body with only the callee
// change 358. See 0x18dc90's note for the register-pair screens (same leftover here).
// M3 lane 14 r118 (fast with 0x18dc90, 0x14b240, 0x249d60 and 0x18ae50; difflib-aligned in brackets, base 287 [172]):
// `dword encoded` 308 [183]; the limit as `if (limit > 64) limit = 64;` 287 [176]; the limit in the for-init after the
// counter 282 [168] (a moved declaration; retail stores the counter before the limit compare); the held r111 gain
// helpers on main's base 307 [162] (1359 B); the index advanced at the row latch as 0x18dc90 286 [173] (1364 B, gain of
// one, not kept); the gain store as if/else 288 [169].
// Sort count shared with fill 306; fill ordinal as sort scratch 319; unsigned sort count scratch 313.
// Separate output bounds, available first 287 and cap first 288; in-place cap 287;
// row ordinal or limit as sort scratch 357/319. NOT kept: the separate bounds reach retail length
// without a score gain. Starting-cluster register and BSP table reloads still differ.
// M3 lane 15 r109 (fast with 0x14b240; main 287): 0x14b240's projection through the existing vector3f_scale 287 (same
// code: an expanded helper's out-pointer gives no may-write summary). On lane 2's rebuilt r111 held form re-applied to
// this file (fast 250 at 1364 B; only the cluster pointer EBX / widened portal index EBP pair differs at the head): the
// seed's path lookup through `bsp->clusters[cluster_index]` 382 (1380 B); the cluster pointer declared ahead of the
// arrays 373 (1348 B); the seed through its own `start` pointer with the loop's pointer declared after it 382; a short
// other index 250; the nearest distance declared ahead of the cluster pointer 250; the portal looked up before the
// short index is named 334. Restored to main.
// M3 lane 15 r111 (fast with 0x14b240; main 287; held form rebuilt on this tree as build/r111/held-0x18dc90.patch, 250 at
// 1364 B): see 0x18dc90's note for the measured cause of the EBX/EBP pair. On the held form, each 250 with the pair
// unchanged: the side bit as `(encoded & 0x8000) != 0` (same code as the shifted cast); the nearest test nested; the
// guard as two continues with the destination resolved first; the seed bit written out with the index evaluated twice;
// `__assume(cluster->sound_path_index != NONE)` after the lookup; the limit as if/else. Diagnostic (wrong code): one more
// cluster field read ahead of the path lookup flips the pair to retail's, and then only the four output stores' base and
// index order is left (aligned), which one named local more or fewer swaps. Restored to main.
// Two seed-index pairs 303, first pair 303, portal/bitmap pair 287, short seed plus
// bitmap index 372. Decode ordinal at stores 287, derive it from portal position 410.
// Packed entry read at both decodes 287, direct seed and search pointer 368. Computing
// the ordinal from the table position recovers EBP but grows the body; both table
// bases still need retail's per-visit loads.
// default gain result 286, missing gain first 286, reused ordinal 287, count reset from ordinal 286, named
// bitmap parts 287, nonnearest arm first 287, separate output bounds 287, separate comparison distance 287.
// NOT kept: The gain and ordinal forms improve by one point. Separate bounds tie at 1364 bytes. The starting-
// cluster register and BSP base loads still differ.
// Required index advanced after its store 287, after the gain 302, after the portal 301, or
// after the cluster 294. Required cursor position 339, cursor end 352, position at the
// latch 343. Named gain range 288; gain range through a sound pointer 288. NOT kept: the
// position and end forms shrink the frame to 0x424 and move the length away from retail.
// The starting-cluster register and both table reloads still differ.
// M3 lane 14 r124 (fast with 0x18dc90, 0x14b240, 0x249d60, 0x18ae50; main's callee, so both table bases still hoist):
// `long encoded` with a `short portal_index` and the side bit as `((dword)encoded >> 15) & 1` together 360 (1369 B); the
// nearest test with the NONE test first 292; the deep block's two range tests as one `!valid || distances >= distance`
// condition 288; the distance sum written `travel + ...` 287 (same code); the nearest distance stored before the index
// 287 (same code). BLOCKED-BY 0x14b240's write summary (see above): every form on main's base keeps both table bases.
// Destination array names 287; distance and bitmap names 287; four ranked table roles 287; destination
// arrays before order 287, order before the distance array 287; source and destination record names
// 287; distinct phase ordinals 287, with candidate array and bitmap roles 287.
// Round 648: deferred seed portal 391; destination ordinal from its record 322; split flag predicates 311;
// bitmap word shifted at reads 338; fallback-first range 344; short-circuited range tests 287;
// defaulted output gain 286; common count update 288;
// split flags with signed link decode 356; nested flag guards 311.
// NOT kept: the 286 gain result improves one point and the 311 split flag forms keep both table spills.
// M3 lane 16 r160 (fast with 0x18dc90, 0x14b240, 0x249d60, 0x18ae50; main 287 at 1367 B, difflib-aligned 166; twin 295
// / 194): the seed stores as clusters, portals, distances 287 (aligned 165: the seed block is exactly retail's, the float
// store alone forcing the index re-read; statements reordered by score, match-only); the range clamp as `if (!(range >
// 0.001f)) range = 0.001f;` 287 (same code); the clamped range in a new local 287 at 1364 B (aligned 188, twin 303); the
// three pins through the existing PIN_126660 macro, as the looping-path code does, 287 (aligned 162, twin 294). Lane 13's
// summary form for 0x14b240 re-measured on main's bodies: this one 370 (1353 B), the twin 372 (1346 B), 0x222150, 0x12e960
// and 0x18ab70 still matched (0x222150 without its volatile reads), 0x18ae80 846 -> 911. BLOCKED-BY 0x14b240's write
// summary while 0x18ae80 rises on it.
// M3 lane 16 r162 (fast with 0x18dc90, 0x14b240, 0x249d60, 0x18ae50; main 287; each form applied to both twins, twin's count
// in brackets): the range limit as `limit = maximum_range; if (sound) limit = sound->range;` 344 [357]; the count reset moved
// ahead of the fill loop 322 [318]; the found arm's index store after the gain/portal/cluster/count stores 289 [296]; the fill
// loop's order store after the distance reset 290 [296]; the seed bit set ahead of the seed stores 375 [368]. Still BLOCKED-BY
// 0x14b240's write summary (0x18ae80's file held this round).
// M3 lane 8 r212 (h2m fast with 0x18dc90 and 0x14b240, 0x14b240 MATCH; control 287): the fill loop's order store after
// the valid test 290; the path list pointer const 287 (same code); the other cluster's sound index named once in the walk
// 322; the sound compare written cluster first 287 (same code); the found arm's index store and step as two statements 287
// (same code). Still BLOCKED-BY 0x14b240's write summary.
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

// score 295 (was 295; 305 before r204): Frame 0x42c versus 0x428. The starting cluster is in EBX and both
// BSP table bases stay live through the walk. (Earlier header: 305, was 315.)
// The 0x42c frame, starting cluster in EBX and cached BSP table bases still differ.
// Earlier comparisons were 315 and 354. Advancing the required index at the row latch brought the
// entry frame to 0x42c.
// Advancing the required index at the row latch brings the entry frame to 0x42c.
// Earlier scores were 315 and 354.
// Earlier 315 (was 354). Advancing the required index at the row latch brings the entry frame to 0x42c.
// Earlier header: 305 (was 305; 315 before r118, 354 before that): Advancing the required index at the row latch brings the entry frame to 0x42c.
// Earlier at 305 (was 305; 315 before r118, 354 before that): Advancing the required index at the row latch brings the entry frame to 0x42c.
// (A second header line, "score 305 (was 354)" with the same text, merged here in m3 lane 14 r124.)
// Frame 0x42c versus 0x428. The starting cluster is in EBX and both BSP table bases stay live through the walk.
// Prior score history: 315 before r118, 354 before that.
// Earlier 305 (was 354): Advancing the required index at the row latch brings the entry frame to 0x42c.
// Round 192 (m3 lane 8; one-file harness with 0x14b240 from the reference-bound form and the callee files, counts
// positional/aligned; on main 304/232): lane 13's r111 held form re-applied (lane 2's rebuilt patch) 15/16 at 1363 B,
// the twin 244/17. Only the cluster pointer (ours EBX, retail EBP) and the widened portal index (ours EBP, retail
// EBX) differ in both. On that form: the nearest test nested 15; the nearest distance stored before the index 16; the
// side bit named 19; the environment compare operands swapped 16; the seed's bit set through
// `bsp->clusters[cluster_index]` 359; the travel call taking `cluster - bsp->clusters` 358; a const cluster pointer
// 15; `bsp->portals + portal_index` 15; a long portal index 324 (the pair stays, the portal pointer moves); the
// implicit short conversion 15; a short other index 15; a short encoded entry 15; the cluster through a reference
// 19. Not kept (held form unchanged: needs a match).
// r118 wrote the path loop as its twin 0x18d730 does (destination resolved before the flag
// test, one nested condition, long portal index, the valid words cleared by memset). Advancing the required index at the
// row latch brings the entry frame to 0x42c.
// Retail's 0x428 frame keeps the starting cluster in EBP, reloads both BSP table bases per path entry
// and resolves the destination before testing flags. Both extra table-base spills persist.
// Before the cursor change the frame was 0x430. Tried: const portal 354; const views 354; endpoint mask
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
// The two table spills still enlarge the frame by eight bytes. Common index store 390; index and
// gain 372; cluster 376; portal and cluster 375; all optional stores 369; index and cluster 371;
// all stores with a branch-selected gain 374; index, portal and cluster 370.
// Low-byte portal flags 354; destination before low-byte flags 368. Portal array header 354,
// cluster array header 354, both 354. Packed link record 354; index and endpoint bitfields 371;
// path count and link pointer through the array header 354.
// The bitfield and earlier destination forms shorten to 1347 bytes. The 0x428 frame and table reloads remain open.
// Required index cursor at the stores 362 or row latch 315 (kept, plain); required-index
// position/end 390/388; separate sorted cursor/sorted end 369/383; scoped sort scratch or
// validity memset with the index cursor 359/359. The latch restores gain-based row addressing.
// M3 lane 2 r106 (one-file LTCG harness with the callee files; main 304 positional for this note's 315): lane 13's r111
// held form rebuilt on this body 15 (1363 B, aligned 16): only the cluster pointer in EBX and the widened portal index in
// EBP are left. Diagnostics (wrong code): one more cluster read anywhere in the loop or in the seed block, or the nearest
// block storing a constant instead of the portal index, flips the pair to retail's (aligned 2-8), so the two tie; 1-8
// dead locals do not (19). No flip from: const cluster or path list; `if (paths->count > 0)` on the loop 17; `short
// encoded`; the guard's operands swapped 16; the seed index in a local 341; the nearest index assigned in the test 19;
// the portal index at function scope (short, short = NONE, long declared first 324); the loop locals declared at the
// top; the cluster after the nearest distance or after the clear 15/17; the 23 other seed statement orders (15 at best);
// the nearest distance stored first 16; the portal record indexed from the packed value 317 (1379 B); 53 random
// combinations of 14 code-neutral toggles 15-21. Without the reverted-unless-match forms the rewrite is 351 (1354 B);
// main's body with only the callee change 349.
// M3 lane 14 r118 (fast with 0x18d730, 0x14b240, 0x249d60 and 0x18ae50; the kept form plain-confirmed with 0x18ae80,
// 0x18bb80, 0x13de30, 0x2197f0; difflib-aligned in brackets, base 315 [232]): written as 0x18d730's body (its scoped
// sort scratch and index cursor at the stores included) 291 [176] (1365 B; the scope is match-only); without the scope
// 305 [208] (1365 B); that with the index advanced at the row latch 305 [209] (1363 B, kept). Its parts alone on the old
// body: memset 358, long portal index 353, nested condition with the destination first 367, cursor at the stores 362;
// on the kept form: short portal index 356, the `= { 0, 0 }` initialiser for memset 368; memset and long index with the
// old `continue`s 340. On 305: `dword encoded` 304 [207]; the limit as `if (limit > 2) limit = 2;` 305; the limit in
// the for-init after the counter (retail sets the counter before the compare and jumps from the capped arm past the
// entry test) 300 [205]; the held r111 gain helpers (aggregate pin, fraction and curve helpers, both twins) 313 [165]
// (1354 B); the gain store as if/else 305. Left: both table bases hoisted and spilled where retail reloads them after
// 0x14b240 (see 0x18d730's note), the cluster pointer in EBX for EBP.
// Self-review: the kept form needs the destination resolved before the flag test (retail loads the cluster table base
// and forms the destination before `test byte [portal + 0x18], 0x28`); nested with the destination after the test 340.
// Sort scratch scoped at the common row latch 311; late order buffer 315; both 311.
// Signed fill-count scratch 350; scoped unsigned sort-count scratch 375; row ordinal scratch 371;
// output bound scratch 311; two bounds at the latch 361. NOT kept: the 311 forms
// recover 0x428 but gain four and leave the BSP table spills.
// M3 lane 15 r111 (fast with 0x18d730 and 0x14b240; main 305). Lane 13's r111 held form rebuilt on this tree
// (build/r111/held-0x18dc90.patch: 21 fast at 1363 B, 0x222150 MATCH without its volatile reads, 0x18ae80 846 -> 911);
// not kept, it needs a match. What is left is only the pair: cluster pointer EBX / widened portal index EBP, retail the
// other way round. Measured with counted diagnostic reads (wrong code): the pair is a use-count contest among the
// values that cross the two calls, registers handed out ESI, EDI, EBP, EBX by count, and the two tie with the tie going
// to the portal index. One more cluster field read ahead of the seed test or of the path lookup gives retail's pair
// with every other row of the head equal; a read after the loop or before the sort does not count. One more
// portal-index use (a store into a local array that kills no load) needs this many more cluster reads to flip back:
// 4 at the loop top, ahead of the calls, after them, in the destination block and in the deep update block (a use
// there counts 3, before or after the block's own use); 3 at the join after the nearest block and at the bit test
// (2); 2 in the nearest block (1). Nothing that emits no code adds a count. On the held form (21 unless given, pair
// unchanged):
// the seed bit written out with the index evaluated twice 25 (the four output stores swap base and index: one symbol
// more or fewer flips those); the side bit as `(encoded & 0x8000) != 0` 21 (same code as the shifted cast), as a
// conditional 21, with a short entry 21, as a named bool 21, as `(encoded >> 15) & 1` 22; a long portal index 325 (no
// movsx), a short masked value copied to a long 325, an inline accessor returning the short index into a short 21,
// into a long 325 (three spellings); `bsp->portals + portal_index` 21; a short other index 21; an element pointer for
// the packed entry 25; the distance summed in a second statement 21; the portal distance named 25; the nearest test
// nested 21, as `!(nearest < distance)` 305, as `distance <= nearest` 21, with `NONE !=` 21; the test-bit helper in the
// fill loop or the output loop 21/21; a redundant `if (0 < paths->count)` guard 22; a while loop 21; the limit as
// if/else 21; the guard nested 21, as two continues 21, as `== 0` 21, with its compare operands swapped 22, as one
// negated or 21; a long copy of the portal index for both stores (declared after the index, or after the distance)
// 25/25, for the nearest store only 25, for the lookup only 25; `__assume(cluster != NULL)` at three places 21;
// `__assume` on the path index (not NONE 21, in range 23); the cluster pointer declared NULL then assigned 21, as the
// base then `+=` 21, as `bsp->clusters + cluster_index` 21, declared at the top and assigned later 21; a discarded read
// or an empty test of the path index ahead of the lookup 21/21 (both removed before allocation); the seed test as an
// empty arm with an else 21, as `!(== NONE)` 21, on a long cast 21, as `NONE !=` 21, against `(short)NONE` 21; an
// unused cluster argument on an inline forwarder of 0x249d60 21 (dropped early) and on an out-of-line one 20 (the
// forwarder folds; pair unchanged); one or two more values held across the sort call 266/291 (pair unchanged).
// M3 lane 16 r146 (one-file LTCG harness on lane 13's held r111 form with the callee files, positional/aligned; the held
// form reproduces at 15/16, 1363 B, 0x18d730 244/17): only the cluster pointer in EBX and the widened portal index in EBP
// are left. No flip from: the cluster as `bsp->clusters + cluster_index` 15/16; the nearest test as `distance <=
// nearest_distance` 15/16; both range tests with their operands reversed 15/16; the seed's set-bit index as a short
// 15/16; the other cluster index as a short 15/16; the portal index cast to short before the mask 19/20. Diagnostics:
// the loop's cluster compare through `bsp->clusters[cluster_index]` 371 (1393 B); a portal-index test added 351 (1380 B).
// Main's body unchanged at 305.
// Named output homes 305, separate rank and missing-distance fills 332/336.
// Selected sort key 681, cap-first empty arm 311, empty-first cap 310.
// While with common latch 305, positive bound scope 310. The 0x42c frame and both
// BSP table lifetimes remain.
// widened link with short ordinal and logical shift 356, unsigned word link 356, endpoint by sign 355, unsigned
// portal ordinal 305, early short side 305, early logical side 356, pointer sort scratch 305, cap-first count
// 305.
// NOT kept: The 0x42c frame and two cached BSP bases persist.
// Independent blocked flag bits 320; portal lookup before named ordinal 305; both distance declarations
// at the iteration entry 305; named path eligibility 305. Shared squared power 305; separate curve
// terms 305; curve subtraction assignment 305; pinned curve initializer 305. The 0x42c frame, starting
// cluster in EBX and two cached table bases persist.
// M3 lane 14 r124 (fast with 0x18d730, 0x14b240, 0x249d60, 0x18ae50; main's callee): `long encoded` with a `short
// portal_index` and the side bit as `((dword)encoded >> 15) & 1` together 356; the nearest test with the NONE test first
// 310; the deep block's two range tests as one `!valid || distances >= distance` condition 306; the distance sum written
// `travel + ...` 305 (same code); the nearest distance stored before the index 305 (same code). BLOCKED-BY 0x14b240's
// write summary, as for the twin.
// Candidate table names in distance/portal/order/destination and rank/destination/portal/distance
// order 305/305; walking record roles alone and with candidate arrays 305/305; distance and order
// declarations first or last 305/305; distinct traversal/fill/output ordinals 305, with path
// measurement names 305.
// Rank-fill cursor 321, distance-fill cursor 321, both 337, ordinal from rank pointer 339.
// Rank storage first declared at the fill with rank/key/both cursors 321/321/337.
// Both fill cursors with the two-row condition in the output loop 334. All keep the 0x42c
// frame and the cached BSP bases.
// Round 648: deferred seed portal 396; destination ordinal from its record 364; split flag predicates 320;
// bitmap word shifted at reads 348; fallback-first range 346; short-circuited range tests 305;
// defaulted output gain 308; common count update 305.
// M3 lane 8 r204 (fast with 0x18d730, 0x14b240, 0x249d60, 0x18ae50; main 305): kept, the three hand-written valid-bit
// tests through include/bit_vector.h's existing bit_vector_test 295 (the destination block's test alone gives the gain:
// retail tests the environment index it has already widened instead of reading the field again; the fill and output
// tests give the same code); in the twin the same helper gives 309 (its destination test alone 309, the other two 287),
// so the twin keeps its spelling. Tried on main's 305 body: the path list read through the bsp at each use 357; the valid
// words cleared by two assignments 368 (1350 B); `bsp->portals + portal_index` 305; the distance summed `travel + ...`
// 305 (same code). On 295: the setters through bit_vector_set (seed, destination, both) 295 each (same code); `*indices++`
// at the stores 295 (1365 B); the limit capped by an `if` 295; `count >= 2 ? 2 : count` 295 (same code); the sort scratch
// in its own block, the twin's spelling, 280 (frame 0x428 as retail, first difference +0x16; left the cluster pointer in
// EBX and the cached table bases; a scope that moves a stack home, match-only, not kept).
// Reacquire destination after travel, geometry, or at the range gate 359/359/359;
// reacquire start after travel or geometry 369/369; reacquire both after travel, geometry,
// or at their consumers 405/370/366. NOT kept: all raise the score and retain the extra frame word.
// M3 lane 8 r206 (h2m fast with 0x18d730 and 0x14b240; 0x14b240 MATCH, 0x18d730 287 each time; control 295): the frame
// word is the cached cluster-table base (spilled at +0x24; retail reloads `bsp` and both table bases each entry, and
// 0x14b240 takes register arguments, so it is a link-time-compiled callee whose body retail summarised as writing memory).
// Diagnostic, reverted: 0x14b240's in-plane offset through a reference-bound local alone (bytes unchanged) 372 (1346 B;
// 0x18d730 370, 0x18ae80 846 -> 911), so today's body still needs the held rewrite. Tried: the sort scratch as `long`
// 295 and declared at the top 295 (same code); the output counter declared `long i = 0;` ahead of the limit with the
// loop as `for (; i < limit; i++)` 290 (retail's counter store before the limit compare and the capped arm threaded past
// the entry test; under the bar, not kept), ahead of the count reset 294; on the 290 form: `dword encoded` 291, the count
// reset after the limit 300, a named environment element pointer for the gain (retail forms that element's address in a
// dead `lea`) 290 (same code), the limit capped by an `if` 295, the counter declared ahead of the sort 296. The gain
// helper's degenerate arm is laid out first here and second in retail (an arm swap with the test negated, match-only).
// M3 lane 16 r162 (fast with 0x18d730, 0x14b240, 0x249d60, 0x18ae50; main 295; each form applied to both twins, twin's count
// in brackets): the range limit as `limit = maximum_range; if (environment) limit = environment->range;` 357 [344]; the count reset moved
// ahead of the fill loop 318 [322]; the found arm's index store after the gain/portal/cluster/count stores 296 [289]; the fill
// loop's order store after the distance reset 296 [290]; the seed bit set ahead of the seed stores 368 [375]. Still BLOCKED-BY
// 0x14b240's write summary (0x18ae80's file held this round).
// Shared ordinal for fill/output 295, walk/fill 295, walk/output 299, or all three 299.
// Advance portal and cluster outputs at their stores 291, gain and portal outputs 286, all
// optional outputs 318, or all with shared fill/output ordinal 318. NOT kept: the 291 form
// gains four. The 286 form restores the 0x428 frame but grows to 1372/1363 bytes, advances
// outputs at their stores and leaves the BSP base loads and starting-cluster register unresolved.
// M3 lane 5 r223 (H2M_FAST with 0x18d730 (287) and 0x14b240 (MATCH); control 295, aligned 194 / register-masked 155):
// the deep block's environment index named once (`long environment_index = other->environment_index;`, six uses)
// 389 (1318 B); the start cluster's environment index named for the seed block 340 at 1363 B; both 389 (1330 B);
// 0x14b240's in-plane subtraction through the existing vector3d_from_points3d 295 (0x14b240 still MATCH, same code
// here: an expanded helper's out-pointer gives no may-write summary, so the table bases stay hoisted); the packed
// entry as `long encoded` alone 297. Body unchanged; BLOCKED-BY 0x14b240's write summary as before.
// M3 lane 8 r212 (h2m fast with 0x18d730 and 0x14b240, 0x14b240 MATCH; control 295): the fill loop's order store after
// the valid test 296; the path list pointer const 295 (same code); the other cluster's environment index named once in
// the walk 342; the environment compare written cluster first 295 (same code); const portal and other-cluster pointers by
// pointer addition 295 (same code). Still BLOCKED-BY 0x14b240's write summary (ours hoists both table bases out of the
// walk and spills one, frame 0x42c).
// @retail 0x18dc90
void __stdcall function_18dc90(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
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
		long portal_index = (short)(encoded & 0x7fff);
		s_portal_path_portal *portal = &bsp->portals[portal_index];
		long other_index = portal->clusters[(encoded >> 15) & 1];
		s_portal_path_cluster *other = &bsp->clusters[other_index];
		if (!(portal->flags & 0x28) && other->environment_index != cluster->environment_index)
		{
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
					if (bit_vector_test(valid, other->environment_index))
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
	}
	for (long i = 0; i < bsp->environment_count; i++)
	{
		order[i] = i;
		if (!bit_vector_test(valid, i))
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
		if (bit_vector_test(valid, index))
		{
			*indices = index;
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
			*indices = NONE;
			if (gains)
				gains[i] = 0xc2800000;
			if (portals)
				portals[i] = NONE;
			if (clusters)
				clusters[i] = NONE;
		}
		++indices;
	}
}
