#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"
#include "reset_storage.h"
#include <math.h>
#include "normalize3d.h"
#include "vector_products.h"

// @flags /O2 /Ob1 /Gr /arch:SSE
#include "flags_storage.h"
#include "projection_storage.h"

#define k_real_epsilon 0.0001f
#define k_real_max 3.4028234663852886e+38f

struct s_31f50_object
{
	byte unknown00[0xc];
	long child;
	long next;
	byte unknown14[0x1c];
	point3f center;
	real radius;
};

struct s_31f50_header
{
	byte unknown00[8];
	s_31f50_object *object;
};

PRIVATE inline real bounds_vector_length(vector3f const *vector)
{
	return (real)sqrt(vector->k * vector->k + vector->j * vector->j + vector->i * vector->i);
}

// @retail 0x31f50
void __stdcall function_31f50(long index, point3f const *origin, vector3f const *axis, real *radial, real *axial)
{
	if (index != NONE)
	{
		s_31f50_object *object = ((s_31f50_header *)g_4e0300->data)[index & 0xffff].object;
		vector3f perpendicular;
		vector3f parallel;
		vector3f delta;
		delta.i = object->center.x - origin->x;
		delta.j = object->center.y - origin->y;
		delta.k = object->center.z - origin->z;
		real projection = axis->k * delta.k + axis->j * delta.j + axis->i * delta.i;
		parallel.i = axis->i * projection;
		parallel.j = axis->j * projection;
		parallel.k = axis->k * projection;
		perpendicular.i = delta.i - parallel.i;
		perpendicular.j = delta.j - parallel.j;
		perpendicular.k = delta.k - parallel.k;
		real width = bounds_vector_length(&perpendicular);
		real length = bounds_vector_length(&parallel);
		if (object->radius + width > *radial)
			*radial = object->radius + width;
		if (object->radius + length > *axial)
			*axial = object->radius + length;
		if (object->child != NONE)
			function_31f50(object->child, origin, axis, radial, axial);
		if (object->next != NONE)
			function_31f50(object->next, origin, axis, radial, axial);
	}
}

/* ---- globals ---- */
#define BIT(n) ((bool)((g_4ba014 >> (n)) & 1))
long g_4ba134;

/* the vector scaled and rotated into the camera's frame (no translation) */
__inline void transform_vector(s_camera const *camera, vector3f const *v, vector3f *out)
{
	real x = v->i, y = v->j, z = v->k;
	if (camera->scale != 1.f)
	{
		x = camera->scale * x;
		y = camera->scale * y;
		z = camera->scale * z;
	}
	out->i = camera->right.i * x + camera->up.i * y + camera->forward.i * z;
	out->j = camera->right.j * x + camera->up.j * y + camera->forward.j * z;
	out->k = camera->right.k * x + camera->up.k * y + camera->forward.k * z;
}

/* the screen bounds of a sphere (centre p, radius) seen through the camera:
   the tangents to it in the horizontal and vertical planes, projected */
// @retail 0x30290
void function_30290(point3f const *p, s_view const *view, s_camera const *camera, real radius, box2f *bounds)
{
	/* read through its address, so the radius stays in its stack slot as in retail */
	real const *radius_home = &radius;
	real const &radius_value = *radius_home;
	vector3f delta;
	vector3d_from_points3d(&view->position, p, &delta);
	transform_vector(camera, &delta, &delta);
	real cy = delta.j;
	real cz = delta.k * -1.f;

	if (cz + radius_value > 0.01f)
	{
		real m78 = camera->unknown78;
		real m98 = camera->unknown98;
		real m8c = camera->unknown8c;
		real m9c = camera->unknown9c;
		box2f half;
		half.x0 = view->bounds.left * 0.5f;
		half.y0 = view->bounds.top * 0.5f;
		half.x1 = view->bounds.right * 0.5f;
		half.y1 = view->bounds.bottom * 0.5f;

		real xz = delta.i * delta.i + cz * cz;
		real r2 = radius_value * radius_value;
		real disc = xz - r2;
		if (disc > k_real_epsilon)
		{
			real root = (real)sqrt(disc);
			real k = radius_value / xz;
			real za = cz - (cz * radius_value + root * delta.i) * k;
			real xb = delta.i - (delta.i * radius_value - root * cz) * k;
			real zb = cz - (cz * radius_value - root * delta.i) * k;
			if (za > 0.01f)
			{
				real xa = delta.i - (delta.i * radius_value + root * cz) * k;
				real n = xa / za * m78 + m98;
				bounds->x0 = (1.f - n) * half.x0 + (n + 1.f) * half.x1;
			}
			else
				bounds->x0 = -k_real_max;
			if (zb > 0.01f)
			{
				real n = xb / zb * m78 + m98;
				bounds->x1 = (1.f - n) * half.x0 + (n + 1.f) * half.x1;
			}
			else
				bounds->x1 = k_real_max;
		}
		else
		{
			bounds->x0 = -k_real_max;
			bounds->x1 = k_real_max;
		}

		real yz = cy * cy + cz * cz;
		real disc2 = yz - r2;
		if (disc2 > k_real_epsilon)
		{
			real root = (real)sqrt(disc2);
			real k = radius_value / yz;
			real za = cz - (cz * radius_value + root * cy) * k;
			real yb = cy - (cy * radius_value - root * cz) * k;
			real zb = cz - (cz * radius_value - root * cy) * k;
			if (za > 0.01f)
			{
				real ya = cy - (cy * radius_value + root * cz) * k;
				real n = ya / za * m8c + m9c;
				bounds->y0 = (1.f - n) * half.y0 + (n + 1.f) * half.y1;
			}
			else
				bounds->y0 = -k_real_max;
			if (zb > 0.01f)
			{
				real n = yb / zb * m8c + m9c;
				bounds->y1 = (1.f - n) * half.y0 + (n + 1.f) * half.y1;
			}
			else
				bounds->y1 = k_real_max;
		}
		else
		{
			bounds->y0 = -k_real_max;
			bounds->y1 = k_real_max;
		}
	}
	else
	{
		bounds->x0 = k_real_max;
		bounds->x1 = -k_real_max;
		bounds->y0 = k_real_max;
		bounds->y1 = -k_real_max;
	}
}

// @retail 0x30710
bool function_30710(s_camera const *camera, short_rectangle2d const *rect, vector3f const *v, point2f *out, s_view const *view)
{
	bool result = false;
	if (!rect)
		rect = &view->bounds;
	if (v->k < 0.f)
	{
		real width = (real)(rect->right - rect->left);
		real height = (real)(rect->bottom - rect->top);
		real inv = -1.f / v->k;
		out->x = (camera->unknown78 * v->i + camera->unknown98 * v->k) * inv;
		out->y = 0.f - (camera->unknown8c * v->j + camera->unknown9c * v->k) * inv;
		if (out->x >= -1.f && 1.f >= out->x && out->y >= -1.f && 1.f >= out->y)
			result = true;
		real scaled_x = (out->x + 1.f) * 0.5f;
		out->x = scaled_x * width + (real)rect->left;
		real scaled_y = (out->y + 1.f) * 0.5f;
		out->y = scaled_y * height + (real)rect->top;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
	}
	return result;
}

__inline void transform_point(s_camera const *camera, point3f const *p, point3f *out)
{
	real x = p->x, y = p->y, z = p->z;
	if (camera->scale != 1.f)
	{
		x = camera->scale * x;
		y = camera->scale * y;
		z = camera->scale * z;
	}
	out->x = camera->right.i * x + camera->up.i * y + camera->forward.i * z + camera->position.x;
	out->y = camera->right.j * x + camera->up.j * y + camera->forward.j * z + camera->position.y;
	out->z = camera->right.k * x + camera->up.k * y + camera->forward.k * z + camera->position.z;
}

