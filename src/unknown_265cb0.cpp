// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_265CB0.CPP: the props an actor knows (0x265cb0..) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "props.h"

real __stdcall function_265d30(long actor_index, long prop_index);

// @retail 0x265cb0
void function_265cb0(long actor_index)
{
	long prop_index = actor_get(actor_index)->first_prop_index;

	for (;;)
	{
		s_prop_node_view *node;
		long current_index;
		s_prop_view_fields *view;

		if (prop_index == NONE)
		{
			break;
		}
		node = prop_node_get(prop_index);
		current_index = prop_index;
		prop_index = node->next_index;
		view = prop_node_view(node);
		if (view)
		{
			view->unknown3c = function_265d30(actor_index, current_index);
		}
	}
}

// @retail 0x263d30
long function_263d30(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result;
	if (actor->unknown086 >= 3)
		result = 2;
	else
		result = actor->unknown084 >= 4;
	return result;
}

PRIVATE __forceinline s_type_f95cd3 *actor_tracked_prop_view(long prop_index)
{
	s_prop_node_view *node = prop_node_get(prop_index);
	s_type_f95cd3 *result = NULL;
	if (node->view_index != NONE)
	{
		byte *base = g_502414->data + (node->view_index & 0xffff) * sizeof(s_type_e5ff81);
		if (base)
			result = (s_type_f95cd3 *)(base + 0x70);
	}
	return result;
}

struct s_actor_prop_iterator
{
	long index;
	long next;
};

PRIVATE __forceinline s_prop_node_view *actor_next_prop(s_actor_prop_iterator *iterator)
{
	s_prop_node_view *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = prop_node_get(index);
		iterator->index = index;
		iterator->next = node->next_index;
	}
	return node;
}

// @retail 0x265bb0
void function_265bb0(long actor_index)
{
	s_actor_prop_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = actor_next_prop(&iterator)) != NULL)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(iterator.index);
		if (view)
		{
			view->unknown2a = false;
			*(short *)((byte *)view + 0x28) = NONE;
		}
	}
}

struct s_prop_thresholds
{
	byte unknown000[0x158];
	real actor_threshold;
	real object_threshold;
};

struct s_prop_threshold_table
{
	byte unknown000[0xc8];
	long count;
	s_prop_thresholds *entries;
};

long function_1e4990(long index);

// @retail 0x267550
bool function_267550(long actor_index)
{
	byte *definition = (byte *)function_1e4990(actor_get(actor_index)->unknown054);
	bool result = false;
	if (definition && ((s_prop_threshold_table *)g_4e034c)->count > 0)
		result = *(real *)(definition + 8) >= ((s_prop_threshold_table *)g_4e034c)->entries->actor_threshold;
	return result;
}

struct s_equipped_object_view
{
	long tag_index;
	byte unknown004[0x212 - 4];
	char selected;
	byte unknown213[5];
	long objects[4];
};

/* The held-object lookup rereads the pool's data field. */
// @retail 0x2675f0
bool function_2675f0(long object_index)
{
	s_equipped_object_view *object = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[object_index & 0xffff].object;
	long held_index = NONE;
	short selected = object->selected;
	if (selected != NONE)
		held_index = object->objects[selected];
	bool result = false;
	if (held_index != NONE && ((s_prop_threshold_table *)g_4e034c)->count > 0)
	{
		s_prop_thresholds *thresholds = ((s_prop_threshold_table *)g_4e034c)->entries;
		s_equipped_object_view *held = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[held_index & 0xffff].object;
		byte *definition = g_4e3b44[held->tag_index & 0xffff].bytes;
		result = *(real *)(definition + 0x238) >= thresholds->object_threshold;
	}
	return result;
}

// @retail 0x2684f0
short function_2684f0(s_actor_view *actor)
{
	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_view_fields_get(actor->prop_index);
		if (view)
			return view->unknown00;
	}
	return 0;
}

// @retail 0x2675b0
bool function_2675b0(long node_index)
{
	s_prop_node_view *node = prop_node_get(node_index);
	s_type_76cf92 *prop = prop_get(node->unknown08);
	bool result = false;
	if (prop->actor_index != NONE)
		result = function_267550(prop->actor_index);
	return result;
}

