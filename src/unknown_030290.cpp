#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"
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
byte *g_4858c4;
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

// score 195 (was 195): Frame 0x30 versus 0x2c. Ours gives the dead scale slot to the second inverse
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
// @retail 0x30830
long function_30830(point3f const *a, s_camera const *camera, vector3f const *d, point2f const *scale, bool perspective, bool negate, point2f *out)
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

s_projection_storage g_projection_storage;

s_flags_storage g_flags_storage;
