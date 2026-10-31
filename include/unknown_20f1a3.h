#ifndef __UNKNOWN_20F1A3_H__
#define __UNKNOWN_20F1A3_H__

#include "unknown_11c920.h"

/* the game time globals' tick (+8) plus a delay in seconds converted to
   ticks at their tick rate (+2), rounded: the tick a delay ends on
   (inlined in 0x20f1a0 and 0x11bb40) */
PRIVATE __forceinline long function_20f1a3(byte const *arg_0, real arg_1)
{
    long local_0 = *(long const *)(arg_0 + 8);
    real local_1 = *(short const *)(arg_0 + 2) * arg_1;
    long local_2;
    __asm
    {
        fld local_1
        fistp local_2
    }
    return local_0 + local_2;
}

#endif
