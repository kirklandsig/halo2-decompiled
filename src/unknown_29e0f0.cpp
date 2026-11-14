#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
#include "slot_handler.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_29de20 { short field_0; short field_2; short field_4; };
struct s_29de21 { s_29de20 field_0[30]; short field_b4; };
short function_29de20(short arg_1, s_29de21 *arg_0, point3f const *arg_2, long arg_3);

struct s_29e0f0 { long field_0; point3f *field_4; };
struct s_29e0f1 { byte field_0[0x40]; long field_40; s_29e0f0 *field_44; };
struct s_29e0f2 { byte field_0[0x6c]; long field_6c; s_29e0f1 *field_70; };
struct s_29e0f3 { byte field_0[0xc4]; long field_c4; s_29e0f2 *field_c8; };
struct s_29e0f4 { short field_0; short field_2; };
struct s_29e0f5 { byte field_0[0x80]; long field_80; s_29e0f4 *field_84; };
struct s_29e0f6
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0xa];
    long field_30;
    s_29e0f5 *field_34;
};
struct s_29e0f7 { byte field_0[0x168]; long field_168; s_29e0f6 *field_16c; };
struct s_29e0f8
{
    short field_0;
    short field_2;
    long field_4;
    short field_8;
    byte field_a[2];
    s_type_c3b527 field_c;
};
struct s_29e0f9
{
    bool field_0;
    byte field_1[3];
    s_type_c3b527 field_4;
    long field_14;
    real field_18;
    point3f field_1c;
    short field_28;
    byte field_2a[2];
    bool field_2c;
    char field_2d;
    bool field_2e;
    byte field_2f;
    s_29e0f8 field_30[5];
    byte field_bc[8];
};

PRIVATE __forceinline real function_29e2ea(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_1->x - arg_0->x;
    real local_1 = arg_1->y - arg_0->y;
    real local_2 = arg_1->z - arg_0->z;
    /* retail adds (y*y + z*z) first, then x*x (0x29e316-0x29e32c) */
    return local_0 * local_0 + (local_1 * local_1 + local_2 * local_2);
}
PRIVATE __forceinline void function_29e64d(s_29e0f8 *arg_0, point3f const *arg_1)
{
    arg_0->field_0 = NONE;
    arg_0->field_8 = NONE;
    arg_0->field_c.point = *arg_1;
    arg_0->field_c.output_index = NONE;
    arg_0->field_4 = NONE;
    arg_0->field_2 = 0;
}

