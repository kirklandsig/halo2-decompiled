// @flags /O2 /arch:SSE /Gr
#include "cluster_mask_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_173B90.CPP: the particle systems effects start (g_510c74) and
   their particles, emitters and locations */

#include "unknown_11c920.h"
#include "bit_vector.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "matrix_transform.h"
#include <xtl.h>
#include "unknown_246cd0.h"

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_structure_leaf_173b90
{
	short cluster_index;
	byte unknown02[6];
};

struct s_structure_bsp_173b90
{
	byte unknown00[0x30];
	s_structure_leaf_173b90 *leaves;
};

void __stdcall function_2486e0(long particle_location_index);
long function_248620(void); /* unknown_2483f0.cpp */
void function_248d90(s_particle_location_datum *particle_location, long *first_index, long *last_index);
long function_248d50(s_particle_location_datum *particle_location);
void function_248970(s_particle_location_datum *particle_location, bool field_b4, real unknown, s_particle_system_datum *particle_system, real *values, transform4x3f const *matrix);
real function_248df0(long index, void *a, void *b, void const *c);
bool function_178af0(long effect_index);
void function_178b30(long effect_index, long unknown0, long unknown4);
color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color3f(const color3f *color);

/* the last time particle systems ran out */
long g_47ff88;
void effect_remove_event_slot(long effect_index, long value);
void function_1753f0(s_particle_system_datum *particle_system);
bool function_174a30(s_particle_system_datum *particle_system, real dt);
void function_17af80(long effect_index, long particle_system_index);

struct s_particle_callback_output_173cb0
{
	long field_0;
	byte field_4;
	byte field_5;
	short field_6;
	short field_8;
	short field_a;
	long field_c;
	long field_10;
	long field_14;
};

// @retail 0x173cb0
bool __stdcall function_173cb0(long tag_index, long unused, long first, long second, long third, short value, s_particle_callback_output_173cb0 *output)
{
	/* This callback is passed at 0x1748f2 with all seven arguments on the stack. */
	long const *tag_argument = &tag_index;
	long const *unused_argument = &unused;
	long const *first_argument = &first;
	long const *second_argument = &second;
	long const *third_argument = &third;
	short const *value_argument = &value;
	s_particle_callback_output_173cb0 *const *output_argument = &output;
	tag_index = *tag_argument;
	unused = *unused_argument;
	first = *first_argument;
	second = *second_argument;
	third = *third_argument;
	value = *value_argument;
	output = *output_argument;
	long const *reference;
	dword group = TAG_GROUP(tag_index);
	if (group == 0x5052544d || group == 'prt3')
		reference = function_137bd0(tag_index)->function_x947334();
	else
		reference = *(long const **)(g_4e3b44[tag_index & 0xffff].bytes + 0x24);
	byte *definition = g_4e3b44[*reference & 0xffff].bytes;
	byte *table = *(byte **)(definition + 0x5c);
	word *entries = *(word **)(table + 4);
	long index = (entries[first * 5] & 0x1ff) + second;
	word *indices = *(word **)(table + 0xc);
	index = (indices[index] & 0x1ff) + third;
	byte *items = *(byte **)(table + 0x14);
	long item_tag = *(long *)(items + index * 10 + 4);
	byte *item = g_4e3b44[item_tag & 0xffff].bytes;
	s_particle_callback_output_173cb0 *out = output;
	out->field_10 = (*(long **)(item + 0x20))[1];
	out->field_5 = 3;
	out->field_4 = 0;
	out->field_8 = 7;
	out->field_14 = 0;
	out->field_6 = value;
	out->field_c = NONE;
	return true;
}

bool function_16e210(long cluster_index, long value);
void function_25ca0(bool enabled);

#pragma optimize("p", on)
// @retail 0x174f80
void function_174f80(s_particle_system_datum *system, void *a, void *b, void *c)
{
	c_type_4e7709 *definition = function_137bd0(system->function_1751d0()->tag_index);
	bool enabled = function_16e210(system->location.cluster_index, g_4b9f8c);
	if (!enabled)
		function_25ca0(false);
	definition->render(system, a, b, c);
	if (!enabled)
	{
		real divisor = 0.0001f > g_4b9fa4 ? 0.0001f : g_4b9fa4;
		real reciprocal = 1.0f / divisor;
		real value = 0.0f - g_4b9ff8 * reciprocal;
		real constants[4];
		constants[0] = value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
		constants[1] = 0.0f;
		constants[2] = g_4b9f18;
		constants[3] = g_4b9f9c;
		D3DDevice_SetVertexShaderConstant(-81, constants, 1);
	}
}
#pragma optimize("", on)

struct s_particle_render_request_173c90
{
	s_particle_system_datum *system;
	void *location;
};

// @retail 0x173c90
void __stdcall function_173c90(s_particle_render_request_173c90 const *request)
{
	s_particle_render_request_173c90 const *const *argument = &request;
	request = *argument;
	function_174f80(request->system, request->location, (void *)15, 0);
}

// @retail 0x173d80
void __stdcall function_173d80(long a, long b, long c, void *first, void *second, long system_index, void *unused)
{
	long const *a_argument = &a;
	long const *b_argument = &b;
	long const *c_argument = &c;
	void *const *unused_argument = &unused;
	a = *a_argument;
	b = *b_argument;
	c = *c_argument;
	unused = *unused_argument;
	void *const *first_argument = &first;
	void *const *second_argument = &second;
	long const *index_argument = &system_index;
	first = *first_argument;
	second = *second_argument;
	system_index = *index_argument;
	s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, system_index);
	long index = system->location_index;
	while (index != NONE)
	{
		s_particle_location_datum *location = DATUM(g_51ec8c, s_particle_location_datum, index);
		function_174f80(system, location, first, second);
		index = location->next_index;
	}
}

// @retail 0x173b90
long function_173b90(s_particle_system_datum *particle_system)
{
	return particle_system->flag10;
}

// @retail 0x173de0
void function_173de0(void)
{
	g_510c74 = data_new_inlined("particle_system", 0x80, sizeof(s_particle_system_datum), 0, g_510c2c);
	g_51ec84 = data_new_inlined("particles", 0x400, 0x40, 0, g_510c2c);
	g_51ec88 = data_new_inlined("particle_emitter", 0x100, 0x4c, 0, g_510c2c);
	g_51ec8c = data_new_inlined("particle_location", 0x100, sizeof(s_particle_location_datum), 0, g_510c2c);
}

// @retail 0x173ee0
void particle_systems_update_locations(void)
{
	s_record_pool_iterator iterator;
	union { short index; word bits; } bsp_snapshot;
	bsp_snapshot.index = g_4686c4;
	s_particle_system_datum *particle_system;

	iterator.data = g_510c74;
	iterator.index = NONE;
	while ((particle_system = (s_particle_system_datum *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (particle_system->location.bsp_index != bsp_snapshot.index)
		{
			s_location location;

			if (particle_system->location_index != NONE)
			{
				s_particle_location_datum *particle_location = DATUM(g_51ec8c, s_particle_location_datum, particle_system->location_index);

				if (!(bsp_snapshot.index != NONE))
				{
					location.leaf_index = NONE;
					location.cluster_index = NONE;
				}
				else
				{
					location.leaf_index = function_14a280(g_4e033c, 0, &particle_location->position);
					if (location.leaf_index != NONE)
						location.cluster_index = ((s_structure_bsp_173b90 *)g_4e0348)->leaves[location.leaf_index].cluster_index;
					else
						location.cluster_index = NONE;
				}
			}
			else
			{
				location.leaf_index = NONE;
				location.cluster_index = NONE;
			}
			location.bsp_index = bsp_snapshot.index;
			particle_system->set_location(&location);
		}
	}
}

// @retail 0x174180
void __stdcall function_174180(long particle_system_index)
{
	s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);
	long child_index;

	function_1753f0(particle_system);
	child_index = particle_system->first_child_index;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
		long next_index = child->next_index;

		particle_system_unlink(child, &particle_system->first_child_index, &particle_system->last_child_index);
		function_174180(child_index);
		child_index = next_index;
	}
	if (particle_system->effect_index != NONE && TAG_GROUP(particle_system->tag_index) == 'effe')
		effect_remove_event_slot(particle_system->effect_index, particle_system_index);
	record_pool_release(g_510c74, particle_system_index);
}

PRIVATE __forceinline long data_scan_174990(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		long count = data->high_water_index;
		if (index < count)
		{
			dword const *bits = data->bitmap;
			do
			{
				if (bits[index >> 5] & (1 << (index & 31)))
					return index;
				++index;
			} while (index < count);
		}
	}
	return NONE;
}

// @retail 0x174990
void __stdcall function_174990(real dt)
{
	struct
	{
		s_particle_system_datum *current;
		s_record_pool_iterator cursor;
	} iterator;
	iterator.cursor.data = g_510c74;
	iterator.cursor.index = NONE;

	for (;;)
	{
		long index = data_scan_174990(iterator.cursor.data, iterator.cursor.index + 1);
		if (index == NONE)
			break;

		iterator.current = (s_particle_system_datum *)(iterator.cursor.data->data + iterator.cursor.data->size * index);
		s_particle_system_datum *particle_system = iterator.current;
		iterator.cursor.datum_index = (particle_system->salt << 16) | index;
		iterator.cursor.index = index;

		if (particle_system->unknown4c == NONE && !function_174a30(particle_system, dt))
		{
			if (particle_system->effect_index != NONE)
			{
				function_17af80(particle_system->effect_index, iterator.cursor.datum_index);
				particle_system->effect_index = NONE;
			}
			function_174180(iterator.cursor.datum_index);
		}
	}
}

