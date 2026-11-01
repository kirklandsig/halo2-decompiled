// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "data_array.h"
#include "unknown_1fb7e0.h"
#include "unknown_1f4460.h"
#include "unknown_1f9240.h"
#include "unknown_0259a0.h"

/* slot types 0x64, 0x65 and 0x66, the slot groups 0x62 and 0x63, and the
   slot tests 0x1c and 0x1f */

struct s_slot_64
{
	s_slot_header header;
	long prop_index;
	short unknown10;
	short unknown12;
	byte unknown14[2];
	bool unknown16;
	bool unknown17;
	bool unknown18;
	bool unknown19;
	byte unknown1a[2];
	s_type_c3b527 unknown1c;
	long unknown2c;
	byte unknown30[0x40 - 0x30];
};

/* the same slot's block after its header (s_slot's unknownc), offsets 0xc
   lower */
struct s_slot_64_data
{
	long prop_index;
	short unknown04;
	short unknown06;
	byte unknown08[2];
	bool unknown0a;
	bool unknown0b;
	bool unknown0c;
	bool unknown0d;
	byte unknown0e[2];
	s_type_c3b527 unknown10;
	long unknown20;
	byte unknown24[0x34 - 0x24];
};

struct s_slot_66
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	byte unknown14[0x40 - 0x14];
};

