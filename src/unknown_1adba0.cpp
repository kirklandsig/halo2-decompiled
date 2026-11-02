// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1ADBA0.CPP: the slot handlers of types 0x48, 0x49, 0x47, 0x3b,
   0x16 and 0x31 (0x47dcd4..0x47dde4) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "ai_actor.h"
#include "unknown_1f9240.h"
#include <string.h>

/* the state of a slot of type 0x47 (a joint behavior) */
struct s_slot_47
{
	s_slot_header header;
	bool joined;
	byte unknown0d[3];
	long joint_index;
	byte unknown14[0x40 - 0x14];
};

/* the state of a slot of type 0x3b */
struct s_slot_3b
{
	s_slot_header header;
	short ticks;
	short timer;
	short unknown10;
	bool unknown12;
	byte unknown13[0x40 - 0x13];
};

/* a joint (g_502424, 0xbc bytes) */
struct s_joint_view
{
	byte unknown00[2];
	short state;
	byte unknown04[0x7c - 0x4];
	short participant_count;
	byte unknown7e[2];
	bool unknown80;
	bool unknown81;
	byte unknown82[0xbc - 0x82];
};

/* the actor's tag entries function_1e4ef0 and function_1e4be0 return */
struct s_actor_entry_4ef0
{
	byte unknown00[0x14];
	real unknown14;
	real unknown18;
	real unknown1c;
	real unknown20;
	real unknown24;
	real unknown28;
};

struct s_actor_entry_4be0
{
	byte unknown00[4];
	real unknown04;
	real unknown08;
};

struct s_prop_datum_48
{
	byte unknown00[0x14];
	long view_index;
	byte unknown18[0x27 - 0x18];
	char unknown27;
	real unknown28;
	byte unknown2c[0x3c - 0x2c];
};

struct s_prop_view_54
{
	byte unknown00[0x54];
	real unknown54;
};

struct s_ai_actor_view_344
{
	byte unknown000[0x344];
	long unknown344;
	byte unknown348[0x3d8 - 0x348];
	real unknown3d8;
	byte unknown3dc[0x449 - 0x3dc];
	bool unknown449;
	bool unknown44a;
	byte unknown44b[0x480 - 0x44b];
	bool unknown480;
	byte unknown481[0x5d4 - 0x481];
	bool unknown5d4;
};

#define ACTOR_VIEW_344(actor) ((s_ai_actor_view_344 *)(actor))

/* the game's deterministic random, as Bungie's macro */
#define AI_RANDOM_REAL() \
	(g_4e7408->unknown0 = 1664525 * g_4e7408->unknown0 + 1013904223, \
	(real)(g_4e7408->unknown0 >> 16) * (1.f / 65535.f))

void *function_1e4ef0(long actor_index);
void *function_1e4be0(long actor_index);
long function_1e4a50(long index);
short __stdcall function_1bcc90(long actor_index);
short __stdcall function_1adff0(long actor_index, short level, bool active);

/* ---- slot type 0x48 ---- */

// @retail 0x1adba0
short __stdcall function_1adba0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_prop_datum_48 *prop = (s_prop_datum_48 *)prop_node_get(actor->prop_index);
		if (prop->unknown27 >= 2)
		{
			s_actor_entry_4ef0 *entry = (s_actor_entry_4ef0 *)function_1e4ef0(actor_index);
			if (entry && entry->unknown24 >= prop->unknown28)
				result = 0x46;
		}
	}
	return result;
}

/* ---- slot type 0x49 ---- */

PRIVATE __forceinline s_prop_view_54 *function_1adc21(s_prop_node_view *arg_0)
{
    s_prop_view_54 *local_0 = NULL;
    s_prop_node_view *local_1 = arg_0;
    if (local_1->view_index != NONE)
    {
        byte *local_2 = g_502414->data + (local_1->view_index & 0xffff) * 0x124;
        if (local_2)
            local_0 = (s_prop_view_54 *)(local_2 + 0x70);
    }
    return local_0;
}

// @retail 0x1adc20
short __stdcall function_1adc20(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_actor_entry_4ef0 *entry = (s_actor_entry_4ef0 *)function_1e4ef0(actor_index);
		if (entry)
		{
			s_prop_datum_48 *prop = (s_prop_datum_48 *)prop_node_get(actor->prop_index);
			s_prop_view_54 *view = function_1adc21((s_prop_node_view *)prop);
			if (view && prop->unknown27 > 0 && entry->unknown28 > view->unknown54)
				result = 0x46;
		}
	}
	return result;
}

/* ---- slot type 0x47 ---- */

