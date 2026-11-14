// @flags /O2 /arch:SSE /Gr
#include "view_context_storage.h"
struct short_rectangle2d;
void function_1fa50(short_rectangle2d const *bounds, word const *text, void const *clip, void const *position, long line_gap, real scale, long color, void const *shadow);
/* UNKNOWN_1E9700.CPP: a value of the difficulty table for the current
   campaign difficulty (lane M; called by the behaviors of 0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "data_array.h"
#include <string.h>
#include <float.h>
#include <xmmintrin.h>
#include "slot_handler.h"
#include "input_record.h"
#include "unknown_157450.h"
#include "player_queries.h"

real function_1e96a0(short column, short row);

// @retail 0x1e9700
real function_1e9700(short row)
{
	long difficulty = 1;

	if (g_4e6948->state == 1)
		difficulty = g_4e6948->difficulty;
	return function_1e96a0(difficulty, row);
}

struct s_object_values
{
	long field_0;
	short values[16][14];
	short team_values[8][9];
};

void function_b58c0(long index, dword mask);
struct s_statborg;
void function_1968b0(long index, long team, long counter, long value);
void function_1e9df0(long team, s_statborg *statistics, long field, long counter, short delta);
long function_196ef0(byte code);
void function_196e60(long player_index, long counter, long code, long delta);
void function_225880(long player_index);
void function_162bf0(long player_index, long arg_1f407d_2);
void function_1e9ce0(long player_index, long counter, s_statborg *statistics, long field, long delta, bool by_team);

// @retail 0x1ea770
void function_1ea770(byte code, long player_index, s_statborg *statistics, long arg_1f407d_2, bool self, bool friendly)
{
	long kind = function_196ef0(code);
	s_statborg *const volatile *statistics_reference = &statistics;
	long volatile other = arg_1f407d_2 & 0xffff;
	long index = player_index & 0xffff;
	s_statborg *first_statistics = *statistics_reference;
	function_1e9ce0(player_index, 9, first_statistics, 3, 1, true);
	function_196e60(index, 1, kind, 1);
	if (self)
	{
		function_1e9ce0(player_index, 11, statistics, 4, 1, true);
		function_196e60(index, 3, kind, 1);
	}
	else if (friendly && arg_1f407d_2 != NONE)
	{
		if (*(short *)(g_4e8c24->data + other * 0x21c + 0x28) != NONE) function_225880(arg_1f407d_2);
		function_1e9ce0(arg_1f407d_2, 10, statistics, 5, 1, true);
		function_196e60(other, 2, kind, 1);
		function_162bf0(player_index, arg_1f407d_2);
	}
	*(short *)((byte *)statistics + index * 0x1c + 0x16) = 0;
	*(short *)((byte *)statistics + index * 0x1c + 0x18) = 0;
	*(long *)((byte *)statistics + index * 0x1c + 0x1c) = NONE;
	*(dword *)g_4e9ae8 |= 0x20;
}

// MiniMax H (11-14): no new form, no lower score (106): this round re-read the diff: the -1 default still lands in EAX where retail holds it in ECX, and the 15-bit counter is still read with movzx where retail clears EAX and loads a word; the two counts' registers come from the callers in another file, so the body's spelling cannot move them.
// score 106 (was 106): no frame; enable observations and maximum/register homes differ.
// MiniMax B (11-12): no new form, no lower score (106): retail keeps the old maximum in ECX with the enable byte in AL and reads the 15-bit counter with the 16-bit idiom, ours keeps them in EAX and DL, and the enable reload needs one object for g_510ca0 and the counters: BLOCKED-BY include/globals.h and src/unknown_1967d0.cpp.
// MiniMax H (11-10): no new form, no lower score (106): retail keeps the old maximum in ECX with the
// enable byte in AL and reads the 15-bit counter with the 16-bit idiom, ours keeps them in EAX and DL;
// the permuter (--fast, 10 and 25 minutes) never reached a search, and the enable reload needs one
// object for g_510ca0 and the counters: BLOCKED-BY include/globals.h and src/unknown_1967d0.cpp.
// MiniMax D (11-10): no new form, no lower score (106); retail keeps the old maximum in ECX with the enable
// byte in AL and reads the 15-bit counter with the 16-bit idiom (`xor eax, eax` then a word load), ours keeps
// the maximum in EAX, the enable byte in DL and zero-extends the counter; the enable reloads and the two
// inlined helpers of src/unknown_1967d0.cpp stay out of reach.
// MiniMax H (11-04): no new form, no lower score; the enable reloads and the maximum homes still differ.
// Earlier 106 (was 107): no frame; enable observations and maximum/register homes differ.
// Lane 12 r78: MATCH (fast, plain and full check, lost []) with two changes together, not kept by the coordinator:
// (1) the enable byte and the input counters declared as one struct symbol in globals.h (retail's record at 0x510cb0
// reaches 0x51e8d4, so the region is one object); the padding of that trial struct overlaps the separately defined
// 0x510cb0..0x511000 globals, so a real form must fold those in as fields; held like the 0x2136c0 struct globals.
// (2) 0x196960's getter as a file-local forceinline used twice, the setter in the body, the increment before the
// compare with nested clamp tests, `bool teams` set from the flag bit, player->team read at each use.
// Without the struct symbol the best was 96 (layered getter/setter, first enable read through a const pointer);
// the struct gave 53 and the teams form 0. All in the body, no helpers: 80 and 51.
// Earlier 107 (was 107): this round: the previous maximum read through a 16-bit local (112 at 419
// MiniMax R (11-01): no new form, no lower score; retail's maximum lives in ECX with the counter
// in AX and the enable byte in AL, ours keeps the maximum in EAX, the enable byte in DL and
// materialises the -1 default twice; still BLOCKED-BY 0x196960/0x1968b0 in another lane's file.
// Earlier 107 (was 107): this round: the previous maximum read through a 16-bit local (112 at 419
// bytes, the value loaded with the sign-extending idiom), through an unsigned 16-bit local (112),
// and through a short cast, a word read of the counter and a word cast (107 each). Still the
// maximum's register home: retail keeps the old maximum in ECX and the counter in AX and reads
// the 15-bit value with the 16-bit idiom (xor eax,eax; mov ax,[...]) with one -1 default and the
// enable byte in AL; ours zero-extends the value into EAX, keeps the enable byte in DL and
// materialises the -1 default twice. Still BLOCKED-BY 0x196960/0x1968b0, whose file is held.
// Earlier 107 (was 107): this round: the comparison written the other way round (the counter first)
// gives 107, and the counter read through a 16-bit local, at block scope or at function scope, 112
// with and without that comparison (419 bytes). Still the maximum's register home: retail keeps the
// old maximum in ECX and the counter in AX and reads the 15-bit value with the 16-bit idiom
// (xor eax,eax; mov ax,[...]) into ECX with one -1 default and the enable byte in AL, ours
// zero-extends the value into EAX, keeps the enable byte in DL and materialises the -1 default
// twice. Still BLOCKED-BY 0x196960/0x1968b0 (retail inlines both; their file is held).
// Earlier 107 (was 107): this round: the player maximum read as a nested initialise/compare/assign
// gave 109, the same nested read for the team maximum 117 and the player maximum alone 112.
// Still the enable reloads and the maximum-register homes: retail keeps the old maximum in ECX
// and the -1 comparison in EAX, ours merges the enable into DL and the maximum into EAX.
// Earlier 107 (was 107): no frame; enable reloads and maximum-register homes differ.
// Four meaningful inputs agree; the converter consumes ECX and preserves player EDX.
// Earlier: the two maximum
// reads as `if (g_510ca0) x = i != NONE ? value : NONE` gave retail's nested
// shape (115 -> 107). Tried: the old maximum through a call to 0x196960 (167,
// not inlined), the team maximum as one nested ternary (110).
// Earlier: still BLOCKED-BY 0x196960/0x1968b0 (retail inlines both; their file is held) and retail re-reads g_510ca0 after each pointer store where ours keeps it in DL (whole-program aliasing). Combined `g_510ca0 && index != NONE` maximum 112; same for the team maximum 112; increment through the statistics address 111; counter pointer with the nested-ternary clamp 106 (its store then matches retail's); that clamping `consecutive` directly 106.
// Native descriptor value with update/ternary/exclusive/nested clamps 151/160/160/151;
// const descriptor counterparts 151/160/160/151. Original scalar limits restored;
// enable reloads, maximum/register roles and the held inline boundaries remain.
// Round 414: No frame; enable reloads and maximum-register allocation remain. Shared previous maximum
// in long/short storage at phase/entry, with read-once or per-use consecutive field, 107 in all eight
// forms. Independent original maxima and counter snapshot restored.
// Round 85: caller-local inline declarations with the existing player/team getters 164/118
// (401/413 bytes), and with the player setter 202 (362); index admission nested after the limit
// reads 110 (420); early consecutive and row declarations 107/107; team-mode byte and nested
// team guards 107/107. Original scalar limits, counter operations and guards restored.
// Round 279: admission index/writable/enabled/improved 103; writable/index/enabled/improved 104;
// enabled/index/writable/improved 106; enabled/writable/index/improved 106;
// writable/enabled/improved/index 104; enabled/improved/writable/index 107;
// improved/index/enabled/writable 107; improved/writable/enabled/index 107.
// Best admission with short maximum / separate increment / conditional clamp / per-use counter
// 103/103/105/103. Four-point gain excluded; original body restored.
// Round 202 (lane 3): the team test through the existing function_x340af0() 106 (425 bytes; it gives retail's
// store of the flag into the code parameter's slot); on it the team read at each use 108, the counter re-read
// after the increment 106, a short old maximum 106, the row pointer before the old maximum 106. Restored.
// J247: Actual shared counter-word reader/store view subsets 107/107/107/107/107/107/107;
// complete view with the team-mode helper 106 (425 bytes). NOT kept: one-point gain below threshold.
// Original shared type and body restored; enable reload, maximum homes and word schedule remain.
// Round 138: setter counter pointer before/after the clamp 106/111; independent per-use counter 107;
// value between the enable gates 110; separate player/team inner results 109/109; two-call by-value
// accessor 107; player record copy 71 (434 bytes), read-only copy 71, with the team-mode helper 78.
// NOT kept: the one-use record copy spills through the code slot, absent in retail; original body restored.
// MiniMax C (11-10): no new form, no lower score (106); retail still keeps the old maximum in ECX with the enable byte in AL and reads the 15-bit counter with the 16-bit idiom (xor eax, eax; mov ax, [mem]) while ours keeps them in EAX and DL with a zero-extend, and the enable reload stays blocked by src/unknown_1967d0.cpp.
// MiniMax C (11-05): no new form, no lower score: the enable reload, the maximum homes and the word
// schedule still differ here.
// Round 74 (m3 lane 11, fast and plain): the team test through the existing function_x340af0() in the condition 106
// (kept: it replaces a hand-copied copy of that helper and the reuse of `code`; the helper's result takes code's
// slot, retail's store), and the player record as the header's s_player_datum (same code; replaces a local struct).
// Retail's two maxima are 0x196960's inlined body (its `mov eax, -1` preset, the `xor eax, eax; mov ax` read and
// the team's `or ecx, ecx`), and the player clamp is 0x1968b0's: both sit in /Ob1 src/unknown_1967d0.cpp (held),
// where `inline` on their definitions is the lead. On 107: the increment before the row pointer is formed 111
// (retail's `inc word ptr [eax+edx+0x16]; lea edi` shape, but the player/kind homes swap); that with the index taken
// before the classification call 115; row declared first and assigned after the increment 111. On 106: the
// increment form 110; the team field read at each use 108; the team read once before the test 113.
// Round 79 (m3 lane 11, fast): the enable reloads are an escape: retail re-reads g_510ca0 after the store through
// `statistics` because g_510ca0 shares one object with 0x510cb0, whose address retail pushes (0x1962f6 and four more
// sites); retail never takes 0x510ca0's own address. A one-file LTCG link adds exactly that reload once &g_510ca0 is
// stored anywhere. Diagnostic only (a file-scope pointer to g_510ca0, reverted): 74 at 434 bytes. The real form is one
// object from 0x510ca0 with g_510ca1/ca2/ca4/ca8/cb0/cb1/cb2 as members, but held src/unknown_1967d0.cpp declares
// g_510ca4/g_510ca8 itself, defines g_510cb2 and takes &g_510cb0: BLOCKED-BY 0x196960/0x1968b0 (their file).
// Forms on main's body: old maximum as one nested ternary initialiser 109; the counter as long 109; the player clamp as a ternary
// in the store 100 (427 bytes, a cmp/mov/jg clamp unlike retail's); team maximum as one nested ternary 109; the team
// test and setter call on `consecutive` in place of the counter re-read 110.
// Round 187 (m3 lane 4, fast with 0x1968b0, 0x196960, 0x196e60, 0x196ef0, 0x1e9ce0): the threshold test with its arms
// swapped 105 (positional only: retail keeps the `>=` arm first), its operands swapped 106; the row offset written at
// each use as 0x1ea770 does 110 (427 bytes); the setter guard nested under the maximum compare 106 (same code); `kind`
// as a short 111 (428 bytes); the row as the header's player record (`&statistics->players[index]`, fields at +0x12,
// +0x14, +0x18) 109 (428 bytes).
// MiniMax G (11-13): no new form, no lower score (106): the reload of the enable flag after the store
// through `statistics` needs the aliasing that src/unknown_1967d0.cpp gets by taking &g_510cb0, so it
// stays blocked; retail's old maximum in ECX and its 16-bit counter idiom are still unmatched here.
// Round 157 (m3 lane 7, no counted trial): still BLOCKED-BY 0x196960/0x1968b0 (src/unknown_1967d0.cpp, held this round) and
// the held globals.h aggregate for the enable reload; the MiniMax B/H/D/G lines and round 187 above list the in-body forms.
// Reading of the team block: ours reloads the enable byte into DL ahead of the engine test and holds it through the team
// byte test, where retail reloads it after that test, so retail's EDX is free for the engine index (EDX/EAX) and ours
// takes EAX/ECX; the swap follows from the blocked reload, not a separate cause.
// @retail 0x1ea5b0
void function_1ea5b0(long player_index, s_statborg *statistics, long threshold, byte code)
{
	long kind = function_196ef0(code);
	long index = player_index & 0xffff;
	s_player_datum *player = ((s_player_datum *)g_4e8c24->data) + index;
	function_1e9ce0(player_index, 7, statistics, 2, 1, true);
	function_196e60(index, 0, kind, 1);
	if ((code & 0xc0) == 0x40) function_196e60(index, 6, kind, 1);
	long old_maximum = NONE;
	if (g_510ca0) old_maximum = index != NONE ? g_511bf4.groups[index][12].value : NONE;
	byte *row = (byte *)statistics + index * 0x1c;
	short consecutive = ++*(short *)(row + 0x16);
	if ((short)old_maximum < consecutive && g_510ca0 && !g_510ca1 && index != NONE)
	{
		long minimum = g_46ddc0[12].minimum;
		long maximum = g_46ddc0[12].maximum;
		long value = consecutive;
		if (value < minimum) value = minimum;
		else if (value > maximum) value = maximum;
		g_511bf4.groups[index][12].value = (word)value;
	}
	if (function_x340af0() && player->team != NONE)
	{
		long team = player->team;
		long team_maximum = NONE;
		if (g_510ca0) team_maximum = team != NONE ? g_511bf4.counters[team][12].value : NONE;
		if ((short)team_maximum < *(short *)(row + 0x16)) function_1968b0(NONE, team, 12, *(short *)(row + 0x16));
	}
	if (*(long *)(row + 0x1c) >= threshold) ++*(short *)(row + 0x18);
	else *(short *)(row + 0x18) = 1;
	*(long *)(row + 0x1c) = g_510c54->game_time;
	*(dword *)g_4e9ae8 |= 0x20;
}

struct s_object;
s_object *function_badc0(long object_index, dword types);
bool function_1df560(short team, short other_team);
void function_197160(long player_index, long arg_1f407d_2, long code);
long function_baf80(long object_index);
void function_15ce70(long attacker_index, long victim_index, bool betrayal, byte damage);
void function_196dd0(long player_index, long counter, long arg_1f407d_2, long delta);
void function_1ea880(long first_player, long player_index, s_object_values *table);

long unit_get_player_index(long unit_index);
long function_146650(void);
bool function_15eaf0(void);

/* one of the four most recent damage sources an object keeps (the layout of unit_object_type.cpp's
   s_unit_damage_source; the time is compared unsigned here) */
