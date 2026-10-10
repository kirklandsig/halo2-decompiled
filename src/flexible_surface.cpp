// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "effects.h"
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
static inline bool surface_pinned(s_surface_record *record, long i)
{
	return (record->pinned[i >> 5] & (1 << (i & 31))) != 0;
}
static inline real surface_normalize(vector3f *v)
{
	real length = (real)sqrt(v->i*v->i + v->j*v->j + v->k*v->k);
	if (!(fabs(length) < 0.0001f))
	{
		real inverse = 1.0f / length;
		v->i *= inverse; v->j *= inverse; v->k *= inverse;
		return length;
	}
	return 0.0f;
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
void object_widget_delete(long, long);
extern long g_4b9ed8;
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
	s_record_pool *pool = g_4e0338;
	long index = data_datum_index(pool, function_16bc00(pool, 0));
	while (index != NONE)
	{
		long slot = index & 0xffff;
		((s_surface_record *)pool->data + slot)->unknown_c = 0.0f;
		function_117790(index);
		function_118140(index);
		function_117510(index);
		function_117080(index);
		pool = g_4e0338;
		index = data_datum_index(pool, data_find_index(pool, index == NONE ? 0 : slot + 1));
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
		if (g_4e8c20->entries[g_4b9ed8] == *(long *)(object + 0x13c))
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
	long order[6] = {0, 1, 2, 3, 0, 0};
	record->attachment_count = function_b8d30(record->object_index, definition->marker_name, markers, 6, false);
	long axis = (definition->flags & 2) ? 0 : 2;
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
			long vertex;
			if (i == 0) vertex = 0;
			else if (i == record->attachment_count - 1) vertex = (definition->rows - 1) * definition->columns;
			else vertex = (long)(ceil(i * step) * definition->columns);
			record->attachments[i].vertex = (short)vertex;
			record->attachments[i].marker = (short)order[i];
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
		point3f origin = *(point3f *)(surface_object_header(object_index)->object + 0x30);
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			s_surface_vertex *vertex = &record->vertices[i];
			point3f *source = &definition->vertices[i].position;
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
		s_surface_vertex *a = &record->vertices[definition->triangle_indices[i]];
		s_surface_vertex *b = &record->vertices[definition->triangle_indices[i+1]];
		s_surface_vertex *c = &record->vertices[definition->triangle_indices[i+2]];
		vector3f v = {b->position.x-a->position.x, b->position.y-a->position.y, b->position.z-a->position.z};
		vector3f u = {c->position.x-a->position.x, c->position.y-a->position.y, c->position.z-a->position.z};
		vector3f normal = {u.k*v.j-u.j*v.k, u.i*v.k-u.k*v.i, u.j*v.i-u.i*v.j};
		surface_normalize(&normal);
		a->normal.i += normal.i; a->normal.j += normal.j; a->normal.k += normal.k;
		b->normal.i += normal.i; b->normal.j += normal.j; b->normal.k += normal.k;
		c->normal.i += normal.i; c->normal.j += normal.j; c->normal.k += normal.k;
	}
	for (long i = 0; i < definition->vertex_count; ++i)
		if (surface_normalize(&record->vertices[i].normal) == 0.0f) record->vertices[i].normal = *g_4687a8;
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
			point3f *a = &record->vertices[link->a].position;
			point3f *b = &record->vertices[link->b].position;
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
			if (!surface_pinned(record, link->a))
			{
				a->x -= delta.i; a->y -= delta.j; a->z -= delta.k;
				if (surface_pinned(record, link->b)) { a->x -= delta.i; a->y -= delta.j; a->z -= delta.k; }
			}
			if (!surface_pinned(record, link->b))
			{
				b->x += delta.i; b->y += delta.j; b->z += delta.k;
				if (surface_pinned(record, link->a)) { b->x += delta.i; b->y += delta.j; b->z += delta.k; }
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
	point3f origin = *(point3f *)(surface_object_header(record->object_index)->object + 0x30);
	vector3f delta = {record->origin.x-origin.x, record->origin.y-origin.y, record->origin.z-origin.z};
	if (delta.k*delta.k + delta.j*delta.j + delta.i*delta.i > 0.1600000113248825f)
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
		if (function_b8d30(record->object_index, definition->marker_name, markers, 6, false) > 0)
		{
			long vertex = record->attachments[i].vertex;
			record->vertices[vertex].position = markers[record->attachments[i].marker].matrix.position;
			record->pinned[vertex >> 5] |= 1 << (vertex & 31);
		}
	}
}

// @retail 0x117c80
void function_117c80(vector3f const *input, vector3f *output, real angle)
{
	real limit = (real)cos(angle);
	*output = *input;
	if (fabs(input->k) > limit) output->k = input->k < 0.0f ? -limit : limit;
	if (fabs(output->i) < 0.01f && fabs(output->j) < 0.01f || fabs(surface_normalize_horizontal(output)) < 0.0001f)
	{
		output->i = 1.0f; output->j = 0.0f;
	}
	surface_normalize_horizontal(output);
	real horizontal = (real)sqrt(1.0f - output->k*output->k);
	output->i *= horizontal;
	output->j *= horizontal;
}

