// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1AC090.CPP: the slot handlers of types 0x2c, 0x2b and 0x54
   (0x47dbe8..0x47dcd4) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_1fb7e0.h"
#include "unit_requests.h"

/* the state of the slots of types 0x2c and 0x2b */
struct s_slot_2c
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short ticks;
	bool unknown10;
	byte unknown11;
	bool unknown12;
	bool unknown13;
	vector3f facing;
	short unknown20;
	short unknown22;
	bool unknown24;
	bool unknown25;
	byte unknown26[2];
	s_type_c3b527 point;
	char unknown38;
	bool unknown39;
	bool unknown3a;
	bool unknown3b;
	byte unknown3c[0x40 - 0x3c];
};

/* the state of a slot of type 0x54 */
struct s_slot_54
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d;
	short ticks;
	byte unknown10[0x40 - 0x10];
};

struct s_ai_actor_view_3c
{
	byte unknown000[0x3c];
	bool unknown03c;
	byte unknown03d[0x20c - 0x3d];
	long unknown20c;
	byte unknown210[0x328 - 0x210];
	short unknown328;
	byte unknown32a[0x3f4 - 0x32a];
	long unknown3f4;
};

struct s_prop_datum_54
{
	byte unknown00[0x27];
	char unknown27;
	real unknown28;
	byte unknown2c[0x3c - 0x2c];
};

struct s_tag_element_54
{
	byte unknown00[0x98];
	real unknown98;
	real unknown9c;
	real unknowna0;
};

/* the unit's current mode: a short at +0x36 of its current state, whose
   offset is at +0x346 */
#define UNIT_MODE(unit_index) \
	(*(short *)((byte *)ai_object_get(unit_index) + 0x36 + *(short *)((byte *)ai_object_get(unit_index) + 0x346)))

#define ACTOR_VIEW_3C(actor) ((s_ai_actor_view_3c *)(actor))

/* the block of the actor's character tag (function_1e4d10) as handler 0x2b
   reads it */
struct s_character_2b
{
	byte unknown00[4];
	real lower;
	real upper;
	byte unknown0c[0x18 - 0xc];
	real unknown18;
};

void *function_1e4d10(long actor_index);
void function_1f86a0(long index);
bool function_25d9b0(long prop_index);
bool actor_has_joint_invitation(long actor_index, short type);

/* function_259d0 (unknown_0259d0), which retail inlines here: the random value
   is drawn into a local first */
static inline real random_range(real lower, real upper)
{
	real random = slot_random();

	return lower + (upper - lower) * random;
}

bool function_e68c0(long type, long unit_index);
void function_26def0(long actor_index);
bool function_1f86f0(long index);
short __stdcall function_1ac100(long actor_index, s_slot *slot, bool active);
void __stdcall function_1acda0(long actor_index, s_slot *slot);
void __stdcall function_1ad130(long actor_index, s_slot *slot);
void __stdcall function_1ad6a0(long actor_index, s_slot *slot);
void __stdcall function_1ada70(long actor_index, s_slot *slot);

/* ---- slot type 0x2c ---- */

// @retail 0x1ac090
bool __stdcall function_1ac090(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;
	dword *seed = &g_4e7408->unknown0;
	long ticks;

	*seed = 1664525 * *seed + 1013904223;
	real seconds = ((real)(*seed >> 16) * (1.f / 65535.f) + 1.0f) * 2.0f * g_510c54->field_2_3;
	__asm
	{
		fld seconds
		fistp ticks
	}
	state->ticks = (short)ticks;
	state->unknown39 = false;
	return true;
}

// @retail 0x1ac270
void __stdcall function_1ac270(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (!state->unknown39)
	{
		s_actor_view *actor = actor_get(actor_index);
		s_unit_request request = {0};
		request.type = 0x27;
		function_e6900(actor->unknown018, &request);
		if (!ACTOR_VIEW_3C(actor)->unknown03c && ACTOR_VIEW_3C(actor)->unknown3f4 != NONE)
			function_26def0(actor_index);
	}
}

// @retail 0x1ac300
bool __stdcall function_1ac300(long actor_index, s_slot *slot)
{
	long unit_index = actor_get(actor_index)->unknown018;
	short mode = UNIT_MODE(unit_index);
	bool result = false;

	if (mode == 6)
		return function_e68c0(0x25, unit_index);
	if (mode == 7)
		result = true;
	return result;
}

// @retail 0x1ac360
void __stdcall function_1ac360(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (state->ticks > 0)
		state->ticks--;
}

// @retail 0x1ac380
void __stdcall function_1ac380(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	if (UNIT_MODE(unit_index) == 7)
		actor->unknown488 = true;
}

/* ---- slot type 0x2b ---- */

// @retail 0x1ac3f0
short __stdcall function_1ac3f0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && !actor->unknown007)
		result = 3;
	return result;
}

