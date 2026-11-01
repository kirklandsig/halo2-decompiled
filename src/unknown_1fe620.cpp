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
