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
	s_particle_location_datum *location;
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


// MiniMax R (11-14): the emitter list test wrapped around the loop with the offset's zero store
// inside it, and the scale declared before the multiplier, measure 39 (was 42) at 1130 bytes; not
// kept, since the gain is under eight and the score stays above 30. Left: row i of both rotations
// sums the up product before the forward one, the two saved registers are reloaded in the other
// order after each property call, the copy's second and third components swap, and the branch
// offsets and the two trailing bytes follow from those lengths.
// score 42 (was 42): left: the offset's zero store before the list test, the 1.0 defaults' order, three
// reload pairs and row i of both rotations. Round 176 (m3 lane 5, fast): on indexed emitter definitions with
// scale first (re-measured 113 at 1136 B; every integer difference fixed, only the rotations differ: VC
// reassociates each row's three-term sum, retail row i (left*y + forward*x) + up*z, ours (up*z + forward*x)
// + left*y): `unknown0c != 0` 113, the 0x107f0 refresh mask named 113, the location flag as a (bool) cast
// 113 (all same code), the definition formed before the emitter lookup 124; on the body the emitter mask read
// before the defaults 204.
// MiniMax A (11-12): the emitter list test wrapped around the loop with the offset's zero store
// inside it: 42 -> 39 (1130 bytes), the store now follows the list test as in retail.
// MiniMax A's form, not kept: 39 (was 42): the offset's zero store now follows the list test as in retail; left: row i of
// both rotations sums the up product before the forward one (retail sums the forward one first and
// loads forward.i into a register), the two saved registers are reloaded in the other order after
// each property call, and the branch offsets and the two trailing bytes follow from the rotation
// rows' lengths.
// MiniMax b (11-11): no new form, no lower score (42): the emitter loop as an explicit list test
// with the scale declared before the multiplier was measured at 39 and restored, since it stays
// above 30 and the gain is small; row i of both rotations still sums the up product first.
// MiniMax E (11-11): no new form tried, no lower score (42): row i still sums the up product before the
// forward one, the offset's zero store still comes before the list test and the particle is still reloaded
// before the emitter definition after each property call; the shared rotate helper in
// include/matrix_transform.h keeps the sum order, and the body is unchanged since round 20's permuter run
// (58 tries, none below 42), so it stands.
// Earlier 42 (was 42): both in-place rotations go through the shared rotate helper (it copies the
// input when the output is the input), which gives retail's j and k rows. Left: row i sums the up
// product before the forward one (retail forward first, loading forward.i into a register); the
// offset's zero store comes before the list test (retail after); the two 1.0 stores are swapped; and
// after each property call ours reloads the particle before the emitter definition (retail the
// other way round).
// MiniMax C (11-09): no new form, no lower score (42); row i still sums the up product before the forward one and
// the emitter loop still reloads its two saved registers in the other order; the shared rotation in
// include/matrix_transform.h keeps that sum order.
// MiniMax I (11-08): no new form, no lower score (42): row i still sums the up product before the forward
// one, the offset's zero store still comes before the list test, the two 1.0 stores are still swapped, and
// after each property call ours still reloads the particle before the emitter definition. The two in-place
// rotations go through the shared forceinline helper in include/matrix_transform.h, whose product order
// this file cannot change.
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
// Round 39 (m3 lane 14, fast): scale first with `+=` translation 40; that with the emitter definition formed before
// the particle index 40; the motion copy declared before the position copy 50, with scale first 48; the next index
// read before the particle-mask refresh 58. Diagnostic one-use locals (removed): at the top, after the emitter
// lookup or before the multiplier 109 (the rotation rows move to another of their cycle states, nothing else
// fixed); after the particle mask or the particle lookup 42 (no change).
// Round 20 (lane 17, fast): permuter 600 s, 58 tries, none below 42. Row i's input copy is stored in the
// order the row consumes it (ours y, z, x; retail y, x, z), so the copy follows the sum order. Both
// rotations fed from a separate input copy (`position = translated_position`, rotated into it; the same
// for the velocity) 123 (1136 bytes, row i unchanged); indexed definitions with scale first 113
// (re-measured), plus the separate input copies 115 (1138 bytes), with the emitter mask read from the
// definition at both uses 232 (1130 bytes); the separate input copy for the position rotation only 42
// (same code: VC folds it into the helper's own copy).
// Round 295: Mutable per-emitter multiplier/scale/both defaults before the first-particle read [55, 54, 55],
// before the emitter lookup [55, 54, 55], pre-gate multiplier/both [55, 55].
// All overrides and memory observations stayed fixed; original defaults and complete body restored.
// Round 39 (m3 lane 14, fast): scale first with `+=` translation 40; that with the emitter definition formed before
// the particle index 40; the motion copy declared before the position copy 50, with scale first 48; the next index
// read before the particle-mask refresh 58. Diagnostic one-use locals (removed): at the top, after the emitter
// lookup or before the multiplier 109 (the rotation rows move to another of their cycle states, nothing else
// fixed); after the particle mask or the particle lookup 42 (no change).
// Round 299: Word carried particle mask with indexed definitions, scale-first defaults and early pure mask
// derivation, masks 0..7: 199/201/197/199/259/260/260/261. Original dword mask, offset/default lifetimes and
// body restored. Shared rotation/frame providers stay matched; offset/default/reload and row-i input/product
// residence remain.
// Round 295: Mutable per-emitter multiplier/scale/both defaults before the first-particle read [55, 54, 55],
// before the emitter lookup [55, 54, 55], pre-gate multiplier/both [55, 55].
// All overrides and memory observations stayed fixed; original defaults and complete body restored.
// Round 453: Unscaled property residence with callback/pre-rotation scale product, indexed
// definitions and scale-first defaults, masks 0..7: 304/308/305/309/315/316/315/316.
// Original executable body restored.
// Round 337: returned rotation consumed through shared addition/three-use pointer, with
// native emitter indexing and scale-first defaults: 122/124/117/121/120/122/115/119.
// All lengthen and regress; original in-place rotations, translation and whole body restored.
// Round 455: Original particle position/motion captures through memcpy/native,
// native/memcpy, both memcpy, memmove/native, native/memmove, both memmove,
// memcpy/memmove and memmove/memcpy: 42/42/42/193/177/210/177/193. Original native copies restored.
// Round 457: Native emitter-row cursor across readonly rows, scale-first defaults and complete
// mask-field observation combinations; original shared rotations and callback inputs preserved.
// Scores 270/270/269/269/298/298/299/299. Original executable body restored.
// Round 459: Double scale/multiplier/both storage, scale-first variants and indexed single-wide variants
// 328/320/322/328/320/322/311/324. Original executable source restored.
// Round 461: Conditional validity results at system/location/particle cache transitions,
// subsets 1-7, 140/131/144/179/171/161/192; one shared entry validity result 283.
// Original invalidations, masks, offset/default lifetimes and shared rotations restored.
// Round 74 (lane 9, fast): scale first with `scale = value; scale *= multiplier` in the emitter gate or the
// particle gate 120/120 (1136 bytes); scale first with the definition computed before the particle index 40;
// the next index read before the particle-mask refresh 58; scale first with the two copies swapped 48; scale
// first with the transform flag in a named bool 107; scale first with `definition->unknown0c != 0` 40.
// Compile-only, about 500 sets of indexed definitions, scale first, masks inline or at each use, `+=`, swapped
// copies, next index early or late and named values: the rotations never get closer than row i's up/forward
// pair (score 20 of 65 ops against 22 now); indexed with scale first and the mask at each use has the current
// rotations but the integer homes turn over (232, round 61).
// Round 343: Native property-bit views, alpha/color/scale subsets 1..7 with Boolean conversion
// 124/224/229/42/124/224/229; all bits tested directly 199. Original real flag storage,
// all six observations, shared rotations and complete body restored; offset/default/reload and row-i schedule remain.
// M3 lane 4 r157 (fast): the particle pointer declared at the emitter body's top or before the particle loop, the
// emitter pointer declared before the emitter loop 42/42/42 (identical); indexed definitions with scale first 113
// (re-measured, 1136 bytes), with the motion rotated before the position 116; the emitter-mask refresh before the
// frame query 140 (1131 bytes); the particle copies after the particle-mask refresh 340 (1172 bytes).
// AC495: Actual cache identity field system/location/particle pointer types, seven subsets,
// then all with immutable emitter-row receiver: eight at 42, 1130 bytes. Original field types,
// declarations and complete body restored; offset/default/reload and row-i residence remain.
// M3 lane 4 r159 (fast): scale first with the emitter definition declared first 40; declared first alone 42; offset as a for-loop init with scale first 40;
// on the indexed form with scale first: scale through a shared inline 113, rotated offset local 196 (1086 bytes), motion rotated first 116, matrix local 112
// (1134 bytes), `+=` origin 113, particle copies swapped 121.
// Round 122 (lane 7, fast; base 42 [aligned 38]): position rotated from the particle's own position into
// the copy (not in place) 189 (1131 bytes); motion rotated from the particle's velocity into the copy 143
// (1136); origin/result pointers declared before the rotation 42 (same code); scale first with the motion
// rotated before the position 90; scale first with origin declared first 40 [36] (the 1.0 stores only).
// M3 lane 14 r59 (fast, base 42): row i differs only in its second and third terms (ours left*y, up*z, forward*x;
// retail left*y, forward*x, up*z); the copy's register mapping agrees (y EAX, x EDX, z ECX), only its order
// follows the sum. The emitter definition formed inside the list block after the 1.0 defaults 56 (its address
// then follows the list test); the particle copies after the particle-cache invalidation 181; indexed definitions
// with scale first formed inside the list block 130 (1136 bytes); the first particle read from the emitter at the
// guard and again for the loop 293 (1145 bytes); the position rotated through the existing
// vector3f_rotate_into_axes, the motion through the shared helper 129 (1140 bytes).
// AC515: Cache invalidation mask-before-identity stores at system/location/particle, subsets 1..7,
// then all native mask computations before the original identity/mask stores: 283/279/286/169/285/282/289/53.
// NOT kept: no exact result from store/result diagnostics; original invalidations and body restored.
// AC517: Both shared unit-default assignment chains with both scalar declaration priorities:
// original offset 120/122/120/122; native row index 115/117/115/117. NOT kept: all regress and lengthen.
// Original defaults, offset walk, shared rotations and body restored; offset/default/reload and row-i order remain.
// AC519: Complete nonempty/empty emitter outcomes, bool/byte/short/long,
// 109/109/109/109/109/109/109/109. NOT kept: regressions from native admission
// storage; original gate, offset, defaults, shared rotations and body restored.
// AC524: Actual particle-loop continuation bool/byte/short/long, then complementary
// completion results, 42 each at 1130 bytes. NOT kept: offset/default/reload and
// row-i schedules remain; original guards, shared rotations and complete body restored.
// AC527: Shared actual alpha/color/scale property receivers, subsets 1/2/4/3/5/6/7,
// then complete receivers after the frame query: 247/117/117/258/258/135/257/240. NOT kept:
// pointer residence unmatched and extent grows; original observations, rotations and body restored.
// AC531: Complete six-use cache-value array reference/pointer, entry/post-invalidation/
// emitter/post-frame acquisition: 113/113/113/122/113/113/113/122. NOT kept:
// property-input residence grows length; original observations, helpers and body restored.
// AC535: Actual outer-emitter continuation bool/byte/short/long, guard-owned then
// entry/latch-owned: eight at 109, 1130 bytes/frame 0x98. NOT kept: premature row-offset
// zero store and row-i/default/reload gaps remain; original complete body restored.
// AC537: Actual cache-mask/property-flags/emitter-mask signed 32-bit field domains,
// seven subsets then all int: 42/44/44/42/42/44/44/44. NOT kept: no gain; original fields,
// shared alias-safe rotations and entire body restored.
// AC539: Independent frame/rotation mode observations at both original gates, raw
// short/word/long/dword 117/117/117/117, bool/byte/short/long truth 109/109/109/109.
// NOT kept: one-use field captures unmatched; original reads, helpers and full body restored.
// AC541: Paired entry invalidation as clear-bit/keep-bit/mask-value/conditional-bit
// results, dword then long: 288/288/260/260/283/283/288/288. NOT kept:
// shared mask publication does not match native separate stores and length grows;
// original identity invalidations, shared alias-safe rotations and whole body restored.
// AC543: Callback-position vector backing with native/memcpy/memmove/shared-setter
// captures, original byte offset then native row index: 42/42/193/281/115/115/192/263.
// NOT kept: no equality/gain; setter and indexed forms change extent or early allocation.
// Original point backing, alias-safe rotations, observations and full body restored.
// B446: Shared complete emitter/particle property evaluator, free/member provider,
// color/scale pointer/reference outputs: 77/77/77/77/77/77/77/77. NOT kept: no exact result;
// original complete body, types and helpers restored. Offset/default/reload and row-i schedules remain.
// M3 lane 8 r135 (fast and a compile-only screen of about 3300 forms; aligned in brackets, base 42 [38]): row order follows
// one global tuple phase (period 4; extra products in any row or at the callback only shift it) and the rotate helper's
// symbol numbering, which earlier forms never moved: a scratch copy taking (vector, matrix, out) or (out, vector, matrix)
// gives odd row i states 45 [45]; with a `result` local for out in the helper 40 [36], scale first 38 [34]. On indexed
// definitions with scale first (113, every integer difference fixed): helper (vector, matrix, out) with `result` and a
// `matrix` local 39 [33] at 1130 bytes, no chain, only product forms left; with `result` alone and a diagnostic 4-deep
// no-op chain 31 [26], the sums retail's and only row i F*x and row j L*y copy-first in both rotations (`movaps t, x; mulss
// t, [m]` for `movss t, [m]; mulss t, x`); that helper with the mask read at each use has `yxz xzy zxy`, retail's rows, in
// both rotations but the homes turn over 229-231 (retail keeps the mask's slot). In the 31 state: row term orders (216)
// identical, 256 named-term subsets 13-26 block differences against 13, operand swaps identical, helper shapes
// (unconditional copy, block-scoped copy, source pointer, in/matrix locals, dummy parameters) and six parameter orders no
// better. Index spellings (typed sum, dword, ++, += 1) identical; declaration moves and the particle mask inline identical.
// Named input components x, y, z in every subset, order and phase (336 forms) no better than 13 block differences.
// Left for a match: all products load-first in the retail phase. A new one-caller helper is match-only: body unchanged.
// Round 95 (m3 lane 12, fast): the emitter loop as a for with the offset as its init and step 42 (the zero store stays ahead
// of the guard; retail stores it after the guard, beside the shrink-wrapped push ebp); scale first re-measured 40 (gain 2);
// the offset declared before the emitter index 42; the particle copies motion first 50; the next index read right after the
// particle lookup 272 (1113 B); scale first with the colour declared before the frame locals 40; scale first with the
// particle mask formed beside the emitter mask 261 (1117 B).
// M3 lane 11 r76 (fast; aligned count in brackets): the motion rotated through the existing vector3f_rotate_into_axes,
// the position through the shared helper 127 (1138 bytes) [105]; indexed definitions with scale first re-measured
// 113 (1136) [89]; on that, the motion through vector3f_rotate_into_axes 119 (1140) [96], the emitter-definition
// block base named once before the loop 284 (1118) [176], the motion rotated from the particle's velocity into its
// copy 141 (1140) [135], plus the position rotated from the particle's position into its copy 199 (1143) [221].
// Lane 9 r99 (fast): matched 0x248c60 walks the same emitter chain with `objects + index * 0xb8` and a for-loop
// index; that spelling here 115 (1136 bytes, the indexed form's code), with scale first 113; also with the emitter
// read as `&data[index & 0xffff]` 115; the frame locals (rotation, origin, both result pointers, colour) at function
// scope 42 (same code), with the sibling spelling and scale first 113. Re-measured AC517's chains `multiplier =
// scale = 1.0f` / `scale = multiplier = 1.0f` 120/122 (1136 bytes), on the indexed form 115/117 (1138 bytes).
// T79: Independent alpha/color/scale admission results at all original emitter/particle flag
// observations, bool subsets 1/2/4/3/5/6/7 then all long: 155/217/136/234/143/236/227/173.
// NOT kept: unmatched one-use decisions grow the extent; original complete body and all
// shared alias-safe rotations restored. Offset/default/reload and row-i schedules remain.
// Round 84 (m3 lane 1, fast with 0x174220, 0x1751d0, 0x174420 and 0x2477b0 listed, 42 [22] as the full check; aligned count in
// brackets): the position's rotation and translation in one inline point-transform helper built on the shared rotate helper, with the
// parameters (rotation, origin, point, out), (point, rotation, origin, out) or (out, point, rotation, origin) 121 (1134 bytes) [63]
// each, with scale first 119 [62] each; one inline helper for both the position and the motion 121, with scale first 119; the point
// helper with a thin inline vector-transform wrapper for the motion 121, with scale first 119. Every wrapper gives one state: the
// helper's copy is made through the wrapper's parameters and all three rows of both rotations turn. Body unchanged.
// M3 lane 2 r67 (fast): indexed definitions (`&emitters[index]`, index++ at the latch) with scale first re-measured 113
// (1136 bytes; the zero store then follows the guard as retail's strength-reduced offset); forms listed above for other
// bases, re-measured on it: both rotations through the existing vector3f_rotate_into_axes 102 (1136 bytes; rows j and k
// then differ too), the position through it and
// the motion through the shared helper 117 (1140), both through it with the motion rotated first 115 (1136), both through
// it with the emitter mask read from the definition at both uses 237 (1130 bytes, the homes turn over); on this body both
// rotations through vector3f_rotate_into_axes with scale first 82 (1130 bytes).
// M3 lane 14 r80 (fast, base 42): the next particle index read right before the callback 203 (1106 bytes); the emitter
// definition formed as the first statement inside the list block, before the 1.0 defaults, 57; both scale products written
// multiplier first 42 (same code); the multiplier test through the existing flag getter 0x173b90 273 (1110 bytes); the next
// particle index read between the two particle copies and the particle-cache invalidation 273 (1113 bytes).
// m3 lane 14 r82 (fast): register/esp-masked aligned diff 13 rows (row i's sums in both rotations, the offset's zero store ahead of
// the list test, two reload pairs); indexed definitions re-measured 115 (1136 bytes, masked 55: the integer rows become retail's and
// every rotation row reorders). The body is unchanged since round 20's permuter run (58 tries, none below 42), so it was not rerun.
// Also r82, each with scale first (re-measured 40): the 0x107f0 mask named 40 (same code); the offset declared at the function top
// 63; the emitter-mask refresh before the frame query 140 (1131 bytes, as listed alone); the multiplier as a ternary at its
// declaration 259; the particle copies after the particle-cache invalidation 179; the position rotated from the particle into its
// copy 189 (1131, as listed alone); the translation through the existing vector3f_add 120 (1136); the next index read right after
// the particle lookup 272 (1113, as listed alone); the emitter definition formed in the list block after the defaults 57; the first
// particle read at the guard and again for the loop 310 (1162).
// m3 lane 15 r74: the listed indexed-definition form (fixes the zero store and the three reload pairs)
// turns both rotations through the shared rotate helper, whose product phase this file cannot change. Fast: indexed
// definitions with scale first re-measured 113 (1136 B, masked 55); the index as a for counter 113 (same code); on it
// the motion copied first 121, the motion rotated first 116, the emitter definition formed inside the list block 128,
// scale declared just before its emitter-level test 251 (1138 B), the particle copies after the cache invalidation 193.
// M3 lane 12 r115 (fast with 0x173c10/0x2477b0): the definitions indexed by a short counter 271 (1130 bytes); the
// defaults as one chained `multiplier = scale = 1.0f` 120 (1136), on the long-indexed form 115 (1138); a named flags
// value for the six emitter-definition flag tests 292 (1157), on the long-indexed form 297 (1159).
// NOT kept (coordinator review, MiniMax A 11-12): 42 -> 39 is under the keep bar (8+ on a function still above 30); the body is main's again.
// Round 178 (m3 lane 5, fast with 0x173c10/0x175430/0x2477b0, control 42): the placeholder-local scan that matched 0x1e6360
// (n unused `long` locals at the function top, n = 0..7; diagnostic, removed) on four structures. This body: 42, 109, 53, 53,
// 57, 57, 57, 70; with scale declared first: 40, 107, 51, 51, 55, 55, 55, 68 (all 1130 bytes); indexed definitions: 115, 106,
// 118, 118, 113, 113, 113, 118; indexed with scale first: 113, 104, 116, 116, 111, 111, 111, 116 (1132-1138 bytes). The local
// count does move both rotations here (every row turns at n = 1), but no count beats zero on any structure: row i's sum
// order (retail left*y, forward*x, up*z) is not a product-phase state, and the indexed form's rows differ in their sum
// order as well, so the count is not the lever for this function. One placeholder on the indexed form gives retail's
// copy order (y, x, z into EAX/EDX/ECX) with the sums still (up*z + forward*x) + left*y.
// P460: Callback-only position/motion/both rotation-return receivers, mutable/read-only, then complete early lifetimes: 122/122/126/122/122/126/126/126. NOT kept: output aliases miss equality and add 6-8 bytes; original shared rotations and whole body restored.
// M3 lane 7 r158 (fast, base 42): on the emitter walk as `if (emitter_index != NONE) { offset = 0; do ... while }` (41
// alone): scale first 39 (left: three reload pairs, row i of both rotations, branch offsets); with both refresh masks inline 39;
// the emitter definition formed before the emitter lookup 44; `+=` translation 39; the motion rotated first 89; the particle
// loop as do/while under its `if` 39.
// P484: Linked-list for successor with empty-emitter continue, byte offset/row ordinal
// 212/235; nested admission 42/115; post-admission definition receivers 260/262;
// pre-admission unity defaults on the continue graph 210/233. NOT kept: no equality/gain;
// original observations, offset/default timing, shared alias-safe rotations and body restored.
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
		transform4x3f matrix;
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
		long next_index = child->next_index;
		byte *particle = record_pool_lookup_checked(g_51ec84, child->unknown4c);
		if (particle)
		{
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
				function_11d000(&matrix.forward, &matrix.up);
				cross3f(&matrix.forward, &matrix.up, &matrix.left);
			}
			(void)matrix.forward.k; /* no-code read: retail keeps forward.k live here; match-only form */
			matrix.position = *(point3f *)(particle + 0x1c);
			matrix.scale = 1.0f;
			dword color = particle_system->color;
			s_particle_system_spawn spawn;
			spawn.scale = *(real *)(particle + 8) / *(real *)(particle + 0xc) - dt;
			spawn.unknown = dt;
			spawn.location_index = child->location_index;
			s_effect_particle_system_definition *child_definition = child->function_1751d0();
			c_type_4e7709 *child_tag = particle_object_for_tag(child_definition->tag_index);
			function_175a80(color, 0xffffffff, child, child_tag->multiplied(), child_tag->tinted());
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

