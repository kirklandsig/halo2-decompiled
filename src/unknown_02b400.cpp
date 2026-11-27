// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "timed_effect.h"
#include "visibility_storage.h"
/* UNKNOWN_02B400.CPP: vector math and rectangular grid placement */

#include "unknown_11c920.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "globals.h"
#include "vector_products.h"

struct s_masked_list;
typedef void (__stdcall *masked_list_proc)(long value, short mask);
void masked_list_iterate(s_masked_list const *list, long mask, masked_list_proc proc);
void __stdcall function_c3340(long light_index, long unused);
void function_3eec0();

// @retail 0x2c340
void function_2c340(bool enabled)
{
	if (enabled)
	{
		masked_list_iterate(g_547f98, 0xffff, (masked_list_proc)function_c3340);
		function_3eec0();
	}
}

struct s_2f800_source
{
	point3f position;
	byte unknown0c[0x14];
	vector3f forward;
	vector3f up;
	byte unknown38[0x14];
	real angle;
	real scale;
};

struct s_2f800_view
{
	point3f position;
	vector3f forward;
	vector3f up;
	bool flag24;
	byte unknown25[3];
	real angle;
	real scale;
	byte unknown30[0x10];
	real near_distance;
	real far_distance;
	byte unknown48[0x10];
	bool flag58;
};

struct s_2f800_size
{
	short width, height;
};

real g_5234dc;

struct s_2f970_view
{
	byte unknown00[0x30];
	short top, left, bottom, right;
	short outer_top, outer_left, outer_bottom, outer_right;
	byte unknown40[0x18];
	bool crop;
	byte unknown59[3];
	real center_x, center_y, crop_scale;
	bool offset;
	byte unknown69[3];
	real offset_x, offset_y;
};

bool g_4ba019;

struct s_short_bounds_3f660
{
	short top, left, bottom, right;
};

// The volatile crop pointer, the two volatile out[3] reads and the reference that delays the offset arm's out[1]
// store stand for retail's reloads after the g_4ba019 store: in retail the flag lives in a larger address-taken
// aggregate (0x13c20 copies 0x120 bytes to 0x4b9ef0, ending at 0x4ba010), so its store may alias view and out.
// Without each of them: crop pointer 77, crop out[3] 30, offset out[3] 51, reference 19, all four 88 (492 B).
// Earlier forms (score 33 before the match): 0.5f in XMM4 (retail XMM5) and crop arm schedule; caller 0x13c20 passes
// global &g_4b9da0 as view. Tried (22 trials, 14 matched peers MATCH): splitting `new_width = view->crop_scale * width;` and
// `new_height = view->crop_scale * height;` with `out[0..3] = x/y +/- new_w/h * 0.5f` fixes 0.5f in XMM5 and all crop
// arithmetic/stores (6, first +0x106); ordering x before height and dropping crop_scale's volatile and right_result hits
// 0 (MATCH, not kept: still uses main's volatile crop pointer through view); same clean body with zero volatile and a
// diagnostic escaped g_4ba019 hits 0 (MATCH, not kept); clean body without escape 86-88 (492-494 B), only out[3]
// volatile 77 (503 B, +0xbd); early return in crop 40; x-then-y crop 55; inline width/height 74; box2f view 55;
// vertical_scale first 129; inline crop helper 61; local box2f copy 166; scenario helper 123 (loses 0x3f660).
// M3 lane 15 r149 (MATCH): the crop sizes named once (new_width/new_height, each read twice) with the crop_scale volatiles
// dropped 21; the half step parenthesised, `((center + 1.0f) * 0.5f) * size`, 9 (the chain no longer reassociates
// the size ahead of 0.5f); the four outputs in index order 0 (every other order of the four 10-25).
// @retail 0x2f970
bool function_2f970(s_2f970_view const *view, real *out)
{
    real center_x = (view->outer_right + view->outer_left) * 0.5f;
    real center_y = (view->outer_top + view->outer_bottom) * 0.5f;
    real vertical_scale = 2.0f / (view->outer_bottom - view->outer_top);
    real horizontal_scale = (real)(view->bottom - view->top) / (view->right - view->left) * vertical_scale;
    out[0] = (view->left - center_x) * horizontal_scale;
    out[1] = (view->right - center_x) * horizontal_scale;
    out[2] = (center_y - view->bottom) * vertical_scale;
    out[3] = (center_y - view->top) * vertical_scale;
    g_4ba019 = view->crop || view->offset;
    if (g_4e0350)
        g_4ba019 |= *(short *)((byte *)g_4e0350 + 0x10) == 2;
    bool const volatile *crop = &view->crop;
    if (*crop)
    {
        real width = out[1] - out[0];
        real height = *(real const volatile *)&out[3] - out[2];
        real new_width = view->crop_scale * width;
        real new_height = view->crop_scale * height;
        real x = ((view->center_x + 1.0f) * 0.5f) * width + out[0];
        real y = ((view->center_y + 1.0f) * 0.5f) * height + out[2];
        out[0] = x - new_width * 0.5f;
        out[1] = new_width * 0.5f + x;
        out[2] = y - new_height * 0.5f;
        out[3] = new_height * 0.5f + y;
    }
    else if (view->offset)
    {
        real x = (real)((out[1] - out[0]) * (1.0f / 640.0f)) * view->offset_x;
        real y = (real)((*(real const volatile *)&out[3] - out[2]) * (1.0f / 480.0f)) * view->offset_y;
        out[0] += x;
        real right = out[1] + x;
        real const &right_result = right;
        out[2] -= y;
        out[3] -= y;
        out[1] = right_result;
    }
    return true;
}

// @retail 0x2f800
void function_2f800(s_2f800_view *view, s_2f800_source const *source, s_2f800_size const *first, s_2f800_size const *second)
{
	(void)&second;
	if (source)
	{
		view->position = source->position;
		view->forward = source->forward;
		view->up = source->up;
		view->angle = source->angle;
		view->scale = source->scale;
		real factor = 0.785f;
		if (first && second)
		{
			long ratio = first->width * second->height * 100 / (first->height * second->width);
			if (ratio == 200)
				factor = 0.5f;
			else if (ratio == 50)
				factor = 0.8f;
		}
		view->angle *= factor;
		view->scale *= factor;
	}
	else
	{
		view->position = *g_468788;
		view->forward = *g_4687a8;
		view->up = *g_4687b0;
		view->scale = 1.0f;
		view->angle = (real)(2.0 * atan2((double)(tan(g_5234dc * 0.5f) * 0.75f), 1.0));
	}
	view->flag24 = false;
	view->near_distance = g_485ad4.lo;
	view->far_distance = g_485ad4.hi;
	view->flag58 = false;
}

extern byte g_485ac2;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_projection_scalars
{
	box2f clip;
	real width, height;
	real half_width, half_height;
	real offset_x, offset_y;
	real scale_x, scale_y;
	real left, right, bottom, top;
};

byte function_016a90();

// @retail 0x2fb70
void function_2fb70(s_2f800_view const *view, box2f const *clip, s_projection_scalars *out)
{
    short const *viewport = (short const *)view->unknown30;
    out->width = (real)(viewport[3] - viewport[1]);
    out->height = (real)(viewport[2] - viewport[0]);
    if (!clip)
    {
        out->clip.x0 = out->clip.y0 = -1.0f;
        out->clip.x1 = out->clip.y1 = 1.0f;
    }
    else
        out->clip = *clip;
    out->half_width = (out->clip.x1 - out->clip.x0) * 0.5f;
    out->half_height = (out->clip.y1 - out->clip.y0) * 0.5f;
    out->offset_x = (out->clip.x1 + out->clip.x0) / out->half_width * -0.5f;
    out->offset_y = (out->clip.y0 + out->clip.y1) / out->half_height * -0.5f;
    real tangent = (real)tan((double)(view->angle * 0.5f));
    real aspect = out->width / out->height;
    if (function_016a90())
    {
        aspect *= 4.0f / 3.0f;
        if (g_510c50 && ((byte *)g_510c50)[5])
            tangent = 0.5625f * (tangent * (4.0f / 3.0f));
    }
    else if (g_510c50 && ((byte *)g_510c50)[5])
    {
        real scale = out->height * (1.0f / 480.0f);
        out->clip.y0 = out->clip.y0 * scale;
        out->clip.y1 = scale * out->clip.y1;
    }
    out->scale_x = 1.0f / (out->half_width * aspect * tangent);
    out->scale_y = 1.0f / (out->half_height * tangent);
    out->left = 0.0f - (out->offset_x + 1.0f) * (1.0f / out->scale_x);
    out->right = 0.0f - (out->offset_x - 1.0f) * (1.0f / out->scale_x);
    out->bottom = 0.0f - (out->offset_y + 1.0f) * (1.0f / out->scale_y);
    out->top = 0.0f - (out->offset_y - 1.0f) * (1.0f / out->scale_y);
}

#define k_real_epsilon 0.0001f

// @retail 0x48e70
long function_48e70(real value)
{
	long bits = *(long *)&value;
	long exponent = (bits >> 23) & 255;
	long shift = 150 - exponent;
	long mantissa = ((bits & 0x7fffff) | 0x800000) >> shift;
	long sign = bits >> 31;
	long result = ((mantissa ^ sign) - sign) & ~((exponent - 127) >> 31);
	long increment;
	if (!sign && bits && (((exponent - 127) >> 31) || (((1 << shift) - 1) & bits & 0x7fffff)))
		increment = 1;
	else
		increment = 0;
	return result + increment;
}

struct s_sphere_plane_volume
{
	point3f center;
	real radius;
	long count;
	plane3f *planes;
};

