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

short function_2cbf0(long arg_1, short arg_2, dword arg_3,
	long arg_4, dword arg_5, long arg_6, long arg_7, byte *arg_8, short arg_9, byte arg_10,
	void *arg_11, long arg_12, real arg_13, long arg_14, long arg_15,
	bool arg_16, bool arg_17, bool arg_18, real arg_19, real arg_20,
	real arg_21, word arg_22, point3f const *arg_23, real arg_24);

// @retail 0x133f70
void function_133f70(s_133f70 const *record, bool arg_2)
{
	s_133f70 const *const original = record;
	s_133f70 copy;
	for (long i = 0; i < record->field_166; i++)
	{
		record = original;
		if (i <= 0 || !arg_2 || *(long *)original->field_48[i] == *(long *)original->field_48[0])
		{
			if (record->field_1c4 == 1)
			{
				byte const *definition = *(byte **)(g_4e0300->data + (*(long *)original->field_48[i] & 0xffff) * 12 + 8);
				if (definition[0xaa] == 2 ? (bool)*(short *)(g_4e3b44[*(long *)definition & 0xffff].bytes + 0x290) : false)
				{
					memcpy(&copy, original, sizeof(copy));
					copy.field_1cc = false;
					copy.field_1c0 = 0x1f;
					copy.field_1c4 = NONE;
					copy.field_1d0 = NONE;
					copy.field_1d4 = false;
					copy.field_1d8 = 0.0f;
					record = &copy;
				}
			}
			dword flags = ((record->field_170 & 4) << 13) | ((record->field_170 & 0x40) << 7)
				| (record->field_70[i] ? 0x800 : 0) | (record->field_16c ? 0x400 : 0)
				| record->field_1ac | record->field_1b0;
			for (long j = 0; j < record->field_18[i]; j++)
			{
				byte selected = record->field_75[i][j];
				long index;
				if (selected != 0xff)
					index = selected;
				else
					index = NONE;
				dword entry_flags = record->field_170;
				bool first = false;
				bool second = false;
				if (entry_flags & 2)
				{
					first = (bool)((entry_flags >> 3) & 1);
					second = (bool)((entry_flags >> 4) & 1);
				}
				function_2cbf0(index, record->field_16a, flags,
					record->field_5c[i], record->field_4[i], record->field_2c[i], j, record->field_48[i],
					record->field_1be, record->field_1bc, NULL, record->field_1d0, record->field_1b4,
					record->field_1c0, record->field_1c4, first, second, record->field_1d4,
					record->field_1d8, record->field_1dc, record->field_1e0,
					record->field_c6[i][j], &record->field_194, record->field_1a0);
			}
		}
	}
}
