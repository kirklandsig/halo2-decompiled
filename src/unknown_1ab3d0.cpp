// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1AB3D0.CPP: the slot handlers of types 0x53 and 0x52
   (0x47db48..0x47dbe8) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "vector_products.h"
#include "unknown_26c380.h"
#include "unknown_0259d0.h"

/* the state of a slot of type 0x53 */
struct s_slot_53
{
	s_slot_header header;
	short ticks;
	byte unknown0e[2];
	vector3f direction;
	byte unknown1c[4];
	short timer;
	byte unknown22[0x40 - 0x22];
};

struct s_prop_datum_53
{
	byte unknown00[0x24];
	short unknown24;
	byte unknown26;
	char unknown27;
	real unknown28;
	byte unknown2c[0x3c - 0x2c];
};

struct s_prop_state_53
{
	byte unknown00[4];
	point3f position;
	byte unknown10[0x1c - 0x10];
	vector3f velocity;
	byte unknown28[0x3c - 0x28];
	long object_index;
	byte unknown40[0x64 - 0x40];
	bool unknown64;
};

struct s_definition_244
{
	byte unknown000[0x244];
	short unknown244;
};

struct s_weapon_entry
{
	byte flags00_0 : 1;
	byte unknown01[0x7a - 0x1];
	short unknown7a;
};

struct s_tag_element_53
{
	byte unknown00[0x8c];
	real unknown8c;
	real unknown90;
};

long function_1469f0(real seconds);
bool __stdcall function_1f4280(long actor_index);
real function_30bf0(vector3f *v);
bool __stdcall function_1ab770(long actor_index, s_slot *slot);
bool function_25d9b0(long prop_index);
real function_259a0(dword *seed);

/* the tag element (function_1e5450) as handler 0x52 reads it */
struct s_tag_element_52
{
	byte unknown00[0x14];
	real unknown14;
	byte unknown18[0x74 - 0x18];
	real unknown74;
	real unknown78;
	real unknown7c;
	real unknown80;
	real unknown84;
	real unknown88;
};

/* an element of g_50241c (0xc4 bytes) */
struct s_50241c_element
{
	byte unknown00[0x25];
	bool unknown25;
	byte unknown26[0xc4 - 0x26];
};

#define object_definition_244(object_index) \
	((s_definition_244 *)g_4e3b44[ai_object_get(object_index)->definition_index & 0xffff].bytes)

/* ---- slot type 0x53 ---- */

// @retail 0x1ab3d0
short __stdcall function_1ab3d0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (!actor->unknown229 && actor->prop_index != NONE)
	{
		s_prop_datum_53 *prop = (s_prop_datum_53 *)prop_node_get(actor->prop_index);
		if (prop->unknown24 >= 1 && prop->unknown24 <= 2)
		{
			s_prop_state_53 *state = (s_prop_state_53 *)function_25d690((s_prop_datum *)prop);
			if (state->object_index == NONE ||
				object_definition_244(actor->unknown26c)->unknown244 >= object_definition_244(state->object_index)->unknown244)
			{
				result = 3;
			}
		}
	}
	return result;
}

PRIVATE __forceinline vector3f *function_1ab4b1(vector3f const *arg_0, real arg_1,
    point3f const *arg_2, point3f const *arg_3, vector3f *arg_4)
{
    point3f local_0;
    local_0.x = arg_0->i * arg_1 + arg_2->x;
    local_0.y = arg_0->j * arg_1 + arg_2->y;
    local_0.z = arg_0->k * arg_1 + arg_2->z;
    scaled_difference3f(arg_3->n, local_0.n, 1.0f, arg_4->n);
    return arg_4;
}

static __forceinline long real_to_long(real value);

