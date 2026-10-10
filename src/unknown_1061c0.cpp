#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "data_array.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

struct s_entry
{
	byte unknown00[0x14];
	dword flags;
	byte unknown18[4];
};

struct s_entry_tag_data
{
	byte unknown00[0x68];
	long count;
	s_entry *entries;
};

struct s_parent_tag_data
{
	byte unknown00[0x38];
	long entry_tag_index;
	byte unknown3c[0x240 - 0x3c];
	dword flags;
	byte unknown244[0x2c8 - 0x244];
	long count;
	byte *entries;
};

struct s_slot
{
	long object_index;
	long entry_index;
	byte unknown08[0x14];
	real value;
};

struct s_object
{
	long tag_index;
	byte unknown04[0xa6];
	byte type;
	byte unknownab[0xe8 - 0xab];
	real value_e8;
	byte unknownec[4];
	real value_f0;
	byte unknownf4[0x177 - 0xf4];
	byte byte177;
	byte unknown178[0x194 - 0x178];
	long object_index194;
	byte unknown198[0x1c8 - 0x198];
	s_slot slot;
	byte unknown1e8[0x20c - 0x1e8];
	byte state20c;
	byte unknown20d[0x24c - 0x20d];
	long time;
};

struct s_entry_pair
{
	long object_index;
	long entry_index;
};

struct s_entry_view
{
	byte unknown00[6];
	short s6;
	byte unknown08[0x10];
	short s18;
};

s_object *function_bae20(long object_index, dword type_mask);
s_object *function_badc0(long object_index, dword type_mask);

#define OBJECT(index) (((s_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define TAG_DATA(index) (g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x1061c0
bool function_1061c0(long object_index, long *out_index, byte *out_entry)
{
	s_slot *slot = &((s_object *)OBJECT(object_index))->slot;
	bool result = false;

	*out_index = NONE;
	*out_entry = 0xff;
	if (slot->object_index != NONE && slot->value >= 1.0f)
	{
		long slot_object_index = slot->object_index;
		s_object *slot_object = function_bae20(slot_object_index, NONE);

		if (slot_object && slot->entry_index != NONE)
		{
			long tag_index = slot_object->tag_index;
			s_parent_tag_data *parent = (s_parent_tag_data *)TAG_DATA(tag_index);

			if (parent->entry_tag_index != NONE)
			{
				s_entry_tag_data *tag = (s_entry_tag_data *)TAG_DATA(parent->entry_tag_index);

				if (slot->entry_index >= 0 && slot->entry_index < tag->count && (tag->entries[slot->entry_index].flags & 1))
				{
					*out_index = slot_object_index;
					result = true;
					*out_entry = (byte)slot->entry_index;
				}
			}
		}
	}
	return result;
}

// @retail 0x106280
bool function_106280(long object_index, long *out_index, byte *out_entry)
{
	s_object *unit = (s_object *)OBJECT(object_index);
	s_game_time_globals *time = g_510c54;
	long now = time->game_time;
	bool result = false;
	long ticks;
	real r = (real)time->field_2_3;

	__asm
	{
		fld r
		fistp ticks
	}
	if (now - unit->time < ticks)
	{
		long other = unit->object_index194;

		if (other != NONE && function_bae20(other, NONE))
		{
			*out_index = other;
			*out_entry = unit->byte177;
			result = true;
		}
	}
	return result;
}

// @retail 0x106320
bool function_106320(s_entry_pair *pair)
{
	bool result = false;
	bool positive = false;

	if (pair->object_index != NONE)
	{
		s_object *unit = function_badc0(pair->object_index, NONE);

		if (unit)
		{
			positive = unit->value_f0 > 0.0f && unit->value_e8 > 0.0f;

			s_parent_tag_data *parent = (s_parent_tag_data *)TAG_DATA(unit->tag_index);
			long one = 1;

			if ((one << unit->type) & 1)
			{
				if ((parent->flags & 2) && positive || (parent->flags & 4))
					result = true;
			}
			if (parent->entry_tag_index != NONE && pair->entry_index != NONE)
			{
				s_entry_tag_data *tag = (s_entry_tag_data *)TAG_DATA(parent->entry_tag_index);

				if (pair->entry_index < tag->count)
				{
					dword flags = tag->entries[pair->entry_index].flags;

					if ((flags & 2) && positive || (flags & 0x10))
						return true;
				}
			}
			return result;
		}
	}
	return false;
}

// @retail 0x106400
bool function_106400(long object_index, bool flag)
{
	s_object *unit = (s_object *)OBJECT(object_index);
	s_parent_tag_data *parent = (s_parent_tag_data *)TAG_DATA(unit->tag_index);
	bool result = false;

	if (parent->count > 0)
	{
		s_entry_view *entries = (s_entry_view *)parent->entries;

		if (flag || unit->state20c == 2)
		{
			if (entries->s6 == 2 && entries->s18 == 1)
				result = true;
		}
	}
	return result;
}

// @retail 0x106460
void device_groups_initialize()
{
	g_4e0328.groups = data_new("device groups", 0x400, 12, 0, g_510c2c);
	g_4e0328.initialized = false;
}

// @retail 0x106490
void device_groups_dispose()
{
	if (g_4e0328.initialized)
	{
		if (g_4e0328.groups)
		{
			data_dispose(g_4e0328.groups);
			g_4e0328.groups = 0;
		}
		g_4e0328.initialized = false;
	}
}
