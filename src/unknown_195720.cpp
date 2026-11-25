/* UNKNOWN_195720.CPP: the bit stream module, the packet encoding every
   network message codec uses: bit reads and writes, buffers, checkpoints,
   and the direction, logarithmic real and wide string codecs built on them.
   Retail never inlines these, hence /Ob1; the codecs that inline the small
   helpers are in unknown_1946f0.cpp */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_1946f0.h"
#include <math.h>
#include <string.h>

// @flags /O2 /Ob1 /Gr /arch:SSE


// @retail 0x1946f0
bool function_1946f0(s_bitstream *stream)
{
	bool result = stream->bit_position > (stream->size_in_bytes << 3);
	if (stream->error)
		result = true;
	return result;
}

// score 2 (was 2): the masked-byte and clear-start LEAs keep the reversed data/index operand order.
// Lane 16 round 74: a data local assigned before the index gives [index + data] (assigned after it,
// [data + index]; its declaration position does not matter), but retail reads the field in each block.
// Real: data local at the top of the discard block 40; explicit sums with the division inline, in
// either operand order 2/2; the masked byte by inline division with the clear from the index 2; data
// local declared first and assigned in each block 35.
// Reversed additions 2; grouped clear start 2; data plus one then index 2; shared clear start 24;
// count before mask 47; count declared first 2; split index declaration 2; index plus one then data 2;
// element address plus one 2; const index and count 2; size local 47.
// discard entry guard 2; nested writing gates 2; unchanged writer guard 2; accounting store order 13;
// positive clear count 21; inclusive remaining count 2; named byte mask 2; native signed bit offset 2.
// Index expression at each use 2; bytes through the dword buffer member 2; data pointer local 35;
// plain masked assignment 2; size-first bound comparison 4.
// `saved / 8` at the two address uses or at every use 2/2; a `byte *const &` reference to the data
// field before/after the index 2/2; the masked byte as an explicit read-modify-write 2.
// Round 37: the byte index declared at the function's top, after `saved`, at the discard block's top, at the
// mode block's top with the count 2/2/2/2; an int index 2; the index computed before the counter store 17.
// Round 97: masked pointer dereference 2; clear-start index first alone 2; two-use buffer pointer 40;
// current-byte pointer 46; two-use next-byte index 24; grouped remaining count 2;
// buffer member only at the mask or only at the clear 2/2.
// Round 70 (--fast): index and count declared at function scope 2; a named bit index 2; the mask named
// in its block 2; the clear from `&data[index + 1]` 2; a named size for the count 2. Compile-only screens
// (same code as the build here): no spelling, cast, integer address sum, scope, temp count or deleted
// neighbour statement flips either lea; small probes give [index + data] only when the address is the
// index's sole use (0x1957d0's shape) and [data + index] whenever the index is used again.
// Round 87 (m3 lane 12, fast): 1, 2, 3, 5 or 8 unused initialised locals at the top, 1, 2 or 4 before the
// index: 2 each (diagnostic: not a numbering tie); `byte_index[stream->data]` 2; `saved & 7` 42 (not the same
// value for negative positions); clear start and count from a parenthesised `byte_index + 1` 2; the index
// computed before the discard counter stores 17; `count >= 0 && count > 0` 2.
// Round 139: data loaded then the index reassigned in the mask block 23 (141 bytes), in the clear block 2;
// a distinct clear index after the data load 2; count formed by subtract then decrement, or size minus one
// then subtract 2/2; clear start derived from size minus count 45 (142); index reread from the checkpoint 60;
// saved declaration split from its assignment 2. Original body restored.
// Round 72 (m3 lane 10, fast): the index copied in the subscript by an embedded assignment 2; block-local data, copied index and a pointer to
// the byte 9; block-local data, own division, direct subscript 35 (its memory operand is [index + data], not a LEA); inline mask helper 35;
// inline address helper with the index first 2. One-file harness (exact here; diagnostics): identity pointer chains of 1 to 8 locals, at the top
// or used for both reads 2 each; one or two new stored fields 3/4; extra arithmetic at the top, in the block or after it 33/16/6, always data first.
// Round 77 (m3 lane 1, fast): the count tests and memset as a forceinline clear helper 2; a data local assigned
// before the index, used for the masked byte only 39 (that access becomes index-first `[edx + edi]`); single-use
// `saved / 8` index expressions and a single-use start index 2 (identical); the early data local used for the clear
// only 40 (the clear LEA becomes retail's `[edx + edi + 1]`); the data local read at the function's top for both 52.
// The newer value goes first, so retail orders its in-block field loads as values older than the index.
// Round 173 (m3 lane 4): compile-only screen of 22 forms (size-first compare, data + index pointer sum, `saved / 8` at the
// mask or the clear, a named next index, a named bit first, counter stores after the index, a size local, the count before
// the mask, buffer member or char casts, data locals at the function, discard-block, mode-block or mask-block top): no
// spelling flips the masked-byte LEA; only a data local read before the index flips the clear LEA (and keeps the data in
// a register). Real (fast): size-first compare 4; char-typed masked byte 2; data local at the mode block's top 49; data
// local in the mask block 35; counter stores after the index 17.
// Round 142 (m3 lane 8, fast): the pop as a separate decrement then the read 2; the read from `count - 1` with the decrement
// after 62; a named popped checkpoint index 2; the mask without the byte cast 2; the subscript through `(dword)byte_index` 2.
// Compile-only, both LEAs data-first: embedded index assignment in the bound test, a byte mask local in a block,
// `*(byte_index + stream->data)`, `saved % 8` named before the index.
// Round 162 (m3 lane 5, fast; all the same code, 2): a local view of the stream with a plain data pointer, with data and size
// as a nested first member, as a base type, or the data as a one-element array (diagnostics); the index cast to a pointer and
// the data to an integer at both addresses (diagnostic); forceinline address helpers taking (data, index), (stream, index) or
// (index, data); the address as an integer sum from the dword buffer member.
// Round 95 (m3 lane 10, fast): the whole discard block as a force-inlined helper taking (stream, bit) 2; the byte
// index from a force-inlined bit-to-byte helper at each of its four uses 2; `if (!discard) return;` then the flat body
// 2; a named bit-position local for the compare and the subtraction 4; the masked byte as `mask & byte` 2. A 600 s
// permuter run on the body (76 tries) found none below 2.
// Round 76 (m3 lane 2, fast): the clear from `saved / 8` with the count from the index 2; a second named index in the mask block
// 2; the mask indexed by a copy of saved 2; the mask index as `(long)(saved / 8)` 2; the mask as `(byte)~(-1 << (saved % 8))` 4.
// Compile-only screen (exact here): every spelling whose index value numbers to the named index (`saved / 8` at each use, in an
// inline, through a copy, with `+ 0`, through a second local, in the join block) keeps both LEAs data-first; only an index that
// stays a separate computation (`saved >> 3`, `(saved & ~7) / 8`, a reread checkpoint) ranks first, and it is then recomputed.
// Round 192 (m3 lane 4, fast): the mode and position tests as nested ifs 2; the mask as `(byte)~(0xff << (saved % 8))` 4;
// `unknown30 += 1` with the clear under one `count >= 0 && count != 0` 3; the count as `size_in_bytes - 1 - byte_index` 2;
// the pop as a decrement statement then the read 2. Compile-only screens (diagnostic): 0 to 40 unused locals at the top and
// 1 to 8 dead inline-helper locals at the top, the mode block, before the bound test or before the count: both LEAs stay
// data-first.
// Round 119 (m3 lane 9, fast): the index bound to a `long const &` 2; `int saved` 2; `register` on the index 2; a
// `s_bitstream *const` parameter 2; the index incremented after the mask with count and clear from it, as a statement
// or as `size - ++byte_index`, 11 each (first difference +0x15: `inc edx` then `add edi, edx`, no clear LEA).
// M3 lane 2 r85 (fast): the definition moved to the end of the file 2 (same code; diagnostic, so source position is not
// the lever); the mask shifted from `1u` 2; the index as `(saved + (saved < 0 ? 7 : 0)) >> 3` 74 (151 bytes; the stream
// then arrives in EDX; diagnostic); the bit-position test ahead of the mode test 7; the mask's bit offset as
// `saved - byte_index * 8` 36. Re-measured: the explicit masked assignment 2, the clear from `&data[index + 1]` 2.
// M3 lane 5 r187 (fast): a data local inside the clear's `count > 0` block 2; the mask as a `long` local in a braced mask block 2;
// the index as `saved` then `byte_index /= 8` 2; a data local after the mask block, before the count, used only by the clear 2;
// the same local after the count 2. Both LEAs stay data-first in each.
// Round 186 (m3 lane 6, fast): the member form (`s_bitstream::function_194710(bool)`, every call `stream->function_194710(true)`,
// all callers still matched) 2 (same code); the mask as `(byte)~(~0u << (saved % 8))` 4; the index renamed to sort first or
// after the count 2/2; the mask under the clear's `count >= 0` test (the bound test folded into it) 40. Diagnostic: constant
// stores (1, 0, -1, 4) to a scratch global after the pop leave both LEAs data-first.
// M3 lane 2 r90 (fast): the masked byte through a `byte &` element reference in its block 9; the checkpoint bound to a
// `long const &` 54 (reloaded at each use); the pop as `checkpoints[checkpoint_count -= 1]` 2; the bit offset named
// (`saved % 8`) before the byte index 2; a `byte *const &` to the data field inside the clear's `count >= 0` block only 2.
// Re-measured: the data-field reference after the index or at the function's top 2/2, the counter stores swapped 13.
// M3 lane 13 r101 (fast): the count formed as size minus index, tested as `--count >= 0` 2; the bound test as
// `size_in_bytes - byte_index > 0` 52 (142 bytes); the memset size written out at the call (count kept for the tests) 15
// (143 bytes); `long bit_offset = saved % 8;` named after the byte index for the mask 2; one `count >= 0` test guarding
// both the mask and the clear (the separate bound test dropped) 40. Both LEAs stay data-first in each 140-byte form.
// M3 lane 5 r193 (fast): the mask named as a byte local at the mode block's top (sunk to its use) 2; `saved / 8` written
// at every use, re-measured 2; a data local at the mode block's top used by the mask 48 (the mask access becomes
// index-first `[edx + edi]` but folded into both memory operands, no LEA, data held in EDI); an unused read of the data
// field at the mode block's top or the function's top 2/2 (diagnostic, dead, folded away before ordering); block-local
// data locals in both the mask and the clear blocks 35. Sibling evidence: retail 0x1957d0 and 0x195820 put the value
// computed after the data load first (`[eax + edx]` index first, `add esi, edi` index destination), so retail's two
// LEAs need data loads ordered before an index that is still computed once.
// Then the clear's start as a `byte &` element reference passed by address 2 (same code); with the mask's element
// reference as well 9 (round 90's mask form).
// M3 lane 12 r137 (fast): `__assume(stream->data != NULL)` at the mode block's top or just before the index 2/2 (same code;
// an early value-numbered data read does not reorder the in-block loads); `short byte_index` 47 (143 bytes, a movsx);
// a force-inline clear helper taking (data, size, saved) with the stream's data and size passed in 37 (the inlined data
// parameter is loaded early into EBX, not propagated to its uses); the index through a `long &` bound to a plain local 2.
// M3 lane 5 r198 (fast): the masked byte's address named before the bound test with the division written at the address
// 23 (140 bytes: the data load moves above the test and the index is added into it); the bound test as a comparison of
// the byte's address with the buffer's end 53 (142 bytes); the bit offset named first with the index as
// `(saved - bit_offset) / 8` 60 (146 bytes); the data through a new file-local inline accessor at both addresses 2 (same
// code); both addresses as integer sums written index first 2 (same code). Compile-only screens (exact here, build/r198
// b1-b7.py): in every form the base of each LEA is the operand loaded last (the data), also with the data as a global
// (`[data + index + 1]`); only a data value held in a register before the index is defined (a parameter, an early local)
// gives retail's index base, and then it is not reloaded. Diagnostic (compiler settings, not kept): favour-size and
// minimum-size builds keep the data as the base too, so the order is not a speed heuristic.
// Round 118 (m3 lane 10, fast): the masked byte and the clear start as `byte_index + stream->data` sums 2; a named
// `long bit_index = saved % 8;` for the mask 2; the pop as `checkpoint_count--;` then the read 2; the mode and position
// tests nested 2; unknown2c stored first, then the index, then `unknown30++` 15. Both LEAs stay data-first in every
// 140-byte form.
// M3 lane 4 r214 (fast, with the file's 18 matched functions): a data local declared ahead of the byte index and
// assigned from the field inside the mask block and inside the clear block (declared just before the index, at the
// mode block's top or at the function's top) 35/35/35 (the mask access becomes `[edi + edx]` memory operands with the
// data first, no LEA; the clear stays data-first); the same assigned in the mask block only 35; a `byte *const *`
// pointer to the data field taken before the index and read at both uses 2 (same code); the clear's start as
// `&stream->data[byte_index] + 1` 2 (same code). A data symbol older than the index does not give the index-first
// order once the data is read in the block.
// M3 lane 6 r197 (fast): the masked byte read into a byte local and stored back 2; `saved / 8` at every use with the
// read-modify-write written out 2; the mask as `(byte)~(0xffu << (saved % 8))` 4; both addresses through an
// `(unsigned long)` index 2; the count as `size_in_bytes - (byte_index + 1)` 2. Compile-only probes (small model): a
// plain indexed load or store folds index first (`mov [index + data], bl`, as 0x1957d0 does), while a read-modify-write's
// address and a memset's start are formed data first in every spelling tried; retail has both LEAs index first.
// M3 lane 3 r266 (fast): the mask and the clear in a forceinline (stream, byte index, bit offset) helper given `saved / 8`
// and `saved % 8` as expressions (parameter-only values) 2; a `byte *const *` to the data field formed after the index
// and read at both addresses 2; a `byte *const &` bound to the field after the index 2; the byte mask named before the
// index 2 (all four the same code, both LEAs data first); a named next index (`byte_index + 1`) for the count and the
// clear 24 (the size load moves ahead of the accounting store).
// M3 lane 4 r221 (fast, with the file's 18 matched functions): the mask's bit offset named right after the pop, at the
// function's top, 2; the masked byte as `*(stream->data + byte_index) &= ...` 2; the clear's start as
// `&stream->data[byte_index] + 1` inside the `count > 0` test 2; the two count tests as one `count >= 0 && count != 0`
// 3; a block-local size read once for the bound test and the count 44 (first difference +0x2a). Compile-only: a
// block-local pointer to the masked byte (from the field, or from a block-local data local) forms it as
// `mov edi, [esi]; add edi, edx` (no LEA); a block-local data local or a named start pointer in the clear block keeps
// the clear LEA data-first.
// M3 lane 2 r112: a compile-only screen (the file's flags without /GL; both LEAs reproduced) of 22 spellings flips neither
// LEA. Probes: the operand created later is the base in every form (a mask-block index computed separately, as
// `saved / 4 / 2`, is index-first there but is then recomputed). Fast: both addresses as `data - -index` 2; an
// `unsigned long` index 2; the bound test as `saved < size_in_bytes * 8` 54 (143 B); the clear's start from the buffer's
// end (`&data[size_in_bytes] - count`) 45 (142 B); the masked byte as `((byte *)byte_index)[(long)stream->data]` 2.
// M3 lane 8 r197 (fast with 0x1957d0, 0x195820): the clear's start as `1 + byte_index + stream->data` 2; the mask
// as `(byte)(-1 + (1 << (saved % 8)))` 2 (both the same code); the position test as `stream->bit_position > saved`
// 4; the index computed ahead of the unknown2c store 54; the size bound by `long const &` for the bound test and the
// count 11. Both LEAs stay data-first in each (compile-only listing).
// M3 lane 3 r274 (fast): an early data local (before the index) with the masked byte through a byte pointer `data +
// byte_index` 9; a block-local data local in the mask block with the masked byte through `&data[byte_index]` 9; an early
// data local for the masked-byte pointer and the clear 40; the masked byte through the dword buffer member, the clear
// through a block-local data local and a named start pointer 2 (same code); one data local declared before the index and
// assigned in each block re-measured 35. Compile-only: the body compiled as C keeps both LEAs data first; an early data
// local used only by the mask is sunk into the mask block and becomes the add's destination (`add edi, edx`).
// M3 lane 11 r132 (fast with 0x1957d0/0x195820; compile-only LEA screen): the pop through a file-local inline helper 2; the mask and
// clear as a forceinline (stream, position) helper with its own byte index 2; both data reads through a const view of the stream 2;
// `saved` and the index as `long const` 2 (all the same code); the mask as `(byte)~(-1 << (saved % 8)) & 0xff` 4.
// M3 lane 6 r210 (fast): a block-local data local read through a `byte *const &` in the mask block 35, in the clear's
// `count > 0` block 2 (same code); the bound test as `stream->size_in_bytes > byte_index` 4; `byte_index` declared ahead
// of the counter stores and assigned after them 2 (same code). Diagnostic: both addresses through a byte-array pointer
// cast of the data (`(*(byte (*)[1])data)[i]`) 2 (same code, both LEAs data-first).
// M3 lane 7 r199 (fast with 0x1957d0/0x195820, both MATCH): the pop through a `long *` to the checkpoint slot 2 (same
// code); the bound test as `byte_index <= size_in_bytes - 1` 51 (141 B); the mask with a redundant complement term 40
// (142 B); the clear started from the buffer's end (`&data[size_in_bytes] - count`) 45 (142 B); the index recomputed
// from the stored fields (`(bit_position - unknown2c) / 8`) 51. Compile-only probes (standalone, the file's flags): an
// index expression written in the address with no earlier use gives index-first (`add idx, data`, as 0x194830); once
// the same value is used earlier (the bound test), every spelling gives data-first in both the RMW and the clear LEA,
// with the data read in its block; only a data value defined before the index reverses them.
// M3 lane 16 r156 (fast with 0x1957d0/0x195820, both MATCH each time): a data local read before the index for the clear
// with a block-local data local for the mask 40 (the clear's data then held in EDI from the top); the mask as
// `~(byte)(0xff << (saved % 8))` 4; the index and the data field bound to `long const &` / `byte *const &` and used at
// every site 2; the explicit read-modify-write with one `count >= 0 && count > 0` test and the clear from
// `&data[byte_index + 1]` 2; a block-local byte mask with the count as `size - 1 - byte_index` and the clear from
// `&data[1] + byte_index` 2 (the 2s the same code: both LEAs data first).
// M3 lane 12 r164: a compile-only grid (both LEAs reproduced) of 192 combinations: index named, written `saved / 8` at every
// use, or named for the tests with the expression at the addresses x data from the field, through the dword buffer, or
// through forceinline (index, data) / (data, index) address helpers x pre-decrement in the subscript or a separate
// decrement x either counter-store order x clear start `data + i + 1`, `&data[i + 1]` or `&data[i] + 1` x the mask with or
// without its byte cast: every one keeps both LEAs data-first. Fast: the expression index with the buffer, the separate
// decrement, swapped counter stores and `&data[i + 1]` 13; the named index with the (index, data) helper 2; the mixed index
// with the (data, index) helper and the separate decrement 2. All restored.
// M3 lane 8 r210 (fast with 0x1957d0/0x195820, both MATCH each time): `long const saved` 2; a const stream parameter 2;
// memset's count cast to `size_t` 2 (all the same code); the index computed between the two accounting stores 54; the
// bound test as `byte_index + 1 <= size_in_bytes` 53 (143 B). Compile-only diagnostics: an unused field read at the
// function's top and a data read ahead of the accounting stores leave both LEAs data-first; the data tested in the mode
// test stays in EDI and the mask addresses `[edi + edx]`, still data first.
// M3 lane 16 r159 (h2m fast with 0x1957d0/0x195820, both MATCH each time): a block-local data local with the masked byte
// through a `byte &` bound to `data[byte_index]` 9; that with a pointer element and the explicit read-modify-write 9; that
// in the mask block plus a block-local data local in the clear's `count > 0` block 9 (the clear LEA stays data-first); a
// `byte &` to the masked byte bound before the bound test, the clear from its address plus one 46; the index as a `long
// const &` bound to the temporary `saved / 8` 2 (same code). With a block-local data local the mask becomes `mov edi,
// [esi]; add edi, edx` (data first, no LEA) where retail loads the data into EAX and adds it into the index's LEA.
// M3 lane 16 r161 (h2m fast with 0x1957d0/0x195820, both MATCH each time): a data local read before the index and used
// only through a `byte &` element reference in the bound block 9 (the load sinks into the block, `mov edi, [esi]; add
// edi, edx`); a block pointer started at the field and advanced by the index with `+=` 9; a const data local read before
// the index with a const pointer formed as `byte_index + data` 9; a block-local byte mask and data local 35 (memory
// operands, no LEA); the clear's start as `&stream->data[byte_index + 1]` with `count * sizeof(byte)` 2 (same code).
// M3 lane 7 r204 (h2m fast with 0x1957d0/0x195820, both MATCH each time): the index at each use with a named byte mask,
// the explicit read-modify-write and the clear through a block-local data local 2; a named index with block-local mask
// data, a named mask, `&=`, block-local clear data and the count as `size - 1 - index` 35; that with `size > index`, the
// explicit read-modify-write and the clear from `&data[index + 1]` 36; the index at each use with block-local mask data,
// a pointer read-modify-write, the complement mask, the clear from the element address and the count as
// `size - (index + 1)` 35; the index at each use with `size > index`, block-local mask data, the explicit read-modify-write,
// a named mask and block-local clear data 36. Compile-only grid (the file's flags, exact here): 1296 crossings of the index
// named or at each use, the bound test's side, the mask block's data (field or block-local), three read-modify-write
// spellings, three mask spellings, three clear starts, three count spellings and both accounting store orders: neither
// LEA comes out index-first (best 2). Probes: an index that dies at the read-modify-write is added into (`add index,
// data`); one live afterwards gives data-first whenever the data is a field load in the block.
// M3 lane 13 r136 (h2m fast): the masked byte through a `byte &` bound to the element inside the bound block 9 (`mov edi,
// [esi]; add edi, edx`); the byte mask named before the bound test 2; the checkpoint pop through a file-local inline helper 2;
// a named bit index beside the byte index 2 (the three 2s the same code).
// M3 lane 15 r125 (h2m fast with 0x1957d0/0x195820, both MATCH): the masked byte through a `byte &` element of the byte view
// of the dword buffer member 9; a `byte *const data` read just ahead of the index for both addresses 40; the clear from
// `&data[index] + 1` with `count * sizeof(byte)`, the bound test `size > index` and the mask through `*(data + index)` 4;
// `saved / 8` written at the bound test and the masked byte with a block-local data local and the index named after the mask
// block 35; the same without the data local 2 (same code).
// M3 lane 4 r243 (h2m fast with 0x1957d0/0x195820, both MATCH): `saved / 8` written at every use with the clear from
// `&stream->data[saved / 8 + 1]` and the count as `size_in_bytes - (saved / 8 + 1)` 2; that with the two accounting stores
// swapped 13; a named index with the masked byte as `*(stream->data + byte_index)` and the clear from `stream->data +
// (byte_index + 1)` 2, that with `if (!discard) return;` ahead of the block 2, that with the index named at the
// function's top beside `saved` 26. Both LEAs stay data first in every 2.
// M3 lane 1 r150 (h2m fast with 0x1957d0/0x195820, both MATCH each time): the count computed ahead of the bound test 47;
// `int` for saved, the index and the count 2 (same code); a `size_t` count 3; a data local read just ahead of the index
// for both addresses 40 (re-measured); the count as `size_in_bytes - (byte_index + 1)` 2 (same code). Compile-only (the
// file's flags, both LEAs reproduced): the same body as a member function of the stream type keeps both LEAs data first;
// a data local read anywhere ahead of the index (function top, before the mode test, mode block top, after the accounting
// stores) gives the clear index first but folds the mask's address into `[index + data]` memory operands (no LEA); the
// index or saved typed `int`, a `size_t` count, `saved / 8` written at the addresses only, and a byte pointer to the end
// byte formed at the index all keep both LEAs data first.
// M3 lane 6 r226 (h2m fast with 0x1957d0/0x195820, both MATCH): the clear's start as `stream->data + (byte_index + 1)`
// with the count on the index 2; `1 + stream->data + byte_index` 2; a `bool const` discard parameter 2 (the three the
// same code); the two accounting stores moved after the clear 49 (141 B); the checkpoint slot popped first with the
// position read inside the discard block 66 (first difference +0x1).
// M3 lane 7 r213 (h2m fast with 0x1957d0/0x195820, both MATCH; base 2): the mask and clear through a signed `char` view
// of the buffer 2; the bit offset named beside the byte index 2; the mask as `~(byte)(0xff << (saved % 8))` 4; the count
// as `size - 1 - index` 2; the masked byte as `*(data + index)` with the clear from `&data[index + 1]` 2. Both LEAs stay
// data first.
// M3 lane 2 r137 (h2m fast with 0x1957d0/0x195820, both MATCH; base 2): the data read through the stream's first member
// (`*(byte **)stream`) at both addresses 2, at the masked byte only 2, at the clear only 2, with the masked byte as an
// explicit read-modify-write through it 2 (the four the same code, both LEAs data first); the bound test as
// `size_in_bytes - byte_index > 0` 52 (142 B).
// M3 lane 14 r144 (h2m fast with 0x1957d0/0x195820, both MATCH; base 2): a data local at the discard block's top for the mask
// only (clear on the field) 48, the same just ahead of the index 39 (the mask folds into `[edx + edi]`, index first; the
// clear reloads, data first); that local for the clear only 40 (the clear's LEA is retail's `[edx + edi + 1]`, but the
// mask's field read numbers to the local and folds `[edi + edx]`), at the function's top 52; an unused early data read 2.
// M3 lane 8 r227 (h2m fast; base 2): a phase scan on the LTCG build (the earlier scans were compile-only), 1 to 7
// initialised dead locals at the function's top, 2 each (both LEAs stay data first: no symbol-count phase here).
// M3 lane 4 r250 (h2m fast with 0x1957d0/0x195820, both MATCH; base 2), the buffer passed as a parameter value to new
// file-local forceinline helpers (match-only): the count tests and memset in a (buffer, first, count) helper 2 (same
// code); the mask in a helper taking the element by `byte &` and the bit count 9 (the address becomes `mov edi, [esi];
// add edi, edx`), with the clear helper too 9; the mask in a (buffer, index, bits) helper 35 (memory operands), with the
// clear helper too 35.
// M3 lane 13 r146 (h2m fast with 0x1957d0/0x195820, both MATCH; base 2): one data local read just ahead of the index and
// used by both blocks, with the masked byte through a `byte &` element 40, through a `byte *` element pointer 40, as an
// explicit read-modify-write 40 (each: the mask folds into `[edx + edi]` memory operands, the clear's LEA is retail's
// `[edx + edi + 1]`); the body flattened with two early returns (`if (!discard) return;`, then the mode/position miss storing
// the position and returning) 2 (same code), that with the masked byte through a `byte &` element on the field 9.
// M3 lane 13 r157 (fast with 0x1957d0/0x195820, both MATCH; base 2): 12, 16, 24 or 40 initialised dead locals at the
// top on the LTCG build 2 each, 16 or 32 just before the index 2/2 (no symbol-count threshold); the discard test merged
// into the mode test with `if (discard)` again for the position store 2; the mode test held in a named bool 2 (same
// code); the position test as a named discarded-bit count (`discarded > 0`, stored to unknown2c) 56 (142 B).
// M3 lane 7 r229 (fast with 0x1957d0/0x195820, both MATCH each time; base 2). Reading of the address sums'
// operand sort: a field load (the data read in its block) always ranks above a named local, so the data comes first in
// both LEAs whatever the spelling; the index ranks first only against a data local referenced before it (ranks follow
// the order of first reference), and such a local is then loaded once, early, and kept. Retail's index-first LEAs with
// a data read in each block fit no form found. Forms: a data local at the mode block's top for both addresses 49 (both
// LEAs index first, the data load moves up to +0x15); that with the local read again from the field ahead of the clear
// 48, and ahead of the mask too 35; the index incremented after the mask with the count and the clear from it 11; the
// masked byte through a block-local `byte *` formed from the field 9.
// @retail 0x194710
void function_194710(s_bitstream *stream, bool discard)
{
	long saved = stream->checkpoints[--stream->checkpoint_count];
	if (discard)
	{
		if (stream->mode == 1 && saved < stream->bit_position)
		{
			stream->unknown30++;
			stream->unknown2c = stream->bit_position - saved;
			long byte_index = saved / 8;
			if (byte_index < stream->size_in_bytes)
				stream->data[byte_index] &= (byte)((1 << (saved % 8)) - 1);
			long count = stream->size_in_bytes - byte_index - 1;
			if (count >= 0)
			{
				if (count > 0)
					memset(stream->data + byte_index + 1, 0, count);
			}
		}
		stream->bit_position = saved;
	}
}

