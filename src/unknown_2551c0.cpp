// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2551C0.CPP: slot handler 0xc (handler at 0x47f790) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"
#include "unknown_0259a0.h"

/* the slot state of handler 0xc */
struct s_slot_0c_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[2];
	point3f point;
	real unknown1c;
};

/* the unit's offset at +0x346 */
struct s_unit_0c_view
{
	byte unknown000[0x346];
	short unknown346;
};

/* turns a vector about a unit axis by the angle whose sine and cosine are
   given */
static inline void function_x84d9e8(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real i = vector->i;
	real j = vector->j;
	real k = vector->k;
	real projection = (axis->k * k + axis->i * i + j * axis->j) * (1.f - cosine);

	vector->i = (axis->i * projection + i * cosine) - (axis->k * j - k * axis->j) * sine;
	vector->j = (j * cosine + projection * axis->j) - (k * axis->i - axis->k * i) * sine;
	vector->k = (axis->k * projection + k * cosine) - (i * axis->j - j * axis->i) * sine;
}

real function_30bf0(vector3f *v);
bool function_26bf10(long object_index);
long function_1fa7f0(void);
void function_cfec0(long unit_index);
bool function_e68c0(long type, long unit_index);

short g_470b4c = -1;
short g_470b50 = -2;

short __stdcall function_2551c0(long actor_index);
short __stdcall function_255570(long actor_index, s_slot *slot, bool active);
bool __stdcall function_2551f0(long actor_index, s_slot *slot);
void __stdcall function_255520(long actor_index, s_slot *slot);
void __stdcall function_255660(long actor_index, s_slot *slot);

s_slot_handler_2 g_47f790 =
{
	{
		0xc, 2, NONE, -2, 0,
		function_2551c0, function_255570, function_2551f0, function_255520, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, function_255660
};

// @retail 0x2551c0
short __stdcall function_2551c0(long actor_index)
{
	return (actor_get(actor_index)->unknown018 == NONE) ? 0 : 3;
}

// @retail 0x2551f0
bool __stdcall function_2551f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;

	if (actor->unknown018 == NONE)
	{
		return false;
	}

	state->timer = g_510c54->field_2_3 * 10;
	*(vector3f *)&state->point = actor->unknown290;
	state->unknown1c = 0.f;
	if (function_30bf0((vector3f *)&state->point) > 0.f)
	{
		if (function_26bf10(actor->unknown018))
		{
			function_26c180(actor_index);
			if (actor->unknown27c.unknown10 != NONE)
			{
				s_pathfinding_data *pathfinding = (s_pathfinding_data *)function_1fa7f0();
				real best_score = 0.f;
				bool found = false;
				vector3f forward;
				vector3f best_direction;

				function_210770(actor->unknown27c.point.output_index, &actor->unknown290, &forward);
				for (long i = 0; i < 8; i++)
				{
					real angle = (real)i * 0.7853982f;
					vector3f direction = forward;
					s_path_trace_result trace;

					function_x84d9e8(&direction, g_4687b0, (real)sin(angle), (real)cos(angle));
					function_26c590(actor->unknown27c.unknown10, &actor->unknown27c.point.point, &trace, pathfinding,
						&actor->unknown27c.point.point, NONE, &direction, 5.f, 0);

					real score = (dot3f(&forward, &direction) + 1.f) * (trace.distance * 0.2f);
					if (score > best_score)
					{
						best_score = score;
						best_direction = direction;
						found = true;
					}
				}

				if (found)
				{
					function_210770(actor->unknown27c.point.output_index, &best_direction, (vector3f *)&state->point);
				}
			}
		}

		function_cfec0(actor->unknown018);
		s_unit_0c_view *unit = (s_unit_0c_view *)object_get(actor->unknown018);
		if (*(short *)((byte *)unit + unit->unknown346 + 0x36) != 0)
		{
			function_e68c0(0x27, actor->unknown018);
		}
		return true;
	}
	return false;
}

// @retail 0x255520
void __stdcall function_255520(long actor_index, s_slot *slot)
{
	dword *flags = &handler_object_get(actor_get(actor_index)->unknown018)->flags134;
	*flags &= ~0x200000;
}

// @retail 0x255570
short __stdcall function_255570(long actor_index, s_slot *slot, bool active)
{
	s_slot_0c_state *state = (s_slot_0c_state *)slot;
	short result = g_470b50;
	real ticks;
	long rounded;

	state->timer--;
	if (state->timer <= 0)
	{
		return g_470b4c;
	}

	ticks = g_510c54->field_2_3 * 0.5f;
	__asm
	{
		fld ticks
		fistp rounded
	}

	if (state->timer > rounded)
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE)
		{
			long child_index = handler_object_get(actor->unknown018)->first_child_index;

			while (child_index != NONE)
			{
				s_handler_object_view *child = handler_object_get(child_index);
				if (child->type == 5)
				{
					return result;
				}
				child_index = child->next_object_index;
			}

			long timer;

			ticks = g_510c54->field_2_3 * 0.5f;
			__asm
			{
				fld ticks
				fistp timer
			}
			state->timer = (short)timer;
		}
	}

	return result;
}

// @retail 0x255660
void __stdcall function_255660(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;

	actor->unknown458 = *(vector3f *)&state->point;
	actor->unknown450 = 0x4000089;
	actor->unknown456 = true;
}
