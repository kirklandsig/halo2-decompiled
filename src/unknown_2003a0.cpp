#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include "unknown_1a58b0.h"
#include <math.h>
#include <string.h>
#include "object_iterator.h"
#include "unit_requests.h"
#include "unknown_11cc90.h"

// @flags /O2 /Gr /arch:SSE

struct s_scenario_identifier_ab;
void *__stdcall function_b7a40(s_scenario_identifier_ab const *identifier, long *index_out);

// @retail 0x201330
long function_201330(short placement_index)
{
	long result = NONE;
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	state.iterator.signature = 0x86868686;
	state.iterator.type_mask = 2;
	state.iterator.flags = 0;
	state.iterator.index = 0;
	state.iterator.object_index = NONE;
	while ((state.object = function_baeb0(&state.iterator)) != NULL)
	{
		long index;
		if (function_b7a40((s_scenario_identifier_ab const *)((byte *)state.object + 0xa4), &index) && index == placement_index)
		{
			result = state.iterator.object_index;
			break;
		}
	}
	return result;
}

extern s_flag_bits g_557c74;

long function_1e4990(long index);
PRIVATE short const g_44aee8[20] =
{
	3, 7, 3, 3, 3, 3, 1, 2, 2, 4, 4, 4, 5, 5, -1, 3, 7, 7, 7, 4
};

// @retail 0x200c20
void function_200c20(byte const *definition)
{
	long index = record_pool_allocate(g_51e9d8);
	s_squad_datum *squad = squad_get(index);
	*(word *)((byte *)squad + 2) = 0;
	*(word *)((byte *)squad + 2) |= (word)((*(dword const *)(definition + 0x20) >> 8) & 1);
	if (*(dword const *)(definition + 0x20) & 0x200)
		*((byte *)squad + 2) |= 2;
	else
		*((byte *)squad + 2) &= ~2;
	squad->first_actor_index = NONE;
	squad->first_vehicle_index = NONE;
	*(long *)((byte *)squad + 0x78) = NONE;
	*(short *)((byte *)squad + 0x7c) = 0;
	*((byte *)squad + 0x26) = 0;
	*(short *)((byte *)squad + 0x2a) = NONE;
	*(short *)((byte *)squad + 0x62) = NONE;
	*(short *)((byte *)squad + 0x64) = NONE;
	*(long *)((byte *)squad + 0x80) = NONE;
	*(long *)((byte *)squad + 0x2c) = NONE;
	*(long *)((byte *)squad + 0x5c) = NONE;
	squad->value24 = 0;
	*((byte *)squad + 0x60) = 0;
	*(long *)((byte *)squad + 0x6c) = *(short const *)(definition + 0x26);
	squad->next_squad_index = NONE;
	*(short *)((byte *)squad + 0x7e) = NONE;
	for (long i = 0; i < 5; i++)
		((long *)((byte *)squad + 0x84))[i] = NONE;
	*((byte *)squad + 0x76) = definition[0x24];
	if (!*((byte *)squad + 0x76))
	{
		short palette_index = *(short const *)(definition + 0x36);
		byte *scenario = (byte *)g_4e0350;
		if (palette_index >= 0 && palette_index < *(long *)(scenario + 0x178))
		{
			long *palette = (long *)(*(byte **)(scenario + 0x17c) + palette_index * 8);
			if (palette[1] != NONE)
			{
				byte *tag = g_4e3b44[palette[1] & 0xffff].bytes;
				long unit_index = *(long *)(tag + 0x10);
				if (unit_index != NONE)
					*((byte *)squad + 0x76) = g_4e3b44[unit_index & 0xffff].bytes[0xc0];
				else
				{
					unit_index = *(long *)(tag + 0x18);
					if (unit_index != NONE)
						*((byte *)squad + 0x76) = g_4e3b44[unit_index & 0xffff].bytes[0xc0];
				}
				if (!*((byte *)squad + 0x76))
				{
					byte *properties = (byte *)function_1e4990(palette[1]);
					if (properties)
					{
						short type = *(short *)(properties + 4);
						if (type >= 0 && type < 20)
							*((byte *)squad + 0x76) = (byte)g_44aee8[type];
					}
				}
			}
		}
	}
}

void record_pool_release_all(s_record_pool *data);
void function_2009d0(void);

// @retail 0x200b30
void function_200b30(void)
{
	byte *scenario = (byte *)g_4e0350;
	g_51e9d8->valid = true;
	record_pool_release_all(g_51e9d8);
	g_51e9dc->valid = true;
	record_pool_release_all(g_51e9dc);
	for (short i = 0; i < *(long *)(scenario + 0x160); i++)
		function_200c20(*(byte **)(scenario + 0x164) + i * 0x74);
	for (short i = 0; i < *(long *)(scenario + 0x158); i++)
	{
		byte *definition = *(byte **)(scenario + 0x15c) + i * 0x24;
		long index = record_pool_allocate(g_51e9dc);
		byte *group = (byte *)squad_group_get(index);
		*(long *)(group + 0xc) = NONE;
		*(long *)(group + 4) = NONE;
		*(long *)(group + 8) = NONE;
		*(short *)(group + 0x14) = NONE;
		*(short *)(group + 0x24) = NONE;
		*(short *)(group + 0x26) = NONE;
		group[0x28] = 0;
		*(short *)(group + 0x2a) = NONE;
		*(short *)(group + 0x2c) = NONE;
		*(short *)(group + 0x34) = 0;
		group[0x23] = 0;
		*(long *)(group + 0x10) = *(short *)(definition + 0x20);
		*(long *)(group + 0xc) = NONE;
	}
	function_2009d0();
}

struct s_squad_activity_flags
{
	byte unknown00[2];
	word unused : 7;
	word active : 1;
	word remaining : 8;
};

// @retail 0x200930
void function_200930(void)
{
	for (long i = 0; i < 5; i++)
		g_557c74.d[i] = NONE;
	g_51e9d8 = data_new_inlined("squad", 335, sizeof(s_squad_datum), 0, g_510c2c);
	g_51e9dc = data_new_inlined("squad group", 100, sizeof(s_squad_group_datum), 0, g_510c2c);
}

// @retail 0x202420
void __stdcall function_202420(long group_index)
{
	(void)&group_index;
	long const *group_reference = &group_index;
	s_squad_group_datum *group = squad_group_get(*group_reference);
	bool active = false;
	for (long child = group->first_child_index; child != NONE; )
	{
		function_202420(child);
		s_squad_group_datum *child_group = squad_group_get(child);
		active |= *(bool *)((byte *)child_group + 0x22);
		child = child_group->next_sibling_index;
	}
	for (long index = group->first_squad_index; index != NONE; )
	{
		s_squad_datum *squad = squad_get(index);
		active |= (bool)((((dword)*(short *)((byte *)squad + 2)) >> 7) & 1);
		index = squad->next_squad_index;
	}
	if (active && !*(bool *)((byte *)group + 0x22))
		*(bool *)((byte *)squad_group_get(*group_reference) + 0x22) = true;
}

struct s_group_record_iterator
{
	s_squad_group_datum *group;
	s_record_pool_iterator records;
	long unknown10;
};

struct s_squad_group_totals
{
	byte unknown00[4];
	long first_child_index;
	long first_squad_index;
	long next_sibling_index;
	byte unknown10[0x24 - 0x10];
	short count;
	short actor_count;
	byte active;
	byte unknown29;
	short initial_count;
	short remaining_count;
	byte unknown2e[2];
	real average;
	short maximum;
	byte unknown36[2];
};

struct s_squad_count_flags
{
	byte unused : 4;
	byte active : 1;
	byte remaining : 3;
};

// @retail 0x202570
void __stdcall function_202570(long group_index)
{
	s_squad_group_totals *group = (s_squad_group_totals *)squad_group_get(group_index);
	byte active = 0;
	short count = 0;
	short remaining_count = 0;
	group->maximum = 0;
	group->actor_count = 0;
	real weighted_sum = 0.0f;
	for (long child_index = group->first_child_index; child_index != NONE; )
	{
		function_202570(child_index);
		s_squad_group_totals *child = (s_squad_group_totals *)squad_group_get(child_index);
		count += child->count;
		remaining_count += child->remaining_count;
		active |= child->active;
		if (child->maximum > group->maximum)
			group->maximum = child->maximum;
		group->actor_count += child->actor_count;
		if (child->actor_count > 0)
			weighted_sum += child->actor_count * child->average;
		child_index = child->next_sibling_index;
	}
	for (long squad_index = group->first_squad_index; squad_index != NONE; )
	{
		s_squad_datum *squad = squad_get(squad_index);
		count += squad->count_a;
		remaining_count += squad->value16;
		active |= ((s_squad_count_flags *)((byte *)squad + 2))->active;
		group->actor_count += squad->actor_count;
		if (squad->actor_count > 0)
			weighted_sum += squad->actor_count * squad->value10;
		short maximum = *(signed char *)((byte *)squad + 0x26);
		if (maximum > group->maximum)
			group->maximum = maximum;
		squad_index = squad->next_squad_index;
	}
	group->average = group->actor_count > 0 ? weighted_sum / group->actor_count : 0.0f;
	if ((group->remaining_count == NONE || group->remaining_count == 0) && remaining_count > 0)
	{
		if (group->count == NONE)
			group->initial_count = count;
		else
			group->initial_count = group->count;
	}
	group->count = count;
	group->remaining_count = remaining_count;
	group->active = active;
}

PRIVATE __forceinline s_squad_group_datum *next_group_record(s_group_record_iterator *iterator)
{
	s_squad_group_datum *result = NULL;
	if (g_4f55d0->active)
	{
		result = (s_squad_group_datum *)data_iterator_next_inlined(&iterator->records);
		iterator->group = result;
	}
	return result;
}

// @retail 0x202370
void function_202370(void)
{
	s_group_record_iterator iterator;
	if (g_4f55d0->active)
	{
		iterator.records.data = g_51e9dc;
		iterator.records.index = NONE;
	}
	s_squad_group_datum *group;
	while ((group = next_group_record(&iterator)) != NULL)
	{
		if (group->parent_index == NONE)
			function_202420(iterator.records.datum_index);
	}
}

