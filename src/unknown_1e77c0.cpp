// @flags /O2 /Gr
/* UNKNOWN_1E77C0.CPP: recording a unit request in its player's local state
   (the e6900 callee) */

#include "unknown_11c920.h"
#include "globals.h"
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

/* a local player's state in g_51e9c0 (src/unknown_1e6a40.cpp), 0x1b0 bytes:
   the four last unit requests at +0x150 */
struct s_local_player_state_view
{
	s_request_transition transitions[32];
	byte unknown080[0x150 - 0x80];
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
	long index = current;
	if (index == NONE)
		index = 0;
	else
		index++;
	for (; index < 4; index++)
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
	s_entry_420 *entry = entry_420_get(index);
	if (entry)
		result = entry->value;
	return result;
}

// @retail 0x1e8d50
s_entry_420 *function_1e8d50(long index)
{
	return entry_420_get(index);
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
	if (stored_type != type || source->id != state->id)
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
		s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[index];
		s_request_source *current = (s_request_source *)((byte *)state + 0x88);
		*(s_request_source *)((byte *)state + 0xe4) = *current;
		*current = *source;
		if (player->unit_index != NONE)
		{
			s_request_object *unit = ((s_request_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
			if (unit->field_1fc == NONE && source->field_20 != 0xff && source->field_20 != unit->field_216)
			{
				state->field_197++;
				state->field_196 = (byte)(g_510c54->field_2_3 * 2);
			}
		}
		function_1e8ce0((s_request_state *)((byte *)state + 0x140), source);
	}
}

// @retail 0x1e8d80
bool __stdcall function_1e8d80(long index, s_local_player_state_view *state, s_entry_420 *entry, s_request_transition *output)
{
	s_flag_pair *flags = (s_flag_pair *)((byte *)state + 0x170);
	long value = ((flags->high & (1 << index)) ? 2 : 0) + ((flags->low & (1 << index)) ? 1 : 0);
	if (index == 21)
		function_1e8d80(22, state, entry_420_get(22), (s_request_transition *)((byte *)state + 0x58));
	if (!entry)
	{
		output->state = 4;
		output->ticks = g_510c54->field_2_3 * 5;
		return false;
	}
	if (entry->limit != 0 && entry->limit <= 3)
	{
		if (value + 1 >= entry->limit)
		{
			output->state = 5;
			output->ticks = 0;
			function_1e6a00(3, flags, index);
			return true;
		}
		output->state = 4;
		output->ticks = g_510c54->field_2_3 * entry->duration;
		function_1e6a00(value + 1, flags, index);
		return false;
	}
	output->state = 4;
	output->ticks = g_510c54->field_2_3 * entry->duration;
	return false;
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

// @retail 0x1e73b0
void function_1e73b0()
{
	for (long local_index = next_request_player(NONE); local_index != NONE;
		local_index = next_request_player(local_index))
	{
		long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
		if (player_index != NONE)
		{
			s_player_request_view *player = (s_player_request_view *)(*(byte *volatile *)&g_4e8c24->data + (player_index & 0xffff) * 0x21c);
			long profile_index = *(long *)((byte *)player + 0x24);
			if (profile_index != NONE)
			{
				s_request_profile profile;
				long version;
				s_player_slot *slot = &g_54e8e0[profile_index];
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
				s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
				memcpy(state->request_flags, profile.flags, sizeof(profile.flags));
				state->version = version;
				dword flags0 = state->request_flags[0];
                dword flags1 = state->request_flags[1];
                for (long i = 2; i - 2 < 32; i += 4)
				{
					{
						s_request_transition *transition = &state->transitions[(i - 2)];
						transition->ticks = 0;
						long value = ((flags1 & (1 << (i - 2))) ? 2 : 0) + ((flags0 & (1 << (i - 2))) ? 1 : 0);
						transition->state = value == 3 ? 5 : 0;
					}
					{
						s_request_transition *transition = &state->transitions[(i - 1)];
						transition->ticks = 0;
						long value = ((flags1 & (1 << (i - 1))) ? 2 : 0) + ((flags0 & (1 << (i - 1))) ? 1 : 0);
						transition->state = value == 3 ? 5 : 0;
					}
					{
						s_request_transition *transition = &state->transitions[i];
						transition->ticks = 0;
						long value = ((flags1 & (1 << i)) ? 2 : 0) + ((flags0 & (1 << i)) ? 1 : 0);
						transition->state = value == 3 ? 5 : 0;
					}
					{
						s_request_transition *transition = &state->transitions[(i + 1)];
						transition->ticks = 0;
						long value = ((flags1 & (1 << (i + 1))) ? 2 : 0) + ((flags0 & (1 << (i + 1))) ? 1 : 0);
						transition->state = value == 3 ? 5 : 0;
					}
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

// @retail 0x1e83c0
bool function_1e83c0(long local_index, long type)
{
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 volatile bool result = false;
 long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
 if (player_index != NONE)
 {
  s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
   switch (type)
   {
   case 10:
    if (*(long *)(data + 0x80) == 10)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      volatile real yaw = request_angle_delta(*(real *)(data + 0x8c), *(real *)(data + 0x1a4));
      volatile real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 11:
    if (*(long *)(data + 0x80) == 11)
    {
     if (!data[0x1a0])
     {
      function_b9dd0(unit_index, (point3f *)(data + 0x1a4));
      data[0x1a0] = true;
     }
     else
     {
      point3f point;
      function_b9dd0(unit_index, &point);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 12:
    if (*(long *)(data + 0x80) == 12)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      volatile real yaw = request_angle_delta(*(real *)(data + 0x8c), *(real *)(data + 0x1a4));
      volatile real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 13:
    if (*(long *)(data + 0x80) == 13)
    {
     if (!data[0x1a0])
     {
      *(point3f *)(data + 0x1a4) = *(point3f *)(unit + 0x64);
      data[0x1a0] = true;
     }
     else
     {
      point3f point = *(point3f *)(unit + 0x64);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 14: if (function_c8860(unit_index) == NONE) result = true; break;
   case 2:
    if (*(long *)(data + 0x80) == 2)
    {
     if (!data[0x1a0])
     {
      data[0x1ac] = false; data[0x1ad] = false;
      *(real *)(data + 0x1a4) = *(real *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real delta = request_angle_delta(*(real *)(data + 0x8c), *(real *)(data + 0x1a4));
      if (delta < -0.3490658402442932f) data[0x1ac] = true;
      if (delta > 0.3490658402442932f) data[0x1ad] = true;
      if (data[0x1ad] && data[0x1ac]) result = true;
     }
    }
    break;
   case 4: if (function_1e69d0(state->requests, 0x1d, g_510c54->field_2_3)) result = true; break;
   case 15: if (state->timer19a > 0) { result = true; state->timer19a = 0; } break;
   case 18: if (state->timer199 > 0) { result = true; state->timer199 = 0; } break;
   case 0:
    if (data[0x88] & 4) { state->field_194 &= ~1; result = true; }
    break;
   case 1:
    if (data[0x88] & 1) { state->field_194 &= ~2; result = true; }
    break;
   case 31: if (data[0x88] & 4) { state->field_194 &= ~4; result = true; } break;   case 17: if (state->timer198 > 0) { result = true; state->timer198 = 0; } break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0())
    {
     long gamepad = *(long *)((byte *)player + 0x24);
     if (gamepad != NONE)
     {
      const s_type_ff3a2a *input = function_1249a0((short)gamepad);
      if (input && (((const byte *)input)[0x15] > 0 || ((const byte *)input)[0x18] > 0)) result = true;
     }
    }
    break;
   case 16:
    if (*(long *)(data + 0x80) == 16)
    {
     if (!data[0x1a0]) { data[0x1a0] = true; *(long *)(data + 0x1a4) = 0; }
     else
     {
      if (function_c8860(unit_index) != NONE) ++*(long *)(data + 0x1a4);
      else *(long *)(data + 0x1a4) = 0;
      if (*(long *)(data + 0x1a4) > g_510c54->field_2_3 / 2) result = true;
     }
    }
    break;
   case 20: if (function_1e69d0(state->requests, 0x1f, g_510c54->field_2_3)) result = true; break;
   case 23:
    if (function_cbd50(unit_index, *(signed char *)(unit + 0x212)) != NONE)
    {
     s_weapon_status status;
     byte *current_unit = *(byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12 + 8);
     function_100520(function_cbd50(player->unit_index, *(signed char *)(current_unit + 0x212)), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded < status.magazines[0].loaded_maximum && (*(dword *)(unit + 0x148) & 0x10000000)) result = true;
    }
    break;
   case 9: if (*(dword *)(data + 0x88) & 0x200000) result = true; break;
   case 25: if (*(dword *)(data + 0x88) & 0x10000) result = true; break;
   case 29:
    if (function_1e8eb0(unit_index, 4, false) && function_f42f0(*(long *)(unit + 0x14))) result = true;
    break;
   case 27: if (*(dword *)(data + 0x88) & 0x20000000) result = true; break;
   case 24: if (*(dword *)(data + 0x88) & 0x4000) result = true; break;
   case 26: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 28: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 21: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 22: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 5: if (state->field_195 > 0) result = true; break;
   case 6: if (function_1e69d0(state->requests, 0x1c, g_510c54->field_2_3)) result = true; break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 3) &&
     *(word *)(data + 0x148) >= 3 && (data[0xa4] & 2)) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && (data[0xa4] & 0x20)) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
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

// @retail 0x1e7ab0
bool function_1e7ab0(long local_index, long type)
{
 (void)&local_index;
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 volatile bool result = false;
 long player_index = NONE;
 if (local_index != NONE) player_index = g_4e8c20->entries[local_index];
 s_entry_420 *entry = entry_420_get(type);
 if (player_index != NONE && entry &&
  (!(*((byte *)entry + 0x10) & 1) || g_4e6948->state != 2))
 {
  s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = request_unit(unit_index);
   switch (type)
   {
   case 10:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 10) data[0x1a0] = false; }
    break;
   case 11:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 11) data[0x1a0] = false; }
    break;
   case 12:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 12) data[0x1a0] = false; }
    break;
   case 13:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 13) data[0x1a0] = false; }
    break;
   case 14:
    if (function_1e7a60(local_index, 16) == 0 && function_c8860(unit_index) != NONE) result = true;
    break;
   case 2:
    if (!function_13cb40() && function_1e7a10(local_index, 12) && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 2) data[0x1a0] = false; }
    break;
   case 4:
    if (!function_13cb40())
    {
     long parent = *(long *)(unit + 0x14);
     short seat = *(short *)(unit + 0x1fc);
     if (parent != NONE && seat != NONE)
     {
      byte *definition = g_4e3b44[*(long *)request_unit(parent) & 0xffff].bytes;
      byte *seats = *(byte **)(definition + 0x1cc);
      dword flags = *(dword *)(seats + seat * 0xb0);
      if (!((bool)((flags >> 2) & 1)) && !((bool)((flags >> 3) & 1)) && !((bool)((flags >> 11) & 1)))
      {
       s_unit_child_iterator iterator;
       function_d0590(&iterator, parent);
       bool blocked = false;
       while (function_d05c0(&iterator))
       {
        if (iterator.seat_index != NONE && iterator.unit_index != unit_index &&
         ((bool)((*(dword *)(seats + iterator.seat_index * 0xb0) >> 2) & 1)))
        { blocked = true; break; }
       }
       if (!blocked) result = true;
      }
     }
    }
    break;
   case 15:
    if (request_weapon(player, false) != NONE && !motion_sensor_enemy_nearby(local_index)) result = true;
    break;
   case 18:
    if (request_weapon(player, false) != NONE && !motion_sensor_enemy_nearby(local_index) && function_1e7a10(local_index, 15)) result = true;
    break;
   case 0: result = (state->field_194 & 1) != 0; break;
   case 1: result = ((state->field_194 >> 1) & 1) != 0; break;
   case 31: result = ((state->field_194 >> 2) & 1) != 0; break;
   case 17: if (state->field_19b > 0 && *(short *)(unit + 0x1fc) == NONE) result = true; break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0() && function_162fd0(local_index) != NONE)
    {
     long gamepad = *(long *)((byte *)player + 0x24);
     if (gamepad != NONE && voice_port_can_talk(gamepad)) result = true;
    }
    break;
   case 16:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false) != NONE &&
     request_weapon(player, true) == NONE && function_100fd0(request_weapon(player, false)) &&
     function_101010(request_weapon(player, false), 0) != NONE)
    { result = true; if (*(long *)(data + 0x80) != 16) data[0x1a0] = false; }
    break;
   case 20:
    if (motion_sensor_enemy_vehicle_ahead(local_index) && *(short *)(unit + 0x1fc) == NONE) result = true;
    break;
   case 23:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false) != NONE)
    {
     s_weapon_status status;
     function_100520(request_weapon(player, false), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded * 2 <= status.magazines[0].loaded_maximum &&
      status.magazines[0].total_maximum != 0 && status.magazines[0].unloaded > 0) result = true;
    }
    break;
   case 9:
    if (request_weapon(player, true) != NONE && function_100f00(request_weapon(player, true))) result = true;
    break;
   case 25: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 28: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 24: if (function_1e8eb0(unit_index, 1, false)) result = true; break;
   case 26: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 27: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 29: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 22:
    if (*(real *)(unit + 0xf0) < 0.25f || (*(long *)(data + 0x80) == 22 && function_1e7a10(local_index, 21))) result = true;
    break;
   case 21:
    if ((bool)(((dword)*(word *)(unit + 0x10a) >> 9) & 1))
     if (*(real *)(unit + 0xf0) < 0.25f || *(long *)(data + 0x80) == 21) result = true;
    break;
   case 5:
    if (*(word *)(data + 0x140) == 3 && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 6:
    if (*(word *)(data + 0x140) == 5 && *(word *)(data + 0x148) >= 3)
    {
     byte *target = (byte *)function_badc0(*(long *)(data + 0x144), 2);
     if (target && *(short *)(g_4e3b44[*(long *)target & 0xffff].bytes + 0x21a) != 0) result = true;
    }
    break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 3) && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && state->field_197 >= 3) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
}

