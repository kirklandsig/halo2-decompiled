// @flags /O2 /arch:SSE /Ob1 /Gr
/* UNKNOWN_03BCB0.CPP: predicting one bitmap's texture (unknown_03bcb0.h);
   its own /Ob1 file because retail calls it out of line from 0x16e5e0 */

#include "unknown_11c920.h"
#include "unknown_03bcb0.h"
#include <xtl.h>
#include "globals.h"
#include "geometry_cache.h"

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
extern byte g_485b48[0x1fc0];
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

// @retail 0x441b0
void function_441b0(s_planar_camera_source const *source, s_planar_camera *state, byte const *view)
{
	memset(state, 0, sizeof(*state));
	state->disabled = false;
	state->matrix.left = source->horizontal;
	function_30bf0(&state->matrix.left);
	state->matrix.up.i = 0.0f - source->normal.i;
	state->matrix.up.j = 0.0f - source->normal.j;
	state->matrix.up.k = 0.0f - source->normal.k;
	function_30bf0(&state->matrix.up);
	matrix_cross(&state->matrix.left, &state->matrix.up, &state->matrix.forward);
	function_30bf0(&state->matrix.forward);
	state->matrix.position = source->origin;
	state->matrix.scale = 1.0f;
	function_141590(&state->matrix, &state->inverse);
	state->has_plane = true;
	state->offset = source->offset;
	state->plane.n = source->normal;
	state->plane.d = source->normal.k * source->origin.z + source->normal.j * source->origin.y + source->normal.i * source->origin.x + source->offset;
	*(bool *)state->unknown84 = true;
	state->depth = source->depth;
	box2f const *rectangle = (box2f const *)(view + 0x68);
	state->corners[0].x = rectangle->x0;
	state->corners[0].y = rectangle->y0;
	state->corners[1].x = rectangle->x1;
	state->corners[1].y = rectangle->y0;
	state->corners[2].x = rectangle->x1;
	state->corners[2].y = rectangle->y1;
	state->corners[3].x = rectangle->x0;
	state->corners[3].y = rectangle->y1;
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

struct s_record_sources
{
	dword unknown00;
	long count;
	dword flags;
	long first;
	dword unknown10;
	s_record_source *sources;
};

class c_type_4e7709;
c_type_4e7709 *function_137bd0(long tag_index);

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
	(void)&pass; (void)&first; (void)&second;
	byte *definition = g_4e3b44[tag & 0xffff].bytes;
	long result = NONE;
	byte *groups = record_format_groups(tag);
	long group = *(word *)(*(byte **)(groups + 4) + pass * 10) & 0x1ff;
	word range = (*(word **)(groups + 0xc))[group + stage];
	if (range > 0x1ff)
	{
		result = 0;
		switch (stage)
		{
		case 3: result = function_0226d0() ? NONE : 3; break;
		case 10: result = function_30cd0(second, first); break;
		case 11: result = function_30d10(*(word *)(definition + 0x3e) != 0, true, second); break;
		case 12: result = function_30da0(*(word *)(definition + 0x3e) != 0, true); break;
		case 13: result = function_30e00(*(word *)(definition + 0x3e) != 0); break;
		}
	}
	return result;
}

long function_4cbb0(long tag_index, dword block_index);

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
		out->unknown04 = 0;
		out->group = 4;
		out->subkey = 0xffff;
		out->value0c = handle;
		byte *material = record_material(tag, pass, stage, entry);
		out->value10 = (dword)material;
		out->value08 = 0;
		out->unknown14 = 0;
		return function_4cbb0(*(long *)(material + 0x100), 0) != 0;
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
		byte index = out->unknown04;
		out->group = 1;
		out->subkey = 0xffff;
		out->value0c = handle;
		byte *material = record_material(tag, pass, stage, entry) + index * 0x132;
		out->value10 = (dword)material;
		out->value08 = 1;
		out->unknown14 = 0;
		{ final_value = function_4cbb0(*(long *)(material + 0x100), 1) != 0; goto complete; }
	}
	{ final_value = false; goto complete; }
complete:
    return final_value;
}

