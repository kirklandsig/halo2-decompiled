// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"

struct s_clump_view
{
	byte unknown0[0x18];
	long first_object;
	byte unknown1c[0x34];
};

struct s_clump_object_view
{
	byte unknown0[0x58];
	long value58;
	byte unknown5c[0x24];
	long next;
	byte unknown84[0x804];
};

struct s_node_view
{
	byte unknown0[0x2c];
	long next;
	byte unknown30[0x0c];
};

struct s_flag_byte
{
	byte flag0 : 1;
	byte flag1 : 1;
	byte flag2 : 1;
	byte flag3 : 1;
	byte flag4 : 1;
	byte flag5 : 1;
	byte flag6 : 1;
	byte flag7 : 1;
};

struct s_game_object
{
	byte unknown00[0x14];
	long parent;
	byte unknown18[0x92];
	char type;
	byte unknownab[0x81];
	s_flag_byte flags12c;
	byte unknown12d[0x4f];
	byte state17c;
	byte unknown17d[0x1cb];
	s_flag_byte flags348;
	byte unknown349[7];
	long time350;
	long value354;
	byte unknown358[0x84];
	byte state3dc;
};

struct s_game_object_header
{
	byte unknown0[8];
	s_game_object *object;
};


#define OBJECT(index) (((s_game_object_header *)(g_4e0300->data) + ((index) & 0xffff))->object)

// @retail 0x26bda0
void function_26bda0(long clump_index, s_iterator *iterator)
{
	s_clump_view *clump = (s_clump_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_view));
	iterator->next = clump->first_object;
}

// @retail 0x26bdd0
s_clump_object_view *function_26bdd0(s_iterator *iterator)
{
	s_clump_object_view *object = 0;
	long index = iterator->next;

	if (index != NONE)
	{
		object = (s_clump_object_view *)(g_4f55f0->data + (index & 0xffff) * sizeof(s_clump_object_view));
		iterator->index = index;
		iterator->next = object->next;
	}

	return object;
}

// @retail 0x26be00
void function_26be00(long clump_index, s_iterator *iterator)
{
	s_clump_object_view *object = (s_clump_object_view *)(g_4f55f0->data + (clump_index & 0xffff) * sizeof(s_clump_object_view));
	iterator->next = object->value58;
}

// @retail 0x26be30
s_node_view *function_26be30(s_iterator *iterator)
{
	s_node_view *node = 0;
	long index = iterator->next;

	if (index != NONE)
	{
		node = (s_node_view *)(g_502418->data + (index & 0xffff) * sizeof(s_node_view));
		iterator->index = index;
		iterator->next = node->next;
	}

	return node;
}

// @retail 0x26be60
bool function_26be60(long object_index)
{
	s_game_object *object = OBJECT(object_index);

	if (object->state3dc == 1)
	{
		return (bool)!object->flags348.flag0;
	}
	return true;
}

// @retail 0x26be90
bool function_26be90(long object_index)
{
	long root = NONE;

	while (object_index != NONE)
	{
		root = object_index;
		object_index = OBJECT(object_index)->parent;
	}

	s_game_object *object = OBJECT(root);
	long type = object->type;

	switch (type)
	{
	case 0:
		if (object->state3dc == 1)
		{
			return (bool)!object->flags348.flag0;
		}
		return true;
	case 12:
		if (object->state17c == 1)
		{
			return (bool)!object->flags12c.flag7;
		}
		return true;
	default:
		return true;
	}
}

// @retail 0x26bf10
bool function_26bf10(long object_index)
{
	bool root_check_outcome = false;
    long root = NONE;

	while (object_index != NONE)
	{
		root = object_index;
		object_index = OBJECT(object_index)->parent;
	}

	s_game_object *object = OBJECT(root);
	long type = object->type;

    switch (type)
    {
    case 0:
        if (object->value354 != NONE || object->time350 == g_510c54->game_time)
            root_check_outcome = true;
        break;
    case 12:
        if (object->state17c == 1)
            root_check_outcome = (bool)!object->flags12c.flag7;
        else
            root_check_outcome = true;
        break;
    default:
        return false;
    }
    return root_check_outcome;
}
