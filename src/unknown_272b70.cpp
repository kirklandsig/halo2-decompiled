// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_272B70.CPP: the ai references of the script functions. An ai index
   names a squad, a squad group, an actor or a starting location by its type
   in the top two bits. */

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include "unknown_272b70.h"
#include "command_scripts.h"
#include "units.h"
#include "slot_handler.h"
#include "unknown_1dee50.h"
#include "unknown_2551c0.h"
#include "unknown_20fe20.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "object_markers.h"
#include "unknown_11a4d0.h"
#include <stdlib.h>
#include <string.h>

/* the actor whose command script is being run (unknown_29f5b0.cpp) */
extern long g_50240c;

enum
{
	_ai_index_type_squad = 0,
	_ai_index_type_squad_group,
	_ai_index_type_actor,
	_ai_index_type_starting_location
};

/* walks the squads an ai index names */
struct s_ai_squad_iterator
{
	s_squad_group_iterator group_iterator;
	long squad_group_index;
	long squad_index;
	long next_squad_index;
};

/* the scenario's squads (g_4e0350, globals.h), 0x74 bytes each, and their
   starting locations, 0x64 bytes each */
struct s_scenario_starting_location
{
	long name;
	byte unknown04[0x64 - 4];
};

struct s_scenario_squad
{
	byte unknown00[0x48];
	long starting_location_count;
	s_scenario_starting_location *starting_locations;
	byte unknown50[0x74 - 0x50];
};

struct s_scenario_squads_view
{
	byte unknown000[0x158];
	long squad_group_count;
	void *squad_groups;
	long squad_count;
	s_scenario_squad *squads;
};

/* the objects (a local view) */
struct s_ai_script_object
{
	byte unknown000[0xec];
	real body_vitality;
	byte unknownf0[0x10a - 0xf0];
	word flags10a;
};

inline s_ai_script_object *ai_script_object_get(long object_index)
{
	return (s_ai_script_object *)object_header_get(object_index)->object;
}

inline long ai_index_get_type(long ai_index)
{
	return (dword)ai_index >> 30;
}

/* the vehicles of a squad (a local view of the objects) */
struct s_ai_script_squad_vehicle
{
	byte unknown000[0x3a4];
	long next_squad_vehicle_index;
	long starting_location_name;
};

/* the actor an actor or starting location index names */
// @retail 0x272b70
long function_272b70(long ai_index)
{
	long actor_index = NONE;
	if (ai_index_get_type(ai_index) == _ai_index_type_actor)
	{
		actor_index = data_datum_index(g_4f55f0, ai_index & 0xffff);
	}
	else if (ai_index_get_type(ai_index) == _ai_index_type_starting_location)
	{
		short squad_index = (short)((ai_index >> 16) & 0x3fff);
		short starting_location_index = (short)ai_index;
		if (squad_index >= 0 && squad_index < ((s_scenario_squads_view *)g_4e0350)->squad_count)
		{
			s_scenario_squad *squad = &((s_scenario_squads_view *)g_4e0350)->squads[(word)squad_index];
			if (starting_location_index >= 0 && starting_location_index < squad->starting_location_count)
			{
				s_scenario_starting_location *starting_location = &squad->starting_locations[starting_location_index];
				s_squad_actor_iterator iterator;
				function_204d30(&iterator, squad_index);
				s_actor_datum *actor;
				while ((actor = function_204d70(&iterator)) != NULL)
				{
					if (actor->starting_location_name == starting_location->name)
					{
						actor_index = iterator.actor_index;
						break;
					}
				}
			}
		}
	}
	return actor_index;
}

/* the vehicle a starting location index names */
// @retail 0x272c90
long function_272c90(long ai_index)
{
	long result = NONE;
	if ((ai_index & 0xc0000000) == 0xc0000000)
	{
		short squad_index = (short)((ai_index >> 16) & 0x3fff);
		short starting_location_index = (short)ai_index;
		if (squad_index >= 0 && squad_index < ((s_scenario_squads_view *)g_4e0350)->squad_count)
		{
			s_scenario_squad *squad = &((s_scenario_squads_view *)g_4e0350)->squads[(word)squad_index];
			if (starting_location_index >= 0 && starting_location_index < squad->starting_location_count)
			{
				s_scenario_starting_location *starting_location = &squad->starting_locations[starting_location_index];
				long object_index = squad_get((word)squad_index)->first_vehicle_index;
				while (object_index != NONE)
				{
					s_ai_script_squad_vehicle *vehicle = (s_ai_script_squad_vehicle *)ai_script_object_get(object_index);
					if (vehicle->starting_location_name == starting_location->name)
					{
						result = object_index;
						break;
					}
					object_index = vehicle->next_squad_vehicle_index;
				}
			}
		}
	}
	return result;
}
// @retail 0x272d50
void ai_squad_iterator_new(s_ai_squad_iterator *iterator, long ai_index)
{
	if (ai_index_get_type(ai_index) == _ai_index_type_squad_group)
	{
		iterator->squad_group_index = ai_index & 0xffff;
		function_204db0(&iterator->group_iterator, ai_index & 0xffff);
	}
	else if (ai_index_get_type(ai_index) == _ai_index_type_squad)
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = ai_index & 0xffff;
	}
	else
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = NONE;
	}
}

/* retail inlines ai_squad_iterator_new (0x272d50) into some callers; this
   file is built /Ob1, so those callers use this copy */
inline void ai_squad_iterator_new_inline(s_ai_squad_iterator *iterator, long ai_index)
{
	if (ai_index_get_type(ai_index) == _ai_index_type_squad_group)
	{
		long squad_group_index = ai_index & 0xffff;
		iterator->squad_group_index = squad_group_index;
		function_204db0(&iterator->group_iterator, squad_group_index);
	}
	else if (ai_index_get_type(ai_index) == _ai_index_type_squad)
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = ai_index & 0xffff;
	}
	else
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = NONE;
	}
}
// @retail 0x272d90
void ai_actor_iterator_new(long ai_index, s_ai_actor_iterator *iterator)
{
	short type = (short)(ai_index_get_type(ai_index) & 3);
	iterator->single_actor = false;
	if (type == _ai_index_type_squad_group)
	{
		iterator->squad_group_index = ai_index & 0xffff;
		function_204db0(&iterator->group_iterator, ai_index & 0xffff);
		function_204e10(&iterator->group_iterator);
		iterator->squad_index = iterator->group_iterator.squad_index;
		function_204d30(&iterator->field_10, iterator->squad_index);
		iterator->actor_index = NONE;
	}
	else if (type == _ai_index_type_squad)
	{
		iterator->squad_group_index = NONE;
		iterator->squad_index = ai_index & 0xffff;
		iterator->actor_index = NONE;
		function_204d30(&iterator->field_10, ai_index & 0xffff);
	}
	else if (type == _ai_index_type_actor || type == _ai_index_type_starting_location)
	{
		iterator->squad_index = NONE;
		iterator->squad_group_index = NONE;
		iterator->actor_index = function_272b70(ai_index);
		iterator->single_actor = true;
	}
	else
	{
		iterator->squad_index = NONE;
		iterator->squad_group_index = NONE;
		iterator->actor_index = NONE;
	}
}

// @retail 0x272e20
s_actor_datum *ai_actor_iterator_next(s_ai_actor_iterator *iterator)
{
	s_actor_datum *actor = NULL;
	while (iterator->squad_index != NONE)
	{
		actor = function_204d70(&iterator->field_10);
		iterator->actor_index = iterator->field_10.actor_index;
		if (actor)
			return actor;
		iterator->actor_index = NONE;
		if (iterator->squad_group_index == NONE)
			return actor;
		s_squad_datum *squad = function_204e10(&iterator->group_iterator);
		iterator->squad_index = iterator->group_iterator.squad_index;
		if (!squad)
			return NULL;
		function_204d30(&iterator->field_10, iterator->squad_index);
		actor = NULL;
	}
	if (iterator->single_actor)
	{
		if (iterator->actor_index != NONE)
			actor = actor_datum_get(iterator->actor_index);
		iterator->single_actor = false;
		return actor;
	}
	return NULL;
}

long function_1ded60(void);
void function_1dedb0(long list_index, long object_index);

struct s_ai_script_object_list
{
	byte unknown00[6];
	short count;
	long first_reference_index;
};

/* The perception loop inlines the object-list insertion. */
inline void ai_script_object_list_add(long list_index, long object_index)
{
	s_ai_script_object_list *list = &((s_ai_script_object_list *)g_4f55d8->data)[list_index & 0xffff];
	long reference_index = record_pool_allocate(g_4f55d4);
	if (reference_index != NONE)
	{
		s_object_reference_1dee50 *reference = (s_object_reference_1dee50 *)(g_4f55d4->data + (reference_index & 0xffff) * g_4f55d4->size);
		reference->object_index = object_index;
		reference->next_reference_index = list->first_reference_index;
		list->first_reference_index = reference_index;
	}
	list->count++;
}

// @retail 0x272ea0
long __stdcall function_272ea0(long ai_index)
{
	/* The caller supplies the ai index on the stack. */
	long const *const local_87dd4a = &ai_index;
	long list_index = NONE;
	if (*local_87dd4a != NONE)
	{
		list_index = function_1ded60();
		if (list_index != NONE)
		{
			s_ai_actor_iterator iterator;
			ai_actor_iterator_new(*local_87dd4a, &iterator);
			s_actor_datum *actor;
			while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
			{
				if (actor->unit_index != NONE)
					function_1dedb0(list_index, actor->unit_index);
				if (actor->perception_index != NONE)
				{
					s_ai_object_iterator object_iterator;
					object_iterator.next_index = perception_get(actor->perception_index)->object_index;
					object_iterator.index = NONE;
					while (function_290c80(&object_iterator))
						ai_script_object_list_add(list_index, object_iterator.index);
				}
			}
		}
	}
	return list_index;
}

/* the objects (a local view) */
struct s_ai_script_unit
{
	byte unknown000[0xaa];
	byte object_type;
	byte unknown0ab[0x12c - 0xab];
	long actor_index;
};

/* the ai index of the actor of a unit */
// @retail 0x272ff0
long function_272ff0(long object_index)
{
	if (object_index != NONE)
	{
		s_ai_script_unit *unit = (s_ai_script_unit *)ai_script_object_get(object_index);
		if ((1 << unit->object_type) & 3)
		{
			long actor_index = unit->actor_index;
			if (actor_index != NONE)
				return (actor_index & 0xffff) | 0x80000000;
		}
	}
	return 0xc3e703e7;
}
inline s_squad_datum *ai_squad_iterator_next(s_ai_squad_iterator *iterator)
{
	s_squad_datum *squad = NULL;
	if (iterator->squad_group_index == NONE)
	{
		if (iterator->next_squad_index != NONE)
		{
			iterator->squad_index = iterator->next_squad_index;
			iterator->next_squad_index = NONE;
			squad = squad_get(iterator->squad_index);
		}
	}
	else
	{
		squad = function_204e10(&iterator->group_iterator);
		iterator->squad_index = iterator->group_iterator.squad_index;
	}
	return squad;
}

/* the clump objects (g_502420), 0x50 bytes each */
struct s_ai_script_clump_object
{
	byte unknown00[0x3c];
	bool flag3c;
	byte unknown3d[0x50 - 0x3d];
};

void __stdcall function_1e1a00(long index, long value);