// @retail 0x38390
bool function_38390(point3f const *point, s_sphere_plane_volume const *volume)
{
	vector3f delta;
	delta.i = point->x - volume->center.x;
	delta.j = point->y - volume->center.y;
	delta.k = point->z - volume->center.z;
	real distance = delta.i * delta.i;
	distance += delta.k * delta.k;
	distance += delta.j * delta.j;
	bool result = false;
	if (distance < volume->radius * volume->radius)
	{
		bool outside = false;
		for (long i = 0; i < volume->count; ++i)
		{
			plane3f const *plane = &volume->planes[i];
			real distance_to_plane = point->x * plane->i + plane->j * point->y + plane->k * point->z - plane->d;
			if (distance_to_plane > k_real_epsilon)
			{
				outside = true;
				break;
			}
		}
		result = !outside;
	}
	return result;
}

// @retail 0x1c1a0
double __cdecl function_1c1a0(real value)
{
	return fabs(value);
}

// @retail 0x1d6d0
double __stdcall function_1d6d0(real value)
{
	return ceil(value);
}

// @retail 0x2b460
double __cdecl function_2b460(real value)
{
	return floor(value);
}

__declspec(noinline) double __stdcall function_2b480(real value);

// @retail 0x2b480
double __stdcall function_2b480(real value)
{
	return floor(value);
}

/* Preserve the retail call boundary used by 0x2c2a70. */
__declspec(noinline) inline real normalize2d(point2f *v);

// @retail 0x2b400
inline real normalize2d(point2f *v)
{
	real m = (real)sqrt(v->x * v->x + v->y * v->y);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->x = v->x * inv;
		v->y = inv * v->y;
		return m;
	}
	return 0.f;
}

// @retail 0x4efb0
inline vector3f *function_4efb0(vector3f *v)
{
	real length_squared = v->i * v->i + v->j * v->j + v->k * v->k;
	if (length_squared != 0.0f)
	{
		real inverse = (real)(1.0f / sqrt(length_squared));
		v->i = (real)(v->i * inverse);
		v->j = (real)(v->j * inverse);
		v->k = (real)(v->k * inverse);
	}
	return v;
}

// @retail 0x461c0
long function_461c0(point3f const *a, point3f const *b, real radius)
{
	/* Retail receives the radius on the stack. */
	real const *radius_reference = &radius;
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	real distance_squared = delta.i * delta.i;
	distance_squared += delta.k * delta.k;
	distance_squared += delta.j * delta.j;
	if (distance_squared <= *radius_reference * *radius_reference)
		return 1;
	return 0;
}

struct s_grid_pair
{
	short x;
	short y;
};

// @retail 0x2f600
void function_2f600(long count, long mode, s_grid_pair *out)
{
	long x = 1;
	long y = 1;
	bool narrow = mode == 1;
	while (x * y < count)
	{
		bool grow;
		if (narrow)
			grow = x < y;
		else
			grow = x <= y;
		if (grow)
			++x;
		else
		{
			x = 1;
			++y;
		}
	}
	out->x = (short)x;
	out->y = (short)y;
}

// @retail 0x2f640
void function_2f640(long index, s_grid_pair const *dimensions, s_grid_pair *extent,
	long count, long mode, s_grid_pair *out)
{
	extent->x = extent->y = 1;
	long width = dimensions->x;
	long area = width * dimensions->y;
	long offset = 0;
	if (count < area)
	{
		if (mode == 1)
		{
			if (!index)
				extent->x = 2;
			else
				offset = 1;
		}
		else if (!index)
			extent->y = 2;
		else
			offset = index / width;
	}
	long position = index + offset;
	out->x = (short)(position % dimensions->x);
	out->y = (short)(position / dimensions->x);
}


#include <string.h>
void function_141590(transform4x3f const *in, transform4x3f *out);

PRIVATE __forceinline void normalize_projection_axis(vector3f *axis)
{
    real length = (real)sqrt(axis->k * axis->k + axis->j * axis->j + axis->i * axis->i);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        axis->i *= inverse;
        axis->j *= inverse;
        axis->k *= inverse;
    }
}

struct s_2fd90_projection
{
    transform4x3f inverse;
    transform4x3f camera;
    real bounds[4];
    real matrix[16];
    real half_width, half_height;
};

// Score 221 in a full build with stand-in SDK headers (was 256): up is the shared cross3f of right and the forward
// vector (retail loads each field and multiplies by a register), and the half sizes multiply size by scale.
// Roasted-Codes (draft #1563, released 2026-11-27; body left as above, 221 at 1278 of 1278 bytes): the half sizes
// computed before the bounds copy, the fabs sum written i then j and plane.d accumulated on the member in three
// statements 191 at 1278 B (first difference +17; whole-image checks lose no matched function). Adding the existing
// cross3f_staged for right and vector3f_negate_ab for backward on that base 107 at 1278 B (first difference +738), but
// that combination loses the matched 0x1c4590 (0 -> 5) and 0x24cf66 (0 -> 2) on two of three bases, so it is not a
// stable base. A fixed two-component plane-scaling loop (reduced image) 102 at 1279 B, 105 on the member-accumulator
// base: it puts i before j and worsens the prefix and output stores, where retail wants j first with the reciprocal
// as the destination. A whole-image permuter run from the 191 form (141 tries) found nothing lower.
// @retail 0x2fd90
void function_2fd90(s_2f800_view const *view, box2f const *clip, byte *out)
{
    s_2fd90_projection *projection = (s_2fd90_projection *)out;
    s_projection_scalars scalars;
    function_2fb70(view, clip, &scalars);
    vector3f right, up, backward;
    vector3f const *f = &view->forward;
    right.i = view->up.k * f->j - view->up.j * f->k;
    right.j = f->k * view->up.i - view->up.k * view->forward.i;
    right.k = view->up.j * view->forward.i - f->j * view->up.i;
    cross3f(&right, &view->forward, &up);
    backward.i = 0.0f - view->forward.i;
    backward.j = 0.0f - view->forward.j;
    backward.k = 0.0f - view->forward.k;
    normalize_projection_axis(&right);
    normalize_projection_axis(&up);
    normalize_projection_axis(&backward);
    projection->camera.forward = right;
    projection->camera.left = up;
    projection->camera.up = backward;
    projection->camera.position = view->position;
    projection->camera.scale = 1.0f;
    transform4x3f *inverse = &projection->inverse;
    function_141590(&projection->camera, inverse);
    memcpy(projection->bounds, &scalars.left, 16);
    projection->half_width = scalars.width * scalars.scale_x * 0.5f;
    projection->half_height = scalars.height * scalars.scale_y * 0.5f;
    plane3f plane;
    if (view->near_distance == 0.0f)
    {
        plane3f const *cutting_plane = (plane3f const *)view->unknown48;
        real i = cutting_plane->i;
        real j = cutting_plane->j;
        real k = cutting_plane->k;
        plane.i = inverse->up.i * k + inverse->left.i * j + inverse->forward.i * i;
        plane.j = inverse->up.j * k + inverse->left.j * j + inverse->forward.j * i;
        plane.k = inverse->up.k * k + inverse->left.k * j + inverse->forward.k * i;
        plane.d = cutting_plane->d * inverse->scale + inverse->position.z * plane.k + inverse->position.y * plane.j + inverse->position.x * plane.i;
    }
    else
    {
        plane.i = plane.j = 0.0f;
        plane.k = 1.0f;
        plane.d = 0.0f - view->near_distance;
    }
    real far_distance = view->far_distance;
    real reciprocal = 1.0f / plane.k;
    real near_value = 0.0f - reciprocal * plane.d;
    real scale = (real)(far_distance / ((far_distance - near_value) *
        (fabs(reciprocal * plane.j) + fabs(reciprocal * plane.i) + 1.0f)));
    plane.i = reciprocal * scale * plane.i;
    plane.j = reciprocal * scale * plane.j;
    plane.k = scale;
    plane.d = 0.0f - scale * near_value;
    if (plane.d > 0.0f && view->near_distance == 0.0f)
    {
        plane.i = 0.0f - plane.i;
        plane.j = 0.0f - plane.j;
        plane.k = 0.0f - plane.k;
        plane.d = 0.0f - plane.d;
    }
    memset(projection->matrix, 0, 64);
    projection->matrix[0] = scalars.scale_x;
    projection->matrix[5] = scalars.scale_y;
    projection->matrix[8] = 0.0f - scalars.offset_x;
    projection->matrix[9] = 0.0f - scalars.offset_y;
    projection->matrix[2] = 0.0f - plane.i;
    projection->matrix[6] = 0.0f - plane.j;
    projection->matrix[10] = 0.0f - plane.k;
    projection->matrix[11] = -1.0f;
    projection->matrix[14] = plane.d;
}

struct s_view_setup;
struct s_view_collection_1733e0;
struct s_predicted_resource_block;
struct s_planar_camera_source;
struct s_planar_camera
{
    bool disabled;
    byte unknown01[3];
    transform4x3f inverse;
    transform4x3f matrix;
    bool has_plane;
    byte unknown6d[3];
    real offset;
    plane3f plane;
    byte unknown84[4];
    real depth;
    bool valid;
    byte unknown8d[3];
    byte frustum[0x108];
    long count;
    point2f corners[4];
};
void function_441b0(s_planar_camera_source const *source, s_planar_camera *state, byte const *view);
void function_1726d0(s_view_setup *view, point3f const *position, vector3f const *forward, vector3f const *up);
void __stdcall function_173130(long camera_count, byte *cameras, long cluster_index, s_view_collection_1733e0 *collection);
bool function_16e5e0(s_predicted_resource_block const *block, short mode);
void function_146de0(void);
void function_146b80(void);
extern byte *g_510c44;
extern bool g_510c48;

