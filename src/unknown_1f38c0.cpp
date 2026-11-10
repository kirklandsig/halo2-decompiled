// @flags /O2 /arch:SSE /Gr
// BLOCKED-BY the full check (#581, batch r294): enabling 0x1f38c0 and 0x1f6090 loses matched 0x1c83c0 and
// 0x1e2f50 under whole-program optimisation; neither byte-matches yet. fecheck: both behave like retail on 6/6
// in-game snapshots (0x1f38c0 takes the actor in eax; 0x1f6090 is __stdcall, 7 pushed arguments, ret 0x1c).
// The stub for 0x1f38c0 stays.
// Round 78 (m3 lane 13, fast): enabling this body alone (0x1f6090 as a stdcall stub) pads its callers 0x1e34e0, 0x1c83c0
// and 0x137fe0 by four bytes (`push ecx`). The cause is two named doubles below it: 0x29bab0's `double local_27` and
// 0x1f9e70's `double dot` (through 0x29bab0's call). With both removed (diagnostic) 0x1c83c0, 0x1e2f50 and 0x137fe0
// match and 0x1e34e0 passes this function's argument in EAX as retail does (25). This body's own frame stays 0x1c
// (retail 0x18, its lowest slot unused) either way. Diagnostic patch: build/r78/held-581-diag.patch (m3-lane13).
#if 0
/* UNKNOWN_1F38C0.CPP: an actor's movement update: it picks the path point or
   the plane it moves toward, runs the mover for the kind of thing it drives,
   notices when it stays in one place and chooses its movement speed */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1e3920.h"
#include "slot_handler.h"

void function_296580(long actor_index);
long function_1e1f20(long actor_index);
void *function_1e5280(long actor_index, long key);
long function_1e4a50(long character_index);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);
void function_1f3640(long actor_index);
void function_29a190(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, real *arg_5, bool *arg_6, bool *arg_7, short arg_8);
void __stdcall function_29ab90(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8);
void __stdcall function_29bab0(long arg_0, long arg_1, vector3f const *arg_2,
	vector3f *arg_3, short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8);
void function_29cbb0(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8);
void __stdcall function_1f5ab0(long actor_index, vector3f const *input, vector3f *movement_out,
	short *type_out, vector3f *facing_out, bool *blocked);
void __stdcall function_1f6090(long actor_index, vector3f const *input, vector3f *facing_out,
	short *type_out, vector3f *movement_out, bool *turning, bool *reached);

// Retail 0x1f38c0, disabled with its blocker above.
void function_1f38c0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short pass = 0;

	function_296580(actor_index);
	actor->unknown6d4 = actor->unknown290;
	actor->unknown6d2 = false;
	actor->unknown6d1 = true;

	vector3f normal;
	if (actor->unknown605 && function_1f3540(actor_index, &normal))
	{
		actor->unknown456 = true;
		actor->unknown458 = normal;
	}
	if (actor->unknown456)
	{
		actor->unknown5ec = actor->unknown458;
		actor->unknown5d0 = true;
		actor->unknown6d1 = false;
	}

	if (actor->unknown61e > 0)
	{
		actor->unknown61e--;
		actor->unknown480 = true;
	}
	if (actor->unknown480)
	{
		actor->unknown5d0 = false;
	}
	else
	{
		long weapon_index = function_1e1f20(actor_index);
		if (weapon_index != NONE)
		{
			byte const *properties = (byte const *)function_1e5280(actor_index, object_get(weapon_index)->tag_index);
			if (properties && (*properties & 1) && *(short *)((byte *)actor + 0x6fe) == 2)
				actor->unknown5d0 = false;
		}
	}

	do
	{
		switch (actor->unknown270)
		{
		case 0:
			if (actor->unknown229)
			{
				function_29bab0(actor_index, actor->unknown018, &actor->unknown5ec, &actor->unknown6d4,
					&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
			}
			else
			{
				long unit_index = actor->unknown018;
				byte const *unit = (byte const *)object_get(unit_index);
				if (unit[0xaa] == 0)
				{
					switch (unit[0x3dc])
					{
					case 2:
						function_29bab0(actor_index, unit_index, &actor->unknown5ec, &actor->unknown6d4,
							&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
						break;
					case 5:
						function_1f5ab0(actor_index, &actor->unknown5ec, &actor->unknown6d4,
							&actor->unknown5d6, &actor->unknown814, &actor->unknown5d2);
						break;
					default:
						function_1f6090(actor_index, &actor->unknown5ec, &actor->unknown6d4,
							&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2);
						break;
					}
				}
			}
			break;
		case 2:
			function_29bab0(actor_index, actor->unknown26c, &actor->unknown5ec, &actor->unknown6d4,
				&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
			break;
		case 3:
			function_29cbb0(actor_index, actor->unknown26c, &actor->unknown5ec, &actor->unknown6d4,
				&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
			break;
		case 4:
			function_29ab90(actor_index, actor->unknown26c, &actor->unknown5ec, &actor->unknown6d4,
				&actor->unknown5d6, &actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
			break;
		case 5:
			function_29a190(actor_index, actor->unknown26c, &actor->unknown5ec, &actor->unknown6d4,
				&actor->unknown5d6, (real *)&actor->unknown814, &actor->unknown5d3, &actor->unknown5d2, pass);
			break;
		default:
			actor->unknown5d0 = false;
			actor->unknown5d2 = false;
			actor->unknown5d6 = 0;
			break;
		}
		pass++;
	} while (!actor->unknown456 && actor->unknown5d2 && function_1f3100(actor_index) && actor->unknown5d0);
	if (actor->unknown5d2)
		actor->unknown5d0 = false;

	/* counts the ticks the actor stays within 0.03 of where it was */
	if (actor->unknown5d0)
	{
		vector3f delta;
		delta.i = actor->unknown5dc.x - actor->position.x;
		delta.j = actor->unknown5dc.y - actor->position.y;
		delta.k = actor->unknown5dc.z - actor->position.z;
		if (0.0009f > delta.k * delta.k + delta.j * delta.j + delta.i * delta.i)
		{
			actor->unknown5da++;
		}
		else
		{
			actor->unknown5dc = actor->position;
			actor->unknown5da = 0;
		}
		if (actor->unknown5da * g_510c54->rate > 2.0f)
		{
			actor->unknown5d8 = true;
			if (function_1f8660(actor_index))
			{
				if (++actor->unknown5b4 > 32)
					actor->unknown5b4 = 0;
				actor->unknown5da = 0;
			}
		}
	}
	else
	{
		actor->unknown5dc = actor->position;
		actor->unknown5d8 = false;
		actor->unknown5da = 0;
	}

	if (actor->unknown5e8 > 5)
	{
		function_1a8220(actor_index, 0x81, (short)real_to_long((real)g_510c54->field_2_3), 3, 1, 0x2f, 3);
		actor->unknown5e8 = 0;
	}
	if (actor->unknown5d0)
		actor->unknown6d1 = false;

	byte const *character = (byte const *)function_1e4a50(actor->unknown054);
	bool value = false;
	if ((!character || !(*character & 0x40)) && !actor->unknown267)
	{
		if (actor_get(actor_index)->unknown50c && actor->unknown5d0)
			value = actor->unknown44a;
		else
			value = actor->unknown449;
	}
	s_actor_view *current = actor_get(actor_index);
	current->unknown5d4 = value;
	if (value)
		current->flags810 |= 1;
	else
		current->flags810 &= ~1;

	function_1f3640(actor_index);
}
#endif
