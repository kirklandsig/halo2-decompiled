// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <math.h>
#include "unknown_1f4460.h"

/* slot handler 0x7c (g_47ef08) */

struct s_slot_7c
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	long unknown14;
	bool unknown18;
	byte unknown19[3];
	long prop_index;
	byte unknown20[0x40 - 0x20];
};

/* the prop state's position (unknown_25d690.cpp) */
struct s_prop_state_position
{
	long unknown00;
	point2f position;
	byte unknown0c[0x3c - 0xc];
	long unknown3c;
};

/* the object header (g_4e0300) as these callbacks read it: the object type
   at +3 */
struct s_object_header_type_view
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	byte *object;
};

/* (0, 0, -1), defined in unknown_11d180.cpp */
extern vector3f *g_4687bc;

real normalize2d(point2f *v);
void function_1f86a0(long index);
real function_1c9ee0(real fraction);
long function_baf80(long object_index);
point3f *function_b9dd0(long object_index, point3f *result);

PRIVATE inline byte object_header_type(long object_index)
{
	return ((s_object_header_type_view *)g_4e0300->data)[object_index & 0xffff].type;
}

PRIVATE inline real distance2d(point2f const *a, point2f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;

	return (real)sqrt(dx * dx + dy * dy);
}

PRIVATE inline real game_ticks_to_seconds(long ticks)
{
	return (real)ticks * g_510c54->rate;
}

PRIVATE inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

// @retail 0x1c0290
bool function_1c0290(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (actor->unknown018 != NONE)
	{
		s_object_marker marker;

		if (function_b8d30(actor->unknown018, 0x11000799, &marker, 1, false))
		{
			long object_index = actor->unknown018;

			if (function_b8d30(object_index, 0x1100079a, &marker, 1, false))
			{
				result = true;
			}
		}
	}
	return result;
}

PRIVATE __forceinline real function_1c0301(point2f const *arg_0, point2f const *arg_1)
{
	real local_0 = arg_0->x - arg_1->x;
	real local_1 = arg_0->y - arg_1->y;
	local_0 *= local_0;
	return (real)sqrt(local_0 + local_1 * local_1);
}

// @retail 0x1c0300
short __stdcall function_1c0300(long actor_index)
{
	long result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && function_1c0290(actor_index))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(node);
		s_actor_tag_entry_1e4f90 *entry;

		if ((prop->unknown3c != NONE || object_header_type(node->object_index) == 1) &&
			(entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index)) != NULL &&
			entry->unknown04 >= function_1c0301(&prop->position, (point2f *)&actor->position) &&
			(actor->times[3] == NONE || game_ticks_to_seconds(g_510c54->game_time - actor->times[3]) > entry->unknown18) &&
			function_1c9ee0(entry->unknown08) > function_259a0(&g_4e7408->unknown0))
		{
			result = 3;
		}
	}
	return result;
}

#pragma inline_depth(0)
PRIVATE __forceinline bool function_1c04f1(s_slot_7c const *arg_0, s_actor_tag_entry_1e4f90 const *arg_1)
{
	long local_0 = g_510c54->game_time;
	long local_1 = function_1469f0(*(real const *)&arg_1->unknown14);
	return local_0 - arg_0->unknown10 > local_1;
}

#pragma inline_depth(255)

// @retail 0x1c04f0
short __stdcall function_1c04f0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7c *state = (s_slot_7c *)slot;
	short result = g_46fbe4;

	if (actor->prop_index == state->prop_index && actor->prop_index != NONE && function_1c0290(actor_index))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(node);

		long local_4 = actor->unknown018;
		if (function_110ab0(local_4))
		{
			result = g_46fbe8;
		}
		else if (prop->unknown3c != NONE || object_header_type(node->object_index) == 1)
		{
			s_actor_tag_entry_1e4f90 *entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index);
			s_slot_7c *local_3 = (s_slot_7c *)*(s_slot *const volatile *)&slot;
            bool local_0 = *(volatile bool const *)&local_3->unknown18;
			real distance = distance2d(&prop->position, (point2f *)&actor->position);

			result = g_46fbe8;
			if (local_0)
			{
				int local_1 = g_510c54->game_time;
                long local_2 = game_seconds_to_ticks_round(4.0f);
                if (local_1 - local_3->unknown14 > local_2)
				{
					result = g_46fbe4;
				}
			}
			else if (distance > entry->unknown10 ||
				function_1c04f1(local_3, entry))
			{
				result = g_46fbe4;
			}
		}
	}
	if (result == g_46fbe4)
	{
		g_46eeb8[0x7c]->unknown8 = g_46f348;
	}
	return result;
}

