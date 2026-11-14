// @flags /O2 /Gr
#include "handles_storage.h"
/* UNKNOWN_03D380.CPP: the game module callbacks of the table at 0x46e320
   (batch 18-4); every entry takes one dword on the stack */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_03d380.h"
#include "object_iterator.h"

/* ---- types ---- */

struct s_object
{
	byte unknown00[0xcc];
	long unknownCC;
};

struct s_simulation_world
{
	byte unknown00[8];
	long state;
	byte unknown0c[0x22];
	byte flag2e;
};

struct s_4e6380
{
	byte unknown00[0x78];
	byte flag78;
	byte unknown79[2];
	byte flag7b;
	byte unknown7c;
	byte flag7d;
	byte unknown7e[2];
	long unknown80;
	long unknown84;
	byte unknown88[0x1f8 - 0x88];
	long unknown1f8;
	byte unknown1fc[4];
	long unknown200;
};

struct s_element_bc
{
	byte unknown00[2];
	byte kind;
	byte unknown03[9];
	long object_index;
	byte unknown10[0x7c];
	long unknown8c;
	byte unknown90[0x2c];
};

struct s_element_18_flags
{
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 8;
};

struct s_element_18
{
	byte unknown00[3];
	byte state;
	s_element_18_flags flags;
	byte unknown06[6];
	long value;
	byte unknown10[8];
};

struct s_element_40
{
	byte unknown00[4];
	long table_index;
	byte unknown08[0x30];
	long datum_index;
	byte unknown3c[4];
};

struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long unknown08;
	long index;
	dword signature;
};


struct s_unknown_5c;
s_unknown_5c *function_221810(short index);

/* ---- globals ---- */

byte g_4cf771;
byte g_4cf772;
byte g_4cf77b;
dword g_453498[1];
s_4e6380 *g_4e6380;
s_record_pool *g_4e637c;
s_looping_sound_globals *g_4ed288;

void decal_delete_group(long decal_index);

// @retail 0x43820
void __stdcall function_43820(long index)
{
	dword *entry = g_4c8798 + (index & 0xffff);
	if (*entry != NONE)
	{
		decal_delete_group(*entry);
		*entry = NONE;
	}
}

// @retail 0x43850
long __stdcall function_43850(long index)
{
	long handle = g_4c8798[index & 0xffff];
	byte *entry = (byte *)g_4ea950->data + (handle & 0xffff) * 0x40;
	word flags = entry[2];
	return (flags & 1) || (flags & 2);
}
byte g_4ea934;
byte g_4ea936;
byte g_4e6388;
s_record_pool *g_51ebfc;
s_record_pool *g_51ec00;
long g_47f04c;
byte g_47f059;
s_47f048_object *g_47f048;
void *g_51ecac;

#define GAME_MODE (g_4e6948->mode)
#define ELEMENT(array, type, datum) ((type *)((array)->data + sizeof(type) * ((datum) & 0xffff)))

/* ---- the table ---- */

void __stdcall function_1c3540(dword flags);
void __stdcall function_68090(dword flags);
void __stdcall function_16eff0(dword flags);
void __stdcall function_1e6af0(dword flags);
void __stdcall function_1c3590(dword flags);
void __stdcall function_123b00(dword flags);
void __stdcall function_1889d0(dword flags);
void __stdcall function_155f10(dword flags);
void __stdcall function_67fb0(dword flags);
void __stdcall function_124750(dword flags);
void __stdcall function_1264c0(dword flags);
void __stdcall function_188ac0(dword flags);
void __stdcall function_16f090(dword flags);
void __stdcall function_14b660(dword flags);
void __stdcall function_67fc0(dword flags);
void __stdcall function_43970(dword flags);
void __stdcall function_3d380(dword flags);
void __stdcall function_124620(dword flags);
void __stdcall function_155360(dword flags);
void __stdcall function_1c7f80(dword flags);
void __stdcall function_17d190(dword flags);
void __stdcall function_16f0c0(dword flags);
void __stdcall function_226030(dword flags);
void __stdcall function_24c829(dword flags);

game_module_proc g_46e320[30] =
{
	function_1c3540,
	function_68090,
	function_16eff0,
	function_1e6af0,
	function_1c3590,
	function_1c3540,
	function_123b00,
	function_1889d0,
	function_155f10,
	function_67fb0,
	function_124750,
	function_72c70,
	function_1264c0,
	function_188ac0,
	function_16f090,
	function_14b660,
	function_67fc0,
	function_43970,
	function_3d380,
	function_72c70,
	function_72c70,
	function_72c70,
	function_124620,
	function_155360,
	function_24c829,
	function_1c3590,
	function_1c7f80,
	function_17d190,
	function_16f0c0,
	function_226030,
};

// @retail 0x3d380
void __stdcall function_3d380(dword flags)
{
	struct { s_object *object; s_type_f1af8e iterator; } iteration;

	function_bae80(&iteration.iterator, 0, 0);
	for (iteration.object = function_baeb0(&iteration.iterator); iteration.object; iteration.object = function_baeb0(&iteration.iterator))
		iteration.object->unknownCC = NONE;
}

// @retail 0x43970
void __stdcall function_43970(dword flags)
{
	g_509448->proc20 = function_43820;
	g_509448->proc24 = function_43850;
}

// @retail 0x67fb0
void __stdcall function_67fb0(dword flags)
{
	g_4cf77b = 1;
}

struct s_67fc1
{
	byte field_0[0xc];
	signed long field_c : 8;
};

