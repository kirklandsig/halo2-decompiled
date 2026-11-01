// @flags /O2 /Ob1 /arch:SSE /Gr
#include "orientations_storage.h"
/* UNKNOWN_1DACB0.CPP: the animation graph tag's lookups (0x1dacb0..0x1ddea0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "animation_cache.h"
#include "unknown_0259d0.h"
#include "unknown_11cb00.h"

// @retail 0x1dafc0
s_graph_tag *function_1dafc0(s_graph_tag *graph, long graph_index)
{
	return graph_inherited_get(graph, graph_index);
}

/* requests the resource of an animation (inlined copies; the out-of-line one
   is function_1dd840) */
inline void graph_animation_request(s_graph_tag *graph, long animation_index)
{
	s_animation *animation = graph_animation_get(graph, animation_index);

	if (animation->internal_flags & 0x10)
	{
		s_cache_resource *resource = &graph->resources[animation->resource_index];

		if (resource->streamed)
		{
			animation_cache_request_resource(resource, true);
		}
	}
}

// @retail 0x1dd840
void function_1dd840(s_graph_tag *graph, long animation_index)
{
	graph_animation_request(graph, animation_index);
}

// @retail 0x1dd9d0
void function_1dd9d0(s_graph_tag *graph, c_type_709360 animation_id)
{
	if (g_510c20 && g_510c21 && animation_id.index != NONE)
	{
		s_graph_tag *arg_0e6cbc = graph;
		s_animation *animation;

		if (animation_id.graph_index != NONE)
		{
			arg_0e6cbc = function_1dafc0(graph, animation_id.graph_index);
		}
		animation = graph_animation_get(arg_0e6cbc, animation_id.index);
		if (animation->parent_animation != NONE)
		{
			animation = graph_animation_get(arg_0e6cbc, animation->parent_animation);
		}
		function_1dd840(arg_0e6cbc, animation_id.index);
		while (animation->next_animation != NONE)
		{
			graph_animation_request(arg_0e6cbc, animation->next_animation);
			animation = graph_animation_get(arg_0e6cbc, animation->next_animation);
		}
	}
}

// @retail 0x1daea0
s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id)
{
	s_animation *animation = NULL;

	if (animation_id.index != NONE)
	{
		if (animation_id.graph_index == NONE)
		{
			animation = graph_animation_get(graph, animation_id.index);
		}
		else
		{
			animation = graph_animation_get(graph_inherited_get(graph, animation_id.graph_index), animation_id.index);
		}
		if (animation)
		{
			function_1dd9d0(graph, animation_id);
		}
	}
	return animation;
}

// @retail 0x1daff0
s_graph_inheritance *function_1daff0(s_graph_tag *graph, c_type_709360 animation_id)
{
	s_graph_inheritance *result = NULL;

	if (animation_id.index != NONE && animation_id.graph_index >= 0 && animation_id.graph_index < graph->inheritance_count)
	{
		s_graph_inheritance *inheritance;

		function_1dd9d0(graph, animation_id);
		inheritance = &graph->inheritance[animation_id.graph_index];
		if (inheritance->graph_tag_index != NONE && inheritance->node_map_flag_count > 0 && inheritance->node_map_count > 0 &&
			inheritance->node_map_flags && inheritance->node_map)
		{
			result = inheritance;
		}
		else if (inheritance->graph_tag_index != NONE)
		{
			function_1daea0(graph, animation_id);
		}
	}
	return result;
}

// @retail 0x1dadb0
short function_1dadb0(s_animation const *animation, long type)
{
	long i;

	for (i = 0; i < animation->event_count; i++)
	{
		s_animation_event const *event = &animation->events[i];

		if (event->type == type)
		{
			return event->frame;
		}
	}
	return NONE;
}

// @retail 0x1dade0
short function_1dade0(s_animation const *animation, long type, long frame)
{
	long i;

	for (i = 0; i < animation->event_count; i++)
	{
		s_animation_event const *event = &animation->events[i];

		if (event->type == type && event->frame > frame)
		{
			return event->frame;
		}
	}
	return NONE;
}

// @retail 0x1dae20
short function_1dae20(s_animation const *animation)
{
	return animation_event_frame_get(animation, 0);
}

// @retail 0x1dae50
short function_1dae50(s_animation const *animation)
{
	return animation_event_frame_get(animation, 1);
}

// @retail 0x1dae80
long function_1dae80(s_animation const *animation)
{
	long result = NONE;

	if (animation->sound_event_count > 0)
	{
		result = animation->sound_events[0].sound;
	}
	return result;
}

// @retail 0x1daf30
real *function_1daf30(s_graph_tag *graph, c_type_709360 animation_id)
{
	real *result = NULL;

	if (animation_id.index != NONE)
	{
		s_animation *animation;

		function_1dd9d0(graph, animation_id);
		if (animation_id.graph_index != NONE)
		{
			graph = graph_inherited_get(graph, animation_id.graph_index);
		}
		animation = graph_animation_get(graph, animation_id.index);
		if (animation->blend_screen != NONE)
		{
			result = &graph->blend_screens[animation->blend_screen].right_yaw_per_frame;
		}
	}
	return result;
}

// @retail 0x1db120
void *function_1db120(s_graph_tag *graph, long mode, long weapon_class, long weapon_type)
{
	return graph_weapon_type_get(graph, mode, weapon_class, weapon_type);
}

// @retail 0x1dd490
long function_1dd490(s_graph_tag *graph, long flags)
{
	long i;

	for (i = 0; i < graph->node_count; i++)
	{
		if ((graph->nodes[i].flags & flags) == flags)
		{
			return i;
		}
	}
	return NONE;
}

/* a node of a render model (0x60 bytes) */
struct s_render_model_node
{
	long name;
	byte unknown04[0x5c];
};

struct s_render_model_nodes
{
	byte unknown00[0x48];
	long node_count;
	s_render_model_node *nodes;
};

