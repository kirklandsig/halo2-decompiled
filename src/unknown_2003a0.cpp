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
	word flags;
	bool flag_a;
	bool flag_b;
	bool flag_c;
	byte unknown0d[3];
	s_squad_definition_entry *definition;
};

s_squad_definition_entry *function_203330(s_squad_definition_view const *squad);

// score 74 (was 92): the scan index, declared with the block's other locals, ends the zero
// register and gives retail's walk registers and stores, but it is live across the walk and takes
// a stack slot (push ecx frame, 347 bytes), so the flags go to EBX where retail keeps them in memory.
// Before: no frame; squad/definition EAX/ECX exchanged in the walk, and a zero kept
// in EBP (stored as next, scan index, count and palette tests) where retail uses immediates/TEST.
// Shared lookup 92; parent while 148; read-only squad 92; unused formal addresses removed 92;
// definition scope 92; cached entries 87 (one-use pointer excluded); direct mode flags 94; early
// index 92; native count/entries block 90; common scan exit 94; direct count 92; alternate reads 92;
// fallback-first flags 102; search condition 92; exit/direct count 94; exit/alternate reads 94;
// definition result 92. Round 87: squad re-fetched after the walk 118; definition declared first
// 92; entry pointer in the scan 92; a 72-form screen of count/entries selection (if/ternary/value),
// loop form (for/for-init/while), alternate local/field and >=/== exhaustion test: all 92.
// The definition lookup now calls the existing getter 0x203330 (byte-identical).
// Round 407: branch-owned next at entry/end 89/95; late current/handle/definition 93/123/89;
// all late 122; all late with branch-owned next 116, with late palette 116. All restored.
// DH round 257: Reused flag/mode carriers in parameter/word/long domains, eight non-original pairs
// 92/92/160/160/158/92/92/92. Every carrier and body restored; parent registers and persistent zero
// remain.
// Round 200: getter inline/cdecl/stdcall/inline-cdecl/inline-stdcall contracts 92 each;
// inline/cdecl/stdcall with branch-owned next 89 each. Original getter and iterator restored.
// Round 21 (lane 15): a found flag set at the type-2 break and tested after the scan gives
// retail's control flow at retail length (336) but 94, since retail threads both scan exits past
// the exhaustion test; with the 0x58 arm first 94 (aligned diff 48 against 52). The zero register
// needs both the next = 0 store and the index's zero start (start at one diagnostic 81). On the
// flag form: index before count/outer/found first/count in the condition/!= NULL/entry pointer
// 94 each; NONE arm first 117, early return 117; select forms (alternate local or field read,
// count slot if/ternary/value ternary, entries if/!if/ternary) 94 each, address ternary 91;
// inline entries helper 94; index from next 94, long index 95, guarded do-while 94, squad index
// stored before next 93; block view selected per use 93, in the condition 99, one block pointer
// 104 (318 bytes). Retail also places the NONE store between the two entries arms.
// Round 51 (lane 9): found flag 92 (330 bytes); getter as one ternary 92 (identical code); getter with a
// NULL-initialised result 101 (346), with the found flag 114; `index == count` exhaustion 92; `for (index = 0;`
// 92; count local dropped 92; scan as a while loop 92; dword definition flags 92; `short index = 0;` at the top
// of the definition block 97, at function scope 74 at 347 bytes (a `push ecx` frame; the walk then gets EAX
// definition, ECX squad and no zero register); uninitialised index at function/squad/definition scope 92 each;
// the unused-parameter address statements dropped 92 (identical, removed).
// Round 32 (one-file LTCG closure harness, exact; [harness positional, base 88]): index declared with
// squad/definition 74 [56] (kept), first in that list [56], after them [56]; with a found flag [78] (338
// bytes), long index [76] (342 bytes), count as a value [57], alternate read per use [56], entries declared
// outside the scan [56]. Without it: found flag [86] (330 bytes), with a selection pointer or a found
// pointer [86], long index [86] (322 bytes), do-while scan [86], squad index stored before next [85], index
// declared just after the walk [88], in the definition block [93] (each still with the zero register).
// @retail 0x204ec0
void function_204ec0(s_squad_iterator *iterator, short squad_index, short flags, short mode)
{
	if (squad_index != NONE)
	{
		s_squad_datum *squad;
		s_squad_definition_entry *definition;
		short index = 0;
		for (;;)
		{
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
				long count = *count_slot;
				for (; index < count; index++)
				{
					byte *entries;
					if (alternate) entries = *(byte **)(definition->data + 0x60);
					else entries = *(byte **)(definition->data + 0x58);
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
	else
		iterator->next = NONE;
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

// score 176 (was 176): Frame 8 and length agree; next-word cache and sentinel allocation remain.
// Earlier: Frame 8 and length agree; entry next-word cache and sentinel register remain.
// Earlier: frame 8 agrees; next stays in CX and the sentinel takes EBX.
// Full-width result 211; full-width entries 214; full-width palettes 186;
// result plus entries 214; selection role names 176. Natural while scans 174;
// common output exit 227; exhausted-entry return 228; frame pointer 221;
// global optimization off 533.
// Conditional next publication 218; definition mode first 211; current-field result 223;
// size preference 285; zero inline depth 232. Original kept.
// Earlier: round 312: next/sentinel allocation remains; rejection continuations 176; flat validation 176; count snapshot 191; four-short records 176; shared scalar scope 176.
// Earlier: round 106: Frame 8 agrees; next-word and sentinel allocation remain. Boolean predicate 176; Boolean arms 224; lazy entry address 176; direct definition observation 176; result-driven valid scans 203.
// AC338: accept-all snapshot 192; requested-bit snapshot 191; both flags 191; fallback mode first 187; read-only squad 176.
// round 312: next/sentinel allocation remains; rejection continuations 176; flat validation 176; count snapshot 191; four-short records 176; shared scalar scope 176.
// round 106: Frame 8 agrees; next-word and sentinel allocation remain. Boolean predicate 176; Boolean arms 224; lazy entry address 176; direct definition observation 176; result-driven valid scans 203.
// round 108: frame 8 agrees; next-word/sentinel allocation remains.
// Native entry table 176; native palette table 176; both table views 176;
// palette retained across advance 188; direct category-bit extraction 202.
// Round 110: native explicit selection pair 196; direct published success returns 224; pair with first while scan 211; published current return 176; unsigned-word result 198.
// Round 117: Native selection block 176; optional definition result 176; fallback postadvance 203; shared palette publication 210; readonly selection graph 176.
// Native complete squad owner/count/mode fields, their pairs, all three and the read-only complete record 176 each.
// Round 225: complete scenario/definition-owner/both/mode/scenario-mode/owner-mode/all/
// all with full-width result 188/198/192/194/207/175/205/192. Owner-mode gains only
// one at retail length, with a larger frame; below threshold. Original reads and scans restored.
// Round 30 (lane 10, one-file harness exact): retail compares the bottom `next` in memory and reloads
// it at the loop top, and keeps `result` in its stack slot (store, then reload at the exit); ours
// carries `next` in CX across the back edge and holds NONE in EBX from entry. Current cleared at entry
// 173; first scan as a while loop 202 (617 bytes); `goto done` on the exhausted iterator 185 (619);
// block selection as if/else 176; squad lookup without the word cast 176; third scan without the
// palette re-read 188.
// Round 21 (lane 15): retail sign-extends the iterator flags at +8 (movsx), so the field is a
// short: short flags 175 (aligned 123 against 124); with it, direct g_4e0350 reads in the second
// scan 175, the definition read once before the squad test 198, current cleared at entry 172,
// long result 211 (633 bytes). Retail keeps the definition in EBP through the second scan and
// re-reads g_4e0350 each pass; ours hoists g_4e0350 into EBX and re-reads the definition.
// Retail's NONE lives in ECX from entry into the third scan (re-set by `or ecx, -1` on its back
// edge), with the result always in its stack slot.
// Round 32 (one-file LTCG closure harness, exact): first scan with a named `next` (read once, stored
// back plus one) 176; `++iterator->next >=` 176; the palette/entry pair through a short pointer 196
// (617 bytes); next tested again in the count guard 176; flag comparison operands swapped 176, entry
// bit as bool (harness, 617 bytes, no gain); squad declared at the function top 176, the first scan's
// pair at the scan top and the palette index at the definition block's top (harness, identical).
// Ours carries next in CX from the entry test into the first pass; retail folds both tests into
// `cmp word [edx+4]` and reloads at the loop top.
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
			long *block = squad[0x60] ? (long *)(definition + 0x5c) : (long *)(definition + 0x54);
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

// score 18 (was 18): same instructions as retail, only registers differ after the first call: retail
// loads the squad pool into EAX, builds the index in EDX and forms the squad with lea eax,[edx+ecx] (so the
// test of the index can sit before it), then uses EDX/ECX for the object reload and EDX for the new index;
// ours builds the index in EAX (add eax,ecx) and swaps ECX/EDX after it.
// The first vehicle read once through a volatile pointer, before the object reload as retail orders
// it, gives 16; not kept, since it isn't a byte match.
// Tried: squad_get 18; array address 18; index times size plus data 18; index local 18; or operands swapped
// 18; early return 18; squad declared at the top 18; object reload into a local first 18; field through a
// long pointer 18; first vehicle saved before the store 19; inverted test 24; squad lookup at each use 33;
// squad computed before the NONE test 38 (at the top 42); ternary 45; one store after the test 47;
// index local updated in place 56/57.
// Round 42: named scaled offset signed/unsigned 18/18; const squad, then both pointers 18/18;
// identifier as a real short field 18; low-word helper 44; typed pointer increment/address-of-element 18/18;
// low-word equality 21; early high-bits predicate 18. The pool/index registers and three-address lea remain.
// Round 15: a link pointer to the first vehicle (0x201a50's form) 18; byte pointer squad 18; old squad
// index in a local 18; header pointer local 19; high-word test by shift 18. Compile-only screens
// reproduce ours exactly and ~20 squad/header/test forms (helper return, NULL-initialized squad, pool
// local) all keep add eax,ecx; retail's other two sites of this lea (0x1f3430, 0x20d570) are unmatched.
// Round 54 (lane 9): `(salt << 16) | (squad_index & 0xffff)` (0x1e0160's spelling) 37 (the index moves to EBP);
// the existing object_get for both object lookups 18; that with squad_get 18; the reload through
// object_header_get(...)->object 18; the insertion written in both arms of the high-word test 40 (143 bytes).
// @retail 0x201ad0
void function_201ad0(long squad_index, long object_index)
{
	s_squad_vehicle_view *object = ((s_squad_vehicle_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (object->squad_index != NONE)
		function_201a50(object->squad_index, object_index);
	if (squad_index != NONE)
	{
		s_squad_datum *squad = (s_squad_datum *)(g_51e9d8->data + (squad_index & 0xffff) * sizeof(s_squad_datum));
		((s_squad_vehicle_header *)g_4e0300->data)[object_index & 0xffff].object->next = squad->first_vehicle_index;
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

// @retail 0x204b20
// score 2 (was 2): the record pool's pointer is reloaded after the branch on the index,
// retail reloads it before, so the jump lands on the shared reload.
// Pool read before the active test 2; record address size * index + data 2; not-found
// stores swapped 2; datum index written index | salt 2; early return when inactive 20.
// Round 32: the file's data-iterator step (0x200db0's helper) 110 (368 bytes); `while (step(&iterator))` with
// the squad read from the iterator 2, the index read first 2; squad declared in the while
// condition 2; squad index read before the NULL test 2; next index read before the pool in the
// shared step 2.
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

#pragma inline_depth(0)
// score 64 (was 64): frame 0xc agrees; the squad address stays in EBX instead of
// retail's spilled ESI pointer and EBP handle. The common loop head and
// full-width return mask still differ.
// byte record 64; native absolute index 64 (caller loss); record field types 64.
// record flag bitfield 64; existing actor step 100 (caller loss); existing actor getter 84.
// frame pointer 74; size preference 82; native index stdcall 69.
// byte record stdcall 64; record fields stdcall 64; record bit stdcall 64.
// Iterator current handle 64; scalar next handle 64; zero iterator 76;
// invalid iterator 80; tick snapshot 62; initial flags 64; current handle
// with tick 62; scalar cursor with tick 62. Two-point gains rejected.
// Actual squad/actor/both record references 64/64/64;
// result from update 79; retained Boolean guard/result 70; proven success exit 79;
// update result with squad reference 79; Boolean result with actor reference 70.
// Earlier: the squad pointer stays in EBX where retail keeps the handle in EBP and spills
// the pointer to the handle's argument slot; loop rotation and return width as above. Inlined
// actor step under the depth-zero pragma (stays a call) 100; the step inlined with the pragma
// dropped and the two callees declared noinline 78 (184 bytes; unrotated loop and the or-byte
// flag store as in retail, structural 16 against 19); that with no parameter address 78, an actor
// index local 78, both 78, the squad getter 78, the iterator's handle stored 85, the squad
// re-fetched after the loop 76.
// Round 407: uniform live squad lookup with native flag test/store and initializer-owned
// handle combinations 84/89/78/83/88/93/82/87. Receiver refresh grows the body; all restored.
// Round 41: the `(void)&squad_index;` statement removed 64 (identical code, so dropped), the
// index read once through its address at the lookup or at the iterator call 64/64; the actor
// index kept in the iterator as the header step does 64; the header step itself 97 (the
// depth-zero pragma keeps it out of line). Retail gives the index EBP for the whole body and
// splits the squad pointer around the loop (ESI outside, reloaded from the index's home at the
// loop's back edge); ours keeps the pointer in EBX across the loop and reads the index from its
// home inside it.
// Round 86: existing squad_actor_next_inline step with 0x203d70's statement-level inline-depth pair
// around the 0x203f60 call (function-level pragma dropped) 78 at 184 bytes (retail's unrotated loop and
// `or byte` flag store), with the return as 0x203d70's `(((dword)*(short *)(squad + 2)) >> 7) & 1` 71
// (retail's full-width return exactly); on that body squad_get, parameter address taken, parameter read
// through its address, squad re-fetched after the loop, extra parameter uses 71-95: the parameter never
// gets a callee-saved register, so the pointer is never split.
// Round 30 (lane 10, one-file harness exact): a diagnostic extra read of the parameter at the top of
// the loop body (every pass) gives it a callee-saved register and splits the squad pointer around the
// loop as retail does; the same read beside the conditional store does not, so retail's parameter
// outweighs the pointer without any every-pass use. Word flag store as 0x203d70 62 (199 bytes); that
// with 0x203d70's short-view flag test 61; the existing step with the statement pragma 71 (191);
// existing squad_get with the word store 62; squad re-fetched after the loop 79 (212). A 32-form
// screen of loop form, flag store width, return spelling, pragma placement and lookup: 62 or more.
// Round 51 (lane 9): 0x203d70's loop (`while ((actor = squad_actor_next_inline(&iterator)) != NULL)`, the
// statement-level depth pair, word flag store, full-width return) 71 (191); with the function-level pair 109
// (152); byte squad pointer 71; actor index read from the iterator at each use 71; header squad_actors_next
// 71; `if (...) continue;` body 71, on the current body 64; iterator declared first 71; result local 74
// (188). Word flag store on the current body 62 (199/199), with the word flag test 62, with a word return 65.
// Round 54 (lane 9): the return spelled `(((dword)*(short *)(squad + 2)) >> 7) & 1` (0x203d70's flag read) gives
// retail's `and eax, 0x80; shr eax, 7` exactly (75 alone, 210 bytes); with the word flag store 65 (206; store and
// return as retail, the loop still rotated); with 0x203d70's loop and statement-level pair 71 (191: retail's
// unrotated loop, missing only the squad pointer's spill into the parameter's home and its back-edge reload);
// that with actor/actor_index declared first 71, the parameter copied into a local 71, the word flag test 71.
// File-local word/dword bitfield views of bit 7 72/69 (shift-then-mask). The same read matched 0x202e90.
// Round 119 (lane 5): word flag store with the full-width return 65 (206); that with 0x203d70's loop 98 (165);
// game time stored before the flag 64; squad_get with the word flag store 84 (179); word flag store and word
// flag test 62 (199).
// @retail 0x203bf0
bool function_203bf0(long squad_index)
{
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

// score 39 (was 39): 850/850 bytes. Left: the health sum loads the squad total into XMM1 ahead of the
// count add (retail adds the total into XMM0 in place), retail keeps the tracking node address before its
// +8 read and loads the prop pool after it (ours folds the read and loads the pool first), and the +0x26
// maximum uses EAX/ECX where retail uses EDX/EAX.
// Earlier: Frame 0x14 agrees; next-cursor memory home, contribution register and split entry flag clear remain.
// Earlier: frame 0x14 agrees; iterator index is cached and the flag clear splits.
// frame 0x14 agrees; iterator index is cached and the flag clear splits.
// Earlier iterator declaration 242; iterator/count role names 242; natural for
// walks 348; unsigned full-width count 242; delayed flag clear 242. Shared
// iterator helpers 242; marked iterator calls 350; branch-assigned count 179;
// frame pointer 218; global optimization off 472; branch-assigned word 183.
// Earlier: Frame 0x14 agrees; next-index memory home, contribution register and entry
// flag clear differ. Word hostility 233; separate position iterator 235; branch-assigned health
// 179; position component loop 221; scale component loop 218.
// Earlier: Frame 0x14 agrees; next-cursor storage, contribution register and flag
// clear remain. Long hostility 179; contribution from test 241; absent-tracking continue 179;
// early centroid pointer 179; unit count before health 179.
// Earlier: frame 0x14 agrees; cursor memory home, contribution and split flag clear remain.
// Shared unit handle 179; shared perception handle 234; native swarm member 179;
// health clamp assignments 179; shared centroid setter 179.
// AC338: signed-word contribution 183; entry count 179; entry health 179; absent-definition exit 179; disabled-definition exit 179.
// Frame 0x14 agrees; next-index memory home, contribution register and entry
// Frame 0x14 agrees; next-cursor storage, contribution register and flag
// Round 110: delayed long count from native population 235; known native squad fields 179; direct hostility OR 238; hostility first 179; native point memcpy 179.
// Signed-byte/byte hostility and unsigned branch count 179 each; non-unit default count 242; unsigned tracking handle and readonly tracking/target/both views 179 each. Original values and declaration types restored.
// Round 164: shared swarm input 179; shared pool getter 179; aggregation phase 235; phase swarm 235;
// phase pool getter 235; iteration actor scope 182; actor scope swarm 182; all native scopes 235.
// Zero/sentinel iterator seeds 243/228; actor pointer/health names 179/179; shared actor level 248; shared definition flags 179; live last-update reads 179; health publication before count 179. Cursor memory home and contribution allocation remain.
// Round 257: complete live squad observations by byte displacement 357 each
// with native count/header begin/both; native accessor 374/375/374/375. All restored.
// Round 225: consistent live actor receivers/shared vector sum/both/shared scale/live-scale/
// sum-scale/all/all with shared iterator 335/179/313/179/335/179/313/313.
// Original receivers, component operations, both tests and empty-path zero restored.
// Round 179: Actual first iterator initialization before disjoint header resets, with
// receiver/hostility lifetimes and chained zeroes, 228/228/227/227/234/234/237/237.
// Original begin order and complete body restored; cursor/contribution homes remain.
// Round 201: Independent marked aggregation/centroid begin boundaries 266/238/281; next boundaries
// 250/259/249; whole aggregation/centroid boundaries 284/249. Shared byte setter at opening/every
// bit store 218/179. Original complete body and include set restored; cursor/contribution homes
// remain.
// Round 21 (lane 15), aligned instruction count in brackets (baseline 140): retail never sets
// the count for an actor with neither unit nor perception (EBX keeps its last value, loaded from
// the stack parameter before the loop); without our `else count = 0` 242 [94] (869 bytes), which
// also gives retail's EBX load and the EDX zero for the clears; count at function scope 242;
// header begin/next/both 242 each; hostile declared first 245; word/short count 241/241. Left:
// our `&= ~0x10` splits around the zero, and g_4f55d0 stays in EDX into the first walk (retail's
// squad arm reuses EDX, so it reloads at the loop top).
// Round 51 (lane 9), aligned counts in brackets: no `else count = 0` (retail never sets the count for an
// actor with neither unit nor perception) 242; the +2 clear as a word 243; both 205 (850) [90]; with a
// separate position iterator 190 [62], word count 176, both 187 [61]; dword definition flags 187 [60]; the
// perception health computed from the short field first and the count read as a word after it 102 [24],
// with dword flags 101 [23]; the target address in the call argument 60 (848); target index inlined 105;
// `health + total` 101; with `else count = 0` back 230, byte clear 238, long count 167; hostile declared
// first 50 [26], after the iterator 60; vector3f_scale 60; count before health 60; node read inside the
// target index 56; all kept 46 [24]; existing prop_node_get 46; function-scope health 46; tracking index at
// each use 46; scale declared first 46; typed value10 46; parameter address statement dropped 46 (removed);
// typed count/total fields 46; `*(char *)` maximum 46; separate actor pointer for the position walk 83 (846);
// every +2 flag operation as a word 39 [21] (kept); the existing prop_node_get/prop_get getters for the
// tracking node and its team 39 (identical, kept); health sum first 39, after the count_c sum 37 [19].
// Round 54 (lane 9): `total = health + total` 39 (identical: VC turns every spelling into the same
// read-modify-write with the total loaded first); `real health` declared first at function scope 39; the health
// sum after the +0xc sum 37; `health += total; total = health;` 39 (still the load-first copy); health set only
// per path with `else health = 0.0f` 126 (851); the perception count read before the health division 178.
// In all of retail the unfolded `imul 0x3c; add; mov [+8]` node lookup appears only here and in 0x2761d0/0x25d810,
// where the node pointer has more than one use.
// Round 119 (lane 5): the tracking node as a named pointer (`s_prop_node_view *node = prop_node_get(...)`,
// the prop team through it) 84: it gives retail's unfolded node lookup, the pool load after it, the si-then-cx
// argument order and the DX/AL registers of the +0x26 maximum, so only the health sum's extra load is left and it
// shifts the rest by one instruction; with the health sum after the +0xc sum 82, where an aligned diff leaves only
// the health add (`movss xmm1,[total]; addss xmm1,xmm0` against retail's in-place `addss xmm0,[total]`) and the
// alignment pad. Health-add forms on the node pointer: total read at the loop top 126 (in place but held in a
// register); `= health + total` 84; health before the count 84; a total reference/pointer declared before health
// 131/131/131; inline health getter returning a variable 91 (forceinline 91, in the add 84) or a ternary 284;
// `health * 1.0f` 84; ternary health in the add 289; one-member struct health 90; out-pointer helper 82; squad
// written out by hand 82; hostile declared first 84; count declared before health 82. Without the node pointer:
// typed squad fields 39; `actor->swarm` 39; count set before health 39. A forceinline add helper taking health
// by value 82 (copy form kept). Matched in-place field adds add a parameter (0x1c9c80, 0x1e2570), a fresh load
// (0xdf5f0) or an inline clamp's result (0x187510), never a plain multi-path local.
// @retail 0x203360
void function_203360(long squad_index)
{
    byte *squad = (byte *)squad_get(squad_index);
    bool hostile = false;
    *(word *)(squad + 2) &= ~0x10;
    *(short *)(squad + 0x16) = 0;
    *(short *)(squad + 0xc) = 0;
    *(short *)(squad + 0xa) = 0;
    squad[0x26] = 0;
    *(real *)(squad + 0x10) = 0.0f;
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
        *(real *)(squad + 0x10) += health;
        *(short *)(squad + 0xc) += *((byte *)actor + 7) * count;
        /* retail calls the actor test here */
#pragma inline_depth(0)
        *(short *)(squad + 0x16) += function_1e1de0(iterator.actor_index) * count;
#pragma inline_depth()
        long tracking_index = *(long *)((byte *)actor + 0x338);
        if (tracking_index != NONE)
        {
            if (!function_1df5d0(*(short *)((byte *)actor + 0x24), prop_get(prop_node_get(tracking_index)->unknown08)->team))
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

// score 275 (was 275): Frame 0x48 versus 0x54, early saved-register pushes, iterator/point homes and intersection spill remain.
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
// @retail 0x201ea0
void function_201ea0(void)
{
    long elapsed = (short)squad_tick_round((real)g_510c54->field_2_3);
    dword const *clusters = (dword const *)((byte *)g_4e6948 + 0x1138);
    byte *structure = (byte *)g_4e0348;
    s_squad_record_iterator squads;
    if (g_4f55d0->active)
    {
        squads.records.data = g_51e9d8;
        squads.records.index = NONE;
    }
    while (g_4f55d0->active)
    {
        byte *squad = (byte *)function_200d83(&squads);
        short squad_index = (word)squads.squad_index;
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
        long local_2 = data_next_absolute_index_inlined(local_1, arg_0->records.index + 1);
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
// Round 119 (lane 5): the order/pending part as a forceinline or plain inline helper taking the index and squad
// 50/50 (copy-propagated); the trigger part's squad re-fetched at the join through squad_get(arg_1)/(word)arg_1
// 199/212 (the reload stays); the follow-up arm without the short casts 50.
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

// score 318 (was 318): 1332 bytes against 1328. Left: the squad index is kept as a zero-extended word in EBP
// and passed masked to the calls where retail keeps the third parameter in EBP, masks the index with AND,
// keeps the squad address and the 0x98 offset in their own slots and passes the index unmasked; the frame
// slots of the row, seat, chain and entry index differ.
// Earlier: Frame 0x134 agrees; placement/definition registers and final chain homes remain.
// Round 73: arg_1 passed unmasked to every call and the +0x3a0 store (masked only for the
// table lookups, as retail reads it) 374 (EBP takes the placement index as in retail, frame 0x130);
// with a squad pointer local for the body 353 (retail's entry layout; the 0x200ee0 output shares the
// seat result's slot), its output declared beside the seat result 353; with the placement index
// chosen by if/else 345; that if/else alone 358.
// Earlier: Frame 0x138 versus 0x134; placement/definition carriers and final chain homes remain.
// Earlier: Frame 0x138 versus 0x134; placement/definition carriers and final chain homes remain.
// Earlier: frame 0x13c versus 0x134; scalar and spawn/definition homes differ.
// Direct low-word index 387; other direct parameters 387; all direct reads 387;
// workspace role names 387; one shared native request 399. Pool-first lookup
// on low-word inputs 387; scalar masks 387; countdown while 388; zero inline
// depth 387; frame pointer 481.
// Native-word count-up 387; repeated scenario reads 404 (frame 0x138);
// repeated signed category 387; function-scope workspaces 387; case two first 387.
// round 312: frame 0x13c versus 0x134 remains; per-arm optional record 390; signed placement 387; full-width chosen entry 396; per-arm object result 388; native-short fallback selector 383 plain; direct parameter reads retained with identical conversions.
// round 106: Frame 0x13c versus 0x134 and spawn/definition homes remain. Direct list splice 391; placement rejection 383; native masked index 390; reuse continuation 383; direct difficulty 383.
// Short countdown 389; long reuse result 389; loop-scope seat output 383; loop-scope separate actor/object/seat results 383; traversal-scope selected entry 383.
// Late chain head 385; late accumulated reuse result 390; shared difficulty-zero/one arm 383; first request payload before type 383; chain scalar lexical home 383.
// Earlier: round 312: frame 0x13c versus 0x134 remains; per-arm optional record 390; signed placement 387; full-width chosen entry 396; per-arm object result 388; native-short fallback selector 383 plain; direct parameter reads retained with identical conversions.
// Earlier: round 106: Frame 0x13c versus 0x134 and spawn/definition homes remain. Direct list splice 391; placement rejection 383; native masked index 390; reuse continuation 383; direct difficulty 383.
// Earlier: round 108: frame 0x13c/0x134 and spawn/definition homes remain.
// Native placement workspace 383; completion-result guard 383; full-width category 387;
// unconditional initialized override 388; shared default word 387.
// Consistent squad-root displacement 358; with scenario/category/both observations 351/357/357;
// negative reuse state on those four graphs 358/351/357/357;
// scenario graph with early displacement/short placement/word squad/both inputs 351 each.
// Round 146: native group/entry/scalar masks/both records/group-masks/entry-masks/all/all-
// unsigned-displacement 351 each.
// Early result constants and input guard in seven combinations 361/351/361/359/360/360/360;
// both result lifetimes in positive-count branch 360.
// Isolated native final Boolean formal 359; short placement 351; byte control 351; placement/Boolean 359;
// byte/Boolean 359; placement/byte 351; all native 359; all native with signed byte control 359.
// Every prototype trial restored; native final Boolean reaches 1328 bytes but retains frame 0x138.
// Round 344: unsigned category once/at uses 351/357; with per-arm seat output 349/338;
// with per-arm object result 352/357; both outputs 350/338.
// Per-use native unsigned category with actual output-arm initialization kept at 338 plain.
// Native row word at +0x2a with unchanged 0x64 size/two-byte alignment 338 plain.
// Round 347: first cached native squad receiver 336/1328; with native squad word 391/1292,
// promoted difficulty 334/1328, both 389/1292. Request/mask/chain home names repeat
// 336/391/334/389 at the same lengths. Gains of two/four are below threshold; body restored.
// Round 198: Placement/prior-object lookup declarations, ordinary/inline/noinline
// in all eight non-original pairs, 338 each. Frame 0x134 agrees; original boundaries restored.
// Round 257: all group-table fields observed through a byte displacement
// 485; with one squad receiver 426, promoted difficulty 495, both 414;
// native group-array observations 538/466/537/452 in the same order. All restored.
// Round 225: optional-record arms 361; prior-object arms 374; both 382;
// selected-entry arms 358; optional/entry 357; prior/entry 378; all 379;
// all with first cached squad receiver 358 at retail length. Original lifetimes and values restored.
// Round 179: Natural short attempt counter on recovered category/seat-output frame 338;
// with promoted difficulty/signed placement/both 344/359/357; full-width fallback on
// those four graphs 354/359/354/359. Frame 0x134 and original body retained.
// Round 200: native creation/request/final-chain getter subsets 355/338/355/338/355/338/355;
// all with first cached squad receiver 403. Creation subsets reach 1328 bytes but worsen
// instruction allocation. Original lookup producers, narrowed values and complete body restored.
// Round 21 (lane 15), scores with the aligned instruction count (baseline 192) in brackets:
// renamed unavailable mask 338 (slot unchanged); squad = squad_get(arg_1) at entry with the
// final splice through squad_get 348 [192]; arg_1 unmasked at every call and the +0x3a0 store
// 374 [206]; both 356 [167] (frame 0x130); with the placement row from arg_1 & 0xffff 352
// [155]; with the 0x203330 getter for the definition 348 [156] (getter alone 374); 0x200ee0
// output declared at seat/entry scope 348/348; with seat = NONE once at its declaration 355
// [88] (frame 0x134 again; alone 357 [221]); long row category 351 [88]; short-compared
// prior-object word 352 [87]; long difficulty 349, switched as short 352; seat/placement/
// spawn/chain renamed in retail slot order or reversed 352/352 (slot order unchanged); spawn
// index set in each arm of `(short)arg_2 == NONE` 347 [78]; natural short attempt for loop
// 352 [85], with per-arm spawn index 353 [80] (gives retail's unthreaded arg_0 = 0); a squad
// local fetched in each chain pass 350 [75] / 356 [77] with the for loop. Lead: retail sizes
// the memset through a fresh placement lookup ((word)arg_1 * 0x74 + 0x48, movzx) while the
// row local serves every other use; that mixed read (not kept) gives 253 [61] at 1332 bytes.
// Left after it: placement/arg_2 EBX/EBP swap, seat/placement/spawn/chain slot order,
// promoted difficulty, setne on the long final flag (retail pushes a bool formal).
// Round 51 (lane 9), aligned counts in brackets: third parameter short (retail `cmp bp, -1`; 0xed710 pushes
// it from a 16-bit load) 338, kept (0xed710 then matches with a short surprise field); full retail-order
// rewrite (squad pointer, 0x203330 getter, unmasked index, long placement type, seat NONE once, bool flags)
// 357 (1328) [210]; bool fourth/fifth parameters alone 349; 0x203330 getter 374 (1348); seat NONE once 357
// [220]; long placement type 354 [147]; unmasked index 374 (1352); long type + unmasked 380, with a squad
// pointer 363; long type + seat 350 [141]; long type + bool parameters 318 [141] (kept); that with seat 358,
// getter 350, unmasked 378, squad pointer 356; long type + squad pointer 351; row/offset index as `& 0xffff`
// 377 (1352) [148].
// Round 54 (lane 9), aligned counts in brackets (baseline 141): a squad pointer from squad_get(arg_1) for
// +0x2a/+0x76/+0x24, the row from `arg_1 & 0xffff`, arg_1 unmasked to every call and the +0x3a0 store, the
// splice through squad_get and the memset sized by a fresh `(word)arg_1` row read 344 (1336) [132] (retail's
// and/imul entry); with local_9 by if/else and the seat NONE once at its declaration 288 [65]; one squad pointer
// per splice pass 283 (1332) [58]; the splice walking a fresh variable started from the chain head 276 (1328) [38];
// the spawn index set in each arm of `arg_2 == NONE` 281 [34]; the row declared first 295 [45]; the arg_2 test
// first 138 [57] (arg_1 cached in EBX); 0xc5340's `long difficulty; if/else; switch ((short)difficulty)` 280 [33]
// (retail's movsx pair); row/seat renamed 280 (identical); the placement index through
// `placement_entry->placement_index` 280 [31] (`cmp di, -1`); the row typed through s_squad_placement_table 280
// [31]; `for (short attempt = 0; attempt < arg_0; attempt++)` with the splice unwrapped 338 (1332) [64] (retail's
// unthreaded `xor esi, esi`, arg_2 moves to EDI). The same set without the fresh memset read 365 (1312) [43];
// the difficulty and placement forms alone 322 [139]. Left at 280 [31]: arg_2 EBX/row EBP (retail the reverse),
// the seat/row frame slots swapped, the threaded `arg_0 = 0`.
// Round 119 (lane 5): `long difficulty; if/else; switch ((short)difficulty)` 318 (identical); definition pointer
// by if/else 361; placement index through `placement_entry->placement_index` 322; all three 359; a separate
// `attempts` local instead of reassigning arg_0 384 (1348).
// @retail 0x201520
void function_201520(short arg_0, long arg_1, short arg_2, bool arg_3, bool arg_4)
{
    if (!g_4f55d0->active)
        return;
    byte *local_5 = *(byte **)(((byte *)g_4e0350) + 0x164) + ((word)arg_1) * 0x74;
    long squad_offset = ((word)arg_1) * 0x98;
    long local_7 = NONE;
    bool local_8 = false;
    if (arg_2 != NONE)
        arg_0 = 1;
    byte *local_9 = NULL;
    if (*(short *)((g_51e9d8->data + squad_offset) + 0x2a) != NONE)
        local_9 = *(byte **)(((byte *)g_4e0350) + 0x244) + *(short *)((g_51e9d8->data + squad_offset) + 0x2a) * 0x7c;
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
    if (arg_2 == NONE)
        function_201460(((word)arg_1), local_12);
    else if (!arg_3 && !function_201400(((word)arg_1), arg_2))
        arg_0 = 0;
    if (arg_0 > 0)
    {
        long local_13 = (word)arg_0;
        do
        {
            short local_14 = arg_2;
            if (local_14 == NONE)
                local_14 = function_2039d0((short)((word)arg_1), local_11, local_12);
            if (local_14 >= 0 && local_14 < *(long *)(local_5 + 0x48))
            {
                byte *local_15 = *(byte **)(local_5 + 0x4c) + local_14 * 0x64;
                s_squad_placement_entry const *placement_entry = (s_squad_placement_entry const *)local_15;
                long local_16 = NONE;
                word local_17 = *(word *)(local_15 + 0x3c);
                if (local_17 != (word)NONE)
                {
                    local_16 = function_201330((short)local_17);
                    if (!arg_3 && local_16 != NONE)
                    {
                        s_unit_request local_18;
                        local_18.type = 0x32;
                        *(word *)local_18.arguments = ((word)arg_1);
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
                    short local_21;
                    if (local_19 != NONE && placement_entry->field2a != 7)
                    {
                        local_21 = NONE;
                        local_20 = function_201010(local_7, (short)local_19, placement_entry->field2a, &local_21);
                        if (local_20 == NONE)
                        {
                            byte local_23[0xc4];
                            function_b7930(local_23, *(long *)(*(byte **)(((byte *)g_4e0350) + 0x7c) + local_19 * 0x28 + 4), NONE, NULL);
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
                                *(long *)(local_25 + 0x3a0) = ((word)arg_1);
                                *(long *)(local_25 + 0x3a4) = local_7;
                                *(long *)(local_25 + 0x3a8) = *(long *)local_15;
                                local_7 = local_20;
                                if (!(g_51e9d8->data + squad_offset)[0x76])
                                {
                                    byte *local_26 = g_4e3b44[*(long *)local_25 & 0xffff].bytes;
                                    if (*(short *)(local_26 + 0xc0))
                                        (g_51e9d8->data + squad_offset)[0x76] = local_26[0xc0];
                                }
                                else
                                    *(short *)(local_25 + 0x138) = (signed char)(g_51e9d8->data + squad_offset)[0x76];
                                function_a7870(local_20);
                                if (placement_entry->field2a != 6)
                                {
                                    long local_27 = NONE;
                                    local_21 = function_200ee0(local_20, placement_entry->field2a, &local_27, NULL);
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
                    else
                    {
                        local_21 = NONE;
                    }
                    if (placement_entry->field2a != 6)
                    {
                        long local_30 = function_203780(local_16, local_14, (short)((word)arg_1), arg_4);
                        if (local_30 != NONE)
                        {
                            ++*(short *)((g_51e9d8->data + squad_offset) + 0x24);
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
                            function_201b70(((word)arg_1), local_14, local_30);
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
            *(long *)(local_36 + 0x3a4) = *(long *)((g_51e9d8->data + squad_offset) + 0x70);
            *(long *)((g_51e9d8->data + squad_offset) + 0x70) = local_34;
        }
        if (local_8)
            return;
    }
    function_2011f0(((word)arg_1));
}
