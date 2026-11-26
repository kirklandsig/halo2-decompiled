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

// score 29 (was 29): r131, r232 and r234 unchanged. Frame, incoming data in EBP, delayed saves and the level loop agree; retail
// r133 unchanged. Frame, incoming data in EBP, delayed saves and the level loop agree; retail
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
// Round 142 (m3 lane 8, fast, all on the round-51 signed pitch/packed lead, itself re-measured 29 at 484 bytes): zero arm
// as its own store, upper clamp a ternary in the else arm 33 (485); clamp through a named level-count copy 29; busy test
// negated with the work arm first 29; width term first in the OR 29; one merged count zeroed, set and capped under
// `count >= 0` 43.
// Round 177 (m3 lane 4, fast, on the round-51 signed pitch/packed lead, 29 at 484 bytes): retail's top block is a
// source-order allocation (kind EAX, height EAX after the kind store and kept for the packing, clamp value EDX, the
// count's load and compare then scheduled above the kind store); ours gives the clamp value EAX before the height is
// placed. New: the clamp as `(4 < (0 > count ? 0 : count)) ? 4 : (0 > count ? 0 : count)` 150 (495 bytes, setl form); a
// two-stage clamp through a level-count local, `count > 0 ? count : 0` then `< 4 ?`, or `count < 0 ? 0 : count` then
// `> 4 ?` 150/150; a level-count local read right after the kind store and clamped by if / else-if after the other
// stores 166 (497: the count takes EBP, the data EBX); the clamp ternary into a local ahead of the kind store 140 (483:
// the block splits at the clamp). Re-measured: the level-count local set by if / else-if at the store 166 (497).
// Diagnostic: the packing without its height term 131 (479), the height still in EDX, so the clamp value holds EAX
// whether or not the height outlives its store. A 600 s permuter run on the lead (57 tries) found none better.
// Round 95 (m3 lane 10, fast, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the clamp
// through a named copy and two independent ifs 166 (497 bytes); the element count stored between the height and
// data stores 41; the resource's first word stored ahead of the packing 31; the linear select as an if/else 148 (471);
// the busy test as two separate else-if arms 137 (476). Diagnostic, not retail's code: a constant third argument to
// 0x1d6f0 176 (503: LTCG then passes the count in EAX, held in EBX, and the index in ECX, so the call's raw count use sets the convention).
// Round 115 (m3 lane 9, fast, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): reading: with no
// merged clamp value (the three field stores in the arms, re-measured 139 at 495 bytes) the height is placed first
// (EAX, loaded after the kind store) and the count takes EDX with its compare at the block's top, as retail; with a
// merged value, that value is placed ahead of the height and takes EAX. An uninitialised level-count local set in three
// if / else-if / else arms and stored after them 29 (484, same code as the ternary); a level-count copy of the count
// clamped in place by if / else-if 166 (497: the count takes EBP), the copy read right after the kind store 166, its
// store written after the packing 166. Diagnostics, not retail's code: one or two more uses of the height in the top
// block 165/163 (495: the height then takes a callee-saved register).
// m3 lane 14 r90 (fast with 0x1dc40, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the entry
// pointer `*const` 29, pitch and packed `long const` 29, the pitch as `(width * sizeof(dword)) >> 6` 29 (all three the
// lead's code); the offset declared after the width and height stores 31; the offset declared just before the level
// loop 147 (493 bytes). Retail loads the count and hoists its compare above the kind store, then loads the height into
// EAX after it; ours loads the height (EDX) first and the count into EAX after the kind store.
// Round 105 (m3 lane 10, fast, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the count
// parameter renamed to sort after the height 29 (same code: the swap is not a name tie); the call's count read through
// `*(long const volatile *)&count` 152 (487 bytes: the count goes to EBX at the top); the height stored after the data
// pointer 30; the three zero stores (+0x7c, +8, +0xc) ahead of the clamp 35 (the swap stays); the upper bound as its own
// store with the lower clamp a ternary in the else arm 152 (495 bytes, unmerged).
// Round 125 (m3 lane 9, fast, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the clamp's upper
// test as `count < 4 ? count : 4` 30 (jl); the clamp in a `long const` local at the store 29; `(void)&count` 29;
// `(void)&height` 29 (the swap stays in all three); the busy test negated with the work arm first and `result = false`
// in the else 29. Reading: the schedule follows the allocation (the value given EDX is loaded above the kind store,
// the one given EAX after it), so the open cause is only which of the merged clamp value and the height the allocator
// colours first.
// M3 lane 9 r127 (fast, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the clamp's store right
// after the kind store 156 (493 bytes); the clamp through a levels copy with the upper test nested in the else 29 (same
// code); the clamp through a plain inline multi-return helper 29; the call's level count through a copy declared just
// before the call 29 (propagated); the clamp's store just before the linear select's store 70. Diagnostic, not retail's
// code: the 0x1d6f0 call reading `entry->element_count` instead of the raw count 130 (485 bytes) puts the height in EAX
// after the kind store as retail does (the clamp then takes EBP), so the call's use of the raw count is what ranks the
// merged clamp value ahead of the height.
// Round 111 (m3 lane 10, fast with 0x1dc40/0x1db10, all on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes):
// the packing as one expression with no pitch local 29 (same code); a level count defaulted to the count and capped by if /
// else-if tests on the count 166 (497 bytes); the clamp written from the non-negative side, `count >= 0 ? (count <= 4 ? count :
// 4) : 0`, 32; the data pointer stored ahead of the width and height 31; the height stored ahead of the kind 31.
// Round 189 (m3 lane 5, fast with 0x1dc40): `long offset = 0;` declared at function scope 36; the count clamped in place by
// if / else-if with the raw request kept in a named local for the format call 69 (489 bytes, 0x1dc40 9); the three zero
// stores as one chained assignment (+0x7c, +8, +0xc) 29 (same code); the entry pointer computed at function scope 36; the
// round-51 signed pitch/packed lead with the field stored in each clamp arm 139 (495 bytes: retail's packing and top
// registers, three stores). The existing PIN_126660 macro is this ternary (same expansion). All restored.
// Round 93 (m3 lane 2, fast with 0x1dc40, on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): a
// copy of the count read right after the kind store and clamped by the ternary at the store 29 (same code: the copy is
// propagated); width and height stored through a force-inline setter 29 (same code); the count clamped in place inside
// a force-inline setter that stores it 166 (497 bytes, the listed if / else-if shape); the clamp's store written right
// after the packing 32 (the height then takes EAX as retail, but the clamp branch follows the zero stores and the
// packing is scheduled above the dimension stores). All restored.
// Round 106 (m3 lane 14, fast with 0x1dc40/0x1db10, all on the round-51 signed pitch/packed lead, re-measured 29 at 484
// bytes): a redundant `0 < element_count` guard around the level loop 36 (the top swap stays, the loop's offset store
// moves); the kind stored through a trivial force-inline setter 29 (same code); the packing's height term read from the
// stored height field 29 (same code: the store is forwarded); the clamp assigned through a reference bound to the
// element-count field 30; the clamp through a reference-bound copy of the count, set by if / else-if, 166 (497 bytes).
// Round 210 (m3 lane 4, fast, all on the round-51 signed pitch/packed lead, re-measured 29 at 484 bytes): the clamp's
// store between the width and height stores 161 (493 bytes); the count stored, then overwritten by `< 0` / `> 4` stores
// 109 (495: the count still takes EAX and the height EDX with no merged value); the clamp as the runtime library's
// min(max(count, 0), 4) expansion 150 (495), as `MIN(MAX(count, 0), 4)` with `>` in both 150; width and height stored
// ahead of the kind store 31, the kind store between them 31 (kind is still loaded and stored first). Diagnostic,
// reverted: the clamp reading its own volatile copy of the count 46 (the count still takes EAX, so the call's raw-count
// use does not give it EAX).
// Round 212 (m3 lane 4, fast with 0x1dc40/0x1db10, both matched in each): the clamp reading the count through a `long const &`
// bound to the parameter 29; through a trivial forceinline accessor returning the count 29 (both the same code); the clamp
// as `count < 0 ? 0 : count` stored, then `if (field > 4) field = 4;` 156 (495 bytes); as an unsigned range test
// `(dword)count > 4 ? (count < 0 ? 0 : 4) : count` 140; the height bound by `long const &` for its store and the packing
// 29 (same code). All restored.
// Round 185 (m3 lane 8, fast with 0x1dc40/0x1db10, all on the round-51 signed pitch/packed lead, 29 at 484 bytes):
// the clamp through a forceinline helper that clamps its own parameter by the ternary and returns it 166 (497 bytes,
// the count takes EBP); the clamp's store ahead of the +0x8c clear 33; the clamp assigned through a reference-bound
// local then stored 29 (same code); a copy of the count made ahead of the kind store and clamped at the store 29
// (propagated); that copy clamped in place by the ternary, then stored, 166 (503 bytes; 0x1dc40 and 0x1db10 go to 5).
// Reading: retail's top block fits the clamp value interfering with the kind value (count loaded before the kind
// store, so it cannot share EAX) while the height starts after it; ours starts the clamp after the kind store.
// M3 lane 1 r121 (fast with the file's functions and 0x1d6f0): the clamp's store after the 0x1d6f0 call 179 (453 bytes;
// 0x1db10/0x1dc40 10, 0x1d770 213), after the register call 141 (479 bytes); element_count typed dword 31 (four of
// the file's matched functions differ); the round-51 lead with the clamp's store after the call 178. LEAD, on the
// round-51 signed pitch/packed lead (re-measured 29 at 484 bytes): the select store (`sub_header[4] = linear ? packed :
// 0`) written ahead of the header-word store (`sub_header[0] = 0x40001`) 38 at 483 bytes, first difference +0x85: the
// whole top block is then retail's (count loaded into EDX and compared above the kind store, height into EAX after it).
// The three stores are emitted in retail's order whatever their source order; any order with the select ahead of the
// header word gives this (flag/select/header, select/flag/header, select/header/flag 38 each; header/flag/select and
// header/select/flag 31). Left on it: the clamp's store is scheduled ahead of the packing (retail after it), so the
// height term is built in a copy (`mov edx, eax`) and the height stays in EAX for the call's push (retail reloads it
// from the stack), the select then takes EBX, the header word is stored straight through ESI (retail through EBX, the
// header address formed first), and the loop's ECX/EDX rotate. On that base: the call's height (or width and height)
// read from the stored fields 38 (same code); the height term first in the OR 38, as its own first statement 38 (same
// code); a clamped `levels` local published after the packing 41, after the flag 60, after the select 142, after the
// header word 142; the clamp's store after the packing 141; the two header zero stores after the header word 133, ahead
// of it 133; six random orders of the seven statements from the +0x7c clear to the select 49-143; a named header
// pointer used for every header access (main's order or the lead's) 162 (471 bytes); the call's height read once
// through the parameter's address, on the select-first lead 38, on the plain lead 29 (both the same code: the address folds).
// M3 lane 10 r122 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH each time). LEAD: retail's header word
// is stored through a register holding the header address (`lea ebx, [esi + 4]` ahead of `mov [ebx], 0x40001`), so the
// header is a named pointer there. On the round-51 signed pitch/packed lead: `long *header = entry->sub_header;` used for the
// header word and the register call 28 (484 bytes: from the packing on, the select in EAX, the header in EBX, the call's
// height reloaded from the stack, all retail's); with the clamp in a `long levels` local stored after the packing as well 29
// (484: everything from the clamp's store on is retail's; only the top block's height/count swap is left). Diagnostic, not
// retail's code: on that levels form one more read of `height` anywhere before the packing (a byte store of it) gives
// retail's whole listing apart from that store (the height then outranks the merged clamp for EAX); after the call it
// does not. So retail's source reads the height once more than ours ahead of the packing. On the levels form: the header
// pointer also for the loop's element pointer and increment 36 (the loop's +0x7c load then waits for the increment's
// store), with the select ahead of the header word 45; the pointer for the header word only 31; the select ahead of the
// header word 38; levels declared at the block top 29; the clamp as `count <= 4 ? count : 4` 29; the packing's height
// from the stored field 29; the packing through a forceinline size helper taking width and height 29; the call's height
// from the stored field 106 (495 bytes: a real load). On the in-place clamp with the header pointer: the height term first
// 28; `count >= 0 ? (count > 4 ? 4 : count) : 0` 31; the clamp as three field stores 139 (495). The header word through
// a cast of the header array to the platform's resource type 38/41 (same code as the leads).
// M3 lane 8 r193 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH each time; base: the round-51 lead
// with the header pointer for the header word and the register call and the clamp in a `long levels` local stored after
// the packing, header word ahead of the select, re-measured 29 at 484 bytes with only the top block's height/count swap
// left; with the select ahead of the header word it is 38). On that base: a trivial forceinline identity on the kind,
// the width, the height or the data store 29 each (same code: the scratch choice is no temporary-creation rotation);
// `__assume(height > 0)` ahead of the packing (diagnostic) 29 (same code); the clamp as `levels = count;` then if / else-if
// on `levels` 166 (497 bytes), on `count` 166 (497 bytes); `levels = count < 0 ? 0 : count;` then `if (levels > 4) levels = 4;`
// 141 (485 bytes).
// M3 lane 10 r124 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20; all on r122's lead: the signed pitch/packed
// packing, the clamp in a `long levels` local stored after the packing, `long *header` for the header word and the register
// call, re-measured 29 at 484 bytes, only the top block's height/count swap left): the packing through a forceinline helper
// with dword parameters (0x1ddd0's types) 29; the select through the packed local (`packed = linear ? packed : 0;`, 0x1ddd0's
// idiom) 29, that select ahead of the levels store 29; the packed local renamed 29 (all four the same code); the data pointer
// stored first 31; the height stored after the data pointer 30, after the +0x8c clear 31; the offset declared after the
// dimension stores 31; the kind stored after the width 31; the select from the stored flag field 146 (495 bytes), the
// call's linear argument from it 163, both 143; the call's dimensions from the stored fields 169 (503 bytes, 0x1db10,
// 0x1dc40 and 0x1d770 change too). A 1200 s permuter run on that lead (118 tries) found none below it.
// Also on that lead: a trivial forceinline identity accessor for the height store, the kind store, the width store,
// the data store or the offset's zero 29 each (same code: no scratch-rotation shift).
// M3 lane 12 r151 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH each time). r122's lead (signed
// pitch/packed, the clamp in a `long levels` local stored after the packing, `long *header` for the header word and the
// register call) re-measured 31 at 484 bytes (the top block's height/count swap, and the pitch part now scheduled ahead
// of the height part). On it: the packing computed ahead of the clamp 33 (483 bytes: the zero moves to EDX); the header
// pointer declared at the block top 42; `levels` typed `dword` 31 (same code); the clamp stored in place (main's
// position) with the signed packing and the header pointer 30. On main's body: the clamp as `count > 0 ? (count > 4 ? 4 :
// count) : 0` 32.
// M3 lane 13 r122 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH each time; base: the round-51
// signed pitch/packed lead, the clamp in a `long levels` local stored after the packing and `long *header` for the header
// word and the register call, measured 31 at 484 bytes here with only the top block's height/count swap and the header
// lea's place left): the levels store after the flag store 31 (same code), after the select store 134 (496 bytes: levels
// goes to the count's stack home and the height then takes EAX), after the register call 115 (495 bytes, the same spill).
// Diagnostics, not keepable: 1-8 dead initialised locals at the top 31 each (flat: no symbol-count state); a byte store
// of the count ahead of the height store, ahead of the kind store or after the +0x8c clear 166 each (495 bytes: the count
// then takes EDX at the top and the height EBX). 0x1d6f0 reads all five of its parameters, so no dropped argument.
// Also on that base: the guard written `if (!entry->data && !entry->flag95) { ... } else result = false;` 31 and the loop
// index declared beside the offset 31 (both the base's code).
// M3 lane 6 r208 (fast with 0x1dc40/0x1db10, both MATCH; base: round 122's lead re-measured 31 at 484 bytes): a 16-form
// screen of the clamp's store (in place / after the packing / after the flag / after the select) x the select ahead of the
// header word x the header pointer: 29/30/38/39, 29/31/41/40, 29/31/60/40, 132/134/142/138; only select-first without the
// pointer gives retail's top block (38 at +0x85 in place, 41 at +0x7c with levels after the packing), where the height stays
// in EAX up to the call's push. On levels after the packing with the select first: the header pointer declared after the
// select for the header word 49; only for the register call 41; declared before the flag store for the register call only
// 29 (484 bytes: all but the top block's height/count swap retail's; the call then reloads the height and the swap returns),
// with the header word through it 38, with the select and the header word through it 31. On round 122's lead: the packing's
// height from the stored field 31, both dimensions from the stored fields 31, a const reference to the stored height 140.
// The per-level byte count named ahead of the loop 43, at the top 138/138. Diagnostics: a byte store of the height after its
// store or after the zero stores 142/133 (retail's top block); the clamp with only its lower or upper arm 102/138 (the
// upper-only clamp still takes EAX). A 900 s permuter run on the 29 header-pointer form: 96 tries, none below it.
// M3 lane 16 r151 (fast with 0x1dc40/0x1db10, both MATCH): round 122's lead re-measured 31 (484 bytes); on it the height
// store moved after the packing 159 (493 bytes); on main's body the same move 98 (480 bytes); the clamp's tests
// constant-left (`0 > count ? 0 : 4 < count ? 4 : count`) 29, only the upper one 29, only the lower one 29 (all the
// same code), the first spelling on round 122's lead 31.
// M3 lane 12 r157 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH; round 122's lead re-measured 31
// at 484 bytes): on that lead the height store moved right after the levels clamp 159 (493 bytes), after the three zero
// stores 159 (493 bytes); on main's body the height store right after the clamp's store 98 (480 bytes); the lead's whole
// else body in a forceinline helper taking the entry and every parameter 31 (same code); no entry local (every access
// through the global array) 144 (528 bytes; 0x1db10 4, 0x1dc40 1, 0x1d770 55).
// M3 lane 15 r118 (fast with 0x1dc40, MATCH each time; control 29): the entry bound as a reference (`entry.` for every
// access) 29 (same code); the clamp's lower test `count <= 0` 30; its upper test `count >= 4` 30; the no-data counter
// path written first under `if (!data)` with the work in the else arm 150 (487 bytes); round 122's lead (signed
// pitch/packed, the clamp in a `levels` local stored after the packing, the header pointer for the header word and the
// register call) rebuilt 29 at 484 bytes, with the entry bound as a reference 29 (484 bytes, same code).
// M3 lane 1 r137 (H2M_FAST with 0x1dc40/0x1db10, both MATCH; on the round-51 signed pitch/packed lead, re-measured 29 at
// 484 bytes with 0x1ddd0's `value = linear ? value : 0;` select): the clamp as a raw-count store then two independent
// `if (count < 0)` / `if (count > 4)` field overwrites 133 (485 bytes); the entry as a reference 29 (same code); the format
// call's linear argument from the stored flag field 29 (forwarded); the loop's per-level bytes from the stored width and
// height fields 152. Re-measured: round 122's header-pointer form 28 (484 bytes, a gain of one, not kept).
// M3 lane 6 r211 (fast with 0x1dc40/0x1db10, both MATCH; base: round 122's lead re-measured 31 at 484 bytes, the pitch part
// scheduled ahead of the height part): on it the clamp as `count < 0 ? 0 : count >= 4 ? 4 : count` 32; as `count <= 0 ? 0 :
// count >= 4 ? 4 : count` 33; as if / else-if / else with the `<= 0` and `>= 4` bounds 33; upper bound first (`count > 4 ? 4 :
// count < 0 ? 0 : count`) 153 (495 bytes); the level loop bounded by the levels local 158 (506 bytes: levels spills to the
// count's home and the height then takes EAX at the top as retail, so a longer clamp range is what lowers its rank).
// M3 lane 8 r211 (fast with 0x1dc40/0x1db10/0x1d6f0/0x1d770/0x1db90/0x1de20, all MATCH unless noted; round 122's lead
// re-measured 31 at 484 bytes). Diagnostics, restored at once: 0x1d6f0 given one more parameter that its body never reads
// (the link drops it after the caller's common-subexpression pass; this gave retail's load in 0x12e290), declared last
// and fed the height, on the lead 31, on this body 29, declared first on the lead 31 (all the same code: the argument
// re-reads the parameter's home, so it is no further use of the top block's value), fed `height >> i` in the loop as
// well 55; the call's height through `*(long const volatile *)&height` on the lead 31, with the select ahead of the
// header word 40, the select-first lead alone 40, on the select-first form without the header pointer 140 (487 bytes;
// 0x1db10 10, 0x1dc40 10, 0x1d770 213: the address-taken parameter moves the convention). Forms: the dimension and data
// stores, the +0x8c clear and the clamp in a new forceinline setup helper taking (entry, width, height, count, data),
// whose arguments would be evaluated count ahead of height, on this body 29, on the lead 31 (both the same code: an
// inlined call's arguments are propagated, so they do not order the temporaries).
// M3 lane 11 r140 (H2M_FAST with 0x1dc40/0x1db10, both MATCH; base: round 122's lead re-measured 31 at 484 bytes, the pitch
// part scheduled ahead of the height part): the levels value read through a `long const &` for its store 31; a second entry
// pointer local for the header part 31; the levels clamp, zero stores and packing in their own block scope 31; the packing's
// height through a `long const &` to the parameter 31 (all four the base's code); the header pointer declared ahead of the
// sub-header zero stores and used for them 157 (488 bytes), also for the select and the format store 162 (471 bytes). The
// packing ahead of the clamp re-measured 31 (483 bytes, in-place clamp), with the header pointer 33, with levels 33.
// M3 lane 12 r161 (fast with 0x1dc40/0x1db10, both MATCH; round 122's lead re-measured 31 at 484 bytes, the top block's
// height/count swap left). On that lead: the height stored right after the kind 33; `levels` declared bare at the block top
// and assigned at the clamp 31; `levels = 0; if (count >= 0) levels = count > 4 ? 4 : count;` 45; `levels = 4; if (count
// <= 4) levels = count < 0 ? 0 : count;` 137 (485 bytes); the kind stored after the +0x8c clear 31 (the kind store is
// still scheduled first); the kind stored after the levels clamp 43 (the kind store then follows the clamp, and the count
// still takes EAX). In none does the count take EDX at the top.
// M3 lane 14 r137 (H2M_FAST with 0x1dc40/0x1db10, both MATCH; base: round 122's lead, the signed pitch/packed packing with
// the clamp in a `long levels` local stored after the packing and `long *header` for the header word and the register call,
// re-measured 31 at 484 bytes): everything from the header zero stores to the end of the level loop in a forceinline helper
// (entry, width, height, alternate, linear, count, levels, data) with the offset declared inside 41 (the offset store moves),
// with the offset kept in the caller and passed by `long &` 31 (same code); `(void)&width;` 31; the count's sign test named
// ahead of the kind store (`bool negative = count < 0;`) 31 (the compare sinks to the branch); the format call's count as
// `(dword)count` 31 (all the lead's code). Re-measured: the round-121 select-first form 38 at 483 bytes (retail's top block,
// the height kept in EAX to the call's push and the select in EBX; in retail the select's EAX forces the height reload).
// M3 lane 2 r125 (H2M_FAST with 0x1dc40/0x1db10, both MATCH; round 122's lead re-measured 31 at 484 bytes, the pitch
// part scheduled ahead of the height part): a redundant `result = true;` at the top of the work arm 29 on this body, 31
// on the lead (both the same code); the busy tests through their own `const` entry pointer with the work arm's entry
// formed inside it 143 (484 bytes), on the lead 155 (495 bytes); `result` declared bare and set true in the work arm
// and the no-data arm 93 (first difference +0x1); the format call's result named and stored through a `dword const &`
// 90 (480 bytes); a redundant `offset = 0;` ahead of the level loop 110 (480 bytes), on the lead 146 (493 bytes).
// M3 lane 1 r146 (H2M_FAST, control 29): the count clamped into a `levels` local at function entry, ahead of the data
// test, and stored at the clamp's place 154 (487 bytes, first difference +0x1); one to four redundant parenthesis pairs in
// the clamp ternary 29 each (same code: integer registers ignore them); the per-level byte count declared bare beside the
// offset and assigned in the loop 29; the loop's level width and height named before the format call 29 (both the same
// code). Diagnostic, restored: the height stored twice (again after the +0x8c clear) 31: the first store is eliminated as
// dead and the height/count swap stays, so an eliminated read does not raise the height's rank. On round 122's lead
// (re-measured 31 at 484 bytes) the header address is now formed after the flag store (main's body forms it before).
// M3 lane 5 r228 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the clamp stored through a `long &` bound to the
// count field (`long &levels = entry->element_count; levels = ...`) 30, with the round-51 signed packing 30 (484 B); the
// round-51 packing with the width and height stores ahead of the kind store 31, that with round 122's late levels store and
// header pointer 33; round 122's lead re-measured 31 (484 B). The top block's height/count swap stays in all of them.
// M3 lane 3 r290 (H2M_FAST, control 29; re-measured: round 51's signed packing 29 at 484 B, round 122's lead 29 at 484 B,
// round 121's select-first lead 38 at 483 B with retail's whole top block). New: the total byte count named right after
// the height store and shifted in the loop 96 (first difference +0x1); the data pointer stored ahead of width and height 31;
// the packing's height term named as the work arm's first statement 154 (495 B); the loop's element row through a `long *`
// to `entry->elements[i]` 36; the separate offset dropped for the running +0x7c total 191 (439 B). Reading on the
// select-first lead: retail's height web ends at the packing (`shl eax, 0xc` in place) and the format call reloads the
// height from its home, where ours keeps it in EAX to the push and moves the select to EBX.
// M3 lane 5 r228 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the clamp stored through a `long &` bound to the
// count field (`long &levels = entry->element_count; levels = ...`) 30, with the round-51 signed packing 30 (484 B); the
// round-51 packing with the width and height stores ahead of the kind store 31, that with round 122's late levels store and
// header pointer 33; round 122's lead re-measured 31 (484 B). The top block's height/count swap stays in all of them.
// M3 lane 11 r150 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the offset declared in the level loop's
// for-init (`for (long i = 0, offset = 0; ...)`) 109 (480 B, first difference +0x1: data leaves EBP); the offset declared
// right after the register call 110 (480 B); the packing's `width - 1` term first 29 (same code: the OR operands are
// canonicalised); the height store moved after the packing 98 (480 B); the width and height stores both after the
// packing 103 (480 B). All restored.
// M3 lane 15 r131 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the clamp with its constants on the left
// (`0 > count ? 0 : 4 < count ? 4 : count`) 29 (same code); the entry pointer declared at the function's top, ahead of the
// data test, 36 (the `imul` hoists above the test); the entry bound as a reference (`entry.` throughout) 29 (same code);
// the round-51 signed pitch/packed lead with the constant-left clamp 29 (484 B); that lead with the entry pointer at the
// top 36 (484 B).
// M3 lane 5 r232 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the clamp's store moved between the data
// store and the +0x8c clear 33; round 51's signed pitch/packed lead (re-measured 29 at 484 B) with the select read from
// the stored flag field (`entry->flag94 ? packed : 0`, 0x1ddd0's idiom) 29 (484 B); the clamp as a `levels` local
// capped by two independent ifs 168 (497 B); round 51's lead with the packing ahead of the clamp, after the +0x8c clear,
// 31 (the height in EAX as retail, but the clamp branch and the zero register move); the clamp written non-negative
// first (`count >= 0 ? (count > 4 ? 4 : count) : 0`) 32. All restored.
// M3 lane 5 r234 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): early returns (`return false;` for a busy
// entry, `return true;` on the other paths, no result local) 104 (486 B, first difference +0x1); the clamp through the
// CRT's `__min(__max(count, 0), 4)` 140 (484 B), as `__max(0, __min(count, 4))` 151 (495 B); the clamp's upper arm as
// `count < 4 ? count : 4` 30; round 51's signed pitch/packed packing with the `__min(__max(...))` clamp 150 (495 B).
// All restored.
// M3 lane 15 r133 (h2m fast; control 29, aligned 20 unmatched rows; the round-51 signed lead re-measured 29 at 484 B,
// aligned 18): the two header zero stores as one chained assignment 31; the clamp's lower test as `!(count >= 0)` 29
// (same code); on the signed lead the size word's three terms named in retail's order of computation (rows, pitch,
// then the OR) 29 (484 B, the lead's code); the height term written first in the one-expression OR 29 (the lead's
// code); the last column and last row named ahead of the pitch 29 (the lead's code); the three zero stores moved ahead
// of the dimension stores 38 (484 B, aligned 32). The height/count EAX/EDX choice stays in all of them.
// M3 lane 14 r147 (H2M_FAST with 0x1dc40/0x1db10, both MATCH; round 51's signed pitch/packed lead re-measured 29 at 484 B,
// only the top block's height/count swap left): on that lead the format call's count through a copy declared just before the
// call 29 (propagated); the clamp's store after the pitch, ahead of the packed word, 71 (486 B); the height part named
// inside the packing (`long rows = (height - 1) << 12;`) 29; the entry as `g_4b4b58 + index` 29; the busy test with the
// flag first 32 (the 29s are the lead's code). All restored.
// M3 lane 15 r135 (h2m fast with 0x1dc40/0x1db10, both MATCH; control 29): the clamp positive-first (`count > 0 ? (count <
// 4 ? count : 4) : 0`) 32; `(void)&height;` at the top 29, `(void)&count;` 29, `(void)&kind;` 29 (all three the same code);
// round 51's signed pitch/packed lead re-measured 29 (484 B), with `(void)&height;` 29 (484 B). All restored.
// M3 lane 11 r160 (h2m fast with 0x1dc40/0x1db10, both MATCH; round 51's lead re-measured 29 at 484 B): on the lead the
// clamp in a `levels` local right after the +0x8c clear, stored after the packing 29 or after the three zero stores 29
// (both the lead's code); the direct clamp store after the packing 32 (483 B, zero in EDX); main's packing with that
// local 29 (main's code); the clamp through a file-local forceinline helper taking the count 29 (the lead's code); 1..12
// probe locals 29 each (no tie). Diagnostics: the call's count as `(long)(short)count` 54, as the stored field 130 (any
// reference to the count at the call keeps the clamp value in EAX). All restored.
// M3 lane 13 r151 (h2m fast with 0x1dc40/0x1db10, both MATCH; round 51's lead re-measured 29 at 484 B, aligned 18): on
// the lead the register parameter `index` declared last (all callers in the file updated; 0x1d770 MATCH) 29 (the
// lead's code: parameter numbering does not move the height/clamp choice); the offset declared right after the clamp
// store 36, after the three zero stores 39; the pitch read from the stored width field 29, the `width - 1` term so 29
// (both forwarded); re-measured the clamp after the packing 32 (483 B). All restored.
// M3 lane 1 r163 (h2m fast with 0x1dc40/0x1db10/0x1d770, all MATCH unless noted; control 29; round 122's lead re-measured
// 29 at 484 B). Reading of the allocator's ranking on that lead: the merged clamp value ranks 36 (three definitions in
// two-candidate blocks and one store in the join block, which references twelve candidates) and is coloured ahead of the
// height's top piece, which ranks 24 (its store, its packing use, minus the four clamp blocks it only crosses), so the
// clamp takes EAX and the height EDX; the count's piece (20) then follows the clamp. The height's piece is that small
// because its three-use range (store, packing, the format call's push; 48) loses its last free register to the data
// pointer (49, EBP) and is cut at the packing. The clamp value ranked anywhere above the height changes nothing; only
// below 24 does the lead give retail's bytes, a 13-point gap that no statement order inside the two blocks retail fixes
// (six stores ahead of the clamp's branch, the rest after its join) can close. Forms: the packing inside `do { } while
// (0)` 29, the levels store inside one 29 (both the lead's code: no block boundary survives to the allocator).
// Diagnostics, restored: the height read at every use through the width parameter's address (an explicit load) 153
// (493 B; 0x1db10, 0x1dc40 and 0x1d770 lost), for the store, the packing and the format call only 152 (493 B).
// Re-measured: main's stores with the signed packing 29 (484 B), the select-first form 38 (483 B: its three-use height
// range keeps EAX to the push at rank 48, so the clamp takes EDX as retail), the count copied and clamped in place 166
// (497 B: the count's whole range then ranks 55 and takes EBP ahead of the data pointer).
// On the lead, the header pointer formed between the packing and the levels store 29 (484 B, the lead's score).
// M3 lane 12 r191 (h2m fast with 0x1dc40/0x1db10/0x1d770, all MATCH; control 29): matched 0x1ddd0's own packing (`long
// pitch = width * sizeof(dword) / 64; ((pitch - 1) << 24) | ((height - 1) << 12) | (width - 1)`, round 51's lead) 29 at
// 484 B, the packing then retail's instruction for instruction (a compile-only screen shows why: `(h - 1) << 12` is
// distributed to retail's `shl; sub`, while `(h << 12) - 0x1000` is factored to `(h + 0xfffff) << 12`, and the long pitch
// keeps `lea; shr 6; shl 0x18` unfolded); on it the packed word as a dword 97 (479 B), the pitch as a dword 29, the pitch
// named at the else arm's top 155 (495 B), the width and height parameters typed dword (declaration, definition) 32, the
// same on main's packing 32 (483 B). Reading of the allocator's ranking on the lead: the height's whole range (store,
// packing, the level loop's shifts) is not worth a register in the first pass, so only its top piece is tried, in the
// fourth pass, after the merged clamp value (36: three definitions in two-candidate blocks, one store in the join block)
// has taken EAX in the first; a forced-ranking search that puts the clamp in EDX leaves 7 (`sub eax, 0x1000` scheduled
// after the pitch terms and EDX for the `width - 1` term), the same 7 with the height term written first in the OR.
// No statement order in the source moves the clamp's pass.
// Roasted-Codes (#1515 comment, 2026-11-26; fast reduced-image scores): the primary header writes and the Register call
// addressed through `g_4b4b58[index]`, with the entry pointer kept for the other fields, 167 (507 B; the scaled index
// stays live and the stores take the global's displacements); only the common-word store so addressed 161 (528 B).
// Register's data argument read as `entry->data` (on the signed-packing, late-levels, header-pointer form) 152 (499 B;
// the height takes the data's EBP and a real field load follows the format call).
// M3 lane 1 r172 (h2m fast with 0x1dc40/0x1db10/0x1d770, all MATCH; control 29): a forced-ranking search over 24
// single decisions finds no match (best 17: the clamp's pick moved from EAX to EDX), so at least two decisions remain.
// Forms: the clamp's middle test as `count < 4 ? count : 4` 30; the clamp with reversed compares (`0 > count ? 0 : 4 <
// count ? 4 : count`) 29 (same code); the clamp stored right after the data store 33; the pitch named as a long and the
// packing as one OR expression of `(x - 1) << n` terms, with the `< 4` middle test 98 (479 B), with the clamp after the
// data store 101 (479 B). All restored.
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

