// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "effects.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "bit_vector.h"
#include "vertex_inputs.h"
#include <math.h>
#include <xtl.h>

struct s_surface_vertex { point3f position, previous; vector3f normal; };
struct s_surface_attachment { short vertex, marker, unknown4; };
struct s_surface_record
{
	short salt, unknown2;
	long tag_index, object_index;
	real unknown_c;
	point3f origin;
	real speed;
	s_surface_vertex vertices[128];
	dword pinned[4];
	s_surface_attachment attachments[6];
	short attachment_count, unknown1256;
};
struct s_surface_rest_vertex { point3f position; real u, v; };
struct s_surface_link { long unknown0; long a : 16, b : 16; real length; long unknownc; };
struct s_surface_definition
{
	word flags, unknown2;
	long marker_name;
	byte unknown8[8];
	short columns, rows;
	byte unknown14[10];
	short iterations;
	real gravity, damping, wind_scale, tangent_drag;
	byte unknown30[0x1c];
	long vertex_count;
	s_surface_rest_vertex *vertices;
	long triangle_index_count;
	short *triangle_indices;
	long strip_index_count;
	short *strip_indices;
	long link_count;
	s_surface_link *links;
};
struct s_surface_object_flags { byte unknown0[0x10a]; word bit0 : 1, bit1 : 1, bit2 : 1; };
struct s_surface_object_header { short salt; byte flags, inactive; long unknown4; byte *object; };

static inline s_surface_record *surface_record(long index)
{
	return (s_surface_record *)(g_4e0338->data + (index & 0xffff) * 0x1258);
}
static inline s_surface_definition *surface_definition(s_surface_record *record)
{
	return (s_surface_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
}
static inline s_surface_object_header *surface_object_header(long index)
{
	return (s_surface_object_header *)(g_4e0300->data + (index & 0xffff) * 12);
}

static inline real surface_normalize_horizontal(vector3f *v)
{
	real length = (real)sqrt(v->i*v->i + v->j*v->j);
	if (!(fabs(length) < 0.0001f))
	{
		real inverse = 1.0f / length;
		v->i *= inverse; v->j *= inverse;
	}
	else
		length = 0.0f;
	return length;
}

void function_ba1d0(long, vector3f *, vector3f *);
real function_30bf0(vector3f *);
long function_155760(long);
long function_14de70(long local_player_index);
void object_widget_delete(long, long);
extern dword g_4ba034;
long g_55e710;
bool g_55e714;
real g_55ecd0;
vector3f g_55ecd4;

bool function_1171a0(long);
bool function_1173e0(long, long);
void function_117510(long);
void __stdcall function_117790(long);
void function_117a80(long);
void function_117dd0(long, vector3f *);
void function_118140(long);
void __stdcall function_118430(long);
void function_117080(long);

// @retail 0x1168a0
void __stdcall function_1168a0(real elapsed)
{
	long index = data_datum_index(g_4e0338, function_16bc00(g_4e0338, 0));
	while (index != NONE)
	{
		long slot = index & 0xffff;
		s_surface_record *const &record = surface_record(index);
		record->unknown_c = 0.0f;
		function_117790(index);
		function_118140(index);
		function_117510(index);
		function_117080(index);
		long next = index == NONE ? 0 : slot + 1;
		index = data_datum_index(g_4e0338, data_find_index(g_4e0338, next));
	}
}

// @retail 0x116980
long __stdcall function_116980(long tag_index, long object_index)
{
	long result = NONE;
	if (tag_index != NONE && object_index != NONE)
	{
		result = record_pool_allocate(g_4e0338);
		if (result != NONE)
		{
			s_surface_record *record = surface_record(result);
			record->tag_index = tag_index;
			record->object_index = object_index;
			record->unknown_c = 0.0f;
			if (!function_1173e0(object_index, result))
			{
				record_pool_release(g_4e0338, result);
				result = NONE;
			}
		}
	}
	return result;
}

// @retail 0x117080
void function_117080(long index)
{
	long object_index = surface_record(index)->object_index;
	if (object_index != NONE)
	{
		s_surface_object_header *header = surface_object_header(object_index);
		byte *object = header->object;
		if (!header->inactive && TEST_FIELD_BIT(((s_surface_object_flags *)object)->bit2) &&
			(real)(g_510c54->game_time - *(long *)(object + 0xbc)) * g_510c54->rate > 10.0f)
			object_widget_delete(object_index, index);
	}
}

// @retail 0x117100
bool function_117100(long index)
{
	bool result = true;
	long object_index = surface_record(index)->object_index;
	long root = NONE;
	while (object_index != NONE)
	{
		root = object_index;
		object_index = *(long *)(surface_object_header(object_index)->object + 0x14);
	}
	s_surface_object_header *header = surface_object_header(root);
	if (!header->inactive && g_4b9ed8 != NONE && function_155760(g_4b9ed8) == 0)
	{
		byte *object = header->object;
		if (function_14de70(g_4b9ed8) == *(long *)(object + 0x13c))
			result = false;
	}
	return result;
}

// @retail 0x1171a0
bool function_1171a0(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	s_object_marker markers[6];
	long order[6] = {0, 1, 2, 3};
	long marker_name = definition->marker_name;
	long object_index = record->object_index;
	record->attachment_count = function_b8d30(object_index, marker_name, markers, 6, false);
	long axis = 2;
	if (definition->flags & 2) axis = 0;
	for (long i = 0; i < record->attachment_count; ++i)
	{
		for (long j = i + 1; j < record->attachment_count; ++j)
		{
			if (markers[i].matrix.position.n[axis] > markers[j].matrix.position.n[axis])
			{
				s_object_marker temporary = markers[i]; markers[i] = markers[j]; markers[j] = temporary;
				long swap = order[i]; order[i] = order[j]; order[j] = swap;
			}
		}
	}
	if (record->attachment_count > 0)
	{
		real step = (real)definition->rows / (real)record->attachment_count;
		for (long i = 0; i < record->attachment_count; ++i)
		{
			s_surface_attachment *attachment = &record->attachments[i];
			long vertex;
			if (i == 0) vertex = 0;
			else if (i == record->attachment_count - 1) vertex = (definition->rows - 1) * definition->columns;
			else vertex = (long)(ceil(i * step) * definition->columns);
			attachment->marker = (short)order[i];
			attachment->vertex = (short)vertex;
		}
	}
	return record->attachment_count > 0;
}

// @retail 0x1173e0
bool function_1173e0(long object_index, long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	bool result = function_1171a0(index);
	if (result)
	{
		byte *object = surface_object_header(object_index)->object;
		point3f origin = *(point3f *)(object + 0x30);
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			point3f *source = &definition->vertices[i].position;
			s_surface_vertex *vertex = &record->vertices[i];
			vertex->position.x = source->x + origin.x;
			vertex->position.y = source->y + origin.y;
			vertex->position.z = source->z + origin.z;
			record->pinned[i >> 5] &= ~(1 << (i & 31));
		}
		for (long iteration = 0; iteration < 5; ++iteration) function_117790(index);
	}
	return result;
}

// @retail 0x117510
void function_117510(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	for (long i = 0; i < definition->triangle_index_count; i += 3)
	{
		short a_index = definition->triangle_indices[i];
		short b_index = definition->triangle_indices[i+1];
		short c_index = definition->triangle_indices[i+2];
		s_surface_vertex *a = &record->vertices[a_index];
		s_surface_vertex *b = &record->vertices[b_index];
		s_surface_vertex *c = &record->vertices[c_index];
		vector3f v, u, normal;
		vector3d_from_points3d(&a->position, &b->position, &v);
		vector3d_from_points3d(&a->position, &c->position, &u);
		cross3f(&v, &u, &normal);
		real length = normalize3d(&normal);   // unused in this build; retail's code needs the named result
		vector3f_add(&a->normal, &normal, &a->normal);
		vector3f_add(&b->normal, &normal, &b->normal);
		vector3f_add(&c->normal, &normal, &c->normal);
	}
	for (long i = 0; i < definition->vertex_count; ++i)
		if (normalize3d(&record->vertices[i].normal) == 0.0f) record->vertices[i].normal = *g_4687a8;
}

