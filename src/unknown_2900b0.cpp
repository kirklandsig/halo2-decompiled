#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_2551c0.h"
#include "unknown_20fe20.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

point3f *function_b9dd0(long arg_0, point3f *arg_1);
bool function_fa1a0(real arg_0, real arg_1, point3f const *arg_2, point3f const *arg_3,
	real *arg_4, real const *arg_5, real const *arg_6, bool arg_7, vector3f *arg_8,
	real *arg_9, real *arg_10, real *arg_11, real *arg_12, real *arg_13);
real function_30bf0(vector3f *arg_0);

// score 13 (was 13): our call to the arc-aiming helper 0xfa1a0 passes its time-scale pointer
// in edx where retail pushes it (retail's callee loads its gravity pointer into edx at entry,
// ours into ebx, so ours has edx free for a parameter). The callee's body decides the registers
// (src/projectiles.cpp, unmatched). Tried in the callee: the high-arc flag and the time-scale
// pointer read once through their addresses 13/13; horizontal tests with the constant first 13;
// the horizontal length computed after the gravity value 13; the gravity operands swapped 13.
// Round 49: magnitude between vectors 13; split result declaration 13; retry-speed local 13;
// separate retry result 13; vector assignment helper 13; array component outputs 13;
// copy then scale 67; scaled local 101. Starting body restored.
// @retail 0x2900b0
bool function_2900b0(long arg_0, point3f const *arg_1, real arg_2, real arg_3, vector3f *arg_4)
{
	point3f local_0;
	vector3f local_1;
	real local_2;
	function_b9dd0(arg_0, &local_0);
	bool local_3 = function_fa1a0(arg_2, 1.0f, &local_0, arg_1, &arg_2, NULL, NULL, false,
		&local_1, &local_2, NULL, NULL, NULL, NULL);
	if (!local_3 && arg_2 > 0.0f && arg_3 > arg_2)
	{
		arg_2 += 0.01f;
		local_3 = function_fa1a0(arg_2, 1.0f, &local_0, arg_1, &arg_2, NULL, NULL, false,
			&local_1, &local_2, NULL, NULL, NULL, NULL);
	}
	if (local_3)
	{
		arg_4->i = local_1.i * local_2;
		arg_4->j = local_1.j * local_2;
		arg_4->k = local_1.k * local_2;
	}
	return local_3;
}

// @retail 0x28f290
bool function_28f290(long arg_0, vector3f *arg_1, vector3f *arg_2)
{
	volatile bool local_0 = false;
	s_handler_object_view *local_1 = handler_object_get(arg_0);
	byte *local_2;
	if (!local_1->flags134)
		local_2 = (byte *)local_1 + local_1->ai_offset;
	else
		local_2 = NULL;
	if (*(short *)(local_2 + 0x2c) == 0)
	{
		if (local_2[0x2b] & 1)
		{
			*arg_1 = *(vector3f *)(local_2 + 0x34);
			*arg_2 = *g_4687a8;
			return true;
		}
		if (local_2[0x2b] & 0x40)
		{
			point3f local_3;
			vector3f local_4;
			function_b9dd0(arg_0, &local_3);
			function_210c90(&local_3, (s_type_c3b527 const *)(local_2 + 0x30), &local_4);
			if (function_30bf0(&local_4) > 0.0f)
			{
				*arg_1 = local_4;
				*arg_2 = *g_4687a8;
				return true;
			}
		}
	}
	*arg_2 = *g_4687a4;
	return local_0;
}

struct s_28e600
{
	byte field_0[0xc];
	long field_c;
	long field_10;
	point3f field_14;
	long field_20;
	byte field_24[0x5c];
	vector3f field_80;
	vector3f field_8c;
};

struct s_28e601
{
	byte field_0[0x12c];
	word field_12c : 5;
	word field_12c_5 : 1;
	word field_12c_6 : 10;
	byte field_12e[0x4e];
	byte field_17c;
	byte field_17d[0x73];
	vector3f field_1f0;
};

void function_b9fc0(long arg_0, vector3f *arg_1, vector3f *arg_2);
vector3f *function_11d090(vector3f const *arg_0, vector3f *arg_1);

__forceinline void function_28e65d(vector3f const *a, vector3f const *b, vector3f *out)
{
	real k = a->i * b->j - a->j * b->i;
	real j = a->k * b->i - a->i * b->k;
	real i = a->j * b->k - a->k * b->j;
	out->i = i;
	out->j = j;
	out->k = k;
}

// @retail 0x28e600
void function_28e600(long arg_0, s_28e600 *arg_1)
{
	s_28e601 *local_0 = (s_28e601 *)object_get(arg_0);
	if (local_0->field_17c == 1 && !(bool)local_0->field_12c_5)
	{
		vector3f const *new_up = &local_0->field_1f0;
		arg_1->field_8c = *new_up;
		vector3f local_1;
		function_28e65d(&arg_1->field_8c, &arg_1->field_80, &local_1);
		if (function_30bf0(&local_1) > 0.0f)
		{
			vector3f *forward = &arg_1->field_80;
			function_28e65d(&local_1, &arg_1->field_8c, forward);
			if (function_30bf0(forward) != 0.0f)
				return;
		}
		function_b9fc0(arg_0, &arg_1->field_80, &arg_1->field_8c);
	}
	else
		function_11d090(&arg_1->field_80, &arg_1->field_8c);
}

struct s_28f8d0
{
	long field_0;
	long field_4;
	point3f field_8;
	short field_14;
	short field_16;
	long field_18;
};

struct s_28f3b0
{
	byte field_0[0x14];
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
};

