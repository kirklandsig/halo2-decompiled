#include "unknown_11c920.h"
#include "effects.h"
#include "havok_reference.h"
#include <new>

// @flags /O2 /Gr

struct s_particle_impact;
struct s_object_246eb0;
void function_246e60(s_particle_impact const *impact, s_object_246eb0 const *particle,
	long definition_index, dword group, long tag_index);

class c_view_248340
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual dword v2() = 0;
	virtual long v3() = 0;
};

struct s_emitter_248340
{
	short field_00;
	short field_02;
};

// @retail 0x248340
void function_248340(s_particle_system_datum *system, s_object_246eb0 *particle,
	s_emitter_248340 *emitter)
{
	c_view_248340 *definition = (c_view_248340 *)function_137bd0(system->function_1751d0()->tag_index);
	if (definition->v3() != NONE)
	{
		long definition_index = system->function_1751d0()->tag_index;
		function_246e60(0, particle, definition_index, definition->v2(), definition->v3());
	}
	*(long *)((byte *)particle + 4) = NONE;
	--emitter->field_02;
}

/* the block whose count 0x246c60 tests (+0x28) */
struct s_block_246c60
{
	byte unknown00[0x28];
	long count;
};

/* the three reference-counted objects 0x246c60 builds in place: each base
   starts its count at 1, and each object ends up flagged 0x80 */
class c_referenced_246c60 : public c_havok_reference_counted
{
public:
	__forceinline c_referenced_246c60()
	{
		reference_count = 1;
	}
};

class c_object_246c60_a_base : public c_referenced_246c60
{
public:
	__forceinline c_object_246c60_a_base(s_block_246c60 *block) : unknown08(0), block(block) {}

	long unknown08;
	s_block_246c60 *block;
};

class c_object_246c60_a : public c_object_246c60_a_base
{
public:
	__forceinline c_object_246c60_a(s_block_246c60 *block) : c_object_246c60_a_base(block)
	{
		reference_count = 0x80;
	}
};

class c_object_246c60_b_base : public c_referenced_246c60
{
public:
	__forceinline c_object_246c60_b_base(void *owner, void *data) : unknown08(0), owner(owner), data(data) {}

	long unknown08;
	void *owner;
	void *data;
};

class c_object_246c60_b : public c_object_246c60_b_base
{
public:
	__forceinline c_object_246c60_b(void *owner, void *data) : c_object_246c60_b_base(owner, data)
	{
		reference_count = 0x80;
	}
};

class c_object_246c60_c_base : public c_referenced_246c60
{
public:
	__forceinline c_object_246c60_c_base(c_havok_reference_counted *child) : unknown08(0), child(child)
	{
		child->reference_count++;
	}

	long unknown08;
	c_havok_reference_counted *child;
};

class c_object_246c60_c : public c_object_246c60_c_base
{
public:
	__forceinline c_object_246c60_c(c_havok_reference_counted *child) : c_object_246c60_c_base(child)
	{
		reference_count = 0x80;
	}
};

__declspec(noinline) void function_246c60(long unknown, void *block);

/* builds the three objects of a 0x70-byte storage block when the block has
   entries */
// @retail 0x246c60
void function_246c60(long unknown, void *block)
{
	s_block_246c60 *source = (s_block_246c60 *)block;
	byte *storage = (byte *)unknown;

	if (source->count > 0)
	{
		void *data = *(void **)(storage + 0x68);

		new (storage) c_object_246c60_a(source);
		new (storage + 0x50) c_object_246c60_b(storage, data);
		new (storage + 0x40) c_object_246c60_c((c_object_246c60_b *)(storage + 0x50));
	}
}
