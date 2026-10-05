// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
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
