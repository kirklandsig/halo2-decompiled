// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "unknown_11cc90.h"
#include "units.h"
#include "unknown_0d0690.h"
#include <float.h>

/* the slot tests 0x5f, 0x60, 0x5e, 0x4d, 0x4e and 0x4f, and slot type 0x4c */

struct s_slot_4c
{
	s_slot_header header;
	byte unknown0c[4];
	long element_index;
	byte unknown14[0x1c - 0x14];
	long unknown1c;
	short seat_index;
	byte flags;
	byte unknown23;
	short unknown24;
	byte unknown26[2];
	real unknown28;
	real unknown2c;
	byte unknown30[0x34 - 0x30];
	point3f point;
};

short __stdcall function_1b94c0(long actor_index, s_slot *slot);
short __stdcall function_1b9500(long actor_index, s_slot *slot);
short __stdcall function_1b9540(long actor_index, s_slot *slot);
short __stdcall function_1b9640(long actor_index, s_slot *slot);
short __stdcall function_1b9890(long actor_index, s_slot *slot);
short __stdcall function_1b99d0(long actor_index, s_slot *slot);
short __stdcall function_1b9fc0(long actor_index);
short __stdcall function_1ba4e0(long actor_index, s_slot *slot, bool active);
long __stdcall function_1ba090(long actor_index, s_slot *slot);
void __stdcall function_1ba3f0(long actor_index, s_slot *slot, s_slot_target_list *list);
bool __stdcall function_1ba5c0(long actor_index, s_slot *slot, long index);
short __stdcall function_1bb3a0(long actor_index, long joint_index, long a, long b);
short __stdcall function_1b2ff0(long actor_index);
void *function_1e5240(long actor_index);

/* how the actor's character uses its weapon (unknown_1e1f20.h) */
struct s_character_weapon_1b9
{
	byte unknown00[0x10];
	real unknown10;
};
short __stdcall function_1bcc90(long actor_index);

/* a seat of a vehicle's tag (0xb0 bytes) */
struct s_vehicle_seat_definition
{
	s_seat_definition_flags flags;
	byte unknown04[0x88 - 0x4];
	real unknown88;
	real unknown8c;
	byte unknown90[0xb0 - 0x90];
};

struct s_vehicle_tag_view
{
	byte unknown000[0xbc];
	byte flagsbc;
	byte unknownbd[0x1c8 - 0xbd];
	long seat_count;
	s_vehicle_seat_definition *seats;
	byte unknown1d0[0x1f0 - 0x1d0];
	short unknown1f0;
};


struct s_player_view
{
	byte unknown000[0xc0];
	char team;
	byte unknown0c1[0x21c - 0xc1];
};

point3f *function_b9dd0(long object_index, point3f *result);
real function_30bf0(vector3f *v);
real function_11cc90(vector2f const *a, vector2f const *b);

/* an element of g_502424 as slot type 0x4c sees it */
struct s_4c_element
{
	byte unknown00[0x7c];
	short unknown7c;
	byte unknown7e[2];
	long object_index;
	bool unknown84;
	byte unknown85;
	bool unknown86;
};

inline void object_seat_unreserve(s_slot_object_view *object, long seat_index)
{
	if (seat_index >= 0 && seat_index < 32)
		object->unknown3b4 &= ~(1 << seat_index);
}

/* the outermost vehicle carrying the object */
// @retail 0x1b8c80
long function_1b8c80(long object_index)
{
	long result = NONE;

	while (object_index != NONE)
	{
		s_slot_object_view *object = object_get(object_index);

		if (object->type != 1)
			break;
		result = object_index;
		object_index = object->parent_index;
	}
	return result;
}

// @retail 0x1b8cc0
short function_1b8cc0(long object_index, long *seat_object_index)
{
	s_object_seat seats[0x40];
	short result = NONE;
	long seat_object = NONE;
	short count = 0;

	function_c8a40(function_1b8c80(object_index), seats, &count, 0x40);
	for (short i = 0; i < count; i++)
	{
		if (TEST_FIELD_BIT(seats[i].definition->flags.bit2))
		{
			result = i;
			seat_object = seats[i].object_index;
			break;
		}
	}
	if (seat_object_index)
		*seat_object_index = seat_object;
	return result;
}

// @retail 0x1b8d40
long function_1b8d40(long object_index)
{
	long result = NONE;
	long seat_object_index = NONE;
	long vehicle_index = function_1b8c80(object_index);
	short seat_index = function_1b8cc0(vehicle_index, &seat_object_index);

	if (seat_object_index != NONE && seat_index != NONE)
		result = function_c8f60(seat_object_index, seat_index);
	return result;
}

// @retail 0x1b8d80
bool function_1b8d80(long actor_index, long object_index, short seat_index, bool ignore_reserved)
{
	s_actor_view *actor = actor_get(actor_index);
	s_object_header_view *header = object_header_get(object_index);

	if (header->type == 1)
	{
		s_slot_object_view *object = (s_slot_object_view *)header->object;

		if (seat_index >= 0 && seat_index < 32)
		{
			dword mask = 1 << seat_index;

			if (object->unknown3b0 & mask)
				return true;
			if (!ignore_reserved && (object->unknown3b4 & mask))
				return true;
		}
	}
	if (actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1))
	{
		for (short i = 0; i < MAXIMUM_AI_PLAYERS; i++)
		{
			s_ai_player *entry = &g_4f55cc[i];

			if (entry->player_index != NONE && entry->unit_index == object_index && entry->unknown08 == seat_index &&
				entry->unknown0a > 0)
			{
				return true;
			}
		}
	}
	if (actor->unknown2f2 > 0 && actor->unknown2e8 == object_index && actor->unknown2ec == seat_index)
		return true;
	return false;
}

