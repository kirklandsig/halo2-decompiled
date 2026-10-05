// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2963F0.CPP: the search of a graph's weapon types with the "any"
   names as fallbacks */

#include "unknown_11c920.h"
#include "unknown_1dacb0.h"

#define ANY_NAME 0x30000d9

// @retail 0x2963f0
s_graph_weapon_type *graph_weapon_type_iterate(s_graph_weapon_type_iterator *iterator, long *found_mode,
	long *found_weapon_class, long *found_weapon_type)
{
	s_graph_weapon_type *result = NULL;

	do
	{
		if (iterator->step >= 8)
		{
			break;
		}
		if (!iterator->mode_entry)
		{
			if (iterator->step & 4)
			{
				if (iterator->mode == ANY_NAME)
				{
					goto mode_found;
				}
				*found_mode = ANY_NAME;
			}
			else
			{
				*found_mode = iterator->mode;
			}
			iterator->mode_entry = (s_graph_mode_entry *)function_1dd560((s_sorted_array *)&iterator->graph->mode_count,
				*found_mode, 0x14);
			iterator->class_entry = NULL;
		}
	mode_found:
		if (iterator->mode_entry)
		{
			if (!iterator->class_entry)
			{
				if (iterator->step & 2)
				{
					if (iterator->weapon_class == ANY_NAME)
					{
						goto class_found;
					}
					*found_weapon_class = ANY_NAME;
				}
				else
				{
					*found_weapon_class = iterator->weapon_class;
				}
				iterator->class_entry = (s_graph_mode_entry *)function_1dd560(
					(s_sorted_array *)&iterator->mode_entry->child_count, *found_weapon_class, 0x14);
			}
		class_found:
			if (iterator->class_entry)
			{
				if (iterator->step & 1)
				{
					if (iterator->weapon_type == ANY_NAME)
					{
						goto type_found;
					}
					*found_weapon_type = ANY_NAME;
				}
				else
				{
					*found_weapon_type = iterator->weapon_type;
				}
				result = (s_graph_weapon_type *)function_1dd560((s_sorted_array *)&iterator->class_entry->child_count,
					*found_weapon_type, 0x34);
			type_found:
				iterator->step++;
				if (!(iterator->step & 1))
				{
					iterator->class_entry = NULL;
					if (!(iterator->step & 2))
					{
						iterator->mode_entry = NULL;
					}
				}
			}
			else
			{
				iterator->step = (iterator->step + 2) & ~1;
				if (!(iterator->step & 2))
				{
					iterator->mode_entry = NULL;
				}
			}
		}
		else
		{
			iterator->step = (iterator->step + 4) & ~3;
			iterator->class_entry = NULL;
		}
	}
	while (!result);
	return result;
}