// @retail 0x117790
void __stdcall function_117790(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	function_117a80(index);
	long iterations = (long)((real)definition->iterations + record->speed * 3.0f);
	if (iterations < 1) iterations = 1;
	else if (iterations > 15) iterations = 15;
	for (long iteration = 0; iteration < iterations; ++iteration)
	{
		for (long i = 0; i < definition->link_count; ++i)
		{
			s_surface_link *link = &definition->links[i];
			short first_index = link->a;
			short second_index = link->b;
			point3f *a = &record->vertices[first_index].position;
			point3f *b = &record->vertices[second_index].position;
			real rest_squared = link->length*link->length;
			vector3f delta;
			vector3d_from_points3d(a, b, &delta);
			real delta_i = delta.i, delta_j = delta.j;
			real squared = dot3f(&delta, &delta);
			long bits = (*(long *)&squared >> 1) + 0x1fc00000;
			real length = *(real *)&bits;
			if (length < 0.0001f) length = 0.0001f;
			real inverse = 1.0f / length;
			vector3f direction = { delta_i*inverse, delta_j*inverse, delta.k*inverse };
			length = (rest_squared / (length*length + rest_squared) - 0.5f) * length;
			vector3f_scale(&direction, length, &direction);
			if (!bit_vector_test(record->pinned, link->a))
			{
				vector3f_add_scaled((vector3f *)a, &direction, -1.0f, (vector3f *)a);
				if (bit_vector_test(record->pinned, link->b)) vector3f_add_scaled((vector3f *)a, &direction, -1.0f, (vector3f *)a);
			}
			if (!bit_vector_test(record->pinned, link->b))
			{
				vector3f_add((vector3f *)b, &direction, (vector3f *)b);
				if (bit_vector_test(record->pinned, link->a)) vector3f_add((vector3f *)b, &direction, (vector3f *)b);
			}
			if (fabs(length) > 2.0f) { function_118430(index); --iteration; }
		}
		function_117a80(index);
	}
}

// @retail 0x117a80
void function_117a80(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	byte *object = surface_object_header(record->object_index)->object;
	point3f origin = *(point3f *)(object + 0x30);
	vector3f delta;
	vector3d_from_points3d(&origin, &record->origin, &delta);
	if (length_sq3f(&delta) > 0.1600000113248825f)
	{
		delta.i = origin.x-record->origin.x; delta.j = origin.y-record->origin.y; delta.k = origin.z-record->origin.z;
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			record->vertices[i].position.x += delta.i;
			record->vertices[i].position.y += delta.j;
			record->vertices[i].position.z += delta.k;
			record->vertices[i].previous = record->vertices[i].position;
		}
	}
	vector3f velocity;
	function_ba1d0(record->object_index, &velocity, NULL);
	record->origin = origin;
	record->speed = (real)sqrt(velocity.i*velocity.i + (velocity.j*velocity.j + velocity.k*velocity.k));
	for (long i = 0; i < record->attachment_count; ++i)
	{
		s_object_marker markers[6];
		long marker_name = definition->marker_name;
		long object_index = record->object_index;
		if (function_b8d30(object_index, marker_name, markers, 6, false) > 0)
		{
			s_surface_attachment *attachment = &record->attachments[i];
			record->vertices[attachment->vertex].position = markers[attachment->marker].matrix.position;
			record->pinned[attachment->vertex >> 5] |= 1 << (attachment->vertex & 31);
		}
	}
}

// @retail 0x117c80
void function_117c80(vector3f const *input, vector3f *output, real angle)
{
	real limit = (real)cos(angle);
	real magnitude = (real)fabs(input->k);
	*output = *input;
	if (magnitude > limit) output->k = input->k < 0.0f ? -limit : limit;
	if (fabs(output->i) < 0.01f && fabs(output->j) < 0.01f || fabs(surface_normalize_horizontal(output)) < 0.0001f)
	{
		output->i = 1.0f; output->j = 0.0f;
	}
	surface_normalize_horizontal(output);
	real horizontal = (real)sqrt(1.0f - output->k*output->k);
	output->i *= horizontal;
	output->j *= horizontal;
}

