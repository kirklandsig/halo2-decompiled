// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1470F0.CPP: the game's c_havok_abstract_allocator (vtable 0x45378c), which Havok
   allocates through: it takes memory from the fixed buffer while only that
   exists (g_479890 is 1), otherwise from the pool, then the ai's scratch
   buffers, then the buffer in physical memory, and frees an address to the
   allocator whose memory holds it (see unknown_146a20.cpp) */

#include "unknown_11c920.h"
#include "unknown_146a20.h"

extern long g_479890;
extern c_havok_abstract_allocator *g_479894;
extern c_havok_abstract_allocator *g_479898;
extern c_havok_fixed_memory *g_47989c;

c_havok_abstract_allocator *function_146e30(void);

class c_game_havok_memory : public c_havok_abstract_allocator
{
public:
	virtual void *allocate(long size, long memory_class);
	virtual void deallocate(void *address);
	virtual void *allocate_aligned(long alignment, long size, long memory_class);
	virtual void deallocate_aligned(void *address);
	virtual void *allocate16(long size, long memory_class);
	virtual void deallocate16(void *address, long size, long memory_class);
	virtual void method6(long unknown) {}
	virtual void method7(long unknown) {}
};

/* whether the address lies in the block */
#define ADDRESS_IN_BLOCK(address, start, end) ((long)(start) <= (long)(address) && (long)(end) >= (long)(address))

// @retail 0x1470f0 deleting c_game_havok_memory

// @retail 0x147110
void *c_game_havok_memory::allocate(long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate(size, memory_class);
	}
	result = g_479898->allocate(size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate(size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate(size, memory_class);
		}
	}
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 9 (was 9): retail's second block uses eax for its bounds and edx for its vtable, so its
// call stays apart and the third joins the first; ours uses edx/eax there, so the third joins the
// second. The third's end addition and the fallback's ecx/push order also differ.
// Tried: plain four-call chain 38; third in the inclusive end form of the first two 7 (not kept, two
// points); four calls with inclusive first/third 29, all inclusive 29; shared dispatch with inclusive
// second/third only 31; third abstract/fixed-base dispatch 19/19; nested receivers 19;
// first/second/both inclusive 18/28/27; shared receiver name 19 and with inclusive 27; allocation
// local 19; fallback inline/noinline/consistent-inline declarations 19; inverted third 19; third
// predicate 19; first/third shared dispatch 11 with concrete/same-local second 11/11 and inclusive 9;
// early third 39 (132/121); late second 9; second predicate 9; dispatch flag 38 (109/121); inverse
// admission with separate range lifetimes 41-52, shared end 52; native range lifetimes 31/9, shared end
// 31; each receiver global read once on its path 9; a hand-merged first/third dispatch 9; void
// completion second/fallback/both 26; common 9; second/common and all completions 9; entry null return
// with all completions 26; inclusive third with second receiver lifetime 7; eager second/first ends
// 9/32; address-first second lower 8; scoped first/second range results 7; fallback result local 7;
// entry/reversed receiver declarations 7; eager first receiver 7; named second/first upper predicates
// 7; standalone third range 7; no barrier 49 (109 bytes: all three share one call), with the inclusive
// third 49; a forceinline inclusive range helper for all three 44 (135), without the barrier 43
// (123); second and third nested under the first's else 9, with the inclusive third 9. Round 55: four
// plain calls with `>= || ==` ends in all three ranges 29, with a plain `>=` second 29; the second and the
// fallback sharing one receiver call through a label 43; the barrier in the second arm instead of the first
// 49; the third and the fallback sharing the final call 49. A one-file LTCG screen of 194 forms (call sharing
// x end test x barrier arm, and four plain calls) found none below 9 but the inclusive third; four plain calls
// give retail's second block and fallback but join the first range to the second's call.
// @retail 0x147160
void c_game_havok_memory::deallocate(void *address)
{
	if (address)
	{
		c_havok_abstract_allocator *allocator;
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)((byte *)g_479898 + 0xf0340) >= (long)address || (byte *)g_479898 + 0xf0340 == address)))
		{
			_ReadWriteBarrier();
			allocator = g_479898;
		}
		else if (((long)((byte *)g_479894 + 0x30) <= (long)address && ((long)((byte *)g_479894 + 0x15030) >= (long)address || (byte *)g_479894 + 0x15030 == address)))
		{
			allocator = g_479894;
			allocator->deallocate(address);
			return;
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			allocator = g_47989c;
		}
		else
		{
			function_146e30()->deallocate(address);
			return;
		}
		allocator->deallocate(address);
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1471e0
void *c_game_havok_memory::allocate_aligned(long alignment, long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate_aligned(alignment, size, memory_class);
	}
	result = g_479898->allocate_aligned(alignment, size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate_aligned(alignment, size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate_aligned(alignment, size, memory_class);
		}
	}
	return result;
}