/* These wrappers read the handle after resolving the complete record address. */
// @retail 0x267680
bool function_267680(long node_index)
{
	s_prop_node_view volatile *node = prop_node_get(node_index);
	long object_index = node->object_index;
	long type = ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].type;
	bool result = false;
	if ((1 << type) & 3)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2676d0
bool function_2676d0(long actor_index)
{
	s_actor_view volatile *actor = actor_get(actor_index);
	long object_index = actor->unknown018;
	bool result = false;
	if (object_index != NONE)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2672e0
void function_2672e0(s_type_f95cd3 *view, s_prop_node_view const *node)
{
	s_prop_node_view const *const *node_reference = &node;
	if (view->unknown54 > 1.0f)
		view->unknown54 = 1.0f;
	real value = 1.0f;
	if (!view->unknown2a)
	{
		if ((*node_reference)->unknown27 < 2 || view->unknown39 == 4)
			value = 0.0f;
		else if (view->unknown39 == 3)
			value = 0.3f;
		else if (view->unknown39 == 2)
			value = 0.6f;
		else
			value = 0.8f;
	}
	if (!(value >= view->unknown54))
		value = value * (1.0f - 0.995f) + view->unknown54 * 0.995f;
	view->unknown54 = value;
}


PRIVATE inline s_type_f95cd3 *countdown_prop_view(s_prop_datum *node)
{
	s_type_f95cd3 *result = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking)
			result = &tracking->view;
	}
	return result;
}

// @retail 0x263670
void function_263670(s_prop_datum *node)
{
	short *countdown = (short *)node->unknown1e;
	if (*countdown > 0)
	{
		--*countdown;
		if (*countdown == 0)
			node->unknown1c = NONE;
	}
	s_type_5cfb45 *state = function_25d690(node);
	s_type_f95cd3 *view = countdown_prop_view(node);
	if (state->unknown5e)
		++*(short *)((byte *)state + 0x5c);
	else
		*(short *)((byte *)state + 0x5c) = 0;
	if (view)
	{
		short *age = (short *)((byte *)view + 0x28);
		if (*age != NONE)
		{
			++*age;
			if ((real)*age * g_510c54->rate >= 1.5f)
			{
				view->unknown2a = false;
				*age = NONE;
			}
		}
		short *remaining = (short *)view->unknowna0;
		if (*remaining > 0)
			--*remaining;
		short *duration = (short *)view->unknown08;
		if (node->unknown27 >= 1)
		{
			if (*duration < 0x7fff)
				++*duration;
		}
		else
			*duration = 0;
	}
}


struct s_prop_object_links_view
{
	byte unknown00[0x10a];
	word unknown10a_0 : 2;
	word has_links : 1;
	word unknown10a_3 : 13;
	byte unknown10c[0x120 - 0x10c];
	short size;
	short offset;
};

struct s_prop_object_link
{
	byte unknown00[4];
	word packed_index;
	byte unknown06[2];
};

/* The optional output pointer stays on the stack until the scan is complete. */
// @retail 0x2651e0
bool function_2651e0(long object_index, short *volatile output_index)
{
	s_prop_object_links_view *object = (s_prop_object_links_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	bool result = false;
	short index = NONE;
	if (TEST_FIELD_BIT(object->has_links))
	{
		long count = (dword)(long)object->size >> 3;
		s_prop_object_link *links = (s_prop_object_link *)((byte *)object + object->offset);
		index = 0x7fff;
		for (short i = 0; i < count; i++)
		{
			dword packed = links[i].packed_index;
			if ((packed & 0xfff8) > 0)
			{
				word candidate = links[i].packed_index;
				if ((word)(candidate >> 3) < (word)index)
				{
					index = candidate >> 3;
					result = true;
				}
				break;
			}
		}
	}
	short *output = output_index;
	if (output)
	{
		if (result)
			*output = index;
		else
			*output = NONE;
	}
	return result;
}
