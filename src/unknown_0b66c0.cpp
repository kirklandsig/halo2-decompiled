// @flags /O2 /Ob1 /Gr
#include "transaction_storage.h"
#include "unknown_11c920.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>
#include <stdlib.h>
#include "unknown_0b4da0.h"

struct s_index_pair
{
	long index;
	byte unknown04[2];
	short index2;
};

struct s_index_triple
{
	long index;
	byte unknown04[2];
	short index2;
	byte unknown08[0x60];
	long index3;
};

struct s_small_index
{
	byte unknown00[0x18];
	signed char index;
	byte unknown19[0x5b];
	long count;
};

// @retail 0x000b66c0
char *csprintf_1024(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0x3ff, format, arguments);
	buffer[0x3ff] = 0;
	return buffer;
}

// @retail 0x000b66f0
char *csprintf_256(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0xff, format, arguments);
	buffer[0xff] = 0;
	return buffer;
}

// @retail 0x000b6720
unsigned long function_0b6720(const char *string)
{
	unsigned long length = 0;
	for (; length < 0xff; length++)
	{
		if (!*string++)
			break;
	}
	return length;
}

// @retail 0x000b6760
bool function_0b6760(const s_index_pair *pair)
{
	return pair->index != NONE && pair->index2 != NONE;
}

// @retail 0x000b6780
bool function_0b6780(const s_index_triple *triple)
{
	return triple->index3 != NONE && triple->index != NONE && triple->index2 != NONE;
}

__declspec(noinline) short function_0b67a0(const s_small_index *data);

// @retail 0x000b67a0
inline short function_0b67a0(const s_small_index *data)
{
	signed char index = data->index;
	if (index >= 0 && index < data->count)
		return index;
	return NONE;
}

// @retail 0xb6190
byte function_b6190(char const *a, char const *b)
{
	return strcmp(a, b) == 0;
}

static __forceinline unsigned long string_length_512_ab(char const *string)
{
	unsigned long length = 0;
	for (; length < 0x1ff; length++)
	{
		if (!*string++)
			break;
	}
	return length;
}

struct s_text512_ab
{
    char bytes[0x200];
    long find(long start, char const *substring) const;
};

// @retail 0xb61d0
long s_text512_ab::find(long start, char const *substring) const
{
    char const *string = bytes;
	long result = NONE;
	if (start < (long)string_length_512_ab(string))
	{
		start += (long)string;
		char const *found = strstr((char const *)start, substring);
		if (found)
			result = found - string;
	}
	return result;
}

