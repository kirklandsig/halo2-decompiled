// @flags /O2 /Ob1 /Gr /arch:SSE
#include "orientations_storage.h"
/* UNKNOWN_141590.CPP: transform4x3f and quaternion math */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "normalize3d.h"
#include <math.h>
#include <string.h>



vector3f *function_11d000(vector3f const *v, vector3f *out);

// @retail 0x141590
void function_141590(
	transform4x3f const *in,
	transform4x3f *out)
{
	if (in->scale != 0.f)
	{
		real z, y, x;
		real t;
		x = 0.f - in->position.x;
		y = 0.f - in->position.y;
		z = 0.f - in->position.z;
		if (in->scale != 1.f)
		{
			real const &inverse = 1.f / in->scale;
			out->scale = inverse;
			x = (real)(x * inverse);
			y = (real)(y * inverse);
			z = (real)(z * inverse);
		}
		else
		{
			out->scale = 1.f;
		}
		out->forward.i = in->forward.i;
		out->left.j = in->left.j;
		out->up.k = in->up.k;
		t = in->left.i; out->left.i = in->forward.j; out->forward.j = t;
		t = in->up.i; out->up.i = in->forward.k; out->forward.k = t;
		t = in->up.j; out->up.j = in->left.k; out->left.k = t;
		out->position.x = out->up.i * z + out->forward.i * x + out->left.i * y;
		out->position.y = out->forward.j * x + out->up.j * z + out->left.j * y;
		out->position.z = out->forward.k * x + out->up.k * z + out->left.k * y;
	}
	else
	{
		memset(out, 0, sizeof(transform4x3f));
	}
}

// @retail 0x1416c0
void function_1416c0(
	point3f const *position,
	transform4x3f *out)
{
	out->scale = 1.f;
	out->forward.i = 1.f;
	out->forward.j = 0.f;
	out->forward.k = 0.f;
	out->left.i = 0.f;
	out->left.j = 1.f;
	out->left.k = 0.f;
	out->up.i = 0.f;
	out->up.j = 0.f;
	out->up.k = 1.f;
	out->position = *position;
}

// @retail 0x141710
void matrix4x3_from_forward_and_up(
	transform4x3f *out,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
}

real function_30bf0(vector3f *v);
bool function_143120(vector3f const *forward, vector3f const *left, vector3f const *up);

quaternionf *g_4687cc;
extern transform4x3f *g_4687d0;

/* rotates a vector by a unit quaternion */
static inline void quaternion_transform_vector(
	quaternionf const *q,
	vector3f const *v,
	vector3f *out)
{
	real dot = (q->i * v->i + q->j * v->j + q->k * v->k) * 2.f;
	real w2 = q->w * 2.f;
	real s = q->w * q->w * 2.f - 1.f;
	vector3f cross;

	cross.i = q->j * v->k - q->k * v->j;
	cross.j = q->k * v->i - q->i * v->k;
	cross.k = q->i * v->j - q->j * v->i;
	out->i = v->i * s + q->i * dot + cross.i * w2;
	out->j = v->j * s + q->j * dot + cross.j * w2;
	out->k = v->k * s + q->k * dot + cross.k * w2;
}

