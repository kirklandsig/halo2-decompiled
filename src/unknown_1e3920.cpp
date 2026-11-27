// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E3920.CPP: the radius within which an actor counts as arrived */

#include "unknown_11c920.h"
#include "unknown_1fb7e0.h"
#include "props.h"
#include "ai_actor.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"
#include "vector_products.h"
#include <math.h>
#include <string.h>
#include "unknown_1e46c0.h"

long function_1e4a50(long index);

struct s_actor_reset_view_x
{
	byte field_000[0x290];
	vector3f a;
	vector3f b;
	vector3f c;
	byte field_2b4[0x7fc - 0x2b4];
	long field_7fc;
	long field_800;
	byte field_804[0xc];
	dword flags;
	vector3f field_814;
	real field_820;
	real field_824;
	vector3f field_828;
	vector3f field_834;
	vector3f field_840;
};

// @retail 0x1e3860
void function_1e3860(long actor_index)
{
	s_actor_reset_view_x *actor = (s_actor_reset_view_x *)actor_get(actor_index);
	actor->field_828 = actor->a;
	actor->field_834 = actor->b;
	actor->field_840 = actor->c;
	actor->flags = 0;
	actor->field_820 = 0.0f;
	actor->field_824 = 0.0f;
	actor->field_814 = *g_4687a4;
	actor->field_800 = NONE;
	actor->field_7fc = 0x6000086;
}

/* the actor's movement block of its character tag (function_1e4a50) */
struct s_character_movement_view
{
	byte unknown0[8];
	real arrival_radius;
};

// @retail 0x1e3920
real function_1e3920(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real radius = actor->unknown4cc;

	if (radius == 0.0f)
	{
		s_moving_object *unit = moving_object_get(actor->unit_index);

		if (actor->unknown26c != NONE)
		{
			s_tag_element *element = function_1e5450(actor_index, moving_object_get(actor->unknown26c)->tag_index);

			if (element)
				radius = *(real *)((byte *)element + 0x18);
			if (radius <= 0.0f)
				radius = 0.5f;
		}
		else
		{
			s_tag_element *element;

			if (actor->unknown229 && (element = function_1e5450(actor_index, unit->tag_index)) != 0)
			{
				radius = *(real *)((byte *)element + 0x18);
				if (radius <= 0.0f) radius = 0.3f;
			}
			else
			{
				s_character_movement_view *movement = (s_character_movement_view *)function_1e4a50(actor->tag_index);

				if (movement)
					radius = movement->arrival_radius;
			}
			if (radius <= 0.0f)
				radius = 0.3f;
		}
	}

	if (radius > 0.2f)
		return radius;
	return 0.2f;
}

struct s_actor_point_request
{
	byte field_0[0x14];
	s_record_pool *pool;
	long iterator_index;
	long datum_index;
	long group_index;
	s_element_502420_member_iterator actors;
	short type;
	short mode;
	real radius;
	real radius_squared;
	long index;
	point3f point;
	long result_index;
	real squared_distance;
};

struct s_actor_point_group
{
	long field_0;
	point3f point;
	short field_10;
	short team;
	byte field_14[4];
	long first_actor;
	byte field_1c[0x50 - 0x1c];
};

bool function_1df560(short first, short second);

PRIVATE __forceinline s_actor_point_group *actor_point_next_group(s_actor_point_request *request)
{
	s_actor_point_group *result = NULL;
	if (g_4f55d0->active)
	{
		s_record_pool *pool = request->pool;
		long index = function_16bc00(pool, request->datum_index + 1);
		s_actor_point_group *group;
		if (index != NONE)
		{
			group = (s_actor_point_group *)(pool->data + pool->size * index);
			request->datum_index = index;
			request->iterator_index = (*(short *)group << 16) | index;
		}
		else
		{
			request->datum_index = pool->maximum_count;
			request->iterator_index = NONE;
			group = NULL;
		}
		*(s_actor_point_group **)((byte *)request + 0x10) = group;
		if (group)
			result = group;
		request->group_index = request->iterator_index;
	}
	return result;
}

// @retail 0x1e47d0
s_actor_moving *function_1e47d0(s_actor_point_request *request)
{
	s_actor_view *actor;
	for (;;)
	{
		if (request->index == NONE)
		{
			bool eligible = false;
			s_actor_point_group *group;
			while ((group = actor_point_next_group(request)) != NULL)
			{
				switch (request->type)
				{
				case 0: eligible = !function_1df560(group->team, request->mode); break;
				case 1: eligible = function_1df560(group->team, request->mode); break;
				case 2: eligible = true; break;
				}
				if (eligible)
				{
					vector3f delta;
					vector3d_from_points3d(&request->point, &group->point, &delta);
					if (sqrt(length_sq3f(&delta)) < request->radius + 6.0)
					{
						request->index = request->group_index;
						request->actors.next_index = ((s_actor_point_group *)g_502420->data)[request->index & 0xffff].first_actor;
						break;
					}
				}
			}
		}
		if (request->index == NONE)
		{
			actor = NULL;
			break;
		}
		while ((actor = element_502420_member_next(&request->actors)) != NULL)
		{
			vector3f delta;
			vector3d_from_points3d(&actor->position, &request->point, &delta);
			real squared = length_sq3f(&delta);
			if (squared < request->radius_squared)
			{
				request->result_index = request->actors.index;
				request->squared_distance = squared;
				goto done;
			}
		}
		request->index = NONE;
	}
done:
	return (s_actor_moving *)actor;
}

__forceinline void actor_point_iterator_initialize(s_record_pool_iterator *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->data = g_502420;
		iterator->index = NONE;
		iterator->datum_index = NONE;
	}
}

// @retail 0x1e4770
void function_1e4770(s_actor_point_request *request, short mode, const point3f *point, short type, real radius)
{
	const short *type_reference = &type;
	request->mode = mode;
	request->type = *type_reference;
	request->radius = radius;
	request->radius_squared = radius * radius;
	request->point = *point;
	request->index = NONE;
	request->result_index = NONE;
	actor_point_iterator_initialize((s_record_pool_iterator *)&request->pool);
	request->index = NONE;
}

long function_1e4a10(long index);
real function_30bf0(vector3f *vector);

struct s_actor_impulse_view
{
	byte field_0[0x18];
	long unit_index;
	byte field_1c[0x54 - 0x1c];
	long tag_index;
	byte field_58[0x3b8 - 0x58];
	bool active;
	byte field_3b9[3];
	real magnitude;
	vector3f direction;
};

struct s_character_impulse_view
{
	byte field_0[0x50];
	real threshold;
};

// @retail 0x1e28b0
void function_1e28b0(long actor_index, const vector3f *direction, real magnitude)
{
	s_actor_impulse_view *actor = (s_actor_impulse_view *)actor_moving_get(actor_index);
	s_character_impulse_view *definition = (s_character_impulse_view *)function_1e4a10(actor->tag_index);
	if (definition && magnitude > definition->threshold)
	{
		actor->active = true;
		actor->magnitude = magnitude;
		actor->direction = *direction;
		if (function_30bf0(&actor->direction) == 0.0f)
			actor->direction = *g_4687a8;
	}
}

__forceinline void actor_direction_between_points(const point3f *position, const s_type_c3b527 *target, vector3f *direction)
{
	function_210c90(position, target, direction);
}

// @retail 0x1e3370
bool function_1e3370(long actor_index, void *unknown)
{
	vector3f *direction = (vector3f *)unknown;
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;
	if (!actor->unknown007)
	{
		if (actor->unknown5d0)
			*direction = actor->unknown5ec;
		else if (actor->unknown50c)
		{
			const s_type_c3b527 *target = &actor->unknown4ec;
			const point3f *position = &actor->position;
			actor_direction_between_points(position, target, direction);
		}
		else
			goto done;
		result = true;
		if (function_30bf0(direction) == 0.0f)
			result = false;
	}
done:
	return result;
}

short actor_behavior_find_level(long owner_index, short type);

#pragma inline_depth(0)
// @retail 0x1e1e50
bool function_1e1e50(long actor_index, vector3f *direction)
{
	const long *index_reference = &actor_index;
	s_actor_moving *actor = (s_actor_moving *)(g_4f55f0->data + (*index_reference & 0xffff) * sizeof(s_actor_moving));
	bool result = false;
	if (actor->prop_index != NONE)
	{
		s_type_f95cd3 *view = function_25d700(actor->prop_index);
		if (view && *(short *)view > 6 && actor_behavior_find_level(*index_reference, 14) != NONE && actor->unknown722 > 0)
		{
			*direction = *(vector3f *)((byte *)actor + 0x758);
			result = direction->i * direction->i + direction->j * direction->j + direction->k * direction->k > 0.0f;
		}
	}
	return result;
}
#pragma inline_depth()


struct s_actor_periodic_view
{
	byte field_0[0x40];
	bool update_first;
	byte field_41;
	short first_ticks;
	bool update_second;
	byte field_45;
	short second_ticks;
};

struct s_ai_periodic_view
{
	byte field_0[6];
	short first_threshold;
	short first_maximum;
	bool first_used;
	byte field_b;
	short second_threshold;
	short second_maximum;
	bool second_used;
};

bool function_26c120(long actor_index);

// @retail 0x1e3790
void function_1e3790(long actor_index)
{
	s_actor_periodic_view *actor = (s_actor_periodic_view *)actor_moving_get(actor_index);
	actor->first_ticks++;
	actor->second_ticks++;
	bool first = false;
	bool eligible = function_26c120(actor_index);
	if (eligible)
	{
		s_game_time_globals *time = g_510c54;
		if (!((s_ai_periodic_view *)g_4f55d0)->first_used && actor->first_ticks > ((s_ai_periodic_view *)g_4f55d0)->first_threshold &&
			actor->first_ticks * time->rate > 0.5f)
		{
			actor->first_ticks = 0;
			((s_ai_periodic_view *)g_4f55d0)->first_used = true;
			first = true;
		}
		else if (actor->first_ticks > ((s_ai_periodic_view *)g_4f55d0)->first_maximum)
			((s_ai_periodic_view *)g_4f55d0)->first_maximum = actor->first_ticks;
	}
	actor->update_first = first;
	bool second = false;
	s_game_time_globals *time = g_510c54;
	if (!((s_ai_periodic_view *)g_4f55d0)->second_used && actor->second_ticks > ((s_ai_periodic_view *)g_4f55d0)->second_threshold &&
		actor->second_ticks * time->rate > 0.5f)
	{
		actor->second_ticks = 0;
		((s_ai_periodic_view *)g_4f55d0)->second_used = true;
		second = true;
	}
	else if (actor->second_ticks > ((s_ai_periodic_view *)g_4f55d0)->second_maximum)
		((s_ai_periodic_view *)g_4f55d0)->second_maximum = actor->second_ticks;
	actor->update_second = second;
}

struct s_actor_height_definition
{
	byte field_0[0x1a];
	short height_index;
};

struct s_height_table
{
	byte field_0[0x98];
	real heights[6];
};

struct s_height_globals
{
	byte field_0[0xc8];
	long count;
	s_height_table *table;
};

// @retail 0x1e20b0
real function_1e20b0(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_actor_height_definition *definition = (s_actor_height_definition *)function_1e4a50(actor->tag_index);
	short height_index = definition->height_index;
	real result;
	if (height_index == 7)
		result = 100000.0f;
	else
	{
		short index = (height_index < 6 ? height_index : 6) - 1;
		real height = 0.0f;
		s_height_globals *globals = (s_height_globals *)g_4e034c;
		if (index >= 0 && index < 6 && globals && globals->count > 0)
			height = globals->table->heights[index];
		result = (real)sqrt((height * 2.0f) * 6.417322635650635f);
	}
	return result;
}

struct s_actor_control_request
{
	long name;
	short mode;
	byte field_6[0x10 - 6];
	dword flags;
	vector3f movement;
	real first_scale;
	real second_scale;
	vector3f forward;
	vector3f first;
	vector3f second;
	vector3f third;
	byte field_58[0x7c - 0x58];
};

struct s_actor_control_view
{
	byte field_0[8];
	bool reset;
	byte field_9[0x18 - 9];
	long unit_index;
	byte field_1c[0x7fc - 0x1c];
	long name;
	byte field_800[0xc];
	short mode;
	byte field_80e[2];
	dword flags;
	vector3f movement;
	real first_scale;
	real second_scale;
	vector3f forward;
	vector3f first;
	vector3f second;
	vector3f third;
};

struct s_unit_control_view
{
	byte field_0[0x130];
	long simulation_index;
	byte field_134[8];
	long player_index;
};

struct s_unit_state_c6ef0;
class c_class_6a600;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);
void __stdcall function_cbf60(long unit_index, bool active);
void function_690d0(c_class_6a600 *world, long actor_index, const dword *state);

// @retail 0x1e4390
void function_1e4390(long actor_index)
{
	s_actor_control_view *actor = (s_actor_control_view *)actor_moving_get(actor_index);
	s_unit_control_view *unit = (s_unit_control_view *)moving_object_get(*(long volatile *)&actor->unit_index);
	if (unit->player_index == NONE || *((byte *)g_4e8c20 + 6))
	{
		s_actor_control_request state;
		function_c6ef0((s_unit_state_c6ef0 *)&state);
		state.name = actor->name;
		state.flags = actor->flags;
		state.movement = actor->movement;
		state.first_scale = actor->first_scale;
		state.second_scale = actor->second_scale;
		state.mode = actor->mode;
		state.forward = actor->forward;
		state.first = actor->first;
		state.second = actor->second;
		state.third = actor->third;
		if (actor->reset)
		{
			function_cbf60(*(long volatile *)&actor->unit_index, true);
			actor->reset = false;
		}
		function_c6de0(*(long volatile *)&actor->unit_index, &state);
		if (unit->simulation_index != NONE && !g_4cf772 && *(long *)((byte *)g_4cf77c + 8) != 0)
			function_690d0((c_class_6a600 *)g_4cf77c, unit->simulation_index, (const dword *)&state);
	}
}

#include "object_markers.h"
#include "location_zones.h"
#include "object_tag_definitions.h"

struct s_actor_object_sample
{
	point3f point;
	point3f center;
	vector3f direction;
	dword location[2];
	vector3f velocity;
};

long function_1e4990(long index);
void function_1e3a00(long object_index, s_actor_object_sample *sample);
void function_28f8d0(long index);
void function_caf90(long unit_index, point3f *position);
long function_baf40(long object_index);
long __stdcall function_cbd80(long object_index, long *holder_index);
void *function_1e5280(long actor_index, long key);
void function_118e80(long object_index, vector3f *forward);
real normalize2d(point2f *vector);
real function_267370(long object_index);


/* refreshes the cached movement state of an actor: whether its head marker lies in its cluster's zone, the
   vehicle it rides or is held by, and its forward, up and left vectors. Matches retail byte for byte; the
   named output pointer of the second cross and the zone helper's plane pointer give retail's operand order. */
