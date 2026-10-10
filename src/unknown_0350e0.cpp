// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0350E0.CPP: format conversion and geometry helpers */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "crc.h"
#include "visibility_slot.h"
#include "geometry_cache.h"
#include "unknown_0494b0.h"


struct s_frame_offset
{
    point3f position;
    vector3f forward;
    vector3f up;
};
extern s_frame_offset g_485618;
matrix3x3 g_467104 = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

PRIVATE __forceinline void rotation_from_axis(vector3f const *axis, double angle, matrix3x3 *matrix)
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

// @retail 0x39e50
void __stdcall function_39e50(real step)
{
    vector3f axis0 = g_485618.forward;
    vector3f axis1 = g_485618.up;
    matrix3x3 first, second;
    rotation_from_axis(&axis0, (double)step * 0.5f, &first);
    rotation_from_axis(&axis1, (double)step * 0.30000001192092896f, &second);
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
    if (state->large_count > 0)
    {
        long i = 0;
        long offset = 0;
        do
        {
            function_36f50(state->large_entries + offset);
            ++i;
            offset += 0x3a8;
        } while (i < state->large_count);
    }
    if (state->count > 0)
    {
        long i = 0;
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

union s_transition_scalar
{
	real value;
	long bits;
};

struct s_transition_state
{
	union
	{
		long flags;
		struct { byte active; byte changed; word unknown02; };
	};
	long index;
	long unknown08;
	s_transition_scalar start;
	s_transition_scalar elapsed;
	s_transition_scalar output;
};

s_transition_state g_4670cc = {0, 0, 0, {0.0f}, {-1.0f}, {0.0f}};
struct s_transition_filter
{
	long unknown00;
	s_transition_scalar previous;
	s_transition_scalar delta;
};
s_transition_filter g_4b99a0;
byte g_4b99b0[0x4c];

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

extern bool g_4b9ee9;
extern long g_4b9eec;

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

struct rigid_transform_scaled
{
	quaternionf rotation;
	point3f position;
	real scale;
};

struct s_render_model_definition;
void render_model_get_default_orientations(s_render_model_definition const *definition, rigid_transform_scaled *orientations);
void render_model_build_node_matrices(vector3f const *forward, vector3f const *up, point3f const *position,
	s_render_model_definition const *definition, transform4x3f *matrices, rigid_transform_scaled const *orientations);

bool g_4c5038;
long g_4c503c;
transform4x3f g_4c5040[255];

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
		long count = *(long *)(definition + 0x48);
		if (count != g_4c503c)
		{
			g_4c503c = count;
			g_4c5038 = false;
			return;
		}
		if (g_4c5038)
		{
			function_3ebd0(g_4687a4, g_4c5040, matrices, count);
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
		g_4c5038 = true;
		g_4c503c = *(long *)(definition + 0x48);
		render_model_build_node_matrices(g_4687a8, g_4687b0, g_468788,
			(s_render_model_definition *)definition, g_4c5040, scratch.orientations);
	}
}

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
void function_350e0(real *out, real const *a, real const *b, real x)
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
real function_36a40(real mean, real spread, real minimum, real maximum)
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
	a.red += color_delta(b.red - a.red, 0.0f - step, step);
	a.green += color_delta(b.green - a.green, 0.0f - step, step);
	a.blue += color_delta(b.blue - a.blue, 0.0f - step, step);
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
	volatile long begin = first + 1;
	{
    long i = first;
    if (i <= *last_reference)
    {
        do
        {
		for (long j = begin; j <= *last_reference; ++j)
		{
			bool swap;
			switch (mode)
			{
			case 0: swap = entries[j].distance > entries[j - 1].distance; break;
			case 1: swap = entries[j - 1].key < entries[j].key; break;
			default: swap = entries[j].depth > entries[j - 1].depth; break;
			}
			if (swap)
			{
				s_4b160_entry temporary = entries[j - 1];
				entries[j - 1] = entries[j];
				entries[j] = temporary;
			}
		}
	
            ++i;
        } while (i <= *last_reference);
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

// @retail 0x4b3d0
void function_4b3d0(s_vector_perturbation *state)
{
	state->speed_fraction = function_36a40(state->speed_fraction, state->speed_spread * g_509418, 0.0f, 1.0f);
	real speed = (state->speed_maximum - state->speed_minimum) * state->speed_fraction + state->speed_minimum;
	state->velocity.i = state->direction.i * speed;
	state->velocity.j = state->direction.j * speed;
	state->velocity.k = state->direction.k * speed;
	state->first_fraction = function_36a40(state->first_fraction, state->direction_spread * g_509418, -1.0f, 1.0f);
	state->second_fraction = function_36a40(state->second_fraction, state->direction_spread * g_509418, -1.0f, 1.0f);
	state->magnitude_fraction = function_36a40(state->magnitude_fraction, state->magnitude_spread * g_509418, 0.0f, 1.0f);
	real first = state->first_fraction * state->direction_scale;
	state->perturbation.i = state->first_axis.i * first + state->base_direction.i;
	state->perturbation.j = state->first_axis.j * first + state->base_direction.j;
	state->perturbation.k = state->first_axis.k * first + state->base_direction.k;
	real second = state->second_fraction * state->direction_scale;
	state->perturbation.i = state->second_axis.i * second + state->perturbation.i;
	state->perturbation.j = state->second_axis.j * second + state->perturbation.j;
	state->perturbation.k = state->second_axis.k * second + state->perturbation.k;
	function_30bf0(&state->perturbation);
	real magnitude = state->magnitude * state->magnitude_fraction;
	state->perturbation.i *= magnitude;
	state->perturbation.j *= magnitude;
	state->perturbation.k *= magnitude;
	state->velocity.i += state->perturbation.i;
	state->velocity.j += state->perturbation.j;
	state->velocity.k += state->perturbation.k;
	state->step = state->step_scale * g_509418;
}

struct s_33a0b_view;
extern s_33a0b_view *g_485a58;
extern real g_485774, g_485a6c;
extern long g_4b9ed8;
real g_485a30, g_4857f8, g_485864, g_485858;
real g_48578c, g_485868, g_4857a4;
long g_4b9f5c;
real g_4b9f84, g_4b9f80;
real g_4e69c0[4];
real g_4c19b0, g_4c19b4;

// @retail 0x336f0
real function_336f0(long selector)
{
    switch (selector)
    {
    case 0: return 0.0f;
    case 2: return 0.0f;
    case 13: return 0.0f;
    case 38: return 0.0f;
    case 16: return 1.0f > g_485a30 ? g_485a30 : 1.0f;
    case 23:
        if (g_485a58)
        {
            real const *values = (real const *)g_485a58;
            color3f summed;
            summed.red = values[4] + values[12];
            summed.green = values[5] + values[13];
            summed.blue = values[6] + values[14];
            real value = summed.blue * 0.114f + summed.green * 0.587f + summed.red * 0.299f;
            return 0.0f > value ? 0.0f : value > 1.0f ? 1.0f : value;
        }
        return 1.0f;
    case 12: return 1.0f;
    case 22: return 1.0f;
    case 17: return g_485a30;
    case 10: return g_485774;
    case 24: return g_485774;
    case 3: return g_4857f8;
    case 33: return g_4857f8;
    case 4: return g_485864;
    case 25: return g_485864;
    case 18: return g_485864 * g_485774;
    case 27: return g_485864 * g_485774;
    case 19: return g_485858 * g_4857f8;
    case 36: return g_485858 * g_4857f8;
    case 20: return (1.0f - g_485858) * g_4857f8;
    case 35: return (1.0f - g_485858) * g_4857f8;
    case 21: return g_485a6c;
    case 26: return (1.0f - g_485864) * g_4857f8;
    case 28: return g_48578c;
    case 29: return g_485868;
    case 30: return (1.0f - g_485868) * g_48578c;
    case 31: return g_485868 * g_485774;
    case 32: return g_4857a4;
    case 34: return g_485858;
    case 37: return function_33670(*(long *)g_485a58);
    case 39:
        if (g_4b9f5c != NONE && g_4b9f84 > g_4b9f80)
            return 0.0f - (1.0f / (g_4b9f84 - g_4b9f80)) * g_4b9f80;
        return 0.0f;
    case 40:
        if (g_4b9ed8 >= 0 && g_4b9ed8 < 4)
            return g_4e69c0[g_4b9ed8];
        return 1.0f;
    case 41: return g_4c19b0;
    case 42: return g_4c19b4;
    default: __assume(0);
    }
}

struct s_slot_key
{
    byte a;
    long b, c, d, e;
};
long function_0209b0(s_slot_key *key, void const *data, long size);
void function_020b40(long value, long index);
extern point3f g_4b9da0;
extern vector3f g_4b9dac;
long g_4b9ed4;

// @retail 0x2dba0
bool function_2dba0(long tag, vector3f const *direction, long c, long d, long e, point3f const *position, color3f const *color, real alpha, real amount, real scale, bool alternate)
{
    bool result_value = 0;
    bool volatile result = false;
    if (g_4b9ed4 != NONE && tag != NONE && alpha > 0.0f)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if ((amount > 0.0f || !(definition[0x2a] & 1)) &&
            !(definition[0x28] & (alternate ? 8 : 4)))
        {
            vector3f delta;
            delta.i = position->x - g_4b9da0.x;
            delta.j = position->y - g_4b9da0.y;
            delta.k = position->z - g_4b9da0.z;
            real near_distance = *(real *)(definition + 0x18);
            real far_distance = *(real *)(definition + 0x1c);
            if (near_distance == far_distance || far_distance >
                delta.k * g_4b9dac.k + g_4b9dac.j * delta.j + g_4b9dac.i * delta.i)
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
    { result_value = result; goto return_exit; }

return_exit:
    return result_value;
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
        real heading = *(real *)(definition + 0x80);
        vector3f forward;
        forward.i = (real)(cos(heading) * cos(0.0));
        forward.j = (real)(sin(heading) * cos(0.0));
        forward.k = (real)sin(0.0);
        vector3f left;
        left.i = forward.k * g_4687b0->j - forward.j * g_4687b0->k;
        left.j = forward.i * g_4687b0->k - forward.k * g_4687b0->i;
        left.k = forward.j * g_4687b0->i - forward.i * g_4687b0->j;
        transform4x3f transform = *g_4687d0;
        transform.forward = forward;
        transform.left = left;
        transform.up = *g_4687b0;
        transform.scale = *(real *)(definition + 0x14);
        transform.position.x = *(real *)(definition + 0x18) * g_4b9da0.x;
        transform.position.y = *(real *)(definition + 0x18) * g_4b9da0.y;
        transform.position.z = *(real *)(definition + 0x18) * g_4b9da0.z;
        /* The existing assembly callee reads its first matrix through ECX.
           Keep these local stores visible across that assembly boundary. */
        transform4x3f applied;
        for (long component = 0; component < 13; ++component)
            ((volatile real *)&applied)[component] = ((real const *)&transform)[component];
        for (long i = 0; i < count; ++i)
        {
            out[i] = matrices[i];
            out[i].position.x += offset->i;
            out[i].position.y += offset->j;
            out[i].position.z += offset->k;
            function_142a60(&applied, &out[i], &out[i]);
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
struct s_bit_vector_pool_sizes
{
    short unknown0;
    short list_sizes[4];
    short record_count;
};
struct s_bit_vector_pool
{
    void *context;
    byte unknown004[8];
    c_entry_list *lists[4];
    long indices[0x80];
    dword flags[0x10];
    dword pool[0x200];
    word pool_used, entry_count;
    dword entries[0x200][4];
    dword flags2a60;
    byte unknown2a64[0x2a88 - 0x2a64];
    plane3f plane;
    byte unknown2a98[0x2acc - 0x2a98];
    byte *records;
    byte unknown2ad0[4];
    s_bit_vector_pool_sizes sizes;
};
extern s_bit_vector_pool g_547f88;
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
    real const volatile *box_scale = &box->matrix.scale;
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
        for (long endpoint = 0; endpoint < 2; ++endpoint)
        {
            long i = edge * 2 + endpoint;
            point3f point = ((point3f *)edges)[i];
            if (*box_scale != 1.0f)
            {
                point.x = *box_scale * point.x;
                point.y = *box_scale * point.y;
                point.z = *box_scale * point.z;
            }
            real const *m = (real const *)&box->matrix;
            transformed[i].x = m[1] * point.x + m[7] * point.z + m[4] * point.y + m[10];
            transformed[i].y = m[2] * point.x + m[8] * point.z + m[5] * point.y + m[11];
            transformed[i].z = m[3] * point.x + m[9] * point.z + m[6] * point.y + m[12];
        }
    }
    s_bounds3d projected;
    bool visible = function_1652e0((s_view *)(context + 4), *(real *)(context + 0x74), transformed,
        24, NULL, NULL, &projected, true, false);
    bool overlaps = projected.x1 >= *(real *)(context + 0xa4) && *(real *)(context + 0xa8) >= projected.x0 &&
        projected.y1 >= *(real *)(context + 0xac) && *(real *)(context + 0xb0) >= projected.y0;
    return visible & overlaps;
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
    s_random_globals *random = g_4e7408;
    byte *entry = state + 0xc8;
    for (long i = 0; i < 4; ++i, entry += 0xb8)
    {
        for (long j = 0; j < 9; ++j) ((dword *)entry)[j] = 0;
        for (long k = 0; k < 9; ++k) ((dword *)(entry + 0x24))[k] = 0;
        *(vector3f *)(entry + 0x3c) = *(vector3f *)entry;
        *(vector3f *)(entry + 0x48) = *(vector3f *)entry;
        *(real *)(entry + 0x80) = 1.0f;
        *(real *)(entry + 0x84) = 1.0f;
        *(real *)(entry + 0x78) = 0.0f;
        vector3f const *a = (vector3f const *)(state + 0x24);
        vector3f const *b = (vector3f const *)(state + 0x30);
        vector3f const *c = (vector3f const *)(state + 0x3c);
        *(real *)(entry + 0xac) = (real)sqrt((double)a->i * a->i + (double)a->j * a->j + (double)a->k * a->k);
        *(real *)(entry + 0xb0) = (real)sqrt((double)b->i * b->i + (double)b->j * b->j + (double)b->k * b->k);
        *(real *)(entry + 0xb4) = (real)sqrt((double)c->i * c->i + (double)c->j * c->j + (double)c->k * c->k);
        *(real *)(entry + 0x88) = random_signed_value(random);
        *(real *)(entry + 0x8c) = random_signed_value(random);
        *(real *)(entry + 0x90) = random_signed_value(random);
        *(real *)(entry + 0x94) = random_signed_value(random);
        *(real *)(entry + 0x98) = random_signed_value(random);
        *(real *)(entry + 0x9c) = random_signed_value(random);
        *(real *)(entry + 0xa0) = random_signed_value(random);
        *(real *)(entry + 0xa4) = random_signed_value(random);
        *(real *)(entry + 0xa8) = random_signed_value(random);
    }
}

// @retail 0x34100
bool __stdcall function_34100(long mode, real const *rectangle, real const *coordinates,
    real *out, real const *parameters)
{
    real center_y = (rectangle[2] + rectangle[3]) * 0.5f;
    real center_x = (rectangle[0] + rectangle[1]) * 0.5f;
    long index = mode - 1;
    switch (mode)
    {
    case 0:
        function_350e0(out, rectangle, coordinates, parameters[0]);
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
                real y = coordinates[1] * 2.0f - 1.0f;
                real distance_squared = y * y + x * x;
                if (distance_squared < 0.0f) distance_squared = 0.0f;
                else if (distance_squared > 1.0f) distance_squared = 1.0f;
                real radius = (real)sqrt(distance_squared);
                real fraction = (radius - parameters[22]) / (parameters[23] - parameters[22]);
                if (fraction < 0.0f) fraction = 0.0f;
                else if (fraction > 1.0f) fraction = 1.0f;
                weight *= fraction;
            }
            real x_sign = (index & 1) ? 0.5f : -0.5f;
            real y_sign = (index & ~1) ? 0.5f : -0.5f;
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

vector3f g_4b9e4c;
vector3f g_4b9e64;

// @retail 0x2f2d0
real function_2f2d0(vector3f const *direction, point3f const *position, long mode, real scale)
{
    real result = 0.0f;
    real y;
    real x;
    vector3f reference;
    switch (mode)
    {
    case 1:
    case 3:
        {
            vector3f cross;
            cross.i = direction->j * g_4b9e4c.k - direction->k * g_4b9e4c.j;
            cross.j = direction->k * g_4b9e4c.i - direction->i * g_4b9e4c.k;
            cross.k = direction->i * g_4b9e4c.j - direction->j * g_4b9e4c.i;
            vector3f perpendicular;
            perpendicular.i = direction->k * cross.j - direction->j * cross.k;
            perpendicular.j = direction->i * cross.k - direction->k * cross.i;
            perpendicular.k = direction->j * cross.i - direction->i * cross.j;
            if (mode == 1)
                reference = g_4b9dac;
            else
            {
                reference.i = position->x - g_4b9da0.x;
                reference.j = position->y - g_4b9da0.y;
                reference.k = position->z - g_4b9da0.z;
            }
            y = reference.j * perpendicular.j + reference.k * perpendicular.k + reference.i * perpendicular.i;
            x = 0.0f - (direction->k * reference.k + direction->j * reference.j + direction->i * reference.i);
        }
        break;
    case 2:
    case 4:
        if (mode == 2)
        {
            reference.i = 0.0f - direction->i;
            reference.j = 0.0f - direction->j;
            reference.k = 0.0f - direction->k;
        }
        else
        {
            reference.i = position->x - g_4b9da0.x;
            reference.j = position->y - g_4b9da0.y;
            reference.k = position->z - g_4b9da0.z;
        }
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
extern point3f g_4b9da0;
extern vector3f g_4b9dac;
vector3f g_4b9e58;
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
void function_0222d0(D3DRENDERSTATETYPE state, dword value);

PRIVATE inline real clamp_2e9e0(real value)
{
    return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

// @retail 0x2e9e0
long __stdcall function_2e9e0(byte const *context, s_2e3f0_record const *record)
{
    real time = g_510c54 && g_510c54->active ? g_510c54->game_time * g_510c54->rate : 0.0f;
    long tag = NONE;
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
    if ((context[4] & 2) && (context[4] & 1))
        context_alpha = clamp_2e9e0(context[3] * g_5234c8);
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
    real near_angle = *(real *)(definition + 8), far_angle = *(real *)(definition + 0xc);
    if (near_angle != far_angle)
    {
        real inverse = 1.0f / (near_angle - far_angle);
        real projection = delta.i * direction.i + direction.j * delta.j + direction.k * delta.k;
        attenuation *= clamp_2e9e0(0.0f - far_angle * inverse - projection * inverse);
    }
    attenuation *= clamp_2e9e0(alpha);
    real unoccluded_alpha = context_alpha * attenuation;
    attenuation *= visibility;
    if (!(definition[0x48] & 1))
    {
        dword seed = (dword)context >> 5;
        dword hash = 0xffffffff;
        function_163ba0(&hash, &seed, 4);
        time = (hash & 0xff) * (1.0f / 256.0f) + time + ((hash >> 8) & 0xff);
    }
    if (*(long *)(definition + 0x4c) > 0)
        attenuation *= function_13bb90((s_tag_data *)*(byte **)(definition + 0x50), time, 0.0f);
    if (attenuation > 0.0f)
    {
        real base_angle = function_2f2d0(&direction, &position,
            *(short *)(definition + 0x2c), *(real *)(definition + 0x30));
        real camera_angle = (real)atan2((double)delta.j * g_4b9e4c.j + (double)delta.i * g_4b9e4c.i + (double)delta.k * g_4b9e4c.k,
            (double)delta.j * g_4b9e58.j + (double)delta.i * g_4b9e58.i + (double)delta.k * g_4b9e58.k);
        color3f animated = { 1.0f, 1.0f, 1.0f };
        if (*(long *)(definition + 0x54) > 0)
        {
            s_tag_data *curve = (s_tag_data *)*(byte **)(definition + 0x58);
            function_13be80(curve, function_13b390(curve, time, 0.0f), &animated);
        }
        if (*(long *)(definition + 0x5c) > 0)
            base_angle += function_13bb90((s_tag_data *)*(byte **)(definition + 0x60), time, 0.0f) * 3.1415927410125732f;
        long i = 0;
        do
        {
            byte *entry = *(byte **)(definition + 0x44) + i * 0x30;
            word flags = *(word *)entry;
            real opacity = clamp_2e9e0((*(real *)(entry + 0x1c) - *(real *)(entry + 0x18)) * amount + *(real *)(entry + 0x18));
            real size = ((*(real *)(entry + 0x14) - *(real *)(entry + 0x10)) * amount + *(real *)(entry + 0x10)) * scale;
            opacity *= (flags & 0x20) ? unoccluded_alpha : attenuation;
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
                if (!(flags & 0x10))
                {
                    selected.red *= color.red;
                    selected.green *= color.green;
                    selected.blue *= color.blue;
                }
                if (flags & 1) angle += camera_angle;
                if (flags & 4) size *= clamp_2e9e0((visibility + 1.0f) * 0.5f);
                if (flags & 2) size *= forward;
                real displacement = *(real *)(entry + 8);
                point3f point;
                point.x = displacement * reflected.i + position.x;
                point.y = reflected.j * displacement + position.y;
                point.z = reflected.k * displacement + position.z;
                if (!function_47fd0(*(long *)(definition + 0x24), 0, *(short *)(entry + 4), 0))
                {
                    if ((flags & 8) && !g_4c19c0)
                    {
                        function_0222d0(D3DRS_ZENABLE, 2);
                        g_4c19c0 = true;
                    }
                    else if (!(flags & 8) && g_4c19c0)
                    {
                        function_0222d0(D3DRS_ZENABLE, 0);
                        g_4c19c0 = false;
                    }
                    D3DDevice_SetVertexData2f(8, clamp_2e9e0(1.0f - *(real *)(entry + 0x20)), 0.0f);
                    real sine = angle == 0.0f ? 0.0f : (real)sin((double)angle);
                    real cosine = angle == 0.0f ? 1.0f : (real)cos((double)angle);
                    function_480a0(&point, cosine * size, sine * size, x_scale, y_scale,
                        function_131f40(opacity, &selected));
                }
            }
            ++i;
        } while (i < *(long *)(definition + 0x40));
    }
    if (definition[0x28] & 1)
    {
        if (g_4c1a04 < 4)
        {
            g_4c19c4[g_4c1a04].tag = tag;
            g_4c19c4[g_4c1a04++].position = position;
        }
        else if (!g_55e6c6) g_55e6c6 = true;
    }
    return 0;
}


extern real g_48568c[46];
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern real g_48565c;

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
        real pw = g_48568c[41] * vz + g_48568c[37] * vy + g_48568c[33] * vx + g_48568c[45];
        real rx = g_48568c[30] * radius;
        real ry = g_48568c[35] * radius;
        if (pz > 0.0f)
        {
            real px = g_48568c[38] * vz + g_48568c[34] * vy + g_48568c[30] * vx + g_48568c[42];
            real inverse = 1.0f / pw;
            screen[0] = ((px * inverse + 1.0f) * width + (short)g_48564a * 2.0f - 1.0f) * 0.5f;
            screen[1] = ((1.0f - py * inverse) * height + (short)g_485648 * 2.0f - 1.0f) * 0.5f;
            real depth = pz * inverse;
            screen[2] = (1.0f < depth ? 1.0f : depth) * 16777215.0f;
            screen[3] = pw / g_48565c * 16777215.0f;
            extent[0] = width * inverse * rx * 0.5f;
            extent[1] = height * inverse * ry * 0.5f;
            result = true;
        }
    }
    return result;
}



extern byte g_4670bc;
extern byte g_485b48[0x1fc0];
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
vector3f *function_11d000(vector3f const *v, vector3f *out);
real g_4b9f58;

// @retail 0x3eec0
void function_3eec0()
{
    if (g_4b9ee9 && g_4b9eec != NONE && g_4c5038)
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
            long tag = *(long *)(entry + 0x18);
            if (tag == NONE) continue;
            real heading = *(real *)(entry + 0xc), elevation = *(real *)(entry + 0x10);
            vector3f local;
            local.i = (real)(cos(heading) * cos(elevation));
            local.j = (real)(sin(heading) * cos(elevation));
            local.k = (real)sin(elevation);
            real angle = *(real *)(definition + 0x80);
            vector3f forward;
            forward.i = (real)(cos(angle) * cos(0.0));
            forward.j = (real)(sin(angle) * cos(0.0));
            forward.k = (real)sin(0.0);
            vector3f up = *g_4687b0;
            vector3f left;
            left.i = up.j * forward.k - up.k * forward.j;
            left.j = up.k * forward.i - up.i * forward.k;
            left.k = up.i * forward.j - up.j * forward.i;
            vector3f direction;
            direction.i = local.j * left.i + local.k * up.i + local.i * forward.i;
            direction.j = local.i * forward.j + local.j * left.j + local.k * up.j;
            direction.k = local.i * forward.k + local.j * left.k + local.k * up.k;
            if (1.0f > g_4b9f58)
            {
                g_4670bc = true;
                function_16b10((s_render_reset_state *)g_485b48);
                point3f position;
                position.x = direction.i * 1023.875f + g_4b9da0.x;
                position.y = direction.j * 1023.875f + g_4b9da0.y;
                position.z = direction.k * 1023.875f + g_4b9da0.z;
                vector3f basis[3];
                basis[0].i = 0.0f - direction.i;
                basis[0].j = 0.0f - direction.j;
                basis[0].k = 0.0f - direction.k;
                function_11d000(&basis[0], &basis[2]);
                function_30bf0(&basis[2]);
                function_2dba0(tag, basis, 3, 0, i, &position, (color3f const *)((byte const *)g_4686cc + 4), 1.0f - g_4b9f58, 1.0f, 1.0f, false);
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
        real tree_radius = (real)*(long const *)(tree + 0xc) * 8.0f * 0.5f;
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
    point3f child;
    child.x = center->x - child_radius;
    child.y = center->y - child_radius;
    child.z = center->z - child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[0], planes, bounds, &child, child_radius);
    child.x = center->x - child_radius;
    child.y = center->y - child_radius;
    child.z = center->z + child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[1], planes, bounds, &child, child_radius);
    child.x = center->x - child_radius;
    child.y = center->y + child_radius;
    child.z = center->z - child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[2], planes, bounds, &child, child_radius);
    child.x = center->x - child_radius;
    child.y = center->y + child_radius;
    child.z = center->z + child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[3], planes, bounds, &child, child_radius);
    child.x = center->x + child_radius;
    child.y = center->y - child_radius;
    child.z = center->z - child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[4], planes, bounds, &child, child_radius);
    child.x = center->x + child_radius;
    child.y = center->y - child_radius;
    child.z = center->z + child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[5], planes, bounds, &child, child_radius);
    child.x = center->x + child_radius;
    child.y = center->y + child_radius;
    child.z = center->z - child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[6], planes, bounds, &child, child_radius);
    child.x = center->x + child_radius;
    child.y = center->y + child_radius;
    child.z = center->z + child_radius;
    ((c_octree_radius_view_ab const *)tree)->function_3f970( children[7], planes, bounds, &child, child_radius);
}