// @retail 0x1947a0
void function_1947a0(s_bitstream *stream)
{
	stream->mode = 3;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (function_1959c0(stream, 32) == k_debug_marker)
	{
		stream->error = true;
		return;
	}
	stream->bit_position = 0;
	stream->error = false;
}

// @retail 0x1947e0
void function_1947e0(s_bitstream *stream, dword value, long bits)
{
	if (bits < 32 && value >= (dword)(1 << bits))
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", value, 1 << bits);
	}
	function_195720(stream, value, bits);
}

PRIVATE __forceinline void vector_basis_cross(vector3f const *v, vector3f const *axis, vector3f *out)
{
	real k = axis->j * v->i - v->j * axis->i;
	real j = v->k * axis->i - v->i * axis->k;
	real i = v->j * axis->k - axis->j * v->k;
	out->i = i;
	out->j = j;
	out->k = k;
}

// @retail 0x194870
real function_194870(vector3f const *v, vector3f *a, vector3f *b)
{
	if ((real)fabs(g_4687a8->i * v->i + g_4687a8->j * v->j + g_4687a8->k * v->k) < (real)fabs(g_4687ac->i * v->i + g_4687ac->j * v->j + g_4687ac->k * v->k))
		vector_basis_cross(v, g_4687a8, a);
	else
		vector_basis_cross(g_4687ac, v, a);
	function_30bf0(a);
	vector_basis_cross(v, a, b);
	return function_30bf0(b);
}

