#include "unknown_11c920.h"
#include "frame_storage.h"
#include "effects.h"
#include "vector_products.h"

// @flags /O2 /arch:SSE /Gr

struct s_248df0
{
	byte field_00[8];
	real field_08;
	real field_0c;
	real field_10;
	word field_14[4];
	point3f field_1c;
	byte field_28[0x38 - 0x28];
	real field_38;
};


inline vector3f *function_4efb0(vector3f *arg_0);

PRIVATE __forceinline real function_248eff(real arg_0)
{
	real local_0 = arg_0 - (real)real_truncate(arg_0);
	if (local_0 < 0.f)
		local_0 += 1.f;
	return local_0;
}

PRIVATE __forceinline real function_248ef0(real const *arg_0)
{
	long local_0 = real_truncate(*arg_0);
	real local_1 = *arg_0 - (real)local_0;
	if (local_1 < 0.f)
		local_1 += 1.f;
	return local_1;
}

PRIVATE __forceinline real function_248df1(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	real local_0 = 0.f;
	s_248df0 const *local_1 = (s_248df0 const *)arg_3;
	s_particle_system_datum *local_2 = (s_particle_system_datum *)arg_1;
	byte const *local_5 = (byte const *)arg_2;
	switch (arg_0)
	{
	case 0:
		if (local_1) local_0 = local_1->field_08;
		break;
	case 1:
		if (local_1) local_0 = local_1->field_10;
		break;
	case 2:
	case 3:
		if (local_1)
		{
			long channel_index = arg_0 - 2;
			word channel = local_1->field_14[channel_index];
			local_0 = channel * (1.f / 65535.f);
		}
		break;
	case 14:
	case 15:
		if (local_1)
		{
			long channel_index = arg_0 - 12;
			word channel = local_1->field_14[channel_index];
			local_0 = channel * (1.f / 65535.f);
		}
		break;
	case 4:
		if (local_2) local_0 = local_2->unknown04;
		break;
	case 5:
	case 6:
		if (local_2)
		{
			real const *values = (real const *)local_2;
			long value_index = arg_0 + 5;
			local_0 = values[value_index];
		}
		break;
	case 7:
		if (local_5) local_0 = *(real const *)(local_5 + 0x1c);
		break;
	case 8:
		{
			s_game_time_globals *time_globals = g_510c54;
			long ticks = time_globals->game_time;
			real rate = time_globals->rate;
			local_0 = (real)ticks * rate;
		}
		break;
	case 9:
		{
			long effect_index = local_2->effect_index;
			if (effect_index != NONE)
			{
				s_effect_datum *effects = (s_effect_datum *)g_4ea93c->data;
				s_effect_datum *effect = &effects[effect_index & 0xffff];
				local_0 = effect->scale_a;
			}
			else
				return 0.f;
		}
		break;
	case 10:
		{
			long effect_index = local_2->effect_index;
			if (effect_index != NONE)
			{
				s_effect_datum *effects = (s_effect_datum *)g_4ea93c->data;
				s_effect_datum *effect = &effects[effect_index & 0xffff];
				local_0 = effect->scale_b;
			}
			else
				return 0.f;
		}
		break;
	case 11:
		if (local_1) local_0 = function_248ef0(&local_1->field_38);
		break;
	case 12:
		if (local_1 && local_5)
		{
			vector3f local_3;
			point3f const *position = &local_1->field_1c;
			local_3.i = position->x - g_485618.position.x;
			local_3.j = position->y - g_485618.position.y;
			local_3.k = position->z - g_485618.position.z;
			function_4efb0(&local_3);
			local_0 = function_248eff(dot3f(&local_3, &g_485618.forward) * local_1->field_38);
		}
		break;
	case 13:
		if (local_1 && local_5)
		{
			vector3f local_3;
			vector3f local_4;
			point3f const *position = &local_1->field_1c;
			local_3.i = position->x - g_485618.position.x;
			local_3.j = position->y - g_485618.position.y;
			local_3.k = position->z - g_485618.position.z;
			cross3f(&g_485618.up, &g_485618.forward, &local_4);
			function_4efb0(&local_3);
			local_0 = function_248eff((local_4.k * local_3.k + local_4.j * local_3.j + local_4.i * local_3.i) * local_1->field_38);
		}
		break;
	case 16:
		if (local_5) local_0 = local_5[3] * (1.f / 255.f);
		break;
	}
	return local_0;
}

// @retail 0x248df0
real function_248df0(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	return function_248df1(arg_0, arg_1, arg_2, arg_3);
}
