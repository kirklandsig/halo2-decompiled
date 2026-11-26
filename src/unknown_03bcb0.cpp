#include "render_source_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
// @flags /O2 /arch:SSE /Ob1 /Gr
#include "material_records_storage.h"
#include "material_shader_storage.h"
#include "material_constants_storage.h"
#include "shader_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_03BCB0.CPP: predicting one bitmap's texture (unknown_03bcb0.h);
   its own /Ob1 file because retail calls it out of line from 0x16e5e0 */

#include "unknown_11c920.h"
#include "unknown_03bcb0.h"
#include <xtl.h>
#include "globals.h"
#include "geometry_cache.h"
#include "unknown_044e90.h"

void function_3c650(byte const *state);
void function_3d000(real const *state);

// @retail 0x3bf00
bool function_3bf00(byte const *state)
{
	function_3c650(state);
	function_3d000((real const *)state);
	return true;
}

extern byte g_4670bc;
extern long g_4858b8;
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
void __stdcall function_352e0(long target, bool multiple);
void function_14bc0(short index, short element, bool use_depth);
byte g_509430;

// @retail 0x3bea0
bool function_3bea0(void)
{
    if (!g_509430)
    {
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
        if (*(long *)((byte *)g_4e0348 + 0x214) > 0)
        {
            long target = g_4858b8;
            function_352e0(22, true);
            function_14bc0((short)target, 0, true);
        }
        g_509430 = true;
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
    }
    return true;
}

#include <string.h>
struct s_frustum_1648d0;
struct s_camera_163db0;
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result);
void function_141590(transform4x3f const *in, transform4x3f *out);
real function_30bf0(vector3f *vector);

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

struct s_planar_camera_source
{
	point3f origin;
	vector3f normal;
	vector3f horizontal;
	byte unknown24[0x40 - 0x24];
	real offset;
	real depth;
};

PRIVATE inline void matrix_cross(vector3f const *a, vector3f const *b, vector3f *out);

// Matched in a full build with stand-in SDK headers (was 4): the left axis is stored into the state's matrix
// directly and its address passed to 0x30bf0, with no `left` pointer local; the two products of the inlined
// matrix_cross then load up.j / up.k first as retail does.
// Before, 4 fast (was 7): only the operand order of two products in the inlined matrix_cross differs (retail loads
// up.j then multiplies by left.i, 0x4420e/0x4421c, and loads up.k then multiplies by left.i, 0x44221/0x44226; ours
// loads left.i first in both). Swapping the operands in matrix_cross (k and j terms, four spellings) gives identical
// bytes: the compiler canonicalizes them; 0x3c9a0 shares the helper and stays MATCH.
// Permuter (25 min on r643): best 6, only with volatile stores to the state's count and valid flag; not kept.
// b14K (11-17) walk against retail, fast build on r847: the statements agree with retail one for one (stores, calls,
// constants, corner copy order). Two source changes took 7 to 4: the cross product now takes the field addresses
// directly instead of the old `vector_reference` pointer-to-pointer (13 with the plain `left`, 11 with only the left
// field direct, 5 with both fields direct), and the corner stores go through a local `corners` pointer, which put the
// last corner store (0x1b8) before the `lea edi`/`push esi` of the 0x163db0 call as in retail (5 -> 4). Tried and
// dropped (no change or worse): all 64 operand spellings of the six products (5), every statement order of the
// three terms (no better, most also break 0x3c9a0), reference parameters, swapped helper parameters, `count`/`valid`
// stores moved or swapped (9-25), a frustum local (7). Callers 0x3f2c0 MATCH and 0x3f660 97 unchanged.
// @retail 0x441b0
void function_441b0(s_planar_camera_source const *source, s_planar_camera *state, byte const *view)
{
	memset(state, 0, sizeof(*state));
	state->disabled = false;
	state->matrix.left = source->horizontal;
	function_30bf0(&state->matrix.left);
	vector3f *up = &state->matrix.up;
	up->i = 0.0f - source->normal.i;
	up->j = 0.0f - source->normal.j;
	up->k = 0.0f - source->normal.k;
	function_30bf0(up);
	matrix_cross(&state->matrix.left, &state->matrix.up, &state->matrix.forward);
	function_30bf0(&state->matrix.forward);
	state->matrix.position = source->origin;
	state->matrix.scale = 1.0f;
	function_141590(&state->matrix, &state->inverse);
	state->has_plane = true;
	state->offset = source->offset;
	vector3f const &normal = source->normal;
	state->plane.n = normal;
	state->plane.d = normal.k * source->origin.z + normal.j * source->origin.y + normal.i * source->origin.x + source->offset;
	*(bool *)state->unknown84 = true;
	state->depth = source->depth;
	point2f *corners = state->corners;
	box2f const *rectangle = (box2f const *)(view + 0x68);
	corners[0].x = rectangle->x0;
	corners[0].y = rectangle->y0;
	corners[1].x = rectangle->x1;
	corners[1].y = rectangle->y0;
	corners[2].x = rectangle->x1;
	corners[2].y = rectangle->y1;
	corners[3].x = rectangle->x0;
	corners[3].y = rectangle->y1;
	state->count = 4;
	state->valid = true;
	function_163db0((s_camera_163db0 *)state, rectangle, 0, (s_frustum_1648d0 *)state->frustum);
}

struct s_3c9a0_matrix
{
	real scale;
	vector3f forward, left, up;
	point3f position;
};

real function_30bf0(vector3f *vector);

PRIVATE inline void matrix_cross(vector3f const *a, vector3f const *b, vector3f *out)
{
	real k = b->j * a->i - b->i * a->j;
	real j = a->k * b->i - b->k * a->i;
	real i = b->k * a->j - a->k * b->j;
	out->i = i;
	out->j = j;
	out->k = k;
}

// @retail 0x3c9a0
void function_3c9a0(vector3f const *forward, vector3f const *up, s_3c9a0_matrix *matrix)
{
	matrix->scale = 1.0f;
	matrix->up = *up;
	matrix_cross(up, forward, &matrix->left);
	function_30bf0(&matrix->left);
	matrix_cross(&matrix->left, up, &matrix->forward);
	function_30bf0(&matrix->forward);
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
}

struct s_bitmap_view;
D3DTexture *fetch_bitmap_texture(s_bitmap_view *, dword, real);

// @retail 0x3bcb0
D3DTexture *function_3bcb0(s_bitmap_data *bitmap)
{
	return fetch_bitmap_texture((s_bitmap_view *)bitmap, 0xe, 0.0f);
}

struct s_sort_record
{
	word index;
	word key;
	byte unknown04;
	byte group;
	word subkey;
	word value08;
	word unknown0a;
	dword value0c;
	dword value10;
	dword unknown14;
};

struct s_record_source
{
	bool (__stdcall *fill)(long, void *, long, long, long, void *, s_sort_record *);
	dword unknown04;
	long key;
	void *context;
	dword flags;
	byte value14;
	byte value15;
	word unknown16;
};

class c_type_4e7709;
c_type_4e7709 *function_137bd0(long tag_index);
extern c_type_4e7709 g_479868;
extern c_type_4e7709 g_479874;

class c_record_reference_view
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual void slot7() = 0;
	virtual void slot8() = 0;
	virtual void slot9() = 0;
	virtual long *reference() = 0;
	virtual void slot11() = 0;
	virtual void slot12() = 0;
	virtual void slot13() = 0;
	virtual void slot14() = 0;
	virtual void slot15() = 0;
	virtual void slot16() = 0;
	virtual void slot17() = 0;
	virtual void slot18() = 0;
	virtual void slot19() = 0;
	virtual void initialize(long tag_index) = 0;
};

PRIVATE __forceinline byte *record_format_groups(long tag_index)
{
	long *reference;
	dword kind = *(dword *)g_4e3b44[(short)tag_index].unknown00;
	if (kind == 0x5052544d || kind == 0x70727433)
	{
		c_record_reference_view *provider = (c_record_reference_view *)function_137bd0(tag_index);
		reference = provider->reference();
	}
	else
		reference = *(long **)(g_4e3b44[tag_index & 0xffff].bytes + 0x24);
	return *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
}

long function_30cd0(bool a, bool b);
long function_30d10(bool a, bool b, bool c);
long function_30da0(bool a, bool b);
long function_30e00(bool a);
bool function_0226d0(void);

// @retail 0x15d70
long function_15d70(long tag, long stage, long pass, bool first, bool second)
{
	byte *definition = g_4e3b44[tag & 0xffff].bytes;
	long result = 0;
	byte *groups = record_format_groups(tag);
	long group = *(word *)(*(byte **)(groups + 4) + pass * 10) & 0x1ff;
	long count = (*(word **)(groups + 0xc))[group + stage] >> 9;
	if (count > 0)
	{
		switch (stage)
		{
		case 10: result = function_30cd0(second, first); break;
		case 11: result = function_30d10(*(word *)(definition + 0x3e) != 0, true, second); break;
		case 12: result = function_30da0(*(word *)(definition + 0x3e) != 0, true); break;
		case 13: result = function_30e00(*(word *)(definition + 0x3e) != 0); break;
		case 3: result = function_0226d0() ? NONE : 3; break;
		}
	}
	else
		result = NONE;
	return result;
}

__declspec(noinline) long function_4cbb0(long tag_index, dword block_index);

PRIVATE __forceinline byte *record_material(long tag, long pass, long stage, long entry)
{
	byte *groups = record_format_groups(tag);
	long group = *(word *)(*(byte **)(groups + 4) + pass * 10) & 0x1ff;
	long first = (*(word **)(groups + 0xc))[group + stage] & 0x1ff;
	long material = *(long *)(*(byte **)(groups + 0x14) + (first + entry) * 10 + 4);
	return *(byte **)(*(byte **)(g_4e3b44[material & 0xffff].bytes + 0x20) + 4);
}

