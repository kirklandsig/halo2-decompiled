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
struct s_surface_link { long unknown0; short a, b; real length; long unknownc; };
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

// score 139 (was 139): the 139 differing lines are all real ones (no unmatched callees here):
// retail loads the tag table into EDX before saving the registers and keeps the tag data
// base in EBX where ours keeps it in EDI, and with it the record and the iteration count:
// retail's record is in EDI and its count in EBX, ours the other way round. The endpoint
// and SSE carriers, the split pinned-word test and the correction staging differ as well;
// the frame 0x1c and the entry agree.
// Earlier: frame 0x1c agrees; link-base/endpoint carriers, pinned-word test and correction staging differ.
// Round 28: the existing particle_distance_estimate_2b for the root (an LTCG cross-file inline) 139, per-use 154;
// per-use with the root through a long pointer or a union 154/154 (same code); the existing bit_vector_test for the
// pin tests 139 (same code, kept: it replaces the file-local copy), per-use 154; per-use with the rest length read
// before / between the vertex pointers 154/154. Retail re-reads link->a after the root's stack stores (ours keeps it).
// Round 26 (fast): retail re-reads link->a/link->b at every pin test (per-use form, aligned 156 -> 91, 742 bytes);
// on it: a before b 154, b before a 157, rest length read at its square 175, vector3d_from_points3d delta 154, const
// link 154, i,j,k square order 154. Left there: our first pin test keeps the top link->a read in EBX and splits
// its test, and the link pointer add folds into the first field load. All restored.
// Shared difference 154; vertex base 155; distinct correction 161, with shared difference 161;
// both shared scales 154, with difference 154; early rest square 155; all producers/lifetimes 162.
// All executable forms restored.
// frame 0x1c agrees; link/endpoint carriers, split pin test and correction scheduling differ.
// Shared difference/scales 154; vertex base 155; distinct correction 161, with difference 161;
// early rest square 155; all producer/correction forms 162. Endpoint short snapshots forward 176,
// reverse 139; extended long 183 in either order; shared pin helper ties each form.
// Reverse short snapshot ordinary check 139/741.
// Native endpoint vertices with rest square/correction/shared-scale subsets:
// 139/138/151/150/139/138/151/150. The one-point leads are dropped; original body restored.
// Round 163: both pin decisions shared after/before correction, bool 201/201,
// byte 201/201, word 189/189, long 195/195; with uniform live endpoint fields
// bool/byte/word/long 212/212/200/206. All restored; 0x1c frame, entry/link homes
// and correction/pin-word scheduling remain.
// Round 264: live record/definition/both/link/record-link/definition-link/all receivers
// 309/232/379/147/309/254/377; definition acquired before record 149. All restored;
// frame 0x1c, entry pool/record carriers and correction/pin scheduling remain.
// Round 196: retain only first endpoint with point/vertex/shared-difference/link-cursor forms
// 179/179/179/173; retain only second 211/211/211/159. All restored; native entry/link
// carriers, pin-word observations and correction scheduling remain.
// Round 113: link->a/link->b read at each use (no endpoint locals) 154 at 742 bytes, but it gives retail's
// prologue and outer loop (record EDI, iterations EBX; first difference +0x8 -> +0x8a; aligned 116 -> 77);
// left: the link pointer is folded (`[edx+eax+4]`, retail `add eax,[esp+0x18]`), the first pin test reuses
// the top read of link->a in EBX (retail re-reads it) and splits its test, a/link swap EAX/EDX. On it:
// rest square before the clamp 155 (twice); shared point difference 154; long bits local 154;
// short-parameter forceinline pin test 163; fields through definition->links[i] 220. All restored.
// Round 418: entry declarations for definition/count/both/link/definition-link/
// count-link/all 139 each; all with coherent geometric role names 139. Every
// acquisition stays at its original operation. All restored; entry record/link
// allocation, pinned-word observations and correction scheduling remain.
// Round 86 (harness, exact for this function): per-use link fields 154 (742 bytes; retail's link/endpoint
// setup except the folded link offset, and the first pin test reuses the top link->a read where retail
// re-reads it), with a `const &` link 155; existing vector3d_from_points3d for the difference 139 (same
// code), with per-use fields 154, with dot3f for the square 164/178; root through a bits local, a union,
// a long pointer or in place 154/154/154/158; constant-first clamp 139 (same code), per-use 154;
// `!(length >= c)` 155; rest square from link->length at its use 175.
// Round 35 (lane 10, fast): per-use link fields again 154 (742; first difference +0x8a: link materialized after one
// folded read, link->a kept in EBX across the root's stack stores, a/link in EAX/EDX); with the rest length read
// first 154, link as `definition->links + i` 154, vertex-record endpoints 154, a separate correction local 161.
// Round 254: Complete endpoint correction helpers for first/second/both, plain component loops for
// first/second/both, then both with shared difference 139/139/139/191/186/203/139/203. All restored.
// Round 37 (lane 13, one-file LTCG harness equal to the full build, harness counts): per-use link fields 151
// (aligned 91, 742 bytes); on it the link as `definition->links + i`, the square as i,j,k / length_sq3f / dot3f,
// the correction or the normalize through vector3f_scale, the delta through vector3d_from_points3d and a const
// link 151 each, `!(length >= c)` 152, b's pointer first 151, the rest length at its use 169; endpoint locals
// for the pointers with link fields in the pin tests 151, the pointers through a file-local inline 151 (ours
// still reuses the top link->a read where retail reads it again); fast root in place on length 155 (base 148),
// in place on squared 151 (base 138). Base forms: link + i 138, rest at use 172, i,j,k / helper squares 138,
// `!(>=)` clamp 138, vector3f_scale correction 138, b first / a,b locals swapped 172/172. All restored.
// Round 39 (lane 13, harness, base 138, per-use 151 / aligned 91): per-use with the rest square at the top 152 (aligned
// 85), delta as j, k, i statements 151 (87). Diagnostics only: a compiler barrier after the fast root 138 (aligned 74),
// after the clamp 134 (aligned 72: retail's link setup and pin-test re-read appear). Retail has something VC treats as a
// memory write between the top link->a read and the first pin test; no source construct found yet.
// Round 41 (lane 13, --fast): per-use link fields 154 (742 bytes; ours colours delta j/k/i xmm5/xmm6/xmm4 and length
// xmm2, retail xmm6/xmm4/xmm3 and xmm5); on it `real length`, `real squared, length`, `vector3f delta` or the link
// pointer declared at function scope 154 each (scope reaches neither the re-read nor the colouring). All restored.
// M290: Exact 1..15 bounded counts long/short/word/byte on indexed links, then each on the live-count link cursor:
// 148/156/156/189/180/183/183/183. Original executable body restored.
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
			short second_index = link->b;
			short first_index = link->a;
			point3f *a = &record->vertices[first_index].position;
			point3f *b = &record->vertices[second_index].position;
			real rest_length = link->length;
			vector3f delta = {b->x-a->x, b->y-a->y, b->z-a->z};
			real squared = delta.k*delta.k + delta.j*delta.j + delta.i*delta.i;
			real length;
			*(long *)&length = (*(long *)&squared >> 1) + 0x1fc00000;
			if (length < 0.0001f) length = 0.0001f;
			real inverse = 1.0f / length;
			delta.i *= inverse; delta.j *= inverse; delta.k *= inverse;
			real rest_squared = rest_length * rest_length;
			length = (rest_squared / (length*length + rest_squared) - 0.5f) * length;
			delta.i *= length; delta.j *= length; delta.k *= length;
			if (!bit_vector_test(record->pinned, first_index))
			{
				a->x -= delta.i; a->y -= delta.j; a->z -= delta.k;
				if (bit_vector_test(record->pinned, second_index)) { a->x -= delta.i; a->y -= delta.j; a->z -= delta.k; }
			}
			if (!bit_vector_test(record->pinned, second_index))
			{
				b->x += delta.i; b->y += delta.j; b->z += delta.k;
				if (bit_vector_test(record->pinned, first_index)) { b->x += delta.i; b->y += delta.j; b->z += delta.k; }
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

// score 139 (was 139): frame 0x28 versus 0x20; seed/time, rotation and tail scheduling remain.
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
// after/before the projection 163/163, cross3f_staged 163, the same struct body in place 163, rotate_direction 150,
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
// rotation helper projection-first 183 (849 bytes), cross-first 188, dot3f with a struct cross 170, cross3f in the
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
// scores 139/139/139/169/169/169/169/143/169/165/165/165. Original source forms restored.
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
// 148; cross3f_staged alone 159; the projection before cross3f on the struct copy 161 (885 bytes). Diagnostics (scalar
// cross, kept only with a match): 0x177d10's j, k, i cross on the field-wise `old` 176 (853), on the global 181 (850).
// M290: Completed rotation in the consumed direction/axis/cross workspace, then projection-first on each and staged cross with direction/axis output:
// 178/178/178/189/189/189/178/178. Original executable body restored.
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
		real projection = (axis.k*old.k + axis.j*old.j + axis.i*old.i) * (1.0f-cosine);
		g_55ecd4.i = old.i*cosine + axis.i*projection - cross.i*sine;
		g_55ecd4.j = old.j*cosine + axis.j*projection - cross.j*sine;
		g_55ecd4.k = old.k*cosine + axis.k*projection - cross.k*sine;
		function_117c80(&g_55ecd4, &g_55ecd4, 0.34906584f);
		/* retail reloads the frame count after the call, as for an address-taken global (0x4a7c0, 0x248620) */
		g_55e710 = *(dword const *)&g_4ba034;
	}
	g_55ecd0 += (real)((cos(time * 0.001) * cos(time * 0.0006f) + sin(time * 0.005)) * cos(time * 0.0001f) * 0.01f * 0.3f);
	if (g_55ecd0 < -0.1f) g_55ecd0 = -0.1f;
	else if (g_55ecd0 > 0.1f) g_55ecd0 = 0.1f;
	double strength = (fabs(g_55ecd0) + 0.0075f) * definition->wind_scale * 0.08000000566244125f;
	wind->i = (real)(g_55ecd4.i * strength); wind->j = (real)(g_55ecd4.j * strength); wind->k = (real)(g_55ecd4.k * strength);
}
// score 63 (was 63): frame 0x48, normal.k cursor and the history (else) loop agree; weight publication, the Verlet x
// register (retail keeps it in xmm6 to the final add, ours stores it) and vector scheduling remain.
// Earlier: frame 0x48 and normal.k cursor agree; weight publication, the Verlet x register (retail keeps it
// in xmm6 to the final add, ours stores it), vector scheduling and the history-loop base remain.
// Earlier: frame 0x48 and normal.k cursor agree; weight publication, vector scheduling and history-loop base remain.
// Not kept (MiniMax A, 11-01): the two weights in one two-element local (weight[1] the current, weight[0]
// the previous) sit in retail's slots: 71, but an array grouping locals to move their stack homes is not
// kept. Named weights declared before the gravity step, the position scaled before the previous is read
// and the previous stored after the force step: 73 (fast; a gain of 7, under the keep bar). Left: the loop
// still reloads the current weight first, the dot-product tree and the vector scheduling differ, and the
// else loop keeps its own base (previous instead of position).
// Round 36 (MiniMax A, on the array form): the acceleration as the first addend 112; the else loop through a
// vertex pointer 108; the Verlet difference and the acceleration split into separate statements 172 (776
// bytes); the position scale written z, x, y 72; the difference written x, z, y 71 (the same code); the dot
// written (i + k) + j 103; the permuter run killed with no output. All restored.
// Earlier (80): frame 0x48 and normal.k cursor agree; weight publication, vector scheduling and history-loop base remain.
// Earlier: 744 bytes and the normal.k cursor agree; retail adds the acceleration as the first operand and
// keeps position.x in a register through the force step, evaluates the dot as (i + k) + j (ours i, j, k), and its
// else loop runs through a vertex pointer. Round 28: the projection through the existing vector3f_scale 103, with
// dot3f(&vertex->normal, &force) 80 at 744 bytes (kept; force stays in memory as in retail, dot3f(&force, ..) 80),
// with the drag through vector3f_scale 80 (kept), named drag 81; early weights 80/102 (with/without dot3f); the
// dot written i,j,k / force-first i,k,j / force-first i,j,k / k,i,j 104/103/104/103 (736 bytes); on 80: the
// acceleration first 118, one-expression Verlet / acceleration first 183/183 (767), scaled position then
// one-expression 139; else loop through a vertex / position pointer 114/114 (aligned 80 -> 65); tangent through
// vector3f_add_scaled(&force, &normal, -dot, ..) with dot3f / the written dot 86/86; tangent built in place 114.
// Earlier first line: the loop's vertex cursor points at the normal's j (retail: k), so the dot product,
// Round 26 (fast, fast baseline 101): Verlet steps as one expression per component 176 (760 bytes), with the
// acceleration first 176; that plus the else-loop vertex pointer 134 (747 bytes); the else-loop pointer with the drag
// field read at each use 204 (719 bytes); all three 171 (744 bytes). All restored.
// frame 0x48 agrees; normal.j versus normal.k cursor and weight/vector scheduling differ.
// tangent and position sums load and schedule in another order; retail adds the acceleration and the force
// as first operands. Weights stored after the acceleration (retail stores them first, with the current
// weight above the previous). Tried: dot sums i,j,k / k,i,j / j,k,i 109 each; dot3f either argument order
// 117 (744 bytes); weights before the acceleration 109; previous weight declared first 113, before the
// acceleration 113; current weight renamed 109; acceleration and force as first addends 116 (744 bytes).
// Shared tangent scale 109; previous point first 122; outer vertex declaration 109; native scaling loop
// 120; scale helper/previous first 122; native additions 116; scaling/additions 154, with helper 151.
// All restored.
// Complete late normal snapshot 189; with symmetric displacement 226, direct drag field 194, both 186;
// early normal snapshot 192, with displacement 225, direct drag 194, both 186. Snapshots grow the frame.
// Round 221: Shared point-setter boundaries for both weights/both sums/both 108/108/108;
// complete tangent/force assignment boundaries alone/with either/both point phases 195 each;
// all with intrinsic fast-history copy 194. One-point gains rejected; all original source restored.
// Round 216: native projected-normal scale/vertex cursor/both 103/205/187; overlap-safe
// point-history transfers alone/projection/cursor/all 212/213/207/199; in-place projection 153.
// On projected normal, early weights/acceleration first/force first/both/all 102/113/103/113/110;
// weight point setters 104, with all 112. Normal.k cursor recovered at 103/102; sub-eight gains restored.
// Round 71: the else loop through a vertex pointer (`vertex->previous = vertex->position`) gives retail's
// else loop and its pad exactly and retail's 744 bytes (aligned 85 -> 66) but scores 116 (the main loop's
// shifts); with acceleration and force as first addends 113 (aligned 59); with the tangent through
// vector3f_add_scaled(..., -dot, ...) 196 (normal.k cursor, 712 bytes), plus dot3f 195; with the projection
// through scale_vector_by_double 150 (normal.k cursor, frame changes). Retail loads acceleration.i/j into
// a register before adding (ours folds the memory operand).
// Round 263: damping shared by both weights 154; force copy at its consumer 194;
// both 184; pin-test continue 109, with damping/late force/both 154/194/184;
// shared damping with native projected-normal scale 159. All original source restored.
// Round 39: complete inertial displacement/accelerated displacement/accelerated value/next point
// 159/159/115/124; with native normal projection 166/166/109/118 (frames 0x54/0x60/0x6c).
// Native projection plus early weights and history vertex cursor, then force first/acceleration
// first/both, 112/112/111/111 at 744 bytes. Original body restored.
// Round 425: projected normal and early weights with shared tangent scale 104; force loop 112;
// paired weighted-point loop 118; tangent/force 114, tangent/weights 119, force/weights 118, all 119;
// all with final position loop 119. Five-point best rejected; all original source restored.
// Round 311: native projection/early weights with history scope, projection-consumer scope,
// paired point-role names and their combinations 102/102/102/102/102/102/102;
// all with history-branch cursor 112. Seven-point gains rejected; original body restored.
// DL210: native point setters for weighted/accelerated/final point phases, masks 1-7, then all with CRT history copy on the shared dot/projection graph;
// scores 80/80/80/79/79/79/79/114. Original source forms restored.
// Round 35 (lane 10, fast): slot/store order of the preheader spills follows where the previous weight is defined
// relative to the k update (names change nothing: history_weight/position_weight 80, also with weights first 80):
// weights before the acceleration 80 (retail's store order, slots swapped), previous weight first 84 (slots right),
// current weight first and previous after the k update 78 (slots right, k stored between: 2 points, not kept),
// seven other orders 80 each; history copy before the force subtraction 80, before the drag scale 125 (734);
// final add through vector3f_add(&force, position, position) 81, with weights first 80; dot grouped
// n.j*f.j + (n.i*f.i + n.k*f.k) 112 (gives retail's dot loads k, i, j and force registers, rest shifts), with
// acceleration-first Verlet 106 (736), with the 78 order 112; dot k, i, j 105 (736); Verlet through
// vector3d_from_points3d + vector3f_add 170 (776); else-loop vertex pointer with the 78 order 113.
// Round 136 (lane 4, one-link harness exact at 80): retail keeps the Verlet x in xmm6 until the final add and reloads
// all three normal components for the projection (ours writes x home and reloads normal i early into xmm6). Separate
// `-=`/`+=` Verlet statements + `i + (k + j)` written dot + `tangent.i *= field` 148 (--fast; definition takes ESI, record
// spilled); accel-first / `a + acc - b` / `acc + a - b` Verlet x dot3f either order / `(i + k) + j` / `j + (i + k)` x
// written projection x drag field at each use / named drag / `force -= tangent * drag` x final add order (256 forms)
// 80 best (same code), parenthesised dots 104-105 (736); previous copied first 96, previous scaled first 104, force block
// before or inside the Verlet 168-200, previous copy before the Verlet 128; force/position/vertex declared outside the
// loop, projected/tangent declared early or assigned, acceleration assigned 80 each.
// Round 37 (lane 13, one-file LTCG harness equal to the full build; harness counts read 4 below the check): the history
// copy after the final add 76 -> 70, the final add through the existing vector3f_add 77 alone, both 68 (check 72, kept).
// On 68: else loop through a vertex pointer 110 (aligned 73 -> 55: retail's else loop exactly, the main loop reschedules;
// on it a 3-pass search over orders and forms reaches 91), weights first 70, position scale orders 68/70/94/70/94,
// previous scale orders 70/69/71/70/70, Verlet orders 68/68/138/89/147, acceleration-first Verlet 114, split -=/+= 137,
// Verlet through vector3d_from_points3d + vector3f_add 139, force copied inside the pin test 164, vertex pointer
// before the test 68, previous copied first 84, scales through vector3f_scale 68, dot3f(&force, ..) 68, written dot
// i,k,j / k,i,j / i,j,k 82/82/81 (736 bytes), j + (i + k) / (i + k) + j 105/105 (aligned 55: retail's dot), k + (i + j)
// 109, force-first products 82/105, written projection 113, tangent statements 68, written drag 179, force-update
// orders 68, history copy before the drag / after the Verlet / after the force update 113/116/77.
// Round 39 (lane 13, same harness): one function-scope `vertex` pointer assigned in both loops 68 -> 59 (retail's else
// loop; --fast and plain 72 -> 63, kept). On it a 22-dimension coordinate descent found nothing lower: dot
// `(i + k) + j` / `j + (i + k)` 105/105 (aligned 45, 744 bytes), acceleration-first Verlet 114, weights first 61, written
// dots i,k,j / k,i,j / i,j,k 81/81/80 (736 bytes); weights first with `(i + k) + j` 104, acceleration first with it 80,
// both 82; weight names in the other sort order with either preheader order 59/61 (names move no home).
// Round 41 (lane 13, --fast): retail loads the acceleration component and adds the difference (x then stays in xmm6 and
// its early store goes); Verlet rows written `acceleration.i + (position.x - previous.x)` 116 (VC still folds the
// acceleration as a memory operand); a velocity local then `velocity + acceleration` rows 172 (776 bytes),
// vector3f_add(&acceleration, &velocity, ..) 174 (776), vector3f_add(&velocity, &acceleration, ..) 174 (776); the
// acceleration filled field by field, k summed in its own statement / in the same one, 183/187. All restored.
// Round 43 (lane 13, --fast, control 63): tangent through vector3d_from_points3d(projected, force) 66; drag
// applied as vector3f_add_scaled(&force, &tangent, -drag, &force) 189 (719 bytes); the dot passed straight to
// vector3f_scale 63 (same code); both helper forms 66; the point-difference tangent with a written dot
// `i*i + k*k + j*j` 97 (736 bytes). All restored.
// Round 339: Native acceleration aggregate, paired weighted-point aggregates and uniform vertex-count
// observation, seven subsets 187/265/265/2540/2692/2248/2373; all with CRT wind transfer 2373.
// Original producers/counts restored; frame agrees, preheader and vector scheduling remain.
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
		acceleration.k += gravity / (real)g_510c54->field_2_3 * 0.03125f;
		real current_weight = 2.0f - definition->damping;
		real previous_weight = 1.0f - definition->damping;
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
				position.x = position.x - previous.x + acceleration.i;
				position.y = position.y - previous.y + acceleration.j;
				position.z = position.z - previous.z + acceleration.k;
				real dot = dot3f(&vertex->normal, &force);
				vector3f projected;
				vector3f_scale(&vertex->normal, dot, &projected);
				vector3f tangent = { force.i - projected.i, force.j - projected.j, force.k - projected.k };
				vector3f_scale(&tangent, definition->tangent_drag, &tangent);
				force.i -= tangent.i; force.j -= tangent.j; force.k -= tangent.k;
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
// score 422 (was 422): frame 0x44 versus 0x2c; descriptor scans, application convention and outer vertex API expansion remain.
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
	long count = *(long *)(attributes + 4);
	short *types = *(short **)(attributes + 8);
	long normal = NONE, tangent = NONE, binormal = NONE, uv = NONE;
	for (long i = 0; i < count; ++i) if (types[i] == 4) { normal = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 5) { tangent = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 6) { binormal = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 3) { uv = i; break; }
	DWORD old_cull;
	D3DDevice_GetRenderState(D3DRS_CULLMODE, &old_cull);
	function_1ccb0(59);
	function_1cd90();
	function_1cf50();
	for (long orientation = 1; orientation >= -1; orientation -= 2)
	{
		D3DDevice_SetRenderState(D3DRS_CULLMODE, orientation > 0 ? D3DCULL_CCW : D3DCULL_CW);
		real sign = orientation;
		if (tangent != NONE) D3DDevice_SetVertexData4f(tangent, g_4687ac->i*sign, g_4687ac->j*sign, g_4687ac->k*sign, 1.0f);
		if (binormal != NONE)
		{
			vector3f binormal_direction = *g_4687a8;
			D3DDevice_SetVertexData4f(binormal, binormal_direction.i, binormal_direction.j, binormal_direction.k, 1.0f);
		}
		D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
		for (long i = 0; i < definition->strip_index_count; ++i)
		{
			long vertex_index = definition->strip_indices[i];
			s_surface_vertex *vertex = &record->vertices[vertex_index];
			s_surface_rest_vertex *rest = &definition->vertices[vertex_index];
			if (normal != NONE) D3DDevice_SetVertexData4f(normal, vertex->normal.i*sign, vertex->normal.j*sign, vertex->normal.k*sign, 1.0f);
			if (uv != NONE) D3DDevice_SetVertexData2f(uv, rest->u, rest->v);
			point3f position = vertex->position;
			D3DDevice_SetVertexData4f(0, position.x, position.y, position.z, 1.0f);
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
