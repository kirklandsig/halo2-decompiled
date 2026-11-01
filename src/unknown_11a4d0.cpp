// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_11A4D0.CPP: unit queries and flag setters of the script functions */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11a4d0.h"
#include "unknown_1dee50.h"
#include "unit_requests.h"
#include "units.h"
#include "object_iterator.h"
#include "object_default_placement.h"
#include "unknown_1c62f0.h"
#include <string.h>

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* the units (a local view of the object data) */
struct s_unit_11a4d0
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x88 - 0x18];
	vector3f vector88;
	byte unknown094[0xaa - 0x94];
	byte object_type;
	byte unknown0ab[0xd4 - 0xab];
	long index_d4;
	byte unknown0d8[0xe4 - 0xd8];
	real maximum_body_vitality;
	real maximum_shield_vitality;
	real body_vitality;
	real shield_vitality;
	byte unknownf4[0x10a - 0xf4];
	union
	{
		word flags10a;
		struct
		{
			word : 2;
			word flag10a_2 : 1;
			word : 13;
		};
	};
	byte unknown10c[0x12a - 0x10c];
	short animation_offset;
	byte unknown12c[0x134 - 0x12c];
	union
	{
		dword unit_flags;
		struct
		{
			dword : 16;
			dword unit_flag16 : 1;
			dword : 2;
			dword unit_flag19 : 1;
			dword : 12;
		};
	};
	byte unknown138[0x1fc - 0x138];
	short parent_seat_index;
	byte unknown1fe[0x212 - 0x1fe];
	char weapon_index_a;
	char weapon_index_b;
	byte unknown214[0x218 - 0x214];
	long weapon_object_indices[4];
	byte unknown228[0x2b8 - 0x228];
	real rate;
	byte unknown2bc[0x33e - 0x2bc];
	short offset33e;
	byte unknown340[0x346 - 0x340];
	short offset346;
	struct s_flags348_11a4d0
	{
		word bit0 : 1;
		word : 15;
	} flags348;
};

struct s_unit_animation_11a4d0
{
	long index0;
	byte unknown04[2];
	short index6;
	byte unknown08[0x68 - 8];
	long index68;
	byte flags6c;
	byte unknown6d[0x7c - 0x6d];
	long state_name;
};

struct s_unit_346_11a4d0
{
	byte unknown0[8];
	dword : 18;
	dword flag18 : 1;
	dword : 13;
};

struct s_object_header_11a4d0
{
	byte unknown00[8];
	s_unit_11a4d0 *object;
};

inline s_unit_11a4d0 *unit_get_11a4d0(long object_index)
{
	return ((s_object_header_11a4d0 *)g_4e0300->data)[object_index & 0xffff].object;
}

struct s_1d9240;
void function_1d9240(s_1d9240 *p, char flag, real x);

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

inline void object_set_maximum_vitality(long object_index, real maximum_body_vitality, real maximum_shield_vitality)
{
	if (object_index != NONE)
	{
		s_unit_11a4d0 *object = unit_get_11a4d0(object_index);
		if (!TEST_FIELD_BIT(object->flag10a_2))
		{
			object->maximum_body_vitality = maximum_body_vitality;
			object->maximum_shield_vitality = maximum_shield_vitality;
			object->body_vitality = maximum_body_vitality > 0.0f ? 1.0f : 0.0f;
			object->shield_vitality = maximum_shield_vitality > 0.0f ? 1.0f : 0.0f;
		}
	}
}

/* sets the vitality of every object of an object list (full where the
   maximum is above zero) */
// @retail 0x11a220
void function_11a220(long list_index, real maximum_body_vitality, real maximum_shield_vitality)
{
	long reference_index;
	long object_index = script_list_head_object(list_index, &reference_index);
	while (object_index != NONE)
	{
		object_set_maximum_vitality(object_index, maximum_body_vitality, maximum_shield_vitality);
		object_index = function_x457076(&reference_index);
	}
}

struct s_damage_owner;
extern s_damage_owner const *g_467420;
void function_d6a70(long object_index);
void function_d6800(long object_index, s_damage_owner const *owner, bool notify_parent, bool unknown);

/* sets the vitality of an object as fractions of its maximum vitality,
   depleting what drops to zero */
