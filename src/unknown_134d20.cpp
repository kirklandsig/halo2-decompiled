// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_134D20.CPP: the scenario's named interpolators (the block at
   +0x3c0 of g_4e0350) and their state in g_4e6740 (unknown_134d90.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_134d20.h"

struct s_tag_data
{
	long size;
	byte *address;
};

/* the scenario's interpolators, 0x18 bytes each */
struct s_scenario_interpolator
{
	long name;
	byte unknown04[8];
	s_tag_data function;
	short input_index;
	short scale_index;
};

struct s_scenario_interpolators_view
{
	byte unknown000[0x3c0];
	long interpolator_count;
	s_scenario_interpolator *interpolators;
};

/* g_4e6740: the name last looked up, then the state of each interpolator */
struct s_unknown_134d90;
extern s_unknown_134d90 *g_4e6740;

struct s_interpolator_globals
{
	long last_name;
	s_interpolator_state states[16];
};

real function_13b390(void const *function, real input, real range);
real function_13bb90(s_tag_data const *function, real input, real range);

// @retail 0x134fe0
real __stdcall function_134fe0(long index, real value)
{
	long local_1 = *(long volatile *)&index;
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	s_interpolator_state *state = &globals->states[local_1];
	real result = 0.0f;
	if (state)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		s_scenario_interpolator *definition = &scenario->interpolators[local_1];
		value += state->value18;
		short scale_index = definition->scale_index;
		long clamped = scale_index < 0 ? 0 : (scale_index > scenario->interpolator_count - 1 ? scenario->interpolator_count - 1 : scale_index);
		if (clamped == scale_index)
		{
			if (scenario->interpolators[scale_index].scale_index == NONE)
			{
				s_tag_data *function_data = &definition->function;
				real output = function_13b390(function_data, value, 0.0f);
				s_interpolator_state *scale_state = &globals->states[definition->scale_index];
				real scale = 0.0f;
				if (scale_state)
					scale = function_134fe0(definition->scale_index, scale_state->value);
				result = scale * output;
				byte *function = function_data->address;
				if (!(function[1] & 0xf0))
				{
					real lo = *(real *)(function + 4);
					real hi = *(real *)(function + 8);
					result = result < 0.0f ? 0.0f : (result > 1.0f ? 1.0f : result);
					result = (hi - lo) * result + lo;
				}
			}
		}
		else
			result = function_13bb90(&definition->function, value, 0.0f);
	}
	return result;
}

real function_134c50(real value);

// @retail 0x134fc0
inline real function_134fc0(long index)
{
	s_interpolator_state *state = &((s_interpolator_globals *)g_4e6740)->states[index];
	real result = 0.0f;
	if (state)
		result = function_134fe0(index, state->value);
	return result;
}

// @retail 0x1352e0
real function_1352e0(long name, bool flag)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state)
	{
		real value = state->value;
		if (flag)
			result = function_134fc0(index);
		else
			result = value;
	}
	return result;
}

__forceinline real interpolator_sample(s_interpolator_state *state, real value)
{
	if (state->flag1)
		value = function_134c50(value);
	else
		value = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
	return (1.0f - value) * state->start_value + state->target_value * value;
}

// @retail 0x135530
real function_135530(long name, real value, bool flag)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state)
	{
		value = interpolator_sample(state, value);
		result = value;
		if (flag)
			result = function_134fe0(index, value);
	}
	return result;
}

// @retail 0x1355b0
real function_1355b0(long name, real value, bool flag)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state && state->end_time > state->time10)
	{
		value = 0.0f > (value - state->time10) / (state->end_time - state->time10) ?
			0.0f : (value - state->time10) / (state->end_time - state->time10);
		value = interpolator_sample(state, value);
		result = value;
		if (flag)
			result = function_134fe0(index, value);
	}
	return result;
}