// @retail 0x1dd4c0
bool function_1dd4c0(long render_model_tag_index, s_graph_tag *graph, long *node_count, long *node_map)
{
	s_render_model_nodes *render_model = (s_render_model_nodes *)g_4e3b44[render_model_tag_index & 0xffff].bytes;
	bool result = true;
	long i;

	*node_count = render_model->node_count;
	for (i = 0; i < render_model->node_count; i++)
	{
		s_render_model_node *node = &render_model->nodes[i];
		long node_index = NONE;
		long j;

		for (j = 0; j < graph->node_count; j++)
		{
			if (node->name == graph->nodes[j].name)
			{
				node_index = j;
				break;
			}
		}
		node_map[i] = node_index;
		if (node_index == NONE)
		{
			result = false;
		}
	}
	return result;
}

// @retail 0x1dd5d0
c_type_709360 *function_1dd5d0(s_graph_tag *graph, c_type_709360 animation_id, c_type_709360 *output)
{
	c_type_709360 result = animation_id;

	if (animation_id.index != NONE)
	{
		s_animation *animation = function_1daea0(graph, animation_id);

		if (animation->parent_animation != NONE || animation->next_animation != NONE)
		{
			short parent_index = function_1daea0(graph, animation_id)->parent_animation;

			if (parent_index != NONE)
			{
				result.index = parent_index;
			}
		}
	}
	*output = result;
	return output;
}

PRIVATE __forceinline real function_1dd632(bool arg_0, s_random_globals *arg_1)
{
	return arg_0 ? function_x82e52f(&arg_1->unknown0, NULL, 0) : function_x82e52f(&arg_1->seed, NULL, 0);
}

// @retail 0x1dd630
c_type_709360 *function_1dd630(s_graph_tag *graph, c_type_709360 *result, c_type_709360 animation_id, bool first_seed)
{
	if (animation_id.index != NONE)
	{
		s_animation *animation = function_1daea0(graph, animation_id);

		if (animation->parent_animation != NONE || animation->next_animation != NONE)
		{
			s_graph_tag *arg_0e6cbc = graph;

			if (animation_id.graph_index != NONE)
			{
				arg_0e6cbc = function_1dafc0(graph, animation_id.graph_index);
			}
			long temporary;
			animation_id = *function_1dd5d0(graph, animation_id, (c_type_709360 *)&temporary);
			animation = graph_animation_get(arg_0e6cbc, animation_id.index);
			if (1.0f > animation->weight)
			{
				real random = function_1dd632(first_seed, g_4e7408);

				while (animation->next_animation != NONE)
				{
					real weight = animation->weight;
					short next_index;

					if (weight >= random)
					{
						break;
					}
					next_index = animation->next_animation;

					animation_id.index = next_index;
					animation = graph_animation_get(arg_0e6cbc, next_index);
				}
			}
		}
		if (animation_id.index != NONE)
		{
			function_1dd9d0(graph, animation_id);
		}
	}
	*result = animation_id;
	return result;
}

// @retail 0x1dd7a0
void function_1dd7a0(long *reference)
{
	reference[0] = NONE;
	reference[1] = NONE;
	((short *)reference)[4] = NONE;
	((short *)reference)[5] = 0;
	reference[3] = NONE;
	reference[4] = NONE;
}

// @retail 0x1dd7c0
byte *function_1dd7c0(s_graph_tag *graph, c_type_709360 animation_id)
{
	s_animation *animation;

	if (animation_id.graph_index != NONE)
	{
		graph = graph_inherited_get(graph, animation_id.graph_index);
	}
	animation = graph_animation_get(graph, animation_id.index);
	if (animation->internal_flags & 0x10)
	{
		s_cache_resource *resource = &graph->resources[animation->resource_index];

		return (byte *)animation_cache_get_resource(resource, animation->name) + animation->resource_offset;
	}
	return animation->data;
}

// @retail 0x1dd880
void function_1dd880(s_graph_tag *graph, c_type_709360 animation_id, s_graph_tag **arg_0e6cbc, s_animation **animation)
{
	*arg_0e6cbc = NULL;
	*animation = NULL;
	if (animation_id.graph_index == NONE)
	{
		*arg_0e6cbc = graph;
	}
	else
	{
		*arg_0e6cbc = graph_inherited_get(graph, animation_id.graph_index);
	}
	*animation = graph_animation_get(*arg_0e6cbc, animation_id.index);
}

// @retail 0x1ddab0
void function_1ddab0(s_graph_tag *graph)
{
	long i;

	for (i = 0; i < graph->resource_count; i++)
	{
		s_cache_resource *resource = &graph->resources[i];

		if (resource->streamed)
		{
			animation_cache_request_resource(resource, false);
		}
	}
}

// @retail 0x1ddaf0
void function_1ddaf0(s_graph_tag *graph)
{
	long i;

	function_1ddab0(graph);
	for (i = 0; i < graph->inheritance_count; i++)
	{
		function_1ddab0(graph_inherited_get(graph, i));
	}
}

PRIVATE __forceinline void animation_data_fields_set(s_animation_data *output, byte *bytes,
	s_animation_data_sizes *sizes, long node_count, long frame_info_type, long frame_count)
{
	output->data = bytes;
	output->sizes = sizes;
	output->node_count = (byte)node_count;
	output->frame_info_type = (char)frame_info_type;
	output->frame_count = (short)frame_count;
}

// @retail 0x1ddb40
void function_1ddb40(s_animation_data *data, s_graph_tag *graph, c_type_709360 animation_id)
{
	if (animation_id.index != NONE)
	{
		s_animation *animation = function_1daea0(graph, animation_id);

		byte *bytes = function_1dd7c0(graph, animation_id);

		animation_data_fields_set(data, bytes, &animation->sizes, animation->node_count,
			animation->frame_info_type, animation->frame_count);
	}
}

/* the body of 0x1ddb90, which retail inlines into the graph's lookups through
   0x1ddd00 and 0x1ddc70 (graph_resources_request below) */
