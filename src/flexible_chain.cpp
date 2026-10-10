// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "object_queries.h"
#include <math.h>

// Layouts follow the pool and tag accesses in the retail callback group.
struct s_chain_node
{
	point3f position;
	vector3f velocity;
	real texture_scale;
	short ticks;
	short unknown1e;
};

struct s_chain_record
{
	short salt;
	short unknown02;
	byte unknown04;
	bool fixed;
	short idle_ticks;
	long tag_index;
	long object_index;
	point3f origin;
	s_chain_node nodes[21];
};

struct s_chain_segment
{
	real stiffness;
	byte unknown04[0x20];
	real length;
	short sequence;
	byte unknown2a[0x4a];
	vector3f rest_offset;
};

struct s_chain_definition
{
	long marker_name;
	long unknown04;
	long bitmap_index;
	long unknown0c;
	long physics_index;
	byte unknown14[0x50];
	real stiffness;
	byte unknown68[0x30];
	long segment_count;
	s_chain_segment *segments;
};

struct s_chain_frame
{
	short bitmap;
	byte unknown02[6];
	real left;
	real right;
};

struct s_chain_sequence
{
	byte unknown00[0x34];
	long frame_count;
	s_chain_frame *frames;
};

struct s_chain_bitmap_view
{
	byte unknown00[0x38];
	short border;
	short unknown3a;
	long sequence_count;
	s_chain_sequence *sequences;
};

s_record_pool *g_4e0334;
struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
void function_11bed0(s_location *location, point3f const *point);
real function_11ce20(vector3f const *a, vector3f const *b);
void function_211060(long unknown0, void *physics, s_location *location, long unknown3, point3f *position, long unknown5, long unknown6, long unknown7, real radius, real dt, vector3f *velocity);
typedef void (__stdcall *chain_draw_callback)(long, long);
void function_47870(long object_index, long chain_index, point3f const *origin, chain_draw_callback draw);

void __stdcall function_1161f0(s_chain_record *record, s_chain_definition *definition, real dt);
void function_116750(s_chain_record *record, s_chain_definition *definition, s_location *location, point3f *origin, vector3f *direction);

// @retail 0x115d10
void __stdcall function_115d10()
{
	g_4e0334 = data_new_inlined("antenna", 12, sizeof(s_chain_record), 0, g_510c2c);
}

// @retail 0x115d50
void __stdcall function_115d50()
{
	g_4e0334->valid = 1;
	record_pool_release_all(g_4e0334);
}

// @retail 0x115d70
void __stdcall function_115d70()
{
	g_4e0334->valid = 0;
}

// @retail 0x115d80
void __stdcall function_115d80()
{
	if (g_4e0334)
		g_4e0334 = 0;
}

// @retail 0x115da0
long __stdcall function_115da0(long tag_index, long object_index)
{
	long result = NONE;
	if (tag_index != NONE && object_index != NONE)
	{
		s_chain_definition *definition = (s_chain_definition *)g_4e3b44[tag_index & 0xffff].bytes;
		result = record_pool_allocate(g_4e0334);
		if (result != NONE)
		{
			s_chain_record *record = (s_chain_record *)(g_4e0334->data + (result & 0xffff) * sizeof(s_chain_record));
			record->unknown04 = 0;
			record->fixed = definition->segment_count < 2;
			point3f *origin = &record->origin;
			origin->x = origin->y = origin->z = 0.f;
			point3f position = *origin;
			record->tag_index = tag_index;
			record->object_index = object_index;
			record->idle_ticks = 0;
			short i;
			for (i = 0; i < definition->segment_count; ++i)
			{
				s_chain_node *node = &record->nodes[i];
				s_chain_segment *segment = &definition->segments[i];
				node->position = position;
				node->velocity.i = node->velocity.j = node->velocity.k = 0.f;
				node->ticks = 0;
				node->texture_scale = 0.f;
				if (definition->bitmap_index != NONE)
				{
					s_chain_bitmap_view *bitmap = (s_chain_bitmap_view *)g_4e3b44[definition->bitmap_index & 0xffff].bytes;
					if (segment->sequence >= 0 && segment->sequence < bitmap->sequence_count)
					{
						s_chain_sequence *sequence = &bitmap->sequences[segment->sequence];
						if (sequence->frame_count)
						{
							s_chain_frame *frame = sequence->frames;
							s_type_7ba8e9 *image = function_137550(definition->bitmap_index, frame->bitmap);
							if (image)
								node->texture_scale = segment->length / ((frame->right - frame->left) * *(short *)((byte *)image + 4) - bitmap->border * 2.f - 1.f);
						}
					}
				}
				position.x = segment->rest_offset.i + position.x;
				position.y = segment->rest_offset.j + position.y;
				position.z = segment->rest_offset.k + position.z;
			}
			s_chain_node *terminal = &record->nodes[i];
			terminal->position = position;
			terminal->velocity.i = terminal->velocity.j = terminal->velocity.k = 0.f;
		}
	}
	return result;
}

// @retail 0x115ff0
void __stdcall function_115ff0(long index)
{
	record_pool_release(g_4e0334, index);
}

