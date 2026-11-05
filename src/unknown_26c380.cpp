// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_26C380.CPP: traces along a line across the sectors of the
   pathfinding data, edge by edge */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector2f_products.h"
#include "globals.h"
#include "unknown_1fa590.h"
#include "unknown_26c380.h"
#include "path.h"
#include "vector_products.h"

bool function_270600(long node_index, s_path_location const *location);

/* an edge as the traces read it: its flags, and the sectors on either side */
struct s_sector_edge
{
	word vertices[2];
	word flags;
	word unknown06;
	word next_edges[2];
	word sectors[2];
};

static inline real dot_product2d(point3f const *point, vector2f const *vector)
{
	return point->x * vector->i + point->y * vector->j;
}

static inline void vector2d_from_points3d(point3f const *p0, point3f const *p1, vector2f *out)
{
	out->i = p1->x - p0->x;
	out->j = p1->y - p0->y;
}

static inline real magnitude3d(vector3f const *v)
{
	return (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
}

static inline void function_x697631(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

static inline void point_from_line2d(point2f const *point, vector2f const *vector, real t, point2f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
}

/* starts walking the edges of a sector (and first those of its surfaces) */
__forceinline void edge_iterator_new(s_edge_iterator *iterator, s_pathfinding_data const *pathfinding,
	long sector_index, bool surfaces)
{
	long first_edge = pathfinding->nodes[sector_index].first_edge;
	s_pathfinding_node const *sector = &pathfinding->nodes[sector_index];

	iterator->next_edge_index = (word)first_edge;
	iterator->first_edge_index = (word)first_edge;
	iterator->surface = sector_index;
	iterator->edge_index = (word)NONE;
	iterator->vertex = iterator->previous_surface_index = NONE;
	iterator->pathfinding = (s_pathfinding_edges_view const *)pathfinding;
	iterator->surface_index = surfaces && pathfinding->surface_count > 0 ? sector->first_surface : NONE;
	iterator->count = 0;
}

/* where the trace crosses an edge of a sector, when the edge's two ends
   (point0, point1) lie on either side of the trace's line (distance0 <= 0 <=
   distance1): the sector beyond the edge, whether the trace is blocked there,
   and the point and distance along the trace */
// @retail 0x26c380
inline bool function_26c380(s_pathfinding_data const *pathfinding, point3f const *origin, bool forward,
	long sector_index, long edge_index, vector3f const *direction, long *next_sector_index,
	real distance0, real distance1, point3f const *point0, point3f const *point1,
	s_sector_trace_result *result)
{
	bool success = false;

	if (distance1 >= 0.0f && distance0 <= 0.0f)
	{
		s_sector_edge const *edge = &((s_sector_edge const *)pathfinding->edges)[edge_index];
		long next = edge->sectors[!forward];
		vector2f edge_vector;
		vector2f offset;
		real denominator;

		if (next == NONE || next == (word)NONE)
		{
			result->blocked = true;
		}
		else
		{
			s_pathfinding_node const *sector = &pathfinding->nodes[next];

			result->blocked = !(sector->flags & 1) || ((sector->flags & 2) && function_1fa6b0(pathfinding, sector));
		}
		result->sector_index = sector_index;
		result->edge_index = edge_index;
		vector2d_from_points3d(point1, point0, &edge_vector);
		vector2d_from_points3d(point1, origin, &offset);
		denominator = cross_product2d((vector2f const *)direction, &edge_vector);
		if (denominator != 0.0f)
		{
			result->distance = cross_product2d(&edge_vector, &offset) / denominator;
			point_from_line2d((point2f const *)origin, (vector2f const *)direction, result->distance,
				(point2f *)&result->point);
			result->point.z = origin->z;
			result->distance = 0.0f > result->distance ? 0.0f : result->distance;
		}
		else
		{
			result->distance = 0.0f;
			result->point = *origin;
		}
		if (next_sector_index)
		{
			*next_sector_index = next;
		}
		success = true;
	}
	return success;
}

// score 244 (was 410): the held 34 form (see the coordinator note below) with its two goto exits written as breaks.
// Earlier: frame 0x60 versus 0x64; full/planar normal overlap, pathfinding/zero roles, byte flags and tails differ.
// Earlier: frame 0x5c versus 0x64; full/planar normal overlap, pathfinding/zero roles, byte flags and tails differ.
// pathfinding pointer in EBP and the -1 edge seed in EDX where ours keeps them in EBX
// and EDI, and retail keeps a byte local at [esp+0xe] where ours has a long zero at
// [esp+0x14].
// Earlier 419 (was 419): frame 0x5c versus 0x64, independent normal homes, memory flags and distinct exit
// Frame 0x5c versus 0x64, independent normal homes, memory flags and distinct output tails remain.
// Earlier: frame 0x5c versus 0x64, independent normal homes, memory flags and distinct exit
// tails remain; tried late first independent normal 490; late first native result word 438; native edge
// orientation 438; existing planar product inline 435; late first unsigned count 435.
// frame 0x5c versus 0x64; normal, result/first/hit homes and exit tails differ.
// Count byte 451; first word 443; success declared first 435; previous edge split 440; hit byte 435.
// Hit at producer 435; sector history storage 435; orientation word 434; planar scalar/value inputs 435/538;
// magnitude value input 556; unsigned success 435; normal length result 435.
// Final blocked output 658; signed result byte 460; endpoint array index 443; direct node flags 435;
// vertex base 458; accepted distance 476; vertex base plus distance 501; endpoint index plus distance 432.
// nonempty edge while 435; rejected hit reset 460; blocked success test 529; count first progress guard 435;
// edge endpoint homes 435; iterator storage lifetime 435; paired edge workspace 435; overshoot expression 434.
// Iterator native node/first seed/both 433/435/433; count traversal scope 419 kept (plain);
// progress-history scope 433; native planar/full-normal copies 490/505; planar input order 435.
// Only constant counter initialization moves into the valid-normal walk. Frame 0x5c versus 0x64
// and normal/result/exit homes remain.
// Existing point producer output return alone/with forced point expansion/with magnitude/with point and
// magnitude/with cross/with point and cross/with magnitude and cross/with all three 419 each; every sibling
// retains its preview baseline. Original helpers restored.
// Native cross axis/direction/both values 522/522/579; planar point2 input 419; planar plus axis/direction/both
// 522/522/579; orientation at every use 418 (one-point gain rejected).
// Existing planar point/vector and magnitude input reference contracts, singly and in all seven combinations, 419 each;
// existing cross-producer output reference 419. Original helpers and body restored.
// Complete named products: planar/cross/magnitude 421/455/421; planar and cross 453;
// planar and magnitude 419; cross and magnitude 453; all three 455; point offsets 418. All restored.
// Edge-test / iterator / passability call boundaries, ordinary inline versus noinline,
// eight combinations 419/435/460/460/435/435/460/460. All contracts and bodies restored.
// Complete endpoint stages 421; planar sum accumulator 421; normal-first products 419;
// stages with accumulator / normal-first products 421/421; named completed projection 421;
// stages with forced planar expansion 421; stages with native planar memcpy 483. All restored.
// Round 129, traversal-count scope retained: native planar copy with long/bool/short/byte
// result 479/543/483/427; CRT planar copy with those result types 479/543/483/427.
// All original executable source restored.
// Round 223: explicit false results at all three false tails, long/bool/short/byte
// 554/583/546/561; same result forms with independent planar memcpy 510/544/503/517.
// Distinct tails collapse further; frame and result homes remain unresolved. All restored.
// Round 234: normal/endpoint-scan/node-admission consumption endings and combinations
// 421/418/421/418/421/418/421; all endings with Boolean success 583. Original body restored.
// Round 226: Complete endpoint/distance, progress-distance and sector-history names,
// all seven subsets and the complete Boolean-role group 418 each. All names and body restored.
// Round 254: magnitude accumulator/squared-sum/real-result/accumulator-result
// 428/418/423/425; each with all three product parts 426/417/417/428. One-point
// reductions rejected; original helper, normal storage and body restored.
// Round 236: shared vector type / squared-length helper / point setter / vector+length /
// vector+point / length+point / all / all with native normal initialization
// 418/418/469/418/469/469/469/469; all restored.
// Round 239: actual blocked/admission/both byte fields with long result 418/418/418;
// with bool result 577/583/577; both char fields with long/bool result 418/577. All restored.
// Blocked-field forms also move the matched edge helper to 10.
// Earlier 404 (was 418): the trace result fields are stored in retail's order, the traversal count is an
// int, the first flag is a word, the edge direction a byte, and the accepted and blocked paths return
// their result directly. The frame 0x5c versus 0x64 and the remaining field and register rotations remain.
// Round 205: complete scalar input reads in cross first/second/both 492/472/482;
// magnitude alone/with first/with second/with both 400/492/469/482;
// all with planar input reads 482. Four-point magnitude gain rejected; original producers restored.
// Round 215: first-edge constant at sector/pre-iterator/pre-edge phases in word storage 426/426/427;
// bool storage 420/420/429; original-phase bool 417; pre-iterator byte 420. Original first word and
// producer phase restored; frame 0x5c/0x64, full/planar overlap and byte/tail homes remain.
// Round 434: valid-walk edge seed, post-initialization success/sector seeds and all domain subsets
// 422/404/421/404/421/403/422/422; last includes the early planar declaration. Frame remains 0x5c at 1605 bytes.
// Post-initialization success plus sector 403 is a rejected one-point form; original complete body restored.
// DL202: CRT planar transfer 440, frame 0x60 and seven additional caller losses;
// existing magnitude/planar/both widened results 404/495/495; unused cross return
// removed alone/with magnitude/with planar/with both 404/404/495/495.
// Original helpers, result contracts and body restored; frame 0x5c versus 0x64,
// independent normal homes, byte state and separate output tails remain.
// Round 245: complete point/vector captures via native/const copies, aggregate inputs
// and complete output publication 412/412/453/453/458/458/464/464; mixed input
// constructions 492/488. Native copies gain 31 but demote matched 0x26ccb0;
// output publication demotes eight controls. Original helper and entire body restored.
// Round 259: native returned cross vector, component/aggregate construction, mutable/const
// normal and explicit/aggregate planar initialization, eight combinations 597 each.
// All original helper contracts, normal storage and executable source restored.
// Round 65 (lane 16): the edge-loop first flag as bool (it was a word) 443 -> 419 kept (1605 bytes; byte first
// the same code); the file's private cross3f copy replaced by the shared vector_products.h cross3f 443 (identical
// code, kept), shared cross3f_staged 443; last_distance declared with the entry locals 442, on the bool form 418
// (one point, not kept); on the bool form count as long 419, success as bool 428 (1568 bytes); a redundant
// `success = false;` before the final tail (retail re-stores its memory byte there; diagnostic) 452. Retail keeps
// success, first and hit as memory bytes, pathfinding in EBP and the zero in EBX.
// On the bool form: the one-use edge-index and sector-index locals read in place 419 each and together
// (identical code, kept); the blocked-flag and vertex one-use locals read in place 429 each, all four 429
// (1611 bytes); forward as bool 526 (1690 bytes).
// Round 67 (lane 16, fast; kept forms confirmed plain): retail compares the traversal count as a word (`cmp word
// ptr [esp+0x18], 0` and `0xc8`, then `inc dword ptr`), so the count is a short: 419 -> 412 kept; the entry
// result stores in retail's order (FLT_MAX, blocked, sector, edge, distance, unknown1c) 412 -> 410 kept. On
// those: the count declared at the top `= 0` 429; `bool first = true` at its declaration (retail stores it at
// the top of each sector iteration) 412; the line normal and distance as a two-member plane (n, d) 454 (1651
// bytes, frame 0x64); the final tail's stores in retail's order (blocked, edge, sector, distance) 410 and tail
// C's (blocked, edge, distance) 410 (no gain); `bool success` 432 (1568 bytes); `iterator.forward` read at
// each use 526 (1696 bytes, 0x26ccb0 209). Retail keeps `hit` in the dead pathfinding parameter slot (+0x74),
// reaches the hit block only from the edge loop's break, and returns constants from separate tails.
// Round 478: cross component publication ikj/jik/jki/kij/kji 450/444/449/451/451;
// planar j/i transfer with ijk/kij/kji cross publication 441/449/449. Two-point gain rejected.
// Frame 0x60 versus 0x64 and normal/result/tail homes remain; original helper and entire body restored.
// Round 233: CRT memmove planar-normal transfer, long/bool/short/byte success: 460/544/462/520;
// with actual multi-use vertex-table owner: 446/545/439/526. Copy-only long/short reach retail
// frame 0x64 but regress; all eight lose fourteen matches. Original complete body restored;
// full/planar normal homes, byte state and separate output tails remain.
// Round 70 (lane 16, fast): retail reaches the hit block only from the edge loop's break, runs `++count`, then
// re-tests `hit` (no-hit tail) before the loop condition; one no-hit tail serves both "no first edge" and "no hit";
// every tail but the final one returns a constant, and the final one reads a stack byte that is only ever stored 0
// (at entry and at the two breaks). The hit block inside a `while (edge)` edge loop (`++count; break;`) with one
// `if (!hit)` tail after it and the two outer breaks as gotos (diagnostic) 483 (1508 bytes; retail's hit, increment
// and re-test sequence appear); that with constant returns and a bool success 604 (1417 bytes, frame 0x58: success
// folds and the false tails merge), plus `success = false;` at the two breaks 604 (folded away). Value difference:
// the no-target over-distance tail stores `(dword)NONE` (0xffffffff) where retail stores 0xffff; `(word)NONE` there
// 429 (1585 bytes: that tail then merges into the final tail), not kept. `edge` declared first 409.
// Round 506: common second-endpoint producer, native orientation/vertex-table/paired-scope
// combinations 524/524/528/528/518/518/416/416; best graph with complete planar library
// copy 444. Original producers and full body restored; frame, byte state and tails remain.
// DH round 279: Current-sector parameter ownership with long/bool/byte/word success and valid-walk/entry
// next-sector declarations; all forms also lose four matched callers [426, 422, 424, 430, 426, 422, 424,
// 430]. Original executable body restored.
// Round 236: Native first endpoint value with original/long count, then staged cross,
// 667/670/667/670; both endpoint values with those same subsets 671/680/671/680.
// First-only frame 0x68 loses twelve matches; paired frame 0x74 preserves matches.
// Original pointer geometry and complete body restored; full/planar normal homes,
// byte state and separate output tails remain.
// DH281: Complete endpoint-source/first-edge-source/hit-tail priorities, seven subsets then all with native
// Boolean success; the one-point 409 form is below the retention gates [446, 410, 449, 419, 441, 419, 444,
// 409]. Original executable body restored.
// P262: Native counted sector for with short/word/long/dword count and long success
// 410/410/409/409; with Boolean success 448/448/446/446. Original while,
// count scope, long success, normal storage and every output tail restored.
// P264: negative edge-search outcome bool/byte/short/long, long success
// 423/423/404/409; Boolean success 458/458/437/443. Six- and one-point reductions rejected;
// original positive hit, normal storage, tails and complete body restored.
// P266: Native two-component normal-copy for with long/dword/short/word ordinal
// 459/459/464/464; plain while 459/459/464/464. Frame 0x64 recovered,
// but all grow to 1667 bytes; original scalar transfers and complete body restored.
// DH292: Pending-sector parameter owns the genuine next-sector query output, long/bool/byte/short
// success with short count 440/444/433/439; with long count 449/447/442/453.
// Original pending/current histories, normal homes, count, result and complete body restored.
// DH294: Shared terminal-point producer with long/bool/byte/short success and bool first
// 410/432/414/419; word first 442/415/396/447. Original point calls with byte success/word
// first also 396 at 1606/1605 bytes. Not kept (coordinator): retail keeps first as a byte (byte
// stores of 1/0 to [esp+0xf], read through al), so the word is a score-picked type; byte success
// alone (retail's byte at [esp+0xe]) is 414. Both declarations restored at 410.
// DH296: Outer input-admission conjunction partitions 1-7 all 410; fully
// phased admission with byte success 414 (1598 bytes). Original guards and body restored;
// independent normal homes, frame 0x5c/0x64 and result/first/hit tails remain.
// DH303: Common first-endpoint producer, native orientation/vertex-table ownership,
// and planar memcpy 465/467/449/440/413/406/388/392. NOT kept: all lose four matched
// callers. Original body restored; normal/planar homes and byte/output tails remain.
// M3 lane 13 r54 (fast): `first = true` at the top of each sector iteration (retail's store before the flag
// test) 412; the sector flags read without the sector pointer 425 (1611 bytes); the hit test without its bool cast
// 410 (same code); the blocked result assigned directly without its bool temporary 425 (1611 bytes); the count
// initialized at its declaration 410 (same code). Retail's separate planar normal copy and byte success remain.
// B383: Standard forward/backward copy of the genuine two planar components,
// each with long/bool/byte/short success: 432/550/529/437/432/550/529/437.
// NOT kept: all forms lose ten matched callers and regress; original scalar transfer, result and whole body restored.
// DH312: Terminal-point actual component-array loops with long/dword/short/byte
// ordinals: streaming 478/478/549/549; complete native point publication 537 each.
// NOT kept: every form regresses the other matched helper user. Original shared
// producer and complete body restored; independent normal homes and byte/tail state remain.
// M339: Raw edge-query positive/complementary test-owned hit in bool/byte/short/long:
// 446/452/452/452/446/452/452/452. NOT kept: test-produced flags did not match; original full body
// restored.
// Round 202 (lane 3, fast): the full normal declared at function scope 410, the planar normal at function scope
// 410, both 410 (frame stays 0x5c: retail keeps a separate planar copy made with integer moves at 0x34/0x38 from
// the full normal at 0x40); last_distance declared with the entry locals 412; the count and next-sector locals
// initialised at their declarations 410 (same code). Restored.
// M3 lane 2 r47 (fast): `bool success` 432 (1568 bytes, frame 0x58, aligned 280 -> 260); with `first = true` at its
// declaration 437; the no-edge exit's temporary dropped (`success = result->blocked;`) 425 (1611); `success = true;`
// there 410 (1598, aligned 268); `success = true;` in the no-hit exit 409; both 412 (1605). Retail keeps the zero in
// EBX (byte stores through BL) and pathfinding in EBP where ours swaps them.
// M339: Raw edge-query positive/complementary test-owned hit in bool/byte/short/long:
// 446/452/452/452/446/452/452/452. NOT kept: test-produced flags did not match; original full body
// restored.
// M3 lane 7 r126 (fast, plain-confirmed for the kept form): retail's EBX zero is `edge = NULL` (edge lives in EBX from
// entry: `test ebx, ebx; je; test byte ptr [ebx + 5], 2`, spilled to +0x14 in the inner loop), which also feeds the BL
// byte stores and the count store; pathfinding stays in EBP throughout. Ours keeps edge in memory, the zero in EBP and
// shares EBX between pathfinding and edge (reloaded from its home twice). The file's private magnitude3d copy replaced
// by the existing 0x24590 410 (same code, inlined; NOT kept: the new caller raised 0x15f3a0 41 -> 42 in the full check);
// on it `bool success` with `short count = 0` at entry 550 (1472 bytes); `short count = 0` with the entry locals 425
// (1615); `edge = NULL` declared first 427 (1615); `vector2f line_normal = { normal.i, normal.j };` 427 (1615).
// Round 206 (m3 lane 3, fast; aligned = difflib count, base 280): 0x18c250's cross operand spelling (diagnostic)
// 410 (same code: the x87 loads follow the pointer loads, retail loads `direction` before the global); constant
// `return true` in the four blocked/no-edge tails 499 (1526 bytes); `first = true` before the first-edge fetch
// 412; the planar normal as `{ normal.i, normal.j }` 410 (same code); as `*(vector2f const *)&normal` 462 (1667
// bytes, frame 0x64 and retail's integer-move planar copy at 0x34/0x38, aligned 255, but the success tail is
// duplicated), with the constant tails 432 (1566), with the blocked result without its temporary 462.
// Original body restored.
// DH349: Complete sole-return outcome on native short-count/initial-store graph,
// default-owned long/bool/byte/short 428/449/480/435; terminal-owned counterparts
// 454/486/466/443. NOT kept: all regress; independent normal homes, frame and byte
// tails remain. Original result sources, early return, helpers and full body restored.
// N361: Common accepted-block state/result publication, long/bool/byte/short result,
// original planar transfer 422/444/424/412; native planar record transfer 436/375/402/441.
// NOT kept: 375 combines individually regressing storage forms and adds a redundant blocked store;
// retail has distinct accepted epilogues. Original geometry, state, transfers and full body restored.
// DH351: Existing shared planar products at origin/first endpoint/second endpoint,
// seven subsets, then complete immutable projection distance with unused overload removed:
// 410 each. NOT kept: no gain; full/planar normal homes, frame and byte/output tails remain.
// DH356: Planar components initialized before the full-normal magnitude consumer,
// scalar/aggregate crossed with long/bool/byte/short success: 410/432/414/419 twice.
// Original body restored; no independent normal homes or native frame recovered; all peers hold.
// M3 lane 12 r90 (fast; masked = register-masked aligned count): retail's +0xe stack byte is a real `bool success`, false at entry and at the two
// breaks, with one `return success;` (`success = X; goto done;` in each tail): retail's `mov al, [esp+0xa]` final tail, `mov byte ptr [esp+0xe], 0`
// at the break and constant `mov al, 1`/`xor al, al` tails all appear. The hit block inside the edge loop with `goto finish` at the two breaks and
// literal returns 558 (1441 bytes, frame 0x60); long/byte success 558/558; `{ normal.i, normal.j }` planar normal 597 (frame 0x58); do-while 552. The same
// with the success flag and goto done 540 (1464), long/byte success 486/522 (long: frame 0x64), count at the loop 525, first after the fetch 537; the final
// tail copied at the breaks 536 (0x26c4e0 16), explicit no-edge tail and do-while 532 (1482, masked 319), both 526; the no-hit tail at a label after the final
// tail 544, with copied tail 547. In the 532 form tail 3 merges into tail 1 and the no-hit tail into tail 2; where `success = X;` sits in a tail decides this:
// success first in tail 1 or tail 3 427 (1587), both 533, tail 2 426, no-edge 428, no-hit 486 (1665), no-edge + no-hit 485, tail 2 + no-hit 515. All 32 masks 0-31 with a bool
// forward and long vertex-index arms 485/547/486/548/416/473/487/546/486/548/487/549/487/545/585/646/414/472/415/473/487/546/572/632/415/473/416/474/562/621/667/728; retail's length 1605 at masks 4/16/18/24/26 (masked 378-381). On mask 16, where 0x26c4e0 goes to 16: the iterator's
// first edge in a long local 415, no long arms 421, byte forward 458, byte/long success 502/550, first at the edge 417, distance1 moved 414. `helper(..., result->distance, ...)` in
// any tail 532 (no merge change). On the 427 form: bool forward 480 (1535), with the first edge in a long local 481, long vertex arms 486, all 487. The current layout with bool success and constants 472 (1535, frame 0x58, 0x26c4e0 16), long/byte 502/502, count at entry 467. Loop condition `hit && next != NONE` with `if (hit) final else no-hit` after it (retail's tail order, last tail no-hit) and an `if (hit)` block after
// the edge loop 586, single exit 520 (masked 276), first edge local 520 (masked 269); hit block inside the edge loop with a goto into the final arm 587, 551, 586, 556. The current long success and
// (dword)NONE in the third tail (a value retail does not store) both only stop tails merging. Still open: why tails 1/3 and 2/no-hit stay unmerged at retail's frame 0x64, and pathfinding EBP against EBX.
// DH359: Complementary whole-trace outcome long/bool/byte/short, entry seed
// 583/568/568/572; post-output seed 584/569/569/573. NOT kept: no byte match;
// original positive result, normal homes, tails, helpers and complete body restored.
// DH361: Pending-sector reset after node/metadata/destination/iterator/fetch/empty-exit/
// nonempty-entry phases 410/449/436/440/442/432/432; traversal-header declaration 411.
// NOT kept: no native-frame or score gain; original reset, normal homes, tails and body restored.
// DH363: Native coordinate unions at magnitude/planar product/full-to-planar stores,
// subsets 1/2/4/3/5/6/7: 410/410/425/410/425/425/425; all with staged cross 425.
// NOT kept: planar stores grow to 1611 bytes without native frame recovery; original full body restored.
// DH367: Actual edge flag high/low byte views, unsigned/signed metadata and low sign test:
// 410/406/410/404/410/410/406/406. NOT kept: best six-point gain is below the eight-point gate;
// original edge word, frame 0x5c/0x64, overlapping normal homes and complete body restored.
// DH369: Standard magnitude/point-projection reductions, native/direct/named
// nonoriginal pairs: 584/585/426/657/657/426/657/657. NOT kept: all regress and grow;
// magnitude-only forms change ret 0x18 to 0x1c and lose seven matched callers. Original
// helpers/body restored; frame 0x5c/0x64, normal overlap and byte/output tails remain.
// DH369 revisit: Full-normal magnitude-admission positive/complementary results,
// bool/byte/short/long, eight at 416 (1611/1605). NOT kept: all regress; frame
// stays 0x5c versus 0x64. Original admission and complete body restored; normal/planar
// homes and byte/tail state remain. All calibrated file/caller/provider scores hold.
// M3 lane 9 r88 (fast unless marked; 157 plain; masked = register-masked aligned count; every form below keeps 0x26c4e0/0x26ccb0 unless noted). What took 410 to 157 together: the hit block inside
// the edge loop with `success` the only result and one `return`, `inline` on 0x26c380 (a callee kept out of line tells its caller nothing, so nothing stays in XMM7 across it), the planar normal copied as a
// struct, the count declared at the top, distance1 assigned in each arm (retail's partial products in the arms), the blocked store chained with success, the iterator setup's long first edge, and the
// statement order in the entry and the iterator. Steps: `bool success` 432; count at top 430; struct planar 464; bool+planar 385; long arms 382; distance1 in arms 556 (origin moved to a register, callers lost); that
// with bool forward 524; rewrite 409 (masked 145); `for` edge loop 409; do with guard 397; two explicit no-edge tails 414, with success first 508; edge declared first 343, before the loop 307, in the magnitude block
// 320; distances as an array 451; no sector local 388; chained first/next store 307; first-edge local 374; plain inline iterator 307; sector taken from next before the indices move 396 (retail's EBX zero); flag computed
// from next 396; index copy 377, const reference 377, surface first 314; per-iteration index local 402, 405; next seeded from current 402; guard around the while 408 (same code as plain while once the rest is in);
// distance declaration scopes 408 each; out-parameter distance helper 408; end point + distance struct 337 (frame 0x5c), one-element array 408; XMM7 probes: k extra locals 408-428; inline-asm clobber of XMM7 in
// 0x26c380 (diagnostic, removed) 410 masked 64, the same effect as `inline`. On `inline`: dot helper y-first/vector-first 410, named x,y or named result 410 masked 64, accumulated 409; grid edge top/late/magnitude by
// sector local/none/index local/from next 362 379 410 245, 419 425 397 245, 362 379 397 245. Tail C in retail's printed order merges it into tail A (486, 1485 bytes); final tail order 379; chained result 371; with
// success-first masks 372 476 372 467 445 373 447 373 468 373 553 446 374 448 553 (1,2,4,8,3,5,6,9,10,12,7,11,13,14,15). First arm: `vertices + word ternary` 360 (1605); pointer ternary or if/else 522-538 (the arms equal
// retail's but forward leaves BL, edge takes EBX, NONE EDI, tails merge to 1448), the same at both arms or through an inline helper 526; the distance statement in both forward arms 157 (kept, forward in BL). Entry
// orders: distance stored first 286 (entry lines exact), 8 others 282-376 on 360, 12 others 157-160 on 157. Line struct (normal, offset) 295 at 1605: previous/normal/offset slots then equal retail's, but the second
// distance's products flip (`mulss xmm3, [edx]` lost); dot forms on it 295-311; a line distance helper 295/309. Removing tmp3 314; arm 1 pointer if/else 475, word ternary 497. Literal returns per tail 442 472 446 472 447,
// all 394. Two-level exit by a flag 345, by clearing next then break 250 (the `finish` label stays). Parameter itself as current 391/402 loses 0x26c4e0 and 0x26ccb0. `if (hit) continue;` shapes 382-419.
// The rest of r88 on that 157 (plain) to 34 (plain): success-first masks over tails A,B,C,H 178 123 279 171 192 257 387 158 179 131 280 172 193 258 388 (mask 2 at 1605); word-ternary first arm by the same masks plus none 282 285 285 360
// 285 286 360 466 285 286 286 362 286 287 362 467; the first arm with a dot in each forward arm and `distance0 -= line_distance;` after them 40 (1605, XMM0 and every tail but C as retail), with mask 2 74, one dot after pointer if/else 535; plain dot
// helper 36; callee parameters pathfinding, origin, forward 34 (retail sets EDI ahead of EDX; 0x26c380 still matches). On 34: named dot result with named magnitude result 349 (first 68 lines exact, cross and magnitude order right, then a copied product);
// other dot and magnitude forms 34-38 or 349-361; line struct 304, with named results 306-338 (slots as retail; XMM rotation stays, second distance products flip); offset after next's seed 138; no tmp3 309 (1612); tail C order 251 (1500); unused locals 34.
// First arm trials against the rotation: vertex locals in the arms + one dot + first=false last 534 (NONE moves to EDI), first=false ahead 347 (XMM roles and homes as retail, 1612), first=false in the arms 536; x, y loaded in the arms 292 (1618, BL,
// XMM roles, cross and in-place products as retail but the arms' products copy), vector-first or y-first 292, in-place parts 298, value helper 288; a vertex copy 298; mixed arms 161; forward local `!!` or `!= false` or ternary 393, byte 554, long 555; two-statement distance at the other sites 40.
// Next: retail's one dot with first=false last keeps forward in BL only in a form not found yet; x, y in the arms is the nearest (its product copy is the blocker).
// Lane 9 r88 (coordinator): kept at 244 is the 34 form with its two two-level exits written as `next_sector_index = NONE; break;`
// (a `goto` is a match-only form). build/held/lane9-r88-26c590-34.patch is the whole 34 form; its two `goto finish` exits are its only
// difference from the body below, so a byte match from it keeps them. The planar normal as `*(vector2f const *)&normal` is the code
// base's planar idiom (as two member stores 406). 0x26c380 is `inline` with (pathfinding, origin, ...): retail sets EDI ahead of EDX,
// and it still matches.
// Held form at 34: 1605 bytes as retail, first difference +0x94, aligned count with registers and stack homes masked 4. Left: the cross's row j operands and the magnitude's j/i order, XMM1-3 rotated
// (retail line offset, normal j, normal i; ours normal j, normal i, line offset), the line offset's and previous sector's stack slots, tail C's edge/distance store order.
// @retail 0x26c590
bool function_26c590(s_pathfinding_data const *pathfinding, point3f const *origin, long sector_index,
	long target_sector_index, vector3f const *direction, real distance, s_path_location const *location,
	s_path_trace_result *trace)
{
	s_sector_trace_result *result = (s_sector_trace_result *)trace;
	bool success = false;
	long current_sector_index = sector_index;
	short count = 0;
	s_pathfinding_edge *edge = NULL;

	result->blocked = false;
	result->sector_index = NONE;
	result->edge_index = (word)NONE;
	result->distance = distance;
	result->unknown20 = 3.4028235e38f;
	result->unknown1c = false;
	real last_distance = -0.1f;
	if (pathfinding && sector_index != target_sector_index && sector_index >= 0 && sector_index < pathfinding->node_count)
	{
		vector3f normal;

		cross3f(g_4687b0, direction, &normal);
		if (magnitude3d(&normal) > 1e-5)
		{
			vector2f line_normal = *(vector2f const *)&normal;
			real line_distance = dot_product2d(origin, &line_normal);
			long next_sector_index = sector_index;

			while (NONE != next_sector_index)
			{
				long previous_sector_index = current_sector_index;
				s_edge_iterator iterator;
				bool first = true;
				bool hit = false;
				real distance1;
				point3f const *point1;

				current_sector_index = next_sector_index;
				next_sector_index = NONE;
				if (!(pathfinding->nodes[current_sector_index].flags & 0x10) && !result->unknown1c &&
					(edge == 0 || (((s_sector_edge const *)edge)->flags & 0x200)))
				{
					result->unknown1c = true;
					result->unknown20 = last_distance;
				}
				if (current_sector_index == target_sector_index)
				{
					break;
				}
				edge_iterator_new(&iterator, pathfinding, current_sector_index, true);
				edge = function_1fa720(&iterator);
				while (edge)
				{
					bool forward = iterator.forward;
					point3f const *point0;
					real distance0;

					if (iterator.previous_surface_index != NONE)
					{
						long tmp3 = iterator.vertex;
						point1 = &pathfinding->vertices[tmp3];
						{
							long entry_vertex;

							if (forward)
								entry_vertex = edge->vertices[1];
							else
								entry_vertex = edge->vertices[0];
							point0 = &pathfinding->vertices[entry_vertex];
						}
						distance0 = dot_product2d(point0, &line_normal) - line_distance;
						distance1 = dot_product2d(point1, &line_normal) - line_distance;
					}
					else
					{
						if (first)
						{
							if (forward)
							{
								point0 = &pathfinding->vertices[edge->vertices[1]];
								distance0 = dot_product2d(point0, &line_normal);
							}
							else
							{
								point0 = &pathfinding->vertices[edge->vertices[0]];
								distance0 = dot_product2d(point0, &line_normal);
							}
							distance0 -= line_distance;
							first = false;
						}
						else
						{
							distance0 = distance1;
							point0 = point1;
						}
						point1 = &pathfinding->vertices[iterator.vertex];
						distance1 = dot_product2d(point1, &line_normal) - line_distance;
					}
					hit = function_26c380(pathfinding, origin, forward, current_sector_index, iterator.edge_index,
						direction, &next_sector_index, distance0, distance1, point0, point1, result);
					if (hit)
					{
						if (result->blocked || next_sector_index != previous_sector_index)
						{
							if (result->distance - last_distance < -0.1 && count > 0)
							{
								next_sector_index = NONE;
								break;
							}
							last_distance = result->distance;
							if (result->distance - distance > 0.0001)
							{
								if (target_sector_index != NONE && target_sector_index != result->sector_index)
								{
									if (next_sector_index == target_sector_index && result->distance - distance < 0.01)
									{
										result->sector_index = next_sector_index;
										result->blocked = false;
										result->edge_index = (word)NONE;
										result->distance = distance;
										function_x697631(origin, direction, distance, &result->point);
										success = false;
										goto done;
									}
									else
									{
										result->distance = 0.0f;
										result->blocked = true;
										result->edge_index = (word)NONE;
										function_x697631(origin, direction, distance, &result->point);
										success = true;
										goto done;
									}
								}
								else
								{
									result->blocked = false;
									result->distance = distance;
									result->edge_index = (word)NONE;
									function_x697631(origin, direction, distance, &result->point);
									success = false;
									goto done;
								}
							}
							if (result->blocked)
							{
								success = true;
								goto done;
							}
							if (location && (((s_sector_edge const *)edge)->flags & 0x80) &&
								function_270600(iterator.edge_index, location))
							{
								result->blocked = success = true;
								goto done;
							}
							if (count > 200)
							{
								next_sector_index = NONE;
								break;
							}
							++count;
							break;
						}
						hit = false;
					}
					edge = function_1fa720(&iterator);
				}
				if (!hit)
				{
					result->sector_index = current_sector_index;
					result->blocked = true;
					result->edge_index = (word)NONE;
					result->distance = distance;
					function_x697631(origin, direction, distance, &result->point);
					success = true;
					goto done;
				}
			}
		}
	}
	result->blocked = false;
	result->edge_index = (word)NONE;
	result->sector_index = current_sector_index;
	result->distance = distance;
	function_x697631(origin, direction, distance, &result->point);
done:
	return success;
}


// @retail 0x26d370
bool function_26d370(point3f const *point, vector3f const *direction, plane3f const *plane, real *distance)
{
	bool result = true;
	real denominator = dot3f(direction, &plane->n);
	if (fabs(denominator) > 0.01)
		*distance = -(plane_distance_to_point(plane, point) / denominator);
	else
		result = false;
	return result;
}

/* Keep the local second component read separate from the first component store. */
PRIVATE inline real trace_normalize_direction(vector3f *vector)
{
	real distance = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (!(fabs(distance) < 0.0001f))
	{
		real scale = 1.0f / distance;
		vector->i = scale * vector->i;
		vector->j = ((vector3f volatile *)vector)->j * scale;
		vector->k = vector->k * scale;
	}
	else
		distance = 0.0f;
	return distance;
}

struct s_type_c3b527;

// @retail 0x26c4e0
bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long sector_index,
	s_type_c3b527 const *end, long target_sector_index, long location, s_path_trace_result *trace)
{
	point3f const *origin = (point3f const *)start;
	point3f const *target = (point3f const *)end;
	vector3f direction;
	direction.i = target->x - origin->x;
	direction.j = target->y - origin->y;
	direction.k = 0.0f;
	real distance = trace_normalize_direction(&direction);
	return function_26c590(pathfinding, origin, sector_index, target_sector_index,
		&direction, distance, (s_path_location const *)location, trace);
}

// @retail 0x26cbe0
bool function_26cbe0(s_pathfinding_data const *pathfinding, point3f const *origin,
	point3f const *target, long sector_index, long target_sector_index,
	s_path_location const *location)
{
	vector3f direction;
	vector3d_from_points3d(origin, target, &direction);
	real distance = (real)sqrt(direction.i * direction.i + direction.j * direction.j + direction.k * direction.k);
	if (!(fabs(distance) < 0.0001f))
	{
		real scale = 1.0f / distance;
		direction.i *= scale;
		/* Preserve the component access order while scaling the local direction. */
		((vector3f volatile *)&direction)->j = ((vector3f volatile *)&direction)->j * scale;
		direction.k = ((vector3f volatile *)&direction)->k * scale;
	}
	else
		distance = 0.0f;
	if (distance > 0.0f)
	{
		s_path_trace_result trace;
		return function_26c590(pathfinding, origin, sector_index, target_sector_index,
			&direction, distance, location, &trace);
	}
	return false;
}

struct s_obstacle_list;
struct s_obstacle_hit
{
	real distance;
	short index;
	short group;
};

struct s_avoidance_trace
{
	real distance;
	long sector;
	long edge;
	short obstacle;
	short group;
};

bool obstacle_list_cast_ray(s_obstacle_list const *list, short ignore_index, point2f const *origin,
	point2f const *direction, real radius, real maximum_distance, bool ignore_flagged, s_obstacle_hit *hit);

// @retail 0x26ccb0
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
	short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
	real radius, real distance, bool first, bool stop_at_goal, bool ignore_flagged,
	s_path_location const *location, s_avoidance_trace *trace)
{
	bool hit = false;
	trace->distance = distance;
	trace->sector = NONE;
	trace->edge = 0xffff;
	trace->obstacle = NONE;
	trace->group = NONE;
	if (sector == NONE || sector == 0xffff)
		return true;
	real limit = stop_at_goal ? distance - radius : distance;
	point3f start = {origin->x, origin->y, 0.0f};
	vector3f forward = {direction->x, direction->y, 0.0f};
	s_sector_trace_result offset, side, center;
	if (!first)
	{
		if (!(pathfinding->nodes[sector].flags & 0x10))
			return true;
		point3f end;
		function_x697631(&start, &forward, distance, &end);
		if (function_26c590(pathfinding, &start, sector, target_sector, &forward, distance, location,
			(s_path_trace_result *)&center) &&
			(center.distance < limit || (target_sector != NONE && center.sector_index != target_sector)))
		{
			hit = true;
			trace->distance = center.distance;
			trace->edge = center.edge_index;
		}
		vector3f lateral = {0.0f - direction->y, direction->x, 0.0f};
		function_26c590(pathfinding, &start, sector, NONE, &lateral, radius, NULL, (s_path_trace_result *)&offset);
		if (function_26c590(pathfinding, &offset.point, offset.sector_index, NONE, &forward,
			trace->distance, location, (s_path_trace_result *)&side) &&
			side.distance < trace->distance && side.distance < limit &&
			(center.blocked || function_26cbe0(pathfinding, &side.point, &end, side.sector_index, target_sector, location)))
		{
			hit = true;
			trace->distance = side.distance;
			trace->edge = side.edge_index;
		}
		lateral.i *= -1.0f;
		lateral.j *= -1.0f;
		lateral.k *= -1.0f;
		function_26c590(pathfinding, &start, sector, NONE, &lateral, radius, NULL, (s_path_trace_result *)&offset);
		if (function_26c590(pathfinding, &offset.point, offset.sector_index, NONE, &forward,
			trace->distance, location, (s_path_trace_result *)&side) &&
			side.distance < trace->distance && side.distance < limit &&
			(center.blocked || function_26cbe0(pathfinding, &side.point, &end, side.sector_index, target_sector, location)))
		{
			hit = true;
			trace->distance = side.distance;
			trace->edge = side.edge_index;
		}
	}
	if (obstacles)
	{
		s_obstacle_hit obstacle;
		if (obstacle_list_cast_ray(obstacles, ignored_obstacle, origin, direction, radius, distance, ignore_flagged, &obstacle) &&
			obstacle.distance < trace->distance && obstacle.distance < limit)
		{
			hit = true;
			trace->distance = obstacle.distance;
			trace->edge = 0xffff;
			trace->obstacle = obstacle.index;
			trace->group = obstacle.group;
		}
	}
	function_26c590(pathfinding, &start, sector, target_sector, &forward, trace->distance, location,
		(s_path_trace_result *)&center);
	trace->sector = center.sector_index;
	return hit;
}

