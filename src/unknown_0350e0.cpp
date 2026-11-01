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



matrix3x3 g_467104 = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

PRIVATE __forceinline void rotation_from_axis(vector3f const *axis, real angle, matrix3x3 *matrix)
{
    real sine = (real)sin(angle);
    real cosine = (real)cos(angle);
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

// score 356 (was 356): second pass walked retail 0x39e50-0x3a540 formula by formula and found no behaviour
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
// @retail 0x39e50
void __stdcall function_39e50(real step)
{
    matrix3x3 first, second;
    rotation_from_axis(&g_485618.forward, step * 0.5f, &first);
    rotation_from_axis(&g_485618.up, step * 0.3f, &second);
    vector3f forward = g_467104.forward;
    vector3f left = g_467104.left;
    vector3f up = g_467104.up;
    forward = rotate_axis_vector(&first, &forward);
    left = rotate_axis_vector(&first, &left);
    up = rotate_axis_vector(&first, &up);
    g_467104.forward = forward;
    g_467104.left = left;
    g_467104.up = up;
    forward = rotate_axis_vector(&second, &forward);
    left = rotate_axis_vector(&second, &left);
    up = rotate_axis_vector(&second, &up);
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
byte g_4ba020, g_4ba021, g_4ba022, g_4ba023, g_4ba024, g_4ba025;
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

// score 17 (was 17): the three calls are set up differently: retail passes the globals
// in ECX and the word in EAX where ours uses EDX, and the last call's argument order
// differs (the callees' conventions are fixed in their own files).
// Retail also sets function_14bc0's bool (AL) between its two pushes, which suggests a different
// declared parameter order (index, use_depth, element); that callee has 89 call sites, not tried.
// Tried: swapping function_4c690's parameters (no change), locals for the index/tag read before
// the first call (40), dropping the (short) cast or using word (17), testing function_3ebd0's result (24).
// Round 104: callee entries confirm the current signatures; retail's caller temporaries are offset EDX,
// output ECX and target EAX against our ECX/EAX/EDX. Matrix pointer at the calls/function scope 17/17;
// direct element-zero address at both/first/last calls 17/17/17; const count 17; late tag/target snapshots
// 17/17; offset snapshot 17. Original restored.
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
		long count = *(long *)(definition + 0x48);
		if (count != g_4c5038.count)
		{
			g_4c5038.count = count;
			g_4c5038.valid = false;
			return;
		}
		if (g_4c5038.valid)
		{
			function_3ebd0(g_4687a4, g_4c5038.matrices, matrices, count);
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

extern byte *g_4858c4;
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

// score 26 (was 26): retail reloads g_4858c4 after function_1bd50 (no saved register, no
// push/pop esi); ours keeps it in ESI across the call. Tried: a local for the first read,
// a local for the tag (both 26). Also tried (all 26): an early return, the test in a bool local,
// a pointer to the +0xe0 field for the tests, `NULL`/`true` spellings of the arguments, a local for
// the second tag read after the call. Retail never takes g_4858c4's address, so its reload means
// retail's 0x1bd50 counts as writing it (the same symbol as a global 0x1bd50 stores, a structure?).
// Round 186: a store to g_4858c4 added to 0x1bd50 as a diagnostic gives retail's reload exactly, so the
// cause is outside this body: g_4858c4 and g_485a58 (which 0x1bd50 writes) look like members of one
// retail storage struct, BLOCKED-BY g_4858c4's definition (src/unknown_030290.cpp). Tried (all 26 unless
// noted): `_ReadWriteBarrier()` after the call; the signature test nested in its own `if`; the fields
// read by index (`((long *)g_4858c4)[0x38]`); `&& tag` for `!= 0`; the flag stored after the call 31.
// Round 109 (all 26): ours already reloads `[esi+0xe0]` after 0x1bd50, only the g_4858c4 symbol stays cached;
// the signature tested first; the test as `!(... == NONE || ... == 0 || ... != 'shad')`; a shader local assigned
// again from the global after the call; an early return on a tag local; an inline predicate on the shader.
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
bool function_2dba0(long tag, vector3f const *direction, bool alternate, long c, long d, long e, point3f const *position, color3f const *color, real alpha, real amount, real scale)
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

// score 12 (was 12): the parent lookup differs. Retail masks the parent index (`and eax, 0xffff`,
// `lea`) before it loads g_4e0300, ours loads g_4e0300 first and uses movzx; the first model
// load also goes through EDX in ours and EAX in retail. Tried: `long parent_slot = parent & 0xffff`
// (22), the same with byte-offset addressing (22). Also tried: `[parent & 0xffff]` written directly (22,
// 459 bytes: retail loads g_4e0300 into EAX after the mask, ours into ECX before it), the parent object
// and definition in one expression (22), `(word)parent` inline (12), the model read without the
// definition local (12), `model` reused for the parent's model (12), `object_index != parent` (13),
// `parent` declared before the model test (12), definition/model declared at the top (12).
// Round 109: the first chain's definition also lands in EDX (retail EAX, reusing the dying register as the model
// load does). Tried: the parent object as `long *` (as matched 0x17bab0 writes it) with `[parent & 0xffff]` 22,
// a parent header pointer 22, `long parent_index = parent & 0xffff` 22, `if (model == NONE) return;` 12, a two-caller
// inline model-index helper for the object and its parent 22.
// Round 111: initial definition as `long *` 12; parent definition as `long *` 12; parent definition inlined 12;
// parent slot as dword/long from the narrowed handle 12/12; a shared tag-table local 12; settings as a real
// array 12; the read-only object/definition chain const 12; the parent slot masked again in its array subscript 12.
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
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long model = *(long *)(definition + 0x38);
    if (model != NONE)
    {
        byte *settings = g_4e3b44[model & 0xffff].bytes + 0x28;
        long parent = function_baf80(object_index);
        if (parent != object_index)
        {
            word parent_slot = (word)parent;
            byte *local_be682a_2 = ((s_scalar_object_header *)g_4e0300->data)[parent_slot].object;
            byte *parent_definition = g_4e3b44[*(long *)local_be682a_2 & 0xffff].bytes;
            long parent_model = *(long *)(parent_definition + 0x38);
            if (parent_model != NONE)
                settings = g_4e3b44[parent_model & 0xffff].bytes + 0x28;
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
// @retail 0x34100
bool __stdcall function_34100(long mode, real const *rectangle, real const *coordinates,
    real *out, real const *parameters)
{
    real center_x = (rectangle[0] + rectangle[1]) * 0.5f;
    real center_y = (rectangle[2] + rectangle[3]) * 0.5f;
    long index = mode - 1;
    switch (mode)
    {
    case 0:
        function_350e0(rectangle, coordinates, out, parameters[0]);
        out[0] *= parameters[24];
        out[1] *= parameters[24];
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
                real x = coordinates[0] * 2.0f - 1.0f;
                real radius = (real)sqrt(PIN((coordinates[1] * 2.0f - 1.0f) * (coordinates[1] * 2.0f - 1.0f) + x * x, 0.0f, 1.0f));
                real fraction = PIN((radius - parameters[22]) / (parameters[23] - parameters[22]), 0.0f, 1.0f);
                weight *= fraction;
            }
            real x_sign = -0.5f;
            if (index & 1)
                x_sign = 0.5f;
            real y_sign = -0.5f;
            if (index & ~1)
                y_sign = 0.5f;
            out[0] = (parameters[1] * x_sign + weight * center_x + (1.0f - weight) * out[0]) * parameters[10 + index * 2];
            out[1] = ((1.0f - weight) * out[1] + parameters[1] * y_sign + weight * center_y) * parameters[11 + index * 2];
        }
        else
        {
            real x_sign = (index & 1) ? 0.5f : -0.5f;
            real y_sign = (index & ~1) ? 0.5f : -0.5f;
            out[0] = (parameters[1] * x_sign + out[0]) * parameters[10 + index * 2];
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
    perpendicular->i = direction->k * cross.j - direction->j * cross.k;
    perpendicular->j = direction->i * cross.k - direction->k * cross.i;
    perpendicular->k = direction->j * cross.i - direction->i * cross.j;
}

// score 193 (was 193): the statements match retail (cross products, dot-product operand order, atan2 tail).
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
        y = reference.j * perpendicular.j + reference.k * perpendicular.k + reference.i * perpendicular.i;
        x = 0.0f - (direction->k * reference.k + direction->j * reference.j + direction->i * reference.i);
        break;
    case 2:
        reference.i = 0.0f - direction->i;
        reference.j = 0.0f - direction->j;
        reference.k = 0.0f - direction->k;
        y = g_4b9e4c.k * reference.k + g_4b9e4c.j * reference.j + g_4b9e4c.i * reference.i;
        x = 0.0f - (g_4b9e64.k * reference.k + g_4b9e64.j * reference.j + g_4b9e64.i * reference.i);
        break;
    case 3:
        direction_perpendicular_to_up(direction, &perpendicular);
        reference.i = position->x - g_4b9da0.x;
        reference.j = position->y - g_4b9da0.y;
        reference.k = position->z - g_4b9da0.z;
        y = reference.j * perpendicular.j + reference.k * perpendicular.k + reference.i * perpendicular.i;
        x = 0.0f - (direction->k * reference.k + direction->j * reference.j + direction->i * reference.i);
        break;
    case 4:
        reference.i = position->x - g_4b9da0.x;
        reference.j = position->y - g_4b9da0.y;
        reference.k = position->z - g_4b9da0.z;
        y = g_4b9e4c.k * reference.k + g_4b9e4c.j * reference.j + g_4b9e4c.i * reference.i;
        x = 0.0f - (g_4b9e64.k * reference.k + g_4b9e64.j * reference.j + g_4b9e64.i * reference.i);
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
extern dword g_4b8438;
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
    return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

real game_time_get_seconds();

// score 507 (was 508, 594 originally): draft passed (cosine * size, sine * size, x_scale, y_scale) to function_480a0; retail passes
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
        if (visibility <= 0.0f) return 0;
    }
    real context_alpha = 1.0f;
    // retail loads the slot's flags as a dword (mov eax, [esi+4], 0x2eaa9), as 0x20d80 does, not a byte
    if ((*(dword const *)(context + 4) & 2) && (*(dword const *)(context + 4) & 1))
        context_alpha = clamp_2e9e0(context[3] * g_5234c8);
    color3f animated = { 1.0f, 1.0f, 1.0f };
    vector3f delta;
    delta.i = position.x - g_4b9da0.x;
    delta.j = position.y - g_4b9da0.y;
    delta.k = position.z - g_4b9da0.z;
    real forward = g_4b9dac.j * delta.j + delta.i * g_4b9dac.i + delta.k * g_4b9dac.k;
    vector3f reflected;
    reflected.i = (g_4b9dac.i * forward - delta.i) * 2.0f;
    reflected.j = (g_4b9dac.j * forward - delta.j) * 2.0f;
    reflected.k = (g_4b9dac.k * forward - delta.k) * 2.0f;
    real attenuation = 1.0f;
    real near_distance = *(real *)(definition + 0x18), far_distance = *(real *)(definition + 0x1c);
    if (near_distance != far_distance)
        attenuation = clamp_2e9e0((forward - far_distance) / (near_distance - far_distance));
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
            animated = sampled;
        }
        if (*(long *)(definition + 0x5c) > 0)
            base_angle += function_13bb90((s_tag_data *)*(byte **)(definition + 0x60), time, 0.0f) * 3.1415927410125732f;
        for (long i = 0; i < *(long *)(definition + 0x40); ++i)
        {
            byte *entry = *(byte **)(definition + 0x44) + i * 0x30;
            word const *flags = (word const *)entry;
            real opacity = PIN((*(real *)(entry + 0x1c) - *(real *)(entry + 0x18)) * amount + *(real *)(entry + 0x18), 0.0f, 1.0f);
            real size = ((*(real *)(entry + 0x14) - *(real *)(entry + 0x10)) * amount + *(real *)(entry + 0x10)) * scale;
            opacity *= (*flags & 0x20) ? unoccluded_alpha : attenuation;
            if (opacity > 0.0f && size > 0.0f)
            {
                real angle = *(real *)(entry + 0xc) * 0.01745329238474369f;
                real x_scale = 1.0f, y_scale = 1.0f;
                if (i == 0)
                {
                    x_scale = *(real *)(definition + 0x34);
                    y_scale = *(real *)(definition + 0x38);
                    angle += base_angle;
                }
                color3f selected;
                selected.red = *(real *)(entry + 0x24) * animated.red;
                selected.green = *(real *)(entry + 0x28) * animated.green;
                selected.blue = *(real *)(entry + 0x2c) * animated.blue;
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
                            g_4b8438 = 2;
                            D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
                            g_4c19c0 = true;
                        }
                    }
                    else if (g_4c19c0)
                    {
                        g_4b8438 = 0;
                        D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
                        g_4c19c0 = false;
                    }
                    D3DDevice_SetVertexData2f(8, clamp_2e9e0(1.0f - *(real *)(entry + 0x20)), 0.0f);
                    real sine = angle == 0.0f ? 0.0f : (real)sin((double)angle);
                    real cosine = angle == 0.0f ? 1.0f : (real)cos((double)angle);
                    function_480a0(&point, x_scale * size, y_scale * size, cosine, sine,
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

PRIVATE __forceinline vector3f direction_from_angles(real heading, real elevation)
{
    vector3f result;
    result.i = (real)(cos(elevation) * cos(heading));
    result.j = (real)(cos(elevation) * sin(heading));
    result.k = (real)sin(elevation);
    return result;
}

// score 188 (was 188; earlier 204, 220, 298): frame 0x80 vs 0xc0, vector/loop homes and argument copies differ.
// Earlier: the statements match retail (cos(elevation) is evaluated first, the entry's tag is read again
// at the call instead of cached, `left` is built from g_4687b0 directly while `up` is a separate copy). Writing the result back
// with `local = direction;` (retail stores the new direction into the slot of `local`, 0x14) took 220 to 204. What still differs:
// frame 0x7c against retail's 0xc0 and 846 against 857 bytes, and the register roles (retail keeps the loop count in ebx, the entry
// in ebp, spills `definition` to [esp+4] and the entry offset to [esp+0x2c]; ours keeps `definition` in ebx). Retail builds `local`
// and `forward` straight into their slots (0x14, 0x20) and then block-copies local, forward and up into one argument-like area
// (0x30, 0x3c, 0x54, with a slot for `left` at 0x48 that is never stored); `left` stays in xmm registers there.
// Retail reads the entry's heading and elevation straight from memory for `local` (fld [ebp+0x10] ... fld [ebp+0xc] twice) but
// loads the definition's heading once for `forward`, so `local` is not a call of the same helper as `forward`.
// Tried: sin/cos without the (real) casts (223), out-pointer helper for both (284/260), for `local` only (256/228), by-value basis helper with `left` as a vector (261, 357 when
// passed whole) or as three reals (253), `left` from a returning cross helper (235), `local` written inline from the entry fields
// (269/247), a by-value world_direction(local, forward, up) helper (203 but 846 bytes of different temps), basis declared before position (no change).
// Round 435: shared/staged left cross 182/182; native matrix plus shared rotation 181/181,
// in-place 189/189; original cross rows with shared rotation 191, in-place 189. All restored;
// six/seven-point leads are below the keep gate. Current frame 0x80 vs 0xc0; loop/vector homes differ.
// @retail 0x3eec0
void function_3eec0()
{
    if (g_4b9ee9 && g_4b9eec != NONE && g_4c5038.valid)
    {
        s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
        long index = NONE;
        if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
            index = table->entries[(short)g_4b9eec].tag;
        byte *definition = NULL;
        if (index != NONE) definition = g_4e3b44[index & 0xffff].bytes;
        for (long i = 0; i < *(long *)(definition + 0x78) && i < 64; ++i)
        {
            byte *entry = *(byte **)(definition + 0x7c) + i * 0x34;
            if (*(long *)(entry + 0x18) == NONE) continue;
            vector3f local = direction_from_angles(*(real *)(entry + 0xc), *(real *)(entry + 0x10));
            vector3f forward = direction_from_angles(*(real *)(definition + 0x80), 0.0f);
            vector3f up = *g_4687b0;
            vector3f left;
            left.i = g_4687b0->j * forward.k - g_4687b0->k * forward.j;
            left.j = g_4687b0->k * forward.i - g_4687b0->i * forward.k;
            left.k = g_4687b0->i * forward.j - g_4687b0->j * forward.i;
            vector3f direction;
            direction.i = local.j * left.i + local.k * up.i + local.i * forward.i;
            direction.j = local.i * forward.j + local.j * left.j + local.k * up.j;
            direction.k = local.i * forward.k + local.j * left.k + local.k * up.k;
            local = direction;
            if (1.0f > g_4b9f58)
            {
                g_4670bc = true;
                function_16b10((s_render_reset_state *)g_485b48);
                point3f position;
                position.x = local.i * 1023.875f + g_4b9da0.x;
                position.y = local.j * 1023.875f + g_4b9da0.y;
                position.z = local.k * 1023.875f + g_4b9da0.z;
                vector3f basis[3];
                basis[0].i = 0.0f - local.i;
                basis[0].j = 0.0f - local.j;
                basis[0].k = 0.0f - local.k;
                function_11d000(&basis[0], &basis[2]);
                function_30bf0(&basis[2]);
                function_2dba0(*(long *)(entry + 0x18), basis, false, 3, 0, i, &position, (color3f const *)((byte const *)g_4686cc + 4), 1.0f - g_4b9f58, 1.0f, 1.0f);
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

// score 57 (was 202): the random fraction is (real)random * (1/65535) as before; motion[2] is stored
// first and the length is then built from the stored motion[] values, so the just-stored z stays on the
// x87 stack unrounded (retail's `fst [edx+8]`) with no temp; the length is a double kept for fabs, with the
// float copy (`magnitude`) used by the divide and `fabs((real)length)` so the 0.0001 constant stays a float
// (retail: sqrt result moved into z's slot, `fst` to the float temp); the six -1..1 samples are `real`
// (sample + sample - 1.0f), which gives retail's dword `fsub`; the entry+0xc fraction is evaluated before its
// range, as retail does. What still differs: retail pushes ebx/ebp and esi/edi lazily inside the two count
// guards (we push all four at the top, with a 4-byte larger frame; ours also loads the random globals and the
// count above the second guard); `motion[1] = inverse * motion[1]` order (same code); the loop test reads the
// count into edx instead of comparing against [ebx].
// Tried: explicit `if (count > 0)` guard around the loop (257), count read once before the loop (116),
// double length without the float copy (172/106), assignment of the length inside the fabs (178),
// `(real)fabs(length)` (106), samples as doubles with a float constant (does not change the fsub).
// Round 280: loop-local/random-at-every-use 57/57; plain while 57; repeated count
// receiver alone/with loop random/while/both 57 each; outer empty-state exit with
// loop random and count receiver 300. Exact-only receiver forms restored with the
// full original body; lazy saves and the post-fstp memory compare remain.
// @retail 0x39a80
bool function_39a80(byte *state)
{
    *(real *)(state + 0x80) = 0.0f;
    state[0x86] = false;
    if (*(long *)(state + 0x64) > 0)
    {
        byte *group = *(byte **)(state + 0x68);
        s_random_globals *random = g_4e7408;
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
            double upper = *(real *)(state + 0x20);
            double lower = *(real *)(state + 0x1c);
            motion[6] = (real)(random_unit_fraction(random) * (upper - lower) + lower);
            motion[0] = (real)(random_unit_fraction(random) * *(real *)(state + 0x30));
            motion[1] = (real)(random_unit_fraction(random) * *(real *)(state + 0x34));
            motion[2] = (real)(random_unit_fraction(random) * *(real *)(state + 0x38));
            double length = sqrt((double)motion[0] * motion[0] + (double)motion[1] * motion[1] + (double)motion[2] * motion[2]);
            real magnitude = (real)length;
            if (!(fabs((real)length) < 0.0001f))
            {
                real inverse = 1.0f / magnitude;
                motion[0] *= inverse;
                motion[1] *= inverse;
                motion[2] *= inverse;
            }
            double fraction = random_unit_fraction(random);
            *(real *)(entry + 0xc) = (real)(fraction *
                (*(real *)(state + 0x28) - (double)*(real *)(state + 0x24)) + *(real *)(state + 0x24));
            motion[7] = 0.0f;
            if (0.0001f > *(real *)(state + 8)) *(real *)(state + 8) = 1.0f;
            if (0.0001f > *(real *)(state + 0xc)) *(real *)(state + 0xc) = 1.0f;
            if (0.0001f > *(real *)(state + 0x10)) *(real *)(state + 0x10) = 1.0f;
            real sample = (real)random_unit_fraction(random);
            *(real *)entry = (sample + sample - 1.0f) * *(real *)(state + 0x10);
            sample = (real)random_unit_fraction(random);
            *(real *)(entry + 4) = (sample + sample - 1.0f) * *(real *)(state + 8);
            sample = (real)random_unit_fraction(random);
            *(real *)(entry + 8) = (sample + sample - 1.0f) * *(real *)(state + 0xc);
            sample = (real)random_unit_fraction(random);
            motion[3] = sample + sample - 1.0f;
            sample = (real)random_unit_fraction(random);
            motion[4] = sample + sample - 1.0f;
            sample = (real)random_unit_fraction(random);
            motion[5] = sample + sample - 1.0f;
        }
        state[0x86] = true;
    }
    *(real *)(state + 0x88) = 1.0f;
    return true;
}



real magnitude3d(vector3f const *v);

// score 182 (earlier 193, 324; 147 with the shared cross3f axis, held: see Round 435): PIN reevaluates the clamped expression as retail does (the target's
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
        real delta = target->i - current->i;
        current->i += delta < -step ? -step : delta > step ? step : delta;
        delta = target->j - current->j;
        current->j += delta < -step ? -step : delta > step ? step : delta;
        delta = target->k - current->k;
        current->k += delta < -step ? -step : delta > step ? step : delta;
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

// score 132 (was 132; 139; r581: 139; 146 -> 139 on a 169-source fast image): retail keeps 0.0f, 0.1f and 2.0f in xmm5-xmm7 across the loop; here they are
// loaded inside it, and the depth dot product orders its terms differently. Tried: all six
// term orders and several groupings (140), constants in locals (140). The flag as a branchy
// && stored through a long (136).
// Round 186 (plain checks, positional | LCS-aligned, base 132 | 71): retail keeps `list` in a frame slot
// (loaded once, re-read at the loop top) with i in esi, the index in ebx, the object in edi and the
// cursor in ebp; ours gives `list` ebp. `g_547f98` read at each use instead of the `list` local 113 | 77
// (frame 0x9c, retail 0xa0, so not kept), with `!(depth >= 0.0f)` 111 | 75, with the camera fields 110
// | 75, with `g >= 0 ? (g <= 3 ? g : 3) : 0` 113 | 83; `!(depth >= 0.0f)` alone 132 | 69 (retail's
// `comiss depth, 0; jae`); the depth as `g_4b9e14.right.k * x + up.k * y + forward.k * z + position.z`
// 133 | 68. The view/count slots are swapped against retail as in 0x33400 (view is register-assigned
// at the clamp's join, then spilled: `xor eax, eax` for the zero arm and a dead reload).
// Round 109 (plain checks, positional | LCS-aligned, base 132 | 61): the three forms that matched sibling 0x33400
// (`&g_5234e8[view]` at both calls in place of the `state` local, the depth sum written `g_4b9e2c * y + g_4b9e20 * x
// + g_4b9e38 * z`, `!(depth >= 0.0f)`) give retail's clamp stores, view/count slots and xmm5-xmm7 constants:
// 130 | 54, not kept (a 2-point gain). What is left is a register permutation: ours keeps `list` in EBP (pushed at
// entry, spilled for the entry pointer and reloaded after 0x3e9c0), index in EDI (0x31590's argument register),
// object in ESI; retail spills `list` (reloaded into EAX at the loop top), keeps index in EBX (copied to EDI for
// 0x31590), object in EDI, i/entry in ESI and the cursor in EBP. On those forms: object read before the definition
// 130 | 54; `shape` declared before index 130 | 54; `view` declared before `count` 128 | 55; g_547f98 read at
// each use (no list local, 556 bytes) 118 | 65.
// Round 226: native shape/list/output-record reference subsets and enclosing native shape each 136 on a
// reduced control of 136; native shape/list plain 132. Coordinated callee inline 136 reduced, 139 plain
// (549 bytes); restored. The list/cursor register split remains; original body and contracts restored.
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
            long object = *(long *)(entry + 0x4c);
            long parent = function_baf80(object);
            point3f position = *(point3f *)(entry + 0x18);
            real radius = *(real *)(entry + 0x24);
            s_cached_input *result = &output[count];
            result->data[1] = index;
            result->data[2] = parent;
            result->data[3] = *(dword *)(entry + 0x100);
            real depth = g_4b9e38 * position.z + g_4b9e20 * position.x + g_4b9e2c * position.y + g_4b9e44;
            if (depth < 0.0f) depth = 0.0f - depth;
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
    s_cached_input_list *state = &g_5234e8[view];
    function_4a7c0(state, output, count);
    return function_4ac30(state, output, 3, 0.01f);
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

// score 317 (was 317): the logic was walked against retail block by block (0x4ac30-0x4b159) and
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

// score 517 (was 529; 561 before that): the logic matches retail block by block (walked statement by statement against
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
            real norm = (real)sqrt((double)velocity->k * velocity->k + (double)velocity->i * velocity->i + (double)velocity->j * velocity->j);
            if (!(fabs(norm) < 0.0001f))
            {
                real inverse = 1.0f / norm;
                velocity->i *= inverse;
                velocity->j *= inverse;
                velocity->k *= inverse;
            }
            velocity->i *= maximum;
            velocity->j *= maximum;
            velocity->k *= maximum;
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

#include "object_markers.h"

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

// score 146 (was 149 on a 169-source fast image): retail has color, marker count and the markers in one frame block with the
// amount in its own slot, and loops on a count it holds in ebx; the slots still differ. Tried:
// separate color/count/markers locals (+1), one shared scale local (no change).
// @retail 0x41980
void __stdcall function_41980(short type, long object_index)
{
    struct s_attachment_scratch_r16 { color3f color; long count; s_object_marker markers[65]; };
    s_attachment_scratch_r16 scratch_r16;
    
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
                scratch_r16.count = function_b8d30(object_index, entry->marker, scratch_r16.markers, 65, false);
                real amount = function_be6d0(object_index, i);
                scratch_r16.color = *(color3f const *)g_468710;
                if (entry->color_index)
                    function_bad50(object_index, entry->color_index, (point3f *)&scratch_r16.color);
                for (long j = 0; j < scratch_r16.count; ++j)
                {
                    transform4x3f *matrix = &scratch_r16.markers[j].matrix;
                    if (entry->type == 0x6c656e73)
                    {
                        if (added < 64)
                        {
                            byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                            real scale = (tag[0x28] & 0x40) ? matrix->scale : 1.0f;
                            function_2dba0(entry->tag, &matrix->forward, function_3e9c0(object_index), 0, object_index & 0xffff, added, &matrix->position, &scratch_r16.color, 1.0f, amount, scale);
                            ++added;
                        }
                        else if (!g_55e6c7)
                            g_55e6c7 = true;
                    }
                    else if (entry->type == 0x4d475332)
                    {
                        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                        real scale = 1.0f;
                        if (*(long *)(tag + 8) > 0 && (**(byte **)(tag + 12) & 0x20))
                            scale = matrix->scale;
                        function_42850(object_index, entry->tag, &matrix->position, (vector3f *)&scratch_r16.color,
                            &matrix->forward, scale, amount, &matrix->up);
                    }
                    else if (entry->type == 0x7464746c)
                        function_429a0(0, object_index, entry->tag, NONE, &matrix->position,
                            NULL, &matrix->forward, &matrix->up, 1.0f);
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

// score 177 (was 175, 194): the alpha is read from function_4baf0's second output (the byte pushed
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
// @retail 0x3d7f0
bool __stdcall function_3d7f0(long object_index, real *out_alpha, bool *out_special)
{
    bool result;
    s_scalar_object_header *header = &((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff];
    byte **object_slot = &header->object;
    byte *object = *object_slot;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    volatile long kind = *(signed char *)(object + 0xaa); // kept in memory as retail does, see the note above
    real alpha = 0.0f;
    if (!(object[6] & 1))
    {
        byte first_output, opacity;
        switch (*(short *)(definition + 0x18))
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
        if (alpha > 0.0f)
        {
            result = true;
            if (kind == 0)
            {
                if ((bool)(((dword)object[0x10a] >> 2) & 1))
                {
                    real elapsed = (g_510c54->game_time - *(long *)(object + 0xbc)) * g_510c54->rate - 2.0f;
                    alpha = 1.0f - elapsed;
                    if (alpha < 0.0f) alpha = 0.0f;
                    else if (alpha > 1.0f) alpha = 1.0f;
                    result = alpha > 0.0f;
                }
                real fade = *(real *)(((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object + 0x2b0);
                real clamped = fade < 0.0f ? 0.0f : fade > 1.0f ? 1.0f : fade;
                if (clamped == fade)
                    alpha *= 1.0f - fade;
            }
        }
        else
            result = false;
    }
    else
        result = false;
    if (!result && *(long *)(object + 0x14) != NONE)
        result = function_3d7f0(function_baf80(object_index), &alpha, out_special);
    header = &((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff];
    if ((1 << ((byte *)header)[3]) & 3)
    {
        object = header->object;
        real fade = *(real *)(object + 0x2b0);
        if (fade < 0.0f) fade = 0.0f;
        else if (fade > 1.0f) fade = 1.0f;
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

// score 4 (was 4): only the frame slots of `table` and `definitions` are swapped (retail: table at the lower
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
    corner->x = matrix->forward.i * x + matrix->up.i * z + matrix->left.i * y + matrix->position.x + origin->x;
    corner->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y + origin->y;
    corner->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z + origin->z + 0.001f;
}

// score 394 (was 394): the four corners are written out (retail has four copies of the scale
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
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[0].x, corners[0].y, corners[0].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 0.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[1].x, corners[1].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[2].x, corners[2].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 0.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[3].x, corners[3].y, corners[3].z, 1.0f);
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
    function_0497a0(source, (s_view_result *)g_48597c, camera, flags, &g_485944, &g_485950, g_48595c,
        &g_485a2c, (vector4f *)g_485a0c, (vector4f *)(g_485a0c + 4));
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

// score 272 (was 272; earlier 257 with a volatile counter): frame 0x74 vs 0x70, early zero and loop homes differ.
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
        long fog = *(short *)(clusters + cluster * 0xb0 + 0x76);
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
extern byte g_485a75, g_485a76, g_4ba022, g_4ba023;
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

// score 1 (was 1): indexed byte-load encoding remains.
// Native previous-level short/long, four-byte byte/signed-char table pointers, selected byte/signed-char element
// pointers, read-only table with char/short previous; pointer advance mutable/const, exhaustive null branches,
// late null-pointer lifetime: 13/53/1/1/1/1/1/13/1/1/50/53. Indexed cache byte still differs only in SIB
// base/index order; original loads and arithmetic restored.
// A 480-second annealed search: 45 attempts, none below 1.
// Direct/commuted pointer addition, cache/slot declarations swapped, role-based local names, an unsigned index,
// one-byte memcpy, implicit byte conversion and a four-element array reference: all 1. Original body restored.
// @retail 0x4c450
long function_4c450(long object_index, real distance, real const *definition, real *fraction)
{
    (void)&object_index;
    real defaults[6] = { 10.0f, 8.0f, 6.0f, 4.0f, 2.0f, 0.0f };
    real thresholds[6];
    real upper;
    *fraction = 0.9999f;
    if (definition && definition[3] > 0.0f && definition[3] >= definition[4]
        && definition[4] >= definition[5] && definition[5] >= definition[6]
        && definition[6] >= definition[7] && definition[8] == 0.0f)
    {
        memcpy(thresholds, definition + 3, sizeof(thresholds));
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
            real value = (upper - distance) / (upper - lower);
            if (value < 0.0f) value = 0.0f;
            else if (value > 0.9999f) value = 0.9999f;
            *fraction = value;
            break;
        }
        ++level;
    }
    signed char previous = -1;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        byte *slot = NULL;
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
            slot = g_509434->data + (cache & 0xffff) * 0x100 + 0xbc;
        previous = (signed char)slot[g_4ba050];
    }
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

// score 43 (was 270): the three clamps use PIN, which reevaluates the
// clamped expression as retail does (for the angle, function_50650 inlined
// twice and then called), and the curve's base value is read where it is
// used. What still differs: retail pushes ebx and edi after the first test and
// keeps the state pointer in esi (ours ebx); it stores the pow result straight
// to *strength and rereads it, comparing with a zeroed register; and it tests
// function_30830's result as a byte. A pow on *strength or in double, and a
// bool return for function_30830 (195 -> 189 for that function, 81 -> 80
// here, across four files) were tried.
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
            if (*strength > 0.0f)
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
void function_0494b0(real const *bounds, real const *clip);
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

PRIVATE __forceinline void draw_light_source_pass(long group, long mode, bool extra_enabled, bool depth, bool normal_cull, long target, bool filter)
{
    byte enabled = (byte)(g_4c1a48[group].flags >> mode) & extra_enabled;
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

// score 88 (was 182): only difference left is one stack slot. Retail's frame is 4 bytes larger: it stores
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
        if (function_b8d30(entry->key, entry->value, &marker, 1, false) <= 0)
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
        function_0494b0(clip, (real const *)&bounds3d);
        draw_configured_pass_r18(10);
        draw_light_source_pass(1, 11, true, false, false, g_4858b8, true);
        group.flags = group.unknown10;
        group.first = 0;
        rendered = true;
    }
    g_4c19ac = first_group;
    if (rendered) function_40e90(0);
}


#include "index_cache_storage.h"
extern bool g_4ba010;
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

PRIVATE __forceinline void draw_selected_source_pass(long group, long mode, byte extra_enabled, bool depth, bool normal_cull, long target, bool filter)
{
    byte enabled = (g_4c1a48[group].flags >> mode) & 1;
    if (enabled) enabled = extra_enabled;
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

// score 165 (was 165, 213): the pass flag is now tested as retail does (and al,1 stored to the stack slot, then
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
    draw_selected_source_pass(0, 1, true, true, true, 1, false);
    draw_selected_source_pass(0, 3, true, true, true, g_4858b8, false);
    draw_selected_source_pass(0, 8, g_4ba022 != 0, false, false, g_4858b8, false);
    draw_selected_source_pass(0, 7, true, false, false, g_4858b8, false);
    draw_selected_source_pass(0, 2, g_4ba023 != 0, false, true, g_4858b8, false);
    draw_selected_source_pass(0, 16, true, false, false, g_4858b8, false);
    draw_configured_pass_r18(15);
    draw_configured_pass_r18(21);
    group.first = 0;
    group.flags = group.unknown10;
}

void function_48ee0(byte const *state, byte const *matrix, byte const *tag);
void __stdcall function_15720(real x, real y);

extern byte g_4ba018;
extern dword g_4b843c;

// score 237 (was 278, 300): the camera is now a register parameter (the `&camera` alias that forced it onto the stack is gone, so the
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
        draw_light_source_pass(1, 11, true, false, false, g_4858b8, true);
    if (g_510c50 && ((byte *)g_510c50)[5] && (bool)((g_4ba014 >> 2) & 1) && g_4ba134 > 0) function_30ed0();
    draw_light_source_pass(1, 14, true, false, false, 1, false);
    draw_light_source_pass(1, 12, true, false, false, g_4858b8, false);
    if ((g_4ba014 & 1) && (bool)((g_4ba014 >> 1) & 1)) draw_light_source_pass(1, 13, true, false, false, g_4858b8, false);
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

// score 138 (was 210): the passes now use draw_selected_source_pass (shared with 0x423c0), whose `enabled` is a separate
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
    draw_selected_source_pass(0, 1, true, true, true, 1, false);
    draw_selected_source_pass(0, 3, true, true, true, g_4858b8, false);
    draw_selected_source_pass(0, 8, g_4ba022 != 0, false, false, g_4858b8, false);
    draw_selected_source_pass(0, 20, true, false, false, g_4858b8, false);
    draw_selected_source_pass(0, 7, true, false, false, g_4858b8, false);
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
long function_3ba90(long bit, s_record_sources const *data);
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

// score 331 (was 453; the fast check's score counts every line after a one-instruction shift, so most of it is
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
                draw_selected_source_pass(0, 1, true, true, true, 1, false);
                draw_selected_source_pass(0, 3, true, true, true, g_4858b8, false);
                draw_selected_source_pass(0, 5, true, false, false, g_4858b8, false);
                draw_selected_source_pass(0, 8, g_4ba022, false, false, g_4858b8, false);
                draw_selected_source_pass(0, 20, true, false, false, g_4858b8, false);
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
                    long last = function_3ba90(6, &g_4c1a48[0]);
                    draw_configured_pass_r18(6);
                    function_3c3c0(true, last != 0);
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
void __stdcall function_29b00(long tag, short first_bitmap, real first_alpha, real first_angle, real first_scale, long second_bitmap, real second_alpha, real second_angle, real second_scale, bool screen_target);
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

// score 491 (was 492, 623, 506; 491 since 0x31170 sets g_4ba014 bits through bit_vector_set): retail returns a separate false byte, not the special flag (`mov al, [esp+0xb]` on every
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
// @retail 0x32350
bool __stdcall function_32350(long index, real input_alpha)
{
    byte *object = ((s_scalar_object_header *)g_4e0300->data)[index & 0xffff].object;
    bool result = false;
    real alpha = 0.0f;
    bool special;
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
            center.x = second_point.x + box.matrix.forward.i * (0.0f - box.radius.i);
            center.y = second_point.y + box.matrix.forward.j * (0.0f - box.radius.i);
            center.z = second_point.z + box.matrix.forward.k * (0.0f - box.radius.i);
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
                    draw_light_source_pass(1, 11, true, false, false, g_4858b8, true);
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