struct s_295600_state;
void *function_1e4b50(long actor_index);
void function_295600(s_295600_state *state, bool planar, real magnitude,
	real minimum_time, real maximum_time, vector3f *result);

__forceinline real function_28f594(real const &arg_0)
{
	return arg_0 < -1.0f ? -1.0f : arg_0 > 1.0f ? 1.0f : arg_0;
}

__forceinline void function_28f500(vector3f *arg_0, real arg_1)
{
	arg_0->i = arg_1 * arg_0->i;
	arg_0->j = arg_1 * arg_0->j;
	arg_0->k = arg_0->k * arg_1;
}

__forceinline void function_28f521(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_0->i + arg_1->i;
	arg_2->j = arg_0->j + arg_1->j;
	arg_2->k = arg_0->k + arg_1->k;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

PRIVATE __forceinline void function_28f5b7(vector3f *arg_0)
{
    arg_0->i = PIN(arg_0->i, -1.0f, 1.0f);
    arg_0->j = PIN(arg_0->j, -1.0f, 1.0f);
    arg_0->k = PIN(arg_0->k, -1.0f, 1.0f);
}

// score 60 (was 56): the function is the same length but the first branch's
// block is laid out differently.
// @retail 0x28f3b0
void function_28f3b0(long arg_0, long arg_1)
{
	s_handler_object_view *local_0 = handler_object_get(arg_1);
	if (!local_0->flags134)
	{
		s_28e600 *local_1 = (s_28e600 *)((byte *)local_0 + local_0->ai_offset);
		if (local_1)
		{
			s_28f3b0 *local_2 = (s_28f3b0 *)function_1e4b50(arg_0);
			if (local_2)
			{
				vector3f local_3 = *g_4687a4;
				function_295600((s_295600_state *)((byte *)local_1 + 0x64), true, 1.0f,
					local_2->field_18, local_2->field_1c, &local_3);
				real local_4 = *(short *)((byte *)actor_get(arg_0) + 0x86) <= 1 ? local_2->field_20 : local_2->field_24;
				if (local_4 > fabs(local_3.i))
					local_3.i = 0.0f;
				if (local_4 > fabs(local_3.j))
					local_3.j = 0.0f;
				vector3f *local_5 = (vector3f *)((byte *)local_1 + 0x98);
				if (*(short *)((byte *)local_1 + 0x24) == 0)
				{
					real magnitude = function_30bf0(&local_3);
					if (magnitude > 0.0f)
					{
						local_1->field_80 = local_3;
						function_28e600(arg_1, local_1);
						local_5->i = local_2->field_14;
						local_5->j = 0.0f;
						local_5->k = 0.0f;
					}
				}
				else
				{
					function_28f500(&local_3, local_2->field_14);
					function_28f521(local_5, &local_3, local_5);
				}
				function_28f5b7(local_5);
			}
		}
	}
}

struct s_object_ai_data;
struct s_actor_object_sample;
void swarm_creature_reset_position(long arg_0, s_object_ai_data *arg_1);
void function_1e3a00(long arg_0, s_actor_object_sample *arg_1);

__forceinline void function_28f998(point3f const *arg_0, point3f const *arg_1, point3f *arg_2)
{
	arg_2->x = arg_0->x + arg_1->x;
	arg_2->y = arg_1->y + arg_0->y;
	arg_2->z = arg_1->z + arg_0->z;
}

__forceinline void function_28f9e1(point3f *arg_0, real const &arg_1)
{
	arg_0->x *= arg_1;
	arg_0->y *= arg_1;
	arg_0->z *= arg_1;
}

PRIVATE __forceinline void copy_point_28f8d0(point3f const *source, point3f *destination)
{
	*destination = *source;
}

/* the perception's centre: starting from the origin, the average of every
   creature's position; the first creature is read before the centre and the
   count are reset */
PRIVATE __forceinline void center_update_28f8d0(s_28f8d0 *arg_0)
{
	long local_2 = arg_0->field_18;
	copy_point_28f8d0(g_468788, &arg_0->field_8);
	arg_0->field_14 = 0;
	while (local_2 != NONE)
	{
		s_handler_object_view *local_3 = handler_object_get(local_2);
		s_28e600 *local_4;
		if (!local_3->flags134)
			local_4 = (s_28e600 *)((byte *)local_3 + local_3->ai_offset);
		else
			local_4 = NULL;
		swarm_creature_reset_position(local_2, (s_object_ai_data *)local_4);
		function_28f998(&arg_0->field_8, &local_4->field_14, &arg_0->field_8);
		arg_0->field_14++;
		local_2 = local_4->field_c;
	}
	if (arg_0->field_14 > 0)
	{
		real local_5 = 1.0f / arg_0->field_14;
		function_28f9e1(&arg_0->field_8, local_5);
	}
}

// @retail 0x28f8d0
void function_28f8d0(long arg_0)
{
	s_28f8d0 *local_0 = (s_28f8d0 *)perception_get(arg_0);
	if (local_0->field_4 != NONE)
	{
		byte *local_1 = (byte *)actor_get(local_0->field_4);
		center_update_28f8d0(local_0);
		memset(local_1 + 0x22c, 0, 0xbc);
		*(long *)(local_1 + 0x26c) = NONE;
		*(long *)(local_1 + 0x274) = NONE;
		*(long *)(local_1 + 0x28c) = NONE;
		*(bool *)(local_1 + 0x278) = false;
		function_1e3a00(local_0->field_18, (s_actor_object_sample *)(local_1 + 0x22c));
	}
}