// @retail 0x1adcd0
short __stdcall function_1adcd0(long actor_index)
{
	return 3;
}

// @retail 0x1adce0
short __stdcall function_1adce0(long actor_index, s_slot *slot, bool active)
{
	s_slot_47 *state = (s_slot_47 *)slot;
	short result = g_46fbe8;
	s_joint_view *joint = (s_joint_view *)element_502424_get(state->joint_index);

	if (state->joined)
	{
		if (joint->state == 1)
		{
			if (joint->participant_count < 2)
			{
				result = g_46fbe4;
			}
			else
			{
				joint->unknown80 = true;
				result = 0x46;
			}
		}
	}
	else if (joint->unknown80)
	{
		result = 0x46;
	}
	else if (joint->unknown81)
	{
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1add50
long __stdcall function_1add50(long actor_index, s_slot *slot)
{
	s_slot_47 *state = (s_slot_47 *)slot;
	s_slot_entry_iterator iterator;
	s_slot_memory_entry *entry;
	long joint_index;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x47;
	iterator.reference.unknown0 = NONE;
	for (entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
	{
		if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
		{
			joint_index = entry->unknown4;
			if (joint_index != NONE)
				return joint_index;
			break;
		}
	}
	joint_index = function_26e940(actor_index);
	if (joint_index != NONE)
	{
		state->joined = true;
		state->joint_index = joint_index;
	}
	return joint_index;
}

/* walks the actors of the group at 0x502420 (the walk 0x256150 makes);
   retail tests the next index at the loop head */
struct s_member_iterator_1addd0
{
	long next_index;
	long index;
};

static inline s_actor_view *member_iterator_next_1addd0(s_member_iterator_1addd0 *iterator)
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

// @retail 0x1addd0
short __stdcall function_1addd0(long actor_index, long joint_index, s_slot *slot, long unknown)
{
	short count = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown07c != NONE)
	{
		s_member_iterator_1addd0 iterator;
		s_actor_view *other;
		iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
		iterator.index = NONE;
		while ((other = member_iterator_next_1addd0(&iterator)) != 0)
		{
			if (actor != other && function_26eae0(joint_index, iterator.index, 3, 1.0f))
				count++;
		}
	}
	return count;
}

/* ---- slot type 0x3b ---- */

// @retail 0x1ade70
short __stdcall function_1ade70(long actor_index, s_slot *slot, bool active)
{
	s_slot_3b *state = (s_slot_3b *)slot;
	short result;

	if (ACTOR_VIEW_344(actor_get(actor_index))->unknown344 == NONE)
	{
		result = g_46fbe4;
		goto local_0;
	}
	if (state->timer <= 0)
	{
		g_46eeb8[0x3b]->unknown8 = g_46f348;
		result = 0x46;
		goto local_0;
	}
	result = g_46fbe8;
local_0:
	return result;
}

PRIVATE __forceinline real function_1aded1(real arg_0, real arg_1)
{
    return (real)(arg_0 + (arg_1 - arg_0) * AI_RANDOM_REAL());
}

// @retail 0x1aded0
bool __stdcall function_1aded0(long actor_index, s_slot *slot)
{
	s_slot_3b *state = (s_slot_3b *)slot;
	bool result = true;
	s_actor_entry_4ef0 *entry = (s_actor_entry_4ef0 *)function_1e4ef0(actor_index);

	if (entry)
	{
		real delay = function_1aded1(entry->unknown14, entry->unknown18);
		real duration = function_1aded1(entry->unknown1c, entry->unknown20);
		{
			real rate = g_510c54->field_2_3;
			real ticks = rate * delay;
			long rounded;
			__asm
			{
				fld ticks
				fistp rounded
			}
			state->ticks = (short)rounded;
		}
		{
			real rate = g_510c54->field_2_3;
			real ticks = rate * duration;
			long rounded;
			__asm
			{
				fld ticks
				fistp rounded
			}
			state->unknown10 = 0;
			state->unknown12 = false;
			state->timer = (short)rounded;
		}
	}
	else
		result = false;
	return result;
}

// @retail 0x1ae350
void __stdcall function_1ae350(long actor_index, s_slot *slot)
{
	s_slot_3b *state = (s_slot_3b *)slot;

	state->unknown12 = false;
}

/* ---- slot type 0x16 ---- */

// @retail 0x1ae360
short __stdcall function_1ae360(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->unknown225 && function_1bcc90(actor_index) > 0)
		result = 0x6b;
	return result;
}

/* ---- slot type 0x31 ---- */

PRIVATE __forceinline bool function_1ae3b1(s_ai_actor_view_344 *view, s_actor_entry_4be0 *entry)
{
	bool flag = view->unknown5d4;
	real value = view->unknown3d8;
	if (!flag && value > entry->unknown04)
		return true;
	if (view->unknown5d4 && entry->unknown08 > value)
		return false;
	return flag;
}

// @retail 0x1ae3b0
short __stdcall function_1ae3b0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_ai_actor_view_344 *view = ACTOR_VIEW_344(actor);
	s_actor_entry_4be0 *entry = (s_actor_entry_4be0 *)function_1e4be0(actor_index);

	if (entry)
	{
		bool flag = function_1ae3b1(view, entry);
		view->unknown449 = flag;
		view->unknown44a = flag;
		if (flag && !(*(byte *)function_1e4a50(actor->unknown054) & 1))
			view->unknown480 = true;
	}
	return g_46fbe4;
}