// @retail 0x175070
void particle_system_link(s_particle_system_datum *particle_system, long *first_index, long *last_index)
{
	s_record_pool *data = g_510c74;
	long particle_system_index = ((dword)particle_system->salt << 16) | (particle_system - (s_particle_system_datum *)data->data);

	particle_system->next_index = NONE;
	if (*first_index == NONE)
		*first_index = particle_system_index;
	if (*last_index != NONE)
	{
		s_particle_system_datum *last = DATUM(data, s_particle_system_datum, *last_index);

		last->next_index = particle_system_index;
		particle_system->previous_index = ((dword)last->salt << 16) | (last - (s_particle_system_datum *)data->data);
		*last_index = particle_system_index;
	}
	else
	{
		*last_index = particle_system_index;
		particle_system->previous_index = NONE;
	}
}

// @retail 0x175100
void particle_system_unlink(s_particle_system_datum *particle_system, long *first_index, long *last_index)
{
	s_record_pool *data = g_510c74;
	s_particle_system_datum *elements = (s_particle_system_datum *)data->data;
	long particle_system_index = (particle_system - elements) | (particle_system->salt << 16);

	if (particle_system->next_index != NONE)
		DATUM(data, s_particle_system_datum, particle_system->next_index)->previous_index = particle_system->previous_index;
	if (particle_system->previous_index != NONE)
		DATUM(data, s_particle_system_datum, particle_system->previous_index)->next_index = particle_system->next_index;
	if (*first_index == particle_system_index)
		*first_index = particle_system->next_index;
	if (*last_index == particle_system_index)
		*last_index = particle_system->previous_index;
	particle_system->next_index = NONE;
	particle_system->previous_index = NONE;
}

// @retail 0x175180
void s_particle_system_datum::set_location(s_location const *location)
{
	long child_index = first_child_index;

	this->location = *location;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);

		child->set_location(location);
		child_index = child->next_index;
	}
}

// @retail 0x1753a0
long function_1753a0(s_particle_system_datum *particle_system)
{
	long location_index = particle_system->location_index;
	long count = 0;

	if (location_index != NONE)
	{
		s_particle_location_datum *locations = (s_particle_location_datum *)g_51ec8c->data;

		do
		{
			s_particle_location_datum *location = &locations[location_index & 0xffff];

			count += function_248d50(location);
			location_index = location->next_index;
		} while (location_index != NONE);
	}
	return count;
}

// @retail 0x1753f0
void function_1753f0(s_particle_system_datum *particle_system)
{
	long index = particle_system->location_index;

	while (index != NONE)
	{
		long next_index = DATUM(g_51ec8c, s_particle_location_datum, index)->next_index;

		function_2486e0(index);
		index = next_index;
	}
	particle_system->location_index = NONE;
	particle_system->unknown34 = NONE;
}

// @retail 0x173ba0
void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values)
{
	long index = 0;
	while (mask)
	{
		__asm
		{
			bsf ecx, mask
			mov index, ecx
		}
		mask &= ~(1 << index);
		values[index] = function_248df0(index, a, b, c);
	}
}

struct s_particle_value_cache
{
	real values[17];
	dword mask;
	void *field_48;
	void *field_4c;
	void const *field_50;

	void function_173c10(dword mask);
};

struct s_particle_emitter_175610
{
	short salt;
	word count;
	long first_particle_index;
	long next_index;
	byte unknown0c[0x40];
};

struct s_particle_175610
{
	short salt;
	word flags;
	long next_index;
	byte unknown08[0x14];
	point3f position;
	vector3f velocity;
	byte unknown34[0xc];
};

struct s_emitter_definition_175610
{
	byte unknown00[0x48];
	s_particle_property scale;
	s_particle_property color;
	s_particle_property alpha;
	byte unknown78[0x38];
	dword flags;
	dword mask;
};

struct s_particle_frame;
void function_248450(matrix3x3 *rotation, s_particle_frame const *frame, point3f *position, matrix3x3 const **rotation_result, point3f const **position_result, bool field_b4);
void function_246d80(s_particle_property const *property, real const *values, color3f *color);
real function_3eb70(void);
typedef void (__stdcall *particle_callback_175610)(s_particle_175610 *, s_particle_value_cache *, point3f const *, vector3f const *, real, color4f const *, void *);