PRIVATE __forceinline void graph_type_resources_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type,
	bool urgent, bool other)
{
	if (urgent || other)
	{
		s_graph_weapon_type *animations = (s_graph_weapon_type *)graph_weapon_type_get(graph, mode, weapon_class, weapon_type);

		if (animations)
		{
			long i;

			if (urgent)
			{
				for (i = 0; i < animations->urgent_resource_count; i++)
				{
					s_cache_resource *resource = &graph->resources[animations->urgent_resources[i]];

					if (resource->streamed)
					{
						animation_cache_request_resource(resource, true);
					}
				}
			}
			if (other)
			{
				for (i = 0; i < animations->resource_count; i++)
				{
					s_cache_resource *resource = &graph->resources[animations->resources[i]];

					if (resource->streamed)
					{
						animation_cache_request_resource(resource, false);
					}
				}
			}
		}
	}
}

// @retail 0x1ddb90
void function_1ddb90(s_graph_tag *graph, long mode, long weapon_class, long weapon_type, bool urgent, bool other)
{
	graph_type_resources_request(graph, mode, weapon_class, weapon_type, urgent, other);
}

// @retail 0x1ddc70
void function_1ddc70(s_graph_tag *graph, long mode, long weapon_class, long weapon_type, bool urgent, bool other)
{
	if (urgent || other)
	{
		function_1ddb90(graph, mode, weapon_class, weapon_type, urgent, other);
		function_1ddb90(graph, mode, weapon_class, 0x30000d9, urgent, other);
		function_1ddb90(graph, mode, 0x30000d9, weapon_type, urgent, other);
		function_1ddb90(graph, mode, 0x30000d9, 0x30000d9, urgent, other);
		function_1ddb90(graph, 0x30000d9, 0x30000d9, 0x30000d9, urgent, other);
	}
}

// @retail 0x1ddd00
void function_1ddd00(s_graph_tag *graph, long mode, long weapon_class, long weapon_type, bool urgent, bool other)
{
	if (urgent || other)
	{
		long i;

		function_1ddc70(graph, mode, weapon_class, weapon_type, urgent, other);
		for (i = 0; i < graph->inheritance_count; i++)
		{
			function_1ddc70(graph_inherited_get(graph, i), mode, weapon_class, weapon_type, urgent, other);
		}
	}
}

/* 0x1ddc70 and 0x1ddd00 as retail inlines them into the graph's lookups */
PRIVATE __forceinline void graph_kinds_resources_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type,
	bool urgent, bool other)
{
	if (urgent || other)
	{
		graph_type_resources_request(graph, mode, weapon_class, weapon_type, urgent, other);
		graph_type_resources_request(graph, mode, weapon_class, 0x30000d9, urgent, other);
		graph_type_resources_request(graph, mode, 0x30000d9, weapon_type, urgent, other);
		graph_type_resources_request(graph, mode, 0x30000d9, 0x30000d9, urgent, other);
		graph_type_resources_request(graph, 0x30000d9, 0x30000d9, 0x30000d9, urgent, other);
	}
}

PRIVATE __forceinline void graph_resources_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type,
	bool urgent, bool other)
{
	if (urgent || other)
	{
		long i;

		graph_kinds_resources_request(graph, mode, weapon_class, weapon_type, urgent, other);
		for (i = 0; i < graph->inheritance_count; i++)
		{
			graph_kinds_resources_request(graph_inherited_get(graph, i), mode, weapon_class, weapon_type, urgent, other);
		}
	}
}

// @retail 0x1dacb0
void function_1dacb0(s_graph_tag *graph, c_type_709360 animation_id, real *event_distance, real *distance)
{
	real total = 0.0f;
	real total_at_event = 0.0f;
	s_animation *animation = function_1daea0(graph, animation_id);
	s_animation_data data;
	s_animation_data_sizes *sizes;

	function_1ddb40(&data, graph, animation_id);
	sizes = data.sizes;
	if (sizes->movement_data_size != 0)
	{
		real *movement = (real *)(data.data + sizes->static_node_flags_size + sizes->animated_node_flags_size +
			sizes->static_data_size + sizes->animated_data_size);
		short event_frame = animation_event_frame_get(animation, 0);
		short frame;

		for (frame = 0; frame < animation->frame_count; frame++)
		{
			switch (animation->frame_info_type)
			{
			case 1:
				total += *movement++;
				break;
			case 2:
				total += *movement++;
				break;
			case 3:
				total += *movement++;
				break;
			}
			if (frame == event_frame)
			{
				total_at_event = total;
			}
		}
	}
	if (distance)
	{
		*distance = total;
	}
	if (event_distance)
	{
		*event_distance = total_at_event;
	}
}

// @retail 0x1db070
c_type_709360 s_graph_weapon_type::variant_find(long name, char a, char b, long c, long d, char e, char f, char g)
{
	c_type_709360 animation_id;
	long i;

	for (i = 0; i < variant_group_count; i++)
	{
		s_graph_variant_group *reference = &variant_groups[i];

		if (reference->name == name && reference->unknown0a == a && reference->unknown0b == b)
		{
			long j;

			for (j = 0; j < reference->variant_count; j++)
			{
				s_graph_variant *variant = &reference->variants[j];

				if (variant->unknown04 == c && variant->unknown08 == d && variant->unknown0e == e &&
					variant->unknown0f == f && variant->unknown0c == g)
				{
					return variant->animation_id;
				}
			}
		}
	}
	return animation_id;
}

// @retail 0x1dceb0
bool function_1dceb0(s_graph_iterator3c *iterator, s_graph_tag *graph)
{
	short index = iterator->next_index + 1;

	if (index < graph->unknown3c_count)
	{
		s_graph_element3c *element;

		iterator->next_index = index;
		element = &graph->unknown3c[index];
		iterator->animation_id = element->animation_id;
		iterator->unknown10 = element->unknown14;
		iterator->unknown0c = element->unknown10;
		iterator->unknown00 = element->unknown08;
		iterator->unknown08 = element->unknown0c;
		iterator->index = index;
		iterator->unknown04 = element->unknown18;
		iterator->unknown1c = element->unknown24;
		iterator->unknown18 = element->unknown20;
		iterator->unknown14 = element->unknown1c;
		if (iterator->animation_id.index != NONE)
		{
			function_1dd9d0(graph, iterator->animation_id);
		}
		return true;
	}
	return false;
}

