#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"

// @flags /O2 /arch:SSE /Gr

long __stdcall function_a9f70(long object_index, long mode, point3f const *position);
void function_aa260(long selection, long object_index, vector3f const *linear, long mode,
	point3f const *position, vector3f const *forward, vector3f const *up, vector3f const *angular);
bool function_a76b0(long index, long which);
void function_dc0a0(long index);
void function_b7680(long index, real scale, real seconds);
void function_bf830(long index, real shield, short value);
void function_bf890(long index, byte const *states);
void function_dbc80(long index, short first, short second);

PRIVATE byte *update_object_0a6430(long index)
{
	return *(byte **)(g_4e0300->data + (index & 0xffff) * 12 + 8);
}

// score 16 (was 16): the 0x100 block's object pointer takes ECX (retail EAX, reusing the pool
// base) and the pushed state pointer EDX (retail ECX), one scratch register on in rotation; the
// 0x200 block loads the object pool before the index (retail after it, +1 byte). Tried: the
// 0x100 compare reversed 17; the object read without a local 16; the tag lookups as tag-array
// elements 16; the definition read straight from the object getter 16; the count cleared after
// the object read 16.
// Round 53: force-inline object getter 16; getter object return local 16; getter byte-offset
// local 16; final declarations before assignments 16; count declared at function scope 16;
// count initialized at function scope 184; final count comparison reversed 17; second word
// read directly at the call 16. Opening body restored.
// Round 28: getter as an s_object_header array element 16; a named short level in the 0x100
// block 16; getter plain inline 16; 0x200 compare count-first 17; the shadowing definition
// locals renamed 16.
// Round 48: the 0x200 block's object read before the count 16; the count set in else arms 53 (552
// bytes); the 0x100 compare against a long state byte 16, the states passed as &state[0x51] 16, the
// level cast back to short 20 (550 bytes).
// Round 33 (lane 17): the getter written index-first 16; the 0x200 block's tag index read into a local 16;
// the 0x100 block through a header pointer (the 0x2 block's form) 16; the 0x200 object ahead of the count
// with the 0x100 compare state-first 17; the 0x40 flag written in place at the store 36 (548 bytes).
// Round 134: typed final/0x100/both object views 16/16/16; typed 0x2 header/object 16; count after
// definition 16; final/0x100 masked index 16/16; const state pointer 16. Opening body restored.
// Round 163: a named level, a named state byte, a named `state + 0x51`, the object read in place, a named
// offset in the getter, an inline level getter, an inline definition lookup, the tag index read into a local (two
// forms) 16 each; the 0x80 flag as a named bool 32 (550 bytes); the count zeroed after the definition read 20
// (550 bytes, the tag pool load joins the head). Opening body restored.
// Round 75 (lane 1, fast): the 0x40 block as a setter helper inline or forced 16, with a bool flag 43 (550 bytes); a level
// test helper 16; a count helper forced 16, plain inline 126 (471 bytes, not inlined); both forced 16; an unused states
// read beside the level 16; the level named and divided in place 16; a forced region helper with count out or states out
// 16/16; the compared byte read first 16; the object advanced to the field 16; the model info pointer with a NULL default
// in place 45 (553 bytes), through a forced helper with the count first or after 45/48, as a ternary 59, plain inline
// 113. Diagnostic: a dead load at the 0x100 head gives that block retail's 0x200 shape (index first, EDX object); no
// count of dead loads gives retail's own 0x100 shape, where only the object differs (ours ECX, retail EAX).
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
		byte *object = update_object_0a6430(object_index);
		if (*(short *)(object + 0x118) / 10 == state[0x50])
			function_bf890(object_index, state + 0x51);
	}
	if (mask & 0x200)
	{
		long count = 0;
		byte *object = update_object_0a6430(object_index);
		byte *definition = *(byte **)((byte *)g_4e3b44 + (*(long *)object & 0xffff) * 16 + 8);
		long model = *(long *)(definition + 0x38);
		if (model != NONE)
		{
			byte *local_0f244f = *(byte **)((byte *)g_4e3b44 + (model & 0xffff) * 16 + 8);
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
