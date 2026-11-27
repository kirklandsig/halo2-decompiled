/* the floor of a real as a long: retail has this body out of line at 0x16ae0 and inlined in 0x18a90 and 0x2cbf0 */

#ifndef FLOOR_TO_LONG_H
#define FLOOR_TO_LONG_H

#include "unknown_11c920.h"
#include "globals.h"

__forceinline long floor_to_long_inline(real value)
{
	__asm {
		movss xmm0, value
		cvttss2si eax, xmm0
		cvtsi2ss xmm1, eax
		cmpneqss xmm1, xmm0
		cmpltss xmm0, g_45dbd8
		andps xmm0, xmm1
		movmskps ecx, xmm0
		sub eax, ecx
	}
}

#endif
