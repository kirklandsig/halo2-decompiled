// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "path.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "unknown_0259a0.h"
#include <float.h>
#include <string.h>

real function_30bf0(vector3f *v);

bool function_26c4e0(s_type_c3b527 const *start, s_type_c3b527 const *end,
	s_path_trace_result *result, s_pathfinding_data *pathfinding,
	long start_node_index, long end_node_index, long flags);

/* Local views of fields not yet named in path.h. */
struct s_path_input_view
{
	real radius;
	bool unknown04;
	byte unknown05[0xb];
	bool start_valid;
	s_type_c3b527 start;
	long start_node_index;
	bool attractor_valid;
	point3f attractor_point;
	long attractor_object_index;
	real attractor_radius;
	real field_40;
	bool unknown44;
	bool distance_limit_valid;
	byte unknown46[2];
	real distance_limit;
	real link_penalty;
};

struct s_path_destination_view
{
	byte unknown00[0x54];
	bool destination_valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_radius;
};

/* The existing s_type_136112 view begins at the heap-index field. This view
   begins at the actual node's start and exposes the hash key at +8. */
struct s_path_node_key_view
{
	short child;
	short parent;
	long unknown04;
	long node_index;
	bool flag0c;
	bool flag0d;
	bool flag0e;
	byte unknown0f;
	short unknown10;
	short unknown12;
	long unknown14;
	s_type_c3b527 entry_point;
	real entry_distance;
	real attractor_distance;
	real path_distance;
	real cost;
	real estimated_distance;
	short quantized_cost;
	short depth;
	short heap_index;
	short unknown42;
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

struct s_path_link_view
{
	long node_index;
	word flags;
	short unknown06;
	long type;
	long index;
	s_type_c3b527 point;
	vector3f vector;
	bool unknown2c;
	bool unknown2d;
	bool unknown2e;
};

struct s_path_closest_view
{
	byte unknown00[0x90];
	short node_index;
	short unknown92;
	real distance;
	real estimated_distance;
	s_type_c3b527 point;
};

struct s_path_edge_view
{
	word vertices[2];
	byte flags;
	byte unknown05[3];
	word next_edges[2];
	word nodes[2];
};

struct s_path_surface_link_view
{
	short type;
	short next;
	word unknown04;
	word unknown06;
	word unknown08;
	word unknown0a;
	byte unknown0c;
	signed char unknown0d;
	word unknown0e;
	short unknown10;
	short unknown12;
};

struct s_path_links_data_view
{
	long node_count;
	s_pathfinding_node *nodes;
	long edge_count;
	s_path_edge_view *edges;
	byte unknown10[0x2c - 0x10];
	point3f *vertices;
	byte unknown30[8];
	long surface_count;
	s_path_surface_link_view *surfaces;
};

struct s_path_step_view
{
	short type;
	short unknown02;
	long node_index;
	short link_index;
	short unknown0a;
	s_type_c3b527 point;
};

struct s_path_result_view
{
	bool valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_distance;
	s_type_c3b527 start;
	bool complete;
	signed char step_count;
	byte step_index;
	s_path_step_view steps[4];
	long object_index;
	short type;
	bool unknowna6;
	s_path_location_view location;
};

bool function_26f3f0(s_pathfinding_data *pathfinding, long surface_index,
	s_type_c3b527 const *entry, long actor_index, s_type_f17a25 *state,
	s_type_c3b527 const *parent_point, long parent_node_index,
	long *parent_node_index_out, s_type_c3b527 *out, long *out_node_index);
void __stdcall function_2c2060(s_type_f17a25 *state, short count, s_path_step_view const *steps,
	short *out_count, s_path_step_view *out, bool *complete);
bool __stdcall function_2c41b0(long actor_index, s_type_f17a25 *state, short count,
	s_path_step_view const *steps, bool avoid, short *out_count,
	s_path_step_view *out, bool *complete, long *object_index, long *type, bool *flag);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

bool function_26f150(short type, point3f const *start, point3f const *end,
	point3f const *alternate_start, point3f const *alternate_end);

PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state);
PRIVATE bool function_271630(s_type_f17a25 *state);
PRIVATE void function_271ef0(s_type_f17a25 *state, short index);
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out);

PRIVATE void function_271e50(s_type_f17a25 *state, short index);
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost);
short function_272700(s_type_f17a25 *state, long node_index);
PRIVATE void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out);