struct s_trace_structure_view
{
	byte unknown00[0xc4];
	long count;
	s_pathfinding_data *pathfinding;
};

// @retail 0x26d290
bool function_26d290(point3f const *origin, point3f const *target, long sector_index, long *output_sector)
{
	vector3f direction;
	direction.i = target->x - origin->x;
	direction.j = target->y - origin->y;
	direction.k = 0.0f;
	real distance = trace_normalize_direction(&direction);
	bool result;
	if (distance > 0.0f)
	{
		s_trace_structure_view *structure = (s_trace_structure_view *)g_4e0348;
		s_pathfinding_data *pathfinding = NULL;
		if (structure->count > 0)
			pathfinding = structure->pathfinding;
		s_path_trace_result trace;
		result = !function_26c590(pathfinding, origin, sector_index, NONE,
			&direction, distance, NULL, &trace);
		if (result)
			*output_sector = ((s_sector_trace_result *)&trace)->sector_index;
	}
	else
	{
		result = true;
		*output_sector = sector_index;
	}
	return result;
}

struct s_collision_result_1697c0;
struct s_path_collision
{
    long type;
    real fraction;
    point3f point;
    byte field_14[0x3c - 0x14];
    long structure_index;
    long object_index;
    long field_44;
    long node_index;
    long field_4c;
    long surface_index;
    long field_54;
};
struct s_path_node_point
{
    point3f point;
    short output_index;
    short field_e;
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
long function_1fa3a0(long structure_index, long surface_index, long object_index,
    long node_index, point3f const *point);
short function_210310(long object_index, long a);
bool function_210690(short output_index, point3f const *point, point3f *out);

// score 28 (was 28): retail computes start before direction, sets the NONE result before the call
// setup and keeps point on the stack. Round 61: with the parameters as (up, point, output, collision)
// (point then needs no `&point` read to stay on the stack), start first and result declared first it
// matches (fast), and so does 0x1c19b0, every matched caller kept: BLOCKED-BY 0x270400 (two call lines
// in contributor code). With this order: point read directly 35, plus start first 2. With this
// signature: start and result first 37; that through one `point3f const *` read of `&point` 37;
// point read directly 28 here but five matched callers lose their argument setup.
// Round 52 (fast only, not kept): an unused last parameter (the pathfinding data, which 0x26d0e0 receives as its
// unused third argument and 0x26d570 computes before its call) is dropped by LTCG: every caller's code unchanged,
// 28. Kept only with every call passing it: BLOCKED-BY 0x270400 (two call lines in contributor code); a
// defaulted parameter would avoid them but needs a ruling. The plain check hit a Wine hang; body restored.
// Round 70 (m3 lane 1, fast): point read directly with result and start first 30, with the output and hit views first 30,
// a named local at every read 30 (each loses five matched callers); its address read once into that local 37; its
// address taken only for the else arm's copy 37.
// @retail 0x26d100
long function_26d100(vector3f const *up, long *unknown, s_collision_result_1697c0 *collision, point3f const *point)
{
    point3f const *const *point_reference = &point;
    vector3f direction;
    direction.i = up->i * -4.0f;
    direction.j = up->j * -4.0f;
    direction.k = up->k * -4.0f;
    point3f start;
    start.x = up->i * 0.5f + (*point_reference)->x;
    start.y = up->j * 0.5f + (*point_reference)->y;
    start.z = up->k * 0.5f + (*point_reference)->z;
    s_path_collision *hit = (s_path_collision *)collision;
    s_path_node_point *output = (s_path_node_point *)unknown;
    long result = NONE;
    if (function_1697c0(0x84000d, &start, &direction, NONE, NONE, collision))
    {
        result = function_1fa3a0(hit->structure_index, hit->surface_index, hit->object_index,
            hit->node_index, &start);
        if (output)
        {
            if (hit->object_index == NONE)
            {
                output->point = hit->point;
                output->output_index = NONE;
            }
            else
            {
                short index = function_210310(hit->object_index, hit->node_index);
                if (index == NONE)
                {
                    output->point = hit->point;
                    output->output_index = index;
                }
                else if (function_210690(index, &hit->point, &output->point))
                    output->output_index = index;
                else
                {
                    output->point = hit->point;
                    output->output_index = NONE;
                }
            }
        }
    }
    else if (output)
    {
        output->point = **point_reference;
        output->output_index = NONE;
    }
    return result;
}

struct s_downward_collision_result
{
    s_path_collision collision;
    long field58;
};

// @retail 0x26d0e0
long __stdcall function_26d0e0(point3f const *point, s_type_c3b527 *output, long unused)
{
    s_downward_collision_result collision;
    // retail keeps the unused third parameter, so it returns with ret 4
    long const *unused_reference = &unused;
    (void)unused_reference;
    *(short *)((byte *)&collision + 0x24) = NONE;
    return function_26d100(g_4687b0, (long *)output, (s_collision_result_1697c0 *)&collision, point);
}