// @retail 0x4e0d0
bool __stdcall function_4e0d0(long tag, long context, long pass, long stage, long entry, long handle, s_sort_record *out)
{
	(void)&tag; (void)&context; (void)&pass; (void)&stage;
	(void)&entry; (void)&handle; (void)&out;
	byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
	byte *blocks = *(byte **)(table + 0x14);
	byte *record = *(byte **)(table + 0x1c) + (((dword)handle >> 8) & 0x3fffff) * 24;
	long record_tag = (*(long **)((byte *)g_4e0350 + 0x37c))[(signed char)record[0] * 2 + 1];
	byte *definition = g_4e3b44[record_tag & 0xffff].bytes;
	byte *part = *(byte **)(definition + 0x14) + record[1] * 20;
	if (function_12de70((s_geometry_block_info *)(blocks + *(short *)(record + 6) * 0x2c), 3) && *(word *)(record + 0xe) > 0)
	{
		out->unknown04 = 0;
		out->group = 4;
		out->subkey = 0xffff;
		out->value0c = handle;
		byte *material = record_material(tag, pass, stage, entry);
		out->value10 = (dword)material;
		switch (part[4])
		{
		case 3: out->value08 = 7; break;
		case 4: out->value08 = 8; break;
		default: out->value08 = 0; break;
		}
		out->unknown14 = 0;
		return function_4cbb0(*(long *)(material + 0x100), out->value08) != 0;
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

s_record_sources g_4c1a48[3];

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

// @retail 0x3ba90
long function_3ba90(long bit, s_record_sources const *data)
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
long g_4858b0, g_48574c;
byte g_485a75, g_485a76;
extern byte g_4670bc;
extern byte g_485b48[0x1fc0];
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
bool function_1bf50(long tag, long level, real distance, long *out)
{
    long selected = 0;
    byte *groups = record_format_groups(tag);
    long count = *(long *)groups;
    if (g_4ba019)
        distance *= 2.0f;
    if (level != 5)
    {
        real threshold = g_485b28[level] * distance;
        for (selected = 0; selected < count; ++selected)
        {
            byte *entries = *(byte **)(record_format_groups(tag) + 4);
            if (threshold >= *(real *)(entries + selected * 10 + 6))
                break;
        }
    }
    *out = selected;
    if (selected < 0)
        *out = 0;
    else
        *out = selected > count - 1 ? count - 1 : selected;
    return *out < count;
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
        return count;
    }
    if (strip)
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

void function_40e30(short group, long tag, real distance, long level, word kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context);

struct s_cache_record;
s_cache_record *function_1e2d0(void);
long function_40e10(bool first, bool second);

struct s_frame_offset
{
    point3f position;
    vector3f forward;
    vector3f up;
};
extern s_frame_offset g_485618;

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

// @retail 0x41490
void function_41490(short group, long tag, short kind, real distance, t_record_fill fill,
    dword value, t_41490_callback callback, void *context, point3f const *position)
{
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    long pass;
    function_1bf50(tag, *(word *)(definition + 0x3c), distance, &pass);
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
            record->position = *position;
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
    double sine = sin(state[30]);
    direction[0] = (real)(cos(state[29]) * sine);
    direction[1] = (real)(sin(state[29]) * sine);
    direction[2] = (real)cos(state[30]);
    direction[3] = state[25];
    real mapped[4];
    mapped[0] = (real)((direction[0] + 1.0) * 0.5);
    mapped[1] = (real)((direction[1] + 1.0) * 0.5);
    mapped[2] = (real)((direction[2] + 1.0) * 0.5);
    mapped[3] = 0.4970000088214874f;
    D3DDevice_SetVertexShaderConstant(-73, mapped, 1);
    if (state[34] > 0.0001f && state[40] > 0.0001f && state[41] > 0.0001f &&
        state[41] - state[40] > 0.0001f)
    {
        double inverse = 1.0 / state[34];
        struct { real a, b; } local_low_high_record = { (real)(inverse * state[40] * 16777215.0), (real)(inverse * state[41] * 16777215.0) };
        direction[0] = 1.0f / (local_low_high_record.b - local_low_high_record.a);
        direction[1] = 0.0f - direction[0] * local_low_high_record.a;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
    }
    else
    {
        direction[0] = 0.0f;
        direction[1] = 0.0f;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
    }
    D3DDevice_SetVertexShaderConstant(-72, direction, 1);
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
struct s_shader_cache;
extern byte g_51f0f0[0x2d8];
extern byte *g_485a80;
extern long g_4858b8;
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);
void function_14bc0(short index, short element, bool use_depth);
void function_12d50(real const *projection, bool scaled, real *output);
point3f g_4c1b08[4];

// @retail 0x3bf20
long __stdcall function_3bf20(s_slot *slot, byte *data)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x14), 0);
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
    function_12d50(NULL, false, NULL);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    D3DDevice_SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 1);
    D3DDevice_SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    D3DDevice_SetRenderState(D3DRS_STENCILREF, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    D3DDevice_SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    D3DDevice_SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    D3DDevice_SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_ZERO);
    D3DDevice_SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    D3DDevice_SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    D3DDevice_Begin(D3DPT_QUADLIST);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(0, g_4c1b08[0].x, g_4c1b08[0].y, g_4c1b08[0].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(0, g_4c1b08[3].x, g_4c1b08[3].y, g_4c1b08[3].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(0, g_4c1b08[2].x, g_4c1b08[2].y, g_4c1b08[2].z, 1.0f);
    D3DDevice_SetVertexDataColor(9, 0xffffffff);
    D3DDevice_SetVertexData4f(0, g_4c1b08[1].x, g_4c1b08[1].y, g_4c1b08[1].z, 1.0f);
    D3DDevice_End();
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    return 0x4b000;
}



#include <math.h>
extern vector3f g_4b9dac;
s_3c9a0_matrix g_4c1b9c;

PRIVATE __forceinline void set_bump_component(long stage, D3DTEXTURESTAGESTATETYPE component, real value)
{
    D3DDevice_SetTextureStageState(stage, component, *(dword *)&value);
}

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
    real xs = (real)((double)trig.sine * axis->i);
    real ys = axis->j * trig.sine;
    real zs = axis->k * trig.sine;
    real inverse = 1.0f - trig.cosine;
    g_4c1b9c.scale = 1.0f;
    g_4c1b9c.forward.i = (1.0f - xx) * trig.cosine + xx;
    g_4c1b9c.forward.j = inverse * axis->i * axis->j + zs;
    g_4c1b9c.forward.k = inverse * axis->i * axis->k - ys;
    g_4c1b9c.left.i = inverse * axis->i * axis->j - zs;
    g_4c1b9c.left.j = (1.0f - yy) * trig.cosine + yy;
    g_4c1b9c.left.k = inverse * axis->k * axis->j + xs;
    g_4c1b9c.up.i = inverse * axis->i * axis->k + ys;
    g_4c1b9c.up.j = inverse * axis->k * axis->j - xs;
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



extern byte g_485ac2;
extern word g_485648, g_48564a, g_48564c, g_48564e;
real g_485640;
real g_4c1b38[3][4];
s_3c9a0_matrix g_4c1b68;

// @retail 0x3ca90
void function_3ca90(byte const *state)
{
	real angle = *(real const *)(state + 0x7c);
	if (angle == 0.0f) return;
	real height = *(real const *)(state + 0x84);
	real offset = *(real const *)(state + 0xa8);
	point3f camera = g_485618.position;
	real distance = camera.z - height;
	if (distance < 0.0f) distance = 0.0f - distance;
	if (distance < 0.0001f) distance = 0.0001f;
	vector3f forward = g_4b9dac;
	s_3c9a0_matrix basis;
	function_3c9a0(&forward, g_4687b0, &basis);
	basis.scale = distance;
	basis.position.x = camera.x;
	basis.position.y = camera.y;
	basis.position.z = height - distance * offset;
	g_4c1b68 = basis;
	real aspect = ((real)(short)g_48564e - (real)(short)g_48564a) / ((short)g_48564c - (short)g_485648);
	real tangent = (real)(tan((double)angle * 0.5f) * *(real const *)(state + 0x80));
	if (!(g_4e6948 && g_4e6948->flag && g_4e6948->index != NONE && g_4e6948->state == 3) && g_485ac2)
		aspect *= 1.5f;
	real half_angle = g_485640 * 0.5f;
	if (!(half_angle < 0.7806857824325562f)) half_angle = 0.7806857824325562f;
	real view_tangent = (real)(tan((double)half_angle) * aspect);
	real width_scale = view_tangent / tangent;
	if (width_scale <= 1.0f) width_scale = 1.0f;
	else if (width_scale > 1.0f) width_scale *= 1.13f;
	width_scale *= distance;
	g_4c1b38[0][0] = basis.forward.i * distance;
	g_4c1b38[0][1] = basis.left.i * width_scale;
	g_4c1b38[0][2] = basis.up.i;
	g_4c1b38[0][3] = basis.position.x;
	g_4c1b38[1][0] = basis.forward.j * distance;
	g_4c1b38[1][1] = basis.left.j * width_scale;
	g_4c1b38[1][2] = basis.up.j;
	g_4c1b38[1][3] = basis.position.y;
	g_4c1b38[2][0] = basis.forward.k * distance;
	g_4c1b38[2][1] = basis.left.k * width_scale;
	g_4c1b38[2][2] = basis.up.k;
	g_4c1b38[2][3] = basis.position.z;
	real near_x = (real)tan((double)angle * 0.04f * -12.0f);
	double tangent2 = tan((double)angle * 0.5f) * *(real const *)(state + 0x80);
	real near_y = (real)(sqrt((double)near_x * near_x + 1.0f) * tangent2);
	real far_x = *(real const *)(state + 0x88);
	real far_y = (real)(sqrt((double)far_x * far_x + 1.0f) * tangent2);
	point3f corners[4];
	for (long i = 0; i < 4; ++i)
	{
		real x = i < 2 ? near_x : far_x;
		real y = i == 0 ? 0.0f - near_y : i == 1 ? near_y : i == 2 ? far_y : 0.0f - far_y;
		corners[i].x = g_4c1b38[0][1] * y + g_4c1b38[0][0] * x + basis.up.i * 0.0f + camera.x;
		corners[i].y = g_4c1b38[1][1] * y + g_4c1b38[1][0] * x + basis.up.j * 0.0f + camera.y;
		corners[i].z = g_4c1b38[2][1] * y + g_4c1b38[2][0] * x + basis.up.k * 0.0f + basis.position.z;
	}
	memcpy(g_4c1b08, corners, sizeof(corners));
}

struct s_44940_entry
{
    dword unknown00;
    long tag;
    dword unknown08;
    dword flags;
    byte unknown10[0x10];
};
extern s_44940_entry g_4ba138[850];
void *function_44940(short index, bool instance);
long function_1cb20(long mode, dword index, long tag);
extern byte *g_485a80;
extern long g_4b9f5c;
extern bool g_4ba004;
extern byte g_485a75, g_485a76;
real g_4b9f78, g_4b9f7c;
byte g_4b9f88;

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

// @retail 0x41040
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask)
{
    part_index = *(long volatile *)&part_index;
    index = *(short volatile *)&index;
    (void)&group; (void)&weight; (void)&context; (void)&material_override;
    (void)&force; (void)&and_mask; (void)&or_mask;
    s_44940_entry const *entry = &g_4ba138[index];
    byte const *geometry;
    if (entry->tag != NONE)
    {
        byte const *definition = g_4e3b44[entry->tag & 0xffff].bytes;
        byte const *section = *(byte **)(definition + 0x28) + ((entry->flags >> 9) & 0x1ff) * 0x5c;
        geometry = *(byte **)(section + 0x34);
    }
    else
    {
        byte const *structure = (byte *)g_4e0348;
        byte const *section;
        if (!(entry->unknown00 & 0x1000))
            section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
        else
        {
            byte const *instance = *(byte **)(structure + 0x144) + ((entry->flags >> 18) & 0x7ff) * 0x58;
            section = *(byte **)(structure + 0x13c) + *(short const *)(instance + 0x34) * 0xc8;
        }
        geometry = *(byte **)(section + 0x50);
    }
    byte const *part = *(byte **)(geometry + 4) + part_index * 0x48;
    dword handle = (((dword)weight << 16) | (word)index) << 8 | part_index;
    real priority = weight * context->priority * g_4670e4 * 4.0f;
    word kind = *(word const *)part;
    if (kind == 0 || kind == 5 || context->material == NONE) return;
    long material = material_override == NONE ? context->material : material_override;
    byte const *definition = g_4e3b44[material & 0xffff].bytes;
    long pass;
    function_1bf50(material, *(word const *)(definition + 0x3c), priority, &pass);
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
        bool second = (entry->unknown00 & 4) != 0 && group != 3;
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
            record->type = function_40e10((bool)(definition[0x16] & 1), (bool)((definition[0x16] >> 1) & 1));
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

// @retail 0x4cbf0
bool __stdcall function_4cbf0(long tag, long context, long pass, long stage,
    long entry_index, long handle, void *record)
{
    s_sort_record *out = (s_sort_record *)record;
    tag = *(long volatile *)&tag;
    long key = (byte)handle;
    short index = (short)(handle >> 8);
    s_44940_entry *source = &g_4ba138[index];
    byte *object = *(byte **)source->unknown10;
    volatile bool result = false;
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
            if (object)
                mode = (*(dword *)(pass_entry + 2) & 0x8000) ? 2 : object[0x76] == 0xff;
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
            if (object && g_4b9f5c != NONE && !g_4b9f88 &&
                (g_4b9f78 > g_4b9f7c ? g_4b9f78 : g_4b9f7c) > 0.0f && object[0x76] < 0xff)
                mode = 0;
        }
        break;
    }
    case 16:
    {
        bool enabled = g_4ba004 && !g_485a75 && !g_485a76;
        if (!(source->unknown00 & 0x80)) mode = NONE;
        else if (source->unknown00 & 0x100) mode = enabled ? NONE : 1;
        else mode = enabled ? 2 : 0;
        break;
    }
    }
    out->unknown04 = (byte)mode;
    if (mode != NONE)
    {
        long kind = source->flags >> 29;
        if (kind)
        {
            out->group = kind == 1 ? 1 : 2;
            out->value0c = ((source->flags >> 9) & 0x1ff) | ((source->tag & 0xffff) << 11);
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
        long selection;
        switch (primitive)
        {
        case 1: selection = (signed char)section[0x10]; break;
        case 2: selection = (signed char)section[0x10]; break;
        case 3: selection = (signed char)section[0x10]; break;
        case 4: selection = (signed char)section[0x11]; break;
        case 5: selection = (signed char)section[0x11]; break;
        default: selection = (signed char)section[0x10]; break;
        }
        long shader = function_1cb20(*(word *)(section + 0x14), selection, *(long *)(material + 0x100));
        out->value08 = (word)shader;
        out->unknown14 = *(dword *)source->unknown10;
        result = function_4cbb0(*(long *)(material + 0x100), (word)shader) != 0;
        if (stage == 5)
            result = (bool)((source->unknown00 >> 13) & 1);
    }
    return result;
}

