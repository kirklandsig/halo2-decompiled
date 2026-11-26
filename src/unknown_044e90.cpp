// @flags /O2 /Gr /arch:SSE
#include "unknown_044e90.h"
#include "normalize3d.h"

plane3f *function_1428b0(transform4x3f const *matrix, plane3f const *plane, plane3f *out);

struct s_visibility_bound_record
{
    point3f center;
    real radius;
    byte transform_index;
    byte padding[3];
};
// score 430 (was 430): r239, r275 and lane 11 r145 unchanged. The geometry lookup goes through the existing visible_entry_geometry (r107, at the end); the entry address and tag table still have different register homes, moving the entry spill and the plane/query schedule.
// MiniMax E (11-20): no new form, no lower score (430): the frame 0x74a4 agrees with retail's, but the head still differs - retail forms the entry address with a lea into EBX from the scaled index and keeps the tag table in ESI from there to the bottom arm, while this build reads the tag through a displacement from the scaled index and hoists the table into ECX at the top, with the entry spilled at [esp+0x48] against retail's [esp+0x30]; making the header's geometry lookup take the caller's entry pointer leaves 0x458d0 and 0x45a80 matched but gives 449 here, so the lookup stays index-based.
// MiniMax E (11-16): no new form tried this round, no lower score (443): the frame still agrees with retail's, and the first
// difference is still at +0x10, where retail forms the entry address with a lea into ebx from a sign-extended index before the
// register saves and this build forms it with a shifted copy added to the table after them; the part centre's body is inlined
// from 0x142570, whose file this lane may not edit.
// MiniMax H (11-13): no new form, no lower score (443).
// MiniMax I (11-11): no new form, no lower score (443): the first difference is at +0x10, where retail forms the entry address as a sign-extended index scaled by the entry size and added to the entry table (`movsx eax, word [ebp + 8]`, `shl eax, 5`, `lea ebx, [eax + 0x4ba138]`) and we form it as a shifted copy added to the same table (`shl edi, 5`, `add edi, ...`); retail then keeps the tag table in esi and spills the entry at [esp+0x30], we keep the entry in a register.
// MiniMax I (11-11): reading the entry's flags in each arm instead of through the `flags` local (retail loads
// them per arm) scores 465 and is not kept: the tag then takes eax and the flags edx, retail's tag is in eax
// and the flags in ecx, and the body's length stays 1760 against retail's 1742.