#pragma intrinsic(_ReadWriteBarrier)
// score 19 (was 19): retail tests the end of the block with >= and gives the second block its own
// deallocate, ours shares the first and the second.
// Second return 19; first+second returns 19; address local 19; second concrete 19; second concrete
// with third base 19; primary/second branch inversions 19/19; third-before-second 19; fallback local 19;
// second predicate 19. First start/end 41; with second return/concrete/end/receiver 41/41/42/41;
// inclusive tests and inclusive returns 42/42; first start/end without the barrier 41.
// 0x147160's shared first/third dispatch with inclusive ends and the barrier 9 (not kept: redundant
// end tests), without the barrier 49; four plain calls with inclusive / exclusive ends 38/29;
// shared dispatch with exclusive ends 47.
// @retail 0x147240
void c_game_havok_memory::deallocate_aligned(void *address)
{
	if (address)
	{
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)((byte *)g_479898 + 0xf0340) > (long)address || (byte *)g_479898 + 0xf0340 == address)))
		{
			_ReadWriteBarrier();
			g_479898->deallocate_aligned(address);
		}
		else if (((long)((byte *)g_479894 + 0x30) <= (long)address && ((long)((byte *)g_479894 + 0x15030) > (long)address || (byte *)g_479894 + 0x15030 == address)))
		{
			g_479894->deallocate_aligned(address);
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			g_47989c->deallocate_aligned(address);
		}
		else
		{
			function_146e30()->deallocate_aligned(address);
		}
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1472c0
void *c_game_havok_memory::allocate16(long size, long memory_class)
{
	void *result;

	switch (g_479890)
	{
	case 1:
		return g_479894->allocate16(size, memory_class);
	}
	result = g_479898->allocate16(size, memory_class);
	if (!result)
	{
		result = g_47989c->allocate16(size, memory_class);
		if (!result)
		{
			result = function_146e30()->allocate16(size, memory_class);
		}
	}
	return result;
}

// score 8 (was 8): First upper-bound branch pair and EAX bound reuse still differ; shared bounds 36;
// outer bounds 36; shared bounds with delayed fixed receiver 43; numeric bounds 36;
// shared upper/lower 36/36; read-only bounds 36; shared bounds with exits 45.
// Fixed membership 95; paired memberships 146; independent pool/fixed/physical bounds 106/48/48;
// all independent bounds 134; separate pool-bound decisions 36; eager memberships 114.
// First upper-bound branch pair, EAX bound reuse and shared-dispatch placement still differ.
// first upper-bound branch pair is missing and bound
// registers differ. Typed pool bounds 36; typed bounds and pool dispatch 73;
// direct pool fields 36; typed bounds with delayed fixed receiver 43; fixed
// object size 36; physical object reference 36; pool object reference 36;
// typed fixed receiver 36.
// Shared inclusive begin/end helper 43; begin/size helper 44; helper with the fixed
// receiver 36; begin/size helper with early returns 44; size-first end 43. Retail's
// jge/jne pair and EAX ends come only from a second (equality) test of each end.
// Round 282: void-return completion on pool/fixed/physical/fallback paths
// 36/36/45/36; pool+fixed/pool+physical/three pools/all paths 36 each.
// Original calls restored; first upper-bound branch pair and bound-register reuse remain.
// the first range's end is now a named local tested for equality, which gives
// the first range's end is now a named local tested for equality, which gives
// retail's jge/jne pair and the shared dispatch block; the first range's start and the second and
// third ranges' ends are still computed in edx where retail uses eax. Only an end named before
// the first test keeps the jne, and it is then computed before the jg; every end written in the
// test (>= or > with ==, either order, casts, comma, range helper) gives a lone jge and split calls.
// Inclusive third 6 (moves 0x147160 9 -> 7 and 0x147240 19 -> 30); end named at function scope
// with inclusive third 11; inline range helper for the first two 10; inline >= || == end 70;
// start test nesting the named end 70; third testing == first 23.
// Round 33: every range's start and end share EAX in retail; ours computes the named first end
// before the jg (start in EDX) and gives the second and third ends EDX (`add edx, eax`, the same
// third-range difference as 0x147160 and 0x147240). Third end as `m_size + (byte *)m_buffer` 8
// (same code), as `(long)m_buffer + m_size` 8; first end inline with a barrier in the first arm 60
// (153 bytes); first end as a long local 44 (the == folds away); first start named too 8; the
// equality test before the >= 10; permuter, 59 tries, none lower.
// @retail 0x147320
void c_game_havok_memory::deallocate16(void *address, long size, long memory_class)
{
	if (address)
	{
		byte *pool_end = (byte *)g_479898 + 0xf0340;
		if (((long)((byte *)g_479898 + 0x4340) <= (long)address && ((long)pool_end >= (long)address || pool_end == address)))
		{
			g_479898->deallocate16(address, size, memory_class);
		}
		else if (ADDRESS_IN_BLOCK(address, (byte *)g_479894 + 0x30, (byte *)g_479894 + 0x15030))
		{
			g_479894->deallocate16(address, size, memory_class);
		}
		else if (ADDRESS_IN_BLOCK(address, g_47989c->m_buffer, (byte *)g_47989c->m_buffer + g_47989c->m_size))
		{
			g_47989c->deallocate16(address, size, memory_class);
		}
		else
		{
			function_146e30()->deallocate16(address, size, memory_class);
		}
	}
}

/* the game's c_havok_abstract_allocator, which Havok's code asks for */
c_game_havok_memory g_4798b0;

// @retail 0x1473b0
c_havok_abstract_allocator *function_1473b0(void)
{
	return &g_4798b0;
}
