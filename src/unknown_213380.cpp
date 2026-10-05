// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_213380.CPP: the end of a content signature calculation. Decompiled
   by lane L: the preferences file (unknown_120d80.cpp) begins the
   calculation inline and calls this with a register argument. */

#include "unknown_11c920.h"
#include <xtl.h>

/* the signature calculation in progress */
HANDLE g_470024 = INVALID_HANDLE_VALUE;

// @retail 0x213380
bool signature_calculate_end(XCALCSIG_SIGNATURE *signature)
{
	DWORD error = XCalculateSignatureEnd(g_470024, signature);

	g_470024 = INVALID_HANDLE_VALUE;
	return error == ERROR_SUCCESS;
}