static __forceinline char *upload_copy_ab(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

// score 21 (was 40): the string pointer and the character counter are kept in different registers
// here, and an extra register is pushed and restored around the copy loop. Tried: count before
// start in the parameter list 40; the length in a named local after the sign tests 40;
// string + start with start unmodified 28 but start then arrives in edx (0xb5f10 differs);
// that with a plain string parameter 41; start = string + start 40.
// MiniMax H (11-07): the sign and count tests with the bounded length in a named local and the
// source pointer named before the compare give 21 with the matched caller 0xb5f10 and the matched
// 0xb61d0 unchanged: the start now reaches EDI at entry as in retail, but the inlined scan still
// walks the text in ECX and counts in EDX where retail walks in EDX and counts in ECX, so the
// bounded-length sum and the copied source stay one register off. Tried and not kept: the source
// named after the compare (28, the start then arrives in EDX and 0xb5f10 differs at 6); a local
// copy of the start (28, same caller loss); the count tested before the start (27); the source
// named but the length inlined (28, caller loss); the source and the sum written through the text
// pointer (34, caller loss); the length masked with 0x1ff, which gives retail's scan registers but
// an extra instruction (29); the compare as count <= length - start (35, caller loss); the scan
// helper walking a local (0xb61d0 then differs at 2).
// MiniMax R (11-06): no lower score (40); the matched caller 0xb5f10 sets the start in ECX and the count in EAX with the string and the destination on the stack, so the declared order already produces retail's call setup; what is left is retail's start saved in EDI and its scan in EDX/ECX against our start in ECX and scan in EDI/EDX.
// MiniMax D (run 46): the start copied into a local before or after the guard gives retail's body
// (28) but LTCG then passes the start in EDX and the matched caller 0xb5f10 differs at 6; not kept.
// A second local for the count gives 51, the guard still testing the parameter gives 28 with the same
// caller loss, and the scanner helper without __forceinline gives 56 here and 49 for 0xb61d0.
// Cursor displacement with fixed/explicit maximum, unequal/ordered end tests and signed/unsigned
// returns: eight forms 36 each; threshold rejected, shared find changed.
// that with a plain string parameter 41; start = string + start 40; a local copy of the start
// 28, but the start then arrives in edx and the matched caller 0xb5f10 differs.
// Lane 8 r79: `string + start` gives retail's loop registers (the compile-only /Gd body is
// retail's: start EDI, walk EDX, length ECX) but LTCG passes start in EDX, not ECX: 28 with
// 0xb5f10 6; with count before start in every declaration and call 28; the length test nested
// 28; a plain string parameter 41; early returns 48.
// retail moves the start from ECX into EDI at entry so the inlined length scan
// counts in ECX and walks in EDX (as matched 0xb61d0 does); ours keeps the start in ECX, counts in
// EDX and walks in EDI, so EDI is saved with EBX around the scan. Tried: count before start in the
// parameter list 40; the length in a named local after the sign tests 40; string + start with start
// unmodified 28 but start then arrives in edx and the matched caller 0xb5f10 differs (also as
// `&string[start]`); that with a plain string parameter 41; start = string + start 40; cursor
// displacement with fixed/explicit maximum, unequal/ordered end tests and signed/unsigned returns:
// eight forms 36 each (shared find changed). Round 35, one-file harness (aligned/positional,
// current 16/40): early returns 21/42 (--fast 48); nested length test 16/40 (--fast 40);
// `length >= start + count` 17/40; all 23 other parameter orders 16-17/40; a const method of the
// text struct, with or without noinline, 18/41 (0xb5f10 then differs); no `volatile` with a
// noinline declaration 22/34 (0xb5f10 differs). Round 37 (real builds): the walk vs start
// competition decides it (retail: walk EDX, start moved to EDI; whenever ours moves start out of
// its incoming register, LTCG passes it in EDX and 0xb5f10 differs): plain parameter 41; a
// volatile read of the plain parameter at the add 40; `string + start` with the volatile
// parameter 28 (0xb5f10 6); a const method of the text struct (also reading bytes directly)
// 48/48 (0xb5f10 82); start added before the clamp 36, between the increment and the clamp 32
// (0xb5f10 6 each); `length >= start + count` 40.
// noinline declaration 22/34 (0xb5f10 differs). Round 11 (relink screen, current form 42 there,
// 0xb5f10 0): `string + start` or `&string[start]` 28 (start arrives in EDX, 0xb5f10 6); that body in
// all 24 parameter orders with matching calls 28 each (0xb5f10 5-37, never 0); `start += string`
// before the count clamp 38 (aligned 14, start in EDX again, 0xb5f10 6); `result` assigned after its
// declaration 42 (same code); the clamped size as a ternary or a separate size local 53 (0xb5f10 104).
// Retail evicts start to EDI so the scan counts in ECX, as 0xb61d0 does with its start in EAX.
// Later note: retail moves the start from ECX into EDI at entry so the inlined length scan
// counts in ECX and walks in EDX (as matched 0xb61d0 does with its start in EAX), saving EBX only
// around the scan; ours keeps the start in ECX, counts in EDX and walks in EDI, so EDI is saved with
// EBX around the scan. Whenever ours moves start out of its incoming register, LTCG passes it in
// EDX and the matched caller 0xb5f10 differs. Tried: count before start in the parameter list 40;
// the length in a named local after the sign tests 40; `string + start` (also `&string[start]` or a
// local copy of the start) with start unmodified 28, retail's loop registers (the compile-only /Gd
// body is retail's), but start arrives in EDX (0xb5f10 6), with count before start in every
// declaration and call 28, in all 24 parameter orders with matching calls 28 each (0xb5f10 5-37,
// never 0), with the length test nested 28; that with a plain string parameter 41; start = string +
// start 40; `start += string` before the count clamp 36-38 (aligned 14), between the increment and
// the clamp 32 (0xb5f10 6 each); cursor displacement with fixed/explicit maximum, unequal/ordered
// end tests and signed/unsigned returns: eight forms 36 each (threshold rejected, shared find
// changed); plain parameter 41; a volatile read of the plain parameter at the add 40; early returns
// 48 (harness 21/42); nested length test 40 (harness 16/40); `length >= start + count` 40 (harness
// 17/40); a const method of the text struct, with or without noinline, 41-48 (0xb5f10 differs, 82);
// no `volatile` with a noinline declaration 34 (0xb5f10 differs); `result` assigned after its
// declaration 42 (same code); the clamped size as a ternary or a separate size local 53 (0xb5f10
// 104). Round 41 (fast): the length through the existing bounded length 0x11c9a0 (inlined by LTCG
// as retail does in other files) 40, identical code, also with the length test nested 40; result
// set in an else arm 48 (95 bytes, 0xb5f10 41); `count + start` 40.
// Round 109 (lane 5, fast): the natural `string + start` at the copy re-viewed: the body then keeps
// start in EDX for its whole life (scan counts in ECX, walks in EDI), so LTCG passes it in EDX;
// retail evicts it ECX->EDI for the scan. A plain string parameter read once through its own
// address, at the scan with `string + start` 40 (104 bytes, 0xb5f10 6), at the add 40 (104), at the
// scan with `start += (long)string` 40 (104); `string + start` with `if (++count > 0x200)` 28
// (0xb5f10 6); `string + start` named after the clamp 28 (0xb5f10 6).
// Round 44 (lane 16, fast with 0xb5f10/0xb61d0): retail's scan takes ECX (count), EDX (walk) and
// EBX, so the start it receives in ECX moves to EDI (matched 0xb61d0 moves its EAX start to EDI
// the same way); ours keeps the start in whichever scratch register the scan leaves free and LTCG
// passes it there. Tried: `string + start` with the length from the existing out-of-file 0x11c9a0
// 28 (start in EDX, scan counts in ECX and walks in EDI; 0xb5f10 6); the result as the combined
// condition (`bool result = start >= 0 && ...; if (result)`) 44 (0xb5f10 41); a non-volatile local
// copy of the string for the scan and the add 60 (83 bytes, 0xb5f10 80); a `byte` result 41
// (0xb5f10 41); `result = true` before the copy 40 (same code).
// Round 26 (lane 15, fast): the string as the text struct of 0xb61d0 (`s_text512_ab const *volatile`, scanning and
// adding its bytes; callers cast as they do for find) 40 (same code), without the volatile 41 (104 bytes, 0xb5f10
// 35); a named end offset (`long end = start + count;`) before the test 43 (101 bytes); `start = (long)string +
// start` 40 (same code); the string added before the count increment 36 (0xb5f10 6, as listed above).
// Round 132 (an LTCG env link of this file reproduces it and 0xb5f10 exactly): `string + start` gives
// retail's body except that start lives in EDX and the walk in EDI (retail: walk EDX, start EDI), so
// LTCG passes start in EDX. With it, the length loop as `while (length < 0x1ff && *string++)` 28, a
// walk local in the length helper 28, the source named before the clamp 32 (0xb5f10 6 each); the
// while-loop helper alone 40 (same code).
// Round 54 (lane 12, fast with 0xb5f10/0xb61d0; env link = the build's listing): `if (++count > 0x200)` 40
// (same code); `string + start` named after the clamp with the length test nested 28 (0xb5f10 6); a walk
// local in the shared length helper 40 (0xb61d0 2); the length named inside the sign test with an early
// `return result` 40 (103 bytes); `long end = start + count` tested against the length 43 (101 bytes).
// Env grid (start += string / string + start / named source x count++ or ++count x one or nested
// tests, 12 forms): start in ECX for its whole life (40) or in EDX (28, 0xb5f10 6) only; retail
// colours start after the scan's counter and walk (EDI) and still receives it in ECX.
// Round 118 (lane 5, fast with 0xb5f10/0xb61d0): `string + start` with start renamed to sort after the
// scan's walk 28 (start in EDX, 0xb5f10 6; names do not move it); with a const start 28 (0xb5f10 6); the
// length on the left of `>= count + start` 29 (0xb5f10 6). Lead: `string + start` with the count tested
// before the start 39 gives retail's body colouring (start EDI, walk EDX, counter ECX, the push of EBX
// around the scan only), but LTCG then passes start in EDI and 0xb5f10 differs (35); the same test order
// with `start += string` 40 (start back in ECX). Retail tests the start first.
// Round 42 (lane 1, fast with 0xb5f10/0xb61d0): LTCG passes a parameter in a callee-saved register only
// when the body leaves it unmodified (0xb61d0 gets `this` in ESI, its modified start in EAX), so retail's
// ECX -> EDI start is a modified start coloured EDI. A modified local copy of the start (tests, sum and
// `+= (long)string`) 28 with 0xb5f10 6 (scan counter ECX as retail, but the walk takes EDI and the copy
// EDX, so the start arrives in EDX); the copy declared before `result` 28 (0xb5f10 6); the copy with a
// plain string parameter 41 (104 bytes, 0xb5f10 35); the copy taken inside the sign tests 28 (0xb5f10 6);
// copies of both start and count 51 (92 bytes, 0xb5f10 104).
// Round 122 (lane 5, fast with 0xb5f10/0xb61d0): a compile-only stack-parameter build colours a
// `+=`-modified start in a scratch register in every placement (so not only LTCG's incoming ECX);
// `string + start` there is retail's body but the sum goes to EAX, never writing start's EDI. Tried:
// the source as `(char const *)(start + (long)string)` 28 (0xb5f10 6); a long result 48 (95 bytes,
// 0xb5f10 92); `string + start` with `result = true` before the copy 38 (102 bytes, 0xb5f10 6); count
// tested first with `start + (long)string` 39 (retail's colouring but start arrives unsaved in EDI,
// 0xb5f10 35); `char const *source = string; source += start;` 27 (start in EDX, 0xb5f10 6).
// Round 105 (lane 8, fast with 0xb5f10/0xb61d0): the count-first `string + start` lead re-measured 39
// (start passed in EDI, the sum in a new EAX temp; 0xb5f10 35); count tested first with `start +=
// string` before the count increment 36 (counter ECX as retail, but start in EDX, walk EDI; 0xb5f10
// 6); with a plain string parameter: that 49 (92 bytes), start tested first with the add before the
// increment 49 (92 bytes), count first with `string + start` 46 (103 bytes); the add reading the
// plain parameter once through its own address (volatile) before the increment, count first or start
// first, 36/36 (0xb5f10 6 each). Moving the add ahead of the clamp always passes start in EDX.
// Round 124 (lane 5): every other parameter order, the definition and both calls changed together, in the
// real build: 40 for the orders keeping string first and start before count (0xb5f10 0 only for
// string/start/destination/count and string/count/start/destination), 40-41 for the rest (0xb5f10 2-6).
// An LTCG env link of this file (entry stub calling 0xb5f10; reproduces 40/28/39 and 0xb5f10 0/6/0) over
// 48 forms (start or count tested first, `start += string` / `string + start` / `&string[start]`, volatile
// or plain string, result variable or early returns, `++count` in the clamp): 28-40, nothing at retail's
// colouring with start arriving in ECX; `length - start >= count` 33 (start in EDX, 0xb5f10 6).
// Round 114 (lane 8, fast with 0xb5f10/0xb61d0): `(long)length >= start + count` 40 (same code); the shared
// length helper returning `long` 40 (0xb61d0 then 1, `jl` for `jb`); `0 <= start && 0 < count` 40; `0x200 <
// count` 40; `count <= (long)length - start` 34 (start arrives in EDX, 0xb5f10 6).
// Round 67 (lane 16, fast with 0xb5f10/0xb61d0): `length - count >= start` 34 (scan counter in ECX as retail,
// but start in EDX, walk EDI; 0xb5f10 6); that with `string + start` or `start + string` at the copy 29 each
// (the sum goes to the string load's EAX, retail adds into start's EDI; 0xb5f10 6); `start <= length - count`
// 34 (0xb5f10 6); `__stdcall` on the definition 40 (identical code).
// Round 21 (lane 17, fast with 0xb5f10/0xb61d0): on the lead with nested sign tests, a named length and an early
// one-use `source = string + start` (21: entry and colours retail's; the scan walks in ECX and counts in EDX, and the
// add precedes the compare): the length through the existing 0x11c9a0 21, the source as `&string[start]` 21, the
// source declared before the length 37 (0xb5f10 84); 0x11c9a0 with `string + start` after the clamp 28 (0xb5f10
// 6); the current body with 0x11c9a0 40 (same code); 1-7 dead temporaries (diagnostic) leave 40/28/21 unchanged.
// Round 22 (lane 18): the length helper `static inline` 40 and the copy helper `static inline` 40 (same
// code); the scan as `string[length]` 60 (83 bytes, 0xb61d0 46); constants on the left of the sign tests 40;
// the clamp as `count >= 0x201` 40. Retail's matched 0xb61d0 shows the same pattern: the inlined scan takes
// the start's incoming register (EAX there) and the start moves to EDI.
// Round 127 (lane 5, fast with 0xb5f10/0xb61d0): the count clamp as a ternary 40; `length >= start +
// count` 40; the copy helper written out (diagnostic) 40; the scan given a cast of the string 40 (these
// four same code); start added before the count increment 36 (0xb5f10 6).
// Round 48 (lane 10, fast with 0xb5f10/0xb61d0): lead, not kept (a one-use local computed ahead of the
// compare): inside `if (start >= 0 && count > 0)`, `long length = ...;` then `char const *source = string +
// start;` then `if (start + count <= length)` 21 with 0xb5f10 0: retail's prologue (`mov edi, ecx`, start
// arriving in ECX and coloured EDI), but the scan walks in ECX and counts in EDX (retail EDX/ECX) and the
// source is formed before the compare (retail after the clamp, in EDI); the compare as `length >= start +
// count` or `count + start <= length`, `&string[start]`, an unsigned length local 21 each (same code).
// Tried: `string + start` named at the function top 41; named inside the sign tests before the scan 37
// (start EDI and the scan counter ECX as retail, but the destination becomes an EBX register parameter,
// `ret 4`, 0xb5f10 84); with a plain string parameter at the top 54 (88 bytes); the nested length local
// with the source named after the clamp 28 (0xb5f10 6), with `start += (long)string` after the clamp 40,
// with the source named before the clamp 32 (0xb5f10 6), with a named `end` offset 40, with a string copy
// before the compare and `copy + start` at the call 40 (0xb5f10 35); `length - start >= count` nested or
// flat 34/34 (0xb5f10 6); the 21 form with a plain string parameter 49 (92 bytes, 0xb5f10 6); the nested
// length local with `start += string` and a plain string parameter 41 (104 bytes, 0xb5f10 35).
// Second lead (not kept, one-use local, 0xb5f10 6): `char const *text = string;` read after the clamp, then
// `start += (long)text` 27: the scan counts in ECX and the read is retail's unfolded `mov eax, [string];
// add` before the pushes, but start and the walk swap (start EDX and passed there, walk EDI; retail start
// EDI, walk EDX); the same with the nested length local, with `text + start` at the call, with `start =
// (long)text + start`, and with the existing 0x11c9a0 for the length 27 each (0xb5f10 6 each). Hybrids:
// the text read before `count++` 31 (0xb5f10 6); nested length local, text read before the compare and
// `start += (long)text` after the clamp 40 (0xb5f10 35); the first lead with a named `end` 21 (same code).
// Round 282: Shared end-pointer scan with a separate signed/unsigned length counter,
// unequal/ordered end tests, original/direct source: 36/36/36/36/40/40/40/40.
// Search sibling 19 and caller 6/96 lose their matches; helper and full body restored.
// Round 283: bounded-helper terminator returns, native signed/unsigned ordinal and return contracts,
// with original/direct source construction: 53/53/53/53/55/55/55/55; both matched caller and shared-
// find sibling lost in every trial. Original helper/body/interface restored; restored control 40, both
// siblings 0.
// Round 217: shared bounded-length short/word/int/unsigned-int counter, then nonzero-only
// input advance: 40/40/40/40/58/58/60/60. Original helper, body and all matched callers restored.
// Round 21 (lane 17, fast with 0xb5f10/0xb61d0): on the lead with nested sign tests, a named length and an early
// one-use `source = string + start` (21: entry and colours retail's; the scan walks in ECX and counts in EDX, and the
// add precedes the compare): the length through the existing 0x11c9a0 21, the source as `&string[start]` 21, the
// source declared before the length 37 (0xb5f10 84); 0x11c9a0 with `string + start` after the clamp 28 (0xb5f10
// 6); the current body with 0x11c9a0 40 (same code); 1-7 dead temporaries (diagnostic) leave 40/28/21 unchanged.
// Round 217: shared bounded-length short/word/int/unsigned-int counter, then nonzero-only
// input advance: 40/40/40/40/58/58/60/60. Original helper, body and all matched callers restored.
// M303: Shared bounded helper with library byte search, conditional/early return, original/direct source and flat/nested guards: 50 in all eight forms; shared find 37 and caller 84/75 regress, so helper and body restored.
// Round 38 (m3 lane 2, fast with 0xb5f10/0xb61d0): retail moves start to EDI at entry so the inlined
// length counter takes start's ECX (as 0xb61d0's counter takes start's EAX); ours keeps start in ECX and
// counts in EDX. `(long)length >= count + start` 40; the length test nested under the sign tests 40; the
// source formed in the copy call as `start + (long)string` 28 (counter ECX as retail, but start arrives in
// EDX; 0xb5f10 6); `return true` / `return false` with no result 48 (94 bytes); `if (++count > 0x200)` 40.
// Round 51 (m3 lane 13, fast with 0xb5f10/0xb61d0): the volatile string parameter advanced (`string += start`)
// instead of start 43 (108 bytes, 0xb5f10 6), advanced before the clamp 47 (108 bytes, 0xb5f10 6); the clamp to
// 0x1ff before `count++` 40 (same code); the length test first, then the sign tests 46 (92 bytes, 0xb5f10 6);
// the function `static` 40 (same code).
// Round 45 (m3 lane 15, fast with 0xb5f10/0xb61d0): a const member of the text record, no volatile, 48 (93 bytes; `this`
// in EBX, ret 4, 0xb5f10 82); the length scan as a `while` or a `do` 40 (same code), as a `for` with the walk in its body 60
// (0xb61d0 46); 30 random mixes of guard nesting, length local, compare spelling, increment, clamp, source form and result
// fall in the known classes 28 (start stays in EDX, walk EDI, 0xb5f10 6), 40, 48, 51 and 53. In retail the walk and counter
// take EDX and ECX first, which leaves start only a saved register; here start is coloured before the walk.
// Round 528: native int start/count and immutable destination formal, seven subsets
// 40 each; complete native graph with exhaustive result arms 48. Original signature,
// shared scanner/copy helper and whole body restored; saved-start/scan allocation remains.
// Round 140 (m3 lane 6, fast with 0xb5f10/0xb61d0): the sign tests swapped 40 (same code); the length
// in a local before the tests 46 (92 bytes, 0xb5f10 6); the inverted guard with a `result = false` arm 48
// (95 bytes, 0xb5f10 41); a `size = count + 1` local clamped and passed 53 (90 bytes, 0xb5f10 104); the
// result assigned from the whole test 44 (0xb5f10 41).
// Round 78 (m3 lane 9, fast with 0xb5f10/0xb61d0): the clamp as `if (count >= 0x200) count = 0x200;
// else count++;` 40, with `> 0x1ff` 40; the guard as `!(start < 0 || count <= 0 || ... > length)` 40;
// the length test as a second nested if 40 (all same code); start added before the increment 36
// (0xb5f10 6, as listed).
// DH313: Native scanner char/signed-char/unsigned-char/byte captures with original
// postincrement or separate pre-test advance: 40 each; shared find regresses to 15/70 bytes
// in all eight. NOT kept: no target gain and shared-user loss. Original helper/body restored.
// Lane 12 r86 (fast with 0xb5f10/0xb61d0): unused-local numbering probes (1-3, diagnostic) 40, so the
// start/counter registers are not a symbol-order tie; the scan helper as `while (length < 0x1ff &&
// *string++) length++` 40 (find still matches); indexed `string[length]` 60 (find 46, 0xb5f10 83);
// the length test as `length - start >= count` 34 (0xb5f10 6); the clamp as a ternary 53 (90 bytes);
// the copy source as `&string[start]` 28 (retail's shrink-wrapped EBX, but start arrives and stays
// in EDX; 0xb5f10 6, as the listed `string + start`).
// Round 124 (m3 lane 7, fast with 0xb5f10/0xb61d0): retail's 0xb5f10 calls set the count (EAX) before the start
// (ECX), so the declared start-then-count order already fits. The length helper returning `long` 40 (0xb61d0 1);
// a plain string parameter with one own-parameter volatile read at the `start +=` 40; result set in an else arm
// 48 (95 bytes, 0xb5f10 41); the helper's counter initialised in the for 40; `count >= 1` 38 (`cmp esi, 1`, no
// register change; not kept). Retail pushes EDI in the prologue (start lives there) and EBX only around the scan.
// P311: Shared bounded scanner !=511 / <=510 exhaustion, unsigned-long/long/word/short counters:
// P311: eight forms 40 at exact 100 bytes; shared find loses its match in every form.
// P311: NOT kept: no target gain and shared-user regressions. Original helper/body restored; saved-start and scan register split remain.
// M362: Existing copy helper terminator through direct CRT result/destination
// formal, long/unsigned-long/int/unsigned-int sizes: eight 53s at 90/100 bytes.
// NOT kept: target regresses and shared users rise (caller 13, constructor 29,
// formatter 1, connector 90); original copier, scanner and full body restored.
// M366: Canonical success signed-byte/short/word/int64 storage, false default then true-default rejection:
// 41/47/47/48/62/64/64/48. NOT kept: all regress; false defaults and true int64 also lose
// the matched caller. Original Boolean body restored; saved-start/scan register split remains.
// Round 86 (m3 lane 16, fast with 0xb5f10/0xb61d0): a plain `string` with `string + start`, all 12 parameter orders that keep
// string before destination: STCD STDC SCTD SCDT SDTC SDCT 41 (104 bytes), TSCD TSDC TCSD CTSD 48 (93 bytes), CSTD CSDT 42 (104),
// 0xb5f10 21-82 in all; the read `char const *text = string;` after the clamp in the same 12 orders 27 (0xb5f10 5-38; start
// EDX, walk EDI where retail has start EDI, walk EDX). Nested tests, named length, early `source` 21 with 0xb5f10 matched
// (entry and start EDI as retail, walk ECX, count EDX; one-use locals ahead of the compare, so not kept), with `unsigned long`
// length 21, with an `end` local too 21; `long end` before the scan 49 (94 bytes); source after the compare 32; `start +=`
// before the clamp 36; nested tests with the text read 27, named length with it 27, `length >= start + count` with it 28;
// named length with `string + start` 28, with `start +=` 40.
// Round 55 (m3 lane 15, fast with 0xb5f10/0xb61d0; one-file LTCG harness exact at 40 / 0xb5f10 0): count tested first with a text
// local and `start += (long)text` 39 (start arrives unsaved in EDI, 0xb5f10 35); `long source = start; source += (long)string;` 28
// (0xb5f10 6); `start = (long)&text[start]` from a text local 27 (0xb5f10 6); (start, count, string, destination) in every
// declaration and call with the text local 27 (237 bytes for 0xb5f10, 38); that order with a plain string 48 (93 bytes, 0xb5f10 82).
// Harness: plain string x orders (string/start/count/destination, string/start/destination/count, string/destination/start/count,
// start/string/count/destination, start/string/destination/count, start/count/string/destination) x `start += (long)string`,
// `string + start`, `start + (long)string` 41 or 48; on the string-first and start-first orders flat, nested, three-level with a
// length local, early returns, result set in an else arm, result from the whole test x the same sums and a text local 41-54, never
// string on the stack (it arrives in ECX or EBX and the scan's byte then takes AL); volatile string x the four orders x start or count
// tested first x sums 28-40. Compile-only: the declared order alone picks the sum's register (string first `add ebp, edi`, start
// first `add edi, ebp`, retail's direction). Probes (harness only, not kept): extra uses of the walk or of destination move
// the walk or start between scratch registers and start then arrives there, never in ECX with an EDI home, so re-weighting the scan
// does not reach retail's convention.
// M380: Shared copier short/word/int64/uint64 size contracts with native/void return:
// 39/39/56/56/39/39/56/56. NOT kept: narrow forms gain one and grow to 103 bytes;
// wide forms regress at 112 bytes. Original helper, scanner and complete body restored.
// AC559: Original signed-start admission/rejection, bool/byte/short/long: eight
// at 40, 100/100; caller and all file companions unchanged. NOT kept: no match
// or saved-start/scan repair; original sign guard, helpers and whole body restored.
// Round 153 (fast with 0xb5f10/0xb61d0): the add as pointer arithmetic, `start = (long)(string + start)` 40 (same code). On round 48's 21
// form (re-measured 21: start arrives in ECX and lives in EDI as retail, but the scan walks in ECX and counts in EDX, retail EDX/ECX, and
// the one-use source sum precedes the compare): the length helper as `while (length < 0x1ff && *string++) length++` 21 (same code); the
// helper walking a local declared after the counter 21 (0xb61d0 2); `count <= length - start` 35 (0xb5f10 35); a const length local 21.
// Round 88 (m3 lane 16, fast with 0xb5f10/0xb61d0; [0xb5f10]): nested tests + named length + early `source` 21 [match] at 100 bytes,
// aligned 13, masked 5, is the nearest shape (one-use locals, kept only on a match). On it: scan as `while` 21; scan with a named walk
// 21; length declared at the top 21; named end after the scan 21; `start + string` 21; orders TCSD 21 [38], TSCD 21 [2], CTSD 25 [41],
// CSTD 25 [41]. Source at the clamp 28 [6], reversed 28 [6], through a text read 27 [6], reversed 27 [6]; after the compare, reversed 32
// [6]; in the call, reversed 28 [6]. Early text read used at the copy 40 [35] (aligned 10, masked 6: count ECX and walk EDX as retail, but
// start arrives directly in EDI), with `start +=` 40 [35]; text read before the scan 48 (93 bytes) [84]; named end with `start +=` 40 [match]
// (aligned 20, masked 12); clamp written `count + 1 > 0x200` 53 (90 bytes) [104]. Plain string: early source 49 (92 bytes) [6]; source at
// clamp 41 (104) [35]; early text 41 (104) [35]; string kept through its own address: at clamp 40 (104) [6], text read 40 [6], reversed
// 40 [6], early 49 (92) [6], `start +=` 40 [6]. Left: count and walk swapped on the match-keeping form, or start's entry register on the other.
// Round 96 (m3 lane 12, fast with 0xb5f10/0xb61d0; [0xb5f10]): nested tests with the source before the named length 37 [84] (250-byte
// caller); the length named first, then the source, re-measured 21 [match] (walk ECX/count EDX, retail EDX/ECX), with its compare
// reversed 21 [match]; the copy's end named before the scan 40 [83] (96 bytes); the original compare reversed 40 [match]; the
// `start +=` ahead of the clamp 36 [6].
// Round 138 (m3 lane 7, fast with 0xb5f10/0xb61d0, both still matched in each): a 600 s permuter run on this body
// (78 variants, full and reduced image both 21) found nothing below 21; `result = true` before the copy call 27
// (101 bytes); the two sign tests as separate nested ifs 21; `count + start` in the compare 21; the early source
// pointer `const` 21 (all three the same code). In retail AL holds the false result through the scan, so only ECX
// and EDX are free for it: retail colours the counter first (ECX), ours the walk.
// Round 76 (m3 lane 13, fast with 0xb5f10/0xb61d0; [0xb5f10]; compile-only /Gd harness with every parameter on the stack):
// the natural body (named length, `string + start` at the call) is retail's body in that pass (start EDI, walk EDX, count
// ECX), and LTCG then passes start in EDX. Over eight forms a start loaded into EDI in that pass arrives in the first of
// ECX/EDX written after entry (the walk's), so the 21 form's early source is what puts the walk in ECX; by that rule retail's
// own body would also give EDX. New: a named end after the scan with the source at the call 28 [6]; the guards as `break`s
// out of `do { } while (false)` 28 [6], with the length call in the test 28 [6], with the early source 21 [match] (the 21
// code). Re-measures: `long end` before the scan 49 [83]; `length >= start + count` 29 [6]; one condition with the length
// call 28 [6]; nested tests, named length and `start += (long)string` after the clamp 40 [match].
// Round 110 (m3 lane 16, fast with 0xb5f10/0xb61d0, both matched each time; [0xb5f10] unchanged at match): on main's 21
// body, true facts `__assume(start >= 0 && count > 0)` inside the guard, `__assume(length >= 0 && length < 0x200)` after the
// scan, `__assume(count > 0 && count <= 0x200)` after the clamp 21/21/21; the source declared (not assigned) before the
// length 21; the length renamed to sort after source, start and string 21. All the same code: value facts, declaration
// order and names leave the walk ECX / count EDX split. The rename repeats round 118's names finding; no untried natural
// source form was found this round.
// Round 237 (m3 lane 3, fast with 0xb5f10/0xb61d0; [0xb5f10], 0xb61d0 matched each time): a plain string parameter with
// a second forceinline length helper taking the pointer by reference (the counter then starts before the pointer is
// read) 40 [35] at 104 bytes with the natural body, 49 [6] with the early source: the reference is folded and the string
// arrives in ECX, start in EDX, `ret 4`. A plain string read through its own address at the scan 40 [6], at both uses
// 40 [6] (the string stays a stack argument but is loaded once into EBX at entry, the scan's byte in AL and two exits).
// The body in a forceinline inner function called from this one (inline parameter copies of start and count) 46 [94],
// with the unnamed length 46, with `start += (long)string` 46, the inner plain inline 130 [31], with the volatile
// parameter on this function 52 [84]. Re-measured: the natural body with the volatile parameter 28 [6] (start arrives
// and stays in EDX, the walk in EDI); `start += (long)string` after the clamp 40 [match] (start stays in its arrival
// ECX, `add ecx, [string]`, the walk in EDI). In retail start is moved to EDI at entry and the two loop values take
// ECX/EDX, so start is coloured after them; here it keeps a scratch register unless the early source is present.
// Round 119 (m3 lane 9, fast with 0xb5f10/0xb61d0; [0xb5f10]): the length helper walking a local copy declared after
// the counter, with the natural body (source at the call) 40 [35] (0xb61d0 2): the body then has retail's start EDI,
// walk EDX, count ECX, but start arrives directly in EDI (no entry move); the same helper on the kept 21 form 21 [match]
// (0xb61d0 2: its retail sets the walk before the counter, so the helper's order is right). Natural body with
// `__assume(start >= 0)` inside the guard 28 [6]; with `count + start <= length` 28 [6]; the length taken before the
// sign tests 50 (92 bytes) [35].
// @retail 0xb6220
bool function_b6220(char const *volatile string, long start, long count, char *destination)
{
	bool result = false;
	if (start >= 0 && count > 0)
	{
		long length = (long)string_length_512_ab(string);
		char const *source = string + start;
		if (start + count <= length)
		{
			count++;
			if (count > 0x200)
				count = 0x200;
			upload_copy_ab(destination, source, count);
			result = true;
		}
	}
	return result;
}

// @retail 0xb6290
char *function_b6290(char *buffer, char const *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0xf, format, arguments);
	buffer[0xf] = 0;
	return buffer;
}


