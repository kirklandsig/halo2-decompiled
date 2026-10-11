// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E3920.CPP: the radius within which an actor counts as arrived */

#include "unknown_11c920.h"
#include "unknown_1fb7e0.h"
#include "props.h"
#include "ai_actor.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"

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
	long actor_index;
	long next_actor_index;
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
		s_actor_point_group *group = NULL;
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
						request->next_actor_index = ((s_actor_point_group *)g_502420->data)[request->index & 0xffff].first_actor;
						break;
					}
				}
			}
		}
		if (request->index == NONE)
			return NULL;
		long next_actor_index = request->next_actor_index;
		s_actor_moving *actor = NULL;
		if (next_actor_index != NONE)
		{
			actor = actor_moving_get(next_actor_index);
			request->actor_index = next_actor_index;
			request->next_actor_index = *(long *)((byte *)actor + 0x80);
		}
		if (actor)
		{
			vector3f delta;
			vector3d_from_points3d(&actor->position, &request->point, &delta);
			real squared = length_sq3f(&delta);
			if (squared < request->radius_squared)
			{
				request->result_index = request->actor_index;
				request->squared_distance = squared;
				return actor;
			}
		}
		else
			request->index = NONE;
	}
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

short function_1a6fe0(long owner_index, short type);

// @retail 0x1e1e50
bool function_1e1e50(long actor_index, vector3f *direction)
{
	const long *index_reference = &actor_index;
	vector3f *const *direction_reference = &direction;
	s_actor_moving *actor = actor_moving_get(*index_reference);
	bool result = false;
	if (actor->prop_index != NONE)
	{
		s_type_f95cd3 *view = function_25d700(actor->prop_index);
		if (view && *(short *)view > 6 && function_1a6fe0(*index_reference, 14) != NONE && actor->unknown722 > 0)
		{
			**direction_reference = *(vector3f *)((byte *)actor + 0x758);
			vector3f *value = *direction_reference;
			result = value->i * value->i + value->j * value->j + value->k * value->k > 0.0f;
		}
	}
	return result;
}

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
long function_baf40(long object_index);
long __stdcall function_cbd80(long object_index, long *holder_index);
void *function_1e5280(long actor_index, long key);
void function_118e80(long object_index, vector3f *forward);
real normalize2d(point2f *vector);
real function_267370(long volatile object_index);

