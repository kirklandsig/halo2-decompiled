// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "geometry_cache.h"
#include "unknown_234c64.h"

#if 0
struct s_interface_function_context
{
    long index;
    real (__stdcall *evaluate)(long context, long name);
    long field_8;
    byte unknown0c[4];
    vector3f field_10;
    vector3f field_1c;
    real field_28, field_2c;
    vector3f field_30;
    vector3f field_3c;
    vector3f field_48;
    vector3f field_54;
    short field_60;
    byte unknown62[0x12];
    bool field_74;
    byte unknown75;
    bool field_76, field_77;
};

// Retail initializer at 0x2c3b0; activation changes shared caller conventions.
inline void function_2c3b0(s_interface_function_context *context)
{
    s_interface_function_context volatile *initialization = context;
    initialization->field_74 = false;
    initialization->evaluate = NULL;
    initialization->field_77 = false;
    initialization->field_76 = false;
    initialization->index = NONE;
    initialization->field_8 = NONE;
    initialization->field_10.i = 0.09f;
    initialization->field_48.i = 0.09f;
    initialization->field_10.j = 0.09f;
    initialization->field_48.j = 0.09f;
    initialization->field_10.k = 0.09f;
    initialization->field_48.k = 0.09f;
    initialization->field_30.i = 0.12f;
    initialization->field_30.j = 0.12f;
    initialization->field_30.k = 0.12f;
    initialization->field_28 = 0.4f;
    initialization->field_2c = 0.4f;
    double pitch = -1.2566370964050293;
    double yaw = 0.0;
    context->field_3c.i = (real)(cos(yaw) * cos(pitch));
    context->field_3c.j = (real)(sin(yaw) * cos(pitch));
    context->field_3c.k = (real)sin(pitch);
    vector3f volatile const *direction = &context->field_3c;
    context->field_54.i = 0.0f - direction->i;
    context->field_54.j = 0.0f - direction->j;
    context->field_54.k = 0.0f - direction->k;
    context->field_1c = context->field_3c;
    context->field_60 = NONE;
}

#endif

struct s_render_view_2b790
{
	long mode, player_index, object_index;
	point3f position;
	vector3f forward, up;
	byte unknown30[4];
	real scale;
	byte unknown38[0x4c - 0x38];
	real near_distance, far_distance;
	byte unknown54[0x80 - 0x54];
	byte camera[0x74];
	byte lighting[0x24];
};
struct s_2f970_view;
bool function_2f970(s_2f970_view const *view, real *out);
bool function_2b720(point3f *point, long *cluster, long *leaf);
void cluster_get_sky(long cluster_index, long *sky_index, bool *found, vector3f *vector);
void function_12e5e0(byte *out, long cluster_index, point3f const *position,
	vector3f const *forward, byte kind, bool enabled);
struct s_speed_result
{
	long type;
	real amount;
	s_speed_shake shake;
};
void function_154310(long index, s_speed_result *result);
void function_2ba10(s_2f970_view const *view, long cluster, long player,
	byte const *camera, bool enabled, long leaf, bool invalid, vector3f const *sky_color,
	long unknown0, long object, long object_index, byte kind, long sky_index,
	byte const *geometry, bool unknown1, long unknown2, byte const *lighting,
	long unknown3, s_speed_result const *speed);
struct s_33a0b_default
{
	dword unknown0;
	vector3f vector;
};
extern s_33a0b_default *g_4686d4;
extern long g_4ba050, g_4ba054[4], g_4ba064[4];

// @retail 0x2b790
void function_2b790(s_render_view_2b790 *view, bool enabled)
{
	s_speed_result speed;
	memcpy(&speed.shake, g_4686d4, sizeof(speed.shake));
	speed.type = 0;
	speed.amount = 0.0f;
	long object_index = NONE;
	g_4ba050 = view->player_index;
	long cluster = g_4ba054[view->player_index];
	long leaf = g_4ba064[view->player_index];
	bool invalid;
	if (function_2b720(&view->position, &cluster, &leaf))
	{
		g_4ba054[view->player_index] = cluster;
		g_4ba064[view->player_index] = leaf;
		invalid = false;
	}
	else invalid = true;
	union { vector3f color; real bounds[4]; } sky;
	sky.color = *(vector3f const *)&g_4686cc->red;
	bool found = false;
	long sky_index;
	cluster_get_sky(cluster, &sky_index, &found, &sky.color);
	byte kind = 0;
	if (cluster != NONE)
	{
		short index = (short)sky_index;
		byte *scenario = (byte *)g_4e0350;
		if (index >= 0 && index < *(long *)(scenario + 8))
		{
			long tag = *(long *)(*(byte **)(scenario + 0xc) + index * 8 + 4);
			if (tag != NONE)
			{
				byte *definition = g_4e3b44[tag & 0xffff].bytes;
				kind = definition && *(long *)(definition + 4) != NONE;
			}
		}
	}
	byte geometry[0x120];
	function_12e5e0(geometry, cluster, &view->position, &view->forward, kind, true);
	if (found) *(vector3f *)(geometry + 4) = sky.color;
	if (geometry[0x18])
	{
		real distance = *(real *)(geometry + 0x14);
		if (view->far_distance > distance && distance > view->near_distance)
			view->far_distance = distance;
	}
	function_2f970((s_2f970_view const *)&view->position, sky.bounds);
	if (view->object_index != NONE)
	{
		long player = g_4e8c20->entries[view->object_index];
		object_index = *(long *)((byte *)g_4e8c24->data + (player & 0xffff) * 0x21c + 0x24);
		function_154310(object_index, &speed);
	}
	if (view->scale > 0.0f)
		function_2ba10((s_2f970_view const *)&view->position, cluster, view->player_index,
			view->camera, enabled, leaf, invalid, &sky.color, 0, view->object_index,
			object_index, kind, sky_index, geometry, false, NONE, view->lighting, 0, &speed);
}

bool function_256f0(transform4x3f *out);
long function_25960(void);
void __stdcall function_352e0(long target, bool multiple);
extern long g_4b9970[12];
extern byte g_5093fc;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
s_render_view_2b790 g_55eed8;
dword g_55eff0;

// @retail 0x25820
bool function_25820(s_render_view_2b790 const *source)
{
	bool result = false;
	transform4x3f matrix;
	if (!source->mode && function_256f0(&matrix))
	{
		result = true;
		if (!(g_55eff0 & 1))
		{
			g_55eff0 |= 1;
			g_55eed8 = *source;
		}
		g_55eed8.position = matrix.position;
		g_55eed8.forward = matrix.forward;
		g_55eed8.up = matrix.up;
		g_55eed8.object_index = NONE;
		union { dword bits; real value; } caption;
		caption.bits = g_4b9970[10];
		if (caption.value != 0.0f) g_55eed8.scale = caption.value * 0.5f;
		s_render_view_2b790 view = g_55eed8;
		memcpy(view.camera, &view.position, sizeof(view.camera));
		memcpy(g_55eed8.camera, view.camera, sizeof(view.camera));
		function_2b790(&view, true);
		g_55eed8.far_distance = view.far_distance;
		if (!g_5093fc)
		{
			long target = function_25960();
			if (target != NONE) function_352e0(target, true);
		}
	}
	if (!g_510c50 || !((byte *)g_510c50)[5])
	{
		memset(g_4b9970, 0, sizeof(g_4b9970));
		g_4b9970[0] = NONE;
		g_5093fc = false;
	}
	return result;
}

struct s_matrix_workspace { long size; byte data[0x27000]; };
extern s_matrix_workspace g_487b18;

#if 0 // Activation must preserve all matched callers and callees.
struct s_223240;
struct s_frame_parameters_12a50
{
    long mode;
    dword identifier;
    double time;
    dword unknown10, unknown14;
};
extern double g_4ba040;
extern long g_4ba048, g_4ba04c, g_4ba030, g_5234c4;
extern dword g_4ba034, g_4c56c0[64];
extern vector3f g_4b9dac;
long g_4aeb1c, g_4aeb28;
real g_4aeb4c;
vector3f g_485b14;
void function_12a50(s_frame_parameters_12a50 const *parameters);
void function_13b90(void);
void function_13cd0(void);
void function_020720(long player);
void __stdcall function_2c790(byte const *source);
void function_146de0(void);
void function_146b80(void);

// Disabled retail 0x2b5d0; activation regresses an opening match.
void function_2b5d0(long count, long mode, long players, long layout, s_223240 const *source)
{
    long const *players_reference = &players;
    long const *layout_reference = &layout;
    s_223240 const *const *views_reference = &source;
    bool restore = g_47989c != 0;
    if (restore) function_146de0();
    __declspec(align(8)) s_frame_parameters_12a50 parameters = {};
    ++g_4ba034;
    g_4ba04c = *players_reference;
    g_4ba048 = *layout_reference;
    g_487b18.size = 0;
    g_4aeb1c = 0;
    g_4aeb28 = 0;
    memset(g_4c56c0, 0, 16 * sizeof(dword));
    g_4ba030 = mode;
    g_4aeb4c = 98304.0f;
    parameters.mode = mode;
    parameters.time = g_4ba040;
    function_12a50(&parameters);
    *(long *)g_54d598.unknown00 = NONE;
    if (count > 0)
    {
        s_render_view_2b790 *views = (s_render_view_2b790 *)*views_reference;
        do
        {
        s_render_view_2b790 *view = views;
        if (!view->mode)
        {
            if (!function_25820(view) || !g_5093fc)
            {
                function_020720(view->player_index);
                function_2b790(view, false);
                g_5234c4 = NONE;
                g_485b14 = g_4b9dac;
            }
        }
        else function_2c790((byte const *)view);
        ++views;
    }
        while (--count);
    }
    function_13b90();
    function_13cd0();
    if (restore) function_146b80();
    g_4ba030 = 0;
}
#endif

struct s_23600_section
{
	byte unknown00[0x34];
	byte *data;
	s_geometry_block_info block;
};

struct s_23600_definition
{
	byte unknown00[0x28];
	s_23600_section *sections;
};

struct s_23600_object_header
{
	byte unknown00[8];
	byte *object;
};

