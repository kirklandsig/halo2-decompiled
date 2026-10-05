#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"

// @flags /O2 /Gr /arch:SSE

struct s_actor_limit_view
{
	byte unknown000[0x706];
	short value706;
	byte unknown708[0x720 - 0x708];
	short value720;
	byte unknown722[0x888 - 0x722];
};

struct s_actor_channel_view
{
	byte unknown000[0x18];
	long object_index;
	byte unknown01c[0x6ce - 0x1c];
	short channel;
	byte unknown6d0[0x888 - 0x6d0];
};

void function_10e9f0(long object_index, short channel, real value, real time);

// @retail 0x20d870
void function_20d870(long actor_index, short channel, short value)
{
	(void)&value;
	s_actor_channel_view *actor = &((s_actor_channel_view *)g_4f55f0->data)[actor_index & 0xffff];
	if (channel >= 0 && channel < 13)
	{
		function_10e9f0(actor->object_index, channel, 1.0f, 0.5f);
		actor->channel = value;
	}
}

struct s_squad_definition_view
{
	byte unknown00[0x2a];
	short definition_index;
};

struct s_squad_definition_entry
{
	byte data[0x7c];
};

struct s_squad_definition_table_view
{
	byte unknown000[0x244];
	s_squad_definition_entry *entries;
};

struct s_squad_object_state
{
	byte unknown000[0x10a];
	word unknown_bits : 2;
	word blocked : 1;
	word unused_bits : 13;
	byte unknown10c[0x346 - 0x10c];
	short state_offset;
};

struct s_squad_object_header
{
	byte unknown00[8];
	s_squad_object_state *object;
};

struct s_squad_vehicle_view
{
	byte unknown000[0x3a0];
	long squad_index;
	long next;
};

struct s_squad_vehicle_header
{
	byte unknown00[8];
	s_squad_vehicle_view *object;
};

// @retail 0x201a50
void function_201a50(long squad_index, long object_index)
{
	long *link = &squad_get(squad_index)->first_vehicle_index;
	while (*link != NONE)
	{
		long current = *link;
		s_squad_vehicle_view *object = ((s_squad_vehicle_header *)g_4e0300->data)[current & 0xffff].object;
		if (current == object_index)
		{
			*link = object->next;
			((s_squad_vehicle_header *)g_4e0300->data)[object_index & 0xffff].object->squad_index = NONE;
			break;
		}
		link = &object->next;
	}
}

struct s_squad_object_extra
{
	byte unknown00[8];
	dword unknown_bits : 18;
	dword blocked : 1;
	dword unused_bits : 13;
};

PRIVATE inline void squad_actor_begin_inline(s_squad_actor_iterator *iterator, long squad_index)
{
	if (g_4f55d0->active)
	{
		iterator->squad_index = squad_index;
		iterator->actor_index = NONE;
		iterator->next_actor_index = squad_index == NONE ? g_4f55d0->unknown14 : squad_get(squad_index)->first_actor_index;
	}
}

PRIVATE inline s_actor_datum *squad_actor_next_inline(s_squad_actor_iterator *iterator)
{
	s_actor_datum *actor = NULL;
	if (g_4f55d0->active && iterator->next_actor_index != NONE)
	{
		actor = actor_datum_get(iterator->next_actor_index);
		iterator->actor_index = iterator->next_actor_index;
		iterator->next_actor_index = actor->next_actor_index;
	}
	return actor;
}

// @retail 0x2003a0
long function_2003a0(long actor_index)
{
	s_actor_limit_view *actor = &((s_actor_limit_view *)g_4f55f0->data)[actor_index & 0xffff];
	return actor->value706 > g_510c54->field_2_3 || actor->value720 > 6;
}

// @retail 0x2013c0
bool function_2013c0(long object_index)
{
	bool result = true;
	s_squad_object_state *object = ((s_squad_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (TEST_FIELD_BIT(object->blocked) || TEST_FIELD_BIT(((s_squad_object_extra *)((byte *)object + object->state_offset))->blocked))
		result = false;
	return result;
}

// @retail 0x203240
bool function_203240(long squad_index)
{
	bool result = true;
	s_squad_actor_iterator iterator;
	squad_actor_begin_inline(&iterator, squad_index);
	s_actor_datum *actor;
	while (result && (actor = squad_actor_next_inline(&iterator)) != NULL)
		result &= *(bool *)&actor->unknown224[3];
	return result;
}

// @retail 0x2032b0
bool function_2032b0(long squad_index)
{
	bool result = true;
	s_squad_actor_iterator iterator;
	squad_actor_begin_inline(&iterator, squad_index);
	s_actor_datum *actor;
	while (result && (actor = squad_actor_next_inline(&iterator)) != NULL)
		result &= actor->unknown26c != NONE;
	return result;
}

// @retail 0x203330
s_squad_definition_entry *function_203330(s_squad_definition_view const *squad)
{
	if (squad->definition_index != NONE)
		return &((s_squad_definition_table_view *)g_4e0350)->entries[squad->definition_index];
	return NULL;
}

// @retail 0x203ed0
void function_203ed0(long actor_index, bool keep_count)
{
	if (g_4f55d0->active)
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		if (actor->squad_index != NONE)
		{
			s_squad_datum *squad = squad_get(actor->squad_index);
			long *link = &squad->first_actor_index;
			long current = *link;
			while (current != actor_index)
			{
				link = &actor_datum_get(current)->next_actor_index;
				current = *link;
			}
			*link = actor->next_actor_index;
			if (!keep_count)
				squad->actor_count--;
			actor->next_actor_index = NONE;
			actor->squad_index = NONE;
			((byte *)squad)[3] |= 1;
		}
	}
}

// @retail 0x203fb0
void function_203fb0(long actor_index)
{
	if (g_4f55d0->active)
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		long *link = &g_4f55d0->unknown14;
		long current = *link;
		while (current != actor_index)
		{
			link = &actor_datum_get(current)->next_actor_index;
			current = *link;
		}
		*link = actor->next_actor_index;
		actor->flag00a = false;
		actor->next_actor_index = NONE;
		actor->flag00b = false;
	}
}