struct s_upload_file_ab
{
    byte unknown00[0x108];
    HANDLE handle;
    dword position;
};



// @retail 0xb62b0
s_upload_ab *function_b62b0(s_upload_ab *upload)
{
    char *content_type = upload->content_type;
    char *filename = upload->filename;
    char volatile &path = upload->path[0];
    path = 0;
    char volatile &header = upload->header[0];
    header = 0;
    char volatile &part = upload->part[0];
    part = 0;
    char volatile &ending = upload->ending[0];
    ending = 0;
    char volatile &type_prefix = content_type[0];
    type_prefix = 0;
    char volatile &filename_prefix = filename[0];
    filename_prefix = 0;
    char volatile &extra_prefix = upload->extra_headers[0];
    extra_prefix = 0;
    upload->kind = 0;
    upload_copy_ab(content_type, "text/plain", sizeof(upload->content_type));
    upload_copy_ab(filename, "blob", sizeof(upload->filename));
    return upload;
}

static __forceinline bool upload_seek_ab(s_upload_file_ab *file)
{
    file->position = SetFilePointer(file->handle, 0, 0, FILE_BEGIN);
    bool result = file->position != 0xffffffff;
    if (!result)
    {
        GetLastError();
        SetLastError(0);
    }
    return result;
}

// @retail 0xb6570
bool function_b6570(s_upload_ab *upload)
{
    bool result = true;
    if (upload->kind == 2)
    {
        s_upload_file_ab *file = upload->file;
        if (file->position)
        {
            if (!upload_seek_ab(file))
                result = false;
        }
    }
    upload->position = 0;
    return result;
}