// @retail 0x1949b0
real function_1949b0(vector3f const *v, vector3f const *w)
{
	vector3f a, b;
	function_194870(v, &a, &b);
	return (real)atan2(b.i * w->i + b.k * w->k + b.j * w->j, a.i * w->i + a.j * w->j + a.k * w->k);
}

// @retail 0x194a10
real function_194a10(vector3f const *axis, real angle, vector3f *out)
{
	vector3f a, b;
	function_194870(axis, &a, &b);
	real s = (real)sin(angle);
	*out = a;
	real c = (real)cos(angle);
	real k = (axis->i * out->i + axis->k * out->k + axis->j * out->j) * (1.f - c);
	real cross_j = out->k * axis->i - axis->k * out->i;
	real cross_k = axis->j * out->i - out->j * axis->i;
	real cross_i = axis->k * out->j - out->k * axis->j;
	out->i = axis->i * k + out->i * c - cross_i * s;
	out->j = out->j * c + axis->j * k - cross_j * s;
	out->k = out->k * c + axis->k * k - cross_k * s;
	return function_30bf0(out);
}

// @retail 0x194b60
void function_194b60(s_bitstream *stream, real value, real lo, real hi, long bits)
{
	long n = (1 << bits) - 1;
	real log_lo = (real)log(lo);
	real a = (real)log(value) - log_lo;
	real b = (real)log(hi) - log_lo;
	real r = a / (b / (real)n);
	long q;
	__asm
	{
		fld r
		fistp q
	}
	function_195720(stream, q, bits);
}