// score 42 (was 42): both in-place rotations go through the shared rotate helper (it copies the
// input when the output is the input), which gives retail's j and k rows. Left: row i sums the up
// product before the forward one (retail forward first, loading forward.i into a register); the
// offset's zero store comes before the list test (retail after); the two 1.0 stores are swapped; and
// after each property call ours reloads the particle before the emitter definition (retail the
// other way round).
// Tried: combined entry decisions 53; entry decisions with shared cache 53; scale/alpha/both result
// locals 119/56/119; saved successor 217; particle cache with scale result 119; combined
// entry/property/cache 116; forward/left/up/all axis receivers 118/55/55/118; forward/left/up/first-pair
// values 182/167/181/289; left/up receivers with empty-list return and scale-first initialization
// 52/52 (the return only repeats the loop test); captured position/late output 353; captured
// velocity/late output 357; both 437; translation component loop 56; inner cache member 56; derived
// mask at use 198; scale initialized at property selection 273; translation value 160; the
// definitions indexed (index++), which put the offset's zero store after the list test and fix the
// three reload pairs but turn the rotations' products around and add 10 bytes: indexed 119, with
// scale first 117, plus split += rows 116, plus a matrix pointer local 123; scale declared before
// multiplier alone 54 (fixes the 1.0 store order only); existing 0x173c10 for both hand-written mask
// refreshes 56 (same code, kept); on the indexed form input-first products 117 and a greedy screen of
// every rotation row's term order 116 at best; the offset as a for-loop init 56; complete
// position/velocity/both row products 127/56/127; all-products before stores 164/121/171; both input
// copies on complete-product graph 170; shared translation add with row products 127; indexed
// definitions plus scale-first defaults with those six product graphs 103/117/103/141/153/116. With
// the helper: row i's terms in every order 42; row i right-grouped (four groupings) 45-49; indexed
// definitions 115 (1136 bytes), with scale first 113; scale first alone 40; locals renamed to sort
// the other way 42; helper with an unconditional copy, a result local, or the copy inside its own
// block 50/287/50 (0x2477b0 then differs).
// Round 19: in the first rotation only row x differs (ours evaluates forward.i*x first, so x and z
// take each other's XMM registers; rows y and z agree apart from them). The row x pair swapped in both
// rotations 56 (identical: VC orders the products itself, by the function's tuple count); both
// rotations through the existing in-place matrix_transform_174420 119, on the indexed form 121; on
// this body the translation as `+=` 56, a named scale value at both scale sites 119 (1136 bytes), the
// 0x107f0 mask named 55 (second rotation's register numbering only); on the indexed form with scale
// first (117, every non-rotation difference fixed): `+=` 117, named scale value 121, named mask 117.
// Round 307: Complete per-emitter alpha/color/scale policy subsets 248/267/261/255/246/241/282;
// one raw flag observation 260. Policy homes enlarge the frame; original property observations
// and shared in-place rotations restored.
// Round 43: indexed definitions 115 and with scale first 113 again (the rotation sums reorder in every row;
// the compile-only screen reproduces it, and 50 placements of the index, copies, result pointers and the
// next-index read keep it); the next index read after the callback 231, on the indexed form 218;
// rotation result pointers declared first 42 (identical).
// Round 27: the emitter definition computed first in the loop body 47 (fixes all three reload pairs:
// they follow where the definition's value is computed, but the emitter lookup then comes after it);
// declared first and assigned in place 42 (same code); the offset declared at the function top 65 (the
// zero store follows the declaration). On the indexed form with scale first (113, 1136 bytes; only the
// two rotations differ: rows i/j/k sum k,i,j / j,i,k / i,k,j against retail j,i,k / i,k,j / i,k,j):
// the 0xf80f mask passed inline 113 and the index declared at the function top 113 (rotations unchanged).
// Round 13 (fast): the existing vector3f_add for the translation 122 (1136 bytes); both rotations through the
// existing in-place vector3f_rotate_into_axes 84, with vector3f_add 111; emitter_definition computed before
// particle_index 42 (same code), with scale first 40; on the indexed form with scale first:
// vector3f_rotate_into_axes 102, with vector3f_add 120, vector3f_add alone 115. Diagnostics (not kept): no-op
// `(real)(x * 1.f)` chains in the callback's scale argument cycle row i through y,z,x (base) / z,x,y / x,y,z
// with rows j and k fixed, never retail's y,x,z; on the indexed form with scale first a four-deep chain gives
// 33 at 1130 bytes (every difference but row i of both rotations, one load pair and the branch offsets fixed).
// Round 93 (lane 8): the helper's copy loads the vector j, k, i where retail loads j, i, k, and row i's
// sum follows that order. Scale initialised before multiplier 40 (the 1.0 stores only); with the two
// particle copies swapped 48; with them declared at the inner loop's scope and assigned after the
// particle lookup 40; with them made just before the rotation test 352 (1187 bytes); indexed
// definitions with scale first 113 (1136 bytes) and alone 115.
// Lane 16 r48 (fast): long-indexed definitions 115 (1136 B); colour declared first 42; offset step before
// the emitter step 42; offset declared before the emitter index 42; dword offset 42. A compile-only build
// of this file reproduces both rotations op for op (9 ops from retail's 33): row i's terms in all six
// orders and both groupings, member-wise copies in all six orders (these become float copies), `+=` or
// swapped translation adds, the motion rotation first, swapped particle copies, scale first, colour or
// result pointers first, the masks inline, the next index read after the callback and an emitter-first
// body all leave row i at j,k,i; indexed definitions (long/short) 33/46.
// Round 95 (lane 8): the zero store after the list test is the mark of an index that VC
// strength-reduces into the offset (retail keeps only the offset IV), so retail likely indexed the
// definitions; the indexed form `&(*(T **)(definition + 0x34))[index]` 115 (1136 bytes, aligned 38 ->
// 91: every rotation row's sum order changes); with scale first 113; with the definition computed
// before the emitter lookup 126, both 124; the index as a for-loop counter 115 (same code).
// Round 114 (lane 6, fast): scale initialised before multiplier 40 again (the 1.0 stores only; below
// the margin); the particle position copied as a vector3f (no casts) 42 (identical); the next index
// read right after the particle lookup 272 (1113 bytes); color declared first 42; the 0xf80f mask
// passed at the call 42; on scale-first, the translation after the motion rotation 84.
// Round 27 (lane 13): both rotations through the existing vector3f_rotate_into_axes 84; the multiplier as a
// ternary 186; indexed definitions with a 16-bit index 271; indexed + scale first + the definition before the
// emitter lookup 124; the colour declared at the function top 42 (identical). A compile-only screen of the whole
// file (it reproduces our rotations exactly; ~110 forms) shows flat row sums canonicalised (every term and
// operand order identical) and four row-order states that cycle as whole-function tuples are added (indexed
// form +1, each no-op product past the first +1, a 16-bit index +2); none is retail's for either rotation.
// Helper shapes (row statements jki/kij, unconditional copy, copied parameter, component locals, row locals,
// `+=` rows), inline datum getters and index types (long/dword/int/short/word, pointer arithmetic), each in all
// four states: no form gives retail's left/forward/up row i with forward/up/left rows j and k.
// Round 137 (lane 4): scale first 40; indexed definitions 115, with scale first 113 (1136 bytes: every difference
// before the first rotation is retail's, the rotations differ from the helper copy's store order on, ours i first,
// retail j first); on that, the particle copies swapped 121, declared before the particle lookup and assigned after
// it 113 (same code).
// Round 59 (lane 12, fast): the scale product as `scale = ...; scale *= multiplier;` at both scale
// sites 124 (1138 bytes); that on the indexed form with scale first 119 (1140 bytes, every rotation
// row reorders); const emitter definition and particle pointers 42; the particle loop as a do/while
// under its existing `if` 42; the emitter definition computed before the first-particle read 42.
// Round 140 (lane 4): the reload pairs follow where the emitter definition is formed relative to the
// emitter lookup: formed first, with scale first, 45 (reloads and 1.0 stores as retail, the outer
// address computation then leads); the offset advanced before the next index 42 (same code); the
// offset declared first 42 (same code); the offset as `dword` 42; the transform test read once into a
// bool for both tests 326 (1156 bytes).
// Round 46 (m3 lane 10, fast): the emitter's next index read right after the emitter lookup 318 (1150
// bytes), with scale first 318; the 0xf80f particle mask computed right after the emitter mask read 263
// (1117 bytes), with scale first 261; the emitter mask read from the definition at both uses 239 (1136 bytes).
// Round 59 (lane 16, fast with callees and 0x2243c0): one flags local for the six flag tests 292 (1157 bytes);
// the emitter mask declared just before its first use 236, at the emitter loop's scope 220; indexed
// definitions with the flags local 297 (1159 bytes); the particle mask inline at its call 42 (identical). A
// compile-only screen of 190 combinations (indexed, flags local, mask at each use, scale/colour/result pointers
// first, mask scope, `+=` translation) shows five whole-function rotation states; the mask read at each use is
// the only one with retail's rows i and j in both rotations (row k then left/forward/up), none is retail's.
// Round 39 (m3 lane 14, fast): scale first with `+=` translation 40; that with the emitter definition formed before
// the particle index 40; the motion copy declared before the position copy 50, with scale first 48; the next index
// read before the particle-mask refresh 58. Diagnostic one-use locals (removed): at the top, after the emitter
// lookup or before the multiplier 109 (the rotation rows move to another of their cycle states, nothing else
// fixed); after the particle mask or the particle lookup 42 (no change).
// Round 61 (lane 16, fast): `real scale = 1.0f, multiplier = 1.0f;` 40 (the 1.0 stores only, as scale
// first); indexed definitions with scale first and the emitter mask read at each use 232 (1130 bytes; the
// register allocation turns over: the definition and particle homes swap), with `+=` translation 232;
// indexed with scale first and `+=` translation 113 (1136 bytes); indexed with the one declaration 113. A
// compile-only screen of the helper's copy as memcpy gives the same code as the struct copy; copying from
// `*out` (the same vector) loses the copy's alias and turns every row.
// Round 20 (lane 17, fast): permuter 600 s, 58 tries, none below 42. Row i's input copy is stored in the
// order the row consumes it (ours y, z, x; retail y, x, z), so the copy follows the sum order. Both
// rotations fed from a separate input copy (`position = translated_position`, rotated into it; the same
// for the velocity) 123 (1136 bytes, row i unchanged); indexed definitions with scale first 113
// (re-measured), plus the separate input copies 115 (1138 bytes), with the emitter mask read from the
// definition at both uses 232 (1130 bytes); the separate input copy for the position rotation only 42
// (same code: VC folds it into the helper's own copy).
// @retail 0x175610
void function_175610(s_particle_value_cache *cache, s_particle_system_datum *system, s_particle_location_datum *location, particle_callback_175610 callback, void *context)
{
	s_effect_particle_system_definition *definition = system->function_1751d0();
	if (system != cache->field_48)
	{
		cache->field_48 = system;
		cache->mask &= 0xfffff98f;
	}
	if (location != cache->field_4c)
	{
		cache->field_4c = location;
		cache->mask &= 0xfffecf7f;
	}
	long emitter_index = *(long *)((byte *)location + 4);
	long emitter_definition_offset = 0;
	while (emitter_index != NONE)
	{
		s_particle_emitter_175610 *emitter = DATUM(g_51ec88, s_particle_emitter_175610, emitter_index);
		long particle_index = emitter->first_particle_index;
		s_emitter_definition_175610 *emitter_definition =
			(s_emitter_definition_175610 *)(*(byte **)((byte *)definition + 0x34) + emitter_definition_offset);
		if (particle_index != NONE)
		{
			real multiplier = 1.0f;
			real scale = 1.0f;
			if (TEST_FIELD_BIT(system->flag10))
				multiplier = function_3eb70();
			matrix3x3 rotation;
			point3f origin;
			matrix3x3 const *rotation_result;
			point3f const *origin_result;
			color4f color;
			dword emitter_mask = emitter_definition->mask;
			if (definition->unknown0c)
				function_248450(&rotation, (s_particle_frame const *)emitter, &origin, &rotation_result, &origin_result, *((byte *)location + 2) != 0);
			cache->function_173c10(emitter_mask & 0x107f0);
			if (emitter_definition->flags & 1)
				color.alpha = function_246cd0(&emitter_definition->alpha, cache->values);
			if (emitter_definition->flags & 2)
				function_246d80(&emitter_definition->color, cache->values, (color3f *)&color.red);
			if ((bool)((emitter_definition->flags >> 2) & 1))
				scale = function_246cd0(&emitter_definition->scale, cache->values) * multiplier;
			dword particle_mask = emitter_mask & 0xf80f;
			while (particle_index != NONE)
			{
				s_particle_175610 *particle = DATUM(g_51ec84, s_particle_175610, particle_index);
				point3f translated_position = particle->position;
				vector3f motion = particle->velocity;
				if (particle != cache->field_50)
				{
					cache->field_50 = particle;
					cache->mask &= 0xffff07f0;
				}
				cache->function_173c10(particle_mask);
				particle_index = particle->next_index;
				if (!(emitter_definition->flags & 1))
					color.alpha = function_246cd0(&emitter_definition->alpha, cache->values);
				if (!(emitter_definition->flags & 2))
					function_246d80(&emitter_definition->color, cache->values, (color3f *)&color.red);
				if (!(bool)((emitter_definition->flags >> 2) & 1))
					scale = function_246cd0(&emitter_definition->scale, cache->values) * multiplier;
				if (definition->unknown0c)
				{
					function_2477b1(rotation_result, (vector3f *)&translated_position, (vector3f *)&translated_position);
					translated_position.x = origin_result->x + translated_position.x;
					translated_position.y = origin_result->y + translated_position.y;
					translated_position.z = origin_result->z + translated_position.z;
					function_2477b1(rotation_result, &motion, &motion);
				}
				callback(particle, cache, &translated_position, &motion, scale, &color, context);
			}
		}
		emitter_index = emitter->next_index;
		emitter_definition_offset += sizeof(s_emitter_definition_175610);
	}
}

