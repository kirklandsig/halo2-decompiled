#include "unknown_11c920.h"

// @flags /O1 /arch:SSE /Gr

union field_param
{
	long i;
	real r;
};

typedef void (__stdcall *field_info_proc)(long *type, long *offset, field_param *param);

void __stdcall function_236ae6(long *type, long *offset, field_param *param);
void __stdcall function_236b0d(long *type, long *offset, field_param *param);
void __stdcall function_236b34(long *type, long *offset, field_param *param);
void __stdcall function_236b5b(long *type, long *offset, field_param *param);
void __stdcall function_236b7c(long *type, long *offset, field_param *param);
void __stdcall function_236b9d(long *type, long *offset, field_param *param);
void __stdcall function_236bbe(long *type, long *offset, field_param *param);
void __stdcall function_236be5(long *type, long *offset, field_param *param);
void __stdcall function_236c0c(long *type, long *offset, field_param *param);
void __stdcall function_236c33(long *type, long *offset, field_param *param);
void __stdcall function_236c5a(long *type, long *offset, field_param *param);
void __stdcall function_236c81(long *type, long *offset, field_param *param);
void __stdcall function_236ca8(long *type, long *offset, field_param *param);
void __stdcall function_236ccf(long *type, long *offset, field_param *param);
void __stdcall function_236ceb(long *type, long *offset, field_param *param);
void __stdcall function_236d0c(long *type, long *offset, field_param *param);
void __stdcall function_236d2d(long *type, long *offset, field_param *param);
void __stdcall function_236d54(long *type, long *offset, field_param *param);
void __stdcall function_236d7b(long *type, long *offset, field_param *param);
void __stdcall function_236d9c(long *type, long *offset, field_param *param);
void __stdcall function_236dc3(long *type, long *offset, field_param *param);
void __stdcall function_236de4(long *type, long *offset, field_param *param);
void __stdcall function_236e05(long *type, long *offset, field_param *param);
void __stdcall function_236e26(long *type, long *offset, field_param *param);
void __stdcall function_236e47(long *type, long *offset, field_param *param);
void __stdcall function_236e6e(long *type, long *offset, field_param *param);
void __stdcall function_236e8f(long *type, long *offset, field_param *param);
void __stdcall function_236eb6(long *type, long *offset, field_param *param);
void __stdcall function_236edd(long *type, long *offset, field_param *param);
void __stdcall function_236f04(long *type, long *offset, field_param *param);
void __stdcall function_236f25(long *type, long *offset, field_param *param);
void __stdcall function_236f4c(long *type, long *offset, field_param *param);
void __stdcall function_236f6d(long *type, long *offset, field_param *param);
void __stdcall function_236f8e(long *type, long *offset, field_param *param);
void __stdcall function_236fb5(long *type, long *offset, field_param *param);
void __stdcall function_236fdc(long *type, long *offset, field_param *param);
void __stdcall function_236ffd(long *type, long *offset, field_param *param);
void __stdcall function_237024(long *type, long *offset, field_param *param);
void __stdcall function_23704b(long *type, long *offset, field_param *param);
void __stdcall function_237072(long *type, long *offset, field_param *param);
void __stdcall function_23708e(long *type, long *offset, field_param *param);
void __stdcall function_2370ac(long *type, long *offset, field_param *param);
void __stdcall function_2370d3(long *type, long *offset, field_param *param);
void __stdcall function_2370fa(long *type, long *offset, field_param *param);
void __stdcall function_237121(long *type, long *offset, field_param *param);
void __stdcall function_237148(long *type, long *offset, field_param *param);
void __stdcall function_23716f(long *type, long *offset, field_param *param);
void __stdcall function_237196(long *type, long *offset, field_param *param);
void __stdcall function_2371bd(long *type, long *offset, field_param *param);
void __stdcall function_2371e4(long *type, long *offset, field_param *param);
void __stdcall function_23720b(long *type, long *offset, field_param *param);
void __stdcall function_237232(long *type, long *offset, field_param *param);
void __stdcall function_237259(long *type, long *offset, field_param *param);
void __stdcall function_237280(long *type, long *offset, field_param *param);
void __stdcall function_2372a1(long *type, long *offset, field_param *param);
void __stdcall function_2372c2(long *type, long *offset, field_param *param);
void __stdcall function_2372e3(long *type, long *offset, field_param *param);
void __stdcall function_23730a(long *type, long *offset, field_param *param);
void __stdcall function_23732b(long *type, long *offset, field_param *param);
void __stdcall function_237352(long *type, long *offset, field_param *param);
void __stdcall function_237379(long *type, long *offset, field_param *param);
void __stdcall function_237397(long *type, long *offset, field_param *param);

