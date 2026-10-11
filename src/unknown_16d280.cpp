// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_16D280.CPP: render model queries (0x16d280..0x16e2b3), the rest of
   the models file that unknown_16d180.cpp starts: node lookups, default
   orientations and node matrices built down the node hierarchy */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "effects.h"

/* a render model node (0x60 bytes) */
struct s_render_model_node
{
	long name;
	short parent_node_index;
	short first_child_node_index;
	short next_sibling_node_index;
	short unknown0a;
	point3f default_translation;
	quaternionf default_rotation;
	byte unknown28[0x60 - 0x28];
};

/* a render model's marker group (16 bytes) */
struct s_render_model_marker_group
{
	byte unknown00[5];
	char index;
	byte unknown06[0x10 - 6];
};

/* an entry of the block at +0x78 (0x5c bytes) */
struct s_render_model_named_entry
{
	long name;
	byte unknown04[0x5c - 4];
};

struct s_render_model_definition
{
	byte unknown00[4];
	long model_index;
	byte unknown08[0x48 - 0x8];
	long node_count;
	s_render_model_node *nodes;
	byte unknown50[0x70 - 0x50];
	long marker_group_count;
	s_render_model_marker_group *marker_groups;
	long named_entry_count;
	s_render_model_named_entry *named_entries;
};

/* an orientation: a quaternion, a translation and a scale (0x20 bytes;
   unknown_141590.cpp) */
struct rigid_transform_scaled
{
	quaternionf rotation;
	point3f position;
	real scale;
};

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

static inline s_render_model_definition *render_model_get(long render_model_index)
{
	return (s_render_model_definition *)g_4e3b44[render_model_index & 0xffff].bytes;
}

