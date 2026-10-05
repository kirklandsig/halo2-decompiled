// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1D8F00.CPP: a render model's named marker groups, the markers
   of a group placed on an object's node matrices, and pulling a node chain
   toward a marker */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include <math.h>

#define k_real_epsilon 0.0001f

/* one marker of a group (0x24 bytes) */
struct s_render_model_marker
{
	char region_index;
	char permutation_index;
	byte node_index;
	byte unknown03;
	point3f translation;
	quaternionf rotation;
	real scale;
};

/* a named group of markers (0xc bytes) */
struct s_render_model_marker_group
{
	long name;
	long marker_count;
	s_render_model_marker *markers;
};

/* a node (0x60 bytes) */
struct s_render_model_node_view
{
	long name;
	short parent_node_index;
	byte unknown06[0x60 - 6];
};

struct s_render_model_view
{
	byte unknown00[0x48];
	long node_count;
	s_render_model_node_view *nodes;
	byte unknown50[0x58 - 0x50];
	long marker_group_count;
	s_render_model_marker_group *marker_groups;
};

struct s_first_person_marker;

matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
void function_141590(transform4x3f const *in, transform4x3f *out);
void function_1421b0(transform4x3f *out, point3f const *position, quaternionf const *rotation);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_120220(transform4x3f *mid, transform4x3f *root, transform4x3f *target, transform4x3f *end);
struct s_render_model_definition;
void render_model_build_child_node_matrices(s_render_model_definition const *definition, transform4x3f const *parent_matrix,
	long node_index, long node_count, transform4x3f *field_50);

PRIVATE inline s_render_model_view *render_model_view_get(long render_model_index)
{
	return (s_render_model_view *)g_4e3b44[render_model_index & 0xffff].bytes;
}

/* the index of the render model's marker group with the name, or NONE */
// @retail 0x1d8f00
long function_1d8f00(long render_model_index, long marker_name)
{
	long result = NONE;

	if (render_model_index != NONE && marker_name != NONE && marker_name != 0)
	{
		s_render_model_view *definition = render_model_view_get(render_model_index);
		long i;

		for (i = 0; i < definition->marker_group_count; i++)
		{
			if (definition->marker_groups[i].name == marker_name)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

/* whether the marker's region shows the marker's permutation (always without
   a region permutation table, or for markers in no region) */
PRIVATE __forceinline bool marker_permutation_visible(byte const *region_permutations, s_render_model_marker const *marker)
{
	bool result = true;

	if (region_permutations && marker->region_index != NONE)
	{
		result = region_permutations[marker->region_index] == (byte)marker->permutation_index;
	}
	return result;
}

/* fills up to count markers of the group, each placed on its node's matrix;
   returns how many it filled */
// @retail 0x1d8f50
short function_1d8f50(long marker_group_index, long render_model_index, byte const *region_permutations,
	long const *node_remapping, transform4x3f const *field_50, bool mirrored, s_object_marker *markers, long count)
{
	long result = 0;

	if (marker_group_index != NONE)
	{
		s_render_model_marker_group *group = &render_model_view_get(render_model_index)->marker_groups[marker_group_index];
		long i;

		for (i = 0; i < group->marker_count; i++)
		{
			s_render_model_marker *marker = &group->markers[i];

			if (marker_permutation_visible(region_permutations, marker))
			{
				s_object_marker *out;
				long node_index;

				if (result >= count)
				{
					break;
				}
				node_index = marker->node_index;
				out = markers++;
				result++;
				if (node_remapping)
				{
					node_index = node_remapping[node_index];
				}
				out->node_index = (short)node_index;
				function_1421b0(&out->node_matrix, &marker->translation, &marker->rotation);
				function_142a60(&field_50[out->node_index], &out->node_matrix, &out->matrix);
				out->unknown6c = marker->scale;
				if (mirrored)
				{
					out->matrix.left.i = 0.0f - out->matrix.left.i;
					out->matrix.left.j = 0.0f - out->matrix.left.j;
					out->matrix.left.k = 0.0f - out->matrix.left.k;
				}
			}
		}
	}
	return result;
}

/* function_1d8f50 for the group with the name */
// @retail 0x1d90b0
short function_1d90b0(long render_model_index, long marker_name, byte const *region_permutations, long model_index,
	long const *node_remapping, long node_count, transform4x3f const *field_50, bool mirrored, s_first_person_marker *markers,
	long count)
{
	return function_1d8f50(function_1d8f00(render_model_index, marker_name), render_model_index, region_permutations,
		node_remapping, field_50, mirrored, (s_object_marker *)markers, count);
}

/* moves the node and its parent and grandparent so the marker on the node
   reaches the target (weighted toward the node's own position), then carries
   the node's children along */
// @retail 0x1d90e0
void function_1d90e0(long render_model_index, transform4x3f *nodes, long node_index, transform4x3f const *marker_matrix,
	transform4x3f const *target_matrix, real weight, long node_count)
{
	s_render_model_view *definition = render_model_view_get(render_model_index);
	short parent_node_index = definition->nodes[node_index].parent_node_index;

	if (parent_node_index != NONE)
	{
		short grandparent_node_index = definition->nodes[parent_node_index].parent_node_index;

		if (grandparent_node_index != NONE)
		{
			transform4x3f target;
			transform4x3f node_inverse;
			transform4x3f delta;
			transform4x3f *node = &nodes[node_index];

			function_141590(marker_matrix, &target);
			function_142a60(target_matrix, &target, &target);
			function_141590(node, &node_inverse);
			if (!(fabs(weight - 1.0f) < k_real_epsilon))
			{
				target.position.x = node->position.x * (1.0f - weight) + target.position.x * weight;
				target.position.y = node->position.y * (1.0f - weight) + target.position.y * weight;
				target.position.z = node->position.z * (1.0f - weight) + target.position.z * weight;
			}
			function_120220(&nodes[parent_node_index], &nodes[grandparent_node_index], &target, node);
			function_142a60(node, &node_inverse, &delta);
			render_model_build_child_node_matrices((s_render_model_definition const *)definition, &delta, node_index, node_count,
				nodes);
		}
	}
}
