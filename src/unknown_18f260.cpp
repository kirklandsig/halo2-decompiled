// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_223b60.h"
#include "physical_memory.h"
#include "unknown_080e10.h"

void function_12d520(long a);

struct s_key
{
	long a;
	long b;
};

class c_resource_lock
{
public:
	virtual long method0(long handle);
	virtual void method1(long block);

	s_key key;
};

// score 4 (was 4): retail saves edi before it stores the call's arguments,
// ours stores the handle first and saves edi after it, so the two instructions
// and the offsets that follow them differ. _ReadWriteBarrier (as 0x8c790 has it):
// before the return 9 (key loads after the pushes); key a, b locals then barrier 6;
// key b, a locals then barrier 4 (retail's stores, but xor esi after push edi).
// Round 21 (m3 lane 2): a barrier at the top of the shared helper 4 (xor esi after the key
// stores; 0x6a8a0 3, 0x199740 4); an s_key local copy 8; the reader's local as volatile
// ULARGE_INTEGER halves 4; the reader plain inline 4; (diagnostic) a non-volatile reader
// local named in the asm block 49 (retail's order, but an extra lea and a 0x1c frame).
// Compile-only screens: every volatile spelling hoists the first zero store into the prologue.
// @retail 0x18f260
long c_resource_lock::method0(long handle)
{
	return function_80e14(handle, 0, this->key.a, this->key.b, 30, 0.1f);
}

// @retail 0x18f350
void c_resource_lock::method1(long block)
{
	function_12d520(block);
}