// @retail 0x1e3b60
void function_1e3b60(long actor_index)
{
	(void)&actor_index;
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
		s_object_marker marker;
		function_b8d30(actor->unit_index, 0x40000bd, &marker, 1, false);
		point3f point = marker.matrix.position;
		bool volatile below_plane = false;
		short plane_index = *(short *)(actor_bytes + 0x254);
		if (plane_index != NONE)
		{
			byte *globals = (byte *)g_4e0348;
			byte plane_key = *(*(byte **)(globals + 0xa0) + plane_index * 0xb0 + 0x70);
			if (plane_key != 0xff)
			{
				byte *entry = *(byte **)(globals + 0x68) + (plane_key & 0x7f) * 24;
				if (*(short *)(entry + 2) != NONE)
				{
					if (!(plane_key & 0x80))
						below_plane = true;
					else
					{
						plane3f *plane = (plane3f *)(entry + 4);
						if (plane->i * point.x + plane->k * point.z + plane->j * point.y - plane->d < 0.0f)
							below_plane = true;
					}
				}
			}
		}
		actor_bytes[0x265] = below_plane;
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
			long root = function_baf40(carrier);
			if (((s_moving_object_header *)g_4e0300->data)[root & 0xffff].type == 1)
				carrier = root;
			byte *carrier_object = (byte *)moving_object_get(carrier);
			actor->unknown26c = carrier;
			if (*(long *)(carrier_object + 0x248) == actor->unit_index)
			{
				byte *definition = g_4e3b44[*(long *)carrier_object & 0xffff].bytes;
				if (*(short *)(definition + 0x1f0) != 6)
				{
					actor->unknown266 = true;
					*(short *)(actor_bytes + 0x270) = 1;
					dword flags = *(dword *)(definition + 0x1ec);
					if (flags & 0x800)
					{
						if (flags & 0x1000)
						{
							actor->unknown229 = true;
							*(short *)(actor_bytes + 0x270) = (flags & 0x2000) ? 2 : 4;
						}
						else if (flags & 0x2000)
							*(short *)(actor_bytes + 0x270) = 3;
						else
						{
							short mode = *(short *)(definition + 0x1f0);
							if (mode == 0 || mode == 1 || mode == 2)
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
				short seat = *(short *)(object + 0x1fc);
				if (!actor->unknown268 && !actor->unknown266 && seat != NONE)
				{
					byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
					actor_bytes[0x269] = (byte)((*(dword *)(*(byte **)(definition + 0x1cc) + seat * 0xb0) >> 11) & 1);
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
		function_118e80(actor->unknown266 ? actor->unknown26c : actor->unit_index, &actor->unknown290);
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
			if (header->type == 1 && (*(dword *)(g_4e3b44[*(long *)held & 0xffff].bytes + 0x1ec) & 0x100))
				function_118e80(held_index, (vector3f *)(actor_bytes + 0x29c));
			else
				*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(held + 0x168);
		}
		else
			*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(object + 0x168);
		vector3f *up = (vector3f *)(actor_bytes + 0x2a8);
		*up = *(vector3f *)(object + 0x18c);
		vector3f *left = (vector3f *)(actor_bytes + 0x2b4);
		left->i = up->k * g_4687b0->j - g_4687b0->k * up->j;
		left->j = g_4687b0->k * up->i - g_4687b0->i * up->k;
		left->k = g_4687b0->i * up->j - up->i * g_4687b0->j;
		function_30bf0(left);
		vector3f *forward = (vector3f *)(actor_bytes + 0x2c0);
		forward->i = up->j * left->k - left->j * up->k;
		forward->j = left->i * up->k - up->i * left->k;
		forward->k = left->j * up->i - up->j * left->i;
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
		if (*(long *)(actor + 0x308) == NONE || time - *(long *)(actor + 0x308) > g_510c54->field_2_3)
		{
			if (*(long *)(actor + 0x18) != NONE)
				function_20ba60(3, *(long *)(actor + 0x18), object_index, NONE, type, NULL);
			real incoming = *(real volatile *)&amount;
			*(long *)(actor + 0x308) = time;
			*(real *)(actor + 0x318) += incoming;
		}
		else
		{
			real incoming = *(real volatile *)&amount;
			*(real *)(actor + 0x318) += incoming;
		}
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
		long unit_index = actor_get(actor_index)->unknown018;
		if (unit_index != NONE)
		{
			long child_index = *(long *)((byte *)ai_object_get(unit_index) + 0x10);
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

#if 0
// Activating this body changes the matched forwarding caller 0x1ca260.
#include "unknown_1fb7e0.h"
// Disabled retail draft 0x1e18f0
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
 long unit = actor_get(actor_index)->unknown018;
 if (new_value > old_value)
 {
  if (unit != NONE)
   function_20ba60(0xbc, unit, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (old_value > new_value)
 {
  if (unit != NONE)
   function_20ba60(0xbd, unit, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (unit != NONE)
  function_20ba60(0xbe, unit, *(long *)(player + 0x2c), NONE, NONE, 0);
}
#endif

/* Actor equipment cleanup uses retail SSE arithmetic. */

#include "unknown_0259a0.h"
extern bool g_4f55e5;
extern bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *function_1e53e0(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);
void __stdcall function_1e1a00(long actor_index, long value);
long function_1e1f20(long actor_index);
PRIVATE inline long equipment_current_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = unit->current_weapon;
	long result = NONE;

	if (slot != NONE)
		result = unit->weapons[slot];
	return result;
}

PRIVATE inline long equipment_secondary_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = *((signed char *)unit + 0x213);
	long result = NONE;
	if (slot != NONE)
		result = unit->weapons[slot];
	return result;
}

// @retail 0x1e2a90
void function_1e2a90(long actor_index)
{
	long const *index_reference = &actor_index;
	s_actor_view *actor = actor_get(*index_reference);
	byte *actor_bytes = (byte *)actor;
	byte *weapon_options = NULL;
	long weapon_index = function_1e1f20(*index_reference);
	if (weapon_index != NONE)
		weapon_options = (byte *)function_1e5280(*index_reference,
			ai_object_get(weapon_index)->definition_index);
	if (*(short *)(actor_bytes + 0x84) == 4 && *(short *)(actor_bytes + 0x86) >= 3 &&
		actor->unknown018 != NONE)
	{
		byte *unit = (byte *)ai_object_get(actor->unknown018);
		if ((unit[0x134] & 1) && *(signed char *)(unit + 0x1f5) > 0 &&
			(function_cbd50(actor->unknown018, *(signed char *)(unit + 0x212)) != NONE ||
			function_cbd50(actor->unknown018, *(signed char *)(unit + 0x213)) != NONE))
		{
			real probability = 0.1f;
			if (weapon_options)
			{
				real value = *(real *)(weapon_options + 0x58);
				probability = value < 0.1f ? 0.1f : (value > 0.6f ? 0.6f : value);
			}
			if (actor_bytes[0x225] || (*(short *)(actor_bytes + 0x722) > 0 &&
				*(real *)(actor_bytes + 0x764) < 3.0f))
			{
				real increased = probability * 4.0f;
				if (increased > 0.6f) increased = 0.6f;
				if (probability <= increased) probability = increased;
			}
			if (probability > function_259a0(&g_4e7408->unknown0))
			{
				real delay;
				if (weapon_options && *(real *)(weapon_options + 0x5c) != 0.0f)
				{
					real value = *(real *)(weapon_options + 0x5c);
					delay = value < 0.8f ? 0.8f : (value > 1.3f ? 1.3f : value);
				}
				else
					delay = function_259d0(&g_4e7408->unknown0, NULL, 0, 0.8f, 1.3f);
				long ticks = function_1469f0(delay);
				function_c6f80(actor->unknown018, (short)ticks, 0x210000);
				unit[0x1f5] = (byte)ticks;
			}
		}
	}
	if (actor->unknown018 != NONE)
	{
		byte *unit = (byte *)ai_object_get(actor->unknown018);
		byte *grenade_options = (byte *)function_1e53e0(actor->unknown054,
			*(signed char *)(unit + 0x23c));
		long weapons[2];
		weapons[0] = equipment_current_weapon_r21(actor->unknown018);
		weapons[1] = equipment_secondary_weapon_r21(actor->unknown018);
		if (!(g_4e6948->state == 1 && g_4f55e5) &&
			(!*((byte *)g_4f55d0 + 0x340) || (grenade_options &&
			*(real *)(grenade_options + 0x38) > function_x82e52f(&g_4e7408->unknown0, NULL, 0))))
			*(short *)(unit + 0x23e) = 0;
		if (weapon_options)
		{
			long i = 0;
			do
			{
				if (weapons[i] != NONE)
				{
					real lower = *(real *)(weapon_options + 0x8c);
					real upper = *(real *)(weapon_options + 0x90);
					if (lower > 0.0f || upper > 0.0f)
					{
						real fraction = lower + (upper - lower) *
							function_x82e52f(&g_4e7408->unknown0, NULL, 0);
						if (g_4e6948->state == 1 && g_4f55e0) fraction *= 0.5f;
						function_101a10(weapons[i], fraction);
					}
					short minimum = *(short *)(weapon_options + 0x94);
					short maximum = *(short *)(weapon_options + 0x96);
					if (minimum > 0 || maximum > 0)
					{
						dword random = _random(&g_4e7408->unknown0, NULL, 0);
						long range = (short)(maximum + 1) - minimum;
						short rounds[2] = { (short)(((random * range) >> 16) + minimum), 0 };
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
	function_1e1a00(*index_reference, 1);
}
