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


// score 252 (was 252): frame 0x118 and length agree; the -1 sentinel in EDI (retail ESI, with the squad index reloaded into
// ECX), retail's one shared spill slot for the squad pointer, the squad bit and the actor, and the unit arm's place remain.
// M3 lane 8 r159 (fast; masked aligned 115 at 253): kept, the squad bit as `bool flag = squad && ((bits >> 10) & 1);`
// in place of the bool started false and set under `if (squad)` 252 (a re-measure of round 48's form; it removes a flag set
// by a test; masked 119, since retail's code is the store-0-then-store-bit shape). On 252: the extra scope around the body
// dropped 252 (same code, restored to keep the diff small); the failure cleanup as a flat else-if chain 252 (same code); the
// equipment fallbacks testing `squad` first 341 (1264 bytes); the placement view declared before the squad bit 252 (same
// code); the facing block's direction declared before the position call 252 (same code).
// MiniMax A (11-12): no new form, no lower score (253): the two NONE locals declared ahead of the
// definition change nothing; the frame, the length, the callees, the argument order and every field
// access still agree with retail, and the first difference is still the NONE sentinel, which this
// build holds in EDI where retail holds it in ESI, so the squad index takes the other register and
// the blocks after the first call are laid out differently.
// Earlier status line, 253 (was 253): Frame 0x118 and length agree; persistent sentinel EDI/ESI, squad/substitution homes, the unrotated
// substitution loop and the unit arm's placement (retail puts it after the mode-2 case) remain.
// MiniMax E (11-10): no new form, no lower score (253); the fields, conditions, callees and argument order all agree with retail, but the sentinel is held in edi here and in esi there, and the successor and query homes sit at other frame offsets, so the blocks after the first call are laid out differently.
// b5C (fast, 253): statement-by-statement walk against retail 0x1e0850 found no logic difference: every field, offset,
// condition direction, callee/argument order (1e04a0's primary tag in EAX, grenade `(ebx - 1)` sign-extended on the
// data path and `(word)cx - 1` on the squad path), the cos/sin products (fstp dword at 0x1e0a2c/0x1e0a37/0x1e0a42, same
// term order) and the failure exits (0x1e0cfc: unit first, else perception) agree. Kept: the actor and object reads through
// s_actor_view fields and ai_object_get (existing helpers, no new include; 253 either way, callers/callees unchanged).
// Tried: character_definition_get for the tag reads (needs character_tag_accessors.h, 253 same); `for (;;)` with the top
// break (253, same code). Remaining difference is only register/layout as above.
// Earlier: Frame 0x118 agrees; sentinel EDI/ESI, squad/substitution homes and success layout remain.
// Earlier 345 (was 364): frame 0x11c versus 0x118; sentinel and success blocks differ.
// frame 0x118 agrees; entry sentinel, squad and substitution/result homes differ.
// Earlier form: frame 0x11c versus 0x118; sentinel and success blocks differ.
// Frame 0x118 agrees; entry sentinel EDI/ESI and squad/settings/substitution homes remain.
// frame 0x11c versus 0x118; sentinel and success blocks differ.
// Facing products 345/1272; split squad lookup 388; direct substitution gates 369;
// guarded Boolean 384; typed actor fields 364.
// frame 0x11c versus 0x118; missing-index register and squad/settings homes differ.
// Byte creation predicate 383; word predicate 361; short grenade 377; depth one 364; grenade first 364; word pair 375.
// Round 156: depth zero 348; target size 504; table-base reads with complete
// equipment results 346, entry cleanup outputs 342, substitution output order 342.
// Three-point gains rejected; original frame and body retained.
// Equipment base read at use 342 (frame 0x118; three-point gain rejected);
// count reads 350; both 352; selected-character local 380; outer creation buffer 348;
// base read plus Boolean predicate 365, shifted Boolean 364, combined loop guard 438,
// or full-width shifted predicate 350.
// Earlier 320 (was 345): frame 0x118 agrees; sentinel and later register homes differ.
// Base reads with explicit success/cleanup 320; positive flags guard 342;
// complete equipment arms 346; squad-mode word 342; guarded long predicate 350.
// Frame 0x11c versus 0x118 and missing-index homes remain; repaired frame with vertical-first, second-horizontal-first, vector initializer, word creation or word request each 342; three-point gains rejected.
// Earlier 320 (was 320): Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Earlier 320 (was 320): Frame 0x118 agrees; sentinel and settings/squad homes remain.
// Earlier form: frame 0x118 agrees; sentinel and later register homes differ.
// Earlier form: Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Earlier form: Frame 0x118 agrees; sentinel and settings/squad homes remain.
// frame 0x118 agrees; sentinel and later register homes differ.
// Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Frame 0x118 agrees; sentinel and settings/squad homes remain.
// Native Boolean flag 357; guarded Boolean flag 356; late cleanup indices 365; entry placement
// view 320; complete replacement arms 358; late actor result 367; zero grenade guards 339;
// short grenade result 338.
// Frame 0x118 agrees; sentinel, squad/settings homes and success layout remain. Tried scoped
// perception cleanup 366; unit producer cleanup 363; both producer cleanups 347; outer perception cleanup 366;
// both with outer index 347; late actor result 345; Boolean creation mode 378; late unit sentinel 356; perception
// first cleanup 320; independent perception first 337; independent unit first 337; perception first with Boolean
// mode 357.
// Round 152: Shared squad slot 324; retained next character 369; both 342; read-only metadata 320; both/read-only 342; next/query workspace 369; both/query workspace 341; all substitution forms 341.
// Chained cleanup defaults/all three defaults/reverse chain/perception-first defaults 320 each;
// unsigned actor/cleanup/all handles and unsigned chained defaults 320 each.
// Short squad admission 382; identity first 360; horizontal/vertical/both angle inputs 337/321/338; angles plus admission 383; vertical-first inputs 338; angle-home names 338.
// Separate actor/unit/perception creation producers from persistent outputs 320/320/320;
// actor-unit/actor-perception/unit-perception/all producer splits 320/320/320/320;
// all splits with delayed cleanup publication 331. Original output and cleanup ownership restored.
// Round 377: Complete clear-mode bool/byte/short/long results 358/358/337/320;
// guarded producers 349/349/345/328 (fast control 320). Both creation-call values
// preserved; polarity does not recover sentinel or squad homes. All forms restored.
// Native creation/request/both payloads 320/320/320; direct variant field alone/with
// creation/with request/with both 339/339/339/339; direct variant with entry placement view 339.
// Round 176: Complementary substitution result bool/byte/short/long, guarded
// producer 362/362/364/363; complete conditional producer 372/372/375/374.
// Frame 0x118 agrees; sentinel and query-result homes persist. All forms restored.
// Lane 5 r77: retail's substitution loop is top-tested and not rotated (its jmp only skips the
// head's padding), computes the result as `forced ? 0x203900(...) : enabled`, keeps the flag as a
// byte `(flags >> 10) & 1` in the squad pointer's slot, tests grenade indices as ints and reads the
// palette count at each use. Each alone: int grenade tests 349 (frame changes), count at each use
// 336, byte shifted flag 359, ternary result 358 (its block then matches retail); all four 379
// (aligned 233 against 243). All restored.
// Round 149: Substitution producer/test ordinary inline, noinline, paired and mixed contracts [320, 320,
// 320, 320, 320, 320, 320, 320]; all restored.
// Round 196: Initial definition/cleanup/squad/lookup observation schedules
// [331, 333, 368, 342, 368, 342, 336, 348]; original observation order and all mutable work restored.
// Round 30: Squad parameter reference 320; unit re-observation 340; separate early failure 338;
// uninitialized/late-scoped actor result 338 each; settings lookup noinline 320; unit re-observation
// with a second guard or separate phases 340 each; cleanup indices first 320. Opening body restored.
// Round 231: independent query-output initialization orders 320 each in five forms;
// enabled-output default omitted 348; corrected-join Boolean/probability home identities 320/320.
// Original query defaults and all executable source restored; sentinel and homes remain.
// DL167: Native unsigned character/squad/returned handle interfaces, seven nonempty
// subsets, and all with the unsigned actor result: 320 each. Every declaration, original
// handle domain and complete body restored; frame 0x118 and sentinel EDI/ESI remain.
// Round 414: direct success-path object/weapon/metadata views, their pairs and all three
// 320 each; immutable placement view at each use 339. Original observations and full body restored.
// Round 92 (--fast, reproduces 320): pairs of lane 5 r77's block shapes (a int grenade tests, b palette
// count at each use, c the squad flag as a bool set from `(flags >> 10) & 1` under `if (squad)`, d the
// ternary substitution result): c 356; c+d 363; b+d 350; b+c 357; a+b 346; a+d 355; a+c 350; b+c+d 365;
// all four 369. First difference unchanged: retail's -1 web (ESI) reaches the loop-entry
// `squad_index != NONE` compare with squad_index in ECX; ours keeps squad_index in ESI and -1 in EDI.
// Lane 9 r52 (fast): an unrotated top-tested substitution loop gives retail's whole entry (-1 in ESI, the
// squad index in ECX): `bool replace = true; while (replace && next != NONE)` with the ternary result 302
// (a flag loop, kept only with a match); with the byte squad flag 322; with the mode switch 324; unit arm
// first on those 324 aligned worse. Flag-free `if (substitute && squad) while (squad_index != NONE &&
// next != NONE)` gives the same loop shape but LTCG then inlines 0x1e4990 at the in-loop call 423 (also
// with the advance under `if (ternary) ... else break;` 423). Unit field read at each use: two sequential
// guards 310 (a repeated test), one guard 333. VC places the unit arm right after the perception arm
// whatever the source order (goto-ordered arms, cleanup first, an inlined unit arm, a switch on the bit).
// Lane 18 round 24 (fast; plain confirms 254): kept: the placement mode as a switch (retail's `dec ecx; je; dec ecx;
// jne` dispatch), the actor's unit field read at each use with retail's three -1 tests (switch, weapon block,
// 0xb9c60), and the squad flag as a bool set from bit 10 under `if (squad)` (retail's byte home, the callees' bool
// parameter): 320 -> 304 -> 254. Tried: the switch alone 322; field reads and guards without the switch 348; with one
// guard 331, two guards 325; a `unit` local for the case-1 calls 304 (same code: our LTCG reloads the field where
// retail holds EBX across 0xb9dd0); on 254: ternary substitution result 348; palette count at each use 258; with int
// grenade tests and a squad grenade local 347; int grenade compares alone 352 (frame changes); `squad` declared at
// the top of the block 254 (same code). Loop guards folded into the while condition (retail's unrotated loop) 445 /
// `if (substitute) while (squad && squad_index != NONE && ...)` 439: both inline 0x1e4990 at the in-loop call.
// Round 464: separate actual request records at case-entry/successful-producer scopes,
// four phase pairs with each genuine case order: 254 each. Original shared request restored;
// persistent sentinel and substitution/squad homes still differ at the matching frame and length.
// DL234: current definition-table field at every use, guarded/conditional replacement,
// inner/outer query storage and mutable/readonly metadata: 397/396/397/396/397/396/397/396.
// Original observation and query-output lifetimes restored; sentinel and stack homes remain.
// Round 295: positive perception/unit creation producers and both, each 253; with settings
// admission each 253; both/all with conditional squad bit 337. Original failure exits restored.
// M297: Without the redundant squad-index gate, inner/outer substitution outputs,
// retained/omitted overwritten enabled default and guarded/conditional replacement
// [384, 382, 384, 382, 395, 380, 395, 380]. Original gate, defaults and lifetimes restored.
// N302: Complete paired equipment lookup loop, long/short/word/byte ordinal, defaults
// at pair / at each producer: 345/369/369/369/336/369/369/369. Exact-only output pair and all
// original code restored; frame agrees, sentinel/squad/substitution homes remain.
// Round 466: Direct guarded substitution-output consumption / complete branch producer,
// inner/outer real query storage and retained/omitted overwritten enabled default:
// 348/275/348/275/348/275/348/275 on the retained mode/field/squad-bit graph.
// Omitted default expands frame to 0x11c; original 0x118-frame body and all outputs restored.
// P260: Squad-bit zero default before/after actor result, after definition/cleanup/
// settings/squad and before final settings guard: 341/341/341/341/340/336/254;
// entry declaration with original initialization 254. Original lifetime/body restored;
// sentinel EDI/ESI and squad/substitution homes remain at agreeing frame and length.
// P266: Independent settings/perception/unit failure returns, seven nonempty subsets
// using failed creation outputs, then all constant returns: 345/288/345/348/372/348/350/346.
// Original shared failure exit, actor default and complete executable body restored.
// Round 520: all six lexical orders of the native query-enabled/query-forced/squad
// Boolean homes, then probability-home names at both endpoints: 254 each.
// Original names and complete body restored; sentinel and squad/query homes remain.
// DL243: Matched creation/variant/equipment producer inline contracts, seven subsets: 254 each;
// all three with uniform placement view at use 335. Original body, every field and all
// declarations restored; sentinel EDI/ESI, substitution/squad homes and unrotated loop remain.
// Round 375: Sequential primary/secondary equipment scalar, signed short/long, producer/entry
// storage and optional grenade reuse: 254/254/254/254/262/262/262/262. Original independent selectors
// and complete body restored; frame/length agree, sentinel EDI/ESI and substitution homes remain.
// M325: On the retained mode/field/squad-bit graph, continuation acceptance bool/byte/short/long
// 337/337/299/302; rejection 326/326/330/329. ESI sentinel and unrotated entry recover,
// but squad/query homes remain. NOT kept: flag-driven continuation is unmatched; whole body restored.
// Round 46 (fast; plain confirms 253): kept: the three failure `goto done` exits (missing flags, perception,
// unit) as plain blocks and tests (no label) 254 -> 253; the flags block alone 253, each creation failure as a
// test alone 253/253. Tried: squad pointer ternary with the found arm first 353; unit/perception defaults
// before the definition read 254, their declarations swapped 254 (same code); squad bit as a ternary 337; on
// 253: ternary substitution result inside the break test 348; the squad-index gate moved into the while test 420
// (1319 bytes, 0x1e4990 inlined).
// m3 lane 16 r83 (fast): retail places the unit arm after the actor block (between switch case 2 and case 1)
// and jumps back to the shared actor test; ours keeps it after the perception arm. Switch with case 1 first
// 253 (same code); substitution result as a plain if/else 348; loop over a named `next` index 543 (1427 bytes,
// 0x1e4990 inlined); a compiled-out `do {} while (false);` at the perception arm's end, the unit arm's start,
// the loop body's top or before the actor test 253 each (same code). All restored.
// Round 48 (fast): retail's squad pointer shares its spill slot ([esp+0x10]) with the squad bit and the actor
// result, ours keeps a separate slot. The substitution loop in a forceinline helper taking the squad pointer 389
// (1307 bytes); the squad pointer `byte const *` 253 (same code); `bool flag = squad && (bit 10)` 252; the squad
// lookup ahead of the 0x1e4990 call 264; the loop's probability declared before its bools 253 (same code).
// AC522: Six remaining substitute/squad/squad-index admission partitions,
// then complete/default optional-squad pointer arms on the goto-free graph:
// 392/436/408/452/420/392/353/341. NOT kept: entry/layout regressions; original body restored.
// B421: Post-creation success bool/byte/short/long and complementary failure
// results 253/253/253/253/253/253/253/253. NOT kept: no byte match; condition
// residence does not recover sentinel ESI or squad/query homes. Original body restored.
// N370: Two-use optional squad-index availability/absence results, bool/byte/short/long:
// 356/356/352/354/356/356/352/354. NOT kept: no equality; original complete body restored.
// Sentinel EDI/ESI, squad/query/result homes and rotated substitution head remain.
// F374: Post-substitution settings presence/absence results in bool/byte/short/long:
// 253/253/253/253/253/253/253/253. NOT kept: condition residence does not recover
// native sentinel/squad/query homes; original admission and complete body restored.
// DH350: Query-output last-consumer scopes, complete decision/output-only phases,
// bool/byte/short/long replacement storage: 253/253/353/350 twice. NOT kept:
// no match or gain; original workspace, result domain and complete body restored.
// DH354: Settings query in the natural for step, four consumption forms with
// inner/outer outputs: 504/515/515/515/504/515/515/515. NOT kept: frame 0x12c versus 0x118,
// lookup expansion and extent regress; original guards/defaults/loop/body restored.
// DH356: Common Boolean query-default chains in both directions, probability zero
// before/after, inner/outer query storage: eight at 253. Original body restored;
// sentinel EDI/ESI, squad/query homes and rotated substitution head remain.
// DL264: Matched alternate/ordinary creation and cleanup inline
// contracts, seven subsets then all with the target inline: 253/253/253/253/253/253/253/253.
// NOT kept: no eligible native repair with preserved shared users; original executable source restored.
// Y589: Actual creation-payload/equipment-selection/actor-object last-use scopes,
// seven subsets then all with immutable settings view: eight at 253. NOT kept:
// sentinel/squad/query homes unchanged; original complete body restored.
// F383: Metadata refresh in the normal for step 504 (frame 0x12c, 1403 bytes);
// immutable squad/placement/actor bindings, seven subsets, all 253. Original loop,
// receivers and complete body restored; sentinel ESI and squad/query homes remain.
// B433: Common continuation/cleanup outside flags admission, producer-pointer scope
// ending before continuation, direct success return and flat cleanup combinations:
// 406/406/374/406/374/406/374/374. NOT kept: native extent regresses;
// original guards, producer scope, sentinel defaults and entire body restored.
// P333: Complete initial substitution admission, allowed/blocked in bool/byte/short/long:
// eight 389s at 1307/1272. NOT kept: entry/query homes and extent regress; full body restored.
// P349: Complete return/cleanup/settings/squad same-size lexical home orders:
// 253/253/253/253/253/253/253/253. NOT kept: no byte equality; original executable source restored.
// DH374: Successor-NONE result owned by while condition, bool/byte/signed-char/short/word/int/long/dword:
// 253/253/253/253/253/253/253/253 (fast control 253; full 253).
// NOT kept: no byte match or eligible gain. Original dynamic successor test, replacement result, query
// defaults, refreshes, cleanup and full body restored; sentinel ESI and squad/query homes remain.
// B452: Actual tag/squad/placement input layouts, seven subsets then all with immutable
// squad/placement bindings: eight at 253. NOT kept: sentinel and query homes unchanged;
// original input observations, outputs and complete body restored at the native frame/extent.
// Round 78 (m3 lane 13, fast): the compile-only harness shows `if (substitute && squad) while (squad_index != NONE &&
// next != NONE)` is retail's exact loop shape (squad index tested once, aligned head testing next only, back-edge jmp
// past the squad test by jump threading); every other loop spelling there rotates. Under LTCG it inlines 0x1e4990
// at the in-loop call: re-measured 420; terms swapped 408 (re-measure of lane 9 r106). New: that loop with the
// definition reads through character_definition_get 420 (still inlined); with the arms swapped 425; with the
// in-loop 0x1e4990 call before the definition update 378 (1314 bytes, first difference +0x4b as the flag loop's);
// that with nested break tests (`if (forced) { if (!0x203900) break; } else if (!enabled) break;`) 372; the call
// before the definition update on the current loop 258 (1272 bytes). Lane 9's flag loop keeps 0x1e4990 out of line
// because the call no longer dominates the back edge; no flag-free spelling found that does the same.
// m3 lane 8 r146 (fast; aligned/register-masked aligned rows from a difflib walk, base 249/222): `else return NONE;` after the unit
// arm's success block 268 (283/221); the actor setup tail as one inline helper called from both creation arms 888 (742 bytes, not
// inlined) and forceinline 1066 (1830 bytes, two copies, no merge: retail's displaced unit arm is not a merged duplicate tail); early
// `return NONE` after each failed creation call 359 (1279 bytes). Loop guards (which ones keep 0x1e4990 out of line with retail's
// unrotated loop): nested `if (substitute) if (squad) while (squad_index != NONE && next != NONE)` 420 (1319, inlined); ternary break
// test 421; `while (next != NONE && squad_index != NONE)` 408 (1323); if/else break tests 421; no squad-index test (diagnostic) 384;
// the squad-index test both in the guard and in the while (a repeated test, diagnostic) 337 (1256, 0x1e4990 out of line, aligned 221,
// masked 204), with the ternary break 296 (1258, aligned 218, masked 201: retail's entry and loop; what is left is the unit arm placed
// after the perception arm and two byte-store orders in the query setup). Lead: the redundant guard is what keeps the call cold.
// Round 106 (lane 9, fast with 0x1e4990/0x2036c0/0x203900/0x1e04a0): on the current base the flag loop (`bool replace =
// true; ... while (replace && next != NONE)`, ternary result) 296 (1258 bytes, first difference +0x4b: entry, sentinel
// and loop are retail's; left are the unit arm's placement, the stack slots and one store pair), with the arms
// swapped (`if (!(*flags & 1))`, unit arm first) 232 (1263); the arms swapped alone 339 (1273); the flag-free
// `while (squad_index != NONE && next != NONE)` 420 and with its terms swapped 408 (both inline 0x1e4990). Diagnostic:
// the real 0xb7b40/0xb8540 bodies in place of their stubs leave 253 (masked aligned 222 either way).
// On the flag loop: the unit arm reached by a goto after the actor block and jumping back to the actor test 336 (1275);
// a switch on the bit (case 1 perception, default unit) 299 (1272 bytes, unit arm first); switch case 1 written first
// 296 (same code); the cleanup arm first (`if (actor_index == NONE)`) 301 (1253). None places the unit arm after case 2.
// Lane 12 r105 (fast with 0x1e4990/0x2036c0/0x203900/0x1e02f0/0x1e0160/0x1e04a0/0x1e06b0/0x1e4a50, all matched): a
// register-masked diff leaves the frame homes (retail packs the squad pointer, the squad bit and the actor into one slot at
// [esp+0x10], below the 0x2036c0 outputs at 0x16/0x17/0x1c; ours puts the outputs lowest at 0x12/0x13/0x14 and the squad at
// 0x1c, so the squad bit takes the probability's slot) and the rotated substitution head. Re-measured on this base: the
// squad-index test in the while condition with the ternary result 421 (1321 B; retail's whole entry and the query/call block, but
// LTCG inlines 0x1e4990 at the in-loop call), with the original replace flag 420 (1319 B). New: the three query outputs renamed
// with a `substitute_` prefix (sorting after `squad`) 253 (slots unchanged: names do not order these homes); the squad pointer set
// by an if statement 341 (1264 B), with the guard testing the squad index first 368 (1253 B); the 0x1e4990 result kept as a
// `long` 253 (same code).
// Lane 12 r107 (fast; masked aligned count in brackets, 222 at 253): re-measured lane 9's flag loop (`bool replace = true;
// while (replace && next != NONE)`, ternary result) 296 [201] (1258 B; masked: only the creation arm's place and slots). Retail
// places the creation arm behind the first predecessor chain of the bit test (after mode 2's jmp), the place VC gives a block that
// jump threading created. On the flag loop: the bit test held in a bool tested twice (`if (swarm)` / `if (!swarm)`) 302 [204]
// (the bool stays in memory, no threading); the else arm as `else if (!(*flags & 1))` 296 (same code, folded); the perception
// result as one conditional 321 [236] (1273 B). On the original loop: probability declared before the two bools 253 (same code);
// the three query outputs declared in the enclosing block and set in the loop 253 (same code).
// M3 lane 12 r111 (fast, 0x1e4990/0x2036c0/0x203900/0x1e02f0/0x1e0160/0x1e04a0/0x1e06b0/0x1e4a50 matched each time; aligned =
// difflib count, then registers masked, base 142/115). Lane 9's flag loop rebuilt 296 (119/102). Its unit arm: retail lets the
// perception arm fall into the actor test and parks the unit arm (it ends in a jump back to that test) after the first
// unconditional jump that follows, case 2's break; ours lets the unit arm fall into the actor test. Retail also shares one slot
// between the squad pointer, the squad bit and the actor index, reads the count at +0x98 and the table at +0x9c at each use with
// the scenario pointer in EDX, sign-extends each index before its `>= 0` test, and compares `grenade - 1` with NONE as an int
// (the data path re-extends the short, `movsx edx, bx; dec edx`, and recomputes `lea eax, [ebx - 1]` for the value). Trials:
// the two grenade tests without the (short) casts 352 (1283 B, frame 0x11c; the compare becomes retail's but the value reuses it);
// the count read at each use 257 (1273 B, 151/111); on the flag loop the count at each use 346 (1263 B, 155/108); on the flag
// loop the bit dispatched as `switch (*flags & 1)` with `case 1` perception and `case 0` unit 356 (1281 B, 163/116); a two-call
// inline helper taking the index as `long` and reading the block at its use 379 (1231 B, 179/121). Body restored.
// M3 lane 8 r152 (fast, the matched callees listed; masked aligned 115 at 253). The creation arm moves after case 2, as in
// retail (and retail's 0x2902b0, which carries a copy of this body), only when the whole arm from 0xb7930 to 0x1e0160 is a
// forced-inline body. Forced-inline creation helper (actor returned, unit through a pointer) 448 (1368 B, masked 70: the arm
// placed as retail, but 0x11dfb0 inlines inside the helper); the same `inline` or static 694 (933 B, not inlined); with the flag
// loop 336 (1358 B, masked 59), as `static __forceinline` 336; plus a palette getter for both weapon lookups 452; both arms in the
// helper 448; everything after the loop as one helper 346 (1250 B, no move); a helper holding the flags test and both arms with
// the join in the caller 399 (1331 B: arm moved, 0x11dfb0 a call with retail's argument setup, but the tail's 0xcbd50/0xb9c60
// inline), with the flag loop 340, plus the palette count read at each use 330, plus 32-bit grenade tests (`x - 1 != NONE`) 325,
// plus the join under a second flags test 282 (1272 B, no move), or the call and the join under the caller's flags test 249
// (1278 B, masked 37; build/r152/1e0850-helper-249.patch in the m3-lane8 worktree), that on the rotated loop 351, that with the
// helper returning the unit and writing the actor through a pointer 340 (1279 B, masked 30, frame 0x118).
// No move: early `return NONE` on the perception and unit failures 345; a helper for the setup calls only 296; a helper for the
// part after the unit exists 296. LTCG inlines by static frequency inside the function that holds the call (0x11dfb0 stays a call
// only under two tests; the tail's calls only at the tail's current depth), and an out-pointer for unit_index or
// perception_index stops the jump threading that sends retail's failures straight to `return NONE`. Diagnostic (reverted):
// `__declspec(noinline)` on this file's 0x11dfb0 declaration changes nothing (LTCG goes by the definition). Body unchanged.
// Round 155 (m3 lane 8, fast; masked = register-masked aligned count, 115 at 253): re-measured on this base, lane 5 r77's
// palette count read at each use 257 (1273 B, masked 111: the count reads then match retail), int grenade tests 352 (1283 B),
// both 276 (masked 116). On round 78's loop shape (420 re-measured): character_definition_get for both definition reads plus
// file-local inline squad-entry and palette getters 431 (diagnostic: 0x1e4990 still inlined in the loop, so this function's
// inline budget is not what keeps retail's call); 0x1e4990 respelled as a `while` with an early return 421 (0x1e4990's own
// code changes, still inlined). Retail inlines 0x1e4990 at 0x1e1a00's top-level call, so call frequency is not the reason
// either.
// MiniMax D (11-11): no new form, no lower score (253): the two NONE sentinels are still in EDI where retail keeps them
// in ESI, the squad pointer still takes the slot at +0x1c where retail puts it at +0x10, and the 0x2036c0 outputs keep the
// lower slots.
// Round 92 (m3 lane 11, fast): the direction through the existing direction_from_angles (vector_angles.h) 308 (1273 B;
// register-masked aligned 236 -> 215, the unit arm stays in source order); that with the palette count read at each use 307
// (masked 194); with 32-bit grenade tests as well 308; the weapon test without its one-use unit, weapon and definition locals
// 253 (same code; kept, it removes three one-use locals).
// M3 lane 12 r118 (fast, the eight matched callees still matched): retail's ESI is written before any read (`ret 0x10`), so it is
// no register argument. The substitution query as a force-inline predicate in the while condition (no break) 252 (same code, the
// loop still rotates); the predicate plain inline 483 (1151 B); taking the squad mode instead of the squad 252 (same code);
// returning `forced ? 0x203900(...) : enabled` 348; taking the squad index as short 252 (same code).
// P437: Fixed actual palette bound/squad bit/weapon handle/movement receiver, read-only
// movement receiver and fixed placement/squad/substitution inputs: 252 each. NOT kept: no native
// sentinel or squad/query-home recovery; original full body, helpers and call tuples restored.
// P440: Settings/perception/unit failure breaks, seven subsets then common failure cleanup: 947/947/947/947/947/947/971/963.
// NOT kept: Shared failure scopes expand the frame and creation/query inlining. Original complete body restored.
// P457: library enabled/forced/probability defaults 252/252/281; independent entry
// enabled/forced/probability workspaces 252 each; existing complete direction setter
// and actor-state library clear 252/252. NOT kept: ties or regression; required query
// defaults retained and original complete body restored; sentinel and query homes remain.
// P462: Actual enabled-query output also owns the final replacement decision,
// guarded/conditional producers, inner/outer output storage: 348/348/348/348;
// omitting its wholly overwritten default on those forms: 250/250/250/250.
// NOT kept: the two-point lead is below the gain threshold; original query
// outputs, all defaults, separate decision and complete body restored.
// P464: Independent equipment primary/secondary/grenade result, palette bound,
// primary/secondary/grenade selector and creation-variant role names: eight 252s.
// NOT kept: sentinel and squad/query homes unchanged; original complete body restored.
// T103: Existing typed 32-byte request, shared current/function/actor/settings lifetimes,
// then four case-entry/success pairs: 252/351/351/351/252/252/252/252.
// NOT kept: no native sentinel/squad/query-home equality; original byte payload,
// include set, exact request writes and complete body restored.
// MiniMax A (11-13): no new form, no lower score (252): the permuter (318 s) finds no try under the baseline; the squad
// pointer still takes the slot at +0x1c where retail puts it at +0x10, and the two NONE sentinels still sit in EDI where
// retail keeps them in ESI.
// Round 177 (m3 lane 6, fast with the eight matched callees): the flags lookup declared before the two NONE sentinels 267
// (1274 bytes); the squad ternary written `squad_index != NONE ? ... : NULL` 353; the squad entry index as `(word)squad_index`
// 237 (positional: `movzx ecx, si` where retail has `and eax, 0xffff`; not kept); the squad bit read before the flags test
// 253, after the placement view 252. Diagnostics: the squad index read through its address 252 (folded); volatile reads at
// its later uses 333, also in the guard 330 (the head still keeps it in ESI).
// P466: Unit/perception/both defaults inside settings 354/324/374; defaults only
// at complementary creation producers 365/362/370; both with branch-complete actor
// outcome 392; actor outcome alone 304. NOT kept: all regress; original defaults,
// result production and full body restored. Sentinel and squad/query homes remain.
// P469: Actual creation/request payload scalar element domains dword/long/word/real,
// each independently: eight 252s. NOT kept: genuine buffer alignment/alias information
// leaves sentinel EDI/ESI, squad/query homes and rotated entry unchanged; full body restored.
// P471: Standard signed sentinel predicates at squad selection/admission, live
// successor, perception/unit/actor creation results, every final actor-unit field
// and exclusive cleanup: 389/361/407/357/341/363/380/359. NOT kept: no native
// sentinel/query-home equality; original predicates, body and include set restored.
// P483: Complete forced/enabled short-circuit selection as disjunction/conjunction,
// bool/byte/short/long result: 348/348/348/348/348/348/348/348. NOT kept: no native sentinel/squad/query
// home repair; original query defaults, replacement producer and complete body restored.
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
			while (*(long *)(definition + 0x28) != NONE)
			{
				bool enabled = false, forced = false;
				real probability = 0.0f;
				function_2036c0(character_index, *(short *)(squad + 0x30), &enabled, &forced, &probability);
				bool replace = enabled;
				if (forced) replace = function_203900((short)squad_index, probability);
				if (!replace) break;
				character_index = *(long *)(definition + 0x28);
				definition = g_4e3b44[character_index & 0xffff].bytes;
				flags = (byte *)function_1e4990(character_index);
			}
		if (flags)
		{
			bool flag = squad && ((*(dword *)(squad + 0x20) >> 10) & 1);
			byte const *data = (byte const *)placement;
			if (*flags & 1)
			{
				perception_index = function_28dab0(character_index, (s_28dab2 const *)placement);
				if (perception_index != NONE)
					actor_index = function_1e02f0(squad_index, character_index, NONE, perception_index, flag);
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
					long primary = NONE, secondary = NONE, grenade = NONE;
					short primary_index = *(short *)(data + 0x22);
					if (primary_index == NONE && squad) primary_index = *(short *)(squad + 0x3c);
					long count = *(long *)((byte *)g_4e0350 + 0x98);
					if (primary_index != NONE && primary_index >= 0 && primary_index < count) primary = *(long *)(*(byte **)((byte *)g_4e0350 + 0x9c) + primary_index * 0x28 + 4);
					short secondary_index = *(short *)(data + 0x24);
					if (secondary_index == NONE && squad) secondary_index = *(short *)(squad + 0x3e);
					if (secondary_index != NONE && secondary_index >= 0 && secondary_index < count) secondary = *(long *)(*(byte **)((byte *)g_4e0350 + 0x9c) + secondary_index * 0x28 + 4);
					short grenade_index = *(short *)(data + 0x2c);
					if ((short)(grenade_index - 1) != NONE) grenade = grenade_index - 1;
					else if (squad && (short)(*(short *)(squad + 0x40) - 1) != NONE) grenade = (word)*(short *)(squad + 0x40) - 1;
					function_1e04a0(primary, character_index, unit_index, secondary, (short)grenade);
					actor_index = function_1e0160(squad_index, character_index, unit_index, flag);
				}
			}
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

// score 26 (was 26): retail passes the actor index to function_262ba0 and to
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
extern bool g_4f55e5;
extern bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);
void __stdcall function_1e1a00(long actor_index, long value);
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

// score 251 (was 251): frame 0x14 and the actor index in EBX agree; retail loads the pool global into EDI above the
// options lookup's `je` and holds it across 0x1e5280 (actor ESI, options EBP); ours re-reads it (actor EDI, options ESI).
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
// P468: Double probability/delay/fraction/increased result storage, then probability-
// increased/probability-delay/first-three/all: 348/302/272/307/328/321/307/304.
// NOT kept: precision/storage graphs regress and miss native actor/options/pool
// residence; original real values, expression groups and complete body restored.
// P483: Existing current/secondary getter results as actual-object references, mutable
// current/secondary/both, readonly current/secondary/both, both full-width slots,
// both byte slots: 251/251/251/251/251/251/272/251. NOT kept: native pool/actor/options
// residence unchanged; original complete body and both private queries restored.
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
		if (!(g_4e6948->state == 1 && g_4f55e5) &&
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