// score 187 (was 195): declared (d, a, camera, ...) and returning bool (`d` is set up after `a` at both calls and both
// callers test the result as a byte; 0x4ff50 then matches). Earlier at 195: one note (the stacked score lines merged, r184, r116 and again r289); r155, r116, r286 unchanged; r289 found the cause (symbol-count state) and a HELD lead that matches, see the end of this note.
// one note (the stacked score lines merged, r184, again r116 and r143); r155, r116, r286, r143 unchanged.
// MiniMax E (11-20): no new form, no lower score (195): the frame is still 0x30 against retail's 0x2c, both inverse
// defaults still sit at +0xc and in an argument slot where retail keeps them at +4 and +8, and the transformed points
// still take the opposite component order.
// MiniMax S (11-20): no new form, no lower score (195): the frame is 0x30 against retail's 0x2c, so every local home sits four bytes higher; retail's caller 0x4f7e0 sets the arguments up as three pushed values, then ECX = camera, EDX = a, ESI = the displacement vector and EBX = the output point.
// MiniMax D (11-20): no new form, no lower score (195): the frame is still 0x30 against retail's 0x2c, retail keeps both inverse
// defaults at [esp+4] and [esp+8] with the displaced point at +0xc/+0x10/+0x14 where this build keeps the inverses at +0xc and in
// the dead scale slot and spills the second transform's scaled x and z; the written operand order of the three displaced sums is
// ignored by the compiler, and a helper taking the three components as scalars (with the reversed point as three locals, as
// retail's third transform reads) shrinks the frame to 0x24 but reorders the sums and the scale block (214), so the pointer form stands.
// Earlier first line (merged into the one above, r289): 195 (was 195): one note (the stacked score lines merged, r184, and again r116); r155, r116 unchanged.
// Earlier first line: one note (the stacked score lines merged, r184, and again r116); r155, r116 unchanged.
// MiniMax C (11-19): no new form, no lower score (195): frame 0x30 against retail's 0x2c, the two zeroed floats at [esp+0xc] and [esp+0x38] against retail's [esp+4] and [esp+8], and the three b-sums take the other side of the add.
// MiniMax B (11-20): no new form, no lower score (195): the frame is still 0x30 against retail's 0x2c, both
// inverse defaults still sit at +0xc and in an argument slot where retail keeps them at +4 and +8, and the
// transformed points still take the opposite component order. The shared point transform at the origin,
// displaced, both and reversed sites was measured at 423/376/529/359/538/536/718 in an earlier round, all worse,
// and the in-place displaced transform and the perspective-only inverse locals measure 263 and 261. No form
// found this round.
// Earlier findings. one note (the stacked score lines merged, r184).
// Merged header (r184): 195 (was 195), one note.
// Frame 0x30 versus 0x2c. Ours gives the dead scale slot to the second inverse
// depth and spills the second transform's x and z (x into the frame); retail keeps both inverse
// defaults at [esp+4]/[esp+8], keeps that transform's x/y in xmm7/xmm6 (ours swaps x and z) and spills
// only z, into the scale slot. Also differ: the displaced point's load order and our camera-space
// origin point above the displaced one. In-place displaced transform 263; later displaced input 270;
// perspective-only inverse locals 261; zero-factor early exit 195; both
// displaced transforms in place 263.
// MiniMax S (11-17): no new form, no lower score (195): the frame is 0x30 against retail's 0x2c, the two inverse defaults sit at +0xc and in an argument slot where retail keeps them at +4 and +8, the displaced point takes y, x, z in that order where retail takes x, y, z, and the transformed points keep the opposite order.
// MiniMax D (run 46): no new form, no lower score.
// MiniMax C (11-17): 195, no lower score; the frame is still 0x30 against retail's 0x2c
// because the second point transform spills both its scaled x and scaled z into two local slots, where
// retail keeps them in xmm7 and xmm4 and spills only the scaled z into the scale parameter's home, so our
// inverse defaults land in one local and in that dead parameter slot instead of the two fresh locals at
// [esp+4] and [esp+8]. Tried: the three orders of the scaled components inside transform_point, a hoisted
// scale local and the swapped multiply operands, all 195 with identical code.
// Frame 0x30 versus 0x2c. Ours gives the dead scale slot to the second inverse
// depth and spills the second transform's x and z (x into the frame); retail keeps both inverse
// defaults at [esp+4]/[esp+8], keeps that transform's x/y in xmm7/xmm6 (ours swaps x and z) and spills
// only z, into the scale slot. Also differ: the displaced point's load order and our camera-space
// origin point above the displaced one. In-place displaced transform 263; later displaced input 270;
// perspective-only inverse locals 261; zero-factor early exit 195; both
// displaced transforms in place 263.
// Later second inverse 259; later first inverse 271; later second output
// declaration 195; native displaced-point copy 315; direct predicate 195.
// Direction-first x/z and origin-first y sums together, x/z alone, y alone
// 195 each; inverse-depth names 195; native displaced-point aggregate 195.
// Later paired factor publication 197; depth-first displaced point 216; outer displacement
// scalars 195; zero-output failure test 195; chained zero stores 195; opening 195 restored.
// frame 0x30 versus 0x2c and projected-point/factor homes differ;
// role-named points 195; displaced transform first 250; orthographic defaults first 261;
// shared point initializer 195; single result 280.
// Native transform input/result/both by value 402/314/288; helper-local
// scale observation 225; orthographic arm first 215. Frame and point/factor homes remain.
// Origin/camera/scale fallback selections, all three paired selections and all together 195 each;
// native floating factor admission 195. Complete original body restored.
// Origin-depth snapshot 195; displaced-depth snapshot 195; paired depths 195;
// y-first displaced construction 218; scale shared through existing helper 235.
// frame 0x30 versus 0x2c remains; initial/reversed/both
// component loops 195/195/195; forced existing transform with no/initial/reversed/
// both loops 195/195/195/195; component loops with displaced storage reused 195.
// Shared point transform at origin/displaced/both/reversed/origin plus reversed/displaced plus reversed/all
// 423/376/529/359/538/536/718; all with local inline declaration 718.
// All nonexact shared-transform forms restored, including their floating sum order; no external change remains.
// Native point input/output/camera references 195/195/195; paired point/all references 195/195;
// parallel named pre-translation sums 263.
// Round 115: Origin/displaced/both reciprocal selections 195/195/195; guarded helper coordinate initialization 293; negate selections 195; initial displacement scope ends after transforms 195. Opening body/helper restored.
// Round 117: Transformed storage first 195; chained inverse defaults 195; complete inverse results 296; helper coordinate homes 195; depth-first inputs 225; output-pointer result 195.
// Parallel transform product values right/up/right+up/forward 265/276/251/264;
// right+forward/up+forward/all 251/251/213; all with y-first output 224.
// Complete products recover frame 0x2c but regress; opening helper and body restored.
// Recovered-frame inverse declaration order/names and displaced sums/combined 211/213/213/211;
// original frame 0x30 and inverse/point homes remain.
// Both inverse defaults after origin/camera/all fallbacks 195/203/202; displaced-only/origin-
// only default after all fallbacks 204/204. All retain frame 0x30.
// Round 161: Complete translated-row results in all six row computation orders
// 195 each; those results with shared helper-local scale, x/y/z and z/y/x,
// 252/252. Original helper snapshots, row trees and executable body restored.
// Native input/camera/output point records singly, pairs, all and all/shared scale [248, 693, 334, 693, 690, 693, 693, 693];
// forced native camera/output/both expansion [443, 287, 456]. Original helper and body restored.
// Round 327: corresponding two-product sums at each row 259; all partial sums before stores in
// xyz/xzy/yxz/yzx/zxy/zyx order 259 each; partial rows with shared scale 279.
// Round 59: the displaced point's sum operands follow parameter order, not text: d-first text 195
// (same code); `d` declared before `a` gives retail's operand order with the same registers, 193
// (diagnostic, through a forwarder: both callers, 0x4f7e0 and 0x4ff50, are in contributor files).
// Reverse displacement written into the same point 195 (same code: the slots were already
// shared); transformed points declared tb, ta 195; inverse defaults declared b, a 195. Retail keeps
// the second transform's x in the register b.x was built in and spills only z (to the dead
// scale slot); ours moves x and spills x and z to two frame slots.
// Round 192: five other internal transform pointer-parameter orders 195 each; reverse order
// with actual camera/point/both reference inputs 195 each. Every definition/call binding,
// original snapshots and full body restored; frame 0x30 versus 0x2c remains.
// DL round 156: uniform final projection products, all/origin/displaced/const 195 each;
// projected origin/displaced/both aggregate/both component inputs 197/196/197/197.
// Frame 0x30 and original transform, inverse defaults and expression trees restored.
// Round 58: transform helper locals renamed 195; inverse locals renamed 195; both 195 (identical code:
// names do not move these slots); transformed points declared tb before ta 195; displaced point declared
// after the transformed points 195. Ours also stores ta above tb (retail: ta below tb).
// Round 61 (lane 7): both inverses as one `real[2]` 195 (in the frame, but the second transform still
// spills into it); with retail's displaced-sum operand order 195 (identical code); camera-space points
// as `point3f[2]` 195 (origin point below as retail, homes otherwise unchanged); transform helper
// locals declared z, y, x 228; second inverse declared first 195.
// Round 151: Native right/up/right+up/forward/right+forward/up+forward/all/all+translation captures 261/257/690/257/690/690/690/690; forced pair/pair/pair/all expansion 476/471/476/639. Singles recover frame 0x2c but change input ownership; combined unforced helper outlines. All restored.
// Original frame 0x30 versus 0x2c and inverse/point/factor homes remain; native basis captures do not qualify.
// Round 28 (m3 lane 12): the inverses used directly, without fa/fb copies (non-perspective path sets both
// to 1), gives retail's registers for the whole inverse/zero/output tail and 974 bytes, but 207; with b.y
// computed before b.x the displaced point also takes retail's xmm7/xmm6, 206 (971 bytes); z, y, x order 201
// (registers wrong); ta/tb declared first 206. Left: b at +0x10 (retail +0xc, frame 0x30 v 0x2c) and the second
// inlined transform spilling two scaled components where retail spills one. Retail-order sum operands 195
// (same code); a `d`-first parameter order needs callers in contributor files.
// Round 14: forced shared transform 718; helper camera home 234 (frame 0x2c); with snapshots
// y,x,z / x,z,y / y,z,x / z,x,y / z,y,x 234/207/204/231/227; best plus outer reversed point 204;
// outer reversed point 195; pointer-returning helper with it 195. All restored.
// DL175: Uniform transform term permutations, every parallel product named, early uninitialized
// reversed-point scratch and direct inverse factors [195, 195, 195, 195, 195, 213, 195, 207]. All
// arithmetic-order forms are exact-only; original helper and body restored.
// Round 219: Native matrix-row loops long/dword/short/word 214/214/477/477;
// with shared scale 258/258/495/495. Forced long/short, original/shared scale 214/405/258/410.
// Long-row frame is retail 0x2c; inverse-order/direct factors/both/y-first/y-first with order/
// y-first with direct factors/descending rows/descending with direct factors 213/262/261/214/213/254/231/218.
// All restored; inverse/point homes and scaled-component allocation remain.
// Round 436: row compound accumulators through translation/at publication/first pair 238/238/262;
// parallel compound rows through translation/at publication/first pair 238/238/262;
// output accumulation through translation/final two contributions 238/262. All original helper/body restored.
// Round 446: Existing vector scale/rotation stage with direct/native point publication 222/295;
// with translation copy 264/690; aggregate vector input gives the same four scores.
// Original point helper restored; simplest frame 0x1c, combined value form outlines.
// Round 451: Signed depth intervals at reversed/origin/displaced test subsets 1/2/1+2/3/1+3/2+3/all,
// then all upper-first: 227/216/248/250/242/244/245/214. Original absolute tests and helper
// restored.
// Round 256: Guarded coordinate acquisition xzy/yxz/yzx/zxy/zyx gives 287/293/293/287/293;
// coherent scale with xyz/yzx/zyx gives 301/301/301. Original shared guard, alias-safe snapshots
// and body restored; frame 0x30 remains.
// Round 468: Displaced reciprocal first, with factor admission/publication/perspective-zero
// scope subsets 0..7: 239/215/227/197/264/283/267/283. Original inverse evaluation, defaults, factors
// and complete body restored; frame 0x30 and displaced-transform spill remain.
// Round 471: Separate raw/scaled coordinate families, default-copy/explicit-branch/conditional/
// native-point conditional, field/shared scale: 225/199/265/269/316/306/690/230. Original
// alias-safe helper, frame defaults, inverse graph and complete body restored.
// Round 475: Component-owned compound scale updates with input snapshot orders
// xyz/xzy/yxz/yzx/zxy/zyx: 195/227/230/237/225/228; explicit component-first products
// with xyz/zyx snapshots 195/228. Original alias-safe helper and body restored;
// frame 0x30 versus 0x2c and the second transform spill remain.
// Round 25 (lane 18, fast): orthographic arm first 215 (repeat); the direct-inverse graph with b.y first
// re-measured 206 (b.x/b.y in retail's xmm7/xmm6, but the second transform's x and z take xmm4/xmm7 where
// retail has xmm7/xmm4, so ours spills both and retail only z, into the scale slot); on it the helper reading
// z, x, y 213; the displaced point as a copy plus compound adds 303; the orthographic arm first 205.
// Round 230: Origin/displaced inverse defaults staggered across the first/second transform
// boundaries: entry-between/between-entry/both-between/entry-before/before-entry/both-before/
// between-after/after-between 237/237/272/208/208/204/282/274. Both final staggered forms
// recover frame 0x2c but regress. Original defaults restored.
// Round N284: Native real absolute-value APIs at the three epsilon consumers,
// masks 1..7 195/195/195/195/195/195/195; all with displaced-factor admission first 219.
// Original absolute tests, inverse defaults, alias-safe helper and complete body restored.
// Round 477: Native translation at entry/post-scale with no/right/up/forward axis capture 261/690/690/690/260/690/690/690.
// Translation alone recovers frame 0x2c but changes the scale carrier; paired captures outline. Original alias-safe helper and complete body restored.
// Round 483: independent reverse-displacement admission with entry/perspective
// inverse defaults, immediate/paired factor publication and either reciprocal
// order 195/239/197/227/294/285/294/285. Original guard, defaults, factor
// producers and full body restored; frame and displaced-transform spill remain.
// Round 485: Existing scale helper only, native branch/in-place result and field/coherent
// scale, 283/251/282/251; with all parallel row products 272/257/272/257.
// All native-coordinate forms rejected; original alias-safe helper, inverse defaults,
// caller bindings and full body restored. Frame and second-transform spill remain.
// Round 131 (lane 5, fast): displaced point through the existing vector3f_add, displacement first, 195
// (same code); locals named so the first point and inverse sort after the second's 195 (same code: names
// do not move these slots); round 28's direct inverses with y-first displaced point re-measured 206 (971 B:
// b.x/b.y in retail's xmm7/xmm6, but the second inlined transform still moves x out of xmm7 and spills x
// and z where retail keeps x in xmm7 and spills only z); on it z, x, y displaced order 204 (971 B);
// helper coordinates declared z, y, x 218 (966 B). Original body restored.
// Round 230: Origin/displaced inverse defaults staggered across the first/second transform
// boundaries: entry-between/between-entry/both-between/entry-before/before-entry/both-before/
// between-after/after-between 237/237/272/208/208/204/282/274. Both final staggered forms
// recover frame 0x2c but regress. Original defaults restored.
// Round 501: alias-protected native scaled input before/after the scale branch, field/loop
// scaling and repeated/coherent scale observations 267/267/258/258/269/269/260/260.
// Original alias-safe snapshots, helper and body restored; inverse/point homes remain.
// Round 477: Native translation at entry/post-scale with no/right/up/forward axis capture 261/690/690/690/260/690/690/690.
// Translation alone recovers frame 0x2c but changes the scale carrier; paired captures outline. Original alias-safe helper and complete body restored.
// Round 483: independent reverse-displacement admission with entry/perspective
// inverse defaults, immediate/paired factor publication and either reciprocal
// order 195/239/197/227/294/285/294/285. Original guard, defaults, factor
// producers and full body restored; frame and displaced-transform spill remain.
// Round 485: Existing scale helper only, native branch/in-place result and field/coherent
// scale, 283/251/282/251; with all parallel row products 272/257/272/257.
// All native-coordinate forms rejected; original alias-safe helper, inverse defaults,
// caller bindings and full body restored. Frame and second-transform spill remain.
// Round 503: Existing row dots, coefficient/input operand order, immediate/eager
// coefficient rows and immediate/complete result publication 263/263/447/447/447/447/256/256.
// All native-coordinate forms are nonexact and restored; original alias-safe helper,
// snapshots, expressions and calls remain. Frame and second-transform spill remain.
// Round 505: Complete branch-owned return predicate, long/bool, entry/after-factor declaration,
// predicate after/before output stores: 203/235/203/235/210/245/210/245.
// All regress; original early returns, alias-safe helper and frame defaults restored.
// Round 507: Direct translated-row publication xzy/yxz/yzx/zxy/zyx 216/213/227/220/230;
// complete parallel product publication xzy/yzx/zyx 214/225/227. Product forms recover
// frame 0x2c but not inverse homes; original alias-safe helper and body restored.
// Round 60 (m3 lane 1, fast): transform helper with the scale branch reading the point's fields and the else branch
// copying them (no shared snapshot) 285 (893 bytes); with the displaced point's y first 298 (884); with each row
// summed right, forward, up 285 (893); the displaced point's y first alone 218; rows summed right, forward, up alone
// 195 (same code). Retail passes out in EBX and d in ESI (camera ECX, a EDX; scale, perspective, negate on the stack).
// Round 513: Complete native transform input/output library transfers, input memcpy/memmove,
// output memcpy/memmove, then both APIs in all four pairs: 248/294/334/295/690/685/687/692.
// Input memcpy recovers frame 0x2c but regresses; original alias-safe helper and body restored.
// Round 57 (lane 11, fast): displaced point y, x, z / z, x, y 218/216 (982 bytes), each with the reverse displacement
// written into the same point 218/216 (same code); displaced order (x, y, z first) by helper coordinate declaration order:
// xyz base zyx 228, yzx 237, yxz 230, xzy 227, zxy 225; y-first 230, 236, 207, 227, 225, xyz 218; z-first 227, 232, 223, 225,
// 224, xyz 216; helper scale statements reordered zyx 229, yzx 217, yxz 199, xzy 201, zxy 205, with y-first displaced
// 229, 217, 222, 224, 228. The second transform's colours stay x XMM4, y XMM6, z XMM7 (retail XMM7, XMM6, XMM4).
// Round 523: Direct inverse factors with complete raw/scaled copy/branch/conditional/native-point
// families, field/coherent scale, 213/227/262/301/301/277/679/226. Final form reaches 955 bytes
// but not the frame/homes; original alias-safe helper, inverse defaults and full body restored.
// Round 525: Genuine coefficient-column cursors with indexed/cursor translation and output,
// repeated/coherent scale, eight graphs 278/276/278/275/278/275/277/276. Original helper and body restored; frame and displaced-transform spills remain.
// Round 527: Eager admission/eager returned comparisons/library zero-output subsets
// 1/2/4/3/5/6/7 give 238/243/194/269/237/239/265; all with a Boolean result accumulator 285.
// One-point clearing lead rejected; original comparisons, output stores, includes, helper and full body restored; frame and inverse/displaced-transform homes remain.
// F342: Displaced-input/inverse-workspace last-use scopes and complete translated point setter, masks
// 1/2/4/3/5/6/7; all with reversed inverse constants: 195/195/195/195/195/195/195/195. NOT kept: no
// byte match; original complete body and every helper restored. Inverse homes and displaced-transform
// spill remain.
// Round 77 (m3 lane 9, fast): both factors as `fabs(z) < k_real_epsilon ? 0.f : 1.f / z` without the inverse
// defaults 301 (890 bytes); fa/fb initialised 1.f with no orthographic arm 261 (906); the displaced inverse
// computed before the origin one 239 (1004); b, ta and tb declared together 195 (same code); the success test as
// `return dx != 0.f || dy != 0.f;` 195 (same code). Restored.
// Round 549: Paired native double inverse/factor/delta residence, masks 1..7:
// 264/262/276/232/238/266/260; all with double reciprocal producers 260.
// Both inverse/factor pairs recover frame 0x2c at 276; delta forms reach 955 bytes
// but regress. NOT kept: nonexact precision changes; original body restored.
// Round 554: Native double transform input/scale/complete-row accumulation, seven
// families 690/465/690/690/690/690/690; all with direct inverse factors 679.
// NOT kept: nonexact precision forms outline or change scalar code; original alias-safe
// helper and full body restored. Frame and inverse/displaced-transform homes remain.
// Round 158 (m3 lane 4, fast): retail's sum operands (direction, origin, direction first by component) 195; with the inverses declared b, a 195; fa/fb initialised to 1 with no else arm 261 (906 bytes);
// both inverse tests before the fa/fb copies 197; the inverses declared just above the perspective test 261 (926 bytes).
// Round 141 (m3 lane 6, fast): transform_point as __forceinline 195 (same code); the displaced point as an
// aggregate initializer 195; the displaced point in its own block around the second transform 270 (994 bytes),
// with the initializer 270; the success test returned directly with the zero stores after the block (no else) 195.
// Round 143 (m3 lane 6, fast): retail's sum operands re-measured 195 (same code); the point default moved after
// the camera and scale defaults 207; displaced sums written z, x, y 216 (982 B), z, y, x 213 (979 B); the origin
// transform ahead of the displaced sums 270 (994 B); fa/fb declared before ta/tb 195 (same code).
// Round 66 (m3 lane 11, fast): retail packs inv_a, inv_b, b, ta, tb at frame +0/+4/+8/+0x14/+0x20; ours
// inv_a +8, b +0xc, tb +0x18, ta +0x24, inv_b in the scale slot, +0/+4 unused. ta/tb as two declarations
// 195; the inverses in one declaration 195; ta/tb declared at the top 195; the inverse defaults declared
// after the displaced point 204 (974 bytes), after both transforms 261 (926 bytes).
// Y562: Existing same-camera vector transform, distinct/in-place vector result, scalar/setter
// point publication, original/direct inverse factors: 222/221/222/221/240/239/240/239. Original alias-safe helper and body restored.
// Y564: Factor selection deferred until both native reciprocals, independent conditional/
// independent branches/joint branch/perspective-default/orthographic-default/immutable
// conditional: 195/195/197/295/293/195; deferred conditional/joint with late inverse
// defaults 283/280. NOT kept: no gain; original helper, defaults and complete body restored.
// Y573: Complete source direction/origin and immutable displaced-input ownership, masks
// 1/2/3/4/5/6/7: 285/336/364/195/285/336/364; full graph with native direction 341.
// NOT kept: no gain; input-record copies and changed observation timing miss equality.
// Original alias-safe helper and full body restored; frame and inverse/point homes remain.
// Y575: Shared final projected-point difference with native input construction, displaced
// setter and unit-scale reverse difference, eight graphs: 197 each. NOT kept: all regress;
// native frame/inverse homes and displaced-transform spill remain. Original full body restored.
// Y577: Native final planar delta, field/aggregate producers, scalar/record/memcpy/memmove
// publication: 195/251/251/272/195/251/251/272. NOT kept: no equality; grouped residence
// and copies tie or regress. Original helper/body restored; inverse homes and displaced-transform spill remain.
// Round 63 (m3 lane 14, fast): the parameters as (d, a, camera, scale, ...) with every declaration and call changed 193
// (retail's calls set up EBX out, ECX camera, EDX a, ESI d, last to first in that order; retail's three displaced-sum load
// orders become exact, caller 0x4ff50 aligned 12 -> 9, 0x4f7e0 108 -> 105; under the keep bar, restored): lead, keep it
// for a form that fixes the second transform. Also (d, camera, a) 193; (camera, d, a) 193; (camera, a, d) 195; (a, d, camera)
// 195. On (d, a, camera): displaced rows xzy 217, yxz 216 (b.x, b.y then in retail's XMM7, XMM6), yzx 213, zxy 216, zyx 211;
// helper coordinate/scale orders, 36 forms 183-235, best zyx with zyx 183 (973 B) but aligned 49 -> 71: retail's first transform
// runs x, y, z, so not kept; yxz rows with coordinate orders xzy 225, yxz 205, yzx 234, zxy 225, zyx 230. Old parameters: rows
// xzy 217, yxz 218, yzx 213, zxy 216, zyx 213; identity pointer chains of 1..7 locals ahead of the sums 195 each (same
// code). Retail's second and third transforms share a rule: the component computed first (z, then x) goes through XMM0 into
// XMM4 while the other two stay in XMM7/XMM6. Ours gets this in the third but gives x XMM4 in the second.
// AC545: Uniform final scale products, paired/row-local named parts and
// paired/row-local delta ownership 195 each; on direct reciprocal ownership
// 207 each. NOT kept: ties or regressions; native frame and second-transform
// spill remain. Original helper, defaults and complete body restored.
// Y583: Standard bounded transform input copy/backward-copy/uninitialized-copy
// 282/282/282; output equivalents 295/295/295; paired copy/backward-copy 692/692.
// NOT kept: nonexact record transfer and lost native frame/expansion; original
// alias-safe helper/body restored; inverse homes and displaced X/Z spill remain.
// Round 586: Two-transform source-table/destination-array traversal, long/dword/short/word
// ordinals with indexed/body-advanced destination: 289/289/337/337/195/195/322/322. NOT kept: nonexact
// traversal/grouped residence; original direct calls, helper and full body restored.
// Round 588: Standard scaled-point multiplication, branch/copy input, factor/component
// first and direct/native output: 693/691/693/691/693/691/693/691; forced expansion of
// the four direct outputs 525/527/525/527. NOT kept: helper outlines or frame/length
// regress. Original alias-safe helper/body restored; inverse homes and X/Z spill remain.
// Y590: Standard binary displacement producers at initial/direction-first/reverse/both/
// direction-first both and seeded initial/reverse/both: 270/270/280/314/293/294/318/360.
// NOT kept: allocation or extent regresses; original complete helper/body restored.
// Native inverse homes and second-transform X/Z spill remain.
// Y592: Existing transform with separate scalar outputs, pointer/reference, native/scalar result storage, native/scalar input:
// 195/195/259/259/228/228/242/242. NOT kept: no native frame/spill recovery; original alias-safe helper and body restored.
// Y594: Plain paired transformed-point/inverse-depth reciprocal table traversal,
// for/while with long/dword/short/word ordinals: 197/197/224/224/197/197/224/224.
// NOT kept: native table residence is nonexact; narrow ordinals recover frame
// 0x2c but shorten to 937/955. Original alias-safe helper and body restored.
// Round 130 (m3 lane 7, fast): the first cause is the second transform: retail keeps its x in the displaced point's XMM7, ours
// moves x to XMM4 and spills x and z; that then gives ours the 1.0 in XMM6 (retail XMM7), the second inverse stored in the
// scale slot (retail a register phi) and fa/zero swapped. Parameters (d, a, camera, scale) 193 (re-measured, 2 points); on it rows
// zxy 216, zyx 211, yxz 216 (x XMM7, y XMM6 as retail, the transform still moves x), xzy 217. Defaults in an array, point2f or
// scalars read in the else arm, if/else, else/if, ternary: 301/297/301 each (folded, 890-894 B). Scale read through a pointer
// to the camera parameter 234, with the order 232, with yxz 232 (971 B); separate result scalars 195/193; displaced point
// through a pointer local 216.
// Round 217 (m3 lane 3, fast; m = masked aligned count, base 195 m82): the parameters (d, a, camera, scale, ...) with every declaration and call 193 (m82, under the bar); fa/fb dropped, the inverses used directly with `inv_a = inv_b = 1.f` in the orthographic arm 207 (974 B, m53),
// the two stores in either order or chained the other way 207; that with the parameters 205 (m53); on it displaced rows zyx 199 (968 B, m50), yzx 201, zxy 204, yxz 204, xzy 205; helper coordinates declared z, x, y 213 (963 B, m113), z, y, x 216, y, x, z 216 (971 B). Frame stays 0x30.
// Round 66 (m3 lane 14, fast): the inverses used directly (no fa/fb) re-measured 207 (974 bytes); the scale default applied
// inside the negate block 222 (972); the displaced and reversed points through the existing vector3f_add_scaled with 1.f and
// -1.f, both/forward only/reverse only, 195/195/195 (same code: the unit factors fold); the reversed retransform under its own
// `perspective && fabs(tb.z) < epsilon` guard ahead of the factor block 195 (same code).
// AC555: Existing scale-comparison truth, bool/byte/word/long, unequal-scale
// and unit-equality polarity, 270 each (1001/955 bytes). NOT kept: all regress;
// original alias-safe helper/body restored; frame and inverse/X/Z homes remain.
// Y604: Complete rotation followed by actual component translation, output/native
// rotated storage with indexed for/while, pointer and standard traversal:
// 237/237/338/347/236/236/690/306. NOT kept: no equality; indexed output reaches
// native length but loses native allocation. Original alias-safe helper/body restored.
// N396: Complete ordered epsilon outcomes, direct/exhaustive/default-false/default-true
// bool and byte producers: eight at 195. NOT kept: no equality or gain; original complete body
// restored, native inverse homes and the second-transform X/Z spill remain.
// MiniMax D (11-06): no new form, no lower score.
// b6S (11-06, fast build 195, 985 bytes vs retail 955): statement-by-statement walk against retail, no behaviour difference
// found. Checked: a/camera/scale defaults (cmove 0x30838 loads the pointer stored at 0x468788, the other two are addresses
// 0x30854/0x3085d); b = a + d with the displaced sums (0x30872-0x30884); both transforms' scale tests at 0x30866/0x30935
// and rows; perspective flag test 0x30988 (je 0x30b48 = orthographic: both factors 1.0, zero test skipped); `|tb.z| < eps` retransform
// of a - d (0x30a15 jbe, 0x30a26-0x30a3d) and the re-test of tb.z after it (0x30b2d-0x30b3d); `|ta.z| < eps` keeps the 0 default
// from [esp+4] (0x30b1c/0x30b27) and `|tb.z| < eps` the one from [esp+8] (0x30b3d/0x30b58); both factors tested against 0 with
// `jnp` = equal only (0x30b61-0x30b6a, so NaN passes, as `!=` does); dx/dy = tb*fb - ta*fa (0x30b79-0x30b8d); negate = 0 - scale*d
// (0x30b93-0x30bae); outputs stored before the success test (0x30bb9-0x30bbd), then `dx != 0 || dy != 0` -> 1 (0x30bcd) else 0
// (0x30be2); factor-zero path stores 0 to both outputs, returns 0 (0x30bd9). Stack args scale/perspective/negate = [esp+0x34/38/3c]
// (ret 0xc); callers 0x4f7e0 (0x4f951-0x4f969) and 0x4ff50 (0x500e3-0x500fb) push 1, 1, scale and use only al.
// Operand order only (not behaviour; adds commute exactly): retail sums b as d.i + a.x, a.y + d.j, d.k + a.z (0x30872-0x30884);
// the source has them the other way round. Writing retail's order gives 195 and the same code (the compiler picks the load order),
// not kept. transform_point (outside this edit) sums right, up, forward; retail sums x*[camera+4] + z*[camera+0x1c] first,
// then y*[camera+0x10] (0x308cc-0x308f2); an earlier round measured that order as 195 with the same code. Frame is still
// 0x30 vs retail 0x2c (retail packs inv_a, inv_b, b, ta, tb into 11 slots; its a - d point reuses b's y/z slots 0x30a49/0x30a4f).
// m3 lane 11 r110 (fast): the helper's scale bound by const reference 195 (same code); the displaced point bound
// by reference for its sums and the second transform 268 (1001 B); the two inverses bound by reference in the
// perspective block 195 (same code); the helper's rows through a camera reference 267 (998 B); the displacement
// bound as `vector3f const &displacement = *d;` for the sums 193 (the z row then loads d first as retail, like the
// (d, a, ...) parameter order without touching the callers; x still loads a first), with retail's operand text
// 193 (same code), with the rows z, y, x 211 (979 B). Restored.
// M3 lane 6 r190 (fast with 0x30bf0; callers 0x4f7e0/0x4ff50 unmatched at 325/43 throughout), each 195 with the same
// code: transform_point's rows summed forward*z + up*y + right*x + position (the order the file-local point transforms of
// 0x1ee340/0x1689b0/0xa76b0 use); that helper returning its output pointer; the displaced sums written in retail's operand
// order (d.i + a.x, a.y + d.j, d.k + a.z: the compiler picks the load order, ours stays a-first for x and z); the three
// defaults as `a = a ? a : g_468788;` and likewise; the helper scaling with `x *= camera->scale`. Body unchanged.
// Displacement seeded into the forward/reverse/both points by component stores 238/230/273,
// or by the point setter 238/230/273. Initialized forward/both points 238/273.
// NOT kept: all retain the larger frame and increase the difference count.
// In-place scaling in transform output fields, point input copy, scalar snapshot before final row stores
// 392; Branch-owned scaling in transform output fields, point input copy, scalar snapshot before final row
// stores 371; In-place scaling in transform output fields, component input copy, scalar snapshot before
// final row stores 230; Branch-owned scaling in transform output fields, component input copy, scalar
// snapshot before final row stores 250; In-place scaling in transform output fields, point input copy,
// complete transformed result 690; Branch-owned scaling in transform output fields, point input copy,
// complete transformed result 690; In-place scaling in transform output fields, component input copy,
// complete transformed result 338; Branch-owned scaling in transform output fields, component input copy,
// complete transformed result 341; Stage only Z in the transform output before the scale test, snapshot Z
// for all rows 197; Stage only Z in the transform output in the scale branches, snapshot Z for all rows
// 225; Stage only Z in the transform output before the scale test, read Z per row 234; Stage only Z in the
// transform output in the scale branches, read Z per row 281.
// NOT kept: Whole-point/scalar and three Z-only forms recover frame 0x2c but still regress, including
// unresolved inverse homes.
// N405: Signed displacement selection inside the transform as component
// branches, captured origin updates, a complete point or component conditionals
// 255/307/263/307. Explicit expansion repeats those four scores.
// NOT kept: the transform still needs the native X/Z and inverse allocation.
// M3 lane 13 r108 (fast with 0x30bf0): transform_point as a plain static function (this /Ob1 file keeps it out of line)
// 728 (465 B); the inverse defaults in one declaration 195 (same code); declared after the three parameter defaults 202
// (974 B, frame 0x30), after the displaced sums 204 (974 B); the orthographic factors as `fa = fb = 1.f;` 195. Lead, not a
// source form: retail's frame keeps ta and tb as memory points in name order (b, ta, tb, as address-taken locals sort),
// which a link-time inline of an out-of-line point transform such as 0x142570 (the same row code) would give; its /Ob1
// file keeps it a call in our build (a file-flag decision).
// M3 lane 3 r257 (fast; control 195, aligned differences exact / stack-offset-masked / fully masked 80 / 61 / 34). The
// positional 195 hides two separate causes. (1) The tail: with the factor copies after the perspective test (the
// orthographic arm setting the inverses to 1, then `fa = inv_a; fb = inv_b;`, the listed direct-inverse form) the tail is
// retail's: the second inverse as a register value with its default reloaded on its own edge, and the orthographic block
// laid out between them: 207 (974 B, aligned 57 / 33 / 21); only fb deferred 209 (977 B, 76 / 52 / 22), only fa 225
// (992 B, 62 / 38 / 26). (2) What is left on that form is the second transform alone: retail keeps the displaced x and y
// in the registers their sums were built in (XMM7, XMM6) and brings z back through XMM0 into XMM4 with one spill, ours
// moves x to XMM4 and y to XMM6 and spills x and z (the extra frame slot). On the 207 form the displaced sums written
// xzy 205, yxz 206, yzx 201, zxy 204, zyx 201 (968-971 B, aligned 54-56). On this body: the displaced depth named
// first and stored last 195 (same code); the helper scaling the depth from the point's field 238 (974 B); the helper's
// scale read once into a local 252 (983 B); the displaced point built after the origin transform 270 (re-measure); the
// helper's depth snapshot first 197 (973 B). Diagnostics, reverted: the reverse retransform removed 480 (704 B: the
// second transform then keeps x in the sum's register but moves y); a twin copy of the helper for the second, the
// third, or both later transforms 195 each (same code: the three inline copies do not share register choices).
// Round 416: The displaced-point transform spills X and Z and reserves 0x30 bytes, compared with one spill
// and a 0x2c frame in retail. That extra stack word displaces both inverse defaults. matched vector
// transform with direct translation into its output 211; matched vector transform with shared addition into
// its output 211; matched vector transform to a rotation local and direct translation 212; matched vector
// transform to a rotation local and shared translation 218; matched vector transform with direct translation
// into its output with deferred factor copies 234; matched vector transform with shared addition into its
// output with deferred factor copies 234; matched vector transform to a rotation local and direct
// translation with deferred factor copies 235; matched vector transform to a rotation local and shared
// translation with deferred factor copies 235. NOT kept: Reusing the existing vector transform and applying
// the translation separately shortens the body to 958 or 959 bytes but leaves an incorrect frame and raises
// the score. Deferring the factor copies raises it further. The opening transform and body are restored.
// m3 lane 1 r120 (fast with 0x30bf0 and 0x30710, both MATCH; control 195). Reading: in the second transform retail keeps the
// displaced x and y in the registers their sums were built in (XMM7, XMM6) as the transform's own x and y, loads z through
// XMM0 into XMM4 and gives only z a memory home (the dead scale slot) although XMM5 is free there; ours gives the transform's
// x, y, z XMM4, XMM6, XMM7, so x is moved and both x and z get homes (the extra frame word). A sum's register follows its
// first operand's base (the displacement's XMM7, the origin's XMM6), hence round 63's parameter-order lead. Tried: no-code
// mentions `(void)ta.z;` or `(void)tb.z;` after the two transforms 195/195 (same code); `(void)tb.x; (void)tb.y;` 263 (991 B);
// `(void)ta.x; (void)ta.y;` 251 (988 B); the direct-inverse form re-measured 207 (974 B), with `(void)tb.z;` 207, with the
// displacement reference for the sums 205. Compile-only (the stream agrees with the real build but two lines): mentions of the
// displaced point's or the retransform point's components, before or after their sums, in every ordered subset, change nothing.
// M3 lane 7 r184 (fast; control 195): lead: the three transforms read like inlined copies of the out-of-line point
// transform 0x142570, which this /Ob1 file cannot inline from another file (a flag lead, not a source form).
// transform_point with 0x142570's body (address-taken camera, rows forward*z + up*y + right*x) 234 (971 B,
// frame 0x2c as retail, the scaled values and products move); its original rows with only the address-taken camera
// 234; its body without the address-taken camera 195 (same code); the calls as 0x142570 itself with `inline` added to
// its definition (cross-file, diagnostic) 234 (same code as the helper) and 0x1396c7 48, 0x24e59f 42 lost; 1..16 dead
// locals on the 234 form 234-235 (symbol count moves nothing); parameters (d, a, camera, ...) with every
// declaration and call (callers are contributor code, so held) 193, with the 234 form 232.
// Origin reciprocal before the displaced transform, before displaced-point creation,
// before the perspective block, or before fallback: zero default 246/228/211/211;
// direct factor 240/253/232/232. The native extent alone leaves the larger frame.
// M3 lane 2 r102 (fast; control 195 at 985 B): the point helper given 0x142570's own parameter-address read (a named
// `s_camera const *const *` to the camera, the scale tested through it) 234 at 971 B: the frame becomes retail's 0x2c and
// the second transform spills one value into the scale parameter's home as retail does, but the first transform's x, y, z
// rotate to XMM0, XMM1, XMM4 (retail XMM4, XMM0, XMM1) and the second gives the spill to y; with the rows in 0x142570's
// order 234; the reference declared after the coordinates 234; the helper as 0x142570 written out (separate coordinate
// declarations, returning the output) 234; the scale read through a cast of the parameter's address instead 195 (same
// code: no new symbol); on the named-pointer form, zero to seven dead initialised long locals and zero to three real ones
// at the top 234 each (diagnostic: the rotation is not a symbol-count state). Body unchanged.
// m3 lane 5 r202 (fast with 0x30bf0 and the callers 0x4f7e0/0x4ff50, unchanged at 325/43; control 195, aligned 80): the point
// helper testing the scale through a const pointer to its camera parameter (0x142570's shape) 234 (971 B: the frame becomes
// 0x2c, but the defaults land at +8 and in an argument slot); the helper's x row summed term by term (forward*z, up*y,
// right*x, then position) with the y/z rows in that order 228 (972 B, frame 0x2c); both 216 (966 B); that product order as
// single expressions 195 (same code); the existing out-of-line 0x142570 called for the three transforms 718 (475 B: a call
// from this /Ob1 file).
// Round 140 (m3 lane 9, fast with 0x30bf0 and 0x30710): retail sums z = d.k + a.z (XMM0, stored first), x = d.i + a.x
// (XMM7), y = a.y + d.j (XMM6). The written operand order is ignored, but statement order and a new origin local move
// it; the compile-only harness reproduces the sum code exactly (96 combinations of order, per-sum spelling and the
// local): the last statement always takes XMM0 and the second is always d-first, so retail's last-and-d-first z never
// appears. Orders xzy 217, yxz 218, yzx 213, zxy 216, zyx 213; the defaulted origin in a new local `a ? a : g_468788`
// 195 (only the x sum turns d-first); that with order y, x, z 218 (retail's x/y sums and XMM7/XMM6, z still a-first).
// A two-argument inline point-plus-vector helper, ternary defaults and the default statement order change nothing.
// M3 lane 12 r149 (fast with 0x30bf0/0x30710; control 195): retail expands the existing 0x142570 three times here.
// Diagnostic (file flag, not kept): this file without /Ob1 and the helper calling 0x142570 still leaves three
// calls, 718 (475 B), with the direct inverses 724, so the file flag is not what keeps it out of line: only
// `inline` on 0x142570 (another lane's file) expands it. The helper written as 0x142570's text with its parameter-
// address read plus the direct inverses 220 (960 B); the helper's scaled coordinates bound by reference 236
// (974 B), with the direct inverses 222 (963 B); the displaced point bound by reference 268 (repeat of r110);
// rows summed position first 195 (same code); `b` declared at the top 195 (same code).
// M3 lane 15 r106 (fast with 0x142570/0x30bf0/0x30710; control 195, aligned 95, stack- and register-masked 49): the existing
// transform4x3f_apply_point (0x142570) marked `inline` and called for the three transforms (camera viewed as the
// transform4x3f it begins with) 234 (971 B, frame 0x2c); the factor copies dropped, the inverses set to 1 in the
// orthographic arm and tested and used directly, 207 (974 B, aligned 68, masked 32); both together 175 at retail's 955 B
// (frame 0x2c, defaults at +4/+8 as retail; left: the displaced sums' operand order and the transforms' x/y registers),
// but `inline` on 0x142570 loses 0x1396c7 and 0x24e59f and takes 0x2907d0 19 -> 333 in the full check: not kept. The
// same 175 comes from transform_point reading the scale through a `s_camera const *const *` to its own parameter (with
// one or three coordinate declarations, either row text order: same code), a parameter-address read, match-only;
// 0x142570's rows written forward, left, up: it still matches and 0x30830 stays 175.
// An `inline` or `__forceinline` declaration of 0x142570 in this file only leaves it a call: 724 (469 B). 0x142570 is unchanged.
// On the 175 parameter-address form: the displaced sums written d-first for x and z, a-first for y (retail's load
// sides) 175 (same code: ours loads the other side in all three); the origin transform ahead of the displaced point 236 (944 B).
// M3 lane 14 r121 (fast with 0x30bf0, 0x30710, 0x142570 and the callers 0x4f7e0/0x4ff50, which stay 325/43; control
// 195). HELD lead, build/r121/held-0x30830.patch (needs the owners' OK: the call lines in 0x4f7e0 and 0x4ff50 change):
// the parameter-address helper with the inverses used directly (re-measured 175, 955 B, first difference +0x40), plus
// the parameters as (d, a, camera, scale, ...), the order retail's calls set them up in, with every declaration and
// call, 173 (+0x4a: the three displaced sums then load retail's sides), plus the helper's coordinates declared y, x, z
// 150 at 955 bytes (+0xa0: x, y, z then sit in XMM4, XMM0, XMM1 and the displaced x, y in XMM7, XMM6 as retail).
// On 173: the displaced sums in statement order yxz/zxy/zyx 173, xzy/yzx 175. On 150: coordinates yzx 241, zyx 239,
// zxy 240, xzy 182 (985 to 988 B but for xzy); the sums yxz 156, zxy 152; the helper's row terms written in any of
// the six orders 150 (same code); a compile-only screen of all 216 coordinate, scale-statement and row-statement
// orders ranks this form first. Left on 150: each row's term order (retail right*x, then z*forward, then y*up; ours
// forward*z and up*y, then right*x) and which side of each product is loaded. Body unchanged (195).
// Y643: project origin in place 197; project displaced point in place 196; project both in place 197; direct inverses
// and origin in place 207; direct inverses and displaced point in place 207; direct inverses and both in place 207;
// both in place with point2f difference 197; direct inverses and both in place with point2f difference 207.
// NOT kept: The in-place point forms retain the extra frame word. Resolving inverse depth independently shortens the
// function to 974 bytes but raises the score to 207.
// Returned depth with planar point or separate planar scalars 195/195, depth computed
// first 195/195, direct inverse factors 207/207, reversed input reusing displacement
// 195/195. NOT kept: the depth return folds to the existing transform. Direct use of
// the inverse factors shortens the body to 974 bytes and shrinks the frame to 0x1c.
// M3 lane 12 r155 (fast with 0x30bf0/0x30710; control 195). The displaced sums' load sides follow the parameters' index
// (the held reorder above moves them), so non-reorder probes: the displaced point as a copy of `*a` plus `+=` per
// component 315 (1029 B); `d` through a `vector3f const *const &` alias in the sums 257 (981 B); `a` through such an alias
// 231 (976 B); the displaced point transformed before the origin 250 (924 B); the two inverse defaults as one point2f
// 195 (same code). 0x142570's file (src/unknown_141590.cpp) was on another lane's list this round. All restored.
// The second displaced transform still spills X and Z in a 0x30 frame. Orthographic bypass
// 223; perspective-only early invalid-factor exit 210, or inside the perspective arm 210.
// Direct inverse factors with the bypass 217, early exit 208, or in-arm test 204.
// View-owned admission result with factor copies 210 or direct inverses 204.
// NOT kept: all forms retain the extra spill word and raise the score.
// M3 lane 2 r116 (fast with 0x30bf0 and 0x30710, both MATCH; control 195): the point helper's scale test written
// `1.f != camera->scale` 263 (995 B); the factor test written `0.f != fa && 0.f != fb` 211 (978 B); the factors defaulted
// to 1 at their declaration with the else arm dropped 261 (906 B); the zero outputs as one chained assignment 195 (same
// code); the displaced and transformed points renamed point/origin/displaced (name order reversed) 195 (same code: the
// frame does not follow these names). All restored.
// M3 lane 6 r212 (fast with 0x30bf0/0x30710; control 195): the displacement copied into a value local for the six sums
// 324 (1032 B); the origin copied into a value local 336 (1039 B); both 341 (1046 B); the helper's scale tested through a
// `s_camera const *const *` to its own parameter with the inverses set to 1 in the orthographic arm and copied to fa/fb
// after the block 220 (960 B), with the displacement bound by const reference 253 (990 B). On the match-only
// parameter-address and direct-inverse lead (re-measured 175 at 955 B): the displacement by const reference 200; a
// `vector3f const *` copy of d for the sums 200 (the z sum then loads d first as retail, the first transform's x/z swap);
// a `point3f const *` copy of a 175; the sums in statement order y, x, z 175.
// M3 lane 3 r286 (single-address fast, control 195): both inverse tests as `fabs(z) >= k_real_epsilon` 225 (989 B);
// the orthographic arm first under `if (!perspective)` 215 (980 B); the nonzero arm returning `dx != 0.f || dy != 0.f`
// 195 (same code); the scale default moved into the negate block 222 (972 B); the factors as ternaries over the
// defaults 301 (890 B: the defaults fold). Reading: ours homes inv_b in the dead scale argument slot and stores the
// computed inverse there, retail keeps both defaults at frame +0/+4 and spills transform components into the scale and
// perspective slots. All restored.
// M3 lane 6 r214 (fast with 0x30bf0/0x30710, both MATCH; control 195, aligned 95, masked 38): the orthographic factors as
// `fa = fb = 1.f;`, the transformed points declared ahead of the displaced point, `fa`/`fb` declared with the inverse
// defaults, the displaced point as an aggregate initialiser, the transformed points declared `tb, ta` (retail's ta.z
// slot sits below tb.z, ours above) and as two declarations: 195 each, identical code. The 0x4f7e0/0x4ff50 call-line
// reorder stays held (both still contributor code). Body unchanged.
// Array views of transform input, output, camera basis and translation, separately and together 195 each.
// Input and output as bounded array references with camera array access 195. The X/Z spills and inverse homes persist.
// M3 lane 3 r289 (fast, single-address image; control 195; the lead below confirmed MATCH in a plain check with the rest of
// this file still matching and the callers 0x4f7e0/0x4ff50 unchanged at 325/43). Cause: the x/z register roles of the second
// and third inlined transforms, the frame and the tail follow the function's symbol count modulo 32, not the body's shape.
// With n unused initialised locals anywhere among the function's own locals the unchanged body is 195 at n = 0..15 and
// 21..31, 202 (971 B) at 16..19 and 131 at n = 20 with retail's 955 bytes and frame 0x2c (52 and 84 the same); with the
// held (d, a, camera) parameter order 193/200/125; the direct-inverse form two later: 190 (960 B) at 18..21, 138 (941 B) at
// 22, held order 132 at +0x127. Such locals inside the helper shift the three inline copies one, two and three times, so
// the copies are numbered in call order and a copy's roles flip when a 32-entry boundary falls inside it. Retail has the
// second copy flipped and the third not, which needs the reverse transform's copy numbered ahead of the forward one's.
// Diagnostic, reverted at once (gotos placing the reverse call first in the text): 90 (966 B) at 17..19, with the direct
// inverses 6 at 955 B at 19..21. The same numbering without gotos: the forward transform through one nested inline
// wrapper (the inliner numbers one call level at a time) with the direct inverses and the held order, 6 at 955 B at n = 19
// and 20 (left: the two view points' homes, ours tb below ta); without the held order 12 (the six displaced-sum rows).
// View points declared tb, ta, as two declarations, renamed three ways or moved across a boundary: 6/12 each (same
// code); as one two-element point3f array: MATCH at n = 22 and 23 with the held order, 6 at 22..24 without it; with the
// reverse point also written into `b` (no second point) MATCH at n = 28..30. Recipe: held (d, a, camera) order + direct
// inverses + the forward transform through one nested inline wrapper + the view points as a two-element array + the
// reverse point in `b` + 28..30 more symbols (or two to four fewer). No natural source of those symbols found; shifts
// measured (matching n): helper returning its output 22..24 (none); origin as a ternary local 21..23 (and it turns the
// sums' sides: 4 and 6 left); scale as a ternary local 237 at best (other code); a point2f delta 26..28 on the 28..30
// form; an inverse array 27..29 on it; the forward sums through an inline helper 26..28 while the reverse point is its
// own local, still ..30 once it is `b`; the reverse sums through a helper, alone or with the forward ones, or both
// through one scaled helper: no matching state at any n; a pair wrapper transforming origin and displaced point 27 only
// (21 with a separate reverse point); wrappers on the first and second transforms 28 only; the forward transform two
// levels deep 17 at 25..30, with the reverse one level deep MATCH at 25..27; the first one level and the forward two
// levels deep 17 at 25..28. Other states seen: the helper's parameters in the five other orders on the 175 and 150
// leads 175/150 (same code); 1..7 unused locals at the top of the 150 lead 150 each, inside its helper 150/150/150/174/
// 178/178/178; the helper's coordinates in the five other orders x 32 states with the held order: best 155 (y, x, z at
// 20) and 174 (z, x, y at 20); the displaced sums inside the forward wrapper 237..265; sum statement orders xzy/yxz/
// yzx/zxy/zyx 217/218/213/216/213 again. The displaced sums' operand sides are not text: for parameters they are fixed
// by the parameters' positions (ours a-first/a-first/d-first for z/x/y, retail and the held order d/d/a), local aliases
// move them (origin as a ternary local turns x, an origin copy y, a displacement pointer or reference z; the ternary
// origin with a displacement pointer used in the y and z sums only gives retail's three sides on the unchanged body),
// and with aliases the x side then moves with the symbol count, period 8. HELD (needs the owners' OK for the call
// lines in 0x4f7e0 and 0x4ff50, both contributor code): build/r289/held-0x30830.patch is the matching form with 28
// placeholder locals standing in for the unknown symbols. Body unchanged (195).
// M3 lane 1 r143 (fast; control 195): the point helper's coordinates declared y, x, z 230 (982 B), as three declarations
// 230, z, x, y 225 (974 B), x, z, y 227 (982 B) (the y, x, z order was measured before only on the held reorder); the
// helper returning the point by value 308 (875 B); the point passed by const reference 195 (same code).
// M3 lane 3 r313: declared (d, a, camera, ...) and returning bool with no placeholder locals, 195 -> 187 (fast and plain);
// with 0x4ff50's double local removed that caller matches (kept since: see the score line above).
// @retail 0x30830
bool function_30830(vector3f const *d, point3f const *a, s_camera const *camera, point2f const *scale, bool perspective, bool negate, point2f *out)
{
	real inv_a = 0.f;
	real inv_b = 0.f;
	if (!a)
		a = g_468788;
	if (!camera)
		camera = &g_4b9e14;
	if (!scale)
		scale = &g_4b9ecc;

	point3f b;
	b.x = a->x + d->i;
	b.y = d->j + a->y;
	b.z = a->z + d->k;

	point3f ta, tb;
	transform_point(camera, a, &ta);
	transform_point(camera, &b, &tb);

	real fa, fb;
	if (perspective)
	{
		if (fabs(tb.z) < k_real_epsilon)
		{
			point3f b2;
			b2.x = a->x - d->i;
			b2.y = a->y - d->j;
			b2.z = a->z - d->k;
			transform_point(camera, &b2, &tb);
		}
		if (!(fabs(ta.z) < k_real_epsilon))
			inv_a = 1.f / ta.z;
		fa = inv_a;
		if (!(fabs(tb.z) < k_real_epsilon))
			inv_b = 1.f / tb.z;
		fb = inv_b;
	}
	else
	{
		fa = 1.f;
		fb = 1.f;
	}

	if (fa != 0.f && fb != 0.f)
	{
		real dx = tb.x * fb - ta.x * fa;
		real dy = tb.y * fb - ta.y * fa;
		if (negate)
		{
			dx = 0.f - scale->x * dx;
			dy = 0.f - scale->y * dy;
		}
		out->x = dx;
		out->y = dy;
		if (dx != 0.f || dy != 0.f)
			return true;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
	}
	return false;
}