// @retail 0x23600
void *function_23600(long tag, long object_index, byte const *indices, long section_index, long node_index)
{
	void *result = NULL;
	(void)&indices;
	(void)&section_index;
	(void)&node_index;
	if (tag != NONE)
	{
		s_23600_definition *definition = (s_23600_definition *)g_4e3b44[tag & 0xffff].bytes;
		s_23600_section *section = &definition->sections[indices[section_index]];
		if (function_12de70(&section->block, 3))
		{
			byte *object = ((s_23600_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			byte *nodes = *(byte **)(section->data + 0x68);
			result = object + *(short *)(object + 0x116) + 0x28 + nodes[node_index] * 0x34;
		}
	}
	return result;
}

byte g_43f220[16] = { 0, 1, 4, 5, 16, 17, 20, 21, 64, 65, 68, 69, 80, 81, 84, 85 };

struct s_bucket_source
{
	byte unknown0[6];
	struct
	{
		word value;
		word unknown2;
	} entries[1];
};

/* one entry of the 0x68 byte table at 0x4b89b0 */
struct s_table_entry
{
	long key;
	long unknown4;
	long data_handle;
	long data_offset[16];
	byte data_count[16];
	vector3f scale;
};

struct s_surface_size
{
	short unknown0;
	short width;
	short count;
};

struct s_table_entry g_4b89b0[24];
long g_5093e8;
real g_5093ec, g_5093f0;
extern double g_4858a0;
extern bool g_5093f4;
extern real g_4b8940[9], g_4b8964[9], g_4b8988[9];
void function_022750(real const *a, long size, real const *b, real *out, real t);
real *function_143600(real const *rotation, unsigned long order, real const *coefficients, real *result);

// @retail 0x22850
void function_22850(void)
{
    for (long i = 0; i < 24; ++i)
    {
        s_table_entry *entry = &g_4b89b0[i];
        entry->key = NONE;
        *(short *)&entry->unknown4 = NONE;
        entry->data_handle = NONE;
        memset(entry->data_offset, 0, sizeof(entry->data_offset));
        memset(entry->data_count, 0, sizeof(entry->data_count));
    }
    g_5093e8 = 0;
    byte const *structure = (byte *)g_4e0344;
    if (!structure || *(long const *)(structure + 0x80) <= 0 || !g_4e0348) return;
    byte *source = *(byte **)(structure + 0x84);
    if (*(long *)(source + 0x1c) == NONE || *(long *)(source + 4) != *(long *)((byte *)g_4e0348 + 8)) return;
    long count = *(long *)(source + 0x38);
    if (count <= 0) return;
    byte *samples = *(byte **)(source + 0x3c);
    for (long i = 0, remaining = count; remaining > 0; ++i, --remaining)
    {
        byte *sample = samples + i * 0xdc;
        real angle = *(real *)(sample + 0xa0);
        if (angle != 0.0f)
        {
            vector3f axis = *(vector3f *)(sample + 0x94);
            real sine = (real)sin(angle), cosine = (real)cos(angle), complement = 1.0f - cosine;
            real rotation[9];
            rotation[0] = (1.0f - axis.i * axis.i) * cosine + axis.i * axis.i;
            rotation[1] = axis.j * complement * axis.i + axis.k * sine;
            rotation[2] = axis.k * complement * axis.i - axis.j * sine;
            rotation[3] = axis.j * complement * axis.i - axis.k * sine;
            rotation[4] = (1.0f - axis.j * axis.j) * cosine + axis.j * axis.j;
            rotation[5] = axis.j * axis.k * complement + axis.i * sine;
            rotation[6] = axis.k * complement * axis.i + axis.j * sine;
            rotation[7] = axis.j * axis.k * complement - axis.i * sine;
            rotation[8] = (1.0f - axis.k * axis.k) * cosine + axis.k * axis.k;
            for (long color = 0; color < 3; ++color)
            {
                real result[9];
                real *coefficients = (real *)(sample + 0xc + color * 0x24);
                function_143600(rotation, 3, coefficients, result);
                memcpy(coefficients, result, sizeof(result));
            }
        }
    }
    real period = *(real const *)(structure + 0x34);
    if (period <= 0.0f)
    {
        g_5093f4 = false;
        return;
    }
    real previous = g_5093ec;
    g_5093ec = (real)g_4858a0;
    real phase = (g_5093ec - previous) / period + g_5093f0;
    phase -= (long)phase;
    g_5093f4 = true;
    if (phase < 0.0f) phase = 0.0f;
    g_5093f0 = phase;
    if (count == 1)
    {
        memcpy(g_4b8940, samples + 0xc, sizeof(g_4b8940));
        memcpy(g_4b8964, samples + 0x30, sizeof(g_4b8964));
        memcpy(g_4b8988, samples + 0x54, sizeof(g_4b8988));
    }
    else
    {
        real step = 1.0f / (count - 1);
        real inverse = 1.0f / step;
        long index = (long)(phase * inverse);
        real fraction = (phase - index * step) * inverse;
        byte *sample = samples + index * 0xdc;
        function_022750((real *)(sample + 0xe8), 3, (real *)(sample + 0xc), g_4b8940, fraction);
        function_022750((real *)(sample + 0x10c), 3, (real *)(sample + 0x30), g_4b8964, fraction);
        function_022750((real *)(sample + 0x130), 3, (real *)(sample + 0x54), g_4b8988, fraction);
    }
}

struct s_487b10_arena
{
	byte initialized;
	byte field_01[7];
	long field_08;
	byte data[0x27000];
	long count;
	byte active;
	byte field_27011[3];
	long field_27014;
	long field_27018;
	long field_2701c;
	byte field_27020[0x20];
	long record_count;
	byte records[0x6000];
};

s_487b10_arena g_487b10;

// @retail 0x1d660
void function_01d660(void)
{
	g_487b10.field_08 = 0;
	g_487b10.count = 0;
	memset(g_487b10.data, 0, sizeof(g_487b10.data));
	g_487b10.active = false;
	g_487b10.field_27014 = 0;
	g_487b10.field_27018 = 0;
	g_487b10.field_2701c = 0;
	g_487b10.record_count = 0;
	memset(g_487b10.records, 0, sizeof(g_487b10.records));
	g_487b10.initialized = true;
}

// @retail 0x23540
long function_023540(
	short count,
	s_bucket_source const *source,
	s_table_entry *entry,
	long multiplier)
{
	long total = 0;
	for (long i = 0; i < count; i++)
	{
		entry->data_offset[i] = total * multiplier;
		long value = source->entries[i].value;
		if (value > 2)
			value = 2;
		total += value;
		entry->data_count[i] = (byte)value;
	}
	return total;
}

// @retail 0x235a0
void function_0235a0(
	real const *source,
	transform4x3f *matrix)
{
	real *m = (real *)matrix;
	m[1] = source[0];
	m[4] = source[1];
	m[7] = source[2];
	m[10] = source[3];
	m[2] = source[4];
	m[5] = source[5];
	m[8] = source[6];
	m[11] = source[7];
	m[3] = source[8];
	m[6] = source[9];
	m[9] = source[10];
	m[12] = source[11];
	m[0] = 1.0f;
}

// @retail 0x24550
real distance_sq3f(
	point3f const *a,
	point3f const *b)
{
	vector3f v;
	v.i = b->x - a->x;
	v.j = b->y - a->y;
	v.k = b->z - a->z;
	real sum = v.k * v.k;
	sum += v.i * v.i;
	sum += v.j * v.j;
	return sum;
}

// @retail 0x24590
real magnitude3d(
	vector3f const *v)
{
	return (real)sqrt(v->i*v->i + v->j*v->j + v->k*v->k);
}

// @retail 0x24e50
dword function_024e50(
	word a,
	word b)
{
	return
		((((((((g_43f220[a >> 12] << 1 | g_43f220[b >> 12]) << 7 | g_43f220[(a >> 8) & 0xf]) << 1 | g_43f220[(b >> 8) & 0xf]) << 7 | g_43f220[(a >> 4) & 0xf]) << 1 | g_43f220[(b >> 4) & 0xf]) << 7 | g_43f220[a & 0xf]) << 1) | g_43f220[b & 0xf]);
}

__inline void histogram_add(
	real *histogram,
	byte index)
{
	histogram[index] += 1.0f;
}

// @retail 0x24a70
void function_024a70(
	real *histogram,
	dword const *pixels,
	word stride)
{
	memset(histogram, 0, 256 * sizeof(real));
	for (long y = 0; y < 48; y++)
	{
		for (long x = 0; x < 64; x++)
		{
			histogram_add(histogram, (byte)*pixels);
			pixels++;
		}
		pixels += stride - 64;
	}
}

// @retail 0x24b80
void function_024b80(
	real *histogram)
{
	real total = 0.0f;
	for (long i = 0; i < 256; i++)
		total += histogram[i];
	if (total != 0.0f)
	{
		real inverse = 1.0f / total;
		for (long i = 0; i < 256; i++)
			histogram[i] *= inverse;
	}
}

/* the depth terms of function_350e0, inlined with x == 1 */
__inline void clip_depth_terms(
	real *out,
	real x)
{
	real scale;
	real v;
	real w;

	scale = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
	v = (scale * x - g_485ad4.lo * scale) / x * 16777215.0f;
	if (0.0f > v)
		v = 0.0f;
	else if (v > 16777215.0f)
		v = 16777215.0f;
	out[2] = v;

	w = x / g_485ad4.hi;
	w = w * 16777215.0f;
	if (0.0f > w)
		w = 0.0f;
	else if (w > 16777215.0f)
		w = 16777215.0f;
	out[3] = w;
}

// @retail 0x24860
bool __stdcall function_024860(
	long mode,
	real const *bounds,
	real const *t,
	real *out,
	long unused)
{
	switch (mode)
	{
		case 0:
			out[0] = t[0] * 64.0f;
			out[1] = t[1] * 48.0f;
			clip_depth_terms(out, 1.0f);
			return true;
		case 1:
			out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
			out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
			out[2] = 0.0f;
			out[3] = 1.0f;
			return true;
		default:
			return false;
	}
}

// @retail 0x254c0
bool __stdcall function_0254c0(
	long mode,
	real const *bounds,
	real const *t,
	real *out,
	long unused)
{
	switch (mode)
	{
		case 0:
			out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
			out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
			clip_depth_terms(out, 1.0f);
			return true;
		case 1:
			out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
			out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
			out[2] = 0.0f;
			out[3] = 1.0f;
			return true;
		default:
			return false;
	}
}

real g_509400;
extern real g_485ae0;

real g_48565c;
real g_485778, g_48577c, g_485790, g_485794;
real g_4b9c74[5][14];
real g_4b9d90, g_4b9d94;
void function_350e0(real *out, real const *a, real const *b, real x);

// @retail 0x26710
bool __stdcall function_26710(long mode, real const *bounds, real const *t, real *out, long unused)
{
	(void)&mode;
	(void)&bounds;
	(void)&t;
	(void)&out;
	(void)&unused;
	if (g_509400 <= 0.0f)
		g_509400 = g_485ae0;
	switch (mode)
	{
	case 0:
		function_350e0(out, bounds, t, g_509400);
		return true;
	case 1: case 4:
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		out[2] = 0.0f;
		out[3] = 1.0f;
		return true;
	case 2:
		{
			real span = g_48577c - g_485778;
			real scale = 1.0f / (0.0001f > span ? 0.0001f : span);
			out[0] = g_48565c * scale;
			out[1] = 0.0f;
			out[2] = 0.0f - g_485778 * scale;
			out[3] = 0.0f;
			return true;
		}
	case 3:
		{
			real span = g_485794 - g_485790;
			real scale = 1.0f / (0.0001f > span ? 0.0001f : span);
			out[0] = g_48565c * scale;
			out[1] = 0.0f;
			out[2] = 0.0f - g_485790 * scale;
			out[3] = 0.0f;
			return true;
		}
	default: return false;
	}
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x26ca0
bool __stdcall function_26ca0(long mode, real const *bounds, real const *t, real *out, long unused)
{
	(void)&mode;
	(void)&bounds;
	(void)&t;
	(void)&out;
	(void)&unused;
	real const *matrix = g_4b9c74[mode];
	if (g_509400 <= 0.0f)
		g_509400 = g_485ae0;
	if (mode > 0 && mode <= 4)
	{
		out[0] = (t[0] * 2.0f - 1.0f) * matrix[6] - (t[1] * 2.0f - 1.0f) * matrix[7] * g_4b9d94 + matrix[9];
        _ReadWriteBarrier();
		out[1] = (t[0] * 2.0f - 1.0f) * matrix[10] - (t[1] * 2.0f - 1.0f) * matrix[11] * g_4b9d94 + matrix[13];
		out[2] = 0.0f;
		out[3] = 1.0f;
		return true;
	}
	if (mode == 0)
	{
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		clip_depth_terms(out, 1.0f);
        _ReadWriteBarrier();
		out[0] *= g_4b9d90;
        _ReadWriteBarrier();
		out[1] *= g_4b9d90;
		return true;
	}
	return false;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x27960
bool __stdcall function_27960(long mode, real const *bounds, real const *t, real *out, long unused)
{
	(void)&mode;
	(void)&bounds;
	(void)&t;
	(void)&out;
	(void)&unused;
	if (g_509400 <= 0.0f)
		g_509400 = g_485ae0;
	switch (mode)
	{
	case 0:
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		{
		real x = g_509400;
		real scale = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
		real depth = scale * x;
		depth -= g_485ad4.lo * scale;
		depth = depth / x * 16777215.0f;
		if (0.0f > depth) depth = 0.0f;
		else if (depth > 16777215.0f) depth = 16777215.0f;
		out[2] = depth;
		real reciprocal = x / g_485ad4.hi * 16777215.0f;
		if (0.0f > reciprocal) reciprocal = 0.0f;
		else if (reciprocal > 16777215.0f) reciprocal = 16777215.0f;
		out[3] = reciprocal;
		}
		return true;
	case 1:
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		out[2] = 0.0f;
		out[3] = 1.0f;
		return true;
	default:
		return false;
	}
}

/* the table entry whose key matches, or NONE */
__inline long find_table_entry(
	long key)
{
	short i;
	for (i = 0; i < g_5093e8; i++)
	{
		if (g_4b89b0[i].key == key)
			return i;
	}
	return NONE;
}

struct s_24490_entry
{
	long key;
	long count;
};

struct s_24490_group
{
	long unknown00;
	long count;
	s_24490_entry *entries;
};

struct s_24490_definition
{
	byte unknown00[0x14];
	long count;
	s_24490_group *groups;
	byte unknown1c[0x14];
	byte *data;
	s_geometry_block_info block;
};

// @retail 0x24490
void *function_24490(long key, s_24490_definition *definition, long entry_key)
{
	long index = find_table_entry(key);
	if (index == NONE)
		return NULL;
	long maximum = definition->count - 1;
	long group = 5 - (short)g_4b89b0[index].unknown4;
	if (group >= maximum)
		group = maximum;
	long entry_count = definition->groups[group].count;
	long i = 0;
	if (entry_count > 0)
	{
		s_24490_entry *current = definition->groups[group].entries;
		do
		{
		if (current->key == entry_key)
		{
			long count = current->count / 5;
			if (!function_12de70(&definition->block, 3))
				return NULL;
			byte *data = definition->data;
			long size = data[0] == 0x39 ? count * 10 : count * 20;
			*(long *)(data + 4) = size;
			*(long *)(data + 8) = size;
			return data;
		}
	
			++i;
			++current;
		} while (i < entry_count);
	}
	return NULL;
}

// @retail 0x1d6b0
__inline real *table_entry_data(
	long handle)
{
	byte *base = NULL;
	if (handle != NONE && !(handle & 0x80000000))
		base = g_487b10.data + (handle & 0xfffffff);
	return (real *)base;
}

// @retail 0x24040
void function_024040(
	long key,
	s_surface_size const *size,
	long index)
{
	long entry_index = find_table_entry(key);
	if (entry_index == NONE)
		return;

	real *data = table_entry_data(g_4b89b0[entry_index].data_handle) + g_4b89b0[entry_index].data_offset[index];
	long total = (size->count * 3 + 4) * size->width;
	byte stored = g_4b89b0[entry_index].data_count[index];
	long count;
	if (stored > 2)
		count = 2;
	else
	{
		count = stored;
		if (count <= 0)
			return;
	}

	long vectors = (count * total) / 4;
	real constants[4] = { 1.0f, 4.0f, 10200.5f, 0.0039254902f };

	D3DDevice_SetVertexShaderConstant(-28, &g_4b89b0[entry_index].scale, 1);
	D3DDevice_SetVertexShaderConstant(-27, constants, 1);
	D3DDevice_SetVertexShaderConstant(-26, data, vectors);
}

/* a sample location: a matrix index (stored as a real) and four weights */
struct s_sample_point
{
	real index;
	real weight[4];
};

// @retail 0x241c0
bool function_0241c0(
	long key,
	long block,
	vector3f *scale_out,
	long index,
	s_sample_point const *point0,
	s_sample_point const *point1,
	s_sample_point const *point2,
	real fraction0,
	real fraction1,
	vector3f *result)
{
	long entry_index = find_table_entry(key);
	if (entry_index != NONE)
	{
		*scale_out = g_4b89b0[entry_index].scale;
		scale_out->i *= 0.8f;
		scale_out->j *= 0.8f;
		scale_out->k *= 0.8f;

		real const *data = table_entry_data(g_4b89b0[entry_index].data_handle) + g_4b89b0[entry_index].data_offset[index];
		if (block > g_4b89b0[entry_index].data_count[index] - 1)
			block = 0;
		data += block * 160;

		s_sample_point const *points[3] = { point0, point1, point2 };
		real corner[3][3];
		long i = 0;
		do
		{
			s_sample_point const *point = points[i];
			real const *m = data + (long)point->index * 16;
			real x = m[6] * point->weight[2];
			x += m[4] * point->weight[0];
			x += m[7] * point->weight[3];
			x += m[5] * point->weight[1];
			x += m[0];
			corner[i][0] = x;
			real y = m[10] * point->weight[2];
			y += m[8] * point->weight[0];
			y += m[11] * point->weight[3];
			y += m[9] * point->weight[1];
			y += m[1];
			corner[i][1] = y;
			real z = m[14] * point->weight[2];
			z += m[12] * point->weight[0];
			z += m[15] * point->weight[3];
			z += m[13] * point->weight[1];
			z += m[2];
			corner[i][2] = z;
			i++;
		}
		while (i < 3);

		for (long k = 0; k < 3; k++)
		{
			real v = (corner[1][k] - corner[0][k]) * fraction0 + (corner[2][k] - corner[0][k]) * fraction1 + corner[0][k];
			real r = v;
			if (v > 1.0f)
				r = 1.0f;
			if (0.0f > v)
				r = 0.0f;
			result->n[k] = r;
		}
		return true;
	}
	return false;
}

struct s_block_layout
{
	short size;
	short groups;
	short columns;
};

/* dot product of two 3x3 blocks stored as nine reals */
#define DOT_PRODUCT9(result, a, b) \
	__asm mov ecx, a \
	__asm mov edx, b \
	__asm movlps xmm0, qword ptr [ecx] \
	__asm movlps xmm1, qword ptr [edx] \
	__asm movlps xmm2, qword ptr [ecx + 0x10] \
	__asm movlps xmm3, qword ptr [edx + 0x10] \
	__asm movhps xmm0, qword ptr [ecx + 8] \
	__asm movhps xmm1, qword ptr [edx + 8] \
	__asm movhps xmm2, qword ptr [ecx + 0x18] \
	__asm movhps xmm3, qword ptr [edx + 0x18] \
	__asm mulps xmm0, xmm1 \
	__asm mulps xmm2, xmm3 \
	__asm addps xmm0, xmm2 \
	__asm movss xmm1, dword ptr [ecx + 0x20] \
	__asm movhlps xmm2, xmm0 \
	__asm mulss xmm1, dword ptr [edx + 0x20] \
	__asm addps xmm0, xmm2 \
	__asm addss xmm1, xmm0 \
	__asm shufps xmm0, xmm0, 0x55 \
	__asm addss xmm0, xmm1 \
	__asm movss result, xmm0

// @retail 0x23ca0
void function_023ca0(
	s_block_layout const *layout,
	real const *data,
	real const *vector0,
	real const *vector1,
	real const *vector2,
	real *out)
{
	long block_size = layout->size * layout->size;
	long group_size = block_size * 3;
	long stride = (layout->columns + 1) * group_size;
	real const *block0 = data;
	real const *block1 = data + block_size;
	real const *block2 = data + block_size * 2;
	real const *first_cell0 = data + group_size;
	real const *first_cell1 = data + group_size + block_size;
	real const *first_cell2 = data + group_size + block_size * 2;
	for (long i = 0; i < layout->groups; i++)
	{
		real const *cell0 = first_cell0;
		real const *cell1 = first_cell1;
		real const *cell2 = first_cell2;
		real const *block;
		real dot0;
		real dot1;
		real dot2;
		block = block0;
		DOT_PRODUCT9(dot0, block, vector0);
		block = block1;
		DOT_PRODUCT9(dot1, block, vector1);
		block = block2;
		DOT_PRODUCT9(dot2, block, vector2);
		out[(layout->columns * 3 + 4) * i] = dot0;
		out[(layout->columns * 3 + 4) * i + 1] = dot1;
		out[(layout->columns * 3 + 4) * i + 2] = dot2;
		real *row = out + (layout->columns * 3 + 4) * i + 4;
		for (long j = 0; j < layout->columns; j++)
		{
			real const *cell_block;
			real cell_dot0;
			real cell_dot1;
			real cell_dot2;
			cell_block = cell0;
			DOT_PRODUCT9(cell_dot0, cell_block, vector0);
			cell_block = cell1;
			DOT_PRODUCT9(cell_dot1, cell_block, vector1);
			cell_block = cell2;
			DOT_PRODUCT9(cell_dot2, cell_block, vector2);
			row[j] = cell_dot0;
			row[layout->columns + j] = cell_dot1;
			row[layout->columns * 2 + j] = cell_dot2;
			cell0 += group_size;
			cell1 += group_size;
			cell2 += group_size;
		}
		block0 += stride;
		block1 += stride;
		block2 += stride;
		first_cell0 += stride;
		first_cell1 += stride;
		first_cell2 += stride;
	}
}

struct s_24c70_range
{
    real low, high, average;
    real smoothed_low, smoothed_high, smoothed_average;
};

PRIVATE __forceinline real histogram_blend(real previous, real value)
{
    real result;
    if (previous > value)
        result = previous * (1.0f - 0.16f) + value * 0.16f;
    else
        result = previous * (1.0f - 0.16f) + value * 0.16f;
    return result;
}

// @retail 0x24c70
void function_24c70(real const *histogram, s_24c70_range *range)
{
    real low, high;
    real sum = 0.0f;
    for (long i = 0; i < 256; ++i)
    {
        sum += histogram[i];
        if (sum >= 0.0f)
        {
            low = (real)i * (1.0f / 255.0f);
            break;
        }
    }
    sum = 0.0f;
    for (long j = 255; j >= 0; --j)
    {
        sum += histogram[j];
        if (sum >= 0.01f)
        {
            high = (real)j * (1.0f / 255.0f);
            break;
        }
    }
    sum = 0.0f;
    for (long k = 0; k < 256; ++k)
        sum += (real)k * histogram[k];
    real average = sum * (1.0f / 255.0f);
    range->low = low;
    range->high = high;
    range->average = average;
    range->smoothed_low = histogram_blend(range->smoothed_low, low);
    range->smoothed_high = histogram_blend(range->smoothed_high, high);
    range->smoothed_average = range->smoothed_average * 0.8f + average * 0.2f;
    range->smoothed_low = range->smoothed_low < 0.0f ? 0.0f : range->smoothed_low > 0.0f ? 0.0f : range->smoothed_low;
    range->smoothed_high = range->smoothed_high < 0.7f ? 0.7f : range->smoothed_high > 1.0f ? 1.0f : range->smoothed_high;
    range->smoothed_average = range->smoothed_average < range->smoothed_low ? range->smoothed_low :
        range->smoothed_average > range->smoothed_high ? range->smoothed_high : range->smoothed_average;
}

namespace D3D { namespace PixelJar {
    void __stdcall FindSurfaceWithinTexture(D3DPixelContainer *container, D3DCUBEMAP_FACES face, unsigned int level,
        byte **bits, dword *pitch, dword *width, dword *height, dword *slice);
} }

void *function_01dcc0(long index);
struct s_histogram_state
{
    s_24c70_range range;
    real *histogram;
};

struct s_unknown_01dcc0
{
    byte unknown00[4];
    long sub_header[5];
    long elements[4][6];
    long element_count;
    byte unknown7c[4];
    long width, height;
    void *data;
    byte unknown8c[8];
    bool flag94;
    byte flag95;
    byte unknown96[2];
};
extern s_unknown_01dcc0 g_4b4b58[39];

// @retail 0x24970
void __cdecl function_24970(s_histogram_state *state)
{
    (void)&state;
    real *histogram = state->histogram;
    XSaveFloatingPointStateForDpc();
    s_unknown_01dcc0 *entry = g_4b4b58 + 14;
    D3DPixelContainer *texture = NULL;
    if (entry->data && !entry->flag95)
        texture = (D3DPixelContainer *)entry->sub_header;
    byte *bits;
    dword pitch, width, height, slice;
    D3D::PixelJar::FindSurfaceWithinTexture(texture, (D3DCUBEMAP_FACES)0, 0,
        &bits, &pitch, &width, &height, &slice);
    function_024a70(histogram, (dword const *)bits, (word)(pitch >> 2));
    function_024b80(histogram);
    function_24c70(histogram, &state->range);
    XRestoreFloatingPointStateForDpc();
}

bool g_4b9d9c;
real g_4857dc, g_4857e0;

// @retail 0x27520
bool __stdcall function_27520(long mode, real const *bounds, real const *t, real *out, long unused)
{
    (void)&mode; (void)&bounds; (void)&t; (void)&out; (void)&unused;
    if (g_509400 <= 0.0f)
        g_509400 = g_485ae0;
    switch (mode)
    {
    case 0:
        function_350e0(out, bounds, t, g_509400);
        return true;
    case 1:
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        out[2] = 0.0f;
        out[3] = 1.0f;
        return true;
    case 2:
        if (!g_4b9d9c) goto scaled;
    case 3:
        {
            real minimum = 0.0001f;
            real end = g_4857e0;
            real begin = g_4857dc;
            real span = end - begin;
            real const *divisor_reference = minimum > span ? &minimum : &span;
            real divisor = *divisor_reference;
            real scale = 1.0f / divisor;
            out[0] = g_48565c * scale;
            out[1] = 0.0f;
            out[2] = 0.0f - g_4857dc * scale;
            out[3] = 0.0f;
            return true;
        }
    case 4:
scaled:
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        out[2] = 0.0f;
        out[3] = 1.0f;
        out[0] *= g_4b9d90;
        out[1] *= g_4b9d90;
        return true;
    default:
        return false;
    }
}

extern byte g_4b99b0[0x4c];

// @retail 0x298f0
bool __stdcall function_298f0(long mode, real const *bounds, real const *t, real *out, long unused)
{
    (void)&mode; (void)&bounds; (void)&t; (void)&out; (void)&unused;
    if (g_509400 <= 0.0f)
        g_509400 = g_485ae0;
    switch (mode)
    {
    case 0:
        function_350e0(out, bounds, t, g_509400);
        return true;
    case 2:
        if (g_4b99b0[0x41] && g_4b99b0[1])
        {
            out[0] = *(real *)(g_4b99b0 + 0x24);
            out[1] = 0.0f;
            out[2] = t[0] * 640.0f;
            out[3] = 0.0f;
            return true;
        }
    case 3:
        if (g_4b99b0[0x41] && g_4b99b0[1])
        {
            out[0] = 0.0f;
            out[1] = *(real *)(g_4b99b0 + 0x28);
            out[2] = t[1] * 480.0f;
            out[3] = 0.0f;
            return true;
        }
    case 1: case 4:
        {
            out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
            out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
            out[2] = 0.0f;
            out[3] = 1.0f;
            real strength = *(real *)(g_4b99b0 + 0x3c);
            if (strength != 0.0f)
            {
                real fraction = (5 - mode) * 0.25f;
                real x = *(real *)(g_4b99b0 + 0x1c) * strength * fraction + 1.0f;
                real y = *(real *)(g_4b99b0 + 0x20) * strength * fraction + 1.0f;
                real inverse_x = x > 0.0f ? 1.0f / x : 0.0f;
                real inverse_y = y > 0.0f ? 1.0f / y : 0.0f;
                out[0] = (bounds[1] - bounds[0]) * (1.0f - inverse_x) * 0.5f + out[0] * inverse_x;
                out[1] = (bounds[3] - bounds[2]) * (1.0f - inverse_y) * 0.5f + out[1] * inverse_y;
                return true;
            }
            return mode == 1;
        }
    default:
        return false;
    }
}

extern D3DPIXELSHADERDEF g_484f68;
extern long g_4858b8;
extern byte *g_485a80;
extern byte g_51f0f0[0x2d8];
color3f g_4857a8;
real g_4857b4;
void function_14bc0(short index, short element, bool use_depth);
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
bool function_1ccf0(D3DPIXELSHADERDEF const *program);
struct s_shader_cache;
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);
dword __cdecl function_131f40(real alpha, color3f const *color);

// @retail 0x26520
bool __stdcall function_26520(void *context)
{
    g_509400 = g_485ae0;
    if (g_4857b4 > 0.0f)
    {
        function_14bc0((short)g_4858b8, 0, true);
        function_0222d0(D3DRS_COLORWRITEENABLE, 0x1010101);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTCOLOR);
        function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        function_0222d0(D3DRS_CULLMODE, 0);
        function_0222d0(D3DRS_STENCILENABLE, 0);
        function_0222d0(D3DRS_ZENABLE, 2);
        function_0222d0(D3DRS_ZWRITEENABLE, 0);
        function_0222d0(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        function_0222d0(D3DRS_ZBIAS, 0);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSFinalCombinerConstant0 = function_131f40(g_4857b4, &g_4857a8);
        g_484f68.PSFinalCombinerInputsABCD = 0x1200000;
        g_484f68.PSFinalCombinerInputsEFG = 0x1100;
        function_1ccf0(&g_484f68);
        function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
        function_1c710(g_51f0f0);
        return true;
    }
    return false;
}

void function_14f60(short stage, short index);

// @retail 0x2b000
bool __stdcall function_2b000(void *context)
{
    g_509400 = g_485ae0;
    function_14bc0(16, 0, false);
    function_14f60(0, (short)g_4858b8);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSFinalCombinerInputsABCD = 8;
    g_484f68.PSFinalCombinerInputsEFG = 0;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

// @retail 0x296c0
bool __stdcall function_296c0(void *context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1])
        return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    function_14f60(0, *(short *)(g_4b99b0 + 0x34));
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSFinalCombinerInputsABCD = 8;
    g_484f68.PSFinalCombinerInputsEFG = 0;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

color3f g_4857bc;
dword __cdecl pack_color3f(color3f const *color);

// @retail 0x276d0
bool __stdcall function_276d0(void *context)
{
    g_509400 = g_485ae0;
    function_14bc0((short)g_4858b8, 0, false);
    function_14f60(0, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);

    function_0222d0(D3DRS_COLORWRITEENABLE, 0x1010101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTCOLOR);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSAlphaInputs[0] = 0x8200000;
    g_484f68.PSAlphaOutputs[0] = 0xc0;
    g_484f68.PSFinalCombinerConstant0 = pack_color3f(&g_4857bc);
    g_484f68.PSFinalCombinerInputsABCD = 0x1c010000;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

// @retail 0x29080
bool __stdcall function_29080(void *context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1])
        return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    function_14f60(0, g_4b99b0[1] ? *(short *)(g_4b99b0 + 0x38) : 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);

    if (g_4b99b0[1])
    {
        function_14f60(2, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(2, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);

    }
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, g_4b99b0[0]);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_ONE);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_CONSTANTCOLOR);
    color3f blend = *(color3f *)(g_4b99b0 + 0x10);
    function_0222d0(D3DRS_BLENDCOLOR, pack_color3f(&blend));
    function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    color3f color = *(color3f *)(g_4b99b0 + 4);
    if (g_4b99b0[1])
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x2621;
        g_484f68.PSDotMapping = 0x11;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSFinalCombinerConstant0 = pack_color3f(&color);
        g_484f68.PSFinalCombinerInputsABCD = 0xa010000;
    }
    else
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSFinalCombinerConstant0 = pack_color3f(&color);
        g_484f68.PSFinalCombinerInputsABCD = 0x8010000;
    }
    g_484f68.PSFinalCombinerInputsEFG = 0;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