// @retail 0xb65f0
void function_b65f0(s_upload_ab *upload)
{
    upload_copy_ab(upload->ending, "\r\n--BUNGIEr0x0rz--\r\n", sizeof(upload->ending));
    unsigned long length = 0;
    char const *string = upload->ending;
    for (; length < 0x7f; length++)
        if (!*string++) break;
    upload->ending_length = length;
    csprintf_256(upload->part, "--BUNGIEr0x0rz\r\nContent-Disposition: form-data; name=\"upload\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n", upload->filename, upload->content_type);
    upload->part_length = function_0b6720(upload->part);
    csprintf_1024(upload->header, "POST %s HTTP/1.0\r\nContent-type: multipart/form-data; boundary=BUNGIEr0x0rz\r\nContent-Length: %d\r\n%s\r\n", upload->path, upload->part_length + upload->ending_length + upload->file_length, upload->extra_headers);
    length = 0;
    string = upload->header;
    for (; length < 0x3ff; length++)
        if (!*string++) break;
    upload->header_length = length;
}


static char g_4530e8[] = "Content-Length: ";

// @retail 0xb5f10
bool function_b5f10(char const *response, bool *complete)
{
    char buffer[0x200];
    char length_text[0x200];
    char body[0x200];
    bool result = false;
    upload_copy_ab(buffer, response, sizeof(buffer));
    long begin = ((s_text512_ab const *)buffer)->find(0, g_4530e8);
    *complete = false;
    if (begin != NONE)
    {
        begin += strlen(g_4530e8);
        long end = ((s_text512_ab const *)buffer)->find(begin, "\r\n");
        if (end != NONE)
        {
            length_text[0] = 0;
            if (function_b6220(buffer, begin, end - begin, length_text))
            {
                body[0] = 0;
                long size = atoi(length_text);
                if (function_b6220(buffer, end + 4, size, body))
                {
                    result = true;
                    *(byte *)complete = function_b6190(body, "DONE");
                }
            }
        }
    }
    return result;
}

