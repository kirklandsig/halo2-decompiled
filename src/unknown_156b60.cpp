#include "unknown_11c920.h"
#include "frame_storage.h"
#include "globals.h"
#include "effects.h"
#include <xtl.h>
#include <d3d8.h>

// @flags /O2 /arch:SSE /Gr
#include "function_context_storage.h"
#include "shader_storage.h"
#include "unknown_0259d0.h"

struct s_render_entry_156b60
{
	byte *definition;
	point3f position;
	vector3f forward;
	real value;
};

struct s_material_156b60
{
	long index;
	byte kind;
	byte mode;
	word value6;
	short count;
	short unknowna;
	long valuec;
	byte *definition;
	void *state;
};

s_render_entry_156b60 g_4e92e8[64];
long g_4e92e0;

struct s_tag_data { long size; byte *address; };
struct s_shader_cache;
struct s_cache_record;
struct s_sort_record;

extern dword g_4b8348;
byte const g_43f78f[] = {43, 0, 11, 255};

real function_13b390(void const *function, real input, real range);
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color4f(color4f const *color);
void function_1bd50(void *state);
void function_1bbf0(long tag, long first, long second, long third, long fourth, real scale);
void function_1cf50();
void function_1c590(s_shader_cache *state, long tag, long index);
void function_1c6b0(void *state);
void __stdcall function_1c710(void *state);
void function_23e460();
void function_23ecd0();
void function_23ed30(point3f *a, point3f *b, real c, real d, dword color, real e, real *f, real *h, real *g);
s_cache_record *function_1e2d0();
matrix3x3 *function_142da0(real yaw, real pitch, real roll, matrix3x3 *out);
vector3f *function_143070(matrix3x3 const *m, vector3f const *v, vector3f *out);
typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);
void function_40e30(short group, long tag, real distance, long level, short kind,
	dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context);

void __stdcall function_157020(byte *definition, point3f const *position, vector3f const *forward,
	real value, long row, long column, long element, long entry);

static __forceinline long const *material_reference_156b60(long tag)
{
	dword group = *(dword *)((byte *)g_4e3b44 + (short)tag * 16);
	if (group == 'PRTM' || group == 'prt3')
		return function_137bd0(tag)->function_x947334();
	byte *definition = *(byte **)((byte *)g_4e3b44 + (tag & 0xffff) * 16 + 8);
	return *(long const **)(definition + 0x24);
}

// @retail 0x156fe0
void __stdcall function_156fe0(long unused0, long unused1, long row, long column,
	long element, long entry, void *unused6)
{
	s_render_entry_156b60 *render = &g_4e92e8[entry];
	function_157020(render->definition, &render->position, &render->forward, render->value,
		row, column, element, entry);
}

// @retail 0x156ed0
long __stdcall function_156ed0(long tag, long unused1, long row, long column,
	long element, long unused5, s_material_156b60 *material)
{
	long model = *material_reference_156b60(tag);
	byte *local_0f244f = *(byte **)((byte *)g_4e3b44 + (model & 0xffff) * 16 + 8);
	byte *tables = *(byte **)(local_0f244f + 0x5c);
	word *rows = *(word **)(tables + 4);
	word *columns = *(word **)(tables + 0xc);
	byte *elements = *(byte **)(tables + 0x14);
	long column_index = (rows[row * 5] & 0x1ff) + column;
	long element_index = (columns[column_index] & 0x1ff) + element;
	long selected = *(long *)(elements + element_index * 10 + 4);
	byte *selected_definition = *(byte **)((byte *)g_4e3b44 + (selected & 0xffff) * 16 + 8);
	byte *definition = *(byte **)(*(byte **)(selected_definition + 0x20) + 4);
	material->definition = definition;
	material->mode = 3;
	material->kind = 0;
	material->count = 8;
	material->state = &g_4e9268;
	material->value6 = NONE;
	material->valuec = NONE;
	long dependency = *(long *)(material->definition + 0x100);
	if (dependency != NONE)
	{
		byte *dependency_definition = *(byte **)((byte *)g_4e3b44 + (dependency & 0xffff) * 16 + 8);
		if (*(dword *)(dependency_definition + 4) > 8)
		{
			byte *data = *(byte **)(dependency_definition + 8);
			if ((dword)(*(long *)(data + 0xec) >> 4) > 0)
				return 1;
		}
	}
	return 0;
}

// @retail 0x156e10
void __stdcall function_156e10(long const *entry)
{
	s_render_entry_156b60 *render = &g_4e92e8[*entry];
	long model = *material_reference_156b60(*(long *)(render->definition + 4));
	byte *definition = *(byte **)((byte *)g_4e3b44 + (model & 0xffff) * 16 + 8);
	byte *tables = *(byte **)(definition + 0x5c);
	word *rows = *(word **)(tables + 4);
	word *columns = *(word **)(tables + 0xc);
	long count = columns[(rows[0] & 0x1ff) + 15] >> 9;
	for (long i = 0; i < count; ++i)
		function_157020(render->definition, &render->position, &render->forward,
			render->value, 0, 15, i, *entry);
}

PRIVATE real render_range_156b60(s_tag_data const *curve, real value)
{
	byte *data = curve->address;
	if (!(data[1] & 0xf0))
	{
		real lower = *(real *)(data + 4);
		real upper = *(real *)(data + 8);
		real fraction = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
		return (upper - lower) * fraction + lower;
	}
	return value;
}

PRIVATE real render_curve_156b60(s_tag_data const *curve, real input)
{
	return render_range_156b60(curve, function_13b390(curve, input, 0.0f));
}