struct s_damage_record
{
	dword time;
	real amount;
	long object_index;
	long player_index;
};

/* the damage history of an object (at +0x2f0) and the damage codes of its records */
struct s_object_damage_view
{
	byte unknown000[0x2f0];
	s_damage_record history[4];
	byte damage_codes[4];
};

// MiniMax H (11-14): no new form, no lower score (353): re-read the diff and the earlier rounds' thirteen trials; the frame stays 0x48 against retail's 0x50 and the three homes stay rotated, and every earlier trial (rounding in place, result lifetimes, short selections, direct duplicate skips, the history cursor) measured above 353.
// score 353 (was 353): victim_team/killer/damage_code slots rotated, history cursor biased +4 (the earlier note below).
// MiniMax H (11-12): no new form, no lower score (353): the history entries read through a named record
// pointer (player_index, time and amount off one pointer) compile to the same bytes as the indexed form, and
// the permuter refuses this function because the reduced image and the full image disagree (446 against 350).
// The score line above read 344 from a reduced build; the full build gives 353, which is the number here.
// MiniMax H (11-11): the consideration flag is now a true default cleared by the test, which is retail's shape.
// Round 170 (m3 lane 5, fast with the file's functions and 0xd9640; base aligned 132, masked 35): slot references counted in
// both listings: retail killer 8, victim_team 8, damage_code 7 (killer stays in its home and is reloaded into EDX/EDI around the
// fallback, the code is read once into ESI for both pushes); ours victim_team 8, damage_code 8, killer 6 (killer in EBP through
// the killer block). Tried: the related-assist limit as `assist_count < 5` (no cast) 353 (aligned 133, masked 36); `self` as
// `victim == reported_player` 353 (133/36); the selection fallback as `selected = selected != NONE ? selected : reported_slot`
// 353 (same code); `byte damage_code = code;` declared just before the selected block 470 (1578 B, first difference +0x8);
// `long killer = NONE;` declared there 409 (1564 B; aligned 137, masked 37).
// MiniMax C (11-10): the score is 353 (was 353); the three homes sit in rotated slots (ours +0x14/+0x1c/+0x18 against retail's +0x18/+0x14/+0x1c) and our function is four bytes longer, so the first branch's jump lands elsewhere.
// Round 147 (m3 lane 8, fast; [aligned ours/retail, masked ours/retail], control 353 [128/132, 31/35]): the related unit's
// player index read at each use (retail reads +0x13c again for the store) 357 (1552 B) [119/124, 29/34]; with the killer's
// team byte passed to 0x1df560 at the call 370 (1564 B) [131/132, 32/33]; the dedupe searches as `for (j...) if (==) break;`
// 357 (same code); diagnostic, the dedupe through a two-site inline search returning from its loop (retail's threaded exits,
// no `j == i` test) 361 (1532 B) [115/126, 25/36], with `player_index == players[i]` (retail's `cmp ecx, [mem]`) 361
// [113/124, 23/34]; that plus the assist record's player index read at each use 371 (frame moves) [154/161, 35/42].
// MiniMax H (11-04): the selected damage record read through a local record pointer 454; not
// kept, the original body is restored.
// MiniMax S (11-06): the record walk is anchored at the field with the most source-level reads, ties going to the lowest offset: reading the timestamp twice moves the selection walk onto retail's 0x2f0 base (373) and reading the assist player index at each use moves that walk onto retail's 0x2fc base (390), both loops together 390; every such form drops a frame word (0x4c against 0x50) and the base spill, so the original body stays.
// Earlier 353 (was 353): frame 0x50 and the inline rounding now as retail; victim_team/killer/damage_code sit in
// rotated slots (ours +0x14/+0x1c/+0x18, retail +0x18/+0x14/+0x1c), the selection cursor is biased +4, and the killer
// block reads the record without retail's `&history[selected]` pointer (killer in EBP, retail EDX).
// Byte selection index 421; signed-byte teams 487; explicit append phases 421; selected/fallback result
// 421; early self result 453 (frame 0x50).
// Earlier (421): frame 0x48 versus 0x50; victim/result homes and rounding calls differ.
// Assist cursor 464; damage snapshot 466; early hostility 463; hostile arm first 421; size optimization 713.
// j91: frame 0x48 versus 0x50 and victim/selection/count homes remain; isolated early killer recovers 0x50 at 454 but regresses.
// early killer result 454; early assist threshold 440; early attribution results 463; shared tick frequency
// 421; attribution shared frequency 460
// Frame pointer 530; long selection index 451; short assist counts 421 (1549 bytes);
// long consideration result 426; long hostility result 430.
// Frame 0x48/0x50, result homes and rounding calls remain; typed history 421; history base before team query 466; named rounded windows 421; byte consideration 421; short award counters 433.
// Complete history-object layout 421; fallback slot alone short 422; later killer-team result 426;
// retained player-pool pointer 462; final friendly result from nested tests 427.
// Early assist arrays alone 421; unsigned short selection ordinal 421; first assist-prefix cursors 451; both assist-prefix cursors 430; expired-history continue 421.
// Object before/between rounding 421/421; object and history before/between 466/465;
// later victim-team observation 450; early object with later team 449;
// early object / object and history with forced rounding expansion 439/472. All restored.
// Round 160: native award counts/prefix ordinals 421/422; shared award/prefix/both
// ordinals 421/421/421; native counts with shared ordinals/slot homes/early
// credit default 421/421/452. Original types, lifetimes and loops restored.
// Round 149: Shared current tick with rate, earlier code and clock receiver combinations [427, 450, 445,
// 432, 427, 450, 445, 432]; all restored.
// Round 381: Victim/selection/victim-selection/threshold/victim-threshold/selection-threshold/all
// workspace scopes 421/464/464/421/421/464/464; all scopes with unsigned selection ordinal 464.
// Original lifetimes restored; frame, result homes and rounding calls remain.
// Round 242: Native statistics-valid/player-record/object-header fields, all
// seven subsets and complete graph with native history, 421 each. Original
// body, types, rounding calls and memory observations restored.
// Round 262: native team/credited-player/victim/secondary-award call summaries,
// inline and noinline 421/421/421/421/421/421/421/421. Every declaration and complete
// body restored; frame 0x48 versus 0x50 and result/history/rounding homes remain.
// Round 399: First-lookup entry defaults for damage code, selection/fallback slots, killer team and all
// combinations, then credited player: 461/442/433/467/460/442/433/443. Original restored.
// Round 209: History player fields at every use in selection/attribution/assist 466/447/426; selection-
// attribution/selection-assist/attribution-assist 456/440/458; all phases 420, with related-unit player
// at each use 416. Five-point best excluded; original observations restored; frame/result/rounding
// homes remain.
// Round 308: Complete victim-index/killer-index/award-player subsets 452/442/435/442/441/458/453;
// all with native word indices 461. Earlier victim-index lifetime recovers frame 0x50 but regresses;
// original handle arithmetic, award observations, frame and rounding calls restored.
// Round 203: native selected/fallback/both record results 484/505/459;
// with a twice-used amount 460/483/459; native record cursor
// without/with amount 473/473. Original result types and body restored;
// frame, victim/selection/count homes and rounding expansion remain.
// Round 120 (lane 6, fast = plain): forceinline real_to_long declaration 444 (retail's inline fld/fistp);
// with the victim through unit_get_player_index 472; with both assist arrays [5] 469 (frame 0x50); with
// the game time through function_146650 472 (game_time read before the asm as retail); with retail's
// statement order (NONE defaults first, consider_team cleared after the test, short selected and
// reported_slot, hostile/threshold/counts after the selection, `if (killer == NONE) ... else if (killer
// == reported_player)`, friendly through function_15eaf0) 501; with the record fields read as
// `*(long *)(history + i * 0x10 + 0xc)` 501 (same code); with the history as a typed damage-record
// array 376; the assist stores and the related player re-reading the field 387 (assist loop alone 418:
// frame 0x4c); dedupe as `j >= i` 378; dedupe compare as `assists[i] != assists[j]` 376 (same count);
// as a for loop with break 376; killer read from the record at each use 344 (kept); `if (killer !=
// NONE) {...} else if (datum...)` 354, with the fallback's stores swapped 354; object as a typed view
// with history/damage_codes members 344 (cursor still +4); damage read once under a nested time test
// 440; the killer-team call argument read from the player byte again 362 (retail's movsx si, al), with
// the != NONE arm order 345; consider_team as one expression 353; friendly as one expression 344
// (aligned 176 against 169); a walked record pointer in the selection loop 346 (still rebased +4);
// named victim/killer index locals for the two 0x196dd0 calls 344 (killer first gives retail's
// EBP/ESI roles there, same count). Kept with consider_team and friendly as single expressions 353:
// consider_team set true and cleared inside the reported-team test gives retail's store pair and 344,
// but it is a flag set inside a test (left for a matching round); `reported_player == NONE ||
// !function_1df560(...)` 353.
// With all of it 353 (cursor biased +4, killer/victim_team/damage_code in rotated slots, killer kept in
// EBP, the assist player index in EBX), but only with a file-local __forceinline redeclaration of the
// header's plain-inline real_to_long (483 without it): an inline twin, kept only with a match; body restored.
// Round 62 (m3 lane 16, fast, on this body without the forceinline real_to_long): the victim through the existing
// unit_get_player_index 459 (1561 bytes); the game time through function_146650 441; friendly through function_15eaf0
// 438; all three 463 (1567 bytes); other_assists[5] 421 (frame 0x4c). Retail still inlines real_to_long's fld/fistp
// where this body calls it.
// Round 142 (lane 4, fast): other_assists sized 5 (retail's frame ends with two five-entry arrays) 421
// (same frame); short selected/reported_slot 453 (1556 bytes), with the [5] array 456; the victim through
// unit_get_player_index 459; friendly through function_x340af0()/function_15eaf0() 438/438; the game time
// through function_146650 441.
// Round 49 (lane 10, fast): include/slot_handler.h's real_to_long made `static __forceinline` (every other live
// caller already redeclares it so; the rest of its plain-inline users are #if 0 files) and lane 6 r120's chain
// rebuilt on it (unit_get_player_index victim, function_146650 game time, NONE defaults and damage_code first,
// short reported_slot/selected, hostile/threshold/counts after the selection, `if (killer == NONE) ... else if
// (killer == reported_player)`, killer read from the record at each use, other_assists[5], typed damage records,
// `assists[i] != assists[j]`, friendly through function_15eaf0; consider_team and friendly single expressions)
// 353 (kept). On it: a record pointer in the selection loop 353 (same code); the damage read once into a local
// 454 (1583 bytes); the player index read at each use in the selection loop 376; a `&history[selected]` record
// pointer in the killer block 454 (1583); victim_team/killer/damage_code declared in the other five orders
// 353-360 (the three slots never move).
// Round 114 (m3 lane 7, fast): a `&object->history[selected]` record pointer in the killer block 454 (1583 bytes:
// retail's `lea [sel+0x2f]; shl 4; add` record and victim_team/damage_code slots, but selected loses EDI to a
// parameter home and the frame grows to 0x54), with a record pointer in the selection loop too 454 (same code),
// with the record's player index named 491 (1617); the selection amount read once before the time test 454 (the
// loop base stays +0x2f4); a `history` array pointer local for every lookup 353 (same code: retail saves
// object+0x2f0 at [esp+0x34] for both loops, ours object+0x2f4).
// Lane 12 r72 (fast = plain, the same chain rebuilt on its own base before round 49 landed): the shared header's real_to_long made __forceinline (every other user
// already redeclares it so; retail has its fld/fistp inline at every call) 444 (1561 bytes) alone; with
// round 120's form (victim through unit_get_player_index, the game time through function_146650, the
// NONE/code defaults and the counts declared early, short selected/reported_slot, both assist arrays [5],
// the killer read from the record at each use, `if (killer == NONE) ... else if (killer ==
// reported_player)`, the four records typed as unit_object_type.cpp's damage-source struct) 353; with
// friendly kept as written (not through function_15eaf0) 349, kept. Round 120's form without the header
// change 483. On 349: other_assists[4] 368; the game time read directly 394; long selected/reported_slot
// 424; the selected block through a record pointer with a player-index local 498, the local alone 380,
// the pointer alone 435; killer declared before victim_team 350, damage_code first 378, killer first
// 366. Compile-only (reproduces the +4 cursor): the cursor follows the most-read field (the player read
// at each use: +0xc); the amount named once, the time test after the amount test, `best` read through
// `selected`: still +4.
// Lane 12 r72 result not kept: friendly written out (not through function_15eaf0) gave 349 (retail's 1544
// bytes) on its own form, 4 under the keep bar against round 49's 353.
// Round 290: Persistent victim-team/credited-player/code role names, all seven subsets, 353 each;
// all names with separate declarations and original assignments 353. Frame 0x50 agrees; the
// three scalar homes, biased history cursor and attribution/award allocation remain. Original restored.
// Round 445: Existing shared validated-record provider in selected/fallback/assist subsets,
// then consistent existing inline validation: 375/396/509/494/509/526/557/515.
// Original complete executable body restored.
// Round 305: Record-pointer selection, assist admission and both award-prefix scans, seven subsets:
// 461/379/465/370/480/398/485; all with native short teams 494. Original body restored; frame/scalar
// homes and the biased history cursor remain.
// Round 308: Long/dword/short/word event-code carriers 423/423/453/453;
// with both team results short 454/454/456/456. Every code remains in 0..255 and all
// callback arguments stay unchanged. Wider carriers disturb the entry and do not recover
// retail result homes; original byte code, long teams and complete body restored.
// N308: Complete native history-record values in selection/attribution/assist, seven subsets,
// 500/378/514/436/615/498/584; all by aggregate assignment 584. Original records and body
// restored; scalar homes, amount-biased history cursor and credited-player validation remain.
// P263: original byte code with native victim/killer/both signed-byte teams 488/381/485;
// corresponding short teams 366/357/366; both byte/short teams with long consideration 480/401.
// Original domains and body restored; scalar homes and biased history cursor remain.
// Round 370: Chained team/credit, selection and award-count defaults, subsets
// 1/2/4/3/5/6/7, then all with earlier parameter-derived code: 353/463/355/463/355/463/463/463.
// Selection chains grow frame to 0x54; original body restored. Scalar homes and biased history cursor remain.
// DL245: Frame 0x50 agrees; victim-team/credited-player/code homes and amount-biased history cursor remain. Dword consideration/hostility/friendly subsets 381/360/390/354/380/354/397; all word 412. Native unsigned cutoff/recent/both 353 each. Original Boolean, signed window and record graph restored.
// Round 372: positive engine/valid-selection/eligible-assist regions, subsets 1/2/4/3/5/6/7,
// then common conditional slot fallback: 353/353/353/353/353/353/353/353. Original complete body
// restored; scalar homes and amount-biased history cursor remain.
// Round 390: External amount-field identity after timestamp acceptance, original/whole
// record reference with short/word/long/dword ordinals: 353/354/355/355/353/354/355/355.
// Original body restored; scalar homes, amount-biased cursor and attribution remain.
// Round 202 (lane 3): the assist stores re-reading object->history[i].player_index (both arrays) 392 (1564
// bytes, frame 0x4c; the assist cursor becomes retail's +0xc record field); the friendly store alone 379; both
// with the selection amount read once into a local under the time test 451 (selection cursor becomes retail's
// object+0x2f0, but the assist loop keeps its own base: retail spills one object+0x2f0 for both loops); both
// with a walked record pointer in the selection loop 390, started from a history local used by both loops 392;
// killer/victim_team/damage_code renamed in all six name orders in the harness, one order in --fast, 353 (the slots never move). Restored.
// Round 129 (m3 lane 8, fast): victim_team as a ternary initializer 410; friendly as a false default set true
// under the full test 353 (retail's tail shape: self +0x68, friendly +0x6c, the stores line up); with consider_team
// as a true default cleared in the reported-team test as well 344 (match-only flag form). On that 344: assist
// stores re-reading the record's player 386 (1564 bytes, frame 0x4c: the shared object+0x2f0 base at +0x34 goes);
// with a history pointer for both loops 386; stores re-read plus the amount read once under the time test 444;
// a `&history[selected]` record pointer in the killer block 449 (1583); the killer-team call argument from the
// player byte 349. Original body restored.
// Round 390: External amount-field identity after timestamp acceptance, original/whole
// record reference with short/word/long/dword ordinals: 353/354/355/355/353/354/355/355.
// Original body restored; scalar homes, amount-biased cursor and attribution remain.
// J249: Existing recorded-event byte/streak long/final-event long/paired first two contracts
// 354/354/355/353. Final-event long loses matched 0x1ea770 (19). NOT kept: paired contracts
// lower the streak provider 107 -> 76 but replace its native byte flag-home store with a
// full-width flag test. Coherent long/word caller event carriers with the streak contract,
// without/with the recorded-event byte formal: 423/423/453/453; all lose matched 0xd9640 (61).
// Original body, code domains and every provider restored; scalar homes and biased cursor remain.
// DL253: existing streak/final/secondary provider ordinary-inline singles 353/353/353;
// ordinary-inline pairs 353/353/353 and all three 353 on the native-frame graph;
// forced streak expansion 962 (1949 bytes), loses a matched caller; NOT kept: shared regression.
// Original providers and calls restored; scalar homes, history cursor and attribution still differ.
// B399: Standard prefix searches on the two award arrays: find first/second/both
// 360/360/373; count first/second 360/361; count-find/find-count/both count
// 374/374/375. NOT kept: every form regresses and shortens away from retail. Original
// whole body and include list restored; scalar homes and amount-biased history cursor remain.
// B406: Attribution-produced event code byte/word/long/dword, definite assignment
// 436/451/452/452; conditional selected-code producers 436/440/472/472. All regress;
// original eager code, scalar homes, history observations and complete body restored.
// P303: Late constant killer-team/hostility defaults and direct statistics parameter, subsets
// 1/2/4/3/5/6/7: 374/338/353/378/374/338/378; reversed award-array declarations 353.
// NOT kept: hostility at 338 delays its clear past attribution, unlike retail; original lifetimes and body restored.
// M346: Existing consideration/hostility/friendly results as byte, masks 1/2/4/3/5/6/7:
// 353/369/351/369/351/362/362; all char 362. NOT kept: friendly alone or with
// consideration gains only two through extent, with scalar homes unchanged. Original body restored.
// M354: Complementary consideration/hostility/both bool and byte, then word/long pairs:
// 353/369/369/353/369/369/417/392. NOT kept: complemented status publication misses native byte/store allocation.
// M356: Complete timestamp/strict-improvement results, expression then ordered
// tests, bool/byte/short/long: 353/353/353/353/353/353/353/353. NOT kept: no gain;
// original selection and complete body restored. Scalar homes and biased cursor remain.
// DH336: Two real award buffers/counts in array and record ownership forms:
// 353/353/353/385/385/385/385/385; pointer-difference controls repeat the earlier form.
// NOT kept: grouped storage needs equality; original complete body restored.
// DH350: Both award-prefix searches own duplicate/unique outcomes,
// bool/byte/short/long: 366/366/366/366/366/366/366/366. NOT kept: test-set outcomes need equality;
// original ordinal searches, scalar homes and complete body restored.
// J257: Completed selected/reported validation outcomes, bool subsets and byte/short/long pairs, pointers and direct credit results:
// 353/353/353/353/353/353/353/425. NOT kept: seven validation forms tie 353; direct credit production grows to 1580 bytes at 425 and leaves scalar/history homes unresolved.
// Original executable source restored; allocation gaps remain.
// DH371: Actual engine/reported-source/selected-validation/fallback-validation/credit-hostility/
// assist-admission/credit-pair/related-owner Boolean test results: 353 each.
// NOT kept: no gain or equality; original validation, record observations and full body restored.
// Native scalar homes and amount-biased history cursor remain.
// Round 53 (m3 lane 2, fast; positional/aligned/masked aligned, baseline 353/160/56): consider_team as a true default
// cleared by an if 344/156/52 (retail's store, but a flag form: match-only, not kept); the killer test as `if (killer !=
// NONE) { if (killer == reported) code; } else if (reported is valid) { ... }` (retail's layout) 363 (1532)/157/52, with the
// default 354 (1532)/153/48; a walked record pointer from a history local in the selection loop 374/169/68, with the default
// 368/165/64; a `&history[selected]` pointer 454 (1583)/264/106, with the new test 438 (1567)/264/102; a named selected
// player 403 (1596)/174/105, with the new test 354/160/58; pointer and player together 491 (1617)/275/192, with the test
// 433 (1569)/272/142; those with the walked pointer and default 438 (1585)/259/89, with history reads in the assists 417
// (1569)/247/81. Slots follow memory references, heaviest lowest: ours victim_team 8, damage_code 7, killer 6 references
// (+0x14/+0x18/+0x1c); retail killer 8, victim_team 8, damage_code 6 (+0x14/+0x18/+0x1c). So retail's killer is read from
// its home two more times and its damage code once less; the record pointer forms lose `selected`'s EDI and grow the frame.
// N398: Native post-root shared object-header raw/typed/readonly tables, then related-player observations at every use 353/353/353/353/357/357/357/357.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// MiniMax R (11-05): the team-consideration result is a true default cleared by the query, 344 (was 353); not kept: a test-set flag, match only.
// Round 79 (m3 lane 11, fast): `s_damage_record *record = &object->history[selected]` in the killer block, declared before or
// after the damage code read, 454/454 (1583 bytes): victim_team and damage_code take retail's +0x18/+0x1c and the record
// is retail's `lea [sel+0x2f]; shl 4; add`, but `selected` moves to a parameter home, the pool pointer takes EDI (retail
// ESI) and the frame grows to 0x54. On it: `selected` declared first 464; `if (killer != NONE) { if (killer ==
// reported_player) ... } else if (reported valid)` 438 (1567); that with the consideration flag as a true default cleared
// by the test 433 (match-only flag form; diagnostic). No form reaches below 353.
// Round 163 (m3 lane 5, fast): the damage code read after the validation block 342 (1568 bytes; retail reads it
// before the validation's test, a statement order by score, not kept); `if (killer == NONE && reported valid)
// killer = reported; if (killer == reported) code` 332, discarded: not the same behaviour (retail leaves the code
// alone when the fallback fails); the selection fallback below the hostile/threshold/count defaults 386; the
// record's player assigned to killer first and cleared when the validation fails 415 (1612); the credited player's
// team byte as a twice-used char local for the team store and the query 358 (1552), as a short local 358. Retail
// keeps the credited player in its home (+0x14; reloaded into EDX at the fallback, into EDI at the team block).
// M3 lane 13 r72 (fast with 0x1e9ce0 and 0x1ea5b0, unchanged at 13 and 106; brackets: aligned count with registers and
// stack offsets masked, base 353 [35]). Retail's two award loops have no compare after the prefix search (the search
// leaves straight to the next award when it finds the player). The search as an inline early-return list predicate
// used by both loops 366 (1528 B) [37]; with the killer-team call argument read from the player byte again 365 [38];
// with the consideration flag as a true default cleared by the test 357 [35]; those three with the `if (killer !=
// NONE) { if (killer == reported_player) ... } else if (reported valid)` layout 358 [33]; plus the assist stores
// re-reading the record's player index 366 (1544 B, retail's length, first difference +0x8) [39]; the predicate with
// that killer layout alone 383 (1512 B). NOT kept: all above 353; body below unchanged.
// M3 lane 14 r79 (fast with 0x1ea770/0x1ea5b0/0x1e9ce0, unchanged): `victim != NONE && assist_threshold > 0.0f` 353
// (same code); `victim != NONE && killer != NONE` 354; `victim != NONE &&` ahead of the killer-team query 371 (1564
// bytes); `reported_slot` declared before `object` 355; `hostile = true;` ahead of the hostility count increment 353
// (same code). The three rotated scalar homes and the biased history cursor stay.
// Round 228 (m3 lane 3, fast with 0xd9640, 0x1e9700, 0x1ea770 and the other file-mates, which keep their state;
// aligned counts exact/stack-masked/register-masked in brackets, base 353 [179/160/56]): the assist index short 353,
// the entry test as an enclosing `if` instead of the early return 353, the award searches as `for` loops with a
// `break` 353 (same code each). Retail reads the related unit's player field again for the assist store (after the
// team call): that field read at each use in the related block 357 (1552 B) [170/151/53] (the block is then retail's);
// with both award searches through a two-caller early-return predicate 361 (1532 B) [166/147/49] (retail's searches
// leave straight for the next award); with the assist stores re-reading the record's player as well 371 (1544 B,
// frame 0x4c) [198/141/56], plus one history pointer for both loops 371 [198/141/55]; the selection amount in a local
// under the time test on those 441/442 (1563 B). All restored.
// Round 148 (m3 lane 7, fast; [stack+register masked, register masked] aligned counts, control 353 [53, 78]): the reported
// player's code written once after the fallback (`if (killer == NONE && valid(reported)) killer = reported; if (killer != NONE
// && killer == reported) damage_code = code;`, the same behaviour with one store fewer) 355 [55, 80]; consider_team as
// `reported_player == NONE || !function_1df560(...)` 353 (same code); victim_team set in an if/else with no NONE default 422
// [60, 84]; the victim's team byte through a named player pointer 353 (same code); the hostile arm's flag set before the
// counter 353 (same code); re-measured: the selection amount named once 454. Retail keeps killer in EDX across the
// fallback merge and reloads it from its home on the paths that skip the record (0x1ea1de) and at the team block; ours
// threads those paths into the fallback and keeps killer in EBP.
// M3 lane 12 r114 (fast with the file's 19 functions and 0xd9640; [masked aligned, register-masked], control 353 [112, 36]):
// the selection loop as a file-local helper returning the chosen slot, plain inline 686 (1308 B: it stays out of line and
// 0xd9640 then reads 61), forced inline 509 (1484 B) [155, 98]; assist_threshold set before killer in the selected block 353
// (one store moves) [112, 37]; the killer choice (selected record, then the reported fallback) as a forced-inline helper
// returning the killer and setting the damage code and threshold through pointers 366 (1548 B, 450 instructions as retail)
// [115, 39]: the caller then reloads the killer from a home at the killer block as retail does, but the helper's killer takes
// a parameter home and the caller's copy EBP; on that helper, consider_team as a true default cleared by the test 366 [113, 37]
// and the assist stores re-reading the record's player 432 [110, 48]; the object, root header and related unit through the
// existing object_get/object_header_get 353 (same code). Re-measured (listed): one history pointer for both loops, the amount
// read once under the time test and the assist stores re-reading the player 451 (1578 B) [170, 54]: the selection base stays
// +0x2f4 and selected moves to a home, so the amount's read count does not set the cursor. Also seen: ours loads the object
// pool's data ahead of the 0xbaf80 call in the related block, retail after it.
// M3 lane 16 r106 (fast with the file's functions and unknown_196d20.cpp's, all matched ones stay matched; [register-masked
// aligned], base [78]): lead: retail pushes the damage code to 0x197160 straight from its byte home (`mov esi, [esp+0x1c]; push
// esi`, reused for 0x1ea5b0), so 0x197160's code parameter is a byte (its own body reads only the low byte); typed so at its
// definition and declaration 354 [78] (0x197160 stays matched; ours then pushes as retail and the damage code has one reference
// fewer, retail's count per round 53). On it: the killer's team byte passed to 0x1df560 at the call 354 [79] (retail's `movsx edx,
// al` / `movsx si, al` pair); with `if (killer != NONE) { if (killer == reported_player) ... } else if (reported valid)` as well
// 353 [73]; that arm order alone 363 (1532 B); the arm order, team byte and a `&history[selected]` record pointer used three times
// 455 (1583 B). The three slots stay rotated (killer still kept in EBP across its block). Body restored at 353.
// MiniMax H (11-11), NOT kept (coordinator 11-11): 353 -> 344 only from consider_team set true and cleared by two nested tests in place of its one expression (a bool set by tests: match-only); body at 353.
// m3 lane 1 r98 (fast with the file's functions and 0xd9640; control 353): `best = object->history[selected].amount;` in the
// selection (to take one amount read off the walk's anchor), `else if (!consider_team) continue;` in the selection loop, and the
// assist loop's skip test split into three `continue` tests: 353 each, identical code (the selected index is copy-propagated
// before the walk is anchored). No other unlisted single form found; the cause stays the allocation one above (killer kept in
// EBP across the fallback where retail keeps its home, the three rotated slots, the +4 walk base). 0 counted trials: no
// unlisted code-changing natural form (round 163/lane 14 r79/lane 12 r114 above cover the remaining spellings).
// Round 157 (m3 lane 7, fast): the killer choice as a force-inline helper setting the killer, the damage code and the
// threshold through pointers 353 (same code: the pointer outs are promoted after inlining, so the killer does not become
// memory-resident as retail's is, stored at every assignment and reloaded at the killer block). Re-measured, the
// `&object->history[selected]` record pointer 454 (1583 bytes): `selected` then takes the reported team parameter's home,
// where retail keeps `best`, so `best` needs a new slot (frame 0x54) and the killer sorts below the victim. No counted
// trial: lane 1 r98's survey, round 163, lane 14 r79 and lane 12 r114 above cover the remaining killer-block spellings.
// @retail 0x1e9fa0
void __stdcall function_1e9fa0(void *engine_globals, long object_index, long reported_player, word reported_team, byte code)
{
	if (!*(byte *)engine_globals) return;
	s_statborg *statistics = (s_statborg *)engine_globals;
	long victim = unit_get_player_index(object_index);
	long victim_team = NONE;
	long killer_team = NONE;
	long killer = NONE;
	byte damage_code = code;
	if (victim != NONE)
		victim_team = (char)g_4e8c24->data[(victim & 0xffff) * 0x21c + 0xc0];
	long cutoff = function_146650() - real_to_long((real)g_510c54->field_2_3 * 4.0f);
	long recent = function_146650() - real_to_long((real)g_510c54->field_2_3 * 6.0f);
	s_object_damage_view *object = *(s_object_damage_view **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	short reported_slot = NONE;
	bool consider_team = !(reported_player != NONE && function_1df560((short)victim_team, (short)reported_team));
	short selected = NONE;
	real best = -FLT_MAX;
	for (short i = 0; i < 4; ++i)
	{
		long player_index = object->history[i].player_index;
		if (player_index == NONE) continue;
		if (player_index == reported_player) reported_slot = i;
		else
		{
			if (!consider_team) continue;
			byte *player = record_pool_lookup(g_4e8c24, player_index);
			if (!player || !function_1df560((short)victim_team, (char)player[0xc0])) continue;
		}
		if (object->history[i].time > (dword)recent && object->history[i].amount > best)
		{
			selected = i;
			best = object->history[i].amount;
		}
	}
	if (selected == NONE) selected = reported_slot;
	bool hostile = false;
	real assist_threshold = 0.0f;
	long assist_count = 0, other_count = 0;
	if (selected != NONE)
	{
		damage_code = object->damage_codes[selected];
		if (datum_get_inlined(g_4e8c24, object->history[selected].player_index))
		{
			killer = object->history[selected].player_index;
			assist_threshold = object->history[selected].amount * 0.4f;
		}
	}
	if (killer == NONE)
	{
		if (datum_get_inlined(g_4e8c24, reported_player)) { killer = reported_player; damage_code = code; }
	}
	else if (killer == reported_player)
		damage_code = code;
	if (killer != NONE)
	{
		byte *player = g_4e8c24->data + (killer & 0xffff) * 0x21c;
		killer_team = (char)player[0xc0];
		if (function_1df560((short)victim_team, (short)killer_team) && victim != NONE)
		{
			function_197160(killer, victim, damage_code);
			function_1ea5b0(killer, statistics, cutoff, damage_code);
		}
		else { ++*(short *)(player + 0x1c4); hostile = true; }
	}
	long assists[5], other_assists[5];
	if (assist_threshold > 0.0f && victim != NONE)
		for (long i = 0; i < 4; ++i)
		{
			long player_index = object->history[i].player_index;
			byte *player = datum_get_inlined(g_4e8c24, player_index);
			if (!player || player_index == killer ||
				(player_index != reported_player && !(object->history[i].amount > assist_threshold))) continue;
			if (function_1df560((short)victim_team, (char)player[0xc0])) assists[assist_count++] = player_index;
			else other_assists[other_count++] = player_index;
		}
	if (killer != NONE && victim != NONE)
	{
		byte *player = g_4e8c24->data + (killer & 0xffff) * 0x21c;
		if (function_1df560((short)victim_team, (char)player[0xc0]))
		{
			long killer_unit = *(long *)(player + 0x2c);
			if (killer_unit != NONE)
			{
				long root = function_baf80(killer_unit);
				byte *header = g_4e0300->data + (root & 0xffff) * 12;
				if ((1 << header[3]) & 3)
				{
					byte *root_unit = *(byte **)(header + 8);
					long related = *(long *)(root_unit + 0x248);
					if (related != NONE && related != killer_unit)
					{
						byte *related_unit = *(byte **)(g_4e0300->data + (related & 0xffff) * 12 + 8);
						long related_player = *(long *)(related_unit + 0x13c);
						if (related_player != NONE)
						{
							byte *other = g_4e8c24->data + (related_player & 0xffff) * 0x21c;
							if (function_1df560((short)victim_team, (char)other[0xc0]) && (dword)assist_count < 5)
								assists[assist_count++] = related_player;
						}
					}
				}
			}
		}
	}
	if (victim != NONE)
	{
		function_15ce70(killer, victim, hostile, damage_code);
		if (killer != NONE)
		{
			function_196dd0(killer & 0xffff, 0, victim & 0xffff, 1);
			function_196dd0(victim & 0xffff, 1, killer & 0xffff, 1);
		}
	}
	for (long i = 0; i < assist_count; ++i)
	{
		long j = 0;
		while (j < i && assists[i] != assists[j]) ++j;
		if (j == i) { function_1e9ce0(assists[i], 8, statistics, 6, 1, true); *(dword *)g_4e9ae8 |= 0x20; }
	}
	for (long i = 0; i < other_count; ++i)
	{
		long j = 0;
		while (j < i && other_assists[i] != other_assists[j]) ++j;
		if (j == i) function_1ea880(victim, other_assists[i], (s_object_values *)statistics);
	}
	if (victim != NONE)
	{
		bool self = reported_player == victim;
		bool friendly = function_15eaf0() && victim_team != NONE && killer_team != NONE && victim_team == killer_team;
		function_1ea770(damage_code, victim, statistics, killer, self, friendly);
	}
}

// score 13 (was 13): retail reads the field argument into EDX before the
// prologue pushes and adds it there; here the load happens after the pushes
// and goes into ECX, which forces the counter to be moved out first.
// The first engine test also takes EAX/ECX where retail takes ECX/EAX.
// Tried: a plain field read 85 (folded into the add); an inline entry
// accessor 85; game_engine_get() and function_x340af0() for the tests 13;
// local names 13; an old-value local 16 (engine test then EDX/EAX); a word
// or dword index 44-61 (convention changes). Each of these folds the field
// read into the add (85, one row short): a plain short delta, a volatile
// field parameter, nested row structs, values[0][field + index * 14] and
// other flat spellings, delta + old, the indirection without volatile; the
// array re-indexed at each use (also with inline get/set helpers) 80 (the
// delta read is then the one hoisted into EDX). The clamp as one ternary or
// into a short local 13 (the engine test swap stays). The arguments arrive
// as in retail (player in EAX, counter in ECX, four pushes) and 0x1ea5b0's
// call already matches, so that caller does not decide the load order.
// 0x15b7c0's clamped-add helper (delta first) 84; a plain short delta and
// field 85 (and 0x15b7c0/0x15b3a0 lose their match through the declaration);
// the field read after the index 83, or inside the index expression 85; a row
// pointer indexed by the field at each use 64 (0x15b3a0 and 0x1ea770 lose theirs).
// Round 11 (lane 17): the team path through the existing function_x340af0() and
// datum_get_absolute_inlined() (kept: same code, no parameter reuse); the first engine test
// through game_engine_get() 13; the clamp as two ifs 13, upper bound first 19; a plain delta
// parameter 18 (its movsx is then hoisted to entry, the field read folds); the field read
// through its address without volatile 85; the volatile read after the index 83 (statistics is
// then hoisted), inside the subscript or as a row offset 85 (folded).
// Round 43 (lane 15, fast; a CL-only copy with counter in ECX and player in EDX reproduces the folded add):
// the plain field stops folding (retail's `mov edx, [esp+8]` first line, the plain read hoisted) as soon as
// the engine globals pointer is a named local defined in the entry block: at the top, after the index or after
// the entry 24 each (only that global's load then sits at the top instead of the clamp's join, plus the engine
// test swap). Defined at the join 54 (254 bytes); so what keeps the field unfolded with the load at the join is
// still open. The engine index read into a local before the store gives retail's ECX/EAX test 12 (its load then
// sits ahead of the store); after the store 13; the peer read before the store 14; peer and value28 locals 14;
// the engine pointer through the getter after or before the store 13/13. Statistics first 85; statistics then
// field first 85 (all callers updated; retail's calls set ECX then EAX and do not order the stack arguments
// against them). Plain first read with the later read through a plain or volatile reference or volatile cast 85;
// both reads volatile 85 (diagnostics, reverted); the team call passing 0 (diagnostic) moves the field to a
// register argument, 92; the players pointer named at the top is propagated, 85. CL-only, all folded: statistics,
// delta and by_team qualifiers (16 forms), clamp forms (17: pin helpers, min/max, stores in the arms), the entry
// in two steps (row pointer then `+= field`, offset in place, field first; 7).
// Round 76: a plain field snapshot reused for the team update 85; a register snapshot with two/one
// uses 85/85; a short volatile delta declaration 13; an unsigned field parameter 13; an unsigned
// plain snapshot reused for the team update 85; the field copied with memcpy 85; a first-engine-peer
// local 13. Original body and declarations restored.
// Round 202 (lane 3): the entry as `values[index] + field` 13, as a byte offset 13; `(short)delta + *entry` 13,
// `value = *entry; value += delta` 25, delta through a short local 84; a table pointer local 13; the index
// before the field read 83; a `(word)` index 13; the clamp as one ternary in the store 13; statistics through
// a volatile reference with a plain field 85; a short field local 81 (263 bytes).
// Round 89 (m3 lane 12, fast), plain field read: the element indexed twice, with the entry pointer first or the value first, or the entry pointer
// after the clamp, delta on either side of the sum: 85 each (the field stays folded; two reads do not unfold it). The engine pointer named at the top 24 (the
// note's form, re-measured: the entry block is then exact but for that load's place, plus the engine test swap); named after the entry 69; named at the top and
// assigned again at the join 85, declared without a value and assigned at the join 85, NULL then assigned at the join 85 (a dead definition does not unfold it).
// Round 169 (lane 4, fast): a plain delta parameter (field read kept) 18, re-measured: delta's movsx then takes EDX at
// the top, where retail has the field. Both volatiles removed (and the field named `column` as in 0x1e9c90) 85, but
// that body is retail's but for two places: the folded `add eax, [esp+0x18]` (one instruction short, which shifts the
// rest) and the first engine test's EAX/ECX swap. On that plain body: the first test through game_engine_get() 85; the
// notify through a force-inline helper taking the bit (diagnostic, this caller only) 85; a `long const` field 85.
// Round 73 (lane 14, fast): the dirty test held in a bool before the store 86 (270 bytes); plain field with the engine
// globals in a local at the top for the dirty test 24 (field at entry, engine load at the top), with the engine peer read
// before the store 25; plain field with only the peer read before the store 85 (folded); the volatile field with the peer
// read before the store 14.
// Round 143 (m3 lane 7, fast; the two volatiles cannot carry a match, so volatile-free scores too):
// volatile-free control 85; volatile-free with the engine globals pointer named at the top 32 (the field
// load is retail's, the delta movsx is hoisted into EDX, the engine load sits at the top), with the volatile
// delta 24, named at the join 85; a named field copy for the entry and the team call 80 volatile-free, 85
// with the volatile delta, for the entry only 85; the delta read through its own address with the top
// engine local 32, alone 85, at the join 85; a 600 s permuter run, 59 tries, none below 13.
// Round 110 (m3 lane 16, fast with the matched callers 0x1ea770, 0x15b3a0, 0x15b7c0 and callees; volatile-free body
// re-measured 85, the top engine globals local re-measured 32): retail's field is a stack parameter cached in EDX for the
// entry block and read from its slot again for the team call; one parameter at most gets that EDX entry load. On the
// volatile-free body: `by_team` copied into a local at the top for the team test 80 (256 bytes: delta's movsx takes the top
// EDX load, the field stays folded); statistics read once through the siblings' `s_statborg *const *` own-address reference
// 80 (same code as the copy); that reference with the volatile delta kept 85 (folded; delta out of the way does not hand the
// field the load); `value = *entry; value += (short)delta;` 85; the entry as `values[index] + field` 85 (same code as the
// volatile-free control); re-measured, round 169's first test through game_engine_get() with the peer named 85.
// Round 87 (m3 lane 14, fast with 0x1ea770, 0x15b3a0, 0x15b7c0, 0x1e9df0, 0x1968b0; volatile-free forms): control 85
// (260 bytes, retail's but for the folded add and the first engine test's swap); an unused local reading the field at
// the top (diagnostic) 80 (256: delta's movsx takes the EDX entry load and the old value is read through the entry
// pointer); `(void)field;` 85; the value read as the array element with the entry pointer kept for the store 80 (256);
// a row pointer indexed by the field at each use 270 bytes, and 0x15b3a0 (13) and 0x1ea770 (26) lose their matches;
// the first engine test through game_engine_get() 85.
// Round 157 (m3 lane 7, fast with 0x1ea770, 0x1ea5b0, 0x1e9df0, 0x15b3a0, 0x15b7c0): the dirty test through the existing
// game_engine_statborg_inline() 83 (270 bytes), on the volatile-free body 94 (268); the volatile-free body with a row pointer
// local and `&row[field]` 80 (256: delta's movsx takes the EDX entry load, as in round 87's diagnostic, the field stays folded);
// the team block through the header's s_player_datum (`player->team`) 13 (same code). Diagnostics: the volatile-free body with
// the engine globals local at the top loads the field into EDX at entry (32, round 143's form); without the team call's use of
// the field, LTCG makes it a register argument (89; 0x15b3a0, 0x1ea770 and 0x1ea5b0 change), so its cross-call use is what
// keeps it a stack parameter. Stopped there: rounds 43, 87, 110, 143 and 169 cover the remaining entry and clamp spellings.
// @retail 0x1e9ce0
void function_1e9ce0(long player_index, long counter, s_statborg *statistics,
	long field, long volatile delta, bool by_team)
{
	long const volatile *field_reference = &field;
	long stored_field = *field_reference;
	long index = player_index & 0xffff;
	short *entry = &((s_object_values *)statistics)->values[index][stored_field];
	long value = *entry + (short)delta;
	if (value < -30000) value = -30000;
	else if (value > 30000) value = 30000;
	*entry = (short)value;
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
	if (counter != NONE) function_1968b0(index, NONE, counter, *entry);
	if (by_team && function_x340af0())
	{
		byte *player = datum_get_absolute_inlined(g_4e8c24, index);
		if (player && (char)player[0xc0] != NONE)
			function_1e9df0((char)player[0xc0], statistics, field, counter, delta);
	}
}

// @retail 0x1e9c90
void function_1e9c90(long object_index, long column, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	long index = object_index & 0xffff;
	(*table_reference)->values[index][column] = 0;
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
}

// @retail 0x1e9f40
void function_1e9f40(long object_index, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	long index = object_index & 0xffff;
	memset((*table_reference)->values[index], 0, sizeof(table->values[index]));
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
}

// @retail 0x1e97b0
void function_1e97b0(s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	for (long i = 0; i < 16; i++)
	{
		short *row = (*table_reference)->values[i];
		row[9] = 0;
		*(long *)&row[12] = 0;
		row[10] = 0;
		if (row[0] != 0)
		{
			row[1] += row[0];
			if (!g_4e6948->flag1128)
				row[0] = 0;
			if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
				function_b58c0(g_4e9ae8->value28, 1 << i);
		}
	}
	long team = 0;
	do
	{
		short *row = (*table_reference)->team_values[team];
		if (row[0] != 0)
		{
			row[1] += row[0];
			if (!g_4e6948->flag1128)
				row[0] = 0;
			if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
				function_b58c0(g_4e9ae8->value28, 1 << (team + 16));
		}
		team++;
	} while (team < 8);
}

struct s_object_values_packet
{
	short values[16][9];
	short team_values[8][9];
};

struct s_team_value_row
{
	short first;
	short values[8];
};

PRIVATE __forceinline bool update_team_value_row(s_team_value_row *destination, const short *source)
{
	bool same = true;
    short *values = destination->values;
	if (destination->first != source[0]) { destination->first = source[0]; same = false; }
	if (values[0] != source[1]) { values[0] = source[1]; same = false; }
	if (values[1] != source[2]) { values[1] = source[2]; same = false; }
	if (values[2] != source[3]) { values[2] = source[3]; same = false; }
	if (values[3] != source[4]) { values[3] = source[4]; same = false; }
	if (values[4] != source[5]) { values[4] = source[5]; same = false; }
	if (values[5] != source[6]) { values[5] = source[6]; same = false; }
	if (values[6] != source[7]) { values[6] = source[7]; same = false; }
	if (values[7] != source[8]) { values[7] = source[8]; same = false; }
	return same;
}

PRIVATE __forceinline bool update_value_row(short *destination, const short *source)
{
	bool same = true;
	if (destination[0] != source[0]) { destination[0] = source[0]; same = false; }
	if (destination[1] != source[1]) { destination[1] = source[1]; same = false; }
	if (destination[2] != source[2]) { destination[2] = source[2]; same = false; }
	if (destination[3] != source[3]) { destination[3] = source[3]; same = false; }
	if (destination[4] != source[4]) { destination[4] = source[4]; same = false; }
	if (destination[5] != source[5]) { destination[5] = source[5]; same = false; }
	if (destination[6] != source[6]) { destination[6] = source[6]; same = false; }
	if (destination[7] != source[7]) { destination[7] = source[7]; same = false; }
	if (destination[8] != source[8]) { destination[8] = source[8]; same = false; }
	return same;
}

// @retail 0x1e98e0
void __stdcall function_1e98e0(void *state, long mask_address, void *data)
{
	void *const *state_reference = &state;
	const long *mask_reference = &mask_address;
	void *const *data_reference = &data;
	s_object_values *table = (s_object_values *)*state_reference;
	s_object_values_packet *packet = (s_object_values_packet *)*data_reference;
	dword *mask = (dword *)*mask_reference;
	dword changed = 0;
	for (long i = 0; i < 16; i++)
	{
		dword selected = 1u << i;
		if (selected & *mask)
		{
			if (!update_value_row(packet->values[i], table->values[i]))
				changed |= selected;
			else
				changed &= ~selected;
		}
	}
	short const *source = table->team_values[0] + 2;
	for (long team = 0; team < 8; source += 9, team++)
	{
		if (*mask & (1 << (team + 16)))
		{
			short *destination = packet->team_values[team] + 8;
			bool same = true;
			if (destination[-8] != source[-2]) { destination[-8] = source[-2]; same = false; }
			if (destination[-7] != source[-1]) { destination[-7] = source[-1]; same = false; }
			if (destination[-6] != source[0]) { destination[-6] = source[0]; same = false; }
			if (destination[-5] != source[1]) { destination[-5] = source[1]; same = false; }
			if (destination[-4] != source[2]) { destination[-4] = source[2]; same = false; }
			if (destination[-3] != source[3]) { destination[-3] = source[3]; same = false; }
			if (destination[-2] != source[4]) { destination[-2] = source[4]; same = false; }
			if (destination[-1] != source[5]) { destination[-1] = source[5]; same = false; }
			if (destination[0] != source[6]) { destination[0] = source[6]; same = false; }
			if (!same) changed |= 1 << (team + 16);
			else changed &= ~(1 << (team + 16));
		}
	}
	*mask = changed;
}

void function_225880(long player_index);
void function_225910(long player_index);

// @retail 0x1e9ad0
bool __stdcall function_1e9ad0(void *state, long mask, void *data)
{
	s_object_values *table = (s_object_values *)state;
	s_object_values_packet *packet = (s_object_values_packet *)data;
	for (long i = 0; i < 16; i++)
	{
		short *row = table->values[i];
		if (mask & (1 << i))
		{
			for (long column = 0; column < 9; column++)
			{
				if (column == 5 || column == 8)
				{
					long count = packet->values[i][column] - row[column];
					if (count > 0)
					{
						long player_index = NONE;
						if (datum_get_absolute_inlined(g_4e8c24, i))
							player_index = data_datum_index(g_4e8c24, i);
						if (player_index != NONE)
						{
							byte *player = NULL;
							long index = player_index & 0xffff;
							if (index < g_4e8c24->high_water_index)
							{
								byte *entry = g_4e8c24->data + index * g_4e8c24->size;
								if (*(short *)entry && *(short *)entry == player_index >> 16)
									player = entry;
							}
							if (player && *(short *)(player + 0x28) != NONE)
							{
								do
								{
									if (column == 8)
										function_225910(player_index);
									else
										function_225880(player_index);
								} while (--count);
							}
						}
					}
				}
				row[column] = packet->values[i][column];
			}
		}
	}
	for (long team = 0; team < 8; team++)
	{
		if (mask & (1 << (team + 16)))
		{
			short *destination = &table->team_values[team][0];
			for (long column = 0; column < 9; column++)
				destination[column] = packet->team_values[team][column];
		}
	}
	return true;
}

typedef void (__stdcall *text_callback)(const char *text, long context);

struct s_text_callback_state
{
	text_callback begin;
	text_callback field_4;
	text_callback end;
	long context;
	text_callback override_begin;
	text_callback field_14;
	text_callback override_end;
	long override_context;
	bool disabled;
	bool pending;
	char text[0x100];
	long index;
	long value;
};

s_text_callback_state g_4f55f8;

// @retail 0x1e9650
void function_1e9650()
{
	if (g_4f55f8.pending)
	{
		if (!g_4f55f8.disabled)
		{
			if (g_4f55f8.override_end)
				g_4f55f8.override_end(g_4f55f8.text, g_4f55f8.override_context);
			else if (g_4f55f8.end)
				g_4f55f8.end(g_4f55f8.text, g_4f55f8.context);
		}
		g_4f55f8.pending = false;
	}
}

// @retail 0x1e95d0
void __stdcall function_1e95d0(const char *text)
{
	const char *const *text_reference = &text;
	function_1e9650();
	g_4f55f8.pending = true;
	strncpy(g_4f55f8.text, *text_reference, sizeof(g_4f55f8.text));
	g_4f55f8.text[0xff] = 0;
	g_4f55f8.index = NONE;
	g_4f55f8.value = 0;
	if (!g_4f55f8.disabled)
	{
		if (g_4f55f8.override_begin)
			g_4f55f8.override_begin(g_4f55f8.text, g_4f55f8.override_context);
		else if (g_4f55f8.begin)
			g_4f55f8.begin(g_4f55f8.text, g_4f55f8.context);
	}
}

// @retail 0x1e9e80
void function_1e9e80(long first, s_object_values *table, long second)
{
	long const *second_reference = &second;
	short temporary[14];
	if (g_4e6948->mode != 4)
	{
		memcpy(temporary, table->values[first], sizeof(temporary));
		memcpy(table->values[first], table->values[*second_reference], sizeof(temporary));
		memcpy(table->values[*second_reference], temporary, sizeof(temporary));
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << first);
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << *second_reference);
	}
}

