// @flags /O2 /Gr
/* UNKNOWN_1E77C0.CPP: recording a unit request in its player's local state
   (the e6900 callee) */

#include "unknown_11c920.h"
#include "globals.h"
#include "game_loop.h"
#include <string.h>
#include <xmmintrin.h>

/* a player (g_4e8c24, 0x21c bytes): +0x28 is its local player slot */
struct s_player_request_view
{
	byte unknown00[0x28];
	short local_player_index;
	byte unknown2a[2];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

struct s_time_entry
{
	long time;
	short a;
	short b;
};

struct s_request_transition
{
	short state;
	short ticks;
};

/* a local player's state in g_51e9c0 (src/player_request_state.cpp), 0x1b0 bytes:
   the four last unit requests at +0x150 */
struct s_local_player_state_view
{
	s_request_transition transitions[32];
	long current_request;
	long elapsed_ticks;
	byte unknown088[0x150 - 0x88];
	s_time_entry requests[4];
	dword request_flags[8];
	long version;
	byte field_194;
	byte field_195;
	byte field_196;
	byte field_197;
	byte timer198;
	byte timer199;
	byte timer19a;
	byte field_19b;
	byte unknown19c[0x1b0 - 0x19c];
};

struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;

void function_1e6980(s_time_entry *entries, short a, byte b);
bool function_1e7ab0(long local_index, long type);
bool function_1e83c0(long local_index, long type);

// @retail 0x1e8f60
void function_1e8f60(long player_index)
{
	s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long index = player->local_player_index;
	if (index != NONE)
		((s_local_player_state_view *)g_51e9c0)[index].field_195 = (byte)g_510c54->field_2_3;
}

// @retail 0x1e9070
void function_1e9070(long player_index)
{
	s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long index = player->local_player_index;
	if (index != NONE)
		((s_local_player_state_view *)g_51e9c0)[index].field_19b = (byte)g_510c54->field_2_3;
}

struct s_request_screen_bounds
{
	short top, left, bottom, right;
};



PRIVATE inline long next_request_player(long current)
{
	long result = NONE;
	long start;
	if (current == NONE)
		start = 0;
	else
		start = current + 1;
	for (long index = start; index < 4; index++)
	{
		if (g_4e8c20->entries[index] != NONE)
		{
			result = index;
			break;
		}
	}
	return result;
}

PRIVATE inline void set_request_flag(long local_index, byte flag)
{
	long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
	if (player_index != NONE)
	{
		s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
		long index = player->local_player_index;
		if (index != NONE)
		{
			s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[index];
			byte flags = state->field_194;
			state->field_194 = flags | flag;
		}
	}
}

// @retail 0x1e7800
void function_1e7800(void)
{
	for (long index = next_request_player(NONE); index != NONE; index = next_request_player(index))
		set_request_flag(index, 1);
}

// @retail 0x1e78b0
void function_1e78b0(void)
{
	for (long index = next_request_player(NONE); index != NONE; index = next_request_player(index))
		set_request_flag(index, 2);
}

// @retail 0x1e7960
void function_1e7960(void)
{
	for (long index = next_request_player(NONE); index != NONE; index = next_request_player(index))
		set_request_flag(index, 4);
}

struct s_entry_420
{
	long value;
	word field_04;
	word limit;
	word field_08;
	short duration;
	byte field_0c[8];
};

struct s_table_420
{
	byte field_00[0x420];
	long count;
	s_entry_420 *entries;
};

inline s_entry_420 *entry_420_get(long index)
{
	s_entry_420 *result = NULL;
	s_table_420 *table = (s_table_420 *)g_510c94;
	if (table && index >= 0 && index < table->count)
		result = &table->entries[index];
	return result;
}

// @retail 0x1e8cb0
long function_1e8cb0(long index)
{
	long result = 0;
	s_entry_420 *entry = NULL;
	s_table_420 *table = (s_table_420 *)g_510c94;
	if (table && index >= 0 && index < table->count)
	{
		/* the table is read a second time for the lookup (retail does; with one read the index lands in another register) */
		s_table_420 *entry_table = (s_table_420 *)g_510c94;
		entry = &entry_table->entries[index];
	}
	if (entry)
		result = entry->value;
	return result;
}

// @retail 0x1e8d50
s_entry_420 *function_1e8d50(long index)
{
	s_entry_420 *result = NULL;
	s_table_420 *table = (s_table_420 *)g_510c94;
	if (table && index >= 0 && index < table->count)
	{
		/* the table is read a second time for the lookup (retail does; with one read the index lands in another register) */
		s_table_420 *entry_table = (s_table_420 *)g_510c94;
		result = &entry_table->entries[index];
	}
	return result;
}

// @retail 0x1e77c0
void function_1e77c0(long player_index, long type, byte result)
{
	s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

	if (player->local_player_index != NONE)
		function_1e6980(((s_local_player_state_view *)g_51e9c0)[player->local_player_index].requests, (short)type, result);
}

// @retail 0x1e69d0
bool function_1e69d0(s_time_entry *entries, long type, long age_limit)
{
	long now = g_510c54->game_time;
	bool result = false;
	long i = 0;
	do
	{
		if ((word)entries[i].a == type && now - entries[i].time < age_limit)
		{
			result = true;
			break;
		}
		i++;
	} while (i < 4);
	return result;
}

struct s_flag_pair
{
	dword low;
	dword high;
};

// @retail 0x1e6a00
void function_1e6a00(long value, s_flag_pair *flags, long index)
{
	bool high = (bool)(((dword)value >> 1) & 1);
	if (value & 1)
		flags->low |= 1 << index;
	else
		flags->low &= ~(1 << index);
	if (high)
		flags->high |= 1 << index;
	else
		flags->high &= ~(1 << index);
}

// @retail 0x1e7a10
bool function_1e7a10(long player, long index)
{
	bool result = false;
	if (player != NONE)
	{
		s_flag_pair *flags = (s_flag_pair *)((byte *)g_51e9c0 + player * 0x1b0 + 0x170);
		long value = ((flags->high & (1 << index)) ? 2 : 0) + ((flags->low & (1 << index)) ? 1 : 0);
		result = value == 3;
	}
	return result;
}

// @retail 0x1e7a60
long function_1e7a60(long player, long index)
{
	long result = 0;
	if (player != NONE)
	{
		s_flag_pair *flags = (s_flag_pair *)((byte *)g_51e9c0 + player * 0x1b0 + 0x170);
		result = ((flags->high & (1 << index)) ? 2 : 0) + ((flags->low & (1 << index)) ? 1 : 0);
	}
	return result;
}

struct s_request_source
{
	byte field_00[0x1c];
	dword flags;
	byte field_20;
	byte field_21[7];
	short type;
	byte value;
	byte field_2b;
	long id;
	byte field_30[0x2c];
};

struct s_request_state
{
	word type;
	word field_02;
	long id;
	short count;
	short field_0a;
	short value;
};

// @retail 0x1e8ce0
void function_1e8ce0(s_request_state *state, s_request_source *source)
{
	short type = source->type;
	long stored_type = state->type;
	long const &incoming_type = (long)type;
	if (incoming_type != stored_type || source->id != state->id)
	{
		state->type = type;
		state->id = source->id;
		state->value = source->value;
		state->count = 0;
		state->field_0a = 0;
	}
	if (state->type == 1 || state->type == 3 || state->type == 4 || state->type == 5 || state->type == 6)
	{
		if (source->flags & 8)
			state->count++;
	}
}

struct s_request_object
{
	byte field_000[0xaa];
	byte type;
	byte field_0ab[0x138 - 0xab];
	short team;
	byte field_13a[0x1fc - 0x13a];
	short field_1fc;
	byte field_1fe[0x216 - 0x1fe];
	byte field_216;
};

struct s_request_object_header
{
	byte field_00[8];
	s_request_object *object;
};

bool function_1df560(short team_a, short team_b);

// @retail 0x1e6b00
void function_1e6b00(long player_index, s_request_source *source)
{
	s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long index = player->local_player_index;
	if (index != NONE)
	{
		s_request_source *current = (s_request_source *)((byte *)&((s_local_player_state_view *)g_51e9c0)[index] + 0x88);
		*(s_request_source *)((byte *)&((s_local_player_state_view *)g_51e9c0)[index] + 0xe4) = *current;
		*current = *source;
		if (player->unit_index != NONE)
		{
			s_request_object *unit = ((s_request_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
			if (unit->field_1fc == NONE && source->field_20 != 0xff && source->field_20 != unit->field_216)
			{
				((s_local_player_state_view *)g_51e9c0)[index].field_197++;
				((s_local_player_state_view *)g_51e9c0)[index].field_196 = (byte)(g_510c54->field_2_3 * 2);
			}
		}
		function_1e8ce0((s_request_state *)((byte *)&((s_local_player_state_view *)g_51e9c0)[index] + 0x140), source);
	}
}

// @retail 0x1e8d80
bool __stdcall function_1e8d80(long index, s_local_player_state_view *state, s_entry_420 *entry, s_request_transition *output)
{
	bool completed = false;
	s_flag_pair *flags = (s_flag_pair *)((byte *)state + 0x170);
	long value = ((flags->high & (1 << index)) ? 2 : 0) + ((state->request_flags[0] & (1 << index)) ? 1 : 0);
	if (index == 21)
		function_1e8d80(22, state, entry_420_get(22), (s_request_transition *)((byte *)state + 0x58));
	if (!entry)
	{
		output->state = 4;
		output->ticks = g_510c54->field_2_3 * 5;
	}
	else if (entry->limit != 0 && entry->limit <= 3)
	{
		if (value + 1 >= entry->limit)
		{
			completed = true;
			output->state = 5;
			output->ticks = 0;
			function_1e6a00(3, flags, index);
		}
		else
		{
			output->state = 4;
			output->ticks = g_510c54->field_2_3 * entry->duration;
			function_1e6a00(value + 1, flags, index);
		}
	}
	else
	{
		output->state = 4;
		output->ticks = g_510c54->field_2_3 * entry->duration;
	}
	return completed;
}

// @retail 0x1e8fa0
void function_1e8fa0(long player_index, long object_index, byte kind)
{
	s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	s_request_object *object = ((s_request_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	long local_index = player->local_player_index;
	if (local_index != NONE && ((1 << object->type) & 3) && function_1df560(player->team, object->team))
	{
		s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
		byte type = kind & 0x3f;
		if (type == 0x15 || type == 0x16)
			state->timer198 = (byte)g_510c54->field_2_3;
		else if (type == 3)
			state->timer199 = (byte)g_510c54->field_2_3;
		else if ((type >= 5 && type <= 0x13) || (type >= 0x27 && type <= 0x28))
			state->timer19a = (byte)g_510c54->field_2_3;
	}
}

struct s_parent_seat_object
{
	long definition_index;
	byte field_4[0x14 - 4];
	long parent_index;
	byte field_18[0x1fc - 0x18];
	short seat_index;
};

struct s_parent_seat_header
{
	short salt;
	byte flags;
	byte type;
	byte field_4[4];
	s_parent_seat_object *object;
};

struct s_parent_seat
{
	dword : 2;
	dword enabled : 1;
	dword : 29;
	byte field_4[0xb0 - 4];
};

struct s_parent_seat_definition
{
	byte field_0[0x1cc];
	s_parent_seat *seats;
	byte field_1d0[0x21a - 0x1d0];
	short type;
};

// @retail 0x1e8eb0
bool function_1e8eb0(long object_index, long type, bool any_nonzero)
{
	s_parent_seat_header *headers = (s_parent_seat_header *)g_4e0300->data;
	s_parent_seat_object *object = headers[object_index & 0xffff].object;
	bool result = false;
	const long *type_reference = &type;
	const bool *any_reference = &any_nonzero;
	if (object->parent_index != NONE && object->seat_index != NONE)
	{
		short seat_index = object->seat_index;
		s_parent_seat_header *parent = &headers[object->parent_index & 0xffff];
		if ((1 << parent->type) & 2)
		{
			s_parent_seat_definition *definition = (s_parent_seat_definition *)g_4e3b44[parent->object->definition_index & 0xffff].bytes;
			if (TEST_FIELD_BIT(definition->seats[seat_index].enabled))
			{
				if (definition->type == *type_reference || (*any_reference && definition->type != 0))
					result = true;
			}
		}
	}
	return result;
}

struct s_request_profile
{
	__int64 field_0[0x128 / 8];
	dword flags[8];
	byte field_148[0x1e0 - 0x148];
};

/* copies each requesting player's profile flags into its local state and resets the 32 transitions */
// @retail 0x1e73b0
void function_1e73b0()
{
	for (long local_index = next_request_player(NONE); local_index != NONE;
		local_index = next_request_player(local_index))
	{
		long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
		if (player_index != NONE)
		{
			s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
			if (*(long *)((byte *)player + 0x24) != NONE)
			{
				s_request_profile profile;
				long version;
				s_player_slot *slot = &g_54e8e0[*(long *)((byte *)player + 0x24)];
				if (slot && (*(byte *)slot & 0x10))
				{
					profile = *(s_request_profile *)((byte *)slot + 0x18);
					version = *(long *)((byte *)slot + 0x1f8);
				}
				else
				{
					memset(&profile, 0, sizeof(profile));
					version = NONE;
				}
				memcpy(((s_local_player_state_view *)g_51e9c0)[local_index].request_flags, profile.flags, sizeof(profile.flags));
				((s_local_player_state_view *)g_51e9c0)[local_index].version = version;
				for (long i = 0; i < 32; i += 4)
				{
					((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i].ticks = 0;
					if (function_1e7a10(local_index, i))
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i].state = 5;
					else
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i].state = 0;
					((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 1].ticks = 0;
					if (function_1e7a10(local_index, i + 1))
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 1].state = 5;
					else
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 1].state = 0;
					((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 2].ticks = 0;
					if (function_1e7a10(local_index, i + 2))
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 2].state = 5;
					else
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 2].state = 0;
					((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 3].ticks = 0;
					if (function_1e7a10(local_index, i + 3))
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 3].state = 5;
					else
						((s_local_player_state_view *)g_51e9c0)[local_index].transitions[i + 3].state = 0;
				}
			}
		}
	}
}

#include "unknown_1248b0.h"
#include <math.h>

struct s_weapon_status_magazine
{
 bool active;
 bool idle;
 short loaded;
 short loaded_maximum;
 short unloaded;
 short total_maximum;
};

struct s_weapon_status
{
 real value;
 real heat;
 bool flag8;
 bool flag9;
 byte unknown0a[2];
 real fraction;
 bool charging;
 bool target_available;
 bool target_charging;
 bool target_ready;
 real target_fraction;
 point3f target_position;
 short magazine_count;
 s_weapon_status_magazine magazines[2];
};

void function_100520(long weapon_index, s_weapon_status *status);
point3f *function_b9dd0(long object_index, point3f *result);
short function_c8860(long unit_index);
long function_cbd50(long unit_index, short weapon_index);
bool function_f42f0(long vehicle_index);
bool function_15eaf0(void);

PRIVATE inline real request_angle_delta(real angle, real previous)
{
 real delta = angle - previous;
 if (delta > 3.1415927410125732f) delta -= 6.2831854820251465f;
 if (delta < -3.1415927410125732f) delta += 6.2831854820251465f;
 return delta;
}

bool function_13cb40(void);
bool motion_sensor_enemy_vehicle_ahead(long local_player_index);
bool motion_sensor_enemy_nearby(long local_player_index);
long function_162fd0(long index);
bool voice_port_can_talk(long port);
bool function_100f00(long weapon_index);
bool function_100fd0(long weapon_index);
short function_101010(long weapon_index, short index);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_unit_child_iterator
{
 long object_index;
 long unit_index;
 short seat_index;
 long next_index;
};
struct s_damage_object;
void function_d0590(s_unit_child_iterator *iterator, long object_index);
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);

PRIVATE inline byte *request_unit(long object_index)
{
 return *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
}

PRIVATE inline long request_weapon(s_player_request_view *player, bool secondary)
{
 long unit_index = player->unit_index;
 byte *unit = request_unit(unit_index);
 return function_cbd50(unit_index, *(signed char *)(unit + (secondary ? 0x213 : 0x212)));
}

long function_1469f0(real seconds);
// @retail 0x1e6bd0
void function_1e6bd0(long player_index)
{
 long local_index = ((s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c))->local_player_index;
 s_game_options_view *options = g_4e6948;
 bool restricted = options->state == _game_state_campaign &&
  (game_options_difficulty(options) == 3 || game_options_difficulty(options) == 2);
 if (local_index != NONE && *((byte *)g_51e9c0 + 0x6c5) && !restricted)
 {
  s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
  long selected = state->current_request;
  if (state->field_195 > 0) --state->field_195;
  if (state->field_196 > 0 && --state->field_196 == 0) state->field_197 = 0;
  if (state->timer198 > 0) --state->timer198;
  if (state->timer199 > 0) --state->timer199;
  if (state->timer19a > 0) --state->timer19a;
  if (state->field_19b > 0) --state->field_19b;
  for (long i = 0; i < 32; ++i)
  {
   s_request_transition *transition = &state->transitions[i];
   switch ((word)transition->state)
   {
   case 0:
   {
    if (selected == NONE || selected > i || i == 31)
     if (function_1e7ab0(local_index, i)) selected = i;
    break;
   }
   case 4:
   {
    if (--transition->ticks <= 0) { transition->ticks = 0; transition->state = 0; }
    break;
   }
   }
  }
  if (selected != NONE)
  {
   s_entry_420 *entry = entry_420_get(selected);
   long old_current = state->current_request;
   s_request_transition *transition = &state->transitions[selected];
   if (selected != old_current)
   {
    if (old_current != NONE)
    {
     state->transitions[old_current].ticks = 0;
     state->transitions[old_current].state = 0;
    }
    state->current_request = selected;
    state->elapsed_ticks = 0;
   }
   switch ((word)transition->state)
   {
   case 0:
    transition->state = 1;
    transition->ticks = entry ? function_1469f0(*(real *)entry->field_0c) : 0;
    break;
   case 1:
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     state->current_request = NONE;
    }
    else if (function_1e7ab0(local_index, selected))
    {
     if (--transition->ticks <= 0) { transition->state = 2; transition->ticks = 0; }
    }
    else
    {
     transition->state = 0;
     transition->ticks = 0;
     state->current_request = NONE;
    }
    break;
   case 2:
    ++state->elapsed_ticks;
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     state->current_request = NONE;
     state->elapsed_ticks = 0;
    }
    else if (!function_1e7ab0(local_index, selected))
    {
     transition->state = 3;
     transition->ticks = entry ? g_510c54->field_2_3 * entry->field_08 : 0;
    }
    break;
   case 3:
    ++state->elapsed_ticks;
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     state->current_request = NONE;
     state->elapsed_ticks = 0;
    }
    else if (function_1e7ab0(local_index, selected))
    {
     transition->state = 2;
     transition->ticks = 0;
    }
    else if (--transition->ticks <= 0)
    {
     transition->ticks = 0;
     transition->state = 0;
     state->current_request = NONE;
     state->elapsed_ticks = 0;
    }
    break;
   case 4:
    if (--transition->ticks <= 0) { transition->ticks = 0; transition->state = 0; }
    break;
   }
  }
  for (long i = 0; i < 32; ++i)
  {
   if (i != state->current_request)
   {
    if (!function_1e7a10(local_index, i))
    {
     switch (i)
     {
     case 15: case 17: case 18: case 23: case 30:
      if (function_1e83c0(local_index, i))
       function_1e8d80(i, state, entry_420_get(i), &state->transitions[i]);
      break;
     }
    }
   }
  }
 }
}

