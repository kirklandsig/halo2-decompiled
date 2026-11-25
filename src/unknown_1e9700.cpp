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
#include "unknown_124f90.h"

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

// MiniMax D (11-20): no new form kept, no lower score (76): the row pointer formed after the increment with the limits as
// longs gives 83 here (it swaps the EBP and EDI homes of the player pointer and the kind, so the first difference moves to
// +0x16), the limits as longs alone 78, the enable byte read at each use 103, the old maximum as a short 76 (its counter
// read then sign-extends where retail zeroes the register), and the row pointer spelled out at every use 85; what still
// differs is the enable byte, which retail reloads into AL and CL after the statistics write while this build keeps it
// in a bool local, the old maximum's home (ECX there, EAX here), the 15-bit counter read (retail's `xor eax, eax; mov ax`
// comes from the inlined getter in the held file) and the consecutive count's register.
// MiniMax I (11-20): no new form, no lower score (76): reading the enable byte at each use point instead of through the bool local gives 103 (the reload then lands where retail has it), the old maximum as a nested ternary 108, and the increment before the row pointer 107 with the direct enable read and 85 alone (it swaps the EBP and EDI roles); what still differs is the old maximum's home (ECX there, EAX here), the 15-bit value read (a zeroed 16-bit move there, movzx here) and the row pointer's register order.
// MiniMax S (11-20): score 72 (was 76): the increment is now the address expression `++*(short *)((byte *)statistics +
// index * 0x1c + 0x16)` and the row pointer is formed after it, which gives retail's `inc word ptr [eax + edx + 0x16];
// lea edi, [eax + edx]`, and the clamp's limits are longs, which gives retail's two `movzx word ptr` reads and its
// thirty-two-bit comparison; the old maximum's read moves after the increment (it reads the same counter, which the
// increment does not touch). What still differs: the enable byte is still cached in a bool local where retail reloads
// it into AL and CL, the old maximum is still in EAX where retail keeps it in ECX and the consecutive count in DX
// where retail keeps it in AX, the fifteen-bit counter is read with movzx where retail uses `xor eax, eax; mov ax,
// [mem]`, and the game-time load keeps this build's registers.
// score 76 (was 76): Retail reloads enable after the player statistics write and carries the maximum in ECX.
// The existing getter and setter live in a separate /Ob1 translation unit. The global layout affects alias analysis.
// MiniMax I (11-20): no new form, no lower score (76): reading the enable byte at each use point instead of through the bool local gives 103 (the reload then lands where retail has it), the old maximum as a nested ternary 108, and the increment before the row pointer 107 with the direct enable read and 85 alone (it swaps the EBP and EDI roles); what still differs is the old maximum's home (ECX there, EAX here), the 15-bit value read (a zeroed 16-bit move there, movzx here) and the row pointer's register order.
// MiniMax B (11-20): no new form, no lower score (76): the enable byte's raw reload needs one address-taken object that src/unknown_1967d0.cpp holds (BLOCKED-BY 0x196960/0x1968b0); what still differs is that reload, the old maximum's home (EAX here, ECX in retail), the 15-bit value read (movzx against retail's xor/mov) and the row pointer's register order.
// Earlier score 76 (was 76): BLOCKED-BY 0x196960/0x1968b0 (retail inlines both here; their file is /Ob1 and was on
// this round's avoid list) and the enable reload's aggregate; earlier notes below.
// MiniMax B (11-19): no new form, no lower score (76): reading the enable byte at both use points instead of through
// the bool local gives 103 (the same form MiniMax G measured on 11-18) and a byte local gives 103, so the body is
// back as it was. What still differs is retail's reload of the enable byte after the counter store (the reload needs
// the enable byte and the counters to be reachable from one address: BLOCKED-BY include/globals.h and
// src/unknown_1967d0.cpp), retail's old maximum in ECX against this build's EAX, retail's `xor eax, eax; mov ax,
// [mem]` read of the fifteen-bit counter against this build's zero-extending word read, and retail's increment of
// `word ptr [eax + edx + 0x16]` before it forms the row pointer.
// MiniMax B (11-18): no new form, no lower score (76): this build still holds the enable byte in a bool local across the counter store where retail reloads it into CL, keeps the old maximum in EAX where retail keeps it in ECX, zero-extends the fifteen-bit counter where retail reads it with `xor eax, eax; mov ax, [mem]`, and forms the row pointer before the increment where retail increments `word ptr [eax + edx + 0x16]` first (85 when the increment comes first, where the prologue's kind and player homes swap); this round's forms: the comparison's operands swapped (76, the same code), the old maximum as a short (76, the read becomes sign-extending), the old maximum read as a nested if (101) and the permuter (started, stopped before it printed a result); the enable reload needs one object for the enable byte and the counters (include/globals.h and src/unknown_1967d0.cpp) and the clamp's store through the counter's address is a one-use pointer kept only with a match: BLOCKED-BY both.
// MiniMax G (11-18, second pass): no new form, no lower score (76): reading the enable byte at both use points gives
// 103 and at the later one only 103, a short old maximum gives 76 and the comparison's operands swapped gives 76, so
// this build still keeps the old maximum in EAX where retail keeps it in ECX and the counter in CX where retail keeps
// it in AX.
// MiniMax G (11-18): earlier 76 (was 106): the enable byte is read once into a bool local before the counter store (the
// maximum's test and the clamp's test then share one value) and the clamp's limits are taken as the table's own
// `word` type; what still differs is that retail reloads the enable byte after the counter store (into CL, and again
// into DL in the team block) where this build holds the local's value across that store (the reload needs the enable
// byte and the counters to be one object: BLOCKED-BY include/globals.h and src/unknown_1967d0.cpp), retail keeps the
// old maximum in ECX and the counter in AX and reads the fifteen-bit value with `xor eax, eax; mov ax, [mem]` where
// this build keeps the maximum in EAX and zero-extends the word, retail increments `word ptr [eax + edx + 0x16]` before
// it forms the row pointer, and retail's clamp store is the four-instruction bitfield idiom through the counter's
// address (a one-use pointer, kept only with a match); this round's other forms: the flag local as a byte (106), the
// flag compared with `!= 0` at each use (103), the local in one of the two tests only (103 each), the local also in
// the team block (97), the maximum's `&&` form (101), the team maximum's `&&` form (102), the clamp as a ternary (96),
// the counter through a pointer (86), the flat `all[index * 0x1b5 + 12]` indexing (76, the same) and the increment
// through the statistics address before the row pointer (85, where the prologue's kind and player homes swap).
// MiniMax I (11-17, run 103): no new form, no lower score (106): retail still keeps the old maximum in ECX and reads the
// fifteen-bit counter with the sixteen-bit idiom (xor eax, eax; mov ax, [mem]) where this build keeps the maximum in
// EAX and zero-extends the word; a short old maximum measures 106 and the nested enable test 111, and the enable byte's
// reload and the two maxima's homes stay in include/globals.h and src/unknown_1967d0.cpp: BLOCKED.
// MiniMax H (11-14): no new form, no lower score (106): this round re-read the diff: the -1 default still lands in EAX where retail holds it in ECX, and the 15-bit counter is still read with movzx where retail clears EAX and loads a word; the two counts' registers come from the callers in another file, so the body's spelling cannot move them.
// MiniMax G (11-14): no new form, no lower score (106): the enable reload is blocked by src/unknown_1967d0.cpp and the two maximum reads only duplicate 0x196960's body (105).
// MiniMax E (11-15): no new form, no lower score (106): the old maximum read as a nested if instead of the conditional expression 111 (the maximum stays in EAX against retail's ECX); retail keeps the enable byte in AL and the maximum in ECX, loads the counter's word with `xor eax,eax; mov ax,[...]` against our zero-extended load, increments `word ptr [eax+edx+0x16]` where we compute the row pointer first, and re-reads the enable byte into CL after the counter store where our build keeps it in DL across that store.
// MiniMax D (11-16): no new form, no lower score (106): this round re-read the retail diff against ours and the
// remaining delta is still the three homes and the counter read: retail keeps the old maximum in ECX and tests it with
// `cmp cx, ax`, loads the 15-bit counter with retail's `xor eax, eax; mov ax, [mem]` idiom where this build zero
// extends the word into EAX, increments `word ptr [eax + edx + 0x16]` and only then forms the row pointer where this
// build forms the row first, and re-reads the enable byte after the store where this build keeps it in DL across that
// store; the counter's type and the enable byte's home are in include/globals.h and the reload's aliasing in
// src/unknown_1967d0.cpp: BLOCKED-BY both.
// MiniMax R (11-17): no new form, no lower score (106): spelling the old maximum as a nested double assignment (the
// outer test, then the inner test) scores 108 and was restored; what still differs is that retail keeps the old
// maximum in ecx and reads the fifteen-bit counter with the sixteen-bit idiom (xor eax, eax; mov ax, [mem]) where this
// build keeps the maximum in eax and zero-extends the word, and retail reloads the enable byte after the counter store
// where this build holds it in dl across that store; the reload needs one object for the enable byte and the counters
// (include/globals.h and src/unknown_1967d0.cpp) and the maxima's homes follow the held helpers 0x196960 and 0x1968b0
// in src/unknown_1967d0.cpp: BLOCKED.
// MiniMax H (11-18): earlier 106 (was 106): no new form, no lower score; retail keeps the old maximum in ECX with
// the enable byte in AL and reads the 15-bit counter with the 16-bit idiom (`xor eax, eax` then a word load),
// this build keeps the maximum in EAX and the enable byte in DL; the enable reload needs the enable byte and
// the input counters in one object: BLOCKED-BY include/globals.h and src/unknown_1967d0.cpp. The permuter's best
// (80 against its 99) swaps the arguments of 0x196e60 and 0x1968b0 and adds a volatile pointer and constant-only
// locals, so it changes what the calls do and is dropped.
// Earlier 106 (was 106): no frame; enable observations and maximum/register homes differ.
// M3 lane 8 r165 (fast, plain where noted): both maxima and the clamp store through the flat `all[index * 0x1b5 + 12]`
// / `counters[0][team * 0x2d + 12]` indexing the two held helpers use 106/106/106 (same code); LEAD, match-only: the
// clamp's store through `s_input_counter *counter = &g_511bf4.groups[index][12];` formed before the clamp (the setter
// helper's own form) 70 plain (427 B): the counter's address is formed ahead of the clamp as in retail and the old
// maximum then takes retail's `xor eax, eax; mov ax` read, leaving the enable byte held in BL (retail reloads it) and
// the two maxima's NONE arms; on it the player maximum as a nested init/assign 100, the limits read before a nested
// index test 106. A one-use pointer: kept only with a match, which the blocked enable reload prevents.
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
// MiniMax S (11-14): no new form, no lower score (106): the enable flag's reload after the store still needs the aliasing that the held src/unknown_1967d0.cpp gets by taking &g_510cb0, so retail's EDX/EAX turnover stays unmatched here.
// MiniMax D (11-15): the two maxima and the clamp store now use the held helpers' own forms (0x196960's flat
// `g_511bf4.all[index * 0x1b5 + 12]` read and 0x1968b0's `s_input_counter *counter` store) 70 (was 106): the
// counter's address is formed ahead of the clamp as retail does, the old maximum takes retail's `xor eax, eax;
// mov ax` read and the 15-bit store matches retail's `mov cx, word ptr [esi]; xor cx, ax; and ecx, 0x7fff`.
// Still differ: the enable byte is held in BL where retail reloads it into AL, CL and DL (that reload needs the
// aliasing in src/unknown_1967d0.cpp, held), the old maximum's home is EAX against retail's ECX, and the row pointer
// is formed before the increment against retail's `inc word ptr [eax+edx+0x16]; lea edi, [eax+edx]`.
// MiniMax D (11-15): the old maximum now reads as an inverted ternary in its own block, which gives
// retail's `cmp esi, -1` against the copy in ECX 64 (was 70): the block keeps the outer home in ECX and
// the loaded value in EAX, so the copy to the outer survives. Still differ: the enable byte stays in BL
// where retail reloads it into AL, CL and DL (the reload needs the flag's aliasing, held in globals.h),
// the compare against the immediate where retail compares the register, and the row pointer is formed
// before the increment against retail's `inc word ptr [eax+edx+0x16]; lea edi, [eax+edx]`.
// NOT kept (coordinator review, MiniMax D 11-15): 106 -> 64 rested on match-only forms: a one-use current_maximum
// local copied into old_maximum, a one-use counter pointer for the store, and the typed groups[index][12] written
// as flattened all[index * 0x1b5 + 12] indexing.
// M3 lane 15 r95 (fast with 0x1968b0, 0x196960, 0x196e60, 0x196ef0, 0x1e9ce0, all MATCH; control 106): the clamp's
// limits and value as `short` locals 104 (426 bytes); the limits as `word` locals (the table's own type) 103 (426 bytes,
// under the keep gate); the clamp applied to `consecutive` itself with no value local 106; the limits read from the
// table at each use 116 (415 bytes); the maximum tested before the minimum 106. Still BLOCKED-BY 0x196960/0x1968b0
// (src/unknown_1967d0.cpp, held) and the enable byte's aggregate in include/globals.h.
// MiniMax C (11-17): no new form, no lower score (106): the counter's 15-bit read still uses movzx where retail
// zeroes the register and loads the word, the old maximum's home stays EAX where retail holds it in ECX, and the
// enable byte stays in DL where retail reloads it into AL, CL and DL; the flat all[] indexing that gives retail's
// read form is the match-only form the coordinator rejected, and the permuter's best (80) adds volatile, a swapped
// call argument and constant locals. The parameter layout is now known: LTCG gives this body player_index in EDX,
// statistics at +0x4, threshold at +0x8 and code at +0xc (three stack arguments, ret 0xc), with ECX carrying only
// the byte argument, so the remaining gap is codegen shape and not the convention: the old maximum nested into a
// value local with a word intermediate 108, the maximum read written as a conditional assignment inside the enable
// block 111, and the row addressed through a typed player pointer with the counter reached by its address 110 (the
// prologue's register allocation shifts and EDI and EBP swap). The flat all[] indexing that gives retail's read
// form stays a match-only form the coordinator rejected.
// M3 lane 1 r127 (fast with 0x1968b0/0x196960/0x196e60/0x196ef0/0x1e9ce0, all MATCH; control 76): the maxima and the
// player clamp as calls to the existing 0x196960/0x1968b0 with a `__forceinline` redeclaration of 0x196960 in this file
// 215 (350 B: LTCG keeps both as calls, so a redeclaration cannot inline a function of an /Ob1 file); the counter
// incremented and then read in a separate statement 76 (same code); the old maximum as `index == NONE ? NONE : ...` 79;
// `old_maximum` declared ahead of `enabled` 76 (other code); the clamp limits read ahead of a nested `index != NONE`
// test (0x1968b0's own shape) 76 (other code: the index jump moves below the two limit loads, as retail). Body unchanged.
// M3 lane 4 r225 (fast with the file's functions, src/unknown_1967d0.cpp's and the helpers' other callers; control 76,
// aligned 70). HELD lead (needs the owner's OK: one word in src/unknown_1967d0.cpp, another lane's file this round; full
// diff in build/r225/held-0x1ea5b0.patch): `inline` on 0x196960's own definition makes the link expand the real getter
// here, so no copy of it is needed, and every other function of both files and the callers keeps its state (0x1969a0
// still calls it). With it: this body with only the two maximum reads as `if (g_510ca0) x = function_196960(...)` 101
// (428 B, aligned 66); those reads with the increment written through the statistics address first, no `consecutive`
// or `team` local and the setter's own body (its `g_510ca0 && !g_510ca1` test, long limits, a counter pointer and the
// nested conditional clamp) 86 (426 B, plain 86; aligned 33, first difference +0xa1: retail's code to the reload of the
// enable byte, the one thing left, which needs the enable byte and the counters in one address-taken object); that with
// the setter as a plain call 158 (371 B: it stays out of line). `inline` on 0x1968b0's definition as well 111 (414 B)
// and costs matched 0x1ea770 (26), 0x1e9ce0 (179), 0x1e9df0 (129) and 0x15ca10 (46), which call it in retail: the
// setter must stay as it is. Diagnostic: a file-scope pointer to g_510ca0, set by an initialiser or by a store in an
// uncalled function, gives no reload in this build (86 both).
// Round 438: Enable reload and maximum transport remain separate from the statistics row shape. Test the existing short-row layout.
// Use the existing short row layout for both counters and the timestamp 86.
// Bind the actual fourteen-short player row as an array reference 86.
// Index the existing statistics table at every row use 85.
// Use the native row and bind the time field for the threshold test and store 86.
// Use the native row and bind the recent count through its update branches 86.
// Bind both updated fields of the native row in the final streak update 86.
// Compute the new recent count from the threshold and store it once 102.
// Index the statistics table directly and bind the time field locally only for its two uses 85.
// NOT kept: Direct table indexing reaches the retail extent at 85. All eight forms raise the score and move the first mismatch into the prologue. The original enable and maximum lifetimes remain.
// Exhaustive maximum branches in long/short/word 95/95/98,
// those branches with direct enable uses 100, exhaustive player and team
// maxima 99, nested enabled maximum 99, early clamp input promotion 99,
// and limits before the index check 95. The 434-byte forms still carry
// enable across the increment and leave the maximum in EAX.
// Separate clamp output in long/word form: exhaustive 86/108, lower
// default 85/113, upper default 95/121, lower-bound admission with nested
// upper comparison 86/109. NOT kept: all regress. The 434-byte long forms
// still keep enable across the statistics write and the old maximum in EAX.
// M3 lane 8 r207 (src/unknown_1967d0.cpp open again; fast with 0x196960, 0x1969a0, 0x1968b0, 0x1ea770, 0x1e9ce0,
// 0x1e9df0, 0x15ca10, all MATCH each run; control 76). Lane 4 r225's held lead re-measured: `inline` on 0x196960's
// definition with both maxima as `if (g_510ca0) x = function_196960(...)`, the increment through the statistics
// address first, the row formed after it and the setter's own clamp (long limits, counter pointer, nested conditional)
// 96 (420 B: the getter now expands as retail's, `mov eax, -1`, `xor eax, eax; mov ax`, old maximum in ECX; left: the
// enable byte held in BL where retail reloads it into CL after the increment and into DL in the team block, then the
// clamp's registers). One object for 0x510ca0..0x511000 (every global of that range folded in as a field, the old
// names as macros; a header trial, check --fast over the 47 users of these globals, all 46 others MATCH): main's body
// 76 (same code), the inline-getter form 86 (426 B, first difference +0xa1, the enable still in BL): an object whose
// address other files take does not make this function reload the enable byte; lane 12 r78's reload needs the
// counters in the same object, which brings in g_511000 (lane 10 r120: 0x196470 then loses its match). On the
// inline-getter form the setter's enable test read through a const address cast 86 (426 B: the reload appears right
// after the increment, in BL, and the team block reuses it), the team test too 86 (same code); match-only forms.
// MiniMax S (11-20), NOT kept: 72 (was 76), a 4-point gain, under the 8-point bar for a function above 30.
// Raw enable held as unsigned byte, signed byte, word or long 103/103/98/101.
// Either byte with a direct setter read 103/103; raw team byte added to the
// player byte, with a held or direct setter read, 103/103.
// NOT kept: all regress. The raw byte avoids Boolean materialization but the
// enable still remains live across the increment and the maximum stays in EAX.
// M3 lane 1 r149 (H2M fast; control 76): the enable byte read at each use with no bool local 103 (the second read is
// still CSE'd into DL after the counter store: no reload without aliasing); the old maximum as a short local 76 (same
// code); the player clamp as 0x1968b0's own shape (long limits, a counter pointer, nested conditional in the store) 96
// (aligned 54 -> 25: retail's clamp and bitfield store); that with the enable read at each use 70 (427 B, aligned 26; a
// gain of 6, under the bar: left the getter's `mov eax, -1` / `xor eax, eax; mov ax` shape, the increment's `inc word
// [eax+edx+0x16]; lea edi` order and the two enable reloads); on 70 the increment before the row 79, on 96 the increment
// first 100, on 96 the index test nested after the limits 81; the old maximum read under `enabled && index != NONE` 101;
// on 70 `inline` on 0x196960's definition with both maxima as `if (g_510ca0) x = function_196960(...)` 102 (423 B).
// On 70, the old maximum's read as the getter's own shape (`long value = NONE; if (index != NONE) value = ...;` then
// the copy) 100 (425 B), the team maximum so 94 (425 B), both 102 (423 B; aligned 22: the shape is retail's, the
// missing enable reloads shift the rest).
// M3 lane 15 r137 (H2M_FAST with 0x1ea770, 0x1e9ce0, 0x1e9df0, all MATCH; control 76 at 432 B, aligned exact/stack-
// masked/register-masked 79/54/21): the old maximum's read nested (`if (enabled) { if (index != NONE) ... }`) 101
// (427 B); the player clamp as one nested conditional expression 86 at retail's 434 B (aligned 67/38/21: the team block
// then schedules retail's `mov ecx, 0xffffffff` between its test and branch); the clamp test's first compare written
// `consecutive > (short)old_maximum` 76 and the bool as `!!g_510ca0` 76 (same code each); the increment through the
// statistics address with the row formed after it 85 (434 B, first +0x16), with the conditional clamp 91 (436 B); the
// increment and the read as two statements through the row 76 (same code). The enable byte stays in DL. Body unchanged.
// M3 lane 12 r182 (H2M_FAST with 0x1e9fa0, 0x1ea770, 0x1e9ce0, 0x1e9df0, all MATCH each run; control 76 at 432 B,
// aligned / register-masked 70 / 31): the team as a char local 104 (426 B); the row as a `short *` indexed by element
// (11, 12, 14) 76 (same code); the clamp limits read after the value local 101 (426 B); the old maximum as a word with
// a word NONE 77 (436 B); the streak update as one conditional store 90 (436 B); the team test nested in the engine
// test 76 (same code). The enable byte is still held in DL across the counter store (retail reloads it).
// M3 lane 3 r305 (H2M_FAST with 0x1ea770, 0x1e9ce0 and 0x1e9df0, MATCH each run; control 76). The one caller passes
// the engine's statistics block (0x1e9fa0, from the statistics getter), not the enable byte's object, so the reload
// still needs that object's address to escape somewhere, as listed above. Forms: the bool set from the byte without
// `!= 0` 76 (same code); the kind lookup after the index 80; the player pointer formed after the 0x1e9ce0 call 91 (434
// B); the team read ahead of the engine test 107 (422 B); the row formed by short-element indexing 76 (same code).
// Body unchanged.
// M3 lane 13 r154 (H2M_FAST with 0x1ea770, 0x1e9ce0, 0x1e9df0 and 0x1e9fa0, MATCH each run; control 76, aligned 70).
// A scan of retail's code for the enable byte's address as an immediate finds none: only 0x510cb0's address is formed
// (pushed at 0x1962f6, 0x196415, 0x196766, loaded at 0x195a4b, 0x195ba1 and others), so the reload after the counter
// store comes from the object the enable byte shares with that record, as listed above. On main's body: the old
// maximum in the getter's own shape (`long value = NONE; if (index != NONE) value = ...; old_maximum = value;`) under
// the enable test 99 (430 B), the team maximum so 96 (430 B), both 101 (428 B), both with the increment through the
// statistics address and the row formed after it 105 (430 B, first +0x16), the player one with that increment 103.
// Body unchanged (76).
// GA3 (fast with 0x1e9fa0, 0x1ea770, 0x1e9ce0, 0x1e9df0, MATCH each run; control 76): team_maximum as nested
// `if (g_510ca0) { if (team != NONE) ... }` (dropping redundant `?: NONE`) 100 (427 B); with limits loaded ahead of
// `index != NONE` 107 (420 B); baseline with nested clamp guard + long limits 102 (425 B); baseline without `consecutive`
// local + long limits 78 (431 B); increment-before-row + old_maximum-after-increment + long limits forms (keeping or
// dropping team `?: NONE`, direct `player->team`, `bool teams`, limits before `index != NONE`, ternary clamp, no
// `consecutive` local, both nested, `enabled` after increment, `row` after `old_maximum`, `consecutive` reused in team)
// 100..114 (420..435 B; the increment-first forms free EDX and emit retail's `xor eax, eax; mov ax` and `inc word
// [edx+eax+0x16]; lea`, but swap EBP/EDI at +0x16). Reading: 0x195a40 (which passes 0x510cb0's address to 0x321aae in
// retail) is still `todo`, explaining why lane 8 r207's 0x510ca0..0x511000 object did not escape in this LTCG image.
// Body unchanged (76).
// Existing player statistics record for consecutive/recent/timestamp in all seven subsets 85 each.
// Typed increment before binding the row 85. NOT kept: all reach 434 bytes but exchange the opening player and kind registers.
// The later enable reload and previous maximum home still differ.
// M3 lane 15 r146 (H2M_FAST with 0x1ea770, 0x1e9ce0, 0x1e9df0, all MATCH each run; control 76 at 432 B). Still
// BLOCKED-BY 0x196960/0x1968b0 (their /Ob1 file is another lane's this round). Lane 1 r149's 70 form (the enable read at
// each use, long limits, a counter pointer and the nested conditional store) re-measured 70 (427 B); the increment
// through the statistics address with the row formed after it, on main's bool body with long limits 83 (434 B), on the
// 70 form 79 (429 B); on the 70 form the counter pointer ahead of the limits 70, the store clamping a `value` copy 70
// (same code each), the index test nested inside the enable test after the limits 106 (425 B).
// @retail 0x1ea5b0
void function_1ea5b0(long player_index, s_statborg *statistics, long threshold, byte code)
{
	long kind = function_196ef0(code);
	long index = player_index & 0xffff;
	s_player_datum *player = ((s_player_datum *)g_4e8c24->data) + index;
	function_1e9ce0(player_index, 7, statistics, 2, 1, true);
	function_196e60(index, 0, kind, 1);
	if ((code & 0xc0) == 0x40) function_196e60(index, 6, kind, 1);
	bool enabled = g_510ca0 != 0;
	long old_maximum = NONE;
	if (enabled) old_maximum = index != NONE ? g_511bf4.groups[index][12].value : NONE;
	byte *row = (byte *)statistics + index * 0x1c;
	short consecutive = ++*(short *)(row + 0x16);
	if ((short)old_maximum < consecutive && enabled && !g_510ca1 && index != NONE)
	{
		word minimum = g_46ddc0[12].minimum;
		word maximum = g_46ddc0[12].maximum;
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
void function_197160(long player_index, long arg_1f407d_2, byte code);
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

/* whether a player index already appears among the first `count` entries of a list */
static inline bool player_list_contains(long const *players, long count, long player_index)
{
	for (long i = 0; i < count; ++i)
		if (player_index == players[i]) return true;
	return false;
}

/* a unit died: picks the player to credit (the heaviest recent damage record on the victim's object, else the
   reported player), awards the kill or the betrayal, then the assists: every other recent damager above 40% of the
   credited record's amount, and the driver of the vehicle the killer rides. The record pointer walks the history in
   both loops (retail keeps the history's address in one home for both); the driver field is read at each use. */
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
	bool consider_team = true;
	if (reported_player != NONE && function_1df560((short)victim_team, (short)reported_team))
		consider_team = false;
	short selected = NONE;
	real best = -FLT_MAX;
	s_damage_record *record;
	short i;
	for (i = 0, record = object->history; i < 4; ++i, ++record)
	{
		long player_index = record->player_index;
		if (player_index == NONE) continue;
		if (player_index == reported_player) reported_slot = i;
		else
		{
			if (!consider_team) continue;
			byte *player = record_pool_lookup(g_4e8c24, player_index);
			if (!player || !function_1df560((short)victim_team, (char)player[0xc0])) continue;
		}
		if (record->time > (dword)recent && record->amount > best)
		{
			selected = i;
			best = record->amount;
		}
	}
	if (selected == NONE) selected = reported_slot;
	bool hostile = false;
	real assist_threshold = 0.0f;
	long assist_count = 0, other_count = 0;
	if (selected != NONE)
	{
		s_damage_record const *selected_record = &object->history[selected];
		damage_code = object->damage_codes[selected];
		if (datum_get_inlined(g_4e8c24, selected_record->player_index))
		{
			killer = selected_record->player_index;
			assist_threshold = selected_record->amount * 0.4f;
		}
	}
	if (killer != NONE && killer == reported_player) damage_code = code;
	if (killer == NONE && datum_get_inlined(g_4e8c24, reported_player))
	{
		killer = reported_player;
		damage_code = code;
	}
	if (killer != NONE)
	{
		byte *player = g_4e8c24->data + (killer & 0xffff) * 0x21c;
		killer_team = (char)player[0xc0];
		if (function_1df560((short)victim_team, (char)player[0xc0]) && victim != NONE)
		{
			function_197160(killer, victim, damage_code);
			function_1ea5b0(killer, statistics, cutoff, damage_code);
		}
		else { ++*(short *)(player + 0x1c4); hostile = true; }
	}
	long assists[5], other_assists[5];
	if (assist_threshold > 0.0f && victim != NONE)
	{
		record = object->history;
		for (long i = 0; i < 4; ++i)
		{
			byte *player = datum_get_inlined(g_4e8c24, record->player_index);
			if (player && record->player_index != killer &&
				(record->player_index == reported_player || record->amount > assist_threshold))
			{
				if (function_1df560((short)victim_team, (char)player[0xc0])) assists[assist_count++] = record->player_index;
				else other_assists[other_count++] = record->player_index;
			}
			++record;
		}
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
				if ((1 << object_header_get(root)->type) & 3)
				{
					byte *root_unit = (byte *)object_get(root);
					if (*(long *)(root_unit + 0x248) != NONE && *(long *)(root_unit + 0x248) != killer_unit)
					{
						byte *related_unit = (byte *)object_get(*(long *)(root_unit + 0x248));
						if (*(long *)(related_unit + 0x13c) != NONE)
						{
							byte *other = g_4e8c24->data + (*(long *)(related_unit + 0x13c) & 0xffff) * 0x21c;
							if (function_1df560((short)victim_team, (char)other[0xc0]) && (dword)assist_count < 5)
								assists[assist_count++] = *(long *)(related_unit + 0x13c);
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
			long killer_absolute = killer & 0xffff;
			long victim_absolute = victim & 0xffff;
			function_196dd0(killer_absolute, 0, victim_absolute, 1);
			function_196dd0(victim_absolute, 1, killer_absolute, 1);
		}
	}
	for (long i = 0; i < assist_count; ++i)
	{
		if (!player_list_contains(assists, i, assists[i]))
		{
			function_1e9ce0(assists[i], 8, statistics, 6, 1, true);
			*(dword *)g_4e9ae8 |= 0x20;
		}
	}
	for (long i = 0; i < other_count; ++i)
	{
		if (!player_list_contains(other_assists, i, other_assists[i]))
			function_1ea880(victim, other_assists[i], (s_object_values *)statistics);
	}
	if (victim != NONE)
	{
		bool self = reported_player == victim;
		bool friendly = false;
		if (function_15eaf0() && victim_team != NONE && killer_team != NONE && victim_team == killer_team)
			friendly = true;
		function_1ea770(damage_code, victim, statistics, killer, self, friendly);
	}
}

// @retail 0x1e9ce0
void function_1e9ce0(long player_index, long counter, s_statborg *statistics,
	long field, long delta, bool by_team)
{
	long index = player_index & 0xffff;
	((s_object_values *)statistics)->values[index][field] = (short)PIN_126660(((s_object_values *)statistics)->values[index][field] + (short)delta, -30000, 30000);
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
	if (counter != NONE) function_1968b0(index, NONE, counter, ((s_object_values *)statistics)->values[index][field]);
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
