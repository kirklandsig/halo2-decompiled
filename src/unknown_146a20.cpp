// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_146A20.CPP: the memory Havok allocates from. One block of virtual
   memory (g_479888) holds the allocators, built in place: a fixed buffer, a
   Havok pool, and two fixed buffers made when the pool runs short, one in
   the ai's scratch buffers (g_47989c) and one in physical memory
   (g_4798a0). The game's own c_havok_abstract_allocator (vtable 0x45378c, 0x147110 and on)
   allocates from them. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_146a20.h"
#include "physical_memory.h"
#include "unknown_223b60.h"
#include "unknown_080e10.h"
#include <xtl.h>
#include <new.h>

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
void function_12d520(long a);
double timing_ticks_to_seconds(__int64 ticks);

/* the havok system (unknown_1c25a0.cpp and later) */
void function_1c27a0(void);
void function_1c2690(void);
void __stdcall function_1c4590(long unknown);
bool function_1c4040(long attempt, bool active, bool any_object, bool even_if_unknown, long excluded_component_index);

/* the block the allocators live in (unknown_1c25a0.cpp) */
extern void *g_479888;

/* the ai's scratch buffers (ai.cpp) */
extern byte *g_510c44;
extern bool g_510c48;

/* the fixed buffer in physical memory (unknown_147090.cpp) */
extern c_havok_fixed_memory *g_4798a0;

#define HAVOK_SCRATCH_MEMORY_SIZE 0x452e8

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void function_146b80(void);
void function_146da0(void);
void function_146de0(void);
long function_146fd0(void);

bool g_479880;
long g_479884;
long g_47988c;
long g_479890;
c_havok_abstract_allocator *g_479894;
c_havok_abstract_allocator *g_479898;
long g_4798b4;
bool g_47f05a;
bool g_47f05b;