PRIVATE void render_clamp_156b60(real *value)
{
	if (*value < 0.0f) *value = 0.0f;
	else if (*value > 1.0f) *value = 1.0f;
}

// @retail 0x157020
void __stdcall function_157020(byte *definition, point3f const *position, vector3f const *forward,
	real value, long row, long column, long element, long entry)
{
	long model = *material_reference_156b60(*(long *)(definition + 4));
	byte *local_0f244f = *(byte **)((byte *)g_4e3b44 + (model & 0xffff) * 16 + 8);
	byte *tables = *(byte **)(local_0f244f + 0x5c);
	word *rows = *(word **)(tables + 4);
	word *columns = *(word **)(tables + 0xc);
	byte *elements = *(byte **)(tables + 0x14);
	long column_index = (rows[row * 5] & 0x1ff) + column;
	long element_index = (columns[column_index] & 0x1ff) + element;
	long selected = *(long *)(elements + element_index * 10 + 4);
	byte *selected_definition = *(byte **)((byte *)g_4e3b44 + (selected & 0xffff) * 16 + 8);
	byte *material = *(byte **)(*(byte **)(selected_definition + 0x20) + 4);
	s_tag_data const *width_curve = (s_tag_data *)(definition + 0x1c);
	real width = function_13b390(width_curve, value, 0.0f);
	s_tag_data const *length_curve = (s_tag_data *)(definition + 0x24);
	real length = function_13b390(length_curve, value, 0.0f);
	width = render_range_156b60(width_curve, width);
	length = render_range_156b60(length_curve, length);
	s_tag_data *color_curve = (s_tag_data *)(definition + 0xc);
	real color_value = function_13b390(color_curve, value, 0.0f);
	color4f color;
	if (color_curve->address && color_curve->size > 0)
		unpack_color3f(function_13bc00(color_curve, color_value), (color3f *)&color.red);
	else
		*(color3f *)&color.red = *(color3f const *)&g_4686cc->red;
	color.alpha = render_curve_156b60((s_tag_data *)(definition + 0x14), value);
	render_clamp_156b60(&color.alpha);
	render_clamp_156b60(&color.red);
	render_clamp_156b60(&color.green);
	render_clamp_156b60(&color.blue);
	real attributes[8] = {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, width * -0.5f, length, width};
	g_4b8348 = 0;
	D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE, 0);
	real matrix[12] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
	D3DDevice_SetVertexShaderConstant(0x4e, matrix, 3);
	g_4e9268 = entry;
	function_1bd50(&g_4e9268);
	function_1bbf0(*(long *)(definition + 4), row, column, element, 0, 10000.0f);
	function_1cf50();
	function_1c590((s_shader_cache *)g_51f0f0, *(long *)(material + 0x100), 8);
	function_1c6b0(g_51f0f0);
	*(byte const **)(g_51f0f0 + 0x8c) = g_43f78f;
	if (*(byte const **)(g_51f0f0 + 0xcc) != g_43f78f)
		g_51f0f0[0x20c] = 1;
	function_1c710(g_51f0f0);
	function_23e460();
	function_23ecd0();
	function_23ed30((point3f *)position, (point3f *)forward, 1.0f, 0.0f, pack_color4f(&color), 0.0f,
		attributes, attributes, attributes + 4);
}

// @retail 0x156b60
void function_156b60(s_effect_beam *beam, real progress, transform4x3f const *matrix)
{
	byte *definition = (byte *)beam;
	if (*(long *)(definition + 4) == NONE || g_4e92e0 >= 64)
		return;
	long model = *material_reference_156b60(*(long *)(definition + 4));
	byte *local_0f244f = *(byte **)((byte *)g_4e3b44 + (model & 0xffff) * 16 + 8);
	byte *tables = *(byte **)(local_0f244f + 0x5c);
	dword flags = *(dword *)(*(byte **)(tables + 4) + 2);
	s_render_entry_156b60 *entry = &g_4e92e8[g_4e92e0];
	real yaw = render_curve_156b60((s_tag_data *)(definition + 0x2c), progress) * 0.01745329238474369f;
	real pitch = render_curve_156b60((s_tag_data *)(definition + 0x34), progress) * 0.01745329238474369f;
	entry->definition = definition;
	entry->position = matrix->position;
	entry->forward = matrix->forward;
	entry->value = progress;
	if (!(fabs(pitch) < 0.0001f && fabs(yaw) < 0.0001f))
	{
		matrix3x3 rotation;
		function_142da0(yaw, pitch, 0.0f, &rotation);
		function_143070(&rotation, &entry->forward, &entry->forward);
	}
	if (flags & 0x8000)
	{
		byte *record = (byte *)function_1e2d0();
		if (record)
		{
			*(long *)(record + 0x24) = g_4e92e0;
			vector3f delta;

			vector3d_from_points3d(&g_485618.position, &entry->position, &delta);
			*(long *)record = 3;
			*(long *)(record + 8) = *(long *)(definition + 4);
			*(void (__stdcall **)(long const *))(record + 0x20) = function_156e10;
			*(real *)(record + 4) = 0.0f - dot3f(&delta, &g_485618.forward);
			*(point3f *)(record + 0x64) = entry->position;
		}
	}
	if (flags & 0xffff7fff)
		function_40e30(0, *(long *)(definition + 4), 10000.0f, 5, NONE, 0xffffffff, 0,
			(t_record_fill)function_156ed0, (dword)function_156fe0, (void *)g_4e92e0);
	++g_4e92e0;
}

s_function_context_storage g_function_context_storage;