// @retail 0x251b0
bool __stdcall function_251b0(void *context)
{
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    if (!context)
    {
        function_14bc0(1, 0, true);
        function_14f60(0, g_4858b8);
        function_14f60(1, 15);
        function_14f60(2, 15);
        D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(1, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(2, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x41e1;
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSConstant0[0] = 0xff0000;
        g_484f68.PSConstant1[0] = 0xff00;
        g_484f68.PSConstant1[1] = 0xff;
        g_484f68.PSRGBInputs[0] = 0x9010a02;
        g_484f68.PSRGBInputs[1] = 0xc200a02;
        g_484f68.PSRGBOutputs[0] = 0xc00;
        g_484f68.PSRGBOutputs[1] = 0xc00;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
    }
    else
    {
        function_14bc0((word)g_4858b8, 0, true);
        function_14f60(0, 1);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 1;
        g_484f68.PSFinalCombinerInputsABCD = 8;
    }
    g_484f68.PSFinalCombinerInputsEFG = 0x1800;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}


// @retail 0x29370
bool __stdcall function_29370(void *context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1]) return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    { short stage = 0; if (stage < 4) do {
        function_14f60(stage, *(short *)(g_4b99b0 + 0x38));
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
    
++stage;
} while (stage < 4); }
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHATESTENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_ONE);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_CONSTANTCOLOR);
    color3f blend = *(color3f *)(g_4b99b0 + 0x10);
    function_0222d0(D3DRS_BLENDCOLOR, pack_color3f(&blend));
    function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    D3DPIXELSHADERDEF program;
    memset(&program, 0, sizeof(program));
    program.PSRGBOutputs[1] = 0x30d00;
    program.PSRGBOutputs[2] = 0x30d00;
    program.PSTextureModes = 0x8421;
    program.PSCombinerCount = 0x11003;
    program.PSRGBInputs[0] = 0x08200920;
    program.PSRGBOutputs[0] = 0x30c00;
    program.PSRGBInputs[1] = 0x0a200b20;
    program.PSRGBInputs[2] = 0x0c200d20;
    program.PSFinalCombinerInputsABCD = 0xd;
    g_484f68 = program;
    D3DDevice_SetPixelShaderProgram(&program);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}