// @retail 0x4dfa0
bool __stdcall function_4dfa0(long tag, long context, long pass, long stage, long entry, long handle, s_sort_record *out)
{
	(void)&tag; (void)&context; (void)&pass; (void)&stage;
	(void)&entry; (void)&handle; (void)&out;
	byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
	byte *record = *(byte **)(table + 0x1c) + (((dword)handle >> 8) & 0x3fffff) * 24;
	s_geometry_block_info *block = (s_geometry_block_info *)(*(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c);
	if (function_12de70(block, 3) && *(word *)(record + 0xe) > 0)
	{
		out->value0c = handle;
		out->unknown04 = 0;
		out->group = 4;
		out->subkey = 0xffff;
		byte *material = record_material(tag, pass, stage, entry);
		out->value10 = (dword)material;
		out->value08 = 0;
		out->unknown14 = 0;
		// The helper returns 0 or 1, so its low byte is the bool result.
		union { long value; bool ready; } result;
		result.value = function_4cbb0(*(long *)((byte *)out->value10 + 0x100), 0);
		return result.ready;
	}
	return false;
}

// @retail 0x4de20
bool __stdcall function_4de20(long tag, long context, long pass, long stage, long entry, long handle, s_sort_record *out)
{
    bool final_value;
	(void)&tag; (void)&context; (void)&pass; (void)&stage;
	(void)&entry; (void)&handle; (void)&out;
	out->unknown04 = 0;
	if (stage == 1) { final_value = false; goto complete; }
	byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
	byte *record = *(byte **)(table + 0x1c) + (((dword)handle >> 8) & 0x3fffff) * 24;
	long record_tag = (*(long **)((byte *)g_4e0350 + 0x37c))[(signed char)record[0] * 2 + 1];
	byte *definition = g_4e3b44[record_tag & 0xffff].bytes;
	s_geometry_block_info *block = (s_geometry_block_info *)(*(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c);
	if (function_12de70((s_geometry_block_info *)(definition + 0x38), 3) && function_12de70(block, 3))
	{
		dword index = out->unknown04;
		out->group = 1;
		out->subkey = 0xffff;
		out->value0c = handle;
		byte *material = record_material(tag, pass, stage, entry) + index * 0x132;
		out->value10 = (dword)material;
		out->value08 = 1;
		out->unknown14 = 0;
		union { long value; bool ready; } availability;
		availability.value = function_4cbb0(*(long *)((byte *)out->value10 + 0x100), 1);
		final_value = availability.ready;
		goto complete;
	}
	{ final_value = false; goto complete; }
complete:
    return final_value;
}

static __forceinline byte *record_format_groups_4e0d0(long tag_index)
{
	long *reference;
	dword const *kind_pointer = (dword const *)g_4e3b44[(short)tag_index].unknown00;
	dword const *const &kept_kind = kind_pointer;
	dword kind = *kept_kind;
	if (kind == 0x5052544d || kind == 0x70727433)
	{
		c_record_reference_view *provider = (c_record_reference_view *)function_137bd0(tag_index);
		reference = provider->reference();
	}
	else
	{
		byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
		byte *const &kept_definition = definition;
		reference = *(long **)(kept_definition + 0x24);
	}
	byte *format = g_4e3b44[*reference & 0xffff].bytes;
	byte *const &kept_format = format;
	return *(byte **)(kept_format + 0x5c);
}

static __forceinline byte *record_material_4e0d0(long tag, long pass, long stage, long entry)
{
	byte *groups = record_format_groups_4e0d0(tag);
	word const *group_pointer = (word const *)(*(byte **)(groups + 4) + pass * 10);
	word const *const &kept_group = group_pointer;
	long group = *kept_group & 0x1ff;
	word const *first_pointer = &(*(word **)(groups + 0xc))[group + stage];
	word const *const &kept_first = first_pointer;
	long first = *kept_first & 0x1ff;
	byte const *shader_entry = *(byte **)(groups + 0x14) + (first + entry) * 10;
	byte const *const &kept_entry = shader_entry;
	long material = *(long const *)(kept_entry + 4);
	byte *shader = g_4e3b44[material & 0xffff].bytes;
	byte *const &kept_shader = shader;
	byte *block = *(byte **)(kept_shader + 0x20);
	byte *const &kept_block = block;
	return *(byte **)(kept_block + 4);
}

// Matched in a full build with stand-in SDK headers (was 88): private inline copies of the two record helpers read
// each table pointer through a reference-bound local (kind, group word, first word, shader entry, definition,
// format, shader, block), which gives every `.bytes` load the register of its own index as in retail; the pair
// pointer is read the same way, and the 0x4dfa0 tail returns the union's ready byte directly.
// Before:
// score 88 (was 88): logic walked block by block against retail, no logic or behaviour difference. Retail returns 4cbb0's
// al directly (call 4cbb0; pop; ret) and `xor al,al` on the false path; the draft's `result` byte costs a push ebx/setne/test.
// The 4dfa0 union form (availability.ready, return false) gives that tail (381 bytes) but scores 109, because retail also
// keeps four pointers the draft folds: lea eax,[eax+edx*8] + [eax+4] for the g_4e0350+0x37c pair, `add eax,edi; mov eax,[eax]`
// for the tag instance, and lea eax,[eax+edx*2] before xor edx,edx/mov dx,[eax] (group), the movzx (first) and the dead lea
// (shader entry) in the record_material chain; +12 bytes. Tried: named pointers (pair, group, first, shader entry) as
// plain pointers, as a local struct pair and as bitfield structs, hoisted before `block` (105); dropping the (void)& lines (no change).
// Permuter (25 min on r643): best 82, only with volatile stores and reads and record_material's arguments
// reordered (a behaviour change); not kept.
// @retail 0x4e0d0
bool __stdcall function_4e0d0(long tag, long context, long pass, long stage, long entry, long handle, s_sort_record *out)
{
	(void)&tag; (void)&context; (void)&pass; (void)&stage;
	(void)&entry; (void)&handle; (void)&out;
	byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
	byte *blocks = *(byte **)(table + 0x14);
	byte *record = *(byte **)(table + 0x1c) + (((dword)handle >> 8) & 0x3fffff) * 24;
	s_geometry_block_info *block = (s_geometry_block_info *)(blocks + *(short *)(record + 6) * 0x2c);
	long const *pair = &(*(long **)((byte *)g_4e0350 + 0x37c))[(signed char)record[0] * 2];
	long const *const &kept_pair = pair;
	long record_tag = kept_pair[1];
	byte *definition = g_4e3b44[record_tag & 0xffff].bytes;
	byte *part = *(byte **)(definition + 0x14) + record[1] * 20;
	if (function_12de70(block, 3) && *(word *)(record + 0xe) > 0)
	{
		out->value0c = handle;
		out->unknown04 = 0;
		out->group = 4;
		out->subkey = 0xffff;
		byte *material = record_material_4e0d0(tag, pass, stage, entry);
		out->value10 = (dword)material;
		switch (part[4])
		{
		case 3: out->value08 = 7; break;
		case 4: out->value08 = 8; break;
		default: out->value08 = 0; break;
		}
		out->unknown14 = 0;
		union { long value; bool ready; } availability;
		availability.value = function_4cbb0(*(long *)(material + 0x100), out->value08);
		return availability.ready;
	}
	return false;
}

typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);

// @retail 0x3b9d0
bool function_3b9d0(s_record_sources *data, long tag_index, long pass, dword and_mask, dword or_mask,
	t_record_fill fill, dword value04, void *context, byte value14)
{
	bool result = false;
	if (data->count < (long)data->unknown00)
	{
		byte *groups = record_format_groups(tag_index);
		dword flags = *(dword *)(*(byte **)(groups + 4) + pass * 10 + 2);
		s_record_source *source = &data->sources[data->count++];
		source->fill = fill;
		source->unknown04 = value04;
		source->context = context;
		source->key = tag_index;
		source->value14 = value14;
		source->value15 = (byte)pass;
		source->flags = (flags & and_mask) | or_mask;
		data->flags |= source->flags;
		result = true;
	}
	return result;
}

s_render_source_storage g_render_source_storage;

// @retail 0x3b8a0
void function_3b8a0(void)
{
	g_4c1a48[0].unknown00 = 0xa00;
	g_4c1a48[0].count = 0;
	void *buffer = VirtualAlloc(0, 0xf000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[0].sources = (s_record_source *)buffer;
	g_4c1a48[1].unknown00 = 0x400;
	g_4c1a48[1].count = 0;
	buffer = VirtualAlloc(0, 0x6000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[1].sources = (s_record_source *)buffer;
	g_4c1a48[2].unknown00 = 0x200;
	g_4c1a48[2].count = 0;
	buffer = VirtualAlloc(0, 0x3000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[2].sources = (s_record_source *)buffer;
}

// @retail 0x3b950
void function_3b950(void)
{
	if (g_4c1a48[0].sources)
	{
		if (!VirtualFree(g_4c1a48[0].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[0].sources = 0;
	}
	if (g_4c1a48[1].sources)
	{
		if (!VirtualFree(g_4c1a48[1].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[1].sources = 0;
	}
	if (g_4c1a48[2].sources)
	{
		if (!VirtualFree(g_4c1a48[2].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[2].sources = 0;
	}
}

// Returns bool: its one caller (0x2bcd0) stores AL straight to a byte slot (0x2c10e) with no 0/1 normalisation.
// @retail 0x3ba90
bool function_3ba90(long bit, s_record_sources const *data)
{
	long result;
	/* Retail keeps the source-table pointer on the stack. */
	s_record_sources const *const *reference = &data;
	result = 1 << bit;
	result &= (*reference)->flags;
	return !result;
}

// @retail 0x3bb80
bool __stdcall function_3bb80(void const *a, void const *b, void const *context)
{
	s_sort_record const *left = (s_sort_record const *)a;
	s_sort_record const *right = (s_sort_record const *)b;
	if (left->value10 > right->value10) return true;
	if (left->value10 < right->value10) return false;
	if (left->key > right->key) return true;
	if (left->key < right->key) return false;
	if (left->group > right->group) return true;
	if (left->group < right->group) return false;
	if (left->value08 > right->value08) return true;
	if (left->value08 < right->value08) return false;
	return false;
}

// @retail 0x3bbd0
bool __stdcall function_3bbd0(void const *a, void const *b, void const *context)
{
	s_sort_record const *left = (s_sort_record const *)a;
	s_sort_record const *right = (s_sort_record const *)b;
	if (left->group > right->group) return true;
	if (left->group < right->group) return false;
	if (left->subkey > right->subkey) return true;
	if (left->subkey < right->subkey) return false;
	if (left->value10 > right->value10) return true;
	if (left->value10 < right->value10) return false;
	if (left->key > right->key) return true;
	if (left->key < right->key) return false;
	if (left->value0c > right->value0c) return true;
	if (left->value0c < right->value0c) return false;
	if (left->index > right->index) return true;
	if (left->index < right->index) return false;
	return false;
}

typedef bool (__stdcall *t_record_compare)(void const *, void const *, void const *);
typedef void (__stdcall *t_record_draw)(long, void *, long, long, long, void *, s_sort_record *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_record_compare compare, void const *context);

// @retail 0x3bab0
long function_3bab0(s_record_sources *data, void *context, long mode, s_sort_record *out)
{
	/* All four arguments occupy stack slots in retail. */
	(void)&data;
	(void)&context;
	(void)&mode;
	(void)&out;
	long count = 0;
	for (long i = data->first; i < data->count; ++i)
	{
		s_record_source *source = &data->sources[i];
		if (source->flags & (1 << mode))
		{
			long key = source->key;
			long value15 = source->value15;
			s_sort_record *record = &out[count++];
			record->index = (word)i;
			record->key = (word)source->key;
			if (!source->fill(key, context, value15, mode, source->value14, source->context, record))
				--count;
		}
	}
	t_record_compare compare = function_3bb80;
	if (mode == 5)
		compare = function_3bbd0;
	function_13da70(out, count, sizeof(s_sort_record), compare, data);
	return count;
}

// @retail 0x3bc30
void function_3bc30(void *context, s_record_sources *data, long mode)
{
    void *const *context_reference = &context;
    s_sort_record records[2560];
    long count = function_3bab0(data, *context_reference, mode, records);
    if (count > 0)
    {
        s_sort_record *record = records;
        volatile long remaining = count;
        do
        {
            s_record_source *source = &data->sources[record->index];
            if (*(long *)((byte *)record->value10 + 0x100) != NONE)
            {
                t_record_draw callback = (t_record_draw)source->unknown04;
                callback(source->key, *context_reference, source->value15, mode,
                    source->value14, source->context, record);
            }
            count = remaining;
            ++record;
            remaining = --count;
        } while (count);
    }
}


#include "flexible_surface_calls.h"

struct s_40f60_source
{
	long tag;
	long context;
};

// @retail 0x40f60
void function_40f60(void *submission, surface_render_test fill, surface_render_draw submit)
{
	s_40f60_source const *source = (s_40f60_source const *)submission;
	(void)&fill;
	(void)&submit;
	byte *groups = record_format_groups(source->tag);
	long group = **(word **)(groups + 4) & 0x1ff;
	long count = (*(word **)(groups + 0xc))[group + 15] >> 9;
	for (long i = 0; i < count; ++i)
	{
		s_sort_record record;
		if (fill(source->tag, 0, 0, 15, i, source->context, &record))
			submit(source->tag, 0, 0, 15, i, source->context, &record);
	}
}

bool __stdcall function_4cbf0(long, long, long, long, long, long, void *);
void __stdcall function_4d0b0(long, long, long, long, long, long, void *);

// @retail 0x41020
void __stdcall function_41020(void *submission)
{
	void *const *submission_reference = &submission;
	function_40f60(*submission_reference, function_4cbf0, function_4d0b0);
}

extern long g_467130;
void *g_467134 = (void *)NONE;
s_record_sources *g_467138;
long g_4858b0;
byte g_485a75, g_485a76;
extern byte g_4670bc;
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);

// @retail 0x44550
void function_44550(void)
{
    g_4858b0 = g_467130;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    long mode = g_467130;
    if (((1 << mode) & 0xbffdee) &&
        (mode != 16 || g_48574c == 3 || g_485a75 || g_485a76))
        function_3bc30(g_467134, g_467138, mode);
    g_4858b0 = NONE;
}

extern bool g_4ba019;
extern real g_485b28[7];

// @retail 0x1bf50
bool function_1bf50(long level, long tag, real distance, long *out)
{
    long selected = 0;
    long const *tag_pointer = &tag;
    byte *groups = record_format_groups(*tag_pointer);
    long count = *(long *)groups;
    if (g_4ba019)
        distance *= 2.0f;
    if (level != 5)
    {
        real threshold = g_485b28[level] * distance;
        for (selected = 0; selected < count; ++selected)
        {
            long *reference;
            dword kind = *(dword *)g_4e3b44[(short)*tag_pointer].unknown00;
            if (kind == 0x5052544d || kind == 0x70727433)
            {
                c_record_reference_view *provider = 0;
                switch (kind)
                {
                case 0x5052544d: provider = (c_record_reference_view *)&g_479874; break;
                case 0x70727433: provider = (c_record_reference_view *)&g_479868; break;
                }
                if (provider)
                    provider->initialize(*tag_pointer);
                reference = provider->reference();
            }
            else
                reference = *(long **)(g_4e3b44[*tag_pointer & 0xffff].bytes + 0x24);
            byte *groups = *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
            byte *entries = *(byte **)(groups + 4);
            if (threshold >= *(real *)(entries + selected * 10 + 6))
                break;
        }
    }
    *out = selected;
    long result;
    if (selected < 0)
        result = 0;
    else
        result = selected > count - 1 ? count - 1 : selected;
    *out = result;
    return result < count;
}

void function_1cf50(void);

struct s_4daa0_part
{
    byte unknown00[6];
    word first, count;
    short subpart, subpart_count;
};
struct s_4daa0_subpart
{
    word first, count;
    dword unknown04;
};
struct s_4daa0_geometry
{
    byte unknown00[0xc];
    s_4daa0_subpart *subparts;
    byte unknown10[0x14];
    word *indices;
};

// @retail 0x4daa0
long function_4daa0(bool filtered, bool strip, s_4daa0_geometry const *geometry,
    s_4daa0_part const *part, dword const *mask)
{
    long result = 0;
    if (!filtered)
    {
        long count = part->count;
        word *indices = geometry->indices + part->first;
        function_1cf50();
        D3DDevice_DrawIndexedVertices(strip ? D3DPT_TRIANGLESTRIP : D3DPT_TRIANGLELIST, count, indices);
        result = count;
    }
    else if (strip)
    {
        for (long i = part->subpart; i < part->subpart + part->subpart_count; ++i)
        {
            if (mask[i >> 5] & (1 << (i & 31)))
            {
                s_4daa0_subpart const *subpart = &geometry->subparts[i];
                long count = subpart->count;
                result += count;
                word *indices = geometry->indices + subpart->first;
                function_1cf50();
                D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLESTRIP, count, indices);
            }
        }
    }
    else
    {
        long first = NONE;
        long last = NONE;
        for (long i = part->subpart; i < part->subpart + part->subpart_count; ++i)
        {
            if (mask[i >> 5] & (1 << (i & 31)))
            {
                s_4daa0_subpart const *subpart = &geometry->subparts[i];
                if (first == NONE)
                {
                    first = subpart->first;
                    last = first + subpart->count - 1;
                }
                else if (subpart->first != last + 1)
                {
                    word *indices = geometry->indices + first;
                    function_1cf50();
                    D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST, last - first + 1, indices);
                    result += last - first + 1;
                    first = subpart->first;
                    last = first + subpart->count - 1;
                }
                else
                    last = subpart->first + subpart->count - 1;
            }
        }
        if (first != NONE)
        {
            word *indices = geometry->indices + first;
            function_1cf50();
            D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST, last - first + 1, indices);
            result += last - first + 1;
        }
    }
    return result;
}

void function_40e30(short group, long tag, real distance, long level, short kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context);

struct s_cache_record;
s_cache_record *function_1e2d0(void);
long function_40e10(bool first, bool second);



typedef void (__stdcall *t_41490_callback)(void *);

struct s_41490_record
{
    long type;
    real depth;
    long flags;
    byte active;
    byte unknown0d[0x13];
    void (__stdcall *callback)(void *);
    long tag;
    void *context;
    byte unknown2c[0x38];
    point3f position;
};

// score 72 (was 72): the build inlines 0x40e10 (27 bytes, defined in unknown_03f2b0.cpp) where retail calls it with
// cl/dl = definition[0x16] bits 1/0; with `// @flags /O2 /Ob1 /Gr` on unknown_03f2b0.cpp (a trial only) ours calls it too and
// the size is retail's (426), score 127. record->position as a struct copy gives retail's `add esi,0x64` then three stores
// but scores 75 in the full build (122 with the call kept), so the member copies stay. With the call kept, ours takes `position` in ebx
// (ret 0x1c) where retail has it on the stack (ret 0x20, loaded into ebp at entry, its slot reused for `pass`). Tried with the
// call kept: volatile read of position at its first use (119; ret 0x20 but distance moves to ebx), that plus a volatile read of
// distance at the 0x40e30 call (83; group then travels in ecx), that plus a volatile read of group (113), (void)&position (no change).
// Permuter (25 min on r643): 8 below its own baseline, only with a float product truncated through a word and
// 0x40e10's two flags swapped (a behaviour change); not kept.
// With 0x40e10 kept out of line as a trial (noinline at its definition, or /Ob1 on its file: same code), reading
// position once through its address (`point3f const *const *position_reference = &position;`, then
// `point3f const *point = *position_reference;`) keeps it on the stack (ret 0x20, 426 bytes, 51), and with the
// position copied as one aggregate 40; left: point and definition swap ebx/ebp, the flag byte is read once (retail
// twice), and the volatile delta reads turn the depth products (retail folds the spilled delta as memory operands).
// Retail spills the delta across the call where ours keeps it in xmm0-2 (the plain delta then gives 148): retail's
// callers get no register summary for 0x40e10. `inline` on it alone still expands here (131 plain delta, 75).
// Tried with the call expanded: the flag arguments as `? true : false` 72 or `!= 0` 72 (same code), the position
// address read 72, the aggregate copy 75, both 75.
// M3 GB r14 (fast with 0x40e10, 0x41040, 0xd4cf0, 0x244630, 0x2be670, 0x40e30; control 72). Match-pursuit lead (NOT kept:
// requires moving 0x40e10 out of line, uses position_reference, raises 0x41040 300 -> 335 though cutting 0x41040 mk 74 -> 65,
// and still 8 away from a byte match): putting 0x40e10 as an `inline` function in its own `/O2 /Ob0 /Gr` LTCG file keeps it
// out of line with no XMM register summary (so plain non-volatile `vector3f delta` spills cleanly across `call 0x40e10` and
// folds `[esp+0x10]`/`[esp+0x14]` into `mulss`) AND keeps LTCG `(first, second)` order (`and cl, 1` before `and dl, 1`);
// combining that with `*(word *)(definition + 0x16)` bit tests (fixes both `ebx=definition`/`ebp=position` and the double
// `mov cl/dl, [ebx+0x16]` loads), `position_reference` (`ret 0x20`, all 3 callers MATCH), and `record->position = *point`
// reaches 8 at retail's exact 426/426 B and 117/117 insns (first diff `+0x13e`, `al=3`, `mk=4`: only `movss xmm0, [esp+0x18];
// mulss xmm0, [0x48562c]` vs `movss xmm0, [0x48562c]; mulss xmm0, [esp+0x18]` remains). With `/GL-` `__fastcall(second, first)`
// instead: plain delta + `position_reference` 34 (`+0x8`), + `*(word *)(definition + 0x16)` + staged depth 10 (`+0x12e`, `al=5`,
// `and dl, 1` before `and cl, 1`), + `dot3f` 11, + `word` bitfield `TEST_FIELD_BIT` 10, `dword` flags 12, `vector3d_from_points3d`
// 10, `basis_dot_xzy` 10, 3 scalar `real` deltas 119-120; on the 8 lead: `depth` before `flags`/`callback` 11, flat dot / `dot3f`
// / `k,j,i` / `-dot3f` 9, `i,k,j` / `basis_dot_xzy` / `delta` at top / `delta[3]` / init-list / `delta.k` first / 1..4 unused
// locals 8, `delta.k` after `tag` 15, `delta.k *=` 15; `#pragma inline_depth(0)` around 0x41490 stops `record_format_groups`
// inlining (196/224), around 0x40e10 in unknown_03f2b0.cpp still expands 0x40e10 (131); 0x40e10 moved into this `/Ob1` file
// keeps the register summary (154). On HEAD (call expanded): `*(word *)(definition + 0x16)` 70 (-2, under bar), flat dot 72.
// M3 GB r17, NOT KEPT (moving a function into a file with other flags is kept only with a byte match of the target; the
// same body without the move is 73; the 12 form is held as a patch). Fast and plain checked with 0x40e10, 0x41040, callers
// 0xd4cf0/0x244630/0x2be670 and all file peers:
// moving 0x40e10 into this `/Ob1` file (next to its retail neighbours and its only two callers 0x41040/0x41490) + `*(word *)(definition + 0x16)`
// + `record->position = *position` + `0.0f - (delta.i * forward.i + delta.j * forward.j + delta.k * forward.k)` keeps `position`
// on the stack (`ret 0x20`, no `position_reference`), keeps `0x40e10` and all 3 callers MATCH and `0x41040` unchanged at 300,
// and drops 0x41490 72 -> 12 at retail's exact 426/426 B (`+0x12d`; only `delta.k` spill order before `and cl/dl, 1` and
// volatile `delta.i/j` non-folded `mulss` remain). Also tried on HEAD (call expanded): word+aggpos 73, plain v3d+dot3f 131,
// word bitfield / nested if / neg dot / dword flags 70, scalar volatile deltas 127, `depth = delta.k; depth *=` 75; with 0x40e10
// moved here: main body 127 (breaks callers), word+aggpos+staged depth 17 (kij/kji/ijk/neg/bitfield 17, flat kij 13, sym ijk 12).
// M3 GB r23 (44 fast trials with 0x41040, 0x40e10, 0x40e30, 0xd4cf0, 0x244630, 0x2be670, 0x44ac0, 0x44c90, 0x402b0, 0x1bf50,
// 0x1e2d0; control 72, 0x41040 189): on inline `0x40e10` in a `/O2 /Ob0 /Gr` file (no XMM summary; non-inline `/Ob0` stays 146/189),
// plain non-volatile `vector3f delta` + `*(word *)(definition + 0x16)` + `record->position = *position` + staged `k,i,j` depth
// drops `0x41490` 72 -> 8 (`426/426 B`, `+0x13e`) with NO `position_reference` and NO `volatile` AND simultaneously drops `0x41040`
// 189 -> 156 (`1089/1089 B`, `+0x47`), keeping all 10 peers/callers (incl. `0x40e10`) MATCH (NOT kept: moving `0x40e10` to a
// different-flags file is match-only). On that score-8 base: all operand orders of staged `k,i,j` / unary `-depth` / `forward` ptr
// / `vector3d_from_points3d` / plain struct / `depth = delta.k; depth *=` / `basis_dot_xzy` / staged `i,k,j` / `type` local /
// renamed `delta` / plain global camera diag / parenthesized `(k+i)+j` / `/O2 /arch:SSE /Ob0` / `delta` ptr / top/mid `delta` /
// `real delta[3]` / `.n[]` all stay 8 (`movss xmm0, [forward.k]; mulss xmm0, [esp+0x18]`); flat sums / `dot3f` / 3 term locals 9;
// depth before callback/flags 11-12; `delta.k` after tag 15; 3 scalar reals 120 (`sub esp, 0`); `/O1 /Ob0` 54 (breaks `0x40e10`);
// diag volatile read on `delta.k` only 7 (`+0x16d`, matches lines 94-101 verbatim but flips `addss xmm1, xmm0`). Restored at 72.
// @retail 0x41490
void function_41490(short group, long tag, short kind, real distance, t_record_fill fill,
    dword value, t_41490_callback callback, void *context, point3f const *position)
{
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    long pass;
    function_1bf50(*(word *)(definition + 0x3c), tag, distance, &pass);
    byte *groups = record_format_groups(tag);
    dword flags = *(dword *)(*(byte **)(groups + 4) + pass * 10 + 2);
    if (flags & 0x10006e)
        function_40e30(group, tag, distance, *(word *)(definition + 0x3c), kind,
            0xffffffff, 0, fill, value, context);
    if ((flags & 0x8000) && group == 0)
    {
        s_41490_record *record = (s_41490_record *)function_1e2d0();
        if (record)
        {
            vector3f volatile delta;
            delta.i = position->x - g_485618.position.x;
            delta.j = position->y - g_485618.position.y;
            delta.k = position->z - g_485618.position.z;
            record->context = context;
            record->tag = tag;
            record->type = function_40e10((bool)(definition[0x16] & 1), (bool)((definition[0x16] >> 1) & 1));
            record->flags = 0;
            record->callback = callback;
            real depth = delta.k * g_485618.forward.k;
            depth += g_485618.forward.i * delta.i;
            depth += g_485618.forward.j * delta.j;
            record->depth = 0.0f - depth;
            record->position.x = position->x;
            record->position.y = position->y;
            record->position.z = position->z;
        }
    }
}

#include <math.h>
dword __cdecl pack_color3f(color3f const *color);

// @retail 0x3d000
void function_3d000(real const *state)
{
    real direction[4] = { 1.0f, -1.0f, -1.0f, 0.5f };
    D3DDevice_SetVertexShaderConstant(-75, direction, 1);
    direction[0] = (real)(cos(state[29]) * sin(state[30]));
    direction[1] = (real)(sin(state[29]) * sin(state[30]));
    direction[2] = (real)cos(state[30]);
    direction[3] = state[25];
    real mapped[4];
    mapped[0] = (real)((direction[0] + 1.0f) * 0.5f);
    mapped[1] = (real)((direction[1] + 1.0f) * 0.5f);
    mapped[2] = (real)((direction[2] + 1.0f) * 0.5f);
    mapped[3] = 0.4970000088214874f;
    D3DDevice_SetVertexShaderConstant(-73, mapped, 1);
    if (state[34] > 0.0001f && state[40] > 0.0001f && state[41] > 0.0001f &&
        state[41] - state[40] > 0.0001f)
    {
        real inverse = 1.0f / state[34];
        struct { real b, a; } local_low_high_record;
        local_low_high_record.a = (real)(inverse * state[40] * 16777215.0);
        local_low_high_record.b = (real)(inverse * state[41] * 16777215.0);
        real inverse_range = 1.0f / (local_low_high_record.b - local_low_high_record.a);
        direction[0] = inverse_range;
        direction[1] = 0.0f - inverse_range * local_low_high_record.a;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
        D3DDevice_SetVertexShaderConstant(-72, direction, 1);
    }
    else
    {
        direction[0] = 0.0f;
        direction[1] = 0.0f;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
        D3DDevice_SetVertexShaderConstant(-72, direction, 1);
    }
    D3DDevice_SetRenderState(D3DRS_TEXTUREFACTOR, pack_color3f((color3f const *)(state + 22)));
    real parameters[4];
    memcpy(parameters, state + 36, sizeof(parameters));
    if (parameters[2] > 0.0001f)
        parameters[2] = 1.0f / parameters[2];
    if (parameters[3] > 0.0001f)
        parameters[3] = 1.0f / parameters[3];
    parameters[1] = 0.0f - state[37];
    D3DDevice_SetVertexShaderConstant(-74, parameters, 1);
    parameters[0] = state[13];
    parameters[1] = state[14];
    parameters[2] = state[15];
    parameters[3] = 1.0f;
    D3DDevice_SetVertexShaderConstant(-71, parameters, 1);
}


#include <string.h>
#include "visibility_slot.h"
#include "sampler_state.h"
struct s_shader_cache;
extern byte *g_485a80;
extern long g_4858b8;
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);
void function_14bc0(short index, short element, bool use_depth);
void function_12d50(bool scaled, real const *projection, real *output);
dword function_1cbe0(long mode, dword index, long slot_index);
point3f g_4c1b08[4];

#pragma inline_depth(0)
PRIVATE __forceinline void submit_quad_vertices(void)
{
    D3DDevice_Begin(D3DPT_QUADLIST);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, g_4c1b08[0].x, g_4c1b08[0].y, g_4c1b08[0].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, g_4c1b08[3].x, g_4c1b08[3].y, g_4c1b08[3].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, g_4c1b08[2].x, g_4c1b08[2].y, g_4c1b08[2].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, g_4c1b08[1].x, g_4c1b08[1].y, g_4c1b08[1].z, 1.0f);
    D3DDevice_End();
}
#pragma inline_depth()

// @retail 0x3bf20
long __stdcall function_3bf20(s_slot *slot, byte *data)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_1cbe0(0, 0, 2);
    function_1c710(g_51f0f0);
    function_14bc0((short)g_4858b8, 0, true);
    D3DPIXELSHADERDEF program;
    memset(&program, 0, sizeof(program));
    program.PSRGBOutputs[0] = 0xc0;
    program.PSAlphaOutputs[0] = 0xc0;
    program.PSCombinerCount = 0x11101;
    program.PSTextureModes = 0;
    program.PSInputTexture = 0;
    program.PSDotMapping = 0;
    program.PSCompareMode = 0;
    program.PSRGBInputs[0] = 0xc4200000;
    program.PSAlphaInputs[0] = 0xd4301010;
    program.PSConstant0[0] = 0;
    program.PSConstant1[0] = 0;
    program.PSC0Mapping = 0xffffffff;
    program.PSC1Mapping = 0xffffffff;
    program.PSFinalCombinerConstants = 0x1ff;
    D3DDevice_SetPixelShaderProgram(&program);
    function_12d50(false, NULL, NULL);
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    sampler_state_write(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    sampler_state_write(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 1);
    D3DDevice_SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    D3DDevice_SetRenderState(D3DRS_STENCILREF, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    D3DDevice_SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    D3DDevice_SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    D3DDevice_SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE);
    D3DDevice_SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    D3DDevice_SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    submit_quad_vertices();
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    return 0x4b000;
}



#include <math.h>
s_3c9a0_matrix g_4c1b9c;

PRIVATE __forceinline void set_bump_component(long stage, D3DTEXTURESTAGESTATETYPE component, real value)
{
    D3DDevice_SetTextureStageState(stage, component, *(dword *)&value);
}

// score 84 (was 213): off-diagonal products are shared (xy/xz/zy) and the statement order is xx,yy,zz,ys,zs,xs,inverse. Still differs:
// scheduling and register choice only (retail loads the axis as j,i,k,j,k, takes 1.0f after fcos, and keeps ys/zs on one xmm copy of
// sine; ours computes ys through x87). Tried: sine/cosine as plain locals instead of the struct (115-130), ys/zs before the squares
// (116-189), inverse first (117), ys,zs computed right after sin and before cos (195), xs as (double) cast / plain / swapped operands (all 84).
// Permuter (25 min on r643): 24 below its own baseline, only with a volatile xs and a float term truncated through
// a long; not kept.
// Retail's frame puts sine and cosine in the two lowest cells ([esp], [esp+4]) and the xs/ys spills in view_forward's
// dead slots, the layout of plain locals (aggregates sort above the cells), and computes ys and zs in SSE from an xmm
// copy of sine with only xs on x87 (ours sends ys and xs to x87: register pressure). The 0x1544e0 builder (matched)
// shows the same mixed x87/SSE sine products. Tried: the angle as a real 179 (852 bytes, rounded); plain sine/cosine
// locals 130 (retail's two cells, view_forward one cell higher); the sine products as one vector in j,k,i order 142,
// in i,j,k order 195, and in i,j,k order with plain trig locals 170; a search over 60 allocation choices: best 83.
// GB r14 (fast with 0x3bf00 and 0x4d640, both MATCH; control 84 at 845/845 B, al=63, mk=40; 50+ trials): plain real
// sine/cosine locals (no trig struct) with vector3f squares + vector3f sine_axis and 0x1544e0's store order recovers
// retail's 0x14 frame, [esp]=sine/[esp+4]=cosine/[esp+8]=xs/[esp+0xc]=ys, SSE ys/zs + x87 xs, and post-fcos 1.0f load:
// direct with `inverse = 1.0f - cosine` 67 (847/845 B, al=49, mk=36); without `inverse` local mk=33 (849 B, 0 tail diffs);
// direct with squares in k,j,i order 64 (845/845 B, 189/187 insns, al=39, mk=26). Dropping redundant `(double)` casts on
// `atan2(view_forward.i, view_forward.j)` on that form makes the first 136 bytes (+0x0..+0x87) byte-exact at 845/845 B
// (115, al=17, mk=16: stage 1 flips g_4c1b9c ahead of state); adding a two-level inline rotation helper (matching 0x17fd20)
// without a state alias hits 39 (845/845 B, 187/187 insns, al=34, mk=30; new one-caller helper, not kept; calling 0x17fd20's
// existing decal_transform_rotation_17fd20 directly gives 190-198 at 859-861 B because its j*i product order differs);
// match-only lead with `real const *values = (real const *)state;` (parameter rename) + up.j before left.k hits 14
// (845/845 B, 187/187 insns, +0x88, al=12, mk=12: only 2-insn movss [esp+4] and subss shifts + stage 2 [esi+0x6c] order
// left; `real const *state` param 115, `values` after angle 115, `scales = (real const *)(state + 0x68)` 90/202,
// `scale = 1.0f` after rotation 81, 1-7 dead locals 115-150). None passes keep rules without a byte match; body unchanged.
// M3 GB r17 (fast with 0x3bf00 and 0x4d640 MATCH; control 84; 34 real trials): HEAD with no `(double)` casts on atan2 88,
// `transform4x3f g_4c1b9c` 84, typed `s_3c650_state const *state` param across 0x3c650/0x3bf00/0x4d640 84, plain `sine`/`cosine`
// + scalar squares/sine_axis 134; plain `sine`/`cosine` + `vector3f` squares/sine_axis without shared xy/xz/zy 190-222 (861-886 B),
// `0x1544e0`-style scalar squares + `vector3f sine_axis` 165 (851 B), direct `D3DDevice_SetTextureStageState` 189 (858 B),
// `kji` squares + `ijk`/`jki` `sine_axis` + shared xy/xz/zy with/without `inverse` and swapped bump operands 76-177 (with
// typed `s_3c650_state const *water` matching `values` at 76/86, confirming bump operand order is symbol-canonicalized). Body unchanged.
// M3 lane 15 r149 (h2m fast; control 84). Lead, match-only: plain `real sine`/`cosine` locals, `vector3f squares` and
// `vector3f sine_axis` filled i, j, k, `(1.0f - cosine)` written in each shared product (xy = (1.0f - cosine) * axis->i *
// axis->j, xz, zy), 0x1544e0's store order (scale, forward.i, left.i, forward.j, left.j, up.i, forward.k, up.k, up.j,
// left.k) and atan2 without the `(double)` casts: 18 at 845/845 B (first +0x88; the head, frame and x87/SSE split are
// retail's). Left there: the xy/xz chains sorted j before i (retail i, j: four more symbols ahead of `axis` fix both, 14
// with four unused longs), stage 3's state load ranks below the matrix (one grouping pair in any stage 1 or stage 2
// product fixes it: 10, or 6 with the four longs), then two schedule spots (the cosine load ahead of the x87 sine
// product; up.j's subtract ahead of the left.k store). Not kept: `(1.0f - cosine)` at each use replaces a named local
// (with `inverse` named it measures 115-165), and the grouping pair and the longs only steer. Also on the 18 form:
// squares k, j, i 169-174; product text orders 18 (canonical); `inverse` after forward.i 164, after axis 115; axis
// declared at the top 66-70, after view_forward 18, read through g_4687b0 18; the parameter alias `values` 18 (119 with
// the grouping pair); the angle offset named 164; the two vectors declared ahead of axis 10 (same); position zeroed z, y,
// x or x, y, z 18; stage 2 as unary minus 18; sine_axis in every order 6-7 or 185, placed after scale 31 or forward.i 38.
// @retail 0x3c650
void function_3c650(byte const *state)
{
    vector3f view_forward = g_4b9dac;
    double angle = atan2((double)view_forward.i, (double)view_forward.j) + *(real const *)(state + 0x8c);
    struct { real sine; real cosine; } trig;
    trig.sine = (real)sin(angle);
    trig.cosine = (real)cos(angle);
    vector3f const *axis = g_4687b0;
    real xx = axis->i * axis->i;
    real yy = axis->j * axis->j;
    real zz = axis->k * axis->k;
    real ys = axis->j * trig.sine;
    real zs = axis->k * trig.sine;
    real xs = trig.sine * axis->i;
    real inverse = 1.0f - trig.cosine;
    g_4c1b9c.scale = 1.0f;
    g_4c1b9c.forward.i = (1.0f - xx) * trig.cosine + xx;
    real xy = inverse * axis->i * axis->j;
    g_4c1b9c.forward.j = xy + zs;
    g_4c1b9c.left.i = xy - zs;
    g_4c1b9c.left.j = (1.0f - yy) * trig.cosine + yy;
    real xz = inverse * axis->i * axis->k;
    g_4c1b9c.forward.k = xz - ys;
    g_4c1b9c.up.i = xz + ys;
    real zy = inverse * axis->k * axis->j;
    g_4c1b9c.left.k = zy + xs;
    g_4c1b9c.up.j = zy - xs;
    g_4c1b9c.up.k = (1.0f - zz) * trig.cosine + zz;
    g_4c1b9c.position.x = g_4c1b9c.position.y = g_4c1b9c.position.z = 0.0f;
    set_bump_component(1, D3DTSS_BUMPENVMAT00, *(real const *)(state + 0x68) * g_4c1b9c.forward.i);
    set_bump_component(1, D3DTSS_BUMPENVMAT01, *(real const *)(state + 0x68) * g_4c1b9c.forward.j);
    set_bump_component(1, D3DTSS_BUMPENVMAT10, *(real const *)(state + 0x68) * g_4c1b9c.left.i);
    set_bump_component(1, D3DTSS_BUMPENVMAT11, *(real const *)(state + 0x68) * g_4c1b9c.left.j);
    set_bump_component(2, D3DTSS_BUMPENVMAT00, 0.0f - *(real const *)(state + 0x6c) * g_4c1b9c.forward.i);
    set_bump_component(2, D3DTSS_BUMPENVMAT01, 0.0f - *(real const *)(state + 0x6c) * g_4c1b9c.forward.j);
    set_bump_component(2, D3DTSS_BUMPENVMAT10, 0.0f - *(real const *)(state + 0x6c) * g_4c1b9c.left.i);
    set_bump_component(2, D3DTSS_BUMPENVMAT11, 0.0f - *(real const *)(state + 0x6c) * g_4c1b9c.left.j);
    set_bump_component(3, D3DTSS_BUMPENVMAT00, *(real const *)(state + 0x70) * g_4c1b9c.forward.i);
    set_bump_component(3, D3DTSS_BUMPENVMAT01, *(real const *)(state + 0x70) * g_4c1b9c.forward.j);
    set_bump_component(3, D3DTSS_BUMPENVMAT10, *(real const *)(state + 0x70) * g_4c1b9c.left.i);
    set_bump_component(3, D3DTSS_BUMPENVMAT11, *(real const *)(state + 0x70) * g_4c1b9c.left.j);
}



byte function_016a90();
s_3c9a0_matrix g_4c1b68;

// score 229 (was 291): logic matches retail block by block (constants checked in the XBE: 0.5, 0.04, -12.0, 0.78068578,
// 1.0, 1.13, 1.5, 0.0001); no behaviour difference found. Retail spills tangent and view_tangent to float stack slots and
// divides with divss, ours keeps them on the x87 stack (fdiv st(1)); retail also keeps the near_x/near_y/z stores to
// g_4c1b08 and reloads them in the corner sums, ours forwards them and drops the stores (1330 vs 1387 bytes), and its
// frame is 0x4c, ours 0x60.
// Tried: store order z,x,y,scale (291 -> 291), volatile tangent (-6), volatile tangent+view_tangent (-6), writing only
// g_4c1b08[3].y and keeping the other corners in locals (same code).
// The shared tangent for the near/far rows is a real local, not a double: retail keeps it unrounded on the x87 stack
// for both rows, which a real held in a register also gives (same code here but one fadd), and a double local made
// the caller 0x3c3c0 pad its frame for 8-byte stack parity, which retail's 0x3c3c0 does not (315 -> 291 in the full
// build, 0x3c3c0 150 -> 136 with it).
// M3 lane 2 r151 (fast screen; 291 at 1330 B before; the reused pair and the `-12.0f *` form are HELD as a patch at
// m3 review: computing the far corner after the near stores is statement order and `-12.0f * (angle * 0.04f)` is
// operand order, both kept only with a byte match; the 0x16a90 call is kept, same code): one pair of corner_x/corner_y locals reused for the near and then the
// far corner (retail's shared cells: the near values are re-read from g_4c1b08 in the corner sums once the far ones
// overwrite the locals) 239 at retail's 1387 B; the hand-written aspect test replaced by the existing 0x16a90, which
// retail expands here, 239 (same code); the near angle as `-12.0f * (angle * 0.04f)`, so the two constant products stay
// apart as in retail (`angle * 0.04f * -12.0f` folds them into one), 229 (1388 B); without the reused pair that form
// measures 287 (1336 B). Lead, not kept (0x3c9a0 matches in every order, so only this caller's count moves): 0x3c9a0's
// parameters as (matrix, forward, up), the order retail's register setup implies (EAX before EBX, last to first), 237 on
// the 239 form and 227 on the 229 form; (matrix, up, forward) and (up, matrix, forward) also 237. Left: retail rounds the
// first tangent and the view tangent through stack cells and divides and compares in SSE (1.0 held in XMM0, the
// distance in XMM5); ours keeps both on the x87 stack (fdiv st(1), fcompi) and keeps the near corner_x there too, where
// retail stores it at once; frame 0x54 against 0x4c.
// Tried, none lower: the half-angle local reused for the view tangent 291 (same code); on the 239 form, the width scale
// as a copy then `/=`, the view tangent as tan then `*=`, the tangent as tan then `*=`, the width scale computed in place
// of the view tangent, the tangent and corner_x as one local, the aspect and corner_y as one local, 239 each (same code);
// corner_x/corner_y as members of a point2f local 242; the near angle as `angle * 0.04f * -12.0` 230 (1388 B), as
// `angle * 0.04 * -12.0` 242, through a named `angle * 0.04f` step 227 (the kept form's code with the lead reorder); on
// the 227 lead form, position stored z, x, y, then the scale 226; the camera
// local's z rewritten and copied as a whole, scale first or last, 281 (1408 B); the view tangent through the float tan
// overload 227, the first tangent through the double overload 227. Diagnostic (reverted): a float-consistency pragma around
// the body 433 (1545 B: tan becomes a library call, so retail is not built that way, though the divide and compare then
// turn SSE).
// M3 lane 3 r309 (H2M_FAST; 0x3c9a0 MATCH and 0x3c3c0 74 throughout; control 291 at 1330 B). Retail gives the first
// tangent, the view tangent and the near corner_x memory homes (the x87 result stored as a float, then an SSE divide and
// SSE/integer copies); ours keeps them on the x87 stack. None of these moves it: the tangent or the view tangent as a
// real product `(real)tan(...) * x` 291 / 291, both 291 (same code); both with no casts 291; every local declared bare
// at the top 291; the clamp's first test as `!(width_scale > 1.0f)` 306 (1321 B: the second compare folds away, so the
// `<=` / `>` pair is retail's). On the reused corner pair (r151's held form, re-measured 239 at 1387 B, 229 with the
// two-multiply angle): tangent2 computed ahead of the near corner 239; the width scale in one expression 239; no casts
// 239; the `!(... > 1.0f)` test 249; bare top declarations with the two-multiply angle 229. Reading: retail keeps one
// home for corner_x across its near (x87) and far (SSE copy) values, ours splits the two.
// GB r14 (fast with 0x3c9a0 MATCH and 0x3c3c0 74; control 291 at 1330/1387 B, fr 0x60, al 283, mk 169; 23 real trials + 1 diag):
// retail's corner transform computes all four .x, then all four .y, then all four .z (hoisting g_4c1b38[row][2] * 0.0f
// once per axis): nested `for (row 0..2) for (i 0..3)` recovers exact fr 0x4c but does not unroll (630, 996 B); three
// separate 4-corner axis loops (x, y, z) cut mk 169 -> 136 (al 240, fr 0x54, 325 at 1297 B); with near corners stored
// ahead of far (separate locals) 285 (1338 B, 278/280 insns, fr 0x54, al 235, mk 103); with r151's reused x/y + `-12.0f *`
// 247 (1402 B, 281/280 insns, exact fr 0x4c, al 224, mk 143); with `point2f corner` + `real_point3d_set` 256 (fr 0x50).
// Diagnostic (reverted): `volatile tangent` + `volatile view_tangent` on the 247 form 215 (1396 B, 279/280 insns, exact
// fr 0x4c, al 210, mk 132: recovers retail's `fstp` + `divss` + `comiss` across 0x3cb75..0x3cc5c). Also tried (none kept):
// tangent before aspect 291 (same code), + z,x,y,scale position order & camera.x/y in g_4c1b38 291 (al 280); `real_point3d_set`
// 291; 2-caller `half_tan` inline helper 293 (mk 162); column-order matrix fill + row loop 630; 2-row `x_values[2]` loop
// 457 (fr 0x4c); `point3f const *p` loop 291; `(real)tan(...) * scale` with tangent first 291; `for` copy in place of
// memcpy 298; `if (width_scale > 1.0f)` clamp 306; `fabs` distance 347; `1.0f >= width_scale` 291; `real (*matrix)[4]`
// pointer 260 (1402 B, 292 insns, mk 215 worse: indirect loads); constant-LHS clamps 291; `transform_point_3x4` helper 291.
// Body unchanged.
// M3 lane 15 r149 (h2m fast; 0x3c9a0 MATCH, 0x3c3c0 74; control 291). Retail stores the first tangent as a float at
// once and divides and compares in SSE; ours keeps it on the x87 stack across the inlined 0x16a90 test. Tried: the clamp
// as one ternary 306 (1321 B); the near angle through the float tan overload 291; the tangent without casts with the view
// tangent folded into `tan(half_angle) * aspect / tangent` 291; the field of view (+0x7c) named for its five reads 333;
// the tangent scale (+0x80) named for its two reads 313.
// @retail 0x3ca90
void function_3ca90(byte const *state)
{
	if (*(real const *)(state + 0x7c) == 0.0f) return;
	real height = *(real const *)(state + 0x84);
	real offset = *(real const *)(state + 0xa8);
	point3f camera = g_485618.position;
	real distance = camera.z - height;
	if (distance < 0.0f) distance = 0.0f - distance;
	if (distance < 0.0001f) distance = 0.0001f;
	/* retail passes the view forward vector itself (eax = 0x4b9dac) */
	function_3c9a0((vector3f const *)&g_4b9dac, g_4687b0, &g_4c1b68);
	g_4c1b68.scale = distance;
	g_4c1b68.position.x = camera.x;
	g_4c1b68.position.y = camera.y;
	g_4c1b68.position.z = height - distance * offset;
	real aspect = ((real)(short)g_48564e - (real)(short)g_48564a) / ((short)g_48564c - (short)g_485648);
	real tangent = (real)(tan(*(real const *)(state + 0x7c) * 0.5f) * *(real const *)(state + 0x80));
	if (function_016a90())
		aspect *= 1.5f;
	real half_angle = g_485640 * 0.5f;
	if (!(half_angle < 0.7806857824325562f)) half_angle = 0.7806857824325562f;
	real view_tangent = (real)(tan((double)half_angle) * aspect);
	real width_scale = view_tangent / tangent;
	if (width_scale <= 1.0f) width_scale = 1.0f;
	else if (width_scale > 1.0f) width_scale *= 1.13f;
	width_scale *= distance;
	g_4c1b38[0][0] = g_4c1b68.forward.i * distance;
	g_4c1b38[0][1] = g_4c1b68.left.i * width_scale;
	g_4c1b38[0][2] = g_4c1b68.up.i;
	g_4c1b38[0][3] = g_4c1b68.position.x;
	g_4c1b38[1][0] = g_4c1b68.forward.j * distance;
	g_4c1b38[1][1] = g_4c1b68.left.j * width_scale;
	g_4c1b38[1][2] = g_4c1b68.up.j;
	g_4c1b38[1][3] = g_4c1b68.position.y;
	g_4c1b38[2][0] = g_4c1b68.forward.k * distance;
	g_4c1b38[2][1] = g_4c1b68.left.k * width_scale;
	g_4c1b38[2][2] = g_4c1b68.up.k;
	g_4c1b38[2][3] = g_4c1b68.position.z;
	real near_x = (real)tan(*(real const *)(state + 0x7c) * 0.04f * -12.0f);
	real tangent2 = tan(*(real const *)(state + 0x7c) * 0.5f) * *(real const *)(state + 0x80);
	real near_y = (real)(sqrt(near_x * near_x + 1.0f) * tangent2);
	real far_x = *(real const *)(state + 0x88);
	real far_y = (real)(sqrt(far_x * far_x + 1.0f) * tangent2);
	g_4c1b08[0].x = near_x;
	g_4c1b08[0].y = 0.0f - near_y;
	g_4c1b08[0].z = 0.0f;
	g_4c1b08[1].x = near_x;
	g_4c1b08[1].y = near_y;
	g_4c1b08[1].z = 0.0f;
	g_4c1b08[2].x = far_x;
	g_4c1b08[2].y = far_y;
	g_4c1b08[2].z = 0.0f;
	g_4c1b08[3].x = far_x;
	g_4c1b08[3].y = 0.0f - far_y;
	g_4c1b08[3].z = 0.0f;
	point3f corners[4];
	for (long i = 0; i < 4; ++i)
	{
		corners[i].x = g_4c1b38[0][1] * g_4c1b08[i].y + g_4c1b38[0][0] * g_4c1b08[i].x + g_4c1b38[0][2] * g_4c1b08[i].z + g_4c1b38[0][3];
		corners[i].y = g_4c1b38[1][1] * g_4c1b08[i].y + g_4c1b38[1][0] * g_4c1b08[i].x + g_4c1b38[1][2] * g_4c1b08[i].z + g_4c1b38[1][3];
		corners[i].z = g_4c1b38[2][1] * g_4c1b08[i].y + g_4c1b38[2][0] * g_4c1b08[i].x + g_4c1b38[2][2] * g_4c1b08[i].z + g_4c1b38[2][3];
	}
	memcpy(g_4c1b08, corners, sizeof(corners));
}


void *function_44940(short index, bool instance);
long function_1cb20(long mode, dword index, long tag);
extern byte *g_485a80;
extern byte g_485a75, g_485a76;

struct s_render_part_context
{
    long material;
    point3f position;
    real radius;
    byte active;
    byte unknown15[3];
    real values[4];
    real priority;
};
extern real g_4670e4;
long function_40de0(long index, bool first, bool second);

// score 189 (was 300): logic matches retail block by block. The two parameter volatiles stay: without them 0x44ac0 (a caller)
// stops matching because index/part_index leave edx/ecx. Retail takes index raw from edx (`or ebp, edx` at 0x410ed), ours
// zero-extends it ((word)index); the signature is short in every declaring file, so not changed here. Still differs: register
// allocation (retail keeps g_4e3b44 in ebx and spills entry/part to [esp+0x1c]/[esp+0x10]), retail re-reads *part for kind
// and orders the 40de0 call after the 40e30 pushes. Tried: no volatiles (291, breaks 0x44ac0), second via if-flag (296),
// ?: for material (312), long/dword kind (316), kind re-read from part (318), part_index/index swapped (306).
// Also tried (@coldspear, #104 2026-11-02): index as long in all five declarations: 0x41040 315, and the matched caller
// 0x44ac0 stops matching (137), 0x44c90 107 -> 149. Not a lead.
// M3 lane 3 r309 (H2M_FAST with 0x44ac0, 0x44c90 and 0x402b0, MATCH throughout; control 300). Retail ORs the incoming
// index register into the handle raw (`or ebp, edx`) and sign-extends it only for the entry lookup (`movsx esi, dx`); its
// callers pass 32-bit loop counters in EDX. Index as `long` in the definition and all four declarations, the three
// `(short)` casts dropped at the calls, the entry read as `g_4ba138[(short)index]` and the handle OR-ing `index` 315
// (1076 B: retail's raw `or`, every caller still MATCH); on it a local copy of the tag table for the two lookups ahead
// of the first call 314, `long kind` 309, the material as a ternary 302 (cmove), all three 316. On the short form: the
// tag-table local 301 (1079 B: the table load moves to the entry as retail's, the registers stay ours), `long kind`
// with the material ternary 305, the tag-table local with the ternary 307.
// M3 lane GA r15 (46 fast trials with 10 peers MATCH; plain confirmed 300 -> 248, 308/308 insns, aligned 163 -> 43, masked
// 72 -> 30): GA's form, existing visible_entry_geometry(index) (unknown_044e90.h) + direct *(word const *)part + material ?: +
// *(word const *)(definition + 0x16) + if (record->active), matching retail lines 7-50 and 287-313 verbatim, all callee-saved
// registers (ebx=g_4e3b44->definition, ebp=handle, edi=material) and all 4 non-delta stack slots ([esp+0x10..0x1c]). Also
// tried (not kept): without visible_entry_geometry: TFB 305, TFB+no_sec 304, +part direct 315, +ternary 319, +word16 287,
// +active local 283, +dot jki 283, +entry late 287, +tag first 290/287, +tag table 280, active only 314, word16 only 310,
// record->active only 314, mat+active 314, mat+word16 310, long kind combos 312/311/308/279, vec helpers 302, member pos
// 337 (breaks callers), first/second TFB 303, nested if 281, override via material 284 (breaks callers), direct+word16 only
// 291; with visible_entry_geometry: alone 277 (breaks callers), +part 313 (breaks callers), +mat 313 (breaks callers),
// +word16 272, +record->active 287, +long kind check 251 (aligned 38), keeping word/long kind local 192/194 (1062/1060 B,
// 303 insns, aligned 99/98, wrong part/kind spill), dot jki 248, active local 248, entry before geom 248.
// Review: kept only the existing visible_entry_geometry(index) and the word read of definition + 0x16, with the `kind` local,
// main's material lines and context->active: 189 (1065/1089 B; 0x44ac0, 0x44c90 and 0x402b0 MATCH). The kind re-reads and
// record->active are second reads (match-only) and the ternary changed nothing; byte reads of +0x16 on this form give 277
// and 0x44ac0, 0x44c90 and 0x402b0 stop matching.
// M3 lane 13 r161 (fast with 0x44ac0, 0x44c90 and 0x402b0, MATCH throughout; control 189): the 24 missing bytes and
// the 12-byte frame gap are the delta vector: retail keeps it in three stack cells across the 0x40e10 call (each
// component stored as it is computed, read back for the depth), as if that call clobbered every xmm register; ours
// keeps it in xmm0-2. Diagnostics, reverted: delta in memory 156 at 1089 B (first difference +0x47); 0x40e10 built
// without link-time code generation gives the same 156 at 1089 B but 0x40e10 itself then differs (3), so that is not
// retail's form. Forms: the kind word read through `part` at every use, no local 272 (1077 B); the depth through the
// existing dot3f 189 (same code); the values copy tested on record->active 192 (1062 B), with the dot3f depth 192;
// delta declared at function scope 189 (same code).
// @retail 0x41040
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask)
{
    // Keeps index/part_index in edx/ecx and every other parameter on the stack, as retail's callers
    // pass them (0x44ac0 stops matching without this).
    part_index = *(long volatile *)&part_index;
    index = *(short volatile *)&index;
    (void)&group; (void)&weight; (void)&context; (void)&material_override;
    (void)&force; (void)&and_mask; (void)&or_mask;
    s_44940_entry const *entry = &g_4ba138[index];
    byte const *geometry = visible_entry_geometry(index);
    byte const *part = *(byte **)(geometry + 4) + part_index * 0x48;
    dword handle = (((dword)weight << 16) | (word)index) << 8 | part_index;
    real priority = weight * context->priority * g_4670e4 * 4.0f;
    word kind = *(word const *)part;
    if (kind == 0 || kind == 5 || context->material == NONE) return;
    long material = material_override;
    if (material == NONE) material = context->material;
    byte const *definition = g_4e3b44[material & 0xffff].bytes;
    long pass;
    function_1bf50(*(word const *)(definition + 0x3c), material, priority, &pass);
    byte const *groups = record_format_groups(material);
    dword flags = *(dword const *)(*(byte **)(groups + 4) + pass * 10 + 2);
    if (material_override != NONE)
    {
        groups = record_format_groups(context->material);
        dword source_flags = *(dword const *)(*(byte **)(groups + 4) + pass * 10 + 2);
        groups = record_format_groups(material_override);
        if ((source_flags & 0x8080) && !(source_flags & 0xa) &&
            (*(dword const *)(*(byte **)(groups + 4) + 2) & 0x200000)) return;
    }
    if ((flags & 0xffff7fff) || force)
    {
        bool first = (entry->unknown00 & 2) != 0;
        bool second = (entry->unknown00 & 4) && group != 3;
        if (first && second && group != 0)
            function_40e30(2, material, priority, *(word const *)(definition + 0x3c), kind,
                and_mask, or_mask, (t_record_fill)function_4cbf0, (dword)function_4d0b0, (void *)handle);
        function_40e30((short)function_40de0(group, first, second), material, priority,
            *(word const *)(definition + 0x3c), kind, and_mask, or_mask,
            (t_record_fill)function_4cbf0, (dword)function_4d0b0, (void *)handle);
    }
    if ((flags & 0x8000) && !force && group == 0)
    {
        s_41490_record *record = (s_41490_record *)function_1e2d0();
        if (record)
        {
            vector3f delta;
            delta.i = context->position.x - g_485618.position.x;
            delta.j = context->position.y - g_485618.position.y;
            delta.k = context->position.z - g_485618.position.z;
            record->tag = material;
            record->context = (void *)handle;
            record->type = function_40e10((*(word const *)(definition + 0x16) & 1) != 0, ((*(word const *)(definition + 0x16) >> 1) & 1) != 0);
            record->flags = 0;
            record->callback = function_41020;
            real depth = delta.k * g_485618.forward.k;
            depth += delta.j * g_485618.forward.j;
            depth += delta.i * g_485618.forward.i;
            record->depth = (0.0f - depth) - context->radius;
            record->position = context->position;
            record->active = context->active;
            if (context->active) memcpy((byte *)record + 0x10, context->values, sizeof(context->values));
        }
    }
}

// score 305 (was 352): logic matches retail block by block (no behaviour difference found); what still differs is register
// allocation: retail keeps tag in ebp and stage in edi and leaves pass in its stack slot, ours puts pass in ebp and tag in
// edi, so the 0x15d70 call setup, the case 18 pass*10 and the record_material reads differ; retail also loads the source
// fields as [esi+g_4ba138] and takes the address afterwards (lea), and reloads out->value10 for the 0x4cbb0 call.
// Tried: volatile result/tag (352), (void)&param (no effect), direct g_4ba138[index] field reads (412), pass/tag address
// tricks (none), statement-order permutations of result/out/key/index (305-360), key as long (341), group as switch or
// if-chain (switch gives sub/dec instead of test/cmp; the nested != 1 form gives retail's cmp), selection as byte with one
// movsx (closer to retail, score 340-361 only because the size changes: 1196 vs 1204 bytes).
// M3 lane 15 r147 (fast screen, 305): retail splits the tag parameter: its early uses (the 0x15d70 call, the case 18
// and 19 definition reads) come from ebp, the override in cases 18/19 is stored to the parameter's own slot and the
// material lookup reads it back from there; stage is in edi only up to the switch, pass stays in its slot. Ours keeps
// one tag web in edi to the material lookup, pass in ebp, stage in memory. Forms: the source fields for the 0x15d70
// call read as g_4ba138[index] with the pointer formed after the call 376 (1248 B, the pointer is never formed); the
// override in a separate `material_tag` local initialised from tag 316 (1204 B, retail's length: pass now stays in its
// slot as retail, but tag takes edi and stage ebp, and the copy is stored to tag's slot); (void)& on tag/pass/stage
// 305 (same code), with the separate local 316; the 0x4cbb0 call reading out->value10 as retail does 305 (same code:
// the stored value is forwarded); the selection switch's default listed with cases 1-3 365 (1164 B). Retail's primitive
// switch shares one block for cases 1-3 and the default (same register), and tests the 0x4cbb0 result as a byte.
// M3 lane GA r15 (34 fast trials with 8 peers MATCH; control 305 at 1196/1204 B, aligned 181, masked 80; body unchanged).
// `(byte)function_4cbb0(...) != 0` gives retail's `test al, al` (304, -1 under bar); dropping the `material` local and
// reading `out->value10` for 0x1cb20/0x4cbb0 puts `tag` in EBP (+0x1b), reproduces retail's 2-block primitive switch and
// its `out->value10` reload after 0x1cb20 (334, with `case 4/5` first 320, with 449e0-shape inline geometry helper 329 at
// +0x31 matching lines 8-22 verbatim incl. `lea esi, [esi+0x4ba138]`, with `material_tag` 340/351); existing
// visible_entry_geometry(index) 332 (1208 B), with material_tag 357 (aligned 140, masked 69); 449e0-shape helper alone
// 308 (1204 B); `(byte)mode * 0x132` first 305; value0c order swap 305; out late/in if 305; pass=0 after object 304;
// shared override ptr/ref helpers 305; primitive init-before 356, signed char/long 305; result in else 364; stage/tag
// locals 305/328; g_4ba138[index] in 15d70 args 376; out->value08 in 4cbb0 304/334. Left: splitting `tag` at switch
// without an extra stack slot so stage takes EDI and pass stays on stack.
// M3 lane GB r23, NOT kept at review (305 -> 96 needs the one-use `material_offset` local and the `(byte)` cast on
// 0x4cbb0's long result, both match-only; the allowed parts alone, `out->value10` reads and no `instance` local, give 340):
// naming `dword material_offset = (byte)mode * 0x132;` ahead of `record_material`, dropping the
// `material` local to read `*(long *)((byte *)out->value10 + 0x100)` for 0x1cb20/0x4cbb0 (as siblings 0x4dfa0/0x4de20
// do), testing `(byte)function_4cbb0(...) != 0`, and dropping the `bool instance` local in favour of
// `(bool)((source->unknown00 >> 12) & 1)` at both uses frees `ebx`/`edi` after `switch (stage)`, recovering retail's
// exact register split (`ebp=tag->out`, `edi=stage->material`, `ebx=mode->g_4e3b44`, `pass` in `[esp+0x24]`), retail's
// `mov byte [esp+0x34], al; push eax` CSE across 0x44940, the 2-block primitive switch, the `out->value10` reload after
// 0x1cb20, and `test al, al; setne al` (305 -> 96, 1200/1204 B, al 205 -> 41, mk 112 -> 36, all 10 peers MATCH).
// Still differs: `+0x1b` `add esi` vs retail `mov [esi+g_4ba138]; lea esi` (4 B, from retail inlining 0x449e0),
// `imul edi, 0x132` scheduled before `record_format_groups` finishes instead of after, `out->value0c` operand order
// (`source->tag` before `source->flags`), and the 2-block primitive switch order (`0x11` before `0x10`).
// M3 GB r23 (77 real + 1 diag fast trials across 7 batches; plain confirmed 305 -> 96, 1200/1204 B, +0x1b, all 10 peers
// MATCH and 0x41040 unchanged at 189):
// Match-pursuit lead (not kept without a byte match: one-use `dword material_index = out->unknown04` + local `entry`
// lookup for inlined 0x449e0): reaches 20 at exact 1204/1204 B, `+0x244` (`al=12, mk=8`, matching 580+ bytes verbatim
// and leaving ONLY `out->value0c` load order and the 2-block primitive switch order); `material_offset` + `entry` block
// reaches 38 (with `bool instance` after 44940) or 140 (without `(bool)` CSE); `out->unknown04` index without `entry`
// block reaches 80 (1200 B). Also tried on the 20/96 base (all stayed 20 or worse): primitive switch case orders
// (default top/middle/sep breaks/54-321/signed char/inline return helper/1cb20 in cases/init-before-switch/long primitive)
// 20-359; `out->value0c` spellings (`(flags & 0x3fe00)>>9`, `* 2048`, `% 512`, `(word)tag`, bitfield view, temp locals,
// zero-or-or, inline pack helper, byte-offset ptrs, `if/else if`) 20-202; `visible_entry_geometry(index)` 182; `union`
// 4cbb0 215 (1180 B, `sub esp, 0xc`); `out->unknown04` inline 344; `bool instance` kept 182-207; `tag`/`pass` overrides
// (`material_tag`, `original_tag`, `&tag` diag, shared 18/19, `(void)&`, `key` long, case reorder) 305-390.
// @retail 0x4cbf0
bool __stdcall function_4cbf0(long tag, long context, long pass, long stage,
    long entry_index, long handle, void *record)
{
    bool result = false;
    s_sort_record *out = (s_sort_record *)record;
    short key = handle & 0xff;
    short index = (short)(handle >> 8);
    s_44940_entry *source = &g_4ba138[index];
    long mode = function_15d70(tag, stage, pass, source->tag != NONE,
        (bool)((source->unknown00 >> 10) & 1));
    switch (stage)
    {
    case 3:
        switch (source->unknown00 >> 29)
        {
        case 0: mode = 0; break;
        case 1: mode = 1; break;
        case 2: mode = 2; break;
        case 3: mode = 3; break;
        case 4: mode = 4; break;
        default: mode = 3; break;
        }
        break;
    case 18:
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (!(definition[0x16] & 4))
        {
            byte *groups = record_format_groups(tag);
            byte *pass_entry = *(byte **)(groups + 4) + pass * 10;
            tag = *(long *)(g_485a80 + 0xd0);
            pass = 0;
            byte *object = *(byte **)source->unknown10;
            if (object)
            {
                if (!(*(dword *)(pass_entry + 2) & 0x8000)) mode = object[0x76] >= 0xff;
                else mode = 2;
            }
        }
        break;
    }
    case 19:
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (!(definition[0x16] & 4))
        {
            tag = *(long *)(g_485a80 + 0xd0);
            pass = 0;
            byte *object = *(byte **)source->unknown10;
            if (object && g_4b9f5c != NONE && !g_4b9f88 &&
                (g_4b9f78 > g_4b9f7c ? g_4b9f78 : g_4b9f7c) > 0.0f && object[0x76] < 0xff)
                mode = 0;
        }
        break;
    }
    case 16:
    {
        bool enabled = g_4ba004 && !g_485a75 && !g_485a76;
        dword bits = source->unknown00;
        if (bits & 0x80)
        {
            if (bits & 0x100) mode = enabled ? NONE : 1;
            else mode = enabled ? 2 : 0;
        }
        else
            mode = NONE;
        break;
    }
    }
    out->unknown04 = (byte)mode;
    if (mode != NONE)
    {
        if (source->flags >> 29)
        {
            if (source->flags >> 29 != 1)
            {
                out->group = 2;
                out->value0c = ((source->flags >> 9) & 0x1ff) | ((source->tag & 0xffff) << 11);
            }
            else
            {
                out->group = 1;
                out->value0c = ((source->flags >> 9) & 0x1ff) | ((source->tag & 0xffff) << 11);
            }
        }
        else
            out->group = 1;
        bool instance = (bool)((source->unknown00 >> 12) & 1);
        byte *section = (byte *)function_44940(index, instance);
        byte *geometry;
        if (source->tag != NONE)
        {
            byte *definition = g_4e3b44[source->tag & 0xffff].bytes;
            byte *sections = *(byte **)(definition + 0x28);
            geometry = *(byte **)(sections + ((source->flags >> 9) & 0x1ff) * 0x5c + 0x34);
        }
        else
        {
            byte *structure = (byte *)g_4e0348;
            if (!instance)
                geometry = *(byte **)(*(byte **)(structure + 0xa0) + ((source->flags >> 9) & 0x1ff) * 0xb0 + 0x50);
            else
            {
                short definition_index = *(short *)(*(byte **)(structure + 0x144) +
                    ((source->flags >> 18) & 0x7ff) * 0x58 + 0x34);
                geometry = *(byte **)(*(byte **)(structure + 0x13c) + definition_index * 0xc8 + 0x50);
            }
        }
        byte *geometry_entry = *(byte **)(geometry + 4) + (short)key * 72;
        byte *material = record_material(tag, pass, stage, entry_index) + (byte)mode * 0x132;
        out->value10 = (dword)material;
        word primitive = *(word *)geometry_entry;
        byte selection;
        switch (primitive)
        {
        case 1:
        case 2:
        case 3:
            selection = section[0x10];
            break;
        case 4:
        case 5:
            selection = section[0x11];
            break;
        default:
            selection = section[0x10];
            break;
        }
        long shader = function_1cb20(*(word *)(section + 0x14), (signed char)selection, *(long *)(material + 0x100));
        out->value08 = (word)shader;
        out->unknown14 = *(dword *)source->unknown10;
        result = function_4cbb0(*(long *)(material + 0x100), (word)shader) != 0;
        if (stage == 5)
            result = (bool)((source->unknown00 >> 13) & 1);
    }
    return result;
}

#if 0 // Activation changes matched rendering helpers.
struct s_projection_copy_3bd00
{
    transform4x3f transform;
    transform4x3f inverse;
    byte unknown68[0xc0 - 0x68];
};
void function_14ac0(void);
void function_12fa0(byte const *camera, real const *projection, bool scaled, long mode);
void function_3ede0(void);

// Retail 0x3bd00
bool __stdcall function_3bd00(real height, dword color)
{
    long target = g_4858b8;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    __declspec(align(8)) s_projection_copy_3bd00 saved_projection = *(s_projection_copy_3bd00 const *)g_48568c;
    __declspec(align(8)) s_projection_copy_3bd00 projection = saved_projection;
    s_frame_camera saved_camera = g_485618;
    s_frame_camera camera = saved_camera;
    g_4858b8 = 21;
    function_14bc0(21, 0, false);
    if (g_485607 && !g_4858b8) function_14ac0();
    D3DDevice_Clear(0, 0, 0xf0, color, 1.0f, 0);
    projection.transform.up.i = 0.0f - projection.transform.up.i;
    projection.transform.up.j = 0.0f - projection.transform.up.j;
    projection.transform.up.k = 0.0f - projection.transform.up.k;
    projection.transform.position.z = height * 2.0f - projection.transform.position.z;
    camera.field_30 = 0;
    camera.field_32 = 0;
    camera.field_34 = 256;
    camera.field_36 = 512;
    function_141590(&projection.transform, &projection.inverse);
    function_12fa0((byte const *)&camera, (real *)&projection, false, 21);
    function_3ede0();
    function_14bc0((short)target, 0, true);
    g_485618 = saved_camera;
    memcpy(g_48568c, &saved_projection, sizeof(saved_projection));
    g_4858b8 = target;
    function_12fa0((byte const *)&saved_camera, (real *)&saved_projection, false, target);
    return true;
}
#endif


// Disabled: passing the shared source-list address violates the shared-global rule.
#if 0
// Retail 0x40e30
void function_40e30(short group, long tag, real distance, long level, short kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context)
{
    if (group != NONE && function_1bf50(level, tag, distance, &level))
        function_3b9d0(&g_4c1a48[group], tag, level, and_mask, or_mask, fill, value, context, 0);
}
#endif



// Disabled: retail passes shared render-table and reset-state addresses; their canonical declarations remain split.
#if 0
struct s_render_transform_record;
struct s_primitive_definition;
struct s_primitive_header;
struct s_24490_definition;
struct s_1c8c0_stream;
void function_1c0d0(long tag, long stage);
void function_4d830(s_render_transform_record const *record);
void function_4d7c0(dword const *record, word const *kind);
void function_24040(long key, long slot, s_24490_definition *definition);
void *function_24490(long key, s_24490_definition *definition, long entry);
s_primitive_header *function_4dc60(s_primitive_definition *, long, bool, long);
void function_15c90(long stage, long index, long element);
bool function_1caa0(long, byte const *, word const *, byte const *,
    s_1c8c0_stream const *, s_1c8c0_stream const *, s_1c8c0_stream const *);
void function_4d9c0(byte const *, long, byte, short, long, long, long, long);
void __stdcall function_4d640(short, long, bool);
void function_4d720(short, long);
extern long g_485a60;

// Retail 0x4d0b0
void __stdcall function_4d0b0(long tag, long first, long second, long mode,
    long packed, long fourth, void *record_address)
{
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    long part_index = packed & 0xff;
    short index = (short)(packed >> 8);
    byte value = (byte)(packed >> 24);
    *(long *)(g_485b48 + 8) = tag;
    if ((mode == 18 || mode == 19) && !(definition[0x16] & 4))
    {
        tag = *(long *)(g_485a80 + 0xd0);
        second = 0;
    }
    bool compound = (definition[0x16] & 1) != 0;
    s_44940_entry *entry = &g_4ba138[index];
    if ((entry->unknown00 & 0x4000) && mode == 16) return;
    long section = (entry->flags >> 9) & 0x1ff;
    long primitive = (entry->flags >> 18) & 0x7ff;
    byte *geometry;
    if (entry->tag != NONE)
        geometry = *(byte **)(*(byte **)(g_4e3b44[entry->tag & 0xffff].bytes + 0x28) + section * 0x5c + 0x34);
    else if (!(entry->unknown00 & 0x1000))
        geometry = *(byte **)(*(byte **)((byte *)g_4e0348 + 0xa0) + section * 0xb0 + 0x50);
    else
    {
        short cluster = *(short *)(*(byte **)((byte *)g_4e0348 + 0x144) + primitive * 0x58 + 0x34);
        geometry = *(byte **)(*(byte **)((byte *)g_4e0348 + 0x13c) + cluster * 0xc8 + 0x50);
    }
    byte *part = *(byte **)(geometry + 4) + part_index * 0x48;
    function_1c0d0(tag, mode);
    function_4d830((s_render_transform_record *)entry);
    function_4d7c0((dword *)entry, (word *)part);
    byte const *selection = (byte const *)function_44940(index, (entry->unknown00 & 0x1000) != 0);
    void *extra = 0, *stream = 0, *mask = 0;
    if (mode == 5 && entry->tag != NONE && !compound)
    {
        byte *model = g_4e3b44[entry->tag & 0xffff].bytes;
        if (*(long *)(model + 0x74) > 0)
        {
            s_24490_definition *resource = *(s_24490_definition **)(model + 0x78);
            long key = **(long **)entry->unknown10;
            function_24040(key, entry->flags & 15, resource);
            extra = function_24490(key, resource, section);
        }
    }
    bool setup = false;
    if (mode == 3 && (entry->unknown00 >> 29) != 3 && !compound)
    {
        long bitmap = NONE;
        long texture = NONE;
        byte *data = *(byte **)((byte *)g_4e0344 + 0x84);
        if (primitive != 0x7ff)
        {
            if (entry->tag != NONE)
            {
                if ((entry->unknown00 >> 29) != 2)
                {
                    byte *group = *(byte **)(data + 0x64) + primitive * 12;
                    byte *buffer = *(byte **)(data + 0x44) + *(word *)(group + 2) * 0x38;
                    long size = ((word *)*(byte **)(group + 8))[section] * 4;
                    if (buffer[0] & 1)
                    {
                        mask = *(void **)(*(byte **)(buffer + 0x34) + 4);
                        ((long *)mask)[1] = size;
                        ((long *)mask)[2] = size;
                    }
                    stream = function_4dc60((s_primitive_definition *)data, primitive, false, section);
                }
            }
            else
            {
                byte *group = *(byte **)(data + 0x54) + primitive * 12;
                byte *buffer = *(byte **)(data + 0x44) + *(word *)(group + 2) * 0x38;
                long size = **(word **)(group + 8) * 4;
                if (buffer[0] & 1)
                {
                    mask = *(void **)(*(byte **)(buffer + 0x34) + 4);
                    ((long *)mask)[1] = size;
                    ((long *)mask)[2] = size;
                }
                stream = function_4dc60((s_primitive_definition *)data, primitive, true, NONE);
                byte *mapping = *(byte **)(data + 0x4c) + primitive * 4;
                bitmap = *(short *)mapping;
                texture = *(signed char *)(mapping + 2);
            }
        }
        else if (entry->tag == NONE)
        {
            byte *mapping = *(byte **)(data + 0x2c) + section * 4;
            bitmap = *(short *)mapping;
            texture = *(signed char *)(mapping + 2);
        }
        if (texture != NONE)
        {
            function_15c90(3, texture, bitmap);
            setup = true;
        }
        g_485a60 = bitmap;
    }
    for (short i = 0; i < *(long *)(geometry + 0x38); ++i)
        _mm_prefetch((char *)(geometry + 0x3c + i * 32), _MM_HINT_T0);
    _mm_prefetch((char *)selection, _MM_HINT_T0);
    _mm_prefetch((char *)mask, _MM_HINT_T0);
    _mm_prefetch((char *)part, _MM_HINT_T0);
    _mm_prefetch((char *)geometry, _MM_HINT_T0);
    _mm_prefetch((char *)extra, _MM_HINT_T0);
    _mm_prefetch((char *)stream, _MM_HINT_T0);
    byte *record = (byte *)record_address;
    if (compound)
    {
        byte *references = *(byte **)(definition + 0x24);
        for (long i = 0; i < *(long *)(references + 4); ++i)
        {
            long bitmap_tag = *(long *)(*(byte **)(references + 8) + i * 12);
            if (bitmap_tag == NONE) continue;
            byte *bitmap_definition = g_4e3b44[bitmap_tag & 0xffff].bytes;
            for (long j = 0; j < *(long *)(bitmap_definition + 0x44); ++j)
            {
                s_bitmap_predict_view *bitmap = (s_bitmap_predict_view *)(*(byte **)(bitmap_definition + 0x48) + j * 0x74);
                if (bitmap) bitmap_predict_inline(bitmap, 14);
            }
        }
        if (*(dword *)(*(byte **)(record + 0x10) + 0x118) & 0x100000)
            function_3bea0();
    }
    long shader = *(long *)(*(byte **)(record + 0x10) + 0x100);
    if (function_1caa0(shader, selection, (word *)part, geometry,
        (s_1c8c0_stream *)extra, (s_1c8c0_stream *)mask, (s_1c8c0_stream *)stream))
    {
        function_4d9c0(part, mode, value, index, first, second, fourth, record[4]);
        function_4d640(index, mode, setup);
        bool filtered = (entry->unknown00 & 0x200) && !(part[2] & 8);
        bool strip = (entry->unknown00 & 0x40) && *(long *)(entry->unknown10 + 4) && *(long *)(geometry + 8);
        function_4daa0(filtered, strip, (s_4daa0_geometry *)geometry,
            (s_4daa0_part *)part, *(dword **)(entry->unknown10 + 4));
        function_4d720(index, mode);
    }
}
#endif



// Disabled: retail passes shared shader state addresses and needs the blocked 14b60/3bd00 interfaces.
bool __stdcall function_3c270(dword value);
dword function_1cbb0(long mode, dword index, long tag);
void function_151c0(real const *constants);
struct s_stream_description;
void function_1cda0(s_stream_description const *stream);
void function_1bd50(void *state);
void function_1cdd0(real const *bounds, byte flags);
void function_1bbf0(long tag, long first, long second, long third, long fourth, real scale);
bool __stdcall function_1c8a0(D3DPRIMITIVETYPE type, word const *indices, long count);
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil);
bool __stdcall function_3bd00(real height, dword color);

