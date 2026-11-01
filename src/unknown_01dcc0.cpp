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

// score 21 (was 21): width stack/height ESI differ from ESI/EDI inputs; packed gate differs.
// width/height/both unit subtraction before shifts 25/28/30; native packed words 30;
// row-byte part 30; selected/masked new packed word 27/28; early dimensions 30.
// width/height input registers and byte-gate normalization differ.
// Byte storage 33; direct byte input 25; byte mask 35/27; byte selection 25/23; gated packing 30/24.
// width remains stacked and height/register saves differ; retail masks the packed fields.
// Direct width 24; byte pitch expression 24; field store schedule 22; zero flag branch 25; Boolean mask 25; flag result selection 25; complete height field 28; packed role names 24; direct input and mask 22.
// Width remains on the stack and height in saved ESI instead of retail ESI/EDI
// inputs; the unsigned packing and Boolean gate still lower differently; height expression 21;
// width expression 21; packed expression 21; direct width height expression 24; direct packed
// expression 24; late entry 21; shared publication 21; separate gated result 21.
// Selecting the size by the flag before the width/height stores, with no
// width reference, gives retail's ESI/EDI inputs, ret 0 and retail's tail, but the compiler
// folds the shifts ((w*4/64)<<24 to (w<<20)&mask, (h<<12)-0x1000 to (h+0xfffff)<<12) where
// retail keeps shr/shl/sub: (p-1)<<24 form 28; shift-then-subtract 24; reversed or order 28;
// pitch local 28; sums 29; multiply forms 22-30; signed forms 27-28; size helper 39; size
// helper without global optimisation (diagnostic) 24. Only builds without global
// optimisation keep retail's shr/shl/sub.
// Width/height input registers and flag normalization still differ. Native
// index/width/height parameter-type sets 24 in all seven forms; native contract with separate
// noinline declaration 24, all at 79 bytes.
// The XDK D3DSIZE_* shifts with `flag94 ? size : 0` 28 (shifts merged and Horner-factored);
// `(p * 0x1000000 - 0x1000000) | (h * 0x1000 - 0x1000)` with the ternary after the stores 22
// (79 bytes, ret 0, retail's neg/sbb/and select), before the stores 22 (retail's ESI/EDI
// inputs; index EAX/size ECX where retail has ECX/EAX, and the -1 folded into the shifts),
// with the size computed before the entry pointer 22; a forced-inline size helper 27; favour
// size (diagnostic) 21. A compile-only build folds the -1 inside every shift/multiply spelling.
// Round 60 (no width reference, all 79 bytes and ret 0 unless noted): the one-expression multiply
// packing with a select after the stores 22 (index in EAX); the packed word as one shift
// expression with a named select 23; 0x1df20's three-statement packing with the select stored
// directly and copied to the element 23 (width EDX, height ESI); that with an if/else store 40
// (91 bytes); packing from the stored width/height fields 25. The shifts fold in every form.
// Round 44 (lane 14): no width reference, with `value = e->flag94 ? value : 0;` before the width/height
// stores, gives retail's exact convention (index ECX, width ESI, height EDI, plain ret) 24: only the
// folded shifts differ. Callers then: 0x48ee0 226 (189 with its width rounded first and a long mode),
// 0x2d140 637 -> 686 at retail's 2652 bytes (aligned 593 -> 543), 0x335e0 42. Not kept (score rises).
// @retail 0x1ddd0
void function_01ddd0(long index, dword width, dword height)
{
	s_unknown_01dcc0 *e = &g_4b4b58[index];
	dword const *width_reference = &width;
	dword x = ((*width_reference * 4) / 64) << 24;
	dword y = height << 12;
	dword value = (x - 0x1000000) | (y - 0x1000) | (width - 1);
	e->width = width;
	e->height = height;
	value *= (dword)(bool)e->flag94;
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