// @retail 0x173c10
void s_particle_value_cache::function_173c10(dword mask)
{
	function_173ba0(~this->mask & mask, field_48, field_4c, field_50, values);
	this->mask |= mask;
}

struct s_248cc0;
struct s_247fd0;
void function_248cc0(s_248cc0 *location, s_247fd0 *values, s_particle_system_datum *system,
	point3f const *position, vector3f const *velocity);

PRIVATE inline real particle_clamp_175430(real const &value, real minimum, real maximum)
{
	return value < minimum ? minimum : value > maximum ? maximum : value;
}

// @retail 0x175430
void __stdcall function_175430(s_particle_system_datum *system, point3f const *position, vector3f const *velocity)
{
	if (particle_clamp_175430(position->x, -100000.0f, 100000.0f) != position->x ||
		particle_clamp_175430(position->y, -100000.0f, 100000.0f) != position->y ||
		particle_clamp_175430(position->z, -100000.0f, 100000.0f) != position->z ||
		particle_clamp_175430(velocity->i, -100.0f, 100.0f) != velocity->i ||
		particle_clamp_175430(velocity->j, -100.0f, 100.0f) != velocity->j ||
		particle_clamp_175430(velocity->k, -100.0f, 100.0f) != velocity->k)
		return;
	s_record_pool *locations = g_51ec8c;
	if (system->location_index == NONE)
	{
		long index = function_248620();
		if (index == NONE)
			return;
		s_particle_location_datum *location = DATUM(locations, s_particle_location_datum, index);
		function_248d90(location, &system->location_index, &system->unknown34);
		location->position = *position;
	}
	if (system->location_index != NONE)
	{
		s_particle_value_cache cache;
		cache.field_4c = 0;
		cache.field_50 = 0;
		s_particle_location_datum *location = DATUM(locations, s_particle_location_datum, system->location_index);
		cache.field_48 = system;
		cache.mask = 0;
		if (location != cache.field_4c)
		{
			cache.field_4c = location;
			cache.mask = 0;
		}
		cache.function_173c10(0x1ffff);
		function_248cc0((s_248cc0 *)location, (s_247fd0 *)&cache, system, position, velocity);
	}
}

// @retail 0x173fd0
long function_173fd0(s_effect_particle_system_definition *definition, long effect_index, long tag_index, short definition_index, long event_index)
{
	long particle_system_index = NONE;

	if ((bool)((~(((byte *)definition)[0x16] >> 3)) & 1) && definition->tag_index != NONE && function_137bd0(definition->tag_index) && definition->unknown30 > 0)
	{
		if (effect_index == NONE || function_178af0(effect_index))
		{
			particle_system_index = record_pool_allocate(g_510c74);
			if (particle_system_index != NONE)
			{
				s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);

				*((word *)particle_system + 6) = 9;
				if (TEST_FIELD_BIT(definition->flag0))
					particle_system->flag4 = true;
				else
					particle_system->flag4 = false;
				if (TEST_FIELD_BIT(definition->flag1))
					particle_system->flag5 = true;
				else
					particle_system->flag5 = false;
				if (TEST_FIELD_BIT(definition->flag2))
					particle_system->flag6 = true;
				else
					particle_system->flag6 = false;
				for (long i = 0; i < 2; i++)
				{
					g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
					particle_system->random[i] = (real)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
				}
				particle_system->tag_index = tag_index;
				particle_system->effect_index = effect_index;
				particle_system->next_index = NONE;
				particle_system->previous_index = NONE;
				particle_system->location_index = NONE;
				particle_system->unknown34 = NONE;
				particle_system->definition_index = definition_index;
				particle_system->event_index = event_index;
				particle_system->first_child_index = NONE;
				particle_system->last_child_index = NONE;
				particle_system->parent = 0;
				particle_system->unknown4c = NONE;
				particle_system->color = 0xff808080;
			}
			else
			{
				long game_time = g_510c54->game_time;

				if (game_time - g_47ff88 > g_510c54->field_2_3 * 60)
					g_47ff88 = game_time;
			}
			if (particle_system_index != NONE && effect_index != NONE && TAG_GROUP(tag_index) == 'effe')
				function_178b30(effect_index, (long)definition, particle_system_index);
		}
	}
	return particle_system_index;
}

// @retail 0x1751d0
s_effect_particle_system_definition *s_particle_system_datum::function_1751d0()
{
	long index = tag_index;
	switch (TAG_GROUP(index))
	{
	case 'effe':
		return &TAG_GET(s_effect_definition, index)->events[event_index].particle_systems[definition_index];
	case 'bsdt':
		return (s_effect_particle_system_definition *)(*(byte **)(g_4e3b44[index & 0xffff].bytes + 0x18) + definition_index * sizeof(s_effect_particle_system_definition));
	case 'PRTM':
	case 'prt3':
		{
		c_type_4e7709 *definition = function_137bd0(parent->function_1751d0()->tag_index);
		long element = definition_index;
		return definition->function_1751d0((word)element);
	}
	}
	return 0;
}

// @retail 0x175270
void function_175270(s_particle_system_datum *particle_system, s_particle_system_spawn *spawn, transform4x3f const *matrix, bool field_b4)
{
	s_particle_value_cache cache;
	s_particle_location_datum *particle_location;

	*((word *)particle_system + 6) |= 0x101;
	cache.mask = 0;
	cache.field_48 = 0;
	cache.field_4c = 0;
	cache.field_50 = 0;
	if (spawn->location_index == NONE)
	{
		spawn->location_index = function_248620();
		if (spawn->location_index == NONE)
			return;
		particle_location = DATUM(g_51ec8c, s_particle_location_datum, spawn->location_index);
		function_248d90(particle_location, &particle_system->location_index, &particle_system->unknown34);
		particle_location->position = matrix->position;
	}
	particle_system->unknown04 = (spawn->unknown + spawn->scale) * particle_system->unknown08;
	particle_location = DATUM(g_51ec8c, s_particle_location_datum, spawn->location_index);
	if (particle_system != cache.field_48)
	{
		cache.field_48 = particle_system;
		cache.mask &= 0xfffff98f;
	}
	if (particle_location != cache.field_4c)
	{
		cache.field_4c = particle_location;
		cache.mask &= 0xfffecf7f;
	}
	cache.function_173c10(0x1ffff);
	function_248970(particle_location, field_b4, spawn->unknown, particle_system, cache.values, matrix);
	spawn->location_index = particle_location->next_index;
}

// score 17 (was 17): Frame and entry agree; half-path buffer/result ownership and the distant untinted exit remain.
// Earlier 17 (was 29): the untinted half-path exit is a long jump to the last
// the untinted half-path exit is a long jump to the last
// epilogue where retail's short jump joins the half path's own; the half path
// reads the unpacked color through the returned pointer where retail reads the
// buffer and pushes the returned pointer. Store per path (retail's entry
// registers) 41; returned pointer packed only 41; one shared buffer 17;
// early untinted return 17; tinted tested first 37.
// Update inline/noinline declarations 17/17; packing noinline 17; paired update-inline/packing-noinline 17.
// Plain product loop with long/short/byte ordinals 21/45/45; shared native product-loop storage 23.
// Shared double scale 58; immutable/ordinary complete products 23/23;
// product value copy 74; decode value copy 63; exchanged decode workspaces 17;
// two decode-result lifetimes 45; double scale with in-place result 64.
// Standalone unpack before scaling 41; unpacked pointer for scale and pack 42 (204 bytes);
// vector half buffer 17; half names buffer/scaled 17/17; product declarations swapped 17;
// second input alternate with declarations swapped/original 17/17.
// Scale from the buffer through the unpacked result 42 (204 bytes); a 2400 s permuter run, no result.
// Round 27 (lane 13): parameters reordered to (color_a, color_b, system, multiplied, tinted): all three retail
// callers query the tint before the multiply flag, which a call written with the tint last evaluates first
// (0x174a30 292 -> 248 with it); this body's code is unchanged (17), and so are 0x177040 and 0x178c80.
// @retail 0x175a80
void function_175a80(dword color_a, dword color_b, s_particle_system_datum *particle_system, bool multiplied, bool tinted)
{
	if (!multiplied)
	{
		if (tinted)
		{
			color3f color;

			vector3f_scale((vector3f *)unpack_color3f(color_b, &color), 0.5f, (vector3f *)&color);
			particle_system->color = pack_color3f(&color);
		}
	}
	else if (tinted)
	{
		color3f color;
		color3f other;

		unpack_color3f(color_a, &color);
		unpack_color3f(color_b, &other);
		color.red *= other.red;
		color.green *= other.green;
		color.blue *= other.blue;
		particle_system->color = pack_color3f(&color);
	}
	else
	{
		particle_system->color = color_a;
	}
}

extern vector3f *g_4687bc;
extern vector3f *g_4687b4;
real function_30bf0(vector3f *vector);
vector3f *function_11d000(vector3f const *vector, vector3f *out);
long function_1753a0(s_particle_system_datum *system);

class c_248767 : public s_effect_particle_system_definition
{
public:
	void function_248750(s_particle_system_datum *system, real dt, byte *location);
};