/* inline, as Bungie's header math was: the out-of-line copy then gives its callers no register summary, so they assume edx is clobbered across the call, as retail's 360 callers do */
__declspec(noinline) real function_30bf0(vector3f *v);
// @retail 0x30bf0
inline real function_30bf0(vector3f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = v->i * inv;
		v->j = inv * v->j;
		v->k = inv * v->k;
		return m;
	}
	return 0.f;
}

// @retail 0x30c60
void function_30c60(dword handle, point3f *position, long *out)
{
	s_obj *obj = ((s_obj_array *)g_4e0300->data)->elements[handle & 0xffff].obj;
	*position = obj->position;
	*out = obj->unknown3c;
}

// @retail 0x30ca0
void function_30ca0(bool value, long bit)
{
	if (value)
		g_4ba014 |= 1 << bit;
	else
		g_4ba014 &= ~(1 << bit);
}

// @retail 0x30cd0
long function_30cd0(bool a, bool b)
{
	if (a)
		return -1;
	if (b)
	{
				if (BIT(4) && !BIT(5))
			return 0;
		return 1;
	}
	return 2;
}

// @retail 0x30d10
long function_30d10(bool a, bool b, bool c)
{
	if (BIT(0)) return NONE;
	if (*g_4858c4 & 0x20) return 2;
	if (!BIT(2) || !b)
	{
		if (BIT(1) && a) return 2;
		return NONE;
	}
	if (c) return 2;
	if (BIT(6)) return 6;
	if (BIT(4)) return BIT(5) ? 3 : 4;
	dword bit;
	switch ((byte)(g_4ba014 >> 3) & 1)
	{
	case 0: bit = 0; break;
	default: bit = 1; break;
	}
	return (long)bit;
}

