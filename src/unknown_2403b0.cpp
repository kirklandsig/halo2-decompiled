#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "unknown_19ec40.h"
#include "unknown_2420a0.h"
#include "bit_vector.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr

bool function_15b2f0();
long function_cbd50(long unit_index, short weapon_index);
short function_101280(long object_index);
bool function_138880();
bool function_138860();
void function_15fe70(long player_index);
bool function_244680(long team, long unit_index);
void __stdcall function_a7810(dword mask);
struct s_242ef1 { point3f const *field_0; };
long function_242ef0(s_242ef1 point, long team);
struct s_team_entry;
s_team_entry *function_15e410(short team);
void function_242ba0(long marker_index, long object_index, long player_index);
void function_197210(long player_index, long value14, long value10);
void function_242e00(long player_index, long team);
void function_242660(long object_index, bool flag);
void function_2430a0(long player_index, bool flag);

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static inline bool slot_flag_clear_2403b0(s_slot_table *table, long team)
{
	return !(table->flags[team] & 1);
}

// score 86 (was 86): the pool-data reload still needs the side-effect summary of 0xcbd50/0x101280
// (other files), and MSVC always loads the limit product's cap global before the scale global whatever
// order the source writes them in; the flag test still uses `not dl; test dl, 1` where retail uses
// `not al; and al, 1`, and the team is still copied into edx where retail uses ecx.
// Earlier (95 -> 86): the object-pool data pointer is still spilled across 0xcbd50/0x101280 where
// retail reloads it, and the limit product loads its two globals in the opposite order; the
// completion-bit stores and the team's re-extension now match.
// Earlier (95, was 95): the object-pool data pointer is spilled across 0xcbd50/0x101280 where
// retail reloads it, and the completion bits are set through AL where retail uses DL and CL;
// the frame 0x3c agrees.
// Frame 0x3c agrees; pool-data spill and scratch-register ties remain.
// (Merged from a second score line, 95 (was 95):) frame 0x3c agrees; pool-data spill and scratch-register ties remain.
// Earlier: frame 0x3c agrees; the object-pool data pointer is cached where retail reloads
// frame 0x3c agrees; the object-pool data pointer is spilled across 0xcbd50/0x101280
// frame 0x3c agrees; the object-pool data pointer is cached where retail reloads
// it, the completion bits use other registers, and the final predicate's layout differs. Kept: the two
// completion bits set through byte_flags_set (retail stores the flag byte twice), 190 -> 146.
// frame 0x3c agrees; the object-pool data pointer is spilled across 0xcbd50/0x101280
// where retail reloads it (a forced reload, reverted at once, gives 72), the virtual-call setup and the
// completion bits use other scratch registers, the team keeps `mov esi, edi` where retail re-extends
// (`movsx esi, di`: only a long team cast back to short at the 0x242e00 call gives it; 0x242e00's
// other caller 0x242660 pushes its team unextended) and the clear-flag predicate is tested where
// retail materializes it (`not al; and al, 1`). Kept: the two completion bits through byte_flags_set
// 190 -> 146; the blocked call as the else arm 146 -> 106; the decay and the capped increment as
// MAX/MIN expressions 106 -> 95 (MAX alone 105).
// Tried on the 190 body: scoring team word 187, long 214; blocked long/byte 190/190; existing flag helper
// force-inline 190; returned point 199; marker bits 190; option scope 190; read-only unit/type result
// 190/190; scoring team at call 226; completion event prepared first 193; marker/slot entry lifetimes
// 190/190; success-result predicate 193; type result plus event preparation 193; read-only helper input
// 190; table reference 190; native team parameter 190; result-first predicate 192; flag byte sample 190;
// direct capped result 190; cap and table reference 190; option guard order 190; native player record
// 190; scoring-team field binding 190, both 190; increment counter binding 202; decrement binding 190;
// both counters 202; marker-team value 194; native player with both counters 202; predicate
// long/short/byte/signed-byte returns 189/190/190/190; byte predicate input 190, with long/short/byte
// results 189/190/190; native unit/weapon/both header references 190 each; external carried-index,
// selected-slot or both field bindings 190 each; initializer return used directly 311; external options
// record reference 190; permission result 190; fallback permission, present-unit, eligible-weapon,
// present-slot, present-marker and unit/weapon scopes 190 each; option once 190; negative-three
// addition 190; default-NONE/explicit-arm team results 191/190; reversed completion bit order 190;
// in-place limit 190; two-use stored-short ticks 190; limit plus stored ticks 190; default-NONE team plus
// negative-three addition 191; fresh player/unit/slot reads 254/190/271; player-unit/player-slot/
// unit-slot/all three 254/329/271/329; unit-index field at each use 195; team-first existing predicate
// with original/const/native-team/both inputs 190 each, long/short/byte/signed-byte results
// 189/190/190/190; event initialization between the completion bits 183; initialize/first bit/cause/
// second bit 163; first bit/initialize/cause/second bit 183; both preparation steps first 193; separate
// event buffers 196/160/183/193 (163/160 grow the frame to 0x60); native flag-array and flag-element
// predicate inputs with logical/complement extraction: bool 190, long 189; separate noinline
// declarations of 0x101280, 0x15b2f0, 0x138880, 0x138860, 0x15fe70, 0x244680, 0x197210 and 0x242660:
// 190 each.
// Native flag-bit object by value/pointer with bool/byte/short/long results 190/190/190/192
// each; complete slot-table owner, mutable/const first-consumer captures 200/200, entry captures
// 245/245, long predicate on those 199/199/244/244; plain descending notification-mask loop 187, with
// completion initialization between bits/preparation before the second bit/initialization before both
// 240/251/240, independent event buffers 193; complete eligibility bool/byte/short/long 190 each;
// default-false reader-gate results 190 each; common score result and one publication (eight forms)
// 239; slot/limit/ticks/marker/unit/weapon home names 190; complete entry carried-unit value 221, with
// short slot/clock-first product/both 239/221/239; word teams alone/slot/product/all 207/223/207/223.
// Round 146, separate byte-store graph: table-first explicit-bool/byte/short/long predicates
// 146/146/146/149; team-first bool/byte/short/long 146/146/146/149. Original helper/body restored.
// On the 146 body: limit product operands swapped 106 (no change); short slot 236; `+ -3` 106 (no
// change); event initialization before both bits 109. On the 95 body: permission as `allowed = false;
// if (0x15e410(team)) allowed = clear;` 128; engine call through game_engine_get 95; slot read without
// the weapon pointer 95; position/event declarations swapped 95; signed-char predicate team 95.
// Round 211: on the repaired MIN/MAX graph, twice-used increment/decrement inputs, native short
// clamp output and their seven subsets 95 each; complete graph with entry result declaration 95.
// Original input observations, result lifetime and body restored.
// Round 62 (95 body): unit through the existing unit_weapon_view_get 95; predicate as `== 0` 95;
// blocked as one value expression 95; second event initialized before both bits 107; both events
// through the out-of-line initializer nested in the cause call 284 (not inlined).
// Round 77: unit/weapon/both through object_get 95/95/95 (the pool read stays shared: both callees
// between them only read memory in this build, so the reload looks like a callee-summary effect);
// the entry guard split in two 95; the team as a long 119.
// Native cause-setter/event-sender/notification noinline boundaries, all seven
// subsets 95; native marker-query noinline boundary 95. Existing declarations,
// helper bodies, pool observations, two stores and complete scoring graph restored.
// Native type-reader/team-predicate/marker-query inline declarations in seven subsets
// 95 each; all three with byte admission result 95. All outside declarations restored;
// the pool-data spill remains on the repaired MIN/MAX and separate-byte-store graph.
// Round 190 on the recovered graph: native completion read 95; native completion stores 187;
// read/stores 187; native helper admission read 95; completion/admission reads 95;
// stores/admission read 187; complete native flag graph 187; complete graph with short slot
// 186.
// Round 289: Terminal query completion/non-special-first/blocked completion subsets; full graph with final team result arms:
// [95, 111, 111, 131, 131, 131, 131, 130]. All executable source and temporary types restored.
// Complete object-data, object-pointer and object-index ownership: seven subsets 95 each;
// complete graph with native short slot 237/922. Original pool observations, identifiers
// and pointer lifetimes restored.
// Round 79: nested blocked assignment 93; default-true admission 128; peer receiver 95;
// flag pointer arithmetic 95; clock-first product 95; final-team assignment 121; positive blocked
// assignment 132; nested blocked with peer 93. Two-point gains and one-use receiver forms restored.
// Round 42 (lane 16): weapon checks as one positive block setting the slot 95 (identical code: the
// pool read is shared whatever the control flow; retail's 0x101690 keeps the same pool pointer in ESI
// across 0xcbd50, so the callee summary is not the cause); permission as `if (!0x15e410(team)) blocked =
// true; else blocked = !clear;` 93 (blocked materialized in a stack byte; retail materializes `~flags & 1`
// with `not al; and al, 1` and branches on it); with the predicate spelled `(~flags & 1) != 0` 93; the
// arms swapped 131; the blocked flag dropped (call and return inside the test) 131.
// Round 315: Seven joint positive unit/weapon/slot admission graphs 95 each; full graph with
// independent completion event 116 at 912/912. Original resource, score and event lifetimes restored;
// pool-data spill and scratch-register ties remain.
// Round 53 (lane 9, fast): the spilled pool pointer is a callee-summary effect after all: a diagnostic
// global store inside 0x101280 (reverted) makes this body reload the pool after that call as retail does,
// 95 -> 53, with 0x101280 itself then 55 bytes. Byte-identical spellings of 0x101280 keep it pure here:
// `inline` on its definition 95; the object read through an out-parameter helper 95; its file linked after
// this one 95 (all three callers of 0x101280 still match); one pool-data local for both lookups 95.
// BLOCKED-BY 0x101280's summary.
// Round 26 (lane 14, fast with 0x101280 and its other callers): retail reloads the pool data right
// after the 0x101280 call (its other callers 0xa8040/0x241520 don't use the pool after it); `inline` on
// 0x101280's definition 95 (callers unchanged); completion bits in reverse order 95; the limit branch
// falling through to the shared `function_a7810(0x100); return;` tail (no second 0x100 call) 90
// (lead: the completion-bit block then matches, aligned 47 -> 38; five short of the keep bar); `ticks ==
// limit` without the short cast 188. On that 90 form: clock-first limit product 90; complement predicate
// `(bool)(~flags & 1)` 90; the flag test written in the caller 126; a long team 108; unit and weapon
// through the existing object_get 90. Diagnostic: a store through a pointer added to 0x101280 (so
// callers see it as writing memory) gives retail's reload here, 53; retail's 0x101280 bytes only read,
// and retail's 0x241520 keeps g_4e6948 in EBP across the same call, so retail treats it as writing
// pointed-to memory but no globals (BLOCKED-BY 0x101280's side-effect summary, cause unknown).
// Round 23 (lane 17, fast): the +0x10 first difference is a branch target; the first real one is the
// pool-data load (ours early, kept in a stack slot). The slot-clear helper reading the flag byte into a
// local 86; as `(flags & 1) == 0` 86; as `((flags >> 0) & 1) == 0` 86; the limit's tick-rate factor in a
// named local first 86; the seconds in a named local first 86. Diagnostic: 1-7 dead long temps 86 each.
// @retail 0x2403b0
void __stdcall function_2403b0(long player_index)
{
	s_event event;
	point3f position;
	if (g_4e6948->mode == 4 || !function_15b2f0())
		return;
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	long unit_index = *(long *)(player + 0x2c);
	if (unit_index == NONE)
		return;
	byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
	long weapon_index = function_cbd50(unit_index, *(signed char *)(unit + 0x212));
	if (*(short *)(unit + 0x1fc) != NONE || weapon_index == NONE || (short)weapon_index == NONE ||
		function_101280(weapon_index) != 1 || *((byte *)g_4e6948 + 0x1128))
		return;
	byte *weapon = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
	long slot = *(short *)(weapon + 0x17e);
	if (slot == NONE)
		return;
	if (!g_55e4d0[g_4e9ae8->engine_index]->p35(player_index, 1) && !function_138880())
		function_15fe70(player_index);
	if (g_4e6948->mode_180 == 9 && !(g_51ec80->flags[slot] & 8))
	{
		signed char team = *(signed char *)(player + 0xc0);
		if (team == NONE)
			return;
		if (function_244680(team, *(long *)(player + 0x2c)))
		{
			if (g_51ec80->e[slot] == NONE)
			{
				game_engine_event_initialize_inline(&event, 10, 14);
				game_engine_event_set_cause_player(player_index, &event);
				function_19eb90(&event);
			}
			long limit = (short)*(short *)((byte *)g_4e6948 + 0x244) * g_510c54->field_2_3;
			long ticks = MIN(g_51ec80->e[slot] + 1, limit);
			g_51ec80->e[slot] = (short)ticks;
			if ((short)ticks == limit)
			{
				byte_flags_set(&g_51ec80->flags[slot], 3, true);
				byte_flags_set(&g_51ec80->flags[slot], 0, true);
				game_engine_event_initialize_inline(&event, 10, 15);
				game_engine_event_set_cause_player(player_index, &event);
				function_19eb90(&event);
				function_a7810(0x200);
			}
		}
		else
			g_51ec80->e[slot] = (short)MAX(g_51ec80->e[slot] - 3, NONE);
		function_a7810(0x100);
		return;
	}
	function_b9dd0(*(long *)(player + 0x2c), &position);
	s_242ef1 point = { &position };
	long marker = function_242ef0(point, *(signed char *)(player + 0xc0));
	if (marker == NONE)
		return;
	bool blocked = false;
	if ((g_4e6948->flags22c & 8) && *(long *)((byte *)g_4e6948 + 0x240) != 1 && *(long *)((byte *)g_4e6948 + 0x240) != 2 && g_4e6948->mode_180 != 9)
	{
		signed char team = *(signed char *)(player + 0xc0);
		if (!function_15e410(team) || !slot_flag_clear_2403b0(g_51ec80, team))
			blocked = true;
	}
	if (!blocked)
	{
		if (function_138860())
		{
			if (g_4e6948->mode_180 == 9)
				function_242ba0(marker, weapon_index, player_index);
			else
			{
				long team = (short)slot == 8 ? NONE : (short)slot;
				function_197210(player_index, 1, slot);
				function_242e00(player_index, (short)team);
				function_242660(weapon_index, false);
			}
		}
	}
	else
		function_2430a0(player_index, true);
}