class c_particle_tag_view_174a30
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual void v5() = 0;
	virtual void v6() = 0;
	virtual void v7() = 0;
	virtual long count() = 0;
	virtual long const *names() = 0;
};
// score 248 (was 248): 1355 bytes and frame 0x60 as retail (round 27 form, below). Before it (292,
// 1372 bytes against 1355): our entry stored a `result`
// slot (retail has none and returns `xor eax, eax` / `mov eax, 1` from the bool expression);
// ours merges the flag-9 word update with the flag-8 clear (retail stores the word, clears flag 8
// with `and byte ptr [ebp+0xd], 0xfe` and reloads); retail keeps the child index in EDI from the
// loop head into the recursion block (so the particle and the tag object go to EBX), spills the
// tinted byte to the bottom slot and the parent color to a stack local read before the child
// definition call; ours keeps the index on the stack, the particle and tag in EDI and the tint in
// BL. 0x248750 and (since round 67) 0x175270 now take retail's registers.
// Kept: child-loop tag lookup through the inline particle_object_for_tag 294 (direct parent
// color 308 earlier); the cluster test through the existing bit_vector_test 292 (identical).
// Round 67 (with 0x248970's location now stacked and its flag in AL, so 0x175270 matches and
// takes the child in EBX as retail): bool-expression return 356 (1355 bytes, frame 0x5c); that
// with the parent color read into a local before the child definition call 355 (color spilled as
// retail, tint still in BL); that without the volatile parent 345 (frame 0x60, 1362 bytes; VC then
// hoists and shares `&particle_system->first_child_index` with the first-child reads, which
// retail computes at the unlink call).
// Round 139: the flag word through a `word *` local (read, set/clear 0x200, flag8 bitfield clear)
// gives retail's word store + byte clear 292, and testing 0x20 through the flag5 bitfield then
// reproduces retail's `mov ax, [ebp+0xc]; test al, 0x20` exactly, but our entry `result = false`
// store still sits there and every later instruction shifts (352); with the pointer and a value
// local 292/292; the bitfield test on the value form 294; result declared at the end or direct
// returns on top of the exact flag form 345/351 (frame 0x5c). Round 137: an inline flag-word
// setter on the word's address per bit gives retail's store/byte clear/reload (306 alone, 292 with
// the lookup; a new one-caller helper); with the lookup, boolean return / plain parent / both
// 350/344/345 (frame 0x60, 1362 bytes), with the setter too 353/345/344; 0x248970 flag-first
// parameter order 294 (its convention unchanged); 0x248970 flag store after its definition call /
// after the position copy 294/294; child index declared before the matrix 294.
// Round 195: tint byte/word/long/dword carriers 298/301/299/299, the same declared before the
// matrix 298/301/299/299. Round 183: five fixed-basis column-order permutations
// 320/320/326/326/320; three default-position permutations 308/308/308. Round 174: existing cross
// input references 308; forward/up/both values 411/409/507; reference inputs/output 308; native
// result with pointer/reference/value inputs 369/369/624; fallback normalization vector/scalar/
// working-vector/computed-input forms 349/308/349/359, with result after flags 346/305/346/356.
// Earlier forms: reversed-component loops 394; normalization loop 394; perpendicular call 394;
// inline depth one 394, zero 517; spawn initializer 394; early matrix values 393; child tag scalar
// 394; long result 329; dword flags 401; ordinary local parent 366; word result 338; direct final
// result 388; child for-loop 338; native fallback/default/both perpendicular results 354/354/364;
// published before cross 354/354/364; native orientation result 373, with shared cross 373; late
// long result 411; entry/late complete result arms 395/395; late result from tests 395; child/
// location-stage result 315/316; late successful default 411; signed flags with late result 411;
// native flag bytes 382; flag-stage result 378; native flag overlay 382, with flag-stage result
// 378; named byte transfer 411; transfer/result 411; byte flags with ordinal observations 388;
// overlay/ordinal/result 384 (all native byte layouts lose five matched controls); flag-stage long
// result/native bitfield transfer/direct visibility and combinations 305-368; post-visibility
// result 315; native definition, shared velocity address, readonly particle and their
// combinations 308; native receivers/location for 308; native location/child/successor/all
// handle domains 308; byte/signed-byte/word/bool canonical results 374/374/308/375; native memcpy
// forward/up/left/position transfers and combinations 308; cleanup/count/spawn/unlink inline or
// outlined inline 308; full-EAX return with long/bool/complete-arm results 405/371/405; one
// bool-expression return 405 (frame 0x5c); 0x137bd0 moved out of its /Ob1 file 457; the volatile
// parent removed 371 (frame 0x64), with the bool return 392; flag 9/8 update through the
// bitfields 368.
// Round 270: both real color predicates 327; reversed readonly queries 323; both
// declared before pose 327; predicates with parent color 333; direct result with
// predicates 357, reversed 361, with color 374, entry predicates/color 374. All
// eight match-only capture forms restored; early result/flag stores and child/tint homes remain.
// Round 24: scoped flags 292; plus direct Boolean result 356; plus early parent color 355;
// plus function-entry tint 355; word-pointer flags instead 354; ordinary parent instead 344;
// long tint instead 364; without parent color 353. Original restored.
// Round 206: Two-state completion enum, entry default/entry complete arms/tail default/tail complete arms
// [292, 366, 361, 366]; with word-pointer/bit-five flag idiom [359, 369, 359, 369]. Original result and flags restored.
// Round 232: four complete positive/negative factor orders, direct or all six
// products staged before output, 292 each. Original two-use helper restored.
// Round 214: Direct selector/for-owned child handle/both 292 each; tint at the color-call
// expression, alone/with selector/with child-for/with both, 288 each (four-point gains rejected).
// All three with Boolean completion storage 344. Original body and every call restored;
// entry result/flag publication and child/tint homes remain.
// Round 163: recursive inline/cross forceinline/both/recursive noinline
// boundaries 292 each; complete ordered completion on each graph 356 each
// (1355 bytes, frame 0x5c). Original result, declarations and boundaries restored;
// entry completion storage, word/byte publication and child/tint homes remain.
// Round 234: native checked-particle record, mutable/readonly, ordinary/aggregate spawn
// 292 each; those four with direct ordered completion 356 each (1355 bytes, frame 0x5c).
// All record, request and completion forms restored.
// Entry matrix/spawn/both workspaces with ordered completion 356/356/356;
// with word receiver 353/353/353; both workspaces with original completion 292, with word receiver 290; original restored.
// Round 387: Shared checked-datum boundary with native retained salt, tint consumer, word-pointer flags and ordered completion combinations 292/288/359/360/356/359/353/354; all restored.
// Round 219: native negative-forward/perpendicular/both setters 294/292/294; cross setter 292;
// negative/cross 294; perpendicular/cross 292; all 294; with normalization setter 294. Original
// restored.
// Round 221: final completion accumulation, two-part eager/logical and incremental eager/logical,
// long 376/366/374/366; bool 367/359/360/359. Original result and body restored; Boolean
// accumulation reaches 1355 bytes but changes the frame and regresses.
// Round 173: coupled count/visibility/cleanup/unlink inline summaries, all six pairs,
// count/visibility/cleanup and all four 292 each. Original declarations and complete
// caller/provider bodies restored; entry completion, flag publication and child/tint homes remain.
// Round 290: Native long/bool/byte/short emptiness results 295/346/346/292; with existing shared
// word-flag publication 369/366/366/367. Original executable source restored.
// Round 427: uninitialized child/successor/selector declarations at entry, all seven
// subsets, then all with lexical handle names, 292 each. Original declaration scopes,
// value observations and complete body restored; entry result/flags and child/tint homes remain.
// Round 45: shared ordinary/staged cross producers, each with original/word receiver/
// direct completion/both graphs, 292/290/356/353 each. Two-point gains restored;
// original private cross, completion, flags and complete body retained.
// Round 27 (lane 13): child->unknown24 read at each use instead of an index local 316 (aligned 117 -> 91:
// child index in EDI at the loop head, particle in EBX, name index in EDI, as retail); plus the tint and
// multiplied queries written in the 0x175a80 call 292 (tag object in EBX as retail; multiplied queried
// first); plus the flag-9 update as a bitfield if/else `if (flag8) flag9 = true; else flag9 = false;`
// (retail's word store, byte clear and reload) and the boolean-expression return 267 (word-pointer or
// cast-per-use forms of the flag update 267/267; `flag9 = flag8` is branchless 354); plus
// TEST_FIELD_BIT on flag0 252 (retail tests flag0 and flag3 as bytes, flag9 as a plain bit; on both
// 274, on flag9 only 273); a named tint/both named/named multiplied 285/275/269; then 0x175a80's
// parameters as (color_a, color_b, system, multiplied, tinted), so the call evaluates the tint query
// first as all three retail callers do, 263 (frame 0x60, every stack slot as retail); then the
// `volatile` parent dropped 248 (1355 bytes; VC no longer hoists the child-list address); the
// existing normalize3d and vector3f_scale for the hand-written normalisation and negation 248
// (identical, kept). Also tried: member function (this in ECX) 292; datum_get_inlined 292; parent
// colour read into a local before the child definition call 274/262; an inline colour helper taking
// the parent colour 414 (not inlined), forceinline 260 (colour read early as retail). Left (aligned
// 59): the default case loads the velocity after the -1 constant and its cross product takes other
// XMM registers; the fallback's x87 length sums k, j, i (retail j, i, k, a rotation: tuple count) and
// retail keeps the normalised forward in xmm4/xmm2/xmm5 across the join (ours reloads); retail reads
// the parent colour before the child definition call.
// Round 59 (lane 12, fast): the fallback normalisation through the existing normalize_inline 248
// (different code, same count); the existing cross3f for both cross products 248 (identical code,
// kept: it replaces a private copy of cross3f); both 248; a velocity pointer local for both
// negations 248; the matrix declared before the particle lookup 248. Retail reads the child's
// tag index into ECX for the tag-group switch (ours EAX), as 0x174420's note describes.
// Round 477: Perpendicular publication ikj/jik/jki/kij/kji 252/247/268/275/275; double absolute magnitudes with ijk/kji/jik 282/297/282.
// One-point gain rejected; original frame 0x60, helper calls, magnitude types, all axis stores and full body restored. Cross/normalized-component and color/tag carriers remain.
// Round 105 (lane 7, fast): the spawn location assigned before its scale 248 (same count); the parent colour in a
// local before the child definition call 262 (1363 bytes); the name tests as an if/else chain 277; the particle
// typed as the file's particle record (velocity/position by name) 248 (identical); the position copied before
// the 1.0 scale store 250. A compile-only screen of the default case (scale by hand, scale/cross through
// returned pointers, the staged cross, the typed particle) never loads the velocity ahead of the -1.0 constant.
// Round 43 (lane 14, fast; the old perpendicular call 394 retried on the round-27 base): the fallback's
// perpendicular through the existing out-of-line 0x11d000 286 (1327
// bytes), its normalisation through 0x30bf0 357 (1259), both 326 (1253): retail inlines both there; switch on the
// table entry with no `name` local 248 (same code); spawn record declared before the matrix 248 (same code);
// scale and position stores ahead of the orientation if/else 280; the child's next index read after the
// particle block, before the recursive update 303.
// @retail 0x174a30
bool function_174a30(s_particle_system_datum *particle_system, real dt)
{
	if (particle_system->flag8)
		particle_system->flag9 = true;
	else
		particle_system->flag9 = false;
	particle_system->flag8 = false;
	if (!particle_system->flag5 && !bit_vector_test(g_4c56c0, particle_system->location.cluster_index))
		function_1753f0(particle_system);
	s_effect_particle_system_definition *definition = particle_system->function_1751d0();
	long location_index = particle_system->location_index;
	while (location_index != NONE)
	{
		s_particle_location_datum *location = DATUM(g_51ec8c, s_particle_location_datum, location_index);
		((c_248767 *)definition)->function_248750(particle_system, dt, (byte *)location);
		location_index = location->next_index;
	}
	c_particle_tag_view_174a30 *tag = (c_particle_tag_view_174a30 *)function_137bd0(definition->tag_index);
	long const *names = tag->names();
	long child_index = particle_system->first_child_index;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
		long next_index = child->next_index;
		byte *particle = record_pool_lookup_checked(g_51ec84, child->unknown4c);
		if (particle)
		{
			transform4x3f matrix;
			if (child->unknown24 != NONE && child->unknown24 < tag->count())
			{
				long name = names[child->unknown24];
				switch (name)
				{
				case 0x20000ca:
					matrix.forward = *g_4687b0;
					matrix.up = *g_4687a8;
					matrix.left = *g_4687ac;
					break;
				case 0x70000c0:
					matrix.forward = *g_4687bc;
					matrix.up = *g_4687b4;
					matrix.left = *g_4687ac;
					break;
				default:
				{
					vector3f_scale((vector3f *)(particle + 0x28), -1.0f, &matrix.forward);
					function_30bf0(&matrix.forward);
					function_11d000(&matrix.forward, &matrix.up);
					cross3f(&matrix.forward, &matrix.up, &matrix.left);
					break;
				}
				}
			}
			else
			{
				vector3f_scale((vector3f *)(particle + 0x28), -1.0f, &matrix.forward);
				normalize3d(&matrix.forward);
				real x = (real)fabs(matrix.forward.i);
				real y = (real)fabs(matrix.forward.j);
				real z = (real)fabs(matrix.forward.k);
				if (y >= x && z >= x)
				{
					matrix.up.i = 0.0f;
					matrix.up.j = matrix.forward.k;
					matrix.up.k = 0.0f - matrix.forward.j;
				}
				else if (z >= y)
				{
					matrix.up.i = 0.0f - matrix.forward.k;
					matrix.up.j = 0.0f;
					matrix.up.k = matrix.forward.i;
				}
				else
				{
					matrix.up.i = matrix.forward.j;
					matrix.up.j = 0.0f - matrix.forward.i;
					matrix.up.k = 0.0f;
				}
				cross3f(&matrix.forward, &matrix.up, &matrix.left);
			}
			matrix.scale = 1.0f;
			matrix.position = *(point3f *)(particle + 0x1c);
			s_particle_system_spawn spawn;
			spawn.scale = *(real *)(particle + 8) / *(real *)(particle + 0xc) - dt;
			spawn.unknown = dt;
			spawn.location_index = child->location_index;
			c_type_4e7709 *child_tag = particle_object_for_tag(child->function_1751d0()->tag_index);
			function_175a80(particle_system->color, 0xffffffff, child, child_tag->multiplied(), child_tag->tinted());
			function_175270(child, &spawn, &matrix, false);
		}
		if (!function_174a30(child, dt))
		{
			particle_system_unlink(child, &particle_system->first_child_index, &particle_system->last_child_index);
			function_174180(child_index);
		}
		child_index = next_index;
	}
	particle_system->flag0 = false;
	return function_1753a0(particle_system) > 0 && TEST_FIELD_BIT(particle_system->flag3) ||
		TEST_FIELD_BIT(particle_system->flag0) || particle_system->first_child_index != NONE || particle_system->flag9;
}


