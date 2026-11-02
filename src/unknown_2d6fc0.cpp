// @flags /O2 /GL-
#include "unknown_146a20.h"

/* Havok's memory instance; 0x2d6fe0 installs it. Defined in this /GL- library file: code built with
   link-time generation may read it ahead of pointer stores, as 0x1d5ec0 does. */
class c_allocator;
c_allocator *g_480118;

// @retail 0x2d6fc0
void c_havok_abstract_allocator::function_2d6fc0(void *buffer, long size)
{
	m_unknown08[0] = (long)buffer;
	m_unknown08[1] = size;
}