// @retail 0x2024c0
void function_2024c0(void)
{
	s_group_record_iterator iterator;
	if (g_4f55d0->active)
	{
		iterator.records.data = g_51e9dc;
		iterator.records.index = NONE;
	}
	s_squad_group_datum *group;
	while ((group = next_group_record(&iterator)) != NULL)
	{
		if (group->parent_index == NONE)
			function_202570(iterator.records.datum_index);
	}
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);

struct s_actor_response_flags
{
	dword unused : 1;
	dword disabled : 1;
	dword remaining : 30;
};

// @retail 0x200240
void function_200240(long object_index, long target_index)
{
	(void)&target_index;
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	if (!TEST_FIELD_BIT(((s_actor_response_flags *)(definition + 0xbc))->disabled))
	{
		byte *owner = (byte *)function_badc0(*(long *)(object + 0x140), NONE);
		if (owner && !*(byte *)(owner + 0xaa))
		{
			long actor_index = *(long *)(owner + 0x12c);
			if (actor_index != NONE)
			{
				byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
				if (!*(bool *)(actor + 0x714) && *(long *)(actor + 0x718) > 0 && *(short *)(actor + 0x722) == 1)
				{
					long reference = *(long *)(actor + 0x724);
					if (reference != NONE && g_510c54->game_time - *(long *)(actor + 0x718) >= *(short *)(object + 0x1aa))
					{
						long root;
						if (!(*(real *)(object + 0x168) + 0.5f > *(real *)(actor + 0x71c)) &&
							(!function_badc0(target_index, NONE) ||
							(root = function_baf80(*(long *)(g_502418->data + (reference & 0xffff) * 0x3c + 0x20)), function_baf80(target_index) != root)))
						{
							++*(short *)(actor + 0x720);
						}
						else
						{
							*(short *)(actor + 0x720) -= 2;
							if (*(short *)(actor + 0x720) < 0)
								*(short *)(actor + 0x720) = 0;
						}
					}
				}
			}
		}
	}
}

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

struct s_squad_iterator
{
	short squad_index;
	short current;
	short next;
	short palette_index;
	word flags;
	bool flag_a;
	bool flag_b;
	bool flag_c;
	byte unknown0d[3];
	s_squad_definition_entry *definition;
};

// @retail 0x204ec0
void function_204ec0(s_squad_iterator *iterator, short squad_index, short flags, short mode)
{
	(void)&flags;
	(void)&mode;
	if (squad_index == NONE)
	{
		iterator->next = NONE;
		return;
	}
	s_squad_datum *squad;
	s_squad_definition_entry *definition;
	for (;;)
	{
		squad = squad_get((word)squad_index);
		short index = ((s_squad_definition_view *)squad)->definition_index;
		definition = index == NONE ? NULL : &((s_squad_definition_table_view *)g_4e0350)->entries[index];
		if (!definition || !(definition->data[0x24] & 0x20) || *(short *)(definition->data + 0x4e) == NONE)
			break;
		squad_index = *(short *)(definition->data + 0x4e);
	}
	iterator->definition = definition;
	iterator->current = NONE;
	iterator->next = 0;
	iterator->squad_index = squad_index;
	if (definition)
	{
		iterator->palette_index = NONE;
		if ((flags & 4) && !(flags & 1))
		{
			bool alternate = *(bool *)((byte *)squad + 0x60);
			long count = *(long *)(definition->data + (alternate ? 0x5c : 0x54));
			short index = 0;
			for (; index < count; index++)
			{
				byte *entries = *(byte **)(definition->data + (alternate ? 0x60 : 0x58));
				if (*(short *)(entries + index * 8) == 2)
					break;
			}
			if (index >= count)
				flags |= 1;
		}
	}
	else
	{
		byte *scenario = (byte *)g_4e0350;
		byte *entry = *(byte **)(scenario + 0x164) + (word)squad_index * 0x74;
		iterator->palette_index = *(short *)(entry + 0x38);
		if (*(short *)(entry + 0x38) == NONE)
			iterator->next = NONE;
		else
		{
			byte *palette = *(byte **)(scenario + 0x16c) + (word)iterator->palette_index * 0x38;
			iterator->next = (*(long *)(palette + 0x30) > 0) - 1;
		}
	}
	iterator->flags = (word)flags;
	iterator->flag_b = false;
	iterator->flag_a = false;
	iterator->flag_c = false;
	if (mode == 2)
		iterator->flag_c = true;
	else if (mode == 1)
		iterator->flag_a = true;
}

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

bool function_1e13f0(long actor_index);

PRIVATE inline s_actor_datum *next_actor_204390(s_squad_actor_iterator *iterator)
{
	s_actor_datum *actor = NULL;
	if (*(volatile bool *)&g_4f55d0->active && iterator->next_actor_index != NONE)
	{
		actor = actor_datum_get(iterator->next_actor_index);
		iterator->actor_index = iterator->next_actor_index;
		iterator->next_actor_index = actor->next_actor_index;
	}
	return actor;
}

// @retail 0x204390
bool function_204390(long squad_index)
{
	bool result = false;
	s_squad_actor_iterator iterator;
	squad_actor_begin_inline(&iterator, squad_index);
	while (next_actor_204390(&iterator))
	{
		if (function_1e13f0(iterator.actor_index))
		{
			result = true;
			goto done;
		}
	}
	for (long index = g_4f55d0->unknown14; index != NONE; )
	{
		s_record_pool *local_0 = g_4f55f0;
		s_actor_datum *actor = &((s_actor_datum *)local_0->data)[index & 0xffff];
		long source_squad = *(volatile long *)((byte *)actor + 0x34);
		if (source_squad == squad_index && function_1e13f0(index))
		{
			result = true;
			goto done;
		}
		index = actor->next_actor_index;
	}
done:
	return result;
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

struct s_squad_placement_entry
{
	byte unknown00[0x3c];
	short placement_index;
	byte unknown3e[2];
	char command_script_name[32];
	short command_script_index;
	byte unknown62[2];
};

struct s_squad_placement_definition
{
	byte unknown00[0x48];
	long count;
	s_squad_placement_entry *entries;
	char command_script_name[32];
	short command_script_index;
	byte unknown72[2];
};

struct s_squad_placement_table
{
	byte unknown000[0x164];
	s_squad_placement_definition *squads;
	byte unknown168[0x1b8 - 0x168];
	long script_count;
	byte *scripts;
};

PRIVATE inline bool placement_bit(dword const *bits, short index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

PRIVATE inline short placement_priority(s_squad_placement_entry *entry)
{
	return (*((byte *)entry + 0x1c) & 8) ? 100 : *(short *)((byte *)entry + 0x2a);
}

// @retail 0x2039d0
short function_2039d0(short squad_index, dword *available, dword const *blocked)
{
	(void)&available;
	(void)&blocked;
	s_squad_placement_definition *squad;
	short maximum;
	short maximum_count;
	for (;;)
	{
		squad = &((s_squad_placement_table *)g_4e0350)->squads[(word)squad_index];
		maximum = NONE;
		maximum_count = 0;
		short available_count = 0;
		short blocked_count = 0;
		for (short i = 0; i < squad->count; i++)
		{
			if (placement_bit(blocked, i))
				blocked_count++;
			else if (placement_bit(available, i))
			{
				short priority = placement_priority(&squad->entries[i]);
				available_count++;
				if (priority > maximum)
				{
					maximum = priority;
					maximum_count = 1;
				}
				else if (priority == maximum)
					maximum_count++;
			}
		}
		if (blocked_count == squad->count)
			return NONE;
		if (!available_count && squad->count > 0)
			memset(available, 0xff, ((squad->count + 31) >> 5) * sizeof(dword));
		else
			break;
	}
	if (!maximum_count)
		return NONE;
	short choice = 0;
	if (maximum_count > 1)
	{
		g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
		choice = (short)(((g_4e7408->unknown0 >> 16) * maximum_count) >> 16);
	}
	short found = 0;
	for (short i = 0; i < squad->count; i++)
	{
		if (placement_bit(available, i) && !placement_bit(blocked, i) && placement_priority(&squad->entries[i]) == maximum)
		{
			if (found == choice)
			{
				available[i >> 5] &= ~(1 << (i & 31));
				return i;
			}
			found++;
		}
	}
	return NONE;
}

// @retail 0x205010
short function_205010(s_squad_iterator *iterator)
{
	short result = NONE;
	if (iterator->next != NONE)
	{
		byte *squad = (byte *)squad_get((word)iterator->squad_index);
		if (*(long *)(squad + 0x80) != NONE)
		{
			if (*(signed char *)(squad + 0x30) >= 0)
			{
				do
				{
					short palette_index = *(short *)(squad + iterator->next * 4 + 0x32);
					short entry_index = *(short *)(squad + iterator->next * 4 + 0x34);
					byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)palette_index * 0x38;
					byte *entry = *(byte **)(palette + 0x34) + entry_index * 0x88;
					iterator->next++;
					if (iterator->next >= *(signed char *)(squad + 0x30))
						iterator->next = NONE;
					if (iterator->flag_c || (*(dword *)(entry + 0x20) & 1) == iterator->flag_a)
					{
						iterator->palette_index = palette_index;
						result = entry_index;
						break;
					}
				} while (iterator->next != NONE);
			}
		}
		else if (iterator->definition)
		{
			byte *definition = iterator->definition->data;
			long *block = (long *)(definition + (squad[0x60] ? 0x5c : 0x54));
			if (*block > 0)
			{
				do
				{
					byte *entries = squad[0x60] ? *(byte **)(definition + 0x60) : *(byte **)(definition + 0x58);
					short *selection = (short *)(entries + iterator->next * 8);
					iterator->next++;
					if (iterator->next >= *block)
						iterator->next = NONE;
					if (iterator->flags & (1 << selection[0]))
					{
						short palette_index = selection[2];
						byte *scenario = (byte *)g_4e0350;
						if (palette_index >= 0 && palette_index < *(long *)(scenario + 0x168))
						{
							byte *palette = *(byte **)(scenario + 0x16c) + (word)palette_index * 0x38;
							short entry_index = selection[3];
							if (entry_index >= 0 && entry_index < *(long *)(palette + 0x30))
							{
								byte *entry = *(byte **)(palette + 0x34) + entry_index * 0x88;
								if (iterator->flag_c || (*(dword *)(entry + 0x20) & 1) == iterator->flag_a)
								{
									iterator->palette_index = palette_index;
									result = entry_index;
									break;
								}
							}
						}
					}
				} while (iterator->next != NONE);
			}
		}
		else
		{
			do
			{
				byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)iterator->palette_index * 0x38;
				short entry_index = iterator->next;
				if (entry_index < 0 || entry_index >= *(long *)(palette + 0x30))
				{
					iterator->next = NONE;
					result = NONE;
					break;
				}
				byte *entry = *(byte **)(palette + 0x34) + entry_index * 0x88;
				iterator->next++;
				palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)iterator->palette_index * 0x38;
				if (iterator->next >= *(long *)(palette + 0x30))
					iterator->next = NONE;
				if (iterator->flag_c || (*(dword *)(entry + 0x20) & 1) == iterator->flag_a)
				{
					result = entry_index;
					break;
				}
			} while (iterator->next != NONE);
		}
	}
	iterator->current = result;
	return result;
}