PRIVATE __forceinline real random_unit_fraction(s_random_globals *random)
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
            motion[6] = (real)((*(real *)(state + 0x20) - (double)*(real *)(state + 0x1c)) *
                random_unit_fraction(random) + *(real *)(state + 0x1c));
            motion[0] = (real)(random_unit_fraction(random) * *(real *)(state + 0x30));
            motion[1] = (real)(random_unit_fraction(random) * *(real *)(state + 0x34));
            motion[2] = (real)(random_unit_fraction(random) * *(real *)(state + 0x38));
            real magnitude = (real)sqrt((double)motion[0] * motion[0] + (double)motion[1] * motion[1] + (double)motion[2] * motion[2]);
            if (!(fabs(magnitude) < 0.0001f))
            {
                real inverse = 1.0f / magnitude;
                motion[0] *= inverse;
                motion[1] *= inverse;
                motion[2] *= inverse;
            }
            *(real *)(entry + 0xc) = (real)(random_unit_fraction(random) *
                (*(real *)(state + 0x28) - (double)*(real *)(state + 0x24)) + *(real *)(state + 0x24));
            motion[7] = 0.0f;
            if (0.0001f > *(real *)(state + 8)) *(real *)(state + 8) = 1.0f;
            if (0.0001f > *(real *)(state + 0xc)) *(real *)(state + 0xc) = 1.0f;
            if (0.0001f > *(real *)(state + 0x10)) *(real *)(state + 0x10) = 1.0f;
            double sample = random_unit_fraction(random);
            *(real *)entry = (real)((sample + sample - 1.0f) * *(real *)(state + 0x10));
            sample = random_unit_fraction(random);
            *(real *)(entry + 4) = (real)((sample + sample - 1.0f) * *(real *)(state + 8));
            sample = random_unit_fraction(random);
            *(real *)(entry + 8) = (real)((sample + sample - 1.0f) * *(real *)(state + 0xc));
            sample = random_unit_fraction(random);
            motion[3] = (real)(sample + sample - 1.0f);
            sample = random_unit_fraction(random);
            motion[4] = (real)(sample + sample - 1.0f);
            sample = random_unit_fraction(random);
            motion[5] = (real)(sample + sample - 1.0f);
        }
        state[0x86] = true;
    }
    *(real *)(state + 0x88) = 1.0f;
    return true;
}



