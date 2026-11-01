// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_058dd0.h"
#include "unknown_0662e0.h"
#include "globals.h"
#include "unknown_08b110.h"
#include <xtl.h>
#include <string.h>

struct s_surface_description;
struct s_matchmaking_ratings;
struct s_match_player_list;
struct s_session_property_value
{
	long kind;
	long value04;
	long value08;
	long value0c;
	long value10;
	long value14;
	long value18;
};
struct s_71730
{
	long field_0[3];
	long field_c;
	long field_10;
	byte field_14[0xc0];
	long field_d4[16];
	long field_114[16];
	long field_154[16];
	long field_194[16];
	byte field_1d4[0x358 - 0x1d4];
	long field_358;
	dword field_35c;
	dword field_360;
};

void function_71e00(c_session_state *arg_0);
s_surface_description *function_192e60(long arg_0);
long function_193400(s_surface_description *arg_0);
long function_1931d0(s_surface_description *arg_0, long arg_1);
bool function_1934b0(s_surface_description *arg_0);
long function_1933a0(s_surface_description *arg_0);
long function_1933d0(s_surface_description *arg_0);
bool function_1934d0(s_surface_description *arg_0);
long function_193370(s_surface_description *arg_0);
long function_193440(s_surface_description *arg_0);
bool function_193470(s_surface_description *arg_0);
bool function_7e210(c_class_58d20 *arg_0, s_matchmaking_ratings *arg_1);
bool network_session_players_match(c_class_58d20 *arg_0, c_class_58d20 *arg_1);
long network_session_get_maximum_players(c_class_58d20 *arg_0);
long network_session_find_player(c_class_58d20 *arg_0, const dword *arg_1);
bool network_session_members_ready(c_class_58d20 *arg_0, dword *arg_1);
bool function_73ae0(unsigned __int64 *arg_0);
bool network_session_host_set_id49f0(c_class_58d20 *arg_0, const s_session_id *arg_1);
bool network_session_host_set_data49cc(c_class_58d20 *arg_0, const dword *arg_1);
void session_property_value_set_kind2(s_session_property_value *arg_0, long arg_1);
void session_property_value_set_kind3(s_session_property_value *arg_0, long arg_1);
bool function_72140(c_session_state_matchmaking *arg_0);
bool balance_teams_by_count(long arg_0, long const *arg_1, long arg_2, long arg_3,
	bool arg_4, long arg_5, long arg_6, bool arg_7, long *arg_8);
bool balance_teams(long arg_0, long arg_1, bool arg_2, long arg_3, long arg_4,
	bool arg_5, long arg_6, long const *arg_7, long const *arg_8, bool arg_9, long *arg_10);
void function_71ff0(c_session_state_matchmaking *arg_0, long arg_1,
	const s_match_player_list *arg_2, const long *arg_3);

