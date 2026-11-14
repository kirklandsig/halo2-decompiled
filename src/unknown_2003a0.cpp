#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "squads.h"
#include "unknown_1a58b0.h"
#include <math.h>
#include <string.h>
#include "object_iterator.h"
#include "unit_requests.h"
#include "unknown_11cc90.h"
#include "data_iterator.h"
#include "props.h"

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
/* external linkage: machine 1's 0x1e0160 reads this table too */
extern short const g_44aee8[20] =
{
	3, 7, 3, 3, 3, 3, 1, 2, 2, 4, 4, 4, 5, 5, -1, 3, 7, 7, 7, 4
};

// Allocates a squad record and initialises it from its definition.
// @retail 0x200c20
void function_200c20(byte const *definition)
{
	long index = record_pool_allocate(g_51e9d8);
	s_squad_datum *squad = squad_get(index);
	*(word *)((byte *)squad + 2) = 0;
	if (*(dword const *)(definition + 0x20) & 0x100)
		squad->flag0 = true;
	else
		squad->flag0 = false;
	if (*(dword const *)(definition + 0x20) & 0x200)
		squad->flag1 = true;
	else
		squad->flag1 = false;
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
	memset((byte *)squad + 0x84, 0xff, 5 * sizeof(long));
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
	memset(&g_557c74, 0xff, sizeof(g_557c74));
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
		active |= (*(signed char const *)((byte const *)squad + 2) & 0x10) != 0;
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

/* walks the squad groups of g_51e9dc, only while the ai is active */
struct s_group_datum_walk
{
	s_data_datum_iterator iterator;
	long group_index;
};

static inline void group_datum_walk_new(s_group_datum_walk *walk)
{
	if (g_4f55d0->active)
	{
		walk->iterator.data = g_51e9dc;
		walk->iterator.index = NONE;
		walk->iterator.datum_index = NONE;
	}
}

/* the next squad group while the ai is active, or NULL */
PRIVATE __forceinline s_squad_group_datum *group_datum_walk_next(s_group_datum_walk *walk, bool active)
{
	s_squad_group_datum *group = NULL;

	if (active)
	{
		if (data_datum_iterator_next_inlined(&walk->iterator))
			group = (s_squad_group_datum *)walk->iterator.datum;
		walk->group_index = walk->iterator.datum_index;
	}
	return group;
}

/* runs 0x202420 for each squad group without a parent */
// @retail 0x202370
void function_202370(void)
{
	s_group_datum_walk groups;
	group_datum_walk_new(&groups);
	s_squad_group_datum *group;
	while ((group = group_datum_walk_next(&groups, g_4f55d0->active)) != NULL)
	{
		if (group->parent_index == NONE)
			function_202420(groups.group_index);
	}
}

/* runs 0x202570 for each squad group without a parent */
// @retail 0x2024c0
void function_2024c0(void)
{
	s_group_datum_walk groups;
	group_datum_walk_new(&groups);
	s_squad_group_datum *group;
	while ((group = group_datum_walk_next(&groups, g_4f55d0->active)) != NULL)
	{
		if (group->parent_index == NONE)
			function_202570(groups.group_index);
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
	short flags;
	bool flag_a;
	bool flag_b;
	bool flag_c;
	byte unknown0d[3];
	s_squad_definition_entry *definition;
};

s_squad_definition_entry *function_203330(s_squad_definition_view const *squad);

/* one candidate of a squad definition's selection blocks */
struct s_squad_selection
{
	short type;
	short unknown02;
	short palette_index;
	short entry_index;
};

// @retail 0x204ec0
void function_204ec0(s_squad_iterator *iterator, short squad_index, short flags, short mode)
{
	s_squad_datum *squad;
	s_squad_definition_entry *definition;
	for (;;)
	{
		if (squad_index == NONE)
		{
			iterator->next = NONE;
			return;
		}
		squad = squad_get((word)squad_index);
		definition = function_203330((s_squad_definition_view const *)squad);
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
			long *count_slot;
			if (alternate) count_slot = (long *)(definition->data + 0x5c);
			else count_slot = (long *)(definition->data + 0x54);
			bool found = false;
			for (short index = 0; index < *count_slot; index++)
			{
				s_squad_selection *selection;
				if (!alternate) selection = &(*(s_squad_selection **)(definition->data + 0x58))[index];
				else selection = &(*(s_squad_selection **)(definition->data + 0x60))[index];
				if (selection->type == 2)
				{
					found = true;
					break;
				}
			}
			if (!found)
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
			if (*(long *)(palette + 0x30) > 0)
				iterator->next = 0;
			else
				iterator->next = NONE;
		}
	}
	iterator->flags = flags;
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

PRIVATE __forceinline s_actor_datum *squad_actor_next_inline(s_squad_actor_iterator *iterator)
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
	byte unknown00[0x2a];
	word field2a;
	byte unknown2c[0x3c - 0x2c];
	short placement_index;
	byte unknown3e[2];
	char command_script_name[32];
	short command_script_index;
	byte unknown62[2];
};
typedef char assert_placement_entry_size[(sizeof(s_squad_placement_entry) == 0x64) ? 1 : -1];
typedef char assert_placement_entry_field2a[(offsetof(s_squad_placement_entry, field2a) == 0x2a) ? 1 : -1];
typedef char assert_placement_entry_alignment[(__alignof(s_squad_placement_entry) == 2) ? 1 : -1];

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

/* bit index of a placement in a squad's bit vector */
static inline bool placement_bit_2039d0(dword const *bits, long index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

/* a placement's priority, as an if/else */
static inline short placement_priority_2039d0(s_squad_placement_entry *entry)
{
	if (*((byte *)entry + 0x1c) & 8)
		return 100;
	return *(short *)((byte *)entry + 0x2a);
}

// @retail 0x2039d0
short function_2039d0(long squad_index, dword *available, dword const *blocked)
{
	(void)&available;
	(void)&blocked;
	short result = NONE;
	s_squad_placement_definition *squad;
	short maximum;
	short maximum_count;
	long index = squad_index & 0xffff;
	for (;;)
	{
		squad = &((s_squad_placement_table *)g_4e0350)->squads[index];
		maximum = NONE;
		maximum_count = 0;
		short available_count = 0;
		short blocked_count = 0;
		for (short i = 0; i < squad->count; i++)
		{
			if (placement_bit_2039d0(blocked, i))
				blocked_count++;
			else if (placement_bit_2039d0(available, i))
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
			goto done;
		if (!available_count && squad->count > 0)
			memset(available, 0xff, ((squad->count + 31) >> 5) * sizeof(dword));
		else
			break;
	}
	if (!maximum_count)
		goto done;
	short choice;
	if (maximum_count > 1)
	{
		g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
		choice = (short)(((g_4e7408->unknown0 >> 16) * maximum_count) >> 16);
	}
	else
		choice = 0;
	short found = 0;
	for (short i = 0; i < squad->count; i++)
	{
		if (placement_bit_2039d0(available, i) && !placement_bit_2039d0(blocked, i) && placement_priority_2039d0(&squad->entries[i]) == maximum)
		{
			if (found == choice)
			{
				available[i >> 5] &= ~(1 << (i & 31));
				result = i;
				goto done;
			}
			found++;
		}
	}
done:
	return result;
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
				while (iterator->next != NONE)
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
				}
			}
		}
		else
		{
			s_squad_definition_entry *definition = iterator->definition;
			if (definition)
			{
				long *count_slot;
				if (squad[0x60]) count_slot = (long *)(definition->data + 0x5c);
				else count_slot = (long *)(definition->data + 0x54);
				if (*count_slot > 0)
				{
					while (iterator->next != NONE)
					{
						s_squad_selection *selection;
						if (!squad[0x60]) selection = &(*(s_squad_selection **)(definition->data + 0x58))[iterator->next];
						else selection = &(*(s_squad_selection **)(definition->data + 0x60))[iterator->next];
						iterator->next++;
						if (iterator->next >= *count_slot)
							iterator->next = NONE;
						if (iterator->flags & (1 << selection->type))
						{
							short palette_index = selection->palette_index;
							byte *scenario = (byte *)g_4e0350;
							if (palette_index >= 0 && palette_index < *(long *)(scenario + 0x168))
							{
								byte *palette = *(byte **)(scenario + 0x16c) + (word)palette_index * 0x38;
								short entry_index = selection->entry_index;
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
					}
				}
			}
			else
			{
				while (iterator->next != NONE)
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
				}
			}
		}
	}
	iterator->current = result;
	return result;
}

long function_257ed0(long actor_index, short script_index, long thread_index);

