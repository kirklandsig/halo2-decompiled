// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0E5930.CPP: the biped's return to its default physics mode and the
   wake-up that uses it; retail calls both out of line at every site, so they
   build in their own /Ob1 file. 0xe5930 is BirchWoodGod's decompilation, moved
   here from unknown_0e57e0.cpp with its body unchanged. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_e57e0_object_header
{
	byte unknown00[8];
	byte *biped;
};

#define E57E0_BIPED_GET(index) (((s_e57e0_object_header *)g_4e0300->data)[(index) & 0xffff].biped)

void function_1e54d0(void *state, long mode);
long function_e5980(long object_index);
void function_e5750(long object_index);
void function_e5790(long object_index);

/* returns the biped to its default physics mode */
// @retail 0xe5930
void function_e5930(long object_index)
{
	long mode = function_e5980(object_index);

	switch (mode)
	{
	case 1:
		function_1e54d0(E57E0_BIPED_GET(object_index) + 0x3dc, 1);
		break;
	case 2:
		function_e5750(object_index);
		break;
	default:
		function_e5790(object_index);
		break;
	}
}

void __stdcall function_e18d0(long arg_159e6d, bool flag);
void __stdcall function_b8890(long unit_index);

// @retail 0xe4bd0
void function_e4bd0(long arg_159e6d)
{
	byte *biped = E57E0_BIPED_GET(arg_159e6d);

	function_e18d0(arg_159e6d, true);
	function_e5930(arg_159e6d);
	if (!TEST_FIELD_BIT((*(word *)(biped + 0xc0) >> 6) & 1))
	{
		function_b8890(arg_159e6d);
	}
}
