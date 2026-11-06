// @flags /O2 /Ob1 /Gr
#include "launch_stack_storage.h"
#include "unknown_11c920.h"
#include <xtl.h>

/* 0x98-byte entries starting at 0x4b4b58 */
struct s_unknown_01dcc0
{
	byte unknown00[4];
	long sub_header[5];                         /* +0x04 */
	long elements[4][6];                        /* +0x18, 24-byte elements */
	long element_count;                         /* +0x78 */
	byte unknown7c[4];
	long width;                                 /* +0x80 */
	long height;                                /* +0x84 */
	void *data;                                 /* +0x88 */
	byte unknown8c[8];
	bool flag94;
	byte flag95;
	byte unknown96[2];
};

s_unknown_01dcc0 g_4b4b58[39];

void function_12d520(long address);

bool function_1df20(long index, long kind, long width, long height, bool alternate, bool linear, long count, void *data);

// @retail 0x1db10
bool function_1db10(long index, long width, long height, long unused, bool alternate)
{
	(void)&index;
	(void)&width;
	(void)&alternate;
	long const *unused_reference = &unused;
	long ignored = *unused_reference;
	(void)ignored;
	long allocator_index = g_4e6420;
	long allocation_top = g_4e6440[allocator_index];
	long size = ((((width + 63) & ~63) * height * 4) + 4095) & ~4095;
	long address = allocation_top - size;
	void *data = 0;
	if (address >= g_4e642c[allocator_index])
	{
		g_4e6440[allocator_index] = address;
		data = (void *)address;
		if (data)
		{
			data = (void *)(address | 0x80000000);
			if (data)
				XPhysicalProtect(data, size, PAGE_READWRITE | PAGE_WRITECOMBINE);
		}
	}
	return function_1df20(index, 1, width, height, alternate, true, 1, data);
}

// @retail 0x1db90
void __stdcall function_1db90(void *unused, long index)
{
    (void)&unused;
    (void)&index;
    s_unknown_01dcc0 *entry = &g_4b4b58[index];
    if (entry->data)
    {
        index = 0;
        bool busy;
        do
        {
            busy = D3DResource_IsBusy((D3DResource *)&entry->sub_header) != 0;
            if (!busy)
            {
                for (long i = 0; i < entry->element_count; ++i)
                {
                    if (D3DResource_IsBusy((D3DResource *)&entry->elements[i]))
                    {
                        busy = true;
                        break;
                    }
                }
            }
            ++index;
        } while (index <= 100000 && busy);
        function_12d520((long)entry->data);
        entry->width = 0;
        entry->height = 0;
        entry->data = 0;
        entry->flag95 = 0;
        *(long *)entry->unknown00 = 0;
    }
}

// @retail 0x25960
long function_25960(void)
{
	long result = NONE;
	if (g_4b4b58[20].data && !g_4b4b58[20].flag95)
		return 20;
	if (g_4b4b58[18].data && !g_4b4b58[18].flag95)
		result = 18;
	return result;
}

// @retail 0x1dcc0
void *function_01dcc0(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95)
	{
		result = &e->sub_header;
	}
	return result;
}

// @retail 0x1dcf0
void *function_01dcf0(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95 && e->element_count > 0)
	{
		result = &e->elements;
	}
	return result;
}

// @retail 0x1dd20
void *function_01dd20(long index, long element)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	void *result = 0;
	if (e->data != 0 && !e->flag95 && element >= 0 && element < e->element_count)
	{
		result = &e->elements[element];
	}
	return result;
}

// @retail 0x1dd60
bool function_01dd60(long index, long *width, long *height)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	bool result;
	if (e->data != 0 && !e->flag95)
	{
		*width = e->width;
		*height = e->height;
		result = true;
	}
	else if (index == 0 || index == 3 || index == 0x18)
	{
		*width = 0x280;
		*height = 0x1e0;
		result = true;
	}
	else
	{
		*width = 0;
		*height = 0;
		result = false;
	}
	return result;
}

// @retail 0x1ddd0
void function_01ddd0(long index, dword width, dword height)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	long pitch = width * sizeof(dword) / 64;
	long value = ((pitch - 1) << 24) | ((height - 1) << 12) | (width - 1);
	value = e->flag94 ? value : 0;
	e->width = width;
	e->height = height;
	e->sub_header[4] = value;
	e->elements[0][4] = value;
}