// @retail 0x16d890
long render_model_find_marker_group(long render_model_index, long index)
{
	s_render_model_definition *definition = render_model_get(render_model_index);
	long result = NONE;
	long i;

	for (i = 0; i < definition->marker_group_count; i++)
	{
		if (definition->marker_groups[i].index == index)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x16d8d0
void __stdcall render_model_build_child_node_matrices(s_render_model_definition const *definition, transform4x3f const *parent_matrix,
	long node_index, long node_count, transform4x3f *field_50)
{
	long child_index = definition->nodes[node_index].first_child_node_index;

	while (child_index >= 0 && child_index < definition->node_count && child_index < node_count)
	{
		s_render_model_node const *child = &definition->nodes[child_index];

		function_142a60(parent_matrix, &field_50[child_index], &field_50[child_index]);
		if (child->first_child_node_index != NONE)
		{
			render_model_build_child_node_matrices(definition, parent_matrix, child_index, node_count, field_50);
		}
		child_index = child->next_sibling_node_index;
	}
}

// @retail 0x16d940
void render_model_get_default_orientations(s_render_model_definition const *definition, rigid_transform_scaled *orientations)
{
	long i;

	for (i = 0; i < definition->node_count; i++)
	{
		s_render_model_node const *node = &definition->nodes[i];

		orientations[i].rotation = node->default_rotation;
		orientations[i].position = node->default_translation;
		orientations[i].scale = 1.0f;
	}
}

// @retail 0x16da90
long render_model_find_named_entry(long render_model_index, long name)
{
	long result = NONE;

	if (render_model_index != NONE)
	{
		s_render_model_definition *definition = render_model_get(render_model_index);
		long i;

		for (i = 0; i < definition->named_entry_count; i++)
		{
			if (definition->named_entries[i].name == name)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x16ddf0
void *render_model_get_model_definition(long render_model_index)
{
	return g_4e3b44[render_model_get(render_model_index)->model_index & 0xffff].bytes;
}

void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);

#define MACRO_D9C4AE 255
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

// @retail 0x16d9c0
void render_model_build_node_matrices(vector3f const *forward, vector3f const *up, point3f const *position,
	s_render_model_definition const *definition, transform4x3f *field_50, rigid_transform_scaled const *orientations)
{
	transform4x3f local_matrix;
	transform4x3f root_matrix;
	long local_03e996[MACRO_D9C4AE];

	function_1420f0(&root_matrix, position, forward, up);
	if (definition->node_count > 0)
	{
		long read_index = 0;
		long write_index;

		local_03e996[0] = 0;
		write_index = 1;
		do
		{
			long node_index = local_03e996[read_index++];
			s_render_model_node const *node = &definition->nodes[node_index];
			transform4x3f const *parent_matrix = node_index == 0 ? &root_matrix : &field_50[node->parent_node_index];

			function_1421f0(&local_matrix, &orientations[node_index]);
			function_142a60(parent_matrix, &local_matrix, &field_50[node_index]);
			if (node->next_sibling_node_index != NONE)
			{
				local_03e996[write_index++] = node->next_sibling_node_index;
			}
			if (node->first_child_node_index != NONE)
			{
				local_03e996[write_index++] = node->first_child_node_index;
			}
		}
		while (read_index != write_index);
	}
}

/* a weighted choice of a permutation (0x1c bytes) */
struct s_16dce0_choice
{
	long permutation_index;
	long index;
	long name;
	long unknown0c;
	long unknown10;
	long unknown14;
	real weight;
};

// @retail 0x16dce0
long function_16dce0(long count, s_16dce0_choice const *choices)
{
	long result = NONE;

	if (count == 0)
	{
		result = NONE;
	}
	else if (count == 1)
	{
		result = 0;
	}
	else if (count > 1)
	{
		real total = 0.0f;
		real random;
		real sum;
		long i;

		for (i = 0; i < count; i++)
		{
			total = choices[i].weight + total;
		}
		random = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
		random *= total;
		sum = 0.0f;
		for (i = 0; i < count; i++)
		{
			sum = choices[i].weight + sum;
			if (sum >= random || i == count - 1)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

/* a model variant's choices for one region (0x20 bytes each); each holds
   weighted alternatives (0x18 bytes each) */
struct s_model_variant_alternative
{
	byte unknown00[4];
	char permutation_index;
	byte unknown05;
	word name;
	byte unknown08[4];
	long unknown0c;
	long unknown10;
	real weight;
};

struct s_model_variant_permutation
{
	byte unknown00[4];
	char permutation_index;
	byte unknown05[3];
	real weight;
	long alternative_count;
	s_model_variant_alternative *alternatives;
	byte unknown14[0x20 - 0x14];
};

struct s_type_f83fc7
{
	byte unknown00[8];
	long permutation_count;
	s_model_variant_permutation *permutations;
	byte unknown10[0x14 - 0x10];
};

/* the permutations of a variant's region that match a name, or a random
   alternative of each */
// @retail 0x16dad0
void model_variant_region_get_choices(s_type_f83fc7 const *region, long permutation_index, long name, long value,
	bool random, long maximum_count, long *count, s_16dce0_choice *choices)
{
	long passes = 1;

	do
	{
		long i;

		passes--;
		for (i = 0; i < region->permutation_count; i++)
		{
			if (permutation_index == NONE || permutation_index == i)
			{
				s_model_variant_permutation const *permutation = &region->permutations[i];
				long choice_name = name;
				byte choice_value = (byte)value;
				long choice_permutation_index;
				long unknown10 = 0;
				long unknown0c = NONE;

				if (random && permutation->alternative_count > 0)
				{
					real random_value = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
					real sum = 0.0f;
					long j;

					for (j = 0; j < permutation->alternative_count; j++)
					{
						s_model_variant_alternative const *alternative = &permutation->alternatives[j];
						real next_sum = alternative->weight + sum;

						if (next_sum > random_value)
						{
							unknown0c = alternative->unknown0c;
							unknown10 = alternative->unknown10;
							choice_permutation_index = alternative->permutation_index;
							choice_name = alternative->name;
							choice_value = alternative->unknown05;
							goto found;
						}
						sum = next_sum;
					}
					choice_permutation_index = permutation->permutation_index;
				}
				else if (!name && !value)
				{
					choice_permutation_index = permutation->permutation_index;
				}
				else
				{
					long j;

					for (j = 0; j < permutation->alternative_count; j++)
					{
						s_model_variant_alternative const *alternative = &permutation->alternatives[j];

						if (alternative->name == name && alternative->unknown05 == value)
						{
							unknown0c = alternative->unknown0c;
							unknown10 = alternative->unknown10;
							choice_permutation_index = alternative->permutation_index;
							goto found;
						}
					}
					continue;
				}
found:
				if (*count < maximum_count)
				{
					choices[*count].permutation_index = choice_permutation_index;
					choices[*count].index = i;
					choices[*count].name = choice_name;
					choices[*count].unknown0c = choice_value;
					choices[*count].unknown10 = unknown0c;
					choices[*count].unknown14 = unknown10;
					choices[*count].weight = permutation->weight;
					(*count)++;
				}
			}
		}
		if (*count == 0 && value)
		{
			passes++;
			value = 0;
		}
	}
	while (passes);
}

/* the render model's regions (0x10 bytes each) and their permutations
   (8 bytes each) */
struct s_render_model_permutation
{
	long name;
	byte flags;
	byte unknown05[3];
};

struct s_render_model_region
{
	byte unknown00[8];
	long permutation_count;
	s_render_model_permutation *permutations;
};

/* the model's variants (0x38 bytes each) */
struct s_model_variant
{
	byte unknown00[4];
	char region_indices[16];
	byte unknown14[4];
	s_type_f83fc7 *regions;
	byte unknown1c[0x38 - 0x1c];
};

struct s_16d280_render_model
{
	byte unknown00[0x54];
	s_model_variant *variants;
	byte unknown58[0x70 - 0x58];
	long region_count;
	s_render_model_region *regions;
};

/* the permutation chosen for a region (8 bytes) */
struct s_region_permutation_choice
{
	char permutation_index;
	byte name;
	byte unknown02;
	byte unknown03;
	long unknown04;
};

static inline void permutation_choice_add_default(s_16dce0_choice *choice, long permutation_index)
{
	choice->permutation_index = permutation_index;
	choice->index = NONE;
	choice->name = 0;
	choice->unknown0c = 0;
	choice->unknown10 = NONE;
	choice->unknown14 = 0;
	choice->weight = 1.0f;
}

// @retail 0x16d280
void render_model_choose_permutations(long render_model_index, long variant_index, char *permutation_indices,
	s_region_permutation_choice *region_choices, dword fixed_region_mask)
{
	s_16d280_render_model *definition = (s_16d280_render_model *)g_4e3b44[render_model_index & 0xffff].bytes;
	long region_index;

	for (region_index = 0; region_index < definition->region_count; region_index++)
	{
		permutation_indices[region_index] = NONE;
		region_choices[region_index].permutation_index = NONE;
		region_choices[region_index].name = 0;
		region_choices[region_index].unknown02 = 0;
		region_choices[region_index].unknown04 = NONE;
	}
	for (region_index = 0; region_index < definition->region_count; region_index++)
	{
		s_render_model_region *region = &definition->regions[region_index];
		s_16dce0_choice choices[32];
		long count = 0;

		if (variant_index != NONE && render_model_index != NONE && region_index != NONE)
		{
			s_model_variant *variant = &((s_16d280_render_model *)g_4e3b44[render_model_index & 0xffff].bytes)->variants[variant_index];
			long variant_region_index = variant->region_indices[region_index];

			if (variant_region_index != NONE)
			{
				model_variant_region_get_choices(&definition->variants[variant_index].regions[variant_region_index], NONE, 0, 0,
					true, NUMBEROF(choices), &count, choices);
			}
		}
		if (count == 0)
		{
			long permutation_index;

			for (permutation_index = 0; permutation_index < region->permutation_count; permutation_index++)
			{
				if (!(region->permutations[permutation_index].flags & 1))
				{
					permutation_choice_add_default(&choices[count++], permutation_index);
				}
			}
			if (count == 0 && region->permutation_count > 0)
			{
				permutation_choice_add_default(&choices[0], 0);
				count = 1;
			}
		}
		if (!(fixed_region_mask & (1 << region_index)))
		{
			long choice_index = function_16dce0(count, choices);

			if (choice_index != NONE)
			{
				s_16dce0_choice *choice = &choices[choice_index];

				permutation_indices[region_index] = (char)choice->permutation_index;
				region_choices[region_index].permutation_index = (char)choice->index;
				region_choices[region_index].unknown02 = (byte)choice->unknown0c;
				region_choices[region_index].name = (byte)choice->name;
			}
		}
	}
}

__declspec(noinline) void effect_parameters_initialize(s_effect_parameters *parameters);
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);
void function_177260(long effect_index, bool flag);

/* changes the permutations of an object's regions to the ones its variant
   offers under a name (and a bit of the regions' state), starting and
   stopping the permutations' effects */
// @retail 0x16d660
bool object_change_region_permutations(long object_index, long render_model_index, long variant_index, long region_filter,
	long name, bool force, long bit_index, short bit_mask, char *permutation_indices, s_region_permutation_choice *region_choices)
{
	s_16d280_render_model *definition = (s_16d280_render_model *)g_4e3b44[render_model_index & 0xffff].bytes;
	bool result = false;
	long region_index;

	for (region_index = 0; region_index < definition->region_count; region_index++)
	{
		s_region_permutation_choice *region_choice = &region_choices[region_index];
		long wanted_name;
		dword value;

		if (name == NONE)
		{
			wanted_name = (char)region_choice->name;
		}
		else
		{
			wanted_name = name;
		}
		value = region_choice->unknown02;
		if (bit_index != NONE)
		{
			if (bit_mask & (1 << region_index))
			{
				value |= 1 << bit_index;
			}
			else
			{
				value &= ~(1 << bit_index);
			}
		}
		if ((wanted_name >= (char)region_choice->name || force) && (region_filter == NONE || region_filter == region_index))
		{
			s_16dce0_choice choices[32];
			long variant_region_index = NONE;
			long count;
			long choice_index;

			if (render_model_index != NONE && variant_index != NONE && region_index != NONE)
			{
				variant_region_index = ((s_16d280_render_model *)g_4e3b44[render_model_index & 0xffff].bytes)->
					variants[variant_index].region_indices[region_index];
			}
			count = 0;
			if (variant_region_index != NONE)
			{
				model_variant_region_get_choices(&definition->variants[variant_index].regions[variant_region_index],
					region_choice->permutation_index, wanted_name, value, false, NUMBEROF(choices), &count, choices);
			}
			choice_index = function_16dce0(count, choices);
			if (choice_index != NONE)
			{
				s_16dce0_choice *choice = &choices[choice_index];

				permutation_indices[region_index] = (char)choice->permutation_index;
				region_choice->permutation_index = (char)choice->index;
				if ((char)region_choice->name != choice->name)
				{
					if (region_choice->unknown04 != NONE)
					{
						function_177260(region_choice->unknown04, true);
					}
					if (choice->unknown10 != NONE)
					{
						s_effect_parameters parameters;

						effect_parameters_initialize(&parameters);
						parameters.unknown30 = choice->unknown14;
						parameters.tag_index = choice->unknown10;
						parameters.object_index = object_index;
						parameters.unknown34 = 0x30005a6;
						parameters.unknown38 = 0x30005a6;
						parameters.flags = 3;
						region_choice->unknown04 = effect_new_from_parameters(&parameters);
					}
				}
				region_choice->name = (byte)choice->name;
				region_choice->unknown02 = (byte)choice->unknown0c;
				result = true;
			}
		}
	}
	return result;
}