// score 143 (was 143): frame 0xa0 versus 0xb0 (the 0x1697c0 collision output is 0x5c bytes; the
// shared header type is 0x4c), then the scratch registers are rotated by one from the 0x110ab0
// argument on (eax/ecx/edx), which also moves the request/collision arguments, the target copy and
// the inlined 0x1f4460 head. With the 0x5c output and the NONE store first, the only structural
// difference left is the target point: retail finishes point.z before the inlined head and adds the
// actor offset from memory (`add esi, [esp + 0x34]; cmp byte ptr [esi + 0x229], bl`); ours sinks
// point.z after the head's table load and goes through ECX (5 bytes longer).
// Tried: native slot-data block 143; full 0x5c collision output (frame 0xb0) 140, with native
// slot-data 140; complete point setter/vector setter/position-add/both vector producers 140 each;
// destination, request, destination/request, hit/probe, both point names, target/request/probe
// names and all workspace names 140 each; full output with shared scaled target, whole-point
// initialization or shared distance difference, alone and with native slot-data: eight forms 140;
// material initialized at declaration 138, aggregate target/early material 138; partial unit copy
// 104 and with aggregate target or CRT point copy 104 (restored); CRT point copy/aggregate target
// alone 140/140; uniform unit copy at entry 234, after position 161, with early material/slot data
// 161/161; collision NONE store before the vector math 141 (138 with the 0x5c output: gives
// retail's store order); with the 0x5c output, stamp through the existing 0x146650 getter 138,
// stamp stores swapped 141, slot pointer declared after the lookups 138, `== false` test 138,
// height declared at its definition 138. Deleting the whole stamp block (diagnostic) leaves 10
// aligned differences: everything after it then takes retail's registers.
// Full output with stamp byte/native payload, stamp signed byte, near byte, near signed byte,
// both bytes and the three mixed/signed combinations: eight forms 140 at 955 bytes.
// Native status declarations do not fix the post-stamp register rotation; all types/body restored.
// Round 289: Computed proximity bool/byte/short/long on the complete collision/native-payload graph; then each with one result exit:
// [247, 247, 252, 250, 231, 231, 226, 228]. All executable source and temporary types restored.
// Round 423: returned position with native/original payload 226/226; each with complete
// collision output 224/224; target declaration at publication gives 226/226/224/224.
// All reduced previews; complete original body and declarations restored.
// height declared at its definition 138 (deleting the whole stamp block, as a diagnostic, leaves 10
// aligned differences: everything after it then takes retail's registers); full output with stamp
// byte/native payload, stamp signed byte, near byte, near signed byte, both bytes and the three
// mixed/signed combinations: eight forms 140 at 955 bytes; computed proximity bool/byte/short/long
// on the complete collision/native-payload graph, then each with one result exit: 247, 247, 252, 250,
// 231, 231, 226, 228. Round 36, on the 0x5c output with the NONE store first: request fields in
// reverse order 140; `distance2d(...) < 2.0f` 138; target index stored before the point copy 138;
// point declared before the target 138; sums written collision point first 138; object lookup
// after the point 148.
// Round 48: the stamp's time read into a local first 135 (fixes the 0x110ab0 push register but
// moves the flag store after the time load; a one-use field copy, not kept); with the full 0x5c
// output 132; action declared before the unit test 143; time local after the flag store 143; named
// unit-test result 143; position declared first 143; state declared after the lookups 143.
// Round 19: unchanged; the frame and the collision output size still differ; no new form was found.
// Round 31: stamp test through the slot parameter 143; state declared after the lookups 143; no state
// local 143; both tests as `== false` 143; a unit index local for the two unit calls 115 (950 bytes, but
// the collision call still reads the field: a partial copy, not kept); one unit index at all three uses
// 219. Diagnostics: the stamp's flag store or time store removed 188/193, both stores unconditional 113;
// the 0x110ab0 argument stays in EDX in all three.
// after the point 148. Round 19: unchanged.
// Round 28 (lane 10): the scratch rotation follows the stamp block's temps: the stamp's time in a
// one-use local (diagnostic, not keepable) 135 gives retail's EAX for the 0x110ab0 argument, but the
// stamp's load moves above the flag store and the later temps (request actor, collision arguments)
// sit one register further on; plus a named 0x110ab0 result 135; plus the timing pointer in a local
// 135 (same code). Stamp through the existing 0x146650 getter 143 (same code); the slot cast at each
// use instead of the state local 143.
// Round 48: the stamp's time read into a local first 135 (fixes the 0x110ab0 push register but
// moves the flag store after the time load; a one-use field copy, not kept); with the full 0x5c
// output 132; action declared before the unit test 143; time local after the flag store 143; named
// unit-test result 143; position declared first 143; state declared after the lookups 143.
// Round 53 (plain): `distance2d(...) < 2.0f` on the original body 143; height declared at function scope
// 143; the request type stored after its fields 146; the stamp time stored before its flag 146;
// `height < entry->unknown0c` 143. `--fast` warns that 0x110ab0 builds differently in its reduced image.
// @retail 0x1c0670
bool __stdcall function_1c0670(long actor_index, s_slot *slot)
{
	s_slot_7c *state = (s_slot_7c *)slot;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor->prop_index);
	s_actor_tag_entry_1e4f90 *entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index);
	point3f position;

	function_b9dd0(node->object_index, &position);
	state->unknown0c = false;
	if (2.0f > distance2d((point2f *)&position, (point2f *)&actor->position))
	{
		real height;

		state->unknown0c = true;
		height = actor->position.z - position.z;
		if (height >= 0.0f && entry->unknown0c > height)
		{
			if (!state->unknown18)
			{
				state->unknown18 = true;
				state->unknown14 = g_510c54->game_time;
			}
			if (!function_110ab0(actor->unknown018))
			{
				s_unit_request action;

				action.type = 0x1a;
				action.type1a.unknown4 = 0;
				action.type1a.unknown6 = false;
				function_e6900(actor->unknown018, &action);
			}
		}
	}
	if (actor->unknown040)
	{
		vector3f vector;
		s_collision_result_1697c0 collision;

		vector.i = g_4687bc->i * 4.0f;
		vector.j = g_4687bc->j * 4.0f;
		vector.k = g_4687bc->k * 4.0f;
		position.x += g_4687b0->i;
		position.y += g_4687b0->j;
		position.z += g_4687b0->k;
		collision.unknown24 = NONE;
		if (function_1697c0(0x1808c0d, &position, &vector, node->object_index, actor->unknown018, &collision))
		{
			long object_index = function_baf80(node->object_index);
			s_actor_point_target target;

			point3f point;

			point.x = g_4687b0->i * 2.5f + collision.point.x;
			point.y = g_4687b0->j * 2.5f + collision.point.y;
			point.z = g_4687b0->k * 2.5f + collision.point.z;
			target.point = point;
			target.output_index = NONE;
			return function_1f4460(actor_index, &target, NONE, object_index, true);
		}
		return false;
	}
	return true;
}

