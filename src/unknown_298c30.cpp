#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_26c380.h"
#include "unknown_2c0d00.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

struct s_298c30
{
	byte field_0[0x20];
	real field_20;
	real field_24;
};

struct s_298c46
{
	byte field_0;
	bool field_1;
	byte field_2[2];
	real field_4;
	real field_8;
	point2f field_c;
	byte field_14[4];
	short field_18;
	byte field_1a[0xe];
};

// @retail 0x298c30
void function_298c30(long arg_0, s_298c30 const *arg_1, real arg_2,
	real arg_3, s_298c46 *arg_4, point2f const *arg_5)
{
	s_slot_object_view *local_0 = object_get(arg_0);
	bool local_1 = arg_4->field_4 < 0.0f;
	bool local_2 = arg_3 > 0.0;
	real local_3 = 10.0f;
	real local_4;
	if (local_0->type == 1)
	{
		local_4 = *(real *)(g_4e3b44[(*(long *)local_0) & 0xffff].bytes + 0x1f4);
		if (local_4 <= 0.0f)
			local_4 = 15.0f;
	}
	else
		local_4 = 15.0f;
	if ((local_1 || arg_4->field_1) && local_2)
		local_3 /= (arg_3 / local_4) * 0.5f + 1.0f;
	if (local_1 || arg_4->field_1)
		local_3 /= arg_1->field_20 * arg_2 * 0.03333333507180214f + 1.0f;
	if (arg_4->field_1)
		local_3 *= 0.8f;
	local_3 = (real)(local_3 / (fabs(arg_4->field_4) * 0.2f + 1.0f));
	if (arg_5)
		arg_4->field_8 = (arg_4->field_c.y * arg_5->y + arg_4->field_c.x * arg_5->x + 3.0f) * local_3 * 0.25f;
	else
		arg_4->field_8 = local_3;
}

s_298c46 g_5047f8[8];
short g_504938;

__forceinline real function_298df3(point2f *arg_0)
{
	real local_0 = (real)sqrt(arg_0->x * arg_0->x + arg_0->y * arg_0->y);
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->x *= local_1;
		arg_0->y *= local_1;
	}
	return local_0;
}

__forceinline real function_2990b0(real arg_0)
{
	real local_0;
	if (-1.0f > arg_0)
		local_0 = -1.0f;
	else
		local_0 = arg_0 > 1.0f ? 1.0f : arg_0;
	return (real)acos(local_0);
}

__forceinline void function_2992ba(short arg_0, bool arg_1, real arg_2, point2f const *arg_3,
	point2f const *arg_4, long arg_5, s_298c30 const *arg_6, real arg_7, real arg_8, point2f const *arg_9, point2f *arg_10)
{
	g_5047f8[arg_0].field_0 = arg_1;
	g_5047f8[arg_0].field_4 = arg_2;
	point2f &local_0 = *arg_10;
	if (arg_2 < 0.0f)
	{
		local_0.x = arg_3->x * -1.0f;
		local_0.y = arg_3->y * -1.0f;
	}
	else
		local_0 = *arg_3;
	g_5047f8[arg_0].field_1 = local_0.y * arg_4->y + local_0.x * arg_4->x < 0.0f;
	g_5047f8[arg_0].field_c = *arg_3;
	function_298c30(arg_5, arg_6, arg_7, arg_8, &g_5047f8[arg_0], arg_9);
	g_5047f8[arg_0].field_18 = arg_0;
}

