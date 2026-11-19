// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_189010.CPP: sound sources: whether a sound plays from an object
   of the vehicle type, and the source description a sound starts from */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "sound_records.h"
#include "object_markers.h"
#include "local_cameras.h"
#include <math.h>

long function_189ee0(long tag_index);
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);
void object_dispatch_sound_started(long object_index, long tag_index, long sound_index);
extern s_sound_source_callbacks const g_444b3c;

struct s_sound_tag_flags
{
	byte flags;
};

struct s_source_object
{
	byte unknown00[0x12c];
	byte flags12c;
	byte unknown12d[0x154 - 0x12d];
	long value154;
};

struct s_source_object_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_source_object *object;
};

struct s_sound_source_description
{
	char value0;
	byte flags;
	byte unknown02[2];
	point3f position;
	vector3f direction;
	long value1c;
	long value20;
	long value24;
};

static inline s_source_object_header *source_object_header(long object_index)
{
	return (s_source_object_header *)g_4e0300->data + (object_index & 0xffff);
}

// @retail 0x189010
bool function_189010(long tag_index, long object_index)
{
	bool result = false;

	if (tag_index != NONE && (((s_sound_tag_flags *)g_4e3b44[tag_index & 0xffff].bytes)->flags & 0x40) && object_index != NONE)
	{
		if ((1 << source_object_header(object_index)->type) & 4)
		{
			result = true;
		}
	}
	return result;
}

dword vector3d_compress(vector3f const *vector);
struct s_sound_label_play;
long function_1890c0(s_sound_label_play const *play, long object_index, short value, point3f const *position, vector3f const *direction);
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale);

// @retail 0x1892a0
void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description)
{
	description->position = *position;
	description->direction = *direction;
	description->value24 = NONE;
	description->value0 = value == NONE ? (char)0 : (char)value;

	if (object_index != NONE && function_189010(tag_index, object_index))
	{
		s_source_object_header *header = source_object_header(object_index);
		if ((1 << header->type) & 4)
		{
			s_source_object *object = header->object;
			if (object->flags12c & 1)
			{
				long value154 = object->value154;
				if (value154 != NONE)
				{
					description->value24 = value154;
				}
			}
		}
	}
}

struct s_sound_class_play_flags
{
	byte unknown00[8];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
	word unknown0a : 7;
	word flag23 : 1;
	word : 8;
};

struct s_sound_play_tag
{
	byte unknown00;
	byte flags;
	char class_index;
	byte unknown03[0x10 - 3];
	long duration;
};

struct s_unknown_5c;
s_unknown_5c *function_221810(short index);
long function_18d5b0(long label);
void function_18d4f0(long object_index, char *audible, long *local_player_index);