// @retail 0x1ac430
bool __stdcall function_1ac430(long actor_index, s_slot *slot)
{
	s_character_2b *character = (s_character_2b *)function_1e4d10(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;

	if (!state->unknown0c)
	{
		real delay = random_range(character->lower, character->upper);
		real scale = random_range(1.0f, 3.0f);
		real seconds;
		long ticks;

		state->unknown0d = true;
		state->unknown12 = false;
		state->unknown10 = false;
		state->unknown25 = false;
		state->unknown3a = false;
		state->unknown0c = true;
		state->unknown22 = 0;
		state->unknown3b = false;
		seconds = g_510c54->field_2_3 * scale;
		__asm
		{
			fld seconds
			fistp ticks
		}
		state->unknown20 = (short)ticks;
		state->unknown13 = false;
		function_1f86a0(actor_index);
		if (g_45dbd8 > delay)
		{
			state->unknown24 = true;
		}
		else
		{
			seconds = g_510c54->field_2_3 * delay;
			__asm
			{
				fld seconds
				fistp ticks
			}
			state->ticks = (short)ticks;
			state->unknown24 = false;
		}
	}
	state->unknown39 = false;
	return true;
}

// @retail 0x1ac570
short __stdcall function_1ac570(long actor_index, s_slot *slot, s_slot *next)
{
	s_slot_2c *state = (s_slot_2c *)slot;
	short result = g_46fbe8;

	if (state->unknown12)
	{
		g_46eeb8[0x2b]->unknown8 = g_46f348;
		if (!actor_has_joint_invitation(actor_index, 0x36) && state->unknown10 && state->unknown25 &&
			g_46eeb8[0x2c]->unknown8 != g_46f348 &&
			(g_46eeb8[0x2c]->mask & g_4ee4ec) == g_4ee4ec &&
			TEST_FIELD_BIT(SLOT_TYPE_BITS->type2c))
		{
			*(s_slot_2c *)next = *state;
			state->unknown39 = true;
			result = 0x2c;
		}
		else
		{
			state->unknown39 = false;
			return g_46fbe4;
		}
	}
	return result;
}

// @retail 0x1acfd0
void __stdcall function_1acfd0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_character_2b *character = (s_character_2b *)function_1e4d10(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;

	if (actor->prop_index == NONE)
	{
		state->unknown12 = true;
	}
	else
	{
		s_prop_node_view *prop = prop_node_get(actor->prop_index);

		if (state->unknown22 > 0)
			state->unknown22--;
		if (state->unknown10)
		{
			real scale = random_range(1.0f, 3.0f);
			real seconds;
			long ticks;

			if (state->unknown24)
			{
				state->unknown12 = false;
			}
			else
			{
				short remaining = --state->ticks;

				if (function_25d9b0(actor->prop_index))
					state->unknown12 = false;
				else
					state->unknown12 = actor->unknown225 || actor->unknown2d4 >= character->unknown18 && remaining <= 0;
			}
			seconds = g_510c54->field_2_3 * scale;
			__asm
			{
				fld seconds
				fistp ticks
			}
			state->unknown20 = (short)ticks;
		}
		else if (state->unknown20 > 0 && --state->unknown20 == 0)
		{
			function_1fb7e0(actor_index, 0x47, NULL, prop->object_index, NONE);
		}
	}
}

// @retail 0x1ad130
void __stdcall function_1ad130(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;
	if (actor->unknown504 == 2)
	{
		if (state->unknown25)
		{
			if (state->unknown13 && dot3f(&actor->unknown290, &state->facing) > 0.95f)
			{
				short mode = UNIT_MODE(actor->unknown018);
				if (mode == 0 || mode == 7)
				{
					s_unit_request request;
					request.type = 0x24;
					function_210850(&state->point, &request.type25.point);
					request.type25.facing = state->facing;
					request.type25.unknown1c = state->unknown38;
					if (function_e6900(actor->unknown018, &request))
					{
						real seconds = g_510c54->field_2_3 * 0.5f;
						long ticks;
						__asm
						{
							fld seconds
							fistp ticks
						}
						state->unknown22 = (short)ticks;
						state->unknown3a = true;
					}
					else
						state->unknown25 = false;
				}
			}
		}
		else
			actor->unknown449 = true;
	}
	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);
		actor->unknown4a2 = true;
		if (view)
		{
			if (actor_get(actor_index)->unknown504 == 2 && state->unknown13 && state->unknown25)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 4;
				actor->unknown424.vector = state->facing;
				actor->unknown44d = true;
			}
			else if (node->unknown27 >= 1 || 1.0f > (g_510c54->game_time - view->unknown10) * g_510c54->rate)
			{
				actor->unknown488 = true;
				actor->unknown41c = 4;
				actor->unknown420 = 2;
			}
			else if (!view->unknown88 && view->unknown8c <= 0)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 2;
			}
			else if (function_1f86f0(actor_index) && state->unknown13)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 4;
				actor->unknown424.vector = state->facing;
			}
			else
			{
				actor->unknown41c = 2;
				actor->unknown420 = 2;
			}
		}
		else
		{
			actor->unknown41c = 2;
			actor->unknown420 = 2;
		}
		if (actor->unknown229)
			actor->unknown482 = true;
	}
}

