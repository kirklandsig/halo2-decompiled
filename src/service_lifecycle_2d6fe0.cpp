// @flags /O2 /GL-
// Havok service replacement: release the previous singleton before installing
// its replacement. The allocator uses a long reference count; the other
// services use the sized-reference header and the game's release callback.
#include "unknown_146a20.h"
#include "sized_reference_a9ef0.h"

class error_handler_2d97d0;
class reporter_2ff210;
extern error_handler_2d97d0 *g_480178;
extern reporter_2ff210 *g_48017c;
c_sized_reference_a9ef0 *g_480180;
bool __cdecl function_279740(long value);

static void release_service(c_sized_reference_a9ef0 *service)
{
	--service->reference_count;
	if (service->reference_count == 0)
	{
		if (function_279740((long)service))
			service->reference_count = 1;
		else
			delete service;
	}
}

// Score 22: retail keeps a separate counter address and a test before its
// store. The compiler folds those operations into the object's field access.
// Prefix decrement scores 26; explicit-count and pointer-helper forms 22.
// @retail 0x2d6fe0
void __cdecl function_2d6fe0(c_havok_abstract_allocator *replacement)
{
	c_havok_abstract_allocator *old = (c_havok_abstract_allocator *)g_480118;
	if (old)
	{
		long remaining = old->m_unknown04 - 1;
		old->m_unknown04 = remaining;
		if (remaining == 0)
			delete old;
	}
	g_480118 = (c_allocator *)replacement;
}

// @retail 0x2d73f0
void __cdecl function_2d73f0(error_handler_2d97d0 *replacement)
{
	if (g_480178)
		release_service((c_sized_reference_a9ef0 *)g_480178);
	g_480178 = replacement;
}

// @retail 0x2d7460
void __cdecl function_2d7460(reporter_2ff210 *replacement)
{
	if (g_48017c)
		release_service((c_sized_reference_a9ef0 *)g_48017c);
	g_48017c = replacement;
}

// @retail 0x2d74d0
void __cdecl function_2d74d0(c_sized_reference_a9ef0 *replacement)
{
	if (g_480180)
		release_service(g_480180);
	g_480180 = replacement;
}
