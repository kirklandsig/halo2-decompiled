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
void function_1e9df0(long field, long counter, long team, s_statborg *statistics, short delta);
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

// score 107 (was 107): no frame; enable observations and maximum/register homes differ.
// Earlier 107 (was 107): this round: the previous maximum read through a 16-bit local (112 at 419
// MiniMax R (11-01): no new form, no lower score; retail's maximum lives in ECX with the counter
// in AX and the enable byte in AL, ours keeps the maximum in EAX, the enable byte in DL and
// materialises the -1 default twice; still BLOCKED-BY 0x196960/0x1968b0 in another lane's file.
// score 107 (was 107): this round: the previous maximum read through a 16-bit local (112 at 419
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
// @retail 0x1ea5b0
void function_1ea5b0(long player_index, s_statborg *statistics, long threshold, byte code)
{
	long kind = function_196ef0(code);
	long index = player_index & 0xffff;
	struct s_team_player { byte before[0xc0]; char team; byte after[0x21c - 0xc1]; };
	s_team_player *player = ((s_team_player *)g_4e8c24->data) + index;
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
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		code = (byte)g_4e6948->flags184.bit0;
	}
	else code = 0;
	if (code && player->team != NONE)
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

// score 353 (was 353): frame 0x50 and the inline rounding now as retail; victim_team/killer/damage_code sit in
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
			function_1e9df0(field, counter, (char)player[0xc0], statistics, delta);
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

// score 2 (was 2): the two words are read in the other order: retail loads the
// global into CX before it reads the word of the record into AX.
// Right offset right after its inset 197; bounds copied to a local first 302; the
// rectangles as short_rectangle2d 2; top inset after the other offsets 123; piece
// declared before inner 2.
// Round 41 (lane 10): inset and offset in one expression per edge 207 (1162 bytes);
// top offset before bottom 2; bounds read directly without the `input` alias 2 (kept);
// both screen offsets read into named locals first 44; horizontal edges inset and
// offset before the vertical ones 52. The compile-only harness gives retail's load
// order for the current source, so the swap comes from link-time scheduling.
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
// @retail 0x1e9df0
void function_1e9df0(long field, long counter, long team, s_statborg *statistics, short volatile delta)
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

// @retail 0x1e90b0
void function_1e90b0(const s_request_screen_bounds *bounds, point2f *points, long first)
{
	real left = (real)bounds->left;
	real right = (real)bounds->right;
	real top = (real)bounds->top;
	real bottom = (real)bounds->bottom;
	points[first % 4].x = left;
	points[first % 4].y = top;
	points[(first + 1) % 4].x = right;
	points[(first + 1) % 4].y = top;
	points[(first + 2) % 4].x = right;
	points[(first + 2) % 4].y = bottom;
	points[(first + 3) % 4].x = left;
	points[(first + 3) % 4].y = bottom;
}
