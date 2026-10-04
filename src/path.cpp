// @flags /O2 /arch:SSE /Gr
#include "cseries.h"
#include "real_math.h"
#include "path.h"
#include "unknown_20fe20.h"

/* Local views of fields not yet named in path.h. */
struct s_path_input_view
{
	byte unknown00[0x10];
	bool start_valid;
	s_node_point start;
	long start_node_index;
	bool attractor_valid;
	real_point3d attractor_point;
	long attractor_object_index;
	real attractor_radius;
	real attractor_weight;
	bool unknown44;
};

struct s_path_destination_view
{
	byte unknown00[0x54];
	bool destination_valid;
	s_node_point destination;
	long destination_node_index;
	real destination_radius;
};

/* The existing path_node view begins at the heap-index field. This view
   begins at the actual node's start and exposes the hash key at +8. */
struct s_path_node_key_view
{
	byte unknown00[8];
	long node_index;
	byte unknown0c[0x44 - 0xc];
};

struct s_path_lookup_view
{
	byte unknown00[0xb0];
	s_path_node_key_view nodes[1024];
	short heap_count;
	path_heap_entry heap[1025];
	short hash_table[4096];
};

struct s_path_location_entry_view
{
	short unknown00;
	short unknown02;
	long node_index;
};

struct s_path_location_view
{
	short flags;
	short count;
	s_path_location_entry_view entries[3];
};

PRIVATE void path_heap_bubble_up(path_state *state, short index);

// @retail 0x270590
void path_input_set_start(s_path_source *source, s_node_point const *point, long node_index)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->start_valid = true;
	input->start = *point;
	input->start_node_index = node_index;
}

// @retail 0x2705c0
void path_input_set_attractor(s_path_source *source, real_point3d const *point,
	real radius, long object_index, real weight, bool unknown)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->attractor_valid = true;
	input->attractor_point = *point;
	input->attractor_object_index = object_index;
	input->attractor_radius = radius;
	input->attractor_weight = weight;
	input->unknown44 = unknown;
}

// @retail 0x270600
PRIVATE bool function_270600(s_path_location const *location, long node_index)
{
	s_path_location_view const *view = (s_path_location_view const *)location;
	for (short i = 0; i < view->count; i++)
	{
		if (view->entries[i].unknown00 == NONE &&
			view->entries[i].node_index == node_index && (view->flags & (1 << i)))
		{
			return true;
		}
	}
	return false;
}

// @retail 0x2713c0
void path_state_destination(path_state *state, s_node_point const *point,
	long node_index, real radius)
{
	s_path_destination_view *destination = (s_path_destination_view *)state;
	destination->destination_valid = true;
	destination->destination = *point;
	destination->destination_node_index = node_index;
	destination->destination_radius = radius;
}

// @retail 0x271fd0
PRIVATE void path_heap_insert(path_state *state, short node_index, short cost)
{
	short index = state->heap_count;
	if (index < 1024)
	{
		state->heap_count = index + 1;
		state->heap[index].node = node_index;
		state->heap[index].cost = cost;
		path_heap_bubble_up(state, index);
	}
}

// @retail 0x272700
short path_node_from_hash_table(path_state *state, long node_index)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	short hash_index = (node_index & 511) * 8;
	short result;
	do
	{
		result = lookup->hash_table[hash_index];
		hash_index = (hash_index + 1) & 4095;
	}
	while (result != NONE && lookup->nodes[result].node_index != node_index);
	return result;
}

// @retail 0x272740
PRIVATE void closest_point_to_attractor(real_point3d const *attractor,
	real_point3d const *start, real_point3d const *end, real_point3d *out)
{
	real_vector3d delta;
	delta.i = end->x - start->x;
	delta.j = end->y - start->y;
	delta.k = end->z - start->z;
	real length_squared = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
	if (length_squared > 0.0f)
	{
		real t = ((start->y - attractor->y) * delta.j +
			(start->z - attractor->z) * delta.k +
			(start->x - attractor->x) * delta.i) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			*out = *end;
		}
		else
		{
			out->x = delta.i * t + start->x;
			out->y = delta.j * t + start->y;
			out->z = delta.k * t + start->z;
		}
	}
	else
	{
		*out = *start;
	}
}