/* resets (1e1a00) the actor of every object in an object list */
// @retail 0x273150
void function_273150(long list_index)
{
	long reference_index;
	long object_index = function_1dee50(list_index, &reference_index);

	while (object_index != NONE)
	{
		s_slot_object_view *object = object_get(object_index);

		if (object->actor_index != NONE)
			function_1e1a00(object->actor_index, 0);
		object_index = function_x457076(&reference_index);
	}
}

inline void ai_script_object_set_flag10a_14(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		word *flags = &ai_script_object_get(object_index)->flags10a;
		if (flag)
			*flags |= 0x4000;
		else
			*flags &= ~0x4000;
	}
}

/* sets a flag of the unit of every actor an ai index names, or of the objects
   of its perception when it has no unit */
// @retail 0x273200
void function_273200(long ai_index, bool flag)
{
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;

	ai_actor_iterator_new(ai_index, &iterator);
	while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
	{
		if (actor->unit_index != NONE)
		{
			ai_script_object_set_flag10a_14(actor->unit_index, flag);
		}
		else if (actor->perception_index != NONE)
		{
			s_ai_object_iterator object_iterator;

			object_iterator.next_index = perception_get(actor->perception_index)->object_index;
			object_iterator.index = NONE;
			while (function_290c80(&object_iterator))
				ai_script_object_set_flag10a_14(object_iterator.index, flag);
		}
	}
}

/* whether an object is dead: flagged so, or by its model, and without
   vitality (unknown_0dc310.cpp) */
bool function_dc310(long object_index);

/* whether every actor an ai index names (or every object of the perception of
   one without a unit) is dead */
// @retail 0x2732e0
bool function_2732e0(long ai_index)
{
	bool result = true;
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;

	ai_actor_iterator_new(ai_index, &iterator);
	actor = ai_actor_iterator_next(&iterator);
	while (actor)
	{
		if (actor->unit_index != NONE)
		{
			result = function_dc310(actor->unit_index);
		}
		else if (actor->perception_index != NONE)
		{
			s_ai_object_iterator object_iterator;

			object_iterator.next_index = perception_get(actor->perception_index)->object_index;
			object_iterator.index = NONE;
			while (function_290c80(&object_iterator))
			{
				result = function_dc310(object_iterator.index);
				if (!result)
					break;
			}
		}
		if (!result)
			break;
		actor = ai_actor_iterator_next(&iterator);
	}
	return result;
}

