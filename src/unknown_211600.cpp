// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"

void function_295210(long arg_0);
void function_28fc50(long arg_0, word arg_1);
void function_28fd00(long object_index);
void function_2952e0(long object_index);
void __stdcall function_28fe50(long object_index);

// @retail 0x211600
void function_211600(long arg_0, long arg_1)
{
    long const *local_0 = &arg_1;
    byte *local_1 = *(byte **)(g_4e0300->data + (arg_0 & 0xffff) * 12 + 8);
    switch (*(long *)(local_1 + 0x134))
    {
    case 0:
        function_28fc50(arg_0, (word)*local_0);
        break;
    case 1:
        function_295210(arg_0);
        break;
    }
}

// @retail 0x211640
void function_211640(long object_index)
{
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    switch (*(long *)(object + 0x134))
    {
    case 0:
        function_28fd00(object_index);
        break;
    case 1:
        function_2952e0(object_index);
        break;
    }
}

// Retail keeps unknown1 and unknown2 on the stack (ret 8). LTCG currently
// removes them; taking their addresses did not retain the retail convention.
// @retail 0x2116c0
void function_2116c0(long object_index, long unknown1, long unknown2)
{
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    if (!*(long *)(object + 0x134))
        function_28fe50(object_index);
}
