// @flags /O2 /Gr
/* UNKNOWN_165A20.CPP: the object lists of a visibility pass (the end of
   visibility_volume_group.cpp in the original): adding objects
   and clearing the flags of the ones a viewer's own object hides */

#include "unknown_11c920.h"
#include "globals.h"

/* a list of visible objects */
struct s_visible_object_list
{
	long maximum_count;
	word count;
	byte unknown06[2];
	short *flags;
	long *object_indices;
	long *unknown10;
	short *unknown14;
};

struct s_visibility_object_lists
{
	byte unknown00[0xc];
	s_visible_object_list *lights;
	s_visible_object_list *objects;
	s_visible_object_list *extra_objects;
	s_visible_object_list *effects;
};

/* the objects, as read here */
struct s_165a20_object
{
	long definition_index;
	dword unknown_flags : 31;
	dword hidden : 1;
	byte unknown008[0xaa - 0x8];
	byte type;
};

struct s_165a20_object_header
{
	byte unknown00[8];
	s_165a20_object *object;
};

static inline s_165a20_object *visibility_object_get(long object_index)
{
	return ((s_165a20_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* clears the shadow flags of the objects of a pass but a viewer's own */
// @retail 0x165a20
void visibility_object_lists_update_flags(s_visibility_object_lists *lists, long object_index, bool field_b4,
	dword type_mask)
{
	if (object_index != NONE)
	{
		s_165a20_object *object = visibility_object_get(object_index);
		byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;

		if (*(long *)(definition + 0x38) != NONE)
		{
			short i;

			for (i = 0; i < (short)lists->extra_objects->count; i++)
			{
				s_visible_object_list *list = lists->extra_objects;

				if (list->object_indices[i] == object_index)
				{
					short flags;

					if (*(long *)(g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes + 4) != NONE)
					{
						flags = list->flags[i];
					}
					else
					{
						flags = 0;
					}
					if (!field_b4)
					{
						flags &= ~0x1000;
					}
					list->flags[i] = flags;
				}
				else
				{
					s_165a20_object *other = visibility_object_get(list->object_indices[i]);
					short flags = list->flags[i] & ~0x2000;

					if (!(type_mask & (1 << other->type)) && !TEST_FIELD_BIT(other->hidden))
					{
						flags &= ~0x1000;
					}
					list->flags[i] = flags;
				}
			}
			for (i = 0; i < (short)lists->lights->count; i++)
			{
				lists->lights->flags[i] &= ~0x2000;
			}
			for (i = 0; i < (short)lists->effects->count; i++)
			{
				lists->effects->flags[i] &= ~0x2000;
			}
		}
		else
		{
			lists->lights->count = 0;
		}
	}
}

static inline void visible_object_list_add(s_visible_object_list *list, long object_index)
{
	if (list->count < list->maximum_count - 1)
	{
		list->object_indices[list->count] = object_index;
		list->flags[list->count] = 0;
		list->unknown10[list->count] = 0;
		list->unknown14[list->count] = NONE;
		list->count++;
	}
}

/* adds objects to a pass's two object lists */
// @retail 0x165bc0
void visibility_object_lists_add(long const *object_indices, long object_count, s_visibility_object_lists *lists,
	long const *extra_object_indices, long extra_object_count, bool append)
{
	long i;

	if (object_count > 0)
	{
		if (!append)
		{
			lists->objects->count = 0;
		}
		for (i = 0; i < object_count; i++)
		{
			visible_object_list_add(lists->objects, object_indices[i]);
		}
	}
	if (extra_object_count > 0)
	{
		if (!append)
		{
			lists->extra_objects->count = 0;
		}
		for (i = 0; i < extra_object_count; i++)
		{
			visible_object_list_add(lists->extra_objects, extra_object_indices[i]);
		}
	}
	lists->lights->count = 0;
	lists->effects->count = 0;
}