// score 439 (was 439): retail length; frame 0x3e8 versus 0x3e4, result byte and secondary-state/getter homes remain.
// Earlier: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// Round 30 (lane D): the permuter found no better form, so the body is unchanged. What still differs:
// retail calls 0x5b080 where this body expands it and expands the elapsed-time helpers 0x75870/0x75890
// where this body calls them, retail's frame is 0x3e4 against our 0x3e8, and the result byte and the
// getter/readiness handling are still the ones the rows below describe.
// Earlier at 439: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// retail length; frame 0x3e8 versus 0x3e4, result byte and secondary-state/getter homes remain.
// Earlier: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// Retail stores the tested session state to a dead stack slot before each early exit (the idiom
// 0x706f0 and 0x5c490 reproduce only with volatile locals), so every later row is shifted in the
// lockstep compare. Round 152, nested rewrite without gotos (existing value getter, live-state
// helper, ready = player match then cleared by each failing test, deadline block skipped on the
// success path): calling the /Ob1 time helpers 512; with file-local time copies and a noinline
// data getter 468 (frame, prologue and entry registers now retail's; 353 of 463 retail rows align
// against 268 here); mode-7 arm before a re-tested guard 470; owner progress pointer first 492;
// force-inline time copies 468. All restored.
// Refreshed first balance inputs 454; fallback balance inputs 466; both phases 513;
// native state-value getter 449; final transition-result union 449.
// frame 0x3e8 versus 0x3e4, result-byte home and getter boundary differ;
// output-scalar lifetime 449; type/first-state/second-state scopes 449; inline depth one 449; the five
// settings read into one-use locals first 443 (not kept: the locals only fix the argument order).
// Existing property setter 475; selected value result 451; unsigned reset/balance 449/449;
// balance test outcomes 449; readiness at outcome 450; reset at outcome 481; unsigned found count 449.
// Ratings count, group-size cursor, player-rating cursor, combined ratings inputs, readiness-output
// lifetime, native identifier, paired outputs and combined inputs/outputs all 449.
// player scan while 449; assignment while 449; paired counted whiles 449; capacity admission scopes 449; roster
// admission stages 449; countdown expression homes 449; valid input work scope 449; future deadline first 449.
// Native producer ratings type 449; signed reset/balance/both decision bytes 464/461/483; signed
// transition/advance bytes 468/449; native property clear 454; native ratings plus reset byte 464.
// All forms restored; frame 0x3e8 versus 0x3e4, secondary-state/getter boundary and result home
// remain.
// Complemented final/reset/both decisions 489/467/489; waiting transition decision 439 kept;
// with complemented final/reset/both 492/469/498; complemented balance 446, restored.
// On the waiting transition graph: reset bool/byte 469/469, short/word 442/442;
// balance bool/byte 458/458; paired bool/byte 476/476. All eight widths restored.
// Complementary waiting carrier signed byte/byte 439/439, short/word 450/450, long/dword 454/454; paired
// waiting/final short 450, paired long 443. All restored.
// Round 187 advance result carried through roster/count/member-readiness test outcomes,
// each nonempty combination 459; full combination with native reset byte 465.
// Exact-match-only decision forms; all restored to the complementary waiting graph.
// Existing lookup/ratings/transition inline declarations, all three pairs and all three
// 439 each; all three noinline declarations 439. Original declarations and body restored.
// Consistent live primary/secondary/both/owner receivers 462/463/468/470; primary,
// secondary or both with live owner 461/456/466; all with direct state tests 466.
// All restored; three combined forms report reduced-image callee caveats.
// retail length; frame 1000 versus 996, result byte home and second state/getter
// handling differ. Retail calls 0x5b080 (ours expands it), expands the elapsed-time helpers 0x75870/
// 0x75890 (the start time is loaded before the clock is read), stores each session state to the stack
// before the early exits, and keeps a ready byte at [esp+0x11] (from the player match, cleared by each
// failed check, later from 0x72140) and the reset flag as a byte at [esp+0x12].
// Refreshed first balance inputs 454; fallback balance inputs 466; both phases 513; native state-value
// getter 449; final transition-result union 449; output-scalar lifetime 449; type/first-state/
// second-state scopes 449; inline depth one 449; the five settings read into one-use locals first 443
// (not kept: the locals only fix the argument order); existing property setter 475; selected value
// result 451; unsigned reset/balance 449/449; balance test outcomes 449; readiness at outcome 450;
// reset at outcome 481; unsigned found count 449; ratings count, group-size cursor, player-rating
// cursor, combined ratings inputs, readiness-output lifetime, native identifier, paired outputs and
// combined inputs/outputs all 449; player scan while 449; assignment while 449; paired counted whiles
// 449; capacity admission scopes 449; roster admission stages 449; countdown expression homes 449;
// valid input work scope 449; future deadline first 449; native producer ratings type 449; signed
// reset/balance/both decision bytes 464/461/483; signed transition/advance bytes 468/449; native
// property clear 454; native ratings plus reset byte 464; complemented final/reset/both decisions
// 489/467/489; waiting transition decision 439 kept; with complemented final/reset/both 492/469/498;
// complemented balance 446; on the waiting transition graph reset bool/byte 469/469, short/word
// 442/442, balance bool/byte 458/458, paired bool/byte 476/476; complementary waiting carrier signed
// byte/byte 439/439, short/word 450/450, long/dword 454/454; paired waiting/final short 450, paired
// long 443.
// Round 93: noinline class declaration of 0x5b080 467 (frame 0x3e4; entry registers move); the state
// tests as switches 470 (range compares); the three time reads through file-local inline copies of
// 0x75870/0x75890 (as network_session_membership.cpp has them) 448 (start time loaded first, as
// retail); those with the noinline getter 486 (entry registers and frame now retail's, 1549 bytes);
// plus the reset flag as bool 495.
// Round 190: existing time helpers at first elapsed/growth stamp/both 494/494/508;
// second elapsed/first-and-second/growth-and-second/all 496/503/510/520;
// all with private force-inline declarations 520. Original timing expressions and declarations restored.
// Round 97: the existing value getter for the session index 439; noinline getter 467; with a
// file-local time-since helper 486 (1549 bytes: retail also clears a ready byte in every failed
// check); a ready byte from the player-match chain, cleared in each failure arm and set from
// 0x72140, 448 (1580 bytes); that with the noinline getter and time-since helper 461 (1573 bytes,
// frame, entry and failure stores now retail's; left: the property value and both flag bytes
// cleared before the player-count branch, a separate mode-7 exit per missing pointer).
// Round 369: ready/getter interaction 504; byte reset 511; early property 502; property/reset 511; separate missing-input exits 488, with reset/property/both 493/501/493. Match-only carrier forms, header and body restored.
// Round 235: ratings/assignment/admission consumption scopes, all seven combinations 439 each; readiness-output consumption scope 439.
// Round 201: signed/unsigned outstanding roster counts 463/463; missing counts 447/447;
// direct lookup-result accumulation 469/469; bool/byte all-found results 443/443 (exact-only).
// Original complete lookup count and signed zero/negative admission restored.
// Round 226: Property/balance defaults before count observation 457/439; both 457; each/both
// with Boolean reset 472/469/472; property with Boolean balance 462; all with both Booleans 479. All restored.
// Round 409: roster count at every use 439; actual identity-end traversal with captured/live count
// 450/450; those three with the shared live-state predicate 484/480/480; field-count/end
// traversal with native long final result 444/468. Original complete body restored.
// Round 189: plain strict/fallback balance loop, long ordinal with short/bool/long/byte
// result 580/591/588/591; short ordinal with those four results 580 each. Original
// attempts restored; the rolled loop shortens away from retail and leaves the frame excess.
// Round 214: final canonical 0/1 result storage on the complementary waiting graph:
// signed-byte/byte 465 each; short/word 449 each; int/unsigned-int/long/dword 444 each.
// Original bool result restored; frame 0x3e8/0x3e4, result/state homes and getter boundary remain.
// Round 431: Final result from unchanged transition/reset tests, bool/byte/short/long carriers,
// combined tail then per-phase publication: 449/455/484/480/441/461/476/471. Original conditional true
// stores, result carrier and complete executable body restored.
// Round 441: Deadline expiry owns guard/reset result; bool expiry with long/bool/short/byte reset 469/450/469/450; long expiry 463/447/463/447. Long expiry plus byte/bool reset recovers frame 0x3e4 but regresses; all original computations and body restored.
// Round 449: primary/secondary/both entry states read at each equality
// use 439 each; with secondary receiver declared after first guard 457
// each; primary/both live states with conditional index producer 467 each.
// Original snapshots, receiver lifetime and index producer restored; frame,
// dead state stores and getter boundary remain.
// Round 247: distinct strict/fallback imbalance outputs with phase/entry cost and
// readiness storage, 439/439/439/439; the same four with identifier storage at entry
// 442/442/442/442. Original shared output, declarations and entire body restored.
// Round 259: persistent settings across all four solver uses: team count, capacity, party
// flag, minimum, maximum, capacity pair, party triple and all five,
// 467/474/451/455/455/499/472/455. Original queries, receivers and full body restored.
// Round 478: complete strict solver predicate with short/bool/byte/long result
// 455/458/458/457; both strict and fallback predicates 458/458/458/458.
// All original producers and body restored; frame 0x3e8 versus 0x3e4, state/result homes and getter boundary remain.
// Round 53 (m3 lane 1; kept: the three clock reads through the existing header time_now() and the
// variant index through the existing get_value_49c8(), byte-identical 439, in place of their bodies
// written out): the property value cleared before the player-count test 457 (1577 bytes); the first
// start time read into a local before the clock 439 (aligned diff 155 from 162: retail's load order;
// one-use, not kept); the reset flag as bool 469 (frame 0x3e4). Retail's 0x5b080 call is why ours
// keeps the session state in ESI after the getter call (the inlined body re-tests it).
// Round 259: Entry uninitialized reset/waiting/balance storage, seven subsets, then all with Boolean reset
// 439/439/439/439/439/439/439/469. Original body and producer sites restored.
// Round 478: complete strict solver predicate with short/bool/byte/long result
// 455/458/458/457; both strict and fallback predicates 458/458/458/458.
// All original producers and body restored; frame 0x3e8 versus 0x3e4, state/result homes and getter boundary remain.
// DH round 264: On the complementary waiting graph, native producer record/existing kind-four setter/
// both/native long balance/record-balance/setter-balance/all/all with initial memset
// 439/477/477/454/454/477/477/486. Original record, property construction, short balance
// and complete body restored; frame/result/state/getter homes remain.
// Round 282: Boolean reset with result/completed, reset/reset_required and
// transition_pending/waiting name combinations, eight at 469/1562. Frame 0x3e4 agrees
// but result remains at +0xe; original long reset, names and entire body restored.
// Round 283: positive type/primary-state/secondary-state admission gates, all seven subsets, each 439;
// all gates with delayed secondary receiver 457. Original complementary waiting graph, receivers and
// whole body restored; frame, state stores and getter boundary remain.
// Round 217: actual assignment output cursor, long/int/short/word ordinal at phase/entry scope,
// 441/441/443/443/441/441/443/443. Original indexed producer and complete body restored.
// Round 259: Entry uninitialized reset/waiting/balance storage, seven subsets, then all with Boolean reset
// 439/439/439/439/439/439/439/469. Original body and producer sites restored.
// DH round 264: On the complementary waiting graph, native producer record/existing kind-four setter/
// both/native long balance/record-balance/setter-balance/all/all with initial memset
// 439/477/477/454/454/477/477/486. Original record, property construction, short balance
// and complete body restored; frame/result/state/getter homes remain.
// @retail 0x71730
bool __stdcall function_71730(c_session_state_matchmaking *arg_0)
{
	bool local_3 = false;
	{
		s_session_owner *local_0 = arg_0->owner;
		c_class_58d20 *local_1 = local_0->session_a;
		c_class_58d20 *local_2 = local_0->session_b;
		if (local_1->type != 11)
			goto finished;
		long local_4 = local_1->state;
		if (local_4 != 5 && local_4 != 6 && local_4 != 7 && local_4 != 8)
		{
			function_71e00(arg_0);
			goto finished;
		}
		local_4 = local_2->state;
		if (local_4 != 5 && local_4 != 6 && local_4 != 7 && local_4 != 8)
		{
			arg_0->mode = 14;
			local_3 = true;
			goto finished;
		}
		long local_7 = local_2->get_value_49c8();
		s_surface_description *local_8 = function_192e60(local_7);
		byte *local_9 = local_2->get_data_4a00();
		long local_10 = time_now() - arg_0->unknowna68;
		byte *local_11 = *(byte **)((byte *)local_0 + 0x3c);
		long local_12 = false;
		bool wait_for_transition = true;
		long local_15[16];
		s_71730 local_16;
		if ((dword)local_2->player_count > *(dword *)((byte *)arg_0 + 0xa6c))
		{
			*(dword *)((byte *)arg_0 + 0xa6c) = local_2->player_count;
			*(long *)((byte *)arg_0 + 0xa70) = time_now();
		}
		long local_17 = time_now() - *(long *)((byte *)arg_0 + 0xa70);
		s_session_property_value local_14 = {0};
		if (!local_8 || !local_9)
		{
			arg_0->mode = 7;
			local_3 = true;
			goto local_28;
		}
		if (!function_7e210(local_2, (s_matchmaking_ratings *)&local_16))
			goto local_28;
		if (local_16.field_358 > function_193400(local_8) &&
			*(long *)(local_9 + 0x184) == network_session_get_maximum_players(local_2))
			goto local_28;
		if (*(long *)(local_11 + 8) == 0 &&
			(local_10 > g_network_configuration.value1c0[9] * 1000 ||
			 *(long *)(local_11 + 0x10) > g_network_configuration.value1c0[8]))
			*(long *)(local_11 + 8) = 2;
		if (!network_session_players_match(local_1, local_2) || local_16.field_360 ||
			local_2->player_count != *(long *)(local_9 + 0x184))
			goto local_29;
		{
			long local_18 = *(long *)(local_9 + 0x184);
			long local_19 = 0;
			for (long local_20 = 0; local_20 < local_18; local_20++)
				if (network_session_find_player(local_2, (const dword *)(local_9 + 0x188 + local_20 * 12)) != NONE)
					local_19++;
			if (local_19 != local_18)
				goto local_29;
			long local_21 = function_1931d0(local_8, local_2->player_count);
			if (local_21 == 0x7fffffff || local_21 > local_17)
			{
				session_property_value_set_kind3(&local_14, (local_21 - local_17) / 1000);
				goto local_29;
			}
			if (function_1934b0(local_8) && local_16.field_358 > function_193400(local_8))
				goto local_29;
			dword local_22;
			if (!network_session_members_ready(local_2, &local_22))
			{
				memset(&local_14, 0, sizeof(local_14));
				local_14.kind = 4;
				goto local_29;
			}
			if (!(local_2->state > 2 && local_2->state <= 8 && local_2->flag49e8))
			{
				s_session_id local_23;
				function_73ae0((unsigned __int64 *)&local_23);
				network_session_host_set_id49f0(local_2, &local_23);
			}
			if (*(long *)(local_11 + 8) != 1)
				*(long *)(local_11 + 8) = 1;
			wait_for_transition = !function_72140(arg_0);
			if (wait_for_transition)
				goto local_30;
			if (function_1934b0(local_8))
			{
				long local_24;
				short local_25 = false;
				if (balance_teams_by_count(local_16.field_c, local_16.field_194,
					function_193440(local_8), function_193370(local_8), function_1934d0(local_8),
					function_1933d0(local_8), function_1933a0(local_8), true, &local_24) &&
					local_24 <= function_193400(local_8))
					local_25 = balance_teams(function_193440(local_8), function_193370(local_8),
						function_1934d0(local_8), function_1933d0(local_8), function_1933a0(local_8),
						true, local_16.field_c, local_16.field_194, local_16.field_d4, true, local_15);
				if (!local_25 && !function_193470(local_8))
				{
					if (balance_teams_by_count(local_16.field_c, local_16.field_194,
						function_193440(local_8), function_193370(local_8), function_1934d0(local_8),
						function_1933d0(local_8), function_1933a0(local_8), false, &local_24) &&
						local_24 <= function_193400(local_8))
						local_25 = balance_teams(function_193440(local_8), function_193370(local_8),
							function_1934d0(local_8), function_1933d0(local_8), function_1933a0(local_8),
							false, local_16.field_c, local_16.field_194, local_16.field_d4, true, local_15);
				}
				if (!local_25)
				{
					wait_for_transition = true;
					local_12 = true;
					goto local_30;
				}
			}
			else
			{
				for (long local_26 = 0; local_26 < local_16.field_c; local_26++)
					local_15[local_26] = local_26;
			}
			function_71ff0(arg_0, local_7, (const s_match_player_list *)((byte *)&local_16 + 4), local_15);
			goto local_31;
		}
	local_29:
		if (local_2->state > 2 && local_2->state <= 8 && local_2->flag49e8)
			local_12 = true;
		goto local_30;
	local_28:
		if (arg_0->mode == 3)
			local_12 = true;
	local_30:
		{
			long local_27 = *(long *)((byte *)local_8 + 0x59c) + *(long *)((byte *)arg_0 + 0x974);
			if (local_27 <= local_17)
				local_12 = true;
			else
			{
				if (!local_14.kind)
					session_property_value_set_kind2(&local_14, (local_27 - local_17) / 1000);
				network_session_host_set_data49cc(local_2, (const dword *)&local_14);
			}
		}
		if (!wait_for_transition)
		{
	local_31:
			network_session_set_mode(local_1, 12);
			local_3 = true;
		}
		if (local_12)
		{
			if (*(long *)(local_11 + 8) != 1)
				*(long *)(local_11 + 8) = 1;
			network_session_set_mode(local_1, 6);
			local_3 = true;
		}
	}
finished:
	return local_3;
}
