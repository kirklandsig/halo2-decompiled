// stubs for game functions not decompiled yet, called by unknown_01e930.cpp
#include "unknown_11c920.h"

// @stub 0x35b90
void __stdcall function_35b90(void *material) { }
// @stub 0x363a0
void __stdcall function_363a0(void *vertices) { }
// @stub 0x1e930
void __stdcall function_1e930(long a) { }
/* builds a texture header in the given one (retail passes it in edi) and
   returns it, or 0 when the allocation fails */
// @stub 0x23e340
struct D3DTexture *function_23e340(short width, short height, short format,
	void *(__stdcall *allocate)(long size, long alignment), long *size, void **data, struct D3DTexture *header) { return 0; }