#include <math.h>
struct c_entry_list;


extern dword g_4ba034;

struct s_frustum_set_view;
struct s_sphere_frustum;
struct s_distance_fade;
bool function_165010(s_frustum_set_view const *, long, point3f const *, real, bool *);
long function_181800(s_sphere_frustum const *, point3f const *, real);
real function_249bb0(s_distance_fade const *, real);
bool function_3eb20(long cluster_index);
typedef void (__stdcall *t_1e4e0_callback)(void *);
bool function_1e4e0(point3f const *, t_1e4e0_callback, void const *, long);
struct s_sort_record;
typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);
void function_40e30(short, long, real, long, short, dword, dword, t_record_fill, dword, void *);

/* the particle system definition's flags byte at 0x16, as 0x174420 tests it */
struct s_definition_flags_174420
{
    byte : 5;
    byte flag5 : 1;
    byte : 2;
};

/* `in` rotated into the frame `m` (in may be out) */
static inline void matrix_transform_174420(matrix3x3 const *m, vector3f const *in, vector3f *out)
{
    vector3f copy;

    if (in == out)
    {
        copy = *in;
        in = &copy;
    }
    out->i = (in->j * m->left.i + in->k * m->up.i) + in->i * m->forward.i;
    out->j = (in->i * m->forward.j + in->j * m->left.j) + in->k * m->up.j;
    out->k = (in->i * m->forward.k + in->j * m->left.k) + in->k * m->up.k;
}

static inline void cross_product3d_174420(vector3f const *a, vector3f const *b, vector3f *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
}

