// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_058dd0.h"
#include "unknown_0662e0.h"
#include <xtl.h>
#include <string.h>

void network_session_close(c_class_58d20 *arg_0);
bool function_71f80(c_session_state_matchmaking *arg_0);
bool network_session_is_leaving(c_class_58d20 *arg_0);
bool network_session_players_match(c_class_58d20 *arg_0, c_class_58d20 *arg_1);
long function_75890(long arg_0);
struct s_session_search;
struct s_search_session;
void session_search_mark_session(s_session_search *arg_0, const s_search_session *arg_1);
bool __stdcall function_59e50(c_class_58d20 *arg_0, long arg_1, long arg_2,
	const XNKID *arg_3, const XNKEY *arg_4, const s_session_member_identity *arg_5,
	long arg_6, const dword *arg_7, const long *arg_8, const long *arg_9,
	bool arg_10, long arg_11, const s_session_id *arg_12, const void *arg_13);

struct s_71e00
{
	dword field_0[3];
};

PRIVATE inline bool function_5b1a0(c_class_58d20 *arg_0)
{
	bool local_0 = false;
	if (arg_0->state > 2 && arg_0->state <= 8 && arg_0->flag5dd8)
	{
		local_0 = true;
	}
	return local_0;
}

// @retail 0x71e00
void function_71e00(c_session_state *arg_0)
{
	c_class_58d20 *local_0 = arg_0->owner->session_a;
	c_class_58d20 *local_1 = arg_0->owner->session_b;
	s_session_snapshot local_2;
	if (session_state_is_live(local_0) && function_5b1a0(local_0))
	{
		long local_11 = local_1->state;
		memcpy(&local_2, local_0->data5ddc, sizeof(local_0->data5ddc));
		if (local_11)
		{
			const s_session_id *local_3 = NULL;
			if (local_1->flag24)
				local_3 = (const s_session_id *)&local_1->unknown1c;
			if (memcmp(local_3, local_2.unknown04, sizeof(s_session_id)) != 0)
				network_session_close(local_1);
			if (local_1->state)
				goto local_12;
		}
		{
			long local_4 = 0;
			s_71e00 local_5[4];
			long local_6[4];
			long local_7[4];
			long local_8 = 0;
			do
			{
				if (local_0->player_mask & (1 << local_8))
				{
					long local_13 = *(const volatile long *)&local_0->current_member;
					if (local_0->players[local_8].member_index == local_13)
					{
						local_5[local_4] = *(const s_71e00 *)&local_0->players[local_8].user_id;
						local_7[local_4] = NONE;
						local_6[local_4] = NONE;
						local_4++;
					}
				}
				local_8++;
			} while (local_8 < 16);
			if (local_4 > 0)
			{
				const s_session_id *local_9 = NULL;
				if (local_0->state && local_0->flag24)
					local_9 = (const s_session_id *)&local_0->unknown1c;
				s_session_id local_10 = *local_9;
				function_59e50(local_1, 2, local_2.unknown00,
					(const XNKID *)local_2.unknown04, (const XNKEY *)local_2.unknown0c,
					(const s_session_member_identity *)local_2.words, local_4,
					local_5[0].field_0, local_7, local_6, false, 0, &local_10, NULL);
			}
		}
	local_12:;
	}
}

#pragma inline_depth(0)
// @retail 0x71d70
bool __stdcall function_71d70(c_session_state_matchmaking *arg_0)
{
	c_session_state_matchmaking *const *local_6 = &arg_0;
	s_session_owner *local_0 = (*local_6)->owner;
	c_class_58d20 *local_1 = local_0->session_a;
	c_class_58d20 *local_2 = local_0->session_b;
	bool local_3 = function_71f80(arg_0);
	if (!local_3 && !local_0->failed)
	{
		long local_4 = local_1->state;
		if (local_4 == 5 || local_4 == 6 || local_4 == 7 || local_4 == 8)
		{
			if (local_2->function_058d20())
				network_session_set_mode(local_2, 2);
			else
				arg_0->mode = 14;
			local_3 = true;
		}
		else
		{
			volatile long local_5 = local_4;
			function_71e00(arg_0);
		}
	}
	return local_3;
}
#pragma inline_depth(255)