// @retail 0x30da0
long function_30da0(bool a, bool b)
{
	if (BIT(0))
		return 1;
	if (*g_4858c4 & 0x20)
		return 4;
	if (BIT(1) && a)
		return 0;
	if (BIT(2) && b)
		return 2;
	return 3;
}
// @retail 0x30e00
long function_30e00(bool a)
{
		if (BIT(0) && BIT(1) && a)
		return 0;
	return -1;
}

// @retail 0x30e30
void function_30e30(long key, long value, bool add, real x)
{
	if (key == NONE)
		return;
	long count = g_4ba134;
	long i;
	for (i = 0; i < count; i++)
	{
		if (g_4ba074[i].key == key)
		{
			if (add)
			{
				s_flag_entry *entry = &g_4ba074[i];
				entry->value = value;
				entry->x = x;
			}
			else
			{
				count--;
				g_4ba134 = count;
				if (i < count)
					g_4ba074[i] = g_4ba074[count];
			}
			return;
		}
	}
	if (add && (dword)count < 16)
	{
		s_flag_entry *entry = &g_4ba074[count];
		g_4ba134 = count + 1;
		entry->key = key;
		entry->value = value;
		entry->x = x;
	}
}

real function_30bf0(vector3f *v);

/* makes vector perpendicular to axis in the plane of both: axis x (vector x axis), normalized by
   0x30bf0, whose length result it returns */
