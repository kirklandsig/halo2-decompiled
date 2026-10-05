// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_20fe20.h"
#include "unknown_11cc90.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_2626b0.h"

/* Location records share one owner and type, with a bitmap of active entries. */
struct s_location_record_view
{
	short salt;
	short type;
	long owner;
	short count;
	short users;
	byte unknown0c[0x40 - 0x0c];
	dword active[1];
	s_262b40_result entries[32];
	byte unknown444[0x40];
};

struct s_location_record_actor_view
{
	byte unknown000[0x3f4];
	long record_index;
	byte unknown3f8[0x888 - 0x3f8];
};

// @retail 0x26db50
long function_26db50(long owner, short type)
{
	long result = NONE;
	long const *owner_reference = &owner;
	s_record_pool_iterator iterator;
	iterator.data = g_51eca4;
	iterator.index = NONE;
	s_location_record_view *record;
	while ((record = (s_location_record_view *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (record->owner == *owner_reference && record->type == type)
		{
			result = iterator.datum_index;
			break;
		}
	}
	return result;
}

// @retail 0x26dbd0
void function_26dbd0(long record_index, long actor_index)
{
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	s_location_record_actor_view *actor = (s_location_record_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_location_record_actor_view));
	if (actor->record_index != record_index)
	{
		actor->record_index = record_index;
		record->users++;
	}
}

// @retail 0x26e030
s_262b40_result *__stdcall function_26e030(s_reference reference)
{
	s_262b40_result *result = NULL;
	long index = reference.unknown0;
	s_location_record_view *record = &((s_location_record_view *)g_51eca4->data)[reference.unknown2 & 0x7fff];
	if (index >= 0 && index < record->count && (record->active[index >> 5] & (1 << (index & 0x1f))))
		result = &record->entries[index];
	return result;
}

struct s_record_object_motion_view
{
	byte unknown00[0x30];
	point3f position;
	byte unknown3c[0x88 - 0x3c];
	vector3f velocity;
};

long function_baf80(long object_index);

// @retail 0x26df40
bool function_26df40(long actor_index, long object_index, bool ignore_speed)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (object_index != NONE)
	{
		long root_index = function_baf80(object_index);
		s_record_object_motion_view *object = (s_record_object_motion_view *)((s_object_header_view *)g_4e0300->data)[root_index & 0xffff].object;
		if (ignore_speed || sqrt(length_sq3f(&object->velocity)) < 0.1f)
		{
			point3f position = object->position;
			vector3f delta;
			vector3d_from_points3d(&position, &actor->position, &delta);
			if (sqrt(delta.j * delta.j + (delta.i * delta.i + delta.k * delta.k)) < 15.0f)
				result = true;
		}
	}
	return result;
}


struct s_location_entry_view
{
	union
	{
		s_type_c3b527 location;
		struct
		{
			byte unknown00[0xe];
			word flags;
		};
	};
	short field10;
	word sector;
	long owner;
	vector2f direction;
};

struct s_record_sector_map
{
	byte unknown00[0x30];
	struct s_sector_entry
	{
		word sector;
		byte unknown02[6];
	} *entries;
};

struct s_bsp3d;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
long function_14a280(s_bsp3d *bsp, point3f *point, long index);
vector2f *function_11df30(vector2f *angles, vector3f const *vector);

// @retail 0x26d9c0
bool function_26d9c0(long record_index, s_type_c3b527 const *point, short entry_index, short type, long owner, vector3f const *direction)
{
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	bool result = false;
	if (entry_index >= 0 && entry_index < 32)
	{
		if (!(record->active[entry_index >> 5] & (1UL << (entry_index & 0x1f))))
		{
			s_location_entry_view *entry = (s_location_entry_view *)&record->entries[entry_index];
			point3f position;
			function_210850(point, &position);
			position.x = g_4687b0->i * 0.05f + position.x;
			position.y = g_4687b0->j * 0.05f + position.y;
			position.z = g_4687b0->k * 0.05f + position.z;
			long sector_index = function_14a280((s_bsp3d *)g_4e0340, &position, 0);
			if (sector_index == NONE)
				return false;
			entry->sector = ((s_record_sector_map *)g_4e0348)->entries[sector_index].sector;
			if (entry->sector == (word)NONE)
				return false;
			record->active[entry_index >> 5] |= 1 << (entry_index & 0x1f);
			entry->owner = owner;
			entry->location = *point;
			entry->flags = 0;
			entry->field10 = NONE;
			if (direction)
				function_11df30(&entry->direction, direction);
			else
			{
				entry->direction.i = 0.0f;
				entry->direction.j = 0.0f;
			}
			entry->flags |= 0x40;
			long type_value = type;
			dword flags = entry->flags;
			if (type_value > 0 && type_value <= 3)
				entry->flags = (word)(flags | 0x90);
			((short *)record->unknown444)[entry_index] = type;
		}
		return true;
	}
	return result;
}