// @retail 0x1acd30
bool function_1acd30(long actor_index, s_prop_datum_54 *prop)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (ACTOR_VIEW_3C(actor)->unknown328 >= 12)
	{
		result = true;
	}
	else if (UNIT_MODE(actor->unknown018) == 5)
	{
		if (2.0f > prop->unknown28)
			result = true;
	}
	else if (prop->unknown27 >= 1)
	{
		result = true;
	}
	return result;
}

// @retail 0x1ad400
void __stdcall function_1ad400(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;
	long unit_index = actor->unknown018;

	ACTOR_VIEW_3C(actor)->unknown20c = g_510c54->game_time;
	short mode = UNIT_MODE(unit_index);
	if (!state->unknown39)
	{
		if (mode == 6 || mode == 7)
			function_e68c0(0x27, unit_index);
		if (!ACTOR_VIEW_3C(actor)->unknown03c && ACTOR_VIEW_3C(actor)->unknown3f4 != NONE)
			function_26def0(actor_index);
	}
}

// @retail 0x1ad4a0
void __stdcall function_1ad4a0(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (state->unknown25)
		state->unknown25 = false;
}

/* ---- slot type 0x54 ---- */

// @retail 0x1ad4c0
bool function_1ad4c0(long actor_index, long prop_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);

	if (element)
	{
		s_prop_datum_54 *prop = (s_prop_datum_54 *)prop_node_get(prop_index);
		real range = element->unknown98 > g_45dbd8 ? element->unknown98 : 20.0f;
		return prop->unknown28 > range;
	}
	return result;
}

// @retail 0x1ad550
short __stdcall function_1ad550(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->prop_index != NONE)
		result = 3;
	return result;
}

// @retail 0x1ad590
bool __stdcall function_1ad590(long actor_index, s_slot *slot)
{
	s_slot_54 *state = (s_slot_54 *)slot;

	state->unknown0c = false;
	state->ticks = 0;
	return true;
}

// @retail 0x1ad5b0
short __stdcall function_1ad5b0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_54 *state = (s_slot_54 *)slot;
	short result = g_46fbe4;

	if (actor->unknown26c != NONE)
	{
		s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
		if (actor->prop_index != NONE && element)
		{
			result = g_46fbe8;
			if (function_1ad4c0(actor_index, actor->prop_index))
				state->ticks++;
			else
				state->ticks = 0;
			if (state->unknown0c && function_1f86f0(actor_index) ||
				(real)state->ticks * g_510c54->rate > element->unknown9c)
			{
				result = 0xe;
			}
		}
		else
		{
			result = g_46fbe4;
		}
	}
	return result;
}

void function_26c180(long actor_index);
real function_30bf0(vector3f *v);
void function_210be0(s_type_c3b527 const *a, s_type_c3b527 const *b, vector3f *out);

// @retail 0x1ada70
void __stdcall function_1ada70(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 2;
	actor->unknown420 = 2;
	actor->unknown488 = true;
	if (((s_slot_54 *)slot)->unknown0c)
	{
		char index = actor->unknown53a;

		if (index < actor->unknown539)
		{
			s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
			real distance = 4.0f;
			s_actor_point_entry *entry = &actor->unknown53c[index];
			vector3f direction;

			if (element->unknowna0 > 0.0f)
				distance = element->unknowna0;
			function_26c180(actor_index);
			function_210be0(&actor->unknown27c.point, &entry->point, &direction);
			if (function_30bf0(&direction) > distance && dot3f(&actor->unknown290, &direction) > 0.9f)
				function_1e4290(actor_index, true);
		}
	}
}
/* ---- the handlers ---- */

s_slot_handler_2 g_47dbe8 =
{
	{
		0x2c, 2, 0, -2, 0,
		0, function_1ac100, function_1ac090, function_1ac270, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1ac300, function_1ac360, function_1ac380
};

s_slot_handler_2 g_47dc38 =
{
	{
		0x2b, 2, 0, -2, 0,
		function_1ac3f0, (t_slot_evaluate)function_1ac570, function_1ac430, function_1ad400, NONE, {0},
		0, 0, 0, function_1ad4a0, 0, 0, 0
	},
	function_1acda0, function_1acfd0, function_1ad130
};

s_slot_handler_2 g_47dc88 =
{
	{
		0x54, 2, 0, -2, 0,
		function_1ad550, function_1ad5b0, function_1ad590, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1ad6a0, 0, function_1ada70
};
