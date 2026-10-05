// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1DACB0.CPP: the animation graph tag's lookups (0x1dacb0..0x1ddea0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_123680.h"
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
			function_1236f0(resource, true);
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
c_type_709360 function_1dd5d0(s_graph_tag *graph, c_type_709360 animation_id)
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
	return result;
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
			animation_id = function_1dd5d0(graph, animation_id);
			animation = graph_animation_get(arg_0e6cbc, animation_id.index);
			if (1.0f > animation->weight)
			{
				s_random_globals *local_4c4858 = g_4e7408;
				real random = first_seed ? function_x82e52f(&local_4c4858->unknown0, NULL, 0) : function_x82e52f(&local_4c4858->seed, NULL, 0);

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

		return (byte *)function_1237e0(resource, animation->name) + animation->resource_offset;
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
			function_1236f0(resource, false);
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
						function_1236f0(resource, true);
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
						function_1236f0(resource, false);
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
void function_1dacb0(s_graph_tag *graph, c_type_709360 animation_id, real *distance, real *event_distance)
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
			case 2:
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
c_type_709360 function_1dd0b0(s_graph_tag *graph, long name)
{
	c_type_709360 animation_id;

	if (graph)
	{
		s_graph_tag *current = graph;
		long graph_index = NONE;

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
				if (animation->name == name)
				{
					animation_id.graph_index = (short)graph_index;
					animation_id.index = (short)index;
					break;
				}
			}
			if (animation_id.index == NONE)
			{
				s_graph_inheritance *inheritance;

				graph_index++;
				if (graph_index >= graph->inheritance_count)
				{
					break;
				}
				inheritance = &graph->inheritance[graph_index];
				current = NULL;
				if (inheritance->graph_tag_index != NONE)
				{
					current = graph_tag_get(inheritance->graph_tag_index);
				}
			}
		}
		while (current);
		if (animation_id.index != NONE)
		{
			function_1dd9d0(graph, animation_id);
		}
	}
	return animation_id;
}

/* an orientation (as unknown_141590.cpp declares it) */
struct rigid_transform_scaled
{
	quaternionf rotation;
	point3f position;
	real scale;
};

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

/* the weapon class entry of a mode (inlined copies) */
inline s_graph_mode_entry *graph_weapon_class_get(s_graph_tag *graph, long mode, long weapon_class)
{
	s_graph_mode_entry *result = NULL;
	s_graph_mode_entry *mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&graph->mode_count, mode, 0x14);

	if (mode_entry)
	{
		result = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&mode_entry->child_count, weapon_class, 0x14);
	}
	return result;
}

// @retail 0x1dcf20
bool function_1dcf20(s_graph_tag *graph, s_graph_pair_iterator *iterator)
{
	long index = iterator->index + 1;

	for (;;)
	{
		s_graph_mode_entry *entry = NULL;

		while (iterator->step < 2 && !entry)
		{
			switch (iterator->step)
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
				iterator->step++;
				index = 0;
			}
		}
		if (!entry)
		{
			return false;
		}
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
			switch (iterator->step)
			{
			case 0:
				entry = graph_weapon_class_get(graph, iterator->mode, iterator->weapon_class);
				break;
			case 1:
				entry = graph_weapon_class_get(graph, iterator->mode, 0x30000d9);
				break;
			case 2:
				entry = graph_weapon_class_get(graph, 0x30000d9, iterator->weapon_class);
				break;
			case 3:
				entry = graph_weapon_class_get(graph, 0x30000d9, 0x30000d9);
				break;
			}
			if (!entry)
			{
				iterator->step++;
				index = 0;
			}
		}
		if (!entry)
		{
			return false;
		}
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
				vector3f forward = matrix.forward;
				vector3f up = matrix.up;
				point3f position = matrix.position;

				parent = &matrices[mirror_parent_index];
				forward.j = 0.0f - forward.j;
				up.j = 0.0f - up.j;
				position.y = 0.0f - position.y;
				matrix.scale = 1.0f;
				matrix.forward = forward;
				matrix.left.i = up.j * forward.k - up.k * forward.j;
				matrix.left.j = up.k * forward.i - up.i * forward.k;
				matrix.left.k = up.i * forward.j - up.j * forward.i;
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
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type);
		while (weapon_type_entry)
		{
			c_type_709360 animation_id;
			s_graph_set_entry *set_entry = (s_graph_set_entry *)function_1dd560(
				(s_sorted_array *)&weapon_type_entry->set_count, set, 0xc);

			if (set_entry)
			{
				animation_id = set_entry->items[item_index].animation_ids[animation_index];
			}
			result = animation_id;
			if (result.index != NONE)
			{
				break;
			}
			weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
				&iterator_weapon_type);
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

// @retail 0x1db170
c_type_709360 s_graph_tag::animation_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
	long *found_weapon_class, long *found_weapon_type)
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
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		while ((weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type)) != NULL)
		{
			c_type_709360 animation_id;
			s_graph_named_animation *named = (s_graph_named_animation *)function_1dd560(
				(s_sorted_array *)&weapon_type_entry->named_animation_count, set, sizeof(s_graph_named_animation));

			if (named)
			{
				animation_id = named->animation_id;
			}
			result = animation_id;
			if (result.index != NONE)
			{
				break;
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
			graph_resources_request(this, mode, weapon_class, weapon_type, true, false);
			graph_resources_request(this, iterator_mode, iterator_weapon_class, iterator_weapon_type, true, false);
		}
	}
	return result;
}

// @retail 0x1dbc80
c_type_709360 s_graph_tag::overlay_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
	long *found_weapon_class, long *found_weapon_type)
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
		iterator.mode = mode;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		while ((weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type)) != NULL)
		{
			c_type_709360 animation_id;
			s_graph_named_animation *named = (s_graph_named_animation *)function_1dd560(
				(s_sorted_array *)&weapon_type_entry->overlay_count, set, sizeof(s_graph_named_animation));

			if (named)
			{
				animation_id = named->animation_id;
			}
			result = animation_id;
			if (result.index != NONE)
			{
				break;
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
		iterator.mode = *mode_reference;
		iterator.weapon_class = weapon_class;
		iterator.weapon_type = weapon_type;
		iterator.step = 0;
		iterator.mode_entry = NULL;
		iterator.class_entry = NULL;
		while ((weapon_type_entry = graph_weapon_type_iterate(&iterator, &iterator_mode, &iterator_weapon_class,
			&iterator_weapon_type)) != NULL)
		{
			result = weapon_type_entry->variant_find(name, a, b, c, d, e, f, g);
			if (result.index != NONE)
			{
				function_1dd9d0(this, result);
				graph_resources_request(this, c, iterator_weapon_class, iterator_weapon_type, true, false);
				break;
			}
		}
	}
	return result;
}