// score 26 (was 26): the slot pointer and the actor index are loaded into the
// opposite registers here (index ECX, slot EAX; retail EAX/ECX, so retail scales the
// index before the actor-table load and its `and` is one byte shorter), and the reset
// stores are scheduled in another order (retail keeps the two NONE stores between the
// zero stores). 0x1b3ee0 has the same entry difference.
// Round 272: native slot-data block 26; with one clock 34, chained false stores 26,
// readonly actor 26, late actor 36, actor at use 36; existing reset with data/full slot 85/85.
// Round 70: array-index actor lookup 26; slot declared first 26; reset stores in retail's
// order 26 (identical code: the order is the scheduler's); existing reset function through a
// forceinline declaration 85 (stays a call: its file is /Ob1); prop copy first 37.
// Round 89: the reset's actor address offset-first, as an array element or through actor_get 26 each;
// bool result single exit 26; const state/actor 26; chained time stores 34. Matched 0x1b4490 has the
// same head without the reset and gets retail's eax/ecx: the offset's life into the reset decides it.
// BLOCKED-BY 0x1f86a0 (retail inlines it at link time; its /Ob1 file keeps ours out of line).
// @retail 0x1c0430
bool __stdcall function_1c0430(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7c *state = (s_slot_7c *)slot;

	state->unknown0c = false;
	state->unknown18 = false;
	state->unknown10 = g_510c54->game_time;
	state->unknown14 = g_510c54->game_time;
	state->prop_index = actor->prop_index;
	actor_reset_state(actor_index);
	return true;
}

// @retail 0x1c04c0
void __stdcall function_1c04c0(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->times[3] = g_510c54->game_time;
}

PRIVATE __forceinline bool function_1c0a31(point2f *arg_0)
{
	return normalize2d(arg_0) > 0.0f;
}

PRIVATE __forceinline void vector2d_from_points2d_1c0a30(point2f const *a, point2f const *b, point2f *out)
{
	out->x = b->x - a->x;
	out->y = b->y - a->y;
}

// @retail 0x1c0a30
void __stdcall function_1c0a30(long actor_index, s_slot *slot)
{
	s_slot_7c *state = (s_slot_7c *)slot;

	if (state->unknown0c)
	{
		s_actor_view *local_0 = actor_get(actor_index);
		local_0->flags810 = local_0->flags810 | 0x2000;
	}
	else
	{
		if (actor_get(actor_index)->unknown50c && actor_get(actor_index)->unknown504 == 1)
		{
			s_actor_view *actor = actor_get(actor_index);
			s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(prop_node_get(actor->prop_index));
			point2f direction;
			point2f facing;

			vector2d_from_points2d_1c0a30((point2f const *)&actor->position, &prop->position, &direction);
			if (function_1c0a31(&direction))
			{
				facing = *(point2f *)&actor->unknown290;
				if (function_1c0a31(&facing) && direction.y * facing.y + direction.x * facing.x > 0.9f)
				{
					actor->unknown482 = true;
				}
			}
		}
	}
}

// @retail 0x1c0b60
void __stdcall function_1c0b60(long actor_index, s_slot *slot, long index)
{
	s_slot_7c *state = (s_slot_7c *)slot;

	if (state->prop_index == index)
	{
		state->prop_index = NONE;
	}
}

s_slot_handler_2 g_47ef08 =
{
	{
		0x7c, 2, NONE, -2, 0,
		function_1c0300, function_1c04f0, function_1c0430, function_1c04c0, NONE, {0},
		0, function_1c0b60, slot_release_nothing, 0, 0, 0, 0
	},
	(t_slot_proc)function_1c0670, 0, function_1c0a30
};