// @retail 0x11a320
void function_11a320(long object_index, real body_vitality, real shield_vitality)
{
	if (object_index != NONE)
	{
		s_unit_11a4d0 *object = unit_get_11a4d0(object_index);
		if (!TEST_FIELD_BIT(object->flag10a_2))
		{
			real shield;

			if (object->maximum_shield_vitality <= 0.0f)
				shield = 0.0f;
			else if (shield_vitality >= object->maximum_shield_vitality)
				shield = 1.0f;
			else
				shield = shield_vitality / object->maximum_shield_vitality;

			if (object->maximum_body_vitality <= 0.0f)
				body_vitality = 0.0f;
			else if (body_vitality >= object->maximum_body_vitality)
				body_vitality = 1.0f;
			else
				body_vitality = body_vitality / object->maximum_body_vitality;

			if (object->shield_vitality > 0.0f && shield <= 0.0f)
				function_d6a70(object_index);
			object->shield_vitality = shield;

			if (object->body_vitality > 0.0f && body_vitality <= 0.0f)
				function_d6800(object_index, g_467420, true, false);
			object->body_vitality = body_vitality;
		}
	}
}

/* sets the vitality of every object of an object list (11a320) */
// @retail 0x11a430
void function_11a430(long list_index, real body_vitality, real shield_vitality)
{
	long reference_index;
	long object_index = object_list_get_first_inlined(list_index, &reference_index);
	while (object_index != NONE)
	{
		function_11a320(object_index, body_vitality, shield_vitality);
		object_index = function_x457076(&reference_index);
	}
}

/* whether the unit holds a weapon of the given definition */
// @retail 0x11a4d0
bool function_11a4d0(long unit_index, long definition_index)
{
	bool result = false;

	if (unit_index != NONE && definition_index != NONE)
	{
		s_record_pool *globals = g_4e0300;
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		short weapon_index = unit->weapon_index_a;
		if (weapon_index != NONE)
		{
			long weapon_object_index = unit->weapon_object_indices[weapon_index];
			if (weapon_object_index != NONE && ((s_object_header_11a4d0 *)globals->data)[weapon_object_index & 0xffff].object->definition_index == definition_index)
				result = true;
		}
		weapon_index = unit->weapon_index_b;
		if (weapon_index != NONE)
		{
			long weapon_object_index = unit->weapon_object_indices[weapon_index];
			if (weapon_object_index != NONE && ((s_object_header_11a4d0 *)globals->data)[weapon_object_index & 0xffff].object->definition_index == definition_index)
				result = true;
		}
	}
	return result;
}

/* sets a flag of every unit of an object list */
// @retail 0x11a570
void function_11a570(long list_index, bool flag)
{
	long reference_index;
	long object_index = script_list_head_object(list_index, &reference_index);
	while (object_index != NONE)
	{
		s_unit_11a4d0 *unit = (s_unit_11a4d0 *)function_badc0(object_index, 3);
		if (unit)
		{
			if (flag)
				unit->unit_flag19 = true;
			else
				unit->unit_flag19 = false;
		}
		object_index = function_x457076(&reference_index);
	}
}

/* sets another flag of every unit of an object list */
// @retail 0x11a680
void function_11a680(long list_index)
{
	long reference_index;
	long object_index = script_list_head_object(list_index, &reference_index);
	while (object_index != NONE)
	{
		s_unit_11a4d0 *unit = (s_unit_11a4d0 *)function_badc0(object_index, 3);
		if (unit)
			unit->unit_flag16 = true;
		object_index = function_x457076(&reference_index);
	}
}

// @retail 0x11a770
void function_11a770(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		SET_FLAG(unit->unit_flags, 20, flag);
		unit->vector88 = *g_4687a4;
		if (unit->object_type == 0)
		{
			s_unit_11a4d0::s_flags348_11a4d0 *flags = &unit_get_11a4d0(unit_index)->flags348;
			flags->bit0 = false;
		}
	}
}

// @retail 0x11a7f0
void function_11a7f0(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		dword *flags = &unit_get_11a4d0(unit_index)->unit_flags;
		SET_FLAG(*flags, 18, !flag);
	}
}

void function_b58c0(long index, dword mask);
void function_d0e00(long unit_index, real rate);