// @retail 0x270590
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->start_valid = true;
	input->start = *point;
	input->start_node_index = node_index;
}

// @retail 0x2705c0
void function_2705c0(s_path_source *source, point3f const *point,
	real radius, long object_index, real weight, bool unknown)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->attractor_valid = true;
	input->attractor_point = *point;
	input->attractor_object_index = object_index;
	input->attractor_radius = radius;
	input->field_40 = weight;
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

// @retail 0x270640
PRIVATE bool function_270640(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, bool *arg_c793c4, s_type_c3b527 *out)
{
	short index = function_272700(state, node_index);
	if (index == NONE)
	{
		return false;
	}
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_node_key_view *node = &lookup->nodes[index];
	while (node->parent != NONE)
	{
		s_path_node_key_view *parent = &lookup->nodes[node->parent];
		if (point->output_index != parent->entry_point.output_index)
		{
			break;
		}
		s_path_trace_result trace;
		if (function_26c4e0(point, &parent->entry_point, &trace,
			(s_pathfinding_data *)state->pathfinding, node_index, parent->node_index, 0))
		{
			break;
		}
		node = &lookup->nodes[node->parent];
	}
	if (node->parent == NONE)
	{
		*arg_c793c4 = true;
		*out = ((s_path_input_view *)&state->source)->start;
	}
	else
	{
		*arg_c793c4 = false;
		*out = node->entry_point;
	}
	return true;
}

// @retail 0x270750
bool function_270750(byte *buffer, long unknown, s_actor_point_target const *target,
	real *distance, long a, long b)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	real *attractor_distance = (real *)a;
	vector3f *direction = (vector3f *)b;
	short index = function_272700(state, unknown);
	if (index == NONE)
	{
		if (attractor_distance)
		{
			*attractor_distance = FLT_MAX;
		}
		if (direction)
		{
			*direction = *g_4687a4;
		}
		*distance = FLT_MAX;
		return false;
	}
	s_path_node_key_view *node = &lookup->nodes[index];
	real path_distance = function_210970(&node->entry_point, target) + node->path_distance;
	real closest_distance = 0.0f;
	if (input->attractor_valid)
	{
		point3f start_point, end_point, closest;
		function_210850(&node->entry_point, &start_point);
		function_210850(target, &end_point);
		function_272740(&input->attractor_point, &start_point, &end_point, &closest);
		double dx = (double)closest.x - input->attractor_point.x;
		double dy = (double)closest.y - input->attractor_point.y;
		double dz = (double)closest.z - input->attractor_point.z;
		real value = (real)sqrt(dz * dz + dx * dx + dy * dy);
		closest_distance = value > node->attractor_distance ? node->attractor_distance : value;
	}
	if (attractor_distance)
	{
		*attractor_distance = closest_distance;
	}
	*distance = path_distance;
	if (direction)
	{
		short child = NONE;
		short current = index;
		do
		{
			node = &lookup->nodes[current];
			node->child = child;
			child = current;
			current = node->parent;
		}
		while (current != NONE);
		current = child;
		real accumulated = 0.0f;
		while (current != NONE && accumulated < 0.8f)
		{
			node = &lookup->nodes[current];
			current = node->child;
			accumulated += node->entry_distance;
		}
		s_type_c3b527 const *end = current == NONE ? target : &node->entry_point;
		function_210be0(&input->start, end, direction);
		function_30bf0(direction);
	}
	return true;
}