static __forceinline long game_seconds_to_ticks_round(real seconds)
{
	real ticks_real = (real)g_510c54->field_2_3 * seconds;
	long ticks;

	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

// score 32 (was 89): what still differs is the end test only (460 B against 470): retail tests bit 8 of the location
// flags as a byte (`test byte [esp+0x21], 1`) and bit 11 by a dword load and shift, ours merges both into `test ch, 9`.
// Every spelling that gives retail's test moves the request from EBX to EBP and onto the stack (45 at 476 B, callers lost).
// MiniMax H (11-18), then at 106 (from 150): the priority is read from the request's audible byte, then the location is
// copied, then the flag word is built, which is retail's order, and the source size's zero default is stored before the
// marker's own assignment instead of in an else arm; what still differs: retail keeps the sound pointer in a frame
// slot and reloads it, this build keeps it in EBP and pushes EBP at the start; retail loads the source before the tag lookup and builds
// the flag word in EAX, this build builds it in EDX afterwards; retail's object and platform bits use the or/and
// complement form; retail's end test takes bit 8 as a byte and bit 11 by a shift where this build merges both into one
// word test; the first 0x221810 lookup is still expanded here where retail calls it out of line (that helper's
// inline-ness is set in src/unknown_221490.cpp) and the 0x21d110 call still takes the play state by pointer on the
// stack where retail passes it in a register (that callee's declaration is in another file).
// BLOCKED-BY 0x221810 and 0x21d110.
// Earlier at 150 (from 110): The 0xa8 frame agrees. The first class lookup still expands, and playback state still passes on the stack.
// Native short priority and padding by separate stores, chain first, after flags, or before location 162/162/163/163.
// Native priority and padding published before or after flags 152/152. Complete priority copy at the original store 150.
// NOT kept: complete priority copy before flags 139 requires a one-use priority temporary.
// MiniMax E (11-17): no new form, no lower score (110): the first 0x221810 lookup is still expanded in this body where retail calls it out of line, and the 0x21d110 call still takes the play state by pointer on the stack where retail passes it in a register; both are decided outside this file. BLOCKED-BY 0x221810 and 0x21d110.
// MiniMax H (11-14): no new form, no lower score (110): retail passes the class index to the first
// lookup as a sign-extended byte, and whether that lookup is a call or an expansion is settled in
// src/unknown_221490.cpp, which is not one of this lane's seven files; the body is unchanged.
// MiniMax E (11-14): no new form, no lower score (110, was 110): the first 0x221810 lookup is still expanded in this
// body where retail calls it out of line, and the 0x21d110 call still takes the play state by pointer on the stack
// where retail passes it in a register; both are decided outside this file. The request is still copied field by field
// where retail copies the whole request and patches the flags after.
// MiniMax D (11-14): no new form, no lower score (110): our build still expands the first 0x221810 lookup inline where
// retail calls it out of line (that helper's inline-ness is set in src/unknown_221490.cpp), and the 0x21d110 call still
// takes the play state by pointer on the stack where retail passes the state in a register (that callee's declaration is
// in another file); the request is still copied field by field where retail copies the whole request and patches the
// flags after. BLOCKED-BY 0x221810 and 0x21d110.
// MiniMax E (11-11): no new form, no lower score (110): the request is still copied field by field
// into the play state, where retail copies the whole request and patches the flags after.
// MiniMax B (11-12): no new form, no lower score (110); our build still expands the first 0x221810 lookup inline where retail calls it out of line (that helper's inline-ness is set in src/unknown_221490.cpp, a file this lane may not edit), and the 0x21d110 call still takes the state by pointer on the stack where retail passes the state in a register (that callee's declaration is in another file); the header order, the flag word and the end-time publication were measured again and left as they are.
// MiniMax A (11-13): no new form, no lower score (110); what still differs: the first 0x221810 class lookup is
// expanded in the body here and called out of line in retail, so the flags and the request are forwarded across that
// call where retail reloads them, and the 0x21d110 call passes the state by pointer on the stack where retail passes
// it in a register; the lookup's inline-ness (src/unknown_221490.cpp) and 0x21d110's declaration
// (include/sound_records.h) are outside this file: BLOCKED-BY 0x221810 and 0x21d110.
// MiniMax R (11-15): no new form, no lower score (110): the first 0x221810 lookup is still expanded here where retail calls it out of line, and the 0x21d110 call still takes the state by pointer on the stack where retail passes it in a register; a 300-second permuter run over this body (33 tries) found nothing better than its 107-instruction baseline on either image. BLOCKED-BY 0x221810 and 0x21d110.
// MiniMax R (11-15, second pass): no new form, no lower score (110): retail's end test tests the flag 8 byte bit,
// then shifts flag 11 out of the word and then tests the audible nibble (test byte ptr [x], 0xf), while this build
// merges the two flag bits into one byte test; the first 0x221810 lookup is still expanded here where retail calls
// it out of line and the 0x21d110 call still takes the state by pointer on the stack where retail passes it in a
// register. BLOCKED-BY 0x221810 and 0x21d110.
// MiniMax I (11-17): no new form, no lower score (150): still BLOCKED-BY 0x221810 and 0x21d110, whose definitions
// are in other files: LTCG expands the first 0x221810 lookup here where retail calls it out of line, and 0x21d110
// takes the tag in EAX and the state on the stack where retail passes the state in a register; the forms that give
// 0x21d110 retail's convention move this function's request to EDX and cost its eight matched callers.
// Earlier 150 (was 110): BLOCKED-BY 0x221810 and 0x21d110 (callee side): LTCG inlines the first
// 0x221810 call (retail calls both; its file is not this function's) and 0x21d110 (unmatched) takes the
// tag in EAX and the state on the stack where retail passes the state in EAX; the forms that give 0x21d110
// retail's convention move this function's request to EDX and cost its eight matched callers. Round 71:
// the end test as `((flags >> 8) & 1) || ((flags >> 11) & 1)` 110 (VC merges both into `test ch, 9`; retail
// tests the byte bit 8, then shifts out bit 11). In the current header order (plain checks): set-or-clear
// object and platform bits 148 (486 bytes), object only 131 (478), platform only 134 (481), both with the
// priority from the request's audible field 147 (485). Retail's full header order
// (priority from the request byte, set-or-clear bits, then the location copy) is listed below with its scores.
// MiniMax D (11-10): the priority read from the request and stored before the location copy scores
// 106, the same form with the flags computed before the copy 139, with the source in a local after the
// sound lookup 134, with the object index in a local 136 and with the object and platform bits as
// or/and complement selects 163; none of the gains is enough to keep, so the body stands. The fast
// permuter found nothing better than 103 differing instructions against its 104 baseline, and the
// variant it preferred spells the state fields with volatile casts and one-use temporaries.
// MiniMax E (11-10): no new form, no lower score (110); the first difference is at +0x15, where retail reloads the state flags after the call that our link still expands, so the flags are still forwarded across it.
// MiniMax E (11-09): no new form, no lower score (110); the priority and the flag word are
// still formed after the location copy, and the first 0x221810 call is still expanded by our link.
// MiniMax I (11-08): no new form, no lower score (110).
// MiniMax B (11-06): no new form, no lower score; retail reads the audible byte from the request and
// builds the priority and the flag word before it copies the request, this build copies the request first
// and takes the byte from the copy, and retail still calls 0x221810 where this link expands it.
// MiniMax C (11-06): no new form, no lower score; this link still inlines 0x221810 so the state flags are forwarded across the call where retail reloads them (0x221810 lives in src/unknown_221490.cpp).
// MiniMax E (11-06): score 110 (was 110): no new form lowered it. The location copy after the flags
// (retail's header order) with the priority before or after the flags gives 140/150 and the flags as
// ternaries 160; the priority read from the request alone is 109 at 472 bytes, a gain of one, dropped
// by the review rule, so the original body stays. Still different: retail builds the flag word in EAX
// with the source loaded before the tag lookup and ours in EDX, retail's object and platform bits use
// the or/and complement form and ours only or, retail homes the sound pointer at [esp+4] where ours
// keeps it in EBP, and retail copies the location after the flags where ours copies it first.
// MiniMax D (11-04): no lower score. With the source in a local declared before the sound lookup our
// load order is retail's (the tag table's base, the source, then the lookup) and the priority comes from
// the request byte, but the link still gives EAX to the source and EDX to the tag table's base where retail
// gives EAX to the base and EDX to the source, so the flag word is built in EDX; that form scores 135 at
// 486 bytes, and 149 at 499 bytes with retail's set-or-clear object and platform bits. With the sound
// lookup written first the base keeps EAX, but then the source's load lands after the lookup. The priority
// read from the request alone is 109 at 472 bytes, and the permuter's best form (the 0x400 bit set after
// the variant stores) is 109 as well; both gains are too small to keep. Body restored to the original.
// MiniMax G (11-04): the priority read from the request and stored before the location copy 106 (a gain of
// four, dropped by the review rule); the end test split into the 0x100 and the 0x800 masks, or written as a
// shift, 106; the class test as nested ifs 106; the class lookup nested under the sound-flag test 160 (446
// bytes: both lookups stay out of line, but the tag parameter is then copied into the frame); an always-true
// test around the first class lookup 157. Body restored to the original.
// MiniMax A (11-03): no new form, no lower score: the priority and the flags are still formed after the
// location copy, and our link still expands the first 0x221810 call.
// MiniMax S (11-01): no new form, no lower score; the priority and the flags are still formed after the location copy, and our link still expands the first 0x221810 call, so the state flags are forwarded across it where retail reloads them.
// MiniMax S (11-09): no new form, no lower score (110); the priority and the flags still form after the location
// copy, and the first 0x221810 call is still expanded by our link.
// Earlier score 110 (was 110): this round: the fast check reads 110 at 473 bytes against retail's 470 again
// frame 0xa8 and request EAX agree; first query expansion, header/result homes and playback convention differ.
// Earlier 110 (was 110): this round: the fast check reads 110 at 473 bytes against retail's 470 again
// and no new form was tried; the difference is still blocked by 0x221810, which retail calls twice
// and our link inlines, in src/unknown_221490.cpp.
// Earlier 110 (was 110): this round: no new form. Still BLOCKED-BY 0x21d110/0x221810 (held files):
// retail homes the request in EBX and reloads the state flags after the first 0x221810 call,
// ours forwards them across it and builds the flag word in EDX where retail builds it in EAX.
// Earlier 110 (was 110): Frame 0xa8 and incoming request agree; class-query expansion and state/flag/result homes remain.
// Earlier: this round: no new form. Still BLOCKED-BY 0x21d110/0x221810 (held files):
// Earlier: Frame 0xa8 and incoming request agree; class-query expansion and state/flag/result homes remain.
// Frame 0xa8 and request EAX agree; first class-query expansion and header homes differ.
// BLOCKED-BY 0x21d110 and 0x221810 (their tag/state registers): retail homes the request in EBX and
// reloads the state flags after the first 0x221810 call, ours forwards them in a callee-saved
// register across it, and retail builds the flag word in eax where ours builds it in edx.
// frame 0xa8 and the request in EAX agree; the first class lookup expands
// where retail calls it (and 0x21d110 takes the tag in EAX where retail passes the state there), so
// the flag/priority/result homes, push placement and the end-test spill differ. Round 54: the
// seconds-to-ticks rounding helper with the looping-sound global read at each use 110 kept
// (helper alone 115); bitfield end tests 137 and on the kept form 122 (each loses eight callers);
// mask bit 8 with bitfield bit 11 132; renamed rounding temporary 110; priority from the request's
// audible field 109; that with the location copy after the flags 140. Earlier: word flags 158;
// index-order variant guards 134; flag11 field store 134; combined location mask 118 kept;
// original priority observation 133; priority arms 182; nested variant tests 182; shared exit 182;
// marker memcpy 182; base-first flags 178 (caller losses); role names 182; source snapshot 178;
// object snapshot 183; priority after flags 192; conditional flags then priority 187; deferred
// flags publication 176; location first with copied audible priority 167 kept; location memcpy
// 182; conditional end-time publications 175; outer state scope 167; early optional-size default
// 164; size before marker copy 167; nested class tests 167; platform-flags block first 167;
// platform index snapshot 167; redundant object/platform clears omitted 149 kept; scoped header on
// clean flags 149; native flag8 conversion 157 (eight caller losses); repeated class index on
// clean flags 159; location/priority phase after flags/header 188/194; first/shared class
// receiver 167/167; seconds/ticks terms 167/167; phase with seconds 188; shared receiver with
// seconds 167; late tag receiver 184; branch-assigned result 193; shared marker observation 167;
// absent-marker-first 165; native short priority and padding memset 176; whole source flags 149;
// clean state flags 169 (eight caller losses); variant fields before flag 149; audible-first
// admission 149; post-query marker default 134 kept; size conditional 147; looping global reads
// 130 fast/135 plain; time-global reads 134; size override before copy 134; whole
// request/tag/class records and combinations 181/196/228/146/197/215/247 (request/tag copies lose
// eight callers); first class record 165; consistent variant-pointer field reads 118; disjoint
// source mask OR 118; conditional optional flags 161 and full flag expression 162 (eight caller
// losses each); nested class admission 118; real class-word mask with direct query 135;
// state/result/flags/both names 118/118/118/118; early object/source/platform/all stores
// 118/135/118/133; swapped object-source/source-platform stores 118/118.
// Round 187 named duration/source expression parts and both 110/110/110; complete fresh
// tag lookup alone and with those parts 158 each; all plus signed native flags 158. Restored.
// Round 93: with 0x21d110 starting its sounds inline (state in EAX; 0x21d110 204 -> 52) the request
// moves to EDX: 135 at 470 bytes, each caller one line off. A noinline declaration of 0x221810 in
// this file leaves the first lookup expanded; one ahead of its definition calls both lookups
// (0x129aa0 101 -> 4) but this function then takes both arguments on the stack (156, 450 bytes;
// with set-or-clear object/platform flags, the priority read from the request and flag8/flag11
// end tests 132 at 470 bytes). Those body forms with the inline start alone 153, alone 163.
// Round 95 (plain checks; --fast builds this function differently): the marker size set in an
// if/else 135 (475 bytes, aligned 94 from 97); that with the priority and flags formed from the request
// before the location copy 150 (487 bytes, aligned 74); that with a noinline declaration ahead of
// 0x221810 149, stack arguments, eight callers lost; with set-or-clear object/platform flags 166 (500
// bytes, aligned 73); that with flag8/flag11 bitfield end tests 191, stack arguments, eight callers
// lost. Whenever the body homes the request in EBP, LTCG passes both arguments on the stack; homed
// in EBX it arrives in EAX (retail). Retail reloads the state flags after the first 0x221810 call (its
// compiler treats that call as opaque); ours forwards them in a callee-saved register across it,
// which pushes the request into EBP. BLOCKED-BY 0x21d110 (its tag/state registers) and 0x221810.
// Round 410: Joint source/object, source/platform, object/platform and all-three two-use header
// snapshots 110/110/110/110; each with priority from the request 109/109/109/109 (472 bytes).
// Reduced-image context differs; all one-point gains are rejected. All restored; first class-query
// expansion, header homes and playback convention remain.
// Round 419: existing rounding helper with explicit short/long/real rate inputs, each
// rate first/last, and native millisecond input in both orders 110/110/110/110/110/110/110/110.
// All tie; original helper and body restored. Query expansion, state/flags/result homes and
// clock-conversion spill remain.
// Round 194: native source-location mutable/const with source/copied priority 181/181/110/110;
// the same four with library publication 181/181/110/110. Original location-first
// body restored; input ownership regresses, and single-consumer copies only tie.
// Round 85: retail-order header with conditional object/platform clears 163 (498 bytes);
// separate location masks 110; the high-byte flag8 view 128 (486); an ordinary inline query
// declaration 110; state/tag declarations swapped 110; tag pointer renamed 110; immutable tag
// pointer 110; early uninitialized flags 110; split tag declaration/assignment 110. All restored;
// first query expansion, initial tag spill and playback convention remain.
// Earlier 110 (was 110): this round: no new form. The first class lookup is still expanded here and
// Earlier: this round: no new form. The first class lookup is still expanded here and
// called in retail: our link inlines 0x221810 (src/unknown_221490.cpp, matched) into it, so our
// compiler forwards the state flags across that call, while retail treats the call as opaque and
// reloads them. Suppressing the inline (inline depth zero) costs six matched callers of this
// function. Still BLOCKED-BY 0x221810 and by 0x21d110 (src/unknown_221490.cpp), whose tag and
// state registers decide where the request is homed.
// MiniMax R (10-31): no new form, no lower score.
// MiniMax I (11-01): no new form, no lower score; the priority and the flags are still formed after the location
// copy, and our compiler still forwards the state flags across the 0x221810 call where retail reloads them.
// MiniMax I (11-01, second pass): no lower score. Forming the priority and the flags before the location copy,
// with retail's set/clear branches for the two flag bits and the priority read from the request, reproduces
// retail's shape (140-149 against 110, 485-499 bytes against retail's 470): our link then inlines the first
// 0x221810 call, so the body grows past retail's, and the flags still land in EDX where retail builds them in
// EAX. The original order stays.
// Round 446: native 64-bit result/optional-flags/end-tick transport, seven subsets
// 157/137/166/207/248/234/256; all with unsigned result 256. Original domains, helpers and body restored.
// DL218: Complete query/playback/both inline contracts 110/110/110; query and combined contracts
// lose matched control 0x129790. Playback contract with first/shared query receivers 110/110,
// complete marker-size arms 135, request priority 109 and combined marker/priority 134. One-point
// gain rejected; all declarations, helpers and original body restored. Frame 0xa8, first query
// expansion, state/flags/result homes and playback convention remain.
// Round 300: completed-time destination captured after/before rounding with
// ternary/first/second/both conditional publications 115/113/113/119 and
// 121/119/119/125; original per-use destination with first/second/both
// conditional stores 113/108/119. Two-point 108 rejected; original body restored.
// Round 122 (fast): the link-time inliner weighs the call site, not this body's size. 40 extra statements after
// the second lookup 133 (476 bytes), before the first 139 (486), first still expanded; second lookup removed 143
// (456), first still expanded; first lookup under one more conditional (diagnostic, a redundant test) 140 (476):
// both lookups become calls as in retail, arguments on the stack, request in EBP; first lookup after the variant
// block 154 (491); `sound->flags` test dropped 129; receiver local 110; second lookup written like the first 136;
// first test in a static inline helper 173 (433), forceinline 110; class flag alone in a helper 163; class getter
// helper 110; the lookup's header helper as if/else with named locals 131 (0x221810 17, 0x129aa0 96), as nested
// inline getters 110. Restored: retail's first lookup would need conditional depth two or a costlier callee.
// Lane 14 r55 (fast): an early return for a NONE tag 152 (483 B); the tag's flag tested ahead of the class lookup
// 160 (446 B; both 0x221810 calls then stay out of line as in retail, but in the wrong test order); one sound_class
// local for both lookups 110 (same code); the flags built in state.flags 150 (484 B); the flags as one OR of three
// conditional terms 160 (490 B).
// Round 35 (m3 lane 18, fast with the matched callers): the variant pointer read from the request at
// each use 110 (same code); the flag word kept in its local through the variant test and stored just
// before the playback call 130 (470 bytes); that with the priority from the request's audible field 130
// (470 bytes); the object/source/platform stores before the flag word 134 (0x1895f0 4, 0x189a50 3);
// an early `return NONE` for a NONE tag 152 (483 bytes). All restored.
// M3 lane 14 r59 (plain checks): the end test as `(flags & 0x100) || (bool)((flags >> 11) & 1) || !audible` 132
// (486 bytes; the flags stay in ESI); the first class flag named in a bool before its test 110 (same code); the
// +0xa class bits as word bitfields (`word unknown0a : 7; word flag23 : 1; word : 8;`) 132 (478 bytes). Retail's
// header order (priority from the request, set-or-clear object/platform bits, location copy after the flags) with
// the first lookup under a repeated tag test (diagnostic only) 132 at retail's 470 bytes: both 0x221810 calls stay
// out of line and the body takes retail's shape, but the request then arrives on the stack and is homed in EBP;
// that with the word bitfields 135 (471 bytes), whose class test becomes retail's `shr al, 7; test al, 1` (the
// byte bitfield gives `test cl, cl; jns`), so retail's +0xa bits are word bitfields like flag1/flag2.
// Round 106 (plain): nested class condition, bool/long end-update results, previous value20/value24/both and
// disjoint source mask 110 each; word +0xa 132 (478), marker-size local 131 (470), location-flags local 111 (474).
// Original body restored.
// Round 154 (m3 lane 6, fast with its 8 matched callers): the class lookup as a shared header helper (if/else,
// named locals, both) 110 (same code); header = NULL then if 131; tag test as == NONE 152; class test through a
// named bool / inverted / nested 110; flag as a ternary OR 136; retail's header order alone 163. The lookup's inline
// cost is not lexical (named locals change nothing) and the file already has /Ob1; retail inlines 0x221810 at hot
// sites (0x20b660's loop), so here it needs a colder site than one `if`. Body unchanged.
// M399: Existing post-playback class input read-only, immutable variant binding/completed-time producer,
// seven nonempty subsets 110 each; all with immutable tag-index formal 110. NOT kept: every form ties.
// Original whole body restored; early source/header work, first query expansion and playback register homes remain.
// M401: Playback definition/declaration/sole call reordered together to (tag, state), default/stdcall/cdecl/fastcall,
// then the same four with inline metadata: 110/110/134/110/110/110/134/110.
// NOT kept: native EAX state/stack tag setup is not recovered; all interface, provider and caller changes restored.
// Frame 0xa8 agrees; early header work, first class-query expansion and playback homes remain.
// M3 lane 11 r81 (plain, with the eight matched callers): flag base as `5 + (source ? 0x20 : 0)` 110, as `source ? 0x25 : 5` 110,
// as `long` 110, end time as `game_time + 10 + ticks` 110 (all same code); the end test reading the audible byte from the request
// 135 at retail's 470 bytes, but the request stays live and all eight matched callers are lost.
// Round 82 (m3 lane 1, fast with the eight matched callers, 0x221810 and 0x21d110; the fast image reads 110 for this body):
// retail's whole header at once, the priority from the request's audible byte, the flag word started at 5 with three
// set-or-clear steps (source bit 5, object bit 3, platform bit 8, the file-local macro other files use), the location copied
// after the flag store and the marker size set in an if/else: 166 (500 bytes; the first 0x221810 call is still expanded);
// that with the +0xa flag bits as word bitfields 173 (505); the three steps applied to state.flags itself 170 (499; the
// request then arrives in another register and all eight callers differ by 1 to 4); the local form with word bitfields and
// the end test split into bit 8, bit 11 and the audible test 190 (518); the state.flags form with both 193 (516, callers
// differ). Retail calls 0x221810 out of line at every site (see its comment in src/unknown_221490.cpp); giving it a file of
// its own is a build-flag decision this lane does not make. Body unchanged.
// Round 93 (m3 lane 1, plain with the eight matched callers; --fast builds this body differently): an early return
// when the start fails 143 (478 bytes); the start result assigned inside its test 110 (same code); the result declared
// at the start call, returned inside the tag block with `return NONE` after it, 134 (488 bytes); the object bit as
// `(dword)(object_index != NONE) << 3` 141 (476 bytes, all eight callers lost: the request convention moves); the
// priority as `(audible ? 2 : 0) + 1` 110 (same code). Re-measured, not new: the variant pointer read from the
// request at each use 110. Still BLOCKED-BY 0x221810 (inlined at the first site here, called in retail) and
// 0x21d110 (whose note names this function as its blocker: the two conventions have to move together).
// MiniMax A (11-11, second pass): no new form, no lower score (110); what still differs is unchanged: the request is still copied field
// by field where retail reloads the state flags after the call, because our build expands the first 0x221810 lookup inline (that helper's
// inline-ness is set in src/unknown_221490.cpp, a file this lane may not edit) and the 0x21d110 call still takes the state by pointer on the
// stack where retail passes it in a register (that callee's declaration is in another file).
// MiniMax A (11-11): no new form, no lower score (110); our build still expands the first 0x221810 lookup inline where retail calls it out of line (that helpers inline-ness is set in another file and was ruled off limits), and the 0x21d110 call still takes the state by pointer on the stack and the tag index in the register where retail passes the state in the register and the tag index on the stack (that callees declaration is in another file).
// M3 lane 13 r92 (fast with the ten matched callers, 0x221810 and 0x21d110; control 110): the priority read from the request
// and stored before the location copy 106 (472 bytes, as MiniMax D/G found); on it, the end test as `(flags & 0x100) ||
// ((flags >> 11) & 1) || !audible` 106 (same code: still `test ch, 9`), the variant read from the request at each use 106,
// the object/source/platform stores ahead of the flag store 106, the marker size in an if/else 131 (474 bytes). The gains do
// not stack; no caller lost in any form. Retail's 0x221810 has a stack argument (`ret 4`) and is called at every site, so it
// behaves like a function LTCG never expands; ours is expanded at the first site here only.
// MiniMax I (11-16): no new form, no lower score (110): this build still expands the first 0x221810 lookup inline where retail calls it out of line - that helper's inline-ness is set in src/unknown_221490.cpp - and the 0x21d110 call still takes the state by pointer on the stack where retail passes it in a register, so the request copy's field order still differs; the priority read ahead of the copy reaches 106, a gain of 4 that is below the keep bar. BLOCKED-BY 0x221810 and 0x21d110.
// b9G (11-17) walk against retail, fast build on r839: fields, offsets, test senses, the 0x221810/0x21d110 arguments,
// the 0.001f rate (0x45dc70) and the sum order (duration * 0.001f) * rate agree; no behaviour difference found. Body now
// takes retail's order: the priority is read from the request byte before the copy (0x18a012), the flag word is stored
// (0x18a055) and the location copied after it (0x18a067), and the marker size is one if/else (0x28 arm 0x18a0ae, 0 arm
// 0x18a0ba) where it was a store overridden by the arm. Fast build 150 at 487 bytes (was 110 at 473): header and marker
// line up with retail and the rate/ticks temps now share its frame slots; the excess is the inlined first 0x221810 and
// the 0x21d110 convention. Alone: priority from the request 106 (old order), if/else marker 135, both with the old
// location-first order 134; retail's order with set-or-clear bits `? (flags |= 8) : (flags &= ~8)` (retail 0x18a035-
// 0x18a04b) 166 (500 bytes). Still differing: retail reloads the flags from the state at 0x18a0da (its 0x221810 call
// clobbers the address-taken state, ours forwards them), and tests bit 8 as a byte, bit 11 by shift (0x18a183-0x18a191).
// m3 lane 7 r178 (fast with the ten matched callers; worked as 0x21d110's blocker): body unchanged (150); HELD with 0x21d110 in
// build/r178/held-21d110.patch (full check of that configuration: nothing lost, 0x21d110 gained, this function 32). With 0x21d110 in
// its matching form (state in EAX) and the first 0x221810 lookup out of line (inline depth 0 as a diagnostic; 0x221810 in a file of its
// own built /Ob1 gives the same code): this body 149 (456 B), both arguments on the stack and the request in EBP, because the flags
// local is forwarded in EBX across the lookup; the flags local with set-or-clear steps 124 at retail's 470 B, same registers; the
// flags built in state.flags itself by three set-or-clear steps 129 (457 B) with `push ebx; mov ebx, eax`, the ten callers and 0x21d110
// MATCH. On that: source in a local 126; with the object index in a local 126; source, object index and an early priority local
// stored after the steps 98; object index and priority locals 108; the priority assigned after the steps 136, with the two other
// locals 136; the 98 form with the +0xa bits as word bitfields 32 (460 B: 18 real lines, all in the end test), the same without the
// object index local 32 (the held form). End test on it: `(flags & 0x100) || (bool)((flags >> 11) & 1) || !audible` 79 at 470 B
// without the word bitfields and 60 (473 B) with them; the flag8 bitfield with a dword read, with the word read, the mask with a dword
// read and two bitfield tests 60 each (the two reads share one load in ESI and the result moves to EDI). A byte read of the second
// flags byte with a dword read, by casts or by byte and dword views added to the location's type in its header, gives retail's end
// test line for line but swaps EBX and EBP: the request is then loaded from the stack into EBP, 45 at 476 B, callers lost; the same
// with or without the source local and with the object index declared first 45 each; state declared first, source first, priority
// ahead of the source 45 each; source and priority ahead of the tag pointer 88. Without the out-of-line lookup (matching 0x21d110,
// request in EDX, callers lost): source in a local 137, with the object index local 137, source ahead of the tag pointer 137,
// set-or-clear steps on the flags local 156, the flags in state.flags 161 (499 B). The setup as an inline helper taking the state, the
// request and the tag: `static inline` by pointer or by reference is not expanded (211 B, 396); forceinline expands the lookup again
// (161). On the byte-read end test: the marker pointer in a local 45, the lookup's record through a reference 45, the priority
// stored first with no local 115, the variant bytes read through the local pointer only 88 (470 B); the request stays in EBP.
// Left: the end test (retail tests bit 8 as a byte, then loads the dword for bit 11) without EBX going to the object index.
// Coordinator (11-18): with 0x221810 called out of line from its own /Ob1 file (0x21d110's match) and the flags set bit by bit
// on state.flags (the source read once), 89 at 458 B (was 106 at 472 B).
// M3 lane 3 r261 (fast with the ten matched callers, 0x221810 and 0x21d110, all MATCH; control 89): the marker size as
// an if/else (0x28 in the marker arm, 0 in the else, retail 0x18a0ae/0x18a0ba) 32 at 460 B, plain 32 with every caller
// matched: kept. On the 89 body the end test through the location's word bitfields `flag8 || flag11 || !audible` 89 and as
// `(flags & 0x100) || (flags & 0x800) || !audible` 89 (same code). On the 32 body: an inline location-flag test taking the
// bit index, used for bits 8 and 11, 45 (476 B; retail's end test line for line, but the request arrives on the stack
// and lives in EBP, the object index and the sound pointer in EBX, and all ten callers differ by 1 byte); a word-flags
// test helper `(*flags & (1 << bit)) != 0` 45 (same code); `TEST_FIELD_BIT(flag8) || TEST_FIELD_BIT(flag11) || !audible`
// (the file's own idiom for the class-flag bits) 45 (same code); `(flags & 0x100) || TEST_FIELD_BIT(flag11) || !audible` 60
// (473 B: one dword load tested twice); that 45 form with the 0x800 store as `flag11 = true` 45. Compile-only screens
// (CL /c of the file; they reproduce both the EBX and the EBP homes) find the rule: whenever the end test starts with a
// byte test from memory, the request takes EBP; one dword load first keeps EBX. None of 76 declaration orders of the tag
// pointer, the source, the state and the priority (with or without the source, variant and priority locals), 72 helper
// combinations (pointer/reference/word helpers, the 0x800 set through word_flags_set, the variant copies through the
// local, `state.source = request->source`, the object index through `state.object_index` or a local, a const class-flag
// pointer), unused locals 0..7, extra temporaries before the test or the 0x21d110 call, extra end-test blocks, a
// `globals` local for g_4ed288, a named ticks local, a bool for the test, nested class-flag tests, flag-setting helpers or
// the marker size as a ternary moves the request back to EBX with retail's end test.
// Round 636: the merged tail test remains. Reduced control 41, caller-context control 32. Single-exit
// rejection with split masks or direct fields 41/41, Boolean fields 50, nested bit 8 or audible 65/50,
// audible first 58; bit 8 then audible then bit 11 56; direct return on rejection 82.
// @retail 0x189fe0
long function_189fe0(s_sound_request const *request, long tag_index)
{
	long result = NONE;

	if (tag_index != NONE)
	{
		s_sound_play_tag *sound = (s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes;
		s_sound_source_callbacks const *source = request->source;
		s_sound_play_state state;
		long priority = request->location.audible ? 3 : 1;
		state.flags = 5;
		source ? (state.flags |= 0x20) : (state.flags &= ~0x20);
		request->object_index != NONE ? (state.flags |= 8) : (state.flags &= ~8);
		request->platform_playback != NONE ? (state.flags |= 0x100) : (state.flags &= ~0x100);
		// Retail also clears the padding after the priority field.
		*(long *)&state.priority = priority;
		state.location = request->location;
		state.object_index = request->object_index;
		state.source = source;
		state.platform_playback = request->platform_playback;

		if (TEST_FIELD_BIT(((s_sound_class_play_flags *)function_221810(sound->class_index))->flag23) && !(sound->flags & 1))
		{
			state.location.flags |= 0x800;
		}

		if (request->marker)
		{
			state.marker = *request->marker;
			state.source_data_size = sizeof(s_sound_marker);
		}
		else
		{
			state.source_data_size = 0;
		}

		char const *variant = request->variant;
		if (variant && variant[1] != NONE && variant[0] != NONE)
		{
			state.flags |= 0x400;
			state.variant0 = request->variant[0];
			state.variant1 = request->variant[1];
		}

		result = function_21d110(&state, tag_index);
		if (result != NONE)
		{
			s_sound_class_play_flags *sound_class = (s_sound_class_play_flags *)function_221810(sound->class_index);
			if (TEST_FIELD_BIT(sound_class->flag1) && TEST_FIELD_BIT(sound_class->flag2))
			{
				long end_time = g_510c54->game_time + game_seconds_to_ticks_round((real)sound->duration * 0.001f) + 10;

				g_4ed288->value20 = end_time > g_4ed288->value20 ? end_time : g_4ed288->value20;
				if ((state.location.flags & 0x900) || !state.location.audible)
				{
					g_4ed288->value24 = end_time > g_4ed288->value24 ? end_time : g_4ed288->value24;
				}
			}
		}
	}
	return result;
}

// @retail 0x1895f0
long function_1895f0(s_sound_position const *position, real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.spatial = *position;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.location.audible = 1;
	request.location.requested_audible = 1;
	request.object_index = NONE;
	request.platform_playback = NONE;
	return function_189fe0(&request, local_0);
}

struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

// @retail 0x189650
long function_189650(s_sound_position const *position, s_sound_label_play const *play)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.spatial = *position;
	request.location.scale = play->scale;
	request.location.audible = 1;
	request.location.requested_audible = 1;
	request.location.unknown08 = 0;
	request.object_index = NONE;
	request.platform_playback = function_18d5b0(play->label);
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	return function_189fe0(&request, play->tag_index);
}

// @retail 0x1896c0
long function_1896c0(real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.object_index = NONE;
	request.platform_playback = NONE;
	return function_189fe0(&request, local_0);
}

extern s_sound_source_callbacks const g_444b7c;

// @retail 0x189710
long function_189710(real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.object_index = NONE;
	request.platform_playback = NONE;
	request.source = &g_444b7c;
	return function_189fe0(&request, local_0);
}

// @retail 0x189760
long function_189760(s_sound_label_play const *play)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.scale = play->scale;
	request.location.unknown08 = 0;
	request.object_index = NONE;
	request.platform_playback = function_18d5b0(play->label);
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	return function_189fe0(&request, play->tag_index);
}