// MiniMax B (11-13): no new form, no lower score (117): retail keeps the old direction in xmm3-xmm5 and sums the dot as (axis.k*old.k + axis.j*old.j) + old.i*axis.i, an order the shared dot3f cannot produce, and the frame stays 0x28 against retail's 0x20 because the old direction's address reaches cross3f and dot3f; the rotation in place is the shared rotate_vector_170d70 body written out, which is kept only with a match.
// score 117 (was 117): frame 0x28 versus 0x20; seed/time and rotation register roles remain.
// MiniMax I (11-10): no new form, no lower score (117); the draw before the time product (193) and the oscillation
// before the draw (197) move the seed update up but split the float chain, so both are worse.
// MiniMax D (11-05): no new form, no lower score.
// MiniMax S (11-06): no new form, no lower score: retail still loads the random-state pointer before the time multiply and keeps the old direction in registers, ours spills the old direction to the stack and frames 0x28 against retail's 0x20.
// MiniMax G (11-06): no new form, no lower score: the frame is 0x28 against retail's 0x20 because retail loads
// the random-state pointer before the seed read, so our locals sit eight bytes higher (esp+0x28 and esp+0x2c
// against retail's esp+0x18 and esp+0x20).
// Earlier (round 28 reached 104, not kept: its gain needs the rotation's cross product as scalar terms, which is
// cross3f's body written inline and is kept only with a match; with cross3f the round's other changes give 163).
// At 104: frame 0x1c versus 0x20 (retail spills cross j as well as k and keeps axis.i in memory, so its
// rotation registers differ: axis j/k in xmm2/xmm1, the global's j/k in xmm5/xmm4); retail loads the seed pointer
// into ESI at entry and finishes the seed update before the time product; its tail computes the 0.001 cosine before
// the 0.0006f one and stores sinf's argument.
// Round 28: the rotation in place on the global in matched 0x177d10's order (projection, cross j, k, i as scalars)
// 152, with sinf/cosf in the tail 142, with the tail split at `wave` (two float factors, as retail) 104 (kept, with
// dot3f and random_index). On 104: (real)sin 145, sinf((real)..)/(real)sin((real)..) 104/104, cos pair swapped/
// cosf/(real)cos 104/104/104, plain sin and cos 149; random-state local at entry 104, random_unit_vector 104;
// dot3f(&axis, ..)/axis-first i,j,k 104/104, k,j,i/j,k,i/retail operand order 165/166/166 (864 bytes); dot3f with
// cross i,j,k 165 (864); cross order j,k,i,P/i,P,j,k/P,i,j,k/P,k,j,i/j,k,P,i 160/168/104/161/160; cross3f
// after/before the projection 163/163, cross3f_staged 163, the same aggregate body in place 163, rotate_direction 150,
// a same-file forceinline copy of it 150, with scalar cross terms 173 (860), rotate_vector_170d70 172 (860);
// axis drawn before sine/cosine 104; axis-first cross/scalar-first rows/axis-first rows/split sine,cosine 104 each;
// fraction before time 191, at the top 203, oscillation before the fraction 191, fraction inline 196, product
// reordered 104, definition after the draws 146 (882). Before the rotation change: rotate_direction 170 (861),
// cross3f/projection on the global 181 (854), the new tail alone 165 (889), plain 154 (881).
// The tail's product without the (real) cast in one expression 105 (0.01 as a double); scaled in its own
// statement 104 (kept, no cast).
// Round 26: the float time product (fimul), the random fraction and a float oscillation/angle gave retail's head
// x87 code (160 -> 139). Then: random drawn before the time product 197; double oscillation with float constants
// 140; sinf/cosf in the tail 157 (883 bytes), three spellings 157; cos operand order compiles the same; in-place
// rotation helper projection-first 183 (849 bytes), cross-first 188, dot3f with an aggregate cross 170, cross3f in the
// helper 171; projection then cross3f on the global directly 181; projection before cross3f on the copy 161.
// Earlier: frame 0x28 against 0x20 and random/time/oscillation staging remain; shared cross product reaches retail length.
// Earlier frame 0x24 against 0x20 and the random-seed/oscillation schedule. Reading the frame
// count again after the 0x117c80 call (as retail does) gives retail's convention: wind in EBX, definition in EDI.
// Complete output staging 201; old before axis 199, before trigonometry 199; vector names 195;
// three named cross terms 195; named variation 195; scalar declarations first 195;
// projection before shared cross 182. Shared cross product 168 fast/plain, 880/880 bytes.
// Round 219: immutable axis pointer 210; shared final double scale 168; both 210; native rotation
// setter 168; axis/setter 210; scale/setter 168; all 210; readonly old with axis pointer 210.
// Restored.
// Round 381: RNG before time 199; oscillation before RNG 203; axis before/between trigs 168/168;
// RNG-first/axis-between 199; oscillation-first/axis-first 203; rotation-scoped angle 201,
// with shared final scale 201. Late angle reaches frame 0x20 but loses wind register; all restored.
// Round 238: live direction inputs 209; complete output 199, with live inputs 188;
// all three cosine products 188, with live inputs 183; sine/cosine role names 168/168;
// separate trigonometric declarations with live inputs 209. Original snapshots and output order restored.
// Round 32: shared initial RNG 160; shared table-index draws 160; both 160;
// complete axis/direction CRT captures 168, with initial/table/both 160/160/160;
// all with alternate shared RNG contract 160. Initial shared RNG retained, plain 160; frame 0x28 remains.
// Round 264: all six lexical orders of the three rotation objects on the shared
// cross/RNG graph 160 each; vector declarations at entry/after angle 160/160.
// Original names and scopes restored; frame 0x28 and time/rotation staging remain.
// Round 401: uniform seed receiver, time receiver and both 160/160/160; projection before cross on
// shared-random graph 174, with seed/time/both 174/174/174; both receivers and commuted time
// factors 174. Original complete body restored; frame and rotation scheduling remain.
// Round 412: Staged cross 160; native old-direction setter 187; native axis setter 199; staged/old
// 187; staged/axis 199; both setters 208; all producers 208; all with alternate random contract
// 208. All restored.
// Round 216: existing in-place scale of direction/axis/both 180/188/191; cross alone 174;
// direction-cross/axis-cross/all 188/194/181; direction scale and shared partial-sum producer 179.
// Original workspaces, snapshots, three row trees and all publications restored.
// Round 221: j/k projection pair 160, uniform direction-first factors 178; either projection before
// shared cross 176/176; existing dot-helper orders before cross 179/179; named cosine complement
// 176, with direction-first factors and staged cross 176. Original body restored; frame 0x28 remains.
// Round 49: old snapshot before axis/before trigonometry on the current shared graph 164/164;
// const old/both inputs/staged cross with both const 160 each; post-cross dot3f in either input order
// on the const staged graph 181 each; const staged inputs with shared random_index draws 160.
// Original body restored.
// DL210: native tail producer/clamp result/component output, masks 1-7, then all with existing vector setter; component-output frame with staged cross/projection-first/both/both and readonly inputs;
// DL210 earlier scores 139/139/139/169/169/169/169/143/169/165/165/165. Original source forms restored.
// Round 96 (one-link harness of this file with 0x30bf0's, exact at 139; the full build already passes wind in
// EBX with a plain ret as retail): cross3f(&axis, &old, &cross) with `+ cross*sine` rows 166, with the
// projection first 167 (864 bytes); rows as sine*cross / projection*axis / cosine*old 139 each (same code);
// cross3f on the global with the projection first and rows in place 181 (852 bytes). Diagnostics (scalar
// cross terms, kept only with a match): 0x177d10's in-place order on the global 181 (850 bytes; retail loads
// the global's j/k before the axis draw), the same on an `old` copy 163.
// Round 35 (lane 10, fast): matched 0x177d10's rotation (projection, cross j, k, i) as a shared forceinline helper
// (direction pointer, axis by value; 0x177d10 stays matched with it, with an axis pointer it scores 17): on the
// global 189 (845 bytes, frame 0x18: the global's fields are re-read at each use, as retail 0x177d10 does through
// its pointer but retail 0x117dd0 does not), with random_index 193, with a split float tail 177; on a local copy
// written back 174 (895), with the tail 191. Retail 0x117dd0 holds the old vector in registers, so it is not the
// same inline sequence as 0x177d10. Helper removed.
// Round 136 (lane 4, one-link harness exact at 139 and 104): tail with the cosines swapped, sinf, cosf last, a `wave`
// split before or after `* 0.01f` (48 forms) 139 at best (sinf 160, split inside the cast 154-165); `old` copy or direct
// global reads x dot3f / `(k + j) + i` projection x cross3f / scalar cross j,k,i or i,j,k x row order (48) 139 at best,
// direct global reads with scalar cross terms 152; round 28's global-in-place scalar cross + random_index + sinf/cosf
// tail split at `wave` 104 (--fast 104; scalar cross terms kept only with a match), with the cosines swapped, the fraction
// first 189-191, a seed pointer local, sinf/cosf for the angle 105, separate sine/cosine 104; on the cross3f body:
// sinf/cosf tail 160, split tail 165. At 104: frame 0x1c (retail spills cross j, keeps axis i in memory, global j/k in
// xmm5/xmm4), seed update before the time product, tail cosine order.
// Round 37 (lane 13, one-file LTCG harness equal to the full build, harness counts, base 119): random_index for both
// table draws in place of the file-local generator copy 119 (same code, kept); the unit-axis rotation helper of
// unknown_2551c0.cpp/unknown_050650.cpp (projection first, scalar cross i, j, k) as a file-local inline on the
// global 150 (849 bytes, frame 0x18), with dot3f 136 (866), forceinline 150, with random_index 153; the tail's
// float factors in their own `wave` statement 118; sinf/cosf in the tail in three spellings, with or without the
// split, 156 (883 bytes: the cos operands reorder, the sinf argument is not stored, 0.01f * 0.3f fold); cross3f
// on the global without the `old` copy 152 (854 bytes, frame stays 0x28), with the split tail 147.
// Round 39 (lane 13, harness, base 119): retail's frame 0x20 holds time, tick/angle/cosine, the 0x30bf0 result/cross k,
// sine, random int/cross j/a stored tail float and axis; the global is read straight into registers (j, k early, i
// late). Rotation on the global with cross3f(&g_55ecd4, ..) 152 (854), projection first 148 (852), dot3f(&axis,
// &g_55ecd4) 153 (856); diagnostics (scalar cross, kept only with a match): 0x177d10's own form with its axis_k local on
// the global 135 (862, aligned 101), on an `old` copy 148.
// Round 41 (lane 13, --fast): `old` copied field by field 159 (the global's j/k then load straight into xmm registers as in
// retail, but the frame grows to 0x34), declared before the axis 163, as `{ g.i, g.j, g.k }` 159; field-wise `old` with the
// projection before cross3f 148 (frame 0x28; retail's store/reload of the first cross product appears, but cross3f gives
// i, j, k where retail forms j, k, i), with dot3f(&old, &axis) 148, with cross3f_staged 148, with `projection*axis` rows
// 148; cross3f_staged alone 159; the projection before cross3f on the aggregate copy 161 (885 bytes). Diagnostics (scalar
// cross, kept only with a match): 0x177d10's j, k, i cross on the field-wise `old` 176 (853), on the global 181 (850).
// DH round 273: native real tail trigonometric results at first/second/third/fourth/first-pair/last-
// pair/all producers 155/139/155/139/155/155/138; all with real double-product arguments 163.
// Precision-only forms restored; frame and seed/rotation staging remain.
// M290: Completed rotation in the consumed direction/axis/cross workspace, then projection-first on each and staged cross with direction/axis output:
// 178/178/178/189/189/189/178/178. Original executable body restored.
// Round 154 (lane 4, one-link harness, exact here, then plain): dot3f(&axis, &old), the j and k rows projection-first and a
// `wave` split with (real)sin kept 117 (880, aligned 68). Not kept, written cross terms and dot (cross3f's and dot3f's bodies):
// sine, axis, cosine order with cosine-first rows 104 (aligned 63, frame 0x1c); the same inside a forceinline pointer helper
// with in-place trig args 104; scalar captures of the global's fields old_j/old_k (frame 0x20) 151 (aligned 58), old_i/old_j/old_k
// 130 (882), old_j/old_i/old_k 128 (882); rotation through scaled_old/scaled_axis/scaled_cross locals or vector3f_add_scaled 151+;
// cross3f_staged 117 (same code). Field-mention diagnostics on axis and the global 143-146 (scratch only). Retail's colouring
// (axis k/j xmm1/xmm2, global i/k/j xmm3/xmm4/xmm5) and spilled cross j/k are still open.
// Symmetric rows on the kept form: old-first 158 (882), cosine-first 158 (882), projection-first 134 (881); retail's own adds put the
// old i product first and the axis j and k products first, which the kept rows follow.
// Round 508: Source-local forceinline cross with publication ijk/ikj/jik/jki/kij/kji
// 139/139/139/139/157/157; staged-cross declaration with ijk/jki 139/139.
// Direct publication ikj/jik/jki/kij/kji and staged ikj/jki/kji
// 139/139/139/157/157/139/139/157. Original declarations and full body restored;
// frame 0x28 and input/rotation/tail homes remain.
// Round 299: Complete native-array rotation consumer; direct initialization result, staged cross,
// projection-first and final-output-loop interactions 169/172/169/172/165/165/169/183. All original
// source restored; frame, old-direction storage and seed/time/rotation/tail scheduling remain.
// N304: Uniform scalar direction/axis/cross product families, seven subsets
// then all immutable: 157/139/139/139/157/139/139/139. Original body restored;
// frame 0x28/0x20 and old-direction, seed/time and tail scheduling remain.
// Round 328: actual random-angle/rotation/clamp phase scopes in all seven
// subsets, then all with immutable complete rotation inputs, 117 each. Original
// scopes and full body restored; frame 0x28/0x20 and seed/direction/cross homes remain.
// P271: Consumer-scoped angle on retained projection/row/tail graph, independent
// entry old/axis/cross declarations masks 0..7: 165/165/165/165/165/165/165/165.
// Original body restored; frame and seed/direction/rotation homes remain.
// Round 364: Real-coordinate-array transfers for direction/axis/both/both plus output,
// long ordinals 167/183/190/190; short ordinals 117/141/141/152. Long axis forms shrink
// the frame to 0x24, direction alone grows it to 0x30; all original transfers and body
// restored; frame 0x28, seed/time and rotation homes remain.
// DH round 290: sampled axis ordinal short/word/long/dword reused for all three native
// component reads, aggregate producers 161/161/161/161; shared setter 173/173/173/173.
// Original draws, native axis/direction captures, cross/dot, all rows and tail restored.
// Round 533: Sequential real cosine or wave reuse from the initial envelope,
// original/immutable time/immutable angle/both input ownership: 117 each. Original
// scalar lifetimes and complete body restored; frame 0x28 versus 0x20, seed/time
// schedule and direction/axis/cross homes remain.
// AC480: Existing in-place double output scale / complete initialization result / named amplitude
// envelope, subsets 1/2/4/3/5/6/7, then double scale with weighted oscillation: 158/178/118/204/159/178/204/158.
// Original transfers, producers and complete body restored; frame and direction/cross homes remain.
// P275: Complete final wind value via native aggregate/existing setter, with uniform
// direction capture and final scale/result-only scope, subsets 0-7:
// 156/192/156/192/156/192/156/192. Original body restored; frame and seed/rotation homes remain.
// J241: Native tail argument producers at first/second/sine/last/first-pair/last-pair/all
// boundaries 117 each; real sine argument and result 164 at 888 bytes. NOT kept: unmatched
// precision and one-use argument/result storage. Original entire body and math APIs restored;
// frame 0x28 versus 0x20 and seed/time/direction/cross homes remain.
// P277: Actual time/fraction/envelope/angle/projection/wave/strength scalar home names,
// individually then all seven: 117 each, 880/880 bytes. Original names/body restored;
// seed/time and direction/cross residence remain, frame 0x28 versus 0x20.
// Round 345: existing fraction/bounded-draw/projection forceinline declarations, seven subsets; all with
// nested generator: 117/117/117/117/117/117/117/117. Original body and contracts restored. Frame 0x28
// versus 0x20 and seed/time/direction/cross homes remain.
// N332: Native clock-value and initial weighted-fraction producers, seven subsets
// then all immutable: 117 each. Original entire body restored; frame 0x28/0x20,
// seed/time scheduling and direction/axis/cross homes remain.
// N339: Initial sampled-direction table receiver with field/native-copy/array/complete-value
// transfers, direct then short sampled ordinal: 161/155/188/235/161/155/188/228.
// Original initial acquisition and complete body restored; frame 0x28/0x20 and
// seed/time/direction/cross storage remain.
// NOT kept: transfer/one-use ordinal and value-copy forms regress beyond native length.
// Round 199 (fast, with 0x118140): the convention already agrees (index EAX, wind EBX, plain ret); the 8 extra frame
// bytes are the stack copy of `old` (retail reads the global's j/k straight into XMM5/XMM4). `old` declared before
// the axis draw 121; the projection as `(1 - cosine) * dot3f(..)` 117; sine and cosine declared separately 117; `cross`
// declared ahead of the trig values 117; the frame-count store as a plain `g_55e710 = g_4ba034;` 207 (886 bytes,
// 0x118140 0 -> 53: the reload after the call is needed).
// Round 47 (lane 15, fast): scalar j, k, i rotation on the global, cross j and k in locals as retail spills them, 175 (856 bytes, frame
// 0x1c); casts on the sine argument and last cosine for retail's three float stores 164 (888); both 165 (864); both with the RNG first
// 209 (861); the dot on the global with cross3f and both casts 163 (880). Body unchanged at 117.
// N343: Initial definition record/tag-handle/tag-table receiver families, seven subsets then
// table-first, 117/117/117/117/117/117/117/117. Original acquisition and full body restored;
// frame, seed/time and direction/cross homes remain. NOT kept: unmatched one-use receivers.
// Round 206 (m3 lane 3, fast): sine, axis draw, then cosine 117 (same code); cross3f(&axis, &old) added 134 (881
// bytes); cross and dot on the global with the `old` copy after them 164 (893); diagnostic: 0x2551f0's unit-axis rotate
// body on the `old` copy, copied back 201 (918); the fraction draw before the time 193. Retail ret is plain, as ours.
// P322: Independent sine/cosine/projection entry declarations, seven subsets then
// all with immutable time: 117 each. Every producer stays at its original point;
// frame 0x28/0x20, seed/time and direction/cross homes remain. Original body restored.
// B422: Current-frame rotation admission, bool/byte/short/long, positive/complementary:
// 117/117/117/117/117/117/117/117. NOT kept: no byte match; all canonicalize to the original complete body.
// Original observations restored; frame 0x28/0x20 and seed/time/direction/cross homes remain.
// P324: Independent clock/fraction, envelope/angle and wave/strength entry
// declarations, masks 1-7, then all with the rotation scalar declarations:
// 117 each. All producers stay at their original statements; original body
// restored. Frame 0x28/0x20 and seed/time/direction/cross homes remain.
// N368: Complete double cosine-product/sum/full-wave boundaries and their sequential
// producers, then complete wave accumulation: 117 each; complete wave/strength
// accumulation 118. NOT kept: no equality or gain; original expression graph restored.
// Frame 0x28/0x20 and seed/time/direction/cross homes remain.
// Round 69 (lane 11, fast, compile-only screens exact for the tail): the tail alone is solved: `cos((real)(time * 0.001)) *
// cos(time * 0.0006f) + sin((real)(time * 0.005))`, times `cos(time * 0.0001f)`, gives retail's x87 sequence op for op (the
// first cast is retail's fxch pair and its cosine order, the sine cast its two fst stores). It adds 12 bytes that the rotation must
// give back: on the current rotation 171 (892 bytes), the first cast alone 164 (888), the sine cast alone 164 (888). With it,
// cross3f on the global 158 (880), projection before cross3f 163 (880); scalar cross on the global (kept only with a match) with
// dot3f 147 (884), `old_k`, `old_j` read before the axis 151 (880, aligned 49), read after the axis 148 (880, aligned 47). Those
// captures give retail's early xmm5/xmm4 loads and the store-and-reload of cross k; still open: retail adds the global's i term
// last with its load late, every direct read adds it first; capturing i as well ranks it last but loads it early and spills.
// J254: Existing record/tag/limiter declaration contracts, force subsets 1/2/4/3/5/6/7 and ordinary
// acquisition with inline limiter: 117/117/604/117/604/604/604/117. NOT kept: force-limiter forms
// grow to 1247/880 and lose matched 0x3db00. All provider bodies, definitions/declarations and
// complete target body restored; frame 0x28/0x20 and direction/cross homes remain.
// P339: Complete old-direction scalar captures ijk/ikj/jik/jki/kij/kji,
// immutable component aggregate and existing setter: 154/154/155/134/153/132/154/157.
// NOT kept: all regress; original full capture, products and body restored.
// Native 0x20 frame, seed/time order and old/cross homes remain; exact peers survive.
// N370: Input-owned cross result, old/axis native copies and component aggregates, projection-first copies and readonly aggregates:
// 181/196/164/159/180/234/164/159. NOT kept: no equality; original complete body restored.
// Frame 0x28/0x20 and old-direction/cross/seed/time homes remain.
// N377: Incremental row/complete-phase rotation outputs, partial-sum and first-product
// construction, then genuine local partial results/native setter/consumed old workspace:
// 117/153/116/183/182/205/182/182. NOT kept: unmatched publication/lifetime forms;
// original complete body restored. Frame 0x28/0x20 and direction/cross/seed homes remain.
// N380: Initial envelope/rotation sine/rotation cosine real APIs, masks 1..7, then
// all with complete angle accumulation: 117 each. NOT kept: no byte match; original
// producers and body restored. Frame 0x28/0x20 and seed/direction/cross homes remain.
// N382: All output rows read their original global component while shared cross/dot retain
// the complete old snapshot; staged-cross/projection-first/projection-accumulator masks 0..7:
// 159/159/157/157/159/159/157/157. NOT kept: unmatched mixed observations; original complete body restored.
// Frame 0x28/0x20 and seed/time/direction/cross homes remain.
// Round 56 (m3 lane 15; one-link harness: entry calling 0x118140 plus src/unknown_030290.cpp, exact, 2 s a form). Recipe, kept only with a match since it writes
// out dot3f and cross3f: sine, cosine, axis; `real axis_i = axis.i; real old_k = g.k; real old_j = g.j;`; the dot in stages `p = axis.k*old_k; p += axis.j*old_j; p += g.i*axis_i;
// p *= 1 - cosine`; scalar cross j, k, i and rows; tail `(cosf(time*0.001)*cosf(time*0.0006f) + sinf(time*0.005)) * cosf(time*0.0001f)`: 103 (885 B, masked 25: frame, spills, dot and
// tail are retail's). With `short ticks = field_2_3;`, the draw, then the time 96 (885 B, first difference +0x35); `long ticks` and a separate draw statement, harness, 159 (882 B, aligned 28).
// Left: `axis.k*g.i` and `axis.j*g.i` load g.i first where retail copies the axis, `sine*cross_j/k` reloads, the opening's RNG/time schedule. Source operand order, cross statement order,
// staged cross or rows, sine/cosine placed later, pointer or reference reads of the global and a rotation helper taking (vector, axis, sine, cosine) do not change them (about 6,500 forms).
// Fast: the float tail alone on this body 171 (892 B); dot3f with a scalar cross 104 (880 B); `short ticks` alone 110. NOT kept: helper bodies written out, one-use local under 8.
// T59: Consumed direction/axis/cross workspaces through the existing scaler on the retained
// mixed-row/split-wave graph, subsets 1/2/4/3/5/6/7 and all with staged cross:
// 172/181/164/182/180/187/174/174. NOT kept: frames and extents miss retail; original
// complete captures, shared products, row trees, observations and body restored.
// Round 59 (m3 lane 15, fast, base 117; the build already returns with a plain ret and takes wind in EBX): one seed pointer local for the
// three draws 136; the gust value and direction globals (0x55ecd0/0x55ecd4, only used here) as one struct 117 (same code); sinf/cosf for the
// angle 117; no `old` copy, cross/dot/rows on the global into a `rotated` local stored back 161 (885 B); the axis draw before sine/cosine 117;
// the frame-count and initialized-flag globals (0x55e710/0x55e714) as one struct 117 (same code).
// P384: Complete normalization admission, bool/byte/short/long positive then complementary:
// 117/117/117/117/117/117/117/117. NOT kept: no equality or native old/cross home recovery.
// Original direct unordered-safe test, true-only initialization store and complete body restored; frame 0x28/0x20 remains.
// MiniMax E (11-08): no new form, no lower score: our frame is eight bytes larger (sub esp, 0x28 against retail's 0x20) and the global load sits before the push edi.
// P396: Complete sine/cosine/projection double receivers, seven subsets then immutable all:
// 231/220/218/220/218/218/218/218. NOT kept: nonexact precision forms shrink to 804-818/880 bytes;
// original float coefficients, full captures, shared products, draws and whole body restored.
// MiniMax E (11-09): no new form, no lower score: our frame is eight bytes larger (sub esp, 0x28 against retail's 0x20) and the global load sits before the push edi.
// Round 141 (m3 lane 7, fast, 0x118140 matched; our build already returns with a plain ret and takes wind in EBX): `old` as a const
// reference to the global 173 (862 B, frame 0x28); the angle as `oscillation * oscillation * fraction * 0.15f` 117 (same code); the
// projection before the cross product 158 (883 B, frame 0x24); the oscillation squared in place 197 (882 B); the axis draw before
// sine/cosine 117 (same code); the length test as `>= 0.0001f` 170 (882 B); cosine computed before sine, own statements 118; the
// definition read only at the strength product 208; the time product's operands swapped 117, the frame-count compare's operands
// swapped 117 (same code).
// M3 lane 14 r74 (fast, base 117, aligned 72; 0x118140 stays matched in each; retail and ours both end in a plain ret): the rotation as a file-local
// inline of unknown_26db50.cpp's form (dot3f(axis, vector) scaled by 1 - cosine, the vector's i, j, k copied, rows `axis * along + old * cosine -
// (cross) * sine`) on the global 156 (880 bytes, aligned 84), the same forceinline 156, with dot3f(vector, axis) 156, with the i row old-first 156 (all
// the same code); the kept rows into a `rotated` local copied to the global 183 (903 bytes); cross3f and dot3f reading the global with the `old` copy
// kept for the rows 164 (893); the `old` copy taken after the cross and dot, just before the rows 173 (862). Body unchanged.
// (The first four are one compiled body; the cross/dot-on-the-global form re-measures round 206's 164 at 893 bytes.) On the opening: the time
// product with its factors swapped 117 (same code); the time globals through a pointer local used for both reads 117 (same code); the fraction
// held as a double 122; the angle as `fraction * (oscillation * oscillation) * 0.15f` 197 (882 bytes), the square as a named local 197 (882).
// Round 166 (m3 lane 6, fast with 0x118140; masked = register-masked aligned count, base 72; plain ret on both sides): `old`
// copied before the trig values and the axis draw 121 (masked 96); the cross from the global itself with the dot and rows
// from `old` 142 (889 bytes); row i with the axis term first, as rows j and k, 134 (881 bytes); axis and old in one
// declaration 117 (same code); the projection's factors the other way round 117 (same code). Body unchanged.
// Round 87 (m3 lane 1, fast with 0x118140, base 117, aligned/masked 68/56; ours and retail both end in a plain ret, the round
// prompt's ret 4 is not what this build gives): 0x2551f0's (vector, axis, sine, cosine) rotate helper applied in place on the
// global (same-file copy, diagnostic) 169 (864 bytes), with the sine and cosine computed in the call as 0x2551f0 does 148 (880,
// masked 49: the helper re-reads the global's j from memory at each use where retail keeps j and k in XMM5/XMM4). Matched 0x177d10
// draws its axis the same way (g_4417f0 by random_index 0x402) and writes the same rotation out (s, c, t, cross j/k/i, rows); round
// 41 measured that shape here (176/181); 0x177d10's exact statements on the global 143 (880, aligned 75); matched 0x23d260's
// staged shape (axis k local, staged projection, cross i/j/k, staged rows stored as they finish) on the global 188 (846 bytes).
// No other natural rotation or opening form outside this note's list is left. Not kept.
// Round 148 (m3 lane 8, fast with the file's other functions, 0x118140, 0x30bf0 and 0x117c80; base 117, aligned 68, masked 45),
// each the same code (117, 880 bytes): `old` as a const copy; dot3f(&old, &axis); the axis as a const copy; `cross` declared
// ahead of the axis and `old`; the three rows and the 0x117c80 call through a named direction pointer to the global.
// The frame's extra eight bytes are `old` kept in memory (its address goes to cross3f and dot3f); retail holds the global's
// j and k in XMM5/XMM4 and reads i late, and none of these forms changes that.
// M3 lane 16 r100 (fast, 0x118140 stays matched; the build already returns with a plain ret and takes wind in EBX), re-measured on
// the current body: the `old` copy dropped with cross3f, dot3f and the rows on the global 173 (862 B; round 37 had 152 at 854),
// the projection first 175 (858 B, frame 0x1c; round 39 had 148 at 852), that with dot3f(&g_55ecd4, &axis) or vector-first rows
// 175 (same code); the cross-file rotate_direction on the global (plain check) 183 (852 B; round 28 had 150): cross i then sinks
// into the i row and cross j, k form early as in retail, but the global's j/k reload in the i row. Round 69's tail (`cos((real)(time
// * 0.001))`, `(real)sin((real)(time * 0.005))`) re-measured 171 (892 B; retail's fxch pair and fst stores), with rotate_direction
// 171 (864 B), with rotate_vector_170d70 170 (864 B); with rotate_direction on an `old` copy stored back 161 (889 B). Body unchanged.
// m3 lane 2 r71 (fast with the file's functions and 0x30bf0): the axis as a pointer into the table (no axis copy)
// 159. Retail copies the axis into the frame with integer moves, keeps no copy of the global (its j and k load early
// into XMM5/XMM4, i late) and spills only cross j and k: no form that keeps cross3f/dot3f on the existing helpers
// gives that frame, and the hand-written rotation is match-only. Body unchanged.
// M3 lane 9 r109 (fast with 0x118140, 0x30bf0 and 0x117c80; base 117, aligned 100, masked 72; both sides end in a plain
// `ret`): the fraction drawn through random_next with `(real)... * (1.f / 65535.f)` in place of function_x82e52f 117 (same
// code, not kept); the clamp of g_55ecd0 through a file PIN macro (`PIN(x, lo, hi)` as other files define it) 117 (same code,
// not kept); the wind written through the existing scale_vector_by_double 118 plain, and matched 0x118140 drops to 50
// (round 219's shared final double scale on this base: not kept). No other untried natural form of the opening, rotation or
// tail. Body unchanged.
// M3 lane 14 r83 (fast with the file and 0x30bf0; both sides end in a plain `ret`, index in EAX): cross3f(&axis, &old) with
// `+ cross * sine` in the rows 134 (881 B); rows j and k written old-first 158 (882 B); the projection's dot on the global
// with cross3f on `old` 141 (890 B), that dot before cross3f 143 (894 B); the dot then cross3f, both on the global, with
// `old` for the rows only 162 (890 B). Same code: scalar-first products, the projection staged as `*= 1 - cosine`,
// parenthesised row sums, dot3f(&g_55ecd4, &axis). Retail keeps no `old` or `cross` copy in memory (the global's j/k in
// XMM5/XMM4, cross j/k spilled early, cross i formed inside the i row); only scalar code gives that.
// m3 lane 8 r158 (fast with 0x118140, 0x30bf0, 0x117c80; both sides end in a plain `ret`): the permuter (`--fast --anneal`,
// 600 s after an 856 s full-image baseline) tried 64 variants, best 117 (none lower); re-measured the listed definition
// read only at the strength product 208 (880 B; 0x118140 stays matched). Register-masked aligned 45: the frame (0x28 for 0x20: `old`
// lives in memory because its address reaches cross3f and dot3f), the seed pointer loaded after the time product (retail
// before it) and the rotation block (retail keeps no `old` or `cross` copy: the global's j/k in XMM5/XMM4, cross j/k spilled,
// cross i formed in the i row), which only the hand-written rotation (an existing helper's body, match-only) gives.
// M3 lane 7 r156 (fast with the file, 0x30bf0, 0x117c80; plain `ret` and index in EAX on both sides; base masked 72): tail
// `(real)sin((real)(time * 0.005))` 164 (888 B, an fxch pair before fsin; `sin((real)..)`, a `(real)` on the 0.0006f cosine and the
// cosine factors swapped the same code); with `(real)cos(time * 0.001)` too 165 (888 B: the sine term gets retail's `fst; fsin;
// fst`, the cosines still run 0.0006f first). Match-only, written-out cross and dot terms in retail's dataflow (dot3f(&axis,
// &g_55ecd4), scalar cross_j/cross_k, the i row's cross term inline, rows on the global): with that tail 142 (880 B, frame 0x1c,
// masked 53: ours copies the global's component in each product where retail copies the axis's; dot arguments swapped, a
// direction pointer, a const reference or a named cross_i the same code), with the base tail 145, cross terms first 159; old
// i/j/k scalar captures after the axis 165 (the axis is then the copied operand, but all three loads go early); an `old` copy after
// the axis with dot3f 157, with the dot written out 113 (880 B, masked 90, `old` in the frame, 0x24), that with the base tail 158,
// with `old` declared first 117 (other code); j/k captured and i read from the global 149 (frame 0x20, first difference +0x2a),
// that with retail's dot grouping `(k + j) + i` 124 (frame 0x20, masked 61), i captured after the j/k terms 164. Body unchanged.
// m3 lane 14 r88 (fast with 0x118140, 0x30bf0, 0x117c80; both sides end in a plain `ret`, frame 0x28 against 0x20):
// rotate_vector_170d70 on an `old` copy stored back 374 (664 B: the helper stays a call here). Every other rotation, seed
// and opening form found is listed above; retail's opening loads the random-state pointer into ESI before the time
// product and its rotation reads the global straight into XMM5/XMM4/XMM3 with the dot summed k, j, then i. Body unchanged.
// MiniMax B (11-12): no new form, no lower score (117): the frame is 0x28 against retail's 0x20 and the entry keeps the
// random's state pointer in ESI where retail reloads it, so the locals differ before the first compare; the body is
// retail's length (880 bytes).
// MiniMax H (11-11): no new form, no lower score (117); retail keeps the old direction in xmm3-xmm5 and computes the dot as (axis.k*old.k + axis.j*old.j) + old.i*axis.i, an order dot3f cannot produce, and the frame stays 0x28 against retail's 0x20 because the old direction's address reaches the cross and dot helpers.
// MiniMax D (11-11): no new form, no lower score (117); the frame is still 0x28 against retail's 0x20, and the
// eight extra bytes are still the copy of the old direction that has to live in memory because its address is
// handed to the cross and dot helpers; dropping that copy and reading the global at each use gives 173. Retail
// keeps the rotation scalar, so the cross terms are the shared helper's body written out, which is kept only
// with a match; with the helper in place the rotation stays in the vector form.
// MiniMax E (11-11): no new form, no lower score (117); retail's frame is 0x28 bytes where ours is 0x20 and the
// seed global is read later in ours than in retail.
// MiniMax S (11-11): no new form, no lower score (117): the frame stays 0x28 against retail's 0x20 because retail
// loads the random-state pointer before the seed read, and the old direction's address is passed to cross3f and
// dot3f here where retail keeps the old direction in registers.
// T104: Returned direction/axis/both snapshot destinations, mutable/readonly memcpy, then
// complete memmove: 161/161/160/161/161/160/178/178. NOT kept: no equality or native frame/home
// recovery; original full captures, shared products, draws, row trees and entire body restored.
// MiniMax H (11-14): no new form, no lower score (117): retail keeps the old direction in xmm3-xmm5 and sums the dot as (axis.k*old.k + axis.j*old.j) + old.i*axis.i, an order the shared helpers cannot produce, and the frame stays 0x28 against retail's 0x20 while the old direction's address reaches them.
// @retail 0x117dd0
void function_117dd0(long index, vector3f *wind)
{
	s_surface_definition *definition = surface_definition(surface_record(index));
	real time = (real)g_510c54->game_time * g_510c54->field_2_3;
	real fraction = function_x82e52f(&g_4e7408->seed, NULL, 0);
	real oscillation = (real)cos(time * 0.005f);
	real angle = fraction * oscillation * oscillation * 0.15f;
	if (!g_55e714)
	{
		g_55ecd4 = g_4417f0[random_index(&g_4e7408->seed, 1026)];
		if (!(fabs(function_30bf0(&g_55ecd4)) < 0.0001f)) g_55e714 = true;
	}
	if ((long)g_4ba034 > g_55e710)
	{
		real sine = (real)sin(angle), cosine = (real)cos(angle);
		vector3f axis = g_4417f0[random_index(&g_4e7408->seed, 1026)];
		vector3f old = g_55ecd4;
		vector3f cross;
		cross3f(&old, &axis, &cross);
		real projection = dot3f(&axis, &old) * (1.0f-cosine);
		g_55ecd4.i = old.i*cosine + axis.i*projection - cross.i*sine;
		g_55ecd4.j = axis.j*projection + old.j*cosine - cross.j*sine;
		g_55ecd4.k = axis.k*projection + old.k*cosine - cross.k*sine;
		function_117c80(&g_55ecd4, &g_55ecd4, 0.34906584f);
		/* retail reloads the frame count after the call, as for an address-taken global (0x4a7c0, 0x248620) */
		g_55e710 = *(dword const *)&g_4ba034;
	}
	real wave = (real)((cos(time * 0.001) * cos(time * 0.0006f) + (real)sin(time * 0.005)) * cos(time * 0.0001f)) * 0.01f;
	g_55ecd0 += wave * 0.3f;
	if (g_55ecd0 < -0.1f) g_55ecd0 = -0.1f;
	else if (g_55ecd0 > 0.1f) g_55ecd0 = 0.1f;
	double strength = (fabs(g_55ecd0) + 0.0075f) * definition->wind_scale * 0.08000000566244125f;
	wind->i = (real)(g_55ecd4.i * strength); wind->j = (real)(g_55ecd4.j * strength); wind->k = (real)(g_55ecd4.k * strength);
}
// @retail 0x118140
void function_118140(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	s_surface_vertex *vertex;
	vector3f wind;
	function_117dd0(index, &wind);
	if (record->speed < 1.5f)
	{
		vector3f acceleration = *g_4687a4;
		real gravity = definition->gravity * -3.2086613178253174f;
		real current_weight = 2.0f - definition->damping;
		real previous_weight = 1.0f - definition->damping;
		acceleration.k += gravity / (real)g_510c54->field_2_3 * 0.03125f;
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			vector3f force = wind;
			if (!bit_vector_test(record->pinned, i))
			{
				vertex = &record->vertices[i];
				point3f position = vertex->position;
				point3f previous = vertex->previous;
				position.x *= current_weight; position.y *= current_weight; position.z *= current_weight;
				previous.x *= previous_weight; previous.y *= previous_weight; previous.z *= previous_weight;
				position.x = position.x - previous.x; position.y = position.y - previous.y; position.z = position.z - previous.z;
				position.x = acceleration.i + position.x; position.y = acceleration.j + position.y; position.z = acceleration.k + position.z;
				real dot = vertex->normal.j*force.j + vertex->normal.k*force.k + vertex->normal.i*force.i;
				vector3f projected;
				projected.i = vertex->normal.i*dot; projected.k = vertex->normal.k*dot; projected.j = vertex->normal.j*dot;
				vector3f tangent;
				vector3d_from_points3d((point3f *)&projected, (point3f *)&force, &tangent);
				vector3f_scale(&tangent, definition->tangent_drag, &tangent);
				vector3d_from_points3d((point3f *)&tangent, (point3f *)&force, &force);
				vector3f_add((vector3f *)&position, &force, (vector3f *)&position);
				vertex->previous = vertex->position;
				vertex->position = position;
			}
		}
	}
	else for (long i = 0; i < definition->vertex_count; ++i)
	{
		vertex = &record->vertices[i];
		vertex->previous = vertex->position;
	}
}