#if 0 // Full camera and projection storage must be reconciled before enabling.
struct s_camera_copy_3bd00
{
    byte data[0x74];
};
struct s_projection_copy_3bd00
{
    transform4x3f transform;
    transform4x3f inverse;
    byte unknown68[0xc0 - 0x68];
};
extern s_camera_copy_3bd00 g_485618;
extern s_projection_copy_3bd00 g_48568c;
extern byte g_485607;
void function_14ac0(void);
void function_12fa0(real const *projection, byte const *camera, bool scaled, long mode);
void function_3ede0(void);

// Disabled retail 0x3bd00; retail copies 0x74 camera bytes and 0xc0 projection bytes.
bool __stdcall function_3bd00(real height, dword color)
{
    long target = g_4858b8;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    s_projection_copy_3bd00 saved_projection = g_48568c;
    s_projection_copy_3bd00 projection = saved_projection;
    s_camera_copy_3bd00 saved_camera = g_485618;
    s_camera_copy_3bd00 camera = saved_camera;
    g_4858b8 = 21;
    function_14bc0(21, 0, false);
    if (g_485607 && !g_4858b8) function_14ac0();
    D3DDevice_Clear(0, 0, 0xf0, color, 1.0f, 0);
    real *matrix = (real *)&projection.transform;
    matrix[7] = 0.0f - matrix[7];
    matrix[8] = 0.0f - matrix[8];
    matrix[9] = 0.0f - matrix[9];
    matrix[12] = height * 2.0f - matrix[12];
    *(short *)(camera.data + 0x30) = 0;
    *(short *)(camera.data + 0x32) = 0;
    *(short *)(camera.data + 0x34) = 256;
    *(short *)(camera.data + 0x36) = 512;
    function_141590(&projection.transform, &projection.inverse);
    function_12fa0((real *)&projection, camera.data, false, 21);
    function_3ede0();
    function_14bc0((short)target, 0, true);
    g_485618 = saved_camera;
    g_48568c = saved_projection;
    g_4858b8 = target;
    function_12fa0((real *)&saved_projection, saved_camera.data, false, target);
    return true;
}
#endif


