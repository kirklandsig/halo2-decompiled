// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "loop_allocator.h"
#include "network_voice.h"
#include "unknown_12b400.h"
#include <string.h>

/* the memory source the voice pool is built from (0x476fbc, vtable 0x4508fc):
   whole pages from the top of the physical memory map; releasing does
   nothing */
class c_physical_memory_source : public c_memory_source
{
public:
	virtual void *allocate(long size);
	virtual void release(void *block) {}

	long unknown04;
};

c_physical_memory_source g_476fbc;

// @retail 0x531b0
void *c_physical_memory_source::allocate(long size)
{
	return physical_memory_malloc_fixed(size, PAGE_READWRITE);
}

void voice_xhv_reset_masks(c_voice_xhv *xhv);
void voice_xhv_reset_port_modes(c_voice_xhv *xhv);
bool voice_xhv_create(c_voice_xhv *xhv);
void voice_xhv_dispose(c_voice_xhv *xhv);

// @retail 0x53310
void __stdcall function_53310(long stage)
{
	long type;
	long size = 0;
	long mode = 0;
	c_memory_source *source = &g_476fbc;
	s_loop_allocator *loop;

	if (stage > 1)
	{
		type = (stage <= 3);
	}
	else
	{
		type = 2;
	}

	switch (type)
	{
	case 1:
		mode = 1;
		break;
	case 2:
		size = 0x78000;
		mode = 2;
		break;
	}

	g_4c9878.type = type;
	g_4c9878.pool_mode = mode;

	if (type == 2)
	{
		loop = (s_loop_allocator *)source->allocate(size + 0x50);
		if (loop)
		{
			function_18e250(loop, size, "voice pool", source);
		}
		g_4c9878.pool = loop;
		g_4c9878.pool->field3c = 1;
		g_4c9878.pool->field3d = 1;
		g_4c9878.pool->field3e = 1;

		if (!g_476fc8.initialized)
		{
			g_476fc8.engine = 0;
			g_476fc8.mode = mode;
			voice_xhv_reset_masks(&g_476fc8);
			voice_xhv_reset_port_modes(&g_476fc8);
			g_476fc8.initialized = voice_xhv_create(&g_476fc8);
		}
	}
}

PRIVATE __forceinline void function_533e1(s_loop_allocator *arg_0)
{
 c_memory_source *local_0 = arg_0->source;
 memset(arg_0, 0, sizeof(*arg_0));
 local_0->release(arg_0);
}

// @retail 0x533e0
void function_533e0(void)
{
	if (g_4c9878.pool)
	{
		voice_xhv_dispose(&g_476fc8);
		function_533e1(g_4c9878.pool);
		g_4c9878.pool = 0;
	}
}