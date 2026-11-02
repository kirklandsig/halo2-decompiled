#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

struct s_133f70
{
	long field_0;
	long field_4[4];
	byte field_14[4];
	long field_18[4];
	byte field_28[4];
	byte field_2c[4];
	byte field_30[0x18];
	byte *field_48[4];
	byte field_58[4];
	long field_5c[4];
	byte field_6c[4];
	byte field_70[4];
	byte field_74;
	byte field_75[4][16];
	byte field_b5[0xc6 - 0xb5];
	word field_c6[4][16];
	byte field_146[0x166 - 0x146];
	short field_166;
	byte field_168[2];
	short field_16a;
	bool field_16c;
	byte field_16d[3];
	dword field_170;
	byte field_174[0x194 - 0x174];
	point3f field_194;
	real field_1a0;
	byte field_1a4[8];
	dword field_1ac;
	dword field_1b0;
	real field_1b4;
	byte field_1b8[4];
	byte field_1bc;
	byte field_1bd;
	short field_1be;
	long field_1c0;
	long field_1c4;
	byte field_1c8[4];
	bool field_1cc;
	byte field_1cd[3];
	long field_1d0;
	bool field_1d4;
	byte field_1d5[3];
	real field_1d8;
	real field_1dc;
	real field_1e0;
};

typedef char c_133f70[sizeof(s_133f70) == 0x1e4 ? 1 : -1];

short function_2cbf0(long arg_1, long arg_2, short arg_3, dword arg_4,
	long arg_5, dword arg_6, long arg_7, byte *arg_8, short arg_9, byte arg_10,
	void *arg_11, long arg_12, real arg_13, long arg_14, long arg_15,
	bool arg_16, bool arg_17, bool arg_18, real arg_19, real arg_20,
	real arg_21, byte arg_22, point3f const *arg_23, real arg_24);