extern word g_485648, g_48564a, g_48564c, g_48564e;
extern byte g_485607, g_4670bc;
extern byte g_485b48[0x1fc0];
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
void function_1cf50();

typedef bool (__stdcall *t_34770_begin)(long);
typedef bool (__stdcall *t_34770_vertex)(long, real const *, real const *, real *, long);
typedef void (__stdcall *t_34770_end)(long);
bool __stdcall function_0254c0(long mode, real const *bounds, real const *t, real *out, long unused);

// @retail 0x34770
void function_34770(t_34770_begin begin, real const *bounds,
    t_34770_vertex vertex, t_34770_end end, long columns, long rows, long user)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    real default_bounds[4];
    if (!bounds)
    {
        default_bounds[0] = (real)(short)g_48564a;
        default_bounds[1] = (real)(short)g_48564e;
        default_bounds[2] = (real)(short)g_485648;
        default_bounds[3] = (real)(short)g_48564c;
        bounds = default_bounds;
    }
    if (!vertex) vertex = function_0254c0;
    if (!begin || begin(user))
    {
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(3, D3DTSS_ALPHAKILL, 0);
        function_1cf50();
        function_1c710(g_51f0f0);
        for (long row = 0; row < rows; ++row)
        {
            D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
            for (long column = 0; column <= columns; ++column)
            {
                real fraction_x = (real)column / columns;
                for (long side = 0; side < 2; ++side)
                {
                    real fraction_y = (real)(row + side) / rows;
                    for (long attribute = 15; attribute >= 0; --attribute)
                    {
                        real out[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                        real t[2] = { fraction_x, fraction_y };
                        if (vertex(attribute, bounds, t, out, user))
                            D3DDevice_SetVertexData4f(attribute ? attribute : D3DVSDE_VERTEX,
                                out[0], out[1], out[2], out[3]);
                    }
                }
            }
            D3DDevice_End();
        }
    }
    if (end) end(user);
}

// @retail 0x34420
void function_34420(t_34770_begin begin, real const *bounds,
    t_34770_vertex vertex, t_34770_end end, long user)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    real default_bounds[4];
    if (!bounds)
    {
        long target = NONE;
        if (g_4b4b58[20].data && !g_4b4b58[20].flag95)
            target = 20;
        else if (g_4b4b58[18].data && !g_4b4b58[18].flag95)
            target = 18;
        if (g_4858b8 == target && g_485607)
        {
            default_bounds[0] = 0.0f;
            default_bounds[1] = 640.0f;
            default_bounds[2] = 0.0f;
            default_bounds[3] = 480.0f;
        }
        else
        {
            default_bounds[0] = (real)(short)g_48564a;
            default_bounds[1] = (real)(short)g_48564e;
            default_bounds[2] = (real)(short)g_485648;
            default_bounds[3] = (real)(short)g_48564c;
        }
        bounds = default_bounds;
    }
    if (!vertex) vertex = function_0254c0;
    if (!begin || begin(user))
    {
        real corners[8] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(3, D3DTSS_ALPHAKILL, 0);
        function_1cf50();
        function_1c710(g_51f0f0);
        D3DDevice_Begin(D3DPT_TRIANGLEFAN);
        real *t = corners;
        long remaining = 4;
        do
        {
            { long attribute = 15; if (attribute >= 0) do {
                real out[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                if (vertex(attribute, bounds, t, out, user))
                    D3DDevice_SetVertexData4f(attribute ? attribute : D3DVSDE_VERTEX,
                        out[0], out[1], out[2], out[3]);
            
--attribute;
} while (attribute >= 0); }
            t += 2;
        } while (--remaining);
        D3DDevice_End();
    }
    if (end) end(user);
}


point2f g_509408;
void function_1396c7(long mode, point2f *point);

// @retail 0x2b220
bool __stdcall function_2b220(long mode, real const *bounds,
    real const *t, real *out, long unused)
{
    if (g_509400 <= 0.0f) g_509400 = g_485ae0;
    switch (mode)
    {
    case 0:
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        clip_depth_terms(out, g_509400);
        return true;
    case 1:
        {
            real width = (real)((short)g_48564e - (short)g_48564a) * g_509408.x * 0.5f;
            real height = (real)((short)g_48564c - (short)g_485648) * g_509408.y * 0.5f;
            point2f center;
            function_1396c7(4, &center);
            out[0] = (center.x - width) * (1.0f - t[0]) + (center.x + width) * t[0];
            out[1] = (center.y - height) * (1.0f - t[1]) + (center.y + height) * t[1];
            return true;
        }
    default:
        return false;
    }
}

// @retail 0x2afa0
void function_2afa0(point2f const *scale)
{
    real bounds[4];
    g_509408 = *scale;
    bounds[0] = 0.0f;
    bounds[1] = 64.0f;
    bounds[2] = 0.0f;
    bounds[3] = 64.0f;
    function_34420((t_34770_begin)function_2b000, bounds, function_2b220, NULL, 0);
}

bool __stdcall function_246a0(void *context);

// @retail 0x249f0
void function_249f0(void)
{
    real bounds[4];
    bounds[0] = (real)(short)g_48564a;
    bounds[1] = (real)(short)g_48564e;
    bounds[2] = (real)(short)g_485648;
    bounds[3] = (real)(short)g_48564c;
    function_14bc0(14, 0, true);
    function_14f60(0, (short)g_4858b8);
    function_34420((t_34770_begin)function_246a0, bounds, function_024860, NULL, 0);
}

extern byte g_485a75;
extern long g_48574c;
byte g_485860;
extern dword g_4b8344;
bool __stdcall function_25e50(void *context);

// @retail 0x25d50
void function_25d50(void)
{
    if (!g_485a75)
    {
        if (g_48574c == 1 || g_48574c == 4 || g_48574c == 5 || g_485860)
            function_34420((t_34770_begin)function_25e50, NULL, function_26710, NULL, 0);
        else if (g_48574c == 2 || g_48574c == 3)
            function_34420((t_34770_begin)function_26520, NULL, function_26710, NULL, 0);
        g_4b8344 = 0;
        D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
    }
}

long g_4857b8;
real g_4857d8;
byte g_5093fd, g_5093fe;
bool function_143c0(long tag, short index, short stage, real priority);

// @retail 0x26880
bool __stdcall function_26880(void *context)
{
    function_14bc0(19, 0, false);
    byte *definition = g_4e3b44[g_4857b8 & 0xffff].bytes;
    for (long stage = 0; stage < 4; ++stage)
    {
        real index_bits = g_4b9c74[stage + 1][0];
        function_143c0(*(long *)(definition + 0x18), *(short *)&index_bits, (short)stage, 0.0f);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 1);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 1);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 1);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
    }
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x1010101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x8421;
    color3f colors[4];
    for (long i = 0; i < 4; ++i)
    {
        colors[i].red = g_4b9c74[i + 1][2];
        colors[i].green = g_4b9c74[i + 1][3];
        colors[i].blue = g_4b9c74[i + 1][4];
    }
    if (!g_4b9d9c)
    {
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSConstant0[0] = pack_color3f(&colors[0]);
        g_484f68.PSConstant1[0] = pack_color3f(&colors[1]);
        g_484f68.PSConstant0[1] = pack_color3f(&colors[2]);
        g_484f68.PSConstant1[1] = pack_color3f(&colors[3]);
        g_484f68.PSRGBInputs[2] = 0x28292a2b;
        g_484f68.PSRGBOutputs[2] = 0xcd;
        g_484f68.PSRGBInputs[3] = 0x0c0d0000;
        g_484f68.PSRGBOutputs[3] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0x2c;
        g_484f68.PSFinalCombinerInputsEFG = 0x2c00;
    }
    else
    {
        g_484f68.PSCombinerCount = 0x11003;
        g_484f68.PSConstant0[0] = pack_color3f(&colors[0]);
        g_484f68.PSConstant1[0] = pack_color3f(&colors[1]);
        g_484f68.PSConstant0[1] = pack_color3f(&colors[2]);
        g_484f68.PSConstant1[1] = pack_color3f(&colors[3]);
        g_484f68.PSConstant0[2] = 0xff0000;
        g_484f68.PSConstant1[2] = 0xff00;
        g_484f68.PSRGBInputs[2] = 0x9010a02;
        g_484f68.PSRGBOutputs[2] = 0xc00;
        g_484f68.PSConstant1[3] = 0xff;
        g_484f68.PSFinalCombinerInputsABCD = 0xb01000c;
        g_484f68.PSFinalCombinerInputsEFG = 0x800;
    }
    g_484f68.PSRGBInputs[0] = 0x8010902;
    g_484f68.PSRGBOutputs[0] = 0x3089;
    g_484f68.PSRGBInputs[1] = 0xa010b02;
    g_484f68.PSRGBOutputs[1] = 0x30ab;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}
bool __stdcall function_26e50(void *context);

// @retail 0x25dc0
void function_25dc0(bool first, bool second)
{
    if (g_4857b8 != NONE && g_4857b8 != 0 && g_4857d8 > 0.0f)
    {
        g_5093fd = first;
        g_5093fe = second;
        if (first)
        {
            function_34420((t_34770_begin)function_26880, NULL, function_26ca0, NULL, 0);
            function_34420((t_34770_begin)function_26e50, NULL, function_27520, NULL, 0);
        }
        else if (second)
            function_34420((t_34770_begin)function_276d0, NULL, function_27960, NULL, 0);
    }
    g_4b8344 = 0;
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
}

struct s_24ee0_state
{
    s_24c70_range range;
    real *histogram;
    long index;
};
byte g_4b9370[2][3][256];
extern long g_4ba04c;