// @retail 0x194bc0
void function_194bc0(s_bitstream *stream, vector3f const *direction)
{
	long index = function_24f590(direction);
	if ((dword)index >= k_direction_limit)
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", index, k_direction_limit);
	}
	function_195720(stream, index, k_direction_bits);
}

// @retail 0x194fa0
void function_194fa0(s_bitstream *stream, word *buffer, long count)
{
	long i;
	for (i = 0; i < count; i++)
	{
		buffer[i] = (word)function_1959c0(stream, 16);
		if (buffer[i] == 0)
			break;
	}
	if (i >= count)
	{
		buffer[count - 1] = 0;
		stream->error = true;
	}
}

// @retail 0x194ff0
real function_194ff0(s_bitstream *stream, real lo, real hi, long bits)
{
	long value = (long)function_1959c0(stream, bits);
	if (value != 0)
	{
		if (value >= (1 << bits) - 1)
			return hi;
		lo = (real)exp(event_dequantize_real(value, (1 << bits) - 1, (real)log(lo), (real)log(hi)));
	}
	return lo;
}

// @retail 0x195240
void function_195240(s_bitstream *stream, vector3f *up, vector3f *forward)
{
	if (function_1957d0(stream))
		*forward = *g_4687b0;
	else
		function_24f6b0(function_1959c0(stream, k_direction_bits), forward);
	long value = (long)function_1959c0(stream, 8);
	real angle;
	if (value == 0)
		angle = -k_pi;
	else if (value < 254)
		angle = ((real)value * k_pi - (real)(254 - value) * k_pi) * (1.f / 254.f);
	else
		angle = k_pi;
	function_194a10(forward, angle, up);
}