// Disabled: passing the shared source-list address violates the shared-global rule.
#if 0
// Retail 0x40e30
void function_40e30(short group, long tag, real distance, long level, word kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context)
{
    if (group != NONE && function_1bf50(tag, level, distance, &level))
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
#if 0
// Retail 0x3c3c0
bool __stdcall function_3c3c0(bool render, bool clear)
{
    byte *structure = (byte *)g_4e0348;
    if (*(long *)(structure + 0x214) <= 0) return true;
    byte *state = *(byte **)(structure + 0x218);
    if (*(real *)(state + 0x7c) == 0.0f || !render) return true;
    function_3ca90(state);
    long tag = *(long *)(state + 4);
    if (function_3c270(state) && function_12de70(3, state + 0x10) && tag != NONE)
    {
        s_tag_instance *instance = g_4e3b44 + (short)tag;
        byte *material_reference;
        if (instance->group_tag == 0x5052544d || instance->group_tag == 0x70727433)
            material_reference = (byte *)function_137bd0(tag)->v10();
        else material_reference = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
        byte *material = g_4e3b44[*(long *)material_reference & 0xffff].bytes;
        byte *table = *(byte **)(material + 0x5c);
        long first = **(word **)(table + 4) & 0x1ff;
        long second = ((word *)(*(byte **)(table + 0xc) + 6))[first] & 0x1ff;
        long shader_tag = *(long *)(*(byte **)(table + 0x14) + second * 10 + 4);
        byte *shader = *(byte **)(*(byte **)(g_4e3b44[shader_tag & 0xffff].bytes + 0x20) + 4);
        long shader_index = *(long *)(shader + 0x100);
        dword flags = *(dword *)(shader + 0x118);
        if (flags & 0x100000) function_3bea0();
        if (flags & 0x80000) function_3bd00(*(real *)(state + 0x84), pack_color3f((color3f const *)(state + 0x58)));
        if (clear) function_14b60(0x10, 0xffffff, 0.0f, 0);
        byte *geometry = *(byte **)(state + 0xc);
        function_1cbb0(1, 1, shader_index);
        function_151c0((s_render_data *)&g_4c1b38);
        function_1cda0(*(void **)(geometry + 0x3c));
        function_1c710(g_51f0f0);
        memset(g_4c1a90, 0, 0x78);
        function_1bd50(g_4c1a90);
        function_1cdd0(0, 0);
        function_1bbf0(tag, 0, 3, 0, 0, 100000.0f);
        function_12d50(0, false, 0);
        function_3bf00(state);
        function_1c8a0(8, *(long *)(geometry + 0x24), *(long *)(geometry + 0x20));
    }
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
                if (bitmap) function_3bcb0(bitmap);
            }
        }
    }
    return true;
}
#endif