// @retail 0x1e3b60
void function_1e3b60(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	byte *actor_bytes = (byte *)actor;
	if (actor->unknown007)
	{
		function_28f8d0(*(long *)(actor_bytes + 0x1c));
	}
	else
	{
		byte *movement = (byte *)function_1e4990(actor->tag_index);
		byte *object = (byte *)moving_object_get(actor->unit_index);
		long parent_index = *(long *)(object + 0x14);
		byte *parent = parent_index == NONE ? NULL : (byte *)moving_object_get(parent_index);
		short type = *(signed char *)(object + 0xaa);
		function_1e3a00(actor->unit_index, (s_actor_object_sample *)(actor_bytes + 0x22c));
		point3f point;
		function_caf90(actor->unit_index, &point);
		actor_bytes[0x265] = location_point_in_zone_inline((s_location *)(actor_bytes + 0x250), &point, NULL);
		actor->unknown229 = (movement[0] & 2) != 0 ||
			(type == 0 && *((byte *)moving_object_get(actor->unit_index) + 0x3dc) == 2);
		actor->unknown266 = false;
		actor_bytes[0x267] = 0;
		actor->unknown268 = false;
		actor_bytes[0x269] = 0;
		actor->unknown26c = NONE;
		*(short *)(actor_bytes + 0x270) = 0;
		actor->unknown274 = NONE;
		long carrier = parent && *(byte *)(parent + 0xaa) == 1 ? parent_index :
			*(byte *)(object + 0xaa) == 1 ? actor->unit_index : NONE;
		if (carrier != NONE)
		{
			long resolved = carrier;
			long root = function_baf40(carrier);
			if (((s_moving_object_header *)g_4e0300->data)[root & 0xffff].type == 1)
				resolved = root;
			byte *carrier_object = (byte *)moving_object_get(resolved);
			actor->unknown26c = resolved;
			if (*(long *)(carrier_object + 0x248) == actor->unit_index)
			{
				byte *definition = g_4e3b44[*(long *)carrier_object & 0xffff].bytes;
				if (*(short *)(definition + 0x1f0) != 6)
				{
					actor->unknown266 = true;
					*(short *)(actor_bytes + 0x270) = 1;
					if ((bool)((*(dword *)(definition + 0x1ec) >> 11) & 1))
					{
						if ((bool)((*(dword *)(definition + 0x1ec) >> 12) & 1))
						{
							actor->unknown229 = true;
							*(short *)(actor_bytes + 0x270) = ((bool)((*(dword *)(definition + 0x1ec) >> 13) & 1)) ? 2 : 4;
						}
						else if ((bool)((*(dword *)(definition + 0x1ec) >> 13) & 1))
							*(short *)(actor_bytes + 0x270) = 3;
						else
						{
							short mode = *(short *)(definition + 0x1f0);
							if (mode == 1 || mode == 0 || mode == 2)
								*(short *)(actor_bytes + 0x270) = 5;
						}
					}
				}
			}
		}
		parent_index = *(long *)(object + 0x14);
		if (parent_index != NONE)
		{
			s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[parent_index & 0xffff];
			if ((1 << header->type) & 3)
			{
				parent = header->object;
				if (*(long *)(parent + 0x24c) == actor->unit_index)
				{
					long holder = NONE;
					long held = function_cbd80(parent_index, &holder);
					if (held != NONE && function_1e5280(actor_index, *(long *)moving_object_get(held)))
					{
						actor->unknown268 = true;
						actor->unknown274 = holder;
					}
				}
				if (!actor->unknown268 && !actor->unknown266 && *(short *)(object + 0x1fc) != NONE)
				{
					s_unit_definition const *definition = (s_unit_definition const *)g_4e3b44[*(long *)parent & 0xffff].bytes;
					actor_bytes[0x269] = (byte)((*(dword *)((byte *)definition->unit.seats.address + *(short *)(object + 0x1fc) * 0xb0) >> 11) & 1);
				}
			}
		}
		if (*(long *)(object + 0x14) != NONE)
			actor_bytes[0x267] = !actor->unknown266;
		if (function_26c120(actor_index))
		{
			actor->unknown28c = NONE;
			actor_bytes[0x278] = 0;
		}
		actor->unknown264 = false;
		if (*(byte *)(object + 0xaa) == 0 && actor->unknown26c == NONE)
		{
			byte *unit = (byte *)moving_object_get(actor->unit_index);
			if (*(signed char *)(unit + 0x399) * g_510c54->rate >= 0.2f &&
				*(short *)(unit + *(short *)(unit + 0x346) + 0x36) == 0)
				actor->unknown264 = true;
		}
		if (actor->unknown266)
			function_118e80(actor->unknown26c, &actor->unknown290);
		else
			function_118e80(actor->unit_index, &actor->unknown290);
		if (!actor->unknown229)
		{
			if (normalize2d((point2f *)&actor->unknown290) > 0.0f)
				actor->unknown290.k = 0.0f;
			else
				actor->unknown290 = *g_4687a8;
		}
		if (actor->unknown268)
		{
			long held_index = actor->unknown274;
			s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[held_index & 0xffff];
			byte *held = header->object;
			byte held_bit = 0;
			if (header->type == 1)
				held_bit = (byte)((*(dword *)(g_4e3b44[*(long *)held & 0xffff].bytes + 0x1ec) >> 8) & 1);
			if (held_bit)
				function_118e80(held_index, (vector3f *)(actor_bytes + 0x29c));
			else
				*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(held + 0x168);
		}
		else
			*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(object + 0x168);
		*(vector3f *)(actor_bytes + 0x2a8) = *(vector3f *)(object + 0x18c);
		vector3f *left = (vector3f *)(actor_bytes + 0x2b4);
		vector3f *forward = (vector3f *)(actor_bytes + 0x2c0);
		cross3f_staged(g_4687b0, (vector3f *)(actor_bytes + 0x2a8), left);
		function_30bf0(left);
		cross3f_staged((vector3f *)(actor_bytes + 0x2a8), left, forward);
		*(long *)(actor_bytes + 0x2d0) = *(long *)(object + 0xec);
		*(long *)(actor_bytes + 0x2d4) = *(long *)(object + 0xf0);
		*(long *)(actor_bytes + 0x2d8) = *(long *)(object + 0x100);
		*(long *)(actor_bytes + 0x2dc) = *(long *)(object + 0xfc);
	}
	*(real *)(actor_bytes + 0x2cc) = function_267370(actor->unit_index);
	actor_bytes[0x2e0] = 0;
}

struct s_actor_child_limit_view
{
	byte field_0[0xbc];
	dword : 3;
	dword limited : 1;
	dword : 28;
	byte field_c0[0xe6 - 0xc0];
	short limit;
};

long function_25d810(long object_index, long actor_index, bool create);
long function_25c3a0(long actor_index, long prop_ref_index, short mode);
void function_264210(long actor_index, long prop_ref_index);
void function_25b9f0(long actor_index, long prop_ref_index, vector3f const *direction);
short ai_player_index_get(long player_index);
void function_10e9f0(long object_index, short channel, real value, real time);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

// @retail 0x1e2570
void __stdcall function_1e2570(long actor_index, word type, long object_index, real amount, long direction)
{
	long prop_ref_index = NONE;
	if (object_index != NONE)
	{
		prop_ref_index = function_25d810(object_index, actor_index, true);
		if (prop_ref_index != NONE)
		{
			s_prop_datum *reference = prop_ref_get(prop_ref_index);
			if (reference->tracking_index == NONE && g_470f10[reference->type].unknown8 == 2)
				function_25c3a0(actor_index, prop_ref_index, 3);
			function_264210(actor_index, prop_ref_index);
			s_type_f95cd3 *view = NULL;
			if (reference->tracking_index != NONE)
			{
				s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
				if (tracking)
					view = &tracking->view;
			}
			s_type_76cf92 *prop = prop_get(reference->prop_index);
			if (view)
			{
				if (prop->unknown25)
					view->unknown58 = amount * 0.2f + view->unknown58;
				else
					view->unknown58 = amount + view->unknown58;
				*(short *)((byte *)view + 0x28) = 0;
				view->unknown2a = true;
			}
			*(real *)((byte *)prop + 0x2c) += amount > 1.0f ? 1.0f : amount;
			if (reference->state < 1)
				prop_ref_index = NONE;
			if (prop->unknown25 && !prop->unknown23)
			{
				s_actor_view *actor = actor_get(actor_index);
				if (actor->unknown26c != NONE)
				{
					long player_index = *(long *)((byte *)ai_object_get(object_index) + 0x13c);
					if (player_index != NONE)
					{
						short player = ai_player_index_get(player_index);
						if (player == actor->unknown31c || player == NONE)
						{
							actor->unknown31c = player;
							real scaled_ticks = (real)g_510c54->field_2_3 * 0.6f;
							long rounded_ticks;
							__asm
							{
								fld scaled_ticks
								fistp rounded_ticks
							}
							actor->unknown31e += (short)rounded_ticks;
						}
					}
				}
			}
		}
		function_25b9f0(actor_index, prop_ref_index, (vector3f const *)direction);
	}
	byte *actor = (byte *)actor_get(actor_index);
	if (amount > 0.0f)
	{
		long time = g_510c54->game_time;
		/* the interval test looks the actor up again (retail forms the actor pointer with a lea) */
		if (*(long *)((byte *)actor_get(actor_index) + 0x308) == NONE
			|| time - *(long *)((byte *)actor_get(actor_index) + 0x308) > g_510c54->field_2_3)
		{
			if (*(long *)(actor + 0x18) != NONE)
				function_20ba60(3, *(long *)(actor + 0x18), object_index, NONE, type, NULL);
			*(long *)(actor + 0x308) = time;
		}
		*(real *)(actor + 0x318) += amount;
		if (*(long *)(actor + 0x18) != NONE)
		{
			actor = (byte *)actor_get(actor_index);
			short ticks = g_510c54->field_2_3 * 2;
			function_10e9f0(*(long *)(actor + 0x18), 12, 1.0f, 0.5f);
			*(short *)(actor + 0x6ce) = ticks;
		}
	}
	if (type == 11)
	{
		actor = (byte *)actor_get(actor_index);
		if (*(long *)(actor + 0x18) != NONE)
		{
			long child_index = *(long *)((byte *)ai_object_get(*(long *)(actor + 0x18)) + 0x10);
			s_actor_child_limit_view *first_definition = NULL;
			short count = 0;
			short limit = 50;
			while (child_index != NONE)
			{
				byte *child = (byte *)ai_object_get(child_index);
				if (child[0xaa] == 5)
				{
					s_actor_child_limit_view *definition = (s_actor_child_limit_view *)g_4e3b44[*(long *)child & 0xffff].bytes;
					if (TEST_FIELD_BIT(definition->limited))
					{
						if (!first_definition)
						{
							limit = definition->limit;
							first_definition = definition;
						}
						count++;
					}
				}
				child_index = *(long *)(child + 0xc);
			}
			if (count >= limit)
				function_1a8220(actor_index, 12, 1, 3, 1, NONE, 0);
		}
	}
}

