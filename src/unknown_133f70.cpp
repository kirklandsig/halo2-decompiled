#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

struct s_133f70
{
	long field_0;
	long field_4[4];
	byte field_14[4];
	long field_18[4];
	byte field_28[4];
	byte field_2c[4];
	byte field_30[0x18];
	byte *field_48[4];
	byte field_58[4];
	long field_5c[4];
	byte field_6c[4];
	byte field_70[4];
	byte field_74;
	byte field_75[4][16];
	byte field_b5[0xc6 - 0xb5];
	word field_c6[4][16];
	byte field_146[0x166 - 0x146];
	short field_166;
	byte field_168[2];
	short field_16a;
	bool field_16c;
	byte field_16d[3];
	dword field_170;
	byte field_174[0x194 - 0x174];
	point3f field_194;
	real field_1a0;
	byte field_1a4[8];
	dword field_1ac;
	dword field_1b0;
	real field_1b4;
	byte field_1b8[4];
	byte field_1bc;
	byte field_1bd;
	short field_1be;
	long field_1c0;
	long field_1c4;
	byte field_1c8[4];
	bool field_1cc;
	byte field_1cd[3];
	long field_1d0;
	bool field_1d4;
	byte field_1d5[3];
	real field_1d8;
	real field_1dc;
	real field_1e0;
};

typedef char c_133f70[sizeof(s_133f70) == 0x1e4 ? 1 : -1];

short function_2cbf0(long arg_1, byte arg_2, long arg_3, dword arg_4,
	long arg_5, dword arg_6, long arg_7, byte *arg_8, short arg_9, byte arg_10,
	void *arg_11, long arg_12, real arg_13, long arg_14, long arg_15,
	bool arg_16, bool arg_17, bool arg_18, real arg_19, real arg_20,
	real arg_21, byte arg_22, point3f const *arg_23, real arg_24);

// @retail 0x133f70
void function_133f70(s_133f70 const *arg_1, bool arg_2)
{
	volatile long local_1 = 0;
	s_133f70 const *local_12 = arg_1;
	if (local_12->field_166 > 0)
	{
		long local_13 = 0x18;
		long local_14 = 0;
		long local_15 = 0;
		do
		{
			s_133f70 const *local_2 = local_12;
			if (local_13 <= 0x18 || !arg_2 || *(long *)local_12->field_48[local_1] == *(long *)local_12->field_48[0])
			{
				struct { long field_0[4]; s_133f70 field_10; } local_16;
				s_133f70 &local_3 = local_16.field_10;
				if (local_2->field_1c4 == 1)
				{
					long local_4 = *(long *)*(byte *const *)((byte const *)local_2 + local_13 + 0x30);
					byte const *local_5 = *(byte **)(g_4e0300->data + (local_4 & 0xffff) * 12 + 8);
					if (local_5[0xaa] == 2)
					{
						byte const *local_6 = g_4e3b44[*(long *)local_5 & 0xffff].bytes;
						if (*(short const *)(local_6 + 0x290) != 0)
						{
							local_3 = *local_12;
							local_3.field_1c0 = 0x1f;
							local_3.field_1c4 = NONE;
							local_3.field_1cc = false;
							local_3.field_1d0 = NONE;
							local_3.field_1d4 = false;
							local_3.field_1d8 = 0.0f;
							local_2 = &local_3;
						}
					}
				}
				dword local_7 = ((local_2->field_170 & 4) << 13) | ((local_2->field_170 & 0x40) << 7)
					| (local_2->field_70[local_1] ? 0x800 : 0) | (local_2->field_16c ? 0x400 : 0)
					| local_2->field_1ac | local_2->field_1b0;
				for (long local_8 = 0; local_8 < *(long const *)((byte const *)local_2 + local_13); local_8++)
				{
					long local_9 = *((byte const *)local_2 + 0x75 + local_14 + local_8);
					if (local_9 == 0xff)
					local_9 = NONE;
					bool local_10 = false;
					bool local_11 = false;
					if (local_2->field_170 & 2)
					{
						local_10 = (bool)((local_2->field_170 >> 3) & 1);
						local_11 = (bool)((local_2->field_170 >> 4) & 1);
					}
					function_2cbf0(local_9, local_2->field_2c[local_1], local_2->field_16a, local_7,
						*(long const *)((byte const *)local_2 + local_13 + 0x44), *(dword const *)((byte const *)local_2 + local_13 - 0x14), local_8,
						*(byte *const *)((byte const *)local_2 + local_13 + 0x30), local_2->field_1be, local_2->field_1bc,
						NULL, local_2->field_1d0, local_2->field_1b4, local_2->field_1c0,
						local_2->field_1c4, local_10, local_11, local_2->field_1d4,
						local_2->field_1d8, local_2->field_1dc, local_2->field_1e0,
						(byte)*(word const *)((byte const *)local_2 + 0xc6 + local_15 + local_8 * 2), &local_2->field_194, local_2->field_1a0);
				}

			}

			local_1++;
			local_13 += 4;
			local_14 += 0x10;
			local_15 += 0x20;
		} while (local_1 < local_12->field_166);
	}
}