// @retail 0x205280
void function_205280(long group_index, long squad_index)
{
	s_squad_group_datum *group = squad_group_get(group_index);
	long index = group->first_squad_index;
	if (index == NONE)
	{
		group->first_squad_index = squad_index;
	}
	else
	{
		s_squad_datum *squad;
		do
		{
			squad = squad_get(index);
			index = squad->next_squad_index;
		} while (index != NONE);
		squad->next_squad_index = (short)squad_index;
	}
}

// @retail 0x2052d0
bool function_2052d0(long squad_index, long group_index)
{
	bool result = false;
	long index = *(long *)squad_get(squad_index)->unknown6c;
	while (index != NONE)
	{
		if (index == group_index)
		{
			result = true;
			break;
		}
		index = squad_group_get(index)->parent_index;
	}
	return result;
}

struct s_squad_record_iterator
{
	s_squad_datum *squad;
	s_record_pool_iterator records;
	long squad_index;
};

PRIVATE __forceinline s_squad_datum *next_squad_record(s_squad_record_iterator *iterator)
{
	s_squad_datum *result = NULL;
	if (g_4f55d0->active)
	{
		s_record_pool *records = iterator->records.data;
		long index = data_next_absolute_index_inlined(records, iterator->records.index + 1);
		if (index != NONE)
		{
			result = (s_squad_datum *)(records->data + records->size * index);
			iterator->records.index = index;
		}
		iterator->squad = result;
	}
	return result;
}

// @retail 0x205320
void __stdcall function_205320(long group_index)
{
	long const *group_reference = &group_index;
	s_squad_record_iterator iterator;
	if (g_4f55d0->active)
	{
		iterator.records.data = g_51e9d8;
		iterator.records.index = NONE;
	}
	s_squad_datum *squad;
	while ((squad = next_squad_record(&iterator)) != NULL)
	{
		if (*(long *)((byte *)squad + 0x80) == *group_reference)
			*(long *)((byte *)squad + 0x80) = NONE;
	}
}

// @retail 0x201ad0
void function_201ad0(long squad_index, long object_index)
{
	long const *object_reference = &object_index;
	s_squad_vehicle_view *object = ((s_squad_vehicle_header *)g_4e0300->data)[*object_reference & 0xffff].object;
	if (object->squad_index != NONE)
		function_201a50(object->squad_index, *object_reference);
	if (squad_index != NONE)
	{
		s_squad_datum *squad = (s_squad_datum *)(g_51e9d8->data + (squad_index & 0xffff) * sizeof(s_squad_datum));
		((s_squad_vehicle_header *)g_4e0300->data)[*object_reference & 0xffff].object->next = squad->first_vehicle_index;
		squad->first_vehicle_index = *object_reference;
		if (!(squad_index & 0xffff0000))
			object->squad_index = (*(short *)squad << 16) | squad_index;
		else
			object->squad_index = squad_index;
	}
}

struct s_actor_position_state
{
	s_actor_position_state();
	volatile long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real field14;
	real field18;
	long field1c;
	short field20;
	short field22;
	short field24;
	byte unknown26[2];
	short field28;
	short field2a;
	short field2c;
	short field2e;
	long field30;
	long field34;
	real field38;
	short field3c;
	short field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
};

PRIVATE __forceinline void reset_actor_position(s_actor_position_state *state)
{
	state->flags = 0;
	state->position = *g_468788;
}

// @retail 0x200e60
s_actor_position_state::s_actor_position_state() : flags(0)
{
	s_actor_position_state *state = this;
	reset_actor_position(state);
	state->field10 = NONE;
	state->field14 = 0.0f;
	state->field18 = 0.0f;
	state->field1c = 0;
	state->field20 = NONE;
	state->field22 = NONE;
	state->field24 = NONE;
	state->field28 = NONE;
	state->field2a = NONE;
	state->field2c = NONE;
	state->field2e = 0;
	state->field30 = 0;
	state->field34 = 0;
	state->field38 = 0.0f;
	state->field3e = 0;
	state->field3c = NONE;
	state->field60 = NONE;
	state->field40 = 0;
}