struct s_http_connection_ab
{
    s_type_99af70 address;
    char hostname[16];
    s_transport_endpoint internal_endpoint;
    s_transport_endpoint *endpoint;
    byte unknown30[4];
    char path[0x80];
    byte unknownb4[0x634 - 0xb4];
    long upload_header_length;
    long upload_part_length;
    long upload_ending_length;
    byte unknown640[0x96c - 0x640];
    long upload_file_length;
    long upload_position;
    char response[0x200];
    long response_length;
    long attempt_count;
    long state;
    s_http_connection_ab();
    bool connect(long address, word port, char const *path, bool flag);
    bool receive(bool *complete);
    bool send();
};

bool function_b51b0(s_transport_endpoint *endpoint, s_type_99af70 const *address);

bool function_7af40(s_type_99af70 const *address);

/* points the connection at an IPv4 address and port and opens its endpoint;
   on success counts the attempt and keeps the request path */
// The endpoint local for the three clearing stores beside `connection->endpoint` for the flag and the
// open call, and the open result tested twice, are retail's code shape: kept only with the byte match.
// @retail 0xb5d40
bool s_http_connection_ab::connect(long address, word port, char const *path, bool flag)
{
    s_http_connection_ab *connection = this;
    connection->address.address_length = 4;
    connection->address.ipv4_address = htonl(address);
    connection->address.port = htons(port);
    connection->hostname[0] = 0;
    function_b6290(connection->hostname, "%d.%d.%d.%d", (byte)address, (byte)(address >> 8), (byte)(address >> 16), (byte)(address >> 24));
    if (function_7af40(&connection->address))
    {
        s_transport_endpoint *endpoint = connection->endpoint;
        endpoint->socket = NONE;
        endpoint->flags = 0;
        endpoint->type = 4;
        if (flag) connection->endpoint->flags |= 0x40;
        bool opened = function_b51b0(connection->endpoint, &connection->address);
        if (opened)
            connection->attempt_count++;
        else
            transport_endpoint_close(connection->endpoint);
        if (opened)
        {
            connection->state = 1;
            upload_copy_ab(connection->path, path, sizeof(connection->path));
            return true;
        }
    }
    return false;
}

