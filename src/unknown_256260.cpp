// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_256260.CPP: slot handlers 0x76, 0x73, 0x74, 0x75 and their parent
   0x72 (handlers at 0x47f858..0x47f958, children at 0x470b70) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "props.h"
#include "unknown_2551c0.h"
#include "unknown_0259a0.h"

/* the slot state of handler 0x76 */
struct s_slot_76_state
{
	s_slot_header header;
	bool started;
	byte unknown0d;
	short timer;
	byte unknown10[0x40 - 0x10];
};

/* what function_1e4b50 returns for the actor's character */
struct s_character_76_view
{
	short unknown0;
	byte unknown2[2];
	real range;
	real duration;
};

s_record_pool *g_5044c8;

short g_470be8 = -1;
short g_470bec = -2;

void *function_1e4b50(long actor_index);
short actor_behavior_find_level(long owner_index, short type);
s_type_5cfb45 *function_25d670(long prop_ref_index);
short __stdcall function_1ad550(long actor_index);

short __stdcall function_256260(long actor_index);
short __stdcall function_256300(long actor_index, s_slot *slot, bool active);
void __stdcall function_2562c0(long actor_index, s_slot *slot);
void __stdcall function_256460(long actor_index, s_slot *slot);
short __stdcall function_2564a0(long actor_index, s_slot *slot, bool active);
void __stdcall function_2564e0(long actor_index, s_slot *slot);
short __stdcall function_256620(long actor_index);
short __stdcall function_256650(long actor_index, s_slot *slot, bool active);
void __stdcall function_256680(long actor_index, s_slot *slot);
short __stdcall function_256810(long actor_index, s_slot *slot);
short __stdcall function_256af0(long actor_index, short level, bool active);