// @retail 0x1de20
bool function_01de20(long index)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	bool result = false;
	if (e->data != 0 && !e->flag95)
	{
		result = true;
	}
	return result;
}


dword function_1d6f0(bool linear, bool alternate, dword format, long width, long height);
long g_55e6c0;

// score 29 (was 29): frame, incoming data in EBP, delayed saves and the level loop agree; retail
// keeps the size packing unfolded (lea/shr 6/shl 24/sub, shl 12/sub) where /Og folds it
// ((w<<20)&mask, (h+0xfffff)<<12), and height/count then take each other's register.
// Kept: the offset declared after the kind store (data stays in EBP) 107; one result variable
// with the busy entry inside an if/else 40; the level headers written in full 33/29.
// Tried (all reverted): pitch multiply 28; pitch and height multiplies 28; height multiply 29;
// minus-one multiplies 97; negative addends 28; clamp as an if/else 168; packing or call from
// the stored fields 29/29; offset first in the block 29; one packing expression 29; explicit
// shift 121; named level size 120; offset before the entry 109; whole function without global
// optimisation (diagnostic) 336; nested result 136; absent-data guard 156; common result exit
// 131; level-index shift 122; role-named workspaces 136; shared primary header 161; walking
// level pointer 135; Boolean packed-word product 152; byte count in the level loop 115; delayed
// clamped-count publication 148; inline/noinline format declarations 115/115; published byte
// total 192; level/offset names 115; size after format 190; named aligned width 115; element
// before size 123; linear size branches 174; data after/before primary format 155/155; primary
// common after format 161; data/common after format 142; primary format result 115; level
// parent/count before format 121/131; unsigned offset/level/both 115/116/116; const byte
// count/output pointer 115/115; paired/all reset chains 117/118; native parameter and field
// types 115-151; clamped count before kind/after kind/between dimensions 103/103/99; early
// resource resets 38; data before dimensions 31; height before width 30; early linear
// flag/common word 44/31; every packing accumulator order and complete OR expression 29;
// named byte pitch 87; all packing parts early 47; unsigned-int packing 29; shared clamped
// count 154; selected packed result 119; linear-only packing 145. A compile-only screen folds
// every shape of the packing (casts, 64-bit, helpers, multiplies) under /Og.
// The D3D-style ((w*4/64 - 1) << 24) | ((h - 1) << 12) | (w - 1) keeps the pitch shr but factors
// the shifts: 97 as one expression, three statements, height first or a dword width; minus-one
// multiplies 97 again; plain multiplies with separate subtractions 28 (subtraction folded in);
// the width term between or before the shift terms 97 (the ORs are reassociated).
// Round 183 (lane 3, fast): element count stored before the data pointer 41; kind stored after the
// dimensions 29 (same code); height stored before width with kind first 30; the lock word cleared after
// the element count 33; the clamp testing the upper bound first 151 (495 bytes). The loop and tail
// match; the differing call and global lines are positional knock-ons of the folded packing block.
// Round 93: destination-field accumulator 153; typed resource pointer 167; size macros 29;
// parameter-address reads 29; unsigned packing dimension copies 29; direct typed Size publication 29;
// signed packed carrier 29; named height part 29. Original packing restored.
// Round 72 (lane 9, fast): packing after the linear-flag store 44; pitch as `width * sizeof(dword) / 64` 29;
// the height part in its own local ahead of the pitch 29; the whole packing inside the linear select 175
// (451 bytes, 0x1dc40 10); pitch as `(width * 4 >> 6) * 0x1000000` 28 (484 bytes). The fold stays in all.
// Round 31 (lane 17, fast unless noted): the packing through a forceinline helper taking the bytes per
// pixel (4 passed) 29; the same helper as a plain function in the format helper's file, inlined by
// LTCG (plain) 29, and without global optimisation there (diagnostic: not inlined) 164; the shift
// terms computed before the clamp and the subtractions after it 68 (484 bytes); the width term
// unsigned 29; the pitch units truncated to a byte local 142 (495 bytes; it keeps retail's
// `lea [w*4]; shr 6` unfolded, with a movzx and the -1 moved ahead of the shl 0x18).
// Round 133: native typed Size with subtraction-before-shift in the select 170; computed before
// the typed select 97; raw level-count local at the clamp/at entry 29/29; multi-use height/width/
// both dimension locals 29/29/29; native pitch only 31; native height only 96. The typed native
// form unfolds its shifts but factors the two 12-bit shifts; original packing and body restored.
// Round 51 (fast): LEAD: signed locals stop VC pulling the subtraction under the shift. `long pitch = width * sizeof(dword) / 64; long packed = ((pitch - 1) << 24) |
// ((height - 1) << 12) | (width - 1);` is 29 (484 bytes) with every packing instruction retail's (lea/shr 6/shl 24/sub, shl 12/sub); what is left is height in EDX and the clamped
// count in EAX where retail has them the other way (the extra byte is `sub edx, imm`), and so the top block's load order. The same code: shift-then-subtract statements, own height
// local, accumulated pitch, pitch decremented first, the cast `(long)(...) << 24` 29; multiply forms unsigned 28 (484), `dword packed` with a signed height 29 (483, folded); a
// signed pitch as a plain shift of the dword quotient 29 (483, folded). Register swap, on the natural form: clamped local published after the packing, after the flag, after the
// select's constant 29 each; arms assigning from count 29; nested else 29; zero first 43; store after the clears 35; declared at the top 29; packed declared at the top 29; height
// stored before width 30; pitch before the stores 155 (495 bytes); stored fields 29; height part before the clamp 37; packing before the clamp 31 (483: height EAX, zero EDX); copy
// of count before the stores 166 (503), at the clamp 166 (497); forced-inline pin as a ternary or with returns 29, with a result local 166; forced-inline size helper 29; format local
// for the call 29; `count >= 0 ?` first 32. Field stores in the clamp's arms 139 (495) give retail's top block and registers exactly, so the merged clamp value is what takes EAX.
// Round 216 (m3 lane 3, fast): the entry's +0x00/+0x7c/+0x8c typed as long fields (no `*(long *)` casts in the file)
// 29 (same code; every function of the file still matches); that with the round-51 signed pitch/packed lead 29 (484
// bytes, packing instructions retail's); on it the height term first in the OR 29 and as its own first statement 29
// (VC canonicalises both: pitch in ECX first, height in EDX); a named clamped local set by if/else-if 166 (497 bytes).
// Retail loads the count first (EDX) for the clamp compare, hoisted above the kind store, and the height after it (EAX).
// @retail 0x1df20
bool function_1df20(long index, long kind, long width, long height, bool alternate, bool linear, long count, void *data)
{
    bool result = true;
    if (data)
    {
        s_unknown_01dcc0 *entry = &g_4b4b58[index];
        if (entry->data || entry->flag95)
            result = false;
        else
        {
            *(long *)entry->unknown00 = kind;
            long offset = 0;
            entry->width = width;
            entry->height = height;
            entry->data = data;
            *(long *)entry->unknown8c = 0;
            entry->element_count = count < 0 ? 0 : count > 4 ? 4 : count;
            *(long *)entry->unknown7c = 0;
            entry->sub_header[1] = 0;
            entry->sub_header[2] = 0;
            dword packed = ((((dword)width * 4) / 64) << 24) - 0x1000000;
            packed |= ((dword)height << 12) - 0x1000;
            packed |= width - 1;
            entry->flag94 = linear;
            entry->sub_header[0] = 0x40001;
            entry->sub_header[4] = linear ? packed : 0;
            entry->sub_header[3] = function_1d6f0(linear, alternate, count, width, height);
            D3DResource_Register((D3DResource *)entry->sub_header, data);
            for (long i = 0; i < entry->element_count; ++i)
            {
                long bytes = (((width + 63) & ~63) * height * 4) >> (i * 2);
                entry->elements[i][0] = 0x50001;
                entry->elements[i][1] = entry->sub_header[1] + offset;
                entry->elements[i][2] = entry->sub_header[2];
                entry->elements[i][4] = entry->sub_header[4];
                entry->elements[i][3] = function_1d6f0(linear, alternate, 1, width >> i, height >> i);
                entry->elements[i][5] = (long)entry->sub_header;
                ++entry->sub_header[0];
                offset += bytes;
                *(long *)entry->unknown7c += bytes;
            }
        }
    }
    else if (g_55e6c0 < 10)
        ++g_55e6c0;
    return result;
}

