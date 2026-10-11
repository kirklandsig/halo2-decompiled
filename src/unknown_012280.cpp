// @flags /O2 /Gr
/* UNKNOWN_012280.CPP: the title's XMemAlloc and XMemFree, which send the
   voice allocators' requests to the voice pools (decompiled by lane D for
   the voice chat, outside its region) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>

extern char g_509344[256];

// @retail 0x12200
bool function_12200(void)
{
    DWORD type;
    LAUNCH_DATA launch_payload_blob;
    if (XGetLaunchInfo(&type, &launch_payload_blob) == ERROR_SUCCESS && type == LDT_TITLE &&
        strcmp((char const *)&launch_payload_blob, "XDEMOS") == 0)
    {
        char const *cursor = g_509344;
        unsigned long length = 0;
        while (length < 8 && *cursor++)
            ++length;
        strncpy(g_509344 + length, "xdemo ", 8 - length);
        g_509344[7] = 0;
    }
    return true;
}

/* src/network_voice.cpp */
void *voice_allocate(long size, long attributes);
void voice_free(void *pointer, long attributes);

/* not decompiled yet */
LPVOID WINAPI function_12c090(SIZE_T size, DWORD attributes);

/* the last block of the 0x82 allocator */
void *g_510c3c;
bool g_510c40;

// @retail 0x12280
LPVOID __stdcall XMemAlloc(SIZE_T dwSize, DWORD dwAllocAttributes)
{
	switch (((XALLOC_ATTRIBUTES *)&dwAllocAttributes)->dwAllocatorId)
	{
	case 0x82:
		return function_12c090(dwSize, dwAllocAttributes);
	case 0x89:
	case 0x8a:
		return voice_allocate(dwSize, dwAllocAttributes);
	default:
		return XMemAllocDefault(dwSize, dwAllocAttributes);
	}
}

// @retail 0x122c0
void __stdcall XMemFree(PVOID pAddress, DWORD dwAllocAttributes)
{
	switch (((XALLOC_ATTRIBUTES *)&dwAllocAttributes)->dwAllocatorId)
	{
	case 0x82:
	{
		PVOID address = *(PVOID volatile *)&pAddress;
		if (g_510c3c == address)
		{
			g_510c40 = false;
			return;
		}
		XMemFreeDefault(address, dwAllocAttributes);
		return;
	}
	case 0x89:
	case 0x8a:
		voice_free(pAddress, dwAllocAttributes);
		return;
	}
	XMemFreeDefault(pAddress, dwAllocAttributes);
}
