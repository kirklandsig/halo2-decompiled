#include "unknown_11c920.h"
#include "unknown_20fe20.h"
#include "slot_handler.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

bool function_270240(long arg_0, long arg_1, long arg_2, point3f const *arg_3,
	vector3f const *arg_4, transform4x3f *arg_5);
bool function_26d290(point3f const *arg_0, point3f const *arg_1, long arg_2, long *arg_3);

// @retail 0x29da40
bool __stdcall function_29da40(long arg_0, s_type_c3b527 const *arg_1, long arg_2,
	short arg_3, bool arg_4, point3f const *arg_5, vector3f const *arg_6,
	s_type_c3b527 *arg_7, long *arg_8)
{
	vector3f const *const volatile *local_6 = &arg_6;
	bool local_0 = false;
	long local_1;
	switch (arg_3)
	{
	case 1: local_1 = arg_4 ? 0x10000229 : 0x110001b4; break;
	case 2: local_1 = arg_4 ? 0x1100022a : 0x120001b5; break;
	case 3: local_1 = arg_4 ? 0x0b0006c4 : 0x0c0006c3; break;
	default: goto local_5;
	}
	if (local_1 != NONE)
	{
		point3f local_2;
		transform4x3f local_3;
		if (arg_0 != NONE)
		{
			if (!function_270240(actor_get(arg_0)->unknown018, local_1, 0x5000049, arg_5, *local_6, &local_3))
				goto local_5;
			local_2 = local_3.position;
		}
		else
			local_2 = *arg_5;
		local_2.z = arg_5->z;
		long local_4;
		if (function_26d290(&arg_1->point, &local_2, arg_2, &local_4))
		{
			if (arg_7)
			{
				arg_7->point = local_2;
				arg_7->output_index = arg_1->output_index;
			}
			if (arg_8)
				*arg_8 = local_4;
			local_0 = true;
		}
	}
local_5:
	return local_0;
}

struct s_29db91
{
	short field_0;
	short field_2;
	short field_4;
};

struct s_29db90
{
	s_29db91 field_0[30];
	short field_b4;
};

struct s_29db92
{
	long field_0;
	long field_4;
};

struct s_29db93
{
	byte field_0[0x40];
	long field_40;
	s_29db92 *field_44;
};

struct s_29db94
{
	byte field_0[0x80];
	long field_80;
	short (*field_84)[2];
};

struct s_29db95
{
	byte field_0[0x30];
	long field_30;
	s_29db94 *field_34;
};

long function_1fa7f0(void);

__forceinline bool function_29dc40(dword const *arg_0, long arg_1)
{
	return (arg_0[arg_1 >> 5] & (1 << (arg_1 & 31))) != 0;
}

__forceinline void function_29dc62(dword *arg_0, short const &arg_1)
{
	arg_0[arg_1 >> 5] |= 1 << (arg_1 & 31);
}

// @retail 0x29db90
void function_29db90(short arg_1, short arg_0, s_29db90 *arg_2)
{
	if (arg_2->field_b4 < 30 && arg_0 >= 0 && arg_0 < *(long *)((byte *)g_4e0350 + 0x168))
	{
		s_29db95 *local_0 = &(*(s_29db95 **)((byte *)g_4e0350 + 0x16c))[arg_0];
		if (arg_1 >= 0 && arg_1 < local_0->field_30)
		{
			byte *local_1 = (byte *)function_1fa7f0();
			if (local_1 && *(long *)(local_1 + 0x6c) > 0)
			{
				s_29db93 *local_2 = *(s_29db93 **)(local_1 + 0x70);
				dword local_3[2];
				memset(local_3, 0, sizeof(local_3));
				s_29db94 *local_4 = &local_0->field_34[arg_1];
				for (short local_5 = 0; local_5 < local_4->field_80; local_5++)
				{
					short *local_6 = local_4->field_84[local_5];
					short local_7 = *local_6;
					if (!function_29dc40(local_3, local_7))
					{
						function_29dc62(local_3, local_7);
						for (short local_8 = 0; local_8 < arg_2->field_b4; local_8++)
						{
							if (arg_2->field_0[local_8].field_0 == local_7)
								return;
						}
						if (local_7 >= 0 && local_7 < local_2->field_40)
						{
							s_29db92 *local_9 = &local_2->field_44[local_7];
							for (short local_10 = 0; local_10 < local_9->field_0; local_10++)
							{
								arg_2->field_0[arg_2->field_b4].field_0 = *local_6;
								arg_2->field_0[arg_2->field_b4].field_2 = local_10;
								arg_2->field_0[arg_2->field_b4].field_4 = 0;
								arg_2->field_b4++;
								if (arg_2->field_b4 >= 30)
									return;
							}
							if (arg_2->field_b4 >= 30)
								return;
						}
					}
				}
			}
		}
	}
}