// score 135 (was 135): frame and length agree, and so do the entry saves. Left: the tag index lands
// in EAX (retail EDX, 0x137bd0's register) and the extra `mov edx, eax` shifts the camera block;
// retail tests definition bit 5 as `mov dl; shr dl, 5; test dl, 1`; it reloads the location
// after 0x1e4e0 (BLOCKED-BY 0x1e4e0, todo) and calls 0x1751d0 before pushing 0x40e30's arguments.
// Round 74: retail's visibility semantics (not forced, bit 9 clear, cluster not in the set and not
// visible: nothing is tested or stamped and the system draws) 213 -> 207; the fade sets the draw
// flag (`draw = fade > 0`, `draw = true` when visible: retail's `mov bl, 1`) 166 (as `||` 175,
// without the else 170); the child flag copied as a set/clear if/else 135 (1380 bytes). At 135 or
// worse: early location index/request system 134 (one point); tag group as a switch 144, group read
// twice 144, inverted test 135, reference declared first 135, const tag 135, tag through a named
// definition 136, long group 135; bit 5 as byte shifts 135 (still `test byte`), in this function's
// word-flag form 310 (retail's three instructions, but the location loop then runs out of step),
// named 324; the in-view comparison written in place 297 (with the word form 351); cross product
// before/between the camera copies 135; tag read into a local for 0x40e30 166.
// Round 57: the child flag copy as a set/clear if/else (retail's `test word; or word / and byte`)
// gives retail's entry saves (the early exits reach the child loop, which then uses edi) and
// child loop, but the inner tag value lands in eax (retail edx) and the extra `mov edx, eax`
// before 0x137bd0 shifts everything after: 219. On that form: tag/reference/group declaration
// orders 219, group read twice 228, a forceinline reference helper (tag, definition or system
// argument, early return or if/else) 219, inverted or ternary selection 217, 0x23ea80's group
// read 217, definition-pointer reads 219, TEST_FIELD_BIT set/clear 220; with request.system and
// the location index read before the camera block (retail's order) 218, plus the bit-5 test in
// this function's word-flag form (retail's `mov dl; shr dl, 5; test dl, 1`) 217, plus the cross
// product's second products written b-first (retail's xmm0/xmm1) 217; camera inputs from the
// copied locals 242. The tag register stays eax in every form (also compile-only with 0x137bd0
// modelled as an edx argument).
// Native draw 354; native membership 213; translation component loop 312; bias component loop 222; delta component loop 226.
// native request initialization 211; immutable render inputs 213; native definition bit five 317; native
// definition flag word 317; native location fields 211; frame pointer 402; global optimization 434; size
// preference 646; child constant bit stores 220.
// Frame 0x64 agrees; early EDI/tag homes remain. Direct mask chain 222; late native request 331; persistent point workspace 213; bias after camera 213; library point copy 213 (fast).
// Direct render-table expression 222; request location at rendering 327; byte view result 213;
// ordinary while location traversal 213; camera up copied first 213.
// Negative visibility 287; exclusion membership 213; opposite-view classification 213; three negative
// predicates 287; signed render mask 213. Original source restored.
// Unsigned tag handle 213; word draw 354; word visibility 334; word membership 213;
// availability result before the work guard 355. Early EDI/tag lifetimes remain.
// Group-test order, high-bit shift/equality and combinations 213; other-bit OR test 222.
// Round 329: shared pool/data 339/354; group/render-mask/both observations 222/331/331;
// pool with group/mask/both 275/364/364. Original receivers and every memory observation restored.
// Round 376: Independent child cursor/receiver/both entry declarations 213/213/213; native
// request with cursor/receiver/both 211/211/211; both with native request and positive/negative
// child bit stores 340/340. Original lifetimes and stores restored.
// Round 223: Real request parent aggregate/assignment receivers 235/238; with child bit stores
// 369/255; location receiver 230; both aggregate/assignment 244/247; location with child stores 250.
// Metadata lifetimes ending at the shader/table, reference chain or full tag chain give
// 213/213/213; with native request initialization 211/211/211 (two-count gains rejected);
// full tag scope with child set/clear stores 219, with native request as well 217.
// Complete original scopes, request and child-bit copy restored.
// Round 217: Selector inline/noinline/inline-out-of-line boundaries 213/213/213;
// with child set/clear 219/219/219; with word bit five and child stores 316/316
// (fast). Original selector declarations, complete body and call tuples restored.
// Round 29: child set/clear 220; with early request/index 332; with shifted-word bit five 330;
// with b-first cross products 366; duplicate tag carrier 314; conditional reference 220;
// signed group 220; immutable tag 220. Original body restored; tag/group roles and child copy remain.
// Round 248: definition-getter inline/noinline/both declarations 213 each; with child
// set/clear stores 219 each; noinline/both with child stores and native word bit five 316/316.
// Complete getter declaration and body restored; frame agrees, early EDI/tag and child homes remain.
// Round 411: Availability rejection to the common child tail, with original/reference-first/
// separate-tag/group-first declarations 135/135/135/135; definition rejection with the
// same four declaration graphs 135/135/135/135. Original nested body restored; tag/group
// ownership, definition bit five and callback reloads remain.
// Round 411: existing shared lookup alone/field flags/request/both 135/343/133/331;
// with word bit five, alone/field flags/request/both 310/340/129/328. The 133 and 129
// forms are below threshold and restored. Callback inline/noinline/both 125/135/125;
// the inline-only declaration of the external 0x1e4e0 (125) is not kept: no definition in this
// file; complete original body and shared lookup linkage restored.
// Round 35 (lane 9): the camera rotation through the shared vector3f_rotate_into_axes (accumulated rows,
// which match 0x248450) 137: retail's x-row pair order (y * left, then z * up) is right, but the three
// z * up products load the matrix operand first (retail loads z first). One grouped pair then
// accumulated 135, operands written matrix first 137, `t + i` sums 137, column-wise accumulation 137.
// The file-local expression-row helper (135) is kept.
// Round 411: Availability rejection to the common child tail, with original/reference-first/
// separate-tag/group-first declarations 135/135/135/135; definition rejection with the
// same four declaration graphs 135/135/135/135. Original nested body restored; tag/group
// ownership, definition bit five and callback reloads remain.
// @retail 0x174420
void __stdcall function_174420(long system_index, bool view_pass, bool camera_relative, bool force_visible)
{
    s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, system_index);
    if (function_137bd0(system->function_1751d0()->tag_index))
    {
        long tag_index = system->function_1751d0()->tag_index;
        long const *reference;
        dword group = TAG_GROUP(tag_index);
        if (group == 0x5052544d || group == 'prt3')
            reference = function_137bd0(tag_index)->function_x947334();
        else reference = *(long const **)(TAG_GET(byte, tag_index) + 0x24);
        byte *shader = TAG_GET(byte, *reference);
        byte *table = *(byte **)(shader + 0x5c);
        dword render_flags = *(dword *)(*(byte **)(table + 4) + 2);
        s_effect_particle_system_definition *definition = system->function_1751d0();
        if (!TEST_FIELD_BIT(((s_definition_flags_174420 const *)((byte *)definition + 0x16))->flag5))
        {
            real bias = *(short *)definition->unknown14 * 0.05f;
            matrix3x3 camera;
            if (camera_relative)
            {
                camera.forward = g_4b9dac;
                camera.up = g_4b9db8;
                cross_product3d_174420(&g_4b9db8, &g_4b9dac, &camera.left);
            }
            s_particle_render_request_173c90 request;
            request.system = system;
            for (long index = system->location_index; index != NONE; )
            {
                s_particle_location_datum *location = DATUM(g_51ec8c, s_particle_location_datum, index);
                request.location = location;
                bool in_view = ((byte *)location)[2] != 0;
                if (view_pass == in_view)
                {
                    bool draw = true;
                    point3f position;
                    if (camera_relative)
                    {
                        matrix_transform_174420(&camera, (vector3f const *)&location->position, (vector3f *)&position);
                        position.x = g_4b9da0.x + position.x;
                        position.y = g_4b9da0.y + position.y;
                        position.z = g_4b9da0.z + position.z;
                        *(dword *)((byte *)location + 0x30) = g_4ba034;
                    }
                    else
                    {
                        long cluster = system->location.cluster_index;
                        bool visible = function_3eb20(cluster) || (bool)(((dword)*(word const *)((byte const *)system + 0xc) >> 10) & 1);
                        bool in_pool = (g_547f88.flags[cluster >> 5] & (1 << (cluster & 31))) != 0;
                        if (force_visible || (bool)(((dword)*(word const *)((byte const *)definition + 0x16) >> 9) & 1) || in_pool || visible)
                        {
                            if (!visible && !force_visible)
                            {
                                if (!(bool)(((dword)*(word const *)((byte const *)definition + 0x16) >> 9) & 1))
                                {
                                    bool contained = false;
                                    draw = function_165010((s_frustum_set_view const *)g_547f88.context,
                                        ((signed char *)g_547f88.indices)[cluster],
                                        (point3f const *)((byte *)location + 0x20), *(real *)((byte *)location + 0x2c), &contained);
                                }
                                else
                                    draw = function_181800((s_sphere_frustum const *)((byte *)g_547f88.context + 0x94),
                                        (point3f const *)((byte *)location + 0x20), *(real *)((byte *)location + 0x2c)) != 0;
                            }
                            if (draw)
                            {
                                if (visible)
                                    system->flag10 = true;
                                else
                                    system->flag10 = false;
                                vector3f delta = {location->position.x - g_4b9da0.x,
                                    location->position.y - g_4b9da0.y, location->position.z - g_4b9da0.z};
                                if (!visible)
                                {
                                    real distance = (real)(sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i) * g_4b9dcc);
                                    draw = function_249bb0((s_distance_fade const *)definition, distance) > 0.0f;
                                }
                                else draw = true;
                                *(dword *)((byte *)location + 0x30) = g_4ba034;
                            }
                        }
                        position = location->position;
                    }
                    if (draw)
                    {
                        position.x += g_485618.forward.i * bias;
                        position.y += g_485618.forward.j * bias;
                        position.z += g_485618.forward.k * bias;
                        if (render_flags & 0x8000)
                            function_1e4e0(&position, (t_1e4e0_callback)function_173c90, &request, sizeof(request));
                        if (render_flags & ~0x8000)
                            function_40e30(0, system->function_1751d0()->tag_index, 0.0f, 5, (word)NONE, NONE, 0,
                                (t_record_fill)function_173cb0, (dword)function_173d80, (void *)system_index);
                    }
                }
                else *(dword *)((byte *)location + 0x30) = g_4ba034;
                index = location->next_index;
            }
        }
    }
    for (long child_index = system->first_child_index; child_index != NONE; )
    {
        s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
        if (system->flag10)
            child->flag10 = true;
        else
            child->flag10 = false;
        function_174420(child_index, view_pass, camera_relative, force_visible);
        child_index = child->next_index;
    }
}



long function_155760(long index);
bool function_17afd0(long effect_index, long value);