// @retail 0x1b8eb0
bool function_1b8eb0(long actor_index, long object_index, short seat_index, bool ignore_reserved)
{
	bool result = false;

	if (function_c8f60(object_index, seat_index) == NONE &&
		!function_1b8d80(actor_index, object_index, seat_index, ignore_reserved))
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE && function_c8200(object_index, seat_index, actor->unknown018))
			result = true;
	}
	return result;
}

// @retail 0x1b8f30
bool function_1b8f30(long actor_index, long object_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short count = 0;
	bool result = false;
	long last_object_index = NONE;
	s_object_seat seats[0x40];

	function_c8a40(object_index, seats, &count, 0x40);
	for (short i = 0; i < count; i++)
	{
		s_object_seat *seat = &seats[i];
		long seat_object_index = seat->object_index;
		s_seat_definition_flags *definition = &seat->definition->flags;

		if (seat_object_index != last_object_index)
		{
			s_slot_object_view *object = object_get(seat_object_index);

			if (object->type == 1 && object->unknown3b4)
				return false;
			last_object_index = seat_object_index;
		}
		if (!TEST_FIELD_BIT(definition->bit11))
		{
			long occupant = function_c8f60(seat_object_index, seat->seat_index);

			if (occupant != NONE)
			{
				short team = object_get(occupant)->team;

				if (team != actor->unknown024 && function_1df560(team, actor->unknown024))
					return false;
			}
			else if (!function_1b8d80(actor_index, seat_object_index, seat->seat_index, false) &&
				function_c8200(seat->object_index, seat->seat_index, actor->unknown018))
			{
				result = true;
			}
		}
	}
	return result;
}

/* false when an enemy of the actor rides the object */
// @retail 0x1b90b0
bool function_1b90b0(long actor_index, long object_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;

	for (long child_index = object_get(object_index)->first_child_index; child_index != NONE; )
	{
		s_slot_object_view *child = object_get(child_index);

		if (child->type == 0 && child->unknown1fc != NONE)
		{
			short team = 0;

			if (child->actor_index != NONE)
				team = actor_get(child->actor_index)->unknown024;
			else if (child->player_index != NONE)
				team = ((s_player_view *)g_4e8c24->data)[child->player_index & 0xffff].team;
			result = !function_1df560(team, actor->unknown024);
			if (!result)
				break;
		}
		child_index = child->next_object_index;
	}
	return result;
}

/* the player riding the object */
// @retail 0x1b9190
long function_1b9190(long object_index, long *rider_index)
{
	long result = NONE;
	s_slot_object_view *object = object_get(object_index);

	if (rider_index)
		*rider_index = NONE;
	for (long child_index = object->first_child_index; child_index != NONE; )
	{
		s_slot_object_view *child = object_get(child_index);

		if (child->type == 0 && child->player_index != NONE)
		{
			result = child->player_index;
			if (rider_index)
				*rider_index = child_index;
			break;
		}
		child_index = child->next_object_index;
	}
	return result;
}