// Matched in a full build with stand-in SDK headers (was 97), without the five volatile reads: the rectangle is
// copied as a struct and the cluster index is a named local behind a barrier reference.
// @retail 0x3f660
void function_3f660(transform4x3f const *matrix)
{
    bool restore = g_47989c != NULL;
    if (restore)
        function_146de0();
    byte *collection = g_510c44;
    g_510c48 = true;
    byte setup[0x54];
    function_1726d0((s_view_setup *)setup, &matrix->position,
        &matrix->forward, &matrix->up);
    s_2f970_view view;
    function_2f800((s_2f800_view *)&view, (s_2f800_source const *)setup, NULL, NULL);
    view.top = view.left = 0;
    view.right = 640;
    view.bottom = 480;
    *(s_short_bounds_3f660 *)&view.outer_top = *(s_short_bounds_3f660 const *)&view.top;
    view.outer_left = (short)(view.outer_left + 48.0f);
    view.outer_top = (short)(view.outer_top + 48.0f);
    view.outer_right = (short)(view.outer_right - 48.0f);
    view.outer_bottom = (short)(view.outer_bottom - 48.0f);
    view.crop = false;
    view.center_x = view.center_y = 0.0f;
    view.offset = false;
    view.offset_x = view.offset_y = 0.0f;
    box2f clip;
    function_2f970(&view, (real *)&clip);
    byte projection[0xc0];
    function_2fd90((s_2f800_view *)&view, &clip, projection);
    s_planar_camera camera;
    long cluster_index = *(short *)(setup + 0x10);
    long const &kept = cluster_index;
    function_441b0((s_planar_camera_source const *)&view, &camera, projection);
    function_173130(1, (byte *)&camera, kept, (s_view_collection_1733e0 *)collection);
    byte *structure = (byte *)g_4e0348;
    for (long i = 0; i < *(short *)(collection + 0xa6c); ++i)
    {
        short cluster = *(short *)(collection + 0xa6e + i * 0x1a);
        byte *entry = *(byte **)(structure + 0xa0) + cluster * 0xb0;
        function_16e5e0((s_predicted_resource_block *)(entry + 0x84), 0);
    }
    g_510c48 = false;
    if (restore)
        function_146b80();
}

struct s_28580_view
{
    bool initialized;
    byte field_01[3];
    transform4x3f previous;
    long ring;
    real angle;
    real phase;
    real previous_time;
    point2f points[4];
    point2f direction;
    point2f target_direction;
    real speed;
    real target_speed;
    real changed_time;
    real duration;
};

struct s_28580_definition
{
    byte field_00[0x2c];
    real speed_low, speed_high;
    real duration_low, duration_high;
    real blend;
    real turn;
};

static __forceinline dword random_next_28580(dword *seed)
{
    *seed = *seed * 1664525 + 1013904223;
    return *seed >> 16;
}

static __forceinline real random_real_28580(void)
{
    return (real)random_next_28580(&g_4e7408->seed) * (1.0f / 65535.0f);
}


static __forceinline real random_real_range_28580(real lower, real upper)
{
    real random = random_real_28580();
    return lower + (upper - lower) * random;
}

long g_4b9a00;
s_28580_view g_4b9a04[5];

// score 151 (was 296) in a full build with stand-in SDK headers: rewritten from retail: the state is addressed as
// g_4b9a04[view_index].field (no state pointer), the random fractions are float with the exponent 1.0f - turn, the
// seed steps through an inline helper taking &g_4e7408->seed (retail reloads the seed after the pow call), the sign
// test is (seed & 0x80000000) > 0, the blend works through a point2f pointer with x/y read before keep = 1.0f -
// blend, and the first normalize is written in place while the second stays a call to 0x2b400 as in retail. Left:
// the x row's load order, the == 0 compare after the inlined normalize, the speed lerp's folded load, the order of
// the target rows and of the out rows' products.
// Retail compares the first normalize's result at the join (the zero arm copies 0.0f into the result register and
// falls through to the test), the shape of 0x2b400 expanded at link time; written in place, ours threads the zero arm
// straight to the (1, 0) stores. 0x2b400 keeps its existing noinline (about fifty retail sites call it), so that part is
// BLOCKED-BY 0x2b400. Tried: the x blend row reading the element directly 151 (same code), the target pointer taken
// in the turn block with the blend rows on the element 202; the out rows as (x * speed) * delta 151, through a named
// speed * delta 155; the cross terms written in place 148 (846 bytes); the side terms named first (side * (0 - y),
// x * side) 148, with keep = 1.0f - turn named 147 (855 bytes, retail's length), keep named first 147, the side terms
// over the cross locals 147, the sums written side term first 147.
// @retail 0x28580
void function_28580(long tag, real time, vector3f *out)
{
    s_28580_definition const *definition =
        (s_28580_definition const *)g_4e3b44[tag & 0xffff].data;
    long view_index = g_4b9a00;
    point2f *direction = &g_4b9a04[view_index].direction;
    real delta = time - g_4b9a04[view_index].previous_time;
    g_4b9a04[view_index].previous_time = time;
    if (definition->speed_high > 0.0f)
    {
        point2f *target = &g_4b9a04[view_index].target_direction;
        real x = direction->x;
        real y = direction->y;
        real keep = 1.0f - definition->blend;
        x *= keep;
        y *= keep;
        direction->x = x;
        direction->y = y;
        real blend = definition->blend;
        direction->x = target->x * blend + x;
        direction->y = target->y * blend + y;
        real magnitude = (real)sqrt(direction->x * direction->x +
            direction->y * direction->y);
        if (!(fabs(magnitude) < k_real_epsilon))
        {
            real inverse = 1.0f / magnitude;
            direction->x *= inverse;
            direction->y *= inverse;
        }
        else magnitude = 0.0f;
        if (magnitude == 0.0f)
        {
            direction->x = 1.0f;
            direction->y = 0.0f;
        }
        real speed_blend = definition->blend;
        g_4b9a04[view_index].speed = (1.0f - speed_blend) * g_4b9a04[view_index].speed + speed_blend * g_4b9a04[view_index].target_speed;
        if (time - g_4b9a04[view_index].changed_time >= g_4b9a04[view_index].duration)
        {
            real turn = (real)pow(random_real_28580(), 1.0f - definition->turn);
            g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
            real sign = (g_4e7408->seed & 0x80000000) > 0 ? -1.0f : 1.0f;
            real side = sign * turn;
            real cross_x = 0.0f - direction->y;
            real cross_y = direction->x;
            target->x = direction->x * (1.0f - turn) + side * cross_x;
            target->y = direction->y * (1.0f - turn) + side * cross_y;
            if (normalize2d(target) == 0.0f)
            {
                target->x = 1.0f;
                target->y = 0.0f;
            }
            g_4b9a04[view_index].changed_time = time;
            g_4b9a04[view_index].target_speed = random_real_range_28580(definition->speed_low, definition->speed_high);
            g_4b9a04[view_index].duration = random_real_range_28580(definition->duration_low, definition->duration_high);
        }
    }
    out->i = direction->x * g_4b9a04[view_index].speed * delta;
    out->j = direction->y * g_4b9a04[view_index].speed * delta;
    out->k = 0.0f;
}

#include <string.h>
extern transform4x3f *g_4687d0;

void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

struct s_27aa0_record
{
    long bitmap;
    real fraction;
    real weights[4];
    real coordinates[8];
};
#include "filter_kernel_storage.h"
#include "matrix_transform.h"
s_filter_kernel_storage g_filter_kernel_storage;
byte g_55e6c4;