// @retail 0x298d90
void function_298d90(volatile long arg_0, long arg_1, s_298c30 const *arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, real arg_6, point2f const *arg_7, point2f const *arg_8)
{
	(void)arg_0;
	point2f const *const *local_26 = &arg_3;
	s_slot_object_view *local_0 = object_get(arg_1);
	point2f local_1 = { arg_7->x + arg_8->x, arg_7->y + arg_8->y };
	point2f local_2 = *arg_8;
	function_298df3(&local_2);
	point2f local_27;
	point2f local_3[2];
	local_3[0].x = arg_4->x * -arg_6 + arg_7->x;
	local_3[0].y = arg_4->y * -arg_6 + arg_7->y;
	local_3[1].x = arg_4->x * arg_6 + arg_7->x;
	local_3[1].y = arg_4->y * arg_6 + arg_7->y;
	bool local_4 = local_2.x * (*local_26)->x + local_2.y * (*local_26)->y >= 0.0;
	real local_5 = *(real *)((byte *)local_0 + 0x8c) * (*local_26)->y + (*local_26)->x * *(real *)((byte *)local_0 + 0x88);
	short local_6 = 0;
	for (short local_7 = 0; local_7 < 2; local_7++)
	{
		point2f *local_8 = &local_3[local_7];
		real local_9 = (real)sqrt((local_1.x - local_8->x) * (local_1.x - local_8->x) + (local_1.y - local_8->y) * (local_1.y - local_8->y));
		bool local_10 = local_7 == 1;
		if (local_9 < arg_2->field_24)
		{
			real local_11 = (real)cos(1.5707963705062866);
			real local_12 = (real)sin(local_7 == 0 ? -1.5707963705062866 : 1.5707963705062866);
			g_5047f8[local_6].field_4 = 1.5707963705062866f;
			g_5047f8[local_6].field_c.x = local_11 * (*local_26)->x - local_12 * (*local_26)->y;
			g_5047f8[local_6].field_c.y = (*local_26)->y * local_11 + (*local_26)->x * local_12;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
			local_11 = (real)cos(-1.5707963705062866);
			local_12 = (real)sin(local_7 == 0 ? 1.5707963705062866 : -1.5707963705062866);
			g_5047f8[local_6].field_4 = -1.5707963705062866f;
			g_5047f8[local_6].field_c.x = local_11 * (*local_26)->x - local_12 * (*local_26)->y;
			g_5047f8[local_6].field_c.y = (*local_26)->y * local_11 + (*local_26)->x * local_12;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
		}
		else
		{
			real local_13 = arg_6 / local_9;
			if (local_13 > 1.0)
				local_13 = 1.0f;
			real local_14 = function_2990b0(local_13);
			point2f local_15 = { local_1.x - local_8->x, local_1.y - local_8->y };
			function_298df3(&local_15);
			real local_16 = local_15.y * arg_4->y + local_15.x * arg_4->x;
			if (local_16 > 1.0)
				local_16 = 1.0f;
			else if (local_16 < -1.0)
				local_16 = -1.0f;
			real local_17 = function_2990b0(local_16);
			real local_18;
			if (local_7 != 0)
				local_18 = local_4 ? 3.1415927410125732f - local_17 : local_17 + 3.1415927410125732f;
			else
				local_18 = local_4 ? local_17 : 6.2831854820251465f - local_17;
			real local_19[2];
			local_19[0] = local_18 + local_14;
			if (local_19[0] > 6.2831854820251465f)
				local_19[0] -= 6.2831854820251465f;
			local_19[1] = local_18 - local_14;
			long local_20 = 0;
			do
			{
				real local_21 = local_19[local_20];
				real local_22 = local_21 < 0.0f ? local_21 + 6.2831854820251465f : local_21 - 6.2831854820251465f;
				real local_23 = (real)sin(local_7 == 0 ? -local_22 : local_22);
				real local_24 = (real)cos(local_22);
				local_15.x = local_24 * (*local_26)->x - local_23 * (*local_26)->y;
				local_15.y = local_24 * (*local_26)->y + local_23 * (*local_26)->x;
				function_2992ba(local_6, local_10, local_21, &local_15, &local_2, arg_1, arg_2, local_13, local_5, arg_5, &local_27);
				local_6++;
				function_2992ba(local_6, local_10, local_22, &local_15, &local_2, arg_1, arg_2, local_13, local_5, arg_5, &local_27);
				local_6++;
				local_20++;
			} while (local_20 < 2);
		}
	}
	g_504938 = local_6;
}

struct s_299460
{
	long field_0;
	long field_4;
	point2f field_8;
	real field_10;
};

struct s_29946c
{
	short field_0;
	short field_2;
	byte field_4[8];
	s_299460 field_c[1];
};

short function_2108a0(long arg_0);