PRIVATE __forceinline real function_29e3b1(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_0->x - arg_1->x;
    real local_1 = arg_0->y - arg_1->y;
    real local_2 = arg_0->z - arg_1->z;
    return (real)sqrt(local_2 * local_2 + local_1 * local_1 + local_0 * local_0);
}
// score 382 (was 382; 382 before): earlier 391, 390; frame 0x64 and the node-pointer/group-table roles (EBX/EBP) agree; the selected defaults' homes,
// point bitmap, endpoint homes and x87 scheduling differ.
// MiniMax R (11-11): no lower score (382); retail's selected point index is compared as a dword where ours is compared as a word and
// retail's selected endpoint as a word where ours is compared as a dword, retail's group index is a sign-extended 32-bit value parked in the
// dead argument slot, and retail's output loop indexes &arg_4->field_30[count] and rebuilds the record address from the group base; taking
// the selected point as a long, the endpoint and the endpoint candidate as shorts, the group index as a long and the output indexed reaches
// 431 at 1605 bytes, because our locals sit eight bytes below retail's (the marker position at [esp+0x70] against retail's [esp+0x68]) and
// the extra spills fall inside the x87 sequences. A permuter run over this body (470 tries in 1212 s) reached
// 381 in the full image, one line better, but it needs a volatile pointer to the group table and split
// conditions, and a gain under eight is dropped, so the original body is kept.
// Earlier 391 (was 391; 390 before): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Earlier 391 (was 391): 390 before; frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// MiniMax C (11-05): no new form, no lower score; retail's three selected-index homes are at [esp+0x44], [esp+0x28] and [esp+0x24] where ours are at [esp+0x40], [esp+0xc] and [esp+0x20]; retail holds the node pointer in ebx and the group table in ebp where we hold them the other way round, and loading the group table before the node pointer scores 397.
// MiniMax E (11-10): no new form, no lower score (391); the frame size agrees (0x64), but retail holds the node pointer in ebx and the group table in ebp where this build holds them the other way round, and the three selected-index homes sit at other frame offsets.
// Earlier 391 (was 390): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Earlier 391 (was 391): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// frame 0x64 agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Earlier 391 (was 390): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// M3 lane 13 r75 (fast, on the round-64 recipe rebuilt on today's file: 155 at 1596 bytes, masked aligned 20; all match-only): the found
// flag stored first in the success block 152 (our compiler then stores it from AL too: retail's AL comes from local_29 loading into EDX,
// not EAX); local_29 stored first 161; the call compare as `1 == call` 155 (same code); the call nested under the distance test 155 (same
// code); the two pair distances summed in the other order 164. Retail's ECX-then-ESI setup at 0x29de20 is forced by the shuffle (ESI holds
// the table), so it is no evidence for a parameter reorder.
// Round 64 (fast; masked aligned count in brackets, base 390 [182]): NOT kept, scratch recipe reaching 155 [11] at retail's 1596 bytes with every stack
// slot right (aligned 428 -> 22): (1) group as `long`, tested on the field read and copied after: `if (bit_vector_test(visited_groups, entry.field_0))
// continue; long local_15 = entry.field_0;`, set through bit_vector_set: 398 [161] (read beside a local copy, match-only; without it the rest stays 435-461);
// (2) endpoint and start `short`; record index set before the pass loop and after each step; output through `&out[count]` with a separate cursor and the
// record pointer before the loop: 313 [67] (428 -> 87); (3) group bitmap by memset 225, found flag declared before the endpoint, `else result = false` on
// the sentinel test: 190 [45] at 1596 (the else is a redundant store unless retail's `mov byte [esp+0x13], 0` counts as evidence); (4) output pointer named per
// pass so it is computed before the point: 192 [39]; (5) x87 sums: scalars canonicalise (declaration order, written order, 4 identity locals: same code), but a
// vector local through vector3d_from_points3d keeps the written order, reversed in the code; retail's sites are written (i,j,k) at the pass start, (k,i,j) at
// the visited point, (j,k,i) twice at the pair: 159 [15] (needs per-site expressions); (6) squared distance helper as declared x, y, z with
// `x*x + (y*y + z*z)` 155 [11]. Left then: the third distance loads arg_0 into ESI, where retail keeps arg_3 in ESI from the range test to the call, arg_0
// in EDX, `mov ecx, esi; mov esi, edx` (one line, shifts all that follows); `mov [esp+0x80], al` for the flag; the step's EAX; `movsx edx, ax` ahead of the
// record base. Same code at (a): arg_3 or arg_0 copied to a local, the range test as a helper, repeated subtests, helper arguments reversed. Other forms:
// named total split 426 (1612 bytes). Each of (1)-(6) raises the score alone or is match-only, so nothing is kept without a match.
// Round 27: the instruction order follows retail closely, but the locals sit in different frame
// slots, the group pointer is in EBP where retail keeps EBX and retail parks the group index in the
// incoming s_reference slot at 0x80; no form found this round.
// MiniMax S (10-31): no new form, no lower score. The frame agrees (0x64), but retail parks its loop
// index and point index in memory (frame 0x2c and 0x80) where ours keeps them in registers, its three
// NONE defaults sit at frame 0x44/0x28/0x24 with the best distance at 0x38, against our 0x3c/0x40/0x1c
// and 0x34, and it holds the group table in ebx and the point table in ebp where we swap them.
// Earlier 390 (was 398): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Round 132 (m3 lane 5, fast): root-table test nested under the count test with the pointer declared there
// (no NULL default) 398; overall result declared first 399; best distance before the selected defaults 398;
// group bitmap declared before the record/group pointers 412; record pointer after the marker call 420.
// Earlier: frame 0x68/0x64, induction/endpoint homes and x87 scheduling differ.
// Earlier: table-pointer roles now agree; frame 0x68/0x64, outer-loop reload placement, inner-loop registers and x87 square order remain.
// Complete initial guard exits 458; CRT output point copy 416; distance second point by value 569;
// squared-distance second point by value 430; conditional table pointer initialization 416.
// Group-table field at each use 457; native long overall result 445; bounded output for 420; same-group pretested while 416; positive nearest-point scope 416.
// Entry secondary root 417; direct initial root fields 416; native output-point reference 416;
// native distance references 416; native first squared-distance point 433; unsigned-word fixed pass 416.
// Round 121: Success set before output fill 440; native first point of length helper 579; signed-byte direction step 416; metadata before point copy 439; signed-byte per-pass result 416. Original helper contracts and complete body restored.
// Earlier: entry bitmap storage 416; output-helper formal order 454; failed-selection common exit 416; ordinary distance inline 416; byte overall result 445.
// Round 341: One short direction counter, point payload and long record-index graphs 425/454/431/459; long direction counter 479/453/477/453. Original loops and values restored.
// Round 350: real outer/inner/both link-element receivers 416/416/416; with read-only
// link/group/both/point/all views 416 each. Frame 0x6c/0x64, table roles and bound spill remain.
// Round 146: short group index/pass counter/best indices (kept) 406; the three alone 409/416/512;
// distance index at top of the inner while 416; long search result 415, on the kept form 428;
// long point bounds 456; long last point 424; long nearest point 396 (word compares in retail, not kept).
// Round 86 (fast; aligned counts in brackets, 378 at the start): long group index 513 [313, retail's first-loop
// registers]; separate output cursor (retail compares the end index with the original start) 416 [357]; both 490
// [278]; bit_vector_test/bit_vector_set instead of the open-coded bits, identical; else-false result 459 [238];
// record index set before the pass loop and after each step 446 [189, frame 0x64]; memset group bitmap 437 [185];
// bit test on the field read ahead of the group-index local 197 [65], with the (point, record) output-helper order
// 199 [57], x*x + (y*y + z*z) squared sum 190 [48] and the found flag declared first 188 [46] (not kept: a field read
// beside a local copy needs a match; the same body without it 437). Left on 188: x87 square orders (retail rotates
// them per call; a vector-local distance with j, k, i terms gives d1/d2 175), arg_0 in EDX with arg_3 kept in ESI
// at the 0x29de20 call, AL reused for the found flag, the pass latch's lea, the output loop's entry jump.
// Round 148: the visited bits through the existing bit_vector_test/bit_vector_set (include/bit_vector.h)
// 416 at 1577 bytes, but difflib-aligned 378 -> 338: retail's separate test and set (word address and
// mask recomputed, load/or/store) and its unpadded outer loop; the same on the group bits only 416; with a
// long group index 513 (the set CSEs again); the length helper's sum written y, x, z 416 (406 without the
// bit helpers; same code).
// Round 267: complete parallel square terms in squared-distance/length/both helpers
// 406/406/406; selected-index role names 406; those names with squared-distance/length/
// both terms 406/406/406; complete graph with helper-axis names 406. Frame/index and
// square-input homes remain; original helpers, names, loops and body restored.
// Round 366: overwritten endpoint initializer removed 417; early group receiver 432; both 440; complete two-leg distance sum 406; with endpoint/receiver/both 417/432/440; all with scoped best-distance default 474. All restored.
// Round 232: direct-step pretested inner for, alone and with all seven complete
// link/directional/selected short-workspace identity subsets, 406 each. All restored.
// Round 234: complete-search link count/base/both 459/438/478; with promoted group
// domain 452/494/465; count/base with shared bits 462, plus promoted group 465. All restored.
// Round 368: test-produced final result 440; complementary pass admission 406; search-scoped best distance
// 457; result/admission 440; result/distance 478; admission/distance 457; all and named result 478/478.
// Original executable body restored.
// Round 370: shared point-array base with long group index, captured point count and shared
// bitmap operations 429/428/410/432/515/515/464/464; body, helpers and original includes restored.
// Round 214: Function-owned short outer/grouped-link/pass induction declarations, all seven
// nonempty subsets, 406 each; all three plus the record-search induction declaration 406.
// Every original initialization/pretest/increment and executable body restored; frame/index spill remains.
// Round 163: selected-index chain/per-group index chain/explicit native bitmap
// defaults, all seven subsets and complete graph with separate bitmap stores:
// 406 each at 1596 bytes. Frame 0x68/0x64 and bound/endpoint homes remain;
// original default initializers and every helper restored.
// Round 380: Native point bitmap unsigned/signed 456/456; unsigned group bitmap with array/unsigned/signed point bitmap 426/434/434; signed group bitmap 426/434/434. Original arrays restored.
// Round 239: coherent validator/reference-provider ordinary, inline and noinline
// declaration pairs (eight non-original combinations) 406 each. All restored.
// Earlier: the selected point index is a byte, as retail stores it. The frame 0x68 versus
// 0x64, the group and point bitmap homes and the remaining register rotations remain.
// Round 412: byte terminal/pass/both/output/terminal-output/pass-output/all/all plus accepted byte 446/436/439/398/447/436/439/439; native signed group on the current byte producer 496. All original types, loops and helper contracts restored.
// Round 215: consistent group/lower-endpoint/both field observations on the byte-last-point graph
// 462/452/473; with shared bitmap operations 488/429/476; real link receiver for group/all 429/475.
// Original snapshots, bitmap operations and body restored; frame 0x64 agrees, selected defaults, root
// roles and endpoint homes remain.
// Round 434: scan-scoped selected group/first/both/terminal/group-terminal/first-terminal/all
// constant defaults 438/438/440/438/440/440/444; all plus scan-scoped best distance 468. Frame
// stays 0x64; the last grows to 1609 bytes. Late group default recovers root registers but loses an entry store; restored.
// Round 205: Consistent link-array base with outer/inner/both actual cursors 464/433/447; record cursor 433;
// outer-record/inner-record/all cursors 500/455/436; all with shared bitmap operations 458.
// All restored; selected-default, endpoint and bitmap homes and root-table roles remain.
// Round 4 (lane 18, fast): the selected defaults, best distance or first default renamed to sort
// first/last 398 each (slots don't move: retail 0x44/0x28/0x24/0x38, ours 0x3c/0x40/0x1c/0x34); the
// group table read before the link table 414; the group bitmap cleared before the marker call 413;
// the three defaults as one chained assignment 398; the group index declared with its read 398; the
// marker position declared at function scope 398. Restored.
// Round 205: Consistent link-array base with outer/inner/both actual cursors 464/433/447; record cursor 433;
// outer-record/inner-record/all cursors 500/455/436; all with shared bitmap operations 458.
// All restored; selected-default, endpoint and bitmap homes and root-table roles remain.
// Round 245: complete squared/length metric deltas via the shared point difference or
// native whole-vector initializer, eight non-original pairs 402/398/418/398/418/398/418/398.
// Original scalar producers and entire body restored on the byte-endpoint/frame-0x64 graph.
// Round 464: late per-group record default/endpoints/both/nearest-distance default
// 403/448/430/398; record-distance/endpoints-distance/all 403/448/430; all with
// complete matching-record search 423. Original per-group lifetimes and search restored.
// Round 146 (lane 4, fast with 0x29e730): the group read through a multi-use `link` pointer at every use (no
// local_15) 429 (1601 bytes); round 86's bit test on the field read ahead of local_15 459 (aligned 209 masked,
// against 213); `long local_15` 496 (1559 bytes); the group bits through the existing bit_vector_test/
// bit_vector_set 424 (1561 bytes, masked aligned 195: retail's separate test and set, but the pass-scan compare
// stays 16-bit, `cmp word [..], dx`, where retail compares the movsx'd field with a 32-bit group in EBX); the
// point bits through them too 424 (same code); `long local_15` with the helpers 496 (the set shares the test's mask again). Retail spills the group (EBX) to the arg_2 home and later reuses
// that home for the pass's found flag (`mov byte ptr [esp + 0x80], 0`).
// Round 474: Native early reference/provider/both ordinal snapshots 398 each;
// complete link payload alone/with reference/provider/both 475 each; complete
// graph with promoted provider ordinal 513. Original body and helpers restored.
// Round 112 (lane 8, fast): group-table pointer declared first and assigned at its test 398; scenario
// pointer declared first 398; no structure-BSP local (global read at both uses) 398; the three defaults
// declared in reverse order 398; the group-table pointer as one ternary initialiser 398 (all same code).
// Round 494: complete point-input copies inside squared/length metrics, first/second/both
// in each alone, then both squared with first/both length: 439/437/473/558/555/787/677/892.
// Original input reads, metric helpers and entire body restored; selected homes and table roles remain.
// Round 221: native selected group/terminal/both 466/394/459; with native group input 513/470/471
// (native input control 496); complete graph with short byte-narrowed terminal producer 433. Native
// selected terminal plus actual output-record cursor and consistent point-table fields 390, kept:
// best-distance home agrees at frame 0x64. Separate short/long output-point induction 415/412; with one
// shared computed step 418/427. Original signed-short input scans, byte narrowing, metric helpers and
// validator tuples remain; selected-default, bitmap/endpoint, root-table and x87 homes still differ.
// DH round 264: Actual destination-record cursor with short/word/byte/long output ordinals 402/402/402/404;
// with consistent point-table fields 391/391/391/392. Indexed destination with those fields 412;
// point-first/terminal-first eligibility on the 391 graph 391/391. Seven-point lead below gate;
// original body restored; selected-default/root roles and metric/bitmap homes remain.
// Round 51 (lane 11, fast): round 86's t19 form rebuilt on this body 437 with the byte selected point,
// 188 (aligned 46) with it short again; on that graph the existing distance3d (0x3ea30, inlined) for the
// three point distances with b - a operands 185 (aligned 57), with a - b 216; the distance helper through
// a vector local with j,k,i / i,j,k / k,j,i sums 188 each (same code). Left on 188: per-call x87 square
// orders, arg_3 not kept in ESI across the distances to the 0x29de20 call, the found flag stored from AL,
// the record-index add, the output loop's entry jump. distance3d on the original 418/418.
// Round 494: complete point-input copies inside squared/length metrics, first/second/both
// in each alone, then both squared with first/both length: 439/437/473/558/555/787/677/892.
// Original input reads, metric helpers and entire body restored; selected homes and table roles remain.
// DH round 264: Actual destination-record cursor with short/word/byte/long output ordinals 402/402/402/404;
// with consistent point-table fields 391/391/391/392. Indexed destination with those fields 412;
// point-first/terminal-first eligibility on the 391 graph 391/391. Seven-point lead below gate;
// original body restored; selected-default/root roles and metric/bitmap homes remain.
// DL224: Group range-first/negative-first/split-range/split-bitmap eligibility 473/440/473/390;
// the four with shared group bits 434/434/434/424. Original group observation, domains, helpers
// and body restored; default/bitmap/endpoint homes, root roles and x87 scheduling remain.
// DL229: twice-used validator status feeds admission and pass acceptance: raw short/long/word/dword
// 428/426/428/426; normalized bool/byte/short/long 392/406/424/426. Exact-only flag producers
// restored; selected defaults, root roles and endpoint/bitmap/x87 homes remain.
// Round 306: Native anchor/candidate/destination payloads, seven subsets 432/408/474/424/460/464/509;
// complete library transfers 509. All regress and disturb frame or extent. Original pointer payloads,
// metrics, signed-short scans, selected output cursor and full body restored at 390;
// default/bitmap/endpoint homes and root-table roles remain.
// DH271: Signed-byte-narrowed anchor carried in short with bool/byte/short/long accepted state
// 457/457/448/456; long anchor with those states 453/453/433/442. All preserve original byte narrowing but
// regress and grow the frame/extent; original complete body restored. Selected-default, root-register and
// bitmap/endpoint homes remain.
// DH round 279: Shared per-iteration candidate-point address before/after stop guard, native group and
// scalar point-bitmap interactions [442, 445, 452, 470, 459, 474, 492, 509]. Original executable body
// restored.
// Round 77 (m3 lane 12, fast control 390): retail stores false into the result byte again on the
// no-selection exit and keeps the selected terminal and its candidate as shorts (`cmp word ptr [..], -1`,
// no byte narrowing; its byte at the arg_2 home is the pass's found flag). Result assigned from the
// selection test 430; `else` false after the output 400; short terminal and candidate 431; short terminal
// alone 391; short candidate alone 446 (register-masked aligned 170 against 183). Original body restored.
// DH281: Admitted-group/nearest-selection/candidate-validation lexical ownership, seven subsets then all
// with full-width signed group [390, 390, 390, 390, 390, 390, 390, 461]. Original executable body restored.
// Y522: Output-record point setter/native input value/returned output cursor, seven subsets
// 395/397/476/390/395/397/476; complete graph with ordinary output-helper inline 482.
// Original private output helper and body restored; selected-default, table and endpoint homes remain.
// DH291: Native selected group/point/record accumulator subsets on the retained output-cursor
// graph 462/444/390/466/462/409/468; all with full-width input group 440. Original short
// traversals, selected homes, byte narrowing, bitmaps, metrics and complete body restored.
// DH293: Joint selected-terminal/best-distance/point-bitmap four-byte lexical
// priorities, all six 390; two opposite priorities with nearest-distance identity
// 390/390. Original complete body restored; default/table/bitmap/endpoint homes remain.
// DH295: Consistent provider/membership/group-record descriptor field observations, subsets
// 1/2/4/3/5/6/7: 386/452/428/447/450/481/470; all with shared bits 523. The four-point
// provider-only lead is below the keep gate; original body/includes restored.
// Selected-default, root-table, bitmap/endpoint and x87 homes remain.
// AC488: native candidate range/type, best-score/validator and final-selected admission
// nesting subsets 1/2/4/3/5/6/7: 390/390/390/390/390/390/390; all with native long group 461.
// Original body restored; selected defaults, root/group roles, point bitmap and pass homes remain.
// M3 lane 13 r54 (fast): the structure table read before the root-table test 452; the group record pointer
// formed right after the group bit is set 438 (1576 bytes); a link-count local for both link loops 439 (1576
// bytes); the nearest-distance compare swapped 390 (same code); the path sum written last-to-first 390 (same code).
// DH312: Single native input reads through own addresses, five direct consumers then
// entry-owned point/reference/record pointers: eight forms 390 each. NOT kept:
// own-address and one-use aliases did not match; the record alias also demotes
// the caller. Original full body restored.
// M339: Actual output-array extent owns traversal and produces short/word/long/dword count, posttested
// then pretested: 412/412/411/411/430/430/429/429. Original output ordinal and complete body restored.
// Round 202 (lane 3, fast): retail reads the group with `movsx ebx` (32-bit compares) and recomputes the set's word
// address first, then the mask (bit_vector_set's shape). On this body: `long local_15` 461 (1560 bytes); long with
// bit_vector_test for the test only 461; long with bit_vector_set for the set only 470 (1551); short with
// bit_vector_set for the set only 424 (1567); the found flag declared once per group and cleared per pass 390 (same
// code). Restored.
// M3 lane 2 r47 (fast): the three NONE defaults renamed (selected group/point/end) 390, homes unchanged; retail keeps
// the endpoint copy as a dword at +0x4c (copied to the result without extension) and its +0x80 byte is the found flag
// in arg_2's home: endpoint `long` 436 (1579 bytes, frame 0x68), `short` 446 (1563), long with the flag declared
// first 436; byte endpoint with the flag declared first 390 (same code).
// M339: Actual output-array extent owns traversal and produces short/word/long/dword count, posttested
// then pretested: 412/412/411/411/430/430/429/429. Original output ordinal and complete body restored.
// Round 68 (m3 lane 10, fast; aligned difflib counts in brackets, base 390 [428]): the output loop indexed
// `&arg_4->field_30[count]` with a separate point cursor stepped by `local_2 > local_1` (retail's imul and
// unchanged-start compare) 394 [395] (local_1/local_2 homes move to 0x20/0x1c, retail 0x28/0x24); with the
// selected group record named before the loop 391 [380] (hoisted, but spilled to arg_3's home where retail keeps
// EDI). On that output form (O): memset group bitmap + long group + bit_vector_test/set 446 [360] (1570 bytes),
// with the point bits through them too 446 [360], memset + bit helpers 410 [341] (1572), memset + long 458 [355]
// (1559): the test's mask still CSEs into the set. All restored.
// Round 206 (m3 lane 3, fast; aligned counts from a difflib pass, base 287): the group bitmap cleared by
// memset 387 (aligned 276: retail's two zero registers, separate xor for the loop index); on it, the output
// loop indexing &arg_4->field_30[count] with a separate walking point stepped by `local_2 > local_1` (retail
// compares against the unchanged start, [esp+0x38]) 391 (aligned 268, that loop then as retail's but for the
// hoisted point-list base); with the point-list pointer formed before the loop 388 (267); with `long local_15`
// 459 (1556 bytes, aligned 259: the outer loop entry then falls into the body as retail, no jmp/stub), same
// with bit_vector_test/bit_vector_set 459 (same code: our test and set share the word address and mask,
// retail recomputes both); the pass-start point and endpoint `short` (retail compares the endpoint as a word)
// 432 (frame 0x68), the pass-start alone 424. Retail keeps the per-pass found flag in the arg_2 slot (0x80).
// Original body restored.
// F365: Point-owned squared/ordinary metrics and output-record initializer, masks 1/2/4/3/5/6/7, then ordinary inline:
// 390/390/390/390/390/390/390/503 fast. NOT kept: no qualifying byte match; complete original body and helpers restored.
// F368: Existing squared/length nominal double carriers with original real rounding and output-reference contract, subsets 1/2/4/3/5/6/7, then full graph with pointer output result: 390 each at 1596 bytes. NOT kept: no byte match; original helper contracts, casts and complete body restored.
// B412: Predicate-owned first-record search, short/long ordinal and cursor-ordinal,
// actual pointer-difference while/for, conditional short/long result: 390/425/412/425/434/434/434/410.
// Original scan/body restored; selected-default, root, bitmap/endpoint and x87 homes remain.
// DH342: Immutable existing squared-delta/length-delta/metric-result producers,
// seven subsets and all with the immutable candidate record index: eight at 390.
// Original declarations and body restored; default/root/bitmap/endpoint homes remain.
// DH348: Native immutable initial-root/membership-root/reference/provider/list/point-table/
// current-group/candidate-point handles: eight at 390. Original handle declarations,
// body and helpers restored; selected-default/root/bitmap/endpoint homes remain.
// DH352: Read-only initial-root/membership-root/membership-group descriptor pointees,
// seven subsets then the read-only current group: eight at 390. Original pointer
// declarations and body restored; selected-default/root/bitmap/endpoint homes remain.
// Round 134 (m3 lane 8, fast; al = aligned raw/masked/masked+stack, base 428/207/182). Rebuilt: memset group bitmap, bit_vector_test/set,
// the group read in the bit test and then into a long local, short selected point, else-false 425 (1604, frame 0x68, al 221/148/107); output
// loop with a cursor stepped against the unchanged start 428 (1605, al 205/150/91); the record index set before the pass loop and after each
// step 197 (1596, frame 0x64, first diff +0x13a, the group spilled to arg_2's home as retail, al 65/47/47); (point, record) output helper order
// 199 (al 57/41/41); scan sum plain or as k, j, i statements 190 (al 48/37/37, first 197 lines exact), y, x, z locals 194; the found flag
// declared before the pass's point copy 188 (al 46/35/35). NOT kept: the field read beside the local needs a match. Left on 188: the per-call
// x87 orders (retail z,y,x then y,x,z then x,z,y, ours z,y,x throughout; identity chains in the body or helper, a vector local, a named sum
// and length_sq3f 188, a from-points inline 185, do not move them), arg_3 kept in ESI with arg_0 in EDX to the 0x29de20 call, AL stored as
// the found flag (a named short status stored as a byte folds to 1), the latch's `add eax, ebp`, the selected record's load order, the output
// loop's entry jump. On that form: group local before the test 437 (1561), short 400, joined 437, link pointer 437; field at every use, direct
// or through a link pointer, 491/474 (1657/1641). The natural pieces on this body: step 425 (1608), plus memset 431, bit helpers 446,
// else-false 439, short selected point 435 (1564), memset+helpers+else-false 462, with the short point 412 (1599); all but the step 437.
// DH359: Whole-operation failed outcome bool/byte/short/long, cleared after output
// 408/408/432/431; at output entry 410/410/432/431. NOT kept: no byte match;
// original result, selected homes, roots, bitmaps, helpers and complete body restored.
// DH361: Step-equality/polarity stopping tests 405/396; loop-owned ordinal/selected/step-equality/
// step-polarity/complementary-ordinal/complementary-selected boundaries 390/389/405/396/390/389.
// NOT kept: one-point leads below the gate; original directional predicate and full body restored.
// DH363: Native indexed coordinates in squared/length metrics and actual output triplet loop,
// subsets 1/2/4/3/5/6/7: 390/390/393/390/393/393/393; full graph with indexed length delta 428.
// NOT kept: no match or native-home recovery; original metrics, copy and whole body restored.
// DH365: One append-capacity outcome owns the marker append and output flag,
// bool/byte/char/signed-char/short/word/int/long: 414/409/409/409/404/404/406/406. NOT kept: no exact
// result/join recovery; original body restored. Selected-default/root/bitmap homes remain.
// DH368: Standard coordinate-range squared/length delta and output-copy boundaries,
// seven subsets then the complete graph with native float root: 437/507/435/555/443/501/537/537.
// NOT kept: all regress and miss native homes/extent; original metrics, copies and whole body restored.
// Round 75 (m3 lane 10, fast with 0x29e730 and the three callees; control 390, difflib-aligned 287): retail keeps the
// group record in EBP and the loop bound in EDI for the whole group loop, ours puts the record in EDI (the mask
// temporary clobbers it, hence the reload stub ahead of the loop head) and the bound in EBX. Outer index `long` 458
// (1561 bytes, aligned 272); the link table read after the marker call 392; the group bitmap declared before the
// marker position 405; the record pointer formed after the marker call 417; the outer bound in a short local 422.
// M3 lane 11 r72 (fast, control 390, masked 181): the round-64 recipe rebuilt as a control, 155 at 1596 bytes (masked 11); left on it: arg_3 stays
// in ESI from the range test to the 0x29de20 call in retail (`mov ecx, esi; mov esi, edx`, arg_0 in EDX for the third distance) where ours reloads
// arg_3 into ECX and gives arg_0 ESI, the found flag stored from AL (retail loads local_29 into EDX, keeping the call's AX = 1), the step's EAX and
// the output loop's record-base order. On this body: squared distance `x*x + (y*y + z*z)` 391 (masked 177); found flag declared first in the pass
// 390 (same code); `short local_29` 446 (1563 bytes, frame changes at +0x2, masked 159); with `short local_2` too 431 (1577, masked 195); a group
// entry pointer for both the group and start fields 390 (same code).
// AC556: Selected-sentinel pair chains and six complete chains on the retained
// long-terminal/output-cursor graph: eight at 390, 1596 bytes. Earlier chain trials
// used the former short terminal. NOT kept: default homes and root roles unchanged;
// original initializers, helpers, traversals and full body restored.
// M403: Existing shared distance at pass-start/visited-anchor/paired-candidate,
// seven subsets then shared squared metric: 401/390/404/401/415/404/415/415. NOT kept:
// no gain; original helpers and full body restored, native selected/root/bitmap homes remain.
// DH378: Single-exit failure ownership for complete root/reference, reference bound, provider and compatibility
// guards, then root count/pointer/count-in-pointer/reference-in-root guards: 390 each in eight forms.
// NOT kept: all canonicalize at 1596 bytes; original full body, guards, sole return and helpers restored.
// DH380: Actual incoming origin/record/output pointer bindings made immutable, seven subsets,
// then all with the packed-reference binding: 390/390/390/390/390/390/390/390. NOT kept: no native
// selected/default/root-home recovery; original parameter bindings, helpers and full body restored.
// b2M (11-06) statement-by-statement walk against retail 0x29e0f0..0x29e729: every test, bound, field offset, stride, callee,
// argument and exit agrees with the source. One behaviour fix: the SSE squared distance (0x29e2f0-0x29e32c) adds
// (z*z + y*y) + x*x, so the helper is x*x + (y*y + z*z); the old y*y + (x*x + z*z) rounded the sum differently (390 -> 391).
// Float rounding points agree: fstp dword at 0x29e400/0x29e4a8 (pass-start and visited length), 0x29e51f (second pair length,
// reloaded), 0x29e54d (total); the first pair length stays unrounded on the x87 stack. The x87 square sums canonicalise
// whatever the written order (three per-site orders tried, same code), so the per-site x87 association stays 390-level.
// Width only, no behaviour: retail compares the nearest point as a word (0x29e60c, 0x29e680, 0x29e687): short local_2 392;
// local_29 as short/word/long 447/447/436 (1563/1579 bytes), unsigned char 391; indices stay below 32 (one-word visited bitmap).
// Round 183 (m3 lane 4, fast; aligned/register-masked difflib counts 281/135 at 391): no new form. The listing
// agrees with the walk above: the differences are register and slot assignment from the first group read on (retail
// sign-extends the group into EBX with `movsx`, compares it 32-bit and recomputes the bit mask for the set; ours
// keeps it in BP and reuses the mask), and the output loop, which retail indexes (`&arg_4->field_30[count]`,
// movsx/imul each pass and again for the marker) where ours walks a pointer. Each of these is listed above (long
// group index, bit_vector_test/set, indexed destination 412, round 64's recipe), so nothing was re-run.
// m3 lane 7 r146 (fast; aligned/masked = difflib counts with addresses, then registers and stack offsets, masked; control 391
// [420/178]): round 64's scratch recipe re-applied on this body (its squared-distance change is already here) 155 [22/11] at 1596
// bytes, 0x29e730 still matching. On it: the step `long` 155 and local_29 set before local_30 155 (same code); the output record
// pointer dropped (field read at the call) 98 at 1604 bytes [26/16]; the third distance through the scalar helper 176 [32/21]
// (diagnostic); success-block orders flag-first 152 [24/14], local_2-first 161 [33/13], local_2 second 153 [23/11], local_0
// first 155 [24/13]. On the 98 form: flag first 95 [28/19]; with it local_29 before local_30 95, the step `long` 95 (same
// code); orders flag/local_2/best/local_0/local_1 106, flag/local_0/local_1/best/local_2 98, flag/best/local_2/local_0/local_1
// 97 (all 1604 bytes, structurally worse than 155 [22/11]). Permuter on the recipe (600 s) best 92 with volatile table pointers
// (not kept, diagnostic only: each volatile alone 155). Left on the recipe: retail keeps arg_3 in ESI from the range test to
// the 0x29de20 call and loads arg_0 into EDX for the third distance (`mov ecx, esi; mov esi, edx`), ours loads arg_0 into ESI
// and reloads arg_3. The flag store from AL, the step's EAX and the jmp over the output loop's padding follow from that.
// m3 lane 8 r146 (fast; aligned/masked base 420/178; 0x29e730 MATCH throughout): both bitmaps or the group bitmap alone through the
// existing bit_vector_test/bit_vector_set 425/425 (1567 bytes); the group bitmap cleared by memset 388 (aligned 382, masked 176; three
// points, not kept), both bitmaps by memset 388 (same code); the output as `&arg_4->field_30[local_37]` with no cursor 396 (masked 164);
// the found flag declared first 393; memset + indexed output 393 (aligned 356, masked 162), with the flag first 395.
// m3 lane 11 r88 (fast, 382 plain-confirmed): the scenario global read at its two uses in place of the local_7 view pointer
// 391 -> 382 (aligned 420 -> 403, masked 203 -> 199: the node pointer takes EBX and the table EBP as retail; kept). The
// group record read at each use in place of local_11 443 (1579 bytes). On 382: the structure global read at its two uses
// 382 (same code); local_9's fields at each use 387; the three defaults named best_group/best_point/best_end_point 382
// (same code, homes unchanged); `short local_2` 383 (masked 269); the group bitmap cleared by memset 379 (aligned 365,
// masked 197; three lower, not kept). On the memset 379: `short local_2` 380; `short local_29` 411 (1563 bytes); the group
// bits through bit_vector_test/set 412 (1567); the output written by index (no cursor) 384 (aligned 347, masked 186);
// that with `short local_2` 402 (1577); the found flag declared first 379 (same code).
// m3 lane 2 r70 (fast with 0x29e730 and the three callees; control 391, masked 178): the round-64 recipe as saved by m3 lane 7
// r146 re-measures 155 (1596 bytes, aligned 22, masked 11; 0x29e730 still matches). On it, for the one named cause (retail keeps
// arg_3 in ESI from the range test to the 0x29de20 call and loads arg_0 into EDX for the third distance): a table local for arg_3
// declared before the pair loop 155, declared at the range test 155 (both copy-propagate to the same code). Ours loads arg_0 into
// ESI before the index scaling (the call's ESI argument) and reloads arg_3. Also on the recipe: a named group count read once
// for the search loop and the range test 426 (1612 bytes; on this body 447 at 1576), the pair point's pointer formed before the
// range test 160 (arg_3 then reloaded from +0x88), the two pair distances named 155 (same code). No untried natural form is left.
// m3 lane 11 r90 (fast with 0x29e730, control 382, aligned 403, masked 199): `if (local_19 == NONE) continue;` as an if-block
// around the rest of the group body 382 (same code); the group-skip `continue` as an if-block 382 (same code); the link array named
// once for its four reads 415; re-measured on the 382 base, round 68's indexed output with the selected record named before the loop
// 388. Diagnostic: the search result written through a pointer (to see whether an escaping result stops the pass latch's strength
// reduction; retail recomputes the record index with `add eax, ebp`, ours steps it in a second register) 382, same latch. No untried
// natural form left beyond these: the remaining levers (retail's 32-bit group with a separate test and set, short terminal and
// candidate) are listed above and change the length or need the field read beside a local copy. A scan of include/ for helpers this
// body writes out found none unlisted (bit_vector, dot3f/length_sq3f, vector3d_from_points3d and distance3d are all above).
// M3 lane 10 r97 (fast with 0x29e730 MATCH; base 382): nothing kept. Descriptive names for every local (best_group, best_point,
// best_end_point, found, ... count) 382 (same code: the three defaults stay at +0x40/+0xc/+0x20, retail +0x44/+0x28/+0x24, so
// names do not order these homes); the improvement flag declared before the pass loop 382; the pass's anchor pointer declared
// before the anchor index 382; the selected group record named before the output loop (direct output kept) 382 (all same
// code); the output step as an if/else increment/decrement 389.
// M3 lane 5 r177 (fast with 0x29e730 and 0x29de20 matched each time; base 382): the pair loop counter `long` 448 (1561 B);
// the run loop counter `long` 1260 (2242 B); the endpoint search counter `long` 423 (1576 B); the group bitmap as `= {0}`
// 382 (same code); the marker flag stored as `arg_4->field_2c = local_37 < 4;` before the append 404 (1574 B).
// Re-measured: the marker position declared at function scope 382 (same code; listed 398 on an older body).
// P433: Fixed group/lower-endpoint/candidate-record bindings, seven subsets,
// then all with fixed validator mode: 382 each, 1596/1596 bytes. NOT kept:
// native selected/default and pass homes unchanged; original full body restored.
// T96: Float-root overload across all four length expansions, complete squared input,
// delta aggregate, existing from-points delta, uniform square parts, named return,
// ordinary inline and shared squared-length composition: 382/382/382/407/382/382/382/382.
// NOT kept: no selected/default/bitmap-home recovery. Original private metrics,
// helper contracts, includes and complete body restored.
// T99: Full-width selected point with unchanged-start direction and separate output
// walking cursor; pointer/indexed destinations, short/long walk and count domains:
// 424/422/434/436/419/426/425/454. NOT kept: no exact output ownership
// or default-home recovery; all original traversals and selected values restored.
// P448: Original-site group/point/improvement/anchor/accepted-stop/visited-point/record/route-score decisions:
// 382/382/382/382/382/382/382/382. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// M3 lane 9 r118: no trial run. First difference +0x11 is still the three selected defaults' homes (+0x40/+0xc/+0x20 for
// retail's +0x44/+0x28/+0x24), which lane 10 r97 showed names do not order. The note records twice that no untried natural
// form is left (the 32-bit group test-and-set, short terminal and candidate change the length); I found none outside it.
// P450: Original-site root-count/root-reference/reference-bound/record-compatibility
// decisions in bool/long: 382/382/382/382/382/382/382/382. NOT kept: no byte equality or selected-default
// home recovery; original complete body, metrics, bitmaps and traversals restored.
// P453: nearest scalar at scan scope, route scalar at pass scope,
// candidate pointer at pass scope, anchor pointer/distance at group
// scope, record ordinal at pass scope, earlier uninitialized group
// receiver and output cursor declarations: eight 382s. NOT kept:
// default/bitmap/pass homes remain; original body retained.
// P474: Native standard group/point fixed-bitsets with test/set or indexed
// proxy contracts, each alone and all four paired forms.
// Selected-default, bitmap and pass homes remain.
// Scores 424/458/404/546/416/511/461/570. NOT kept: ties/regressions;
// original executable body and helper contracts restored.
// P485: Actual accepted point/record ordinal owns pass history, short/word/long/dword:
// 419/419/417/417 and 448/448/444/444. NOT kept: no byte match; frame or extent
// regresses. Original Boolean state, selected/default homes, bitmaps and whole body restored.
// @retail 0x29e0f0
bool __stdcall function_29e0f0(point3f const *arg_0, long arg_1, s_reference arg_2,
    s_29de21 *arg_3, s_29e0f9 *arg_4)
{
    short local_0 = NONE;
    short local_1 = NONE;
    long local_2 = NONE;
    real best_distance = 3.402823466e38f;
    bool local_4 = false;
    s_29e0f3 *local_5 = (s_29e0f3 *)g_4e0348;
    s_29e0f2 *local_6 = NULL;
    if (local_5->field_c4 > 0) local_6 = local_5->field_c8;
    if (local_6 && local_6->field_6c > 0 && !(arg_2.unknown2 & 0x8000) && arg_2.unknown2 >= 0)
    {
        if (arg_2.unknown2 < ((s_29e0f7 *)g_4e0350)->field_168)
        {
            s_262b40_result *local_8 = function_262b40(arg_2);
            if (local_8)
            {
                s_29e0f6 *local_9 = &((s_29e0f7 *)g_4e0350)->field_16c[(word)arg_2.unknown2];
                if (local_8->unknown10 >= 0 && local_8->unknown10 < local_9->field_30 && local_9->field_24 == g_4686c4)
                {
                    s_29e0f5 *local_10 = &local_9->field_34[local_8->unknown10];
                    s_29e0f1 *local_11 = local_6->field_70;
                    point3f marker_position;
                    function_210850((s_type_c3b527 const *)local_8, &marker_position);
                    dword visited_groups[2] = {0, 0};
                    for (short local_14 = 0; local_14 < local_10->field_80; ++local_14)
                    {
                        short local_15;
                        local_15 = local_10->field_84[local_14].field_0;
                        if ((visited_groups[local_15 >> 5] & (1 << (local_15 & 31))) || local_15 < 0 || local_15 >= local_11->field_40) continue;
                        visited_groups[local_15 >> 5] |= 1 << (local_15 & 31);
                        dword visited_points[1] = {0};
                        short local_17 = local_10->field_84[local_14].field_2;
                        short local_18 = local_17;
                        short local_19 = NONE;
                        short local_20 = NONE;
                        real local_21 = 3.402823466e38f;
                        s_29e0f0 *local_22 = &local_11->field_44[local_15];
                        for (short local_23 = local_14; local_23 < local_10->field_80 && local_10->field_84[local_23].field_0 == local_15; ++local_23)
                        {
                            local_18 = local_10->field_84[local_23].field_2;
                            visited_points[local_18 >> 5] |= 1 << (local_18 & 31);
                            if (local_18 >= 0 && local_18 < local_22->field_0)
                            {
                                real local_24 = function_29e2ea(&marker_position, &local_22->field_4[local_18]);
                                if (local_21 > local_24) { local_21 = local_24; local_19 = local_18; }
                            }
                        }
                        if (local_19 == NONE) continue;
                        for (short local_25 = 0; local_25 < arg_3->field_b4; ++local_25)
                            if (arg_3->field_0[local_25].field_0 == local_15) { local_20 = local_25; break; }
                        short local_26 = 1;
                        short local_28 = local_19;
                        for (short local_27 = 0; local_27 < 2; ++local_27)
                        {
                            char local_29 = local_19;
                            point3f *local_30 = &local_22->field_4[local_19];
                            real local_31 = function_29e3b1(&marker_position, local_30);
                            bool local_32 = false;
                            while (local_28 >= 0 && local_28 < local_22->field_0)
                            {
                                if (local_32 && ((local_27 == 0 && local_28 > local_18) || (local_27 == 1 && local_28 < local_17))) break;
                                if (visited_points[local_28 >> 5] & (1 << (local_28 & 31)))
                                {
                                    local_30 = &local_22->field_4[local_28];
                                    local_29 = local_28;
                                    local_31 = function_29e3b1(&marker_position, local_30);
                                }
                                short local_33 = local_20 + local_28;
                                if (local_33 >= 0 && local_33 < arg_3->field_b4 && arg_3->field_0[local_33].field_4 != 2)
                                {
                                    point3f *local_34 = &local_22->field_4[local_28];
                                    real local_35 = function_29e3b1(local_34, arg_0) + function_29e3b1(local_30, local_34) + local_31;
                                    if (best_distance > local_35 && function_29de20(local_33, arg_3, arg_0, arg_1) == 1)
                                    {
                                        best_distance = local_35;
                                        local_0 = local_15;
                                        local_1 = local_28;
                                        local_2 = local_29;
                                        local_32 = true;
                                    }
                                }
                                local_28 += local_26;
                            }
                            local_26 = -local_26;
                            local_28 = local_19 + local_26;
                        }
                    }
                    if (local_0 != NONE && local_1 != NONE && local_2 != NONE)
                    {
                        s_29e0f8 *output = &arg_4->field_30[0];
                        short local_37 = 0;
                        do
                        {
                            function_29e64d(output, &local_11->field_44[local_0].field_4[local_1]);
                            ++output;
                            ++local_37;
                            if (local_1 == local_2) break;
                            local_1 += local_2 > local_1 ? 1 : -1;
                        } while (local_37 < 4);
                        if (local_37 < 4)
                        {
                            function_29e64d(output, &marker_position);
                            ++local_37;
                            arg_4->field_2c = true;
                        }
                        else arg_4->field_2c = false;
                        arg_4->field_2d = (byte)local_37;
                        local_4 = true;
                    }
                }
            }
        }
    }
    return local_4;
}