/* the rotation that takes one unit vector to another */
// score 200 (was 200): Frame 0x1c versus 0x24 and quaternion/cross spill overlap remain.
// Earlier (200, was 237): Frame 0x1c versus 0x24 and quaternion/cross spill overlap remain.
// Round 32 (second pass): retail's frame is 0x24 and its quaternion sits at [esp+0x24..0x34],
// the same absolute homes as ours, but retail's cosine half is the frame bottom local [esp+0x10] and its sine half
// lands in the matrix parameter's home slot [esp+0x38], so the halves are two separate locals and not a struct.
// Tried: the halves as plain locals 290 and with the clamp in place 290 (the sine half does take the parameter
// home, but our frame then shrinks to 0x18 and the transform spills one slot instead of retail's three); the axis
// components as named locals 282 and 325; a result vector in the transform helper 367 (frame grows, code grows).
// Frame 0x1c versus 0x24 and quaternion/cross spill sharing remain.
// Earlier (237, was 237): Round 32: retail's frame is 0x24 against our 0x1c; retail keeps the clamped cosine and then
// Earlier 237 (was 237): Round 32: retail's frame is 0x24 against our 0x1c; retail keeps the clamped cosine and then
// the sine half in the matrix parameter's home slot ([esp+0x38]) and the cosine half at the bottom local
// ([esp+0x10]), while ours keeps both halves in the two-member struct at [esp+0x14]/[esp+0x18]; retail holds the
// quaternion in xmm2/xmm1/xmm0 (i/j/k) where ours uses k/i/j. Tried: cross products with the vector operand first
// 237 (identical - VC commutes the products); the axis components as named locals 344; the scale before the
// crosses 325.
// Earlier (237, was 237): Frame 0x1c versus 0x24 and quaternion/cross spill overlap remain.
// Earlier 237 (was 237): Frame 0x1c versus 0x24 and quaternion/cross spill overlap remain.
// Frame 0x1c versus 0x24 and quaternion/cross spill overlap remain.
// Lane 6 r85: helper with `s` computed first 302 (s and the doubled scalar hoisted above the matrix copy,
// the doubled scalar spilled twice as retail, frame 0x20, constants one xmm lower); with vector-first
// products 302 (identical code); with plain half-angle locals 291; doubled scalar after the cross 301;
// scalar cross terms 281 (retail's 0x24 frame and first difference +0x71, doubled scalar in the parameter
// home and [esp+0x10] as retail; quaternion registers permuted); scalar cross with `s` first 306.
// frame 0x1c versus 0x24 and quaternion/cross-term spill reuse differ;
// quaternion scale loop 302; shared axis scale loop 294.
// shared rotation cross 241; shared cross scale loop 294.
// shared cross both phases 281; identity library copy 237.
// per component scale ratio 290; scaled axis before stores 289.
// Frame 0x1c versus 0x24 and quaternion/cross-term spill reuse remain.
// rotation output ikj 283; rotation output jik 287; rotation output jki 288; rotation output
// kij 290; rotation output kji 290.
// Frame 0x1c versus 0x24 and quaternion/cross spills remain; construction scope 237; vector-first helper 240; output-middle helper 290; cross i/k/j 284, j/i/k 235 (only -2), k/i/j 284. Both helpers restored.
// Sine-branch construction j/k/i 327, k/i/j 240; initial dot products
// named 324; both half-angle radicands named 251; orthogonal early exit 237.
// Native vector input contract 403; quaternion/vector reference formals 237/237;
// projection sum named 246; both half-angle sum/difference parts named 281. Body and helpers restored.
// Per-row projection terms 311; scaled-input terms 282; scaled-cross terms 292; named direct sums 278; named
// scalar square 288. Frame/half-angle/quaternion spill sharing remains; both original helpers restored.
// Native quaternion snapshot 458 (frame 0x24); all dot products named 288; all cross input products named 292;
// sine-branch construction j/i/k 310, i/k/j 240. Frame/quaternion spill reuse remains; body and both helpers
// restored.
// Axis orders flu/ufl/ulf/lfu/luf 301/237/237/301/238;
// flu/ufl/ulf with sine-half first 301/240/240.
// Separate quaternion axis/scalar inputs: axis-first/scalar-first/reference
// 246/246/246; native axis value 353; the actual output-pointer return gives
// 246/246/246/353 respectively. Original helpers and body restored; frame
// 0x1c/0x24 and half-angle/quaternion/cross spill overlap remain.
// Native quaternion i/j/k observations, with vector/scalar cross storage 282/294;
// with native input components 348/284; complete i/j/k/w observations 282/302;
// complete quaternion and input components 346/284. Original helpers restored.
// Round 172: Dot/scalar/doubled-scalar and doubled-scalar/dot/scalar schedules 285/240; first
// schedule with input+projection/input+cross/projection+cross/all row parts 281/291/299/281; second
// schedule with all parts/direct sums 281/283. Every row stays symmetric; original helper and body
// restored. Frame 0x1c versus 0x24 and half-angle/quaternion/cross spill sharing remain.
// Lane 8 r58: plain half-angle locals 291 (sine half then in the parameter home as retail);
// cross term first in the sums 291; helper result through a local vector 367 (plain 361);
// helper in 0x11cb00's rotate-point operand/statement form 272 at 1317 bytes (retail's
// cross/scalar spills and doubled w2 copies, quaternion registers permuted); with plain
// halves 280; with sine-branch i/k/j, j/i/k, k/j/i 274/263/295; swapped state 272.
// Round 94: retail never reuses the quaternion's 16 bytes for the transform spills (ours spill the
// cross terms into it) and keeps the clamped cosine, sine half and doubled scalar in the matrix
// parameter's slot. Plain half-angle locals in place of the two-member struct 291 (frame 0x18);
// transform cross orders jki/kji/jik/kij 286/290/235/284; transform into a result vector 367, with
// jik 367. Body and helpers restored.
// Round 356: Existing inline point-rotation producer 272; independent half angles 280;
// shared axis cross 251; independent halves/shared axis 239; with shared repair crosses
// the same four forms 272/280/251/239. Original helper, paired halves and both cross phases restored.
// Round 253: Cosine half owned by the quaternion scalar, cosine/sine-first 291/293;
// with shared cross 265/267; shared double-factor scale 311/311 (1317 bytes);
// both shared products 327/327. Original body and helpers restored.
// Round 210: Scalar crosses with coefficient orders dot/s/w2, w2/dot/s, w2/s/dot,
// s/dot/w2, s/w2/dot: 274/274/272/306/235; cross-first schedules 282/297/304.
// Best 235 gains two, rejected; original helper/body restored.
// Round 80: plain half-angle locals 291 (sine half in the parameter home; frame 0x18). The existing
// quaternion_rotate_point for the three transforms 272 (1317 bytes, frame 0x20, xmm numbering one
// lower than retail's from entry: 1.0 in xmm6 not xmm7); with the axis scaled at construction 342;
// with `0.f > cosine` 272; with rotation.w assigned before the axis 257 (1317 bytes). Original body
// and helpers restored.
// Round 82: sine-branch axis k/j/i 327; transform scalars s/w2/dot 289, s/dot/w2 302; w assigned
// before the axis 302; scale computed before the axis 301. Original body and helpers restored.
// Round 259: Complete native cross-vector initializer with all six coefficient
// schedules 237/285/240/240/302/289; immutable cross with dot/w2/s and s/w2/dot
// 237/289. Original helper/body restored; frame and quaternion/cross spill reuse remain.
// Round 25: paired native point storage 237; independent scalar crosses on it 281;
// shared axis construction with scalar crosses 237 (frame 0x24, first +0x7d);
// pre-copy doubled scalar/quadratic scalar 302/347; scalar-first construction 272;
// shared input dot and shared repair crosses on the shared-axis base 237 each.
// Original body and both helpers restored.
// Round 258: output/vector/quaternion contract with original/scalar-cross/scale-first/
// both forms 237/281/302/306; vector/output/quaternion contract with those forms
// 240/237/298/311. Original helper, three call roles and body restored.
// Round 228: two-argument in-place helper with pointer/reference quaternion and vector
// contracts 290/290/290/290; each with independent cross scalars 313/313/313/313.
// Original helper contracts and complete body restored.
// Round 242: Complete shared scale/add-scaled output pipeline with staged-cross/shared-dot/
// independent-half subsets 0..7 363/363/358/361/355/356/349/350. Native half storage recovers frame
// 0x24 but grows code; original helper and body restored, quaternion/cross spill overlap remains.
// Round 255: shared real construction scale with independent-half/scalar-cross/staged-axis masks
// 0..7 237/291/281/293/200/267/265/252. Staged axis plus real scale kept: 200 plain,
// 1321/1317 bytes. Five further coefficient orders 256/205/205/257/260; scalar before
// cross/ratio/scale 310/200/200. Frame and quaternion/cross spill homes remain.
// Round 50 (lane 1, fast; score/bytes/register-blind aligned, base 200/1321/210): retail computes
// s = 2w*w - 1 above the identity copy, the doubled scalar after the dot (2.0 reloaded) and spills two
// copies each of the doubled scalar and s (frame 0x24). A copy of 0x11cb00's quaternion_rotate_point
// (a contributor's file) 258/1303/180, with plain halves 201/1324/115; plain halves alone 267/1297.
// This file's helper (used here only) as s, w2, dot with rotate-point operand forms and scalar crosses,
// plain halves 247/1317/90 (closest code: entry and transform-1 registers right, frame 0x20); retail's
// row sum order the same 247; vector3f cross 270/1297; struct halves 273/1296; s, dot, w2 248/1305/183
// (entry xmm numbering then matches); s, dot, crosses, w2 245/1312/185; `2.f * q->w` 248; doubled
// scalar inline at each use 326/1245, with the doubled dot inline too 336/1285; `cross * q->w * 2.f`
// 269/1326; dot, w2, s 252 (frame 0x24, nothing above the copy); dot, s, w2 250/1317/94; s, w2,
// crosses, dot 253/1304; s, crosses, dot, w2 247/1308/229; crosses inline in the rows 345/1222.
// Round 258: isolated quaternion scalar observation with six coefficient schedules 246/265/257/249/272/269;
// native scalar crosses on dot/w2/s and s/dot/w2 276/282. Original helper and body restored;
// frame 0x1c versus 0x24 and quaternion/cross spill sharing remain.
// Round 263: Both library cosine clamp orders, each with neither/quaternion/matrix/both identity copies, score
// Round 263: Both library cosine clamp orders, each with neither/quaternion/matrix/both identity copies, scoring
// Round 263: Both library cosine clamp orders, each with neither/quaternion/matrix/both identity copies,
// each gave 332. Original clamp, native identity assignments, staged construction and real scale restored;
// 332 each. Original clamp, native identity assignments, staged construction and real scale restored;
// frame 0x1c/0x24 and quaternion/cross spill sharing remain.
// Round 267 on staged construction/real scale: transform cross direct/staged 241/252; shared dot
// with written/direct/staged cross 246/243/254; independent halves with direct/staged/staged-dot
// 208/209/250. Frames remain 0x1c or shrink to 0x18; original helper and half storage restored.
// Round 49 (lane 11): retail computes the transform scalar s = 2w*w - 1 interleaved with the identity copy's
// setup (before rep movsd) and 2w in place on w after it; its first row sums cross*2w + v*s + q*dot, the other two
// v*s + q*dot + cross*2w, and its dot sums i, k, j. Fast: only the first row written cross-first 200 (identical
// code: VC reassociates); the helper computing s first, then dot and 2w, 257 (1305 bytes; s and 2w both hoisted
// above the copy); s first and 2w after the cross 277 (1331 bytes). Plain: the three transforms through the
// existing quaternion_rotate_point on the staged-cross/real-scale construction 258 (1303 bytes). 0x11d000 writing
// through a named result pointer (still matched) 200: its summary does not keep the quaternion's slots unshared.
// Round 285: Native direct-plus-projection vector with final shared scaled-cross addition,
// ordinary/shared dot and ordinary/staged cross, cross-first/direct-first:
// 305/349/309/336/295/338/321/330. Frame and quaternion/cross spill overlap remain; complete helper and
// body restored.
// Round 278: Complete independent scalar output production in six row orders 333 each; with staged
// transform cross 346, independent half angles 343. Scalar outputs recover frame 0x24 in the first seven
// but grow code; original helper, half storage and body restored. Frame 0x1c/0x24 and quaternion/cross
// spill overlap remain.
// Round 30 (lane 2, fast): retail computes the transform's scalar (w*w*2 - 1) before the identity copy and the
// doubled w after the first dot, and gives the cross terms slots of their own (ours spill them into the dead
// quaternion). Transform dot in i/k/j text order 200 (same code); scalar cross terms 265 (1300 bytes, frame
// 0x1c); the scalar computed once by the caller before the identity copy and passed to the helper 294 (frame
// 0x18: the doubled w then also hoists above the copy), with scalar cross terms 309; the helper in retail's
// computation order (scalar, i/k/j dot, doubled w, cross j/k/i, cross-term-first rows) 272 (frame 0x20).
// Round 34 (m3 lane 2, fast): plain halves named `w` and `sine_half` 267 (1297 bytes); the struct's members
// swapped 200 and the struct renamed `halves` 200 (both the same code: these names do not move its slots);
// `2.f * sine_half * cosine_half` 200 (same); the scale computed before the cross product 252 (1317 bytes,
// retail's length, stack-masked aligned 259 against 229).
// Round 46 (lane 14, fast): retail's rows group as i (cross + scalar) + dot, j/k (scalar + dot) + cross; only the
// i row written cross-first 200 (same code: the sums are reordered by the compiler), with the i/k/j dot 200 (a
// repeat of round 30's form), with s/dot/w2 order 257 (1305 bytes); state declared before rotation 200; rotation
// renamed quaternion 200; state renamed halves 200; its members swapped (sine half first) 200; both 200.
// Round 278: Complete independent scalar output production in six row orders 333 each; with staged
// transform cross 346, independent half angles 343. Scalar outputs recover frame 0x24 in the first seven
// but grow code; original helper, half storage and body restored. Frame 0x1c/0x24 and quaternion/cross
// spill overlap remain.
// Round 289: existing complete vector assignment output with staged-cross/shared-dot/independent-half
// masks 0..7 gives 333/346/340/351/344/360/333/368. Paired halves recover frame 0x24 but grow code;
// independent halves give 0x20. Original helper/body restored; quaternion/cross spill sharing remains.
// Round 118: transform helper taking the quaternion by value 436 (1429 bytes); rotation declared after the
// halves/dot/cosine 200; half-angle struct members sine-first 200; the dot clamped in place (no cosine local)
// 241 (1318 bytes); the axis crossed into its own vector then scaled into the quaternion with cross3f /
// cross3f_staged 241/240 (1317 bytes, frame still 0x1c). Retail never reads the quaternion's slot after loading
// it (0x1418d5-0x1418f8) yet gives the transform spills five slots of their own; ours overlay two on it.
// Round 294: Complete rotated-vector memcpy/memmove publication with staged-cross/independent-half
// combinations 334/311/334/311 and 348/383/348/383. Original helper/body restored; frame and
// quaternion/cross spill sharing remain.
// Round 267: result-component loop on staged-axis/real-scale construction,
// long/short/byte/word ordinals with paired halves 266/1023/1023/1023,
// independent halves 276/1023/1023/1023. Short ordinals retain an out-of-line
// transform at 596 bytes; original helper, halves and complete body restored.
// Frame 0x1c/0x24 and quaternion/cross spill overlap remain.
// DL228: plain forward/up/left field-table traversal, long/dword/short/word ordinals,
// ordinary and forced existing helper: 279/279/745/745/279/279/745/745.
// Original three in-place calls, helper and body restored; quaternion/spill homes remain.
// M292: Staged dot/doubled-scalar/quadratic coefficient subsets 1-7, then all with independent
// cross components:
// 200/200/200/249/249/249/249/261. Original executable body restored.
// P260: Independent half-angle lexical homes before/between/after rotation, four
// forms 267 each; with independent transform cross scalars 252 each. The latter
// reaches frame 0x24 but grows to 1336/1317; original helper and body restored.
// Round 523: Actual quaternion/matrix identity transfers native/memmove, memcpy/memmove,
// memmove/native, memmove/memcpy, memmove/memmove 285/285/319/319/306; both memmove with
// independent halves/cross components/both 316/321/317. Matrix transfer alone gives frame
// 0x20; original identities, paired halves, transform helper and full body restored.
// Round 312: Alias-conditioned native/CRT vector input capture in the existing rotation helper,
// original/staged cross and paired/independent halves: 347/347/340/340/340/340/320/320.
// Independent halves recover frame 0x24 and first +0x163 but grow code; original helper,
// paired halves and complete body restored. Quaternion/cross spill overlap remains.
// M3 lane 4 r155 (fast): retail never reuses the quaternion's 16-byte home (+0x24) for the transform spills (its five spill
// slots sit below it), ours packs two cross-term spills into the dead quaternion home. New: the zero-sine arms first (`if
// (sine == 0.f) { cosine test } else { construction }`) 299 (1342 bytes); the identity matrix copy at entry 315 (1325 bytes),
// right after the sine 315; the sine half computed before the cosine half 205; `cosine_half * sine_half * 2.f` 200 (same
// code); the existing quaternion_rotate_point with plain half-angle locals 1023 (calls out of line). Linkage diagnostics only:
// the transform helper `static` or a plain global 1028 (out of line, /Ob1 file), `__forceinline` 200. Re-measured: the
// existing quaternion_rotate_point for the transforms 258 (1303 bytes), plain half-angle locals 267 (frame 0x18).
// N329: Actual half-angle outputs own cosine/sine/both-sequential/both-early radicands: paired storage
// 253/253/249/289, independent halves 270/270/258/258. All regress; original root expressions, paired
// storage, helper and full body restored; frame and spill overlap remain.
// DH304: Cosine/zero/default-quaternion/common-scalar construction results, paired/independent halves, 277/283/244/300/246/308/243/241.
// NOT kept: moved stores/observations or common one-use scalar without a match. Original construction restored; quaternion/spill homes remain.
// N333: Plain indexed quaternion dot reductions, long/dword/short/word ordinals
// 315/315/350/350; paired initial-vector dot reduction 276/276/402/402.
// Original helper, initial dot and full body restored; quaternion/cross spill overlap remains.
// NOT kept: indexed reductions regress and fail the exact-match gate.
// M336: Transform dot/quadratic/doubled-scalar accumulators, seven subsets then all with
// a positive-validation return: 251/249/253/196/219/210/243/243. NOT kept: four-point
// combined gain below margin; original helper/body restored, frame 0x1c versus 0x24 remains.
// N337: Complete running helper-dot accumulation with caller-dot/staged-cross/
// independent-half subsets 0..7: 251/210/297/309/283/276/256/256. Original helper,
// paired halves and complete body restored; frame/quaternion/cross homes remain.
// Round 85 (m3 lane 12, fast): the initial dot as the existing dot3f(a, b)/dot3f(b, a) 205/205 (1326 B); the transform
// helper's dot as dot3f(q, v) * 2/dot3f(v, q) * 2 246/246 (1308 B); the clamp as one nested ternary 297 (1317 B, first
// +0x0); the halves through sqrtf 249 (1309 B). Original body and helper restored.
// N343: Complete cross first-product/subtraction lifetimes, vector/scalar storage crossed
// with shared dot and independent halves: 210/261/203/270/266/251/257/243. Original helper/body restored.
// NOT kept: no qualifying gain; native frame/quaternion/cross spill homes remain.
// M3 lane 7 r126 (fast): retail stores the doubled scalar to two homes (the matrix parameter's +0x38 and +0x10) as if
// each inlined transform kept its own copy. Named forward/left/up row pointers for the transforms, the 0x143120 test
// and the repair 322 (1294 bytes); `rotation.w = 0.f` before the 0x11d000 call 213 (1324); plain cross3f in the
// repair path 238 (1322); `cross` declared ahead of the helper's scalars 200 (same code); the helper's scalars `const`
// 200 (same code). Body restored.
// N346: Immutable transform dot/doubled-scalar/quadratic coefficient masks 1..7, then complete
// immutable coefficients/cross: 200/200/200/200/200/200/200/200. Original helper and all three users restored;
// NOT kept: unmatched lifetime/combination forms. Native frame/quaternion/cross spill homes remain.
// N361: Half-angle root-input double addition/factor/both/post-addition promotion, paired
// outputs 211/202/211/202, independent real outputs 278/269/278/269. Original root inputs,
// paired storage and body restored. NOT kept: unmatched precision-only forms; native spill homes remain.
// N363: Existing shared scaled-add owns final cross contribution, crossed with shared
// initial scale/projection/staged cross: 352/352/351/325/307/307/309/311. Original private helper and body restored;
// NOT kept: unmatched staged stores. Native frame/quaternion/cross spill homes remain.
// N365: Existing complete vector setter output with staged-cross/shared-dot/independent-half
// masks 0..7, 333/346/340/351/344/360/333/368. Paired output restores frame 0x24
// but grows code. Original helper/body restored; NOT kept: unmatched output ownership.
// N368: Caller initial-dot/sine/axis-scale adjacent producers, seven family subsets
// 263/253/253/221/221/249/214; all with entry declarations 214. Original producers restored.
// NOT kept: unmatched storage/producer forms regress; native half/quaternion/transform homes remain.
// N374: Paired root division, doubled helper-scalar addition and doubled
// half-angle-product addition, masks 1..7 and all with independent halves:
// 200/200/200/252/252/252/252/254. Original roots, factors, halves, helper and body
// restored; NOT kept: no native frame/spill recovery or byte match.
// N376: Actual quaternion vector-prefix construction/all transform reads/staged cross,
// seven subsets then shared prefix dot, 200/200/252/200/252/252/252/254. NOT kept: no exact frame/spill
// recovery; original union, helper, input safety and complete body restored.
// m3 lane 1 r71 (one-link harness, control 197 for the real 200; masked = aligned count with registers and esp offsets masked, control 187): retail's three transforms
// all compute the crosses j, k, i and compute s above the identity copy. Plain half-angle locals 266/1297 (masked 132, retail's two half homes; w and the sine swap xmm4/xmm5);
// with scalar crosses s, w2, dot 248/1315 (84, frame 0x20); dot, s, w2 251/1315 (87); s, dot, w2 in any cross order 248/1305 (151); dot term order, cross order on that base
// no change (canonicalised). The matched 0x11cb00 helper's exact operand and row forms on plain halves 198/1324 (99), on the struct 258/1303 (155); s computed before the identity copy as a
// helper parameter, plain halves 308/1262 (170) and 302/1270 (163), on the struct 309/1262 (139) and 305/1270 (107). Three annealed searches over statement order, operand
// forms and row orders, about 700 forms, none under 198 or below masked 84. None plain-confirmed, none kept. The two-member half struct stays until a form beats it.
// N399: Destructive scalar/cross-scale/cross-result ownership, families 1..7 then
// independent cross scalars: fast 256/306/253/313/333/333/312/306. Original ordered output trees and all three
// in-place users preserved. NOT kept: unmatched storage/result forms; original helper/body restored.
// Round 90 (m3 lane 16, fast): the doubled sine product written 2.f first 200 (same code); the clamp testing the
// cosine copy 200 (same code); the construction scale computed before the axis cross 252 (1317 bytes); the cosine
// arms swapped (`cosine >= 0.f` takes the identity) 210; rotation.w stored between the cross and the scale 200.
// @retail 0x1417b0
void matrix4x3_rotation_between_vectors(
	transform4x3f *matrix,
	vector3f const *arg_5f338b,
	vector3f const *arg_bc44c6)
{
	quaternionf rotation;
	struct { real cosine_half, sine_half; } state;
	real dot = arg_5f338b->i * arg_bc44c6->i + arg_5f338b->j * arg_bc44c6->j + arg_5f338b->k * arg_bc44c6->k;
	real cosine = dot;

	if (dot < -1.f)
	{
		cosine = -1.f;
	}
	else if (dot > 1.f)
	{
		cosine = 1.f;
	}

	state.cosine_half = (real)sqrt((cosine + 1.f) * 0.5f);
	state.sine_half = (real)sqrt((1.f - cosine) * 0.5f);
	real sine = state.sine_half * state.cosine_half * 2.f;

	if (sine != 0.f)
	{
		cross3f_staged(arg_5f338b, arg_bc44c6, (vector3f *)&rotation);
		real scale = state.sine_half / sine;
		vector3f_scale((vector3f *)&rotation, scale, (vector3f *)&rotation);
		rotation.w = state.cosine_half;
	}
	else if (cosine < 0.f)
	{
		function_11d000(arg_5f338b, (vector3f *)&rotation);
		rotation.w = 0.f;
	}
	else
	{
		rotation = *g_4687cc;
	}

	*matrix = *g_4687d0;
	quaternion_transform_vector(&rotation, &matrix->forward, &matrix->forward);
	quaternion_transform_vector(&rotation, &matrix->up, &matrix->up);
	quaternion_transform_vector(&rotation, &matrix->left, &matrix->left);
	if (!function_143120(&matrix->forward, &matrix->left, &matrix->up))
	{
		function_30bf0(&matrix->up);
		cross3f_staged(&matrix->up, &matrix->forward, &matrix->left);
		function_30bf0(&matrix->left);
		cross3f_staged(&matrix->left, &matrix->up, &matrix->forward);
	}
}