// score 15 (was 15): only the order of the players-match block and the timeout test differs:
// retail lays out [search][players match][timeout: jle tail; jmp search], ours
// [search][timeout: jle tail; jmp search][players match]. Retail's two search-block copies are
// cross-jumped after layout; VC merges them before layout (block last) whenever the timeout
// copy is the last arm before the shared return, so the match arm is written last here.
// Tried: a search flag set on both paths (37); if/else with the goto into the else (37);
// the search block written out twice, with local_0 = true (37), return true (65) or one of
// each (90); returns in the players-match branch and a goto to the end (41); a
// do/while (false) with breaks (37); state == 0 and inline state helpers (37).
// Round 34, the search block written in both arms (state == 0 first, then players-match, then timeout):
// first copy `return true`, timeout copy `local_0 = true` and players-match `return true` 74 (aligned 16:
// retail's block order exactly, but the timeout copy ends `mov bl, 1; ...; mov al, bl` instead of
// `mov al, 1`, so the copies are not cross-jumped); with the players-match arm setting the result 85; the
// first copy setting it 90/85; all three `return true` 65 (copies merged, search block last); the
// timeout copy with a byte-offset search pointer or flag 65/65; `local_0 = true; return local_0;` in
// one copy 65/65, in both 34 (178 bytes, copies merged); a dead `local_0 = true` before either
// `return true` 65/65.
// Unhandled-result bool/char/short/word/dword 83/83/89/89/86; genuine success
// char/short/word carriers 66/54/54. Round 96: two copies in the original arm order 37
// (merged), with the result set before/between the calls 95/74; the state test as a switch
// 54 (range compare, default last; copies kept apart); a named bool for the state test 55
// (copies apart, the 71e00 arm last); the state test inverted 37; an extra return in the
// state arm 38/44; an inline search helper at both sites 20 (the timeout site keeps a call),
// forced inline 37; the players-match test `== false` 15; no volatile store 75.
// Round 45: timeout/first flag `!= false` 15/15 and `== true` 35/37; a named flag
// in the timeout/first copy 15/15; a search receiver local in the timeout/first copy 15/15.
// Round 17: natural arm order (state 0, match, timeout) 37; with the state test as a named bool 55
// (session arms as retail, the 71e00 arm last); a plain inline search helper at both sites 20
// (second site a call); one search copy behind `!state || timeout` 55; a search flag from `!state`
// set by the timeout test 40.
// Round 10: the timeout arm a goto into the first search block 37 (block laid out after the timeout
// test); retail's arm order (state, match, timeout) 37; with a named state bool 55 (session arms
// exactly retail's, 71e00 arm last); with the 71e00 arm first under a named bool or `!=` chain 37/37;
// the state test inverted inside the session arm 37; the result set before the timeout or first
// copy's calls 95/115; an early return for type != 9 72; a new inline search helper at the timeout
// site only, the first site only or both 20/37/20 (inlined copies merge before layout).
// Round 16 (lane 2), retail's arm order (state 0, players match, timeout) with the timeout copy made
// different in source: its session argument named first 37; its search pointer named first 37; the
// timeout read into a named elapsed value inside an else arm 37; the players-match result named in
// the else arm 37; its flag tested `!= 0` 37 (all merged before layout, 176 bytes).
// @retail 0x712d0
bool function_712d0(c_session_state_matchmaking *arg_0)
{
	bool local_0 = false;
	s_session_owner *local_1 = arg_0->owner;
	c_class_58d20 *local_2 = local_1->session_a;
	c_class_58d20 *local_3 = local_1->session_b;
	if (local_2->type == 9)
	{
		long local_4 = local_2->state;
		if (local_4 == 5 || local_4 == 6 || local_4 == 7 || local_4 == 8)
		{
			if (!local_3->state)
			{
				if (arg_0->flag97c)
					session_search_mark_session((s_session_search *)&arg_0->flag97c,
						(const s_search_session *)((byte *)arg_0 + 0xa20));
				network_session_set_mode(local_2, 7);
				local_0 = true;
			}
			else if (!network_session_players_match(local_2, local_3))
			{
				if (function_75890(arg_0->unknown970) > g_network_configuration.value190)
				{
					if (arg_0->flag97c)
						session_search_mark_session((s_session_search *)&arg_0->flag97c,
							(const s_search_session *)((byte *)arg_0 + 0xa20));
					network_session_set_mode(local_2, 7);
					local_0 = true;
				}
			}
			else
			{
				network_session_set_mode(local_2, 13);
				local_0 = true;
			}
		}
		else
		{
			volatile long local_5 = local_4;
			function_71e00(arg_0);
		}
	}
	return local_0;
}

// @retail 0x71390
bool __stdcall function_71390(c_session_state_matchmaking *arg_0)
{
	s_session_owner *local_0 = arg_0->owner;
	c_class_58d20 *local_1 = local_0->session_a;
	c_class_58d20 *local_2 = local_0->session_b;
	bool local_3 = false;
	if (local_1->type == 13)
	{
		local_3 = function_71f80(arg_0);
		if (!local_3 && !local_0->failed)
		{
			if (!local_2->state)
			{
				local_2 = local_1;
				if (local_2->function_058d20())
				{
					if (arg_0->flag97c)
						session_search_mark_session((s_session_search *)&arg_0->flag97c,
							(const s_search_session *)((byte *)arg_0 + 0xa20));
					network_session_set_mode(local_2, 7);
					local_3 = true;
				}
				else
					function_71e00(arg_0);
			}
			else
			{
				if (local_2->function_058d20() && local_2->type != 14 &&
					!network_session_is_leaving(local_2))
				{
					network_session_set_mode(local_2, 14);
					local_3 = true;
				}
				if (local_1->function_058d20() && local_2->type == 14)
				{
					network_session_set_mode(local_1, 7);
					local_3 = true;
				}
			}
		}
	}
	return local_3;
}