long function_257ed0(long thread_index, long actor_index, short script_index);

PRIVATE __forceinline short squad_valid_command_script(s_squad_placement_table *scenario, short index, char const *name)
{
	if (index >= 0 && index < scenario->script_count)
	{
		unsigned long length = 0;
		while (length < 32 && *name++)
			length++;
		if (length > 0 && *(short *)(scenario->scripts + index * 0x28 + 0x20) == 5)
			return index;
	}
	return NONE;
}

// @retail 0x201b70
void function_201b70(long squad_index, short entry_index, long actor_index)
{
	(void)&actor_index;
	s_squad_placement_table *scenario = (s_squad_placement_table *)g_4e0350;
	s_squad_placement_definition *squad = &scenario->squads[squad_index & 0xffff];
	s_squad_placement_entry *entry = &squad->entries[entry_index];
	short script_index = squad_valid_command_script(scenario, entry->command_script_index, entry->command_script_name);
	if (script_index == NONE)
		script_index = squad_valid_command_script(scenario, squad->command_script_index, squad->command_script_name);
	if (script_index != NONE)
		function_257ed0(NONE, actor_index, script_index);
}

PRIVATE __forceinline s_squad_placement_entry *squad_placement_entry(s_squad_placement_definition *squad, long index)
{
	return &squad->entries[index];
}

// @retail 0x201400
bool function_201400(long squad_index, short entry_index)
{
	s_squad_placement_definition *squad = &((s_squad_placement_table *)g_4e0350)->squads[squad_index & 0xffff];
	bool result = true;
	if (entry_index >= 0 && entry_index < squad->count)
	{
		dword placement_index = squad_placement_entry(squad, entry_index)->placement_index;
		if ((word)placement_index != (word)NONE)
		{
			long object_index = function_201330((short)placement_index);
			if (object_index != NONE)
				result = function_2013c0(object_index);
			else
				result = false;
		}
	}
	return result;
}

// @retail 0x201460
void function_201460(long squad_index, dword *unavailable)
{
	(void)&unavailable;
	s_squad_placement_definition *squad = &((s_squad_placement_table *)g_4e0350)->squads[squad_index & 0xffff];
	for (short i = 0; i < squad->count; i++)
	{
		long placement_index = squad_placement_entry(squad, i)->placement_index;
		if ((short)placement_index != NONE)
		{
			long object_index = function_201330((short)placement_index);
			if (object_index == NONE || !function_2013c0(object_index))
				unavailable[i >> 5] |= 1 << (i & 0x1f);
		}
	}
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

void function_1e22d0(long actor_index, bool conditional);

// @retail 0x203f60
void function_203f60(long actor_index)
{
	if (g_4f55d0->active)
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		actor->next_actor_index = g_4f55d0->unknown14;
		g_4f55d0->unknown14 = actor_index;
		actor->flag00a = true;
		*(long *)((byte *)actor + 0x14) = g_510c54->game_time;
		function_1e22d0(actor_index, false);
	}
}

bool function_1e12b0(long actor_index, dword const *clusters);

// @retail 0x202310
bool __stdcall function_202310(long squad_index, dword const *clusters, long cluster_count)
{
    long const *local_0 = &cluster_count;
    (void)local_0;
	bool result = false;
	long actor_index = squad_get(squad_index)->first_actor_index;
	while (actor_index != NONE)
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		result = function_1e12b0(actor_index, clusters);
		if (result)
			break;
		actor_index = actor->next_actor_index;
	}
	return result;
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

void function_205280(long group_index, long squad_index);

struct s_squad_tree_group_definition
{
	byte unknown00[0x20];
	short parent;
	byte unknown22[2];
};

struct s_squad_tree_squad_definition
{
	byte unknown00[0x26];
	short parent;
	byte unknown28[0x74 - 0x28];
};

struct s_squad_tree_definition
{
	byte unknown000[0x158];
	long group_count;
	s_squad_tree_group_definition *groups;
	long squad_count;
	s_squad_tree_squad_definition *squads;
};

// @retail 0x2009d0
void function_2009d0(void)
{
	s_squad_tree_definition *definition = (s_squad_tree_definition *)g_4e0350;
	s_record_pool *groups = g_51e9dc;
	s_record_pool *squads = g_51e9d8;
	short i;
	for (i = 0; i < definition->group_count; i++)
	{
		s_squad_group_datum *group = &((s_squad_group_datum *)groups->data)[(word)i];
		group->first_child_index = NONE;
		group->first_squad_index = NONE;
		group->next_sibling_index = NONE;
		group->parent_index = definition->groups[(word)i].parent;
	}
	for (i = 0; i < definition->squad_count; i++)
	{
		s_squad_datum *squad = (&((s_squad_datum *)squads->data)[(word)i]);
		squad->next_squad_index = NONE;
		*(long *)squad->unknown6c = definition->squads[(word)i].parent;
	}
	for (i = 0; i < definition->group_count; i++)
	{
		s_squad_group_datum *group = &((s_squad_group_datum *)groups->data)[(word)i];
		if (group->parent_index != NONE)
		{
			s_squad_group_datum *parent = &((s_squad_group_datum *)groups->data)[group->parent_index & 0xffff];
			long child = parent->first_child_index;
			if (child == NONE)
				parent->first_child_index = i;
			else
			{
				s_squad_group_datum *last;
				do
				{
					last = &((s_squad_group_datum *)groups->data)[child & 0xffff];
					child = last->next_sibling_index;
				} while (child != NONE);
				last->next_sibling_index = i;
			}
		}
	}
	for (i = 0; i < definition->squad_count; i++)
	{
		long parent = *(long *)(&((s_squad_datum *)squads->data)[(word)i])->unknown6c;
		if (parent != NONE)
			function_205280(parent, i);
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

// @retail 0x203900
bool function_203900(short squad_index, real probability)
{
	(void)&probability;
	s_squad_datum *squad = squad_get((word)squad_index);
	real *global_balance = (real *)((byte *)g_4f55d0 + 0x1c);
	real delta = *global_balance * (-1.0f / 3.0f);
	real correction = -*(real *)squad->unknown04;
	real adjustment = fabs(delta) > fabs(correction) ? delta : correction;
	bool result = function_x82e52f(&g_4e7408->unknown0, NULL, 0) < adjustment + probability;
	real change = (real)result - probability;
	*(real *)squad->unknown04 += change;
	*global_balance += change;
	return result;
}

PRIVATE __forceinline s_actor_datum *next_active_squad_actor(s_squad_actor_iterator *iterator)
{
	s_actor_datum *result = NULL;
	while (g_4f55d0->active && iterator->next_actor_index != NONE)
	{
		long current = iterator->next_actor_index;
		s_actor_datum *actor = actor_datum_get(current);
		iterator->actor_index = current;
		iterator->next_actor_index = actor->next_actor_index;
		if (*(bool *)((byte *)actor + 9))
		{
			result = actor_datum_get(current);
			break;
		}
	}
	return result;
}

// @retail 0x203cc0
void function_203cc0(long squad_index)
{
	s_squad_activity_flags *squad = (s_squad_activity_flags *)squad_get(squad_index);
	squad->active = false;
	s_squad_actor_iterator iterator;
	squad_actor_begin_inline(&iterator, squad_index);
	s_actor_datum *actor;
	while ((actor = next_active_squad_actor(&iterator)) != NULL)
	{
		if (*(bool *)((byte *)actor + 9))
		{
			*(bool *)((byte *)actor + 9) = false;
			*(long *)((byte *)actor + 0x10) = g_510c54->game_time;
			(*(short *)((byte *)g_4f55d0 + 0x36a))--;
		}
	}
}

struct s_squad_difficulty_values
{
	long unknown00;
	real values_a[4];
	real values_b[4];
	real values_c[4];
};

long function_1e49d0(long index);
bool g_4f55df;

// @retail 0x2036c0
void function_2036c0(long definition_index, short mode, bool *enabled, bool *forced, real *value)
{
	(void)&mode;
	(void)&forced;
	s_squad_difficulty_values *data = (s_squad_difficulty_values *)function_1e49d0(definition_index);
	if (data)
	{
		short difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;
		switch (mode)
		{
		case 4:
			if (data->values_c[difficulty] > 0.0f)
			{
				*enabled = false;
				*forced = true;
			}
			else
			{
				*enabled = true;
				*value = 0.0f;
			}
			break;
		case 1:
			*enabled = true;
			*value = data->values_a[difficulty];
			break;
		case 2:
			*enabled = true;
			*value = data->values_c[difficulty];
			break;
		case 3:
			*enabled = false;
			*forced = false;
			break;
		default:
			*enabled = true;
			*value = data->values_b[difficulty];
			break;
		}
	}
	else
	{
		*enabled = false;
		*forced = false;
	}
	if (g_4e6948->state == 1 && g_4f55df)
	{
		*enabled = false;
		*forced = true;
	}
}


bool function_1df560(short team_a, short team_b);
bool function_2104b0(short output_index, point3f const *point, point3f *out);

// @retail 0x204950
real function_204950(long actor_index, long squad_index, short mode)
{
    (void)&mode;
    real result = 3.402823466e+38F;
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    byte *squad = (byte *)squad_get(squad_index);
    if (*(short *)(squad + 0x7e) == g_4686c4 &&
        !function_1df560(*(short *)(actor + 0x24), (signed char)squad[0x76]))
    {
        s_squad_iterator iterator;
        function_204ec0(&iterator, (short)squad_index, 15, mode);
        while (function_205010(&iterator) != NONE)
        {
            byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)iterator.palette_index * 0x38;
            byte *entry = *(byte **)(palette + 0x34) + iterator.current * 0x88;
            point3f const *point = (point3f const *)(entry + 0x24);
            point3f transformed;
            real squared;
            if (*(short *)(entry + 0x30) == NONE)
            {
                real x = *(real *)(actor + 0x238) - point->x;
                real z = *(real *)(actor + 0x240) - point->z;
                real y = *(real *)(actor + 0x23c) - point->y;
                squared = x * x + z * z + y * y;
            }
            else
            {
                if (!function_2104b0(*(short *)(entry + 0x30), point, &transformed))
                    transformed = *point;
                real z = *(real *)(actor + 0x240) - transformed.z;
                real x = *(real *)(actor + 0x238) - transformed.x;
                real y = *(real *)(actor + 0x23c) - transformed.y;
                squared = z * z + x * x + y * y;
            }
            real distance = (real)sqrt(squared) - *(real *)(entry + 0x34);
            real clamped = distance > 0.0f ? distance : 0.0f;
            if (clamped < result)
                result = clamped;
        }
    }
    return result;
}


// @retail 0x204b20
long function_204b20(long actor_index, short mode)
{
    (void)&actor_index;
    (void)&mode;
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    real best = 15.0f;
    long result = NONE;
    s_record_pool_iterator iterator;
    if (g_4f55d0->active)
    {
        iterator.data = g_51e9d8;
        iterator.index = NONE;
    }
    for (;;)
    {
        byte *squad = NULL;
        long squad_index = NONE;
        if (g_4f55d0->active)
        {
            squad = data_iterator_next_inlined(&iterator);
            squad_index = iterator.datum_index;
        }
        squad_index &= 0xffff;
        if (!squad)
            break;
        if ((signed char)squad[2] >= 0)
            continue;
        short team = (signed char)squad[0x76];
        short actor_team = *(short *)(actor + 0x24);
        real weight;
        if (team == actor_team)
            weight = 1.0f;
        else if (function_1df560(team, actor_team))
            continue;
        else
            weight = 1.333f;
        real distance = function_204950(actor_index, squad_index, mode);
        if (distance < 3.402823466e+38F)
        {
            if (squad_index == *(long *)(actor + 0x28))
                weight *= 0.666f;
            distance *= weight;
            if (distance < best)
            {
                result = squad_index;
                best = distance;
            }
        }
    }
    return result;
}


#include "unit_requests.h"

struct s_time_entry;
struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;
void function_1e6980(s_time_entry *entries, short type, byte result);
void function_b7360(long object_index);
long function_baf40(long object_index);
short function_200ee0(long object_index, short mode, long *selected_object, short *occupied_count);

typedef bool (__stdcall *t_unit_request_proc)(long, s_unit_request *);
typedef bool (__stdcall *t_unit_request_update_proc)(long, long);
typedef void (__stdcall *t_unit_request_end_proc)(long, long);
struct s_unit_request_definition
{
    t_unit_request_proc perform;
    t_unit_request_update_proc update;
    t_unit_request_end_proc finished;
    t_unit_request_end_proc interrupted;
};
extern s_unit_request_definition *g_4677c8[60];

PRIVATE inline byte *squad_transfer_object(long index)
{
    return *(byte **)(g_4e0300->data + (index & 0xffff) * 12 + 8);
}

PRIVATE __forceinline bool squad_transfer_request(long unit_index, long object_index, short seat)
{
    s_unit_request request;
    request.type = 0x1c;
    request.type1c.object_index = object_index;
    request.type1c.seat_index = seat;
    request.type1c.unknowna = true;
    request.type1c.unknownb = false;
    s_unit_request_definition *definition = g_4677c8[0x1c];
    function_b7360(unit_index);
    bool result = definition->perform(unit_index, &request);
    if (unit_index != NONE)
    {
        long player_index = *(long *)(squad_transfer_object(unit_index) + 0x13c);
        if (player_index != NONE)
        {
            short controller = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);
            if (controller != NONE)
                function_1e6980((s_time_entry *)((byte *)g_51e9c0 + controller * 0x1b0 + 0x150),
                    (short)request.type, result);
        }
    }
    return result;
}