static inline dword surface_random()
{
	g_4e7408->seed = 1664525 * g_4e7408->seed + 1013904223;
	return g_4e7408->seed >> 16;
}

// @retail 0x117dd0
void function_117dd0(long index, vector3f *wind)
{
	s_surface_definition *definition = surface_definition(surface_record(index));
	real time = (real)(g_510c54->game_time * (double)g_510c54->field_2_3);
	dword random = surface_random();
	double oscillation = cos(time * 0.005f);
	real angle = (real)(random * (double)(1.0f/65535.0f) * oscillation * oscillation * 0.15f);
	if (!g_55e714)
	{
		g_55ecd4 = g_4417f0[(short)((surface_random() * 1026) >> 16)];
		if (!(fabs(function_30bf0(&g_55ecd4)) < 0.0001f)) g_55e714 = true;
	}
	if ((long)g_4ba034 > g_55e710)
	{
		real sine = (real)sin(angle), cosine = (real)cos(angle);
		vector3f axis = g_4417f0[(short)((surface_random() * 1026) >> 16)];
		vector3f old = g_55ecd4;
		real projection = (axis.k*old.k + axis.j*old.j + axis.i*old.i) * (1.0f-cosine);
		g_55ecd4.i = old.i*cosine + axis.i*projection - (axis.k*old.j-axis.j*old.k)*sine;
		g_55ecd4.j = old.j*cosine + axis.j*projection - (axis.i*old.k-axis.k*old.i)*sine;
		g_55ecd4.k = old.k*cosine + axis.k*projection - (axis.j*old.i-axis.i*old.j)*sine;
		function_117c80(&g_55ecd4, &g_55ecd4, 0.34906584f);
		g_55e710 = g_4ba034;
	}
	g_55ecd0 += (real)((cos(time * 0.001) * cos(time * 0.0006f) + sin(time * 0.005)) * cos(time * 0.0001f) * 0.01f * 0.3f);
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
			if (!surface_pinned(record, i))
			{
				s_surface_vertex *vertex = &record->vertices[i];
				point3f position = vertex->position;
				point3f previous = vertex->previous;
				position.x *= current_weight; position.y *= current_weight; position.z *= current_weight;
				previous.x *= previous_weight; previous.y *= previous_weight; previous.z *= previous_weight;
				position.x = position.x - previous.x + acceleration.i;
				position.y = position.y - previous.y + acceleration.j;
				position.z = position.z - previous.z + acceleration.k;
				real dot = vertex->normal.i*force.i + vertex->normal.k*force.k + vertex->normal.j*force.j;
				vector3f tangent = { force.i - vertex->normal.i*dot, force.j - vertex->normal.j*dot, force.k - vertex->normal.k*dot };
				real drag = definition->tangent_drag;
				tangent.i *= drag; tangent.j *= drag; tangent.k *= drag;
				force.i -= tangent.i; force.j -= tangent.j; force.k -= tangent.k;
				vertex->previous = vertex->position;
				position.x += force.i; position.y += force.j; position.z += force.k;
				vertex->position = position;
			}
		}
	}
	else for (long i = 0; i < definition->vertex_count; ++i) record->vertices[i].previous = record->vertices[i].position;
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
byte *g_485a80;
// Render-state storage read by the two external state helpers.
byte g_51f0f0[0x2d8];
// Attribute pairs terminated by 255, as consumed by the retail state helper.
byte g_43f8df[] = {59, 0, 2, 4, 2, 6, 2, 5, 2, 3, 1, 255};

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
	function_1c6b0(g_51f0f0);
	*(byte **)(g_51f0f0 + 0x8c) = g_43f8df;
	if (*(byte **)(g_51f0f0 + 0xcc) != g_43f8df) g_51f0f0[0x20c] = 1;
	function_1c710(g_51f0f0);
	function_1c710(g_51f0f0);
	function_1cf50();
	for (long side = 0; side < 2; ++side)
	{
		real sign = side == 0 ? 1.0f : -1.0f;
		D3DDevice_SetRenderState(D3DRS_CULLMODE, side == 0 ? D3DCULL_CCW : D3DCULL_CW);
		if (tangent != NONE) D3DDevice_SetVertexData4f(tangent, g_4687ac->i*sign, g_4687ac->j*sign, g_4687ac->k*sign, 1.0f);
		if (binormal != NONE) D3DDevice_SetVertexData4f(binormal, g_4687a8->i, g_4687a8->j, g_4687a8->k, 1.0f);
		D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
		for (long i = 0; i < definition->strip_index_count; ++i)
		{
			long vertex_index = definition->strip_indices[i];
			s_surface_vertex *vertex = &record->vertices[vertex_index];
			s_surface_rest_vertex *rest = &definition->vertices[vertex_index];
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