// @retail 0x270d90
PRIVATE short function_270d90(long actor_index, s_type_f17a25 *state,
	s_path_result_view const *result, short node_index, short maximum_steps,
	s_path_step_view *steps, bool *complete_out, short *link_count_out,
	s_path_location_entry_view *links, short maximum_links)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	s_path_links_data_view const *data = (s_path_links_data_view *)state->pathfinding;
	long edge_indices[40];
	long initialize_count = lookup->nodes[node_index].depth + 1;
	if (initialize_count > maximum_steps)
		initialize_count = maximum_steps;
	for (short i = 0; i < initialize_count; i++)
	{
		edge_indices[i] = NONE;
		steps[i].type = NONE;
		steps[i].link_index = NONE;
		steps[i].unknown02 = 0;
	}
	short previous_index = NONE;
	s_path_node_key_view *previous = NULL;
	short count = 0;
	short link_count = 0;
	bool complete = true;
	while (node_index != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[node_index];
		if (link_count < maximum_links && (node->unknown10 != NONE ||
			(node->unknown14 >= 0 && node->unknown14 < data->edge_count &&
			(data->edges[node->unknown14].flags & 0x80))))
		{
			memcpy(&links[link_count++], &node->unknown10, sizeof(*links));
		}
		if (node->depth >= maximum_steps)
		{
			complete = false;
		}
		else
		{
			if (count == 0)
				count = node->depth + 1;
			s_path_step_view *step = &steps[node->depth];
			step->node_index = node->node_index;
			if (previous_index == NONE)
				step->point = result->destination;
			else
			{
				step->point = previous->entry_point;
				if (previous->unknown10 == NONE)
					edge_indices[node->depth] = previous->unknown14;
			}
			bool special = node->unknown10 == 2 &&
				data->surfaces[node->unknown14].unknown0a == 0;
			if (node->unknown10 == 1 || node->unknown10 == 6 ||
				node->unknown10 == 5 || special)
			{
				step[-1].type = node->unknown10;
				step[-1].link_index = (short)node->unknown14;
				s_path_node_key_view *parent = &lookup->nodes[node->parent];
				function_26f3f0((s_pathfinding_data *)state->pathfinding,
					node->unknown14, &node->entry_point, actor_index, state,
					&parent->entry_point, parent->node_index, &parent->node_index,
					&step[-1].point, &step[-1].node_index);
			}
			else if (node->unknown10 == 2)
			{
				step[-1].type = node->unknown10;
				step[-1].link_index = (short)node->unknown14;
			}
		}
		previous_index = node_index;
		previous = node;
		node_index = node->parent;
	}
	real margin = input->radius * 1.2f;
	for (short j = 0; j < count; j++)
	{
		long edge_index = edge_indices[j];
		if (edge_index >= 0 && edge_index < data->edge_count && edge_index != 0xffff)
		{
			s_type_c3b527 const *previous_point = j > 0 ? &steps[j - 1].point : &input->start;
			s_path_edge_view const *edge = &data->edges[edge_index];
			s_type_c3b527 *point = &steps[j].point;
			/* Retail widens these unsigned words before comparing against NONE. */
			if ((long)edge->vertices[0] != NONE && (long)edge->vertices[1] != NONE)
			{
				point3f const *a = &data->vertices[edge->vertices[0]];
				point3f const *b = &data->vertices[edge->vertices[1]];
				vector3f edge_vector = { b->x - a->x, b->y - a->y, b->z - a->z };
				vector3f offset = { previous_point->point.x - a->x,
					previous_point->point.y - a->y, previous_point->point.z - a->z };
				real length = function_30bf0(&edge_vector);
				if (length > margin * 2.0f)
				{
					real projection = offset.k * edge_vector.k + offset.j * edge_vector.j +
						offset.i * edge_vector.i;
					if (margin > projection)
						projection = margin;
					else if (projection > length - margin)
						projection = length - margin;
					point->point.x = a->x + edge_vector.i * projection;
					point->point.y = a->y + edge_vector.j * projection;
					point->point.z = a->z + edge_vector.k * projection;
				}
			}
			vector3f delta;
			if (input->start.output_index == point->output_index)
			{
				delta.i = point->point.x - input->start.point.x;
				delta.j = point->point.y - input->start.point.y;
				delta.k = point->point.z - input->start.point.z;
			}
			else
			{
				point3f start, end;
				if (input->start.output_index == NONE ||
					!function_2104b0(input->start.output_index, &input->start.point, &start))
					start = input->start.point;
				if (point->output_index == NONE ||
					!function_2104b0(point->output_index, &point->point, &end))
					end = point->point;
				delta.i = end.x - start.x;
				delta.j = end.y - start.y;
				delta.k = end.z - start.z;
			}
			if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 1600.0f && j < count - 1)
			{
				count = j + 1;
				complete = false;
			}
		}
	}
	if (complete_out)
		*complete_out = complete;
	if (link_count_out)
		*link_count_out = link_count;
	return count;
}