__forceinline s_pathfinding_data *function_29962f()
{
	if (*(long *)((byte *)g_4e0348 + 0xc4) > 0)
		return *(s_pathfinding_data **)((byte *)g_4e0348 + 0xc8);
	return NULL;
}

__forceinline real function_299a36(point3f const *arg_0, point3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_1->x - arg_0->x;
	arg_2->j = arg_1->y - arg_0->y;
	arg_2->k = arg_1->z - arg_0->z;
	real local_0 = (real)sqrt(arg_2->j * arg_2->j + (arg_2->i * arg_2->i + arg_2->k * arg_2->k));
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_2->i *= local_1;
		arg_2->j *= local_1;
		arg_2->k *= local_1;
	}
	else
		local_0 = 0.0f;
	return local_0;
}

__forceinline real function_299ae9(point3f const *arg_0, point3f const *arg_1)
{
	real local_0 = arg_0->x - arg_1->x;
	real local_1 = arg_0->y - arg_1->y;
	real local_2 = arg_0->z - arg_1->z;
	return (real)sqrt(local_0 * local_0 + local_1 * local_1 + local_2 * local_2);
}

__forceinline void function_29991b(point2f const *arg_0, real arg_1, real arg_2,
	point3f const *arg_3, real arg_4, point3f *arg_5)
{
	arg_5->x = arg_0->x * arg_2 - arg_0->y * arg_1 + arg_3->x;
	arg_5->y = arg_0->y * arg_2 + arg_0->x * arg_1 + arg_3->y;
	arg_5->z = arg_4;
}

__forceinline void function_2994a1(point2f const *arg_0, real arg_1,
	point2f const *arg_2, point2f *arg_3)
{
	arg_3->x = arg_1 * arg_2->x + arg_0->x;
	arg_3->y = arg_1 * arg_2->y + arg_0->y;
}