s_slot_handler_2 g_47f858 =
{
	{
		0x76, 2, 0, -2, 0,
		function_256260, function_256300, 0, function_2562c0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_256460
};

s_slot_handler_2 g_47f8a8 =
{
	{
		0x73, 2, 0, -2, 0,
		function_1ad550, function_2564a0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_2564e0
};

s_slot_handler_2 g_47f8f8 =
{
	{
		0x74, 2, 0, -2, 0,
		function_256620, function_256650, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_256680
};

s_slot_handler_0 g_47f944 =
{
	0x75, 0, 0, -2, 0, function_256810
};

s_slot_child g_470b70[6] =
{
	{4, 1, -1, {0}, 0.f, 0, 0},
	{0x75, 1, 0, {0}, 1.f, 0, 0},
	{0x76, 1, -1, {0}, 0.f, 0, 0},
	{0x74, 1, -1, {0}, 0.f, 0, 0},
	{0x73, 1, -1, {0}, 0.f, 0, 0},
	{5, 1, -1, {0}, 0.f, 0, 0},
};

s_slot_handler_1 g_47f958 =
{
	{
		0x72, 1, NONE, -2, 0,
		actor_behavior_priority_minimum, 0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_256af0, 6, g_470b70
};

// @retail 0x256260
short __stdcall function_256260(long actor_index)
{
	short result = 0;
	s_handler_actor_view *actor = (s_handler_actor_view *)actor_get(actor_index);
	s_character_76_view *character = (s_character_76_view *)function_1e4b50(actor_index);

	if (character && perception_get(actor->perception_index)->unknown1c > character->unknown0)
	{
		result = 3;
	}
	return result;
}

// @retail 0x2562c0
void __stdcall function_2562c0(long actor_index, s_slot *slot)
{
	s_handler_actor_view *actor = (s_handler_actor_view *)actor_get(actor_index);

	if (actor->perception_index != NONE)
	{
		perception_get(actor->perception_index)->unknown1c = 0;
	}
}

#pragma inline_depth(0)
// score 19 (was 19): retail loads the pointer into ecx and the byte into dl, ours uses eax and cl; retail
// compares the two floats with comiss against memory where ours compares registers. Ours is
// padded with five int3s.
// A `short result = g_470bec;` at the top of the character block returned by all three
// success paths gives retail's ecx/dl and dword preload, and `timer * rate >= duration`
// returning g_470be8 gives retail's comiss; the started success then jumps into the nearby
// g_470be8 epilogue instead of the final one, so the code is 12 bytes short (70). Also
// tried: that form with `<` 75 or `!(<)` 75, with an early `if (!character)` return 75; the
// plain `>=` test without the local 97. Round 89: the 70 form's started success jumps into the
// g_470be8 epilogue, so its jb is short and the loop head loses its pad; one final `return result`
// (found path result/global; started failure return or assignment) 97/97/125 (load sunk into the
// return); result in the started path and one not-started return 97/97/19/19; started failure falling
// to the outer return 78/60/84; on the 70 form: early !character return 70, outer returns merged 91,
// result declared before the character call 74 or at the top 97, a final extra return 70.
// Lane 10 round 42: `!(a >= b)` is the form that tests with jb (comiss sets CF for less or unordered),
// `a < b` swaps the operands to use ja. The preload needs result returned on three paths (the 70 form);
// with two uses the load sinks: started success only 97, success and found 97, success and after-loop 97,
// found and one final block return 97. Started failure assigning result 125 (result spilled); started
// failure falling to one shared return of g_470be8 111 (edi pushed before the guards), or to the
// !character return with `!(>=)` 84 (the success gets its own epilogue); the started success falling to
// a final block return after the else 70 (the jb still joins the nearby g_470be8 epilogue).
// Round 68: slot data through unknownc 19; started and exhausted-search success sharing one return 19;
// that with a plain result-bearing for loop 19; handler-view actor 19; one outer failure return 29;
// state arms reversed 86; found flag with all success paths sharing one return 19; that with slot data 19.
// Original state type, actor view, comparison and body restored.
// @retail 0x256300
short __stdcall function_256300(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = ((s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view)));
	s_slot_76_state *state = (s_slot_76_state *)slot;

	if (actor->prop_index != NONE && ((s_handler_actor_view *)actor)->perception_index != NONE)
	{
		s_character_76_view *character = (s_character_76_view *)function_1e4b50(actor_index);

		if (character)
		{
			if (state->started)
			{
				state->timer++;
				if (!(state->timer * g_510c54->rate < character->duration))
				{
					return g_470be8;
				}
				return g_470bec;
			}
			else
			{
				s_type_5cfb45 *prop = function_25d670(actor->prop_index);
				real range = character->range * 0.75f;
				s_ai_object_iterator iterator;
				s_handler_object_view *object;

				iterator.index = NONE;
				iterator.next_index = ((s_perception_datum *)(g_5044c8->data + (((s_handler_actor_view *)actor)->perception_index & 0xffff) * sizeof(s_perception_datum)))->object_index;

				while ((object = swarm_creature_iterator_next(&iterator)) != NULL)
				{
					byte *data;
					if (!object->flags134 && (data = (byte *)object + object->ai_offset) != NULL)
					{
						double local_0 = (double)prop->position.x - *(real *)(data + 0x14);
						double local_1 = (double)prop->position.y - *(real *)(data + 0x18);
						double local_2 = (double)prop->position.z - *(real *)(data + 0x1c);
						double local_4 = local_2 * local_2;
						local_4 += local_0 * local_0;
						local_4 += local_1 * local_1;
						if (range > (real)sqrt(local_4))
						{
							return g_470bec;
						}
					}
				}

				state->started = true;
				return g_470bec;
			}
		}

		return g_470be8;
	}

	return g_470be8;
}
#pragma inline_depth(255)

// @retail 0x256460
void __stdcall function_256460(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4a4 = 6;
	actor->unknown4a5 = 2;
	actor->unknown4a8 = actor->prop_index;
}

// @retail 0x2564a0
short __stdcall function_2564a0(long actor_index, s_slot *slot, bool active)
{
	short result = g_470be8;

	if (actor_get(actor_index)->prop_index != NONE)
	{
		result = g_470bec;
	}
	return result;
}

// @retail 0x2564e0
void __stdcall function_2564e0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4a4 = 1;
	actor->unknown4a5 = 2;
	actor->unknown4a8 = actor->prop_index;
}

struct s_member_iterator_256520
{
	long next_index;
	long index;
};

static inline s_actor_view *member_iterator_next_256520(s_member_iterator_256520 *iterator)
{
	s_actor_view *member = 0;

	if (iterator->next_index != NONE)
	{
		member = actor_get(iterator->next_index);
		iterator->index = iterator->next_index;
		iterator->next_index = member->next_index;
	}
	return member;
}

// @retail 0x256520
bool function_256520(long actor_index, real range)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && actor->unknown07c != NONE)
	{
		s_prop_datum *prop = prop_ref_get(actor->prop_index);
		s_member_iterator_256520 iterator;
		s_actor_view *member;

		iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
		iterator.index = NONE;
		while ((member = member_iterator_next_256520(&iterator)) != 0)
		{
			if (member != actor && !member->unknown007 && member->prop_index != NONE)
			{
				s_prop_datum *other = prop_ref_get(member->prop_index);

				if (other->prop_index == prop->prop_index &&
					actor_behavior_find_level(iterator.index, 0xe) != NONE &&
					range > other->unknown28)
				{
					result = true;
				}
			}
		}
	}

	return result;
}