/* ---- the handlers ---- */

s_slot_handler_0 g_47dcd4 =
{
	0x48, 0, 0, -2, 0, function_1adba0
};

s_slot_handler_0 g_47dce8 =
{
	0x49, 0, 0, -2, 0, function_1adc20
};

s_slot_handler_2x g_47dd00 =
{
	{
		{
			0x47, 2, 0, -2, 0,
			function_1adcd0, function_1adce0, joint_initiate, joint_leave, NONE, {0},
			0, 0, 0, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1add50, 0, 0, 0, 0, (t_slot_proc4)function_1addd0,
	2, 10, 1.0f, 0
};

s_slot_child g_46f3d8[4] =
{
	{0x49, 1, NONE, {0}, 0.0f, 0, 0},
	{0x48, 1, NONE, {0}, 0.0f, 0, 0},
	{0x3a, 1, NONE, {0}, 0.0f, 0, 0},
	{0x47, 1, 11, {0}, 2.0f, 0, 0},
};

s_slot_handler_1 g_47dd70 =
{
	{
		0x3b, 1, 0, -2, 0,
		function_1adcd0, function_1ade70, function_1aded0, 0, NONE, {0},
		0, 0, 0, function_1ae350, 0, 0, 0
	},
	function_1adff0, 4, g_46f3d8
};

s_slot_handler_0 g_47ddbc =
{
	0x16, 0, -1, -2, 0, function_1ae360
};

s_slot_handler_0 g_47ddd0 =
{
	0x31, 0, 0, -2, 0, function_1ae3b0
};

struct s_1ae680
{
	byte field_0[0x7c8];
	long field_7c8;
};

struct s_1ae681
{
	byte field_0[6];
	short field_6;
	byte field_8[0x24 - 8];
	real field_24;
	real field_28;
};

bool g_4f55e5;
real function_259a0(dword *arg_0);
real function_1c9ee0(real arg_0);
void __stdcall function_26c2d0(long arg_0);
bool function_255b10(long arg_0, s_type_c3b527 const *arg_1, long arg_2, bool arg_3);

// score 15 (was 15): only the EBX/EBP pair differs: retail keeps the -1 sentinel and then the
// game-time pointer in EBP and the 0x1e5380 result in EBX, ours the other way round (so the
// prologue pushes and every use of the two registers differ). Tried: the result as a const
// pointer 15; the result/ticks/unit index declared elsewhere 15; the enable test nested 15; a
// game-time pointer local 15 or 91 (the flat form with that pointer 28); elapsed-seconds
// ready/arms/wait/nested results 15/15/15/15, each with the weighted probability product
// separated 65/65/65/65; actor reference 15; immutable actor 15; actor/node references 15;
// settings reference 15; timestamp reference 15; interval reference 15; settings/timestamp
// references 15; actor/coefficient references 15; whole eligibility, cooldown, reaction and
// combined test results 15/15/15/15; immutable cooldown inputs 15; read-only two-use clock 91;
// complete actor/timestamp record and its read-only receiver 15/15; native const/mutable clock
// records 91/91; interval accumulators on those records 95/95; named elapsed phase 91/91;
// elapsed plus interval accumulation 95/95; callback failure returns at actor/prop/unit/
// settings/cooldown/reaction phases 15/106/106/106/106/106; combined actor-settings/all phases
// 39/39; getter inline/noinline declarations 15/15; actor-index local 15; entry-scoped
// clock/timestamp 15/15; prop-index local 113; unit inline declaration 15; result-first 116;
// direct clock 96. Round 15: settings field before the factor in either product 15/15; clock
// read after the first factor 80; timestamp read before the clock 94 (418 bytes); unit index
// passed directly 41.
// Round 28: descriptive local names 15; the random compare written `f() < chance` 15; the prop
// state named at its test 15; the rate first in the elapsed product 15; `!= NULL` result test 15.
// Round 48: the permuter (46 forms, 600 s) 15; the result declared bare at the top 15, NULL at the
// top 15, bare beside the unit index 15; the settings factor first in the cooldown product 15. The
// two webs' instructions are identical in both builds, only their registers swap; the callee
// 0x110ab0 (unmatched, src/unknown_10f1e0.cpp) builds differently in the reduced image.
// @retail 0x1ae680
short __stdcall function_1ae680(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->prop_index != NONE && !local_0->unknown225)
	{
		s_prop_node_view *local_1 = prop_node_get(local_0->prop_index);
		if (local_1->unknown24 >= 1 && local_1->unknown24 <= 2 &&
			prop_node_state(local_1)->unknown3c == NONE)
		{
			long unit_index = local_0->unknown018;
			if (!function_110ab0(unit_index))
			{
				s_1ae681 *local_2 = (s_1ae681 *)function_1e5380(arg_0);
				if (g_4f55d0->unknown340 && local_2 && local_2->field_6 != NONE)
				{
					long local_3 = g_510c54->game_time;
					real local_4 = function_1e9700(0x18) * local_2->field_28;
					long local_5 = ((s_1ae680 *)local_0)->field_7c8;
					if (local_5 == NONE || (g_4e6948->state == 1 && g_4f55e5) ||
						(real)(local_3 - local_5) * g_510c54->rate >= local_4)
					{
						real local_6 = function_1c9ee0(function_1e9700(0x17) * local_2->field_24);
						if (g_4e6948->state == 1 && g_4f55e5)
							local_6 = 0.5f;
						if (local_6 > function_259a0(&g_4e7408->unknown0) && local_0->unknown07c != NONE)
						{
							function_26c2d0(local_0->prop_index);
							function_255b10(arg_0, &prop_node_state(local_1)->unknown48, local_0->prop_index, true);
						}
					}
				}
			}
		}
	}
	return g_46fbe4;
}

s_slot_handler_0 g_47ddf8 =
{
	0x13, 0, 0, -2, 0, function_1ae680
};


struct s_1ae440
{
	byte field_0[0x1c];
	vector3f field_1c;
	byte field_28[0x48 - 0x28];
	s_type_c3b527 field_48;
};

struct s_1ae441
{
	byte field_0[0x30];
	real field_30;
};

// @retail 0x1ae440
short __stdcall function_1ae440(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->prop_index != NONE)
	{
		long unit_index = local_0->unknown018;
		if (!function_110ab0(unit_index) && g_4f55d0->unknown340)
		{
			s_prop_node_view *local_1 = prop_node_get(local_0->prop_index);
			s_prop_state_view *local_2 = prop_node_state(local_1);
			if (local_1->unknown24 >= 1 && local_1->unknown24 <= 2 && local_2->unknown3c != NONE)
			{
				s_1ae681 *local_3 = (s_1ae681 *)function_1e5380(arg_0);
				if (local_3 && local_3->field_6 != NONE)
				{
					real local_4 = function_1e9700(0x18) * local_3->field_28;
					long local_5 = ((s_1ae680 *)local_0)->field_7c8;
					if (local_5 == NONE || (g_4e6948->state == 1 && g_4f55e5) || (real)(g_510c54->game_time - local_5) * g_510c54->rate >= local_4)
					{
						real local_6 = function_1c9ee0(((s_1ae441 *)local_3)->field_30) * function_1e9700(0x17);
						if (g_4e6948->state == 1 && g_4f55e5)
							local_6 = 1.0f;
						if (local_6 > function_259a0(&g_4e7408->unknown0) && local_0->unknown07c != NONE)
						{
							function_26c2d0(local_0->prop_index);
							s_1ae440 *local_7 = (s_1ae440 *)local_2;
							vector3f local_8;
							function_210770(local_7->field_48.output_index, &local_7->field_1c, &local_8);
							s_type_c3b527 local_9 = local_7->field_48;
							local_9.point.x += local_8.i * 1.5f;
							local_9.point.y += local_8.j * 1.5f;
							local_9.point.z += local_8.k * 1.5f;
							if (function_255b10(arg_0, &local_9, local_0->prop_index, false))
								function_1fb7e0(0x1c, arg_0, NULL, local_1->object_index, NONE);
						}
					}
				}
			}
		}
	}
	return g_46fbe4;
}