// @retail 0x270930
bool function_270930(long actor_index, s_type_f17a25 *state, s_path_result_view *result)
{
	byte *actor = actor_index == NONE ? NULL : g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	s_path_destination_view const *destination = (s_path_destination_view *)state;
	s_path_closest_view const *closest = (s_path_closest_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	result->valid = false;
	result->location = *(s_path_location_view *)&state->location;
	result->unknowna6 = false;
	if (!destination->destination_valid)
		return result->valid;
	short index = function_272700(state, destination->destination_node_index);
	if (index != NONE)
	{
		memcpy(&result->destination, &destination->destination, 24);
		result->destination_distance = 0.0f;
	}
	else
	{
		if (!(destination->destination_radius > closest->distance))
			return result->valid;
		index = closest->node_index;
		result->destination = closest->point;
		result->destination_node_index = lookup->nodes[index].node_index;
		result->destination_distance = closest->distance;
	}
	if (index == NONE)
		return result->valid;

	s_path_step_view local_0af630[40], name_b343c4[4], final_steps[4];
	s_path_location_entry_view links[10];
	short smoothed_count = 0, final_count = 0, link_count = 0;
	bool complete = true, success = true;
	short raw_count = function_270d90(actor_index, state, result, index, 40,
		local_0af630, &complete, &link_count, links, 10);
	function_2c2060(state, raw_count, local_0af630, &smoothed_count, name_b343c4, &complete);
	if (actor && actor[0x478])
	{
		final_count = smoothed_count > 4 ? 4 : smoothed_count;
		memcpy(final_steps, name_b343c4, final_count * sizeof(*final_steps));
	}
	else
	{
		bool avoid = state->unknownac != 0;
		long object_index = NONE, type = NONE;
		if (!avoid && result->location.flags > 0)
			avoid = (result->location.flags / (1 << (byte)result->location.count)) % 2 == 1;
		success = function_2c41b0(actor_index, state, smoothed_count, name_b343c4, avoid,
			&final_count, final_steps, &complete, &object_index, &type, &result->unknowna6);
		if (success)
		{
			result->object_index = object_index;
			result->type = (short)type;
			if (object_index != NONE && actor_index != NONE && (short)type == 6)
			{
				real duration = (real)g_510c54->field_2_3 * 0.2f;
				long ticks;
				__asm { fld duration }
				__asm { fistp ticks }
				function_1a8220(actor_index, 9, (short)ticks, 3, 1, 42, 3);
			}
		}
		else
		{
			result->object_index = NONE;
			result->type = NONE;
		}
	}
	if (result->location.flags == 0)
	{
		if (link_count > 0)
			result->location.entries[0] = links[0];
		if (link_count > 1)
			result->location.entries[1] = links[link_count - 1];
		if (link_count > 2)
			result->location.entries[2] = links[link_count / 2];
		result->location.count = link_count < 0 ? 0 : link_count > 3 ? 3 : link_count;
	}
	if (success)
	{
		result->complete = complete;
		result->step_count = (signed char)final_count;
		result->valid = true;
		result->step_index = 0;
		result->start = input->start;
		memcpy(result->steps, final_steps, final_count * sizeof(*final_steps));
		if (result->complete)
		{
			if (result->step_count > 0)
			{
				result->destination = result->steps[result->step_count - 1].point;
				result->destination_node_index = result->steps[result->step_count - 1].node_index;
			}
			else
			{
				result->destination = input->start;
				result->destination_node_index = input->start_node_index;
			}
			result->destination_distance = function_210970(&result->destination, &destination->destination);
		}
	}
	else if (++result->location.flags > 32)
		result->location.flags = 0;
	return result->valid;
}

// @retail 0x2713c0
void function_2713c0(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, real radius)
{
	s_path_destination_view *destination = (s_path_destination_view *)state;
	destination->destination_valid = true;
	destination->destination = *point;
	destination->destination_node_index = node_index;
	destination->destination_radius = radius;
}

// @retail 0x2713f0
PRIVATE bool function_2713f0(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	s_pathfinding_data *pathfinding = (s_pathfinding_data *)state->pathfinding;
	if (!pathfinding)
	{
		return false;
	}
	long node_count = *(long *)pathfinding;
	if (input->start_node_index < 0 || input->start_node_index >= node_count ||
		!(input->start.point.z > -1000.0f))
	{
		return false;
	}
	real distance = 0.0f;
	long quantized = 0;
	if (destination->destination_valid)
	{
		double length = function_210970(&input->start, &destination->destination);
		distance = (real)length;
		quantized = (long)(length * 10.0);
		if (quantized >= 32767)
		{
			return false;
		}
	}
	s_pathfinding_node *sector = &pathfinding->nodes[input->start_node_index];
	short index = state->unknownae++;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_node_key_view *node = &lookup->nodes[index];
	node->parent = NONE;
	node->unknown04 = NONE;
	node->node_index = input->start_node_index;
	node->entry_point = input->start;
	node->entry_distance = 0.0f;
	node->path_distance = 0.0f;
	node->attractor_distance = FLT_MAX;
	node->cost = 0.0f;
	node->estimated_distance = distance;
	node->quantized_cost = (short)quantized;
	node->depth = 0;
	node->flag0c = (sector->flags >> 12) & 1;
	node->flag0d = (sector->flags & 0x3c0) != 0;
	node->flag0e = false;
	node->unknown10 = NONE;
	node->unknown14 = NONE;
	if (destination->destination_valid)
	{
		state->unknown90 = index;
		*(real *)((byte *)state + 0x94) = distance;
		*(real *)((byte *)state + 0x98) = distance;
		*(s_type_c3b527 *)((byte *)state + 0x9c) = input->start;
	}
	lookup->hash_table[(node->node_index & 511) * 8] = index;
	function_271fd0(state, index, (short)quantized);
	return true;
}

// @retail 0x2715a0
bool function_2715a0(byte *buffer)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	state->unknownae = 0;
	state->heap_count = 1;
	memset(lookup->hash_table, 0xff, sizeof(lookup->hash_table));
	closest->node_index = NONE;
	closest->distance = FLT_MAX;
	closest->estimated_distance = FLT_MAX;
	bool result = false;
	if (function_2713f0(state))
	{
		result = function_271630(state);
	}
	if (!result)
	{
		if (state->location.unknown00 == 0)
		{
			state->location.unknown00 = 32;
		}
		else if (++state->location.unknown00 > 32)
		{
			state->location.unknown00 = 0;
		}
	}
	return result;
}

