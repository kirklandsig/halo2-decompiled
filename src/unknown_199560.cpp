// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_199560.CPP: compressed data blocks (lane H) */

#include "unknown_11c920.h"
#include "physical_memory.h"
#include "loop_allocator.h"
#include <xtl.h>

#define BYTE_SWAP_LONG(v) ((((v) & 0xff0000) | ((v) >> 16)) >> 8 | ((((v) << 16) | ((v) & 0xff00)) << 8))

__declspec(noinline) dword function_199560(void *data, dword size);

// @retail 0x199560
dword function_199560(void *data, dword size)
{
	dword result = 0;

	if (size >= 4)
	{
		result = BYTE_SWAP_LONG(*(dword *)data);
	}
	return result;
}

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
void function_12d520(long address);
double timing_ticks_to_seconds(__int64 ticks);
void *__stdcall loop_allocator_zalloc(void *opaque, long count, long size);
void __stdcall loop_allocator_zfree(void *opaque, void *address);
typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size, block_allocate allocate, block_free release, void *opaque);
long __stdcall function_2cb510(byte *destination, long *destination_size, byte const *source, long source_size, long level, block_allocate allocate, block_free release, void *opaque);

#include "unknown_080e10.h"

static __forceinline s_loop_allocator *compression_pool_allocate(long size)
{
    s_loop_allocator *result = NULL;
    __int64 start = function_80e13();
    if (g_4e6464->page_count > 0)
    {
        long retries = 0;
        do
        {
            result = (s_loop_allocator *)function_12d2f0(size, 0, 0, 0);
            if (!result)
            {
                if (retries < 90)
                {
                    retries++;
                    function_12c600();
                }
                else
                {
                    __int64 elapsed = function_80e13() - start;
                    if (elapsed < 0)
                        elapsed = 0;
                    if (!((real)timing_ticks_to_seconds(elapsed) < 1.0f))
                        break;
                    D3DDevice_KickPushBuffer();
                    D3DDevice_IsBusy();
                    SwitchToThread();
                }
            }
        } while (!result);
    }
    return result;
}

bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size);

// score 51 (was 63): retail hands the compressor (0x2cb510) the source in EAX, its size in ECX and the destination in
// EDX with five stack arguments; ours pushes all eight, since 0x2cb510 is still a stub. Earlier note (63): retail hands
// the compressor the source and its size in the first two registers and the destination and its size in the next two,
// so its frame is four bytes smaller than ours.
// Round 87 (m3 lane 14, fast): the pool allocator's start and elapsed stamps through the shared header reader
// (function_80e13, a zero-initialised result assigned from the raw reader) in place of this file's volatile copy:
// 63 -> 51 (the frame, the stamp stores and the whole allocation loop are retail's). BLOCKED-BY 0x2cb510.
// b15I (11-17) walk against retail, fast build on r848: every statement agrees with retail up to the compressor call:
// the capacity >= 4 guard (0x1995ac, jb at 0x1995b3), the pool allocation loop (0x1995e8-0x199660), 0x18e250 with size
// 0x4afb0 (0x19967d), the two flag bytes at +0x3c/+0x3d (0x1996b6, 0x1996b9), the byte-swapped size stored first
// (0x1996bc), the capacity - 4 store (0x1996c3, 0x1996cb), the allocator local stored before the call (0x1996e2), the
// +4 added back to the size only when the call returns 0 (0x1996ef), and the pool freed on every path (0x1996f3,
// 0x199707). No logic or branch-sense difference. Every differing instruction is the argument setup for the call at
// 0x1996e6: retail loads EAX (source, 0x1996db), ECX (source size, 0x1996e0) and EDX (destination + 4, 0x1996d8) and
// pushes five stack arguments (&allocator 0x1996c2, 0x24f880, 0x24f850, level 0x1996d7, compressed_size 0x1996df); ours
// pushes all eight in the stub's stdcall form (+1 byte, 408 vs 407). No form tried: a register convention cannot be
// declared for 0x2cb510 (library code, an @stub). No code edit, so callers 0x86800 and 0xb3ed0 are unaffected.
// @retail 0x1995a0
bool function_1995a0(byte const *source, dword source_size, byte *destination, long *compressed_size, dword capacity, long level)
{
    bool result = false;
    if (capacity >= 4)
    {
        s_loop_allocator *pool = compression_pool_allocate(0x4b000);
        if (pool)
        {
            function_18e250(pool, 0x4afb0, "zlib pool", NULL);
            pool->field3c = true;
            pool->field3d = true;
            *(dword *)destination = BYTE_SWAP_LONG(source_size);
            s_loop_allocator *allocator = pool;
            *compressed_size = capacity - 4;
            if (function_2cb510(destination + 4, compressed_size, source, source_size, level, loop_allocator_zalloc, loop_allocator_zfree, &allocator) == 0)
            {
                *compressed_size += 4;
                result = true;
            }
            function_12d520((long)pool);
        }
    }
    return result;
}