struct s_player_profile;
struct s_player_profile_settings;
void __stdcall player_slot_change_profile(long index, s_player_profile *settings, long profile_index);
void __stdcall player_slot_store_profile(long index, s_player_profile_settings *settings, long profile_index);

struct __declspec(align(8)) s_request_settings
{
	byte values[0x1e0];
};

struct s_request_profile_slot
{
	dword flags;
	byte field_4[0x18 - 4];
	s_request_settings settings;
	long profile_index;
	byte field_1fc[0xc70 - 0x1fc];
};

// Retail realigns the stack before the aligned request settings.
// @retail 0x1e75d0 standard
void __stdcall function_1e75d0(dword flush)
{
	__declspec(align(8)) s_request_settings settings;
	for (long index = next_request_player(NONE); index != NONE; index = next_request_player(index))
	{
		long player_index = index == NONE ? NONE : g_4e8c20->entries[index];
		if (player_index != NONE)
		{
			s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
			long slot_index = *(long *)((byte *)player + 0x24);
			if (slot_index != NONE)
			{
				s_request_profile_slot *slot = (s_request_profile_slot *)g_54e8e0 + slot_index;
				long profile_index;
				if (slot && (slot->flags & 0x10))
				{
					settings = slot->settings;
					profile_index = slot->profile_index;
				}
				else
				{
					memset(&settings, 0, sizeof(settings));
					profile_index = NONE;
				}
				if (profile_index != NONE)
				{
					s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[index];
					if (state->version == profile_index)
					{
						bool changed = false;
						int comparison = memcmp(settings.values + 0x128, state->request_flags, sizeof(state->request_flags));
						if (comparison != 0)
						{
							memcpy(settings.values + 0x128, state->request_flags, sizeof(state->request_flags));
							*((byte *)state + 0x19d) = 1;
							changed = true;
						}
						if (!flush)
						{
							if (changed)
								player_slot_change_profile(*(long *)((byte *)player + 0x24), (s_player_profile *)&settings, profile_index);
						}
						else if (*((byte *)state + 0x19d))
						{
							player_slot_store_profile(*(long *)((byte *)player + 0x24), (s_player_profile_settings *)&settings, profile_index);
							*((byte *)g_51e9c0 + index * 0x1b0 + 0x19d) = 0;
						}
					}
				}
			}
		}
	}
}
