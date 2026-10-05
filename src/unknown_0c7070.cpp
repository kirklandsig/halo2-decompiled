// @flags /O2 /Gr
/* UNKNOWN_0C7070.CPP: object queries */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object_c7070
{
	byte unknown00[0x12a];
	short index;
	byte unknown12c[0x7c - 0x12c + 0x12c];
};

struct s_object_c7070_header
{
	byte unknown00[8];
	byte *object;
};

// retail's table at 0x467430
extern const long g_467430[5] = { 0x5000049, 0x7000679, 0x700067a, 0x700067b, 0x700067c };

// @retail 0xc7070
bool function_0c7070(long object_index)
{
	byte *object = *(byte **)((byte *)g_4e0300->data + 8 + (object_index & 0xffff) * 12);
	long value = *(long *)(object + *(short *)(object + 0x12a) + 0x7c);
	bool found = false;
	for (long i = 0; !found && i < 5; i++)
	{
		if (g_467430[i] == value)
		{
			found = true;
		}
	}
	return found;
}