/* the flags of the units (a local view) */
struct s_ai_script_unit_flags
{
	byte unknown000[0x134];
	dword : 7;
	dword flag7 : 1;
	dword flag8 : 1;
	dword : 23;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* sets a flag of every unit of an object list */
// @retail 0x275160
void function_275160(long list_index, bool flag)
{
	long reference_index;
	long object_index = function_1dee50(list_index, &reference_index);
	while (object_index != NONE)
	{
		s_ai_script_unit_flags *unit = (s_ai_script_unit_flags *)function_badc0(object_index, 3);
		if (unit)
		{
			if (flag)
				unit->flag7 = true;
			else
				unit->flag7 = false;
		}
		object_index = function_x457076(&reference_index);
	}
}

/* sets another flag of every unit of an object list */
// @retail 0x275270
void function_275270(long list_index, bool flag)
{
	long reference_index;
	long object_index = function_1dee50(list_index, &reference_index);
	while (object_index != NONE)
	{
		s_ai_script_unit_flags *unit = (s_ai_script_unit_flags *)function_badc0(object_index, 3);
		if (unit)
		{
			if (flag)
				unit->flag8 = true;
			else
				unit->flag8 = false;
		}
		object_index = function_x457076(&reference_index);
	}
}

// @retail 0x2738a0
void function_2738a0(long ai_index, bool flag)
{
	if (ai_index != NONE)
	{
		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		s_actor_datum *actor = ai_actor_iterator_next(&iterator);
		while (actor)
		{
			actor->flag00c = flag;
			if (flag && actor->clump_object_index != NONE)
				((s_ai_script_clump_object *)g_502420->data)[actor->clump_object_index & 0xffff].flag3c = true;
			actor = ai_actor_iterator_next(&iterator);
		}
	}
}

inline void squad_set_flag1(long squad_index, bool flag)
{
	if (g_4f55d0->active)
		squad_get(squad_index)->flag1 = flag;
}

inline void squad_set_flag0(long squad_index, bool flag)
{
	if (g_4f55d0->active)
		squad_get(squad_index)->flag0 = flag;
}

// @retail 0x273900
void function_273900(long ai_index, bool flag)
{
	if (ai_index != NONE)
	{
		s_ai_squad_iterator iterator;
		ai_squad_iterator_new_inline(&iterator, ai_index);
		while (ai_squad_iterator_next(&iterator))
		{
			if (g_4f55d0->active)
			{
				s_squad_datum *local_0 = squad_get(iterator.squad_index);
				if (flag) local_0->flag1 = true;
				else local_0->flag1 = false;
			}
		}
	}
}

PRIVATE __forceinline void function_2739d7(s_squad_datum *arg_0, bool arg_1)
{
    byte *local_0 = (byte *)arg_0 + 2;
    if (arg_1)
        *local_0 |= 1;
    else
        *local_0 &= 0xfe;
}

// @retail 0x2739d0
void function_2739d0(long ai_index, bool flag)
{
	if (ai_index != NONE)
	{
		switch (ai_index_get_type(ai_index))
		{
		case _ai_index_type_squad:
		case _ai_index_type_squad_group:
		{
			s_ai_squad_iterator iterator;
			ai_squad_iterator_new(&iterator, ai_index);
			while (ai_squad_iterator_next(&iterator))
				{
                if (g_4f55d0->active)
                    function_2739d7(squad_get(iterator.squad_index), flag);
            }
			break;
		}
		case _ai_index_type_actor:
		case _ai_index_type_starting_location:
		{
			long actor_index = function_272b70(ai_index);
			if (actor_index != NONE)
				actor_datum_get(actor_index)->flag228 = flag;
			break;
		}
		}
	}
}

/* the same as unknown_29f5b0.cpp's game_seconds_to_ticks_round */
inline long ai_seconds_to_ticks_round(real seconds)
{
	real ticks_real = (real)g_510c54->field_2_3 * seconds;
	long ticks;
	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

/* the clumps (g_502420, 0x50 bytes), their props (g_50241c, 0xc4 bytes) and
   the props' members (g_502418, 0x3c bytes) */
struct s_ai_clump_273d30
{
	byte unknown00[0x14];
	long first_prop_index;
	byte unknown18[0x50 - 0x18];
};

struct s_ai_prop_273d30
{
	byte unknown00[8];
	long object_index;
	byte unknown0c[0x14 - 0xc];
	long next_prop_index;
	long first_member_index;
	long actor_index;
	byte unknown20[0xc4 - 0x20];
};

struct s_ai_prop_member_273d30
{
	byte unknown00[4];
	long actor_index;
	byte unknown08[0x1c - 8];
	short state;
	short timer;
	byte unknown20[0x34 - 0x20];
	long next_member_index;
	byte unknown38[0x3c - 0x38];
};

inline bool bit_vector_test(dword const *vector, long bit)
{
	return (vector[bit >> 5] & (1 << (bit & 31))) != 0;
}

inline void bit_vector_set(dword *vector, long bit, bool value)
{
	dword *word = &vector[bit >> 5];
	if (value)
		*word |= 1 << (bit & 31);
	else
		*word &= ~(1 << (bit & 31));
}

/* function_204d70 (squads.cpp), which retail inlines here */
static inline s_actor_datum *squad_actor_iterator_next_inlined(s_squad_actor_iterator *iterator)
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

/* sets the state of the props of an object the actors of a squad know (each
   clump once) */
PRIVATE __forceinline void function_273d37(dword *arg_0, long arg_1)
{
    dword *local_0 = &arg_0[(dword)arg_1 >> 5];
    dword local_1 = *local_0;
    local_1 |= 1u << (arg_1 & 31);
    *local_0 = local_1;
}

// @retail 0x273d30
void function_273d30(long ai_index, long object_index)
{
	if (!(ai_index & 0xc0000000))
	{
		long squad_index = ai_index & 0xffff;
		dword clump_bits[1];
		s_squad_actor_iterator iterator;
		s_actor_datum *actor;

		clump_bits[0] = 0;
		function_204d30(&iterator, squad_index);
		while ((actor = squad_actor_iterator_next_inlined(&iterator)) != NULL)
		{
			long clump_index = actor->clump_object_index;
			if (clump_index == NONE)
				continue;
			clump_index &= 0xffff;
			if (bit_vector_test(clump_bits, clump_index))
				continue;
			function_273d37(clump_bits, clump_index);

			long prop_index = ((s_ai_clump_273d30 *)g_502420->data)[clump_index].first_prop_index;
			while (prop_index != NONE)
			{
				s_ai_prop_273d30 *prop = &((s_ai_prop_273d30 *)g_50241c->data)[prop_index & 0xffff];
				if (prop->object_index == object_index)
				{
					long member_index = prop->first_member_index;
					while (member_index != NONE)
					{
						s_ai_prop_member_273d30 *member = &((s_ai_prop_member_273d30 *)g_502418->data)[member_index & 0xffff];
						if (actor_datum_get(member->actor_index)->squad_index == squad_index)
						{
							member->state = 3;
							member->timer = (short)ai_seconds_to_ticks_round(5.0f);
						}
						member_index = member->next_member_index;
					}
				}
				prop_index = prop->next_prop_index;
			}
		}
	}
}

// @retail 0x273ef0
void function_273ef0(long ai_index, bool flag)
{
	s_ai_actor_iterator iterator;
	ai_actor_iterator_new(ai_index, &iterator);
	s_actor_datum *actor = ai_actor_iterator_next(&iterator);
	while (actor)
	{
		actor->flag223 = flag;
		actor = ai_actor_iterator_next(&iterator);
	}
}

void function_1e2a00(long actor_index, bool flag, bool keep);
bool function_1e32e0(long actor_index, bool flag);
bool function_1e1de0(long actor_index);

/* an actor index singled out for the ai scripts (NONE when there is none) */
long g_46fc80 = NONE;

// @retail 0x273670
void function_273670(long ai_index, bool flag)
{
	if (ai_index != NONE)
	{
		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		while (ai_actor_iterator_next(&iterator))
			function_1e2a00(iterator.actor_index, flag, iterator.actor_index == g_46fc80);
	}
}

void function_201520(short value, word type, long a, long b, long c);
void function_204010(long squad_index, long other_squad_index);

/* for every squad an ai index names (or the squad of a starting location),
   calls 201520 and remembers the ai index in the ai globals */
PRIVATE __forceinline s_squad_datum *function_273481(s_ai_squad_iterator *arg_0)
{
	s_squad_datum *local_0 = NULL;
	if (arg_0->squad_group_index == NONE)
	{
		long const volatile *local_1 = &arg_0->next_squad_index;
		long local_2 = *local_1;
		if (local_2 != NONE)
		{
			arg_0->squad_index = local_2;
			arg_0->next_squad_index = NONE;
			local_0 = squad_get(arg_0->squad_index);
		}
	}
	else
	{
		local_0 = function_204e10(&arg_0->group_iterator);
		arg_0->squad_index = arg_0->group_iterator.squad_index;
	}
	return local_0;
}

// @retail 0x273480
void function_273480(long ai_index)
{
	if (ai_index != NONE)
	{
		long type = ai_index_get_type(ai_index);
		if (type == _ai_index_type_squad || type == _ai_index_type_squad_group)
		{
			s_ai_squad_iterator iterator;
			ai_squad_iterator_new_inline(&iterator, ai_index);
			while (function_273481(&iterator))
				function_201520(NONE, (word)iterator.squad_index, NONE, 0, 1);
			g_4f55d0->unknown364 = ai_index;
		}
		else if (type == _ai_index_type_starting_location)
		{
			short squad_index = (short)((ai_index >> 16) & 0x3fff);
			short starting_location_index = (short)ai_index;
			if (squad_index >= 0 && squad_index < ((s_scenario_squads_view *)g_4e0350)->squad_count &&
				starting_location_index >= 0 &&
				starting_location_index < ((s_scenario_squads_view *)g_4e0350)->squads[(word)squad_index].starting_location_count)
			{
				function_201520(1, squad_index, ai_index, 0, 1);
				g_4f55d0->unknown364 = (word)squad_index;
			}
		}
	}
}

/* the actors (a local view) */
struct s_actor_274140
{
	byte unknown000[0x20];
	long next_actor_index;
	byte unknown024[0x34 - 0x24];
	long squad_index;
};

void function_1e3400(long actor_index, long squad_index);
bool function_2052d0(long squad_index, long squad_group_index);

// @retail 0x272c40
bool function_272c40(long ai_index, long squad_index)
{
	bool result = false;
	switch (ai_index_get_type(ai_index))
	{
	case _ai_index_type_squad:
		result = squad_index == (ai_index & 0xffff);
		break;
	case _ai_index_type_squad_group:
		result = function_2052d0(squad_index, ai_index & 0xffff);
		break;
	case _ai_index_type_actor:
		result = false;
		break;
	case _ai_index_type_starting_location:
		result = false;
		break;
	}
	return result;
}
void function_201ad0(long squad_index, long vehicle_index);
void function_2011f0(long squad_index);
void function_201df0(void);

/* moves the actors and vehicles an ai index names into a squad */
// @retail 0x274140
void function_274140(long ai_index, long squad_index)
{
	long const *squad_reference = &squad_index;
	short actor_count = 0;
	s_ai_actor_iterator field_10;
	ai_actor_iterator_new(ai_index, &field_10);
	while (ai_actor_iterator_next(&field_10))
	{
		function_1e3400(field_10.actor_index, *squad_reference);
		actor_count++;
	}

	long actor_index = g_4f55d0->unknown14;
	while (actor_index != NONE)
	{
		s_actor_274140 *actor = (s_actor_274140 *)actor_datum_get(actor_index);
		if (actor->squad_index != NONE)
		{
			bool in_ai = false;
			switch (ai_index_get_type(ai_index))
			{
			case _ai_index_type_squad:
				in_ai = actor->squad_index == (ai_index & 0xffff);
				break;
			case _ai_index_type_squad_group:
				in_ai = function_2052d0(actor->squad_index, ai_index & 0xffff);
				break;
			case _ai_index_type_actor:
			case _ai_index_type_starting_location:
				in_ai = false;
				break;
			}
			if (in_ai)
				actor->squad_index = squad_index;
		}
		actor_index = actor->next_actor_index;
	}

	s_ai_squad_iterator local_278a95;
	s_squad_datum *squad;
	ai_squad_iterator_new_inline(&local_278a95, ai_index);
	while ((squad = ai_squad_iterator_next(&local_278a95)) != NULL)
	{
		long vehicle_index = squad->first_vehicle_index;
		while (vehicle_index != NONE)
		{
			long next_vehicle_index = ((s_ai_script_squad_vehicle *)ai_script_object_get(vehicle_index))->next_squad_vehicle_index;
			function_201ad0(squad_index, vehicle_index);
			vehicle_index = next_vehicle_index;
		}
	}

	if (actor_count > 0 && squad_index != NONE)
		function_2011f0(squad_index);
	function_201df0();
}

/* for every squad an ai index names, calls 201520 and 204010 with a squad */
// @retail 0x2735c0
void function_2735c0(long ai_index, long squad_ai_index)
{
	if (ai_index != NONE && squad_ai_index != NONE && !(squad_ai_index & 0xc0000000))
	{
		long squad_index = squad_ai_index & 0xffff;
		s_ai_squad_iterator iterator;
		ai_squad_iterator_new(&iterator, ai_index);
		while (ai_squad_iterator_next(&iterator))
		{
			function_201520(NONE, (word)iterator.squad_index, NONE, 0, 1);
			function_204010(squad_index, iterator.squad_index);
		}
		g_4f55d0->unknown364 = ai_index;
	}
}

// @retail 0x273eb0
void function_273eb0(long ai_index, bool flag)
{
	s_ai_actor_iterator iterator;
	ai_actor_iterator_new(ai_index, &iterator);
	while (ai_actor_iterator_next(&iterator))
		function_1e32e0(iterator.actor_index, flag);
}

// @retail 0x274090
short function_274090(long ai_index)
{
	short result = 0;

	switch (ai_index_get_type(ai_index))
	{
	case _ai_index_type_actor:
	case _ai_index_type_starting_location:
	{
		long actor_index = function_272b70(ai_index);
		if (actor_index != NONE && function_1e1de0(actor_index))
			result = 1;
		break;
	}
	case _ai_index_type_squad:
	{
		long squad_index = ai_index & 0xffff;
		if (squad_index >= 0 && squad_index < ((s_scenario_squads_view *)g_4e0350)->squad_count)
			result = squad_get(squad_index)->value16;
		break;
	}
	case _ai_index_type_squad_group:
	{
		long squad_group_index = ai_index & 0xffff;
		if (squad_group_index >= 0 && squad_group_index < ((s_scenario_squads_view *)g_4e0350)->squad_group_count)
			result = squad_group_get(squad_group_index)->value2c;
		break;
	}
	}
	return result;
}

/* the actors (a local view) */
struct s_actor_274e70
{
	byte unknown000[7];
	bool flag007;
	bool flag008;
	byte unknown009[0x18 - 9];
	long unit_index;
	long field_1c;
	byte unknown020[0x84 - 0x20];
	short value084;
};

void function_290040(long field_1c);
void function_1e31b0(long unit_index);

inline void actor_274e70_erase(long actor_index)
{
	s_actor_274e70 *actor = (s_actor_274e70 *)actor_datum_get(actor_index);
	if (actor->flag007 && actor->field_1c != NONE)
		function_290040(actor->field_1c);
	else
		function_1e31b0(actor->unit_index);
	actor->flag008 = true;
}

// @retail 0x274e70
void function_274e70(long ai_index, bool flag)
{
	if (ai_index != NONE)
	{
		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		while (ai_actor_iterator_next(&iterator))
		{
			volatile long actor_index = iterator.actor_index;
			s_actor_274e70 *actor = (s_actor_274e70 *)actor_datum_get(actor_index);
			if (flag)
			{
				actor->value084 = 0;
				actor_274e70_erase(actor_index);
			}
			else if (actor->value084 == 0)
			{
				actor->value084 = 3;
			}
		}
	}
}

inline void actor_274f30_update(long actor_index, bool flag)
{
	s_actor_274e70 *actor = (s_actor_274e70 *)actor_datum_get(actor_index);
	if (flag)
	{
		actor->value084 = 0;
		actor_274e70_erase(actor_index);
	}
	else if (actor->value084 == 0)
	{
		actor->value084 = 3;
	}
}

/* the same for the actors of the units of an object list and of the units
   attached to them */
// @retail 0x274f30
void function_274f30(long list_index, bool flag)
{
	long reference_index;
	long object_index = object_list_get_first_inlined(list_index, &reference_index);
	while (object_index != NONE)
	{
		s_slot_object_view *unit = (s_slot_object_view *)function_badc0(object_index, 3);
		if (unit)
		{
			if (unit->actor_index != NONE)
				actor_274f30_update(unit->actor_index, flag);
			long child_index = unit->first_child_index;
			while (child_index != NONE)
			{
				s_slot_object_view *child = object_get(child_index);
				if (((1 << child->type) & 3) && child->actor_index != NONE)
					actor_274f30_update(child->actor_index, flag);
				child_index = child->next_object_index;
			}
		}
		object_index = function_x457076(&reference_index);
	}
}

void __stdcall function_202e90(long squad_index, long index, long flag);
void __stdcall function_203120(long squad_group_index, long index, long flag);

struct s_scenario_275cb0_view
{
	byte unknown000[0x240];
	long count;
};

// @retail 0x275cb0
void function_275cb0(long ai_index, short index)
{
	long squad_index = NONE;
	long squad_group_index = NONE;

	if (!(ai_index & 0xc0000000))
		squad_index = ai_index & 0xffff;
	else
		squad_group_index = ai_index & 0xffff;

	if (index >= 0 && index < ((s_scenario_275cb0_view *)g_4e0350)->count)
	{
		if (squad_index != NONE)
			function_202e90(squad_index, index, true);
		else if (squad_group_index != NONE)
			function_203120(squad_group_index, index, true);
	}
}

/* the scenario's squads and the entries their +0x36 index names (8 bytes
   each at +0x17c), a local view */
struct s_scenario_squad_273040
{
	byte unknown00[0x20];
	dword flags;
	byte unknown24[0x36 - 0x24];
	short entry_index;
	byte unknown38[0x74 - 0x38];
};

struct s_scenario_entry_273040
{
	long unknown0;
	long index;
};

struct s_scenario_273040_view
{
	byte unknown000[0x160];
	long squad_count;
	s_scenario_squad_273040 *squads;
	byte unknown168[0x17c - 0x168];
	s_scenario_entry_273040 *entries;
};

long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);
void function_201df0(void);

/* puts a unit into a squad */
// @retail 0x273040
void function_273040(long unit_index, long squad_index)
{
	s_scenario_273040_view *scenario = (s_scenario_273040_view *)g_4e0350;

	if (g_4f55d0->active && unit_index != NONE && squad_index != NONE)
	{
		long index = squad_index & 0xffff;
		if (index >= 0 && index < scenario->squad_count)
		{
			s_scenario_squad_273040 *squad = &scenario->squads[index & 0xffff];
			if (squad->entry_index != NONE)
			{
				long entry_index = scenario->entries[squad->entry_index].index;
				if (entry_index != NONE)
				{
					function_1e0160(index, entry_index, unit_index, (bool)((squad->flags >> 10) & 1));
					function_201df0();
				}
			}
		}
	}
}

/* puts the units of an object list into a squad */
// @retail 0x2730c0
void function_2730c0(long list_index, long squad_index)
{
	long reference_index;
	long object_index = function_1dee50(list_index, &reference_index);
	while (object_index != NONE)
	{
		function_273040(object_index, squad_index);
		object_index = function_x457076(&reference_index);
	}
}

/* function_204d30 (squads.cpp), which retail inlines here */
static inline void squad_actor_iterator_new_inlined(s_squad_actor_iterator *iterator, long squad_index)
{
	if (g_4f55d0->active)
	{
		iterator->squad_index = squad_index;
		iterator->actor_index = NONE;
		if (squad_index == NONE)
			iterator->next_actor_index = g_4f55d0->unknown14;
		else
			iterator->next_actor_index = squad_get(squad_index)->first_actor_index;
	}
}

/* the team fields of the squads, actors and objects (local views) */
struct s_squad_275b60
{
	byte unknown00[0x76];
	byte team;
};

struct s_actor_275b60
{
	byte unknown000[7];
	bool flag007;
	byte unknown008[0x18 - 8];
	long unit_index;
	long field_1c;
	long next_actor_index;
	short team;
};

struct s_object_275b60
{
	byte unknown000[0x138];
	short team;
};

void function_290bf0(long field_1c, short team);
void function_1c9a00(void);

/* sets the team of the squads an ai index names and of their actors */
// @retail 0x275b60
void function_275b60(long ai_index, short team)
{
	if (((dword)ai_index & 0xc0000000) <= 0x40000000 && ai_index != NONE && team != NONE)
	{
		s_ai_squad_iterator iterator;
		s_squad_datum *squad;

		ai_squad_iterator_new(&iterator, ai_index);
		while ((squad = ai_squad_iterator_next(&iterator)) != NULL)
		{
			s_squad_actor_iterator field_10;
			s_actor_datum *actor;

			((s_squad_275b60 *)squad)->team = (byte)team;
			squad_actor_iterator_new_inlined(&field_10, iterator.squad_index);
			while ((actor = squad_actor_iterator_next_inlined(&field_10)) != NULL)
			{
				s_actor_275b60 *actor_view = (s_actor_275b60 *)actor_datum_get(field_10.actor_index);
				actor_view->team = team;
				if (actor_view->flag007)
				{
					function_290bf0(actor_view->field_1c, team);
				}
				else if (actor_view->unit_index != NONE)
				{
					((s_object_275b60 *)object_header_get(actor_view->unit_index)->object)->team = team;
				}
			}
		}
		function_1c9a00();
	}
}

short ai_trigger_find_by_name(char const *name);
bool function_290e20(short trigger_index, long squad_index, long squad_group_index);

/* whether a named trigger holds for the squad or squad group an ai index names */
// @retail 0x275d10
bool function_275d10(char const *name, long ai_index)
{
	volatile bool result = false;
	long trigger_index = ai_trigger_find_by_name(name);

	if (trigger_index != NONE && ai_index != NONE)
	{
		if (ai_index_get_type(ai_index) == _ai_index_type_squad)
			return function_290e20((short)trigger_index, ai_index & 0xffff, NONE);
		else if (ai_index_get_type(ai_index) == _ai_index_type_squad_group)
			return function_290e20((short)trigger_index, NONE, ai_index & 0xffff);
	}
	return result;
}

/* counts the actors an ai index names (mode 0 and 1 pick a count of each
   squad, 2 their difference) and averages their vitality */
// @retail 0x273f30
long function_273f30(long ai_index, short mode, long *actor_count, real *average_vitality)
{
	long result = 0;
	long count = 0;
	real vitality = 0.0f;
	if ((short)(ai_index_get_type(ai_index) & 3) == _ai_index_type_actor || (short)(ai_index_get_type(ai_index) & 3) == _ai_index_type_starting_location)
	{
		long actor_index = function_272b70(ai_index);
		if (actor_index != NONE)
		{
			s_actor_datum *actor = actor_datum_get(actor_index);
			vitality = ai_script_object_get(actor->unit_index)->body_vitality;
			result = 1;
			count = 1;
		}
	}
	else
	{
		s_ai_squad_iterator iterator;
		ai_squad_iterator_new(&iterator, ai_index);
		s_squad_datum *squad;
		while ((squad = ai_squad_iterator_next(&iterator)) != NULL)
		{
			switch (mode)
			{
			case 0:
				result += squad->count_a;
				break;
			case 1:
				result += squad->count_c;
				break;
			default:
			{
				long difference = squad->count_a - squad->count_c;
				result += difference < 0 ? 0 : difference;
				break;
			}
			}
			vitality += squad->actor_count * squad->value10;
			count += squad->actor_count;
		}
	}
	if (actor_count)
		*actor_count = count;
	if (average_vitality)
	{
		if (count > 0)
			*average_vitality = vitality / count;
		else
			*average_vitality = 0.0f;
	}
	return result;
}

void function_1df6c0(short team_a, short team_b, bool team_b_provokes, bool team_a_provokes,
	short incident_threshold, short incident_decay_ticks);

/* makes two teams allies; an alliance with the player team (1) breaks after
   five incidents and mends after a time that grows with the difficulty */
// @retail 0x2742f0
void function_2742f0(short team_a, short team_b)
{
	if (team_a != NONE && team_b != NONE)
	{
		long difficulty = 1;
		short ai_team = NONE;
		short incident_threshold = NONE;
		short incident_decay_ticks = NONE;
		bool player_alliance = false;

		if (team_a == 1)
			ai_team = team_b;
		else if (team_b == 1)
			ai_team = team_a;
		if (ai_team != NONE)
		{
			real decay_seconds[4] = { 10.0f, 15.0f, 40.0f, 90.0f };

			if (g_4e6948->state == 1)
				difficulty = g_4e6948->difficulty;
			incident_decay_ticks = (short)ai_seconds_to_ticks_round(decay_seconds[(short)difficulty]);
			incident_threshold = 5;
			player_alliance = true;
		}
		function_1df6c0(team_a, team_b, player_alliance && team_a == ai_team, player_alliance && team_b == ai_team,
			incident_threshold, incident_decay_ticks);
	}
}

/* an actor that may board a vehicle: whether it is busy with a vehicle
   entry already, then nearest first */
struct s_vehicle_load_candidate
{
	long actor_index;
	real distance_squared;
	bool busy;
};

/* the slot of type 0x4c (entering a vehicle seat) */
struct s_vehicle_enter_slot
{
	short type;
	byte unknown02[0x1c - 0x2];
	long vehicle_index;
	short seat_index;
	byte flag0 : 1;
	byte unknown22_1 : 2;
	byte flag3 : 1;
	byte unknown22_4 : 1;
	byte flag5 : 1;
	byte flag6 : 1;
	byte unknown22_7 : 1;
	byte unknown23[0x28 - 0x23];
	real unknown28;
	real unknown2c;
	byte unknown30[0x40 - 0x30];
};

struct s_slot;
struct s_object;
point3f *function_b9dd0(long object_index, point3f *result);
s_object *function_badc0(long object_index, dword type_mask);
long function_1b8c80(long object_index);
long function_2116f0(long unit_index, long filter_range, long seat_type, long occupancy, s_object_seat *results, long maximum_count);
short function_1a6fe0(long owner_index, short type);
bool function_1a80e0(long index, short type, s_slot *data, short slot);
bool function_e68c0(long type, long unit_index);

real distance_sq3f(point3f const *a, point3f const *b); /* unknown_023540.cpp */

/* the unit definition's seats, as function_274400 reads them */
struct s_unit_definition_274400
{
	byte unknown000[0x1c8];
	long seat_count;
	s_unit_seat_definition *seats;
};

struct s_object_274400
{
	long definition_index;
};

/* the first seat of a vehicle flagged as the driver's (bit 2), NONE if none */
// @retail 0x274400
short function_274400(long vehicle_index)
{
	s_object_274400 *vehicle = (s_object_274400 *)((s_object_header_view *)g_4e0300->data)[vehicle_index & 0xffff].object;
	s_unit_definition_274400 *definition = (s_unit_definition_274400 *)g_4e3b44[vehicle->definition_index & 0xffff].bytes;
	short result = NONE;

	for (short seat_index = 0; seat_index < definition->seat_count; seat_index++)
	{
		s_unit_seat_definition *seat = &definition->seats[seat_index];
		if (TEST_FIELD_BIT(seat->flags.bit2))
		{
			result = seat_index;
			break;
		}
	}
	return result;
}

// @retail 0x274a10
PRIVATE int __cdecl vehicle_load_candidate_compare(void const *a, void const *b)
{
	s_vehicle_load_candidate const *candidate_a = (s_vehicle_load_candidate const *)a;
	s_vehicle_load_candidate const *candidate_b = (s_vehicle_load_candidate const *)b;

	if (candidate_a->busy != candidate_b->busy)
		return candidate_a->busy ? 1 : -1;
	if (candidate_b->distance_squared > candidate_a->distance_squared)
		return -1;
	if (candidate_a->distance_squared > candidate_b->distance_squared)
		return 1;
	return 0;
}

PRIVATE __forceinline bool function_274a51(long arg_0, s_object_seat const *arg_1, s_unit_request *arg_2)
{
	arg_2->type = 0x1c;
	arg_2->type1c.object_index = arg_1->object_index;
	arg_2->type1c.seat_index = arg_1->seat_index;
	arg_2->type1c.unknowna = true;
	arg_2->type1c.unknownb = false;
	return function_e6900(arg_0, arg_2);
}

/* puts the actors an ai index names into the free seats of a vehicle (or
   makes them walk to them), nearest first, best seat first */
// @retail 0x274a50
void function_274a50(long ai_index, long vehicle_index, long filter_range, bool load)
{
	if (ai_index != NONE && function_badc0(vehicle_index, 3))
	{
		short candidate_count = 0;
		long unit_index = function_1b8c80(vehicle_index);
		point3f position;
		s_object_seat seats[64];
		s_vehicle_load_candidate candidates[64];
		short seat_count;
		s_vehicle_enter_slot slot;
		s_unit_request request;

		function_b9dd0(unit_index, &position);
		seat_count = function_2116f0(unit_index, filter_range, 4, 2, seats, sizeof(seats) / sizeof(seats[0]));
		if (seat_count > 0)
		{
			s_ai_actor_iterator iterator;
			s_actor_datum *actor;

			ai_actor_iterator_new(ai_index, &iterator);
			while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
			{
				if (actor->unknown26c != unit_index && candidate_count < sizeof(candidates) / sizeof(candidates[0]))
				{
					vector3f vector;

					candidates[candidate_count].actor_index = iterator.actor_index;
					vector3d_from_points3d(&actor->position, &position, &vector);
					candidates[candidate_count].distance_squared = length_sq3f(&vector);
					candidates[candidate_count].busy = function_1a6fe0(iterator.actor_index, 0x4c) != NONE;
					candidate_count++;
				}
			}

			qsort(candidates, candidate_count, sizeof(s_vehicle_load_candidate), vehicle_load_candidate_compare);
			for (short candidate_index = 0; candidate_index < candidate_count; candidate_index++)
			{
				s_vehicle_load_candidate *candidate = &candidates[candidate_index];
				s_actor_datum *candidate_actor = actor_datum_get(candidate->actor_index);
				real best_score = 0.0f;
				short best_seat_index = NONE;

				for (short seat_index = 0; seat_index < seat_count; seat_index++)
				{
					s_object_seat *seat = &seats[seat_index];

					if (seat->object_index != NONE && seat->seat_index != NONE &&
						function_c8200(seat->object_index, seat->seat_index, candidate_actor->unit_index))
					{
						real score;

						if (TEST_FIELD_BIT(seat->definition->flags.bit2))
							score = 3.0f;
						else if (TEST_FIELD_BIT(seat->definition->flags.bit3))
							score = 2.0f;
						else
						{
							score = 1.0f;
							if (TEST_FIELD_BIT(seat->definition->flags.bit11))
								score = 0.1f;
						}

						if (score > best_score)
						{
							best_score = score;
							best_seat_index = seat_index;
						}
					}
				}

				if (best_seat_index != NONE)
				{
					s_object_seat *seat = &seats[best_seat_index];
					s_slot_object_view *unit = object_get(candidate_actor->unit_index);
					bool success;

					if (unit->parent_index != NONE && unit->unknown1fc != NONE && unit->parent_index != seat->object_index)
						function_e68c0(load ? 0x1e : 0x1d, candidate_actor->unit_index);

					if (load)
					{
						success = function_274a51(candidate_actor->unit_index, seat, &request);
					}
					else
					{
						memset(&slot, 0, sizeof(slot));
						slot.vehicle_index = seat->object_index;
						slot.seat_index = seat->seat_index;
						slot.flag0 = false;
						slot.flag3 = false;
						slot.flag5 = true;
						slot.flag6 = true;
						slot.unknown28 = 3.4028235e38f;
						slot.unknown2c = 3.4028235e38f;
						success = function_1a80e0(candidate->actor_index, 0x4c, (s_slot *)&slot, 1);
					}

					if (success)
					{
						seat->object_index = NONE;
						seat->seat_index = NONE;
					}
				}
			}
		}
	}
}

bool function_211830(long filter_range, long object_index, long seat_index);

/* makes the actors an ai index names leave their vehicles (only the seats
   the filter names, unless it is NONE) */
// @retail 0x274da0
void function_274da0(long ai_index, long filter_range)
{
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;

	ai_actor_iterator_new(ai_index, &iterator);
	actor = ai_actor_iterator_next(&iterator);
	if (actor)
	{
		bool all_seats = filter_range == NONE;

		do
		{
			bool unload = all_seats;

			if (!unload && actor->unknown26c != NONE)
			{
				s_slot_object_view *unit = object_get(actor->unit_index);

				if (unit->parent_index != NONE && unit->unknown1fc != NONE)
					unload = function_211830(filter_range, unit->parent_index, unit->unknown1fc);
			}

			if (unload && actor->unknown26c != NONE && actor->unit_index != NONE)
			{
				s_unit_request request;

				memset(&request, 0, sizeof(request));
				request.type = 0x1d;
				function_e6900(actor->unit_index, &request);
			}

			actor = ai_actor_iterator_next(&iterator);
		} while (actor);
	}
}

/* the actor (0x888 bytes) as 2758b0 reads it: its unit and character */
struct s_ai_actor_2758b0
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x54 - 0x1c];
	long character_index;
	byte unknown058[0x888 - 0x58];
};

