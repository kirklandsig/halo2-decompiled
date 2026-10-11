#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_2551c0.h"
// @flags /O2 /arch:SSE /Gr

struct s_28fa60
{
	word field_0;
	byte field_2[0xa];
	long field_c;
	long field_10;
	point3f field_14;
};

struct s_28fb50
{
	byte field_0[0xc];
	point3f field_c;
};

struct s_actor_object_sample;
void function_1e3a00(long arg_0, s_actor_object_sample *arg_1);

__forceinline s_28fa60 *function_28fa61(s_handler_object_view *arg_0)
{
	s_28fa60 *local_0;
	if (!arg_0->flags134)
		local_0 = (s_28fa60 *)((byte *)arg_0 + arg_0->ai_offset);
	else
		local_0 = NULL;
	return local_0;
}

__forceinline s_handler_object_view *function_28fa62(s_ai_object_iterator *arg_0)
{
	s_handler_object_view *local_0 = NULL;
	if (arg_0->next_index != NONE)
	{
		local_0 = handler_object_get(arg_0->next_index);
		arg_0->index = arg_0->next_index;
		s_28fa60 *local_1;
		if (!local_0->flags134 && (local_1 = (s_28fa60 *)((byte *)local_0 + local_0->ai_offset)) != NULL)
			arg_0->next_index = local_1->field_c;
		else
			arg_0->next_index = NONE;
	}
	return local_0;
}

// @retail 0x28fa60
bool function_28fa60(long arg_0, point3f const *arg_1, s_actor_object_sample *arg_2)
{
	long local_2 = NONE;
	s_ai_object_iterator local_0;
	s_perception_datum *local_10 = perception_get(arg_0);
	local_0.next_index = local_10->object_index;
	s_perception_datum *volatile local_11 = local_10;
	real local_1 = 3.4028234663852886e+38f;
	s_handler_object_view *local_3;
	while ((local_3 = function_28fa62(&local_0)) != NULL)
	{
		s_28fa60 *local_4 = function_28fa61(local_3);
		real local_5 = arg_1->x - local_4->field_14.x;
		real local_6 = arg_1->y - local_4->field_14.y;
		real local_7 = arg_1->z - local_4->field_14.z;
		real local_8 = local_5 * local_5 + local_6 * local_6 + local_7 * local_7;
		if (local_1 > local_8)
		{
			local_1 = local_8;
			local_2 = local_0.index;
		}
	}
	if (local_2 != NONE)
	{
		function_1e3a00(local_2, arg_2);
		return true;
	}
	return false;
}

__forceinline real function_28fb51(point3f const *arg_0, point3f const *arg_1)
{
	vector3f local_0 = { arg_0->x - arg_1->x, arg_0->y - arg_1->y, arg_0->z - arg_1->z };
	real local_1 = local_0.k * local_0.k;
	local_1 += local_0.i * local_0.i;
	local_1 += local_0.j * local_0.j;
	return local_1;
}

// @retail 0x28fb50
long function_28fb50(long arg_0, s_28fb50 const *arg_1, long arg_2)
{
	long local_2 = NONE;
	s_ai_object_iterator local_0;
	local_0.next_index = perception_get(arg_0)->object_index;
	real local_1 = 3.4028234663852886e+38f;
	s_handler_object_view *local_3;
	while ((local_3 = function_28fa62(&local_0)) != NULL)
	{
		s_28fa60 *local_4 = function_28fa61(local_3);
		real local_8 = function_28fb51(&arg_1->field_c, &local_4->field_14);
		if (local_4->field_0 & 1)
			local_8 *= 2.25f;
		else if (local_0.index == arg_2)
			local_8 *= 0.36000001430511475f;
		if (local_1 > local_8)
		{
			local_1 = local_8;
			local_2 = local_0.index;
		}
	}
	return local_2;
}