/* the animation of the given name in the graph or the graphs it inherits
   from, the graph first */
// @retail 0x1dd0b0
c_type_709360 s_graph_tag::animation_by_name(long name)
{
	c_type_709360 animation_id;
	s_graph_tag *current = this;
	long graph_index = NONE;

	if (this)
	{

		do
		{
			long index;

			if (animation_id.index != NONE)
			{
				break;
			}
			for (index = 0; index < current->animation_count; index++)
			{
				s_animation *animation = NULL;

				if (index != NONE)
				{
					animation = &current->animations[index];
				}
				if (animation->name == *(volatile long const *)&name)
				{
					animation_id.graph_index = (short)graph_index;
					animation_id.index = (short)index;
					break;
				}
			}
			if (animation_id.index == NONE)
			{
				graph_index++;
				if (graph_index >= inheritance_count)
				{
					break;
				}
				current = graph_inherited_get(this, graph_index);
			}
		}
		while (current);
		if (animation_id.index != NONE)
		{
			function_1dd9d0(this, animation_id);
		}
	}
	return animation_id;
}

/* an orientation (as unknown_141590.cpp declares it) */


void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

/* the node matrices of the graph's skeleton from the nodes' orientations,
   the root node's relative to the given matrix */
// @retail 0x1dd1c0
void function_1dd1c0(s_graph_tag *graph, transform4x3f *matrices, rigid_transform_scaled const *orientations, transform4x3f const *root)
{
	long node_indices[255];
	transform4x3f matrix;
	long count;
	long i = 0;

	if (graph->node_count > 0)
	{
		count = 1;
		node_indices[0] = 0;
		do
		{
			long node_index = node_indices[i++];
			s_graph_node *node = &graph->nodes[node_index];
			transform4x3f const *parent;

			if (node_index == 0)
			{
				parent = root;
			}
			else
			{
				parent = &matrices[node->parent_index];
			}
			function_1421f0(&matrix, &orientations[node_index]);
			function_142a60(parent, &matrix, &matrices[node_index]);
			if (node->next_sibling_index != NONE)
			{
				node_indices[count++] = node->next_sibling_index;
			}
			if (node->first_child_index != NONE)
			{
				node_indices[count++] = node->first_child_index;
			}
		}
		while (i != count);
	}
}

/* the identity transform */
real_quaternion_transform *g_4687d8;

PRIVATE __forceinline void first_frame_rotation_unpack(s_animation_first_frame const *a, real_quaternion_transform *transform)
{
	__asm
	{
		mov ecx, a
		mov eax, transform
		movq mm3, [ecx]
		punpcklwd mm1, mm3
		punpckhwd mm2, mm3
		psrad mm1, 0x10
		psrad mm2, 0x10
		cvtpi2ps xmm1, mm1
		cvtpi2ps xmm2, mm2
		emms
		movlhps xmm1, xmm2
		movaps xmm0, xmm1
		mulps xmm0, xmm1
		movaps xmm3, xmm0
		shufps xmm3, xmm3, 0x4e
		addps xmm0, xmm3
		movaps xmm4, xmm0
		shufps xmm4, xmm4, 0x11
		addps xmm0, xmm4
		rsqrtps xmm0, xmm0
		mulps xmm1, xmm0
		movaps [eax], xmm1
	}
}

// @retail 0x1dd8f0
bool function_1dd8f0(s_graph_tag *graph, c_type_709360 animation_id, real_quaternion_transform *transform)
{
	bool result = false;
	s_graph_tag *arg_0e6cbc;
	s_animation *animation;

	function_1dd880(graph, animation_id, &arg_0e6cbc, &animation);
	if (animation->first_frame_index != NONE)
	{
		s_animation_first_frame *a = &arg_0e6cbc->first_frames[animation->first_frame_index];

		first_frame_rotation_unpack(a, transform);
		transform->position = a->position;
		transform->scale = a->scale;
		function_1dd9d0(graph, animation_id);
		result = true;
	}
	else
	{
		*transform = *g_4687d8;
	}
	return result;
}

// @retail 0x1dcf20
bool function_1dcf20(s_graph_tag *graph, s_graph_pair_iterator *iterator)
{
	bool local_1 = false;
	long index = iterator->index + 1;

	for (;;)
	{
		s_graph_mode_entry *entry = NULL;

		while (iterator->step < 2 && !entry)
		{
			short local_0 = *(volatile short const *)&iterator->step;
			switch (local_0)
			{
			case 0:
				entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, iterator->mode, 0x14);
				break;
			case 1:
				entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, 0x30000d9, 0x14);
				break;
			}
			if (!entry)
			{
				iterator->step = local_0 + 1;
				index = 0;
			}
		}
		if (!entry)
		{
			break;
		}
		if (index >= entry->pair_count)
		{
			iterator->step++;
			continue;
		}
		{
			s_graph_pair *pair = &entry->pairs[index];

			iterator->a = pair->a;
			iterator->b = pair->b;
			iterator->index = (short)index;
			local_1 = true;
			break;
		}
	}
	return local_1;
}