short __stdcall function_1b3820(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b36e0(long actor_index, s_slot *slot);
void __stdcall actor_behavior_release_prop(long actor_index, s_slot *slot, long index);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
bool __stdcall function_1b3600(long actor_index, s_slot *slot);
void __stdcall function_1b3880(long actor_index, s_slot *slot);
bool __stdcall function_1b3a80(long actor_index, s_slot *slot);
void __stdcall actor_behavior_64_update48(long actor_index, s_slot *slot);
short __stdcall function_1b3f60(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b3fd0(long actor_index, s_slot *slot);
bool __stdcall function_1b4240(long actor_index, s_slot *slot);
short __stdcall function_1b4390(long actor_index, short level, bool active);
short __stdcall function_1b3e20(long actor_index, short level, bool active);
short __stdcall function_1b44d0(long actor_index, s_slot *slot);
short __stdcall function_1b4560(long actor_index, s_slot *slot);

inline void actor_unit_function_20ba60(long actor_index, short type)
{
	long unit_index = actor_get(actor_index)->unknown018;

	if (unit_index != NONE)
		function_20ba60(type, unit_index, NONE, NONE, NONE, NULL);
}

void function_267770(long prop_index, long actor_index);
void *function_1e4e50(long actor_index);

// @retail 0x1b3590
short __stdcall function_1b3590(long actor_index, s_slot *slot, bool active)
{
	s_slot_64 *state = (s_slot_64 *)slot;
	short result = g_46fbe8;

	if (state->prop_index == NONE)
	{
		result = g_46fbe4;
	}
	else if (state->unknown17 && actor_get(actor_index)->unknown504 == 2 || state->unknown18)
	{
		if (--state->unknown10 <= 0)
			result = g_46fbe4;
	}
	else if (--state->unknown12 <= 0)
	{
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1b3600
bool __stdcall function_1b3600(long actor_index, s_slot *volatile slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool local_0 = actor->unknown040;
	s_slot_64 *state = (s_slot_64 *)slot;
	bool result = true;

	if (local_0 && !state->unknown17)
	{
		if (state->unknown2c != NONE)
			result = function_1f4460(actor_index, &state->unknown1c, state->unknown2c, NONE, false);
		else
			result = false;
		if (result)
		{
			actor->unknown4cc = 1.0f;
			state->unknown17 = true;
		}
	}
	return result;
}

// @retail 0x1b3670
short __stdcall function_1b3670(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (node->type == 6 && !(object_get(node->object_index)->unknownb2 & 1))
			result = 3;
	}
	return result;
}

struct s_1b36e0
{
    long field_0;
    short field_4;
    short field_6;
    short field_8;
    bool field_a;
    bool field_b;
    long field_c;
    s_type_c3b527 field_10;
    long field_20;
    real field_24;
};

void __stdcall function_26c2d0(long arg_0);
extern "C" void * __cdecl memset(void *, int, unsigned int);

// @retail 0x1b36e0
bool __stdcall function_1b36e0(long actor_index, s_slot *slot)
{
    s_actor_view *local_0 = actor_get(actor_index);
    s_1b36e0 *local_3 = (s_1b36e0 *)((byte *)slot + 0xc);
    s_prop_node_view *local_1 = prop_node_get(local_0->prop_index);
    s_prop_state_view *local_2 = prop_node_state(local_1);
    byte *local_4 = &object_get(local_1->object_index)->unknownb2;
    *local_4 |= 1;
    memset(local_3, 0, sizeof(*local_3));
    real local_5 = g_510c54->field_2_3 * 15.0f;
    long local_6;
    __asm
    {
        fld local_5
        fistp local_6
    }
    local_3->field_6 = (short)local_6;
    local_5 = g_510c54->field_2_3 * 4.0f;
    __asm
    {
        fld local_5
        fistp local_6
    }
    local_3->field_4 = (short)local_6;
    local_3->field_0 = local_0->prop_index;
    local_3->field_24 = 1.5f;
    function_26c2d0(local_0->prop_index);
    local_3->field_8 = 2;
    local_3->field_10 = local_2->unknown48;
    long local_7 = *(long *)((byte *)local_2 + 0x44);
    local_3->field_20 = local_7;
    local_3->field_a = false;
    local_3->field_b = false;
    return true;
}

// @retail 0x1b3820
short __stdcall function_1b3820(long actor_index, s_slot *slot, bool active)
{
	short result = function_1b3590(actor_index, slot, active);

	if (result == g_46fbe8)
	{
		s_slot_64 *state = (s_slot_64 *)slot;

		if (actor_get(actor_index)->prop_index != state->prop_index && state->prop_index != NONE)
			function_267770(state->prop_index, actor_index);
	}
	return result;
}

/* (retail reads slot from its argument twice, once for the target and once
   after the distance test: the parameter is volatile for that) */
// @retail 0x1b3880
void __stdcall function_1b3880(long actor_index, s_slot *volatile slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = (s_prop_node_view *)datum_get_inlined(g_502418, actor->prop_index);
		s_type_c3b527 *target = &((s_slot_64 *)slot)->unknown1c;
		real distance_squared;

		if (target->output_index == NONE)
		{
			vector3f v;

			vector3d_from_points3d(&target->point, &actor->position, &v);
			distance_squared = (v.k * v.k + v.i * v.i) + v.j * v.j;
		}
		else
		{
			point3f point;
			vector3f v;

			function_210850(target, &point);
			vector3d_from_points3d(&point, &actor->position, &v);
			distance_squared = (v.j * v.j + v.k * v.k) + v.i * v.i;
		}
		actor->unknown41c = 3;
		actor->unknown420 = 2;
		actor->unknown430 = 2;
		actor->unknown434 = 2;
		actor->unknown444 = NONE;
		if (distance_squared < 2.25f)
		{
			s_slot_64 *state;

			actor->unknown488 = true;
			actor->unknown4a2 = true;
			state = (s_slot_64 *)slot;
			if (!state->unknown16 && node)
			{
				function_1fb7e0(0x60, actor_index, NULL, node->object_index, NONE);
				state->unknown16 = true;
			}
		}
	}
}

// @retail 0x1b39e0
short __stdcall function_1b39e0(long actor_index)
{
	long result = 0;
	s_record_pool *object_pool = g_4e0300;
	s_record_pool *node_pool = g_502418;

	long prop_index = actor_get(actor_index)->first_prop_index;

	while (prop_index != NONE)
	{
		s_prop_node_view *node = (s_prop_node_view *)(node_pool->data + (prop_index & 0xffff) * sizeof(s_prop_node_view));

		prop_index = node->next_index;
		if (node->type != 7) continue;
		if (!(node->unknown28 < 3.5f)) continue;
		if (prop_node_state(node)->unknown00 == NONE) continue;
		if (((s_slot_object_view *)((s_object_header_view *)object_pool->data)[node->object_index & 0xffff].object)->unknownb2 & 1) continue;
		result = 3;
		break;
	}
	return (short)result;
}

// @retail 0x1b3a80
bool __stdcall function_1b3a80(long actor_index, s_slot *slot)
{
    bool result = false;
    s_1b36e0 *local_0 = (s_1b36e0 *)((byte *)slot + 0xc);
    long local_1 = actor_get(actor_index)->first_prop_index;
    long local_2 = NONE;

    while (local_1 != NONE)
    {
        s_prop_node_view *local_3 = prop_node_get(local_1);
        long local_10 = local_1;

        local_1 = local_3->next_index;
        if (local_3->type != 7) continue;
        if (!(local_3->unknown28 < 3.5f)) continue;
        if (prop_node_state(local_3)->unknown00 == NONE) continue;
        if (object_get(local_3->object_index)->unknownb2 & 1) continue;
        local_2 = local_10;
        break;
    }
    if (local_2 != NONE)
    {
        s_prop_node_view *local_3 = prop_node_get(local_2);
        s_prop_state_view *local_4 = prop_node_state(local_3);
        byte *local_5 = &object_get(local_3->object_index)->unknownb2;
        *local_5 |= 1;
        memset(local_0, 0, sizeof(*local_0));
        real local_6 = g_510c54->field_2_3 * 15.0f;
        long local_7;
        __asm
        {
            fld local_6
            fistp local_7
        }
        local_0->field_6 = (short)local_7;
        local_6 = g_510c54->field_2_3 * 4.0f;
        __asm
        {
            fld local_6
            fistp local_7
        }
        local_0->field_4 = (short)local_7;
        local_0->field_0 = local_2;
        local_0->field_24 = 1.0f;
        function_26c2d0(local_2);
        local_0->field_8 = 2;
        local_0->field_10 = local_4->unknown48;
        local_0->field_20 = *(long *)((byte *)local_4 + 0x44);
        local_0->field_a = false;
        local_0->field_b = false;
        long local_8 = actor_get(actor_index)->unknown018;
        if (local_8 != NONE)
            function_20ba60(0x62, local_8, local_3->object_index, NONE, NONE, NULL);
        result = true;
    }
    return result;
}


// @retail 0x1b3e20
short __stdcall function_1b3e20(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = function_1a79e0(actor_index, level, active);

	if (actor->unknown07c != NONE && element_502420_get(actor->unknown07c)->unknown26 != 1 &&
		(actor->slots[level].unknown4 == NONE || actor->slots[level + 1].type == 5))
	{
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1b3ea0
short __stdcall function_1b3ea0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown07c != NONE && !actor->unknown314.bit0 && !actor->unknown314.bit1)
		result = 3;
	return result;
}

// score 26 (was 26): offset/pool setup and zero register differ: retail computes the offset in
// EAX, keeps the pool pointer in ESI (the second data load reuses it, so the reset's actor is the
// offset plus that load in EAX), the first actor in ECX and the zero in EDX; ours has the offset
// in ECX, the pool in EAX, the first actor in EDX and the zero in EBX, and the reset's zero
// stores are then scheduled ahead of its NONE stores.
// compound bit 26; word mask 26; slot sentinel result 36; slot Boolean result 38;
// joint slot result 43; reset mode result 38; reset owner result 39; active slot result 43;
// native word ordinal 32; sentinel-first stores 26; actor after/between slot stores 52/27;
// actor after reset/direct final getter 35/34; published-bit return 31;
// sentinel after reset 36; activation before reset 25, rejected.
// Boolean after reset 26; both slots after reset 46; Boolean after activation 25; both slots after
// activation 53; sentinel after activation 34; actor after reset 34; actor after slots 34; direct
// slots after reset 46. Out-of-line reset function call 80 (stays a call); reset actor without the
// nested getter 26; offset-first actor address 26; local and parameter renames 26 each.
// Explicit getter/reset/both inline contracts 26/26/26; Boolean/sentinel/both field lvalues
// 26/26/26; inline contracts with Boolean/both lvalues 26/26.
// Slot state through a past-the-header view (`slot->unknownc`) 26, declared first 27 (127 bytes),
// with the NONE store first 26; stores through the cast parameter 26; `bool result = true` first 26.
// Round 39: `bool result = true` stored into the bit and returned 26; actor first with the NONE store
// first 26; `bit0 |= 1` 26; `result = true` after the bit 26; returning the bit itself 31 (128 bytes).
// Harness (exact for this function): the actor address spelled offset-first, by index or with a named
// offset 26 each; the reset in retail's store order as a forceinline or static helper 26 (identical code).
// Round 71: PRIVATE/inline/both 26 each; const actor/state/both 26 each; activation from the slot
// Boolean/sentinel 49/47; reset index signed/unsigned word 39/37 (sibling regressions; restored).
// @retail 0x1b3ee0
bool __stdcall function_1b3ee0(long actor_index, s_slot *slot)
{
	s_slot_66 *state = (s_slot_66 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	state->unknown0c = false;
	state->unknown10 = NONE;
	actor_reset_state(actor_index);
	actor->unknown314.bit0 = true;
	return true;
}

// @retail 0x1b3f60
short __stdcall function_1b3f60(long actor_index, s_slot *slot, bool active)
{
	s_slot_66 *state = (s_slot_66 *)slot;
	short result = g_46fbe8;

	if (state->unknown0c && state->unknown10 == NONE)
		return g_46fbe4;
	if (actor_get(actor_index)->unknown504 == 2)
	{
		if (state->unknown10 != NONE)
			function_1fb7e0(0x63, actor_index, NULL, actor_get(state->unknown10)->unknown018, NONE);
		return g_46fbe4;
	}
	return result;
}

// @retail 0x1b4240
bool __stdcall function_1b4240(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_64_data *state = (s_slot_64_data *)slot->unknownc;
	real seconds = function_259d0(&g_4e7408->unknown0, NULL, 0, 10.0f, 15.0f);
	long ticks;

	actor_unit_function_20ba60(actor_index, 0x61);
	state->unknown04 = 0;
	state->unknown0a = false;
	state->prop_index = actor->prop_index;
	state->unknown0d = false;
	seconds = g_510c54->field_2_3 * seconds;
	__asm
	{
		fld seconds
		fistp ticks
	}
	state->unknown06 = (short)ticks;
	memset(&actor->unknown314, 0, sizeof(actor->unknown314));
	return true;
}

// @retail 0x1b4310
short __stdcall function_1b4310(long actor_index, s_slot *slot, bool active)
{
	s_slot_64 *state = (s_slot_64 *)slot;
	short result = g_46fbe8;

	if (state->unknown19)
		result = 0x63;
	return result;
}

// @retail 0x1b4330
short __stdcall function_1b4330(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown07c != NONE && actor->unknown227 && element_502420_get(actor->unknown07c)->unknown26 == 1)
		result = 3;
	return result;
}

// @retail 0x1b4480
void __stdcall function_1b4480(long actor_index, s_slot *slot)
{
	s_slot_64 *state = (s_slot_64 *)slot;

	state->unknown2c = NONE;
}

long function_26bc60(long clump_index);

// @retail 0x1b4390
short __stdcall function_1b4390(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_64 *state = (s_slot_64 *)&actor->slots[level];
	state->unknown10++;
	state->unknown12--;
	short result = function_1b3e20(actor_index, level, active);
	if (state->unknown12 <= 0)
	{
		*(volatile short *)&state->unknown12 = 0;
		if (((volatile s_slot_64 *)state)->header.unknown4 != NONE && actor->slots[level + 1].type == 5)
			state->unknown19 = true;
	}
	if (state->unknown10 >= g_510c54->field_2_3 * 8 && !state->unknown16 && actor->unknown07c != NONE)
	{
		short type;
		switch ((short)function_26bc60(actor->unknown07c))
		{
		case 0: type = 0x5f; break;
		case 1: type = 0x5d; break;
		case 2: type = 0x5e; break;
		}
		function_1fb7e0(type, actor_index, NULL, NONE, NONE);
		state->unknown16 = true;
	}
	return result;
}

// @retail 0x1b4490
bool __stdcall function_1b4490(long actor_index, s_slot *slot)
{
	s_slot_64 *state = (s_slot_64 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	state->unknown10 = 0;
	state->unknown16 = false;
	state->prop_index = actor->prop_index;
	state->unknown19 = false;
	return true;
}

// @retail 0x1b44d0
short __stdcall function_1b44d0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (function_1e4e50(actor_index) && actor->prop_index != NONE && prop_node_view(prop_node_get(actor->prop_index)))
		result = 0x23;
	return result;
}

s_slot_handler_2 g_47e210 =
{
	{
		0x65, 2, 0, -2, 0,
		function_1b3670, function_1b3820, function_1b36e0, 0, NONE, {0},
		0, actor_behavior_release_prop, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b3600, 0, function_1b3880
};

s_slot_handler_2 g_47e260 =
{
	{
		0x64, 2, 0, -2, 0,
		function_1b39e0, function_1b3590, function_1b3a80, 0, NONE, {0},
		0, actor_behavior_release_prop, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b3600, 0, actor_behavior_64_update48
};

s_slot_handler_2 g_47e2b0 =
{
	{
		0x66, 2, 0x7ff, -2, 0,
		function_1b3ea0, function_1b3f60, function_1b3ee0, 0, NONE, {0},
		function_1c1520, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b3fd0, 0, slot_proc_nothing
};

/* the children of slot group 0x62 */
s_slot_child g_46f6e4[2] =
{
	{0x65, 0, -2, {0}, -1.0f, 0, 0},
	{5, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47e300 =
{
	{
		0x62, 1, 0, -2, 0,
		function_1b4330, function_1b4310, function_1b4240, 0, NONE, {0},
		0, actor_behavior_release_prop, 0, function_1b4480, 0, 0, 0
	},
	function_1b4390, 2, g_46f6e4
};

/* the children of slot group 0x63 */
s_slot_child g_46f710[3] =
{
	{0x64, 0, NONE, {0}, 0.0f, 0, 0},
	{0x66, 0, NONE, {0}, 0.0f, 0, 0},
	{5, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47e350 =
{
	{
		0x63, 1, 0, -2, 0,
		0, 0, function_1b4490, 0, NONE, {0},
		0, actor_behavior_release_prop, 0, function_1b4480, 0, 0, 0
	},
	function_1b3e20, 3, g_46f710
};

s_slot_handler_0 g_47e39c =
{
	0x1c, 0, 0, -2, 0, function_1b44d0
};

s_slot_handler_0 g_47e3b0 =
{
	0x1f, 0, 0, -2, 0, function_1b4560
};

long function_1e4a50(long index);
void function_26c180(long actor_index);
short actor_behavior_find_level(long owner_index, short type);

// score 59 (was 59): the 0x270750 call passes scratch/sector/target in other registers (its own
// convention; the extra byte also makes the loop's back-edge jump near, +5 bytes), and after the
// loop the sector and unit loads and the pool/actor registers are scheduled the other way.
// Earlier (frame 0x84 versus 0x88 and actor/scratch homes): source store order 145, movement result 142, positive membership scope 161,
// role names 144, native zero initialization 144, combined setup/result 141.
// frame 0x84 versus 0x88 and actor/scratch/best homes remain.
// Double best distance 214; failure-default active result 172; inactive success exit 188;
// ordered member rejection 144; native point memcpy 144.
// Full-width result 157; setup-scope best distance 155; early native workspaces 144;
// initialized query distance 167; shared member iterator 160 (frame 0x88).
// Nearest-distance name alone 144; candidate-availability result 160;
// identity after member lookup 144; nested query/distance scopes 144;
// distance before initial member lookup 144.
// frame 0x84 versus 0x88 and actor/scratch/best homes remain.
// Round 110: radius aggregate 140 plain; redundant zero stores omitted 167; selected actor name 144; movement scope 144; search construction scope 144.
// Library source clear 144; complete source aggregate 219; unsigned candidate 140, with clear 144;
// slot state view 140, with clear 144; captured successor 143; aggregate and unsigned candidate 219.
// Admission-result join 166; typed search buffer 140; both 166; candidate-first distance comparison 140;
// comparison with admission/buffer/both 166/140/166; native settings initialization 215.
// Native start/candidate/both/selected location receivers 159/139/161/139;
// start-selected/candidate-selected/all receivers 158/138/160; all with query-output scope 160.
// Round 70: element member iterator 142 (frame 0x88); with memset source 144; best index scoped in
// the found branch 132; NONE test as result = false in an else-if chain 59 (kept). On that: location
// pointer 62; getter at each use after the loop 88; loop member reused 59; best locals first 59;
// inverted selection test 59; slot data block view (unknownc) 59.
// Round 218: Chained source validity with data block/alone 64/64; chained sentinels 61;
// both chains 66; complete point after sector alone/with either/both chains 86/87/85/86.
// Data-block form does not apply; original body and source construction restored.
// Round 144, recovered result-first/iterator frame: native slot data 59f; movement
// result 154f, with slot 154f; search result 121f; search/movement 145f; selection/
// movement 137f; all results 154f, with slot 154f. Original body restored.
// Round 41: selected target-index snapshot 62; source-selected target index 62;
// all-phase target index 85; candidate-selected target index 85; candidate-selected location 84;
// all-phase location 84; source-selected location 62; source-selected target pointer 131.
// Round 155: selected actor getter at each use 88; selected pointer at the top 59; inverted 0x1f4460
// test 59; slot-state pointer local 59. BLOCKED-BY 0x270750 (its convention: target EAX, sector EDX, scratch ECX).
// @retail 0x1b3fd0
bool __stdcall function_1b3fd0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;
	if (actor->unknown07c == NONE)
		result = false;
	else if (actor->unknown040 && !((s_slot_66 *)slot)->unknown0c)
	{
		byte *movement = (byte *)function_1e4a50(actor->unknown054);
		function_26c180(actor_index);
		s_path_source source;
		memset(&source, 0, sizeof(source));
		source.radius = *(real *)(movement + 4);
		source.unknown04 = false;
		source.object_index = NONE;
		source.unknown0c = NONE;
		source.unknown4c = 0.0f;
		source.has_point = true;
		source.point = *(s_path_point *)&actor->unknown27c.point;
		source.unknown24 = actor->unknown27c.unknown10;
		source.unknown45 = true;
		source.unknown48 = 10.0f;
		s_path_settings settings;
		function_1f9240(actor_index, &settings);
		byte *scratch = ai_scratch_buffer_get();
		function_271300((s_type_f17a25 *)scratch, NULL, &settings, &source, 0);
		if (function_2715a0(scratch))
		{
			s_element_502420_member_iterator iterator;
			s_actor_view *member;
			iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
			iterator.index = NONE;
			long best_index = NONE;
			real best_distance = 50.0f;
			while ((member = element_502420_member_next(&iterator)) != 0)
			{
				if (member != actor && actor_behavior_find_level(iterator.index, 0x66) == NONE)
				{
					function_26c180(iterator.index);
					real distance;
					if (function_270750(scratch, member->unknown27c.unknown10,
							(s_actor_point_target const *)&member->unknown27c.point, &distance, 0, 0) && best_distance > distance)
					{
						best_index = iterator.index;
						best_distance = distance;
					}
				}
			}
			if (best_index != NONE)
			{
				s_actor_view *selected = actor_get(best_index);
				if (function_1f4460(actor_index, &selected->unknown27c.point, selected->unknown27c.unknown10, selected->unknown018, false))
				{
					actor->unknown4cc = 1.5f;
					((s_slot_66 *)slot)->unknown10 = best_index;
					selected->unknown314.bit1 = true;
				}
				else
					result = false;
			}
			else
				result = false;
		}
		else
			result = false;
		ai_scratch_buffer_release(scratch);
	}
	return result;
}


bool function_255b10(long actor_index, s_type_c3b527 const *point, long target_index, bool unknown);
void *function_1e5380(long actor_index);

// @retail 0x1b4560
short __stdcall function_1b4560(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    long object_index;
    if (actor->prop_index != NONE &&
        (object_index = actor->unknown018, !function_110ab0(object_index)) && g_4f55d0->unknown340)
    {
        s_prop_node_view *node = prop_node_get(actor->prop_index);
#pragma inline_depth(0)
        s_prop_view_fields *view = (s_prop_view_fields *)function_25d740((s_prop_node *)node);
#pragma inline_depth()
        if (view && view->unknown06 == 3 && *(long *)((byte *)view + 0x10) >= 0)
        {
            byte *entry = (byte *)function_1e5380(actor_index);
            if (entry && *(short *)(entry + 6) != NONE &&
                (*(long *)((byte *)actor + 0x7c8) == NONE ||
                 (g_510c54->game_time - *(long *)((byte *)actor + 0x7c8)) * g_510c54->rate >= *(real *)(entry + 0x28)) &&
                function_259a0(&g_4e7408->unknown0) < *(real *)(entry + 0x2c) && actor->unknown07c != NONE &&
                function_255b10(actor_index, (s_type_c3b527 *)((byte *)view + 0x18), NONE, false))
            {
                function_1fb7e0(0x21, actor_index, 0, *(long *)((byte *)node + 0x20), NONE);
            }
        }
    }
    return g_46fbe4;
}