// Retail table of field descriptor callbacks, indexed by field id (0x470828).
// Entries whose functions are not decompiled yet are 0.
field_info_proc g_470828[0x70] =
{
	function_236ae6, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b0d, function_236b34, function_236b5b, function_236b7c, function_236b9d, function_236bbe, function_236be5,
	function_236c33, function_236c5a, function_236c81, function_236ca8, function_236ccf, function_236ceb, function_236d0c, function_236d2d, function_236d7b, function_236d54, function_236d9c, 0, function_23708e, function_237072, function_236e26, 0,
	0, 0, 0, 0, 0, 0, function_23708e, function_237072, function_2370ac, function_236e26, function_236e6e, function_2370ac, function_236ffd, function_237024, function_23708e, function_23704b,
	0, 0, function_236e47, function_237072, function_236e26, function_236e6e, function_236e8f, function_236eb6, function_236edd, function_236f04, function_236f25, function_236e47, function_237072, function_236e8f, function_236e6e, function_236f8e,
	function_236eb6, function_236edd, function_236f04, function_236f25, function_236e6e, function_236f04, function_236fdc, function_237072, function_236e26, function_23708e, function_2370d3, function_237121, function_237148, function_23716f, function_237196, function_2371bd,
	function_2371e4, function_23720b, function_237232, function_237259, function_237280, function_2372a1, function_2372c2, function_2372e3, function_23730a, function_23732b, function_236c0c, function_237379, function_236e26, function_237397, function_237352, function_236fb5,
	function_2370ac, function_236fdc, function_2370fa, function_2370ac, function_237072, function_236f04, function_236e26, function_236e6e, function_236dc3, function_236de4, function_236f4c, function_236f6d, function_236f4c, function_236f4c, function_236f6d, function_236e05,
};

// @retail 0x236ae6
void __stdcall function_236ae6(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x4c;
	param->r = 1.0f;
}

// @retail 0x236b0d
void __stdcall function_236b0d(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x50;
	param->r = 1.0f;
}

// @retail 0x236b34
void __stdcall function_236b34(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x54;
	param->r = 1.0f;
}

// @retail 0x236b5b
void __stdcall function_236b5b(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 3;
}

// @retail 0x236b7c
void __stdcall function_236b7c(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 4;
}

// @retail 0x236b9d
void __stdcall function_236b9d(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 5;
}

// @retail 0x236bbe
void __stdcall function_236bbe(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x58;
	param->r = 1.0f;
}

// @retail 0x236be5
void __stdcall function_236be5(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x74;
	param->r = 1.0f;
}

// @retail 0x236c0c
void __stdcall function_236c0c(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x78;
	param->r = 1.0f;
}

// @retail 0x236c33
void __stdcall function_236c33(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x7c;
	param->r = 1.0f;
}

// @retail 0x236c5a
void __stdcall function_236c5a(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x80;
	param->r = 1.0f;
}

// @retail 0x236c81
void __stdcall function_236c81(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x84;
	param->r = 1.0f;
}

// @retail 0x236ca8
void __stdcall function_236ca8(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x88;
	param->r = 1.0f;
}

// @retail 0x236ccf
void __stdcall function_236ccf(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 1;
}

// @retail 0x236ceb
void __stdcall function_236ceb(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 2;
}

// @retail 0x236d0c
void __stdcall function_236d0c(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 6;
}

