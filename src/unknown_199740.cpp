// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_199740.CPP: unpacking a compressed data block (0x199740); its own
   file because retail compares floats with fcomi here (/arch:SSE) */

#include "unknown_11c920.h"
#include "physical_memory.h"
#include "loop_allocator.h"
#include "unknown_080e10.h"
#include <xtl.h>

__declspec(noinline) dword function_199560(void *data, dword size);

void function_12d520(long address);
void *__stdcall loop_allocator_zalloc(void *opaque, long count, long size);
void __stdcall loop_allocator_zfree(void *opaque, void *address);
typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size, block_allocate allocate, block_free release, void *opaque);

// score 3 (was 3): the low timestamp-zero store precedes PUSH EDI and the false-result
// store; the frame, timestamp halves, retry body and decompressor setup now agree.
// Earlier call graph at 52: 0x2cb5b0: retail passes it source in eax, source size in ecx,
// destination in edx and the destination size in edi (three pushes); its stub is built without
// /GL, so ours pushes all seven, and the argument loads after the pool setup differ. Retail also
// keeps the low half of the start time in ebp and the high half in edi (ours the other way round)
// and stores the timer zero after the edi save. A real 0x2cb5b0 body (an inflate wrapper over
// stubbed 0x2cc8d0/0x2cc9d0/0x2cc890) takes eax/ecx/edx but the size in esi: 64 (323 bytes);
// stdcall or cdecl stubs 64; retail's field store order 66; one-exit result 64. Retail's esi comes
// from 0x2cc8d0 taking its stream in esi, so its real body (and its library callees 0x2cc850,
// 0x2cc890, 0x2ce950) is needed first. Round 158: unsigned start 52; start read before the result
// local 52; elapsed as a separate subtraction 52; plain-inline pool helper 301 (not inlined);
// retry counter declared before the test 52.
// retail keeps the low half of the size in ebp and the high
// half in edi, ours keeps them the other way round, and retail saves the third
// argument after the push, so the stores and the subtraction differ.
// BLOCKED-BY 0x2cb5b0: retail passes it source in eax, source size in
// ecx, destination in edx and the destination size in edi (three pushes); its stub is built
// without /GL, so ours pushes all seven. A real 0x2cb5b0 body (an inflate wrapper over
// stdcall or cdecl stubs 64; retail's field store order 66; one-exit result 64. Retail's esi
// comes from 0x2cc8d0 taking its stream in esi, so its real body is needed first.
// Native unpack status at entry/after allocation 130/102; raw storage/unsigned bytes/both
// 52/52/52; entry status with raw storage/unsigned bytes 130/130; later status with both 102.
// Round 96: result declared after the allocation 89; the retry loop with break/continue (the
// matched 0x194260 form) 52; the allocator local before the pool setup 57; elapsed as a read
// then `-= start` 52; the timestamp read before the result default 52.
// Native byte/char/short success 105/105/106; direct success 54; allocation-derived
// success 66; inverse failure 120; final-only result 124; entry callback storage 22.
// Pool itself as callback context 74; native numeric/raw storage receivers 3/3;
// immutable pool pointer 3; callback context after header read 15; scoped context 3;
// success published after release from Boolean/native error 48/11.
// only the first timer-zero dword store precedes PUSH EDI and the result-byte
// store; retail puts it after both. The frame, timestamp pair, retry work and decompressor
// convention now agree. Earlier with the standard decompressor: real wrapper 64 (323/319 bytes),
// stdcall/cdecl stubs 64; field store order 66; one-exit result 64. Unsigned start 52;
// start before the result local 52; separate elapsed subtraction 52; plain-inline pool helper
// 301; retry counter before the test 52. Native unpack status at entry/after allocation
// 130/102; raw storage/unsigned bytes/both 52/52/52; entry status with raw storage/unsigned
// bytes 130/130; later status with both 102. Result after allocation 89; break/continue retry
// loop 52; allocator local before pool setup 57; elapsed read then -= start 52; timestamp
// before the result default 52.
// Round 202 with the recovered callee contract: byte/signed-byte/short success 105/105/106;
// direct test result 54; success from pool admission 66; inverse failure result 120;
// result assigned in both allocation outcomes 124; entry allocator-output workspace 22.
// All result forms and workspace lifetimes restored; shared helper bodies and qualifiers unchanged.
// Round 51: pool declaration split 3; pool declared before result 3; result declaration split 3;
// result renamed 3; callback-context names early/late 3/3; combined pool/result/context forms 3;
// callback-context declaration split 3. Original restored; the duplicate low timestamp store remains.
// Round 131: the same low-dword hoist above the last push is in 0x6a8a0, 0x18f260, 0x12d400 and
// 0x223b60; a compile-only screen ties every reader spelling (two volatile dwords, LARGE_INTEGER
// halves or QuadPart, a two-long array, unsigned, declared then assigned, braces, plain inline, a
// separately defined reader inlined at link time). Real build: pool flags as one chained assignment
// 5 (0x3d stored first); the helper's result declared before its start read 3 (0x18f260 75,
// 0x6a8a0 12); start declared then assigned 3; unsigned reader local 3; plain `inline` reader 3.
// @retail 0x199740
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size)
{
    bool result = false;
    s_loop_allocator *pool = (s_loop_allocator *)function_80e14(0x10000, 0, 0, 0, 90, 1.0f);
    if (pool)
    {
        function_18e250(pool, 0xffb0, "zlib pool", NULL);
        pool->field3c = true;
        pool->field3d = true;
        s_loop_allocator *allocator = pool;
        *decompressed_size = function_199560(buffer, size);
        if (function_2cb5b0(destination, decompressed_size, buffer + 4, size, loop_allocator_zalloc, loop_allocator_zfree, &allocator) == 0)
            result = true;
        function_12d520((long)pool);
    }
    return result;
}