// @retail 0x118430
void __stdcall function_118430(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	for (long i = 0; i < definition->vertex_count; ++i)
	{
		s_surface_vertex *vertex = &record->vertices[i];
		point3f *source = &definition->vertices[i].position;
		vertex->position = *source;
		vertex->previous = *source;
		vertex->normal = *g_4687ac;
	}
	record->origin = *g_468788;
	function_117a80(index);
}

#include "flexible_surface_calls.h"
extern s_record_pool *g_509434;
long __stdcall function_3ddd0(long);
void function_1cf50();
void function_1ccb0(long);
void function_1cd90();
byte *g_485a80;
// Render-state storage read by the two external state helpers.
// Attribute pairs terminated by 255, as consumed by the retail state helper.

class c_surface_reference_view
{
public:
 virtual void slot0() = 0; virtual void slot1() = 0;
 virtual void slot2() = 0; virtual void slot3() = 0;
 virtual void slot4() = 0; virtual void slot5() = 0;
 virtual void slot6() = 0; virtual void slot7() = 0;
 virtual void slot8() = 0; virtual void slot9() = 0;
 virtual byte *reference() = 0;
};

static inline byte *surface_tag(long index) { return g_4e3b44[index & 0xffff].bytes; }
static inline byte *surface_pointer(byte *base, long offset) { return *(byte **)(base + offset); }