#include "unknown_1fb7e0.h"
// @retail 0x1e18f0
void __fastcall function_1e18f0(long actor_index, long old_weapon, long player_index, long new_weapon)
{
 long const *player_reference = &player_index;
 long const *weapon_reference = &new_weapon;
 byte *player = g_4e8c24->data + (*player_reference & 0xffff) * 0x21c;
 real old_value = 0.0f;
 real new_value = 0.0f;
 if (old_weapon != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (old_weapon & 0xffff) * 12 + 8);
  old_value = *(real *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0x238);
 }
 if (*weapon_reference != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (*weapon_reference & 0xffff) * 12 + 8);
  new_value = *(real *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0x238);
 }
 if (new_value > old_value)
 {
  if (actor_get(actor_index)->unknown018 != NONE)
   function_20ba60(0xbc, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (old_value > new_value)
 {
  if (actor_get(actor_index)->unknown018 != NONE)
   function_20ba60(0xbd, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (actor_get(actor_index)->unknown018 != NONE)
  function_20ba60(0xbe, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
}


/* Actor equipment cleanup uses retail SSE arithmetic. */

extern bool g_4f55e8;
long function_1e4a10(long character_index);
long function_1e4990(long character_index);
void function_d5c90(long unit_index, real *body, real *shield);
void __stdcall function_ccff0(long unit_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
short function_cccd0(long unit_index, short type, short count);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void __stdcall function_b8540(long object_index);
void *character_grenade_settings_get(long character_index, short key);
struct s_203780;
union vector2f;
struct s_28dab2;
struct s_type_c3b527;
struct s_unit_request;
long __stdcall function_28dab0(long character_index, s_28dab2 const *placement);
void function_2036c0(long character_index, short mode, bool *enabled, bool *forced, real *value);
bool function_203900(short squad_index, real probability);
long __stdcall function_1e02f0(long squad_index, long entry_index, long unit_index, long perception_index, bool flag);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);
long function_1e06b0(long tag_index);
point3f *function_210850(s_type_c3b527 const *point, point3f *out);
void function_11dfb0(vector2f const *rotation, vector3f *forward, vector3f *up);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
real function_30bf0(vector3f *vector);
point3f *function_b9dd0(long object_index, point3f *position);
long function_cbd50(long unit_index, short weapon_index);
void function_b9c60(long object_index, bool flag);
bool function_e6900(long unit_index, s_unit_request *request);
void function_a7b30(long index);
void __stdcall swarm_delete(long perception_index);
void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type);


// @retail 0x1e0850
long __stdcall function_1e0850(long character_index, long squad_index, s_203780 const *placement, bool substitute)
{
	long actor_index = NONE;
	{
		byte *definition = g_4e3b44[character_index & 0xffff].bytes;
		long unit_index = NONE, perception_index = NONE;
		byte *flags = (byte *)function_1e4990(character_index);
		byte *squad = squad_index == NONE ? NULL : *(byte **)((byte *)g_4e0350 + 0x164) + (squad_index & 0xffff) * 0x74;
		if (substitute && squad && squad_index != NONE)
		{
			bool replace = true;
			while (replace && *(long *)(definition + 0x28) != NONE)
			{
				bool enabled = false;
				real probability = 0.0f;
				bool forced = false;
				function_2036c0(character_index, *(short *)(squad + 0x30), &enabled, &forced, &probability);
				replace = enabled ? function_203900((short)squad_index, probability) : forced;
				if (replace)
				{
					character_index = *(long *)(definition + 0x28);
					definition = g_4e3b44[character_index & 0xffff].bytes;
					flags = (byte *)function_1e4990(character_index);
				}
			}
		}
		if (flags)
		{
			bool flag = false;
			if (squad) flag = (*(dword *)(squad + 0x20) >> 10) & 1;
			byte const *data = (byte const *)placement;
			if (*flags & 1)
			{
				perception_index = function_28dab0(character_index, (s_28dab2 const *)placement);
			}
			else
			{
				byte creation[0xc4];
				function_b7930(creation, *(long *)(definition + 0x10), NONE, 0);
				function_210850((s_type_c3b527 const *)(data + 4), (point3f *)(creation + 0x1c));
				long variant = *(long *)(data + 0x30);
				if (!variant) variant = function_1e06b0(character_index);
				*(long *)(creation + 0xc) = variant;
				function_11dfb0((vector2f const *)(data + 0x14), (vector3f *)(creation + 0x28), (vector3f *)(creation + 0x34));
				unit_index = function_b7b40(creation);
				if (unit_index != NONE)
				{
					long primary = NONE, secondary = NONE;
					short grenade = NONE;
					byte *scenario = (byte *)g_4e0350;
					long primary_index = NONE;
					if (*(short *)(data + 0x22) != NONE) primary_index = *(short *)(data + 0x22);
					else if (squad && *(short *)(squad + 0x3c) != NONE) primary_index = *(short *)(squad + 0x3c);
					if (primary_index >= 0 && primary_index < *(long *)(scenario + 0x98)) primary = *(long *)(*(byte **)(scenario + 0x9c) + primary_index * 0x28 + 4);
					long secondary_index = NONE;
					if (*(short *)(data + 0x24) != NONE) secondary_index = *(short *)(data + 0x24);
					else if (squad && *(short *)(squad + 0x3e) != NONE) secondary_index = *(short *)(squad + 0x3e);
					if (secondary_index >= 0 && secondary_index < *(long *)(scenario + 0x98)) secondary = *(long *)(*(byte **)(scenario + 0x9c) + secondary_index * 0x28 + 4);
					short grenade_index = *(short *)(data + 0x2c);
					if (grenade_index - 1 != NONE) grenade = grenade_index - 1;
					else if (squad && *(short *)(squad + 0x40) - 1 != NONE) grenade = *(short *)(squad + 0x40) - 1;
					function_1e04a0(primary, character_index, unit_index, secondary, grenade);
					actor_index = function_1e0160(squad_index, character_index, unit_index, flag);
				}
			}
			if (perception_index != NONE)
				actor_index = function_1e02f0(squad_index, character_index, NONE, perception_index, flag);
			if (actor_index != NONE)
			{
				s_actor_view *actor = actor_get(actor_index);
				if (data[0x1c] & 1) actor->unknown086 = 0;
				if (*(real *)(data + 0x38) > 0.0f)
				{
					point3f position;
					function_210850((s_type_c3b527 const *)(data + 4), &position);
					vector3f direction;
					direction.i = (real)(cos(*(real *)(data + 0x14)) * cos(*(real *)(data + 0x18)));
					direction.j = (real)(sin(*(real *)(data + 0x14)) * cos(*(real *)(data + 0x18)));
					direction.k = (real)sin(*(real *)(data + 0x18));
					function_1f34b0(actor_index, &position, &direction, *(real *)(data + 0x38), NONE);
				}
				if (actor->unknown018 != NONE)
				{
					byte request[0x20];
					switch (*(short *)(data + 0x3e))
					{
					case 2:
					{
						byte *movement = (byte *)function_1e4a50(actor->unknown054);
						if (movement && (*movement & 0x20))
						{
							*(long *)request = 0x2c;
							request[4] = true;
							function_e6900(actor->unknown018, (s_unit_request *)request);
						}
						break;
					}
					case 1:
					{
						vector3f forward;
						function_b9fc0(actor->unknown018, &forward, NULL);
						forward.k = 0.0f;
						if (function_30bf0(&forward) > 0.0f)
						{
							*(long *)request = 0x22;
							function_b9dd0(actor->unknown018, (point3f *)(request + 4));
							*(real *)(request + 0x10) = forward.i * -1.0f;
							*(real *)(request + 0x14) = forward.j * -1.0f;
							request[0x18] = true;
							function_e6900(actor->unknown018, (s_unit_request *)request);
						}
						break;
					}
					}
				}
				if (actor->unknown018 != NONE)
				{
					long weapon = function_cbd50(actor->unknown018, ai_object_get(actor->unknown018)->current_weapon);
					if (weapon != NONE &&
						*(long *)(g_4e3b44[ai_object_get(weapon)->definition_index & 0xffff].bytes + 0x2f4) != NONE)
						actor->unknown22a = true;
				}
				if (actor->unknown018 != NONE && (data[0x1c] & 0x10))
					function_b9c60(actor->unknown018, true);
				function_a7b30(unit_index);
			}
			else
			{
				if (unit_index != NONE) function_b8540(unit_index);
				else if (perception_index != NONE) swarm_delete(perception_index);
			}
		}
	}
	return actor_index;
}

// @retail 0x1e04a0
void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type)
{
	real *limits = (real *)function_1e4a10(character_index);
	if (limits)
	{
		short difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : (short)1;
		real body, shield;
		switch (difficulty)
		{
		case 3:
			body = limits[3];
			shield = limits[4];
			if (body <= 0.0f && shield <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			break;
		case 2:
			if (limits[3] <= 0.0f && limits[4] <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			else
			{
				body = (limits[1] + limits[3]) * 0.5f;
				shield = (limits[4] + limits[2]) * 0.5f;
			}
			break;
		default:
			body = limits[1];
			shield = limits[2];
			break;
		}
		if (g_4e6948->state == 1 && g_4f55e8)
		{
			body *= 2.0f;
			shield *= 2.0f;
		}
		if (body > 0.0f || shield > 0.0f)
			function_d5c90(unit_index, &body, &shield);
	}
	byte placement[0xc4];
	if (primary_tag != NONE)
	{
		function_ccff0(unit_index);
		function_b7930(placement, primary_tag, unit_index, 0);
		long weapon = function_b7b40(placement);
		if (weapon != NONE && !function_cd0c0(unit_index, weapon, 1))
			function_b8540(weapon);
	}
	if (secondary_tag != NONE)
	{
		function_b7930(placement, secondary_tag, unit_index, 0);
		long weapon = function_b7b40(placement);
		if (weapon != NONE)
		{
			byte *settings = (byte *)function_1e4990(character_index);
			if (!((settings[0] & 4) && function_cd0c0(unit_index, weapon, 5)) &&
				!function_cd0c0(unit_index, weapon, 1))
				function_b8540(weapon);
		}
	}
	if (grenade_type != NONE)
	{
		byte *settings = (byte *)character_grenade_settings_get(character_index, grenade_type);
		if (settings)
		{
			short lower = *(short *)(settings + 0x34);
			short upper = (short)(*(short *)(settings + 0x36) + 1);
			long count = ((random_next(&g_4e7408->unknown0) * (upper - lower)) >> 16) + lower;
			function_cccd0(unit_index, grenade_type, count);
		}
	}
}

bool __stdcall function_25d020(long actor_index, long object_index, long type,
	long prop_index, long unknown, long reference_index);

extern long g_46fc80;
void function_1a6f70(long actor_index);
void function_1e3790(long actor_index);
void function_262ba0(long actor_index);
void function_204ca0(long actor_index);
void __stdcall function_26dde0(long actor_index);
void function_266640(long actor_index);
void function_267c50(long actor_index);
void function_1e36d0(long actor_index);
void function_258660(long actor_index);
void __stdcall function_1a7cc0(long actor_index);
void actor_behavior_current_update44(long actor_index);
void function_1e3860(long actor_index);
void actor_behavior_current_update48(long actor_index);
void __stdcall swarm_update(long actor_index);
void function_25a380(long actor_index);
void function_1f38c0(long actor_index);
void function_298370(long actor_index);
void __stdcall function_1fc7e0(long actor_index);
void function_1e4390(long actor_index);
void function_2959c0();
void function_1e2a90(long actor_index);
bool function_1e35f0(long actor_index);
void function_1e3b60(long actor_index);

// score 25 (was 25; 26 before round 183; m3 lane 8 r220 at the end): round 183 (m3 lane 6, plain) enabled 0x1f38c0's written body (#581's last named double, in
// 0x1f9e70, became a PIN_126660 clamp), so its call now passes the actor in EAX as retail. Left: retail passes the actor index
// to function_262ba0 in EAX, ours pushes it, because 0x262ba0's body keeps it as a stack argument.
// Earlier at 26: retail passes the actor index to function_262ba0 and to
// function_1f38c0 in EAX; our build passes the first on the stack and the
// second in ECX, because those two callees' argument registers are fixed in
// their own translation units.
// inline/noinline callee declaration 26; noinline declaration 26;
// unsigned callee input 26; inline unsigned callee declaration 26;
// unsigned caller input 26; inline caller 26; inline depth two 26; depth three 26.
// Round 41: the written 0x1f38c0 and 0x1f6090 bodies enabled 90 (a frame slot; 0x1e2f50
// and 0x1c83c0 stop matching); in 0x262ba0, which spills the actor because a loop bool
// takes EBX: 0x2641c0 inlinable 26 (0x264210 lost), plain maximum word 26, call result
// only clearing the sample flag 26, sample/update flags at the loop top 26.
// Round 21 (m3 lane 2, fast): in 0x262ba0 the `update` bool takes EBX (the only callee-saved
// byte register) across the 0x2640c0/0x2641c0 calls, so the actor stays a stack parameter; retail
// keeps the three flags in stack bytes. Tried there: sample/update as two assignments 26 (0x262ba0
// 424), declared after the node call 26, update declared first 26 (422), sampled_one before
// maximum 26, the update test nested 26.
// Round 58 (fast): the global store before the actor lookup 26; the swarm field in a local 26; the
// actor test as a bool local 26; the clear through a pointer to +0x41c 34; the arms swapped 40.
// Round 129: callee-side inline declaration 26; const actor/reset pointer/both 26/26/26;
// const actor_index 26; active_index for the two mismatched calls, every call, or the first three 26/26/26.
// Round 46 (fast, all in 0x262ba0): volatile sample/update flags (diagnostic, reverted) 26 (0x262ba0 424,
// frame 0xa0, but `index` then takes EBX and the actor is still re-read from its stack slot: our loop keeps
// the actor pointer in EDI where retail spills it); the loop's actor fields through fresh actor lookups 26
// (457); plain unqualified word maximum re-measured 26 (429 at 1616 bytes); the current-target test as one named
// bool 26 (438); the list head read through the held actor pointer 26 (442, 1616 bytes); the sample/update
// flags declared before the prior state 26 (412, same code).
// Round 69 (m3 lane 14, fast, all in 0x262ba0, which loads the actor into EBX at entry but hands EBX to a loop byte flag and
// re-reads the actor from its stack home 12 times): `maximum` as a plain word 26 (429 at 1616 bytes); sample/update declared
// at function scope 26 (412); the 0x2641c0 result in its own bool 26 (412); old_state short 26 (413); the actor pointer
// declared then assigned 26 (412).
// Round 143 (m3 lane 8, fast with 0x262ba0, which reads 412 as control): 0x2641c0's noinline declaration dropped, so
// 0x262ba0 inlines it as retail does: 26 (0x262ba0 497 at 1688 bytes; 0x264210 inlines it too and loses its match; its
// five other callers stay matched); on that, 0x262ba0's update flag as long 26 (495), sample/update at function scope 26
// (497), maximum a plain word 26 (497), sampled_one as long 26 (496). The actor stays a stack argument in every form.
// Round 78 (m3 lane 13, fast): in 0x262ba0 the case-2 and completion view lookups through the existing 0x25d740
// (retail's NONE test, NULL test and +0x70 there) 26 (0x262ba0 413, inlined at both sites); 0x1f38c0's real body
// enabled alone (0x1f6090 as a stdcall stub) 90: LTCG pads the callers by four bytes (`push ecx` here, in 0x1c83c0 38
// and 0x137fe0 80; 0x1e2f50 2), which is #581; with 0x29bab0's distance as `real` 90. Bisect (diagnostics, reverted):
// the padding comes from two named doubles below 0x1f38c0, 0x29bab0's `double local_27` and 0x1f9e70's `double dot`
// (through 0x29bab0's call; a static copy of 0x1f9e70 pads with `double dot`, not with `real dot`). With both gone 25
// (`mov eax, ebx` before the 0x1f38c0 call, as retail) and 0x1c83c0, 0x1e2f50 and 0x137fe0 match. Cutting the 0x29cbb0,
// 0x29ab90 or 0x296580 calls instead leaves 90. 0x1f38c0's own frame stays 0x1c (lowest slot unused) either way.
// In 0x262ba0 (fast): case 2's distance through the existing vector2d_from_points2d and dot_product2d gives retail's
// block exactly (y first, the dead `add eax, 4`) but 26 here, 0x262ba0 425 at 1624 bytes (masked aligned 218); with the
// 0x25d740 lookups too 26 (417, masked 219).
// BLOCKED-BY 0x262ba0 (stacked actor) and 0x1f38c0 (stub until #581 is solved: 0x1f9e70's named double, in
// src/unknown_1f5560.cpp, and 0x29bab0's local_27).
// Round 92 (m3 lane 11, fast, in 0x262ba0 with 0x263210/0x264330/0x264b50/0x264210): case 3's limit as one conditional
// 26 (0x262ba0 412, other code); case 3's decrement exit as `if (>= 0) break; goto release_tracking;` 26 (433, 1616 B, frame
// 0xa0); the loop-tail state through the typed `->unknown68` field 26 (412, same code); case 4's arms the other way round 26
// (443, 1644 B); sample/update declared after the 0x263670 call 26 (412, other code). The #581 route stays closed: the two
// named doubles as `real` raise 0x29bab0 (715 -> 750) and 0x1f9e70 (137 -> 302), per their notes.
// M3 lane 10 r100 (fast, with 0x262ba0, 0x263210, 0x264210, 0x2641c0, their five other callers and 0x1e2570; 26 here in
// every form): the stacked actor needs 0x2641c0 inlined in 0x262ba0 (retail), which its noinline declaration stops because
// 0x264210 would inline it too. New on that decision: 0x264210 moved beside 0x264330 (the other file, same flags) with the
// noinline declaration dropped still inlines it (0x264210 114 at 139 bytes, 0x262ba0 497, 0x1e2570 161), the move alone
// changes nothing; 0x2641c0's copy as an aggregate assignment, its two tests nested, an explicit else arm, a named source
// pointer: 0x2641c0 still matches and 0x264210 still inlines it (114) each; the two actor fields in named locals 0x2641c0 2,
// 0x264210 116. Diagnostics: an extra statement in 0x264210 does not stop the inline (139 at 154 bytes), nor does hosting
// 0x264210 in a file without /arch:SSE (114); a heavier 0x2641c0 does (its copy as a 14-pass loop: 0x264210 stays a call at
// 68 bytes while 0x262ba0 still inlines it, 489 at 1684 bytes), so the decision follows the callee's size at a
// non-loop site, not the caller, the file or the flags. Needed: a 0x2641c0 that compiles to retail's 78 bytes but
// weighs more to the inliner. Still BLOCKED-BY 0x262ba0 and 0x1f38c0.
// M3 lane 12 r123 (fast; 26 in every form): only two instructions differ (the 0x262ba0 call's `push ebx` for retail's
// `mov eax, ebx`, the stub call's ECX for EAX); the one-byte shorter push shifts every later call. 0x2641c0 without its
// noinline declaration, reading the actor through actor_get at each use: 0x2641c0 20, 0x264210 still inlines it (113,
// 138 B), 0x262ba0 475; the same with its arms inverted (copy first): 0x2641c0 matches, 0x264210 114, 0x262ba0 497. In
// 0x262ba0 a per-iteration actor pointer 471 (1648 B), that with `update` as long 479. #581 re-measured: the written
// 0x1f38c0 enabled with a stdcall 0x1f6090 stub gives 90 here (274 B), 0x1c83c0 38, 0x137fe0 80, as in round 78.
// Round 183 (m3 lane 6): 0x1f38c0 enabled with 0x1f9e70's named double still in place 90 (274 bytes; 0x1e2f50 lost, as
// round 78); with 0x1f9e70's clamp through PIN_126660 25 (kept). In 0x262ba0 (fast, 25 here each): `update` as the named sampling
// condition with `sample = update` 430 there (1616 bytes, frame 0xa0, the actor index still loaded from the stack into
// EDI); a __forceinline declaration of 0x2641c0 beside 0x262ba0 (its noinline kept in its own file) 412 (same code); the
// saved state as an unsigned word 413; the named condition with `update` long 449 (1600 bytes); the named condition with
// `!sampled_one` tested last 454 (1600 bytes). BLOCKED-BY 0x262ba0 (stacked actor).
// b13M (11-17) walk against retail, fast build on r846: every statement matches 0x1e34e0 (269 bytes): the actor lookup,
// the `g_46fc80` store before the `actor[6]` test (retail 0x1e3503, ebx), the call order, the 0x90-byte clear (retail
// 0x1e355b `rep stosd` of 0x24 dwords) with the two NONE stores through the second lookup (0x1e3561, 0x1e3567), the
// `actor[7]` and `[actor + 0x1c]` swarm tests (0x1e358e, 0x1e3596), and the esi-held NONE reused for the last
// `g_46fc80` store on both swarm/non-swarm exits (0x1e35a3, 0x1e35d5). Register arguments load as retail: EAX for
// 0x1a6f70, 0x1e3790, 0x1e36d0, 0x1e3860, 0x1f3190, 0x1f38c0, 0x298370 and 0x1e4390; EDX for the two
// actor_behavior_current_update calls (0x1e3579, 0x1e3587); 0x204ca0 reads EBX itself, as ours compiles it. The only
// real difference is 0x262ba0's argument (retail 0x1e3520 `mov eax, ebx`, ours `push ebx`): retail's 0x262ba0 takes
// the index in EAX (0x262ba7 `mov ebx, eax`), ours keeps a stack parameter, so nothing in this caller can change it;
// the other `!!` lines are call targets of the reduced image. No new form tried here; the fix is in 0x262ba0's body
// (BLOCKED-BY above).
// M3 lane 14 r120 (fast with 0x262ba0, control 412 there; 25 here in every form): in 0x262ba0 the actor pointer
// `const` 412 (same code); the entry test through a fresh lookup with the pointer fetched after the two entry calls
// 423; the list head read through the held pointer 442 (1616 B); the sampled flag declared after the first call
// 413; the prop loop as `for (long index = head; index != NONE; index = next)` 453 (1600 B, retail's frame 0xa0,
// first difference +0x6). The actor index stays a stack argument in each. Still BLOCKED-BY 0x262ba0.
// Round 642 forms: layout-derived clear extent 25, typed reset span and sentinels 25,
// byte-distance reset extent 25, typed dword sentinel 25, unsigned child test 25,
// typed prefix with child count > 0 at 26, const typed update view 25, scoped
// layout-derived reset 25, callee __cdecl declaration and definition 71.
// NOT kept: the call still pushes the actor index. The __cdecl form also adds caller cleanup.
// M3 lane 2 r115 (fast with 0x262ba0; 25 unless said; this body unchanged): this function MATCHES as it stands once
// 0x262ba0 takes the actor index in EAX, which needs 0x262ba0 never to read the argument's stack home again. Three
// things there each force such reads: our link expands 0x25c820 inline at its release site (retail calls it; the
// expanded 0x25c780 call takes the prop index in EBX), the `update` byte takes BL, and the prop index is promoted to
// a register (retail keeps it in the iterator's cell). Diagnostics in 0x262ba0, reverted at once: the walk form with the
// iterator and the update flag kept in memory and the 0x25c820 call kept out of line gives MATCH here (0x262ba0 545
// at 1496 B); any two of the three without the third 25 (see 0x262ba0's note for the forms and counts).
// BLOCKED-BY 0x262ba0 (0x25c820 expanded inline there; iterator and flag homes).
// M3 lane 11 r137 (fast with 0x262ba0, 0x263210, 0x265680, 0x264330, 0x26be00, 0x26be30; 25 here in every form; 0x262ba0
// control 404 at 1624 B): in 0x262ba0, the walk through the existing 0x26be00/0x26be30 re-measured on today's body 443 at
// 1616 B (aligned 301 against the control's 317); the three release gotos as a `release` flag tested after the switch 442
// (1644 B: 0x25c820 is still expanded at that single site, 35 calls against retail's 34); the walk with that flag 445; the
// walk with sample/update declared ahead of the loop 443 (same code); the walk reading `iterator.index` at every use 443
// (same code). Diagnostic, restored: 0x2641c0's noinline declaration dropped 509 (1696 B; 0x25c820 still expanded and one
// of the four 0x25c230 calls merged; 0x264210 114). Still BLOCKED-BY 0x262ba0.
// M3 lane 5 r217 (fast with 0x262ba0, 0x2641c0, 0x264210, 0x1be120, 0x1fba10, 0x25c230, 0x25d020, 0x265680; 25 here in
// every form; 0x262ba0's file was another lane's this round, so nothing there was tried): 0x2641c0's noinline
// declaration dropped: 0x262ba0 509 (1696 B, expanded there as retail), 0x264210 114 (139 B), and the five other
// callers keep the call and their match, so the one site our link expands against retail is 0x264210's unconditional
// call. With the declaration dropped: two dead constant locals in 0x2641c0 114 there; eight dead computed locals 114;
// the swarm flag, swarm index and sample block through three forceinline accessors 114 (0x2641c0 still matches): the
// decision does not follow the callee's statement count. 0x264210 as an early return 114; its result in a named bool
// 114; its position pointer named ahead of the call 114. Listing with 0x2641c0 expanded in 0x262ba0: the release label
// still expands 0x25c820 while case 1's arm calls it, so that expansion is not a budget effect of the missing
// 0x2641c0 expansion. Lead for 0x262ba0: only the label that three gotos reach is expanded; the two release calls
// written in each failing arm (no label) would make each site as cold as case 1's, which stays a call.
// M3 lane 12 r162 (fast with 0x262ba0, control 404; 25 here in every form): in 0x262ba0 `update && sample` in the 0x264b50
// test 402 (1620 B); `update` as a short 408 (1628 B); `update` read through a const reference at that test 404 (same
// code); case 1's 0x25c820 call jumping to the release block's call (one call site) with a short `old_state` 452 (1600 B,
// frame 0xa0); that with `update && sample` 431 at retail's 1616 B. The actor index stays a stack argument in each.
// Still BLOCKED-BY 0x262ba0.
// M3 lane 5 r223 (fast with 0x262ba0, 0x263210; 25 here in every form; 0x262ba0 control 404 at 1624 B; 0x2641c0's
// noinline declaration is again in another lane's file): in 0x262ba0, retail's case code order 0/4/2/1/3 with the release
// block as the switch's last labelled arm (no tracking_complete goto) 412 (1628 B); that with case 1's 0x25c820 call
// going to the release block's call 436 at 1616 B (frame 0xa0 as retail, actor index still stacked); that with a short
// old state 440 (1620 B); the walk through 0x26be00/0x26be30 with the first form 447 (1620 B), with the second 442
// (1616 B; walk alone 443). Still BLOCKED-BY 0x262ba0 (0x2641c0's expansion).
// M3 lane 2 r129 (h2m fast with 0x262ba0, 0x25c820, 0x25c780 and 0x25c820's three inlining callers; 0x262ba0 control 404 at
// 1624 B with 35 calls against retail's 34; 0x262ba0's and 0x2641c0's files were other lanes' this round, so nothing there):
// 0x25c820 reading the datum through prop_ref_get at each use 25 (0x25c820 25 at 69 B: the lookup is redone after the
// 0x25c780 call; 0x262ba0 423); diagnostic, a second test nested around 0x25c820's 0x25c780 call 25 (0x262ba0 423, still 35
// calls: more written depth in the callee does not stop the release-site expansion); here the not-active case as an early
// return storing NONE 75; the `g_46fc80` store ahead of the actor lookup 25 (same code); the clear and the two NONE stores
// through a fresh actor_get each 69 (276 B). BLOCKED-BY 0x262ba0 (stacked actor).
// M3 lane 5 r227 (h2m fast with 0x262ba0, control 404 at 1624 B; 25 here in every form; 0x2641c0's noinline declaration
// is in another lane's file again, so all five in 0x262ba0): the walk through 0x26be00/0x26be30 with the iterator's
// index read at every use and `update` long 427 (1616 B: the index takes EBX, a zero constant EDI and the long flag a
// stack cell); that with case 1's 0x25c820 call jumping to the release block's call 442; main's body with `update` long
// and that single call 436 (1616 B); the first form with `sampled_one` long 435 (1624 B); main's body with `sample` and
// `update` long and the single call 437 (1616 B). Still BLOCKED-BY 0x2641c0's expansion into 0x262ba0.
// M3 lane 8 r220 (h2m fast with 0x262ba0, 0x264210, 0x2641c0, 0x263210 and 0x2641c0's five other callers; 25 here in
// every form; both 0x262ba0's file and 0x2641c0's declaration file were free): 0x2641c0's noinline declaration dropped
// re-measured 0x262ba0 509 (1696 B), 0x264210 114 (139 B); with that, 0x264210 defined ahead of 0x2641c0 in its file
// (diagnostic: the inline decision does not follow source order) 509 / 114; in 0x262ba0 on that inline base, the sampler
// as `if (!sampler(...)) sample = false; else if (update && ...) 0x264b50(...)` (retail stores the flag only on the
// failed path) 504 (1700 B); that with the walk through 0x26be00/0x26be30 and `iterator.index` at each use 523 (1692 B,
// frame 0xa0 and the {index, next} cells at 0x14/0x18 as retail); that with case 1's 0x25c820 call jumping to the release
// block's call 495 (1672 B, one 0x25c820 call); that with `update` long 499 (1676 B). In each the update flag keeps EBX
// (a byte, then the long) and the actor index arrives in EDI from the stack and is re-read from its home after the
// inlined copy's `rep movsd` takes EDI. All restored. Still BLOCKED-BY 0x262ba0 (update flag in EBX).
// M3 lane 13 r142 (h2m fast with 0x262ba0; 25 in every form; both 0x262ba0's file and 0x2641c0's were free): in
// 0x262ba0 the release calls moved to case 3's underflow test (retail's order) 404 there (same code, 0x25c820 still
// expanded); the three flags grouped in one local aggregate (diagnostic) 394 there at retail's frame, but `update` still
// takes BL; `update` long 404; the sampler's result tested with the store only on its failed path 460, with `update` long
// 454. The actor index stays a stack argument in each. Still BLOCKED-BY 0x262ba0.
// M3 lane 14 r142 (h2m fast with 0x262ba0, control 404 at 1624 B; 25 here in every form; 0x2641c0's noinline declaration
// is in another lane's file again): in 0x262ba0, the old state a `short` read after the flag declarations 404 (same code);
// the sampling threshold read through a fresh actor lookup 452 (1636 B); `sample = update = sampled_one = true;` ahead of
// the field clear 405; the node and byte-view pointers `*const` 404 (same code); `update` typed `byte` 404 (same code).
// Still BLOCKED-BY 0x262ba0 (0x2641c0's expansion keeps retail's actor index in EBX).
// M3 lane 11 r154 (h2m fast with 0x262ba0, control 404; 25 here in every form): diagnostic, 0x2641c0's noinline
// declaration dropped (reverted): 0x262ba0 509 at 1696 B with 34 calls as retail, but our link still expands 0x25c820 at
// the release label (its 0x25c780 call takes the prop index in EBX) and merges one 0x25c230 call, 0x264210 114 (139 B),
// 0x265680 stays matched. In 0x262ba0: the sampled arm's flags as `sample = true; update = true;` 404, with the two flags
// in separate declarations 404; `motion` and `context` declared at function scope 404 (same code each); the release
// calls written in each failing arm (no label, `break` after each) 488 (1680 B), that with the noinline declaration
// dropped 570 (1744 B: the per-arm sites do not leave 0x25c820 called once 0x2641c0 is expanded either).
// M3 lane 13 r143 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680; 25 here in every form): five forms in
// 0x262ba0 (listed in its note: a second prop lookup for the byte view 438 there, the release pair through a
// forceinline helper 404, the flags declared ahead of the lookup 404, `update` declared after the 0x263670 call 405,
// `index` at function scope 404); the actor index stays a stack argument in each. Still BLOCKED-BY 0x262ba0.
// M3 lane 3 r298 (h2m fast with 0x262ba0, control 404 at 1624 B; 25 here in every form; 0x2641c0's noinline declaration
// is in another lane's file again): five forms in 0x262ba0 (listed in its note: char flags, a const prop index, the
// 0x25d610 test nested under `sample && update`, the prop walk as a guarded do-while 464, the sample counter through a
// `short &`); the actor index stays a stack argument in each. Still BLOCKED-BY 0x262ba0.
// M3 lane 6 r232 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680; 25 here in every form): the plain listing's only real
// difference is the 0x262ba0 call (`push ebx` for retail's `mov eax, ebx`); the later rows are operands shifted by that byte. In
// 0x262ba0: sample/update at function scope 404, `short old_state` 404, the flags declared `update, sample` 402 (1620 B), a const
// actor parameter 404, sampled_one declared ahead of maximum 404; the actor stays a stack parameter in each. Still BLOCKED-BY 0x262ba0.
// M3 lane 3 r300 (h2m fast with 0x262ba0, 0x2641c0, 0x264210, 0x265680, 0x263210 and 0x264330; 25 here in every form):
// diagnostic, 0x2641c0's noinline declaration dropped: 0x262ba0 509 (1696 B), this 25, 0x264210 114 (139 B); reverted. In
// 0x262ba0 (404 each, same code): a `short volatile` maximum, `short old_state`, `long const index`, the node pointer and its
// byte view declared the other way round, the state cases in numeric order. Still BLOCKED-BY 0x262ba0.
// M3 lane 16 r182 (h2m fast with 0x262ba0, control 404 at 1624 B; 25 here in every form): in 0x262ba0, `update` typed
// `short` 408 (1628 B), with integer stores 408; `sampled_one` typed `long` 408 (1628 B); the update test nested under
// `if (sample)` after the sampler 404 (same code); the actor pointer `byte *const` 404 (same code). Reading: retail's loop
// is top-tested (the walk test at the loop head, `mov edi, edi` aligned) with sample/update/sampled_one in stack bytes
// 0x11/0x12/0x13 and the actor index in EBX throughout; ours is rotated and keeps sample in BL, so the actor index is
// spilled in the loop and LTCG keeps it a stack argument. Still BLOCKED-BY 0x262ba0.
// M3 lane 16 r183 (h2m fast with 0x262ba0, control 404; 25 here in every form): in 0x262ba0 the context record declared
// ahead of the motion record, the two flags declared after the 0x263670 call, the prior state read ahead of the next
// link, case 1's tracking pointer in its own block, the prior state as a `word` at both uses: 404 each (same code).
// Retail expands 0x2641c0 inside 0x262ba0 and calls it elsewhere; its noinline declaration sat in a file held by another
// lane this round. Still BLOCKED-BY 0x262ba0 (and through it 0x2641c0).
// M3 lane 4 r257 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680, 0x26be00 and 0x26be30; 25 here in every
// form; 0x262ba0 control 404 at 1624 B, aligned rows 209). Measured ranking in 0x262ba0: the node 634, the prop index
// 118, the update flag 30, the actor index 17 (eighteen references, but live and unreferenced across 62 blocks);
// the update flag takes EBX, the last callee-saved register, and the actor index is left in its stack home. The
// loop-top block touches eleven values, so the update flag's clear there counts 44 of its 76. Retail's loop head is the
// existing walk (0x26be00 / 0x26be30) expanded inline: the next link and the index live in two stack cells and the
// three flags in stack bytes. That walk form re-measured: 443 there at retail's 1616 B and frame, aligned rows
// 209 -> 187; in it the prop index takes EBX (36), the update flag 26, the actor index 14. Diagnostic (forced
// ranking, not source): with the actor index ranked first in either body it gets a callee-saved register but is
// still loaded from its stack home (469 / 472 there), so its weight alone does not make it a register argument.
// On the walk form: a `short` prior state 443 (same code); the actor looked up again at each use in the loop
// 466 (1648 B); a plain `short` maximum 446 (1620 B); the release pair written at case 2's failing test 442; the
// two flags declared after the 0x263670 call 443 (same code). Retail expands 0x2641c0 here; its noinline
// declaration sits in a file another lane held this round. Still BLOCKED-BY 0x262ba0 (and through it 0x2641c0).
// M3 lane 7 r227 (fast with 0x262ba0, 0x263210, 0x264330 and 0x265680; 25 here in every form): in 0x262ba0, the
// case-3, case-2 and completion view pointers each declared in its own block (no function-scope view) 404 (same code);
// that with the walk through the existing 0x26be00/0x26be30 443 (1616 B). Readings: forcing the actor index's
// colouring priority in 0x262ba0 (73 forced decisions, diagnostic) fixes its frame (0xa0) at best but never makes it a
// register argument; in a plain compile of 0x262ba0 (no link-time code generation) the actor index sits in EDI and is
// re-read from its stack home six times (EDI goes to the view pointers), and none of the forms above changes that
// count. Retail keeps the loop index in a stack cell (the walk's iterator) and the actor index in EBX. Still
// BLOCKED-BY 0x262ba0.
// M3 lane 7 r228 (fast with 0x262ba0; 25 here in every form; 0x262ba0 control 404 at 1624 B). Ranking in 0x262ba0:
// the actor index 17 (references +332, live and unreferenced over 62 blocks -315) against the update flag's 30 (its
// clear in the eleven-value loop-top block counts 44), so the flag takes EBX, the only callee-saved byte register.
// Retail's loop is the existing 0x26be00/0x26be30 walk with the iterator's index and next in stack cells re-read at
// each use; written that way (443 at 1616 B) the iterator's index is scalar-replaced and takes EBX first (36; the flag
// 26, the actor index 14). In 0x262ba0: the two flags assigned in each arm of the sampling test instead of at their
// declaration 428 (1624 B), on the walk 442; on the walk the flags declared at function scope 442; the actor fetched
// once per iteration for the loop's reads 463 (1644 B); the 0x264b50 guard testing `update` first 435. Still
// BLOCKED-BY 0x262ba0.
// M3 lane 2 r151 (fast screen with 0x262ba0; 25 here in every form, the actor index still pushed for 0x262ba0): in
// 0x262ba0, the 0x2641c0 call written out as its body (diagnostic) 509 there; the update flag computed from the sampling
// test after the 0x263670 call 453; both flags declared after that call 404; the parameter copied into a local at entry
// 404; both flags at function scope, cleared at the loop top, 402. A first-pass (all-stack) compile of 0x262ba0 holds the
// actor index in EDI but reloads it from its home five times after EDI goes to the view pointers, so link-time code
// generation keeps it a stack argument. Still BLOCKED-BY 0x262ba0.
// M3 lane 8 r239 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680; 25 here in every form): in 0x262ba0 (forms
// and counts in its note) an all-stack compile-only screen that reads the actor index from its home once (the update
// flag at function scope, cleared after its use) still leaves it a stack argument here (0x262ba0 431); a nested
// 0x264b50 test 404, `actor[0x48] != 0` 404, the existing prop walk 446 and that with the function-scope flag 447.
// Still BLOCKED-BY 0x262ba0 (0x2641c0's expansion; its noinline declaration is in another lane's file this round).
// M3 lane 4 r263 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680 and 0x264210; 25 here in every form). In
// 0x262ba0 with 0x2641c0's noinline declaration dropped (diagnostic: 0x264210 then 114 at 139 B): `sample` and `update`
// declared bare at function scope and set false at the loop top 512 there (1696 B); `sampled_one` replaced by the
// sampled prop's index (NONE until set) 505 (1688 B), and without the expansion 439 (1616 B, against 404); case 1's
// 0x25c820 call jumping to the release pair's own 0x25c820 call 461 (1660 B); the 0x264b50 guard nested under
// `if (sample)` 509. The actor index stays a stack argument in each. Still BLOCKED-BY 0x262ba0.
// M3 lane 15 r150 (h2m fast with 0x262ba0, 0x263210, 0x264330, 0x265680; 25 here in every form; 0x262ba0 control 404).
// In 0x262ba0: the tracked-prop tests written field first (three sites) 406; the prior state read ahead of the next link
// 404 (same code); the prior state read after the 0x263670 call 434 (1620 B); the sampling test with the actor's sampling
// flag tested first 429; the tail reading a fresh actor lookup after the walk 421 (1632 B). The actor index stays a stack
// argument in each. Still BLOCKED-BY 0x262ba0 (and through it 0x2641c0's noinline declaration, in another lane's file
// this round).
// M3 lane 4 r267 (fast screen with 0x262ba0, 0x263210, 0x264330, 0x265680; 25 here in every form). In 0x262ba0, more
// actor-index references by fetching the actor again: the loop's three tracked-prop reads 476 there (1652 B), every
// actor read in the loop 473 (1648 B), every actor read in the tail 501 (1696 B), both 530 (1708 B), both with the
// update flag at function scope cleared after its use 531. The actor index stays a stack argument in each. Still
// BLOCKED-BY 0x262ba0 (and through it 0x2641c0's noinline declaration, in another lane's file this round).
// @retail 0x1e34e0
void function_1e34e0(long actor_index)
{
	byte *actor = (byte *)actor_get(actor_index);
	g_46fc80 = actor_index;
	if (actor[6])
	{
		function_1a6f70(actor_index);
		function_1e3790(actor_index);
		function_262ba0(actor_index);
		function_204ca0(actor_index);
		function_26dde0(actor_index);
		function_266640(actor_index);
		function_267c50(actor_index);
		function_1e36d0(actor_index);
		byte *current = (byte *)actor_get(actor_index);
		memset(current + 0x41c, 0, 0x90);
		*(long *)(current + 0x450) = NONE;
		*(short *)(current + 0x454) = NONE;
		function_258660(actor_index);
		function_1a7cc0(actor_index);
		actor_behavior_current_update44(actor_index);
		function_1e3860(actor_index);
		actor_behavior_current_update48(actor_index);
		if (actor[7])
		{
			if (*(long *)(actor + 0x1c)) swarm_update(actor_index);
		}
		else
		{
			function_25a380(actor_index);
			function_1f3190(actor_index);
			function_1f38c0(actor_index);
			function_298370(actor_index);
			function_1fc7e0(actor_index);
			function_1e4390(actor_index);
		}
	}
	g_46fc80 = NONE;
}

// @retail 0x1e2f50
void function_1e2f50()
{
	byte *globals = (byte *)g_4f55d0;
	*(short *)(globals + 6) = *(short *)(globals + 8);
	globals[3] = false;
	globals[4] = false;
	globals[0xa] = false;
	globals[0x10] = false;
	*(short *)(globals + 8) = 0;
	*(short *)(globals + 0xc) = *(short *)(globals + 0xe);
	*(short *)(globals + 0xe) = 0;
	long last = 0;
	short remaining = 5;
	s_actor_iterator iterator;
	if (globals[1])
	{
		iterator.iterator.data = g_4f55f0;
		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.active_only = true;
	}
	byte *actor;
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL) actor[0x48] = false;
	if (globals[1])
	{
		iterator.iterator.data = g_4f55f0;
		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.active_only = true;
	}
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
	{
		if (remaining <= 0) break;
		if ((short)iterator.actor_index > *(short *)(globals + 0x368)) { actor[0x48] = true; --remaining; }
		last = iterator.actor_index;
	}
	if (remaining > 0)
	{
		if (globals[1])
		{
			iterator.iterator.data = g_4f55f0;
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			iterator.active_only = true;
		}
		while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
		{
			if (remaining <= 0) break;
			if ((short)iterator.actor_index > *(short *)(globals + 0x368)) break;
			last = iterator.actor_index;
			actor[0x48] = true;
			--remaining;
		}
		if (remaining > 0) last = NONE;
	}
	*(short *)(globals + 0x368) = (short)last;
	function_2959c0();
	function_x66da2b(&iterator, true);
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
	{
		if (actor[0xd])
		{
			long index = iterator.actor_index;
			byte *current = (byte *)actor_get(index);
			long unit = *(long *)(current + 0x18), proxy = *(long *)(current + 0x1c);
			function_1e2a90(index);
			if (unit != NONE) function_b8540(unit);
			else if (proxy != NONE) swarm_delete(proxy);
		}
		else if (*(short *)(actor + 0x84) > 0)
		{
			long index = iterator.actor_index;
			g_46fc80 = index;
			if (function_1e35f0(index)) function_1e3b60(index);
		}
	}
	function_x66da2b(&iterator, true);
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
		if (*(short *)(actor + 0x84) > 0) function_1e34e0(iterator.actor_index);
}

struct s_actor_position_state
{
	s_actor_position_state();
	volatile long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real field14, field18;
	long field1c;
	short field20, field22, field24;
	byte unknown26[2];
	short field28, field2a, field2c, field2e;
	long field30, field34;
	real field38;
	short field3c, field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
};
bool function_1e11b0(long actor_index, bool active);
bool function_e58e0(long unit_index);
bool function_109e00(long object_index, vector3f *velocity, bool required);
void swarm_apply_impulse(long perception_index, vector3f const *velocity);

// @retail 0x1e0fe0
long function_1e0fe0(point3f const *point, long unit_index, long character_index, short mode)
{
	byte *object = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
	short const *mode_reference = &mode;
	long squad_index = NONE;
	byte cleanup = false;
	s_actor_position_state placement;
	placement.position = *point;
	placement.field10 = NONE;
	placement.field14 = 0.0f;
	placement.field18 = 0.0f;
	placement.field1c = 2;
	short local_792ecd = *mode_reference;
	placement.field2e = local_792ecd > 0 ? local_792ecd : 3;
	long previous = *(long *)(object + 0x12c);
	if (previous != NONE)
	{
		byte *actor = (byte *)actor_get(previous);
		squad_index = *(long *)(actor + 0x30);
		cleanup = actor[0xc];
	}
	long result = function_1e0850(character_index, squad_index, (s_203780 const *)&placement, false);
	if (result != NONE)
	{
		byte *actor = (byte *)actor_get(result);
		if (*(long *)(actor + 0x30) == NONE)
		{
			*(short *)(actor + 0x2c) = g_510c54->field_2_3;
			actor[0x3c] = true;
			function_1e11b0(result, true);
		}
		actor[0xc] = cleanup;
		if (actor[7] && *(long *)(actor + 0x1c) != NONE)
		{
			dword mask = 1;
			byte *type_object = object;
			mask <<= type_object[0xaa];
			if ((mask & 1) && function_e58e0(unit_index))
			{
				vector3f velocity;
				if (function_109e00(unit_index, &velocity, true))
					swarm_apply_impulse(*(long *)(actor + 0x1c), &velocity);
			}
		}
	}
	return result;
}

// @retail 0x1e2930
void __stdcall function_1e2930(long object_index, long actor_index)
{
	long const *actor_reference = &actor_index;
	s_ai_object *object = ai_object_get(object_index);
	if (*(real *)(g_4e3b44[object->definition_index & 0xffff].bytes + 0xd0) > 0.0f)
		function_25d020(*actor_reference, object_index, 4, NONE, NONE, NONE);
}

void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void __stdcall function_b8540(long object_index);
void __stdcall swarm_delete(long index);
void function_1e2a90(long actor_index);
bool actor_update_bsp_locations(long actor_index);
void function_203ed0(long actor_index, bool keep_count);
void function_203f60(long actor_index);

// @retail 0x1e35f0
bool function_1e35f0(long actor_index)
{
	bool actor_active = actor_get(actor_index)->unknown007;
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;
	if (actor_active && *(long *)((byte *)actor + 0x1c) == NONE)
	{
		long object_index = actor->unknown018;
		long const *other_field = (long *)((byte *)actor + 0x1c);
		volatile long other_index = *other_field;
		function_1e2a90(actor_index);
		if (object_index != NONE)
			function_b8540(object_index);
		else
		{
			long captured_index = other_index;
			if (captured_index != NONE)
				swarm_delete(captured_index);
		}
		result = false;
	}
	else
		*((byte *)actor + 0x506) = 0;
	if (*(short *)((byte *)actor + 0x3e) != g_4686c4 && !actor_update_bsp_locations(actor_index))
	{
		*(long *)((byte *)actor + 0x34) = actor->unknown030;
		function_203ed0(actor_index, false);
		function_203f60(actor_index);
		s_actor_view *current = actor_get(actor_index);
		if (current->unknown009)
		{
			current->unknown009 = false;
			*(long *)((byte *)current + 0x10) = g_510c54->game_time;
			--*(short *)((byte *)g_4f55d0 + 0x36a);
		}
		result = false;
	}
	*((byte *)actor + 6) = result;
	return result;
}

class c_1e2990
{
public:
	__declspec(noinline) void function_1e2990(bool preserve);
};

// @retail 0x1e2990
void c_1e2990::function_1e2990(bool preserve)
{
	s_actor_view *actor = actor_get((long)this);
	long object_index = actor->unknown018;
	long other_index = *(long *)((byte *)actor + 0x1c);
	function_1e2a90((long)this);
	if (object_index != NONE)
	{
		if (preserve)
		{
			function_b83b0(object_index, true);
			function_b8460(object_index, true);
		}
		else
			function_b8540(object_index);
	}
	else if (other_index != NONE)
		swarm_delete(other_index);
}

#include "unknown_0259a0.h"
#include "hud_messages.h"
extern bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);
void __stdcall function_1e1a00(long actor_index, bool value);
long function_1e1f20(long actor_index);
void *function_1e5240(long actor_index);
PRIVATE inline long equipment_current_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = unit->current_weapon;
	return slot != NONE ? unit->weapons[slot] : NONE;
}

PRIVATE inline long equipment_secondary_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = *((signed char *)unit + 0x213);
	return slot != NONE ? unit->weapons[slot] : NONE;
}