// @retail 0x1dcfa0
bool function_1dcfa0(s_graph_tag *graph, s_graph_pair_iterator *iterator)
{
	long index = iterator->index + 1;

	for (;;)
	{
		s_graph_mode_entry *entry = NULL;

		while (iterator->step < 4 && !entry)
		{
			/* the weapon class lookup is written out in each case: retail rereads
			   the weapon class after the first lookup */
			short local_0 = iterator->step;
			s_graph_mode_entry *mode_entry;
			switch (local_0)
			{
			case 0:
			{
				s_graph_mode_entry *result = NULL;

				mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, iterator->mode, 0x14);
				if (mode_entry)
				{
					result = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&mode_entry->child_count, iterator->weapon_class, 0x14);
				}
				entry = result;
				break;
			}
			case 1:
			{
				s_graph_mode_entry *result = NULL;

				mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, iterator->mode, 0x14);
				if (mode_entry)
				{
					result = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&mode_entry->child_count, 0x30000d9, 0x14);
				}
				entry = result;
				break;
			}
			case 2:
			{
				s_graph_mode_entry *result = NULL;

				mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, 0x30000d9, 0x14);
				if (mode_entry)
				{
					result = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&mode_entry->child_count, iterator->weapon_class, 0x14);
				}
				entry = result;
				break;
			}
			case 3:
			{
				s_graph_mode_entry *result = NULL;

				mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, 0x30000d9, 0x14);
				if (mode_entry)
				{
					result = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&mode_entry->child_count, 0x30000d9, 0x14);
				}
				entry = result;
				break;
			}
			}
			if (!entry)
			{
				iterator->step = local_0 + 1;
				index = 0;
			}
		}
		if (entry)
		{
			if (index < entry->pair_count)
			{
				s_graph_pair *pair = &entry->pairs[index];

				iterator->a = pair->a;
				iterator->b = pair->b;
				iterator->index = (short)index;
				return true;
			}
			iterator->step++;
		}
		else
		{
			break;
		}
	}
	return false;
}

static inline void cross_product3d_1dd290(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

// @retail 0x1dd290
void function_1dd290(s_graph_tag *graph, transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root, short mirrored_node_index, short mirror_parent_index)
{
	long node_indices[255];
	transform4x3f matrix;
	long count;
	long i = 0;

	if (graph->node_count > 0)
	{
		count = 1;
		node_indices[0] = 0;
		do
		{
			long node_index = node_indices[i++];
			s_graph_node *node = &graph->nodes[node_index];
			transform4x3f const *parent;

			if (node_index == 0)
			{
				parent = root;
			}
			else
			{
				parent = &matrices[node->parent_index];
			}
			function_1421f0(&matrix, &orientations[node_index]);
			if (mirrored_node_index == node_index)
			{
				parent = &matrices[mirror_parent_index];
				vector3f forward = matrix.forward;
				vector3f up = matrix.up;
				point3f position = matrix.position;

				position.y = 0.0f - position.y;
				forward.j = 0.0f - forward.j;
				matrix.scale = 1.0f;
				up.j = 0.0f - up.j;
				vector3f const *mirrored_forward = &forward;
				matrix.forward = forward;
				cross_product3d_1dd290(&up, mirrored_forward, &matrix.left);
				matrix.up = up;
				matrix.position = position;
			}
			function_142a60(parent, &matrix, &matrices[node_index]);
			if (node->next_sibling_index != NONE)
			{
				node_indices[count++] = node->next_sibling_index;
			}
			if (node->first_child_index != NONE)
			{
				node_indices[count++] = node->first_child_index;
			}
		}
		while (i != count);
	}
}

/* the animation at (item, index) of a weapon type's set, or none */
static inline c_type_709360 animation_find_in_type_1dc790(s_graph_weapon_type *weapon_type_entry, long set,
	long item_index, long animation_index)
{
	c_type_709360 animation_id;
	s_graph_set_entry *set_entry = (s_graph_set_entry *)function_1dd560(
		(s_sorted_array *)&weapon_type_entry->set_count, set, 0xc);

	if (set_entry)
	{
		animation_id = set_entry->items[item_index].animation_ids[animation_index];
	}
	return animation_id;
}

// @retail 0x1dc790
c_type_709360 s_graph_tag::animation_find(long mode, long weapon_class, long weapon_type, long set, long item_index,
	long animation_index, long *found_mode, long *found_weapon_class, long *found_weapon_type)
{
	c_type_709360 result;

	if (weapon_class != NONE)
	{
		s_graph_weapon_type_iterator iterator;
		long iterator_mode = NONE;
		long iterator_weapon_class = NONE;
		long iterator_weapon_type = NONE;
		s_graph_weapon_type *weapon_type_entry;

		iterator.graph = this;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type);
		bool found = false;
		while (!found && weapon_type_entry)
		{
			result = animation_find_in_type_1dc790(weapon_type_entry, set, item_index, animation_index);
			if (result.index != NONE)
			{
				found = true;
			}
			else
			{
				weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
					&iterator_weapon_type);
			}
		}
		if (found_mode)
		{
			*found_mode = iterator_mode;
		}
		if (found_weapon_class)
		{
			*found_weapon_class = iterator_weapon_class;
		}
		if (found_weapon_type)
		{
			*found_weapon_type = iterator_weapon_type;
		}
		if (result.index != NONE)
		{
			function_1dd9d0(this, result);
		}
	}
	return result;
}