// @retail 0x3e5a0
void function_3e5a0(vector3f *current, vector3f const *target, real step)
{
    real current_size = (real)sqrt((double)current->i * current->i + (double)current->j * current->j + (double)current->k * current->k);
    real a = current_size < 0.0001f ? 0.0001f : current_size > 10.0f ? 10.0f : current_size;
    real target_size = (real)sqrt((double)target->i * target->i + (double)target->j * target->j + (double)target->k * target->k);
    real b = target_size < 0.0001f ? 0.0001f : target_size > 10.0f ? 10.0f : target_size;
    vector3f first, second;
    real inverse = 1.0f / a;
    first.i = current->i * inverse;
    first.j = current->j * inverse;
    first.k = current->k * inverse;
    inverse = 1.0f / b;
    second.i = target->i * inverse;
    second.j = target->j * inverse;
    second.k = target->k * inverse;
    real dot = second.k * first.k + second.j * first.j + second.i * first.i;
    if (fabs(dot) < 0.9999f && a > 0.0001f && b > 0.0001f)
    {
        real angle = (real)(acos(dot) * step);
        vector3f axis;
        axis.i = second.k * first.j - second.j * first.k;
        axis.j = first.k * second.i - second.k * first.i;
        axis.k = second.j * first.i - first.j * second.i;
        function_30bf0(&axis);
        real sine = (real)sin(angle), cosine = (real)cos(angle);
        real projection = (axis.k * first.k + axis.j * first.j + axis.i * first.i) * (1.0f - cosine);
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
    dword initial;
    dword last_seen;
    dword created;
};
struct s_cached_input_list
{
    s_cached_input_entry entries[32];
    long count;
};

struct s_masked_list;
extern s_masked_list *g_547f98;
struct s_light_shape_ab;
bool function_31590(long index, s_light_shape_ab *shape);
bool function_3e9c0(long object_index);
extern s_record_pool *g_4e030c;
void function_4a7c0(s_cached_input_list *state, s_cached_input const *inputs, long count);
long __stdcall function_4ac30(s_cached_input_list *state, s_cached_input *output, long limit, real step);
s_cached_input_list g_5234e8[4];
real g_4b9e20, g_4b9e2c, g_4b9e38, g_4b9e44, g_4b9ed0;

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
            real depth = position.z * g_4b9e38 + position.x * g_4b9e20 + position.y * g_4b9e2c + g_4b9e44;
            if (depth < 0.0f) depth = 0.0f - depth;
            if (!(depth > 0.1f)) depth = 0.1f;
            real score = (g_4b9ed0 / depth) * radius * 2.0f;
            *(real *)&result->data[4] = score;
            bool object_flag = false;
            if (object != NONE)
                object_flag = g_4e0300->data[(object & 0xffff) * 12 + 3] == 2;
            ((byte *)result)[2] = object_flag;
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

// @retail 0x4a7c0
void function_4a7c0(s_cached_input_list *state, s_cached_input const *inputs, long count)
{
    dword volatile used = 0;
    for (long i = 0; i < state->count; ++i)
    {
        for (long j = 0; j < count; ++j)
        {
            if (state->entries[i].input.data[1] == inputs[j].data[1])
            {
                state->entries[i].input = inputs[j];
                state->entries[i].last_seen = g_4ba034;
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
    long n = 0;
    for (long center = 2; n < count - 3; n += 4, center += 4)
    {
        if (state->count < 32 && !(used & (1 << (n + 0))))
        {
            state->entries[state->count].input = inputs[n + 0];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = state->entries[state->count].input.data[3];
            state->entries[state->count].elapsed = 0.0f;
            ++state->count;
            used |= 1 << (n + 0);
        }
        if (state->count < 32 && !(used & (1 << (center - 1))))
        {
            state->entries[state->count].input = inputs[center - 1];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = state->entries[state->count].input.data[3];
            state->entries[state->count].elapsed = 0.0f;
            ++state->count;
            used |= 1 << (center - 1);
        }
        if (state->count < 32 && !(used & (1 << center)))
        {
            state->entries[state->count].input = inputs[center];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = state->entries[state->count].input.data[3];
            state->entries[state->count].elapsed = 0.0f;
            ++state->count;
            used |= 1 << center;
        }
        if (state->count < 32 && !(used & (1 << (center + 1))))
        {
            state->entries[state->count].input = inputs[center + 1];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = state->entries[state->count].input.data[3];
            state->entries[state->count].elapsed = 0.0f;
            ++state->count;
            used |= 1 << (center + 1);
        }
    }
    for (; n < count; ++n)
    {
        if (state->count < 32 && !(used & (1 << n)))
        {
            state->entries[state->count].input = inputs[n];
            state->entries[state->count].last_seen = g_4ba034;
            state->entries[state->count].created = g_4ba034;
            state->entries[state->count].initial = state->entries[state->count].input.data[3];
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
	real &value = *(real *)&entry->initial;
	real maximum = *(real *)&entry->input.data[3];
	if (value < 0.0f) value = 0.0f;
	else if (value > maximum) value = maximum;
}

// @retail 0x4ac30
long __stdcall function_4ac30(s_cached_input_list *state, s_cached_input *output, long limit, real step)
{
	if (state->count > 0 && (!g_510c50 || !((byte *)g_510c50)[5]))
	{
		long requested = limit;
		if (limit > state->count) limit = state->count;
		long first = 0;
		long last = state->count - 1;
		bool found = false;
		long i;
		for (i = 0; i <= last; ++i)
		{
			byte *flags = (byte *)&state->entries[i].input.data[0];
			if (i < last && flags[0] && flags[1])
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
			for (i = first; i <= last; ++i)
			{
				s_cached_input_entry *entry = &state->entries[i];
				if (((byte *)&entry->input.data[0])[2] && entry->created == g_4ba034)
				{
					entry->initial = 0;
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
				entry->initial = 0;
				entry->elapsed = 0.0f - step;
				swap_cached_input(entry, &state->entries[last--]);
			}
		}
		function_4b160(first, (s_4b160_entry *)state->entries, 2, last);
		if (state->count > 0 && requested < state->count)
		{
			for (i = state->count - 1; i >= limit; --i)
			{
				s_cached_input_entry *entry = &state->entries[i];
				if (!((byte *)&entry->input.data[0])[3] && *(real *)&entry->initial > 0.0f)
				{
					entry->elapsed -= step;
					*(real *)&entry->initial += entry->elapsed;
					clamp_cached_input(entry);
				}
				else entry->elapsed = 0.0f;
			}
		}
		for (i = 0; i < limit; ++i)
		{
			s_cached_input_entry *entry = &state->entries[i];
			if (*(real *)&entry->input.data[3] > *(real *)&entry->initial)
			{
				entry->elapsed += step;
				*(real *)&entry->initial += entry->elapsed;
				clamp_cached_input(entry);
			}
			else entry->elapsed = 0.0f;
		}
	}
	for (long i = 0; i < state->count; ++i)
	{
		output[i] = state->entries[i].input;
		output[i].data[3] = state->entries[i].initial;
	}
	return state->count;
}

extern real g_485640;
short g_485600;

PRIVATE __forceinline real motion_random_step(real value, real step)
{
    double random = (double)random_next(&g_4e7408->seed) * (1.0f / 65535.0f);
    real next = (real)((random + random - 1.0f) * step + value);
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
    if (flags & 2)
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
        real phase = *(real *)(state + 0x18 + i * 4);
        if (cycle)
        {
            if (*(real *)(view + 0x140) > phase) phase += 1.0f;
            phase -= *(real *)(view + 0x140);
            *(real *)(view + 0x104 + i * 4) = phase;
            real centered = phase * 2.0f - 1.0f;
            *(real *)(view + 0xec + i * 4) = (1.0f - centered * centered) * *(real *)(state + 0x88 + i * 4);
        }
        else
        {
            *(real *)(view + 0x104 + i * 4) = phase;
            *(real *)(view + 0xec + i * 4) = *(real *)(state + 0x88 + i * 4);
        }
        real distance = phase * range + *(real *)(state + 0x78);
        real radius = (real)(tan((double)g_485640 * 0.5f) * distance);
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
            long index = entry->state_index;
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

// @retail 0x41980
void __stdcall function_41980(short type, long object_index)
{
    struct s_attachment_scratch_r16 { long count; color3f color; real amount; s_object_marker markers[65]; };
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
                scratch_r16.amount = function_be6d0(object_index, i);
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
                            function_2dba0(entry->tag, &matrix->forward, 0, object_index & 0xffff, added, &matrix->position, &scratch_r16.color, 1.0f, scratch_r16.amount, scale, function_3e9c0(object_index));
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
                            &matrix->forward, scale, scratch_r16.amount, &matrix->up);
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
real g_4b9dcc;

PRIVATE inline real object_view_distance(point3f const *position)
{
    double x = (double)position->x - g_4b9da0.x;
    double y = (double)position->y - g_4b9da0.y;
    double z = (double)position->z - g_4b9da0.z;
    return (real)(sqrt(z * z + y * y + x * x) * g_4b9dcc);
}

// @retail 0x3d7f0
bool __stdcall function_3d7f0(long object_index, real *out_alpha, bool *out_special)
{
    s_scalar_object_header *header = &((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    volatile long kind = *(signed char *)(object + 0xaa);
    real alpha = 0.0f;
    bool result = false;
    if (!(object[6] & 1))
    {
        byte opacity, second;
        switch (*(short *)(definition + 0x18))
        {
        case 0:
            if (kind == 0 || kind == 1)
            {
                point3f position;
                long region;
                function_30c60(object_index, &position, &region);
                function_4baf0(object_index, object_view_distance(&position), &opacity, &second);
                alpha = opacity * 0.003921568859368563f;
            }
            break;
        case 2:
        {
            point3f position = *(point3f *)(object + 0x30);
            function_4baf0(object_index, object_view_distance(&position), &opacity, &second);
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
    }
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
    long volatile selected = 0;
    long view = g_4b9ed4 < 0 ? 0 : g_4b9ed4 > 3 ? 3 : g_4b9ed4;
    long volatile count = *(short const *)(list + 4);
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
                real depth = g_4b9e38 * position.z + g_4b9e20 * position.x + g_4b9e2c * position.y + g_4b9e44;
                if (depth < 0.0f) depth = 0.0f - depth;
                if (!(depth > 0.1f)) depth = 0.1f;
                *(real *)&entry->data[4] = (g_4b9ed0 / depth) * radius * 2.0f;
                ((byte *)entry)[0] = function_3e9c0(object_index);
                ((byte *)entry)[2] = 0;
                ((byte *)entry)[1] = 0;
                ++selected;
            }
        }
    }
    s_cached_input_list *state = &g_5246f8[view];
    function_4a7c0(state, output, selected);
    return function_4ac30(state, output, 4, 0.005f);
}

struct s_sort_record;
typedef bool (__stdcall *t_41640_fill)(long, void *, long, long, long, void *, s_sort_record *);
typedef void (__stdcall *t_41640_draw)(long, long, long, long, long, dword, byte *);
bool __stdcall function_4de20(long, long, long, long, long, long, s_sort_record *);
bool __stdcall function_4dfa0(long, long, long, long, long, long, s_sort_record *);
bool __stdcall function_4e0d0(long, long, long, long, long, long, s_sort_record *);
void __stdcall function_4e260(long, long, long, long, long, dword, byte *);
void __stdcall function_4ebd0(long, long, long, long, long, dword, byte *);
void __stdcall function_4ede0(long, long, long, long, long, dword, byte *);
void __stdcall function_4efa0(long, long, long, long, long, dword, byte *);
void function_40e30(short group, long tag, real distance, long level, word kind,
    dword and_mask, dword or_mask, t_41640_fill fill, dword value, void *context);
point3f *function_3f220(dword a, dword b, dword c, point3f *out);
struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index,
    point3f const *center, real radius, bool *contained);
real distance3d(point3f const *a, point3f const *b);
real g_525980[3];

// @retail 0x41640
void function_41640(void)
{
    t_41640_fill fill[6] = { (t_41640_fill)function_4de20, (t_41640_fill)function_4dfa0,
        (t_41640_fill)function_4dfa0, (t_41640_fill)function_4e0d0,
        (t_41640_fill)function_4e0d0, (t_41640_fill)function_4e0d0 };
    t_41640_draw draw[6] = { function_4e260, function_4ebd0, function_4ebd0,
        function_4ede0, function_4efa0, function_4efa0 };
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
            long definition_index = (signed char)record[0];
            if (definition_index < 0 || definition_index >= *(long *)(definitions + 0x378)
                || *(short *)(record + 0xa) <= 0 || !function_12de70(block, 3)) continue;
            long tag = (*(long **)(definitions + 0x37c))[(signed char)record[0] * 2 + 1];
            if (tag == NONE) continue;
            byte *definition = g_4e3b44[tag & 0xffff].bytes;
            long part_index = record[1];
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
            point3f position;
            function_3f220(packed & 0x3ff, (packed >> 10) & 0x3ff, packed >> 20, &position);
            position.x = node->center.x - 4.0f + position.x;
            position.y = node->center.y - 4.0f + position.y;
            position.z = node->center.z - 4.0f + position.z;
            real radius = record[3] * 0.0313725508749485f;
            long section = *(short *)(record + 4);
            if (section == NONE || !(g_547f88.flags[section >> 5] & (1 << (section & 31)))) continue;
            long frustum = ((signed char *)g_547f88.indices)[section];
            bool contained = false;
            if (function_165010((s_frustum_set_view *)g_547f88.context, frustum,
                    &position, radius, &contained))
            {
                long distance_class = entry[9];
                point3f camera = g_4b9da0;
                if (g_525980[distance_class] > distance3d(&camera, &position) - radius)
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
extern short_rect_pair g_485a8a;

PRIVATE __forceinline real clamp_4a1e0(real value, real lower, real upper)
{
    return value < lower ? lower : value > upper ? upper : value;
}

void matrix4x3_from_forward_and_up(transform4x3f *out, vector3f const *forward, vector3f const *up);
extern vector3f *g_4687bc;
struct s_collision_result_1697c0
{
    byte unknown00[8];
    point3f point;
    byte unknown14[0x24 - 0x14];
    short unknown24;
    byte unknown26[0x4c - 0x26];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

// @retail 0x32ce0
void function_32ce0(long object_index, real opacity)
{
    (void)&opacity;
    s_collision_result_1697c0 collision;
    collision.unknown24 = NONE;
    byte const *object = ((s_scalar_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    if (opacity > 0.05f && function_1697c0(0x4800001, (point3f const *)(object + 0x30),
            g_4687bc, NONE, NONE, &collision))
    {
        real radius = *(real const *)(object + 0x3c) * 0.35f;
        transform4x3f matrix;
        matrix4x3_from_forward_and_up(&matrix, (vector3f const *)(object + 0x70),
            (vector3f const *)(object + 0x7c));
        point3f corners[4];
        for (long i = 0; i < 4; ++i)
        {
            real x = i < 2 ? 0.0f - radius : radius;
            real y = i == 0 || i == 3 ? 0.0f - radius : radius;
            real z = 0.0f;
            if (matrix.scale != 1.0f)
            {
                x *= matrix.scale;
                y *= matrix.scale;
                z *= matrix.scale;
            }
            corners[i].x = matrix.forward.i * x + matrix.up.i * z + matrix.left.i * y + matrix.position.x + collision.point.x;
            corners[i].y = matrix.up.j * z + matrix.left.j * y + matrix.forward.j * x + matrix.position.y + collision.point.y;
            corners[i].z = matrix.up.k * z + matrix.left.k * y + matrix.forward.k * x + matrix.position.z + collision.point.z + 0.001f;
        }
        real fraction = *(real const *)(collision.unknown00 + 4);
        real alpha = (1.0f - fraction) * opacity;
        D3DDevice_Begin(D3DPT_QUADLIST);
        D3DDevice_SetVertexData4f(9, 1.0f, 1.0f, 1.0f, alpha);
        D3DDevice_SetVertexData2f(3, 0.0f, 0.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[0].x, corners[0].y, corners[0].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 0.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[1].x, corners[1].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 1.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[2].x, corners[2].y, corners[3].z, 1.0f);
        D3DDevice_SetVertexData2f(3, 1.0f, 0.0f);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, corners[3].x, corners[3].y, corners[3].z, 1.0f);
        D3DDevice_End();
    }
}

// @retail 0x4a1e0
void function_4a1e0(s_view_source *source, s_view_camera *camera, long resource_index,
    s_view_flags *flags, real unused_scale, bool enabled, real scale)
{
    byte *resource = 0;
    if (resource_index != NONE)
        resource = g_4e030c->data + (resource_index & 0xffff) * 0x110;
    byte *context = (byte *)g_547f88.context;
    g_4858bc = true;
    g_4858c4 = (byte *)flags;
    g_4858cc = resource && *(short *)(resource + 0x54) != NONE ? *(long *)(resource + 0x4c) : NONE;
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
    vector3f first = g_485944;
    vector3f second = g_485950;
    real input_scale = g_48595c;
    long mode = g_485a2c;
    vector4f old_first, old_second;
    memcpy(&old_first, g_485a0c, sizeof(old_first));
    memcpy(&old_second, g_485a0c + 4, sizeof(old_second));
    s_view_result result;
    memcpy(&result, g_48597c, sizeof(result));
    function_0497a0(source, &result, camera, flags, &first, &second, input_scale,
        &mode, &old_first, &old_second);
    memcpy(g_48597c, &result, sizeof(result));
    memcpy(g_485a0c, &old_first, sizeof(old_first));
    memcpy(g_485a0c + 4, &old_second, sizeof(old_second));
    g_485a2c = mode;
    s_bounds3d bounds;
    bool clipped = false;
    if (resource && function_1650a0((s_view const *)(context + 4),
        (point3f const *)(resource + 0x28), &bounds, *(real *)(resource + 0x34)))
    {
        real x0 = *(real *)(context + 0xa4), x1 = *(real *)(context + 0xa8);
        real y0 = *(real *)(context + 0xac), y1 = *(real *)(context + 0xb0);
        if (x0 != x1 && y0 != y1 && x1 >= bounds.x0 && bounds.x1 >= x0
            && y1 >= bounds.y0 && bounds.y1 >= y0)
        {
            real left = clamp_4a1e0(bounds.x0, x0, x1);
            real right = clamp_4a1e0(bounds.x1, x0, x1);
            real top = clamp_4a1e0(0.0f - bounds.y1, y0, y1);
            real bottom = clamp_4a1e0(0.0f - bounds.y0, y0, y1);
            g_485964[0] = (g_485a8a.a.v3 - g_485a8a.a.v1) * (left - x0) / (x1 - x0) + g_485a8a.a.v1;
            g_485964[1] = (g_485a8a.a.v3 - g_485a8a.a.v1) * (right - x0) / (x1 - x0) + g_485a8a.a.v1;
            g_485964[2] = (g_485a8a.a.v2 - g_485a8a.a.v0) * (top - y0) / (y1 - y0) + g_485a8a.a.v0;
            g_485964[3] = (g_485a8a.a.v2 - g_485a8a.a.v0) * (bottom - y0) / (y1 - y0) + g_485a8a.a.v0;
            g_485964[4] = bounds.z0;
            g_485964[5] = bounds.z1;
            clipped = true;
        }
    }
    if (!clipped)
    {
        g_485964[0] = g_485a8a.a.v1;
        g_485964[1] = g_485a8a.a.v3;
        g_485964[2] = g_485a8a.a.v0;
        g_485964[3] = g_485a8a.a.v2;
        g_485964[4] = 0.0f - *(real *)(context + 0x8c);
        g_485964[5] = 0.0f - *(real *)(context + 0x74);
    }
    g_4858d0 = resource != 0;
    if (resource) memcpy(g_485934, resource + 0x18, sizeof(g_485934));
    g_485a44 = enabled;
    function_3a550();
}