// @retail 0x204010
void function_204010(long squad_index, long other_squad_index)
{
    (void)&other_squad_index;
    if (other_squad_index != NONE && squad_index != NONE)
    {
        s_squad_datum *squad = squad_get(squad_index);
        for (long vehicle_index = squad_get(other_squad_index)->first_vehicle_index; vehicle_index != NONE; )
        {
            byte *vehicle = squad_transfer_object(vehicle_index);
            byte *definition = g_4e3b44[*(long *)vehicle & 0xffff].bytes;
            for (long target = squad->first_vehicle_index; target != NONE; )
            {
                byte *target_vehicle = squad_transfer_object(target);
                long selected = NONE;
                short seat = function_200ee0(target, 4 + (*(short *)(definition + 0x244) != 0), &selected, NULL);
                if (seat != NONE && squad_transfer_request(vehicle_index, selected, seat))
                    break;
                target = *(long *)(target_vehicle + 0x3a4);
            }
            vehicle_index = *(long *)(vehicle + 0x3a4);
        }
        s_squad_actor_iterator iterator;
        function_204d30(&iterator, other_squad_index);
        while (g_4f55d0->active && iterator.next_actor_index != NONE)
        {
            s_actor_datum *actor = actor_datum_get(iterator.next_actor_index);
            iterator.next_actor_index = actor->next_actor_index;
            long unit_index = actor->unit_index;
            byte *unit = squad_transfer_object(unit_index);
            long parent_index = *(long *)(unit + 0x14);
            if (parent_index != NONE)
            {
                byte *parent = squad_transfer_object(function_baf40(parent_index));
                if (parent[0xaa] == 1 && (*(long *)(parent + 0x3a0) & 0xffff) != (other_squad_index & 0xffff))
                    function_e68c0(0x1e, unit_index);
            }
            if (*(long *)(unit + 0x14) == NONE)
            {
                for (long target = squad->first_vehicle_index; target != NONE; )
                {
                    byte *target_vehicle = squad_transfer_object(target);
                    long selected = NONE;
                    short seat = function_200ee0(target, 0, &selected, NULL);
                    if (seat != NONE && squad_transfer_request(actor->unit_index, selected, seat))
                        break;
                    target = *(long *)(target_vehicle + 0x3a4);
                }
            }
        }
    }
}


#include "unknown_20fe20.h"
extern s_reference g_471004;
point3f *function_b9dd0(long object_index, point3f *point);
void __stdcall function_2628f0(long actor_index, s_reference reference);

PRIVATE inline byte *squad_area_get(short palette_index, short area_index)
{
    byte *scenario = (byte *)g_4e0350;
    if (palette_index >= 0 && palette_index < *(long *)(scenario + 0x168))
    {
        byte *palette = *(byte **)(scenario + 0x16c) + (word)palette_index * 0x38;
        if (area_index >= 0 && area_index < *(long *)(palette + 0x30))
            return *(byte **)(palette + 0x34) + area_index * 0x88;
    }
    return NULL;
}

PRIVATE inline void squad_area_consider(byte *squad, byte const *definition, byte *area,
    short palette_index, short area_index, point3f const *point, real *nearest, s_reference *best)
{
    real distance = function_210ac0((s_type_c3b527 const *)(area + 0x24), point);
    if (*(real *)(area + 0x34) + *(real const *)(definition + 0x50) > distance)
    {
        *(short *)(squad + 0x32 + (signed char)squad[0x30] * 4) = palette_index;
        *(short *)(squad + 0x34 + (signed char)squad[0x30] * 4) = area_index;
        ++squad[0x30];
    }
    else if (distance < *nearest)
    {
        *nearest = distance;
        best->unknown0 = palette_index;
        best->unknown2 = area_index;
    }
}