// score 102 (was 102): an earlier round: the availability flag written as `query(view) == 0` 102 (the
// same code as the negation), the definition's bit 9 as (bool)(((dword)word >> 9) & 1) 128 (491
// bytes: retail's movzx/shr/test shape, but the surrounding allocation gets worse). Retail's
// availability flag lives in bl because its NONE sentinel lives in ebp and frees ebx; ours keeps
// the sentinel in ebx, so the flag lands in al and the same-view in bl instead of a stack byte.
// The sentinel's register and the Boolean homes remain.
// Earlier (102): frame 0x14 agrees; retail keeps the NONE sentinel and the owner in ebp
// Frame 0x14 agrees; retail keeps the NONE sentinel and the owner in ebp
// (coalesced; later NONE tests use immediates) and the availability flag in bl; ours keeps the
// sentinel in ebx, the owner in edi and the flag in al, and tests the definition's bit 9 as a
// byte instead of a shifted word. Owner/NONE coalescing and Boolean homes differ.
// View read from the global at each use 143 (gives the reload after the attachment query);
// bit 9 as (word >> 9) & 1 102 (still a byte test), with view at use 143; view at use with the
// flag false-then-set 148 (flag in a stack byte) or as view != NONE && !query 146; owner as a
// conditional expression 149, with view at use 143, plus the && flag 144.
// Next-slot assignments 107; word absolute slot 158; separate scan result 107;
// shared datum conversion 199; long attachment 120; short same-view 145; byte draw 107;
// long camera-relative 144 (fast preview 107, full 106). Original body retained.
// Round 109: unavailable predicate 148; other-view predicate 146; detached predicate 108;
// fixed-camera result 148; external system reference 107; nullable effect view 149;
// const definition 107; packed effect handle 107 (fast preview 107, full 106).
// Round 18: the availability gate (flag from view != NONE, then the query result if
// set) 101 plain, kept; stack-bool rename 102; bit 9 as a named bitfield 102; nested
// attached else-if 102; wide view 102; wide effect index 102; two-step same-view 102;
// flag as false-init plus conditional set 144 (fast previews 107/102/144).
// NONE/owner coalescing and Boolean homes remain; fast control 102.
// Implied owner validity/availability-first equality 170/172; classification scope 102; scope
// with implied validity 170; short/signed-byte availability 147/102; combined forms 148/102.
// NONE/owner coalescing, BL availability and Boolean homes remain.
// View-query inline/noinline, attachment noinline, draw inline/noinline,
// scan inline/noinline and paired predicate boundaries all 102 fast, tying preview.
// NONE/owner sharing and availability/Boolean homes remain.
// Next-used helper entry/advance/both 200/202/244; native draw/query+draw bits 101/128;
// native bits with entry/advance helper 195/198; inline/outline-inline query 101/101.
// Arm-specific draw calls with original/word flag 134/137; camera scoped 143/129;
// arm-specific draw and camera 144/121; direct other-view eligibility 144/121.
// NONE/owner sharing and Boolean homes remain; original shared draw call retained.
// Round 290: Complete effect observations, pass-selected draw and shared cluster-bit boundary
// subsets 106/143/144/102/106/143/144; complete pass result arms 144. Original executable source
// restored.
// Round 189: Complete pool-global observations 102; implied back-edge NONE arm removed 150;
// existing calling iterator with aggregate/field initialization 252/252 (exact-only). Coordinated
// cluster-provider inline alone/pool/iterator/advance 102/102/252/150. All provider declarations
// and the original body restored.
// Lane 13 r20 (fast): with view read from the global at each use, bit 9 as ((dword)word >> 9) & 1 and
// availability as one `view != NONE && !query`: 123; owner declared first 125; availability as false-init
// plus conditional set 138; owner assigned in an if/else 113 at retail length (effect index in EDI as
// retail; NONE stays in EBX because availability lands in AL); bit 9 form alone 128; if/else owner with the
// false-init availability 138 (availability spilled, same-view takes BL), without the bit form 150, with a
// view local 138.
// Round 43: bit 9 as (bool)(((dword)word >> 9) & 1) gives retail's movzx/shr/test 128 (491 bytes); with
// the view global read at each use 124 (retail length, and retail's view reload after 0x17afd0); plus
// the availability as one `g != NONE && !query` 123 plain (aligned 63 -> 51; NONE/owner in ebx/esi
// against ebp, availability in al against bl, same-view in bl against its stack byte remain); with
// false-then-set availability 138 (508 bytes) or same-view 122 (499); view global alone 143; attached
// computed only when not same-view 163 (retail computes it on both paths), with the forms above 121-152.
// Round 312: same-view classification after attachment on the retained availability gate,
// bool/byte/short/long attachment with original/direct camera result 100/125/100/125/111/122/110/122.
// Two-point gain restored; later-result forms can coalesce owner/NONE but regress overall. Original snapshot and body restored.
// Round r4 (lane 17): camera flag renamed to sort after same_view 102 (slots unchanged: not name-ordered here); camera
// flag declared before same_view 103; shifted word bit 9 + view global at each use + false-then-set availability 138
// (508 bytes); owner = NONE at the top of the loop body 141 (488); same_view assigned in if/else arms 102 (same code).
// Round 27 (lane 13, fast): availability false-then-set 144 (availability spilled, same-view in BL, NONE in EBX);
// plus same-view declared before availability 144; plus owner in an if/else 142/142; plus the view global at each
// use 148/148, with the if/else owner 150/150, with the shifted bit 9 138/138 (510 bytes); the effect index read
// from the field at each use 106; one-expression availability + view at each use + shifted bit 9 + if/else owner
// 113 (498 bytes; owner in ESI, view in EBP, NONE in EBX), without the owner form 123.
// Round 61 (lane 16, fast): all 71 combinations of six forms (the view read from the global at each use; bit 9
// as (bool)(((dword)word >> 9) & 1); the availability as the two-step test, as false-then-set
// `if (view != NONE && !query(view)) flag = true;` or as one `view != NONE && !query(view)`; the owner in an
// if/else; same_view set in if/else arms; same_view declared before the availability): none below 102 (the
// same-view arms and its declaration give the same code). Best at retail's 498 bytes: view at each use, bit 9
// shifted, one-expression availability and the if/else owner 113 (aligned 63 -> 51; the NONE sentinel stays
// in EBX where retail has EBP coalesced with the owner); with false-then-set availability 138-148 (the flag
// spills to a stack byte and same_view takes BL, retail the other way round).
// Round 105 (lane 7, fast): with the view global at each use, the shifted bit 9 and one-expression availability
// 123 (496 bytes; NONE stays in EBX, so owner = NONE is a copy, not retail's EBP coalescing); availability
// false-then-set (retail's `xor bl, bl` before the query) 138 (508 bytes, availability spilled, same-view in
// BL), with the owner in an if/else 138, with same-view in an if/else 138, with the camera flag declared
// before the owner 138; one-expression availability with the if/else owner 113 (498 bytes); owner declared
// before the effect index 125; availability declared at the loop top 123. Retail keeps -1 in EBP until the
// owner takes EBP; ours keeps -1 in EBX all through, so five values compete across the view query.
// Round 20 (lane 17, fast): the cluster test through the existing bit_vector_test 102 (same code, kept in
// place of its expression written out); on it, lane 13 r27's retail-length form re-measured as a base (the
// view global at each use, bit 9 as (bool)(((dword)word >> 9) & 1), one-expression availability and an
// if/else owner) 113 (498 bytes; NONE in EBX, owner in ESI, view in EBP, availability in AL, same view in
// BL), and its false-then-set availability re-measured 138 (510 bytes); new on that base: the camera flag
// declared before the availability 115; the same view false-then-set 121 (501 bytes). On the kept form:
// the loop as `for (long index = ...; index != NONE;)` (this file's own form above) 102 (same code); the
// availability as `view != NONE ? !query(view) : false` 107 (485 bytes).
// @retail 0x174220
void __stdcall function_174220(bool view_pass)
{
    s_record_pool *systems = g_510c74;
    long index = data_datum_index(systems, function_16bc00(systems, 0));
    while (index != NONE)
    {
        s_particle_system_datum *system = DATUM(systems, s_particle_system_datum, index);
        s_effect_particle_system_definition *definition = system->function_1751d0();
        short cluster = system->location.cluster_index;
        if (cluster != NONE &&
            ((*(word *)((byte *)definition + 0x16) & (1 << 9)) ||
             bit_vector_test(g_547f88.flags, cluster) || function_3eb20(cluster)) &&
            system->unknown4c == NONE && system->location_index != NONE)
        {
            long effect_index = system->effect_index;
            definition = system->function_1751d0();
            long owner = NONE;
            if (effect_index != NONE) owner = DATUM(g_4ea93c, s_effect_datum, effect_index)->unknown58;
            long view = g_4b9ed8;
            bool local_287b6e_2 = view != NONE;
            if (local_287b6e_2) local_287b6e_2 = !function_155760(view);
            bool same_view = owner != NONE && view == owner && local_287b6e_2;
            bool attached = local_287b6e_2 && effect_index != NONE && function_17afd0(effect_index, view);
            bool draw = same_view == view_pass;
            bool camera_relative = false;
            if (same_view)
            {
                if (view != NONE && *(short *)((byte *)g_4ed284 + view * 0x94 + 0x42) != NONE)
                    draw = (((byte *)definition)[0x16] >> 6) & 1;
                if (definition->unknown0c != 0) camera_relative = true;
            }
            else if (attached && definition->location_mode == 2) draw = false;
            if (draw) function_174420(index, same_view, camera_relative, false);
        }
        long next_index = index == NONE ? 0 : (index & 0xffff) + 1;
        systems = g_510c74;
        index = data_datum_index(systems, function_16bc00(systems, next_index));
    }
}
