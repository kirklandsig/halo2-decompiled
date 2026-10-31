// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"
#include "unknown_26b230.h"
#include "unknown_0d0690.h"
#include "unknown_2605d0.h"
#include "unknown_25fc30.h"
#include "unknown_2626b0.h"

/* slot type 0x26, the slot tests 0x35, 0x34, 0x32, 0x33, 0x55, 0x30, 0x57
   and 0x56, and slot group 0x2a */

struct s_slot_2a
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	byte unknown0e[0x40 - 0xe];
};

short __stdcall function_1ad550(long actor_index);
short __stdcall function_1b6c90(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b6930(long actor_index, s_slot *slot);
void __stdcall function_1b6d40(long actor_index, s_slot *slot);
short __stdcall function_1b6ef0(long actor_index, s_slot *slot);
short __stdcall function_1b6f80(long actor_index, s_slot *slot);
short __stdcall function_1b7000(long actor_index, s_slot *slot);
short __stdcall function_1b70f0(long actor_index, s_slot *slot);
short __stdcall function_1b7210(long actor_index, s_slot *slot);
short __stdcall function_1b73b0(long actor_index, s_slot *slot);
short __stdcall function_1b74c0(long actor_index, s_slot *slot);
short actor_behavior_find_level(long owner_index, short type);

/* the block of the actor's character tag function_1e4d10 returns */
struct s_character_d10
{
	byte unknown00[0xc];
	real unknown0c;
	real unknown10;
	real unknown14;
	byte unknown18[4];
	real unknown1c;
	real unknown20;
	real unknown24;
	real unknown28;
	real unknown2c;
	real unknown30;
	real unknown34;
	real unknown38;
	real unknown3c;
};

void *function_1e4d10(long actor_index);
void *function_1e4f90(long actor_index);
real function_1e96a0(short column, short row);
real function_1c9ee0(real fraction);
real function_259a0(dword *seed);

/* the block of the actor's character tag function_1e4f90 returns */
struct s_character_f90
{
	byte unknown00[0x18];
	real unknown18;
};
bool function_25d9b0(long prop_index);
bool function_1b6010(long index);

/* the state of slot types 0x2b and 0x2c */
struct s_slot_2b
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d;
	short ticks;
	byte unknown10[0x40 - 0x10];
};

/* the ticks left on the actor's slot of type 0x2b or 0x2c, unless a slot of
   type 0x2a is flagged */
// @retail 0x1b6930
bool __stdcall function_1b6930(long actor_index, s_slot *slot)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		function_1b6010(node->unknown08);
		if (view && view->unknown70 == 0 && function_26ba60(node->unknown08, actor_index, actor->unknown07c))
			function_1fb7e0(0x4c, actor_index, NULL, node->object_index, NONE);
	}
	else
		result = false;
	return result;
}

bool function_1b60d0(long index, short type, short level);
void function_25d420(long prop_index, short type, long actor_index);
short __stdcall function_272af0(s_match_globals *structure, point3f const *point);

