// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FDBF0.CPP: tests of an actor's combat state (+0x6fe, +0x722) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1e3920.h"
#include "unknown_0259d0.h"
#include "slot_handler.h"
#include "props.h"
#include "vector_products.h"

/* the prop view (unknown_25d690.cpp) starts with its state */
struct s_type_f95cd3;
s_type_f95cd3 *function_25d700(long index);

struct s_combat_prop_view
{
	short state;
};

PRIVATE __forceinline bool function_1fdbf1(s_actor_moving *arg_0)
{
    bool local_0 = false;
    s_combat_prop_view *local_1;
    local_0 = arg_0->unknown722 == 0 && arg_0->prop_index != NONE &&
        (local_1 = (s_combat_prop_view *)function_25d700(arg_0->prop_index)) != 0 && local_1->state >= 4 &&
        (real)arg_0->unknown350 * g_510c54->rate >= 2.5f;
    return local_0;
}

// @retail 0x1fdbf0
bool function_1fdbf0(long actor_index, short type)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;
	s_combat_prop_view *view;

	switch (type)
	{
	case 1:
		result = actor->unknown722 == 1 && actor->prop_index != NONE &&
			(view = (s_combat_prop_view *)function_25d700(actor->prop_index)) != 0 && view->state >= 6;
		break;
	case 2:
		result = function_1fdbf1(actor);
		break;
	case 3:
		result = actor->unknown722 == 1 && actor->prop_index != NONE &&
			(view = (s_combat_prop_view *)function_25d700(actor->prop_index)) != 0 && view->state >= 6 &&
			actor->unknown268;
		break;
	}
	return result;
}
// @retail 0x1fdd90
bool function_1fdd90(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown722 > 0)
		result = actor->unknown6fe == 2;
	return result;
}

/* the weapon object the actor holds (unknown_1e1f20.cpp) */
long function_1e1f20(long actor_index);

struct s_type_67e06b;

// @retail 0x1fe0e0
s_type_67e06b *function_1fe0e0(long actor_index)
{
	s_type_67e06b *result = 0;
	long weapon_index = function_1e1f20(actor_index);

	if (weapon_index != NONE)
	{
		s_moving_object *weapon = moving_object_get(weapon_index);

		result = (s_type_67e06b *)g_4e3b44[weapon->tag_index & 0xffff].bytes;
	}
	return result;
}

/* the seconds range of a character's combat behaviour */
struct s_combat_delay
{
	byte unknown00[0x48];
	real delay_lower;
	real delay_upper;
};

real function_1e96a0(short column, short row);

/* starts the actor's combat delay (+0x700, in ticks); false once it is engaged */
// @retail 0x1fee20
bool function_1fee20(long actor_index, s_combat_delay const *delay)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool engaged = actor->unknown48b;

	if (actor->unknown722 == 1 && prop_node_get(actor->unknown724)->unknown24 >= 3)
		engaged = true;
	if (!engaged && delay)
	{
		real seconds = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, delay->delay_lower, delay->delay_upper);
		real ticks;
		long result;

		seconds *= function_1e96a0(g_4e6948->state == 1 ? g_4e6948->difficulty : 1, 13);
		ticks = seconds * (real)g_510c54->field_2_3;
		__asm
		{
			fld ticks
			fistp result
		}
		actor->unknown700 = (short)result;
	}
	else
	{
		actor->unknown700 = 0;
	}

	return !engaged;
}

real function_1e9720(short kind, short team);

struct s_combat_wait_range
{
	byte unknown00[0x28];
	real lower;
	real upper;
};

// @retail 0x1fed70
void function_1fed70(long actor_index, s_combat_wait_range const *range)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	double upper = range->upper;
	double lower = range->lower;
	real seconds = (real)(lower + (upper - lower) * slot_random());
	real ticks;
	long result;
	seconds *= function_1e9720(14, *(short *)((byte *)actor + 0x24));
	ticks = (real)g_510c54->field_2_3 * seconds;
	__asm
	{
		fld ticks
		fistp result
	}
	actor->unknown700 = (short)result;
}

void function_118e80(long object_index, vector3f *forward);
bool __stdcall function_cba50(long unit_index, vector3f *direction, bool looking);

struct s_aim_definition
{
	byte unknown00[0x1ec];
	dword flags;
};

