// @flags /O2 /Gr
#include "unknown_11c920.h"

/* 0x98-byte entries starting at 0x4b4b58 */
struct s_unknown_01dcc0
{
	byte unknown00[4];
	long sub_header[5];                         /* +0x04 */
	long elements[4][6];                        /* +0x18, 24-byte elements */
	long element_count;                         /* +0x78 */
	byte unknown7c[4];
	long width;                                 /* +0x80 */
	long height;                                /* +0x84 */
	void *data;                                 /* +0x88 */
	byte unknown8c[8];
	bool flag94;
	byte flag95;
	byte unknown96[2];
};

s_unknown_01dcc0 g_4b4b58[32];

// @retail 0x25960
long function_25960(void)
{
	long result = NONE;
	if (g_4b4b58[20].data && !g_4b4b58[20].flag95)
		return 20;
	if (g_4b4b58[18].data && !g_4b4b58[18].flag95)
		result = 18;
	return result;
}

// @retail 0x1dcc0
void *function_01dcc0(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95)
	{
		result = &e->sub_header;
	}
	return result;
}

// @retail 0x1dcf0
void *function_01dcf0(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95 && e->element_count > 0)
	{
		result = &e->elements;
	}
	return result;
}

// @retail 0x1dd20
void *function_01dd20(long index, long element)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95 && element >= 0 && element < e->element_count)
	{
		result = &e->elements[element];
	}
	return result;
}

// @retail 0x1dd60
bool function_01dd60(long index, long *width, long *height)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	if (e->data != 0 && !e->flag95)
	{
		*width = e->width;
		*height = e->height;
		return true;
	}
	if (index == 0 || index == 3 || index == 0x18)
	{
		*width = 0x280;
		*height = 0x1e0;
		return true;
	}
	*width = 0;
	*height = 0;
	return false;
}

// @retail 0x1ddd0
void function_01ddd0(long index, dword width, dword height)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	dword x = ((width * 4) / 64) << 24;
	dword y = height << 12;
	dword value = (x - 0x1000000) | (y - 0x1000) | (width - 1);
	e->width = width;
	e->height = height;
	value = e->flag94 ? value : 0;
	e->sub_header[4] = value;
	e->elements[0][4] = value;
}

// @retail 0x1de20
bool function_01de20(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	bool result = false;
	if (e->data != 0 && !e->flag95)
	{
		result = true;
	}
	return result;
}
