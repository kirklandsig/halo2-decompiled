// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_02B400.CPP: vector math and rectangular grid placement */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "globals.h"

struct s_masked_list;
typedef void (__stdcall *masked_list_proc)(long value, short mask);
void masked_list_iterate(s_masked_list const *list, long mask, masked_list_proc proc);
void __stdcall function_c3340(long light_index, long unused);
void function_3eec0();
s_masked_list *g_547f98;

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
        real height = out[3] - out[2];
        real x = (view->center_x + 1.0f) * 0.5f * width + out[0];
        real y = (view->center_y + 1.0f) * 0.5f * height + out[2];
        real half_width = *(real const volatile *)&view->crop_scale * width * 0.5f;
        real half_height = *(real const volatile *)&view->crop_scale * height * 0.5f;
        out[0] = x - half_width;
        out[2] = y - half_height;
        out[3] = half_height + y;
        out[1] = half_width + x;
    }
    else if (view->offset)
    {
        real x = (out[1] - out[0]) * (1.0f / 640.0f) * view->offset_x;
        real y = (out[3] - out[2]) * (1.0f / 480.0f) * view->offset_y;
        out[0] += x;
        real right = out[1] + x;
        out[2] -= y;
        out[3] -= y;
        out[1] = right;
    }
    return true;
}

// @retail 0x2f800
void function_2f800(s_2f800_source const *source, s_2f800_size const *first,
	s_2f800_size const *second, s_2f800_view *view)
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
    real half_width = (out->clip.x1 - out->clip.x0) * 0.5f;
    real half_height = (out->clip.y1 - out->clip.y0) * 0.5f;
    real offset_x = (out->clip.x1 + out->clip.x0) / half_width * -0.5f;
    real offset_y = (out->clip.y0 + out->clip.y1) / half_height * -0.5f;
    out->half_height = half_height;
    out->half_width = half_width;
    out->offset_x = offset_x;
    out->offset_y = offset_y;
    real volatile tangent = (real)tan((double)view->angle * 0.5f);
    real aspect = out->width / out->height;
    if (!(g_4e6948 && g_4e6948->flag && g_4e6948->index != NONE && g_4e6948->state == 3) && g_485ac2)
    {
        aspect *= 4.0f / 3.0f;
        if (g_510c50 && ((byte *)g_510c50)[5])
            tangent = tangent * (4.0f / 3.0f) * 0.5625f;
    }
    else if (g_510c50 && ((byte *)g_510c50)[5])
    {
        real scale = out->height * (1.0f / 480.0f);
        out->clip.y0 *= scale;
        out->clip.y1 *= scale;
    }
    out->scale_x = 1.0f / (half_width * aspect * tangent);
    real inverse_x = 1.0f / out->scale_x;
    out->scale_y = 1.0f / (out->half_height * tangent);
    real left = 0.0f - (offset_x + 1.0f) * inverse_x;
    out->right = 0.0f - (offset_x - 1.0f) * inverse_x;
    real inverse_y = 1.0f / out->scale_y;
    real bottom = 0.0f - (out->offset_y + 1.0f) * inverse_y;
    real top = 0.0f - (out->offset_y - 1.0f) * inverse_y;
    out->left = left;
    out->bottom = bottom;
    out->top = top;
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
	long small = (exponent - 127) >> 31;
	long result = ((mantissa ^ sign) - sign) & ~small;
	long increment;
	if (!sign && bits && (small || (((1 << shift) - 1) & bits & 0x7fffff)))
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
__declspec(noinline) real normalize2d(point2f *v);

// @retail 0x2b400
real normalize2d(point2f *v)
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
vector3f *function_4efb0(vector3f *v)
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
void function_2f640(long index, s_grid_pair *extent, s_grid_pair const *dimensions,
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
    real length = (real)sqrt((double)axis->k * axis->k + (double)axis->j * axis->j + (double)axis->i * axis->i);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        axis->i *= inverse;
        axis->j *= inverse;
        axis->k *= inverse;
    }
}

