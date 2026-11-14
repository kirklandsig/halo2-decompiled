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

// MiniMax H (11-11): no new form, no lower score (828); the six declaration orders of the mode, class and type out-locals all compile to the same code, so their frame homes are not set by the declaration order, and the first difference is still retail's compare of the weapon class against NONE with the operands the other way round.
// MiniMax G (11-12): the three resolved outputs now share one array whose entries are set after the class check, which moves the -1 stores past the compare and puts the per-attempt candidate in the found_mode argument home where retail has it; the class load still lands in ECX where retail uses EDX, the result sits at frame+0x00 where retail has frame+0x0c, and the resolved outputs sit at frame+0x04 where retail has frame+0x00.
// score 828 (was 828; m3 lane 8 r164 at the end): Frame 0x2c agrees; class/NONE, result/output/candidate homes and later zero carrier remain.
// NOT kept (coordinator review, MiniMax G 11-13): 828 -> 807 came from grouping the three resolved outputs into one
// `long resolved[3]` array (an aggregate grouping locals: kept only with a byte match); the three named locals stay.
// MiniMax S (11-06): Measured from the frame base, retail keeps iterator_weapon_type at frame+0x04,
// iterator_weapon_class at frame+0x00 and iterator_mode at frame+0x08, while ours keeps iterator_weapon_type
// at frame+0x00, iterator_mode at frame+0x04 and iterator_weapon_class in the reused argument slot; our
// result shorts therefore land at frame+0x08 instead of frame+0x0c and the tail compares out of step.
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
// Round 43 (lane 14, fast, both bodies): single exit as the matched 0x1dc790 1040 (2857 bytes); 0x1dc790's
// iterator field order 828 (code changes); both 1040; iterator outputs declared after the iterator initialisation
// 826 (two-point gain rejected); that with 0x1dc790's field order 828. Retail gives found_mode's dead parameter home
// to the per-attempt candidate (ours to iterator_mode) and the no-result path its own epilogue.
// Round 489: chained candidate/result assignment in the named branch, native iterator, late constant
// candidate and missing name branch first, combinations 978/976/984/982/991/989/984/982.
// All reach 2825 bytes but regress; original complete producer, iterator and body restored.
// Round 211: native resolved-key array in six fixed mode/class/type layouts 823 each;
// entry-before-result array 978; array initialized after iterator setup 821.
// Best gain seven is below the keep floor; grouped storage also requires a match.
// Original separate outputs and complete body restored; result/class/NONE homes remain.
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
// Round 216: Complete selected identifier through optional outputs and return copy
// after outputs/after requests, with member/native iterator and before/after-lookup reset:
// 998/998/998/998/1017/1017/1017/1017. Original constructor and publication lifetimes restored.
// Round 234: Downstream request eligibility as the actual urgent argument, bool/byte/short/long after
// outputs 996/995/1008/1009; before outputs 996/995/1009/1010. Original search and request guards
// restored.
// Round 47 (m3 lane 14, fast): both builds keep `mode` in EBP for the first four type requests; once it is dead
// retail caches a zero there (`xor ebp, ebp` before the fifth request's lookups, `cmp reg, ebp` for its tests and
// the inheritance loop's tests, index and offset zero stores), ours uses `test` and immediates and jumps over
// the loop's alignment padding (`jle; jmp`) where retail falls through it. The found outputs stored after the
// requests 1009 (2849 bytes); the iterated kinds requested before the asked-for ones 980; the named-animation
// pointer declared at the block top 828; the iterator outputs declared before the iterator 828; the iterator
// declared after the entry pointer 828. 0x1dbc80 stays 828 in every form.
// Round 43 (lane 15, fast): retail keeps the three iterator outputs contiguous in mode, class, type order and gives the
// per-attempt candidate found_mode's dead parameter home; the three outputs as one `long found[3]` do the same (807, not kept: locals packed into an array are against the lane rules; the
// global alignment falls 163 -> 141, candidate stores now at retail's [esp+0x60]); with the iterator stored graph, zeros, keys as
// retail and matched 0x1dc790 807, not kept (no gain). On that: initialised array 823, anonymous struct 823, memset of the array 807, any array/result names
// or array declared first 807 (renaming the locals on the old form 828: names do not move this home). Outputs and id as one named
// struct: 972 (2807 bytes), retail's first 11 instructions and frame, with loop forms for/while/flag all the same code (for or
// while with a break 997, 2839 bytes; flag 972); a second id set on both loop exits gives retail's reload path and jump shape 979
// (2825 bytes; on the array form 988), the store-order permutations of the iterator 818-972. Left: retail's NONE reloaded after
// the je (`or eax, -1`), ECX carrying the id from the loop to the 0x1dd9d0 call, the second request's EBX/EBP roles and the late
// zero carrier.
// M294: Per-attempt scalar candidate graph/index short/short, short/long, long/short, long/long,
// defaults before/after lookup:
// 1012/1012/1012/1012/1012/1012/1012/1012. Original complete executable source restored.
// P260: Partial resolved-output sentinels before result construction, subsets
// mode/class/both/type/mode-type/class-type: 830/830/979/830/979/979; mode/class after it
// 830/830. Original constructor, iterator and complete body restored.
// N308: Shared resource/inheritance counter zero lifetimes and independent resource ordinals,
// seven subsets 1011/902/1007/828/1011/902/1007; complete graph with plain inheritance while 1007.
// Several shared users regress; every helper and original body restored. Return/output/candidate
// homes, class/NONE roles and persistent zero carrier remain.
// w14A (r718 base, fast 828/828): statement walk of both twins against retail, block by block. Their retail bodies are identical
// but for the named-animation array at entry+4 (0x1db170) versus the overlay array at entry+0xc (0x1dbc80). Iterator setup and
// zero stores, per-pass candidate reset to NONE, the 8-byte lookup keyed by `set`, the NULL-entry break, found-id loop exit, the three
// optional output stores, the NONE test, 0x1dd9d0, both resource requests (first keyed by the arguments, second by the iterator's
// outputs) with five kinds each in retail's order, and the inheritance loops all agree: no behaviour difference. All remaining
// instruction differences are register homes/scheduling, the EBP zero (about 736 shifted lines) and epilogue layout: retail returns
// through four epilogues (early NONE class 0x1dbc6b, no result 0x1dbc47, empty second inheritance loop 0x1dbc57, loop end), ours
// through one (part of the 2807 against 2825 byte gap). No source change kept.
// Round 520: actual named-table pointer/reference before/after candidate construction,
// original/native iterator: 828/828/828/828/826/826/826/826.
// Field receivers do not match; two-count initialization gains rejected. Body restored.
// M319: Native urgent-resource/inheritance/both endpoints, field-read bounds
// 1325/807/1341; cached bounds 999/808/999; urgent/both equality bounds 999/999.
// Inheritance-only 807/808 loses matched 0x1dc8c0 to 565/558; shared-user
// gate rejects both. Every helper/body restored; initial/output homes and zero remain.
// N334: Complete per-attempt result defaults before/after lookup, aggregate/member transfer,
// then the same four without the guaranteed mode default: 989/927/993/1002/988/926/992/995. Frames
// 0x2c/0x34 and result/output homes regress. Separate candidate, iterator and full body restored.
// Round 45 (m3 lane 2, fast, base 828): aggregate-initialized iterator 826 (as round 520's native iterator); the
// search wrapped in `if (weapon_class != NONE)` 1040 (2857 bytes); early `return c_type_709360();` with the result
// declared in the block 988 (2825 bytes, the early exit gets its own epilogue); the candidate after the lookup 996
// (as listed); the three outputs set by one chained assignment 828 (same code).
// Round 63 (m3 lane 10, plain-confirmed): the matched siblings' form (0x1dc790/0x1dc8c0: `if (weapon_class
// != NONE)` block, iterator stores graph/step/entries then mode/class/type, first iterate before `while (!found
// && entry)`, `found = true` or re-iterate in the else arm) 755 at 2825 bytes (twin 0x1dbc80 the same); without
// the flag as `while (result.index == NONE && entry)` with `if (result.index == NONE)` re-iterate 755 (same code).
// Not kept: a flag set in a test / a guard repeating the loop test, kept only on a match. On it: the loop as
// `while (entry && result.index == NONE)` 1104; `while (entry) {...; if (found) break; iterate}` 1049; original
// iterator store order 765; candidate renamed after the iterator outputs 755; existing 0x1ddd00 called (stays out of
// line) 3494; early return in place of the block 995; `while (entry && !found)` 1132. Left on the 755 form: found_mode's
// dead home goes to iterator_mode (retail: the candidate), and retail's `xor ebp, ebp` zero for the fifth type request
// and the inheritance init (matched 0x1dc8c0 has `xor ecx, ecx` there; ours `test`, immediates and a jmp over padding).
// Round 124 (m3 lane 7, fast): iterator outputs renamed resolved_* 828 (same code); no candidate, result
// set in an if/else on `named` 977 (2825 bytes); a `graph` local for `this` 828 (same code); a `for` over
// the iterate call with a found break 998 (2839 bytes); chained NULL entry stores 828. Body restored.
// M3 lane 7 r126 (fast, 0x1dc790/0x1dc8c0 checked): retail's EBP zero is the inheritance loop's `i = 0` (the matched
// out-of-line 0x1ddd00 keeps i in EBP: `xor ebp, ebp; cmp eax, ebp; mov [esp+0xc], ebp`), placed before the fifth type
// request and also used for its NULL/count tests; the requests' mode-table temp shares found_mode's dead home with the
// loop candidate. `long i = 0;` above the kinds request in the inline helper 1011 (loses 0x1dc8c0; no EBP zero); an
// inline iterator-setup helper for both twins (siblings' store order) 828; `for (long i = 0; ...)` 828; the outputs
// declared type, class, mode 828 (stores reorder; iterator_mode keeps found_mode's home); the per-type lookup as an
// inline helper returning the id, as the siblings do, for both twins 828. All restored.
// DH347: Iterator record lifetime ends after search/output publication, with guarded/entry
// scalar defaults and member/native aggregate initialization: 828/828/979/979/828/828/979/979.
// Original full body restored; frame agrees, class/NONE and result/output homes plus late zero remain.
// DL263: Const complete value-return interface with original producer, explicit return copy,
// single exit, loop-lived candidate, immutable candidate, native result words, iteration entry and
// direct result producer: 828/828/1011/932/978/979/828/977. NOT kept: no exact return-contract
// keeper; original interface and complete body restored. Frame 0x2c agrees; class/NONE,
// output/candidate homes and late zero remain.
// N371: Actual selector/output-pointer/search-key formal lexical families, seven
// subsets then complete result/local roles: 828/828/828/828/828/828/828/828. NOT kept: unmatched
// parameter-home forms; original body restored; class/NONE, result/output/candidate homes and zero remain.
// M3 lane 8 r135 (fast; raw/register-masked/stack-masked aligned, base 580/234/76): the siblings' form again 755 (297/525/57, 2825
// bytes), through the mode's address 755; on it the candidate at the block top or after the entry pointer 1028 (2856 bytes), the
// iterator outputs renamed either way 755 (same homes), type first 755, the result set before the search test 1005, a direct result
// with a default 1021 (206/151/86, 2841 bytes). Retail's late zero is a temp in EBP from the first kinds lookup on; ours keeps
// a parameter in EBP over that block and re-reads it (retail reads it from the stack), so the zero stays immediates. Flag form: match-only.
// DL274: Six native stack-formal home reads individually, mode/class pair, then all six: 828/828/828/828/828/828/828/828.
// NOT kept: native class/NONE, candidate/output/result homes and late zero remain; no eligible byte match or native repair.
// Original complete executable source restored.
// M3 lane 12 r93 (fast): the second request's keys copied into named locals first 828 (identical: the copies coalesce);
// `if (result.index == NONE) return result;` with the calls unguarded 1012 (2839 bytes); the inheritance loop as a while
// with `long i = 0;` above the kinds request 1011 (0x1dbc80 1011, loses 0x1dc8c0 to 474: the zero is stored at the top);
// the inherited graph named in the loop 828; the siblings' iterator store order 828; the search, outputs and requests in
// one `while (true)` with a found `continue` and a closing break 828 (same code). Diagnostics: the second request dropped
// 2235 (no zero register even then); 1dd9d0 and both requests inside the loop's found pass (outputs after the loop) 991 at
// 2825 bytes with retail's `xor ebp, ebp; cmp eax, ebp` zero and `mov [i], ebp` stores; the outputs after the requests,
// outside the loop, 1009 without it. Our build makes the EBP zero only for request code inside the search loop.
// m3 lane 12 r103 (fast, both bodies; 0x1dc8c0/0x1dd9d0 matched throughout): the entry pointer declared in the loop 828;
// the candidate as `named ? named->animation_id : c_type_709360()` 978; mode read through its own address as 0x1dc8c0
// does 828; that with the outputs declared after the iterator's stores 826; the outputs there with the candidate declared
// at the top of the loop body 965 (2825 B, the candidate reset leaves the lookup's argument pushes). Retail's three
// epilogues (early NONE, the no-result/loop end pair through ECX, the empty second inheritance loop through EDX) stay one.
// MiniMax I (11-10): no new form, no lower score; the iterate helper takes its fourth output in EBX in both builds and its first two on the stack, retail's three found locals sit at E-0x2c, E-0x28 and E-0x24 while ours puts the third one in the dead found_mode argument slot at E+0x18, and the result struct (E-0x20) and the iterator base (E-0x1c) already agree, so only that third local's home and retail's EBP zero carrier are left.
// M3 lane 12 r108 (fast): `while (true)` with the found break tested on the candidate (not result) 998 (2839 B, the iterate
// block duplicated at the bottom; the listed while/for-break code, a re-measure); a top-tested `while (result.index == NONE)` 828, the iterate call in that condition 828,
// `while (!found && (entry = iterate()) != NULL)` with `found = true` 828, a do-while guarded `if (entry)` body with `while (entry
// && result.index == NONE)` 828 (all four the same code). Retail's single iterate call, `jne exit; jmp top` and the NULL exit's
// `mov ecx, [result]` come from carrying the result dword in ECX across the output stores to the 0x1dd9d0 push and the no-result
// return copy (matched 0x1dc790 carries it in EBP); ours reloads it after the stores. Diagnostics: the outputs renamed to sort last
// (same code); one or two of the three output stores dropped, no zero; all three dropped, a zero in EBX from the fifth kind on.
// Also: `found = result.index != NONE;` with `while (!found)` 828 (same code); the second request through the real 0x1ddd00 with
// a `__forceinline` redeclaration 2157 (1667 B; it inlines into 0x1ce180 too, which then differs). Retail loads the second
// request's mode once into EBP (`mov ebp, [esp + 0x10]`) and reads class/type from memory at each kind; ours reads all three
// from memory, as matched 0x1dc8c0 does for its address-taken keys.
// m3 lane 3 r227 (fast): the siblings' two-call form (755, re-measured) is retail's loop: our two iterate calls cross-jump into one
// (`jne exit; jmp call`) at 2825 bytes, and its first 95 lines differ only in homes. From the second request on, retail holds the found
// mode in EBP across the lookups and keeps `this` in EBX (the +0x34 table in found_mode's dead slot), where ours re-reads iterator_mode
// from its home per lookup and gives EBX the table, EBP `this`; matched 0x1dc8c0 re-reads its address-taken key per lookup in retail
// too, so retail's EBP copy is particular to this body. The three outputs as one record re-measured 755: the second request still
// re-reads the mode. `return result;` inside the found block as well 1012 (2839 bytes: the listed early-return form's code, a
// re-measure). New forms: on the two-call form `if (found)` as
// the post-loop test 1060 (2873), `found` declared first in the block 755 (same code); both twins' bodies as one shared forceinline
// helper (overlay bool) 828 (same code); the search as `while (result.index == NONE)` 828 (same code, the first test folds; close to
// the listed result-driven while). No other untried natural loop, return or declaration form is left on either base.
// Round 188 (m3 lane 4, fast with the twin, the siblings, 0x1dd9d0/0x1ddd00/0x2963f0/0x1dd560; aligned/masked control 321/141):
// re-measured the siblings' flag form 755 (aligned 174) and the single exit around the do-while 1040 (2857 B, aligned 125: closest
// in content; its excess is retail's EBP zero, our jumps over loop padding and `this` in EBP where retail has EBX). New: that single
// exit with the siblings' store order 1040; with no per-iteration candidate 1018 (2845 B), with both 1018; no bare block after
// the early return 828 (same code); the named lookup as a shared inline helper on the 755 form 755 (same code); the iterator by
// an aggregate initializer 826 (two points); on that, the single exit 1038, no candidate 1020 (2791 B), `named` first 994, the
// flag loop 769, the found outputs stored type, class, mode 827. Re-measured: `named` before the candidate 996.
// m3 lane 14 r82 (fast; masked = register- and esp-masked aligned rows, base 76): `result = named ? named->animation_id :
// result;` with no candidate 848 (2791 bytes, masked 96); the found mode copied into a named local before the outputs and used for
// the found_mode store and the second request 828 (identical code: the copy coalesces even across the output stores); a do-while
// on the entry with the lookup inside `if (entry)` and a found `break` 828 (identical code); the second request's graph read from
// the iterator 962 (2791 bytes, masked 101). Retail's loop is this do-while: the NULL exit reloads the result into ECX and the
// found exit already has it there (`jne join; jmp iterate`), then the index test reads memory (`cmp word [esp+0x1e], si`).
// Also r82: the requests' `false` as a `bool other = false;` local 927 (2775 bytes), in the first request only 927 (other code), in the
// second only 826 (2807, masked 75: the same code as `urgent` there); `bool urgent = true;` for both or the first request 801 (2791
// bytes, masked 83: only loop-alignment padding moves, retail's `jmp` over padding in the first request is lost; a constant-only
// local, not kept), for the second only 826, `bool const urgent` 828 (the base code), both locals in both requests 1050 (2724 bytes);
// 0x1dd9d0 after the requests 1030 (2853); the requests in reverse order 980 (diagnostics).
// M3 lane 12 r120 (fast with the file's 37 functions, 0x2963f0, 0x1dd560, 0x1236f0; control 828/828, 39 matched): slot counts
// (build/r120/slots.py) explain found_mode's dead home: the compiler gives a dead parameter home to the heaviest value that can
// live there. Retail's is the shared +0x34 table pointer with the per-pass candidate (14 references at +0x54 after the pushes; the
// three iterator outputs have 8, 8 and 7 and stay in the frame); ours is iterator_mode with 12 (one store, one address, ten
// reads), because our second request reads the mode from memory at each of its kinds, keeps the table pointer in EBX and
// loads `this` into EBP once per kind, where retail loads `this` into EBX and the mode into EBP once at the request's start
// (as both builds do for the first request). Diagnostic (reverted, changes behaviour): the second request keyed by the `mode`
// parameter 829/828: iterator_mode (3 references) then sits in the frame and the candidate and table pointer take a dead
// parameter home, as the rule predicts. Trials: 0x1dd9d0 declared `inline` (callers of an out-of-line `inline` callee assume
// the standard registers clobbered) 828 (same code); the request helper's three keys as `long const &` 828 (same code); a fresh
// `graph` and `found` local declared just ahead of the second request 828 (same code: both copies still coalesce with the
// homes). Left as before: the second request's register roles, hence the dead-home owner and the EBP zero.
// m3 lane 8 r164 (fast, base 828; twin 0x1dbc80 828 throughout): the three outputs declared ahead of the iterator 828; the
// outputs declared bare and set by one chained `= NONE` after the declarations 828; the candidate copy-initialised from
// `c_type_709360()` 828; the entry pointer declared first in the block 828; the iterator's keys stored type, class, mode 828
// (each the same code). Our iterate call passes iterator_mode at found_mode's dead home (F+0x44) with class/type/result at
// F+0/+4/+8 and the candidate at F+0xc; retail has mode/class/type/result at F+0/+4/+8/+0xc and the candidate (later the
// +0x34 table pointer) at F+0x44: one home choice shifts every frame offset after it.
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