// @retail 0x204450
void function_204450(long definition_index, long squad_index)
{
    (void)&squad_index;
    byte *squad = (byte *)squad_get(squad_index);
    byte *definition = *(byte **)((byte *)g_4e0350 + 0x244) + definition_index * 0x7c;
    point3f point;
    if (definition[0x24] & 0x10)
    {
        long target = NONE;
        long index = NONE;
        while ((index = data_next_absolute_index_inlined(g_4e8c24, index + 1)) != NONE)
        {
            byte *player = g_4e8c24->data + index * g_4e8c24->size;
            if (!player)
                break;
            long unit_index = *(long *)(player + 0x2c);
            if (unit_index != NONE)
            {
                function_b9dd0(unit_index, &point);
                vector3f difference;
                vector3d_from_points3d(&point, (point3f *)(squad + 0x18), &difference);
                if (sqrt(length_sq3f(&difference)) < 3.402823466e+38F)
                    target = unit_index;
            }
        }
        *(long *)(squad + 0x80) = target;
    }
    if (*(long *)(squad + 0x80) != NONE)
    {
        s_reference best = g_471004;
        real nearest = 3.402823466e+38F;
        squad[0x30] = 0;
        function_b9dd0(*(long *)(squad + 0x80), &point);
        byte *entries = definition + (squad[0x60] ? 0x5c : 0x54);
        if (*(long *)entries > 0)
        {
            for (short i = 0; i < *(long *)entries && (signed char)squad[0x30] < 10; i++)
            {
                byte *entry = *(byte **)(definition + (squad[0x60] ? 0x60 : 0x58)) + i * 8;
                short palette_index = *(short *)(entry + 4);
                short area_index = *(short *)(entry + 6);
                byte *area = squad_area_get(palette_index, area_index);
                if (area)
                    squad_area_consider(squad, definition, area, palette_index, area_index, &point, &nearest, &best);
            }
        }
        else
        {
            byte *scenario = (byte *)g_4e0350;
            byte *local_b3aa32 = *(byte **)(scenario + 0x164) + (squad_index & 0xffff) * 0x74;
            short palette_index = *(short *)(local_b3aa32 + 0x38);
            if (palette_index >= 0 && palette_index < *(long *)(scenario + 0x168))
            {
                byte *palette = *(byte **)(scenario + 0x16c) + (word)palette_index * 0x38;
                for (short i = 0; i < *(long *)(palette + 0x30) && (signed char)squad[0x30] < 10; i++)
                {
                    byte *area = *(byte **)(palette + 0x34) + i * 0x88;
                    squad_area_consider(squad, definition, area, *(short *)(local_b3aa32 + 0x38), i, &point, &nearest, &best);
                }
            }
        }
        if (!squad[0x30] && (best.unknown0 != g_471004.unknown0 || best.unknown2 != g_471004.unknown2))
        {
            *(s_reference *)(squad + 0x32) = best;
            squad[0x30] = 1;
        }
        s_squad_actor_iterator iterator;
        squad_actor_begin_inline(&iterator, squad_index);
        s_actor_datum *actor;
        while ((actor = squad_actor_next_inline(&iterator)) != NULL)
        {
            s_reference reference = *(s_reference *)((byte *)actor + 0x418);
            bool valid = false;
            if ((reference.unknown0 != g_470fa0.unknown0 || reference.unknown2 != g_470fa0.unknown2) &&
                !(reference.unknown2 & 0x8000))
            {
                byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)reference.unknown2 * 0x38;
                if (reference.unknown0 >= 0 && reference.unknown0 < *(long *)(palette + 0x28))
                {
                    byte *entry = *(byte **)(palette + 0x2c) + reference.unknown0 * 0x20;
                    if (entry)
                    {
                        for (short i = 0; i < (signed char)squad[0x30]; i++)
                        {
                            if (*(short *)(squad + 0x32 + i * 4) == reference.unknown2 &&
                                *(short *)(squad + 0x34 + i * 4) == *(short *)(entry + 0x10))
                            {
                                valid = true;
                                break;
                            }
                        }
                    }
                }
            }
            if (!valid)
            {
                function_2628f0(iterator.actor_index, g_470fa0);
                *((byte *)actor + 0x227) = 0;
            }
        }
    }
    *(long *)(squad + 0x5c) = g_510c54->game_time;
}


bool function_1e11b0(long actor_index, bool active);

#pragma inline_depth(0)
// @retail 0x203bf0
bool function_203bf0(long squad_index)
{
    (void)&squad_index;
    s_squad_datum *squad = (s_squad_datum *)g_51e9d8->data + (squad_index & 0xffff);
    if (*(short *)((byte *)squad + 0x7e) == g_4686c4)
    {
        if (!(((byte *)squad)[2] & 0x80))
        {
            s_squad_actor_iterator iterator;
            function_204d30(&iterator, squad_index);
            while (g_4f55d0->active && iterator.next_actor_index != NONE)
            {
                long actor_index = iterator.next_actor_index;
                s_actor_datum *actor = (s_actor_datum *)g_4f55f0->data + (actor_index & 0xffff);
                iterator.next_actor_index = actor->next_actor_index;
                if (!function_1e11b0(actor_index, true))
                {
                    *(long *)((byte *)actor + 0x34) = squad_index;
                    function_203ed0(actor_index, false);
                    function_203f60(actor_index);
                }
            }
        }
        ((byte *)squad)[2] |= 0x80;
        *(long *)((byte *)squad + 0x78) = g_510c54->game_time;
    }
    return (bool)(((dword)((byte *)squad)[2] & 0x80) >> 7);
}
#pragma inline_depth(255)


extern s_record_pool *g_5044c8;
bool function_1e1de0(long actor_index);
bool function_1df5d0(short team_a, short team_b);

// @retail 0x203360
void function_203360(long squad_index)
{
    (void)&squad_index;
    byte *squad = (byte *)squad_get(squad_index);
    squad[2] &= ~0x10;
    *(short *)(squad + 0x16) = 0;
    *(short *)(squad + 0xc) = 0;
    *(short *)(squad + 0xa) = 0;
    squad[0x26] = 0;
    *(real *)(squad + 0x10) = 0.0f;
    bool hostile = false;
    s_squad_actor_iterator iterator;
    squad_actor_begin_inline(&iterator, squad_index);
    s_actor_datum *actor;
    while ((actor = squad_actor_next_inline(&iterator)) != NULL)
    {
        real health = 0.0f;
        long count = 0;
        if (actor->unit_index != NONE)
        {
            health = *(real *)(squad_transfer_object(actor->unit_index) + 0xec);
            count = 1;
        }
        else if (actor->perception_index != NONE)
        {
            byte *perception = g_5044c8->data + (actor->perception_index & 0xffff) * 0x34;
            count = *(word *)(perception + 0x14);
            health = (real)(short)count / (real)*(short *)(perception + 0x16);
        }
        *(short *)(squad + 0xa) += count;
        *(real *)(squad + 0x10) += health;
        *(short *)(squad + 0xc) += *((byte *)actor + 7) * count;
        *(short *)(squad + 0x16) += function_1e1de0(iterator.actor_index) * count;
        long tracking_index = *(long *)((byte *)actor + 0x338);
        if (tracking_index != NONE)
        {
            byte *tracking = g_502418->data + (tracking_index & 0xffff) * 0x3c;
            long target_index = *(long *)(tracking + 8);
            byte *target = g_50241c->data + (target_index & 0xffff) * 0xc4;
            if (!function_1df5d0(*(short *)((byte *)actor + 0x24), *(short *)(target + 0x20)))
                hostile = true;
            function_1e1de0(iterator.actor_index);
            if (actor->value086 > (signed char)squad[0x26])
                squad[0x26] = (byte)actor->value086;
            if (actor->value086 >= 7)
                squad[2] |= 0x10;
        }
    }
    if (hostile)
        squad[2] &= ~0x20;
    if (*(short *)(squad + 8) > 0)
    {
        real health = *(real *)(squad + 0x10) / (real)*(short *)(squad + 8);
        *(real *)(squad + 0x10) = health < 0.0f ? 0.0f : health;
    }
    squad[3] &= ~1;
    squad_actor_begin_inline(&iterator, squad_index);
    point3f *position = (point3f *)(squad + 0x18);
    *position = *g_468788;
    while ((actor = squad_actor_next_inline(&iterator)) != NULL)
    {
        position->x += actor->position.x;
        position->y += actor->position.y;
        position->z += actor->position.z;
    }
    real scale = (real)*(short *)(squad + 0xa);
    position->x *= scale;
    position->y *= scale;
    position->z *= scale;
    short definition_index = *(short *)(squad + 0x2a);
    if (definition_index != NONE)
    {
        byte *definition = *(byte **)((byte *)g_4e0350 + 0x244) + definition_index * 0x7c;
        if ((definition[0x24] & 0x10) || ((definition[0x24] & 0x20) && *(short *)(definition + 0x4e) != NONE))
        {
            long last_update = *(long *)(squad + 0x5c);
            if (last_update == NONE || (g_510c54->game_time - last_update) * g_510c54->rate > 2.0f)
                function_204450(definition_index, squad_index);
        }
    }
}


void function_1e1150(long actor_index, short team);

// @retail 0x203d70
void function_203d70(long actor_index, short squad_index, bool keep_team)
{
    (void)&actor_index;
    (void)&squad_index;
    (void)&keep_team;
    if (g_4f55d0->active)
    {
        s_actor_datum *actor = actor_datum_get(actor_index);
        s_squad_datum *squad = squad_get((word)squad_index);
        if (function_1e13f0(actor_index))
        {
            if (*(short *)((byte *)squad_get((word)squad_index) + 0x7e) != g_4686c4)
            {
                s_squad_actor_iterator iterator;
                function_204d30(&iterator, squad_index);
                while (g_4f55d0->active && iterator.next_actor_index != NONE)
                {
                    long previous_index = iterator.next_actor_index;
                    s_actor_datum *previous = actor_datum_get(previous_index);
                    iterator.next_actor_index = previous->next_actor_index;
                    *(long *)((byte *)previous + 0x34) = squad_index;
                    function_203ed0(previous_index, false);
                    function_203f60(previous_index);
                }
                *(short *)((byte *)squad_get((word)squad_index) + 0x7e) = g_4686c4;
            }
            if (*((byte *)actor + 9) && !(((byte *)squad)[2] & 0x80))
                function_203bf0(squad_index);
        }
        *(long *)((byte *)actor + 0x34) = NONE;
        actor->next_actor_index = squad->first_actor_index;
        squad->first_actor_index = actor_index;
        actor->squad_index = squad_index;
        function_1e11b0(actor_index, (bool)((((dword)((byte *)squad)[2]) >> 7) & 1));
        short team = (signed char)((byte *)squad)[0x76];
        if (*(short *)((byte *)actor + 0x24) != team && !keep_team)
            function_1e1150(actor_index, team);
        squad->actor_count++;
        ((byte *)squad)[3] |= 1;
    }
}


#include "object_queries.h"
void function_11bed0(s_location *location, point3f const *point);

PRIVATE inline long squad_tick_round(real ticks)
{
    long result;
    __asm
    {
        fld ticks
        fistp result
    }
    return result;
}

