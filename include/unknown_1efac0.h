/* UNKNOWN_1EFAC0.H: reference objects with registry-backed name virtuals. */

#ifndef UNKNOWN_1EFAC0_H
#define UNKNOWN_1EFAC0_H

#include "unknown_11c920.h"
#include "globals.h"

class eight_values_2d96c0;
extern eight_values_2d96c0 g_480120;

struct c_b
{
	virtual ~c_b() {}
	word flags;
	word unknown06;

	static __forceinline void __cdecl operator delete(void *block)
	{
		/* The first word records the allocation size; allocator slot 5 frees it. */
		g_480118->allocate((long)block, ((c_b *)block)->flags, 0x10);
	}
};

struct c_a : c_b
{
	c_a() {}
	c_a(unsigned short references) { unknown06 = references; }

	virtual ~c_a()
	{
		if (flags & 0x8000)
		{
			flags &= 0x7fff;
			v2(0);
		}
	}

	virtual unsigned int v1(char *buffer, unsigned int size);
	virtual void v2(char const *name);
	virtual void *v3() { return &g_480120; }
	virtual void v4(dword unused) {}
};

/* the object's tag chain (0x1efb40) */
struct s_tag_ref_data;
struct s_lookup
{
	long handle;
	s_tag_ref_data *tag_a;
	s_tag_ref_data *tag_b;
	void *pointer_a;
	void *pointer_b;

	bool initialize(long handle);
};

#endif