// @retail 0x1b9200
bool function_1b9200(long object_index, long prop_index)
{
	s_slot_object_view *object = object_get(object_index);
	s_vehicle_tag_view *tag = (s_vehicle_tag_view *)g_4e3b44[object->tag_index & 0xffff].bytes;
	bool result = false;

	if (prop_index != NONE && tag->unknown1f0 == 6 && object->parent_index == NONE)
	{
		result = true;
		if (!(tag->flagsbc & 1))
		{
			s_vehicle_seat_definition *seat = NULL;

			for (short i = 0; i < tag->seat_count; i++)
			{
				if (TEST_FIELD_BIT(tag->seats[i].flags.bit3))
					seat = &tag->seats[i];
			}
			if (seat)
			{
				s_prop_state_view *state = prop_node_state(prop_node_get(prop_index));
				point3f position;
				vector3f forward;

				function_b9dd0(object_index, &position);
				object_get_forward(object_index, &forward);
				forward.k = 0.0f;
				if (function_30bf0(&forward) > 0.0f)
				{
					vector3f direction;

					direction.i = state->position.x - position.x;
					direction.j = state->position.y - position.y;
					direction.k = 0.0f;
					if (function_30bf0(&direction) > g_45dbd8)
					{
						real angle = function_11cc90((vector2f *)&direction, (vector2f *)&forward);

						if (angle <= seat->unknown88 || angle > seat->unknown8c)
							result = false;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1b9420
bool function_1b9420(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool volatile result = false;

	if (actor->unknown086 >= 3)
		return true;

	s_object_child_iterator iterator;

	function_d0620(actor->unknown26c, &iterator);
	while (function_d0690(&iterator))
	{
		if (object_get(iterator.child_value)->player_index != NONE)
			return true;
	}
	return result;
}

// @retail 0x1b94c0
short __stdcall function_1b94c0(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (function_1b9420(actor_index) && function_1b2ff0(actor_index) > 0)
		result = 0x6d;
	return result;
}

// @retail 0x1b9500
short __stdcall function_1b9500(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (function_1b9420(actor_index) && function_1bcc90(actor_index) > 0)
		result = 0x6b;
	return result;
}

/* the vehicle's entry in the actor's character tag says when the actor
   wants to be in it */
// @retail 0x1b9540
short __stdcall function_1b9540(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_object_view *vehicle = object_get(actor->unknown26c);
	s_tag_element *element = function_1e5450(actor_index, vehicle->tag_index);
	bool wanted = actor->unknown5d4;

	if (element && actor->prop_index != NONE)
	{
		if (wanted)
		{
			real elapsed = (real)(g_510c54->game_time - actor->unknown2f8) * g_510c54->rate;

			if (element->unknowna8 > elapsed)
			{
				wanted = true;
			}
			else if (elapsed > element->unknownac)
			{
				wanted = false;
			}
			else
			{
				s_character_weapon_1b9 *weapon = (s_character_weapon_1b9 *)function_1e5240(actor_index);

				if (weapon && prop_node_get(actor->prop_index)->unknown28 > weapon->unknown10)
					wanted = false;
			}
		}
		else if (vehicle->unknown100 > element->unknowna4)
		{
			wanted = true;
			actor->unknown2f8 = g_510c54->game_time;
		}
	}
	else
	{
		wanted = false;
	}
	actor->unknown44a = wanted;
	actor->unknown449 = wanted;
	return g_46fbe4;
}

// @retail 0x1b9fc0
short __stdcall function_1b9fc0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown858 == NONE)
	{
		s_slot_entry_iterator iterator;

		iterator.actor_index = actor_index;
		iterator.reference.unknown2 = 0x4c;
		iterator.reference.unknown0 = NONE;
		while (function_26f0c0(&iterator))
		{
			s_4c_element *element = (s_4c_element *)element_502424_get(actor->memory[iterator.reference.unknown0].unknown4);

			if (!element->unknown86)
			{
				if (actor->unknown26c != NONE)
				{
					if (actor->unknown266 && actor->unknown26c == element->object_index)
						result = 3;
				}
				else if (actor->unknown328 < 10)
				{
					result = 3;
				}
			}
		}
	}
	return result;
}

// @retail 0x1ba3f0
void __stdcall function_1ba3f0(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_slot_4c *state = (s_slot_4c *)slot;

	if (state->unknown1c != NONE && state->seat_index != NONE)
		object_seat_unreserve(object_get(state->unknown1c), state->seat_index);

	s_4c_element *element = (s_4c_element *)list;

	if (element->object_index != NONE && element->unknown7c == 1)
	{
		short count = 0;
		s_object_seat seats[0x40];

		function_c8a40(element->object_index, seats, &count, 0x40);
		if (count > 0)
		{
			s_object_seat *seat = seats;
			long local_0 = (word)count;
			do
			{
				s_object_header_view *header = object_header_get(seat->object_index);

				if (header->type == 1)
					object_seat_unreserve((s_slot_object_view *)header->object, seat->seat_index);
				seat++;
			}
			while (--local_0);
		}
	}
}

// @retail 0x1ba8c0
void __stdcall function_1ba8c0(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_slot_4c *state = (s_slot_4c *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown26c == NONE)
	{
		if (state->flags & 4)
		{
			actor->unknown420 = 4;
			actor->unknown41c = 3;
			actor->unknown424.point = state->point;
			actor->unknown44d = true;
		}
		else if (actor->unknown086 >= 7)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 2;
			actor->unknown488 = true;
		}
		else if (actor->unknown50c)
		{
			actor->unknown41c = 2;
			actor->unknown420 = 0;
		}
	}
	actor->unknown450 = 0x6000086;
}

/* the unit fields function_1ba990 reads */
struct s_unit_1ba990
{
	byte unknown000[0x84];
	real unknown084;
	vector3f velocity;
	byte unknown094[0xec - 0x94];
	real unknownec;
	byte unknownf0[0x10a - 0xf0];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word unknown10a_3 : 13;
};

/* unknown_0259d0's distance_sq3f (0x24550), inlined */
static inline real distance_squared3d_1ba990(point3f const *a, point3f const *b)
{
	vector3f v;
	v.i = b->x - a->x;
	v.j = b->y - a->y;
	v.k = b->z - a->z;
	real sum = v.k * v.k;
	sum += v.i * v.i;
	sum += v.j * v.j;
	return sum;
}

/* whether the unit is close enough to the actor (and slow enough) for it */
// @retail 0x1ba990
bool function_1ba990(long actor_index, long unit_index, bool force, real near_radius, real far_radius, bool use_near_radius)
{
	s_actor_view *actor = actor_get(actor_index);
	s_unit_1ba990 *unit = (s_unit_1ba990 *)object_get(unit_index);
	bool result = false;

	if (TEST_FIELD_BIT(unit->flag10a_2))
	{
		result = false;
	}
	else if (force)
	{
		result = true;
	}
	else if (0.1f > unit->unknownec)
	{
		result = false;
	}
	else
	{
		real radius = use_near_radius ? near_radius : far_radius;
		point3f position;

		function_b9dd0(unit_index, &position);
		if (distance_squared3d_1ba990(&actor->position, &position) < radius * radius)
		{
			result = true;
			if (!use_near_radius && length_sq3f(&unit->velocity) > 0.25f)
				result = false;
		}
	}
	if (0.5f > unit->unknown084)
		return false;
	return result;
}

__declspec(noinline) bool function_f5dc0(long object_index);
long function_25d810(long object_index, long actor_index, bool create);
bool __stdcall function_25c230(long actor_index, long prop_ref_index, short unknown);

/* a player as slot test 0x4d sees it */
struct s_player_unit_view
{
	byte unknown00[0x2c];
	long unit_index;
};

short function_1a6fe0(long owner_index, short type);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* slot test 0x4d: the actor boards the vehicle of a friendly player near
   enough to it */
// @retail 0x1b9640
short __stdcall function_1b9640(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (!actor->unknown224 && !actor->unknown225 && actor->unknown858 == NONE)
	{
		long vehicle_index = NONE;
		real near_radius = FLT_MAX;
		real far_radius = FLT_MAX;
		s_data_datum_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		if (data_datum_iterator_next(&iterator))
		do
		{
			s_player_unit_view *player = (s_player_unit_view *)iterator.datum;

			if (player->unit_index != NONE)
			{
				s_slot_object_view *unit = object_get(player->unit_index);

				if (unit->unknown1fc != NONE && !function_1df560(unit->team, actor->unknown024))
				{
					long outermost_index = function_1b8c80(unit->parent_index);
					s_slot_object_view *vehicle = (s_slot_object_view *)function_badc0(outermost_index, 2);

					if (vehicle && function_1b8f30(actor_index, outermost_index))
					{
						real scale = vehicle->unknown3a0 == actor->unknown030 ? 2.0f : 1.0f;
						real near_distance = 10.0f;
						real far_distance = 12.0f;

						if (function_1ba990(actor_index, outermost_index, false, scale * near_distance, scale * far_distance, false))
						{
							vehicle_index = outermost_index;
							near_radius = near_distance;
							far_radius = far_distance;
						}
					}
				}
			}
		}
		while (data_datum_iterator_next(&iterator));

		if (vehicle_index != NONE)
		{
			long prop_index = function_25d810(vehicle_index, actor_index, true);
			short slot_index;

			if (prop_index != NONE && prop_node_get(prop_index)->unknown24 < 1)
				function_25c230(actor_index, prop_index, 3);
			slot_index = function_1a6fe0(actor_index, 0x4c);
			if (slot_index == NONE || function_1b8c80(((s_slot_4c *)&actor->slots[slot_index])->unknown1c) != vehicle_index)
			{
				s_slot_4c *state = (s_slot_4c *)slot;

				state->unknown1c = vehicle_index;
				state->unknown28 = near_radius;
				state->seat_index = NONE;
				state->unknown2c = far_radius;
				state->flags |= 0x29;
				result = 0x4c;
			}
		}
	}
	return result;
}

/* slot test 0x4e: the actor goes for the vehicle it was told to use */
// @retail 0x1b9890
short __stdcall function_1b9890(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (!actor->unknown224 && !actor->unknown225 && actor->unknown858 == NONE)
	{
		long vehicle_index = actor->unknown2e8;

		if (vehicle_index != NONE && actor->unknown2f0 && !function_f5dc0(vehicle_index) &&
			object_get(vehicle_index)->unknown34f > 0 &&
			function_1ba990(actor_index, vehicle_index, false, 20.0f, 24.0f, false))
		{
			s_slot_4c *state = (s_slot_4c *)slot;
			long prop_index = function_25d810(actor->unknown2e8, actor_index, true);

			if (prop_index != NONE && prop_node_get(prop_index)->unknown24 < 1)
				function_25c230(actor_index, prop_index, 3);
			long updated_vehicle = actor->unknown2e8;
			state->unknown28 = 20.0f;
			state->seat_index = NONE;
			state->unknown2c = 24.0f;
			state->unknown1c = updated_vehicle;
			state->flags |= 0x29;
			result = 0x4c;
		}
	}
	return result;
}

/* invites the clump members to the joint, the nearest first, when the
   object has more than one seat to take */
// @retail 0x1bb3a0
short __stdcall function_1bb3a0(long actor_index, long joint_index, long a, long b)
{
	long count = 0;
	s_actor_view *actor = actor_get(actor_index);
	s_4c_element *element = (s_4c_element *)element_502424_get(joint_index);
	long local_0 = element->object_index;
	short seat_count = 0;
	short free_count = 0;
	point3f position;
	s_object_seat seats[0x40];

	function_c8a40(local_0, seats, &seat_count, 0x40);
	for (short i = 0; i < seat_count; i++)
	{
		if (!TEST_FIELD_BIT(seats[i].definition->flags.bit11))
			free_count++;
	}
	function_b9dd0(element->object_index, &position);
	if (free_count > 1 && actor->unknown07c != NONE)
	{
		long index = element_502420_get(actor->unknown07c)->first_actor_index;

		while (index != NONE)
		{
			s_actor_view *other = actor_get(index);
			long other_index = index;

			index = other->next_index;
			if (actor != other &&
				(other->unknown26c == NONE || other->unknown26c == element->object_index && other->unknown266))
			{
				vector3f delta;

				vector3d_from_points3d(&position, &other->position, &delta);
				if (function_26eae0(joint_index, other_index, 3, (real)(1.0 / (length_sq3f(&delta) + 0.1f))))
					count++;
			}
		}
	}
	return (short)count;
}

// @retail 0x1bb530
void __stdcall function_1bb530(long actor_index, s_slot *slot, long index)
{
	s_slot_4c *state = (s_slot_4c *)slot;
	s_502424_element *element = element_502424_get(state->element_index);

	if (state->unknown1c == index || element->target.unknown0 == index)
	{
		state->unknown1c = NONE;
		element->target.unknown0 = NONE;
	}
}

s_slot_handler_0 g_47e954 =
{
	0x5f, 0, 0xbff, -2, 0, function_1b94c0
};

s_slot_handler_0 g_47e968 =
{
	0x60, 0, 0xbff, -2, 0, function_1b9500
};

s_slot_handler_0 g_47e97c =
{
	0x5e, 0, 0, -2, 0, function_1b9540
};

s_slot_handler_0 g_47e990 =
{
	0x4d, 0, 0, -2, 0, function_1b9640
};

s_slot_handler_0 g_47e9a4 =
{
	0x4e, 0, 0, -2, 0, function_1b9890
};

s_slot_handler_0 g_47e9b8 =
{
	0x4f, 0, 0, -2, 0, function_1b99d0
};

s_slot_handler_2x g_47e9d0 =
{
	{
		{
			0x4c, 2, 0, -2, 0,
			function_1b9fc0, function_1ba4e0, joint_initiate, joint_leave, 1, {0},
			0, 0, function_1bb530, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1ba090, (t_slot_release)function_1ba3f0, (t_slot_release)function_1ba5c0, slot_release_nothing, function_1ba8c0, (t_slot_proc4)function_1bb3a0,
	1, 10, 1.5f, 0x5b
};

// @retail 0x1ba4e0
short __stdcall function_1ba4e0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_4c *state = (s_slot_4c *)slot;
	s_4c_element *element = (s_4c_element *)element_502424_get(state->element_index);
	short result = g_46fbe8;

	state->unknown24++;
	if (element->object_index != NONE && state->unknown1c != NONE)
	{
		real seconds = g_510c54->field_2_3 * 10.0f;
		long ticks;

		__asm
		{
			fld seconds
			fistp ticks
		}

		if (state->unknown24 <= ticks)
		{
			if (actor->unknown26c != NONE)
			{
				result = g_46fbe4;
				if (element->unknown7c > 1)
				{
					if (actor->unknown266)
						result = g_46fbe8;
					if (!element->unknown84)
						function_1fb7e0(0x39, actor_index, NULL, NONE, NONE);
				}
			}
			return result;
		}
	}

	return g_46fbe4;
}

long __stdcall function_c7160(long unit_index, short seat_index, long marker_position, long vehicle_index, long position);
bool function_cc750(long unit_index, short seat_index);
bool function_cc7b0(long unit_index, short seat_index);
real normalize2d(point2f *vector);

struct s_seat_approach_result
{
	point3f point;
	vector3f direction;
	real score;
	bool close;
	bool facing;
	bool approaching;
};

// @retail 0x1baae0
bool function_1baae0(long actor_index, long object_index, short seat_index, bool ignore_bonus,
	bool ignore_reserved, bool vertical, s_seat_approach_result *out)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (function_1b8eb0(actor_index, object_index, seat_index, ignore_reserved))
	{
		point3f marker;
		point3f position;
		if (function_c7160(actor->unknown018, seat_index, (long)&marker, object_index, (long)&position) != NONE)
		{
			vector3f direction;
			direction.i = marker.x - position.x;
			direction.j = marker.y - position.y;
			direction.k = 0.0f;
			real bonus = 0.0f;
			if (normalize2d((point2f *)&direction) == 0.0f)
				direction = actor->unknown290;
			real distance;
			double dx = (double)position.x - actor->position.x;
			double dy = (double)position.y - actor->position.y;
			if (vertical)
			{
				double dz = (double)position.z - actor->position.z;
				distance = (real)sqrt(dx * dx + dy * dy + dz * dz);
			}
			else
			{
				real mx = marker.x - actor->position.x;
				real my = marker.y - actor->position.y;
				point3f const *nearest = sqrt(dx * dx + dy * dy) > sqrt(mx * mx + my * my) ? &marker : &position;
				double nx = (double)nearest->x - actor->position.x;
				double ny = (double)nearest->y - actor->position.y;
				distance = (real)sqrt(nx * nx + ny * ny);
			}
			s_slot_object_view *object = object_get(object_index);
			if (object->type == 1 && !ignore_bonus && function_cc750(object_index, seat_index) &&
					((*(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x1ec) >> 11) & 1))
				bonus = 50.0f;
			real dot = actor->unknown290.i * direction.i + actor->unknown290.j * direction.j;
			bool close = distance < 0.4f;
			bool facing = dot > 0.6f;
			bool approaching = distance < 1.1f && dot > 0.0f;
			real score = 10.0f / (distance + 1.0f) + bonus;
			if (function_cc7b0(object_index, seat_index))
				score += 200.0f;
			if (out)
			{
				out->point = position;
				out->direction = direction;
				out->score = score;
				out->close = close;
				out->facing = facing;
				out->approaching = approaching;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x1bb1d0
short function_1bb1d0(long actor_index, long object_index, s_object_seat const *seats,
    short count, bool reject_single, long *chosen_object, s_seat_approach_result *out)
{
    s_actor_view *actor = actor_get(actor_index);
    short result = NONE;
    if (actor->unknown26c != NONE)
    {
        if (actor->unknown26c == object_index && actor->unknown018 != NONE)
        {
            s_slot_object_view *unit = object_get(actor->unknown018);
            if (chosen_object)
                *chosen_object = unit->parent_index;
            result = unit->unknown1fc;
            if (out)
            {
                out->facing = true;
                out->approaching = true;
                out->direction = actor->unknown290;
                out->point = actor->position;
                out->score = 1.0f;
                out->close = true;
            }
        }
    }
    else
    {
        real best_score = 0.0f;
        long best_object = NONE;
        short eligible = 0;
        s_seat_approach_result best;
        for (short i = 0; i < count; i++)
        {
            s_object_seat const *seat = &seats[i];
            if (!TEST_FIELD_BIT(seat->definition->flags.bit11))
            {
                s_seat_approach_result approach;
                if (function_1baae0(actor_index, seat->object_index, seat->seat_index, false, false, false, &approach))
                {
                    eligible++;
                    if (approach.score > best_score)
                    {
                        best_object = seat->object_index;
                        result = seat->seat_index;
                        best = approach;
                        best_score = approach.score;
                    }
                }
            }
        }
        if (reject_single && eligible == 1)
            return NONE;
        if (result != NONE)
        {
            if (chosen_object)
                *chosen_object = best_object;
            if (out)
                *out = best;
        }
    }
    return result;
}

struct s_seat_selection
{
    long object_index;
    short seat_index;
    byte flags;
    byte unknown7;
};

bool function_2676d0(long actor_index);
bool function_1e2030(long actor_index);

// @retail 0x1b9e70
bool function_1b9e70(long actor_index, s_seat_selection *selection, bool reserved,
    s_4c_element const *element, s_seat_approach_result *out)
{
    bool search = false;
    bool result = false;
    if (selection->seat_index != NONE)
    {
        if (function_1baae0(actor_index, selection->object_index, selection->seat_index, false, !reserved, false, out))
            result = true;
        else
        {
            object_seat_unreserve(object_get(selection->object_index), selection->seat_index);
            search = ((selection->flags >> 3) & 1) != 0;
            selection->seat_index = NONE;
            selection->object_index = NONE;
        }
    }
    else
        search = true;
    if (selection->seat_index == NONE && search)
    {
        s_object_seat seats[0x40];
        short count = 0;
        long object_index;
        function_c8a40(element->object_index, seats, &count, 0x40);
        bool reject_single = element->unknown85 && (!function_2676d0(actor_index) || !function_1e2030(actor_index));
        selection->seat_index = function_1bb1d0(actor_index, element->object_index, seats, count,
            reject_single, &object_index, out);
        if (selection->seat_index != NONE && object_index != NONE)
        {
            selection->object_index = object_index;
            return true;
        }
    }
    return result;
}


long function_baf40(long object_index);
bool __stdcall function_f5d10(long vehicle_index, long bit, bool set);
void function_26ee40(s_joint_behavior_state *behavior);
void function_1f86a0(long actor_index);

// @retail 0x1ba090
long __stdcall function_1ba090(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    s_slot_object_view *unit = object_get(actor->unknown018);
    s_slot_4c *state = (s_slot_4c *)slot;
    s_seat_selection *selection = (s_seat_selection *)&state->unknown1c;
    if (*(long *)((byte *)unit + 0x14) != NONE)
        return NONE;
    s_slot_entry_iterator iterator;
    iterator.actor_index = actor_index;
    iterator.reference.unknown2 = 0x4c;
    iterator.reference.unknown0 = NONE;
    s_slot_memory_entry *entry;
    long joint_index = NONE;
    while ((entry = function_26f0c0(&iterator)) != NULL)
    {
        s_4c_element *element = (s_4c_element *)element_502424_get(entry->unknown4);
        bool specified = (selection->flags & 0x20) != 0;
        bool same = specified && function_baf40(element->object_index) == function_baf40(selection->object_index);
        if ((element->unknown86 ? !(selection->flags & 0x40) || !same : specified && !same))
            joint_decline(actor_index, iterator.reference.unknown0);
        else if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
        {
            joint_index = entry->unknown4;
            break;
        }
    }
    if (joint_index == NONE)
    {
        joint_index = function_26e940(actor_index);
        if (joint_index != NONE)
        {
            *((bool *)slot + 0xc) = true;
            state->element_index = joint_index;
        }
    }
    if (joint_index != NONE)
    {
        s_4c_element *element = (s_4c_element *)element_502424_get(joint_index);
        if (*((bool *)slot + 0xc))
        {
            if (selection->flags & 0x20)
            {
                element->object_index = function_1b8c80(selection->object_index);
                element->unknown86 = ((selection->flags >> 6) & 1) != 0;
                if (actor->unknown26c == selection->object_index)
                    selection->seat_index = object_get(actor->unknown018)->unknown1fc;
            }
        }
        else if (!(selection->flags & 0x20))
        {
            selection->object_index = element->object_index;
            selection->seat_index = NONE;
            selection->flags |= 0x29;
            state->unknown28 = 11.0f;
            state->unknown2c = 14.0f;
        }
        if (!(selection->flags & 0x20) || selection->object_index == NONE || element->object_index == NONE ||
            !function_1ba990(actor_index, selection->object_index, ((selection->flags >> 6) & 1) != 0, state->unknown28, state->unknown2c, false))
            goto failed;
        if (function_2676d0(actor_index) && function_1e2030(actor_index))
        {
            selection->seat_index = NONE;
            element->unknown85 = true;
        }
        else
        {
            if (!function_1b9e70(actor_index, selection, true, element, 0) || selection->object_index == NONE || selection->seat_index == NONE)
                goto failed;
            s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[object_get(selection->object_index)->tag_index & 0xffff].bytes;
            s_vehicle_seat_definition *seat = &definition->seats[selection->seat_index];
            if (actor->unknown26c == NONE)
            {
                short type;
                if (TEST_FIELD_BIT(seat->flags.bit2)) type = 0x65;
                else if (TEST_FIELD_BIT(seat->flags.bit3)) type = 0x67;
                else type = 0x68;
                function_1fb7e0(type, actor_index, 0, selection->object_index, NONE);
                function_f5d10(selection->object_index, selection->seat_index, true);
            }
        }
        function_1f86a0(actor_index);
        state->unknown24 = 0;
    }
    return joint_index;
failed:
    if (joint_index != NONE)
        function_26ee40((s_joint_behavior_state *)slot);
    return NONE;
}


#include "unit_requests.h"
struct s_unit_child_iterator
{
    long object_index;
    long unit_index;
    short seat_index;
    long next_index;
};
struct s_damage_object;
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);
void function_1e3400(long actor_index, long squad_index);
void function_201ad0(long squad_index, long object_index);

// @retail 0x1bb570
void __stdcall function_1bb570(long vehicle_index, long actor_index)
{
    if (g_4f55d0->active)
    {
        s_actor_view *actor = actor_get(actor_index);
        bool driver = *(long *)((byte *)object_get(vehicle_index) + 0x248) == actor->unknown018;
        if (*((bool *)actor + 0x3c))
            *((bool *)actor + 0x3c) = false;
        else if (actor->unknown030 != NONE)
            *(long *)((byte *)actor + 0x28) = actor->unknown030;
        long root_index = function_baf40(vehicle_index);
        if (g_4e0300->data[(root_index & 0xffff) * 12 + 3] == 1)
            vehicle_index = root_index;
        byte *vehicle = (byte *)object_get(vehicle_index);
        long squad_index = *(long *)(vehicle + 0x3a0);
        if (actor->unknown030 != squad_index)
        {
            if (!driver && *(long *)(vehicle + 0x248) == NONE)
                goto event;
            byte *squad = squad_index == NONE ? 0 : g_51e9d8->data + (squad_index & 0xffff) * 0x98;
            if (squad && !function_1df560(actor->unknown024, *(signed char *)(squad + 0x76)))
                function_1e3400(actor_index, (word)squad_index);
            else if (driver)
            {
                if (actor->unknown030 == NONE && *(long *)((byte *)actor + 0x28) != NONE)
                    function_1e3400(actor_index, *(word *)((byte *)actor + 0x28));
                if (actor->unknown030 != NONE)
                    function_201ad0(actor->unknown030, vehicle_index);
            }
            else
            {
                byte *local_11eac6 = (byte *)object_get(*(long *)(vehicle + 0x248));
                if (*(long *)(local_11eac6 + 0x13c) != NONE && !team_is_enemy(actor->unknown024, 1))
                    goto event;
                if (!squad)
                    function_1e3400(actor_index, NONE);
                else
                {
                    s_slot_object_view *unit = object_get(actor->unknown018);
                    s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[object_get(*(long *)((byte *)unit + 0x14))->tag_index & 0xffff].bytes;
                    if (!TEST_FIELD_BIT(definition->seats[unit->unknown1fc].flags.bit11))
                    {
                        function_e68c0(0x1d, actor->unknown018);
                        goto done;
                    }
                }
                goto event;
            }
        }
        if (driver)
        {
            s_object_seat seats[0x40];
            long count = 0;
            long previous = NONE;
            function_c8a40(vehicle_index, seats, (short *)&count, 0x40);
            for (short i = 0; i < (short)count; ++i)
            {
                if (seats[i].object_index != previous)
                {
                    previous = seats[i].object_index;
                    s_unit_child_iterator iterator;
                    iterator.object_index = previous;
                    iterator.unit_index = NONE;
                    iterator.seat_index = NONE;
                    iterator.next_index = *(long *)((byte *)object_get(previous) + 0x10);
                    byte *unit;
                    while ((unit = (byte *)function_d05c0(&iterator)) != NULL)
                    {
                        long other_index = *(long *)(unit + 0x12c);
                        if (other_index != NONE && other_index != actor_index && *(short *)(unit + 0x1fc) != NONE)
                        {
                            s_actor_view *other = actor_get(other_index);
                            if (function_1df560(actor->unknown024, other->unknown024))
                                function_e68c0(0x1d, iterator.unit_index);
                            else if (other->unknown030 != actor->unknown030)
                                function_1e3400(other_index, (word)actor->unknown030);
                        }
                    }
                }
            }
        }
event:
        {
            short seat_index = object_get(actor->unknown018)->unknown1fc;
            if (seat_index != NONE)
            {
                s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[*(long *)vehicle & 0xffff].bytes;
                if (!TEST_FIELD_BIT(definition->seats[seat_index].flags.bit11))
                    function_1fb7e0(0x66, actor_index, 0, vehicle_index, NONE);
            }
        }
done:
        actor->unknown2e8 = NONE;
        actor->unknown2ec = NONE;
        *(short *)actor->unknown2ee = NONE;
        actor->unknown2f0 = false;
        *(short *)((byte *)actor + 0x2f4) = (short)real_to_long((real)g_510c54->field_2_3 * 10.0f);
        actor = actor_get(actor_index);
        *((bool *)actor + 0x5d4) = false;
        *(dword *)((byte *)actor + 0x810) &= ~1;
    }
}

struct joint_state;
long joint_count_invited_participants(joint_state const *arg_0);
bool __stdcall function_1badc0(point3f const *arg_2, long arg_1, long arg_0,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6);
real function_1f8940(long arg_0);
bool function_1f8660(long arg_0);

// @retail 0x1ba5c0
bool __stdcall function_1ba5c0(long arg_0, s_slot *arg_1, long arg_2)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_seat_selection *local_2 = (s_seat_selection *)((byte *)arg_1 + 0x1c);
    s_4c_element *local_3 = (s_4c_element *)arg_2;
    volatile bool local_4 = true;
    if (local_2->object_index == NONE || local_3->object_index == NONE)
        return false;
    if (local_0->unknown26c != NONE || (local_2->flags & 2))
        return local_4;
    if (*(short *)((byte *)local_3 + 2) <= 0 &&
        (short)joint_count_invited_participants((joint_state *)local_3) != 0)
        return local_4;
    if (*((byte *)local_0 + 0x5d8) || !function_1b90b0(arg_0, local_3->object_index) ||
        !function_1ba990(arg_0, local_3->object_index, !(local_2->flags & 1),
            ((s_slot_4c *)((byte *)local_2 - 0x1c))->unknown28, ((s_slot_4c *)((byte *)local_2 - 0x1c))->unknown2c, true))
        return false;
    s_seat_approach_result local_5;
    if (!function_1b9e70(arg_0, local_2, false, local_3, &local_5))
        return false;
    function_f5d10(local_2->object_index, local_2->seat_index, true);
    ((s_slot_4c *)((byte *)local_2 - 0x1c))->point = *(point3f *)&local_5.direction;
    bool local_6 = false;
    if (local_5.approaching)
    {
        ++*(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30);
        local_6 = (real)*(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30) * g_510c54->rate >= 1.0f;
    }
    else
        *(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30) = 0;
    if (local_6 || (local_5.close && local_5.facing))
    {
        s_unit_request local_7;
        local_7.type = 0x1c;
        local_7.type1c.object_index = local_2->object_index;
        local_7.type1c.seat_index = local_2->seat_index;
        local_7.type1c.unknowna = false;
        local_7.type1c.unknownb = false;
        local_4 = function_e6900(local_0->unknown018, &local_7);
    }
    else if (local_5.close)
        function_1f86a0(arg_0);
    else if (local_0->unknown040)
    {
        s_type_c3b527 local_8;
        long local_9;
        bool local_10;
        if (function_1badc0(&local_5.point, local_2->object_index, arg_0,
            &local_8, &local_9, false, &local_10))
        {
            bool local_14 = function_1f4460(arg_0, &local_8, local_9,
                local_10 ? NONE : local_2->object_index, false);
            local_4 = local_14;
            if (!local_14)
            {
                ++local_2->unknown7;
                short local_11 = (local_2->flags & 1) ? 5 : 50;
                if ((short)local_2->unknown7 > local_11)
                {
                    local_4 = false;
                    actor_get(arg_0)->unknown040 = false;
                }
                else
                    local_4 = true;
            }
            else
                local_2->unknown7 = 0;
        }
        else
            local_2->unknown7 = 0;
    }
    bool local_12;
    if (function_1f8660(arg_0))
        local_12 = 1.0f > function_1f8940(arg_0);
    else
    {
        vector3f local_13;
        vector3d_from_points3d(&local_0->position, &local_5.point, &local_13);
        local_12 = 1.0f > local_13.k * local_13.k + local_13.i * local_13.i + local_13.j * local_13.j;
    }
    if (local_12)
        local_2->flags |= 4;
    else
        local_2->flags &= ~4;
    return local_4;
}