// @retail 0x1952f0
bool function_1952f0(real a1, real a2, real a3, real a4, long bits)
{
	bool result = false;
	if (a1 >= 0.f && a2 >= 0.f)
	{
		long n = (1 << bits) - 1;
		if (fabs(log(a2) - log(a1)) < (log(a4) - (real)log(a3)) / (real)n)
			result = true;
	}
	return result;
}

// @retail 0x195560
bool function_195560(vector3f const *up_a, vector3f const *a, vector3f const *up_b, vector3f const *b)
{
	bool result = false;
	if (a->quantized_equal(b))
	{
		real angle_a = function_1949b0(a, up_a);
		real angle_b = function_1949b0(a, up_b);
		real difference = (real)fabs(angle_b - angle_a);
		if (difference < 0.024736950173974037f)
			result = true;
		else
			result = fabs(difference - k_two_pi) < 0.024736950173974037f;
	}
	return result;
}

// @retail 0x1955d0
void function_1955d0(s_bitstream *stream, void const *source, long bits)
{
	long position = stream->bit_position;
	long remaining = stream->size_in_bytes * 8 - position;
	long n;
	if (remaining > bits)
		n = bits;
	else
		n = remaining;
	if (n > 0)
	{
		if (position % 8 == 0)
		{
			memcpy(stream->data + (position >> 3), source, (n + 7) >> 3);
			if (((stream->bit_position + n) & 7) != 0)
				stream->data[(stream->bit_position + n) >> 3] &= (byte)(0xff >> (8 - ((stream->bit_position + n) & 7)));
		}
		else
		{
			long shift = position % 32;
			dword *end = stream->buffer + ((position + n - 1) >> 5) + 1;
			dword *destination = stream->buffer + (position >> 5);
			dword const *s = (dword const *)source;
			*destination++ |= *s << shift;
			if (destination < end)
			{
				do
				{
					*destination = *s >> (32 - shift);
					s++;
					*destination++ |= *s << shift;
				}
				while (destination < end);
			}
			if (((stream->bit_position + n) & 0x1f) != 0)
			{
				dword *last = stream->buffer + ((stream->bit_position + n) >> 5);
				*last &= 0xffffffff >> (32 - ((stream->bit_position + n) & 0x1f));
			}
		}
	}
	stream->bit_position += bits;
}