// @retail 0x24ee0
void function_24ee0(s_24ee0_state const *state, short mode)
{
    (void)&mode;
    D3DTexture *texture = (D3DTexture *)function_01dcc0(15);
    texture->Size = 0;
    texture->Format = 0x06610629;
    volatile long index = state->index;
    if (index < 0) index = 0;
    else if (index > 1) index = 1;
    real low = state->range.smoothed_low;
    real high = state->range.smoothed_high;
    real gamma = 1.0f;
    if (mode)
        gamma += ((state->range.smoothed_average - low) / (high - low)) * 0.8f;
    byte curve[256];
    for (long i = 0; i < 256; ++i)
    {
        real value = (real)i * (1.0f / 255.0f);
        real result;
        if (value <= low) result = 0.0f;
        else if (value >= high) result = 1.0f;
        else
        {
            result = (real)pow(((double)value - low) / ((double)high - low), (double)gamma);
            if (result < 0.0f) result = 0.0f;
            else if (result > 1.0f) result = 1.0f;
        }
        byte mapped = (byte)(long)((double)result * 255.0 + 0.5);
        curve[i] = mapped;
        if (mode == 1 && g_4ba04c == 1)
        {
            g_4b9370[index][2][i] = mapped;
            g_4b9370[index][1][i] = mapped;
            g_4b9370[index][0][i] = mapped;
        }
        else
        {
            g_4b9370[index][2][i] = (byte)i;
            g_4b9370[index][1][i] = (byte)i;
            g_4b9370[index][0][i] = (byte)i;
        }
    }
    if (mode == 2)
    {
        D3DResource_BlockUntilNotBusy(texture);
        byte *bits;
        dword pitch, width, height, slice;
        D3D::PixelJar::FindSurfaceWithinTexture(texture, (D3DCUBEMAP_FACES)0, 0,
            &bits, &pitch, &width, &height, &slice);
        for (long y = 0; y < 64; ++y)
        {
            dword component = curve[y * 4];
            dword outer = (component | 0xffffff00) << 8;
            for (long x = 0; x < 64; ++x)
            {
                dword color = ((curve[x * 4] | outer) << 8) | component;
                ((dword *)bits)[function_024e50((word)y, (word)x)] = color;
            }
        }
        real bounds[4];
        bounds[0] = (real)(short)g_48564a;
        bounds[1] = (real)(short)g_48564e;
        bounds[2] = (real)(short)g_485648;
        bounds[3] = (real)(short)g_48564c;
        function_34420((t_34770_begin)function_251b0, bounds, function_0254c0, NULL, 0);
        function_34420((t_34770_begin)function_251b0, bounds, function_0254c0, NULL, 1);
    }
}

extern short g_4b9dd0, g_4b9dd2, g_4b9dd4, g_4b9dd6;
extern long g_485af4[4], g_485b04[4];
extern dword g_4b8308, g_4b8438, g_4b8450, g_4b82ec, g_4b8448, g_4b843c;
extern dword g_4b82e0, g_4b82fc;
bool function_01dd60(long index, long *width, long *height);
void function_142f0(short mode);
bool __stdcall function_34100(long mode, real const *rectangle, real const *coordinates,
    real *out, real const *parameters);

struct s_34a90_parameters
{
    real depth;
    real distortion;
    point2f offsets[4];
    point2f scales[4];
    real weights[4];
    real low, high, scale;
    long count;
};

PRIVATE __forceinline void configure_34a90_state(long target, short blend,
    dword color_write, bool use_depth, bool depth_write, real depth)
{
    if (target != NONE) function_14bc0((short)target, 0, use_depth);
    if (blend != NONE) function_142f0(blend);
    g_4b8308 = color_write;
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, color_write);
    g_4b8438 = use_depth ? 2 : 0;
    D3DDevice_SetRenderState(D3DRS_ZENABLE, g_4b8438);
    g_4b8450 = 0;
    D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    g_4b82ec = 0;
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    g_4b8448 = 0;
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b843c = 0;
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    if (use_depth)
    {
        g_4b82e0 = depth < 0.0f ? D3DCMP_GREATER : D3DCMP_LESSEQUAL;
        D3DDevice_SetRenderState(D3DRS_ZFUNC, g_4b82e0);
        g_4b82fc = depth_write;
        D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, g_4b82fc);
    }
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
}

// @retail 0x34a90
void __stdcall function_34a90(long target, short blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real distortion, real scale,
    long count, bool full_surface, bool viewport_textures)
{
    struct { long width; long height; } dimensions = { 0, 0 };
    long selected_target = target;
    if (selected_target == NONE) selected_target = g_4858b8;
    function_01dd60(selected_target, &dimensions.width, &dimensions.height);
    real bounds[4];
    if (!(full_surface)) {
        bounds[0] = (real)g_4b9dd2;
        bounds[1] = (real)g_4b9dd6;
        bounds[2] = (real)g_4b9dd0;
        bounds[3] = (real)g_4b9dd4;
    } else {
        bounds[0] = 0.0f;
        bounds[1] = (real)dimensions.width;
        bounds[2] = 0.0f;
        bounds[3] = (real)dimensions.height;
    }
    real inverse_x = 1.0f / dimensions.width;
    real inverse_y = 1.0f / dimensions.height;
    s_34a90_parameters parameters;
    parameters.depth = depth >= 0.0f ? depth : 0.0f - depth;
    parameters.distortion = distortion;
    for (long i = 0; i < count; ++i)
    {
        if (viewport_textures)
        {
            parameters.offsets[i].x = (real)g_4b9dd2;
            parameters.offsets[i].y = (real)g_4b9dd0;
            parameters.scales[i].x = (g_4b9dd6 - g_4b9dd2) * inverse_x;
            parameters.scales[i].y = (g_4b9dd4 - g_4b9dd0) * inverse_y;
        }
        else
        {
            parameters.offsets[i].x = 0.0f;
            parameters.offsets[i].y = 0.0f;
            parameters.scales[i].x = g_485af4[i] * inverse_x;
            parameters.scales[i].y = g_485b04[i] * inverse_y;
        }
        parameters.weights[i] = 0.0f;
    }
    parameters.low = 0.0f;
    parameters.high = 0.0f;
    parameters.scale = scale;
    parameters.count = count;
    configure_34a90_state(target, blend, color_write, use_depth, depth_write, depth);
    function_34420(NULL, bounds, (t_34770_vertex)function_34100, NULL, (long)&parameters);
}

// @retail 0x34d90
void function_34d90(long target, real const *weights, short blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real low, real high,
    bool full_surface, t_34770_vertex vertex)
{
    long width = 0, height = 0;
    function_01dd60(target != NONE ? target : g_4858b8, &width, &height);
    real bounds[4];
    if (full_surface)
    {
        bounds[0] = 0.0f;
        bounds[1] = (real)width;
        bounds[2] = 0.0f;
        bounds[3] = (real)height;
    }
    else
    {
        bounds[0] = (real)g_4b9dd2;
        bounds[1] = (real)g_4b9dd6;
        bounds[2] = (real)g_4b9dd0;
        bounds[3] = (real)g_4b9dd4;
    }
    real inverse_x = 1.0f / width;
    real inverse_y = 1.0f / height;
    s_34a90_parameters parameters;
    parameters.depth = depth >= 0.0f ? depth : 0.0f - depth;
    parameters.distortion = 0.0f;
    for (long i = 0; i < 4; ++i)
    {
        parameters.offsets[i].x = 0.0f;
        parameters.offsets[i].y = 0.0f;
        parameters.scales[i].x = g_485af4[i] * inverse_x;
        parameters.scales[i].y = g_485b04[i] * inverse_y;
        parameters.weights[i] = weights[i];
    }
    parameters.low = low;
    parameters.high = high;
    parameters.scale = 1.0f;
    parameters.count = 4;
    configure_34a90_state(target, blend, color_write, use_depth, depth_write, depth);
    if (!vertex) vertex = (t_34770_vertex)function_34100;
    function_34770(NULL, bounds, vertex, NULL, 20, 30, (long)&parameters);
}

struct s_245c0_state
{
    s_24ee0_state curve;
    dword frame_count;
};

// @retail 0x245c0
void function_245c0(s_245c0_state *state, real *histogram, short mode)
{
    state->curve.histogram = histogram;
    if (mode > 0 && mode <= 3)
    {
        if (!(state->frame_count & 7))
        {
            function_249f0();
            function_14bc0((short)g_4858b8, 0, true);
            D3DDevice_InsertCallback(D3DCALLBACK_WRITE, (D3DCALLBACK)function_24970, (dword)state);
        }
    }
    else
    {
        state->curve.range.low = 0.0f;
        state->curve.range.high = 0.75f;
        state->curve.range.average = 0.4f;
        state->curve.range.smoothed_low = 0.0f;
        state->curve.range.smoothed_high = 0.75f;
        state->curve.range.smoothed_average = 0.4f;
        state->frame_count = 0;
    }
    if (mode == 2 || !(state->frame_count & 7))
        function_24ee0(&state->curve, mode);
    ++state->frame_count;
}

PRIVATE __forceinline void configure_351a0_texture(long stage, D3DTEXTUREFILTERTYPE filter)
{
    D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
    D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, filter);
    D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, filter);
    D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
}

// @retail 0x351a0
void __stdcall function_351a0(long texture, long target)
{
    function_14bc0((short)target, 0, true);
    function_14f60(0, (short)texture);
    configure_351a0_texture(0, D3DTEXF_POINT);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    g_4b8450 = 0;
    D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    g_4b8448 = 0;
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSFinalCombinerInputsABCD = 8;
    g_484f68.PSFinalCombinerInputsEFG = 0x1800;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_34a90(target, NONE, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
}

extern real g_4b8494;
dword *function_1c290(real value);

// @retail 0x352e0
void __stdcall function_352e0(long target, bool multiple)
{
    long count = multiple ? 4 : 1;
    bool const volatile *multiple_reference = &multiple;
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    if (target == NONE) target = 19;
    for (long stage = 0; stage < count; ++stage)
    {
        function_14f60((short)stage, (short)g_4858b8);
        configure_351a0_texture(stage, D3DTEXF_LINEAR);
    }
    memset(&g_484f68, 0, sizeof(g_484f68));
    dword output;
    if (*multiple_reference)
    {
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSAlphaInputs[0] = 0x18201920;
        g_484f68.PSAlphaInputs[1] = 0x1a201b20;
        g_484f68.PSAlphaInputs[2] = 0x1c201d20;
        g_484f68.PSAlphaOutputs[1] = 0x30d00;
        g_484f68.PSAlphaOutputs[2] = 0x30c00;
        g_484f68.PSRGBInputs[0] = 0x8200920;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBOutputs[2] = 0x30c00;
        output = 0x30c00;
    }
    else
    {
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSAlphaInputs[0] = 0x18200000;
        g_484f68.PSRGBInputs[0] = 0x8200000;
        output = 0xc0;
    }
    g_484f68.PSAlphaOutputs[0] = output;
    g_484f68.PSRGBOutputs[0] = output;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_34a90(target, 10, 0x1010101, false, false, 1.0f,
        *multiple_reference ? 0.5f : 0.0f, 1.0f, count, true, true);
}

void function_144f0(long tag, short stage, long fallback, short fallback_index, short index, real priority);
void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value);
dword function_1cc30(long index);
dword __cdecl function_131fc0(real alpha);

// @retail 0x2a8d0
void __stdcall function_2a8d0(long tag, long bitmap_index, real const *source,
    real const *destination, long arg_af5a49, color3f const *color,
    real strength, bool blend, bool inverse, bool alternate_target,
    bool screen_target, bool simple, bool preserve_alpha)
{
    if ((!g_4b99b0[0] && !g_4b99b0[1]) || strength < 0.0f) return;
    if (strength > 1.0f) strength = 1.0f;
    else if (strength <= 0.0f) return;
    if (!g_4b99b0[1] && alternate_target) return;
    real width = 1.0f, height = 1.0f;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (bitmap_index >= 0 && bitmap_index < *(long *)(definition + 0x44))
        {
            byte *bitmap = *(byte **)(definition + 0x48) + bitmap_index * 0x74;
            real w = (real)*(short *)(bitmap + 4);
            real h = (real)*(short *)(bitmap + 6);
            width = 1.0f > w ? 1.0f : w;
            height = 1.0f > h ? 1.0f : h;
        }
    }
    if (alternate_target) function_14bc0(*(short *)(g_4b99b0 + 0x38), 0, false);
    else if (screen_target) function_14bc0(0, 0, false);
    else function_14bc0(*(short *)(g_4b99b0 + 0x34), 0, false);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, inverse ? 0x306 : 1);
    function_0222d0(D3DRS_DESTBLEND, inverse ? 0 : 0x303);
    function_0222d0(D3DRS_BLENDOP, blend && alternate_target ? 0xf006 : 0x8006);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_144f0(tag, 0, 0, 0, (short)bitmap_index, 0.0f);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    function_0224f0(0, D3DTSS_MAXANISOTROPY, 0);
    function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
    function_0224f0(0, D3DTSS_COLORSIGN, 0);
    function_0224f0(0, D3DTSS_ALPHAKILL, 0);
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    if (!simple && !preserve_alpha)
    {
        g_484f68.PSCombinerCount = 0x11002;
        real alpha = 1.0f - *(real *)(g_4b99b0 + 0x30);
        alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
        g_484f68.PSConstant0[0] = function_131fc0(alpha);
        g_484f68.PSConstant1[0] = 0xff;
        g_484f68.PSRGBInputs[0] = 0x8200211;
        g_484f68.PSRGBOutputs[0] = inverse ? 0x800 : 0;
        g_484f68.PSConstant0[1] = function_131f40(strength, color);
        g_484f68.PSRGBInputs[1] = 0x8011811;
        g_484f68.PSRGBOutputs[1] = 0xcd;
        alpha = 1.0f - *(real *)(g_4b99b0 + 0x2c);
        alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
        g_484f68.PSFinalCombinerConstant0 = function_131fc0(alpha);
        g_484f68.PSFinalCombinerInputsABCD = ((inverse ? 0x11 : 0x20) | 0xf00) << 16;
        g_484f68.PSFinalCombinerInputsEFG = ((blend ? 0 : 0x1d) | 0xc0d00) << 8;
    }
    else
    {
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSConstant0[0] = function_131fc0(strength > 1.0f ? 1.0f : strength);
        g_484f68.PSAlphaInputs[0] = 0x18110000;
        g_484f68.PSAlphaOutputs[0] = 0x80;
        g_484f68.PSFinalCombinerInputsABCD = 0x8180000;
        g_484f68.PSFinalCombinerInputsEFG = (preserve_alpha ? 0 : 0x18) << 8;
    }
    function_1ccf0(&g_484f68);
    function_1cc30(12);
    function_1c710(g_51f0f0);
    real default_source[4] = { 0.0f, width, 0.0f, height };
    real default_destination[4];
    default_destination[0] = 320.0f - width * 0.5f;
    default_destination[1] = 320.0f + width * 0.5f;
    default_destination[2] = 240.0f - height * 0.5f + arg_af5a49;
    default_destination[3] = arg_af5a49 + height * 0.5f + 240.0f;
    if (!source) source = default_source;
    if (!destination) destination = default_destination;
    D3DDevice_Begin(D3DPT_QUADLIST);
    real inverse_width = 1.0f / width;
    real inverse_height = 1.0f / height;
    D3DDevice_SetVertexData2f(1, source[0] * inverse_width, source[2] * inverse_height);
    D3DDevice_SetVertexData4f(0, destination[0], destination[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, source[1] * inverse_width, source[2] * inverse_height);
    D3DDevice_SetVertexData4f(0, destination[1], destination[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, source[1] * inverse_width, source[3] * inverse_height);
    D3DDevice_SetVertexData4f(0, destination[1], destination[3], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, source[0] * inverse_width, source[3] * inverse_height);
    D3DDevice_SetVertexData4f(0, destination[0], destination[3], 16777215.0f, 16777215.0f);
    D3DDevice_End();
}

struct s_coefficient_level { long offset, unknown04, unknown08; };
struct s_coefficient_layout
{
    short order, groups, columns;
    byte unknown06[0xe];
    long level_count;
    s_coefficient_level const *levels;
    long unknown1c;
    real const *data;
};
struct s_137801;
struct s_light_sample_source;
matrix3x3 *function_142bf0(matrix3x3 const *matrix, real scale, matrix3x3 *out);
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order,
    real red, real green, real blue, real *red_result, real *green_result, real *blue_result);