// @retail 0x189340
long function_189340(long object_index, long tag_index, long audible, s_sound_marker const *marker, s_sound_source_callbacks const *source, real scale, long platform_playback, char const *variant)
{
	s_sound_request request;

	request.location.flags = 0;
	char local_audible = *(char const volatile *)&audible;
	function_18d4f0(object_index, &local_audible, NULL);
	request.location.audible = local_audible;
	request.location.requested_audible = local_audible;
	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.unknown08 = 0;
	request.location.spatial.location.leaf_index = NONE;
	request.location.spatial.location.cluster_index = NONE;
	request.location.spatial.location.bsp_index = g_4686c4;
	request.location.scale = scale;
	request.variant = variant;

	if (source->update(object_index, tag_index, marker, &request.location))
	{
		request.platform_playback = platform_playback;
		request.object_index = object_index;
		request.marker = marker;
		request.source = source;
		return function_189fe0(&request, tag_index);
	}
	return NONE;
}

void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description);
extern point3f *g_468788;
extern s_sound_source_callbacks const g_444b5c;

struct s_sound_globals_side_view
{
	long front[2];
	long side[2];
	long back[2];
};

struct s_tag_header_alt_sound_view
{
	byte unknown00[0x14];
	long sound_globals_tag_index;
};

struct s_tag_header_globals_sound_view
{
	byte unknown00[0xc0];
	void *header;
	s_tag_header_alt_sound_view *header_alt;
};