// @retail 0x67fc0
void __stdcall function_67fc0(dword flags)
{
	if (flags & 4)
	{
		function_593e0();
		s_simulation_world *world;
		if (GAME_MODE == 1 && (world = g_4cf77c)->state == 1)
		{
			function_6b040((c_class_6a600 *)world);
			g_4cf772 = 0;
			g_4cf771 = 0;
		}
		else
		{
			function_67f60();
			if (GAME_MODE == 4)
			{
				GAME_MODE = 5;
				void *proc = g_55e4d0[g_4e9ae8->engine_index];
				if (proc)
					function_162060(proc);
			}
			s_67fc1 *local_0 = (s_67fc1 *)g_4e6948;
			long local_1 = local_0->field_c;
			if (local_1 >= 4 && local_1 <= 5)
			{
				function_162420();
				function_bb7f0();
				function_183f10();
			}
			GAME_MODE = 1;
			function_67ee0();
		}
	}
	((c_class_6a600 *)g_4cf77c)->function_6a600();
	function_83370(g_4cf780, flags);
	function_6a770(g_4cf77c);
	g_4cf77b = 0;
}

// @retail 0x68090
void __stdcall function_68090(dword flags)
{
	if (g_4cf770 && !(flags & 1))
	{
		long state = g_4cf77c->state;
		if (state != 3 && state != 5)
			g_4cf77c->flag2e = 1;
	}
}

static __forceinline void function_123b01(byte *arg_0)
{
	XPhysicalProtect(arg_0, 0x3be000, PAGE_READWRITE);
	XPhysicalProtect(arg_0 + 0x3be000, 0x40000, PAGE_READWRITE | PAGE_WRITECOMBINE);
}

// @retail 0x123b00
void __stdcall function_123b00(dword flags)
{
	function_123b01(game_state_globals.base_address);
}

// @retail 0x124620
void __stdcall function_124620(dword flags)
{
	game_state_globals.game_time = g_510c54->game_time;
	g_510c54->unknown01 = 0;
}

// @retail 0x124750
void __stdcall function_124750(dword flags)
{
	if (g_510c2c)
		*(dword *)g_510c2c = (dword)g_453498;
}

void function_125a90(long value);

