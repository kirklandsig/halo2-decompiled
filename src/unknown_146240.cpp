// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_146240.CPP: the global random seed, random unit vectors and the game
time globals */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include <xtl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

vector3f g_4417f0[1026];

// @retail 0x146240
void random_initialize(void)
{
	s_random_globals *globals = (s_random_globals *)function_123d40("random", "random", sizeof(s_random_globals));

	g_4e7408 = globals;
	globals->unknown0 = 0x78a8;
	dword time_seed = (dword)time(0);
	dword seed = (dword)rand();
	seed ^= GetTickCount();
	g_4e7408->seed = seed ^ time_seed;
}

// @retail 0x1462b0
dword function_1462b0(void)
{
	dword seed = (dword)time(0);
	seed = (dword)rand() ^ GetTickCount() ^ seed;
	return seed;
}

// @retail 0x1462e0
vector3f *random_unit_vector(vector3f *result, dword *seed)
{
	*seed = 1664525 * *seed + 1013904223;
	short index = (short)(((*seed >> 16) * 0x402) >> 16);
	*result = g_4417f0[index];
	return result;
}

PRIVATE inline void rotate_scalars_146320(vector3f *v, real axis_i, real axis_j, real axis_k, real sine, real cosine)
{
	real projection = (v->k * axis_k + v->j * axis_j + v->i * axis_i) * (1.f - cosine);
	real cross_j = v->k * axis_i - v->i * axis_k;
	real cross_k = v->i * axis_j - v->j * axis_i;
	v->i = v->i * cosine + projection * axis_i - (v->j * axis_k - v->k * axis_j) * sine;
	v->j = v->j * cosine + projection * axis_j - cross_j * sine;
	v->k = v->k * cosine + projection * axis_k - cross_k * sine;
}

// @retail 0x146320
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle)
{
	vector3f random_vector;
	vector3f axis;

	*result = *forward;

	random_unit_vector(&random_vector, seed);

	axis.i = random_vector.k * forward->j - random_vector.j * forward->k;
	axis.j = random_vector.i * forward->k - random_vector.k * forward->i;
	axis.k = random_vector.j * forward->i - random_vector.i * forward->j;
	real magnitude = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);

	if (!(fabs(magnitude) < 0.0001f))
	{
		real scale = 1.0f / magnitude;
		real axis_i = axis.i * scale;
		real axis_j = axis.j * scale;
		real axis_k = axis.k * scale;
		if (magnitude > 0.0f)
		{
			real angle = function_259d0(seed, __FILE__, __LINE__, min_angle, max_angle);
			real s = (real)sin(angle);
			real c = (real)cos(angle);

			rotate_scalars_146320(result, axis_i, axis_j, axis_k, s, c);
		}
	}
}

// @retail 0x146550
void game_time_globals_allocate(void)
{
	s_game_time_globals *globals = (s_game_time_globals *)function_123d40("game time", "game time", sizeof(s_game_time_globals));

	g_510c54 = globals;
	memset(globals, 0, sizeof(s_game_time_globals));
}

// @retail 0x1465b0
void game_time_globals_initialize(void)
{
	s_game_time_globals *globals = g_510c54;

	memset(globals, 0, sizeof(s_game_time_globals));
	short ticks = g_4e6948->field_2_3;
	globals->field_2_3 = ticks;
	globals->rate = 1.0f / (real)ticks;
	globals->scale = 1.0f;
	globals->active = true;
}

// @retail 0x146610
void game_time_stop(void)
{
	s_game_time_globals *globals = g_510c54;

	if (globals && globals->active)
	{
		globals->active = false;
	}
}

// @retail 0x146630
real game_time_get_seconds(void)
{
	s_game_time_globals *globals = g_510c54;

	if (globals && globals->active)
	{
		return (real)globals->game_time * globals->rate;
	}
	return 0.0f;
}

// @retail 0x146650
long function_146650(void)
{
	return g_510c54->game_time;
}