// @retail 0x135680
real function_135680(long name, real value, bool flag)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	real result = 0.0f;
	if (state && state->end_time > state->time10)
	{
		value += state->start_time;
		value = 0.0f > (value - state->time10) / (state->end_time - state->time10) ?
			0.0f : (value - state->time10) / (state->end_time - state->time10);
		value = interpolator_sample(state, value);
		result = value;
		if (flag)
			result = function_134fe0(index, value);
	}
	return result;
}

struct s_interpolator_tick_state
{
	real value;
	real start_value;
	real target_value;
	real start_time;
	real time10;
	real end_time;
	real value18;
	byte active : 1;
	byte flag1 : 1;
	byte flag2 : 1;
	byte : 5;
	byte unknown1d[3];
};

// @retail 0x134e00
void function_134e00(real delta)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	if (globals)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		if (scenario && scenario->interpolator_count > 0)
		{
			real time = g_510c54 && g_510c54->active ? g_510c54->game_time * g_510c54->rate : 0.0f;
			for (long i = 0; i < scenario->interpolator_count; i++)
			{
				s_interpolator_tick_state *state = (s_interpolator_tick_state *)&globals->states[i];
				s_scenario_interpolator *definition = &scenario->interpolators[i];
				if (state->active)
				{
					real value = 0.0f;
					if (state->end_time > state->time10)
					{
						real fraction = (time - state->time10) / (state->end_time - state->time10);
						if (fraction < 0.0f)
							fraction = 0.0f;
						if (state->flag1)
							fraction = function_134c50(fraction);
						else
							fraction = fraction < 0.0f ? 0.0f : (fraction > 1.0f ? 1.0f : fraction);
						value = (1.0f - fraction) * state->start_value + state->target_value * fraction;
					}
					state->start_time = time;
					state->value = value;
				}
				if (definition->function.address[0] == 3)
					state->flag2 = true;
				else
					state->flag2 = false;
			}
			for (long j = 0; j < scenario->interpolator_count; j++)
			{
				s_interpolator_state *state = &((s_interpolator_globals *)g_4e6740)->states[j];
				if (state->active)
				{
					short input = scenario->interpolators[j].input_index;
					long clamped = input < 0 ? 0 : (input > scenario->interpolator_count - 1 ? scenario->interpolator_count - 1 : input);
					if (clamped == input)
					{
						s_scenario_interpolator *source = &scenario->interpolators[input];
						if (source->input_index == NONE && source->scale_index == NONE)
							state->value18 += ((s_interpolator_globals *)g_4e6740)->states[input].value * delta;
					}
				}
			}
		}
	}
}

/* unknown_146240.cpp */
real game_time_get_seconds(void);

// @retail 0x134d20
s_interpolator_state *interpolator_get(long name, long *index_out)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	long index = NONE;
	if (globals && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
		{
			long interpolator_name = scenario->interpolators[i].name;
			if (interpolator_name && interpolator_name == name)
			{
				globals->last_name = name;
				index = i;
				break;
			}
		}
	}
	if (index != NONE)
	{
		if (index_out)
			*index_out = index;
		return &globals->states[index];
	}
	if (index_out)
		*index_out = NONE;
	return NULL;
}
// @retail 0x135110
long interpolator_start(long name, real target, real seconds)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	if (state)
	{
		real time = game_time_get_seconds();
		state->flag1 = false;
		state->active = true;
		state->start_time = time;
		state->time10 = time;
		state->start_value = state->value;
		state->target_value = target;
		state->end_time = time + seconds;
		state->value18 = 0.0f;
	}
	return index;
}

// @retail 0x135210
long interpolator_resume(long name)
{
	long index = NONE;
	s_interpolator_state *state = interpolator_get(name, &index);
	if (state && !TEST_FIELD_BIT(state->active))
	{
		real time = game_time_get_seconds();
		real elapsed = time - state->start_time;
		state->time10 += elapsed;
		state->end_time += elapsed;
		state->start_time = time;
		state->active = true;
	}
	return index;
}