/* The 35 short entries beginning at retail 0x445580. */
const unsigned short g_445580[35] =
{
	4, 5, 6, 7, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1
};

// Both the incoming row and the alternate table row are 16-bit values;
// keeping them narrow reproduces retail's partial-register table load.
// @retail 0x1e9720
real function_1e9720(short kind, short team)
{
	short difficulty;
	if (g_4e6948->state == 1)
		difficulty = g_4e6948->difficulty;
	else
		difficulty = 1;
	if (g_4e6948->state == 2)
	{
		difficulty = 1;
	}
	else if (g_4e6948->state == 1 && !team_is_enemy(1, team))
	{
		short alternate = g_445580[(short)kind];
		if ((short)alternate == NONE)
			difficulty = 1;
		else
			kind = alternate;
	}
	return function_1e96a0(difficulty, (short)kind);
}

void function_225910(long player_index);
void function_162bf0(long player_index, long spectated_player_index);

struct s_counted_player
{
	byte field_0[0x28];
	short local_index;
	byte field_2a[0x21c - 0x2a];
};

// @retail 0x1ea880
void function_1ea880(long arg_1f407d, long player_index, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	if (player_index != NONE && arg_1f407d != player_index)
	{
		long index = player_index & 0xffff;
		if (((s_counted_player *)g_4e8c24->data)[index].local_index != NONE)
			function_225910(player_index);
		function_162bf0(arg_1f407d, player_index);
		short *entry = &(*table_reference)->values[index][8];
		long value = *entry + 1;
		if (value < -30000)
			value = -30000;
		else if (value > 30000)
			value = 30000;
		*entry = (short)value;
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << index);
	}
}