long function_baf80(long arg_0);
long function_1e4990(long arg_0);
bool function_2702a0(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4,
    point3f const *arg_5, vector3f const *arg_6, point3f *arg_7,
    long *arg_8, vector3f *arg_9, long *arg_10);
struct s_29db90;
void function_29db90(s_29db90 *list, short block_index, short range_index);
bool function_29dd50(point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3);

struct s_29e730
{
    short field_0;
    byte field_2[0xa];
    s_reference field_c;
    bool field_10;
    byte field_11[0x18];
    bool field_29;
    byte field_2a[2];
    vector3f field_2c;
};
struct s_29e731 { byte field_0[0x18]; long field_18; byte field_1c[0x38]; long field_54; };

// @retail 0x29e730
bool __stdcall function_29e730(s_29e730 *arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 const *arg_3, s_reference arg_4, s_29de21 *arg_5,
    long arg_6, s_29e0f9 *arg_7)
{
    bool local_0 = false;
    s_29e731 *local_1 = (s_29e731 *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888);
    long local_2;
    if (local_1->field_18 != NONE)
        local_2 = function_baf80(local_1->field_18);
    else
        local_2 = NONE;
    memset(arg_7, 0, sizeof(*arg_7));
    s_type_c3b527 local_3;
    s_type_c3b527 local_6;
    if (arg_0->field_0 == 6)
    {
        vector3f local_4;
        vector3f local_8;
        point3f local_9;
        {
            point3f local_5;
            if (!function_262a90(arg_0->field_c, &local_5, &local_4)) goto local_20;
            if (!function_2702a0(arg_1, 0xf000545, 0x5000049, 0x6000542, 0x400000c,
                &local_5, &local_4, &local_9, NULL, &local_8, NULL)) goto local_20;
        }
        s_262b40_result *local_10 = function_262b40(arg_0->field_c);
        vector3f local_11;
        if (!function_210770(((s_type_c3b527 *)local_10)->output_index, &local_8, &local_11)) goto local_20;
        if (!function_210690(((s_type_c3b527 *)local_10)->output_index, &local_9, &local_3.point)) goto local_20;
        local_3.output_index = ((s_type_c3b527 *)local_10)->output_index;
        point3f local_24;
        local_24.x = local_9.x + local_4.i * 1.5f;
        local_24.y = local_9.y + local_4.j * 1.5f;
        local_24.z = local_9.z + local_4.k * 1.5f;
        if (arg_0->field_10)
        {
            arg_0->field_29 = true;
            arg_0->field_2c = local_8;
        }
        else if (function_210690(((s_type_c3b527 *)local_10)->output_index, &local_24, &local_6.point))
        {
            local_6.output_index = local_3.output_index;
            local_0 = true;
        }
        goto local_21;
    }

    if (arg_0->field_0 == 4)
    {
        long local_12 = function_1e4990(local_1->field_54);
        if (!local_12 || (*(byte *)local_12 & 2)) goto local_20;
        s_262b40_result *local_10 = function_262b40(arg_0->field_c);
        if (!local_10 || !((byte)local_10->flags & 0x40)) goto local_20;
        vector3f local_4;
        point3f local_5;
        vector3f local_8;
        point3f local_9;
        if (!function_262af0(arg_0->field_c, &local_5, &local_8)) goto local_20;
        if (!function_2702a0(arg_1, 0x6000542, 0x400069d, 0x6000542, 0x400000c,
            &local_5, &local_8, &local_9, NULL, &local_4, NULL)) goto local_20;
        if (!function_210690(((s_type_c3b527 *)local_10)->output_index, &local_9, &local_3.point)) goto local_20;
        local_3.output_index = ((s_type_c3b527 *)local_10)->output_index;
        goto local_21;
    }
local_20:
    local_3 = *arg_3;
    local_0 = false;
local_21:
    point3f local_7;
    if (local_3.output_index != NONE)
    {
        if (!function_2104b0(local_3.output_index, &local_3.point, &local_7))
            local_7 = local_3.point;
    }
    else
        local_7 = local_3.point;
    arg_7->field_1c = *arg_2;
    arg_7->field_4 = local_3;
    arg_7->field_28 = NONE;
    arg_7->field_14 = NONE;
    arg_7->field_2e = false;
    arg_7->field_18 = 0.0f;
    bool local_13;
    if (function_29dd50(arg_2, &local_7, local_2, arg_6))
    {
        arg_7->field_30[0].field_0 = NONE;
        arg_7->field_30[0].field_c = local_3;
        arg_7->field_30[0].field_4 = NONE;
        arg_7->field_30[0].field_8 = NONE;
        arg_7->field_2d = 1;
        arg_7->field_2c = true;
        local_13 = true;
    }
    else
    {
        if (REFERENCE_EQUAL(arg_4, g_470fa0) || (arg_4.unknown2 & 0x8000))
        {
            local_13 = false;
        }
        else
        {
            s_29de21 local_15;
            if (!arg_5)
            {
                s_262b40_result *range_data = function_262b40(arg_4);
                arg_5 = &local_15;
                local_15.field_b4 = 0;
                function_29db90((s_29db90 *)&local_15, arg_4.unknown2, range_data->unknown10);
            }
            local_13 = function_29e0f0(arg_2, local_2, arg_4, arg_5, arg_7);
            if (local_13 && arg_7->field_2c && arg_7->field_2d > 0)
            {
                arg_7->field_30[arg_7->field_2d - 1].field_0 = NONE;
                arg_7->field_30[arg_7->field_2d - 1].field_c = local_3;
                arg_7->field_30[arg_7->field_2d - 1].field_4 = NONE;
                arg_7->field_30[arg_7->field_2d - 1].field_8 = NONE;
            }
        }
    }
    if (local_13 && arg_7->field_2c && local_0)
    {
        short local_18 = arg_7->field_2d - 1;
        if (arg_7->field_2d < 4)
        {
            arg_7->field_30[arg_7->field_2d] = arg_7->field_30[arg_7->field_2d - 1];
            ++arg_7->field_2d;
        }
        else arg_7->field_2c = false;
        s_29e0f8 *local_19 = &arg_7->field_30[local_18];
        local_19->field_0 = NONE;
        local_19->field_8 = NONE;
        local_19->field_c = local_6;
        local_19->field_4 = NONE;
        local_19->field_2 = 0;
    }
    arg_7->field_0 = local_13;
    if (!local_13)
    {
        short *local_23 = (short *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888 + 0x5b4);
        if (++*local_23 > 32) *local_23 = 0;
    }
    return arg_7->field_0;
}