struct s_local_camera_view
{
	byte unknown00[0x18];
	vector3f forward;
	byte unknown24[0x30 - 0x24];
	point3f position;
};

// @retail 0x189400
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale)
{
	long local_player_index = NONE;
	bool play_at_position = true;

	if (object_index != NONE)
	{
		char audible = 1;
		if (!(((s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes)->unknown00 & 4))
		{
			function_18d4f0(object_index, &audible, &local_player_index);
		}

		play_at_position = audible == 1;
		if (!play_at_position && local_player_index != NONE)
			play_at_position = !local_camera_get(local_player_index)->active;
	}

	if (play_at_position)
		return function_1895f0(position, scale, tag_index);

	s_sound_source_description description;

	description.flags = 0;
	function_1892a0(NONE, g_468788, (vector3f *)g_4687a8, tag_index, object_index, &description);

	s_tag_header_globals_sound_view *globals = (s_tag_header_globals_sound_view *)g_4e034c;
	s_tag_header_alt_sound_view *header = globals->header ? globals->header_alt : NULL;
	long sound_globals_tag_index = header->sound_globals_tag_index;
	if (sound_globals_tag_index != NONE)
	{
		s_sound_globals_side_view *sides = (s_sound_globals_side_view *)g_4e3b44[sound_globals_tag_index & 0xffff].bytes;
		s_local_camera_view *camera = (s_local_camera_view *)local_camera_get(local_player_index);
		plane3f plane;

		plane.n = camera->forward;
		plane.d = dot3f((vector3f const *)&camera->position, &plane.n);
		real distance = plane_distance_to_point(&plane, &position->position);

		if ((real)fabs(distance) <= 0.01f)
		{
			description.value1c = sides->side[0];
			description.value20 = sides->side[1];
		}
		else if (distance > 0.0f)
		{
			description.value1c = sides->front[0];
			description.value20 = sides->front[1];
		}
		else
		{
			description.value1c = sides->back[0];
			description.value20 = sides->back[1];
		}
		description.flags |= 2;
	}
	return function_189340(object_index, tag_index, 0, (s_sound_marker *)&description, &g_444b5c, scale, NONE, NULL);
}

extern s_sound_source_callbacks const g_444afc;
extern s_sound_source_callbacks const g_444b1c;

// @retail 0x189060
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index)
{
	short const *local_0 = &value;
	s_sound_source_description description;

	description.flags = 0;
	function_1892a0(*local_0, position, direction, tag_index, object_index, &description);
	return function_189340(object_index, tag_index, 1, (s_sound_marker *)&description, function_189010(tag_index, object_index) ? &g_444b1c : &g_444afc, scale, NONE, NULL);
}

