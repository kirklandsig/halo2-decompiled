#include "unknown_11c920.h"
#include "effects.h"
#include "havok_reference.h"
#include <new>

// @flags /O2 /Gr

struct s_particle_impact;
struct s_object_246eb0;
void function_246e60(long tag_index, dword group, s_particle_impact const *impact,
	s_object_246eb0 const *particle, long definition_index);

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

// score 4 (was 4): virtual-table setup precedes the tag-result spill in retail;
// tried group const query 4; tag const query 4; both const queries 4; group signed return 4;
// tag unsigned return 4; tag const home 4; tag unsigned home 4; const query receiver 4. Restored.
// second virtual-call vtable/spill order remains.
// initial virtual-query result lifetime 4;
// invalid-query common exit 4;
// system index name after tag home 4;
// query tag name before definition home 4;
// group name before tag home 4;
// descriptive group and system names 4;
// descriptive query and group names 4;
// all three descriptive stack candidate names 4;
// Invalid-query cleanup return 38; complete cleanup in both branches 38; dispatch inner scope 4;
// countdown field pointer 38; const system index 4; sequenced query expression 4;
// unsigned-int group home 4; wide tag home 57.
// Direct nested queries 27; split tag assignment 4; split tag with direct group 8;
// split group assignment 4; both assignments split 4; outer callee inline/noinline/both 4/4/4.
// Volatile tag at function scope 4; all three locals at function scope 4; group declared before the
// volatile tag 4; without the volatile, (void)&tag 27 and tag at function scope 27 (particle moves to
// the stack and caller 0x247140 138).
// Round 39: nested queries as a diagnostic (27: no spill and particle stays in ebx, but the two stack
// arguments are pushed before the queries, so retail used named locals); the tag local set by the
// guard's query and re-queried in the block 27 (31 with the volatile); the emitter decrement before
// the particle store 8; the definition as a reference 4.
// Round 57: without the volatile the all-stack first pass gives the tag ebx and particle ebp
// (so particle stays a stack argument); renaming tag, definition index, group, particle or
// emitter leaves that pass unchanged. Without the volatile (one-file harness, aligned counts): plain
// 11 (`ret 8`, tag in ebx); tag declared before the definition index, emitter decrement first, group
// named: the same code as plain.
// Round 97: plain tag with the emitter address at the end or entry 27/27 (caller 138); tag read through
// its address 27; both addresses 27; const group home 4; const definition pointer 4;
// const particle or emitter parameter 4/4 (caller 137).
// @retail 0x248340
void function_248340(s_particle_system_datum *system, s_object_246eb0 *particle,
	s_emitter_248340 *emitter)
{
	c_view_248340 *definition = (c_view_248340 *)function_137bd0(system->function_1751d0()->tag_index);
	if (definition->v3() != NONE)
	{
		long definition_index = system->function_1751d0()->tag_index;
		long volatile tag_index = definition->v3();
		dword local_0 = definition->v2();
		function_246e60(tag_index, local_0, 0, particle, definition_index);
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