void __stdcall function_22cf0(s_light_sample_source const *source, real *red, real *green,
    real *blue, byte order, vector3f *direction, point3f const *position);

// @retail 0x23690
bool __stdcall function_23690(s_coefficient_layout const *layout, short order, long object_index,
    s_137801 const *vertices, byte const *indices, short count, long tag)
{
    (void)&layout; (void)&order; (void)&object_index; (void)&vertices;
    (void)&indices; (void)&count; (void)&tag;
    volatile bool found = false;
    for (short i = 0; i < g_5093e8; ++i)
        if (g_4b89b0[i].key == object_index) found = true;
    if (found || g_5093e8 >= 24) return true;
    s_table_entry *entry = &g_4b89b0[g_5093e8];
    entry->key = object_index;
    *(short *)&entry->unknown4 = order;
    byte const *structure = (byte *)g_4e0344;
    bool use_samples = structure && *(long const *)(structure + 0x80) > 0 && g_4e0348 &&
        *(long const *)(*(byte const **)(structure + 0x84) + 0x1c) != NONE &&
        *(long const *)(*(byte const **)(structure + 0x84) + 4) == *(long const *)((byte *)g_4e0348 + 8);
    real red[16], green[16], blue[16];
    if (!use_samples)
    {
        vector3f direction = { 0.0f, 0.0f, -1.0f };
        spherical_harmonics_evaluate_directional_light(&direction, layout->order,
            1.0f, 1.0f, 1.0f, red, green, blue);
    }
    long level = 5 - order;
    if (level > layout->level_count - 1) level = layout->level_count - 1;
    real const *data = layout->data + layout->levels[level].offset;
    long stride = layout->groups * (layout->columns * 3 + 4);
    s_bucket_source const *buckets = (s_bucket_source const *)vertices;
    long total = function_023540(count, buckets, entry, stride);
    long bytes = total * stride * sizeof(real);
    if (!bytes) return true;
    long next = g_487b18.size + bytes;
    if (next > 0x27000) return false;
    long handle = g_487b18.size & 0x0fffffff;
    g_487b18.size = next;
    entry->data_handle = handle;
    real *output = (real *)(g_487b18.data + handle);
    for (long bucket = 0; bucket < count; ++bucket)
    {
        byte const *pair = (byte const *)vertices + 4 + bucket * 4;
        long available = *(word const *)(pair + 2);
        if (!available) continue;
        real const *source = (real const *)((byte const *)vertices + 0x44 + *(short const *)pair * 0x30);
        transform4x3f matrix;
        function_0235a0(source, &matrix);
        long samples = available > 1 ? 2 : 1;
        for (long sample = 0; sample < samples; ++sample)
        {
            if (sample) function_0235a0(source + 12, &matrix);
            real determinant = matrix.left.k * matrix.forward.j * matrix.up.i;
            determinant += matrix.forward.k * matrix.up.j * matrix.left.i;
            determinant += matrix.up.k * matrix.left.j * matrix.forward.i;
            determinant -= matrix.forward.i * matrix.left.k * matrix.up.j;
            determinant -= matrix.forward.j * matrix.left.i * matrix.up.k;
            determinant -= matrix.forward.k * matrix.left.j * matrix.up.i;
            matrix3x3 rotation;
            function_142bf0((matrix3x3 const *)&matrix.forward, determinant, &rotation);
            if (use_samples)
            {
                point3f const *position = (point3f const *)function_23600(tag, object_index, indices, bucket, sample);
                if (position)
                {
                    byte *bsp = *(byte **)(structure + 0x84);
                    vector3f direction;
                    function_22cf0((s_light_sample_source const *)(bsp + 0x38), red, green, blue,
                        (byte)layout->order, &direction, position);
                    entry->scale = direction;
                }
            }
            real rotated_red[16], rotated_green[16], rotated_blue[16];
            function_143600((real const *)&rotation, 3, red, rotated_red);
            function_143600((real const *)&rotation, 3, green, rotated_green);
            function_143600((real const *)&rotation, 3, blue, rotated_blue);
            function_023ca0((s_block_layout const *)layout, data, rotated_red, rotated_green, rotated_blue, output);
            output += stride;
        }
    }
    ++g_5093e8;
    return true;
}