// @retail 0x1fe120
void function_1fe120(long actor_index, vector3f *direction)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	long object_index = actor->unit_index;
	if (actor->unknown268)
	{
		if (actor->unknown274 != NONE)
		{
			object_index = actor->unknown274;
			s_object_header_view *header = object_header_get(actor->unknown274);
			if (header->type == 1)
			{
				s_moving_object *object = (s_moving_object *)header->object;
				s_aim_definition *definition = (s_aim_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
				if ((bool)((definition->flags >> 8) & 1))
				{
					function_118e80(actor->unknown274, direction);
					return;
				}
			}
		}
	}
	*direction = *(vector3f *)(object_header_get(object_index)->object + 0x168);
	function_cba50(object_index, direction, false);
}

struct s_combat_blend_values
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
	real field_10;
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
	real field_28;
	real field_2c;
	real field_30;
	real field_34;
	real field_38;
	real field_3c;
};

struct s_combat_blend_block
{
	byte unknown00[8];
	long count;
	s_combat_blend_values *values;
};

void *actor_get_character_weapon_extra(long actor_index, long key);

// @retail 0x1fe1e0
bool function_1fe1e0(long actor_index, real blend, s_combat_blend_values *out)
{
	bool result = false;
	long weapon_index = function_1e1f20(actor_index);
	if (weapon_index != NONE)
	{
		s_combat_blend_block *block = (s_combat_blend_block *)actor_get_character_weapon_extra(actor_index, moving_object_get(weapon_index)->tag_index);
		if (block && block->count > 0)
		{
			s_combat_blend_values const *lower = &block->values[0];
			result = true;
			if (block->count < 2)
				*out = *lower;
			else
			{
				s_combat_blend_values const *upper = &block->values[1];
				real local_0 = *(real const volatile *)&upper->field_0;
				local_0 *= blend;
				local_0 += lower->field_0 * (1.0f - blend);
				out->field_0 = local_0;
				out->field_4 = lower->field_4 * (1.0f - blend) + upper->field_4 * blend;
				out->field_8 = lower->field_8 * (1.0f - blend) + upper->field_8 * blend;
				out->field_c = lower->field_c * (1.0f - blend) + upper->field_c * blend;
				out->field_10 = lower->field_10 * (1.0f - blend) + upper->field_10 * blend;
				out->field_14 = lower->field_14 * (1.0f - blend) + upper->field_14 * blend;
				out->field_18 = lower->field_18 * (1.0f - blend) + upper->field_18 * blend;
				out->field_1c = lower->field_1c * (1.0f - blend) + upper->field_1c * blend;
				out->field_20 = lower->field_20 * (1.0f - blend) + upper->field_20 * blend;
				out->field_24 = lower->field_24 * (1.0f - blend) + upper->field_24 * blend;
				out->field_28 = lower->field_28 * (1.0f - blend) + upper->field_28 * blend;
				out->field_2c = lower->field_2c * (1.0f - blend) + upper->field_2c * blend;
				out->field_30 = lower->field_30 * (1.0f - blend) + upper->field_30 * blend;
				out->field_34 = lower->field_34 * (1.0f - blend) + upper->field_34 * blend;
				out->field_38 = lower->field_38 * (1.0f - blend) + upper->field_38 * blend;
				out->field_3c = lower->field_3c * (1.0f - blend) + upper->field_3c * blend;
			}
		}
	}
	return result;
}

bool function_1ff7d0(long actor_index, point3f const *point, real enemy_radius, real friendly_radius, short *count_out);
void *function_1e5380(long actor_index);

PRIVATE __forceinline bool function_1fdce1(long arg_0)
{
	bool result = true;
	s_actor_view *actor = actor_get(arg_0);
	s_prop_node_view *node = prop_node_get(*(long *)((byte *)actor + 0x724));
	s_type_5cfb45 *state = function_25d690((s_prop_datum *)node);
	if (state->unknown3c != NONE)
		return true;
	byte *prop = g_50241c->data + (node->unknown08 & 0xffff) * 0xc4;
	if (prop[0x25])
		return false;
	short count = 0;
	function_1ff7d0(arg_0, &state->position, 6.0f, 0.0f, &count);
	result = count >= 3;
	return result;
}

// @retail 0x1fdce0
bool function_1fdce0(long actor_index, short type)
{
	bool result = true;
	if (type == 3)
	{
		result = function_1fdce1(actor_index);
	}
	return result;
}

// @retail 0x1fef40
bool function_1fef40(s_type_c3b527 const *target, long actor_index)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	byte *ranges = (byte *)function_1e5380(actor_index);
	point3f point;
	if (target->output_index != NONE)
	{
		if (!function_2104b0(target->output_index, &target->point, &point))
			point = target->point;
	}
	else
	{
		point = target->point;
	}
	if (ranges && function_1ff7d0(actor_index, &point, *(real *)(ranges + 0xc), *(real *)(ranges + 0x20), NULL))
	{
		*(s_type_c3b527 *)((byte *)actor + 0x7d0) = *target;
		result = true;
	}
	return result;
}