// MiniMax E (11-14): no new form, no lower score (36): the record's address is still built with an imul into ESI and an
// add where retail multiplies into EAX and forms the address with a lea, so the type byte is tested in another value;
// the layouts that put retail's special arm first need 0x221810 kept out of line, and that file is outside this lane.
// score 36 (was 36): the address of the record is built here with an imul
// into ESI and an add; retail multiplies into EAX and forms the address with a
// lea, so the type byte that is tested afterwards is compared in another value.
// Retail also falls from the type test into the special-type arm, then the
// removal, then the kind test; ours puts the kind test first. Tried: the
// special arm as `type != 0x26 || a` 88; as `type == 0x26 && !a` 88; the kind
// tests joined with && 46; the type test negated, kind arm first 46; removal
// first with an if/else special arm 50.
// Round 411: Existing calling iterator with returned/salted receivers and remove/keep
// decisions 115/110/168/177; existing fully inlined iterator with the same four forms
// 115/110/168/177. Original traversal restored; the early policy load, ESI multiply
// and special/remove/ordinary/keep ordering remain.
// Lane 14 r9: the join as `if (remove) {removal} else keep` 51 (kind arm, keep, then the special arm and
// removal after the loop step); every action written in its own arm (no flag) 110; the flag declared
// before the element 47 (same code); the kind tests joined with && 47, plus the flipped join 51; the
// special arm as `remove = type == 0x26 ? !a : false` 47.
// Round 95 (one-file harness with 0x221810 pinned out of line; aligned 23 = 47 here): ours ends
// the kind arm with a jump into the special arm's `je` (tail merge), so the kind arm is the
// fall-through; that order survives the special arm cut down to `remove = false`, the arms
// swapped, removal first and every flag form tried. Real: kind arm `remove = f() != 1` with
// removal first 88 (417 bytes); arms swapped with removal first 51; kind arm as one `||`
// value with removal first 46; data pointer local 47; special arm as if/else with removal
// first 51. Harness only: the file's ELEMENT macro 37 (worse registers); element as
// `&((T *)data)[i]`, `data + i * sizeof`, `sizeof * i`, `i + (T *)data`, an elements
// pointer: the ESI multiply in each; `remove = type == 0x26 && !a` threads the sete away.
// Round 100 (real builds): the file's ELEMENT macro 62 (same early flag load, EBP/EDI swap);
// `remove` declared first 47; removal written first 51: both arm bodies then end as retail's
// (special arm `je keep` into the removal, kind arm `jne remove` into keep), but the kind arm
// stays the type test's fall-through and the special arm with the removal moves after the
// next-index code; with the type test negated and the kind arm first 51 (identical); with the
// kind tests joined by && 51.
// Round 11: `if (0x20) remove = false; else if (0x26) remove = !a; else if (kind)` with removal
// first 87 (retail's block order, but no re-test of 0x26 for 0x20); the condition named `bool
// special = 0x20 || 0x26` with removal first gives retail's whole block order in the compile-only
// harness, but LTCG then inlines 0x221810 (144 at 467 bytes; with the kind test as one `&&` 144,
// keep first 157 at 471 bytes); removal first alone 51.
// Round 13 (lane 13): the element written out at each use (`((T *)array->data)[index].field`)
// gives retail's `imul eax` + `lea esi` but sinks it below the b test 43; with `if (remove)`
// first as well 47; `if (remove)` first alone 51; ELEMENT(array, T, datum) 62, with index 94;
// `&((T *)data)[index]` 47; `(T *)(data + index * sizeof(T))` 47.
// Round 442: Live type observations with bool/byte/short/long removal 91/91/85/90; removal-
// first completion 91/91/85/90. Original type capture, comparison widths and complete body
// restored.
// Later (36, from 47): the special arm now assigns the removal flag itself
// (`remove = (type == 0x26) && !a`) and the kind arm assigns `remove = (byte != 1)`,
// so both arms branch to their own blocks instead of merging into the special arm's test;
// the removal arm is written before the keep arm, which puts retail's blocks in retail's order.
// Still differs: the b flag is loaded before the element address (retail loads it after) and
// the type lookup uses EAX where retail uses EDX.
// Round 109 (lane 5, fast): the special arm as `if (type == 0x26) remove = !a; else remove = false;`
// on the long type with the kind arm as `if (byte == 1) remove = false;` 51: both arm bodies are
// then retail's code (`cmp eax, 0x26`, the `sete` of !a, the fused kind arm), but the kind chain is
// still laid out first; that with keep written first 47 (tail-merged); the condition named `bool
// special = 0x20 || 0x26` with those arms 144 (retail's block order, but LTCG then inlines 0x221810,
// 467 bytes), with `if (!special)` and the kind arm first 158 (471 bytes, inlined), with the call's
// argument as `(short)type` 144. BLOCKED-BY 0x221810: the named-condition layout needs 0x221810 kept
// out of line (its file is outside this round).
// Round 44 (lane 16, fast counts; control 36): retail lays out the special arm first, falling
// into the removal with its `!a` materialized (`sete al; test al, al; je keep`), then the kind
// arm fused (`jne remove`) falling into keep; ours lays out the kind arm first (materialized
// `setne al; test al, al`) and fuses the special arm. Tried: the inner test on the long type
// (`type == 0x26 && !a`, retail's `cmp eax, 0x26`) 46, still kind arm first; that as a ternary
// `type == 0x26 ? !a : false` 88; the type test negated with the kind arm first and the special
// arm in the else 46 (same code as the long test); the kind arm as `remove = false` under one
// `kind && byte == 1` test 88; a two-case switch on the short type 86 (compares in ECX).
// Round 108 (lane 6): the type byte is the sound definition's class (the existing
// sound_definition_get(...)->promotion_index): read as a short 87 (`movsx ax`, retail widens to
// 32 bits), as a long through the getter 36 (same code as the direct read). Retail re-tests
// `cmp eax, 0x26` after `cmp ax, 0x26`, so its inner test is on the full-width value and is not
// threaded away (matched 0x18d360 tests the same pair on a short); that inner long test alone
// 46 gives retail's control flow but keeps the kind arm as the test's fall-through. `remove`
// assigned on every path with no initializer 36 and the negated test with the kind arm first 36
// both compile to the same code (VC normalises the arm order).
// Round 284: complete policy observations from incoming flags, b/a/both 103/143/144;
// those with the native inner type comparison 104/144/145; both with short/byte type
// carriers 149/152. Original policy snapshots, type and complete body restored;
// scaled ordinal before policy load and decision-block placement remain.
// Round 55 (fast): element computed before the index local 51; index as `(word)datum` 114; the special arm
// as `if (type == 0x26) remove = !a; else remove = false;` on the long type 88 (417 bytes); `remove` declared
// before the index and element 36; the element through a typed elements array local 36.
// Round 132 (lane 6, fast): the condition named as a bool 157 (471 bytes; LTCG still inlines
// 0x221810); the whole decision as one ternary 88 (both arms threaded, kind arm first); the file's
// ELEMENT macro with `(datum & 0xffff) + 1` and no index local 51; the special arm as `if (type ==
// 0x26) remove = !a; else remove = false;` on the long type 88 (aligned 31 against 35: the special arm
// is then materialised and falls into the join as in retail, but the kind arm is laid out first and
// jumps into the join with `setne`); that with the kind arm as one `kind && x == 1` test setting
// `remove = false` 51: both arm bodies are then retail's, but the kind arm stays the class test's
// fall-through and VC moves the special arm and the removal into the gap after the next-index
// ternary's `jmp`, where retail puts the kind arm in the gap after the removal's `jmp`.
// Round 71 (lane 12, fast): the special arm as `if (type == 0x26) remove = !a; else remove = false;` with the kind
// arm as `if (byte == 1) remove = false;` 51 (retail's arm bodies; the kind arm falls into keep and the special arm
// with the removal moves after the loop step); the condition named `bool special` with those arms 144 (467 bytes:
// retail's block order, special arm with `sete`, removal, kind arm, keep, but LTCG inlines 0x221810), with
// `remove = type == 0x26 && !a` and `!= 1` 157 (471), with the kind arm as one `&&` test 144 (467); the element
// pointer formed from `datum & 0xffff` before index 51. Still BLOCKED-BY 0x221810 (inlined whenever the layout is
// retail's).
// Round 287: Paired unsigned/signed-byte policies, each isolated byte policy, and paired byte
// mask-predicate producers 36 each; paired unsigned/signed-byte policies with the native long
// inner type test 46 each. Original policy storage, producers and body restored.
// Round 284: complete policy observations from incoming flags, b/a/both 103/143/144;
// those with the native inner type comparison 104/144/145; both with short/byte type
// carriers 149/152. Original policy snapshots, type and complete body restored;
// scaled ordinal before policy load and decision-block placement remain.
// Round 59 (lane 10): byte MATCH with the pool global at each use (no array/index locals), a named `special`
// bool and the kind test written `if (!special && remove && K && f == 1)`, plus the keep store through the
// global beside the element local (a mixed read). Not kept at review (11-02): `remove` is implied by `!special`
// there, a redundant always-true test that only keeps 0x221810 out of line (10-30 ruling: reverted even with a
// match), and the mixed read is match-only. With the implied test, no direct keep use 25. Without it, direct `||`
// with the same global/index forms 29 (415 bytes; still uses the mixed keep store). 0x221810 pinned out of line
// as a diagnostic gives 25, so the inlining is one of the two real causes; the other is retail's `imul eax` +
// `lea esi` pair. Body restored at 36.
// Round 89: special arm without an else assignment 88 (417 bytes), with the fused kind keep test 51;
// full-width named special predicate 159 (472 bytes); no index local with a pool local 51; pool global
// at each use with or without the index local 36 each; tag-byte subscript 36; direct global-array object
// read 34 (416 bytes, -2 not kept); every record field direct with no element local 146 (398 bytes).
// Original body restored.
// Round 204 (fast): a declared before b 50; the special arm as `!a && type == 0x26` 36 (same code); the getter
// argument as `(short)type` 36; a dword index 36; the after-loop calls as `if (b) ... else ...` 39. Restored.
// Round 125 (lane 8, fast): `a` declared before `b` 50 (the two flag bytes swap stack slots: slot order follows
// declaration order here); `a` declared in the flag78 block 57, at its use in the b arm 133; the flags as mask
// tests (`(flags & 0x20) != 0`) 36 and the kind test as `(0x1e >> kind) & 1` 36 (same code each). Still
// BLOCKED-BY 0x221810 (src/unknown_221490.cpp held this round) and the ESI multiply. Body unchanged.
// M341: Actual kind-bitset producer byte/word/long/dword, negative/positive decision:
// 48/48/36/36/48/48/36/36. Original expression and full policy loop restored;
// element multiply/LEA, early policy load and decision-block order remain.
// Round 84 (m3 lane 9, fast control 36): the arm whose flag is materialized is the one that falls into the
// removal (retail: special arm with `sete`; ours: kind arm with `setne`). The stop decision as an inline predicate
// with early returns (`if (!b || stops(element, a))`): a plain `inline` one stays out of line, 170 (342 bytes); as
// `__forceinline` 47 (retail's `cmp eax, 0x26` and both arm bodies, but mirrored: kind arm, keep, special arm,
// removal); the predicate in keep sense 87; its class test negated, kind case first 47 (same code); the caller's
// keep branch first 47 (same code); the named class condition inside the forceinline predicate 153 (467 bytes,
// 0x221810 inlined again). Original body restored; still BLOCKED-BY 0x221810 (its file held this round).
// Round 104: named special with the callee declared inline 159 (472 bytes; declarations restored);
// named element byte offset 36; keep result 36; keep arm first 86; type declared at loop top 36;
// index declared only at the iterator 51; const element pointer 36; const type 36. All restored;
// the element multiply/LEA, early policy load and decision-block placement remain.
// Round 69 (lane 11, fast; pool global at each use, no index local, direct keep store unless noted; [aligned]): no implied test, the call
// stays out of line: `type == 0x20` keep, else `type == 0x26` `remove = !a`, else the kind test, 74 (positional only) [5]: retail's block
// order and every register, only retail's `cmp eax, 0x26; jne` re-test is missing; 0x26 first 77 [7]. Under one `||`: `if (type == 0x26) remove
// = !a; else remove = false;` 29 [12], as `remove = false; if ...` or a ternary 29 [12], the removal arm after the keep arm 25 [12] (kind arm
// still falls through), that without the direct keep store 47 [23]; `remove = type == 0x26 && !a` under `||` 75 [15]; `type != 0x26 || a` 75;
// inverted `&&` 75, inverted `!(||)` 29; each type test a named bool 29. Named special or ordinary, with two ifs or nested tests, and no
// implied test: 151/142/162 (466-471 bytes, 0x221810 inlined). Not kept: 29 and 25 need the direct keep store, which is match-only.
// Round 153 (m3 lane 5, fast; all 415 bytes): with the inner test on the long type and the kind arm as one
// `remove = !(mask) || f(type) != 1` value: `remove = type == 0x26 && !a` 88; `if (type == 0x26) remove = !a; else remove =
// false;` 46, with keep written first 51; `remove = false; if (type == 0x26) remove = !a;` 46; the if/else special arm with
// the kind arm as `if (mask && f(type) == 1) remove = false;` 51. Compile-only screen with /d2inlT0 (reproduces LTCG's
// choice: 0x221810 stays out of line for an inline `||` class-type test, inlined for a named class-type bool) over 80 forms: every
// inline `||` form lays the kind arm out as the class-type test's fall-through; a named bool, an inline class-test helper or the
// kind test as `kind == 1 || ... == 4` (no bit test then) all inline 0x221810.
// J260: Immutable b/a/both policies 36/36/36; receiver initialized from the pool base then advanced by index, with no/b/a/both immutable policies, 36/36/36/36; receiver initialized from base then indexed 36. All eight complete target listings equal the unchanged preview.
// NOT kept: no eligible native repair; opening executable source restored. EAX multiply/ESI LEA before policy loading and native type/removal block placement remain; original flags, loop, pool refresh, fields, calls and body restored.
// Round 398: Original single type observation with complete removal-result byte/signed-byte/short/word/
// int/unsigned-int/long/dword storage: 36/36/95/95/94/94/94/94. NOT kept: no gain; original
// Boolean removal result, policy captures, comparison widths and complete body restored.
// MiniMax D (11-06): no new form, no lower score.
// Round 73 (m3 lane 13, fast, control 36; aligned 22, register-masked 14): the `||` written in the `if` lays the kind
// arm out first whatever is in the arms, and a class-type test held as a bool value lays the special arm first as retail
// does, but 0x221810 is then inlined. `if (type == 0x26) remove = !a; else remove = false;` with the kind arm as
// `if (x == 1) remove = false;` under the `||` 51 (re-measured; both arm bodies retail's, chains in the other order);
// that with the class-type test named as a bool 144 (143 instructions, 0x221810 inlined); with the type read through the
// existing sound_definition_get as well 144 (same code); with a shared next-datum inline helper for the loop as
// well 152 (still inlined, so the caller's other expansions do not keep it out); a named short class-index value under the
// `||` 51 (same code); an inline class-type predicate taking a short 144 (inlined); the sound-class-record pointer named before its
// test 51 (same code). Diagnostics, reverted: the kind arm reading a table instead of calling 98, and a call added to
// the special arm 98: the kind arm stays first in both, so the order does not follow which arm holds a call.
// Still BLOCKED-BY 0x221810 (retail never inlines it; its file is another lane's this round).
// Round 65 (m3 lane 15, fast; base 36, all 415 bytes unless noted): retail's kind arm is threaded (its exits jump
// straight to the removal or the keep store) and only its special arm holds a value (`sete`), with the order special
// arm, removal, kind arm, keep store. With the kind arm as `if (f(type)[0x4c] == 1) remove = false;` under the bit
// test and the special arm on the long type as `remove = false; if (type == 0x26) remove = !a;` 51, as if/else 51, as
// `type == 0x26 && !a` 88, as a ternary 51; those four special arms with the kind arm as a value 88 (417), 88 (417),
// 46, 88 (417). The 51 forms hold retail's instructions (16-bit pair, 32-bit re-test, sete arm, threaded kind arm)
// but lay them out kind arm, keep store, special arm, removal. To move that: the kind arm as the `then` of a negated
// `&&` 51; `if (type != 0x26) remove = false; else remove = !a;` 51; `remove = !a; if (type != 0x26) remove = false;`
// 51; `if (type != 0x26 || a) remove = false;` 88; both arms as if/else constants with the kind tests joined by `||`
// 46; the kind bit test split out as `remove = true` 88 (417). A two-case switch on the short type (compares widen to
// 32 bits, same layout) with those special arms 89, 89, 92, and with both arms as values 86. An else-if chain
// (0x20 keeps, else 0x26 `remove = !a`, else the kind arm as a value or as the `if`) 87, 87: retail's order, special
// arm first, but 0x20 jumps to the keep store where retail sends it to the re-test; the special arm written out under
// each of the two tests 89 (417, the copies are not merged). `if (0x20 || 0x26) remove = !a;` as a probe 87: every
// `||` form sends both tests to the special arm and lays the kind arm first, whatever the arm holds. Body unchanged.
// MiniMax R (11-10): no new form, no lower score (36): the special arm and the kind arm still swap their block, and the layout needs 0x221810 kept out of line, so it stays BLOCKED-BY 0x221810 in src/unknown_221490.cpp.
// MiniMax R (11-09): no new form, no lower score; the special arm and the kind arm still swap their block
// order, and the record address still goes to esi where retail multiplies into eax and lea's it.
// This round: the element as an array subscript 36, the type test inverted 36, the special arm nested with
// an explicit else 89, the re-test without the short cast 46 (it gives retail's shared jump target and
// `cmp eax, 0x26`), the kind arm written as a branch 87; the block order still decides the score.
// MiniMax lane G (11-04): no new form, no lower score; two separate type cases give retail's chain
// (je to the special arm, jne to the kind arm) with 0x221810 out of line, but the pool pointer moves
// to edi and the score is 83; the named `||` gives the same chain and the a/b slots but inlines 0x221810.
// MiniMax H (11-04): no new form, no lower score; retail multiplies the element index in eax and lea's the address, ours multiplies in esi, and 0x221810 stays out of line.
// MiniMax A (11-03): no new form, no lower score: the element address is still formed after the policy
// byte is read, and the special, remove, ordinary and keep blocks stay in our order.
// MiniMax lane E (11-01): no new form, no lower score; the named-condition form that gives retail's
// block order still needs 0x221810 kept out of line.
// MiniMax C (11-08): no new form, no lower score.
// M3 lane 14 r83 (fast with the file, 0x221810, 0x127320, 0x125a90): 0x26 tested before 0x20 in the `||` 90; the special
// arm as `(short)type != 0x20 && !a` 88 (`!((short)type == 0x20 || a)` the same code); the next index as
// `datum != NONE ? index + 1 : 0` 40; the kind arm as `else if (kind && f(type)[0x4c] == 1) remove = false;` 87; the
// outer `||` on the long type with the inner test on the short 49. Same code: an explicit `else remove = true;`, the
// record pointer of 0x221810 named before its test, the special arm as an if/else, the definition flags through a named
// pointer. src/unknown_221490.cpp is open this round, but 0x221810 is one getter call, so no spelling of it keeps its
// bytes and moves the inliner: still BLOCKED-BY 0x221810.
// MiniMax H (11-12): no new form, no lower score (36): the special arm still comes before the kind arm where retail puts the kind arm first, the element address is still a scaled index added to the base where retail multiplies into eax and adds the base, and 0x221810 still has to stay out of line, which needs a change in src/unknown_221490.cpp.
// MiniMax I (11-11): no new form, no lower score (36): the special arm still comes before the kind arm in the layout where retail puts the kind arm first, our element address is a scaled index added to the base where retail multiplies into eax and adds the base, and 0x221810 still has to stay out of line, which needs a change in src/unknown_221490.cpp.
// MiniMax D (11-11): no new form, no lower score (36): still BLOCKED-BY 0x221810, the getter in src/unknown_221490.cpp that this lane may not edit and that LTCG inlines here.
// MiniMax S (11-10): no new form, no lower score (36): the special arm still comes before the kind arm in the layout where retail puts the kind arm first, and our element address is a scaled index added to the base where retail keeps the scaled product in EAX and adds the base to it.
// MiniMax A (11-10): no new form, no lower score (36): retail keeps the sound-class lookup 0x221810
// out of line - it is a one-line wrapper in src/unknown_221490.cpp - and lays the blocks special
// arm, removal, kind arm, keep store; LTCG inlines it here, so the kind arm's block order cannot be
// reached from this file.
// MiniMax R (11-10): no new form, no lower score (36): the special arm and the kind arm still swap their block, and the layout needs 0x221810 kept out of line, so it stays BLOCKED-BY 0x221810 in src/unknown_221490.cpp.
// MiniMax R (11-09): no new form, no lower score; the special arm and the kind arm still swap their block
// order, and the record address still goes to esi where retail multiplies into eax and lea's it.
// This round: the element as an array subscript 36, the type test inverted 36, the special arm nested with
// an explicit else 89, the re-test without the short cast 46 (it gives retail's shared jump target and
// `cmp eax, 0x26`), the kind arm written as a branch 87; the block order still decides the score.
// MiniMax lane G (11-04): no new form, no lower score; two separate type cases give retail's chain
// (je to the special arm, jne to the kind arm) with 0x221810 out of line, but the pool pointer moves
// to edi and the score is 83; the named `||` gives the same chain and the a/b slots but inlines 0x221810.
// MiniMax H (11-04): no new form, no lower score; retail multiplies the element index in eax and lea's the address, ours multiplies in esi, and 0x221810 stays out of line.
// MiniMax A (11-03): no new form, no lower score: the element address is still formed after the policy
// byte is read, and the special, remove, ordinary and keep blocks stay in our order.
// MiniMax lane E (11-01): no new form, no lower score; the named-condition form that gives retail's
// block order still needs 0x221810 kept out of line.
// MiniMax C (11-08): no new form, no lower score.
// MiniMax H (11-13): no new form, no lower score (36): still BLOCKED-BY 0x221810, the getter in
// src/unknown_221490.cpp that this lane may not edit - retail keeps it out of line and LTCG inlines it here, and every
// spelling of the named-condition layout that gives retail's block order inlines it too.
// MiniMax G (11-12): no new form tried this round, no lower score (36): the element address is still built with an
// imul and an add where retail multiplies into eax and forms the address with a lea; a probe shows no pointer
// arithmetic spelling of that form in this compiler.
// M3 lane 12 r119 (fast with the file and 0x221810/0x127320/0x125a90): diagnostic only, one `#pragma inline_depth(0)`
// around the 0x221810 call (reverted): the class-type condition named (`bool special = (short)type == 0x20 ||
// (short)type == 0x26;`) with `remove = type == 0x26 && !a;` 47 (special arm first, its `!a` threaded); the special arm
// as `if (type == 0x26) remove = !a; else remove = false;` and the kind arm as `if (f(type)[0x4c] == 1) remove = false;`
// 25 (retail's whole block order and arm code; left: the element address, ours `imul esi` + `add`, retail `imul eax` +
// `lea esi`); with the pool global at each use 25 (same code); with the element read at each use through the pool 22
// (416 bytes: retail's `imul eax` + `lea esi`, sunk below the b test). So with 0x221810 out of line the named-condition
// layout is all but retail's; every natural spelling of it inlines 0x221810 (listed above). Still BLOCKED-BY 0x221810.
// Real (no pragma): that whole 22 form 133 (468 bytes, 0x221810 inlined); the same with the class-type test written
// inline as `||` (no named bool) and every element field read through the pool at each use (no element local, so no
// mixed keep store) 47 (416 bytes).
// m3 lane 6 r178 (one-file LTCG harness: this file, src/unknown_221490.cpp, src/unknown_124f90.cpp,
// src/unknown_16b570.cpp and the build's ballast object; it gives 35 for this body where --fast gives 36, and
// lab counts below are its instruction differences): LTCG inlines 0x221810 when the call site is hot enough,
// and every test that guards the call halves its weight; the inline `||` puts the call two tests deep, the
// named class-type bool one, which is why that form inlines it. On the named-bool layout (round 119's 25 form,
// without its pragma) one more guarding test that folds away keeps the call out of line: `element &&` on the
// kind test 25 in --fast (lab 11; most of the 25 are relocation shifts from one byte); `b &&` before the
// getter call, `!special &&` there, `(short)type != 0x20 &&` on the kind test lab 11 each. Left: retail
// computes the element address before the `b` test as `imul eax` + `lea esi` with `b` loaded after it; ours
// loads `b` first and builds the address in ESI, and the type lookup's registers rotate (ours EAX/ECX/EDX,
// retail EDX/EAX/ECX). On the guarded form (lab): the element at each use through the pool 10 (retail's type
// registers and `imul eax` + `lea`, but the address sinks below the `b` test), through g_4e637c 33; mixed
// local and pool reads 23 each; byte-offset element, `&elements[index]`, `index * 0xbc`, `sizeof * index` 11
// each; `(word)datum` index 35; the type through sound_definition_get 11, as a char 43; `remove` declared
// before the element 11; index and element declared before the loop 11; `if (!b) remove = true; else` 11.
// The guards are implied tests, so none is kept without a match. Body unchanged.
// @retail 0x1264c0
void __stdcall function_1264c0(dword flags)
{
	bool b = (flags >> 5) & 1;
	bool a = (flags >> 6) & 1;


	/* neither pause (0) nor resume (1): only clears the paused flag */
	if (!b)
		function_125a90(2);

	if (g_4e6380->flag78)
	{
		long datum;

		function_21d4d0();
		s_record_pool *array = g_4e637c;
		datum = data_datum_index(array, function_16bc00(array, 0));
		while (datum != NONE)
		{
			long index = datum & 0xffff;
			s_element_bc *element = (s_element_bc *)array->data + index;
			bool remove = true;

			if (b)
			{
				long type = *((char *)g_4e3b44[element->object_index & 0xffff].flags + 2);

				if ((short)type == 0x20 || (short)type == 0x26)
					remove = ((short)type == 0x26) && !a;
				else if ((1 << element->kind) & 0x1e)
					remove = ((byte *)function_221810(type))[0x4c] != 1;
			}

			if (remove)
			{
				function_127320(datum, 10);
				array = g_4e637c;
			}
			else
				element->unknown8c = NONE;

			datum = data_datum_index(array, function_16bc00(array, datum == NONE ? 0 : index + 1));
		}

		if (!b)
			function_21f290();
		else
			function_125d60();
	}

	dword ticks = GetTickCount();
	s_4e6380 *globals = g_4e6380;
	globals->unknown80 = ticks;
	globals->unknown1f8 = NONE;
	globals->unknown200 = NONE;
	globals->unknown84 = 0;
	g_4ed288->value20 = 0;
	g_4ed288->value24 = 0;
	if (!b)
		function_21a1e0();
}