/* the unit as 2758b0 reads it: two values that set two flags, and its
   grenades (the current type and a count of each) */
struct s_ai_unit_2758b0
{
	byte unknown000[0xe4];
	real valuee4;
	real valuee8;
	real valueec;
	real valuef0;
	byte unknown0f4[0x23c - 0xf4];
	char current_grenade_index;
	char desired_grenade_index;
	char grenade_counts[4];
};

/* the character's grenade properties of a grenade type (1e53e0) */
struct s_character_grenades_2758b0
{
	byte unknown00[4];
	short grenade_type;
	byte unknown06[0x34 - 6];
	short minimum_count;
	short maximum_count;
};

void *function_1e53e0(long character_index, short key);

inline s_ai_unit_2758b0 *ai_unit_get(long unit_index)
{
	return (s_ai_unit_2758b0 *)((s_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;
}

inline s_character_grenades_2758b0 *actor_get_grenade_properties(long actor_index)
{
	s_ai_actor_2758b0 *actor = &((s_ai_actor_2758b0 *)g_4f55f0->data)[actor_index & 0xffff];
	s_character_grenades_2758b0 *result = NULL;
	if (actor->unit_index != NONE)
	{
		short grenade_index = ai_unit_get(actor->unit_index)->current_grenade_index;
		if (grenade_index != NONE)
			result = (s_character_grenades_2758b0 *)function_1e53e0(actor->character_index, grenade_index);
	}
	return result;
}

inline short unit_get_current_grenade_count(long unit_index)
{
	s_ai_unit_2758b0 *unit = ai_unit_get(unit_index);
	short grenade_index = unit->current_grenade_index;
	short count;
	if (grenade_index != NONE)
		count = unit->grenade_counts[grenade_index];
	else
		count = 0;
	return count;
}

inline void unit_add_grenades(long unit_index, short grenade_type, char count)
{
	ai_unit_get(unit_index)->grenade_counts[grenade_type] += count;
	s_ai_unit_2758b0 *unit = ai_unit_get(unit_index);
	unit->desired_grenade_index = (char)grenade_type;
	unit->current_grenade_index = (char)grenade_type;
}

inline short ai_random_range(dword *seed, short lower, short upper)
{
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return lower + (short)(((upper - lower) * (*seed >> 16)) >> 16);
}

/* gives the actors an ai index names a random number of grenades of their
   current type, by their character */
// @retail 0x2758b0
void function_2758b0(long ai_index)
{
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;

	ai_actor_iterator_new(ai_index, &iterator);
	while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
	{
		long unit_index = actor->unit_index;
		if (unit_index != NONE)
		{
			s_character_grenades_2758b0 *grenades = actor_get_grenade_properties(iterator.actor_index);
			s_ai_unit_2758b0 *unit = ai_unit_get(unit_index);

			unit->valueec = unit->valuee4 > 0.0f ? 1.0f : 0.0f;
			unit->valuef0 = unit->valuee8 > 0.0f ? 1.0f : 0.0f;
			if (grenades)
			{
				short count = ai_random_range(&g_4e7408->unknown0, grenades->minimum_count, grenades->maximum_count + 1);
				short current_count = unit_get_current_grenade_count(actor->unit_index);
				if (current_count < count)
					unit_add_grenades(actor->unit_index, grenades->grenade_type, (char)(count - current_count));
			}
		}
	}
}

// @retail 0x275a50
void function_275a50(long ai_index, bool flag)
{
	if (g_4f55d0->active && ai_index != NONE)
	{
		s_ai_squad_iterator iterator;
		ai_squad_iterator_new(&iterator, ai_index);
		s_squad_datum *squad;
		while ((squad = ai_squad_iterator_next(&iterator)) != NULL)
		{
			if (flag)
				squad->flag9 = true;
			else
				squad->flag9 = false;
		}
	}
}

// @retail 0x275ad0
void function_275ad0(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		long actor_index = ((s_ai_script_unit *)ai_script_object_get(unit_index))->actor_index;
		if (actor_index != NONE)
		{
			s_actor_datum *actor = actor_datum_get(actor_index);
			if (actor->flag00a)
				actor->flag00b = flag;
		}
	}
}

/* the objects (a local view) */
struct s_ai_script_vehicle_object
{
	byte unknown000[0x14];
	long parent_object_index;
	byte unknown018[0xaa - 0x18];
	byte object_type;
};

inline s_ai_script_vehicle_object *ai_script_vehicle_object_get(long object_index)
{
	return (s_ai_script_vehicle_object *)ai_script_object_get(object_index);
}

/* the vehicle the actor an ai index names rides in */
// @retail 0x275e20
long function_275e20(long ai_index)
{
	long result = NONE;
	long type = ai_index_get_type(ai_index);
	if (type == _ai_index_type_actor || type == _ai_index_type_starting_location)
	{
		long actor_index = function_272b70(ai_index);
		if (actor_index != NONE)
		{
			s_actor_datum *actor = (s_actor_datum *)datum_get_inlined(g_4f55f0, actor_index);
			if (actor)
			{
				long parent_index = ai_script_vehicle_object_get(actor->unit_index)->parent_object_index;
				if (parent_index != NONE && ai_script_vehicle_object_get(parent_index)->object_type == 1)
				{
					result = parent_index;
				}
			}
		}
	}
	return result;
}

void function_1e4650(long actor_index, bool value);

/* sets a flag (1e4650) on every actor an ai index names */
// @retail 0x275b20
void function_275b20(long ai_index, bool value)
{
	if (ai_index != NONE)
	{
		s_ai_actor_iterator iterator;

		ai_actor_iterator_new(ai_index, &iterator);
		while (ai_actor_iterator_next(&iterator))
			function_1e4650(iterator.actor_index, value);
	}
}

/* the sum of a count of the squads an ai index names */
// @retail 0x275d70
short function_275d70(long ai_index)
{
	long result = 0;
	s_ai_squad_iterator iterator;
	ai_squad_iterator_new_inline(&iterator, ai_index);
	s_squad_datum *squad;
	while ((squad = ai_squad_iterator_next(&iterator)) != NULL)
		*(short *)&result += squad->value24;
	return *(short *)&result;
}
/* the objects (a local view) */
struct s_ai_script_seat_unit
{
	byte unknown000[0x3b0];
	long value3b0;
};

/* sets or clears the bit of the seat of a vehicle with the given label in the
   unit holding it */
// @retail 0x275eb0
bool function_275eb0(long vehicle_index, long seat_label, bool flag)
{
	bool result = false;
	if (vehicle_index != NONE)
	{
		s_object_seat seats[0x40];
		short count = 0;
		function_c8a40(vehicle_index, seats, &count, 0x40);
		for (short i = 0; i < count; i++)
		{
			s_object_seat *seat = &seats[i];
			if (seat->definition->label == seat_label && object_header_get(seat->object_index)->type == 1)
			{
				s_ai_script_seat_unit *unit = (s_ai_script_seat_unit *)object_header_get(seat->object_index)->object;
				if (flag)
					unit->value3b0 |= 1 << seat->seat_index;
				else
					unit->value3b0 &= ~(1 << seat->seat_index);
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x275fc0
bool function_275fc0(long vehicle_index, bool flag)
{
	bool result = false;
	if (vehicle_index != NONE)
	{
		s_object_seat seats[0x40];
		short count = 0;
		function_c8a40(vehicle_index, seats, &count, 0x40);
		long object_index = NONE;
		if (count > 0)
		{
			s_object_seat *seat = seats;
			for (short i = count; i > 0; i--, seat++)
			{
				if (object_index != seat->object_index)
				{
					s_object_header_view *header = object_header_get(seat->object_index);
					if (header->type == 1)
						((s_ai_script_seat_unit *)header->object)->value3b0 = flag ? NONE : 0;
					object_index = seat->object_index;
				}
			}
		}
		result = true;
	}
	return result;
}
// @retail 0x276050
short function_276050(long ai_index)
{
	short result = 0;
	s_ai_actor_iterator iterator;
	ai_actor_iterator_new(ai_index, &iterator);
	s_actor_datum *actor = ai_actor_iterator_next(&iterator);
	while (actor)
	{
		if (actor->value086 > result)
			result = actor->value086;
		actor = ai_actor_iterator_next(&iterator);
	}
	return result;
}

extern long const g_444ae0;
void __stdcall function_189cd0(long sound_index, long object_index, real scale, long a, long b, long name, long flags);

/* plays a sound on the unit of an actor and makes its command script (or the
   actor) wait for it */
// @retail 0x2760a0
real function_2760a0(long actor_index, long script_index, long name, long sound_index, real scale, real pitch)
{
	real duration;
	function_189cd0(sound_index, actor_datum_get(actor_index)->unit_index, pitch, g_444ae0, g_444ae0, name, (long)&duration);

	real seconds = duration * scale;
	real local_0 = (real)g_510c54->field_2_3 * seconds;
	__asm
	{
		fld local_0
		fistp duration
	}

	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		real local_1 = (real)*(long *)&duration;
		script->type = 0;
		script->value8 = local_1;
	}
	else
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		long ticks = *(volatile long *)&duration;
		if (actor->value620 < (short)ticks)
			actor->value620 = (short)ticks;
	}
	return seconds;
}

bool function_291ea0(long arg_80f1d4, long script_index, long actor_index, real *duration);

/* the ticks a vocalization of the first actor an ai index names lasts */
// @retail 0x276160
short function_276160(long ai_index, long arg_80f1d4)
{
	real duration = 0.0f;
	if (arg_80f1d4 != NONE)
	{
		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		if (ai_actor_iterator_next(&iterator))
		{
			real seconds;
			function_291ea0(arg_80f1d4, NONE, iterator.actor_index, &seconds);
			if (seconds > g_45dbd8)
				duration = seconds;
		}
	}
	real ticks_real = duration * 30.0f;
	long ticks;
	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return (short)ticks;
}

/* the actor's prop references (g_502418, 0x3c bytes each), chained from the
   actor's +0x58, and the props (g_50241c, 0xc4 bytes each) they name */
struct s_ai_prop_reference_2761d0
{
	byte unknown00[8];
	long prop_index;
	byte unknown0c[0x20 - 0xc];
	long object_index;
	short state;
	byte unknown26[2];
	real unknown28;
	long next_index;
	byte unknown30[0x3c - 0x30];
};

struct s_ai_prop_2761d0
{
	byte unknown00[0x25];
	bool unknown25;
	byte unknown26[0xc4 - 0x26];
};

struct s_ai_actor_2761d0
{
	byte unknown000[0x58];
	long first_prop_index;
	byte unknown05c[0x22c - 0x5c];
	point3f position22c;
	byte unknown238[0x684 - 0x238];
	short unknown684;
	short unknown686;
	short unknown688;
	byte unknown68a[2];
	long unknown68c;
	byte unknown690[0x888 - 0x690];
};

struct s_ai_actor_prop_iterator
{
	long index;
	long next;
};

/* function_26be00 and function_26be30 (unknown_26bda0.cpp), which retail
   inlines here */
static inline void actor_prop_iterator_new(long actor_index, s_ai_actor_prop_iterator *iterator)
{
	s_ai_actor_2761d0 *actor = &((s_ai_actor_2761d0 *)g_4f55f0->data)[actor_index & 0xffff];
	iterator->next = actor->first_prop_index;
}

static inline s_ai_prop_reference_2761d0 *actor_prop_iterator_next(s_ai_actor_prop_iterator *iterator)
{
	s_ai_prop_reference_2761d0 *reference = 0;
	long index = iterator->next;

	if (index != NONE)
	{
		reference = &((s_ai_prop_reference_2761d0 *)g_502418->data)[index & 0xffff];
		iterator->index = index;
		iterator->next = reference->next_index;
	}

	return reference;
}

/* plays a vocalization on the first actor an ai index names and points that
   actor at the prop it rates highest; returns the vocalization's ticks */
// @retail 0x2761d0
short function_2761d0(long ai_index, long arg_80f1d4)
{
	real duration = 0.0f;
	if (arg_80f1d4 != NONE)
	{
		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		if (ai_actor_iterator_next(&iterator))
		{
			long actor_index = iterator.actor_index;
			long best_index = NONE;
			real best_rating = 0.0f;
			real seconds = 0.0f;

			function_291ea0(arg_80f1d4, NONE, actor_index, &seconds);
			if (seconds > g_45dbd8)
				duration = seconds;

			s_ai_actor_2761d0 *actor = &((s_ai_actor_2761d0 *)g_4f55f0->data)[actor_index & 0xffff];
			s_ai_actor_prop_iterator prop_iterator;
			s_ai_prop_reference_2761d0 *reference;

			actor_prop_iterator_new(actor_index, &prop_iterator);
			while ((reference = actor_prop_iterator_next(&prop_iterator)) != NULL)
			{
				s_ai_prop_2761d0 *prop = &((s_ai_prop_2761d0 *)g_50241c->data)[reference->prop_index & 0xffff];
				if (prop->unknown25)
				{
					real rating = 0.1f;
					if (reference->state >= 1 && reference->state <= 2)
						rating = 15.1f;
					if (15.0f > reference->unknown28)
						rating = 15.0f - reference->unknown28 + rating;
					if (rating > best_rating)
					{
						best_rating = rating;
						best_index = prop_iterator.index;
					}
				}
			}

			if (best_index != NONE)
			{
				s_ai_prop_reference_2761d0 *best = &((s_ai_prop_reference_2761d0 *)g_502418->data)[best_index & 0xffff];
				if (actor->unknown684 <= 1)
				{
					actor->unknown686 = (short)ai_seconds_to_ticks_round(2.0f);
					actor->unknown688 = 6;
					actor->unknown68c = best->object_index;
					actor->unknown684 = 1;
				}
			}
		}
	}
	real ticks_real = duration * 30.0f;
	long ticks;
	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return (short)ticks;
}

/* the players (g_4e8c24), 0x21c bytes each */
struct s_player_276dd0
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0x21c - 0x30];
};

/* record_pool_iterator_step (unknown_16b570.cpp) with its next used index search,
   both inlined; the search returns from inside its loop */
static inline long player_data_next_index(s_record_pool *data, long index)
{
	if (index >= 0 && index < data->high_water_index)
	{
		long count = data->high_water_index;
		dword *bits = data->bitmap;
		do
		{
			if (bits[index >> 5] & (1 << (index & 0x1f)))
				return index;
			index++;
		} while (index < count);
	}
	return NONE;
}

static inline byte *player_iterator_next(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = player_data_next_index(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	return result;
}

/* the object an actor would look at: the object of the nearest prop it
   perceives, else the unit of the nearest player */
// @retail 0x276dd0
long function_276dd0(long actor_index)
{
	real best_distance = 3.4028235e38f;
	long result = NONE;
	s_ai_actor_2761d0 *actor = &((s_ai_actor_2761d0 *)g_4f55f0->data)[actor_index & 0xffff];
	s_ai_actor_prop_iterator iterator;
	s_ai_prop_reference_2761d0 *reference;

	actor_prop_iterator_new(actor_index, &iterator);
	while ((reference = actor_prop_iterator_next(&iterator)) != NULL)
	{
		if (reference->state >= 1)
		{
			s_ai_prop_2761d0 *prop = &((s_ai_prop_2761d0 *)g_50241c->data)[reference->prop_index & 0xffff];
			if (prop->unknown25 && best_distance > reference->unknown28)
			{
				result = reference->object_index;
				best_distance = reference->unknown28;
			}
		}
	}

	if (result == NONE)
	{
		real best_distance_squared = 3.4028235e38f;
		s_record_pool_iterator local_51cbf5;
		s_player_276dd0 *player;

		local_51cbf5.data = g_4e8c24;
		local_51cbf5.index = NONE;
		while ((player = (s_player_276dd0 *)player_iterator_next(&local_51cbf5)) != NULL)
		{
			if (player->unit_index != NONE)
			{
				s_object_marker marker;
				function_b8d30(player->unit_index, 0x4000095, &marker, 1, false);
				point3f position = marker.matrix.position;
				vector3f vector;
				vector3d_from_points3d(&position, &actor->position22c, &vector);
				real distance_squared = length_sq3f(&vector);
				if (best_distance_squared > distance_squared)
				{
					result = player->unit_index;
					best_distance_squared = distance_squared;
				}
			}
		}
	}

	return result;
}

PRIVATE __forceinline bool function_276381(long arg_0, s_unit_request *arg_1)
{
	arg_1->type = 0x24;
	return function_e6900(arg_0, arg_1);
}

/* whether an actor an ai index names runs the command script named */
// @retail 0x276380
bool function_276380(long ai_index)
{
	bool result = false;
	long actor_index = function_272b70(ai_index);
	if (actor_index != NONE)
	{
		s_unit_request request = { 0 };
		result = function_276381(actor_datum_get(actor_index)->unit_index, &request);
	}
	return result;
}
long function_258040(long actor_index, short script_index, long thread_index);

short flock_definition_find(long name);
bool function_2930c0(long definition_index);

/* the scenario's flocks (0x84 bytes each; unknown_293070.cpp): their structure
   bsp */
struct s_scenario_flock_2763f0
{
	short field_0_3;
	byte unknown02[0x84 - 2];
};

struct s_scenario_flocks_2763f0
{
	byte unknown000[0x354];
	s_scenario_flock_2763f0 *flocks;
};

/* creates the flock named, if it is in the current structure bsp */
// @retail 0x2763f0
bool function_2763f0(long name)
{
	volatile bool result = false;
	short definition_index = flock_definition_find(name);
	if (definition_index != NONE &&
		((s_scenario_flocks_2763f0 *)g_4e0350)->flocks[definition_index].field_0_3 == g_4686c4)
	{
		return function_2930c0(definition_index);
	}
	return result;
}

/* gives every actor an ai index names a command script (258040) */
// @retail 0x276440
void function_276440(long ai_index, short script_index)
{
	s_ai_actor_iterator iterator;

	ai_actor_iterator_new(ai_index, &iterator);
	while (ai_actor_iterator_next(&iterator))
		function_258040(iterator.actor_index, script_index, NONE);
}

long function_257fa0(long actor_index, short script_index, long thread_index);

/* gives every actor an ai index names a command script (257fa0) */
// @retail 0x276480
void function_276480(long ai_index, short script_index)
{
	s_ai_actor_iterator iterator;

	ai_actor_iterator_new(ai_index, &iterator);
	while (ai_actor_iterator_next(&iterator))
		function_257fa0(iterator.actor_index, script_index, NONE);
}

long function_257ed0(long thread_index, long actor_index, short script_index);
bool function_2580c0(short squad_index, short script_index, long *actor_indices, short count);

/* gives every actor an ai index names a command script (257ed0) */
// @retail 0x2764c0
void function_2764c0(long ai_index, short script_index)
{
	s_ai_actor_iterator iterator;

	ai_actor_iterator_new(ai_index, &iterator);
	while (ai_actor_iterator_next(&iterator))
		function_257ed0(NONE, iterator.actor_index, script_index);
}

/* gives the first actors two ai indices name a shared command script */
// @retail 0x276500
bool function_276500(short script_index, long ai_index0, long ai_index1)
{
	long actor_indices[2];
	s_ai_actor_iterator iterator;

	ai_actor_iterator_new(ai_index0, &iterator);
	if (ai_actor_iterator_next(&iterator))
	{
		actor_indices[0] = iterator.actor_index;
		ai_actor_iterator_new(ai_index1, &iterator);
		if (ai_actor_iterator_next(&iterator))
		{
			actor_indices[1] = iterator.actor_index;
			return function_2580c0(NONE, script_index, actor_indices, 2);
		}
	}
	return false;
}

/* gives the first actors three ai indices name a shared command script */
// @retail 0x276560
bool function_276560(short script_index, long ai_index0, long ai_index1, long ai_index2)
{
	long actor_indices[3];
	s_ai_actor_iterator iterator;

	ai_actor_iterator_new(ai_index0, &iterator);
	if (ai_actor_iterator_next(&iterator))
	{
		actor_indices[0] = iterator.actor_index;
		ai_actor_iterator_new(ai_index1, &iterator);
		if (ai_actor_iterator_next(&iterator))
		{
			actor_indices[1] = iterator.actor_index;
			ai_actor_iterator_new(ai_index2, &iterator);
			if (ai_actor_iterator_next(&iterator))
			{
				actor_indices[2] = iterator.actor_index;
				return function_2580c0(NONE, script_index, actor_indices, 3);
			}
		}
	}
	return false;
}

/* the object position (+0x30) of a player's unit */
struct s_object_2765e0
{
	byte unknown00[0x30];
	point3f center;
};

/* unknown_0259d0's squared distance, inlined (the same terms as
   unknown_29f5b0.cpp's distance3d_inline) */
inline real ai_distance_squared3d(point3f const *a, point3f const *b)
{
	vector3f v;
	v.i = a->x - b->x;
	v.j = a->y - b->y;
	v.k = a->z - b->z;
	return v.j * v.j + (v.i * v.i + v.k * v.k);
}

/* whether the unit of a player is within a distance of an actor an ai index
   names */
// @retail 0x2765e0
bool function_2765e0(long ai_index, real distance)
{
	real distance_squared = distance * distance;
	bool result = false;
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;

	ai_actor_iterator_new(ai_index, &iterator);
	while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
	{
			s_record_pool_iterator local_51cbf5;
			s_player_276dd0 *player;

			local_51cbf5.data = g_4e8c24;
			local_51cbf5.index = NONE;
			while ((player = (s_player_276dd0 *)player_iterator_next(&local_51cbf5)) != NULL)
			{
				if (player->unit_index != NONE)
				{
					point3f *center = &((s_object_2765e0 *)((s_object_header_view *)g_4e0300->data)[player->unit_index & 0xffff].object)->center;
					if (distance_squared > ai_distance_squared3d(center, &actor->position))
					{
						result = true;
						break;
					}
				}
			}
		if (result)
			break;
	}
	return result;
}

/* the joint command scripts (g_502404, 0x8c bytes each; unknown_257d00.cpp) */
struct s_ai_script_joint
{
	byte unknown00[8];
	short scene_index;
	byte unknown0a[0x8c - 0xa];
};

extern s_record_pool *g_502404;

/* the scenario's scenes and their roles (local views) */
struct s_ai_script_scene_role
{
	long name;
	byte unknown04[0x10 - 0x4];
};

struct s_ai_script_scene
{
	byte unknown00[0x10];
	long role_count;
	s_ai_script_scene_role *roles;
};

struct s_ai_script_scenes_view
{
	byte unknown000[0x170];
	long scene_count;
	s_ai_script_scene *scenes;
};

bool function_258340(short participant_index, long joint_index);

bool function_258230(long cs_index, short mode, long actor_index, long new_actor_index);

/* makes the current command script hand itself (258230) to the first actor
   an ai index names */
// @retail 0x276860
void function_276860(long ai_index, short mode)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		command_script_get(script_index)->type = 0x17;

		s_ai_actor_iterator iterator;
		ai_actor_iterator_new(ai_index, &iterator);
		if (ai_actor_iterator_next(&iterator) && g_50240c != iterator.actor_index)
			function_258230(script_index, mode, g_50240c, iterator.actor_index);
	}
}

