// @flags /O2 /Gr
#include <string.h>

// @retail 0x52020
bool __stdcall function_52020(void *destination, long unknown, void const *source)
{
    memcpy(destination, source, 80);
    return true;
}