// score 2 (was 2): retail reloads the spilled view before it loads the
// choice from the entry; ours loads the choice first. Retail passes the
// reference's own slot for function_2605d0's other index (0x18 for both;
// a separate other_index local makes the frame 8 bytes bigger, 38); the high bit test of
// unknown2 is retail's load and test ah; type is NONE before the call.
// Tried: choice read after the first view store 140; result set first 118;
// result cleared before the test 131; branches swapped, a view alias, const
// or outer choice, the copy through a cast 2/2/2/2/2; choice read at each
// use 33; the request's +4 store before +8 5; a pointer to the destination
// point 2; choice or view renamed to sort after/before the other 2/2; choice or
// view declared first 2/2; point copy before the state store 140; point copied
// with memcpy 2; const choice through a const pointer 2; +0x74 stored before
// +0x72 6; result set first in the success block 118; type NONE first in the
// success block 92.
// Round 101 (lane 7): the entry as the evaluated-position type the callee
// copies out, its definition pointer read into a local before the view store
// (kept, 2; replaces the made-up choice struct read through the entry's
// address); a destination point pointer taken before that read 2; the read
// after the view store 140; the local declared at the top of the view block 2;
// the scratch buffer released in each arm 2.
// @retail 0x1b69e0
bool __stdcall function_1b69e0(long actor_index, long prop_index)
{
    bool unknown;
    s_type_b36ac5 entry;
    s_2605d0_request request;
    bool result = false;
    s_actor_view *actor = actor_get(actor_index);
    s_prop_node_view *node = prop_node_get(prop_index);
    s_prop_view_fields *view = prop_node_view(node);
    if (view)
    {
        memset(&request, 0, sizeof(request));
        request.type = 5;
        *(long *)((byte *)&request + 8) = actor->prop_index;
        *((bool *)&request + 4) = true;
        *(long *)((byte *)&request + 0xc) = *(long *)((byte *)view + 0xc);
        *((bool *)&request + 0x10) = false;
        vector3f *direction = (vector3f *)((byte *)view + 0x94);
        if (direction->i * direction->i + direction->j * direction->j + direction->k * direction->k > 0.0f)
        {
            *((bool *)&request + 0x668) = true;
            *(vector3f *)((byte *)&request + 0x66c) = *direction;
            *((bool *)&request + 0x54) = true;
            *((bool *)&request + 0x55) = true;
            if (view->unknown70 == 1)
            {
                s_type_c3b527 *point = &view->unknown78;
                function_210850(point, (point3f *)((byte *)&request + 0x24));
                short cluster = *(short *)((byte *)view + 0x84) == NONE ?
                    *(short *)((byte *)view + 0x72) : function_272af0(g_4e0348, (point3f *)((byte *)&request + 0x24));
                *((bool *)&request + 0x20) = true;
                *(s_type_c3b527 *)((byte *)&request + 0x30) = *point;
                *(long *)((byte *)&request + 0x40) = *(long *)((byte *)view + 0x74);
                *(short *)((byte *)&request + 0x44) = cluster;
            }
        }
        *(real *)((byte *)&request + 0x1c) = 20.0f;
        byte *scratch = ai_scratch_buffer_get();
        /* the other index the callee writes lands in the reference's own slot,
           which its result then overwrites, as in retail */
        s_reference reference = function_2605d0(actor_index, &request, (long)&entry, (long)&reference, scratch, &unknown);
        if (!REFERENCE_EQUAL(reference, g_470fa0) && !(reference.unknown2 & 0x8000))
        {
            s_type_d4fbfa const *definition = entry.definition;
            view->unknown70 = 1;
            view->unknown78 = *(s_type_c3b527 const *)definition;
            *(short *)((byte *)view + 0x72) = definition->unknown12;
            *(long *)((byte *)view + 0x74) = definition->unknown14;
            short type = NONE;
            s_262b40_result *target = function_262b40(reference);

            if (target)
                type = target->unknown10;
            function_1b60d0(node->unknown08, reference.unknown2, type);
            result = true;
        }
        else
        {
            function_25d420(prop_index, 2, actor_index);
            view->unknown4c = true;
            result = false;
        }
        ai_scratch_buffer_release(scratch);
    }
    return result;
}

// @retail 0x1b6c90
short __stdcall function_1b6c90(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (node->unknown24 >= 3 && prop_node_view(node))
		{
			if (!actor->unknown040)
				result = g_46fbe8;
			else
			{
				function_1b69e0(actor_index, actor->prop_index);
				actor_get(actor_index)->unknown040 = false;
			}
		}
	}
	return result;
}

PRIVATE __forceinline long function_1b6e55(void)
{
	long local_0 = 0;
	return local_0;
}

// @retail 0x1b6e50
short function_1b6e50(long actor_index)
{
	short result = 0x7fff;
	short level = actor_behavior_find_level(actor_index, 0x2a);

	if (level != NONE)
	{
		s_actor_view *actor = actor_get(actor_index);

		if (((s_slot_2a *)&actor->slots[level])->unknown0d)
		{
			result = (short)function_1b6e55();
			goto local_1;
		}
		level = actor_behavior_find_level(actor_index, 0x2b);
		if (level == NONE)
			level = actor_behavior_find_level(actor_index, 0x2c);
		if (level != NONE)
		{
			struct s_1b6e51 { bool field_0; byte field_1; short field_2; };
			s_1b6e51 const *local_0 = (s_1b6e51 const *)((byte *)&actor->slots[level] + 0xc);

			if (local_0->field_0)
				result = local_0->field_2;
		}
	}
local_1:
	return result;
}

// @retail 0x1b6ef0
short __stdcall function_1b6ef0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE && !actor->unknown225)
	{
		s_prop_view_fields *view = prop_view_fields_get(actor->prop_index);

		if (view && view->unknown54 >= 0.8f)
		{
			s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);

			if (character && character->unknown3c > g_45dbd8 && view->unknown60 >= character->unknown3c)
				result = 0x2a;
		}
	}
	return result;
}