/* makes the current command script take the role named in its joint command
   script */
// @retail 0x2768d0
void function_2768d0(long role_name)
{
	if (g_502410 != NONE)
	{
		s_command_script *script = command_script_get(g_502410);
		long joint_index = script->joint_index;

		script->type = 0x16;
		if (joint_index != NONE)
		{
			short scene_index = ((s_ai_script_joint *)(g_502404->data + (joint_index & 0xffff) * sizeof(s_ai_script_joint)))->scene_index;
			s_ai_script_scenes_view *scenario = (s_ai_script_scenes_view *)g_4e0350;

			if (scene_index != NONE && scene_index >= 0 && scene_index < scenario->scene_count)
			{
				s_ai_script_scene *scene = &scenario->scenes[scene_index];
				short participant_index = NONE;

				if (role_name != NONE)
				{
					for (short i = 0; i < scene->role_count; i++)
					{
						if (scene->roles[i].name == role_name)
						{
							participant_index = i;
							break;
						}
					}
				}
				if (participant_index != NONE)
					function_258340(participant_index, joint_index);
			}
		}
	}
}

// @retail 0x2766f0
bool function_2766f0(long ai_index, long name_index)
{
	bool result = false;
	s_ai_actor_iterator iterator;
	ai_actor_iterator_new(ai_index, &iterator);
	s_actor_datum *actor = ai_actor_iterator_next(&iterator);
	while (actor)
	{
		if (actor->active_command_script_index != NONE && command_script_get(actor->active_command_script_index)->name_index == name_index)
		{
			result = true;
			break;
		}
		actor = ai_actor_iterator_next(&iterator);
	}
	return result;
}