// @retail 0x1890c0
long function_1890c0(s_sound_label_play const *play, long object_index, short value, point3f const *position, vector3f const *direction)
{
	long result = NONE;
	s_sound_source_description description;
	s_sound_request request;
	char audible;

	description.flags = 0;
	audible = 1;
	function_18d4f0(object_index, &audible, NULL);
	function_1892a0(value, position, direction, play->tag_index, object_index, &description);
	request.location.audible = audible;
	request.location.requested_audible = audible;
	request.location.spatial.location.leaf_index = NONE;
	request.location.spatial.location.cluster_index = NONE;
	request.location.spatial.location.bsp_index = g_4686c4;
	request.variant = play->variant;
	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.unknown08 = 0;
	request.location.scale = play->scale;

	if (function_18c3b0(object_index, play->tag_index, (s_sound_marker *)&description, &request.location))
	{
		request.object_index = object_index;
		request.marker = (s_sound_marker *)&description;
		request.platform_playback = function_18d5b0(play->label);
		request.source = function_189010(play->tag_index, object_index) ? &g_444b1c : &g_444afc;
		result = function_189fe0(&request, play->tag_index);
	}
	return result;
}

// @retail 0x1891d0
long function_1891d0(long object_index, long marker_name, s_sound_label_play const *play)
{
	s_object_marker marker;

	function_b8d30(object_index, marker_name, &marker, 1, false);
	return function_1890c0(play, object_index, marker.node_index, &marker.node_matrix.position, &marker.node_matrix.forward);
}