struct s_statborg;
void function_1968b0(long index, long team, long counter, long value);

struct s_request_screen_bounds
{
	short top, left, bottom, right;
};

void function_1e90b0(s_request_screen_bounds const *bounds, point2f *points, long first);

struct s_bitmap_data;
struct D3DTexture;
struct s_widget_quad_2b11;
struct s_float_rect;
D3DTexture *function_3bcb0(s_bitmap_data *bitmap);
void function_22a664(s_widget_quad_2b11 const *quad, s_float_rect const *coordinates,
	long bitmap_index, long sequence, long color);

// @retail 0x1e9140
bool __stdcall function_1e9140(s_request_screen_bounds const *bounds)
{
	byte *definition = (byte *)g_510c94;
	long color = *(long *)(definition + 0x444);
	bool result = true;
	long diagonal, corner, edge, center;
	bool alternate = false;
	if (g_4b9ed8 != NONE)
	{
		long player_index = g_4e8c20->entries[g_4b9ed8];
		if (player_index != NONE)
		{
			byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
			alternate = player[0x88] == 1 || player[0x88] == 3;
		}
	}
	if (alternate)
	{
		corner = *(long *)(definition + 0x47c);
		diagonal = *(long *)(definition + 0x46c);
		edge = *(long *)(definition + 0x474);
		center = *(long *)(definition + 0x484);
	}
	else
	{
		corner = *(long *)(definition + 0x45c);
		diagonal = *(long *)(definition + 0x44c);
		edge = *(long *)(definition + 0x454);
		center = *(long *)(definition + 0x464);
	}
	if (color != NONE && corner != NONE && diagonal != NONE && edge != NONE && center != NONE)
	{
		s_bitmap_data *corner_bitmap = *(s_bitmap_data **)(g_4e3b44[corner & 0xffff].bytes + 0x48);
		s_bitmap_data *diagonal_bitmap = *(s_bitmap_data **)(g_4e3b44[diagonal & 0xffff].bytes + 0x48);
		s_bitmap_data *edge_bitmap = *(s_bitmap_data **)(g_4e3b44[edge & 0xffff].bytes + 0x48);
		s_bitmap_data *center_bitmap = *(s_bitmap_data **)(g_4e3b44[center & 0xffff].bytes + 0x48);
		if (!function_3bcb0(corner_bitmap))
			result = false;
		if (!function_3bcb0(diagonal_bitmap))
			result = false;
		if (!function_3bcb0(edge_bitmap))
			result = false;
		if (!function_3bcb0(center_bitmap))
			result = false;
		if (result)
		{
			s_request_screen_bounds inner;
			inner.left = bounds->left + 8;
			inner.top = bounds->top + 8;
			inner.right = bounds->right - 8;
			inner.bottom = bounds->bottom - 8;
			inner.left += g_4b9dd2;
			inner.right += g_4b9dd2;
			inner.bottom += g_4b9dd0;
			inner.top += g_4b9dd0;
			s_request_screen_bounds piece = inner;
			real coordinates[4];
			coordinates[0] = 0.0f;
			coordinates[1] = 1.0f;
			coordinates[2] = 0.0f;
			coordinates[3] = 1.0f;
			point2f points[4];
			function_1e90b0(&piece, points, 0);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, center, 0, color);
			short top = inner.top - 10;
			piece.top = top;
			piece.left = inner.left;
			piece.right = inner.right;
			piece.bottom = inner.top;
			function_1e90b0(&piece, points, 0);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			piece.top = inner.bottom;
			short bottom = inner.bottom + 10;
			piece.bottom = bottom;
			piece.left = inner.left;
			piece.right = inner.right;
			function_1e90b0(&piece, points, 2);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			short left = inner.left - 10;
			piece.left = left;
			piece.bottom = inner.bottom;
			piece.top = inner.top;
			piece.right = inner.left;
			function_1e90b0(&piece, points, 1);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			short right = inner.right + 10;
			piece.right = right;
			piece.bottom = inner.bottom;
			piece.left = inner.right;
			piece.top = inner.top;
			function_1e90b0(&piece, points, 3);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			piece.left = left;
			piece.top = top;
			piece.right = inner.left;
			piece.bottom = inner.top;
			function_1e90b0(&piece, points, 0);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, corner, 0, color);
			piece.top = inner.bottom;
			piece.right = right;
			piece.bottom = bottom;
			piece.left = inner.right;
			function_1e90b0(&piece, points, 2);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, corner, 0, color);
			piece.left = inner.right;
			piece.top = top;
			piece.right = right;
			piece.bottom = inner.top;
			function_1e90b0(&piece, points, 0);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, diagonal, 0, color);
			piece.left = left;
			piece.top = inner.bottom;
			piece.bottom = bottom;
			piece.right = inner.left;
			function_1e90b0(&piece, points, 2);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, diagonal, 0, color);
		}
	}
	return result;
}

