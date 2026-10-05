// @flags /O2 /Gr
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* The callbacks of the table at 0x4674d8: a data array of 0x1258-byte
   elements (g_4e0338) and its accessors. */

struct s_cloth_tag_data
{
	byte unknown00[0xc];
	long value;
};

struct s_cloth_element
{
	byte unknown00[4];
	long tag_index;
};

// @retail 0x116a10
void __stdcall function_116a10()
{
	g_4e0338 = data_new_inlined("cloth", 8, 0x1258, 0, g_510c2c);
}

// @retail 0x116a50
void __stdcall function_116a50()
{
	g_4e0338->valid = 1;
	record_pool_release_all(g_4e0338);
}

// @retail 0x116a70
void __stdcall function_116a70()
{
	g_4e0338->valid = 0;
}

// @retail 0x116a80
void __stdcall function_116a80()
{
	if (g_4e0338)
	{
		data_dispose(g_4e0338);
		g_4e0338 = 0;
	}
}

// @retail 0x1169f0
void __stdcall function_1169f0(long index)
{
	record_pool_release(g_4e0338, index);
}

// @retail 0x116ac0
long __stdcall function_116ac0(long index)
{
	s_cloth_element *element = (s_cloth_element *)(g_4e0338->data + (index & 0xffff) * 0x1258);

	return ((s_cloth_tag_data *)g_4e3b44[element->tag_index & 0xffff].bytes)->value;
}

void __stdcall function_1168a0(real elapsed);
long __stdcall function_116980(long tag_index, long object_index);
void __stdcall function_116b00(long, long, long, long, long, long, void *);
void __stdcall function_117060(void *submission);

/* the callbacks of the table at 0x4674d8 that are decompiled */
void *g_4674d8[12] =
{
	(void *)function_116a10, (void *)function_116a50, (void *)function_116a70, (void *)function_116a80, (void *)function_116980,
	(void *)function_1169f0, (void *)function_1168a0, 0, (void *)function_116b00, (void *)function_117060,
	(void *)function_116ac0, 0
};