// score 51 (was 51): Root-location registers, saves and stacked play remain; matrix
// pointer/reference/mutable bindings 51 each; complete submission copy 87; matrix with submission
// copy 87; library submission copy 87; matrix with velocity copy 51; reference with submission copy
// 87.
// BLOCKED-BY 0xba300 (src/unknown_0b8bd0.cpp): retail passes the object in ECX and the location
// in EAX to the root-location helper, ours passes the object in EAX and the location in ESI;
// with the helper's own body matching, this function loses only the 0x189400 call.
// frame agrees; saved registers, root-location contract and stacked play differ.
// Plain workspace order 51; workspace names 51; zero initialization 99; aggregate construction 64;
// native point copy 51; compression first 52; velocity first 49; separate location output 73.
// Earlier: root-location convention still adds saves and stacks the play pointer.
// Shared point setter 63; native velocity copy 51; both 63; marker scope 51;
// late playback record 69; pre-query playback record 64; root before velocity/compression 52/52.
// Earlier: root-location registers and stacked playback input remain.
// root-location setup, saved registers and stacked play remain.
// Marker value/clear 99/89; position clear 101; transform value/library copy 78/78;
// forward value 86; marker array 51; initialized marker array 99.
// root-location convention still adds saves and stacks the play pointer.
// root-location registers and stacked playback input remain.
// Entry/post-marker playback records 82/63; entry tag/scale/both 64/64/54;
// root before marker/point 60/52; entry record with root first 82.
// Post-marker tag/scale/both 52/63/53; post-compression 52/63/52;
// post-velocity tag/scale 52/63; all one-use snapshots restored.
// Play first/middle parameter orders 51/51 (same code); play fields read at entry 54 (play
// becomes a register argument, in eax). Retail keeps the object in esi and play in edi; ours
// needs esi for 0xba300's location argument (BLOCKED-BY 0xba300's convention).
// Round 6: unchanged; the first difference (+0x7, play stacked and EBP saved) still comes from
// 0xba300's object EAX / location ESI arguments (retail: object ECX, location EAX).
// Round 282: shared velocity setter with global/pointer/value inputs 63/63/95;
// point plus velocity setters 75; partial point/point+direction construction with
// velocity setter 96/90; velocity before compression 63, with both setters 75.
// All restored; root-location registers, saves and stacked playback input remain.
// Round 7 (lane 14): velocity first 52; named result 51; play fields through `(*play).` 51.
// Round 30: named result with one return 51; velocity/root/point/compression 52;
// point/velocity/root/compression 49; compression/velocity/point/root 52;
// point/root/velocity/compression 52. Restored; still BLOCKED-BY 0xba300's convention.
// The play record comes before the marker name: callers set up register arguments last to first, and retail's
// 0xc5890 loads the marker (eax) before the play record (edi).
// @retail 0x189210
long function_189210(long object_index, s_sound_label_play const *play, long marker_name)
{
	s_object_marker marker;
	s_sound_position position;

	function_b8d30(object_index, marker_name, &marker, 1, false);
	position.position = marker.matrix.position;
	position.compressed_forward = vector3d_compress(&marker.matrix.forward);
	position.velocity = *g_4687a4;
	object_get_root_location(object_index, &position.location);
	return function_189400(&position, object_index, play->tag_index, play->scale);
}

