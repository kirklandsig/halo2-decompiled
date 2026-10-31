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

// score 40 (was 40): the string pointer and the character counter are kept in different registers
// here, and an extra register is pushed and restored around the copy loop. Tried: count before
// start in the parameter list 40; the length in a named local after the sign tests 40;
// string + start with start unmodified 28 but start then arrives in edx (0xb5f10 differs);
// that with a plain string parameter 41; start = string + start 40.
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
// @retail 0xb6220
bool function_b6220(char const *volatile string, long start, long count, char *destination)
{
	bool result = false;
	if (start >= 0 && count > 0 && start + count <= (long)string_length_512_ab(string))
	{
		count++;
		if (count > 0x200)
			count = 0x200;
		start += (long)string;
		upload_copy_ab(destination, (char const *)start, count);
		result = true;
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

// score 32 (was 32): the flag is read before the three endpoint field stores instead of after
// tried the flag bit set through a byte cast of the flags word, as retail
// the flag is read before the three endpoint field stores instead of after
// them, the endpoint pointer is kept in a register here instead of being reloaded from the
// connection, and the attempt counter is incremented in a register instead of in place. Retail
// reads connection->endpoint once for the three clearing stores and again for the flag (`or
// byte`, as `flags |= 0x40` on the word gives) and for the open call.
// Tried: the flag bit set through a byte cast of the flags word 30 (an address cast: not kept);
// open inline 32; open noinline 32; valid inline 32; valid noinline 90; both inline 32; open
// inline/valid noinline 90; open noinline/valid inline 32; both noinline 90; an early
// invalid-address exit with the failed open closing and returning first 35 (245 bytes); the copy
// helper as plain inline 32 (identical); the state stored before the counter 33; the endpoint read
// for the flag and the open call with the existing copy 111 (the split counter takes EBX, the
// address then arrives in EAX; 0xb40b0 61); with that, the state store first, `++`, `+= 1` and
// member spellings 111 each, the copy before the counter 35; with strncpy and the terminator written
// out the counter stays an in-place `inc` and the convention holds 17 (destination ECX and the
// terminator `[edi + 0xb3]`, retail ESI and `[esi + 0x7f]`; 249 bytes), and on that an early
// `return false` 17, an inner `return false` 17, an else around the close 17, a single result 106,
// the address check's loop with a single exit 17; strncpy alone 36; an early `return false` for a
// bad address 32; `count = count + 1` 32; the failed open closing and returning first 33;
// `connection->endpoint` read at each use 119 (0xb40b0 61); the endpoint read before the address
// check 64. A compile-only copy splits the counter whenever the copy's destination survives the
// call in a callee-saved register, so it cannot screen this.
// Round 84: the endpoint local for the clears with `connection->endpoint` for the flag and the
// open call gives retail's endpoint code exactly but splits the counter into EBX (111, 0xb40b0 61);
// with the copy before the state store the counter is retail's in-place `inc` and only the state
// store differs (after the call, retail before it): 15 at 245 bytes, but the mixed reads can only
// be kept with a match. Other orders of the counter, state and copy 35/35/37/111; the flag alone
// re-read 42; the open alone re-read 109; `= count + 1` 111.
// tried the flag bit set through a byte cast of the flags word, as retail
// tried the flag bit set through a byte cast of the flags word, as retail
// sets it in the low byte, 30 (an address cast for 2 points: not kept); the flag is still read
// before the three field stores instead of after them, the endpoint pointer is kept in a register
// here instead of being reloaded from the connection, and the attempt counter is incremented in a
// register instead of in place. Earlier: the connection state is cleared with the three field
// stores in a different order here, the flag is written as a word here and or-ed into the field in retail, and the two
// calls take their arguments in a different order.
// DL180: open inline 32; open noinline 32; valid inline 32; valid noinline 90; both inline 32;
// open inline/valid noinline 90; open noinline/valid inline 32; both noinline 90. Original
// executable body restored.
// Round 35: an early invalid-address exit with the failed open closing and returning first 35 at
// 245 bytes (the open test inverts; the false exits then go to the close path's return); the copy
// helper as plain inline 32 (identical); the state stored before the counter 33; the flag and the
// open call reading the connection's endpoint again 111 (the split counter takes ebx, the address
// then arrives in eax and 0xb40b0 stops matching). A compile-only copy without link-time code
// generation splits the counter even with the copy written out, so it cannot screen this.
// Round 5: retail reads connection->endpoint once for the three clearing stores and again for the
// flag (`or byte`, as `flags |= 0x40` on the word gives) and for the open call. Those reads with
// the existing copy 111 (the counter's split temporary takes EBX and the address then arrives in
// EAX; 0xb40b0 61); the state store first, `++`, `+= 1` and member spellings 111 each; the copy
// before the counter 35; with strncpy and the terminator written out the counter stays an in-place
// `inc` and the convention holds: 17 (the copy's destination is then ECX and the terminator
// `[edi + 0xb3]`, retail ESI and `[esi + 0x7f]`; and the address check's false exits take a long
// jump to the final return, where retail's go to the check's own return after its loop: 249
// bytes); with that form an early `return false` 17, an inner `return false` 17, an else around the
// close 17, a single result 106; the address check's loop with a single exit 17 (0x7aec0 breaks).
// strncpy alone 36. A compile-only probe splits the counter whenever the destination survives the
// call in a callee-saved register (existing copy, a local pointer, an inline wrapper, a separate
// block), so retail's in-place `inc` with an ESI destination still needs its source form.
// Lane 8 r79: an early `return false` for a bad address 32; `count = count + 1` 32; the failed
// open closing and returning first 33; `connection->endpoint` read at each use 119 (0xb40b0 61);
// the endpoint read before the address check 64.
// Round 33: with retail's endpoint reads (one read for the three stores, re-reads for the flag and
// the open call) the body is retail's except the counter, split into load/inc/store in EBX, which
// moves the address to EAX (111, 0xb40b0 lost); a compiled-out `do {} while (false)` after the
// increment 32, with the reads 111; the copy helper with a dword size and the reads 111; the
// failed open closing and returning first with the reads 101.
// the flag is read before the three endpoint field stores instead of after
// them, the endpoint pointer is kept in a register here instead of being reloaded from the
// connection, and the attempt counter is incremented in a register instead of in place. Retail
// reads connection->endpoint once for the three clearing stores and again for the flag (`or
// byte`, as `flags |= 0x40` on the word gives) and for the open call.
// Tried: the flag bit set through a byte cast of the flags word 30 (an address cast: not kept);
// open inline 32; open noinline 32; valid inline 32; valid noinline 90; both inline 32; open
// inline/valid noinline 90; open noinline/valid inline 32; both noinline 90; an early
// invalid-address exit with the failed open closing and returning first 35 (245 bytes); the copy
// helper as plain inline 32 (identical); the state stored before the counter 33; the endpoint read
// for the flag and the open call with the existing copy 111 (the split counter takes EBX, the
// address then arrives in EAX; 0xb40b0 61); with that, the state store first, `++`, `+= 1` and
// member spellings 111 each, the copy before the counter 35; with strncpy and the terminator written
// out the counter stays an in-place `inc` and the convention holds 17 (destination ECX and the
// terminator `[edi + 0xb3]`, retail ESI and `[esi + 0x7f]`; 249 bytes), and on that an early
// `return false` 17, an inner `return false` 17, an else around the close 17, a single result 106,
// the address check's loop with a single exit 17; strncpy alone 36; an early `return false` for a
// bad address 32; `count = count + 1` 32; the failed open closing and returning first 33;
// `connection->endpoint` read at each use 119 (0xb40b0 61); the endpoint read before the address
// check 64. A compile-only copy splits the counter whenever the copy's destination survives the
// call in a callee-saved register, so it cannot screen this.
// Round 84: the endpoint local for the clears with `connection->endpoint` for the flag and the
// open call gives retail's endpoint code exactly but splits the counter into EBX (111, 0xb40b0 61);
// with the copy before the state store the counter is retail's in-place `inc` and only the state
// store differs (after the call, retail before it): 15 at 245 bytes, but the mixed reads can only
// be kept with a match. Other orders of the counter, state and copy 35/35/37/111; the flag alone
// re-read 42; the open alone re-read 109; `= count + 1` 111.
// Actual open completion from false entry or validity result, bool/long carrier, direct/shared
// endpoint initialization 106/100/128/124/109/105/131/128. False joins, endpoint flag scheduling
// and split attempt increment remain; original body restored.
// Round 109 (lane 5, fast), all on retail's endpoint reads (local for the three stores,
// `connection->endpoint` for the flag and the open call): the copy's source read once through the own
// parameter's address 111; the state store inside the copy's destination argument (an inline member
// returning the path buffer) 111; the path read into a named local between the counter and the state
// store 111; the copy helper returning void 111; the three stores through an inline taking
// `connection->endpoint` and the type (the existing private endpoint initializer, without its null
// test) 111. The counter's load/inc/store split (EBX, address then in EAX) survives each; only a
// written-out strncpy keeps the in-place `inc` (listed, 17).
// Round 44 (lane 16, fast with 0xb40b0/0xb5f10): with retail's endpoint reads and the copy before
// the state store (round 84's 15) only the state store differs: VC puts it after the strncpy call
// and shares `eax = 1` with the return value. Whenever the state store precedes the call in the
// source, VC sinks it and the counter's store together to the call, splitting the `inc` (EBX,
// 0xb40b0 61): with the reads, state stored first 111; the increment through a pointer to the
// field 111; plain member accesses without the `connection` alias 111; the copy's source as a
// volatile read of the own path parameter 111. Without the reads, counter/copy/state order 36.
// Round 132 (an LTCG env link with 0x7af40's object reproduces the full listing): with retail's
// endpoint reads, the in-place `inc` survives when `connection` is used after the copy call (a
// diagnostic store there gives retail's code plus that store); a destination pointer kept across the
// call with no later use of `connection` splits it into EBX and moves the address to EAX. On the
// reads: the copy helper with a dword or unsigned size, plain inline, returning void, keeping
// strncpy's result, writing the terminator through `destination + size - 1` or a pointer computed
// first, a destination local, or that local declared before the endpoint stores 111 each; a template
// overload on the array 31 (kept out of line: in-place `inc`, destination ESI); one result carried
// through the open call 94; the open test inverted with the close and return first 101; the counter
// after the copy 37; the counter, state and copy in an inline member with the flag alone re-read 30
// (env); the terminator through `connection` with a destination local 17 (as the written-out copy).
// Round 54 (lane 12, fast with 0xb40b0/0xb5f10; env link = the build's listing): the copy, then the
// counter and the state 36 (248 bytes); counter, copy, state 36; the copy first with retail's endpoint
// reads 35 (252); `flags = flag ? 0x40 : 0` 28 (244; neg/sbb select, not retail's or-byte); a
// pre-increment 32 (same code). Env only: the three stores through a struct-initialized endpoint copied
// to `*connection->endpoint` 92 instructions (a stack temporary); the copy's terminator through its
// returned destination 89.
// Round 36 (lane 10, fast with 0xb40b0), on retail's endpoint reads: the copy's source read once through
// the own parameter's address 111 (the path load still comes before both sunk stores); counter, copy,
// state 15 (245 bytes, 0xb40b0 matches; only the state store after the call remains, sharing EAX = 1 with
// the return); the destination named before the counter 111; the path read into a named source between
// the counter and the state store 111. VC splits the counter whenever any store precedes the call.
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
        if (flag) endpoint->flags |= 0x40;
        if (function_b51b0(endpoint, &connection->address))
        {
            connection->attempt_count++;
            connection->state = 1;
            upload_copy_ab(connection->path, path, sizeof(connection->path));
            return true;
        }
        transport_endpoint_close(connection->endpoint);
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

// score 10 (was 10): retail lays the shared false exit out after the would-block test and the
// restore exit last; ours puts the restore exit first. Tried: `return true` in the would-block arm 44;
// direct returns everywhere 72 (retail's block order, but the results fold to constants); the
// plain failure as `return false` behind `if (sent != -2)` 20; looping while the iteration
// succeeded with the completion guarded after the loop 44; success arm ending in `continue` 10;
// `if (sent != -2) return result;` then the restore and the goto 25 (retail's order, folded false);
// success arm first in an if/else 10; position declared before the loop 10. Retail's shared
// return (BL, not folded) follows the would-block test, so it comes first in its source order.
// Round 90: accepted-send else/continue with the restore after it 10; the nonnegative-guard
// equivalent 10; result from the sentinel before the conditional restore 74; result assigned
// before the restore 10; both negative paths returning result 37; only the blocked path returning
// result 44; result from a second sentinel test after the restore 75; success-first direct negative
// return 30; error/sentinel else-if chain 50. Original body restored.
// @retail 0xb6000
bool s_http_connection_ab::send()
{
    bool result;
    byte buffer[0x518];
    do
    {
        long position = upload_position;
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
            if (sent == -2)
            {
                upload_position = position;
                result = true;
            }
            goto return_exit;
        }
        result = true;
        if (sent < written)
            upload_position = position + sent;
    } while (upload_position != upload_file_length + upload_ending_length + upload_part_length + upload_header_length);
    response_length = 0;
    state = 3;

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