// @retail 0x14b660
void __stdcall function_14b660(dword flags)
{
	struct
	{
		long a[2];
		long b;
		byte c[0xe40];
	} locals;

	if (flags & 8)
	{
		function_23654b(locals.c, locals.a, &locals.b);
		function_152f80(locals.a, locals.c);
	}
}

// @retail 0x155360
void __stdcall function_155360(dword flags)
{
	function_155380();
	function_155a30(*g_4e8c34);
}

// @retail 0x155f10
void __stdcall function_155f10(dword flags)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = 0;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = 0;
			g_4e6388 = 0;
		}
	}
}

// @retail 0x16eff0
void __stdcall function_16eff0(dword flags)
{
	if ((bool)((~(flags >> 4)) & 1))
	{
		long i;

		g_510c70[0] = g_4686c4;
		for (i = 0; i < 4; i++)
		{
			bool valid = false;

			g_510c70[i + 1] = NONE;
			if (i != NONE)
				valid = g_4e8c20->entries[i] != NONE;
			if (valid)
			{
				struct
				{
					byte unknown[4];
					short value;
				} result;

				function_11bed0((s_location *)&result, (point3f const *)&g_4e9bd4[i].state);
				g_510c70[i + 1] = result.value;
			}
		}
	}
}

// @retail 0x16f090
void __stdcall function_16f090(dword flags)
{
	g_4ea934 = 1;
	for (long i = 0; i < 4; i++)
		function_16f4b0(&g_4e9bd4[i]);
}

