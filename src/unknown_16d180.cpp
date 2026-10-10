#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_16d180.h"

// @flags /O2 /Gr

struct s_model_variant
{
	string_handle name;
	byte unknown04[0x34];
};

struct s_model_permutation
{
	string_handle name;
	long unknown04;
};

struct s_model_region
{
	string_handle name;
	long unknown04;
	long permutation_count;
	s_model_permutation *permutations;
};

struct s_type_d42621
{
	byte unknown00[0x50];
	long variant_count;
	s_model_variant *variants;
	byte unknown58[0x18];
	long region_count;
	s_model_region *regions;
};

// @retail 0x16d180
long function_16d180(long model_index, string_handle name)
{
	long variant_slot_found = NONE;

	if (model_index != NONE)
	{
		s_type_d42621 *model = (s_type_d42621 *)g_4e3b44[model_index & 0xffff].bytes;

		if (name == _string_id_none)
		{
			if (model->variant_count > 0)
			{
				variant_slot_found = 0;
			}
		}
		else
		{
			for (long i = 0; i < model->variant_count; i++)
			{
				if (model->variants[i].name == name)
				{
					variant_slot_found = i;
					break;
				}
			}
		}
	}

	return variant_slot_found;
}

// @retail 0x16d1d0
long function_16d1d0(long model_index, string_handle name)
{
	long result = NONE;

	if (model_index != NONE && name != _string_id_none)
	{
		s_type_d42621 *model = (s_type_d42621 *)g_4e3b44[model_index & 0xffff].bytes;

		for (long i = 0; i < model->region_count; i++)
		{
			if (model->regions[i].name == name)
			{
				result = i;
				break;
			}
		}
	}

	return result;
}

// @retail 0x16d220
long function_16d220(long region_index, long model_index, string_handle name)
{
	long result = NONE;

	if (model_index != NONE && region_index != NONE && name != _string_id_none)
	{
		s_type_d42621 *model = (s_type_d42621 *)g_4e3b44[model_index & 0xffff].bytes;
		s_model_region *region = &model->regions[region_index];

		for (long i = 0; i < region->permutation_count; i++)
		{
			if (region->permutations[i].name == name)
			{
				result = i;
				break;
			}
		}
	}

	return result;
}