// @retail 0x2fd90
void function_2fd90(s_2f800_view const *view, box2f const *clip, byte *out)
{
    s_projection_scalars scalars;
    function_2fb70(view, clip, &scalars);
    vector3f right, up, backward;
    right.i = view->up.k * view->forward.j - view->up.j * view->forward.k;
    right.j = view->forward.k * view->up.i - view->up.k * view->forward.i;
    right.k = view->up.j * view->forward.i - view->forward.j * view->up.i;
    up.i = view->forward.k * right.j - view->forward.j * right.k;
    up.j = view->forward.i * right.k - view->forward.k * right.i;
    up.k = view->forward.j * right.i - view->forward.i * right.j;
    backward.i = 0.0f - view->forward.i;
    backward.j = 0.0f - view->forward.j;
    backward.k = 0.0f - view->forward.k;
    normalize_projection_axis(&right);
    normalize_projection_axis(&up);
    normalize_projection_axis(&backward);
    transform4x3f *camera = (transform4x3f *)(out + 0x34);
    camera->forward = right;
    camera->left = up;
    camera->up = backward;
    camera->position = view->position;
    camera->scale = 1.0f;
    transform4x3f *inverse = (transform4x3f *)out;
    function_141590(camera, inverse);
    memcpy(out + 0x68, &scalars.left, 16);
    *(real *)(out + 0xb8) = scalars.scale_x * scalars.width * 0.5f;
    *(real *)(out + 0xbc) = scalars.scale_y * scalars.height * 0.5f;
    real x, y, z, distance;
    if (view->near_distance == 0.0f)
    {
        real const *plane = (real const *)view->unknown48;
        x = inverse->up.i * plane[2] + inverse->left.i * plane[1] + inverse->forward.i * plane[0];
        y = inverse->up.j * plane[2] + inverse->left.j * plane[1] + inverse->forward.j * plane[0];
        z = inverse->up.k * plane[2] + inverse->left.k * plane[1] + inverse->forward.k * plane[0];
        distance = plane[3] * inverse->scale + inverse->position.z * z + inverse->position.y * y + inverse->position.x * x;
    }
    else
    {
        x = y = 0.0f;
        z = 1.0f;
        distance = 0.0f - view->near_distance;
    }
    real reciprocal = 1.0f / z;
    real near_value = 0.0f - reciprocal * distance;
    real scale = (real)(view->far_distance / ((view->far_distance - (double)near_value) *
        (fabs((double)reciprocal * x) + fabs((double)reciprocal * y) + 1.0f)));
    real a = reciprocal * scale * x;
    real b = reciprocal * scale * y;
    real c = 0.0f - scale * near_value;
    if (c > 0.0f && view->near_distance == 0.0f)
    {
        a = 0.0f - a;
        b = 0.0f - b;
        c = 0.0f - c;
        scale = 0.0f - scale;
    }
    real *matrix = (real *)(out + 0x78);
    memset(matrix, 0, 64);
    matrix[0] = scalars.scale_x;
    matrix[5] = scalars.scale_y;
    matrix[8] = 0.0f - scalars.offset_x;
    matrix[9] = 0.0f - scalars.offset_y;
    matrix[2] = 0.0f - a;
    matrix[6] = 0.0f - b;
    matrix[10] = 0.0f - scale;
    matrix[11] = -1.0f;
    matrix[14] = c;
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

// @retail 0x3f660
void function_3f660(transform4x3f const *matrix)
{
    bool restore = g_47989c != NULL;
    if (restore)
        function_146de0();
    byte *collection = g_510c44;
    g_510c48 = true;
    byte setup[0x54];
    function_1726d0((s_view_setup *)setup, (point3f const *)((byte const *)matrix + 4),
        (vector3f const *)((byte const *)matrix + 0x1c), (vector3f const *)((byte const *)matrix + 0x28));
    s_2f970_view view;
    function_2f800((s_2f800_source const *)setup, NULL, NULL, (s_2f800_view *)&view);
    view.top = view.left = 0;
    view.bottom = 480;
    view.right = 640;
    view.outer_top = view.top;
    view.outer_left = view.left;
    view.outer_bottom = view.bottom;
    view.outer_right = view.right;
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
    function_441b0((s_planar_camera_source const *)&view, &camera, projection);
    function_173130(1, (byte *)&camera, *(short *)(setup + 0x10), (s_view_collection_1733e0 *)collection);
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
    byte field_00[0x44];
    real previous_time;
    byte field_48[0x20];
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

long g_4b9a00;
s_28580_view g_4b9a04[5];

// @retail 0x28580
void function_28580(long tag, real time, vector3f *out)
{
    s_28580_definition const *definition =
        (s_28580_definition const *)g_4e3b44[tag & 0xffff].data;
    s_28580_view *state = &g_4b9a04[g_4b9a00];
    real delta = time - state->previous_time;
    state->previous_time = time;
    if (definition->speed_high > 0.0f)
    {
        real blend = definition->blend;
        state->direction.x *= 1.0f - blend;
        state->direction.y *= 1.0f - blend;
        state->direction.x += state->target_direction.x * blend;
        state->direction.y += state->target_direction.y * blend;
        real magnitude = (real)sqrt(state->direction.x * state->direction.x +
            state->direction.y * state->direction.y);
        if (!(fabs(magnitude) < k_real_epsilon))
        {
            real inverse = 1.0f / magnitude;
            state->direction.x *= inverse;
            state->direction.y *= inverse;
        }
        else magnitude = 0.0f;
        if (magnitude == 0.0f)
        {
            state->direction.x = 1.0f;
            state->direction.y = 0.0f;
        }
        state->speed = (1.0f - definition->blend) * state->speed +
            definition->blend * state->target_speed;
        if (time - state->changed_time >= state->duration)
        {
            g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
            double fraction = (double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
            real turn = (real)pow(fraction, 1.0 - definition->turn);
            g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
            real sign = (g_4e7408->seed & 0x80000000) ? -1.0f : 1.0f;
            real cross_x = 0.0f - state->direction.y;
            real cross_y = state->direction.x;
            state->target_direction.x = (1.0f - turn) * state->direction.x + sign * turn * cross_x;
            state->target_direction.y = (1.0f - turn) * state->direction.y + sign * turn * cross_y;
            if (normalize2d(&state->target_direction) == 0.0f)
            {
                state->target_direction.x = 1.0f;
                state->target_direction.y = 0.0f;
            }
            g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
            double speed_fraction = (double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
            state->changed_time = time;
            state->target_speed = (real)(definition->speed_low +
                (definition->speed_high - definition->speed_low) * speed_fraction);
            g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
            double duration_fraction = (double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
            state->duration = (real)(definition->duration_low +
                (definition->duration_high - definition->duration_low) * duration_fraction);
        }
    }
    out->i = state->direction.x * state->speed * delta;
    out->j = state->direction.y * state->speed * delta;
    out->k = 0.0f;
}

#include <string.h>
extern long g_4857b8;
extern real g_4857dc, g_4857e0, g_485640;
extern real g_4b9d90, g_4b9d94;
extern bool g_4b9d9c;
extern short g_485600;
extern byte g_485607;
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern real g_48568c[46];
extern double g_4858a0;
extern transform4x3f *g_4687d0;
struct s_frame_offset { point3f position; vector3f forward; vector3f up; };
extern s_frame_offset g_485618;
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

struct s_27aa0_record
{
    long bitmap;
    real fraction;
    real weights[4];
    real coordinates[8];
};
extern real g_4b9c74[5][14];
long g_4b9d8c;
real g_4b9d98;
byte g_55e6c4;

PRIVATE __forceinline double filter_random_fraction(void)
{
    g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
    return (double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
}

PRIVATE __forceinline void filter_random_pair(point2f *point)
{
    g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
    g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
    point->x = (real)((double)(g_4e7408->seed >> 16) * (1.0f / 65535.0f));
    point->y = (real)(word)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
}

// @retail 0x27aa0
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
    real blend = (real)((cos((double)g_485618.forward.k * 3.1415927410125732f) + 1.0f) * 0.5f);
    g_4b9d8c = 19;
    g_4b9d90 = 0.5f;
    g_4b9d94 = *(real *)(definition + 0x20) * (1.0f - blend) + *(real *)(definition + 0x1c) * blend;
    g_4b9d98 = *(real *)(definition + 0x24) * (1.0f - blend) + blend;
    g_4b9d9c = (definition[0] & 1) != 0;
    transform4x3f identity = *g_4687d0;
    transform4x3f *previous = (transform4x3f *)(state->field_00 + 4);
    point2f *points = (point2f *)state->field_48;
    if (!state->field_00[0])
    {
        memset(state, 0, sizeof(*state));
        filter_random_pair(points + 0);
        filter_random_pair(points + 1);
        filter_random_pair(points + 2);
        filter_random_pair(points + 3);
        *previous = *(transform4x3f *)g_48568c;
        state->field_00[0] = 1;
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
    real near_distance = g_4857dc;
    real far_distance = g_4857e0;
    if (!(far_distance > near_distance)) return;
    real step = (far_distance - near_distance) * 0.25f;
    real inverse_span = 1.0f / (far_distance - near_distance < 0.0001f ? 0.0001f : far_distance - near_distance);
    real aspect = ((real)(short)g_48564c - (real)(short)g_485648) /
        ((real)(short)g_48564e - (real)(short)g_48564a);
    real largest_distance = far_distance < 0.0001f ? 0.0001f : far_distance;
    real radius = (real)(*(real *)(definition + 0x10) * 0.5f /
        (tan((double)g_485640 * 0.5f) * largest_distance * aspect));
    real angle_delta = (real)atan2(relative.forward.j + (relative.left.j + relative.up.j) * 0.0f,
        relative.forward.i + (relative.left.i + relative.up.i) * 0.0f);
    real *angle = (real *)(state->field_00 + 0x3c);
    *angle -= angle_delta * *(real *)(definition + 4);
    real cosine = (real)cos((double)*angle);
    real sine = (real)sin((double)*angle);
    real phase = *(real *)(state->field_00 + 0x40);
    phase -= *(real *)(definition + 0xc) / (step < 0.0001f ? 0.0001f : step) * relative.position.z;
    real crossing = (real)floor((double)phase);
    phase -= crossing;
    *(real *)(state->field_00 + 0x40) = phase;
    long *ring = (long *)(state->field_00 + 0x38);
    *ring -= (long)crossing;
    while (*ring < 0) *ring += 4;
    while (*ring >= 4) *ring -= 4;
    if (crossing < 0.0f) filter_random_pair(points + ((*ring + 3) % 4));
    else if (crossing > 0.0f) filter_random_pair(points + *ring);
    if (phase < 0.0f || phase >= 1.0f) return;
    real offsets[4] = { 0.0f, 0.7135000228881836f, 0.34220001101493835f, 0.579800009727478f };
    volatile long bitmap_count = *(long *)(bitmap + 0x44);
    for (long i = 0; i < 4; ++i)
    {
        if (*(real *)(definition + 0x28) > 0.0f)
        {
            double value = bitmap_count * (double)offsets[i] +
                (double)state->previous_time / *(real *)(definition + 0x28);
            real rounded = (real)value;
            records[i].bitmap = (long)floor(value) % bitmap_count;
            records[i].fraction = (real)(rounded - floor(value) < 0.0f ? 0.0f :
                rounded - floor(value) > 1.0f ? 1.0f : rounded - floor(value));
            if (records[i].bitmap < 0)
            {
                if (!g_55e6c4) g_55e6c4 = 1;
                records[i].bitmap = 0;
            }
        }
        else
        {
            records[i].bitmap = i % bitmap_count;
            records[i].fraction = 0.0f;
        }
    }
    for (long j = 0; j < 4; ++j)
    {
        volatile long index = (*ring + j) % 4;
        real distance = (j + phase) * step + g_4857dc;
        real fraction = (distance - g_4857dc) * inverse_span;
        real weight = (real)(pow(1.0 - pow((double)fabs(fraction * 2.0f - 1.0f), 3.0), 2.0) * g_4b9d98);
        real size = *(real *)(definition + 0x10) / (g_4857e0 < 0.0001f ? 0.0001f : g_4857e0) * distance;
        real x = 0.0f, y = 0.0f, z = 0.0f - distance;
        if (relative.scale != 1.0f) { x *= relative.scale; y *= relative.scale; z *= relative.scale; }
        real dx = relative.up.i * z + relative.left.i * y + relative.forward.i * x + relative.position.x;
        real dy = relative.up.j * z + relative.left.j * y + relative.forward.j * x + relative.position.y;
        points[index].x -= (dy * sine * g_4b9d94 + dx * cosine) * *(real *)(definition + 8) * radius * 0.5f;
        points[index].y -= (dy * cosine * g_4b9d94 - dx * sine) * *(real *)(definition + 8) * radius * 0.5f;
        real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
        if (*(real *)(definition + 0x28) != 0.0f)
        {
            real t = records[index].fraction;
            real values[3];
            values[0] = (1.0f - t) * (1.0f - t) * weight;
            values[1] = t * 2.0f * (1.0f - t) * weight;
            values[2] = t * t * weight;
            for (long k = 0; k < 3; ++k)
                records[index].weights[k] = values[k] < 0.0f ? 0.0f : values[k] > 1.0f ? 1.0f : values[k];
        }
        else
        {
            records[index].weights[0] = 0.0f;
            records[index].weights[1] = weight < 0.0f ? 0.0f : weight > 1.0f ? 1.0f : weight;
            records[index].weights[2] = 0.0f;
        }
        records[index].weights[3] = bounded;
        records[index].coordinates[0] = size * cosine * 0.5f;
        records[index].coordinates[1] = size * sine * aspect * 0.5f;
        records[index].coordinates[2] = 0.0f;
        records[index].coordinates[3] = points[index].x;
        records[index].coordinates[4] = size * sine * -0.5f;
        records[index].coordinates[5] = size * cosine * aspect * 0.5f;
        records[index].coordinates[6] = 0.0f;
        records[index].coordinates[7] = points[index].y;
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
extern byte g_485b48[0x1fc0], g_51f0f0[0x2d8], g_4670bc;
extern byte *g_485a80;
extern long g_4858b8;
struct s_render_reset_state;
struct s_shader_cache;
void function_16b10(s_render_reset_state *);
void function_1c590(s_shader_cache *, long, long);
void function_12fa0(real const *, byte const *, bool, long);
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
    function_12fa0(g_48568c, (byte *)&g_485618, false, g_4858b8);
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
    D3DDevice_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    D3DDevice_SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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