// score 2 (was 2): the last product loads the facing first (retail direction.i first);
// the i term through slot_state->direction 4; direction first in every product 2;
// direction->n[] 2; a projection local tested < 0 2; the slot pointer at the top 2;
// every operand and term order ties (the sum is canonicalised).
// final i-product load/multiply operands differ at +0xf4.
// Projection in the existing helper 6; Boolean projection 6; facing first 6;
// Boolean facing first 6; product scalars kji/ijk 2/2; Boolean scalars 2;
// facing first with scalars 2. Round 14: the data block through slot->unknownc (FORMS
// report) 6; the else branch through vector3d_from_points3d 2 (kept); the helper's result
// ignored 2; a facing pointer before the call or before the test 6/6 (j term first); the
// direction declared from the helper's result 51. Retail keeps each product's text order.
// Round 88: dot3f argument orders 6/6; direct < 0 2; slot-field projection 50;
// vector3f_add_scaled 2; with point difference 39; manual point difference 39;
// real_point3d_set 2. Original two-difference body restored.
// Round 38 (fast): a facing pointer for the k and i actor reads with slot_state->direction.i 2;
// slot_state->direction for all three direction reads 50 (472 B); a facing pointer for all three 6
// (also before the helper call, or for k/j, or j/i with direction.i through the slot 6 each). A
// one-file LTCG harness (exact: 2) over 136 spellings of the dot never loads direction.i first.
// @retail 0x1ab4b0
bool __stdcall function_1ab4b0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (!actor->unknown229 && actor->prop_index != NONE)
	{
		s_prop_datum_53 *prop = (s_prop_datum_53 *)prop_node_get(actor->prop_index);
		if (prop->unknown24 >= 1 && prop->unknown24 <= 2)
		{
			s_prop_state_53 *state = (s_prop_state_53 *)function_25d690((s_prop_datum *)prop);
			s_slot_53 *slot_state = (s_slot_53 *)slot;
			vector3f *direction = &slot_state->direction;

			direction = function_1ab4b1(&state->velocity, 0.5f, &state->position, &actor->position, direction);
			if (0.0f > actor->unknown290.k * direction->k + actor->unknown290.j * direction->j + direction->i * actor->unknown290.i)
			{
				vector3d_from_points3d(&actor->position, &state->position, direction);
			}
			slot_state->direction.k = 0.0f;
			if (function_30bf0(direction) > g_45dbd8)
			{
				s_tag_element_53 *element = (s_tag_element_53 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
				short ticks;
				if (!element || (ticks = (short)function_1469f0(element->unknown8c)) <= 0)
				{
					real seconds = (real)g_510c54->field_2_3 * 2.5f;
					long rounded = real_to_long(seconds);
					ticks = (short)rounded;
				}
				slot_state->ticks = ticks;
				slot_state->timer = NONE;
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x1ab690
short __stdcall function_1ab690(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_53 *state = (s_slot_53 *)slot;
	short result = g_46fbe8;

	if (state->timer == NONE)
	{
		if (actor->prop_index != NONE && state->ticks > 0)
			return result;

		s_tag_element_53 *element = (s_tag_element_53 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
		if (element)
		{
			real seconds = (real)g_510c54->field_2_3 * element->unknown90;
			long rounded;
			__asm
			{
				fld seconds
				fistp rounded
			}
			state->timer = (short)rounded;
		}
		else
		{
			state->timer = g_510c54->field_2_3 * 2;
		}
		if (state->timer != 0)
			return result;
		return g_46fbe4;
	}
	if (--state->timer <= 0)
		return g_46fbe4;
	return result;
}

// @retail 0x1ab880
void __stdcall function_1ab880(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_53 *state = (s_slot_53 *)slot;

	if (state->timer == NONE)
	{
		actor->unknown456 = true;
		actor->unknown458.i = state->direction.i * 20.0f;
		actor->unknown458.j = state->direction.j * 20.0f;
		actor->unknown458.k = state->direction.k * 20.0f;
		state->ticks--;
		function_1e4290(actor_index, true);
		actor->unknown41c = 4;
		actor->unknown420 = 0;
	}
	else
	{
		function_1f4280(actor_index);
		actor->unknown41c = 4;
		actor->unknown420 = 4;
		actor->unknown424.vector = actor->unknown290;
	}
}

/* ---- slot type 0x52 ---- */

// @retail 0x1abcf0
bool __stdcall function_1abcf0(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	long weapon_index = function_1e1f20(actor_index);

	if (weapon_index != NONE)
	{
		s_weapon_entry *entry = (s_weapon_entry *)function_1e5280(actor_index, ai_object_get(weapon_index)->definition_index);
		if (entry && entry->unknown7a == 3 && actor->unknown70a > 0)
			actor->unknown70a--;
	}
	if (actor->prop_index != NONE)
		result = true;
	return result;
}

// @retail 0x1abd70
void __stdcall function_1abd70(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown3e8 = g_510c54->game_time;
}

// @retail 0x1abfa0
bool __stdcall function_1abfa0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;

	if (actor->prop_index == NONE)
	{
		result = false;
	}
	else if (actor->unknown040 && !function_1f4810(actor_index, actor->prop_index, 0.0f, 1))
	{
		function_1f4280(actor_index);
		actor_get(actor_index)->unknown040 = false;
		result = false;
	}
	return result;
}

// @retail 0x1ac010
void __stdcall function_1ac010(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	actor->unknown44d = true;
	actor->unknown488 = true;
	long weapon_index = function_1e1f20(actor_index);
	if (weapon_index != NONE)
	{
		s_weapon_entry *entry = (s_weapon_entry *)function_1e5280(actor_index, ai_object_get(weapon_index)->definition_index);
		if (entry)
			actor->unknown488 = !TEST_FIELD_BIT(entry->flags00_0);
	}
}

// @retail 0x1ab950
bool function_1ab950(long actor_index, long prop_index, s_tag_element_52 *element, real value, bool strict)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *prop = prop_node_get(prop_index);
	s_prop_view_fields *view = prop_node_view(prop);

	if (view)
	{
		real distance = prop->unknown28 - value;
		real range_scale = strict ? 0.8f : 1.0f;
		real facing_scale = strict ? 0.8f : 1.0f;
		real range = 15.0f;

		if (element->unknown74 > 0.0f)
			range = element->unknown74;
		if (element->unknown78 > 0.0f)
			range = element->unknown78;
		real cosine = dot3f(&actor->unknown290, &view->unknown2c);
		result = true;
		if (range * range_scale > distance && facing_scale * 0.5f > cosine)
			result = false;
	}
	return result;
}

// @retail 0x1aba50
bool function_1aba50(long actor_index, long prop_index, real *out)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_object_view *unit = object_get(actor->unknown26c);
	bool result = false;
	s_tag_element_52 *element = (s_tag_element_52 *)function_1e5450(actor_index, unit->tag_index);
	s_prop_node_view *prop = prop_node_get(prop_index);
	s_prop_state_53 *state = (s_prop_state_53 *)prop_node_state(prop);
	s_prop_view_fields *view = prop_node_view(prop);

	if (view)
	{
		real scale = 1.0f;
		real height = unit->unknown03c;
		real value;

		if (state->object_index != NONE)
			height = object_get(state->object_index)->unknown03c + height;
		real prop_speed = dot3f(&state->velocity, &view->unknown2c);
		real unit_speed = dot3f(&unit->velocity, &view->unknown2c);

		if (element->unknown80 > 0.0f)
			scale = element->unknown80;
		value = unit_speed - prop_speed;
		value *= scale;
		result = prop->unknown28 - height - value > 0.0f;
		if (out)
			*out = value;
	}
	return result;
}

// @retail 0x1abbc0
short __stdcall function_1abbc0(long actor_index)
{
	long result = 0;
	real value;
	s_actor_view *actor = actor_get(actor_index);
	s_tag_element_52 *element = (s_tag_element_52 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);

	if (element)
	{
		long prop_index = actor->prop_index;

		if (prop_index != NONE && !function_25d9b0(prop_index))
		{
			s_prop_node_view *prop = prop_node_get(prop_index);

			if ((actor->unknown229 || !((s_prop_state_53 *)prop_node_state(prop))->unknown64) &&
				(element->unknown84 == g_45dbd8 || element->unknown84 > prop->unknown28) &&
				prop->unknown27 >= 2 &&
				function_1aba50(actor_index, actor->prop_index, &value) &&
				function_1ab950(actor_index, actor->prop_index, element, value, false))
			{
				long time = actor->unknown3e8;

				if (time == NONE || (real)(g_510c54->game_time - time) * g_510c54->rate > element->unknown7c)
					result = 4;
			}
		}
	}
	return result;
}

// @retail 0x1abda0
short __stdcall function_1abda0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe4;
	s_actor_view *actor = actor_get(actor_index);
	s_tag_element_52 *element = (s_tag_element_52 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);

	if (element && actor->prop_index != NONE)
	{
		s_prop_node_view *prop = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(prop);

		if (view && actor->unknown26c != NONE &&
			(element->unknown88 == g_45dbd8 || element->unknown88 > prop->unknown28) &&
			view->unknown00 >= 5)
		{
			real value;
			real random;
			bool close = function_1aba50(actor_index, actor->prop_index, &value);

			if (element->unknown14 * 2.0f > prop->unknown28)
				close = false;
			if (function_1ab950(actor_index, actor->prop_index, element, value, true))
			{
				if (close)
				{
					result = g_46fbe8;
				}
				else if (g_46eeb8[0x53]->unknown8 != g_46f348 &&
					(g_46eeb8[0x53]->mask & g_4ee4ec) == g_4ee4ec &&
					TEST_FIELD_BIT(SLOT_TYPE_BITS->type53) &&
					function_1ab3d0(actor_index) > 0 &&
					(((s_prop_state_view *)prop_node_state(prop))->unknown3c != NONE ||
					!((s_50241c_element *)g_50241c->data)[prop->unknown08 & 0xffff].unknown25 ||
					(random = function_259a0(&g_4e7408->unknown0), function_1e9700(0x1f) > random)))
				{
					result = 0x53;
				}
				else
				{
					result = 0x54;
				}
			}
		}
	}
	return result;
}
struct s_pathfinding_structure_53
{
	byte unknown00[0xc4];
	long count;
	s_pathfinding_data *data;
};

real function_1e3920(long actor_index);
void function_26c180(long actor_index);

// @retail 0x1ab770
bool __stdcall function_1ab770(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown229)
	{
		function_26c180(actor_index);
		if (actor->unknown27c.unknown10 != NONE)
		{
			real radius = function_1e3920(actor_index);
			s_pathfinding_structure_53 *structure = (s_pathfinding_structure_53 *)g_4e0348;
			s_pathfinding_data *pathfinding = NULL;
			vector3f direction;
			s_path_trace_result trace;

			if (structure->count > 0)
				pathfinding = structure->data;
			function_210770(actor->unknown27c.point.output_index, &actor->unknown290, &direction);
			function_26c590(pathfinding, &actor->unknown27c.point.point, actor->unknown27c.unknown10,
				NONE, &direction, 3.5f, NULL, &trace);
			s_sector_trace_result *result = (s_sector_trace_result *)&trace;
			if (result->blocked && result->edge_index != 0xffff)
			{
				s_pathfinding_edge *edge = &pathfinding->edges[result->edge_index];
				if ((*(word *)&edge->unknown04 & 0x400) || ((*(word *)&edge->unknown04 & 0x200) && radius > result->distance))
					((s_slot_53 *)slot)->ticks = 0;
			}
		}
	}
	return true;
}

/* ---- the handlers ---- */

s_slot_handler_2 g_47db48 =
{
	{
		0x53, 2, 0, -2, 0,
		function_1ab3d0, function_1ab690, function_1ab4b0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1ab770, 0, function_1ab880
};

s_slot_handler_2 g_47db98 =
{
	{
		0x52, 2, 0, -2, 0,
		function_1abbc0, function_1abda0, function_1abcf0, function_1abd70, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1abfa0, 0, function_1ac010
};