// @retail 0xb5e40
bool __stdcall function_b5e40(void *block)
{
    s_http_connection_ab *connection = (s_http_connection_ab *)block;
    bool result = true;
    if (connection->state)
    {
        if (!function_b6570((s_upload_ab *)((byte *)connection + 0x30)))
            result = false;
        transport_endpoint_close(connection->endpoint);
        connection->attempt_count--;
        connection->state = 0;
    }
    return result;
}

// @retail 0xb6180
bool __stdcall function_b6180(void *block)
{
    return function_b5e40(block);
}

void transport_register_transition_functions(transport_transition_function startup, transport_transition_function shutdown, transport_transition_function reset, void *context);

/* an idle connection, with its close (function_b6180) registered as a transport shutdown
   function; retail expands transport_register_transition_functions (src/transport.cpp) here.
   Retail's only caller is the dynamic initializer of the upload connection g_546a8c
   (src/unknown_0b3d30.cpp), 0x376bf0, which passes the connection in edi */
// @retail 0xb5cd0
s_http_connection_ab::s_http_connection_ab()
{
    hostname[0] = 0;
    function_b62b0((s_upload_ab *)((byte *)this + 0x30));
    memset(&internal_endpoint, 0, sizeof(internal_endpoint));
    endpoint = &internal_endpoint;
    state = 0;
    transport_register_transition_functions(0, (transport_transition_function)function_b6180, 0, this);
}