// @retail 0x256620
short __stdcall function_256620(long actor_index)
{
	short result = 0;

	if (function_256520(actor_index, 4.f))
	{
		result = 3;
	}
	return result;
}

// @retail 0x256650
short __stdcall function_256650(long actor_index, s_slot *slot, bool active)
{
	short result = g_470bec;

	if (!function_256520(actor_index, 6.f))
	{
		result = g_470be8;
	}
	return result;
}

// @retail 0x256680
void __stdcall function_256680(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4a5 = 2;
	actor->unknown4a8 = actor->prop_index;
	actor->unknown4a4 = 3;
}

// @retail 0x256af0
short __stdcall function_256af0(long actor_index, short level, bool active)
{
	return function_1a79e0(actor_index, level, active);
}

/* the marker function_b8d30 finds (0x70 bytes): its forward vector at +0x3c
   and its position at +0x60 */
struct s_marker_2566c0_view
{
	byte unknown00[0x3c];
	vector3f forward;
	byte unknown48[0x60 - 0x48];
	point3f position;
	byte unknown6c[0x70 - 0x6c];
};

/* the ai globals' count at +0x36a */
struct s_ai_globals_256810_view
{
	byte unknown000[0x36a];
	short unknown36a;
};

/* the ai data of an object (at its ai_offset): the prop it takes and how */
struct s_object_ai_256810_view
{
	byte unknown00[0x50];
	long prop_ref_index;
	short unknown54;
};

point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x2566c0
bool function_2566c0(s_object_marker *markers, long object_index, bool *facing)
{
	s_marker_2566c0_view *marker = (s_marker_2566c0_view *)markers;
	bool result = false;
	bool front = false;

	if (function_b8d30(object_index, 0xf0005b4, markers, 1, false) > 0 &&
		dot3f(&marker->forward, g_4687b0) > 0.f)
	{
		result = true;
		front = true;
	}
	else if (function_b8d30(object_index, 0xe0005b5, markers, 1, false) > 0 &&
		dot3f(&marker->forward, g_4687b0) > 0.f)
	{
		result = true;
		front = false;
	}

	if (facing)
	{
		*facing = front;
	}
	return result;
}

/* the object fields function_256790 reads */
struct s_object_256790_view
{
	byte unknown000[0x19];
	byte flags19;
	byte unknown01a[0xaa - 0x1a];
	byte type;
	byte unknownab[0x10a - 0xab];
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word unknown10a : 13;
};

PRIVATE __forceinline s_prop_datum *function_256791(long arg_0)
{
	return (s_prop_datum *)(g_502418->data + (arg_0 & 0xffff) * sizeof(s_prop_datum));
}

PRIVATE __forceinline bool function_256792(s_object_256790_view *arg_0)
{
	return TEST_FIELD_BIT(arg_0->bit2);
}

// @retail 0x256790
bool function_256790(long prop_ref_index)
{
	bool result = false;
	s_prop_datum *prop = function_256791(prop_ref_index);

	if (prop->state >= 1 && prop->state <= 2 && prop->type == 7)
	{
		s_object_256790_view *object = (s_object_256790_view *)object_get(prop->object_index);

		if (function_256792(object) && !object->type &&
			(!(object->flags19 & 1) || !(object->flags19 & 2)))
		{
			result = true;
		}
	}
	return result;
}