// MiniMax B (11-20): no new form, no lower score (251): retail keeps the seed global in EBX across the out-of-line draw at 0x259a0, and that callee is opaque to this build because its file is built without link-time inlining, so the seed reloads here; the actor, the weapon options and the object-pool global still take different registers (ours actor EDI, options ESI, pool reloaded).
// score 251 (was 251): retail holds the object-pool global in EDI from the options lookup on (actor ESI, options EBP);
// ours reloads it at each lookup, so the actor and the options take EDI and ESI. The forms tried follow.
// MiniMax G (11-18), no new form, no lower score (251): retail loads the object-pool global into EDI above the
// options lookup's `je` and keeps that base for every field read ([edi+0x84], [edi+0x18], [edi+0x225],
// [edi+0x722], [edi+0x764]), while ours holds the actor in EDI and re-reads the global for the second lookup; the
// actor and the weapon options take the other two registers (ours actor EDI, options ESI; retail actor ESI,
// options EBP), and the unit pointer still rides across the opaque callee 0x1e53e0, so the residency is settled
// in another file (src/unknown_1e53e0.cpp) together with the /GL- build of 0x259a0 (src/unknown_0259a0.cpp).
// MiniMax H (11-18): no new form, no lower score (251): the pool and seed globals still take retail's registers the
// other way round (retail holds the pool in EDI and reloads the seed into EBX), the two-weapon index is still
// strength-reduced into a cursor and a countdown where retail keeps a 32-bit index in the frame's slot at +0x14 and
// reads the pair at +0x1c, retail re-derives the unit from the pool for each of the two slot queries where this build
// keeps one unit pointer, and retail gives the grenade options pointer a frame home that the random draw then reuses
// where this build keeps it in a register; the 0x259a0 call is opaque to this build because src/unknown_0259a0.cpp is
// built /GL-, which is another lane's file; the permuter (--fast, 300 s, 18 tries) reached 246, a five-point gain that
// is under the keep bar, and its variant swaps 0xc6f80's arguments and adds a constant-only local.
// MiniMax R run 92 (11-17): no new form, no lower score (251): the pool global is still re-read after each lookup
// where retail holds it in EDI across them, and the actor and options still take the other two registers (ours:
// actor EDI, options ESI; retail: actor ESI, options EBP); the random-state load still rides across the opaque call
// 0x259a0 in src/unknown_0259a0.cpp, which this lane may not edit.
// MiniMax A (11-18): no new form, no lower score (251): the permuter's tries leave the baseline at 251, and the actor, the weapon options and the object-pool global still rotate against retail (retail ESI/EBP/EDI, ours EDI/ESI); the residency is decided by the opaque callee 0x1e53e0 in another file.
// The 0x14 frame and actor index in EBX agree. Retail loads the pool global into EDI above the
// options lookup's `je` and holds it across 0x1e5280 (actor ESI, options EBP); ours re-reads it (actor EDI, options ESI).
// MiniMax E (11-16): no new form, no lower score (251): the permuter (1500 s, 131 tries, one job) left the baseline at 251, so the pool global is still re-read after each lookup where retail holds it in EDI across the callees, and the actor, options and pool registers still rotate the way the notes below say.
// MiniMax R (11-17): no new form, no lower score (251): the permuter's 20 tries over the reduced image (311.7 s) reach
// 246 in the full image as well, a five-point gain that is under the keep bar, so nothing was written; what still
// differs is that retail holds the pool global in EDI and reloads the seed into EBX where this build keeps them the
// other way round, the two-weapon index is spilled, and the random-state load rides across the opaque /GL- call
// 0x259a0 in src/unknown_0259a0.cpp.
// MiniMax D (11-15): no new form, no lower score (251): naming the object-pool global in a local does not make it live in a
// register, because the compiler reads the global again after any call it cannot prove pure (the opaque callee in another
// file the notes above name): the local after the weapon options measures 278 (the copy is spilled to a stack slot and the
// frame grows to 0x18), the local first measures 283 (the load moves above the actor), the local holding the header array
// measures 301 (both loads move to the top), and the local used in the first block only compiles to the same 251 code.
// MiniMax G (11-14): no new form, no lower score (251): the actor, options and pool registers rotate against retail and that rides on the opaque callee 0x1e53e0 in another file.
// M3 lane 8 r159 (fast): the halved rounds as `rounds[0] *= 0.5f;` 251 (same code). Re-measured: the loop's weapon in a
// named `long weapon` 251 (same code; round 126's weapon entry), the raised probability as `4.0f * probability` 251 (same
// code; round 46). No other natural form found that the notes above do not list: the levers left are 0x1e53e0's purity
// (round 38: retail's LTCG treats it as writing memory), the /GL- build of 0x259a0's file (lane 3 r233; flags are the
// owner's call) and header fields for +0x213/+0x722/+0x764 (same code at best).
// MiniMax A (11-12): no new form, no lower score (251): taking the unit pointer from the first
// lookup instead of the repeated ai_object_get calls gives 361 (1171 bytes), so the repeated lookups
// stay; the actor, weapon-options and object-pool registers still rotate against retail (ours actor
// EDI, options ESI, the pool global re-read at every lookup; retail actor ESI, options EBP, the pool
// global held in EDI across the callees), and that difference rides on the opaque callee the notes
// above name.
// Earlier status line, 251 (was 251): Frame 0x14 and the actor index in EBX agree; the actor, options and object-pool registers
// rotate (ours actor EDI, options ESI, pool never held; retail actor ESI, options EBP, pool global in EDI) and scalar homes remain.
// MiniMax E (11-10): no new form, no lower score (251); the ranges, clamp bounds, the x87 fraction and every store target agree with retail, and what is left is where the unit pointers live: retail keeps the index-derived unit pointer in esi and holds the object-pool global in edi across the calls, this build keeps the index in edi, puts the unit pointer in esi where retail has ebp, and re-reads the pool global at every lookup.
// MiniMax H (11-06): retail keeps the actor in ESI and the object pool global in EDI across the callees and
// re-reads the pool's data at every lookup, ours keeps the actor in EDI and re-reads the pool global itself;
// the difference rides on the callee being opaque to LTCG, and that callee (0x1e53e0) is in another file.
// MiniMax H (11-06): the pool pointer as a local, declared before or after the actor, gives retail's shape of
// a held pool pointer, but the pool lands in ESI, the actor stays on the stack and the frame grows to 0x18, 283;
// the score stays 251.
// Earlier: Frame 0x14 and actor ESI agree; the probability and delay clamps now match
// retail's code; the actor index stays in EDI instead of EBX and the object pool is reloaded instead
// Frame 0x14 and actor ESI agree; input EBX/object-pool EDI allocation and scalar/ammunition homes remain.
// Earlier: probability/delay clamps agree; the actor index stays in EDI instead of EBX and the object pool is reloaded instead
// of kept in a register.
// Tried ammo clear before value 314; two weapon for 314; shared fraction range 308; probability clamp branches 322; delay clamp branches 345; shared range native homes 308.
// Equipment-phase unit handle 308; weapon-pair entry workspace 308; rounds-pair entry workspace 308;
// word two-weapon counter 299 kept (plain); promoted minimum 316.
// Round 115: Probability field reads 288 kept (plain); delay field reads 292; weapon array initializer 299; initial lookup scope 299; increased-probability selection 299; both field clamps 291.
// Round 124: Signed tick result 288; weapon-array cursor 302; empty-weapon continue 288; group handle 290;
// consistent minimum field reads 284 kept plain; with word random or unsigned-word ordinal 284/284;
// with native long ordinal 328; lower-default probability join 288, with minimum reads 284.
// Round 126: Weapon entry/native unit fields/existing random step/nested grenade guard 284 each; optional
// settings initialization 317; word admission minimum 288; magazine memset 282; complete probability
// initialization 309; memset with long ordinal 328 or word minimum 279 (plain kept); combined candidate
// with long ordinal 300 or weapon entry 279 (fast).
// force-inline existing pair of equipment queries 279; native byte current equipment slot 296; native
// signed byte secondary equipment slot 296; both native byte equipment slots 299; both equipment query
// conditional results 270; both equipment query rejection returns 275; existing equipment queries consume
// actual unit record 279; actor record lexical identity on native frame 279.
// Round 224: Native/unsigned-word ordinal 318/270; complete output initialization 271; consistent
// delay-field reads 282; settings default after query 272; signed-word duration 270; branch-owned
// probability 279; native ordinal with delay reads 295. Frame agrees; index/pool and scalar homes
// remain.
// Round 226: Actual weapon/grenade/both metadata 270/270/270; native unit fields 270;
// unit fields with weapon/grenade/both metadata 270/270/270; complete readonly metadata 270.
// Round 206: Shared unit receiver 270; with entry weapon/ammunition/both output lifetimes 270/270/270;
// both output lifetimes 270; shared unit with entry grenade receiver, weapon/grenade or all workspaces 270 each.
// Round 212: Consistent actor getter observations 352; with long ordinal 327; fixed-two for 352;
// long ordinal with for 327; output initializer 351; initializer with long/for/both 327/351/327.
// Round 145: Both ammunition bounds captured, lower/upper/ordinal short-short-short 326, short-
// short-long 339, long-short-short 333, long-short-long 347, short-long-short 332, short-long-long
// 340, long-long-short 334, long-long-long 348; all restored.
// Round 230: Actor after weapon/settings queries 277/308; shared current/secondary/both
// slot queries 329/317/349; both queries with later actor 352/308; direct shared queries 279.
// Uninitialized probability/delay at entry 270/270; constant probability at entry/after query 295/286;
// with the other scalar workspaces 295/286; scalar-role names with early constant 295,
// with uninitialized entry workspaces 270. Original declarations, initializers and names restored.
// Scalar-home names for probability, fraction, delay, ticks, minimum and loop index,
// plus both grouped renames: 270 each.
// Round 351: bounded equipment for, actual pair initializer, early constant ordinal and
// their four combinations 270/270/270/270/270/270/270; complete graph with word ordinal 270. All restored.
// Round 250: Conditional-result query record inputs: current/secondary/both/both const 270/270/270/270;
// both with long slots mutable/const 293/293; both with signed-byte slots mutable/const 268/268 (fast
// control 270). Two-point gain rejected; original scalar-index inputs and short slots restored. Frame
// 0x14 agrees; index/pool roles and scalar/ammunition homes remain.
// Round 22: existing object/header/moving/nested-object accessors 263 each; actor-index
// address read 263; split/reversed entry declarations 263/263; const actor index 263.
// Round 280: shared random-state receiver at entry/after actor/before query/after
// query 340/340/340/333; shared seed-field receiver at those same acquisition
// points 340/340/340/333. Every draw retained; all original receivers restored.
// Round 29: Current/secondary result and actual weapon-pair dword domains in all seven combinations, then the real word ordinal: 263 each. Original result/pair types, ordinal and all code restored.
// Round 111: opening through existing 0x1e5240 (retail inlines it) 263, kept; tail through existing
// element_502420_get 272; current slot through existing unit_get_current_weapon with a result-first
// secondary query 275 (retail's query shape, but ours keeps the unit across 0x1e53e0 where retail
// re-reads it, so the pool global never gets EDI), current slot alone 270; weapons array declared
// first/second 263/263.
// Round 164: nonredundant actor/unit gates, draw-first ordered admission, their seven
// combinations and all with readonly actor 263 each; all original observations restored.
// Ammunition span named at its arithmetic producer, with options/probability/delay result-production combinations 263/310/282/315/263/310/282/315; original expressions and result producers restored.
// Round 249: plain native two-slot shared-query traversal, long/short/word/byte
// ordinal 275/324/324/324; existing unit receiver with those domains 275/324/324/324.
// Original private queries, native object declaration and complete body restored.
// Round 414: current loop weapon captured 263; short/long scalar ammunition count 266/265;
// weapon plus those counts 266/265; private output clear before draw, alone/with weapon/with
// weapon and short count 263/263/266. Original producers, clear order and complete body restored.
// Tried (MiniMax I run 17): 0x1e5240's body (weapon index, record lookup, 0x1e5280) and ai_object_get's pool
// indexing written out by hand 231; hand copies of existing functions are not kept without a match.
// DL210: byte/signed-byte/unsigned-dword ammunition ordinals 263/263/323; paired real-array
// role names 263, with those ordinals 263/263/323; readonly real weapon input pair 263.
// Original ordinal, array names, mutability and complete body restored.
// Round 52 (lane 16): the ammunition minimum read from the options field at each use (no `minimum` local) 263 ->
// 251 plain (kept; the actor index moves to retail's EBX). On 251 (fast): the random draw written inside the rounds
// expression 251; the two slot helpers reading the slot field at each use 251; probability defaulting to 0.1 before
// the options test 288; the weapon pair as an initializer 251; the options lookup before the actor pointer 300; a
// named seed pointer for every random draw (on 263) 333; the counter increment as `+= 1` (on 263) 263 (same code);
// the weapon pair declared at function scope (on 263) 263 (same code). Our callee closure has no stub and nothing
// writes or takes the address of the object-pool global, so retail's held pool register is allocation, not a blocker.
// Lane 9 r52 (fast): retail loads the object-pool global once into EDI above the inlined 0x1e5240's NONE
// test and re-reads its data field at every lookup; ours CSEs the data field and reloads the global (the
// callees in that span are leaves, so it is not a callee summary). `inline` on 0x1e53e0 265 (0x2758b0
// 21 -> 113); the pool read into a local before 0x1e5240's test 263 (same code); NONE-first weapon query
// results 275; the lookups through a plain static getter taking the pool 263 (same code).
// Round 42 (m3 lane 13, fast): retail loads the pool global between the inlined 0x1e5240's NONE compare and its
// branch and holds it in EDI to the end (actor ESI, options EBP, the random state EBX from its first draw); ours
// never carries the global across 0x1e5280 (it reloads it at each lookup, so the actor takes EDI, the options ESI
// and the random state EBP). Diagnostics with the lookups indexed by hand through a pool local (not keepable):
// the local at the top 283 (1237 bytes; pool held in ESI but spilled, options EBP as retail), after the actor 283,
// after the 0x1e5240 call 278 (loaded after 0x1e5280, spilled). A pool local before the NONE test inside 0x1e5240
// (0x1e5240 still MATCH) 251 (same code here: the caller's lookups stay separate loads). The unit-index test split
// from the actor state tests 251 (same code).
// Round 38 (m3 lane 14, fast): retail re-fetches the unit after 0x1e53e0 (the two equipment queries' lookups)
// where ours keeps it in EBX, i.e. retail's LTCG treats 0x1e53e0 as possibly writing memory and ours as pure.
// Diagnostic (reverted at once): a function-static store in 0x1e53e0 gives this body retail's shape (actor in ESI,
// the pool global loaded once and held, the unit re-read after the call; aligned 183 -> 167) but 265 at 1247
// bytes, the pool in EBP and the options in EDI (retail EDI/EBP) and the grenade test laid out longer; its other
// callers keep their scores (0x1e04a0, 0x1e5380 match, 0x2758b0 21). Lead: a 0x1e53e0 form with the same bytes
// that LTCG cannot prove pure. Removing the 0x1e1a00, 0xc6f80 or weapon calls (diagnostics) does not make ours
// hold the pool. Forms: ticks as a short 251; a named group index for the counter 251; the grenade test by De
// Morgan 251; the weapon pair declared before the unit 251; `fraction = fraction * 0.5f` 251 (all same code).
// Lane 18 round 24 (fast): both private slot queries result-first (retail's query shape) 280; the tail through the
// existing element_502420_get 251 (same code); a `long` two-weapon counter 278 (1191 bytes, frame 0x10); the unit index
// read once into a named local for the second block 251 (same code); the loop as `for (short i = 0; i < 2; i++)` 251
// (same code). Diagnostic (plain, reverted): src/character_tag_accessors.cpp renamed to link after this file, as
// retail's layout does, leaves this body at 251 (and costs 0x73360), so callee link order is not the purity cause.
// DL234: actual grenade-reset admission produced as bool/byte/short/long, complete
// condition or guarded result: 251 each. Original predicate and body restored; actor/options,
// retained pool and scalar homes remain. Shared query summaries and all draw guards are unchanged.
// Round 295: per-use unit receiver, first phase 306; second 254; both 315; private queries 330;
// first/private 360; second/private 337; all 372; all with long ordinal 361 (fast). All restored.
// M297: Current-only/secondary-only default-result, failure-exit and complete
// branch-result producers, then mixed default/failure and failure/default
// [279, 251, 251, 251, 251, 251, 278, 278]. Both original private queries restored.
// N302: Shared two-consumer ammunition-reduction predicate bool/byte/short/long,
// per equipment phase / per valid weapon: 278/278/246/245/255/255/326/328. Exact-only
// observations and all original code restored; frame/input agree, pool/actor/options homes remain.
// DH282: Complete probability-default/increased-selection/delay-default ownership, seven subsets then all
// with native signed-word tick storage [251, 251, 251, 251, 251, 251, 251, 251]. Original executable body
// restored.
// Round 259: One actor unit handle throughout both phases 347; with current/secondary/both
// default query results 348/352/356, current/secondary/both rejection results
// 352/352/350, mixed default/rejection 352. Original body and both queries
// restored; actor/options/pool residency and scalar homes remain.
// DL241: Original indexed-query current/secondary long/dword slot-domain pairs
// 269/268/272/269/268/272/272/272; numeric ordinal plus short/word/dword ticks 278 each, with real
// pair initializer 277. All regress. Original two private query bodies, ordinal, tick storage and
// complete body restored; actor/options/pool and scalar homes remain.
// F338: Common final tail reached directly on absent equipment, crossed with
// short/long ordinal, bounded-for/fixed-do traversal and empty-weapon continuation:
// 251/278/251/278/251/278/251/278. All restored; actor/options/pool residency remains.
// Round 46 (fast, with 0x1e3b60): the probability test with its operands swapped 251; the increased
// probability as `if (probability < increased) probability = increased;` 253; the probability clamp with
// the missing-options arm first 251; the delay with the random-default arm first 251; `4.0f * probability`
// 251 (all same code but the 253). Pool EDI / actor ESI / options EBP remain.
// m3 lane 16 r83: plain check without the final 0x1e1a00 call (diagnostic) 289 at 1187 bytes, the pool still
// reloaded at each lookup: that unmatched callee is not what keeps ours from holding the pool. Fast: the
// probability defaulting to 0.1 before the options test 288; the increased probability as one conditional
// 251; the grenade reset's mode test as an outer if 251; the random draw inside the rounds expression 251;
// the delay as one conditional 251 (all same code but the 288). All restored.
// Round 48 (fast with 0x1e3b60): retail also re-derives the unit (actor field, pool data) between the current
// and secondary slot queries, where only the weapons[0] store lies between, so its kill is not 0x1e53e0's
// purity alone. The slot queries writing through an out pointer into weapons[] 251 (same code); `__forceinline`
// on both queries 251 (same code); the fraction bounds as named locals 262; the grenade reset test split into an
// outer `state != 1 || !flag` if 251 (same code); the grenade type through a fresh lookup and `unit` fetched
// after the queries 251 (same code). All restored.
// P321: Actual actor-readiness result in bool/byte/short/long, producer/entry
// declarations with original post-query observation: 251/251/251/251/251/251/251/251.
// NOT kept: no native actor/options/pool residence recovered; full body restored.
// B421: Shared three-consumer options admission bool/byte/short/long, positive/
// complementary results 310/310/321/308/310/310/321/308. NOT kept: all regress
// and exceed retail length; original pointer tests, queries and complete body restored.
// Y577: Shared ammunition output at phase/query scope with end-bound do, bounded for,
// short-ordinal cursor and fixed-count cursor: 278/278/251/251/278/278/251/251. NOT kept: ties
// or regression; original full body restored. Native actor/options/pool and scalar homes remain.
// Y584: Existing slot-query real-field reference/direct-read ownership, independent
// current/secondary pairs and readonly pair: 251/258/251/251/258/251/251/251.
// Original signed-word snapshots, both queries and full body restored; pool/actor/options homes remain.
// P329: Equipment/grenade/unit receiver identities, seven subsets then actor identity:
// 251/251/251/251/251/251/251/251. NOT kept: no byte match; original complete body restored.
// Actor/options and retained-pool homes remain.
// P335: Standard min/max probability/increased/delay producers, seven subsets and
// complete graph with final maximum consumer: 322/325/275/354/323/323/346/395. NOT kept: all regress;
// original clamps, field observations, includes and complete body restored. Actor/options/pool homes remain.
// B449: Repeated unit-handle/fraction-bound/round-bound field addresses, seven subsets
// then complete readonly actor receiver: 361/251/361/251/361/251/361/361.
// NOT kept: no exact field-receiver repair; all original per-use observations, queries and body restored.
// P350: Shared real clamp at probability/delay/both, ordinary/inline, then forced single sites:
// 256/284/286/256/284/286/256/284. NOT kept: regressions; original per-use clamps and body restored.
// Y602: Readonly equipment/grenade settings and fixed unit receivers, then fixed settings pointers:
// 251/251/251/251/251/251/251/251.
// NOT kept: no byte match or qualifying native-shape repair; original complete body and helpers restored.
// b5E (11-06): statement-by-statement walk against retail 0x1e2a90, no behaviour difference found: ranges,
// clamp bounds (0.1/0.6, 0.8/1.3), the 4.0 and 3.0 constants, the x87 fraction (random*(hi-lo)+lo rounded to float at
// fstp 0x1e2e48), the (hi+1-lo)*random>>16 rounds, the 0.5 halving, store targets (unit+0x1f5 low byte, +0x23e) and
// condition directions all agree. Raw offsets replaced by existing fields (s_ai_object flags134/unknown23c,
// s_ai_globals unknown340): 251 (same code). Remaining difference is pool/actor/options residency only.
// Round 78 (m3 lane 13, fast; register-masked aligned 60): retail loads the pool global into EDI above 0x1e5240's NONE
// branch (0x1e5280 is a pure leaf, so this is allocation, not a callee summary), re-derives the unit after 0x1e53e0 and
// again after the weapons[0] store, keeps the loop counter in a stack home (EDI reused inside), and passes the weapon
// to 0x101a10 in EBX (the random-state pointer is reloaded after it). Tried: the block-2 queries through the existing
// 0xcbd50 280 (re-measure of lane 12 r107), with a long counter 333 (1186 bytes); 0x1e53e0 with `return entry;`
// in the loop and `return NULL;` (no goto; retail's two epilogues) 304 (0x1e53e0 becomes 81 bytes and loses its match).
// Struct views (load CSE follows the view): block 2's unit through the existing moving_object_get with raw +0x23c/+0x23e
// 251 (same code); the block-2 queries taking the unit index from the existing actor_moving_get(actor_index) 334 at 1261
// bytes (register-masked aligned 60 -> 45: the field is re-read as retail's, but the actor base is re-derived and the frame
// grows to 0x18); every block-2 unit-index read through it 324 (masked 72).
// m3 lane 8 r146 (fast; aligned/masked base 183/74): diagnostic, a store through a local reference in matched 0x1e53e0 (one-element
// array, bytes unchanged; 0x1e04a0, 0x1e5380 MATCH, 0x2758b0 21) 265 at 1247 bytes, aligned 167, masked 48 (the pool global held in
// EDI, the unit re-read after the call). On it: both slot queries result-first (`long result = NONE; if (slot != NONE) ...`) 300
// (1240, aligned 158, masked 58; the current-slot query gets retail's rows, the secondary one still reuses the cached unit); the
// existing unit_get_current_weapon and unit_weapon_view_get for the two queries 300 (same code); a `long` loop index (do and for) 294
// (1219, frame 0x10, aligned 141, masked 63); a compiler barrier between the queries (diagnostic) 326 (1249, masked 54). Retail
// re-derives the unit between the two queries with no call between them, so a store in the caller's own IL kills it there too.
// Lane 12 r105 (fast with 0x1e5240/0x1e5280/0x1e53e0/0x1e04a0/0x1e5380/0x2758b0): a register-masked aligned diff (87 lines)
// leaves one non-register difference: retail's two-weapon loop keeps a 32-bit index (`mov ecx, [esp + edi*4 + 0x1c]`, `inc edi;
// cmp edi, 2`, the counter homed at [esp+0x14]) where our short counter is strength-reduced into a cursor and a countdown; retail
// also keeps the fild temporary at [esp+0x18] apart from the rounds pair at [esp+0x10]. The long counter re-measured 278 (1191 B,
// frame 0x10: it gives retail's indexed loop, but the fild temporary then shares the rounds slot); with the rounds pair declared at
// the top of the loop body 278, beside the weapon pair 278. New: the five seed reads through an inline getter 251; the direct
// unit lookups through the existing OBJECT_DATUM_GET macro (own-IL global reads) 251, with the two slot queries through it too
// 251 (same code each). Diagnostic (reverted at once): a function-static store in both 0x1e5280 and 0x1e53e0 274 (1247 B).
// Lane 12 r107 (fast): retail's second-block query code is 0xcbd50's body inlined (`or reg, -1; cmp slot, -1; je; movsx;
// mov [unit + slot*4 + 0x218]`). Both second-block queries through the existing 0xcbd50 with the slot read from a fresh lookup
// 280 (retail's `or -1` shape, but the unit stays CSE'd across 0x1e53e0 and the actor index moves to EDI); that with a `long`
// counter 333 (1186 B); with the slot read from the block's unit 280; with no unit local in the second block (grenade type and
// the +0x23e store through fresh lookups) 279 (1227 B); the first block's two 0xcbd50 slot arguments read from `unit` 361
// (1171 B). Diagnostic: the first block's unit through slot_handler.h's object_get view 251 (same code: the pool data load is
// CSE'd across views). Re-measured: the `long` counter 278 (1191 B, frame 0x10).
// M3 lane 8 r152 (fast; masked aligned 44): the clump counter as the header type's element field (`((s_clump *)g_502420->data)
// [index].unknown3e++`) 251 (same code; retail forms the element address before `inc word ptr [eax+0x3e]`, ours leaves a dead
// `lea` after the `inc`); the rounds count computed and halved in a named short before it goes into the zeroed pair 255, with a
// `long` index 288 (1181 B); the probability as one conditional including the options test 251; the rounds bounds as named shorts
// 323. Compile-only identical: the first block's tests nested (two forms), the grenade compare's operands swapped, `!= NULL`.
// Re-measured: both second-block queries through 0xcbd50 with a fresh slot lookup 280 (LTCG inlines it at that depth, giving
// retail's `or -1` shape), with a `long` index 333. What remains is the pool and seed globals' register weights (retail holds the
// pool in EDI and reloads the seed into EBX; ours the reverse) and the spilled 32-bit index. Body unchanged.
// m3 lane 3 r233 (fast, base 251, aligned 135, register-masked 57). Likely contributor, not confirmed: src/unknown_0259a0.cpp
// is built /GL-, so the first block's 0x259a0 call is opaque to the link: ours loads the random-state global for that call
// and again after it, and with the 298 form below the pool global is reloaded after it too (retail loads the random state
// once into EBX ahead of the call and keeps the pool in EDI across it). Diagnostic (reverted at once): that file without /GL-
// 306, worse (the random state then rides in EDX across 0x259a0 and 0x259d0; the pool still not held). Flags are the owner's
// call: lead only.
// Forms: the secondary weapon through the existing 0x1e1fd0(actor_index) 272 (1195 B, it stays a call); the matched
// siblings' idiom (0x1e1f20/0x1e1fd0: `long unit_index = actor->unknown018;` then 0xcbd50(unit_index, slot read through
// a fresh lookup)) for the first block's two calls 361 (1171 B, the score and length of lane 12 r107's listed slot arguments
// read from `unit`, likely the same code); for the second block's two queries through 0xcbd50 298 at
// 1232 B but aligned 92, masked 51: actor ESI and options EBP as retail, and the pool global loaded once above 0x1e5240's
// `je` and held through the first block (in EBX, retail EDI), then reloaded after the opaque 0x259a0 call; the same with
// `unit_index` declared at the top of the second block 309 (aligned 155); on both blocks 315 (1216 B, aligned 155); the
// queries' lookups through `unit_index` too 294 (1229 B, aligned 124, masked 51). Lead: the 298 form plus a transparent
// 0x259a0. Body unchanged.
// MiniMax D (11-11): no new form, no lower score (251): the pool and seed globals still have retail's registers the other
// way round (retail holds the pool in EDI and reloads the seed into EBX) and the two-weapon index is still spilled.
// Round 92 (m3 lane 11, fast): the actor through squads.h's s_actor_datum view (actor_datum_get, raw +0x84) 251 (same
// code); the rounds draw through the existing random_index, its result in a named count 255, inside the expression 252; the
// draw through random_next 251 (same code); random_index inside the expression with both second-block queries through 0xcbd50
// (slot read from a fresh lookup) 279.
// P440: First-phase actor/unit/slot failure scopes, seven subsets then exhaustive success arms: 349/353/349/315/353/349/347/347.
// NOT kept: Phase exits regress; actor/options/pool homes remain. Original complete body restored.
// P459: Complete boost-admission/rejection outcomes, bool/byte/short/long:
// 251/251/251/251/252/252/252/252. NOT kept: native actor/options/pool residence not recovered; original complete body restored.
// MiniMax A (11-13): no new form, no lower score (251): the permuter reaches 246 after five tries, five points short of the
// bar, so the body stands; the actor, the weapon options and the object-pool global still rotate against retail the way the
// notes above say, and the pool global is still re-read at each lookup instead of held in EDI across the callees.
// M3 lane 11 r99 (fast): the clump counter as a postfix `(*(short *)...)++` 251 (same code: the dead `lea` after the `inc`
// stays); block 2's +0x23e store through a fresh lookup (unit kept for the grenade type) 254; the rounds pair as `short rounds[2]
// = { 0, 0 };` in place of the memset 253; the two-weapon loop as `while (i < 2) { ...; i++; }` 251 (same code); the rounds minimum
// named once (`short minimum`, three uses) with the draw inside the rounds expression 263. Retail reads +0x96 before the seed
// store and keeps +0x94 from the test in DX, i.e. it schedules the bounds' loads across the seed store. Body unchanged.
// P468: Double probability/delay/fraction/increased result storage, then probability-
// increased/probability-delay/first-three/all: 348/302/272/307/328/321/307/304.
// NOT kept: precision/storage graphs regress and miss native actor/options/pool
// residence; original real values, expression groups and complete body restored.
// P483: Existing current/secondary getter results as actual-object references, mutable
// current/secondary/both, readonly current/secondary/both, both full-width slots,
// both byte slots: 251/251/251/251/251/251/272/251. NOT kept: native pool/actor/options
// residence unchanged; original complete body and both private queries restored.
// P491: Repeated probability/delay clamp-field const/mutable references, separate/both,
// then complete const/mutable pointers: 251/251/251/251/251/251/251/251. NOT kept: native actor/options/pool
// residence unrecovered; original direct fields, query helpers and whole body restored.
// M3 lane 1 r104 (fast): the weapon pair as an aggregate initialiser 251 (same code); both private slot queries
// result-first with a `long` loop index 333 (1186 B); block 2's grenade type through a fresh lookup with `unit` fetched
// after the grenade-settings call 251 (same code: the lookup is still CSE'd across the pure call); the options fetched
// before the actor pointer 300; one `long unit_index` for both blocks (no 0xcbd50 swap) 349 (1182 B).
// P518: Complete lazy slot-pair present/missing results, bool/byte/short/long:
// 251/251/251/251/251/251/251/251. NOT kept: no exact actor/options/pool residence;
// original gates, lazy queries, observations and complete body restored.
// M3 lane 11 r105 (fast with 0x1e5240/0x1e5280/0x1e53e0/0x1e04a0/0x1e5380/0x2758b0/0x1e3b60; control 251): `inline` on
// 0x1e5280 251 (same code; 0x1ca2d0 inlines it, 243 -> 295); `inline` on 0x1e5280 and 0x1e53e0 together 277 (0x2758b0 21 ->
// 113); the delay test as `0.0f != field` 289 (1199 B); the fraction bounds as `0.0f < field` 251 (same code); the clump index
// read once into a named local for the tail 251 (same code). Diagnostic (flags, reverted at once): src/unknown_0259a0.cpp
// without /GL- and its body `inline` 306: the random state then rides in EBX from before 0x259a0 through 0x259d0 as retail,
// but the pool global is still reloaded at each lookup. Body unchanged.
// P547: Complete complementary grenade-retention admission in bool/char/signed char/byte/
// short/word/long/dword: eight at 251/1216. NOT kept: native actor/options/pool retention
// remains; original lazy tests, queries, random draws, stores and complete body restored.
// M3 lane 15 r92 (fast with 0x1e5240/0x1e5280/0x1e53e0, all MATCH; control 251): the second block's unit looked up at
// each use (no `unit` local) 254 (1231 B: the actor takes retail's ESI, first difference +0x26, but the lookups after
// 0x1e53e0 are still merged; retail spills the unit to +0x14 for the 0x23e store and re-reads the actor's unit index and
// the pool's data field after the call and again after the first weapon store, i.e. it treats both as writing memory);
// that with a `long` weapon index 255 (1218 B); the weapon pair initialised to NONE first 251 (same code); `unit`
// fetched after the weapon queries 251 (same code); the grenade reset test as nested ifs 251 (same code). Re-measured:
// the `long` weapon index alone 278 (1191 B; retail's index is a long in EDI, spilled to +0x14).
// P560: Existing equipment queries return found/missing status and publish the actual
// weapon output only on success, pointer/reference outputs and both formal orders: eight
// 293 at 1230/1216 bytes. NOT kept: actor/options/pool residence regresses; original private
// queries, signed slots, lazy observations and complete body restored.
// M3 lane 12 r135 (fast; control 251): the callee closure's only opaque function is the /GL- 0x259a0, and no file takes the
// pool global's address. Diagnostic (hand lookups, not keepable): a `long` index with a local pool copy after the options and
// this function's lookups through it 281 (1187 B: the pool load sits where retail's does, after 0x1e1f20 and above the `je`,
// but in EBX and spilled; the unit still rides across 0x1e53e0). Forms: the `long` index with block 2's unit looked up at each
// use 255 (1218 B, frame 0x10); that with block 2's queries written like block 1's (0xcbd50, slot from a fresh lookup) 269
// (1217 B), without the `long` index 279 (listed); the `long` index with `short rounds[2] = { 0, 0 };` 277 (1190 B);
// re-measured: the `long` index alone 278 (1191 B).
// M3 lane 13 r114 (fast; control 251). Diagnostics, reverted at once: 0x1469f0's inline asm replaced by plain C 251
// (the pool global is still reloaded at each lookup), that plus src/unknown_0259a0.cpp without /GL- 306 (still
// reloaded: neither opaque callee is what keeps the pool global out of EDI). Forms: the clump counter through the
// existing element_502420_get 251 (one `mov` fewer, the index in the actor's register as retail; no gain), with `+=
// 1` 251; a per-iteration `long weapon = weapons[i];` 251 (same code); `long weapons[2]` at function scope 251; the
// `long` index with the rounds pair at function scope 278 (1191 B, as the `long` index alone). Body unchanged.
// The actor, options and pool still miss ESI, EBP and EDI. First-phase pool 251; second-phase
// pool 251; separate phase pools 251; pool after grenade metadata 302; first-phase header
// array 251; header array after grenade metadata 301; shared header accessor 251; primary
// weapon result inside the flag/delay gate 251. The late pool and header forms still give the
// actor and options different registers.
// Pool, actor and options registers still differ. Phase-local random receiver sharing:
// probability and delay 327, grenade and fraction 251, both 327, per weapon 251,
// probability and per weapon 327, all equipment draws 251, with probability 324,
// and one receiver through fraction with a fresh round receiver 332.
// M3 lane 16 r142 (fast with 0x1e5240/0x1e5280/0x1e53e0/0x1e04a0/0x1e5380/0x2758b0/0x1e3b60, all MATCH; control 251):
// diagnostic, the tail 0x1e1a00 call dropped 289 (1187 B: the pool global is still reloaded at each lookup, so the
// unmatched tail callee is not what keeps it out of a register); the random-state pointer read once into a local for
// all five draws 302 (1199 B); the options probability read once into a named local for its clamp 256; the two-weapon
// loop as `for (short i = 0; i < 2; i++)` 251 (same code); the raised probability kept by `if (probability <=
// increased)` 253.
// M3 lane 11 r128 (fast with 0x1e5240/0x1e5280/0x1e53e0/0x1e04a0, all MATCH; control 251, aligned 183 by a sequence-aligned
// diff, 74 with registers masked): both slot queries through one inline pair helper writing a `long *` 365 (1166 B), that
// helper with result-first queries and a fresh lookup for each 323 (1165 B); the two query results in named locals ahead of
// the weapons[] stores 267 (1200 B); the grenade type in a named short ahead of the settings call 251 (same code); the
// secondary query stored first 251 (aligned 183, masked 75). Diagnostic on the callee, reverted at once: 0x1e53e0 written
// as a tail recursion over the parent 303 here (1242 B, aligned 175, masked 56: the pool global is then held and the unit
// re-read after the call, as with the earlier store diagnostics), but 0x1e53e0 itself becomes 82 bytes with the key in DX,
// and 0x1e5380 2, 0x1e04a0 6 and 0x2758b0 56 lose their code: retail's 0x1e53e0 is a real loop, so recursion is not
// what makes it look impure to its callers.
// M3 lane 14 r120 (fast unless said; control 251). Diagnostic, plain check, body replaced by one comparison of each
// global before and after each callee: only the out-of-line draw 0x259a0 (its file is built without LTCG) and the tail
// 0x1e1a00 are taken to write the pool or the seed global; 0x1e1f20, 0x1e5280, 0xcbd50, 0x259d0, 0x1469f0, 0xc6f80,
// 0x1e53e0, 0x101a10 and 0x1018b0 are not. Retail keeps the seed global in EBX across the 0x259a0 call (loaded before
// it, again on the path round the first block and after 0x101a10 takes EBX for its argument), which ours cannot do
// while that call is opaque: BLOCKED-BY 0x259a0 (build flags of src/unknown_0259a0.cpp, not this lane's to change).
// Forms: block 2's two queries as one private two-argument slot query shaped like 0xcbd50 (result first, slot read at
// the call) 280 (1216 B: retail's `or reg, -1` / test / load per query, but both lookups merge with `unit` across
// 0x1e53e0); that with a `long` index 333 (1186 B). A local copy of the pool global declared after the options, every
// lookup of this function written through it and the slot query taking it: 266 (1216 B, frame 0x18: the copy loads
// into EDI above the `je` as retail and the options lookup reads [edi+0x44], then it is spilled and EDI takes the
// data pointer; actor EBP, options ESI), with a `long` index 323 (1186 B); the copy declared first 326 (1200 B), with
// a `long` index 340. Diagnostic (the draw's file without its no-LTCG flag and the draw kept out of line, reverted at
// once): the body as it stands 306, the four pool-copy forms 269 / 275 (1187 B, aligned 141) / 269 / 275: the copy is
// then propagated back into a reload of the global at each lookup, so retail's EDI is the global itself kept as a
// register candidate, which needs its six separate lookups (retail does not merge the unit's with the queries') to
// outrank the unit and the index. Body unchanged.
// Lazy primary/secondary weapon result by assignment, by conditional, or from an initial NONE,
// and separate slot guard 251 each. Each also scores 251 with a separate valid-unit guard.
// M3 lane 2 r115 (fast with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240 and 0x1e5280; control 251, aligned 57
// with registers masked; body unchanged). Call counts agree with retail (12 each), so no inline decision differs here.
// Re-measured lane 8 r146's diagnostic in a natural-looking spelling: in matched 0x1e53e0 the found entry assigned
// through a reference bound to its own result (`void *&found = result; found = entry;`) keeps 0x1e53e0, 0x2758b0,
// 0x1e04a0 and 0x1e5380 matched and gives this function 265 at 1247 B with the first difference at +0x26 (aligned 48
// masked): the pool global is then held in a callee-saved register and the unit is looked up again after the call and
// after the first weapon store, as retail does. The parent index assigned through a reference to the parameter instead
// loses 0x1e53e0 (43), 0x1e5380 (8), 0x1e04a0 (6) and 0x2758b0 (82) and gives 307 here. On the 265 base: block 2's
// queries through the existing 0xcbd50 with the slot from a fresh lookup 300 (1240 B); the two private queries
// result-first (`long result = NONE; if (slot != NONE) result = ...;`) 299 (1239 B, aligned 97 unmasked, 44 masked);
// those with a `long` weapon index 294 (1218 B); 0xcbd50 with a `long` index 294 (1219 B). Not kept: the reference
// store in 0x1e53e0 is a steering form in a matched function and raises this count. Left on the 265 base: the pool in
// EBP for retail's EDI with the options the other way round, the seed pointer reloaded around the 0x259a0 call
// (BLOCKED-BY 0x259a0's file flags, as recorded above), retail's 32-bit weapon index with its own cell.
// MiniMax A (11-20): no new form, no lower score (251); the pool and the options sit in EDI and ESI where retail keeps ESI and EBP, and the seed pointer must stay in EBX across the opaque 0x259a0 call (BLOCKED-BY 0x259a0).
// m3 lane 4 r235 (fast; aligned by difflib / registers and frame offsets masked in brackets, control 251 [135 / 44]):
// retail loads the pool global once (EDI) and keeps it across every call, re-reading the pool's data at each of six
// lookups, and loads the seed global three times (EBX, once inside the two-weapon loop); this build does the reverse,
// the seed global holding EBP for the whole function and the pool loaded three times. Retail's second block also looks
// the unit up afresh for each of the two weapon slots (the unit index and the pool's data re-read after the first
// slot's store), where the two inline lookups here share one. Forms: the two slots filled through an inline helper
// taking the array 365 (1166 B); stored through a pointer local 279 (1216 B) [137 / 44]; the long loop counter
// re-measured 278 (1191 B) [130 / 42], with the helper 386. Diagnostics, restored at once: without the four
// state-changing calls (0xc6f80, 0x101a10, 0x1018b0, 0x1e1a00) the pool is still loaded per region (340 at 1143 B), so
// no callee's write summary is what keeps it out of EDI; 0x259a0 given a link-time summary (an inline kept out of
// line in an ordinary file) 306 [137 / 49], the pool still loaded three times. No object built without link-time code
// generation refers to the pool, seed or tag globals. Nothing kept.
// M3 lane 3 r292 (H2M_FAST with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240 and 0x1e5280, all MATCH; control 251):
// block 1's two slot queries through the unit already looked up (`unit->current_weapon`, `unit + 0x213`) 361 (1171 B);
// block 2's grenade query reading the type through its own lookup ahead of the `unit` local 251 (same code, CSE'd);
// `short ticks = (short)function_1469f0(delay);` 251 (same code); the options lookup declared ahead of the actor 300
// (first difference +0x0); the two weapon indices as the array's initializer 251 (same code). Body unchanged.
// M3 lane 13 r137 (h2m fast; control 251): a `long` loop index with the weapon read once per pass into `long weapon`
// 278 (1191 B, frame 0x10); the rounds sum written lower bound first 251 (same code); the tick store as a signed
// char like its read 251 (same code); the halved fraction as a conditional argument to 0x101a10 260 (1223 B); the rounds
// pair cleared by `rounds[1] = 0;` in place of the memset 253. Still BLOCKED-BY 0x259a0 (its file's no-LTCG flag).
// M3 lane 5 r238 (h2m fast with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240, 0x1e5280, 0x1e3b60, 0xcbd50,
// 0x1e1f20, 0x1e1fd0 and 0x1ca2d0, all MATCH; control 251, aligned 183 exact / 73 register-masked): the weapon pair
// filled by a loop over the two slot bytes with a `short` counter 323 (1198 B, kept as a loop), with a `long` counter
// 280 (1216 B, unrolled into the two-call form); filled through a cursor 280; both queries through 0xcbd50 with fresh
// slot lookups, the grenade type through a fresh lookup and the unit fetched after the queries 280 (the same code
// each); the pair as an initializer of two 0xcbd50 calls with a `long` loop index 333 (1186 B). Body unchanged.
// M3 lane 6 r239 (fast with 0x1e5240/0x1e5280/0x1e53e0/0x1e04a0/0x1e5380/0x2758b0/0x1e3b60/0x1e1f20/0x1e1fd0; control 251,
// aligned 135): both halving tests with the flag first 251 (aligned 143, other code); the second block's queries through
// 0xcbd50 on a named `unit_index` 280; the rounds pair cleared by `rounds[1] = 0;` 253; the rounds pair declared at the loop
// top 251 and the weapon pair ahead of the grenade lookup 251 (same code each). Body unchanged.
// M3 lane 14 r152 (h2m fast with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240, 0x1e5280, 0x1e3b60 and 0x259a0, all MATCH
// unless said; control 251, sequence-aligned 183 rows). Allocator reading: the three long-lived values are coloured in
// the order options (rank 82, 20 references) > actor (68, 20 references) > the random-state pointer (38), each taking
// the first free callee-saved register (ESI, EDI, EBP); the pool global never becomes a register candidate because its
// lookups are merged into three. Retail's order is actor (ESI), pool (EDI), options (EBP): its actor and pool carry the
// references of six unmerged lookups. A forced choice of EBP for the options alone (diagnostic, not source) leaves the
// count at 257 but brings the aligned distance from 252 to 198, so the options' register is the larger half and the
// pool's residency the rest. Diagnostics on the two callees whose link-time summaries differ from retail's, restored at
// once: `inline` on 0x1e53e0's definition 277 (1216 B, aligned 198; 0x2758b0 21 -> 114, it expands there); 0x259a0's
// file built with link-time code generation and /Ob1 (the draw stays a call) 306 (aligned 183), both 308 (1218 B,
// aligned 189): neither order gives the pool a register here on this body, so the unmerged lookups have to come
// first. Still BLOCKED-BY 0x259a0 (its file's no-LTCG flag) and by 0x1e53e0's summary. Body unchanged.
// Source forms, same round: block 2's two queries in the matched siblings' idiom (0x1e1f20 / 0x1e1fd0: `long unit_index =
// actor->unknown018;` then 0xcbd50 with the slot read through a fresh lookup) re-measured 298 (1232 B), but with stack
// offsets masked the aligned count falls from 213 to 109 (actor ESI, options EBP, the pool loaded once above the `je`);
// that with 0x259a0's file link-visible as above 235 at 1236 B (aligned 113: left are the actor index in EDI for EBX,
// the pool's data held where retail holds the pool and re-reads its data at each lookup, the random-state pointer in
// EDI/EDX for EBX, ticks in a register for retail's home, the unit and the actor's unit index not re-read after the
// first weapon store, the weapon cursor and countdown for retail's 32-bit index, the tail's counter address). A lead
// only: that flag change costs eight matched functions in a full check (0x1a9ec0, 0x1aa9f0, 0x1aea20, 0x1b4560,
// 0x1b54d0, 0x1b7210, 0x1c0300, 0x25b910 need the draw opaque), so the flag stays. Every lookup through an
// out-of-file function taking the pool as a parameter (diagnostic) 251, on the sibling-idiom form 298 (same code each:
// the parameter is substituted, no pool copy survives); the two slot queries as out-of-file functions writing through
// an out pointer (diagnostic) 281 (1247 B, aligned 136), with the link-visible draw 351. On the sibling-idiom form: the
// `unit_index` local serving the first query only (the second reading the actor's field) 294 (1217 B, aligned 128); the
// secondary query stored first 298 (1232 B, aligned 109); the current weapon through the existing 0x1e1f20(actor_index)
// with the secondary through 0xcbd50 285 (1182 B, aligned 136: 0x1e1f20 stays a call).
// M3 lane 5 r245 (h2m fast with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240, 0x1e5280, 0x1e3b60, 0xcbd50, 0x1e1f20
// and 0x1e1fd0, all MATCH; control 251, sequence-aligned 183 rows exact and 97 with registers and stack offsets masked).
// The count is positional and moves against the aligned distance here: the rounds' lower bound read once into a named
// `short minimum` (retail keeps it in DX from the test through the subtraction to the final add) 263 but aligned 136 /
// masked 70; that with the draw inside the rounds expression 263 (same code); with a `long` weapon index 323 (1185 B,
// aligned 124 / 55); with block 2's two queries through 0xcbd50 as well 298 (1186 B, aligned 120 / 48: what is left is
// the pool global held in EDI with its data re-read at each lookup, the seed pointer loaded ahead of the 0x259a0 call
// and the 30 bytes of the three unmerged block-2 lookups); the `long` index with 0xcbd50 but no minimum 333; the
// minimum with 0xcbd50 and the short index 275. Retail's scheduler moves the third lookup's loads above the first
// weapon's store, so that store is not what ends the cached lookup; diagnostic (reverted at once): each block's unit
// assigned through a pointer to the local 298 on the 298 form (same code, the pointer is propagated away).
// M3 lane 7 r234 (fast screen with 0x1e53e0, 0x2758b0, 0x1e04a0, 0x1e5380, 0x1e5240, 0x1e5280 and 0x1e3b60, all MATCH;
// control 251). Allocator reading unchanged: options 82 ahead of the actor 68, the pool never a candidate. Forms: one
// `bool halved` for both halving tests, read ahead of the weapon loop, 278 (1197 B); block 1's first slot query
// through the `unit` already looked up (the second through its own lookup) 340 (1191 B), the second only 337 (1191
// B); the per-pass weapon read into `long weapon` 251 and the grenade reset test as nested ifs 251 (listed forms,
// re-measured: same code); the fraction tests written `0.0f < x` 251 (same code).
// @retail 0x1e2a90
void function_1e2a90(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	byte *weapon_options = (byte *)function_1e5240(actor_index);
	if (actor->unknown084 == 4 && actor->unknown086 >= 3 && actor->unknown018 != NONE)
	{
		s_ai_object *unit = ai_object_get(actor->unknown018);
		if ((unit->flags134 & 1) && *(signed char *)((byte *)unit + 0x1f5) > 0 &&
			(function_cbd50(actor->unknown018, ai_object_get(actor->unknown018)->current_weapon) != NONE ||
			function_cbd50(actor->unknown018, *((signed char *)ai_object_get(actor->unknown018) + 0x213)) != NONE))
		{
			real probability;
			if (weapon_options)
			{
				probability = *(real *)(weapon_options + 0x58) < 0.1f ? 0.1f :
					(*(real *)(weapon_options + 0x58) > 0.6f ? 0.6f : *(real *)(weapon_options + 0x58));
			}
			else
				probability = 0.1f;
			if (actor->unknown225 || (*(short *)((byte *)actor + 0x722) > 0 &&
				*(real *)((byte *)actor + 0x764) < 3.0f))
			{
				real increased = probability * 4.0f;
				if (increased > 0.6f)
					increased = 0.6f;
				probability = probability > increased ? probability : increased;
			}
			if (probability > function_259a0(&g_4e7408->unknown0))
			{
				real delay;
				if (weapon_options && *(real *)(weapon_options + 0x5c) != 0.0f)
				{
					delay = *(real *)(weapon_options + 0x5c) < 0.8f ? 0.8f :
						(*(real *)(weapon_options + 0x5c) > 1.3f ? 1.3f : *(real *)(weapon_options + 0x5c));
				}
				else
					delay = function_259d0(&g_4e7408->unknown0, NULL, 0, 0.8f, 1.3f);
				long ticks = function_1469f0(delay);
				function_c6f80(actor->unknown018, (short)ticks, 0x210000);
				((byte *)unit)[0x1f5] = (byte)ticks;
			}
		}
	}
	if (actor->unknown018 != NONE)
	{
		s_ai_object *unit = ai_object_get(actor->unknown018);
		byte *grenade_options = (byte *)character_grenade_settings_get(actor->unknown054,
			unit->unknown23c);
		long weapons[2];
		weapons[0] = equipment_current_weapon_r21(actor->unknown018);
		weapons[1] = equipment_secondary_weapon_r21(actor->unknown018);
		if (!function_2ac270_get(9) &&
			(!g_4f55d0->unknown340 || (grenade_options &&
			*(real *)(grenade_options + 0x38) > function_x82e52f(&g_4e7408->unknown0, NULL, 0))))
			*(short *)((byte *)unit + 0x23e) = 0;
		if (weapon_options)
		{
			short i = 0;
			do
			{
				if (weapons[i] != NONE)
				{
					if (*(real *)(weapon_options + 0x8c) > 0.0f || *(real *)(weapon_options + 0x90) > 0.0f)
					{
						real fraction = function_259d0(&g_4e7408->unknown0, NULL, 0,
							*(real *)(weapon_options + 0x8c), *(real *)(weapon_options + 0x90));
						if (g_4e6948->state == 1 && g_4f55e0)
							fraction *= 0.5f;
						function_101a10(weapons[i], fraction);
					}
					if (*(short *)(weapon_options + 0x94) > 0 || *(short *)(weapon_options + 0x96) > 0)
					{
						dword random = _random(&g_4e7408->unknown0, NULL, 0);
						short rounds[2];
						memset(rounds, 0, sizeof(rounds));
						rounds[0] = (short)((((short)(*(short *)(weapon_options + 0x96) + 1) - *(short *)(weapon_options + 0x94)) * random >> 16) + *(short *)(weapon_options + 0x94));
						if (g_4e6948->state == 1 && g_4f55e0)
							rounds[0] = (short)(rounds[0] * 0.5f);
						function_1018b0(weapons[i], rounds);
					}
				}
			} while (++i < 2);
		}
	}
	if (actor->unknown07c != NONE)
		++*(short *)(g_502420->data + (actor->unknown07c & 0xffff) * 0x50 + 0x3e);
	function_1e1a00(actor_index, 1);
}
