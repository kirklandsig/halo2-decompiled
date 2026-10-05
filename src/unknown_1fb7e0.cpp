// @flags /O2 /Gr
/* UNKNOWN_1FB7E0.CPP: events of an actor's unit */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"

struct s_1fb7e0_object
{
	byte unknown000[0x12c];
	long unknown12c;
};

struct s_1fb7e0_object_header
{
	byte unknown00[8];
	s_1fb7e0_object *object;
};

void __stdcall function_1fbac0(long unknown, long unit_index, bool unknown2, long unknown3, s_1fbac0_event *event);

// @retail 0x1fb7e0
bool function_1fb7e0(long actor_index, short type, s_1fb7e0_data const *data, long target_index, long unknown)
{
	bool result = false;
	long unit_index = actor_get(actor_index)->unknown018;

	if (unit_index != NONE)
	{
		if (type != NONE)
			return function_20ba60(type, unit_index, target_index, NONE, unknown, data);

		s_1fb7e0_object *unit = ((s_1fb7e0_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
		s_1fbac0_event event;

		event.unknown00 = 0;
		event.unknown02 = 1;
		event.data = *data;
		function_1fbac0(unit->unknown12c, unit_index, true, 0, &event);
		result = true;
	}

	return result;
}

/* the squads (g_51e9d8, 0x98 bytes) as function_1fb8a0 sees them */
struct s_1fb8a0_squad
{
	byte unknown00[0x76];
	char team;
	byte unknown77[0x98 - 0x77];
};

/* has the squad start an event, unless it is a greeting of a squad the
   players are enemies of */
// @retail 0x1fb8a0
bool function_1fb8a0(long squad_index, short type)
{
	bool result = false;
	s_1fb8a0_squad *squad = (s_1fb8a0_squad *)(g_51e9d8->data + (squad_index & 0xffff) * sizeof(s_1fb8a0_squad));
	bool valid = true;

	if (type == 0x77 || type == 0x78)
	{
		short team = squad->team;

		if (team == NONE || team_is_enemy(team, 1))
			valid = false;
	}
	if (valid)
		result = function_20ba60(type, NONE, NONE, squad_index & 0xffff, NONE, NULL);
	return result;
}