// @retail 0x299460
bool function_299460(long arg_0, s_298c46 *arg_1, s_298c30 const *arg_2,
	point3f const *arg_3, vector3f const *arg_4, vector3f const *arg_5,
	real arg_6, s_29946c const *arg_7)
{
	volatile bool local_0 = true;
	s_actor_view *local_1 = actor_get(arg_0);
	point2f local_2;
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)arg_3, arg_6, (point2f const *)arg_5, &local_2);
	}
	else
	{
		function_2994a1((point2f const *)arg_3, -arg_6, (point2f const *)arg_5, &local_2);
	}
	real local_3 = *(real *)((byte const *)arg_2 + 0x14);
	short local_4 = function_2108a0(*(long *)((byte *)local_1 + 0x28c));
	point3f *local_5 = (point3f *)((byte *)local_1 + 0x27c);
	vector3f local_6;
	point3f local_7;
	function_210770(local_4, arg_4, (vector3f *)&local_7);
	function_210770(local_4, arg_5, &local_6);
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)local_5, arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	else
	{
		function_2994a1((point2f const *)local_5, -arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	vector3f local_8 = { local_6.i * -1.0f, local_6.j * -1.0f, local_6.k * -1.0f };
	bool local_9 = false;
	bool local_10 = false;
	long local_11 = *(long *)((byte *)local_1 + 0x28c);
	if (local_11 == NONE)
		return true;
	s_path_trace_result local_12, local_13, local_14;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_6, local_3, NULL, &local_12);
	long local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_8, local_3, NULL, &local_13);
	long local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
	point2f local_17, local_18, local_19;
	if (arg_1->field_0)
	{
		local_17.x = local_6.i * -(arg_6 - local_12.distance);
		local_17.y = local_6.j * -(arg_6 - local_12.distance);
		local_18.x = local_6.i * -(local_13.distance + arg_6);
		local_18.y = local_6.j * -(local_13.distance + arg_6);
		local_19.x = local_6.i * -arg_6;
		local_19.y = local_6.j * -arg_6;
	}
	else
	{
		local_17.x = local_6.i * (local_12.distance + arg_6);
		local_17.y = local_6.j * (local_12.distance + arg_6);
		local_18.x = local_6.i * (arg_6 - local_13.distance);
		local_18.y = local_6.j * (arg_6 - local_13.distance);
		local_19.x = local_6.i * arg_6;
		local_19.y = local_6.j * arg_6;
	}
	point3f local_20 = *local_5;
	point3f local_21 = {local_6.i * local_12.distance + local_5->x, local_6.j * local_12.distance + local_5->y, local_5->z};
	point3f local_22 = {local_6.i * -local_13.distance + local_5->x, local_6.j * -local_13.distance + local_5->y, local_5->z};
	short local_23 = (short)ceil(fabs(arg_1->field_4) * 1.2732394933700562f);
	point3f local_24, local_25, local_26;
	for (short local_27 = 1; local_27 <= local_23; local_27++)
	{
		real local_28 = (arg_1->field_4 < 0.0f ? -local_27 : local_27) * 0.7853981852531433f;
		if (local_27 == local_23)
			local_28 = arg_1->field_4;
		if (!arg_1->field_0)
			local_28 *= -1.0f;
		real local_29 = (real)sin(local_28);
		real local_30 = (real)cos(local_28);
		if (!local_10)
			function_29991b(&local_17, local_29, local_30, &local_7, arg_3->z, &local_24);
		function_29991b(&local_19, local_29, local_30, &local_7, arg_3->z, &local_25);
		if (!local_9)
			function_29991b(&local_18, local_29, local_30, &local_7, arg_3->z, &local_26);
		real local_31 = function_299a36(&local_20, &local_25, &local_6);
		if (!local_10)
		{
			real local_32 = function_299ae9(&local_21, &local_24);
			local_10 = function_26c590(function_29962f(), &local_21, local_15, NONE, &local_6, local_32, NULL, &local_12);
			if (!local_10)
				local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		bool local_33 = function_26c590(function_29962f(), &local_20, local_11, NONE, &local_6, local_31, NULL, &local_14);
		if (!local_33)
			local_11 = ((s_sector_trace_result *)&local_14)->sector_index;
		if (!local_9)
		{
			real local_34 = function_299ae9(&local_22, &local_26);
			local_9 = function_26c590(function_29962f(), &local_22, local_16, NONE, &local_6, local_34, NULL, &local_13);
			if (!local_9)
				local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		if (local_33 || (local_10 && local_9))
		{
			arg_1->field_4 = arg_1->field_0 ? local_28 : -local_28;
			arg_1->field_8 *= (real)(local_27 - 1) / local_23;
			local_0 = false;
			return local_0;
		}
		local_20 = local_25;
		local_21 = local_24;
		local_22 = local_26;
	}
	if (arg_7)
	{
		short local_35 = NONE;
		real local_36 = 0.0f;
		for (short local_37 = 0; local_37 < arg_7->field_2; local_37++)
		{
			s_299460 const *local_38 = &arg_7->field_c[local_37];
			s_slot_object_view *local_39 = object_get(local_38->field_4);
			if (local_39->type == 0)
			{
				long local_40 = *(long *)((byte *)local_39 + 0x12c);
				if (local_40 != NONE && function_1df560(local_1->unknown024, actor_get(local_40)->unknown024))
					continue;
			}
			real local_41 = local_2.x - local_38->field_8.x;
			real local_42 = local_2.y - local_38->field_8.y;
			real local_43 = (real)sqrt(local_41 * local_41 + local_42 * local_42);
			real local_44 = local_38->field_10 + local_3 + arg_6;
			real local_45 = arg_6 - local_3 - local_38->field_10;
			if (!(local_44 > local_43 && local_43 > local_45))
				continue;
			point2f local_46 = { local_38->field_8.x - local_2.x, local_38->field_8.y - local_2.y };
			function_298df3(&local_46);
			bool local_47 = arg_4->j * local_46.y + arg_4->i * local_46.x >= 0.0f;
			real local_48 = local_46.x * arg_5->i + local_46.y * arg_5->j;
			if (local_48 > 1.0)
				local_48 = 1.0f;
			else if (local_48 < -1.0)
				local_48 = -1.0f;
			real local_49 = function_2990b0(local_48);
			real local_50;
			if (arg_1->field_0)
				local_50 = local_47 ? 3.1415927410125732f - local_49 : local_49 + 3.1415927410125732f;
			else
				local_50 = local_47 ? local_49 : 6.2831854820251465f - local_49;
			real local_51;
			if ((arg_1->field_4 > 0.0 && arg_1->field_4 > local_50) ||
				(!(arg_1->field_4 > 0.0) && local_50 > arg_1->field_4 + 6.2831854820251465f))
				local_51 = arg_6 > local_43 ? local_43 - local_45 : local_44 - local_43;
			else
			{
				real local_52 = arg_1->field_0 ? arg_1->field_4 + 3.1415927410125732f : -arg_1->field_4;
				real local_53 = (real)sin(local_52);
				real local_54 = (real)cos(local_52);
				point2f local_55 = { local_54 * arg_5->i - local_53 * arg_5->j, local_54 * arg_5->j + local_53 * arg_5->i };
				real local_56 = local_55.x * arg_6 + local_2.x - local_38->field_8.x;
				real local_57 = local_55.y * arg_6 + local_2.y - local_38->field_8.y;
				local_51 = local_3 + local_38->field_10 - (real)sqrt(local_57 * local_57 + local_56 * local_56);
				if (!(local_51 > 0.0f))
					continue;
			}
			local_51 /= local_3;
			if (local_35 == NONE || local_51 > local_36)
			{
				local_35 = local_37;
				local_36 = local_51;
			}
		}
		if (local_35 != NONE)
		{
			local_0 = false;
			if (local_36 > 1.0)
				arg_1->field_8 = 0.0f;
			else
				arg_1->field_8 *= 1.0f - local_36;
		}
	}
	return local_0;
}

union vector2f;
extern point2f *g_468778;
real function_30bf0(vector3f *arg_0);
real normalize2d(point2f *arg_0);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_26c180(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
bool function_1f9240(long arg_0, s_path_settings *arg_1);
void obstacle_list_group(s_obstacle_list *arg_0, real arg_1);
void __stdcall function_2c0d60(vector3f const *arg_0, long arg_1, byte arg_2, s_path_settings const *arg_3,
	s_obstacle_list *arg_4, s_obstacle_list *arg_5, point3f const *arg_6, real arg_7, vector3f const *arg_8,
	long arg_9, long arg_10);
real function_1f99d0(long arg_0, long arg_1, short arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, s_obstacle_list const *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2, vector2f const *arg_3,
	vector2f const *arg_4, vector2f const *arg_5, vector2f const *arg_6, real arg_7, bool arg_8);

__forceinline void function_29a20e(vector3f *arg_0)
{
	real local_0 = (real)sqrt(arg_0->i * arg_0->i + arg_0->j * arg_0->j);
	arg_0->k = 0.0f;
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->i *= local_1;
		arg_0->j *= local_1;
		arg_0->k *= local_1;
	}
}

// @retail 0x29a190
void function_29a190(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, real *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	s_298c30 const *local_2 = (s_298c30 const *)function_1e5450(arg_0, *(long *)local_1);
	bool local_3 = false;
	bool local_4 = false;
	if (arg_8 == 0)
		*(real *)(local_0 + 0x674) = *(real *)(local_0 + 0x670);
	vector3f local_5 = *(vector3f *)(local_0 + 0x290);
	*(bool *)(local_0 + 0x6d1) = false;
	function_29a20e(&local_5);
	real local_6 = g_468778->x * local_5.i + g_468778->y * local_5.j;
	if (-1.0f > local_6)
		local_6 = -1.0f;
	else if (local_6 > 1.0f)
		local_6 = 1.0f;
	local_6 = function_2990b0(local_6);
	if (g_468778->x * local_5.j - g_468778->y * local_5.i < 0.0f)
		local_6 = -local_6;
	if (!*(bool *)(local_0 + 0x5d0) || *(bool *)(local_0 + 0x5d2) || !local_2)
	{
		*(real *)(local_0 + 0x670) = 0.0f;
		*(bool *)(local_0 + 0x678) = false;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
	point3f local_7;
	function_b9dd0(arg_1, &local_7);
	real local_8 = local_2->field_20;
	real local_9 = (real)sqrt(arg_2->i * arg_2->i + arg_2->j * arg_2->j + arg_2->k * arg_2->k);
	function_26c180(arg_0);
	vector3f local_10;
	if (function_1f9760(arg_0, arg_1, &local_7, local_9, &local_4, &local_3, &local_10))
	{
		*arg_7 = true;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
	if (local_3)
	{
		local_10.k = 0.0f;
		if (function_30bf0(&local_10) == 0.0f)
			local_3 = false;
	}
	vector3f local_11 = {-local_5.j, local_5.i, 0.0f};
	s_obstacle_list local_12;
	s_obstacle_list *local_13 = &local_12;
	s_path_settings local_14;
	if (*(bool *)(local_0 + 0x478))
		local_13 = NULL;
	else
	{
		function_1f9240(arg_0, &local_14);
		local_12.group_count = 0;
		local_12.count = 0;
		local_12.flag0_count = 0;
		local_12.flag3_count = 0;
		*(short *)local_12.unknown08 = 0;
		function_2c0d60(0, arg_0, true, &local_14, &local_12, 0, &local_7, local_8 * 3.0f, arg_2, arg_1, NONE);
		obstacle_list_group(&local_12, *(real *)((byte const *)local_2 + 0x14));
	}
	s_298c46 *local_15 = (s_298c46 *)(local_0 + 0x654);
	bool local_16 = true;
	if (!*(bool *)(local_0 + 0x40) && *(bool *)(local_0 + 0x678))
	{
		real local_17 = local_15->field_c.y * local_5.j + local_15->field_c.x * local_5.i;
		if (!(local_17 > 0.0f))
			local_16 = false;
		else if (!(local_17 > 0.93f))
		{
			bool local_18 = local_15->field_c.y * local_11.j + local_15->field_c.x * local_11.i > 0.0f;
			if (local_15->field_0)
				local_16 = (local_15->field_4 > 0.0f && !local_18) || (local_15->field_4 < 0.0f && local_18);
			else
				local_16 = (local_15->field_4 > 0.0f && local_18) || (local_15->field_4 < 0.0f && !local_18);
		}
	}
	if (local_16)
	{
		function_298d90(arg_0, arg_1, local_2, (point2f *)&local_5, (point2f *)&local_11,
			local_3 ? (point2f *)&local_10 : NULL, local_8, (point2f *)&local_7, (point2f const *)arg_2);
		short local_19 = *(short *)(local_0 + 0x66c);
		if (local_19 >= 0 && local_19 < g_504938)
			g_5047f8[local_19].field_8 *= 1.25f;
		short local_20 = 0;
		bool local_21 = false;
		for (;;)
		{
			short local_22 = NONE;
			real local_23 = 0.0f;
			for (short local_24 = 0; local_24 < g_504938; local_24++)
			{
				if (g_5047f8[local_24].field_8 > local_23)
				{
					local_23 = g_5047f8[local_24].field_8;
					local_22 = local_24;
				}
			}
			if (local_22 == NONE)
				goto local_25;
			if (local_20 & (1 << local_22))
			{
				if (!(g_5047f8[local_22].field_8 > 0.0f))
					goto local_25;
				local_21 = true;
			}
			if (function_299460(arg_0, &g_5047f8[local_22], local_2, &local_7,
				&local_5, &local_11, local_8, (s_29946c const *)local_13))
				local_21 = true;
			else
			{
				local_20 |= 1 << local_22;
				if (!local_21)
					continue;
			}
			local_15->field_0 = g_5047f8[local_22].field_0;
			local_15->field_1 = g_5047f8[local_22].field_1;
			local_15->field_4 = g_5047f8[local_22].field_4;
			local_15->field_8 = g_5047f8[local_22].field_8;
			local_15->field_c = g_5047f8[local_22].field_c;
			local_15->field_18 = g_5047f8[local_22].field_18;
			*(bool *)(local_0 + 0x678) = true;
			if (!local_21)
				goto local_25;
			break;
		}
	}
	else
	{
		real local_26 = local_6 - *(real *)(local_0 + 0x668);
		if (local_15->field_0)
			local_15->field_4 -= local_26;
		else
			local_15->field_4 += local_26;
		local_15->field_4 = -3.1415927410125732f > local_15->field_4 ? -3.1415927410125732f :
			(local_15->field_4 > 3.1415927410125732f ? 3.1415927410125732f : local_15->field_4);
	}
	{
		real local_27 = 0.0f;
		if (local_3 && local_15->field_4 > 0.0f && !local_15->field_1)
			local_27 = function_1f99d0(arg_0, arg_1, (short)!local_4,
				(point2f const *)(local_0 + 0x548 + *(char *)(local_0 + 0x53a) * 0x1c),
				(point2f const *)&local_7, (point2f const *)&local_10, local_13);
		real local_28 = *(real *)((byte const *)local_2 + 0x30) * 0.01745329238474369f;
		real local_29 = *(real *)((byte const *)local_2 + 0x38);
		real local_30;
		if (local_15->field_0)
		{
			local_30 = (local_27 + local_15->field_4) * local_29;
			if (local_15->field_4 < 0.0f && local_15->field_1)
				local_30 = -local_28;
		}
		else
		{
			local_30 = (local_27 - local_15->field_4) * local_29;
			if (local_15->field_4 < 0.0f && local_15->field_1)
				local_30 = local_28;
		}
		if (-local_28 > local_30)
			local_30 = -local_28;
		else if (local_30 > local_28)
			local_30 = local_28;
		real local_31 = (real)sin(local_30);
		real local_32 = (real)cos(local_30);
		arg_3->k = 0.0f;
		arg_3->i = local_5.i * local_32 - local_31 * local_5.j;
		arg_3->j = local_31 * local_5.i + local_32 * local_5.j;
		if (function_30bf0(arg_3) == 0.0f)
			*arg_3 = *(vector3f *)(local_0 + 0x290);
		real local_33;
		if (local_15->field_4 >= 0.0f)
		{
			point2f local_34 = {arg_2->i, arg_2->j};
			normalize2d(&local_34);
			local_33 = function_1f9e70(arg_0, arg_1, (point2f *)&local_7, (vector2f *)&local_34,
				(vector2f *)arg_3, (vector2f *)&local_5, local_3 ? (vector2f *)&local_10 : NULL, local_9, false);
		}
		else
			local_33 = -1.0f;
		real local_35 = *(real *)((byte *)local_1 + 0x8c) * local_5.j + *(real *)((byte *)local_1 + 0x90) * local_5.k + *(real *)((byte *)local_1 + 0x88) * local_5.i;
		real local_36 = *(real *)((byte const *)local_2 + 0x68);
		real local_37;
		if (local_36 > 0.0f)
			local_37 = *(real *)((byte const *)local_2 + 0x6c);
		else
		{
			local_37 = 4.019999980926514f;
			local_36 = 6.0f;
		}
		real local_38 = (local_33 - local_35 / local_36) * local_37 + local_33;
		real local_39 = *(real *)((byte const *)local_2 + 0x60);
		if (local_38 > 0.0f && local_39 > 0.0f)
		{
			real local_40 = local_38 - *(real *)(local_0 + 0x674);
			local_38 = *(real *)(local_0 + 0x674) + (local_40 > local_39 ? local_39 : local_40);
		}
		*(real *)(local_0 + 0x670) = 0.0f > local_38 ? 0.0f : local_38;
		*arg_5 = local_38;
		*arg_5 = *arg_5 > *(real *)((byte const *)local_2 + 0x50) ?
			*(real *)((byte const *)local_2 + 0x50) :
			(-*(real *)((byte const *)local_2 + 0x50) > *arg_5 ?
				-*(real *)((byte const *)local_2 + 0x50) : *arg_5);
		*arg_5 = *arg_5 > 1.0f ? 1.0f : (-1.0f > *arg_5 ? -1.0f : *arg_5);
		*arg_6 = false;
		*arg_4 = 0;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
local_25:
	*(bool *)(local_0 + 0x5d0) = false;
	*(short *)(local_0 + 0x5d6) = 0;
	*(bool *)(local_0 + 0x6d1) = false;
	*arg_6 = true;
	*arg_4 = 0;
	*(real *)(local_0 + 0x668) = local_6;
}