static inline long game_time_speed_to_ticks(s_game_options_view *options, real speed)
{
	real ticks = (real)options->field_2_3 / speed;
	volatile long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

/* sets the game speed: the ticks per second follow from it */
static __forceinline void game_time_apply_speed(real speed)
{
	s_game_options_view *options = g_4e6948;
	real rate = (speed > 0.01f) ? speed : 0.01f;
	long ticks = game_time_speed_to_ticks(options, rate);
	s_game_time_globals *globals = g_510c54;

	globals->rate = 1.0f / (real)ticks;
	globals->field_2_3 = (short)ticks;
	globals->scale = (real)options->field_2_3 / (real)ticks;
}

/* the fields of the game options and the 0x510c50 state read here */
struct s_game_time_options_view
{
	byte unknown00[0xc];
	char mode;
	byte unknown0d[0x1120 - 0xd];
	bool field_1120;
};

struct s_510c50_time_view
{
	byte unknown00[5];
	bool unlimited;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

/* the number of ticks the network simulation allows (sets synchronous) */
long function_680c0(bool *synchronous);
PRIVATE void game_time_set_speed_internal(real speed);

/* advances the game time by a frame: the speed ramp, then the whole ticks
   due, the fraction carried to the next frame */
// score 1 (was 1): the asynchronous path uses jle where retail jumps to the
// shared jg tick-limit test. Twenty variants: nested limits, flag widths,
// negated async test and explicit true/false async arms 1; reversed async
// comparison 2, reversed sync comparison 4; boolean assignments 12; inverted
// flag/async-first arms 35; shared final assignments 38; shared lower bound
// 39; shared compared value with rejection label 40, without rejection 95.
// Original retained: 476 bytes, as in retail.
// @retail 0x146660
void function_146660(real seconds, real *elapsed, long *ticks_due)
{
	s_game_time_globals *globals = g_510c54;
	s_game_time_options_view *options = (s_game_time_options_view *)g_4e6948;
	long ticks = 0;
	real elapsed_seconds = 0.0f;
	real leftover = 0.0f;

	if (options && options->field_1120)
	{
		if (globals->speed_duration > 0.0f)
		{
			globals->speed_timer += seconds;
			if (globals->speed_duration > globals->speed_timer)
			{
				real t = globals->speed_timer / globals->speed_duration;

				game_time_set_speed_internal((1.0f - t) * globals->speed_initial + globals->speed_final * t);
			}
			else
			{
				game_time_set_speed_internal(globals->speed_final);
				globals->speed_duration = 0.0f;
			}
		}
		if (options->field_1120 && !globals->unknown01 && globals->scale > 0.0f)
		{
			bool synchronous = false;
			bool unlimited = false;
			long maximum_ticks;
			real ticks_elapsed;

			if (g_510c50 && ((s_510c50_time_view *)g_510c50)->unlimited)
			{
				unlimited = true;
			}
			long mode = options->mode;
			/* modes 2 to 5 run on the network's ticks (the test function_138840 makes) */
			bool networked = false;
			if (mode >= 2 && mode <= 5)
			{
				networked = true;
			}
			if (!networked)
			{
				maximum_ticks = globals->field_2_3 * 5;
			}
			else
			{
				maximum_ticks = function_680c0(&synchronous);
				globals = g_510c54;
			}
			if (!synchronous && !unlimited)
			{
				real scale = globals->scale < 1.0f ? 1.0f : (globals->scale > 5.0f ? 5.0f : globals->scale);
				long limit_ticks = real_to_long(scale * 2.0f);

				if (maximum_ticks > limit_ticks)
				{
					maximum_ticks = limit_ticks;
				}
			}
			elapsed_seconds = globals->scale * seconds;
			ticks_elapsed = (real)globals->field_2_3 * elapsed_seconds + globals->leftover_ticks;
			long candidate_ticks = real_truncate(ticks_elapsed);
			/* a synchronous game runs the candidate only when it is one tick short of the network's count */
			bool limited = true;
			if (synchronous)
			{
				if (candidate_ticks <= maximum_ticks && candidate_ticks + 7 >= maximum_ticks &&
					candidate_ticks + 1 >= maximum_ticks && candidate_ticks + 1 <= maximum_ticks)
				{
					limited = false;
				}
			}
			else if (candidate_ticks <= maximum_ticks)
			{
				limited = false;
			}
			if (limited)
			{
				elapsed_seconds = (real)maximum_ticks * globals->rate;
				ticks = maximum_ticks;
			}
			else
			{
				ticks = candidate_ticks;
				leftover = ticks_elapsed - (real)candidate_ticks;
			}
		}
	}
	globals->leftover_ticks = leftover;
	if (elapsed)
	{
		*elapsed = elapsed_seconds;
	}
	if (ticks_due)
	{
		*ticks_due = ticks;
	}
}

// @retail 0x146840
long function_146840(void)
{
	s_game_time_globals *globals = g_510c54;

	if (globals->active && globals->unknown01)
	{
		return true;
	}
	return false;
}

PRIVATE void game_time_set_speed_internal(real speed);

// @retail 0x146860
void function_146860(real initial_speed, real speed, real duration)
{
	if (initial_speed < 0.2f)
	{
		initial_speed = 0.2f;
	}
	else if (initial_speed > 5.0f)
	{
		initial_speed = 5.0f;
	}

	if (speed < 0.2f)
	{
		speed = 0.2f;
	}
	else if (speed > 5.0f)
	{
		speed = 5.0f;
	}

	if (duration > 0.0f)
	{
		s_game_time_globals *globals = g_510c54;

		globals->speed_final = speed;
		globals->speed_timer = 0.0f;
		globals->speed_duration = duration;
		globals->speed_initial = initial_speed;
		game_time_apply_speed(initial_speed);
	}
	else
	{
		game_time_apply_speed(speed);
	}
}

// @retail 0x146980
PRIVATE void game_time_set_speed_internal(real speed)
{
	game_time_apply_speed(speed);
}