// @retail 0x271630
PRIVATE bool function_271630(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	s_path_location_view *location = (s_path_location_view *)&state->location;
	s_pathfinding_data *pathfinding = (s_pathfinding_data *)state->pathfinding;
	real radius = 0.2f > input->radius ? 0.2f : input->radius;
	real link_penalty = input->link_penalty;
	s_path_link_view links[64];
	while (state->heap_count > 1)
	{
		short index = state->heap[1].node;
		s_path_node_key_view *node = &lookup->nodes[index];
		node->heap_index = NONE;
		if (--state->heap_count > 1)
		{
			state->heap[1] = state->heap[state->heap_count];
			function_271ef0(state, 1);
		}
		if (index == NONE)
		{
			break;
		}
		if (destination->destination_valid)
		{
			if (node->node_index == destination->destination_node_index)
			{
				closest->point = destination->destination;
				closest->node_index = index;
				closest->distance = 0.0f;
				break;
			}
			real limit = 5.0f > closest->distance ? 5.0f : closest->distance;
			if (node->estimated_distance > limit * 10.0f + closest->estimated_distance)
			{
				break;
			}
		}
		short count = function_272020(pathfinding, node, links, state);
		for (short i = 0; i < count; ++i)
		{
			s_path_link_view *link = &links[i];
			if (link->node_index == node->unknown04 && !link->unknown2e && !node->flag0e)
			{
				continue;
			}
			if (!(link->flags & 1) || (node->flag0c && (link->flags & 0x1000)))
			{
				continue;
			}
			bool blocked = false;
			if (location->flags > 0)
			{
				for (short j = 0; j < location->count; ++j)
				{
					if ((location->flags & (1 << j)) &&
						location->entries[j].unknown00 == (short)link->type &&
						location->entries[j].node_index == link->index)
					{
						blocked = true;
						break;
					}
				}
			}
			if (blocked || (!input->unknown04 && (link->flags & 2) &&
				function_1fa6b0(&pathfinding->nodes[link->node_index], pathfinding)))
			{
				continue;
			}
			real length_squared = link->vector.i * link->vector.i +
				link->vector.j * link->vector.j + link->vector.k * link->vector.k;
			real diameter = radius * 2.0f;
			if (link->unknown2c && !link->unknown2d && diameter * diameter > length_squared)
			{
				continue;
			}
			s_type_c3b527 point;
			point.point.x = link->vector.i * 0.5f + link->point.point.x;
			point.point.y = link->vector.j * 0.5f + link->point.point.y;
			point.point.z = link->vector.k * 0.5f + link->point.point.z;
			point.output_index = link->point.output_index;
			if (destination->destination_valid && length_squared > 16.0f &&
				length_squared > diameter * diameter)
			{
				real length = (real)sqrt(length_squared);
				vector3f to_destination;
				function_210be0(&link->point, &destination->destination, &to_destination);
				real t = (link->vector.j * to_destination.j +
					link->vector.k * to_destination.k + link->vector.i * to_destination.i) /
					(link->vector.i * link->vector.i + link->vector.j * link->vector.j +
					link->vector.k * link->vector.k);
				real margin = radius / length;
				if (margin > t)
				{
					t = margin;
				}
				else if (t > 1.0f - margin)
				{
					t = 1.0f - margin;
				}
				point.point.x = link->vector.i * t + link->point.point.x;
				point.point.y = link->vector.j * t + link->point.point.y;
				point.point.z = link->vector.k * t + link->point.point.z;
			}
			double distance = function_210970(&node->entry_point, &point);
			real entry_distance = (real)distance;
			real path_distance = (real)(distance + node->path_distance);
			real attractor_distance;
			real entry_cost;
			if (input->attractor_valid)
			{
				real weight = function_272810(state, &node->entry_point, &point.point, &attractor_distance);
				entry_cost = (weight + 1.0f) * entry_distance;
				attractor_distance = node->attractor_distance > attractor_distance ?
					attractor_distance : node->attractor_distance;
			}
			else
			{
				attractor_distance = 0.0f;
				entry_cost = entry_distance;
			}
			if (link_penalty > 0.0f)
			{
				switch ((short)link->type)
				{
				case 1: case 2: case 5: case 6:
					entry_cost += link_penalty;
					break;
				}
			}
			real cost = node->cost + entry_cost;
			real estimated_distance = cost;
			real destination_distance;
			if (destination->destination_valid)
			{
				double remaining = function_210970(&point, &destination->destination);
				destination_distance = (real)remaining;
				estimated_distance = (real)(remaining + cost);
			}
			long quantized = (long)(estimated_distance * 10.0f);
			if (quantized >= 32767 || (input->distance_limit_valid && path_distance > input->distance_limit))
			{
				continue;
			}
			short hash = (short)((link->node_index & 511) * 8);
			short next = lookup->hash_table[hash];
			while (next != NONE)
			{
				s_path_node_key_view *candidate = &lookup->nodes[next];
				if (candidate->node_index == link->node_index)
				{
					if (!candidate->flag0e && !link->unknown2e)
					{
						break;
					}
					real dx = candidate->entry_point.point.x - link->point.point.x;
					real dy = candidate->entry_point.point.y - link->point.point.y;
					real dz = candidate->entry_point.point.z - link->point.point.z;
					if (dx * dx + dz * dz + dy * dy <= 0.09f)
					{
						break;
					}
				}
				hash = (short)((hash + 1) & 4095);
				next = lookup->hash_table[hash];
			}
			if (next == NONE)
			{
				next = state->unknownae;
				if (next >= 1024)
				{
					continue;
				}
				++state->unknownae;
				lookup->hash_table[hash] = next;
				lookup->nodes[next].heap_index = NONE;
			}
			else if (quantized >= lookup->nodes[next].quantized_cost || lookup->nodes[next].heap_index == NONE)
			{
				continue;
			}
			if (next == NONE)
			{
				continue;
			}
			s_path_node_key_view *child = &lookup->nodes[next];
			child->parent = index;
			child->unknown04 = node->node_index;
			child->node_index = link->node_index;
			child->entry_point = point;
			child->entry_distance = entry_distance;
			child->attractor_distance = attractor_distance;
			child->path_distance = path_distance;
			child->cost = cost;
			child->estimated_distance = estimated_distance;
			child->quantized_cost = (short)quantized;
			child->flag0c = (link->flags >> 12) & 1;
			child->flag0d = (link->flags & 0x3c0) != 0;
			child->flag0e = link->unknown2e;
			*(long *)&child->unknown10 = link->type;
			child->unknown14 = link->index;
			short depth = node->depth;
			switch ((short)link->type)
			{
			case 1: case 5: case 6:
				depth += 2;
				break;
			case 2:
				depth += *(short *)((byte *)&pathfinding->surfaces[link->index] + 0xa) == 0 ? 2 : 1;
				break;
			default:
				++depth;
				break;
			}
			child->depth = depth;
			if (child->heap_index == NONE)
			{
				function_271fd0(state, next, (short)quantized);
			}
			else
			{
				state->heap[child->heap_index].cost = (short)quantized;
				function_271e50(state, child->heap_index);
			}
			if (destination->destination_valid && destination->destination_radius > 0.0f &&
				closest->distance > destination_distance)
			{
				closest->point = child->entry_point;
				closest->node_index = next;
				closest->distance = destination_distance;
				closest->estimated_distance = estimated_distance;
			}
		}
	}
	return !destination->destination_valid || destination->destination_radius >= closest->distance;
}