struct s_prop_ref_iterator_256810
{
	long next_index;
	long index;
};

static inline s_prop_datum *prop_ref_iterator_next_256810(s_prop_ref_iterator_256810 *iterator)
{
	s_prop_datum *prop_ref = 0;

	if (iterator->next_index != NONE)
	{
		prop_ref = prop_ref_get(iterator->next_index);
		iterator->index = iterator->next_index;
		iterator->next_index = prop_ref->next_index;
	}
	return prop_ref;
}

// score 107 (was 107): retail keeps the actor pointer in a stack slot and the prop loop's current index in
// ebp; ours keeps the actor in ebp and spills the index, so our frame is smaller (both read g_4f55f0). Retail's
// front is 16 bits (movzx dx from facing, word store, cmp word at the end): `short front` gives exactly that
// but scores 166 (the index is still spilled and the actor kept in ebx, next_index moving to ebp), so it is
// left as bool. Tried with short front: index used straight from the iterator, other declaration orders,
// iterator fields swapped or index copied first, one index variable shared by both loops (166-169); for or
// while loops over next_index rotate the loop (202, 205).
// @retail 0x256810
short __stdcall function_256810(long actor_index, s_slot *slot)
{
	if (g_4f55d0->active && ((s_ai_globals_256810_view *)g_4f55d0)->unknown36a <= 0xd)
	{
		long best_prop_ref_index = NONE;
		bool front;
		s_actor_view *actor = actor_get(actor_index);
		real best_distance = 3.4028235e38f;
		s_prop_ref_iterator_256810 prop_iterator;
		s_prop_datum *prop_ref;

		prop_iterator.next_index = actor->first_prop_index;
		prop_iterator.index = NONE;
		while ((prop_ref = prop_ref_iterator_next_256810(&prop_iterator)) != 0)
		{
			long index = prop_iterator.index;
			s_object_marker markers[1];
			bool facing;

			if (function_256790(index) && !prop_get(prop_ref->prop_index)->unknown36 &&
				function_2566c0(markers, prop_ref->object_index, &facing))
			{
				vector3f delta;
				real distance;

				vector3d_from_points3d(&actor->position, &((s_marker_2566c0_view *)markers)->position, &delta);
				distance = length_sq3f(&delta);
				if (best_distance > distance)
				{
					best_prop_ref_index = index;
					front = facing;
					best_distance = distance;
				}
			}
		}

		if (best_prop_ref_index != NONE)
		{
			s_prop_datum *prop_ref = prop_ref_get(best_prop_ref_index);
			s_type_76cf92 *prop = prop_get(prop_ref->prop_index);
			s_ai_object_iterator iterator;
			s_handler_object_view *object;
			point3f origin;
			long best_object_index;
			real nearest = 3.4028235e38f;

			iterator.next_index = perception_get(((s_handler_actor_view *)actor)->perception_index)->object_index;
			function_b9dd0(prop_ref->object_index, &origin);
			best_object_index = NONE;
			while ((object = swarm_creature_iterator_next(&iterator)) != NULL)
			{
				s_object_ai_256810_view *data;

				if (!object->flags134 && (data = (s_object_ai_256810_view *)((byte *)object + object->ai_offset)) != NULL &&
					((s_slot_object_view *)object)->parent_index == NONE && data->prop_ref_index == NONE)
				{
					point3f point;
					vector3f delta;

					function_b9dd0(iterator.index, &point);
					vector3d_from_points3d(&point, &origin, &delta);
					if (nearest > length_sq3f(&delta))
					{
						best_object_index = iterator.index;
						nearest = length_sq3f(&delta);
					}
				}
			}

			if (best_object_index != NONE)
			{
				s_handler_object_view *best = handler_object_get(best_object_index);
				s_object_ai_256810_view *data = (s_object_ai_256810_view *)(!best->flags134 ? (byte *)best + best->ai_offset : NULL);

				data->prop_ref_index = best_prop_ref_index;
				data->unknown54 = front == 0;
				prop->unknown36 = true;
			}
		}
	}
	return g_470be8;
}