// @retail 0x141ce0
void function_141ce0(
	real a,
	real b,
	real c,
	transform4x3f *out)
{
	point2f angle_c;
	real cos_b, sin_b, cos_a, sin_a, sin_b_sin_c, sin_b_cos_c;
	angle_c.x = (real)cos(c);
	angle_c.y = (real)sin(c);
	cos_b = (real)cos(b);
	sin_b = (real)sin(b);
	cos_a = (real)cos(a);
	sin_a = (real)sin(a);
	sin_b_cos_c = sin_b * angle_c.x;
	sin_b_sin_c = sin_b * angle_c.y;
	out->scale = 1.f;
	out->forward.j = sin_a * angle_c.x - sin_b_sin_c * cos_a;
	out->forward.i = cos_a * cos_b;
	out->forward.k = sin_b_cos_c * cos_a + sin_a * angle_c.y;
	out->left.i = 0.f - sin_a * cos_b;
	out->left.j = sin_b_sin_c * sin_a + cos_a * angle_c.x;
	out->left.k = cos_a * angle_c.y - sin_b_cos_c * sin_a;
	out->up.i = 0.f - sin_b;
	out->up.j = 0.f - cos_b * angle_c.y;
	out->up.k = cos_b * angle_c.x;
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
}

// @retail 0x141e10
matrix3x3 *function_141e10(
	matrix3x3 *out,
	quaternionf const *q)
{
	real norm = q->i * q->i + q->j * q->j + q->k * q->k + q->w * q->w;
	real s = norm > 0.0001f ? 2.f / norm : 0.f;
	real xs = q->i * s;
	real ys = q->j * s;
	real zs = q->k * s;
	real wx = q->w * xs;
	real wy = q->w * ys;
	real wz = q->w * zs;
	real xx = q->i * xs;
	real xy = q->i * ys;
	real xz = q->i * zs;
	real yy = q->j * ys;
	real yz = q->j * zs;
	real zz = q->k * zs;
	out->forward.i = 1.f - (yy + zz);
	out->left.i = xy - wz;
	out->up.i = xz + wy;
	out->forward.j = xy + wz;
	out->left.j = 1.f - (xx + zz);
	out->up.j = yz - wx;
	out->forward.k = xz - wy;
	out->left.k = yz + wx;
	out->up.k = 1.f - (xx + yy);
	return out;
}

// @retail 0x141f60
quaternionf *function_141f60(
	matrix3x3 const *matrix,
	quaternionf *out)
{
	real const *m = (real const *)matrix;
	real trace = matrix->left.j + matrix->forward.i + matrix->up.k;
	if (trace > 0.f)
	{
		real root = (real)(sqrt(trace + 1.f) * 0.5f);
		real inverse = 0.25f / root;
		out->w = root;
		out->i = (m[5] - m[7]) * inverse;
		out->j = (m[6] - m[2]) * inverse;
		out->k = (m[1] - m[3]) * inverse;
	}
	else
	{
		long i = 0, j, k;
		real root, inverse;
		if (m[4] > m[0])
			i = 1;
		if (m[8] > m[i * 4])
			i = 2;
		j = (i + 1) % 3;
		k = (i + 2) % 3;
		root = (real)(sqrt(m[i * 4] - m[j * 4] - m[k * 4] + 1.f) * 0.5f);
		inverse = 0.25f / root;
		out->n[i] = root;
		out->w = (m[j * 3 + k] - m[k * 3 + j]) * inverse;
		out->n[j] = (m[i * 3 + j] + m[j * 3 + i]) * inverse;
		out->n[k] = (m[i * 3 + k] + m[k * 3 + i]) * inverse;
	}
	if (out->w < 0.f)
	{
		out->w = 0.f - out->w;
		out->i = 0.f - out->i;
		out->j = 0.f - out->j;
		out->k = 0.f - out->k;
	}
	return out;
}

__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);

// @retail 0x1420f0
void function_1420f0(
	transform4x3f *out,
	point3f const *position,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
	out->position = *position;
}

// @retail 0x1421b0
void function_1421b0(
	transform4x3f *out,
	point3f const *position,
	quaternionf const *rotation)
{
	function_141e10(&out->rotation, rotation);
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
	out->scale = 1.f;
	out->position = *position;
}

real g_45dbdc = 0.0001f;
static const real g_45dbc0 = 1.f;
__declspec(align(16)) static const unsigned long g_453750[4] = {0x80000000, 0, 0, 0x80000000};

// @retail 0x1421f0
void __stdcall function_1421f0(
	transform4x3f *out,
	rigid_transform_scaled const *orientation)
{
	static real epsilon = g_45dbdc;
	__asm
	{
		mov eax, orientation
		mov ecx, out
		movlps xmm0, qword ptr [eax]
		movhps xmm0, qword ptr [eax + 8]
		movaps xmm7, xmm0
		mulps xmm0, xmm7
		movhlps xmm1, xmm0
		addps xmm0, xmm1
		movaps xmm1, xmm0
		shufps xmm1, xmm1, 0x55
		addss xmm0, xmm1
		comiss xmm0, epsilon
		ja big
		movss xmm0, g_45dbc0
		movlps qword ptr [ecx + 4], xmm0
		movhps qword ptr [ecx + 0xc], xmm0
		movlps qword ptr [ecx + 0x14], xmm0
		movhps qword ptr [ecx + 0x1c], xmm0
		movss dword ptr [ecx + 0x24], xmm0
		jmp done
	big:
		rcpss xmm1, xmm0
		mulss xmm0, xmm1
		mulss xmm0, xmm1
		addss xmm1, xmm1
		subss xmm1, xmm0
		addss xmm1, xmm1
		shufps xmm1, xmm1, 0
		mulps xmm7, xmm1
		movss xmm4, dword ptr [eax]
		shufps xmm4, xmm4, 0
		mulps xmm4, xmm7
		movss xmm5, dword ptr [eax + 0xc]
		shufps xmm5, xmm5, 0
		mulps xmm5, xmm7
		movss xmm6, dword ptr [eax + 4]
		shufps xmm6, xmm6, 0
		mulps xmm6, xmm7
		movhlps xmm7, xmm7
		mulss xmm7, dword ptr [eax + 8]
		movaps xmm0, xmm4
		shufps xmm0, xmm0, 0x99
		movaps xmm1, xmm5
		shufps xmm1, xmm1, 0x66
		xorps xmm1, xmmword ptr g_453750
		addps xmm0, xmm1
		movhps qword ptr [ecx + 8], xmm0
		movss dword ptr [ecx + 0x10], xmm0
		movlps qword ptr [ecx + 0x18], xmm0
		movhlps xmm2, xmm6
		shufps xmm6, xmm6, 0x55
		movss xmm0, g_45dbc0
		subss xmm0, xmm7
		movss xmm1, xmm0
		subss xmm0, xmm6
		movss dword ptr [ecx + 4], xmm0
		subss xmm1, xmm4
		movss dword ptr [ecx + 0x14], xmm1
		movss xmm3, g_45dbc0
		subss xmm3, xmm4
		subss xmm3, xmm6
		movss dword ptr [ecx + 0x24], xmm3
		movss xmm7, xmm2
		subss xmm7, xmm5
		movss dword ptr [ecx + 0x20], xmm7
		addss xmm2, xmm5
		movss dword ptr [ecx + 0x18], xmm2
	done:
		movss xmm0, dword ptr [eax + 0x1c]
		movss dword ptr [ecx], xmm0
		movlps xmm0, qword ptr [eax + 0x10]
		movlps qword ptr [ecx + 0x28], xmm0
		movss xmm0, dword ptr [eax + 0x18]
		movss dword ptr [ecx + 0x30], xmm0
	}
}

// @retail 0x142360
void orientation_from_matrix4x3(
	transform4x3f const *matrix,
	rigid_transform_scaled *out)
{
	function_141f60(&matrix->rotation, &out->rotation);
	out->position = matrix->position;
	out->scale = matrix->scale;
}

void function_142390(
	plane3f const *plane,
	transform4x3f *out);

static __forceinline void matrix4x3_basis_store(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	cross3f_staged(up, forward, &out->left);
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
	out->position = *position;
}

static __forceinline void function_x7a3668(plane3f const *plane, transform4x3f *out)
{
	vector3f w;
	point3f position;
	function_11d000(&plane->n, &w);
	normalize3d(&w);
	position.x = plane->d * plane->n.i;
	position.y = plane->n.j * plane->d;
	position.z = plane->n.k * plane->d;
	matrix4x3_basis_store(out, &position, &w, &plane->n);
}

// score 51 (was 51): frame 0x1c and sum grouping agree; the extra early first-component
// load changes normalization-join registers and subsequent cross-product allocation.
// The first difference is an extra early load of the first component (retail
// loads only the second before the magnitude test). Second component scaled first
// 52; first scaled as component * inverse 51; third stored first 53; first and
// third read into locals 51; third scaled as k * inverse 51 (fast).
// Round 406: direct nonnegative magnitude guard, length return and shared setter
// 76/76/76/76; original magnitude expression at the caller boundary with the same
// return/setter choices 89/89/89/89. All restored.
// Round 22 (one-file harness, base 52 for the build's 51): the scale operand order of
// each component, all 8 orders, makes no difference (with the third read through out-> 89
// for every order and for sums i-j-k and j-i-k, 105 for j + (i + k)); component locals in
// every subset and declaration order 52 when k is declared before j, 57 otherwise, 89 for
// i alone; magnitude returned (early or else-zero) 52; `>=` guard 80; i/j scaled with *=
// 52; the normalize written on w directly 91; cross3f_staged/cross3f (up, forward) into
// out->left 77/82, through a local 105/105 (371 bytes).
// Round 27 (fast): the file's own unused normalize3d 88; the shared normalize3d.h normalize (dot3f and
// vector3f_scale) in place of it and the local normalize 68 at 353 bytes: the first component is then
// scaled as retail (a copy of the inverse times the component in memory, only the second loaded before
// the test), but the join keeps the first in xmm2 and moves the third to xmm4 (retail xmm4/xmm0) and the
// cross products run in the other order; that with the position products before the normalize 83; the
// local normalize without the third-component local 88; with dot3f for the magnitude 88.
// Round 18 (lane 14): normalize helper plain `inline` 51 (same code); basis store plain `inline` 51;
// magnitude through the existing length_sq3f 98 (332 bytes); normalize applied to the returned pointer
// of 0x11d000 94 (354 bytes); position declared before the vector 51. Compile-only screens (which
// reproduce the early first-component load): every subset of component locals x operand orders x
// scale-statement orders (384 forms) at best the second-component local; the shared normalize3d.h
// scale and the cross3f/cross3f_staged forms on it leave the position temporaries in xmm0 (retail
// xmm2), the third component in xmm4 (retail xmm0) and the first in xmm2 (retail xmm4).
// @retail 0x142390
void function_142390(
	plane3f const *plane,
	transform4x3f *out)
{
	function_x7a3668(plane, out);
}

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_11d790(quaternionf const *q, real *angle, vector3f *axis);