// @retail 0x271fd0
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost)
{
	short index = state->heap_count;
	if (index < 1024)
	{
		state->heap_count = index + 1;
		state->heap[index].node = node_index;
		state->heap[index].cost = cost;
		function_271e50(state, index);
	}
}

/* Surface records interpret their payload differently by type. Retail's
   vertex indices are unsigned words: its comparisons with long NONE do not
   reject 0xffff. Only destination sector words have an effective NONE test.
   Link type stores write the low short; the upper short stays untouched. */
// @retail 0x272020
PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state)
{
	s_path_links_data_view const *data = (s_path_links_data_view const *)pathfinding;
	long node_index = node->node_index;
	short output_index = node->entry_point.output_index;
	short count = 0;
	short surface_index = data->nodes[node_index].first_surface;
	if (data->surface_count > 0)
	{
		while (surface_index != NONE && count < 64)
		{
			s_path_surface_link_view const *surface = &data->surfaces[surface_index];
			if (state->settings.flags & (1 << surface->type))
			{
				s_path_link_view *link = &links[count];
				switch (surface->type)
				{
				case 0:
				{
					long edge_index = *(long const *)&surface->unknown04;
					s_path_edge_view const *edge = &data->edges[edge_index];
					word next_node = edge->nodes[node_index != edge->nodes[1]];
					if (next_node != (word)NONE)
					{
						link->node_index = next_node;
						link->index = edge_index;
						*(short *)&link->type = NONE;
						link->flags = data->nodes[next_node].flags;
						link->point.point = data->vertices[edge->vertices[0]];
						link->point.output_index = output_index;
						vector3d_from_points3d(&data->vertices[edge->vertices[0]],
							&data->vertices[edge->vertices[1]], &link->vector);
						link->unknown2c = (edge->flags >> 7) & 1;
						link->unknown2d = true;
						link->unknown2e = false;
						++count;
					}
					break;
				}
				case 1:
				case 6:
				{
					if (!state->settings.unknown0c &&
						(state->settings.unknown08 & surface->unknown0d) <= 0)
					{
						break;
					}
					word next_node = surface->unknown0e;
					if (next_node == (word)NONE)
					{
						break;
					}
					word flags = data->nodes[next_node].flags;
					if (node->flag0d)
					{
						vector3f delta;
						vector3d_from_points3d(&data->vertices[surface->unknown04],
							&node->entry_point.point, &delta);
						if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 0.09f)
						{
							break;
						}
					}
					if (!(surface->unknown0c & 1) && surface->unknown10 != surface->unknown12)
					{
						point3f start, end;
						function_2104b0(surface->unknown10, &data->vertices[surface->unknown04], &start);
						function_2104b0(surface->unknown12, &data->vertices[surface->unknown08], &end);
						bool traversable = false;
						for (short type = 0; type < 6; ++type)
						{
							long bit = 1 << type;
							if ((state->settings.unknown08 & bit) && function_26f150(type, &start, &end, NULL, NULL))
							{
								traversable = true;
								break;
							}
							if (bit & surface->unknown0d)
							{
								break;
							}
						}
						if (!traversable)
						{
							break;
						}
					}
					*(short *)&link->type = surface->type;
					link->index = surface_index;
					link->node_index = next_node;
					link->flags = flags | 1;
					link->unknown2c = true;
					link->unknown2d = true;
					link->point.point = data->vertices[surface->unknown08];
					link->point.output_index = surface->unknown12;
					link->unknown2e = surface->type == 6;
					vector3d_from_points3d(&data->vertices[surface->unknown08],
						&data->vertices[surface->unknown0a], &link->vector);
					++count;
					break;
				}
				case 2:
				{
					word next_node = surface->unknown04;
					if (next_node == (word)NONE)
					{
						break;
					}
					point3f const *point = &data->vertices[surface->unknown06];
					if (surface->unknown0a == 1 || surface->unknown0a == 2)
					{
						real dx = node->entry_point.point.x - point->x;
						real dy = node->entry_point.point.y - point->y;
						if (dy * dy + dx * dx > 0.09f)
						{
							break;
						}
					}
					link->index = surface_index;
					*(short *)&link->type = 2;
					link->node_index = next_node;
					link->flags = data->nodes[next_node].flags | 1;
					link->point.point = *point;
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = surface->unknown0a == 3;
					++count;
					break;
				}
				case 5:
				{
					word next_node = surface->unknown04;
					if (next_node == (word)NONE)
					{
						break;
					}
					long flags = state->settings.unknown04;
					if (!(((flags & 0x400) && (surface->unknown0c & 1)) ||
						((flags & 0x800) && (surface->unknown0c & 2)) ||
						((flags & 0x1000) && (surface->unknown0c & 4))))
					{
						break;
					}
					link->index = surface_index;
					*(short *)&link->type = 5;
					link->node_index = next_node;
					link->flags = data->nodes[next_node].flags;
					link->point.point = data->vertices[surface->unknown08];
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = false;
					++count;
					break;
				}
				}
			}
			surface_index = surface->next;
		}
	}
	/* Retail enters the ring even if the surface pass already filled 64 slots.
	   Its count check follows insertion; keep that behavior here. */
	if ((data->nodes[node_index].flags & 1) || (count == 0 && node->parent == NONE))
	{
		long first_edge = *(long const *)data->nodes[node_index].unknown4;
		long edge_index = first_edge;
		do
		{
			s_path_edge_view const *edge = &data->edges[edge_index];
			bool reverse = node_index == edge->nodes[1];
			word next_node = edge->nodes[!reverse];
			if (next_node != (word)NONE)
			{
				s_path_link_view *link = &links[count];
				link->index = edge_index;
				link->node_index = next_node;
				*(short *)&link->type = NONE;
				link->flags = data->nodes[next_node].flags;
				link->unknown2c = (edge->flags >> 7) & 1;
				link->unknown2d = false;
				link->unknown2e = false;
				link->point.point = data->vertices[edge->vertices[0]];
				link->point.output_index = output_index;
				vector3d_from_points3d(&data->vertices[edge->vertices[0]],
					&data->vertices[edge->vertices[1]], &link->vector);
				++count;
			}
			if (count == 64)
			{
				break;
			}
			edge_index = edge->next_edges[reverse];
		}
		while (edge_index != first_edge);
	}
	return count;
}

