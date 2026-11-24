// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "unknown_20fe20.h"
#include "globals.h"
#include <math.h>
#include "vector_products.h"
#include "normalize3d.h"

struct s_28dbd3
{
	long field_0;
	s_type_c3b527 field_4;
	real field_14;
	real field_18;
	long field_1c;
	byte field_20[0x10];
	long field_30;
};

struct s_28dbd0
{
	byte field_0[0x1c];
	point3f field_1c;
	vector3f field_28;
	vector3f field_34;
	vector3f field_40;
	byte field_4c[0x68];
	word field_b4;
	word field_b6;
	byte field_b8[0xc4 - 0xb8];
};

struct s_effect_owner;
struct s_object_ai_data;
struct s_28dab2;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_b8540(long arg_0);
void function_141ce0(real arg_0, real arg_1, real arg_2, transform4x3f *arg_3);
long function_1e06b0(long arg_0);
void swarm_creature_initialize(s_object_ai_data *arg_0);
bool function_28e320(long arg_0, long arg_1);
void swarm_creature_reset_position(long arg_0, s_object_ai_data *arg_1);
void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* forced inline in src/unknown_1eb8a0.cpp */

/* the steepest launch pitch, a quarter turn. Kept in double precision: retail folds the pitch range (upper minus
   lower bound, the constant at 0x45e5b8) from this value before it is rounded, so a real literal gives the
   neighbouring value. */
static const double k_maximum_launch_pitch = 1.5707964;

__forceinline void function_28dbd4(s_type_c3b527 const *arg_0, point3f *arg_1)
{
	if (arg_0->output_index == NONE || !function_2104b0(arg_0->output_index, &arg_0->point, arg_1))
		*arg_1 = arg_0->point;
}

__forceinline s_object_ai_data *function_28dbd5(s_handler_object_view *arg_0)
{
	s_object_ai_data *local_1;
	if (!arg_0->flags134)
		local_1 = (s_object_ai_data *)((byte *)arg_0 + arg_0->ai_offset);
	else
		local_1 = NULL;
	return local_1;
}

// @retail 0x28dbd0
long function_28dbd0(long arg_0, long arg_1, long arg_2, s_28dab2 const *arg_3, bool arg_4)
{
	long local_1 = NONE;
	s_28dbd3 const *local_4 = (s_28dbd3 const *)arg_3;
	s_28dbd0 local_0;
	function_b7930(&local_0, arg_2, NONE, NULL);
	function_28dbd4(&local_4->field_4, &local_0.field_1c);
	if (local_4->field_1c & 4)
	{
		transform4x3f local_1;
		function_141ce0(local_4->field_14, local_4->field_18, 0.0f, &local_1);
		local_0.field_28 = local_1.forward;
		local_0.field_34 = local_1.up;
	}
	else
	{
		real local_1 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -3.1415927f, 3.1415927f);
		real cosine = (real)cos(local_1);
		real sine = (real)sin(local_1);
		vector3f local_2;
		vector3f_assign(&local_2, cosine, sine, 0.0f);
		if (local_4->field_1c & 2)
		{
			real local_3 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 2.0f, 3.0f);
			real local_4 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.2617994f, k_maximum_launch_pitch);
			local_0.field_28 = local_2;
			vector3f local_5;
			cross3f_staged(&local_2, g_4687b0, &local_5);
			sine = (real)sin(local_4);
			cosine = (real)cos(local_4);
			rotate_direction(&local_2, &local_5, sine, cosine);
			vector3f_add_scaled((vector3f *)&local_0.field_1c, &local_2, 0.1f, (vector3f *)&local_0.field_1c);
			vector3f_add_scaled((vector3f *)&local_0.field_1c, g_4687b0, 0.1f, (vector3f *)&local_0.field_1c);
			vector3f_scale(&local_2, local_3, &local_0.field_40);
		}
		else
		{
			if (!arg_4)
			{
				vector3f local_3;
				local_3.i = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -0.5f, 0.5f);
				local_3.j = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -0.5f, 0.5f);
				local_3.k = 0.0f;
				vector3f_add((vector3f *)&local_0.field_1c, &local_3, (vector3f *)&local_0.field_1c);
			}
			local_0.field_28 = local_2;
		}
	}
	if (!local_4->field_30)
		*(long *)((byte *)&local_0 + 0xc) = function_1e06b0(arg_1);
	else
		*(long *)((byte *)&local_0 + 0xc) = local_4->field_30;
	local_0.field_b4 = 0;
	local_0.field_b6 = 0xb0;
	local_1 = function_b7b40(&local_0);
	if (local_1 != NONE)
	{
		s_handler_object_view *local_2 = handler_object_get(local_1);
		s_object_ai_data *local_3 = function_28dbd5(local_2);
		if (local_3)
		{
			swarm_creature_initialize(local_3);
			if (function_28e320(arg_0, local_1))
			{
				swarm_creature_reset_position(local_1, local_3);
				goto local_6;
			}
		}
		function_b8540(local_1);
		local_1 = NONE;
	}
local_6:
	return local_1;
}