/* starts (or ends) a timed state of the unit lasting the given seconds */
// @retail 0x11a830
void function_11a830(long unit_index, bool flag, real seconds)
{
	if (unit_index != NONE)
	{
		real minimum = g_510c54->rate;
		if (!(seconds > minimum))
			seconds = minimum;
		real rate = 1.0f / (seconds * 0.25f);
		if (flag)
		{
			s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
			unit->unit_flags |= FLAG(3);
			unit->rate = rate * 0.25f;
			long index = unit_get_11a4d0(unit_index)->index_d4;
			if (index != NONE)
				function_b58c0(index, 0x800000);
		}
		else
		{
			function_d0e00(unit_index, rate);
		}
	}
}

// @retail 0x11a8c0
void function_11a8c0(long unit_index)
{
	s_unit_request request;
	memset(&request, 0, sizeof(request));
	request.type = 0x17;
	request.type17.unknown4 = false;
	request.type17.unknown5 = false;
	function_e6900(unit_index, &request);
}

// @retail 0x11a910
void function_11a910(long unit_index)
{
	s_unit_request request;
	memset(&request, 0, sizeof(request));
	request.type = 0x18;
	function_e6900(unit_index, &request);
}

// @retail 0x11a960
bool function_11a960(long unit_index)
{
	bool result = false;
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		if (!TEST_FIELD_BIT(unit->flag10a_2))
			result = ((s_unit_346_11a4d0 *)((byte *)unit + unit->offset346))->flag18;
	}
	return result;
}

// @retail 0x11aac0
void function_11aac0(long unit_index, short ticks)
{
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		function_1d9240((s_1d9240 *)((byte *)unit + unit->offset33e + 0x88), false, (real)ticks * (1.0f / 30.0f));
	}
}

// @retail 0x11ab10
void function_11ab10(long unit_index, short ticks)
{
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		function_1d9240((s_1d9240 *)((byte *)unit + unit->offset33e + 0x88), true, (real)ticks * (1.0f / 30.0f));
	}
}

/* the seats of a unit's tag */
struct s_unit_definition_11a4d0
{
	byte unknown000[0x1c8];
	long seat_count;
	s_unit_seat_definition *seats;
};

inline s_unit_definition_11a4d0 *unit_definition_get_11a4d0(s_unit_11a4d0 *unit)
{
	return (s_unit_definition_11a4d0 *)g_4e3b44[unit->definition_index & 0xffff].bytes;
}

/* an iteration over the units: the current unit, then the object iterator */
struct s_unit_iterator_11a4d0
{
	s_unit_11a4d0 *unit;
	s_type_f1af8e iterator;
};

inline void unit_iterator_new(s_unit_iterator_11a4d0 *iterator)
{
	function_bae80(&iterator->iterator, 3, 0);
}

inline bool unit_iterator_next(s_unit_iterator_11a4d0 *iterator)
{
	iterator->unit = (s_unit_11a4d0 *)function_baeb0(&iterator->iterator);
	return iterator->unit != NULL;
}

/* the seats of an object and of the units riding it */
inline long object_get_seats(long object_index, s_object_seat *seats, short maximum_count)
{
	short count = 0;
	function_c8a40(object_index, seats, &count, maximum_count);
	return count;
}

long function_2116f0(long unit_index, long filter_range, long seat_type, long occupancy, s_object_seat *results, long maximum_count);

/* whether the unit sits in a seat (with the label, or any) of the vehicle */
// @retail 0x11ab60
bool function_11ab60(long vehicle_index, long label, long unit_index)
{
	bool result = false;
	if (vehicle_index != NONE && unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		if (function_badc0(vehicle_index, 3))
		{
			short seat_count = 0;
			s_object_seat seats[64];
			function_c8a40(vehicle_index, seats, &seat_count, 64);
			for (long i = 0; i < seat_count; i++)
			{
				if ((label == NONE || label == seats[i].definition->label) &&
					unit->parent_index == seats[i].object_index && unit->parent_seat_index == seats[i].seat_index)
				{
					result = true;
					goto done;
				}
			}
		}
	}
done:
	return result;
}

/* whether a unit of the object list sits in a seat (with the label, or any)
   of the vehicle */