// @retail 0x236d2d
void __stdcall function_236d2d(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xa4;
	param->r = 1.0f;
}

// @retail 0x236d54
void __stdcall function_236d54(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xa8;
	param->r = 1.0f;
}

// @retail 0x236d7b
void __stdcall function_236d7b(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 7;
}

// @retail 0x236d9c
void __stdcall function_236d9c(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xac;
	param->r = 1.0f;
}

// @retail 0x236dc3
void __stdcall function_236dc3(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 12;
}

// @retail 0x236de4
void __stdcall function_236de4(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 13;
}

// @retail 0x236e05
void __stdcall function_236e05(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 14;
}

// @retail 0x236e26
void __stdcall function_236e26(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 2;
}

// @retail 0x236e47
void __stdcall function_236e47(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x104;
	param->r = 1.0f;
}

// @retail 0x236e6e
void __stdcall function_236e6e(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 3;
}

// @retail 0x236e8f
void __stdcall function_236e8f(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xf4;
	param->r = 1.0f;
}

// @retail 0x236eb6
void __stdcall function_236eb6(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xf8;
	param->r = 1.0f;
}

// @retail 0x236edd
void __stdcall function_236edd(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xfc;
	param->r = 1.0f;
}

// @retail 0x236f04
void __stdcall function_236f04(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 4;
}

// @retail 0x236f25
void __stdcall function_236f25(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0x100;
	param->r = 1.0f;
}

// @retail 0x236f4c
void __stdcall function_236f4c(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 6;
}

// @retail 0x236f6d
void __stdcall function_236f6d(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 7;
}

// @retail 0x236f8e
void __stdcall function_236f8e(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0x108;
	param->r = 1.0f;
}

// @retail 0x236fb5
void __stdcall function_236fb5(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0x10a;
	param->r = 1.0f;
}

// @retail 0x236fdc
void __stdcall function_236fdc(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 5;
}
// @retail 0x236ffd
void __stdcall function_236ffd(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xf6;
	param->r = 1.0f;
}

// @retail 0x237024
void __stdcall function_237024(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xf8;
	param->r = 1.0f;
}

// @retail 0x23704b
void __stdcall function_23704b(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xfa;
	param->r = 1.0f;
}

// @retail 0x237072
void __stdcall function_237072(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 1;
}

// @retail 0x23708e
void __stdcall function_23708e(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0xf0;
	param->i = 0;
}

// @retail 0x2370ac
void __stdcall function_2370ac(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xf4;
	param->r = 1.0f;
}

// @retail 0x2370d3
void __stdcall function_2370d3(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xf0;
	param->r = 1.0f;
}

// @retail 0x2370fa
void __stdcall function_2370fa(long *type, long *offset, field_param *param)
{
	*type = 4;
	*offset = 0xf2;
	param->r = 1.0f;
}

// @retail 0x237121
void __stdcall function_237121(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xcc;
	param->r = 1.0f;
}

// @retail 0x237148
void __stdcall function_237148(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xcd;
	param->r = 1.0f;
}

// @retail 0x23716f
void __stdcall function_23716f(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xce;
	param->r = 1.0f;
}

// @retail 0x237196
void __stdcall function_237196(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xcf;
	param->r = 1.0f;
}

// @retail 0x2371bd
void __stdcall function_2371bd(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd0;
	param->r = 1.0f;
}

// @retail 0x2371e4
void __stdcall function_2371e4(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd1;
	param->r = 1.0f;
}

// @retail 0x23720b
void __stdcall function_23720b(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd2;
	param->r = 1.0f;
}

// @retail 0x237232
void __stdcall function_237232(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd3;
	param->r = 1.0f;
}

// @retail 0x237259
void __stdcall function_237259(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd4;
	param->r = 1.0f;
}

// @retail 0x237280
void __stdcall function_237280(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 8;
}
// @retail 0x2372a1
void __stdcall function_2372a1(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 9;
}

// @retail 0x2372c2
void __stdcall function_2372c2(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 10;
}

// @retail 0x2372e3
void __stdcall function_2372e3(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd5;
	param->r = 1.0f;
}