// @retail 0x195720
void function_195720(s_bitstream *stream, dword value, long count)
{
	long remaining = (stream->size_in_bytes << 3) - stream->bit_position;
	long n;
	if (remaining > count)
		n = count;
	else
		n = remaining;
	if (0 < n)
	{
		value &= 0xffffffff >> (32 - n);
		long first;
		first = stream->bit_position >> 5;
		long last = (stream->bit_position + n - 1) >> 5;
		long offset = stream->bit_position & 0x1f;
		value &= 0xffffffff >> (32 - n);
		if (first == last)
			stream->buffer[first] |= value << offset;
		else
		{
			stream->buffer[first] |= value << offset;
			stream->buffer[first + 1] = value >> (32 - offset);
		}
	}
	stream->bit_position += count;
}

// @retail 0x1957d0
bool function_1957d0(s_bitstream *stream)
{
	long position = stream->bit_position;
	bool bit = false;
	if (position <= (stream->size_in_bytes << 3))
		bit = (stream->data[position / 8] & (1 << (position % 8))) != 0;
	stream->bit_position = position + 1;
	return bit;
}

// @retail 0x195820
void function_195820(s_bitstream *stream, void *destination, long bits)
{
	long remaining = stream->size_in_bytes * 8 - stream->bit_position;
	long n = remaining > bits ? bits : remaining;
	if (n > 0)
	{
		long position = stream->bit_position;
		if (position % 8 == 0)
		{
			byte const *source = stream->data + (position >> 3);
			memcpy(destination, source, (n + 7) >> 3);
			long tail = n & 7;
			if (tail != 0)
				((byte *)destination)[n >> 3] = ((byte *)destination)[n >> 3] & (byte)(0xff >> (8 - tail));
		}
		else
		{
			long shift = position % 32;
			dword const *s = stream->buffer + (position >> 5);
			dword *end = (dword *)destination + ((n - 1) >> 5) + 1;
			dword *d = (dword *)destination;
			long tail = n & 0x1f;
			dword saved = 0;
			if (tail > 0)
			{
				dword bytes = (tail + 7) >> 3;
				if (bytes < 4)
					saved = (0xffffffff << (bytes * 8)) & d[n >> 5];
			}
			while (d < end)
			{
				*d = *s >> shift;
				s++;
				*d++ |= *s << (32 - shift);
			}
			if (tail != 0)
			{
				dword *last = (dword *)destination + (n >> 5);
				*last = (*last & (0xffffffff >> (32 - tail))) | saved;
			}
		}
	}
	stream->bit_position += bits;
}

// @retail 0x1959c0
dword function_1959c0(s_bitstream *stream, long count)
{
	long position = stream->bit_position;
	long remaining = (stream->size_in_bytes << 3) - position;
	dword result = 0;
	long n;
	if (remaining > count)
		n = count;
	else
		n = remaining;
	if (n > 0)
	{
		long first = position >> 5;
		long last = (position + n - 1) >> 5;
		long offset = position & 0x1f;
		if (first == last)
			result = stream->buffer[first] >> offset;
		else
			result = (stream->buffer[first + 1] << (32 - offset)) | (stream->buffer[first] >> offset);
		result &= 0xffffffff >> (32 - n);
	}
	stream->bit_position = position + count;
	return result;
}