// The new statistic callers allow this body without the earlier LTCG regressions.
// Parameter order from retail's calls: 0x15b650 sets ECX, EAX and then EDX (a register copy) last,
// 0x15b3a0 computes EDX between the delta and team pushes.
// @retail 0x1e9df0
void function_1e9df0(long team, s_statborg *statistics, long field, long counter, short volatile delta)
{
 short *entry = &((s_object_values *)statistics)->team_values[team][field];
 long value = *entry + delta;
 if (value < -30000)
  value = -30000;
 else if (value > 30000)
  value = 30000;
 *entry = (short)value;
 if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
  function_b58c0(g_4e9ae8->value28, 1 << (team + 16));
 if (counter != NONE)
  function_1968b0(NONE, team, counter, *entry);
}



#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include <wchar.h>
struct s_510c4c;
extern s_510c4c *g_510c4c;
struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;
extern real g_4e69c0[4];
extern long g_4ba04c;
void function_13925f(long string_handle, word *buffer);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_22d2ee(word *string, long maximum_length);
bool function_162c50(long player_index, long *spectated_player_index);
bool function_2259f0(long player_index);
bool function_22acb4(long local_player_index);
void function_13edb0(long font, long style, long justification, dword flags, color4f const *color, color4f const *field_24);
void function_13ec70(color4f const *color);
void function_13e9c0(word const *text, short_rectangle2d const *bounds, short_rectangle2d *a, short_rectangle2d *b, real scale);
long function_1e8cb0(long index);
struct s_entry_420;
s_entry_420 *function_1e8d50(long index);
class c_1fa50
{
public:
 void function_1fb10(short_rectangle2d const *bounds, real scale) const;
};