// @retail 0x31b40
real function_31b40(vector3f const *axis, vector3f *vector)
{
	vector3f cross;
	cross3f(vector, axis, &cross);
	normalize3d(&cross);
	cross3f_staged(axis, &cross, vector);
	return function_30bf0(vector);
}

extern s_record_pool *g_4e030c;

struct s_render_entry_110
{
    long unknown00;
    long tag;
    byte unknown08[0x4c - 8];
    long object;
    long unknown50;
    short index;
    byte unknown56[0x110 - 0x56];
};

struct s_index_flags_31660
{
    long identifier;
    short count;
    short unknown06;
    word *values;
    long *groups;
};
struct s_index_state_31660
{
    byte unknown00[0xc];
    s_index_flags_31660 *first;
    void *unknown10;
    s_index_flags_31660 *second;
    s_index_flags_31660 *third;
};

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* sets or clears flags 13 and 12 of the second list's entries of the first group, then of the second
   group when it differs (every entry for NONE), and/or of every entry of the first and third lists */
static __forceinline void index_state_flags_update_31660(s_index_state_31660 *state, long first_group,
    long second_group, bool update_second, bool update_first_and_third, bool set13, bool set12)
{
    if (update_second)
    {
        for (long i = 0; i < state->second->count; ++i)
            if (first_group == NONE || first_group == state->second->groups[(short)i])
            {
                word *value = &state->second->values[(short)i];
                short flags = *value;
                SET_FLAG(flags, 13, set13);
                SET_FLAG(flags, 12, set12);
                *value = flags;
            }
        if (first_group != second_group)
        {
            for (long i = 0; i < state->second->count; ++i)
                if (second_group == NONE || second_group == state->second->groups[(short)i])
                {
                    word *value = &state->second->values[(short)i];
                    short flags = *value;
                    SET_FLAG(flags, 13, set13);
                    SET_FLAG(flags, 12, set12);
                    *value = flags;
                }
        }
    }
    if (update_first_and_third)
    {
        for (long i = 0; i < state->first->count; ++i)
        {
            word *value = &state->first->values[(short)i];
            short flags = *value;
            SET_FLAG(flags, 13, set13);
            SET_FLAG(flags, 12, set12);
            *value = flags;
        }
        for (long i = 0; i < state->third->count; ++i)
        {
            word *value = &state->third->values[(short)i];
            short flags = *value;
            SET_FLAG(flags, 13, set13);
            SET_FLAG(flags, 12, set12);
            *value = flags;
        }
    }
}