long function_1469f0(real seconds);

// @retail 0x1e6bd0
void function_1e6bd0(long player_index)
{
 s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
 long local_index = player->local_player_index;
 bool restricted = g_4e6948->state == 1 && (g_4e6948->difficulty == 3 || g_4e6948->difficulty == 2);
 if (local_index != NONE && *((byte *)g_51e9c0 + 0x6c5) && !restricted)
 {
  s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
  long *current = (long *)((byte *)state + 0x80);
  long *elapsed = (long *)((byte *)state + 0x84);
  long selected = *current;
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
   s_request_transition *transition = &state->transitions[selected];
   long old_current = *current;
   if (selected != old_current)
   {
    if (old_current != NONE)
    {
     state->transitions[old_current].ticks = 0;
     state->transitions[old_current].state = 0;
    }
    *current = selected;
    *elapsed = 0;
   }
   switch ((word)transition->state)
   {
   case 0:
    transition->state = 1;
    transition->ticks = entry ? (short)function_1469f0(*(real *)entry->field_0c) : 0;
    break;
   case 1:
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     *current = NONE;
    }
    else if (function_1e7ab0(local_index, selected))
    {
     if (--transition->ticks <= 0) { transition->state = 2; transition->ticks = 0; }
    }
    else
    {
     transition->state = 0;
     transition->ticks = 0;
     *current = NONE;
    }
    break;
   case 2:
    ++*elapsed;
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     *current = NONE;
     *elapsed = 0;
    }
    else if (!function_1e7ab0(local_index, selected))
    {
     transition->state = 3;
     transition->ticks = entry ? (short)(g_510c54->field_2_3 * entry->field_08) : 0;
    }
    break;
   case 3:
    ++*elapsed;
    if (function_1e83c0(local_index, selected))
    {
     function_1e8d80(selected, state, entry, transition);
     *current = NONE;
     *elapsed = 0;
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
     *current = NONE;
     *elapsed = 0;
    }
    break;
   case 4:
    if (--transition->ticks <= 0) { transition->ticks = 0; transition->state = 0; }
    break;
   }
  }
  for (long i = 0; i < 32; ++i)
  {
   if (i != *current)
   {
    s_local_player_state_view *local_8da5ff = &((s_local_player_state_view *)g_51e9c0)[local_index];
    long value = ((local_8da5ff->request_flags[1] & (1 << i)) ? 2 : 0) +
     ((local_8da5ff->request_flags[0] & (1 << i)) ? 1 : 0);
    if (value != 3)
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
void __stdcall function_18fcc4(long index, s_player_profile *settings, long profile_index);
void __stdcall function_18fd20(long index, s_player_profile_settings *settings, long profile_index);

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

// @retail 0x1e75d0
void __stdcall function_1e75d0(dword flush)
{
	(void)&flush;
	__declspec(align(8)) s_request_settings settings;
	for (volatile long index = next_request_player(NONE); index != NONE; index = next_request_player(index))
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
						bool volatile changed = false;
						if (memcmp(settings.values + 0x128, state->request_flags, sizeof(state->request_flags)))
						{
							memcpy(settings.values + 0x128, state->request_flags, sizeof(state->request_flags));
							*((byte *)state + 0x19d) = 1;
							changed = true;
						}
						if (!flush)
						{
							if (changed)
								function_18fcc4(*(long *)((byte *)player + 0x24), (s_player_profile *)&settings, profile_index);
						}
						else if (*((byte *)state + 0x19d))
						{
							function_18fd20(*(long *)((byte *)player + 0x24), (s_player_profile_settings *)&settings, profile_index);
							*((byte *)g_51e9c0 + index * 0x1b0 + 0x19d) = 0;
						}
					}
				}
			}
		}
	}
}