// score 16 (was 16): the 22nd argument of the 0x2cbf0 call is a word in retail but 0x2cbf0 declares a
// byte (that declaration is also in src/unknown_1321f0.cpp); field_2c[i] is computed before the
// field_4/field_5c pushes in retail. Round 58: natural loop with the record reset from the parameter
// 247 (parameter moves to the stack); record through the parameter/const source copy/record in the loop
// 247/247/247; volatile source copy 222; parameter as the per-pass record with a const original 169;
// with the caller's ternary object test and field order 109; with a dword feature-flags copy 21;
// with the valid index first 16 (kept); permuter 42 tries, best 13 only with a word flags copy.
// The copy is declared before the loop (16 either way) so the loop test never reads a copy out of scope.
// Earlier forms on the hand-reduced offset body (volatile copies; 155):
// Frame 0x214 agrees; delayed ESI save, source/count cursor and feature-byte homes differ.
// entry word count 162; entry native count 162.
// entry inner ordinal 162; repeated count location 162.
// selected-index default 187; conditional mask updates 163.
// first feature branch 197; second feature branch 195.
// frame agrees; source/count cursors and feature-byte homes differ.
// Shared byte/word feature results 216/210; independent bytes 193; Boolean bitwise AND 208;
// shared bytes plus source cursor 215; ordinary source pointer 232 (caller differs);
// bitwise predicates plus aggregate copy 210 (caller differs); unsigned handle 162.
// delayed ESI/source/count cursors and feature homes remain.
// Fast: native feature bits 206; common bit gate 211; packed bits 162; packed and independent 206;
// packed and common 211; byte bit results 216; bitwise bits 214; packed and bitwise 214.
// frame 0x214 agrees; early source ESI retention and count/cursor homes still differ.
// Source/index initialization swapped 162; ordinal initialized inside guard 163; offset
// declarations reversed 162; filter input captured 162; override query result 162;
// existing object getter 162; entry order plus getter/query result 162/162.
// Early source retention, count/cursor and feature-byte homes still differ.
// selection-row cursor 162f; word-row cursor 155p; both input cursors 155f; outer feature
// lifetimes 162f; outer decoded index 162f; outer encoded byte 162f; mask entry lifetime 162f;
// shared argument lifetimes 162f; cursor values name 155f; outer ordinal name 155f; inner ordinal
// name 155f; source pointer name 155f; cursor and ordinal role names 155f.
// early source retention, count cursors and feature homes remain.
// Actual source-array binding 191; repeated count-field binding 163; feature-word binding 162;
// present-first selected result 162; conditional selected result 162; source/count bindings 200;
// feature binding plus present-first result 162; actual selected-byte binding 162 (fast scores).
// early source retention, count cursor and feature homes remain;
// formal source/original record reference 191/191 (caller differs); selected copy source 210;
// category/object-category values 162/162; chained sentinel/disabled fields 162/162; inherited mask value 197.
// early ESI/source retention, count/cursor and feature homes remain.
// Native word row 221; cursor for-scope 155; word cursor with shared Boolean/byte,
// independent byte and complete-mask features 217/221/195/220; paired source/word
// cursors 156; word cursor with native record assignment 195.
// early source retention and source/count/feature homes remain.
// With the word cursor, native count/argument-5/argument-6/argument-8 arrays 195/161/160/161;
// count plus argument 5 186; arguments 6/8 193; all four arrays 187; native selection array 219.
// early source retention and scalar/feature homes remain.
// Composite-mask, optional-index and feature-byte names 155/155/155;
// mask/index, mask/features and index/features names 155/155/155;
// all argument names and source-handle/argument names 155/155.
// early source/count retention and feature-result homes remain.
// early source/count retention and feature-result homes remain.
// Existing word query 155; combined category/query gate 155; both features before selector with query/native tag 210/210;
// first/second feature early 206/206; features before optional-index result 201; query with while ordinal 155;
// conditional Boolean query 195. Original body, word cursor and both inherited local qualifiers retained.
// early source/count and feature-result homes remain.
// Entry active-record workspace 155; entry current-source workspace 155; packed-only
// source snapshot 155; memcpy-return selection 155; first/second conditional feature
// results 154/154; both conditional results 153; complete feature branches 217.
// early source retention and count/cursor/feature homes remain;
// entry-source count 196 and original filter 165 (caller differs); first-group ordinal 195;
// filter enabled first 206; signed feature bytes 195; integer group/record/both masks 195/201/205.
// Long/read-only-byte/read-only-long source elements 155 each; direct
// long/read-only-long source handles 155; dword optional index alone or
// with long/read-only-long source elements 155. Original typing restored.
// First/second/both conditional source bits 155/155/155; complete mask branches/source-order
// reversal 196/196; all mask parts named 155; independent word features 207; mask branches with
// word features 215.
// Unsigned count/selector/word/all offsets 155/155/155/155; wide inner
// ordinal 223; wide optional index 214; wide composite mask 192; word cursor
// before packed mask 154. One-point reduction rejected; original body retained.
// Actual low-byte independent/common/equality feature access 206/210/221; low-word
// access 158/204/220; byte/word access with native copied-field store order 206/158.
// Round 225: Signed-word outer/decoded/paired/inner/outer-inner/decoded-inner/all carriers [185, 189, 192,
// 195, 182, 197, 193]; unsigned-word outer 196. Early source/count and feature homes remain.
// Common disabled bool/byte/short/long with positive/negative feature results [192, 193, 192, 193, 201, 207, 192, 193].
// Original body, word cursor and inherited qualifiers restored.
// Entry count/selection/word-row offset lifetimes 156/154/154; count-selection/count-word/selection-word/all 156/156/156/156;
// all entry offsets with earlier real word cursor 155. Original body, cursor, observations and local qualifiers restored.
// Canonical shifted enable: independent 194; shared bool/byte/long 194/194/194; feature-first tests 194;
// independent/shared-bool/shared-byte bitwise conjunctions 219/219/219. Original body and cursor restored;
// caller preserved.
// Count/word/byte, byte/count/word, byte/word/count and word/count/byte
// declaration orders 154/155/154/155; same offset-step orders 155/156/156/155.
// Native workspace construction from original/current/formal source at admission 208/210/208;
// current source at iteration entry 210; the same four with existing word query 208/210/208/210.
// Original/formal-source forms also lose the matched caller; frames grow to 0x21c.
// Original conditional copy, query, cursor and local qualifiers retained.
// With retained word recurrence, ordinary source/index/both carriers 207/193/200; conventional while alone/source/index/both 197/209/177/201; conventional for 186. Original body, cursor and inherited qualifiers restored; several forms also disturb the matched caller.
// Round 354 natural-loop graph: feature word before selector 26; both feature results before selector 25;
// both with early word 192 (caller differs); conditional selector 16; decoded-selector name 16;
// both ordinal role names 16; category before selector/feature observation 133/199. Original restored.
// Round 77: word 22nd parameter in the definition and this declaration (diagnostic, third declaration
// unresolved) 8; on it: category through the original 207; bool category array 8; category local at the
// inner-loop top/before the call/before the inner loop/before the flag sample 134/8/195/198; instance
// local 8; byte category cast 8; callee category byte/word 47/15. The register setup of the last three
// arguments remains.
// Round 47: tag local 16; key local 16; tag/key in either order 16; category+tag 16;
// category+key 16; category+tag+key 16; category+key+tag 16; caller preserved.
// Lane 14 round 8: 0x2cbf0's level declared a word (as this caller pushes it) with the word field
// passed directly 8; not kept while the held src/unknown_1321f0.cpp declares level a byte (see 0x2cbf0).
// Round 15 (lane 15): short inner counter 253 (655 bytes); selected index as a ternary 16; NONE
// default then the selected byte 102; entry flags read before the selected byte 26; the bools
// set in both arms 95 (714 bytes). Both remaining causes are 0x2cbf0's prototype (level width,
// register-argument order), which the held file also declares.
// Round 28: flags field read at each use 110; flags computed per entry inside the j loop 187
// (684 bytes); struct assignment for the copy 16; category test through the original 16; first
// record tested `i == 0` 17.
// Round 37 (fast): the level passed without the byte cast 16; first/second declared before the
// flags copy 16; index declared before the selected byte 16; category or instance read through the
// original 206/201. Both causes stay BLOCKED-BY the held src/unknown_1321f0.cpp (0x2cbf0's prototype).
// @retail 0x133f70
void function_133f70(s_133f70 const *record, bool arg_2)
{
	s_133f70 const *const original = record;
	s_133f70 copy;
	for (long i = 0; i < record->field_166; i++)
	{
		record = original;
		if (i <= 0 || !arg_2 || *(long *)original->field_48[i] == *(long *)original->field_48[0])
		{
			if (record->field_1c4 == 1)
			{
				byte const *definition = *(byte **)(g_4e0300->data + (*(long *)original->field_48[i] & 0xffff) * 12 + 8);
				if (definition[0xaa] == 2 ? (bool)*(short *)(g_4e3b44[*(long *)definition & 0xffff].bytes + 0x290) : false)
				{
					memcpy(&copy, original, sizeof(copy));
					copy.field_1cc = false;
					copy.field_1c0 = 0x1f;
					copy.field_1c4 = NONE;
					copy.field_1d0 = NONE;
					copy.field_1d4 = false;
					copy.field_1d8 = 0.0f;
					record = &copy;
				}
			}
			dword flags = ((record->field_170 & 4) << 13) | ((record->field_170 & 0x40) << 7)
				| (record->field_70[i] ? 0x800 : 0) | (record->field_16c ? 0x400 : 0)
				| record->field_1ac | record->field_1b0;
			for (long j = 0; j < record->field_18[i]; j++)
			{
				byte selected = record->field_75[i][j];
				long index;
				if (selected != 0xff)
					index = selected;
				else
					index = NONE;
				dword entry_flags = record->field_170;
				bool first = false;
				bool second = false;
				if (entry_flags & 2)
				{
					first = (bool)((entry_flags >> 3) & 1);
					second = (bool)((entry_flags >> 4) & 1);
				}
				function_2cbf0(index, record->field_2c[i], record->field_16a, flags,
					record->field_5c[i], record->field_4[i], j, record->field_48[i],
					record->field_1be, record->field_1bc, NULL, record->field_1d0, record->field_1b4,
					record->field_1c0, record->field_1c4, first, second, record->field_1d4,
					record->field_1d8, record->field_1dc, record->field_1e0,
					(byte)record->field_c6[i][j], &record->field_194, record->field_1a0);
			}
		}
	}
}