// @retail 0x31660
void function_31660(long index, long tag, s_index_state_31660 *state,
    long first_group, long second_group, bool skip, bool *output)
{
    s_render_entry_110 *entry = &((s_render_entry_110 *)g_4e030c->data)[index & 0xffff];
    dword const *definition = (dword const *)g_4e3b44[tag & 0xffff].bytes;
    if (*definition & 0x10000)
        index_state_flags_update_31660(state, NONE, NONE, false, true, false, true);
    if (*definition & 0x20000)
        index_state_flags_update_31660(state, NONE, NONE, true, false, false, true);
    if (*definition & 0x3800)
    {
        byte flag11 = (byte)~(*definition >> 11);
        byte flag13_source = (byte)~(*definition >> 13);
        byte flag12_source = (byte)~(*definition >> 12);
        byte flag12 = (flag11 | flag13_source) & 1;
        byte flag13 = (flag12_source | flag13_source) & 1;
        index_state_flags_update_31660(state, first_group, second_group, true, false, flag13 != 0, flag12 != 0);
    }
    if (state->first->count > 0 && (*definition & 4) && state->first->count > 1)
    {
        *(real *)((byte *)entry + 0xf8) = 1.0f;
        // Retail's final store is reached only with a null output argument.
        if (!output && !skip)
        {
            for (long i = 0; i < state->first->count; ++i)
            {
                word *value = &state->first->values[(short)i];
                *value |= 0x3000;
            }
            *output = true;
        }
    }
}


s_flags_storage g_flags_storage;
