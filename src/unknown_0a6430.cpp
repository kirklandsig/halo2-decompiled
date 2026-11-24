#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_tag_definitions.h"

// @flags /O2 /arch:SSE /Gr

long __stdcall function_a9f70(long object_index, long mode, point3f const *position);
void function_aa260(long selection, long object_index, vector3f const *linear, long mode,
	point3f const *position, vector3f const *forward, vector3f const *up, vector3f const *angular);
bool function_a76b0(long index, long which);
void function_dc0a0(long index);
void function_b7680(long index, real scale, real seconds);
void function_bf830(long index, real shield, short value);
void function_bf890(long index, byte const *states);
void __stdcall function_dbc80(long index, short first, short second);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);

PRIVATE byte *update_object_0a6430(long index)
{
	return *(byte **)(g_4e0300->data + (index & 0xffff) * 12 + 8);
}

// Applies a state built by 0xa5e70. Matched. The 0x100 block gets the region count from 0xba690 (LTCG inlines it) and
// compares it with the state byte bound as a `long const &` (compared directly, the 0x100 registers come out one off).
// The 0x200 block has 0xa5e70's 0x200 shape: `count = 0` comes after the definition read, and the model tag index is
// read through definition->model_tag_index at both uses. With the count set to 0 before the object lookup, the pool
// loads before the index into ECX (+1 byte).
// @retail 0xa6430
void function_a6430(c_object_type_definition const *definition, long mask, long object_index, long state_address)
{
	byte *state = (byte *)state_address;
	if ((mask & 1) && state[0x4d])
		function_dc0a0(object_index);
	if ((mask & 2) || (mask & 0x34))
	{
		byte *header = g_4e0300->data + (object_index & 0xffff) * 12;
		byte *object = *(byte **)(header + 8);
		if (*(long *)(object + 0x14) == NONE)
		{
			point3f const *position = NULL;
			vector3f const *forward = NULL;
			vector3f const *up = NULL;
			vector3f const *linear = NULL;
			vector3f const *angular = NULL;
			if (mask & 2)
			{
				position = (point3f const *)state;
				if ((1 << header[3]) & 0x1c)
					object[0x12c] &= 0xbf;
			}
			if (mask & 4)
			{
				forward = (vector3f const *)(state + 0xc);
				up = (vector3f const *)(state + 0x18);
			}
			if (mask & 0x10) linear = (vector3f const *)(state + 0x28);
			if (mask & 0x20) angular = (vector3f const *)(state + 0x34);
			long mode = function_a76b0(object_index, 0) ? 1 : 0;
			long selection = function_a9f70(object_index, mode, position);
			function_aa260(selection, object_index, linear, mode, position, forward, up, angular);
		}
	}
	if (mask & 8)
		function_b7680(object_index, *(real *)(state + 0x24), 0.0f);
	if (mask & 0x40)
	{
		short value = state[0x44] == 0;
		byte *object = update_object_0a6430(object_index);
		*(long *)(object + 0xec) = *(long *)(state + 0x40);
		*(short *)(object + 0x106) = value;
	}
	if (mask & 0x80)
		function_bf830(object_index, *(real *)(state + 0x48), state[0x4c] == 0);
	if (mask & 0x100)
	{
		long region_count;
		function_ba690(object_index, &region_count, NULL, NULL, NULL);
		long const &expected = state[0x50];
		if (region_count == expected)
			function_bf890(object_index, state + 0x51);
	}
	if (mask & 0x200)
	{
		byte *object = update_object_0a6430(object_index);
		s_object_definition const *definition = (s_object_definition const *)g_4e3b44[*(long *)object & 0xffff].bytes;
		long count = 0;
		if (definition->model_tag_index != NONE)
		{
			byte *local_0f244f = g_4e3b44[definition->model_tag_index & 0xffff].bytes;
			if (*(long *)(local_0f244f + 0x60) > 0)
				count = *(long *)(*(byte **)(local_0f244f + 0x64) + 0xe0);
		}
		if (state[0x61] == count)
		{
			word first = *(word *)(state + 0x62);
			if (first || *(word *)(state + 0x64))
			{
				word second = *(word *)(state + 0x64);
				function_dbc80(object_index, first, second);
				function_b9b90(object_index, false);
			}
		}
	}
}
