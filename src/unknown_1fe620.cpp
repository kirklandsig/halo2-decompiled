#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_20fe20.h"
#include "normalize3d.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

bool function_1fdce0(long arg_0, short arg_1);
void function_1fe410(point3f *arg_0, real arg_1);

static __forceinline long real_to_long(real value);

// score 373 (was 373): frame 0x4c versus 0x48; early third-input/random-state carriers and scalar homes differ.
// MiniMax I (11-20): no new form, no lower score (373): the frame is 0x4c against retail's 0x48; retail holds the third settings pointer in ESI and the random state in ECX, this build keeps the random state in ESI and reloads the third input from its stack slot; the +0x7a word is passed in ECX against our EDX.
// MiniMax B (11-19): no new form, no lower score (373): reading the random state through the global instead of
// through the named local gives 423 (the same form MiniMax G measured on 11-18), so the body is back as it was. What
// still differs is the 0x4c frame against retail's 0x48, retail holding the third settings pointer in ESI from the
// entry where this build keeps the random-state pointer in ESI and reloads the third input from its stack slot, and
// retail's word at +0x7a passed in ECX where this build passes it in EDX. The argument's transport is set by the
// caller at 0x1fc7e0 and the resolver's copy order belongs to 0x210850, so both are in other files.
// MiniMax S (11-17): no new form, no lower score (373): the frame is 0x4c against retail's 0x48, retail holds the third settings pointer in esi from the entry and the random-state pointer in ecx where this build caches the random state in esi and reloads the third input from its stack slot, the flag byte is stored at +0xf where ours is at +0xb, and retail passes the third input's word at +0x7a in ecx where ours passes it in edx; the argument's transport is set by the caller at 0x1fc7e0, in another file.
// MiniMax G (11-18): no new form, no lower score (373): the frame is still 0x4c against retail's 0x48, this
// build still keeps the random-state pointer in ESI from the first draw where retail holds the third settings
// pointer in ESI and the random state in ECX, so the third input is still reloaded from its stack slot; the
// random-state pointer read through the global instead of through the named local scores 423 and was restored.
// MiniMax D (11-16), at 373: nothing kept: retail's frame is 0x48 and it takes the third settings
// pointer in esi from the entry, this build has a 0x4c frame and takes the random-state pointer in esi instead,
// so the third input is reloaded from its stack slot and the state pointer is kept in a register across the
// draws; retail also forwards the seed from the sign test through the three range draws in eax and stores it
// once, this build stores and reloads it per draw, and retail's sign test is `test eax, 0x80000000` where this
// build tests eax whole. The state pointer read through the global instead of through a named local scores 423
// and was restored; a permutation search over this body (300 s, annealed, one job) found nothing below the
// baseline.
// MiniMax R (11-14, run 65): no new form, no lower score (373): the first difference is at +0x2, the frame, which
// is 0x4c here against retail's 0x48, and the entry differs with it - this build pushes edi and keeps the random
// state in esi from the first draw where retail pushes esi and holds the third settings pointer there - so every
// later home follows from those two. The forms that reproduce retail's frame and its early bound reads need
// one-use bound locals and the resolver written out in two copies, which review rejects, and the permutation
// search over this body found nothing lower; the body stands.
// MiniMax B (11-14): no new form, no lower score (373): the frame is still 0x4c against retail's 0x48, retail keeps the
// third settings pointer in ESI from the entry where this build caches the random state in that slot, and the two flag
// bytes are still stored before the random state is loaded; the forms that write the third input out at each use and
// the ones that move the random state into a named local all compile to the same bytes.
// MiniMax C (11-14): no new form, no lower score (373): retail keeps the pool pointer in the base register
// and forms the actor with lea edi,[edx+esi] (actor EDI, object ESI), while ours loads the pool early
// into EDX and ends with the actor in ESI and the object in EDI, so the frame is 0x4c against retail's
// 0x48; a permutation search over this body (33 tries, 604 s, with the reduced image allowed to disagree
// with the full one) found no variant below the baseline.
// MiniMax A (11-13): no new form, no lower score (373): the timer result written straight into the
// message field and the timer value folded into the conversion both compile to the same bytes; the frame
// is still 0x4c against retail's 0x48, retail keeps the third settings pointer in esi from the entry where
// ours caches the random state there, and the two flag bytes are still stored before the random state is
// loaded.
// MiniMax H (11-18): no new form, no lower score (373): the frame stays 0x4c against retail's 0x48 (both use the
// same total below the entry, retail saving three registers against this build's two) and retail still holds the
// third settings input in ESI from the entry where this build reloads it from its stack slot and keeps the random
// state pointer in ESI instead; dropping the random-globals pointer local and using the global directly gives 423
// (the state pointer then stays live across the vector work), and the reduced image cannot search this function
// (464 differing instructions there against the full image's 373), so the permuter is of no use here; the body stands.
// MiniMax G (11-12): no new form, no lower score (373); the frame is still 0x4c against retail's 0x48, retail keeps the third settings pointer in esi from the entry where ours caches the random state there, and the two flag bytes are still stored before the random state is loaded.
// MiniMax D (11-14): no new form, no lower score (373): retail keeps the third settings pointer in ESI from
// the entry and the random-state pointer in ECX, ours keeps the random state in ESI and reloads the third
// input from the stack; a local for the settings pointer scores 378 and was reverted, and the permuter's 21
// tries over the reduced image (with --allow-mismatch, since the reduced score 464 differs from the full 373)
// found nothing below the baseline.
// MiniMax I (11-17): no new form, no lower score (373): the frame is still 0x4c against retail's 0x48, this build
// still keeps the random state pointer in ESI and reloads the third input from its stack slot where retail holds
// that input in ESI from the entry, and the seed is still stored and reloaded per draw where retail forwards it in
// EAX and stores it once.
// MiniMax E (11-11): no new form, no lower score (373); the frame is still 0x4c against retail's 0x48, retail keeps
// the third settings pointer in esi from the entry where ours caches the random state there, and the two flag bytes
// are still stored before the random state is loaded where retail loads the state pointer first.
// MiniMax R (11-10): no new form, no lower score (373); the two flag bytes are still stored before the random
// state is loaded, where retail loads the state pointer first and then stores the bytes.
// MiniMax R (11-10): no new form, no lower score.
// MiniMax A (11-04): no new form, no lower score; the third input copied into a named local scores 415 and the +0x1c value
// named 437 (both restored), and retail re-reads the third input from its stack slot after the 0x2104b0 call.
// Lane 18 r37 (fast): the +0x84 scale named for its test and product 430; unary negation for the side
// flip 373 (same code); the +0x7c0 initial value as a ternary 430 (1844 B); the target-state pointer
// declared before the prop test 373; the two limits read before +0xc 373. Ours caches the random-state
// pointer in ESI from the first draw, where retail keeps the third input there and the state in ECX.
// Round 28 lead (not kept: needs one-use locals): retail reads the third draw's bounds (+0x14/+0x18) before the
// first draw and keeps them on the x87 stack; with them read into locals ahead of the draws, all three draws through
// local_10 (the random-state local: one seed store after the third draw, as retail) and the lower bounds written as
// call arguments, the third input moves to ESI and the random state to ECX and the frame becomes 0x48 (367, 1843
// bytes, first difference +0x1ab); with +0x3c read at each use and the resolver as an if/else with two copies (as
// 0x210850, which retail inlines here) 342 at 1861 bytes. Without the early bound reads 436 (1870); +0x38 as a local
// changes nothing. Alone on the original body: +0x3c per use 447 (1846), if/else resolver 466 (1889), lower bounds
// as arguments 389, both of the first two 450; early bounds only 424, with +0x38 420, +0x38 only 375; early bounds
// with slot_random_range for draws 1/3, 1, 3: 438/418/441; draws through local_10 without early bounds 438 (1856),
// draws 1/3 only with early bounds 441; the 0x210850 call (not inlined by our LTCG, also with an inline or
// __forceinline declaration here) 455/470.
// Retail runs the three range draws on a seed held in EAX with one store at the end; ours reloads and stores it
// per draw. Round 26: the hand-written normalization replaced by normalize3d 373 (kept); the up-vector cross
// product through cross3f with a const {0, 0, 1} 378, with normalize3d 378; magnitude3d for the displacement
// length 385, with normalize3d 388; the timed scale through vector3f_scale 373.
// Round 74 (baseline 378 after the round-73 real cast in 0x259d0): the timer draw scaled in its own
// statement (`timer_value *= ticks`) 373 again, the output copy block back in step (under the keep bar of 8; restored). At 373 or worse:
// resolver as an if/else with two copies (retail's inlined 0x210850 shape) 466; sign draw through the
// global at each access 417; both 461; random state named once at the top 417; final sums with the
// operands swapped 373, through the vector field 373, from the vector local 429; timer conversion
// as an initialized declaration 373; second draw's sum swapped 373.
// Earlier: frame 0x4c versus 0x48; early third-input/random-state carriers and scalar homes differ.
// frame 0x4c versus 0x48; third input and random-state register lifetimes differ.
// point resolver 445; scale snapshot 430; spread snapshot 437; later second trig pair 379; doubled bound 431; local output sums 429; force-inline resolver 466.
// frame 0x4c versus 0x48 and early settings/seed register allocation remain; native
// initializer forms tie and scale loops regress; tried first native output vector 373; second native output
// vector 373; native side scale loop 382; native timed scale loop 382; native side vector 373; both native
// output vectors 373; native side first output 373; all native motion vectors 373.
// first cosine/sine/pair and second cosine/sine/pair through real trig APIs 378;
// both real trig pairs 373; native side-negation loop 382; all restored.
// frame 0x4c versus 0x48 and settings/random-state homes still differ.
// Immutable third input/actor/state/seed 373 each; signed seed 417;
// immutable bounds/trig results/point receiver 373 each.
// Shared first/second/both planar zero products 373/373/373; shared vertical
// zero product and all repeated zero products 373/373; shared normalization
// boundaries 378/378; shared displacement magnitude 388 (fast control 373).
// Frame 0x4c and early third-input/random-state allocation remain; all restored.
// frame 0x4c versus 0x48 and early third-input/random-state homes remain. Fast
// trials: published magnitude reads 430; timer field reads 378; published magnitude and timer reads
// 430; first cap scope 373; second cap scope 373; both cap scopes 373; full signed timer 427;
// published magnitude with full timer 434. All restored.
// Published magnitude reads recover frame 0x48 at fast 430/1869, but early input ownership still differs.
// Existing state-copy owner 380; published sign-state test 373; native long adjustment result 408.
// Double first/second/both positive angular bounds 414/412/468; double negated bounds 431; all angular bounds double 466.
// Later second bound / initial speed / both 389/388/390 (frame 0x48); cap before ranges 389.
// Distance before side normalization/cross 427 each; cap after first draw 370 (three-count gain rejected); trig reuse 384.
// Middle cap with later bound/speed/both/trig reuse 390/389/391/381.
// Speed/first limit/second limit/first lower/second lower orders
// 13245/21345/23145/31245/32145 373 each;
// interleaved bounds 12435/13524/24351 374 each.
// Point-owner, complete output-phase and normalization-scratch scopes each 373;
// owner/output, owner/scratch, output/scratch and all three 373; all three with
// initial-conversion scope 373. Frame remains 0x4c; original lifetimes restored.
// Round 194: Initial-test/point-query/optional-adjustment providers, inline
// singly, in every pair and together, then all three noinline: 373 each.
// All separate declarations restored; frame and early input allocation remain.
// Round 184: native actor state-byte/timer-scalar/point-vector regions, all seven
// nonempty combinations, and the genuine adjacent output-vector array 373 each.
// External record layouts restored; raw actor receiver and original values retained.
// Round 226: native point-state value / memcpy with bool, byte, short and long
// adjustment carriers 459, 459, 459, 459, 472, 472, 477, 477. All restored; frame/input ownership remain.
// Round 32: retail holds the third input in ESI from entry (so its stack slot is free for the
// float scratch after its last use: frame 0x48), keeps the random seed in EAX across the sign test
// and the three draws (two stores), and loads the random state into ECX before the 0x1fe410 test.
// Timer draw without the named timer locals 373; flag declared at its block 395; random state
// read before the 0x1fe410 test 428; first/third draws through slot_random_range 395; sign
// test through _random 417. All restored.
// Round 381: Shared angular caps with joined/terminal/conditional/nested results 465/431/431/465;
// force-inline repeats 465/431/431/465. Original independent caps restored; frame/input ownership
// remain.
// Round 170: Common adjusted-timer float/conversion, timer-scope and entry reuse, direct initial conversion,
// integer/store join, direct final conversion, shared entry results and entry scratch
// 450/450/458/450/443/450/458/450. Original three conversions and all arithmetic groups restored.
// Round 244: Existing final vector sum with original/normalization/timed/both real scale boundaries
// 376/378/376/376 at 1854 bytes; final point setter with the same four scale graphs 384 each at 1861 bytes.
// Frame 0x4c and third-input/random-state homes remain; original vector operations and published-field
// observations restored.
// Round 388: Shared sign-state receiver at later range subsets 1-7
// 423/377/439/373/423/400/438; complete receiver graph with native upper-word step 438.
// Original independent receiver observations restored; frame and early input ownership remain.
// DL179: Complete magnitude-square products 373; complete parallel unscaled rotation
// terms 378; both 373; complete cosine products and unscaled terms in all six rows 381;
// squares/final-point helper, terms/helper and both/helper 384/384/384; complete cosine
// products with squares/helper 387. Five-count best fails the retention threshold; restored.
// Round 292: Staged cross and complete first/second output-scale boundaries, all seven subsets
// 378/455/451/428/424/441/437; complete graph with timed scaling 437. Original arithmetic and vector
// operations restored.
// Round 414: Frame 0x4c and third-input/random-state homes remain. Existing vector assignment at
// side/first-output/second-output construction masks 1-7 scored 373/378/373/404/397/404/397; all
// constructions with in-place sign assignment 397. Five-count best is below the eight-count retention
// threshold; original vector stores restored.
// DL190: Side/motion/both native real roots 378/378/378; native absolute magnitude 378; both roots plus absolute magnitude 373 (five-point gain rejected). Both complete double squared-length identities alone/absolute/root APIs 378/378/378. Frame and early input/state ownership remain; all restored.
// DL192: Genuine magnitude/reciprocal/displacement workspace reuse, subsets 1-7: 378/404/404/390/390/408/408;
// complete reuse with fallback point setter 466. Frame and early input/state ownership remain; all restored.
// Round 314: Adjustment distance snapshot with bool/byte/short/long outcomes, without/with motion-
// scale snapshot: 437/433/437/433/448/444/452/448. Short and long recover frame 0x48 but retain wrong
// third-input ownership and regress. Original frame 0x4c body restored at 378.
// Round 121 (m3 lane 5, fast): +0x38 as a named local read with the other limits (used twice) 375
// (xmm7 loaded before the draws as retail); with all three draws through the random-state local 434;
// draws 1/3 through it only 423; the 0x70e test nested 373; the prop flag set by an if 415. Probe on
// round 28's lead (early +0x14/+0x18 locals, lower bounds as arguments, +0x3c per use, the resolver as
// 0x210850's if/else) with the +0x38 local: 342 at 1861; there the sign test as
// `(local_11 & 0x80000000) > 0` gives retail's `test eax, 0x80000000 ... jbe` (341; `!= 0`, `>> 31`,
// `(long) < 0`, `>= 0x80000000` 342/342/347/341). Left on the probe: the cross product's inputs
// colour xmm3/xmm2/xmm1 where retail has xmm1/xmm4/xmm3 (one extra movaps in retail).
// Round 253: Retail-backed blend/actor/entry order with early third-range bounds, coherent angular seed
// owner and per-use cap subsets; scores 373/424/438/350/447/449/449/384.
// Original executable source restored; no qualifying gain.
// The 350 form uses one-use early range captures and did not match; restored under the exact-match gate.
// Round 110 (lane 8, fast): retail's sign test is `test eax, 0x80000000; mov [ecx], eax; jbe`, which
// `if ((local_11 & 0x80000000) > 0)` reproduces exactly: 375 (1863 bytes; the longer test is not yet offset);
// `local_11 >= 0x80000000` 380, `local_11 > 0x7fffffff` 380, `local_11 >> 31` 373 (same code). The early
// bounds form again (first/second limits and the third draw's bounds read before the draws, all three draws
// through `&local_10->unknown0` with function_259d0): 340 (1844 bytes, first difference +0x1ab: the head,
// frame 0x48, third input in ESI and state in ECX are retail's), with the masked `> 0` sign test 337 (1847);
// both use one-use bound locals, so not kept. Past +0x1ab retail has two point copies and the NONE-output
// path laid out at the end: 0x210850 inlined at link time (52 other callers keep it out of line; its body
// written here is not allowed), BLOCKED-BY 0x210850. A store through an unknown pointer added to 0x2104b0
// (diagnostic, restored) changes nothing here. On the early form: the third draw's bounds as one
// `real_bounds` copy with the lower bounds as call arguments 334 (plain; 1856 bytes, first +0x1ab, but
// copied through integer moves where retail loads them with fld, so not retail's form: not kept); with
// the lower bounds as named locals 348, the copy first 348; scalar bounds with lower-bound locals 350.
// All three range draws through `&local_10->unknown0` (function_259d0) without early bounds 436 (1856).
// Round 123 (lane 6, fast): the displacement magnitude stored before the side vector 373; the +0x7c0 zero stored
// before the +0x7bc copy 416 (1852 bytes); the +0x714 copy before the time store 372; the sign test as
// `(long)seed < 0` 417; the magnitude's terms in i/j/k order 373.
// Round 63 (lane 12, fast; an LTCG env link with this file's stand-in caller and the 0x259d0/0x210850/0x1fdce0 files
// is exact): the third draw's bounds read into locals with the limits before the draws, all three draws through
// function_259d0(&local_10->unknown0, ...) with the lower bounds as `0.0f - limit` arguments, and the resolver in
// 0x210850's if/else shape with two copies 266 (1873 bytes, first difference +0x1ad: retail's prologue, ESI third
// input, ECX random state and single seed store all appear; the copy after the failed 0x2104b0 call is load-load-load
// then stores in retail, and the up-cross block's XMM numbering differs). Not kept: the bound locals are one-use and
// the resolver is the helper's body. That form without the resolver change 340 (1844); with the sign test as
// `(local_11 & 0x80000000) > 0` 269 (1876, retail's `test eax, 0x80000000; jbe`); with +0x38 named once for its two
// uses 299; with a function_210850 call (stays out of line) 453 (1799). Env: every order of the cross components 181
// or worse (aligned, 186 base); k * 0 named 192.
// Round 30 (lane 2, fast; sequence-aligned counts in brackets, this body 324): retail forwards the sign test's
// seed store into the three draws and stores the seed once after the third, loading every bound (+0xc, +0x10,
// +0x14/+0x18, +0x1c, +0x38) before the draws; ours reloads and stores the seed per draw. The sign test as
// `_random(...) > 32767` 435 (and/cmp 0x7fff0000, not retail's test/jbe); the third draw's bounds as an early
// real_bounds copy 417 [388]; the second draw through function_259d0 like the other two 413 [298], with +0x38
// as a named local read with the limits (used twice) 412 [291], plus the early real_bounds copy 437 [352].
// Round 230: complete sixteen-real input values, native/library copies with bool/byte/short/long
// adjustment outcomes, scores [493, 493, 493, 493, 493, 493, 491, 491].
// Actual resolver definition inline/forceinline [466, 466], with all users checked; restored.
// Original executable source restored. Frame 0x4c and early third-input/random-state ownership remain.
// DL231: Complete eligibility/true-first adjustment/whole displacement square subsets; all with double
// square: 378/427/427/373/378/427/427/427. Frame 0x4c/0x48 and third-input/random-state carriers remain;
// original executable source restored.
// P262: Uniform side/first-output/second-output component orders ikj/jik/jki/kij/kji
// 437/424/425/443/442; early output workspaces with ikj/jik/kji 437/424/442.
// Original publication, workspace lifetimes, all arithmetic and complete body restored.
// Round 131 (m3 lane 8, fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0): the random-state local declared
// right after the 0x1fe410 test (retail loads 0x4e7408 into ECX there) 373 (same code); the point-record copy through
// local_3 380; the output vector through a pointer local read back for the final sums (retail reads [eax], [eax+4],
// [eax+8]) 376 at 1854 bytes; the sign test as `random_next(...) & 0x8000` 417, as `(short)random_next(...) < 0` 429.
// 0x210850 sits in the /Ob1 src/unknown_20fe20.cpp, but 38 of its 52 retail callers are matched /O2 code that calls it
// out of line, so its flags alone do not explain retail inlining it only here.
// Walk b3D (11-06, base r721, fast 373 unchanged): every statement and block compared with retail 0x1fe620-0x1fed62
// (disasm + align.py), no behaviour or precision difference found. Checked: actor index/0x888 stride, the 0x70e/0x70d/0x70c
// flag flow and the 0x1fdce0 word argument (xor ecx/mov cx, same as ours); the timer draw (upper loaded first, product
// rounded by fstp dword, fistp to a word); +0x7bc/+0x7c0 stores and both > 0 tests; local_0 = (+0x24 < 1 || +0x27 == 0)
// (retail's mov [esp+0xf],0 before jne is the same value, nothing overridden); output_index == NONE (cmp ax,0xffff) and the
// failed-0x2104b0 point copy; 0x1fe410 arguments (esi = point, stack = +0x1c); displacement subtract order, the {0,0,1} cross
// terms (j - k*0, k*0 - i, i*0 - j*0), the k,j,i / i,k,j square-sum orders and the float rounding of both roots; the whole-seed
// sign test and 0 - x negations (fchs only for the second output's k); the three draws, with draw 2 added to draw 1 unrounded
// and stored once; both atan2/tan caps (the second limit doubled on the x87 stack); the timer adjustment (fst keeps the
// tan product unrounded for the compare, as ours does); sin/cos stores, the shared sin*0 terms, 1/count scaling, the 16-byte
// state copy and the final sums (stored vector + point; retail adds in the opposite operand order, same value).
// Remaining differences are only register/frame allocation (0x4c vs 0x48, arg_2 reloaded from the stack instead of kept
// in ESI, random-state pointer in ESI instead of ECX) and the per-draw seed reloads listed above.
// Round 112 (m3 lane 9, fast, register-masked aligned counts after the slash): round 63's composite rebuilt
// (re-measure) 266/76 at 1873 bytes; on it, the +0x3c limit read at each use 342/39 at retail's 1861; plus +0x38
// named before the draws for its two uses 342/36; plus the `(local_11 & 0x80000000) > 0` sign test 341/31; plus
// local_12 (+0xc) declared after the limits (retail loads +0x1c first) 342/32; plus the final sums read back through
// a pointer to the stored offset vector 350/29 at 1852; all four 347/23 at 1855. Left on that: the failed-0x2104b0
// copy (retail loads all three words before storing, a copy through 0x210850's `out` pointer: 0x210850 inlined, which
// its /Ob1 file blocks; a pointer to the whole local would be reverted even with a match), a second copy's order,
// the output rows' order and XMM numbering, and two x87 product orders. Nothing kept (match-only composite).
// Round 244 (m3 lane 3, fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0): the timer draw's product with the
// tick count first 373 (same code); the pi/4 cap as a conditional assignment 373 (same code); the +0x38 rate product
// as `+0x38 * (rate * ticks)` 378; the prop state compared with `(short)1` 373 (same code); the 0x70e flag set from
// the 0x1fdce0 result inside `if (flag)` 456 (1856 bytes). Retail forwards the seed from the sign test through all
// three range draws in EAX and stores it once (so the bound loads are scheduled ahead of the draws, holding +0x14/+0x18
// on the x87 stack); ours stores and reloads it per draw. All restored.
// M3 lane 6 r191 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, control 373): retail's own listing fixes three
// facts for the next round. (1) Before the first range draw it loads +0x10, +0x14, +0x18, +0x1c, +0xc and +0x38 in that order
// (0x1fe93f-0x1fe95e; +0x14/+0x18 stay on the x87 stack) and reads +0x3c at each use, so the six values are locals read
// in that order and the seed is then forwarded through the three draws with one store (0x1fe9f2). (2) The third input is
// in ESI only from the entry to the +0x1c test (0x1fe775): ESI then holds the prop pool, then the +0x744 point for the
// inlined 0x210850, and the 0x1fe410 argument re-reads the input from its stack slot (0x1fe807). (3) The random state is
// never kept in a saved register: ECX is loaded at 0x1fe7f6 and again at 0x1fe810 after the 0x1fe410 call. Forms that
// are acceptable without a match, all worse: the limits in retail's load order with +0x38 named for its two uses 375;
// that with the sign test as `(seed & 0x80000000) > 0` 375 (1862 bytes); that with draws one and three through the
// random-state local 418; +0x3c read at each use 447 (1846 bytes); that with the ordered limits and the named +0x38 448
// (1845 bytes). The +0x14/+0x18 locals are one-use and the resolver is 0x210850's body, so the composite of rounds
// 63/110/112 still needs an exact match, and 0x210850 is inlined only here: BLOCKED-BY 0x210850.
// M3 lane 9 r138 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0; base 373): `inline` on 0x210850's definition
// with a call here 466 (1889 bytes), and 25 of its 43 matched callers expand it too (diagnostic); its file built
// without /Ob1 (diagnostic) 466 with the same callers lost: neither gives retail's single expanded site (retail's
// other 52 sites call it under a test or in an arm, this one is unconditional). Round 112's composite re-measured 347
// at 1855 bytes (aligned 127, register-masked 16); on it: the resolver as a forced-inline file-local function taking
// the output pointer 347 (same code, the failed-lookup copy stays load/store interleaved); 1-7 dead initialised
// locals (diagnostic) masked 18/18/16/16/16/16/16; the side vector through cross3f or cross3f_staged with a constant
// {0, 0, 1} vector, static or automatic, 347 (aligned 131, masked 16), with the output rows' 0 and 1 factors read
// from that constant too 347 (aligned 131), the written cross with only the output rows through the constant 347
// (same code as the composite). Left on the composite: the resolver's copy order, a copy of the k*0 product ahead of
// its two uses in the side vector, and the output rows' register copies. BLOCKED-BY 0x210850 (single-site expansion).
// M3 lane 6 r202 (fast with 0x1fe410/0x2104b0 unless noted; control 373, register-masked aligned 193). Diagnostics: the
// random-state global named by a native (non-LTCG) object (a stub-file function reading it; plain check) gives retail's
// head through +0x50 (frame 0x48, the third input in ESI from the entry) but 472 at 1829 B, and a full check with it loses
// 83 matched functions, so retail's global is clean and the reload after the resolver is not a poisoned global; 1 to 4
// and 6 dead initialised longs at the top 373 each (masked 193); the side vector normalised without taking its address
// 378 (the five seed reloads stay). Forms: the sign step and draws one and three through a named `dword *seed` 423; that
// with the second draw through function_259d0 on the same pointer too 438 (1856 B, masked 175: closer in shape).
// M3 lane 9 r144 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, control 373): the head's cause is the
// random-state pointer. Ours loads it into ESI at the timer draw and carries it across the 0x2104b0 call to the
// sign draw. Retail reloads it into ECX after the resolver (0x1fe7f6), so retail's link treats that call as
// possibly writing the global. A diagnostic call to an opaque extern inside 0x2104b0 (reverted) gives retail's
// whole head up to the resolver at +0x1ab: ESI loaded with the third input at entry, ECX loaded with the state
// before the flag stores. Only the frame (0x4c) and the resolver's shape differ there, 359 (1852 bytes). The
// existing 0x210850 called out of line does the same for the head, 445 (1807 bytes). A probe in 0x210d60 437 (it
// changes 0x2104b0's code); 0x210d60 without `inline` 414 (0x2104b0 loses its match). Real trials: the 0x210850
// call 445; draws one and three through the state local 423; that with the third draw's bounds read early 437;
// the state local declared before the 0x1fe410 test 428; the 0x210850 call with all three draws through
// function_259d0 on the state local 452. Next lead: what makes retail's 0x2104b0 call clobber the random-state
// global (its callees 0x210d60 and 0x142570 are matched leaves).
// M3 lane 1 r131 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, control 373, register-masked aligned 64): the
// final state copy as `= *local_3` 380; the timer count read into a long 427 (1855 B); the timer draw through
// slot_random_range 395; the timer count read from its field at each of its four uses (no short local) 378 (masked
// 63); the +0x84 scale product written `scale * value` 373. The largest masked block left is the three range draws
// (retail forwards the seed through them with one store; ours reloads and stores it per draw).
// M3 lane 6 r204 (fast with 0x2104b0, 0x210850, 0x1fdce0, 0x1fe410, 0x259d0, all MATCH each run; control 373, difflib counts as
// [exact, stack offsets masked, registers masked] [237, 193, 64]; body unchanged, the best form saved as
// build/r204/lead-1fe620-composite-body.cpp in worktree m3-lane6). The lead above is answered: retail's 0x2104b0 call does not
// clobber the random-state global. Retail keeps that pointer in ECX, a scratch register, for the whole body and simply loads
// it again after each call that uses ECX (0x1fe7f6 after 0x2104b0, 0x1fe818 after 0x1fe410); the path with no output index
// (0x1feab7) jumps past the reload at 0x1fe7f6 with the first load still in ECX. This build gives the pointer ESI and reads the
// third input from the stack because the pointer has more reads here: the seed is loaded and stored at every draw, where retail
// forwards it from the sign step through the three range draws and stores it once, which it can only do with every bound read
// ahead of the first range draw (a bound read between two seed stores may alias the seed). Diagnostic, reverted: a store
// through a pointer or a reference variable in 0x2104b0 (still matched) 373, same code here, so the callee's summary is not
// the lever. Round 63's composite re-measured 266 (1873 B) [147, 113, 23], the first 113 rows retail's; on it the sign test
// as `(seed & 0x80000000) > 0` 269 [145, 111, 21]; the limits read in retail's order (+0x10, +0x14, +0x18, +0x1c, +0xc) with
// +0x38 named for its two uses 296 [144, 110, 19]; +0x3c read at each use 342 at retail's 1861 bytes [120, 111, 19]; all
// three 341 (1861 B) [115, 106, 13]; plus the final sums read through a named pointer to the stored offset vector (retail's
// `[eax]`, `[eax + 4]`, `[eax + 8]` and its vector-first operand order) 347 (1855 B) [111, 102, 9], the same with a pointer
// for the target point too. On that: the zero product of k named for its two rows 347 [116, 104, 11]; the side vector
// declared ahead of the displacement 347 [9]; the displacement written out k, j, i 347 [9]; the side vector through
// vector3f_assign 347 [9]; the up vector as a local {0, 0, 1} with the full cross written against it 347 [9]; the cross rows
// k, j, i 339 (1864 B) [19]. Left on the 9: the failed-lookup copy's order (follows the pointer's register: three scratch
// registers free after the call in retail), one copy of the k zero product, and the side vector's XMM homes (retail keeps its
// i and j in XMM1 and XMM2 from the cross to the output rows and copies each for the cosine products; this build puts the
// displacement in XMM1 to XMM3 and the side vector after it). Nothing kept: the composite needs the +0x14/+0x18 locals (one
// use each) and the resolver's two-arm body, so it is match-only, and each part alone raises the count.
// MiniMax C (11-19): no gain (373, baseline; body restored). The head differs in the pool load (retail reloads the random-state global after the calls to 0x2104b0 and 0x1fe410, ours keeps the pointer in ESI across them, an LTCG modref decision in another file compilation) and in the transport of the 0x7a argument (ECX against EDX), which the caller in another file decides. Binding the second parameter to one settings local for its five uses measures 415.
// M3 lane 13 r124 (fast with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373, difflib
// [exact, stack masked, registers masked] 324/258/116): the third draw's range read before the first draw as one
// `real_bounds` value (two member uses) 417 (1869 B); the second draw through function_259d0 like the other two 413
// (1861 B, but aligned 298/227/89: closer in shape); both 439 (1865 B); with all limits read in retail's load order
// (+0x10, the +0x14 range, +0x1c, +0xc, +0x38 named for its two uses) 437 (1864 B, 352/255/84); the negated limits
// written in the draw calls in place of their one-use locals 389. The head still needs the seed forwarded through
// the draws (retail's single store), which only the match-only composite gives.
// M3 lane 14 r126 (fast with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373, aligned [exact,
// stack masked, registers masked] 324/255/112; body unchanged): the prop flag as `!(count >= 1 && state != 0)` 373 (same
// code); the held test as `!= false` on both bytes 373 (same code); the third draw through slot_random_range 378; all
// three range draws through slot_random_range 391 (first difference +0x4); the prop block testing the state word before
// the +0x1c limit 373 (other code, 328/259/116); the held byte read into a bool with 0x70e cleared before 0x70d is set
// 465 (1865 B). Re-measured in this tree: lane 8 r110's form (the third draw's bounds as one early bounds copy, the three
// draws through the state local with function_259d0, the lower bounds as call arguments) 334 at 1856 B, first difference
// +0x1ab, 231/152/54: retail's head, frame 0x48, the third input in ESI and the single seed store; still not kept, since
// the copy's two members are read once each (two one-use locals in one) and exists only to read them ahead of the draws.
// Reading for the next round: retail expands 0x210850 only at this call, and this build expands a plain function from
// another file only when that file allows it (lane 9 r138: without /Ob1 on its file it is expanded here and in 25 matched
// callers), so the match stays BLOCKED-BY 0x210850.
// M3 lane 7 r200 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, all MATCH throughout; control 373): the sign
// step through the existing random_next on the state local (`random_next(&local_10->unknown0) & 0x8000`) 378; the same
// on the global with no state local 417; the third input bound by a const reference for its five uses 409 (1852 B);
// `(local_11 & 0x80000000) > 0` with +0x38 named for its two uses 375 (1862 B); random_next on the global with +0x38
// named 412. BLOCKED-BY 0x210850 as above.
// M3 lane 6 r212 (fast; control 373; aligned [exact, stack masked, registers masked]): round 204's composite re-measured
// 347 (1855 B) [127, 118, 16]; on it the two resolver copies as memcpy 347 (same code: the failed-lookup copy stays
// load/store interleaved, where retail loads all three words first, as an aliasing-safe copy through an expanded 0x210850's
// pointers does); the side-vector rows j, i, k 215 at 1861 B [157, 128, 31], i, k, j 268 [132, 121, 25]; on the body,
// the k zero product named for its two rows 373 [320, 255, 112]. Retail keeps the side vector's i, j, k in XMM1, XMM2,
// XMM3 from the cross product to the output rows. BLOCKED-BY 0x210850 (expanded only at this call).
// M3 lane 9 r147 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, all MATCH each run; control 373): on the body
// alone, the sign test as `(local_11 & 0x80000000) > 0` 375 (1863 B); the side-vector rows j, i, k (diagnostic) 425
// (1863 B), with that sign test 428 (1866 B); the second draw through function_259d0 on the state local (draws one and
// three left on the global) 377; the sign step through a `dword *` to the seed field, as slot_random spells it, 373
// (same code). BLOCKED-BY 0x210850 as above.
// M3 lane 8 r214 (h2m fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, all MATCH; control 373): the two lower bounds
// as unary negations 373 (same code); the sign flip through the existing vector3f_negate_ab 373; the +0x38 spread named
// at its block for its test and product 374; both angle tests on `(real)atan2(...)` 373 (same code); the doubled cap as
// `local_16 + local_16` 373. Re-measured (already listed): the state local read before the 0x1fe410 test 428; the third
// input bound by a const reference 409. Retail evidence restated: it reads +0x10, +0x14, +0x18, +0x1c, +0xc and +0x38
// right after the sign step and before draw one's multiply (0x1fe94c), so every bound is read ahead of the first seed
// store; only a source that reads them first (named bounds, one use each) lets the seed stay in EAX. BLOCKED-BY 0x210850.
// M3 lane 16 r164 (fast with 0x1fdce0/0x1fe410/0x2104b0/0x210850/0x259d0, all MATCH; control 373): the side vector through
// unknown_0259d0.h's normalize_inline 378; the sign step tested on the stored seed field with no seed local 373 (same code);
// the +0xc limit read after the two angular limits 373 (same code); the prop flag cleared in an else arm instead of at its
// declaration 398; the sign flip through vector_products.h's vector3f_negate_ab in place 373 (same code). Retail's expanded
// 0x210850 keeps its point in ESI and its output in EDI and lays the NONE path's copy out at the end, jumping past the
// random-state reload: BLOCKED-BY 0x210850.
// M3 lane 16 r167 (H2M fast with 0x2104b0/0x210850, both matched; control 373; body unchanged): the head timer stored as
// `(short)real_to_long(value)` with no result local 373 (same code); the inner timer stores written the same way 373
// (same code, frame still 0x4c); the inner blocks assigning the head's timer value and result locals instead of their
// own 402 (frame 0x50); the angular limit (+0x3c) read from its field at each of its four uses 447 (1846 B); the
// magnitude stored by a chained assignment into its field and local 373 (same code). Retail reads +0x10, +0x14/+0x18
// (onto the x87 stack), +0x1c, +0xc and +0x38 right after the sign step, which is what lets it forward the seed
// through the three draws; BLOCKED-BY 0x210850 as above.
// M3 lane 3 r292 (H2M_FAST with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373): the
// resolver written in 0x210850's own two-arm shape on the target point 466 (1889 B); the same through a `point3f *`
// output pointer 466 (same code); the final three sums from the offset local instead of the stored fields 429; the k
// row's negation written `0.0f - (...)` like the i and j rows 373 (same code). BLOCKED-BY 0x210850 as above. Body
// unchanged.
// M3 lane 1 r148 (H2M fast with 0x210850, MATCH each run; control 373): the early-bounds form (third draw's bounds read
// into locals with the limits, all three draws through function_259d0 on the random-state local, lower bounds as
// `0.0f - limit` arguments) re-measured 340 (1844 B). On it, the existing 0x210850 called for the point (its
// definition marked `inline` in its own file; the out-of-line copy still matches) 266 (1873 B, first +0x1ad: the same
// code as the hand-written two-copy resolver, so the inline definition is how retail's resolver shape can be written);
// with the final sums read back through a named first-output vector pointer (retail's [eax]/[eax+4]/[eax+8]) 251
// (1867 B); the sign test as `> 0` 269; the cross vector declared ahead of the difference vector, and ahead of the
// point, 251 each (same code). What is left at 251: retail's inlined resolver hoists the point loads above the stores
// (its own out-of-line copy keeps them in order, so retail knew the point and the output did not overlap), the
// up-cross block numbers the cross result xmm1/xmm2/xmm3 where ours uses xmm4/xmm3/xmm1, and the NONE path's copy.
// All of it rests on the one-use early bound locals, so nothing is kept. Body unchanged.
// M3 lane 3 r297 (H2M_FAST with 0x210850, 0x2104b0, 0x1fdce0, 0x1fe410 and 0x259d0, all MATCH unless noted; control 373,
// aligned [exact, stack masked, registers masked] 324/255/112; body unchanged). Re-measured: the existing 0x210850 called
// with `inline` on its definition, on this body 466 (1889 B), with the final sums through a named output pointer 460;
// lane 1 r148's early-bounds lead 266, with that pointer 251 [184/134/32]; lane 6 r204's composite 347 (1855 B)
// [127/118/16], the same code with the resolver written out or through the inline 0x210850. New on the composite:
// (1) Retail's failed-lookup copy loads all three words before the stores; ours interleaves because the resolved point
// counts as captured, and the path is 0x1fe410 handing it to 0x1697c0 as its point argument: with that call's point cut
// (diagnostic NULL in 0x1fe620's or 0x1fe410's call, or a copy passed instead) the copy is retail's [120/111/10]. A callee
// that takes the address of its own pointer parameter reproduces the capture (0x1fe410 given a `point3f **` to its
// parameter, with the 0x1697c0 argument still cut: the copy interleaves again), and 0x1697c0 reads its point and vector
// through such parameter addresses: BLOCKED-BY 0x1697c0 (its file is on this round's avoid list; next: measure this
// function with those two parameter-address locals removed, and with each onward callee's point cut).
// (2) Evaluation order: retail computes the rescaled timer as (limit / cap) * count (0x1feb7c `movaps xmm6, xmm4; divss
// xmm6, [cap]; mulss xmm6, xmm3`); this body's `local_12 / local_19 * (real)local_17` compiles to (count / cap) * limit.
// The quotient named, cast to real, written second, or scaled in a second statement all give retail's order: 427 on this
// body (1863 B, aligned 323/254/110; not kept, the count rises), 340 on the composite at retail's 1861 B [132/123/20].
// (3) On composite + quotient: the side vector through cross3f or cross3f_staged against a local up vector 340
// [134/121/18], against a static const up vector 340 [136/123/20], the k zero product named 340 [138/123/20], the three
// rows held in scalars and then stored 340 [130/121/18]: the side vector's homes stay XMM4/XMM3/XMM1 (retail XMM1/2/3,
// with one extra copy of the k zero product).
// M3 lane 3 r298 (H2M_FAST with 0x1697c0, 0x1fe410, 0x16a040, 0x16a7c0 and six other 0x1697c0 callers; control 373): r297's
// lead measured on main's body: 0x1697c0 with both parameter-address locals removed (point and vector read directly)
// 373 here, 0x1697c0 itself 155 (2188 B; its match is lost); with only the point's address local removed 373 here,
// 0x1697c0 122 (2175 B). 0x1fe410 and the other callers stay matched in both. 0x1697c0 needs both locals for its own
// match, so the capture cannot be removed there; restored.
// M3 lane 1 r159 (H2M_FAST with 0x210850 MATCH each run; control 373): the existing 0x210850 called out of line for the
// point (no `inline` on its definition) 445 (1807 B, it stays a call); the random-state local declared at the top for the
// head draw and both function_259d0 draws 413 (1866 B); +0x38 named early with the other limits for its two uses 375; that
// with the second draw through function_259d0 (retail's second draw is the x87 one) 412; the first output named as a
// vector pointer for its store and the three sums, on this body, 376 (1854 B). Retail reads +0x10, +0x14/+0x18, +0x1c,
// +0xc and +0x38 after the sign step and stores the seed once after the three draws. BLOCKED-BY 0x210850 as above. Body
// unchanged.
// M3 lane 11 r163 (H2M_FAST with 0x1fe410, 0x210850 and 0x2104b0 listed; control 373, difflib-aligned 324; body
// unchanged). Reading of the entry: the allocator gives ESI to the random-state pointer (63), which this build keeps
// from the head draw to the three later draws across the calls between them, and ranks the third input far below it
// (3, or not worth a register), so the third input is reloaded from its stack slot; retail loads the third input into
// ESI at the entry and reads the random-state global again into ECX ahead of the 0x1fe410 test and after that call.
// Diagnostic, removed at once: an undefined function called after the 0x1fe410 statement 393 (1852 B): the third input
// then sits in ESI from the entry as retail (the state pointer cannot live across the call); the same call inside the
// 0x1fe410 arm 431 (1865 B, no change at the entry). Forms: the +0x1c limit named for its test and the 0x1fe410
// argument 437 (1866 B); the sign step's state through the global, no pointer local 423 (re-measured); the +0x7a word
// read into a local ahead of the actor lookup 426.
// M3 lane 12 r185 (h2m fast with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373; body
// unchanged). Retail's sign step stores the seed, then loads +0x10, +0x14, +0x18, +0x1c, +0xc and +0x38 interleaved with
// the first draw's multiply, runs the three draws on the seed in EAX and stores it once: every limit is a value before
// the first draw, and with them read first the two seed stores between the draws are dead. The six limits as named
// scalars read in that order (the third draw's two bounds one use each, +0x38 for its two uses): with main's draws 420;
// draws one and three through the state local 441 (1855 B); all three draws through the state local with
// function_259d0 372 (1843 B); that with the two lower bounds written in the calls 367 (1843 B, first difference
// +0x1ab: retail's head, the third input in ESI and one seed store); the same on the global at each draw, no state
// local, 437 (1853 B). Not kept: the 367 form is six points on two one-use locals. BLOCKED-BY 0x210850 as above.
// Local range seed with one final write. Sign-result inputs 440/440, stored inputs 440/440,
// after-first-draw inputs 434/436, shared sign-step inputs 440/440. All keep the 0x4c frame
// and leave the third input off ESI. Original draws and result stores restored.
// M3 lane 12 r187 (H2M_FAST with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373, aligned
// unmatched rows [exact, stack masked, registers masked] 237/193/64; body unchanged). The four conversions written
// without their `(real)` casts (the assignment converts): both angular caps 378 [236/192/63], the timed cap 378
// [236/192/63], the magnitude's root 373, the four trig results 373 (same code each). Lane 3 r297's composite
// re-measured 347 at 1855 B [111/102/9], with the quotient cast 340 at retail's 1861 B [113/104/10]: left there are
// the two point copies (retail loads all three words first: the resolved point is not treated as captured there,
// BLOCKED-BY 0x1697c0's parameter-address locals through 0x1fe410; its file is on this round's avoid list) and one
// colouring order: retail colours the side vector's three components first (XMM1, XMM2, XMM3, live from the cross to
// the output rows) and fits the displacement round them (k XMM1, i XMM3, j XMM4, with a copy of the k zero product
// and of j), where this build colours the displacement's components first (144/126/108 against 101/65 for the side
// vector's) and computes the side vector in what is left. Next: a form that ranks the side vector's components above
// the displacement's (more references to them in the output rows, or fewer register references to the displacement),
// once 0x1697c0 no longer captures its point.
// M3 lane 5 r246 (H2M_FAST with 0x1fdce0, 0x1fe410, 0x210850, all MATCH; control 373). Allocator reading (a diagnostic
// trace): the random-state pointer is one candidate from the first draw to the last (10 references), coloured ESI ahead
// of the third input (priority 63 against 3); retail reloads the global after each call and gives the third input ESI. A
// forced-ranking search: the pointer ranked last 359, the third input ranked first 458 (frame 0x48 as retail, the rest
// worse); no single pick, cut or ranking reaches retail. Forms: the prop test as `!(unknown24 > 0 && unknown27)` 373
// (same code); `local_0` declared at the prop block 395; the timer product written count first 373; the 0x1fe410 bound
// named in its arm 373; the sign step through a `dword *` to the seed 373 (as listed). Body unchanged.
// M3 lane 1 r174 (fast screen with 0x2104b0 and 0x210850, both MATCH; control 373). Reading: the allocator makes the
// random-state pointer one candidate from the timer draw to the last range draw (priority 63, ten references) and
// gives it ESI, so the third input (priority 3) stays on the stack and the frame needs the extra slot; retail loads
// the pointer again after the 0x2104b0 call (and after 0x1fe410) into ECX, i.e. it does not know that 0x2104b0
// leaves the global alone. Diagnostic (reverted): the 0x2104b0 call made opaque gives retail's entry `push esi`,
// the pointer reloaded after that call and short-lived, 423 (other entry changes). So BLOCKED-BY 0x2104b0's
// link-time summary, the same family as the 0x210690 / 0x210770 / 0x2108a0 leads of 0x1f99d0 and 0x299460; an
// `inline` on 0x2104b0's definition (diagnostic) changes nothing here (373). Forms: the sign test as
// `(local_11 & 0x80000000) > 0` 375 (1863 B; it gives retail's `test eax, 0x80000000; jbe` where `& 0x80000000`
// alone becomes `test eax, eax; jns`), with the two range draws through local_10 422, with the length's squares in
// i, j, k order 375; the sign test as `(long)local_11 < 0` 417; the resolver through a call to 0x210850 445 (1807 B),
// with `inline` on that helper's definition (diagnostic) 466 (1889 B). Body unchanged.
// M3 lane 3 r317 (H2M_FAST with 0x1fdce0, 0x1fe410, 0x2104b0, 0x210850 and 0x259d0, all MATCH; control 373, aligned
// exact/register-masked 324/112). The three end sums through vector_products.h's existing vector3f_add on the stored
// output (`vector3f_add(output, (vector3f const *)&local_4, target)`) 378 at 1854 B (321/109): retail's tail exactly
// (all three components read back through the copy's destination pointer) but for the x row's operand order, and the
// 7 bytes the third input's missing ESI home costs elsewhere then show; the operands passed the other way round 378
// (the same code: the order follows the operand ranking); through the local output vector 429; that tail with the
// +0x3c limit read at each use 453 (1840 B); the resolved-point copy as `= *local_3` 380, with the tail 385.
// Body unchanged; still BLOCKED-BY 0x2104b0's link-time summary (retail reloads the random state after it).
// @retail 0x1fe620
void function_1fe620(long arg_0, byte const *arg_1, byte const *arg_2)
{
    bool local_0 = false;
    byte *local_1 = (byte *)actor_get(arg_0);
    if (local_1[0x70e] && !function_1fdce0(arg_0, *(short const *)(arg_2 + 0x7a)))
        local_1[0x70e] = false;
    local_1[0x70d] = local_1[0x70e];
    local_1[0x70e] = false;
    real timer_value = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, *(real const *)(arg_1 + 0x20), *(real const *)(arg_1 + 0x24)) *
        (real)g_510c54->field_2_3;
    long timer_result;
    timer_result = real_to_long(timer_value);
    *(short *)(local_1 + 0x700) = (short)timer_result;
    *(long *)(local_1 + 0x718) = g_510c54->game_time;
    local_1[0x714] = local_1[0x5d0];
    *(real *)(local_1 + 0x7bc) = *(real const *)(arg_1 + 0x34);
    *(real *)(local_1 + 0x7c0) = 0.0f;
    if (*(real const *)(arg_1 + 0x30) > 0.0f)
        *(real *)(local_1 + 0x7c0) = *(real const *)(arg_1 + 0x30);
    if (local_1[0x70d] || local_1[0x70c])
    {
        if (*(real const *)(arg_2 + 0x84) > 0.0f)
            *(real *)(local_1 + 0x7c0) *= *(real const *)(arg_2 + 0x84);
        *(real *)(local_1 + 0x7bc) += *(real const *)(arg_2 + 0x88);
    }
    if (*(real const *)(arg_2 + 0x1c) > 0.0f && *(short *)(local_1 + 0x722) == 1)
    {
        s_prop_node_view *local_2 = prop_node_get(*(long *)(local_1 + 0x724));
        local_0 = local_2->unknown24 < 1 || local_2->unknown27 == 0;
    }
    s_type_c3b527 *local_3 = (s_type_c3b527 *)(local_1 + 0x744);
    point3f local_4;
    if (local_3->output_index == NONE ||
        !function_2104b0(local_3->output_index, &local_3->point, &local_4))
        local_4 = local_3->point;
    if (local_0)
        function_1fe410(&local_4, *(real const *)(arg_2 + 0x1c));
    vector3f local_5;
    vector3d_from_points3d((point3f *)(local_1 + 0x22c), &local_4, &local_5);
    vector3f local_6;
    local_6.i = local_5.j - local_5.k * 0.0f;
    local_6.j = local_5.k * 0.0f - local_5.i;
    local_6.k = local_5.i * 0.0f - local_5.j * 0.0f;
    normalize3d(&local_6);
    real local_9 = (real)sqrt(local_5.i * local_5.i + local_5.k * local_5.k + local_5.j * local_5.j);
    *(real *)(local_1 + 0x71c) = local_9;
    s_random_globals *local_10 = g_4e7408;
    dword local_11 = local_10->unknown0 * 1664525 + 1013904223;
    local_10->unknown0 = local_11;
    if (local_11 & 0x80000000)
    {
        local_6.i = 0.0f - local_6.i;
        local_6.j = 0.0f - local_6.j;
        local_6.k = 0.0f - local_6.k;
    }
    real local_12 = *(real const *)(arg_1 + 0xc);
    real first_limit = *(real const *)(arg_1 + 0x10);
    real second_limit = *(real const *)(arg_1 + 0x1c);
    real first_lower = 0.0f - first_limit;
    real second_lower = 0.0f - second_limit;
    real local_13 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, first_lower, first_limit);
    real local_14 = slot_random_range(second_lower, second_limit) + local_13;
    real local_15 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, *(real const *)(arg_1 + 0x14),
        *(real const *)(arg_1 + 0x18));
    real local_16 = *(real const *)(arg_1 + 0x3c);
    if (local_12 > 0.0f && local_9 > 0.0f && atan2(local_12, local_9) > local_16)
        local_12 = (real)(tan(local_16) * local_9);
    if (local_15 > 0.0f && local_9 > 0.0f && atan2(local_15, local_9) > local_16 * 2.0f)
        local_15 = (real)(tan(local_16 * 2.0f) * local_9);
    short local_17 = *(short *)(local_1 + 0x700);
    if (local_17 > 0 && *(real const *)(arg_1 + 0x38) > 0.0f)
    {
        real local_18 = g_510c54->rate * (real)local_17 * *(real const *)(arg_1 + 0x38);
        if (local_18 > 0.7853981852531433f)
            local_18 = 0.7853981852531433f;
        real local_19 = (real)(tan(local_18) * *(real *)(local_1 + 0x754));
        if (local_12 > local_19)
        {
            real local_20 = local_19 * 1.5f;
            if (local_20 > local_12)
            {
                real timer_value = local_12 / local_19 * (real)local_17;
                long timer_result;
                timer_result = real_to_long(timer_value);
                *(short *)(local_1 + 0x700) = (short)timer_result;
            }
            else
            {
                real timer_value = (real)local_17 * 1.5f;
                long timer_result;
                timer_result = real_to_long(timer_value);
                *(short *)(local_1 + 0x700) = (short)timer_result;
                local_15 *= local_20 / local_12;
                local_12 = local_20;
            }
        }
    }
    real local_21 = (real)cos(local_13);
    real local_22 = (real)sin(local_13);
    real local_23 = (real)cos(local_14);
    real local_24 = (real)sin(local_14);
    vector3f local_25;
    local_25.i = (local_6.i * local_21 + local_22 * 0.0f) * local_12;
    local_25.j = (local_6.j * local_21 + local_22 * 0.0f) * local_12;
    local_25.k = (local_6.k * local_21 + local_22) * local_12;
    vector3f local_26;
    local_26.i = 0.0f - (local_6.i * local_23 + local_24 * 0.0f) * local_15;
    local_26.j = 0.0f - (local_6.j * local_23 + local_24 * 0.0f) * local_15;
    local_26.k = -(local_6.k * local_23 + local_24) * local_15;
    if (*(short *)(local_1 + 0x700) > 0)
    {
        real local_27 = 1.0f / (real)*(short *)(local_1 + 0x700);
        local_26.i *= local_27;
        local_26.j *= local_27;
        local_26.k *= local_27;
    }
    *(s_type_c3b527 *)(local_1 + 0x768) = *(s_type_c3b527 *)(local_1 + 0x744);
    *(vector3f *)(local_1 + 0x784) = local_25;
    *(vector3f *)(local_1 + 0x790) = local_26;
    *(real *)(local_1 + 0x79c) = *(real *)(local_1 + 0x784) + local_4.x;
    *(real *)(local_1 + 0x7a0) = *(real *)(local_1 + 0x788) + local_4.y;
    *(real *)(local_1 + 0x7a4) = *(real *)(local_1 + 0x78c) + local_4.z;
}