// @retail 0x201ea0
void function_201ea0(void)
{
    long elapsed = squad_tick_round((real)g_510c54->field_2_3);
    dword const *clusters = (dword const *)((byte *)g_4e6948 + 0x1138);
    byte *structure = (byte *)g_4e0348;
    s_record_pool_iterator squads;
    if (g_4f55d0->active)
    {
        squads.data = g_51e9d8;
        squads.index = NONE;
    }
    while (g_4f55d0->active)
    {
        byte *squad = data_iterator_next_inlined(&squads);
        short squad_index = (word)squads.datum_index;
        if (!squad)
            break;
        bool wake = false;
        if (*(short *)((byte *)squad_get((word)squad_index) + 0x7e) == g_4686c4)
        {
            wake = (bool)((*(dword *)((byte *)squad + 2) >> 9) & 1);
            if (!wake || *(long *)(squad + 0x78) == NONE)
                wake |= function_202310((word)squad_index, clusters, 0x200);
            if (!wake)
            {
                short definition_index = *(short *)(squad + 0x2a);
                if (definition_index != NONE)
                {
                    byte *definition = *(byte **)((byte *)g_4e0350 + 0x244) + definition_index * 0x7c;
                    if (definition)
                        wake = (bool)((*(dword *)(definition + 0x24) >> 1) & 1);
                }
            }
            if (wake)
                *(short *)(squad + 0x7c) = 0;
            else
                *(short *)(squad + 0x7c) += elapsed;
            if (!wake && *(short *)(squad + 0x7c) < g_510c54->field_2_3 * 20 && *(short *)(squad + 0x2a) != NONE)
            {
                s_squad_iterator areas;
                function_204ec0(&areas, squad_index, 15, 2);
                while (function_205010(&areas) != NONE)
                {
                    if (wake)
                        break;
                    byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)areas.palette_index * 0x38;
                    byte *area = *(byte **)(palette + 0x34) + areas.current * 0x88;
                    long words = (*(long *)(structure + 0x9c) + 31) >> 5;
                    bool overlap = false;
                    while (--words >= 0)
                    {
                        dword intersection = clusters[words] & ((dword *)(area + 0x3c))[words];
                        if (intersection)
                            overlap = true;
                    }
                    wake = overlap;
                }
            }
        }
        if (wake)
        {
            *(short *)(squad + 0x28) = (short)squad_tick_round((real)g_510c54->field_2_3 * 5.0f);
            if (!(squad[2] & 0x80))
                function_203bf0((word)squad_index);
        }
        else if (*(word *)(squad + 2) & 0x80)
        {
            if (*(short *)(squad + 0x28) > elapsed)
                *(short *)(squad + 0x28) -= elapsed;
            else
                function_203cc0((word)squad_index);
        }
    }
    long actor_index = g_4f55d0->unknown14;
    long current_time = g_510c54->game_time;
    while (actor_index != NONE)
    {
        long current = actor_index;
        byte *actor = (byte *)actor_datum_get(current);
        actor_index = *(long *)(actor + 0x20);
        bool wake = false;
        if (function_1e13f0(current))
        {
            wake = function_1e12b0(current, clusters);
            if (wake)
                *(long *)(actor + 0x14) = current_time;
            else if (actor[9] && current_time - *(long *)(actor + 0x14) <
                squad_tick_round((real)g_510c54->field_2_3 * 15.0f))
            {
                byte *state = (byte *)actor_datum_get(current);
                if (state[0x50c] && *(short *)(state + 0x504) == 1)
                {
                    point3f point;
                    function_210850((s_type_c3b527 const *)(actor + 0x4ec), &point);
                    point3f raised;
                    raised.x = point.x + g_4687b0->i * 0.1f;
                    raised.y = point.y + g_4687b0->j * 0.1f;
                    raised.z = point.z + g_4687b0->k * 0.1f;
                    s_location location;
                    function_11bed0(&location, &raised);
                    if (location.cluster_index != NONE)
                        wake = (clusters[location.cluster_index >> 5] & (1 << (location.cluster_index & 31))) != 0;
                }
            }
        }
        function_1e11b0(current, wake);
    }
}


// @retail 0x201df0
void function_201df0(void)
{
    s_squad_record_iterator iterator;
    if (g_4f55d0->active)
    {
        iterator.records.data = g_51e9d8;
        iterator.records.index = NONE;
    }
    for (;;)
    {
        byte *squad = NULL;
        if (g_4f55d0->active)
        {
            squad = data_iterator_next_inlined(&iterator.records);
            iterator.squad_index = iterator.records.datum_index & 0xffff;
            iterator.squad = (s_squad_datum *)squad;
        }
        if (!squad)
            break;
        if (squad[3] & 1)
            function_203360(iterator.squad_index);
    }
}


void function_1e3400(long actor_index, long squad_index);
void function_26def0(long actor_index, long owner_index);

// @retail 0x204ca0
void function_204ca0(long actor_index)
{
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    if (actor[0x3c])
    {
        if (*(short *)(actor + 0x2c) > 0)
            --*(short *)(actor + 0x2c);
        if (!*(short *)(actor + 0x2c))
        {
            long squad_index = function_204b20(actor_index, *(long *)(actor + 0x26c) != NONE);
            if (squad_index != NONE)
            {
                if (squad_index != *(long *)(actor + 0x30))
                    function_1e3400(actor_index, squad_index);
                actor[0x3c] = false;
                if (*(long *)(actor + 0x3f8) != NONE)
                    function_26def0(actor_index, NONE);
            }
            else
                *(short *)(actor + 0x2c) = g_510c54->field_2_3 * 3;
        }
    }
}

void function_291740(short arg_0, long arg_1);

void function_202e30(short arg_0, long arg_1, byte arg_2);

struct s_actor_options;
void function_1e4570(long arg_0, s_actor_options const *arg_1);
void function_26b290(long arg_0);

// @retail 0x202e90
void __stdcall function_202e90(long arg_0, long arg_1, long arg_2)
{
    long const *local_0 = &arg_0;
    long const *local_1 = &arg_2;
    long local_2 = g_510c54->game_time;
    byte *local_3 = (byte *)squad_get(*local_0);
    *(long *)(local_3 + 0x2c) = local_2;
    *(short *)(local_3 + 0x62) = NONE;
    *(short *)(local_3 + 0x64) = NONE;
    local_3[0x60] = false;
    if (*(short *)(local_3 + 0x2a) != (short)arg_1 ||
        (byte)(((dword)local_3[2] & 0x40) >> 6) != (byte)*local_1)
    {
        byte *local_4 = NULL;
        if ((short)arg_1 >= 0 && (short)arg_1 < *(long *)((byte *)g_4e0350 + 0x240))
            local_4 = *(byte **)((byte *)g_4e0350 + 0x244) + (short)arg_1 * 0x7c;
        *(short *)(local_3 + 0x2a) = (short)arg_1;
        if ((byte)*local_1)
            local_3[2] |= 0x40;
        else
            local_3[2] &= ~0x40;
        *(long *)(local_3 + 0x80) = NONE;
        s_squad_actor_iterator local_5;
        if (g_4f55d0->active)
        {
            if (*local_0 == NONE)
                local_5.next_actor_index = g_4f55d0->unknown14;
            else
                local_5.next_actor_index = squad_get(*local_0)->first_actor_index;
        }
        while (g_4f55d0->active && local_5.next_actor_index != NONE)
        {
            long local_6 = local_5.next_actor_index;
            s_actor_datum *local_7 = actor_datum_get(local_6);
            local_5.next_actor_index = local_7->next_actor_index;
            function_1e4570(local_6, (s_actor_options const *)local_4);
        }
        if (local_4)
        {
            if (local_4[0x24] & 0x10)
                function_204450((short)arg_1, *local_0);
            function_291740((short)arg_1, *local_0 & 0xffff);
            short local_8 = *(short *)(local_4 + 0x20);
            if (local_8 == NONE)
            {
                for (long local_9 = 0; local_9 < 5; local_9++)
                    ((s_flag_bits *)(local_3 + 0x84))->d[local_9] = NONE;
            }
            else if (local_8 >= 0 && local_8 < *(long *)((byte *)g_4e0350 + 0x150))
            {
                long local_10 = *(long *)(*(byte **)((byte *)g_4e0350 + 0x154) + local_8 * 8 + 4);
                if (local_10 != NONE)
                    *(s_flag_bits *)(local_3 + 0x84) = *(s_flag_bits *)(g_4e3b44[local_10 & 0xffff].bytes + 0x38);
            }
            if (*(short *)(local_4 + 0x28) > 0)
            {
                function_204d30(&local_5, *local_0);
                long local_11 = NONE;
                while (g_4f55d0->active && local_5.next_actor_index != NONE)
                {
                    s_actor_datum *local_12 = actor_datum_get(local_5.next_actor_index);
                    local_5.next_actor_index = local_12->next_actor_index;
                    long local_13 = local_12->clump_object_index;
                    if (local_13 != NONE && local_13 != local_11)
                    {
                        byte *local_14 = g_502420->data + (local_13 & 0xffff) * 0x50;
                        *(short *)(local_14 + 0x26) = 0;
                        *(long *)(local_14 + 0x28) = 0;
                        function_26b290(local_12->clump_object_index);
                        local_11 = local_12->clump_object_index;
                    }
                }
            }
        }
        else
            *(s_flag_bits *)(local_3 + 0x84) = g_557c74;
    }
}

void function_201520(short arg_0, word arg_1, long arg_2, long arg_3, long arg_4);

struct s_200d80
{
    s_squad_datum *field_0;
    s_record_pool_iterator field_4;
    long field_10;
};

PRIVATE __forceinline s_squad_datum *function_200db0(s_squad_record_iterator *arg_0)
{
    s_squad_datum *local_0 = NULL;
    if (g_4f55d0->active)
    {
        local_0 = (s_squad_datum *)data_iterator_next_inlined(&arg_0->records);
        arg_0->squad_index = arg_0->records.datum_index & 0xffff;
        arg_0->squad = local_0;
    }
    return local_0;
}

// @retail 0x200d80
void function_200d80(void)
{
    s_squad_record_iterator local_0;
    if (g_4f55d0->active)
    {
        local_0.records.data = g_51e9d8;
        local_0.records.index = NONE;
    }
    while (function_200db0(&local_0))
    {
        long local_1 = local_0.squad_index;
        dword local_2 = *(dword *)(*(byte **)((byte *)g_4e0350 + 0x164) + (word)local_1 * 0x74 + 0x20);
        if ((bool)((local_2 >> 12) & 1))
            function_201520(NONE, (word)local_1, NONE, 0, 0);
    }
}

void __stdcall function_203120(long arg_0, long arg_1, long arg_2);