// score 74 (was 150): logic walked against retail, no logic or behaviour difference (calls, argument order, 0x7c==0 test,
// 0x100000 and >>19 flags, 12de70/3c270 order, both loops match). What differs is the frame: retail has none (push ebp after
// the first test, push ebx/esi/edi after the second, three epilogues, spills in the render/clear argument slots [esp+0x14]/
// [esp+0x18]); ours reserves one local slot (push ecx) so everything is pushed up front, shifting the rest. Retail also
// tests 0x100000 as `shr eax,0x14; test al,1` (TEST_FIELD_BIT((flags >> 20) & 1) gives that form but scores 186 until
// the prologue matches). Tried: nested ifs instead of early returns (same bytes), separate `fog`/`reference_tag` locals,
// record_format_groups(tag) for the table walk, tag re-read from state at each use (183, no spill), tag initialised
// before the 3c270 call (150, but moves the load before the call: not retail's order).
// Permuter (25 min on r643): best 139, only with the tag read as a short (a behaviour change); not kept.
// The frame was a stack-parity pad for 0x3ca90's double local (gone with its tangent as a real: 136); with the
// prologue retail's, the 0x100000 test as (bool)((flags >> 20) & 1) gives retail's shr/test al,1 (74). Left: the clear
// call's flags go in eax in retail (BLOCKED-BY 0x14b60, a stack-convention stub; with its real body this function gets
// a parity pad again, 194), and that 3-byte shift moves both loop pads; the outer loop adds the spilled i * 12 as a
// memory operand where ours loads it into ecx. Tried: the overlay bit in its own local in place of render (same code)
// or tested at its use 220; the depth argument as 0 (same code); a copied tag for 0x1bbf0 (same code); the element
// read as i * 12 + base or as a long array with [i * 3] (74, same code); the NONE test as an if around the inner loop,
// the bitmap pointer typed at its definition, the second tag read at both uses (all 74, same code).
// @retail 0x3c3c0
bool __stdcall function_3c3c0(bool render, bool clear)
{
    byte *structure = (byte *)g_4e0348;
    if (*(long *)(structure + 0x214) <= 0) return true;
    byte *state = *(byte **)(structure + 0x218);
    if (*(real *)(state + 0x7c) == 0.0f || !render) return true;
    function_3ca90(state);
    long tag;
    if (function_3c270((dword)state) && function_12de70((s_geometry_block_info *)(state + 0x10), 3) && (tag = *(long *)(state + 4)) != NONE)
    {
        s_tag_instance *instance = g_4e3b44 + (short)tag;
        byte *material_reference;
        switch (*(dword *)instance->unknown00)
        {
        case 0x5052544d:
        case 0x70727433:
            material_reference = (byte *)((c_record_reference_view *)function_137bd0(tag))->reference();
            break;
        default:
            material_reference = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
            break;
        }
        byte *material = g_4e3b44[*(long *)material_reference & 0xffff].bytes;
        byte *table = *(byte **)(material + 0x5c);
        long first = **(word **)(table + 4) & 0x1ff;
        long second = ((word *)(*(byte **)(table + 0xc) + 6))[first] & 0x1ff;
        long shader_tag = *(long *)(*(byte **)(table + 0x14) + second * 10 + 4);
        byte *shader = *(byte **)(*(byte **)(g_4e3b44[shader_tag & 0xffff].bytes + 0x20) + 4);
        long shader_index = *(long *)(shader + 0x100);
        dword flags = *(dword *)(shader + 0x118);
        render = (bool)((flags >> 19) & 1);
        if ((bool)((flags >> 20) & 1)) function_3bea0();
        if (render) function_3bd00(*(real *)(state + 0x84), pack_color3f((color3f const *)(state + 0x58)));
        if (clear) function_14b60(0x10, 0xffffff, 0.0f, 0);
        byte *geometry = *(byte **)(state + 0xc);
        function_1cbb0(1, 1, shader_index);
        function_151c0((real const *)g_4c1b38);
        function_1cda0(*(s_stream_description const **)(geometry + 0x3c));
        function_1c710(g_51f0f0);
        memset(g_4c1a90, 0, 0x78);
        function_1bd50(g_4c1a90);
        function_1cdd0(0, 0);
        function_1bbf0(tag, 0, 3, 0, 0, 100000.0f);
        function_12d50(false, 0, 0);
        function_3bf00(state);
        function_1c8a0((D3DPRIMITIVETYPE)8, *(word const **)(geometry + 0x24), *(long *)(geometry + 0x20));
    }
    tag = *(long *)(state + 4);
    if (tag != NONE)
    {
        byte *references = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
        for (long i = 0; i < *(long *)(references + 4); ++i)
        {
            long bitmap_tag = *(long *)(*(byte **)(references + 8) + i * 12);
            if (bitmap_tag == NONE) continue;
            byte *definition = g_4e3b44[bitmap_tag & 0xffff].bytes;
            for (long j = 0; j < *(long *)(definition + 0x44); ++j)
            {
                byte *bitmap = *(byte **)(definition + 0x48) + j * 0x74;
                if (bitmap) function_3bcb0((s_bitmap_data *)bitmap);
            }
        }
    }
    return true;
}