/* whether an actor an ai index names has the command script named queued */
// @retail 0x276770
bool function_276770(long ai_index, long name_index)
{
	bool result = false;
	s_ai_actor_iterator iterator;
	ai_actor_iterator_new(ai_index, &iterator);
	s_actor_datum *actor = ai_actor_iterator_next(&iterator);
	while (actor)
	{
		long script_index = actor->command_script_index;
		while (script_index != NONE)
		{
			s_command_script *script = command_script_get(script_index);
			if (script->name_index == name_index)
			{
				result = true;
				break;
			}
			script_index = script->next_index;
		}
		if (result)
			break;
		actor = ai_actor_iterator_next(&iterator);
	}
	return result;
}

/* the length of the chain of command scripts of the actor an ai index names */
// @retail 0x2767f0
short function_2767f0(long ai_index)
{
	short count = 0;
	long type = ai_index_get_type(ai_index);
	if (type == _ai_index_type_actor || type == _ai_index_type_starting_location)
	{
		long actor_index = function_272b70(ai_index);
		if (actor_index != NONE)
		{
			long script_index = actor_datum_get(actor_index)->command_script_index;
			while (script_index != NONE)
			{
				script_index = command_script_get(script_index)->next_index;
				count++;
			}
		}
	}
	return count;
}