// @retail 0x201100
void __stdcall function_201100(long arg_0, dword *arg_1)
{
    long local_0 = arg_0;
    (void)&arg_0;
    dword *local_1;
    (void)&arg_1;
    byte *local_2 = *(byte **)((byte *)g_4e0350 + 0x15c) + (local_0 & 0xffff) * 0x24;
    s_record_pool *local_10 = g_51e9dc;
    byte *local_3 = local_10->data + (local_0 & 0xffff) * 0x38;
    if (!local_3[0x23])
    {
        arg_0 = 1 << (local_0 & 0x1f);
        volatile long const *local_4 = &arg_0;
        local_1 = arg_1;
        arg_1 += local_0 >> 5;
        if (!(*arg_1 & *local_4))
        {
            long local_6 = *(short *)(local_2 + 0x20);
            byte *local_7 = NULL;
            local_3[0x23] = true;
            if (local_6 != NONE)
            {
                local_7 = local_10->data + (local_6 & 0xffff) * 0x38;
                if (!local_7[0x23])
                    function_201100(local_6, local_1);
            }
            short local_8 = *(short *)(local_2 + 0x22);
            if (local_8 >= 0 && local_8 < *(long *)((byte *)g_4e0350 + 0x240))
                function_203120(local_0, local_8, true);
            else if (*(short *)(local_3 + 0x14) != NONE && local_7 && local_7[0x23])
            {
                long local_9 = *(short *)(local_7 + 0x14);
                if ((short)local_9 != NONE)
                    function_203120(local_0, local_9, false);
            }
            *arg_1 |= *local_4;
        }
    }
}

// @retail 0x203120
void __stdcall function_203120(long arg_0, long arg_1, long arg_2)
{
    long const *local_0 = &arg_0;
    long const *local_1 = &arg_1;
    long *local_2 = &arg_2;
    byte *local_3 = (byte *)squad_group_get(*local_0);
    if (!(byte)*local_2 && local_3[0x16] && *(short *)(local_3 + 0x14) != NONE)
    {
        byte *local_4 = *(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_3 + 0x14) * 0x7c;
        if (local_4)
        {
            *local_2 = local_4[0x24] & 1;
            if ((byte)*local_2)
                return;
        }
    }
    function_202e30((short)*local_1, *local_0, (byte)*local_2);
    for (long local_5 = *(long *)(local_3 + 4); local_5 != NONE; )
    {
        byte *local_6 = (byte *)squad_group_get(local_5);
        if (local_6[0x23])
            function_203120(local_5, *local_1, false);
        local_5 = *(long *)(local_6 + 0xc);
    }
    for (long local_7 = *(long *)(local_3 + 8); local_7 != NONE; )
    {
        byte *local_8 = (byte *)squad_get(local_7);
        if (local_8[2] & 4)
        {
            byte *local_9 = NULL;
            short local_10 = *(short *)(local_8 + 0x2a);
            if (local_10 != NONE)
                local_9 = *(byte **)((byte *)g_4e0350 + 0x244) + local_10 * 0x7c;
            if (!local_9 || !(local_9[0x24] & 1))
                function_202e90(local_7, *local_1, false);
        }
        local_7 = *(short *)(local_8 + 0x74);
    }
}

short function_290cd0(short arg_0, long arg_1, long arg_2, short *arg_3, short *arg_4);
bool function_290e20(short arg_0, long arg_1, long arg_2);
bool function_1fb8a0(long arg_0, short arg_1);

// @retail 0x202be0
void function_202be0(long arg_0, long arg_1)
{
    long const *local_0 = &arg_1;
    byte *local_1 = (byte *)squad_get(*local_0);
    if (arg_0 != NONE && (!(local_1[2] & 0x40) ||
        *(short *)(local_1 + 0x2a) == NONE ||
        !((*(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_1 + 0x2a) * 0x7c)[0x24] & 1)))
        function_202e90(*local_0, (short)arg_0, false);
    else if ((local_1[2] & 0x40) && *(short *)(local_1 + 0x2a) != NONE)
    {
        long local_2 = NONE;
        short local_3;
        word local_4 = *(word *)(local_1 + 0x62);
        if (local_4 != (word)NONE)
        {
            if (*(short *)(local_1 + 0x64) > 0)
                goto local_16;
            local_2 = *(short *)(local_1 + 0x66);
        }
        else
        {
            local_4 = function_290cd0(*(short *)(local_1 + 0x2a), *local_0, NONE, &local_3, (short *)&local_2);
            if (local_4 == (word)NONE)
                goto local_16;
            if (local_3 > 0)
            {
                *(short *)(local_1 + 0x64) = local_3;
                *(word *)(local_1 + 0x62) = local_4;
                *(short *)(local_1 + 0x66) = (short)local_2;
                goto local_16;
            }
        }
        if (local_4 != (word)NONE)
        {
            function_202e90(*local_0, (short)local_4, true);
            if ((short)local_2 != NONE)
                function_1fb8a0(*local_0, (short)local_2);
        }
    }
local_16:
    if (*(short *)(local_1 + 0x62) == NONE && !local_1[0x60] && *(short *)(local_1 + 0x2a) != NONE)
    {
        byte *local_5 = *(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_1 + 0x2a) * 0x7c;
        if (local_5 && *(long *)(local_5 + 0x5c) > 0 && *(long *)(local_5 + 0x64) > 0)
        {
            byte *local_6 = *(byte **)(local_5 + 0x68);
            long local_7 = 0;
            short local_8;
            if (*(short *)local_6 == 0)
                local_8 = 1;
            else
                local_8 = *(short *)(local_6 + 4);
            for (short local_9 = 0; local_9 < *(long *)(local_6 + 4); local_9++)
            {
                byte *local_10 = *(byte **)(local_6 + 8) + local_9 * 8;
                word local_11 = *(word *)(local_10 + 4);
                if (local_11 != (word)NONE &&
                    function_290e20((short)local_11, *local_0, NONE) != (bool)(*(dword *)local_10 & 1))
                {
                    local_7++;
                    if ((short)local_7 >= (short)local_8)
                    {
                        short local_12 = *(short *)(local_6 + 2);
                        short local_13 = local_12 == 0 ? (short)NONE : (short)(local_12 + 0x6e);
                        s_squad_actor_iterator local_14;
                        local_1[0x60] = true;
                        function_204d30(&local_14, *local_0);
                        while (g_4f55d0->active && local_14.next_actor_index != NONE)
                        {
                            long local_15 = local_14.next_actor_index;
                            local_14.next_actor_index = actor_datum_get(local_15)->next_actor_index;
                            function_1e4570(local_15, (s_actor_options const *)local_5);
                        }
                        if (local_13 != NONE)
                            function_1fb8a0(*local_0, local_13);
                        break;
                    }
                }
            }
        }
    }
}

struct s_1fb7e0_data;
bool __stdcall function_20ba60(short arg_0, long arg_1, long arg_2, long arg_3, long arg_4, s_1fb7e0_data const *arg_5);

// @retail 0x202a00
void __stdcall function_202a00(long arg_0, long arg_1)
{
    long *local_0 = &arg_0;
    long const *local_1 = &arg_1;
    while (*local_0 != NONE)
    {
        long local_2 = NONE;
        long local_3 = *local_0 & 0xffff;
        byte *local_4 = g_51e9dc->data + local_3 * 0x38;
        if (*local_1 != NONE && (!local_4[0x16] ||
            *(short *)(local_4 + 0x14) == NONE ||
            !((*(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_4 + 0x14) * 0x7c)[0x24] & 1)))
        {
            *(short *)(local_4 + 0x14) = (short)*local_1;
            local_4[0x16] = false;
            *(long *)(local_4 + 0x18) = g_510c54->game_time;
            *(short *)(local_4 + 0x1c) = NONE;
            *(short *)(local_4 + 0x1e) = NONE;
            if ((short)*local_1 != NONE)
                function_291740((short)*local_1, local_3 | 0x40000000);
            local_2 = *local_1;
        }
        else if (local_4[0x16] && *(short *)(local_4 + 0x14) != NONE)
        {
            long local_5 = NONE;
            short local_6;
            word local_7 = *(word *)(local_4 + 0x1c);
            if (local_7 != (word)NONE)
            {
                if (*(short *)(local_4 + 0x1e) > 0)
                    goto local_11;
                local_5 = *(short *)(local_4 + 0x20);
            }
            else
            {
                local_7 = function_290cd0(*(short *)(local_4 + 0x14), NONE, *local_0, &local_6, (short *)&local_5);
                if (local_7 == (word)NONE)
                    goto local_11;
                if (local_6 > 0)
                {
                    *(short *)(local_4 + 0x1e) = local_6;
                    *(word *)(local_4 + 0x1c) = local_7;
                    *(short *)(local_4 + 0x20) = (short)local_5;
                    goto local_11;
                }
            }
            if (local_7 != (word)NONE)
            {
                local_2 = (short)local_7;
                function_202e30((short)local_2, *local_0, true);
                if ((short)local_5 != NONE)
                    function_20ba60((short)local_5, NONE, NONE, local_3 | 0x40000000, NONE, NULL);
            }
        }
local_11:
        if (*(long *)(local_4 + 4) != NONE)
            function_202a00(*(long *)(local_4 + 4), local_2);
        for (long local_8 = *(long *)(local_4 + 8); local_8 != NONE; )
        {
            byte *local_9 = (byte *)squad_get(local_8);
            function_202be0(local_2, local_8);
            local_8 = *(short *)(local_9 + 0x74);
        }
        *local_0 = *(long *)(local_4 + 0xc);
    }
}

PRIVATE __forceinline byte *function_202752(s_record_pool_iterator *arg_0)
{
    byte *local_0 = NULL;
    if (g_4f55d0->active)
        local_0 = data_iterator_next_inlined(arg_0);
    return local_0;
}