// @retail 0x1b6f80
short __stdcall function_1b6f80(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);

		if (character && function_25d9b0(actor->prop_index) && actor->unknown3d8 >= character->unknown38)
			result = 0x2a;
	}
	return result;
}

// @retail 0x1b70f0
short __stdcall function_1b70f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE && !actor->unknown223 && !actor->unknown225)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);

		if (character)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (node->unknown24 >= 1 && node->unknown24 <= 2 && character->unknown2c > node->unknown28)
				result = 0x2a;
		}
	}
	return result;
}

// @retail 0x1b7190
short __stdcall function_1b7190(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_object_view *object = object_get(actor->unknown26c);
	short result = g_46fbe4;
	s_tag_element *element = (s_tag_element *)function_1e5450(actor_index, object->tag_index);

	if (element && element->unknown94 > 0.0f && object->unknown100 > element->unknown94)
		result = 0x2a;
	return result;
}

PRIVATE __forceinline byte function_1b7211(long arg_0)
{
	return (g_557c40[arg_0 >> 5] >> (arg_0 & 31)) & 1;
}

// @retail 0x1b7210
short __stdcall function_1b7210(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE && !actor->unknown225)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);
		s_game_time_globals *local_1 = g_510c54;

		if ((actor->times[9] == NONE || (real)(local_1->game_time - actor->times[9]) * local_1->rate > character->unknown14) &&
			actor->unknown3d8 >= character->unknown1c &&
			character->unknown0c >= actor->unknown2d0 &&
			character->unknown10 >= actor->unknown2d4)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (!(character->unknown30 > node->unknown28) || node->unknown27 < 1)
			{
				s_slot_handler *local_0 = g_46eeb8[0x2a];
				if (local_0->unknown8 != g_46f348 && (local_0->mask & g_4ee4ec) == g_4ee4ec && function_1b7211(0x2a) != 0)
				{
					real t;

					if (actor->unknown3d8 > character->unknown20)
					{
						t = 1.0f;
					}
					else
					{
						real range = character->unknown20 - character->unknown1c;

						t = 0.0f;
						if (!(0.0f >= range))
							t = (actor->unknown3d8 - character->unknown1c) / range;
					}
					if (function_1c9ee0(character->unknown24 + (character->unknown28 - character->unknown24) * t) > function_259a0(&g_4e7408->unknown0))
					{
						actor->times[9] = local_1->game_time;
						result = 0x2a;
					}
				}
			}
		}
	}
	return result;
}

PRIVATE __forceinline real function_1b74c1(vector3f const *arg_0, vector3f const *arg_1)
{
	return *(real const volatile *)&arg_0->i * arg_1->i + arg_0->j * arg_1->j + arg_0->k * arg_1->k;
}

PRIVATE __forceinline bool function_1b74c3(long arg_0)
{
	bool local_0 = false;
	local_0 = function_110ab0(arg_0);
	return local_0;
}

// @retail 0x1b74c0
short __stdcall function_1b74c0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE && !function_1b74c3(actor->unknown018))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && node->unknown28 < 20.0f && view->unknown00 >= 9 &&
			function_1b74c1(&view->unknown2c, &actor->unknown290) < -0.1f)
		{
			result = 0x2a;
		}
	}
	return result;
}

// @retail 0x1b75a0
short __stdcall function_1b75a0(long actor_index)
{
	return 0;
}

PRIVATE __forceinline byte function_1b75b1(long arg_0)
{
	return (g_557c40[arg_0 >> 5] >> (arg_0 & 31)) & 1;
}

// @retail 0x1b75b0
short __stdcall function_1b75b0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe4;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && !actor->unknown225)
	{
		s_slot_2a *state = (s_slot_2a *)slot;

		if (state->unknown0d)
		{
			s_slot_handler *handler = g_46eeb8[0x36];

			if (handler->unknown8 != g_46f348 && (handler->mask & g_4ee4ec) == g_4ee4ec &&
				function_1b75b1(0x36) != 0)
			{
				result = 0x36;
			}
		}
		else if (prop_node_get(actor->prop_index)->type == 1)
		{
			result = g_46fbe8;
		}
	}
	return result;
}

// @retail 0x1b7650
bool __stdcall function_1b7650(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown225)
	{
		s_slot_2a *state = (s_slot_2a *)slot;

		actor->unknown220 = false;
		actor->unknown084 = 4;
		state->unknown0c = true;

		s_actor_view *actor_again = actor_get(actor_index);
		short i;

		actor_again->unknown3fe = 3;
		for (i = 0; i < 4; i++)
			actor_again->unknown400[i].reference = g_470fa0;
		result = true;
	}
	return result;
}