/* points the current command script at a point of a point set */
// @retail 0x276990
void function_276990(long point_reference)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->type = 1;
		script->index_a = point_reference;
		script->value8 = 0.0f;
	}
}

/* the scenario's point sets (the scripting data of g_4e0350) */
struct s_scenario_point
{
	byte unknown00[0x20];
	s_type_c3b527 position;
	byte unknown2e[0x3c - 0x2e];
};

struct s_scenario_point_set
{
	byte unknown00[0x20];
	long point_count;
	s_scenario_point *points;
	byte unknown28[0x30 - 0x28];
};

struct s_scenario_scripting_data
{
	long point_set_count;
	s_scenario_point_set *point_sets;
};

struct s_scenario_scripting_view
{
	byte unknown000[0x1d8];
	long scripting_data_count;
	s_scenario_scripting_data *scripting_data;
};


/* points the current command script at the point of a point set nearest the
   current actor */
// @retail 0x2769d0
void function_2769d0(long point_reference)
{
	if (g_50240c != NONE)
	{
		s_scenario_scripting_view *scenario = (s_scenario_scripting_view *)g_4e0350;
		if (scenario->scripting_data_count > 0)
		{
			s_scenario_scripting_data *local_0 = scenario->scripting_data;
			s_actor_datum *actor = actor_datum_get(g_50240c);
			short point_set_index = (short)(point_reference >> 16);
			if (point_set_index >= 0 && point_set_index < local_0->point_set_count)
			{
				s_scenario_point_set *point_set = &local_0->point_sets[point_set_index];
				volatile real best_distance = 3.4028235e38f;
				short best_index = NONE;
				for (short point_index = 0; point_index < point_set->point_count; point_index++)
				{
					s_type_c3b527 *point = (s_type_c3b527 *)((byte *)point_set->points + point_index * 0x3c + 0x20);
					vector3f vector;
					if (point->output_index == NONE)
					{
						vector3d_from_points3d(&point->point, &actor->position, &vector);
					}
					else
					{
						point3f position;
						function_210850(point, &position);
						vector3d_from_points3d(&position, &actor->position, &vector);
					}
					real distance = length_sq3f(&vector);
					if (best_distance > distance)
					{
						best_distance = distance;
						best_index = point_index;
					}
				}
				if (best_index != NONE)
					function_276990((point_set_index << 16) | (word)best_index);
			}
		}
	}
}

/* plays a scripted animation on the current actor's unit and makes the
   current command script wait for it (scaled) */
// @retail 0x2772b0
void __stdcall function_2772b0(long animation_graph_index, long animation_name, real scale, bool interpolate)
{
	if (g_502410 != NONE)
	{
		s_command_script *script = command_script_get(g_502410);
		s_actor_datum *actor = actor_datum_get(g_50240c);
		real duration = 0.0f;

		if (function_11b520(animation_graph_index, actor->unit_index, animation_name, interpolate, NONE, false))
		{
			real ticks_real = (real)g_510c54->field_2_3 * (function_11b6b0(actor->unit_index) * scale);
			long ticks;
			__asm
			{
				fld ticks_real
				fistp ticks
			}
			duration = (real)ticks;
		}
		script->type = 0;
		script->value8 = duration;
	}
}

/* ends the current actor's unit's scripted animation */
// @retail 0x277380
void function_277380(void)
{
	if (g_502410 != NONE)
	{
		s_actor_datum *local_0 = actor_datum_get(g_50240c);
		long unit_index = *(volatile long *)&local_0->unit_index;
		if (function_11b930(unit_index))
			function_11b710(unit_index, 0x7000101);
	}
}

/* the actors (a local view) */
struct s_actor_2773d0
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x229 - 0x1c];
	bool flying;
};

bool function_25ab50(long reference);
void function_118e80(long object_index, vector3f *forward);
vector3f *function_11d090(vector3f const *v, vector3f *out);
real function_30bf0(vector3f *v);
real normalize2d(point2f *v);
long function_baf80(long object_index);
struct s_location;
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	s_location const *location, bool unknown);
void function_b73b0(long object_index);
void __stdcall function_1f4280(long actor_index);

inline s_type_c3b527 *scenario_point_get(long point_reference)
{
	s_scenario_scripting_view *scenario = (s_scenario_scripting_view *)g_4e0350;
	return &scenario->scripting_data->point_sets[(point_reference >> 16) & 0xffff].points[point_reference & 0xffff].position;
}

/* places the current actor's unit at a point, facing another point */
// @retail 0x2773d0
void function_2773d0(long facing_point_reference, long point_reference)
{
	if (g_50240c != NONE && function_25ab50(point_reference))
	{
		s_actor_2773d0 *actor = (s_actor_2773d0 *)actor_datum_get(g_50240c);
		long unit_index = actor->unit_index;
		point3f position;
		point3f facing_position;
		vector3f forward;
		vector3f up;

		function_210850(scenario_point_get(point_reference), &position);
		function_118e80(unit_index, &forward);
		if (function_25ab50(facing_point_reference))
		{
			function_210850(scenario_point_get(facing_point_reference), &facing_position);
			vector3d_from_points3d(&position, &facing_position, &forward);
			if (actor->flying)
			{
				if (function_30bf0(&forward) == 0.0f)
					function_118e80(unit_index, &forward);
				function_11d090(&forward, &up);
			}
			else
			{
				forward.k = 0.0f;
				if (normalize2d((point2f *)&forward) == 0.0f)
				{
					function_118e80(unit_index, &forward);
					forward.k = 0.0f;
					if (normalize2d((point2f *)&forward) == 0.0f)
						forward = *g_4687a8;
				}
				up = *g_4687b0;
			}

			long object_index = function_baf80(unit_index);
			function_b75a0(object_index, &position, &forward, &up, NULL, false);
			function_b73b0(object_index);
			function_1f4280(g_50240c);
		}
	}
}

void function_259e70(long cs_index);

void function_259e70(long cs_index);

/* the current command script's distances (stored squared) to an object */
// @retail 0x276b40
void function_276b40(long object_index, real a, real b, real c)
{
	if (g_502410 != NONE)
	{
		long script_index = g_502410;
		s_command_script *script = command_script_get(script_index);

		function_259e70(script_index);
		script->valueb4 = a * a;
		script->valueb8 = b * b;
		script->type = 0x14;
		script->flagac = true;
		script->flagd0 = true;
		script->indexb0 = object_index;
		script->valuebc = c * c;

		if (object_index != NONE)
		{
			s_command_script *target = command_script_get(script_index);

			((s_command_script volatile *)target)->flag52 = true;
			((s_command_script volatile *)target)->flag46 = false;
			((s_command_script volatile *)target)->flag51 = false;
			target->type54 = 1;
			target->index58 = object_index;
			s_command_script *local_0 = command_script_get(script_index);
			local_0->flag46 = true;
			local_0->type48 = 1;
			local_0->index4c = object_index;
			target->flag50 = true;
			target->flag51 = true;
		}
	}
}