// @retail 0x16f0c0
void __stdcall function_16f0c0(dword flags)
{
	long mode = g_4e6948->mode;

	if (mode < 2 || mode > 5)
		g_4ea936 = 1;
}

/* the next used index at or after index, or NONE */
static __forceinline long record_pool_next_used_index(s_record_pool *array, long index)
{
	if (index >= 0 && index < array->high_water_index)
	{
		long count = array->high_water_index;
		dword *bits = array->bitmap;

		do
		{
			if (bits[index >> 5] & (1 << (index & 0x1f)))
				return index;
			index++;
		} while (index < count);
	}
	return NONE;
}

/* steps the iterator to the next used datum; its address, or NULL at the end */
static __forceinline byte *record_pool_iterator_next_used(s_record_pool_iterator *iterator)
{
	s_record_pool *array = iterator->data;
	long index = record_pool_next_used_index(array, iterator->index + 1);

	if (index == NONE)
		return NULL;

	byte *datum = array->data + array->size * index;

	iterator->index = index;
	iterator->datum_index = (*(short *)datum << 16) | index;
	return datum;
}

// @retail 0x17d190
void __stdcall function_17d190(dword flags)
{
	if (g_4ea950->valid)
	{
		s_record_pool_iterator iterator;

		iterator.data = g_4ea950;
		iterator.index = NONE;
		while (record_pool_iterator_next_used(&iterator))
		{
			s_element_40 *element = ELEMENT(g_4ea950, s_element_40, iterator.datum_index);

			if (element->datum_index == iterator.datum_index)
				g_4c8798[element->table_index & 0xffff] = iterator.datum_index;
		}
	}
}

