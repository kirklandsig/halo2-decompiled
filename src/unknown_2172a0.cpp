// @flags /O2 /Gr
/* UNKNOWN_2172A0.CPP: removes an entry from the table of unknown_2157e0.cpp
   (g_55c164, g_55c160 entries of 0x44 bytes, keyed by the handle at +0) by
   moving the last entry into its place (lane H) */

#include "unknown_11c920.h"
#include "saved_game_storage.h"
#include <string.h>

// @retail 0x2172a0
void function_2172a0(long handle)
{
	long count = g_55c160_table.count;
	long i;

	for (i = 0; i < count; i++)
	{
		long entry = (long)g_55c160_table.entries[i].file_index;

		if (entry != NONE && entry == handle)
		{
			long last = count - 1;

			if (i != last)
				g_55c160_table.entries[i] = g_55c160_table.entries[last];
			g_55c160_table.count--;
			return;
		}
	}
}
