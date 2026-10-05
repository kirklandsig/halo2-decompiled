// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>
#include <string.h>
#include <math.h>
#include <xtl.h>

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

struct s_table_entry g_4b89b0[1];
long g_5093e8;
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
void function_023540(
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