// score 828 (was 828): Frame 0x2c agrees; class/NONE, result/output/candidate homes and later zero carrier remain.
// result/output homes and initial ECX/EDX differ;
// frame 0x2c agrees; class/NONE registers, result/output/candidate homes and later zero carrier remain.
// original selectors via iterator 1016; failed-result exit 1012; zero
// iterator 996; two-pass requests 2187; inline depth two 828 (fast counts).
// Mode initialized only by iterator 828; retained native selected index 838; library candidate copy 828;
// shared graph receiver 828; requested-class snapshot 828. Frame 0x2c agrees; homes remain.
// class/NONE registers, result/output/candidate homes and later zero carrier differ.
// Earlier 828 (was 828): class/NONE registers, result/output/candidate homes and later zero carrier differ.
// Named-entry latch 828; direct result with entry latch 1000; iterator mode/class keys 1016;
// iterator mode/type keys 837; entry latch with native initialization 826 (two-point gain rejected);
// combined continuation while 828.
// frame 0x2c agrees; result/output homes and initial ECX/EDX remain. Existing complete
// helper for original request 2287; resolved request 2208; both 3530; immutable requested mode/type 828/828.
// Shared iterator and helper bodies unchanged.
// Frame 0x2c agrees; class/NONE and result/output/candidate homes plus later zero
// carrier remain. Candidate before iterator 967; post-search result copy 998; earlier candidate/native
// iterator 965; post-search copy with saved mode 998; with saved type 998 (fast).
// Frame 0x2c agrees; result/output/candidate homes remain. Entry iterator after result 1004; missing-name continue 991; returned-object library copy 828; entry iterator before result 993; shared success predicate 992 (fast).
// Native iterator with mode default omitted 826; that form with named-entry latch 826;
// with candidate field copies 920; with retained requested result 994; with result-driven while 826.
// frame 0x2c agrees; class/NONE, result/output homes and later zero remain.
// Tight iterator scope 828; entry output declarations 828; combined scopes 828; native-long
// selected index 990; all original keys retained jointly 828 (fast).
// Requested-mode output initialization 833; returned constructor expression 828; condition-declared named lookup 828; retained class/type key pair 828; mode initialization plus native iterator 831 (fast).
// Earlier 828 (was 828): Frame 0x2c agrees; class/NONE and result/output/candidate homes plus later zero
// Round 112: Iterator, animation-request and cache-request inline declarations 828 each;
// iterator/request and iterator/cache pairs 828 each; chained output defaults 828; chained native
// null entries 828.
// Round 349: Explicit returned copy 828; reverse output defaults 828; shorter candidate scope 828; scope plus return copy 828; defaults plus return copy 828 (fast).
// Per-attempt constructor expression 828; native candidate reset 980; candidate reset/latch 980; paired native
// transfers 988; result constructed after outputs 1004. Native reset aligns initial result home at +0x14, but
// iterator-output/candidate homes and later zero carrier remain.
// Pre-lookup native default conditional 951; class key first 828; type key first 978; index-first
// native transfers 846; sorted lookup noinline 828; type/class/mode output defaults 828.
// Result/output/candidate homes and later zero carrier remain; original body/declaration restored.
// Round 357: Output defaults before class guard 979; traversal-only defaults before
// guard 1007; both 1005; selector-only setup before result 832; selector/output setup 833.
// Entry-owned optional mode/class/type output pointers, all three pairs and all
// three jointly 828 each; immutable joint pointers 828. All aliases restored.
// Loop-lived native candidate reset with guaranteed-mode default omitted, condition-scoped
// lookup, shortened traversal phase, their combinations and traversal-first defaults: 932 each.
// Return object after the class guard, with native iterator/post-lookup candidate/guaranteed
// mode-write combinations: 988/1001/999/1001/989/1000/999/1001 (2825 bytes, first +0).
// Original constructor timing, iterator, candidate, output order and resource requests restored.
// Partial traversal-zero subsets before requested keys (step/mode-entry/both/class-entry/
// step-class/both-entry) 828/827/828/828/828/827; interleaved key/zero stores 828/828
// (fast control 828). Small gains rejected; original initialization and body restored.
// Round 267: Complete named-identifier producer, native value/caller output, default before
// lookup/after lookup/on failure, ordinary/forced expansion 828/828/932/932/996/996/998/998.
// Original candidate, iterator, resource requests and all helpers restored.
// Round 392: Actual iterator output receivers in mode/class/type input slots, all three pairs and all,
// then all with native iterator initialization: 982/1026/982/1023/1001/1023/1001/1024. Original parameter and output lifetimes restored.
// Round 394: Native resolved-name output record, member/aggregate construction, guarded/entry
// declaration and direct/record set key: 837/984/837/964/993/993/993/999. Grouped forms require a match; all restored.
// Round 176: Entry candidate constructed before returned object, with complete/native-word reset,
// member/native iterator and retained/producer-written mode default: 932/986/932/986/932/893/932/895.
// All reach 2825 bytes; native reset fixes initial class/NONE roles and result +0x14 but adds entry
// candidate stores and leaves traversal/candidate homes. Original constructors and complete body restored.
// Round 268: One retained selection-success or selection-absence test, bool/byte/short/long, shared by
// loop and requests: 1023/1023/998/1000/991/991/999/999. Original body restored.
// Round 417: existing native resource hierarchy expanded through one/two/three levels,
// original/resolved/both requests: 2101/2157/3335, 1989/1988/2978, 828/828/828. Full
// expansion ties but changes matched helper controls; all declarations and body restored.
// Unsigned class key/set key/both/all four input-key domains 828 each; definitions
// and shared declarations restored.
// Round 425: Iterator graph field at every post-search graph use, with all eight
// requested mode/class/type field subsets, 969/964/1008/1024/969/964/1008/1024.
// Original graph/key owners, constructors, iterator outputs and requests restored.
// Round 197: Immutable native candidate with both member transfers and native-iterator/
// readonly named-source/readonly iterator-entry masks 0..7 all 997 at 2825 bytes.
// The matched channel-refresh caller also differs; original complete body restored.
// DL204: last native named-record selection, guarded/conditional complete result,
// mutable/readonly source and member/aggregate iterator 990/1005/990/1005/990/1005/990/1005. Restored.
// Round 459: Resolved-output defaults after corresponding key stores,
// mode/class/both/type/mode-type/class-type/all 828/828/828/828/828/828/828; all after traversal defaults
// 826. Original initialization restored. All executable trial forms restored.
// Round 262: Ordinary inline metadata on existing private resource levels: outer alone 3524
// (310/2825); middle/inner and all remaining pairs/triple 828 each; all with forced existing
// animation-resource expansion 828. All provider metadata, target body and request tuples restored.
// Round 53 (m3 lane 1): 736 of the 810 differing lines are one inserted instruction per resource
// request: retail holds a zero in EBP from the fifth type request (all keys 0x30000d9) through the
// inheritance loop's index/offset stores (`cmp eax, ebp`, `mov [home], ebp`), ours tests with
// `test eax, eax` and stores immediates, so all later lines shift. In the first 100 lines retail gives
// the dead found-mode argument home to the loop candidate (then the request pointer; index and offset
// take the found-class/found-type homes); ours gives it to the iterator mode output. Candidate renamed
// 828 (same code); `while ((entry = iterate()) != NULL) {...; if (found) break;}` 998 (rotated,
// 2839 bytes); `while (true)` with both breaks 998; the three outputs as one record (grouping
// diagnostic) 823 (candidate, request pointer, index and offset homes then retail's; result sits below
// the record); candidate declared beside the outputs and reset each pass 932 (frame 0x30); result as
// `named ? named->animation_id : c_type_709360()` 978.
// Round 68: the found_mode slot goes to iterator_mode (the found-class slot when found_mode is unused, 987);
// retail gives it to the per-pass candidate, as a grouped mode/class/type record does (823, grouping form).
// Renamed candidate/mode/result (four forms) 828 each. While loop with break on result/candidate/guarded
// copy 998/998/999 (iterate call duplicated at the bottom); one result write per pass 993 (2825 bytes);
// candidate declared after the lookup 996; candidate before the iterate call 967 (as listed).
// Round 234: Downstream request eligibility as the actual urgent argument, bool/byte/short/long after
// outputs 996/995/1008/1009; before outputs 996/995/1009/1010. Original search and request guards
// restored.
// @retail 0x1db170
c_type_709360 s_graph_tag::animation_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
	long *found_weapon_class, long *found_weapon_type)
{
	c_type_709360 result;

	if (weapon_class == NONE)
		return result;
	{
		s_graph_weapon_type_iterator iterator;
		long iterator_mode = NONE;
		long iterator_weapon_class = NONE;
		long iterator_weapon_type = NONE;
		s_graph_weapon_type *weapon_type_entry;

		iterator.graph = this;
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		do
		{
			weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
				&iterator_weapon_type);
			if (!weapon_type_entry)
				break;
			c_type_709360 animation_id;
			s_graph_named_animation *named = (s_graph_named_animation *)function_1dd560(
				(s_sorted_array *)&weapon_type_entry->named_animation_count, set, sizeof(s_graph_named_animation));

			if (named)
			{
				animation_id = named->animation_id;
			}
			result = animation_id;
		} while (result.index == NONE);
		if (found_mode)
		{
			*found_mode = iterator_mode;
		}
		if (found_weapon_class)
		{
			*found_weapon_class = iterator_weapon_class;
		}
		if (found_weapon_type)
		{
			*found_weapon_type = iterator_weapon_type;
		}
		if (result.index != NONE)
		{
			function_1dd9d0(this, result);
			graph_resources_request(this, mode, weapon_class, weapon_type, true, false);
			graph_resources_request(this, iterator_mode, iterator_weapon_class, iterator_weapon_type, true, false);
		}
	}
	return result;
}