// @retail 0x202720
void function_202720(void)
{
    s_group_record_iterator local_0;
    s_200d80 local_1;
    if (g_4f55d0->active)
    {
        local_0.records.data = g_51e9dc;
        local_0.records.index = NONE;
    }
    while ((local_0.group = (s_squad_group_datum *)function_202752(&local_0.records)) != NULL)
    {
        if (*(short *)((byte *)local_0.group + 0x1e) > 0)
        {
            real local_2 = (real)g_510c54->field_2_3 * 2.0f;
            long local_3;
            __asm
            {
                fld local_2
                fistp local_3
            }
            *(short *)((byte *)local_0.group + 0x1e) -= (short)local_3;
        }
    }
    if (g_4f55d0->active)
    {
        local_1.field_4.data = g_51e9d8;
        local_1.field_4.index = NONE;
    }
    while ((local_1.field_0 = (s_squad_datum *)function_202752(&local_1.field_4)) != NULL)
    {
        if (*(short *)((byte *)local_1.field_0 + 0x64) > 0)
        {
            real local_2 = (real)g_510c54->field_2_3 * 2.0f;
            long local_3;
            __asm
            {
                fld local_2
                fistp local_3
            }
            *(short *)((byte *)local_1.field_0 + 0x64) -= (short)local_3;
        }
    }
    if (g_4f55d0->active)
    {
        local_0.records.data = g_51e9dc;
        local_0.records.index = NONE;
    }
    while ((local_0.group = (s_squad_group_datum *)function_202752(&local_0.records)) != NULL)
    {
        if (*(long *)((byte *)local_0.group + 0x10) == NONE)
            function_202a00(local_0.records.datum_index, NONE);
    }
    if (g_4f55d0->active)
    {
        local_1.field_4.data = g_51e9d8;
        local_1.field_4.index = NONE;
    }
    while ((local_1.field_0 = (s_squad_datum *)function_202752(&local_1.field_4)) != NULL)
    {
        if (*(long *)((byte *)local_1.field_0 + 0x6c) == NONE)
            function_202be0(NONE, (word)local_1.field_4.datum_index);
    }
}

// @retail 0x201c80
void function_201c80(void)
{
    long local_0 = g_510c54->game_time;
    volatile long local_7 = local_0;
    long local_1;
    long local_2;
    long local_3;
    real local_6 = (real)g_510c54->field_2_3;
    __asm
    {
        fld local_6
        fistp local_1
    }
    local_6 = (real)g_510c54->field_2_3 * 2.0f;
    __asm
    {
        fld local_6
        fistp local_2
    }
    local_6 = (real)g_510c54->field_2_3 * 0.5f;
    __asm
    {
        fld local_6
        fistp local_3
    }
    if (local_0 % local_1 == 0)
    {
        function_201df0();
        function_201ea0();
        function_202370();
    }
    if (local_0 % local_2 == 10)
    {
        function_2024c0();
        function_202720();
    }
    s_squad_record_iterator local_4;
    if (g_4f55d0->active)
    {
        local_4.records.data = g_51e9d8;
        local_4.records.index = NONE;
    }
    while (function_200db0(&local_4))
    {
        long local_5 = (word)local_4.squad_index;
        if (((word)local_5 + local_7) % local_3 == 0 && *(signed char *)((byte *)local_4.squad + 2) < 0)
            function_203360(local_5);
    }
    function_201df0();
}

struct s_effect_owner;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_a7870(long arg_0);
void function_11dfb0(vector2f const *arg_0, vector3f *arg_1, vector3f *arg_2);
long function_203780(long arg_0, short arg_1, short arg_2, bool arg_3);
long function_201010(long arg_0, short arg_1, short arg_2, short *arg_3);
void function_2011f0(long arg_0);

// @retail 0x201520
void function_201520(short arg_0, word arg_1, long arg_2, long arg_3, long arg_4)
{
    word const *local_0 = &arg_1;
    long const *local_1 = &arg_2;
    long const *local_2 = &arg_3;
    long const *local_3 = &arg_4;
    if (!g_4f55d0->active)
        return;
    byte *local_4 = (byte *)g_4e0350;
    byte *local_5 = *(byte **)(local_4 + 0x164) + *local_0 * 0x74;
    byte *local_6 = g_51e9d8->data + *local_0 * 0x98;
    long local_7 = NONE;
    bool local_8 = false;
    if ((short)*local_1 != NONE)
        arg_0 = 1;
    byte *local_9 = NULL;
    if (*(short *)(local_6 + 0x2a) != NONE)
        local_9 = *(byte **)(local_4 + 0x244) + *(short *)(local_6 + 0x2a) * 0x7c;
    if (arg_0 == NONE)
    {
        short local_10 = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;
        switch (local_10)
        {
        case 0: arg_0 = *(short *)(local_5 + 0x2c); break;
        case 1: arg_0 = *(short *)(local_5 + 0x2c); break;
        case 2: arg_0 = (short)((*(short *)(local_5 + 0x2c) + *(short *)(local_5 + 0x2e)) / 2); break;
        case 3: arg_0 = *(short *)(local_5 + 0x2e); break;
        }
    }
    dword local_11[1];
    dword local_12[1];
    memset(local_11, 0xff, ((*(long *)(local_5 + 0x48) + 31) >> 5) * sizeof(dword));
    local_12[0] = 0;
    if ((short)*local_1 == NONE)
        function_201460(*local_0, local_12);
    else if (!(byte)*local_2 && !function_201400(*local_0, (short)*local_1))
        arg_0 = 0;
    if (arg_0 > 0)
    {
        long local_13 = (word)arg_0;
        do
        {
            short local_14 = (short)*local_1;
            if (local_14 == NONE)
                local_14 = function_2039d0((short)*local_0, local_11, local_12);
            if (local_14 >= 0 && local_14 < *(long *)(local_5 + 0x48))
            {
                byte *local_15 = *(byte **)(local_5 + 0x4c) + local_14 * 0x64;
                long local_16 = NONE;
                word local_17 = *(word *)(local_15 + 0x3c);
                if (local_17 != (word)NONE)
                {
                    local_16 = function_201330((short)local_17);
                    if (!(byte)*local_2 && local_16 != NONE)
                    {
                        s_unit_request local_18;
                        local_18.type = 0x32;
                        *(word *)local_18.arguments = *local_0;
                        *(short *)(local_18.arguments + 2) = local_14;
                        if (function_e6900(local_16, &local_18))
                            local_8 = true;
                    }
                }
                if (!local_8)
                {
                    long local_19 = *(short *)(local_15 + 0x28);
                    if (local_19 == NONE)
                        local_19 = *(short *)(local_5 + 0x34);
                    long local_20 = NONE;
                    short local_21 = NONE;
                    short local_22 = *(short *)(local_15 + 0x2a);
                    if (local_19 != NONE && local_22 != 7)
                    {
                        local_20 = function_201010(local_7, (short)local_19, local_22, &local_21);
                        if (local_20 == NONE)
                        {
                            byte local_23[0xc4];
                            function_b7930(local_23, *(long *)(*(byte **)(local_4 + 0x7c) + local_19 * 0x28 + 4), NONE, NULL);
                            function_210850((s_type_c3b527 const *)(local_15 + 4), (point3f *)(local_23 + 0x1c));
                            function_11dfb0((vector2f const *)(local_15 + 0x14), (vector3f *)(local_23 + 0x28), (vector3f *)(local_23 + 0x34));
                            long local_24 = *(long *)(local_15 + 0x34);
                            if (!local_24)
                                local_24 = *(long *)(local_5 + 0x44);
                            if (local_24)
                                *(long *)(local_23 + 0xc) = local_24;
                            local_21 = NONE;
                            local_20 = function_b7b40(local_23);
                            if (local_20 != NONE)
                            {
                                byte *local_25 = *(byte **)(g_4e0300->data + (local_20 & 0xffff) * 12 + 8);
                                *(long *)(local_25 + 0x3a0) = *local_0;
                                *(long *)(local_25 + 0x3a4) = local_7;
                                *(long *)(local_25 + 0x3a8) = *(long *)local_15;
                                local_7 = local_20;
                                if (!local_6[0x76])
                                {
                                    byte *local_26 = g_4e3b44[*(long *)local_25 & 0xffff].bytes;
                                    if (*(short *)(local_26 + 0xc0))
                                        local_6[0x76] = local_26[0xc0];
                                }
                                else
                                    *(short *)(local_25 + 0x138) = (signed char)local_6[0x76];
                                function_a7870(local_20);
                                if (local_22 != 6)
                                {
                                    long local_27 = NONE;
                                    local_21 = function_200ee0(local_20, local_22, &local_27, NULL);
                                    if (local_21 != NONE)
                                        local_20 = local_27;
                                }
                            }
                        }
                        if (local_20 != NONE && (*(dword *)(local_5 + 0x20) & 0x2000))
                        {
                            byte *local_28 = *(byte **)(g_4e0300->data + (local_20 & 0xffff) * 12 + 8);
                            dword *local_29 = (dword *)(local_28 + 0x134);
                            *local_29 |= 0x1000;
                        }
                    }
                    if (local_22 != 6)
                    {
                        long local_30 = function_203780(local_16, local_14, (short)*local_0, (bool)*local_3);
                        if (local_30 != NONE)
                        {
                            ++*(short *)(local_6 + 0x24);
                            if (local_9)
                                function_1e4570(local_30, (s_actor_options const *)local_9);
                            if (local_20 != NONE && local_21 != NONE)
                            {
                                byte *local_31 = g_4f55f0->data + (local_30 & 0xffff) * 0x888;
                                s_unit_request local_32;
                                local_32.type = 0x1c;
                                local_32.type1c.object_index = local_20;
                                local_32.type1c.seat_index = local_21;
                                local_32.type1c.unknowna = true;
                                local_32.type1c.unknownb = false;
                                if (function_e6900(*(long *)(local_31 + 0x18), &local_32))
                                {
                                    byte *local_33 = *(byte **)(g_4e0300->data + (local_20 & 0xffff) * 12 + 8);
                                    *(short *)(local_33 + 0x138) = *(short *)(local_31 + 0x24);
                                }
                            }
                            function_201b70(*local_0, local_14, local_30);
                        }
                    }
                }
            }
        }
        while (--local_13);
        while (local_7 != NONE)
        {
            long local_34 = local_7;
            byte *local_35 = *(byte **)(g_4e0300->data + (local_7 & 0xffff) * 12 + 8);
            local_7 = *(long *)(local_35 + 0x3a4);
            byte *local_36 = *(byte **)(g_4e0300->data + (local_34 & 0xffff) * 12 + 8);
            *(long *)(local_36 + 0x3a4) = *(long *)(local_6 + 0x70);
            *(long *)(local_6 + 0x70) = local_34;
        }
        if (local_8)
            return;
    }
    function_2011f0(*local_0);
}