// @retail 0x40e30
void function_40e30(short group, long tag, real distance, long level, short kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context)
{
    long const *level_reference = &level;
    short const *kind_reference = &kind;
    (void)*kind_reference;
    if (group != NONE && function_1bf50(*level_reference, tag, distance, (long *)&distance))
        function_3b9d0(&g_4c1a48[group], tag, *(long *)&distance, and_mask, or_mask, fill, value, context, 0);
}

s_material_constants_storage g_material_constants_storage;

s_material_shader_storage g_material_shader_storage;

struct s_render_transform_record;
struct s_primitive_definition;
struct s_primitive_header;
struct s_24490_definition;
struct s_1c8c0_stream;
void function_1c0d0(long tag, long stage);
void function_4d830(s_render_transform_record const *record);
void function_4d7c0(dword const *record, word const *kind);
struct s_surface_size;
void function_024040(long key, s_surface_size const *size, long index);
void *function_24490(long key, s_24490_definition *definition, long entry);
s_primitive_header *function_4dc60(s_primitive_definition *, long, bool, long);
void function_015c90(long stage, long index);
bool function_1caa0(long, byte const *, word const *, byte const *,
    s_1c8c0_stream const *, s_1c8c0_stream const *, s_1c8c0_stream const *);
void function_4d9c0(byte const *, long, byte, short, long, long, long, long);
void __stdcall function_4d640(short, long, bool);
void function_4d720(short, long);
extern long g_485a60;