/* the fade of the given view, opaque outside the four views */
PRIVATE inline real view_alpha_1e6fe0(long index)
{
 real result;
 if (index >= 0 && index < 4)
  result = g_4e69c0[index];
 else
  result = 1.0f;
 return result;
}

struct s_transition_1e6fe0
{
 short state;
 short ticks;
};

/* a local player's block (0x1b0 bytes) of g_51e9c0 as 0x1e6fe0 reads it */
struct s_local_player_state_1e6fe0
{
 s_transition_1e6fe0 transitions[32];
 long current;
 long time;
 byte unknown088[0x1b0 - 0x88];
};
typedef char assert_local_player_state_1e6fe0_size[(sizeof(s_local_player_state_1e6fe0) == 0x1b0) ? 1 : -1];

// @retail 0x1e6fe0
void function_1e6fe0(long local_player_index)
{
 byte *globals = (byte *)g_51e9c0;
 if (!globals[0x6c5])
  return;
 word text[256];
 word temporary[256];
 text[0] = 0;
 bool spectating = false;
 if (local_player_index != NONE)
 {
  long player_index = g_4e8c20->entries[local_player_index];
  long spectated_player;
  if (player_index != NONE && function_162c50(player_index, &spectated_player))
  {
   *(long *)((byte *)g_510c4c + 0x1b8) = spectated_player;
   temporary[0] = 0;
   if (function_2259f0(player_index))
    function_13925f(*(long *)((byte *)g_510c94 + 0x438), temporary);
   else
    function_13925f(*(long *)((byte *)g_510c94 + 0x43c), temporary);
   wcsncpy((wchar_t *)text, (wchar_t const *)temporary, 255);
   text[255] = 0;
   function_22d2ee(text, 256);
   *(long *)((byte *)g_510c4c + 0x1b8) = NONE;
   spectating = true;
  }
 }
 if (!spectating)
 {
  long string_handle;
  if (globals[0x6c4] && *(long *)(globals + 0x6c0) != 0 && *(long *)(globals + 0x6c0) != NONE)
   string_handle = *(long *)(globals + 0x6c0);
  else
  {
   s_local_player_state_1e6fe0 *players = (s_local_player_state_1e6fe0 *)globals;
   long index = players[local_player_index].current;
   s_local_player_state_1e6fe0 *state = &players[local_player_index];
   if (index == NONE)
    return;
   s_transition_1e6fe0 *transition = &players[local_player_index].transitions[index];
   if (transition->state != 2 && transition->state != 3)
    return;
   byte *entry = (byte *)function_1e8d50(index);
   if (!entry)
    return;
   word delay = *(word *)(entry + 4);
   if (delay && g_510c54->field_2_3 * delay <= state->time)
    return;
   string_handle = function_1e8cb0(state->current);
  }
  temporary[0] = 0;
  if (g_510c94 && g_510c94->string_list != NONE)
   function_1a0180(g_510c94->string_list, string_handle, temporary);
  wcsncpy((wchar_t *)text, (wchar_t const *)temporary, 255);
  text[255] = 0;
  function_22d2ee(text, 256);
 }
 if (!text[0])
  return;
 color4f default_color = *(color4f const *)g_4686d4;
 long font = g_4ba04c <= 1 ? 6 : 5;
 color4f color;
 bool alternate = function_22acb4(g_4b9ed8);
 if (alternate)
 {
  color.alpha = view_alpha_1e6fe0(g_4b9ed8);
  color.red = 0.9647058844566345f;
  color.green = 0.8627451062202454f;
 }
 else
 {
  color.alpha = view_alpha_1e6fe0(g_4b9ed8);
  color.red = 0.7137255072593689f;
  color.green = 0.8588235378265381f;
 }
 color.blue = 1.0f;
 default_color.alpha = view_alpha_1e6fe0(g_4b9ed8);
 function_13edb0(font, NONE, 2, 0, &color, &default_color);
 function_13ec70(&color);
 short_rectangle2d limits;
 limits.left = -32768; limits.top = -32768; limits.right = 32767; limits.bottom = 32767;
 short_rectangle2d bounds, measured;
 function_13e9c0(text, &limits, &bounds, &measured, 1.0f);
 long center_x = (g_4b9dd8.left + g_4b9dd8.right) / 2 - g_4b9dd2;
 long center_y = (g_4b9dd8.top + g_4b9dd8.bottom) / 2 - g_4b9dd0;
 long half_width = (short)(bounds.right - bounds.left) / 2 + 5;
 long half_height = (short)(bounds.bottom - bounds.top) / 2 + 1;
 limits.left = (short)(center_x - half_width);
 limits.top = (short)(center_y - half_height);
 limits.right = (short)(center_x + half_width);
 limits.bottom = (short)(center_y + half_height);
 if (function_1e9140((s_request_screen_bounds const *)&limits))
  function_1fa50(&limits, (word const *)text, NULL, NULL, 0, 1.0f, 0, NULL);
}

/* Each corner is written through a point pointer. 0x1e9140 depends on this: with the
   corners written as indexed stores, link-time code generation reorders its first loads
   (the window offset is read after the bottom edge, against retail). */
// @retail 0x1e90b0
void function_1e90b0(const s_request_screen_bounds *bounds, point2f *points, long first)
{
	real left = (real)bounds->left;
	real right = (real)bounds->right;
	real top = (real)bounds->top;
	real bottom = (real)bounds->bottom;
	point2f *point = &points[first % 4];
	point->x = left;
	point->y = top;
	point = &points[(first + 1) % 4];
	point->x = right;
	point->y = top;
	point = &points[(first + 2) % 4];
	point->x = right;
	point->y = bottom;
	point = &points[(first + 3) % 4];
	point->x = left;
	point->y = bottom;
}