PRIVATE __forceinline double filter_random_fraction(void)
{
    g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
    return (double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
}

inline void point2f_set(point2f *point, real x, real y);

PRIVATE __forceinline void filter_random_pair(point2f *point)
{
    dword seed = g_4e7408->seed * 1664525 + 1013904223;
    *(dword volatile *)&g_4e7408->seed = seed;
    seed = seed * 1664525 + 1013904223;
    g_4e7408->seed = seed;
    /* y is the high half of the stored seed, read back as a word */
    point2f_set(point, (real)(seed >> 16) * (1.0f / 65535.0f),
        (real)*((word const *)&g_4e7408->seed + 1) * (1.0f / 65535.0f));
}

/* standard (frame alignment): retail's prologue realigns this frame itself
   (and esp, -8), which our build does for such a function only when its
   address escapes. It takes no arguments, so the marker changes no call.
   The body doesn't match yet, but with the marker its prologue is
   retail's; its caller 0x25a10 doesn't match yet either. */
// score 529 (was 530): the aligned 0xb8 frame agrees. The exit displacement leads to a body 80 bytes
// short (the first cosine and the cube now take retail's float constants). Float angle literal 593;
// float angle literal with promoted camera component 597; named double cosine 596; named double
// turn increment 597; named distance span 596; plain phase pointer 597; direct phase field 597; double
// bitmap phase terms 596. NOT kept: the float angle literal fixes the three operand widths for four points.
// The random-pair tails are still shared, and the cosine and distance fraction have different homes.
// Float cosine with a shared blend complement 598, bitmap phase rounded after the index 597,
// named turn components 597, named distance fraction 597, local ring normalization 592,
// shared records base 631, shared projected extents 597, shared quadratic complement 597, typed
// filter state 597. NOT kept: local ring normalization gains five and writes the wrapped index once.
// Independent crossing tests 566, positive-first refresh 604, indexed initialization 958,
// complete random point then indexed assignment 537, selected refresh destination 642,
// signed-remainder wrap 638, local wrap passes 624, named integer advance 597.
// NOT kept: independent tests repeat the sign check on the negative path; complete
// points add temporary stack stores absent from retail.
// M3 lane 5 r247 (fast screen; aligned rows against retail in brackets, main 269 at 2683 B): the weights arm's fraction
// clamp after the three weights 654 (331); the other arm's fraction clamp after the weight clamp 600 (297); both 671
// (342); the bitmap fraction part named once for its clamp 679 (310); the heading's two components in named floats,
// x first, 597 (269, same code); re-measured: the float angle literal 593 (262). Leads from the aligned listing: retail
// computes the far-near span three times (the quarter step, the floor test and its else value) where ours shares one;
// it clamps the fraction inside each weights arm (stored to its home in each branch of the clamp) where ours hoists
// the identical clamp above the test; it evaluates the heading's x argument first with left + up; and its two random
// point arms keep separate tails (the second forms the point address late) where ours tail-merges them.
// M3 lane 13 r162 (h2m fast, plain check 587 with the file's other functions and the caller 0x25a10 unchanged): kept
// 597 -> 587: the blend's cosine as cosf of a float product with float constants (retail's dword pi, 1.0 and 0.5
// operands; 593 alone, as the float angle literal) and the cube a real local (retail's `fst dword` and then `fsubr
// dword` 1.0f ahead of the square's pow). Also: far and near read from their globals at each use 593 (same code); the
// turned angle a real with cosf and sinf 593 (same code); the turned angle as `*angle -= -(...)` with cos and sin of
// *angle 588; the heading delta a real 619 (2671 B: retail's fsubr from the angle field, but later sums move to SSE).
// Then kept 587 -> 558: the heading through the existing matrix_transform_vector applied to a constant (1, 0, 0) axis
// (retail's x row first with left + up, as the folded transform gives), and 558 -> 530: filter_random_pair storing
// through the existing point2f_set (its arguments evaluated y first, so both stores follow x's unsigned fix-up as in
// retail). Also: the loop's point transform through the existing transform4x3f_apply_point (0x142570) 663 (2610 B,
// frame 0xb0: inlined, but two stack cells go); `*angle -= -(...)` on 558 558 (same code).
// Left: the angle field is loaded ahead of the heading (retail subtracts from memory), the fraction clamp is hoisted
// above the weights test, the crossing pairs tail-merge (the point address is formed early), the tail copies the
// point through integer registers (retail loads it as a real ahead of the stores), and two 4-byte slots (the loop
// index and the radius) are swapped.
// M3 lane GA r24 (47 fast trials across 4 batches with 0x25a10, 0x22bbca, 0x17d9a0 all MATCH and 0x28580 unchanged at 151;
// baseline 530 at 2702/2782 B, al=172, mk=92 -> kept 459 at 2763/2782 B, 654/656 insns, al=123, mk=52):
// Kept (-71 pts, +61 B toward retail): 4-caller inline vector4f_set for both weights arms and both coordinates rows
// (fixes right-to-left argument evaluation, keeps the fraction clamp inside each weights arm, loads point coordinates
// as reals ahead of the stores, and puts i/radius/cosine/fraction in retail's exact stack slots; 492 alone, weights
// only 497, coords only 522) plus typing point2f points[4] at s_28580_view+0x48 (drops the points local and the spurious
// lea ecx in the tail, making the final 135 instructions 99.3% exact) and filter_random_pair(state, index) (eliminates
// crossing tail-merging without a macro or extra local). Also tried (not kept): `*angle -= (real)-(atan2(heading.j,
// heading.i) * *(real *)(definition + 4));` with cos(*angle)/sin(*angle) 491 (al=117, mk=46: makes the entire angle/cos/sin
// block byte-exact via fsubr [ebp+0x3c], but shifts positional rows by -3 because far-near is still CSE'd; ready to combine
// once far-near is solved); FILTER_RANDOM_PAIR macro + wrap local 454 (al=119, mk=48; 489 without wrap, 530 on baseline);
// max_real helper 559; span if/else 530; MAX lt/gt macros 491; step / 4.0f 491; direct g_4857e0/g_4857dc 457 (491 with angle
// subeq); if (far > near) block 459; inverse_span before step 460; bitmap real value 547/568, inline cast 505/491, assign
// expr 491 (mk=41); relative point3f local 491 (al=105, mk=41), transform helper 531; one_minus_t local 492; two seed locals
// 491; angle fsubr via double local 545 / assign+cos 508. Left: far-near computed 3x (ours CSEs to 1x), angle fsubr (lead
// above), bitmap fst dword before fst qword, crossing branch-2 mov ecx, edx and late lea, relative transform xmm reg order,
// and one mulss xmm0, xmm0 vs xmm0, xmm1 in weights[0].
// review (machine 3, 2026-11-26): the GA r24 form above (459) is held, not applied: its gain rests on a new
// vector4f_set used only by this function (a new one-caller helper, chosen for its argument evaluation order) and on
// filter_random_pair taking (state, index) in place of the point (a helper parameter shape picked by score); both are
// kept only with a byte match. main's form (530) stays.
// M3 lane 11 r175 (h2m fast with 0x25a10, 0x22bbca, 0x17d9a0, 0x28580). Lead, not kept: filter_random_pair drawing its
// two values through the header's existing random real (x82e52f) in place of the written-out seed step with its
// volatile store: 530 at 2702 B, the opening block's four pairs still exact (the y value still comes from the stored
// seed's high word), only the crossing arms' registers move; but the whole-image link then loses matched 0x1c4590
// and 0x24cf66 (both match again with the old body). Tried: the span's else value re-reading the two globals (as
// matched 0x27520 does) 527 (2717 B), with the x82e52f pair 526 (2718 B; a local/global mix, kept only with a match);
// the pairs as direct point2f_set calls with the pointer evaluated after the draws, at all six sites 546 (2686 B), at
// the two crossing arms only 546.
// M3 lane 7 r237 (h2m fast with 0x25a10, 0x22bbca, 0x17d9a0 and 0x28580). Kept 530 -> 529: the view record's members
// typed (the initialized flag, the previous transform, the ring index, the angle, the phase and `point2f points[4]` at
// +0x48, 0x28580 unchanged), the points read through the record in place of a pointer local. Tried on it: the records
// pointer local in the second loop in place of the global array at each use 566 (2666 B); the quarter step on the two
// globals 564 (2701 B), with the span's else value on the globals too 545 (2717 B: the else value then shares the
// step's subtraction, the test keeps its own, two where retail has three); the previous transform read through the
// record at each use (no pointer local) 677.
// M3 lane 4 r270 (fast with 0x25a10, 0x28580, 0x17d9a0 and 0x22bbca: without 0x17d9a0's file the pair setter stays
// out of line and the fast count is 614 at 2782 B). Retail subtracts far - near three times (the quarter step, the
// clamp's test and its else value, recomputed even though the test's difference is still in XMM0), so none of it is
// shared; ours computes it once. Forms: the quarter step ahead of the range test 524, the step and the inverse span
// both ahead of it 525 (statement order only, under 8: not kept); the step as `/ 4.0f` 529 (same code); the clamped
// span named before the reciprocal 529; `0.25f * (far - near)` 529; the inverse span ahead of the step 530; a
// float-consistency pragma as a diagnostic 782 (2898 B; still one subtraction). Re-measured on 529: the angle updated
// in place, `*angle -= (real)-(atan2(...) * scale)` with cos and sin of `*angle`, 559, with the named delta kept 559.
// Lead from the listing: retail's first crossing arm stores through `[ebp + index*8 + 0x48/0x4c]` with the wrapped index
// in EAX (no pointer formed) while the second forms `lea eax, [ebp + ring*8 + 0x48]` after the draws, so the two arms
// differ before any tail could merge. Body unchanged.
// @retail 0x27aa0 standard
void function_27aa0(void)
{
    if (g_4857b8 == NONE || !g_4857b8) return;
    real time = (real)g_4858a0;
    s_28580_view *state = g_4b9a04 + g_4b9a00;
    s_27aa0_record *records = (s_27aa0_record *)g_4b9c74 + 1;
    byte *definition = g_4e3b44[g_4857b8 & 0xffff].bytes;
    byte *bitmap = g_4e3b44[*(long *)(definition + 0x18) & 0xffff].bytes;
    g_4b9a00 = g_485600;
    if (g_485607) g_4b9a00 = 4;
    /* retail takes the cosine of the unrounded product */
    real blend = (cosf(g_485618.forward.k * 3.1415927f) + 1.0f) * 0.5f;
    g_4b9d8c = 19;
    g_4b9d90 = 0.5f;
    g_4b9d94 = *(real *)(definition + 0x20) * (1.0f - blend) + *(real *)(definition + 0x1c) * blend;
    g_4b9d98 = *(real *)(definition + 0x24) * (1.0f - blend) + blend;
    g_4b9d9c = (definition[0] & 1) != 0;
    transform4x3f identity = *g_4687d0;
    transform4x3f *previous = &state->previous;
    if (!state->initialized)
    {
        memset(state, 0, sizeof(*state));
        filter_random_pair(state->points + 0);
        filter_random_pair(state->points + 1);
        filter_random_pair(state->points + 2);
        filter_random_pair(state->points + 3);
        *previous = *(transform4x3f *)g_48568c;
        state->initialized = true;
    }
    vector3f motion;
    function_28580(g_4857b8, time, &motion);
    identity.position.x = *(real *)(definition + 0x44) + motion.i;
    identity.position.y = *(real *)(definition + 0x48) + motion.j;
    identity.position.z = *(real *)(definition + 0x4c) + motion.k;
    transform4x3f relative;
    function_141590(previous, &relative);
    function_142a60(&identity, &relative, &relative);
    function_142a60((transform4x3f *)g_48568c, &relative, &relative);
    *previous = *(transform4x3f *)g_48568c;
    real far_distance = g_4857e0;
    real near_distance = g_4857dc;
    if (!(far_distance > near_distance)) return;
    real step = (far_distance - near_distance) * 0.25f;
    real inverse_span = 1.0f / (0.0001f > far_distance - near_distance ? 0.0001f : far_distance - near_distance);
    real aspect = ((real)(short)g_48564c - (real)(short)g_485648) /
        ((real)(short)g_48564e - (real)(short)g_48564a);
    real largest_distance = 0.0001f > far_distance ? 0.0001f : far_distance;
    real radius = (real)(*(real *)(definition + 0x10) * 0.5f /
        (tan(g_485640 * 0.5f) * largest_distance * aspect));
    vector3f forward_axis = { 1.0f, 0.0f, 0.0f };
    vector3f heading;
    matrix_transform_vector(&relative, &forward_axis, &heading);
    double angle_delta = atan2(heading.j, heading.i);
    real *angle = &state->angle;
    /* retail keeps the turned angle unrounded for the cosine and sine */
    double turned = *angle - -(angle_delta * *(real *)(definition + 4));
    *angle = (real)turned;
    real cosine = (real)cos(turned);
    real sine = (real)sin(turned);
    real volatile *phase_reference = (real volatile *)&state->phase;
    real stepped_phase = *phase_reference - *(real *)(definition + 0xc) / (step < 0.0001f ? 0.0001f : step) * relative.position.z;
    *phase_reference = stepped_phase;
    real crossing = (real)floor((double)stepped_phase);
    *phase_reference = (real)((double)*phase_reference - crossing);
    long *ring = &state->ring;
    *ring -= (long)crossing;
    while (*ring < 0) *ring += 4;
    while (*ring >= 4) *ring -= 4;
    if (crossing < 0.0f) filter_random_pair(state->points + ((*ring + 3) % 4));
    else if (crossing > 0.0f) filter_random_pair(state->points + *ring);
    if (state->phase < 0.0f || *phase_reference >= 1.0f) return;
    real offsets[4] = { 0.0f, 0.7135000228881836f, 0.34220001101493835f, 0.579800009727478f };
    for (long i = 0; i < 4; ++i)
    {
        if (*(real *)(definition + 0x28) > 0.0f)
        {
            double value = *(long *)(bitmap + 0x44) * offsets[i] +
                state->previous_time / *(real *)(definition + 0x28);
            real rounded = (real)value;
            records[i].bitmap = (long)floor(value) % *(long *)(bitmap + 0x44);
            records[i].fraction = rounded - (real)floor(value) < 0.0f ? 0.0f :
                rounded - (real)floor(value) > 1.0f ? 1.0f : rounded - (real)floor(value);
            if (records[i].bitmap < 0)
            {
                if (!g_55e6c4) g_55e6c4 = 1;
                records[i].bitmap = 0;
            }
        }
        else
        {
            records[i].bitmap = i % *(long *)(bitmap + 0x44);
            records[i].fraction = 0.0f;
        }
    }
    for (long j = 0; j < 4; ++j)
    {
        long index = (*ring + j) % 4;
        real distance = (j + state->phase) * step + g_4857dc;
        real fraction = (distance - g_4857dc) * inverse_span;
        real centered = fraction * 2.0f - 1.0f;
        real offset_from_center = centered >= 0.0f ? centered : 0.0f - centered;
        real cubed_offset = (real)pow((double)offset_from_center, 3.0);
        real weight = (real)(pow((double)(1.0f - cubed_offset), 2.0) * g_4b9d98);
        real size = *(real *)(definition + 0x10) / (g_4857e0 < 0.0001f ? 0.0001f : g_4857e0) * distance;
        real x = 0.0f, y = 0.0f, z = 0.0f - distance;
        if (relative.scale != 1.0f) { x *= relative.scale; y *= relative.scale; z *= relative.scale; }
        real dx = relative.up.i * z + relative.left.i * y + relative.forward.i * x + relative.position.x;
        real dy = relative.up.j * z + relative.left.j * y + relative.forward.j * x + relative.position.y;
        state->points[index].x -= (dy * sine * g_4b9d94 + dx * cosine) * *(real *)(definition + 8) * radius * 0.5f;
        state->points[index].y -= (dy * cosine * g_4b9d94 - dx * sine) * *(real *)(definition + 8) * radius * 0.5f;
        if (*(real *)(definition + 0x28) != 0.0f)
        {
            real t = ((s_27aa0_record *)g_4b9c74 + 1)[index].fraction;
            real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
            real weight_third = t * t * weight;
            weight_third = weight_third < 0.0f ? 0.0f : weight_third > 1.0f ? 1.0f : weight_third;
            real weight_second = (t * 2.0f) * (1.0f - t) * weight;
            weight_second = weight_second < 0.0f ? 0.0f : weight_second > 1.0f ? 1.0f : weight_second;
            real weight_first = ((1.0f - t) * (1.0f - t)) * weight;
            weight_first = weight_first < 0.0f ? 0.0f : weight_first > 1.0f ? 1.0f : weight_first;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[0] = weight_first;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[2] = weight_third;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[1] = weight_second;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[3] = bounded;
        }
        else
        {
            real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
            real clamped = weight < 0.0f ? 0.0f : weight > 1.0f ? 1.0f : weight;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[0] = 0.0f;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[1] = clamped;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[2] = 0.0f;
            ((s_27aa0_record *)g_4b9c74 + 1)[index].weights[3] = bounded;
        }
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[0] = size * cosine * 0.5f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[2] = 0.0f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[3] = state->points[index].x;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[1] = size * sine * aspect * 0.5f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[4] = size * sine * -0.5f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[5] = size * cosine * aspect * 0.5f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[6] = 0.0f;
        ((s_27aa0_record *)g_4b9c74 + 1)[index].coordinates[7] = state->points[index].y;
    }
}

#include <xmmintrin.h>
#include "geometry_cache.h"
extern real g_509418, g_4670fc;

struct s_38450_vertex
{
    point3f position;
    real size;
    dword color;
};
struct s_38450_particle
{
    vector3f velocity;
    real jitter_x, jitter_y, jitter_z;
    real lifetime, age;
};
struct s_38450_records
{
    long count;
    s_38450_vertex *vertices;
    long field_08;
    s_38450_particle *particles;
};

PRIVATE __forceinline real particle_inverse_sqrt(real magnitude)
{
    real result;
    __asm
    {
        rsqrtss xmm0, magnitude
        movss result, xmm0
    }
    return result;
}

// score 832 (was 832): the 0xd0 frame agrees. Retail uses XMM2 for zero and receives the state
// in EBX, while the build uses XMM3 and loads the state into EBP. Named fade progress 832; fade
// from separate progress branches 833; geometry-success scope 850; promoted player divisor 857;
// radius through the extent view 832; horizontal radius local 860 at retail length; partition-relative
// index 832; volume count initialized after the lock 860. The state parameter remains on the stack.
// Active fade initialized to zero 832, fade before the snapshots 832, explicit fade results 845
// (zero in XMM2, still a stack-passed state), named extent and camera 832, typed emitter 832,
// shared volume distance with a local camera sample 962, shared exclusion distance 891,
// absolute partition end 863. The opening extent and camera homes still differ.
// Extent copy through memcpy 832, scalar camera snapshot 846, camera-first copy 833,
// named live extent axes 858, explicit player divisor 857, explicit extent maximum 832,
// shared scaled velocity update 831, staged velocity increment 885, array element
// references 832. The scaled update gains one at the same extent; the stacked state remains.
// M3 lane 5 r247 (fast screen; aligned rows against retail in brackets, main 745): retail references no double constant
// here (one double global load) where ours has 41; its random fraction runs in x87 with float constants and doubles
// by `fadd st0, st0` after the 1/65535 multiply. The file's random fraction returning real with float call sites
// 854 (816; ours folds the doubling into the constant); the header's existing random real for every draw 853 (814);
// the float helper dividing by 65535 854 (816, still folded); `2.0f * r` 854 (816); only the three jitter draws through
// the header's random real 832 (736). Registers: retail keeps the state in EBX (register argument, `ret 4`), the
// vertex ESI, the seed pointer EDI and the particle EBP; the link-time colouring here ranks the seed pointer 1272,
// the particle 682, the vertex 492 and the state 185 (left with EBP, so it is passed on the stack), while a
// compile-only first pass of this file already gives the state EBX.
// M3 lane 13 r162 (h2m fast with 0x27aa0 and 0x22bbca; control 832, masked-aligned 462). The convention lever: the
// vertex pointer declared in each arm of the age test (no shared web) drops it below the state, which then takes EBX
// and the function returns `ret 4` as retail does: 858 (3504 B, masked 424; the extent and camera cells sit 8 and 4
// bytes higher than retail's; a declaration moved into the branches, held as a lead). Retail's jitter draws are float
// x87 math with the doubling as `fadd st0, st0` after the 1/65535 multiply: the file's existing
// random_real_range_28580(-1.0f, 1.0f) gives exactly that block, 839 for the three jitter draws, 842 for all nine
// plus-or-minus-one draws (3503 B, masked 431); the jitter field read into `varied` before the draw is added (retail
// loads it ahead of the seed step) with the helper 869 (block exact, 3507 B); those with the per-arm vertex 908 (3532 B,
// masked 381). The maximum speed read from the state at both uses 832 (same code). Body unchanged.
// M3 lane 11 r175 (h2m fast with 0x36b50, 0x38390, 0x27aa0, 0x22bbca): retail's draws are x87 float with the random value
// complete first (scale by 1/65535, then `fadd st0, st0` or `fld high; fsub low; fmulp; fadd low` on float memory
// operands, low read twice); ours folds the scale into the following product. The draws in float (the fraction helper
// returning real, 2.0f, no double casts) 854 (3506 B; the doubling still folds), the helper returning the header's
// random real 854 (same code); the double helper returning the header's random real 852 (3504 B); the float draws with
// the vertex pointer declared in each arm 849 (3498 B, 0x36b50 258 -> 286), plus the clamp into a long divisor 832
// (3514 B, 0x36b50 286). Re-measured: the long divisor alone 857. Body unchanged.
// M3 lane 7 r237 (h2m fast with 0x36b50, 0x38390, 0x27aa0 and 0x22bbca; control 832): the particle loop as a plain
// `j < start + count` loop 863; the tick delta read once ahead of the loop 852 (3507 B); the drift read once ahead of
// the loop 881 (3523 B); the limit as `g_4670fc * count` 832 (same code); the admission test as `!inside && active <=
// limit` 844 at retail's 3483 B (state still on the stack), with the vertex pointer declared in each arm of the age
// test as well 851 (3492 B: the state takes EBX and the function returns `ret 4` as retail, but 0x36b50 258 -> 286).
// @retail 0x38450
bool __stdcall function_38450(byte *state, byte const *definition, real remaining)
{
    struct s_extent { real width, height, depth, field_14; };
    s_extent extent = *(s_extent *)(state + 8);
    point3f camera = g_485618.position;
    real fade = 1.0f;
    long nearby_count = 0;
    if (remaining > 0.0f)
    {
        real duration = *(real *)(definition + 0xac);
        if (duration > 0.0f)
            fade = 1.0f - (remaining / duration < 1.0f ? remaining / duration : 1.0f);
        else fade = 0.0f;
    }
    *(real *)(state + 0x80) += g_509418;
    if (!function_12de70((s_geometry_block_info *)(state + 0x40), 3)) return true;
    s_38450_records *records = *(s_38450_records **)(state + 0x68);
    short players = *(short *)((byte *)g_4e8c20 + 8);
    players = players < 1 ? 1 : players > 4 ? 4 : players;
    long count = records->count / players;
    long start = g_485600 * count;
    long limit = (long)(count * g_4670fc);
    real radius = (*(real *)(state + 8) > *(real *)(state + 0xc) ? *(real *)(state + 8) : *(real *)(state + 0xc)) > *(real *)(state + 0x10) ? (*(real *)(state + 8) > *(real *)(state + 0xc) ? *(real *)(state + 8) : *(real *)(state + 0xc)) : *(real *)(state + 0x10);
    s_sphere_plane_volume *nearby[32];
    byte *structure = (byte *)g_4e0348;
    long volume_count = *(long *)(structure + 0x8c);
    s_sphere_plane_volume *volumes = *(s_sphere_plane_volume **)(structure + 0x90);
    for (long i = 0; i < volume_count; ++i)
    {
        real x = volumes[i].center.x - g_485618.position.x;
        real y = volumes[i].center.y - g_485618.position.y;
        real z = volumes[i].center.z - g_485618.position.z;
        real combined = volumes[i].radius + radius;
        if (combined * combined > z * z + y * y + x * x)
            nearby[nearby_count++] = volumes + i;
    }
    long active = 0;
    for (long j = start, remaining_count = count; remaining_count > 0; ++j, --remaining_count)
    {
        s_38450_vertex *vertex = records->vertices + j;
        s_38450_particle *particle = records->particles + j;
        if (particle->age >= 0.0f)
        {
            real delta = g_509418;
            particle->age += delta;
            real drift = *(real *)(definition + 0xa8);
            real varied = particle->jitter_z + (filter_random_fraction() * 2.0 - 1.0f) * drift;
            particle->jitter_z = (real)(varied < -1.0f ? -1.0f : varied > 1.0f ? 1.0f : varied);
            varied = particle->jitter_x + (filter_random_fraction() * 2.0 - 1.0f) * drift;
            particle->jitter_x = (real)(varied < -1.0f ? -1.0f : varied > 1.0f ? 1.0f : varied);
            varied = particle->jitter_y + (filter_random_fraction() * 2.0 - 1.0f) * drift;
            particle->jitter_y = (real)(varied < -1.0f ? -1.0f : varied > 1.0f ? 1.0f : varied);
            real vx = *(real *)(definition + 0x90) * particle->jitter_x + *(real *)(definition + 0x30);
            real vy = *(real *)(definition + 0x94) * particle->jitter_y + *(real *)(definition + 0x34);
            real vz = *(real *)(definition + 0x98) * particle->jitter_z + *(real *)(definition + 0x38);
            if (!(particle->lifetime > 0.0001f))
                particle->lifetime = (real)(filter_random_fraction() * 0.7f + 0.1f);
            real inverse = 1.0f / particle->lifetime;
            vx = inverse * vx; vy = inverse * vy; vz = inverse * vz;
            vz -= *(real *)(definition + 0x9c);
            particle->velocity.i += delta * vx;
            particle->velocity.j += delta * vy;
            particle->velocity.k += delta * vz;
            real magnitude = particle->velocity.k * particle->velocity.k +
                particle->velocity.j * particle->velocity.j + particle->velocity.i * particle->velocity.i;
            real maximum = *(real *)(state + 0x18);
            if (magnitude > maximum * maximum && magnitude > 9.99999905104687e-09f)
            {
                real scale = particle_inverse_sqrt(magnitude);
                scale *= maximum;
                particle->velocity.i *= scale;
                particle->velocity.j *= scale;
                particle->velocity.k *= scale;
            }
            vertex->position.x += particle->velocity.i * delta;
            vertex->position.y += particle->velocity.j * delta;
            vertex->position.z += particle->velocity.k * delta;
            real x = vertex->position.x, y = vertex->position.y, z = vertex->position.z;
            if (g_48568c[0] != 1.0f) { x *= g_48568c[0]; y *= g_48568c[0]; z *= g_48568c[0]; }
            real px = z * g_48568c[7] + y * g_48568c[4] + x * g_48568c[1] + g_48568c[10];
            real py = g_48568c[5] * y + g_48568c[2] * x + g_48568c[8] * z + g_48568c[11];
            real pz = g_48568c[6] * y + g_48568c[3] * x + g_48568c[9] * z + g_48568c[12];
            if (extent.width > 0.0001f && extent.height > 0.0001f && extent.depth > 0.0001f)
            {
                bool wrapped = false;
                px /= extent.width;
                px -= (long)px;
                if (px < -0.5f) { px += 1.0f; wrapped = true; }
                else if (px > 0.5f) { px -= 1.0f; wrapped = true; }
                px *= extent.width;
                py /= extent.height;
                py -= (long)py;
                if (py < -0.5f) { py += 1.0f; wrapped = true; }
                else if (py > 0.5f) { py -= 1.0f; wrapped = true; }
                py *= extent.height;
                if (pz < 0.0f - extent.depth) wrapped = true;
                pz /= extent.depth;
                pz -= (long)pz;
                if (pz > 0.0f) { pz -= 1.0f; wrapped = true; }
                pz *= extent.depth;
                if (wrapped)
                {
                    if (g_48568c[13] != 1.0f) { px *= g_48568c[13]; py *= g_48568c[13]; pz *= g_48568c[13]; }
                    vertex->position.x = g_48568c[20] * pz + g_48568c[17] * py + g_48568c[14] * px + g_48568c[23];
                    vertex->position.y = g_48568c[21] * pz + g_48568c[18] * py + g_48568c[15] * px + g_48568c[24];
                    vertex->position.z = g_48568c[22] * pz + g_48568c[19] * py + g_48568c[16] * px + g_48568c[25];
                }
                else if (*(short *)(state + 0x6c) != 2)
                {
                    real distance = pz * pz + py * py + px * px;
                    if (0.04000000283122063f > distance)
                        vertex->color = ((long)(distance * 24.999998092651367f * fade * 256.0f) << 24) | 0xffffff;
                }
            }
            bool inside = false;
            real exclusion = *(real *)(state + 0x14);
            if (exclusion > 0.0f)
            {
                real dx = g_485618.position.x - vertex->position.x;
                real dy = g_485618.position.y - vertex->position.y;
                real dz = g_485618.position.z - vertex->position.z;
                inside = exclusion * exclusion > dz * dz + dy * dy + dx * dx;
            }
            if (!inside)
                for (long k = 0; k < nearby_count; ++k)
                    if (function_38390(&vertex->position, nearby[k])) { inside = true; break; }
            if (active <= limit && !inside) ++active;
            else particle->age = -1.0f;
        }
        else if (active < limit)
        {
            vertex->color = 0xffffffff;
            particle->lifetime = (real)(filter_random_fraction() *
                ((double)*(real *)(state + 0x20) - *(real *)(state + 0x1c)) + *(real *)(state + 0x1c));
            particle->velocity.i = (real)(filter_random_fraction() * *(real *)(state + 0x30));
            particle->velocity.j = (real)(filter_random_fraction() * *(real *)(state + 0x34));
            particle->velocity.k = (real)(filter_random_fraction() * *(real *)(state + 0x38));
            vertex->size = (real)(filter_random_fraction() *
                ((double)*(real *)(state + 0x28) - *(real *)(state + 0x24)) + *(real *)(state + 0x24));
            particle->age = 0.0f;
            vertex->position.x = (real)((filter_random_fraction() * 2.0 - 1.0f) * extent.depth + camera.x);
            vertex->position.y = (real)((filter_random_fraction() * 2.0 - 1.0f) * extent.width + camera.y);
            vertex->position.z = (real)((filter_random_fraction() * 2.0 - 1.0f) * extent.height + camera.z);
            particle->jitter_x = (real)(filter_random_fraction() * 2.0 - 1.0f);
            particle->jitter_y = (real)(filter_random_fraction() * 2.0 - 1.0f);
            particle->jitter_z = (real)(filter_random_fraction() * 2.0 - 1.0f);
        }
    }
    return true;
}


// Disabled: the shared camera is a 0x24-byte prefix, but 0x12fa0 reads through +0x36; shared addresses prohibited.
#if 0
#include <xtl.h>
extern byte g_4670bc;
extern byte *g_485a80;
extern long g_4858b8;
struct s_render_reset_state;
struct s_shader_cache;
void function_16b10(s_render_reset_state *);
void function_1c590(s_shader_cache *, long, long);
void function_12fa0(byte const *, real const *, bool, long);
bool function_39780(bool);
void function_1cf80();
bool function_143c0(long, short, short, real);

// Retail 0x391f0
bool __stdcall function_391f0(byte *state)
{
    bool lines = *(short *)(state + 0x6c) == 2;
    if (!function_12de70((s_geometry_block_info *)(state + 0x40), 3)) return true;
    s_38450_records *records = *(s_38450_records **)(state + 0x68);
    g_4670bc = 1;
    function_16b10((s_render_reset_state *)g_485b48);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0xb4), 0);
    function_12fa0((byte *)&g_485618, g_48568c, false, g_4858b8);
    bool result = function_39780(lines);
    if (!lines)
    {
        D3DDevice_SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
        D3DDevice_SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
        D3DDevice_SetRenderState(D3DRS_POINTSCALE_A, 0);
        D3DDevice_SetRenderState(D3DRS_POINTSCALE_B, 0);
        real scale = 1.0f;
        D3DDevice_SetRenderState(D3DRS_POINTSCALE_C, *(dword *)&scale);
    }
    dword first_alpha = (dword)(long)(*(real *)(state + 0x70) * 256.0f) << 24;
    dword second_alpha = (dword)(long)(*(real *)(state + 0x74) * 256.0f) << 24;
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    D3DDevice_SetRenderState(D3DRS_ALPHAREF, 0);
    D3DDevice_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
    D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    if (!lines) function_143c0(*(long *)(state + 4), 0, 3, 0.0f);
    function_1cf80();
    if (lines) D3DDevice_SetRenderState(D3DRS_LINEWIDTH, *(dword *)(state + 0x7c));
    D3DDevice_Begin(lines ? D3DPT_LINELIST : D3DPT_POINTLIST);
    short players = *(short *)((byte *)g_4e8c20 + 8);
    long divisor = players < 1 ? 1 : players > 4 ? 4 : players;
    long count = records->count / divisor;
    long first = g_485600 * count;
    for (long i = first; i < first + count; ++i)
    {
        s_38450_vertex *vertex = records->vertices + i;
        s_38450_particle *particle = records->particles + i;
        if (!(particle->age > 0.0f)) continue;
        if (!lines)
        {
            D3DDevice_SetVertexDataColor(1, vertex->color);
            D3DDevice_SetVertexData4f(0, vertex->position.x, vertex->position.y,
                vertex->position.z, vertex->size * 16777215.0f);
        }
        else
        {
            real scale = g_509418;
            point3f previous;
            previous.x = vertex->position.x - particle->velocity.i * scale * *(real *)(state + 0x78);
            previous.y = vertex->position.y - particle->velocity.j * scale * *(real *)(state + 0x78);
            previous.z = vertex->position.z - particle->velocity.k * scale * *(real *)(state + 0x78);
            D3DDevice_SetVertexDataColor(1, (vertex->color & 0xffffff) | first_alpha);
            D3DDevice_SetVertexData4f(0, previous.x, previous.y, previous.z, 1.0f);
            D3DDevice_SetVertexDataColor(1, (vertex->color & 0xffffff) | second_alpha);
            D3DDevice_SetVertexData4f(0, vertex->position.x, vertex->position.y, vertex->position.z, 1.0f);
        }
    }
    D3DDevice_End();
    D3DDevice_SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    g_4670bc = 1;
    function_16b10((s_render_reset_state *)g_485b48);
    return result;
}
#endif