// score 828 (was 828): Frame 0x2c agrees; class/NONE, result/output/candidate homes and later zero carrier remain.
// result/output homes and initial ECX/EDX differ;
// original mode/class/type via iterator 837/991/837; mode assigned only
// by iterator 828; graph pointer initialized last 828 (fast counts).
// Retained native selected index 838; library candidate copy 828; shared graph receiver 828;
// requested-class snapshot 828; immutable class/type keys together 993. Frame 0x2c agrees; homes remain.
// class/NONE registers, result/output/candidate homes and later zero carrier differ.
// Named-entry latch 828; direct result with entry latch 1000; iterator mode/class keys 1016;
// iterator mode/type keys 837; combined continuation while 828.
// frame 0x2c agrees; result/output homes and initial ECX/EDX remain. Existing complete
// helper for original request 2287; resolved request 2208; both 3530; immutable requested mode/type 828/828.
// Shared iterator and helper bodies unchanged.
// Frame 0x2c agrees; class/NONE and result/output/candidate homes plus later zero
// carrier remain. Candidate before iterator 967; post-search result copy 998; earlier candidate/native
// iterator 965; post-search copy with saved mode 998; with saved type 998 (fast).
// Frame 0x2c agrees; result/output/candidate homes remain. Entry iterator after result 1004; missing-name continue 991; returned-object library copy 828; entry iterator before result 993; shared success predicate 992 (fast).
// Loop-scoped returned entry 828; entry resolved outputs after result 979; all requested keys via iterator 1016;
// native candidate field copies 922 at retail length; exhausted-result exit 1012 (fast).
// Round 349: Explicit returned copy 828; reverse output defaults 828; shorter candidate scope 828; scope plus return copy 828; defaults plus return copy 828 (fast).
// Per-attempt constructor expression 828; native candidate reset 980; candidate reset/latch 980; paired native
// transfers 988; result constructed after outputs 1004. Native reset aligns initial result home at +0x14, but
// iterator-output/candidate homes and later zero carrier remain.
// Pre-lookup native default conditional 951; class key first 828; type key first 978; index-first
// native transfers 846; type/class/mode output defaults 828; sorted lookup noinline 828.
// Result/output/candidate homes and later zero carrier remain; original body/declaration restored.
// Entry-owned optional mode/class/type output pointers, all three pairs and all
// three jointly 828 each; immutable joint pointers 828. All aliases restored.
// Optional-output formal permutations 828/824/824/829/829; native iterator on three formal-home contexts 826/822/827.
// Closest six-point gain 822 remains above 30 and fails the eight-point floor; original formals and iterator restored.
// Round 225: ordered original/resolved request tuples and native records, mutable/const
// with long/dword ordinals, 2174 each (1657/2825 bytes). All eight table forms restored;
// preserve the original two resource calls, native iterator and identifier workspaces.
// Round 243: Immutable complete candidate, readonly named source, both, readonly iterator entry,
// candidate/entry, both readonly sources, all three and one final result exit:
// 978/828/978/828/978/828/978/1011. Complete original body and declarations restored.
// Round 228: flat class rejection to the final return, with member/native iterator
// and original/early iterator/output-default lifetimes 1011/1009/1016/1016/1011/1011/1016/1016. All restored.
// Round 267: Complete named-identifier producer, native value/caller output, default before
// lookup/after lookup/on failure, ordinary/forced expansion 828/828/932/932/996/996/998/998.
// Original candidate, iterator, resource requests and all helpers restored.
// Round 392: Actual iterator output receivers in mode/class/type input slots, all three pairs and all,
// then all with native iterator initialization: 982/1026/982/1023/1001/1023/1001/1024. Original parameter and output lifetimes restored.
// Round 394: Native resolved-name output record, member/aggregate construction, guarded/entry
// declaration and direct/record set key: 837/984/837/964/993/993/993/999. Grouped forms require a match; all restored.
// Round 176: Independently tested entry candidate before returned object, complete/native-word
// resets, member/native iterator and retained/producer-written mode defaults:
// 932/986/932/986/932/893/932/895 at 2825 bytes. Initial result home/class roles improve with native
// reset, but entry candidate stores and traversal homes remain. Original constructors and body restored.
// Round 268: One retained selection-success or selection-absence test, bool/byte/short/long, shared by
// loop and requests: 1023/1023/998/1000/991/991/999/999. Original body restored.
// Round 417: existing native resource hierarchy expanded through one/two/three levels,
// original/resolved/both requests: 2101/2157/3335, 1989/1988/2978, 828/828/828. Full
// expansion ties but changes matched helper controls; unsigned class key/set key/both/
// all four input-key domains 828 each. All declarations and executable source restored.
// Round 191: Persistent-result admission 828; with immutable candidate 978, late candidate 996,
// paired native transfers 988, iterator-owned request keys 1016, native iterator 826, direct result
// 978, retained lookup name 828. Two-point gain rejected; original body restored.
// DL199: Member stdcall, fastcall and inline with member/native iterator give 828/826 each; cdecl
// gives 839/839 and loses twelve checked matches. Two-point reductions rejected. Inline trials
// recover all controls. Return/candidate/output homes and late zero carrier remain; original body
// and declaration restored.
// T47: Full native iterator memset before key publication, with requested-key ownership
// subsets 0..7: 996/999/1021/1042/999/999/1023/1042. Native clear reaches retail
// length in some forms but does not recover the homes; original complete body restored.
// Round 459: Resolved-output defaults after corresponding key stores,
// mode/class/both/type/mode-type/class-type/all 828/828/828/828/828/828/828; all after traversal defaults
// 826. Original initialization restored. All executable trial forms restored.
// Round 471: Separate selected graph/index short/short, short/long, long/short, long/long,
// materialized before/after outputs 1024/1002/1017/1000/1018/1001/1014/1002.
// All selected-component carriers and original constructor lifetimes restored.
// Round 53 (m3 lane 1; zero-carrier/argument-home analysis in the note above 0x1db170): `for (;;)`
// with both breaks 998; early `return c_type_709360();` with result scoped in the block 988 (2825
// bytes); the sibling 0x1dc8c0's iterator store order (graph, step, entries, keys) 828 (stores
// reorder only); the body inside `if (weapon_class != NONE)` 1040; the iterator declared after its
// outputs 828 (same code).
// Round 68: class test as an enclosing `if (weapon_class != NONE)` block 1040 (2857 bytes: retail's three
// exit blocks and result kept in ECX to the 0x1dd9d0 push, exact LCS 113 unmatched against 136, but the
// first request's loop heads take the alignment retail gives their joins); with a while/break loop 1049
// (LCS 106); with the candidate after the lookup 1031; outputs declared after the iterator setup 826, with
// the enclosing if 1038; entry pointer declared in the loop body 828.
// Round 234: Downstream request eligibility as the actual urgent argument, bool/byte/short/long after
// outputs 996/995/1008/1009; before outputs 996/995/1009/1010. Original search and request guards
// restored.
// @retail 0x1dbc80
c_type_709360 s_graph_tag::overlay_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
	long *found_weapon_class, long *found_weapon_type)
{
	c_type_709360 result;

	if (weapon_class == NONE)
		return result;
	{
		s_graph_weapon_type_iterator iterator;
		long iterator_mode = NONE;
		long iterator_weapon_class = NONE;
		long iterator_weapon_type = NONE;
		s_graph_weapon_type *weapon_type_entry;

		iterator.graph = this;
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		do
		{
			weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
				&iterator_weapon_type);
			if (!weapon_type_entry)
				break;
			c_type_709360 animation_id;
			s_graph_named_animation *named = (s_graph_named_animation *)function_1dd560(
				(s_sorted_array *)&weapon_type_entry->overlay_count, set, sizeof(s_graph_named_animation));

			if (named)
			{
				animation_id = named->animation_id;
			}
			result = animation_id;
		} while (result.index == NONE);
		if (found_mode)
		{
			*found_mode = iterator_mode;
		}
		if (found_weapon_class)
		{
			*found_weapon_class = iterator_weapon_class;
		}
		if (found_weapon_type)
		{
			*found_weapon_type = iterator_weapon_type;
		}
		if (result.index != NONE)
		{
			function_1dd9d0(this, result);
			graph_resources_request(this, mode, weapon_class, weapon_type, true, false);
			graph_resources_request(this, iterator_mode, iterator_weapon_class, iterator_weapon_type, true, false);
		}
	}
	return result;
}

// @retail 0x1dc8c0
c_type_709360 s_graph_tag::transition_find(long mode, long weapon_class, long weapon_type, long name, char a, char b,
	long c, long d, char e, char f, char g)
{
	c_type_709360 result;

	if (weapon_class != NONE)
	{
		s_graph_weapon_type_iterator iterator;
		long iterator_mode = NONE;
		long iterator_weapon_class = NONE;
		long iterator_weapon_type = NONE;
		s_graph_weapon_type *weapon_type_entry;
		/* retail keeps mode on the stack, as if its address were taken */
		long const *mode_reference = &mode;

		iterator.graph = this;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		iterator.mode = *mode_reference;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type);
		bool found = false;
		while (!found && weapon_type_entry)
		{
			result = weapon_type_entry->variant_find(name, a, b, c, d, e, f, g);
			if (result.index != NONE)
			{
				function_1dd9d0(this, result);
				graph_resources_request(this, c, iterator_weapon_class, iterator_weapon_type, true, false);
				found = true;
			}
			else
			{
				weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
					&iterator_weapon_type);
			}
		}
	}
	return result;
}