// @retail 0x11ac30
bool function_11ac30(long vehicle_index, long label, long list_index)
{
	bool result = false;
	if (vehicle_index != NONE)
	{
		s_object_seat seats[64];
		long count = object_get_seats(vehicle_index, seats, 64);
		for (long i = 0; i < count; i++)
		{
			long seat_index = seats[i].seat_index;
			long object_index = seats[i].object_index;
			if (label == NONE || label == seats[i].definition->label)
			{
				s_unit_iterator_11a4d0 iterator;
				unit_iterator_new(&iterator);
				while (unit_iterator_next(&iterator))
				{
					if (iterator.unit->parent_index == object_index && iterator.unit->parent_seat_index == seat_index)
					{
						long reference_index;
						long list_object_index = script_list_head_object(list_index, &reference_index);
						while (list_object_index != NONE && iterator.iterator.object_index != list_object_index)
							list_object_index = function_x457076(&reference_index);
						if (iterator.iterator.object_index == list_object_index)
							result = true;
						break;
					}
				}
			}
		}
	}
	return result;
}

/* puts the unit into a free seat with the label of the vehicle */
// @retail 0x11ade0
void function_11ade0(long unit_index, long vehicle_index, long label)
{
	if (unit_index != NONE && vehicle_index != NONE && label)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		if (!TEST_FIELD_BIT(unit->flag10a_2))
		{
			if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
				function_e68c0(0x1e, unit_index);
			if (unit->parent_index == NONE)
			{
				s_unit_definition_11a4d0 *definition = unit_definition_get_11a4d0(unit_get_11a4d0(vehicle_index));
				for (long seat_index = 0; seat_index < definition->seat_count; seat_index++)
				{
					if (label == definition->seats[seat_index].label &&
						function_c8f60(vehicle_index, seat_index) == NONE &&
						function_c8200(unit_index, vehicle_index, seat_index))
					{
						s_unit_request request;
						request.type = 0x1c;
						request.type1c.object_index = vehicle_index;
						request.type1c.seat_index = seat_index;
						request.type1c.unknowna = false;
						request.type1c.unknownb = false;
						function_e6900(unit_index, &request);
						break;
					}
				}
			}
		}
	}
}

/* makes the unit leave its vehicle if the vehicle has a seat with the label */
// @retail 0x11af10
void function_11af10(long unit_index, long label)
{
	if (label)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		if (unit->parent_index != NONE)
		{
			s_unit_11a4d0 *parent = unit_get_11a4d0(unit->parent_index);
			if (parent->object_type == 1)
			{
				s_unit_definition_11a4d0 *definition = unit_definition_get_11a4d0(parent);
				for (long seat_index = 0; seat_index < definition->seat_count; seat_index++)
				{
					if (label == definition->seats[seat_index].label)
					{
						function_e68c0(0x1f, unit_index);
						break;
					}
				}
			}
		}
	}
}

/* makes the unit leave its seat in one of four ways */
// @retail 0x11af90
void function_11af90(long unit_index, short mode)
{
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
		{
			s_unit_request request;
			if (mode == 0)
			{
				function_e68c0(0x1d, unit_index);
			}
			else if (mode == 1)
			{
				request.type = 0x20;
				request.type20.unknown4 = false;
				function_e6900(unit_index, &request);
			}
			else if (mode == 2)
			{
				request.type = 0x20;
				request.type20.unknown4 = true;
				function_e6900(unit_index, &request);
			}
			else if (mode == 3)
			{
				request.type = 0x1d;
				request.type1d.unknown4 = true;
				function_e6900(unit_index, &request);
			}
		}
	}
}

/* puts the units of an object list into the free seats (passing a filter) of
   the vehicle; returns how many got in */
// score 2 (was 2): the two copies are made in the opposite order: retail copies esi into eax first and ebx
// into ecx, ours does it the other way round. Nested seat guards 2; unit lifetime 2; seat lifetime 2;
// const unit pointer 2; const seat pointer 2; unsigned word seat index 2; const scalar parameters 2;
// scoped favor-size 313.
// @retail 0x11b0c0
short function_11b0c0(long vehicle_index, long filter_range, long volatile list_index)
{
	long result = 0;
	if (vehicle_index != NONE && !TEST_FIELD_BIT(unit_get_11a4d0(vehicle_index)->flag10a_2))
	{
		s_object_seat seats[64];
		long seat_count = function_2116f0(vehicle_index, filter_range, 4, 2, seats, 64);
		if (seat_count > 0)
		{
			long reference_index;
#pragma inline_depth(0)
			long object_index = script_list_head_object(list_index, &reference_index);
#pragma inline_depth()
			while (object_index != NONE)
			{
				s_unit_11a4d0 *unit = unit_get_11a4d0(object_index);
				if ((1 << unit->object_type) & 3)
				{
					for (long i = 0; i < seat_count; i++)
					{
						s_object_seat *seat = &seats[i];
						if (seat->object_index != NONE && function_c8200(object_index, seat->object_index, seat->seat_index))
						{
							s_unit_request request;
							if (unit->parent_index != NONE)
								function_e68c0(0x1e, object_index);
							request.type = 0x1c;
							request.type1c.object_index = seat->object_index;
							request.type1c.seat_index = seat->seat_index;
							request.type1c.unknowna = false;
							request.type1c.unknownb = false;
							if (function_e6900(object_index, &request))
							{
								seat->object_index = NONE;
								result++;
								break;
							}
						}
					}
				}
				object_index = function_x457076(&reference_index);
			}
		}
	}
	return (short)result;
}

