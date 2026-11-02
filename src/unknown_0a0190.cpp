// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A0190.CPP: real math validity checks (decompiled by lane N for
   0x143120, which calls both) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_real_tolerance 0.001f

static inline bool function_x41b793(real value)
{
	return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

// @retail 0xa0190
inline bool function_a0190(vector3f const *vector)
{
	real difference = length_sq3f(vector) - 1.0f;

	return function_x41b793(difference) && fabs(difference) < k_real_tolerance;
}

__declspec(noinline) bool function_a0200(real a, real b);

// @retail 0xa0200
bool function_a0200(real a, real b)
{
	real difference = a - b;

	return function_x41b793(difference) && fabs(difference) < k_real_tolerance;
}

/* inline, so its callers assume it clobbers every xmm register, as retail's
   0xa6d50 does when it reloads its zero constant after the call */
// @retail 0xa7570
inline bool function_a7570(vector3f const *vector)
{
	return function_x41b793(vector->i) && function_x41b793(vector->j) && function_x41b793(vector->k);
}


// @retail 0xaba40
bool function_aba40(real const *values)
{
	if (function_x41b793(values[0]) && function_x41b793(values[1]) &&
		values[0] >= 0.0f && values[0] <= 6.2831854820251465f &&
		values[1] >= -1.5707963705062866f && values[1] <= 1.5707963705062866f)
		return true;
	return false;
}


/* inline, so its callers assume it clobbers every xmm register and edx, as 0x1f0870 does when it keeps
   its history in ebx and the offset on the stack across this call */
// @retail 0xa75d0
inline bool __stdcall function_a75d0(vector3f *vector, real maximum)
{
	real squared = length_sq3f(vector);
	if (squared > maximum * maximum)
	{
		real scale = (real)(maximum / sqrt(squared));
		vector->i = (real)(scale * vector->i);
		vector->j = (real)(scale * vector->j);
		vector->k = (real)(scale * vector->k);
		return true;
	}
	return false;
}


bool function_a74c0(vector3f const *forward, vector3f const *up);

struct s_z_transform_state
{
	long identifier;
	vector3f position;
	vector3f up;
	vector3f forward;
	vector3f linear_velocity;
	vector3f angular_velocity;
};

__declspec(noinline) bool function_ab9f0(s_z_transform_state const *state);

// @retail 0xab9f0
bool function_ab9f0(s_z_transform_state const *state)
{
	if (state && state->identifier != NONE)
	{
		if (function_a7570(&state->position))
		{
			if (function_a74c0(&state->up, &state->forward))
			{
				if (function_a7570(&state->linear_velocity))
				{
					if (function_a7570(&state->angular_velocity))
						return true;
				}
			}
		}
	}
	return false;
}


struct s_z_input_state
{
 real angles[2];
 real movement[2];
 word flags;
 byte unknown12[2];
 char first;
 char second;
 short third;
};

// @retail 0xab510
bool function_ab510(s_z_input_state const *state)
{
 if (state && function_aba40(state->angles) && function_x41b793(state->movement[0]) &&
  function_x41b793(state->movement[1]) && fabs(state->movement[0]) <= 1.0f && fabs(state->movement[1]) <= 1.0f &&
  !(state->flags & 0xfff0) &&
  (state->first == NONE || (state->first >= 0 && state->first < 4)) &&
  (state->second == NONE || (state->second >= 0 && state->second < 4)) &&
  (state->first == NONE || state->first != state->second) &&
  (state->third == NONE || (state->third >= 0 && state->third < 4)))
  return true;
 return false;
}
