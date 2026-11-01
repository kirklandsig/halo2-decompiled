#include "unknown_11c920.h"
#include "files.h"
#include <string.h>

// @flags /O2 /Gr

bool function_1249f0(long memory_unit, char *drive_letter);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_122810(char *string, const char *suffix);
const char *saved_game_type_get_storage_name(long type);
bool function_1368f0(s_type_acf665 *file);

// @retail 0x216400
char *function_216400(long memory_unit, char *buffer, short capacity)
{
	char drive_letter;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(buffer, capacity, "%c:\\", drive_letter);
	else
		buffer[0] = 0;
	return buffer;
}

// @retail 0x216d70
bool function_216d70(char *path, const char *directory, long type)
{
	strncpy(path, directory, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
	return true;
}

struct s_storage_file_reference
{
	dword signature;
	word flags;
	short location;
	char path[0x100];
	__int64 state;
};

// @retail 0x217590
long function_217590(long memory_unit)
{
	char root[8] = "";
	char drive_letter;
	long count = 0;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(root, sizeof(root), "%c:\\", drive_letter);
	else
		root[0] = 0;
	XGAME_FIND_DATA data;
	HANDLE find = XFindFirstSaveGame(root, &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			count++;
		}
		while (XFindNextSaveGame(find, &data));
		XFindClose(find);
	}
	return count;
}



void __stdcall function_216e50(word const *display_name, long type, char *language, word *name);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);

struct s_save_entry_2164c0
{
    char directory[20];
    word name[17];
    byte unknown36[2];
    long type;
    byte unknown3c;
    char language;
    byte unknown3e[2];
};

struct s_save_list_2164c0
{
    long count;
    s_save_entry_2164c0 entries[4096];
};

PRIVATE inline s_save_entry_2164c0 *new_save_entry_2164c0(s_save_list_2164c0 *list)
{
    s_save_entry_2164c0 *result = NULL;
    if (list->count != 4096)
    {
        result = &list->entries[list->count++];
    }
    return result;
}