// @retail 0x116010
void __stdcall function_116010(long unused, long index)
{
	s_chain_record *record = (s_chain_record *)(g_4e0334->data + (index & 0xffff) * sizeof(s_chain_record));
	s_chain_definition *definition = (s_chain_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
	if (!record->fixed)
	{
		if (record->idle_ticks > 5)
		{
			function_1161f0(record, definition, .05f);
			function_1161f0(record, definition, .05f);
			function_1161f0(record, definition, .05f);
		}
		record->idle_ticks = 0;
	}
}

// Retail submits the empty ret-8 body at 0x24dc50. The same body is already
// represented by a virtual method elsewhere; this ordinary callback needs
// the two-argument function-pointer form used by the submission queue.
PRIVATE void __stdcall chain_draw_empty(long, long)
{
}

// @retail 0x116080
void __stdcall function_116080(short mode, long index)
{
	if (mode == 0)
	{
		s_chain_record *record = (s_chain_record *)(g_4e0334->data + (index & 0xffff) * sizeof(s_chain_record));
		function_47870(record->object_index, index, &record->origin, chain_draw_empty);
	}
}

// @retail 0x1160c0
void __stdcall function_1160c0(real dt)
{
	s_record_pool *pool = g_4e0334;
	long index = data_datum_index(pool, function_16bc00(pool, 0));
	while (index != NONE)
	{
		long slot = index & 0xffff;
		s_chain_record *record = (s_chain_record *)(pool->data + slot * sizeof(s_chain_record));
		s_chain_definition *definition = (s_chain_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
		if (!record->fixed)
		{
			++record->idle_ticks;
			if (record->object_index != NONE && record->idle_ticks < 5)
			{
				function_1161f0(record, definition, dt > .06666667f ? .06666667f : dt);
				pool = g_4e0334;
			}
		}
		index = data_datum_index(pool, data_find_index(pool, index == NONE ? 0 : slot + 1));
	}
}

// @retail 0x1161f0
void __stdcall function_1161f0(s_chain_record *record, s_chain_definition *definition, real dt)
{
	point3f origin;
	vector3f forward;
	s_location location;
	function_116750(record, definition, &location, &origin, &forward);
	if (!record->fixed && dt > 0.f)
	{
		point3f previous, predicted;
		for (short i = 0; i < definition->segment_count + 1; ++i)
		{
			real inverse_dt = 1.f / dt;
			s_chain_node *node = &record->nodes[i];
			s_chain_segment *segment = &definition->segments[i == definition->segment_count ? definition->segment_count - 1 : i];
			real stiffness = definition->stiffness * segment->stiffness;
			++node->ticks;
			point3f position;
			vector3f direction;
			if (i == 0)
			{
				direction = forward;
				position = origin;
			}
			else
			{
				position = node->position;
				if (definition->physics_index != NONE)
					function_211060(0, g_4e3b44[definition->physics_index & 0xffff].bytes, &location, NONE, &position, 0, 0, 0, .02f, dt, &node->velocity);
				vector3f delta = { position.x - previous.x, position.y - previous.y, position.z - previous.z };
				real scale = (real)(segment->length / sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i));
				point3f target = { scale * delta.i + previous.x, scale * delta.j + previous.y, scale * delta.k + previous.z };
				position.x = (1.f - stiffness) * target.x + predicted.x * stiffness;
				position.y = (1.f - stiffness) * target.y + predicted.y * stiffness;
				position.z = (1.f - stiffness) * target.z + predicted.z * stiffness;
				direction.i = position.x - previous.x;
				direction.j = position.y - previous.y;
				direction.k = position.z - previous.z;
			}
			vector3f axis = { direction.k * 0.f - direction.j, direction.i - direction.k * 0.f, direction.j * 0.f - direction.i * 0.f };
			real length = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);
			if (.0001f > fabs(length))
				length = 0.f;
			else
			{
				real inverse = 1.f / length;
				axis.i = inverse * axis.i;
				axis.j = axis.j * inverse;
				axis.k = axis.k * inverse;
			}
			if (length == 0.f)
				axis = *g_4687ac;
			vector3f offset = segment->rest_offset;
			vector3f vertical = { 0.f, 0.f, 1.f };
			real angle = function_11ce20(&direction, &vertical);
			real sine = (real)sin(angle);
			real cosine = (real)cos(angle);
			real along = (axis.k * offset.k + axis.j * offset.j + axis.i * offset.i) * (1.f - cosine);
			predicted.x = (along * axis.i + cosine * offset.i) - (axis.k * offset.j - axis.j * offset.k) * sine + position.x;
			predicted.y = (along * axis.j + cosine * offset.j) - (axis.i * offset.k - axis.k * offset.i) * sine + position.y;
			predicted.z = (axis.k * along + cosine * offset.k) - (axis.j * offset.i - axis.i * offset.j) * sine + position.z;
			node->velocity.i = (position.x - node->position.x) * inverse_dt;
			node->velocity.j = (position.y - node->position.y) * inverse_dt;
			node->velocity.k = (position.z - node->position.z) * inverse_dt;
			node->position = position;
			previous = position;
		}
	}
}

// @retail 0x116750
void function_116750(s_chain_record *record, s_chain_definition *definition, s_location *location, point3f *origin, vector3f *direction)
{
	s_object_marker marker;
	function_b8d30(record->object_index, definition->marker_name, &marker, 1, false);
	*origin = marker.matrix.position;
	*direction = marker.matrix.forward;
	function_11bed0(location, &marker.matrix.position);
	vector3f delta = { origin->x - record->origin.x, origin->y - record->origin.y, origin->z - record->origin.z };
	if (fabs(delta.i) > 1.f || fabs(delta.j) > 1.f || fabs(delta.k) > 1.f)
	{
		for (short i = 0; i < definition->segment_count + 1; ++i)
		{
			s_chain_node *node = &record->nodes[i];
			node->position.x += delta.i;
			node->position.y += delta.j;
			node->position.z += delta.k;
		}
	}
	record->origin = *origin;
}

// Nine populated slots and three null slots in the retail callback table.
void *g_4674a0[12] =
{
	(void *)function_115d10, (void *)function_115d50,
	(void *)function_115d70, (void *)function_115d80,
	(void *)function_115da0, (void *)function_115ff0,
	(void *)function_1160c0, (void *)function_116010,
	0, 0, 0, (void *)function_116080
};