short function_b4fa0(s_transport_endpoint *endpoint, void *buffer, short length);

// @retail 0xb60e0
bool s_http_connection_ab::receive(bool *complete)
{
    bool result = false;
    char *buffer = response + response_length;
    *complete = false;
    long received = function_b4fa0(endpoint, buffer, (short)(0x1ff - response_length));
    if (received > 0)
    {
        response_length += received;
        response[(dword)response_length] = 0;
        bool done = false;
        if (function_b5f10(response, &done))
        {
            if (done)
            {
                result = true;
                *complete = result;
            }
        }
        else if (response_length < 0x1ff)
            result = true;
    }
    else if (received == -2)
        result = true;
    return result;
}

bool function_b6320(s_upload_ab *upload, byte *buffer, long maximum, long *written);

short function_b5000(s_transport_endpoint *endpoint, void const *buffer, short length);

/* sends the upload a buffer at a time until everything is out (then waits for the response) or the
   endpoint would block. A would-block leaves the loop with the result still false: the test after the
   loop tells it from completion and rewinds the position past the buffer that was not sent. */
// @retail 0xb6000
bool s_http_connection_ab::send()
{
    bool result;
    byte buffer[0x518];
    long position;
    do
    {
        position = upload_position;
        long written = 0;
        result = false;
        if (!function_b6320((s_upload_ab *)((byte *)this + 0x30), buffer, sizeof(buffer), &written))
            goto return_exit;
        short sent;
        if (!written)
            sent = 0;
        else
            sent = function_b5000(endpoint, buffer, (short)written);
        if (sent < 0)
        {
            if (sent != -2)
                goto return_exit;
            break;
        }
        result = true;
        if (sent < written)
            upload_position = position + sent;
    } while (upload_position != upload_file_length + upload_ending_length + upload_part_length + upload_header_length);
    if (result)
    {
        response_length = 0;
        state = 3;
    }
    else
    {
        upload_position = position;
        result = true;
    }

return_exit:
    return result;
}

bool transport_endpoint_test_connection(s_transport_endpoint *endpoint, bool *connected);

// @retail 0xb5e90
bool function_b5e90(s_http_connection_ab *connection, bool *complete)
{
    bool result = false;
    *complete = false;
    if (connection->state == 1)
    {
        bool connected = false;
        if (transport_endpoint_test_connection(connection->endpoint, &connected))
        {
            result = true;
            if (connected)
                connection->state = 2;
        }
    }
    else if (connection->state == 2)
    {
        if (connection->send())
            result = true;
    }
    else if (connection->state == 3)
    {
        if (connection->receive(complete))
            result = true;
    }
    if (!result || *complete)
        function_b5e40(connection);
    return result;
}