// score 20 (was 20): the helper returns the new element (NULL when the list is full) and the caller copies
// into it: retail's length (662), list in ECX and retail's format-call and enumeration registers; only
// the append's own instructions differ: retail materializes the room test (`setne al; test al, al`) with
// the count in ESI copied to EAX, `lea edx, [eax + 1]` and the shift on the copy, and no NULL test.
// Earlier (32): frame 0x8ac and extent agree (659 bytes plus 3 of fill); only registers differ:
// the append's list/count/result (ours edx/eax/cl, retail ecx/esi/al, with the count copied to eax
// and the new count stored before the copy) and the scratch pairs before the format call,
// XFindNextSaveGame and XFindClose. Deleting the append alone makes those pairs retail's; every form
// with a materialized result gives ours, every form without one gives retail's list ECX but no
// setne/test. A byte scan finds retail's append sequence nowhere else.
// Tried: memcpy into `&entries[count++]` 32 (kept, from 51); file path/directory/both receivers
// 51/51/51; conditional character store 69; capacity first 54/41; capacity nonzero 51; short type
// index 189; no parameter address 51/32 (the dead `(void)&list` removed gives the same code, r21 kept);
// file-reference/all helpers 233/233; append names, force-inline and buffer names 51; source step at
// character read 72; const handle/unsigned conversion capacity 51; indexed conversion 63; full path
// copy 110; aggregate-zero file 53; unsigned append count, entry memcpy, enumeration names 51; append
// success result 54; capacity return 54; counted conversion 229; conversion end pointer 58; entry
// lifetime 51; entry zero initializer 199; file lifetime 51; drive-validation return 51; shared
// character conversion 51; element pointer for the slot 32; post-increment index local 51; index then
// count store 51; found flag with the found arm first 51/32; helper parameters swapped 32; force-inline
// helper 32; count local 44; increment first 44; pointer arithmetic slot 32; count reference 32; append
// as a list method 32; conversion local orders 34-36; type/find/data declared at the top 32; NONE-type
// test before the found arm 71/73; format and name locals 32; existing root, path and ASCII helpers 32
// (kept); post-copy success/early-capacity exits 54/54; wide result 51; list/entry/both references
// 32/32/32; count plus array pointer/reference inputs 32/32; root library clear 207; full character
// value 39; character read at each use 73; earlier ASCII buffer declaration 32; append record value 89;
// clear/full character 209; full character/buffer 39; clear/direct reads/buffer 211; explicit
// count/publication memcpy 51; list increment with old count 51; count success/failure results 54/54;
// character first 32; native file length 131; root aggregate zero 32; count local stored before the
// copy 51 (retail's order, no count copy); that with an element pointer 51; count through a pointer
// with an index local 51; count local with a byte-offset destination 33; index local stored first 51;
// loop condition on the append's result 177; break out of the type loop then `if (type < 11)` 64,
// diagnostic arm first 76; `list->count++` then copy into `entries[count - 1]` 53; own-parameter
// reference 32; named count read once with the new count stored first 51; the found block placed
// first 32; direct append with two-use result 32; caller append result 32; split helper result
// declaration 32; enumeration/entry receivers 32; explicit diagnostic addresses 32; mutable append
// input and sizeof(*entry) 32; early `return false` with a named index 54; named index then result 51;
// result then named index 51; early return with `entries[count++]` 54; loop flag in the do/while
// condition 51; append with early `return false` 46; `count < 4096` 32; result preset false and set in
// the arm 46; struct assignment for the memcpy 51; index local read after the test 51; caller's result
// in a named local 32. Harness-only (same allocation as the plain forms): helper in another file, not
// inline, static, generic element-size/count-pointer helpers, nested room/index helpers, list/entry
// references, written-out append, return from the loop.
// Round 21 (one-file LTCG harness, exact for this function): 48 append forms (four result, four
// increment and three copy spellings) all keep count EAX and result CL; `slot = count++` then the copy
// (the matched 0x216990's form) 51; copy then `count = slot + 1` 33; a separate room test 32; an inline
// terminated-copy helper for both strncpy pairs 32; entry type stored after the name call 34; leaving
// the callees' files out of the harness changes nothing.
// frame 0x8ac and extent agree (659 bytes plus 3 of fill); the append's
// list/count/result registers differ (ours EDX/EAX/CL; retail ECX/ESI/AL, the count copied to EAX
// after the test and the new count stored before the copy), and the scratch pairs before the
// format call, XFindNextSaveGame and XFindClose follow from it: deleting the append alone gives
// retail's format-call and enumeration registers (deleting the dead flag block, the ASCII
// conversion, the 0x216e50 call or the path block does not). Every form with a materialized
// result gives ours; every form without one gives retail's list ECX and both scratch pairs but no
// setne/test. A byte scan finds retail's sequence nowhere else. The append's memcpy into
// `&entries[count++]` gave retail's index copy (51 -> 32, kept); the existing root, path and
// ASCII helpers replace their written-out bodies (kept).
// Tried: file path/directory/both receivers 51/51/51; conditional character store 69; capacity
// first 54 (41 once); capacity nonzero 51; short type index 189; no parameter address 51/32 (the
// dead `(void)&list` removed: same code); file-reference/all helpers 233/233; append names,
// force-inline helper and buffer names 51/32; source step at character read 72; const
// handle/unsigned conversion capacity 51; indexed conversion 63; full path copy 110; aggregate-zero
// file 53; unsigned append count, entry memcpy, enumeration names 51; append success result 54;
// capacity return 54; counted conversion 229; conversion end pointer 58; entry lifetime 51; entry
// zero initializer 199; file lifetime 51; drive-validation return 51; shared character conversion
// 51; element pointer for the slot 32; post-increment index local 51; index then count store 51;
// found flag with the found arm first 51/32; helper parameters swapped 32; count local 44;
// increment first 44; pointer arithmetic slot 32; count reference 32; append as a list method 32;
// conversion local orders 34-36; type/find/data declared at the top 32; NONE-type test before the
// found arm 71/73; format and name locals 32; explicit count/publication memcpy 51; list increment
// with old count 51; count success/failure results 54/54; character first 32; native file length
// 131; root aggregate zero 32; post-copy success/early-capacity exits 54/54; wide result 51;
// list/entry/both references 32/32/32; count plus array pointer/reference inputs 32/32; root
// library clear 207; full character value 39; character read at each use 73; earlier ASCII buffer
// declaration 32; append record value 89; clear/full character 209; full character/buffer 39;
// clear/direct reads/buffer 211; count local stored before the copy 51 (retail's order, no count
// copy); that with an element pointer 51; count through a pointer with an index local 51; count
// local with a byte-offset destination 33; index local stored first 51; loop condition on the
// append's result 177; break + if (type < 11) append/else diagnostic 64; diagnostic arm first 76;
// increment before the copy into [count - 1] 53; the list read through its own reference 32; named
// count with the new count stored first 51; the found block placed first 32; direct append with
// two-use result 32; caller append result 32; split helper result declaration 32; enumeration/
// entry receivers 32; explicit diagnostic addresses 32; mutable append input and sizeof(*entry) 32.
// The existing root, path and ASCII helpers replace their written-out bodies (kept).
// Round 15: append with early `return false` 46; `count < 4096` 32; result preset false and set in
// the arm 46; struct assignment for the memcpy 51; count local with the new count stored first 51;
// index local read after the test 51; caller's result in a named local 32; the dead `(void)&list`
// removed 32 (same code, plain; restored, no gain).
// zero initializer 199; file lifetime 51; drive-validation return 51; element pointer for the slot
// 32; post-increment index local 51; index then count store 51; found flag with the found arm first
// orders 34-36; type/find/data declared at the top 32; NONE-type test before the found arm
// 71/73; format and name locals 32; the existing root, path and ASCII helpers in place of their
// written-out bodies 32 (kept).
// diagnostic arguments, append count/result and enumeration registers differ.
// Frame 0x8ac and extent agree; append count/result/list and
// diagnostic/enumeration scratch rotation remain. Post-copy success/early-capacity exits 54/54;
// wide result 51; list/entry/both references 32/32/32; count plus array pointer/reference inputs
// 32/32.
// diagnostic, append and enumeration scratch-register rotation remains; root library clear
// 207f; full character value 39f; character read at each use 73f; earlier ASCII buffer declaration 32f;
// append record value 89f; clear/full character 209f; full character/buffer 39f; clear/direct reads/buffer
// 211f.
// File path/directory/both receivers 51/51/51; conditional character store 69;
// capacity first 54; capacity nonzero 51; short type index 189; no parameter address 51.
// conversion addresses, append list/count/result and final call registers differ.
// Existing root/path helpers 51/51; file-reference/all helpers 233/233;
// append names/force-inline and buffer names 51/51/51; source step at character read 72.
// Earlier: conversion addresses, append and enumeration registers still differ.
// conversion addresses, append and enumeration registers still differ.
// Const handle/unsigned conversion capacity 51; indexed conversion 63; full path copy 110;
// aggregate-zero file 53; unsigned append count/entry memcpy/enumeration names 51.
// matching 0x8ac frame; conversion scratch registers, append count/result homes
// and enumeration argument registers differ. Earlier helper/index/loop forms excluded; tried append
// success result 54; capacity return 54; counted conversion 229; conversion end pointer 58;
// entry lifetime 51; entry zero initializer 199; file lifetime 51; drive-validation return 51;
// shared character conversion 51; element pointer for the slot 32; post-increment index local 51;
// index then count store 51; found flag with the found arm first 51/32; helper parameters
// swapped 32; force-inline helper 32; count local 44; increment first 44; pointer arithmetic slot
// 32; count reference 32; append as a list method 32; conversion local orders 34-36;
// type/find/data declared at the top 32; NONE-type test before the found arm 71/73; format and
// name locals 32; explicit count/publication memcpy 51; list increment with old count 51; count
// success/failure results 54/54; character first 32; native file length 131; root aggregate
// zero 32; post-copy success/early-capacity exits 54/54; wide result 51; list/entry/both
// references 32/32/32; count plus array pointer/reference inputs 32/32; root library clear 207;
// full character value 39; character read at each use 73; earlier ASCII buffer declaration 32;
// append record value 89; clear/full character 209; full character/buffer 39; clear/direct
// reads/buffer 211; count local stored before the copy 51 (retail's order, no count copy);
// that with an element pointer 51; count through a pointer with an index local 51; count local
// with a byte-offset destination 33; index local stored first 51; loop condition on the append's
// result 177. The existing root, path and ASCII helpers replace their written-out bodies (kept).
// shared character conversion 51.
// Earlier: the append's count/result/list registers (ours eax/cl/edx, retail esi/al/ecx,
// new count stored before the copy) and the two scratch pairs before 0x13fb70 and
// XFindNextSaveGame differ. The append's memcpy into `&entries[count++]` gives retail's index copy
// (51 -> 32); an element pointer for the slot 32; post-increment index local 51; index then
// count store 51; found flag with the found arm first 51/32; helper parameters swapped 32;
// force-inline helper 32.
// Explicit count/publication memcpy 51; list increment with old count 51;
// count success/failure results 54/54; capacity first 41; character first 32;
// native file length 131; root aggregate zero 32.
// Round 70: direct append with two-use result 32; caller append result 32; split helper
// result declaration 32; enumeration receiver 32; entry and enumeration receivers 32;
// entry receiver 32; explicit diagnostic addresses 32; mutable append input and
// sizeof(*entry) 32. Original body restored.
// Round 98: existence result named before its goto 32; count/index copy in the append (retail's
// count-to-index copy) 51; the ASCII buffer declared before the wide one 32; `continue` on a
// successful append then `break` 32; no parameter address with the named result 32; `inline` on
// 0x217a80's definition 32 (its callers 0x216da0/0x216e50 still reuse esi after it).
// append's result 177; break out of the type loop then `if (type < 11)` 64, diagnostic arm first
// 76; `list->count++` then copy into `entries[count - 1]` 53; own-parameter reference 32; named
// count read once with the new count stored first 51; early `return false` with a named index
// (everything but the setne/test and the ESI count is retail's, 654 bytes) 54; named index then
// result 51; result then named index 51; early return with `entries[count++]` 54; loop flag in
// the do/while condition 51; append with early `return false` 46; `count < 4096` 32; result
// preset false and set in the arm 46; struct assignment for the memcpy 51; index local read
// after the test 51; caller's result in a named local 32; the found block placed first 32; direct
// append with two-use result 32; caller append result 32; split helper result declaration 32;
// enumeration/entry receivers 32; explicit diagnostic addresses 32; mutable append input and
// sizeof(*entry) 32; copy into `entries[count]` then `count++` 57; struct assignment into
// `entries[count++]` 57; the entry's type stored after the 0x216e50 call 34; an unsigned
// compare against the array's element count 32; the helper without `inline` 32 (LTCG still
// inlines it). Harness-only (same allocation as the plain
// forms): helper in another file, not inline, static, generic element-size/count-pointer helpers,
// nested room/index helpers, struct assignment, list/entry references, written-out append,
// return from the loop.
// frame 0x8ac and extent agree (659 bytes plus 3 of fill). The append's
// count/result/list take EAX/CL/EDX (retail ESI/AL/ECX, the count copied to EAX after the test and
// the new count stored before the copy), and the scratch pairs before the format call,
// XFindNextSaveGame and XFindClose differ. Deleting the append alone makes those pairs retail's
// (deleting the dead flag block, the ASCII conversion, the 0x216e50 call or the path block does
// not), so the append's allocation sets their rotation. Every form with a materialized result gives
// ours; every form without one gives retail's list ECX and both pairs but no setne/test. A byte scan
// finds retail's append sequence nowhere else.
// Tried: memcpy into `&entries[count++]` 32 (kept, from 51); the existing root, path and ASCII
// helpers for their written-out bodies 32 (kept); file path/directory/both receivers 51; conditional
// character store 69; capacity first 54 (41 once); capacity nonzero 51; short type index 189; no
// parameter address 51/32; file-reference/all helpers 233/233; append names, force-inline and buffer
// names 51; source step at character read 72; const handle/unsigned conversion capacity 51; indexed
// conversion 63; full path copy 110; aggregate-zero file 53; unsigned append count, entry memcpy,
// enumeration names 51; append success result 54; capacity return 54; counted conversion 229;
// conversion end pointer 58; entry lifetime 51; entry zero initializer 199; file lifetime 51;
// drive-validation return 51; shared character conversion 51; element pointer for the slot 32;
// post-increment index local 51; index then count store 51; found flag with the found arm first
// 51/32; helper parameters swapped 32; force-inline helper 32; count local 44; increment first 44;
// pointer arithmetic slot 32; count reference 32; append as a list method 32; conversion local
// orders 34-36; type/find/data declared at the top 32; NONE-type test before the found arm 71/73;
// format and name locals 32; post-copy success/early-capacity exits 54/54; wide result 51;
// list/entry/both references 32; count plus array pointer/reference inputs 32/32; root library clear
// 207; full character value 39; character read at each use 73; earlier ASCII buffer declaration 32;
// reads/buffer 211; explicit count/publication memcpy 51; list increment with old count 51; count
// success/failure results 54/54; character first 32; native file length 131; root aggregate zero 32;
// direct append with two-use result 32; caller append result 32; split helper result declaration 32;
// sizeof(*entry) 32; break out of the type loop then `if (type < 11)` 64, diagnostic arm first 76;
// `list->count++` then the copy into `entries[count - 1]` 53; own-parameter reference 32; named count
// read once with the new count stored first 51; early `return false` with a named index 54 (654
// bytes); named index then result 51; result then named index 51; early return with
// `entries[count++]` 54; loop flag in the do/while condition 51; count local with a byte-offset
// destination 33; index local stored first 51; loop condition on the append's result 177; the found
// block placed first 32; `count < 4096` 32; result preset false and set in the arm 46; struct
// assignment for the memcpy 51; index local read after the test 51; caller's result in a named local
// 32; the dead `(void)&list` removed 32 (same code). Harness-only (same allocation): helper in another
// file, not inline, static, generic element-size/count-pointer helpers, nested room/index helpers,
// struct assignment, list/entry references, written-out append, return from the loop.
// Round 22 (lane 10): entry type stored after the 0x216e50 call 34; append written in the caller 54
// (654 bytes); copy into `entries[count]` then `count++` 57 (656 bytes); helper through a count
// pointer and the entries array 32; helper `static` without `inline` 32.
// count/result/list registers (ours eax/cl/edx, retail esi/al/ecx with the count copied to eax and
// the new count stored before the copy) and the scratch pairs before the format call,
// XFindNextSaveGame and XFindClose differ. Retail's three extra bytes are all in the append
// (`cmp esi, 0x1000` and `lea edx, [eax + 1]`). Deleting the append alone makes the format-call and
// enumeration registers retail's (deleting the dead flag block, the ASCII conversion, the 0x216e50
// call or the path block does not), so the append's allocation sets their rotation. Every form with
// a materialized result gives ours; every form without one gives retail's list ECX and both scratch
// pairs but no setne/test. A byte scan finds retail's sequence nowhere else. The memcpy into
// `&entries[count++]` gave retail's index copy (51 -> 32); the existing root, path and ASCII helpers
// replace their written-out bodies (kept).
// first 54 (41 once); capacity nonzero 51; short type index 189; no parameter address 51/32;
// file-reference/all helpers 233/233; append names, force-inline and buffer names 51; source step at
// character read 72; const handle/unsigned conversion capacity 51; indexed conversion 63; full path
// copy 110; aggregate-zero file 53; unsigned append count, entry memcpy, enumeration names 51; append
// success result 54; capacity return 54; counted conversion 229; conversion end pointer 58; entry
// lifetime 51; entry zero initializer 199; file lifetime 51; drive-validation return 51; shared
// character conversion 51; element pointer for the slot 32; post-increment index local 51; index then
// force-inline helper 32; count local 44; increment first 44; pointer arithmetic slot 32; count
// reference 32; append as a list method 32; conversion local orders 34-36; type/find/data declared at
// the top 32; NONE-type test before the found arm 71/73; format and name locals 32; post-copy
// success/early-capacity exits 54/54; wide result 51; list/entry/both references 32/32/32; count plus
// array pointer/reference inputs 32/32; root library clear 207; full character value 39; character
// read at each use 73; earlier ASCII buffer declaration 32; append record value 89; clear/full
// character 209; full character/buffer 39; clear/direct reads/buffer 211; explicit count/publication
// memcpy 51; list increment with old count 51; count success/failure results 54/54; character first
// 32; native file length 131; root aggregate zero 32; count local stored before the copy 51 (retail's
// order, no count copy); that with an element pointer 51; count through a pointer with an index local
// 51; count local with a byte-offset destination 33; index local stored first 51; loop condition on
// the append's result 177; the found block placed first 32; direct append with two-use result 32;
// caller append result 32; split helper result declaration 32; enumeration/entry receivers 32;
// explicit diagnostic addresses 32; mutable append input and sizeof(*entry) 32; break out of the type
// loop then `if (type < 11)` 64, diagnostic arm first 76; `list->count++` then copy into
// `entries[count - 1]` 53; own-parameter reference 32; named count read once with the new count
// stored first 51; early `return false` with a named index (everything but the setne/test and the ESI
// count is retail's, 654 bytes) 54; named index then result or result then named index 51/51; early
// return with `entries[count++]` 54; loop flag in the do/while condition 51; append with early
// `return false` 46; `count < 4096` 32; result preset false and set in the arm 46; struct assignment
// for the memcpy 51; index local read after the test 51; caller's result in a named local 32; the
// dead `(void)&list` removed 32 (same code). Harness-only (same allocation as the plain forms):
// helper in another file, generic element-size/count-pointer helpers, nested room/index helpers,
// written-out append, return from the loop. Round 120 (real checks; a one-file LTCG harness does not
// reproduce this function, its callees' conventions come from other files): `long index =
// list->count++;`, `index = list->count; list->count++;` and a count local with `list->count = count
// + 1` first each give retail's increment/store/shift order but no count copy (count EAX, result CL)
// 51/51/51; the helper as a plain static function 32 (same code); the copy then `list->count++` 57.
// DL200: Native byte/signed-byte completion 32/32; short/word completion 46/46; real shared file-
// reference workspace 32; overlap-safe append copy 52; coordinated matched-name-helper inline
// contract 32; byte helper return 32. Original append, workspace, copy and all game-call values
// restored; matching 0x8ac frame and scratch/append register gap remain.
// Round 109 (lane 5, fast): the append with a named `full` flag, `if (!full)` and `return !full` 32
// (aligned 26); the test written `4096 != list->count` 32 (same code); the `full` flag with `long
// index = list->count++` 51 (retail's new-count store before the copy, but `sete`/`jne` and count
// EAX), with `list->count = index + 1` 51; the `full` flag with a struct assignment 57 (656 bytes).
// The list is the shared s_saved_game_location_table (include/saved_game_storage.h, same layout).
// Round 44 (lane 16): retail keeps the count in ESI and copies it to EAX for the index (a temp
// ours coalesces away); the later scratch pairs are ours rotated by one register in eax, ecx,
// edx order, as one fewer temp would give. Tried: explicit count and index locals with the new
// count stored first 51 (copy coalesced); the helper as an ordinary function in
// unknown_216b60.cpp so LTCG inlines it 32 (plain, same code); a count local post-incremented
// in the copy and stored back after it 33; `entry.type` passed to 0x216e50 32; `entry.type`
// set inside the type loop before leaving it 32.
// Round 37 (lane 1): a nested inline index helper returning `list->count++` 51; an index local with a
// NONE default 51; the index read through a second table pointer with the new count stored first 51;
// the copy through an inline two-pointer copy helper 32; the list and entry typed as the shared
// s_saved_game_location_table / s_saved_game_file_location (callers 0x12b450 and 0x215790 with it) 32
// (same code); the permuter on the original body, 69 variants, best 32.
// Round 177 (fast, append helper): the result declared `bool const` 32, as `char` 32, as `!(count ==
// 4096)` 32 (all same code); the room test `4096 - list->count != 0` 51 (mov/sub before the setne);
// `if (!result) return result;` before the copy 32 (same code).
// Round 24 (lane 14): the append helper reshaped to return the new element pointer, the caller copying
// into it 20 (kept; plain 20); with an early `return NULL` 20 (same code); the copy as `*slot = entry` 20
// (same code); the helper returning the new index or NONE 47 (one instruction short; retail's `lea edx,
// [eax + 1]`, store and `lea edi, [eax + ecx + 4]` tail); the index read, `count = index + 1`, then the
// element 49; a bool result with the index or the element as an out parameter 51/32 (the materialized bool
// puts the list back in EDX); a bool room test with the caller copying into `entries[count++]` 54 (the
// bool folds); the count read before the 0x216e50 call (diagnostic, count in ESI) 62; the original append
// helper returning the copied element 54 (its NULL test threads away, two instructions short).
// Round 117 (fast): an inline room test plus an inline `count++` index helper, the caller breaking then
// copying into `entries[index]` 54 (654 bytes, retail's lea/shift/store tail, no setne); a bool append
// helper `result = count != 4096; if (result) { index = count++; memcpy } return result;` with
// `if (!append) break;` 51 (662 bytes: setne/test now, but bool CL, count EAX, list EDX against
// retail's AL/ESI/ECX); the same over a count local stored `count + 1` 51; diagnostics with the room
// flag tested again after the copy, written out or from an inline room helper, 51/51 (same registers).
// Round 114 (lane 6): the append helper taking the count by value 33; a bool helper handing the entry
// out through a pointer parameter 32; the helper returning the next entry (NULL when full) with the
// caller copying into it 20 (kept), with an early `return NULL` 20, with a struct assignment for the
// copy 20, with an index local (post-increment, or stored as index + 1 before the address) 49/49.
// Round 32 (m3 lane 14): retail's registers fit two inlined helpers sharing one count load: a bool room
// test returned in AL (count then in ESI) and an index helper (`index = list->count; list->count = index
// + 1; return index;`) returned in EAX (`mov eax, esi`). The room helper (`bool result = count != 4096;
// return result;`, `return count != 4096;`, preset false/true filled by an if, `count < 4096`, taking the
// count by value) with the index helper and the caller breaking, as an else arm, or through a named
// `room` local: 54 each (654 bytes: retail's `lea edx, [eax + 1]; shl eax, 6; mov [ecx], edx; lea edi,
// [eax + ecx + 4]` tail with the list in ECX, but the bool threads into `cmp; je`, count EAX, no copy);
// a bool append helper over the room/index helpers 51 (setne/test, bool CL, list EDX, count EAX).
// Round 138 (lane 4): a bool append helper (`bool result = list->count != 4096;`, then `long index =
// list->count; list->count = index + 1;` and the memcpy into `entries[index]`) 51 (setne/test as retail, but
// count EAX, result CL, list EDX); that with `long index = list->count++;` 51; the pointer helper with a named
// `room` test and the index copy 49; the pointer helper as a list member function (`list->new_entry()`) 20 (same
// code); the bool helper as a list member function 51.
// Round 65: a bool reservation with the caller copying at count - 1 53; a named room in the pointer
// helper 20, before the pointer result 20; caller room with an unchecked pointer 54, with the checked
// pointer 54; a caller index across a bool reservation 51; a conditional pointer return 20; the slot
// declared before the name call 20.
// @retail 0x2164c0
void function_2164c0(long memory_unit, s_save_list_2164c0 *list)
{
    char drive;
    if (function_1249f0(memory_unit, &drive))
    {
        char root[8] = "";
        function_216400(memory_unit, root, sizeof(root));
        XGAME_FIND_DATA data;
        HANDLE find = XFindFirstSaveGame(root, &data);
        if (find != INVALID_HANDLE_VALUE)
        {
            do
            {
                s_save_entry_2164c0 entry;
                memset(&entry, 0, sizeof(entry));
                strncpy(entry.directory, data.szSaveGameDirectory, sizeof(entry.directory));
                entry.directory[19] = 0;
                long type;
                for (type = 0; type < 11; type++)
                {
                    char path[256];
                    path[0] = 0;
                    function_216d70(path, entry.directory, type);
                    s_storage_file_reference file;
                    memset(&file, 0, sizeof(file));
                    file.signature = FILE_REFERENCE_SIGNATURE;
                    file.location = NONE;
                    if (file.flags & 1)
                    {
                        short length = (short)strlen(file.path);
                        while (length > 0 && file.path[length] != '\\') length--;
                        file.path[length] = 0;
                    }
                    if (*path)
                    {
                        size_t length = strlen(file.path);
                        char *end = file.path + length;
                        if (end != file.path && end[-1] != '\\')
                        {
                            *end++ = '\\';
                            *end = 0;
                            length++;
                        }
                        strncpy(end, path, sizeof(file.path) - length);
                        file.path[255] = 0;
                    }
                    file.flags |= 1;
                    if (function_1368f0((s_type_acf665 *)&file))
                        goto found;
                }
                {
                    word description[256];
                    unicode_string_snprintf(description, 256, L"unrecognized saved game file found by XFindNextSaveGame(): display name= '%s' path= '%hs'", data.szSaveGameName, data.wfd.cFileName);
                    char text[256];
                    unicode_string_to_ascii(description, text, 256);
                }
                goto next;
found:
                {
                    entry.type = type;
                    function_216e50(data.szSaveGameName, type, &entry.language, entry.name);
                    s_save_entry_2164c0 *slot = new_save_entry_2164c0(list);
                    if (!slot)
                        break;
                    memcpy(slot, &entry, sizeof(entry));
                }
next:;
            } while (XFindNextSaveGame(find, &data));
            XFindClose(find);
        }
    }
}
