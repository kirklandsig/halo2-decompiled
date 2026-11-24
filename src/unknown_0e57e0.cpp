// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E57E0.CPP: helpers for a biped's physics mode

0xe57e0 meets function_e4050's two conditions (bit 0 of +0x348 and the
count at +0x399) at once; 0xe5840 moves a biped in an animation-driven mode
(4 or 5) by an offset, or stores the offset; 0xe5930 returns a biped to the
mode function_e5980 gives. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_e57e0_object_header
{
	byte unknown00[8];
	byte *biped;
};

#define E57E0_BIPED_GET(index) (((s_e57e0_object_header *)g_4e0300->data)[(index) & 0xffff].biped)

void __stdcall function_b9b90(long object_index, bool disable);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	struct s_location const *location, bool unknown);
void function_1e54d0(void *state, long mode);
long function_e5980(long object_index);
void function_e5750(long object_index);
void function_e5790(long object_index);

/* a float rounded to an integer by the x87 (fld, fistp) */
__forceinline long e57e0_round(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* sets bit 0 of +0x348 and the count at +0x399 to 0.18 seconds of ticks */
// @retail 0xe57e0
void __stdcall function_e57e0(long object_index)
{
	byte *biped = E57E0_BIPED_GET(object_index);

	*(word *)(biped + 0x348) |= 1;
	biped[0x399] = (byte)e57e0_round(g_510c54->field_2_3 * 0.18f);
	function_b9b90(object_index, false);
}

/* in an animation-driven mode (4 or 5), moves the biped by the offset, or
   keeps the offset at +0x3f8 for later */
// Reading the x position through a raw cast, rather than through the position pointer, gives
// retail's operand order for the x sum (offset first); the y and z sums keep the position first.
// @retail 0xe5840
void function_e5840(long object_index, vector3f const *offset, bool move)
{
	byte *biped = E57E0_BIPED_GET(object_index);
	long mode = biped[0x3dc];

	if (mode == 4 || mode == 5)
	{
		if (move)
		{
			point3f const *position = (point3f const *)(biped + 0x64);
			point3f point;

			point.x = *(real const *)(biped + 0x64) + offset->i;
			point.y = position->y + offset->j;
			point.z = position->z + offset->k;
			function_b75a0(object_index, &point, NULL, NULL, NULL, false);
		}
		else
		{
			biped[0x415] = 1;
			*(vector3f *)(biped + 0x3f8) = *offset;
		}
	}
}