// MiniMax S (11-06): the permuter's 41 rewrites all score 135 or worse on the reduced image; no new form, no lower score.
// score 108 (was 135): lane 9 r99: the loop reads the request's location field at every use (typed as a location
// pointer; retail reloads it from the request after 0x1e4e0), the camera rotation and cross product go through the
// existing vector3f_rotate_into_axes and cross3f, the second getter call's result is kept as a pointer whose tag is
// read at each use, and bit 5 is the definition's named flag5 (retail's mov dl/shr/test). Left: the tag is
// read per path (retail once into EDX), the location index and request are read after the camera block (retail
// before: EAX holds the index, so its camera copies avoid EAX), view_pass is loaded late at the loop top, the up
// row's products load the matrix first, and the 0x165010/0x1e4e0/0x40e30 argument setups differ.
// Lane 9 r99 trials (fast, plain-confirmed where kept): request field at each use 125 (aligned 82 -> 87), plus
// vector3f_rotate_into_axes 118, plus cross3f 118 (same code; kept as base 118); on 118: an early-return inline
// lookup helper 118, the request before the camera block 118, the cross product before/between the copies 118/118,
// bit 5 as `(byte >> 5) & 1` 118, as the shifted word 320; the second getter's result as a pointer with the tag
// read at each use 113 (one `definition` for both calls 113), plus the shifted-word bit 5 108, plus the named
// header bit 108 (kept with the separate pointer; one shared `definition` 108 too); on 108: the tag in a local
// read from that pointer 290 (aligned 60: retail's EDX tag, but the camera copies use EAX and the loop padding
// grows), plus the index and request before the camera block 62 (aligned 53; match-only: moved declaration), with
// only the request moved 284/285; the index and request moved alone 112; TAG_GROUP read twice 108.
// Earlier 135 (was 135): frame and length agree, and so do the entry saves. Left: the tag index lands
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
// Round 500: native camera/request/position lexical-home subsets, all seven,
// then all with displacement identity, 135 each on the recovered child-tail graph.
// Original names/body restored; tag/group ownership, flag extraction and callback reloads remain.
// Round 141 (KIMI1, fast): tag assigned in the group expression 135; split group/tag/both assignments
// 135/135/135; request-location reread after the callback 116 (retail's reload, NOT kept: a one-use
// field reread after a call); that with the existing tag reused at the final getter 152; request-owned
// loop advance 143; shifted-word bit five on the reread graph 320. Original executable body restored;
// tag/group ownership and final getter scheduling remain.
// M3 lane 7 r158 (fast; plain where marked; base 108): a path to 6 (plain 6 at 1380 B, first difference +0x20f) with every step
// together; not kept, since its moved statements and one-use locals are match-only (diff saved as build/r158/r158-174420-six-
// form.patch): 0x1e4e0's parameters as (size, position, callback, data) (retail's call sets EDX = position before ECX = size,
// and 0x1e4e0 reads them there) 106; the definition's tag index in a local for the group, 0x137bd0 and the lookup 291 alone;
// `request.system = system;` and `long index = system->location_index;` ahead of the bias and the camera block, the index
// before the bias, 31 (the entry and the camera copies retail's; the other three orders 58/60/33); the frustum index read into
// a local before 0x165010 (as 0x41640 does) 26; the 0x40e30 call's definition fetched into a local first 19; both locals 41 (the
// rotation's phase turns), with one existing one-use local folded (in_pool, distance or shader) 12; the camera built as forward
// copy, cross product, up copy (0x248450's order) 17 / 12; the left axis through particle_cross_product3d (0x248450's static
// helper, row i's second product written b-first) 6. Left at 6: the rotation's three up-row products load the up element
// first, retail the vector's k. Diagnostics on the 6 form (removed: volatile is not allowed): a volatile load of the location
// index, of the bias short or of camera_relative at the camera test each give a MATCH (fast; a 600 s permuter run found the
// first, full image MATCH); a volatile next-index read, a volatile request store or a volatile frame-counter read stay 6. So one
// load in the block before the camera test that VC cannot forward gives retail's operand order. No plain form found: the index
// through request.system 6, index before request 6, for and if + do/while loops 6, dword/int/unsigned index 6, the index
// declared at the top or renamed 6, the 0x2477b1 rotation 12-17, other camera orders 10-20, the rotation written out 237, the
// cross product written out 298, the bitfield tests without the macro 200-332 (the alias test folds), camera_relative read
// through its address 195, the bias short named 195, the bias product constant-first 6.
// @retail 0x174420
void __stdcall function_174420(long system_index, bool view_pass, bool camera_relative, bool force_visible)
{
    s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, system_index);
    if (function_137bd0(system->function_1751d0()->tag_index))
    {
        s_effect_particle_system_definition *tag_definition = system->function_1751d0();
        long const *reference;
        dword group = TAG_GROUP(tag_definition->tag_index);
        if (group == 0x5052544d || group == 'prt3')
            reference = function_137bd0(tag_definition->tag_index)->function_x947334();
        else reference = *(long const **)(TAG_GET(byte, tag_definition->tag_index) + 0x24);
        byte *shader = TAG_GET(byte, *reference);
        byte *table = *(byte **)(shader + 0x5c);
        dword render_flags = *(dword *)(*(byte **)(table + 4) + 2);
        s_effect_particle_system_definition *definition = system->function_1751d0();
        if (!TEST_FIELD_BIT(definition->flag5))
        {
            real bias = *(short *)definition->unknown14 * 0.05f;
            matrix3x3 camera;
            if (camera_relative)
            {
                camera.forward = g_4b9dac;
                camera.up = g_4b9db8;
                cross3f(&g_4b9db8, &g_4b9dac, &camera.left);
            }
            s_particle_render_request_173c90 request;
            request.system = system;
            for (long index = system->location_index; index != NONE; )
            {
                request.location = DATUM(g_51ec8c, s_particle_location_datum, index);
                bool in_view = ((byte *)request.location)[2] != 0;
                if (view_pass == in_view)
                {
                    bool draw = true;
                    point3f position;
                    if (camera_relative)
                    {
                        vector3f_rotate_into_axes(&camera, (vector3f const *)&request.location->position, (vector3f *)&position);
                        position.x = g_4b9da0.x + position.x;
                        position.y = g_4b9da0.y + position.y;
                        position.z = g_4b9da0.z + position.z;
                        *(dword *)((byte *)request.location + 0x30) = g_4ba034;
                    }
                    else
                    {
                        long cluster = system->location.cluster_index;
                        bool visible = function_3eb20(cluster) || (bool)(((dword)*(word const *)((byte const *)system + 0xc) >> 10) & 1);
                        bool in_pool = (g_547f88.flags[cluster >> 5] & (1 << (cluster & 31))) != 0;
                        if (force_visible || TEST_FIELD_BIT(definition->flag9) || in_pool || visible)
                        {
                            if (!visible && !force_visible)
                            {
                                if (!TEST_FIELD_BIT(definition->flag9))
                                {
                                    bool contained = false;
                                    draw = function_165010((s_frustum_set_view const *)g_547f88.context,
                                        ((signed char *)g_547f88.indices)[cluster],
                                        (point3f const *)((byte *)request.location + 0x20), *(real *)((byte *)request.location + 0x2c), &contained);
                                }
                                else
                                    draw = function_181800((s_sphere_frustum const *)((byte *)g_547f88.context + 0x94),
                                        (point3f const *)((byte *)request.location + 0x20), *(real *)((byte *)request.location + 0x2c)) != 0;
                            }
                            if (draw)
                            {
                                if (visible)
                                    system->flag10 = true;
                                else
                                    system->flag10 = false;
                                vector3f delta = {request.location->position.x - g_4b9da0.x,
                                    request.location->position.y - g_4b9da0.y, request.location->position.z - g_4b9da0.z};
                                if (!visible)
                                {
                                    real distance = (real)(sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i) * g_4b9dcc);
                                    draw = function_249bb0((s_distance_fade const *)definition, distance) > 0.0f;
                                }
                                else draw = true;
                                *(dword *)((byte *)request.location + 0x30) = g_4ba034;
                            }
                        }
                        position = request.location->position;
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
                else *(dword *)((byte *)request.location + 0x30) = g_4ba034;
                index = request.location->next_index;
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

// MiniMax R (11-14): no lower score (47): the availability's zero store still comes before the
// second definition call and the availability is still made from the query result arithmetically;
// moving the clear to just before the view read measures 125. Left: the availability's store order
// and its arithmetic form against retail's branch, the view's reload after the attachment call, and
// the two flag byte homes.
// score 47 (was 47): availability clear/production and later view observation remain;
// what still differs is the same-view flag: ours writes its true value to the stack byte
// before the availability query, retail keeps the flag in BL and writes the true value only after the owner
// tests, with the availability's own production after its query; frame 0x14, the owner, the view and the
// post-attachment view refresh agree.
// MiniMax b (11-11): 12 (was 47), NOT kept (coordinator 11-11: a bool set true then cleared by a test and a bool set by an if are match-only; the direct expressions with the global re-read score 113; body at 47): the view global is read at each use, the availability is cleared with
// `xor bl, bl` before the view test and set with a branch on success, and same_view is declared true before the
// availability and cleared by one `owner == NONE || g != owner || !available` test; 498 bytes, as retail.
// Left: our `same_view = 1` store is emitted ahead of the view test, retail's sits between `test bl, bl` and
// `jne` in the availability test's block, and the availability's own test block is reached a store earlier.
// frame 0x14, NONE/owner and Boolean stack homes agree;
// availability clear/result and the post-attachment view refresh remain.
// M3 lane 13 r75 (fast; masked aligned count in brackets, base [5]): round 81's `effect_index != NONE && available && ...` re-measured 46
// [7]; with the availability false after the view read and set by a branch 131 [56] (504 bytes); with the view global re-read for the
// camera test 109 [7] (500 bytes); the effect index read after the second definition call 54 [18]; that with the reordered attachment 53 [19].
// MiniMax I (11-08): no new form, no lower score (47).
// MiniMax C (11-06): no new form, no lower score; the availability flag is still computed with neg/sbb/inc where retail clears BL before the view test and sets it with a branch, and retail still re-reads the view global after the 0x17afd0 call.
// MiniMax S (11-09): the availability set by a branch that turns it true scores 135, the same branch with its
// initialisation moved after the view load 131, and the same-value ternary 47 (the same code); the base's 47 stands.
// Earlier (47, was 47): frame 0x14, NONE/owner and Boolean stack homes agree;
// availability clear/result and the post-attachment view refresh remain.
// availability clear/production and the post-attachment view observation remain.
// frame 0x14, eligibility width, NONE/owner and Boolean homes agree;
// availability production and the post-attachment view observation remain.
// Earlier: the availability flag written as `query(view) == 0` 102 (the
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
// Round 295: Native system/definition/cluster entry storage in all seven subsets [102, 102, 102, 102, 102, 102, 102].
// All three plus effect/view entry storage 102. Original producers and body restored.
// Round 297: Native unsigned owner/iterator/next-slot domains in all seven subsets, then all with owner declared before effect acquisition, 102 each. Original signed domains, declaration scopes, queries and complete body restored; NONE/owner, availability and Boolean homes remain.
// Round 453: Nullable effect identity used for owner and attachment, conditional pointer/branch
// pointer/joint arms/default results, original then shifted-word flag:
// 144/144/142/73/152/152/133/113.
// Kept the default-initialized effect record for owner extraction and attachment admission;
// plain-confirmed 73 at 495/498 bytes, with no matched function lost.
// Round 299: Complete owner-valid/equal/available conjunction orders ACB/BAC/BCA/CAB/CBA 102/102/102/147/147;
// explicit owner-result arms with ACB/BCA/CBA 149/149/104. Original full predicate order, owner result and
// body restored; NONE/owner coalescing, BL availability and Boolean homes remain.
// Round 337: raw view classification consumed by both zero-status tests, long/dword/short/byte,
// default/query then conditional producer: 137/137/135/155/136/136/134/149. Wider raw carriers
// recover the first EBP save but worsen later homes; original availability and body restored.
// Round 455: On the retained two-use nullable effect graph, false-default/direct logical/
// complete valid-invalid arms/raw two-use status, original then shifted-word flag:
// 144/150/150/145/132/140/140/72. One-point best rejected; original body restored.
// Round 457: Same-view bool/byte/short/long after attachment on the recovered two-use effect
// graph, original then shifted-word flag; original snapshot and call gates retained. Scores
// 154/151/161/158/164/162/171/168. Original executable body restored.
// Round 459: Complementary unavailable bool/byte/short/long on two-use effect graph, original/shifted-word flag
// 155/155/154/152/144/144/144/142. Original executable source restored.
// Round 313: Complete camera result after the unchanged attachment query, bool/byte/short/long,
// original/shifted-word eligibility: 143/133/157/147/148/138/147/137. Original camera producer and
// complete body restored; eligibility width and availability/view carriers remain.
// Round 315: Complete optional-effect admission bool/byte/short/long shared by owner and
// attachment, original/shifted-word eligibility 144/144/147/147/136/136/136/136. Original two-use effect
// receiver and complete body restored; flag width, availability and view carriers remain.
// M299: Attachment effect-first/nullable-conditional/complete/nested results,
// original and shifted-word flags [72, 132, 134, 134, 113, 116, 87, 86].
// One-point form rejected; original attachment result and eligibility restored.
// Round 137 (aligned count in brackets, baseline 73 [58]; retail: availability BL from before the query, view ESI,
// effect index EDI, owner EBP, same view on the stack): plain owner x availability false-then-set, nested, one
// expression, two-step x view local, at each use, reread x bit 9 mask, shift: 102..148 [51..74], best aligned one
// expression with the shift 123 [51]; false-then-set with every local at function scope, eight orders, 138 [61];
// effect pointer kept x availability false-then-set, expression, two-step x mask, shift x view reread: 73..160
// [55..88]. Same view keeps BL over availability in every false-then-set form. Body unchanged.
// M330: Positive valid-view outcome cleared on query failure, bool/byte/short/long,
// original/shifted-word eligibility: 134/134/142/139/126/126/132/129. Frame grows
// to 0x18; original availability gate, snapshot and complete body restored.
// Round 543: Early visibility result/complete eligibility/nested admission/negative rejection,
// original then shifted-word flag: 73/238/73/73/113/244/113/113. Original two-use effect graph
// and complete executable body restored; flag width and availability/view carriers remain.
// Round 549: Same-view tests owner/available/equal, equal/owner/available,
// equal/available/owner, available/owner/equal, available/equal/owner on the
// two-use effect graph: 72/73/72/132/132; shifted-word flag with the first/fourth/fifth
// orders 114/106/106. Last two recover retail length and entry through +0xb2.
// NOT kept: one-point best or regression; original body restored.
// F346: On the retained two-use effect graph, last-consumer scopes for owner, availability,
// both and complete effect/classification, each original/shifted-word eligibility:
// 73/113/73/113/73/113/73/113. Original complete executable body restored.
// M3 lane 4 r159: 47 (was 73): the shifted flag, owner as if/else without the effect pointer, availability false at the arm's top, `available = !query`, attached
// testing the effect index. Left: availability's false store belongs after the view read, retail's query test is test/jne/mov bl, 1, and retail reloads the view
// after the attached query. Tried, aligned count in brackets: availability set only on success 135 [58]; availability false right before the query 133 (488 bytes)
// [45]; false at the loop-body top 130 (498 bytes) [14]; owner default at the arm's top 47 (495 bytes) [6], owner after the second lookup 133 (496 bytes) [17], owner at
// the loop-body top 154 (504 bytes) [42]; view at each use 110 (500 bytes) [5]; same_view false at the arm's top, set on the test, availability set on success 111 (492
// bytes) [3], the closest shape: same_view's early false store keeps it on the stack and BL for availability, but the store shape differs; if/else same_view 131 (504
// bytes) [57], with an early false 131 [57], with an inline out parameter 131 [57]; byte same_view 121 [51]; byte/char availability 131/131 [57/57]; two-step same_view
// 117 [55]; named query result 47 [5].
// Round 122 (lane 7, fast; base 73): availability false-then-set 144 (507 bytes); attachment gated on
// effect_index != NONE instead of the effect record 102 (486 bytes: owner/NONE coalescing lost); view global
// at each use 143 (502); bit 9 as (bool)(((dword)word >> 9) & 1) 113 (500); owner declared before the effect
// record 73 (same code). Retail: effect index EDI, view ESI, owner EBP, availability BL; ours effect index EBX,
// record ESI, view EDI.
// M3 lane 14 r59 (fast, base 47): retail's 498 bytes are ours plus the view reload after 0x17afd0 (6) less the
// branchy availability set (2). The view global read at each use with availability false-then-set 142 (510 bytes;
// availability spilled to a stack byte, same_view in BL, NONE in EBX, owner in ESI); that with the owner defaulted
// to NONE at its declaration 139 (508); that with the availability as one `g != NONE && !query(g)` 123 (496; view
// in EBP, NONE in EBX, availability in AL); the view local with availability false right after the view read and
// set on success 131 (504; availability spilled, NONE in EBX, owner in EDI); the view local with one-expression
// availability 125 (492). Every branchy availability form moves NONE out of EBP and the flag out of BL.
// AC515: Actual two-use view classification on the shifted-word/effect-index/owner-arm graph,
// long/dword/short/byte with default-query then conditional producers: 139/139/141/129/139/139/141/125.
// NOT kept: unmatched classification storage; original Boolean result and complete body restored.
// AC522: Coupled availability/same-view outputs, complete and success-owned
// producers with bool/byte/short/long same-view domains 142/135/132/135/125/133/138/141.
// NOT kept: unmatched result residence; original observations and body restored.
// F366: Phase-specific post-attachment view observation with original/complete/refined/logical/
// success-only/complete-query/invalidation/complementary availability producers: 111/119/118/119/129/119/79/112.
// Exact-only observation diagnostics restored; original snapshots, query gates and full body retained.
// B419: Complete draw-arm/rejection/final-admission results on the recovered owner/Boolean graph,
// conditional/direct camera result: 107/96/110/136/106/96/110/136. All regress; original
// availability, input snapshots, query gates, draw tuple and complete body restored.
// B421: Owner/view identity before the status query, bool/byte/short/long,
// positive/complementary results 133/133/159/158/133/133/159/158. NOT kept:
// all regress and exceed retail length; original snapshot/query graph restored.
// DL261: Actual optional available-view index, valid-invalidating/default-success/conditional/complete arms, long then dword: 122/121/82/82/122/121/82/82.
// NOT kept: no exact native form; original complete body and all temporary contracts/types restored.
// DH350: Independent visibility/drawing getter lifetimes, availability default
// after getter/owner/view, joined/nested success, view-default and conditional:
// 47/138/125/125/131/131/124/125. NOT kept: no match or gain; original snapshots and body restored.
// DH371: Partial owner-equality/attachment/camera-admission/camera-test/rejection/view-guard/
// owner-validity/location-admission Boolean results: 47 each on the recovered owner-arm graph.
// NOT kept: no match or gain; original snapshots, query gates and complete body restored.
// Native availability clearing/branch production and later current-view observation remain.
// AC541: Both downstream same-view/attachment outcomes with availability-owned or complete
// rejection-owned results, bool/byte/short/long: 120/115/126/133/120/99/127/133.
// NOT kept: all regress; original availability, snapshots, query gates and complete body restored.
// AC544: Availability/owner/view lexical identities on the recovered owner-arm graph,
// seven subsets then complete predicate/result identities: 47/47/47/47/47/47/47/47.
// NOT kept: allocation-only ties; original names, snapshots, queries and whole body restored.
// Round 588: Separate eligibility/drawing getters, complete eligibility producer scope
// and immutable drawing data, seven graphs then early drawing declaration:
// 47/219/47/219/47/219/219/219. NOT kept: no equality; original snapshots and body
// restored. Availability clear/result and post-attachment view observation remain.
// Y593: Independent raw-slot cursor and iteration-local salted handle, long/dword/int/short
// with while/for traversal: 166/166/166/117/166/166/166/117. NOT kept: no equality; original complete
// traversal and body restored; availability production and later view observation remain.
// F380: Complete visibility/rejection results on the shifted-word/owner-arm graph,
// bool/byte/short/long: eight 47. NOT kept: ties; original admission,
// availability, view snapshot, query gates and complete body restored.
// P337: Whole-iteration same-view/attachment/camera output declarations, masks
// 1/2/4/3/5/6/7 then all with draw, each 47. NOT kept: no match; original
// producer sites, snapshots and complete body restored. Early clear/conversion remain.
// M381: View-validity-produced availability then unavailability, bool/byte/short/long
// on the recovered owner-arm graph: 124/124/146/143/121/121/152/149. NOT kept: all regress;
// original query gates, view snapshot, Boolean homes and complete body restored.
// DH371: Partial owner-equality/attachment/camera-admission/camera-test/rejection/view-guard/
// owner-validity/location-admission Boolean results: 47 each on the recovered owner-arm graph.
// NOT kept: no match or gain; original snapshots, query gates and complete body restored.
// Native availability clearing/branch production and later current-view observation remain.
// M3 lane 8 r135 (fast; the view read from the global at each use, availability false late and set on success, aligned
// count in brackets; plain form 138 [70]): one real use of effect_index right before the availability or right before
// attached (diagnostic, removed) gives retail's whole allocation, 104 [1], the store itself the only difference; it counts
// only there (after attached, in an arm or before the view read 134-142). Empty tests, `(void)`, `__assume`, an unused
// bool, inline helpers or copies for availability/attached, `x * 1` no-ops, separate definition lookups, a named absolute
// index, declaration orders: 138 [70], removed before allocation. Same view true at the arm's top and cleared on the
// failed test 25 [2], placed just before the availability 12 [2] (only the true store's place differs), after the
// availability 141; false at the arm's top with a nested set 28 [4]. Attached written in both same-view arms (true: effect
// test and call; false: available test, effect test and call) 17 [8] at 498 bytes: retail's allocation with no early
// store, only the arm layout left (retail's first arm jumps into the second's copy), the condition inverted 17; with direct
// availability 120, with a view local 27 [5] (494 bytes); `attached = same_view ? short : full` or an if/else on same_view
// 91 [2] (500 bytes: `je; mov; jmp` for retail's `mov; jne`). All need the flag form, so match-only: body kept at 47.
// M3 lane 14 r68 (fast; the 47 body differs from retail in 7 aligned instructions: the availability's `xor bl, bl`
// before the 0x1751d0 call, `neg/sbb/inc` for retail's `test; jne; mov bl, 1`, and the view reload after 0x17afd0):
// `if (view != NONE && !query(view)) available = true;` 135 (availability spilled, NONE in EBX); `available = view
// != NONE && !query(view)` reassigned 125 (492); `query(view) ? false : true` 47 (same code); retail's shape with
// the view global at each use and same_view declared at function scope 138 (510); same_view defaulted false at
// the arm's top and reassigned whole 138. The branchy availability always hands BL to same_view.
// MiniMax B (11-06): no lower score (the availability produced by a branch 135, by a negated long 131,
// moved after the view load 125, the view reloaded after the attachment query 110, the same-view result at
// function scope 47); the availability is still cleared before the second definition call and the query
// result still goes through a negated long where retail tests the result and branches.
// MiniMax E (11-11): the view read once before the availability, the availability set on success and the same-view flag cleared by one owner/view/availability test reach retail's twelve (47 -> 12); the flag's true store is still early and the availability keeps its own home, so no untried form is left for them.
// MiniMax E (11-06): no new form, no lower score.
// The availability result moved after the view load 125, after the definition
// call 138 and before the effect index 138: each move flips the NONE constant
// from EBP to EBX. The branchy availability (test/jne/mov bl,1) 135 spills the
// result to a stack byte. The base's 47 stands.
// M3 lane 11 r76 (fast; aligned count in brackets): the view global at each use with the availability as one
// if/else on `g != NONE && !query(g)` 113 (498 bytes); the view local for the availability, same view and
// attachment with the global read only at the camera test 110 (500 bytes) [6: only the availability's xor placement
// and neg/sbb/inc left; a mix, match-only]; the view at each use with availability false at the arm's top and set on
// success 142 (510); that reload mix with availability false right after the view read and set on success 138 (510)
// [70]; the reload mix with one-expression availability 113 (498) [51]. Every branchy availability keeps NONE in
// EBX past the view and owner tests, where retail coalesces the owner with NONE in EBP.
// P394: Post-attachment external-view suppression at its actual else-arm producer,
// positive/complementary byte/short/long/dword results: 47/47/47/47/47/47/47/47.
// NOT kept: one-use suppression results require equality; original admission and body restored.
// Availability clearing/production and post-attachment view observation remain.
// T80: Partial cluster-sentinel present/absent admission in bool/byte/short/long:
// 47/47/47/47/47/47/47/47. Availability clear/production and later view observation remain.
// NOT kept: no byte equality or qualifying gain; original complete body restored.
// Round 81 (m3 lane 10, fast with this file and its callees): `bool available = false;` moved to just before
// the view read 125 (490 bytes); the view read before the owner lookup 47 (same code); the attachment test as
// `effect_index != NONE && available && ...` 46 (the attached block threads differently from retail; a one-point
// change, not kept); `view_pass == same_view` 48; the availability left uninitialised and set in both arms of the
// view test (`else available = false;`) 125 (492 bytes).
// Lane 9 r99 (fast; the view global read at each use and the availability false after the owner and set on success
// unless noted): the 12 form (same_view true before the availability, cleared on the failed test) differs only in
// where VC puts the `same_view = 1` store (the view test's block, retail the third test's block), also with the true
// declared between the availability's clear and its test 12, or the clearing test written with `||` 12; the value
// same_view 138 (510 bytes: same_view takes BL, NONE EBX); effect index tested first in attached 138; `else if
// (available && attached && ...)` 149; availability declared at the arm's top 138; availability in one if/else 113
// (498 bytes); owner defaulted to NONE 138; attached false-then-set 146; same_view false-then-set 141, both 149;
// same_view if/else 138, negated first 138, `? true : false` 138; the current body with the view at each use 110 (500
// bytes; only the xor placement and neg/sbb/inc left), with the clear after the owner 121 (496 bytes), with a nested
// set 138, with the early clear and the set on success 142.
// M3 lane 8 r150 (fast; aligned count in brackets; "g base" = the view global at each use with the availability
// false after the owner and set on success, 138 [59], NONE in EBX, owner EDI, same view BL): on the g base, dropping
// `owner != NONE` from the same-view test (diagnostic) gives retail's homes, 96 [2], the missing compare the only
// difference; removing the effect-index, view or attachment-availability tests instead does not (131/129/121), nor does
// an owner store (127) or a constant store (149); an effect-index store before the availability 104 [1] (r135's
// diagnostic). So retail ranks the effect index above the owner. Tried: an inline owner getter with a NONE default
// (result local, early return, ternary, forceinline) 138/138/138/138 on the g base, 113 on this body; the effect record's
// address taken ahead of the owner test 47 (same code), 138 on the g base; owner nested with an effect pointer and a NONE
// default 138; a named effect-presence test used for the owner and the attachment 126, 157 on the g base (re-measures of round 315's
// shared admission); the equality
// written `owner == view` 47, 138; the camera table as `g_4ed284->entries[view].index` 47 (same code); the effect index read
// from the record at every use (re-measure of lane 13 r27's form) 54 here, 132 on the g base, 117 with one-expression
// availability.
// Round 84 (m3 lane 1, fast with 0x175610, 0x1751d0 and 0x174420 listed, 47 [5] as the full check; aligned count in brackets). Base P:
// r135's plain form (the view global read at each use, `bool available = false; if (g != NONE && !query(g)) available = true;`, same_view
// and attached as plain `&&` expressions) re-measured 138 (510 bytes) [57]. Retail's `test bl, bl; mov same, 1; jne` is that plain
// same_view with the true path threaded past attached's availability test. Diagnostics on P (removed): r135's effect-index store between
// the owner and the availability re-measured 104 (501) [1]; new: a store of NONE there 120 [29, masked 4], of the owner 124 [29, masked
// 3], of the view 133 [34, masked 4] (any store there gives retail's flag homes; the effect index gives the EDI/EBP assignment too);
// the effect-index store between the availability and same_view 142 [61], after same_view 97 [11]; a compare-and-return on the effect
// index there 146 [50]. Re-measured: same_view true first and cleared by one `||` test 12 (498) [1]; attached in both same_view arms
// 17 (498) [4]; same_view in an if/else 138; the effect index tested first in attached 138 [56]; inline availability and attachment
// helpers 138/138. New natural forms on P: same_view false with a nested set and clear 129 (506) [54]; false then `= available` under
// the owner tests 131 (507) [55]; `effect_index != NONE &&` leading same_view 136 (509) [50]; the loop body as a force-inline helper
// taking the index and the pass flag, nested or with early returns, or taking the system pointer too, or only the inner block 138 each
// (same code; plain inline leaves a call); an inline owner helper, with or without the availability helper, 138/138 (same code); the
// effect index read after the definition call 132 (504) [60], read from the field at each use 132, the owner from the field with the
// local after it 132; the owner looked up before the definition call 128 (502) [31, masked 5: retail's flag homes, the call then
// follows the owner load]; the definition call after the availability 126 (499) [70], after same_view 87 (498) [59]; the camera flag
// declared before the availability 138 [58], at the block's top 102 (494) [48], between availability and same_view or after same_view
// 138; attached or same_view declared false before the availability and assigned later 138/138, all three flags so 138. Body unchanged.
// m3 lane 14 r82 (fast): register/esp-masked aligned diff 7 rows: the availability's `xor bl, bl` ahead of the second definition
// call (retail after the view read), `neg; sbb; inc` for retail's `test; jne; mov bl, 1`, and retail's view reload after 0x17afd0.
// No untried natural form for them; the listed branchy and view-at-each-use forms all move NONE out of EBP.
// Also r82: the listed 12 form (the view global at each use, availability false after the owner and set on success, `bool
// same_view = true;` just before the availability, cleared by one `owner == NONE || g != owner || !available` test) re-measured
// 12 at 498 bytes, aligned 2: ours stores the true in the view-test block, retail between `test bl, bl` and `jne` in the third
// test's block (the plain `&&` expression's pattern). Permuter on it (600 s, 56 tries): best 12.
// Also r82 on this body (fast): the location-mode test before `attached` 52; same_view as `view == owner && owner != NONE &&
// available` 47 (other code), `available && owner != NONE && view == owner` 107 (492 bytes), `owner != NONE && available && view ==
// owner` 46 (masked 14, test order only, not kept); on that, the location-mode swap 51, the attached test effect-index first 46;
// `view == owner && available && owner != NONE` 46, `available && view == owner && owner != NONE` 106 (492), the equality first
// with the location-mode swap 52.
// m3 lane 13 r79 (fast, base 47): the camera test in the idiom of src/unknown_03f2b0.cpp (`g_4b9ed8 != NONE &&
// g_4ed284->entries[g_4b9ed8].index != NONE`) with the availability false early and set by a branch 142 (510 bytes);
// the branchy availability with the attachment testing the effect index first 135 (504); the camera idiom with the
// availability uninitialised and set in an if/else 113 (498: availability in AL, same view in BL); the second definition
// lookup in its own local 47 (same code); same_view false then set by the full test 107 (498, owner in ESI, view in EBP).
// Every branchy availability spills the flag or moves NONE out of EBP; ours keeps the flag in BL only with the early
// `available = false` and the neg/sbb/inc set.
// m3 lane 15 r74: a compile-only screen (/O2 /Gr, no /GL; it reproduces this body's allocation, window rows against
// retail with the two call-argument rows the screen cannot model: this body 10, retail-like 3) of 120 forms: view as a
// local, the global at each use, or a local with the global at the camera test; availability as the guarded value, a
// branch, a nested branch or `== 0`; its clear early or late; owner as if/else, NONE default or ternary; owner before or
// after the second definition call; same-view equality either way round. Best is this body (10); the branchy forms 20-34
// (NONE in EBX or ESI, the availability or the owner spilled). Fast re-measures: the plain form P (view global at each
// use, branchy availability) 142 with the clear early, 138 with it late (masked 19/18). Retail's code after the view
// load is P's exactly (same-view materialised in the availability test's block, its true path threaded past attached's
// availability test); only the allocation differs (retail NONE coalesced with the owner in EBP, availability BL).
// Fast: the systems pointer reloaded before the next index 54; on P (clear late) same_view as `available && owner !=
// NONE && g_4b9ed8 == owner` 118 (499 B, masked 8: availability in BL as retail), with the owner defaulted to NONE first
// 59 (498 B, masked 8: effect index EDI and availability BL as retail; left: NONE in EBX and the owner in ESI where retail
// coalesces them in EBP, and the availability tested first where retail tests the owner first); that order on this body
// 107 (492 B); the availability declared at the loop-body top 134 (500 B). Same code: the outer test as nested ifs, the
// cluster read at each use.
// M3 lane 12 r115 (fast; matched siblings 0x3e9c0 and flexible_surface.cpp read the view global at each use in this same
// test): the view global at each use with a nested branchy availability, a NONE-default owner, the second definition call
// after the owner and same_view true then cleared 124 (500 B); the view local with the global in the attachment call and
// the camera test, nested branchy availability, NONE default, call after the owner 124 (500 B); that with the plain
// branchy availability, a ternary owner and the call after the availability 128 (500 B); the view local with the branchy
// availability after the view read and a NONE-default owner 132 (502 B); the view global at each use with this body's
// availability, the call after the owner and the true-then-clear same_view 134 (506 B); the view local with the global at
// the camera test, nested branchy availability, NONE default, true-then-clear 141 (511 B); that with the availability as
// one expression, the call after the availability and plain same_view 136 (504 B); the view local, one-expression
// availability, ternary owner, true-then-clear 122 (495 B). Re-measured: the branchy availability after the view read 131
// (504 B: NONE moves to EBX, the effect index to EBP, the owner to EDI, the flag to a stack byte, same_view to BL).
// Diagnostic (not kept): on that, same_view and the camera flag written through pointers 158 (513 B, the flag still spills).
// M3 lane 4 r190 (fast with 0x1751d0; "P" = the view global at each use, the owner as if/else, `bool available = false;
// if (g != NONE && !query(g)) available = true;` after the owner, plain same_view and attached, 138 at 510 bytes): the
// availability renamed to sort first or last 138/138 (same code: local names do not move registers here); P with the
// owner defaulted to NONE and same_view's operands owner/view/available 138, available/owner/view 59, view/owner/available
// 138, owner/available/view 138, available/view/owner 58 (498 bytes; availability BL as retail), view/available/owner 138;
// on the 58 form the owner default declared before the effect index 60, the effect index read after the definition call
// 105 (495 bytes), the owner as if/else 118 (499 bytes); the view read into a local with the global only at the camera test
// 58, the availability set by a nested test 58 (same code). Cause found: NONE takes EBX first and the availability then
// reuses it, so the owner cannot share NONE's EBP as in retail; only a body whose availability is live across NONE's
// range (this one's early clear) gives retail's homes. Body unchanged.
// MiniMax A (11-12): no new form, no lower score (47); our same-view store is still emitted ahead of the view test where retail's sits inside the availability test's block, and the 12 form needs the match-only bool (set true and then cleared by a test).
// Round 113 (m3 lane 16, fast; "P" = the view global at each use, the owner as if/else, `bool available = false; if (g !=
// NONE && !query(g)) available = true;` after the owner, plain same_view and attached; re-measured 138 at 510 bytes: NONE in
// EBX, the owner in EDI, the availability spilled, same_view in BL): P with the second definition in its own local 138; the
// owner NONE-defaulted and read through an effect pointer 138 (508 bytes); the walk as a for loop with the step in its
// increment 138; the availability set by a nested test 138; `bool camera_relative = false;` at the block's top 102 (494
// bytes: NONE now shares EBX with the owner and the later NONE tests are immediates, as retail's EBP pair, but the second
// definition takes EBP, the availability still spills and the camera flag's store moves ahead of the second definition call);
// P with `view_pass == same_view` 138.
// T102: Complete different-view/detached/skip-draw result graphs, seven subsets,
// then all with direct camera-result production: 116/50/96/118/119/98/121/121.
// NOT kept: no exact availability/Boolean-home repair; original observations,
// predicates, query gates, draw tuple and complete traversal restored.
// M3 lane 7 r158 (fast, base 47): P (the view global at each use, owner if/else, availability false after the owner and set
// on success, plain same_view and attached) 138; P with the owner NONE-defaulted through a block-scoped effect pointer 138
// (508 B); P with the owner if/else inverted 142; this body with it inverted 47 (same code); `long const` effect index and view
// 47 (same code); P with the query tested `== 0` 138. In every P form NONE's register stays live through the view and owner
// tests (`cmp esi, ebx`, `cmp edi, ebx`), where retail's ends at the owner default.
// P468: Existing camera/draw result byte/short/long/dword storage domains:
// 113/140/141/141/47/111/110/110. NOT kept: no equality for the native byte homes;
// original availability, view snapshot, query gates and complete body restored.
// P482: Six eager pure-scalar owner/view/availability conjunction orders, then two
// mixed complete joins: 122/122/122/122/122/122/139/162. NOT kept: all regress;
// full eager joins grow the frame to 0x18, final mix grows to 515 bytes. Original
// Boolean graph, read/query phases and entire body restored; availability/view gaps remain.
// P484: Shared three-relation view outcome (unavailable/available-other/available-owner),
// byte/short/long/dword, original/producer-local zero lifetimes: 60/133/131/131/70/87/85/85.
// NOT kept: no equality/gain; original availability, owner/view observations, query
// gates, callback arguments, traversal and entire body restored.
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
            (TEST_FIELD_BIT(definition->flag9) ||
             bit_vector_test(g_547f88.flags, cluster) || function_3eb20(cluster)) &&
            system->unknown4c == NONE && system->location_index != NONE)
        {
            long effect_index = system->effect_index;
            bool available = false;
            definition = system->function_1751d0();
            long owner;
            if (effect_index != NONE)
                owner = DATUM(g_4ea93c, s_effect_datum, effect_index)->unknown58;
            else
                owner = NONE;
            long view = g_4b9ed8;
            if (view != NONE)
                available = !function_155760(view);
            bool same_view = owner != NONE && view == owner && available;
            bool attached = available && effect_index != NONE && function_17afd0(effect_index, view);
            bool draw = same_view == view_pass;
            bool camera_relative = false;
            if (same_view)
            {
                if (view != NONE && *(short *)((byte *)g_4ed284 + view * 0x94 + 0x42) != NONE)
                    draw = definition->flag6;
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
