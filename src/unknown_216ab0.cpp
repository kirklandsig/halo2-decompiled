#include "unknown_11c920.h"

// @flags /O2 /Gr

// @retail 0x216ab0
long function_216ab0(long index)
{
	switch (index)
	{
	case 0: return 1;
	case 1: return 4;
	case 2: return 5;
	case 3: return 2;
	case 4: return 7;
	case 5: return 8;
	case 6: return 9;
	default: __assume(0);
	}
}

// @retail 0x216b00
long function_216b00(long type)
{
	switch (type)
	{
	case 1: return 0;
	case 4: return 1;
	case 5: return 2;
	case 2: return 3;
	case 7: return 4;
	case 8: return 5;
	case 9: return 6;
	default: __assume(0);
	}
}

// @retail 0x216bd0
long function_216bd0(long type)
{
	long result = 0;
	long names[11] =
	{
		0x1b0006e9, 0x130006ea, 0x110006eb, 0x110006ec,
		0x140006ed, 0x170006ee, 0x170006ef, 0x100006f0,
		0x140006f1, 0x180006f2, 0x150006f3
	};
	if (type >= 0 && type < sizeof(names) / sizeof(names[0]))
		result = names[type];
	return result;
}
