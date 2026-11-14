#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "vector2f_products.h"
// @flags /O2 /arch:SSE /Gr

struct s_1badc0
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0x5c - 0x26];
};

real normalize2d(point2f *arg_0);

// MiniMax H (11-13): no new form, no lower score (171): the permuter's 41 tries in 311 s found nothing below 171,
// and the difference still stands where retail computes the centre-minus-actor and the centre-minus-point differences
// in the prologue and the point-minus-actor difference at its use, while ours forms the point-minus-actor difference first.
// MiniMax C (11-11): no new form, no lower score (180); the length (1025) and the frame 0x90 agree, but the opening still forms the components in our order, and the copy schedule it needs is set by the query helper in src/unknown_1c20f0.cpp and the inline-ness of 0x30c60, both in other files this lane may not edit.
// MiniMax G (11-12): no new form, no lower score (171): retail computes the centre-minus-actor and centre-minus-point differences in
// the prologue and the point-minus-actor difference at its use, while ours still forms the point-minus-actor difference first; every
// reordering of the three difference blocks regresses (219, 219, 221, 231, 231) and the squared length's sum order makes no difference.
// score 171 (was 180): the centre is copied before the radius and both projections now go through
// dot_product2d on a named actor-to-centre vector, and the squared length sums its j term first as
// retail does; the opening still hoists the point's y difference above the copy.
// MiniMax H (11-11): round 66's other two changes re-measured here - `result = true` ahead of the
// output store and `2.0f - local_11` inside the normalize block give 238 and 242 alone, 226 with
// one point copy feeding both differences; the tail then matches retail exactly but the body does not.
// score 180 (was 180): retail's length (1025) and frame agree; the opening's copy and difference schedule and the SSE
// scheduling still differ. A 132 form and a byte-match recipe are measured but not kept (see Round 66 at the end of this note).
// Earlier at 180: no new form and no lower score; retail still forms d3.x, local_6.x,
// d1.x, d1.y, d3.y and local_6.y before the products while ours forms d1.y first and the two d3
// components last, and the two numerator terms swapped give the same code.
// MiniMax G (11-10): no new form, no lower score (180); round 66's 132 form is not present in this worktree, and its three order changes (the clearance moved into its use block, the result set before the output store and the centre copied before the radius) are the only known way below 180, so the opening still forms the components in our order.
// MiniMax E (11-10): no new form, no lower score (180); the length (1025) and the frame 0x90 agree, but the opening still forms the components in our order, which the query helper in src/unknown_1c20f0.cpp fixes.
// MiniMax D (11-09): no new form, no lower score (180); the opening still copies the point and orders the
// components differently from retail, and the escape of the point pointer through the query helper in
// another file this lane may not edit is what orders those copies.
// MiniMax S (11-09): still differs in the order of the components: retail forms d3.x, local_6.x, d1.x, d1.y, d3.y
// and local_6.y before the products, ours forms d1.y first and the two d3 components last; the two numerator terms
// swapped give the same code.
// MiniMax D (11-07): at 180: no new form and no lower score; the opening copies, the SSE
// scheduling and the query-success join still differ, and the earlier rounds' best form (47) needs the
// difference helpers written out by hand, so there is no keepable form here; the opening stays blocked by
// the unmatched 0x26d100.
// MiniMax E (11-05): no new form, no lower score: the projection's multiply order swapped, the point-minus-actor vector written as two scalars, and both together 180; the centre-minus-actor scalar pair 238 (1031 bytes); the permuter's 83 tries all 180.
// Earlier at 180 (was 180): retail's length (1025), output in EBP and the input copy at +0x28 now agree; SSE scheduling of the
// opening point copies and differences remains (retail reads the second copy back from memory for the actor difference).
// Earlier: frame 0x90 agrees; output EBX/EBP, point/vector homes and SSE scheduling remain.
// Round 132 (m3 lane 5, fast): the output-parameter address read is what keeps the output on the stack
// (without it 274, 996 B). That read at the 0x26d100 call instead of the entry self-assignment, reference at the
// top/just before the call 211/211; the input point copied after the object lookup 211 (1005 B); the result-flag
// clear before the actor lookup 261; the far path's output index before the point 217.
// Earlier: Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain. Retail
// Round 30 (lane D): the permuter's best variant scores 194 against its own 195 baseline (one point,
// below the retention margin). What still differs: this body loads the output pointer into EBX before
// saving EBP, retail saves EBP and keeps the pointer there, so the point copy runs through EBP where
// retail uses EBX; the body is also 21 bytes short of retail. Original body kept.
// Earlier (merged stacked note): Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain. Retail
// Earlier 211 (was 211): Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain. Retail
// frame 0x90 agrees; output EBX/EBP, point/vector homes and SSE scheduling remain.
// Earlier: Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain. Retail
// copies the input point a second time (at 0x28, through integer moves) for the two vector
// differences and keeps the center at 0x34; ours has the center at 0x28 and the to-center vector at 0x34.
// Round 90: radius read before the center copy 215 and the copied point as the actor-to-point
// input 211 (both kept); the copy as the point-to-center input 212, as both inputs 264; a second
// point copy for both 236 (1029 bytes); center/to-center names in four pairs 211; an early false
// return for the lookup result 213; the two differences in the other order 211; squared and
// projection terms in the other order 211/211.
// Delayed vertical zero 274; with center difference 271; native 2D perpendicular 286/285;
// complete aggregate 277/274; delayed zero and early three-use clearance 220; with center 259.
// Delayed zero and early clearance kept; other executable forms restored.
// Shared perpendicular 265; split squared/root distances 220; both 265; clean output with shared/split/both
// 282/279/282; shared perpendicular with coherent point source 225; with split distances 225.
// Staged second/first/both direction outputs 231/227/262; center-staged second alone/with first
// 240/273; copied-point staged second/both/center pair 231/262/276. Opening directions restored.
// Expanded radius and existing point-setter first/second/both combinations all 220;
// native squared-vector helper 273. Original point construction restored.
// Round 204: shared far-output store 252; native collision-test result 282; both 284;
// later adjusted-point copy 267; with shared far store/collision result/both 272/283/285;
// all with clean output access 301. Original output, spill and point lifetime restored.
// Output EBX/EBP, point/vector homes and SSE scheduling remain.
// No output self-address access 295; copied input/native object/both 281/295/281;
// shared collision storage alone/with input/object/both 295/282/295/282;
// copied input with entry/after-actor common result 287/287.
// frame 0x90 agrees; output EBX/EBP, vector homes and SSE scheduling differ.
// Vector/center/point/direction names 277 each; named clearance 277; shared actor-to-object difference
// 274; center memcpy 277; names/clearance/difference 274. Three-point gains rejected.
// Round 147: Planar scalar differences 255; complete projection products 220; both side-product
// sets 220; all squared terms 220; projection/side 220; planar/projection/side 255; all products
// 220; all products with planar differences 255.
// Round 150: Coherent first/second/both point inputs 215/217/264; second producer first with
// original/first/second/both 220/216/217/270; early uninitialized directions with both inputs 264.
// Inline collision provider on original/first/second/both inputs 220/215/217/264, identical with an
// explicit out-of-line declaration. Best five-point gain rejected; original body restored.
// Partial original-point input forms are exact-match-only; all trial bodies are restored.
// Round 174: center/radius/both per-use sources 291/242/296; complete actor-position snapshot
// alone/with center/radius/both 267/283/262/246; complete sources with clean output access 296.
// Original observations, early clearance, delayed zero, output access and scalar spill restored.
// Direction declaration/call ordering and collision-storage lifetime combinations 220 in seven forms;
// those together with radius before center copy 214 (six-point gain restored).
// Same-size point/vector name swaps, separately and in combinations, 220 in eight forms.
// Far planar separation through the existing 2D/3D helpers 228/228; complete
// native 2D/3D initializers 220/220; each with entry collision storage 228/220/228/220.
// All restored; original scalar geometry and collision-storage lifetime remain.
// Round 191: Direct-output mode first with original returns, Boolean/long/byte
// common completion results 261/261/236/242; all four with radius before center
// 261/261/236/242. Original mode/result graph and executable body restored.
// Round 345: Native original/center/both CRT copies 220/220/220; entry point
// storage alone/with original/center/both copies 218/218/218/218; both copies
// and entry storage with radius before center 213 (seven-point gain below gate). All restored.
// Round 194: original-mode common bool/byte/long completion 248/274/262;
// native query-test completion 274; Boolean/query-test forms with radius first
// 243/274; common Boolean with entry points 248; query-test completion
// with entry points and radius first 274. Original result graph restored.
// Round 254: entry collision-sentinel initialization with original/first-point/center/both
// entry storage 269/269/269/269; with radius-before-center 269/269/269/269.
// Original late collision initialization, point storage and executable body restored.
// Round 195: Normalizer inline/noinline/combined/forceinline declarations 220 each;
// with radius before center 215 each. Five-point best restored; original output access,
// scalar spill, radius lifetime and body retained, output/vector homes unresolved.
// Round 23: direct const/nonconst parameter reads 279 each; separate output pointer 279;
// nonconst output reference and actor-first order 220 each; far/near/both output point
// setters 226/262/262; actor-first with near setter 262. Original body restored.
// Round 259: Collision-branch-only completion results false-first bool/byte/short/long
// 269/247/275/276; true-first 262/258/258/260. Direct-output exits unchanged.
// Original result graph and entire body restored; output EBP and vector homes remain.
// Round 240: Complete final offset through the existing shared scaler 258; with
// radius first/copied point inputs/both 258/259/259; entry point storage alone/
// with radius/with copied inputs/all 256/256/259/259. Original body restored.
// Round 249: Native object receiver retaining output access with original/early radius
// and aggregate/CRT center copies 220/215/220/215; all four with read-only actor
// 220/215/220/215. Five-point radius lead below gate; original body restored.
// Round 251: Multi-use actor-position receiver with original/entry point storage and
// radius-order combinations 299/299/299/299; consistent current actor-position lookups
// with those combinations 307/307/306/306. Original receiver and full body restored.
// Round 253: successive squared sums/projection/both side sums/both squared-projection/
// squared-side/projection-side/all/all plus radius first 264/220/220/264/264/220/264/264.
// Original complete expressions and executable body restored.
// Round 256: shared final scaled add/far sum/planar center-then-displace subsets
// 220 each; full producers with radius first 215 (five-point form restored).
// Output EBX/EBP and point/vector homes remain; frame 0x90 agrees.
// Round 258: Native planar initial first/second/both directions 220/225/225; with radius first
// 215/220/220; both with entry point storage 223, with radius 218. Five-point best restored;
// output EBP and original 0x90 frame/direction homes remain.
// Round 260: distinct first/second/both perpendicular storage declared at entry or geometry
// scope 220 each; both at entry with radius first 215; all direction/perpendicular storage
// at entry 220. Five-point radius gain below gate; original declarations and body restored.
// Round 263: existing state carrier conditional/branch-owned initialization, crossed with
// entry point storage and early radius, 266/266/264/264/260/260/258/258. Original stores, homes and body restored.
// Round 265: clean output with bool/long common completion, entry-point storage and
// radius-order combinations 279/279/279/279/285/285/285/285. Original complete body restored.
// Round 268: plain component loops far/adjusted/both with long ordinals 220/220/220;
// short ordinals 246/286/284; both byte/dword 284/220. Original scalar stores restored;
// output EBX/EBP, vector homes and return tails remain.
// Round 271: full collision output with the shared native head as member/base, late/entry
// storage, 220 each; all four with radius before center 215 each (five-point gain restored).
// Original opaque full output, output access, scalar spill and complete body restored.
// Round 273: uninitialized squared-separation/projection/adjusted-distance declarations
// at entry, seven subsets 220 each; all with the clearance declaration at entry 220.
// Original scalar declarations restored; output EBX/EBP and vector homes remain.
// Round 269: displacement workspace 260; separation workspace 211; both 260; direction-scale workspace 251;
// displacement/scale 207; separation/scale 251; all 207; all with entry point storage 207.
// Four-point best below the retention margin; original executable body restored.
// Round 430: Native parameter-role names, all seven index/point-output/optional-result groups, then
// all with collision storage/result names: eight forms 211 at 1004/1025. Original identifiers and
// entire executable body restored.
// J198: existing complete point/word getter, crossed with entry point/collision storage
// and coherent input, 296/296/296/296/300/300/300/300. Scalar output cast exact-only; all nonmatching forms restored.
// Output EBX/EBP, vector homes and geometric schedules remain.
// Round 302: Clearance after successful normalization, with shared expanded radius,
// distinct squared/root length and existing scaled-add subsets: all eight 270.
// Original clearance lifetime, output access and body restored; output/vector homes remain.
// Round 29 (fast): a second copy of the input point, after the center, with the to-center vector
// computed in place over it (as retail's stores show) 235, as local_6 only 229, taken first 239; with
// an actor-to-center vector computed before the first test and used in both later tests 226; on that,
// the squared length written i-then-j 222, the 3D squared length i, j, k 219, both 215 (retail's 0x28
// copy / 0x34 center layout and vector sources; output still EBX, 1032 bytes); product operand and
// sum orders in the two dot tests 215-226 (canonicalised); the arg_5 path as an else with one final
// false return 215 (same code); without the output self-address read 238.
// Round 46: retail's 0x28 slot is one variable: the point copy is stored there and the
// point-to-center vector is written over it component by component (copy y stored after the
// vector's i). A to-center vector copied from the point and differenced in place puts the
// center at retail's 0x34 and aligns the middle block, but scores 235; with a named
// actor-to-center vector after/first/between the point vectors 232/226/226 (sum and product
// operand orders then mirror retail's); separate origin copy for both vectors 229. Restored.
// Round 4 (lane 18, fast): the center local renamed to sort after the to-center vector 211 (slots
// unchanged: center stays at 0x28); both vectors declared ahead of the point copy 211; the arg_5 path
// as an else with one final false return 211; the point copy renamed to sort last 211; the point copied
// after the center and radius 224 (1006 bytes). Restored.
// Round 37 (lane 1): the dead pathfinding-data store, the record with its NONE word and the direct
// 0x26d100 call are 0x26d0e0 inlined with 0x1fa7f0's result as its third argument (the form matched
// 0x1b13b0 and 0x25b6b0 call out of line); written as that call 298 plain (975 bytes: our LTCG inlines
// 0x26d0e0 but drops the third argument's [esp+0xc] store, and this function's convention changes to
// arg_0 on the stack and ret 0x10, so matched callers 0x1ba5c0/0x1bdad0/0x1bff80 stop matching). Fast:
// a result local set true before the out-pointer test 278 (the same convention change); the center and
// radius through s_slot_object_view's unknown030/unknown03c 211 (same code); `2.0f - distance` inside
// the normalize-success block, where retail computes it, 270.
// Round 247: complete input snapshot across both real modes, with native/library copy
// and phase/entry collision storage, 255/255/255/255; the same four without output
// self-address observation 250/250/250/250. All disturb two matched callers in the
// reduced image. Original input observations, output access, scalar spill and body restored.
// Round 110 (lane 8, fast): `2.0f - distance` inside the normalize-success block with the center copied
// before the radius 269 (992 bytes); the to-center vector initialised from the input point and differenced
// in place through vector3d_from_points3d 229 (1027 bytes, aligned 136 against 123), with `2.0f - distance`
// in the block 242 (1015); the existing 0x30c60 for center and radius 296 (980 bytes: stays a call from its
// /Ob1 file, so the head changes); no output self-address read with `2.0f - distance` in the block 287.
// Round 258: Complete center-relative planar/projection-numerator/both-side-sum expression
// subsets 1..7, then all with an independent adjusted root: 255/211/255/211/255/211/255/255. Original
// expressions and body restored; output EBP and point/vector/SSE homes remain.
// Round 112 (lane 8, fast): a __forceinline declaration of the existing 0x30c60 for centre and radius 296
// (980 bytes, still a call; three matched callers change convention); `inline` on 0x30c60's definition plus
// the call 215 (1000 bytes, inlined, its two matched callers keep matching, but the radius arrives as an
// integer copy through its `long *` and the output stays EBX); that with the to-centre vector initialised
// from the input point and differenced in place 233 (1023), with `2.0f - distance` in the normalize block
// 274, with the centre/radius call before the input copy 220. Retail's inlined radius is a movss load.
// Round 294: complete adjusted-point xy/yx producers 211/211; complete displaced-
// point xyz/zyx producers 211/211; all four paired producer schedules 211 each.
// Original scalar expressions, publication and complete body restored; output/vector homes remain.
// Round 238: plain second-perpendicular two-component sign traversal, long/short/dword/word
// ordinals, 211/217/211/217; with the existing shared scaled-add output, 211/217/211/217.
// Original scalar signs, offset producer, include list and complete body restored.
// Round 51 (lane 11, fast): the found-flag clear before the actor lookup 261, without the output
// self-address read 275 (both change the convention: 0x1ba5c0/0x1bdad0/0x1bff80 stop matching); the point
// copy declared at function scope 211 (same code); the output self-address read after the actor lookup 211
// (same code); the object lookup before the point copy 211 (aligned 170 -> 200); the radius after the
// centre copy 215. The inline route to 0x30c60 needs its definition changed (unknown_030290.cpp, held by
// another lane this round).
// Round 445: Native double squared-distance/projection/radius-clearance domains, seven
// subsets and complete far-mode scalar graph: 279/262/265/293/261/258/274/274.
// Original complete executable body restored.
// Round 294: complete adjusted-point xy/yx producers 211/211; complete displaced-
// point xyz/zyx producers 211/211; all four paired producer schedules 211 each.
// Original scalar expressions, publication and complete body restored; output/vector homes remain.
// Round 341: Conditional projection admission with shared center parts, separate adjusted
// root and entry point declaration, masks 0-7: 211/255/211/255/211/255/211/255.
// Output EBX/EBP and point/vector homes remain; original complete body restored.
// Round 449: Complete projected-interval/nearby-length/far-separation admission results, bool subsets 1..7
// 211/211/211/211/211/211/211; all with native long results 211 (fast control 211, full 211).
// Original complete executable source restored; opening structural differences remain.
// Round 451: Existing state-spill declaration at entry/after actor/near entry/after input,
// with captured/direct global receiver: 211/211/211/211/211/211/211/211. Original declaration, receiver, stores and full body restored;
// output EBP and point/vector homes remain.
// Round 463: optional adjusted result at the geometry join, positive/complementary
// bool/byte/short/long carriers 236/236/238/237/236/236/235/236; exact-match-only flags.
// Original direct optional stores and complete executable body restored.
// Round 120 (fast with the callees and the three matched callers, all still matching): `return false`
// moved to the end under an if/else 211 (same code); a single-exit bool result (false by default, set
// true in the found arm and at the end of the far path) 249 (1005 bytes: the output pointer then stays in
// EBP and the false return is the last block, as retail; LCS-aligned 123 -> 107); with the result set
// true before the out-pointer test 248 (1011 bytes, aligned 100: retail's `mov al, 1` ahead of that test
// and its shared false epilogue); on that, a named second copy of the input point for the to-center
// difference 240 (1034 bytes, aligned 114); the to-center vector initialised from the input and
// differenced in place 240 (1034 bytes, aligned 115). Lead: the single-exit result plus a fix for the
// center/vector scheduling (retail copies the input a second time into +0x28 and differences in place).
// DL237: Existing complete planar setters at first/second perpendicular construction and
// sign-selection boundaries, masks 1/2/3/4/8/12/15/5: 262/226/296/249/247/236/323/278. All
// restored; output EBP and point/perpendicular homes remain.
// Round 465: ordered native squared-length fold, long/short/signed-byte/int
// ordinals, original/joined optional-index guard: 267/237/237/267/284/236/236/284.
// Joined guards change the convention and lose three caller matches. Original sum,
// guard and complete body restored; output EBP and point/vector homes remain.
// Round 518: common completion initialized from direct-output mode, entry
// bool/byte/short/long results 270/250/256/251; after actor lookup
// 270/254/225/229. Original body restored; caller conventions regress, and
// output EBX/EBP, native point/vector homes and shared false exit remain.
// Round 473: One optional-output admission predicate across all three guards,
// bool/byte/short/long at entry or after actor lookup: 253/253/290/287/249/249/262/259.
// All restored; output EBP and point/vector homes remain.
// Round 521: native input/center/both memmove producers 255/275/259; with entry
// point reservations 255/275/259; both with native object fields, original/entry
// points 259/259. All change caller conventions; original complete body restored.
// Round 476: existing complete collision helper with common query-test result,
// coherent-input/native-object/entry-point subsets: 303/298/303/298/303/298/303/298.
// All lose three caller matches; original collision tail, observations and body restored.
// Round 478: existing shared 2D producer with first/second/both initial planar
// vectors, original/entry storage: 211/216/216/211/216/216; both with coherent
// captured input, original/entry storage, 265/265. Second-vector planar storage
// shrinks frame 0x90 to 0x8c. Original full vectors, observations and body restored.
// P280: Standalone existing scalar getter feeding the inherited state store,
// with every entry scalar/collision/adjusted-point declaration subset: eight forms
// 221. Original getter expansion, scalar stores and full body restored; output/vector homes remain.
// Round 64 (m3 lane 1, fast, plain confirmed): retail stores the input's z at +0x30 but never subtracts it there, so the
// point-to-center difference starts as a copy of the input and is differenced in place (kept), with one result for both
// exits (kept) and the actor difference from the input (kept): 180; the entry self-address read of the output is then not
// needed (removed, 180). On the way: single result alone 249; with a one-use input copy 182; copy for both differences
// 234; in-place difference written out per component 231; in place without the single result 229; actor difference from
// the copy 233; in-place difference after the actor one 181/180; actor difference from the copy after 228. On 180:
// squared terms swapped 180; projection terms swapped 180; actor difference declared after the copy 226; plain local_15
// (not volatile) 267; input copy after the object reads 238.
// Round 80 (m3 lane 12, fast with 0x1ba5c0/0x1bdad0/0x1bff80, which stay matched): the actor-to-point vector
// from `arg_2` instead of the copy 215 (1001 bytes); the found-flag store before the far-side vector 211; the
// clearance `2.0f - local_11` named before the perpendicular 266 (996 bytes); the radius times 1.1 named once
// for both components 211; the first perpendicular's y stored before x 211.
// Lane 12 r86 (fast with 0x1ba5c0/0x1bdad0/0x1bff80, all still matching): retail forms the
// center-minus-actor x/y early, before the squared-length test; an actor-to-center vector through
// vector3d_from_points3d used in both projections 232 (1028 bytes); the clearance computed after
// normalize2d 245 (1013 bytes); `result = true` before the out-pointer store 241 (1031 bytes);
// local_11's squares written i, j, k 217; the actor-to-point vector from local_1 182.
// M3 lane 2 r47 (fast, on 180): the to-center copy declared before the actor-difference call 226 (1027 bytes); the copy
// taken right after the input copy, before the object lookup, 232; `2.0f - local_11` inside the normalize-success
// block 245 (1013 bytes: that block then is retail's code, sqrt stored and re-read after the call; aligned 121 -> 119);
// the actor difference from local_1 instead of arg_2 182 (1028), with the copy declared first 182.
// DH319: Current in-place-copy graph, center-difference vector with original,
// early-success, query-test and complete query arms 232/241/255/253;
// uniform scalar terms 238/245/253/249. All restored; four query-result forms
// change entry convention and lose callers; point/SSE and success ordering remain.
// M360: Existing unit-scale component-array differences, seven direction subsets
// and all with complete input copy publication: 180/180/180/180/180/180/180/180. Original body and
// includes restored; early object masking, input-copy/SSE and query-success schedules remain.
// P311: Existing planar difference over full vector/point storage with native/library captures,
// P311: each with full/planar actor-direction production: eight forms 180 at exact 1025 bytes.
// P311: Original full helper and body restored; object-mask/copy/SSE scheduling and query-success ordering remain.
// Y574: Actual second in-place input supplies both differences, vector/point backing and
// direct/in-place actor output, each with original/late clearance:
// 233/257/233/257/182/245/182/245. NOT kept: all regress or miss byte equality; native early
// object-mask and copy/SSE/query-success schedules remain. Original complete body and helpers
// restored.
// AC517: Scalar lifetime role names for radius, planar length/projection and adjusted distance/clearance,
// phase subsets 1..7 then complete far-separation/state names: eight forms 180 at exact 1025 bytes.
// Original names and full body restored; early object masking and copied-input/SSE schedules remain.
// F368: Actual first/second/both normalizer completion as real, then bool, and entry-owned real/bool pairs: 180 each at 1025 bytes. NOT kept: one-use completion captures do not match or recover early mask/copy/SSE scheduling. Original body and inherited local qualifier restored.
// P315: Full 3D in-place graph, third-input memcpy/memmove/vector-setter/point-setter producers, without/with center memcpy: 180/272/248/248/180/272/248/248.
// P315: NOT kept: no byte match or supported gain; original full body restored.
// P320: Existing object-header producer, raw/const/native/const-native views 180 each;
// P320: header-derived object before input copy, raw/native 185/185; native object/header
// P320: receivers at both radius/center uses 180/180. Original complete body restored;
// P320: early object mask, second-copy SIMD schedule and query success transport remain.
// P330: Optional collision-index output presence/complement at the successful-query
// boundary, bool/byte/short/long: 180/180/180/180/180/180/180/180.
// NOT kept: one-use admission does not match; original query/result and full body restored.
// AC529: Second in-place copy from the adjusted snapshot, vector/point backing, original/captured actor input,
// before/after actor production: 180/182/180/182/226/182/226/182. NOT kept: no equality or gain; native mask/copy/SSE schedules remain.
// F373: Explicit object ordinal at branch/entry/after actor, parameter then long
// ownership, and branch/entry dword: 180/193/193/180/193/193/180/193.
// NOT kept: no match; early forms grow to 1029 bytes. Original body restored;
// early mask, copied-input/SSE and query-success schedules remain.
// F375: Actual direct/adjusted mode selector at its byte observation, bool/byte/short/long:
// 180/180/180/180/180/180/180/180. NOT kept: no byte match; original body restored.
// Early object mask, copied-input/SSE and query-success schedules remain.
// Round 63 (m3 lane 14, fast, plain confirms 47 with 0x1ba5c0/0x1bdad0/0x1bff80 matching). Match-only recipe at 47 (1025 B, aligned 13), not
// kept because it writes the difference helpers out: an actor-to-center vector through vector3d_from_points3d right after the
// center copy, used through dot_product2d at both projections; the second input copy, then the actor difference from it as direct
// i and j rows only (no k); center minus copy in place through the helper; one inline 2-D perpendicular helper and one negate
// helper for both perpendiculars, so their i and j have no direct reference before the scale; `local_12.k = 0` direct, which
// is what gives retail's load-first i and j rows with `mulss t, [k]` last; `local_4 *= 1.1f` for the radius; `2.0f - local_11`
// inside the normalize-success block; local_10 as direct i, j, k rows; dot_product2d(&local_10, &local_12) for the second
// dot; `result = true` before the output store. Left: the opening's copy registers (retail: `and edx` second, center pointer
// `lea edx`, copy x read early in EAX, center temps in EBX, copy z before y, ECX never reused), the first dot's operands, the
// second dot's term order, ECX for the block address with the up global read ahead of the first push, and EAX/EDX in the else
// arm's point copy. Steps: named vector first 223 (1036 B, aligned 50); with clearance in the block 212, result first 231, both
// 217; radius named 217, `local_4 *=` 219 (aligned 28); local_10 rows ijk 214 (1029 B, aligned 21), kji 218, jki 218, kij 219;
// dot_product2d at both projections 193 (1026 B), with the squared length too 189; actor difference rows 69 (1023 B, aligned
// 17); dot for local_12 65; perpendicular and negate helpers 47 (only local_12's are needed; local_9's alone 65).
// From 47: helpers for both differences 206, actor rows with a local_10 helper 194, actor helper with local_10 rows 195,
// vector2d_from_points2d for the actor difference 195; one variable for both perpendiculars 194 (1020 B), for both differences
// 51, both 69; projections direct 208, center vector as rows 213, as j then i 209-213; interleaved rows 204-242; opening
// orders 69-258 (on 69); a named center pointer 51-65 (aligned 12 at best), center by fields 243; first dot arguments
// swapped 47; second dot arguments swapped 47; one variable for block count and value 51; `void *volatile` 47; the block
// global re-read 51; dead `&local_15` instead of volatile 49, one-element array or one-member struct 151 (store removed);
// up global in a local 68 (1021 B; reads it ahead of the push); point copy by `*(point3f *)arg_3` 47, by a pointer local 51;
// output index before the copy 102 (callers lose 5, 6, 11). Permuter (600 s on 47): 63 tries, none below 47. Without
// written-out helpers no subset gains: clearance, result, radius, local_12 helpers, local_9 helpers in all 31 combinations 180-245.
// AC543: Genuine point-pair/direction-trio/perpendicular-pair role-name groups on the
// recovered in-place input graph, subsets 1..7 and complete collision/receiver names:
// eight at 180, 1025 bytes. NOT kept: no equality; early mask/copy/SSE and query-success
// schedules remain. Original identifiers, inherited local qualifier and full body restored.
// F382: Direct-mode point2f/vector2f separation, existing/explicit difference
// and existing/direct magnitude: 180/188/180/180/180/188/180/180. Frame 0x90 and extent 1025 agree.
// NOT kept: ties/regressions; original scalar separation, includes and whole body restored.
// F384: Independent actor-direction in-place point/vector backing, native/memcpy
// capture at its producer or before object acquisition: 259/259/259/259/263/263/263/263.
// NOT kept: all regress and grow to 1050/1025; original full body restored. Native
// early object mask, independent copy/SSE and query-success schedules remain.
// B439: First/second perpendicular sign outcomes in bool/byte/short/long:
// 180/180/180/180/180/180/180/180. NOT kept: one-use predicates do not match;
// original body restored, early mask and copied-input/SSE/query homes remain.
// P332: Initial adjusted-point CRT capture on the full in-place graph,
// with second-input/center library-copy subsets: memcpy four 180s; memmove
// four 245s (1029 bytes, all three caller conventions lost). NOT kept:
// no gain; original captures, helper inputs and complete body restored.
// DH370: Coordinated object-index formal int/dword/unsigned-int/short/word/
// const-long/const-short/const-word: 180/180/180/203/203/180/203/203.
// NOT kept: full-width contracts tie; 16-bit contracts grow and lose three callers.
// Original long contract and every declaration restored; mask/copy/SSE schedules remain.
// Round 130 (m3 lane 7, fast; 0x1ba5c0/0x1bdad0/0x1bff80/0x30c60/0x24d8d3/0x24da7d matching each time). New causes, match-only:
// retail's center and radius are the existing 0x30c60 inlined (`lea edx` center pointer, radius as a movss): `inline` on its
// definition, its radius read as a real and lane 14 r63's recipe (see its report) 47 -> 19 (1025 B, aligned 10; through its `long *` 80, 1021 B).
// A helper accumulating the third dot (`real result = a->j * b->j; result += a->i * b->i;`) 15; written j first, swapped,
// direct, named 19/19/44/25. BLOCKED-BY 0x26d100: its `&point` read makes this function's point copy escape, so the copy's
// stores are ordered against later pointer loads, which is the whole opening schedule (`and edx`, y store, x read). With
// 0x26d100 as (up, point, output, collision), reading `point` directly, start before direction and result first (round 61's
// form in src/unknown_1c20f0.cpp's note): this function 4 (fast). That reorder alone, plain: 0x26d100 and 0x1c19b0 match, no listed caller moves. Left
// at 4: the first dot's operands. Direct rows interleaved (c.i, 5.i, c.j, 5.j, which is the order the operand ranks imply)
// fix them but the second dot's i product takes the perpendicular's temporary first, 177 (1028 B); with a dead `(void)local_9.x`
// after the getter that too is retail's: MATCH in a plain check (0x3d7f0 177 -> 224 from the `inline`: retail calls 0x30c60 there; a
// redundant statement, not keepable, so scratch only; the statement must precede the c.i
// row: after it or after all rows 177, `.y` 177; through a scalar declared early 177). Without the callee's reorder the same
// form 11, opening only. On the way: interleaved orders 187-191, in-place rows 68-70; both differences through helpers 174-197;
// through one interleaving helper, 24 row orders, 172-208, with a directly stored perpendicular 161-197; 2-D helpers for the
// center and in-place differences 19 (same code); center rows before the copy 198 (1031 B); named center scalars 190-198;
// center difference unnamed at both dots 229 (1022 B); block-top declaration orders 174-197; early masked index 25; copy helper
// 15, memcpy 15, pointer local 21, scalar copies 245/256, statement orders 51-85. No written-out rows (existing helpers only)
// 177 at best (1031 B). Identity pointer chains of 1..7 locals 25/187 each. The rows and helpers stay match-only: body restored.
// DL273: Shared planar cross products at both side tests and dot projection, seven
// boundary subsets then squared dot: 248/184/249/248/231/249/231/231. NOT kept:
// no byte match; native object-mask/copy/SSE and query-success schedules remain.
// Round 133 (m3 lane 7, fast): `result = true` before the output store gives retail's whole tail (`mov ecx, eax`, `mov al, 1`
// ahead of the output test, `xor al, al` before the pops; aligned 163 -> 154) but 1031 bytes, 241 (one jump grows: the
// opening is longer than retail's); on it the far path's branches swapped 224 (1029), the far path's output index stored
// once after its if/else 238 (aligned 160); direct returns in place of the result 251 (1016). The far path's copy still
// takes EAX for data where retail copies EBP into EAX. Body restored at 180; the opening stays BLOCKED-BY 0x26d100 (its
// files are held this round).
// Round 66 (m3 lane 15, fast, plain confirmed; 0x1ba5c0/0x1bdad0/0x1bff80, 0x26d100, 0x1c19b0, 0x30c60 matched throughout). The 0x26d100 parameter reorder is on
// main now, so that blocker is gone. NOT kept, needs a ruling on its three order changes (the clearance moved into its use block, `result = true` ahead of the
// output store, the centre copied before the radius; without the first two it is above 180): existing helpers only, 180 -> 132 (1025 bytes, plain and a full check
// with nothing lost; saved as build/r66/1badc0-132-form.patch): a named actor-to-centre vector through
// vector3d_from_points3d used through dot_product2d at both projections, both point differences from the function's own copy of the point (read once), the centre copied
// before the radius (the order of 0x30c60's body), `2.0f - local_11` inside the normalize-success block where retail computes it, and `result = true` ahead of the
// output store. Steps: vector + dots 233; clearance in the block 245; radius `*=` 185; result first 241; vector + clearance 221; those + result first 153; + copy
// source 139; + centre first 132. Leave one out on 132: no clearance move 243, no result first 209, difference from arg_2 143, radius first 139. On 139 or 132:
// vector declared after the point difference 139 (same code), after the in-place difference 231-240; the point difference from the second copy 203 (1027); the second
// copy from the first 139 (same code, so the parameter is read once); `local_4 *= 1.1f` 146 / 140; a named `local_4 * 1.1f` 132 (same code); local_10's squares i, j, k 231; local_1 copied after
// the object lookup 153; second dot or the squared length through dot_product2d 132 / 132 (same code); `local_12.k = 0` ahead of its rows 234. Without the centre swap 139; without it and without result first 210; the result written directly
// (`result = local_17 != NONE`, four spellings) 266 / 274 / 266 / 272 (997-1006 bytes; 0x1ba5c0 52, 0x1bdad0 26, 0x1bff80 115: the convention changes).
// The lane 7 r130 recipe re-measured on today's tree: byte match in a plain check with every listed caller and callee matched, NOT kept: it needs `inline` on
// 0x30c60's definition, which takes 0x3d7f0 from 177 to 224 (BLOCKED-BY 0x3d7f0: it inlines 0x30c60 where retail calls it; its file is another lane's this round),
// and the dead `(void)local_9.x`. `inline` or `__forceinline` only on this file's declaration of 0x30c60 287 (988 bytes, still a call, three callers lost). The
// mention only turns the second dot's i product from (perpendicular x, centre x) into (centre x, perpendicular x) and its sum to j term first; without it 177 (1028
// bytes). Replacements for it on the recipe, none a match: perpendicular helper with a local or stores swapped 175 / 175; second dot's arguments swapped 177; second
// dot through the accumulating helper either way 177 / 177; the perpendicular as a vector2f or vector3f 177 / 177; its address bound to an empty inline function's
// parameter 177; the mention ahead of the getter 20 (1024); second dot through a scalar-parameter helper in three spellings 177 each; written out in three operand
// orders 177 each; one planar vector for the actor-to-point difference rotated in place to the perpendicular 187 / 185 / 187 (1031 / 1028 / 1031). Recipe saved as
// build/r66/held-1badc0.patch.
// @retail 0x1badc0
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6)
{
    s_actor_view *local_0 = actor_get(arg_0);
    bool result = false;
    if (arg_6)
        *arg_6 = false;
    if (!arg_5)
    {
        point3f local_1 = *arg_2;
        byte *local_2 = (byte *)object_get(arg_1);
        point3f local_3 = *(point3f *)(local_2 + 0x30);
        real local_4 = *(real *)(local_2 + 0x3c);
        vector3f local_5;
        vector3d_from_points3d(&local_0->position, arg_2, &local_5);
        vector3f local_6 = *(vector3f const *)arg_2;
        vector3d_from_points3d((point3f const *)&local_6, &local_3, &local_6);
        vector3f local_centre;
        vector3d_from_points3d(&local_0->position, &local_3, &local_centre);
        real local_7 = local_5.i * local_5.i + local_5.j * local_5.j;
        if (local_7 > 0.0f)
        {
            real local_8 = dot_product2d((vector2f const *)&local_centre,
                (vector2f const *)&local_5) / local_7;
            if (local_8 > 0.0f && local_8 < 1.2f)
            {
                point2f local_9;
                local_9.x = 0.0f - local_5.j;
                local_9.y = local_5.i;
                if (dot_product2d((vector2f const *)&local_centre,
                    (vector2f const *)&local_9) > 0.0f)
                {
                    local_9.x = 0.0f - local_9.x;
                    local_9.y = 0.0f - local_9.y;
                }
                if (normalize2d(&local_9) > 0.0f)
                {
                    local_1.x = local_9.x * (local_4 * 1.1f) + local_3.x;
                    local_1.y = local_9.y * (local_4 * 1.1f) + local_3.y;
                    vector3f local_10;
                    vector3d_from_points3d(&local_0->position, &local_1, &local_10);
                    real local_11 = local_10.k * local_10.k + local_10.j * local_10.j + local_10.i * local_10.i;
                    if (local_11 > 0.0001f && local_11 < 4.0f)
                    {
                        local_11 = (real)sqrt(local_11);
                        vector3f local_12;
                        local_12.i = 0.0f - local_6.j;
                        local_12.j = local_6.i;
                        if (0.0f > local_10.j * local_6.i + local_12.i * local_10.i)
                        {
                            local_12.i = 0.0f - local_12.i;
                            local_12.j = 0.0f - local_12.j;
                        }
                        local_12.k = 0.0f;
                        real local_13 = 2.0f - local_11;
                        if (normalize2d((point2f *)&local_12) > 0.0f)
                        {
                            local_1.x += local_12.i * local_13;
                            local_1.y += local_12.j * local_13;
                            local_1.z += local_12.k * local_13;
                        }
                    }
                    if (arg_6)
                        *arg_6 = true;
                }
            }
        }
        byte *local_14 = (byte *)g_4e0348;
        volatile long local_15 = 0;
        if (*(long *)(local_14 + 0xc4) > 0)
            local_15 = *(long *)(local_14 + 0xc8);
        s_1badc0 local_16;
        local_16.field_24 = NONE;
        long local_17 = function_26d100(g_4687b0, &local_1, (long *)arg_3, (s_collision_result_1697c0 *)&local_16);
        if (local_17 != NONE)
        {
            if (arg_4)
                *arg_4 = local_17;
            result = true;
        }
    }
    else
    {
        real local_18 = local_0->position.x - arg_2->x;
        real local_19 = local_0->position.y - arg_2->y;
        real local_20 = (real)sqrt(local_19 * local_19 + local_18 * local_18);
        if (local_20 > 2.0f)
        {
            arg_3->point.x = arg_2->x + g_4687b0->i;
            arg_3->point.y = arg_2->y + g_4687b0->j;
            arg_3->point.z = arg_2->z + g_4687b0->k;
            arg_3->output_index = NONE;
            if (arg_6)
                *arg_6 = true;
        }
        else
        {
            arg_3->point = *arg_2;
            arg_3->output_index = NONE;
        }
        result = true;
    }
    return result;
}