PRIVATE __forceinline short squad_valid_command_script(s_squad_placement_table *scenario, short const &index, char const *name)
{
	if (index >= 0 && index < scenario->script_count)
	{
		char const *string = name;
		unsigned long length = 0;
		while (length < 32 && *string++)
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
	s_squad_placement_definition *squad = &((s_squad_placement_table *)g_4e0350)->squads[squad_index & 0xffff];
	s_squad_placement_entry *entry = &squad->entries[entry_index];
	short script_index = squad_valid_command_script((s_squad_placement_table *)g_4e0350, entry->command_script_index, entry->command_script_name);
	if (script_index == NONE)
		script_index = squad_valid_command_script((s_squad_placement_table *)g_4e0350, squad->command_script_index, squad->command_script_name);
	if (script_index != NONE)
		function_257ed0(actor_index, script_index, NONE);
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
		s_squad_placement_entry *entry = &squad->entries[entry_index];
		if (entry->placement_index != NONE)
		{
			long object_index = function_201330(entry->placement_index);
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
		s_squad_placement_entry *entry = &squad->entries[i];
		if (entry->placement_index != NONE)
		{
			long object_index = function_201330(entry->placement_index);
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

// score 31 (was 31): One-dword frame and first two passes agree; later parent loads still lose the retained record address.
// Round 76: retail's passes 3/4 need the field loaded twice at register allocation and the second load
// turned into `mov eax, ecx` afterwards (a volatile test, diagnostic only, gives the folded load into ECX
// and the dead `add eax, edx` but keeps the second load); a noinline declaration of 0x205280 in place
// of the in-body inline_depth pair 31 (not kept: DECOMPILING.md rules out both; a file-scope pair is
// allowed). Parent through a ternary select 66;
// `continue` on a missing parent in both passes 31; group and squad pointers at function scope 31;
// the group pointer reused for the sibling walk 31.
// parent field into ECX through [eax+edx+offset] and still forms the record pointer (add eax,edx) before
// the compare, then copies the value to EAX; ours forms the pointer first and loads through it into EAX.
// Tried: const current group 43; unsigned squad parent 43; unsigned group parent 63; unsigned sibling 43;
// full-width final index 63; parent-field view 43; parent view in both passes 82; append noinline
// declaration 43; const group/squad/both pool descriptors 43; definition reference 43; global
// optimization off 258; inline depth one/two 43/43; size optimization 146; current/parent/both record
// references 43; short child 60; readonly definition 43; parent reference with short child 60; force
// parent accessor 43; force with current reference 43. Fourth pass: a long parent local 43; the field
// read twice by index 43; a long pointer to the field 32; the squad pointer at function scope 31; a
// parent local beside the double read 31; no inline_depth pragma 175 (0x205280 inlines). Third pass: a
// long parent_index local 63; the field read twice by index 63; one child variable reused for the
// parent index 60 (as an assignment in the condition 60).
// Native group/squad parent-field references 37/31; child-head reference 34; group+squad 37;
// group+head 37, squad+head 34, all three 37; append inline declaration 31. All restored.
// Native parent field 31; shared squad-data base 118; nonempty list arm first 59;
// native field plus data/arm 118/59; data plus arm 127; all three 127;
// definition observed at every use 31 (fast). Opening body and record type restored.
// Native group-parent/head/group+squad/group+head/squad+head/all field pointers 37/34/37/37/34/37; readonly parents with head or squad pointers 37/37.
// Retail's dead `add eax, edx` and `mov eax, ecx` read as two loads of the field (the test through
// the folded index, the use through the record pointer), the second turned into a register copy
// after allocation; ours merges them into one load. Pass 4: test through a fresh getter and the
// call through the pointer 42 (one folded load, no pointer); test and call both through a getter
// inside the block 42; field read once into a group index 31; the call through a file-local
// forceinline helper taking the squad 31. Pass 3: the group through the global getter 94; the
// test by index with the group pointer formed inside the block 60.
// Initialized group-parent/squad-parent/empty-child predicates independently, in each pair
// and all together 31 each; optional parent-record result 66. Original executable source restored.
// Round 7 (lane 13): a noinline declaration of 0x205280 in place of the in-body inline_depth pair
// gives the same code, 31 (not kept: no new noinline, coordinator ruling 2026-10-30).
// Retail's ECX/dead-add shape is the field value and the record pointer in different
// registers (as in matched 0x1b8eb0, where the call takes the value in ECX); here the value only reaches
// EAX through `mov eax, ecx`. Pointer to the pass-4 field 31; the field spelled two ways 31 (diagnostic);
// by-reference parent getter 31 (diagnostic); test through the global getter with the pointer through the
// local pool, and the reverse, 57/56 (two loads, memory compare); per-pass loop counters 31; parent getter
// at each use in pass 3 34; pass-4 squad through the local pool 31.
// Round 41: pass-3 parent through the local pool 103; pass-3 getter argument through a fresh
// group getter 56; pass-4 call argument through a fresh squad getter 44 (the getter falls under
// the depth-zero pragma and goes out of line); the pragma pair around the whole pass-4 test 31;
// the pass-4 group index read once into a named local 42.
// Round 436: native external group/squad/both count-and-element descriptors 31/31/31;
// actual group/squad/paired descriptor receivers 31/31/31/31, readonly pair 31.
// All native descriptor declarations and executable forms restored; folded parent loads remain.
// Round 30 (lane 10): no other retail function has the folded-load/dead-add/copy shape with the
// pointer dead afterwards. Group pointer reused for the parent and the walk 31; record bound to
// `T *const &` in pass 3/4/both 31/31/31; `T const &` elements in both passes 31; `long const &`
// parent field in pass 4 31 (pass 3 37); pass-3 append through an inline helper taking the group
// pointer 31; sibling walk as a while loop 74; pass-4 while loop without the pragma 175; /Ob1 file
// flags without the pragma 31 (diagnostic: same code as the pragma).
// Round 51 (lane 9): compile-only screens of the parent test through an inline bool predicate, a named bool,
// `!= false`, the row expression with the pointer inside or before the block, the getter argument through the
// row expression and a named parent local: all one load, no form keeps two. Pass 3/pass 4/both as `continue`
// forms 31/31/31; pass-4 squad through `squad_get((word)i)` 31; pools declared before the definition 31; one
// group pointer for passes 1 and 3 31.
// Round 54 (lane 9): pass-4 argument through a fresh squad_get(i) read ahead of the depth pair 42 (all reads
// merge into one folded load); pass-3 parent getter at each use 34; pass-3 append through a link pointer
// (0x201a50's form) 82 (324); the first-child field read at each use 31; parent/last declared at the loop top 31.
// The matched examples of the folded-load/dead-add/copy shape (0x256b00, 0x26ace0) read the field three times
// (a test and two lookups); neither pass has a natural third read.
// Round 119 (lane 5, one-file harness, exact here): pass 3's parent pointer computed before the parent test 30
// (retail's folded load, dead add and `mov eax, ecx` appear, but the parent lookup sits before the branch);
// pass-4 index as `(long)i` 31; a pass-4 pointer to the parent field 31; pass-3 test through the local pool's
// element 58; test-only parent locals with the use re-reading the field, pass 3/pass 4/both 31/31/31.
// Round 46 (lane 1): retail's shape needs a second read of the parent field in each then-block, merged with
// the first before the test's value is forwarded: an unused `squad_group_get(field)` local ahead of pass 3's
// parent lookup and ahead of pass 4's call gives a byte match (fast and plain; kept: an unused getter result,
// kept only with a match, as the release build of a compiled-out check would leave it), pass 4's
// alone 40 (its code then retail's, the loop alignment off). Natural second reads in pass 3: the first child
// read through a second getter after/before the parent local 53/53 (one +4 field pointer for the load and the
// store), through the local pool 105 (358 bytes); the getter form with pass 4's unused local 34.
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
			// unused: the release build keeps only the lookup (its check is compiled out)
			s_squad_group_datum *checked_parent = squad_group_get(group->parent_index);
			s_squad_group_datum *parent = squad_group_get(group->parent_index);
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
		s_squad_datum *squad = squad_get(i);
		if (*(long *)squad->unknown6c != NONE)
		{
			// unused: the release build keeps only the lookup (its check is compiled out)
			s_squad_group_datum *checked_group = squad_group_get(*(long *)squad->unknown6c);
			#pragma inline_depth(0)
			function_205280(*(long *)squad->unknown6c, i);
			#pragma inline_depth()
		}
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
	s_squad_vehicle_view *object = (s_squad_vehicle_view *)object_get(object_index);
	if (object->squad_index != NONE)
		function_201a50(object->squad_index, object_index);
	if (squad_index != NONE)
	{
		s_squad_datum *squad = squad_get(squad_index);
		((s_squad_vehicle_view *)object_get(object_index))->next = squad_get(squad_index)->first_vehicle_index;
		squad->first_vehicle_index = object_index;
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

// @retail 0x203cc0
void function_203cc0(long squad_index)
{
	s_squad_activity_flags *squad = (s_squad_activity_flags *)squad_get(squad_index);
	squad->active = false;
	s_squad_actor_iterator iterator;
	squad_actor_begin_inline(&iterator, squad_index);
	s_actor_datum *actor;
	while ((actor = squad_actor_next_inline(&iterator)) != NULL)
	{
		if (*(bool *)((byte *)actor + 9))
		{
			actor = actor_datum_get(iterator.actor_index);
			if (*(bool *)((byte *)actor + 9))
			{
				*(bool *)((byte *)actor + 9) = false;
				*(long *)((byte *)actor + 0x10) = g_510c54->game_time;
				(*(short *)((byte *)g_4f55d0 + 0x36a))--;
			}
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

long character_get_difficulty_values(long index);
bool g_4f55df;

// @retail 0x2036c0
void function_2036c0(long definition_index, short mode, bool *enabled, bool *forced, real *value)
{
	(void)&mode;
	(void)&forced;
	s_squad_difficulty_values *data = (s_squad_difficulty_values *)character_get_difficulty_values(definition_index);
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

/* 0x210850 and 0x210ac0 (src/unknown_20fe20.cpp), which retail inlines in 0x204950 */
PRIVATE __forceinline point3f *function_204a4c(s_type_c3b527 const *point, point3f *out)
{
    if (point->output_index != NONE)
    {
        if (!function_2104b0(point->output_index, &point->point, out))
            *out = point->point;
    }
    else
    {
        *out = point->point;
    }

    return out;
}

PRIVATE __forceinline real function_204a12(s_type_c3b527 const *a, point3f const *b)
{
    real distance_squared;

    if (a->output_index == NONE)
    {
        vector3f v;
        vector3d_from_points3d(&a->point, b, &v);
        distance_squared = (v.k * v.k + v.i * v.i) + v.j * v.j;
    }
    else
    {
        point3f point;
        vector3f v;
        function_204a4c(a, &point);
        vector3d_from_points3d(&point, b, &v);
        distance_squared = (v.i * v.i + v.k * v.k) + v.j * v.j;
    }

    return (real)sqrt(distance_squared);
}

// @retail 0x204950
real function_204950(long squad_index, long actor_index, short mode)
{
    real result = 3.402823466e+38F;
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    if (*(short *)((byte *)squad_get(squad_index) + 0x7e) == g_4686c4 &&
        !function_1df560(*(short *)(actor + 0x24), (signed char)((byte *)squad_get(squad_index))[0x76]))
    {
        s_squad_iterator iterator;
        function_204ec0(&iterator, (short)squad_index, 15, mode);
        while (function_205010(&iterator) != NONE)
        {
            byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)iterator.palette_index * 0x38;
            byte *entry = *(byte **)(palette + 0x34) + iterator.current * 0x88;
            real distance = function_204a12((s_type_c3b527 const *)(entry + 0x24), (point3f const *)(actor + 0x238)) -
                *(real *)(entry + 0x34);
            real clamped = 0.0f;
            if (distance > 0.0f)
                clamped = distance;
            if (clamped < result)
                result = clamped;
        }
    }
    return result;
}


PRIVATE __forceinline s_squad_datum *function_200d83(s_squad_record_iterator *arg_0);

/* the squad nearest the actor by weighted distance, below 15: squads of another team count 1.333 times as far
   (those for which function_1df560(team, actor_team) is true are skipped), the actor's own squad 0.666 times;
   NONE when there is none */
// @retail 0x204b20
long function_204b20(long actor_index, short mode)
{
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    real best = 15.0f;
    long result = NONE;
    s_squad_record_iterator iterator;
    if (g_4f55d0->active)
    {
        iterator.records.data = g_51e9d8;
        iterator.records.index = NONE;
    }
    for (;;)
    {
        byte *squad = (byte *)function_200d83(&iterator);
        if (!squad)
            break;
        long squad_index = iterator.squad_index;
        if (!(squad[2] & 0x80))
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
        real distance = function_204950(squad_index, actor_index, mode);
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
        byte *const &object = squad_transfer_object(unit_index);
        long player_index = *(long *)(object + 0x13c);
        if (player_index != NONE)
        {
            byte *const &player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
            short controller = *(short *)(player + 0x28);
            if (controller != NONE)
            {
/* retail calls the controller update out of line */
#pragma inline_depth(0)
                function_1e6980((s_time_entry *)((byte *)g_51e9c0 + controller * 0x1b0 + 0x150),
                    (short)request.type, result);
#pragma inline_depth(255)
            }
        }
    }
    return result;
}

/* the next actor of a squad walk (the walk's current index is not kept) */
PRIVATE inline s_actor_datum *next_actor_204010(s_squad_actor_iterator *iterator)
{
    s_actor_datum *actor = NULL;
    if (g_4f55d0->active && iterator->next_actor_index != NONE)
    {
        actor = actor_datum_get(iterator->next_actor_index);
        iterator->next_actor_index = actor->next_actor_index;
    }
    return actor;
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
                short mode = 4 + (*(short *)(definition + 0x244) != 0);
                long selected = NONE;
                short seat = function_200ee0(target, mode, &selected, NULL);
                if (seat != NONE && squad_transfer_request(vehicle_index, selected, seat))
                    break;
                target = *(long *)(target_vehicle + 0x3a4);
            }
            vehicle_index = *(long *)(vehicle + 0x3a4);
        }
        s_squad_actor_iterator iterator;
        function_204d30(&iterator, other_squad_index);
        s_actor_datum *actor;
        while ((actor = next_actor_204010(&iterator)) != NULL)
        {
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
                    long selected = NONE;
                    byte *target_vehicle = squad_transfer_object(target);
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

/* fills the squad's area list around its target unit (a player's unit when the definition sets 0x10):
   up to ten areas whose reach covers the target, else the nearest one; the two area loops store that
   pair in opposite orders in retail, so each loop writes out its own test */
// Matches (was 224), lane 10 round 34: round 33's form with each area loop writing out its own nearest-area test
// (retail stores the pair area first in loop 1, palette first in loop 2; one shared body follows source order in
// both). Tried on the way: the shared helper storing area first 8; best through a whole s_reference copy 240;
// area-first stores with the nearest last 12; nearest between the stores 10/11; loop 1 written out with loop 2
// through the helper 4 (an earlier x87 add order flips); the helper area-first for loop 1 with loop 2 written out
// 0; both written out 0 (kept); both written out without the negated table test 3.
// Earlier (224): main's body (m3 review of lane 10 r33). The r33 form below reached 7 (4 with the
// ternary negated), but all of it rests on squad_area_consider taking its two indices as `short const &`
// so the inlined code re-reads entry+4/+6 after the distance call: with plain shorts the same form is 336.
// Like a volatile read, that reference only steers re-reads, so it is kept only with a byte match.
// Lane 10 r33 form at 7: loop 1's table choice falls through to the +0x60 arm (retail to +0x58), and loop
// 1's best-candidate stores differ: retail loads and stores the area index before the palette index
// there (loop 2 stores the palette index first, as ours does).
// Earlier at 224: Frame 0x3c agrees; count/definition/best/point homes, entry loads and packed reference tests remain.
// Packed comparisons 393; shared actor walk 344; split point scopes 344; early target 365;
// native reference indices 350; entry fields at use 344; inline distance call 678.
// Native player iterator 305; native entry cursor 316; word validation 305; matching-area
// result 315; distance home names 305; field-initialized best 357; area-entry name 305;
// player-record name 305.
// Pooled reference 323; entry-block reuse 323; delta workspace 305; shared scan cursor 305; valid
// actor continue 305.
// Pointer count block 264 plain; pointer table 323; shared squared product 305; single time exit 305; native selection fields 274. On pointer block: selection fields 285; earlier block 312; earlier block with selection 333; point name 264; block name 264; unused formal address removed 264.
// Lookup result-first single exit 257 plain kept; count-block lexical home 264; definition lexical home 264; outer copied-reference scope 264; byte bounded reference search 274.
// Frame 0x3c agrees; squad-offset/definition/count/best/point homes and entry reads remain.
// Earlier: frame 0x30 versus 0x3c; packed references and entry-index schedules differ.
// Earlier: Frame 0x3c agrees; stack homes, entry loads and packed reference tests
// differ. Native player iterator 305; native entry cursor 316; word validation 305; matching-area
// Earlier: Frame 0x3c agrees; stack homes, per-entry loads and reference tests remain.
// Earlier: round 106: Frame 0x3c agrees; stack homes, entry loads and packed reference tests remain. Pointer count block 264 plain; pointer table 323; shared squared product 305; single time exit 305; native selection fields 274. On pointer block: selection fields 285; earlier block 312; earlier block with selection 333; point name 264; block name 264; unused formal address removed 264.
// AC338: typed count pointer 264; per-iteration table choice 323; long validation 264; entry best 264; entry nearest 264; earlier player home 264.
// frame 0x3c agrees; definition/count/best/point homes, entry reloads and packed references differ.
// Frame 0x3c agrees; stack homes, entry loads and packed reference tests
// Frame 0x3c agrees; stack homes, per-entry loads and reference tests remain.
// round 106: Frame 0x3c agrees; stack homes, entry loads and packed reference tests remain. Pointer count block 264 plain; pointer table 323; shared squared product 305; single time exit 305; native selection fields 274. On pointer block: selection fields 285; earlier block 312; earlier block with selection 333; point name 264; block name 264; unused formal address removed 264.
// Round 110: branch table with native selection 323; actual palette field reference 279; reference with branch table 301; split point scopes on native iterator/count frame 264; early actor iterator workspace 264.
// Round 186 actual native records: definition 321; entry 257; both 321; area 257;
// definition/area 321; entry/area 257; all 321; all read-only 321. Strides and field widths preserved.
// Round 232: margin value input 342; margin field reference 257.
// located-point/extent inputs 268; located-point/extent reference 257.
// both scalar inputs 348; margin value/extent reference 342.
// both field references 257; native inputs/named reach 348.
// Round 344: joint frame-home names 257; byte/signed-byte validation 257/257; both with joint homes 257 each;
// early target result alone/with joint homes and byte/signed-byte validation 321 each.
// Native packed-view diagnostics 243/248/258 changed containing-record alignment and were rejected;
// the paired packed fields with actor-field reference scored 323. All original layouts restored.
// Round 347: consistent live squad/definition/both roots 491/451/576;
// those roots with multi-use point-getter result 496/453/572; joint displacement names 576,
// with returned point 572. Root/home allocation and lengths regress; original complete body restored.
// Round 198: Direct real entry fields 257; palette/area/both lookup field references
// 284/286/280; consideration palette field reference alone/with palette/area/both
// lookup references 271/271/273/273. Original value inputs and helper contracts restored.
// Round 27: lookup/consideration expansion boundaries, ordinary/inline/forced
// in all eight non-original pairs [546,546,305,546,546,305,462,462]. Original
// helper bodies and metadata restored; coupled count/best/point homes remain.
// Round 260: five complete consideration input/selection/output formal group orders,
// and nearest/best/both native output reference contracts, 257 in all eight forms.
// Original helper signature, both callers and complete executable body restored.
// Best-reference value result 362; result-first formal 362; point value with best result
// 409, result-first 409. Nearest-distance value result 382; result-first formal 382;
// point value with nearest result 447, result-first 447. Native pointer outputs restored.
// Round 176: Native player record 257; datum iterator 262; both 262; native squad state 257;
// squad/player 257; squad/datum 262; all three 262; all/shared actor walk 262.
// Datum storage grows the frame to 0x40; original records and both private helpers restored.
// Round 201: Size4/alignment2 packed view: best/actor/both 243/248/258; native index 254;
// best/index 255; actor/index 242 kept; both/index and word init 260/260. Guard-owned index 235,
// entry index 242; count or best/point homes retain those scores. Long/short/signed-byte/byte
// validation on the guard graph 235 each. Seven-point guard gain is below threshold. Keep native
// actor equality and the three-use signed index, plain-confirmed with no other measurement change.
// Coordinator review: the 242 needs a union/pragma-pack view of the shared s_reference type in globals.h;
// a shared-header layout change for one compare is not kept. Body restored (257).
// Round 436: lookup palette long 224 plain (kept); lookup area/both long 261/253;
// consideration palette/area/both long 259/304/305; both palette inputs/all indices long 258/276.
// The four-point lookup-both gain is below threshold; all other formal widths restored.
// Round 207: promoted repeated entry indices 253; fallback palette 253; both 273;
// retained absolute squad ordinal 347, with entry/fallback/both indices 335/329/305;
// ordinal with complete reach expression 347. Original carriers and helper body restored.
// Round 33 (lane 10): both reference compares through REFERENCE_EQUAL (retail compares each as one
// dword) 265 (1250 bytes); with loop 1's per-pass table choice, entry fields read at each use and
// the range tests nested in the loop (retail has no area null test, so the lookup helper went) 316;
// the area helper taking the two indices as `short const &` (retail re-reads entry+4/+6 and
// record+0x38 after the distance call and keeps loop 2's index in memory) 267 (aligned 41); the
// entry address formed in each arm of the table ternary 86 at 1266 bytes; the ternary's condition
// negated (retail falls through to +0x58) 83; the actor reference's element index in a long local
// 16; the player walk on the 16-byte datum iterator (retail's player index at 0x48, its datum slot
// at 0x3c) 4; with the ternary's condition back to `squad[0x60] ?` 7 (kept: the negation alone is a
// 3-point gain). On the way: area index stored first in the helper 84 (fixes loop 1, breaks
// loop 2); point declared per block 86, with the player iterator at function scope 86; on the 4
// form, the entry pair through an `s_reference const &` 4, nearest stored after the best fields 8,
// the helper's index parameters swapped 4 (identical), the nearest test written `*nearest > distance`
// 4, the area index through a pointer to its entry field 4; a 600 s permuter run, 58 tries, none
// below 4.
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
        s_data_datum_iterator players;
        players.data = g_4e8c24;
        players.index = NONE;
        while (data_datum_iterator_next_inlined(&players))
        {
            long unit_index = *(long *)(players.datum + 0x2c);
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
        byte *entries = squad[0x60] ? definition + 0x5c : definition + 0x54;
        if (*(long *)entries > 0)
        {
            for (short i = 0; i < *(long *)entries && (signed char)squad[0x30] < 10; i++)
            {
                byte *entry = !squad[0x60] ? *(byte **)(definition + 0x58) + i * 8 : *(byte **)(definition + 0x60) + i * 8;
                byte *scenario = (byte *)g_4e0350;
                if (*(short *)(entry + 4) >= 0 && *(short *)(entry + 4) < *(long *)(scenario + 0x168))
                {
                    byte *palette = *(byte **)(scenario + 0x16c) + (word)*(short *)(entry + 4) * 0x38;
                    if (*(short *)(entry + 6) >= 0 && *(short *)(entry + 6) < *(long *)(palette + 0x30))
                    {
                        byte *area = *(byte **)(palette + 0x34) + *(short *)(entry + 6) * 0x88;
                        real distance = function_210ac0((s_type_c3b527 const *)(area + 0x24), &point);
                        if (*(real *)(area + 0x34) + *(real const *)(definition + 0x50) > distance)
                        {
                            *(short *)(squad + 0x32 + (signed char)squad[0x30] * 4) = *(short *)(entry + 4);
                            *(short *)(squad + 0x34 + (signed char)squad[0x30] * 4) = *(short *)(entry + 6);
                            ++squad[0x30];
                        }
                        else if (distance < nearest)
                        {
                            nearest = distance;
                            best.unknown2 = *(short *)(entry + 6);
                            best.unknown0 = *(short *)(entry + 4);
                        }
                    }
                }
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
                    real distance = function_210ac0((s_type_c3b527 const *)(area + 0x24), &point);
                    if (*(real *)(area + 0x34) + *(real const *)(definition + 0x50) > distance)
                    {
                        *(short *)(squad + 0x32 + (signed char)squad[0x30] * 4) = *(short *)(local_b3aa32 + 0x38);
                        *(short *)(squad + 0x34 + (signed char)squad[0x30] * 4) = i;
                        ++squad[0x30];
                    }
                    else if (distance < nearest)
                    {
                        nearest = distance;
                        best.unknown0 = *(short *)(local_b3aa32 + 0x38);
                        best.unknown2 = i;
                    }
                }
            }
        }
        if (!squad[0x30] && !REFERENCE_EQUAL(best, g_471004))
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
            if (!REFERENCE_EQUAL(reference, g_470fa0) &&
                !(reference.unknown2 & 0x8000))
            {
                long entry_index = reference.unknown0;
                byte *palette = *(byte **)((byte *)g_4e0350 + 0x16c) + (word)reference.unknown2 * 0x38;
                if (entry_index >= 0 && entry_index < *(long *)(palette + 0x28))
                {
                    byte *entry = *(byte **)(palette + 0x2c) + entry_index * 0x20;
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

/* 0x203f60 stays out of line here as in 0x203d70's walk; the flag word is tested and returned
   through one pointer, and the tick is read through its own lookup as 0x203d70 reads it */
// score 0 (was 64): matches retail (m3 lane 11 r96, Fable 5.2 xhigh). The statement-level inline_depth pair
// around the 0x203f60 call is an exception the maintainer ruled on (2026-11-13) to DECOMPILING.md's rule against
// pragmas inside a body: it replaces the function-scope inline_depth(0) this function had, and it is the same pair
// around the same call that the matched 0x203d70 has carried since 10-14; the plain check of all 55 functions of
// this file is unchanged with it. Other in-body pragmas still need the maintainer's word.
// @retail 0x203bf0
bool function_203bf0(long squad_index)
{
    s_squad_datum *squad = squad_get(squad_index);
    short *flags = (short *)((byte *)squad + 2);
    if (*(short *)((byte *)squad_get(squad_index) + 0x7e) == g_4686c4)
    {
        if (!(*flags & 0x80))
        {
            s_squad_actor_iterator iterator;
            function_204d30(&iterator, squad_index);
            s_actor_datum *actor;
            while ((actor = squad_actor_next_inline(&iterator)) != NULL)
            {
                long actor_index = iterator.actor_index;
                if (!function_1e11b0(actor_index, true))
                {
                    *(long *)((byte *)actor + 0x34) = squad_index;
                    function_203ed0(actor_index, false);
                    #pragma inline_depth(0)
                    function_203f60(actor_index);
                    #pragma inline_depth()
                }
            }
        }
        *(long *)((byte *)squad + 0x78) = g_510c54->game_time;
        *(word *)((byte *)squad + 2) |= 0x80;
    }
    return (bool)((((dword)*flags) >> 7) & 1);
}
#pragma inline_depth(255)


extern s_record_pool *g_5044c8;
bool function_1e1de0(long actor_index);
bool function_1df5d0(short team_a, short team_b);

// @retail 0x203360
void function_203360(long squad_index)
{
    byte *squad = (byte *)squad_get(squad_index);
    bool hostile = false;
    *(word *)(squad + 2) &= ~0x10;
    *(short *)(squad + 0x16) = 0;
    *(short *)(squad + 0xc) = 0;
    *(short *)(squad + 0xa) = 0;
    *(real *)(squad + 0x10) = 0.0f;
    squad[0x26] = 0;
    s_squad_actor_iterator iterator;
    squad_actor_begin_inline(&iterator, squad_index);
    s_actor_datum *actor;
    while ((actor = squad_actor_next_inline(&iterator)) != NULL)
    {
        real health = 0.0f;
        word count;  /* retail sets it only on the two paths below; an actor with neither adds the previous value */
        if (actor->unit_index != NONE)
        {
            health = *(real *)(squad_transfer_object(actor->unit_index) + 0xec);
            count = 1;
        }
        else if (actor->perception_index != NONE)
        {
            byte *perception = g_5044c8->data + (actor->perception_index & 0xffff) * 0x34;
            health = (real)*(short *)(perception + 0x14) / (real)*(short *)(perception + 0x16);
            count = *(word *)(perception + 0x14);
        }
        *(short *)(squad + 0xa) += count;
        *(short *)(squad + 0xc) += *((byte *)actor + 7) * count;
        *(real *)(squad + 0x10) += health;
        /* retail calls the actor test here */
#pragma inline_depth(0)
        *(short *)(squad + 0x16) += function_1e1de0(iterator.actor_index) * count;
#pragma inline_depth()
        long tracking_index = *(long *)((byte *)actor + 0x338);
        if (tracking_index != NONE)
        {
            s_prop_node_view *node = prop_node_get(tracking_index);
            if (!function_1df5d0(*(short *)((byte *)actor + 0x24), prop_get(node->unknown08)->team))
                hostile = true;
            function_1e1de0(iterator.actor_index);
            if (actor->value086 > (signed char)squad[0x26])
                squad[0x26] = (byte)actor->value086;
            if (actor->value086 >= 7)
                *(word *)(squad + 2) |= 0x10;
        }
    }
    if (hostile)
        *(word *)(squad + 2) &= ~0x20;
    if (*(short *)(squad + 8) > 0)
    {
        real health = *(real *)(squad + 0x10) / (real)*(short *)(squad + 8);
        *(real *)(squad + 0x10) = health < 0.0f ? 0.0f : health;
    }
    *(word *)(squad + 2) &= ~0x100;
    s_squad_actor_iterator position_iterator;
    squad_actor_begin_inline(&position_iterator, squad_index);
    point3f *position = (point3f *)(squad + 0x18);
    *position = *g_468788;
    while ((actor = squad_actor_next_inline(&position_iterator)) != NULL)
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
        if ((*(dword *)(definition + 0x24) & 0x10) || ((*(dword *)(definition + 0x24) & 0x20) && *(short *)(definition + 0x4e) != NONE))
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
                s_actor_datum *previous;
                while ((previous = squad_actor_next_inline(&iterator)) != NULL)
                {
                    long previous_index = iterator.actor_index;
                    *(long *)((byte *)previous + 0x34) = squad_index;
                    function_203ed0(previous_index, false);
                    #pragma inline_depth(0)
                    function_203f60(previous_index);
                    #pragma inline_depth()
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
        function_1e11b0(actor_index, (bool)((((dword)*(short *)((byte *)squad + 2)) >> 7) & 1));
        short team = (signed char)((byte *)squad)[0x76];
        if (*(short *)((byte *)actor + 0x24) != team && !keep_team)
            function_1e1150(actor_index, team);
        squad->actor_count++;
        *(word *)((byte *)squad + 2) |= 0x100;
    }
}


#include "object_queries.h"
void function_11bed0(s_location *location, point3f const *point);

struct s_squad_datum_walk
{
    s_data_datum_iterator iterator;
    long walk_index;
};

/* the next squad while the ai is active, or NULL */
PRIVATE __forceinline s_squad_datum *squad_datum_walk_next(s_squad_datum_walk *walk, bool active)
{
    s_squad_datum *squad = NULL;
    if (active)
    {
        if (data_datum_iterator_next_inlined(&walk->iterator))
            squad = (s_squad_datum *)walk->iterator.datum;
        walk->walk_index = walk->iterator.datum_index & 0xffff;
    }
    return squad;
}

PRIVATE __forceinline long squad_tick_round(real ticks)
{
    long result;
    __asm
    {
        fld ticks
        fistp result
    }
    return result;
}

// MiniMax B (11-07): no new form, no lower score (157); the frame is 0x44 against retail's 0x54 and the two area
// locals (the structure pointer and the removed index) sit 4 bytes higher than retail's +0x30 and +0x38.
// score 157 (was 275): 1118 bytes against 1122, aligned 4. Left: frame 0x44 against 0x54 (point packs with the area
// matches retail byte for byte.
// MiniMax C (11-07): the location moved to function scope, its leaf field takes the intersection's store, and the
// area iterator's address is kept in that field, so the iterator keeps a frame slot of its own as retail's does.
// Match-only form (kept for the byte match): retail stores the iterator's address and each intersection into
// one frame slot that nothing reads; the source variable that slot belonged to is not known yet.
// Earlier 157 (was 275): 1118 bytes against 1122, aligned 4. Left: frame 0x44 against 0x54 (point packs with the area
// iterator) and the intersection's store with no read. Earlier scores in this note are on the older body.
// MiniMax R (11-05): no new form, no lower score; retail's frame is 0x54 with the elapsed conversion before
// the pushes and its clusters/elapsed/structure at +0x04/+0x08/+0x0c, ours 0x48 with them at +0x14/+0x18.
// Earlier 275 (was 275): Frame 0x48 versus 0x54, early saved-register pushes, iterator/point homes and intersection spill remain.
// Round 73: retail keeps the squad walk's pool pointer in memory (reloaded each pass), ours in EBX,
// which forces the pushes first. Walk as `while ((squad = step) != NULL)` 319; long squad index 275;
// flags at +2 read as a word, plain/(dword) cast 294/294 (retail's xor/mov bx load; aligned diff
// 160 -> 158); with a long index passed unmasked to the squad getter 301.
// This round: structure at uses 305; cluster root at uses 302; both roots at uses 300; shared final
// bit test 275; direct bitmap countdown 274; root observations shared test 300; root observations
// countdown 302; roots countdown shared test 302.
// frame 0x48 versus 0x54; iterator/point homes, early saves
// and intersection spill remain. Native workspace names 275; shared point setter 275;
// separate squad-phase scope 275; forward bitmap scan 298; Boolean overlap OR 303.
// Frame 0x48 versus 0x54, iterator/point homes and intersection spill remain; original body and declarations restored.
// Failure-first/composed squad steps 316/306; native bitmap union 294; point copy/update 335;
// unsigned current handle 275; declared word bitfield 294.
// Long overlap 321; raised-component loop 275; shared current-handle scope 275; per-squad area scope 275; long actor wake 301.
// Squad/area/both iterator value initialization 298/333/337; rounding scratch role names 275;
// double rounding input 298 (frame 0x54); native integer result 275; all-row displacement 320;
// live state getter 275. Original body and rounding helper restored; early saves and intersection spill remain.
// Exact tick result 312; native word index/actor type 275/275; complete point value 275;
// value/lexical order 275; failed-team result 275; first-overlap exit 305; early output object 275.
// Round 260: the existing shared rounding producer at all three conversions, with every
// subset of entry area/point-pair/location storage, 275 in all eight forms. Original
// producer, helper bodies, declaration scopes and executable body restored.
// Native squad/actor/both byte wake results 275/285/285; eager logical update 346;
// retained actor eligibility 337; squad byte/logical 346; squad byte/eligibility 337;
// both bytes/eligibility 344. Original Boolean results and predicate placement restored.
// Round 176: Complemented squad/overlap/both/actor/squad-actor/overlap-actor/all carriers
// 330/307/316/322/345/336/330; all with shared raised-point scaled add 330.
// Original complete result graph and every eager call restored.
// Round 201: Native area result/resolved point/both 294/281/291; direct iterator handle 275; handle
// with area/point/both 294/281/291; complete graph with word flag 293. Private iterator
// inline/ordinary 432; forced cdecl/stdcall 275/275; inline or ordinary cdecl/stdcall 432 each,
// losing matched 200d80/201df0/204ca0. Original target and both private helper declarations
// restored. Public area contracts were blocked by a held declaration file.
// Round 21 (lane 15), aligned instruction count in brackets (baseline 160): retail's squad walk
// is the file's squad_datum_walk_next (NULL preset, active test each pass, masked walk index;
// matched 0x202720 uses it): with a word index 335 [139], long index 333 [133] (1088 bytes,
// frame still 0x48); area iterator at function scope/before the walk/first 333 each; with
// `else if (active && timer > elapsed) ... else if (active) deactivate` (retail tests the
// computed flags & 0x80 twice) 312 [126]; word read of the wake flags 314 [133]; both 309
// [124]; point/raised/location at function scope, with the area iterator too 309/309. Retail
// gives the area iterator its own slot (0x40) and shares the squad walk with the raised point;
// ours shares the area iterator with the point; scope changes don't move it. The walk renamed to
// sort before the area iterator (same-size slots by name): original body 275, walk form 333.
// Round 32 (one-file LTCG closure harness, exact; [aligned]): retail passes the masked walk index
// to the long-parameter callees with no word casts: walk form 335 [139] (1087 bytes), without the
// casts 333 [133] (1088 bytes); record step without them 307 [144] (1108 bytes), long index without
// them 275 [155]; walk with the word wake read [133], plus the else-if tail 309 [124] (1108 bytes);
// overlap written straight into wake 318 [156] (1103 bytes), with the else-if tail [202]; words and
// overlap declared at the loop body's top 275. The pool stays in EBX across the walk (so the pushes
// come first) even with the area block, the definition block, the tail or the actor phase cut out.
// Round 475: Failure-first actor completion, with bitmap pointer scan/short overlap/native
// offset-vector subsets 0..7: 301/331/320/349/316/346/338/364. All restored; frame
// 0x48 versus 0x54, early saves, pool/intersection and current-handle homes remain.
// Round 51 (lane 1, fast): the area iterator, the two points, the wake flag or the location declared at
// function scope 275 each (identical code: scope never moves the shared homes); a long squad index 275 (as
// before); the walk through the existing out-of-line record_pool_iterator_step 415 (1007 bytes: the step
// stays a call, also 0x204b20 202, 0x201df0 139, 0x200d80 158). Retail keeps the walk iterator in memory
// (data reloaded each pass) as an escaped local would be.
// Round 135 (m3 lane 5): retail keeps the overlap loop's intersection in a stack home (a dead `mov [esp+0x28], esi`
// into the location's slot) and the wake byte at [esp+0x13]; ours folds the intersection into `test [ecx], edx`.
// `dword intersection` at function scope 275; declared outside the overlap loop 275; `bool overlap` at function
// scope 275; `long words` at function scope 275; a `dword const *` to the area's cluster bits 302 (1119 bytes).
// Round 509: Persistent idle/remaining/last-seen timer field receivers, subsets
// 1/2/4/3/5/6/7, 275 each; all with area bitmap receiver 302 (1119 bytes).
// Original body restored; frame, iterator pool, point homes and intersection spill remain.
// P270: existing generic squad producer with entry-owned area/point-pair/location
// workspace subsets 0..7: 306/306/306/306/306/306/306/306. Original producer,
// declaration scopes and body restored; frame, pool and intersection homes remain.
// Round 119 (lane 7, fast): retail's struct is s_squad_datum_walk (data +4, index +0xc at esp+0x3c). 0x202720's walk with
// `while ((squad = next) != NULL)`, long elapsed, long index without word casts, actor looked up before the current copy,
// word active local with the else-if tail and the word wake read 317 (1100), aligned 125; through 0x200d83 291 (1116),
// aligned 142; 0x200db0 302, aligned 163; next_squad_record 365 (1057); structure read first 305; walk renamed 291;
// sixteen scope combinations of areas/point/raised/location 291 (frame 0x44 each); walk init through an inline helper or a
// pointer to the walk 322 (1100). On the existing while(active) body, 31 combinations of those forms: lowest 273 (1133;
// long index and word wake read, aligned 152); all of them 295 (1119), aligned 99: retail's late pushes and the pool
// reloaded per pass into EDI (without the outer active test it is hoisted into EBX and the pushes come first). That
// body with 0x202720's walk and structure first 307 (1102), aligned 74, the next round's lead. Left there: the second
// active test on the first pass, frame 0x44 against 0x54 (retail's areas shares with nothing), the 16-bit active mask,
// the overlap loop's intersection in a stack home with overlap in DL, and the wake store order. Nothing kept.
// Round 140 (m3 lane 6, fast): a compiled-out `do {} while (false);` after the squad's wake/timer tail, at the
// overlap loop's end, after the area loop, after the +0x7c timer update or before the actor's 0x1e11b0 call:
// 275 each (same code).
// Round 63 (m3 lane 1, fast): 0x202720's squad_datum_walk_next walk + long elapsed + long index without word
// casts + word wake read + `word active` else-if tail 312 (1103; aligned mnemonics 79 -> 36: retail's late
// pushes, walk slots and block order); the same forms on the while(active) body 308 (1108), without the tail 312;
// on the walk form areas/point/raised/location at function scope or renamed 312 each; the area overlap as a
// forceinline helper 301. Diagnostic: an extra use of point inside the squad walk gives retail's frame 0x54 and
// every slot (point 0x30, walk/raised 0x3c, areas 0x50), 299: retail's point is live across the walk.
// Round 143 (m3 lane 6, fast): the state test through the existing 0x1f8660 call 324 (1092 B: it stays out of line,
// so retail inlined it); elapsed without the `(short)` cast 312 (1104 B, aligned 114 against 160: retail reloads
// elapsed as a dword, gets the late pushes and the pool reloaded per pass in EDI; the frame stays 0x48); on that,
// a word wake read 312, `while ((squad = function_200d83(&squads)) != NULL)` 318, a long index without word casts
// 312 (aligned 109).
// DL259: Declared cluster array/shared scaled point/native movement fields,
// seven subsets then readonly movement state: 275/275/275/275/275/275/275/275.
// Original producers, includes and complete body restored; frame, iterator pool,
// point/current-handle homes and intersection spill remain.
// B414: Non-wake idle/area ownership, call-first area admission and actor-presence regions, seven subsets;
// complete graph with empty-intersection continue: 293/275/275/293/293/275/293/293. Original complete body restored.
// Frame 0x48 versus 0x54, iterator pool, point/area homes and intersection spill remain.
// Round 69 (lane 11, fast; [aligned]/{registers masked}): 0x202720's walk, long elapsed, long index, word wake read, word active
// tail 312 (1103) [75] {31}, without the definition block (diagnostic) 381 (1041) [107], the same with the walk in a bare scope, areas at function scope or nested, or both renamed to sort
// first 312 each (frame stays 0x48); walk_index read directly at each use 312; a short index 309 [80]; a short elapsed 311. The walk
// inside `while (active)` with `if (!squad) break;` 289 (1104) [51] {30}: retail's late pushes and the pool reloaded per pass in
// EDI; on it a short active or a word flags local 289 (same code), intersection assigned in the test 289, overlap `|=` 289 (1109)
// [92], structure ahead of clusters 285 (1103) [48] {26}: retail's head registers, the next round's lead. Left there: frame, the
// 16-bit active test kept in CX and re-tested at the deactivate block, overlap in DL with the intersection's dead store. Diagnostic,
// not kept: a function-scope point used again in the area loop gives frame 0x54 (290, 1119 with the extra call, [48] {25}).
// J255: Coherent ordinary-inline squad-test, actor state update and actor-cluster
// provider subsets, then squad-deactivation boundary: 275/275/275/275/275/275/275/275.
// NOT kept: ties/regressions. Every provider contract and complete body restored;
// frame, iterator/point homes and bitmap-intersection spill remain.
// B444: Complete point/raised/location zero construction, seven subsets at native
// output scopes then all at entry: 331/315/343/304/340/320/352/351. NOT kept: frame/layout still misses
// retail and initialization extends code; original complete outputs and body restored.
// N372: Actual current-cursor/saved-successor for/while graphs, raw and existing typed
// actor receiver: 318/318/300/300/318/318/300/300. Original body restored; all retain
// frame 0x48 versus 0x54. Iterator pool, point/area homes and intersection spill remain.
// Round 151 (m3 lane 5, fast; [aligned]): the causes were four forms that only work together: 0x202720's plain walk, the
// actor looked up before the current copy, `wake |= overlap` (the known-false wake folds it to retail's plain store with
// overlap in DL), and the deactivate test written twice with no local (the shared copy keeps retail's `mov cx; and cx,
// 0x80`, and its second `test cx, cx`). In order: while(active) walk with structure first 285 (1103) [66]; plus actor first
// 307 (1102) [44]; overlap byte, char, long with a cast 284/284/286 (1118) [27/27/28]; overlap straight into wake 227
// (1106) [41]; overlap forceinline helper, or at function, squad or area scope 307 each [43]; `if (overlap) wake = true`
// 294 (1118) [55]; `wake |= overlap` 269 (1118) [25]; twice-written test on the while(active) walk 249 (1104) [39]; on the
// plain walk 157 (1118) [4], kept. Word flags local, `active &= 0x80`, late-folded masks through inline helpers, at
// any scope: all narrow to `mov al`, so only the repeated test gives the 16-bit form.
// Each kept form measured alone on the 157 body: clusters read first 163 (1118) [9]; nested deactivate test 287 (1101) [66];
// dword wake read 240 [7]; `(short)` elapsed 276 [55]; plain `wake = overlap` 166 (1116) [43]; current copied before the
// lookup 134 (1119) [26], not kept: lower only because its frame 0x48 lines up the low slots, and it leaves retail's shape.
// Not kept: `dword const &intersection` 16 (1122) [2]: retail's store with no read and its registers; frame slots only are
// left, but it is a one-use reference, so it is only keepable on a match.
// Frame cause: retail treats 0x210850's `out` as escaped, so point packs by its C++ scope. BLOCKED-BY 0x142570: point
// at function scope is identical here until `out` escapes there. Diagnostic, reverted: on the 16 form, point at function
// scope with `out` read through a reference inside 0x142570 gives retail's frame and a full match of this function, but
// that spelling changes 0x142570 (207 bytes) and 0x2104b0 (10). The same read in 0x210850 or 0x2104b0, or a reference to a
// cast copy of `out`, also gives frame 0x54 but changes those two (63-73 and 251 bytes). Next: a spelling in 0x142570
// that keeps its bytes.
// Round 76 (m3 lane 1, fast; on the 16 form): the raised point read through 0x210850's returned pointer 38 (1119 B, frame
// 0x48); that with the point at function scope 21 (1119 B): retail's frame 0x54 and every slot, only the three reads go
// through EAX where retail reads the point's slot, so a used return value is enough for the escape (a pointer to
// the whole local, so not keepable even with a match); the raised point
// through the existing vector3f_add_scaled with `&point`, point at block or function scope, 16/16 (forceinline folds,
// no escape); 0x210850's two copies as memcpy 16 (no escape; 0x210850 keeps its bytes). 0x142570's file is held by
// another lane this round, so BLOCKED-BY 0x142570 stands.
// @retail 0x201ea0
void function_201ea0(void)
{
    long elapsed = squad_tick_round((real)g_510c54->field_2_3);
    byte *structure = (byte *)g_4e0348;
    dword const *clusters = (dword const *)((byte *)g_4e6948 + 0x1138);
    s_squad_datum_walk squads;
    s_location location;
    if (g_4f55d0->active)
    {
        squads.iterator.data = g_51e9d8;
        squads.iterator.index = NONE;
    }
    byte *squad;
    while ((squad = (byte *)squad_datum_walk_next(&squads, g_4f55d0->active)) != NULL)
    {
        long squad_index = squads.walk_index;
        bool wake = false;
        if (*(short *)((byte *)squad_get(squad_index) + 0x7e) == g_4686c4)
        {
            wake = (bool)((*(word *)((byte *)squad + 2) >> 9) & 1);
            if (!wake || *(long *)(squad + 0x78) == NONE)
                wake |= function_202310(squad_index, clusters, 0x200);
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
                function_204ec0(&areas, (short)squad_index, 15, 2);
                location.leaf_index = (long)&areas;
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
                        location.leaf_index = clusters[words] & ((dword *)(area + 0x3c))[words];
                        if (location.leaf_index)
                            overlap = true;
                    }
                    wake |= overlap;
                }
            }
        }
        if (wake)
        {
            *(short *)(squad + 0x28) = (short)squad_tick_round((real)g_510c54->field_2_3 * 5.0f);
            if (!(squad[2] & 0x80))
                function_203bf0(squad_index);
        }
        else if ((*(word *)(squad + 2) & 0x80) && *(short *)(squad + 0x28) > elapsed)
            *(short *)(squad + 0x28) -= elapsed;
        else if (*(word *)(squad + 2) & 0x80)
            function_203cc0(squad_index);
    }
    long actor_index = g_4f55d0->unknown14;
    long current_time = g_510c54->game_time;
    while (actor_index != NONE)
    {
        byte *actor = (byte *)actor_datum_get(actor_index);
        long current = actor_index;
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
    s_squad_record_iterator local_0;
    if (g_4f55d0->active)
    {
        local_0.records.data = g_51e9d8;
        local_0.records.index = NONE;
    }
    while (function_200d83(&local_0))
    {
        if (*(word *)((byte *)local_0.squad + 2) & 0x100)
            function_203360(local_0.squad_index);
    }
}

void function_1e3400(long actor_index, short squad_index);
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
void __stdcall function_202e90(long arg_0, short arg_1, long arg_2)
{
    struct s_entry_view
    {
        long field_0;
        long field_4;
    };
    long local_2 = g_510c54->game_time;
    byte *local_3 = (byte *)squad_get(arg_0);
    *(long *)(local_3 + 0x2c) = local_2;
    *(short *)(local_3 + 0x62) = NONE;
    *(short *)(local_3 + 0x64) = NONE;
    local_3[0x60] = false;
    if (*(short *)(local_3 + 0x2a) != (short)arg_1 ||
        ((((dword)*(short *)(local_3 + 2)) >> 6) & 1) != (byte)arg_2)
    {
        byte *local_4 = NULL;
        if ((short)arg_1 >= 0 && (short)arg_1 < *(long *)((byte *)g_4e0350 + 0x240))
            local_4 = *(byte **)((byte *)g_4e0350 + 0x244) + (short)arg_1 * 0x7c;
        *(short *)(local_3 + 0x2a) = (short)arg_1;
        if ((byte)arg_2)
            *(word *)(local_3 + 2) |= 0x40;
        else
            *(word *)(local_3 + 2) &= ~0x40;
        *(long *)(local_3 + 0x80) = NONE;
        s_squad_actor_iterator local_5;
        squad_actors_begin(&local_5, arg_0);
        s_actor_datum *actor;
        while ((actor = squad_actors_next(&local_5)) != NULL)
            function_1e4570(local_5.actor_index, (s_actor_options const *)local_4);
        if (local_4)
        {
            if (local_4[0x24] & 0x10)
                function_204450((short)arg_1, arg_0);
            function_291740((short)arg_1, arg_0 & 0xffff);
            short local_8 = *(short *)(local_4 + 0x20);
            if (local_8 == NONE)
            {
                memset(local_3 + 0x84, 0xff, sizeof(s_flag_bits));
            }
            else if (local_8 >= 0 && local_8 < *(long *)((byte *)g_4e0350 + 0x150))
            {
                s_entry_view const *const &entry = (s_entry_view const *)*(byte **)((byte *)g_4e0350 + 0x154) + local_8;
                long local_10 = entry->field_4;
                if (local_10 != NONE)
                    *(s_flag_bits *)(local_3 + 0x84) = *(s_flag_bits const *)(g_4e3b44[local_10 & 0xffff].bytes + 0x38);
            }
            if (*(short *)(local_4 + 0x28) > 0)
            {
                long local_11 = NONE;
                function_204d30(&local_5, arg_0);
                s_actor_datum *local_12;
                while ((local_12 = squad_actors_next(&local_5)) != NULL)
                {
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

void function_201520(short arg_0, long arg_1, short arg_2, bool arg_3, bool arg_4);

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

PRIVATE __forceinline s_squad_datum *function_200d83(s_squad_record_iterator *arg_0)
{
    s_squad_datum *local_0 = NULL;
    if (g_4f55d0->active)
    {
        s_record_pool *local_1 = arg_0->records.data;
        long local_2 = data_find_index(local_1, arg_0->records.index + 1);
        if (local_2 != NONE)
        {
            local_0 = (s_squad_datum *)(local_1->data + local_1->size * local_2);
            arg_0->records.index = local_2;
            arg_0->records.datum_index = (*(short *)local_0 << 16) | local_2;
        }
        else
        {
            arg_0->records.index = local_1->maximum_count;
            arg_0->records.datum_index = NONE;
        }
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
    while (function_200d83(&local_0))
    {
        long local_1 = local_0.squad_index;
        dword local_2 = *(dword *)(*(byte **)((byte *)g_4e0350 + 0x164) + (local_1 & 0xffff) * 0x74 + 0x20);
        if ((bool)((local_2 >> 12) & 1))
            function_201520(NONE, local_1, NONE, 0, 0);
    }
}

void __stdcall function_203120(long arg_0, short arg_1, long arg_2);

// @retail 0x201100
void __stdcall function_201100(long arg_0, dword *arg_1)
{
    byte *local_2 = *(byte **)((byte *)g_4e0350 + 0x15c) + (arg_0 & 0xffff) * 0x24;
    s_record_pool *local_10 = g_51e9dc;
    byte *local_3 = local_10->data + (arg_0 & 0xffff) * 0x38;
    if (!local_3[0x23])
    {
        dword mask = 1 << (arg_0 & 0x1f);
        dword *bits = &arg_1[arg_0 >> 5];
        if (!(*bits & mask))
        {
            long local_6 = *(short *)(local_2 + 0x20);
            byte *local_7 = NULL;
            local_3[0x23] = true;
            if (local_6 != NONE)
            {
                local_7 = local_10->data + (local_6 & 0xffff) * 0x38;
                if (!local_7[0x23])
                    function_201100(local_6, arg_1);
            }
            short local_8 = *(short *)(local_2 + 0x22);
            if (local_8 >= 0 && local_8 < *(long *)((byte *)g_4e0350 + 0x240))
                function_203120(arg_0, local_8, true);
            else if (*(short *)(local_3 + 0x14) != NONE && local_7 && local_7[0x23])
            {
                short const *inherited = (short const *)(local_7 + 0x14);
                long local_9 = *inherited;
                if (*inherited != NONE)
                    function_203120(arg_0, local_9, false);
            }
            *bits |= mask;
        }
    }
}

// @retail 0x203120
void __stdcall function_203120(long arg_0, short arg_1, long arg_2)
{
    long const *local_0 = &arg_0;
    short const *local_1 = &arg_1;
    byte *local_3 = (byte *)squad_group_get(*local_0);
    /* retail stores the choice's ignore bit to the stack before testing it (into arg_0's
       dead slot), so it is kept in memory through a pointer here */
    byte ignored;
    byte *local_2 = &ignored;
    if (!(byte)arg_2 && local_3[0x16] && *(short *)(local_3 + 0x14) != NONE)
    {
        byte *local_4 = *(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_3 + 0x14) * 0x7c;
        if (local_4)
        {
            *local_2 = local_4[0x24] & 1;
            if (*local_2)
                return;
        }
    }
    function_202e30((short)*local_1, *local_0, (byte)arg_2);
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

// score 50 (was 50): frame 0x1c, the trigger flag, the task pointer test and the actor walk agree. Left: the
// squad index for the first 0x1fb8a0 call (retail `mov eax, ebp`, ours reloads its home, 2 bytes longer),
// which shifts the later jump targets and drops the actor loop's alignment nop.
// MiniMax D (11-11): no lower score (50): copying the squad index into a local and using the parameter directly at the call
// both score 50, so the reload stays; no other form of that call changed the layout.
// MiniMax S (11-10): no new form, no lower score (50); the only difference left is retail's EBP copy of the squad index where ours reloads its
// home, plus retail's one-byte alignment nop before the loop head at 0x202dd0.
// MiniMax R (11-09): five more forms around that call all give 50 with the same code: the call without the short cast
// on the choice, the test and the call without the casts, a local copy of the squad before the call, before the pair
// of calls, and the squad copied at the top of the function and used for every call.
// MiniMax R (11-08): the lock byte compared with zero instead of masked scores 12 and keeps the index in EBP, but that
// byte is a bitmask (the same field is tested with 0x10 and with bit one elsewhere), so the masked test stays (50); the
// first 0x1fb8a0 call still reloads the index from its home where retail keeps it in EBP.
// MiniMax H (11-07): no new form, no lower score (50); the first call still reloads the squad
// index from its stack home where retail keeps it in EBP (`mov eax, ebp`), which shifts the later
// jump targets. A copy of the parameter in a local (plain, unsigned, or scoped to the block) is
// copy-propagated back and compiles to the same reload (50).
// MiniMax C (11-05): no new form, no lower score: the first call still reloads the squad index from its stack
// home where retail keeps it in EBP.
// Earlier: task zeroing/retest, predicate load and iterator head remained.
// Earlier: frame 0x1c agrees; task zeroing/retest and iterator head remain; optional producer scope, delay scope, both scopes, native trigger entry, signed entry index, threshold selection, iterator name and native entry with producer scopes each 152.
// frame 0x1c agrees; task zeroing/retest, optional-output width and iterator head differ.
// Test-produced task 152; short output 152; wide task 154; both widths 154; direct inputs
// on test-produced task 152; short trigger count 152; wide threshold 152; pending store first 152.
// Complete native task record with signed/unsigned task and short/long optional output 152 each;
// those four forms with trigger-predicate parity 152 each (574 bytes).
// Round 146: inline selector/predicate/task-apply/event helpers 152 each; selector-
// predicate/selector-apply/predicate-apply/all-five/actor-options contracts 152 each.
// Round 139: Shared pending/trigger optional-output workspace long/int/short/word 152/152/152/153;
// those four with shared entry index 152/152/152/153. Frame 0x1c agrees; task zeroing/retest and
// iterator head remain. All original executable source restored.
// Round 180: Separate pending predicate/result with admitted/default-result arms: word task
// and long output 110/110; wide task 168/168; word task and short output 109/109; both widths
// 168/168. A read-once, two-use pending word and separate admitted result is 109 plain, kept.
// Round 198: On the retained pending-result graph, direct inputs/dword predicate/both
// 109/109/109; shared actor step alone/with direct inputs/predicate/both 132 each;
// signed pending/result pair 109. Original input views, predicate and actor walk restored.
// Round 104: the task pointer as the NONE-guarded ternary 61; the actor walk through the file's step, out of
// line 72, header step 72, header step with a forceinline declaration 72, the file's step defined forceinline
// 52 (kept); parameters read directly 52 (kept); the trigger flag as `(... & 1) != 0` 50 (kept); casts dropped
// on the delay value 50; delay value declared last 50; pending word at each use 50; structured exits for the
// three gotos 50 (kept); ticks output declared first 50; permuter 600 s, best only by a char delay value.
// Round 71: the one real difference is that load (EBP still holds the index; 0x202e90 saves EBP);
// the short-output casts dropped 50; the pending arms swapped 145; the result as a short 50; the
// trigger flag as a named bool for both tests 189; the block declarations reversed 50.
// Round 221: Complete logical index snapshot long/int/dword/const long 50 each;
// all four with a separate optional follow-up result join 50 each. Original body restored.
// Round 89: the reload follows the trigger-entry loop's 0x290e20 call (without that call or that loop the
// first part gets `mov eax, ebp`); `return` for the walk's break 50; a found flag with the walk after the
// loop 50; trigger count declared after the threshold 57; threshold as a ternary 50; follow-up event as
// if/else 50.
// Round 257: apply/event ordinary-inline-noinline boundary pairs on the recovered
// pointer/result and actor-step graph 50 each (eight forms); EBP index reload remains.
// Round 97: a named copy of the index for the trigger part 50 (identical); one read of the index through
// its address for the trigger part 50 (identical); short trigger count 50; long entry ordinal 93; entry
// word read at each use 50.
// Round 41: the trigger part reading the squad index once through the parameter's address 50
// (folded); a local copy of the index for the first part 50 (copy propagated); declaration
// orders of the ticks/index, task/pending, threshold/count and the actor iterator 50 each; the
// task pointer filled by an if 107 (the NULL test goes); permuter 600 s, no gain (49 = 49).
// Round 436: native trigger table, raw/native entries and mutable/read-only cached receivers
// 50/50/50/50; the same complete tables observed at every use 91/91/91/91.
// All table/entry declarations and executable forms restored; the index reload remains.
// Round 207: incoming-index, trigger-table/count and combined role names 50/50/50;
// two-use trigger predicate counted as bool/byte/long/word 131/131/135/135,
// Boolean count with the role names 131. Original trigger count and body restored.
// Round 51 (lane 9): task/follow-up locals at function scope with the follow-up block after the chain 59; the
// 0x202e90 call in both arms of the follow-up test 112 (610), with the NONE arm first 52 (head-merged, reload
// remains); the follow-up call as a separate `local_4 != NONE && local_2 != NONE` test 50, tests swapped 97;
// dword definition flags 50; iterator at function scope 50; trigger count/threshold swapped 50; follow-up
// event read at each use 50.
// Round 54 (lane 9): without the post-loop 0x1fb8a0 call the reload stays (diagnostic); the trigger part behind
// an early return 50; local_2 set to NONE only in the compute arm 72 (580); the trigger entry count read into a
// local before the loop 124 (593); one shared 0x202e90 call after the chain with the task, follow-up and flag set
// per arm 180 at 578 bytes (the follow-up call then gets retail's `mov eax, ebp`, but the first arm loses its own
// call site); the first test inverted with the first-arm call in the else 50.
// Round 56 (lane 16, fast): the trigger part's definition through the existing 0x203330 getter 50 (same code);
// local_2 at function scope 59 (578 bytes, reload stays); the follow-up test inverted 50; local_4 declared before
// pending 50; local_3 declared before local_2 50. Diagnostic only: a `return` when no task is found gives the
// follow-up call `mov eax, ebp` (167), so the reload follows how the paths join before the trigger part.
// Round 119 (lane 5): the order/pending part as a forceinline or plain inline helper taking the index and squad
// 50/50 (copy-propagated); the trigger part's squad re-fetched at the join through squad_get(arg_1)/(word)arg_1
// 199/212 (the reload stays); the follow-up arm without the short casts 50.
// Round 46 (lane 1): 0x290e20 already takes its NONE in EBX as in retail. Diagnostics: the trigger loop's
// 0x290e20 call, the 0x204d30 call or the last 0x1fb8a0 call given 0 for the index 87/49/54, the reload stays
// in each. The trigger pointer declared at function scope 50; the optional output, the counter output or the
// task declared at function scope 59/50/50.
// Round 272: Real record selector/pending-
// state/selector+pending/latch/selector+latch/pending+latch/all; all with immutable receiver
// 50/50/50/50/50/50/50/50.
// Original executable body and declarations restored; remaining allocation differences persist.
// Round 190 (fast): a two-use `bool has_definition = flag40 && definition != NONE` for both tests 169 (578 bytes:
// the follow-up 0x1fb8a0 call gets retail's `mov eax, ebp`, but the bool is materialised at entry and the compute
// arm re-reads the definition index); the same bool only in the else arm 50 (same code); the trigger part as a
// forceinline helper taking the squad and index 50 (same code); the trigger guard folded into the local_5 test
// 111; the definition index as a named local for the arm's test and the 0x290cd0 argument 50 (same code).
// The callee 0x290e20 (another lane's) still differs at +0x0.
// Round 73 (fast): `local_2` declared after local_4 50; the follow-up test and call without the short casts 50; a
// named copy of the index for the whole else-if arm 50; local_4 seeded from the pending word (cleared when +0x64 > 0)
// 160 (565 bytes); the follow-up written in both task arms (pending arm with its own 0x202e90 call) 74 at 578 bytes:
// the 0x1fb8a0 call then gets retail's `mov eax, ebp` (two separate blocks before allocation, their tails
// cross-jumped after it), but the pending arm loses retail's jump into the shared `cmp ax, 0xffff` follow-up block.
// Round 272: Real record selector/pending-
// state/selector+pending/latch/selector+latch/pending+latch/all; all with immutable receiver
// 50/50/50/50/50/50/50/50.
// Original executable body and declarations restored; remaining allocation differences persist.
// M298: Native admission bit-six bool/direct/nonzero and complete-word mask,
// then once-owned bool/long bit and word/long mask [186, 50, 50, 50, 189, 191, 187, 172].
// Original shared record and every admission observation restored.
// Round 56 (lane 11, fast; aligned 2: the reload, and the loop pad it shifts): the two 0x1fb8a0 follow-ups through one
// inline/forceinline `if (event != NONE)` helper 50/50; the first follow-up as an `&&` expression statement 50, as a
// ternary 50; a named has-task bool for the follow-up test 50. Diagnostics: the follow-up block given a second call
// 137 (594 bytes; loads the index into EBX, so the block is out of EBP's region, not only a missed copy); the index
// read through its address in the trigger loop 50; a volatile read there 50, at the walk 49 (580 bytes), both reverted.
// Round 479: Sentinel-first, combined-rejection, separate-rejection and valid-outer trigger graphs, each
// with nested action or insufficient-count continuation: 50/50/50/50/50/50/50/50. Original loop and
// call sites restored; the first follow-up still reloads the index instead of copying EBP.
// Round 119 (lane 7, fast; aligned 2: the reload and the pad it shifts): a named event set in each arm 50; event read right
// after the 0x290cd0 call 146 (580); the count test inverted with a task = NONE arm 156 (594); the five other orders of
// the pending stores 50/52/53/54/54; all 23 other declaration orders of the pending locals 50; the first test with a goto
// past the second arm, a nested flag test, an else block, shifted-bool flag reads, the argument test as its own if 50
// each; the trigger part behind an early return 50, its definition through 0x203330 50, by an if 107 (578, reload stays),
// nested guards 50; second-half index reads through the parameter's address everywhere, as a copy, in the loop or in the
// tail 50 each (copy-propagated). Diagnostic: no second-half index uses at all 87 (578): gives `mov eax, ebp`, so the
// reload comes from the index being live past the block.
// Round 140 (m3 lane 6, fast): a compiled-out `do {} while (false);` closing the else-if arm, inside the
// follow-up block, after the whole chain, after the first 0x202e90 call (braced) or before the follow-up
// test: 50 each (same code).
// Round 63 (m3 lane 1, fast): both halves as forceinline helpers taking the index and squad 50; only the order
// part 50; only the trigger part 50; first parameter short 56 (580); short task local with uncast follow-up 50.
// Also round 63: the squad as an s_squad_datum pointer with a byte view 50; the count test first in the
// compute arm 54.
// Round 78 (m3 lane 9, fast; aligned 2 = the reload and the pad it shifts): `arg_0 != NONE` tested
// after the record tests 148 (580 bytes); the trigger guard as two nested ifs 50; the pending word
// read at each use 50; `local_3 > 0` tested before the task 54; the pending arms swapped (compute arm
// first, `else if (+0x64 <= 0)`) 145 at 578 bytes, aligned 11 (the reload stays).
// Round 143 (m3 lane 6, fast): the compute arm's found/count test nested 50; local_4 left uninitialised and set to
// NONE inside the pending arm 50; the follow-up (0x202e90 + 0x1fb8a0) as a forceinline helper taking the index,
// task and event 50; the 0x290cd0 index argument as `(word)arg_1` 117 (582 B); the index parameter as dword 50.
// AC514: Actual mapped trigger-event long/int/dword/word results with conditional
// then default-first producers: 35/35/35/59/50/50/50/60. NOT kept: unmatched width/producer
// allocation forms; original event domain, wrapped scan, calls and body restored.
// First-event index reload and displaced actor-loop alignment remain.
// Wide conditional results 35 grow to 582/578 and add sign extension; the first index reload remains.
// Y590: Actual trigger-record null/count/count rejection exits, seven subsets
// then complete pending/latch rejection ownership: 50 each. NOT kept: the first
// event index reload and actor-loop alignment persist; original full body restored.
// P340: Actual trigger-query bool/byte/char/signed-char/short/word/long/dword results at the original
// call: 50/50/50/50/101/101/62/62. NOT kept: one-use result misses equality; original direct comparison,
// calls, wrapped scan and complete body restored. First-event index reload and loop alignment remain.
// P343: Accumulated trigger-count zero lifetime before squad/input/trigger/task/list/threshold/scan,
// then entry declaration with original seed: 176/176/105/61/50/50/57/50. NOT kept: no equality; original
// whole body restored; first-event index reload and shifted actor-loop alignment remain.
// N372: Delay-owned pending conditional/exhaustive results 135/50; computed candidate
// admission joins 156/50/50; complete graphs 156/50/50. NOT kept: no byte match; original
// word/result graph restored. The first-event index reload and actor-loop pad remain.
// Round 53 (m3 lane 2, fast; aligned 2 = the reload and the pad it shifts): `do { } while (false)` around the second
// half, the first chain, both, the 0x1fb8a0 call, the task block, the follow-up test, the trigger `for`, the first arm's
// call, nested or after it, all 50; a folded one-pass loop around the call 50; `for (;;)`, `switch (0)`, an implied
// guard around the first call 50 (all same code); `> 0` with an empty then-arm on the delay, the else-if, the task and
// the count tests 50/50/50/50; flag tests as `!= 0`/`== 0` 50/50; count before task 54; squad_get(arg_1) for any one
// first-half read 134/157/157/134/182/134/134/134/182 (B3 gets `mov eax, ebp` but the entry and the spill change); the
// first call written out per failing test (identical arms, diagnostic) 183-206 (not merged). Diagnostics that give
// `mov eax, ebp`: deleting any single first-half test (L6 the delay test 156, the event test 131, aligned 3 each), a
// real diamond before or after the first arm's call (aligned 4-5); those that do not: plain stores or reads in the first
// arm, the else-if head or after the chain, a diamond in the pending arm, deleting any second-half test. So it is a
// marginal weight decision in the first half, and block count alone does not move it.
// Round 151 (m3 lane 5, fast): a forceinline flag-and-definition predicate in both first-half tests, ternary or result
// form 50/50; the whole lock test as one forceinline predicate 50; the follow-up test with an empty then-arm 50; the
// follow-up passed the unnarrowed long 50. 0x290e20 (24) differs only in ordering, with the same registers.
// P387: Trigger ordinal with complete signed-sixteen-bit normalization, word/long/dword/int
// at for-owned then pre-threshold zero lifetimes: 50/72/55/72/58/75/63/75. NOT kept:
// no first-event index or actor-loop padding repair; wider graphs grow to 581/578 bytes.
// All original wrapped induction, queries, actor steps and the complete body restored.
// P398: Actual actor-step completion pointer/Boolean ownership, condition declaration,
// assigned while condition, for-owned result and explicit initial/next observation:
// 50/50/103/103/50/50/103/103. NOT kept: unmatched one-use/condition-assignment forms;
// explicit next-result lifetimes grow to 620 bytes. Original complete body restored;
// first-event index reload and shifted actor-loop alignment remain.
// T78: Complete pending/latch scan-join available/blocked outcomes,
// bool/byte/short/long: 50/50/50/50/50/50/50/50. NOT kept: no equality;
// all one-use outcomes and the complete body restored. First-event EBP reload
// and displaced actor-loop alignment remain.
// Round 162 (m3 lane 6, fast; aligned 2 = the reload and the pad it shifts): `(void)&arg_1;` at the top (the matched siblings'
// form), alone or with `(void)&arg_0;` 50/50; the follow-up as two statements with the task test repeated 50; the task test with an
// empty then-arm 50; `return` in place of the trigger loop's `break` 50; the trigger half behind an early return 50 (all the same
// code). Diagnostics on the later index uses: the 0x290e20 argument, the 0x204d30 argument, the last 0x1fb8a0 argument or the last
// two replaced by a constant 87/49/54/54, and the first follow-up still reloads the index in each: any one later use ends the
// index's EBP range before the first follow-up, so it is not one particular second-half read.
// Round 84 (m3 lane 10, fast), at 50: the first test's lock bit read dword-wide 50 (same code); the first test's entry
// through the existing getter 0x203330 (null test, then its bit) 162 (580 B); the lock bit as `!TEST_FIELD_BIT(... & 1)`
// 50; the bit declared as a one-bit field of the entry (overlaid on the byte view) and tested through the table's
// entries 50 (same code); the first condition in a named bool tested once 50.
// Round 179 (m3 lane 4, fast with 0x203bf0, 0x203d70, 0x201ea0, 0x202720 and 0x202a00): the follow-up event as a
// long passed to 0x290cd0 through a short view 51; a const index parameter 50 (same code); the pending word read as a
// signed short 68; the count test as `>= 1` 55 (580 bytes); the pending arm's two assignments swapped 50 (same code).
// The listing is retail's apart from the first follow-up's index load (ours `mov eax, [esp+0x30]`) and the loop pad.
// Round 68 (m3 lane 15, fast): the pending word read through squad_get(arg_1) 134 (581 B); the follow-up event as a
// named short for its test and the call 50; `short local_2;` set NONE just before the pending test 50; the trigger
// definition pointer by if/else with NULL in its own arm 50; the 0x290cd0 definition argument through squad_get(arg_1)
// 182 at 578 B (the index and the squad pointer swap EBP/ESI from entry). 0x290e20 differs in scheduling only.
// Round 168 (m3 lane 5, fast; aligned = difflib count, 2 at 50): the lock test in its De Morgan form `arg_0 != NONE &&
// !(flag && index != NONE && lock)` 50; the else-if written as `else { if (...) {...} }` 50; `short local_3 = 0;` 163
// (594 B); the first 0x202e90 call given arg_0 without the short cast 50 (same code each); the first test's index
// compare ahead of its flag bit 54 (an order probe). The first follow-up still reloads the index from its home.
// Round 70 (m3 lane 15, fast): the lock test as `!(flag40 && group != NONE && (definition[0x24] & 1))` 50 and the lock bit
// as `(definition[0x24] & 1) == 0` 50 (same code); permuter (anneal, 600 s, 35 tries): its best needs a volatile store,
// swapped arguments at the matched 0x1fb8a0 and a steering cast. The second half's count loop has matched 0x290d90's shape
// over a different group layout, and retail keeps it inline here (no post-loop count test). No untried natural form is left:
// rounds 119 and 162 show any one later index use ends EBP's range before the first follow-up, so only first-half shape
// moves it, and every first-half test spelling, order and nesting above compiles to this code or worse.
// Round 153 (m3 lane 8, fast; all callees now match): a named definition index for the first test, the else-if head and the
// 0x290cd0 argument 91 (580 B); the lock test through the file's typed definition-table view (as 0x203330 reads it) 50, with
// the second half's definition pointer through it too 50; every +0x2a read through the squad definition view 50; both
// 0x202e90 calls passed the unnarrowed values (implicit conversion) 50 (all same code but the first).
// Round 155 (m3 lane 8, fast): 0x202e90's third parameter as `bool` (its body reads it only as a byte; definition and the
// two outside declarations, calls unchanged) 50 (same code; 0x202e90, 0x203120, 0x2011f0, 0x275cb0 still match); re-measured
// round 73's follow-up in both pending arms (with its task test in each) 74 at 578 B. No call cycle runs through this function,
// 0x202e90 or 0x1fb8a0, so file order is no lever. No untried natural form is left.
// m3 lane 2 r75 (fast with 0x202e90): the pending arm reading +0x66 into local_2 before the delay test 72 (580 B);
// `short local_3 = NONE;` 169 (578 B); the delay test as `< 1` 51 (579 B); the delay in a named short 50 and `byte *const
// local_1` 50 (same code each). The first follow-up still reloads the index from its home.
// MiniMax A (11-12): no new form, no lower score (50); every instruction agrees with retail except the argument of the first 0x1fb8a0 call, where ours reloads the squad index from its stack home and retail keeps the EBP copy; a local copy of the parameter is propagated back, so the reload stays.
// M3 lane 11 r96 (fast with 0x202e90 and 0x203bf0; control 50): a `byte *flags` for both 0x40 tests 50; a `short *` to the
// +0x2a index for all six reads 50, for the four first-half reads 50, both pointers together 50/50 (all the same code),
// the index pointer for three reads only 162 (578 B). A second squad_get(arg_1) at one first-half read gives the follow-up
// retail's `mov eax, ebp` but the two-lookup entry (data loaded first, `lea`) with the squad spilled: first flag test 134
// (581 B), first index test 157 (576 B), lock index 157, else-if flag 134, pending word 134; the else-if index 182 (578 B,
// index and squad swap ESI/EBP). Diagnostics (not forms): one extra test in the second half ahead of the trigger
// definition, or in the first arm's chain, gives `mov eax, ebp` with every earlier byte as retail; an extra test between
// the task test and the second 0x202e90 call keeps the reload. So the reload is the last block of the index register
// range being dropped when the range is cut for the second half's EBP; one more index reference, or one block more or
// less off the follow-up path, keeps it. No code-neutral form found that adds either.
// MiniMax A (11-13): no new form tried this round, no lower score (50): the first 0x1fb8a0 call still reloads
// the squad index from the stack into EAX where retail keeps it in EBP, and the jumps after it land two bytes
// further in this body, so the block after the call is laid out differently and the body is 579 bytes against
// retail's 578.
// @retail 0x202be0
void function_202be0(long arg_0, long arg_1)
{
    byte *local_1 = (byte *)squad_get(arg_1);
    if (arg_0 != NONE && (!(local_1[2] & 0x40) ||
        *(short *)(local_1 + 0x2a) == NONE ||
        !((*(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_1 + 0x2a) * 0x7c)[0x24] & 1)))
        function_202e90(arg_1, (short)arg_0, false);
    else if ((local_1[2] & 0x40) && *(short *)(local_1 + 0x2a) != NONE)
    {
        short local_2 = NONE;
        short local_3;
        word pending = *(word *)(local_1 + 0x62);
        word local_4 = (word)NONE;
        if (pending != (word)NONE)
        {
            if (*(short *)(local_1 + 0x64) <= 0)
            {
                local_4 = pending;
                local_2 = *(short *)(local_1 + 0x66);
            }
        }
        else
        {
            local_4 = function_290cd0(*(short *)(local_1 + 0x2a), arg_1, NONE, &local_3, &local_2);
            if (local_4 != (word)NONE && local_3 > 0)
            {
                *(short *)(local_1 + 0x64) = local_3;
                *(word *)(local_1 + 0x62) = local_4;
                *(short *)(local_1 + 0x66) = (short)local_2;
                local_4 = (word)NONE;
            }
        }
        if (local_4 != (word)NONE)
        {
            function_202e90(arg_1, (short)local_4, true);
            if ((short)local_2 != NONE)
                function_1fb8a0(arg_1, (short)local_2);
        }
    }
    if (*(short *)(local_1 + 0x62) == NONE && !local_1[0x60])
    {
        byte *local_5 = *(short *)(local_1 + 0x2a) != NONE ?
            *(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(local_1 + 0x2a) * 0x7c : NULL;
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
                    function_290e20((short)local_11, arg_1, NONE) != ((*(dword *)local_10 & 1) != 0))
                {
                    local_7++;
                    if ((short)local_7 >= (short)local_8)
                    {
                        short local_12 = *(short *)(local_6 + 2);
                        short local_13 = local_12 == 0 ? (short)NONE : (short)(local_12 + 0x6e);
                        s_squad_actor_iterator local_14;
                        local_1[0x60] = true;
                        function_204d30(&local_14, arg_1);
                        while (squad_actor_next_inline(&local_14) != NULL)
                            function_1e4570(local_14.actor_index, (s_actor_options const *)local_5);
                        if (local_13 != NONE)
                            function_1fb8a0(arg_1, local_13);
                        break;
                    }
                }
            }
        }
    }
}

struct s_1fb7e0_data;
bool __stdcall function_20ba60(short arg_0, long arg_1, long arg_2, long arg_3, short arg_4, s_1fb7e0_data const *arg_5);

// score 107 (was 126): registers differ: retail keeps the record and its index in EBP/EDI and the
// inherited and new choices in EBX/ESI, reads the 0x1c choice zero-extended into EAX (xor eax,eax;
// mov ax), and keeps the next script index in BX on that path where ours stores it. Retail sets the
// choice passed down back to NONE on every path that finds none, which the `local_11` branch
// reproduces. Tried: the branch as a force-inlined helper (136), a choice variable with no gotos
// (116-153), the index as a word local (122).
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
            short local_5 = NONE;
            short local_6;
            short local_7 = *(short *)(local_4 + 0x1c);
            if (local_7 != NONE)
            {
                if (*(short *)(local_4 + 0x1e) > 0)
                    goto local_11;
                local_5 = *(short *)(local_4 + 0x20);
            }
            else
            {
                local_7 = function_290cd0(*(short *)(local_4 + 0x14), NONE, *local_0, &local_6, &local_5);
                if (local_7 == NONE)
                    goto local_11;
                if (local_6 > 0)
                {
                    *(short *)(local_4 + 0x1e) = local_6;
                    *(short *)(local_4 + 0x1c) = local_7;
                    *(short *)(local_4 + 0x20) = local_5;
                    goto local_11;
                }
            }
            if (local_7 != NONE)
            {
                local_2 = local_7;
                function_202e30(local_7, *local_0, true);
                if (local_5 != NONE)
                    function_20ba60(local_5, NONE, NONE, local_3 | 0x40000000, NONE, NULL);
            }
            else
            {
local_11:
                local_2 = NONE;
            }
        }
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

static __forceinline long real_to_long(real value);

// @retail 0x202720
void function_202720(void)
{
    s_group_datum_walk groups;
    s_squad_datum_walk squads;
    s_squad_group_datum *group;
    s_squad_datum *squad;
    group_datum_walk_new(&groups);
    while ((group = group_datum_walk_next(&groups, g_4f55d0->active)) != NULL)
    {
        if (*(short *)((byte *)group + 0x1e) > 0)
        {
            real local_2 = (real)g_510c54->field_2_3 * 2.0f;
            long local_3;
            local_3 = real_to_long(local_2);
            *(short *)((byte *)group + 0x1e) -= (short)local_3;
        }
    }
    if (g_4f55d0->active)
    {
        squads.iterator.data = g_51e9d8;
        squads.iterator.index = NONE;
    }
    while ((squad = squad_datum_walk_next(&squads, g_4f55d0->active)) != NULL)
    {
        if (*(short *)((byte *)squad + 0x64) > 0)
        {
            real local_2 = (real)g_510c54->field_2_3 * 2.0f;
            long local_3;
            local_3 = real_to_long(local_2);
            *(short *)((byte *)squad + 0x64) -= (short)local_3;
        }
    }
    group_datum_walk_new(&groups);
    while ((group = group_datum_walk_next(&groups, g_4f55d0->active)) != NULL)
    {
        if (*(long *)((byte *)group + 0x10) == NONE)
            function_202a00(groups.group_index, NONE);
    }
    if (g_4f55d0->active)
    {
        squads.iterator.data = g_51e9d8;
        squads.iterator.index = NONE;
    }
    while ((squad = squad_datum_walk_next(&squads, g_4f55d0->active)) != NULL)
    {
        if (*(long *)((byte *)squad + 0x6c) == NONE)
            function_202be0(NONE, squads.walk_index);
    }
}


// @retail 0x201c80
void function_201c80(void)
{
    long local_0 = g_510c54->game_time;
    long period_once;
    long period_twice;
    long period_half;
    real local_6 = (real)g_510c54->field_2_3;
    __asm
    {
        fld local_6
        fistp period_once
    }
    local_6 = (real)g_510c54->field_2_3 * 2.0f;
    __asm
    {
        fld local_6
        fistp period_twice
    }
    local_6 = (real)g_510c54->field_2_3 * 0.5f;
    __asm
    {
        fld local_6
        fistp period_half
    }
    if (local_0 % period_once == 0)
    {
        function_201df0();
        function_201ea0();
        function_202370();
    }
    if (local_0 % period_twice == 10)
    {
        function_2024c0();
        function_202720();
    }
    s_squad_datum_walk squad_walk;
    if (g_4f55d0->active)
    {
        squad_walk.iterator.data = g_51e9d8;
        squad_walk.iterator.index = NONE;
    }
    s_squad_datum *squad;
    while ((squad = squad_datum_walk_next(&squad_walk, g_4f55d0->active)) != NULL)
    {
        long squad_slot = squad_walk.walk_index;
        if (((squad_slot & 0xffff) + local_0) % period_half == 0 && (*((byte *)squad + 2) & 0x80))
            function_203360(squad_slot);
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
void function_201520(short arg_0, long arg_1, short arg_2, bool arg_3, bool arg_4)
{
    if (!g_4f55d0->active)
        return;
    byte *squad = (byte *)squad_get(arg_1);
    s_squad_placement_definition *row = &((s_squad_placement_table *)g_4e0350)->squads[arg_1 & 0xffff];
    long head = NONE;
    bool flag = false;
    if (arg_2 != NONE)
        arg_0 = 1;
    byte *options;
    if (*(short *)(squad + 0x2a) != NONE)
        options = *(byte **)(((byte *)g_4e0350) + 0x244) + *(short *)(squad + 0x2a) * 0x7c;
    else
        options = NULL;

    if (arg_0 == NONE)
    {
        long difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;
        switch ((short)difficulty)
        {
                case 0:
                case 1: arg_0 = *(short *)((byte *)row + 0x2c); break;
                case 2: arg_0 = (short)((*(short *)((byte *)row + 0x2c) + *(short *)((byte *)row + 0x2e)) / 2); break;
                case 3: arg_0 = *(short *)((byte *)row + 0x2e); break;
        }

    }
    dword available[1];
    dword blocked[1];
    memset(available, 0xff, ((((s_squad_placement_table *)g_4e0350)->squads[(word)arg_1].count + 31) >> 5) * sizeof(dword));
    blocked[0] = 0;
    if (arg_2 == NONE)
        function_201460(arg_1, blocked);
    else if (!arg_3 && !function_201400(arg_1, arg_2))
        arg_0 = 0;
    for (short attempt = 0; attempt < arg_0; attempt++)
    {
        short spawn;
        if (arg_2 == NONE)
            spawn = function_2039d0(arg_1, available, blocked);
        else
            spawn = arg_2;
        if (spawn >= 0 && spawn < row->count)
        {
            s_squad_placement_entry *placement_entry = &row->entries[spawn];
            long placement_object = NONE;
            short placement = placement_entry->placement_index;
            if (placement != NONE)
            {
                placement_object = function_201330(placement);
                if (!arg_3 && placement_object != NONE)
                {
                    s_unit_request request;
                    request.type = 0x32;
                    *(word *)request.arguments = (word)arg_1;
                    *(short *)(request.arguments + 2) = spawn;
                    if (function_e6900(placement_object, &request))
                        flag = true;
                }
            }
            if (!flag)
            {
                long location = *(short *)((byte *)placement_entry + 0x28);
                if (location == NONE)
                    location = *(short *)((byte *)row + 0x34);
                long actor = NONE;
                short seat = NONE;
                if (location != NONE && placement_entry->field2a != 7)
                {
                    actor = function_201010(head, (short)location, placement_entry->field2a, &seat);
                    if (actor == NONE)
                    {
                        byte creation[0xc4];
                        function_b7930(creation, *(long *)(*(byte **)(((byte *)g_4e0350) + 0x7c) + location * 0x28 + 4), NONE, NULL);
                        function_210850((s_type_c3b527 const *)((byte *)placement_entry + 4), (point3f *)(creation + 0x1c));
                        function_11dfb0((vector2f const *)((byte *)placement_entry + 0x14), (vector3f *)(creation + 0x28), (vector3f *)(creation + 0x34));
                        long field = *(long *)((byte *)placement_entry + 0x34);
                        if (!field)
                            field = *(long *)((byte *)row + 0x44);
                        if (field)
                            *(long *)(creation + 0xc) = field;
                        seat = NONE;

                        actor = function_b7b40(creation);
                        if (actor != NONE)
                        {
                            byte *actor_bytes = squad_transfer_object(actor);
                            *(long *)(actor_bytes + 0x3a0) = arg_1;
                            *(long *)(actor_bytes + 0x3a4) = head;
                            *(long *)(actor_bytes + 0x3a8) = *(long *)placement_entry;
                            head = actor;
                            if (!squad[0x76])
                            {
                                byte *object = g_4e3b44[*(long *)actor_bytes & 0xffff].bytes;
                                if (*(short *)(object + 0xc0))
                                    squad[0x76] = object[0xc0];
                            }
                            else
                                *(short *)(actor_bytes + 0x138) = (signed char)squad[0x76];
                            function_a7870(actor);
                            if (placement_entry->field2a != 6)
                            {
                                long replaced = NONE;
                                seat = function_200ee0(actor, placement_entry->field2a, &replaced, NULL);
                                if (seat != NONE)
                                    actor = replaced;
                            }
                        }
                    }
                    if (actor != NONE && (*(dword *)((byte *)row + 0x20) & 0x2000))
                    {
                        byte *actor_bytes = squad_transfer_object(actor);
                        dword *flags = (dword *)(actor_bytes + 0x134);
                        *flags |= 0x1000;
                    }
                }
                if (placement_entry->field2a != 6)
                {
                    long spawned = function_203780(placement_object, spawn, (short)arg_1, arg_4);
                    if (spawned != NONE)
                    {
                        ++*(short *)(squad + 0x24);
                        if (options)
                            function_1e4570(spawned, (s_actor_options const *)options);
                        if (actor != NONE && seat != NONE)
                        {
                            byte *actor_view = g_4f55f0->data + (spawned & 0xffff) * 0x888;
                            s_unit_request request;
                            request.type = 0x1c;
                            request.type1c.object_index = actor;
                            request.type1c.seat_index = seat;
                            request.type1c.unknowna = true;
                            request.type1c.unknownb = false;
                            if (function_e6900(*(long *)(actor_view + 0x18), &request))
                            {
                                byte *actor_bytes = squad_transfer_object(actor);
                                *(short *)(actor_bytes + 0x138) = *(short *)(actor_view + 0x24);
                            }
                        }
                        function_201b70(arg_1, spawn, spawned);
                    }
                }
            }
        }
    }
    if (head != NONE)
    {
        long next = head;
        do
        {
            long cursor = next;
            next = *(long *)(squad_transfer_object(cursor) + 0x3a4);
            byte *squad_row = (byte *)squad_get(arg_1);
            *(long *)(squad_transfer_object(cursor) + 0x3a4) = *(long *)(squad_row + 0x70);
            *(long *)(squad_row + 0x70) = cursor;
        }
        while (next != NONE);
    }
    if (!flag)
        function_2011f0(arg_1);
}