extern long g_485898;
long function_12d400(long type, long size, long user_data, long update, long release);

// @retail 0x1dc40
bool function_1dc40(long index, long width, long height)
{
    (void)&width;
    (void)&height;
    long size = ((width + 63) & ~63) * height * 4;
    long active = g_485898 >= 4 && g_485898 <= 7;
    void *data = (void *)function_12d400((long)((byte)active != 0) + 1, size, index, 0, (long)function_1db90);
    if (data)
        XPhysicalProtect(data, size, PAGE_READWRITE | PAGE_WRITECOMBINE);
    return function_1df20(index, 3, width, height, false, true, 1, data);
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
extern long g_4b9970[12];

// @retail 0x1de50
void function_1de50(void)
{
	for (long index = 0; index < 39; ++index)
	{
		s_unknown_01dcc0 *entry = &g_4b4b58[index];
		if (entry->flag95)
		{
			if (!entry->data || D3DResource_IsBusy((D3DResource *)entry->sub_header))
				continue;
			long i;
			for (i = 0; i < entry->element_count; ++i)
			{
				if (D3DResource_IsBusy((D3DResource *)entry->elements[i]))
					goto entry_in_use;
			}
			function_12d520((long)entry->data);
		}
		else if (entry->data)
			continue;
		entry->width = 0;
		entry->height = 0;
		entry->data = 0;
		entry->flag95 = 0;
		*(long *)entry->unknown00 = 0;
	entry_in_use:;
	}
	bool needed = false;
	if (g_510c50 && ((byte *)g_510c50)[5])
		needed = true;
	else if (g_4b9970[0] != NONE && g_4b9970[0] != 0)
		needed = true;
	if (!needed)
	{
		if (g_4b4b58[18].data && !g_4b4b58[18].flag95)
			g_4b4b58[18].flag95 = 1;
	}
	else if (!g_4b4b58[18].data)
		function_1dc40(18, 640, 480);
}

PRIVATE __forceinline void *allocate_entry_bytes(long index, long size)
{
    void *result = (byte *)g_4b4b58[index].data + *(long *)g_4b4b58[index].unknown8c;
    *(long *)g_4b4b58[index].unknown8c += size;
    return result;
}

/* Entry 9's allocations start again twice below: each restart hands out
   the entry's data itself and sets the offset past that first block. */
// @retail 0x1d770
bool function_1d770()
{
    bool success;
    void *data;
    success = (function_1db10(1, 640, 480, 1, false)) &&
        (function_1df20(5, 2, 320, 240, false, true, 1, allocate_entry_bytes(1, 307200))) &&
        (function_1df20(6, 2, 320, 240, false, true, 1, allocate_entry_bytes(1, 307200))) &&
        (function_1db10(9, 512, 512, 1, true)) &&
        (function_1df20(10, 2, 128, 128, false, true, 1, allocate_entry_bytes(9, 65536))) &&
        (function_1df20(11, 2, 128, 128, false, true, 1, allocate_entry_bytes(9, 65536))) &&
        (function_1df20(12, 2, 128, 128, false, true, 1, allocate_entry_bytes(9, 65536))) &&
        (*(long *)g_4b4b58[9].unknown8c = 92160, function_1df20(7, 2, 160, 120, false, true, 1, g_4b4b58[9].data)) &&
        (function_1df20(8, 2, 160, 120, false, true, 1, allocate_entry_bytes(9, 92160))) &&
        (function_1df20(19, 2, 320, 240, false, true, 1, allocate_entry_bytes(9, 307200))) &&
        (function_1df20(13, 2, 256, 256, false, false, 3, g_4b4b58[9].data)) &&
        (*(long *)g_4b4b58[9].unknown8c = 524288, function_1df20(21, 2, 512, 256, false, false, 1, g_4b4b58[9].data)) &&
        (function_1df20(22, 2, 512, 256, false, false, 1, allocate_entry_bytes(9, 524288)));
    *(long *)g_4b4b58[9].unknown8c = 0;
    if (!success) goto failure;
    data = allocate_entry_bytes(9, 307200);
    if (!function_1df20(25, 2, 320, 240, false, true, 1, data)) goto failure;
    data = allocate_entry_bytes(9, 307200);
    if (!function_1df20(26, 2, 320, 240, false, true, 1, data)) goto failure;
    data = allocate_entry_bytes(9, 307200);
    if (!function_1df20(27, 2, 320, 240, false, true, 1, data)) goto failure;
    if (!function_1df20(23, 2, 320, 240, false, true, 1, g_4b4b58[1].data)) goto failure;
    if (function_1db10(15, 64, 64, 0, false))
        return true;
failure:
    return false;
}