/* makes the units in the occupied seats (passing a filter) of the vehicle
   leave; returns how many did */
// @retail 0x11b2b0
short function_11b2b0(long vehicle_index, long filter_range)
{
	short result = 0;
	if (vehicle_index != NONE)
	{
		s_object_seat seats[64];
		long seat_count = function_2116f0(vehicle_index, filter_range, 4, 1, seats, 64);
		for (long i = 0; i < seat_count; i++)
		{
			s_object_seat *seat = &seats[i];
			if (seat->object_index != NONE)
			{
				long occupant = function_c8f60(seat->object_index, seat->seat_index);
				if (occupant != NONE)
				{
					s_unit_request request;
					memset(&request, 0, sizeof(request));
					request.type = 0x1d;
					if (function_e6900(occupant, &request))
						result++;
				}
			}
		}
	}
	return result;
}

long players_first_active_local_player(void);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);

/* the players (g_4e8c24, a local view) */
struct s_player_11a4d0
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0x21c - 0x30];
};

/* sends a request to the unit of the first local player */
// @retail 0x11b350
void function_11b350(void)
{
	long local_player_index = players_first_active_local_player();
	if (function_14ddc0(local_player_index))
	{
		long player_index = function_14de70(local_player_index);
		long unit_index = ((s_player_11a4d0 *)g_4e8c24->data)[player_index & 0xffff].unit_index;
		if (unit_index != NONE)
		{
			s_unit_request request;
			memset(&request, 0, sizeof(request));
			request.type = 0x1a;
			request.type1a.unknown4 = 0;
			function_e6900(unit_index, &request);
		}
	}
}

// @retail 0x11b3e0
void function_11b3e0(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		dword *flags = &unit_get_11a4d0(unit_index)->unit_flags;
		SET_FLAG(*flags, 11, flag);
	}
}

// @retail 0x11b420
void function_11b420(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		dword *flags = &unit_get_11a4d0(unit_index)->unit_flags;
		SET_FLAG(*flags, 12, !flag);
	}
}

// @retail 0x11b460
void function_11b460(long unit_index, bool flag)
{
	if (unit_index != NONE)
	{
		dword *flags = &unit_get_11a4d0(unit_index)->unit_flags;
		SET_FLAG(*flags, 31, flag);
	}
}

/* the object globals (a local view) */
struct s_object_globals_11a4d0
{
	byte unknown00[0x80];
	bool flag80;
};

/* the unit's scripted animation (at the unit's offset +0x33e) */
struct s_unit_scripted_animation_11a4d0
{
	union
	{
		byte flags;
		struct
		{
			word : 4;
			word active : 1;
			word : 3;
			word attached : 1;
			word : 7;
		};
	};
	byte unknown02[0x28 - 2];
	long attached_object_index;
};

/* the unit flags at +4 (a local view) */
struct s_unit_flags_11a4d0
{
	byte unknown000[4];
	dword : 1;
	dword flag1 : 1;
	dword : 30;
};

bool function_1101e0(long animation_graph_index, long unit_index, long animation_name, bool flag, bool global_flag);
void function_ba350(long object_index, real seconds);
void function_ba3d0(long unit_index);

/* plays a scripted animation (by graph and name) on a unit, optionally
   relative to another object */