#include <xtl.h>
#include "geometry_cache.h"
#include "shader_storage.h"
extern byte g_4670bc;
extern byte *g_485a80;
extern long g_4858b8;
struct s_render_reset_state;
struct s_shader_cache;
void function_16b10(s_render_reset_state *);
void function_1c590(s_shader_cache *, long, long);
void function_12fa0(byte const *, real const *, bool, long);
bool function_39780(bool);
void function_1cf80();
bool function_143c0(long, short, short, real);


// score 175 (was 189): GB24's 174 form is not kept (declaration order and one-use locals are kept only on a byte match):
// declaring result before lines, particle before vertex (fixing both loop induction homes
// [esp+0x14]/[esp+0x1c]), and naming the per-tick displacement components delta_x/delta_y/delta_z = velocity * scale
// matches the line arm verbatim (lines 175-278, 189 -> 174, 1420/1420 B, al=96). Earlier (310 -> 189): initializing
// the draw result before the geometry check restores the early true byte and puts the failure exit after the draw exit.
// The state still arrives in EAX, and the shader slot load and primitive submission differ. Short player sample 362; direct SDK
// W-buffer state 365; separate primitive branches 421, with a fresh mode read 422; named partition
// end 365; shader mode helper 364; shader slot helper 378.
// The unqualified vertex cursor restores the stack-passed state but shortens the draw body.
// Unqualified vertex 409, with live primitive branches and a real width 459. Typed state 344,
// real line width 352, current mode selecting the primitive 366, partition offset 340,
// positive age branch 344, previous point setter 346, typed shader slot 344, indexed vertex 371.
// NOT kept: the partition-offset loop gains four at retail length.
// With the ordinary vertex cursor, selected primitive 409, common final submission 509,
// final width assigned with color 509, submitted position snapshots 436, aggregate previous
// point 409, direct SDK width and separate begins 474, vertex element reference 409,
// shared displacement setter 408. The stack state comes back in six forms; each shortens
// the push-buffer code and raises the score.
// M3 lane 3 r310 (fast screen, control 344). Kept: the three vertex writes given the platform's vertex-register
// constant (-1) where the body passed 0 (retail's method word is 0x101518 at all three, ours was 0x101a00; same count,
// a real behaviour fix); point-scale A and B set from the scale local, as a float zero read through its address the
// way the C value is (retail stores a float zero; a separate named zero is the same code) 310. Not kept: the primitive chosen in a local under a fresh mode-word test with the
// line width read as a float (retail compares the mode word again and passes the width through a stack copy) 299,
// with the player count as a short (retail's 16-bit compares) 296: the fresh test re-reads the mode word that `lines`
// holds at every other use (a mix), and without it the same arm under `lines` is 316, with the `long` count 317; the
// short count alone on 310 is 310. Tried: the non-volatile cursor with the named zero 366 (1357 B,
// aligned rows 96 against 172); three named point-scale floats 352; one scale local set to 0 and then 1 310 (same
// code); a by-value float-bits helper for A and B 352, for all three 351; the existing 0x1cbe0(0, 0, 22) for the
// shader slot read 349 (retail's add and load pair, aligned 172 -> 160), with the non-volatile cursor 397, on the 299
// form 350 (1424 B), with the short count 348; separate primitive arms with a fresh mode test 360 (1368 B: both begin
// calls stay out of line), with the non-volatile cursor 406 (1319 B); a named line-mode bool selecting the primitive
// 357; the primitive selected under `lines` 317; the width under `lines` with the conditional begin argument 326 (1432
// B); the `!= 2` arm first 315 (1425 B); the state read once through its address 310 (stack-passed but held in EDI:
// retail keeps the constant 2 there), on the 296 form 319 (1429 B); the 296 form with the non-volatile cursor 339 (1387
// B). Diagnostic, not kept: a forced-inline wrapper for the begin call (both arms expanded, as in retail) with the
// non-volatile cursor, the slot helper and the short count 251 at retail's 1420 B (aligned 101). Our link leaves a
// begin site under two tests out of line where retail expands both (the inliner's limit, not a spelling). Left on that
// form: the constant 2 not held in EDI, the point-scale zero passed through the stack, the loop's induction homes and
// the line arm's product order. On the 296 form: the line arm's products written scale first 296, the state factor
// first 296, `(velocity * scale) * factor` 295; the zero `real const` 296; 1-12 dead longs at the top 296 each (not a
// symbol-count state).
// M3 lane 5 r247 (fast screen, plain 189; aligned counts against retail in brackets, main 168): kept: the begin call
// through the SDK's force-inline member form in each arm of `if (lines)` (the line width read into a float local and
// passed by its bits in the line arm, retail's stack copy), with the plain vertex cursor: both begin sites expand as
// in retail and the state arrives on the stack in EBP, 310 -> 189 (102). On that form: the cursor volatile again 359
// (1456 B); the width read straight as a dword 325 (1398 B, one begin site stays out of line); the arms tested on a
// fresh mode-word compare 173 (92); the point arm first on `!lines` 206 (90), on a fresh `!= 2` compare 279 (80); the
// player count as a short 204 (96), with the point arm first 216 (83), with that on the fresh compare 265 (73); the
// existing 0x1cbe0(0, 0, 22) for the shader slot read 319 (106), with the point arm first and the short count 319
// (62), on the fresh compare 252 (70); the member forms for every device call (same code); the point-scale zero for
// A and B through a by-value float-bits helper 342 (62: both stored straight from XMM0 as retail); the particle
// pointer declared ahead of the vertex pointer 266 (73, retail's load order, the stride homes still swapped). Left:
// retail keeps the constant 2 in EDI from entry (both mode compares, the depth-buffer value, the line primitive),
// which also gives retail's zero in ESI, the records pointer in EDI through the loop and the pop order. The allocator
// ranks the second alpha (-6, created later) ahead of the constant 2 (-6 to -10 in these forms); a forced-ranking
// search that puts the constant first gives aligned 34. 0x36b50 now pushes the state as retail (aligned 173 both).
// Round 159 (m3 lane 14, fast): the mode word read at every use (no bool) 366; a fresh mode compare at the begin arms
// only 173 (re-measured), there and at the point-scale call test 167 (the constant 2 then sits in EDI from entry as in
// retail), in the loop as well 363. On the 167 form: the point arm first on the fresh `!= 2` compare 111, with the
// player count as a short (retail's 16-bit compares) 86 at 1420 B; the line arm first with the short count 145. Match-
// only lead (a bool and fresh compares of the same word mixed): left on it are the shader slot read (retail add 0xb0
// then load +4; a typed slot read, a slot pointer and an index spelling all fold to one load, 86; the existing 0x1cc30
// stays a call 343, 0x1cbe0(0, 0, 22) 313), the sete / flag order at entry, the point-scale zeroes stored straight from
// XMM0, and retail testing the bool (not the word) before the point-scale call. Without the 167 form the constant
// stays out of EDI: the short count alone 204, with the point arm first 265, the typed slot read alone 189 (same code).
// GB24 (43 valid fast trials across 3 batches, 189 -> 174 not kept): naming per-tick displacement delta_x/y/z = velocity * scale
// 175 (al=98, exact line arm); + result before lines 175 (al=96, mov bl,1 before sete); + particle before vertex 174 (
// fixes [esp+0x14]/[esp+0x1c] homes). Also tried: vector3f scaled 269 (1427 B); vector3f_add_scaled 278; inline g_509418 189/175;
// dt+length locals 235; union scale 189 / union zero only 188 (174 with delta); by-value float-bits helper (A/B, all) 338/338
// (al=99/98, emits retail's movss [mem],xmm0 for A/B); switch(mode) default/case 2 208 (197 with delta); case 0/1/2 349 (1431 B);
// typed s_391f0_state 189; vector3f_scale (normalize3d.h) 270/265; signed alpha 297; A/B before enables 338; loop end local 202;
// divisor if-chain 175; records after reset 175; points=!lines 175; diagnostic fresh != 2 at both pointscale & begin + float-bits
// + particle-first + short players + parens reaches al=45 at 1420 B (lines 176-348 verbatim; not kept, mixed mode test).
// M3 lane 7 r235 (fast screen, plain 175): kept, the line arm's per-tick displacement grouped as
// `(particle->velocity.i * scale) * factor` in all three rows, 189 -> 175 (the line arm is retail's: velocity times
// scale, then the state factor; no new names). Tried on main's body: the age test enclosing the arms in place of
// `continue` 189; `result` declared after `lines` 189; the element pointers as `&records->vertices[i]` 189 (all three
// the same code); the geometry test as an early `return true` with `result` declared at the 0x39780 call 352 (1406 B).
// On the 175 form: the player count as a short 190; the point arm first on `!lines` 195; the loop bound named
// `last` 175, a const records pointer 175, the clamp's inner ternary parenthesised 175 (all three the same code).
// M3 lane 11 r175 (h2m fast with 0x36b50 and 0x1cbe0). Lead (held as a patch; each piece is kept only with a match):
// `result` declared ahead of `lines`, the shader slot through the existing 0x1cbe0(0, 0, 22) (as matched callers in
// two other files do), point-scale A and B from a by-value float-bits helper (retail stores XMM0 straight to both), the
// begin arms on a fresh `!= 2` compare with the point arm first, and the player count a short: 348, where only the
// constant 2 misses EDI; with the point-scale call test on a fresh compare as well 72, every other row byte-exact (that
// test is retail's bool). Allocator reading on the 348 form: the constant 2 ranks -7 against the second alpha's -6 (it
// pays 9 for the 9-candidate block after the geometry test and 5 for the point arm's two blocks); a forced-ranking
// search gives a byte match with the constant raised by 2 (to -5 or 1) or the second alpha lowered by 2 (to -8; -14
// misses). The point-scale test sits in the alphas' block: there the bool makes 13 candidates (the second alpha +78)
// where the fresh compare makes 12 (+72, and +12 for the constant). Tried without moving it: the line arm reading the
// global at each use 348, the state factor named once 286, the rgb named once 345 (frame 0x44), a named loop end 348,
// a named geometry pointer 348, the line width and point-scale C through the float-bits helper 348 each, the
// parameter typed `byte *` 348; a redundant `if (count > 0)` around the loop
// puts 2 in EDI but emits its test (198, 1432 B), the loop's own `first < first + count` folds (348). Earlier steps: a
// short mode local compared at each use 335 (the word takes EDI); the begin-only fresh compare 173, with the short count
// 285, with the point arm first too 265; the mode word named for the entry bool and the begin compare 167, with the point
// arm first 142, plus 0x1cbe0 145, plus the float-bits A and B 244, plus the short count 223 (aligned to the loop pad).
// M3 lane 7 r237 (h2m fast with 0x36b50 and 0x1cbe0). Lane 11 r175's 72 lead rebuilt (72), with retail's bool at the
// point-scale call test 348. Lead (held as a patch; each piece is kept only with a match): on that 348 form the records
// pointer read at the top, ahead of the geometry test, 14 at 1420 B: the allocator then ranks the constant 2 above
// the second alpha (-4 against -6: the entry block gains one referenced value and the block after the geometry test
// loses two) and every row is retail's but the records load, which now comes ahead of the geometry call. Tried on the
// 348 form: the second color's operands swapped 348; the begin arms as a switch on the mode 300; the line arm first on
// `== 2` 348; `result` declared after `lines` 348; the reset flag stored ahead of the records read 348; the age test
// enclosing the arms 348; `count * g_485600` 348; the particle pointer ahead of the vertex pointer 349; the age test as
// `<= 0.0f` 348; the loop's arms swapped on `if (lines)` 342; the existing reset helper 0x1bbd0 at both sites 358
// (1394 B, a call; diagnostic with `inline` on its definition, not kept: 348, the same counts as the written pair); the
// plain begin call in either arm 382 (1383 B, out of line); the geometry test as an early `return true` 355 (1408 B).
// @retail 0x391f0
bool __stdcall function_391f0(void *state_address)
{
    byte *state = (byte *)state_address;
    bool lines = *(short *)(state + 0x6c) == 2;
    bool result = true;
    if (function_12de70((s_geometry_block_info *)(state + 0x40), 3))
    {
        s_38450_records *records = *(s_38450_records **)(state + 0x68);
        g_4670bc = 1;
        function_16b10((s_render_reset_state *)g_485b48);
        function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0xb4), 0);
        function_12fa0((byte *)&g_485618, g_48568c, false, g_4858b8);
        result = function_39780(lines);
        if (!lines)
        {
            D3DDevice_SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
            D3DDevice_SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
            real scale = 0.0f;
            D3DDevice_SetRenderState(D3DRS_POINTSCALE_A, *(dword *)&scale);
            D3DDevice_SetRenderState(D3DRS_POINTSCALE_B, *(dword *)&scale);
            scale = 1.0f;
            D3DDevice_SetRenderState(D3DRS_POINTSCALE_C, *(dword *)&scale);
        }
        dword first_alpha = (dword)(long)(*(real *)(state + 0x70) * 256.0f) << 24;
        dword second_alpha = (dword)(long)(*(real *)(state + 0x74) * 256.0f) << 24;
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        D3DDevice_SetRenderState(D3DRS_ALPHAREF, 0);
        D3DDevice_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
        D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        if (!lines) function_143c0(*(long *)(state + 4), 0, 3, 0.0f);
        function_1cf80();
        if (lines)
        {
            real width = *(real *)(state + 0x7c);
            D3DDevice_SetRenderState(D3DRS_LINEWIDTH, *(dword *)&width);
            D3DDevice::Begin(D3DPT_LINELIST);
        }
        else
        {
            D3DDevice::Begin(D3DPT_POINTLIST);
        }
        long players = *(short *)((byte *)g_4e8c20 + 8);
        long divisor = players < 1 ? 1 : players > 4 ? 4 : players;
        long count = records->count / divisor;
        long first = g_485600 * count;
        for (long i = first; i < first + count; ++i)
        {
            s_38450_vertex *vertex = records->vertices + i;
            s_38450_particle *particle = records->particles + i;
            if (!(particle->age > 0.0f)) continue;
            if (!lines)
            {
                D3DDevice_SetVertexDataColor(1, vertex->color);
                D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertex->position.x, vertex->position.y,
                    vertex->position.z, vertex->size * 16777215.0f);
            }
            else
            {
                real scale = g_509418;
                point3f previous;
                previous.x = vertex->position.x - (particle->velocity.i * scale) * *(real *)(state + 0x78);
                previous.y = vertex->position.y - (particle->velocity.j * scale) * *(real *)(state + 0x78);
                previous.z = vertex->position.z - (particle->velocity.k * scale) * *(real *)(state + 0x78);
                D3DDevice_SetVertexDataColor(1, (vertex->color & 0xffffff) | first_alpha);
                D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, previous.x, previous.y, previous.z, 1.0f);
                D3DDevice_SetVertexDataColor(1, (vertex->color & 0xffffff) | second_alpha);
                D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertex->position.x, vertex->position.y, vertex->position.z, 1.0f);
            }
        }
        D3DDevice_End();
        D3DDevice_SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
        D3DDevice_SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        g_4670bc = 1;
        function_16b10((s_render_reset_state *)g_485b48);
    }
    return result;
}
