/* UNKNOWN_080E10.H: function_12d2f0 retried until it succeeds or a time
   limit passes, as 0x80e10, 0x862e0 and 0x8c790 expand it inline (lane D) */

#ifndef UNKNOWN_080E10_H
#define UNKNOWN_080E10_H

#include "physical_memory.h"
#include <xtl.h>
#include <d3d8.h>

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);

/* the time stamp counter */
// Lane 17 r29 (11-02), a lead: with local_0 non-volatile and made address-taken by a dead `if (0) __asm lea eax, local_0`
// before the rdtsc, 0x199740 (3), 0x12d400 (2) and 0x6a8a0 (2) match and 0x80e10 goes 14 -> 13, but 0x18f260 4 -> 12,
// 0x146e30 120 -> 124 and 0x862e0 22 -> 24 (their frames grow: start's spilled high half and the pump counter no longer
// share its slot); no match lost in the 183 functions of the 9 includers. Not kept: the dead asm only steers the compiler.
// A live `__asm mov eax, dword ptr local_0` before the rdtsc keeps retail's frames at 0x199740/0x18f260/0x862e0 but emits a
// mov retail lacks; plain/array/struct/LARGE_INTEGER/memset forms lose the stores. Wanted: a live form that takes the address.
static __forceinline __int64 function_80e13(void)
{
	volatile __int64 local_0 = 0;
	__asm rdtsc
}

/* at most maximum_pumps calls of function_12c600 between the attempts, then
   until timeout seconds have passed since the first */
static __forceinline long function_80e14(long size, long user_data, long update, long release,
	long maximum_pumps, real timeout)
{
	__int64 start = function_80e13();
	long result = 0;
	if (size > 0)
	{
		long page_count = g_4e6464->page_count;
		if (page_count > 0)
		{
			long pumps = 0;
			do
			{
				result = function_12d2f0(size, user_data, update, release);
				if (result == 0)
				{
					if (pumps < maximum_pumps)
					{
						pumps++;
						function_12c600();
					}
					else
					{
						__int64 elapsed = function_80e13() - start;
						if (elapsed < 0)
							elapsed = 0;
						if (!((real)timing_ticks_to_seconds(elapsed) < timeout))
							break;
						D3DDevice_KickPushBuffer();
						D3DDevice_IsBusy();
						SwitchToThread();
					}
				}
			} while (result == 0);
		}
	}
	return result;
}

#endif