// @retail 0x11b520
bool function_11b520(long unit_index, long animation_graph_index, long animation_name, bool interpolate,
	long attached_object_index, bool flag)
{
	bool result = false;

	if (unit_index != NONE && animation_graph_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		bool global_flag = ((s_object_globals_11a4d0 *)g_4de2f4)->flag80;
		if (function_1101e0(animation_graph_index, unit_index, animation_name, flag, global_flag))
		{
			struct s_flags_view
			{
				byte unknown000[4];
				dword flags;
			} *flags = (s_flags_view *)unit_get_11a4d0(unit_index);
			s_unit_scripted_animation_11a4d0 *animation =
				(s_unit_scripted_animation_11a4d0 *)((byte *)unit + unit->offset33e);
			if (!(bool)((*(dword volatile *)&flags->flags >> 1) & 1))
			{
				flags->flags |= 2;
				function_b7360(unit_index);
				function_b7290(unit_index);
			}
			if (interpolate)
				function_ba350(unit_index, 0.267f);
			else
				function_ba3d0(unit_index);
			*(word *)&animation->flags |= 0x11;
			animation->attached_object_index = attached_object_index;
			if (attached_object_index != NONE)
			{
				function_b9a50(unit_index);
				/* the flags word at +0xc0: a word bitfield, which retail tests
				   with a byte load and shift */
				struct s_c0_view
				{
					byte unknown000[0xc0];
					word pad0 : 6;
					word flag_c0_6 : 1;
					word pad1 : 9;
				};
				if (TEST_FIELD_BIT(((s_c0_view *)unit)->flag_c0_6))
					function_b8840(unit_index);
				function_b75a0(unit_index, g_468788, g_4687a8, g_4687b0, NULL, false);
				function_b77d0(unit_index, g_4687a4, g_4687a4);
				function_b7360(unit_index);

				s_object_default_placement_view *placed = object_default_placement_get(unit_index);
				placed->hidden = false;
				if (!TEST_FIELD_BIT(placed->flag8) && g_4de2f4 && *(byte *)g_4de2f4)
					function_b8600(unit_index, 0);
				function_b7290(unit_index);
			}
			function_b9b90(unit_index, true);
			function_bd020(unit_index);
			result = true;
		}
	}

	return result;
}

/* the same for every unit of an object list; false when one fails */
// @retail 0x11a9a0
bool function_11a9a0(long list_index, long animation_graph_index, long animation_name, bool interpolate)
{
	bool result = true;
	long reference_index;
	long object_index = object_list_get_first_inlined(list_index, &reference_index);

	while (object_index != NONE)
	{
		if (function_badc0(object_index, 3))
			result = result && function_11b520(object_index, animation_graph_index, animation_name, interpolate, NONE, false);
		object_index = function_x457076(&reference_index);
	}

	return result;
}

/* plays a scripted animation on a unit from a frame */
// @retail 0x11b4a0
bool function_11b4a0(long unit_index, long animation_graph_index, long animation_name, bool interpolate, short frame)
{
	bool result = false;

	if (function_11b520(unit_index, animation_graph_index, animation_name, interpolate, NONE, false))
	{
		real seconds = (real)frame * (1.0f / 30.0f);
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		s_unit_animation_11a4d0 *animation = (s_unit_animation_11a4d0 *)((byte *)unit + unit->animation_offset);
		bool valid = animation->index68 != NONE && animation->index0 != NONE && animation->index6 != NONE;
		if (valid)
			((c_animation_channel *)animation)->set_frame_position(seconds * 30.0f);
		result = true;
	}

	return result;
}

/* the object flags at +0xc0 (a local view) */
struct s_unit_c0_11a4d0
{
	byte unknown000[0xc0];
	byte : 7;
	byte flag_c0_7 : 1;
};

void function_10f1e0(long unit_index);
bool __stdcall function_10f430(long unit_index, long field_7c, long state_name, long weapon_name, long action_name,
	real blend, bool flags, long mode);
bool function_10fd40(long unit_index, long action_name, long state_name, bool flag);
void __stdcall function_b8890(long unit_index);
void *render_model_get_model_definition(long render_model_index);
transform4x3f *function_b8bd0(long object_index, short node_index);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
bool function_cd660(long unit_index);

/* the unit's flags word at +0xc0, seen as bits */
struct s_unit_flags_c0_11b710
{
	byte unknown000[0xc0];
	word : 7;
	word flag7 : 1;
};

/* ends a unit's scripted animation: back to its weapons' idle states, and
   placed at its root node when the animation was attached */