// score 34 (was 34): the base vtable store is still dropped and the pool placement
// still reloads g_479888 at the join where retail reloads it on the constructed arm.
// Earlier headers, merged: unchanged that round (34); a one-file LTCG probe keeps the
// store only for a load through an unknown pointer between the two stores, which
// belongs to the abstract allocator's constructor in the header and so is out of reach
// from this file (34); the vtable slot at [eax] is never filled in, so the two stores
// that follow start one field too late; retail stores the vtable there first (34).
// Retail keeps the abstract allocator's vtable store only here (0x146b80, 0x146e30 and 0x22c3e0
// drop it), as if something read memory between the two vtable stores; that also keeps 1 in edx.
// Without `block` (g_479888 read at both placements) the reload sits on the constructed arm as in
// retail but the 1 becomes immediates: 38. Tried: the pool placed at `block + 0x15040` 40; the
// buffer argument through g_479888 38; `(c_havok_fixed_memory *)g_479888 + 1` 38; a typed
// `fixed` pointer local with `fixed + 1` 34; `block` read after the three global stores 34; the
// fixed buffer in a local before storing g_479894 38; `block` reassigned from g_479888 for the pool 34.
// A constructor that takes only the size and puts the buffer at `this + 1` (in the fixed buffer or
// its base, with or without `block`), or a subclass holding the 0x15000 bytes, gives retail's reload
// and the 1 in edx, leaving only the dead vtable store: 35 each (140 bytes). A one-file LTCG harness
// keeps the store for none of member reads, constant-folded branches, a cross-file constructor or the
// globals as one struct.
// Force-inline abstract constructor 34; scoped block 34; abstract-typed block 34; refcount-typed block 34;
// intermediate/derived/abstract result reused for the pool 44/44/44; intermediate result published 38;
// inline target 34.
// Round 7 (lane 13): retail's matched functions that keep a base vtable store (0x2784c0, 0x2dec70 and the
// other Havok constructors) all read memory through another pointer between the two stores; here nothing
// does. On the size-only constructor: intermediate class novtable 35, virtual destructor in the fixed class
// 35, base stores through an inline initialize member 35, abstract loop split 3 + NONE 34 (0x146b80 and
// 0x22c3e0 lose their matches), a buffer/size description struct read by the base constructor 36 (146
// bytes; the reads are forwarded before the store is dropped). Harness only: memset in the abstract
// constructor, size by reference bound to a local, buffer by reference: store dropped each time.
// inline target 34. Round 5 (g_479888 read at both placements, which puts the reload on the
// constructed arm, while only the `block` form keeps the 1 in edx): chained flag assignment 38;
// pool address through a byte pointer named after the first construction 38; if/else on the
// placement result 38; bool flag first 38; NONE flag last 38. Four explicit base-constructor
// stores in place of the loop still drop the vtable store (30, 0x146b80 lost). Round 20 (size-only
// constructor, g_479888 at both placements): a class placement operator new 35; the size by const
// reference 35; `m_size = size; m_size &= ...` 35; the base's size by const reference 35; the
// construction copied from a temporary 34 (elided). A one-file LTCG harness also keeps the store for
// none of: the abstract constructor out of line in another file (still inlined, store dropped), a
// virtual call in the derived constructor (devirtualised after the final vtable store), out-params
// by reference in the base, a pointer-walk loop.
// stores in place of the loop still drop the vtable store (30, 0x146b80 lost).
// Round 103 (fast, with 0x146b80/0x146e30/0x22c3e0): the fixed base storing the count before the
// size 33 (0x146e30 121); the abstract loop reading a const initial table 34; the fixed base's
// fields as a member initializer list 34; `__declspec(novtable)` fixed base 34; size-only
// constructor whose body reads the buffer into the count before clearing it 34 (140 bytes).
// Harness only: a field read in the fixed base (not forwarded, store still dropped) 23 but
// 0x146b80 15; a description struct by const reference 36 (146 bytes, 1 as immediates).
// Round 37: the fixed, fixed-base and abstract constructors each defined out of line in another file (LTCG
// inlines them) 34/34/34, no other function changes; the block typed as its layout (fixed allocator, buffer,
// pool) 34; the count flag stored first 38. With the vtable store forced (diagnostic only) 12 at 147 bytes:
// the rest is the pool placement's reload of g_479888 at the join, where retail reloads on the constructed arm.
// Round 3 (lane 18, header constructors, with 0x146b80/0x146e30/0x22c3e0): the count header's 1 as a member
// initializer 34; the abstract loop counting down 33 (0x146b80 2, 0x22c3e0 lost); its NONE test as `i < 3`
// 34; the fixed base storing the count first 32 (0x146e30 120 -> 121); the size before the buffer 33
// (0x146b80 2). The base vtable store is dropped in every form.
// Round 235: publish fixed identity before construction, saved/published placement
// and published buffer 35/35/46; pool identity with saved/published placement 52/52;
// both identities 53/53/64. All original publications and executable code restored.
// Round 22 (lane 13, fast with 0x146b80/0x22c3e0 matched): the three global stores after the
// first construction 41 (143 bytes); g_479880 stored first 39; the block typed `byte *` 34;
// the first placement as an explicit if/else on the block 34; the block typed as the fixed
// allocator with the buffer at `block + 1` 34 (close to the listed `fixed + 1` form); g_479890
// stored first 39.
// Round 97 (lane 8, fast with 0x146b80/0x146e30/0x22c3e0/0x1c2690): retail's 0x4798b4 is the reference
// count (+4) of the game's allocator object at 0x4798b0 (0x1473b0 returns it); the increment through
// 0x1473b0 34 (same code), with g_479888 at both placements 38. The fixed class without the
// intermediate base 31 (its vtable store moves up beside the abstract stores; 0x146b80 3). g_479888 at
// both placements re-measured 38: retail's reload and layout, the 1 as three immediates, no base store.
// An explicit `block ? new (block) ... : NULL` after the global stores 34 (same code).
// Round 32 (lane 11, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0; retail's 0x453760 is the abstract class's
// own vtable, eight pure slots, and 0x146b80 builds the same class at 0x105410 with the store dropped): the
// fixed class flattened onto the abstract one 31 (0x146b80 3); `::new` 34; the refcount header as a member
// in place of a base 34; the abstract constructor storing NONE before its zero loop 31 (0x146b80 4); the
// masked size in a named local 34. The store is dropped in all (141 bytes).
// Round 7 (m3 lane 18, fast with 0x146b80/0x22c3e0 matched): the pool address read from g_479888
// before the fixed construction 43 (140 bytes); the block typed as one layout struct (fixed
// allocator, buffer, pool) read from g_479888 at each placement 38 (144 bytes); the fixed placement
// from g_479888 with the buffer from `block` 36 (146 bytes: reload on the constructed arm, but the 1s
// become immediates, no base store); the layout struct through `block` with the pool through
// g_479888 34 (same code). Diagnostic only (header): a global read in the fixed base constructor,
// between the two vtable stores, still drops the base store.
// Round 113 (lane 6, fast with 0x146b80/0x22c3e0 matched): a typed fixed pointer from g_479888 with the
// buffer at `fixed + 1` 34 (same code), read after the three global stores 34; the first placement through
// g_479888 with the buffer through `block` 36 (146 bytes by chance: block copied to EDX, the 1s immediates,
// no vtable store); neither local (diagnostic, listed) 38 (reload on the constructed arm as retail, 1s
// immediates); a buffer local read from g_479888 before the global stores 38.
// Round 53: `block` for the placement and the pool with the buffer from g_479888 43; `block` read after
// the three stores with the placement from g_479888 36 (146 bytes), with the buffer from g_479888 38;
// `block` (at the top or after the stores) used only for the pool 34/34 (149 bytes). Compile-only, all
// store orders and `block`/g_479888 mixes for placement, buffer and pool: the 1 in a register comes only
// with both from `block`, the arm reload only when the arm reads g_479888; never both, no base store.
// Round 100 (lane 8, one-file harness exact with unknown_22c0a0.cpp): diagnostic only, a compiler barrier at the end of
// the abstract constructor keeps the base vtable store (12 at 147 bytes; 0x146b80/0x22c3e0 lose), and with the size-only
// constructor it is an exact 0, so the kept store is the last difference. Forms: the size-only buffer as
// `(byte *)this + sizeof(*this)` 35; the size-only constructor setting the buffer in its body after a NULL base argument
// 35; an explicit upcast at the publish 35; `byte *block` 34 (same code); the fixed allocator in a typed local first 38.
// Round 22 (m3 lane 2, fast with 0x146b80/0x22c3e0 matched): `_ReadWriteBarrier()` after the three global stores 41,
// the same with g_479888 at both placements 41 (144 bytes); `g_4798b4++` before the constructions 48 (137 bytes); the
// size-only constructor 35, with the barrier after / before the global stores 41/34.
// Round 43 (fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0): a byte block for the buffer and the pool 40; the
// buffer named before the global stores 36; the construction in a typed local published after 38; the size as a
// named const 34 (same code); the pool address named after the first construction 34 (same code).
// Round 122 (lane 5, fast with 0x146b80/0x22c3e0 matched): the size-only constructor re-measured 35
// (140 bytes, only the abstract vtable store missing); on it, the fixed base's buffer/size stores under
// a constant-folded `if (size > 0)` 35 (a folded branch does not keep the store); the buffer as `block +
// sizeof(c_havok_fixed_memory)` 34 (same code); the buffer named after the three global stores 36 (146
// bytes, the 1s become immediates); `g_479894 = NULL; if (block) g_479894 = new (block) ...` 32 (139
// bytes, the NULL store moves above the test, the vtable store still dropped).
// Round 106 (lane 7, fast with every function of the files using the header): the size-only
// constructor (buffer `this + 1`) 35 (140 bytes; 0x146b80/0x22c3e0 kept); the abstract constructor
// clearing its four words with memset and then storing NONE 38 (152 bytes), clearing three words 37
// (149), each with the size-only constructor 46/27 (153/150): the base vtable store is still dropped
// and 0x146b80/0x146e30/0x22c3e0 lose (memset keeps a pointer to the words). Compile-only is not
// exact here (the constants' registers differ).
// Round 105 (lane 8, fast with 0x146b80/0x22c3e0/0x146ac0): retail 0x146b80 and 0x146e30 build the
// same class at g_479888 + an offset and drop the base store; only here (object at g_479888 itself,
// buffer at this + 0x30) is it kept. Header forms: the abstract array value-initialised with NONE set
// in the body 42 (0x146b80 21, 0x22c3e0 219); the fixed base storing buffer, count, size 33 (Round
// 103's count-before-size form re-measured); the abstract loop zeroing all four slots, then NONE 21
// (144 bytes; the plain dead zero store at +0x14 survives the loop, but the vtable store is still
// dropped; 0x146b80 15, 0x22c3e0 207). Diagnostics (not kept): the pool constructed first 52; the
// pool construction removed 75: the base store is dropped either way, so nothing after the
// construction keeps it.
// Lane 12 r66 (fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0; a /GL- compile-only build also drops the store):
// g_479880 stored before g_479890 36; `g_4798b4 += 1` 34 (same code); the buffer at `block +
// sizeof(c_havok_fixed_memory)` 34 (same code); the pool placed through `block` too 40 (135 bytes); the
// construction in a typed local published after the pool construction 38 (154 bytes).
// Round 19 (lane 17, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0/0x146de0/0x146da0/0x147090/0x2d6fc0):
// the size-only constructor re-measured 35 (140 bytes, only the abstract vtable store missing). On it: a
// constructor taking the whole block size (buffer `block_size - sizeof(*this)`) 35; the pool placed at
// `(byte *)g_479894 + 0x15040` 44 (131 bytes); the fixed allocator and its 0x15000-byte buffer as one
// struct whose constructor passes its buffer member 35; `g_4798b4++` between the two constructions 39
// (136 bytes); the three global stores after the fixed construction 42 (142 bytes).
// @retail 0x146a20
void function_146a20(void)
{
	void *block = g_479888;

	g_479884 = NONE;
	g_479890 = 1;
	g_479880 = true;
	g_479894 = new (block) c_havok_fixed_memory((byte *)block + 0x30, 0x15000);
	g_479898 = new ((byte *)g_479888 + 0x15040) c_havok_page_pool_allocator;
	g_4798b4++;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x146ac0
void function_146ac0(void)
{
	if (g_47989c)
	{
		function_146de0();
	}
	if (g_4798a0)
	{
		((c_havok_abstract_allocator *)g_4798a0)->~c_havok_abstract_allocator();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
	g_479894->~c_havok_abstract_allocator();
	c_havok_abstract_allocator *last = g_479898;
	_ReadWriteBarrier();
	g_479894 = NULL;
	_ReadWriteBarrier();
	last->~c_havok_abstract_allocator();
	g_479898 = NULL;
	g_479890 = 0;
	g_479880 = false;
}

#pragma function(_ReadWriteBarrier)

/* a page aligned block of virtual memory: its size and the offset back to
   the allocation sit before it */
static void virtual_memory_allocate_aligned(void **block_out, long size)
{
	void *result = NULL;
	byte *block = (byte *)VirtualAlloc(result, size + 0x1000, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!block)
	{
		GetLastError();
	}
	else
	{
		byte *aligned = (byte *)(((dword)block + 0x1000) & ~0xfff);

		((long *)aligned)[-2] = size;
		((long *)aligned)[-1] = aligned - block;
		result = aligned;
	}
	*block_out = result;
}

// @retail 0x146b30
void function_146b30(void)
{
	virtual_memory_allocate_aligned(&g_479888, 0x105470);
}

// @retail 0x146b80
void function_146b80(void)
{
	byte *buffer = g_510c44;
	long size = HAVOK_SCRATCH_MEMORY_SIZE;

	g_510c48 = true;
	if ((dword)buffer & 0xf)
	{
		long adjustment = -(long)buffer & 0xf;

		buffer += adjustment;
		size -= adjustment;
	}
	g_47989c = new ((byte *)g_479888 + 0x105410) c_havok_fixed_memory(buffer, size);
}

// score 5 (was 5): retail's function_1c4040 takes the active flag in AL and reads
// it after the other arguments are pushed; here the callee takes it in DL, so the
// byte is loaded into DL before the other byte is stored. The callee lives in
// src/unknown_1c25a0.cpp and is far from retail.
// @retail 0x146bf0
void function_146bf0(void)
{
	if (!g_47f05b)
	{
		long level = function_146fd0();
		long last_time = g_479884;
		long attempt;

		g_47f05b = true;
		for (attempt = 0; attempt < 3 && level >= 2; attempt++)
		{
			long count = 0;
			long pass;

			if (level >= 3)
			{
				function_1c4590(1);
				last_time = g_479884;
			}
			level = function_146fd0();
			for (pass = 0; pass < 3 && level >= 2; pass++)
			{
				bool active = pass == 2;
				bool any_object = pass > 1;
				long tries = 0;

				do
				{
					bool recent = last_time != NONE && g_510c54->game_time - last_time < 10;

					if (!function_1c4040(attempt, active, any_object, recent, NONE))
					{
						if (!attempt && !recent)
						{
							function_146da0();
							level = function_146fd0();
							g_479884 = last_time = g_510c54->game_time;
							break;
						}
						last_time = g_479884;
						break;
					}
					level = function_146fd0();
					if (level < 2)
					{
						last_time = g_479884;
						break;
					}
					tries++;
					if (++count % 8 == 0)
					{
						function_146da0();
						level = function_146fd0();
						g_479884 = last_time = g_510c54->game_time;
					}
					else
					{
						last_time = g_479884;
					}
				}
				while (tries <= 50);
			}
		}
		g_47f05b = false;
	}
}

// @retail 0x146da0
void function_146da0(void)
{
	bool scratch = g_47989c != NULL;

	function_1c27a0();
	g_47f05a = true;
	function_1c2690();
	g_47f05a = false;
	if (scratch && !g_47989c)
	{
		function_146b80();
	}
}

// @retail 0x146de0
void function_146de0(void)
{
	if (g_47989c)
	{
		((c_havok_abstract_allocator *)g_47989c)->~c_havok_abstract_allocator();
		g_47989c = NULL;
		g_510c48 = false;
	}
	if (g_4798a0)
	{
		((c_havok_abstract_allocator *)g_4798a0)->~c_havok_abstract_allocator();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
}

// score 120 (was 120): the function still misses retail's inline of the texture cache
// update (0x12c600) at this one call site and stays 22 bytes short; unchanged this
// round
// score 120 (was 143): the function still misses retail's inline of the texture
// cache update (0x12c600) at this one call site, so it stays 22 bytes short;
// this round's trials in this file found no lower form of it
// score 120 (was 143): BLOCKED-BY 0x12c600: retail inlines the texture cache update (0x12c600)
// at this one call site only (its other 13 callers call it), ours calls it; the padding is consumed
// only when the allocation succeeds and the handle is published inside the retry loop.
// Lane 10 r15: `inline` on 0x12c600 inlines it at every caller (all 14 worse, matched 0x8c790,
// 0x1241b0, 0x194260 lost; reverted). Retail computes the padding after the loop:
// `long padding = (((dword)block + 0xf) & ~0xf) - (dword)block;` then `c_havok_fixed_memory(block,
// size - padding)` gives retail's prologue, frame entry, registers (size EBP, block EDI) and tail
// instruction for instruction, 151 here only because the missing inline leaves it 55 bytes short;
// the same as `size -= ...` 149, as a pointer difference in the argument 163, as `-(long)block & 0xf`
// 152. A local forceinline copy of the update (diagnostic) also inlines its callees 207.
// Earlier: global analysis off 120 (0x50 frame); frame pointer 161; size/speed 281/143; local
// analysis with frame/size 120/158; frame with size/speed 281/161.
// Constructed allocator result 164; conditional usable capacity 140 (frame 0x14); conditional
// padding 141; block/padding outer scopes 143; current block-first homes 143; construction scope
// 143; zero-first padding 143 at 377 bytes. Per-attempt padding after/before publication 149/149;
// consumed padding 139/151; guarded consumption 145; rounded padding/consumption 163/162; bounded
// search 149. Update defining-source inline/force-inline declarations 139/139; separate
// request/usable capacity 139; per-attempt request 139; split capacity plus inline/force-inline
// update 139/139; request plus inline 139; native handle plus split capacity 139. Byte/word
// alignment padding 150/140; allocation success result 143; explicit placement guard 143; post
// retry capacity 143. Constructor mask 139; repaired native construction result with/without mask
// 164/164; entry capacity plus mask 141; native buffer/address/padding-scope plus mask 139/139/139;
// actual masked-capacity local 138. Final rounded/low-bit/conditional/word padding (inside the
// loop's structure) 149/152/143/145; signed-word per-attempt padding 129 (kept then);
// capacity/allocation/joint home names 139/139/139. Signed-short delta 139; attempt/final short
// scope 129/129; short remainder consumption 136; entry capacity 131; unsigned capacity/native
// handle/separate usable with short padding 129/129/129. Wrapped/full boundary distances 164/149;
// direct consumption 164; counted distance 153; remainder update 140; byte padding 144; unsigned
// capacity/native allocation 139/139. Short-padding failure/body, counted and capacity loops
// 129/131/129; zero-default/select padding 134/129; allocator result 129; full short distance 135;
// success-only padding consumption 120 (held: the guard repeats the loop's own test). Separate
// byte/char/word/dword/short/long remainders with short padding 134/134/136/146/136/146; byte/word
// conditional padding 134/136.
// @retail 0x146e30
c_havok_abstract_allocator *function_146e30(void)
{
	if (!g_4798a0)
	{
		long size = 0x800000;
		byte *block;
		short adjustment;

		do
		{
			block = (byte *)function_80e14(size, 0, 0, 0, 90, 1.0f);
			g_47988c = (long)block;
			if (block)
			{
				adjustment = (dword)block & 0xf;
				if (adjustment)
					adjustment = 0x10 - adjustment;
				size -= adjustment;
			}
		}
		while (!block && ((size >>= 1) >= 0x100000));

		g_4798a0 = new ((byte *)g_479888 + 0x105440) c_havok_fixed_memory(block, size);
		function_1c4590(1);
	}
	return g_4798a0;
}

// @retail 0x146fd0
long function_146fd0(void)
{
	long physical_level = 0;
	long pool_level;

	if (g_4798a0 && g_4798a0->m_count > 0)
	{
		physical_level = 3;
	}
	long scratch_level = 0;
	if (g_47989c && g_47989c->m_count > 0)
	{
		scratch_level = 3;
	}
	if (g_479898)
	{
		long level;
		c_havok_page_pool_allocator *pool = (c_havok_page_pool_allocator *)g_479898;

		level = 3;
		if ((real)pool->page_count * (1.0f / 118.0f) < 0.8f)
		{
			real used = pool->get_used_fraction();

			if (used < 0.85f)
			{
				level = 2;
				if (used < 0.75f)
				{
					level = 1;
					if (used < 0.65f)
					{
						level = 0;
					}
				}
			}
		}
		pool_level = level;
	}
	else
	{
		pool_level = 0;
	}
	return MAX(scratch_level, MAX(pool_level, physical_level));
}