// @retail 0x23730a
void __stdcall function_23730a(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 11;
}

// @retail 0x23732b
void __stdcall function_23732b(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd6;
	param->r = 1.0f;
}

// @retail 0x237352
void __stdcall function_237352(long *type, long *offset, field_param *param)
{
	*type = 3;
	*offset = 0xd7;
	param->r = 1.0f;
}

// @retail 0x237379
void __stdcall function_237379(long *type, long *offset, field_param *param)
{
	*type = 1;
	*offset = 0x48;
	param->i = 0;
}

// @retail 0x237397
void __stdcall function_237397(long *type, long *offset, field_param *param)
{
	*type = 5;
	*offset = 0xb4;
	param->r = 1.0f;
}

// @retail 0x2373be
void function_2373be(long index, void *base, long field_store_amount)
{
	field_info_proc proc;
	long type;
	long offset;
	field_param param;

	proc = (index >= 0 && index < 0x70) ? g_470828[index] : 0;
	if (base && proc)
	{
		proc(&type, &offset, &param);

		switch (type)
		{
		case 0:
			if (field_store_amount)
				*(word *)((byte *)base + offset) |= (word)(1 << param.i);
			else
				*(word *)((byte *)base + offset) &= ~(word)(1 << param.i);
			break;
		case 1:
			if (field_store_amount)
				*(dword *)((byte *)base + offset) |= (1 << param.i);
			else
				*(dword *)((byte *)base + offset) &= ~(1 << param.i);
			break;
		case 2:
			*(byte *)((byte *)base + offset) = (field_store_amount != 0);
			break;
		case 3:
		{
			long r;
			real v = 1.0f / param.r;
			__asm
			{
				fld v
				fistp r
			}
			field_store_amount *= r;
			*(byte *)((byte *)base + offset) = (byte)field_store_amount;
			break;
		}
		case 4:
		{
			long r;
			real v = 1.0f / param.r;
			__asm
			{
				fld v
				fistp r
			}
			field_store_amount *= r;
			*(word *)((byte *)base + offset) = (word)field_store_amount;
			break;
		}
		case 5:
		{
			long r;
			real v = 1.0f / param.r;
			__asm
			{
				fld v
				fistp r
			}
			field_store_amount *= r;
			*(long *)((byte *)base + offset) = field_store_amount;
			break;
		}
		case 6:
			*(real *)((byte *)base + offset) = (1.0f / param.r) * (real)field_store_amount;
			break;
		}
	}
}
/* reads the field of a variant's settings that index names, scaled to the
   value the user interface shows */
// @retail 0x2374f0
long function_2374f0(void *base, long index)
{
	long result = 0;
	field_info_proc proc;

	proc = (index >= 0 && index < 0x70) ? g_470828[index] : 0;
	if (proc && base)
	{
		long type;
		long offset;
		field_param param;

		proc(&type, &offset, &param);
		switch (type)
		{
		case 0:
			result = (*(short *)((byte *)base + offset) & (1 << param.i)) != 0;
			break;
		case 1:
			result = (*(long *)((byte *)base + offset) & (1 << param.i)) != 0;
			break;
		case 2:
			result = (bool)*(byte *)((byte *)base + offset);
			break;
		case 3:
		{
			long r;
			real v = (real)*(byte *)((byte *)base + offset) * param.r;
			__asm
			{
				fld v
				fistp r
			}
			result = r;
			break;
		}
		case 4:
		{
			long r;
			real v = (real)*(short *)((byte *)base + offset) * param.r;
			__asm
			{
				fld v
				fistp r
			}
			result = r;
			break;
		}
		case 5:
		{
			long r;
			real v = (real)*(long *)((byte *)base + offset) * param.r;
			__asm
			{
				fld v
				fistp r
			}
			result = r;
			break;
		}
		case 6:
		{
			long r;
			real v = *(real *)((byte *)base + offset) * param.r;
			__asm
			{
				fld v
				fistp r
			}
			result = r;
			break;
		}
		}
	}
	return result;
}