// @retail 0x1889d0
void __stdcall function_1889d0(dword flags)
{
	s_record_pool *array = g_4ed28c;
	long datum = data_datum_index(array, function_16bc00(array, 0));

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(array, s_element_18, datum);

		if (function_18d1c0(element->value) == datum)
		{
			long value = element->value;
			long other = function_18d1c0(value);
			if (other != NONE)
			{
				s_element_18 *other_element = ELEMENT(array, s_element_18, other);
				if (other_element->value == value)
					other_element->flags.flag5 = 0;
			}
		}

		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
}

/* releases the deleted looping sounds and the active ones a reset ends (with flags bit 6 the
   ones whose tag has an effect of type 0x20 or 0x26 stay), clears the tags' entries at +0x4c of
   g_51ebd4 and the sound globals, and with flags bit 5 updates the listeners once */
// @retail 0x188ac0
void __stdcall function_188ac0(dword flags)
{
	bool a = (flags >> 6) & 1;
	long datum = data_datum_index(g_4ed28c, function_16bc00(g_4ed28c, 0));
	s_tag_iterator iterator;
	long tag_datum;
	s_looping_sound_globals *globals;

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(g_4ed28c, s_element_18, datum);

		if (element->state == 2)
			record_pool_release(g_4ed28c, datum);
		else
		{
			word flags5 = *(word const *)&element->flags;
			if ((bool)((flags5 >> 5) & 1))
			{
				long value = element->value;
				if (!(function_18d360(value) && a))
				{
					if ((bool)((flags5 >> 6) & 1) || (*(byte *)g_4e3b44[value & 0xffff].flags & 2))
						record_pool_release(g_4ed28c, datum);
					else
						function_18d290(value, datum);
				}
			}
		}

		datum = data_datum_index(g_4ed28c, data_find_index(g_4ed28c, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}

	iterator.index = 0;
	iterator.signature = 0x736e6421;
	while ((tag_datum = function_122c70(&iterator)) != NONE)
	{
		byte *definition = (byte *)g_4e3b44[tag_datum & 0xffff].flags;

		if (definition[0xc] != 0xff)
		{
			long *entry = (long *)(g_51ebd4->entries + (char)definition[0xc] * 0x1c);

			if (entry)
			{
				entry[4] = NONE;
				entry[5] = 0;
				entry[6] = 0;
			}
		}
	}

	globals = g_4ed288;
	function_220fd0((s_sound_driver_volumes const *)globals->gains);
	memset(globals->slots, 0xff, 0x100);
	function_225ab0();
	if ((flags & 0x20) && !g_4ed288->flag241)
		function_18bb80(3.4028235e38f);
	g_4ed288->flag241 = 0;
}

// @retail 0x1c3540
void __stdcall function_1c3540(dword flags)
{
	if (!(flags & 1))
	{
		function_1c2b10();
		record_pool_release_all(g_51ebfc);
		record_pool_release_all(g_51ec00);
		g_47f058 = false;
	}
	if (flags & 2)
	{
		function_1c29d0();
		function_1c2890();
		g_47f059 = 0;
	}
	g_47f04c++;
}

// @retail 0x1c3590
void __stdcall function_1c3590(dword flags)
{
	g_47f04c--;
	if (flags & 2)
	{
		g_47f059 = 1;
		function_1c2910();
		function_1c39c0(g_47f048, 0);
		g_47f048->function_30be40((long)g_51ecac);
	}
	if (!(flags & 1))
	{
		g_47f058 = true;
		function_1c2a10();
	}
}

// @retail 0x1c7f80
void __stdcall function_1c7f80(dword flags)
{
	g_557c6c->proc2c = function_25dd20;
	g_557c6c->proc30 = function_25dd30;
}

// @retail 0x1e6af0
void __stdcall function_1e6af0(dword flags)
{
	function_1e75d0(0);
}

// @retail 0x226030
void __stdcall function_226030(dword flags)
{
	if (!(flags & 0x80))
		g_4701ec.stage = 0;
}


// Disabled: shared decal handles currently hold 256 entries; retail initializes 1024.
#if 0
#include "crc.h"
#include "physical_memory.h"
extern D3DResource *g_509444;

// Retail 0x43890
void function_43890()
{
    byte *state = (byte *)VirtualAlloc(0, 12, 0x101000, PAGE_READWRITE);
    if (!state) GetLastError();
    *(long *)state = 1;
    long size = 0x14000;
    game_state_globals.unknown8 += size;
    byte *buffer = (byte *)game_state_globals.base_address + 0x3fe000 - game_state_globals.unknown8;
    g_509444 = (D3DResource *)state;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
    *(byte **)(state + 4) = buffer;
    *(long *)(state + 8) = 0;
    if ((*(long *)state & 0x70000) != 0x20000) buffer = (byte *)((dword)buffer & 0x0fffffff);
    *(byte **)(state + 4) = buffer;
    c_data_allocator *allocator = g_510c2c;
    void *table = allocator->allocate(0x613c);
    function_13d170((s_physical_object *)table, "decal vertex cache", 0x500, 6, 1024,
        function_43820, (physical_block_busy_proc)function_43850, 0, allocator);
    memset(g_4c8798, 0xff, 1024 * sizeof(dword));
    g_509448 = (s_game_proc_table_509448 *)table;
}
#endif


s_handles_storage g_handles_storage;

#include "crc.h"
#include "physical_memory.h"
extern D3DResource *g_509444;

// @retail 0x43890
void function_43890()
{
    byte *state = (byte *)VirtualAlloc(0, 12, 0x101000, PAGE_READWRITE);
    if (!state) GetLastError();
    *(long *)state = 1;
    g_509444 = (D3DResource *)state;
    long size = 0x14000;
    game_state_globals.unknown8 += size;
    byte *buffer = (byte *)game_state_globals.base_address + 0x3fe000 - game_state_globals.unknown8;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
    *(byte **)(state + 4) = buffer;
    *(long *)(state + 8) = 0;
    if ((*(long *)state & 0x70000) != 0x20000) buffer = (byte *)((dword)buffer & 0x0fffffff);
    c_data_allocator *allocator = g_510c2c;
    *(byte **)(state + 4) = buffer;
    void *table = g_510c2c->allocate(0x613c);
    function_13d170((s_physical_object *)table, "decal vertex cache", 0x500, 6, 1024,
        function_43820, (physical_block_busy_proc)function_43850, 0, allocator);
    memset(g_4c8798, 0xff, 1024 * sizeof(dword));
    g_509448 = (s_game_proc_table_509448 *)table;
}
