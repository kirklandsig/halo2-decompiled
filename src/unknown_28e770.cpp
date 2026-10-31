// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "object_markers.h"
#include "normalize3d.h"
#include "vector_products.h"

struct s_28e770
{
	byte field_0[0x1c];
	long field_1c;
	byte field_20[0x4a4 - 0x20];
	char field_4a4;
	char field_4a5;
	byte field_4a6[2];
	long field_4a8;
	byte field_4ac[0x888 - 0x4ac];
};

struct s_28e771
{
	byte field_0[8];
	point3f field_8;
	byte field_14[0x10];
	point3f field_24;
	bool field_30;
	byte field_31[3];
};

struct s_28e600
{
	word field_0;
	byte field_2[0x12];
	point3f field_14;
	long field_20;
	short field_24;
	short field_26;
	byte field_28[0x24];
	short field_4c;
	byte field_4e[2];
	long field_50;
	short field_54;
	bool field_56;
	bool field_57;
	point3f field_58;
	byte field_64[4];
	real field_68;
	byte field_6c[8];
	real field_74;
	byte field_78[8];
	vector3f field_80;
	vector3f field_8c;
	vector3f field_98;
};

struct s_28e772
{
	byte field_0[0x14];
	long field_14;
	byte field_18[0x58];
	vector3f field_70;
	vector3f field_7c;
	byte field_88[0xa4];
	word : 5;
	word field_12c : 1;
	word : 10;
	byte field_12e[0x134 - 0x12e];
	dword flags134;
	byte field_138[0x13a - 0x138];
	short ai_offset;
	byte field_13c[0x17c - 0x13c];
	byte field_17c;
	byte field_17d[0x73];
	vector3f field_1f0;
};

struct s_28e773
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
	real field_10;
};

struct s_object_ai_data;
struct s_28f0e0;
void *function_1e4b50(long arg_0);
void swarm_creature_update_countdown(long arg_0, s_object_ai_data *arg_1);
bool swarm_target_get_location(long arg_0, short arg_1, long *arg_2, point3f *arg_3, vector3f *arg_4);
real function_30bf0(vector3f *arg_0);
bool function_2900b0(long arg_0, point3f const *arg_1, real arg_2, real arg_3, vector3f *arg_4);
long function_1469f0(real arg_0);
byte function_118f70(long arg_0);
void function_b9a90(long arg_0);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
bool function_28f0e0(long arg_2, s_28f0e0 const *arg_1, vector3f const *arg_0, long arg_3);
bool function_28f290(long arg_0, vector3f *arg_1, vector3f *arg_2);
void function_28e600(long arg_0, s_28e600 *arg_1);
vector3f *g_4687b4;

__forceinline void function_28e774(point3f const *arg_0, point3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_0->x - arg_1->x;
	arg_2->j = arg_0->y - arg_1->y;
	arg_2->k = arg_0->z - arg_1->z;
}

