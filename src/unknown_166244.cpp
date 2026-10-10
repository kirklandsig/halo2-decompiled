// @flags /O1 /Gr
/* UNKNOWN_166244.CPP: lookups in the per player groups at g_4e9bc8 (the file
   of unknown_16658d.cpp). Decompiled by lane R for the effects. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;

long function_16658d(long group_index, long key);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

struct s_166244_entry
{
	long key;
	byte unknown04[0x304 - 4];
	transform4x3f nodes[1];
	byte unknown338[0x1010 - 0x338];
};

struct s_166244_group
{
	byte unknown00[0x10];
	s_166244_entry entries[2];
	byte unknown2030[0x30];
	transform4x3f matrix;
	byte unknown2094[0x20cc - 0x2094];
};

#define GROUPS ((s_166244_group *)g_4e9bc8)

// @retail 0x166244
long function_166244(long key)
{
	long matching_group_index = NONE;

	for (long i = 0; i < 4; i++)
	{
		for (long j = 0; j < 2; j++)
		{
			if (key == GROUPS[i].entries[j].key)
			{
				matching_group_index = i;
				break;
			}
		}
	}
	return matching_group_index;
}

// @retail 0x166283
bool function_166283(long group_index, long key)
{
	bool result = false;

	if (group_index != NONE && function_16658d(group_index, key) != NONE)
		result = true;
	return result;
}

// @retail 0x1664a5
transform4x3f *function_1664a5(long group_index, long key, short node_index)
{
	long entry_index = function_16658d(group_index, key);

	return &GROUPS[group_index].entries[entry_index].nodes[node_index];
}

// @retail 0x1664da
void function_1664da(long group_index, long key, short node_index, transform4x3f *out)
{
	s_166244_group *group = &GROUPS[group_index];
	long entry_index = function_16658d(group_index, key);

	function_142a60(&group->matrix, &group->entries[entry_index].nodes[node_index], out);
}