// @retail 0x11b710
void function_11b710(long unit_index, long field_7c)
{
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		s_unit_flags_c0_11b710 *object = (s_unit_flags_c0_11b710 *)unit_get_11a4d0(unit_index);
		s_unit_scripted_animation_11a4d0 *animation =
			(s_unit_scripted_animation_11a4d0 *)((byte *)unit + unit->offset33e);
		if (TEST_FIELD_BIT(animation->active))
		{
			long attached_object_index = animation->attached_object_index;
			bool attached = TEST_FIELD_BIT(animation->attached);
			animation->active = false;
			animation->attached = false;
			animation->attached_object_index = NONE;

			s_unit_flags_11a4d0 *flags = (s_unit_flags_11a4d0 *)unit_get_11a4d0(unit_index);
			if (TEST_FIELD_BIT(flags->flag1))
				((s_unit_flags_11a4d0 *)unit_get_11a4d0(unit_index))->flag1 = false;
			function_10f1e0(unit_index);
			if (field_7c == 0x7000101)
				field_7c = 0x6000085;

			bool dual_wielding = function_cd660(unit_index);
			long state_name = 0x7000101;
			long weapon_name = 0x7000101;
			if (unit->weapon_index_a != NONE)
			{
				long weapon_index = unit->weapon_object_indices[unit->weapon_index_a];
				if (weapon_index != NONE)
				{
					byte *definition = g_4e3b44[unit_get_11a4d0(weapon_index)->definition_index & 0xffff].bytes;
					state_name = dual_wielding ? 0x400054b : *(long *)(definition + 0x288);
					weapon_name = *(long *)(definition + 0x28c);
				}
			}
			s_unit_animation_11a4d0 *animation_state = (s_unit_animation_11a4d0 *)((byte *)unit + unit->animation_offset);
			animation_state->flags6c &= ~1;
			function_10f430(unit_index, field_7c, state_name, weapon_name, 0x400000c, 0.0f, true, 2);

			if (unit->weapon_index_b != NONE)
			{
				long weapon_index = unit->weapon_object_indices[unit->weapon_index_b];
				if (weapon_index != NONE)
				{
					byte *definition = g_4e3b44[unit_get_11a4d0(weapon_index)->definition_index & 0xffff].bytes;
					long secondary_state_name = dual_wielding ? 0x400054b : *(long *)(definition + 0x288);
					function_10fd40(unit_index, *(long *)(definition + 0x28c), secondary_state_name, false);
				}
			}

			if (attached && attached_object_index != NONE)
			{
				transform4x3f matrix;
				function_ba3d0(unit_index);
				byte *model = (byte *)render_model_get_model_definition(
					*(long *)(g_4e3b44[unit->definition_index & 0xffff].bytes + 0x38));
				function_142a60(function_b8bd0(unit_index, 0), (transform4x3f *)(*(byte **)(model + 0x4c) + 0x28), &matrix);
				object->flag7 = false;
				if (unit->parent_index == NONE)
				{
					function_b75a0(unit_index, &matrix.position, &matrix.forward, &matrix.up, NULL, false);
					function_b77d0(unit_index, g_4687a4, g_4687a4);
					function_b8890(unit_index);
				}
			}
		}
	}
}

real function_10f690(long object_index, real *duration);

/* the seconds left of the unit's animation in state 0xe0000c2 */
// @retail 0x11b6b0
real function_11b6b0(long unit_index)
{
	real result = 0.0f;
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		s_unit_animation_11a4d0 *animation = (s_unit_animation_11a4d0 *)((byte *)unit + unit->animation_offset);
		if (animation->index68 != NONE && animation->index0 != NONE && animation->index6 != NONE && animation->state_name == 0xe0000c2)
		{
			real duration;
			real elapsed = function_10f690(unit_index, &duration);
			result = duration - elapsed;
		}
	}
	return result;
}

/* whether the unit's current animation state is 0xe0000c2 */
// @retail 0x11b930
bool function_11b930(long unit_index)
{
	bool result = false;
	if (unit_index != NONE)
	{
		s_unit_11a4d0 *unit = unit_get_11a4d0(unit_index);
		s_unit_animation_11a4d0 *animation = (s_unit_animation_11a4d0 *)((byte *)unit + unit->animation_offset);
		long state_name = NONE;
		if (animation->index68 != NONE && animation->index0 != NONE && animation->index6 != NONE)
			state_name = animation->state_name;
		result = state_name == 0xe0000c2;
	}
	return result;
}