real function_30bf0(vector3f *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);

// @retail 0x1fddd0
bool function_1fddd0(long actor_index, point3f const *origin, vector3f *direction,
    long *out_index, real *arg_c9241c, long *out_object)
{
    byte *actor = (byte *)actor_moving_get(actor_index);
    long object_index = NONE;
    bool result = false;
    short state = *(short *)(actor + 0x6fe);
    if (state == 4)
        result = true;
    else if (state == 2)
    {
        result = true;
        if (*(short *)(actor + 0x722) == 1 && *(long *)(actor + 0x724) != NONE)
        {
            s_prop_node_view *node = prop_node_get(*(long *)(actor + 0x724));
            if (node->unknown24 >= 1)
                object_index = *(long *)((byte *)node + 0x20);
        }
        if (actor[0x7a8])
        {
            *direction = *(vector3f *)(actor + 0x7b0);
            if (out_index)
                *out_index = *(long *)(actor + 0x7ac);
        }
        else
        {
            point3f *target = (point3f *)(actor + 0x79c);
            direction->i = target->x - origin->x;
            direction->j = target->y - origin->y;
            direction->k = target->z - origin->z;
            function_30bf0(direction);
        }
        vector3f forward;
        function_1fe120(actor_index, &forward);
        real dot = direction->k * forward.k;
        dot += direction->j * forward.j;
        dot += direction->i * forward.i;
        if (dot < 0.8660253882408142f)
        {
            vector3f axis;
            bool valid = true;
            cross3f(&forward, direction, &axis);
            if (function_30bf0(&axis) == 0.0f)
            {
                function_11d000(&forward, &axis);
                if (function_30bf0(&axis) == 0.0f)
                    valid = false;
            }
            *direction = forward;
            if (valid)
            {
                real axis_i = axis.i;
                real local_0 = (direction->k * axis.k + direction->j * axis.j + direction->i * axis_i) * 0.1339746117591858f;
                real local_1 = direction->j * axis.k - direction->k * axis.j;
                real local_2 = direction->k * axis_i - direction->i * axis.k;
                real local_3 = direction->i * axis.j - direction->j * axis_i;
                direction->i = local_0 * axis_i + direction->i * 0.8660253882408142f - local_1 * 0.5f;
                direction->j = direction->j * 0.8660253882408142f + local_0 * axis.j - local_2 * 0.5f;
                direction->k = direction->k * 0.8660253882408142f + local_0 * axis.k - local_3 * 0.5f;
            }
        }
        *arg_c9241c = *(real *)(actor + 0x7bc);
    }
    if (out_object)
        *out_object = object_index;
    return result;
}

#include "slot_handler.h"
#include <math.h>
struct s_collision_result_1697c0;
struct s_point_collision
{
    long type;
    real fraction;
    byte field_8[0x24 - 8];
    short material;
    byte field_26[0x5c - 0x26];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
extern vector3f *g_4687b0;

// @retail 0x1fe410
void function_1fe410(point3f *point, real distance)
{
    point3f start;
    start.x = g_4687b0->i * 1.5f + point->x;
    start.y = g_4687b0->j * 1.5f + point->y;
    start.z = g_4687b0->k * 1.5f + point->z;
    real angle = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -3.1415927410125732f, 3.1415927410125732f);
    vector3f radial;
    radial.i = (real)cos(angle);
    radial.j = (real)sin(angle);
    radial.k = 0.0f;
    point3f destination;
    destination.x = radial.i * distance + start.x;
    destination.y = radial.j * distance + start.y;
    destination.z = radial.k * distance + start.z;
    vector3f direction;
    vector3d_from_points3d(point, &start, &direction);
    s_point_collision collision;
    collision.material = NONE;
    if (function_1697c0(0x15808c2f, point, &direction, NONE, NONE,
        (s_collision_result_1697c0 *)&collision))
        start = *point;
    vector3d_from_points3d(&start, &destination, &direction);
    if (function_1697c0(0x5808c2f, &start, &direction, NONE, NONE,
        (s_collision_result_1697c0 *)&collision))
    {
        distance = collision.fraction * distance - 0.1f;
        if (distance < 0.0f) distance = 0.0f;
        destination.x = radial.i * distance + start.x;
        destination.y = radial.j * distance + start.y;
        destination.z = radial.k * distance + start.z;
    }
    *point = destination;
}