// @retail 0x29b00
void __stdcall function_29b00(long tag, long first_bitmap, real first_alpha,
    real first_angle, real first_scale, long second_bitmap, real second_alpha,
    real second_angle, real second_scale, bool screen_target)
{
    if ((!g_4b99b0[0] && !g_4b99b0[1]) || tag == NONE) return;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    real width = 1.0f, height = 1.0f;
    if (*(long *)(definition + 0x44) > 0)
    {
        byte *bitmap = *(byte **)(definition + 0x48);
        width = (real)*(short *)(bitmap + 4);
        height = (real)*(short *)(bitmap + 6);
    }
    function_14bc0(screen_target ? 0 : *(short *)(g_4b99b0 + 0x34), 0, false);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, 1);
    function_0222d0(D3DRS_DESTBLEND, 0x303);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    long stage = 0;
    do
    {
        function_144f0(tag, (short)stage, 0, 0,
            (short)(stage == 0 ? first_bitmap : second_bitmap), 0.0f);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 3);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 3);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 3);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
        ++stage;
    } while (stage < 2);
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x21;
    g_484f68.PSCombinerCount = 0x11002;
    first_alpha = first_alpha < 0.0f ? 0.0f : first_alpha > 1.0f ? 1.0f : first_alpha;
    second_alpha = second_alpha < 0.0f ? 0.0f : second_alpha > 1.0f ? 1.0f : second_alpha;
    g_484f68.PSConstant0[0] = function_131fc0(first_alpha);
    g_484f68.PSConstant1[0] = function_131fc0(second_alpha);
    g_484f68.PSRGBInputs[0] = 0x8110912;
    g_484f68.PSRGBOutputs[0] = 0xc00;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    real scale = first_scale > second_scale ? first_scale : second_scale;
    if (scale > 0.0f)
    {
        real inverse_scale = 1.0f / scale;
        first_scale *= inverse_scale;
        second_scale *= inverse_scale;
    }
    real bounds[4];
    bounds[0] = 320.0f - scale * width * 0.5f;
    bounds[1] = 320.0f + scale * width * 0.5f;
    bounds[2] = 240.0f - scale * height * 0.5f;
    bounds[3] = 240.0f + scale * height * 0.5f;
    if (first_scale == 0.0f) first_scale = 1.0f;
    if (second_scale == 0.0f) second_scale = 1.0f;
    real first_coordinates[8], second_coordinates[8];
    real cosine = (real)cos(first_angle), sine = (real)sin(first_angle);
    first_coordinates[0] = ((sine - cosine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[1] = ((-cosine - sine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[2] = ((cosine + sine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[3] = ((sine - cosine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[4] = ((cosine - sine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[5] = ((sine + cosine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[6] = ((-cosine - sine) * first_scale + 1.0f) * 0.5f;
    first_coordinates[7] = ((cosine - sine) * first_scale + 1.0f) * 0.5f;
    cosine = (real)cos(second_angle); sine = (real)sin(second_angle);
    second_coordinates[0] = ((sine - cosine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[1] = ((-cosine - sine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[2] = ((cosine + sine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[3] = ((sine - cosine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[4] = ((cosine - sine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[5] = ((sine + cosine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[6] = ((-cosine - sine) * second_scale + 1.0f) * 0.5f;
    second_coordinates[7] = ((cosine - sine) * second_scale + 1.0f) * 0.5f;
    D3DDevice_Begin(D3DPT_QUADLIST);
    D3DDevice_SetVertexData2f(1, first_coordinates[0], first_coordinates[1]);
    D3DDevice_SetVertexData2f(2, second_coordinates[0], second_coordinates[1]);
    D3DDevice_SetVertexData4f(0, bounds[0], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, first_coordinates[2], first_coordinates[3]);
    D3DDevice_SetVertexData2f(2, second_coordinates[2], second_coordinates[3]);
    D3DDevice_SetVertexData4f(0, bounds[1], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, first_coordinates[4], first_coordinates[5]);
    D3DDevice_SetVertexData2f(2, second_coordinates[4], second_coordinates[5]);
    D3DDevice_SetVertexData4f(0, bounds[1], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2f(1, first_coordinates[6], first_coordinates[7]);
    D3DDevice_SetVertexData2f(2, second_coordinates[6], second_coordinates[7]);
    D3DDevice_SetVertexData4f(0, bounds[0], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice_End();
}

struct s_bitmap_view;
struct s_type_7ba8e9;
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
extern IDirect3DBaseTexture8 *g_51f3c8[2][4];
extern dword g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324;

struct s_2a2f0_record
{
    long y;
    real speed;
    long unused;
};

#pragma inline_depth(0)
// @retail 0x2a2f0
void __stdcall function_2a2f0(volatile long tag, volatile real alpha)
{
    if (tag != NONE)
    {
        s_2a2f0_record records[5];
        records[0].y = 58; records[0].speed = 0.01f; records[0].unused = 0;
        records[1].y = 64; records[1].speed = 0.03f; records[1].unused = 0;
        records[2].y = -19; records[2].speed = 0.05f; records[2].unused = 0;
        records[3].y = -22; records[3].speed = 0.08f; records[3].unused = 0;
        records[4].y = 60; records[4].speed = 0.02f; records[4].unused = 0;
        function_14bc0(0, 0, false);
#pragma inline_depth(255)
        g_4b8308 = 0x10101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x10101);
        g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        g_4b82f4 = 1; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 1);
        g_4b82f8 = 0x303; D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0x303);
        g_4b8324 = 0x8006; D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
        g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
        g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
        function_0222d0(D3DRS_ZENABLE, 0);
        g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
#pragma inline_depth(0)
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        real unclamped_alpha = alpha;
        if (unclamped_alpha < 0.0f) alpha = 0.0f;
        else
        {
            alpha = 1.0f;
            if (!(unclamped_alpha > 1.0f)) alpha = unclamped_alpha;
        }
        real alpha_scale = 255.0f;
        long packed = 0;
        __asm
        {
            fld alpha
            fld alpha_scale
            fmulp st(1), st(0)
            fistp packed
            shl packed, 24
        }
        g_484f68.PSConstant0[0] = packed;
        g_484f68.PSRGBInputs[0] = 0x8200000;
        g_484f68.PSRGBOutputs[0] = 0x80;
        g_484f68.PSAlphaInputs[0] = 0x18110000;
        g_484f68.PSAlphaOutputs[0] = 0x10080;
        g_484f68.PSFinalCombinerInputsABCD = 0x8180000;
        g_484f68.PSFinalCombinerInputsEFG = 0x1800;
#pragma inline_depth(255)
        D3DDevice_SetPixelShaderProgram(&g_484f68);
#pragma inline_depth(0)
        function_1c590((s_shader_cache *)g_51f0f0,
            *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
        function_1c710(g_51f0f0);
        long definition_index = tag & 0xffff;
        long i = 0;
        long bitmap_offset = 0;
        s_2a2f0_record *volatile record = records;
        for (i = 0; bitmap_offset < 5 * 0x74; ++i, ++record, bitmap_offset += 0x74)
        {
            byte *definition = g_4e3b44[definition_index].bytes;
            volatile real &height = *(volatile real *)&tag;
            volatile real &width = alpha;
            height = 1.0f;
            width = 1.0f;
            if (i < *(long *)(definition + 0x44))
            {
                byte *bitmap = *(byte **)(definition + 0x48) + bitmap_offset;
                real local_62a74a_2 = (real)*(short *)(bitmap + 4);
                width = 1.0f;
                if (!(local_62a74a_2 < 1.0f)) width = local_62a74a_2;
                real local_98083a = (real)*(short *)(bitmap + 6);
                height = 1.0f;
                if (!(local_98083a < 1.0f)) height = local_98083a;
            }
            // Reload the count after the dimension writes.
            long count = *(volatile long *)(definition + 0x44);
            byte *selected = NULL;
            if (count > 0)
            {
                short index = (short)((short)i % count);
                if (definition && index >= 0 && index < count)
                    selected = *(byte **)(definition + 0x48) + index * 0x74;
            }
            if (count > 0 && *(short *)(selected + 0xa) == 0)
                function_14390(0, (s_type_7ba8e9 *)selected, 0.0f);
            else
            {
                long fallback = *(long *)(g_485a80 + 0x64);
                if (fallback != NONE)
                {
                    byte *fallback_definition = g_4e3b44[fallback & 0xffff].bytes;
                    if (fallback_definition && *(long *)(fallback_definition + 0x44) > 0)
                    {
                        byte *bitmap = *(byte **)(fallback_definition + 0x48);
                        if (bitmap)
                        {
                            g_51f3c8[1][0] = function_12310((s_bitmap_view *)bitmap, 0.0f);
                            g_485af4[0] = *(short *)(bitmap + 4);
                            g_485b04[0] = *(short *)(bitmap + 6);
                        }
                    }
                }
            }
#pragma inline_depth(255)
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
            D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
#pragma inline_depth(0)
            function_1cf50();
            real half_height = height * 0.5f;
            real center = 240.0f - ((real)record->y - half_height);
            box2f bounds;
            bounds.x0 = 0.0f; bounds.x1 = 640.0f;
            bounds.y0 = center - half_height;
            bounds.y1 = center + half_height;
            real time = 0.0f;
            if (g_510c54 && g_510c54->active)
                time = g_510c54->game_time * g_510c54->rate;
            real offset = 0.0f - record->speed * time;
            D3DDevice_Begin(D3DPT_TRIANGLEFAN);
            D3DDevice_SetVertexData2f(1, offset, 0.0f);
            D3DDevice_SetVertexData4f(0, bounds.x0, bounds.y0, 16777215.0f, 16777215.0f);
            real right = 640.0f / width + offset;
            D3DDevice_SetVertexData2f(1, right, 0.0f);
            D3DDevice_SetVertexData4f(0, bounds.x1, bounds.y0, 16777215.0f, 16777215.0f);
            D3DDevice_SetVertexData2f(1, right, 1.0f);
            D3DDevice_SetVertexData4f(0, bounds.x1, bounds.y1, 16777215.0f, 16777215.0f);
            D3DDevice_SetVertexData2f(1, offset, 1.0f);
            D3DDevice_SetVertexData4f(0, bounds.x0, bounds.y1, 16777215.0f, 16777215.0f);
            D3DDevice_End();
        }
    }
}
#pragma inline_depth(255)

real g_485658;
extern dword g_4b82e8;
__declspec(noinline) double __stdcall function_2b480(real value);
dword __cdecl pack_color4f(color4f const *color);

PRIVATE __forceinline real clamp_filter_unit(real value)
{
    return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

PRIVATE __forceinline dword filter_output_shift(long exponent, long threshold)
{
    return ((exponent > threshold + 1 ? 0x20 : exponent > threshold ? 0x10 : 0) << 12) | 0xd00;
}

PRIVATE __forceinline long filter_quantized_byte(real value, bool direct)
{
    if ((long)(direct ? floor((double)value) : function_2b480(value)) < 0) return 0;
    if ((long)function_2b480(value) > 255) return 255;
    return (long)function_2b480(value);
}

PRIVATE __forceinline void set_filter_blending(bool additive)
{
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, additive ? 0x302 : 0x8001);
    function_0222d0(D3DRS_DESTBLEND, additive ? 1 : 0x303);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    if (!additive) function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
    function_0222d0(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, 0x203);
    g_4b8450 = 0;
    D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
}

// @retail 0x3a5c0
bool __stdcall function_3a5c0(byte const *configuration, real low, real high, long mode,
    bool center, bool shift, bool clip, bool skip_state, real *depth_out)
{
    real offset = 0.0f;
    real maximum = 1.0f;
    bool small = false;
    bool allow_small = false;
    bool allow_negative = false;
    real inverse_distance = 1.0f / g_48565c;
    low *= inverse_distance;
    high *= inverse_distance;
    switch (mode)
    {
    case 0: maximum = 512.0f; allow_small = true; allow_negative = true; break;
    case 1: maximum = 64.0f; allow_small = false; allow_negative = true; break;
    case 2: maximum = 128.0f; allow_small = true; allow_negative = false; break;
    case 3: maximum = 128.0f; allow_small = false; allow_negative = false; break;
    }
    if (high * 256.0f <= 1.0f && allow_small)
    {
        low *= 256.0f;
        high *= 256.0f;
        small = true;
    }
    low = (real)(long)floor(low * 65536.0f) * (1.0f / 65536.0f);
    high = (real)(long)floor(high * 65536.0f) * (1.0f / 65536.0f);
    if (!(high > low)) return false;
    real inverse_span = 1.0f / (high - low);
    if (inverse_span > maximum)
    {
        real width = 1.0f / maximum;
        inverse_span = maximum;
        if (small) return false;
        if (center) low = (low + high) * 0.5f - width * 0.5f;
        else if (shift) low = high - width;
        else if (!clip) return false;
    }
    if (low < 0.0f)
    {
        if (allow_negative) offset = clamp_filter_unit(0.0f - inverse_span * low);
        else if (!shift || !clip) return false;
        low = 0.0f;
    }
    if (!skip_state)
    {
        function_14bc0((short)g_4858b8, 0, true);
        function_14f60(0, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
        D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 1);
        D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
        g_4b8308 = 0x1010101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
        g_4b82e8 = 0; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
        g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
        g_4b8438 = 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
        g_4b82fc = 0; D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
        function_0222d0(D3DRS_ZFUNC, 0x207);
        g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        function_0222d0(D3DRS_DEPTHCLIPCONTROL, 0x10);
        function_1cc30(12);
        function_1c710(g_51f0f0);
    }
    long fixed = (long)floor(low * 65536.0f);
    long whole = fixed >> 8;
    long fraction = fixed & 0xff;
    long input = small ? 5 : 4;
    long alpha_input = small ? 4 : 24;
    if (whole > 255) whole = fraction = 255;
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11108;
    g_484f68.PSConstant0[0] = 0xff0000;
    g_484f68.PSConstant1[0] = 0xff00;
    g_484f68.PSRGBInputs[0] = 0x8010802;
    g_484f68.PSRGBOutputs[0] = 0x3045;
    g_484f68.PSConstant0[1] = (dword)fraction << 24;
    g_484f68.PSRGBInputs[1] = ((dword)input << 24) | 0x201140;
    g_484f68.PSRGBOutputs[1] = 0xc00;
    g_484f68.PSAlphaInputs[1] = ((dword)input << 24) | 0x209140;
    g_484f68.PSAlphaOutputs[1] = 0xc00;
    g_484f68.PSConstant0[2] = (dword)whole << 24;
    g_484f68.PSRGBInputs[2] = 0x2020;
    g_484f68.PSRGBOutputs[2] = 0x4d00;
    g_484f68.PSAlphaInputs[2] = ((dword)alpha_input << 24) | 0x201140;
    g_484f68.PSAlphaOutputs[2] = 0xd00;
    g_484f68.PSConstant0[3] = 0x1000000;
    g_484f68.PSRGBInputs[3] = 0xcc202d20;
    g_484f68.PSRGBOutputs[3] = 0xc00;
    g_484f68.PSAlphaInputs[3] = 0xdd202df1;
    g_484f68.PSAlphaOutputs[3] = 0xd00;
    long exponent = 0;
    if (inverse_span > 1.0f)
        do { ++exponent; } while (inverse_span > (real)(1 << exponent));
    if (mode == 0)
    {
        bool extra_texture = configuration == (byte const *)NONE;
        if (extra_texture)
        {
            if (!skip_state) set_filter_blending(true);
            g_484f68.PSTextureModes = 0x21;
            g_484f68.PSAlphaInputs[0] = 0x19140000;
            g_484f68.PSAlphaOutputs[0] = 0x90;
        }
        g_484f68.PSConstant0[4] = 0x80000000;
        g_484f68.PSRGBInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSRGBOutputs[4] = filter_output_shift(exponent, 1);
        g_484f68.PSAlphaInputs[4] = 0xdd201120;
        g_484f68.PSAlphaOutputs[4] = 0xc00;
        g_484f68.PSRGBInputs[5] = 0x2020;
        g_484f68.PSRGBOutputs[5] = 0x4400;
        g_484f68.PSAlphaInputs[5] = 0xd200d00 | (exponent > 6 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        real gain = 1.0f / (exponent > 8 ? 4.0f : exponent > 7 ? 2.0f : 1.0f);
        gain /= exponent > 0 ? 2.0f : 1.0f;
        gain /= inverse_span / (real)(1 << exponent);
        g_484f68.PSConstant1[6] = function_131fc0(clamp_filter_unit(gain * offset));
        g_484f68.PSConstant0[6] = 0x7f000000;
        g_484f68.PSRGBInputs[6] = 0x1d201220;
        g_484f68.PSRGBOutputs[6] = filter_output_shift(exponent, 7);
        g_484f68.PSAlphaInputs[6] = 0x18201120;
        g_484f68.PSAlphaOutputs[6] = 0xc00;
        long upper = filter_quantized_byte(inverse_span * 256.0f / (real)(1 << exponent), true);
        long lower = filter_quantized_byte(inverse_span * 0.5f, true);
        g_484f68.PSConstant1[7] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[7] = 0x20 | (small ? 0x2000 : 0);
        g_484f68.PSRGBOutputs[7] = 0x4500;
        g_484f68.PSAlphaInputs[7] = 0xd120c02;
        g_484f68.PSAlphaOutputs[7] = filter_output_shift(exponent, 0);
        g_484f68.PSFinalCombinerConstant1 = function_131fc0(offset);
        g_484f68.PSFinalCombinerInputsABCD = 0x41d1105;
        g_484f68.PSFinalCombinerInputsEFG = (extra_texture ? 25 : 32) << 8;
    }
    else if (mode == 1)
    {
        color4f first, second;
        first.alpha = clamp_filter_unit(*(real const *)(configuration + 0x28));
        first.red = clamp_filter_unit(*(real const *)(configuration + 0x1c) * *(real const *)(configuration + 0x28));
        first.green = clamp_filter_unit(*(real const *)(configuration + 0x20) * *(real const *)(configuration + 0x28));
        first.blue = clamp_filter_unit(*(real const *)(configuration + 0x24) * *(real const *)(configuration + 0x28));
        second.alpha = clamp_filter_unit(*(real const *)(configuration + 0x68));
        second.red = clamp_filter_unit(*(real const *)(configuration + 0x5c));
        second.green = clamp_filter_unit(*(real const *)(configuration + 0x60));
        second.blue = clamp_filter_unit(*(real const *)(configuration + 0x64));
        if (!skip_state) set_filter_blending(false);
        g_484f68.PSAlphaInputs[0] = 0x8200000;
        g_484f68.PSAlphaOutputs[0] = 0x40;
        g_484f68.PSAlphaInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[4] = filter_output_shift(exponent, 1);
        real gain = 1.0f / (exponent > 5 ? 4.0f : exponent > 4 ? 2.0f : 1.0f);
        gain /= exponent > 0 ? 2.0f : 1.0f;
        gain /= inverse_span / (real)(1 << exponent);
        g_484f68.PSConstant1[5] = function_131fc0(clamp_filter_unit(gain * offset));
        g_484f68.PSAlphaInputs[5] = 0x1d201220;
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        g_484f68.PSConstant0[6] = pack_color4f(&first);
        long upper = filter_quantized_byte(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = filter_quantized_byte(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[6] = 0x1341134;
        g_484f68.PSRGBOutputs[6] = 0x45;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_shift(exponent, 0);
        g_484f68.PSConstant0[7] = pack_color4f(&second);
        g_484f68.PSAlphaInputs[7] = 0x11141d05;
        g_484f68.PSAlphaOutputs[7] = 0xc00;
        g_484f68.PSFinalCombinerConstant1 = pack_color4f(&second);
        g_484f68.PSFinalCombinerInputsABCD = 0x114000f;
        g_484f68.PSFinalCombinerInputsEFG = 0x1d041c00;
    }
    else if (mode == 2)
    {
        if (!skip_state) set_filter_blending(false);
        g_484f68.PSTextureModes = 0x21;
        g_484f68.PSConstant0[4] = small ? 0x7f0000ff : 0;
        g_484f68.PSRGBInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSRGBOutputs[4] = filter_output_shift(exponent, 1);
        g_484f68.PSAlphaInputs[4] = 0x18011120;
        g_484f68.PSAlphaOutputs[4] = 0xc00;
        g_484f68.PSConstant0[5] = function_131fc0(*(real const *)configuration);
        g_484f68.PSConstant1[5] = function_131fc0(*(real const *)(configuration + 4));
        g_484f68.PSRGBInputs[5] = 0x39111912;
        g_484f68.PSRGBOutputs[5] = 0xd00;
        g_484f68.PSAlphaInputs[5] = 0xd200d00 | (exponent > 6 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        long upper = filter_quantized_byte(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = filter_quantized_byte(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_shift(exponent, 0);
        g_484f68.PSAlphaInputs[7] = 0x1d0d0d20;
        g_484f68.PSAlphaOutputs[7] = 0x4d00;
        if (configuration[0x14])
        {
            if (!skip_state)
            {
                function_14bc0(3, 0, true);
                function_0222d0(D3DRS_COLORWRITEENABLE, 1);
                function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
            }
            g_484f68.PSFinalCombinerInputsABCD = 0x1d;
            g_484f68.PSFinalCombinerInputsEFG = 0;
        }
        else
        {
            g_484f68.PSFinalCombinerConstant1 = pack_color3f((color3f const *)(configuration + 8));
            g_484f68.PSFinalCombinerInputsABCD = 0x1d010000;
            g_484f68.PSFinalCombinerInputsEFG = 0x1d00;
        }
    }
    else if (mode == 3)
    {
        if (!skip_state) set_filter_blending(false);
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSConstant0[4] = pack_color3f((color3f const *)(configuration + 4));
        g_484f68.PSConstant1[4] = pack_color3f((color3f const *)(configuration + 0x10));
        long selector = configuration[0] ? 0x10 : 0;
        g_484f68.PSRGBInputs[4] = ((((9 + selector) << 16) | (10 + selector)) << 8) | 0x10002;
        g_484f68.PSRGBOutputs[4] = 0x3045;
        g_484f68.PSAlphaInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[4] = filter_output_shift(exponent, 1);
        g_484f68.PSConstant0[5] = pack_color3f((color3f const *)(configuration + 0x1c));
        g_484f68.PSRGBInputs[5] = ((11 + selector) << 8) | 0x24250001;
        g_484f68.PSRGBOutputs[5] = 0x1045;
        g_484f68.PSAlphaInputs[5] = 0x1d201d00 | (exponent > 6 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        long upper = filter_quantized_byte(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = filter_quantized_byte(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[6] = 0x4250000;
        g_484f68.PSRGBOutputs[6] = 0xd0;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_shift(exponent, 0);
        g_484f68.PSAlphaInputs[7] = 0x1d2d0000;
        g_484f68.PSAlphaOutputs[7] = 0xd0;
        g_484f68.PSFinalCombinerConstant1 = pack_color3f((color3f const *)(configuration + 0x28));
        g_484f68.PSFinalCombinerInputsABCD = 0x1d010000;
        g_484f68.PSFinalCombinerInputsEFG = 0x1d00;
    }
    function_1ccf0(&g_484f68);
    if (depth_out)
    {
        real depth = g_48565c * low;
        if (small) depth *= 0.00390625f;
        if (g_485658 >= depth)
        {
            if (!skip_state) function_0222d0(D3DRS_ZENABLE, 0);
            depth = g_485658;
        }
        *depth_out = depth;
    }
    return true;
}

dword g_4b8424;
void function_27aa0();
// @retail 0x25a10
void function_25a10(vector3f const *direction, point3f const *position, byte const *state)
{
    real constants[24];
    real dot = position->z * direction->k;
    dot += position->y * direction->j;
    dot += position->x * direction->i;
    real span = *(real const *)(state + 0x30) - *(real const *)(state + 0x2c);
    real first = 1.0f / (0.0001f > span ? 0.0001f : span);
    span = *(real const *)(state + 0x48) - *(real const *)(state + 0x44);
    real second = 1.0f / (0.0001f > span ? 0.0001f : span);
    real third = 1.0f / (0.0001f > *(real const *)(state + 0xb0) ? 0.0001f : *(real const *)(state + 0xb0));
    real fourth = 1.0f / (0.0001f > *(real const *)(state + 0xb4) ? 0.0001f : *(real const *)(state + 0xb4));
    constants[0] = direction->i * first;
    constants[1] = direction->j * first;
    constants[2] = direction->k * first;
    constants[3] = 0.0f - (*(real const *)(state + 0x2c) + dot) * first;
    constants[4] = 0.0f - *(real const *)(state + 0xf4) * fourth;
    constants[5] = 0.0f - *(real const *)(state + 0xf8) * fourth;
    constants[6] = 0.0f - *(real const *)(state + 0xfc) * fourth;
    constants[7] = *(real const *)(state + 0x100) * fourth;
    constants[8] = direction->i * third;
    constants[9] = direction->j * third;
    constants[10] = direction->k * third;
    constants[11] = 0.0f - (*(real const *)(state + 0x110) + dot) * third;
    real fade = 0.0f - *(real const *)(state + 0x108) * fourth;
    constants[12] = fade < 0.0f ? 0.0f : (fade > 1.0f ? 1.0f : fade);
    constants[13] = 0.0f;
    constants[14] = *(real const *)(state + 0x28);
    constants[15] = *(real const *)(state + 0xac);
    constants[16] = direction->i * second;
    constants[17] = direction->j * second;
    constants[18] = direction->k * second;
    constants[19] = 0.0f - (dot + *(real const *)(state + 0x44)) * second;
    constants[20] = *(real const *)(state + 0x40);
    constants[21] = 0.0f;
    constants[22] = 0.0f;
    constants[23] = 0.0f;
    D3DDevice_SetVertexShaderConstant(-84, constants, 6);
    g_4b8424 = pack_color3f((color3f const *)(state + 0x1c));
    D3DDevice_SetRenderState(D3DRS_FOGCOLOR, g_4b8424);
    function_27aa0();
}

extern vector3f g_485768, g_485780;
extern real g_485774, g_48578c, g_485868;
byte g_48575d;
extern dword g_4b8308, g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324, g_4b82ec, g_4b8448, g_4b843c, g_4b8438, g_4b8450;
dword __cdecl pack_color4f(color4f const *color);

// @retail 0x25e50
bool __stdcall function_25e50(void *context)
{
    if (g_48578c == 0.0f)
    {
        byte configuration[0x6c] = { 0 };
        *(vector3f *)(configuration + 0x1c) = g_485768;
        *(real *)(configuration + 0x28) = g_485774;
        *(color3f *)(configuration + 0x5c) = g_4857a8;
        *(real *)(configuration + 0x68) = g_4857b4;
        real depth = g_509400;
        bool filtered = function_3a5c0(configuration, g_485778, g_48577c, 1,
            false, false, false, false, &depth);
        g_509400 = depth;
        if (filtered)
            return true;
    }
    g_509400 = g_485ae0;
    function_14bc0((short)g_4858b8, 0, true);
    function_14f60(0, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x14), 0, 2, 0.0f);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(2, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(2, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);
    g_4b8308 = 0x1010101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    g_4b82f4 = 0x8001; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 0x8001);
    g_4b82f8 = 0x303; D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0x303);
    g_4b8324 = 0x8006; D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
    function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
    g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    g_4b8438 = g_48575d ? 0 : 2; D3DDevice_SetRenderState(D3DRS_ZENABLE, g_4b8438);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, g_48575d ? 0x207 : 0x204);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    color4f first, second, third;
    first.alpha = clamp_filter_unit((1.0f - g_485868) * g_48578c);
    first.red = clamp_filter_unit(g_485768.i * g_485774);
    first.green = clamp_filter_unit(g_485768.j * g_485774);
    first.blue = clamp_filter_unit(g_485768.k * g_485774);
    second.alpha = clamp_filter_unit(g_485774 * g_485868);
    second.red = clamp_filter_unit(g_485780.i * g_48578c);
    second.green = clamp_filter_unit(g_485780.j * g_48578c);
    second.blue = clamp_filter_unit(g_485780.k * g_48578c);
    real first_alpha = clamp_filter_unit(g_485774);
    real second_alpha = clamp_filter_unit(g_48578c);
    third.alpha = clamp_filter_unit(g_4857b4);
    third.red = clamp_filter_unit(g_4857a8.red);
    third.green = clamp_filter_unit(g_4857a8.green);
    third.blue = clamp_filter_unit(g_4857a8.blue);
    D3DPIXELSHADERDEF program;
    memset(&program, 0, sizeof(program));
    dword *fields = (dword *)&program;
    fields[0xd8 / 4] = 0x2621;
    fields[0xdc / 4] = 0x44;
    fields[0xd4 / 4] = 0x11004;
    fields[0x88 / 4] = 0x10a021a;
    fields[0xb4 / 4] = 0xcd;
    fields[0x0 / 4] = 0x111a120a;
    fields[0x68 / 4] = 0xcd;
    fields[0x6c / 4] = 0xcd;
    fields[0x70 / 4] = 0xcd;
    fields[0x8c / 4] = 0xc3c0d3d;
    fields[0xb8 / 4] = 0xc00;
    fields[0x4 / 4] = 0x110a121a;
    fields[0x8 / 4] = 0x3c3d0820;
    fields[0x94 / 4] = 0x3d0c1d01;
    fields[0xc0 / 4] = 0xc00;
    fields[0xc / 4] = 0x3d3c1d11;
    fields[0x74 / 4] = 0xc00;
    fields[0x20 / 4] = 0xc;
    fields[0x24 / 4] = 0x1c00;
    fields[0x28 / 4] = pack_color4f(&first);
    fields[0x48 / 4] = pack_color4f(&second);
    fields[0x2c / 4] = function_131fc0(first_alpha);
    fields[0x4c / 4] = function_131fc0(second_alpha);
    fields[0x34 / 4] = pack_color4f(&third);
    g_484f68 = program;
    function_1ccf0(&program);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

real g_4857d4;
extern dword g_4b8308, g_4b82e8, g_4b82f4, g_4b8448, g_4b843c, g_4b8438, g_4b8450;
dword __cdecl pack_color4f(color4f const *color);

// @retail 0x26e50
bool __stdcall function_26e50(void *context)
{
    union { byte configuration[0x20]; dword words[60]; } scratch;
    bool separate = !g_5093fe;
    if (!g_4b9d9c)
    {
        byte *configuration = scratch.configuration;
        *(real *)(configuration + 0) = g_4857d4;
        *(real *)(configuration + 4) = g_4857d8;
        *(color3f *)(configuration + 8) = g_4857bc;
        configuration[0x14] = separate;
        real depth = g_509400;
        bool filtered = function_3a5c0(configuration, g_4857dc, g_4857e0, 2,
            false, false, true, false, &depth);
        g_509400 = depth;
        if (filtered)
        {
            function_14f60(1, 19);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(1, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(1, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_ALPHAKILL, 0);
            return true;
        }
    }
    g_509400 = g_485ae0;
    function_14bc0(separate ? 3 : (short)g_4858b8, 0, false);
    function_14f60(0, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x14), 0, 2, 0.0f);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(2, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(2, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(2, D3DTSS_ALPHAKILL, 0);
    function_14f60(3, 19);
    D3DDevice_SetTextureStageState(3, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(3, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(3, D3DTSS_ADDRESSW, 3);
    D3DDevice_SetTextureStageState(3, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(3, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(3, D3DTSS_MIPFILTER, 0);
    D3DDevice_SetTextureStageState(3, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(3, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(3, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(3, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(3, D3DTSS_ALPHAKILL, 0);
    g_4b8308 = 0x1010101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    g_4b82f4 = 0x8001; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 0x8001);
    function_0222d0(D3DRS_DESTBLEND, 0x303);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    g_4b8438 = 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    real colors[3][4];
    colors[0][0] = g_4b9c74[1][5] < 0.0f ? 0.0f : (g_4b9c74[1][5] > 1.0f ? 1.0f : g_4b9c74[1][5]);
    colors[0][1] = g_4b9c74[2][5] < 0.0f ? 0.0f : (g_4b9c74[2][5] > 1.0f ? 1.0f : g_4b9c74[2][5]);
    colors[0][2] = g_4b9c74[3][5] < 0.0f ? 0.0f : (g_4b9c74[3][5] > 1.0f ? 1.0f : g_4b9c74[3][5]);
    colors[0][3] = g_4b9c74[4][5] < 0.0f ? 0.0f : (g_4b9c74[4][5] > 1.0f ? 1.0f : g_4b9c74[4][5]);
    colors[1][0] = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    colors[1][1] = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    colors[1][2] = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    colors[1][3] = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    colors[2][0] = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    colors[2][1] = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    colors[2][2] = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    colors[2][3] = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    dword *words = scratch.words;
    memset(words, 0, sizeof(scratch.words));
    D3DPIXELSHADERDEF const *program = (D3DPIXELSHADERDEF const *)words;
    dword *fields = words;
    fields[0xd8 / 4] = 0xa621;
    fields[0xdc / 4] = 0x44;
    fields[0xd4 / 4] = 0x11006;
    fields[0x0 / 4] = 0x1a20b120;
    fields[0x68 / 4] = 0x10c00;
    fields[0x88 / 4] = 0xa20a120;
    fields[0xb4 / 4] = 0x10c00;
    fields[0x8c / 4] = 0xb0c1b1c;
    fields[0xb8 / 4] = 0xcd;
    fields[0x8 / 4] = 0x2c2d0000;
    fields[0x70 / 4] = 0x40;
    fields[0x30 / 4] = 0xff0000;
    fields[0x50 / 4] = 0xff00;
    fields[0x90 / 4] = 0x10c020c;
    fields[0xbc / 4] = 0x3045;
    fields[0xc / 4] = 0x24251a1a;
    fields[0x74 / 4] = 0x58;
    fields[0x10 / 4] = 0x14150000;
    fields[0x78 / 4] = 0xc0;
    fields[0x98 / 4] = 0xa010a02;
    fields[0xc4 / 4] = 0xcd;
    fields[0x9c / 4] = 0x1c0c3c0d;
    fields[0xc8 / 4] = 0xc00;
    fields[0x28 / 4] = pack_color4f((color4f *)colors[0]);
    fields[0x38 / 4] = pack_color4f((color4f *)colors[1]);
    fields[0x58 / 4] = pack_color4f((color4f *)colors[2]);
    if (separate)
    {
        function_0222d0(D3DRS_COLORWRITEENABLE, 1);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
        fields[0x20 / 4] = 0xc;
        fields[0x24 / 4] = 1;
    }
    else
    {
        color3f color = g_4857bc;
        fields[0xac / 4] = pack_color3f(&color);
        fields[0x20 / 4] = 0xc010000;
        fields[0x24 / 4] = 0xc00;
    }
    g_484f68 = *program;
    function_1ccf0(program);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}