// @retail 0x135290
bool interpolator_exists(long name)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	if (globals && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long index = 0; index < scenario->interpolator_count; index++)
		{
			long interpolator_name = scenario->interpolators[index].name;
			if (interpolator_name && interpolator_name == name)
			{
				globals->last_name = name;
				break;
			}
		}
	}
	return false;
}

/* the state of an interpolator by name (no retail function: always inlined) */
inline s_interpolator_state *interpolator_find(long name)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	long index = NONE;
	if (globals && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
		{
			long interpolator_name = scenario->interpolators[i].name;
			if (interpolator_name && interpolator_name == name)
			{
				globals->last_name = name;
				index = i;
				break;
			}
		}
	}
	if (index != NONE)
		return &globals->states[index];
	return NULL;
}

/* the same lookup with a single result, for the one caller that needs this shape */
__forceinline s_interpolator_state *interpolator_find_flagged(long name)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	s_interpolator_state *result = NULL;
	long index = NONE;
	if (globals && g_4e0350 && name)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
		{
			long interpolator_name = scenario->interpolators[i].name;
			if (interpolator_name && interpolator_name == name)
			{
				globals->last_name = name;
				index = i;
				break;
			}
		}
	}
	if (index != NONE)
		result = &globals->states[index];
	return result;
}

/* starts an interpolator that stops at its target */
// @retail 0x135180
long function_135180(long name, real target, real seconds)
{
	long index = interpolator_start(name, target, seconds);
	if (index != NONE)
		interpolator_find_flagged(name)->flag1 = true;
	return index;
}

// @retail 0x135330
real interpolator_get_value18(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	return state ? state->value18 : 0.0f;
}

// @retail 0x135450
real interpolator_get_time10(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	return state ? state->time10 : 0.0f;
}

// @retail 0x1354c0
real interpolator_get_end_time(long name)
{
	s_interpolator_state *state = interpolator_find(name);
	return state ? state->end_time : 0.0f;
}

// @retail 0x135750
void function_135750(void)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	if (globals && g_4e0350)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		for (long i = 0; i < scenario->interpolator_count; i++)
			globals->states[i].active = false;
	}
}

// @retail 0x135790
void function_135790(void)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	if (globals)
	{
		s_scenario_interpolators_view *scenario = (s_scenario_interpolators_view *)g_4e0350;
		if (scenario)
		{
			real time = game_time_get_seconds();
			for (long i = 0; i < scenario->interpolator_count; i++)
			{
				s_interpolator_state *state = &globals->states[i];
				if (!state->active)
				{
					real shifted = state->time10;
					real elapsed = time - state->start_time;
					shifted += elapsed;
					state->time10 = shifted;
					shifted = state->end_time;
					shifted += elapsed;
					state->end_time = shifted;
					state->start_time = time;
					state->active = true;
				}
			}
		}
	}
}

// @retail 0x135820
void function_135820(void)
{
	s_interpolator_globals *globals = (s_interpolator_globals *)g_4e6740;
	if (globals && g_4e0350)
	{
		long name = globals->last_name;
		if (name)
		{
			long index;
			s_interpolator_state *state = interpolator_get(name, &index);
			real target = 0.0f;
			real value = state ? state->value : target;
			if (value < 0.5f)
				target = 1.0f;
			interpolator_start(name, target, 2.0f);
		}
	}
}

real function_134c50(real value);


// @retail 0x1353a0
real function_1353a0(long name)
{
	real result = 0.0f;
	s_interpolator_state *state = interpolator_find_flagged(name);
	if (state && state->end_time > state->time10)
	{
		result = 0.0f > (state->start_time - state->time10) / (state->end_time - state->time10) ?
			0.0f : (state->start_time - state->time10) / (state->end_time - state->time10);
		if (state->flag1)
			result = function_134c50(result);
	}
	return result;
}