long function_155760(long local_player_index);
real function_218d30(long definition_index);
real function_30bf0(vector3f *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);

#pragma inline_depth(0)
/* the distance is a long double: as a plain double local the full image (not a reduced one) puts the
   success exit's `mov al, 1` ahead of both pops, where retail has it after the first; other functions'
   double locals elsewhere in the program decide that, as they did for 0xaaca0's frame */
// @retail 0x1897c0
bool function_1897c0(long local_player_index, long unit_index, long tag_index, s_location const *location, point3f const *origin, vector3f const *direction)
{
	bool local_1 = false;
	{
		s_local_camera *camera = &((s_local_cameras *)g_4e6380)->cameras[local_player_index];

		if (!camera->active)
		{
			goto local_2;
		}

		if ((1 << function_155760(local_player_index)) & 3)
		{
			long player_unit_index = NONE;
			if (local_player_index != NONE)
				player_unit_index = g_4e8c20->entries[local_player_index];
			if (unit_index == player_unit_index)
			{
				goto local_2;
			}
		}

		real maximum_distance = function_218d30(tag_index);
		vector3f to_camera;
		vector3f perpendicular;
		vector3f projection;
		real length_squared;

		to_camera.i = camera->position.x - origin->x;
		to_camera.j = camera->position.y - origin->y;
		to_camera.k = camera->position.z - origin->z;
		length_squared = (direction->j * direction->j + direction->k * direction->k + direction->i * direction->i);
		if (length_squared != 0.0f)
		{
			real local_0 = direction->k * to_camera.k;
			local_0 += direction->i * to_camera.i;
			local_0 += direction->j * to_camera.j;
			real t = local_0 / length_squared;

			projection.i = direction->i * t;
			projection.j = direction->j * t;
			projection.k = direction->k * t;
			perpendicular.i = to_camera.i - projection.i;
			perpendicular.j = to_camera.j - projection.j;
			perpendicular.k = to_camera.k - projection.k;
		}
		else
		{
			perpendicular = to_camera;
			projection.i = 0.0f;
			projection.j = 0.0f;
			projection.k = 0.0f;
		}

		real along = (direction->k * projection.k + direction->j * projection.j + direction->i * projection.i);
		if (along >= 0.0f && (direction->j * direction->j + direction->k * direction->k + direction->i * direction->i) > along)
		{
			real distance_squared = (perpendicular.k * perpendicular.k + perpendicular.j * perpendicular.j + perpendicular.i * perpendicular.i);

			if (maximum_distance * maximum_distance > distance_squared)
			{
				s_sound_position position;
				vector3f forward = *direction;
				long double distance = -sqrt((double)distance_squared);

				position.position.x = (real)(perpendicular.i * distance + camera->position.x);
				position.position.y = (real)(perpendicular.j * distance + camera->position.y);
				position.position.z = (real)(perpendicular.k * distance + camera->position.z);
				function_30bf0(&forward);
				position.compressed_forward = vector3d_compress(&forward);
				position.velocity = *g_4687a4;
				position.location = *location;
				function_1895f0(&position, 1.0f, tag_index);
				local_1 = true;
			}
		}
	}
local_2:
	return local_1;
}
#pragma inline_depth(255)