s_slot_handler_0 g_47dde4 =
{
	0x14, 0, 0, -2, 0, function_1ae440
};



struct s_1adff0
{
	short field_0;
	short field_2;
	short field_4;
	bool field_6;
	byte field_7;
	s_type_c3b527 field_8;
};

struct s_1adff1
{
	byte field_0[4];
	real field_4;
};

struct s_1adff2
{
	byte field_0[0x54];
	bool field_54;
	byte field_55[3];
	s_type_c3b527 field_58;
	long field_68;
	real field_6c;
};

short __stdcall function_1afde0(long arg_0);
bool function_270640(s_type_f17a25 *arg_0, s_type_c3b527 const *arg_1,
	long arg_2, bool *arg_3, s_type_c3b527 *arg_4);
bool function_2715a0(byte *arg_0);
bool function_114b60(short arg_0, short arg_1, long arg_2, long arg_3, void const *arg_4);

// @retail 0x1adff0
short __stdcall function_1adff0(long arg_0, short arg_1, bool arg_2)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_1adff0 *local_2 = (s_1adff0 *)((byte *)local_0 + 0x9c + arg_1 * 0x40);
	short local_1 = g_46fbe4;
	local_0->unknown449 = true;
	if (local_2->field_0 > 0)
		local_2->field_0--;
	if (local_2->field_2 > 0)
		local_2->field_2--;
	if (local_0->unknown040 && local_0->unknown344 != NONE)
	{
		s_1adff1 *local_3 = (s_1adff1 *)function_1e4a50(local_0->unknown054);
		s_prop_node_view *local_4 = prop_node_get(local_0->unknown344);
		s_prop_state_view *local_5 = prop_node_state(local_4);
		local_2->field_6 = false;
		if (local_4->unknown24 >= 1)
			function_26c2d0(local_0->unknown344);
		s_path_settings local_6;
		function_1f9240(arg_0, &local_6);
		long local_7 = local_5->unknown3c == NONE ? local_4->object_index : local_5->unknown3c;
		s_path_source local_8;
		memset(&local_8, 0, sizeof(local_8));
		local_8.radius = local_3->field_4;
		local_8.unknown04 = false;
		local_8.object_index = local_7;
		local_8.unknown4c = 0.0f;
		local_8.has_point = true;
		*(s_type_c3b527 *)&local_8.point = local_5->unknown48;
		local_8.unknown24 = *(long *)((byte *)local_5 + 0x44);
		local_8.unknown0c = local_0->unknown018;
		byte *local_9 = ai_scratch_buffer_get();
		function_271300((s_type_f17a25 *)local_9, NULL, &local_6, &local_8, 0);
		function_26c180(arg_0);
		s_1adff2 *local_10 = (s_1adff2 *)local_9;
		long local_18 = local_0->unknown27c.unknown10;
		local_10->field_54 = true;
		local_10->field_58 = local_0->unknown27c.point;
		local_10->field_68 = local_18;
		local_10->field_6c = 0.0f;
		if (function_2715a0(local_9))
		{
			bool local_11;
			local_2->field_6 = function_270640((s_type_f17a25 *)local_9,
				&local_0->unknown27c.point, local_0->unknown27c.unknown10,
				&local_11, &local_2->field_8);
		}
		ai_scratch_buffer_release(local_9);
	}
	if (local_2->field_6)
	{
		point3f local_12;
		function_210850(&local_2->field_8, &local_12);
		local_0->unknown41c = 3;
		local_0->unknown420 = 3;
		local_0->unknown424.point = local_12;
	}
	if (function_1afde0(arg_0) >= 3 && slot_type_enabled(0x3a))
		local_1 = 0x3a;
	else if (local_2->field_0 == 0)
		local_1 = function_1a79e0(arg_0, arg_1, arg_2);
	if (local_1 == g_46fbe4)
	{
		long local_13 = local_0->unknown018;
		byte *local_14 = (byte *)ai_object_get(local_13);
		short local_15 = *(short *)(local_14 + 0x342);
		if (*(short *)(local_14 + local_15 + 0xc) > 0)
			local_1 = 2;
		else if (local_2->field_4 > 0)
		{
			local_2->field_4--;
			local_1 = 2;
		}
		else
		{
			function_114b60(NONE, 0xd, local_13, 0xd, NULL);
			real local_16 = (AI_RANDOM_REAL() + 2.0f) * g_510c54->field_2_3;
			long local_17;
			__asm
			{
				fld local_16
				fistp local_17
			}
			local_2->field_4 = (short)local_17;
			local_1 = 2;
		}
	}
	return local_1;
}