// M3 lane 15 r124 (H2M_FAST with 0x458d0 and 0x45a80, both MATCH each run; control 430): the entry index read through
// its address for the entry and the lookup (the spelling of 0x458d0/0x45a80) 430 (same code); a redundant `if (0 < view
// count)` guard around the view loop 499 (1792 B); a redundant `if (0 < part count)` guard around the part loop 454 (1765
// B); the part bit written at both uses instead of the local 456 (1767 B); the entry pointer declared `*const` 430 (same
// code). Lane 11 r133's lead re-measured 396 at 1742 B (first difference +0x57). Body unchanged.
// MiniMax S (11-17): no new form, no lower score (443): the frame 0x74a4 agrees with retail's and the first
// difference is still at +0x10, where retail forms the entry address with a lea into ebx from an index in eax
// before the register saves and we form it with an add into edi after them; the part centre's body is inlined
// from 0x142570 in another file.
// MiniMax E (11-11, run 80): no new form, no lower score (443): a named local for the entry index gives the same
// code (443) and reading the entry's flags at each of its three uses instead of once at the top scores 465; the
// frame 0x74a4 agrees with retail's and the difference sits in the head, where retail forms the entry address with
// a lea into ebx from an index in eax before the register saves and we form it with an add into edi after them.
// MiniMax B (11-12): no new form, no lower score (443): the frame agrees with retail's, and the difference sits in
// the head, where retail forms the entry address with a lea into EBX from an index in EAX before the register
// saves and we form it with an add into EDI after them; the entry's flags are read ahead of the tag test here
// and in each arm there.
// MiniMax I (11-14): no new form, no lower score (443): the first difference is still at +0x10, where retail forms the entry address as a sign-extended index scaled by the entry size and added to the entry table (`movsx eax, word [ebp + 8]`, `shl eax, 5`, `lea ebx, [eax + 0x4ba138]`) and we form it as a shifted copy added to the same table (`shl edi, 5`, `add edi, ...`); retail keeps the tag table in esi and spills the entry at [esp+0x30], we keep the entry in a register, and the part centre's body is inlined from 0x142570 in another file.
// Earlier at 443 (was 443): earlier 476; plain, 1760 B: the releases through the existing release_visibility_list (moved from
// MiniMax D (11-14): no new form, no lower score (443): the frame still agrees with retail's, and the difference is
// still in the head, where retail forms the entry address with a lea into ebx from a sign-extended index before the
// register saves and this build forms it with a shifted copy added to the table after them; the part centre's body
// is inlined from 0x142570, whose file this lane may not edit.
// MiniMax R (11-15): no new form, no lower score (443): retail still forms the entry address as a sign-extended index scaled by the entry size and added to the table, and the part centre is still 0x142570's body inlined from another file.
// MiniMax R (11-16): no new form, no lower score (443): the zero constant in EBX (retail keeps the entry there), the
// list-start stores, the two-step frustum source pointer, the plane cursor at .k and the j/k register naming, the tag table
// reloaded in the bottom arm and the bound centre copied with integer moves (retail loads it into xmm0-2 and scales in place,
// which is 0x142570's body in a file this lane may not edit) all remain; the permuter (29 tries in 306 s, annealing) reached
// 399 in the full image against its 429 baseline with a variant built on a volatile entry pointer, a volatile visible
// pointer, a constant-only local and a one-use pointer local, so it was reverted, and the same variant without the two
// locals measures 441 on the reduced image.
// MiniMax S (11-16): no new form, no lower score (443): the body is still 1760 bytes against retail's 1742 and the first difference is
// still at +0x10, where retail forms the entry address as a scaled index added to the entry table (`movsx eax, word [ebp + 8]`, `shl eax, 5`,
// `lea ebx, [eax + table]`) and this build forms it as a shifted copy added to the same table (`shl edi, 5`, `add edi, ...`); retail then
// loads the tag table into esi and spills the entry at [esp+0x30] while this build keeps the entry in a register, the flags are read
// ahead of the tag test where retail reads them in each arm, and the parts calls' pushes are hoisted where retail sets up each arm;
// the permuter (annealing, 420 s) found no variant below 443 - the best it wrote ties the baseline and only respells the constants and
// adds a volatile entry pointer, so it was reverted.
// Earlier 443 (was 443): earlier 476; plain, 1760 B: the releases through the existing release_visibility_list (moved from
// earlier 443 was 476 (plain, 1760 B); the releases through the existing release_visibility_list (moved from
// Earlier 443 (was 443; 476 before; plain, 1760 B): the releases through the existing release_visibility_list (moved from
// unknown_044720.cpp to unknown_044e90.h; retail's `test; js`), counts and capacities as chained assignments, and the tag
// table in a local read at entry (retail's ESI). Left: the entry in EDI (retail EBX via `lea`), the flags read ahead of
// the tag test (retail in each arm), the parts calls' pushes hoisted (retail sets up each arm), register rotations.
// Round 147 (m3 lane 8, fast, agreeing with the full build at 443): retail's part centre is 0x142570's body inlined (sums
// up*z + left*y + forward*x + position, ours forward first), so BLOCKED-BY 0x142570 (src/unknown_141590.cpp is /Ob1 and
// was on this round's avoid list). The 0x142570 call for the centre 645 (1544 B, not inlined); the centre rows in
// 0x142570's order, written out (diagnostic) 440; 0x1428b0 is already inlined here: a per-file `__forceinline`
// redeclaration 443, `inline` on its definition 443 (same code), `__forceinline` there (diagnostic) 443 with 0x17e670
// 429 -> 1131 (retail calls it there).
// Earlier (476, was 406; fast, 1700 B, aligned ratio 0.675 -> 0.727): the plane transform is a call to function_1428b0, retail's inlined
// MiniMax I (11-10): entry flags read in each branch and the tag table read once at the top, 475 each; a per-file inline threshold changes nothing, and retail keeps the scaled tag index in EAX (`movsx eax,[ebp+8]; shl eax,5; lea ebx,[eax+0x4ba138]`) where our build computes it in EBX with `add ebx,imm`, so the remaining head difference is the inline of function_1428b0, which lives in another file.
// Earlier 476 (was 406; fast, 1700 B, aligned ratio 0.675 -> 0.727): the plane transform is a call to function_1428b0, retail's inlined
// Earlier score line: 476 (was 406; fast, 1700 B, aligned ratio 0.675 -> 0.727): the plane transform is a call to function_1428b0, retail's inlined
// body, which also stores 0 into the distance for a zero scale (behaviour fix, b2L walk 11-06). Entry/table and stack homes remain.
// Earlier: frame/list strides and entry/table/plane homes differed.
// Earlier: Frame 0x7494 versus 0x74a4; list strides, entry/table and plane homes differ.
// Storage headers are at 0xb0/0x10bc/0x20c8/0x40d4 versus retail 0xb0/0x10c0/0x20d0/0x40e0.
// Input plane homes are 0x2c/0x30/0x34/0x38 versus 0x34/0x38/0x3c/0x40; normal registers,
// inverse-scale reuse, distance division and capacity-release scheduling also differ.
// (Scale-zero distance handling is now retail's, through function_1428b0.)
// plain plane for 2649; native view ranges 438; plane copy blocks 438; center point setter 438; views read
// before entry 442; entry bitfield 441
// Round 310: long bound index 438; explicit plane query 438; visibility-word pointer 438;
// capacity snapshot 438; guarded native input records 445 (plain). Entry/table and plane/list homes remain.
// Plane ordinal word 438; view ordinal word 438; frustum ordinal word 437; byte parts predicate 438; early containment byte
// 436 (plain).
// Entry-data cursor 438; selection flags 434; query count 438; inverse per frustum 525;
// captured frustum/view bounds 481/483; entry data plus inverse 525; complete matrix value 527.
// Round 166: query data result/conditional/negative-first 438/463/455; shared normal products
// 436; query result with products 440; const query 434; query result/products with const query
// 438/436. All product regroupings restored.
// Destination cursor 444; both plane cursors 484; destination/both with full publication
// 444/444; destination/both with outer plane 444/484; destination/both with normal declaration 444/484.
// Native input plane subobjects 468; separate shared-scaler normal output 476;
// multi-use distance scalar 453; input plus normal/distance 438/468;
// normal plus distance 471; all three 438; dword plane ordinal 434 (fast). All restored.
// DL131: Entry/tag-table registers and plane/list homes remain; existing geometry resolver
// before/after entry 443 each; native geometry record 434; resolver with native record in either
// entry order 443; resolver with shared normal scaling 443; native geometry with shared normal
// scaling 434; all three 443. Original source and resolver declaration restored; existing resolver
// body unchanged.
// Signed 32-bit flags 434; named section/both indices 455/455; structure-first/instance-first/
// combined selection 453/434/453; native normal/distance publication 441; source-plane cursor 445.
// Round 171 reduced control 434: direct entry 472; output planes 459; live transform pointer 434;
// entry/output planes 449; entry/live transform 472; planes/live transform 459; all three boundaries 449;
// entry/live selection flags 575.
// Round 317: Native rotation result 472; native center initializer 434; both 472; member alignment 435; aligned rotation/center/both 473/435/473; aligned rotation with shared normal scale 473. Header fully restored.
// Round 142: native tag/table/both observations 434/444/444; on the latter graph,
// workspace after selection/branch scope, signed flags, query count and entry-data receiver
// all 444; uniform selection-flag reads with tag-table/tag/tag-table-data 447/438/447.
// Round 211: Second/third/fourth capacity snapshots, each pair, all three and all four
// capacities at their sign/size consumers 434 each. Original capacity observations restored.
// Round 175: Native position/three rotation rows/both observations 475/556/586;
// with shared scale observation 477/556/586; independent scalar normals with
// all rows/position plus all rows 515/549. All original input ownership restored.
// Native storage-template alignment eight/sixteen 435/435; with scalar normal inputs
// 447/447; with whole-plane library publication 435/435; both 447/447. Header/body restored.
// Round 190: input-end/output-index 436; output-end/input-index 445; both cursors
// with input/output end do 486/451; input/output end for 473/451; input/output
// end while 473/451. Original fixed-six ordinal loop and all plane observations restored.
// Round 150: Native first/second, third/fourth and transform-array handles, seven subsets 434 each; all descriptor lifetimes beginning at first use 434. All handles restored.
// Round 146 (retail reads the flags inside each branch and the table into ESI before the tag test): entry flags
// at each use 438; table read once at the top 444; both 447 (the table lands in EDX); flags local per branch 441;
// entry declared before the workspace 434.
// Native stride-0x108 frustum source with raw/native count/native ranges/both 434 each;
// the same four forms with native six-plane array reference 434 each. Original source
// ownership, real record declarations and fixed-six loop restored.
// Round 240: Complete whole-query decision bool/byte/short/long 437/437/443/436;
// equivalent ordered alternatives 437/437/443/436. Entry/table and list/plane
// homes remain; all original selection observations and body restored.
// Round 162: Native selection fields alone/table/aligned/aligned-table 438/447/439/448; the same
// four with the existing normal scaler 438/447/439/448. Entry/table and plane/list homes remain;
// original external fields, storage alignment and all arithmetic restored.
// Round 273: native storage alignment eight/sixteen, crossed with original/frustum/view
// plane-normal declarations, eight forms 417/1742 each with frame 0x74a4. Four-point gain
// below gate. On that layout scale/plane-name/both/table/scale-table/plane-table/all/
// all-readonly-query 421/417/421/455/468/455/468/468. Header and entire body restored.
// Round 38: Independent list objects, list/transform lifetimes branch/branch, branch/entry,
// entry/branch and entry/entry, each with default/eight-byte alignment:
// 418/418/419/419/419/419/419/419 fast, all 1742 bytes. Branch/branch recovers frame 0x74a4;
// three-point gain rejected. Original combined workspace and complete body restored.
// Round 218: Live view source/direct ranges/both/both with native short view 481/474/453/450;
// native eight-byte storage alignment on those graphs 481/473/453/449. List/plane and entry/table
// homes remain; original header, observations and complete body restored.
// Round 435: Native eight-byte storage alignment plus chained counts and paired capacities,
// field orders pointer/count/capacity, pointer/capacity/count, count/pointer/capacity,
// count/capacity/pointer, capacity/pointer/count, capacity/count/pointer:
// 418/418/418/417/417/417; initialization by real capacity pairs, counts/capacities
// or capacities/counts 418/418. Four-point gain rejected; original header/body restored.
// Round 443: Native eight-byte storage plus ordinary-inline transform/containment/query
// declarations, seven subsets and all plus both publication boundaries, 417 each at 1742 bytes
// with frame 0x74a4. Four-point lead rejected; original header/body restored; entry/table and
// plane/list homes remain.
// Round 445: Real storage alignment eight with natural/eight plane-array alignment,
// each with aggregate/explicit query fields, 429 each; storage sixteen on the same four
// graphs 417 each. Four-point lead below keep gate; complete header and body restored.
// Round 97 (lane 7): retail stores 0 into the plane distance when the transform scale is 0 (a value
// difference: its zero path shares the distance store with the division path); `else p.d = 0.0f;` alone
// 444, on the 16-aligned storage template (417) 441. On that: the normal computed in both transform arms into
// a local vector and stored once at the join with `0.0f - p.d` (retail's join stores) 462; with
// `transform->scale != 0.0f` 450; with the normal locals defined after the distance block (the transform block
// is then retail's but for register rotation) 475 at 1700 bytes; with the release tests as
// `!(capacity & 0x80000000)` (retail's `test/js` and capacity reloads) 448 at 1744 bytes, aligned differences
// 294 -> 235; parts arms swapped 448 (the two calls' identical pushes stay hoisted, retail sets up each call in
// its arm). Left on that graph: the entry pointer in EDI (retail EBX, pushes interleaved), the tag table loaded
// in each branch (retail once into ESI before the tag test) and register rotations.
// Round 456: Persistent global entry reference; flags/table input graphs on native/default storage alignment:
// 421/457/433/454/417/455/432/452. Original executable body and supporting declarations restored.
// Round 467: Full-width input ordinals entry/section/both, then both with live
// selection fields, original storage 438/421/438/466; native eight-byte storage
// 435/417/435/465. Four-point aligned lead rejected; original header and body restored.
// Round 471: Native workspace list construction on eight-byte storage, original/reverse
// object order and six field-group orders 573 each (1620 bytes). Constructor and native
// alignment removed; original branch-local descriptor publication and full header restored.
// Round 474: Unsigned capacity/sign-mask on native four/eight storage 456/454;
// external-storage bit view 456/454; native allocation-count view 456/454;
// aligned whole-plane publication with bit/count or mask 480/480. Header and
// body restored; cleanup tests/preloads recover but entry roles and length regress.
// Round 437: Native sixteen-byte storage with the per-part containment output
// 406 reduced; with const query 406; live flags 442/442; branch-owned query data
// 448/447, with const query 448/447. Ordinary judgment unavailable; original
// storage, observations and complete body restored. Frame/list and pointer homes remain.
// Round 230: Native bound record and sixteen-byte storage with point/vector copy/component
// construction/direct shared scale 417/417/450/424; per-part containment output on those
// graphs 406/406/449/421. Native point form kept at 406 after full validation; all call values
// and float grouping retained. Storage has its own type identity.
// Round 233: fixed-six indexed input with plane/normal scopes loop/branch, frustum/branch,
// view/branch, loop/frustum, loop/view, frustum/frustum, view/view and view/frustum
// 415 each at 1742 bytes. Original cursor, all observations and complete body restored.
// Round 437: Native sixteen-byte storage with the per-part containment output
// 406 reduced; with const query 406; live flags 442/442; branch-owned query data
// 448/447, with const query 448/447. Ordinary judgment unavailable; original
// storage, observations and complete body restored. Frame/list and pointer homes remain.
// Round 476: Distance-before-normal graph, original/eight-byte storage, scalar/shared normal scale and direct/whole-plane output: 463/461/441/442/457/455/457/458.
// Scalar forms shrink to 1716; shared scale recovers 1742 but regresses. Original zero-scale behavior, whole body and header restored; entry/table and plane/list homes remain.
// Round 481: Complete scalar-normal initialization schedules i/j/k, j/i/k, j/k/i on
// native-four storage 421/420/420; all five non-original schedules on eight-byte storage
// 417/416/416/417/417. Best five-point form rejected; original body and header restored.
// Round 483: native-eight storage with complete descriptor publication orders
// second/first/third/fourth 416; second/first/fourth/third 417; second/third/first/fourth
// and second/third/fourth/first 418/418; third/second/first/fourth and
// third/first/second/fourth 417/417; fourth/third/second/first and
// first/third/second/fourth 418/418. Five-point lead below gate; body/header restored.
// Round 485: Native-eight storage with second/first descriptor publication and
// j/i/k or j/k/i normal creation, original/reversed final pair 415/416/415/416;
// coherent tag table 454/455/454/455. On the first graph, short frustum/early
// containment/both/short view/view with containment/both ordinals/all three
// 425/404/427/427/416/421/410. Eight-byte 404 loses three caller matches in the
// full check and is rejected. Sixteen-byte storage retains fast 404 and recovers
// all three callers, but its closing full check hangs. Complete body and storage
// restored; original frame/list strides and entry/table/plane homes remain.
// Round 238: copied-plane normal reused in place, with all shared rotation-dot/position-dot/
// inverse-scale subsets 459/459/478/478/459/459/478/478. Dot regroupings exact-only;
// original complete plane/normal producers restored; frame agrees, entry/table and plane homes remain.
// Round 230: Native bound record and sixteen-byte storage with point/vector copy/component
// construction/direct shared scale 417/417/450/424; per-part containment output on those
// graphs 406/406/449/421. Native point form kept at 406 after full validation; all call values
// and float grouping retained. Storage has its own type identity.
// Round 233: fixed-six indexed input with plane/normal scopes loop/branch, frustum/branch,
// view/branch, loop/frustum, loop/view, frustum/frustum, view/view and view/frustum
// 415 each at 1742 bytes. Original cursor, all observations and complete body restored.
// Round 302: frame remains 0x74a4; input-plane/query homes are eight bytes low and entry register
// lifetimes differ.
// Complete point helper with local/direct input, output at use/part entry and ordinary/inline declaration 626/648;
// all eight regroup the float expressions and miss, so restored.
// Round 240: Successive complete rotation-row/center-row/position-product statements, seven
// subsets and all with coherent plane/query home names: 406/403/403/406/406/403/403/403.
// Original operands, grouping, zero-scale handling and complete executable body restored.
// On the successive center graph, uninitialized output storage before the bound copy/at part
// entry: 403/403. Below-margin center gain and both lifetimes restored.
// Round 267: actual destination-plane transform, indexed/pointer/reference with
// assignment/library copies 485/485/476/476/476/476; indexed/pointer with existing
// normal scaler 472/460. Original input-plane and output publication restored;
// entry/table and plane/query homes remain at the 0x74a4 frame.
// Round 27 (m3 lane 18, fast; register-masked aligned 108 for the kept form): the plane transformed in place with one common store (retail's transform block
// register for register) 476 (1700) [84]; all 24 combinations of that, the scale test written transform->scale != 0, the index read through a reference (no effect), flags at each
// use and sign-mask capacity tests: scale test + flags 420 (1744) [94], scale test alone 427, flags alone 442, in place + flags + sign mask 445 (1760) [71], other combinations
// 455-487. Plus: scale test in place 476; the query-data arms as if/else 484; bool false arm first 480 (1778) [71], with a two-step source pointer 480; table local at the top 447 (1744) [77].
// Retail also reads the four capacities into registers once and re-reads them after each free, and pushes the sign-extended index at the visible-list calls.
// Round 248: Complete independent scalar normal results joined after both transform arms,
// loop/frustum/view/query declaration scopes with normal-first/distance-first publication: 459 each.
// Original input plane, scalar lifetimes, publication and zero-scale behavior restored.
// Round 311: Complete fallback bound value, const value and library copy 434/434/434;
// with native fallback transform values 467/467/467; value/const bounds with library transform copies
// 467/467. All actual record observations restored; entry/table and plane/query homes remain.
// Round 515: Boolean instance selector with coherent live flags/table input/branch-owned
// query-data subsets 419/444/442/447/453/443/462/448. Original selectors, observations,
// zero-scale behavior and complete body restored; entry/table and plane/query homes remain.
// Round 517: Output-array/query ownership: array at entry 406; query uninitialized at entry 413;
// both uninitialized at entry 413; both per view/frustum 407/407; entry array with view/frustum
// query 407/407; both initialized at entry 422. Original declaration scopes and all values restored;
// entry/table and plane/query homes remain.
// Round 471: Uninitialized containment output at entry/fallback/part-loop/part/record/transform/point-input/query,
// 433 each. Original false initialization and complete body restored.
// Round 137 (aligned count in brackets, baseline 406 [357]; retail's scalars sit below the plane and query, ours
// keep the views pointer and inner counter above them): flags read in each arm with a (bool) bit-12 test 444 [389]
// at 1744 bytes; query data as if/else arms 446 [361]; both 459 [407] at 1760; loop counters at branch or function
// scope 406; the plane copy at branch scope, after the query, at or before the top of the function 406 each; planes
// and query at function top, either order 413 [389]. Body unchanged.
// Round 525: Complete input-plane memcpy/memmove with native/library normal and center transfers,
// eight graphs 406/406/444/444/456/458/445/446. All original transfers and full body restored; entry/table and plane/query homes remain.
// Round 527: Uninitialized reciprocal declaration at function/query/view/frustum scope,
// each with the original/fallback-entry center-output scope: 406 each. Every original
// reciprocal producer, center scope and full body restored; entry/table and plane/query homes remain.
// Round 545: Actual visibility-buffer capture before fallback tag/object/count/nodes
// setup and function-entry uninitialized pointer with early/latest pre-loop producers:
// 442/430/430/442/441/441/442/441. Original observation and body restored; entry/table and plane/query homes remain.
// Round 63 (m3 lane 10, fast): retail stores each plane once after the transform/copy arms (i, j, k from xmm3/xmm5/xmm4,
// then the negated distance), keeps the views pointer in its slot ([esp+0x2c]) and forms the source pointer in two
// steps. A named state pointer for the views and active reads 411; the sibling 0x45560's chained count/capacity pairs
// (no header change) 407; the plane transformed in place into its copy with `planes[j] = p` 440 (1742 bytes, the copy
// stays in memory); the same with field-by-field stores 457 (1716 bytes, retail's transform block, views kept in ESI
// with the source folded into one lea); the six-plane do/while as a plain for 2754 (fully unrolled, 3604 bytes).
// Round 548: Real four-descriptor release table, long/dword/short/word/byte/signed-char
// counter and pointer </!= walks: 522/522/520/520/520/520/511/511. NOT kept: changed release/frame
// shape misses; original four releases, field reads, native workspace and full body restored.
// Round 142 (fast; aligned count in brackets, control 406 [357]): flags read in each arm 442 at 1744 [395], with a (bool) bit test
// 444 [389]; the (bool) bit test alone 419 [357]; a view count local 453 at 1748 [351]; a frustum count local 443 at 1748 [366]; both 474
// at 1764 [373]; the loop index before the transform 406 [357].
// Round 550: Actual fallback visibility-membership result, bool/byte/short/long,
// nested unseen admission then already-visible continue: 406/406/406/406/406/406/406/406.
// NOT kept: one-use predicates do not match; original direct bit test, containment
// initialization and complete body restored. Entry/table and plane/query homes remain.
// Y553: Independent fallback-center input scalars 448; part-entry storage 448;
// branch-owned inputs 463; shared scale with update/branch inputs 462/462; existing
// point setter on update/branch/all producers 448/463/462. All controls survive.
// NOT kept: all forms regress and shrink away from 1742 bytes; original body restored.
// M345: All four release-ownership results in bool/char/signed-char/byte/short/word/int/long,
// immediate per release 406/406/406/406/406/406/406/406; together at cleanup entry
// 507/507/507/507/511/511/528/528. NOT kept: new ownership carriers miss.
// Original direct sign tests, capacity reloads, cleanup order and complete body restored.
// B394: Inverse-transform scale reference/pointer, whole transform, translation and all-row references;
// then transform/scale, translation/scale and complete subobject identities: 406/406/406/406/436/406/406/406.
// NOT kept: no exact match; original complete executable source restored.
// Round 82 (m3 lane 9, fast): retail tests bit 12 as `mov ecx, [ebx]; shr ecx, 0xc; test cl, 1` and keeps the table
// in ESI from entry through the bottom arm (no reload). Flags read at each use 442 (1744 bytes); the bit-12 test through
// TEST_FIELD_BIT 419; both 444 (retail's shr/test and tag in EAX, but no table preload); with the table read once into a
// local 447; the table local and TEST_FIELD_BIT with the flags local 442.
// AC514: Existing complete inverse-plane provider on copied/original inputs and
// local/destination outputs, zero-scale preservation before/after the provider:
// 815/835/816/817/518/499/479/467. NOT kept: complete-provider/grouping forms do not match; every
// original plane, zero-scale output, declaration and complete body restored.
// Entry/table and plane/query homes remain.
// Y565: Actual outer/inner/both while traversals 406/406/406; live count-table cursor
// with original/outer/inner/both while graphs 433/433/433/433; both real table cursors
// 443. Original body restored; entry/table and plane/query homes remain.
// M347: Shared nonunit-scale decision for both transform phases, direct bool/byte/short/long then
// test-produced bool/byte/short/long: 422/422/443/445/427/427/447/450.
// NOT kept: shared/test-produced decision does not reproduce the native repeated scale tests; original complete body restored.
// Y568: Actual fallback node-input/composed-output/part-record cursors, seven subsets,
// then all with a part while loop: 432/432/436/432/436/436/436/436. All regress;
// original indexed buffers, observations and complete body restored. Frame 0x74a4,
// entry/table roles and eight-byte plane/query displacement remain.
// Y570: Native entry/range/frustum displacement producers, seven subsets and all with
// selection-owned flags, 406 each. Original indexed addresses and body restored;
// entry/table roles and plane/query homes remain.
// Y572: Simultaneous cleanup capacity values, each field, natural pairs, all,
// then immutable all: 406/450/448/439/445/450/446/446. NOT kept: native
// repeated capacity observations/entry and plane homes not recovered; body restored.
// Remaining three capacities before the first release 446; fourth-only 406 is
// a prior-form control, not one of eight new forms. Original full body restored.
// Y574: Actual fallback bit consumers, direct/shared test/shared set/both, each with nested
// unseen/visible continue admission: 403/403/403/403/403/403/403/403. NOT kept: three-point lead
// below keep gate; repeated computed masks miss equality and native retained bit ownership;
// entry/table and plane/query homes remain. Original complete body and helpers restored.
// Y576: Descriptor-count six-plane do traversal, long/short/word/byte ordinals
// 425/438/438/457; ordinary while 465/463/463/464. NOT kept: all regress;
// native entry/table and plane/query homes remain. Original fixed-six traversal and body restored.
// Y578: Actual mutable visibility-word reference at part/admission, direct/shared test/set/both
// consumers relative to that word: 406/432/441/449/406/436/441/449. NOT kept: native
// element ownership does not match; helper forms grow. Original bit mask, observations and body restored.
// Y582: Actual fallback sphere-query return in bool/byte/word/long, success publication
// or failed-query continue: 406 each. NOT kept: one-use results miss equality; original
// call, direct test, input observations and full body restored; entry/table/plane homes remain.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 406 [357]): part loop: named word-mask pointer 406; containment flag
// set in the arm 406 [356]; both 406; `inline` on 0x142a60 407, on 0x165010 406, on 0x45a80 and 0x458d0 406. Plane transformed in
// place with one store at the join and retail's zero-scale store 476 (1700) [278]; flags at each use with a (bool) bit test 474
// (1712) [320]; sign-mask capacity tests 467 (1760) [310]; query arms 438 (1760) [282]; `!parts` arm first 481 (1777) [283];
// chained capacities 481; two-step frustum pointer 481. Index parameter long at 0x458d0 and 0x45a80 (retail's callers push
// movsx eax, word [ebp+8]) 519 (1808) [274]; with chains and frustum 520, register-masked aligned 110.
// Round 129 (m3 lane 7, fast, 393 confirmed plain; aligned/masked = difflib counts, control 274/98; the score is positional, so forms were chosen on those first). NOT kept, 393 (1762 B, 208/65; restored, the gain needs the `visible` move, a field read moved ahead of calls): retail's plane block (zero scale
// stores 0 through the tail shared with the division, normals copied then scaled, one store at the join with `0.0f - d`; this also fixes the zero-scale value difference earlier notes left open), flags read at each use and the (bool) bit test
// (retail's `shr ecx, 0xc; test cl, 1`), the table local read before the tag test (retail's ESI, two uses), a structure local in the else arm (retail's read ahead of the test), a state local for the views and active reads, sign-mask capacity
// tests, chained counts/capacities in retail's store order, and `visible` read before the node loop (retail reads it there; the matrix calls write only their output). Removing any from 393: visible 451, state 454, structure form 449, capacity tests 487,
// chained lists 451, bit test 456, plane block 498; retail's arm order, explicit query fields, a word pointer and direct field forms change nothing here 393 (restored).
// Match path, none of it keepable alone: 0x458d0/0x45a80's index as `long` (retail's calls push `movsx eax, word [ebp + 8]`; both stay matched with plain `(short)index`, no address-of; call arms then come out exactly as retail with the pushes
// un-hoisted; 0x45560 182 -> 183, its body untouched), and the part components as scalars in place with a named scale in place of vector3f_scale: 377 (1763 B, 153/44). The existing geometry helper (short overload moved to a header) gives retail's
// else arm at retail's 1742 B, 425 (152/39), entry-pointer overload 448 (124/35; 0x458d0 falls to 138 in that image). Retail re-reads entry->tag at the bottom where ours reuses EAX from the first read (the copy `mov ecx, eax`); a short read there
// (a steering cast, diagnostic) frees EAX: 428 (127/33), with the state local 417 (112/30). Left then: count test `xor; cmp` for `test` and `mov; test` on active (zero register live from the list counts), query-data arms (written as if/else the bool
// test threads away, 427, 117/37; one shared address-taken bool for parts and containment 427, a parts helper 417/427, `do {} while (false)` around it 417/427), lea entry, the plane cursor base at .k (ours .j; store orders move it but never to .k).
// On the kept 393: negated distance named before the stores 393, list pointers second/first/third/fourth 394, outputs declared i/k/j 393, normals scaled in place 393, visible after the object index 393, scaled copies declared k/i/j 406.
// With `visible` left after the node loop, 64 combinations of state local, structure/section with and without the (bool) bit, chained lists, capacity tests and table local: best 411 (1760 B), then 435-440, none under 406; all restored. Behaviour difference still open:
// (fixed in the b2L walk: see the note above @retail.)
// Y585: Initialized containment false at unseen/bound-index/bound/transform/local-point/
// scaled-point/first-center/completed-center boundaries: 406/406/406/406/406/417/417/417.
// NOT kept: no eligible gain; original producer and full body restored; entry/table and plane/query homes remain.
// Y587: Fallback-center component for traversal long/dword/short/word/byte/
// signed-char, then long/short while: 406/406/502/502/502/502/406/502.
// NOT kept: narrow traversal stays rolled and shortens away from retail; original full body restored.
// Y589: Actual composed-node/frustum-plane/fallback-center input last-use scopes,
// seven subsets then all with immutable query, eight at 406. NOT kept: no exact
// entry/table or plane/query-home recovery; original complete body restored.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 406 [357]): part loop: named word-mask pointer 406; containment flag
// set in the arm 406 [356]; both 406; `inline` on 0x142a60 407, on 0x165010 406, on 0x45a80 and 0x458d0 406. Plane transformed in
// place with one store at the join and retail's zero-scale store 476 (1700) [278]; flags at each use with a (bool) bit test 474
// (1712) [320]; sign-mask capacity tests 467 (1760) [310]; query arms 438 (1760) [282]; `!parts` arm first 481 (1777) [283];
// chained capacities 481; two-step frustum pointer 481. Index parameter long at 0x458d0 and 0x45a80 (retail's callers push
// movsx eax, word [ebp+8]) 519 (1808) [274]; with chains and frustum 520, register-masked aligned 110.
// M3 lane 10 r73 (fast; aligned/masked in brackets, control 406 [357/254]): retail's `lea ebx, [eax + table]` reads as a repeated sum, so the tag read
// through the array expression 467 (1768) [351/251], the flags through it 448 (1742) [346/242], both 467 [351/251] (the read folds instead); the entry as
// `table + index`, through a long index and through a const reference to the index 406 each (same code); the plane copy as two statements 406 (same code).
// T61: Ordered parts-mode result, preset-false and exhaustive outcomes in bool/byte/short/long:
// 411/411/450/450 and 408/408/413/446. NOT kept: test-produced flags miss equality;
// original conjunction and full body restored. Entry/table and plane/query homes remain.
// MiniMax E (11-04): entry flags read per branch 442, the same with a shifted instance flag 444, the tag table read once at the top 455, an int index local 406, the entry address as an explicit stride and with named offset and index locals 406, the permuter's best 403 (that variant swaps call arguments); no lower score.
// MiniMax S (11-01): the entry address and the flags load keep the first difference at +0x10; no lower score.
// M3 lane 16 r91 (fast; aligned unmatched in brackets, control 406 [286]): retail builds the four lists in the order
// second, first, third, fourth (the reverse of its releases) and releases them on a sign-bit test (`js`) after reading all
// four capacities. Counts and capacities as two chained assignments 407 [284]; the four list starts in retail's order 405
// [284] (one point, not kept); the same through a four-caller inline list-start helper 405 (same code); with the plane
// query declared ahead of the lists 407; the starts in retail's order with the tag table read once at the top 454 (1728 B)
// [263]; with `!(capacity & 0x80000000)` release tests 482 (1776 B: the release block and the init's `mov esi, ecx`
// copies become retail's, but the plane loop's registers turn over) [264]; that plus the table local 462 (1792 B) [277].
// M3 lane 12 r93 (fast, 0x44720/0x458d0/0x45a80 match, 0x45560 182): the entry as `g_4ba138 + entry_index` 406
// (same code); the tag test's arms swapped (NONE arm first) 450 (1748 bytes); the geometry query count named and
// used for both tests 406; the tag in a named local for the test and the definition 406; the bottom arm's object
// index read before its tag 406. Retail's bottom arm reads the tag again, then the visibility word pointer, with
// the table still in ESI from entry.
// b2L walk (11-06, against retail statement by statement): entry/tag/section/geometry reads, query-count and parts test, list setup
// (frame offsets 0xb0/0x10c0/0x20d0/0x40e0, capacities), the call and release order, the node loop (unsigned /0x34, 0x142a60 args),
// the part loop (bound stride 0x14, scale test, centre rows, 0x165010 args, flag stores) all agree. One behaviour difference found
// and fixed: for a zero transform scale retail stores 0 into the plane distance (0x450e3 jnp -> 0x45129 stores xmm0 = xorps zero);
// the old body left it unchanged. The plane block is retail's 0x1428b0 inlined (same instructions), so it calls function_1428b0
// (in place; 0x1428b0 and 0x17e670 unchanged). Tried with the call: whole-plane store 478, normal vector copy 478, d first 476,
// views declared before the entry 487, tag table in a local 485. Remaining: retail spills views [esp+0x2c] and entry [esp+0x30]
// and loads the table into ESI up front; ours keeps them in registers (42 bytes smaller).
// m3 lane 12 r103 (fast unless marked; aligned = difflib unmatched ours/retail raw, then registers masked; base 243/250,
// 120/127): releases as `!(capacity & 0x80000000)` 464 (1760 B; 233/235, 119/121); with chained counts/capacities 455
// (1751 B; 234/238, 130/134); with the tag table local 443 (1760 B; 247/247, 138/138; plain 443, kept); then the flags
// at each use 465; mask tests and table local without the chains 450 (1744 B); chains and table local with `>= 0` tests
// 486 (1696 B; 226/236, 111/121); 0x458d0/0x45a80's index `long` with `(short)index` in their bodies and 0x45560 passing
// its long index (retail's arms push `movsx eax, word [ebp + 8]`; all three stay matched) 480 (1776 B; 242/238, 132/128);
// the chains in retail's store order (counts fourth = third = first = second, capacities fourth = third) 443 (retail's
// count and capacity stores exactly; 245/245, 134/134), not kept (no gain); on the kept form a state local for the views
// read and the active test 463, a structure local for the else arm's three reads 443, both 468.
// M3 lane 1 r88 (fast with 0x44720, 0x458d0, 0x45a80, all matched; aligned = difflib count, base 208): the entry,
// tag-table, flags, workspace and geometry declarations in five orders 443 each (same code). Retail stores `parts` to its
// byte and reloads it, then picks the query data in two arms, each reading the geometry's +0x2c field (the not-parts arm
// first). Re-measured on this base (round 166's conditional/negative-first forms and the if/else arms form): an if/else with
// the read in both arms 446 (1744 B, aligned 212: the parts test is threaded into the arms, no reload); the same with
// `!parts` first 482 (1780 B, aligned 194); a ternary 469 (1764 B, aligned 194); the ternary with the arms the other way
// 482 (1780 B, aligned 195). New combinations: the list starts in retail's order (second, first, third, fourth) with the
// counts and capacities chained in retail's store order 443 (aligned 203, same score); the retail-order chains with the
// if/else query data 446 (1744 B, aligned 207); the retail-order starts with the ternary query data 469 (1764 B, aligned
// 194); starts, chains and ternary together 469 (aligned 190).
// Round 228 (m3 lane 3, fast with 0x44720, 0x458d0, 0x45a80, 0x45560, all still matched; aligned counts
// exact/stack-masked/register-masked in brackets, base 443 [336/298/148]): the tag table local declared after the
// flags 443; `parts` declared const 443; the doubled part count written `count * 2` 443; the plane copy declared,
// then assigned 443 (same code each); the views and view-range pointers computed ahead of the tag test 452 (1763 B)
// [336/313/147]. Re-measured: the flags read in each arm 465 [367/296/138]; the six-plane `do`/`while` as a plain
// `for` 2639 (3504 B, fully unrolled; retail keeps it rolled). All restored.
// MiniMax R (11-10): no new form, no lower score (443): ours is 1760 bytes against retail's 1742 and
// the first difference is at +0x10, where retail forms the entry address as a scaled index added to
// the entry table (`movsx eax, word [ebp + 8]`, `shl eax, 5`, `lea ebx, [eax + 0x4ba138]`) and we
// form it as a shifted copy added to the same table (`shl edi, 5`, `add edi, ...`); retail then
// loads the tag table into esi and spills the entry at [esp+0x30], we keep the entry in a register.
// What stands between the two is the allocator's choice of live ranges, not a source shape: the forms
// the earlier rounds record (the tag table local, the entry and the two views declared in five
// orders, the release masks, the chained counts and capacities, the query data read in the arms, the
// plane loop rolled as a plain `for`) all measure 443 or worse.
// MiniMax A (11-12): no new form, no lower score (443); this build is 1760 bytes against retail's 1742 and the first
// difference is at +0x10, where retail forms the entry address as a scaled index added to the entry table
// (`movsx eax, word [ebp + 8]`, `shl eax, 5`, `lea ebx, [eax + table]`) and this build forms it as a shifted copy
// added to the same table (`shl edi, 5`, `add edi, ...`); retail then loads the tag table into esi and spills the
// entry at [esp+0x30] while this build keeps the entry in a register.
// m3 lane 6 r181 (fast with 0x3e380/0x43a40/0x44720/0x45560/0x458d0/0x45a80 matched throughout; control 443, aligned/register-masked
// 336/148): LEAD (declaration move, match-only): retail reads the visibility-word pointer (+0x14) in the bottom arm right after the tag,
// before the object lookup and the node loop's calls; `visible` declared at the top of that arm 456 (1760 B) but aligned 246, masked 122:
// the entry address is then formed ahead of the pushes as in retail. On it: a state local for the views and active reads 454 (1749 B,
// 259/118); the flags read at each use 447 (1750 B, 245/121); both 443 (1762 B, 246/119); those plus 0x458d0/0x45a80's index as `long`
// with `(short)index` in their bodies (no address-of; both and 0x45560 stay matched, retail's arms push `movsx eax, word [ebp + 8]`)
// 487 (1780 B, 234/104: both call arms then set up as retail's); with the arms as `if (!parts) 458d0 else 45a80` (retail's layout) 487
// (232/103). Left on that: the zero constant in EBX (retail keeps the entry there), the list-start stores, the two-step frustum source
// pointer, the plane cursor at .k and the j/k register naming, the tag table reloaded in the bottom arm, the bound centre copied with
// integer moves (retail loads it into xmm0-2 and scales in place: 0x142570's body, /Ob1 file).
// Geometry/query/fallback entry scope, seven nonempty subsets, then all with direct fallback visibility mark: 449/457/449/449/449/449/449/449.
// NOT kept: seven forms come within two bytes of retail at 449 differences. All eight preserve the first entry-address difference and increase the score.
// Original executable body restored.
// MiniMax D (11-17): no new form, no lower score (443, re-measured with the full check; 1760 bytes
// Re-measured at 443. MiniMax D (11-17): no new form, no lower score (443, re-measured with the full check; 1760 bytes
// against retail's 1742): the first difference is still at +0x10, where retail forms the entry address as a scaled index
// added to the entry table (`movsx eax, word [ebp + 8]`, `shl eax, 5`, `lea ebx, [eax + 0x4ba138]`) and this build forms it
// as a shifted copy added to the same table (`shl edi, 5`, `add edi, ...`), and retail then loads the tag table into esi and
// spills the entry at [esp+0x30] where this build keeps the entry in a register; what stands between the two is the
// allocator's choice of live ranges, not a source shape, and the forms the earlier rounds list (the tag table local, the
// entry and the two views in five declaration orders, the release masks, the chained counts and capacities, the query data
// read in the arms, the plane loop as a plain `for`) all measure 443 or worse.
// MiniMax C (11-17): no new form, no lower score (443): this build is 1760 bytes against retail's 1742 and the first difference is at +0x10, where retail forms the entry address as a scaled index added to the entry table (movsx, shl, lea) where this build forms it as a shifted copy added to the same table (shl, add), and retail then loads the tag table into esi and spills the entry at [esp+0x30] where this build keeps the entry in a register; what stands between the two is the allocator's choice of live ranges, not a source shape - this round's named entry-offset local (the entry taken from the table with an index held separately) gives 443, unchanged, and the earlier rounds' forms measure 443 or worse.
// Y616: Shared section ordinal 467, common entry tag 443, indexed entry at
// every use 469, shared instance bit 481, fallback base and offset 443, signed
// range grid 443, conditional query displacement 460. NOT kept: geometry
// field read at the join 438 is below the gate and grows to 1764/1742.
// The entry address and register-save order still differ.
// Fallback base plus displacement 443, fallback geometry-address join 460, same-base structure reads
// with a displacement join 464, one tagged/fallback address join 452, per-path flags with the
// displacement join 462, displacement before the base 464, address join with the bool selector 441,
// displacement join with the entry by reference 443. NOT kept: 441 saves two points. The entry
// address and tag-table saves still differ.
// M3 lane 13 r107 (fast with 0x458d0, 0x45a80, 0x44720, 0x44940, 0x449e0, 0x45560, 0x460d0, all MATCH unless noted; control 443,
// difflib-aligned 336, masked 148): KEPT 443 -> 430 (plain, 1751 B; aligned 267, masked 129): the hand-written geometry lookup
// replaced by the existing visible_entry_geometry(short) of 0x458d0/0x45a80, moved from src/unknown_03f2b0.cpp into the header
// the two files share (both callers stay matched); the flags local goes with it and the table is read where the bottom arm
// uses it (a table local kept for that one use is the same code). On that: a state local for the views and active reads 434;
// the query data as `if (!parts) ... else ...` with the +0x2c read in each arm 456 (1763 B). Lead, not kept (it raises this
// count): 0x458d0's and 0x45a80's index parameter as `long` (retail's two callers show it: this function pushes `movsx eax,
// word [ebp + 8]` and 0x45560 pushes its long index register) keeps 0x458d0, 0x45a80 and 0x44720 matched with a plain
// `(short)index` in place of the read through the parameter's address, and 0x45560 too once its call passes `index` without
// the cast; this function is then 458 (1776 B, first difference +0x14, aligned 254, masked 107): the entry is formed with
// retail's `lea` from the scaled index and the two call arms come out separately with retail's `movsx` pushes, but in source
// order (retail lays the !parts arm first at both parts tests). Left on the kept form: the entry in EDI (retail EBX), the
// table in ECX (retail ESI from entry to the bottom arm), the views and zero in EBX, the plane copy's j/k registers and the
// plane cursor (0x1428b0's inlined body), the visible-mask read after the node loop (retail before it).
// Also on the kept form: the +0x30 count as a named local for its two tests 430 (same code).
// M3 lane 4 r213 (fast with 0x458d0, 0x45a80, 0x44720, 0x44940, 0x449e0, 0x45560, 0x460d0, all MATCH each run; control 430,
// difflib-aligned 215 exact / 73 masked). Nothing kept. Lane 13's lead re-measured on today's base: 0x458d0's and
// 0x45a80's index parameter as `long` with `(short)index` in their bodies and 0x45560 passing its long index 458 (1776
// bytes, first difference +0x14, aligned 211 / 64; the three callee-side functions stay matched); with the call arms as
// `if (!parts) 0x458d0 else 0x45a80` 458 (211 / 63). On that: the frustum address as a pointer local with the planes
// at +0x78 (retail's two `lea`s) 458 (same code); the query data read in each arm, !parts first, 484 (1792 bytes,
// 207 / 65); both 484. On the kept body: the frustum pointer local 430 (same code); the arms !parts first 430 (216 / 73).
// Reading of the head on the long-index form: retail keeps the entry in EBX, the tag table in ESI (loaded after the tag
// compare, kept to the bottom arm) and the geometry in EDI, with the views and the state pointer in ECX/EDX; ours gives
// EDI to the entry, ESI to the state pointer and EBX to the views and then to a zero that serves the count test, the
// active test and the four count stores, where retail tests with `test` and has a zero (EDI) only for the count stores
// and the view counter. The geometry sitting in EDX is what pushes the state pointer into ESI.
// M3 lane 12 r146 (fast with 0x142570, MATCH): the view counter declared ahead of the list setup (`long view = 0;`, the
// loop without an initializer) 430 (its store joins the EBX zero); the query count named once for the outer test and
// the parts test 430 (same code); the outer test inverted with the node-transform arm first 463 (1758 bytes); the outer
// test as `>= 1` 453 (1760 bytes: the active test then also takes a register zero); the state pointer named for the
// views read and the active test 434. A compile-only screen shows the early zero (`xor; cmp eax, reg; cmp [state +
// 0x2a78], reg`) for per-list setup in three orders, separate count stores, `active == 0`, the parts flag set by an if,
// the flags tested first, and even with the active test removed: the `> 0` count test alone joins the store zero.
// MiniMax A (11-18): no new form, no lower score (430): retail forms the entry address with a lea into ebx from the scaled
// index after the first register save and reads the tag through that pointer, while this build reads the tag through a
// displacement from the scaled index and only then materializes the entry pointer; retail loads the tag table into esi
// after the tag compare and this build hoists it into ecx at the very top, and the entry's spill is at [esp+0x48] here
// against retail's [esp+0x30]. The geometry lookup goes through the header's visible_entry_geometry, whose own
// &g_4ba138[index] is what the based-pointer folding applies to; the source permuter's best variant was one differing
// instruction better and carried a declaration split and a pointer-to-a-local.
// M3 lane 1 r127 (fast with 0x142570/0x1428b0/0x165010/0x2dd930/0x45a80/0x458d0, all MATCH; control 430): the entry local
// renamed 430 (same code: the names do not move its [esp+0x48] home); the entry as a reference-bound local (`entry.` at
// each use) 430 (same code); a redundant `if (0 < node_count)` around the node loop 430 (one `lea` operand order only); the
// entry declared after the geometry and views 430 (same code); diagnostic: the geometry through a copy of the pointer-taking
// (entry, flags) helper that src/unknown_044720.cpp keeps private 449 (1760 B: the entry is then formed before its reads,
// `add edi, table`, but stays in EDI and the tag table moves to EDX). Body unchanged.
// M3 lane 5 r208 (fast with 0x458d0, 0x45a80, 0x44720, 0x45560, all MATCH each run; control 430, difflib-aligned 215,
// masked 126). Nothing kept. Aimed at the zero register (ours creates one in EBX ahead of the count test and uses it for
// both tests and the count stores; retail tests with `test reg, reg`, and its zero starts at the count stores, in EDI,
// where it also serves the view counter): the count test held in a named bool 430 (same code); the parts flag started
// false and set under three nested tests 438; the active flag read into a named local ahead of the count test 430 (same
// code); the query count named once with the bool count test 430 (same code); the count test as `!(count <= 0)` 430
// (same code). Aimed at the byte both arms share at [esp + 0x1b]: one function-scope flag for the parts test and the
// containment output 430 (same code); with the query data picked in two arms, each reading the geometry's +0x2c field,
// `!parts` first, 456 (1763 B); the two-arm query data alone 456 (re-measured on this body). Reading: the zero is a
// consequence, not the cause: retail gives EBX to the entry (until the node loop), ESI to the tag table (from the head
// to the bottom arm) and EDI to the geometry (both arms; its home [esp + 0x20] is the slot the top arm reuses for the
// query data), ours gives EDI to the entry, ESI to the state pointer and leaves EBX free for the zero. The `long` index
// lead for 0x458d0/0x45a80 was not re-measured: their file was another lane's this round.
// M3 lane 13 r122 (fast with 0x458d0/0x45a80/0x44720/0x44940/0x449e0/0x45560/0x460d0, all MATCH each time; control 430,
// difflib-aligned 213/215): retail loads the tag table into ESI before the tag test (matched 0x458d0 and 0x45560 load it
// inside the tag path), so 0x44e90 reads it once for both the lookup and the bottom arm; a table local read at the top
// for the bottom arm is sunk to its one use (430, same code; 458 on the long-index lead). The query data through a
// `byte *const &tree` bound to the geometry's +0x2c field (0x45560's spelling) as `parts ? tree + aligned size : tree`
// 426 (1760 bytes, aligned 212/212: each arm reads the field, as retail, but the parts arm comes first), as `!parts ?`
// 426 (aligned 209/209, the arm swap is match-only), with the field written out at each read 456 (1763 bytes), with an
// if on `query_data = tree` 430 (same code); the first form on the long-index lead 489 (1792 bytes). Diagnostic: 1-12
// dead initialised locals at the top 430 each (flat: the inlined plane transform's product order is no symbol-count
// state here). The long-index lead re-measured 458 (1776 bytes).
// M3 lane 3 r275 (fast with 0x458d0/0x45a80/0x165010/0x1428b0/0x2dd930 MATCH; control 430, difflib-aligned 215,
// stack-masked 184): the query data as an if/else reading the field in each arm 449 (1760 B, aligned 217: the parts
// bool is then threaded into the arms where retail re-tests its stack byte); as a ternary 449 (same code); the
// plane copy as `planes[j].n = p.n;` 418 (1744 B) but aligned 253 and masked 230 (the inlined plane transform
// loses retail's shape: not kept); the frustum base named before the +0x78 source 430 (same code); 0x1428b0's
// loads ordered i, k, j (retail's inlined copy names k before j) 430 here and 0x1428b0 lost (16): restored. LEAD,
// not keepable (a memory read moved ahead of calls): the bottom arm's visible-bits pointer read at the top of the
// arm, before the node loop, as retail does ([ebx+0x14] read beside the tag) 397 (1778 B, aligned 193, masked 167).
// M3 lane 1 r132 (fast with 0x458d0/0x45a80, both MATCH each time; control 430): ours already loads the tag table at the
// top (ECX) but only for the tag path; retail keeps it in ESI to the bottom arm, keeps the entry in EBX in the outer
// region (reloading it from its home after the frustum loop's call) and the geometry in EDI. A tag-table local read
// before the geometry lookup and used by the bottom arm 430 (same code). Retail's bottom arm reads the tag, then the
// visible-mask pointer (+0x14, stored to a home), then the object: the mask pointer read right after the tag, ahead of
// the node loop, 397 (1778 B: the geometry then takes EDI as retail; a read moved past the loop's calls, match-only, the
// lead); on 397: the tag-table local 397 (same code), the query data read in each arm (!parts first) 469, view_ranges
// computed just before the view loop 479, the state pointer named for the views read and the active test 441 (1762 B,
// the state then in EAX near retail's EDX; the early EBX zero remains). Diagnostic on 441: the geometry through a
// pointer-taking copy of the lookup 417 (1776 B: the entry is formed first, `add ecx, table`, but stays in ECX).
// M3 lane 11 r133 (fast with 0x458d0, 0x45a80, 0x44720 and 0x45560, all MATCH each run; control 430, difflib-aligned 267,
// register-masked 195, frame-blind 129; body unchanged). LEAD, match-only, saved as build/r133/lead-0x44e90.patch: 396 at
// retail's 1742 B and 456 lines, first difference +0x57, aligned 153, masked 108, frame-blind 55. Its parts, each on the one
// before: the mask pointer read right after the tag in the bottom arm (lane 1 r132's form, 397; matched 0x45560 reads it
// at the top of its bottom arm too); 0x45560's opening (`long index = entry_index;`, the geometry through a copy of the
// pointer-taking lookup that src/unknown_044720.cpp keeps private, then `entry = &g_4ba138[index]`) 444 (the entry is then
// formed by retail's `lea` from the scaled index; with `entry` formed first and passed 416); the query data as `byte *const
// &tree` on the geometry's +0x2c field with an if/else, `!parts` first, 399 (each arm reads the field and the flag byte is
// stored and re-read as retail; as a conditional expression 399, the same code); the four lists set up one list at a time
// in the order second, first, third, fourth 399 (aligned 221, frame-blind 96: from here the whole head up to the list
// setup is retail's instruction for instruction, entry in EBX, tag table in ESI from the entry block to the bottom arm,
// geometry in EDI, state and views in EDX/ECX, `test` for the count and the active flag; only the homes differ); the part
// centre in 0x142570's shape (x, y, z reals, `scale * x` under the test, rows up, left, forward, position) 396 (retail's
// length; but an early zero returns in EAX for the count test, the active test and the four count stores, and the query
// data's two arms share one store again); the list setup through a four-caller start helper (list, storage, capacity)
// with the plane query declared ahead of it 396 (aligned 155: the capacities then come out as retail's two loads with a
// register copy each, `mov esi, ecx` / `mov eax, edx`); the containment flag declared inside the visibility test 396
// (aligned 153, frame-blind 55: its clear then sits after the scale compare as retail's).
// On the way, not in the lead: the query data as a plain if/else with the field read in each arm 422 (the flag test is
// then threaded away; with the flag read through a `bool const &` 422, with the field written out in the second arm 422,
// `parts` arm first 429); the view counter declared ahead of four separate count stores 378 on the reference form but
// with the head's registers gone again (first difference +0x17), 412 on the plain form; renaming the six dword locals
// so their names sort in retail's slot order, and in the reverse order, 399 each (same code: names do not order these
// homes); the frustum address as a pointer local with the planes at +0x78 399 (same code); the call arms `!parts` first
// 399 (same code); the scale step as `x *= transform->scale` or `x = x * transform->scale` 396 (same code), with a named
// scale 397 (458 lines); a named active flag 396 (same code); the view counter declared ahead of the list starts 400; the
// plain two-arm query data on the lead 401; counts, starts and capacities grouped by field 396 (frame-blind 68). On this
// body alone: the start helper in retail's order 457, in the present order 459.
// Left on the lead: the early zero (retail makes its zero in EDI after the plane query's address, for the four count
// stores and the view counter's home); the homes (retail, upward from +0x1c: view ranges, geometry and query data, inner
// counter, view counter, views, entry at +0x30 below the plane copy; ours has the entry at +0x4c in the gap under the
// planes and the others in another order); the frustum address in two `lea`s; the two call arms laid out separately with
// their own pushes; the part loop's counter and bit (retail ESI/EBX, ours EBX/ESI) and its scale products (retail `x *
// scale` in place with the scale held in XMM3, ours `scale * x` with a reload); the bottom arm's first reads.
// M3 lane 4 r234 (fast; 0x44720, 0x44c90, 0x45560, 0x458d0 and 0x45a80 MATCH throughout; control 430): the pointer-taking
// geometry lookup of src/unknown_044720.cpp moved into include/unknown_044e90.h and called here with the entry and its first
// word 449 (1760 B); on that, the entry formed from a `long` copy of the index first 449, 0x45560's opening exactly (the
// lookup on the indexed element, then the entry) 440 (1751 B), the same without the `long` copy 443; header and both files
// restored. On main's body, the part centre in 0x142570's shape (x, y, z reals, `scale * x` under the test, rows up,
// left, forward, position) 432 (1744 B, first difference +0x12). Retail keeps the tag table in ESI from the head to the
// bottom arm and the entry in EBX; ours still loads the table into ECX for the lookup only.
// M3 lane 1 r136 (fast with 0x458d0 and 0x45a80, both MATCH each run; control 430): the plane copy declared once at function
// scope 430; the query aggregate declared ahead of the plane array and filled by two stores 430 (same code each); each plane
// stored as an aggregate copy with d negated after it 431 (1767 B); the transform arm writing into the copy from the source
// with an else-arm aggregate copy 440 (1760 B); the containment flag declared inside the visibility test 430 (same code).
// Reading: retail's plane copy sits 8 bytes higher (+0x34, ours +0x2c) and the query count at +0x44 (ours +0x3c), with the
// tag table held in ESI from the head to the bottom arm (ours in ECX). Body unchanged.
// M3 lane 1 r141 (fast with 0x458d0, 0x45a80 and 0x1428b0, all MATCH each run; control 430 at 1751 B, aligned 267, register-
// masked 129). New reading of the inner plane loop: the order in which the copy after the transform reads the plane's
// components sets the XMM numbers of the inlined 0x1428b0 (its loads ahead of the scale test and its first sum's operand
// sides) and the member the loop's output pointer is anchored on. Retail reads i, k, j (XMM4 / XMM5 / XMM6, the distance
// sum as y * j + z * k), anchors the pointer on k (`lea edx, [esp + 0x58]`) and stores i, j, k, -d upward. LEADS, not
// kept (statement order or one-use locals with no match): the copy written i, k, j, -d 427 (aligned 241, masked 122:
// the whole inlined transform is then retail's instruction for instruction and the pointer is anchored on k; left in the
// loop only the j and k stores in the other order); the components read into three locals in the order i, k, j and
// stored i, j, k, -d 427 (aligned 243: retail's registers and store order, but the pointer anchored on j). Other
// forms: the normal as an aggregate copy then d 418 (1744 B, aligned 336, masked 162: the copy goes through integer
// registers and the transform loses retail's shape, as lane 1 r132 found; not kept); d first 430; k, j, i, -d 430
// (aligned 275); d negated in the copy and the whole plane assigned 431; the normal through the existing vector3f_assign
// then d 430 (aligned 274: arguments are read k, j, i, pointer anchored on k, stores i, j, k), with d stored first 430,
// with the normal's own members as arguments 430; i, j, then d, then k 430; a named negated d ahead of the three
// stores 430 (same code); a plane temporary filled i, j, k, -d or i, k, j, -d and assigned whole 464 / 461 (1783 B, each
// member goes through the stack). Diagnostics, removed at once: the four reals through an SSE set intrinsic in
// either argument order 446 / 446 (1767 B: built on the stack and copied with movaps), through the existing four-real
// vector's set member 430 (as vector3f_assign), i stored and then j and k through a two-real setter 427 (pointer
// anchored on i). Body unchanged.
// M3 lane 11 r145 (H2M fast with 0x458d0 and 0x45a80, both MATCH each run; control 430): the geometry pointer bound to a
// `byte *const &` and read through it 451; the entry pointer bound to an `s_44940_entry *const &` 430 (same code); a redundant
// `if (0 < frustum count)` guard around the frustum loop 467 (1776 B); the plane stores through `query.planes[j]` 441 (1760 B,
// first difference +0x7). The part centre through the existing transform4x3f_apply_point (0x142570) 633 (1539 B, a call);
// with `inline` on 0x142570's definition as well (diagnostic) it is inlined here, 432 at 1744 B, but 0x1396c7 (48) and
// 0x24e59f (42) then stop matching (retail calls it there), so 0x142570 stays out of line. Body unchanged.
// M3 lane 12 r169 (h2m fast with 0x458d0, 0x45a80, 0x44720 and 0x45560, all MATCH; control 430): the geometry lookup
// ahead of the entry pointer 430 (same code); the part-count test as `>= 1` (diagnostic of the EBX zero) 453 (1760 B); the
// four list counts as separate stores 430 (same code); `view_ranges` computed inside the parts arm 451; the parts bit as
// `(entry->unknown00 >> 12) & 1` 430 (same code). Retail keeps the tag table in ESI from above the tag test to the bottom
// arm and has no function-level zero; ours loads the table into ECX for the lookup only and keeps a zero in EBX.
// M3 lane 5 r229 (h2m fast with 0x458d0 and 0x45a80, both MATCH; control 430): the view ranges as a `short *` read by
// element (`view_ranges[view + 1]`, `view_ranges[7 + view]`) 430; the bound index held in a `dword` 430; the geometry
// count test as `0 < count` 430; the part loop test as `count > part` 430 (same code each); the part-centre scale test
// as `1.0f != transform->scale` 436 (1755 B).
// M3 lane 16 r170 (h2m fast with 0x458d0, 0x45a80, 0x44720, 0x45560, 0x1428b0, all MATCH; control 430): the query data as
// a plain if/else with the field read in each arm, `!parts` first (retail's fall-through arm) 456 (1763 B, first +0x12),
// `parts` first 449 (1760 B); the geometry lookup ahead of the entry pointer 430 (same code); the bottom arm's node transforms
// named once from the tag 436 (1749 B); the visibility word as a `dword *` formed once per part for the test and the set 430
// (same code).
// M3 lane 16 r180 (H2M_FAST with 0x458d0, MATCH each run; control 430): the plane copy written i, k, j 427 (re-measured,
// lane 1 r141's form); the plane local declared ahead of the plane loop and assigned in it 430 (same code); the frustum index
// as `short` 440 (1760 B); the view counter as `short` 455 (1760 B); the copy through a `plane3f &` to the element 430 (same
// code); the transform reading the frustum plane straight into the local with the copy in an else arm 440 (1760 B, aligned
// 339). Body unchanged.
// M3 lane 5 r239 (H2M_FAST with 0x458d0, 0x45a80, 0x44720 and 0x45560, all MATCH; control 430): the shared geometry
// lookup reading the tag table into a local ahead of its tag test (retail's hoisted ESI load) 430 (same code: the load
// still sinks into the arm; header restored); the four index pointers set second, first, third, fourth 430 (same code);
// each list set up whole in that order 457 (1760 B); the plane copy as a `for` with `++j, ++source` 2599 (3481 B: a
// constant-count `for` is unrolled, the do-while is what keeps retail's loop), as a `for` over `source[j]` 2599.
// M3 lane 8 r233 (h2m fast with 0x458d0, 0x45a80 and 0x142570, all MATCH; control 430). Allocator reading: the
// transform and frustum values take ECX/EDX/ESI over the plane loop and the part index EBX, so the entry (retail EBX)
// and the tag table (retail ESI from the tag test to the bottom arm) never get callee-saved homes here. Forms: the view
// ranges computed inside the count test 451; the geometry looked up ahead of the entry pointer 430 (same code); the
// visible-mask pointer read before the node loop 397 (1778 B, the listed match-only lead); the containment flag
// declared ahead of the part bit 430 (same code); the planes and plane query declared ahead of the list set-up 430
// (same code). Body unchanged.
// Tag snapshot 430, shared structure-section load 452, single geometry result 443, with the tag snapshot 443,
// instance-first section 452, single result and section join 452, structure flag snapshot 456, and flag
// snapshot with section join 452. NOT kept: the shared section and flag forms lose both matched helper users,
// and the remaining forms tie or raise the score.
// M3 lane 14 r152 (h2m fast with 0x458d0, 0x45a80, 0x44720 and 0x45560, all MATCH each run; control 430, sequence-aligned
// 267 rows; a per-register comparison of retail's value ranges with ours: 49 of 128 placed differently). Allocator
// reading of the bottom arm: the geometry pointer's whole range (head, part loop, live through the node loop) ranks 53
// and the node loop's output-base difference 54, so that difference takes EDI, the countdown EBX, and the geometry is
// cut into two scratch pieces; retail has the order the other way (geometry EDI through the node loop, the base
// difference EBX, the node countdown in memory), and once the geometry holds EDI the entry can only take EBX. Two
// more points on the geometry (one more reference in the head or the part loop, or one candidate fewer in the node
// loop's blocks) is the whole gap. Re-measured against that: the query data as `if (!parts) ... else ...` with the
// +0x2c field read in each arm 456 (1763 B, listed): the two extra geometry reads do give retail's roles (geometry
// EDI with its home at +0x20, 18 of the callee-saved ranges then agree), but the entry is formed in a scratch register,
// the table is loaded inside the tag arm and the parts test is threaded into the arms. Lane 11 r133's lead rebuilt on
// today's body 396 at 1742 B (aligned 154; 24 of 135 ranges placed differently: ECX/EDX swapped from +0xe3, the part
// loop's counter and bit, one EDI range, 19 stack cells); with 0x458d0's and 0x45a80's index parameter as `long`
// (`(short)index` in their bodies, 0x45560 passing its long index; all three stay matched) the lead is 412 at 1756 B,
// aligned 139, and the two call arms are then retail's row for row; the same parameter change on this body 458 (1776
// B, aligned 254, re-measured), on the two-arm query form 484. Left on that best form, by group: the early zero
// (retail tests the count and the active flag with `test` and makes its zero in EDI after the plane array's address);
// the query data stored once after the arms (retail stores it in each arm); the list set-up's store order; the
// frustum address in one `lea` (retail two); the plane copy's j/k registers; the capacity loads ahead of the releases
// (retail second, first, third); the part loop's counter and bit (retail ESI/EBX); the scale products (0x142570's
// body, BLOCKED-BY 0x142570's /Ob1 file); the homes. Body unchanged.
// Fallback through the existing object lookup 430, header lookup 430, node base helper 430, object-plus-node 430,
// header-plus-node 430. Each single helper with one view state read through the parts test 434. Entry/table
// allocation and the geometry home through node composition are unchanged.
// Scalar-input shared rotation with coordinate stores/vector add/in-place input/independent rotation 430/426/457/426;
// axes rotation with the same four consumers 430/426/514/430.
// NOT kept: the 426 forms gain four at 1745 bytes and leave entry/table allocation unresolved.
// M3 lane 12 r187 (H2M_FAST with 0x458d0, 0x45a80, 0x44720, 0x45560, 0x1428b0 and 0x142570, all MATCH; control 430
// at 1751 B, aligned unmatched rows [exact, stack masked, registers masked] 215/184/73; body unchanged). The query data
// as `if (!parts) ... else ...` with the +0x2c field read in each arm, through a named base or written out, 456
// (1763 B) [217/191/74] each (re-measured for its aligned count); the view counter declared ahead of its loop 430
// (same code), declared after the four count stores (retail zeroes it with them from one register) 430 [216/185/73];
// both with the two-arm query 453 (1760 B); the tag table read into a local at the top (ahead of or after the entry
// pointer) and used in the bottom arm 430 (same code: the copy is folded and the load stays in the lookup's arm), with
// the two-arm query 456. Reading: retail holds three values in saved registers from the head, the entry (EBX), the
// tag table (ESI, loaded once ahead of the tag test and read again in the bottom arm) and the geometry (EDI), and keeps
// the view state pointer in EDX for its two reads; this build gives ESI to the view state pointer and EBX to the
// views, so the table is a scratch piece that ends in the lookup and the entry is spilled. Diagnostic (a file-local
// copy of the lookup taking the entry pointer, removed): alone 449 (1760 B) [244/208/75]; with the two-arm query
// 481 (1777 B) [213/184/71], and then the entry is in EBX from the first instruction with the tag read through it and
// the geometry in EDI, as retail; the table is still loaded inside the tag arm.
// M3 lane 16 r194 (h2m fast with 0x458d0, 0x45a80, 0x44720 and 0x45560, all MATCH; control 430 at 1751 B): the frustum
// transform read once ahead of the view loop 483 (1780 B, first difference +0x7); the plane taken through 0x1428b0 from
// the source into the copy, with a plain copy in the else arm, 440 (1760 B); the plane stored as an aggregate assignment
// and its distance negated after 431 (1767 B); the four counts cleared by four statements 430 (same code); the
// containment flag declared ahead of the part loop and cleared in it 430 (same code). Body unchanged.
// @retail 0x44e90
void __stdcall function_44e90(short entry_index, short section_index)
{
    s_visibility_query_workspace workspace;
    s_44940_entry *entry = &g_4ba138[entry_index];
    byte *geometry = visible_entry_geometry(entry_index);
    byte *views = *(byte **)g_4c0b78.state->unknown00;
    byte *view_ranges = views + section_index * 0x1a + 0xa6e;
    if (*(long *)(geometry + 0x30) > 0) {
        bool parts = !g_4c0b78.state->active && (entry->unknown00 & 0x1000) &&
            *(long *)(geometry + 0x30) > 2 * *(long *)(geometry + 8);
        byte *query_data = *(byte **)(geometry + 0x2c);
        if (parts)
            query_data += (*(long *)(query_data + 0x20) + 15) & ~15;
        workspace.first.list.indices = workspace.first.indices;
        workspace.second.list.indices = workspace.second.indices;
        workspace.third.list.indices = workspace.third.indices;
        workspace.fourth.list.indices = workspace.fourth.indices;
        workspace.first.list.count = workspace.second.list.count = workspace.third.list.count = workspace.fourth.list.count = 0;
        workspace.first.list.capacity = workspace.second.list.capacity = (long)0x80000400;
        workspace.third.list.capacity = workspace.fourth.list.capacity = (long)0x80000800;
        __declspec(align(16)) plane3f planes[6];
        s_visibility_plane_query query = { 6, planes };
        for (long view = 0; view < *(short *)views; ++view)
        {
            for (long i = 0; i < ((short *)view_ranges)[view + 1]; ++i)
            {
                long frustum_index = *(short *)(view_ranges + 0xe + view * 2) + i;
                plane3f const *source = (plane3f *)(views + 0x1974 + frustum_index * 0x108 + 0x78);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                long j = 0;
                do
                {
                    plane3f p = *source;
                    if (transform)
                        function_1428b0(transform, &p, &p);
                    planes[j].i = p.i;
                    planes[j].j = p.j;
                    planes[j].k = p.k;
                    planes[j].d = -p.d;

                    ++source;
                    ++j;
                } while (j < 6);
                function_2dd930(query_data, &query, &workspace.first.list, &workspace.second.list);
            }
            if (parts)
                function_45a80(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
            else
                function_458d0(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
        }
        release_visibility_list(&workspace.fourth.list);
        release_visibility_list(&workspace.third.list);
        release_visibility_list(&workspace.first.list);
        release_visibility_list(&workspace.second.list);
    } else {
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        long object_index = **(long **)entry->unknown10;
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        long node_count = *(short *)(object + 0x114) / sizeof(transform4x3f);
        transform4x3f const *nodes = (transform4x3f *)(object + *(short *)(object + 0x116));
        for (long i = 0; i < node_count; ++i)
            function_142a60(&nodes[i], (transform4x3f *)(*(byte **)(tag + 0x4c) + i * 0x60 + 0x28), &workspace.transforms[i]);
        dword *visible = *(dword **)(entry->unknown10 + 4);
        for (long part = 0; part < *(long *)(geometry + 8); ++part)
        {
            dword bit = 1 << (part & 31);
            bool contained = false;
            if (!(visible[part >> 5] & bit))
            {
                word bound_index = *(word *)(*(byte **)(geometry + 0xc) + part * 8 + 4);
                s_visibility_bound_record const *bound = (s_visibility_bound_record const *)*(byte **)(geometry + 0x14) + bound_index;
                transform4x3f const *transform = &workspace.transforms[bound->transform_index];
                point3f local = bound->center;
                if (transform->scale != 1.0f)
                    vector3f_scale((vector3f const *)&local, transform->scale, (vector3f *)&local);
                point3f center;
                center.x = transform->forward.i * local.x + transform->left.i * local.y + transform->up.i * local.z + transform->position.x;
                center.y = transform->forward.j * local.x + transform->left.j * local.y + transform->up.j * local.z + transform->position.y;
                center.z = transform->forward.k * local.x + transform->left.k * local.y + transform->up.k * local.z + transform->position.z;
                if (function_165010((s_frustum_set_view *)views, section_index, &center, bound->radius, &contained))
                {
                    entry->unknown00 |= 1;
                    visible[part >> 5] |= bit;
                }
            }
        }
    }
}
