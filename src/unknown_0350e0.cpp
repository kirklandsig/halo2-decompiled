#include "filter_storage.h"
#include "material_records_storage.h"
#include "render_source_storage.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
// @flags /O2 /arch:SSE /Gr
#include "flags_storage.h"
#include "filter_buffer_storage.h"
#include "light_visibility_storage.h"
#include "light_definition_storage.h"
#include "light_frame_storage.h"
#include "orientations_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_0350E0.CPP: format conversion and geometry helpers */

#include "unknown_11c920.h"
#include "globals.h"
#include "depth_groups_storage.h"
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "crc.h"
#include "visibility_slot.h"
#include "geometry_cache.h"
#include "unknown_0494b0.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "unknown_033a0b.h"
#include "bit_vector.h"
#include "object_tag_definitions.h"
bool __stdcall function_c6b90(long unit_index, long name, real *value, bool *active);



matrix3x3 g_467104 = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

PRIVATE __forceinline void rotation_from_axis(vector3f const *axis, real cosine, real sine, matrix3x3 *matrix)
{
    real xx = axis->i * axis->i;
    real yy = axis->j * axis->j;
    real zz = axis->k * axis->k;
    real inverse = 1.0f - cosine;
    real xy = inverse * axis->j * axis->i;
    real xz = inverse * axis->k * axis->i;
    real yz = inverse * axis->k * axis->j;
    matrix->forward.i = (1.0f - xx) * cosine + xx;
    matrix->forward.j = xy + axis->k * sine;
    matrix->forward.k = xz - axis->j * sine;
    matrix->left.i = xy - axis->k * sine;
    matrix->left.j = (1.0f - yy) * cosine + yy;
    matrix->left.k = yz + axis->i * sine;
    matrix->up.i = xz + axis->j * sine;
    matrix->up.j = yz - axis->i * sine;
    matrix->up.k = (1.0f - zz) * cosine + zz;
}

PRIVATE __forceinline vector3f rotate_axis_vector(matrix3x3 const *matrix, vector3f const *vector)
{
    vector3f result;
    result.i = matrix->up.i * vector->k + matrix->left.i * vector->j + matrix->forward.i * vector->i;
    result.j = matrix->up.j * vector->k + matrix->left.j * vector->j + matrix->forward.j * vector->i;
    result.k = matrix->up.k * vector->k + matrix->left.k * vector->j + matrix->forward.k * vector->i;
    return result;
}

// MiniMax D (11-20, run 131): no new form, no lower score (334, 1781 bytes against retail's 1779): the frame is retail's 0x90, but retail loads the axis as i, j and k into XMM0, XMM7 and XMM3 and squares them in that order, where this build loads j, k and i into XMM4, XMM5 and XMM6 and squares j first, and the sine products and the square spills still differ; the earlier rounds' square and phase orders, named sine products and helper receiver forms are all measured.
// MiniMax G (11-19): no new form, no lower score (334): retail computes each axis's sine and cosine inside the builder
// (fsin before fcos) and keeps both angles live on the x87 stack, while this build computes them in the caller and passes
// them to the helper; the forms that compute the trig inside the helper measured 355-357 and lost the matched caller
// 0x137ed0 whose inputs are doubles, so the body stands.
// score 334 (was 334): the builder's squares spill where retail keeps them in XMM1/XMM4/XMM6 and re-reads the axis.
// GB r11 lead, held as a patch (not kept: the forms are kept only with a byte match): 224 against 334: transform4x3f rotation builder (with matrix3x3 *rotation = &matrix->rotation, axis_squares squared in place, sine_axis, and symmetric cross terms in up.j/left.k order) and in-place rotate_axis_vector match retail's 338-insn schedule across both rotation passes (aligned 642->135, 0x137ed0 MATCH); still differs: frame 0x8c vs 0x90 (one fewer pre-sine_axis spill slot), opening axis load order j/k/i vs i/j/k, and x87 i*sine product.
// GB11 (11-25, 34 trials): decal-style matrix3x3 builder on matrix3x3 body 382; transform4x3f + in-place rotate_axis_vector 350, with axis_squares squared in place 330, vector3f_scale 350, up.j before left.k 332 (exact 1779 B, aligned 176), forward-first 337, single-expr rotate 356, out-of-place rotate 361, xy-only 350, angle-inside-builder 348, 4-trig-locals diag 350 (fr=0x90), matrix3x3 + in-place 350, matrix-vector-rotate 444; combining up.j-first + axis_squares name + squared-in-place + matrix3x3* rotation pointer gives 224 (1762 B, 338/338 insns, al=135, plain-confirmed, kept); with forward.j/k reuse 222 (al=128), yz inlined 222/324, jk order 222, vector3f_scale 319, struct-copy *axis 364, brace init 332, 4-trig diag 195 (fr=0x90, fd=+0x10, not kept: one-use trig locals), direct matrix->rotation.* 340, direct matrix3x3* with scale/pos 353.
// MiniMax A (11-18): no new form, no lower score (334): retail's rotation helper keeps the axis in xmm0, xmm3 and xmm7
// with the squares in xmm1, xmm4 and xmm6, takes the cosine on the x87 stack before the cross terms and forms them as
// (inverse * j) * i, (inverse * k) * i and (inverse * k) * j, while this build keeps the axis in xmm4, xmm5 and xmm6,
// spills the squares and reassociates the cross products; the helper's named sine products in retail's order measure
// 386, the three diagonals first measure 347, the cross terms split into named scaled components measure 336 and the
// nine matrix writes in retail's order measure 352, so the body stands.
// Earlier at 334: Frame 0x90 agrees. The first axis load uses a different XMM register.
// The build spills the squares before the cross products.
// MiniMax S (11-17): no new form, no lower score (334): the body is still 1781 bytes against retail's 1779 and
// the frame still 0x98 against retail's 0x90; retail's rotation helper keeps the axis in xmm0, xmm3 and xmm7 with
// the squares in xmm1, xmm4 and xmm6 and takes the cosine on the x87 stack before the cross terms, while this
// build keeps the axis in xmm4, xmm5 and xmm6 and forms the cross terms first.
// MiniMax R (11-16): no new form, no lower score (334): the body is retail's 1779 bytes; retail's rotation helper keeps the axis in xmm0, xmm3 and xmm7 with the squares in xmm1, xmm4 and xmm6, while this build keeps the axis in xmm4, xmm5 and xmm6 and publishes the nine entries from xmm0, xmm1 and xmm2, and the frame stays 0x98 against retail's 0x90.
// MiniMax S (11-16): at 334, no new form; the frame 0x90 agrees but the length is 1781 against
// retail's 1779, and the trig prologue still loads the three axis squares into different registers than retail
// and forms the cross terms before the sine and cosine instead of retail's order, so the body is offset from
// the start.
// Round 80 at the end of this note (334 was 343). Earlier 343 (was 356): round 103 below.
// MiniMax I (11-15): no new form, no lower score (334): retail's rotation helper still keeps the axis in xmm0, xmm3 and xmm7 with its squares in xmm1, xmm4 and xmm6 and takes the cosine on the x87 stack before the cross terms, while this build keeps the axis in xmm4, xmm5 and xmm6 and forms the cross products first; the frame stays 0x98 against retail's 0x90.
// Earlier 334 (was 343): round 80 at the end of this note. Earlier 343 (was 356): round 103 below.
// MiniMax C (11-15): no new form, no lower score (334): retail's rotation helper keeps the axis in xmm0, xmm3 and xmm7 with its squares in xmm1, xmm4 and xmm6, and takes the cosine on the x87 stack before the cross terms, while this build keeps the axis in xmm4, xmm5 and xmm6, forms the cross products before the x87 cosine and publishes the nine entries from xmm0, xmm1 and xmm2; the frame stays 0x98 against retail's 0x90.
// MiniMax B (11-14, later): no new form, no lower score (334): the permuter (300 s, --anneal) was stopped at its
// outer time limit with no result, so the body stands.
// MiniMax B (11-14): no new form, no lower score (334): retail's rotation helper keeps the axis in xmm0, xmm3 and
// xmm7 with its squares in xmm1, xmm4 and xmm6, and takes the cosine on the x87 stack before the cross terms, while
// this build keeps the axis in xmm4, xmm5 and xmm6, forms the cross products (i*j, j*k, k*j) before the x87 cosine
// and publishes the nine entries from xmm0, xmm1 and xmm2; the frame stays 0x98 against retail's 0x90. The body stands.
// MiniMax G (11-12): no new form, no lower score (343): retail's angles are the x87 products of the step with 0.5 and 0.3 and its
// axes are at 0x485624 and 0x485630, which the file's two helpers already take; the frame and the spill homes are what is left.
// MiniMax D (11-14, run 90): no new form, no lower score (334): the frame is 0x98 bytes here against retail's 0x90
// and the register homes of the body still differ; the compile-only copy that reproduces this build's register
// skeleton exactly was screened and left as it is, so what is left is the frame size and the homes; re-measured at
// 334 (1781 bytes, against retail's 1779).
// r136 lane 7 (at 356): a compile-only copy reproduces this build's register skeleton exactly; screened
// there, then built: `sinf(angle)` 357, `cosf(angle)` 357, matrix entries in retail's computation order (forward.i,
// forward.j, left.i, left.j, forward.k, up.i, up.k, left.k, up.j) 381 (1750 bytes), caller-computed sine/cosine with
// both angles and all four trig values first 370 (1796 bytes), the same with double angles 371.
// MiniMax H (11-06): no new form, no lower score.
// Retail builds both matrices from the same two axis angles and rotates with the same nine-component sums
// as the source, so the remaining difference is scheduling: our frame is 0x98 against retail's 0x90, we
// keep the sine and cosine of the two axes in xmm4, xmm5 and xmm6 where retail uses xmm0, xmm3 and xmm7,
// and the nine published components land in different stack homes.
// MiniMax R (11-01): no new form, no lower score.
// Earlier 356 (was 356): second pass walked retail 0x39e50-0x3a540 formula by formula and found no behaviour
// difference: both matrices (forward axis angle step*0.5 from 0x485624.., up axis angle step*0.3 from 0x485630..,
// constants 0.5 / 0.3 / 1.0 read from the XBE), all nine entries of each, and the up*k + left*j + forward*i sum order
// of the vector rotation (0x3a148-0x3a2f0, 0x3a37c-0x3a526) agree with the source. Only scheduling and registers differ.
// Original note: both axes are read straight from g_485618 as retail does (no local copies of
// the axis, which was the whole 16 points). Retail interleaves the two matrices' sin/cos and spills
// differently (frame 0x90 vs 0x98), and stores each rotated component to g_467104 straight from
// xmm with the input vector copied component-wise; here the vectors are copied through integer
// moves. Tried: rotating g_467104 in place (368), the vector by value into an out parameter
// (386), in-place scalar locals (377), accumulating one sum variable (377), aliasing in/out (639,
// wrong), both angles hoisted (361), second rotation first (372), sine before cosine (357).
// Round 38: retail writes each rotated component straight back to g_467104 (only i and j are copied to the stack, k is
// re-read from memory) and the second pass reads g_467104 back, so the in-place form is the closer shape: in place
// (368, frame 0x78, 633 -> 548 aligned differences), in place with both angles as named locals (353; the 0.3 angle goes
// to SSE where retail keeps both angles on the x87 stack), in place with the squares before sine/cosine (390), in place
// with named sine products (356), in place with each rotated vector stored right after its own local (368); on the
// in-place + named-angles form: the matrices declared second, first (353), the angles declared before them (353).
// Round 487: first-turn results at declaration 376; with direct final publication 376; final publication
// alone 357; diagonal-first matrix stores 367; alias-safe explicit output with staged components 380;
// that output form with global first-turn inputs 472; native returned-vector initialization with late
// inputs 376; diagonal-first/late-input/direct-final combination 390. All eight forms and existing helpers
// restored. Frame 0x98 versus 0x90 and simultaneous angle/vector lifetimes remain. Earlier score was 372.
// Round 344: complete axis/input staging, their pair, returned matrix and its three staging subsets,
// then native input-vector copies: 393/356/393/431/452/429/453/485. Original helpers and body
// restored; frame 0x98 versus 0x90 and simultaneous angle/vector lifetimes remain.
// J238: Existing returned-vector setter 356; complete reused sine-axis products 366; double
// trigonometric input 357; setter/products, setter/double, products/double, all 366/357/366/366; all
// with complete native input setters 394. Double forms lose matched 0x137ed0 (42, 147/145 bytes) and
// are rejected. Original helpers and body restored; concurrent angle lifetimes and frame 0x98 versus
// 0x90 remain.
// Round 308: existing shared axes helper, first/second/both phases with original snapshots 366/527/370;
// Round 308: the same phases in place 374/348/465; native first-turn outputs with local/direct
// Round 308: final outputs 405/429. All exact-only forms restored; shared helper grouping differs.
// J240: Sequential reused/scoped matrix 488/486; with complete vector setter 488/486; with symmetric
// reused sine products 467/469; both 467/469. Every trial frame is 0x6c versus retail 0x90. Original frame
// 0x98, concurrent angles and vector publication homes remain; original body and both helpers restored.
// Round 531: Global second-turn inputs with original helper, lexical first-result scope, paired scalar
// setters and matrix/trig role names 422/422/478/478/422/422/478/478; original-graph trig/vector/matrix
// home names 356/356/356/356/356/356/356; complete names with row-local diagonal differences 356.
// Original helpers/body restored; frame 0x98 versus 0x90 remains.
// Round 490: Existing builder consumes caller-computed sine/cosine, pair ownership
// call/first/second/both 355/355/358/357; with two-use real angles 368/368/357/370.
// NOT kept: one-point lead below gate; hoisted one-use trig results require a match.
// Original helper signature, both helpers and complete body restored.
// M323: Component-loop producer with long/dword/short/word ordinal 356/356/715/715; the same with native running sum 356/356/732/732. Narrow loops reach frame 0x90 but lose native extent; original producer and body restored.
// Round 64 (fast, aligned counts from 346): in place through vector3f_rotate_into_axes 465 (aligned 322), through function_2477b1
// 465; scalar in-place helper (i, j, k read once; k re-read as retail) as one expression, running sum or staged 377/377/377 (aligned 276,
// frame 0x64). On that: squares before trig 376; named sine products 411; hoisted angles 394; squares+products 420; squares+angles 396;
// products+angles 434; all three 440; with rows in retail's pair order 481. Retail's matrix slots: first at 0x60, second at 0x2c.
// B409: Paired cross-coefficient construction, original/late diagonal squares and reused sine
// parts, interleaved/grouped diagonal phases: 364/409/362/385/411/441/355/391.
// NOT kept: one-point lead at native length below gate; original builder and complete body restored.
// Y571: Existing builder/returned-vector producer ordinary-inline and plain-private policies,
// eight non-original pairs 356 each; original force-inline declarations and bodies restored.
// Frame 0x98 versus 0x90, concurrent x87 inputs and scalar publication homes remain.
// Y573: Native orientation matrix capture/lifetime/publication graphs:
// 355/422/356/441/394/415/395/436. NOT kept: one-point best below gate; record consolidation and
// copies miss equality; native frame and scalar publications unresolved. Original complete body
// and helpers restored.
// Y575: Existing matrix-row setters, forward/left/up subsets then all with native real trig:
// 356/374/355/374/355/363/363/365. NOT kept: best gain one; matrix-row boundaries do not
// recover frame 0x90 or concurrent x87 inputs. Original two helpers and complete body restored.
// F366: Real two-turn/three-vector tables, both, vector traversal, turn traversal, turn plus
// vector table, both traversals, head-tested turn traversal: 364/372/391/396/675/625/626/626.
// Grouped-storage diagnostics rejected; original builders, arithmetic, snapshots and body restored.
// Y577: Serial first/second/both turn publication and producer-local input windows,
// then those four graphs with symmetric two-axis partials: 356/357/357/356/356/357/357/356.
// NOT kept: all tie or regress; frame 0x98/0x90 and concurrent x87 inputs remain. Original helpers/body restored.
// Y593: Separate scalar i/j/k outputs, pointer/reference, record/scalar results and reused input
// scalars: 470/470/414/414/470/470/450/450. NOT kept: no equality or native frame/extent;
// original six-use helper, snapshots, arithmetic and complete body restored.
// F387: Existing component-array views at axis observations, matrix-row stores,
// rotation operands and returned components, masks 1/2/4/8/3/12/7/15: 356 each.
// NOT kept: all tie at 1775/1779 bytes and frame 0x98/0x90. Original helpers/body restored.
// F390: Existing builder with only sine/cosine supplied by its caller, direct/named part and
// original/twice-used angle graphs: 355/355/368/368/357/357/362/362. NOT kept: one-point
// lead below gate; every frame stays 0x98 versus 0x90. Both original helpers, real-angle contract
// and full body restored; concurrent x87 inputs and scalar publication homes remain, caller exact.
// F392: Actual aggregate forward/left/up row producers in the existing builder, seven subsets then complete aggregate rotation-result initialization: 391/421/406/440/425/491/507/507. Forward+left, forward+up and complete rows recover frame 0x90 but grow/regress.
// NOT kept: no exact match. Original two helpers and complete body restored; native concurrent x87 inputs, frame 0x90 and scalar publications remain unresolved.
// B446: Existing builder owns step/rate multiplication, direct products, real/double
// angle and native trig, original/native in-place publication: 356/356/357/357/368/368/370/369. NOT kept:
// all tie or regress; original private helper contract, helpers and entire body restored.
// Frame 0x98 versus 0x90, concurrent x87 inputs and scalar publication homes remain.
// N399: Matrix-field diagonal/positive/negative compound producers, families 1..7
// then full returned-component accumulation: fast 356/356/356/356/356/356/356/356. Original ordered outputs
// preserved. NOT kept: no native frame/equality recovery; original two helpers/body restored.
// F396: Complete parallel rotation products, up/left/forward subsets then immutable complete terms: 356/353/360/356/359/355/356/356.
// NOT kept: best gain three is below the keep threshold; native frame and concurrent x87 inputs remain. Original complete body and helpers restored.
// R221 (lane 3; whole-file harness, exact 356): builder with a sine*axis vector 358 (1773); plus a shared inverse*k
// 358; rows stored in pair order 391 (1747); the src/unknown_153950.cpp builder's staged scalars with one matrix copy
// 450 (1847), stored per field 358.
// R223 (lane 3, fast, control 356): retail's second matrix keeps the three squares in adjacent homes (+0x1c/+0x20/+0x24) and the
// three sine products in adjacent homes (+0x10/+0x14/+0x18), and produces forward.i, xy, left.i, forward.j, left.j, xz, up.i,
// forward.k, up.k, yz, left.k, up.j. Builder grid of 24 (squares as scalars or a vector3f; sine products inline or a vector3f;
// stores in source order, each cross product named right before its pair, or products first with the stores in retail's order;
// trig before or after the squares): squares vector 379 (1750); sine vector 358 (1773); squares before trig 369 (1761); interleaved
// order 372 (1755); stores-only order 372 (1760); both vectors 390 (1743), with the interleaved order 387 (1752), with the
// stores-only order 380 (1760); squares vector with either order 398/408, before trig 395; the other combinations
// 376/378/383/399/413/420/429/433/434/441/453/463. Builder and body unchanged.
// R223, second pass: the two matrices declared second, first 356 (same code).
// MiniMax E (11-09): no new form, no lower score (356).
// Round 103 (m3 lane 9, fast, confirmed plain): both passes rotate straight from g_467104's rows (retail reads the rows back from
// the global in its second pass) 356 -> 343 (1785 bytes, frame 0x90 as retail; kept); with separately declared locals 343 (same
// code); each row rotated in place 368 (1749); entry copies with direct publication and copy-back 384 (1811); the second pass
// from the locals 376 (1738); the vector by value, rows in place 371 (1795); a per-row copy before an in-place rotation, three
// locals / one `vector` local 384 / 359 (1774: an integer aggregate copy where retail loads floats); plus named angles 353 (1795).
// Left: retail rotates each row in place in the first pass (i and j copied as floats, k re-read), keeps six of the first
// matrix's entries in XMM1-XMM6, and has both angles on the x87 stack before the first fsin. On the kept body: the first pass in
// place with the second as kept 343 (aligned 532 against 519); squares before the trig 354 (1762); the vector by value 363 (1789);
// the two matrices declared separately 343 (same code). Main's entry copies with only the second pass read from the rows 422
// (1846), so both passes reading g_467104 carry the gain.
// R149 (m3 lane 7, fast, control 343): retail's first pass stores each rotated component straight to g_467104 with i and j
// copied to stack temporaries and the partial sum of each component written to a third temporary at every step (a running
// sum with a stack home under XMM pressure: six of the first matrix's entries sit in XMM1-XMM6). An in-place helper (i and
// j copied, k read again, one running sum per component) for both passes 421 (1692 B); for the first pass only, the second
// as kept 361 (1810 B); first pass so, second pass from copies of all three components 344 (1790 B); first pass so,
// second pass storing each rotated row right after it (`g_467104.forward = rotate_axis_vector(&second, ...)`) 341 (1785 B,
// frame 0x78: the running sums lose their stack home); that with the first pass's components as single expressions (k
// re-read) 348 (1793 B). The listed angle, builder and publication forms were not repeated.
// Round 108 (m3 lane 9, fast = plain here): the builder's cross products as `axis->i * axis->j * inverse` (i*k, j*k)
// 343 (same code: VC reassociates them); `inverse` declared right after the cosine 344 (1786 B; masked aligned 588 -> 549),
// with the i*j operand order too 344; diagonals as `xx + (1.0f - xx) * cosine` 343 (same code); off-diagonals as
// `axis->k * sine + xy` 343 (same code); the angles as `0.5f * step` / `0.3f * step` 343. Ours forms j*i and k*i before the trig and multiplies by the complement after it;
// retail keeps (inverse * j) * i in source order after the cosine and starts the second angle and axis squares inside the
// first matrix.
// R151 (m3 lane 7, fast, control 343): the reassociation comes from the `inverse` local: the cross products with
// `(1.0f - cosine)` written in each (no `inverse`) keep (1 - cos) * j * i in source order after the cosine as retail, 350
// (1792 bytes); `axis->j * inverse * axis->i` (and k) 343 (no change); the inline complement with both angles as named
// real locals 360 (the 0.3 angle goes to SSE); with named sine products 390 (frame 0x94); the inline complement with both
// angles as double locals and a double angle parameter 342: retail's exact x87 angle sequence (both products on the stack
// before the first fsin), but the matched caller 0x137ed0 gains a `push ecx` stack-alignment word (42, 147/145 bytes), as
// J238's double forms; the double parameter alone 351 and without the inline complement 344, both losing 0x137ed0; the
// builder in retail's computation order (diagonal, cross term, its pair, per row) 371 (1743 bytes, frame 0x84); squares
// before the trig with the inline complement 349; every cross term written in its two entries 371; the inline complement
// with the second pass storing each rotated row straight to g_467104 350 (as the complement alone), with both passes so
// 362 (1756 bytes). The double parameter alone (344) re-measures J238/B446's double-angle forms on this base.
// Diagnostic: `#pragma optimize("p", on)` around the function 380 (1815 bytes; float consistency is not the cause).
// Round 84 (m3 lane 13, fast with 0x137ed0, control 343): diagnostic, both builder bodies written out in the function
// (measured only) 339 (1784 bytes, first difference +0x2, aligned count up); the rows rotated through the existing
// vector3f_rotate_into_axes (its term order is not retail's) 347 (1766 bytes); the builder's squares written in place 385
// (1728 bytes); the builder taking the axis by value 384 (1815 bytes); only the second angle named ahead of the first
// builder 353 (its product goes to SSE); the builder's parameters as (angle, axis, matrix) 343 (same code); the second
// pass storing each rotated row straight to g_467104 343 (different code). 0x137ed0 matched throughout. The builder
// taking (axis, step, factor, matrix) and forming the angle itself, with `step, 0.5f` / `step, 0.3f` at the calls: 332 at
// retail's length (1779 bytes, register-masked 285 -> 271), 0x137ed0 still matched; NOT kept, an inline helper's parameter
// list picked by score (the start is unchanged: the second angle is still not computed ahead of the first sine). Same code
// as 332: the angle named inside the builder, `factor * step`, the parameters as (axis, factor, step, matrix). On 332:
// the squares before the trig 354, the second pass storing rows straight to g_467104 332 (different code), the rows
// through vector3f_rotate_into_axes 347, the squares written in place 385, `(1.0f - cosine)` in each cross product 350.
// T90: Complete complement-axis scaler; with sine axis, squared axis, both, in-place
// complement scale, reused sine scalars, diagonal parts and double-factor scale: 350/370/344/360/392/394/350/373.
// NOT kept: No gain; native concurrent x87 inputs and matrix/vector homes remain. Both original helpers and complete body restored.
// T95: Complete scalar input captures in existing builders, axis/vector/matrix
// subsets 1/2/4/3/5/6/7, then axis capture after trigonometry: 365/454/343/490/365/454/490/373.
// NOT kept: no native concurrent-angle/publication recovery; original helper
// input observations and full body restored. Current frame 0x90 agrees.
// Round 80 (m3 lane 15, fast with 0x137ed0, confirmed plain; al = LCS-aligned unmatched lines with registers and
// stack slots masked, control 231): retail forms both angle products on the x87 stack before the first sine and takes
// the sine before the cosine. Kept, 343 -> 334 (1781 bytes, frame 0x90, al 209, 0x137ed0 still matched): the two
// angles as double locals of the function, each used twice, with the sine and cosine taken at the calls and the
// builder receiving (axis, cosine, sine, matrix); arguments are evaluated right to left, so the sine parameter
// follows the cosine. This gives retail's opening x87 lines (two fld/fmul pairs, `fld st(1)`, fsin, fxch, fcos).
// The other ways to hold the angles: named real locals 353 (the 0.3 product goes to SSE); double locals cast to the
// builder's real angle 332 at 1779 bytes (the products are forwarded to the calls, the second still late; the cast
// only steers, not kept); a double angle parameter on the builder 334 with 0x137ed0 at 42 (the caller pads the
// stack); the builder as (axis, sine, cosine, matrix) 335 (cosine first). On the kept form: one-use named sine and
// cosine locals 342, with all four ahead of the builders 349; the builder in retail's computation order (squares,
// sine products, each cross term before its pair) with named trig locals 332 (frame 0x8c, al 225, one-use locals)
// and without them 348/354; cross terms stored in their matrix entry and adjusted in place 346/363; sine products
// first 336; retail's second-matrix order 363; first-matrix order with the sine products i, k, j 345; the diagonal
// between the pairs 341; the complement ahead of the diagonal 351; named sine products in the old order 388 (1821
// bytes); the complement written in each cross product 348; stores in retail's entry order 351; the complement
// declared first 332 (a moved declaration, -2, not kept); the cross products as `i * j * inverse` 334 (same code).
// In-place row rotation (i and j saved, k read again), which is retail's shape for both passes: 413 to 415 at 1694
// to 1701 bytes with frame 0x60 (al 191, the closest aligned form), with named trig locals 336/343 (frame 0x64).
// Left: retail keeps the first matrix's squares in registers and multiplies the sine into the registers holding i
// and k, re-reading the axis from memory for the cross terms; ours stores the squares and forms j*i and k*i early.
// Retail's frame also has 0x20 bytes no instruction touches (after each matrix and at +0x28), which the in-place
// forms lose.
// P461: Builder-owned sine/cosine/both real conversions 335/334/337; consumed
// builder matrix identities first/second/both/readonly/reference 334 each. NOT kept:
// no exact repair; double-input forms also lose the caller. Original helpers/body restored.
// M3 lane 10 r104 (fast with 0x137ed0, which stays matched): the builder's nine stores in column order (forward.i,
// left.i, up.i, forward.j, ...) 352 (1798 B); every product with the trig value first (`sine * axis->k`,
// `cosine * (1.0f - xx) + xx`) 334 (same code); the builder in matched 0x171d90's helper order (cross terms yz, xz, xy,
// then forward.i, left.i, forward.j, left.j, forward.k, up.i, up.k, up.j, left.k) 364 (1748 B, first difference +0x2);
// no angle locals, `step * 0.5f` / `step * 0.3f` written in each trig call 381 (1819 B). Body unchanged.
// P489: Whole weighted up/left/forward vector producers in seven subsets, then direct scale/add-scaled
// composition in the actual returned vector: 416/416/436/527/532/515/605/371. NOT kept: No byte equality;
// computed vector storage and helper operand-order alternatives are exact-only. Both original private helpers,
// concurrent angle observations, all arithmetic trees, matrix/vector phases and complete body restored.
// P489 continued: Actual first/second/both matrix-output defaults as aggregates and memset,
// paired value initialization and clearing at each producer: 379/336/378/334/334/334/334/334.
// NOT kept: no native coefficient/storage equality. Original uninitialized matrices,
// concurrent angles, both helpers, vector observations/publications and full body restored.
// Full-transform backing at first/second/both actual matrix receivers: 349/367/373.
// NOT kept: no exact native backing/allocation equality; original matrix types and full body restored.
// P508: Existing builder axis/output and rotation-matrix reference contracts, seven subsets
// then all with the rotated-vector input reference: 334/334/334/334/334/334/334/334.
// NOT kept: no native coefficient-register or scalar-publication equality; original helpers and full body restored.
// M3 lane 5 r186 (fast with 0x137ed0, matched throughout; control 334): retail's frame is two 0x34-byte transforms
// (scale, three axes, position) plus 0x28 bytes of temporaries: its 0x20 untouched bytes are each matrix's scale and
// position (+0x28, +0x50..+0x60, +0x84..+0x90 against axes at +0x2c and +0x60). Both matrices as transform4x3f locals,
// the helpers given `&first.rotation` 373 (1817 B, frame 0xb0; P489's both-receivers count); that with both passes in
// place (i, j and k copied) 379 (1821 B, frame 0x88); in place with only i and j copied and k read again 419 (1689 B,
// frame 0x88); the first pass in place, the second as kept 432 (1870 B); a full-transform builder that also stores
// scale 1 and a zero position (dead stores, removed) on the kept passes 370 (1816 B, frame 0xb0). Body unchanged.
// M3 lane 1 r109 (fast with 0x137ed0, which stays matched; control 334): the builder in 0x1546f0's shape (squares, a
// sine-axis vector, then each diagonal and cross pair in retail's computation order, stored as computed) 377 (1741 B); the
// cross terms grouped `axis->i * (inverse * axis->j)` 344 (1791 B: the squares are still spilled and the axis products
// still hoisted ahead of the cosine); `real const inverse` 334 (same code); the squares renamed ii/jj/kk 334 (same code);
// the cross terms declared yz, xz, xy 344 (1790 B). Retail keeps xx/yy/zz in XMM1/XMM4/XMM6 and spills the i and j sine
// products, while ours spills the squares to hold j*k and i*k, formed before the trig.
// P557: Complete diagonal coefficient component loop in long/dword/short/word, before/after cross
// coefficients: 365/361/365/361/485/488/485/488. NOT kept: no exact native coefficient/publication
// equality; original helpers, concurrent angles and entire body restored.
// P559: Existing builder cosine/sine value/reference/pointer receiver pairs: 335/334/342/342/342/342/342/342.
// NOT kept: unmatched input ownership and temporary/reference forms; frame 0x90 agrees but native
// axis/square/sine residence remains unresolved; original builder contract and entire source restored.
// P562: Actual squared-axis declaration orders ikj/jik/jki/kij/kji and complete
// scalar lexical groups: 344/334/334/344/344/334/334/334. NOT kept: no equality or native
// coefficient residence; current frame 0x90 agrees. Original helpers/body restored.
// M3 lane 16 r129 (fast with 0x137ed0, matched throughout; control 334): with both matrices as transform4x3f locals and
// each row rotated in place with i, j and k copied to locals (lane 5's 379; a running sum per component is the same
// code), our passes take retail's shape: i and j live in stack temporaries, k is read again from g_467104 and each
// partial sum is written to one stack temporary before the next term. What differs on that base is the builder (ours
// loads the axis j, k, i and squares j first; retail i, j, k and squares i first; retail also forms inverse*k twice,
// for xz and yz, where ours shares it). On it: the complement written in each cross product 369 (1812 B); that with
// named sine products i, k, j and the stores in field order 367 (1814 B); the stores made as each pair is computed
// (squares, sine products i, k, j, inverse, then forward.i, xy pair, xz pair, yz pair, left.j, up.k) 418 (1848 B: VC
// moves inverse last, j*i first); matrix3x3 locals with the complement in each cross product 371 (1810 B); i and j
// copied, k read again, with the complement 435 (1673 B); the row copied as an aggregate before the rotation 455
// (1655 B, integer moves where retail loads floats). Body unchanged.
// Round 276. Frame 0x90 and the two early x87 angles agree. Axis and square registers
// and the cross-coefficient and row schedules still differ.
// Staged cross products 344, shared k partial 336, diagonal accumulator 334,
// scoped cross pairs 362, shared sine-axis scaler 357, scaler with scoped pairs 357,
// column dot products 461, and named signed pairs 362.
// NOT kept: the diagonal form ties, and the other forms regress or change the frame or length.
// P598: Axis scalars captured at each square/before all squares 369/376. By-value axis
// with original/named sine products 380/383. Scalar captures with named sine products
// 400/400, with named diagonal complements 369/376. The 0x90 frame and the square
// and sine-product register lifetimes still need to agree in one form.
// Round 601: each axis scalar becomes its sine product immediately after its square 363; capture and square
// each axis, then reuse all three axis scalars for their sine products 377; capture all axes before their
// squares, then reuse the three scalar owners for the sine products 391; read each axis into its square's
// scalar and square that value in place 334; read each independent sine coefficient into its own scalar and
// multiply it in place 388; build each cross coefficient as two successive multiplies of its own complement
// accumulator 344; give the three diagonal complements their own multiply stage before summing 334; reuse the
// three axis scalars for the sine products and compute the cross coefficients with independent accumulators
// 375. NOT kept: the destructively scaled axis captures either enlarge the frame or leave the opening register
// assignment. The square and diagonal compound forms tie 334, and the cross accumulators raise it. The matched
// caller held.
// P603. Caller-side sine-axis through the scaler 360, aggregate 354, staged components 354,
// or in-place scaler 372. Scaler with a value argument 367, aggregate with a reference 354,
// both sine axes produced first with scaler 360 or aggregates 354. NOT kept: every split
// moves the frame from 0x90 and raises the score. The original coefficient builder and caller
// remain, with the first-axis square and sine-product register homes still different.
// MiniMax R (11-17): no new form, no lower score (334): the first difference is the xmm choice for the two constants (this build keeps them in xmm4 and xmm5 where retail uses xmm0 and xmm7), and the body is two bytes longer than retail's 1779.
// M3 lane 8 r183 (fast with 0x137ed0, matched throughout; control 334): each angle in its own block with the build
// (`{ double angle = step * 0.5f; rotation_from_axis(...); }`) 333 at retail's 1779 B (the products no longer both on the
// x87 stack before the first fsin; one point, not kept); on it: the angles as real 372 (1813 B); the first pass in place
// (i and j copied, k read again) 367 (1810 B); the names forward_angle/up_angle kept 333; the complement in each cross
// product 351 (1792 B); each second-pass row stored straight to g_467104 333. Diagnostic: the first pass in place with the
// running sum in a reference-bound local 333 (1783 B): the partial sums go to a stack slot as retail's do, but ours reads
// them back at each step where retail's are spill stores only. Body unchanged.
// AC571: Sine/cosine pairs by const reference, initializer or member assignments, each pair before its
// builder or both before either 342/343/342/343. By value 364/363/364/363. NOT kept: the pair forms
// shrink the matching frame to 0x80 through 0x88 and raise the score. The native axis/square/sine homes still differ.
// Round 623: diagonal complement scaled and added 411, scaled in place and added 377,
// scaled add 381, scaled add into the squares 342, late scaled add 347, late separate scale and sum
// 426, parallel cross and diagonal scaling 413, late scaled add with scaled cross products 339.
// NOT kept: the closest extent still moves the prologue away from the retail frame.
// Round 630: caller square vector 344, caller square scalars 342, square vector with builder-owned
// angle 344, square scalars with builder-owned angle 341, diagonal initialized to squares 344,
// caller-completed diagonal 361, caller squares and complements 362, caller squares and sine axis 371.
// The axis-register assignment and early square spills remain.
// Second matrix after the first forward/left/up result or first forward store,
// with its angle early or at the builder: 370/371, 370/371, 460/465, 460/465.
// NOT kept: the early-result placements keep the 0x90 frame but still spill
// the first squares and grow. The last two placements shrink to frame 0x60.
// Squares in final diagonal cells, completed in field order, before cross coefficients,
// after cross partials, or after cross pairs 370/373/359/365. Sine products in their
// off-diagonal cells 353. Both, with original, early or late diagonal completion 381/348/363.
// NOT kept: all forms shrink the retail-sized frame and raise the score.
// Transposed coefficient stores with returned or accumulated row dots 334/334,
// row aggregates 464, returned matrix 396, output receiver 362, in-place complete
// or staged row-dot stores 344/532, shared two-turn loop 379.
// NOT kept: corresponding scalar stores keep the original register assignment.
// The alternate publications change the frame or extent and raise the count.
// Symmetric pairs seeded and corrected by pair, chained or mirrored, with direct
// or named sine products 350/351/338/357. All pairs seeded before row corrections
// in the same four forms 343/399/367/376. NOT kept: even the closest form
// shrinks the frame, and the frame-preserving named-sine form grows to 1794 bytes.
// M3 lane 8 r191 (fast with 0x137ed0, matched throughout; control 334): a compile-only build of this file gives this
// function's code exactly, so 180 builder and pass shapes were screened there and five confirmed: sine products named
// first (i, k, j) with the complement and cross terms named and the diagonals first, rows rotated in place on transform
// locals (i and j copied, k read again) 334 (1782 B); sine products named after the squares (i, k, j), each cross term
// written in its two entries, diagonals first, in-place rows 328 (1780 B; a statement order and a value written at each
// use, not kept); sine products first, the complement inline, retail's computation order, in-place rows 355 (1753 B);
// the kept rows with sine products first (i, k, j) and each cross term written at its uses in retail's order 336 (1785 B);
// that with the complement inline and field order 333 (1786 B). In-place rows give retail's row schedule (aligned 418
// against 622), but no builder shape screened keeps the squares in registers. Body unchanged.
// M3 lane 14 r121 (control 334; this function's code compiles the same without the link, so shapes were screened there
// and the best measured with `--fast`, 0x137ed0 matched each time; L = true-LCS unmatched lines with registers masked /
// stack slots masked, main 163 / 288 of 338): two 450-step searches over the builder (squares, complement and cross
// terms named or written at each use, sine products inline, named or through vector3f_scale, four cross-term
// groupings and operand orders, both diagonal spellings and every statement order the definitions allow) reach L 148 /
// 267 and 145 / 286; those two shapes measure 361 (1750 B) and 370 (1811 B). By hand: the complement written in each
// cross product L 165 / 269; retail's second-matrix order with it 165 / 274; that with the sine products through
// vector3f_scale 165 / 275; the axis captured in three scalars, squared, then scaled in place by the sine 173 / 278;
// named sine products i, k, j after the squares in retail's first-matrix order 177 / 295, with the complement inline
// 171 / 273. Nothing below main's 163, so none was checked further. Reading (retail's first matrix): the axis sits in
// XMM0/XMM7/XMM3, the squares are copies squared (XMM1/XMM4/XMM6), the sine products destroy the axis registers in
// place (i and k; j through a copy) and the cross terms read the axis from memory again as (complement * j) * i,
// (complement * k) * i, (complement * k) * j with the complement * k product formed twice. Body unchanged.
// Separate symmetric matrix, with final aggregate, in-place correction, field transfers or row setters:
// direct sine products 429/374/358/348; sine-axis vector 379/377/353/353.
// NOT kept: all change the 0x90 frame and raise the score. The axis-square spills remain.
// M3 lane 1 r134 (fast with 0x137ed0, matched throughout; control 334; about 420 shapes screened in a compile-only
// build first, each aligned with retail by region). Retail's second matrix, where every value is spilled, shows the
// builder's statements: the three squares into adjacent homes, the three sine products into adjacent homes, then
// forward.i; xy stored to forward.j, left.i = that minus the k sine product, forward.j reloaded plus it; left.j; xz
// stored to forward.k, up.i = that plus the j product, forward.k reloaded minus it; up.k; yz, left.k = yz plus the
// i product, up.j = yz minus it; each cross term as (1 - cosine) * second * first with both read from the axis
// again. Retail's passes: each row in place with i and j copied to temporaries, k read from the row again, and the
// partial sum of each component written to a temporary after each term. Its frame: two 0x34-byte transforms at
// +0x28 and +0x5c, the squares at +0x1c, the sine products at +0x10 and four scalar homes at +0 to +0xc that hold
// the first sine (+0), second cosine (+4), first cosine (+8) and second sine (+0xc): four homes, not two shared.
// Forms (the builder with squared / scaled vectors and the pair statements above; the rows in place with i, j and k
// copied; both matrices as transform4x3f locals): sine and cosine taken at the calls, squares as products 356 (1799
// B, frame 0x8c), squares copied then squared in place 335 (1782 B, frame 0x8c), sine products as scalars with the
// complement named after forward.i 341 (1783 B) or squares as scalars 343 (1787 B) / 356 (1803 B), squares as scalars
// squared in place with vector3f_scale for the sine products 351 (1798 B). With the four sines and cosines as named
// locals ahead of both builders (one use each: match-only) the frame is retail's 0x90 and the first difference moves
// to +0x10: order sine, cosine, sine, cosine 323 (1781 B), cosines first 324, the second pair first 327 (1779 B, retail's
// length), with squares as products 354 (1801 B). LEAD, not kept (one-use locals, no match): build/r134's
// lead-0x39e50-323.patch. Compile-only only: the builder taking the angle and forming its own sine and cosine
// (real or double angle, either order, angle as an expression or a named local), the sine and cosine through const
// references or filled through out pointers by a small helper all keep frame 0x8c (the two builders share two
// homes); the rows with a named running sum are the same code as the single expression once i, j and k are copied;
// i and j copied with k read again leaves the partial sums in registers. Left on the lead: retail loads the first
// axis i, j, k into XMM0/XMM7/XMM3, squares copies of them, scales i and k by the sine in place and j through a copy
// (j then serves the xy term from its register), and keeps forward.i, left.j, up.i, up.j and up.k of the first matrix
// in XMM2/XMM3/XMM4/XMM5/XMM1; ours loads j, k, i, squares i in place, forms the i sine product late on the x87 stack
// and keeps one more register free for the second matrix.
// Early samples for squares and sine only, with cross terms reading the axis separately. Grouped scalars
// in i/j/k or i/k/j order 382/382, separately scoped 360/362. Vector copy with direct products or
// shared scaler 383/404, component initializer 364/382. NOT kept: all change the retail frame
// and lengthen the body. The saved axis and square registers still differ.
// Shared scaled k before or after the i/j coefficient, by direct or compound
// multiplication, 334 in all four forms. Also naming the scaled j 336 in all
// four forms. NOT kept: the first loads use retail's i, j, k order, but the
// square spills remain and the new forms grow to 1784 bytes.
// Per-axis square/sine/diagonal scopes with immediate or grouped diagonal stores 378/374.
// Separate unscoped axes 378/374. Shared axis capture 363/346. Sine products before
// diagonals 349/341. NOT kept: all lose the 0x90 frame and shorten the body.
// Const square, cross-coefficient and cosine-complement locals, alone or grouped,
// and all three with const angle products 334 each. The 0x90 frame agrees and the
// axis and square registers and their early spills still differ.
// Sine products/cross products/both at first row use 334/362/362.
// Both in paired output order 363; with the complement at the first pair 373.
// Squares at their diagonals 388; all products staged in row/pair order 346/396.
// NOT kept: naming the sine products ties; the cross and square variants
// raise the score. The first-axis register and square spills remain.
// Round 452: Caller-produced cosine complement, evaluated per turn or for both
// turns first. With both sine and complement passed as expressions 378/382;
// named complement 340/351; named sine 340/346; named sine and complement
// 340/357. The frame stays 0x90, but the first axis register and square spills
// remain and every form increases the length.
// Indexed axis squares with direct diagonals, early diagonal stores, diagonal loop after
// cross products or diagonal loop before cross products. Ascending 351/370/342/361, descending 345/375/356/345.
// NOT kept: the frame moves from 0x90 (0x7c, 0x88, 0x8c) and every score rises.
// M3 lane 14 r132 (H2M_FAST with 0x137ed0, MATCH each run; control 334): the builder in the shape of 0x1546f0's file-local
// rotation builder (squares and sine products as vector3f locals, the complement named after forward.i, (t * j) * i and a
// shared t * k) with this function's cosine/sine parameters: that file's store order 382 (1735 B, frame changes), retail's
// pair order (forward.j ahead of left.i, forward.k ahead of up.i, left.k ahead of up.j) 368 (1747 B), no shared t * k 382,
// squares as scalars 377 (1741 B), squares as scalars in retail's pair order 355 (1759 B).
// The first-axis register assignment and the early square spills still differ.
// Scale then two in-place scaled adds 371. Separate up and left scales then forward scaled add 468.
// Left scaled add then separate forward scale and sum 454. Three independent scales before the two sums 632.
// Distinct up-plus-left vector 478. Up product setter then two scaled adds 371.
// Three scales with a separate two-axis subtotal 648. Two-axis product setter then forward scaled add 418.
// NOT kept: All eight forms grow the body and raise the score. The form with a separately scaled forward
// contribution keeps the 0x90 frame and the first difference at +0x10, while the other seven change the
// frame. The matched caller at 0x137ed0 stays exact throughout.
// M3 lane 4 r242 (H2M_FAST with 0x137ed0, MATCH each run; control 334): each cross term in two steps without sharing
// (`xy = inverse * axis->j; xy *= axis->i;`, xz and yz alike, so complement * k is written twice as retail forms it)
// 344 (1791 B); that with i, k, j sine products named after the squares 377 (1818 B); the diagonals written square
// first (`xx + (1.0f - xx) * cosine`) 334 (same code); the rotated rows summed forward first 334 (same code); the axis
// read through a const reference bound to the pointer parameter 394 (1827 B). Body unchanged.
// The 0x90 frame and both early x87 angle products agree. At +0x10 retail loads axis i and keeps the squares through sine scaling. The existing builder is tested with sine or cosine rounded to real at use and with both conversions separated by the three sine products.
// Named sine rounded at builder entry 335.
// Sine argument rounded at its first coefficient 339.
// Named sine rounded at its first coefficient 339.
// Both raw arguments, rounded around the sine products 367.
// Named cosine rounded at builder entry 334.
// Cosine argument rounded after the squares 334.
// Named cosine rounded after the squares 334.
// Both named trig results, rounded around the sine products 372.
// NOT kept: The single-result forms leave the first-axis mismatch. Separating both conversions shrinks the target's frame and body. Every widened handoff raises matched caller 0x137ed0 to 42 in the reduced image. The original builder and both calls are restored.
// M3 lane 5 r227 (h2m fast with 0x137ed0, MATCH throughout; control 334 at 1781 B): the builder's complement
// (`inverse`) declared ahead of the squares 332 (1782 B: only the 1.0 load moves; a moved declaration, not kept); the
// three rotate_axis_vector rows summed forward, left, up 334 (same code); the two angles as `real` 397 (1835 B); the
// squares declared k, j, i 344 (1789 B); the two builder calls in the other order 367 (1813 B). On the complement-first
// form: named sine products (i, k, j, after the squares) 392 (1825 B; 387 without the complement move); the rows summed
// forward first 332 (same code); the first cross term written `axis->j * inverse * axis->i` 332 (same code); squares
// k, j, i 344 (1790 B). The axis registers and the square spills are unchanged in all of them.
// M3 lane 8 r225 (h2m fast with 0x137ed0, MATCH throughout; control 334): one matrix reused for both rotations, the
// second built after the first pass's stores, 444 (1670 B, frame changes); two matrices with the second builder after
// the first pass 460 (1654 B); each rotated row stored straight into g_467104 in both passes (each row reads only its
// own input) 367 (1751 B), in the first pass only 333 (one point, same frame); the rows rotated up, left, forward in
// both passes 334. Retail builds both matrices (both fsin before any rotation) and scalar-replaces most of the first
// matrix: only forward.j, left.i, forward.k and left.k are stored to it.
// The first-axis load order and square spills remain. First/second/both full transform workspaces 349/367/373.
// Axis reference/native array 334/334; row-result initializer/setter 334/334; independent immutable turn results 334.
// NOT kept: transform workspaces increase the 0x90 frame and the extent. All remaining forms tie.
// M3 lane 5 r242 (control 334; on lane 1 r134's 323 lead, that is transform locals, rows rotated in place, the four trig
// values named and squared/scaled vector3f locals in the builder; 0x137ed0 matched throughout): the axis read once into
// i, j and k locals for the squares and sine products, the cross terms still reading the axis, 350 (1802 B, first
// difference +0x1e: the opening then loads i, j and k and squares copies of them as retail does, and no sine product goes
// through the x87); with the cross terms from the locals too 388; with the complement named after forward.i 356; every
// spelling of the squares and sine products over those locals is the same code (350). On the lead without the capture:
// the sine products copied and then scaled in place 376 (1823 B); the sine products ahead of the squares 356; squares and
// sine products interleaved per axis 340; the squares as products 354, with the complement named 353; `sine * axis->i`
// and the sine products in i, k, j order 323 (same code as the lead). The lead's one x87 sine product (the i product,
// both operands in memory) is what the capture removes. Changing the register or the colouring rank of any one XMM value
// on the lead comes no nearer than 323, so what is left is several allocation choices together, not one near miss.
// M3 lane 6 r241 (h2m fast with 0x137ed0, matched throughout; control 334): the builder taking the axis by value (the
// callers passing the global vectors) 387 (1817 B, frame 0x8c); `first` and `second` each declared at its builder 334
// (same code); the cross terms grouped `(inverse * axis->j) * axis->i` 344 (1791 B); `matrix3x3 second, first;` 334
// (same code); forward.i's diagonal stored right after `inverse` 346 (1793 B). Diagnostic (reverted): 1 to 7 dead
// initialised longs at the top 334/334/334/342/342/342/342; the axis loads stay j, k, i (absolute addresses, so no
// pointer-numbering effect), only their registers move. Retail re-reads the axis for the cross terms because its in-place
// sine products overwrite i and k. Body unchanged.
// The 0x90 frame agrees; the first axis reads and square spills still differ.
// Squared and sine-scaled axis, paired or separate passes, shared input, and square pass with scaler 351/345/348/345.
// Completed diagonal and sine axis in the same four forms 345/339/350/339.
// NOT kept: all raise the score. The separate exact-length forms still change the opening frame.
// M3 lane GB r4 (fast with 0x137ed0, MATCH unless noted; control 334, 20 trials): calling out-of-line 0x142640 1290
// (826 B); inlined 0x142640 with `if (scale != 1.0f)` and `*axis` struct copy for squares 438/434/446 (1864-1877 B;
// double angle in helper broke 0x137ed0 -> 42; scale test did not fold); inlined 0x143070 (`temp = *v`, 3-stmt accum)
// 390/379/462 (1737/1741/1652 B); vector3f_scale for sine_axis 436; `axis->n[0..2]` 446; `(a.i*a.j)*t` + accum 446;
// `compute_sincos` out-pointer helper 438/390 (0x137ed0 -> 42). Without the scale test and with float component square
// copies on transform4x3f + in-place row rotations: sin/cos args 344 (1801 B, frame 0x8c, accum same 344), xy-only copy
// 341 (1765 B, 338/338 insns, frame 0x8c); with 4 named trig locals ahead of builders 350 (1814 B, frame 0x90, +0x10,
// mask 291: reproduces retail's pass-1 double-spill rotation schedule; accum and sq-first same 350), matrix3x3 locals
// 336 (1780 B), row loop over `&g_467104.forward` 347 (1801 B, mask 298). Body unchanged.
// Three named diagonal complements at the squares, after the squares, after the cross
// coefficients, or at each diagonal 334 each. With the early sine products 371/371/386/386.
// NOT kept: the complements alone tie, and the sine-product forms move the first difference
// to the frame and grow to 1812 or 1819 bytes. The opening square and sine homes still differ.
// Opposite-sign coefficient triplets: shared sum/direct difference 348,
// aggregate pairs 348, shared sine scaler 345, scaled difference 348,
// diagonals first 356, in-place difference 349, shared sine scaler with scaled
// or in-place difference 346/347. NOT kept: all displace the 0x90 frame.
// Two forms reach retail length, with the axis and square registers still different.
// Full-transform vector helper, first/second/both turn snapshots 452/536/520,
// first/second/both direct in-place turns 432/349/379, copied in-place rows 540,
// and fresh per-row output 469. NOT kept: all move the 0x90 frame and grow
// the body. The first-axis register and square spills still differ.
// M3 lane 15 r147 (fast screen with 0x137ed0, MATCH each run; control 334): each pass's three rotated rows as one
// matrix3x3 aggregate initialiser stored by aggregate assignment 408 (1839 B); the angles as `0.5f * step` and
// `0.3f * step` 334 (same code); `inverse` computed ahead of the squares in the builder 332 (1782 B; two points, not
// kept); the rotated rows' products written vector component first 334 (same code); the diagonals written
// `xx + (1.0f - xx) * cosine` 334 (same code). Body unchanged.
// @retail 0x39e50
void __stdcall function_39e50(real step)
{
    matrix3x3 first, second;
    double forward_angle = step * 0.5f;
    double up_angle = step * 0.3f;
    rotation_from_axis(&g_485618.forward, (real)cos(forward_angle), (real)sin(forward_angle), &first);
    rotation_from_axis(&g_485618.up, (real)cos(up_angle), (real)sin(up_angle), &second);
    vector3f forward = rotate_axis_vector(&first, &g_467104.forward);
    vector3f left = rotate_axis_vector(&first, &g_467104.left);
    vector3f up = rotate_axis_vector(&first, &g_467104.up);
    g_467104.forward = forward;
    g_467104.left = left;
    g_467104.up = up;
    forward = rotate_axis_vector(&second, &g_467104.forward);
    left = rotate_axis_vector(&second, &g_467104.left);
    up = rotate_axis_vector(&second, &g_467104.up);
    g_467104.forward = forward;
    g_467104.left = left;
    g_467104.up = up;
}

struct s_frustum_1648d0;
struct s_camera_163db0;
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result);
void function_141590(transform4x3f const *in, transform4x3f *out);
real function_30bf0(vector3f *vector);

void function_36f50(byte *state);
bool function_39a80(byte *state);

struct s_motion_state_groups
{
	long count;
	byte *entries;
	long large_count;
	byte *large_entries;
};

// @retail 0x39880
inline void function_39880(s_motion_state_groups *state)
{
    long i = 0;
    if (state->large_count > 0)
    {
        long offset = 0;
        do
        {
            function_36f50(state->large_entries + offset);
            ++i;
            offset += 0x3a8;
        } while (i < state->large_count);
    }
    i = 0;
    if (state->count > 0)
    {
        long offset = 0;
        do
        {
            function_39a80(state->entries + offset);
            ++i;
            offset += 0x8c;
        } while (i < state->count);
    }
}

real g_4670e4 = 0.85f;

struct s_355e0_function
{
	long size;
	byte *data;
};

struct s_355e0_curve
{
	real period;
	long input;
	real duration;
	s_355e0_function function;
};

real function_13b390(void const *function, real input, real range);

PRIVATE __forceinline real curve_endpoint(s_355e0_function const *curve, real input)
{
	real result = function_13b390(curve, input, 0.0f);
	byte *data = curve->data;
	if (!(data[1] & 0xf0))
	{
		real lower = *(real *)(data + 4);
		real upper = *(real *)(data + 8);
		real clamped = 0.0f > result ? 0.0f : result > 1.0f ? 1.0f : result;
		return (upper - lower) * clamped + lower;
	}
	return result;
}

// @retail 0x355e0
bool function_355e0(long tag, long index, real *first, real *second)
{
	long count = 0;
	(void)&index;
	(void)&first;
	(void)&second;
	byte *definition = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
	*first = 0.0f;
	*second = 0.0f;
	long selection = (*(short **)(definition + 0x60))[index * 2 + 1];
		if (selection != NONE)
	{
		word const *range = *(word **)(definition + 0x50) + selection;
		short const *entry = *(short **)(definition + 0x48) + (*range & 0x1ff) * 2;
		for (long i = 0; i < (*range >> 9); entry += 2, ++i)
		{
			if (entry[1] == 4 || entry[1] == 5)
			{
				s_355e0_curve const *curve = *(s_355e0_curve **)(definition + 0x40) + entry[0];
				real duration = 1.0f;
				if (curve->duration != 0.0f) duration = curve->duration;
				s_355e0_function const *function = &curve->function;
				real initial = curve_endpoint(function, 0.0f);
				real final = curve_endpoint(function, 1.0f);
				real slope = (final - initial) / duration;
				switch (entry[1])
				{
				case 4: *first = slope; break;
				case 5: *second = slope; break;
				}
				++count;
			}
		}
	}
	return count > 0;
}

struct s_4ca40_colors
{
	byte unknown00[0x14];
	dword first;
	dword second;
	real color_amount;
	real light_amount;
};

color3f *unpack_color3f(dword pixel, color3f *color);

struct s_fade_record
{
	long state;
	real current;
	real target;
	long next_state;
	real rate;
	dword marker;
};

extern s_fade_record *g_50942c;
extern long g_509420, g_509424;
extern real g_4670fc, g_467100, g_509428;

// @retail 0x398d0
void function_398d0(real seconds)
{
	real const *time_reference = &seconds;
	/* referenced first in the order the path without a record loads them */
	(void)g_509424;
	(void)g_4670fc;
	(void)g_509428;
	(void)g_467100;
	s_fade_record *record = g_50942c;
	if (record && record->marker == 0xdeadbeef)
	{
		g_509420 = record->state;
		g_4670fc = record->current;
		g_467100 = record->target;
		g_509424 = record->next_state;
		g_509428 = record->rate;
	}
	if (g_509424)
	switch (g_509424)
	{
	case 1:
		g_4670fc += g_509428 * *time_reference;
		if (g_4670fc >= g_467100)
		{
			g_4670fc = g_467100;
			g_509424 = 0;
		}
		g_509420 = 0;
		break;
	case 2:
		g_4670fc += g_509428 * *time_reference;
		if (g_4670fc <= 0.0f)
		{
			g_4670fc = 0.0f;
			g_509424 = 0;
			g_509420 = 1;
		}
		break;
	case 3:
		g_4670fc += g_509428 * *time_reference;
		if (g_509428 > 0.0f && g_4670fc >= g_467100)
		{
			g_4670fc = g_467100;
			g_509424 = 0;
		}
		else if (g_509428 < 0.0f && g_4670fc <= g_467100)
		{
			g_4670fc = g_467100;
			g_509424 = 0;
			if (g_467100 == 0.0f)
				g_509420 = 1;
		}
		break;
	}
	if (record)
	{
		record->state = g_509420;
		record->current = g_4670fc;
		record->target = g_467100;
		record->next_state = g_509424;
		record->rate = g_509428;
		record->marker = 0xdeadbeef;
	}
}

// @retail 0x4ca40
void function_4ca40(s_4ca40_colors const *settings, long step, color3f *light, color3f *color)
{
	(void)&step;
	real blend = step * (1.0f / 7.0f);
	real amount = settings->color_amount < 0.0f ? 0.0f : settings->color_amount > 1.0f ? 1.0f : settings->color_amount;
	real light_amount = settings->light_amount < 0.0f ? 0.0f : settings->light_amount > 1.0f ? 1.0f : settings->light_amount;
	color3f first, second;
	unpack_color3f(settings->first, &first);
	unpack_color3f(settings->second, &second);
	color3f mixed;
	mixed.red = (1.0f - blend) * first.red + second.red * blend;
	mixed.green = (1.0f - blend) * first.green + second.green * blend;
	mixed.blue = (1.0f - blend) * first.blue + second.blue * blend;
	color->red = (color->red * amount + (1.0f - amount)) * mixed.red;
	color->green = (color->green * amount + (1.0f - amount)) * mixed.green;
	color->blue = (color->blue * amount + (1.0f - amount)) * mixed.blue;
	light->red = light->red * light_amount + (1.0f - light_amount) * 0.5f;
	light->green = light->green * light_amount + (1.0f - light_amount) * 0.5f;
	light->blue = light->blue * light_amount + (1.0f - light_amount) * 0.5f;
}

// @retail 0x4ca00
void function_4ca00(s_4ca40_colors const *settings, byte const *data, color3f *light, color3f *color)
{
	s_4ca40_colors const *const *settings_reference = &settings;
	unpack_color3f(*(dword const *)(data + 0xc), light);
	unpack_color3f(*(dword const *)(data + 8), color);
	function_4ca40(*settings_reference, (*(dword const *)data >> 13) & 7, light, color);
}

struct s_2e3f0_record
{
	long tag;
	point3f position;
	byte direction[3];
	byte amount;
	dword color;
};

dword __cdecl function_131f40(real alpha, color3f const *color);

PRIVATE inline byte placement_byte(real value)
{
	real clamped = 0.0f > value ? 0.0f : value > 255.0f ? 255.0f : value;
	return (byte)clamped;
}

// @retail 0x2e230
void function_2e230(long tag, point3f const *position, vector3f const *direction,
	color3f const *color, real alpha, real amount, real scale, s_2e3f0_record *record)
{
	(void)&color;
	(void)&alpha;
	(void)&amount;
	(void)&scale;
	byte *definition = g_4e3b44[tag & 0xffff].bytes;
	record->tag = tag;
	record->position = *position;
	record->direction[0] = placement_byte((direction->i + 1.0f) * 128.0f);
	record->direction[1] = placement_byte((direction->j + 1.0f) * 128.0f);
	record->direction[2] = placement_byte((direction->k + 1.0f) * 128.0f);
	if (definition[0x28] & 0x40)
	{
		record->amount = placement_byte(alpha * 256.0f);
		record->color = *(dword *)&scale;
	}
	else
	{
		record->amount = placement_byte(amount * 256.0f);
		record->color = function_131f40(alpha, color);
	}
}

// @retail 0x2e3f0
void function_2e3f0(s_2e3f0_record const *record, long *tag, point3f *position,
	vector3f *direction, color3f *color, real *alpha, real *amount, real *scale)
{
	(void)&direction;
	(void)&alpha;
	(void)&amount;
	(void)&scale;
	byte *definition = g_4e3b44[record->tag & 0xffff].bytes;
	if (tag)
		*tag = record->tag;
	if (position)
		*position = record->position;
	if (direction)
	{
		direction->i = record->direction[0] * (2.0f / 255.0f) - 1.0f;
		direction->j = record->direction[1] * (2.0f / 255.0f) - 1.0f;
		direction->k = record->direction[2] * (2.0f / 255.0f) - 1.0f;
	}
	if (definition[0x28] & 0x40)
	{
		if (color)
			*color = *(color3f const *)((byte const *)g_4686cc + 4);
		if (alpha)
			*alpha = record->amount * (1.0f / 255.0f);
		if (amount)
			*amount = 1.0f;
		if (scale)
			*scale = *(real const *)&record->color;
	}
	else
	{
		if (color)
			unpack_color3f(record->color & 0xffffff, color);
		if (alpha)
			*alpha = (record->color >> 24) * (1.0f / 255.0f);
		if (amount)
			*amount = record->amount * (1.0f / 255.0f);
		if (scale)
			*scale = 1.0f;
	}
}
long g_4ba01c;
byte g_4ba020, g_4ba021;
bool g_4ba022, g_4ba023, g_4ba024;
byte g_4ba025;
real g_4ba028, g_4ba02c;

// @retail 0x2c490
void function_2c490(long mode)
{
    bool enabled;
    real value;
    switch (mode)
    {
    case 2:
        g_4670e4 = 0.3f;
        g_4ba02c = 0.5f;
        value = 0.8f;
        g_4ba01c = 3;
        enabled = false;
        g_4ba021 = true;
        break;
    case 3:
        g_4670e4 = 0.3f;
        g_4ba02c = 0.5f;
        value = 0.8f;
        g_4ba01c = 3;
        enabled = false;
        g_4ba021 = true;
        break;
    case 4:
        g_4670e4 = 0.25f;
        g_4ba02c = 0.25f;
        value = 0.7f;
        g_4ba01c = 2;
        enabled = false;
        g_4ba021 = true;
        break;
    default:
        g_4670e4 = 0.85f;
        value = 1.0f;
        enabled = true;
        g_4ba021 = false;
        g_4ba02c = 1.0f;
        g_4ba01c = 4;
        break;
    }
    g_4ba020 = enabled;
    g_4ba022 = enabled;
    g_4ba024 = enabled;
    g_4ba023 = enabled;
    g_4ba025 = enabled;
    g_4ba028 = value;
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_scalar_object_header
{
    byte unknown00[8];
    byte *object;
};

// @retail 0x33670
real function_33670(long index)
{
    byte *object = (byte *)function_badc0(index, NONE);
    real result = 0.0f;
    while (object)
    {
        if (!object[0xaa])
        {
            byte *current = ((s_scalar_object_header *)g_4e0300->data)[index & 0xffff].object;
            result = *(real *)(current + 0x2b0);
            result = result < 0.0f ? 0.0f : result > 1.0f ? 1.0f : result;
            break;
        }
        long parent = *(long *)(object + 0x14);
        if (parent == NONE)
            break;
        index = parent;
        object = ((s_scalar_object_header *)g_4e0300->data)[index & 0xffff].object;
    }
    return result;
}





s_filter_storage g_filter_storage = {{0, 0, 0, {0.0f}, {-1.0f}, {0.0f}}};
struct s_transition_filter
{
	long unknown00;
	s_transition_scalar previous;
	s_transition_scalar delta;
};
s_transition_filter g_4b99a0;

// @retail 0x28950
void function_28950(void)
{
	memset(&g_4670cc, 0, sizeof(g_4670cc));
	memset(g_4b99b0, 0, sizeof(g_4b99b0));
}

// @retail 0x28980
void function_28980(long index)
{
	memset(&g_4b99a0, 0, sizeof(g_4b99a0));
	g_4670cc.active = true;
	g_4670cc.changed = true;
	g_4670cc.index = index;
	real now;
	if (g_510c54 && g_510c54->active)
		now = g_510c54->game_time * g_510c54->rate;
	else
		now = 0.0f;
	g_4670cc.start.value = now;
	g_4670cc.elapsed.value = 0.0f;
	g_4670cc.output.value = 0.0f;
}


struct s_cluster_tag_entry
{
	dword unknown00;
	long tag;
};

struct s_cluster_tag_table
{
	byte unknown00[8];
	long count;
	s_cluster_tag_entry *entries;
};



struct s_render_model_definition;
void render_model_get_default_orientations(s_render_model_definition const *definition, rigid_transform_scaled *orientations);
void render_model_build_node_matrices(vector3f const *forward, vector3f const *up, point3f const *position,
	s_render_model_definition const *definition, transform4x3f *matrices, rigid_transform_scaled const *orientations);

// The flag, the count and the matrices are one block: the matrices are passed by address.
struct s_node_matrix_cache
{
	bool valid;
	long count;
	transform4x3f matrices[255];
};
s_node_matrix_cache g_4c5038;

void function_4c690(long tag, transform4x3f const *nodes);
bool __stdcall function_3ebd0(vector3f const *offset, transform4x3f const *matrices, transform4x3f *out, long count);
void function_14bc0(short index, short element, bool use_depth);
extern long g_4858b8;

// @retail 0x3ede0
void function_3ede0(void)
{
	transform4x3f matrices[255];
	if (g_4b9ee9 && g_4b9eec != NONE)
	{
		long index = NONE;
		s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
		if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
			index = table->entries[(short)g_4b9eec].tag;
		byte *data = 0;
		if (index != NONE) data = g_4e3b44[index & 0xffff].bytes;
		byte *definition = g_4e3b44[*(long *)(data + 4) & 0xffff].bytes;
		if (*(long *)(definition + 0x48) != g_4c5038.count)
		{
			g_4c5038.count = *(long *)(definition + 0x48);
			g_4c5038.valid = false;
			return;
		}
		if (g_4c5038.valid)
		{
			function_3ebd0(g_4687a4, g_4c5038.matrices, matrices, *(long *)(definition + 0x48));
			function_14bc0((short)g_4858b8, 0, true);
			function_4c690(*(long *)(data + 4), matrices);
		}
	}
}

// @retail 0x3ea60
void function_3ea60(void)
{
	struct
	{
		long count;
		rigid_transform_scaled orientations[255];
	} scratch;
	if (g_4b9ee9 && g_4b9eec != NONE)
	{
		long index = NONE;
		s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
		if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
			index = table->entries[(short)g_4b9eec].tag;
		byte *data = NULL;
		if (index != NONE)
			data = g_4e3b44[index & 0xffff].bytes;
		byte *definition = g_4e3b44[*(long *)(data + 4) & 0xffff].bytes;
		scratch.count = *(long *)(definition + 0x48);
		render_model_get_default_orientations((s_render_model_definition *)definition, scratch.orientations);
		g_4c5038.valid = true;
		g_4c5038.count = *(long *)(definition + 0x48);
		render_model_build_node_matrices(g_4687a8, g_4687b0, g_468788,
			(s_render_model_definition *)definition, g_4c5038.matrices, scratch.orientations);
	}
}

__declspec(noinline) real function_3eb70(void);

// @retail 0x3eb70
real function_3eb70(void)
{
	real result = 1.0f;
	if (g_4b9ee9 && g_4b9eec != NONE)
	{
		long index = NONE;
		s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
		if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
			index = table->entries[(short)g_4b9eec].tag;
		byte *data = 0;
		if (index != NONE)
			data = g_4e3b44[index & 0xffff].bytes;
		result = *(real *)(data + 0x14);
	}
	return result;
}

// @retail 0x350e0
void function_350e0(real const *a, real const *b, real *out, real x)
{
	real scale;
	real v;
	real w;

	out[0] = (a[1] - a[0]) * b[0] + a[0];
	out[1] = (a[3] - a[2]) * b[1] + a[2];

	scale = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
	v = (scale * x - g_485ad4.lo * scale) / x * 16777215.0f;
	if (0.0f > v)
		v = 0.0f;
	else if (v > 16777215.0f)
		v = 16777215.0f;
	out[2] = v;

	w = x / g_485ad4.hi * 16777215.0f;
	if (0.0f > w)
		w = 0.0f;
	else if (w > 16777215.0f)
		w = 16777215.0f;
	out[3] = w;
}

// @retail 0x35510
long function_35510(long format)
{
	switch (format)
	{
	case 0: return 1;
	case 1: return 8;
	case 2: return 7;
	case 3: return 2;
	case 4: return 15;
	case 5: return 3;
	case 12: return 5;
	case 14: return 6;
	case 6: return 12;
	case 7: return 13;
	case 18: return 14;
	case 8: return 10;
	case 9: return 11;
	case 10: return 17;
	case 11: return 16;
	case 13: return 18;
	case 16: return 19;
	case 15: return 20;
	case 17: return 21;
	default: __assume(0);
	}
}

extern byte g_485a77;
real g_48597c[36];
extern byte g_4858bc;
extern short g_4858c8;
extern real g_485a0c[8], g_485a34, g_485a40;
extern long g_485a2c;
long g_4858cc;
bool g_4858d0;
real g_485934[4];
vector3f g_485944, g_485950;
real g_48595c, g_485960;
real g_485964[6];
real g_485a38, g_485a3c;
byte g_485a44;
void function_1be50(void);
void function_1bd50(void *state);
bool __stdcall function_16610(long tag, long stage, long wanted_pass, long variant,
    long target, bool copy);

// @retail 0x3a550
void function_3a550(void)
{
    function_1be50();
    D3DDevice_SetVertexShaderConstantFast(-65, g_48597c, 9);
    if (*(long *)(g_4858c4 + 0xe0) != NONE && *(long *)(g_4858c4 + 0xe0) != 0 &&
        *(dword *)(g_4858c4 + 0xdc) == 0x73686164)
    {
        g_485a77 = true;
        function_1bd50(0);
        function_16610(*(long *)(g_4858c4 + 0xe0), 1, NONE, 0, 13, true);
        g_485a77 = false;
    }
}

// @retail 0x35790
long function_35790(long format)
{
	switch (format)
	{
	case 0: return 0x12;
	case 1: return 0x22;
	case 2: return 0x32;
	case 3: return 0x42;
	case 4: return 0x14;
	case 5: return 0x24;
	case 6: return 0x34;
	case 7: return 0x44;
	case 8: return 0x15;
	case 9: return 0x25;
	case 10: return 0x35;
	case 11: return 0x45;
	case 12: return 0x11;
	case 13: return 0x21;
	case 14: return 0x31;
	case 15: return 0x41;
	case 16: return 0x16;
	case 17: return 0x40;
	default: __assume(0);
	}
}

// @retail 0x35850
long function_35850(long format)
{
	switch (format)
	{
	case 2: return 12;
	case 3: return 16;
	case 4: return 1;
	case 5: return 2;
	case 6: return 3;
	case 10: return 6;
	case 1: return 8;
	case 0: return 4;
	case 7: return 4;
	case 8: return 2;
	case 9: return 4;
	case 11: return 8;
	case 12: return 2;
	case 13: return 4;
	case 14: return 6;
	case 15: return 8;
	case 16: return 4;
	case 17: return 4;
	default: __assume(0);
	}
}

// @retail 0x4dcd0
vector3f *function_4dcd0(vector3f *out, dword packed)
{
	vector3f value;
	value.i = (packed & 15) * (1.0f / 15.0f);
	value.j = ((packed >> 4) & 15) * (1.0f / 15.0f);
	value.k = ((packed >> 8) & 15) * (1.0f / 15.0f);
	value.i = value.i * 2.0f - 1.0f;
	value.j = value.j * 2.0f - 1.0f;
	value.k = value.k * 2.0f - 1.0f;
	*out = value;
	return out;
}

// @retail 0x4dd70
vector3f *function_4dd70(vector3f *out, dword packed)
{
	vector3f value;
	value.i = (packed & 2047) * (1.0f / 2047.0f);
	value.j = ((packed >> 11) & 2047) * (1.0f / 2047.0f);
	value.k = (packed >> 22) * (1.0f / 1023.0f);
	value.i = value.i * 2.0f - 1.0f;
	value.j = value.j * 2.0f - 1.0f;
	value.k = value.k * 2.0f - 1.0f;
	*out = value;
	return out;
}

struct s_format_element
{
	byte unknown00[0x2c];
	short format;
	byte unknown2e[0x2e];
};

struct s_format_element_table
{
	byte unknown00[0x28];
	s_format_element *elements;
};

// @retail 0x4cb80
long function_4cb80(long element_index, long tag_index)
{
	long result = NONE;
	if (tag_index != NONE)
	{
		s_format_element_table *table = (s_format_element_table *)g_4e3b44[tag_index & 0xffff].bytes;
		if (table)
			result = table->elements[element_index].format;
	}
	return result;
}

struct s_format_block
{
	byte unknown00[0xc];
	long size;
	byte unknown10[0xc];
};

struct s_format_block_table
{
	dword unknown00;
	dword count;
	s_format_block *blocks;
};

__declspec(noinline)
// @retail 0x4cbb0
long function_4cbb0(long tag_index, dword block_index)
{
	if (tag_index != NONE)
	{
		s_format_block_table *table = (s_format_block_table *)g_4e3b44[tag_index & 0xffff].bytes;
		if (block_index < table->count && (dword)(table->blocks[block_index].size >> 4) > 0)
			return 1;
	}
	return 0;
}

// @retail 0x3fd20
bool function_3fd20(plane3f const *planes, point3f const *point, real radius)
{
	real extent = radius * 1.7320508f;
	for (long i = 0; i < 6; ++i)
	{
		if (plane_distance_to_point(&planes[i], point) >= extent)
			return true;
	}
	return false;
}

struct s_primitive_header
{
	byte type;
	byte unknown01[3];
	long size;
	long capacity;
};

struct s_primitive_storage
{
	dword unknown00;
	byte *data;
};

struct s_primitive_buffer
{
	byte flags;
	byte unknown01[0x33];
	s_primitive_storage *storage;
};

struct s_primitive_group
{
	word unknown00;
	word buffer_index;
	dword unknown04;
	word *counts;
};

struct s_primitive_definition
{
	byte unknown00[0x44];
	s_primitive_buffer *buffers;
	byte unknown48[0xc];
	s_primitive_group *groups;
	byte unknown58[0xc];
	s_primitive_group *groups_alt;
};

// @retail 0x4dc60
s_primitive_header *function_4dc60(s_primitive_definition *data, long index, bool first, long element)
{
	s_primitive_header *result = 0;
	s_primitive_group *group;
	word const *count;
	if (first)
	{
		group = &data->groups[index];
		count = group->counts;
	}
	else
	{
		group = &data->groups_alt[index];
		count = &group->counts[element];
	}
	s_primitive_buffer *buffer = &data->buffers[group->buffer_index];
	if (buffer->flags & 2)
	{
		result = (s_primitive_header *)(buffer->storage->data + 0x20);
		long size = result->type == 0x2e ? 4 * *count : 3 * *count;
		result->size = size;
		result->capacity = size;
	}
	return result;
}

// @retail 0x288e0
real function_288e0(real value)
{
    real change = value - g_4b99a0.previous.value;
    if (0.0f > change)
        change = 0.0f;
    g_4b99a0.delta.value = g_4b99a0.delta.value * 0.92f + change * (1.0f - 0.92f);
    real result = g_4b99a0.delta.value + g_4b99a0.previous.value;
    if (value >= 0.999f)
        return 1.0f;
    if (0.0f > result)
        result = 0.0f;
    else if (result > 1.0f)
        result = 1.0f;
    return result;
}

// @retail 0x36a40
inline real function_36a40(real mean, real spread, real minimum, real maximum)
{
    real const *mean_reference = &mean;
    real const *spread_reference = &spread;
    real const *minimum_reference = &minimum;
    dword sample = random_next(&g_4e7408->seed);
    real random = (real)sample * (1.0f / 65535.0f);
    real result = (random + random - 1.0f) * *spread_reference + *mean_reference;
    if (result < *minimum_reference)
        return *minimum_reference;
    if (result > maximum)
        result = maximum;
    return result;
}


real g_45dd38 = 1.0f / 1023.0f;
real g_45dd44 = 1.0f / 4095.0f;


color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color3f(color3f const *color);

PRIVATE __forceinline real color_delta(real value, real lower, real upper)
{
 return value < lower ? lower : value > upper ? upper : value;
}

// @retail 0x3e4e0
void function_3e4e0(dword *current, dword const *target, real step)
{
	(void)&target;
	color3f a, b;
	unpack_color3f(*current, &a);
	unpack_color3f(*target, &b);
	real red_delta = b.red - a.red;
	real lower = 0.0f - step;
	a.red += color_delta(red_delta, lower, step);
	a.green += color_delta(b.green - a.green, lower, step);
	a.blue += color_delta(b.blue - a.blue, lower, step);
	*current = pack_color3f(&a);
}

struct s_4b160_entry
{
	dword unknown00[2];
	long key;
	dword unknown0c;
	real depth;
	dword unknown14;
	real distance;
	dword unknown1c[2];
};

// @retail 0x4b160
void function_4b160(long first, s_4b160_entry *entries, long mode, long last)
{
	(void)&entries;
	(void)&mode;
	long const volatile *last_reference = &last;
    long bound = *last_reference;
	volatile long begin;
	{
    long i = first;
    if (i <= bound)
    {
        begin = first + 1;
        do
        {
		for (long j = begin; j <= bound; ++j)
		{
			s_4b160_entry *previous = &entries[j - 1];
			s_4b160_entry *current = &entries[j];
			bool swap;
			switch (mode)
			{
			case 0: swap = current->distance > previous->distance; break;
			case 1: swap = previous->key < current->key; break;
			default: swap = current->depth > previous->depth; break;
			}
			if (swap)
			{
				s_4b160_entry temporary = *previous;
				*previous = *current;
				*current = temporary;
                bound = *last_reference;
			}
		}
	
            ++i;
        } while (i <= bound);
    }
    }
}

real g_509418;
real function_30bf0(vector3f *vector);

struct s_vector_perturbation
{
	dword unknown00;
	vector3f direction;
	real speed_spread;
	real speed_minimum;
	real speed_maximum;
	real speed_fraction;
	vector3f velocity;
	vector3f base_direction;
	real direction_spread;
	real magnitude_spread;
	dword unknown40;
	real magnitude;
	real direction_scale;
	real first_fraction;
	real second_fraction;
	real magnitude_fraction;
	vector3f first_axis;
	vector3f second_axis;
	vector3f perturbation;
	real step_scale;
	byte unknown80[0x18];
	real step;
};

__declspec(noinline)
// @retail 0x4b3d0
void function_4b3d0(s_vector_perturbation *state)
{
	state->speed_fraction = function_36a40(state->speed_fraction, state->speed_spread * g_509418, 0.0f, 1.0f);
	real speed = (state->speed_maximum - state->speed_minimum) * state->speed_fraction + state->speed_minimum;
	vector3f_scale(&state->direction, speed, &state->velocity);
	state->first_fraction = function_36a40(state->first_fraction, state->direction_spread * g_509418, -1.0f, 1.0f);
	state->second_fraction = function_36a40(state->second_fraction, state->direction_spread * g_509418, -1.0f, 1.0f);
	state->magnitude_fraction = function_36a40(state->magnitude_fraction, state->magnitude_spread * g_509418, 0.0f, 1.0f);
	vector3f_add_scaled(&state->base_direction, &state->first_axis, state->first_fraction * state->direction_scale, &state->perturbation);
	vector3f_add_scaled(&state->perturbation, &state->second_axis, state->second_fraction * state->direction_scale, &state->perturbation);
	function_30bf0(&state->perturbation);
	vector3f_scale(&state->perturbation, state->magnitude * state->magnitude_fraction, &state->perturbation);
	vector3f_add(&state->velocity, &state->perturbation, &state->velocity);
	state->step = state->step_scale * g_509418;
}

extern real g_485a6c;
real g_485a30;
real g_4e69c0[4];
real g_4c19b0, g_4c19b4;

// a scalar for a selector: constants, frame values and their products, the clamped luminance of the
// view's two summed colours, an object value, a fade fraction or a per-player value
// score 88 (was 89, 153): the first colour is copied and the second added to it, green first (the other five
// orders of the three `+=` score 88 to 167, the six orders of the copies 89 to 166).
// What still differs: retail adds each second component straight from memory
// (`addss xmm0, [eax + 0x38]`); ours loads both into registers first and adds
// them there. Retail also shares only the `0 > value` exit with case 16,
// where ours shares the whole upper clamp (`jmp` into case 16's compare),
// which shifts the code after it. Summing in one expression, via vector or
// struct fields, through references, with a compiler barrier, if/return or
// assignment forms of the clamp, an inline clamp and reordered cases score
// 89 to 271. Tried 2026-10-31: the sums as three plain locals in any channel order and either operand order,
// or inside the weighted expression (all 147: the second operand then is a memory operand as in retail, but the
// loads and the muls are scheduled differently); if/return, nested ?: and `1.0f < value` spellings of the clamps
// (all 89: still the jump into case 16's compare). Also tried: a struct overlay for the two colours with the sums as
// one expression, as three reals or as a field-wise copy (146 to 147: the
// sums come out in the wrong order and the clamp still shares case 16's compare).
// Round 280: plain add/copy/both channel loops 89/88/89; library copy with add loop 278;
// complete selector result with original/add/both/library-copy producers 182/183/183/287.
// Original body restored; memory adds and upper-clamp sharing remain.
// @retail 0x336f0
real function_336f0(long selector)
{
    real result;
    switch (selector)
    {
    case 0: result = 0.0f; break;
    case 2: result = 0.0f; break;
    case 13: result = 0.0f; break;
    case 38: result = 0.0f; break;
    case 16: result = 1.0f > g_485a30 ? g_485a30 : 1.0f; break;
    case 23:
        if (g_485a58)
        {
            vector3f summed;
            vector3f_add(&g_485a58->vector10, &g_485a58->vector30, &summed);
            real value = summed.i * 0.299f + summed.j * 0.587f + summed.k * 0.114f;
            result = 0.0f > value ? 0.0f : value > 1.0f ? 1.0f : value;
        }
        else
            result = 1.0f;
        break;
    case 12: result = 1.0f; break;
    case 22: result = 1.0f; break;
    case 17: result = g_485a30; break;
    case 10: result = g_485774; break;
    case 24: result = g_485774; break;
    case 3: result = g_4857f8; break;
    case 33: result = g_4857f8; break;
    case 4: result = g_485864; break;
    case 25: result = g_485864; break;
    case 18: result = g_485864 * g_485774; break;
    case 27: result = g_485864 * g_485774; break;
    case 19: result = g_485858 * g_4857f8; break;
    case 36: result = g_485858 * g_4857f8; break;
    case 20: result = (1.0f - g_485858) * g_4857f8; break;
    case 35: result = (1.0f - g_485858) * g_4857f8; break;
    case 21: result = g_485a6c; break;
    case 26: result = (1.0f - g_485864) * g_4857f8; break;
    case 28: result = g_48578c; break;
    case 29: result = g_485868; break;
    case 30: result = (1.0f - g_485868) * g_48578c; break;
    case 31: result = g_485868 * g_485774; break;
    case 32: result = g_4857a4; break;
    case 34: result = g_485858; break;
    case 37: result = function_33670(*(long *)g_485a58); break;
    case 39:
        if (g_4b9f5c != NONE && g_4b9f84 > g_4b9f80)
            result = 0.0f - (1.0f / (g_4b9f84 - g_4b9f80)) * g_4b9f80;
        else
            result = 0.0f;
        break;
    case 40:
        if (g_4b9ed8 >= 0 && g_4b9ed8 < 4)
            result = g_4e69c0[g_4b9ed8];
        else
            result = 1.0f;
        break;
    case 41: result = g_4c19b0; break;
    case 42: result = g_4c19b4; break;
    default: __assume(0);
    }
    return result;
}

struct s_slot_key
{
    byte a;
    long b, c, d, e;
};
long function_0209b0(s_slot_key *key, void const *data, long size);
void function_020b40(long value, long index);

// @retail 0x2dba0
bool function_2dba0(long tag, bool alternate, long c, long d, long e, point3f const *position, vector3f const *direction, color3f const *color, real alpha, real amount, real scale)
{
    bool result = false;
    if (g_4b9ed4 != NONE && tag != NONE && alpha > 0.0f)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (amount > 0.0f || !(definition[0x2a] & 1))
        {
            bool hidden = (alternate && (definition[0x28] & 8)) || (!alternate && (definition[0x28] & 4));
            if (!hidden)
            {
                vector3f delta;
                vector3d_from_points3d(&g_4b9da0, position, &delta);
                if (*(real *)(definition + 0x18) == *(real *)(definition + 0x1c) ||
                    *(real *)(definition + 0x1c) > dot3f(&delta, &g_4b9dac))
                {
                    s_slot_key key;
                    memset(&key, 0, sizeof(key));
                    key.a = alternate;
                    key.b = g_4b9ed4;
                    key.c = c;
                    key.d = d;
                    key.e = e;
                    s_2e3f0_record record;
                    function_2e230(tag, position, direction, color, alpha, amount, scale, &record);
                    long index = function_0209b0(&key, &record, sizeof(record));
                    if (index != NONE)
                    {
                        long next = NONE;
                        if (*(short *)(definition + 0x16))
                        {
                            key.c = 4;
                            next = function_0209b0(&key, &record, sizeof(record));
                        }
                        function_020b40(next, index);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

extern bool g_4ba019;
long function_baf80(long object_index);

// @retail 0x4baf0
void function_4baf0(long object_index, real distance, byte *first, byte *second)
{
    byte *object = ((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    bool opaque = (bool)((*(dword *)(object + 4) >> 19) & 1) | g_4ba019;
    *first = 0;
    *second = 0;
    if (opaque)
    {
        *first = 0xff;
        *second = 0xff;
        return;
    }
    s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
    if (definition->model.tag_index != NONE)
    {
        byte *settings = g_4e3b44[definition->model.tag_index & 0xffff].bytes + 0x28;
        long parent = function_baf80(object_index);
        if (parent != object_index)
        {
            byte *parent_data = ((s_scalar_object_header *)g_4e0300->data)[parent & 0xffff].object;
            s_object_definition *parent_definition = (s_object_definition *)g_4e3b44[*(long *)parent_data & 0xffff].bytes;
            if (parent_definition->model.tag_index != NONE)
                settings = g_4e3b44[parent_definition->model.tag_index & 0xffff].bytes + 0x28;
        }
        long level = 4 - *(word *)(settings + 0x24);
        real width = *(real *)(object + 0x3c) * 7.0f;
        real a = 1.0f;
        real b = 1.0f;
        real limit = level > 0 ? ((real *)settings)[(level > 4 ? 4 : level) + 3] : 3.402823466e38f;
        if (distance >= limit + width)
            a = 0.0f;
        else if (distance > limit)
        {
            a = 1.0f - (distance - limit) / width;
            if (a < 0.0f) a = 0.0f;
            else if (a > 1.0f) a = 1.0f;
            else if (a <= 0.095f) a = 0.0f;
        }
        if (*(real *)settings > 0.0f)
        {
            if (distance >= *(real *)settings)
                b = 0.0f;
            else if (distance > *(real *)(settings + 4))
                b = 1.0f - (distance - *(real *)(settings + 4)) / (*(real *)settings - *(real *)(settings + 4));
        }
        *first = (byte)(b * 255.0f);
        *second = (byte)(a * 255.0f);
    }
}

#include <math.h>
#include "vector_angles.h"
#include "vector_products.h"

extern transform4x3f *g_4687d0;
void function_146de0(void);
void function_146b80(void);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

// @retail 0x3ebd0
bool __stdcall function_3ebd0(vector3f const *offset, transform4x3f const *matrices, transform4x3f *out, long count)
{
    bool result = false;
    bool restore = g_47989c != NULL;
    if (restore)
        function_146de0();
    if (g_4b9ee9 && g_4b9eec != NONE)
    {
        s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
        long index = NONE;
        if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
            index = table->entries[(short)g_4b9eec].tag;
        byte *definition = NULL;
        if (index != NONE)
            definition = g_4e3b44[index & 0xffff].bytes;
        vector3f forward;
        direction_from_angles(&forward, *(real *)(definition + 0x80), 0.0f);
        matrix3x3 axes;
        axes.forward = forward;
        axes.left.i = forward.k * g_4687b0->j - forward.j * g_4687b0->k;
        axes.left.j = forward.i * g_4687b0->k - forward.k * g_4687b0->i;
        axes.left.k = forward.j * g_4687b0->i - forward.i * g_4687b0->j;
        axes.up = *g_4687b0;
        transform4x3f transform = *g_4687d0;
        transform.rotation = axes;
        transform.scale = *(real *)(definition + 0x14);
        transform.position.x = *(real *)(definition + 0x18) * g_4b9da0.x;
        transform.position.y = *(real *)(definition + 0x18) * g_4b9da0.y;
        transform.position.z = *(real *)(definition + 0x18) * g_4b9da0.z;
        for (long i = 0; i < count; ++i)
        {
            memcpy(&out[i], &matrices[i], sizeof(transform4x3f));
            vector3f_add((vector3f const *)&out[i].position, offset, (vector3f *)&out[i].position);
            function_142a60(&transform, &out[i], &out[i]);
        }
        result = true;
    }
    if (restore)
        function_146b80();
    return result;
}

// @retail 0x3d480
bool function_3d480(long object_index, long count, transform4x3f const *matrices, transform4x3f *out)
{
    byte *object = *(byte **)((byte *)g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    point3f const *position = (point3f const *)(object + 0x64);
    vector3f offset;
    offset.i = g_468788->x - position->x;
    offset.j = g_468788->y - position->y;
    offset.k = g_468788->z - position->z;
    return function_3ebd0(&offset, matrices, out, count);
}

#include "unknown_11cb00.h"
struct c_entry_list;


struct s_view;
struct s_bounds3d { real x0, x1, y0, y1, z0, z1; };
typedef point3f rectangle3d_edge[2];
long function_11fa40(box3f const *rectangle, long maximum_edge_count, rectangle3d_edge edges[]);
bool function_1652e0(s_view *view, real margin, point3f const *points, long point_count,
    point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1);
struct s_320b0_box
{
    transform4x3f matrix;
    vector3f radius;
};

// @retail 0x320b0
bool function_320b0(s_320b0_box const *box)
{
    byte *context = (byte *)g_547f88.context;
    box3f bounds;
    bounds.x0 = 0.0f - box->radius.i;
    bounds.x1 = box->radius.i;
    bounds.y0 = 0.0f - box->radius.j;
    bounds.y1 = box->radius.j;
    bounds.z0 = 0.0f - box->radius.k;
    bounds.z1 = box->radius.k;
    rectangle3d_edge edges[12];
    point3f transformed[24];
    function_11fa40(&bounds, 12, edges);
    for (long edge = 0; edge < 12; ++edge)
    {
        {
            long i = edge * 2;
            real x = ((point3f *)edges)[i].x;
            real y = ((point3f *)edges)[i].y;
            real z = ((point3f *)edges)[i].z;
            if (box->matrix.scale != 1.0f)
            {
                x = box->matrix.scale * x;
                y = box->matrix.scale * y;
                z = box->matrix.scale * z;
            }
            transformed[i].x = box->matrix.forward.i * x + box->matrix.left.i * y + box->matrix.up.i * z + box->matrix.position.x;
            transformed[i].y = box->matrix.forward.j * x + box->matrix.left.j * y + box->matrix.up.j * z + box->matrix.position.y;
            transformed[i].z = box->matrix.forward.k * x + box->matrix.left.k * y + box->matrix.up.k * z + box->matrix.position.z;
        }
        {
            long i = edge * 2 + 1;
            real x = ((point3f *)edges)[i].x;
            real y = ((point3f *)edges)[i].y;
            real z = ((point3f *)edges)[i].z;
            if (box->matrix.scale != 1.0f)
            {
                x = box->matrix.scale * x;
                y = box->matrix.scale * y;
                z = box->matrix.scale * z;
            }
            transformed[i].x = box->matrix.forward.i * x + box->matrix.left.i * y + box->matrix.up.i * z + box->matrix.position.x;
            transformed[i].y = box->matrix.forward.j * x + box->matrix.left.j * y + box->matrix.up.j * z + box->matrix.position.y;
            transformed[i].z = box->matrix.forward.k * x + box->matrix.left.k * y + box->matrix.up.k * z + box->matrix.position.z;
        }
    }
    s_bounds3d projected;
    bool visible = function_1652e0((s_view *)(context + 4), *(real *)(context + 0x74), transformed,
        24, NULL, NULL, &projected, true, false);
    visible &= projected.x1 >= *(real *)(context + 0xa4) && *(real *)(context + 0xa8) >= projected.x0 &&
        projected.y1 >= *(real *)(context + 0xac) && *(real *)(context + 0xb0) >= projected.y0;
    return visible;
}

PRIVATE __forceinline real random_signed_value(s_random_globals *random)
{
    dword value = random_next(&random->seed);
    double fraction = (double)value * (1.0f / 65535.0f);
    return (real)(fraction + fraction - 1.0f);
}

// @retail 0x36f50
void function_36f50(byte *state)
{
    struct signed_fraction {
        static __forceinline real evaluate(s_random_globals *state) {
            real fraction = (real)random_next(&state->seed) * (1.0f / 65535.0f);
            return fraction * 2.0f - 1.0f;
        }
    };
    s_random_globals *random = g_4e7408;
    byte *entry = state + 0xec;
    for (long i = 0; i < 4; ++i, entry += 0xb8)
    {
        dword *clear = (dword *)(entry - 0x24);
        for (long n = 0; n < 9; ++n) *clear++ = 0;
        clear = (dword *)((entry - 0x24) + 0x24);
        for (long n = 0; n < 9; ++n) *clear++ = 0;
        clear = (dword *)((entry - 0x24) + 0x3c);
        for (long n = 0; n < 3; ++n) *clear++ = 0;
        clear = (dword *)((entry - 0x24) + 0x48);
        for (long n = 0; n < 3; ++n) *clear++ = 0;
        *(real *)((entry - 0x24) + 0x80) = 1.0f;
        *(real *)((entry - 0x24) + 0x84) = 1.0f;
        *(real *)((entry - 0x24) + 0x78) = 0.0f;
        vector3f const *a = (vector3f const *)(state + 0x24);
        vector3f const *b = (vector3f const *)(state + 0x30);
        vector3f const *c = (vector3f const *)(state + 0x3c);
        *(real *)((entry - 0x24) + 0xac) = (real)sqrt((real)(a->i * a->i) + (real)(a->j * a->j) + (real)(a->k * a->k));
        *(real *)((entry - 0x24) + 0xb0) = (real)sqrt((real)(b->i * b->i) + (real)(b->j * b->j) + (real)(b->k * b->k));
        *(real *)((entry - 0x24) + 0xb4) = (real)sqrt((real)(c->i * c->i) + (real)(c->j * c->j) + (real)(c->k * c->k));
        *(real *)((entry - 0x24) + 0x88) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0x8c) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0x90) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0x94) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0x98) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0x9c) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0xa0) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0xa4) = signed_fraction::evaluate(random);
        *(real *)((entry - 0x24) + 0xa8) = signed_fraction::evaluate(random);
    }
}

#ifndef PIN
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#endif

// score 127 (was 323): clamping the squared distance and the fraction with
// PIN, which reevaluates the clamped expression, and squaring the y offset
// in place gives retail's size and its repeated loads. What still differs:
// the floating-point registers (retail keeps 0.5 in xmm3 and the y centre in
// xmm2), the y centre's stack slot, and the operand order of the mode 0
// scaling (retail loads out[0] and multiplies by parameters[24]). Clamping
// with if/else on locals, a y local inside PIN and reordered products score
// 135 to 323.
// Matched in a full build with stand-in SDK headers (was 124): index is declared first (its read of mode's home comes
// before center_y is stored, so the center_y spill shares that home as in retail); in both arms the x sign is an
// if-form, out[0] is stored, and only then the y sign is chosen (retail uses the `test ebx, ~1` flags after the out[0]
// store), the weighted arm's y sign as `!(index & ~1) ? -0.5f : 0.5f` and its out[0] sum grouped explicitly; the radius
// has no x local; and case 0 multiplies both outputs through one const reference to parameters[24] (operand order of
// the two products).
// @retail 0x34100
bool __stdcall function_34100(long mode, real const *rectangle, real const *coordinates,
    real *out, real const *parameters)
{
    long index = mode - 1;
    real center_x = (rectangle[0] + rectangle[1]) * 0.5f;
    real center_y = (rectangle[2] + rectangle[3]) * 0.5f;
    switch (mode)
    {
    case 0:
        function_350e0(rectangle, coordinates, out, parameters[0]);
        {
            real const &multiplier = parameters[24];
            out[0] *= multiplier;
            out[1] *= multiplier;
        }
        return true;
    case 1:
    case 2:
    case 3:
    case 4:
        if (index >= ((long const *)parameters)[25])
            return false;
        out[0] = (rectangle[1] - rectangle[0]) * coordinates[0] + rectangle[0];
        out[1] = (rectangle[3] - rectangle[2]) * coordinates[1] + rectangle[2];
        out[2] = 0.0f;
        out[3] = 1.0f;
        if (parameters[18 + index] != 0.0f)
        {
            real weight = parameters[18 + index];
            if (parameters[22] != parameters[23])
            {
                real radius = (real)sqrt(PIN((coordinates[1] * 2.0f - 1.0f) * (coordinates[1] * 2.0f - 1.0f) + (coordinates[0] * 2.0f - 1.0f) * (coordinates[0] * 2.0f - 1.0f), 0.0f, 1.0f));
                real fraction = PIN((radius - parameters[22]) / (parameters[23] - parameters[22]), 0.0f, 1.0f);
                weight *= fraction;
            }
            real x_sign = -0.5f;
            if (index & 1)
                x_sign = 0.5f;
            out[0] = ((parameters[1] * x_sign + weight * center_x) + (1.0f - weight) * out[0]) * parameters[10 + index * 2];
            real y_sign = !(index & ~1) ? -0.5f : 0.5f;
            out[1] = ((1.0f - weight) * out[1] + parameters[1] * y_sign + weight * center_y) * parameters[11 + index * 2];
        }
        else
        {
            real x_sign = -0.5f;
            if (index & 1)
                x_sign = 0.5f;
            out[0] = (parameters[1] * x_sign + out[0]) * parameters[10 + index * 2];
            real y_sign = (index & ~1) ? 0.5f : -0.5f;
            out[1] = (parameters[1] * y_sign + out[1]) * parameters[11 + index * 2];
        }
        out[0] += parameters[2 + index * 2];
        out[1] += parameters[3 + index * 2];
        return true;
    case 5:
        out[0] = coordinates[0];
        out[1] = coordinates[1];
        return true;
    default:
        return false;
    }
}


/* the direction's component across the camera's up vector:
   (direction x up) x direction */
PRIVATE inline void direction_perpendicular_to_up(vector3f const *direction, vector3f *perpendicular)
{
    vector3f cross;
    cross.i = direction->j * g_4b9e4c.k - direction->k * g_4b9e4c.j;
    cross.j = direction->k * g_4b9e4c.i - direction->i * g_4b9e4c.k;
    cross.k = direction->i * g_4b9e4c.j - direction->j * g_4b9e4c.i;
    perpendicular->k = direction->j * cross.i - direction->i * cross.j;
    perpendicular->j = direction->i * cross.k - direction->k * cross.i;
    perpendicular->i = direction->k * cross.j - direction->j * cross.k;
}
// MiniMax I (11-20): score 142 (was 177): the three-term projection sums are written as a two-term statement plus
// `+=`, which keeps the source's sum tree (a three-term sum is reassociated by the compiler and would change the
// value), the helper writes the perpendicular's components in retail's k, j, i order, the y sum's third term takes
// the perpendicular first, and case 3 takes its position offsets in z, x, y order; the cases 2 and 4 now share
// retail's tail and the atan2 tail matches byte for byte, but cases 1 and 3 still differ in the XMM register
// choices, case 3 still computes its x and y offsets early and spills the y offset, and the body is 816 bytes
// against retail's 812.
// Tried and not kept this round: the offsets after the helper call (197), the x and y offsets after it (179), the
// offsets inlined in the sums (183), the offsets in three scalar locals (191), the offsets in x, y, z order (143),
// the cross components in k, j, i order (206), the perpendicular in k, i, j order (185), the reference copy before
// the helper call (144), the first or second y term swapped as well (181, 172) and the up component first in the
// cross products (142, the same score and the same code).
// M3 review (11-20, MiniMax R): the up vector read into three one-use locals before the cross products gave 140, under the
// 8-point keep bar on a function above 30 (one-use locals are kept only on a byte match); code back to main (142).
// score 142 (was 177): retail's length, frame 0x18 and result slot; the XMM register choices and case 3's late position offsets still differ (forms below).
// MiniMax B (11-19): no new form, no lower score (193): the frame is still 0x14 against retail's 0x18 and the result still shares the scale argument's home instead of the slot at +8, the reference vector still starts at +8 where retail's starts at +0xc, and the four cases still differ in their float schedule; an else that writes the zero result, a literal zero in the early returns, a declared copy of the result, an angle local, a scaled local and the declaration orders all give byte-identical code, and the 177 form needs the XMM7 clobber that raises the caller 0x2e9e0 from 492 to 572.
// MiniMax D (11-14): no new form, no lower score (193): retail's four cases still compute their cross products with
// their own temporaries - its frame is 0x18 against this build's 0x14 and it stores the zero result to its slot before
// the jump table - and the helper form that reaches 177 raises the caller 0x2e9e0 to 573, while that caller's 492
// body is tuned against the callee this one calls, so the pair cannot be moved apart from this file.
// MiniMax I (11-11): no new form, no lower score (193): the body is 20 bytes shorter than retail's 812; the known form that reaches 177 raises 0x2e9e0 to 573, and 0x2e9e0's 492 body is tuned against the callee this one calls, so the two cannot be moved apart.

// MiniMax E (11-16): no new form, no lower score (193): the frame is still 0x14 against retail's 0x18 and the shared helper 0x2e9e0's frame is 0x90 against retail's 0x94, and none of the earlier rounds' cross-term orders fixes the schedule.
// MiniMax E (11-15): no new form, no lower score (193): cases 1 to 4 still differ in the dot and offset schedule - retail groups the case-1 sums and computes the case-2 and case-4 offsets through dot3f and vector3d_from_points3d where ours differs.
// MiniMax B (11-13): no new form, no lower score (193): retail's four cases still compute their cross products with their own temporaries (its frame is 0x18 against this build's 0x14 and it stores the zero result to its slot before the jump table), so the bodies would have to be respelled one case at a time.
// MiniMax H (11-12): no new form, no lower score (193); the three cross terms' order and the place of the case 1
// reference copy still differ, and the helper forms that reach 177 and 182 still raise the caller to 573, so the body
// stays as it is.
// MiniMax B (11-12): no new form, no lower score (193): this round re-read the diff statement by statement against
// retail and found no behaviour difference; what is left is the three cross terms' order and the place of the
// case 1 reference copy, and the helper forms that reach 177 and 182 still raise the caller to 573.
// MiniMax H (11-14): no new form, no lower score (193): the three cross terms' order and the place of the case 1
// reference copy still differ, and the permuter's best variant turned the atan2 arguments round, which changes the
// behaviour, so the body stands.
// MiniMax R (11-16): no new form, no lower score (193): the permuter's lower variants (178 on its own count) rest on banned forms - a volatile x, a constant-only temporary, one-use locals, a disguised zero test and a score-picked statement order - so they were reverted; retail keeps the zero result in its own slot at [esp+8] with a 0x18 frame where this build promotes it (0x14), and the cross terms' order and the case-1 reference copy still differ.
// Earlier: the statements match retail (cross products, dot-product operand order, atan2 tail).
// MiniMax B (11-11): no new form, no lower score (193): the helper forms that reach 177 and 182 still raise the
// caller to 573, so the body stays as it is.
// MiniMax R (11-10): no new form, no lower score (193): the three cross terms' order and the place of the case 1 reference copy still differ, and the forms tried before (each index ahead of the helper, the term orders and the copy ahead of the helper) all measured at or above the body above.
// MiniMax C (11-09): no new form, no lower score (193).
// MiniMax C (11-09): no new form, no lower score (193).
// MiniMax R (11-06): no new form, no lower score (193).
// MiniMax D (11-06, run 49): no new form, no lower score (193); retail still keeps the result in its own stack slot (0.0f at the entry, filled by the atan2 tail, one shared `movss xmm0, [result]` at the end, frame 0x18) where ours keeps the zero in xmm0, returns early on each path and reuses the scale slot for the x87 result (frame 0x14).
// MiniMax R (11-01): no new form, no lower score: the five-way switch still evaluates the cross products in our order and allocates the XMM registers differently.
// Retail keeps `result` in its own stack slot (stored 0.0f at entry, fstp'd into by the atan2
// tail, one shared `movss xmm0, [result]` at the end, frame 0x18); ours keeps the zero in xmm0,
// returns early on each path and reuses the scale slot for the x87 result (frame 0x14).
// Tried: case 0 and default as break with y and x pre-zeroed (194; the switch then loses its
// case 0 table entry), y = 0 in those cases (193).
// Also tried 2026-10-31 (all 193, `result` is promoted to xmm0 every time): an inline helper that writes
// the angle through a `real *`, `real *angle = &result`, a dead `&result` pointer, the atan2 in a double
// local, `return` inside the `if` (275); `volatile real result` gives retail's frame and entry store (187) but
// zeroes xmm6 instead of xmm0 and so is not kept.
// Also tried 2026-11-01: the helper as two cross3f calls (193, identical code) or cross3f + cross3f_staged (193,
// identical); every three-term sum grouped `(a + b) + c` (195: retail computes all three products before adding,
// so its sums are flattened, j,k,i for case 1/3's y and k,j,i elsewhere, while ours come out i,k,j); dot3f and
// vector3d_from_points3d for the dots and camera-relative vectors (197, 788 bytes). A compile-only screen (no /GL)
// showed product operand order and the case 2/4 term orders are canonicalised, while the helper's component order,
// case 1/3's y term order and the assignment order of `reference` move the schedule; its best (perpendicular
// i,k,j, case 1/3 y i,k,j, case 2/4 reference j,i,k) scored 206 in the real build (776 bytes), since that harness
// returns on x87 and does not model the result slot.
// Shared first cross ordinary/staged, ordinary first with ordinary/staged second, staged first with
// ordinary/staged second, and both staged-second graphs with shared point differences:
// 193/193/193/193/193/193/193/193. Result stack home and SSE producer allocation remain; original arithmetic
// restored.
// Round 503: angle/result-only, scalar-projection, vector-receiver and complete slot names
// 193/193/193/193; the same four graphs with a double result carrier 290/290/290/290.
// Original real result and every expression restored; separate result stack home remains.
// Round 524: shared projection families 1/3, 2/4, both 410/200/402; with one exit
// 410/200/402; both with complete displacement copies, original/one exit 402/402.
// All original source restored; independent result home and frame 0x14 versus 0x18 remain.
// Round 469: Single result exit with entry-zero/if-else/conditional/inverse-if producers, then conditional named double angle: 193/296/296/296/193/296/296/296. All original sums, switch and result restored; independent result home remains.
// F323: Independent result home and frame 0x14 versus 0x18 remain; complete original body restored.
// Whole case orders 01324/02413/13240/24130/43210/31420/12340/04231
// 195/205/195/205/201/222/193/250; every original case expression and helper preserved.
// J240: native output staging in the existing perpendicular helper 247; double atan2 carriers 218; one
// result exit 247/218; full helper input snapshot 314/226; snapshot with one exit 314/225. Frame/output
// lifetime still differs; restored original body and helper after all eight trials worsened 193.
// Round 531: Whole direction input, case-owned projection vectors and whole displacement inputs, seven
// subsets 421/221/341/204/373/275/289; complete immutable inputs 289. The five direction-copy graphs
// with the existing helper force-inlined 189/273/256/336/336. Original helper/body restored; independent
// result home remains.
// Force-inlined direction copy alone: frame 0x18 and result +8 at 189/816 bytes; caller 610.
// DL249: Complete scalar basis/direction/first-cross ownership, seven subsets, then
// all with native result library initialization: 192/229/421/193/192/230/421/360.
// One-point basis gains rejected; direction captures worsen the caller and move length
// away. Original entire helper, result, switch, arithmetic and input observations restored.
// F339: Whole local declaration orders ryxvp/rpxyv/rvpyx/yxrpv/pvyxr/xyrvp/
// vryxp/pyrvx (r result, y/x projections, p perpendicular, v reference):
// 193/193/193/193/193/193/193/193. Original declarations and body restored; independent result
// home and frame 0x14 versus 0x18 remain.
// Lane 16 r80 (fast): retail's xmm0 zero serves the `0.0f -` negations while `result` keeps its own slot; ours
// folds both into one constant. Negations spelled `-(...)` 193 (identical code); result declared at the y test with
// `return 0.0f` in case 0/default 291 (904 bytes); case 0 merged into default 194; `return 0.0f` in case 0/default
// 193; `if (y)` 193. Body restored.
// DH337: Native two-coordinate point/array projections at four declaration boundaries:
// 192/192/192/192/192/192/192/192, 788/812 bytes. NOT kept: one-point gain,
// grouped storage needs equality and length moves farther; independent result home remains.
// Y569: Uniform named first/middle/last product terms in every projection row,
// seven subsets, then complete immutable products:
// 197/197/193/195/197/208/193/193. NOT kept: no qualifying native repair; complete original body restored;
// independent result home, frame and producer allocation remain.
// F369: Existing perpendicular producer returns a native vector, a native vector from reference
// input, or its output pointer, or takes reference output: 382/382/197/193; each with the
// sole result exit 384/384/197/193. All restored; native result home remains, value returns
// shorten to 600 bytes and raise the caller 507 -> 610; no byte match or qualifying gain.
// F371: Uniform leading/trailing/both product parts in first/second cross,
// then leading/both parts in both crosses: 193/193/193/193/193/193/193/193.
// NOT kept: no gain or native frame/result-home recovery; original private
// perpendicular producer, all projection sums and full body restored.
// P318: Whole double projection producers, double result/test/conditional consumers:
// 262/235/262/262; mixed producer domains at the original float atan2 boundary,
// real/double result: 185/262/322/246. NOT kept: the 185 form moves frame to 0x1c
// and caller to 610; native dword result/projection homes remain. All source restored.
// P324: Actual result accumulator at inverse-tangent, scaled-angle and both
// multiplication boundaries, then a named scaled-angle expression: 193 each;
// the same four producers with one common exit: 193 each. Original whole body
// restored; independent result home and frame 0x14 versus 0x18 remain.
// AC542: Complete projection sums as y/x/both accumulations 208/195/197,
// y/x/both named two-product prefixes 219/195/195, then both graphs with CRT result clear
// 282/282. NOT kept: no native frame/result-home repair or equality; original full body restored.
// AC544: Direction/mode/scale formal lexical identities, seven subsets then all
// with position identity: 193/193/193/193/193/193/193/193. NOT kept: no native result-home or frame
// repair; original inputs, helper, switch, arithmetic and whole body restored.
// DH366: Original final projection zero/nonzero admission, bool/byte/short/long outcomes:
// 193/193/193/193/193/193/193/193. NOT kept: no byte match or qualifying native repair; original complete
// body restored.
// DH373: Complete default-mode admission/rejection before the original switch,
// bool/byte/short/long: eight at 286 (696 bytes; caller 610). NOT kept: regression and
// larger length gap; original dispatch, projections, helper and result restored.
// AC546: Alias-safe second cross in its actual first-cross receiver, native/library/setter/
// scalar output: ordinary 421/421/198/197, forced 247/247/198/197. NOT kept: no gain;
// ordinary value copies worsen the caller to 610; original private helper and full body restored.
// DH366: Original final projection zero/nonzero admission, bool/byte/short/long outcomes:
// 193/193/193/193/193/193/193/193. NOT kept: no byte match or qualifying native repair; original complete
// body restored.
// B453: Plain reverse reductions of complete mode-2/4 projection sums: y/x/both for, y/x/both while,
// paired for/while: 193/227/227/193/227/227/227/227. NOT kept: no byte match; original complete body
// restored. Native independent result home and frame remain.
// Y602: Plain component negation/case-three/case-four displacement subsets, then forced existing
// perpendicular producer: 193/193/193/193/193/193/193/193.
// NOT kept: no byte match or qualifying native-shape repair; original complete body and helpers restored.
// F397: Position/direction declaration and sole call reordered together, original/sole exit
// and ordinary/forced perpendicular expansion: 193/193/193/193; cdecl counterparts
// 194/194/194/194. NOT kept: default forms tie, cdecl raises caller to 509;
// original signature, call, helper and body restored. Independent result home remains.
// R221 (lane 3; whole-file harness 189 for the real 193, same bytes): goto to one `done:` exit from case 0 and default
// 189; both labels on one goto 192; result declared at the y test with `return 0.0f` 285 (904); explicit double atan2
// arguments 189; `/ pi` 189. The result stays coalesced with the xmm0 zero.
// R221 also: the case 3 offset through the existing vector3d_from_points3d 189; the case 4 offset that way 189.
// Round 64 (lane 15, fast): result split into `(real)atan2(y, x)` then `* scale * (1/pi)` 193; y and x initialised
// to 0.0f 193; `atan2 * (scale * (1/pi))` 190 (one product where retail has two fmuls); a ternary return 296 (908
// bytes); `scale * atan2 * (1/pi)` 193.
// Round 73 (m3 lane 13; fast, the kept-candidate also plain): the result's own slot is XMM pressure, not a spelling:
// retail's case 3 holds the k offset in xmm7 across both cross products, so with the zero in xmm0 all eight registers
// are busy and the result gets a stack home (entry store, `fstp` into it, one load at the exit). Case 3's three
// offset statements ahead of the perpendicular helper 177 at 812 bytes (plain 177: retail's length, frame 0x18,
// entry store and exit; aligned 100, register-masked 56), NOT kept: the callee then clobbers xmm7 as retail's does
// and the caller 0x2e9e0 goes 492 -> 573 (2232 bytes), a rise; diff in build/r73/lead-0x2f2d0-reference-first.patch.
// Next: re-measure 0x2e9e0's rejected forms on that callee. On it: only i, j or k ahead of the helper 182 each
// (800 bytes; k alone has the lowest masked count, 44), two of them 182; case 1's reference copy ahead of the helper
// too 177 (same code). With k ahead: y's terms written ijk/ikj/jik/jki/kij/kji 211/194/190/182/190/188 (768 to 800
// bytes: the first mention of the fields sets the term order). On the old body: x computed before y in cases 1 and 3
// 185 (800 bytes; frame 0x18 by pressure in case 1, where retail has none; 0x2e9e0 573); the result assigned without
// the cast 193 (same code); the helper's second cross written k, j, i 197 (788), both crosses k, j, i 288 (696);
// case 1's reference copy ahead of the helper 193. Diagnostics, reverted: one to twelve dead initialised locals
// ahead of the result 193 each; a volatile result 187 (aligned 123, masked 48).
// R229 (m3 lane 3, fast, 0x2e9e0 listed): modes 1 and 3 through one shared forced-inline projection helper taking the
// reference by pointer (perpendicular, then both dots; case 1 copies g_4b9dac first, case 3 computes the offset first,
// which then precedes the perpendicular naturally) 182 at 812 bytes: retail's entry (`sub esp, 0x18`, zero stored to
// [esp+8]), result slot and tail, the cases' XMM schedules still differ; but 0x2e9e0 492 -> 573 (2232 bytes), as with
// round 73's reordered case 3. The same with plain `inline` 182, with a shared mode 2/4 helper too 182 (same code). The
// helper taking the reference by value 184 (800 bytes, frame 0x20: case 3's offset is stored and copied through
// integer registers, which retail does not do; 0x2e9e0 stays 492), case 1 passing g_4b9dac by value 184: not kept, the
// parameter type would be picked by score. Round 524's shared 1/3 family measured 410; this helper is forced inline,
// keeps case 1's copy of g_4b9dac and computes the perpendicular itself. The plain-inline, shared 2/4 and by-value-global
// variants compiled to the same code as their siblings. Lead: the pointer helper is the natural form; it needs 0x2e9e0 reworked
// against a callee that clobbers XMM7 (its frame slots all move).
// R149 (m3 lane 7, fast with 0x2e9e0): R229's shared helper re-measured 182 (812 B) with 0x2e9e0 573 (2232 B), but
// 0x2e9e0's register-and-stack-masked aligned count falls 129 -> 103 on it (it stops holding a value in XMM7 across the
// call and keeps 0.0/1.0 in XMM5/XMM6 as retail does), so 0x2e9e0's 492 body was tuned to the wrong callee. Reworked
// against this callee with retail-evidence forms, 0x2e9e0 reaches 521 (see its note); the pair is not kept until 0x2e9e0
// is at or below 492.
// Round 84 (m3 lane 13, fast with 0x2e9e0; plain where marked): R229's shared forced-inline mode 1/3 projection helper
// re-measured 182 (0x2e9e0 573); case 3's offsets ahead of the perpendicular 177 (same 0x2e9e0 counts). On that callee
// 0x2e9e0 gets to 442 (448 plain, the file otherwise unchanged) only with its x/y scale held as one point2f, a
// match-only form (without it 517); see 0x2e9e0's note. Left here: the cross terms' schedule and the case 1 copy's place.
// The pair is a lead, not kept. New on the shared helper (0x2e9e0 573 unless noted): its dots through the existing dot3f
// 179 (aligned 95 -> 91), the same with cases 2 and 4 through dot3f, with either argument order, or with case 3's offset
// through vector3d_from_points3d 179 (same code); case 1 passing g_4b9dac itself 212 (with dot3f 204); x before y 185 (with
// dot3f 184); x before the perpendicular 189. On the dot3f helper: the reference by value 240 (0x2e9e0 stays 492), the
// perpendicular through two cross3f calls 182, through cross3f then cross3f_staged 182; with 0x2e9e0's 448 form 179/449.
// Those two 182s compile to the plain helper's code (the cross3f calls undo the dot3f effect). Also on the dot3f helper: x
// before y 184 is frame 0x18 (not R229's 0x20 by-value 184); the helper returning y with x computed by each case 186;
// x before the perpendicular 189 (800 bytes); same code as 179: `return 0.0f` in case 0 and default, `(real)atan2(y, x)
// * scale * ...`, only the first or only the second cross through cross3f, an unused caller-scope vector.
// m3 lane 8 r168 (`--fast 0x2f2d0 0x2e9e0 0x497a0`; 0x497a0's file defines the camera storage, which the reduced image otherwise
// leaves unresolved): diagnostic `#pragma optimize("p", on)` 183 (812 B; atan2 becomes a CRT call and the sums stay (i + k) + j);
// round 73's offsets-first case 3 re-measured 177 with 0x2e9e0 573; on it cases 1/3 y written ijk/ikj/jik/kij/kji 182/180/186/186/186
// (jki 177), x written ijk/ikj/jik/jki/kij 177 each (same code). Body unchanged.
// M3 lane 3 r243 (fast with 0x2e9e0, control 193/492): cases 2 and 4 with explicit `(k + j) + i` groupings 193 (other
// code, same count); case 1's dots read g_4b9dac directly with no reference copy 204 (776 B; 0x2e9e0 573, the callee
// then uses XMM7); case 1's sums grouped `(j + k) + i` and `(k + j) + i` 193 (other code); cases 2 and 4 computing x
// before y 193 (other code); case 3's offsets ahead of the perpendicular (the 177 lead, re-measured 177 at 812 B,
// 0x2e9e0 573) with the groupings of cases 1, 2 and 4, 179. Same code (193): cases 2 and 4 through dot3f; case 3 and
// case 4 offsets through vector3d_from_points3d. On the 177 lead the register-masked aligned count is 72: the cross
// terms' schedule still differs. Body unchanged.
// M3 lane 13 r97 (fast with 0x2e9e0; on round 73's case-3-offsets-first lead, re-measured 177 with 0x2e9e0 573 each): first
// cross written ikj/jik/jki/kij/kji 179/180/180/205/203; second cross ikj/jik/jki/kij/kji 163/174/175/185/160 (retail computes
// the second cross k, j, i); first-cross orders with the second ikj 165/167/162/191/196, with kji 164/165/167/192/199. A /GL
// link harness of this function alone (build/r97/h2) reproduces the real code exactly: by instruction position the lead and
// the k, j, i form are both 157-158 from retail, so their check scores differ mostly through the jump table's decode. Over
// all 216 first-cross x second-cross x case-1 y orders (case 1's x order is inert) the best case-1 block is 32 lines from
// retail with registers masked: no order fixes the schedule. Also `0.0f != y` 193 (lead 177) and the helper taking the up
// vector by pointer 193 (lead 177), both the same code.
// MiniMax C (11-15): no new form, no lower score (193): the frame is 0x14 against retail's 0x18, and the shared
// helper 0x2e9e0's frame is 0x90 against retail's 0x94, so both are one dword short; retail's frame here holds six
// dwords (the two fpatan operands, the result and the three components of the reference vector) where this build
// holds five, and retail keeps the result in the stack slot at +8 while this build keeps it in a register.
// MiniMax C (11-16): no new form tried this round, no lower score (193): the body is still 792 bytes against retail's 812, so a block is
// MiniMax C (11-16): still shorter here; the earlier rounds' forms are recorded above.
// M3 lane 4 r206 (fast with 0x2e9e0 and 0x497a0; control 193/492): `short mode` 189 (796 bytes: adds a `movsx ecx, cx`
// retail does not have, and mode moves to ECX; the caller already widens the field); `dword mode` 193 (same code); the
// result as `(real)atan2(y, x) * scale * ...` 193 (same code). On round 73's case-3-offsets-first lead (re-measured 177
// at 812 bytes, 0x2e9e0 573 at 2232 bytes, frame 0x90 against 0x94): 0x2e9e0's far distance read at each use 177/573
// (same code); with 0x2e9e0's `!(visibility > 0.0f)` (retail's `comiss xmm0, xmm6; jbe`) 177/572. Body unchanged.
// MiniMax S (11-17): no new form, no lower score (193): the frame is still 0x14 against retail's 0x18 (retail
// holds six dwords where this build holds five, the two fpatan operands, the result and the three components
// of the reference vector) and the result still lives in a register where retail keeps it in the stack slot at
// +8; the lead that reaches 177 (case 3's offsets ahead of the perpendicular) needs the shared helper 0x2e9e0
// reworked against a callee that clobbers XMM7, and that helper is not in this lane's queue, so the pair stays
// as it is.
// Frame 0x14 versus 0x18 and the independent angle result slot still differ. Case-one component stores into
// the reference 214, scalar reference 201, scoped const reference 180; scalar offsets in case three 179
// and case four 257, scalar negation in case two 257, all scalar references 201, complete fixed-axis copies 366.
// NOT kept: 179 and 180 restore the frame and length but raise the caller from 492 to 572.
// Result zero used for projection admission 193; named scaled angle 193; entry double scale 167, with
// named scaled angle 167 (NOT kept: qword spill and 0x20 frame against 0x18); zero projections for
// default cases 194; split case-1 reference 220; case-3 Z first on the original projection order 182
// (NOT kept: caller 492 to 572); both reference splits 220; entry real scale 193. The result still
// shares the scale argument's home.
// N404: Both reference sources produced inside the perpendicular helper after its first cross
// component, between crosses, or after the first or second perpendicular component: scalar
// displacement stores 312/312/581/581, vector setter 312/312/316/316. Four forced-inline
// counterparts reach 178 at 812 bytes with frame 0x18 and result at +8. NOT kept: the caller
// measures 572 against 492, and the cross and displacement still have different XMM allocation.
// Retail has a 0x18-byte frame and a separate result at +8. Earlier position loads alone do not create
// the XMM pressure from earlier displacement computations. The caller stays at 492. fixed reference
// component setter 214; negated reference component setter 193; both displacement component setters 193;
// all reference component setters 214; read position x before cross 193; read position y before cross
// 193; read position z before cross 193; read position components before cross 193. NOT kept: tied or
// higher scores, with no result-slot recovery.
// Camera-up input to the perpendicular as a value local, component aggregate,
// by-value input, or per-mode copy passed by pointer. Original
// displacements / complete origin capture: 421/377, 192/214, 206/279, 206/279.
// NOT kept: the component aggregate leaves the 0x14 frame and gains one. The
// value local gets a separate result at +8 in a larger frame and raises the caller.
// Forward projection before the perpendicular, case one/three/both, with its sign after the
// vertical projection 197/177/186 or before it 184/176/184. Early projections with a common
// sign at dispatch exit 392, sign at inverse tangent only 206. NOT kept: 177 and 176 recover
// the result home and length but compute the projection before retail and raise the caller
// 492 to 572. Other gains shorten the body. The 0x14 frame still lacks the separate result.
// Separate perpendicular receivers 193; fixed-axis receiver scopes 193; direction-relative receiver scopes 180;
// third-case receivers 195; displacement aggregate in both cases 266 or before the third-case perpendicular 171;
// fixed-axis aggregates 193; reversed direction aggregate alone 257.
// NOT kept: the 180 and 171 forms reach frame 0x18 and 812 bytes but raise the caller from 492 to 572.
// The earlier aggregate advances both X and Y ahead of the retail interleaving.
// MiniMax A (11-18): no new form, no lower score (193): the frame is still 0x14 against retail's 0x18, and retail keeps
// the result in its own slot at +8, set to 0.0f in the prologue, where this build reuses the scale argument's home; the
// case-one body still differs in its float order and in how retail loads the two components of the 0x4b9dac vector one
// at a time and copies them through the stack, and restructuring the switch into a fall-through form measures 194.
// Round 144 (m3 lane 12, fast with 0x2e9e0 and 0x497a0; control 193/492): round 73's case-3-offsets-first lead
// re-measured 177 (812 B) with 0x2e9e0 572 (2232 B; same calls, the change is register allocation around the callee that
// now clobbers XMM7, as retail's does); on it the second cross written k, j, i 160 (0x2e9e0 572); with that, case 1/3's y
// terms i, j, k 163, k, j, i 172 (816 B); case 1 reading g_4b9dac directly with no reference copy 196 (784 B). The pair
// stays BLOCKED-BY 0x2e9e0: it has to be reworked against the XMM7-clobbering callee first. Body unchanged.
// M3 lane 14 r121 (plain with 0x2e9e0): kept, 193 -> 177 at retail's 812 bytes: case 3's offsets ahead of the
// perpendicular (round 73's lead; retail computes the k offset at 0x2f456, ahead of both cross products), now that
// 0x2e9e0 is reworked against this callee (492 -> 420, see its note). Also measured on it: the parameters as (position,
// direction, mode, scale), the order retail's call sets them up in (scale pushed, mode, direction, position), with
// the call changed too 177 (0x2e9e0 420, the same counts; its two `lea`s then come in retail's order): not kept, a tie.
// MiniMax C (11-20): no new form, no lower score (142): the x sum operand order changes nothing and the y sum orders score worse.
// @retail 0x2f2d0
real function_2f2d0(vector3f const *direction, point3f const *position, long mode, real scale)
{
    real result = 0.0f;
    real y;
    real x;
    vector3f perpendicular;
    vector3f reference;
    switch (mode)
    {
    case 0:
        return result;
    case 1:
        direction_perpendicular_to_up(direction, &perpendicular);
        reference = g_4b9dac;
        y = reference.j * perpendicular.j + reference.k * perpendicular.k;
        y += perpendicular.i * reference.i;
        x = direction->k * reference.k + direction->j * reference.j;
        x += direction->i * reference.i;
        x = 0.0f - x;
        break;
    case 2:
        reference.i = 0.0f - direction->i;
        reference.j = 0.0f - direction->j;
        reference.k = 0.0f - direction->k;
        y = g_4b9e4c.k * reference.k + g_4b9e4c.j * reference.j;
        y += g_4b9e4c.i * reference.i;
        x = g_4b9e64.k * reference.k + g_4b9e64.j * reference.j;
        x += g_4b9e64.i * reference.i;
        x = 0.0f - x;
        break;
    case 3:
        reference.k = position->z - g_4b9da0.z;
        reference.i = position->x - g_4b9da0.x;
        reference.j = position->y - g_4b9da0.y;
        direction_perpendicular_to_up(direction, &perpendicular);
        y = reference.j * perpendicular.j + reference.k * perpendicular.k;
        y += perpendicular.i * reference.i;
        x = direction->k * reference.k + direction->j * reference.j;
        x += direction->i * reference.i;
        x = 0.0f - x;
        break;
    case 4:
        reference.i = position->x - g_4b9da0.x;
        reference.j = position->y - g_4b9da0.y;
        reference.k = position->z - g_4b9da0.z;
        y = g_4b9e4c.k * reference.k + g_4b9e4c.j * reference.j;
        y += g_4b9e4c.i * reference.i;
        x = g_4b9e64.k * reference.k + g_4b9e64.j * reference.j;
        x += g_4b9e64.i * reference.i;
        x = 0.0f - x;
        break;
    default:
        return result;
    }
    if (y != 0.0f)
        result = (real)(atan2(y, x) * scale * 0.31830987334251404f);
    return result;
}

extern real g_5234c8;
byte g_4c19c0, g_55e6c6;
struct s_2e9e0_saved { long tag; point3f position; };
s_2e9e0_saved g_4c19c4[4];
dword g_4c1a04;
real function_020d80(s_slot *slot, bool skip);
struct s_tag_data;
real function_13bb90(s_tag_data const *curve, real input, real range);
void function_13be80(s_tag_data const *curve, real input, color3f *color);
bool function_47fd0(long tag, short stage, short fallback, byte flags);
void __stdcall function_480a0(point3f const *point, real width, real height, real cosine, real sine, dword color);

PRIVATE inline real clamp_2e9e0(real value)
{
    real result = value;
    if (value < 0.0f) result = 0.0f;
    else if (value > 1.0f) result = 1.0f;
    return result;
}

real game_time_get_seconds();
void function_0222d0(D3DRENDERSTATETYPE state, dword value);

// MiniMax E (11-20): no new form, no lower score (281): the opacity branch's polarity, the angle and axis_scale order and the delta order each lower the score (275 together), but they are reorderings, kept only with a byte match, so they are not kept; what is left is retail's register-first multiply and its source order for the independent float statements, which the source does not steer.
// score 281 (was 281): The 0x94 frame and decoder output homes agree. The first difference at +0x76 is the tag-
// rejection branch displacement. The first differing computation is the camera displacement. Retail starts with
// position X in XMM1 and takes its Y-axis load before computing the Y and Z deltas. This build loads the
// forward-axis terms first and puts the deltas in different registers. The trials separate the camera-origin
// and projected-vector lifetimes, leaving the decoder and graphics calls unchanged.
// Earlier at 281 (was 281): The frame and output homes agree. The delta/forward schedule, draw-point X load, and loop XMM assignments still differ.
// Earlier (417, was 492): frame 0x94 and every stack offset as retail; left: the delta/forward/reflected schedule, the
// product operand sides, the loop's XMM numbering (lead at the end of this note).
// MiniMax I (11-19): no new form, no lower score (492): the body is retail's 2285 bytes but the frame is still 0x90 against retail's 0x94, so the first float lands at +0xc where retail puts it at +0x10 and the tag comes in at +0x48 where retail has +0x30, and the tag, position, direction, colour, alpha, amount and scale slots stay in another order.
// Earlier (492, was 492): Frame 0x90 versus 0x94. The draw point shares the decoded direction's slot.
// Earlier 417 (was 417; 492 before lane 14 r121): frame 0x94 and every stack offset as retail; left: the delta/forward/reflected schedule, the
// Earlier score 417 (was 417; 492 before lane 14 r121): frame 0x94 and every stack offset as retail; left: the delta/forward/reflected schedule, the
// Earlier comparison 417 (was 417; 492 before lane 14 r121): frame 0x94 and every stack offset as retail; left: the delta/forward/reflected schedule, the
// Earlier 492 (was 492): Frame 0x90 versus 0x94. The draw point shares the decoded direction's slot.
// MiniMax S (11-17): no new form, no lower score (492): the body is retail's 2285 bytes but the frame is still
// 0x90 against retail's 0x94, so the first float lands at +0xc where retail puts it at +0x10, the tag comes in at
// +0x48 where retail has +0x30, and the tag, position, direction, colour, alpha, amount and scale slots stay in
// another order; no source form tried here moves them.
// MiniMax R (11-16): no new form, no lower score (496 in this round's nine-address set, 492 in the single-function set): the body is retail's 2285 bytes but the frame is still 0x90 against retail's 0x94, so the first float lands at +0xc where retail puts it at +0x10, the tag comes in at +0x48 where retail has +0x30, and the tag, position, direction, colour, alpha, amount and scale slots stay in another order; no form tried this round moves them.
// MiniMax C (11-16): no new form, no lower score (492): the frame is still 0x90 against retail's 0x94, so the tag, position, direction, colour, alpha, amount and scale slots stay in another order; tried the context alpha before the time (526), the context alpha declared without its initialiser (492), the visibility after the context alpha and the selected colour at function scope (533, a scope move and so reverted), and none of them moves the slots.
// MiniMax R (11-16): no new form, no lower score (492): the body is retail's 2285 bytes; the frame is still 0x90 against retail's 0x94, so the ratio, the definition, the source, the scale and the vertex homes sit four bytes below retail's, and the tag, position, direction, colour, alpha, amount and scale slots are in another order. The permuter over this body (600 s, twelve tries) found no form below the baseline.
// Earlier 492 (was 492), 492 (was 507): every D3D call here in the D3DDevice:: member form (retail inlines the vertex push-buffer
// write, 0x2f17c-0x2f1ba; only the member form inlines it) and clamp_2e9e0 as an initialised single-exit result:
// retail length, EBX definition and EBP NONE as retail. Frame 0x90 versus 0x94, the 12-byte output homes (ours
// direction below position, retail position below direction) and the loop's xmm5/xmm6 constants remain.
// MiniMax S (11-14): no new form, no lower score (496): the body is retail's length (2285) and the first difference is at +0x2, but the frame is 0x90 against retail's 0x94, so the ratio, the definition, the source, the scale and the vertex homes sit four bytes below retail's and the sentinel stays in ebx where retail keeps it in ebp; the length and the register plan have to change together, and nothing tried this round moved them.
// MiniMax B (11-13): no new form, no lower score (492): the frame is 0x90 against retail's 0x94 and the x87 block's locals land in the other slots (retail's ratio is at esp+0x10 where this build has it at esp+0xc), so the body's shape is different from the first block.
// MiniMax I (11-11): no new form, no lower score (492): the body is retail's length (2285) and the first difference is at +0x2; the body is tuned against the callee 0x2f2d0 calls, and the form that would take 0x2f2d0 to 177 raises this one to 573.
// MiniMax G (11-12): no new form, no lower score (492): the frame is still 0x90 against retail's 0x94, so the definition, source,
// scale and vertex homes sit four bytes below retail's, and retail's tag, position, direction, colour, alpha, amount and scale
// slots are in another order; the permuter's search over this body (full image, annealed) found no better form.
// MiniMax H (11-06): 507 (was 507): no new form, no lower score.
// MiniMax E (11-09): no new form, no lower score (507).
// MiniMax H (11-06): score 507 (was 507): no new form, no lower score.
// The argument order into function_480a0, the field offsets, the constants and the loop shape all agree with
// retail; what is left is the frame (0x90 against retail's 0x94), the sentinel register (we keep the -1 in
// ebx and retail keeps it in ebp, so the saved-register layout differs) and the stack homes of the sentinel
// and of the definition pointer.
// Earlier 507 (was 507): Frame 0x90 versus 0x94, output homes and definition/NONE registers remain.
// MiniMax R (11-01): no new form, no lower score.
// Earlier (507 (was 508, 594 originally)): draft passed (cosine * size, sine * size, x_scale, y_scale) to function_480a0; retail passes
// (x_scale * size, y_scale * size, cosine, sine) (0x2f209-0x2f247: edx=sine and [0x2c]=cosine pushed as args 4 and 5, the
// scales multiplied by size into the two float args). Checked against retail block by block (0x2e9e0-0x2f2ca, 2026-10-31): every
// field offset and size, condition direction, constant (0x45dbc0 1.0, 0x45dbbc 0.5, 0x45dc0c 2.0, 0x45dc54 pi/180, 0x45dddc pi,
// 0x45dfa0 1/256), the 2e3f0 outputs, the 163ba0/13bb90/13b390/13be80/2f2d0/47fd0/131f40/480a0 argument orders and the loop shape
// agree; the only other mismatches found are the slot flags read (now a dword like retail's 0x2eaa9 and 0x20d80) and the order of
// the time/hash sum (retail h0*k, + time, + h1 at 0x2ecd6-0x2ed1f; the form below emits that order, the old one emitted h1 first).
// Retail also re-reads the entry flags from memory after function_47fd0
// (test byte [edi], 8), recomputes the opacity expression in each PIN arm, reads far_angle straight from the definition, and
// loops with the count guard kept. What still differs: retail inlines D3DDevice_SetVertexData2f (0x2f17c-0x2f1ba) but the reduced
// image calls it (full build may differ); ebx and ebp swapped (retail definition in ebx, NONE/time in ebp); stack slot order and
// frame (0x90 against 0x94; retail puts alpha at +0x28, tag +0x30, position +0x40, direction +0x4c, amount +0x6c, scale +0x84,
// color +0x88 and shares +0x14 between time and size); the camera angle sum is emitted k, j, i (retail j, i, k at 0x2ed71-0x2edbb)
// whatever order the terms are written in (3 orders, 507); 0.0/1.0 are reloaded instead of kept in xmm5/xmm6.
// Tried: far_distance read in place (552), all clamps as PIN (543), declaration order of the out locals (no change),
// attenuation hoisted together with animated (576), several spellings of the hash sum (548), the flags dword kept in a local
// (590), camera sums as separate double locals (no change). Round 38: the time is the existing game_time_get_seconds
// (0x146630, the same code; 507). The `D3DDevice::SetVertexData2f` member form makes LTCG inline the push-buffer write
// as retail does (2285 bytes = retail's, definition in ebx and NONE in ebp as retail) but scores 512 because the stack
// slots still differ (retail's frame is 0x94; aligned differences 439 -> 370); `if (!(visibility > 0.0f))` gives
// retail's comparison order (506; 511 with the member form).
// J225: point-difference helper 507; projection dot helper 507; scalar-output role names 507; vector-output
// role names 507; all output role names 507; role names plus member vertex API 512; entry flags read
// directly 507; sample directly into animated color 580; whole-color copy then tint 527; color copy plus
// member vertex API 524. Original body restored.
// Round 344: native tag/count/visibility admission subsets 525/507/525/507/525/507/507;
// all admissions with the member vertex API 512 (2285 bytes). Original body restored;
// frame 0x90 versus 0x94, tag/output homes and EBX/EBP roles remain.
// J238: Reflection setter/scale pipeline, paired trigonometric admission, native point setter
// 578/518/507; reflection/trigonometric, reflection/point, trigonometric/point, all 593/578/518/593.
// Complete graph with member vertex API 542 at retail length; reflection/point/member with original
// zero tests 526. All restored; frame 0x90 versus 0x94 and output/definition/NONE homes remain.
// F321: entry-owned hash/sample/draw scratch subsets 1/2/4/3/5/6/7 give
// 507/507/530/507/530/530/530; all with the member vertex API 535 (2308 bytes).
// Original executable body restored; frame/output homes and EBX/EBP roles remain.
// Round 369: native external definition/entry/const receiver subsets 1/2/4/3/5/6/7,
// 507 each; complete native graph with member vertex API 512 at retail length.
// Original receivers, API and whole body restored; frame/output homes remain.
// F334: Existing clamp helper initialized-result/early-return/parameter-clamp/completed-
// branch-result graphs: 567/507/581/567; with the member vertex API 492/512/533/494.
// Initialized result plus member API 492 (2285/2285 bytes) NOT kept: the clamp graph alone
// regresses (567) and the gain needs the member vertex call beside this body's C-style
// D3DDevice_ calls, a combination picked by score. Body restored at 507; frame and output homes remain.
// Y538: Existing member depth-state API at both/enable/disable sites 507/507/507;
// with member vertex command 512/512/512; full member graph with opacity arms 511,
// paired trigonometric admission 529. All restored; frame 0x90 versus 0x94 and output homes remain.
// Round 376: Twice-used converted angle at intrinsic/shader/point/angle phases,
// paired real/double trigonometric results 508/552, 510/579, 510/579, 511/579.
// Earlier real phases reach retail length but frame 0x9c; extended results add 0x9c or aligned
// 0xac frames. Original float-angle observations, all output scopes, APIs and body restored;
// frame 0x90 versus 0x94 and definition/NONE/output homes remain.
// Lane 12 r86: the camera sums follow the first mention of delta's fields: delta assigned k, i, j emits
// retail's j, i, k order (507; product operands still global-first where retail loads delta.i/delta.k
// first); with the j product's cast on the global 507 (same code). Also tried: `color` renamed 507
// (no slot moves); camera sums through dot3f 507; their terms respelled k, i, j 507 (same code).
// B409: Full sampled-color field/library/overlap-safe transfers, then saved-position
// transfers and paired field/library forms: 543/507/555/497/507/515/534/507.
// NOT kept: saved position fields at 497 emit SSE transfers instead of native integer-word capture;
// frame 0x90 versus 0x94 and output homes remain. Original complete body restored.
// Y571: Shared exit, twice-used saved-entry index and draw displacement, seven subsets
// 507/505/531/505/531/520/520; complete unsigned displacement 520. Index plus displacement
// reaches retail frame 0x94 and 2285-byte extent. NOT kept: index-only two-point lead below gate;
// native-frame graphs regress. Complete original body restored; output and definition/NONE homes remain.
// F366: Complete reflection/displacement/both product parts, size/opacity/both spans,
// distance ratio parts and angular normalization parts: 539/507/539/507/572/572/507/507.
// All restored; native 0x94 frame and output homes remain, without altered arithmetic grouping.
// F370: Complete distance/angular/source-visible attenuation result subsets
// 1/2/4/3/5/6/7 give 559/522/509/559/564/526/564; all with complete context-alpha
// arms 556. NOT kept: all regress; native frame/output homes remain. Original
// scalar results, observations, API and full body restored.
// F372: Internal hash, animation/sample and draw workspace role names, seven
// subsets then all with offset/reflection names: eight at 507. Native frame and
// output homes remain; original names, scopes, APIs and complete body restored.
// N385: Coherent forward-component, three-use far-angle and paired camera-axis snapshots,
// seven subsets, then complete inputs with the native forward vector value: 586/537/583/507/586/537/583/519.
// NOT kept: no qualifying gain or equality; all scalar/copy input forms and complete body restored.
// Native frame 0x94, addressed outputs and definition/NONE homes remain.
// F390: Actual position/direction/color eight-byte alignments, seven subsets, then sampled-color
// alignment: 584/584/594/564/567/567/568/568. NOT kept: each introduces a frame-pointer prologue
// absent from retail; complete original output alignments, API and body restored. Native
// frame/output homes remain.
// B440: Plain existing component-array loops for camera displacement/reflection/
// draw position, seven subsets then unsigned ordinals: 708/716/619/920/708/716/1069/1069.
// NOT kept: all regress and grow; native frame/output homes remain. Original full body restored.
// B442: Complete selected-color snapshot at iteration/after-opacity/admission/after-angle,
// native initialization 519/519/524/524; library copy 519/519/524/524. NOT kept:
// no native frame/home recovery or gain. Original color producers and full body restored.
// P342: Complete distance/angular endpoint observations and branch-produced animation-time results,
// seven subsets then immutable original clock: 547/560/533/547/539/542/539/539. NOT kept: all regress;
// native 0x94 frame, output homes and definition/NONE roles remain. Original field reads, time updates,
// vector transfers, APIs and whole body restored.
// P347: Complete first/subsequent-entry scale/angle/both/both-with-predicate outcomes, first/subsequent arm priority:
// 537/512/535/535/537/512/535/535. NOT kept: All eight regress and retain frame 0x90 versus native 0x94. Original scalar producers, tint work, APIs and complete body restored.
// N385: Coherent forward-component, three-use far-angle and paired camera-axis snapshots,
// seven subsets, then complete inputs with the native forward vector value: 586/537/583/507/586/537/583/519.
// NOT kept: no qualifying gain or equality; all scalar/copy input forms and complete body restored.
// Native frame 0x94, addressed outputs and definition/NONE homes remain.
// J261: Existing angle provider inline/forced 610/1450; packed-color provider 507/627; sampled-color provider
// 507/569; scalar-curve provider 507/659. Packed-color inline worsens another user, forced expansion loses
// 0x26520 and 0x2e230; forced scalar curve loses 0x134fe0. All independent annotations restored repo-wide.
// NOT kept: no eligible native repair; opening executable source restored. Native frame 0x94 versus 0x90 and
// output/NONE homes remain unresolved.
// B451: Complete animation-channel scalar receivers at original/reflection/
// attenuation/time/admitted/angle/curve phases, then separate default assignment:
// 522/516/544/528/536/536/536/522. NOT kept: no exact output-storage graph. Original color record,
// producers, APIs and full body restored; frame/output/definition/NONE homes remain.
// Round 64 (lane 15, fast unless marked plain): member vertex call alone 512 (2285 bytes); all D3D calls as members
// 512; all members with clamp_2e9e0 as an initialised result 492 plain (kept), as a completed-branch result 494; the
// initialised clamp alone 567 (2240 bytes). `real attenuation = 1.0f;` declared before `reflected` (retail stores its
// 1.0 right after the delta): with the members 464 plain (2310 bytes, aligned 439 -> 336; not kept, a moved
// declaration), with the C-style calls 530, on the kept form 549; on the 464 form: attenuation before the delta 564,
// beside `animated` at the top 564, `animated` after the delta 472, after the forward dot 472, both after the delta
// 564; `reflected` assigned j, i, k 512; `!(visibility > 0.0f)` 463.
// R223 (lane 3, fast, control 492; second and third numbers are an aligned count and one with stack offsets masked, from
// build/r223/al.py, control 285 and 201: the positional score moves little while the frame is 0x90). Retail shows: (a) the
// visibility test as `!(visibility > 0.0f)`; (b) the context alpha and the vertex-data value clamped by the PIN macro's store
// form, the other four clamps by the helper; (c) the sampled colour copied into the animated one field by field, and each
// entry's colour started as an aggregate copy of the animated colour and then multiplied by the entry's fields; (d) the far
// distance read from the definition at each use; (e) the opacity factor as an if/else; (f) attenuation's 1.0 stored beside the
// animated colour's; (g) all four dot products in one shape (j term first, global on the left). (b) on the context alpha 520,
// 267, 198; (a) 491, 283, 199; both 519, 265, 196; (b) with a named product 560, 283, 212; (c) sampled copy 523, 279, 190;
// (c) entry colour 547 (2302 bytes), 264, 190, declared ahead of the angle 546, 282, 204; (b) at the vertex call 494, 284,
// 199; (a)+(b)+(c) 555 (2322), 224, 154, with the copy at the old place 552, 227, 165, without (a) 556, 226, 156; plus (d) 560,
// 233, 157; plus (f) (a moved declaration, diagnostic) 579 (2315), 242, 150; (a) to (e) 553 (2320), 233, 154; on that, forward,
// projection or the camera sums through dot3f in either argument order 553 each (same code, so (g) is not a spelling).
// Diagnostic only: unused initialised locals ahead of the delta move the whole layout (558 at 2297 bytes, masked 149 to 144),
// so the remaining order differences follow the count of locals, not the expressions. Nothing kept: no form is 8 below 492.
// Round 84 (m3 lane 11, fast, control 492; second and third numbers are LCS-aligned unmatched lines, plain and with
// stack offsets masked, build/r84/aligned.py, control 296 and 215): the entry table pointer read once before the loop
// 543 (2302 bytes), 284, 198; an entry pointer advanced in the for statement 546 (2268 bytes), 290, 209; the opacity
// through clamp_2e9e0 in place of PIN 562 (2246 bytes), 285, 200; the flags read once as a word value 483 (2295
// bytes), 347, 261; x_scale and y_scale set in an else arm in place of their 1.0 initialisers 461 (2285 bytes), 298,
// 219: NOT kept, the positional gain is realignment only (aligned counts rise), and retail stores both 1.0 defaults
// before the `i == 0` branch (0x2ef25-0x2ef2b), which is the original form. On that form (diagnostic) the visibility,
// context alpha or attenuation default in an else arm 475/467/528, the flags as a value 542.
// MiniMax I (11-10): no new form, no lower score (492); the frame is still 0x90 against retail's 0x94, the definition and NONE homes still swap, and the output homes still differ.
// R149 (m3 lane 7, fast; callee 0x2f2d0 in its shared mode 1/3 helper form, which uses XMM7 as retail's does; register-
// and stack-masked aligned counts in brackets, 103 on that callee against 129 on main's): that callee alone 573 (2232 B);
// `!(visibility > 0.0f)` 572; context alpha by PIN 562; vertex data by PIN 573; opacity factor as if/else 582; the sampled
// colour copied field by field 552 (2248 B); each entry's colour as an aggregate copy of `animated`, then multiplied 556
// (2256 B); both copies 534 at 2285 B [111]; + context alpha by PIN 524 [106]; + the visibility test 523 [105]; + vertex
// data by PIN 524 [103]; all six 526 [101]; on 523: the entry colour copy first in its block (retail copies it before the
// angle) 523 [92]; delta assigned k, i, j 524; on that, `attenuation = 1.0f` declared beside `animated` (retail stores its
// 1.0 there) 521 (2292 B) [81], declared before `reflected` 546 (2308 B) [76]. Left: frame 0x90 against 0x94 and the slot
// order (after the three entry pushes retail has alpha 0x28, tag 0x30, position 0x40, colour 0x4c, amount 0x6c, scale
// 0x84, direction 0x88; ours alpha 0x24, colour 0x30, tag 0x40, position 0x4c, scale 0x88, amount 0x8c, direction 0x90),
// and the camera sums' term order. Lead: rework the frame on that callee, then keep both functions together.
// Lane 14 r81 (fast, control 492 as the full check; second number the register-masked aligned count, 237 for 492):
// the draw point through the existing vector3f_add_scaled 492 (same code); the point started as a copy of the
// position and moved with `+=` 546 (2302 bytes, 248); the point declared at function scope 492 (same code: retail's
// point keeps its own frame-top slot, ours shares the direction's); delta through the existing vector3d_from_points3d
// 492 (229: the reflected rows then store j at retail's slot); that with `!(visibility > 0.0f)` 491 (228).
// Round 111 (m3 lane 9, fast with the callees, control 492 = full): x_scale/y_scale as one `point2f scale` (the file's
// existing 2-D type, as function_2afa0's scale pair; defaults by initialiser or member stores, same code) 533 at 2270 bytes with
// retail's frame 0x94 (retail keeps the pair in the dead direction slot and gives the draw point a fresh frame-top slot;
// register-masked aligned 244 -> 212); an aggregate copy of the definition pair 541 (2262 bytes); with the context alpha
// through PIN in place of clamp_2e9e0 (retail's store form) 466 at 2285 bytes (plain too), NOT kept: a match-only lead, since each
// half alone raises the count (PIN alone 520) and the pair only moves stack homes. Re-measured on that 466 form's 0x94 frame (R223's (a), (b), (c), (e), (g) and listed forms): `!(visibility >
// 0.0f)` 465; the vertex-data value through PIN 468; both 467; the sampled colour copied field by field 510 (2288 bytes); delta
// through vector3d_from_points3d 467; far distance read at each use 511; the opacity factor as if/else 468; the flags read once
// as a word value 469 (2301 bytes); the projection dot j term first 466 (same code); each entry colour started as a copy of the
// animated colour and multiplied by the entry's fields 537 (2308 bytes; three spellings, same code). New on the 466 form:
// `scale.x = scale.y = 1.0f;` 466; the angle attenuation's near angle read at each use 466; the animated colour defaults as
// one chained member assignment 466 (all three different code); the clamp, angle-clamp, curve and opacity products written
// factor first (`attenuation = clamp_2e9e0(alpha) * attenuation` ...) 466 each, same code. Renaming direction moves no slot.
// Left: the locals' frame order (alpha, tag, visibility, position, direction in retail; ours alpha, direction, tag,
// visibility, position), the loop's 0.0/1.0/amount registers.
// Round 84 (m3 lane 13, fast unless marked plain; a/m = LCS-aligned / register-masked unmatched lines, control a279
// m179): both depth-state sites call the existing 0x222d0 (g_4b8438 is its cache entry g_4b81fc[D3DRS_ZENABLE]) in
// place of its body written out: 492, the cache store now follows the push-buffer load as in retail (a278; kept, plain
// 492, 0x222d0 and its 16 callers unchanged). With 0x2f2d0 on R229's shared mode 1/3 projection helper (XMM7 in use as
// in retail), re-measured: R149's stack (sampled colour copied field by field, entry colour copied from `animated` and
// then multiplied, context alpha through PIN, `!(visibility > 0.0f)`, entry colour first in its block) 523 (a220 m146);
// plus lane 9's point2f x/y scale pair 499 (frame 0x94 as retail, first difference +0x44, a167 m82); plus the far distance
// read from the definition at each use (retail reads +0x1c three times) 442 (a166 m78); without the entry-colour move
// 448 (plain 448, callee 182); without the point2f pair 517, so the gain is match-only and nothing of it is kept. On the
// 448 form: delta through vector3d_from_points3d 453; the camera sums' j term global-first 448 (same code); with the
// 0x222d0 calls 449; the attenuation default beside `animated` (point2f base, without the far reads; a moved
// declaration, measured only) 521 (2298 bytes). Without the pair: with the 0x222d0 calls 517; x_scale/y_scale declared
// ahead of the loop 517 (same code). Left on 448: the scale output's home (ours +0x68, retail +0x84), the delta, forward
// and reflected schedule (retail computes delta i, j, k, then reflected j, i, k), the attenuation default reloaded in the
// else path, and the camera sums' operand order. On 448: the distance attenuation through PIN 563 (2330 bytes), the
// angular attenuation through PIN 531 (2314 bytes); with 0x2f2d0's dots through dot3f 449. Without the pair, ours keeps
// x_scale and y_scale in unrelated slots (+0x28/+0x5c) where retail has them side by side in the dead direction slot.
// T90: Clamp independent bounds, upper-first result, evolving-result tests and upper-first
// conditional; complete visibility/alpha outcomes, full-capacity arm first, separate count publication: 529/591/533/492/505/498/506/492.
// NOT kept: No gain; frame 0x90 versus 0x94 and addressed-output homes remain. Original clamp and entire body restored.
// T95: Named original intensity/color-input/turn curve results at their calls,
// subsets 1/2/4/3/5/6/7, then entry-declared destinations: 492/492/492/492/492/492/492/492.
// NOT kept: one-use call-result storage misses equality; original expressions,
// all call tuples and complete body restored; frame 0x90/0x94 remains.
// MiniMax B (11-13): no new form, no lower score (492): the frame is still 0x90 against retail's 0x94 and the scale output's home is still at +0x68 against retail's +0x84; retail computes delta i, j, k and then reflected j, i, k.
// MiniMax G (11-13): no new form, no lower score (492): the frame is still 0x90 against retail's 0x94, and the
// vertex data call is expanded here where retail calls it; the member form reaches retail's length but scores
// worse, so the body stands.
// P461: Shared zero-angle outcome bool/char/signed-char/byte/short/word/long/dword:
// 528 each, 2267/2285 bytes, frame 0x90. NOT kept: no exact repeated-test/output-home
// repair; original independent trig producers, APIs and complete body restored.
// P489: Actual output alpha/amount/scale/all-scalars/position/direction/color/all defaults:
// 515/514/518/555/551/524/498/604. NOT kept: no exact native output-storage repair.
// Original uninitialized outputs, provider contract, APIs and complete body restored.
// Frame 0x90 versus 0x94 and output homes remain.
// P499: Actual distance/angular/optical last-use blocks, seven subsets, then complete bounded-fade phase:
// 492/492/492/492/492/492/492/492. NOT kept: All eight tie 492 at the original extent and frame; native output
// homes remain. Original complete executable body restored.
// M3 lane 12 r126 (fast with 0x2f2d0; control 492, register-masked aligned 242): delta through the existing
// scaled_difference3f(camera, position, 1.0f, delta) 540 [244]; reflected through the existing vector3f_scale of the
// camera forward by `forward`, then scaled_difference3f(delta, projected, 2.0f) 559 [234]; the depth-test toggle as one
// comparison (`bool depth_test = (*flags & 8) != 0; if (depth_test != g_4c19c0)`, one 0x222d0 call with `depth_test ? 2 :
// 0`) 690 (2127 bytes: one inlined 0x222d0 copy where retail has two); the unoccluded alpha's product operands swapped 492
// (same code: retail loads the context alpha from its home and multiplies by the attenuation register, ours copies the
// attenuation and folds the home). Retail reloads 0.0/1.0 into XMM6/XMM5 at each iteration's top (0x2ee49) and keeps the
// attenuation and the size in stack homes. Body unchanged.
// MiniMax E (11-15): no new form, no lower score (492): our body is exactly retail's 2285 bytes, but our frame is 0x90 against retail's 0x94 and the locals sit in different slots (retail keeps the frame time at +0x10 and the tag at +0x30, this build keeps them at +0xc and +0x48), so the local set and order differ; the permuter refuses the reduced image here (526 against the full image's 492) and a full image try costs about 100 s.
// M3 lane 1 r109 (fast with 0x2f2d0, 0x2e3f0 and 0x222d0, control 492 = full): x_scale/y_scale declared with their 1.0
// defaults at the top of the loop body 522; the draw point through the existing real_point3d_set 492 (same code); `time`
// renamed to sort ahead of the context alpha 492 (same code: its home is a spill slot, the name does not order it); lane 9's
// point2f scale pair re-measured on this body 533 (2270 B, frame 0x94, first difference +0x16); `delta` as an aggregate
// initialiser 492 (same code). Slot map (frame offsets after `sub esp`): retail context alpha +0, time +4, selected
// colour +0xc, alpha +0x1c, i +0x20, tag +0x24, visibility +0x30, position +0x34, direction +0x40 (x_scale/y_scale reuse
// it at +0x40/+0x44), animated +0x50, amount +0x60, scale +0x78, colour +0x7c, draw point +0x88; ours time +0, context
// alpha +4, alpha +0xc, direction +0x24 (shared with the draw point), tag +0x3c, visibility +0x40, position +0x44, amount
// +0x68, scale +0x7c, colour +0x84, with x_scale/y_scale in alpha's dead slot and +0x50.
// M3 lane 16 r129 (fast with 0x2f2d0 and 0x2e3f0; control 492): the loop index declared at function scope 492 and the
// projection dot ahead of the inverse range 492 (both same code: the slots follow live ranges, not scopes); re-measured
// on this base, the far distance read at each use 532 and the flags read once as a word value 504 (2293 B). Retail
// keeps the index in a stack home and reloads it at the loop top into the entry register because its `selected =
// animated` integer copy at the top of the block takes ECX/EDX; ours keeps the index in ECX. New combinations of
// retail's forms: the sampled colour copied field by field with each entry colour started as a copy of `animated` at
// the top of the block and multiplied in place 568 (2322 B, frame still 0x90); that with `!(visibility > 0.0f)` 567,
// with the far distance read at each use 574, with the flags as a word value 586 (2333 B), with the context alpha
// through PIN 555 (2322 B). Kept (behaviour, count unchanged): `delta` assigned k, i, j. VC reorders a three-term float
// sum by the order delta's fields are first assigned, so with i, j, k the forward and projection dots and both
// camera sums were computed (k + j) + i, while retail computes (j + i) + k in all four (0x2eb3a-0x2eb5b,
// 0x2ec40-0x2ec58, 0x2ed71-0x2ed97, 0x2ed99-0x2edb9); every term order written in the forward dot compiles to the same
// code, and explicit grouping `(j + i) + k` in all four gives retail's order too (493). With k, i, j all four round as
// retail's: 492 plain (register-masked aligned 242 -> 237).
// Frame 0x90 versus 0x94 and the decoded output homes remain. Immutable snapshots of position,
// direction, tint, position/direction, position/tint, direction/tint and all three: 592/573/572/623/637/617/668.
// Center components through saved-position stores 469, with aggregate saved-position transfer 486.
// NOT kept: 469 grows to 2288 bytes and emits scalar stores instead of the integer-word transfer.
// 486 preserves length and the transfer, but gains six and keeps the 0x90 frame.
// Round 407: entry rejection guard 487; separate opacity/size guards 487; bind rejection guard 492; entry and
// bind rejection guards 487; while entry loop 556; depth-disable arm first 495; draw point aggregate 492; draw
// color aggregate 492.
// NOT kept: the entry rejection forms gain five at retail length, below the eight-point threshold. The 0x90
// frame and the draw point overlap remain.
// B458: Final width and height as in-place scales before/after texture bind, after depth state, and after
// vertex setup 586/523/517/514; separate width and height values at those sites 586/532/526/523.
// Frame 0x90 versus 0x94 and shared direction/draw-point home remain.
// B460: Draw-point aggregate or component stores after texture/depth/vertex/trig
// 511/509/509/509 for both forms. These recover frame 0x94 but leave tag and
// addressed-output homes off and grow the body. Projected point or three coordinates
// before texture, with the draw receiver at texture success or at draw 552/511/548/566.
// The separate draw-point slot at retail's ordering still differs. Original body restored.
// MiniMax R (11-17): no new form, no lower score (492): the frame is 0x90 against retail's 0x94, so the stack slots sit four bytes off from the start.
// AC571: Final dimension pair before texture binding / on acceptance / after depth / after vertex
// 586/535/529/526. Apply size in place to the scale pair at the same sites 586/527/526/526.
// NOT kept: the last three in-place forms reach frame 0x94 and 2285 bytes but raise the score and
// keep the tag/time and addressed output homes displaced. Original body restored.
// Round 623: interpolate then scale size 492, compound size interpolation 492; opacity named and
// clamped 563, compound opacity 563, ordered opacity clamp 563, both compound interpolations 563;
// opacity gate before size 491, size gate before opacity 492. NOT kept: the one-point lead is below the keep threshold.
// Round 630: typed draw entry 492, typed draw tag 496, both typed views 496,
// context slot bitfields 492, both views and slot bitfields 496, typed scale pair 543,
// snapshot of entry flags 504, snapshot of entry count 568.
// The separate draw point and original output homes remain unresolved.
// Round 422: Scaled point then translated 492; named displacement products with field/aggregate stores 492/492;
// displacement vector with field/aggregate/scoped field stores 492/492/492;
// named final coordinates 492; displacement vector and point setter 492.
// The 0x90 frame and the overlapping direction and draw-point homes remain.
// Earlier draw point after entry admission, primary scales, RGB tint or camera angle,
// with field stores or a const aggregate: 567/567, 497/497, 497/497, 497/497.
// NOT kept: all retain frame 0x90 and the shared direction/point home. The first
// pair shortens to 2255 bytes and the other six regress at retail length.
// Round 190 (m3 lane 8, fast; --fast agrees with the plain 492 here): the draw point declared with the outputs at function
// scope 492; the selected colour declared at the top of the loop body 487; the sine/cosine and the draw call in their own
// block 492; visibility declared after the tag 521; the outputs declared scalars first, then colour, direction, position
// 492. Retail's 0x2e3f0 outputs sit at frame +0x24 (tag), +0x34 (position), +0x40 (colour), +0x7c (direction); ours at
// +0x3c, +0x44, +0x24, +0x84.
// Shared sampled/draw color 487, with sampled field transfers 524, with an animated-color copy
// 549, with both transfers 567. Early animated copy or channel initialization 571/557.
// Shared color beside angles 487, or beside animated tint with both transfers 596.
// NOT kept: 487 gains five at retail length and the 0x90 frame remains. Other forms regress.
// Tag and center decoded into the saved record with aggregate, field, indexed field,
// indexed scalar, pointer or reference publication 476/486/467/488/474/486.
// Separate decode with record construction from fields or scalars 516/557.
// NOT kept: the indexed-field lead groups the decoder outputs and keeps
// frame 0x90. Scalar record construction reaches frame 0x94 but grows by 26 bytes.
// M3 lane 5 r205 (fast, control 492): one-field locals dropped for reads at each use: the entry flags pointer 492, the
// displacement 492 (same code each), the colour curve pointer 531; the loop index declared at function scope beside the
// outputs (retail homes it between alpha and tag) 492; the near angle read at each use 492. The frame stays 0x90.
// M3 lane 14 r121 (fast with 0x2f2d0, 0x2e3f0, 0x222d0, 0x497a0; kept form plain; a/s/r = LCS-aligned unmatched lines
// exact / stack offsets masked / registers masked too, main 391/269/121). Reading: retail's frame is sorted by how often
// each cell is referenced (most-referenced lowest: 11, 9, 10, 10/8/8 ... 2, 2/1/1), so the 0x90/0x94 frame was a
// symptom. The cause: retail's 0x2f2d0 clobbers XMM7 and main's did not, so this body held `forward` in XMM7 across the
// whole function (reloaded after every call) and the loop's 1.0 never got a register. Kept, 492 -> 417 at 2285 bytes
// (153/153/71) with 0x2f2d0 193 -> 177: that callee's case 3 offsets first, and here retail's own forms together:
// `!(visibility > 0.0f)` (retail's NaN behaviour), the context alpha through PIN (stored in each arm), the far
// distance read at each use, the sampled colour copied by field, each entry's colour as a copy of `animated`
// multiplied in place, the opacity factor multiplied in each arm of an if/else, and the x/y scale as one point2f
// (retail keeps the two side by side in the dead 12-byte direction slot, which two scalars never share: with scalars
// the same body is 520). With the vertex-data value through PIN as well (retail's store form there too) 420
// (149/149/68: closer by alignment, three more by position, so not kept). On that 420 form, each piece dropped:
// the visibility form 421, the far reads 499, the field copy 537 (2248 B), the context PIN 525 (2265 B), the if/else
// 437, the pair with main's entry colour 552 (2236 B); the arms as `if (!(flags & 0x20)) attenuation else unoccluded`
// (retail's block order) 417, not kept (a swap for 3). On main's
// callee: these forms 557 (2326 B), the scalars' defaults at the top of the loop body 434 (2294 B, a moved
// declaration), the pair there 531; `(void)&forward;` and forward read through a const pointer 492 (same code); the
// callee alone 572 (2232 B). On the 420 form: delta assigned i, j, k 419 (160 aligned); delta through
// vector3d_from_points3d 423; forward or the projection through dot3f, either argument order, 420 (same code);
// natural-order parenthesised sums `(i + j) + k` for forward, projection and both camera sums 494 (147/147/64).
// Lead (build/r121/lead-0x2e9e0-helper-forms.patch, 445 at 2288 bytes but 62/60/24 aligned): every use of delta and
// reflected through the existing helpers (vector3d_from_points3d, dot3f for forward, the projection and both camera
// sums, vector3f_scale + scaled_difference3f for reflected, vector3f_add_scaled for the draw point) 495 (150/118/58;
// any one of them left direct is worse, 133 to 151 stack-masked), and with `real attenuation = 1.0f;` declared beside
// `animated` (retail stores its 1.0 there, 0x2eb2c; a moved declaration, so match-only) 444 (69/63/25): delta in
// XMM1-3, forward built and consumed in XMM0, the loop's 0.0/1.0 in XMM6/XMM5 and the index reload as retail. On it:
// the callee's parameters as (position, direction, ...) 445 (67/61), retail's arm order 444 (64/62), reflected through
// a separate projected vector 444 (66/60/22), the pair declared ahead of the angle 444 (68/62), size declared at the
// function's top 445 (same code), one to six unused locals 444 (same code), seven or eight 445. Left on the lead: the camera
// forward's i and j swap XMM4/XMM5 (retail loads its j first and computes reflected.j first), the k products'
// sides in the forward and camera sums (retail delta first), `far * inverse`, `delta.i * direction.i` and `context
// alpha * attenuation` (retail loads the memory side first), the draw point's x product (retail copies the
// displacement), and the loop's numbering (retail size XMM1, amount XMM2, angle XMM4; ours amount 1, angle 3, size 4).
// Diagnostics, reverted: forward in a one-element array 564 (2305 B: 1.0 then takes XMM7 for the whole function);
// the camera forward as its own symbol on the lead 444 (same code).
// Clamp results through pointer/reference with a joined store 492/492;
// complete bound-arm stores 512/512; input-first stores 537/537; in-place clamps 535/535.
// NOT kept: each retains frame 0x90 and the direction/draw-point overlap.
// Field, chained, or attenuation-sourced color defaults after the delta 495/495/495/495.
// Reflected aggregate 417, shared component setter 281, aggregate difference then scale 504,
// projected aggregate with shared scaled difference 500. On the reflected setter, shared draw-point
// setter or aggregate 281/281, or a shared delta setter 284. The reflected setter gives retail’s
// separate draw-point Y and Z loads and stores, with X still loaded separately.
// M3 lane 4 r227 (fast with 0x2f2d0/0x2e3f0/0x222d0, = full 417; a/s/r = LCS-aligned unmatched lines exact / stack
// offsets masked / registers masked too, main 136/128/46). Lane 14 r121's helper-form lead re-measured 445 at 2288 B
// (54/50/16). On it: `size` computed before `opacity` 447 (48/44/22: the angle then takes retail's XMM4, the amount
// stays in XMM1 where retail has XMM2 and size XMM1); with that, the draw point written out 456, the camera sums
// written out 473 (2304 B), the projection written out 379 (93/82/33: a one-line realignment, the delta block worse).
// On the lead: `size` declared ahead of `opacity` 445 and the angle ahead of `selected` 445 (same code); the forward
// dot written out 459 (72/65/23: its sum then emits k first), with reflected written out too 511 (2296 B); delta
// written out in all six orders 518-520 (2302 B); the unoccluded product, the far-angle products and the projection's
// dot3f arguments swapped 445 each (same code). Keepable pieces on main without the lead's moved `attenuation`
// declaration: delta, forward, reflected, the camera sums and the draw point through the helpers 504 (2270 B), with the
// vertex value through PIN 504. Left on the lead: the camera forward's first load (ours i, retail j) and the k
// products' sides in the forward and both camera sums (ours global first, retail delta first in all three), the
// projection's i product, the loop's amount/size registers (ours 1/4, retail 2/1).
// Camera axis in scalars/vector 422/542. Running forward sum 487, or interleaved with delta 487.
// Attenuation initialized by animated tint 503; distance read between named reflection terms 503.
// Draw point accumulated from decoded position 544, or from reflected components 294.
// NOT kept: none improves the opening body. The reflected-component draw point keeps the 0x94 frame and length but still loads the X product differently.
// Coordinate deltas as scalars in original/XYZ order or const XYZ 519/519/519.
// Position scalars then subtraction 518. Vector at camera angle in original/XYZ order 519/519.
// Vector at reflection exit via setter/aggregate 519/519. NOT kept: all lose the 0x94 frame and shorten the body.
// Camera displacement, reflection and animated tint local renames, alone and in pairs
// or together, and forward-distance rename 281 each. The 0x94 frame agrees and the
// delta/reflection XMM allocation and unity-store schedule stay unchanged.
// Shared axis across native differences 421; staged dot with original/native delta order
// 294/225; with tint/unity/both before the dot 225/535/535; reflected coordinates
// staged before the setter, without/with those defaults 225/535. NOT kept: the
// 225 forms retain the 0x94 frame but lengthen the function to 2290 bytes, five
// past retail. The early exit displacement and delta/reflection registers still differ.
// Round 452: Distance ratio through the bounds macro or explicit bound arms 501/501;
// equality-first with helper or bound arms 496/507; bound selected raw ratio or conditional
// input 417/418; named distance span and numerator with helper or bound arms 487/501.
// The distance result still does not bring the unity store forward with the three tint stores.
// Named products in the forward, angular attenuation and camera-angle dots alone 500/281/281;
// forward+attenuation, forward+camera, attenuation+camera and all four dots 500/500/281/500;
// all four with finished camera sums 500. NOT kept: the 0x94 frame stays, and the opening
// delta and axis schedule still differs.
// M3 lane 14 r132 (H2M_FAST, = full 281 for the control): the opacity arms negated with axis_scale ahead of the angle and
// delta assigned in each of the six orders 275-276 (2286 B; reorderings, match-only); with `real attenuation = 1.0f;`
// declared beside `animated` 504-505 (2290 B: the first clamp then stores in each arm and keeps the value in XMM4 as
// retail, aligned exact 112 against 127); on that, reflected as three statements j, i, k 496 at retail's 2285 B (aligned
// 117/110/44), on the 275 form 411; `forward * fwd` operands or `2.0f * (...)` 504/275 (same code); the draw point through
// vector3f_add_scaled, the vertex value through PIN, the projection and forward through dot3f 504 each (same code), delta
// through vector3d_from_points3d 518 (2302 B), with forward through dot3f 513. Phase scan (1-7 initialised dead longs):
// at the top of the 275 form 499-501 (the homes move), in the end block, ahead of the camera angle or ahead of delta 496
// every n (inert: the camera sums' operand sides and the forward block do not follow the symbol count here).
// MiniMax H (11-20): no new form, no lower score (417); the delta and reflection XMM allocation and the unity store schedule still differ.
// Distance bounds pointer 281, Distance bounds reference 281, Angle bounds pointer 281, Angle bounds
// reference 281, Distance and angle bounds pointers 281, Distance and angle bounds references 281,
// Distance bounds with branch result initialization 424, Both bounds with distance result handed to
// angle phase 497.
// NOT kept: The typed bounds tie at the opening extent. Both branch-produced fade forms raise the score, and the staged form lengthens the body.
// The camera delta and reflection registers still differ, and the attenuation default still stores after the tint.
// M3 lane 5 r223 (H2M_FAST, control 281, aligned 142/65 exact/register-masked; the one extra byte is the loop's
// draw point x product, ours `movss xmm1, reflected.i; mulss xmm1, xmm0` where retail multiplies by memory):
// redundant paren pairs (1 to 3) around the forward's first product or one delta term 487 each (one instruction
// fewer later, the delta/forward block unchanged), around a camera sum's first product or the draw point's x product
// 281 each (same code); `attenuation` declared beside `animated` (retail's 1.0 store beside the colour's three) 503
// (2290 B, aligned 164: delta then x, y, z as retail but the colour stores move after the forward block); that with
// delta assigned i, j, k 504; with reflected as three statements j, i, k 495 at 2285 B (aligned 171), with delta
// i, j, k too 496; the opacity arms negated with axis_scale ahead of the angle 276 (aligned 136; a swap, not kept),
// with delta i, j, k 275, with the attenuation and reflected forms 496, with all four 497.
// Displacement aggregate 280, with staged dot 486; projection then reflection 506,
// projection then separate subtract/scale 499, per-axis stages 493, aggregate plus
// reflection stages 505. Near endpoint before reflection 503 or between projection
// and reflection 364. NOT kept: the aggregate gains one point and leaves the delta
// registers and branch span different. The other forms regress.
// Camera origin by value with the point difference, vector setter or direct components 541/537/537. In-place
// position translation 280. In-place projected reflection by scale/difference, setter/difference or
// scale/setter 554/499/539. Camera origin with shared difference and in-place reflection 595. NOT kept: the
// one-point gain is below the threshold and still one byte over retail.
// M3 lane 4 r242 (H2M_FAST with 0x2f2d0/0x2e3f0, control 281): the first distance clamp as a ternary initialiser
// (`near != far ? clamp : 1.0f`) 424, as if/else with `attenuation = 1.0f` in the else arm 424 (both 2286 B); the saved
// record taken at the count and the count incremented in its own statement 281 (same code: retail stores the count
// before the tag); the draw point's x product as `reflected.i * displacement` 281, with the position added first 281,
// all three rows position-first 281 (same code). Reading of the retail-length lead (attenuation beside `animated`,
// reflected as three statements, 495 at 2285 B): it has retail's per-arm stores at the first clamp; left are one more
// instruction in the delta/forward block (retail multiplies each loaded axis in place by `forward`, ours copies
// `forward`), the far-angle product (retail loads `far` and multiplies by `inverse`), the unoccluded product's sides,
// the opacity arms, the draw point's y product and the vertex-data clamp's store form.
// The 0x94 frame and decoded output homes agree. First +0x76 is the missing-tag exit displacement. The first changed computation has a different camera-axis schedule. Retail tests the distance endpoints while the last two reflected components are still in flight. Test the distance predicate at four stages, with the far endpoint either retained or read at every use.
// Distance predicate before delta, direct far reads 540.
// Distance predicate before delta, shared far endpoint 581.
// Distance predicate after delta, direct far reads 528.
// Distance predicate after delta, shared far endpoint 573.
// Distance predicate after projection, direct far reads 528.
// Distance predicate after projection, shared far endpoint 523.
// Distance predicate after reflected side, direct far reads 509.
// Distance predicate after reflected side, shared far endpoint 528.
// NOT kept: Testing after the reflected side reaches 2285 bytes with direct endpoint reads, but breaks the entry frame and scores 509. Earlier predicate forms lengthen the body and leave the camera-delta and reflection registers unresolved. The original distance test and reflection are restored.
// M3 lane 5 r227 (h2m fast, control 281 at 2286 B; retail copies the displacement for the draw point's x product,
// `movaps xmm1, xmm0; mulss xmm1, [reflected.i]`, after the two integer argument loads): the displacement field read at
// each of the three uses (no local) 419 at retail's 2285 B (each row reloads it: the stores to the address-taken point
// kill the common load); the x row written `position.x + displacement * reflected.i` 281 (same code); the x row
// `reflected.i * displacement` 281 (same code); the y and z rows `displacement * reflected.j/k` 281 (same code); the
// displacement declared after the point 281 (same code).
// M3 lane 3 r293 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0, control 281): `delta` as an aggregate initialiser `{ x - cam.x,
// y - cam.y, z - cam.z }` 280 (with it the forward dot's written order no longer matters: the plain i, j, k order 280 too);
// that with `reflected` as an aggregate initialiser (no vector3f_assign) 416 at retail's 2285 B; the forward axis through
// a `vector3f const *` to g_4b9dac in the dot and the reflection 281 (same code), with the delta initialiser 280; the delta
// initialiser with `real attenuation = 1.0f;` beside `animated` 504 (2290 B), with the reflected initialiser too 496 (2285 B).
// M3 lane 3 r295 (h2m fast with 0x2e3f0/0x222d0, both MATCH, and 0x2f2d0 at its 142; control 281): `real attenuation = 1.0f;`
// beside `animated` with the vector3f_assign kept 503 (2290 B); on that form the forward dot's terms in all six written
// orders 503 each and the eight operand-side choices of its products 503 each (the chain is reassociated); main's dot as
// `(j-term + i-term) + k-term` 487, as `j-term + (i-term + k-term)` 487. Re-measured round 242's retail-length lead 495
// (2285 B): its delta/forward block is one instruction longer than retail's (forward copied for two reflected products),
// which shifts every later row by one. Retail's product sides follow the order its values are produced: delta.i, the axis
// j, delta.j and delta.k, then the axis i and k (deltas in xmm1-xmm3, axis in xmm4-xmm6); ours makes the axis i and j first.
// The three delta statements in the other five written orders: i, j, k 280; k, j, i 280; j, k, i 281; i, k, j 281; j, i, k
// 281 (each order only rotates which components take which roles: two axis loads, two deltas, the 1.0 stores, then the
// third delta in xmm5); i, j, k with the early attenuation 504 (2290 B).
// M3 lane 13 r141 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0; control 281, aligned 127, register-masked 43): delta
// assigned i, j, k 280 (aligned 130/49); that with the forward written `delta.i * g.i + delta.j * g.j + delta.k * g.k`
// 280 (130/49, same code); delta assigned i, k, j 281 (127/45); reflected as three plain statements i, j, k 417 at
// 2285 B (136/50); the entry table read once into a named `entries` base ahead of the loop 497 (2290 B, 146/68).
// Reading: the loop's latch `mov edi, [esp + 0x30]` falls into the body in retail, where ours jumps over alignment
// padding to it (`jmp; lea esp, [esp]; nop`), so the byte over retail sits ahead of the loop.
// M3 lane 1 r155 (H2M_FAST with 0x2f2d0 and 0x2e3f0; control 281): the near distance read at both uses (no local) 281
// (same code); the near angle read at both uses 487; the far distance named as well 492; the far angle named as well 234
// (2290 B; the frame homes move off retail's, tag +0x34, with a new spill; register- and offset-masked aligned rows 40
// against 39, so a positional accident, and it names only one of the two parallel far bounds: not kept); both far bounds
// named 522 (2302 B); the clamp helper with early returns 514 (2270 B). Reading: retail holds the attenuation in XMM4 after
// the distance block and stores it to its home in each clamp arm (the equal-distance path reloads it); ours keeps it in
// memory.
// m3 lane 4 r248 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0; control 281, aligned 127): the camera sums with the double cast
// on each product's global operand 281, on both operands 281 (same code: retail loads the global first only in the j
// products, delta first in the i and k ones, ours the global first in all six); the global-side cast with delta
// assigned i, j, k 280 (aligned 130; the sums then emit k, j, i); the opacity's two arms as one conditional multiply,
// `opacity *= (*flags & 0x20) ? unoccluded_alpha : attenuation;` or `opacity = opacity * (...)`, 306 each (2288 B).
// M3 lane 3 r299 (h2m fast with 0x2f2d0, 0x2e3f0 and 0x2df00; control 281, difflib-aligned 143). LEAD, not kept at review
// (the PIN spelling at one of five clamp_2e9e0 sites needs a byte match, and the aggregate alone is 3, under the bar): 281 -> 271 (plain
// too; aligned 135; 0x2df00 still matches): the four saved records and their count as one global aggregate 278 (the count
// sits directly behind the records at 0x4c1a04, and retail stores the incremented count ahead of the new record's tag,
// 0x2f275-0x2f280, as it must when a store through the record pointer may reach the count; as two globals the count's
// store was delayed past the tag), and with it the vertex-data value through the PIN macro 271 (retail stores that value
// in each clamp arm, 0x2f173; PIN alone 274). On that form the distance clamp through PIN 525 (2305 B). Reading of the
// tail: every load is hoisted up to the previous use of its register, so the store order there is the source's.
// M3 lane 8 r225 (h2m fast with 0x2e3f0/0x222d0, both MATCH; control 281): retail's opacity test falls through to the
// attenuation arm (`jne` to the unoccluded arm). The arms in that order under a negated test 278, with delta assigned
// i, j, k 277, as `opacity *= !(*flags & 0x20) ? attenuation : unoccluded_alpha;` 306 (2288 B); the zero or the far
// bound on the other side of the visibility, attenuation, opacity/size tests 281 each, of the near-distance test 286,
// of the near-angle test 489; attenuation's 1.0 assigned in an else arm 424; the forward dot through dot3f in either
// argument order 281 (same code); delta through vector3d_from_points3d 284, with the opacity arms in retail's order 281.
// Reading: the extra byte is loop-top padding (ours jumps from the entry-pointer reload over four bytes of padding;
// retail's reload falls into an already aligned body), so it follows the pre-loop length, not the loop.
// M3 lane 13 r150 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0; control 281, difflib-aligned 142 with addresses masked):
// retail stores the attenuation's 1.0 right after the three `animated` stores, ahead of the forward products, and
// computes reflected.j first, then i and k interleaved. `real attenuation = 1.0f;` declared beside `animated` 503 (2290 B,
// aligned 164); the reflected rows as plain statements j, i, k 417 (retail's 2285 B and its row order, aligned 153: the
// forward products still load the global first and the deltas come after the axis loads), as j, k, i 417 (aligned 153);
// the camera position through a `point3f const *` 500 (aligned 200); the deltas as three `real` locals 519 (2270 B, a new
// frame), that with the j-first rows 527 (2264 B).
// Round 114: Axis reads interleaved with the deltas 421, an axis copy at the dot 542, and named atan2 sums 281.
// One dword for both context flag tests 500, a combined mask 308, and visibility tested after selecting its value 519.
// Named definition flag reads at the two tests 500, aggregate sampled tint assignment 532, nested tag and entry
// admission 324, and distances through real_bounds 281. The camera-axis load schedule and the extra byte remain.
// Camera displacement and reflected-row scheduling still differ. horizontal forward subtotal 500, both SSE
// subtotals 500, all four subtotals 500, origin copy and shared difference 540, aggregate delta and subtotal
// 500, scale after setter 365, separate projection and reflection setters 499, subtotal and separate setters
// 187, subtotal and Y-first stores 375, named projected/reflected rows 500, ordered delta and subtotal 500,
// ordered delta and Y-first stores 375. NOT kept: the subtotal and separate setters improve only together and
// compute X before Y, while retail publishes Y first.
// M3 lane 11 r162 (H2M_FAST with 0x2df00 MATCH and 0x2f2d0 142; control 281): lane 3 r299's saved records and count as one
// global aggregate (0x2df00 updated to it, still matched) 278; `delta` assigned i, j, k 280; both 277 (difflib-aligned 149
// against main's 142); both with the opacity arms under a negated test 274; that with the vertex-data value through PIN
// 267 (two match-only parts: the swapped arms and PIN at one of five clamp sites). Body unchanged.
// M3 lane 13 r153 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0/0x2df00; control 281, aligned 142). Operand-rank reading of the
// opening block: the compiler forms each product in a copy of its higher-ranked operand and sums products in falling rank.
// Retail's sides (axis j above delta j, delta i above axis i, delta k above axis k, every axis component above `forward`,
// sum j, i, k) need the order delta i > axis j > delta j > delta k > axis i > axis k > forward. Here the three deltas rank
// below the three axis reads in every statement form, and `forward` ranks above them all through a late temporary, so its
// rows copy `forward` where retail multiplies each loaded axis in place. With delta through vector3d_from_points3d and
// forward through dot3f (0x2dba0's own spelling) `forward` ranks lowest and the three rows multiply in place as retail,
// and the last delta takes the late temporary: with `attenuation` beside `animated` and the rows as statements j, i, k
// 511 (2296 B, aligned 160); dot3f(axis, delta) 511 (same code); the rows through vector3f_assign 513 (2302 B, aligned
// 150); both helpers on main's order (attenuation after the rows) 280 with vector3f_assign (2286 B, aligned 152) and 420
// with the statement rows (2285 B). The helper delta with the dot written out in main's order 511 (2296 B), i, j, k 507,
// with vector3f_assign and the late attenuation 284. Statement deltas i, j, k with the early attenuation: every forward
// spelling (main's, i, j, k, dot3f) the same code per row form, 504 with vector3f_assign (2290 B), 496 with statement
// rows in either order (2285 B, aligned 179), and with the late attenuation 280 / 416. On the 495 lead: `forward`
// declared at the top and assigned at its place 495 (same code); forward accumulated in three statements 511 (2296 B);
// the near distance read ahead of the rows 516 (2296 B). Diagnostic, restored at once: `forward` given an early zero
// ranks it lowest (rows in place) and moves the late temporary to the last delta assigned.
// M3 lane 11 r166 (h2m fast with 0x2f2d0/0x2e3f0/0x222d0/0x2df00 and 0x20d80; control 281, aligned 142). Reading of the
// operand ranks: a local's rank follows its first reference; the first 31 locals rank below every global member read and
// later ones above them all, so with delta and forward written here no single dot gives retail's sides (axis j above
// delta j, delta i and k above the axis, every axis member above forward) together with retail's sum order j, i, k: late
// deltas make their products outrank the j product (diagnostic, forward zero-initialised early, deltas j, i, k and four
// dead longs: all three sides as retail but the sum k, i, j, 511 at 2296 B; without the dead longs 511; with the sum
// parenthesised 511). The context alpha block is the body of the existing 0x20d80 with its second argument true, and
// retail's `context alpha * attenuation` loads the context alpha first, as it does when that value is an expanded
// callee's result: `real context_alpha = function_020d80((s_slot *)context, true);` is expanded in place here, 493
// (aligned 146: the expansion stores the product ahead of the clamp, as 0x20d80's two statements say); with 0x20d80's
// clamp as one statement (`result = PIN(slot->f * g_5234c8, 0.0f, 1.0f);`, its own fast result unchanged; its file was
// another lane's this round, so a two-file diagnostic, restored) 486 at 2286 B (aligned 142: the context block and the
// unoccluded product are then retail's and every later row sits one place off; LEAD, the two patches are
// build/r166/lead-0x2e9e0-context-alpha-call.patch and lead-0x20d80-one-statement-clamp.patch). A file-local inline copy
// of only the used part (diagnostic) 487 (aligned 140). Lane 14 r121's helper-form lead rebuilt 444 at 2288 B (aligned 64;
// build/r166/lead-0x2e9e0-helper-forms-444.patch), with that inline copy 447 (aligned 61), with the real call 539 (2303 B,
// aligned 201: the reflected loop is then no longer unrolled), with the one-statement callee as well 533 (aligned 194);
// the lead with vector3f_assign rows, the real call and the one-statement callee 510 (aligned 130), with the late
// attenuation 489 (aligned 132), without the draw-point helper, PIN and arm swap 510 / 486, without the camera and
// projection helpers too 486 (aligned 150). `vector3f const delta = {...}` with `real const forward` 280 (the aggregate
// initialiser's code); on the lead a bare `real attenuation;` declared beside visibility 444 (same code) and
// `real attenuation = 1.0f;` there 476 (its store moves ahead of the 0x20d80 call). Body unchanged.
// X-first displacement with retained Y, X/Y, Y/Z or X/Z camera basis: 500/500/421/556.
// In-place basis scaling with setter or aggregate: 422/420. Retained basis with separate reflection
// stores or a running projection sum: 563/422. The early camera loads and projection registers remain.
// NOT kept: the 2285-byte forms reach 420 and 556, both above the opening comparison.
// Decoded-position seeds through an aggregate, setter or explicit components before in-place
// origin subtraction 280/280/280; per-component seed and subtraction 486; retained draw
// coordinates with direct deltas, aggregate seeds, component seeds or early capture 500/498/498/500.
// The early camera-axis schedule and branch displacement remain. NOT kept: the three 280 forms gain one, and the other forms regress.
// In-place X subtraction or all three subtractions before tint initialization 486/500.
// The original camera-axis schedule and missing-tag branch displacement remain.
// M3 lane GB r4 (30 fast trials with 0x2f2d0/0x2e3f0/0x222d0; control 281, al=142, mask=65): clamp_2e9e0 as `return PIN(value, 0.0f, 1.0f);`
// 514 (2270 B), with vector3f_add_scaled draw point 514, with 0x20d80(context, true) 513; clamp_2e9e0 mutating value in place 509 (2285 B);
// reflected via vector3f_scale + vector3d_from_points3d + vector3f_scale (no loop) on main 500, with early attenuation + helper delta/forward
// 383 (2286 B); full loop-free helper form (vector3d_from_points3d delta, dot3f x4, 3-helper reflected in place or via projected,
// vector3f_add_scaled draw point, early attenuation; saved in build/gb4/2e9e0-noloop-helpers.cpp) 435 at 2289 B (568/567 insns, al=64, mask=26;
// 136 of first 137 insns byte-identical, only projection's `direction.i * delta.i` adds 1 movss/+4 B shifting the remaining 422 insns);
// on it: scaled_difference3f 444 (al=70, mask=28), vector3f_scale + vector3f_assign 452, clamp_2e9e0 PIN 469 (2302 B, al=68), plus 0x20d80 call
// 475 (al=68, no loop-unroll loss without scaled_difference3f), late attenuation 501, inplace clamp 452 (2295 B), delta declared earlier /
// all locals at top / inline dot3f / projection before inverse / reversed dot3f args / no projected local 435 each (same code), plus 0x20d80 call
// 445 (al=65, mask=29), retail opacity arm order 435 (al=63, mask=25), axis_scale before angle 435 (al=62, mask=26), statement delta i,j,k 517,
// direct projection `delta.i*dir.i + dir.j*delta.j + dir.k*delta.k` 383 (al=139), `delta*dir` i,j,k 458 (al=95), j+i+k 380 (al=111),
// `dir*delta` i,j,k 263 (2286 B, 567/567 insns, al=138, mask=70; NOT kept: combination/operand-order steering that worsens aligned/masked counts
// against both the helper form and main, emitting three reg-first projection multiplies where retail has one). Body unchanged.
// M3 lane 5 r246 (H2M_FAST with 0x2f2d0/0x2e3f0/0x222d0; control 281). Reading of the operand ranks (a diagnostic print of
// the back end's reassociation sort): each product's first operand (the copied or in-place one) is its higher-ranked leaf
// and a sum adds its products in falling rank; a leaf's rank follows its first reference, the first 32 locals below every
// global member and later locals above them. Here delta (24-30) sits below the axis (42-45), so all three forward products
// copy the axis. Retail's sides (axis j first, delta i and delta k first) with its sum order j, i, k need the six leaves
// ranked axis.k < delta.k < axis.i < delta.i < delta.j < axis.j, a right-to-left reading of the dot itself, which no delta
// defined ahead of the dot gives; the camera sums (global j first, delta i and k first) and the projection (delta i first,
// direction j and k first, sum j, i, k) show the same pattern. Diagnostics: two to six dead longs between delta.j and
// delta.i give retail's forward sides but the sum k, i, j (499-500); dead longs at the top, 2 to 96, lift the deltas above
// the axis (every product delta first, forward copied; 499-500); the reassociation-off pragma 522 (2325 B, sort unchanged).
// Forms: forward through dot3f(&delta, &g_4b9dac) 281; delta through vector3d_from_points3d with dot3f(&g_4b9dac, &delta)
// 280; delta i, j, k with the dot written i, j, k delta first 280; the camera sums written global first in the j term 281;
// the projection written j, i, k 281 (same code each). Body unchanged.
// The camera displacement registers and early attenuation default still differ.
// Named forward products grouped or interleaved with the deltas, with early/late tint.
// Opening Z/X/Y delta order 500/500/500/500. X/Y/Z order 500/500/498/500.
// NOT kept: length stays 2286 and all scores rise. The generated dot still combines
// the products in a different order from retail.
// Staged axis projection into the reflection vector 506/515, named projected scalars 487/518,
// in-place doubling of the projected displacement 365/515, and displacement scalars through
// the setter 281/503, with the attenuation default at its original point or with the tint.
// NOT kept: in-place doubling reaches the 2285-byte extent at a higher score. The camera
// displacement registers and the early attenuation store still differ.
// M3 lane 14 r157 (fast screens with 0x2f2d0, 0x2e3f0, 0x222d0 and 0x497a0; control 281 at 2286 B, aligned 143, masked 63).
// Reading: our loop head carries a `jmp` and 9 pad bytes that retail's does not, because our code ahead of the loop is
// about 10 bytes short (retail's four extra spills in the delta/forward/attenuation block), so the pad is a result, not a
// cause. Retail's needed leaf ranks are the forward dot's six leaves read right to left (axis.k, delta.k, axis.i,
// delta.i, delta.j, axis.j), as if the dot were their first reference. Forms: the three differences written inside the
// forward dot with the delta vector filled after it 490; the axis read through a const reference to the camera's
// forward 500; the attenuation default declared ahead of the delta (after the tint) 503 (2290 B), with the delta filled
// i, j, k 504, with the delta through vector3d_from_points3d 518 (2302 B). A forced-ranking search (12 of 2692
// candidates) found no single allocator decision that lowers 281: the first difference is in the scheduler, not the
// colouring. Body unchanged.
// M3 lane 11 r173 (H2M_FAST with 0x2f2d0/0x2e3f0/0x222d0/0x2df00/0x20d80; control 281, difflib-aligned 142). Retail keeps the
// attenuation in its stack cell with a register copy: the 1.0 default stored after the tint, each arm of the distance clamp
// copying and storing its result, the equal-bounds path reloading the cell. Forms: the distance and angle factors as
// named locals defaulting to 1.0 and multiplied with the alpha clamp at the end 506 (2271 B, aligned 163); attenuation
// declared bare beside the decoder outputs and its default assigned in place 281 (same code); the distance attenuation
// as an if/else on equal bounds (1.0 in the taken arm) 496 (retail's 2285 B, aligned 184); the far distance named once
// for its two reads 492 (aligned 146); the alpha clamp as the attenuation's initial value with the distance clamp
// multiplied in 488 (aligned 205).
// GA22 (26 fast trials, control 281 at 2286/2285 B, +0x76, fr=0x94, al=143, mk=62, 0x2df00/0x222d0/0x2e3f0 MATCH, 0x2f2d0 142, body unchanged):
// on loop-free helper form (vector3d_from_points3d delta, dot3f x4, 3-helper reflected, vector3f_add_scaled draw point, early attenuation; 435 at 2289 B,
// ins=568/567, al=58, mk=23): inline projection dot3f 435, basis_dot_xzy projection 436 (mk=21), running-sum projection i,j,k 457 (2286 B, al=87),
// j,i,k 379 (2286 B, al=98, mk=30), single-expr j+i+k or k+j+i 455 (2286 B, al=73, mk=30: 2 in-place multiplies instead of retail's 1), i+(j+k) 383 (2286 B),
// delta-first i,j,k 458, projection before inverse 435, single-div projection 445 (2285 B, ins=575), direct near bounds 435, s_camera_storage pointer 441,
// vector3f_add_scaled reflected 544, separate projected local 435, double atan2 457, main draw point 457, basis_dot_xzy forward 456, declaration reorders
// (direction first/last, delta early/top, motion locals at top) 435 each; with retail opacity arm order + axis_scale before angle 435 (al=56, mk=22),
// + PIN on vertex data 435 (al=52, mk=19), + function_020d80(context, true) 445 (al=53, mk=19). The single extra movss in projection (`direction.i * delta.i`
// loading direction.i instead of multiplying delta.i in place) shifts all 422 trailing lines by one instruction.
// @retail 0x2e9e0
long __stdcall function_2e9e0(byte const *context, s_2e3f0_record const *record)
{
    long tag = NONE;
    real time = game_time_get_seconds();
    point3f position;
    vector3f direction;
    color3f color;
    real alpha, amount, scale;
    function_2e3f0(record, &tag, &position, &direction, &color, &alpha, &amount, &scale);
    if (tag == NONE) return 0;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    if (*(long *)(definition + 0x40) <= 0) return 0;
    real visibility = 1.0f;
    if (!(definition[0x28] & 2))
    {
        visibility = function_020d80((s_slot *)context, false);
        if (!(visibility > 0.0f)) return 0;
    }
    real context_alpha = 1.0f;
    // retail loads the slot's flags as a dword (mov eax, [esi+4], 0x2eaa9), as 0x20d80 does, not a byte
    if ((*(dword const *)(context + 4) & 2) && (*(dword const *)(context + 4) & 1))
        context_alpha = PIN(context[3] * g_5234c8, 0.0f, 1.0f);
    color3f animated = { 1.0f, 1.0f, 1.0f };
    vector3f delta;
    delta.k = position.z - g_4b9da0.z;
    delta.i = position.x - g_4b9da0.x;
    delta.j = position.y - g_4b9da0.y;
    real forward = g_4b9dac.j * delta.j + delta.i * g_4b9dac.i + delta.k * g_4b9dac.k;
    vector3f reflected;
    vector3f_assign(&reflected, (g_4b9dac.i * forward - delta.i) * 2.0f,
        (g_4b9dac.j * forward - delta.j) * 2.0f,
        (g_4b9dac.k * forward - delta.k) * 2.0f);
    real attenuation = 1.0f;
    real near_distance = *(real *)(definition + 0x18);
    if (near_distance != *(real *)(definition + 0x1c))
        attenuation = clamp_2e9e0((forward - *(real *)(definition + 0x1c)) / (near_distance - *(real *)(definition + 0x1c)));
    real near_angle = *(real *)(definition + 8);
    if (near_angle != *(real *)(definition + 0xc))
    {
        real inverse = 1.0f / (near_angle - *(real *)(definition + 0xc));
        real projection = delta.i * direction.i + direction.j * delta.j + direction.k * delta.k;
        attenuation *= clamp_2e9e0(0.0f - *(real *)(definition + 0xc) * inverse - projection * inverse);
    }
    attenuation *= clamp_2e9e0(alpha);
    real unoccluded_alpha = context_alpha * attenuation;
    attenuation *= visibility;
    if (!(definition[0x48] & 1))
    {
        dword seed = (dword)context >> 5;
        dword hash = 0xffffffff;
        function_163ba0(&hash, &seed, 4);
        time = ((hash >> 8) & 0xff) + ((hash & 0xff) * (1.0f / 256.0f) + time);
    }
    if (*(long *)(definition + 0x4c) > 0)
        attenuation *= function_13bb90((s_tag_data *)*(byte **)(definition + 0x50), time, 0.0f);
    if (attenuation > 0.0f)
    {
        real base_angle = function_2f2d0(&direction, &position,
            *(short *)(definition + 0x2c), *(real *)(definition + 0x30));
        real camera_angle = (real)atan2((double)delta.j * g_4b9e4c.j + (double)delta.i * g_4b9e4c.i + (double)delta.k * g_4b9e4c.k,
            (double)delta.j * g_4b9e58.j + (double)delta.i * g_4b9e58.i + (double)delta.k * g_4b9e58.k);
        if (*(long *)(definition + 0x54) > 0)
        {
            s_tag_data *curve = (s_tag_data *)*(byte **)(definition + 0x58);
            color3f sampled;
            function_13be80(curve, function_13b390(curve, time, 0.0f), &sampled);
            animated.red = sampled.red;
            animated.green = sampled.green;
            animated.blue = sampled.blue;
        }
        if (*(long *)(definition + 0x5c) > 0)
            base_angle += function_13bb90((s_tag_data *)*(byte **)(definition + 0x60), time, 0.0f) * 3.1415927410125732f;
        for (long i = 0; i < *(long *)(definition + 0x40); ++i)
        {
            byte *entry = *(byte **)(definition + 0x44) + i * 0x30;
            word const *flags = (word const *)entry;
            real opacity = PIN((*(real *)(entry + 0x1c) - *(real *)(entry + 0x18)) * amount + *(real *)(entry + 0x18), 0.0f, 1.0f);
            real size = ((*(real *)(entry + 0x14) - *(real *)(entry + 0x10)) * amount + *(real *)(entry + 0x10)) * scale;
            if (*flags & 0x20) opacity *= unoccluded_alpha;
            else opacity *= attenuation;
            if (opacity > 0.0f && size > 0.0f)
            {
                color3f selected = animated;
                real angle = *(real *)(entry + 0xc) * 0.01745329238474369f;
                point2f axis_scale = { 1.0f, 1.0f };
                if (i == 0)
                {
                    axis_scale.x = *(real *)(definition + 0x34);
                    axis_scale.y = *(real *)(definition + 0x38);
                    angle += base_angle;
                }
                selected.red *= *(real *)(entry + 0x24);
                selected.green *= *(real *)(entry + 0x28);
                selected.blue *= *(real *)(entry + 0x2c);
                if (!(*flags & 0x10))
                {
                    selected.red *= color.red;
                    selected.green *= color.green;
                    selected.blue *= color.blue;
                }
                if (*flags & 1) angle += camera_angle;
                if (*flags & 4) size *= clamp_2e9e0((visibility + 1.0f) * 0.5f);
                if (*flags & 2) size *= forward;
                real displacement = *(real *)(entry + 8);
                point3f point;
                point.x = displacement * reflected.i + position.x;
                point.y = reflected.j * displacement + position.y;
                point.z = reflected.k * displacement + position.z;
                if (!function_47fd0(*(long *)(definition + 0x24), 0, *(short *)(entry + 4), 0))
                {
                    if (*flags & 8)
                    {
                        if (!g_4c19c0)
                        {
                            function_0222d0(D3DRS_ZENABLE, 2);
                            g_4c19c0 = true;
                        }
                    }
                    else if (g_4c19c0)
                    {
                        function_0222d0(D3DRS_ZENABLE, 0);
                        g_4c19c0 = false;
                    }
                    D3DDevice::SetVertexData2f(8, clamp_2e9e0(1.0f - *(real *)(entry + 0x20)), 0.0f);
                    real sine = angle == 0.0f ? 0.0f : (real)sin((double)angle);
                    real cosine = angle == 0.0f ? 1.0f : (real)cos((double)angle);
                    function_480a0(&point, axis_scale.x * size, axis_scale.y * size, cosine, sine,
                        function_131f40(opacity, &selected));
                }
            }
        }
    }
    if (definition[0x28] & 1)
    {
        if (g_4c1a04 < 4)
        {
            s_2e9e0_saved *saved = &g_4c19c4[g_4c1a04++];
            saved->tag = tag;
            saved->position = position;
        }
        else if (!g_55e6c6) g_55e6c6 = true;
    }
    return 0;
}



// @retail 0x48b00
bool function_48b00(point3f const *point, real radius, real *screen, real *extent)
{
    bool result = false;
    if (radius > 0.0f)
    {
        short width = (short)(g_48564e - g_48564a);
        short height = (short)(g_48564c - g_485648);
        real x = point->x, y = point->y, z = point->z;
        if (g_48568c[0] != 1.0f)
        {
            x *= g_48568c[0];
            y *= g_48568c[0];
            z *= g_48568c[0];
        }
        real vx = g_48568c[7] * z + g_48568c[4] * y + g_48568c[1] * x + g_48568c[10];
        real vy = g_48568c[8] * z + g_48568c[5] * y + g_48568c[2] * x + g_48568c[11];
        real vz = g_48568c[9] * z + g_48568c[6] * y + g_48568c[3] * x + g_48568c[12];
        real py = g_48568c[39] * vz + g_48568c[35] * vy + g_48568c[31] * vx + g_48568c[43];
        real pz = g_48568c[40] * vz + g_48568c[36] * vy + g_48568c[32] * vx + g_48568c[44];
        real pw = (real)(g_48568c[37] * vy + g_48568c[41] * vz) + g_48568c[33] * vx + g_48568c[45];
        real rx = g_48568c[30] * radius;
        real ry = g_48568c[35] * radius;
        if (pz > 0.0f)
        {
            real px = g_48568c[38] * vz + g_48568c[34] * vy + g_48568c[30] * vx + g_48568c[42];
            real inverse = 1.0f / pw;
            screen[0] = ((px * inverse + 1.0f) * width + (short)g_48564a * 2.0f - 1.0f) * 0.5f;
            screen[1] = ((1.0f - py * inverse) * height + (short)g_485648 * 2.0f - 1.0f) * 0.5f;
            real depth = pz * inverse;
            screen[2] = (depth < 1.0f ? depth : 1.0f) * 16777215.0f;
            screen[3] = pw / g_48565c * 16777215.0f;
            extent[0] = width * inverse * rx * 0.5f;
            extent[1] = height * inverse * ry * 0.5f;
            result = true;
        }
    }
    return result;
}



extern byte g_4670bc;
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
vector3f *function_11d000(vector3f const *v, vector3f *out);

#include "object_markers.h"

struct s_angle_pair
{
    real heading;
    real elevation;
};

PRIVATE __forceinline void direction_from_angle_pair(s_angle_pair const *angles, vector3f *direction)
{
    real cosine = (real)cos(angles->elevation);
    direction->i = (real)cos(angles->heading) * cosine;
    direction->j = (real)sin(angles->heading) * cosine;
    direction->k = (real)sin(angles->elevation);
}

struct s_direction_entry
{
    byte unknown0[0xc];
    s_angle_pair angles;
    long unknown14;
    long tag;
    byte unknown1c[0x18];
};
struct s_direction_list
{
    byte unknown0[0x78];
    long count;
    s_direction_entry *entries;
    real heading;
};
typedef char assert_direction_entry_size[(sizeof(s_direction_entry) == 0x34) ? 1 : -1];
typedef char assert_direction_list_size[(sizeof(s_direction_list) == 0x84) ? 1 : -1];

// @retail 0x3eec0
void function_3eec0()
{
    if (g_4b9ee9 && g_4b9eec != NONE && g_4c5038.valid)
    {
        s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
        long index = NONE;
        if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
            index = table->entries[(short)g_4b9eec].tag;
        s_direction_list *definition = NULL;
        if (index != NONE) definition = (s_direction_list *)g_4e3b44[index & 0xffff].bytes;
        for (long i = 0; i < definition->count; ++i)
        {
            s_direction_entry *entry = definition->entries + i;
            if (i >= 64) break;
            if (entry->tag == NONE) continue;
            vector3f local;
            direction_from_angle_pair(&entry->angles, &local);
            s_angle_pair facing;
            facing.heading = definition->heading;
            facing.elevation = 0.0f;
            vector3f forward;
            direction_from_angle_pair(&facing, &forward);
            matrix3x3 axes;
            axes.forward = forward;
            axes.left.i = forward.k * g_4687b0->j - forward.j * g_4687b0->k;
            axes.left.j = forward.i * g_4687b0->k - forward.k * g_4687b0->i;
            axes.left.k = forward.j * g_4687b0->i - forward.i * g_4687b0->j;
            axes.up = *g_4687b0;
            vector3f_rotate_into_axes(&axes, &local, &local);
            if (1.0f > g_4b9f58)
            {
                g_4670bc = true;
                function_16b10((s_render_reset_state *)g_485b48);
                s_object_marker marker;
                marker.matrix.position.x = local.i * 1023.875f + g_4b9da0.x;
                marker.matrix.position.y = local.j * 1023.875f + g_4b9da0.y;
                marker.matrix.position.z = local.k * 1023.875f + g_4b9da0.z;
                marker.matrix.forward.i = 0.0f - local.i;
                marker.matrix.forward.j = 0.0f - local.j;
                marker.matrix.forward.k = 0.0f - local.k;
                function_11d000(&marker.matrix.forward, &marker.matrix.up);
                function_30bf0(&marker.matrix.up);
                function_2dba0(entry->tag, false, 3, 0, i, &marker.matrix.position, &marker.matrix.forward, (color3f const *)((byte const *)g_4686cc + 4), 1.0f - g_4b9f58, 1.0f, 1.0f);
            }
        }
    }
}



extern void *g_509438;
real g_525924;
struct s_octree_output
{
    long node;
    point3f center;
};
s_octree_output g_4c5700[256];

class c_octree_radius_view_ab
{
public:
    void function_3f970(short index, plane3f const *planes, real const *bounds, point3f const *center, real radius) const;
};
real g_525930;

// @retail 0x3f830
void __stdcall function_3f830(byte const *tree, point3f const *center)
{
    byte *context = (byte *)g_547f88.context;
    box3f original = *(box3f *)(context + 0xf4);
    box3f bounds = original;
    real radius = g_525930;
    real lower = center->x - radius;
    if (lower > original.x0) bounds.x0 = lower;
    real upper = center->x + radius;
    if (original.x1 > upper) bounds.x1 = upper;
    lower = center->y - radius;
    if (lower > original.y0) bounds.y0 = lower;
    upper = radius + center->y;
    if (original.y1 > upper) bounds.y1 = upper;
    lower = center->z - radius;
    if (lower > original.z0) bounds.z0 = lower;
    upper = center->z + radius;
    if (original.z1 > upper) bounds.z1 = upper;
    g_509438 = NULL;
    if (*(long const *)(tree + 0x20) > 0)
    {
        real scaled_radius = (real)*(long const *)(tree + 0xc) * 8.0f;
        dword scaled_bits = *(dword const *)&scaled_radius;
        real tree_radius = *(real const *)&scaled_bits * 0.5f;
        ((c_octree_radius_view_ab const *)tree)->function_3f970( 0, (plane3f *)(context + 0x10c), (real *)&bounds,
            (point3f const *)tree, tree_radius);
    }
}

// @retail 0x3f970
void c_octree_radius_view_ab::function_3f970(short index, plane3f const *planes, real const *bounds, point3f const *center, real radius) const
{
    byte const *tree = (byte const *)this;
    if (index == NONE) return;
    short const *children = (short const *)(*(byte *const *)(tree + 0x24) + index * 24);
    if (bounds[0] > center->x + radius || center->x - radius > bounds[1] ||
        bounds[2] > center->y + radius || center->y - radius > bounds[3] ||
        bounds[4] > center->z + radius || center->z - radius > bounds[5]) return;
    real delta = radius - g_525924;
    if (0.0001f > delta && delta > -0.0001f)
    {
        if (function_3fd20(planes, center, radius)) return;
        long count = (long)g_509438;
        if (count < 256)
        {
            g_4c5700[count].node = index;
            g_4c5700[count].center = *center;
            g_509438 = (void *)(count + 1);
        }
        return;
    }
    real child_radius = radius * 0.5f;
    // each child center is its own brace-initialized local: assigning one shared point3f member by member
    // schedules the stores ahead of the argument pushes, retail interleaves them
    {
        point3f child = { center->x - child_radius, center->y - child_radius, center->z - child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[0], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x - child_radius, center->y - child_radius, center->z + child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[1], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x - child_radius, center->y + child_radius, center->z - child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[2], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x - child_radius, center->y + child_radius, center->z + child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[3], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x + child_radius, center->y - child_radius, center->z - child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[4], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x + child_radius, center->y - child_radius, center->z + child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[5], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x + child_radius, center->y + child_radius, center->z - child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[6], planes, bounds, &child, child_radius);
    }
    {
        point3f child = { center->x + child_radius, center->y + child_radius, center->z + child_radius };
        ((c_octree_radius_view_ab const *)tree)->function_3f970( children[7], planes, bounds, &child, child_radius);
    }
}



/* retail keeps the fraction on the x87 stack, unrounded */
PRIVATE __forceinline double random_unit_fraction(s_random_globals *random)
{
    return (real)random_next(&random->seed) * (1.0f / 65535.0f);
}

// @retail 0x39a80
bool function_39a80(byte *state)
{
    *(real *)(state + 0x80) = 0.0f;
    state[0x86] = false;
    if (*(long *)(state + 0x64) > 0)
    {
        byte *group = *(byte **)(state + 0x68);
        for (long i = 0; i < *(long *)group; ++i)
        {
            byte *entry = *(byte **)(group + 4) + i * 0x14;
            real *motion = (real *)(*(byte **)(group + 0xc) + i * 0x20);
            *(long *)(entry + 0x10) = NONE;
            if ((0.0001f > *(real *)(state + 0x1c) && 0.0001f > *(real *)(state + 0x20)) ||
                *(real *)(state + 0x1c) > *(real *)(state + 0x20))
            {
                *(real *)(state + 0x1c) = 0.1f;
                *(real *)(state + 0x20) = 0.8f;
            }
            motion[6] = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, *(real *)(state + 0x1c), *(real *)(state + 0x20));
            motion[0] = (real)(random_unit_fraction(g_4e7408) * *(real *)(state + 0x30));
            motion[1] = (real)(random_unit_fraction(g_4e7408) * *(real *)(state + 0x34));
            motion[2] = (real)(random_unit_fraction(g_4e7408) * *(real *)(state + 0x38));
            real length = (real)sqrt(length_sq3f((vector3f const *)motion));
            if (!(fabs(length) < 0.0001f))
                for (long component = 0; component < 3; ++component)
                    motion[component] *= 1.0f / length;
            *(real *)(entry + 0xc) = (real)(random_unit_fraction(g_4e7408) *
                (*(real *)(state + 0x28) - *(real *)(state + 0x24)) + *(real *)(state + 0x24));
            motion[7] = 0.0f;
            if (0.0001f > *(real *)(state + 8)) *(real *)(state + 8) = 1.0f;
            if (0.0001f > *(real *)(state + 0xc)) *(real *)(state + 0xc) = 1.0f;
            if (0.0001f > *(real *)(state + 0x10)) *(real *)(state + 0x10) = 1.0f;
            real sample = (real)random_unit_fraction(g_4e7408);
            *(real *)entry = (sample + sample - 1.0f) * *(real *)(state + 0x10);
            sample = (real)random_unit_fraction(g_4e7408);
            *(real *)(entry + 4) = (sample + sample - 1.0f) * *(real *)(state + 8);
            sample = (real)random_unit_fraction(g_4e7408);
            *(real *)(entry + 8) = (sample + sample - 1.0f) * *(real *)(state + 0xc);
            motion[3] = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, -1.0f, 1.0f);
            motion[4] = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, -1.0f, 1.0f);
            motion[5] = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, -1.0f, 1.0f);
        }
        state[0x86] = true;
    }
    *(real *)(state + 0x88) = 1.0f;
    return true;
}



real magnitude3d(vector3f const *v);

// MiniMax A (11-20): no new form, no lower score (170): the raw-length slot is +4 against retail's +0xc
// MiniMax I (11-19): no new form, no lower score (170): the raw current magnitude still shares the clamped length's slot where retail keeps it in a slot of its own ([esp + 0xc]) and reloads it for both clamp tests, and the dot's promotion still widens to a spill; the named raw length (251) and the clamped pair (320) recorded below still measure worse, so the body is unchanged.
// MiniMax H (11-20): no new form, no lower score (170): the body is retail's length and the whole difference is
// the raw current magnitude's home - retail keeps it in a slot of its own ([esp + 0xc]) and reloads it for both
// clamp tests, where this build leaves it on the x87 stack and lets the clamped value take its place. Note for the
// next round: with this function alone in the reduced image the length drops to 997 bytes and the score to 315,
// because magnitude3d stays out of line there, so measure it with the rest of the file's functions present.
// score 170 (was 170): The 0x38 frame and 1045-byte extent agree. First +0x30 is the x87 rounding store for the
// raw current length, at +4 in this build and +0xc in retail. Retail reuses its transfer slot for the real dot
// and angle. This build keeps a separate promoted dot for inverse cosine. The trials alter the overlap of
// inverse cosine, angle multiplication and cross-product production to test whether the earlier slot exchange
// follows that later x87 lifetime.
// Earlier at 170 (was 170): The raw current length shares the bounded slot at ESP+4. Retail reuses ESP+0xc for it and the real dot and angle.
// the raw current length, the dot's double copy and the angle reload (below).
// MiniMax S (11-17): no new form, no lower score (170): retail still keeps the raw current length in its own slot
// and the pinned value in another, where this build reuses one slot for both, so the later homes shift; retail
// still re-reads the dot as a float for the arccosine where our acosf and fabsf widen to their double forms, and
// the reduced image of this file is not faithful, so the permuter cannot search this body.
// MiniMax I (11-15): no new form, no lower score (170): retail still keeps the raw current length in its own slot and the pinned value in another, where this build reuses one slot for both, so the later homes shift; the reduced image of this file is not faithful (it scores 315 against the full build's 170), so the permuter cannot search this body.
// MiniMax R (11-16): no new form, no lower score (170): the reduced image of this file is not faithful (it scores 315 against the full build's 170), so a permuter search here cannot be kept; retail still keeps the raw current length in its own slot and re-reads the dot as a float for the arccosine, where our acosf and fabsf widen to their double forms and drag the later homes along.
// MiniMax E (11-15): no new form, no lower score (170): the reduced image of this file is not faithful (it scores 315 against the full build's 170), so a permuter run cannot be searched here.
// MiniMax S (11-14): no new form, no lower score (170): retail keeps the raw current length in its own slot at +0xc and the pinned value at +4, where ours reuses one slot for both, so the later homes (the dot's float, the sine at +8, the angle and the cosine at +0xc) all shift; retail re-reads the dot as a float for the arccosine while under /GL our acosf and fabsf widen to their double forms and drag the dot's double copy along, and no source spelling recovers that float load.
// MiniMax B (11-13): no new form, no lower score (170): the products of each cross product are still multiplied in the other order (this build takes the direction's components into xmm4 and xmm3 where retail takes them in xmm1 and xmm3), and the dot product helper's body may not be written out by hand, so the order is not ours to choose.
// MiniMax H (11-12): no new form, no lower score (170); retail keeps the raw current length in its own slot at +0xc
// and the pinned value at +4, while ours reuses one slot for both, and the later homes (the dot's float, the axis)
// still take the other register roles.
// MiniMax S (11-11): no new form, no lower score (170); retail keeps the raw current length in its own slot at +0xc and the pinned value at +4, while ours reuses one slot for both, and the later homes (the dot's float, the sine at +8, the angle and the cosine at +0xc) follow from that; naming the raw length, spelling the clamp as if/else and testing the guard on the raw all measure 251-252 because a named local changes the x87 shape (fst instead of fstp) and the compare. This round traced the arccosine: retail re-reads the dot as a float (`fld dword ptr [esp+0xc]`, the shape a call of acos with a float argument takes in our own build too), while under /GL our acosf and fabsf widen to their double forms and drag the dot's double copy (and with it the angle, sine and cosine homes) along; sinf and cosf keep their float form, and no source spelling recovers the float load.
// MiniMax B (11-12): no new form, no lower score (170): retail keeps the raw current length in its own slot at
// +0xc and the pinned value at +4, where ours reuses one slot for both, and retail re-reads the dot as a float
// for the arccosine while our /GL build widens acosf and fabsf to their double forms.
// MiniMax H (11-14): no new form, no lower score (170): retail keeps the raw current length in its own slot at
// +0xc and the pinned value at +4 where ours reuses one slot for both, and under /GL our acosf and fabsf widen to
// their double forms and drag the dot's double copy along, so the later homes shift with them.
// r136 lane 7 (fast with 0x24590/0x30bf0 in the image): ours keeps a double copy of the dot
// (`fst qword`) for acos where retail re-reads the float; `acos((double)dot)` 170, `fabs((double)dot)` 170,
// `acos(dot) * step` 170, `acosf(dot) * step` with `fabsf(dot)` 170, `(real)acos(dot) * step` 170.
// MiniMax E (11-09): no new form, no lower score (170); the float trigonometry (fabsf, acosf, sinf, cosf) compiles to the same bytes as the double calls, so the trigonometry is not what differs.
// MiniMax B (11-11): no new form, no lower score (170). Retail keeps the step in the angle only: the acos result is
// multiplied by the step argument in x87 (fmul dword ptr [esp + 0x3c]) right after the arccosine call, and the axis is
// the plain cross product, so the step must not move into the axis. Retail re-reads the dot as a float for the arccosine
// (fld dword ptr, then the call) and keeps the dot and the angle in one slot; a named float copy of the dot removes our
// double copy (fst qword) and reproduces that shape, but the frame stays 0x34 against retail's 0x38: retail gives the
// raw length its own home and leaves a hole at 0x8, while we coalesce the raw length with the pinned one, so every
// later slot sits four bytes lower. Named raws, hand-written clamps and expanded PIN forms all score worse (251/262).
// MiniMax H (11-06): no new form, no lower score.
// Retail computes the angle through a single-precision arccosine wrapper (0x322bd8) that loads the dot
// product as a float from [esp+0xc]; we keep a double-precision copy of the dot at [esp+0xc] and call the
// double arccosine (0x48a538) instead. The magnitude homes also differ: retail keeps the raw length in its
// own slot and the pinned value in another, where we reuse one slot for both.
// Earlier 170 (was 170): plain fallback component loop recovers retail frame 0x38;
// first +0x30 stores the raw length at +4 rather than +0xc; rotation allocation still differs.
// MiniMax R (11-01): 132 with the shared or staged axis cross and the rotated components as named locals, but it costs 0xc2d00 its match in the full build like the held cross3f form, so it is not kept; the named components alone (182) and the axis written out in the helper's operand order (182) change nothing.
// Earlier baseline 182 (193, 324; 147 with the shared cross3f axis, held: see Round 435): PIN reevaluates the clamped expression as retail does (the target's
// PIN reevaluates the clamped expression as retail does (the target's
// Earlier counts 193, 324; the shared axis cross reached 147 and was held (see Round 435).
// sqrt is computed up to three times, with its x*x term hoisted and added last). Retail's
// frame is one slot larger (0x38): its unclamped current length, the dot product, the angle
// and the cosine share the slot at +0xc while the temporary at +8 stays separate; here the
// length shares the slot of a. Tried: plain, reference and macro forms of the length (193-217),
// first/second vectors in either order and operand order of the scalings (193-214), b before a (279),
// the dot test last (205), angle and cosine spelled separately (193), the current length as its own named local before the PIN (217), as an assignment inside the PIN (193).
// Retail also spills a second temp at +8 for rotated.i and orders sine/cosine slots (+0x10/+0xc) differently; the rotation arithmetic differs from there on.
// Round 38: both lengths are the existing magnitude3d (0x24590; retail's x87 length code is its body), 193 -> 182; the
// frame (length slot) and the cross product's operand registers still differ. Tried: first rotated in place with named
// cross terms (the 0x11d3b0 shape, 194), vector3f_scale for both normalizations (199), the six normalized components
// interleaved first.i, second.i, first.j, first.k, second.j, second.k (188; per component 190), a named length local
// before the PIN with magnitude3d (282); the two dot products through the existing dot3f (182, same code, kept).
// Round 435: shared axis cross (cross3f(&first, &second, &axis)) 147, held 11-01: in the full LTCG build it makes
// 0xc2d00 (unknown_0bffa0.cpp) lose its match (DIFF at +0x12e), so the axis stays written out; staged axis cross 147; shared/staged rotation tangent 196;
// both cross boundaries in all four pairings 154. Raw current length reused by clamp/guard with
// axis-only/both boundaries 255/276; both raw lengths reused 330/342. Frame remains 0x34 vs 0x38.
// DH round 270: existing vector setters at normalization/rotation/both/final-scale/normalization-and-
// scale/rotation-and-scale/all boundaries 198/188/204/182/198/188/204; normalization and final setters with
// scalar rotation results 198. Magnitude and protected neighboring body stay matched in previews. Frame 0x34
// versus 0x38 and raw-length/dot/angle/cosine homes remain; entire original body and every arithmetic tree
// restored.
// Round 524: explicit current/target/both repeated magnitude clamps 182/182/182;
// bounded deltas alone/with current/target/both clamps 163/163/163/163; all with cosine first 164.
// Repeated per-axis blocks 163 discarded; rotation-only/fallback-only clamps 180/166.
// Plain fallback component loop, original/bounded rotation-size expression 170/170.
// Keep shared loop and original rotation-size expression: plain 170; raw length/dot homes remain.
// F320: separate current/target reciprocals, complete aligned rotation sums and entry native-vector
// declarations in all seven combinations 182 each; all with scalar role names 182. Raw length still
// shares a clamped slot, frame 0x34 versus 0x38. Original executable body restored.
// F323: Raw-length/dot/angle/cosine homes and frame 0x34 versus 0x38 remain.
// Exact double current/target magnitude carriers, early immutable interpolation and combinations
// 268/347/320/354/310/387/378; all with existing final scale 378. Original body restored;
// both exact controls survive every preview.
// Round 527: Native real absolute/sine/cosine API subsets 1/2/4/3/5/6/7 and all with separate
// sine/cosine declarations: 170 each. Original math APIs and complete executable body restored;
// frame 0x38 agrees, raw-length/dot/angle homes and rotation allocation remain.
// AC480: Recovered-frame admission orders and scalar/vector/full workspace names: 170/182/182/182/182/182/182/182/262/314/334.
// The shared real clamp at current/target/both boundaries gives the final three counts;
// original macro clamps and complete body restored. Raw magnitude/dot/cosine homes remain.
// Round 530: ordered dot bounds, exact double dot, double angle with original real rounding,
// bounds/dot, bounds/angle, dot/angle, all three and complete scalar role names: 191/211/198/242/220/213/244/244.
// Both exact controls survive; original body, math APIs and all rounding/grouping restored.
// Round 533: Complete projected-axis/cosine-aligned/sine-tangent expression parts,
// subsets 1/2/4/3/5/6/7 and all with scalar/vector role names: 170/171/170/170/170/171/170/170.
// Original products and body restored; raw-length/dot/angle homes and rotation allocation remain.
// P275: Complete current/target/both input snapshots 246/288/266; same native vectors
// normalized in place 245/287/265; both inputs captured at entry 272; both with early
// normalized workspaces 266. Original body restored; raw-length/dot/angle homes remain.
// J241: Complete rotation output in first/second/rotation/current workspaces 175/170/175/174;
// uniform scalar row staging 170/170/170/174. Real/double absolute-value stages 170/196; NOT
// kept: one-use call-result forms do not match. Scoped precision diagnostic 547 at 1287
// bytes, excluded from real trials and reverted. Original whole body, expressions and helper
// contracts restored; raw-length and promoted-dot homes remain.
// P277: Existing scale helper at current/target/both/output/current+output/target+output/all
// boundaries 191/170/190/170/191/170/190; double-factor helper at all boundaries 224.
// Original body restored; raw-length/dot/angle homes remain with retail frame 0x38.
// AC492: On the retained fallback loop/frame, explicit current/target clamp producers
// 170/170; minimum-default 263/226; nested positive 254/203; conditional-tail 170/170.
// Original macro clamps, repeated roots, controls and complete body restored; raw-length/dot homes remain.
// Lane 16 r80 (plain): retail's dot is re-read as a float for acos (ours keeps a double copy at +0xc,
// moving angle/sine/cosine homes); acos on a fresh dot3f 198 (1039 bytes); arms swapped under the
// inverted test 171; sine, projection dot, cosine, then the product 170; fallback `-step > delta` 170
// (identical); fallback delta through the nested ternary clamp 188 (1025 bytes). Body restored.
// DL257: Complete native/shared/staged tangent producers with a plain rotation loop 191/191/191;
// consuming the first vector 186/186/186; direct cyclic rotation loop 178; tangent and rotation
// loops 191. All retain the raw-length/promoted-dot home mismatch. Original complete body
// restored; shared tangent forms also raise two other scores.
// Y568: Actual current/target normalization and output component loops, seven subsets,
// then fused input lanes: 191/170/335/190/335/335/335/333. Original full body restored;
// raw magnitude/dot/angle homes remain at frame 0x38. All three exact controls survive.
// Y573: Fallback early-exit/while/cursor lifetime graphs: 170/170/170/170/170/170/170/170. NOT
// kept: all tie; raw-length/dot homes persist and cursor forms miss equality. Original complete
// body and helpers restored.
// Y575: Existing rotation-size clamp at tail/direct-tail/rotation-entry/direct-entry/
// predicate/direct-predicate, then original size at predicate/rotation-entry:
// 171/171/303/303/311/311/311/303. NOT kept: no gain; early one-use size producers
// miss equality and move frame/length away. Original helper contracts and body restored.
// P319: Conditional absolute formation, direct negative/nonnegative, real conditional/
// positive/branch and double conditional/positive/branch: 195/195/195/195/195/215/215/199.
// NOT kept: every form regresses and loses the native frame; original fabs and full body restored.
// DH343: Complete native construction of first/second/axis/rotated/both inputs/inputs+axis/
// axis+rotated/all existing vectors: 170/170/170/170/170/170/170/170. Original body restored;
// raw-length and promoted-dot homes remain; frame and extent still agree.
// P324: Recovered-frame independent reciprocal lifetimes, immutable scalar ownership,
// entry shared-scalar and branch-owned rotation declarations: 170/170/170/170;
// named double inverse-cosine result 196; named cosine complement, existing bounded
// helper in fallback, named axis projection: 170/170/170. Original body restored;
// raw-length/promoted-dot homes remain. NOT kept: no eligible gain.
// M392: Computed paired trig/projection/rotation-size double receiver families, subsets 1/2/4/3/5/6/7 then immutable all: 299/294/277/296/354/350/350/350.
// NOT kept: nonexact precision/lifetime forms; native raw-length and promoted-dot homes remain. Original complete body restored.
// B440: Independent angle-only and complete rotation-admission outcomes in
// bool/byte/short/long: eight 170 at 1045/1045 bytes. NOT kept: no equality
// or gain; raw-length and promoted-dot homes remain. Original full body restored.
// B442: Existing complete axis-angle matrix with returned/axis/output/in-place
// vector application, matrix at producer/entry: 388/248/327/242/388/248/327/242.
// NOT kept: changed floating expansion misses equality and grows; original full body restored.
// AC549: Common final vector across rotation/fallback, separate/consumed rotation
// receivers with native/setter/memcpy publication, then native copies with entry
// declarations: 179/179/180/180/179/179/179/179. NOT kept: raw-length/promoted-dot
// homes persist at frame 0x38; 1054/1033 versus 1045 bytes. Original body restored.
// J256: Complete admission/axis dot loops, parallel product parts, immutable and separated producers:
// 184/170/184/170/170/170/170/170. NOT kept: admission loops regress and every other form ties 170; raw-length and promoted-dot homes do not recover
// Original executable source restored; native frame and allocation gaps remain.
// B451: Explicit raw current/target/both magnitude receivers on the recovered
// fallback-loop frame, mutable then immutable, followed by exact widening of
// current/target receivers: 251/309/320/251/309/320/284/321. NOT kept: no native-matching root/storage
// graph. Original full body restored; raw-length and promoted-dot homes remain.
// B453: Standard lower/upper clamp boundaries at current/target roots: lower current/target, upper
// current/target, both current/target, lower both, both complete: 328/288/305/280/247/325/316/311. NOT
// kept: no byte match; original complete body restored. Raw-length and promoted-dot homes remain.
// R221 (lane 3; harness with the magnitude3d and function_30bf0 files linked, exact 170/1045): `real cosine = dot;`
// used by acos (or by fabs) 170 (1037): retail's float reload for _CIacos with no double copy, frame 0x34; plus a copy
// of the angle for sin or cos 166 (1039): retail's fld/fsin/fstp/fld/fcos order, every slot 4 low because the raw
// current length shares a's slot (retail +0xc); on that, a named raw length 266, `a = PIN(a, ..)` 269, if/else clamps
// 269/266; a named raw length alone 251, with only the dot copy 267. The copies are one-use (a byte match needed).
// A same-file noinline normalize copy in place of function_30bf0 moves the raw length to +0xc (168, 1051 bytes).
// R223 (lane 3, fast with 0x30bf0/0x24590, control 170): the dot named `cosine` and assigned again by cos(angle) 170 (same code);
// one variable for the dot, the angle and the cosine 170 (same code); dot/angle/cosine and sine as the elements of a two-real
// array 177 (frame 0x3c); sine and cosine alone in that array 170; the rotation through a file-local inline (vector, axis, sine,
// cosine, out) helper called with sin(angle) and cos(angle) 178 (1035 bytes), kept out of line (diagnostic) 386 (832). None splits
// the promoted dot or angle copy: reassignment, array storage and helper arguments are all folded back to one value.
// R223, second pass: `step * acos(dot)` 170 (same code); the angle as `(real)acos(dot)` then `angle *= step` 170 (same code); both
// dot3f calls with their arguments swapped 170 (same code); the axis through the existing cross3f(&first, &second, &axis) 182
// (1058 bytes); cos(angle) written at both of its uses with no cosine local until after the projection 170 (same code).
// Round 103 (m3 lane 9, fast with 0x24590/0x30bf0, control 170): the rotation size as `a + PIN(delta, -step, step)` 170 (same
// code); `step * (real)acos(dot)` 170; the rotation delta clamped by if/else-if before `real size = a + delta;` 170 (aligned 220);
// the fallback store as `delta + current->n[component]` (retail adds into the delta register in its first component) 170 (same
// code); the fallback through PIN 188 (1025 bytes, the noted nested-ternary form); `first` and `second` declared at their
// normalizations 170 (same code).
// Round 108 (m3 lane 9, fast with 0x24590/0x30bf0, control 170): re-measured because losing 0xc2d00 is now accepted: the
// axis through cross3f 182, cross3f_staged 182, scalar rotated components with cross3f 189 (MiniMax R's 132 form). New: the
// rotated components staged as `= projection * axis.i; += cosine * first.i; -= cross * sine;` 170 (same code), with cross3f
// 182; three named scalar components 170 (same code; MiniMax R's named components, re-measure); `rotated.i * size` stores
// 170 (same code); `(1.0f - cosine) * dot` 170 (same code); the fallback loop index as `short` 250. Retail's rotated.i partial sums are written to +8 after each step and its result lands in first.i's slot.
// Round 111 (m3 lane 9, fast with 0x24590/0x30bf0, control 170): `first` rotated in place with only the j and k cross terms
// (the two that read the old first.i) named ahead of the rows and the i row's cross inline, as retail schedules them (j/k
// crosses early, the i cross late, rotated.i stored into first.i's slot) 181 at 1030 bytes (register+stack-masked aligned
// 116 -> 85; the dot's double copy, the angle's `fld st(0)` and the i row's partial-sum spills to +8 remain); the rotation
// size computed before the crosses 175 (1054 bytes). Same code as the in-place form (confirmations, not new forms): all three
// crosses named, the cosine term first in each row, the stores as `first.i * size`, the projection as dot3f(&first, &axis), each
// 181. Renaming a/b/dot/angle leaves the code byte-identical (170). Re-measured on this base: the fallback as three scoped or
// separately named per-axis blocks 166 each (1039 bytes, frame 0x34; round 524's 163 does not reproduce), with the cosine
// computed first 167, with the in-place rotation 178 (1028 bytes).
// Round 84 (m3 lane 13, fast with 0x24590/0x30bf0, control 170): retail's rotation is the game's rotate-about-axis idiom
// on `first` in place (matched 0x1ecf50 does it through a helper given sine and cosine). A private in-place helper (i, j, k
// copied, then written back; one caller, so match-only) 168; called with `(real)sin(angle), (real)cos(angle)` 168 (same
// code); with a result temporary copied back 208 (1077 bytes). Diagnostics, reverted at once: the dot volatile 189, the dot
// and angle volatile 191. The helper with round 221's acos-input copy 165 at 1045 bytes; on it: an angle copy for the sine
// 162, the copies taken before the projection 157 (first difference back at +0x30), cosine terms first 165, the size
// before the rotation 171. On 157: an angle copy for the sine 155; the axis through cross3f or cross3f_staged 157. On 155:
// cross3f 155 (same code), cross3f_staged 155, a copy for the cosine too 157, the raw current length named 259, a separate
// target reciprocal 155 (same code), the angle product in SSE 155, the fallback as three blocks 155 (same code), the cross
// term added with its sign flipped 184 (1019 bytes). Left on 155: the raw length and the dot share retail's +0xc; ours
// keeps the length in a's slot, which permutes the first/axis/second homes.
// m3 lane 8 r168 (fast with 0x24590/0x30bf0/0x1ecf50): `0.9999f > fabs(dot)` 170 (same code); acos on a fresh dot3f(&first,
// &second) 198 (1039 B); the rotation through the existing shared rotate_direction (forced inline in src/unknown_1eb8a0.cpp, called
// from three other files) on `first` in place 181 (1030 B), with `real cosine = dot;` for acos 182 (1026 B, frame 0x34), with
// vector3f_scale for the output 181 (same code). Body unchanged.
// M3 lane 3 r243 (fast with 0x24590/0x30bf0, both matched; control 170; a fast run without those two in the image
// gives 315 at 997 B for unchanged code, so list them): code-changing: the fallback clamp as three arms that each
// update the component (`-= step`, `+= step`, `+= delta`) 213 (1081 B); the fallback index as `char` 250 (965 B); the
// fallback's upper test first 181 (1055 B); the fallback as a do/while on `++component < 3` 250 (965 B); the target's
// clamped length taken after the first normalization 243 (first difference +0x14). Same code (170): magnitude3d's
// result through a named local (0x24590 still matched); one function-scope `delta` for the rotation size and the
// fallback; `a` and `b` declared together and assigned after; a `dword` fallback index. Body unchanged.
// M3 lane 5 r186 (fast with 0x24590/0x30bf0/0x3dfe0, control 170): `a`/`b` renamed to a source/target length pair
// 170 (identical code: names do not move the raw length out of a's slot); the length tests ahead of the parallel test
// 182 (the double copy of the dot stays, fabs and acos now adjacent); acos on a fresh `dot3f(&second, &first)` in the
// stored dot's operand order 198 (1039 B, as the swapped form); the fallback clamp as two independent ifs 192 (1021 B);
// the cosine declared before the sine 170 (different code, same count). Body unchanged.
// M3 lane 15 r92 (fast with 0x24590/0x30bf0, control 170): the dot named `cosine` and reused for the cosine of the
// angle 170 (different code: the raw current length leaves a's slot for +8, the double copy of the dot stays); the dot
// written at both uses (no local) 198 (1039 B); `angle = acos(dot); angle *= step;` 170 (scheduling only); the angle
// variable reused for the cosine 168 (the name does not fit; sine and cosine still share one x87 load, `fld st(0)`);
// one local for the dot, the angle and the cosine 187. Retail loads the angle from memory once for fsin and again for
// fcos, as ours does in 0x1161f0 where the angle arrives in xmm0 from a call; every form here keeps the x87 duplicate.
// MiniMax C (11-16): no new form tried this round, no lower score (170): the body is retail's length in the twenty-address image (1045
// MiniMax C (11-16): bytes) and the blocks' order still differs in the ways the notes above record.
// M3 lane 16 r129 (fast with 0x24590/0x30bf0, and 0x1ecf50 when the shared rotate helper is used; control 170): the parallel
// test on a fresh `dot3f(&second, &first)` with acos on the stored dot 198 (1039 B, first difference +0x9); the rotation
// through the existing rotate_direction on `first` in place with a `cosine = dot` copy for acos and an angle copy for
// the sine 178 (1028 B; without 0x1ecf50 in the fast image the helper stays a call, 388 at 820 B). Retail's rotation
// writes each component's partial sums to +8 and the result into first's slot, sine at +0x10 and the cosine over the
// angle at +0xc.
// Separate lower/upper current magnitude stages 268, with exact widened minimum 268, with target
// stages 283; widened minimum on repeated current magnitude 171; sign-selected dot magnitude 195,
// inline 195, with staged current bound 274; both reciprocals defined before either normalized vector
// 170. The raw length still shares the clamped slot and the dot keeps a promoted copy in the restored
// body.
// Shared clamp interval 170; separate first lower-bound read for current/target/both 170/170/170;
// preset ceiling and selected fresh in-range read for current/target/both 185/215/222; both with
// shared interval 222. NOT kept: ties or regressions; raw-length and promoted-dot homes remain.
// F408: Two-direction magnitude loop 294; magnitude and reciprocal loop 292;
// original magnitude with scale/point-setting loops 190/190; separate magnitude and
// normalization loops 297/296; fused magnitude and normalization with the scale helper
// or component products 349/353. NOT kept: all regress. Raw-length and promoted-dot homes remain.
// Round 462 scoped angle producer 170, with named sine/cosine 170, immutable cosine complement
// and projection 170, separate immutable trig results 170, direction/axis/projection names 170,
// with scoped angle 170, bounded lengths ahead of scoped reciprocals 170, with scoped angle 170.
// The raw current magnitude still shares the bounded length's slot and the dot promotion persists.
// MiniMax R (11-17): no new form, no lower score (170): the reduced image is not faithful here (it leaves the callee unresolved, so the body reads 315), and with the callee in the image the first difference is the float comparison this build keeps in esi.
// Complete raw tangents before/after sine, sine-weighted tangents before cosine
// or before projection, each with original projection or its dot before cosine:
// 170/170/170/170/170/170/170/170. Raw length and promoted-dot homes unchanged.
// Raw axis assembled or initialized before the angle 237/237, or between inverse cosine and
// its scaling 241/241. Complete axis product pairs before the angle or across its inverse
// cosine and scaling 170/241/196/241. NOT kept: tie or regressions. The raw magnitude still
// shares the clamped length's slot on the baseline frame and the dot still widens to a spill.
// MiniMax A (11-18): no new form kept, no lower score (170): retail keeps the raw current magnitude in a slot of its
// own ([esp + 0xc]) and reloads it for both clamp tests, while this build keeps it in the x87 stack and lets the
// clamped value take its place; a named raw magnitude local measures 251 and the same value in a two-element array
// also 251 (the array is promoted away), so the raw length's home stays as it is.
// Tried current/target/both vector initializers 170/170/170, axis at outer scope 170, rotated initializer 170,
// named absolute dot 196, tangent with shared/staged cross 186/186, angle after normalizer 246,
// shared projected scale and sum 183.
// Round 144 (m3 lane 12, fast with 0x24590/0x30bf0, control 170): the parallel test as `(real)fabs(dot) < 0.9999f` 170
// (same code); with the angle as `(real)acos(dot) * step` too 170 (other code); explicit `(double)dot` at fabs and acos 170
// (the 0.9999 constant then loads as a qword); the current length clamped through the file's existing forced-inline
// three-argument clamp (its value parameter names the raw length) 251 (1037 B, as round 221's named raw length: the raw
// length stays on the x87 stack and is stored straight to `a`); both lengths through it 320 (977 B: the target's three
// expansions fold into one). Body unchanged.
// Refresh the raw length at the upper bound, current/target: reuse it 170/228,
// use a fresh in-range magnitude 170/170, floor default 262/246, ceiling default 170/233.
// The raw current length and promoted-dot homes remain. Original clamps restored.
// Ordered signed dot bounds, lower/upper first 191/192; lengths first 191/191;
// fallback-first for the same four forms 191/192/191/191. NOT kept: the two
// comparisons replace the x87 absolute test and change the frame.
// M3 lane 8 r191 (fast with 0x24590/0x30bf0, control 170): the rotation through the shared in-place helper of
// include/axis_rotation.h on `first` 181 (1030 B; its projection sums i first, retail's dot k first); with a named float
// copy of the dot for the arccosine 182 (1026 B, frame 0x34); `dot` or `first`/`second` declared at the top 170/170 (same
// code); the fallback first under the negated test with its own return 170 (same code). Body unchanged.
// M3 lane 8 r196 (fast with 0x24590/0x30bf0, control 170): the arccosine through a new inline helper taking the dot
// as a real 170 (the double copy stays: inlining exposes the same conversion); the dot as `dot3f(&first, &second)` 170;
// the arccosine of the pinned dot 292 (1091 B). Diagnostic: the two lengths, or the dot/angle/sine/cosine, under other
// names 170/170/170 (no home moves: the raw current length still shares the clamped length's slot).
// Length gate before dot/current unit/target unit/both units 198/204/203/204.
// Separate current-first magnitude/unit gates 267/202, target-first 271/267.
// NOT kept: all retain frame 0x38 but regress. The raw current root and promoted
// dot still occupy different homes from retail.
// M3 lane 4 r227 (fast with 0x24590/0x30bf0, both MATCH; control 170; a/s/r = LCS-aligned unmatched lines exact /
// stack-masked / register-masked, control 137/135/67). The listing agrees with retail line for line up to +0x30: the
// only first-block difference is the raw current length's rounding store (`fstp` to a's home +4 where retail uses
// the transfer cell +0xc that later holds the dot, the angle and the cosine). The current length as `(real)sqrt(
// dot3f(current, current))` 170 (same code); `real a, b;` declared first and assigned by the two PINs 170 (same code);
// a float copy of the dot for acos (round 221's form, re-measured) 170 (1037 B, 149/134/66); that with a named raw
// current length 267 (1033 B); the sine, the projection dot, the cosine and then `projection *= 1.0f - cosine` 170
// (same code); the rotated rows stored straight into `current` with the size computed first 196 (1038 B). Body
// unchanged.
// Current reciprocal before target measurement 233. Complete current unit before the
// target 243, with initializer 243, paired scale calls 247, or separate reciprocal scopes 243.
// One root in the current, target, or both direction phases 251/309/324.
// NOT kept: the phased forms move the first difference to +0x14, and the single-root
// forms shorten the body. The separate raw-length home and real dot reload still differ.
// Named floor, ceiling or both comparison outcomes for the current and target lengths, all four together,
// and their complementary outcomes: 170 each. NOT kept: all forms generate the opening code.
// Shared squared-length producer under the current or target root at the lower
// test, upper test, in-range result, or all three sites: eight at 170.
// The raw current root still shares the bounded slot; the promoted dot stays.
// Fused angle after the first/second/all axis components 201/213/237. Unscaled arccosine kept across
// first/second/all axis productions 216/217/241. Arccosine after the first component, with final angle after
// the second/all 217/241. NOT kept: the transfer slot remains displaced and every score rises.
// M3 lane 1 r146 (H2M_FAST, control 170; the reduced image now gives 170 like the full build): one `cosine` local for the
// dot (the cosine between the unit vectors) and the step angle's cosine 170 (the raw current length then shares the dot's
// transfer cell +8 as retail shares +0xc, but the dot's double copy stays at +0xc); the angle held in that local as well
// 170 (propagated); the dot read through a `real const &` for the test and the arccosine 172; `fabsf` 170, `acosf` 170 and
// both 170 (the header's float overloads already route there); the parallel test on a recomputed dot3f 198 (1039 B: the
// products move to the x87) and the arccosine of a recomputed dot3f 198 (the same). Reading: retail keeps the dot in a
// float home and loads it twice (fabs, then acos), where ours converts once and keeps the double across the a/b tests.
// M3 lane 2 r132 (h2m fast with 0x24590 and 0x30bf0, both matched; control 170). Reading: retail reloads the float
// angle from its home for `fsin` and again for `fcos` (ours loads it once and duplicates it, `fld st(0)`), as it reloads
// the float dot for the arccosine. Forms: the cosine computed before the sine in two statements 170 (same code); the two
// length tests ahead of the parallel test 182; the parallel test on `(real)fabs(dot)` 170; the angle as
// `step * acos(dot)` 170; the fallback clamp through the existing PIN macro 188 (1025 B). Diagnostic (flags, reverted at
// once): the file built with /Op 558 (1296 B), so the reloads are not float-consistency code. Restored.
// M3 lane 3 r299 (H2M_FAST with 0x30bf0 and 0x24590; control 170, difflib-aligned 187): the angle as a double local 198
// (1053 B); a double copy of the angle for the sine and cosine 249 (1009 B); the arccosine named as a double ahead of the
// step product 196 (1051 B); a float copy of the angle for the cosine 166 (1045 B, aligned 183; a one-use copy, round
// 221's form again); that with a float copy of the dot for the arccosine 166 (1039 B, frame 0x34); the parallel test on a
// named `(real)fabs(dot)` 170 (same code). Reading unchanged: retail loads the float dot and the float angle from their
// homes at each of the two uses, ours converts once and keeps the wide value.
// M3 lane 5 r242 (H2M_FAST with 0x24590 and 0x30bf0, both matched; control 170 at 1045 B; A = instructions left unpaired
// after aligning the two listings, control 196). The equal lengths are a coincidence: ours is about 20 bytes long in the
// fallback and as short in the rotation. Fallback: the clamp inside the add, `current->n[component] += PIN(delta, -step,
// step)` (lane 2 r132's 188, re-read) 188 at 1025 B, A 156: the block then has retail's registers and branch shapes
// (two jbe/jmp clamps, then the ja/ja select with -step's register reused for the last component) and only the three
// adds differ (retail adds the component into delta for i and loads the component for j and k; ours the reverse); the
// same code as `= current->n[component] + (...)`, `= (...) + current->n[component]`, the ternary written out, or PIN
// over the difference itself with no delta local; `delta = PIN(...)` then `+= delta` is the control's code (170). On
// the PIN form: `acos((double)dot)`, `sin((double)angle)` and `cos((double)angle)`, `step * (real)acos(dot)`,
// `acosf`/`sinf`/`cosf`/`fabsf`, `angle = (real)acos(dot); angle *= step;` 188 each (A 156 to 158). Rotation: retail
// forms the two cross components that read first.i ahead of the first component's sum and the third at its use, and
// stores the first rotated component into first.i's home, which fits `first` rotated in place with the cross taken
// first: cross3f(&first, &axis, &tangent) then `first.i = projection * axis.i + cosine * first.i - tangent.i * sine`
// (j, k alike) and `size * first.i` 186 (A 247), with the PIN fallback 202 at 1011 B (A 207), with the cross ahead of
// the projection 202 (A 177; the operand loads then go to registers as retail's, and one float home is still missing);
// the cross written out into the temp is the same code. One variable for the dot and the angle 170 (same code), for
// the dot and the cosine 170 (A 208), for all three 187 and for the angle and the cosine 168 (lane 15 r92's, re-read:
// A 226 and 204, no reload appears). On PIN + in-place + early cross, float copies of the dot (for the arccosine) and
// of the angle (for the cosine) 197 at 1009 B (frame 0x34, A 202: both reloads appear but the wide home goes and
// nothing takes its place). Diagnostic, reverted at once: the dot and the angle written through a by-address inline
// setter, the same code as without. Body unchanged: every structural step raises the positional score.
// M3 lane 11 r166 (h2m fast with 0x24590 and 0x30bf0, both matched; control 170, aligned 187, with registers, XMM numbers
// and stack offsets masked 116). Frame reading: cells are packed by reference count, each object joining the newest slot it
// does not overlap. Retail's cosine has four references (stored, then read from memory three times) and its spill temp four,
// so their slots (+8, +0xc) exist before the three-reference raw length, the dot and the angle are placed, and all three
// join the cosine's cell; here the cosine keeps a register (two references), so the raw length joins `a`. Retail's dot,
// angle, sine and cosine all live in memory and are loaded at each use. The rotation through the existing in-place helper
// of include/axis_rotation.h with `(real)sin(angle), (real)cos(angle)` as its arguments, then `size * first` (retail
// stores the first rotated row into first.i's home): 181 (1030 B, aligned 182, masked 85; lane 8 r191's form re-measured
// by the masked count; build/r166/lead-0x3e5a0-rotation-helper-181.patch); with named sine and cosine 181 (same code);
// with the result through vector3f_scale 181 (same code); the cosine computed first 183; the angle as `(real)acos(dot) *
// step` on it 181 (masked 84); the two length tests ahead of the parallel test on it 193. Forcing single allocator
// decisions (14 of 627 tried) leaves 170. Body unchanged.
// Named lower step in fallback, rotation or both 170/175/186; shared lower step from entry 294.
// Step interval in fallback, rotation or both 170/175/186; shared lower step with staged rotation
// clamp 186. NOT kept: fallback-only forms tie, and sharing the step bounds changes the frame.
// The raw current length still uses the bounded length’s slot in the fallback-only forms.
// Joint length tests, joint angle/current-length test, independent target-length test, independent
// angle test, all three tests together, eager lengths before angle, ordered lengths with independent
// angle, and joint lengths with real absolute dot: 190/190/186/186/205/201/198/190.
// NOT kept: all grow the body and leave the raw current magnitude in the bounded length's slot.
// Shared projected-axis scale with cosine-scaled add or separate cosine vector plus sum, each in
// place or with separate projected storage: 183/196/183/196. With the complete shared tangent:
// 170/197/170/197. NOT kept: the two 170 forms tie at 1045 bytes and still store the raw root
// at +4, leaving the real root, dot and cosine transfer slot unresolved.
// Complete size selected in each arm, accumulated from the current length, or selected by a
// nested or full conditional: 169/169/169/169. With a shared lower step bound: 185/185/185/185.
// NOT kept: 169 gains one and grows to 1050 bytes. The raw root and promoted dot still spill
// to different homes from retail.
// Clamped arccosine helper, PIN at arccosine, lower-first and upper-first input selection:
// 292/292/292/292. Dot bounded before both uses 287; lower and upper inverse-cosine bounds
// alone 211/266; axis built before clamped arccosine 191. NOT kept: the new input boundary
// changes the frame or extent and the raw magnitude still lacks its retail home.
// @retail 0x3e5a0
void function_3e5a0(vector3f *current, vector3f const *target, real step)
{
    real a = PIN(magnitude3d(current), 0.0001f, 10.0f);
    real b = PIN(magnitude3d(target), 0.0001f, 10.0f);
    vector3f first, second;
    real inverse = 1.0f / a;
    first.i = current->i * inverse;
    first.j = current->j * inverse;
    first.k = current->k * inverse;
    inverse = 1.0f / b;
    second.i = target->i * inverse;
    second.j = target->j * inverse;
    second.k = target->k * inverse;
    real dot = dot3f(&second, &first);
    if (fabs(dot) < 0.9999f && a > 0.0001f && b > 0.0001f)
    {
        real angle = (real)(acos(dot) * step);
        vector3f axis;
        axis.i = second.k * first.j - second.j * first.k;
        axis.j = first.k * second.i - second.k * first.i;
        axis.k = second.j * first.i - first.j * second.i;
        function_30bf0(&axis);
        real sine = (real)sin(angle), cosine = (real)cos(angle);
        real projection = dot3f(&axis, &first) * (1.0f - cosine);
        vector3f rotated;
        rotated.i = projection * axis.i + cosine * first.i - (axis.k * first.j - axis.j * first.k) * sine;
        rotated.j = projection * axis.j + cosine * first.j - (first.k * axis.i - axis.k * first.i) * sine;
        rotated.k = projection * axis.k + cosine * first.k - (axis.j * first.i - first.j * axis.i) * sine;
        real delta = b - a;
        real size = a + (delta < -step ? -step : delta > step ? step : delta);
        current->i = size * rotated.i;
        current->j = size * rotated.j;
        current->k = size * rotated.k;
    }
    else
    {
        for (long component = 0; component < 3; ++component)
        {
            real delta = target->n[component] - current->n[component];
            if (delta < -step) delta = -step;
            else if (delta > step) delta = step;
            current->n[component] += delta;
        }
    }
}



extern dword g_4ba034;
struct s_cached_input
{
    dword data[5];
};
struct s_cached_input_entry
{
    s_cached_input input;
    real elapsed;
    real initial;
    dword last_seen;
    dword created;
};
struct s_cached_input_list
{
    s_cached_input_entry entries[32];
    long count;
};

struct s_masked_list;
struct s_light_shape_ab;
bool function_31590(long index, s_light_shape_ab *shape);
bool function_3e9c0(long object_index);
extern s_record_pool *g_4e030c;
void function_4a7c0(s_cached_input_list *state, s_cached_input const *inputs, long count);
long __stdcall function_4ac30(s_cached_input_list *state, s_cached_input *output, long limit, real step);
s_cached_input_list g_5234e8[4];

// The loop as matched 0x33400 writes it: a long total and counter with the index list read at `(short)i` (retail
// keeps the counter 32-bit and indexes with movsx); the clamp as `!(depth >= 0.0f)` and `&g_5234e8[view]` at both
// calls (0x33400 again); the object read through the pool expression beside the entry pointer, as matched 0x1321f0
// reads a light field again beside its pointer (it gives retail's one `lea` for the entry and the object load ahead
// of it). Each is needed (any one undone scores 2 to 118).
// @retail 0x31910
long __stdcall function_31910(s_cached_input *output)
{
    long count = 0;
    long view = g_4b9ed4 < 0 ? 0 : g_4b9ed4 > 3 ? 3 : g_4b9ed4;
    byte *list = (byte *)g_547f98;
    long total = *(short *)(list + 4);
    for (long i = 0; i < total; ++i)
    {
        long index = (*(long **)(list + 0xc))[(short)i];
        byte shape[0x7c];
        if (index != NONE && function_31590(index, (s_light_shape_ab *)shape) && count < 128)
        {
            byte *entry = g_4e030c->data + (index & 0xffff) * 0x110;
            byte *definition = g_4e3b44[*(long *)(entry + 4) & 0xffff].bytes;
            long object = *(long *)(g_4e030c->data + (index & 0xffff) * 0x110 + 0x4c);
            long parent = function_baf80(object);
            point3f position = *(point3f *)(entry + 0x18);
            real radius = *(real *)(entry + 0x24);
            s_cached_input *result = &output[count];
            result->data[1] = index;
            result->data[2] = parent;
            result->data[3] = *(dword *)(entry + 0x100);
            real depth = g_4b9e2c * position.y + g_4b9e20 * position.x + g_4b9e38 * position.z + g_4b9e44;
            if (!(depth >= 0.0f)) depth = 0.0f - depth;
            if (!(depth > 0.1f)) depth = 0.1f;
            real score = (g_4b9ed0 / depth) * radius * 2.0f;
            *(real *)&result->data[4] = score;
            long object_flag = object != NONE && g_4e0300->data[(object & 0xffff) * 12 + 3] == 2;
            ((byte *)result)[2] = (byte)object_flag;
            ((byte *)result)[0] = function_3e9c0(parent);
            ((byte *)result)[1] = (byte)((*(dword *)definition >> 5) & 1);
            ((byte *)result)[3] = (byte)((*(dword *)definition >> 20) & 1);
            ++count;
        }
    }
    function_4a7c0(&g_5234e8[view], output, count);
    return function_4ac30(&g_5234e8[view], output, 3, 0.01f);
}

// Matches when function_117dd0's source (flexible_surface.cpp, which takes g_4ba034's address) is in the
// image: retail reloads the frame count from memory, and the other sources only do the same once that
// global is address-taken (check --fast 0x4a7c0 alone: 352). The simple counted loop is unrolled by the
// compiler; written out by hand it scored 385.
// @retail 0x4a7c0
void function_4a7c0(s_cached_input_list *state, s_cached_input const *inputs, long count)
{
    dword used = 0;
    s_cached_input_entry *entry = state->entries;
    for (long i = 0; i < state->count; ++i, ++entry)
    {
        for (long j = 0; j < count; ++j)
        {
            if (entry->input.data[1] == inputs[j].data[1])
            {
                entry->input = inputs[j];
                entry->last_seen = g_4ba034;
                used |= 1 << j;
                break;
            }
        }
    }
    for (long k = 0; k < state->count; ++k)
    {
        if (state->count > 0 && state->entries[k].last_seen != g_4ba034)
        {
            state->entries[k] = state->entries[state->count - 1];
            --state->count;
            --k;
        }
    }
    for (long n = 0; n < count; ++n)
    {
        if (state->count < 32 && !(used & (1 << n)))
        {
            state->entries[state->count].input = inputs[n];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = *(real *)&state->entries[state->count].input.data[3];
            state->entries[state->count].elapsed = 0.0f;
            ++state->count;
            used |= 1 << n;
        }
    }
}


struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

static void swap_cached_input(s_cached_input_entry *a, s_cached_input_entry *b)
{
	s_cached_input_entry temporary = *a;
	*a = *b;
	*b = temporary;
}

static void clamp_cached_input(s_cached_input_entry *entry)
{
	real *maximum = (real *)&entry->input.data[3];
	entry->initial = entry->initial < 0.0f ? 0.0f : entry->initial > *maximum ? *maximum : entry->initial;
}

// MiniMax G (11-20): no new form, no lower score (317): the body is retail's 1324 bytes and only the register plan differs - retail keeps the state pointer in ebp and the swap counter in edx (homes [esp + 0x10] and [esp + 0x14]) while this build takes ebp for the counter and edx for the state, with the two homes swapped.
// MiniMax A (11-20): no new form, no lower score (317): the prefix and state registers and the ordinal homes
// score 317 (was 317): The 0x30 frame and 1324-byte extent agree. The exit span first differs at +0x15. Retail
// holds the retained prefix in EDX and the list in EBP after the limit gate, with the creation ordinal in EBP
// later. This build exchanges the early register roles and the prefix and ordinal homes. The swap expansions
// consume ECX, ESI and EDI and influence which values interfere across the copy. The trials test their real
// copy operands and the point where the creation ordinal becomes live.
// Earlier at 317 (was 317): EBP and EDX hold the prefix and state in reverse order, with swapped ordinal homes.
// MiniMax E (11-16): no new form, no lower score (317): our body is exactly retail's 1324 bytes and the whole difference is the register allocation - retail keeps the state pointer in ebp and the swap counter in edx with their homes at [esp + 0x10] and [esp + 0x14], this build takes ebp for the counter and edx for the state with the homes swapped.
// MiniMax S (11-14): no new form, no lower score (317): the permuter's search over this body (29 tries, 304 s, reduced image and full image) found nothing below the baseline's 317; retail zeroes edx and keeps the block's base in ebp where this build zeroes ebp and keeps it in edx, and the two read the stack slots in the other order, so the frame's shape and the register plan have to change together.
// MiniMax B (11-13): no new form, no lower score (317): retail zeroes edx and keeps the block's base in ebp where this build zeroes ebp and keeps it in edx, and the two then read the stack slots in the other order; the frame's shape and the register plan have to change together.
// MiniMax G (11-12): no new form, no lower score (317); both builds spill the counter and the entries
// base to stack homes, so only the two registers differ (retail: the counter in edx and the base in ebp;
// ours the other way round), and the base is a load of state->entries rather than a local, so declaring
// the locals in another order cannot move it.
// MiniMax H (11-14): no new form, no lower score (317): retail keeps the admission counter in edx and the entries
// base in ebp where this build keeps the counter in ebp and the base in edx, and the two stack homes are the other
// way round; the permuter's search (18 tries, 308 s) reached 316 with the first block nested, one point short of
// the keep bar.
// MiniMax D (11-14, run 90): no new form, no lower score (317): the names and the declaration order do not move the
// first/i slots here, and what is left is the frame's homes, which the compile-only copy reproduces as they stand;
// re-measured at 317 (1324 bytes, as retail's).
// MiniMax R (11-16): no new form, no lower score (317): retail keeps the admission counter in edx and the entries base in ebp where this build keeps them the other way round, and the two read their stack homes in the other order, so the frame's shape and the register plan have to change together; the names and the declaration order cannot move the first/i slots.
// r136 lane 7: names and declaration order do not move the first/i slots here (compile-only
// screen); `first` declared after `found` 358; decay entry pointer before the `initial` pointer 377 (1343 bytes);
// decay without the `initial` pointer 417 (1259 bytes); growth `entry->initial += entry->elapsed;` 317; an
// `entries = state->entries` base local for every access 317.
// MiniMax H (11-06): no new form, no lower score.
// The four passes, both sorts, the decay and growth loops and the output copy all agree with retail; what
// is left is register allocation. Retail keeps the admission counter in edx and the entries base in ebp,
// while we keep the counter in ebp and the base in edx, and the two stack homes follow: retail stores the
// counter at [esp+0x10] and the base at [esp+0x14], we store them the other way round.
// Earlier 317 (was 317): the logic was walked against retail block by block (0x4ac30-0x4b159) and
// agrees: the four passes (pairs to the front, first flagged entry, drop of just-created flagged entries, duplicate
// key removal), both sorts (modes 1 and 2), the decay loop, the growth loop and the output copy. What is left is
// register allocation. Retail keeps `first` in edx and the state in ebp for the first three loops and reloads `first`
// from [esp+0x10] into ebp as loop 3's counter (0x4ad43); here `first` stays in ebp, the state is in edx and the
// counter lives in memory, which adds a few instructions and shifts everything after it. The decay loop's unrolled
// body is rotated one entry (base entry-0xc, first copy at +0x24) where retail's base is the first copy's `initial`
// field; its copies 2-4 match retail. Gains this round: the last (growth) loop is a counted do/while (retail ends
// it with dec ecx/jne, not a pointer compare), and the decay loop declares a pointer to `initial` before the
// entry pointer (377 -> 317 with the same bytes length as retail). Earlier: `initial` is a real, compare operand
// order, one-expression clamp. Tried: index loop `i < limit` for the growth loop (715, unrolled), pointer
// compare for the decay loop (367, divides by 36), decay loop as `--i >= limit`, `i = count; i > limit` with
// entries[i - 1] (349), a second pointer decremented alongside i (375), entry derived from `initial` (369),
// declaration order of first/last/requested/found/i (317-319), per-loop `long i` (317), an `entries` alias (317),
// a copy of `first` for the later calls (370), found set before/after the swap (360/318). Without the
// "last >= i" forms 390; "limit <= i" stops the unrolling (802).
// Round 487: persistent-count / ordinal / paired / complete role names 317 each; no separate found flag 436;
// prefix count initialized before admission 331; separate named phase ordinals 317; retained-only removal
// cursor advancement 318. All eight executable forms restored; frame 0x30 agrees, early prefix/state
// register roles and the two counter homes remain exchanged. Earlier scores were 405 and 424.
// Round 36: a compile-only build reproduces this listing (aligned 131 against retail); `first` takes EBP in all 60
// random combinations of declaration order, loop tests, flag order, per-loop counters, an entries alias and the
// clamp spelling, in renames, and even with its address taken (diagnostic), so the EBP/EDX choice is not a spelling.
// Retail's growth loop computes `elapsed + step`, stores it and re-reads elapsed for `initial + elapsed`; a pointer to
// `initial` there (the decay loop's form) gives the first half (harness 123) but 369 for real (1336 bytes).
// Real (fast): `i < last` in the first loop 321; found set before ++first 318; found declared first 355; the
// second loop as a search with the swap after it 365.
// J238: Actual paired/fallback admission scan cursors and stable-count subsets
// 320/403/437/425/437/411/411; complete graph with matched-sort inline declaration 392, declaration
// alone on original body 339. All restored. Frame 0x30 agrees; prefix/state and ordinal homes and
// rotated decay base remain.
// Round 308: native flag/key/real-maximum physical views, seven subsets 317 each; complete physical view
// Round 308: with direct maximum clamp 316 (below the keep gate). Self-aliasing memcpy swap excluded. Original physical type,
// Round 308: helpers and body restored; prefix/state roles and decay base remain.
// P272: Alias-safe full-record memmove capture/first-publication/second-publication subsets in the existing shared swap, then all with inline declaration: 481/484/509/499/510/509/463/463.
// Original executable source restored; frame 0x30 agrees; prefix/state registers, ordinal homes and decay base remain.
// F329: Output cursor, direct decay initial field and duplicate-scan cursor in seven
// subsets: 370/417/442/422/425/505/504; complete graph plus output source cursor 505.
// All restored; frame 0x30, prefix/state ranks and ordinal homes remain.
// DL247: Standard alias-safe swap with swap/clamp inline and completed clamp result:
// 568/568/568/568/600/600/600/600; original swap with independent helper inline
// subsets 317/317/317, completed result alone/with either/both 404/404/404/404.
// All restored; state/prefix homes and rotated decay base remain.
// DH299: Native nonnegative prefix and explicit post-swap tail decrements in both
// removal passes, seven subsets, then complete graph with byte found: 317 each.
// Original body restored; frame 0x30, prefix/state and ordinal homes plus decay rotation remain.
// DH301: Original requested parameter retained with a genuine clamped-limit local;
// branch/conditional clamp, safe entry parameter-copy and bounded unsigned-prefix
// combinations: 377/321/435/321/377/321/435/321. Complete original body restored; frame 0x30
// and retail length agree, prefix/state and ordinal homes plus decay rotation remain.
// P279: Requested-copy/found-initialization/uninitialized-tail declaration admission at entry,
// subsets 1/2/4/3/5/6/7: 413/335/317/413/413/335/413; all with the existing swap
// scratch declaration separated from assignment 413. Original complete source restored;
// frame 0x30 agrees, prefix/state and ordinal homes plus the rotated decay base remain.
// Round 64 (fast): a copy of `first` for the sorts 370; the third pass through state->entries[i] with no entry pointer 318;
// `--last` as its own statement in the third pass 317, in the fourth pass 317; first swap through the flags pointer 375;
// `entries[first++]` in the second pass 317; `first = i + 1` 442. `first` stays in EBP in all of them.
// DH337: Actual elapsed bindings in decay/growth and maximum-clamp field subsets
// 366/371/317/369/366/371/369; complete elapsed-pointer graph 369. Original helper
// and body restored; state/prefix/ordinal homes and first decay-copy rotation remain.
// Y569: Actual two-use duplicate key, symmetric computed elapsed results and long found
// domains, seven subsets, then complete byte found graph:
// 315/376/330/374/328/389/387/374. NOT kept: two-point key gain below gate; elapsed forms enlarge; complete original body restored;
// prefix/state and ordinal homes plus first decay-copy rotation remain.
// Y571: Creation-removal cursor, admitted creation/duplicate tail bindings, seven subsets
// 440/317/317/440/440/317/440; complete graph with loop-entry tail addresses 440.
// NOT kept: cursor shortens to 1289 bytes and frame 0x2c; one-use tail bindings miss equality.
// Original body restored; prefix/state and ordinal homes plus decay rotation remain.
// Y573: Nested admission, found last-use scope and immutable request; seven subsets and complete
// while scan: 317/317/317/317/317/317/317/317. NOT kept: all tie; native prefix/state and ordinal
// homes remain. Original complete body and helpers restored.
// Y575: Creation/duplicate/decay rejection continues, seven subsets, then negative-first
// growth: 317/317/317/317/317/317/317/367. NOT kept: ties or regression; native prefix/state
// and ordinal homes persist. Original full body, helpers, count reads and swaps restored.
// Y577: Indexed paired/fallback receivers and duplicate element reference, seven subsets,
// then complete graph with creation reference: 375/396/317/399/375/396/399/399. NOT kept: no equality;
// original full body restored. Native state/prefix and ordinal homes plus decay rotation remain.
// Y593: Original inclusive-tail missing-pair state, bool/byte/short/long with for/while
// first scan: 320/320/327/326/320/320/327/326. NOT kept: all regress at native length;
// original positive found state, full body and helpers restored; prefix/state homes remain.
// F387: One-past retained-tail boundary with found in bool/byte/short/long, then
// complemented missing state in those domains: 389/389/397/399/389/389/395/397.
// NOT kept: all regress at native length; prefix/state and ordinal homes remain. Original body restored.
// F390: Fallback zero-prefix scan start, fixed first-entry destination and completed prefix one,
// seven subsets then natural while fallback: 396/331/385/448/374/340/409/409. NOT kept:
// all regress; several shorten to 1273-1310/1324. Original complete body restored; native
// prefix/state and removal-ordinal homes plus decay rotation remain, both controls stable.
// F390: Existing four-use swap first/second/both actual-record references with default/ordinary
// expansion, complete forced expansion and complete clamp reference input: eight at 317. Original
// alias-safe copies, pointer contracts, helpers and whole body restored; frame 0x30, native
// prefix/state and ordinal homes plus decay rotation remain.
// F392: Distinct-entry admission in the existing four-use swap with standard capture/publication copy subsets, then original assignments: 407/407/407/407/407/407/407/407.
// NOT kept: no exact match. Original alias-safe assignment helper and full body restored; native state/prefix/ordinal homes remain unresolved.
// J259: Paired-entry result feeds prefix/found/both through bool and byte: 439/438/448/439/438/448; complete short/long results 449/442.
// NOT kept: no eligible native repair; opening executable source restored. Single-consumer forms grow frame 0x30 to 0x34; shared forms retain 0x30 but grow length and regress. Original admission, alias-safe swaps, counts and full body restored; native state/prefix/ordinal homes and decay-copy base remain.
// J261: Bounded prefix short/word 418/418 (frame 0x2c), signed-byte/byte 379/379 (native frame and extent);
// signed tail short/byte 435/426; positive growth remainder short/word 347/363. Short growth first differs at
// +0x33 but still regresses. Every form regresses; all controls preserved and original types restored.
// NOT kept: no eligible native repair; opening executable source restored. Native state/prefix and ordinal
// homes plus the decay-copy base remain unresolved.
// R221 (lane 3; whole-file harness, exact 317): loop 1 through an entry pointer 375 (1342); `i < last` in loop 1 321;
// a `count` local for the entry tests 370 (1338); `requested` declared first 319; `first == 0` for the found flag 436.
// R221 also: the clamp compare written `state->count < limit` 319.
// Round 64 (lane 15, fast): an index declared in each loop 317; `last` declared before `first` 318; `requested` and
// the clamp before `first`/`last` 416 (1305 bytes); swap helper `__forceinline` 317, clamp helper 317, both 317.
// Round 73 (m3 lane 13, fast; aligned | register-masked counts, base 81 | 59): `first` competes with loop 3's index for
// EBP and wins by one reference; when it loses, the state takes EBP in loops 1-2, `first` EDX, the index EBP in loop 3,
// as in retail. Diagnostics, reverted (they change what the code does): an extra unconditional use of the index in loop 3
// gives retail's registers and both stack homes with only its own two instructions extra (57 | 46); loop 2, 3 or 4
// started from 0 instead of `first` flips the registers too (loop 2: homes as retail; loops 3 and 4: homes still
// exchanged; loop 4 form 274); loop 1's increment removed flips; a call passing 0, loop 2's swap target as entries[0],
// or an extra index use in loop 2 does not. Forms tried, none flips: loop 1's flags read through the state at each use
// 370; loop 3 without the entry local 318, both 369; loop 2's `&state->entries[first++]` 317 (same code); one index
// declared at function scope and used by the output loop too 370; loop 3's swap from `&state->entries[i]` 368, from
// `&state->entries[i--]` 367, with `--last` as its own statement 317; loop 3 tested `i <= last` 367; its guard as
// `requested < state->count` 370; early `continue` 317; `--i` ahead of the swap 355 (flips, but the index then lives in
// memory, 424 instructions); a while form 317; the index set ahead of the first sort call, the call passing either
// variable 317 each (same code); the zero store through the state 317; the entry local declared inside the test 367;
// the created test through the state 375. Next: a natural form that adds one use of loop 3's index or drops one
// `i = first` without changing the instructions.
// MiniMax E (11-09): no new form, no lower score (317); retail's empty-entry exit jumps three bytes further into the function than ours (retail +0x4cf, ours +0x4cc).
// R229 (m3 lane 3, fast): decay `*initial += entry->elapsed;` 317 and `*initial = *initial + entry->elapsed;` 317 (same code: the
// first unrolled decay copy keeps its own load of `initial` either way); growth `entry->initial = entry->elapsed + entry->initial;`
// 317 (same code); loop 1 as a while loop 317 (same code); loop 3 walking its own entry pointer (`++i, ++entry` with `--entry`
// beside `--i`) 385. Left as before: `first` takes EBP over loop 3's index (homes exchanged), and the unrolled decay loop's base
// sits one entry low (ours entry-0xc, retail the entry's `initial`).
// MiniMax R (11-10): no new form, no lower score (317): retail's empty-entry exit jumps three bytes further into the function than ours, so the loop's block layout still differs; the forms tried before (the index inside the test, the swap's direction, a while form and an early continue) all measured the same code.
// MiniMax E (11-09): no new form, no lower score (317); retail's empty-entry exit jumps three bytes further into the function than ours (retail +0x4cf, ours +0x4cc).
// R149 (m3 lane 7, fast; the score-4 recipe is held, not kept): three causes. (1) Retail's third pass is not
// marked for loop alignment (its head at 0x4ad5d needs a 3-byte pad and has none; ours jumps over a 2-byte pad): the
// loop condition repeated at the top of its body (`last >= i &&`) is jump-threaded away, the loop loses its own label
// and passes 1-4 and both sort calls take retail's registers and homes, 227 (1305 B). Not keepable: the test is always
// true where it stands. Real tests there give the registers but keep a compare (`i >= first`, `i >= 0`); `last >= 0`,
// `last >= first`, 0x4a7c0's `state->count > 0` guard, `i <= last` as the loop test, `--last` alone, the created test
// first and no entry pointer keep the pad and `first` in EBP (compile-only screen). (2) The unrolled decay loop's pointer
// sits one entry low whenever the entry's address is held in a pointer (the entry local, or clamp_cached_input's
// parameter); every access through state->entries[i], the sum stored through a `real *initial` that also feeds it, and
// the clamp as PIN on the field give retail's decay loop (on (1): 85). (3) The growth loop folds the stored elapsed value
// and the clamp's reloads; the sum stored through `real *initial`, elapsed read through `real *elapsed` and PIN on
// entry->initial give retail's (on (1) and (2): 4 at 1324 B; left: the two reloads after pass 4's swap and the growth
// loop's first two instructions come in the other order under LTCG). These are one-use pointers and a written-out
// helper as well, so match-only. On (1) alone: decay with an entry pointer only 298 (1243 B), entry pointer declared
// first 227, `initial` taken from the entry pointer 216 (1327 B), only the `initial` pointer 215, `initial` read through
// state->entries[i] 209, `while (--i >= limit)` 227, counted from count with entries[i - 1] 175; (1)+(2) with the sum
// read through state->entries[i] and the growth form 110 (the decay remainder copy then loads initial separately). On the
// 4 form: 0x4b160's parameters as (entries, first, mode, last) 4 (same code, 0x4b160 still matched); the growth loop's
// count declared before its pointer 4 (same code).
// A 600 s fast annealing permuter run on the 4 form: 45 tries, none below 4.
// R151 (m3 lane 7, fast with 0x4b160): the fourth pass comparing with `entry[-1]` for the previous entry 367 (1335 bytes);
// the fourth pass without its entry pointer (every access through state->entries[i]) 375 (1342 bytes). Pass 2's start
// cannot be spelled 0 even though `first` is 0 there (retail reads it: `mov eax, edx`), and the other natural spellings
// of passes 1-4 are listed above; the EBP choice still needs the ruled-out repeated loop test. The clamp helper reading
// the maximum field at each use (no pointer local) 316 (as round 308's direct maximum clamp), the same clamp written at
// both call sites 316 (same code); the growth step as `elapsed = elapsed + step; initial += elapsed` 317 and the fourth
// pass's `-step` 317 (same code each). Retail's growth loop re-reads the stored elapsed (`initial + [elapsed]`) where its
// decay loop and ours forward it. Diagnostic: `#pragma optimize("p", on)` around the function 317 (no change).
// Round 84 (m3 lane 13, fast with 0x4b160, control 317; a/m = LCS-aligned / register-masked unmatched lines, control a81
// m59): a true `__assume(last >= i)` at loop 3's top 367 (1335 bytes; it is not threaded away as the repeated test is);
// loop 3's flag test through a `byte *flags` local as loop 1 does 375 (1342 bytes), with every access through
// state->entries[i] 318; probes on main's base, match-only: R149's decay-loop form 364 (1335 bytes, a60 m36), with its
// growth-loop form 387 (1353 bytes, a60 m33). The `__assume` form compiles exactly as R151's `entry[-1]` fourth pass and the
// flags-pointer form without the entry local exactly as loop 3 without its entry local (built and compared), so both are
// confirmations. Also: R149's decay form keeping clamp_cached_input(&state->entries[i]) 317 (different code, a84); the
// decay loop through state->entries[i] with no `initial` pointer and the clamp as PIN 418 (1255 bytes); R149's growth
// form alone 390 (1353 bytes).
// MiniMax A (11-12, run 62): no new form, no lower score (317); a named local for the state's array pointer used
// at every access grows the body to 1338 bytes (370) and an unsigned `first` is the same code, so what is left is
// retail's home order (first at +0x10 and the loop index at +0x14, ours the reverse) with the state pointer in EBP
// and `first` in EDX where we take EBP for `first` and EDX for the state.
// M3 lane 10 r104: no trial. The open lead above (a natural form that adds one use of loop 3's index or drops one
// reference to `first`) was checked against every reference of both: `first` is read by the loop 2 start, the loop 2
// swap target (hoisted by retail too), the loop 3 and loop 4 starts and both sort calls, all of which retail reads; the
// natural spellings that add a use of loop 3's index (entry through the index, the decrement before the swap, the swap
// from the indexed entry, `i <= last`) are listed above with their scores, and none keeps the instructions. The loop
// tests of passes 1, 2 and 4 already compare in retail's operand order. Body unchanged.
// P508: Complete inclusive admitted-prefix end in long/dword, each with bool/byte/short/long
// original found state: 358/358/361/358/358/358/361/358. NOT kept: all regress at native length;
// state/prefix and ordinal homes remain wrong. Original count representation and full body restored.
// M3 lane 12 r126 (fast with 0x4b160, MATCH each time; control 317, register-masked aligned 105): the third pass as
// `i = first; while (last >= i)` with `else ++i;` and no `--i` 318 [113]; the swapped-in entry re-tested by an inner
// `while (last >= i && ...)` (no `--i`) 368 (1303 bytes) [101]; the third pass as `if (last >= i) do { ... } while (last
// >= ++i);` 370 (1338 bytes) [106]. `first` keeps EBP in all three; retail's third pass is the `--i` form (its body runs
// `dec ebp` and then `inc ebp`). Body unchanged.
// M3 lane 13 r97 (fast with 0x4b160, MATCH each time; control 317): retail's homes are `first` +0x10 and the index +0x14,
// ours the reverse, so names were tested in case same-size slots sort by name: `first` renamed start/retained 317/317,
// the index renamed cursor/entry_index 317/317 (all the same code). Declaration orders: the index declared first 317,
// after `last` 317 (same code); the index and `found` first 355; `found` after `last` 355.
// MiniMax E (11-15): no new form, no lower score (317): our body is exactly retail's 1324 bytes and the whole difference is the register allocation: retail keeps the state pointer in ebp and the swap counter in edx with their homes at [esp + 0x10] and [esp + 0x14], this build takes ebp for the counter and edx for the state with the homes swapped.
// MiniMax C (11-16): no new form tried this round, no lower score (317): the body's length matches retail's (1324 bytes) and the blocks'
// MiniMax C (11-16): order still differs in the ways the notes above record.
// M3 lane 4 r206 (fast with 0x4b160, control 317): pass 2 ended by the found flag in its own test (no break, no
// `if (!found)`) 394; the clamp as a value-returning helper at both loops 383 (1287 bytes); pass 1's `++first; found =
// true;` ahead of the swap 360; pass 3's two tests as nested ifs 317 (same code); pass 3's tail as `swap(entry,
// &entries[last]); --i; --last;` 316 (only the tail's instruction order moves). Body unchanged.
// M3 lane 11 r111 (fast; compile-only screen in brackets, which reproduces this listing but for the 0x4b160 register
// setup): a redundant `if (first <= last)` before pass 4 [flips `first` to EDX, state EBP, homes +0x10/+0x14 as retail,
// aligned 164 -> 115] 353 for real (LTCG keeps `first` in EBP); on it R149's decay form 358 (1335 B), plus the growth
// step through `initial`/`elapsed` pointers with the pin written out 382 (1354 B) [56, growth loop exact]; other guards:
// pass 1 321, pass 2 354, pass 3 357/357, output loop 349, decay 373 (1341 B). R149's always-true loop-3 test re-measured
// 227, with that decay form 212 (1324 B), with that growth form 198 (1338 B, aligned 17) [compile-only: it alone takes
// the pass-3 head out of the loop alignment, as retail, and flips the registers]. Natural loop-3 spellings on the decay/
// growth forms: `i < last + 1` 440, `i <= last; i++` 388, `continue` after `--i` 388, decay/growth alone 388; one more
// index use in loop 3 (flag test 372, created test 372, zero store 388); compile-only, none of a while form, an outer
// entry pointer, `--last` apart, postfix steps, a named removable test, a negated `continue` or nested ifs flips them.
// MiniMax S (11-17): no new form, no lower score (317): the body is retail's 1324 bytes and the whole
// difference is the register allocation (retail keeps the state pointer in ebp and the swap counter in edx with
// their homes at [esp + 0x10] and [esp + 0x14], this build takes ebp for the counter and edx for the state
// with the homes swapped); the natural spellings of the two homes (the declaration orders, the names and the
// index and the found flag first) are listed above and measure the same, so the body stands.
// Admission count and state roles still exchange EDX and EBP, and the ordinal homes differ. Scoped entry-array
// base in paired scan 375, fallback 317, creation removal 375, duplicate removal 317, both admission scans 367,
// creation through both sorts 317, admission through creation 370. One-past creation tail pointer 350.
// Retained-boundary cursor through the paired, fallback, creation and first-sort phases scored
// 422/440/479/479 with the paired flag and 425/451/485/485 with cursor equality.
// N404: Creation-pass admission in either loop-test order 351/417, conditional empty start 244,
// exclusive creation-pass boundary with revisit/advance-on-retention/unsigned ordinal/left
// bound/short ordinal 381/379/374/379/362. NOT kept: the conditional start recovers early
// EDX/EBP roles at 1324 bytes, but adds a manufactured out-of-range start and comparison on
// the path retail skips directly. Counter homes and the decay base still differ.
// MiniMax C (11-17): no new form, no lower score (317): our body is exactly retail's 1324 bytes and the whole difference is the register allocation - retail keeps the state pointer in ebp and the swap counter in edx with their homes at [esp + 0x10] and [esp + 0x14], this build takes ebp for the counter and edx for the state with the homes swapped; the declaration orders of the two homes measure the same, so the body stands.
// Creation-entry cursor with indexed end, ordered/equality bound and else/continue
// advancement 337/337/337/337. Paired entry/end cursors 382/382/382/382.
// NOT kept: the state and prefix registers are still reversed.
// Indexed swap through list pointer, list reference, entry pointer, or fixed array
// reference. Direct subscripts 368 each, record references 380 each. NOT kept:
// direct subscripts preserve the 0x30 frame but leave prefix and state reversed
// at 1336 bytes. Record references shrink the frame to 0x2c at 1326 bytes.
// Separate mutable paired count, creation tail and duplicate tail at their phase boundaries,
// individually and combined 399/390/397/437/388/397/388. All with a separate fallback count
// 349. NOT kept: all regress. The state, prefix and creation ordinal still use different
// registers on the original frame.
// Round 144 (m3 lane 12, fast with 0x4b160, control 317): pass 1's tests nested (`if (last > i)` around the flag
// test) 317 (same code); pass 4's created-flag test ahead of the key tests 373 (1342 B); the growth loop as a plain
// `for` over state->entries[i] with the entry pointer 814 (1655 B), with its own index 794 (1639 B: the plain loop
// is unrolled, so retail's guard plus countdown do-while stands); pass 2 setting `first = 1` (it is always 0 there)
// in place of `++first` 385. `first` keeps EBP in each.
// Swap record shared across all four passes, isolated per gate, shared across admission, shared across
// removal, shared through creation, or shared after paired admission 317 each. Existing swap with a value capture,
// direct or named, 367 each. The prefix and state registers and the ordinal homes remain exchanged.
// M3 lane 5 r205 (fast with 0x4b160, MATCH each time; control 317): passes 3 and 4 sharing their own index (passes 1-2 and
// the decay loop keep `i`) 317; pass 1 with its own in-loop index 375 (1342 B); `register` on `first` 317; the decay loop
// with its own in-loop index 317; pass 2 with its own in-loop index 317. `first` keeps EBP in each.
// M3 lane 1 r131 (fast with 0x4b160, MATCH each time; control 317): pass 2's swap target as a pointer to
// entries[first] taken before its loop 357 (frame 0x2c); pass 3 as a helper taking (state, start index, pointer
// to `last`) with the parameter as the loop index: static 468 (1239 B, called out of line), forceinline 393
// (1351 B); passes 1-2 as a helper returning the new `first`: static 589 (1146 B, out of line), forceinline 358
// (1324 B, but `first` is then not homed before pass 1: frame 0x2c). Body unchanged.
// Paired candidate reference after the tail check 317, creation candidate reference 317. Decay initial
// reference, entry reference and both 317/317/317. Active growth entry reference 317, growth initial
// reference 369, both 388. The growth initial bindings expand the body to 1336 and 1351 bytes. The
// state and prefix registers and ordinal homes remain exchanged.
// M3 lane 1 r134 (fast with 0x4b160, MATCH each time; control 317): one to seven dead initialised locals 317 each (same
// code: not a local-symbol-count state); a second variable holding `first` after pass 2 for passes 3 and 4 and both sort
// calls 370 (1338 B); pass 3 alone with its own in-loop index 367 (1335 B); the swap as an index-taking helper
// (state, a, b) at pass 3 only 368 (1336 B), at all four swaps 368, at passes 1-2 375 (1342 B), at passes 3-4 375, at
// passes 1-3 367 (1335 B); pass 3 as `for (i = first; ; ++i) { if (last < i) break; ...` 317 (same code, also with
// `!(last >= i)`); diagnostic, pass 3's test with pass 1's `last > i &&` in front 338 (1305 B: retail's registers and
// homes through passes 1-4 with one extra `jle` at the loop head, which reuses the loop test's flags; it skips the last
// entry, so it is not retail's code). Compile-only readings (it reproduces passes 1-4 but for the sort calls' register
// setup): pass 1's head has no alignment pad in any spelling tried (start from `first`, `i <= last`, without its
// `last > i`, without its swap), so a missing pad alone is no evidence there; pass 3's head keeps its pad whenever
// `first` loses EBP for another reason (pass 1 without `++first`, diagnostic), and loses it only with a (last, i) test
// at the top of its body, kept (`last > i`) or folded away (`last >= i`). `while (true)` with `if (i > last) break;`
// and the increment at the bottom grows (1335 B, compile-only), the break after the entry pointer more (1351 B).
// Creation remaining count with front ordinal/cursor 850/821, original count 831/787,
// inclusive last candidate 810/787. Remaining and original counts with both front and tail
// cursors 814/789. NOT kept: the greater extent moves away from retail and the early
// prefix/state allocation is unresolved.
// Full-byte masks for the paired admission 422, fallback 367, creation 352, and duplicate/decay
// lock 387. Both admission passes 418, removal and decay 410, all masks 415, shifted paired
// bytes 419. NOT kept: the word-based predicates grow the body, and the retained-prefix
// and state registers still differ.
// Copy-out overlap-safe payload, initial value and both 359/381/366; named live count
// with payload/both 359/366; return the last tested count with payload/initial/both
// 357/382/354. NOT kept: all regress, and the list and prefix keep their exchanged registers.
// MiniMax H (11-20): no new form, no lower score (317); the state and prefix registers and the ordinal homes are still exchanged, and the body stays longer than retail's.
// Swap destination first in paired/fallback/creation/duplicate passes 319/323/317/316. Both admissions, both
// removals, paired plus creation, or all four 325/316/319/324. Creation ordinal seeded before the guard, shared
// or local 357/393. NOT kept: the one-point gain remains below the threshold and the early register roles still
// differ.
// M3 lane 1 r146 (H2M_FAST with 0x4b160, MATCH; control 317): pass 3 spellings that earlier rounds had screened only
// compile-only, now under the link: the removable test held in a named bool 317; the test negated with `continue` 317; the
// entry pointer declared ahead of the loop 317; postfix steps with `last--` apart from the swap 317; the flag byte read
// through a `byte *flags` as pass 1 does 317 (all five `first` in EBP). R149's always-true `last >= i &&` re-measured 227
// (1305 B): pass 3 is then retail's instruction for instruction, the first differences are the sort call's setup order
// and the decay loop's base, so the open cause is still only how pass 3's body is weighted.
// M3 lane 1 r155 (H2M_FAST with 0x4b160, MATCH; control 317): pass 3's test as `created == current && removable` 317, pass
// 4's elapsed store ahead of the initial store 317, pass 1's flags pointer const 317 (same code each); pass 1's test with
// the bound check last 341; the limit clamp as `state->count < limit` 319.
// M3 lane 5 r237 (H2M_FAST with 0x4b160, MATCH each time; control 317): pass 2's swap target as `&state->entries[first++]`
// with its `++first` dropped 317, pass 1's swap target as `state->entries` 317, both 317 (same code each: `first` keeps
// EBP, the base EDX); pass 2's test through a `byte *flags` as pass 1's 317; passes 1-2 in their own block with `found`
// declared there 317; `found` declared beside `first` 355 (first +0xc). Body unchanged.
// M3 lane 13 r153 (h2m fast with 0x4b160, MATCH each run; control 317, aligned 133). Allocator ranking of the open cause:
// after pass 4's cursor (55) takes EBP for its own blocks, `first` is re-ranked at 55 over passes 1 to 3 and takes EBP,
// and pass 3's index (48: one reference at its start, one in the swap block, two in the step block, less 14 for the
// four blocks it only lives through) is left without a register that survives the swap's copies. The index goes ahead
// of `first` on equal rank (it is the later-created value), so the gap is 7. R149's repeated loop test closes it with a
// block at the loop top that reads the index and `last`: the index 56, `first` 50 (re-measured 227 at 1305 B, aligned 68;
// passes 1 to 4 are then retail's but for the order of the two reloads after pass 4's swap). Tried in pass 3: every
// access through state->entries[i] with no entry pointer 318 (the ranks do not move: the indexed accesses are reduced to
// the cursor before the ranking); the entry pointer kept with the swap from `&state->entries[i]` 368 (1335 B); a `byte
// *flags` for the test with indexed accesses 318; the swap written out in the body with a local record 375 (1342 B);
// `swap(entry, &state->entries[last]); --last; --i;` 317; the swap helper's record declared and then assigned 317 (same
// code; the last three leave the ranks at 48 / 55). An empty `do {} while (false);` (what a compiled-out statement
// leaves) at the top of pass 3's body, after the entry pointer, at the end of the body or after the loop 317 each (same
// code: an empty block reads nothing, so it moves neither rank).
// Creation self-swap by ordinal 368, retry only after replacement 369, final-candidate exit 369,
// entry-identity guard 372. Duplicate self-swap guard 327, both removal guards 437, creation
// and fallback guards 366, creation exit plus other guards 381. NOT kept: all regress.
// The state and prefix still exchange registers and the creation ordinal still spills.
// Eager paired bytes, eager bound with first byte, independent bound with paired bytes, all three
// admission tests together, eager creation pair, eager duplicate key tests, eager duplicate
// eligibility, and both removal predicates: 373/408/421/429/369/409/399/417.
// NOT kept: all regress. The paired byte forms add a frame word or more code.
// Shared suppression getter in the original or nested gate 375/375, with
// sampled count in either gate 375/375, selected limit 375, early request 435,
// prefix initialized after the clamp 342, or named gate result 375.
// NOT kept: the inlined gate leaves the prefix and state exchanged. The closest
// extent is 1326 bytes with frame 0x2c.
// Capacity-selected start: exclusive tail 244, default tail assignment 319, list count 245.
// Stable capacity test before/after the bound 351/406, selected start with while 359,
// consecutive replacement runs with a separate gate or selected start 340/358.
// NOT kept: the best forms construct a skipped ordinal where retail branches past the scan.
// The prefix and ordinal homes, and the creation ordinal register, still differ.
// Creation ordinal initialized before the direct capacity skip, reused or separate 357/393;
// nested creation predicates, named capacity decision or while traversal on those two forms
// 357/393 each. NOT kept: the earlier ordinal changes the prologue and raises the score.
// Separate signed-word ordinal on the inclusive direct-skip form 340. NOT kept: the
// narrowed ordinal still differs from retail.
// @retail 0x4ac30
long __stdcall function_4ac30(s_cached_input_list *state, s_cached_input *output, long limit, real step)
{
	if (state->count > 0 && (!g_510c50 || !((byte *)g_510c50)[5]))
	{
		long first = 0;
		long last = state->count - 1;
		long requested = limit;
		if (limit > state->count) limit = state->count;
		bool found = false;
		long i;
		for (i = 0; last >= i; ++i)
		{
			byte *flags = (byte *)&state->entries[i].input.data[0];
			if (last > i && flags[0] && flags[1])
			{
				swap_cached_input(&state->entries[i], &state->entries[0]);
				++first;
				found = true;
			}
		}
		if (!found)
		{
			for (i = first; i <= last; ++i)
			{
				if (*(byte *)&state->entries[i].input.data[0])
				{
					swap_cached_input(&state->entries[i], &state->entries[first]);
					++first;
					break;
				}
			}
		}
		if (state->count > requested)
		{
			for (i = first; last >= i; ++i)
			{
				s_cached_input_entry *entry = &state->entries[i];
				if (((byte *)&entry->input.data[0])[2] && entry->created == g_4ba034)
				{
					entry->initial = 0.0f;
					swap_cached_input(entry, &state->entries[last--]);
					--i;
				}
			}
		}
		function_4b160(first, (s_4b160_entry *)state->entries, 1, last);
		for (i = first; i <= last; ++i)
		{
			s_cached_input_entry *entry = &state->entries[i];
			if (i > 0 && entry->input.data[2] != -1 && entry->input.data[2] == state->entries[i - 1].input.data[2] && !((byte *)&entry->input.data[0])[3])
			{
				entry->initial = 0.0f;
				entry->elapsed = 0.0f - step;
				swap_cached_input(entry, &state->entries[last--]);
			}
		}
		function_4b160(first, (s_4b160_entry *)state->entries, 2, last);
		if (state->count > 0 && requested < state->count)
		{
			for (i = state->count - 1; i >= limit; --i)
			{
				real *initial = &state->entries[i].initial;
				s_cached_input_entry *entry = &state->entries[i];
				if (!((byte *)&entry->input.data[0])[3] && *initial > 0.0f)
				{
					entry->elapsed -= step;
					*initial = entry->elapsed + *initial;
					clamp_cached_input(entry);
				}
				else entry->elapsed = 0.0f;
			}
		}
		if (limit > 0)
		{
			s_cached_input_entry *entry = state->entries;
			long remaining = limit;
			do
			{
				if (*(real *)&entry->input.data[3] > entry->initial)
				{
					entry->elapsed += step;
					entry->initial = entry->initial + entry->elapsed;
					clamp_cached_input(entry);
				}
				else entry->elapsed = 0.0f;
				++entry;
			} while (--remaining);
		}
	}
	for (long i = 0; i < state->count; ++i)
	{
		output[i] = state->entries[i].input;
		*(real *)&output[i].data[3] = state->entries[i].initial;
	}
	return state->count;
}


PRIVATE __forceinline real motion_random_step(double value, double step)
{
    // a float, so the 1.0f below stays a float constant as in retail (`fsub dword [1.0f]`); no rounding store is
    // emitted, so the x87 keeps the fraction unrounded
    real random = (real)random_unit_fraction(g_4e7408);
    real next = (real)(step * (random + random - 1.0f) + value);
    return next < -1.0f ? -1.0f : next > 1.0f ? 1.0f : next;
}

PRIVATE __forceinline point2f motion_project_point(point3f const *point)
{
    real x = point->x, y = point->y, z = point->z;
    if (g_48568c[0] != 1.0f)
    {
        x *= g_48568c[0];
        y *= g_48568c[0];
        z *= g_48568c[0];
    }
    point2f result;
    result.x = g_48568c[7] * z + g_48568c[4] * y + g_48568c[1] * x + g_48568c[10];
    result.y = g_48568c[8] * z + g_48568c[5] * y + g_48568c[2] * x + g_48568c[11];
    return result;
}

// score 486 (was 486): frame 0x60 versus 0x38, stored velocity reloads and projection homes.
// MiniMax G (11-20): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail keeps one 12-byte home for the previous camera position and each element's old position, reads the delta from the stored view position and copies the scaled projection inputs into fresh registers in both arms, while this build's locals and register plan differ from the start.
// MiniMax I (11-20): no new form, no lower score (486): the first difference is the frame, 0x60 here against retail's 0x38; retail keeps one 12-byte home for the previous camera position and one per element.
// MiniMax A (11-20): no new form, no lower score (486): the frame is 0x60 against retail's 0x38
// MiniMax S (11-17): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail
// keeps one 12-byte home for the previous camera position and each element's old position, reads the delta from
// the stored view position and copies the scaled projection inputs into fresh registers in both arms, while this
// build's locals and register plan differ from the start.
// MiniMax I (11-15): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail keeps one 12-byte home for the previous camera position and each element's old position and reads the delta from the stored view position, while this build's locals and register plan differ from the start.
// MiniMax R (11-16): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail keeps one 12-byte home for the previous camera position and each element's old position, reads the delta from the stored view position and copies the scaled projection inputs into fresh registers in both arms, so this build's locals and register plan differ from the start; the permuter (18 tries in 307 s, annealing) reached only 484 by splitting the flags declaration, narrowing the cycle flag to a byte and swapping two stores, so it was reverted.
// Earlier 486 (was 486): frame 0x60 versus 0x38; retail keeps one 12-byte home for the previous camera position and
// each element's old position, reads the delta from the stored view position, and copies the scaled projection
// inputs into fresh registers in both arms; those forms lower the aligned count but shorten the body (below).
// MiniMax C (11-15): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail keeps one 12-byte home for the previous camera position and each element's old position, reads the delta from the stored view position and copies the scaled projection inputs into fresh registers in both arms, and this build's locals and register plan differ from the start.
// MiniMax B (11-13): no new form, no lower score (486): the frame is 0x60 against retail's 0x38 and retail multiplies the mode's short by 0xb8 where this build takes a hoisted constant, so the locals and the register plan differ from the start.
// MiniMax H (11-12): no new form, no lower score (486); the frame is still 0x60 against retail's 0x38 - retail keeps
// one 12-byte home for the previous camera position and each element's old position, reads the delta from the stored
// view position and copies the scaled projection inputs into fresh registers in both arms, and those forms shorten
// the body.
// MiniMax D (11-11): no new form, no lower score (486).
// MiniMax B (11-12): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail
// keeps one 12-byte home for the previous camera position and one for each element's old position, reads the
// delta from the stored view position and copies the scaled projection inputs into fresh registers in both arms.
// MiniMax C (11-14): no new form, no lower score (486): the frame is still 0x60 against retail's 0x38 - retail keeps
// the entry pointer in edx and the view pointer in ebx, reuses the previous camera position's three homes for each
// element's old position and walks five pointers with a countdown, while this build keeps the entry pointer in ebx,
// the view pointer in edi and addresses the arrays through the index; writing the first store as retail does (the
// entry base plus the index times 0xb8) rebuilds the body at 498, a 12-byte union for the velocity components shrinks
// the frame to 0x5c but raises the score to 513, and the permuter (18 tries, 312 s, annealed) reached 484 only with a
// volatile pointer, so all three were restored.
// MiniMax H (11-06): 486 (was 486): no new form, no lower score.
// MiniMax E (11-09): no new form, no lower score (486).
// MiniMax H (11-06): score 486 (was 486): no new form, no lower score.
// The frame is the main difference: retail's is 0x38 against our 0x60. Retail keeps the entry pointer in
// edx and the object pointer in ebx and reads the three velocity components back from memory before both
// square roots, while we push four registers, keep the velocity in registers and spill the results through
// the stack. The rest of the loop and the two square roots agree.
// Earlier 486 (was 517): frame 0x60 versus 0x38; retail reads the added velocity back from memory for both square
// roots (ours spills its registers), and the projection results are copied through the stack.
// Earlier at 517: frame 0x60 versus 0x38; stored velocity reads and loop homes differ.
// MiniMax R (11-01): no new form, no lower score.
// Earlier counts 529 and 561; the logic matches retail block by block (walked statement by statement against
// Earlier at 517 (529, 561 before): frame 0x60 versus 0x38; stored velocity reads and loop homes remain.
// Earlier: the logic matches retail block by block (walked statement by statement against
// 0x37260-0x37a50: offsets, argument order, condition directions, the z,x,y random order and the clamps all agree); what
// is left is register allocation and scheduling. Retail's loop keeps one pointer per (base, stride) group starting at
// state+0x18, view+0x104, view+0x120 (offset[1]), view+0x158 and state+0x2c. Reading the strength before the cycle test
// and storing the fade before the phase now give the first two as retail; the offset group still starts at view+0x11c
// (offset[0]) with every order of its loads and stores (6 spellings, 520-525), and the frame is 0x60 vs 0x38. Retail also
// reads the velocity back from memory for the speed limit (we reuse the registers and spill them), keeps `step` on the
// x87 stack and multiplies with `fmul st(2)` where we emit `fld st(2); fmulp st(1)` (the product's operand order makes no
// difference), computes dz, dy, dx before the three products, and reads the flags byte at state+0x94 for the cycle bit
// (a byte read scores 538 but is a type pun, not kept). Tried: a speed-limit helper (560, same code), factor order in the
// random step (no change), the lane order of offset and of the view fields (no change), dz/dy/dx from the stored
// position (587), phase read before the cycle test (+7 over the per-branch read), structs for the state and view
// layouts (517 with the two reorders, 536 without; the byte offsets are kept).
// Round 344: output projection, shared moved-point add, their pair, shared phase publication,
// its three combinations and complete projection input staging: 556/589/645/526/545/593/644/539.
// Original projection helper and complete body restored; frame 0x60 versus 0x38, stored velocity
// acquisition and loop pointer ownership remain.
// F321: shared phase/reciprocal/previous-position workspaces in subsets
// 1/2/4/3/5/6/7 give 518/517/517/518/572/571/572; all with complete point
// and projection role names 572. Original executable body restored; frame and velocity reloads remain.
// Round 527: Native repeated flag reads/double random rate/real random-helper input subsets
// 1/2/4/3/5/6/7 give 511/520/537/514/530/535/528; all with shared maximum scaling 526.
// Six-point lead rejected; original flags, helper formals, rate and full body restored.
// Frame 0x60 versus 0x38, stored velocity reads and loop ownership remain.
// Round 530: native acceleration vector alone, with shared add, divisor scale, add/divisor,
// add/normalization, add/maximum, add/divisor/normalization and all scales: 517 each.
// Caller stays 251; original scalars, helpers, observations and complete body restored.
// P267: native long cycle/size/reset carriers, subsets 1/2/4/3/5/6/7: 515/511/519/516/518/513/519; all byte carriers 517.
// Best lead is below the keep threshold; original flag types and complete body restored.
// Round 369: actual state-scalar/view-and-offset/three-component cursor subsets
// 1/2/4/3/5/6/7 give 533/523/525/536/536/530/526; all with repeated native
// flags 557 (frame 0x5c). Original indexed graph and whole body restored; frame and velocity reads remain.
// F334: Existing random bounded-result initialized/early-return/local-clamp/completed
// graphs: 502/517/517/517; with coherent per-use rate 500/524/500/524.
// Local next-value clamp with the rate read at all three uses 500 (2052/2035 bytes, frame 0x64)
// NOT kept: retail loads the rate once (fld [eax+0xa8] at 0x375a1, held across the three steps)
// and the clamp alone ties 517. Body restored at 517; frame and velocity/loop homes remain.
// AC488: complete double component/square ownership in the speed test and normalization:
// component/original, original/component, both components, square/original, original/square,
// both squares, component/square, square/component 491/494/515/506/506/557/523/515.
// NOT kept: independent components 491/494 and independent squares 506 enlarge the frame
// to 0x64 versus retail 0x38; combined components 515 gain only two. Original body restored.
// Frame 0x60/0x38, stored velocity acquisition and loop homes remain; original trees/helper preserved.
// Y538: Last-consumer acceleration/speed/projection scope subsets 1/2/4/3/5/6/7
// give 533/517/517/533/533/517/533; all with the existing position receiver before
// old-position capture 526. All restored; frame 0x60 and stored-velocity/loop homes remain.
// Lane 12 r86: the norm block as the existing normalize3d 497, as normalize_inline 486 (kept;
// the velocity scalings as vector3f_scale alone 517). Both projections through the out-of-line
// transform4x3f_apply_point 820 (not inlined); the projection result brace-initialized 497; the
// projection helper with an output pointer 514 (frame 0x44); speed test as sqrt(dot3f) 519, as
// normalize_inline's float sum 527, its double sum grouped j + (i + k) 486 (same code); acceleration
// as a vector3f with vector3f_scale and vector3f_add 513, with vector3f_scale only 513.
// P312: Existing projection input value/const-reference/reference/native snapshot, returned result
// 589/486/486/589; reference output on those inputs 498/514/514/487. NOT kept: no match or gain;
// original projection helper/body restored. Frame 0x60/0x38 and velocity/loop homes remain.
// DL257: Actual state scalar fields, scalar arrays, velocity vectors, complete state, view
// geometry, view scalar/random fields, complete view and complete state/view fields: 486 each on
// the retained inline-normalization graph. Original body and field accesses restored; frame 0x60
// versus 0x38 and stored-velocity/projection homes remain.
// B425: Existing random-helper standard lower/upper clamp boundaries, direct and named,
// composed and sequential: 587/590/587/587/504/507/637/626. All regress; native frame 0x38
// and velocity/projection homes remain. Original helper, include list and body restored.
// Y568: Actual position-transform/moved-point/acceleration-add component loops, seven subsets,
// then all with the divisor loop: 548/542/547/549/565/580/593/579. All regress and
// shorten away from retail; original scalars, vector statements, helpers and body restored.
// Frame 0x60 versus 0x38 and stored-velocity/projection homes remain.
// M365: Existing projection helper with scalar pointer/reference/returned-x/returned-y outputs,
// original rows then parallel row accumulation: 600/600/596/595/600/600/595/595.
// NOT kept: all regress and shorten to 1920-1925/2035 bytes. Original helper and complete
// body restored; frame, stored-velocity acquisition and projection/loop homes remain.
// DH343: Stored-position readback, native old-point constructor, native new-point publication,
// their pairs/all and all with shared complete displacement: 513/519/534/548/535/509/519/507.
// Original body restored; frame 0x60 versus 0x38 and independent velocity reloads remain.
// DH347: Complete previous/old point memcpy boundaries, moved-point initialization and
// projection declarations: 486/486/516/486/516/516/516/516. Original body restored;
// frame 0x60 versus 0x38 and stored-velocity/projection homes remain.
// J252: Projection receiver returns, helper expansion and component-array access: 514/514/486/486/486/486/486/486/486/535/501/501.
// NOT kept: no qualifying native-frame gain or byte equality; original full graph restored.
// P328: Complete double speed-squared expression/compound/separate/partial reductions,
// original named root then root at the comparison: 486/486/486/486/497/486/486/486. Original body
// restored; frame 0x60/0x38 and stored-velocity/projection homes remain.
// M385: Actual loop old-point receiver advanced in place, native/library/setter/initializer
// captures with direct/existing scaled-add updates: 535/556/535/556/495/558/570/570.
// NOT kept: all regress and shorten away from retail; original point receivers and full body restored.
// M387: Existing projection producer as two genuine matrix-row iterations, long/int/dword/uint,
// full expressions then complete accumulators: 498/498/498/498/510/510/510/510. NOT kept:
// frames 0x68/0x70 grow beyond native 0x38; original helper/body restored, velocity/projection homes remain.
// AC549: Complete actual input fields, scalar/vector/array/named-record triples,
// each with direct typed accesses or one typed receiver: eight 486s, frame 0x60
// versus 0x38, 2036/2035 bytes. NOT kept: no velocity/frame/projection repair;
// original raw input observations, types and entire body restored.
// M392: Actual three-pass main ordinal int/uint/dword/short/word/schar/byte/char: 486/492/492/485/485/485/485/485.
// NOT kept: no eligible native frame and stored-velocity recovery; original long ordinal retained. Original complete body restored.
// J256: Native maximum/divisor/projection-scale observation subsets and complete projection initialization:
// 508/490/518/495/496/515/504/504. NOT kept: every form regresses from 486; maximum observations recover only four frame bytes, while scale capture enlarges it
// Original executable source restored; native frame and allocation gaps remain.
// DL274: Entry capture/position-transform/first-projection scopes, seven subsets then both output declarations: 486/486/486/486/486/486/486/486.
// NOT kept: native velocity reloads and frame/projection homes remain; no eligible byte match or native repair.
// Original complete executable source restored.
// F395: Plain x/y random-update loop after the original z update, long/int/dword/short,
// scalar-array then native-vector receivers: 490/490/490/543/490/490/490/543.
// NOT kept: all regress and enlarge the frame to 0x68/0x38; original complete body restored.
// Native stored-velocity acquisition and projection/loop homes remain.
// N390: Declared native cycle-byte/shared-dword flags, cycle bool/byte/schar/short/word/int/long/dword,
// 490/490/490/497/497/493/493/493. NOT kept: all regress; original state observations,
// type set and complete body restored; native frame, velocity reloads and projection/loop homes remain.
// Round 64 (lane 15, fast): `previous` reused for each element's old position 535 (1985 bytes, frame 0x58, aligned
// 485 -> 428); the cycle bit read from the field (other bits from `flags`) 490, with the shared `previous` 533; dz/dy/dx
// from the stored view position 513 (2004 bytes, aligned 421), with the shared `previous` 513; projection scaling
// `x = g_48568c[0] * x` 486, also in the position transform 486, with the stored-position delta 513.
// Lane 5 r164 (fast; aligned/register-masked LCS counts of 353/187 for the 486 body in brackets): `previous` reused for
// each element's previous position with the delta read back from the stored view position 513 (2004 bytes [332/201]); with
// the projection helper writing a point2f through an output pointer 531 (1986 bytes, frame 0x44, both projections now stay
// in registers as retail [322/196]); on that, the phase compare reading the field before the load 526 [327/205], a `for`
// loop 532 [329/203], no velocity pointer (every access and helper argument spelled from state) 534 (1982 bytes [287/177]),
// that with the speed test through existing magnitude3d 550 (stays a call). Diagnostic only: the speed test read through a
// second pointer formed as state + i * 12 + 0x24 makes the first root load the stored velocity as retail (no spill), and
// with normalize/scale on it the frame is retail's 0x38 [292/164] but 1945 bytes: retail also re-reads the velocity after
// each normalize and scale store, as if each inline helper reached it through its own parameter. The 12 bytes left on 0x44
// are the three velocity spills; retail keeps view in EBX and state in EDX from the first instruction (ours the reverse).
// Lane 12 r102 (fast; the fast control agrees at 486): the speed limit (magnitude test, normalize_inline and
// vector3f_scale) in a forceinline helper 486 (identical code: the velocity reads from memory do not come from a helper
// boundary); `old` renamed to sort after `previous` 486 (identical); the size store as an if/else reading the field in
// each arm 495 (2044 bytes; that block then compiles as retail's, integer copy in the else arm); with retail's visible
// phase-block order too (phase stored before the fade, strength read at its use, copied in the else arm) 502 (2019
// bytes: the loop pointers then start at state+0x88 and view+0xec); that order alone 494 (2024 bytes).
// Lane 14 r81 (fast; agrees with the full 486; second number is the register-masked aligned count, 277 for 486): new:
// the existing out-of-line normalize 0x30bf0 for the norm block 531 (stays a call, 1989 bytes). Listed forms paired with
// J256's maximum read at each use (508 alone, re-measured): one shared old-position local for the camera's and each
// element's old position 560 (1960 bytes, 332); the output-pointer projection 525 (frame 0x40, 323); retail's
// phase-block order (phase stored before the fade, strength read in each arm) 501 (frame 0x5c, 272); that with the
// output-pointer projection 530 (frame 0x40, 317). Retail's velocity add leaves no value live (xmm1 reused for all three) and both roots and
// the normalize reload the components from memory; every form here still forwards the added values into the first
// root (spilled to the stack for the x87) and into the normalize.
// R151 (m3 lane 7, fast): retail computes the distance inside both phase arms (after the phase store), copies the
// strength field to the fade with integer moves in the else arm, and reads flag bit 0 as a byte and bits 1-2 from one
// dword read (the field read at each use). Distance at the end of both arms 485 (the tails cross-jump back into one);
// retail's visible arm order (phase store, distance, fade with the strength read at its use, field copy in the else arm)
// 488 (2032 bytes; loop pointers then start at state+0x88/view+0xec); cycle arm fade, phase store, distance with the else
// arm copying both fields and taking the distance from the times field 482 (2035 bytes; statement order, under the bar);
// the strength read before the test on retail's order 498; output-pointer projection (514 re-measured) with the 482 form
// 499, with retail's order 523; flags read from the field at each use 491 (re-measured on this base), with the 482 form
// 495, plus output pointer 506, plus divisor and maximum read at each use 587 (1926 bytes); 482 form + flags + divisor and
// maximum at each use 485 (frame 0x5c); 482 form + divisor and maximum at each use 505 (frame 0x5c); 482 form with the
// else arm keeping a phase local for the store and the distance 492. Diagnostic: `#pragma optimize("p", on)` around the
// function 754 (2285 bytes), so float consistency is not what keeps retail's velocity reloads.
// MiniMax G (11-14): no new form, no lower score (486): re-measured in the batch; the frame is still 0x60 against
// retail's 0x38, and retail's one 12-byte home for the previous camera position and one for each element's old
// position are still absent, so the delta and the scaled projection inputs keep our register colours.
// MiniMax G (11-14, later): no new form, no lower score (486): the permuter (18 tries, 311 s) found one variant a
// single instruction better in the full image (484 against 485), which is under the keeping threshold and is not
// written; the frame is still 0x60 against retail's 0x38.
// M3 lane 3 r243 (fast, base 486 [register-masked aligned 187]): the velocity add into a local vector stored back as
// one aggregate (`*velocity = updated;`) 505 (2012 B [214]): the speed test then reads the stored velocity from memory
// as retail, but the copy goes through integer registers and the stack; with the maximum field read at each use 518
// (2003 B, frame 0x5c [211]); with the output-pointer projection too 493 (2021 B, frame 0x44 [202]). Diagnostics (not
// keepable): the speed test, normalize and scale through a second pointer formed as `state + i * 12 + 0x24` with the
// maximum read at each use 503 (2009 B, frame 0x50 [167]), plus the output-pointer projection 563 (1958 B, frame 0x34
// [205]). Body unchanged.
// M3 lane 5 r186 (fast with 0x36b50, control 486): the y offset updated before x 486 (stores reordered only); the
// magnitude written into the test (no `magnitude` local) 497 (2023 B); the previous camera position, its store and the
// delta in a block of their own 486 (identical code: block scopes do not let `previous` and `old` share retail's one
// 12-byte home); `old` copied through the `position` pointer declared first 486 (different code); on that, `old`
// advanced in place for the first projection (M385's in-place receiver on this base) 544 (1977 B). Body unchanged.
// M3 lane 15 r92 (fast, control 486): one combined trial of the retail-shaped pieces (`previous` reused for each
// element's old position, the delta from the stored view position, phase store then distance then fade in each arm
// with the strength read at its use, the size store as an if/else reading the field in each arm, the projection
// writing through an output pointer) 531 (1985 B, frame 0x44, one home for both positions); on it: the
// delta as dot3f of the forward vector with vector3d_from_points3d 609 (1909 B); the velocity add through the
// indexed state array 560 (1957 B, frame 0x38 and that home at +0x3c as retail: the root loads the stored velocity); on that,
// the normalization written with the speed test's k, i, j sum 562 (the length is shared with the speed test, retail
// computes it twice); the speed test summed in float and the normalization's in double 529 (1989 B: the second root
// appears as retail's). The float speed test alone on this body 510 (2011 B). Left: the first projection's x copy,
// the second projection's if/else with z in a register, and the cycle flag's byte read.
// MiniMax C (11-16): no new form tried this round, no lower score (486): the frame is still 0x60 against retail's 0x38, so the returned
// MiniMax C (11-16): projected-point temporaries and the phase publication are still laid out differently; the earlier rounds' projection,
// MiniMax C (11-16): publication and offset-order forms are recorded above.
// M3 lane 11 r111 (fast): the delta through the existing vector3d_from_points3d from `previous` to the stored view
// position into a displacement vector, dotted k, j, i 512 (2005 B); a camera-position pointer to view+0x110 for the
// copy, the store and dz/dy/dx 524 (1994 B), with that displacement 512 (2005 B); a pointer to the phase field at
// view+0x140 for its five uses 486 (same code); the speed test as `maximum < magnitude` 485 (2035 B, retail's length).
// Paired projection aggregate, paired assignments, indexed projection and projection cursors:
// 486/486/510/537; with the shared velocity setter 513/513/517/537. NOT kept: the setter
// reduces the frame on direct calls but grows the difference and moves away from retail length.
// Returned projection copies and the independent velocity reloads remain.
// F408: Velocity setter 513; moved-point setter 561; both 561. Signed screen displacement with
// the existing point-difference helper 498; in place 500; in place with typed offset 500;
// signed projection result 530; signed result in place with typed offset 530.
// NOT kept: all regress or move length away from retail. The velocity and projection homes remain.
// Five stride groups with countdown 530 or phase end 542; independent arrays
// with countdown 505 or phase end 504; random-base cursor 526; screen-y cursor
// 530; separate position/random cursors 485; independent view scalars 507.
// NOT kept: the separate position/random form gains one at retail length
// and grows the frame to 0x64.
// DH393: Half-angle, tangent and radius-product locals 486/498/486.
// Half-angle/tangent, half-angle/product and tangent/product 505/490/489.
// All three with direct assignment or a short producer scope 494/494.
// NOT kept: ties or regressions. The 0x60 frame, velocity reads and projection homes remain.
// Separate projection input and scaled ordinates, scaling/identity arm first, whole point input,
// or whole scaled point output, with field reads/one scale sample: 530/518, 515/505,
// 576/552, 530/510. Frame 0x5c except whole input 0x7c; velocity reloads still differ.
// The two x87 lengths still need independent stored-velocity loads, and the projection outputs contribute
// to the 0x60 frame. sampled placement scale 497; shared scaled horizontal input 524; separate placement
// scale arms 485; velocity cursor 485; complete screen offset point 504; typed screen offset fields 486;
// maximum field reload through shared address 508; velocity end bound 495. NOT kept: The explicit placement
// inputs and independent velocity cursor both reach the retail extent at 485. The former enlarges the frame
// to 0x64 and the latter keeps 0x60. Their one-point gains fall below the threshold. The other forms tie or
// regress.
// Round 422: Scalar previous camera / element / both snapshots 491/491/528;
// camera readback 530, moved point initialized from previous coordinates 551, both 523,
// camera displacement with original/scalar element capture 500/489.
// The shared previous-position home, frame and independent velocity reloads remain.
// Round 190 (m3 lane 8, fast): the cycle bit read straight from the field after the flags local 485 (2035 B, still one
// dword load); that read ahead of the flags local 490 (2039 B: it gives retail's separate byte load of the cycle bit), as
// `(bool)(field & 1)` 490; the previous position, its deltas and the dot in their own block 486 (same 0x60 frame); that
// block through a pointer to the stored camera position with the deltas read back from it 524 (1994 B).
// Stored bitfield cycle/size/reset, cycle+size/cycle+reset/size+reset:
// 490/486/489, 490/491/489. All direct bitfields 491; one bitfield snapshot 486.
// The 0x60 frame, velocity inputs and projection homes remain. Original flag word restored.
// Cycle-local camera displacement 485; range after time reset 491; local phase wrap 492,
// conditional publication 506, per-arm publication 492; separate phase advance/accumulator/
// fraction 500; radius in size arm 495; optional size update 486. NOT kept: cycle-local
// displacement gains one at retail length and still reserves 0x60 bytes.
// M3 lane 5 r205 (fast, control 486): the old position advanced in place as the moved point (no `moved` local) 535 (1985 B,
// frame 0x5c); the two projections as const locals 486; the offset updates as `+=` 486; the current position projected
// before the moved point 546 (2097 B); `old` copied through the position pointer declared ahead of it 486.
// Size before velocity/limit/offset or after offset 488/499/486/487. Scaled add 518,
// with size before velocity/limit/offset 493/517/518. NOT kept: the scaled add
// shrinks the frame to 0x50 and the body to 2001 bytes. Both velocity roots and
// the 0x38 frame still differ.
// M3 lane 1 r131 (fast with 0x36b50, control 486): the velocity pointer as `&((vector3f *)(state + 0x24))[i]` 486 (same
// code); every velocity access as `velocities[i]` off a base pointer, no element pointer 509 (2009 B, frame 0x5c, the
// adds still forwarded into the root); the divisor field read at its test and in the reciprocal (retail's `divss
// xmm2, [edi+0xa4]`) 490 (2030 B, aligned 337 against 346); the adds written `vx + velocity->i` 486 (same code); one
// saved-position local used for the camera copy and each element's old position 535 (1985 B, frame 0x58), with the
// divisor read at each use 544 (1976 B).
// M3 lane 1 r134 (fast with 0x36b50, control 486; a compile-only build gives this function's code but for the random
// generator's pointer, which it reloads at each draw, so shapes were screened there with an aligned count first; A =
// aligned unmatched lines of the fast listing, ours/retail, 353 for the kept body). One combined body of the
// retail-shaped pieces (camera copy and delta through a pointer to the stored view position; phase stored ahead of the
// fade in both arms with the distance between them and the strength read at its use; one saved-position local for the
// camera and each element; the divisor and the maximum read at each use; the size store as an if/else): 537 (1983 B,
// frame 0x58, A 322/340). On it, the velocity sums written through the indexed state array (`*(real *)(state + 0x24 +
// i * 12) = vx + ...`, `+=` for j and k) with the pointer local kept for the reads: 532 (1988 B, frame 0x4c, A
// 281/290): the three sums are then stored and read back for the length as retail does. On that, the speed test as
// `sqrt(dot3f(velocity, velocity))` with normalize3d and vector3f_scale: 567 (1951 B, A 255/277): from the velocity
// sums to the size store every line is then retail's (the first length's x87 sequence, `1 / length` and the maximum
// each scaled as copy, `[i]`, store, then j and k loaded and scaled) except that the second length is shared with
// the first, where retail computes it again in the same form. Keeping them apart: the speed test through
// length_sq3f and the helpers given the element's address in place of the pointer local 532 (1988 B, A 285/294;
// both lengths then load all three components and keep them on the x87 stack to the end); a second named pointer for
// normalize3d and the scale 545 (1975 B, A 282/294; the first length is retail's, the second keeps its loads).
// Compile-only only: the kept double-cast length with normalize3d, normalize_inline with the dot3f test, the dot
// through a one-pointer private length helper, `(double)dot3f(...)` and dot3f given the element's address twice
// (each product then loads its component twice) are no closer. Readings for the next round: retail takes the cycle
// bit from a byte load of state+0x94 and the other two bits from one dword load (`mov al, [state + 0x94]`, then
// `mov esi, [state + 0x94]`), keeps the state pointer in EDX up to the loop and reloads it from its parameter home
// inside it, keeps the view in EBX with a home, and walks five pointers (state+0x18, view+0x120, view+0x104,
// state+0x2c, view+0x158); both projections copy the point into three locals and scale those (the first in
// registers, the second in stack homes), and the moved point is `step * i + old.x` with j and k scaled in place.
// None of the pieces lowers the positional count while the frame (0x38) and the entry registers differ.
// Single scale read for world, projections or all transforms 497/518/491.
// Selected original or scaled projection coordinates 518, with world scale read once 505.
// World point setter 521, with world scale read once 524, or with selected projection coordinates 508.
// NOT kept: the smallest frame is still 0x5c and shortens the body; all eight raise the score.
// Divisor sampled before x, y or z acceleration 485/485/486. Compare at
// those points with the same divisor for the inverse 494/494/495. Early
// comparison before x or z with a fresh divisor for the inverse 487/488.
// NOT kept: the one-point gains reach 2035 bytes and retain frame 0x60. Both
// velocity roots still spill forwarded values instead of reading the fields.
// Shared projection coefficients for forward/left/up 531/532/532, forward+left/forward+up/left+up
// 519/518/519, all axes 500, all axes and translation 486. NOT kept: the axis-only forms shorten
// the body. All forms stop at a 0x5c frame, and the full coefficient form ties at the opening extent.
// Single scale read for world, projections or all transforms 497/518/491.
// Selected original or scaled projection coordinates 518, with world scale read once 505.
// World point setter 521, with world scale read once 524, or with selected projection coordinates 508.
// NOT kept: the smallest frame is still 0x5c and shortens the body; all eight raise the score.
// Round 499: indexed velocity for speed/advance, normalization, cap, normalization/cap, speed/normalization,
// speed/cap, all three limit stages, and normalization/advance.
// Fast scores in order 520/496/490/495/486/503/521/558.
// NOT kept: Indexed loads for the speed test and normalization recover both velocity memory reads at a 0x5c frame
// and tie 486 at 2039 bytes. Indexing all three limit stages shortens the function and raises the score. Neither
// gives the native addressing or frame.
// The 0x38 frame still needs independent stored-velocity loads and register-only projection results.
// Y inversion at the moved, stored or both projections 530/486/530,
// with both inverted and a shared displacement 513. Screen-y result in the
// projection helper as field, named row or direct expression 517 each;
// the named row with shared displacement 530.
// NOT kept: the stored-only form ties; the others add frame storage and length.
// The independent velocity reads and register projection results remain.
// M3 lane 5 r223 (H2M_FAST, control 486, aligned 485 of 487 retail lines): the velocity sums written through the
// indexed state array with the element pointer kept for the reads 492 (2027 B, aligned 456); that with the maximum
// read at each use 505 (2016 B); the speed test as sqrt(dot3f) on main's body 510 (2011 B); the indexed sums with that
// test 487 (2036 B, aligned 460); that with normalize3d for the norm block 531 (1990 B). Body unchanged.
// The excess frame and the separate stored-velocity and projection homes remain.
// In-place random pointer 485. In-place random reference 485.
// Random value and output pointer 486. Random value and output reference 486.
// Double adjustment with a pointer 525. Double adjustment with a reference 525.
// Accumulated value with a pointer 485. Accumulated value with a reference 485.
// NOT kept: The four direct-update forms reach retail length and gain one point while growing the frame to
// 0x64. Passing the previous value separately keeps 0x60 and ties. Naming the double-precision adjustment
// shrinks the frame to 0x5c and raises the score to 525 at 1995 bytes. The reduced controls for 0x36b50
// and 0x39e50 stay at 251 and 334.
// M3 lane 2 r132 (h2m fast with 0x36b50; control 486 at 2036 B): the three flag bits as plain bool conversions (no
// `!= 0`) 486 (same code); scale_size declared ahead of cycle 486 (same code); the range computed ahead of the camera
// copy 487; the range start read once into a local used for the range and each distance 494 (2028 B); the view pointer
// declared const 486 (same code). Restored.
// M3 lane 5 r237 (H2M_FAST with 0x36b50, control 486; retail multiplies the rate as `fmul st(2)` where this build
// emits `fld st(2); fmulp st(1)`): in the random step, the sum written value-first 486 and `random * 2.0f - 1.0f` 486
// (same code each), the clamp as two early returns 513 (2004 B), the offset named before the product 513 (2004 B), the
// fraction kept as a double 486 (same code). Body unchanged.
// M3 lane 6 r241 (h2m fast with 0x36b50; control 486, aligned 353): retail's else arm copies the strength as a dword
// (read at its use) and both arms store the phase before the fade; on this body the strength read at its use in both
// arms with the phase stored first 494 (2024 B), that with the distance computed in each arm 491 (2041 B), that with the
// projection helper writing through an output pointer 541 (1979 B, frame 0x44); only the phase store moved first 488
// (2038 B); only the strength local dropped 504 (2017 B). Frame reading: retail's 0x38 holds the two flag bytes, ten
// dwords and one 12-byte home shared by the previous camera position and each element's old position; ours adds four
// 8-byte projection results (the by-value returns stay in memory) and gives `old` its own slot (it interferes with the
// first projection result, which shares the previous position's slot). Body unchanged.
// The 0x60 frame, independent stored-velocity reads and projection output homes remain.
// One reused projected point with returned results, direct/delta updates and plain/signed first Y 510/510/539/532.
// The same four forms with the existing projection helper writing its output 549/549/549/549.
// NOT kept: returned forms grow. Output forms shrink the frame but finish 64 bytes short and raise the score.
// M3 lane GB r4 (fast with 0x36b50, 0x24590 and 0x259d0; control 486, al 422, mask 267; 20 trials): calling
// function_259d0(&g_4e7408->seed, NULL, 0, -1.0f, 1.0f) in motion_random_step with (double, double) 486 (same code),
// with (real, real) 502 (2019 B, al 400, step-first same 502: reproduces all 92 instructions of retail's random-step
// block including all three `fmul st(2)` and the third step's double `fstp st(0)`), inline without helper 571 (fr 0x5c);
// LTCG-inlined magnitude3d(velocity) (0x24590 linked) 510 (2011 B, mask 264), with normalize3d 522 (1999 B, al 406);
// unified 3x transform4x3f point helper (`point3f *out`) for world pos and both projections 537 (fr 0x44, yxz same 537),
// proj-only 514 (fr 0x44, al 357), proj-only with real-param 259d0 and `x *= scale` 517 (fr 0x44, al 365); unified
// transform4x3f + magnitude3d + 259d0 563 (fr 0x44); full retail-shaped body (byte cycle read, shared saved position,
// stored camera delta, phase/distance/fade arm order, field-read divisor/maximum, size if/else, unified transform4x3f)
// with magnitude3d + normalize_inline 585 (fr 0x3c), + normalize3d 604 (fr 0x3c), + vector3f_add 610 (fr 0x3c), + indexed
// velocity writes 612 (fr 0x30, 1907 B); on that indexed-write retail base with real-param 259d0 + `x *= scale` + double
// k,i,j speed test + normalize_inline 594 (fr 0x30, 1928 B, al 390, mask 254, 0x36b50 247: velocity add, separate double
// reloads, normalize+max scales and size if/else match retail), float k,i,j speed test 585 (fr 0x3c), same-pointer
// velocity add 589 (fr 0x3c), point2f output helper 561 (fr 0x34, mask 263); real-param 259d0 + float k,i,j speed test on
// main 520 (al 390, 0x36b50 247). Body unchanged.
// One camera/element/advanced point with the source camera reads: components 535, setter
// 540, point value 485, scaled add 535. With stored camera reads 512/512/523/512.
// NOT kept: the 485 form gains one with a larger 0x64 frame. The forms that shrink the
// frame shorten and regress. Independent velocity reads and projection homes still differ.
// Scalar projection by point, direct row/accumulator/row pointer/sampled scale
// 530/530/543/491; by corresponding ordinate 500/500/517/507.
// NOT kept: scalar projection reduces the frame to 0x3c–0x48. All forms raise
// the score, and the independent velocity reads and native frame remain unresolved.
// Two-point projection traversal, raw/screen y: returned point 510/542,
// point pointer 502/499, point reference 502/499, scalar outputs 502/499.
// NOT kept: returned points enlarge the frame. Output arguments retain
// 0x60 and shorten the body. The independent velocity reads still differ.
// Immutable moved point, immutable projected points, immutable saved points, fixed position/velocity
// receivers, named projected points with compound offset updates, scoped speed limiter, and one
// particle position receiver 486 each. Signed screen difference helper 498; shared scaled-add
// for the moved point 556. NOT kept: the signed difference increases the score and length;
// the scaled-add shortens the body to 1964 bytes and raises the score. The frame, stored-velocity
// reloads and projection homes remain different.
// M3 lane 15 r147 (fast screen with 0x259d0, MATCH; control 486): the cycle flag alone read from the flags' low byte, as
// retail's `mov al, byte ptr [state + 0x94]`, 485 (2035 B, retail's length; one point, not kept); the camera delta alone
// read from the stored copy at view + 0x110 513 (2004 B), with the byte read 513; the reset flag as `(flags >> 1) & 1`
// without `!= 0` 486 (same code), with both others 513. Body unchanged.
// @retail 0x37260
bool __stdcall function_37260(byte *state, real step, byte const *parameters)
{
    byte *view = state + g_485600 * 0xb8;
    *(real *)(view + 0x14c) = g_4670fc;
    dword flags = *(dword *)(state + 0x94);
    bool cycle = (flags & 1) != 0;
    bool scale_size = (flags & 4) != 0;
    point3f previous = *(point3f *)(view + 0x110);
    *(point3f *)(view + 0x110) = g_485618.position;
    real dz = g_485618.position.z - previous.z;
    real dy = g_485618.position.y - previous.y;
    real dx = g_485618.position.x - previous.x;
    real delta = g_485618.forward.k * dz + g_485618.forward.j * dy + g_485618.forward.i * dx;
    real range = *(real *)(state + 0x7c) - *(real *)(state + 0x78);
    bool reset_times = ((flags >> 1) & 1) != 0;
    if (reset_times)
    {
        *(real *)(state + 0x18) = 0.25f;
        *(real *)(state + 0x1c) = 0.5f;
        *(real *)(state + 0x20) = 0.75f;
    }
    if (cycle)
    {
        real phase = delta / range + *(real *)(view + 0x140);
        phase -= (real)(long)phase;
        *(real *)(view + 0x140) = phase;
        if (phase < 0.0f) *(real *)(view + 0x140) = phase + 1.0f;
    }
    long i = 0;
    do
    {
        real phase;
        // the strength is read before the cycle test and the fade is stored before the phase: in this order the
        // compiler starts its loop pointers at state+0x18 and view+0x104, where retail's start
        real strength = *(real *)(state + 0x88 + i * 4);
        if (cycle)
        {
            phase = *(real *)(state + 0x18 + i * 4);
            if (*(real *)(view + 0x140) > phase) phase += 1.0f;
            phase -= *(real *)(view + 0x140);
            real centered = phase * 2.0f - 1.0f;
            *(real *)(view + 0xec + i * 4) = (1.0f - centered * centered) * strength;
            *(real *)(view + 0x104 + i * 4) = phase;
        }
        else
        {
            phase = *(real *)(state + 0x18 + i * 4);
            *(real *)(view + 0xec + i * 4) = strength;
            *(real *)(view + 0x104 + i * 4) = phase;
        }
        real distance = phase * range + *(real *)(state + 0x78);
        real radius = (real)(tan(g_485640 * 0.5f) * distance);
        point3f old = *(point3f *)(view + 0xc8 + i * 12);
        real x = 0.0f, y = 0.0f, z = 0.0f - distance;
        if (g_48568c[13] != 1.0f)
        {
            x *= g_48568c[13];
            y *= g_48568c[13];
            z *= g_48568c[13];
        }
        point3f *position = (point3f *)(view + 0xc8 + i * 12);
        position->x = g_48568c[20] * z + g_48568c[17] * y + g_48568c[14] * x + g_48568c[23];
        position->y = g_48568c[21] * z + g_48568c[18] * y + g_48568c[15] * x + g_48568c[24];
        position->z = g_48568c[22] * z + g_48568c[19] * y + g_48568c[16] * x + g_48568c[25];
        real random_step = *(real const *)(parameters + 0xa8);
        real *random = (real *)(view + 0x150 + i * 12);
        random[2] = motion_random_step(random[2], random_step);
        random[0] = motion_random_step(random[0], random_step);
        random[1] = motion_random_step(random[1], random_step);
        real vx = *(real const *)(parameters + 0x90) * random[0] + *(real const *)(parameters + 0x30);
        real vy = *(real const *)(parameters + 0x94) * random[1] + *(real const *)(parameters + 0x34);
        real vz = *(real const *)(parameters + 0x98) * random[2] + *(real const *)(parameters + 0x38) - *(real const *)(parameters + 0x9c);
        real divisor = *(real *)(state + 0xbc + i * 4);
        if (divisor > 0.0001f)
        {
            real inverse = 1.0f / divisor;
            vx *= inverse;
            vy *= inverse;
            vz *= inverse;
        }
        vector3f *velocity = (vector3f *)(state + 0x24 + i * 12);
        velocity->i += vx;
        velocity->j += vy;
        velocity->k += vz;
        double magnitude = sqrt((double)velocity->k * velocity->k + (double)velocity->i * velocity->i + (double)velocity->j * velocity->j);
        real maximum = *(real *)(view + 0x174 + i * 4);
        if (magnitude > maximum)
        {
            normalize_inline(velocity);
            vector3f_scale(velocity, maximum, velocity);
        }
        real size = *(real *)(state + 0x48 + i * 4);
        *(real *)(view + 0x134 + i * 4) = scale_size ? size * radius : size;
        point3f moved;
        moved.x = step * velocity->i + old.x;
        moved.y = step * velocity->j + old.y;
        moved.z = step * velocity->k + old.z;
        point2f a = motion_project_point(&moved);
        point2f b = motion_project_point(position);
        real *offset = (real *)(view + 0x11c + i * 8);
        offset[0] = b.x - a.x + offset[0];
        offset[1] = (0.0f - b.y) - (0.0f - a.y) + offset[1];
        ++i;
    } while (i < 3);
    return true;
}


struct s_motion_material_entry
{
    byte unknown00[0x24];
    long state_index;
    byte unknown28[0x60];
};

struct s_motion_material_map
{
    byte unknown00[0x84];
    long count;
    s_motion_material_entry *entries;
};

real g_50941c;

// @retail 0x36ab0
void function_36ab0(void)
{
    s_motion_material_map *map = (s_motion_material_map *)g_4e0348;
    long i = 0;
    if (map->count > 0)
    {
        do
        {
            s_motion_material_entry *entry = (s_motion_material_entry *)((byte *)map->entries + i * 0x88);
            // Keep the entry address addition separate from the index read.
            long index = *(long const volatile *)&entry->state_index;
            if (index != NONE)
                function_39880((s_motion_state_groups *)g_4e3b44[index & 0xffff].bytes);
            ++i;
        } while (i < map->count);
    }
    g_4670fc = 1.0f;
    g_467100 = 1.0f;
    g_509420 = 0;
    g_509424 = 0;
    g_509428 = 0.0f;
    g_50941c = 0.0f;
}


real function_be6d0(long object_index, long attachment_index);
bool function_bad50(long object_index, long index, point3f *out);
void function_42850(long a, long b, point3f const *position, vector3f const *first,
    vector3f const *second, real scale, real width, vector3f const *third);
void function_429a0(byte type, long a, long b, long c, point3f const *position,
    point3f const *endpoint, vector3f const *first, vector3f const *second, real opacity);
byte g_55e6c7;

struct s_attached_render_entry
{
    dword type;
    long tag;
    long marker;
    short color_index;
    byte unknown0e[10];
};

void __stdcall function_d4cf0(long object_index, short group, dword flags);
void __stdcall function_41980(short type, long object_index);

// @retail 0x41c20
void __stdcall function_41c20(short type, long object_index, long value)
{
    while (true)
    {
        byte *object = ((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        function_d4cf0(object_index, type, (dword)value);
        function_41980(type, object_index);
        long next = *(long *)(object + 0x10);
        if (next != NONE)
            function_41c20(type, next, value);
        long sibling = *(long *)(object + 0xc);
        if (sibling == NONE)
            return;
        object_index = sibling;
    }
}

struct s_41c80_objects
{
    long unknown00;
    short count;
    short unknown06;
    short const *values;
    long const *indices;
};

struct s_41c80_state
{
    byte unknown00[0x14];
    s_41c80_objects *objects;
};

// @retail 0x41c80
void function_41c80(short type, s_41c80_state const *state)
{
    s_41c80_objects const *objects = state->objects;
    long i = 0;
    if (objects->count > 0)
    {
        do
        {
            function_41c20(type, objects->indices[(short)i], objects->values[(short)i]);
            objects = state->objects;
            ++i;
        } while (i < objects->count);
    }
}

// @retail 0x41980
void __stdcall function_41980(short type, long object_index)
{
    s_object_marker markers[65];
    color3f color;
    
    if (type == 0)
    {
        byte *object = (byte *)((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        long added = 0;
        for (long i = 0; i < *(long *)(definition + 0x94); ++i)
        {
            s_attached_render_entry *entry = ((s_attached_render_entry *)*(byte **)(definition + 0x98)) + i;
            if ((entry->type == 0x6c656e73 || entry->type == 0x4d475332 || entry->type == 0x7464746c) && entry->tag != NONE)
            {
                long marker = entry->marker;
                long count = function_b8d30(object_index, marker, markers, 65, false);
                real amount = function_be6d0(object_index, i);
                color = *(color3f const *)g_468710;
                if (entry->color_index)
                    function_bad50(object_index, entry->color_index, (point3f *)&color);
                for (long j = 0; j < count; ++j)
                {
                    if (entry->type == 0x6c656e73)
                    {
                        if (added < 64)
                        {
                            byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                            real scale = 1.0f;
                            if (tag[0x28] & 0x40)
                                scale = markers[j].matrix.scale;
                            function_2dba0(entry->tag, function_3e9c0(object_index), 0, object_index & 0xffff, added, &markers[j].matrix.position, &markers[j].matrix.forward, &color, 1.0f, amount, scale);
                            ++added;
                        }
                        else
                        {
                            if (!g_55e6c7)
                                g_55e6c7 = true;
                            break;
                        }
                    }
                    else if (entry->type == 0x4d475332)
                    {
                        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                        real scale = 1.0f;
                        if (*(long *)(tag + 8) > 0 && (**(byte **)(tag + 12) & 0x20))
                            scale = markers[j].matrix.scale;
                        function_42850(object_index, entry->tag, &markers[j].matrix.position, &markers[j].matrix.forward,
                            &markers[j].matrix.up, scale, amount, (vector3f *)&color);
                    }
                    else if (entry->type == 0x7464746c)
                        function_429a0(0, object_index, entry->tag, NONE, &markers[j].matrix.position,
                            NULL, &markers[j].matrix.forward, &markers[j].matrix.up, 1.0f);
                }
            }
        }
    }
}

void function_30c60(dword handle, point3f *position, long *out);

PRIVATE __forceinline real object_view_distance(point3f const *position)
{
    real x = position->x - g_4b9da0.x;
    real y = position->y - g_4b9da0.y;
    real z = position->z - g_4b9da0.z;
    return (real)sqrt(z * z + y * y + x * x) * g_4b9dcc;
}

// Matched in a full build with stand-in SDK headers (was 51): the fade is read with function_c6b90 (0xc6b90,
// inlined) whose two output pointers are passed through reference-bound locals, which keeps the getter's two zero
// stores as in retail; this function is not blocked by 0xff5f0.
// Before:
// MiniMax S (11-17): no new form, no lower score (51): the frame is still 0x18 against retail's 0x1c, because
// retail's final fade inlines the getter 0xc6b90 and keeps its two dead output stores, which take a float slot at
// +0xc; that getter lives in another file, and the initial alpha and kind homes still differ.
// MiniMax R (11-16): no new form, no lower score (51): the frame is still 0x18 against retail's 0x1c, because retail's final fade inlines the getter 0xc6b90 and keeps its two dead output stores, which take a float slot at +0xc; that getter lives in another file.
// score 51 (was 51): frame 0x18 versus 0x1c. Earlier 51 (was 62), 62 (was 177).
// Retail inlines the final fade getter 0xc6b90, which
// keeps its two dead output stores (`active = false`, then `fade = 0.0f`, a float slot at +0xc); the full build drops them
// (BLOCKED-BY 0xc6b90), so every offset from 0x1c up is 4 short. Earlier first line: frame 0x18 versus 0x1c; initial
// alpha/kind and final fade scratch homes remain.
// MiniMax E (11-16): no new form, no lower score (51); only the frame differs, 0x18 against retail's 0x1c, and
// retail's two dead getter stores; reading the fade once reproduces retail's tail instructions but leaves the frame
// and measures 62, so that body was reverted.
// MiniMax S (11-14): no new form, no lower score (62): the final fade through the existing 0xc6b90 getter still
// matches retail byte for byte on the reduced image, but the full build promotes the two output locals into registers
// and drops their dead zero stores, so the plain check gives 70 and the body stands: BLOCKED-BY 0xc6b90 (its body has
// to hold the two zero stores itself for the full build to keep them).
// MiniMax S (11-14): no new form, no lower score (62): the final fade through the existing 0xc6b90 getter (property
// 0x11000088, its two outputs zeroed) matches retail byte for byte on the reduced image, but the full build promotes
// those two output locals into registers and drops their dead zero stores, so the plain check gives 70: BLOCKED-BY
// 0xc6b90 (its body has to hold the two zero stores itself for the full build to keep them).
// frame 0x18 versus 0x1c; initial alpha/kind and final fade scratch homes remain.
// MiniMax C (11-09): no new form, no lower score (177).
// r136 lane 7 (fast): final fade through existing function_c6b90 (property 0x11000088, outputs zeroed) with kind not
// volatile 207 (789 bytes, frame 0x1c), with the volatile kind 237 (807 bytes; aligned 73 -> 59, the fade inside
// `kind == 0` is still read once where retail re-reads the field per use), with alpha before kind 205, kind not
// volatile alone 242 (747 bytes), the type test without reassigning `header` 237/207.
// MiniMax R (11-06): no new form, no lower score (177).
// MiniMax D (11-06, run 49): no new form, no lower score (177); the frame is still 0x18 against retail's 0x1c, and the initial alpha and kind stores and the final fade scratch stores keep our order.
// MiniMax R (11-01): no new form, no lower score: `kind` still has to be forced into memory to hold retail's frame, and the object table still rides in ebp.
// Earlier 177 (was 177): Frame 0x18 versus 0x1c; initial alpha/kind and final fade scratch homes remain.
// Earlier 175/194: the alpha is read from function_4baf0's second output (the byte pushed
// first, at [esp+0x30]). `result` is left unassigned until a branch sets it, so it stays in dl
// (the early `= false` made it a memory byte); the object is reached through a pointer to the table
// slot (retail reloads it from the slot after the alpha store); and `(real)sqrt(..) * g_4b9dcc`
// frees the x87 temporaries before the multiply, as retail does.
// Still open: retail keeps the object table in ebp and `kind` (spilled to [esp+0x18] after the
// flag test) in memory, with a frame 4 bytes larger (a float slot at +0x1c and the opacity byte get
// dead zero stores in the final fade block); ours needs `volatile` on kind to keep it out of ebp,
// which is why this stays a near miss. Retail loads the flag byte into al and tests it with
// `shr al,2; test dl,al` (dl == 1 from `result = true`), ours folds it to `test [esi+0x10a],4`
// whatever the spelling (bool cast, `& result`, byte local, `== 1`: all 175-190). Retail also reads
// the fade at [ecx+0x2b0] straight from memory at every use of the first clamp (no local); spelled
// that way (a repeated inline helper or a pointer local) the code matches there but swaps esi/edi
// (object / table offset) and scores 186-208.
// Round 109: object_view_distance's differences are `real` locals (they were `double`, which made LTCG keep the
// stack 8-byte aligned down from 0x335e0); the x87 block is byte for byte retail's either way, only the tail's pop
// placement moved: 175 -> 177 positional, LCS-aligned 76 -> 73. Its differences as `double` parameters change
// nothing (175, alignment kept); as one expression with no locals 176.
// Round 226: table already in EBP, frame 0x18 versus 0x1c and missing final fade scratch stores remain.
// Native object/mode/both 177; alpha first with those view subsets 212 each; byte result 173 at
// 768 bytes (four-point gain below the retention bar). Original body and shared helper restored.
// DH round 270: temporal/final/both clamps as PIN value expressions 177/177/177; each with live first-fade
// field reads 204/204/204; both with those reads and reversed cases/shared position lifetime 204/204. Frame
// stays 0x18 versus 0x1c; initial alpha/kind scheduling and final fade scratch stores remain. Entire
// original body, output roles and distance helper restored.
// P266: Entry float ownership for elapsed, the first fade pair and final fade, all seven
// nonempty subsets, then all with reversed declarations: 177 each. Producers stay at
// their original observations; frame remains 0x18 versus 0x1c. All original scopes restored.
// F320: native alpha/kind, position/region and output-byte role-name subsets 177 each (seven forms);
// all names with entry-declared byte outputs 177. Frame 0x18 versus 0x1c and fade scratch homes persist;
// all executable source restored.
// Round 360: elapsed accumulation/native signed-byte kind/result-from-alpha-test subsets,
// 177/208/177/208/177/208/208; all with byte result 213. Native byte kind reaches
// 773 bytes but retains the smaller frame. Earlier counts 175/194; original body restored.
// F327: Complete byte-to-alpha conversion, elapsed product/subtraction and both
// fade-complement producers, subsets 1/2/4/3/5/6/7, then immutable complements,
// 177 each. Original complete body and distance helper restored; four-byte frame gap remains.
// DL249: Existing final property getter with real+byte/real/byte/no receiver defaults
// 176/176/176/176; guarded-first 204; both property boundaries with both/real/byte defaults
// 206/206/206. Final getter plus alpha-tested final result 168 plain, 773/773 bytes: NOT kept
// (re-tests result after the fade, as J240's 168; retail clears dl after one comiss at 0x3dace).
// Both getters on that result graph 197; ordinary kind 143, 764 bytes: NOT kept because
// length moves away from the verified retail-length body. Inherited kind qualification restored.
// J240: Pure ordinal long/dword/word 177/177/203; with final logical result 168/168/192; shared point
// 168/192. Minimal original lookups plus only the logical result 168 (retail length 773) NOT kept:
// `result = result && !(alpha <= 0.0f)` re-tests result, which retail does not (0x3dace: one comiss,
// then xor dl, dl). Body restored at 177; frame and fade scratch homes remain.
// F339: Existing shared scalar-clamp producers for temporal/guarded-first/final
// fades, seven subsets, then the final producer consumed directly:
// 177/177/177/177/177/177/177/177. Original body and shared helper restored; the four-byte frame
// gap and initial/final scratch scheduling remain.
// Lane 16 r80 (fast unless marked plain): kind without volatile 242 (747 bytes); alpha declared ahead of kind 178;
// the final fade through existing 0xc6b90 (property 0x11000088) 176 plain; that with kind not volatile 242;
// with the getter's outputs zeroed (`real fade = 0.0f; bool active = false;`, retail's two dead stores and frame
// 0x1c) and kind not volatile 231 (758 bytes: kind takes EBP, the table is not cached). Diagnostic, NOT kept
// (re-tests result): that graph with `result = !(alpha <= 0.0f) && result;` 113 (775 bytes, first +0x58: entry,
// both cases and frame are retail's; left the folded `test [esi+0x10a],4` and the first fade), `result &= alpha >
// 0.0f` 116; on it the flag as word/dword/byte-cast/no bool cast/`!= 0` 113 each, the first fade read per use
// 144, kind as signed char 220. On the if-form graph: `!(alpha > 0.0f)`, `0.0f >= alpha`, a ternary, the getter's
// return tested, the header unnamed, result initialised false 231 each; kind as signed char 211 (773 bytes),
// short 226, declared after alpha 233. Body restored.
// DL257: Complete failure outcome in bool/byte/short/long, branch-set 167/167/210/208; entry-
// default 213/213/219/217. NOT kept: 167 reaches 773 bytes by an inverted failure carrier with
// recursive neg/sbb/inc and a final test/sete; retail copies and returns its positive byte
// directly. The other forms regress. Original complete body and positive result restored; frame
// and fade scratch homes remain.
// F370: Complete temporal lower/upper/interior result producer, with object/alpha/
// final outcome subsets 0/1/2/3/4/5/6/7: 176/218/176/218/176/218/176/218.
// NOT kept: one-point leads are below the bar and leave frame 0x18 versus 0x1c;
// all original positive result paths, local qualifier and full body restored.
// Y568: Existing two-use distance helper with native point-reference/value inputs and
// real/double/output-reference results: 177/177/185/177/177/177/185/177. NOT kept:
// no contract matches; original helper, both callers and complete body restored.
// Frame 0x18 versus 0x1c and fade scratch stores remain.
// P319: Existing distance producer through native/shared-difference magnitude, point
// distance and squared-length graphs: forced 177/236/236/177; ordinary inline
// 234/310/310/234. NOT kept: no gain; point snapshots grow the frame, ordinary
// expansion loses native arithmetic. Original distance helper and whole body restored.
// DH343: Positive signed-char/short/long/dword carrier, branch-defined then entry-default:
// 173/176/173/173/173/176/173/173. NOT kept: gains one/four are below the gate; original bool
// result and body restored. Frame 0x18 versus 0x1c and fade scratch homes remain.
// J252: Existing position/opacity/recursive inline contracts and own cdecl: 224/177/177/224/224/177/224/176.
// NOT kept: no qualifying native-frame gain or byte equality; original full graph restored.
// AC542: Final raw kind captured before alpha output in signed-char/short/long/dword
// 177/175/176/176; property equality in bool/byte/short/long 168/168/168/170.
// NOT kept: one-use output temporaries are unmatched; the equality forms materialize and
// re-test a Boolean before publication. Original result/body and frame 0x18/0x1c remain.
// AC544: Entry declarations for alpha, existing qualified kind and paired query bytes,
// seven subsets then reversed complete declarations: 177/177/177/177/177/177/177/177. NOT kept: no
// native frame repair or match; original producers, scopes, qualifiers and whole body restored.
// DH366: Original-site first raw/clamped fade equality, accepted/rejected bool/byte/short/long outcomes:
// 177/177/177/177/177/177/177/177. NOT kept: no byte match or qualifying native repair; original complete
// body restored.
// DH373: Lazy parent-presence/absence outcomes at their original recursive admission,
// bool/byte/short/long: eight at 177. NOT kept: no native frame repair or equality; original
// positive recursive result and full body restored. Initial/final fade scratch homes remain.
// AC546: Original-site case-zero kind admission/rejection outcomes in bool/byte/short/long:
// eight at 177. NOT kept: one-use outcomes do not match or repair the frame; original
// predicate, qualified kind observations, positive result and complete body restored.
// DH366: Original-site first raw/clamped fade equality, accepted/rejected bool/byte/short/long outcomes:
// 177/177/177/177/177/177/177/177. NOT kept: no byte match or qualifying native repair; original complete
// body restored.
// B453: Common opacity query after the two genuine position producers, short/long category and both
// complete admission/position arm orders: 311/322/310/299/312/323/311/300. NOT kept: no byte match;
// original complete body restored. Native frame and initial/final fade scratch homes remain.
// Y602: Standard min/max temporal/first/final clamp subsets and immutable consumers:
// 155/252/258/289/283/288/322/322.
// NOT kept: 155 restores frame 0x1c by adding two out-of-line clamp calls absent from retail; no native-
// shape keeper; original complete body and helpers restored.
// F397: Branch-owned first/opacity bytes and shared native position, seven subsets,
// then query-entry region output: 177/177/177/177/177/177/177/177. NOT kept: all tie;
// original output scopes and body restored. Native frame/fade scratch homes remain.
// R221 (lane 3): final fade through existing function_c6b90(index, 0x11000088, &fade, &active) after `real fade = 0.0f;
// bool active = false;` plain 176 (765): the full build drops both zero stores and keeps frame 0x18 (the one-file harness
// keeps them: 159, frame 0x1c). Harness: that with kind not volatile 231; first fade read per use 179; alpha before
// kind 160; the 0x10a flag through a word bit-field view 176 (retail's mov al/shr al,2/test dl,al), with the getter 166.
// Round 64 (lane 15, fast): temporal clamp as two separate ifs 177; `result` left true and `if (alpha <= 0.0f) result =
// false;` after the clamp 198 (743 bytes), with two ifs 198; `result = false` in the lower arm and the test only in
// the interior arm 178; the final fade read through `header->object` without reassigning `object` 177. Retail threads
// the alpha test into each clamp arm and keeps the final getter's two zero stores (frame 0x1c).
// Lane 5 r164 (plain unless marked): the final fade through 0xc6b90 after zeroed outputs is 176 plain but 159 fast because
// the reduced image leaves weapons.cpp out: with 0xff5f0 (the getter's default-path callee) in the image, even with its body
// reduced to `return false;`, both zero stores go and the frame stays 0x18, so the getter's own body decides it
// (BLOCKED-BY 0xc6b90, unmatched). That with kind not volatile 242 (fast 231). The 0x10a flag through a file-scope word
// bit-field view (the idiom of the unit and biped files) 176: retail's `mov al; shr al,2; test dl,al` exactly; with the first
// fade read at each use through a re-fetched object pointer (`PIN(field, 0, 1) == field`, then `alpha *= 1 - field`) 172 (775
// bytes; flag test through first fade is retail's sequence, only the shared false block sits elsewhere); on 172: the getter
// route 173 fast (171 without weapons.cpp), the alpha test false-arm first 172 (false block moves inline), one false arm for
// both tests 172 (same code), kind not volatile 230, case 0 reading the field directly 170 (byte compares, a mixed read).
// The per-use first fade without the view 185. On 170: the later kind test reading the field 173, alpha declared ahead of
// kind 211, case 2 through header->object 178, the getter route 171; `if (alpha <= 0.0f) result = false; else` 152 (151 on
// 172; NaN goes the other way from retail's jbe, so not retail's test; alone 183); `result = false` initialised with the
// else arms gone 200. Kept, 62: the 10a bit through the existing s_object_flags_10a (now in globals.h), the first fade read
// at each use (main's named fade/clamped on the kept body 147), `result = alpha > 0.0f; if (result && kind == 0)` (172 at
// 773 bytes with kind still volatile), and kind no longer volatile (230 on 172 before the result form). Without the view
// 175. The getter route on 62, `active` cleared first: fast MATCH, plain 70 (build/r164/0x3d7f0-getter-fastmatch.patch in
// the m3-lane5 worktree); the weapon getter's tail as `*active = *value > 0.0f` still 70.
// Lane 12 r102: kind not volatile, the final fade through the existing function_c6b90(index, 0x11000088, &fade,
// &active) after `real fade = 0.0f; bool active = false;`, `if (alpha <= 0.0f) result = false;` and the first fade
// read at each use through an object local: fast 139 at 773 bytes with retail's skeleton (frame 0x1c, kind spilled at
// entry, table in EBP, both zero stores), plain 238 (751 bytes: no zero stores, kind in EBP). A fast check listing
// 0xff5f0 as well (its source joins the image) also gives 238: the zero stores survive only while the getter's
// default-arm callee 0xff5f0 is outside the image. Retail links 0xff5f0 too and keeps them, so the lead is our
// unmatched 0xc6b90 (inlined here) and 0xff5f0 bodies: BLOCKED-BY 0xc6b90, 0xff5f0. Same graph with a pointer
// local to the field fast 155; with kind volatile plain 190; with kind as signed char plain 209 (object index in EBP);
// main body with the first fade read per use through the object local plain 185 (773 bytes).
// M3 lane 10 r104 (diagnostics only, all restored): the getter route (`bool active = false; real fade = 0.0f;` then
// function_c6b90(index, 0x11000088, &fade, &active)) with another lane's matched 0xc6b90 body applied (not on main) plain
// 70 (762 B, frame still 0x18); with 0xff5f0's tail as `*active = *value > 0.0f` as well 70. Fast bisection of the getter
// route on main: alone MATCH; with 0xd1210 or 0xd1410 in the image MATCH; with 0xff5f0 in the image 70; with 0xff5f0 also
// storing its `value` pointer in a scratch global 64 (768 B: the fade zero store returns). So the full build drops both
// zero stores because link-time code generation proves 0xff5f0 keeps its two output pointers to itself; retail's build
// did not. Retail's 0xff5f0 only stores through them at its tail, and src/weapons.cpp was link-time compiled in retail
// (50 of its 89 functions take register arguments), so the cause is still open: BLOCKED-BY 0xff5f0. Every other
// difference here is the frame offset, so no form inside this body was left to try.
// P489: Complete positive/default/failure-first/nested initial result regions, original then native branch-
// produced temporal clamp results: 230/201/232/62/231/203/233/62. NOT kept: No exact native result/frame
// repair; tested-result alternatives are equality-only. Original positive Boolean graph, ordinary kind, all
// per-use fade observations, distance helper, recursion and full body restored. The missing native final-query
// scratch remains.
// P489 continued: Native final query outputs through scalar value initialization, real/byte/paired
// memset, paired fill/fill-n/uninitialized-fill/uninitialized-fill-n: 70/70/70/70/70/70/70/70.
// NOT kept: no full-context native scratch/frame equality. Original direct final field
// clamp, positive result, recursive/query inputs, helpers and complete body restored.
// P508: Actual final getter fade/active outputs at entry, across recursion and after recursion,
// with original-site zero defaults, eight ownership/order forms: 70/70/70/70/70/70/70/70.
// NOT kept: no native scratch/frame equality with the default provider present; original complete body restored.
// M3 lane 12 r126 (fast, diagnostics only): the getter route (`real fade = 0.0f; bool active = false;` then
// function_c6b90(object_index, 0x11000088, &fade, &active)) re-measured 2 on the reduced image (only the two zero stores
// in the other order) and 70 with 0xff5f0 in the image. Retail's 0xff5f0 stores through its two output pointers only at
// its two found tails and passes neither to a call, and it is on no call cycle with this function or 0xc6b90 (its
// callees 0x102070, 0x10cf50, 0x162b10, 0x162b70 and 0x242140 reach neither), so its code comes ahead of this function's
// in the link either way; a dead call from 0xff5f0's default arm back into this function (a cycle) stops 0xc6b90
// inlining here at all (219). Retail keeps `active` in the dead object-index argument slot ([esp + 0x20] after the
// pops) and the fade at +0xc. No form inside this body changes more than those two stores. BLOCKED-BY 0xff5f0.
// New forms on this body, both worse: the first fade's range test as `field >= 0.0f && field <= 1.0f` 155 (724 bytes);
// the shadow-mode switch as `if (mode == 0) ... else if (mode == 2) ...` 115 (766 bytes).
// M3 lane 13 r97 (fast with 0xff5f0 and 0xc6b90 in the image; plain where marked): the getter route (`bool active = false;
// real fade = 0.0f;` then function_c6b90(object_index, 0x11000088, &fade, &active)) re-measured 70; on it, the alpha
// update under `if (function_c6b90(...))` 70; both outputs declared at function scope, after `alpha` or after `result`,
// 70/70; `real fade` first with `if (!(alpha > 0.0f))` 70. Kept: the final fade clamp through the file's PIN macro
// (`alpha *= 1.0f - PIN(field, 0.0f, 1.0f)`), 51 plain at retail's 773 bytes (0x33400 stays matched, 0x32350 stays
// 445). The differing-line count is unchanged (51): the field read at each use of the macro fills the 11 bytes that
// retail's two dead getter stores take, so the remaining difference is still the frame and those stores. On the 51
// body: the temporal clamp as PIN 92; PIN reading `header->object` with no object reassignment 51 (same code); PIN
// through the header table at each use 52 (774 B); `alpha = (1.0f - PIN(...)) * alpha` 51 (same code).
// MiniMax E (11-15): no new form, no lower score (62): the last difference is blocked by 0xc6b90, whose body lives in src/unit_object_type.cpp, a file this lane may not edit.
// M3 lane 1 r109 (fast with 0xff5f0 and 0xc6b90 in the image, = plain for this graph; 0xc6b90 is now matched on main): the
// getter route (`real fade = 0.0f; bool active = false;` then function_c6b90(object_index, 0x11000088, &fade, &active),
// `alpha *= 1.0f - fade`) re-measured 70 (762 B, both zero stores dropped, frame 0x18); on it, fade left uninitialised 70;
// the object re-fetch kept ahead of the getter 70. Diagnostics in 0xff5f0 (ours, restored): its tail as c6b90's
// single exit (`if (found) { clamp; stores } return found;`) 70 (0xff5f0 389 -> 430); its two stores under `if (value)` /
// `if (active)` 70 (0xff5f0 408). Neither a null test nor a single exit makes LTCG treat 0xff5f0's output pointers as
// escaping, so the stores still go: BLOCKED-BY 0xff5f0 (retail's 0xff5f0 passes neither pointer to a call: its five callees
// take only the weapon and an index).
// P557: Native final getter output lexical identities in four fade/active name pairs, both declaration
// orders: 70/70/70/70/70/70/70/70. NOT kept: no native scratch/frame equality with the default provider
// present; original direct fade clamp and full body restored.
// P559: Actual paired final-query output record, fade/active orders with aggregate/field/value/memset
// defaults: 70/70/70/70/70/70/70/70. NOT kept: unmatched grouped outputs; no native final-query scratch/frame
// equality; original direct fade and complete body restored.
// P561: Initial query byte outputs owned by each switch case, paired/either output,
// case-entry/call-site declarations and both pair orders: eight 51 at 773/773 bytes.
// NOT kept: no native frame word or final scratch recovery; original common
// byte outputs, final direct fade path and complete executable body restored.
// Stored point by byte-range copy, coordinate stores, setter or coordinate
// aggregate, then each with the region output scoped to its query:
// 51/205/205/205/51/205/205/205. NOT kept: the range copies tie; coordinate forms
// use SSE differences and grow the frame to 0x20. Final fade scratch remains.
// Round 615: frame 0x18 versus 0x1c; the final getter output stores are still absent.
// Position memcpy 51; direct position distance 221; opacity conversion after switch 262;
// named transmission 51; fade set in branches 51; header byte member 51;
// region scope limited to query 51; special result before alpha store 51.
// MiniMax R (11-17): no new form, no lower score (51): only the frame differs, 0x18 against retail's 0x1c, because retail's final fade keeps two dead stores (a byte 0 in the object index argument home and a float 0.0f in an extra frame slot) that this build eliminates; the fade block itself reproduces retail's sequence exactly when the fade value is named and clamped by hand, and the getter route that would keep them (function_c6b90 with zeroed outputs) is BLOCKED-BY 0xff5f0 in src/weapons.cpp.
// MiniMax A (11-18): no new form, no lower score (51); only the frame differs (0x18 against retail's 0x1c) and the two dead
// stores retail keeps (a byte 0 in the object index argument home and a float 0.0f in the extra frame slot); the getter route
// that would keep them is BLOCKED-BY 0xff5f0 in src/weapons.cpp, so the fade stays read at each use.
// @retail 0x3d7f0
bool __stdcall function_3d7f0(long object_index, real *out_alpha, bool *out_special)
{
    bool result;
    s_scalar_object_header *header = &((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff];
    byte **object_slot = &header->object;
    byte *object = *object_slot;
    s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
    long kind = *(signed char *)(object + 0xaa);
    real alpha = 0.0f;
    if (!(object[6] & 1))
    {
        byte first_output, opacity;
        switch (definition->lightmap_shadow_mode)
        {
        case 0:
            if (kind == 0 || kind == 1)
            {
                point3f position;
                long region;
                function_30c60(object_index, &position, &region);
                function_4baf0(object_index, object_view_distance(&position), &first_output, &opacity);
                alpha = opacity * 0.003921568859368563f;
            }
            break;
        case 2:
        {
            point3f position = *(point3f *)(*object_slot + 0x30);
            function_4baf0(object_index, object_view_distance(&position), &first_output, &opacity);
            alpha = opacity * 0.003921568859368563f;
            break;
        }
        }
    }
    result = alpha > 0.0f;
    if (result && kind == 0)
    {
        if (TEST_FIELD_BIT(((s_object_flags_10a *)object)->bit2))
        {
            real elapsed = (g_510c54->game_time - *(long *)(object + 0xbc)) * g_510c54->rate - 2.0f;
            alpha = 1.0f - elapsed;
            if (alpha < 0.0f) alpha = 0.0f;
            else if (alpha > 1.0f) alpha = 1.0f;
            result = alpha > 0.0f;
        }
        // the fade is read from the object at each use, as retail does
        if (PIN(*(real *)(((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object + 0x2b0), 0.0f, 1.0f) ==
            *(real *)(((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object + 0x2b0))
            alpha *= 1.0f - *(real *)(((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object + 0x2b0);
    }
    if (!result && *(long *)(object + 0x14) != NONE)
        result = function_3d7f0(function_baf80(object_index), &alpha, out_special);
    header = &((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff];
    if ((1 << ((byte *)header)[3]) & 3)
    {
        bool active = false;
        real fade = 0.0f;
        real *fade_target = &fade;
        real *const &kept_fade = fade_target;
        bool *active_out = &active;
        bool *const &kept_active = active_out;
        function_c6b90(object_index, 0x11000088, kept_fade, kept_active);
        alpha *= 1.0f - fade;
        if (alpha <= 0.0f)
            result = false;
    }
    *out_alpha = alpha;
    *out_special = kind == 12;
    return result;
}

s_cached_input_list g_5246f8[4];

// @retail 0x33400
long function_33400(byte const *list, s_cached_input *output, long mode)
{
    long selected = 0;
    long view = g_4b9ed4 < 0 ? 0 : g_4b9ed4 > 3 ? 3 : g_4b9ed4;
    long count = *(short const *)(list + 4);
    for (long i = 0; i < count; ++i)
    {
        long object_index = (*(long const **)(list + 0xc))[(short)i];
        word flags = (*(word const **)(list + 8))[(short)i];
        if (mode == 0 || (mode == 1 && (flags & 0x4000)) || (mode == 2 && (flags & 0x8000)))
        {
            real alpha;
            bool special;
            if (function_3d7f0(object_index, &alpha, &special) && selected < 32)
            {
                byte *object = ((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
                point3f position = *(point3f *)(object + 0x30);
                real radius = *(real *)(object + 0x3c);
                s_cached_input *entry = &output[selected];
                entry->data[1] = object_index;
                entry->data[2] = object_index;
                *(real *)&entry->data[3] = alpha;
                real depth = g_4b9e2c * position.y + g_4b9e20 * position.x + g_4b9e38 * position.z + g_4b9e44;
                if (!(depth >= 0.0f)) depth = 0.0f - depth;
                if (!(depth > 0.1f)) depth = 0.1f;
                *(real *)&entry->data[4] = (g_4b9ed0 / depth) * radius * 2.0f;
                ((byte *)entry)[0] = function_3e9c0(object_index);
                ((byte *)entry)[2] = 0;
                ((byte *)entry)[1] = 0;
                ++selected;
            }
        }
    }
    function_4a7c0(&g_5246f8[view], output, selected);
    return function_4ac30(&g_5246f8[view], output, 4, 0.005f);
}

struct s_sort_record;
/* an entry of the scenario's tag reference block at +0x37c, indexed by a placement record's first byte */
struct s_placement_tag_reference
{
    dword group_tag;
    long tag_index;
};
typedef bool (__stdcall *t_41640_fill)(long, void *, long, long, long, void *, s_sort_record *);
typedef void (__stdcall *t_41640_draw)(long, long, long, long, long, dword, byte *);
bool __stdcall function_4de20(long, long, long, long, long, long, s_sort_record *);
bool __stdcall function_4dfa0(long, long, long, long, long, long, s_sort_record *);
bool __stdcall function_4e0d0(long, long, long, long, long, long, s_sort_record *);
void __stdcall function_4e260(long, long, long, long, long, dword, byte *);
void __stdcall function_4ebd0(long, long, long, long, long, dword, byte *);
void __stdcall function_4ede0(long, long, long, long, long, dword, byte *);
void __stdcall function_4efa0(long, long, long, long, long, dword, byte *);

// The draw callback of the sixth placement type. It forwards to the same routine as the fifth
// type's (function_4efa0), so the linker folds the two identical bodies into one; retail's
// table setup still treats them as two different addresses (both are stored as immediates).
void __stdcall draw_placement_type_5(long tag, long context, long first, long second,
    long third, dword handle, byte *material)
{
    function_4ede0(tag, context, first, second, third, handle, material);
}

void function_40e30(short group, long tag, real distance, long level, short kind,
    dword and_mask, dword or_mask, t_41640_fill fill, dword value, void *context);
point3f *function_3f220(dword a, dword b, dword c, point3f *out);
struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index,
    point3f const *center, real radius, bool *contained);
real distance3d(point3f const *a, point3f const *b);
real g_525980[3];
struct s_1321f0;
bool function_1321f0(long index, s_1321f0 const *data);

// MiniMax C (11-09): no new form, no lower score (4).
// MiniMax R (11-06): no lower score (4); the definitions local declared before the table and assigned after the early return gives the same 4, so the two spill homes are not decided by the declaration order; a 600s permuter run (50 tries) found nothing lower, and the same source matches retail in a 47-source reduced image, so the four differing spill homes come from the build context.
// score 4 (was 4): table/definitions spill homes remain exchanged; native frame and length agree.
// native frame and extent agree; table/definitions stack homes remain exchanged.
// native frame and extent agree; table/definitions stack homes remain exchanged.
// MiniMax D (11-05): no new form, no lower score (4); retail spills `table` at [esp+0x10] and `definitions` at [esp+0x1c] where ours uses [esp+0x14] and [esp+0x18], and the loop back edge then reloads the two bases from the swapped slots; the declaration orders and the union's place give the same 4, and assigning the definition pointer first gives retail's two homes but hoists its load above the early return (14).
// Matched in round 55's full check with no edit here (a link-time side effect of other bodies in this file).
// Before that: only the frame slots of `table` and `definitions` were swapped (retail: table at the lower
// slot). This body matched in lane 13 round 38's full image and matched on its earlier --fast image; adding almost any one
// other source to that image swaps the two slots, so the tie is decided by whole-image LTCG state, not this body.
// Tried (all 4): `definitions` renamed `scenario`; that and `table` renamed `placements`; `definitions` declared
// with the table and assigned after the early return; declared before the table; both as `byte const *`.
// Round 73 (--fast now also 4): the `handle` union declared first 4 (plain 4); `view` declared at function scope 4;
// the callback arrays declared after table/definitions 30; `draw` declared before `fill` 17; both pointers `byte *const`
// 4. Compile-only /FAs: the two homes follow which pointer is assigned first (reading the scenario first flips them,
// but moves its load above the early return); names never move them.
// Round 226: current reduced control 4; native table, native palette, both roots, native range, native record,
// range/record, all native views and const callback arrays each 4. Original body restored.
// Round 307: positive enabled scope 4; handle per view 4; table root at every use 217;
// decoded/position/both point storage at entry 4/4/4; node/range and record/geometry storage at entry 4/4.
// Lane 5 r137 (lead): this function matched in a full check whose only code change was 0x3a5c0's
// quantizer (023540.cpp, a macro with one shared value; not kept), with no edit here: a link-time
// side effect. Its own last difference (+0x6b, score 4) may need the form that change implied.
// Round 138: view count in a local 104; guard split into separate continues 4; record index written in the
// record address 4; handle view stored before the range 11; definitions assigned just before the loop 4.
// F404: Frustum/distance admission outcomes in bool/byte/short/long:
// 4/4/4/4/4/4/4/4. NOT kept: no exact outcome-storage form. Original complete body restored;
// table/definitions spill homes remain exchanged.
// DL267: Direct decoded output; actual callback signatures; native node cursor; emission-only handle; sole exit; enabled callback lifetime; readonly node; readonly leaf records: 152/4/170/228/4/30/4/4.
// NOT kept: no byte match or eligible native repair; original executable source restored.
// Round 94 (lane 9, fast): the handle's record field from `record_index` 127; the callback type read once into a
// `type` local for both arrays 61; the definition index as `long` from a signed-char read 161; the table read
// ahead of the callback arrays 21; the handle as a named aggregate type at file scope 4. Body restored.
// @retail 0x41640
void function_41640(void)
{
    t_41640_fill fill[6] = { (t_41640_fill)function_4de20, (t_41640_fill)function_4dfa0,
        (t_41640_fill)function_4dfa0, (t_41640_fill)function_4e0d0,
        (t_41640_fill)function_4e0d0, (t_41640_fill)function_4e0d0 };
    t_41640_draw draw[6] = { function_4e260, function_4ebd0, function_4ebd0,
        function_4ede0, function_4efa0, draw_placement_type_5 };
    byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
    if (!g_4ba025) return;
    byte *definitions = (byte *)g_4e0350;
    union
    {
        dword value;
        struct { dword view : 8; dword record : 22; dword distance : 2; } fields;
    } handle;
    for (long view = 0; view < (long)g_509438; ++view)
    {
        s_octree_output *node = &g_4c5700[view];
        byte *range = *(byte **)(table + 0x24) + node->node * 24;
        handle.fields.view = view;
        for (long i = 0; i < *(short *)(range + 0x12); ++i)
        {
            long record_index = *(long *)(range + 0x14) + i;
            byte *record = *(byte **)(table + 0x1c) + record_index * 24;
            s_geometry_block_info *block = (s_geometry_block_info *)(*(byte **)(table + 0x14)
                + *(short *)(record + 6) * 0x2c);
            signed char definition_index = record[0];
            if (definition_index < 0 || definition_index >= *(long *)(definitions + 0x378)
                || *(short *)(record + 0xa) <= 0 || !function_12de70(block, 3)) continue;
            s_placement_tag_reference const *reference =
                &(*(s_placement_tag_reference **)(definitions + 0x37c))[(signed char)record[0]];
            if (reference->tag_index == NONE) continue;
            long part_index = record[1];
            byte *definition = g_4e3b44[reference->tag_index & 0xffff].bytes;
            if (part_index >= *(long *)(definition + 0x10)) continue;
            byte *part = *(byte **)(definition + 0x14) + part_index * 20;
            long instance = record[2];
            if (instance >= *(long *)(part + 0xc)) continue;
            byte *entry = *(byte **)(part + 0x10) + instance * 40;
            long material_index = entry[4];
            if (material_index >= *(long *)definition) continue;
            long material = (*(long **)(definition + 4))[material_index * 2 + 1];
            if (material == NONE) continue;
            dword packed = *(dword *)(record + 0x14);
            point3f decoded;
            point3f position = *function_3f220(packed & 0x3ff, (packed >> 10) & 0x3ff, packed >> 20, &decoded);
            real radius = record[3] * 0.0313725508749485f;
            position.x = node->center.x - 4.0f + position.x;
            position.y = node->center.y - 4.0f + position.y;
            position.z = node->center.z - 4.0f + position.z;
            short section = *(short *)(record + 4);
            if (section == NONE || !function_1321f0(section, (s_1321f0 const *)&g_547f88)) continue;
            long frustum = ((signed char *)g_547f88.indices)[section];
            bool contained = false;
            if (function_165010((s_frustum_set_view *)g_547f88.context, frustum,
                    &position, radius, &contained))
            {
                long distance_class = entry[9];
                if (g_525980[distance_class] > distance3d(&g_4b9da0, &position) - radius)
                {
                    handle.fields.record = *(long *)(range + 0x14) + i;
                    handle.fields.distance = distance_class;
                    function_40e30(0, material, 10000.0f, 0, 0xffff, NONE, 0,
                        fill[part[4]], (dword)draw[part[4]], (void *)handle.value);
                }
            }
        }
    }
}

void function_0497a0(s_view_source *source, s_view_result *result, s_view_camera *camera,
    s_view_flags *flags, vector3f const *first, vector3f const *second, real scale,
    long *mode, vector4f *old_first, vector4f *old_second);
bool function_1650a0(s_view const *view, point3f const *point, s_bounds3d *bounds, real radius);
struct short_rect { short v0, v1, v2, v3; };
struct short_rect_pair { short_rect a, b; };
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)

PRIVATE __forceinline real clamp_4a1e0(real value, real lower, real upper)
{
    return value < lower ? lower : value > upper ? upper : value;
}

void matrix4x3_from_forward_and_up(transform4x3f *out, vector3f const *forward, vector3f const *up);
extern vector3f *g_4687bc;
struct s_collision_result_1697c0
{
    long type;
    real t;
    point3f point;
    byte unknown14[0x24 - 0x14];
    short unknown24;
    byte unknown26[0x5c - 0x26];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

PRIVATE __forceinline void shadow_corner(transform4x3f const *matrix, point3f const *offset, point3f const *origin,
    point3f *corner)
{
    real x = offset->x;
    real y = offset->y;
    real z = offset->z;
    if (matrix->scale != 1.0f)
    {
        x = matrix->scale * x;
        y = matrix->scale * y;
        z = matrix->scale * z;
    }
    corner->x = matrix->forward.i * x + matrix->up.i * z + matrix->left.i * y + matrix->position.x;
    corner->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
    corner->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
    vector3f_add((vector3f const *)corner, (vector3f const *)origin, (vector3f *)corner);
    corner->z += 0.001f;
}

// MiniMax E (11-20): no new form, no lower score (231): the frame is still 0xd0 against retail's 0xe0 and the four spill homes (y, z, radius, negated radius) still sit at other offsets than retail's (y+4, radius+8, -radius+0xc, z+0x10); the forms recorded below land at 225/226 with a longer body.
// MiniMax I (11-19): no new form, no lower score (231): the frame is still 0xd0 against retail's 0xe0 and the radius, the negated radius and the scaled zero still take other spill slots; the four-channel colour and the `-radius` forms recorded below still land at 225/226 with a longer body, so the body is unchanged.
// score 231 (was 231): The frame is 0xd0 against retail’s 0xe0. The radius, negative radius, and scaled zero still occupy different homes.
// GB r11 lead, held as a patch (not kept: the forms are kept only with a byte match): 211 against 231: four-channel color4f recovers retail's exact 0xe0 frame (collision.unknown24 at +0xac, collision at +0x88, color.alpha at +0x78, first diff +0x2->+0x38), and accumulated rx/ry/rz rows with origin-first vector3f_add match retail's 411/411 instruction count and D3D push-buffer alignment (masked diffs 99, 0x141710/0x1697c0 MATCH); still differs: radius spills at +0xc vs retail's +8, x-row sums left+forward+up where retail sums forward+up+left, and Corner 2 CSEs scale*radius.
// GB11 (11-25, 50 trials): three-term x-row local products scalar 401, color4f 226, alpha-only color4f 223, split apply_point 218/223, in-place corners 367, real_point3d_set offset 376, local out 372; rx/ry/rz += with forward+up+left recovers all four retail spill slots (+4/+8/+0xc/+0x10) and masked diffs 89 at 1797 B (406/411 insns, positional 375 across split/single-sum/in-place/mul-assign/origin-first/explicit-offset/offset-first/add-after; real_point3d_set 395, direct origin add 387 at 1819 B, field += 718, local world 722, neg_radius 407); rx up+forward+left 355 (410/411 insns, with xzy scale 221 at 411/411); rx left+forward+up + ry/rz up+left+forward + origin-first vector3f_add + color4f hits 211 (1825 B, fd=+0x38, fr=0xe0, n=411/411, mk=99, plain-confirmed, kept; same 211 split or alpha-only; rx forward+left+up 376; 1eff40 x + ry/rz += unsplit 365 mk=82, split 211).
// Earlier (231, was 231): Frame 0xd0 versus 0xe0. The radius, negative radius and scaled zero occupy other spill slots.
// MiniMax H (11-06): no new form, no lower score (231).
// The corner geometry does not match retail: retail's four corners are (-r,-r), (-r,+r), (+r,+r) and
// (+r,-r), while ours twists the quad as (-r,-r), (+r,-r), (+r,+r), (-r,+r), and the vertex writes follow
// the same twisted order. Retail also uses a frame of 0xe0 against our 0xd0, with the corner count home at
// [esp+0xac] against our [esp+0x9c] and the radius home at [esp+8] against our [esp+0xc]. A variant with
// retail's corner order, the colour aggregate and the helper call reordered reached 225, six short of the
// keep bar, so the original body is restored.
// b6R (11-06): no new form or lower preview count (231). No behaviour difference found.
// Walked retail 0x32ce0-0x333f8 statement by statement against this source: object index masked 0xffff at 0x32cf8 and
// the object pointer from record +8 (0x32d08); the NONE store at 0x32d0c precedes the opacity test (comiss 0x32cfd,
// jbe 0x32d16, so opacity > 0.05 and ordered); collision call 0x32d38 with the arguments pushed at 0x32d29-0x32d33;
// radius = object+0x3c * 0.35 (0x32d45-0x32d4a); matrix call 0x32d62 (ecx out, edx forward object+0x70, eax up
// object+0x7c); one scale != 1.0 test per corner (ucomiss 0x32d6d, 0x32ddd, 0x32ee3, 0x32f9a) scaling x, y and z;
// each coordinate sums in the source's order (x forward, up, left; y and z up, left, forward) then adds position,
// origin (origin + sum, 0x32e68/0x32e75/0x32e8e) and 0.001 to z only (0x32e92, 0x330f1); the radius sign: forward
// term gets -r, +r, +r, -r (0x32d89 -> 0x32de7, 0x32eb3 -> 0x32ef1, 0x32f73 -> 0x32fa4, 0x33066) and left term -r, -r,
// +r, +r (0x32d9b -> 0x32df5, 0x32eb9 -> 0x32f01, 0x32f67 -> 0x32fbc, 0x3302e -> 0x33074), which is this source's
// (-r,-r) (r,-r) (r,r) (-r,r) in (x, y); z of vertices 1-3 is corner 3's z in [esp+0x74] (written 0x330f9, read
// 0x332b2, 0x33340, 0x333d1; corners 1 and 2 never compute z); alpha = (1.0 - t) * opacity at 0x33107-0x33110; Begin
// 0x33129 then colour (1,1,1,alpha) 0x33186-0x331a1, uv (0,0) (0,1) (1,1) (1,0), positions with w 1.0, End 0x333ec.
// The lead that retail's corners are (-r,-r) (-r,+r) (+r,+r) (+r,-r) is contradicted by those operand uses.
// What the compiled code does differently: it regroups each coordinate sum (our x compiles up, left, forward and y
// left, forward, up where retail adds forward, up, left and up, left, forward), so the source already is retail's
// evaluation order; this is not a behaviour difference. Frame 0xd0 versus 0xe0 (retail's unwritten 12 bytes below
// the alpha at [esp+0x78]), spill homes and the one-byte length difference remain as listed below.
// Earlier 231 (was 394): the corner helper adds the origin as one complete vector sum after the transform;
// 1820 bytes (retail 1819), first difference the frame size at +0x2.
// Earlier 394: the four corners are written out (retail has four copies of the scale
// test and calls End out of line; a counted loop, even one the compiler unrolls, scores 465-496). Retail
// 0x32d62-0x333f1 agrees with this source: object index masked 0xffff, opacity > 0.05 test, collision call
// (flags 0x4800001, point object+0x30, vector g_4687bc, NONE, NONE, result), radius = object+0x3c * 0.35, matrix
// from forward object+0x70 / up object+0x7c, corner order (-r,-r) (r,-r) (r,r) (-r,r) (the old draft had x and y
// swapped), vertex colour (1,1,1,alpha) then uv (0,0) (0,1) (1,1) (1,0), and z of vertices 1-3 taken from corner 3
// (0x74; corners 1 and 2 never compute z). D3DDevice::Begin (the member form) lets the compiler inline Begin as
// retail does (call 0x400fa0 plus the push-buffer words); D3DDevice_Begin keeps it out of line (457). Passing the
// corner offset as one reused point3f (retail stores z at [esp+0x10] for corner 0 but multiplies a constant 0.0 in
// the others) took 403 to 394. Still differs: frame 0xd0 vs 0xe0 (retail reserves the alpha spill at 0x78 and 12
// more bytes below the 0x5c-byte collision result) and the registers/spills of the corner math (retail keeps
// scale, forward.i and up.j in xmm5, xmm3, xmm4 across all four corners and radius / -radius in slots 8 and 0xc).
// Tried: loop with ternaries (950), local point3f/sign array loops (465-496), shared x/y/z locals in four
// blocks (456), block-scoped locals (653), pairs loop (906), sum operand permutations per axis (no change),
// 0.0f - radius (464), a negative_radius local (464), -radius in the offsets (405), four separate offset
// locals (396, frame 0xe4), copying collision.point to a local origin (398, real 12-byte copy), a scale local /
// x *= scale / reordered x sum in the helper (no change). Round 38: the vertex colour as the existing color4f (alpha
// first, red/green/blue 1.0) gives retail's 0xe0 frame: retail spills alpha at +0x74 and keeps the colour's other 12
// bytes unwritten (388, first difference +0x30, not kept: -6); left to differ: retail spills radius at its definition
// (+8, -radius at +0xc) and keeps the scale in xmm5. Also tried: alpha before the corners (413), alpha or the
// corners/offset declared at function scope (394), color4f plus radius after the matrix call (391), plus offset
// declared before corners (388), plus the offsets through real_point3d_set (389), plus the corner helper adding the
// origin after the transform (`origin->x + transformed.x`, as retail orders that add; 402, 1786 bytes), plus the
// colour's red/green/blue set before alpha or the colour declared at function scope (388).
// Round 487, all with native color4f: aggregate offset 388; complete transformed-point staging through
// the existing setter 402; both 402; geometry workspace role names 388; names/aggregate offset 388;
// names/point staging 402; all three 402; all three with one consistent scale observation 402. Six-point
// frame lead rejected; original helper and complete body restored. Earlier scores were 460 and 952.
// F341: Native scaled-input point 523/525; uniform rotation subtotals 412/404;
// uniform translated subtotals 412/390 (without/with native colour). Existing
// complete origin sum 231 (kept), with native colour 225 (gain 6, under the bar).
// On that producer, three-use negative radius 398, with native colour 379 (frame 0xe0
// and retail length 1819, but worse than 231; not kept). Full scalar role names 231/225.
// Offset inputs stay staged before stores; the sum accepts its first input in
// place. Corner, offset and origin are distinct at all four calls. Shared helpers unchanged.
// P283: Complete final-coordinate scalar staging with direct stores, native color and
// three-use negative-radius subsets: 388/388/407/389; with the existing point setter:
// 412/403/417/409. All regress; original helper, full body and alias-safe input capture
// restored. Frame/radius/scaled-coordinate homes remain; no one-use scalar staging retained.
// Round 55 (fast): native color4f (alpha, then red/green/blue 1.0f) 225 again: frame 0xe0 agrees, but the
// spill homes stay x 4 / z 8 / radius 0xc / -radius 0x10 where retail has x 4 / radius 8 / -radius 0xc / z 0x10;
// on that: corner helper locals renamed three ways 225/225/225 (names move no slot), offsets passed by value
// 400 (1805 bytes), offset declared first 225, colour declared at function scope 225.
// M360: Four independent corner points at geometry/matrix/producer/entry scopes
// 410/410/410/410; all with native color 377/377/377/377. Original array and body
// restored; native radius/zero homes and the color-sized frame gap remain.
// P311: Existing device member color/position/UV API families on native color, subsets 1/2/4/3/5/6/7:
// P311: 225/382/225/382/225/376/376; all member families with scalar alpha 394.
// P311: NOT kept: six-point forms grow to 1826; native-length 376 regresses. Original APIs, color and full body restored; frame/scalar homes remain.
// M368: Actual object layout/vector displacement identity, without/with native
// color, then complete fade producer graphs: 231/231/231/225/225/225/225/225.
// NOT kept: six-point leads grow to 1826/1819; original source fields, point input,
// scalar alpha and helper restored. Frame and radius/zero homes remain.
// AC517: Existing full/in-place point transforms and scaled-vector transforms, scalar alpha:
// 1016/894/496/796; native color 1016/894/511/813. NOT kept: no exact helper expansion; lengths move away.
// Original corner helper, four-point array, inputs and body restored; native frame/scalar homes remain.
// AC521: Native RGBA vector, four separate/chained channel forms at producer/entry storage:
// eight 232 at 1832/1819; frame 0xe0 recovers but opacity is +0x84 versus +0x78, radius +0xc versus +8.
// NOT kept: wrong homes and regression; original scalar alpha, alias-safe helper and full body restored.
// F368: Existing corner output reference/pointer result/reference result/value result, scalar/native-color frames: 231/231/231/701/225/225/225/707. NOT kept: contracts do not match; color gains six and grows to 1826/1819; value returns grow to 2086/2092. Original helper and complete body restored.
// P315: Current complete origin-sum graph, four real corner-input producer/geometry/entry/immutable lifetimes, without/with native color: 230/230/230/230/225/225/225/225.
// P315: NOT kept: no byte match or supported gain; original full body restored.
// P315: One-point scalar forms fail the gain gate; six-point native-color forms grow to 1826/1819.
// Y578: Ordinary-inline/plain-private corner producer, pointer/reference input contracts,
// scalar/native-color frames: 231/231/231/231/225/225/225/225. NOT kept: no exact expansion;
// color gains six and grows to 1826/1819. Original helper, contracts, alpha and body restored.
// N364: Homogeneous GPU corner outputs, W at helper end/transform or caller
// before/after geometry, without/with native color: 368/368/453/453/382/382/465/465.
// NOT kept: no equality; scalar-color frames recover 0xe0 but grow, native color
// grows the frame again. Original helper, point array, scalar alpha and body restored.
// B440: Existing conditional vector scaler within the original corner expansion,
// input assign/copy and in-place/direct scaling boundaries, scalar/native color:
// 376/628/409/507/381/634/418/516. NOT kept: all regress; native scalar
// homes remain. Original helper, captures, color, geometry and full body restored.
// B442: Existing input scalar capture/declaration orders yxz/yzx/zxy/zyx, scalar color:
// 388/377/396/390; native color 385/367/393/386. NOT kept: no equality, wrong
// scalar homes and regression. Original captures, helper, color and full body restored.
// N381: Complete alpha-first color aggregate, writable/readonly and direct/parameter-owned fade,
// original/three-use negative radius: 225/225/225/225/379/379/379/379. NOT kept: six-point gains
// grow to 1826/1819; native-length forms regress. Original scalar alpha, radius, helper and body restored.
// N383: Full existing collision-output fields with scalar/alpha-first/SDK RGBA/const SDK RGBA
// colors: 231/225/232/232; genuine three-use negative radius: 398/379/375/375.
// NOT kept: ties/regressions or gain six with larger extent; original view, geometry and body restored.
// P347: Complete opacity-gated collision result, bool/byte/short/long, positive/complementary truth:
// 231/231/403/401/402/402/404/403. NOT kept: Two ties and six regressions; all retain frame 0xd0 versus native 0xe0. State-result forms do not match; original complete body restored.
// N385: Complete forward/left/up scalar snapshots in the corner producer, seven subsets,
// then all with complete translation captures on the native color frame: 362/369/362/361/362/221/355/355.
// NOT kept: left/up 221 uses one-use field copies and grows to 1827/1819 bytes; its single
// families regress. All other forms regress. Original scalar alpha, helper and full body restored.
// AC560: Complete planar corner input, pointer/reference/value/scalars, scalar/native-color
// frames: 231/231/449/396/365/365/520/374. NOT kept: no byte equality; native frame/scalar homes remain.
// Original helper, all input captures, full geometry, scalar alpha and body restored.
// P349: Complete scale classification before alias-safe inputs, bool/byte/short/long and complementary truth:
// 446/446/446/446/462/462/458/458. NOT kept: no byte equality; original executable source restored.
// M3 lane 11 r111 (fast; "col" = the vertex colour as a color4f, alpha then red/green/blue 1.0, 225 at 1826 B
// re-measured): col with each corner's offset in its own block 225 (no slot moves); the same blocks without col 230;
// col with a const reference to the collision point passed to the four corners 225; col with the corner rows in
// y, x, z order 219 (picked by score: retail computes x, y, z), z, y, x 387 (1806 B), as forward*x + left*y + up*z
// 225. Compile-only: renaming radius or the corner locals moves no stack slot (radius stays at frame +8), and that
// build keeps up.i/up.j across the corners where LTCG keeps up.i/left.j (retail forward.i/up.j).
// MiniMax S (11-17): no new form, no lower score (231): the corner helper is private to this file and its four
// call sites are all here, so its shape is ours to pick, but the spill homes (x 4 / z 8 / radius 0xc /
// -radius 0x10 against retail's x 4 / radius 8 / -radius 0xc / z 0x10) do not move with the declarations:
// the helper's own locals reordered with z first gives 396, a named negative radius used at the three corner
// sites gives 412 (the body shrinks to 1799 bytes) and the same local only declared gives the same code as
// before; the corner order, the colour aggregate and the helper call order that reach 225 stay six short of
// the keep bar, so the body stands.
// Signed radius interval, rectangular footprint, four immutable offsets and an indexed transform loop:
// scalar alpha 416/250/400/471; four-channel color 384/384/404/475. NOT kept: all regress.
// The frame and the positive-radius, negative-radius and scaled-zero spill homes remain.
// F408: In-place offset in destination, shared scratch, initialized array, and scoped points:
// 385/698/388/698. With the four-channel color: 375/700/383/696.
// NOT kept: all regress, including native-length forms. Frame and corner scalar homes remain.
// Depth bias at each used corner 231 or after all corners 393; named final depths
// at their corners 401 or after all corners 393; with four-channel color,
// 225/390/376/387. NOT kept: the 225 form gains six and grows to 1826 bytes;
// the 0xe0 frame and radius/scaled-zero homes do not recover together.
// DH393: One scale observation, unity-first input assignment, scaled-first input assignment,
// and scaled-first with one scale observation. Scalar alpha 403/412/412/411.
// Four-channel color 369/398/398/397. NOT kept: the color recovers the frame but radius remains
// at +0xc instead of +8. All eight regress. Original helper and body restored.
// Round 623: indexed UV table with completed depths 654, separate UV and depth tables 489,
// one UV and depth binding table 501, automatic UV table with completed depths 618.
// With the four-channel color 640/493/505/605. NOT kept: both scalar-alpha table forms with explicit depth
// owners reach frame 0xe0, but all eight forms shorten and raise the score.
// Round 630: origin first in the vector sum 231, chained first diagonal 403, axes rotation
// plus matrix and collision translations 681, with four-channel color 225/385/687, respectively.
// The two diagonal Y values from X 403, or 385 with four-channel color.
// NOT kept: the 225 form gains six and lengthens to 1826. The 385 forms have the 0xe0 frame
// and retail length but the radius still spills at +0xc instead of +8.
// Round 422: Explicit depth on corners 0 and 3, scalar alpha/four-channel color: in-place depth 231/225,
// conditional depth bias 231/225, separate rotated displacement 390/372,
// aggregate rotated displacement 390/372.
// NOT kept: the six-point color gains grow to 1826 bytes. The 0xe0 frame and radius/zero homes still diverge.
// Separate opacity/collision gates, nested or early-return: 231/231/231/231.
// The same four with color4f: 225/225/225/225. NOT kept: color recovers frame
// 0xe0 and the collision home, but the radius stays at +0xc and the six-point
// gain grows to 1826 bytes against retail 1819.
// Shared vector scale into a distinct offset, direct/cached scale, with scalar
// alpha or color 507/516 and 507/516. Choosing the original or scaled offset
// by assignment or conditional 616/622 and 628/634. NOT kept: the complete
// vector recovers frame 0xe0 with scalar alpha but moves the radius to +0x1c
// and grows to 1934 bytes. Color grows that frame to 0xe4.
// Round 101 (m3 lane 2; control 231). A compile-only build of this file gives our frame and corner math (0xd0, the same
// two shared helper homes, radius and negated radius), so 580 forms were screened there against retail's code ahead of
// the draw call (aligned differences of 230 lines; control 139), then nine in the fast check. Compile-only: the four-
// channel colour with only its alpha stored and 1.0f passed for the rest 124 (frame 0xe0, alpha at retail's +0x78,
// radius at retail's +8, but the second helper home at +0xc and the negated radius at +0x10 where retail has the
// negated radius at +0xc and the scaled zero at +0x10); all four channels stored, or a brace initialiser, 127/127
// (radius +0xc). On those: the offset refilled by three assignments or by real_point3d_set at each corner, the negated
// radius as `-radius` or a named local, the helper's x row written up-first, the origin as the first operand of the
// vector sum: 144 forms, best 124 (the row and operand orders change nothing). The helper's locals read z, y, x give
// retail's leaf order in the first corner (forward.i, up.i, up.j loaded first and forward.i, up.j kept) with x and z
// exchanged in their registers and homes (132 with the scale block y, x, z; 144/145 with z, y, x or x, y, z); the
// other four read orders and all six scale orders with them 136 to 202; the helper's rows stored in another order 161
// to 263; the offset's three first assignments in any order, chained, or y copied from x, with each read and scale
// order (396 forms) best 124. Fast: the existing transform4x3f_apply_point with vector3f_add and the bias at each
// corner, either operand order, 1016/1016 (1196 bytes: not inlined here); alpha-only colour 225 (1826 bytes), all four
// channels 225; alpha-only with the z, y, x read 386, with the scale block y, x, z as well 391, z, y, x as well 230
// (1830 bytes); all four channels with the z, y, x read 386; scalar alpha with the z, y, x read 390. Body unchanged.
// Scale reusable offsets in place as point or vector 396/390, or scale value inputs
// 641/598. With four-channel color 382/382/610/622.
// NOT kept: the forms at retail length still miss frame 0xe0 and raise the score.
// The original corner helper and scalar alpha keep the lower count.
// M3 lane 8 r191 (fast, control 231): the offsets as `-radius` 235 (1819 B: a `0 - radius` subtraction where retail
// multiplies by -1.0); radius renamed size 231 (same code); the four-channel colour read at the call 225 (1826 B,
// re-measured), with `-radius` 226 (1822 B), with radius renamed extent or width 225/225 (radius still spills at +0xc,
// so the slot does not follow the name). Body unchanged.
// M3 lane 8 r196 (fast, control 231): the corner helper's x row in the matched 0x142570's order (up, left, forward) 231
// (same code); that with 0x142570's pointer to the matrix parameter for the scale test 407 (1792 B); the row order with
// the four-channel colour (alpha only) 225 (1826 B, as listed); `real const radius` 231; the offset as a vector3f (the
// helper reading i/j/k) 231 (same code). The colour form gives retail's frame and aggregate order (matrix, corners,
// colour, collision: by name, descending), but the spill cells stay y 4 / z 8 / radius 0xc / -radius 0x10 against
// retail's y 4 / radius 8 / -radius 0xc / z 0x10, with z in XMM1 where retail's is XMM7. Body unchanged.
// Remaining coverage before matrix, after matrix, after first corner, or before last corner
// 409/407/401/399 with scalar alpha and 411/406/396/396 with four-channel color.
// NOT kept: all regress and grow. The last color form recovers frame 0xe0 but runs to 1841 bytes.
// M3 lane 4 r227 (fast with 0x141710/0x1697c0, both MATCH; control 231; a/s/r = LCS-aligned unmatched lines exact /
// stack-masked / register-masked too, control 165/160/61). Retail keeps the offset's y of corners 2 and 3 in that
// corner's own output y (stores the radius to +0x64 and +0x70, then reads +0x70 back in corner 3's scaled path),
// which suggests each corner's offset written into its output and transformed in place. Each corner's offset written
// into its corner and the helper called in place: with the four-channel colour 369 (1824 B, first difference +0x38,
// frame 0xe0; 173/176/119), with its rgb passed as constants 369 (same code), with the scalar alpha 375 (1819 B,
// 164/161/65), that with each offset's y stored before its x 377 (166/163/65); all four offsets written first, then
// the four in-place calls, with the colour 388 (1826 B). The compiler forwards every in-place store (no y store
// survives), so this is not retail's shape either. Body unchanged.
// Two offset lifetimes split after corner 0, 1 or 2, or reused by diagonal:
// scalar alpha 231/231/231/231; initialized four-channel color 400/400/400/400.
// NOT kept: color restores the 0xe0 frame but moves away from retail length to
// 1794 bytes and leaves the radius at +0xc. The scalar-alpha bodies compile identically.
// Depth within the scale branch 242/224, lateral and depth within the branch 412/399,
// reflected corner offsets 441/444, and shared corner translation pass 792/798
// (scalar alpha/four-channel color). NOT kept: the 224 form recovers frame 0xe0 and
// 1819 bytes but gains seven. The positive-radius, negative-radius and zero homes differ.
// M3 lane 5 r223 (H2M_FAST, control 231, aligned 209 / register-masked 192). The corner helper's rows as two
// steps, a named first pair in each row (`real cx = forward.i * x + up.i * z;` and the y and z rows' first two
// terms) then the third term and the position: 383 (1802 B, aligned 185); the three products named and the
// position added after 372 at retail's 1819 B (aligned 218 / 176); each row as one store and two `+=` 614 (2030 B);
// the rows with the grouping parenthesised 383 (same code as the named pair). With the four-channel colour (alpha
// stored, 1.0f passed for the rest; 225 alone at 1826 B, re-measured): the named products 358 at 1819 B (with all
// four channels stored 358); the named pair 367 (1808 B, aligned 175 / register-masked 66: each row's adds then
// run in retail's order but for x, where ours adds up.i * z into forward.i * x's register); on that, corners 2
// and 3 (or 3 alone) written into their own outputs and transformed in place 367 (same code: the stores are
// forwarded). Lead for the next round: the named pair with the colour, 11 bytes short (retail's two surviving
// offset-y stores into corners 2 and 3). Body unchanged.
// The positive radius, negative radius and scaled depth still need their retail spill offsets.
// Aggregate offset before basis 240. Early zero depth 231.
// Early point setter 385. Early origin initializer 231.
// Early aggregate and four channels 233. Early zero depth and four channels 225.
// Early setter and four channels 379. Early origin and four channels 225.
// NOT kept: All four color forms recover 0xe0. The early depth and origin-initialized color forms gain six
// points and grow to 1826 bytes. Their first differing spill still keeps the radius at +0xc. The scalar
// versions of those two forms tie the baseline. Both collision and basis controls stay matched.
// M3 lane 6 r241 (h2m fast with 0x1697c0/0x141710, both matched; control 231, aligned 165; "col" = the colour as a
// color4f with alpha stored and 1.0f passed for the rest, 225 at 1826 B re-measured). Reading: retail adds each corner
// row as up, left, forward in all four corners (the matched 0x142570's order expanded in place, then the origin add and
// the bias); our helper's x row sorts forward first because x's leaves are numbered after z's. Diagnostic (reverted):
// `inline` on 0x142570's definition and that call with vector3f_add and the bias at each corner, with col, 355 (1822 B,
// aligned 147: frame 0xe0 and radius at +8 as retail); its body as a file-local helper 355 (same code); each offset
// written into its corner and transformed in place 355 (member stores x y z / x z y / z x y 355, y x z / y z x / z y x
// 357; positives as `radius * 1.0f` 355; through real_point3d_set 355; one offset reused through the setter 362); all
// four offsets first 383 (frame 0xdc); the origin added per component 438 (1764 B, aligned 138), as `origin + corner`
// 438; the corner helper calling the transform helper, then vector3f_add and the bias 375 (1797 B, aligned 143; either
// add order 375): only this nested form gives retail's add order in all 12 rows, leaving the XMM choice per corner and
// retail's two kept offset-y stores (+0x64, +0x70) of corners 2 and 3. All copy 0x142570's body or need `inline` on it
// (match-only / held). Body unchanged.
// M3 lane 11 r166 (h2m fast with 0x1697c0/0x141710/0x142570, all MATCH unless noted; control 231). Retail's four corner
// blocks read as four expansions of the existing 0x142570 (its scale test and rows, then the origin add and the bias), which
// this build's link never expands here: the four corners as a counted loop over the real call with a sign table 1112
// (1099 B: neither unrolled nor expanded); the offsets written into the corners first and a loop of in-place calls 1072
// (1136 B); the four written calls under the nested test, under two early returns, or under one negated return 1016 each
// (1196 B, lane 2 r101's form re-measured: the guard shape does not move the expansion); diagnostic (two files, restored):
// 0x142570 without its parameter-address read 1042 (1169 B, still a call; 0x142570 itself 54). Reading of retail's kept
// stores at +0x64 / +0x70: they are the homes of the expansion's y in corners 2 and 3 (stored where y is defined, read
// back as a memory operand in corner 3's scaled arm, where the product is formed in y's own register), and those homes
// coincide with the two corners' own y cells. Body unchanged.
// Share the positive extent from the second offset, copy the first negative lateral offset,
// initialize the full first offset, or retain its negative extent: 231/403/231/398.
// The same forms with four-channel color 225/385/225/379.
// NOT kept: the 225 forms regain frame 0xe0, gain six and grow to 1826 bytes.
// The other color forms reach 1819 bytes and raise the score.
// Separate axis selections from a shared test, per-axis tests, guarded captures, or a complete
// point snapshot: 392/518/231/717. With four-channel color: 390/516/225/723.
// NOT kept: 225 gains six, grows to 1826 bytes, and leaves the radius spill at +0xc.
// Indexed in-place scaling or a separate indexed scaled result, with the scale read at each use
// or retained: scalar alpha 628/507/628/507, four-channel color 634/516/634/516.
// NOT kept: the separate scaled result reaches frame 0xe0 with scalar alpha, but the indexed
// variants lengthen the corner expansion and leave the radius spill at +0xc.
// Split output into successive edges, depth ends and middle pair, a separate first corner, or
// a separate last corner: scalar alpha 390/244/234/386; four-channel color 387/228/218/371.
// NOT kept: 218 recovers frame 0xe0 but grows to 1829 bytes and leaves the radius at +0xc.
// The retail-length color form raises the score.
// Planar middle outputs and separate endpoint depth, scalar alpha/color:
// scalar pointers 438/418, scalar references 438/418, typed point
// receivers 1139/1136, returned planar points 493/493.
// NOT kept: none recovers the radius/depth homes and extent.
// M3 lane 15 r147 (fast screen with 0x1697c0, 0x141710 and 0x142570, all MATCH; control 231): the corner helper's x row
// in 0x142570's up, left, forward order 231, the origin added first 231, both 231; the opacity and collision tests as
// one negated early return 231; the radius renamed to sort ahead of the helper's scaled x, y and z 231 (all the same
// code: the spill homes ignore names). Body unchanged.
// M3 lane 3 r313 (fast, control 231): the opacity test as `0.05f < opacity` 231; the alpha as `opacity * (1.0f - t)` 231;
// the radius as `0.35f * radius field` 231; the offset's z set first 231; the offset declared ahead of the corners 231
// (same code each).
// @retail 0x32ce0
void function_32ce0(long object_index, real opacity)
{
    s_collision_result_1697c0 collision;
    byte const *object = ((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    collision.unknown24 = NONE;
    if (opacity > 0.05f && function_1697c0(0x4800001, (point3f const *)(object + 0x30),
            g_4687bc, NONE, NONE, &collision))
    {
        real radius = *(real const *)(object + 0x3c) * 0.35f;
        transform4x3f matrix;
        matrix4x3_from_forward_and_up(&matrix, (vector3f const *)(object + 0x70),
            (vector3f const *)(object + 0x7c));
        point3f corners[4];
        point3f offset;
        offset.x = radius * -1.0f;
        offset.y = radius * -1.0f;
        offset.z = 0.0f;
        shadow_corner(&matrix, &offset, &collision.point, &corners[0]);
        offset.x = radius;
        shadow_corner(&matrix, &offset, &collision.point, &corners[1]);
        offset.y = radius;
        shadow_corner(&matrix, &offset, &collision.point, &corners[2]);
        offset.x = radius * -1.0f;
        shadow_corner(&matrix, &offset, &collision.point, &corners[3]);
        real alpha = (1.0f - collision.t) * opacity;
        D3DDevice::Begin(D3DPT_QUADLIST);
        D3DDevice_SetVertexData4f(9, 1.0f, 1.0f, 1.0f, alpha);
        D3DDevice_SetVertexData2f(3, 0.0f, 0.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_POSITION, corners[0].x, corners[0].y, corners[0].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 0.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_POSITION, corners[1].x, corners[1].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_POSITION, corners[2].x, corners[2].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 0.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_POSITION, corners[3].x, corners[3].y, corners[3].z, 1.0f);
        D3DDevice::End();
    }
}

// @retail 0x4a1e0
void function_4a1e0(s_view_source *source, s_view_camera *camera, long resource_index,
    s_view_flags *flags, real unused_scale, bool enabled, real scale)
{
    /* the unused scale stays on the stack */
    real *unused_scale_reference = &unused_scale;
    byte *resource;
    if (resource_index == NONE)
        resource = 0;
    else
        resource = g_4e030c->data + (resource_index & 0xffff) * 0x110;
    byte *context = (byte *)g_547f88.context;
    g_4858bc = true;
    g_4858c4 = (byte *)flags;
    if (resource && *(short *)(resource + 0x54) != NONE)
        g_4858cc = *(long *)(resource + 0x4c);
    else
        g_4858cc = NONE;
    byte *input = (byte *)source;
    g_485944 = *(vector3f *)(input + 0x50);
    g_485950 = *(vector3f *)(input + 0x5c);
    g_48595c = *(real *)(input + 0x68);
    g_485960 = 0.0f;
    g_4858c8 = *(short *)camera;
    if (resource)
    {
        g_485a30 = *(real *)(input + 0x74);
        g_485a38 = *(real *)(input + 0x7c) * scale;
        g_485a3c = *(real *)(input + 0x80) > *(real *)(input + 0x7c)
            ? *(real *)(input + 0x7c) : *(real *)(input + 0x80);
        g_485a34 = *(real *)(input + 0x78);
        g_485a40 = *(real *)(input + 0x84);
        g_485944.i *= g_485a38; g_485944.j *= g_485a38; g_485944.k *= g_485a38;
        g_485950.i *= g_485a3c; g_485950.j *= g_485a3c; g_485950.k *= g_485a3c;
    }
    else
    {
        g_485a30 = g_485a34 = g_485a38 = g_485a3c = 1.0f;
        g_485a40 = 0.0f;
    }
    /* retail passes the light's frame in eax (read above) and its shape in ecx
       (the short above); 0x497a0 takes the shape as its source (its kind at +0,
       its extent at +0x38) and the frame as its camera (forward +0x28, up
       +0x34) */
    function_0497a0((s_view_source *)camera, (s_view_result *)g_48597c, (s_view_camera *)source, flags,
        &g_485944, &g_485950, g_48595c, &g_485a2c, (vector4f *)g_485a0c, (vector4f *)(g_485a0c + 4));
    s_bounds3d bounds;
    if (resource)
    {
        real const *clip_rect = (real const *)(context + 0xa4);
        if (function_1650a0((s_view const *)(context + 4),
                (point3f const *)(resource + 0x28), &bounds, *(real *)(resource + 0x34))
            && clip_rect[1] != clip_rect[0] && clip_rect[3] != clip_rect[2]
            && clip_rect[1] >= bounds.x0 && bounds.x1 >= clip_rect[0]
            && clip_rect[3] >= bounds.y0 && bounds.y1 >= clip_rect[2])
        {
            real left = bounds.x0, right = bounds.x1;
            real top = 0.0f - bounds.y1, bottom = 0.0f - bounds.y0;
            if (clip_rect[0] > left) left = clip_rect[0]; else if (left > clip_rect[1]) left = clip_rect[1];
            if (clip_rect[0] > right) right = clip_rect[0]; else if (right > clip_rect[1]) right = clip_rect[1];
            if (clip_rect[2] > top) top = clip_rect[2]; else if (top > clip_rect[3]) top = clip_rect[3];
            if (clip_rect[2] > bottom) bottom = clip_rect[2]; else if (bottom > clip_rect[3]) bottom = clip_rect[3];
            short screen_left = g_485a8a.a.v1, screen_right = g_485a8a.a.v3;
            bounds.x0 = (screen_right - screen_left) * (left - clip_rect[0]) / (clip_rect[1] - clip_rect[0]) + screen_left;
            bounds.x1 = (screen_right - screen_left) * (right - clip_rect[0]) / (clip_rect[1] - clip_rect[0]) + screen_left;
            short screen_top = g_485a8a.a.v0, screen_bottom = g_485a8a.a.v2;
            bounds.y0 = (screen_bottom - screen_top) * (top - clip_rect[2]) / (clip_rect[3] - clip_rect[2]) + screen_top;
            bounds.y1 = (screen_bottom - screen_top) * (bottom - clip_rect[2]) / (clip_rect[3] - clip_rect[2]) + screen_top;
            memcpy(g_485964, &bounds, sizeof(g_485964));
        }
        else
        {
            g_485964[0] = g_485a8a.a.v1;
            g_485964[1] = g_485a8a.a.v3;
            g_485964[2] = g_485a8a.a.v0;
            g_485964[3] = g_485a8a.a.v2;
            g_485964[4] = 0.0f - *(real *)(context + 0x8c);
            g_485964[5] = 0.0f - *(real *)(context + 0x74);
        }
        g_4858d0 = true;
        memcpy(g_485934, resource + 0x18, sizeof(g_485934));
    }
    else
    {
        g_485964[0] = g_485a8a.a.v1;
        g_485964[1] = g_485a8a.a.v3;
        g_485964[2] = g_485a8a.a.v0;
        g_485964[3] = g_485a8a.a.v2;
        g_485964[4] = 0.0f - *(real *)(context + 0x8c);
        g_485964[5] = 0.0f - *(real *)(context + 0x74);
        g_4858d0 = false;
    }
    g_485a44 = enabled;
    function_3a550();
}


extern double g_4858a0;
struct s_masked_list;
bool __stdcall function_38450(byte *state, byte const *definition, real remaining);
bool __stdcall function_37a60(void *state);
bool __stdcall function_391f0(void *state);

// score 251 (was 251): frame 0x74 versus 0x70, early zero and portal/fog loop homes remain. BLOCKED-BY 0x38450 and
// 0x391f0 for a byte match (their argument registers follow their own unmatched bodies, see the end of this note).
// MiniMax H (11-06): no new form, no lower score.
// The frame is 0x74 against retail's 0x70 and the value saved right after the index test sits at [esp+0x30]
// where retail keeps it at [esp+0x2c]; we zero a register before the test where retail zeroes one after it,
// and the portal and fog loop bodies and their homes still differ.
// Earlier: frame 0x74 versus 0x70, early zero and portal/fog loop homes remain.
// Earlier short-fog gain 272 -> 251; 257 with a volatile counter was not retained.
// MiniMax R (11-01): no new form, no lower score.
// earlier 257 with a volatile counter; frame 0x74 vs 0x70, early zero and loop homes differ.
// Earlier 272 (was 272; earlier 257 with a volatile counter): frame 0x74 vs 0x70, early zero and loop homes differ.
// Earlier note: 272 (was 272; earlier 257 with a volatile counter): frame 0x74 vs 0x70, early zero and loop homes differ.
// Earlier: no volatile any more. The portal loop is now `while (remaining-- > 0)` with
// `*portals++` and the two sides are a real 2-iteration loop (`for (side...) if (sides[side] == cluster) break;`), which compiles
// to retail's loop (movsx ebp, word [portal+eax*2]; inc eax; cmp eax, 2; jl). A plain `for (; remaining > 0; --remaining)` counter
// is unrolled four times by the compiler (1535 bytes, 956); retail keeps the count in memory ([esp+0x10], dec; jne) and does not
// unroll, which the post-decrement form also gives without volatile. What still differs: retail compares the portal
// side with `je found` straight into the plane code, ours re-tests `side == 2` after the loop (a `do { if (..) goto found; } while
// (++side < 2); continue; found:` form removes that but scores 274); retail reads the current cluster as a long (movsx ebx, word
// [0x547f90]; cmp ebx, -1) and reloads the global inside the fog loop, ours compares a cached copy (`long current_cluster`
// makes the front match retail's order but scores 299 with the later spills); the camera x/y/z and 32767.0f stay in xmm3-xmm6 across
// the portal loop, and retail re-makes zero with xorps inside each block (here one zero is shared from the function start); the
// outer loop's index lives in edi in retail (spilled around the portal loop) and in ebx here.
// Tried: the 2-iteration for loop with the volatile counter (299), with a plain counter (273); a plain counter with the
// unrolled tests (956, the loop is unrolled and grows to 1535 bytes); `for (k < count) portals[k]` (972, unrolled);
// a while loop with a side index (295); camera_z read before camera_y (no change).
// Round 435: native camera 321, signed long count 268, cluster/camera 324, cluster/count 294,
// camera/count 325, all three 343; all plus shared plane helper 343. Early uninitialized step
// and step/distance declarations with the long count 268/268. All restored; four-point lead rejected.
// Round 503: native signed fog word 251; native flag initialization 299; both 291;
// fog word with byte flags 251; byte flag initialization 299; all three 291;
// fog word plus long count 300; all three plus long count 301. Keep only the fog word;
// retail tests AX before widening the index. Frame 0x74 versus 0x70 and early zero remain.
// Round 524: native short portal side, array declarations reversed, both 297/251/297;
// native short fog-pass ordinal, with side/array/both 323/325/323/325; all with short
// visible ordinal 274. Complete original body restored; narrow-side/fog homes do not recover frame.
// Round 469: Direct global step/update-success loop condition/render-success loop condition subsets 1-7, then complete graph with short visible ordinal: 249/277/298/265/276/296/318/316. Two-point direct-step lead rejected; original observations and loops restored.
// F323: Frame 0x74 versus 0x70, early zero and array/loop homes remain; original observations restored.
// Consistent structure/visibility/live count-table observations, seven subsets,
// 284/266/306/278/281/306/281; all with the bounded native short ordinal 288.
// Original complete body restored; stacked numeric headers merged.
// Round 527: Two-use previous timestamp/native short frame/single-exit admission subsets
// 1/2/4/3/5/6/7 give 294/262/251/290/294/262/290; all with double previous time 324.
// Original time observations, frame carrier, gates and complete body restored; extra frame dword and zero/loop homes remain.
// AC480: Natural portal-cursor for increment with the signed post-decrement condition; conditional
// absolute / array-home names / independent phase ordinal combinations: 266/266/266/266/266/266/266/266. Original
// loop and complete body restored; extra frame dword and zero/portal/fog homes remain.
// Round 530: portal-phase current-cluster workspace, existing native material fields, scan-time
// minimum, current/fields, current/minimum, fields/minimum, all three and native ordinal role names:
// 273/251/251/273/273/251/273/273. Original body restored; frame 0x74, early zero and loop homes remain.
// DL247: Standard count/end-point distance fill with CRT zero, aggregate false,
// count false or end-point false fill: 295/295/296/296/295/279/296/296.
// All restored: shorter than retail, no frame recovery, and a reduced control regresses.
// Round 533: One genuine element ordinal shared by update pair/render pair/corresponding
// first/corresponding second/opposite first/opposite second/all phases/all with ordinal role name:
// 251 each. Original independent ordinals and body restored; frame 0x74 versus 0x70,
// early zero and portal/fog homes remain.
// DH301: Clock at each use, twice-used native elapsed result and distance-array reads
// at each consumer, subsets 1/2/4/3/5/6/7, then immutable elapsed on the complete graph:
// 310/251/277/295/288/277/275/275. All tie or regress; original complete body, reads,
// publications and loops restored. Frame 0x74 versus 0x70 and zero/portal/fog homes remain.
// M323: Existing native state-group fields with bool/byte/short/long outcomes 251/251/281/286; explicit conditional assignments 297/297/263/281. The 263 form reaches 1014 bytes but regresses; original body and phase observations restored.
// Round 64 (fast, aligned counts from 132): long current cluster at the guard 302 (frame 0x6c, aligned 129); camera read at the plane
// test 251 (aligned 131); fog word widened into a long after its NONE test 251 (132); current+camera 302; current+fog 302;
// camera+fog 251; all three 284 at 1020 bytes (aligned 127, frame 0x6c). Retail's slots: remaining 0x10, ordinal 0x14, structure 0x18.
// DH337: Native cluster/portal/plane field subsets and complete read-only geometry
// 251/251/251/251/251/251/251/251. Original views/body restored; frame 0x74
// versus 0x70, early zero and loop homes remain.
// Y569: Positive visible-fog, portal-side and selected-tag payload scopes,
// seven subsets, then positive portal-key scope on the complete graph:
// 251/251/251/251/251/251/251/251. NOT kept: no qualifying native repair; complete original body restored;
// extra frame word, early zero and portal/fog homes remain.
// F369: Whole timestamp/elapsed-result joins (explicit/default/conditional/default plus
// common clamp), then each with direct clock observations: 301/315/324/315/295/326/327/326.
// All restored: no match or qualifying gain; original publications and every loop remain.
// F371: Genuine byte-offset carriers in each update/render phase, then
// update pair/render pair/large-record pair/all phases: 250/251/251/251/250/251/250/250.
// NOT kept: the complete graph gains only one and retains frame 0x74;
// native zero/portal/fog homes remain. Original traversals and body restored.
// P318: One phase-success accumulator for update/render in bool/byte/short/long,
// selected-tag then function-entry declaration: 251/251/281/286/251/251/281/286.
// All retain frame 0x74 versus 0x70; no gain. Original independent flags and complete
// body restored; every calibrated control survives.
// P324: Independent portal plane/value, cursor/count/key and side/minimum
// uninitialized entry declarations, masks 1-7, then complete visible-scan
// declarations: 251 each. All initializers stay at their original statements.
// Original body restored; extra frame word, early zero and portal/fog homes remain.
// Y593: Found-side owns plane/minimum payload, for/while side traversal in long/dword/short/word:
// 257/257/289/289/257/257/289/289. NOT kept: all regress; native frame and zero/portal homes remain.
// Original complete body and every phase call restored.
// F387: Complemented update/render failure predicates in bool (update/render/both), byte
// (update/render/both), then both in short/long: 252/253/254/252/253/254/288/277.
// NOT kept: all regress; frame 0x74 and early zero/portal homes remain. Original body restored.
// F390: Existing visible-list native header, mutable/read-only receiver, signed short/long count,
// snapshot/per-use index base: 251/276/300/306/251/276/300/306. NOT kept: no gain; frame
// stays 0x74 versus 0x70, wide counts grow to 1036/1014. Original complete body and header
// contract restored; early zero and visible/portal/fog homes remain, all callee controls stable.
// F390: Actual two-side standard find/count/search producers, direct mutable and const table
// views, then find/search iterator results: 353/352/394/353/352/394/353/394. NOT kept: frames
// 0x84/0x8c versus native 0x70 and extents 1087/1119 versus 1014; original side loop, whole body
// and include set restored. Early zero and portal/fog homes remain.
// F392: Actual distance aggregate initialization with CRT/aggregate flags and independent constant-array entry lifetimes: 302/332/319/349/305/340/305/340.
// NOT kept: no exact match. Original arrays, initialization loop, CRT zero, reads and full body restored; native zero and frame 0x70 remain unresolved.
// J259: Failure-only positive outcomes in four phases, update/render pairs, all and nested all: 276/261/301/297/263/263/262/262.
// NOT kept: no eligible native repair; opening executable source restored. Complete forms reach 1014 bytes but retain frame 0x74 versus 0x70; original outcomes, live bounds and all phase loops restored. Native zero/portal/fog homes remain; downstream interfaces untouched.
// J261: Twice-used first-frame predicate bool/byte/dword 298/298/315; two-side ordinal byte/signed-byte
// 297/297; bounded fog-pass ordinal byte/word/signed-byte 323/323/323. Every form regresses, grows the body and
// retains frame 0x74. All matched controls preserved; original observations and domains restored.
// NOT kept: no eligible native repair; opening executable source restored. Native frame 0x70, early zero and
// portal/fog homes remain; narrowed domains did not repair them.
// b6Q (11-06): score 251 (was 251, fast build): second statement-by-statement walk, no behaviour difference, no lower score.
// Walked 0x36b50-0x36f45 block by block: timestamp step (the 0.0 test at 0x36b94 is made on the unrounded difference
// that 0x36b88 stores as a float, as ours compiles), the two guards and the 0x46fffe00 fill, the visible-cluster loop
// (fog word, NONE tests, the selected store, the -1.0 for the current cluster), the portal and side walk, the plane
// distance (y, z, x order, 0.0 minus the sum, absolute value, minimum) and the four phase loops with their
// arguments all agree. What differs is allocation: retail rereads the current cluster word at 0x36ce2 where we keep
// it in a slot (that slot is the extra frame dword), reads g_509418 at 0x36e49 inside the update guard into ebp
// where we load it before the guard into a float slot, and keeps the fog index in a slot where we use ebp.
// Tried in this walk: the selected store moved after the fog branch (265), the side loop with the plane
// block inside it and a break (257, drops retail's jmp at 0x36d45 but regresses). Retail passes 0x38450's state in ebx
// (0x36e84) and pushes 0x391f0's (0x36efa); ours pushes the first and passes the second in eax. That follows the
// unmatched callee bodies, not this function.
// M3 lane 11 r162 (H2M_FAST; control 251, difflib-aligned 173, register-masked 131). Reading: our frame's bottom dword
// (+0x10) is never used; the current-cluster word is kept in a slot where retail re-reads the global. A long local for the
// guard and the row pointer with the loop compare still reading the global (a mixed read, match-only) gives retail's
// `movsx ebx; cmp ebx, -1` front and its in-loop re-read: 302 (frame 0x6c, masked 115; fog then keeps EBP instead of
// retail's ECX plus slot). Forms: the step passed as the global at the call 249 (masked 131); the plane through the
// existing plane_distance_to_point with the camera position global, no camera locals, 251 (masked 124, retail's y, z, x
// product order; with `-` for `0.0f -` the same code); that with a `point3f` camera copy 320 (frame 0x7c); the side search
// with a found flag and break 257 (retail's `jmp` exit, no `side == 2` test); the portal walk as `for (long remaining =
// count; remaining > 0; --remaining, ++portals)` with the step global 256 (retail's load/dec/store latch); `portals[k]`
// with a k < count for 290 (not strength-reduced, bound re-read); the visible indices read inside the loop 263 with the
// step global; a long count 293; plane helper + step global 249; + the for walk 256; + the in-loop indices 289; all five
// 257; plane helper + step + in-loop indices 270. With the long-local mix: + plane helper 302 (masked 104); + step 263; +
// found flag 275; all four 263 (masked 100); + the for walk 277 (masked 98); + in-loop indices 295 (masked 95, unmasked
// 146). The zero stays one register hoisted to the entry in every form (retail makes it per block, xorps inside the
// portal loop); without the timestamp block it is hoisted ahead of loop 1 (diagnostic 334).
// The cached current cluster and the frame 0x74 versus 0x70 remain. Short camera-cluster snapshot 269; compare cluster records 272; direct visible ordinal 275; visible-list end pointer 266; portal end pointer 307.
// Portal distance before the current-cluster case 288; common fog-distance candidate 278; conditional minimum update 251.
// NOT kept: the record identity and visible cursor shrink the frame to 0x6c; the common candidate shortens to 1005 bytes. None beats the opening score.
// @retail 0x36b50
void function_36b50()
{
    long frame = g_485600;
    if (frame == 0)
    {
        double now = g_4858a0;
        if (g_50941c != 0.0f)
        {
            g_509418 = (real)(now - g_50941c);
            if (g_509418 < 0.0f) g_509418 = 0.0f;
        }
        else g_509418 = 0.0f;
        g_50941c = (real)now;
        function_398d0(g_509418);
    }
    if (g_509420) return;
    byte *structure = (byte *)g_4e0348;
    real distance[16];
    bool selected[16];
    memset(selected, 0, sizeof(selected));
    for (long i = 0; i < 16; ++i) distance[i] = 32767.0f;
    if (*(long *)(structure + 0x84) <= 0 || g_547f88.location.cluster_index == NONE) return;
    byte *clusters = *(byte **)(structure + 0xa0);
    byte *current = clusters + g_547f88.location.cluster_index * 0xb0;
    byte *visible = (byte *)g_547f94;
    short count = *(short *)(visible + 4);
    long *indices = *(long **)(visible + 0xc);
    real camera_x = g_485618.position.x;
    real camera_y = g_485618.position.y;
    real camera_z = g_485618.position.z;
    for (long i = 0; i < count; ++i)
    {
        long cluster = indices[(short)i];
        short fog = *(short *)(clusters + cluster * 0xb0 + 0x76);
        if (fog == NONE || *(long *)(*(byte **)(structure + 0x88) + fog * 0x88 + 0x24) == NONE) continue;
        selected[fog] = true;
        if (cluster == g_547f88.location.cluster_index) distance[fog] = -1.0f;
        else
        {
            real minimum = 32767.0f;
            short *portals = *(short **)(current + 0x90);
            long remaining = *(long *)(current + 0x8c);
            while (remaining-- > 0)
            {
                short portal_index = *portals++;
                if (portal_index == NONE) continue;
                byte *portal = *(byte **)(structure + 0x60) + portal_index * 0x24;
                long side;
                for (side = 0; side < 2; ++side)
                    if (((short *)portal)[side] == cluster) break;
                if (side == 2) continue;
                real *plane = (real *)(*(byte **)(*(byte **)(structure + 0x18) + 0xc) + *(long *)(portal + 4) * 16);
                real value = 0.0f - (camera_y * plane[1] + camera_z * plane[2] + camera_x * plane[0] - plane[3]);
                if (0.0f > value) value = 0.0f - value;
                if (minimum > value) minimum = value;
            }
            if (distance[fog] > minimum) distance[fog] = minimum;
        }
    }
    for (long i = 0; i < 16; ++i)
    {
        if (!selected[i]) continue;
        long tag = *(long *)(*(byte **)(structure + 0x88) + i * 0x88 + 0x24);
        if (tag == NONE) continue;
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (frame == 0) function_4b3d0((s_vector_perturbation *)(definition + 0x10));
        real fog_distance = distance[i];
        real step = g_509418;
        bool updated = true;
        for (long j = 0; j < *(long *)(definition + 8); ++j)
        {
            byte *state = *(byte **)(definition + 0xc) + j * 0x3a8;
            updated = updated && function_37260(state, step, definition);
        }
        for (long j = 0; j < *(long *)definition; ++j)
            updated = updated && function_38450(*(byte **)(definition + 4) + j * 0x8c, definition, fog_distance);
        bool rendered = true;
        for (long j = 0; j < *(long *)(definition + 8); ++j)
            rendered = rendered && function_37a60(*(byte **)(definition + 0xc) + j * 0x3a8);
        for (long j = 0; j < *(long *)definition; ++j)
            rendered = rendered && function_391f0(*(byte **)(definition + 4) + j * 0x8c);
    }
}



// Disabled: render-clear 14b60 still has a protected bool-only interface; filter callbacks also need shared state addresses.
#if 0
PRIVATE inline real transition_time_r18()
{
    return g_510c54 && g_510c54->active ? g_510c54->game_time * g_510c54->rate : 0.0f;
}
// Retail 0x289e0

#endif



__declspec(noinline) bool function_0226d0();
__declspec(noinline) byte function_335c0();
void function_01ddd0(long index, dword width, dword height);
bool __stdcall function_32350(long index, real alpha);
extern byte g_485a46;
byte g_509413;

// @retail 0x335e0
void __stdcall function_335e0(long mode)
{
    if (!function_0226d0() || !function_335c0()) return;
    s_cached_input records[32];
    g_485a46 = true;
    function_01ddd0(9, 0x100, 0x100);
    g_509413 = true;
    long count = function_33400((byte const *)g_547f88.lists[2], records, mode);
    for (long i = 0; i < count; ++i)
        function_32350(records[i].data[2], *(real *)(records[i].data + 3));
    g_509413 = 0;
    g_485a46 = 0;
}



// Disabled: these passes publish persistent addresses of shared record-source groups; clear 14b60 also retains its incomplete protected interface.
#if 0
struct s_record_source;

extern s_record_sources *g_467138;
extern long g_467130, g_467134;
extern byte g_485a75, g_485a76;
extern bool g_4ba022, g_4ba023;
void __stdcall function_352e0(long target, bool multiple);
void __stdcall function_44c90(long group, dword selection, long level, dword mask, dword value, dword fallback_mask, dword fallback_value, bool force);

PRIVATE inline void draw_source_pass_r18(long group, long mode, bool extra_enabled, bool depth, bool normal_cull, long target, bool filter)
{
    bool enabled = (g_4c1a48[group].flags & (1 << mode)) && extra_enabled;
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = group;
        g_467138 = &g_4c1a48[group];
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (mode == 1 && !g_485a75 && !g_485a76) function_14b60(1, 0, 0.0f, 0);
        function_15680(normal_cull && (g_485898 < 4 || g_485898 > 7) ? 1 : 0);
        if (filter) function_496f0();
    }
    else g_467130 = 0;
    function_15370(depth ? 1 : 0);
    if (enabled) { function_44550(); function_445d0(); }
}
PRIVATE inline void draw_configured_pass_r18(long mode)
{
    if (function_44370(mode)) { function_44550(); function_445d0(); }
}
PRIVATE inline bool fog_pass_enabled_r18()
{
    real maximum = g_4b9f78 > g_4b9f7c ? g_4b9f78 : g_4b9f7c;
    return g_4b9f5c != NONE && !g_4b9f88 && maximum > 0.0f;
}

// Retail 0x423c0


// Retail 0x41cc0




// Retail 0x31170

#endif





// Disabled: shadow rendering requires persistent shared light-frame/definition storage and visibility/global-source addresses.
#if 0
PRIVATE inline vector3f shadow_cross_r18(vector3f const &a, vector3f const &b)
{
    vector3f c;
    c.i = a.j * b.k - a.k * b.j;
    c.j = a.k * b.i - a.i * b.k;
    c.k = a.i * b.j - a.j * b.i;
    return c;
}

// Retail 0x32350

#endif



// Disabled: render orchestration takes persistent shared pool/source addresses and calls the blocked clear/frame bodies.
#if 0
// Retail 0x2bcd0

#endif


extern s_record_pool *g_509434;
extern long g_4ba050;
long __stdcall function_3ddd0(long object_index);

// The level byte cached for the current view: the read counterpart of function_4c640.
PRIVATE inline signed char cached_level_4c450(long object_index)
{
    signed char result = -1;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        byte *slot = NULL;
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
            slot = g_509434->data + (cache & 0xffff) * 0x100 + 0xbc;
        result = (signed char)slot[g_4ba050];
    }
    return result;
}

// Match-only form (kept for the byte match): the previous level through the getter and the named distances,
// validity and fraction parts; with fewer named locals ahead of the cache read, the indexed byte load's base and
// index registers swap (score 1).
// @retail 0x4c450
long function_4c450(long object_index, real distance, real const *definition, real *fraction)
{
    (void)&object_index;
    real defaults[6] = { 10.0f, 8.0f, 6.0f, 4.0f, 2.0f, 0.0f };
    real thresholds[6];
    real upper;
    *fraction = 0.9999f;
    real const *distances = definition + 3;
    bool valid = definition && distances[0] > 0.0f && distances[0] >= distances[1]
        && distances[1] >= distances[2] && distances[2] >= distances[3]
        && distances[3] >= distances[4] && distances[5] == 0.0f;
    if (valid)
    {
        memcpy(thresholds, distances, sizeof(thresholds));
        upper = definition[0];
    }
    else
    {
        memcpy(thresholds, defaults, sizeof(thresholds));
        upper = 0.0f;
    }
    long level = 0;
    while (level < 6)
    {
        real lower = thresholds[level];
        if (distance > lower || lower == 0.0f)
        {
            if (level == 0)
            {
                if (!(upper > lower))
                {
                    *fraction = 0.9999f;
                    break;
                }
            }
            else upper = thresholds[level - 1];
            real offset = upper - distance;
            real range = upper - lower;
            real value = offset / range;
            if (value < 0.0f) value = 0.0f;
            else if (value > 0.9999f) value = 0.9999f;
            *fraction = value;
            break;
        }
        ++level;
    }
    signed char previous = cached_level_4c450(object_index);
    long previous_level = previous;
    if (previous_level != -1)
    {
        long difference = previous_level - level;
        long absolute = abs(difference);
        if (absolute == 1)
        {
            if (difference == absolute && *fraction < 0.8f)
                ++level;
            else if (difference == -1 && *fraction > 0.2f)
                --level;
        }
    }
    return level;
}


struct s_camera;
real function_50650(real cosine);
long function_30830(point3f const *a, s_camera const *camera, vector3f const *d,
    point2f const *scale, bool perspective, bool negate, point2f *out);

#ifndef PIN
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#endif

// score 20 (was 43): retail's 647/647 B length and entire body (0x4ff72..0x501c5, including the pow -> xorps xmm2,xmm2 -> fstp [ebp]
// -> movss xmm0,[ebp] -> comiss xmm0,xmm2 reload and strength guard) now match byte for byte after separating the projection phase into
// `if (facing && *strength > 0.0f)`. All 20 remaining differences (prologue push order/state register ebx vs esi, 0x30830 call setup
// order + test eax vs test al, and both epilogue pop orders) are caused solely by 0x30830's parameter order (`d` before `a`, BLOCKED-BY
// contributor caller 0x4f7e0 in src/unknown_01cf50.cpp) and return type (`bool` gives 19 here and 189 on 0x30830, 0x4f7e0 unchanged at 325,
// not kept for 1 pt across four files).
// Earlier 43 (was 270): the three clamps use PIN, which reevaluates the
// clamped expression as retail does (for the angle, function_50650 inlined
// twice and then called), and the curve's base value is read where it is
// used. What still differs: retail pushes ebx and edi after the first test and
// keeps the state pointer in esi (ours ebx); it stores the pow result straight
// to *strength and rereads it, comparing with a zeroed register; and it tests
// function_30830's result as a byte. A pow on *strength or in double, and a
// bool return for function_30830 (195 -> 189 for that function, 81 -> 80
// here, across four files) were tried.
// M3 lane GA r18 (21 compiled fast trials with 0x50650/0x13bb90 MATCH, 0x30830, 0x4f7e0; 43 -> 20 at 647/647 B):
// Kept: separate `if (facing && *strength > 0.0f)` block for the 0x30830 projection (43 -> 20, 647/647 B; nested `if (facing) { if (*strength > 0.0f) ... }`
// and dropping `real value` on it give the same 20). Tried on 43: bool return on 0x30830 42 (with split facing 19; 0x30830 189, 0x4f7e0 325);
// no value local / pow(*strength) / if (*strength) before pow / combined if (*strength && 30830) / early return / double-cast pow / raw before PIN /
// else-if (value > 0) all 43; assign *strength after if 191 (637 B); else { *strength = value; } 119 (659 B); else { *strength = 0.0f; } 118 (666 B);
// exponent ternary 108 (664 B); 0.0f < value/strength 99 (632 B); nested if (curve) 55.
// M3 lane 3 r313: HELD (needs the owner's OK; held as a patch): MATCH in the fast and plain checks with 0x30830 declared
// (d, a, camera, scale, perspective, negate, out) and returning bool (its definition, the four declarations and both calls,
// one of them a line in contributor 0x4f7e0), and the atan2 result written at the cos and sin calls with no double local
// (that local keeps every register save at entry: 8-byte stack parity). 0x30830 195 -> 187, 0x4f7e0 325 unchanged. Parts:
// the reorder with the bool return 18; with the long return 1 (`test eax` for `test al`); the atan2 form alone on main 21.
// Retail sets up `a` (EDX) before forming `d` in place in the state's ESI: under the link's last-to-first argument setup
// `d` is declared first.
// M3 lane GB r21 (12 compiled fast trials on the 20 base with 0x50650/0x13bb90 MATCH, 0x30830 195): typed definition/state/curve structs 20;
// early return on count <= 0 20; nested projected check 20; inline state projection helper 293 (537 B); inline (dir, pos) wrapper 20;
// inline curve tail helper 166 (542 B); curve read at each use 187 (682 B); curve 2-pass loop 231 (703 B); real atan2 local 21;
// hoisted projected 20; pos/dir local pointers 20; bool ok from 0x30830 20. Confirms all 20 lines depend solely on 0x30830's
// d-before-a parameter order and bool return (still blocked by contributor-claimed caller 0x4f7e0).
// @retail 0x4ff50
void function_4ff50(byte const *definition, byte const *state, bool facing, real time,
    real cosine, real *strength, real *horizontal, real *vertical, real *out)
{
    if (*(long const *)(definition + 0x38) > 0)
    {
        byte const *curve = *(byte **)(definition + 0x3c);
        if (facing)
        {
            real angle = PIN(function_50650(cosine) * 0.6366197466850281f, 0.0f, 1.0f);
            real lower = PIN(curve ? *(real const *)(curve + 0x14) * 0.6366197466850281f : 0.0f, 0.0f, 0.9998999834060669f);
            real value = PIN((angle - lower) / (1.0f - lower), 0.0f, 1.0f);
            *strength = value;
            if (value > 0.0f)
            {
                if (curve && *(real const *)(curve + 0x18) > 0.0f)
                    *strength = (real)pow(value, *(real const *)(curve + 0x18));
                else
                    *strength = 1.0f;
            }
        }
        if (facing && *strength > 0.0f)
        {
            point2f projected;
            if (function_30830((point3f const *)(state + 8), &g_4b9e14, (vector3f const *)(state + 0x14),
                &g_4b9ecc, true, true, &projected) && (projected.x != 0.0f || projected.y != 0.0f))
            {
                double direction = atan2(projected.y, projected.x);
                *horizontal = (real)cos(direction);
                *vertical = (real)sin(direction);
            }
        }
        if (curve)
        {
            if (*strength > 0.0f)
            {
                real value = function_13bb90((s_tag_data const *)curve, time, 0.0f);
                out[0] = *(real const *)(curve + 0x10) - (*(real const *)(curve + 0x10) - value) * *strength;
                value = function_13bb90((s_tag_data const *)(curve + 8), time, 0.0f);
                out[1] = *(real const *)(curve + 0x10) - (*(real const *)(curve + 0x10) - value) * *strength;
            }
            else
            {
                out[0] = *(real const *)(curve + 0x10);
                out[1] = *(real const *)(curve + 0x10);
            }
        }
    }
}

s_frame_storage g_frame_storage;

s_light_frame_storage g_light_frame_storage;

s_light_definition_storage g_light_definition_storage;

s_filter_buffer_storage g_filter_buffer_storage;

extern byte g_485a75, g_485a76;
extern long g_485898;
void function_15680(short mode);
void function_15370(short mode);
extern s_record_sources *g_467138;
extern long g_467130, g_4858b4;
extern void *g_467134;
void function_0496f0();
void function_44550();
void function_445d0();
bool __stdcall function_44370(long mode);
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil);
bool function_133790(s_bit_vector_pool *state, short index, bool flag, long *bounds, long context, bool final);
void __stdcall function_44ac0(long group, dword selection_mask, long level);
void function_0494b0(real const *window, real const *bounds);
void function_40e90(long mode);

extern long g_4ba134;

PRIVATE __forceinline void draw_source_pass_r18(long group, long mode, bool extra_enabled, bool depth, bool normal_cull, long target, bool filter)
{
    bool enabled = (g_4c1a48[group].flags & (1 << mode)) && extra_enabled;
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = (void *)group;
        g_467138 = &g_4c1a48[group];
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (mode == 1 && !g_485a75 && !g_485a76) function_14b60(1, 0, 0.0f, 0);
        function_15680(normal_cull && (g_485898 < 4 || g_485898 > 7) ? 1 : 0);
        if (filter) function_0496f0();
    }
    else g_467130 = 0;
    function_15370(depth ? 1 : 0);
    if (enabled) { function_44550(); function_445d0(); }
}
PRIVATE __forceinline void draw_configured_pass_r18(long mode)
{
    if (function_44370(mode)) { function_44550(); function_445d0(); }
}

PRIVATE __forceinline void draw_light_source_pass(long group, long mode, bool depth, bool normal_cull, long target, bool filter)
{
    byte enabled = (byte)(g_4c1a48[group].flags >> mode) & 1;
    if (enabled)
    {
        if (mode == 2) enabled = g_4ba023;
        else if (mode == 8) enabled = g_4ba022;
    }
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = (void *)group;
        g_467138 = &g_4c1a48[group];
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (mode == 1 && !g_485a75 && !g_485a76) function_14b60(1, 0, 0.0f, 0);
        if (g_485898 >= 4)
        {
            if (g_485898 > 7) function_15680(normal_cull);
            else function_15680(0);
        }
        else function_15680(normal_cull);
        if (filter)
        {
            enabled = true;
            function_0496f0();
        }
        if (mode == 3 || mode == 16) enabled = true;
    }
    else g_467130 = 0;
    function_15370(depth ? 1 : 0);
    if (enabled) { function_44550(); function_445d0(); }
}

/* the index of key in the list's keys, or NONE */
PRIVATE inline short key_list_find_index(byte const *list, long key)
{
    long i;
    for (i = 0; i < *(word const *)(list + 4); i++)
    {
        if ((*(long const *const *)(list + 0xc))[i] == key)
        {
            return (short)i;
        }
    }
    return NONE;
}

// MiniMax A (11-20): no new form, no lower score (47 in the nine-address fast set); the entry's key and value read into one-use locals before the 0xb8d30 call would put the value in eax and the key in ecx as retail has them (7 with the key local declared first, 2 with the value local declared first), but one-use locals are kept only with a byte match, so the form is left out; the 0x494b0 call's argument setup order (ours lea-ecx-then-mov-eax, retail mov-eax-then-lea-ecx) needs that callee's parameters in the other order in src/unknown_0494b0.cpp: BLOCKED-BY 0x494b0.
// MiniMax A (11-20), held (47 stands): the entry's key and value read into one-use locals before the
// 0xb8d30 call, which puts the radius in eax and the group field copies in ecx/edx as retail has
// them; the only difference left is 0x494b0's argument setup order (ours ecx-then-eax, retail
// eax-then-ecx), which needs the callee's parameters in the other order in src/unknown_0494b0.cpp:
// BLOCKED-BY 0x494b0. That form scores 2; one-use locals are kept only with a byte match, so it was
// reverted at review: try it again together with 0x494b0's parameter order.
// MiniMax B (11-19): no new form tried this round, no lower score (47): the 0x494b0 call's arguments are still set
// up ecx-then-eax where retail sets eax-then-ecx, the entry's radius still goes to edx where retail uses eax, and
// the three group fields are still loaded into eax and ecx where retail uses ecx and edx; the callee's own
// convention is in another file: BLOCKED-BY 0x494b0 (and by 0x44370's inlining, as in the passes below).
// MiniMax C (11-14): no new form, no lower score (47): the 0x494b0 call's arguments are still set up
// ecx-then-eax where retail sets eax-then-ecx, the entry's radius still goes to edx where retail uses eax,
// and the three group fields are still loaded into eax and ecx where retail uses ecx and edx.
// MiniMax G (11-20): no new form, no lower score (47): the group field copies still take their values in EAX and ECX where retail uses ECX and EDX, and the radius argument of 0x1650a0 is still loaded into EDX where retail loads it into EAX; the call to 0x494b0 still sets its registers up in the other order, which is that callee's parameter order in another file: BLOCKED-BY 0x494b0.
// score 47 (was 88): r136 lane 7: draw_light_source_pass (pass 11) follows 0x44370's body for its mode: the cull through
// MiniMax I (11-20): matches retail byte for byte (was 47): the entry's value and key are read into one-use
// locals before the 0xb8d30 call, the value local first, and 0x494b0 takes its window parameter first, so the
// call sets up eax before ecx as retail does; the parameter swap is in src/unknown_0494b0.cpp (outside this
// lane's files) with its declaration and the call site here, and 0x494b0 still matches.
// Earlier 47 (was 88): r136 lane 7: draw_light_source_pass (pass 11) follows 0x44370's body for its mode: the cull through
// two 0x15680 calls (retail's dead g_485898 load and arm order) and case 11's
// `enabled = true` before the 0x496f0 call; 655/659 bytes, the remaining differences are register choices.
// MiniMax R (11-06): no new form, no lower score (88).
// MiniMax D (11-05): no new form, no lower score (88); what is left is 0x494b0's swapped argument setup (window first gives 86 but the callee lives in another file) and pass 11's dead g_485898 load and late enabled store (BLOCKED-BY 0x44370).
// Earlier 88 (was 182): only difference left is one stack slot. Retail's frame is 4 bytes larger: it stores
// a second zero (the register that also holds the loop index) at [esp+0x1c] before the guard,
// never read in the loop, and compares the count against that register (`cmp edx, ecx`) where
// ours folds to `test`. Tried: the index declared before the loop, an explicit running group
// index (`++group_index` in the loop header), an extra counter bumped with `rendered` (all 182).
// Also tried 2026-10-31 (all 182): a second counter `j` bumped with `i` and used in the second call, a dead
// local assigned in the loop, `if (g_4ba134 > i)` around the loop with `i` declared first, a cached count; the
// optimizer drops every unused counter, so the slot is not an ordinary dead local. Retail's two other
// differences are in the inlined last pass (also seen in 0x31170): a dead `mov eax, [g_485898]` before
// `xor eax, eax; call 0x15680` and a `mov byte [esp+0x13], 1` after that call; five spellings of the
// `normal_cull && (g_485898 ...)` argument fold them away (182).
// The last pass is 0x44370's mode 11 inlined: its `case 11: enabled = true; function_0496f0();` is the
// late store (BLOCKED-BY 0x44370, see 0x423c0). Also tried: `enabled = true;` before the filter call in
// draw_light_source_pass (177; 0x31170 278 -> 274), with it `first_group + i` as a named group index (177),
// `rendered` declared last (177), direct g_4ba074[i] reads with no entry pointer (177), first_group declared
// first (174; without the helper change 179).
// 2026-11-01: the extra slot was 0x133790's counter argument: retail passes the address of a `long` set to 0 before
// the loop (as matched 0x133720 does), not the bounds; `long counter = 0;` passed as `&counter` gave retail's frame
// (182 -> 88). Still different: a one-register rotation in the group-field copy, the entry->x push and the `&counter`
// push; 0x494b0's arguments are set up ecx-then-eax (its parameters swapped everywhere, window first, gives retail's
// eax-then-ecx and 0x494b0 still matches, but only 86); pass 11's dead g_485898 load and late `enabled` store (BLOCKED-BY
// 0x44370). Also tried (88 unless noted): `counter` declared first (97), direct g_4c1a48[1] field accesses, a named
// `group_index = first_group + i`, the `group` reference declared before the loop.
// Y596: Actual body-stepped entry/group cursors and short list result, seven subsets then all in while:
// 177/170/179/176/127/173/171/174. NOT kept: no equality or gain; original full loop/pass body restored; scratch registers and pass-11 staging remain.
// Round J264: The radius, group mask and counter-address registers still differ. Four accumulator encodings account for the size gap.
// typed clip box 74, named view and projection bounds 74, typed clip bound pairs 54, first record before group save 46.
// accumulated projection success 118, positive projection guard 47, group save at submission 58, window-first callee on the current pass 45.
// scoped marker workspace 47.
// NOT kept: the window-first setup improves two points and the early first-record store improves one. The later group save reaches retail size with a higher score and moves the stores after clipping.
// Round 615: four EAX group flag load/store encodings account for the four missing bytes.
// Named bounds members 47; clip indices 74; first record before flag clear 46 (NOT kept:
// one point); positive lookup region 47; separate lookup guards 47; clamp assignments 58;
// shared marker key 147; typed view pointer 47.
// MiniMax A (11-18): no new form, no lower score (47); the whole four-byte gap is the group field copies (ours encode them
// in EAX at five bytes where retail uses ECX and EDX at six) and the first real divergence is the float temp for the radius
// argument of function_1650a0 (ours in EDX, retail in EAX); seven source forms (the group through a pointer, the statements
// reordered, the flags in a local, the restore through a local) move neither the length nor the register choice.
// Combined projection rejection/admission 47/47; radius value/reference 132/47;
// count sampled before flag reset 47; flags and count sampled together 47; clip
// view before group setup or between save and reset 47/47. The radius value adds
// a frame word and the four short accumulator encodings for the group flags remain.
// @retail 0x30ed0
void function_30ed0()
{
    bool rendered = false;
    byte *state = (byte *)g_4c0b78.state;
    byte *list = *(byte **)(state + 0x14);
    long first_group = g_4c19ac;
    long counter = 0;
    for (long i = 0; i < g_4ba134; ++i)
    {
        s_flag_entry const *entry = &g_4ba074[i];
        long index = key_list_find_index(list, entry->key);
        if (index == NONE || !(*(word *)(*(byte **)(list + 8) + (short)index * 2) & 0x1000))
            continue;
        s_object_marker marker;
        byte *frustum = *(byte **)state + 4;
        long entry_value = entry->value;
        long entry_key = entry->key;
        if (function_b8d30(entry_key, entry_value, &marker, 1, false) <= 0)
            continue;
        s_bounds3d bounds3d;
        if (!function_1650a0((s_view *)frustum, &marker.matrix.position, &bounds3d, entry->x))
            continue;
        s_record_sources &group = g_4c1a48[1];
        group.unknown10 = group.flags;
        group.flags = 0;
        group.first = group.count;
        real *r = (real *)&bounds3d;
        real *clip = (real *)(frustum + 0xa0);
        if (!(r[0] > clip[0])) r[0] = clip[0];
        if (r[1] > *(real *)(frustum + 0xa4)) r[1] = *(real *)(frustum + 0xa4);
        if (!(r[2] > *(real *)(frustum + 0xa8))) r[2] = *(real *)(frustum + 0xa8);
        if (r[3] > *(real *)(frustum + 0xac)) r[3] = *(real *)(frustum + 0xac);
        function_133790((s_bit_vector_pool *)state, index, false, &counter, first_group + i, true);
        function_44ac0(3, 2, first_group + i);
        function_0494b0((real const *)&bounds3d, clip);
        draw_configured_pass_r18(10);
        draw_light_source_pass(1, 11, false, false, g_4858b8, true);
        group.flags = group.unknown10;
        group.first = 0;
        rendered = true;
    }
    g_4c19ac = first_group;
    if (rendered) function_40e90(0);
}


#include "index_cache_storage.h"
long function_b8c40(long object, short node);
struct s_light_frame_ab;
struct s_light_shape_ab;
struct s_light_projection_ab;
struct s_index_state_31660;
struct s_view_camera;
struct s_view_source;
extern long g_509440;
bool function_c17f0(long light, s_light_shape_ab *shape, bool respect_engine);
void __stdcall function_c1d90(s_light_frame_ab const *frame, s_light_shape_ab const *shape, s_light_projection_ab *entries, short *count);
s_bit_vector_pool *function_1320f0(long kind, long count, bool flag, long mask, byte const *projections,
    byte const *origin, real radius, void *filter, real plane_offset, long bsp);
bool first_person_weapon_get_marker(long object, long marker, point3f *position, vector3f *forward, vector3f *up);
real function_31b40(vector3f const *direction, vector3f *up);
void function_31660(long light, long tag, s_index_state_31660 *visibility, long parent, long root, bool flag, bool *special);
void function_31c20(s_bit_vector_pool *visibility, bool flag, long parent);
void function_2cb30();
void function_133390(s_bit_vector_pool const *visibility);
void __stdcall function_133520(s_bit_vector_pool *visibility);
void function_133720(s_bit_vector_pool *visibility, bool enabled);
void function_44720();
void function_31170(long tag, s_view_camera *camera, long resource, s_view_source *source, bool special, real weight);

// @retail 0x31c80
void function_31c80()
{
    s_cached_input records[128];
    long count = function_31910(records);
    for (long i = 0; i < count; ++i)
    {
        long light_index = *(long const *)(records[i].data + 1);
        if (!(*(real *)(records[i].data + 3) > 0.07000000029802322f)) continue;
        byte *light = g_4e030c->data + (light_index & 0xffff) * 0x110;
        long parent = NONE, root = NONE;
        byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
        if (*(short *)(light + 0x54) != NONE)
        {
            parent = *(long *)(light + 0x4c);
            long top = NONE;
            for (long current = parent; current != NONE; current = *(long *)(((s_scalar_object_header *)g_4e0300->data)[current & 0xffff].object + 0x14))
                top = current;
            root = top;
            if ((*(dword *)definition & 0x20) && !(*(dword *)definition & 0x40000))
            {
                vector3f *offset = (vector3f *)(light + 0xac);
                point3f *position = (point3f *)(light + 0x84);
                long node = function_b8c40(parent, *(short *)(light + 0x54));
                first_person_weapon_get_marker(parent, node, position, offset, (vector3f *)(light + 0xb8));
                point3f *endpoint = (point3f *)(light + 0x90);
                endpoint->x = position->x + offset->i;
                endpoint->y = position->y + offset->j;
                endpoint->z = position->z + offset->k;
                function_31b40(offset, (vector3f *)(light + 0xb8));
            }
        }
        byte shape[0x7c], projections[6 * 0x1bc];
        function_c17f0(light_index, (s_light_shape_ab *)shape, true);
        if (g_509440 < 8) g_4c6b00[g_509440++].index = light_index;
        s_bit_vector_pool *parent_visibility = g_4ba010 ? 0 : &g_547f88;
        bool special = !(*(dword *)definition & 8) && g_4ba020 && *(real *)(light + 0xf8) > 0.0f && *(short *)shape != 0;
        long bsp = *(short *)(light + 0x48);
        if (bsp == NONE && (*(dword *)definition & 0x20) && (*(dword *)definition & 0x40000)) bsp = g_4b9ee4;
        long mask = (*(dword *)definition >> 12) & 0x400;
        short projection_count;
        function_c1d90((s_light_frame_ab *)(light + 0x84), (s_light_shape_ab *)shape, (s_light_projection_ab *)projections, &projection_count);
        s_bit_vector_pool *visibility = function_1320f0(1, projection_count, false, mask, projections,
            (byte const *)(light + 0x18), *(real *)(light + 0x24), parent_visibility, 0.0f, bsp);
        function_31660(light_index, *(long *)(light + 4), (s_index_state_31660 *)visibility, parent, root, *(short *)shape == 0, &special);
        if (*(dword *)definition & 0x400000) function_31c20(visibility, special, parent);
        s_bit_vector_pool *state = (s_bit_vector_pool *)g_4c0b78.state;
        function_2cb30();
        function_133390(state);
        function_133520(state);
        function_133720(state, true);
        function_44720();
        function_31170(*(long *)(light + 4), (s_view_camera *)shape, light_index,
            (s_view_source *)(light + 0x84), special, *(real *)(records[i].data + 3));
    }
}

void __stdcall function_352e0(long target, bool multiple);
void __stdcall function_44c90(long group, dword selection_mask, long level,
    dword and_mask, dword or_mask, dword fallback_and_mask, dword fallback_or_mask, bool force);

#include "selected_source_pass.h"

// MiniMax A (11-20): no new form, no lower score (111): the 0x14b60 clear flag again, plus the nonzero cull arm order
// MiniMax B (11-19): no new form tried this round, no lower score (111): the pass helper's late `enabled` store and
// 0x14b60's first argument (retail passes it in eax) still differ; that call's convention belongs to the callee's own
// file: BLOCKED-BY 0x14b60.
// MiniMax C (11-14): no new form, no lower score (111): the pass helper's late `enabled` store and 0x14b60's
// first argument (retail passes it in eax) still differ.
// MiniMax I (11-20): no new form, no lower score (111): 0x14b60's first argument is pushed here, retail passes it in eax, and the pass helper's late `enabled` store differs: BLOCKED-BY 0x14b60.
// score 111 (was 165): r136 lane 7: the shared pass helper follows 0x44370's body (mode-based 2/8 gate, two
// the cull in draw_light_source_pass now has retail's shape, so pass 1's cull matches; the
// clear call still takes its flags on the stack where retail passes them in eax: BLOCKED-BY 0x14b60.
// r136 lane 7: the shared pass helper follows 0x44370's body (mode-based 2/8 gate, two
// 0x15680 calls for the cull, late `case 3: case 16: enabled = true;`). With the cull test in a one-use `bool
// stipple_enabled` (96; kept only with a match, coordinator 11-08) all six inlined passes equal retail; only the 0x14b60 call differs (`push 1` against retail's `mov eax, 1`): BLOCKED-BY 0x14b60 (its real body
// pads matched 0x14ac0's frame; with it this function realigns, 252). Every pass through an `inline` 0x44370 with the
// const table 384 (all eight inline; retail inlines six and calls 15 and 21).
// MiniMax S (11-20): 111 -> 101. The shared helper's cull now reads `if (g_485898 >= 4) { if (g_485898 > 7) cull;
// else 0; } else cull`, which is retail's shape, and passes 1-6 use draw_light_source_pass. (Pushing -127 instead
// of 1 as the clear flag only to give the call block retail's length was reverted at review: a byte-length device.)
// MiniMax R (11-06): no new form, no lower score (165).
// MiniMax D (11-05): no new form, no lower score (165); retail keeps the pass flag in a register and stores it late (passes 3 and 16) where ours stores it early on every enabled path, and that one extra store shifts the four global stores that follow by an instruction.
// MiniMax R (11-01): no new form, no lower score: 0x44370 (src/unknown_01cf50.cpp, not my file) is still called out of line where retail inlines it into six of the passes.
// Earlier 165 (was 165, 213): the pass flag is now tested as retail does (and al,1 stored to the stack slot, then
// reassigned from the extra gate), and the volatile store on it is gone. What still differs: retail stores
// the constant 1 into that slot late (after the cull call, passes 3 and 16 only) where we store it early on
// every enabled path; retail keeps a dead load of g_485898 in the passes that never cull; function_14b60
// takes its first argument in eax in retail (mov eax, 1) but is pushed here (callee not matched).
// Tried: a byte gate with the byte globals passed directly (no setne, 209 because the dead load is missing),
// reversed/ternary/non-short-circuit forms of the cull test (213-216), a local copy of g_485898 (165).
// Cause: each pass is `if (function_44370(mode)) { function_44550(); function_445d0(); }`; retail inlines
// 0x44370 into passes 1, 3, 8, 7, 2 and 16 (its mode table folded per mode) and calls it for 15 and 21. The
// late store is 0x44370's `case 3: case 16: enabled = true;`, the dead load its
// `g_485898 >= 4 && g_485898 <= 7 ? 0 : settings->cull_mode` with a cull mode of 0 (BLOCKED-BY 0x44370: it
// and its table live in a contributor file). Also tried: the cull test reversed (165), the raw byte gates
// (209: pass 8's gate exact, the rest shifts), `if (enabled && !extra_enabled) enabled = 0;` (200),
// `bool enabled = ... && extra_enabled` (235), every pass as draw_configured_pass_r18 (842: 0x44370 stays
// out of line), the helper with a cull-mode parameter and `if (mode == 3 || mode == 16) enabled = true;`
// (180), the same with 0x44370's mode-based gate (187: passes 3 and 8 exact, cull arms reversed) and with
// `cull_mode && (...) ? 1 : 0` (187).
// Also tried 2026-11-01: 0x14b60 written in place of its stub so its flags go in eax (250: the body needs an 8-byte
// stack-alignment pad and this function then realigns its frame with `and esp,-8`); passes 1, 3, 7 and 16 through
// draw_light_source_pass (204: pass 1 then matches, but 3 and 16 lose retail's late store), only passes 1 and 7
// through it (192), with draw_light_source_pass's cull argument as `g_485898 >= 4 && g_485898 <= 7 ? 0 : (normal_cull
// ? 1 : 0)` (211: the dead load still folds away, and 0x32350 rises to 587); the gate as `if (!extra_enabled)
// enabled = 0;` (191; 0x41cc0 186, 0x2bcd0 362); every pass as draw_configured_pass_r18 also with the mode table const (842).
// Round J264: The clear stub still uses a stack flag. Retail also lays out the nonzero cull arms first in modes 1, 3 and 2.
// inclusive-range arm first 111, short cull result 187, requested cull then disable 243, two consecutive bound decisions 175.
// table cull modes 111, table cull and targets 111, table group/cull/target 111, table-driven mode traversal 649.
// NOT kept: the first branch and constant table forms tie. Single-result cull forms raise shared-user scores. The mode loop removes the retail pass expansions.
// MiniMax A (11-20): no new form, no lower score (111); the clear call still needs 0x14b60's real body (BLOCKED-BY 0x14b60) and the cull arm order is the shared header's (BLOCKED-BY include/selected_source_pass.h).
// MiniMax A (11-18): no new form, no lower score (111); the shared pass helper and the pass list stay as they are, because
// retail's clear call passes its flags in EAX and needs 0x14b60's real body (BLOCKED-BY 0x14b60, whose body pads matched
// 0x14ac0's frame and realigns this function); what still differs is that call's argument setup and the cull arm order.
// MiniMax H (11-20): no new form, no lower score (111); the clear call still needs 0x14b60's real body (BLOCKED-BY 0x14b60) and the cull arm order comes from the shared pass header.
// Range-based pass at modes 1, 3, 2, their three pairs and all three:
// 106/101/106/106/111/106/111. Explicit enable at the first pass 223.
// NOT kept: no reduction at retail length. The clear flags still travel on the stack.
// @retail 0x423c0
void __stdcall function_423c0(void *payload)
{
    dword const *selection = (dword const *)payload;
    dword selection_mask = selection[1] | 0xffffffcf;
    long level = (long)selection[0];
    s_record_sources &group = g_4c1a48[0];
    group.unknown10 = group.flags;
    group.flags = 0;
    group.first = group.count;
    function_352e0(NONE, true);
    function_44c90(0, selection_mask, level, NONE, 0, NONE, 0, true);
    draw_light_source_pass(0, 1, true, true, 1, false);
    draw_light_source_pass(0, 3, true, true, g_4858b8, false);
    draw_light_source_pass(0, 8, false, false, g_4858b8, false);
    draw_light_source_pass(0, 7, false, false, g_4858b8, false);
    draw_light_source_pass(0, 2, false, true, g_4858b8, false);
    draw_light_source_pass(0, 16, false, false, g_4858b8, false);
    draw_configured_pass_r18(15);
    draw_configured_pass_r18(21);
    group.first = 0;
    group.flags = group.unknown10;
}

void function_48ee0(byte const *state, byte const *matrix, byte const *tag);
void __stdcall function_15720(real x, real y);

extern byte g_4ba018;
extern dword g_4b843c;

// MiniMax B (11-19): no new form tried this round, no lower score (212): the 0x4a1e0 call still sets its arguments up
// in this build's order (the resource index in eax, the source in ecx) where retail passes the source in eax, the
// camera in ecx and the resource index in edx, and leaves the flags on the stack; this round read retail's 0x4a1e0
// prologue and found that its declaration is (camera, resource_index, source, flags, ...), so the callee's convention
// is fixed by the declaration in this file (src/unknown_0350e0.cpp, with a copy in src/unknown_03fdf0.cpp) and not by
// the call site alone: reordering it here would break the call in src/unknown_03fdf0.cpp, so both files have to move
// together: BLOCKED-BY 0x4a1e0.
// MiniMax C (11-14): no new form, no lower score (212): the 0x4a1e0 call still sets its arguments up in this
// build's order (the resource index in eax, the source in ecx) where retail passes the source in eax, the
// camera in ecx and the resource index in edx, and leaves the flags on the stack.
// score 212 (was 237): r136 lane 7: draw_light_source_pass (passes 11, 14, 12, 13) follows 0x44370's body for its modes
// (stipple cull through two 0x15680 calls, case 11's `enabled = true`); 925/939 bytes.
// Earlier 237 (was 278, 300): the camera is now a register parameter (the `&camera` alias that forced it onto the stack is gone, so the
// stack arguments are retail's four and the epilogue is `ret 0x10`), the pass-13 test is retail's two separate bit tests and the
// pass-30ed0 test is retail's `shr 2` form. What still differs: (1) retail keeps the stores to g_4ba014 that precede a conditional
// branch (the initial 0, after the bit-1 step, after the 0x24 step) and tests the flags through `mov cl, al`, as if g_4ba014 could be
// aliased by the indirect loads that follow; ours drops them. (Forcing that with `((byte *)&g_4ba014)[0]` scores 242, but that takes a
// global's address as a trick and is not used.) (2) the four passes are 0x44370's body inlined in retail (dead g_485898 loads, the
// `enabled = true` re-store after pass 11's cull call), BLOCKED-BY 0x44370 as in 0x404b0. (3) 0x4a1e0 takes its register arguments in a
// different order (retail: camera in ecx, source in eax, resource in edx; callee not matched). (4) retail lays the `>` chain out as
// ja, ja, jbe-to-else with the `or 2` falling through.
// Tried: a local `flags` stored to g_4ba014 after each step (293: the compiler drops the intermediate stores), flags as a ternary or a
// negated condition (289, same), `(byte)` / `!= 0` / bool casts on the first bit test (278, same).
// Second pass (logic walked against 0x31170-0x31518: every branch, mask, pass argument and the tail agree, so what is left is code
// shape): (5) retail reads the flags twice for the last pass (`test byte ptr [g],1` then `mov edx,[g]; shr edx,1`) where we
// load once; 0x3fdf0 shows the same quirk. (6) 0x4a1e0's own prologue (retail 0x4a1e0-0x4a28f) reads source from eax (+0x50..+0x68),
// camera from ecx (`mov dx,[ecx]`) and resource from edx (`cmp edx,-1`), exactly as retail's caller loads them (0x311b4-0x311c3), so
// the call-site difference is the stub's register mapping, not this function. Tried: the bit sets through an inlined copy of
// 0x30ca0 (`if (v) g |= 1 << n; else g &= ~(1 << n)`, 278, same code; the real 0x30ca0 is called, not inlined, from 0x32350), the
// `!enabled || bit0` branch order (278), a `moving` bool folded into one ternary store (278), a volatile-cast read of g_485a40
// to provoke the store before the branch (no change to the store). A dword bit-field view of g_4ba014 (cast of its
// address, not kept) scores 274 and 264 with the inlined sets, but still drops the store before `test bl,bl` and turns the bit-0
// read into `mov al,[g]`; no spelling tried reproduces that store, so the 54-byte shortfall (and the shifted compare after it) stays.
// 2026-11-01: bits 1 and 3 set through the existing bit_vector_set(&g_4ba014, ...) (237): g_4ba014 is then address-taken, so the
// initial 0 store and the store before the definition read appear and the `>` chain gets retail's layout; still one store short
// (retail also stores after the bit-1 step, before `test bl,bl; je`, then tests bit 0 through `mov cl, al`). Also tried on top of
// that (all 237): bit 0 tested with bit_vector_test in the `enabled` test or in the last pass's test, every constant bit
// operation (|= 0x24, &= ~0x2c, &= ~0x10) as bit_vector_set calls, the bit-1 test as a named `moving` bool, the `enabled`
// test inverted (`!enabled || bit 0` clears). g_4ba014 defined in a non-LTCG object instead (diagnostic) changes nothing (278).
// Round J264: The 0x4a1e0 argument registers, the first mask publication and the late flag reads still differ.
// early local mask publication 212, early mask with local gate 212, shared camera classification 218, local mask through special-pass setup 260.
// camera bit through the mask setter 212, joined clear mask 197, shared two-bit decision 212, sibling bit-mask tests 214.
// joined flag result with two clears 215, carried flags with two clears 215.
// NOT kept: the 197 form uses a computed clear mask where retail clears separate constants. The two forms preserving the retail clears score 215.
// MiniMax A (11-20): no new form, no lower score (212); the 0x4a1e0 argument registers, the first mask publication and the late flag reads still differ (BLOCKED-BY 0x4a1e0, whose declaration is in src/unknown_03fdf0.cpp).
// MiniMax A (11-18): no new form, no lower score (212); what still differs is the 0x4a1e0 argument registers, the first mask
// publication and the late flag reads, and the body stays fourteen bytes short of retail because no spelling of the bit-0 and
// bit-1 steps reproduces the store retail keeps before the branch.
// MiniMax B (11-20): no new form, no lower score (212): 0x4a1e0's register mapping is fixed by a declaration shared with another file, and the extra mask store, the twice-read flag and the 0x14b60 argument still differ.
// MiniMax H (11-20): no new form, no lower score (212); the 0x4a1e0 argument registers, the first mask publication and the late flag reads still differ (BLOCKED-BY 0x4a1e0).
// @retail 0x31170
void function_31170(long tag, s_view_camera *camera, long resource, s_view_source *source, bool enabled, real scale)
{
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    function_40e90(0);
    if (enabled) enabled = (g_4c1a48[2].flags & 0x400) != 0;
    function_4a1e0(source, camera, resource, (s_view_flags *)definition, 1024.0f, enabled, scale);
    g_4ba018 = 1;
    g_4ba014 = 0;
    if ((!g_510c50 || !((byte *)g_510c50)[5]) && *(short *)(definition + 4) == 0)
        g_485950.i = g_485950.j = g_485950.k = 0.0f;
    g_4ba014 = *(short *)camera == 0 ? 1 : 0;
    bit_vector_set(&g_4ba014, 1, g_485950.i > 0.009999999776482582f || g_485950.j > 0.009999999776482582f || g_485950.k > 0.009999999776482582f);
    if (enabled && !(bool)((g_4ba014 >> 0) & 1))
    {
        g_4ba014 |= 0x24;
        bit_vector_set(&g_4ba014, 3, g_485a40 > 0.0f && *(short *)(definition + 0x14) != 2);
    }
    else g_4ba014 &= ~0x2c;
    g_4ba014 = g_4ba014 & ~0x10;
    if ((bool)((g_4ba014 >> 2) & 1))
    {
        function_48ee0((byte const *)source, (byte const *)camera, definition);
        draw_configured_pass_r18(10);
    }
    if ((bool)((g_4ba014 >> 2) & 1) || (bool)((g_4ba014 >> 1) & 1) || (definition[0] & 0x20))
        draw_light_source_pass(1, 11, false, false, g_4858b8, true);
    if (g_510c50 && ((byte *)g_510c50)[5] && (bool)((g_4ba014 >> 2) & 1) && g_4ba134 > 0) function_30ed0();
    draw_light_source_pass(1, 14, false, false, 1, false);
    draw_light_source_pass(1, 12, false, false, g_4858b8, false);
    if ((g_4ba014 & 1) && (bool)((g_4ba014 >> 1) & 1)) draw_light_source_pass(1, 13, false, false, g_4858b8, false);
    g_4858bc = 0;
    g_4b843c = 0;
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, FALSE);
    D3DDevice_SetScissors(0, false, 0);
    function_15720(0, 0);
    function_14bc0((short)g_4858b8, 0, true);
    g_4ba018 = 0;
    g_4ba014 = 0;
}

PRIVATE __forceinline bool fog_pass_enabled_41cc0()
{
    if (g_4b9f5c != NONE && !g_4b9f88)
    {
        real maximum = g_4b9f78 > g_4b9f7c ? g_4b9f78 : g_4b9f7c;
        return maximum > 0.0f;
    }
    return false;
}

// MiniMax A (11-20): no new form, no lower score (100): the 0x14b60 clear flag is pushed instead of passed in eax
// MiniMax B (11-19): no new form tried this round, no lower score (100): 0x14b60's first argument still goes on the
// stack here where retail passes it in eax, and the jump-table bytes it shifts still differ; that argument
// convention belongs to the callee's own file: BLOCKED-BY 0x14b60.
// MiniMax C (11-14): no new form, no lower score (100): 0x14b60's first argument still goes on the stack here
// where retail passes it in eax, and the jump-table bytes it shifts still differ.
// MiniMax I (11-20): no new form, no lower score (100): 0x14b60's first argument is pushed here, retail passes it in eax and the jump-table bytes it shifts differ: BLOCKED-BY 0x14b60.
// score 100 (was 138): r136 lane 7: the shared pass helper now follows 0x44370's own body (the 2/8 gate taken by mode,
// the shared helper's cull has retail's shape now; left: the clear call (retail passes its
// flags in eax, BLOCKED-BY 0x14b60) and pass 3's cull test, which still comes from include/selected_source_pass.h
// (off limits) with its `||` form.
// r136 lane 7: the shared pass helper now follows 0x44370's own body (the 2/8 gate taken by mode,
// the two 0x15680 calls for the cull, the late `case 3: case 16: enabled = true;`). With the cull test in a one-use
// `bool stipple_enabled` (91; kept only with a match, coordinator 11-08) the five passes equal retail's inlined 0x44370 (dead g_485898 loads and pass 3's late stores included); only the 0x14b60
// call differs (ours pushes the flags, retail passes them in eax): BLOCKED-BY 0x14b60, whose real body makes LTCG
// pad matched 0x14ac0's frame (0x30 -> 0x34) and realign this function (`and esp,-8`, 268). Retail inlines 0x44370
// itself for the first five passes and calls it for the rest (its per-caller inline budget runs out; 0x423c0 six,
// 0x3fdf0 six, 0x2bcd0 five). Tried r136: mode table as a typed const array + `inline` on 0x44370 + every pass
// through draw_configured_pass_r18 733 (the table folds but all ten passes inline), with 0x44370's body fixed
// 727/761, 0x44370 moved without `inline` to an /O2 file (diagnostic, not kept) 761, the helper's stipple form alone
// 207 (997 bytes).
// MiniMax R (11-06): no new form, no lower score (138).
// MiniMax D (11-06, run 49): no new form, no lower score (138); retail still calls function_14b60 with the flags in eax, still stores 1 into the slot after function_15680 in both arms of pass 3, and still loads g_485898 into a dead eax in the three passes with normal_cull false.
// Earlier 138 (was 138): earlier 210; the passes now use draw_selected_source_pass (shared with 0x423c0), whose `enabled` is a separate
// Earlier 138. MiniMax D (11-06, run 49): no new form, no lower score (138); retail still calls function_14b60 with the flags in eax, still stores 1 into the slot after function_15680 in both arms of pass 3, and still loads g_485898 into a dead eax in the three passes with normal_cull false.
// the passes now use draw_selected_source_pass (shared with 0x423c0), whose `enabled` is a separate
// variable so retail's `shr eax,N; and al,1; mov [esp+0x14],al` appears. Still different: retail
// calls function_14b60 with flags in eax (the stub declares it __stdcall, 4 stack args), retail
// stores 1 into the slot after function_15680 in both arms of pass 3, and the three passes with
// normal_cull false still load g_485898 into a dead eax in retail. Tried: a gated helper that
// keeps the raw g_4ba022 byte in the slot (removes the extra `mov [esp+0x1c],1` stores and matches
// pass 8, 6 differing spots left, but 26 bytes short so the score is 193), the volatile spelling
// from 0x423c0 (209), `enabled = enabled && extra` (213).
// Round 186: LCS-aligned this body is only 12 instructions off (the positional count is the shifts):
// the early `enabled = 1` stores, pass 3's late stores, pass 8's raw gate byte, the dead g_485898 loads
// of the three no-cull passes and function_14b60's eax argument. A gate pointer (`byte const *gate`,
// NULL for the ungated passes, `if (enabled && gate) enabled = *gate;`) leaves 6 (193 positional,
// 958 bytes): the dead loads only appear when 0x44370 itself is inlined at link time (a file-local
// copy folds them away), and 0x14b60's register argument needs its body (disabled in its owner's
// file because it moves matched 0x14ac0's frame). Also tried: `enabled &= extra_enabled` 191,
// `!(g_485898 >= 4 && g_485898 <= 7) && normal_cull` 138, 0x44370's `>= 4 && <= 7 ? 0 : cull`
// 138 (cull arms reversed), `if (enabled && extra_enabled != 1)` 167.
// Round 280: existing configured provider for culled/gated/both/unculled/culled plus
// unculled/gated plus unculled/all five modes 482/280/540/340/636/435/722;
// plain loop over the actual mode sequence 795. Provider stays out of line;
// original whole body and call roles restored. Flag stores and stub convention remain.
// Round 36 (fast, the shared pass helper changed for all three callers): the gate only for modes 2/8 with
// `if (mode == 3 || mode == 16) enabled = true;` after the cull call and pass 8's raw byte 193 (958 bytes;
// 0x423c0 201, 0x2bcd0 371), the same with the `!= 0` gate 186, with `g_485898 >= 4 && g_485898 <= 7 ? 0 :
// cull` 193; the gate pointer with that late store 193, with the cull ternary 193. The late store is
// threaded away in every form (pass 3 then has no store at all): retail keeps it because the value is
// 0x44370's result, and its dead loads come from the cull mode read from 0x44370's mode table, which
// retail folds late from .rdata; ours (g_render_modes_storage) is a non-const global in a contributor file.
// P266: Disabled-return pass epilogue with byte/bool/short/long state 222/222/242/230;
// exclusive enabled/disabled epilogues with those domains 209/209/239/216.
// All shared users regress; original common epilogue, helper and complete caller restored.
// Round 360: complete conditional gate results in byte/bool/long/dword, shifted-bit/native-mask,
// 207/207/241/241/207/207/241/241. Every shared user also regresses; original helper restored.
// Earlier starting score 210; flag-store timing, dead mode loads and clear-call convention remain.
// Round 307: existing shared light-pass helper for culled/gated/unculled/both culled-gated/
// Round 307: culled-unculled/gated-unculled/all families 179/198/192/183/189/210/186;
// Round 307: first pass alone 171. Original caller and all shared helpers restored;
// Round 307: late enabled stores, dead mode loads and the stub register convention remain.
// F327: Existing light-pass producer for individual modes 3/20/7, then
// 3+20/3+7/3+20+7/8+20/8+7: 200/186/174/206/206/211/205/205.
// Original complete callback and all shared helpers restored; late enabled stores and mode loads remain.
// Round 536: Shared cull call owned by nested result/explicit call arms/inside-range arms/normal-cull arms, then the same four with the raw caller gate: 214/138/138/138/217/191/191/191 fast.
// Original helper and complete caller restored; enabled stores, mode loads and clear-call convention remain.
// DH299: Complete branch-owned fog maximum crossed with raw mode-eight gate,
// per-use source-group fields and native payload-byte count: 142/193/142/193/234/217/234/217.
// Original maximum producer and callback restored; late enabled stores, dead mode loads and clear convention remain.
// Y538: Existing selected-bit admission with completed enabled result before depth in
// byte/bool/short/long 257/235/240/212; after depth 234/262/198/239. Every shared user
// regresses. Original helper/caller restored; late pass bytes, dead mode loads and clear convention remain.
// Round 55 (fast): group as a pointer 138; mask as a ternary 275 (998 bytes); raw pass-8 byte gate 191;
// `(bool)g_4ba022` gate 138; byte level count 230. Retail 0x14b60 takes the flags in eax with three stack
// arguments (ret 0xc); ours is a standard 4-argument stub, so its call setup stays different.
// F340: Complementary rejection state in bool/byte/short/long, sequential selected-bit
// then gate versus complete admission: 218/218/228/231/210/210/242/247.
// Every shared user regresses; original helper and complete caller restored.
// B409: Extra-gate-first pass state, initial-gate rejection and admitted-gate conditional
// selection in byte/bool/short/long: 248/248/219/213/205/237/231/238. Both peer users regress;
// original shared producer and complete caller restored; native late bytes/mode loads remain.
// F366: Homogeneous unculled/configured/both mode-table traversals, then head-tested pair,
// positive fixed-count pair and advancing cursors: 277/263/296/277/263/277/263/290.
// All regress and shorten; original callback and shared helpers restored; late pass stores remain.
// F370: Parameter-owned enabled result, rejection/conditional updates in
// byte/word/long/dword: 248/221/215/215 and 207/232/255/255. NOT kept: every
// form also regresses both other shared users. Original separate result, helper
// contracts and callback restored; late pass bytes and mode loads remain.
// DH345: Existing fog producer single-exit admission/rejection results in
// bool/byte/short/long: eight at 138. Original maximum, helper, caller and pass
// operations restored; late enabled bytes, mode loads and clear convention remain.
// J252: Existing cull/depth/draw/setup inline contracts: 138/138/138/138/138/138/138/138.
// NOT kept: no qualifying native-frame gain or byte equality; original full graph restored.
// J252: Existing configured-provider inline and shared pass path 2137; NOT kept: no eligible native-shape gain; original provider and complete caller restored.
// AC542: Normal-cull input owns its completed decision, complete/conditional production in
// bool/byte/short/long: 251/251/258/254/200/200/211/216. NOT kept: target and both shared users regress;
// original helper, input contracts and full callback restored. Late enabled/mode/clear gaps remain.
// AC544: Original two-stage enabled result in bool/char/signed-char/short/word/long/dword,
// then bool with the raw mode-eight gate: 138/138/138/187/187/222/222/138. NOT kept: target ties or
// regresses; wide results also regress both shared users. Original helper and full caller restored.
// AC546: Existing shared pass helper returns availability to all three draw callers in
// bool/signed-char/byte/short/word/long/dword/int: eight at 138. NOT kept: no gain or
// native publication repair; original shared helper and every complete caller restored.
// DH366: Existing provider forced-inline contract, original caller then configured
// culled/gated/both/unculled/culled-unculled/gated-unculled/all families:
// 1445/1683/1635/1841/1772/1984/1937/2137. NOT kept: every graph expands and worsens shared users;
// original full contract and caller restored. Late pass bytes and mode loads remain.
// DH373: Existing shared-pass result, gate, cull, depth, filter, mode, group and target
// lexical identities: eight at 138. NOT kept: no byte match or gain. Original helper and
// complete callback restored; late enabled bytes, dead mode loads and clear convention remain.
// AC546: Existing shared pass helper returns availability to all three draw callers in
// bool/signed-char/byte/short/word/long/dword/int: eight at 138. NOT kept: no gain or
// native publication repair; original shared helper and every complete caller restored.
// N399: Shared selector formal in int/dword/unsigned int/short/word/char/signed char/byte,
// unchanged representable literal modes and all shared users checked: fast 138/138/138/138/138/138/138/138.
// NOT kept: no native publication/equality recovery; original helper/caller restored.
// Y602: Existing bit-vector availability byte/bool/short/long, direct/repeated group receivers:
// 138/138/213/222/138/138/213/222.
// NOT kept: no byte match or qualifying native-shape repair; original complete body and helpers restored.
// F397: Separate byte result with bool/char/signed-char/short/word/int/long/dword gate
// formals: 138/138/138/138/138/138/138/138. NOT kept: target ties; bool changes shared
// caller to 307 without repairing native raw-byte publication. Original helper/body restored.
// Round 94 (lane 9, fast): 0x14b60 written in place of its stub (eax flags, three stack arguments) 268 (991 bytes:
// 0x14b60 and 0x14ac0 get a 4-byte pad and this function realigns with `and esp,-8`; 0x14ac0, 0x31c80 lost); with
// its depth parameter address-taken 268. Diagnostic: 0x14b60 without its 0x14ac0 call gives no pad and 132 here
// (0x423c0 160), so the alignment demand comes through 0x14ac0, not D3DDevice_Clear. Shared pass helper: the clear
// test nested under `mode == 1` 138; a `sources` pointer for the flags and g_467138 138; 0x44370's mask-gated
// flag test 141. 0x44370 is retail-inlined here but lives in an /Ob1 file, so it stays out of line.
// P603. Nested clear checks 100, paired clear-byte guard 100, named clear admission 100,
// one mode read with the in-range cull branch 100, assigned short cull 223, ternary short
// cull 172, upper range bound first 104, nested clear with the one-read range 100.
// NOT kept: the clear guards tie and the shorter shared cull calls raise the peer scores.
// The stub still passes the clear flags on the stack.
// Round 615: the clear call still pushes its first argument, and the cull fallthroughs differ.
// One-based payload 233; end cursor 198; separate index base 101; fog mask bit 100;
// mask set in both arms 113; first record before flag reset 104; direct group fields 100;
// flags restored before first-record reset 100.
// MiniMax S (11-20): 100 -> 96. 0x14b60's first argument selects the clear mask (bit 0 the colour clear, bits 1-4
// the buffer mask, bits 5-6 the stencil); the stub takes it on the stack where retail passes it in eax, so the call
// block is shorter than retail's and every branch after it moves (BLOCKED-BY 0x14b60; passing -127, the same mask,
// to fix the length is a device and was reverted at review). The cull in draw_light_source_pass is now
// `if (g_485898 >= 4) { if (g_485898 > 7) cull; else 0; } else cull`, retail's shape (jl to the shared cull block,
// jle to the zero block). Pass 3 still uses the header's helper, whose `||` cull form is off limits.
// MiniMax A (11-20): no new form, no lower score (100); the five passes match retail's inlined 0x44370 apart from the clear call, which needs 0x14b60's real body (BLOCKED-BY 0x14b60).
// MiniMax A (11-18): no new form, no lower score (100); the five passes equal retail's inlined 0x44370 apart from the clear
// call, and that call needs 0x14b60's real body (BLOCKED-BY 0x14b60, whose body pads matched 0x14ac0's frame and realigns
// this function), so its argument setup stays different.
// MiniMax H (11-20): no new form, no lower score (96); the clear call's first argument is stacked where retail passes it in eax (BLOCKED-BY 0x14b60).
// Other source-pass helper for mode 3, 3 and 8, 3 and 20, 3 and 7, 3 with 20
// and 7, 3 with 8, 20 and 7, 8 and 20, or 8 and 7: 179/186/177/185/178/196/168/171.
// NOT kept: all raise the score. The first-pass clear argument setup still differs.
// @retail 0x41cc0
void __stdcall function_41cc0(void *payload)
{
    byte const *levels = (byte const *)payload;
    long level_count = levels[0];
    dword mask = 0x40000;
    if (fog_pass_enabled_41cc0()) mask = 0xc0000;
    s_record_sources &group = g_4c1a48[0];
    g_485a75 = 1;
    group.unknown10 = group.flags;
    group.flags = 0;
    group.first = group.count;
    function_352e0(NONE, true);
    for (long i = 0; i < level_count; ++i)
        function_44c90(0, NONE, levels[i + 1], 0x11818e, mask, 0x8000, 0x40000, false);
    draw_light_source_pass(0, 1, true, true, 1, false);
    draw_light_source_pass(0, 3, true, true, g_4858b8, false);
    draw_light_source_pass(0, 8, false, false, g_4858b8, false);
    draw_light_source_pass(0, 20, false, false, g_4858b8, false);
    draw_light_source_pass(0, 7, false, false, g_4858b8, false);
    draw_configured_pass_r18(2);
    draw_configured_pass_r18(16);
    draw_configured_pass_r18(18);
    if (fog_pass_enabled_41cc0()) draw_configured_pass_r18(19);
    draw_configured_pass_r18(15);
    group.first = 0;
    g_485a75 = 0;
    group.flags = group.unknown10;
}

#include "depth_parameters_storage.h"
extern long g_4ba03c;
extern byte g_509430;
byte g_48575c;
bool function_13450(s_depth_parameters_13450 const *parameters);
bool function_289e0();
bool function_159610();
void function_2c370(long mode);
void __stdcall function_335e0(long mode);
void function_25d50();
void function_25dc0(bool first, bool second);
bool function_3ba90(long bit, s_record_sources const *data);
bool __stdcall function_3c3c0(bool render, bool clear);
void function_2c340(bool enabled);
void function_c3e90();
void function_1595d0();
void __stdcall function_42b20(dword mask);
void function_1e370();
long function_155760(long local_player);
bool function_14ddc0(long local_player);
long function_14de70(long local_player);
long function_cbd50(long unit, short slot);
void function_2afa0(point2f const *scale);
bool function_3fdf0(long mode, bool single_pass);
bool function_404b0(long mode);
bool function_40870();
void function_2dd30();
void function_020770();
void __stdcall function_13870(bool finish, bool blur);
bool function_13630();

// score 233 (was 262): 0x3ba90 now returns bool (retail stores its AL straight to the byte slot at 0x2c10e, no
// test/setne), which also gives retail's weapon-lookup registers; left: 0x14b60's first argument (retail EAX; its body
// stays disabled in src/unknown_01cf50.cpp, enabling it loses six matched functions), pass 3's cull arms in the other
// block order (shared header form), and the second 0x25dc0 call's first flag, which retail builds after `mov dl, 1` as a
// branch diamond where every form here folds to setle (6 bytes short). M3 lane 5 r246 (H2M_FAST with 0x25dc0, 0x3ba90):
// two calls under `if (g_4c19ac > 0)` / `<= 0` 335 (1724 B, the tail after the call duplicated); the argument as
// `g_4c19ac > 0 ? false : true`, `<= 0 ? true : false`, `!(g_4c19ac > 0)`, an if/else-assigned bool, `g_4b9f88 ||
// g_4c19ac <= 0`, `g_4c19ac <= 0 || g_4b9f88`, `!(g_4c19ac > 0 && !g_4b9f88)` 233 each (setle); a bool started true
// and cleared under `g_4c19ac > 0` 228 (1638 B, but `mov cl, 1` ahead of the test and no jmp: not retail's shape, and a
// flag set in a test, so not kept); a file-local inline returning false/true by early return 233, by a result local
// 237 (diagnostics); passes 3, 5, 8, 20 (each, 3 and 5, all four) through draw_light_source_pass 233 (same code).
// LEAD (match-only, a read of g_4b9f88 the enclosing test already decides): the first flag as `g_4c19ac > 0 ? g_4b9f88 :
// true` 156 at 1640 B (also `<= 0 ? true : g_4b9f88`), 13 rows left: 0x14b60's EAX argument, pass 3's cull arms, the
// flag still built with setle/test/setne where retail branches, and the weapon object's ESI load; on it the weapon
// object named before its tag read 156 (same code). Held as a patch.
// MiniMax A (11-20): no new form, no lower score (269): the 0x25dc0 branch diamond and the 0x14b60 argument
// MiniMax B (11-19): no new form kept, no lower score (269): the permuter's variants again need a flag set by one
// test and read by the next, a swapped argument pair at the second 0x25dc0 call and a constant held in a local, and
// the one-use `bool` form that reaches retail's 1638 bytes (206) is kept only with a byte-for-byte match; what still
// differs is retail's branch diamond for the call's first flag, the weapon lookup's tag read through esi, the byte
// return of 0x3ba90 stored in a stack slot and reloaded, and the 0x14b60 call's first argument in eax:
// BLOCKED-BY include/selected_source_pass.h and by 0x14b60's stub.
// MiniMax I (11-15, second pass): no new form, no lower score (269): taking the return of 0x3ba90 as a byte makes
//   the body exactly retail's 1638 bytes with 436 instructions, the same count as retail, but the score falls only
//   from 269 to 266, so the body stands. What still differs: retail stores that byte in a stack slot and reloads it
//   (0x2c10e, 0x2c125) where this build keeps the long return in a register, and the two differences listed below
//   are unchanged.
// MiniMax I (11-15): no new form, no lower score (269): the permuter's best variant (205 on the reduced image)
//   needs a flag set by one test and read by the next, a swapped argument pair at the second 0x25dc0 call and a
//   constant held in a local, so none of it can be kept. What still differs: retail builds the first flag of the
//   second 0x25dc0 call with a branch diamond (mov dl,1 / jle / xor cl,cl / jmp / mov cl,1) where this build
//   folds it to setle; retail keeps the object pointer of the weapon lookup in esi and reads the tag through it
//   (0x2c25c, 0x2c260) where this build loads it into eax and dereferences eax; retail stores the byte return of 0x3ba90
//   in a stack slot and reloads it (0x2c10e, 0x2c125) where this build keeps the long return in a register; the
//   0x14b60 call takes its first argument in eax in retail and on the stack here.
// MiniMax A (11-20): no new form, no lower score (269); the cull test's branch order is the shared header's (BLOCKED-BY include/selected_source_pass.h), the clear call is the stub's (BLOCKED-BY 0x14b60) and 0x3ba90 returns a long where retail keeps a byte (BLOCKED-BY its declaration).
// MiniMax H (11-18): score 269 (was 269): no new form kept, no lower score. The permuter's variant (205) swaps the
// two arguments of the second 0x25dc0 call, but retail passes the first flag in cl and the constant 1 in dl
// (0x2c1c1, 0x2c1c9), so the swap is wrong and the call was restored. A one-use `bool` holding
// `g_48575c && filter_mode != 2` reaches retail's 1638 bytes and a score of 206, but retail's own block is the
// branch form (test / je / cmp / je, 0x2c0a9), so the local was restored (one-use flags are kept only with a
// byte-for-byte match). What still differs: retail's cull test branches the other way (jle at 0x2be13 and
// 0x2be95 against this build's jg) and that test is in include/selected_source_pass.h, which this lane may not
// edit (BLOCKED-BY); retail's 0x14b60 call takes its first argument in eax (0x2bdff) while the stub in
// src/stubs/lane_w.cpp takes four stack arguments (BLOCKED-BY, one byte); retail's second 0x25dc0 call builds
// its first flag with a branch diamond (mov dl,1 / jle / xor cl,cl / jmp / mov cl,1, 0x2c1c1) where this build
// folds it to setle, and the two-call form duplicates the call (421); retail stores the byte return of 0x3ba90
// in a stack slot and reloads it (0x2c10e, 0x2c125) where this build keeps the long return in a register (the
// byte form, 266, is under the keep gate).
// MiniMax I (11-20): no new form, no lower score (269): retail's branch diamond for the second 0x25dc0 call, the weapon lookup's tag read through esi, 0x3ba90's byte return reloaded from a slot and 0x14b60's first argument in eax still differ: BLOCKED-BY 0x14b60 and the pass helper's file.
// Earlier 269 (was 331): r136 lane 7: the shared pass helper follows 0x44370's body (mode-based gate, two 0x15680
// the 0x3ba90 result is held in a bool (retail stores a byte), so the function is retail's
// length now (1638 bytes); left: mode 3's pass still comes from include/selected_source_pass.h (off limits) with its
// `||` cull form, 0x14b60 still takes its first argument on the stack (BLOCKED-BY 0x14b60) and the second 0x25dc0
// call still folds its first flag to setle where retail builds it with a branch diamond.
// MiniMax S (11-20): 269 -> 262. Mode 1's pass moved off include/selected_source_pass.h (off limits) to
// draw_light_source_pass, whose cull test (>= 4, then > 7) has retail's branch order. The 0x3ba90 result is held
// in a bool (the callee returns `!result`, 0 or 1), which gives the function retail's 1638 bytes (a `(byte)` cast
// on a long local gave the same score and was replaced at review; -127 as the clear flag was reverted). Still
// different: mode 3's pass uses the header helper's `||` cull form, where retail branches jle and merges the two
// cull arms, and it keeps its pass flag in a register where retail stores it in a stack slot (the helper's own
// `||` and range forms all give retail's order in the other passes but not there); 0x14b60's first argument still
// goes on the stack (BLOCKED-BY 0x14b60); the second 0x25dc0 call still folds its first flag to setle.
// r136 lane 7: the shared pass helper follows 0x44370's body (mode-based gate, two 0x15680
// calls for the cull, late `case 3: case 16: enabled = true;`). With the cull test in a one-use `bool stipple_enabled`
// (260; kept only with a match, coordinator 11-08) passes 1, 3, 5, 8 and 20 now
// equal retail's inlined 0x44370 except the 0x14b60 call (BLOCKED-BY 0x14b60). Every pass through an `inline`
// 0x44370 with the const table 1741 (all fifteen inline; retail five).
// Walk b3H (11-06), no source change: every block compared with retail 0x2bcd0-0x2c333 (prologue and memset block, the
// 0x289e0/0x159610 early outs, frame_mode 1/2 vs the other path, passes 1,3,5,8,20 against the 0x44370 mode table
// (target, clear flags, cull mode and depth flag read from the retail table: all agree), the 0x25dc0 flag pairs, the
// local-player weapon scale chain with its ucomiss tests, 0x3fdf0/0x404b0 order, the finish/0x20770/0x13870 tail):
// no field, offset, condition, callee or argument difference left; every remaining difference is the 0x44370 inlining
// listed below. Tried again: the five inlined passes as draw_configured_pass_r18 calls (922).
// MiniMax R (11-01): no new form, no lower score.
// Earlier 331 (was 453; the fast check's score counts every line after a one-instruction shift, so most of it is
// shift): the prologue now matches to +0x48 (memset of the parameters, the identifier read first); draw_ui and
// finish are bool (caller 0x13680 pushes raw bools: 103 -> 16); the calls of 0x2c370 for 2..6 are
// written out as retail has them (a loop kept 0x1595d0 and 0x14ddc0 inlined); the 0x25dc0 call, the scale test and the
// tail match. Still different: (1) retail stores the constant 1 into the pass flag late, only after the
// cull call of pass 3; passes 1, 5, 20 have no such store, ours stores early in all of them (a
// helper variant without the extra gate for 1/5/20 removes those stores but then ours is 20 bytes short and
// the shift raises the score to 411-437); (2) the passes that never cull keep a dead load of g_485898 in
// retail (3 loads, same cause as 0x423c0); (3) function_14b60 takes its first argument in eax in retail
// (callee not matched); (4) the second 0x25dc0 call: retail builds the first flag with a branch diamond
// (mov dl,1 / jle / xor cl,cl / jmp / mov cl,1), ours folds it to setle (kept: `g_4c19ac <= 0`; a flag
// local set inside a test 327, bool/byte/long locals, if-else, ternaries, two calls (447, tail duplicated) 273-357); (5) 0x3ba90 returns long here but retail
// stores only its al (bool), which needs the definition in unknown_03bcb0.cpp changed to bool
// (-1 instruction, 2bcd0 354 only because the shift moves). Tried: a plain {0} parameter block (398, dead
// zero stores dropped), the helper-returned block (320 before the memset form). Round 38: the weapon tag read
// straight from the object table without the one-use object local (331, kept); `!(g_4c19ac > 0)` (331, still
// setle); the weapon object as a pointer whose tag is read at each use (331 positional, but it gives retail's esi
// object register: 20 -> 18 aligned differences); `if (g_4c19ac > 0) f(false, true); else f(true, true);` (421,
// tail duplicated); `bool last = function_3ba90(...) != 0` (352). BLOCKED-BY 0x44370 (pass flags, dead loads),
// 0x14b60 (a stub), 0x3ba90 (returns long).
// P599: Frame 0xc agrees. Cleanup displacement, clear-call transport and selected pass gates still differ.
// sole cleanup label 269.
// nested interruption guards 269.
// admission assigned by branch 414.
// selected mode arm first 269.
// inclusive mode range 381.
// non-2 result at entry 435.
// non-2 result before pass selection 432.
// non-2 result at first world phase 292.
// NOT kept: The cleanup label, nested guards and selected arm first tied. Other forms raised the score. Original admission and pass structure restored.
// MiniMax H (11-20): no new form, no lower score (269); the second 0x25dc0 call's flag diamond, the weapon tag read and the clear-call transport still differ (BLOCKED-BY 0x44370 and 0x14b60).
// @retail 0x2bcd0
void __stdcall function_2bcd0(long frame_mode, bool draw_ui, bool world_effects, bool finish,
    long context_mode, long filter_mode, real transition)
{
    long identifier = ++g_4ba03c;
    s_depth_parameters_13450 parameters;
    memset(&parameters, 0, sizeof(parameters));
    parameters.identifier = identifier;
    parameters.mode = filter_mode;
    parameters.distance = transition;
    function_13450(&parameters);
    if (!function_289e0() && !function_159610())
    {
        function_40e90(context_mode);
        if (frame_mode != 5)
        {
            if (frame_mode != 1 && frame_mode != 2)
            {
                draw_configured_pass_r18(1);
                draw_configured_pass_r18(3);
                draw_configured_pass_r18(5);
                draw_configured_pass_r18(2);
            }
            else
            {
                draw_light_source_pass(0, 1, true, true, 1, false);
                draw_selected_source_pass(0, 3, true, true, g_4858b8, false);
                draw_selected_source_pass(0, 5, false, false, g_4858b8, false);
                draw_selected_source_pass(0, 8, false, false, g_4858b8, false);
                draw_selected_source_pass(0, 20, false, false, g_4858b8, false);
                function_2c370(0);
                function_335e0(filter_mode);
                draw_configured_pass_r18(7);
                function_31c80();
                function_2c370(1);
                draw_configured_pass_r18(2);
                function_2c370(2);
                function_2c370(3);
                function_2c370(4);
                function_2c370(5);
                function_2c370(6);
            }
            if (frame_mode != 2)
            {
                if (g_48575c && filter_mode != 2)
                {
                    function_15370(2);
                    function_3ede0();
                    function_15370(0);
                }
                if (filter_mode != 2)
                {
                    function_25d50();
                    draw_configured_pass_r18(16);
                }
                g_509430 = 0;
                if (!((byte)g_4b9fe0 & 0x10))
                {
                    bool last = function_3ba90(6, &g_4c1a48[0]);
                    draw_configured_pass_r18(6);
                    function_3c3c0(true, last);
                }
                else function_3c3c0(false, false);
                if (filter_mode != 2)
                {
                    if (g_4b9f88) function_25dc0(true, true);
                    else if (g_4c19ac > 0) function_25dc0(true, false);
                    function_2c340(world_effects);
                }
            }
            function_c3e90();
            function_36b50();
            draw_configured_pass_r18(15);
            function_1595d0();
            function_42b20(-13);
            if (draw_ui) function_1e370();
            if (frame_mode != 2)
            {
                if (filter_mode != 2 && !g_4b9f88) function_25dc0(g_4c19ac <= 0, true);
                g_485a76 = 1;
                long local_player = g_4b9ed8;
                if (local_player != NONE && !function_155760(local_player) && function_14ddc0(local_player))
                {
                    long player = function_14de70(local_player);
                    long unit = *(long *)(g_4e8c24->data + (player & 0xffff) * 0x21c + 0x2c);
                    if (unit != NONE)
                    {
                        byte *unit_data = ((s_scalar_object_header *)g_4e0300->data)[unit & 0xffff].object;
                        long weapon = function_cbd50(unit, (signed char)unit_data[0x212]);
                        if (weapon != NONE)
                        {
                            long tag = *(long *)((s_scalar_object_header *)g_4e0300->data)[weapon & 0xffff].object;
                            if (tag != NONE)
                            {
                                byte *definition = g_4e3b44[tag & 0xffff].bytes;
                                if (*(real *)(definition + 0x314) != 0.0f && *(real *)(definition + 0x318) != 0.0f)
                                    function_2afa0((point2f *)(definition + 0x314));
                            }
                        }
                    }
                }
                function_3fdf0(context_mode, false);
                function_404b0(context_mode);
                function_3fdf0(context_mode, true);
                if (draw_ui) function_40870();
                g_485a76 = 0;
                if (filter_mode != 2 && world_effects) function_2dd30();
                if (finish && filter_mode != 2) draw_configured_pass_r18(17);
                if (filter_mode != 2)
                {
                    function_020770();
                    function_13870(world_effects, finish);
                }
            }
        }
    }
    function_13630();
}

extern byte *g_485a80;
real game_time_get_seconds();
typedef bool (__stdcall *t_34770_begin)(long);
typedef bool (__stdcall *t_34770_vertex)(long, real const *, real const *, real *, long);
typedef void (__stdcall *t_34770_end)(long);
void function_34420(t_34770_begin begin, real const *bounds, t_34770_vertex vertex, t_34770_end end, long user);
bool __stdcall function_29080(long context);
bool __stdcall function_296c0(long context);
bool __stdcall function_29370(long context);
bool __stdcall function_298f0(long mode, real const *bounds, real const *t, real *out, long unused);
void __stdcall function_2a8d0(long tag, long bitmap, real const *source, real const *destination, long mask, color3f const *color, real strength, bool blend, bool inverse, bool alternate_target, bool screen_target, bool simple, bool preserve_alpha);
void __stdcall function_29b00(long tag, long first_bitmap, real first_alpha, real first_angle, real first_scale, long second_bitmap, real second_alpha, real second_angle, real second_scale, bool screen_target);
void __stdcall function_2a2f0(volatile long tag, volatile real alpha);
PRIVATE __forceinline real filter_transition_time_289e0()
{
    return g_510c54 && g_510c54->active ? (real)g_510c54->game_time * g_510c54->rate : 0.0f;
}

void function_14ac0();

#if 0
// Retail 0x289e0
bool function_289e0()
{
    byte active = g_4670cc.changed; // Retail's active flag is at +1 in the current canonical declaration.
    if (active && g_485a80)
    {
        byte *settings = (byte *)g_485a80;
        long first_tag = *(long *)(settings + 0x1c), rings_tag = *(long *)(settings + 0x24);
        long last_tag = *(long *)(settings + 0x2c), slide_tag = *(long *)(settings + 0x3c);
        long overlay_tag = *(long *)(settings + 0x44);
        color3f const *color = (color3f const *)((byte *)g_4686cc + 4);
        g_4b99b0[0] = 1;
        g_4b99b0[1] = 0;
        *(color3f *)(g_4b99b0 + 4) = *color;
        *(real *)(g_4b99b0 + 0x10) = *(real *)(g_4b99b0 + 0x14) = *(real *)(g_4b99b0 + 0x18) = 0.699999988079071f;
        *(real *)(g_4b99b0 + 0x1c) = *(real *)(g_4b99b0 + 0x20) = 0.05000000074505806f;
        *(real *)(g_4b99b0 + 0x24) = *(real *)(g_4b99b0 + 0x28) = 0.0f;
        if (g_4670cc.active)
        {
            *(long *)(g_4b99b0 + 0x34) = 1;
            *(long *)(g_4b99b0 + 0x38) = 3;
            function_14bc0(1, 0, false);
            function_14b60(1, 0, 1.0f, 0);
            if (g_4b99b0[1])
            {
                function_14bc0(*(short *)(g_4b99b0 + 0x38), 0, false);
                if (g_485607 && !g_4858b8) function_14ac0();
                D3DDevice_Clear(0, 0, 0xf0, 0x8080ff, 1.0f, 0);
            }
        }
        real fraction = 0.0f;
        real *override_value = (real *)g_4670cc.index;
        if (override_value) fraction = *override_value < 0.0f ? 0.0f : *override_value > 1.0f ? 1.0f : *override_value;
        else if (g_4670cc.output.value > 0.0f)
        {
            real elapsed = filter_transition_time_289e0() - g_4670cc.start.value;
            if (elapsed < 0.0f) elapsed = 0.0f;
            else elapsed = game_time_get_seconds() - g_4670cc.start.value;
            fraction = elapsed / g_4670cc.output.value;
            fraction = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
        }
        real progress = function_288e0(fraction);
        *(real *)(g_4b99b0 + 0x48) = progress;
        if (progress > 0.15000000596046448f && g_4670cc.elapsed.value == 0.0f) g_4670cc.elapsed.value = filter_transition_time_289e0();
        real fade = 1.0f - progress * progress;
        fade = fade < 0.0f ? 0.0f : fade > 1.0f ? 1.0f : fade;
        real rectangle[4] = {0.0f, 640.0f, 0.0f, 480.0f};
        real sliding[4] = {(1.0f - progress) * 640.0f, (1.0f - progress) * 640.0f + 640.0f, 0.0f, 480.0f};
        *(real *)(g_4b99b0 + 0x2c) = (real)pow((double)progress, 3.0) - 0.25f;
        real center = progress * 2.0f - 1.0f;
        center = 1.0f - center * center;
        *(real *)(g_4b99b0 + 0x30) = 1.0f - center * center * 0.6600000262260437f;
        function_2a8d0(first_tag, 0, rectangle, rectangle, 0, color, 1.0f, false, false, false, false, false, false);
        real first_angle = fade * 0.2666666805744171f, second_angle = fade * 0.20392157137393951f;
        for (long i = 0; i < 3; ++i)
        {
            real phase = 0.0f;
            real started = g_4670cc.elapsed.value;
            if (started != 0.0f)
            {
                real elapsed = filter_transition_time_289e0() - started;
                if (elapsed < 0.0f) elapsed = 0.0f;
                else elapsed = filter_transition_time_289e0() - started;
                real period = (started - g_4670cc.start.value) * 1.100000023841858f + (real)i * 0.5f;
                if (period > 0.0f)
                    phase = period >= elapsed ? (real)pow((double)elapsed / period, 2.0) : (elapsed - period) / period * 2.0f + 1.0f;
                else phase = elapsed * 2.0f;
            }
            real size = 0.7199999690055847f / (1.0f - progress * 0.699999988079071f) / (1.0f + (real)i * progress * 2.0f);
            real first_alpha = phase * -0.6283185482025146f * ((real)i + 1.0f) * 1.100000023841858f;
            real second_alpha = phase * 0.41887903213500977f * ((real)i + 1.0f) * 1.100000023841858f;
            function_29b00(last_tag, 0, second_angle, second_alpha, size, 1, first_angle, first_alpha, size, false);
        }
        function_2a8d0(rings_tag, 0, sliding, rectangle, 0, color, 1.0f, false, true, false, false, false, false);
        if (g_4b99b0[0] || g_4b99b0[1])
        {
            *(real *)(g_4b99b0 + 0x3c) = 0.0f;
            g_4b99b0[0x40] = 0;
            g_4b99b0[0x41] = g_4b99b0[1];
            *(long *)(g_4b99b0 + 0x44) = *(long *)(g_4b99b0 + 0x34);
            function_34420(function_29080, 0, function_298f0, 0, 0);
            *(real *)(g_4b99b0 + 0x3c) = 0.0f;
            g_4b99b0[0x40] = g_4b99b0[0x41] = 0;
            *(long *)(g_4b99b0 + 0x44) = 0;
            function_34420(function_296c0, 0, function_298f0, 0, 0);
            function_2a8d0(slide_tag, 0, 0, 0, -14, color, fade, true, false, false, true, true, false);
            function_2a2f0(overlay_tag, fade);
            if (g_4b99b0[0])
            {
                *(real *)(g_4b99b0 + 0x3c) = 1.0f;
                g_4b99b0[0x40] = g_4b99b0[0x41] = 0;
                *(long *)(g_4b99b0 + 0x44) = *(long *)(g_4b99b0 + 0x38);
                function_34420(function_296c0, 0, function_298f0, 0, 0);
                *(real *)(g_4b99b0 + 0x3c) = 2.0f;
                g_4b99b0[0x40] = 1;
                g_4b99b0[0x41] = 0;
                *(long *)(g_4b99b0 + 0x44) = *(long *)(g_4b99b0 + 0x34);
                function_34420(function_29370, 0, function_298f0, 0, 0);
            }
        }
        active = g_4670cc.changed;
        g_4670cc.active = 0;
    }
    if (!g_4670cc.index && *(real *)(g_4b99b0 + 0x48) == 1.0f)
    {
        memset(&g_4670cc, 0, sizeof(g_4670cc));
        memset(g_4b99b0, 0, sizeof(g_4b99b0));
        return g_4670cc.changed != 0;
    }
    return active;
}
#endif

byte g_509414;
byte *g_4c5008;
extern vector3f *g_4687b4;
bool function_3d7c0(long object_index, bool *out);
byte *__stdcall function_3dc90(long object_index);
bool function_32ac0();
void __stdcall function_31f50(long index, point3f const *origin, vector3f const *axis, real *radial, real *axial);
struct s_light_definition_ab;
void function_30ca0(bool flag, long mode);
void function_c1930(s_light_definition_ab const *definition, s_light_shape_ab *shape);
void function_c1a80(s_light_frame_ab const *frame, s_light_shape_ab *shape, s_light_definition_ab const *definition);
struct s_visibility_object_lists;
void visibility_object_lists_update_flags(s_visibility_object_lists *lists, long object, bool value, dword flags);
void __stdcall function_2cbc0(bool enabled);
void function_222a0();
void function_4a780();
PRIVATE __forceinline void shadow_cross_32350(vector3f const &a, vector3f const &b, vector3f *out)
{
    real x = a.j * b.k - a.k * b.j;
    real y = a.k * b.i - a.i * b.k;
    real z = a.i * b.j - a.j * b.i;
    out->i = x;
    out->j = y;
    out->k = z;
}

PRIVATE __forceinline real absolute_32350(real value)
{
    return value >= 0.0f ? value : 0.0f - value;
}

// MiniMax I (11-20): no new form, no lower score (429): this round only re-measured the body; the constant 1 that retail holds in ebx through the inlined pass and the result byte that shares that pass's enabled slot both come from 0x44370 being inlined with its mode table folded, and that is blocked by 0x44370 (src/unknown_01cf50.cpp); what still differs is retail's smaller frame, its five epilogue copies and its ja/jbe diamond for |x| > |y|.
// MiniMax B (11-19): no new form tried this round, no lower score (437): the constant 1 that retail holds in ebx
// through the inlined pass and the result byte that shares that pass's enabled slot both come from 0x44370 being
// inlined with its mode table folded, and that is blocked by 0x44370 (src/unknown_01cf50.cpp); what still differs is
// retail's smaller frame, its five epilogue copies and its ja/jbe diamond for |x| > |y|.
// MiniMax I (11-15): no new form, no lower score (445): the constant 1 that retail holds in ebx through the
//   inlined pass and the result byte that shares that pass's enabled slot both come from 0x44370 being inlined
//   with its mode table folded, and that is blocked by 0x44370. What still differs: retail pushes esi/edi only
//   after the early exits and its frame is four bytes smaller; retail stores the byte returned on the failure
//   exits at entry and reads it back from memory, so it has five epilogue copies where this build folds the exits
//   together; retail's |x| > |y| keeps both absolute values and re-evaluates the winner in each arm (a ja/jbe
//   diamond) where this build selects with a move; 0x4a1e0 takes its three arguments in eax/ecx/edx in retail.
// MiniMax G (11-20): no new form, no lower score (429): the frame is still 0xb34 against retail's 0xb40; retail's zeroed byte sits just below the alpha float and its callee flag is a separate byte above it, where this build has one byte that is both, and declaring the flag before the float only puts the two initialisations in retail's order for the same 429; retail's inlined pass is 0x44370 in another file: BLOCKED-BY 0x44370.
// score 416 (was 445): frame 0xb34 versus 0xb40, and the false result and second query still use different homes.
// MiniMax A (11-20): score 416 (was 437): the special flag is initialised to false before the 0x3d7f0 call, which
// stores the zero retail stores; retail's zero byte sits at +3 and its special flag at +0xb, ours at +7.
// Earlier 445 (was 448): r136 lane 7: only through the shared draw_light_source_pass change (pass 11's stipple cull and
// late enable); the notes below were written at 491.
// Earlier 491 (was 492, 623, 506; 491 since 0x31170 sets g_4ba014 bits through bit_vector_set): retail returns a separate false byte, not the special flag (`mov al, [esp+0xb]` on every
// non-success exit; the flag 3d7f0 writes is the byte at [esp+0x13]), so a special object returns false; the draft
// returned the flag. 31f50's 5th output feeds the depth, its 4th the *2 field (the draft had them swapped), the
// two cross products are separate branches (cmov before), the frame points come from g_47fd18+0xc, and the final
// pass is draw_light_source_pass. A block-by-block walk of the rest against retail (2026-10-31) found no further
// behaviour difference: constants (0.0002, 0.09, 0.9, 0.5, 2.0), field offsets, call argument orders, both
// cross-product arms, the box fields and the g_509413/g_509414 stores all agree. The two pass bits are tested as
// `!(bool)((~(flags >> n)) & 1)` (retail `shr; not; test`), as in 0x3d380. Still differs: (1) retail pushes
// esi/edi only after the early exits, ours at entry, and its frame is 4 bytes smaller; the byte returned on the
// failure exits is stored at entry and read back from memory in retail (the inlined pass's `enabled` shares its
// slot), ours folds it to `xor al, al` and merges the exits, which is why retail has five epilogue copies;
// (2) 0x4a1e0 takes source/camera/resource in eax/ecx/edx in retail (callee unmatched, same as 0x31170);
// (3) retail holds the constant 1 in ebx for the whole inlined pass (`test bl, dl`, `and al, bl`, `mov [..], ebx`),
// ours uses immediates; (4) retail's |x| > |y| keeps both absolute values and re-evaluates the winner in each arm
// (ja/jbe diamond), ours selects with a move (ternary and if/else compile the same, 523 with the origin change).
// Tried: the result as a volatile local byte (482; not kept, as it doesn't make the function match), if/else
// statements for the larger magnitude (531, ternary 524 before the frame-point change), `!(bit) || !(bit)` and
// `== 0` spellings of the gate (same shr/test form), loading the origin once (`*(point3f *)g_47fd18 =
// *(point3f *)(g_47fd18 + 0xc) = *origin;` gives retail's exact store sequence, 527; through a local copy 608
// with the cross arms laid out the other way round; later xmm numbers move, so the score rises), every order of
// the three dot-product terms (the compiler picks k, j, i anyway).
// Also tried 2026-11-01: a single exit (`result = true` on success, the g_509413/g_509414 stores once after the
// `!special` block, `else if` for the special arm, one `return result`; 552 in a fast check, 1758 bytes: the exits
// merge and result stays in al); absolute_32350 as a function-like macro (497: gives retail's two abs diamonds and
// the re-evaluated arms, but the registers differ and the listing shifts), the same with `largest` as one
// conditional expression (497); `bool result = false;` declared first (492); `fabs(dot3f(&forward, axis))` (492).
// The constant 1 that retail keeps in ebx through pass 11 and the result byte sharing that pass's `enabled` slot
// both come from 0x44370 inlined there with its mode table folded (retail's table at 0x43fa1c is in .rdata;
// ours, g_render_modes_storage, is a non-const global; making it const changes nothing here and 0x44370 still stays out of
// line, 842 for 0x423c0 with every pass calling it), BLOCKED-BY 0x44370.
// P599: The shape initializer clears 0x7c bytes. The local still reserves 0x74.
// 0x7c shape extent 445.
// eligibility query scope 445.
// object-flag query scope 445.
// depth calculation scope 445.
// eligibility and object-flag scopes 445.
// eligibility and depth scopes 445.
// object-flag and depth scopes 445.
// single return with completion stores 514.
// shared box center displacement 437.
// NOT kept: the 0x7c shape reaches frame 0xb3c and ties 445. The scopes tie and one return regresses.
// Kept: one displacement used for all three box center products, 437 in the ordinary check.
// P603. Complete typed shape 437, typed projections 437, both typed records 437,
// both records with extents first 436, one extent-vector initializer 503, staged box cross 437,
// staged light and box crosses 528, or renamed query and result locals 437.
// NOT kept: the full shape gives frame 0xb3c, still four bytes short, and the best variant
// gains one. The original 0x74-byte buffer remains, against the initializer's 0x7c-byte clear.
// MiniMax A (11-20): no new form, no lower score (437); the frame is still 0xb34 against retail's 0xb40 because the shape initializer clears 0x7c bytes against the 0x74-byte buffer the source reserves, and the box center's one shared displacement stays.
// MiniMax A (11-18): no new form, no lower score (437); what still differs is the frame (0xb34 against retail's 0xb40), where
// the shape initializer clears 0x7c bytes against the 0x74-byte buffer the source reserves, and the box center's one shared
// displacement; the typed shape and the extents-first records gain one point each and are not kept.
// @retail 0x32350
bool __stdcall function_32350(long index, real input_alpha)
{
    byte *object = ((s_scalar_object_header *)g_4e0300->data)[index & 0xffff].object;
    bool result = false;
    real alpha = 0.0f;
    bool special = false;
    bool eligible = function_3d7f0(index, &alpha, &special);
    alpha *= input_alpha;
    if (!eligible || *(long *)(object + 0x14) != NONE || !g_4ba020) return result;
    bool object_flag;
    if (!function_3d7c0(index, &object_flag)) return result;
    if (!special)
    {
        real radial = *(real *)(object + 0x3c) > 0.00019999999494757503f ? *(real *)(object + 0x3c) : 0.00019999999494757503f;
        real axial = radial;
        byte *lighting = function_3dc90(index);
        bool object_type = object[0xaa] != 1;
        if (!lighting || !(*(real *)(lighting + 0x1c) > 0.0f)) goto failed;
        alpha *= *(real *)(lighting + 0x1c);
        if (!(alpha > 0.09000000357627869f)) goto failed;
        {
            vector3f const *light_direction = (vector3f *)(lighting + 0xc);
            point3f const *origin = (point3f *)(object + 0x30);
            function_31f50(index, origin, light_direction, &radial, &axial);
            *(point3f *)(g_47fd18 + 0xc) = *origin;
            *(point3f *)g_47fd18 = *origin;
            vector3f &forward = *(vector3f *)(g_47fd18 + 0x28);
            forward = *light_direction;
            function_30bf0(&forward);
            real largest;
            if (absolute_32350(forward.i) > absolute_32350(forward.j)) largest = absolute_32350(forward.i);
            else largest = absolute_32350(forward.j);
            real depth = axial * 2.0f + largest;
            *(real *)(g_47fda0 + 0x2c) = depth - axial;
            *(real *)(g_47fda0 + 0x30) = depth + axial - axial;
            *(real *)(g_47fda0 + 0x20) = radial * 2.0f;
            vector3f const *axis = g_4687ac;
            vector3f &up = *(vector3f *)(g_47fd18 + 0x34);
            if (fabs(forward.k * axis->k + forward.i * axis->i + forward.j * axis->j) < 0.8999999761581421f)
                shadow_cross_32350(forward, *axis, &up);
            else
                shadow_cross_32350(forward, *g_4687b4, &up);
            function_30bf0(&up);
            byte shape[0x74];
            function_c1930((s_light_definition_ab *)g_47fda0, (s_light_shape_ab *)shape);
            function_c1a80((s_light_frame_ab *)g_47fd18, (s_light_shape_ab *)shape, (s_light_definition_ab *)g_47fda0);
            s_320b0_box box;
            box.matrix.scale = 1.0f;
            box.matrix.forward = forward;
            shadow_cross_32350(up, forward, &box.matrix.left);
            box.matrix.up = up;
            box.radius.i = *(real *)(g_47fda0 + 0x30) * 0.5f;
            box.radius.j = *(real *)(shape + 0x58) * 0.5f;
            box.radius.k = *(real *)(shape + 0x5c) * 0.5f;
            point3f center;
            point3f const &second_point = *(point3f *)(g_47fd18 + 0xc);
            real displacement = 0.0f - box.radius.i;
            center.x = second_point.x + box.matrix.forward.i * displacement;
            center.y = second_point.y + box.matrix.forward.j * displacement;
            center.z = second_point.z + box.matrix.forward.k * displacement;
            box.matrix.position.x = *(real *)(g_47fd18) - center.x + second_point.x;
            box.matrix.position.y = *(real *)(g_47fd18 + 4) - center.y + second_point.y;
            box.matrix.position.z = *(real *)(g_47fd18 + 8) - center.z + second_point.z;
            if (!function_320b0(&box)) goto failed;
            byte projections[6 * 0x1bc];
            short projection_count;
            g_4858bc = 0;
            *(real *)(g_47fd18 + 0x74) = alpha;
            function_c1d90((s_light_frame_ab *)g_47fd18, (s_light_shape_ab *)shape, (s_light_projection_ab *)projections, &projection_count);
            real radius = (real)sqrt(box.radius.k * box.radius.k + box.radius.j * box.radius.j + box.radius.i * box.radius.i);
            function_1320f0(2, projection_count, false, 0, projections, (byte const *)&box.matrix.position, radius, &g_547f88, 0, *(short *)(object + 0x2c));
            visibility_object_lists_update_flags((s_visibility_object_lists *)&g_54aa68, index, false, 0);
            function_2cbc0(object_type);
            function_44720();
            g_4ba018 = 1;
            g_4ba014 = 0;
            if (*(short *)((byte *)g_54aa68.lists[0] + 4) > 0)
            {
                g_4c5008 = g_47fda0;
                g_4ba014 = 0x14;
                function_30ca0(false, 5);
                function_4a1e0((s_view_source *)g_47fd18, (s_view_camera *)shape, NONE,
                    (s_view_flags *)g_47fda0, 1.0f, true, 1.0f);
                function_48ee0(g_47fd18, shape, g_47fda0);
                g_485a30 = alpha;
                function_40e90(0);
                if (!(bool)((~(g_4c1a48[2].flags >> 10)) & 1) && !(bool)((~(g_4c1a48[1].flags >> 11)) & 1))
                {
                    draw_configured_pass_r18(10);
                    function_222a0();
                    draw_light_source_pass(1, 11, false, false, g_4858b8, true);
                }
                function_4a780();
            }
            g_4ba018 = 0;
            g_4ba014 = 0;
            g_509413 = 1;
            g_509414 = 0;
            return true;
        }
    failed:
        g_509413 = 1;
        g_509414 = 0;
        return result;
    }
    if (g_509413)
    {
        if (function_32ac0())
        {
            g_509414 = 1;
            g_509413 = 0;
            function_32ce0(index, alpha);
        }
        else g_509414 = 0;
    }
    else if (g_509414) function_32ce0(index, alpha);
    return result;
}