// @retail 0x1424f0
vector3f *matrix4x3_rotation_between(
	transform4x3f const *a,
	transform4x3f const *b,
	vector3f *out)
{
	transform4x3f inverse;
	transform4x3f relative;
	quaternionf rotation;
	real angle;

	function_141590(a, &inverse);
	function_142a60(b, &inverse, &relative);
	function_141f60(&relative.rotation, &rotation);
	function_11d790(&rotation, &angle, out);
	out->i *= angle;
	out->j *= angle;
	out->k *= angle;
	return out;
}

// @retail 0x142570
point3f *transform4x3f_apply_point(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	/* retail passes the matrix on the stack (ret 4) while 0x142640, with the
	   same body, takes it in ecx: the parameter's address is taken here, and
	   the optimizer removes the indirection only after LTCG has chosen the
	   convention. Its callers' conventions (0x2104b0 ...) follow from it. */
	transform4x3f const *const *matrix_reference = &matrix;
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if ((*matrix_reference)->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

// @retail 0x142640
vector3f *function_142640(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

// @retail 0x142700
point3f *function_142700(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	if (matrix->scale != 0.f)
	{
		real x = point->x - matrix->position.x;
		real y = point->y - matrix->position.y;
		real z = point->z - matrix->position.z;
		if (matrix->scale != 1.f)
		{
			real inverse = 1.f / matrix->scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		out->x = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
		out->y = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
		out->z = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
		out->z = 0.f;
	}
	return out;
}

// @retail 0x1427f0
vector3f *function_1427f0(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		real inverse = 1.f / matrix->scale;
		x = inverse * x;
		y = inverse * y;
		z = inverse * z;
	}
	out->i = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
	out->j = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
	out->k = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	return out;
}

inline bool function_a0190(vector3f const *vector);
__declspec(noinline) bool function_a0200(real a, real b);

static __forceinline real basis_dot_xzy(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	result += b->k * a->k;
	real side = a->j;
	side *= b->j;
	result += side;
	return result;
}

static __forceinline real basis_dot_xzy_vertical(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	real vertical = b->k;
	vertical *= a->k;
	result += vertical;
	real side = a->j;
	side *= b->j;
	result += side;
	return result;
}

static __forceinline real basis_dot_xyz(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	real side = a->j;
	side *= b->j;
	result += side;
	result += a->k * b->k;
	return result;
}

// @retail 0x143120
bool function_143120(
	vector3f const *forward,
	vector3f const *left,
	vector3f const *up)
{
	return function_a0190(forward) &&
		function_a0190(left) &&
		function_a0190(up) &&
		function_a0200(basis_dot_xzy(forward, left), 0.f) &&
		function_a0200(basis_dot_xyz(left, up), 0.f) &&
		function_a0200(basis_dot_xzy_vertical(forward, up), 0.f);
}

inline void function_143250(real *out, unsigned long n, real const *a, real const *b);

// @retail 0x143250
void function_143250(
	real *out,
	unsigned long n,
	real const *a,
	real const *b)
{
	unsigned long i, j;
	for (i = 0; i < n; i++)
	{
		out[i] = b[0] * a[i * n];
		for (j = 1; j < n; j++)
			out[i] = b[j] * a[i * n + j] + out[i];
	}
}

// score 1333 (was 1333): frame 0x2fc versus retail 0x300 and 10752 of 10773 bytes; `ret 4` with rotation in EAX as retail. 80 of the 83
// matrix entries have retail's value tree; band three's entries 8, 25 and 28 and the stack homes (140 of 216 stores) differ.
// g1A walk (symbolic): retail 0x143600-0x146012 executed as an expression DAG (SSE/x87, f32/f64 and (real) points, association and
// constants from the XBE kept) against this body: all 83 values handed to the three function_143250 calls (3x3, 5x5, 7x7) are identical,
// the order <= 2 / <= 3 returns, call arguments (result+1/+4/+9, n 3/5/7) and result[0] agree; no behaviour difference. Retail's op order
// equals this source's order (0 inversions of 891); ours is reordered (43k). Fast-check trials, no change at 1766: term_64 from term_60; term_41 operands swapped.
// Convention (g2A, 11-05): retail takes rotation in EAX, coefficients in ECX and result in ESI with only `order` on the stack
// (ret 4; callers' setup `push 3; lea ecx; lea eax; lea esi`, 0x23a39); this body takes rotation from the stack (ret 8).
// Not kept: writing the 388 earliest single-use terms into their next statement gives retail's convention with the body
// otherwise identical (0x22850 237 -> 235, 0x23690 481 -> 467, both setups then equal retail's) but 1775 here; 387 terms or
// the last 388 keep ret 8, as does any other 388th term but #420, so it is a count of locals outranking the parameter, not a
// source shape. All 1002 single-use terms inlined: ret 4 at 1789, frame 0x330; the 370 pure copies only: ret 8, 1927.
// Parameters as (result, order, rotation, coefficients) with every call updated: no change on that form, 236/464 on this one
// (rotation still on the stack); retail's ECX/EAX/ESI setup fits both orders, so no evidence for it. The x87 multiply at
// retail 0x1438ba is register allocation (rotation[7] lives in xmm6 in retail, a memory operand here), not the double's spelling.
// Earlier: frame 0x314 versus retail 0x300; rotation admission and basis/matrix homes differ.
// Earlier: full frame 0x300 agrees; rotation input and basis/matrix homes remain.
// Frame pointer 3182; separate adjacent basis pairs [0,1] 2278, [2,3] 2320,
// [4,5] 2286, [6,7] 2329.
// Round 310: second-band product trees 2265; second-band full trees 2265; third-band
// products 2272; third-band full trees 2273; both full trees 2273 (plain). Input and matrix homes remain.
// Frame 0x300 agrees; rotation input and basis/matrix spill homes differ.
// Independent snapshots [1,5,6] 2302; [0,2,7] 2309; [3,4] 2286; [0,4,5] 2286; [3,5,7] 2276 (plain).
// frame 0x300 agrees; rotation admission and matrix homes differ.
// Independent basis pairs [0,5] 2275; [1,6] 2320; [2,4] 2325; [3,6] 2317; subset [0,2,4,6] 2367
// (plain).
// Frame 0x300 agrees; rotation still arrives on the stack and
// basis/matrix homes differ. independent basis 0 6 2311; independent basis 1 7 2293;
// independent basis 2 5 2320; independent basis 4 7 2298; independent basis 0 3 5 7 2299.
// Round 349: Independent basis pairs [1,2] 2323; [2,6] 2355; [0,7] 2287; [3,7] 2290; [5,6] 2317 (fast; control 2266).
// Round 357: Separate first/second/third band tables 2268/2268/2266; independent
// basis pairs [0,2] 2314, [1,4] 2288 (fast; control 2266). Opening body restored.
// Band-exclusive scalar workspaces scoped across each actual 3/5/7 matrix producer,
// all seven subsets and earlier shared declarations: 2266 each (fast control 2266; full 2265).
// Round 224: complete input matrix copy with retained workspace, library/value/aggregate
// 2275/2275/2289; direct matrix library/const/aggregate 2371/2367/2364; direct input table
// library/const 2371/2366 (fast control 2266). Original workspace and expressions restored.
// All four initial negatives at stores/early 2266/2266; shared matrix receiver
// at entry/publication 2163/2163; receiver with inlined negatives 2169/2169;
// receiver with early negatives 2149/2149 (fast control 2266). Last form 2148
// plain, 10390/10773 bytes; receiver used throughout the genuine shared matrix.
// Round 385: Current shared-matrix receiver graph: independent native rows, all seven subsets
// 2240/2237/2348/2191/2294/2286/2397; all with component-order names 2397 (fast control 2149; full
// 2148). Original workspace and complete body restored; input and basis/matrix homes remain.
// Round 259: complete matrix publication after its scalar producer, band masks
// 2/4/6/1/3/5/7 in row order and all in original order: fast
// 2067/2156/2236/2145/2051/2156/2263/2263 (control 2149). First and second
// matrices in row order kept: 2050 plain, 10488/10773 bytes; every scalar
// computation, input, precision boundary, order guard and helper tuple unchanged.
// Round 263: complete column publication of band masks 1/2/3/4/5/6/7 and all reversed columns
// 2051/2040/2040/2199/2199/2210/2210/2291 (fast control 2051). Five-by-five columns kept: 2039 plain,
// 10500/10773 bytes; every scalar producer and helper tuple unchanged.
// Round 268: native two-dimensional band rows, masks 1/2/3/4/5/6/7,
// 2055/2088/2107/2123/2146/2179/2279; all arrays at entry 2279
// (fast control 2040, full 2039). Original shared matrix and publication orders restored;
// full frame 0x300, rotation admission and basis/matrix homes remain.
// Round 271: exact first/second/both native retail matrix store orders after complete
// producers, 2040/2051/2051; separate shared 49-element array alone/first/second/both
// 2279/2279/2294/2294; complete graph with basis identity 2294 (fast control 2040).
// Original shared workspace, publications and complete polynomial body restored.
// Round 187: On retained publications, helper inline/noinline/both 2088/2039/2088; with independent basis 2347/2298/2347; first-matrix negative lifetimes with inline 2092, with both plus independent basis 2347. All restored; rotation admission and matrix homes remain.
// Third-band row publications at the complete scalar producer, rows 0/1/2/3/4/5/6,
// 2076/1997/1972/2026/1808/2105/2040; all rows in producer order 2272
// (fast control 2040). Rows [2,4] 1879; rows [1,2,3,4] 1921. Single row 4
// kept 1808 plain at 10731/10773 bytes; all scalar statements, precision boundaries,
// matrix element values and application calls preserved. Rotation admission and basis/matrix homes remain.
// Round 320: third-band row publication combinations [0,4]/[1,4]/[3,4]/[4,5]/[4,6]/
// [0,1,4]/[3,4,5,6]/[0,1,3,4,5,6] 1843/1806/1837/1836/1799/1781/1915/2075.
// Rows [0,1,4] kept: 1781 plain, 10789/10773 bytes, frame 0x314; all other measurements unchanged.
// Only fourteen additional matrix stores move; every scalar, precision boundary, value and application tuple remains.
// Round 447: Third-band retained late rows [0,1,4] by columns in six row orders, then descending columns across rows and within each row
// 1791/1795/1795/1790/1795/1791/1812/1803. Rotation admission and basis/matrix spill homes remain.
// Original executable source and all trial helper/interface changes restored.
// Lane 10 round 34: retail passes rotation in EAX, coefficients in ECX, result in ESI and the order on the stack
// (ret 4); ours stacks rotation and the order (ret 8). --fast pins conventions from the last full build, so only
// plain checks show them. Plain: the parameters in retail's call-setup order (result, order, rotation,
// coefficients; definition, declaration and four calls) 1781, rotation still stacked; 0x22850's coefficient loop
// written as three per-channel calls as retail has them (and as its second half does) 1781 here, 0x22850 237->254;
// the eight leading terms and the matrix as plain locals 2116 (frame 0x300 as retail, rotation still stacked).
// Fast: rotation-first orders (rotation, order, result, coefficients) and (rotation, result, order, coefficients)
// 1781 each (convention pinned); round 19's body 2266 (rotation stacked there too).
// Round 215: producer-ready retained column 0 1768; producer-ready retained column 1 1782;
// producer-ready retained column 2 1784; producer-ready retained column 3 1913; producer-ready
// retained column 4 1772; producer-ready retained column 5 1766; producer-ready retained column 6
// 1782; all producer-ready retained columns 1833; producer-ready retained columns 0 and 5 (one-
// point increment rejected) 1765. Keep t06 producer-ready column 5, plain-confirmed 1781 to 1766
// at exact 10773-byte retail length; all 11489 baseline matches preserved. Combined columns 0/5 at
// 1765 add only one point and are rejected.
// Round 255: producer-ready retained columns 1/5, 2/5, 3/5, 4/5, 5/6, 1/4/5, 2/4/5,
// 4/5/6 score 1772/1768/1768/1772/1771/1802/1801/1783. Original publication graph restored;
// frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// Round 263: Immutable first-assignment intermediates in all seven band subsets tie 1766 at 10773 bytes;
// complete immutable graph without the mixed workspace 2229 at 10304 bytes. Original scalar declarations
// and inherited workspace restored; frame, input admission and basis/matrix homes remain.
// Round 145: the stacked rotation comes from the number of locals: LTCG passes it in EAX (retail's
// `ret 4`, order alone on the stack) only while the function declares fewer than about 950 of them
// (a one-file LTCG probe flips between 950 and 980 unused declarations on a cut-down body; block or
// struct scope still counts). Plain checks: every term copy propagated (971 terms, rotation still stacked)
// 1927 (10933 bytes); copies propagated and every single-use term written into its consumer (299 terms,
// retail's convention) 1886 (10894); the first 400 single-use terms written in, copies kept (941 terms,
// retail's convention) 1778 (10758); all single-use terms written in, copies kept (356) 1813 (10723); the
// 299-term form with the workspace as plain locals 2222 (10310, frame 0x304). Callers 0x22850 237->235 and
// 0x23690 508->512 with retail's convention.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 46 (lane 14, plain): only terms whose single use is the next statement written into it: the first 420
// (921 declarations) 1775 at 10763 bytes with retail's convention (rotation in EAX, ret 4; 0x22850 235, 0x23690
// 512); all 678 such terms the same 1775 code (only the declaration count changes); on those, the eight leading
// terms and the matrix as plain locals 2236 (frame 0x300 as retail, 10297 bytes), with the matrix pointer kept
// over a plain array 2235. Retail loads the eight leading terms in declaration order, then rotation[7].
// Round 439: single-consumer same-type sum/product/copy elision, all seven subsets,
// 1881/1796/1903/1766/1867/1796/1905; complete graph with matrix receiver renamed 1905.
// Product elision leaves 943 scalars and recovers rotation in EAX, but frame 0x314 remains;
// all nine initial loads and explicit precision conversions were retained. Entire body restored.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 278: Native last-use sum/product/mixed accumulators, left/right priority and band subsets:
// 1835/1776/1805/1774/1776/1781/1774/1810. Product form confirms 1776 plain, EAX rotation and ret 4 but
// frame 0x314. On that product graph, native row subsets and late matrix declaration:
// 1807/1855/1989/1992/2093/2090/2238/2238. Original scalar graph, workspace, publications and body
// restored; frame 0x314/0x300 and basis/matrix homes remain.
// Round 289: native final matrix-entry chains in bands 5/7/both, left-priority 1774/1791/1786;
// right-priority 1766/1804/1804; complete chains plus same-type local alias cleanup 1817/1983.
// The last two recover EAX/ret 4 but frames 0x324/0x314 and caller 508 -> 512 regress; all restored.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 246: Independent complete input columns on the retained publication graph, masks
// 1/2/3/4/5/6/7, 1901/1766/1901/1914/2230/1774/2113; all with a native-table receiver
// 2228. Full extraction recovers frame 0x300 but regresses; original snapshots,
// workspace and polynomial graph restored. Rotation admission and spill homes remain.
// Round 500: literal-only scalar cleanup with complete basis/single-consumer/reused
// same-type copy families, masks 0..7, 1767/1948/1766/1936/1927/2000/1917/1986.
// Original scalar and publication graph restored; frame, rotation admission and basis homes remain.
// The full identity/literal graph has 917 numeric declarations, EAX rotation and frame 0x304
// in the reduced image, but regresses to 1986 at 10988 bytes.
// Round 222: numeric scalar identities, mutable first-assignment declarations,
// first-producer/final-consumer declaration order, double-first/real-first declarations,
// numeric first-assignment and reverse numeric declarations: 1766 each. Every expression,
// precision boundary and publication retained; complete original scalar graph/body restored.
// Round 297: same-real one-use negative/literal-scaling/unequal-factor product families,
// masks 1..7, 1767/1767/1767/1766/1766/1771/1771; complete graph with independent
// original basis/matrix storage 2237. Squares and reused values retained; all executable
// source restored. Frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// Round 72 (lane 16, fast): single-use terms written into the next statement, first 420 and all 648 eligible 1775/1775
// (10763 bytes, rotation EAX, ret 4, frame 0x30c); on the 648 form the leading terms as plain locals, with a matrix
// pointer or a plain array 2238/2238 (10297 bytes, frame 0x2fc); plain leading locals keeping every term 2114 (10412).
// Restored.
// Round 519: Shared identical third-band real sum with complete basis/single-consumer/
// reused copy-family masks 0..7: 1793/1928/1793/1916/1898/1975/1898/1964.
// Native typed replay preserves all 83 matrix values and precision boundaries;
// Full cleanup has frame 0x30c and EAX/ret 4 in preview but regresses and raises a caller score.
// Original scalar, publication and input graphs restored; frame and homes remain.
// P264: single-consumer real arithmetic owned by second-band product/sum/all,
// third-band product/sum/all, crossed product/sum and sum/product: 1766/1771/1816/1790/1869/1854/1869/1784.
// Original scalar graph, workspace, publication phases and complete body restored.
// N325: Native same-type scalar workspace reuse across complete band boundaries,
// six distinct boundary graphs, double-only reuse and first-producer declarations:
// 1767/1767/1788/1766/1788/1780/1767/1788. All 83 typed matrix outputs audited unchanged; original scalar
// lifetimes and complete body restored. Frame, rotation admission and basis/matrix homes remain.
// Round 482: Complete seven-row third-band producer-ready columns 0/1/2/3/4/5/6/all
// give 1952/1805/1778/1767/1779/1786/1859/2210. Exact scalar sequence and all 49
// matrix values retained in every trial; original publication graph and body restored.
// Frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// N333: Single-consumer double roots in bands two/three and double product ownership,
// families 1..7: 1767/1767/1767/1767/1767/1767/1766; complete immutable reused root 1766.
// All 84 typed outputs/call phases agree; original graph restored; frame/admission/homes remain.
// NOT kept: precision-sensitive compaction remained unmatched and tied or regressed.
// N335: Disjoint second-band real/third-band real/both-band double temporary lifetimes, subsets
// 1/2/4/3/5/6/7: 2186/2262/1767/2722/2186/2262/2722; complete last-available reuse 3063. Third-
// band sharing recovers ret 4 but frame 0x324 and length grow; double-only frame 0x30c regresses
// by one. NOT kept: unmatched shared-storage forms; all 84 typed outputs agree, original body
// restored.
// N337: Last-use same-type second/third-band subtraction and zero-minus accumulators,
// family subsets 1..7: 1766/1788/1788/1766/1766/1788/1788; all with right-first ownership 1774.
// All 84 typed outputs agree. NOT kept: unmatched storage reuse ties or regresses and changes
// length away from retail; original scalar graph and complete body restored.
// Round 47 (lane 15, plain): nested single-exit order tests 1766; matrix read directly on the workspace, no pointer, 2027 (10501 bytes,
// frame 0x304); term_10 dropped 1766; term_9 in the workspace 1777 (10791, frame 0x318); first two workspace terms swapped 1766.
// N339: Explicit real narrowing owned by consumers, second-band product/third-band
// roots/third-band product family masks 1..7, then all with immutable reused double root:
// 1767/1781/1780/1771/1771/1775/1774/1774. All 84 typed outputs agree.
// NOT kept: unmatched repeated-value/precision-storage forms regress and move length;
// original polynomial, conversion producers and complete body restored.
// Round 85 (m3 lane 12, plain): retail passes rotation in EAX, coefficients in ECX and result in ESI with order on the
// stack (ret 4); ours pushes rotation (ret 8) although the /Gd first pass loads it once into EAX at entry. Parameters as
// (result, rotation, coefficients, order), retail's call-setup order, 1766 (same code); rotation reads in index order
// 1784 (10754 B); `result[0] = coefficients[0];` first 1766; term_9 read before the matrix pointer 1766; the workspace
// struct as plain locals and a 49-real array 2228 (10304 B, frame 0x300). Body and parameter order restored.
// Round 47 (lane 15, plain): nested single-exit order tests 1766; matrix read directly on the workspace, no pointer, 2027 (10501 bytes,
// frame 0x304); term_10 dropped 1766; term_9 in the workspace 1777 (10791, frame 0x318); first two workspace terms swapped 1766.
// N343: Real storage for both second-band roots, each third-band root, all third-band roots
// and all roots: 1762/1766/1766/1766/1769/1766/1769/1769. Original double roots and full body restored.
// NOT kept: unmatched precision/storage changes; native frame/input/basis homes remain.
// M3 lane 7 r126 (fast): the /Gd first pass loads each parameter once (rotation EAX at entry, result ESI, coefficients
// EDI, order EBX after the first call), so that rule predicts retail's EAX rotation. Diagnostics: a body cut after band
// one still pushes rotation, under five parameter orders and with a void return; the cut body without the 0x143250
// call (out EAX, n EBX) takes rotation in ECX, so that call keeps rotation on the stack in our build. Trials: all nine
// doubles as real 1813 (10825 bytes, frame 0x308, ret 8); the terms as plain locals and a `real matrix[49]` 2114
// (10412, frame 0x2fc, ret 8); `result[0] = coefficients[0]` just before band one's call 1766 (same code); band one in
// retail's store order (each matrix entry after its term) 1794 (10740); the matrix pointer declared first 1766 (same
// code). Retail keeps the nine rotation terms as scattered separate locals and the matrix at the frame top. Restored.
// N346: Isolated matrix/workspace/rotation/result/coefficients/term_9/term_8/term_11 names:
// 1766/1766/1766/1766/1766/1766/1766/1766. Original identifiers and complete scalar graph restored;
// NOT kept: unmatched lexical allocation. Frame, rotation admission and basis/matrix homes remain.
// N361: Actual band-table aggregate initialization, band masks 1..7 and all immutable,
// 1976/1930/1880/2123/2126/2166/2187/2187. Original assignment-based tables and full scalar
// graph restored. NOT kept: unmatched table lifetime/publication forms; frame, admission and homes remain.
// N363: Complete arithmetic-role scalar identities, product/sum-difference/copy-literal
// masks 1/2/4/3/5/6/7 then all scalar roles: 1766 each. Original names and full body restored;
// NOT kept: unmatched home allocation. Native frame, rotation admission and basis homes remain.
// N365: Complete actual-result copy/compound multiply/add/subtract producer families,
// seven subsets and all with producer declarations, 1766 each. Original scalar graph
// restored. NOT kept: unmatched producer ownership; native frame/admission/homes remain.
// N368: Final-consumer lexical homes for real/double/both families, forward/reverse,
// then both within producer bands, forward/reverse: 1766 each. Original names restored.
// NOT kept: unmatched home allocation; native frame, rotation admission and basis/matrix homes remain.
// N371: Complete scalar lexical homes by fanout/depth/live-span, forward/reverse,
// then band-fanout-depth/band-depth-fanout: 1766/1766/1766/1766/1766/1766/1766/1766. Original names restored.
// NOT kept: unmatched spill-priority forms; native frame, input admission and basis/matrix homes remain.
// N374: Actual band-local matrix receiver lifetimes from first store through
// application, band masks 1..7 and all immutable: 1766/1766/1766/1766/1766/1766/1766/1766.
// Original shared receiver and full scalar/publication graph restored;
// NOT kept: no native frame/admission recovery or byte match.
// N376: Same-type copy contraction by actual input/product/root-or-literal/sum
// producers within bands, eight forms 1947/1766/1766/1771/1761/1766/1925/1948. All typed outputs/call
// phases agree. NOT kept: no exact native storage recovery; full scalar graph restored.
// N399: Native-located disjoint scalar reuse, second-band spills/third-band spills/SIMD
// families 1..7 then all at first declarations: fast 1766/2001/2001/2612/2528/2341/2278/2278. All 84 ordered typed outputs
// and application tuples agree. SIMD forms recover EAX/ret 4 and improve both callers, but grow
// the frame/extent (all-SIMD 0x334, 11540 bytes). NOT kept: unmatched physical-storage forms; original body restored.
// Round 90 (m3 lane 16): the workspace declared after the terms 1766 (fast, same code); every same-type plain copy
// propagated (404 copies, 937 declarations) 1986 plain (10988 bytes, rotation EAX and ret 4, frame 0x30c; 0x22850 235,
// 0x23690 467); the workspace matrix ahead of the rotation terms 1766 (fast); that with the 648 single-use terms in
// their next statement 1775 plain (10763 bytes, ret 4, frame 0x314; 0x22850 235, 0x23690 467); only the 37 rotation
// snapshot copies propagated 1948 (fast, 10949 bytes: the band-start copies keep the snapshots out of the workspace).
// M3 lane 12 r108 (plain; callers 0x22850 223 and 0x23690 401 unchanged): band one's four negations and term_10 written into
// the matrix stores 1791 (10740 B); band two's 40 pure copies propagated 1947 (10952 B, likely N376's 1947 form); band two's
// four negations alone 1767; band one's term_10 copy alone 1766 (same code); band three's 363 copies 1802 (10813 B);
// band two's copies and negations (44) 1947; bands one and two together (49) 1906 (10916 B). All keep rotation on the stack (ret
// 8, frame 0x30c). Retail stores each product to two stack slots at once (the term and its copy, e.g. [esp+0x98] and [esp+0x30]),
// so its copies are real variables too, kept in memory in source order. Restored.
// M3 lane 3 r225 (plain; base 1766, frame 0x314, ret 8 against retail's ret 4 with rotation in EAX; fast builds it differently,
// frame 0x30c): `result[0] = coefficients[0]` as the first statement 1766 (same score, aligned 3009 against 3005); the matrix pointer
// declared ahead of the rotation reads 1766; rotation[7] read straight after the other eight 1766; the matrix pointer dropped for
// `workspace.matrix[...]` at every use 2027 (10501 bytes, frame 0x304, ret 8); the first copy first with rotation[7] in line 1766.
// None moves rotation into a register: the entry statement order is not the convention's cause.
// Lane 5 r166 (fast, 0x22850/0x23690/0x143250 listed, control 1766/223/401/MATCH): the nine leading rotation copies
// dropped and rotation[] read at each use (matrix a plain array) 2516 (10021 bytes; rotation then arrives in ESI, 0x23690 350);
// parameters as (result, rotation, coefficients, order), definition, declaration and all seven calls updated, 1766 (callers
// unchanged); the matrix as its own array beside the eight grouped terms 2089 (10438); `result[0] = coefficients[0];` first
// 1766; the matrix pointer as `real *const` 1766; the matrix pointer defined right after the grouped terms (top of the
// body) 1766. Both builds compute the seven sqrt(N.0) terms at run time (seven fsqrt each). All restored.
// m3 lane 14 r77: fast (convention pinned by the last full build, as lane 10 round 34 found), 0x143250 matched each time:
// the workspace with matrix[49] as its first member 1766 (frame 0x314, ret 8); band one's five single-use negations and
// copies written into their matrix stores 1791 (10740 B; a subset of round 145's first-400 form). Re-measured listed
// forms: the matrix pointer declared ahead of the rotation reads 1766, term_10 dropped 1766 (same code). Plain checks of
// round 145's declaration-count cause: every single-use real term written into its consumer within band three only
// (945 written in, 396 declarations left) 2081 (10458 B, rotation EAX and ret 4, frame 0x308; callers 0x22850 223->226,
// 0x23690 401->366), bands two and three (284 left) 2104 (10433 B, frame 0x304, ret 4); disjoint lifetimes sharing one
// term name, all (152 declarations) 3684 (12498 B, frame 0x328, ret 4), the first 400 merges only (941 left) 3145
// (12049 B, frame 0x32c, ret 4, masked difflib count 699 against 1063): any way below about 950 declarations gives
// retail's convention, but each rewrites the body's allocation. Compile-only (no /GL): rotation is loaded once at entry
// under both /Gd (EAX) and /Gr (ECX). The call deletions tried under --fast said nothing about the convention (pinned).
// M3 lane 12 r111 (plain, callers 0x22850 223 and 0x23690 401 unchanged, 0x143250 matched): workspace fields in rotation-index
// order (r0 term_5, r1 term_1, r2 term_6, r3 term_7, r4 term_0, r5 term_2, r6 term_3, r8 term_4) 1766 (67 listing lines move);
// fields in band two's first-use order (term_7, term_5, term_0, term_1, term_3, term_2, term_6, term_4) 1766 (57 lines move);
// `term_10 = workspace.term_4;` right after term_8 (retail evaluates term_8, term_10, term_11, term_12, term_13) 1766 (same
// code); the workspace as plain locals with `real matrix[49]` declared first 2113 (10412 bytes, frame 0x300 as retail, ret 8); the plain
// terms declared in rotation-index order 2113. Body restored.
// Round 87 (m3 lane 11, plain with 0x22850/0x23690/0x143250): band one's nine matrix stores read from rotation[] (its five
// negation/copy terms dropped) 1787 (10745 B, ret 8, rotation still on the stack); with the nine leading copies after
// band one's early return 1830 (10706 B); the 37 copies of copies in bands two and three (`term_N = workspace.term_K`,
// `term_N = term_9`) read from rotation[] 1780 (10788 B): rotation then arrives in ESI and result in EDI, but coefficients
// moves to the stack (still ret 8; 0x23690 401 -> 350); both 1776 (10792 B); band two's 22 only 2050 (11035 B, rotation
// ESI). Use count alone gives rotation a register but never a third register parameter (retail EAX/ECX/ESI).
// m3 lane 11 r90: no trial run. The lever this note records (fewer than about 950 declared locals for retail's EAX rotation and
// ret 4) has a dozen measured families above; each reaches the convention only by rewriting the allocation (1775-3684), and a new
// attempt needs the 1341 term declarations re-expressed per band rather than another one-step form.
// M3 lane 2 r72 (plain checks with 0x22850/0x23690/0x143250; base 1766, aligned 2934, callers 223/401 unchanged in each,
// 0x143250 matched): retail ends `ret 4` (rotation EAX) and ours `ret 8` as the prompt says (both listings' last lines).
// Bands two and three nested under `if (order > 2)` / `if (order > 3)` with one `return result;` 1766 (same code); rotation
// read through one `real const *` local copy at the nine reads 1766 (same code); every term declared at its first
// assignment instead of at the top 1766 (same code); the workspace declared after the 1341 terms (plain; earlier rounds
// measured it fast only, where the convention is pinned) 1766 (same code); the two root products term_41/term_221 stored as
// real 1766 (aligned 2952). Diagnostic (not kept): `#pragma optimize("p", on)` around the body 3960 (12712 bytes), so
// retail's source-order schedule is not the float-consistency setting.
// M3 lane 12 r120 (plain checks with 0x22850/0x23690/0x143250; the terms named term_N above are the old one-operation-per-statement
// body, replaced this round). Retail walk: its 0x300 frame is 49 matrix slots and 143 scalar homes with 215 stores for 952 values,
// so most values never touch memory; each band-two product that band three needs is stored to two homes at once (band two's own
// and a temporary read once in band three), and every xmm value is reloaded after each 0x143250 call. Causes found, in order:
// band three's copies of band-two products written as the product again 2614 (11558 B; gives the paired stores); 0x143250
// defined after this function 1766 (same code); 0x143250 declared `inline` 1819 (10714 B; callers of an out-of-line `inline`
// callee assume every xmm register clobbered, so the rotation values are reloaded as in retail; 0x143250 still matches); the
// body rewritten as expressions (nine rotation locals in retail's load order, band two's twelve repeated products named, each
// entry one expression, band three recomputing its products) with that declaration 1681 (frame 0x2f8, ret 4); the nine rotation
// locals declared r0..r8 (a product's later-declared operand is its first operand; retail's 267 operand pairs fit one
// declaration order), band two's two roots as float locals ahead of the products (`fxch st(1); fxch st(1); fmulp`, the x87
// code delayed to the first use as in retail), a product chain's unsorted head kept together as a parenthesized right operand
// (a plain chain is flattened and sorted by declaration order, constants last), entries 28, 29, 34 and 43 as plain sums of
// products (the compiler then factors them as retail has them) 1403 (10742 B, frame 0x2fc); r5_r0 declared after r4_r2, and
// the three products retail computes on their own between two entries (every use reads the home) declared as locals ahead
// of their first entry, 1333 (kept). Harness forms (build/r120/h, standalone LTCG link, positional preview in brackets against 1279 kept): both r5 pair
// locals declared between r4_r2 and r3_r2, where retail's entry 8 order puts them [1470; entry 8 then factors into three
// sums]; entry 8 with the pairs inline [1496, 1664]; every entry as a plain sum [1631; 61 entries equal]; the fourth
// between-entries product (r2_r2_s105 * s21_s2) as a local ahead of entry 19 as retail computes it [1536: the allocation at
// band three's start shifts]; the three r5 products inline everywhere [1368; operand order of S * (r5 * r1) lost]; root
// locals declared at their conversion instead of at the constant load [1574]. Left: entry 8's term order (retail r3_r2,
// r5_r1, r5_r0, r4_r2, r4_r0, r3_r1 unfactored), entry 25's four-term order, entry 28's subtracted pair, and the homes, which
// retail orders by memory-reference count (r8 with 24 reads at +0, r5 with 20 at +4, r7 with 3 at +0x11c).
// M3 lane 9 r120 (fast unless given): retail's scalar homes are ordered by reference count, and its extra home comes from two
// band-three products (stored at retail 0x144008 and 0x1444eb) that keep their own two-reference homes where ours packs them into slots
// shared with other values, so the frame follows the band-three schedule. Entry 8's first two terms swapped 1333 (same code:
// the sum is re-sorted, later-declared operand first); r3_r2 declared after r5_r1 (entry 8 then has retail's term order) 939 at
// 10777 bytes, plain 939 with 0x22850 226, 0x23690 330 and 0x143250 matched (a declaration-order change, kept only with a
// match: restored, diff in that round's held patch); on it entry 25's four operand products in the 23 other declaration
// orders 943-1474, entry 28 as T1 - (T2 + T3) 940; that grouping on this body 1330. Left on the 939 form: band three's
// preamble (the copies of band-two products into band-three homes, through xmm7 where retail uses xmm6, in another order),
// entries 25-28, and band two's entries 18, 19 and 22. A hill climb of dependency-safe adjacent declaration swaps on
// the 939 form (fast, 290 swaps) then took s105_r0_r0_r6 after r1_r0_r7 866, r7_r7_r0 after r0_r0_r0 865, r3_r3_r0
// after r1_r1_r0 854 and r2_r1 after r5_r4 853 (plain 853 at 10777 bytes with frame 0x300 as retail, first difference
// +0x3b; 0x22850 226, 0x23690 330, 0x143250 matched); every other single swap 858-1500. Restored like the 939 form.
// P476: Explicit-polynomial matrix accumulators in bands 2/3/both; coefficient accumulators in bands 2/3/both; immutable scalar captures; frequency lexical homes. Scores 1337/1535/1539/1439/1535/1539/1333/1333.
// NOT kept: unmatched diagnostic source forms; original executable body restored.
// P478: First-consumer repeated scaled mixed/cubic/triple/pair/cubic products, paired cubic family,
// complete reusable family and prepublication family: 1552/1509/1333/1759/1597/1623/1628/1619.
// NOT kept: four products recover frame 0x300, but spill homes/schedules regress; original body restored.
// P478 m3: Cross-band square/mixed/all/axial-square/transverse-square retained products,
// then all/square/mixed shared receivers with original second producers: 1918/1843/2010/1811/1827/1538/1529/1501.
// NOT kept: lifetimes regress and shorten away from retail; original polynomial restored. Frame and homes remain.
// P480 m3: Complete third-band row-producer lifetimes 0/1/2/3/4, second-band square/all
// product first-consumer declarations and all third-band products: 1810/1643/1742/1648/1627/1672/1707/1789.
// NOT kept: native frame/homes unrecovered; original polynomial and complete body restored.
// @retail 0x143600
real *function_143600(real const *rotation, unsigned long order, real const *coefficients, real *result)
{
	real matrix[49];
	real r0 = rotation[0];
	real r1 = rotation[1];
	real r2 = rotation[2];
	real r3 = rotation[3];
	real r4 = rotation[4];
	real r5 = rotation[5];
	real r6 = rotation[6];
	real r7 = rotation[7];
	real r8 = rotation[8];

	result[0] = coefficients[0];
	matrix[0] = r4;
	matrix[1] = 0.0f - r7;
	matrix[2] = r1;
	matrix[3] = 0.0f - r5;
	matrix[4] = r8;
	matrix[5] = 0.0f - r2;
	matrix[6] = r3;
	matrix[7] = 0.0f - r6;
	matrix[8] = r0;
	function_143250(result + 1, 3, matrix, coefficients + 1);
	if (order <= 2)
		return result;
	{
		real s15 = (real)sqrt(15.0);
		real s5 = (real)sqrt(5.0);
		real s5_s15 = s5 * s15;
		real r3_r0 = r3 * r0;
		real r4_r1 = r4 * r1;
		real r6_r3 = r6 * r3;
		real r7_r4 = r7 * r4;
		real r6_r6 = r6 * r6;
		real r7_r7 = r7 * r7;
		real r6_r0 = r6 * r0;
		real r7_r1 = r7 * r1;
		real r0_r0 = r0 * r0;
		real r1_r1 = r1 * r1;
		real r3_r3 = r3 * r3;
		real r4_r4 = r4 * r4;
		matrix[0] = r4 * r0 + r3 * r1;
		matrix[1] = 0.0f - r7 * r3 - r6 * r4;
		matrix[2] = s5_s15 * r7 * r6 * 0.2f;
		matrix[3] = 0.0f - r7 * r0 - r6 * r1;
		matrix[4] = r1 * r0 - r4 * r3;
		matrix[5] = 0.0f - r4 * r2 - r5 * r1;
		matrix[6] = r7 * r5 + r8 * r4;
		matrix[7] = s5_s15 * r8 * r7 * -0.2f;
		matrix[8] = r7 * r2 + r8 * r1;
		matrix[9] = r5 * r4 - r2 * r1;
		matrix[10] = (r5 * r2 * 0.13333334f - r3_r0 * 0.06666667f) * s5_s15 - r4_r1 * s5_s15 * 0.06666667f;
		matrix[11] = r6_r3 * s5_s15 * 0.06666667f - s5_s15 * r8 * r5 * 0.13333334f
			+ r7_r4 * s5_s15 * 0.06666667f;
		matrix[12] = r8 * r8 - r6_r6 * 0.5f - r7_r7 * 0.5f;
		matrix[13] = r6_r0 * s5_s15 * 0.06666667f - s5_s15 * r8 * r2 * 0.13333334f
			+ r7_r1 * s5_s15 * 0.06666667f;
		matrix[14] = s5_s15 * (r2 * r2) * 0.06666667f - (r5 * r5 * 0.06666667f + r0_r0 * 0.033333335f) * s5_s15
			- r1_r1 * s5_s15 * 0.033333335f + (r4_r4 + r3_r3) * s5_s15 * 0.033333335f;
		matrix[15] = 0.0f - r5 * r0 - r3 * r2;
		matrix[16] = r8 * r3 + r6 * r5;
		matrix[17] = s5_s15 * r8 * r6 * -0.2f;
		matrix[18] = r8 * r0 + r6 * r2;
		matrix[19] = r5 * r3 - r2 * r0;
		matrix[20] = r3_r0 - r4_r1;
		matrix[21] = r7_r4 - r6_r3;
		matrix[22] = r6_r6 * s5_s15 * 0.1f - r7_r7 * s5_s15 * 0.1f;
		matrix[23] = r7_r1 - r6_r0;
		matrix[24] = (r4_r4 + r0_r0) * 0.5f - r1_r1 * 0.5f - r3_r3 * 0.5f;
		function_143250(result + 4, 5, matrix, coefficients + 4);
	}
	if (order <= 3)
		return result;
	{
		real s2 = (real)sqrt(2.0);
		real r3_r3 = r3 * r3;
		real r3_r0 = r3 * r0;
		real r3_r3_r4 = r3_r3 * r4;
		real r4_r1 = r4 * r1;
		real r6_r6 = r6 * r6;
		real r3_r0_r1 = r3_r0 * r1;
		real r0_r0 = r0 * r0;
		real r0_r0_r4 = r0_r0 * r4;
		real r1_r1 = r1 * r1;
		real r1_r1_r4 = r1_r1 * r4;
		real r4_r4 = r4 * r4;
		real r4_r4_r4 = r4_r4 * r4;
		real s35 = (real)sqrt(35.0);
		real s35_s2 = s35 * s2;
		real s105 = (real)sqrt(105.0);
		real s21 = (real)sqrt(21.0);
		real s21_s35 = s21 * s35;
		real s105_s35_s2 = s105 * s35_s2;
		real r3_r1 = r3 * r1;
		real r3_r1_r6 = r3_r1 * r6;
		real r4_r1_r7 = r4_r1 * r7;
		real r4_r0 = r4 * r0;
		real r4_r0_r6 = r4_r0 * r6;
		real r3_r0_r7 = r3_r0 * r7;
		real r6_r6_r4 = r6_r6 * r4;
		real r7_r7 = r7 * r7;
		real r7_r7_r4 = r7_r7 * r4;
		real r6_r3 = r6 * r3;
		real r6_r3_r7 = r6_r3 * r7;
		real s7 = (real)sqrt(7.0);
		real s7_r7 = s7 * r7;
		real s7_r7_r6_r6 = s7_r7 * r6_r6;
		real s7_r7_r7_r7 = s7 * r7_r7 * r7;
		real r6_r0 = r6 * r0;
		real r6_r0_r7 = r6_r0 * r7;
		real r6_r6_r1 = r6_r6 * r1;
		real r7_r7_r1 = r7_r7 * r1;
		real r4_r3 = r4 * r3;
		real r4_r3_r6 = r4_r3 * r6;
		real s105_r3_r3 = s105 * r3_r3;
		real s105_r3_r3_r7 = s105_r3_r3 * r7;
		real s105_r1_r1 = s105 * r1_r1;
		real s105_r1_r1_r7 = s105_r1_r1 * r7;
		real r1_r0 = r1 * r0;
		real r1_r0_r6 = r1_r0 * r6;
		real s105_r0_r0 = s105 * r0_r0;
		real s105_r0_r0_r7 = s105_r0_r0 * r7;
		real s105_r4_r4 = s105 * r4_r4;
		real s105_r4_r4_r7 = s105_r4_r4 * r7;
		real r1_r1_r1 = r1_r1 * r1;
		real r3_r0_r4 = r3_r0 * r4;
		real r4_r4_r1 = r4_r4 * r1;
		real r0_r0_r1 = r0_r0 * r1;
		real r3_r3_r1 = r3_r3 * r1;
		real r4_r2 = r4 * r2;
		real r5_r0 = r5 * r0;
		real r3_r2 = r3 * r2;
		real s105_s2 = s105 * s2;
		real s7_s105 = s7 * s105;
		real r5_r1 = r5 * r1;
		real s105_s2_s21 = s105_s2 * s21;
		real r6_r4 = r6 * r4;
		real r6_r1 = r6 * r1;
		real r6_r5 = r6 * r5;
		real r6_r2 = r6 * r2;
		real r2_r0 = r2 * r0;
		real r2_r1 = r2 * r1;
		real r5_r4 = r5 * r4;
		real r5_r3 = r5 * r3;
		real r2_r2 = r2 * r2;
		real r5_r2 = r5 * r2;
		real r8_r8 = r8 * r8;
		real s21_s2 = s21 * s2;
		real r5_r5 = r5 * r5;
		real r5_r5_s105 = r5_r5 * s105;
		real r2_r2_s105 = r2_r2 * s105;
		real s7_s2 = s7 * s2;
		real s35_r5 = s35 * r5;
		real s35_r5_r1_r1 = s35_r5 * r1_r1;
		real s7_s2_s35 = s7_s2 * s35;
		real r3_r0_r2 = r3_r0 * r2;
		real r4_r1_r2 = r4_r1 * r2;
		real s35_r4_r4_r5 = s35 * r4_r4 * r5;
		real s35_r3_r3_r5 = s35 * r3_r3 * r5;
		real s35_r5_r0_r0 = s35_r5 * r0_r0;
		real r3_r0_r8 = r3_r0 * r8;
		real r3_r2_r6 = r3_r2 * r6;
		real r5_r0_r6 = r5_r0 * r6;
		real r4_r1_r8 = r4_r1 * r8;
		real r5_r1_r7 = r5_r1 * r7;
		real r4_r2_r7 = r4_r2 * r7;
		real s7_s2_s21 = s7_s2 * s21;
		real r6_r3_r8 = r6_r3 * r8;
		real s21_r5 = s21 * r5;
		real s21_r5_r6_r6 = s21_r5 * r6_r6;
		real r8_r7 = r8 * r7;
		real r8_r7_r4 = r8_r7 * r4;
		real s21_r5_r7_r7 = s21_r5 * r7_r7;
		real r7_r7_r8 = r7_r7 * r8;
		real r6_r6_r8 = r6_r6 * r8;
		real r8_r7_r1 = r8_r7 * r1;
		real r6_r0_r8 = r6_r0 * r8;
		real s21_r2 = s21 * r2;
		real s21_r2_r6_r6 = s21_r2 * r6_r6;
		real s21_r2_r7_r7 = s21_r2 * r7_r7;
		real r2_r1_r7 = r2_r1 * r7;
		real r3_r3_r8 = r3_r3 * r8;
		real r5_r3_r6 = r5_r3 * r6;
		real r0_r0_r8 = r0_r0 * r8;
		real r4_r4_r8 = r4_r4 * r8;
		real r1_r1_r8 = r1_r1 * r8;
		real r2_r0_r6 = r2_r0 * r6;
		real r5_r4_r7 = r5_r4 * r7;
		real s35_r2 = s35 * r2;
		real s35_r2_r4_r4 = s35_r2 * r4_r4;
		real r3_r0_r5 = r3_r0 * r5;
		real s35_r0_r0_r2 = s35 * r0_r0 * r2;
		real s35_r1_r1_r2 = s35 * r1_r1 * r2;
		real r4_r1_r5 = r4_r1 * r5;
		real s35_r2_r3_r3 = s35_r2 * r3_r3;
		real r4_r0_r1 = r4_r0 * r1;
		real r4_r4_r3 = r4_r4 * r3;
		real r0_r0_r3 = r0_r0 * r3;
		real r3_r3_r3 = r3_r3 * r3;
		real r1_r1_r3 = r1_r1 * r3;
		real r3_r0_r6 = r3_r0 * r6;
		real r4_r1_r6 = r4_r1 * r6;
		real r3_r1_r7 = r3_r1 * r7;
		real r4_r0_r7 = r4_r0 * r7;
		real r6_r6_r3 = r6_r6 * r3;
		real r6_r4_r7 = r6_r4 * r7;
		real r7_r7_r3 = r7_r7 * r3;
		real s7_r6 = s7 * r6;
		real s7_r6_r7_r7 = s7_r6 * r7_r7;
		real s7_r6_r6_r6 = s7 * r6_r6 * r6;
		real r6_r1_r7 = r6_r1 * r7;
		real r6_r6_r0 = r6_r6 * r0;
		real s105_r4_r4_r6 = s105_r4_r4 * r6;
		real s105_r3_r3_r6 = s105_r3_r3 * r6;
		real r4_r3_r7 = r4_r3 * r7;
		real s105_r1_r1_r6 = s105_r1_r1 * r6;
		real s105_r0_r0_r6 = s105_r0_r0 * r6;
		real r1_r0_r7 = r1_r0 * r7;
		real r3_r1_r4 = r3_r1 * r4;
		real r7_r7_r0 = r7_r7 * r0;
		real r0_r0_r0 = r0_r0 * r0;
		real r4_r4_r0 = r4_r4 * r0;
		real r3_r3_r0 = r3_r3 * r0;
		real r1_r1_r0 = r1_r1 * r0;
		matrix[0] = r0_r0_r4 * 0.75f + r3_r0_r1 * 1.5f - r3_r3_r4 * 0.75f - r1_r1_r4 * 0.75f + r4_r4_r4 * 0.25f;
		matrix[1] = (r4_r1_r7 * 0.014285714f - r3_r1_r6 * 0.014285714f) * s105_s35_s2
			- r4_r0_r6 * s105_s35_s2 * 0.014285714f - r3_r0_r7 * s105_s35_s2 * 0.014285714f;
		matrix[2] = r6_r6_r4 * s21_s35 * 0.035714287f - r7_r7_r4 * s21_s35 * 0.035714287f
			+ r6_r3_r7 * s21_s35 * 0.071428575f;
		matrix[3] = (s7_r7_r7_r7 * 0.035714287f - s7_r7_r6_r6 * 0.10714286f) * s35_s2;
		matrix[4] = (r6_r6_r1 * 0.035714287f + r6_r0_r7 * 0.071428575f) * s21_s35
			- r7_r7_r1 * s21_s35 * 0.035714287f;
		matrix[5] = (s105_r1_r1_r7 + s105_r3_r3_r7) * s35_s2 * 0.007142857f
			+ r4_r3_r6 * s105_s35_s2 * 0.014285714f - r1_r0_r6 * s105_s35_s2 * 0.014285714f
			- s105_r0_r0_r7 * s35_s2 * 0.007142857f - s105_r4_r4_r7 * s35_s2 * 0.007142857f;
		matrix[6] = r0_r0_r1 * 0.75f - (r3_r0_r4 * 1.5f + r1_r1_r1 * 0.25f) + r4_r4_r1 * 0.75f
			- r3_r3_r1 * 0.75f;
		real r4_r0_s105_s35_s2 = r4_r0 * s105_s35_s2;
		matrix[7] = r4_r3 * s105_s35_s2 * r5 * 0.014285714f - r3_r1 * s105_s35_s2 * r2 * 0.014285714f
			- r5_r0 * s105_s35_s2 * r1 * 0.014285714f - r4_r0_s105_s35_s2 * r2 * 0.014285714f;
		matrix[8] = r3_r2 * r7 + r5_r1 * r6 + r5_r0 * r7 + r4_r2 * r6 + r4_r0 * r8 + r3_r1 * r8;
		matrix[9] = r6_r4 * s105_s2_s21 * r8 * -0.023809524f - s105_s2_s21 * r8 * r7 * r3 * 0.023809524f
			- r6_r5 * s105_s2_s21 * r7 * 0.023809524f;
		matrix[10] = s7_s105 * r8 * r7 * r6 * 0.14285715f;
		matrix[11] = r6_r1 * s105_s2_s21 * r8 * -0.023809524f - s105_s2_s21 * r8 * r7 * r0 * 0.023809524f
			- r6_r2 * s105_s2_s21 * r7 * 0.023809524f;
		matrix[12] = r2_r0 * r7 + r2_r1 * r6 + r1_r0 * r8 - r5_r4 * r6 - r5_r3 * r7 - r4_r3 * r8;
		matrix[13] = (r3_r2 * r4 + r3_r1 * r5) * s105_s35_s2 * 0.014285714f
			- r1_r0 * s105_s35_s2 * r2 * 0.014285714f + r4_r0_s105_s35_s2 * r5 * 0.014285714f;
		real r5_r5_s21_s35 = r5_r5 * s21_s35;
		matrix[14] = (r2_r2 * r4 * 0.028571429f - r0_r0_r4 * 0.007142857f) * s21_s35
			- s21_s35 * r1_r1_r4 * 0.021428572f
			+ (r5_r1 * r2 * 0.057142857f + r3_r3_r4 * 0.007142857f) * s21_s35
			- r5_r5_s21_s35 * r4 * 0.028571429f + s21_s35 * r4_r4_r4 * 0.007142857f
			- s21_s35 * r3_r0_r1 * 0.014285714f;
		real r5_r2_s105_s2_s21 = r5_r2 * s105_s2_s21;
		matrix[15] = s105_s2_s21 * r4_r0_r6 * 0.004761905f
			- (s105_s2_s21 * r5_r1 * r8 + r5_r2_s105_s2_s21 * r7) * 0.01904762f
			- s105_s2_s21 * r4_r2 * r8 * 0.01904762f
			+ ((r3_r0_r7 + r3_r1_r6) * 0.004761905f + r4_r1_r7 * 0.014285714f) * s105_s2_s21;
		matrix[16] = r8_r8 * r4 - r7_r7_r4 * 0.75f + r8_r7 * r5 * 2.0f - r6_r3_r7 * 0.5f - r6_r6_r4 * 0.25f;
		matrix[17] = (s7_r7_r6_r6 * 0.035714287f + s7_r7_r7_r7 * 0.035714287f) * s21_s2
			- s21_s2 * r8_r8 * s7_r7 * 0.14285715f;
		matrix[18] = r8_r7 * r2 * 2.0f - r6_r6_r1 * 0.25f - r7_r7_r1 * 0.75f + r8_r8 * r1 - r6_r0_r7 * 0.5f;
		matrix[19] = r5_r4 * s105_s2_s21 * r8 * 0.01904762f + s21_s2 * s105_r1_r1_r7 * 0.007142857f
			- s21_s2 * s105_r4_r4_r7 * 0.007142857f + r5_r5_s105 * s21_s2 * r7 * 0.00952381f
			- s21_s2 * s105_r3_r3_r7 * 0.0023809525f - r2_r1 * s105_s2_s21 * r8 * 0.01904762f
			+ s105_s2_s21 * r1_r0_r6 * 0.004761905f - r7 * (r2_r2_s105 * s21_s2) * 0.00952381f
			- s105_s2_s21 * r4_r3_r6 * 0.004761905f + s21_s2 * s105_r0_r0_r7 * 0.0023809525f;
		matrix[20] = (r3_r3_r1 * 0.007142857f - r4_r2 * r5 * 0.057142857f + r3_r0_r4 * 0.014285714f) * s21_s35
			- r0_r0_r1 * s21_s35 * 0.007142857f
			+ (r2_r2 * r1 * 0.028571429f + r4_r4_r1 * 0.021428572f) * s21_s35
			- r5_r5_s21_s35 * r1 * 0.028571429f - r1_r1_r1 * s21_s35 * 0.007142857f;
		matrix[21] = r3_r0_r2 * s7_s2_s35 * 0.042857144f + s35_r5_r1_r1 * s7_s2 * 0.021428572f
			- s35_r5 * s7_s2 * r2_r2 * 0.042857144f + r4_r1_r2 * s7_s2_s35 * 0.042857144f
			- s35_r4_r4_r5 * s7_s2 * 0.021428572f - s35_r3_r3_r5 * s7_s2 * 0.021428572f
			+ (r5_r5 * s35 * r5 * 0.014285714f + s35_r5_r0_r0 * 0.021428572f) * s7_s2;
		matrix[22] = r5_r2 * s7_s105 * r8 * 0.057142857f - r3_r0_r8 * s7_s105 * 0.028571429f
			- r3_r2_r6 * s7_s105 * 0.028571429f - r5_r0_r6 * s7_s105 * 0.028571429f
			- r4_r1_r8 * s7_s105 * 0.028571429f - r5_r1_r7 * s7_s105 * 0.028571429f
			- r4_r2_r7 * s7_s105 * 0.028571429f;
		matrix[23] = r6_r3_r8 * s7_s2_s21 * 0.071428575f - s21_r5 * s7_s2 * r8_r8 * 0.071428575f
			+ (s21_r5_r7_r7 + s21_r5_r6_r6) * s7_s2 * 0.035714287f + r8_r7_r4 * s7_s2_s21 * 0.071428575f;
		matrix[24] = r8_r8 * r8 - (r6_r6_r8 + r7_r7_r8) * 1.5f;
		matrix[25] = r6_r0_r8 * s7_s2_s21 * 0.071428575f + s21_r2_r7_r7 * s7_s2 * 0.035714287f
			+ s21_r2_r6_r6 * s7_s2 * 0.035714287f + r8_r7_r1 * s7_s2_s21 * 0.071428575f
			- s21_r2 * s7_s2 * r8_r8 * 0.071428575f;
		matrix[26] = (r5_r3_r6 * 0.028571429f + r3_r3_r8 * 0.014285714f - r2_r1_r7 * 0.028571429f) * s7_s105
			- r0_r0_r8 * s7_s105 * 0.014285714f + r4_r4_r8 * s7_s105 * 0.014285714f
			- r1_r1_r8 * s7_s105 * 0.014285714f - r2_r0_r6 * s7_s105 * 0.028571429f
			+ (r2_r2 * r8 + r5_r4_r7) * s7_s105 * 0.028571429f - r5_r5 * s7_s105 * r8 * 0.028571429f;
		matrix[27] = s35_r2 * s7_s2 * r5_r5 * 0.042857144f
			- (r2_r2 * s35 * r2 * 0.014285714f + s35_r2_r3_r3 * 0.021428572f) * s7_s2
			- s35_r2_r4_r4 * s7_s2 * 0.021428572f - r3_r0_r5 * s7_s2_s35 * 0.042857144f
			+ (s35_r1_r1_r2 + s35_r0_r0_r2) * s7_s2 * 0.021428572f - r4_r1_r5 * s7_s2_s35 * 0.042857144f;
		matrix[28] = r4_r4_r3 * s21_s35 * 0.007142857f - r5_r5_s21_s35 * r3 * 0.028571429f
			- r4_r0_r1 * s21_s35 * 0.014285714f - r0_r0_r3 * s21_s35 * 0.021428572f
			+ r2_r2 * r3 * s21_s35 * 0.028571429f + r5_r0 * r2 * s21_s35 * 0.057142857f
			+ r3_r3_r3 * s21_s35 * 0.007142857f - r1_r1_r3 * s21_s35 * 0.007142857f;
		matrix[29] = r3_r0_r6 * s105_s2_s21 * 0.014285714f - r5_r2_s105_s2_s21 * r6 * 0.01904762f
			- s105_s2_s21 * r5_r0 * r8 * 0.01904762f + r3_r1_r7 * s105_s2_s21 * 0.004761905f
			+ r4_r1_r6 * s105_s2_s21 * 0.004761905f - s105_s2_s21 * r3_r2 * r8 * 0.01904762f
			+ r4_r0_r7 * s105_s2_s21 * 0.004761905f;
		matrix[30] = r6_r5 * r8 * 2.0f - r6_r6_r3 * 0.75f - r6_r4_r7 * 0.5f - r7_r7_r3 * 0.25f + r8_r8 * r3;
		matrix[31] = (s7_r6_r7_r7 * 0.035714287f + s7_r6_r6_r6 * 0.035714287f) * s21_s2
			- s7_r6 * s21_s2 * r8_r8 * 0.14285715f;
		matrix[32] = r8_r8 * r0 - r6_r1_r7 * 0.5f - r6_r6_r0 * 0.75f - r7_r7_r0 * 0.25f + r6_r2 * r8 * 2.0f;
		matrix[33] = r5_r3 * s105_s2_s21 * r8 * 0.01904762f
			- (s105_r4_r4_r6 * s21_s2 * 0.0023809525f + r6 * (r2_r2_s105 * s21_s2) * 0.00952381f)
			- s105_r3_r3_r6 * s21_s2 * 0.007142857f - r4_r3_r7 * s105_s2_s21 * 0.004761905f
			+ s105_r1_r1_r6 * s21_s2 * 0.0023809525f - r2_r0 * s105_s2_s21 * r8 * 0.01904762f
			+ (r5_r5_s105 * r6 * 0.00952381f + s105_r0_r0_r6 * 0.007142857f) * s21_s2
			+ r1_r0_r7 * s105_s2_s21 * 0.004761905f;
		matrix[34] = r3_r1_r4 * s21_s35 * 0.014285714f - r5_r5_s21_s35 * r0 * 0.028571429f
			- r0_r0_r0 * s21_s35 * 0.007142857f + r2_r2 * r0 * s21_s35 * 0.028571429f
			+ r3_r3_r0 * s21_s35 * 0.021428572f + r4_r4_r0 * s21_s35 * 0.007142857f
			- r1_r1_r0 * s21_s35 * 0.007142857f - r3_r2 * s21_s35 * r5 * 0.057142857f;
		matrix[35] = s35_r5_r1_r1 * s105_s2 * 0.007142857f - s35_r5_r0_r0 * s105_s2 * 0.007142857f
			+ s35_r3_r3_r5 * s105_s2 * 0.007142857f + r4_r1_r2 * s105_s35_s2 * 0.014285714f
			- r3_r0_r2 * s105_s35_s2 * 0.014285714f - s35_r4_r4_r5 * s105_s2 * 0.007142857f;
		matrix[36] = r5_r0_r6 + r3_r2_r6 + r3_r0_r8 - r4_r1_r8 - r4_r2_r7 - r5_r1_r7;
		matrix[37] = s21_r5_r7_r7 * s105_s2 * 0.011904762f - s21_r5_r6_r6 * s105_s2 * 0.011904762f
			- r6_r3_r8 * s105_s2_s21 * 0.023809524f + r8_r7_r4 * s105_s2_s21 * 0.023809524f;
		matrix[38] = r6_r6_r8 * s7_s105 * 0.071428575f - r7_r7_r8 * s7_s105 * 0.071428575f;
		matrix[39] = (r8_r7_r1 * 0.023809524f - r6_r0_r8 * 0.023809524f) * s105_s2_s21
			- s21_r2_r6_r6 * s105_s2 * 0.011904762f + s21_r2_r7_r7 * s105_s2 * 0.011904762f;
		matrix[40] = r4_r4_r8 * 0.5f - r1_r1_r8 * 0.5f - r3_r3_r8 * 0.5f + r0_r0_r8 * 0.5f - r5_r3_r6 - r2_r1_r7
			+ r5_r4_r7 + r2_r0_r6;
		matrix[41] = (s35_r1_r1_r2 * 0.007142857f - s35_r0_r0_r2 * 0.007142857f + s35_r2_r3_r3 * 0.007142857f) * s105_s2
			+ r3_r0_r5 * s105_s35_s2 * 0.014285714f - s35_r2_r4_r4 * s105_s2 * 0.007142857f
			- r4_r1_r5 * s105_s35_s2 * 0.014285714f;
		matrix[42] = r0_r0_r3 * 0.75f - r1_r1_r3 * 0.75f - r4_r0_r1 * 1.5f + r4_r4_r3 * 0.75f
			- r3_r3_r3 * 0.25f;
		matrix[43] = r3_r1_r7 * s105_s35_s2 * 0.014285714f + r4_r1_r6 * s105_s35_s2 * 0.014285714f
			- r3_r0_r6 * s105_s35_s2 * 0.014285714f + r4_r0_r7 * s105_s35_s2 * 0.014285714f;
		matrix[44] = (r6_r6_r3 * 0.035714287f - r6_r4_r7 * 0.071428575f) * s21_s35
			- r7_r7_r3 * s21_s35 * 0.035714287f;
		matrix[45] = (s7_r6_r7_r7 * 0.10714286f - s7_r6_r6_r6 * 0.035714287f) * s35_s2;
		matrix[46] = r6_r6_r0 * s21_s35 * 0.035714287f
			- (r7_r7_r0 * 0.035714287f + r6_r1_r7 * 0.071428575f) * s21_s35;
		matrix[47] = r1_r0_r7 * s105_s35_s2 * 0.014285714f + s105_r3_r3_r6 * s35_s2 * 0.007142857f
			- s105_r0_r0_r6 * s35_s2 * 0.007142857f - s105_r4_r4_r6 * s35_s2 * 0.007142857f
			+ s105_r1_r1_r6 * s35_s2 * 0.007142857f - r4_r3_r7 * s105_s35_s2 * 0.014285714f;
		matrix[48] = r4_r4_r0 * 0.75f - r1_r1_r0 * 0.75f - r3_r3_r0 * 0.75f + r0_r0_r0 * 0.25f
			+ r3_r1_r4 * 1.5f;
		function_143250(result + 9, 7, matrix, coefficients + 9);
	}
	return result;
}