// @retail 0x28e770
void __stdcall function_28e770(long arg_0, long arg_1)
{
	s_28e770 *local_0 = &((s_28e770 *)g_4f55f0->data)[arg_0 & 0xffff];
	s_28e772 *local_1 = (s_28e772 *)handler_object_get(arg_1);
	s_28e773 *local_2 = (s_28e773 *)function_1e4b50(arg_0);
	if (!local_1->flags134)
	{
		s_28e600 *local_3 = (s_28e600 *)((byte *)local_1 + local_1->ai_offset);
		if (local_3 && local_2)
		{
			s_28e771 *local_4 = (s_28e771 *)perception_get(local_0->field_1c);
			swarm_creature_update_countdown(arg_1, (s_object_ai_data *)local_3);
			short local_5 = local_0->field_4a4;
			short local_6 = local_0->field_4a5;
			long local_7 = local_0->field_4a8;
			if (local_3->field_0 & 1)
			{
				local_5 = 4;
				local_6 = 1;
				local_7 = local_1->field_14;
			}
			else if (local_3->field_50 != NONE)
			{
				local_5 = 5;
				local_6 = 2;
				local_7 = local_3->field_50;
			}
			point3f local_8;
			vector3f direction;
			long local_10;
			vector3f target_velocity;
			bool local_11 = swarm_target_get_location(local_7, local_6, &local_10, &local_8, &target_velocity);
			vector3f *local_14 = &local_1->field_70;
			local_3->field_80 = *local_14;
			local_3->field_8c = local_1->field_7c;
			local_3->field_98 = *g_4687a4;
			if (local_5 != local_3->field_24)
				local_3->field_26 = 0;
			else
				local_3->field_26++;
			vector3f local_13;
			s_object_marker local_15;
			switch (local_5)
			{
			case 0:
			{
				if (!local_4->field_30)
				{
					local_4->field_24 = local_4->field_8;
					local_4->field_30 = true;
				}
				real local_16 = local_4->field_24.x - local_3->field_14.x;
				real local_17 = local_4->field_24.y - local_3->field_14.y;
				real local_18 = local_4->field_24.z - local_3->field_14.z;
				real local_19 = local_18 * local_18 + local_17 * local_17 + local_16 * local_16;
				if (local_3->field_56 || local_19 > 9.0f)
				{
					if (local_19 < 1.0f)
						local_3->field_56 = false;
					else
					{
						vector3f goal_direction;
						function_28e774(&local_4->field_24, &local_3->field_14, &goal_direction);
						if (function_30bf0(&goal_direction) > 0.0f)
						{
							local_3->field_80 = goal_direction;
							local_3->field_98 = *g_4687a8;
						}
						local_3->field_56 = true;
					}
				}
				break;
			}
			case 1:
			case 2:
				if (local_11)
				{
					function_28e774(&local_8, &local_3->field_14, &local_13);
					real target_distance = function_30bf0(&local_13);
					if (target_distance > 0.0f && !local_3->field_57 && !(bool)local_1->field_12c)
					{
						if (local_3->field_4c > 0)
							goto local_21;
						local_3->field_80 = local_13;
						local_3->field_98 = *g_4687a8;
						if (dot3f(&local_3->field_80, local_14) > 0.95f)
						{
							if (function_2900b0(arg_1, (point3f const *)&target_velocity, 0.9f, 2.7f, (vector3f *)&local_3->field_58))
							{
								real local_20 = function_259d0((dword *)g_4e7408, NULL, 0, 1.0f, 3.0f);
								local_3->field_57 = true;
								local_3->field_4c = (short)function_1469f0(local_20);
							}
						}
					}
				}
				break;
			case 3:
			local_21:
				if (local_11)
				{
					function_28e774(&local_8, &local_3->field_14, &local_13);
					real local_22 = function_30bf0(&local_13);
					if (local_22 > 0.0f)
					{
						local_3->field_80 = local_13;
						if (local_22 > local_2->field_10)
						{
							local_3->field_98 = *g_4687a8;
							if (local_3->field_68 < 0.0f) local_3->field_68 = 0.0f;
							if (local_3->field_74 < 0.0f) local_3->field_74 = 0.0f;
						}
						else if (local_22 < local_2->field_c)
						{
							local_3->field_98 = *g_4687b4;
							if (local_3->field_68 > 0.0f) local_3->field_68 = 0.0f;
							if (local_3->field_74 > 0.0f) local_3->field_74 = 0.0f;
						}
					}
				}
				break;
			case 4:
				if (!function_118f70(arg_1) && (real)local_3->field_26 * g_510c54->rate > 1.0f)
				{
					function_b9a90(arg_1);
					if (local_11)
					{
						point3f placed_point;
						function_b9dd0(arg_1, &placed_point);
						vector3f offset;
						function_28e774(&placed_point, &local_8, &offset);
						offset.k = 0.0f;
						if (function_30bf0(&offset) > 0.0f)
						{
							vector3f_add((vector3f const *)&placed_point, &offset, (vector3f *)&local_8);
							if (function_2900b0(arg_1, &local_8, 0.9f, 2.7f, (vector3f *)&local_3->field_58))
								local_3->field_57 = true;
						}
					}
				}
				break;
			case 5:
				if (local_11)
				{
					short local_23 = 0;
					switch (local_3->field_54)
					{
					case 0: local_23 = function_b8d30(local_10, 0xf0005b4, &local_15, 1, false); break;
					case 1: local_23 = function_b8d30(local_10, 0xe0005b5, &local_15, 1, false); break;
					}
					if (local_23 > 0)
					{
						point3f placed_point;
						function_b9dd0(arg_1, &placed_point);
						function_28e774(&local_15.matrix.position, &placed_point, &direction);
						real local_24 = function_30bf0(&direction);
						if (!(local_24 > 0.0f))
							break;
						local_3->field_80 = direction;
						local_3->field_98 = *g_4687a8;
						if (local_24 < 0.2f)
						{
							function_28f0e0(arg_1, (s_28f0e0 const *)&local_15, &direction, local_10);
							break;
						}
						if (local_1->field_17c != 1 || (bool)local_1->field_12c)
							break;
						vector3f const *facing = &local_1->field_1f0;
						real local_25 = dot3f(facing, &direction);
						if (!(local_25 < -0.99f || (facing->k > 0.70710677f && local_25 > 0.99f)))
							break;
					}
					local_3->field_50 = NONE;
				}
				break;
			case 6:
				if (local_11)
				{
					function_28e774(&local_3->field_14, &local_8, &local_13);
					real local_26 = function_30bf0(&local_13);
					if (local_26 > 0.0f)
					{
						if (local_26 < local_2->field_4)
						{
							local_3->field_80 = local_13;
							local_3->field_98 = *g_4687a8;
							function_28e774(&local_3->field_14, &local_4->field_8, &direction);
							if (function_30bf0(&direction) > 0.0f)
							{
								real local_27 = (real)local_3->field_26 * g_510c54->rate;
								local_27 = local_27 < 0.0f ? 0.0f : local_27 > 5.0f ? 5.0f : local_27;
								vector3f contribution;
								vector3f_scale(&direction, (1.0f - local_27 * (1.0f / 3.0f)) * 2.0f, &contribution);
								vector3f_add(&local_3->field_80, &contribution, &local_3->field_80);
								if (function_30bf0(&local_3->field_80) == 0.0f)
									local_3->field_80 = local_13;
							}
						}
						else
						{
							vector3f_scale(&local_13, -1.0f, &local_3->field_80);
							local_3->field_98 = *g_4687a4;
						}
					}
				}
				break;
			case 7:
				function_28f290(arg_1, &local_3->field_80, &local_3->field_98);
				break;
			}
			local_3->field_24 = local_5;
			if (local_5 != 0) local_4->field_30 = false;
			function_28e600(arg_1, local_3);
		}
	}
}