/* the current command script's distances (stored squared) */
// @retail 0x276cc0
void function_276cc0(real a, real b, real c)
{
	if (g_502410 != NONE)
	{
		s_command_script *script = command_script_get(g_502410);

		function_259e70(g_502410);
		script->valueb4 = a * a;
		script->valueb8 = b * b;
		script->type = 0x14;
		script->flagac = true;
		script->flagd0 = true;
		script->indexb0 = NONE;
		script->valuebc = c * c;
	}
}

__forceinline bool squad_in_ai_273ac0(long ai_index, long squad_index)
{
	bool result = false;
	switch (ai_index_get_type(ai_index))
	{
	case _ai_index_type_squad: result = squad_index == (ai_index & 0xffff); break;
	case _ai_index_type_squad_group: result = function_2052d0(squad_index, ai_index & 0xffff); break;
	case _ai_index_type_actor: result = false; break;
	case _ai_index_type_starting_location: result = false; break;
	}
	return result;
}

// @retail 0x273ac0
void __stdcall function_273ac0(long ai_index, long target_ai_index)
{
	dword clump_bits[1];
	s_ai_actor_iterator iterator;
	s_actor_datum *actor;
	clump_bits[0] = 0;
	ai_actor_iterator_new(ai_index, &iterator);
	while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
	{
		long clump_index = actor->clump_object_index;
		if (clump_index == NONE)
			continue;
		clump_index &= 0xffff;
		if (bit_vector_test(clump_bits, clump_index))
			continue;
		bit_vector_set(clump_bits, clump_index, true);
		long prop_index = ((s_ai_clump_273d30 *)g_502420->data)[clump_index].first_prop_index;
		while (prop_index != NONE)
		{
			s_ai_prop_273d30 *prop = &((s_ai_prop_273d30 *)g_50241c->data)[prop_index & 0xffff];
			long target_actor_index = prop->actor_index;
			if (target_actor_index != NONE)
			{
				bool target_matches = false;
				switch (ai_index_get_type(target_ai_index))
				{
				case _ai_index_type_squad:
				case _ai_index_type_squad_group:
				{
					long squad_index = actor_datum_get(target_actor_index)->squad_index;
					if (squad_index != NONE)
						target_matches = function_272c40(target_ai_index, squad_index);
					break;
				}
				case _ai_index_type_actor:
				case _ai_index_type_starting_location:
					target_matches = target_actor_index == function_272b70(target_ai_index);
					break;
				}
				if (target_matches)
				{
					long member_index = prop->first_member_index;
					while (member_index != NONE)
					{
						s_ai_prop_member_273d30 *member = &((s_ai_prop_member_273d30 *)g_502418->data)[member_index & 0xffff];
						long actor_index = member->actor_index;
						bool matches = false;
						switch (ai_index_get_type(ai_index))
						{
						case _ai_index_type_squad:
						case _ai_index_type_squad_group:
						{
							long squad_index = actor_datum_get(actor_index)->squad_index;
							if (squad_index != NONE)
							{
								matches = squad_in_ai_273ac0(ai_index, squad_index);
							}
							break;
						}
						case _ai_index_type_actor:
						case _ai_index_type_starting_location:
							matches = actor_index == function_272b70(ai_index);
							break;
						}
						if (matches)
						{
							s_ai_prop_member_273d30 *update = &((s_ai_prop_member_273d30 *)g_502418->data)[member_index & 0xffff];
							update->state = 3;
							update->timer = (short)ai_seconds_to_ticks_round(5.0f);
						}
						member_index = member->next_member_index;
					}
				}
			}
			prop_index = prop->next_prop_index;
		}
	}
}

void function_1e2a90(long arg_0);
void __stdcall function_b8540(long arg_0);
void __stdcall function_28e200(long arg_0);
bool __stdcall function_beb30(long arg_0);

struct s_2736c0
{
	byte field_0[0x3a0];
	long field_3a0;
	long field_3a4;
};

struct s_2736e0
{
	s_ai_squad_iterator field_0;
	s_ai_actor_iterator field_20;
};

// @retail 0x2736c0
void __stdcall function_2736c0(long arg_0)
{
	if (arg_0 != NONE)
	{
		s_2736e0 local_0;
		s_actor_datum *local_1;
		ai_actor_iterator_new(arg_0, &local_0.field_20);
		while ((local_1 = ai_actor_iterator_next(&local_0.field_20)) != NULL)
		{
			if (local_0.field_20.actor_index == g_46fc80)
				*((byte *)local_1 + 0xd) = true;
			else
			{
				s_actor_datum *local_2 = actor_datum_get(local_0.field_20.actor_index);
				long local_3 = local_2->unit_index;
				long local_4 = local_2->perception_index;
				function_1e2a90(local_0.field_20.actor_index);
				if (local_3 != NONE)
					function_b8540(local_3);
				else if (local_4 != NONE)
					function_28e200(local_4);
			}
		}
		s_squad_datum *local_6;
		ai_squad_iterator_new_inline(&local_0.field_0, arg_0);
		while ((local_6 = ai_squad_iterator_next(&local_0.field_0)) != NULL)
		{
			long local_7 = local_6->first_vehicle_index;
			long local_8 = NONE;
			while (local_7 != NONE)
			{
				long local_11 = local_7;
			s_object_header_view *local_9 = (s_object_header_view *)datum_get_inlined(g_4e0300, local_7);
				if (!local_9 || !((1 << local_9->type) & 2))
					break;
				s_2736c0 *local_10 = (s_2736c0 *)local_9->object;
				if (!local_10)
					break;
				local_7 = local_10->field_3a4;
				if (!function_beb30(local_11))
				{
					local_10->field_3a0 = NONE;
					function_b8540(local_11);
				}
				else
				{
					local_10->field_3a4 = local_8;
					local_8 = local_11;
				}
			}
			local_6->first_vehicle_index = local_8;
		}
	}
}

PRIVATE __forceinline bool function_274611(long arg_0, short arg_1)
{
	s_slot_object_view *local_0 = object_get(arg_0);
	return function_c8f60(arg_0, arg_1) != NONE ||
		(local_0->type == 1 && ((local_0->unknown3b0 & (1 << arg_1)) ||
		(local_0->unknown3b4 & (1 << arg_1))));
}

PRIVATE __forceinline long function_2746fc(long arg_0)
{
	long local_0 = arg_0;
	while (local_0 != NONE)
	{
		s_slot_object_view *local_1 = object_get(local_0);
		if (!TEST_FIELD_BIT((*(dword *)((byte *)local_1 + 4) >> 26) & 1)) break;
		local_0 = local_1->parent_index;
	}
	return object_header_get(local_0)->type == 1 ? local_0 : arg_0;
}

// @retail 0x274470
short __stdcall function_274470(long arg_0)
{
	struct { s_vehicle_enter_slot field_0; s_ai_actor_iterator field_40; } local_40;
	dword local_0[10] = {0};
	s_object_seat local_1[320];
	short local_2 = 0;
	short local_3 = 0;
	short local_4 = 0;
	s_ai_squad_iterator local_5;
	ai_squad_iterator_new_inline(&local_5, arg_0);
	s_squad_datum *local_6;
	while ((local_6 = ai_squad_iterator_next(&local_5)) != NULL)
	{
		long local_7 = local_6->first_vehicle_index;
		while (local_7 != NONE && local_2 < 320)
		{
			s_ai_script_squad_vehicle *local_8 = (s_ai_script_squad_vehicle *)ai_script_object_get(local_7);
			short local_9 = 0;
			function_c8a40(local_7, &local_1[local_2], &local_9, 320 - local_2);
			local_2 += local_9;
			local_7 = local_8->next_squad_vehicle_index;
		}
		if (local_2 >= 320) break;
	}
	for (long local_10 = 0; local_10 < local_2; local_10++)
		if (function_274611(local_1[local_10].object_index, local_1[local_10].seat_index))
			local_0[local_10 >> 5] |= 1 << (local_10 & 31);

	ai_actor_iterator_new(arg_0, &local_40.field_40);
	while (ai_actor_iterator_next(&local_40.field_40))
		if (function_1a6fe0(local_40.field_40.actor_index, 0x4c) == NONE) local_4++;
	for (;;)
	{
		real local_12 = 0.0f;
		short local_13 = NONE;
		long local_14 = NONE;
		bool local_15 = false;
		for (short local_16 = 0; local_16 < local_2; local_16++)
		{
			s_object_seat *local_17 = &local_1[local_16];
			if (local_17->object_index != local_14)
			{
				local_14 = local_17->object_index;
				long local_18 = function_2746fc(local_14);
				local_15 = function_274611(local_18, function_274400(local_18));
			}
			if (!(local_0[local_16 >> 5] & (1 << (local_16 & 31))))
			{
				real local_19 = 0.0f;
				if (TEST_FIELD_BIT(local_17->definition->flags.bit2))
				{
					if (TEST_FIELD_BIT(local_17->definition->flags.bit3)) local_19 = 3.0f;
					else if (local_4 > 0) local_19 = 5.0f;
				}
				else if (TEST_FIELD_BIT(local_17->definition->flags.bit3))
				{
					if (local_15) local_19 = 20.0f;
				}
				else if (!TEST_FIELD_BIT(local_17->definition->flags.bit11))
				{
					if (local_15) local_19 = 1.0f;
				}
				if (local_19 > local_12) { local_12 = local_19; local_13 = local_16; }
			}
		}
		if (local_13 == NONE) break;
		long local_20 = local_1[local_13].object_index;
		short local_21 = local_1[local_13].seat_index;
		real local_22 = 3.402823466e38f;
		long local_23 = NONE;
		if (local_12 == 0.0f) break;
		point3f local_24;
		function_b9dd0(local_20, &local_24);
		s_ai_actor_iterator local_25;
		ai_actor_iterator_new(arg_0, &local_25);
		s_actor_datum *local_26;
		while ((local_26 = ai_actor_iterator_next(&local_25)) != NULL)
		{
			if (function_1a6fe0(local_25.actor_index, 0x4c) == NONE && local_26->unknown26c == NONE)
			{
				vector3f local_27;
				vector3d_from_points3d(&local_26->position, &local_24, &local_27);
				real local_28 = length_sq3f(&local_27);
				if (local_28 < local_22) { local_23 = local_25.actor_index; local_22 = local_28; }
			}
		}
		if (local_23 != NONE)
		{

			memset(&local_40.field_0, 0, sizeof(local_40.field_0));
			local_40.field_0.vehicle_index = local_20;
			local_40.field_0.seat_index = local_21;
			local_40.field_0.flag0 = false;
			local_40.field_0.flag3 = false;
			local_40.field_0.flag5 = true;
			local_40.field_0.flag6 = true;
			local_40.field_0.unknown28 = 3.402823466e38f;
			local_40.field_0.unknown2c = 3.402823466e38f;
			if (function_1a80e0(local_23, 0x4c, (s_slot *)&local_40.field_0, 1))
			{
				local_3++;
				local_4--;
			}
		}
		local_0[local_13 >> 5] |= 1 << (local_13 & 31);
	}
	return local_3;
}