// MiniMax G (11-14): no new form, no lower score (828): the NONE compare still loads the compared argument into ecx
// where retail loads it into edx and keeps the computed -1 in ecx, and the result's and the three resolved outputs'
// frame homes still differ from retail's.

// MiniMax H (11-11): no new form, no lower score (828); the six declaration orders of the mode, class and type out-locals all compile to the same code, so their frame homes are not set by the declaration order, and the first difference is still retail's compare of the weapon class against NONE with the operands the other way round.
// MiniMax G (11-12): the three resolved outputs now share one array whose entries are set after the class check, which moves the -1 stores past the compare and puts the per-attempt candidate in the found_mode argument home where retail has it; the class load still lands in ECX where retail uses EDX, the result sits at frame+0x00 where retail has frame+0x0c, and the resolved outputs sit at frame+0x04 where retail has frame+0x00.
// score 828 (was 828): Frame 0x2c agrees; class/NONE, result/output/candidate homes and later zero carrier remain.
// NOT kept (coordinator review, MiniMax G 11-13): 828 -> 807 came from grouping the three resolved outputs into one
// `long resolved[3]` array (an aggregate grouping locals: kept only with a byte match); the three named locals stay.
// MiniMax S (11-06): Measured from the frame base, retail keeps iterator_weapon_type at frame+0x04,
// iterator_weapon_class at frame+0x00 and iterator_mode at frame+0x08, while ours keeps iterator_weapon_type
// at frame+0x00, iterator_mode at frame+0x04 and iterator_weapon_class in the reused argument slot; our
// result shorts therefore land at frame+0x08 instead of frame+0x0c and the tail compares out of step.
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
// Round 43 (lane 14, fast, both bodies): single exit as the matched 0x1dc790 1040 (2857 bytes); 0x1dc790's
// iterator field order 828 (code changes); both 1040; iterator outputs declared after the iterator initialisation
// 826 (two-point gain rejected); that with 0x1dc790's field order 828. Retail gives found_mode's dead parameter home
// to the per-attempt candidate (ours to iterator_mode) and the no-result path its own epilogue.
// Round 489: chained candidate/result assignment in the named branch, native iterator, late constant
// candidate and missing name branch first, combinations 978/976/984/982/991/989/984/982.
// All reach 2825 bytes but regress; original complete producer, iterator and body restored.
// Round 211: native resolved-key array in six fixed mode/class/type layouts 823 each;
// entry-before-result array 978; array initialized after iterator setup 821.
// Best gain seven is below the keep floor; grouped storage also requires a match.
// Original separate outputs and complete body restored; result/class/NONE homes remain.
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
// Round 119 (lane 8, fast): retail gives found_mode's dead home (+0x4c) to the per-attempt candidate,
// ours to iterator_mode. The three outputs initialized before the class test 979 (stored before the
// branch, home unchanged); outputs and candidate renamed to sort after `result` 828 (same code: names do
// not move these slots); the candidate declared at the loop-body top 967 (2825 bytes, constructed before
// the iterate call); a pre-tested `while (result.index == NONE)` 828 (same code); that with the outputs
// set after the iterator fields 826.
// Round 216: Complete selected identifier through optional outputs and return copy
// after outputs/after requests, with member/native iterator and before/after-lookup reset:
// 998/998/998/998/1017/1017/1017/1017. Original constructor and publication lifetimes restored.
// Round 234: Downstream request eligibility as the actual urgent argument, bool/byte/short/long after
// outputs 996/995/1008/1009; before outputs 996/995/1009/1010. Original search and request guards
// restored.
// Round 43 (lane 15, fast): retail keeps the three iterator outputs contiguous in mode, class, type order and gives the
// per-attempt candidate found_mode's dead parameter home; the three outputs as one `long found[3]` do the same (807, not kept: locals packed into an array are against the lane rules; the
// global alignment falls 163 -> 141, candidate stores now at retail's [esp+0x60]); with the iterator stored graph, zeros, keys as
// retail and matched 0x1dc790 807, not kept (no gain). On that: initialised array 823, anonymous struct 823, memset of the array 807, any array/result names
// or array declared first 807 (renaming the locals on the old form 828: names do not move this home). Outputs and id as one named
// struct: 972 (2807 bytes), retail's first 11 instructions and frame, with loop forms for/while/flag all the same code (for or
// while with a break 997, 2839 bytes; flag 972); a second id set on both loop exits gives retail's reload path and jump shape 979
// (2825 bytes; on the array form 988), the store-order permutations of the iterator 818-972. Left: retail's NONE reloaded after
// the je (`or eax, -1`), ECX carrying the id from the loop to the 0x1dd9d0 call, the second request's EBX/EBP roles and the late
// zero carrier.
// DL229: unsigned resolved mode/class/type storage in seven subsets 828 each;
// all with native iterator aggregate 826. Exact-only API casts and every key type restored;
// class/NONE, result/candidate/output homes and later zero carrier remain.
// DL239: Optional mode/class/type output publication via memcpy subsets 1..7, then all memmove:
// 828/828/828/828/828/828/828/1059. Original complete body, output order, native identifier and
// iterator restored; frame 0x2c agrees, class/NONE and result/candidate/output homes plus late
// zero carrier remain.
// N308: Shared resource base/native list metadata/empty-request return, seven subsets,
// 1098/1436/1546/828/1098/1436/1546; complete graph with readonly metadata 1546. Several
// shared users regress; all helpers and original body restored. Result/output/candidate homes,
// class/NONE roles and persistent zero carrier remain.
// Round 78 (m3 lane 12, fast; aligned counts are stack-masked): the found outputs as one three-long key struct 823 (2807 bytes, retail's
// frame homes for the outputs; not kept, gain under 8). On that: the candidate at function scope 925 (2825 bytes), the candidate as a ternary
// 952, `while (true)` with a break 997 (2839). `if (weapon_class != NONE)` around the body for the early return 1040 (2857 bytes) but
// aligned 984 -> 125: the result stays in ECX across the outputs and retail's jne/jmp loop exit appears; iterator stores in 0x1dc8c0's order on
// it 1040, aligned 119 (not kept: positional rise while the padding jumps shift). Left there: result takes found_mode's home where retail gives it to
// the candidate, no zero carrier in EBP, `this` in EBP and the mode in memory for the second request. On it: names of the result, key or iterator 1040
// (no move); candidate at function scope 1030 (2856), loop condition on the candidate 1030, break form 1076, a reset candidate 1042; the second mode in a local 1040.
// 0x1dd560 or 0x1236f0 marked inline on it 1040 each (no move).
// N325: Resolved-key sentinel chain/iterator null chain/complete optional-publication
// results, seven subsets and all with class-rejection result: 828 each. Original
// initialization, conditions, helpers and complete body restored; homes and zero carrier remain.
// Round 546: Alias-safe named/returned/both identifier transfers 986/977/992;
// mixed library boundaries 977/986; alias-safe with native returned/named words
// 1012/988; both with late candidate 1007. All regress; original complete body restored.
// Lane 14 r55 (fast): the iterator fields in 0x1dc790's store order 828 (same code). The matched siblings' shape
// (0x1dc790, 0x1dc8c0): `if (weapon_class != NONE)` around the body, the first lookup before
// `while (!found && entry)` and the next in the else arm 755 at retail's 2825 B (not kept: a flag set inside a test
// needs a byte match); on it the lookup in an inline helper 755, the candidate renamed 755, three declaration orders of
// the outputs 755/755/755, a break in place of the flag 1049 (2873 B). Left on that form: retail homes the per-pass
// candidate id in found_mode's dead parameter slot where ours puts iterator_mode there, so the frame shifts by four.
// Round 42 (m3 lane 2, fast): the iterator outputs declared class/type/mode and type/class/mode 828/828 (no slot
// moves); the three in one declaration 828; the result set in both arms of the named test (no candidate) 977 (2825
// bytes); matched 0x1dc790's found-flag loop (iterate before the loop and in the miss arm) 995 (2825 bytes, first
// difference +0x1c). Original body restored.
// Round 63 (m3 lane 10, plain-confirmed): the matched siblings' form (0x1dc790/0x1dc8c0: `if (weapon_class
// != NONE)` block, iterator stores graph/step/entries then mode/class/type, first iterate before `while (!found
// && entry)`, `found = true` or re-iterate in the else arm) 755 at 2825 bytes (twin 0x1dbc80 the same); without
// the flag as `while (result.index == NONE && entry)` with `if (result.index == NONE)` re-iterate 755 (same code).
// Not kept: a flag set in a test / a guard repeating the loop test, kept only on a match. On it: the loop as
// `while (entry && result.index == NONE)` 1104; `while (entry) {...; if (found) break; iterate}` 1049; original
// iterator store order 765; candidate renamed after the iterator outputs 755; existing 0x1ddd00 called (stays out of
// line) 3494; early return in place of the block 995; `while (entry && !found)` 1132. Left on the 755 form: found_mode's
// dead home goes to iterator_mode (retail: the candidate), and retail's `xor ebp, ebp` zero for the fifth type request
// and the inheritance init (matched 0x1dc8c0 has `xor ecx, ecx` there; ours `test`, immediates and a jmp over padding).
// Round 61 (m3 lane 11, fast; aligned in brackets): round 78's guard re-measured 1040 (2857 bytes) [126]; the guard with the three-long key 1040 [125], plus the
// iterator stores in 0x1dc8c0's order 1040 [119]; on that the candidate declared ahead of the iterate call 1034 (2857) [124]; the key and that store order without the guard
// 823 (2807) [983]. Left on the guard form: 32 bytes of extra jumps and padding around the shared loops, no zero carrier in EBP, `this` in EBP (retail EBX), result in found_mode's home.
// On the 823 form (same code each, 823): the key declared before the iterator; the iterator's entries cleared first; the key set last to first; the entry pointer declared in the loop;
// the candidate initialised at its declaration.
// Round 67 (m3 lane 10, fast): diagnostic, the second resource request removed: the result takes found_mode's dead
// home and the three outputs go contiguous (retail's output layout), so the outputs' later reads decide that home.
// The lookup in an inline helper on this do-while 828; the candidate at block scope with the loop testing it 926
// (2825 bytes); the outputs declared bare and set by one chained `= NONE` 828; the iterator as an aggregate
// initializer 828; no candidate, `if (named) result = named->animation_id;` 1020 (2791 bytes).
// N361: Existing iterator/sorted-lookup/registration/resource inline call visibility, four
// individuals, three iterator pairs and all four: 828 each. Original declarations and full body
// restored. NOT kept: no result/candidate/output home or class/NONE/zero-carrier recovery.
// Lane 9 r87 (fast; masked = registers and stack homes masked, sequence aligned; base 828, masked 48): the siblings' body (`if
// (weapon_class != NONE)`, iterator stores graph/step/entries then keys, first iterate before `while (!found && entry)`, `found` or
// re-iterate in the else arm) 755 at 2825 bytes, masked 36, then the three outputs and the result as one record `{ long mode, weapon_class,
// weapon_type; c_type_709360 result; }` 732: the candidate and the mode-table temp then take the dead home, and the first 255 lines are
// byte-identical to retail. Not kept, a flag set in a test and grouped storage need a match. Outputs alone as a record, in either member
// order, or an array 755 (the result takes the dead home instead); the record with the members reversed 738. The one structural cause
// left is the zero: retail's `xor ebp, ebp; cmp eax, ebp` from the fifth type request on, ours `test` and immediates. On the 732 form:
// the real 0x1ddd00 with `inline` on it alone 3273 (558 bytes, out of line), on all three 732 (same code, all three still match);
// `bool other = false` as a local 825 (2809). Diagnostics: only the second request kept, or the output stores removed, and the zero
// appears in that request (`xor ebp, ebp` plus five `cmp reg, ebp`); only the first request kept, no zero; requests swapped 964 (2873);
// no 0x1dd9d0 call 978, no loop 1019. Also on the old body: a while with a break 998 (2839 bytes); early `return result` for a NONE id 828.
// w14A (r718 base, fast 828/828): statement walk of both twins against retail, block by block. Their retail bodies are identical
// but for the named-animation array at entry+4 (0x1db170) versus the overlay array at entry+0xc (0x1dbc80). Iterator setup and
// zero stores, per-pass candidate reset to NONE, the 8-byte lookup keyed by `set`, the NULL-entry break, found-id loop exit, the three
// optional output stores, the NONE test, 0x1dd9d0, both resource requests (first keyed by the arguments, second by the iterator's
// outputs) with five kinds each in retail's order, and the inheritance loops all agree: no behaviour difference. All remaining
// instruction differences are register homes/scheduling, the EBP zero (about 736 shifted lines) and epilogue layout: retail returns
// through four epilogues (early NONE class 0x1dbc6b, no result 0x1dbc47, empty second inheritance loop 0x1dbc57, loop end), ours
// through one (part of the 2807 against 2825 byte gap). No source change kept.
// DL261: Actual iterator/sorted-search/registration definitions and every declaration inline, seven subsets then explicit iterator expansion: 828/828/828/828/828/828/828/1265.
// NOT kept: no exact native form; original complete body and all temporary contracts/types restored.
// DL263: Const complete value-return interface with original producer, explicit return copy,
// single exit, loop-lived candidate, immutable candidate, native result words, iteration entry and
// direct result producer: 828/828/1011/932/978/979/828/977. NOT kept: no exact return-contract
// keeper; original interface and complete body restored. Frame 0x2c agrees; class/NONE,
// output/candidate homes and late zero remain.
// Round 65 (m3 lane 14, fast, on the siblings' body plus record, 732 at 2825, masked 55; without the record 755, masked 57): `this` named
// for both requests 732; the found keys named for the second request 732; the first request from the iterator's stored keys 1019 (2875);
// the requests guarded on the found flag 1019 (2841); the keys through a local table 732. Retail keeps `this` in EBX and the found mode in
// EBP through the second request; ours gives EBX the +0x34 table pointer and EBP `this`, and no zero register forms.
// m3 lane 12 r103 (fast, both bodies; 0x1dc8c0/0x1dd9d0 matched throughout): the entry pointer declared in the loop 828;
// the candidate as `named ? named->animation_id : c_type_709360()` 978; mode read through its own address as 0x1dc8c0
// does 828; that with the outputs declared after the iterator's stores 826; the outputs there with the candidate declared
// at the top of the loop body 965 (2825 B, the candidate reset leaves the lookup's argument pushes). Retail's three
// epilogues (early NONE, the no-result/loop end pair through ECX, the empty second inheritance loop through EDX) stay one.
// MiniMax I (11-10): no new form, no lower score; this twin of 0x1db170 differs from ours in the same two places, the third found local's home (E-0x24 in retail, the dead found_mode argument slot at E+0x18 in ours) and the per-attempt candidate's home, with retail's EBP zero carrier still shifting the request block.
// m3 lane 7 r146 (fast, both twins, 0x2963f0/0x1dc790/0x1dc8c0 matched unless stated; aligned/masked = difflib counts with addresses /
// registers and stack offsets masked, base 828 [580/74]): retail reads the second request's mode once into EBP (`mov ebp, [esp+0x10]`), so its
// iterate callee lets the mode output's pointee escape and the inline argument becomes a copy. The iterate body reading `found_mode` through
// its own address at the ANY_NAME store gives that copy: 821 on both twins [583/76] (match-only form, gain under 8; the copy spills into the
// input mode's dead home, EBP takes found_weapon_class and the candidate takes that parameter's dead home); with the single-exit guard 1034;
// with the siblings' found-flag body 777 at 2841 bytes [544/44] (siblings' body alone re-measured 755 [297/55]); on that body the class
// output's escape 774 (0x1dc8c0 0 -> 56), the type's 1034 (iterate, 0x1dc790 and 0x1dc8c0 lost), mode+class 785, mode+type 1034, class+type
// 1077, all three 1099; mode escape with the outputs declared type/class/mode 776 (EBP still on found_weapon_class). Diagnostic: the second request
// keyed by the input mode 829 (EBP again takes found_weapon_class). Left: retail has found_mode in EBP and the candidate plus the +0x34 table
// temp in its dead home; every form that lightens iterator_mode moves EBP to found_weapon_class instead.
// Then, on the mode escape plus siblings' body (777): no candidate, the result set in both named arms 1041 (2857 bytes); the found
// flag declared before the outputs 777 (same code); the iterator stored in declaration order 787; `while (entry && !found)`
// 1131 (2925 bytes); the escape read at the copied-mode store instead 777 (same code).
// m3 lane 10 r87 (fast with 0x1db170/0x1dc8c0/0x1dd9d0/0x1dd560/0x2963f0; al = aligned count): re-measured the
// `if (weapon_class != NONE)` block 1040 (2857 bytes, al 580 -> 153), the sibling's iterator store order 828, both 1040
// (al 146), the while/break loop 998 (2839) and all three 1049 (2873). New: `this` through a named graph pointer 828
// (same code); on the block form the same 1040; the block form with `if (result.index == NONE) return result;` ahead
// of the requests 1016 (2839 bytes, al 134), with the graph pointer 1016, with the sibling's iterator store order 1015
// (2839 bytes, al 113). Retail has three epilogues (the early exit for a NONE
// weapon_class, with two pops; the loop end and no-result exit; a copy for the empty second inheritance loop); ours
// merges them into one.
// m3 lane 13 r79 (fast, 0x1dc790/0x1dc8c0 matched throughout): re-measured, the entry pointer declared first in the
// block 828 (round 47's form) and the siblings' form with `found` declared first 755 (r227's form), used as the base
// below. New: the siblings' form with the result scoped in the block and `return c_type_709360();` after it 999 (2838
// bytes); on the 755 form the entry pointer initialised at its declaration 755, the three outputs declared bare at
// function scope ahead of `result` and set to NONE in the block 755 (same code: no home moves), the outputs declared
// after the iterator's stores 769, the candidate declared after the lookup 995 (2841). On the 755 form only stack homes differ before the
// requests: ours gives found_mode's dead slot (E+0x18) to iterator_mode and the +0x34 table temp found_weapon_class's
// slot, retail gives found_mode's slot to the per-pass candidate and the table temp and keeps the outputs and result
// contiguous below the iterator. Declaration order, scope and names do not move that choice.
// m3 lane 15 r74 (fast; 0x1db170/0x1dc790/0x1dc8c0/0x1dd9d0 unchanged): the siblings' found-flag body re-measured 755 at
// 2825 B (register-masked aligned 57: left are retail's EBP zero from the fifth type request of the first request, which
// retail can form because it reloads the mode from memory for the inheritance loop where ours keeps it in EBP, the
// inheritance counters stored from that zero into dead parameter homes, and loop-head padding jumps). On it, mode read
// once through its own address as 0x1dc8c0 does, at the iterator store or at the first request 755/755 (same code each:
// the plain read through the address does not keep mode out of EBP).
// On the 755 body: the first request keyed by `iterator.mode` (the iterate helper only reads the iterator's keys) 808 at
// 2841 B (aligned 924 -> 84, masked 57 -> 49: the registers mostly agree, but iterator_mode takes a dead parameter slot,
// frame 0x28 against 0x2c, and still no EBP zero); all three first-request keys from the iterator 1020 (2875 B); the
// iterator as an aggregate initializer 769; the candidate as a ternary, or the result set by that ternary with no
// candidate, 1024/1024 (2841 B). Lane 9's record (outputs and result as one local) re-measured 732; with the
// `iterator.mode` first request 762 (2841 B, aligned 78, masked 43).
// MiniMax D (11-11): no new form, no lower score (828): the third iterator output still takes the dead `found_mode`
// argument slot instead of a frame slot, so retail's result shorts sit at +0xc/+0xe and ours at +0x8/+0xa, and retail's
// EBP zero carrier in the second resource request is still missing; declaring the three NONE locals ahead of the early
// return gives retail's sentinel register but reorders the prologue and scores 979.
// M3 lane 12 r120: see 0x1db170's note of this round (same code but for the array offset); the same three trials and the
// diagnostic measured 828 on this body in the same checks.
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