// @retail 0x272700
short function_272700(s_type_f17a25 *state, long node_index)
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
PRIVATE void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out)
{
	vector3f delta;
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

// @retail 0x272810
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out)
{
	s_path_input_view const *input = (s_path_input_view const *)&state->source;
	real distance = FLT_MAX;
	real weight = 0.0f;
	point3f local_end;
	point3f local_attractor;
	function_210690(start->output_index, end, &local_end);
	function_210690(start->output_index, &input->attractor_point, &local_attractor);
	vector3f delta;
	vector3d_from_points3d(&start->point, &local_end, &delta);
	real length_squared = delta.k * delta.k + delta.i * delta.i + delta.j * delta.j;
	point3f closest;
	if (length_squared > 0.0f)
	{
		real t = ((start->point.z - local_attractor.z) * delta.k +
			(start->point.y - local_attractor.y) * delta.j +
			(start->point.x - local_attractor.x) * delta.i) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			closest = local_end;
		}
		else
		{
			closest.x = delta.i * t + start->point.x;
			closest.y = delta.j * t + start->point.y;
			closest.z = delta.k * t + start->point.z;
		}
	}
	else
	{
		closest = start->point;
	}
	vector3d_from_points3d(&local_attractor, &closest, &delta);
	real distance_squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
	if (input->attractor_radius * input->attractor_radius > distance_squared)
	{
		double length = sqrt(distance_squared);
		distance = (real)length;
		weight = (real)((1.0 - length / input->attractor_radius) * input->field_40);
	}
	*distance_out = distance;
	return weight;
}