// @retail 0x1b76d0
short __stdcall function_1b76d0(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown225)
		return g_46fbe4;

	short result = function_1a79e0(actor_index, level, active);

	if (result == g_46fbe4)
	{
		((s_slot_2a *)&actor->slots[level])->unknown0d = true;
		result = 2;
	}
	return result;
}

s_slot_handler_2 g_47e6b8 =
{
	{
		0x26, 2, 0, -2, 0,
		function_1ad550, function_1b6c90, function_1b6930, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_1b6d40
};

s_slot_handler_0 g_47e704 =
{
	0x35, 0, 0x7ff, -2, 0, function_1b6ef0
};

s_slot_handler_0 g_47e718 =
{
	0x34, 0, NONE, -2, 0, function_1b6f80
};

s_slot_handler_0 g_47e72c =
{
	0x32, 0, 0, -2, 0, function_1b7000
};

s_slot_handler_0 g_47e740 =
{
	0x33, 0, 0x7ff, -2, 0, function_1b70f0
};

s_slot_handler_0 g_47e754 =
{
	0x55, 0, 0, -2, 0, function_1b7190
};

s_slot_handler_0 g_47e768 =
{
	0x30, 0, 0, -2, 0, function_1b7210
};

s_slot_handler_0 g_47e77c =
{
	0x57, 0, 0xbff, -2, 0, function_1b73b0
};

s_slot_handler_0 g_47e790 =
{
	0x56, 0, 0, -2, 0, function_1b74c0
};

/* the children of slot group 0x2a */
s_slot_child g_46fad8[8] =
{
	{0x50, 1, 13, {0}, 1.0f, 0, 0},
	{0x32, 1, NONE, {0}, 0.0f, 0, 0},
	{0x14, 1, NONE, {0}, 0.0f, 0, 0},
	{0x31, 1, NONE, {0}, 0.0f, 0, 0},
	{0x58, 1, NONE, {0}, 0.0f, 0, 0},
	{0x2e, 1, NONE, {0}, 0.0f, 0, 0},
	{0x54, 1, NONE, {0}, 0.0f, 0, 0},
	{0x2b, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47e7a8 =
{
	{
		0x2a, 1, 0, -2, 0,
		function_1b75a0, function_1b75b0, function_1b7650, 0, 1, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1b76d0, 8, g_46fad8
};

// @retail 0x1b7000
short __stdcall function_1b7000(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);
		s_character_f90 *timing = (s_character_f90 *)function_1e4f90(actor_index);
		real delay = 0.0f;

		if (timing)
		{
			delay = function_1e96a0(g_4e6948->state == 1 ? g_4e6948->difficulty : 1, 0x15) * timing->unknown18;
		}

		if (character)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (node->unknown27 >= 2 && character->unknown34 > node->unknown28 &&
				prop_node_state(node)->unknown3c == NONE)
			{
				long time = actor->times[3];

				if (time == NONE || (g_510c54->game_time - time) * g_510c54->rate > delay)
				{
					result = 0x10;
				}
			}
		}
	}
	return result;
}

// score 33 (was 33): the address of the argument is copied into ECX and pushed; retail loads it
// into EBX directly.
// BLOCKED-BY 0xd0620: retail's iterator initializer takes the iterator in ebx (it stores through
// ebx and leaves it for the caller's `mov edx, ebx`); ours takes it on the stack.
// @retail 0x1b73b0
short __stdcall function_1b73b0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (!team_is_enemy(actor->unknown024, 1))
	{
		s_object_child_iterator iterator;

		function_d0620(&iterator, actor->unknown26c);
		while (function_d0690(&iterator))
		{
			s_slot_object_view *object = object_get(iterator.child_index);

			if (object->player_index != NONE && object->unknownf0 < 0.25f)
			{
				result = 0x2a;
				break;
			}
		}
	}

	return result;
}

// @retail 0x1b6d40
void __stdcall function_1b6d40(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));

		if (view)
		{
			if (view->unknown70 == 0)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 2;
			}
			else
			{
				point3f point;

				function_210850(&view->unknown78, &point);
				actor->unknown41c = 3;
				actor->unknown420 = 3;
				actor->unknown424.point = point;
			}
		}
	}
	else
	{
		actor->unknown41c = 2;
		actor->unknown420 = 4;
		actor->unknown424.vector = actor->unknown290;
	}
}
