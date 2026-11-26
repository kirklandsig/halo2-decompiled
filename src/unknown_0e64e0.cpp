// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E64E0.CPP: picks which of four axes a direction is snapped to:
   a normalized reference direction, its perpendicular (in the horizontal
   plane, or the global vector at 0x4687a4 in 3D), and their negations. The
   player facing code at 0x150400 calls it. When the reference has no length
   the fallback (the unit's vector at +0x70) is used and the direction is
   returned unchanged (flattened and renormalized in 2D). */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <math.h>

real function_30bf0(vector3f *v);
real function_1201a0(vector3f *v, vector3f const *fallback);

// @retail 0xe64e0
void function_e64e0(vector3f const *reference, vector3f const *direction, vector3f const *fallback,
	bool three_dimensional, vector3f *result, short *axis_index)
{
	vector3f axes[4];
	bool valid = true;

	if (three_dimensional)
	{
		axes[0] = *reference;
		if (function_30bf0(&axes[0]) == 0.0f)
		{
			axes[0] = *fallback;
			valid = false;
		}
		axes[2] = *g_4687a4;
	}
	else
	{
		axes[0] = *reference;
		axes[0].k = 0.0f;
		if (function_30bf0(&axes[0]) == 0.0f)
		{
			axes[0] = *fallback;
			valid = false;
		}
		axes[2].i = -axes[0].j;
		axes[2].j = axes[0].i;
		axes[2].k = 0.0f;
	}

	if (!valid)
	{
		if (axis_index)
			*axis_index = 0;
		if (result)
		{
			if (three_dimensional)
			{
				*result = *direction;
			}
			else
			{
				*result = *direction;
				result->k = 0.0f;
				function_1201a0(result, &axes[0]);
			}
		}
	}
	else
	{
		real fallback_dot;
		real direction_dot;
		real perpendicular_dot;

		for (long component = 0; component < 3; ++component)
			axes[1].n[component] = -axes[0].n[component];
		for (long component = 0; component < 3; ++component)
			axes[3].n[component] = -axes[2].n[component];
		if (three_dimensional)
		{
			fallback_dot = dot3f(fallback, &axes[0]);
			direction_dot = dot3f(direction, &axes[0]);
			perpendicular_dot = dot3f(direction, &axes[2]);
		}
		else
		{
			fallback_dot = fallback->i * axes[0].i + fallback->j * axes[0].j;
			direction_dot = direction->i * axes[0].i + direction->j * axes[0].j;
			perpendicular_dot = direction->i * axes[2].i + direction->j * axes[2].j;
		}

		real up_dot = dot3f(g_4687b0, direction);
		real threshold = 0.3f;

		if (fabs(fallback_dot) < 0.5f)
			threshold = 0.4f;

		bool along = fabs(direction_dot) > threshold;
		bool vertical = fabs(up_dot) > 0.9f;
		long index;

		if (along || vertical)
			index = direction_dot >= 0.0f ? 0 : 1;
		else
			index = perpendicular_dot >= 0.0f ? 2 : 3;

		if (axis_index)
			*axis_index = (short)index;
		if (result)
			*result = axes[index];
	}
}