// MiniMax C (11-01): no new form, no lower score.
// score 322 (was 422): frame 0x2c, the attribute scans, the side loop and the outer vertex calls are retail's; left are
// 0x1ccb0/0x1cd90 called where retail inlines them (BLOCKED-BY 0x1ccb0: its /Ob1 file), 0x4b2d0's convention (retail
// tag EDX, first ECX, second EAX, third/fourth on the stack) and the stack homes of the scan results and the sign.
// Earlier: frame 0x44 versus 0x2c; attribute scans and D3D call expansion differ.
// Integer orientation 529; null-first lighting 522; per-scan results 530; real type
// cursors 574; existing format/state helpers 473 (kept, full); cursors/orientation
// 573; helpers/orientation 471 (two-point addition dropped); all three 516.
// Round 209: Fresh type-field reads/cursors 498/497; with per-scan results 482/498; existing oriented
// tangent setter 473, tangent and normal setters 472 (one-point lead dropped); both setters with fresh
// cursors/results 497/482. All restored; frame 0x44/0x2c, descriptor scans and outer D3D expansion remain.
// Round 280: paired first-match scans normal/tangent and binormal/uv 514;
// normal/binormal and tangent/uv 514; normal/uv and tangent/binormal 504;
// triple normal/tangent/binormal 481, normal/tangent/uv 495, normal/binormal/uv 495;
// all four 491; triple tangent/binormal/uv 487. All original scans restored.
// Round 238: application inline/force-inline 485/485; format inline/force-inline 526/526;
// both helpers inline or force-inline, all four combinations 528. Application-inline forms also
// lose the 0x22a664 reduced control; all original helper declarations and calls restored.
// Native attribute descriptor with uniform type/count observations and complete
// result-role identities: 473/498/473/498/473/498/473/498. All original fields and scans restored.
// Round 163: shared independent-search ordinal, original/per-search defaults
// 473/475; with uniform descriptor-base observations 498/482; native exit-owned
// results 510/510, with descriptor observations 533/533. All original searches
// and APIs restored; 0x44/0x2c frame and outer vertex API expansion remain.
// Round 264: existing 4f/2f API ordinary-inline-noinline declaration pairs, all
// eight non-original combinations 473 each. Original declarations and body restored;
// frame 0x44/0x2c and selective outer vertex API expansion remain.
// Round 401: paired lookup parameter orders: five leading tag/group/variant permutations 473 each;
// trailing context/pass swap 517; context/pass then tag/group/variant 471, pass/context then
// tag/variant/group 471. Two-point gains rejected; every definition, declaration, caller and
// original body restored.
// Round 39: independent search orders T/N/B/U, N/B/T/U, N/T/U/B, B/T/N/U,
// U/N/T/B, T/B/U/N, B/U/N/T, U/B/T/N all 473. Original search order and APIs restored.
// Round 425: signed orientation loop 468; C++ APIs 473; unsigned attributes 510; loop/APIs 468,
// loop/unsigned 505, APIs/unsigned 510, all 505, short orientation 539. Late sign on the signed loop
// 453 (kept); shared vector scales alone/with late sign 468/453; opposite winding 482, late 483,
// scales 480, both/named cull 481/481. Late sign on the original side loop 485, integer producer 469.
// Both orientation passes and all call argument roles are preserved; other forms restored.
// Round 429: Positive admission/entry result declarations/lexical result homes and their four
// combinations 453 each; all with native record/cull names 453. Original body restored.
// DL201: pointer-distance first matches, cached/direct type observations and shared/phased defaults
// 500 each. Standard searches give 390/388/374/372 and frame 0x2c, but outline one new
// search provider with only this caller: rejected. Literal values, readonly iterators and
// force-inline declarations before/after the include 372 each; no inline recovery.
// Original searches, signed orientation, APIs and helper graph restored.
// Round 240: complete oriented/binormal/position API inputs and their pairs/all
// 462/434/441/448/450/422/436; all with readonly vertex/rest receivers 436.
// Keep only binormal and position values, each consumed through all three components: 422 plain,
// 1452/1361 bytes. Original APIs, both orientation passes, fields and arithmetic preserved.
// DL210: independent normal/tangent/texture API inputs, masks 1,2,4,5,6,7; texture with native oriented scales, masks 5,7;
// scores 432/424/422/432/424/436/422/422. Original source forms restored.
// Round 244: retained binormal/position readonly values, complete aggregate
// initializers and lexical role names, seven subsets 422/453/453/422/422/453/453;
// entry uninitialized value workspaces 422. Original carriers and body restored.
// Round 254: Own-result scan ordinals on retained complete API inputs, cached/live count,
// compound/break search and original/entry declarations 549/480/549/480/549/480/549/480. All restored.
// Round 37 (lane 13, one-file LTCG harness with the D3D members, equal to the full build; harness counts): retail
// inlines 0x1ccb0(59) and 0x1cd90 here (and in 0x19f680), but they sit in the /Ob1 src/unknown_01cf50.cpp, so ours
// calls them and LTCG inlines the two outer SetVertexData4f calls instead (retail calls 0x3fb2d0): BLOCKED-BY 0x1ccb0.
// Lighting pointer NULL first and set under the test 320 (base 324); each scan with its own NONE default and the
// types read per scan 322 (aligned 258 -> 225, 1467 bytes: retail's pointer-walking scans), also with the count read
// per scan 322; a counted side loop with `side == 0 ? 1 : -1` signs, two spellings 331 (1465); lighting and loop
// 327; all three 320 (1481). All restored.
// Round 39 (lane 13, harness, base 324 / aligned 258): binormal straight from *g_4687a8 326 (1469), position from the
// vertex 324 (1464), both 326 (1481), short strip index 324 (same code); rest pointer first 324 (aligned 215, --fast
// 422), with per-scan types 322 (aligned 182, 1467), with lighting NULL-first 320 (--fast 418), all three 318 (--fast
// 429). All restored; BLOCKED-BY 0x1ccb0 remains.
// Round 41 (lane 13, --fast): also BLOCKED-BY 0x4b2d0: retail passes variant/group/tag in EAX/ECX/EDX with two stack
// arguments, ours three on the stack (0x4b2d0 sits in the same contributor file). Retail's 0..1 side loop (`side == 0 ?
// D3DCULL_CCW : D3DCULL_CW`, sign `(real)(side == 0 ? 1 : -1)`), sign before or after the cull call 441/441 (1465 bytes);
// lighting pointer NULL before the lookup and set under `lighting_index != NONE` 415 (retail's `xor edi, edi` before the
// call; a gain of 7, under the keep bar), with the side loop 434, with the types read per scan 432 (1470), with the rest
// pointer first 415, all three 435; each scan's NONE default declared just before it 424, with NULL-first lighting 480,
// plus per-scan types 496. All restored.
// Round 131 (m3 lane 8): the four scans through the shared vertex_input_index (include/vertex_inputs.h, also used by
// 0x224db0) give retail's frame 0x2c, and LTCG then keeps both outer SetVertexData4f calls out of line as retail: 366
// with a walked types pointer, 348 reading the element in the loop (kept); retail's 0..1 side loop 335 (kept; 441 on the
// old body); the rest-vertex pointer before the vertex pointer 336 (kept: 412 without it on the final body); position
// and binormal passed by field, no copies, 322 (kept). Not kept: lighting NULL before the lookup 315 (a gain of 7), after
// it 318; `inline` on the 0x1ccb0 and 0x1cd90 definitions 476 (old body; 0x22a664 lost), 500 with the side loop;
// `inline` on 0x1ccb0 alone 243 at 1339 bytes, but 0x47930 244 -> 341 and 0x19f680 316 -> 448; member-form strip calls
// 422 (same code); the search with early returns 332. 0x19f680 has the same scan, for a later round.
// M341: Actual typed descriptor on retained binormal/position graph, uniform type/count
// observations and per-scan defaults, masks 0..7: 422/439/422/439/424/433/423/433. Original
// descriptor/searches restored; frame, application convention and outer API expansion remain.
// @retail 0x116b00
void __stdcall function_116b00(long tag_index, long unused, long group, long variant, long pass, long index, void *context)
{
	if (!function_117100(index)) return;
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	long lighting_index = function_3ddd0(record->object_index);
	void *lighting = lighting_index == NONE ? NULL : g_509434->data + (lighting_index & 0xffff)*0x100 + 0x44;
	function_1bd50(lighting);
	function_1cdd0(0, 0);
	function_4b2d0(tag_index, group, variant, ((byte *)context)[4], pass);
	long material_index = tag_index == NONE ? *(long *)(g_485a80 + 0xd0) : tag_index;
	byte *reference;
	dword kind = *(dword *)&g_4e3b44[(short)material_index];
	if (kind == 0x5052544d || kind == 0x70727433)
	{
		c_type_4e7709 *provider = function_137bd0(material_index);
		// This slot returns a tag-reference pointer in retail. The existing
		// interface leaves this slot untyped; keep its public declaration.
		reference = ((c_surface_reference_view *)provider)->reference();
	}
	else reference = surface_pointer(surface_tag(material_index), 0x24);
	byte *groups = surface_pointer(surface_tag(*(long *)reference), 0x5c);
	long group_offset = *(word *)(surface_pointer(groups, 4) + group*10) & 0x1ff;
	long variant_offset = *(word *)(surface_pointer(groups, 0xc) + (group_offset+variant)*2) & 0x1ff;
	long shader_index = *(long *)(surface_pointer(groups, 0x14) + (variant_offset+pass)*10 + 4);
	byte *shader = surface_pointer(surface_pointer(surface_tag(shader_index), 0x20), 4);
	byte *attributes = surface_pointer(surface_tag(*(long *)(shader + 0x100)), 8);
	long normal = vertex_input_index(attributes, 4);
	long tangent = vertex_input_index(attributes, 5);
	long binormal = vertex_input_index(attributes, 6);
	long uv = vertex_input_index(attributes, 3);
	DWORD old_cull;
	D3DDevice_GetRenderState(D3DRS_CULLMODE, &old_cull);
	function_1ccb0(59);
	function_1cd90();
	function_1cf50();
	for (long side = 0; side < 2; ++side)
	{
		real sign = (real)(side == 0 ? 1 : -1);
		D3DDevice_SetRenderState(D3DRS_CULLMODE, side == 0 ? D3DCULL_CCW : D3DCULL_CW);
		if (tangent != NONE) D3DDevice_SetVertexData4f(tangent, g_4687ac->i*sign, g_4687ac->j*sign, g_4687ac->k*sign, 1.0f);
		if (binormal != NONE) D3DDevice_SetVertexData4f(binormal, g_4687a8->i, g_4687a8->j, g_4687a8->k, 1.0f);
		D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
		for (long i = 0; i < definition->strip_index_count; ++i)
		{
			long vertex_index = definition->strip_indices[i];
			s_surface_rest_vertex *rest = &definition->vertices[vertex_index];
			s_surface_vertex *vertex = &record->vertices[vertex_index];
			if (normal != NONE) D3DDevice_SetVertexData4f(normal, vertex->normal.i*sign, vertex->normal.j*sign, vertex->normal.k*sign, 1.0f);
			if (uv != NONE) D3DDevice_SetVertexData2f(uv, rest->u, rest->v);
			D3DDevice_SetVertexData4f(0, vertex->position.x, vertex->position.y, vertex->position.z, 1.0f);
		}
		D3DDevice_End();
	}
	D3DDevice_SetRenderState(D3DRS_CULLMODE, old_cull);
}

// @retail 0x117060
void __stdcall function_117060(void *submission)
{
	function_40f60(submission, function_d4bc0, function_116b00);
}

s_shader_storage g_shader_storage;
