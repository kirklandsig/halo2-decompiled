// @flags /O2 /arch:SSE /Gr
#include "cluster_mask_storage.h"
#include "function_context_storage.h"
#include "filter_buffer_storage.h"
#include "shader_storage.h"
#include "render_state_storage.h"
#include "saved_frames_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "globals.h"
#include <xmmintrin.h>
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "sampler_state.h"
#include "geometry_cache.h"
#include "unknown_234c64.h"

struct s_interface_function_values
{
    vector3f field_10;
    vector3f field_1c;
    real field_28, field_2c;
    vector3f field_30;
    vector3f field_3c;
    vector3f field_48;
    vector3f field_54;
};

struct s_interface_function_context
{
    long index;
    real (__stdcall *evaluate)(long context, long name);
    long field_8;
    byte unknown0c[4];
    s_interface_function_values values;
    short field_60;
    byte unknown62[0x12];
    bool field_74;
    byte unknown75;
    bool field_76, field_77;
};

// @retail 0x2c3b0
void function_2c3b0(s_interface_function_context *context)
{
    s_interface_function_values *values = &context->values;
    context->index = NONE;
    context->field_8 = NONE;
    context->field_74 = false;
    context->evaluate = NULL;
    context->field_77 = false;
    context->field_76 = false;
    values->field_10.i = 0.09f;
    values->field_30.i = 0.12f;
    values->field_48.i = 0.09f;
    values->field_10.j = 0.09f;
    values->field_30.j = 0.12f;
    values->field_48.j = 0.09f;
    values->field_10.k = 0.09f;
    values->field_30.k = 0.12f;
    values->field_48.k = 0.09f;
    context->values.field_28 = 0.4f;
    context->values.field_2c = 0.4f;
    values->field_3c.i = (real)(cos(0.0) * cos(-1.2566370964050293));
    values->field_3c.j = (real)(sin(0.0) * cos(-1.2566370964050293));
    values->field_3c.k = (real)sin(-1.2566370964050293);
    vector3f const *direction = &values->field_3c;
    vector3f *negative = &values->field_54;
    negative->i = 0.0f - direction->i;
    negative->j = 0.0f - direction->j;
    negative->k = 0.0f - direction->k;
    values->field_1c = values->field_3c;
    context->field_60 = NONE;
}



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
bool cluster_get_sky(long cluster_index, long *sky_index, vector3f *vector, bool *found);
void function_12e5e0(long cluster_index, byte *out, point3f const *position,
	vector3f const *forward, byte kind, bool enabled);
struct s_speed_result
{
	long type;
	real amount;
	s_speed_shake shake;
};
void function_154310(long index, s_speed_result *result);
long function_14de70(long local_player_index);
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
	struct location_indices { long cluster, leaf; bool invalid; long object; } location;
	s_speed_result speed;
	memcpy(&speed.shake, g_4686d4, sizeof(speed.shake));
	location.object = NONE;
	speed.amount = 0.0f;
	speed.type = 0;
	g_4ba050 = view->player_index;
	location.cluster = g_4ba054[view->player_index];
	location.leaf = g_4ba064[view->player_index];
	if (function_2b720(&view->position, &location.cluster, &location.leaf))
	{
		g_4ba054[view->player_index] = location.cluster;
		g_4ba064[view->player_index] = location.leaf;
		location.invalid = false;
	}
	else location.invalid = true;
	union { vector3f color; real bounds[4]; } sky;
	sky.color = *(vector3f const *)&g_4686cc->red;
	bool found = false;
	long sky_index;
	byte kind = cluster_get_sky(location.cluster, &sky_index, &sky.color, &found);
	byte geometry[0x120];
	function_12e5e0(location.cluster, geometry, &view->position, &view->forward, kind, true);
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
		long player = function_14de70(view->object_index);
		location.object = *(long *)((byte *)g_4e8c24->data + (player & 0xffff) * 0x21c + 0x24);
		function_154310(view->object_index, &speed);
	}
	if (view->scale > 0.0001f)
		function_2ba10((s_2f970_view const *)&view->position, location.cluster, view->player_index,
			view->camera, enabled, location.leaf, location.invalid, &sky.color, 0, view->object_index,
			location.object, kind, sky_index, geometry, false, NONE, view->lighting, 0, &speed);
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
		g_55eed8.object_index = NONE;
		g_55eed8.up = matrix.up;
		real caption = *(real const *)(g_4b9970 + 10);
		if (caption != 0.0f) g_55eed8.scale = caption * 0.017453292f;
		memcpy(g_55eed8.camera, &g_55eed8.position, sizeof(g_55eed8.camera));
		function_2b790(&g_55eed8, true);
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
extern long g_4ba048, g_4ba04c, g_4ba030;
extern dword g_4ba034;
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
void *function_23600(long tag, byte const *indices, long section_index, long object_index, long node_index)
{
	void *result = NULL;
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

/* whether the structure's lighting sample set exists and belongs to the
   current bsp */
static inline bool structure_has_matching_samples(void)
{
    byte const *structure = (byte const *)g_4e0344;
    return structure && *(long const *)(structure + 0x80) > 0 && g_4e0348 &&
        *(long const *)(*(byte const **)(structure + 0x84) + 0x1c) != NONE &&
        *(long const *)(*(byte const **)(structure + 0x84) + 4) == *(long const *)((byte *)g_4e0348 + 8);
}

// score 223 (was 252; earlier 252, was 237): b5J walk against retail (2026-11-06). Behaviour: the period test is `> 0` with the
// false path last (0x22adc comiss, 0x22ae0 jbe 0x22c1d), so a NaN period clears g_5093f4 (was: ran the update);
// rotation[5] and [7] use j*(k*complement) with k*complement shared with [2]/[6] (0x22a0e, 0x22a36), was (j*k)*complement.
// Structure (retail shape): reset loop indexed with a signed bound, stores in retail order (0x22860-0x22888);
// sample loop keeps the ebx offset and a countdown (0x22abf-0x22ace); samples, source and the structure pointer are re-read after the calls
// (0x22900, 0x22ab5, 0x22b06); axis fields are read from memory at each use (0x2292f-0x2294f); squares once.
// Remaining: frame 0x5c/0x68, ebp saved in the prologue (retail pushes it after the checks, 0x228f6), retail's
// second load of [edi+0x84] (0x228d9; not reproduced), sine_i on the x87 stack (0x229a1), and callee 0x143600
// has no source here so its argument registers differ. Retail makes three explicit coefficient calls with a copy
// back (0x22a6f, 0x22a8a, 0x22aa5), but written that way the full build moves the callers 0x12a50 from 153 to 178 and
// 0x2ca00 from 3 to 89, so the rolled loop stays. Tried, not kept: store g_5093f0 then clamp and store again (retail
// 0x22b29-0x22b36) 240; first checks through a second [edi+0x84] expression 239; (double) multiply for sine_i 244;
// cross_ji local 236; vector3f axis copy 255; `i < count` without the countdown 239.
// Trials on the earlier shape (rolled coefficient loop, axis copy):
// Earlier trials: frame 0x78 versus 0x68; axis/angle homes and rolled coefficient calls differ.
// Sample cursor 258; sine-scaled axis 259; diagonals first 288; elapsed snapshot 258; size optimization 397.
// frame 0x7c versus 0x68; axis/trigonometric homes and rotation-call layout differ.
// Complement at use 264; reused axis squares 252; coefficient countdown 273;
// contiguous payload reset 286; conditional phase clamp 258.
// Earlier trials: frame, axis/trigonometric homes and rotation-call layout remain;
// native coefficient array 258; payload clears first 257; late axis squares 258;
// reused mixed-axis products 243; entry result buffer 252; source pointer before reset 250.
// Earlier trials: Frame 0x80 versus 0x68, axis/trigonometric homes and rolled
// coefficient calls remain. squares between trig 256; paired mixed products 265; cosine
// before sine 242; scalar sine products 237; single sample exit 243; sines with squares
// between trig 272; sines with paired mixed products 242; sines with cosine before sine 239;
// short table field 243; sines with axis field reference 255; sines with angle field
// reference 257; sines with input field references 273.
// Frame 0x7c versus 0x68 and axis/trig/call homes remain; on reused square/mixed products: direct axis 300; direct angle 264; both 284; native coefficient index 257; geometry scalar scope 243. Reduced callee caveat; no keeper.
// Selective sine i/j/k at uses 245/235/243; pairs ij/ik/jk 243/249/243;
// selective mixed ji/ki/jk 248/248/231; sine-j with mixed ji/ki/jk 229/250/245.
// (sine-j and mixed-ji written out at their matrix uses gave 229; not kept: two of six parallel locals inlined.)
// Direct axis/angle/both on this product context 274/252/274 fast (callee caveat).
// Round 102 (m3 lane 9): retail reads the sample set pointer (+0x84) twice, once inside the admission test chain and
// again after it, the shape of an inlined test; the admission through the shared structure_has_matching_samples (also
// used by 0x23690) 252 -> 223 plain, kept. Left: frame 0x64 against 0x68 and 18 bytes short (969/987).
// @retail 0x22850
void function_22850(void)
{
    for (long entry_index = 0; entry_index < 24; ++entry_index)
    {
        s_table_entry *entry = &g_4b89b0[entry_index];
        entry->data_handle = NONE;
        *(short *)&entry->unknown4 = NONE;
        entry->key = NONE;
        memset(entry->data_offset, 0, sizeof(entry->data_offset));
        memset(entry->data_count, 0, sizeof(entry->data_count));
    }
    g_5093e8 = 0;
    if (!structure_has_matching_samples()) return;
    byte *source = *(byte **)((byte *)g_4e0344 + 0x84);
    long count = *(long *)(source + 0x38);
    if (count <= 0) return;
    for (long i = 0, remaining = count; remaining > 0; ++i)
    {
        byte *sample = *(byte **)(source + 0x3c) + i * 0xdc;
        if (*(real *)(sample + 0xa0) != 0.0f)
        {
            vector3f const *axis = (vector3f const *)(sample + 0x94);
            real sine = (real)sin(*(real *)(sample + 0xa0));
            real cosine = (real)cos(*(real *)(sample + 0xa0));
            real complement = 1.0f - cosine;
            real square_i = axis->i * axis->i;
            real square_j = axis->j * axis->j;
            real square_k = axis->k * axis->k;
            real k_complement = axis->k * complement;
            real rotation[9];
            rotation[0] = (1.0f - square_i) * cosine + square_i;
            rotation[1] = axis->j * complement * axis->i + axis->k * sine;
            rotation[2] = k_complement * axis->i - axis->j * sine;
            rotation[3] = axis->j * complement * axis->i - axis->k * sine;
            rotation[4] = (1.0f - square_j) * cosine + square_j;
            rotation[5] = k_complement * axis->j + axis->i * sine;
            rotation[6] = k_complement * axis->i + axis->j * sine;
            rotation[7] = k_complement * axis->j - axis->i * sine;
            rotation[8] = (1.0f - square_k) * cosine + square_k;
            for (real *coefficients = (real *)(sample + 0xc);
                coefficients != (real *)(sample + 0x78); coefficients += 9)
            {
                real result[9];
                function_143600(rotation, 3, coefficients, result);
                memcpy(coefficients, result, sizeof(result));
            }
        }
        --remaining;
    }
    if (*(real const *)((byte *)g_4e0344 + 0x34) > 0.0f)
    {
        real previous = g_5093ec;
        g_5093ec = (real)g_4858a0;
        real phase = (g_5093ec - previous) / *(real const *)((byte *)g_4e0344 + 0x34) + g_5093f0;
        phase -= real_truncate(phase);
        g_5093f4 = true;
        if (phase < 0.0f) phase = 0.0f;
        g_5093f0 = phase;
        if (count == 1)
        {
            byte *samples = *(byte **)(source + 0x3c);
            memcpy(g_4b8940, samples + 0xc, sizeof(g_4b8940));
            memcpy(g_4b8964, samples + 0x30, sizeof(g_4b8964));
            memcpy(g_4b8988, samples + 0x54, sizeof(g_4b8988));
        }
        else
        {
            real step = 1.0f / (count - 1);
            real inverse = 1.0f / step;
            long index = real_truncate(phase * inverse);
            real fraction = (phase - index * step) * inverse;
            byte *sample = *(byte **)(source + 0x3c) + index * 0xdc;
            function_022750((real *)(sample + 0xe8), 3, (real *)(sample + 0xc), g_4b8940, fraction);
            function_022750((real *)(sample + 0x10c), 3, (real *)(sample + 0x30), g_4b8964, fraction);
            function_022750((real *)(sample + 0x130), 3, (real *)(sample + 0x54), g_4b8988, fraction);
        }
    }
    else
        g_5093f4 = false;
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
	byte const &index)
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
	long y = 48;
	do
	{
		for (long x = 0; x < 64; x++)
			histogram_add(histogram, (byte)*pixels++);
		pixels += stride - 64;
	} while (--y);
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

#include "filter_kernel_storage.h"
void function_350e0(real const *a, real const *b, real *out, real x);

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
		function_350e0(bounds, t, out, g_509400);
		break;
	case 2:
		{
			real span = g_48577c - g_485778;
			real scale = 1.0f / (0.0001f > span ? 0.0001f : span);
			out[0] = g_48565c * scale;
			out[1] = 0.0f;
			out[2] = 0.0f - g_485778 * scale;
			out[3] = 0.0f;
			break;
		}
	case 3:
		{
			real span = g_485794 - g_485790;
			real scale = 1.0f / (0.0001f > span ? 0.0001f : span);
			out[0] = g_48565c * scale;
			out[1] = 0.0f;
			out[2] = 0.0f - g_485790 * scale;
			out[3] = 0.0f;
			break;
		}
	case 1: case 4:
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		out[2] = 0.0f;
		out[3] = 1.0f;
		break;
	default: return false;
	}
	return true;
}