// score 370 (was 380): the two flags passed to 0x4daa0 were swapped (behaviour fix from lane GB r23, checked against
// retail at review): retail 0x4daa0 tests AL as `filtered` (the mask path) and CL as `strip`, and retail 0x4d0b0 puts the
// 0x40 && mask && subparts test in AL and the 0x200 test in CL.
// Before (380, was 499): size equals retail (1418); what still differs is register allocation and frame layout.
// Retail keeps mode in edi and the unextended packed >> 8 in esi, tag in edx and g_4e3b44 in ebx through the prologue,
// loads tag before packed, and puts value at [esp+0x13], compound at [esp+0x24], part_index at [esp+0x34].
// Section/primitive are re-read from entry->flags at every use (no locals): that took the score from 489 to 406.
// Retail passes the compound flag (its stack slot [esp+0x24]) as 4d640's third argument; the texture block's byte
// store at [esp+0x58] (0, then 1 after function_015c90) is never read, so the draft's `setup` was dropped.
// Tried: short primitive (489) vs long primitive (502) as locals, both locals replaced by inline reads (406); decode order of part_index/index/
// value/definition (391 to 411, definition last kept: 393); long index with (short) casts (393, same bytes);
// (byte)packed or short part_index (401, 18 bytes short); TEST_FIELD_BIT on the 0x100000 test (389) and on the
// 0x200 test (380, kept) or both (397); *(long*)(g_485b48+8) store moved first (same).
// Walk r719 (b2O), statement by statement against retail 4d0b0-4d637: no logic or behaviour difference found (decode, the
// mode 18/19 tag/second override with the global store of the ORIGINAL tag, 0x4000 early return, the three geometry
// sources, 1c0d0/4d830/4d7c0/44940 order, mode 5 extra, mode 3 mask/stream/bitmap/texture paths incl. the >>29 == 2 and
// 0x7ff cases, prefetch order, compound walk, 1caa0 stack order, 4d9c0/4d640/4daa0/4d720 arguments all agree, but the 0x4daa0 flags were swapped, see above). Retail facts:
// [esp+0x58] is packed's own incoming slot (a byte 0, then 1 after 15c90, is stored there and never read; the compound
// walk then reuses it as the bitmap offset), so the omitted `setup` is behaviourally a no-op; 4d640 reads its flag as a
// byte while retail pushes the 4-byte slot of compound (ours does too); part_index is read back with movsx word (the
// slot is stored as a dword), values 0..255 so no behaviour change. 0x100000 test: retail `shr edx,0x14; test dl,1` but
// every spelling (TEST_FIELD_BIT, bool local, (x >> 20) & 1) folds to `test [mem],0x100000` here (380 in all four).
// Also tried: definition first then part_index/value/index (402), index kept as long raw_index + short (393), value first (404).
// M3 lane 3 r309 (H2M_FAST; control 380). Retail decodes part_index as `xor ecx, ecx; mov cl, al; ... and ecx, 0xff`
// and reads it back as a short (`movsx eax, word ptr [esp + 0x34]`): `short part_index = (short)(packed & 0xff);` gives
// that exact sequence but 417 (part_index stays in EDI where retail spills it); with it, the definition lookup first
// 414, the definition and the tag store first 414, definition/part/value/index 406, definition/index/part/value 406.
// Other spellings: `short part_index = (byte)packed;` 401 (1399 B, `movzx di, al`), `(byte)packed & 0xff` into a long
// 401 (1400 B, `movzx ecx, al`), `(short)((byte)packed & 0xff)` 401 (1399 B). Lead: the short form plus whatever makes
// part_index spill.
// M3 lane GA r15 (49 fast trials, control 380, all 13 peers MATCH; plain verified 360, all 35 file peers + callers/callees MATCH):
// Not kept at review, 380 -> 360 (1399/1418 B, 418/420 insns, aligned 343 -> 299, masked 333 -> 277): short part_index = (short)(packed & 0xff) (retail's
// xor/mov cl,al/and 0xff/movsx word), existing visible_entry_geometry(index), and 2-caller prepare_mask_stream(buffer, size) (retail's NULL
// init + unconditional mask store in both mode==3 arms). Single-change fast scores: visible_entry_geometry 424 (aligned 243),
// prepare_mask_stream 413 (aligned 237), short part_index 417 (aligned 200), definition first 405, direct shader 378, arm1+arm2 texture first
// 349, 0x100000 TEST_FIELD_BIT/shift 380/380, short i prefetch 380, bit-4000 nested 380, compound/prefetch/tail order variants 380-433.
// Leads (not kept without a byte match: statement-order move or extra local): moving definition first on the 360 body gives 344 (1399 B,
// aligned 284, masked 99; direct shader 344 at 1398 B, aligned 261, masked 94; arm2 texture first 341; arm1+arm2 texture first 402 tail-merged;
// size before buffer 344); adding a 32-bit entry_index local gives 381/382 at exact 1418 B with first diff +0x47 (lines 1-22 byte-for-byte
// match) and masked 86; long index without extra local 407-429.
// Review: prepare_mask_stream is a new helper used only by this function (two sites), match-only; without it the
// short part_index and visible_entry_geometry(index) give 420, so the body stays at 380.
// M3 lane GB r23 (53 fast trials, control 380; plain confirmed 291, all 13 peers + file peers MATCH):
// Tried 380 -> 291 (-89); at review only (1) is kept (380 -> 370): the 291 needs (2), a statement order, and (5), new
// locals (both match-only), and the allowed parts (1), (3), (4), (6), (7) together give 421.
// (1) fixed swapped arguments to function_4daa0 (retail 0x4daa0 tests al for filtered and cl for strip;
// retail 0x4d0b0 computes strip into cl first and filtered into al second; fixing HEAD alone drops 380 -> 370),
// (2) definition loaded before packed decode (matching retail lines 14-21 before line 22; without it 307),
// (3) short part_index = (short)(packed & 0xff) (without it 426), (4) existing visible_entry_geometry(index) (without it 413),
// (5) typed long *stream_mask = NULL before if (buffer[0] & 1) then mask = stream_mask in both mode==3 arms (matching retail's
// xor/test/unconditional mask store without a 1-function helper; mask = NULL 377, omitted 393, ternary 402, else NULL 384),
// (6) (bool)((*(dword *)(*(byte **)(record + 0x10) + 0x118) >> 20) & 1) (same idiom as 0x3c3c0; emits retail's shr 0x14/test dl,1; without it 345),
// (7) inlined shader argument *(long *)(*(byte **)(record + 0x10) + 0x100) into function_1caa0 (without it 328).
// What still differs: tag/g_4e3b44 still swap ebx/ecx and value takes [esp+0x58] instead of [esp+0x13], shifting part..compound down 4 bytes
// and spilling bitmap to [esp+0x24] instead of keeping bitmap in ebx (rematerializing mov ebx, -1 after 0x4dc60); compound loop swaps ebx/edi.
// Also tried on 291 base: size before buffer + arm2 texture before bitmap (291, aligned 122/masked 122, same score),
// s_sort_record *record (291), ref ptr (291), texture decl before bitmap (291), tag store first (291),
// stream/mask decl swap (290), bool 44940 arg (291), mode==3 cond reorder (295), byte compound (302),
// hoisted data (389), compound moved after entry/guard (415/414), index/value/part decode order (417),
// entry_index local (386, +0x47, duplicates g_4ba138[index] lookup; without visible_entry_geometry 398, masked 104),
// entry_index + byte compound (390, +0x7d first 125 B exact; without visible_entry_geometry 392),
// redundant bitmap = NONE after arm1 0x4dc60 (379, keeps bitmap in ebx + fixes compound loop ebx/edi, aligned 95, shifted by 1 insn at 0x15c90).
// @retail 0x4d0b0
void __stdcall function_4d0b0(long tag, long first, long second, long mode,
    long fourth, long packed, void *record_address)
{
    long part_index = packed & 0xff;
    short index = (short)(packed >> 8);
    byte value = (byte)(packed >> 24);
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    *(long *)(g_485b48 + 8) = tag;
    if ((mode == 18 || mode == 19) && !(definition[0x16] & 4))
    {
        tag = *(long *)(g_485a80 + 0xd0);
        second = 0;
    }
    bool compound = (definition[0x16] & 1) != 0;
    s_44940_entry *entry = &g_4ba138[index];
    if ((entry->unknown00 & 0x4000) && mode == 16) return;
    byte *geometry;
    if (entry->tag != NONE)
        geometry = *(byte **)(*(byte **)(g_4e3b44[entry->tag & 0xffff].bytes + 0x28) + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
    else if (!TEST_FIELD_BIT((entry->unknown00 >> 12) & 1))
        geometry = *(byte **)(*(byte **)((byte *)g_4e0348 + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0 + 0x50);
    else
    {
        short cluster = *(short *)(*(byte **)((byte *)g_4e0348 + 0x144) + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
        geometry = *(byte **)(*(byte **)((byte *)g_4e0348 + 0x13c) + cluster * 0xc8 + 0x50);
    }
    byte *part = *(byte **)(geometry + 4) + part_index * 0x48;
    function_1c0d0(tag, mode);
    function_4d830((s_render_transform_record *)entry);
    function_4d7c0((dword *)entry, (word *)part);
    byte const *selection = (byte const *)function_44940(index, (entry->unknown00 & 0x1000) != 0);
    void *extra = 0;
    if (mode == 5 && entry->tag != NONE && !compound)
    {
        byte *model = g_4e3b44[entry->tag & 0xffff].bytes;
        if (*(long *)(model + 0x74) > 0)
        {
            s_24490_definition *resource = *(s_24490_definition **)(model + 0x78);
            long key = **(long **)entry->unknown10;
            function_024040(key, (s_surface_size const *)resource, entry->flags & 15);
            extra = function_24490(key, resource, ((entry->flags >> 9) & 0x1ff));
        }
    }
    void *mask = 0, *stream = 0;
    if (mode == 3 && (entry->unknown00 >> 29) != 3 && !compound)
    {
        long bitmap = NONE;
        long texture = NONE;
        if (((entry->flags >> 18) & 0x7ff) != 0x7ff)
        {
            if (entry->tag != NONE)
            {
                if ((entry->unknown00 >> 29) != 2)
                {
                    byte *data = *(byte **)((byte *)g_4e0344 + 0x84);
                    byte *group = *(byte **)(data + 0x64) + ((entry->flags >> 18) & 0x7ff) * 12;
                    byte *buffer = *(byte **)(data + 0x44) + *(word *)(group + 2) * 0x38;
                    long size = ((word *)*(byte **)(group + 8))[((entry->flags >> 9) & 0x1ff)] * 4;
                    if (buffer[0] & 1)
                    {
                        mask = *(void **)(*(byte **)(buffer + 0x34) + 4);
                        ((long *)mask)[1] = size;
                        ((long *)mask)[2] = size;
                    }
                    stream = function_4dc60((s_primitive_definition *)data, ((entry->flags >> 18) & 0x7ff), false, ((entry->flags >> 9) & 0x1ff));
                }
            }
            else
            {
                byte *data = *(byte **)((byte *)g_4e0344 + 0x84);
                byte *group = *(byte **)(data + 0x54) + ((entry->flags >> 18) & 0x7ff) * 12;
                byte *buffer = *(byte **)(data + 0x44) + *(word *)(group + 2) * 0x38;
                long size = **(word **)(group + 8) * 4;
                if (buffer[0] & 1)
                {
                    mask = *(void **)(*(byte **)(buffer + 0x34) + 4);
                    ((long *)mask)[1] = size;
                    ((long *)mask)[2] = size;
                }
                stream = function_4dc60((s_primitive_definition *)data, ((entry->flags >> 18) & 0x7ff), true, NONE);
                byte *mapping = *(byte **)(data + 0x4c) + ((entry->flags >> 18) & 0x7ff) * 4;
                bitmap = *(short *)mapping;
                texture = *(signed char *)(mapping + 2);
            }
        }
        else if (entry->tag == NONE)
        {
            byte *data = *(byte **)((byte *)g_4e0344 + 0x84);
            byte *mapping = *(byte **)(data + 0x2c) + ((entry->flags >> 9) & 0x1ff) * 4;
            bitmap = *(short *)mapping;
            texture = *(signed char *)(mapping + 2);
        }
        if (texture != NONE)
            function_015c90(3, texture);
        g_485a60 = bitmap;
    }
    for (short i = 0; i < *(long *)(geometry + 0x38); ++i)
        _mm_prefetch((char *)(geometry + 0x3c + i * 32), _MM_HINT_T0);
    _mm_prefetch((char *)selection, _MM_HINT_T0);
    _mm_prefetch((char *)mask, _MM_HINT_T0);
    _mm_prefetch((char *)part, _MM_HINT_T0);
    _mm_prefetch((char *)geometry, _MM_HINT_T0);
    _mm_prefetch((char *)extra, _MM_HINT_T0);
    _mm_prefetch((char *)stream, _MM_HINT_T0);
    byte *record = (byte *)record_address;
    if (compound)
    {
        byte *references = *(byte **)(definition + 0x24);
        for (long i = 0; i < *(long *)(references + 4); ++i)
        {
            long bitmap_tag = *(long *)(*(byte **)(references + 8) + i * 12);
            if (bitmap_tag == NONE) continue;
            byte *bitmap_definition = g_4e3b44[bitmap_tag & 0xffff].bytes;
            for (long j = 0; j < *(long *)(bitmap_definition + 0x44); ++j)
            {
                s_bitmap_predict_view *bitmap = (s_bitmap_predict_view *)(*(byte **)(bitmap_definition + 0x48) + j * 0x74);
                if (bitmap) bitmap_predict_inline(bitmap, 14);
            }
        }
        if (*(dword *)(*(byte **)(record + 0x10) + 0x118) & 0x100000)
            function_3bea0();
    }
    long shader = *(long *)(*(byte **)(record + 0x10) + 0x100);
    if (function_1caa0(shader, selection, (word *)part, geometry,
        (s_1c8c0_stream *)extra, (s_1c8c0_stream *)mask, (s_1c8c0_stream *)stream))
    {
        function_4d9c0(part, mode, value, index, tag, second, fourth, record[4]);
        function_4d640(index, mode, compound);
        bool strip = TEST_FIELD_BIT((entry->unknown00 >> 9) & 1) && !(part[2] & 8);
        bool filtered = (entry->unknown00 & 0x40) && *(long *)(entry->unknown10 + 4) && *(long *)(geometry + 8);
        function_4daa0(filtered, strip, (s_4daa0_geometry *)geometry,
            (s_4daa0_part *)part, *(dword **)(entry->unknown10 + 4));
        function_4d720(index, mode);
    }
}