// @retail 0x26ca0
bool __stdcall function_26ca0(long mode, real const *bounds, real const *t, real *out, long unused)
{
	real const *matrix = g_4b9c74[mode];
	if (g_509400 <= 0.0f)
		g_509400 = g_485ae0;
	switch (mode)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	{
		out[0] = (t[0] * 2.0f - 1.0f) * matrix[6] - (t[1] * 2.0f - 1.0f) * matrix[7] * g_4b9d94 + matrix[9];
		out[1] = (t[0] * 2.0f - 1.0f) * matrix[10] - (t[1] * 2.0f - 1.0f) * matrix[11] * g_4b9d94 + matrix[13];
		out[2] = 0.0f;
		out[3] = 1.0f;
		return true;
	}
	case 0:
	{
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		clip_depth_terms(out, 1.0f);
		out[0] *= g_4b9d90;
		out[1] *= g_4b9d90;
		return true;
	}
	}
	return false;
}

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
	real x = g_509400;
	switch (mode)
	{
	case 0:
		out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
		out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
		{
		s_range const *range = &g_485ad4;
		real scale = range->hi / (range->hi - range->lo);
		real depth = scale * x;
		depth -= range->lo * scale;
		depth = depth / x * 16777215.0f;
		if (0.0f > depth) depth = 0.0f;
		else if (depth > 16777215.0f) depth = 16777215.0f;
		out[2] = depth;
		real reciprocal = x / range->hi * 16777215.0f;
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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// @retail 0x24490
void *function_24490(long key, s_24490_definition *definition, long entry_key)
{
	long index = find_table_entry(key);
	if (index == NONE)
		return NULL;
	long level = (short)g_4b89b0[index].unknown4;
	long group = MIN(5 - level, definition->count - 1);
	s_24490_group *block = &definition->groups[group];
	for (long i = 0; i < block->count; i++)
	{
		s_24490_entry *entry = &block->entries[i];
		if (entry->key == entry_key)
		{
			long count = entry->count / 5;
			if (!function_12de70(&definition->block, 3))
				return NULL;
			byte *data = definition->data;
			long size = data[0] == 0x39 ? count * 10 : count * 20;
			*(long *)(data + 4) = size;
			*(long *)(data + 8) = size;
			return data;
		}
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
PRIVATE __forceinline real block_dot_product9(real const *a, real const *b)
{
	real result;
	__asm
	{
		mov ecx, a
		mov edx, b
		movlps xmm0, qword ptr [ecx]
		movlps xmm1, qword ptr [edx]
		movlps xmm2, qword ptr [ecx + 0x10]
		movlps xmm3, qword ptr [edx + 0x10]
		movhps xmm0, qword ptr [ecx + 8]
		movhps xmm1, qword ptr [edx + 8]
		movhps xmm2, qword ptr [ecx + 0x18]
		movhps xmm3, qword ptr [edx + 0x18]
		mulps xmm0, xmm1
		mulps xmm2, xmm3
		addps xmm0, xmm2
		movss xmm1, dword ptr [ecx + 0x20]
		movhlps xmm2, xmm0
		mulss xmm1, dword ptr [edx + 0x20]
		addps xmm0, xmm2
		addss xmm1, xmm0
		shufps xmm0, xmm0, 0x55
		addss xmm0, xmm1
		movss result, xmm0
	}
	return result;
}

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
	for (long i = 0; i < layout->groups; i++)
	{
		real dot0 = block_dot_product9(data + stride * i, vector0);
		real dot1 = block_dot_product9(data + stride * i + block_size, vector1);
		real dot2 = block_dot_product9(data + stride * i + block_size * 2, vector2);
		out[(layout->columns * 3 + 4) * i] = dot0;
		out[(layout->columns * 3 + 4) * i + 1] = dot1;
		out[(layout->columns * 3 + 4) * i + 2] = dot2;
		real *row = out + (layout->columns * 3 + 4) * i + 4;
		for (long j = 0; j < layout->columns; j++)
		{
			real cell_dot0 = block_dot_product9(data + stride * i + group_size * (j + 1), vector0);
			real cell_dot1 = block_dot_product9(data + stride * i + group_size * (j + 1) + block_size, vector1);
			real cell_dot2 = block_dot_product9(data + stride * i + group_size * (j + 1) + block_size * 2, vector2);
			row[j] = cell_dot0;
			row[layout->columns + j] = cell_dot1;
			row[layout->columns * 2 + j] = cell_dot2;
		}
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

// @retail 0x27520
bool __stdcall function_27520(long mode, real const *bounds, real const *t, real *out, long unused)
{
    if (g_509400 <= 0.0f)
        g_509400 = g_485ae0;
    switch (mode)
    {
    case 0:
        function_350e0(bounds, t, out, g_509400);
        return true;
    case 1:
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        out[2] = 0.0f;
        out[3] = 1.0f;
        return true;
    case 2:
        if (g_4b9d9c)
        {
            real end = g_4857e0;
            real begin = g_4857dc;
            real scale = 1.0f / (0.0001f > end - begin ? 0.0001f : g_4857e0 - g_4857dc);
            out[0] = g_48565c * scale;
            out[1] = 0.0f;
            out[2] = 0.0f - g_4857dc * scale;
            out[3] = 0.0f;
            return true;
        }
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        out[2] = 0.0f;
        out[3] = 1.0f;
        out[0] *= g_4b9d90;
        out[1] *= g_4b9d90;
        return true;
    case 3:
        {
            real end = g_4857e0;
            real begin = g_4857dc;
            real scale = 1.0f / (0.0001f > end - begin ? 0.0001f : g_4857e0 - g_4857dc);
            out[0] = g_48565c * scale;
            out[1] = 0.0f;
            out[2] = 0.0f - g_4857dc * scale;
            out[3] = 0.0f;
            return true;
        }
    case 4:
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

// score 12 fast (was 16 fast, 13 full): only the mode 1 exit differs: retail
// tests `jne` to the shared false exit and falls into its own `mov al, 1`
// return, ours jumps back to case 0's identical return and falls into the false
// exit. The fraction multiplies last only when it is written first (g*s*f and
// s*g*f compile as g*f*s, 16). Tried: a result flag for the whole switch 154;
// `if (mode == 1) return true` 16; `return mode == 1` 20; break to a return
// false after the switch 18; one shared `return true` after the strength block
// 16; the fraction inlined 73 or declared before the strength 26. The old
// `(void)&` parameter line changed nothing (the marker keeps them on the stack).
// @retail 0x298f0 standard
bool __stdcall function_298f0(long mode, real const *bounds, real const *t, real *out, long unused)
{
    if (g_509400 <= 0.0f)
        g_509400 = g_485ae0;
    switch (mode)
    {
    case 0:
        function_350e0(bounds, t, out, g_509400);
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
                real x = fraction * (*(real *)(g_4b99b0 + 0x1c) * strength) + 1.0f;
                real y = fraction * (*(real *)(g_4b99b0 + 0x20) * strength) + 1.0f;
                real inverse_x = x > 0.0f ? 1.0f / x : 0.0f;
                real inverse_y = y > 0.0f ? 1.0f / y : 0.0f;
                out[0] = (bounds[1] - bounds[0]) * (1.0f - inverse_x) * 0.5f + out[0] * inverse_x;
                out[1] = (bounds[3] - bounds[2]) * (1.0f - inverse_y) * 0.5f + out[1] * inverse_y;
                return true;
            }
            if (mode != 1) goto failed;
            return true;
        }
    default:
    failed:
        return false;
    }
}

extern D3DPIXELSHADERDEF g_484f68;
extern long g_4858b8;
extern byte *g_485a80;
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

void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value);

// @retail 0x2b000
bool __stdcall function_2b000(void *context)
{
    g_509400 = g_485ae0;
    function_14bc0(16, 0, false);
    function_14f60(0, (short)g_4858b8);
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
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

// @retail 0x296c0 standard
bool __stdcall function_296c0(long context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1])
        return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    function_14f60(0, *(short *)(g_4b99b0 + 0x34));
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
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

dword __cdecl pack_color3f(color3f const *color);

// @retail 0x276d0
bool __stdcall function_276d0(void *context)
{
    g_509400 = g_485ae0;
    function_14bc0((short)g_4858b8, 0, false);
    function_14f60(0, 3);
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);

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

extern dword g_4b8438;

// @retail 0x29080 standard
bool __stdcall function_29080(long context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1])
        return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    function_14f60(0, (short)(g_4b99b0[1] ? *(long *)(g_4b99b0 + 0x38) : 0));
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);

    if (g_4b99b0[1])
    {
        function_14f60(2, 0);
        sampler_state_write(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        sampler_state_write(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        sampler_state_write(2, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        sampler_state_write(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        sampler_state_write(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        sampler_state_write(2, D3DTSS_MIPFILTER, 0);
        sampler_state_write(2, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(2, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(2, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(2, D3DTSS_COLORSIGN, 0);
        sampler_state_write(2, D3DTSS_ALPHAKILL, 0);

    }
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, g_4b99b0[0]);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_ONE);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_CONSTANTCOLOR);
    color3f const &blend = *(color3f const *)(g_4b99b0 + 0x10);
    function_0222d0(D3DRS_BLENDCOLOR, pack_color3f(&blend));
    function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    g_4b8438 = 0;
#pragma inline_depth(0)
    D3DDevice_SetRenderState_ZEnable(0);
#pragma inline_depth(255)
    function_0222d0(D3DRS_ZBIAS, 0);
    color3f const &color = *(color3f const *)(g_4b99b0 + 4);
    dword constant;
    if (g_4b99b0[1])
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x2621;
        g_484f68.PSDotMapping = 0x11;
        g_484f68.PSCombinerCount = 0x11001;
        constant = pack_color3f(&color);
        g_484f68.PSFinalCombinerInputsABCD = 0xa010000;
        g_484f68.PSFinalCombinerConstant0 = constant;
        g_484f68.PSFinalCombinerInputsEFG = 0;
        function_1ccf0(&g_484f68);
    }
    else
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        constant = pack_color3f(&color);
        g_484f68.PSFinalCombinerInputsABCD = 0x8010000;
        g_484f68.PSFinalCombinerConstant0 = constant;
        g_484f68.PSFinalCombinerInputsEFG = 0;
        function_1ccf0(&g_484f68);
    }
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
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
    if (!context)
    {
        function_14bc0(1, 0, true);
        function_14f60(0, g_4858b8);
        function_14f60(1, 15);
        function_14f60(2, 15);
        sampler_state_write(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        sampler_state_write(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        sampler_state_write(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        sampler_state_write(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        sampler_state_write(1, D3DTSS_MIPFILTER, 0);
        sampler_state_write(1, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(1, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(1, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(1, D3DTSS_COLORSIGN, 0);
        sampler_state_write(1, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
#pragma inline_depth(0)
        function_0224f0(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        function_0224f0(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        function_0224f0(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        function_0224f0(2, D3DTSS_MIPFILTER, 0);
        function_0224f0(2, D3DTSS_MAXANISOTROPY, 0);
        function_0224f0(2, D3DTSS_MIPMAPLODBIAS, 0);
        function_0224f0(2, D3DTSS_MAXMIPLEVEL, 0);
        function_0224f0(2, D3DTSS_COLORSIGN, 0);
        function_0224f0(2, D3DTSS_ALPHAKILL, 0);
#pragma inline_depth(255)
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x41e1;
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSConstant0[0] = 0xff0000;
        g_484f68.PSConstant1[0] = 0xff00;
        g_484f68.PSRGBInputs[0] = 0x9010a02;
        g_484f68.PSRGBOutputs[0] = 0xc00;
        g_484f68.PSConstant1[1] = 0xff;
        g_484f68.PSRGBInputs[1] = 0xc200a02;
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


/* the two colours after the filter buffer's flags */
struct s_filter_colors
{
    byte flags[4];
    color3f color;
    color3f blend;
};

// @retail 0x29370 standard
bool __stdcall function_29370(long context)
{
    if (!g_4b99b0[0] && !g_4b99b0[1]) return false;
    g_509400 = g_485ae0;
    function_14bc0(*(short *)(g_4b99b0 + 0x44), 0, false);
    for (long stage = 0; stage < 4; stage++)
    {
        function_14f60((short)stage, *(short *)(g_4b99b0 + 0x38));
        sampler_state_write(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        sampler_state_write(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        sampler_state_write(stage, D3DTSS_MIPFILTER, 0);
        sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
        sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
    }
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_ONE);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_CONSTANTCOLOR);
    function_0222d0(D3DRS_BLENDCOLOR, pack_color3f(&((s_filter_colors const *)&g_filter_buffer_storage)->blend));
    function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSRGBOutputs[1] = 0x30d00;
    g_484f68.PSRGBOutputs[2] = 0x30d00;
    g_484f68.PSTextureModes = 0x8421;
    g_484f68.PSCombinerCount = 0x11003;
    g_484f68.PSRGBInputs[0] = 0x08200920;
    g_484f68.PSRGBOutputs[0] = 0x30c00;
    g_484f68.PSRGBInputs[1] = 0x0a200b20;
    g_484f68.PSRGBInputs[2] = 0x0c200d20;
    g_484f68.PSFinalCombinerInputsABCD = 0xd;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}


extern byte g_4670bc;
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
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(1, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(2, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(3, D3DTSS_ALPHAKILL, 0);
        function_1cf50();
        function_1c710(g_51f0f0);
        for (long row = 0; row < rows; ++row)
        {
            D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
            for (long column = 0; column <= columns; ++column)
            {
                for (long side = 0; side < 2; ++side)
                {
                    for (long attribute = 15; attribute >= 0; --attribute)
                    {
                        real out[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                        real t[2] = { (real)column / columns, (real)(row + side) / rows };
                        if (vertex(attribute, bounds, t, out, user))
                            D3DDevice_SetVertexData4f(!attribute ? D3DVSDE_VERTEX : attribute,
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
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(1, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(2, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(3, D3DTSS_ALPHAKILL, 0);
        function_1cf50();
        function_1c710(g_51f0f0);
        D3DDevice_Begin(D3DPT_TRIANGLEFAN);
        real *t = corners;
        long remaining = 4;
        do
        {
            for (long attribute = 15; attribute >= 0; --attribute)
            {
                real out[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                if (vertex(attribute, bounds, t, out, user))
                    D3DDevice_SetVertexData4f(!attribute ? D3DVSDE_VERTEX : attribute,
                        out[0], out[1], out[2], out[3]);
            }
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
    real depth_x = g_509400;
    switch (mode)
    {
    case 0:
        out[0] = (bounds[1] - bounds[0]) * t[0] + bounds[0];
        out[1] = (bounds[3] - bounds[2]) * t[1] + bounds[2];
        {
            s_range const *range = &g_485ad4;
            real depth_scale = range->hi / (range->hi - range->lo);
            real depth_value = (depth_scale * depth_x - range->lo * depth_scale) / depth_x * 16777215.0f;
            if (0.0f > depth_value) depth_value = 0.0f;
            else if (depth_value > 16777215.0f) depth_value = 16777215.0f;
            out[2] = depth_value;
            real depth_weight = depth_x / range->hi;
            depth_weight = depth_weight * 16777215.0f;
            if (0.0f > depth_weight) depth_weight = 0.0f;
            else if (depth_weight > 16777215.0f) depth_weight = 16777215.0f;
            out[3] = depth_weight;
        }
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
    bounds[2] = 0.0f;
    bounds[1] = 64.0f;
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

byte g_5093fd, g_5093fe;
bool function_143c0(long tag, short index, short stage, real priority);

/* the filter rows: an index word, then a colour per row */
dword function_1cbe0(long mode, dword index, long slot_index);

struct s_filter_row
{
    real index;
    real unknown04;
    color3f color;
    real unknown14[9];
};

struct s_filter_rows
{
    s_filter_row header, first, second, third, fourth;
};

// @retail 0x26880
bool __stdcall function_26880(void *context)
{
    function_14bc0(19, 0, false);
    byte *definition = g_4e3b44[g_4857b8 & 0xffff].bytes;
    for (long stage = 0; stage < 4; ++stage)
    {
        real const &index_bits = g_4b9c74[stage + 1][0];
        function_143c0(*(long *)(definition + 0x18), *(short *)&index_bits, (short)stage, 0.0f);
        sampler_state_write(stage, D3DTSS_ADDRESSU, 1);
        sampler_state_write(stage, D3DTSS_ADDRESSV, 1);
        sampler_state_write(stage, D3DTSS_ADDRESSW, 1);
        sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
        sampler_state_write(stage, D3DTSS_MINFILTER, 2);
        sampler_state_write(stage, D3DTSS_MIPFILTER, 0);
        sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
        sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
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
    if (!g_4b9d9c)
    {
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSConstant0[0] = pack_color3f(&((s_filter_rows const *)&g_4b9c74)->first.color);
#pragma inline_depth(0)
        g_484f68.PSConstant1[0] = pack_color3f((color3f const *)(g_4b9c74[2] + 2));
        g_484f68.PSConstant0[1] = pack_color3f((color3f const *)(g_4b9c74[3] + 2));
        g_484f68.PSConstant1[1] = pack_color3f((color3f const *)(g_4b9c74[4] + 2));
#pragma inline_depth(255)
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
        g_484f68.PSConstant0[0] = pack_color3f(&((s_filter_rows const *)&g_4b9c74)->first.color);
#pragma inline_depth(0)
        g_484f68.PSConstant1[0] = pack_color3f((color3f const *)(g_4b9c74[2] + 2));
        g_484f68.PSConstant0[1] = pack_color3f((color3f const *)(g_4b9c74[3] + 2));
        g_484f68.PSConstant1[1] = pack_color3f((color3f const *)(g_4b9c74[4] + 2));
#pragma inline_depth(255)
        g_484f68.PSConstant0[2] = 0xff0000;
        g_484f68.PSConstant1[2] = 0xff00;
        g_484f68.PSRGBInputs[2] = 0x9010a02;
        g_484f68.PSRGBOutputs[2] = 0xc00;
        g_484f68.PSFinalCombinerConstant0 = 0xff;
        g_484f68.PSFinalCombinerInputsABCD = 0xb01000c;
        g_484f68.PSFinalCombinerInputsEFG = 0x800;
    }
    g_484f68.PSRGBInputs[0] = 0x8010902;
    g_484f68.PSRGBOutputs[0] = 0x3089;
    g_484f68.PSRGBInputs[1] = 0xa010b02;
    g_484f68.PSRGBOutputs[1] = 0x30ab;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1cbe0(0, 0, 12);
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

/* the surface's texture header, or NULL while it has no storage */
static __forceinline void *surface_texture_get(long index)
{
    s_unknown_01dcc0 *entry = &g_4b4b58[index];
    void *result = NULL;
    if (entry->data && !entry->flag95)
        result = entry->sub_header;
    return result;
}

// @retail 0x24ee0
void function_24ee0(s_24ee0_state const *state, short mode)
{
    D3DTexture *texture = (D3DTexture *)surface_texture_get(15);
    texture->Size = 0;
    texture->Format = 0x06610629;
    long index = state->index;
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
        byte mapped = (byte)(long)(result * 255.0f + 0.5f);
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
        D3DLOCKED_RECT rectangle;
        D3DTexture_LockRect(texture, 0, &rectangle, NULL, 0);
        for (long y = 0; y < 64; ++y)
        {
            byte component = curve[y * 4];
            for (long x = 0; x < 64; ++x)
            {
                ((dword *)rectangle.pBits)[function_024e50((word)y, (word)x)] = D3DCOLOR_XRGB(component, curve[x * 4], component);
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

#include "texture_stage_dimensions.h"
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

PRIVATE __forceinline void configure_34a90_state(long target, long blend,
    dword color_write, bool use_depth, bool depth_write, real depth)
{
    if (target != NONE) function_14bc0((short)target, 0, use_depth);
    if (blend != NONE) function_142f0((short)blend);
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
    function_1cbe0(0, 0, 12);
}

// @retail 0x34a90
void __stdcall function_34a90(long target, long blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real distortion, real scale,
    long count, bool full_surface, bool viewport_textures)
{
    long width = 0;
    long height = 0;
    if (target != NONE)
        function_01dd60(target, &width, &height);
    else
        function_01dd60(g_4858b8, &width, &height);
    real bounds[4];
    if (full_surface)
    {
        bounds[0] = 0.0f;
        bounds[2] = 0.0f;
        bounds[1] = (real)width;
        bounds[3] = (real)height;
    }
    else
    {
        bounds[0] = (real)g_4b9dd2;
        bounds[2] = (real)g_4b9dd0;
        bounds[1] = (real)g_4b9dd6;
        bounds[3] = (real)g_4b9dd4;
    }
    real inverse_x = 1.0f / width;
    real inverse_y = 1.0f / height;
    s_34a90_parameters parameters;
    parameters.depth = depth >= 0.0f ? depth : 0.0f - depth;
    parameters.distortion = distortion;
    for (long i = 0; i < count; ++i)
    {
        long texture_width, texture_height;
        if (viewport_textures)
        {
            texture_width = g_4b9dd6 - g_4b9dd2;
            texture_height = g_4b9dd4 - g_4b9dd0;
            parameters.offsets[i].x = (real)g_4b9dd2;
            parameters.offsets[i].y = (real)g_4b9dd0;
        }
        else
        {
            parameters.offsets[i].x = 0.0f;
            parameters.offsets[i].y = 0.0f;
            texture_width = g_485af4[i];
            texture_height = g_485b04[i];
        }
        parameters.scales[i].x = texture_width * inverse_x;
        parameters.scales[i].y = texture_height * inverse_y;
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
void function_34d90(long target, real const *weights, long blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real low, real high,
    bool full_surface, t_34770_vertex vertex)
{
    long width = 0, height = 0;
    if (target != NONE)
        function_01dd60(target, &width, &height);
    else
        function_01dd60(g_4858b8, &width, &height);
    real bounds[4];
    if (full_surface)
    {
        bounds[0] = 0.0f;
        bounds[2] = 0.0f;
        bounds[1] = (real)width;
        bounds[3] = (real)height;
    }
    else
    {
        bounds[0] = (real)g_4b9dd2;
        bounds[2] = (real)g_4b9dd0;
        bounds[1] = (real)g_4b9dd6;
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
    t_34770_vertex chosen = vertex ? vertex : (t_34770_vertex)function_34100;
    function_34770(NULL, bounds, chosen, NULL, 20, 30, (long)&parameters);
}

struct s_245c0_state
{
    s_24ee0_state curve;
    dword frame_count;
};

/* queues the histogram read-back on the push buffer */
static __forceinline void histogram_callback_insert(s_245c0_state *state)
{
    D3DDevice_InsertCallback(D3DCALLBACK_WRITE, (D3DCALLBACK)function_24970, (dword)state);
}

__declspec(noinline) void function_245c0(s_245c0_state *state, real *histogram, short mode);
// @retail 0x245c0
void function_245c0(s_245c0_state *state, real *histogram, short mode)
{
    state->curve.histogram = histogram;
    switch (mode)
    {
    case 1:
    case 2:
    case 3:
        if (!(state->frame_count & 7))
        {
            function_249f0();
            function_14bc0((short)g_4858b8, 0, true);
            histogram_callback_insert(state);
        }
        break;
    default:
        state->curve.range.low = 0.0f;
        state->curve.range.high = 0.75f;
        state->curve.range.average = 0.4f;
        state->curve.range.smoothed_low = 0.0f;
        state->curve.range.smoothed_high = 0.75f;
        state->curve.range.smoothed_average = 0.4f;
        state->frame_count = 0;
        break;
    }
    if (mode == 2 || !(state->frame_count & 7))
        function_24ee0(&state->curve, mode);
    ++state->frame_count;
}

PRIVATE __forceinline void configure_351a0_texture(long stage, D3DTEXTUREFILTERTYPE filter)
{
    sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
    sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
    sampler_state_write(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    sampler_state_write(stage, D3DTSS_MAGFILTER, filter);
    sampler_state_write(stage, D3DTSS_MINFILTER, filter);
    sampler_state_write(stage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
}

// @retail 0x351a0
void __stdcall function_351a0(long texture, long target)
{
    function_14bc0((short)target, 0, true);
    function_14f60(0, (short)texture);
    configure_351a0_texture(0, D3DTEXF_POINT);
#pragma inline_depth(0)
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
#pragma inline_depth(255)
#pragma inline_depth(0)
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
#pragma inline_depth(255)
    g_4b8450 = 0;
    D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    g_4b8448 = 0;
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
#pragma inline_depth(0)
    function_0222d0(D3DRS_STENCILENABLE, 0);
#pragma inline_depth(255)
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
    bool const *multiple_reference = &multiple;
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    if (target == NONE) target = 19;
    for (long stage = 0; stage < count; ++stage)
    {
        function_14f60((short)stage, (short)*(long const *)&g_4858b8);
        configure_351a0_texture(stage, D3DTEXF_LINEAR);
    }
    memset(&g_484f68, 0, sizeof(g_484f68));
    dword output;
    if (*multiple_reference)
    {
        output = 0x30c00;
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSAlphaInputs[0] = 0x18201920;
        g_484f68.PSAlphaInputs[1] = 0x1a201b20;
        g_484f68.PSAlphaOutputs[1] = 0x30d00;
        g_484f68.PSAlphaInputs[2] = 0x1c201d20;
        g_484f68.PSAlphaOutputs[2] = 0x30c00;
        g_484f68.PSRGBInputs[0] = 0x8200920;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[2] = 0x30c00;
    }
    else
    {
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSAlphaInputs[0] = 0x18200000;
        output = 0xc0;
        g_484f68.PSRGBInputs[0] = 0x8200000;
    }
    g_484f68.PSAlphaOutputs[0] = output;
    g_484f68.PSRGBOutputs[0] = output;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_34a90(target, 10, 0x1010101, false, false, 1.0f,
        *multiple_reference ? 0.5f : 0.0f, 1.0f, count, true, true);
}

void function_144f0(short stage, long fallback, long tag, short fallback_index, short index, real priority);
void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value);
dword function_1cc30(long index);
dword __cdecl function_131fc0(real alpha);

// MiniMax B (11-13): no new form, no lower score (140): re-measured with the plain build (1729 bytes, as retail, first difference at +0x32b); what is left is the preserve_alpha load hoisted above the simple branch.
// score 140 (was 140): frame and length agree; left: retail keeps both rectangle parameters as memory
// variables (each chosen default is written into the parameter's home, `lea ecx; mov [home], ecx; mov ebx, ecx`)
// and re-reads preserve_alpha after 0x131fc0 where ours hoists its load above the `simple` branch into CL.
// Round 176 (m3 lane 5, fast = plain): box2f views of the rectangles (diagnostic cast) 154; PSTextureModes set
// at the top of each shader arm 151 (1735 B, the load still hoisted); one two-row default array 140 (same
// code); the final combiner's alpha in its own locals 148 (1725 B); the bitmap lookup nested under the tag
// test 140 (same code). Re-measured: aggregate-initialised defaults selected after both are built 156
// (1717 B), selected by ternaries into pointer locals 156, that with the height's reciprocal first 162.
// MiniMax H (11-12): no new form, no lower score (140): the `simple` byte is still tested before the texture-mode global is stored where retail stores it first, the `preserve_alpha` byte is still loaded above that store and kept in cl, and retail's else arm still tests a byte one slot past the last argument where ours reuses the earlier load.
// MiniMax I (11-11): no new form, no lower score (140): retail tests the `simple` byte, stores the texture-mode global and only then loads and tests the `preserve_alpha` byte, where our build hoists that load above the store and keeps it in cl; retail's else arm tests a byte one slot past the last argument (the stack slot the caller leaves behind) where ours reuses the earlier load.
// MiniMax G (11-12): no new form tried this round, no lower score (140): the byte at esp+0x6c is still read in a
// different register and retail's short arm still reads a stack argument this function's declaration does not carry.
// Earlier 140 (was 140): Frame and retail extent agree; caller flag residency and native
// fallback selection remain. Both complete coverage clamps now follow the retail upper-default stores.
// MiniMax S (11-11): no new form, no lower score (140): the frame and the extent agree with retail, and what is
// left is the byte at esp+0x6c, which retail reads in AL after the store into the global while ours keeps it in CL
// across the branch, and the two default rectangles, which retail builds unconditionally and writes back into the
// parameters' argument slots while ours branches over the block; retail's short arm reads a fourteenth stack argument
// that this function's own declaration does not carry, and the declaration may not change in this lane:
// BLOCKED-BY this function's parameter list.
// MiniMax D (11-10): no new form, no lower score; retail's 0x2a8d0 reads a fourteenth stack
// argument, the byte at esp+0x70, in the short arm where this source reads preserve_alpha, and
// because that argument is a different value the second flag dies at the test, so retail loads it
// into al after the store into the global while this build keeps it in cl across the branch. The
// declaration may not be changed in this lane and the callers are in src/unknown_0350e0.cpp, so
// retail's parameter list is reported here instead: tag, bitmap_index, source, destination, mask,
// color, strength, blend, inverse, alternate_target, screen_target, simple, preserve_alpha and one
// more byte argument after them, read only by the short arm. This round: the arms swapped
// (simple || preserve_alpha first) give 140 and 1729 bytes, the same code as before, and the short
// arm reading simple instead of preserve_alpha gives 138, a gain of two that is under the keep
// threshold and changes what the arm computes, so the body stands.
// MiniMax S (11-09): no new form, no lower score (140); the round's `(simple != 0 ? 0 : 0x18) << 8`
// spelling gave 138, but a gain under the keep threshold is dropped, so the body is restored;
// caller flag residency and native fallback selection remain.
// MiniMax E (11-09): no new form, no lower score (140); retail loads the byte at esp+0x6c after the branch on the byte at esp+0x68, ours hoists that load above the store into the global.
// MiniMax E (11-06): 140 (was 140): no new form lowered it. The inlined corner rotation still
// expands with the opposite operand order and stores its results in different homes than retail, and
// the byte local at esp+0x6c is read in AL where ours reads CL.
// MiniMax D (11-01): no new form tried, no lower score.
// MiniMax G (11-04, not kept, m3 review: the destination default moved inside the source-null block, so a null
// destination with a non-null source read an unset rectangle; a behaviour change): 132 (was 140) with the two default rectangles built in retail's
// store order (source[3] first, then the 320-based pair, the 240-based pair and the last 320-based
// one) inside the source-null block, which matches retail's fallback arithmetic instruction for
// instruction. Still: retail builds both defaults unconditionally and writes each selected default
// into the parameter's home, ours branches over the block and leaves the parameters alone, so the
// vertex emission that follows is one instruction out of step; retail also reloads preserve_alpha
// after the call in the short arm where ours keeps the value in a register.
// MiniMax C (11-10): 140 (was 140): no lower score this round. Retail builds both default
// rectangles unconditionally (source {0,width,0,height} at esp+0x18, destination {320-w*0.5,
// 320+w*0.5, 240-h*0.5+arg, arg+h*0.5+240} at esp+0x28), tests the source first and the destination
// second, and writes the chosen default pointer back into the two parameters' own argument slots
// (six bytes each). That form matches retail's arithmetic and register roles instruction for
// instruction, but our build keeps the chosen pointer in a register, so the two writes are missing and
// the body is 12 bytes short (1717 against 1729), scoring 156; a named local copy of the two
// parameters folds to the same register-only form, as do the plain, pointer-to-parameter and
// reference-to-parameter spellings. Retail also reads a fourteenth byte argument (esp+0x70) in the
// short arm where this source reads preserve_alpha (esp+0x6c), so the declaration would have to
// change, which this lane may not do; the call sites live in another lane's file.
// Earlier 140 (was 140): Frame and retail extent agree; caller flag residency and native
// fallback selection remain. Both complete coverage clamps now follow the retail upper-default stores.
// Earlier 154 (was 251): frame 0x28 agrees; zero/sampler homes, bitmap transport and late fallback selection remain.
// Earlier: 251 fast (was 252 fast): the pixel-shader RGB output mask is selected before the shift
// ((inverse ? 8 : 0) << 8), which gives retail's and/shl pair instead of a single 0x800 mask.
// Earlier: 252 fast (was 335 fast): the standard marker gives retail's stack convention (ret 0x34,
// inverse at E+36) and width/height default to 1.0f before the bitmap test. Still differs: the
// 0/1/2 constants land in different registers than retail (EBP against EDI), and the call to
// 0x144f0 uses our callee's convention.
// Earlier: 331 (was 331): Frame 0x28 agrees; inverse entry save, dimension homes and bitmap-call setup remain.
// frame 0x28 agrees; inverse entry save, dimension homes and bitmap-call setup remain.
// Frame 0x28 agrees; inverse AL entry save, dimension homes and bitmap-call setup remain.
// fast control 427; direct inverse 427; selected destination/source pointer 427/427;
// distinct final alpha 427; separate clamped strength 428. Original body restored.
// Fast control 427; rectangle clear 433; shared alpha clamp 427; shared strength clamp 427; vertical sample 427; reciprocals before Begin 433.
// Fast control 427; early fallback rectangles 438; early source rectangle 442; early reciprocals 439; blend-operation snapshot 438; late strength clamp removed 461 (fast).
// Fast control 427; long pixel dimensions 433; short dimensions 456; simple combiner first 427; immutable bitmap views 427; explicit bitmap validity 432.
// Height-first acquisition 427; paired dimensions 427; alpha clamp branches 427; combiner output branches 427; destination fallback first 348 fast, 344 plain kept.
// Destination-first control 348 fast; direct inverse 335 fast, 331 plain kept;
// captured inverse 335; direct inverse with width/height/both defaults 365/434/349;
// captured inverse with both defaults 349; bounded dimension results 430;
// entry dimension results 429. Only the own-address inverse alias is removed.
// Tag/bitmap/both observations 400/399/399; pixel fields at use 335;
// native source/destination/both bounds 335/335/335 (exact-only, restored);
// one shared exit 335; all fast against control 335.
// Round 361: native group/bitmap/both records 335/335/335; bitmap/grouped bitmap references
// 335/335; group fields at use alone/with bitmap/with bitmap reference 400/400/400 (fast).
// Round 147: fast control 335; blend complement 401; paired blend branches 453 (exact-only); finite clamp
// operands/branches 335/335; chained source zeros 335; paired half-extents 335; shared UV edges/half-
// extents 456/456.
// Unsigned/signed byte flag formals for inverse, inverse plus blend, all flags,
// and inverse plus combiner flags: 335 each (fast control 335). Original
// Boolean definition and every declaration restored; no call argument or order changed.
// Round 195: explicit output nibble shift 348 fast; full EFG/ABCD/both 442/434/441;
// single-bit output shift 333; full EFG/ABCD/both 444/439/443 (fast control 335).
// The two-point selector gain and all packed-field forms restored; full baseline remains 331.
// Round 164: Common initial alpha packing with ternary/conditional-update/complete
// raw-result/shared-clamp producers 435/435/431/435; function-entry workspace
// 435/435/431/435. All regress against fast control 335; original shader graph restored.
// Round 148: fixed-stage address/filter/outlined-disabled tables 969/983/725;
// address+filter/address+disabled/filter+disabled/all three 1127/1029/1044/1258; all with disabled
// render table 1162, versus control 331.
// Late unconditional destination/source defaults 432/334; both destination-first/source-first
// construction with destination selection first 433/439; source selection first 439/433;
// source construction before conditional destination 331 (four-point fast gain rejected);
// conditional source then unconditional destination 427, against fast control 335.
// Round 60: BLOCKED-BY 0x289e0 (the only caller, under #if 0, so LTCG gives inverse AL). Own-address
// inverse read at entry/first use 348/348 (ret 0x34 as retail, load hoisted, frame 0x24); early
// dimension defaults 349; with the own-address read 316 (exact-only); unconditional rectangles 439.
// Round 204: Tag/bitmap producer, late fallback/draw arrays and paired shader-alpha workspace
// scopes, masks 1-7: 251/251/251/255/255/255/255; all with coherent dimension/reciprocal names
// 255. Full-stack transport retained; original scopes, observations, arrays and equations restored.
// Round 235: Direct enum-range address/filter/disabled-state loops, seven subsets and complete while form
// [926, 883, 685, 1104, 1159, 1089, 1240, 681] fast. Runtime SDK setter specialization grows the body; all
// original ordered direct calls, arrays, fields, directives and body restored.
// Round 253: bounded native pixel short/long/word/dword producers 427/410/427/448;
// native pixel lifetime across bitmap guard 438/439/438/433. Original float dimensions,
// full-stack marker, state calls and complete body restored.
// Round 169: native fallback source/destination/both records, native source/destination/both
// input fields, and both records plus source/both input fields: eight forms 251 each.
// Parallel reciprocal-first UV products with original/native/native-field rectangles 251
// each; native source-before-target frame names 251. Complete native fallback production,
// with native input fields/frame names/both plus reciprocal-first products: 257 each.
// Original arrays, fallback order, products and complete body restored.
// Round 261: actual four-corner vertex table, byte/word/long/dword with long ordinal
// 337/337/353/353; with short ordinal 339/339/343/343. Original unrolled draws restored.
// Native mutable/immutable normalized texture rectangles, combiner order and shared
// alpha-clamp combinations 313/313/313/313/313/313/313/313; frame 0x38 versus 0x28. Original body restored.
// On the standard-marker base: both default rectangles built unconditionally, source first, then
// the source and destination defaults selected 260 (aligned 143 -> 128: retail's slots and test
// order; retail also writes each selected default back into the parameter's home); as aggregate
// initializers 260; offset converted once 260; selected by ternaries 260; with the reciprocals
// before Begin 256. The zero constant (EDI against retail's EBP) follows the 0x144f0 call.
// Round 43: both alpha clamps in 0x2a2f0's clamp shape (zero, else one then the value unless above one;
// retail's store-then-overwrite) 247 (four-point gain, not kept); with the final-combiner words through
// PS_COMBINERINPUTS 247 (same code; the EFG stores stay cross-jumped because both arms use EDX); both
// default rectangles built first, source/destination selected after 256/256 (1711 bytes); with the macros
// 256; the RGB output word through PS_COMBINEROUTPUTS 247. Retail reads inverse from its home each time
// (EBX carries 0x144f0's fallback), ours caches it in EBX: BLOCKED-BY 0x144f0.
// Round 133 (ret 0x34 already agrees): both alpha clamps in 0x2a2f0's shape 247 (four points, not
// kept); defaults built unconditionally, source first, then selected 260 (1711 bytes); both 256;
// clamps with reciprocal-first UV products 247, with the defaults too 256 (same code each).
// Round 122: 0x144f0 now matches and takes the fallback in EBX, so EBX no longer caches the inverse: 154 at retail's
// length (first difference +0x32b).
// Round 74 (lane 9, fast; 0x144f0 edits temporary, restored): BLOCKED-BY 0x144f0, whose convention is now
// found: it matches as (short stage, long fallback, long tag, short fallback_index, short index, real
// priority) with two calls to 0x14390 in place of the goto, the count read at each use, no address-taken
// fallback and no volatile. Its file is left alone this round. This function's count under each callee form:
// fallback read directly 266; plus an address-taken priority 268; direct without the volatile 266; two calls
// 156 (1729 bytes, first +0x280); two calls in order (tag, fallback, stage) 267; (fallback, tag, stage) 267;
// (stage, tag, fallback) 154 (first +0x32b); (stage, fallback, tag) 154; (fallback, stage, tag) 156; with the
// matching 0x144f0 154.
// J242: Shared upper-default/if-ternary coverage clamps 140/140; independent-zero/default-input
// 219/154. Separate producer counterparts 148/148/223/162. Coherent alpha inline/force-inline
// 181/181, both raise 0x3a5c0 from 31 to 248; complete native fallbacks 156, with inline 204. Keep
// only the plain-confirmed upper-default pair, 154 to 140 at retail length. NOT kept: all later
// forms regress or worsen a helper user. Caller flag residency and fallback selection remain.
// DH318: Complete packed register selectors, ternary/branch results in byte/word/dword/long:
// 199/164/209/209/140/213/140/213. Flag residency and fallback parameter-home selection remain. All
// restored.
// Round 38 (lane 17, plain, base 140): both defaults built unconditionally, source first, then source
// and destination selected 156 (1717 bytes); with half extents named once 150 (1723: retail's selection
// except its write of each selected default into the parameter's home); with the vertex reads through
// the parameters' addresses (diagnostic) 150; the arms swapped (`simple || preserve_alpha` first) 140;
// preserve_alpha read through its address in the short arm 140 (retail reloads it after the call).
// Y557: Shared actual combiner count, conditional long/dword/word/byte
// producers 406/406/390/414; branched producers 406/406/390/414.
// NOT kept: all regress and disturb the matched prefix; original flag reads,
// state stores, fallback arrays and complete body restored.
// F357: Complete shader-admission truth in bool/byte/word/dword, logical
// producers 140/140/140/140; eager Boolean producers 243/243/243/243.
// NOT kept: no gain; original flag reads, shader arms, fallback arrays and body restored.
// AC506: Common post-arm first-constant/final-ABCD/final-EFG publication, seven subsets
// and all results declared at entry: 155/180/181/175/193/187/197/197. NOT kept: every
// one-use output form regresses; original branch stores and complete body restored.
// Y563: Common output workspace across shader arms, shader/function scope, original/shared
// raw workspace and ternary/upper-default short alpha: 140/145/140/145/140/145/140/145.
// NOT kept: no gain or byte match; original workspace scopes and complete body restored.
// N353: Separate caller-side scalar/color/finalizer inline declarations, seven subsets,
// and all force-inline declarations: 140 each. NOT kept: no gain; original declarations restored.
// F380: Isolated short-arm final-alpha branch/default stores and expression-owned nibble/word
// results 145/250/252/161/162/140/140/145. NOT kept: regressions or ties;
// original flag reads, shader stores, fallbacks and complete body restored.
// DH358: Nested test-set admission results, rejected/admitted defaults in bool/byte/word/dword,
// 140/140/140/140/140/140/140/140. NOT kept: no byte match; original admission predicate,
// flags, shader workspaces and complete body restored.
// Round 132 (m3 lane 7, fast, base 140): both defaults built unconditionally, source first, then both
// selected, with/without named half extents 156/156 (1717 bytes; no parameter-home writes); inverse
// height computed before inverse width 146 (the reciprocal registers become retail's, xmm0 = 1/h, but
// width and height then swap homes 0x10/0x14: the homes follow the reciprocal order, not names or the
// declaration order: with height declared first 144, with width renamed 146); that with the
// unconditional defaults 162; source coordinates divided by width/height in each call 158 (1721).
// 0x3a5c0's macro fix has no counterpart here (no file-local helper in the body).
// Round 159 (lane 5, fast with 0x144f0/0x3a5c0/0x2a2f0/0x29b00; both builds end `ret 0x34`). Reading of the tail: retail
// builds both default rectangles before either test (source at 0x18, destination at 0x28), tests the source first, and on each
// default path writes the selected address into the parameter's own home as well as its register (`lea ecx; mov [home], ecx;
// mov ebx, ecx`); it also reads preserve_alpha again after the 0x131fc0 call where ours keeps it in CL across the call, and
// multiplies the stored reciprocal by the coordinate (reciprocal first). Unconditional defaults with the parameters assigned
// 156 (1717 bytes, no home writes); the two bounds as one by-value pair or as two one-pointer wrappers 156/156 (same code:
// small by-value aggregates are split into scalars); the first seven arguments, all thirteen, or the first four as one
// by-value parameter block 178/176/170 (1725/1729/1729 bytes but frame 0x18: the defaults then overlap the block, aligned
// rows 140-142 against 109); `inline` on the definitions of 0x131fc0 and 0x131f40 181 (1741 bytes; preserve_alpha then sits
// in BL from before the call, and 0x3a5c0 goes 9 -> 226); unconditional defaults selected into two new pointer locals, each
// after its copy or both copies first, 160/158 (1713). None kept.
// MiniMax R (11-09): no new form, no lower score; the store to the shader global still comes after the second flag test where retail's comes before, and retail keeps that flag in al where ours keeps it in cl; the halves and the pixel-shader fields are still written in our own order.
// Round 102 (m3 lane 9, fast, base 140): the long arm equals retail; what differs is the preserve_alpha load our
// build hoists above the `simple` branch (CL, kept across 0x131fc0) where retail loads it after the branch and again
// (BL) after the short arm's call, and the tail. `(void)&preserve_alpha` 140 (no effect); the y offset named once
// 140; the half extents named once 140; const pointers to source/destination read back before Begin 140 (no home
// writes); source tested before destination 140 (source then in EBX, destination in ESI, as retail); reciprocal-first
// UV products 140; the short arm's clamped strength named 140; source first with the named offset 140; on that both
// defaults built unconditionally and then selected 154 at 1717 bytes, which is retail's tail but for its two writes
// of the chosen default into the parameter's home (`lea ecx; mov [home], ecx; mov ebx, ecx`, 6 bytes each); each
// default array declared at its own null test 140. The short arm's `mov bl, [esp+0x70]` comes right after the cdecl
// `push edx` for 0x131fc0, so it is preserve_alpha (the thirteenth argument, ret 0x34), not a fourteenth argument.
// Round 104 (m3 lane 9, fast): retail's write of each chosen default into the parameter's home occurs at only three
// sites in the image (these two and one in unmatched 0x14b120), so no matched function shows its source form. The
// compile-only harness (no LTCG) gives retail's flag loads, so our CL hoist of preserve_alpha follows from LTCG's
// summary of 0x131fc0 (retail has that summary too: inverse stays in CL across 0x131f40/0x131fc0 in the long arm).
// Both defaults built unconditionally, source first 156 (1717 bytes, aligned 117 -> 95); with the two zero coordinates
// as one chained store 155 (aligned 89); that with the height's reciprocal first and reciprocal-first products 161;
// the four draws in a new inline helper 709 (LTCG keeps it out of line, 1437 bytes); `!(simple || preserve_alpha)`
// 140; the chained zero store on the conditional source default 140.
// Round 93 (m3 lane 1, fast; register-masked aligned count in brackets, 140 is [53]): both defaults built unconditionally,
// source first, the parameters selected by ternaries 156 [39] (1717 bytes: retail's tail but for its two parameter-home
// writes); that with the selection through a forceinline helper taking the pointer by reference 156 [39]; with the source
// test read once through the parameter's own address 156 [39] (the reference and the address read both fold); the
// combined test held in a named bool 140; the test as `simple == false && preserve_alpha == false` 140; both defaults
// built unconditionally, destination first, with the tests in the current order 150 (1723 bytes). Diagnostic,
// reverted: a temporary caller in the 0x289e0 stub making the three calls the disabled 0x289e0 makes 140 [53] (plain), so
// the missing caller is not what keeps the parameters in registers. A permuter run (43 tries in 622 s) reached 105 only
// through an `int` temporary for one texture coordinate (changes what is drawn) and two volatile casts; nothing usable.
// MiniMax A (11-11), NOT kept (coordinator 11-11): 140 -> 54 with both default-rectangle fallbacks written through volatile pointers to the parameters; volatile is reverted, body at 140.
// MiniMax B (11-13): no new form, no lower score (140): our build still hoists the preserve_alpha load above the `simple` branch (kept in cl across 0x131fc0) where retail loads it after the branch and again in bl; the tail's two parameter-home writes are retail's.
// MiniMax G (11-13): no new form, no lower score (140): this build still hoists the preserve_alpha load above
// the `simple` branch (kept in cl across 0x131fc0) where retail loads it after the branch and again in bl; the
// permuter run over this function found no usable form either.
// MiniMax A (11-14): no new form, no lower score: this body's score moves with the reduced image's
// source set (140 on a single-function image, 196 on the queue's image of 562 sources, where the
// first difference is at +0x15); the difference itself is unchanged by the image.
// @retail 0x2a8d0 standard
void __stdcall function_2a8d0(long tag, long bitmap_index, real const *source,
    real const *destination, long arg_af5a49, color3f const *color,
    real strength, bool blend, bool inverse, bool alternate_target,
    bool screen_target, bool simple, bool preserve_alpha)
{
    if ((!g_4b99b0[0] && !g_4b99b0[1]) || strength < 0.0f) return;
    if (strength > 1.0f) strength = 1.0f;
    else if (!(strength > 0.0f)) return;
    if (!g_4b99b0[1] && alternate_target) return;
    real default_source[4];
    real default_destination[4];
    real width = 1.0f;
    real height = 1.0f;
    byte *definition = tag != NONE ? g_4e3b44[tag & 0xffff].bytes : NULL;
    if (tag != NONE && bitmap_index >= 0 && bitmap_index < *(long *)(definition + 0x44))
    {
        byte *bitmap = *(byte **)(definition + 0x48) + bitmap_index * 0x74;
        real w = (real)*(short *)(bitmap + 4);
        width = 1.0f > w ? 1.0f : w;
        real h = (real)*(short *)(bitmap + 6);
        height = 1.0f > h ? 1.0f : h;
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
    function_144f0(0, 0, tag, 0, (short)bitmap_index, 0.0f);
    sampler_state_write(0, D3DTSS_ADDRESSU, 1);
    sampler_state_write(0, D3DTSS_ADDRESSV, 1);
    sampler_state_write(0, D3DTSS_ADDRESSW, 3);
    sampler_state_write(0, D3DTSS_MAGFILTER, 2);
    sampler_state_write(0, D3DTSS_MINFILTER, 2);
    sampler_state_write(0, D3DTSS_MIPFILTER, 2);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
#pragma inline_depth(0)
    function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
    function_0224f0(0, D3DTSS_MAXMIPLEVEL, 0);
    function_0224f0(0, D3DTSS_COLORSIGN, 0);
    function_0224f0(0, D3DTSS_ALPHAKILL, 0);
#pragma inline_depth(255)
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    if (!simple && !preserve_alpha)
    {
        g_484f68.PSCombinerCount = 0x11002;
        real unclamped_alpha = 1.0f - *(real *)(g_4b99b0 + 0x30);
        real alpha;
        if (unclamped_alpha < 0.0f) alpha = 0.0f;
        else
        {
            alpha = 1.0f;
            if (!(unclamped_alpha > 1.0f)) alpha = unclamped_alpha;
        }
#pragma inline_depth(0)
        g_484f68.PSConstant0[0] = function_131fc0(alpha);
#pragma inline_depth(255)
        g_484f68.PSConstant1[0] = 0xff;
        g_484f68.PSRGBInputs[0] = 0x8200211;
        g_484f68.PSRGBOutputs[0] = (inverse ? 8 : 0) << 8;
        g_484f68.PSConstant0[1] = function_131f40(strength, color);
        g_484f68.PSRGBInputs[1] = 0x8011811;
        g_484f68.PSRGBOutputs[1] = 0xcd;
        unclamped_alpha = 1.0f - *(real *)(g_4b99b0 + 0x2c);
        if (unclamped_alpha < 0.0f) alpha = 0.0f;
        else
        {
            alpha = 1.0f;
            if (!(unclamped_alpha > 1.0f)) alpha = unclamped_alpha;
        }
#pragma inline_depth(0)
        g_484f68.PSFinalCombinerConstant0 = function_131fc0(alpha);
#pragma inline_depth(255)
        g_484f68.PSFinalCombinerInputsABCD = ((inverse ? 0x11 : 0x20) | 0xf00) << 16;
        g_484f68.PSFinalCombinerInputsEFG = ((blend ? 0 : 0x1d) | 0xc0d00) << 8;
    }
    else
    {
        g_484f68.PSCombinerCount = 0x11001;
#pragma inline_depth(0)
        g_484f68.PSConstant0[0] = function_131fc0(strength > 1.0f ? 1.0f : strength);
#pragma inline_depth(255)
        g_484f68.PSAlphaInputs[0] = 0x18110000;
        g_484f68.PSAlphaOutputs[0] = 0x80;
        g_484f68.PSFinalCombinerInputsABCD = 0x8180000;
        g_484f68.PSFinalCombinerInputsEFG = (preserve_alpha ? 0 : 0x18) << 8;
    }
    function_1ccf0(&g_484f68);
    function_1cc30(12);
    function_1c710(g_51f0f0);
    if (!destination)
    {
        default_destination[0] = 320.0f - width * 0.5f;
        default_destination[1] = 320.0f + width * 0.5f;
        default_destination[2] = 240.0f - height * 0.5f + arg_af5a49;
        default_destination[3] = arg_af5a49 + height * 0.5f + 240.0f;
        destination = default_destination;
    }
    if (!source)
    {
        default_source[0] = 0.0f;
        default_source[1] = width;
        default_source[2] = 0.0f;
        default_source[3] = height;
        source = default_source;
    }
    D3DDevice_Begin(D3DPT_TRIANGLEFAN);
    real inverse_width = 1.0f / width;
    real inverse_height = 1.0f / height;
    D3DDevice_SetVertexData2f(1, source[0] * inverse_width, source[2] * inverse_height);
#pragma inline_depth(0)
    D3DDevice_SetVertexData4f(0, destination[0], destination[2], 16777215.0f, 16777215.0f);
#pragma inline_depth(255)
    D3DDevice_SetVertexData2f(1, source[1] * inverse_width, source[2] * inverse_height);
#pragma inline_depth(0)
    D3DDevice_SetVertexData4f(0, destination[1], destination[2], 16777215.0f, 16777215.0f);
#pragma inline_depth(255)
    D3DDevice_SetVertexData2f(1, source[1] * inverse_width, source[3] * inverse_height);
#pragma inline_depth(0)
    D3DDevice_SetVertexData4f(0, destination[1], destination[3], 16777215.0f, 16777215.0f);
#pragma inline_depth(255)
    D3DDevice_SetVertexData2f(1, source[0] * inverse_width, source[3] * inverse_height);
#pragma inline_depth(0)
    D3DDevice_SetVertexData4f(0, destination[0], destination[3], 16777215.0f, 16777215.0f);
#pragma inline_depth(255)
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
// MiniMax C (11-11): no new form, no lower score (364); retail keeps the second copy of 0x235a0 out of line because of its call-site frequency (the call appears 17 times in retail), and the sample block's second test and the allocator still differ; the forms tried here score 401 and above.
// MiniMax I (11-10): 364 (was 401): the sample flag is written the way retail writes it (0 at 0x23716 before the
// admission chain, 1 at 0x23745 after it, read at 0x238ad and 0x23b62), the level clamp is `>=` (retail's cmovge),
// the coefficient pointer starts at layout->data and reaches the level entry through pointer arithmetic (retail
// loads the entry address and then the entry), and the sample count is a long. 0x73360 still matches.
// MiniMax S (11-11): score 371 (was 401): the sample flag is stored false before the admission chain and
// set true after it, as retail does at 0x23716; 0x22850 keeps 223.
// MiniMax E (11-11): no new form, no lower score (364); the entry block matches retail's, but our body is 54 bytes
// longer than retail's (1597 against 1543), which moves every later jump target, and the extra code is not located.
// MiniMax H (11-12): no new form, no lower score (364): the body is still 54 bytes longer than retail's, so the shared exit lands at +0x621 where retail's is at +0x5eb, and the inlined matrix copy still loads its values into registers before storing them where retail stores each value as it loads it.
// score 364 (was 481, full build): walked against retail block by block (b2H, 11-06). Fixed, all with retail evidence:
// score 401 (was 481, full build): walked against retail block by block (b2H, 11-06). Fixed, all with retail evidence:
// the result is a bool slot (0x13);
// `order` is a full dword argument (retail pushes it movsx'd at 0x4b9e5 and reads it whole at 0x237ae; short here
// made 0x4b5d0 175, long makes it 93); the structure global is read
// at the use (0x23941, 0x23b75) before the position lookup; the coefficient address is formed once before the cache
// fill (0x237c4). Unchanged and equal to retail: both determinant trees, every callee argument and the loop shapes.
// Still differs: retail inlines the first 0x235a0 copy (0x238a4-0x23935) but calls 0x235a0 for the second (0x23acc,
// its only retail call), ours inlines both; ours keeps a short-compare of the sample count (jbe) against retail's
// dword spill and jle (a long count scored 515 on the old body); esi/edi swap in the prologue. Tried: entry pointer
// local 494; `level < limit ? level : limit` 449 (no change); return false in the failure arm 534; long result 530;
// use_samples stored false and then set by the test (retail stores false at 0x23716) 463, not kept (a value set
// before a test and then overridden).
// Not kept: retail inlines render_memory_allocate and table_entry_data here (dword rounding at 0x237fd-0x23810, the
// NONE test at 0x23830, the sign test at 0x23843); calling them (render_memory.h) scores 467 but loses the matched
// 0xc2d00 in the full build, so the allocation stays written out. The byte count is count << 2, so the rounding never applies.
// Earlier score 506 (was 530): frame 0x21c versus 0x200; order, coefficient and separate sample homes differ.
// Earlier: Frame 0x20c versus 0x200; order, coefficient homes and sample paths remain.

// MiniMax B (11-13): no new form, no lower score (364): the first difference is at +0x52, where this build's early exit jumps to a block that sits further from the function's start than retail's, so the body after that pairs against the wrong instruction; the three rotation calls also pass the rotation on the stack instead of in EAX, because that callee lives in another file.
// Earlier score 530 (was 530): Frame 0x20c versus 0x200; order, coefficient homes and sample paths remain.
// Round 73 (plain checks with 0x73360): the held round 90 body 401 and 0x73360 MATCH -> 33 again;
// that body with the existing render-memory allocator, merged bool result, default light in the
// else arm and globals read at each use but the original sample loop 511, 0x73360 again 33; the
// same without the allocator helper 558 (fast); merged bool result alone 536 (fast, still ret 0x18).
// In the broken builds LTCG has 0x193300's register summary when it compiles 0x73360 (it evaluates
// the maximum-players call first and keeps it in EDX); 0x193300 as an inline definition with a
// noinline declaration keeps 0x73360 at 33 and costs 0x705f0 and 0x1931d0. BLOCKED-BY 0x73360.
// Held (batch 132): Claude lane 4 round 90's two explicit sample blocks with a determinant helper reached 401
// (frame 0x200, all-stack arguments), but in the batch build that code cost c_helper_b::v7 (0x73360) its match
// (code linked ahead of src/unknown_058dd0.cpp); with only this file restored the batch keeps 0x73360. Retry it
// only with a full check that keeps 0x73360 matched.
// Nine-real rotation outputs 553 (frame 0x1bc); cache-count snapshot 544; defaults after empty exit 544;
// unsigned handle 544; native bucket fields 544.
// Frame 0x20c versus 0x200; stack order, coefficient homes and shared sample paths remain.
// Earlier 544 (was 554): Frame 0x214 versus 0x200; stack order, coefficient homes and shared sample paths remain.
// Full-width order 578; byte sample 575; bucket cursor 580; bucket rotation scope 575;
// found-index search 572; long found-index 603.
// Native cache cursor 590; bounded sample while 575; bounded bucket while 575;
// per-bucket transformed buffers 575; inclusive level cap 575.
// Long result 570 kept; outer rotated buffers 575; accumulated search 573; word sample count 576;
// depth one 575; long result/search 575; short result 570; byte result 596.
// Indexed matrix copies 758; structure reload 588; long eligibility 572; layout-order sample 589; long cache-hit result 570 (fast).
// Cache index 575; in-place inverse 584; capped short sample count 561 kept (plain); delayed default coefficients 576; failure-first result 589 (fast).
// Shared cache helper 718; explicit eligibility 560; entry matrix 561; short bounded sample loop 606; post-metadata defaults 554 (plain).
// Native sample arrays 554; byte output cursor 554; exact determinant tree 554; unsigned byte size 554; signed-byte success 563 (fast).
// Next matrix at loop tail 586; sample input cursor 552; short count override 554; entry direction workspace 554; positive allocation scope 544 kept (plain); scope plus sample cursor 542, extra two rejected.
// Round 151: Live structure 561; live cache entry 530 kept; both live 544; six determinant terms 544; terms with structure/cache/both 561/530/544; direct bucket input 544.
// Short/word eligibility 537/537; short/word cache hit 534/534; sampled-transform,
// inverse-rotation and both workspace names 530/530/530; inverse declared at sample
// entry 530; Boolean success with live cache observations 535.
// Round 129, live-cache graph: full-width order with long/bool/short/byte result
// and short sample ordinal 542/535/542/573; with long sample ordinal
// 570/559/570/573. Original order declarations, result and sample types restored.
// Round 131: shared cross/dot determinant in all three cyclic orientations,
// either dot receiver order, 543 each (exact-match-only); signed-byte capped
// sample count 529 (one-point gain rejected); byte capped count 564.
// Negative allocation failure bool/byte/short/long in the original rejection arm
// 531/531/529/528; reused actual capacity predicate 539/539/537/537.
// Best reduction two is below the gate; original result and all helpers restored.
// Coefficient address at the consumer 506; direct bucket/sample/both 530/530/530;
// consumer address with bucket/sample/both 506/506/506. Address begun at accepted
// allocation with both direct receivers 527 (three-point gain rejected). Only the
// consumer address retained; 1404/1543 bytes, frame 0x21c.
// Round 190: Existing copy/inverse/coefficient inline declarations singly, in three pairs and
// together 530 each; all three separate noinline declarations 850. Original helper declarations and
// body restored; frame 0x20c/0x200 and separate-sample boundaries remain.
// Direct cache-hit completion and found-byte break, each with long/bool success and original/positive
// eligibility results, 554/549/554/549 each. Original exhaustive scan and body restored.
// Round 102 (m3 lane 9, fast unless marked): the allocation through render_memory.h's render_memory_allocate with the
// sign-tested handle pointer 478 (1660 bytes, both 0x235a0 copies still inlined); the per-bucket sample count as long
// (retail's movzx, spill and signed jle) 400; the level clamp as `>=` (retail's cmovge) 401; the coefficient pointer
// started from layout->data before the level arithmetic (retail loads +0x20 first) 395; that with the limit named
// 399; data-first + long count + `>=` 394 (plain 394, seven points). Diagnostic only, not kept: inline_depth(0)
// around the second 0x235a0 call 399 at 1486 bytes, and with the allocator helper 295 at 1547/1543: retail's shape is
// the allocator inlined and the second 0x235a0 copy called; no source form found that keeps that call out of line.
// Retail stores the sample flag false (0x23716) before the admission chain and true after it, as 0x22850's test is
// shaped: the admission through the shared structure_has_matching_samples written with a result variable (false,
// set true by the test) 371 plain, but the direct `return` form gives 401, so that gain is a flag form (match-only);
// the direct form is kept (same 401, 0x22850 gains). On it: the inlined allocator as well 478 (1660 bytes, the second
// 0x235a0 copy still inlined); the data handle stored after the output pointer 401; the sample count declared before
// the bucket loop 401; the sample block under `if (available_samples)` instead of `continue` 401.
// Round 104 (m3 lane 9, fast): retail keeps the second 0x235a0 copy out of line because of its call-site frequency,
// not an inline budget: the first copy repeated 6 or 24 times and five more sample blocks all stay inlined (401; 51
// calls), while one more test on the second site (`available_samples > 1 && layout->order > 0`, diagnostic only)
// calls it (384 at 1499 bytes, 17 calls as retail). Forms that leave it inlined: the second test as `== 2` 401; the
// allocation through render_memory.h with the sign-tested output pointer and the handle stored after it 478 (1660
// bytes, aligned 421 -> 223); that with the bucket body under `if (available_samples)` 478, as a while loop 478,
// with a long sample count 462 (aligned 220); the early exits as nested positive tests with one return 478; both
// determinants through one new inline helper 401 (same code), with the allocator too 478; the second test as
// `available_samples >= 2` 401 (same code). Lead: the test retail has on the second sample's path that the optimizer
// later removes.
// MiniMax B (11-11), NOT kept (coordinator): 364 -> 284 needs a new goto after the masked-handle compare with minus one
// (a new goto is match-only; the rounding alone gives 405, fast). The lead as written: the byte count is rounded up to a multiple of four before it is
// tested for zero, the masked handle is compared with minus one the way retail does before the size is stored,
// and the output pointer is taken from the handle masked a second time, which is where retail's second mask
// comes from; what still differs is that our rounding computes four minus the remainder where retail subtracts
// the remainder and then adds four, our first matrix copy loads its four values into registers before it stores
// them where retail stores each value as it loads it, and the three rotation calls still pass the rotation on
// the stack instead of in eax because that callee lives in another file.
// MiniMax A (11-12): no new form, no lower score (364); the first difference is at +0x52, where this build's early
// exit jumps to a block that sits 0x36 bytes further from the function's start than retail's, and the body after that
// pairs against the wrong instruction all the way down; the frame agrees (0x200) and the two length checks on the
// count are the same, so what is left is the layout of the blocks and the order of the reads.
// MiniMax H (11-13): no new form, no lower score (364); the first difference is at +0x52, where this build's early exit jumps to a block that sits further from the function's start than retail's, so the body after that pairs against the wrong instruction; the three rotation calls also pass the rotation on the stack instead of in EAX, because that callee lives in another file.
// @retail 0x23690
bool __stdcall function_23690(s_coefficient_layout const *layout, long order, long object_index,
    s_137801 const *vertices, long count, byte const *indices, long tag)
{
    bool result = true;
    {
        bool found = false;
        for (short i = 0; i < g_5093e8; ++i)
            if (g_4b89b0[i].key == object_index) found = true;
        if (found || g_5093e8 >= 24) goto done;
        g_4b89b0[g_5093e8].key = object_index;
        *(short *)&g_4b89b0[g_5093e8].unknown4 = order;
        bool use_samples = false;
        if (structure_has_matching_samples()) use_samples = true;
        real red[16], green[16], blue[16];
        if (!use_samples)
        {
            vector3f direction = { 0.0f, 0.0f, -1.0f };
            spherical_harmonics_evaluate_directional_light(&direction, layout->order,
                1.0f, 1.0f, 1.0f, red, green, blue);
        }
        real const *coefficients = layout->data;
        long level = 5 - order;
        if (level >= layout->level_count - 1) level = layout->level_count - 1;
        coefficients += (layout->levels + level)->offset;
        long stride = layout->groups * (layout->columns * 3 + 4);
        s_bucket_source const *buckets = (s_bucket_source const *)vertices;
        long total = function_023540(count, buckets, &g_4b89b0[g_5093e8], stride);
        long bytes = total * stride * sizeof(real);
        if (!bytes) goto done;
        long next = g_487b18.size + bytes;
        if (next <= 0x27000)
        {
            long handle = g_487b18.size & 0x0fffffff;
            g_487b18.size = next;
            g_4b89b0[g_5093e8].data_handle = handle;
            real *output = (real *)(g_487b18.data + handle);
            for (long bucket = 0; bucket < count; ++bucket)
            {
                byte const *pair = (byte const *)vertices + 4 + bucket * 4;
                long available_samples = *(word const *)(pair + 2);
                if (!available_samples) continue;
                real const *source = (real const *)((byte const *)vertices + 0x44 + *(short const *)pair * 0x30);
                transform4x3f matrix;
                matrix3x3 rotation;
                real rotated_red[16], rotated_green[16], rotated_blue[16];
                real determinant;
                function_0235a0(source, &matrix);
                if (use_samples)
                {
                    byte *bsp = *(byte **)((byte *)g_4e0344 + 0x84);
                    point3f const *position = (point3f const *)function_23600(tag, indices, bucket, object_index, 0);
                    if (position)
                    {
                        function_22cf0((s_light_sample_source const *)(bsp + 0x38), red, green, blue,
                            layout->order, &g_4b89b0[g_5093e8].scale, position);
                    }
                }
                determinant = matrix.left.k * matrix.forward.j * matrix.up.i;
                determinant += matrix.forward.k * matrix.up.j * matrix.left.i;
                determinant += matrix.up.k * matrix.left.j * matrix.forward.i;
                determinant -= matrix.left.k * matrix.up.j * matrix.forward.i;
                determinant -= matrix.up.k * matrix.forward.j * matrix.left.i;
                determinant -= matrix.forward.k * matrix.left.j * matrix.up.i;
                function_142bf0((matrix3x3 const *)&matrix.forward, determinant, &rotation);
                function_143600((real const *)&rotation, 3, red, rotated_red);
                function_143600((real const *)&rotation, 3, green, rotated_green);
                function_143600((real const *)&rotation, 3, blue, rotated_blue);
                function_023ca0((s_block_layout const *)layout, coefficients, rotated_red, rotated_green, rotated_blue, output);
                output += stride;
                if (available_samples > 1)
                {
                    function_0235a0(source + 12, &matrix);
                    determinant = matrix.left.k * matrix.forward.j * matrix.up.i;
                    determinant += matrix.forward.k * matrix.up.j * matrix.left.i;
                    determinant += matrix.up.k * matrix.left.j * matrix.forward.i;
                    determinant -= matrix.left.k * matrix.up.j * matrix.forward.i;
                    determinant -= matrix.up.k * matrix.forward.j * matrix.left.i;
                    determinant -= matrix.forward.k * matrix.left.j * matrix.up.i;
                    function_142bf0((matrix3x3 const *)&matrix.forward, determinant, &rotation);
                    if (use_samples)
                    {
                        byte *bsp = *(byte **)((byte *)g_4e0344 + 0x84);
                        point3f const *position = (point3f const *)function_23600(tag, indices, bucket, object_index, 1);
                        if (position)
                        {
                            function_22cf0((s_light_sample_source const *)(bsp + 0x38), red, green, blue,
                                layout->order, &g_4b89b0[g_5093e8].scale, position);
                        }
                    }
                    function_143600((real const *)&rotation, 3, red, rotated_red);
                    function_143600((real const *)&rotation, 3, green, rotated_green);
                    function_143600((real const *)&rotation, 3, blue, rotated_blue);
                    function_023ca0((s_block_layout const *)layout, coefficients, rotated_red, rotated_green, rotated_blue, output);
                    output += stride;
                }
            }
            ++g_5093e8;
        }
        else
        {
            result = false;
        }
        goto done;
    }
done:
    return result;
}

/* a corner of the unit square, rotated by an angle and scaled, as a texture
   coordinate pair in [0, 1] */
static __forceinline void texture_corner_rotate(real x, real y, real cosine, real sine,
    real scale, real *coordinates)
{
    coordinates[0] = ((x * cosine - y * sine) * scale + 1.0f) * 0.5f;
    coordinates[1] = ((x * sine + y * cosine) * scale + 1.0f) * 0.5f;
}

// MiniMax H (11-14): no new form, no lower score (54).
// MiniMax B (11-13): no new form, no lower score (54): what is left is the scheduler's order inside the two corner sets, where this build places the x87 coordinate chains one slot earlier among the SSE ones than retail does (first difference +0x527); the corner rows in the other orders, the trig pairs as two statements and the point pairs all give 54 or worse.
// score 54 (was 54): earlier 94. Both trig pairs through the float library cosf/sinf (was (real)cos/(real)sin) 54 (94
// before; frame, bounds block, both trig sequences and every home retail's). Left: inside both corner sets
// the scheduler places the x87 coordinate chains one slot earlier among the SSE ones than retail does
// (first difference +0x527). Round 176 (m3 lane 5, fast = plain): the hand-written slot-12 shader call
// replaced by the existing function_1cc30(12) 274 (2013 B: kept out of line); on 54: corner rows with
// scale first, trig-first products, 0.5 first, 1.0 first, the second sum y first, each trig pair as two
// statements, point2f coordinate pairs: 54 each (same code); rows swapped 211 (2004 B); sine first in
// both pairs 149 (2009 B); the 23 other orders of the first set's four corner calls 57-233.
// MiniMax H (11-12): no new form, no lower score (94): the bounds block now follows retail's order; what is left is the inlined corner rotation's x87 and SSE schedule and its result homes, and the corner helper's two statements swapped give 214.
// MiniMax G (11-11): no new form, no lower score: the corner helper's two statements swapped 214 (2022 bytes, the
// same code as an earlier round); the helper keeps one expression per coordinate.
// MiniMax C (11-10): 94 (was 211): the four bounds are now written corner by corner in the
// order 0, 2, 1, 3 and the second angle's cosine and sine have their own locals, which reproduces
// retail's bounds block (0.5 first, the first zero compare after bounds[1]'s store, 240 in xmm7 and
// once as a memory operand); left: the inlined corner rotation's x87/SSE schedule and result homes.
// The helper's two products given their own named locals (u = x*cosine - y*sine, v = x*sine + y*cosine)
// gives 320 (1914 bytes), so that form is not kept and the helper keeps its single expression.
// MiniMax S (11-11): 94 (was 211): the bounds are written corner by corner (0, 2, 1, 3),
// which reproduces retail's block (0.5 loaded first, the first zero compare right after bounds[1]'s
// store, 240 in xmm7 and once as a memory operand); the corner sets' x87/SSE schedule remains.
// MiniMax S (11-09): no new form, no lower score (211); bounds/trig scheduling and the second
// cosine home remain.
// MiniMax E (11-09): no new form, no lower score (211); the first difference is the two early exits, whose targets are one byte apart - retail's reaches one byte further into the function than ours.
// MiniMax E (11-06): 211 (was 211): no new form lowered it. The inlined corner rotation still
// multiplies the two vectors in the opposite order and folds the constant as a memory operand where
// ours keeps it in a register, so the result homes and the x87 rows differ.
// MiniMax D (11-01): no new form tried, no lower score.
// MiniMax G (11-04): no new form, no lower score.
// MiniMax G (11-12): no new form tried this round, no lower score (94): the corner sets' x87/SSE schedule and
// the second cosine's home still differ from retail's, and the length already agrees.
// Earlier 94 (was 211): Frame 0x58; 2022/2022 bytes. Bounds/trig scheduling and
// the second cosine home remain; paired half extents recover native maximum/reciprocal registers.
// Earlier 247 (was 279): Frame 0x58; 2027/2022 bytes. Maximum/reciprocal and
// trigonometric scheduling remain; both normalized scales now use their native parameter results.
// Earlier 279 (was 282): Frame 0x58 agrees; six excess bytes, sampler constants,
// bitmap-call transport and scale/trig homes remain.
// Round 139: the first difference (+0x194, the stage loop's constants 2/3 in EBP/EDI where retail
// has EDI/EBP and 0 in EBX) follows from the 0x144f0 call: retail passes the fallback in EBX, so
// its 0 lives there. No new form tried (0x144f0 in src/unknown_01cf50.cpp is machine 3's per m3_owner.py, open to a later round).
// The stage call is BLOCKED-BY 0x144f0: retail passes tag in EAX, stage in ECX and the fallback in EBX
// (ret 0xc), and reads the first bitmap formal as a full dword (cmovne). Round 73: for loop 334;
// stage-nonzero ternary 334; selected bitmap local 294 at 2001 bytes (strength-reduced sampler table);
// short stage 500; unsigned stage 334; short-first selection 348.
// Earlier form: frame 0x58 agrees; stage-call convention and constants differ.
// Byte stage 617; distinct second trig scalars 374; separate texture scales 348
// (plain 348, retail length); vertical bounds first 378; selected short bitmap 374.
// Earlier form: Frame 0x58 and retail length agree; stage constants/call convention and UV/trig homes remain. Early texture scales 416; native UV pairs 348; conditional dimensions 395; shared alpha clamp 348; unsigned-word stage 493 (fast).
// Earlier form: Frame 0x58 agrees; stage constants, bitmap call and UV/trig homes differ. Tried word
// Frame 0x58 agrees; stage constants/call convention and UV/trig homes still differ.
// Existing helper producers: x rotation 348; y rotation 424; both 464; x scaled 348;
// y scaled 424; both scaled 464; both biased 465; output-first rotations 464. Original helper retained.
// frame 0x58 agrees; stage-call convention and constants differ.
// Frame 0x58 and retail length agree; stage constants/call convention and UV/trig homes remain. Early texture scales 416; native UV pairs 348; conditional dimensions 395; shared alpha clamp 348; unsigned-word stage 493 (fast).
// Frame 0x58 agrees; stage constants, bitmap call and UV/trig homes differ. Tried word
// dimensions 460; long dimensions 478; word width 444; word height 430; height first 352; word dimensions height
// first 460; long dimensions height first 478; word width height first 444.
// Normalization before states/stages 508/429; complete scale prefix before states/stages 424/348;
// bounds before states/stages 481/412; zero defaults before bounds 361; early normalized defaults 414;
// only maximum before stages/states 424/418; maximum with late zero defaults/early bounds 424/425.
// Native coordinate output 348; first/second/both corner-table loops 466/475/467;
// native output with both loops 467; native corner input 561; corner input loops 560;
// native corner and coordinate loops 560. Original helper and all eight calls restored.
// Original-body helper output/scale/rotation-first parameters 348/348/348;
// ordinary inline 348; separate trig scopes 351; later second output 348;
// separate output plus trig scopes 351; exact stage terminator 348.
// Round 147: native pair result 479; output pointer result 348; common-factor-first products 348; y-first
// stores 363; zero-safe results 348; ordinal scope 348; reverse scale updates 350; later first buffer 348.
// Round 148: first/second/both signed-word bitmap formals 334/373/348; signed-word stage with
// first/second/both 500/536/493; unsigned-word stage with first/second 500/536; on the retained
// contract, in-place scales/independent second trig/later second buffer, their pairs/all and all
// with trig scopes 368/338/334/368/368/338/368/368.
// Round 151: All four named rotation products, with paired rotation/scale/bias stage masks 0-7, each 391
// (1988/2022). Original corner helper and body restored; sampler and protected bitmap transport remain.
// Actual address/filter/disabled sampler tables alone 775/813/794; address+filter,
// address+disabled and filter+disabled 1072/1087/1092; all indexed/pointer walks
// 1262/1261. Original eleven sampler statements and helpers restored.
// Alpha clamps before initial setup first/second/both 445/433/469; before sampler
// setup 358/389/341; both before shader clear 353; both at admitted entry 481.
// Exact-only timing forms restored; original packing and sampler/draw calls retained.
// Native tag block/bitmap records separately/together, their read-only receivers and
// complete tag/bitmap observations at use: eight forms 334 each. Original receivers restored.
// Complete alpha parameter results, ternary/else-if/independent clamps 353 each; native rectangle
// field/aggregate producers 334/334; each clamp with native rectangle 353. Original body restored.
// Round 219: Normalized first/second/maximum scalar role-name masks 1-7 and all plus both trig
// names: 334/334/334/334/334/334/334/334. Original body restored.
// Round 201: split flag/tag rejection, positive flag-first admission, nested flag-first admission and
// nested flag rejection 334 each; tag-first split/positive/nested/mixed admission 370 each (2023 bytes).
// Original guard and all sampler, packing, scale, rotation and vertex operations restored.
// Round 204: Complete dimension/tag, sampler-loop and scale/bounds/trig producer workspace
// endings, masks 1-7, and all with immutable maximum/reciprocal: 334 each. Original
// scalar/output lifetimes, helper calls, state order and arithmetic trees restored.
// Signed integral corner inputs char/short/int/long in the existing rotation helper
// 334/334/334/334; each with native coordinate outputs
// 334/334/334/334. Original helper and all eight calls restored.
// Round 378: Paired scaled pixel diameters 282; complete half extents 334;
// native rectangle 282/334; independent second trig 280/338; rectangle plus trig
// 280/338. Keep only paired diameters 282 (plain), 2028 bytes; the extra two-point
// trig step is rejected. Original trig storage, arrays, helpers and calls retained.
// Round 383: paired diameter products before normalization 356; full bounds before normalization 354;
// complete horizontal/vertical product-and-bound phases 336/340; native rectangle counterparts
// 356/354/336/340. Original reciprocal, product overlap and body restored.
// Round 166: paired corner production with both angle pairs live, explicit/first-corner-first/
// table-loop/table-loop after first corner 407/368/358/361; native point outputs
// 407/368/358/361. Original separate UV producers and trig lifetimes restored.
// Round 169: reciprocal-first normalization 282; branch-owned outputs 340, with reciprocal-first
// 340; incoming scale results 343, with reciprocal-first/exclusive maximum 343/343;
// branch-owned outputs with exclusive maximum 340; paired dimension-first products plus
// reciprocal-first normalization 282. Original paired diameters, scale and trig homes retained.
// Round 180: Entry declarations of first/second/bounds output buffers, masks 1-7, then all with
// entry trig declarations: 282/282/282/282/282/282/282/282. Original executable body restored.
// Round 74: stage name 282; entry stage 443; entry stage with scaled-dimension names 443 and
// with a reciprocal name 443; original-position stage with both scalar names 282; both scalar
// names 282; scaled-dimension names 282; reciprocal name 282. Original executable body restored.
// Positional counts here are dominated by the blocked stage call: the stage loop has a different
// instruction count, so every later line is shifted. Aligned (difflib) leads: scales normalized in
// their own parameters (retail's homes) 343 (aligned 134 -> 123); with half extents
// `scale * width * 0.5f` 368 (aligned 110, 2007 bytes); a long first bitmap formal gives retail's
// cmovne (with half extents 348, aligned 114; with both 374, aligned 95), but its declaration
// is in a contributor file.
// Round 43: the alpha clamps already have retail's store-then-overwrite shape; renaming the inlined
// 0x131fc0's scale local (diagnostic) leaves its param-home slots (scale 0x6c, pixel 0x70; retail the
// reverse) unchanged, 282. Stage loop still BLOCKED-BY 0x144f0 (machine 3's, src/unknown_01cf50.cpp).
// Round 133: 0x131fc0's locals declared pixel-first 282 (0x131fc0 then differs, 2); scales normalized
// in their parameters 343 (2013 bytes); with half extents 368 (2007); half extents alone 334 (2022);
// coordinate arrays declared second-first 282; declared just before the bounds 282.
// Round 122: 0x144f0 now matches, called (stage, 0, tag, 0, index, 0.0f). That alone 366 (2044 bytes); with the first
// bitmap formal `long` (retail reads it as a dword and selects it with cmovne, like the second) 279 (2042): the stage
// loop is now retail's instruction for instruction, and the rest is the six excess bytes and the homes above.
// Round 74 (lane 9, fast; 0x144f0 edits temporary, restored): BLOCKED-BY 0x144f0; see the form that matches it
// in 0x2a8d0's note. This function's count under each callee form: fallback read directly 283; plus an
// address-taken priority 280; direct without the volatile 283; two calls 366 (2044 bytes, a positional shift,
// first still +0x17); two calls in order (tag, fallback, stage) 345; (fallback, tag, stage) 346; (stage, tag,
// fallback) 366; (stage, fallback, tag) 366; (fallback, stage, tag) 365; with the matching 0x144f0 366.
// J242: First-parameter native results 260/260; second 273/273, NOT kept: incomplete parallel
// result contracts without a match. Both parameters 247/247, first form plain-confirmed 247 and
// kept; branch-owned results 251/251. Frame 0x58 and packed-alpha prefix agree; five excess bytes
// and maximum/reciprocal/bounds/trigonometric scheduling remain.
// DH318: Maximum second-default/first-default/if-else/const results, mutable/const reciprocal on paired
// parameter outputs: 258/258/255/255/247/247/247/247. Maximum/reciprocal allocation, five excess bytes and
// trig scheduling remain. All restored.
// Round 38 (lane 17, plain, base 247, aligned 107): the last bound written `scaled_height * 0.5f + 240.0f`
// (retail adds 240 from memory) 247; the two zero-scale tests before the bounds 253 (2023 bytes); bounds[1]
// before bounds[0] (retail's store order) 247; the maximum as `scale = second_scale; if (first > second)`
// 258; the scaled products as `width * scale`/`height * scale` 247.
// Y557: Independent angle pairs on the normalized-parameter graph, with
// immutable pairs, first-producer scope and late second-output declaration, all
// eight forms 246 at 2027/2022 bytes. NOT kept: one-point gain below eight;
// original trig carriers, output declarations and complete body restored.
// F357: Float trig APIs at both angle producers, cosine/sine/both with
// reused pairs 250/250/250, independent pairs 249/249/250; complete immutable
// pairs and complete producer scopes 250/250.
// NOT kept: all regress; original real results, helper, normalized parameters and body restored.
// AC506: Both trigonometric pairs in double result carriers, explicit-real/helper-input
// rounding with mutable/immutable and first-producer scopes: eight at 331 (1972/2022 bytes).
// NOT kept: precision/transport forms regress and move the extent away; original float
// carriers, helper inputs, rounding and complete body restored.
// Y563: Native real cosine/sine/both APIs with reused and independent pairs,
// then both with late output/immutable pairs: 250/250/250/249/249/250/250/250.
// NOT kept: all regress; original APIs, trig carriers, helper and complete body restored.
// Y565: Paired half extents on native normalized-parameter results, retained diameters/direct
// products and joint/axis-local production, original array/native box outputs: eight at 211,
// 2022/2022 bytes. Keep direct paired half extents (plain 211); each has two original bounds
// consumers and retail retains both products. Original grouping, calls, trig and arrays retained.
// F380: Zero/nonzero comparison results spanning vertical bounds, bool/byte/short/long,
// 240/240/244/243/240/240/244/243. NOT kept: regressions and longer extent;
// original comparison/default points, half extents, trig and complete body restored.
// DH358: Complete zero-result lifetimes before half extents/bounds and at horizontal/vertical
// consumers 246/239/217/234; stream-boundary pairs 150/146/208/223. NOT kept:
// 150/146 move extent from 2022 to 2026; 208 gains only three and shrinks to 1972.
// Original tests, half extents, trig carriers and whole body restored.
// Round 132 (m3 lane 7, fast, base 211): the corner helper taking the angle, cos/sin as expressions
// or as helper locals 265 (2048 bytes)/283 (1987); as an expression macro (0x3a5c0's fix) 211 (same
// code); the second angle's cosine and sine as their own locals 210 (second cosine in retail's 0x7c
// home), with `half_width + 320.0f`/`half_height + 240.0f` 210, with bounds[1] first 223; bounds[1]
// first alone 224; vertical bounds first 217. Lead: ours has one instruction more than retail (480):
// ours keeps 240 in xmm1 for both vertical bounds (a copy), retail loads it into xmm7 after bounds[0]'s
// store and adds the second 240 from memory, because its zero for the scale tests already holds xmm1
// (the compare is hoisted into the bounds); that one instruction shifts the whole vertex tail (~100).
// Round 85 (m3 lane 10, fast, base 211; exact aligned count in brackets, base 55, with every stack home already retail's): horizontal bounds then the half height
// and vertical bounds 211 [55]; the first zero test between the horizontal and vertical bounds 217 [82], both tests there 230 [94], the first before all bounds
// 232 [85], after the horizontal bounds with the half height after it 231 [102]; `half_height + 240.0f` 211 [55]; the zero defaults as ternary assignments 211
// [55]; `0.0f == scale` 211 [57], `!scale` 211 [55]; bounds stored 1, 0, 3, 2 224 [69], 0, 1, 3, 2 211 [56]; named scaled width/height halved at each bound 247
// (2027 bytes); the bounds as four scalars 408 (1994 bytes), with the zero tests first 401 (1999); the second angle's cosine and sine in their own locals 210
// [53] (round 132's form again; under the bar). The corner helper with the products' sides swapped, `0.5f * (1.0f + scale * ...)`, `scale * (...)` first or
// the second sum's terms swapped 211 each (same code); with named rotated x/y locals 346 (1910 bytes). The first zero comparison held in a bool across the
// vertical bounds 240 (2031 bytes), both 264 (2041), the bool before all bounds 256, after bounds[2] 236, used before bounds[3] 240. Left: retail starts the
// first zero comparison right after bounds[1]'s store, so its zero holds xmm1, 240 is loaded into xmm7 and subtracted in place and the second 240 is a memory
// operand (ours copies it, one instruction more); in the first corner set retail schedules coordinate 7 ahead of 1, 6 and 2 where ours runs 1, 2, 6, 7.
// MiniMax R (11-10): no new form, no lower score; the vertical bounds still keep 240 in one register for both of them, where retail loads it into xmm7 after the horizontal bound's store and adds the second 240 from memory, because retail's zero for the scale tests already holds xmm1 with the first test hoisted into the bounds; moving that test between the horizontal and the vertical bounds (217), before both bounds (239), swapping the vertical pair (211) and swapping the halves (221, 225) all score worse, and the best lead, the second cosine in retail's home, is 210.
// Round 102 (m3 lane 9, fast, base 211): bounds declared before the half extents 211; the bounds as one initializer
// 211; the two zero tests in the other order 223; bounds declared with the entry locals 211; the half extents before
// the scale normalization 246 (2024 bytes); the maximum as `scale = first; if (second >= first) scale = second;` 221;
// both corner buffers as one two-row array 211. A 1800 s permuter run was cut off by its 2400 s cap before reporting.
// Round 104 (m3 lane 9): LEAD, match-only (a statement order): the compile-only harness reproduces the bounds block,
// and of all 560 orders of its eight statements only the bounds written corner by corner, 0, 2, 1, 3, give retail's
// block (0.5 loaded first, the first zero compare right after bounds[1]'s store, 240 in xmm7 and once as a memory
// operand): 211 -> 94 fast at 2022 bytes, first difference +0x17 -> +0x524 (build/r104/lead-29b00.patch in the
// m3-lane9 worktree); with the second trig pair in its own named locals the second cosine also takes retail's +0x7c
// home, 94 (aligned 73). Left there: only the two corner sets' x87/SSE schedule. On that base: the helper's two
// statements swapped 214; products written cosine/sine first 94; the second sum y-first 94; ordinary inline 94; the
// first trig pair as two statements 94; four helper parameter orders 94; named rotated u/v 320 (1914 bytes); the four
// corner calls of both sets in all 24 orders, main's order best (94), then 96. A 600 s permuter run found nothing:
// its full build took 972 s and its reduced image scores 529 against the full image's 211. The compile-only harness
// reproduces the corner region of that base exactly; the helper's spellings (four u sums, three v sums, scale/bias/half
// operand sides, statement order: 192 forms, 6 distinct codes) give nothing closer to retail than main's spelling.
// MiniMax R (11-11): no new form tried this round, no lower score (94): the body is retail's length and the first difference is at +0x524, where the address of a global that is not matched yet differs, so most of the score here is the unmatched globals and not the statements.
// M3 lane 8 r161 (fast with 0x144f0, 0x14bc0, 0x1c590, 0x1c710, 0x1ccf0, 0x1cf50, 0x131fc0, 0x222d0, 0x288e0, all MATCH;
// control 54): each angle's cosine and sine in a two-element array passed to the corner helper by pointer 165 (2010 B);
// the corner helper plain `static inline` 54 (same code); a four-corner helper per set taking the cosine and sine 54
// (same code), taking the angle and computing the pair itself 127; both corner buffers as one two-row array 54 (same code;
// re-measure of the 211-era form). Left: in both corner sets our scheduler issues each x87 chain step one slot ahead of
// retail's (first at +0x527: the c[3] `fstp` ahead of c[1]'s `subss`). Body unchanged.
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
    if (screen_target)
        function_14bc0(0, 0, false);
    else
        function_14bc0(*(short *)(g_4b99b0 + 0x34), 0, false);
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
        function_144f0((short)stage, 0, tag, 0,
            (short)(stage == 0 ? first_bitmap : second_bitmap), 0.0f);
        sampler_state_write(stage, D3DTSS_ADDRESSU, 3);
        sampler_state_write(stage, D3DTSS_ADDRESSV, 3);
        sampler_state_write(stage, D3DTSS_ADDRESSW, 3);
        sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
        sampler_state_write(stage, D3DTSS_MINFILTER, 2);
        sampler_state_write(stage, D3DTSS_MIPFILTER, 2);
        sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
        sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
        ++stage;
    } while (stage < 2);
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x21;
    g_484f68.PSCombinerCount = 0x11002;
    g_484f68.PSConstant0[0] = function_131fc0(first_alpha < 0.0f ? 0.0f : first_alpha > 1.0f ? 1.0f : first_alpha);
    g_484f68.PSConstant1[0] = function_131fc0(second_alpha < 0.0f ? 0.0f : second_alpha > 1.0f ? 1.0f : second_alpha);
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
    real half_width = (scale * width) * 0.5f;
    real half_height = (scale * height) * 0.5f;
    real bounds[4];
    bounds[0] = 320.0f - half_width;
    bounds[2] = 240.0f - half_height;
    bounds[1] = 320.0f + half_width;
    bounds[3] = 240.0f + half_height;
    if (first_scale == 0.0f) first_scale = 1.0f;
    if (second_scale == 0.0f) second_scale = 1.0f;
    real first_coordinates[8], second_coordinates[8];
    real cosine = cosf(first_angle), sine = sinf(first_angle);
    texture_corner_rotate(-1.0f, -1.0f, cosine, sine, first_scale, &first_coordinates[0]);
    texture_corner_rotate(1.0f, -1.0f, cosine, sine, first_scale, &first_coordinates[2]);
    texture_corner_rotate(1.0f, 1.0f, cosine, sine, first_scale, &first_coordinates[4]);
    texture_corner_rotate(-1.0f, 1.0f, cosine, sine, first_scale, &first_coordinates[6]);
    real second_cosine = cosf(second_angle), second_sine = sinf(second_angle);
    texture_corner_rotate(-1.0f, -1.0f, second_cosine, second_sine, second_scale, &second_coordinates[0]);
    texture_corner_rotate(1.0f, -1.0f, second_cosine, second_sine, second_scale, &second_coordinates[2]);
    texture_corner_rotate(1.0f, 1.0f, second_cosine, second_sine, second_scale, &second_coordinates[4]);
    texture_corner_rotate(-1.0f, 1.0f, second_cosine, second_sine, second_scale, &second_coordinates[6]);
#pragma inline_depth(0)
    D3DDevice_Begin(D3DPT_TRIANGLEFAN);
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
#pragma inline_depth(255)

struct s_bitmap_view;
struct s_type_7ba8e9;
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
#include "texture_stage_storage.h"
extern dword g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324;

struct s_2a2f0_record
{
    long y;
    real speed;
    long unused;
};

#pragma inline_depth(0)
// draws five full-width strips textured with the tag's bitmaps, each scrolled by the game time
// @retail 0x2a2f0
void __stdcall function_2a2f0(long tag, real alpha)
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
        for (long i = 0; i < sizeof(records) / sizeof(records[0]); ++i)
        {
            byte *definition = g_4e3b44[definition_index].bytes;
            real &height = *(real *)&tag;
            real &width = alpha;
            height = 1.0f;
            width = 1.0f;
            if (i < *(long *)(definition + 0x44))
            {
                byte *bitmap = *(byte **)(definition + 0x48) + i * 0x74;
                real local_62a74a_2 = (real)*(short *)(bitmap + 4);
                width = 1.0f;
                if (!(local_62a74a_2 < 1.0f)) width = local_62a74a_2;
                real local_98083a = (real)*(short *)(bitmap + 6);
                height = 1.0f;
                if (!(local_98083a < 1.0f)) height = local_98083a;
            }
            long count = *(long *)(g_4e3b44[definition_index].bytes + 0x44);
            byte *selected;
            if (count > 0)
            {
                short index = (short)((short)i % count);
                byte *group = g_4e3b44[definition_index].bytes;
                selected = NULL;
                if (group && index >= 0 && index < *(long *)(group + 0x44))
                    selected = *(byte **)(group + 0x48) + index * 0x74;
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
                            long width = *(short *)(bitmap + 4);
                            long height = *(short *)(bitmap + 6);
                            g_485af4[0] = width;
                            g_485b04[0] = height;
                        }
                    }
                }
            }
#pragma inline_depth(255)
            sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
            sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
            sampler_state_write(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
            sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
            sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
            sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
            sampler_state_write(0, D3DTSS_COLORSIGN, 0);
            sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
#pragma inline_depth(0)
            function_1cf50();
            s_2a2f0_record *line = &records[i];
            real center = 240.0f - ((real)line->y - height * 0.5f);
            box2f bounds;
            bounds.x0 = 0.0f; bounds.x1 = 640.0f;
            bounds.y0 = center - height * 0.5f;
            bounds.y1 = center + height * 0.5f;
            real time;
            if (g_510c54 && g_510c54->active)
                time = g_510c54->game_time * g_510c54->rate;
            else
                time = 0.0f;
            real offset = 0.0f - line->speed * time;
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

extern dword g_4b82e8;
__declspec(noinline) double __stdcall function_2b480(real value);
dword __cdecl pack_color4f(color4f const *color);


#define FILTER_PIN(x) ((x) < 0.0f ? 0.0f : (x) > 1.0f ? 1.0f : (x))

PRIVATE __forceinline dword filter_output_shift(long exponent, long threshold)
{
    return ((exponent > threshold + 1 ? 0x20 : exponent > threshold ? 0x10 : 0) << 12) | 0xd00;
}

PRIVATE __forceinline dword filter_output_first_shift(long exponent)
{
    // retail only ever halves here; no 0x20 step for large exponents
    return ((exponent > 0 ? 0x10 : 0) << 12) | 0xd00;
}

/* the floored value clamped to a byte; mode zero floors its first test directly */
#define FILTER_QUANTIZED_BYTE(value, direct) \
    ((long)((direct) ? floor((double)(value)) : function_2b480(value)) < 0 ? 0 : \
    (long)function_2b480(value) > 255 ? 255 : (long)function_2b480(value))

/* blending and depth states of the filter passes; retail calls every one of these
   state writes out of line, and each caller writes the depth bias itself */
#pragma inline_depth(0)
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
}
#pragma inline_depth(255)

// score 9 (was 9): retail length (19 before round 134); the blend sites now call 0x222d0 as retail. Left: the input
// word's shift, folded into EBX before the clear where retail copies EBX to EAX and shifts it
// after the clear; the two gain sites, where ours loads the 1.0f constant before the reload; the two small
// selectors (retail loads small into CL first and computes input before the alpha selector) and the
// constant/inverse-span reloads after mode zero and one's depth-bias write (retail reloads the span
// first). Modes one to three's quantizer inputs now take clip's home as retail (round 132 below).
// MiniMax G (11-06): no new form, no lower score; tried the input word as a multiply, the span
// scaled first, the input named after the clear and the shift written at both stores.
// Round 137: one function-scope value for every quantizer, with the quantizer a macro, puts modes
// one to three's inputs in clip's home as retail 19 (a block-scope value 31; the inline helper with a
// by-value parameter keeps a separate symbol per expansion); whole/alpha/fraction/input 18 (aligned 20).
// NOT kept: the macro and the shared function-scope value shape lifetimes, and the order was picked by score
// from 24; body restored at 31. In that build 0x41640 (0350e0.cpp, score 4) matched with no edit to
// its file (a link-time side effect): a lead for 0x41640's own source.
// On it, aligned counts in brackets: whole/fraction/input/alpha 939 [15] (one instruction short, no
// `mov al, cl`), with small declared after the switch 31 [6]: only the false store (after the switch,
// retail before it) and the reload pairs differ; alpha/whole/fraction/input 19 [18]; the other 20 orders
// 23..970; a second input selector 225 [30]; a bool copy of small 939 [15]; negated selectors 939 [15];
// bool/byte/char/long/short small 939/939/939/508/506; small at each earlier declaration position 938..998;
// reversed loop comparisons 19/19; exponent loop at the mode chain 19; named span 19; maximum at the switch
// 22; inverse distance first 26; no result variable 1105; result declared at the first failure 1131.
// Earlier 226 (was 949): length and prologue agree; mode zero and one still inline the
// blending helper's ZENABLE write (VC's inline budget: retail inlines only the first seven
// state writes, ours one more cheap one), the small selector is loaded after the fraction move,
// and the quantizer temporaries of modes one to three take shift's home where retail takes clip's.
// Earlier: frame 0x2c agrees; small permission now has retail byte tests,
// but mode-bound/save roles, state expansion and scalar homes differ.
// Earlier: Frame 0x2c agrees; capability/save roles and state expansion remain.
// frame 0x2c agrees; saved registers, mode flags and state-call expansion differ.
// Short capability flags 1195; short rescale state 1204; color homes renamed 978;
// eligibility-first bound 1212; earlier input selectors 979; blending helper depth one 1393.
// frame 0x2c agrees; saved registers, mode flags and state expansion differ.
// Inner depth pairs removed 1212; direct texture API 983; dispatch defaults 1224;
// normalization after dispatch 984; four-use alpha snapshot 971.
// Byte capabilities 1086; rescale-state name 978; case three first 978;
// short exponent 1084; later integer decomposition 977; exponent for-loop 1273.
// Frame 0x2c agrees; capability/save and state expansion remain. Span power 1080 (retail length); integer range samples 971; quantizer result 1201; shift result 1197; slope/cutoff homes 971 (fast).
// Frame 0x2c agrees; capability/save and state expansion remain. Initial state helper 1179; normalized bounds 985; split zero defaults 942 discarded; dispatch lifetimes 971; branched depth result 971; shared cutoff default 1055 (fast).
// Frame 0x2c agrees; capability/save and state expansion remain. Shared mode-
// two/three tail 1153; redundant case defaults removed 1193; common exponent outputs 1365; second
// packed color shared 1101; byte fraction 971 (fast).
// Frame 0x2c agrees; capability/save roles, state expansion and
// color/quantizer homes differ. Word success 1194; unsigned exponent 1197; mode decision chain
// 1084; word color input 971; word alpha input 1181 at retail length.
// Capability declaration/name order 971/971; late rescaling flag 971; native color aggregates 971;
// conditional-update shared clamp 1207 (fast). Original body and shared clamp retained.
// round 106: Frame 0x2c agrees; capability/save and state expansion remain. Packed range clamp 1199; early exponent lifetime 1201; shared gain divisor 1194; late input selectors 1204; late clipped width 970 (one-point gain rejected).
// Negative permission at use 1187 (retail length); small permission at use 1192; both at use 1190;
// native permission bit mask 1183; shared clamp early returns 971.
// Mode cases 1/0/2/3, 2/0/1/3 and 0/2/1/3 971 each; 0/1/3/2 and
// 2/3/0/1 973 each; function-scope color inputs 971; shared upper/lower
// quantizer scope 971; native long success 1194.
// Mode assignment orders MNS/SMN/SNM/NMS/NSM 973/971/971/973/973; NSM in cases
// 0/1, 2/3, 0/2 971/973/971 (fast; M=maximum, S=allow_small, N=allow_negative).
// Round 364: input byte/alpha byte/both 1185/934/980; signed input/alpha/both
// 1185/934/980; short-input with byte-alpha 935; unsigned-input with byte-alpha 934.
// Byte alpha selector alone kept plain at 934; its only values remain 4 and 24.
// Round 366: blend/depth/both indexed state tables 1395/1417/1484; shared sampler 1223; sampler with blend/depth/both tables 1413/1410/1550; all with Boolean capabilities 1448. All restored.
// Test-defined rescale state, canonical permission-width pairs and negative-
// permission failure-first combinations 1216, 1205, 1208, 933, 1186, 1184, 1180, 1183 fast.
// Round 159: Upper/lower/both byte channels 1119/1202/1178; upper/lower/both word channels
// 1161/1202/1231; rounded result read once 1375; read once with byte channels 1325. All restored.
// Round 232: native mode-one/two/three input layouts, all seven subsets 934;
// complete premultiplied-color production with all layouts 935. All restored.
// Round 164: bounded byte exponent alone/with reused threshold predicates 1182/1165;
// signed byte 1013/1207; word 1200/1246; reused thresholds alone 1204; shared shifted
// color-input word 934. All restored; original values, calls and directives retained.
// Round 166: negative permission as bool 853 (plain), both permissions remain 0/1;
// original long small-permission retained. Other values, operations and calls unchanged.
// Round 166: Permissions small/negative: bool/bool and signed/signed 1088; short/byte 854;
// byte/short 931; short/bool 854; bool/short 931; signed/byte and byte/signed 1088. Direct first-
// alpha fields: long/long 942; bool/bool and signed/signed 1093; short/bool and short/byte 859;
// long/bool 858. Carrier ablations: short/long 936; long/bool 853 plain, kept; word/bool,
// word/byte and short/signed 854. On the recovered long/bool graph, redundant case 1/2/3/all false
// stores removed 1167/855/1186/1195; shared mode tail alone/with case-one cleanup 1186/1190;
// complete case results 1219; maximum result 1210. Restore all other carriers, memory
// observations, cases and result lifetimes. Frame 0x2c agrees; mode bound ESI/EDI, full-width
// small-permission test and state expansion remain.
// Round 282: complete encoded output words, ternary/descending/result/ascending forms,
// each with original/complete first-output word [1128,1131,1128,1131,1121,1121,1129,1130].
// Original output helpers and complete body restored; capability/save roles remain.
// Round 393: complementary small/negative/return-state subsets
// 932/858/933/934/1212/935/1213; all with Boolean small-denial 1210. Original carriers restored.
// Round 261: shared quantizer byte/word/dword results 985/1007/853; quantizer/output/both
// formal orders 853 each; byte/word results with both orders 985/1007. Original helpers restored.
// Round 187: Integral slope cap long/short/word/dword 1141/1161/1161/1082; each with inverse-span entry workspace 1141/1161/1161/1082. All restored; frame/save and state expansion remain.
// Upper-default shared clamp, branch/entry result with original/Boolean permission and
// sampled/live alpha: 1118/1120/1101/1103/1105/1109/1085/1089. Retail length in all forms; original helper/body restored.
// Aligned (difflib) leads, all longer than retail: the eleven stage states through sampler_state_write
// (retail's per-state dirty-flag ors; it never calls 0x224f0 here) 1143 (aligned 218 -> 202); with a
// bool small permission 1121 (192); with mode one's eight clamps as FILTER_PIN over fields read at
// each use (0x25e50's form) 1174 (173: mode one then matches but for registers); with the offset and
// gain clamps as FILTER_PIN too 1171 (102); a long alpha selector instead 1154 (204). Left: our link
// inlines 0x222d0 at mode zero's eight blending states and the final depth state (retail calls all 36
// out of line), and the offset clamp branches where retail stores 1.0 and conditionally overwrites it.
// Round 224: existing blending helper inline/ordinary declarations with bool/byte/
// short/long additive input 1457/1457/1453/1453/1457/1457/1453/1453. Actual-mode helper
// long/short/byte/dword under force-inline 853 each, under inline 1446/1455/1534/1446.
// Original helper contract, all call inputs and complete body restored.
// Round 444: small/negative permission pairs byte/bool, signed-char/bool, bool/byte
// 1083 each; bool/word, byte/word, signed-char/word 1187; dword/bool and dword/byte
// 1109. Byte small permission alone kept after full plain confirmation (4832/4828 bytes).
// Round 234: Packed fraction byte/word, RGB byte/word/dword, alpha word/dword and combined native
// channels on the long/bool capability graph: 903/852/1112/853/853/1101/1107/1111. All restored;
// capability/save roles and state expansion remain.
// Round 449: cutoff floor before state setup, fixed-only/full decomposition,
// with ordinary/scoped normalization reciprocal: 1197/1197/1127/1127; each
// with exponent computed before setup 1121/1121/1151/1151. Original scalar
// lifetimes restored; mode-bound register and state expansion remain.
// Round 451: Byte/signed-byte success crossed with bool/byte/signed-byte/word/long rescale storage:
// 1194/1194/1194/1194/1202/1202/1187/1187. Original status carriers restored.
// Round 468: Late descriptor arms 1023/2013/3012/0213/0312/1230/2301/3210
// 972/1112/1106/1085/1081/1117/1086/1119. Order 1023 kept plain at 4848/4828 bytes;
// order 0312 reaches retail length but gains only two. Initial capability switch unchanged.
// Round 474: Complete encoded mode-three pair 949 (plain, 4828/4828 bytes);
// mode-zero RGB 972; pair/RGB 949; final word 1100; pair/final 1074;
// RGB/final 1100; all 1074; all/full alpha 1083; first/second word alone
// 954/967. Pair kept; mode-bound/save roles and state expansion remain.
// Round 217: Complete span/scaled-endpoint/center-term expression boundaries, seven subsets,
// then all with immutable normalization reciprocal: 1083/1083/1083/1083/1083/1083/1083/1083. Original arithmetic-expression spellings restored.
// Selector 3 and full-word alpha, exact-only sampler/upper-default/live-field subsets: 1085/1092/997/1102/1122/1147/1124/1170. No nonmatching selector-3 form retained.
// Clean upper-default clamp with original/full-word alpha: 1099/1100.
// All clean forms restored too; no eligible gain.
// Round 43 (aligned difflib counts in brackets): retail tests the modes 0, 1, 2, 3: that chain 1094 [286];
// with the stage states through sampler_state_write 1070 [274], plus a long alpha selector 1069 [224]
// (byte selector 1078 on the final form); every render state through function_0222d0 without the
// inline_depth pairs 1120 [224], with 0x224f0 stage states 1160/1148; the seven skip-state states through
// function_0222d0 with the existing pairs 1122 [203]; floors through 0x2b460 1120 (same code); mode one's
// clamps as FILTER_PIN over fields read at each use 1079 [188], offset/gain clamps too 1080 [108]; mode
// three's selected inputs through PS_COMBINERINPUTS 1108 [51]; the blend-mode setter 0x142f0(10) in the
// skip-state block 1219; skip-state ZBIAS through function_0222d0 571 [48]; RGB input 7 through
// PS_COMBINERINPUTS 1094 [46]; the offset clamped as 0x2a2f0 clamps its alpha (zero, else one then the
// value unless above one) 329 [45], as if plus ternary 1094; the blending helper's ZBIAS through
// function_0222d0 328 [42]; `fraction = whole = 255` 326 [40]; extra_texture declared at function or chain
// scope 326/326; a plain-inline stage-state helper 1374; a plain or non-inline blending helper 1556/1556;
// function_14f60(0, 3) as retail passes it (EDI still holds the bound 3, as in 0x25e50 and 0x26e50) 935
// [27] (prologue and registers now agree); the four selector/split declarations in all 24 orders
// 226..980 (alpha before input 227 [41], kept); floors through 0x2b460 again 227/228.
// Round 133: the blending helper under a file-scope inline_depth(0) pair 250 (4812 bytes: ZBIAS
// called too); with its ZBIAS write moved to the four callers 31 (4828, kept); the redundant pair
// around DEPTHCLIPCONTROL removed 31 (kept). On that: the quantizer input by reference 31, a
// function-scope input shared by all quantizers 31, both 31 (the slot stays shift's); fixed/
// whole/fraction/alpha/input orders fwria 939, fiwra 965, fwira 976, fiawr 35, faiwr 950,
// iafwr 43, aifwr 959, frwia 771, frwai 452, fawri 31; alpha selector at its use 947; both
// selectors at their uses 975.
// Round 336: paired-selector branch/default forms 952/952/952/952/948/959/959/948;
// clip/shift admission, width lifetime and negative-arm subsets 39/37/45/1092/1092/1097/1097,
// nested rescale admission 1146; gain-span/all-quantizer producer boundaries and both,
// then with the fixed sample, 31/31/31/31. Original complete body and helpers restored;
// selector scheduling, reciprocal reload order and shift/clip quantizer homes remain.
// Round 267: shared selector decision long/byte/short/dword before floor 250/31/964/250;
// at selector production 243/31/957/243. Original body restored; selector order,
// span/constant reload pairs and shift/clip quantizer homes remain.
// Round 315: Shared quantizer complete result, lower-default, conditional and upper-default
// graphs on the recovered state-call graph 31/787/31/690; each with paired native input
// expression names 31/787/31/690. Original helper/body restored; selector scheduling,
// post-bias reload order and quantizer shift/clip homes remain.
// Round 462: opacity-selector/lower-adjustment/quantizer-sample lexical homes, all seven
// nonempty subsets 31 each; all with color-selector declaration first 939. Original names,
// selector producers and shared quantizer restored; reload pairs and shift/clip homes remain.
// Uniform parallel scale/gain product operand directions, seven subsets and all with the
// cutoff product reversed, 31/31/31/31/31/31/31/31; original product inputs restored.
// J242: Native truth-valued parameter carriers: clip/shift/center/skip-state byte singly,
// clip+shift, with center, with all four, and signed clip+shift: 31 each at 4828/4828. NOT kept:
// all neutral and equivalent input-type selection requires byte equality. Original complete body,
// four bool inputs, every helper and call tuple restored; selector register production, post-bias
// reciprocal/constant load order and six shift-versus-clip quantizer homes remain.
// DH318: Existing quantizer direct-floor/rounded lower tests separated, nested/logical forms, bool/byte
// flag and mutable/const sample: 31/31/31/31/31/31/31/31. Selector, reload-pair and six sample-home
// differences remain. All restored.
// Round 38 (lane 17): retail keeps the quantizer's temporary at [esp+0x58] apart from the clamped gain
// product's at 0x54; ours shares 0x54. Renames for the slot order: the quantizer's input `amount` 31
// (plain), the quantized locals `upper_byte`/`lower_byte` 31, `raw_offset` 31, `span_gain` 31 (fast,
// slots unchanged); the input selector `4 + small` 961 (fast); permuter (41 variants) best 31.
// Y557: Recyclable shift/clip/center parameter-symbol role names in
// seven subsets, then all with the skip-state role name: 31 each at 4828 bytes.
// Original parameter symbols restored; selector scheduling, post-bias reload
// order and six quantizer inputs in shift rather than clip home remain.
// Round 50 (m3 lane 2, fast; not kept, body restored at 31): round 137's shared function-scope value repeated without its macro: assigned before each call of the
// by-value or by-reference helper 19/19 (the helper's parameter does not keep modes one to three in shift's home once its argument is that value); per-expansion block-scope
// values by reference 31. On that base: the four selector/split orders, all 24, wari 18, wria 939 (aligned 15), awri 19, iwar 23, iawr 23, others 231..976; input or alpha typed dword,
// short, byte, char on wria 939/939, 934/477, 945/960, 945/960, on wrai alpha 19/958/957/957; small typed byte/char/long on wria 939/939/496, on wrai 19/19/1179; a named bool or byte selector
// copy at three positions 939/939/939 (forward-substituted), alpha first 19/19; selector and split force-inline helpers, flag parameter first or second, or selectors alone, 939 with
// input first, 19 with alpha first; declarations apart from assignments in whole, input, fraction, alpha order 976 in six declaration orders; gain as one expression, two statements over
// the ratio, or one expression plus the span 95/19/95, a separate 1.0f start 19; the span named ahead of the gain in both/zero/one modes 837/51/53, the span term first 118; span or
// shared value declared at the top, before the maximum or after the inverse distance 19; small set after the switch with wria 31 (aligned 6: only that store's place and the two reload
// pairs differ, the selector block is exact); that store right before the switch, after low or after the inverse distance 998/997/996 (aligned 24), declared after high or the inverse
// distance 998/996; a false-valued local before the switch copied after it 31 (constant-propagated). Any store before the switch loses retail's `mov cl, [small]`.
// DH325: Existing quantizer inline/natural expansion, immutable value/direct
// input subsets 0-3 in each: 1201 each, 4420/4828 bytes. All restored; native
// expansion still needs force-inline. Selector/reload/sample homes remain.
// DH327: Existing quantizer writes native integer outputs, pointer/reference
// destinations with nested/guard/default/conditional clamp graphs:
// 31/31/787/31 twice. Original output contract, helper and complete body
// restored; selector scheduling, post-bias reloads and shift/clip homes remain.
// N350: Immutable shift/clip/center parameter subsets and all with immutable
// state flag, eight forms 31 each at 4828 bytes. Original declarations and body restored;
// selector, reload-pair and six quantizer input-home differences remain.
// F391: Existing quantizer policy as bool/int template argument, mutable/immutable real
// input and conditional/separate lower admissions: 1218 each in eight forms, 4404/4828 bytes.
// NOT kept: specialization outlines both quantizers and moves extent away from retail.
// Original helper, runtime policy formal, eight calls, repeated conversions and whole body restored;
// selector scheduling, post-bias reload pairs and six shift-versus-clip input homes remain.
// N386: Existing quantizer explicit cdecl/stdcall inputs, bool/byte policy and mutable/immutable
// real samples: eight 31s at 4828 bytes. NOT kept: no byte equality; original helper and body restored.
// P332: Native quantizer lower/upper/both conversion producers 32/31/32;
// split lower arms alone/with upper producer 31/31; both with single-exit,
// positive-default and split/conditional results 30/790/646. NOT kept:
// one-point lead or regression, no byte match; original helper and body restored.
// DH358: Existing quantizer double transport with explicit real-rounded callers and a multi-use
// real sample, long/int/short/word results with mutable/immutable input: 660/660/1001/1001 twice.
// NOT kept: all regress and lengthen to 4836/4856; original helper and whole body restored.
// F391: Existing quantizer policy as bool/int template argument, mutable/immutable real
// input and conditional/separate lower admissions: 1218 each in eight forms, 4404/4828 bytes.
// NOT kept: specialization outlines both quantizers and moves extent away from retail.
// Original helper, runtime policy formal, eight calls, repeated conversions and whole body restored;
// selector scheduling, post-bias reload pairs and six shift-versus-clip input homes remain.
// Round 214 (m3 lane 3, fast; masked aligned in brackets, base 9), nothing kept: the span ratio named after the mode-zero texture block 357 [17] at 4832;
// the texture block's skip test merged as `extra_texture && !skip_state` 31 [9]; an empty compiled-out else on the skip test, mode zero or both 31/31;
// gain from a separate 1.0f start 31; the span ratio named ahead of the gain 63 [16]; a named span copy 31. Selector forms: input first 939 [6] (one
// instruction short, no `mov al, cl`), with a named copy of small the same; whole, input, fraction, alpha 976 [8] at 4840; `4 + small` 952 [11];
// input ahead of `fixed` 974 [10]. All 24 orders of whole, fraction, alpha and input on this body: wari 30 (one point, picked by score), wrai and awri 31,
// iwar and iawr 35, the rest 243..976. `mov cl, [small]` first needs alpha or input first, and then the whole/fraction moves come after both selectors.
// Round 132 (m3 lane 7): the quantizer helper's by-value parameter is a symbol per expansion and
// takes shift's dead home in modes one to three; as an expression macro (PIN-style, the input a CSE
// temp, no local) every input takes clip's home as retail: 31 -> 19 plain, kept. On it: span/byte pair
// through one inline helper at all four modes 31 (before the macro, same code); inverse span declared
// at the top 31; exponent guard `1.0f < inverse_span` 31; small declared first, after allow_negative,
// after maximum 19/281/282, each with input before alpha 939/995/996; low and high divided by the
// distance 19 (same code); the inverse span renamed 19 (same code).
// Round 134 (m3 lane 7, fast; kept step plain over all 57 functions of the file): input before alpha alone 939 (aligned
// 7); with the shifted input named after the first constant store and used by both stores 9 (selector block retail's;
// left: the shl's place and two reload pairs); named before the clear 928; at each of the 7 positions after the clear 9;
// copy then <<=, long type, closed scope 9; arms' names swapped 11; the name in one store only 939. Split as whole- or
// fraction-first 226/225; (byte)fixed 937; % 256 960; fraction before whole 767; (small ? 1 : 0) + 4 939; if-assigned
// input 934; fixed through 0x2b460 939/19. The reload pairs follow the end of the skip-state arm (a real if with a
// body last in that arm flips that pair; empty else, do/while(false), inverted arm 9).
// Round 93 (m3 lane 16, fast): the two input words through the SDK's PS_COMBINERINPUTS(input, ...) with no shifted
// local 939 (first difference +0x5b), plus the alpha word the same way 939; the shifted local const 9; `input` as
// dword 9; `alpha_input` as dword 9. Retail shifts a copy of input in EAX after the clear; ours shifts EBX in place
// and the scheduler lifts that shift above the clear.
// Round 59 (m3 lane 2, fast): retail reloads the pair after modes zero and one's ZBIAS write in descending register
// order (xmm5, xmm3 span, xmm2 1.0f). No shifted local with `((dword)input << 24)` at both stores 939 (aligned 15);
// dword input with the shift at both stores 939; the shifted word from the selector itself, no input local 976 (4840
// bytes); mode one's colours declared separately 9; mode zero's extra-texture test at its two uses 905 (4816 bytes);
// the whole/fraction clamp as two statements, whole first 9 (same code), fraction first 11.
// Round 83 (m3 lane 10, fast; plain over the file agrees): the exponent search as a plain `while` loop in place of the
// hand-inverted `if` + do/while 9 (same code, kept); the two floor results divided by 65536 9 (same code); the blending
// states and the zero depth bias through one forced-inline pass-state helper at all four sites 9 (same code); the offset
// clamp through FILTER_PIN 1072; both selectors from one if/else on small 952. The reloads after the depth-bias call stay
// constant first: in 0x2548f0's file, moving that function above 0x254490 (whose reload order is swapped the other way)
// changed neither, so the order does not follow source order within the file.
// Round 171 (m3 lane 6, fast; all restored at 9): 1-8 initialised unused locals at the top, 1-5 before the
// inverse span or before the input word (diagnostic) 9 each, so neither the reload pairs nor the shift follow
// symbol numbering; the input word without the cast 9, as a long 9, from a dword input 9, `(dword)(input << 24)` 9;
// no shifted local 939 (aligned 15: the shift is then retail's, `mov eax, ebx; shl eax, 0x18` after the clear, but
// small is reloaded into AL after fraction's move to EBP) and on that base the other 23 whole/fraction/input/alpha
// declaration orders, best wafi 18 (alpha computed in EBX), wfai 19, awfi 19, the rest 23..976; mode zero's
// depth-bias write as the matched siblings write it (global store, then D3DDevice_SetRenderState) 9, in the
// skip-state arm 9, both 9.
// Permuter (600 s, 46 tries on the reduced image): best 9.
// Round 96 (m3 lane 10; fast, and a compile-only /FAs harness of the file, which reproduces the reload pairs but
// not the shift): one function-scope gain and one function-scope upper/lower pair shared by the modes 9; the two
// colours declared at function scope 9; exponent declared at function scope 9; the inverse span declared at function
// scope 9 (round 132's form, re-measured); `gain = gain / (...)` 9; `exponent <= 3 ? 0 : 0x20` 9; `1L << exponent` 9;
// `exponent < 1 ? 1.0f : 2.0f` 178. Harness only (both pairs still constant first): the shift selector macro instead
// of the force-inline helper, `offset * gain`, `256.0f * inverse_span`, `0.5f * inverse_span`, `(real)1 /`, the
// selected gain alone shared. FILTER_PIN as a
// force-inline function flips both pairs to span first, but only because mode one's colour clamps then read each
// field once (other code); on the two gain clamps alone the pairs stay constant first in other registers.
// Round 176 (m3 lane 6, fast; all restored at 9): on the no-shifted-local form (939: shift retail's, but small loaded once
// into AL after fraction's move where retail loads CL first and copies it) `input` renamed 939 (names do not move it); the
// two selectors ahead of `fixed` 31 (computed before the floor call); mode one's colour inputs through the matched sibling
// 0x25e50's colour/scale references 11 (first.red's product operands swap; shift and pairs unchanged); the shifted word
// declared before the clear and assigned after it 9; the alpha word through its own shifted local too 9; all four shifted
// words (fraction, input, whole, alpha) named in parallel ahead of the first store 9.
// @retail 0x3a5c0
bool __stdcall function_3a5c0(byte const *configuration, real low, real high, long mode,
    bool center, bool shift, bool clip, bool skip_state, real *depth_out)
{
    bool result = false;
    real offset = 0.0f;
    bool small = false;
    byte allow_small = false;
    bool allow_negative = false;
    real maximum = 1.0f;
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
    if (!(high > low)) goto failure;
    real inverse_span = 1.0f / (high - low);
    if (inverse_span > maximum)
    {
        real width = 1.0f / maximum;
        inverse_span = maximum;
        if (small) goto failure;
        if (center) low = (low + high) * 0.5f - width * 0.5f;
        else if (shift) low = high - width;
        else if (!clip) goto failure;
    }
    if (low < 0.0f)
    {
        if (allow_negative)
        {
            real unclamped_offset = 0.0f - inverse_span * low;
            if (unclamped_offset < 0.0f) offset = 0.0f;
            else
            {
                offset = 1.0f;
                if (!(unclamped_offset > 1.0f)) offset = unclamped_offset;
            }
        }
        else if (!shift || !clip) goto failure;
        low = 0.0f;
    }
    if (!skip_state)
    {
        function_14bc0((short)g_4858b8, 0, true);
        function_14f60(0, 3);
        sampler_state_write(0, D3DTSS_ADDRESSU, 3);
        sampler_state_write(0, D3DTSS_ADDRESSV, 3);
        sampler_state_write(0, D3DTSS_ADDRESSW, 3);
        sampler_state_write(0, D3DTSS_MAGFILTER, 1);
        sampler_state_write(0, D3DTSS_MINFILTER, 1);
        sampler_state_write(0, D3DTSS_MIPFILTER, 0);
        sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(0, D3DTSS_COLORSIGN, 0);
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        function_0222d0(D3DRS_COLORWRITEENABLE, 0x1010101);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        function_0222d0(D3DRS_CULLMODE, 0);
        function_0222d0(D3DRS_STENCILENABLE, 0);
        function_0222d0(D3DRS_ZENABLE, 0);
        function_0222d0(D3DRS_ZWRITEENABLE, 0);
#pragma inline_depth(0)
        function_0222d0(D3DRS_ZFUNC, 0x207);
#pragma inline_depth(255)
        function_0222d0(D3DRS_ZBIAS, 0);
        function_0222d0(D3DRS_DEPTHCLIPCONTROL, 0x10);
        function_1cc30(12);
        function_1c710(g_51f0f0);
    }
    long fixed = (long)floor(low * 65536.0f);
    long whole = fixed >> 8;
    long fraction = fixed & 0xff;
    long input = small ? 5 : 4;
    long alpha_input = small ? 4 : 24;
    if (whole > 255) fraction = whole = 255;
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11108;
    g_484f68.PSConstant0[0] = 0xff0000;
    g_484f68.PSConstant1[0] = 0xff00;
    g_484f68.PSRGBInputs[0] = 0x8010802;
    g_484f68.PSRGBOutputs[0] = 0x3045;
    g_484f68.PSConstant0[1] = (dword)fraction << 24;
    dword input_bits = (dword)input << 24;
    g_484f68.PSRGBInputs[1] = input_bits | 0x201140;
    g_484f68.PSRGBOutputs[1] = 0xc00;
    g_484f68.PSAlphaInputs[1] = input_bits | 0x209140;
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
    while (inverse_span > (real)(1 << exponent))
        exponent++;
    if (mode == 0)
    {
        bool extra_texture = configuration == (byte const *)NONE;
        if (extra_texture)
        {
            if (!skip_state)
            {
                set_filter_blending(true);
                function_0222d0(D3DRS_ZBIAS, 0);
            }
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
        g_484f68.PSConstant1[6] = function_131fc0(FILTER_PIN(gain * offset));
        g_484f68.PSConstant0[6] = 0x7f000000;
        g_484f68.PSRGBInputs[6] = 0x1d201220;
        g_484f68.PSRGBOutputs[6] = filter_output_shift(exponent, 7);
        g_484f68.PSAlphaInputs[6] = 0x18201120;
        g_484f68.PSAlphaOutputs[6] = 0xc00;
        long upper = FILTER_QUANTIZED_BYTE(inverse_span * 256.0f / (real)(1 << exponent), true);
        long lower = FILTER_QUANTIZED_BYTE(inverse_span * 0.5f, true);
        g_484f68.PSConstant1[7] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[7] = PS_COMBINERINPUTS(0, 0, small ? 0x20 : 0, 0x20);
        g_484f68.PSRGBOutputs[7] = 0x4500;
        g_484f68.PSAlphaInputs[7] = 0xd120c02;
        g_484f68.PSAlphaOutputs[7] = filter_output_first_shift(exponent);
        g_484f68.PSFinalCombinerConstant0 = function_131fc0(offset);
        g_484f68.PSFinalCombinerInputsABCD = 0x41d1105;
        g_484f68.PSFinalCombinerInputsEFG = (extra_texture ? 25 : 32) << 8;
    }
    else if (mode == 1)
    {
        color4f first, second;
        first.alpha = FILTER_PIN(*(real const *)(configuration + 0x28));
        first.red = FILTER_PIN(*(real const *)(configuration + 0x1c) * *(real const *)(configuration + 0x28));
        first.green = FILTER_PIN(*(real const *)(configuration + 0x20) * *(real const *)(configuration + 0x28));
        first.blue = FILTER_PIN(*(real const *)(configuration + 0x24) * *(real const *)(configuration + 0x28));
        second.alpha = FILTER_PIN(*(real const *)(configuration + 0x68));
        second.red = FILTER_PIN(*(real const *)(configuration + 0x5c));
        second.green = FILTER_PIN(*(real const *)(configuration + 0x60));
        second.blue = FILTER_PIN(*(real const *)(configuration + 0x64));
        if (!skip_state)
        {
            set_filter_blending(false);
            function_0222d0(D3DRS_ZBIAS, 0);
        }
        g_484f68.PSAlphaInputs[0] = 0x8200000;
        g_484f68.PSAlphaOutputs[0] = 0x40;
        g_484f68.PSAlphaInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[4] = filter_output_shift(exponent, 1);
        real gain = 1.0f / (exponent > 5 ? 4.0f : exponent > 4 ? 2.0f : 1.0f);
        gain /= exponent > 0 ? 2.0f : 1.0f;
        gain /= inverse_span / (real)(1 << exponent);
        g_484f68.PSConstant1[5] = function_131fc0(FILTER_PIN(gain * offset));
        g_484f68.PSAlphaInputs[5] = 0x1d201220;
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        g_484f68.PSConstant0[6] = pack_color4f(&first);
        long upper = FILTER_QUANTIZED_BYTE(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = FILTER_QUANTIZED_BYTE(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[6] = 0x1341134;
        g_484f68.PSRGBOutputs[6] = 0x45;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_first_shift(exponent);
        g_484f68.PSConstant0[7] = pack_color4f(&second);
        g_484f68.PSAlphaInputs[7] = 0x11141d05;
        g_484f68.PSAlphaOutputs[7] = 0xc00;
        g_484f68.PSFinalCombinerConstant0 = pack_color4f(&second);
        g_484f68.PSFinalCombinerInputsABCD = 0x114000f;
        g_484f68.PSFinalCombinerInputsEFG = 0x1d041c00;
    }
    else if (mode == 2)
    {
        if (!skip_state)
        {
            set_filter_blending(false);
            function_0222d0(D3DRS_ZBIAS, 0);
        }
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
        long upper = FILTER_QUANTIZED_BYTE(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = FILTER_QUANTIZED_BYTE(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_first_shift(exponent);
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
            g_484f68.PSFinalCombinerConstant0 = pack_color3f((color3f const *)(configuration + 8));
            g_484f68.PSFinalCombinerInputsABCD = 0x1d010000;
            g_484f68.PSFinalCombinerInputsEFG = 0x1d00;
        }
    }
    else if (mode == 3)
    {
        if (!skip_state)
        {
            set_filter_blending(false);
            function_0222d0(D3DRS_ZBIAS, 0);
        }
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSConstant0[4] = pack_color3f((color3f const *)(configuration + 4));
        g_484f68.PSConstant1[4] = pack_color3f((color3f const *)(configuration + 0x10));
        g_484f68.PSRGBInputs[4] = PS_COMBINERINPUTS(configuration[0] ? 0x19 : 0x09, 0x01, configuration[0] ? 0x1a : 0x0a, 0x02);
        g_484f68.PSRGBOutputs[4] = 0x3045;
        g_484f68.PSAlphaInputs[4] = 0x1d201d00 | (exponent > 3 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[4] = filter_output_shift(exponent, 1);
        g_484f68.PSConstant0[5] = pack_color3f((color3f const *)(configuration + 0x1c));
        g_484f68.PSRGBInputs[5] = PS_COMBINERINPUTS(0x24, 0x25, configuration[0] ? 0x1b : 0x0b, 0x01);
        g_484f68.PSRGBOutputs[5] = 0x1045;
        g_484f68.PSAlphaInputs[5] = 0x1d201d00 | (exponent > 6 ? 0x20 : 0);
        g_484f68.PSAlphaOutputs[5] = filter_output_shift(exponent, 4);
        long upper = FILTER_QUANTIZED_BYTE(inverse_span * 256.0f / (real)(1 << exponent), false);
        long lower = FILTER_QUANTIZED_BYTE(inverse_span * 0.5f, false);
        g_484f68.PSConstant1[6] = ((dword)upper << 24) | lower;
        g_484f68.PSRGBInputs[6] = 0x4250000;
        g_484f68.PSRGBOutputs[6] = 0xd0;
        g_484f68.PSAlphaInputs[6] = 0x1d120c02;
        g_484f68.PSAlphaOutputs[6] = filter_output_first_shift(exponent);
        g_484f68.PSAlphaInputs[7] = 0x1d2d0000;
        g_484f68.PSAlphaOutputs[7] = 0xd0;
        g_484f68.PSFinalCombinerConstant0 = pack_color3f((color3f const *)(configuration + 0x28));
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
    result = true;
failure:
    return result;
}

dword g_4b8424;
void function_27aa0();
// @retail 0x25a10
void function_25a10(byte const *state, point3f const *position, vector3f const *direction)
{
    real constants[24];
    real const *settings = (real const *)state;
    real dot = dot3f((vector3f const *)position, direction);
    real first_span = settings[12] - settings[11];
    real first = 1.0f / (0.0001f > first_span ? 0.0001f : first_span);
    real second_span = settings[18] - settings[17];
    real second = 1.0f / (0.0001f > second_span ? 0.0001f : second_span);
    real third = 1.0f / (0.0001f > settings[44] ? 0.0001f : settings[44]);
    real fourth = 1.0f / (0.0001f > settings[45] ? 0.0001f : settings[45]);
    constants[0] = direction->i * first;
    constants[1] = direction->j * first;
    constants[2] = direction->k * first;
    constants[3] = 0.0f - (settings[11] + dot) * first;
    constants[4] = 0.0f - settings[61] * fourth;
    constants[5] = 0.0f - settings[62] * fourth;
    constants[6] = 0.0f - settings[63] * fourth;
    constants[7] = settings[64] * fourth;
    constants[8] = direction->i * third;
    constants[9] = direction->j * third;
    constants[10] = direction->k * third;
    constants[11] = 0.0f - (settings[68] + dot) * third;
    real fade = 0.0f - settings[66] * fourth;
    constants[12] = fade < 0.0f ? 0.0f : (fade > 1.0f ? 1.0f : fade);
    constants[13] = 0.0f;
    constants[14] = settings[10];
    constants[15] = settings[43];
    constants[16] = direction->i * second;
    constants[17] = direction->j * second;
    constants[18] = direction->k * second;
    constants[19] = 0.0f - (dot + settings[17]) * second;
    constants[20] = settings[16];
    constants[21] = 0.0f;
    constants[22] = 0.0f;
    constants[23] = 0.0f;
    D3DDevice_SetVertexShaderConstant(-84, constants, 6);
    g_4b8424 = pack_color3f((color3f const *)(state + 0x1c));
    D3DDevice_SetRenderState(D3DRS_FOGCOLOR, g_4b8424);
    function_27aa0();
}

extern dword g_4b8308, g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324, g_4b82ec, g_4b8448, g_4b843c, g_4b8438, g_4b8450;
dword __cdecl pack_color4f(color4f const *color);


// @retail 0x25e50
bool __stdcall function_25e50(void *context)
{
    if (g_48578c == 0.0f)
    {
        if (function_3a5c0((byte const *)&g_48574c, g_485778, g_48577c, 1,
                false, false, false, false, &g_509400))
            return true;
    }
    g_509400 = g_485ae0;
    function_14bc0((short)g_4858b8, 0, true);
    function_14f60(0, 3);
    sampler_state_write(0, D3DTSS_ADDRESSU, 3);
    sampler_state_write(0, D3DTSS_ADDRESSV, 3);
    sampler_state_write(0, D3DTSS_ADDRESSW, 3);
    sampler_state_write(0, D3DTSS_MAGFILTER, 1);
    sampler_state_write(0, D3DTSS_MINFILTER, 1);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x14), 0, 2, 0.0f);
    sampler_state_write(2, D3DTSS_ADDRESSU, 3);
    sampler_state_write(2, D3DTSS_ADDRESSV, 3);
    sampler_state_write(2, D3DTSS_ADDRESSW, 3);
    sampler_state_write(2, D3DTSS_MAGFILTER, 2);
    sampler_state_write(2, D3DTSS_MINFILTER, 2);
    sampler_state_write(2, D3DTSS_MIPFILTER, 0);
    sampler_state_write(2, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(2, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(2, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(2, D3DTSS_COLORSIGN, 0);
    sampler_state_write(2, D3DTSS_ALPHAKILL, 0);
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
#pragma inline_depth(0)
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, g_48575d ? 0x207 : 0x204);
#pragma inline_depth(255)
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    byte const *settings = (byte const *)&g_48574c;
    vector3f const &first_color = *(vector3f const *)(settings + 0x1c);
    real const &second_scale = *(real const *)(settings + 0x40);
    real const &first_scale = *(real const *)(settings + 0x28);
    real const &mix = *(real const *)(settings + 0x11c);
    vector3f const &second_color = *(vector3f const *)(settings + 0x34);
    color3f const &third_color = *(color3f const *)(settings + 0x5c);
    color4f colors[3];
    colors[0].alpha = FILTER_PIN((1.0f - mix) * second_scale);
    colors[0].red = FILTER_PIN(first_color.i * first_scale);
    colors[0].green = FILTER_PIN(first_color.j * first_scale);
    colors[0].blue = FILTER_PIN(first_color.k * first_scale);
    colors[1].alpha = FILTER_PIN(first_scale * mix);
    colors[1].red = FILTER_PIN(second_color.i * second_scale);
    colors[1].green = FILTER_PIN(second_color.j * second_scale);
    colors[1].blue = FILTER_PIN(second_color.k * second_scale);
    real first_alpha = FILTER_PIN(first_scale);
    real second_alpha = FILTER_PIN(second_scale);
    colors[2].alpha = FILTER_PIN(*(real const *)(settings + 0x68));
    colors[2].red = FILTER_PIN(third_color.red);
    colors[2].green = FILTER_PIN(third_color.green);
    colors[2].blue = FILTER_PIN(third_color.blue);
    memset(&g_484f68, 0, sizeof(g_484f68));
    dword *fields = (dword *)&g_484f68;
    fields[0xd8 / 4] = 0x2621;
    fields[0xdc / 4] = 0x44;
    fields[0xd4 / 4] = 0x11004;
    fields[0x28 / 4] = pack_color4f(&colors[0]);
    fields[0x48 / 4] = pack_color4f(&colors[1]);
    fields[0x88 / 4] = 0x10a021a;
    fields[0xb4 / 4] = 0xcd;
    fields[0x0 / 4] = 0x111a120a;
    fields[0x68 / 4] = 0xcd;
    fields[0x2c / 4] = function_131fc0(first_alpha);
    fields[0x4c / 4] = function_131fc0(second_alpha);
    fields[0x6c / 4] = 0xcd;
    fields[0x70 / 4] = 0xcd;
    fields[0x8c / 4] = 0xc3c0d3d;
    fields[0xb8 / 4] = 0xc00;
    fields[0x4 / 4] = 0x110a121a;
    fields[0x8 / 4] = 0x3c3d0820;
    fields[0x34 / 4] = pack_color4f(&colors[2]);
    fields[0x94 / 4] = 0x3d0c1d01;
    fields[0xc0 / 4] = 0xc00;
    fields[0xc / 4] = 0x3d3c1d11;
    fields[0x74 / 4] = 0xc00;
    fields[0x20 / 4] = 0xc;
    fields[0x24 / 4] = 0x1c00;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

extern dword g_4b8308, g_4b82e8, g_4b82f4, g_4b8448, g_4b843c, g_4b8438, g_4b8450;
dword __cdecl pack_color4f(color4f const *color);

// @retail 0x26e50
bool __stdcall function_26e50(void *context)
{
    bool separate = !g_5093fe;
    if (!g_4b9d9c)
    {
        byte configuration[0x18];
        memset(configuration, 0, sizeof(configuration));
        *(real *)(configuration + 0) = g_4857d4;
        *(real *)(configuration + 4) = g_4857d8;
        *(color3f *)(configuration + 8) = g_4857bc;
        configuration[0x14] = separate;
        bool filtered = function_3a5c0(configuration, g_4857dc, g_4857e0, 2,
            false, false, true, false, &g_509400);
        if (filtered)
        {
            function_14f60(1, 19);
    sampler_state_write(1, D3DTSS_ADDRESSU, 3);
    sampler_state_write(1, D3DTSS_ADDRESSV, 3);
    sampler_state_write(1, D3DTSS_ADDRESSW, 3);
    sampler_state_write(1, D3DTSS_MAGFILTER, 2);
    sampler_state_write(1, D3DTSS_MINFILTER, 2);
    sampler_state_write(1, D3DTSS_MIPFILTER, 0);
    sampler_state_write(1, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(1, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(1, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(1, D3DTSS_COLORSIGN, 0);
    sampler_state_write(1, D3DTSS_ALPHAKILL, 0);
            return true;
        }
    }
    g_509400 = g_485ae0;
    function_14bc0((short)(separate ? 3 : g_4858b8), 0, false);
    function_14f60(0, 3);
    sampler_state_write(0, D3DTSS_ADDRESSU, 3);
    sampler_state_write(0, D3DTSS_ADDRESSV, 3);
    sampler_state_write(0, D3DTSS_ADDRESSW, 3);
    sampler_state_write(0, D3DTSS_MAGFILTER, 1);
    sampler_state_write(0, D3DTSS_MINFILTER, 1);
    sampler_state_write(0, D3DTSS_MIPFILTER, 0);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x14), 0, 2, 0.0f);
    sampler_state_write(2, D3DTSS_ADDRESSU, 3);
    sampler_state_write(2, D3DTSS_ADDRESSV, 3);
    sampler_state_write(2, D3DTSS_ADDRESSW, 3);
    sampler_state_write(2, D3DTSS_MAGFILTER, 2);
    sampler_state_write(2, D3DTSS_MINFILTER, 2);
    sampler_state_write(2, D3DTSS_MIPFILTER, 0);
    sampler_state_write(2, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(2, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(2, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(2, D3DTSS_COLORSIGN, 0);
    sampler_state_write(2, D3DTSS_ALPHAKILL, 0);
    function_14f60(3, 19);
    sampler_state_write(3, D3DTSS_ADDRESSU, 3);
    sampler_state_write(3, D3DTSS_ADDRESSV, 3);
    sampler_state_write(3, D3DTSS_ADDRESSW, 3);
    sampler_state_write(3, D3DTSS_MAGFILTER, 2);
    sampler_state_write(3, D3DTSS_MINFILTER, 2);
    sampler_state_write(3, D3DTSS_MIPFILTER, 0);
    sampler_state_write(3, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(3, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(3, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(3, D3DTSS_COLORSIGN, 0);
    sampler_state_write(3, D3DTSS_ALPHAKILL, 0);
    g_4b8308 = 0x1010101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    g_4b82f4 = 0x8001; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 0x8001);
#pragma inline_depth(0)
    function_0222d0(D3DRS_DESTBLEND, 0x303);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
#pragma inline_depth(255)
    g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    g_4b8438 = 0;
#pragma inline_depth(0)
    D3DDevice_SetRenderState_ZEnable(0);
#pragma inline_depth(255)
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    color4f first, second, third;
    first.alpha = g_4b9c74[1][5] < 0.0f ? 0.0f : (g_4b9c74[1][5] > 1.0f ? 1.0f : g_4b9c74[1][5]);
    first.red = g_4b9c74[2][5] < 0.0f ? 0.0f : (g_4b9c74[2][5] > 1.0f ? 1.0f : g_4b9c74[2][5]);
    first.green = g_4b9c74[3][5] < 0.0f ? 0.0f : (g_4b9c74[3][5] > 1.0f ? 1.0f : g_4b9c74[3][5]);
    first.blue = g_4b9c74[4][5] < 0.0f ? 0.0f : (g_4b9c74[4][5] > 1.0f ? 1.0f : g_4b9c74[4][5]);
    second.alpha = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    second.red = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    second.green = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    second.blue = g_4857d4 < 0.0f ? 0.0f : (g_4857d4 > 1.0f ? 1.0f : g_4857d4);
    third.alpha = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    third.red = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    third.green = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    third.blue = g_4857d8 < 0.0f ? 0.0f : (g_4857d8 > 1.0f ? 1.0f : g_4857d8);
    memset(&g_484f68, 0, sizeof(g_484f68));
    dword *fields = (dword *)&g_484f68;
    fields[0xd8 / 4] = 0xa621;
    fields[0xdc / 4] = 0x44;
    fields[0xd4 / 4] = 0x11006;
    fields[0x28 / 4] = pack_color4f(&first);
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
    fields[0x38 / 4] = pack_color4f(&second);
    fields[0x58 / 4] = pack_color4f(&third);
    fields[0x10 / 4] = 0x14150000;
    fields[0x78 / 4] = 0xc0;
    fields[0x98 / 4] = 0xa010a02;
    fields[0xc4 / 4] = 0xcd;
    fields[0x9c / 4] = 0x1c0c3c0d;
    fields[0xc8 / 4] = 0xc00;
    if (separate)
    {
#pragma inline_depth(0)
        function_0222d0(D3DRS_COLORWRITEENABLE, 1);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
#pragma inline_depth(255)
        fields[0x20 / 4] = 0xc;
        fields[0x24 / 4] = 0;
    }
    else
    {
        fields[0xac / 4] = pack_color3f(&g_4857bc);
        fields[0x20 / 4] = 0xc010000;
        fields[0x24 / 4] = 0xc00;
    }
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}


// Construct a saved camera frame, then select and run its render passes.
extern bool g_4ba010;
extern long g_4ba038, g_46701c, g_4e9194, g_4e64a8, g_4e92e0;
void __stdcall function_132d0(s_frame_view_2c560 const *frame);
void function_13c20();
void function_13680(bool skip_effects);
void function_15370(short mode);
void function_15680(short mode);
void function_1beb0();
void function_3d2c0();
void function_3ea60();
void function_c15a0();
void function_156040();
real __stdcall function_156b30(long track, long type);
void function_3fd70();
void function_2b990(long object);
void function_40890();
void function_1917eb();
class c_render_window;
void function_147af1(long first, long second, c_render_window *window);
struct s_2f800_view;
void function_2fd90(s_2f800_view const *view, box2f const *clip, byte *out);

// MATCH (was 8): the frame counter and the flag live in storage whose address retail takes, so
// their stores keep their place among the other render-global stores and block copies; they are
// written through their own addresses here for that (the full check loses nothing).
// @retail 0x2ba10
void function_2ba10(s_2f970_view const *view, long cluster, long player,
    byte const *camera, bool enabled, long leaf, bool invalid, vector3f const *sky_color,
    long unknown0, long object, long object_index, byte kind, long sky_index,
    byte const *geometry, bool unknown1, long unknown2, byte const *lighting,
    long unknown3, s_speed_result const *speed)
{
    if (!camera) camera = (byte const *)view;
    g_4b9ed4 = player;
    g_4b9ee4 = cluster;
    g_4b9ee0 = leaf;
    g_4b9ee8 = invalid;
    g_4b9ed8 = object;
    g_4b9edc = object_index;
    ++*(dword *)&g_4ba038; /* address-taken in retail: see above */
    g_4b9ee9 = kind;
    g_4b9eec = sky_index;
    memcpy(&g_4b9ef0, geometry, 0x120);
    *(byte *)&g_4ba010 = 0; /* address-taken in retail: see above */
    memcpy(&g_4b9da0, view, 0x74);
    function_2fd90((s_2f800_view const *)&g_4b9da0, (box2f const *)sky_color, (byte *)&g_4b9e14);
    function_3d2c0();
    function_3ea60();
    function_c15a0();
    s_frame_view_2c560 frame;
    memset(&frame, 0, sizeof(frame));
    memcpy(frame.view, camera, sizeof(frame.view));
    function_2fd90((s_2f800_view const *)frame.view, (box2f const *)sky_color, frame.projection);
    frame.mode = (short)unknown0;
    frame.flag06 = unknown1;
    frame.index = (short)player;
    frame.unknown04 = (word)unknown2;
    frame.position = *(point3f const *)(geometry + 4);
    frame.scaled = enabled;
    frame.location = 2;
    memcpy(frame.records, g_view_context_storage.frame_records, sizeof(frame.records));
    frame.unknown26c[0] = 1;
    memcpy(frame.unknown26c + 4, speed, 24);
    *(byte const **)(frame.unknown26c + 0x24) = lighting;
    frame.unknown26c[0x28] = (byte)unknown3;
    long saved_index = ++g_46701c;
    g_4850d0[saved_index] = frame;
    function_132d0(&g_4850d0[saved_index]);
    function_1beb0();
    if (g_4e9194)
    {
        function_156040();
        function_13c20();
        return;
    }
    g_4e64a8 = 0;
    g_4e92e0 = 0;
    function_2c3b0((s_interface_function_context *)&g_4e9268);
    g_4e926c = (void *)function_156b30;
    function_3fd70();
    function_2b990(object);
    function_13680(enabled);
    function_15370(0);
    function_15680(0);
    if (g_4b9ed8 != NONE) function_1917eb();
    function_40890();
    function_147af1(NONE, object_index, (c_render_window *)(camera + 0x30));
    function_13c20();
}
