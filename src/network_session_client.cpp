// @flags /O2 /Ob1 /Gr
/* NETWORK_SESSION_CLIENT.CPP: the session client's request queue, the
   session owner and the joining state's helpers (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"
#include "unknown_0662e0.h"
#include "online_tasks.h"
#include "unknown_234c64.h"
#include "unknown_19b510.h"

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

struct s_session_summary
{
	long machine_count;
	s_session_id machine_ids[16];
	XUID machine_users[16];
	long machine_times[16];
	long player_count;
	XUID player_users[16];
	long player_values248[16];
	long player_values288[16];
	long player_machines[16];

	bool remove_machine(s_session_id *id);
};
long __stdcall function_063190(void *summary, long user);
long session_summary_get_player_value(s_session_summary *summary, long player_index, long variant_index);
long network_session_find_member(c_class_58d20 *session, const s_session_member_identity *identity);
void network_session_disband_member(c_class_58d20 *session, long member_index);
__declspec(noinline) long network_session_cancel_reservations(c_class_58d20 *session, const s_session_id *id);
bool network_session_host_set_summary(c_class_58d20 *session, const s_session_summary *summary);

/* disbands the other members that share the machine id, cancels its
   reservations and takes the machine out of the published summary */
// @retail 0x6d380
void __stdcall function_06d380(c_session_client *client, const s_session_id *id)
{
	{
		c_class_58d20 *session = client->session;
		long current = session->current_member;
		long found = 0;
		long count = session->member_count;
		s_session_member_identity members[16];
		for (long i = 0; i < count; i++)
		{
			if (i != current && !memcmp(&session->members[i].id, id, sizeof(*id)))
			{
				memcpy(&members[found], session->members[i].words, sizeof(members[found]));
				found++;
			}
		}
		if (found > 0)
		{
			long remaining = found;
			s_session_member_identity *member = members;
			do
			{
				session = client->session;
				long index = network_session_find_member(session, member);
				if (index != NONE && index != session->current_member &&
					session->state != 7 && session->state != 6 && session->state != 8)
					network_session_disband_member(session, index);
				member++;
			}
			while (--remaining);
		}
	}
	network_session_cancel_reservations(client->session, id);
	s_session_summary *source = (s_session_summary *)client->session->get_data_4a00();
	if (source)
	{
		s_session_summary summary = *source;
		if (summary.remove_machine((s_session_id *)id))
			network_session_host_set_summary(client->session, &summary);
	}
}

// @retail 0x6e910
void function_6e910(const s_network_session_player *player, const s_session_machine *machines,
	const byte *variant, s_session_summary *summary, char default_team, s_session_player *output, long variant_index)
{
	output->active = true;
	output->flag1 = false;
	output->machine = machines[player->member_index];
	output->index = (short)player->slot;
	output->controller = player->unknown14;
	memcpy(&output->id, player, sizeof(output->id));
	memcpy(output->name, player->propertiesa8, sizeof(player->propertiesa8));
	if (variant_index != NONE)
	{
		long index = function_063190(summary, (long)player);
		if (index != NONE)
		{
			*(long *)((byte *)output + 0xa0) = variant_index;
			*(long *)((byte *)output + 0xa8) = *(long *)((byte *)summary + 0x288 + index * 4);
			*(short *)((byte *)output + 0xa6) = *(short *)((byte *)summary + 0x248 + index * 4);
			*(short *)((byte *)output + 0xa4) = (short)session_summary_get_player_value(summary, index, variant_index);
		}
	}
	if (variant && output->flag98 != 0xff)
	{
		if (variant[0x48] & 1)
		{
			long team;
			if ((char)output->flag98 < 0)
				team = 0;
			else if ((char)output->flag98 > 7)
				team = 7;
			else
				team = (char)output->flag98;
			output->flag98 = (byte)team;
		}
		else
			output->flag98 = default_team;
	}
}

// @retail 0x6dd00
bool c_session_client::function_06dd00(long a, s_session_remote *remote)
{
	bool result = false;
	if (request_count < 30)
	{
		s_allocator_globals *globals = g_4d87f8;
		s_session_request *request = (s_session_request *)globals->allocator->allocate(sizeof(s_session_request), 0, 0);
		if (!request)
		{
			globals->allocator->compact(0);
			request = (s_session_request *)globals->allocator->allocate(sizeof(s_session_request), 0, 0);
		}
		if (request)
			globals->count++;
		if (request)
		{
			request->remote = *remote;
			memcpy(request->key04, (const void *)a, sizeof(request->key04));
			request->next = requests;
			requests = request;
			request_count++;
			result = true;
		}
	}
	return result;
}

static __forceinline void function_6ddb0(s_allocator_globals *arg_0, s_session_request *arg_1)
{
 arg_0->allocator->release(arg_1, NONE);
 if (arg_1)
  arg_0->count--;
}

// @retail 0x6ddb0
void session_client_remove_request(c_session_client *client, s_session_request *request)
{
	s_session_request **link = &client->requests;
	while (*link && *link != request)
		link = &(*link)->next;
	if (*link == request)
	{
		*link = (*link)->next;
		long info;
		g_4d87f8->allocator->get_info(request, &info);
		function_6ddb0(g_4d87f8, request);
		client->request_count--;
	}
}

// @retail 0x6de10
bool c_session_client::function_06de10(s_session_remote *remote)
{
	s_session_request *request = requests;
	bool found = false;
	for (; request; request = request->next)
	{
		if (found)
			break;
		if (!memcmp(request->remote.key188, remote->key188, sizeof(remote->key188)))
			found = true;
	}
	return found;
}

// @retail 0x6de50
void session_owner_initialize(s_session_owner *owner_, long unknown40, long unknown44, void *unknown2c, c_class_58d20 *session_a, c_class_58d20 *session_c, c_class_58d20 *session_b, void *unknown3c)
{
	s_session_owner_view *owner = (s_session_owner_view *)owner_;
	memset(owner->states, 0, sizeof(owner->states));
	owner->mode = 0;
	owner->unknown40 = unknown40;
	owner->unknown44 = unknown44;
	owner->unknown2c = unknown2c;
	owner->session_a = session_a;
	owner->session_c = session_c;
	owner->session_b = session_b;
	owner->unknown3c = unknown3c;
	owner->unknown49 = false;
	owner->unknown48 = false;
	owner->failed = false;
}

// @retail 0x6df60
inline void function_06df60(s_session_owner *o, long a, long b, long c)
{
	o->failed = true;
	o->error_code = a;
	o->data_size = b;
	long *data = o->data;
	*data = 0;
	if (o->data_size > 0)
		memcpy(data, (const void *)c, o->data_size);
}

// @retail 0x6e0f0
inline bool network_session_members_ready(c_class_58d20 *session, dword *unready_mask)
{
	dword mask = 0;
	bool ready = true;
	for (long i = 0; i < session->member_count; i++)
	{
		if (session->members[i].unknown88 <= 2)
		{
			mask |= 1 << i;
			ready = false;
		}
	}
	if (unready_mask)
		*unready_mask = mask;
	return ready;
}

// @retail 0x6f090
void session_state_joining_initialize(c_session_state_joining *state_, s_session_owner *owner)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	state->owner = (s_session_owner_view *)owner;
	state->index = 5;
	state->unknown0d = true;
	state->skip_cleanup = false;
	state->owner->states[5] = (c_session_state *)state_;
	state->unknown104 = 16;
	state->unknownf0 = NONE;
	state->unknownf4 = NONE;
	state->unknown68 = false;
	state->unknown10 = false;
	state->unknown11 = false;
	state->unknownf8 = false;
	state->unknowne4 = 0;
	state->unknowne8 = false;
	state->unknownf9 = false;
	state->unknowne9 = false;
	state->unknownec = 0;
}

// @retail 0x6fc80
void session_state_joining_check_target(c_session_state_joining *state_)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	c_class_58d20 *local_0 = state->owner->session_c;
	long session_state = *(volatile long *)&local_0->state;
	if (session_state != 1)
	{
		if (session_state != 0)
		{
			if (SESSION_STATE_IS_LIVE(session_state))
				state->unknownf8 = true;
		}
		else
		{
			state->unknown104 = 16;
		}
	}
}

void function_6b640(long task_index);
void qos_release(long handle);

/* clears the joining state's progress */
static inline void session_state_joining_reset(s_session_state_joining_view *state)
{
	state->unknown68 = false;
	state->unknown10 = false;
	state->unknown11 = false;
	state->unknownf8 = false;
	state->unknowne4 = 0;
	state->unknowne8 = false;
	state->unknownf9 = false;
	state->unknowne9 = false;
	state->unknownec = 0;
}

// @retail 0x6f0f0
void c_session_state_joining::function_06f0f0()
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)this;
	c_class_58d20 *session = state->owner->session_c;
	if (!state->unknown104)
		state->unknown104 = 16;
	if (state->unknownf0 != NONE)
	{
		function_6b640(state->unknownf0);
		state->unknownf0 = NONE;
	}
	if (state->unknownf4 != NONE)
	{
		qos_release(state->unknownf4);
		state->unknownf4 = NONE;
	}
	if (session->state && !function_058d90(session))
		network_session_leave(session, false);
	session_state_joining_reset(state);
}

/* a random value in [lower, upper) from the second seed */
static inline short session_random_range(short lower, short upper)
{
	dword *seed = &g_4e7408->seed;
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return lower + (short)(((upper - lower) * (*seed >> 16)) >> 16);
}

// @retail 0x70190
void session_state_matchmaking_initialize(c_session_state_matchmaking *state_, s_session_owner *owner)
{
	s_session_state_matchmaking_view *state = (s_session_state_matchmaking_view *)state_;
	session_state_initialize((s_session_state_view *)state, (s_session_owner_view *)owner, 6, true, false);
	bool &initial_flag = state->unknown97c;
	initial_flag = false;
	long &first_zero = state->unknowna08;
	first_zero = 0;
	long &second_zero = state->unknowna0c;
	second_zero = 0;
	long &third_zero = state->unknowna1c;
	third_zero = 0;
	state->unknown9ec = NONE;
	state->unknown9f0 = NONE;
	state->unknown9f4 = NONE;
	state->mode = 1;
	state->unknown96c = session_random_range(0, (short)g_network_configuration.value19c);
	state->unknown974 = session_random_range(0, (short)g_network_configuration.value194);
	state->unknowna64 = false;
	state->unknowna80 = 0;
	state->unknowna84 = 0;
	state->unknowna88 = 0;
	state->unknowna78 = false;
	state->unknowna8c = 0;
	state->unknowna90 = 0;
	state->unknowna94 = 0;
}

// @retail 0x6f1a0
void c_session_state_joining::function_06f1a0()
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)this;
	s_session_owner_view *owner = state->owner;
	long mode = owner->mode;
	c_class_58d20 *session = owner->session_a;
	if (mode != 1 && mode != 0 && mode != 4 && mode != 9)
		state->unknown104 = 15;
	if (!state->unknown104)
	{
		if (session->state && !function_058d50(session))
			state->unknown104 = 14;
	}
}

// @retail 0x6f200
void c_session_state_joining::function_06f200(bool flag, const void *target, long count, const void *entries)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)this;
	state->unknown104 = 0;
	state->unknown11 = flag;
	function_06f1a0();
	if (!state->unknown104)
	{
		s_session_owner *owner = (s_session_owner *)state->owner;
		memcpy(state->target, target, sizeof(state->target));
		state->entry_count = count;
		memset(state->entries, 0, sizeof(state->entries));
		memcpy(state->entries, entries, count * 12);
		state->unknown10 = true;
		function_06df60(owner, 5, 0, 0);
	}
}

// @retail 0x6f2b0
void c_session_state_joining::function_06f2b0(const s_session_description *description, long count)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)this;
	long minimum_version = *(volatile long *)&description->unknown0c;
	long version = *(volatile long *)&description->unknown08;
	if (description->unknown04 == 4 && version >= 0x2651 && minimum_version <= 0x2651)
	{
		if (description->unknown10 == (online_logon_connected() ? 2 : 1))
		{
			short kind = description->unknown14;
			if (kind == 0)
			{
				if (count > description->unknown94)
					state->unknown104 = 7;
			}
			else if (kind == 1 && state->unknown11)
			{
				if (count > description->unknown96)
					state->unknown104 = 7;
			}
			else
			{
				state->unknown104 = 8;
			}
		}
		else
		{
			state->unknown104 = 10;
		}
	}
	else
	{
		state->unknown104 = 9;
	}
	short status = description->unknown9e;
	if (status == 7 || status == 8 || status == 6)
		state->unknown104 = 12;
	if (description->unknown9e == 5)
		state->unknown104 = 13;
}

// @retail 0x6f3a0
void c_session_state_joining::function_06f3a0(const s_session_description *description, long count, const void *entries)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)this;
	state->unknown104 = 0;
	function_06f1a0();
	if (!state->unknown104)
		function_06f2b0(description, count);
	if (!state->unknown104)
	{
		s_session_owner *owner = (s_session_owner *)state->owner;
		state->entry_count = count;
		memset(state->entries, 0, sizeof(state->entries));
		memcpy(state->entries, entries, count * 12);
		state->part.unknown00 = description->unknown02;
		*(XNKID *)state->part.unknown04 = description->kid;
		*(XNKEY *)state->part.unknown0c = description->key;
		*(XNADDR *)state->part.unknown1c = description->address;
		state->part.unknown40 = description->unknown10;
		state->unknown68 = true;
		function_06df60(owner, 5, 0, 0);
	}
}

static inline long session_time_get(void)
{
	long time;
	if (g_510548)
		time = g_51054c;
	else
		time = GetTickCount();
	return time;
}

void network_session_set_mode(c_class_58d20 *session, long mode);
bool network_session_parameters_set_mode(c_class_58d20 *session, long mode);
bool network_session_parameters_set_data5ddc(c_class_58d20 *session, const s_parameters_part *data);
bool network_session_id_differs(c_class_58d20 *session, const s_parameters_part *part);
long function_75890(long time);

// @retail 0x6f9d0
void session_state_joining_request_host_mode(c_session_state_joining *state_)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	c_class_58d20 *session = state->owner->session_a;
	c_class_58d20 *target = state->owner->session_c;
	state->unknowne8 = true;
	if (session->function_058d20())
	{
		long target_state = target->state;
		if (!target_state)
		{
			state->unknown104 = 16;
		}
		else if (target_state > 2 && target_state <= 8)
		{
			network_session_set_mode(session, 17);
			state->unknownfc = session_time_get();
		}
	}
}

// @retail 0x6fbe0
void session_state_joining_check_ready(c_session_state_joining *state_)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	c_class_58d20 *session = state->owner->session_a;
	c_class_58d20 *target = state->owner->session_c;
	state->unknowne8 = true;
	if (session_state_is_live(target))
	{
		if (session->function_058d20())
		{
			long start = state->unknown100;
			long now = session_time_get();
			if (session->member_count == 1 || now - start > g_network_configuration.value17c)
				state->unknownf8 = true;
		}
		else
		{
			state->unknownf8 = true;
		}
	}
	else
	{
		state->unknown104 = 16;
	}
}

// @retail 0x6f800
void __stdcall session_state_joining_set_mode(c_session_state_joining *state_)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	c_class_58d20 *session = state->owner->session_a;
	if (session->type != 1)
	{
		long last = state->unknownec;
		if (!last || session_time_get() - last > g_network_configuration.value188)
		{
			network_session_parameters_set_mode(session, 1);
			state->unknownec = session_time_get();
		}
	}
}

// @retail 0x6f880
void __stdcall session_state_joining_send_target(c_session_state_joining *state_)
{
	s_session_state_joining_view *state = (s_session_state_joining_view *)state_;
	c_class_58d20 *session = state->owner->session_a;
	if (state->unknown68 && network_session_id_differs(session, &state->part))
	{
		if (session->type != 15)
		{
			long last = state->unknowne4;
			if (!last || function_75890(last) > g_network_configuration.value184)
			{
				network_session_parameters_set_data5ddc(session, &state->part);
				network_session_parameters_set_mode(session, 15);
				state->unknowne4 = session_time_get();
			}
		}
	}
	else
	{
		state->unknown104 = 16;
	}
}

/* sets the session's machine limit while it is live and hosting */
static inline bool session_set_machine_limit(c_class_58d20 *session, long value)
{
	bool result = false;
	if (SESSION_STATE_IS_LIVE(session->state))
	{
		if (session->function_058d20())
		{
			session->value4990 = value;
			session->update_count++;
			result = true;
		}
	}
	return result;
}

// score 22 (was 22): kept in round 184 (coordinator ruling): the variant address named before the summary
// getter gives the session EBX as in retail, and 0x6e500 and 0x72700 match with it. Left: the opening's
// second lea after the je (retail's comes before it) and a one-step scratch rotation through the summary
// lea, the mask temp and the copy loop.
// Round 105: direct unconditional stored-pointer pair 22 (same code); named missing condition 218; summary declared
// after the buffers 22; named machine-mask value 22; memcpy machine copy 22; two scalar copies 144;
// address/member pointer arithmetic 22/22. Original body restored; caller and matched-callee controls preserved.
// Before (55): aligned frame agrees; the code is retail's apart from the colours. Retail takes the
// session in EBX (its callers copy it there) and gives the loop temporaries ESI/EDI; ours keeps the session
// in ESI and the loop temporaries in EBX/EDI. Deleting the whole tail (live test, 0x4994 setter, machine
// limit) moves it to EBX (diagnostic 244, 321 bytes); deleting either setter alone does not; a /Gd first
// pass and a harness build give ESI for every body, even with either loop removed. The stored machine/player
// pointers preset to NULL and set when the session is live and has them (0x6ec80's form) give retail's test
// schedule and opening scratch registers at 49 (session still ESI, also without the tail; not kept: a gain
// of 6). The --fast image doesn't represent this function (116 for the unchanged source): use the plain check.
// Tried: countdown native object copy 55; native address copy 55; machine object reference 55; machine value
// copy 55; short machine alias 55; immutable member count 55; machine counter names 55; top-tested countdown
// 114; member count read before the clears 129; summary set after the clears 129; machine pointer before the
// clears 55; zero initialisers for the two buffers 151; summary as a conditional 131; summary NULL in an
// else 131; as a session method 55; the address copy as a plain for loop 114 (477 bytes); memcpy for the
// address copy 55; machine array indexed at both uses 55; members array pointer for the address copy 55;
// address cursor in the copy loop 59; one index for both loops 55; bit-first mask test 55; nested mask and
// slot tests 55; existing setter: void result, guard exits, test-derived result and direct success 55 each;
// native result 146 (out of line); shared live predicate, native receiver and input order 55 each;
// force-inline native/original result 55/55; limit helper as one `if` 55; live and 0x5dd0 tests nested 55;
// state macro for the tail's live test 221 (setters stop inlining); limit setter as a cross-file session
// method beside 0x5c490 55 (inlined, session still ESI); state read once in the limit helper 55; inline live
// predicate in the limit helper 55; named player pointer 58 (52 with the preset pointers); members array
// pointer plus a player pointer 58; short player-loop index 128 (492 bytes); output pointer walked beside the
// player loop 58; source/output player cursors 58; player buffer declared first 55; players cleared first 53
// (with the preset pointers: declared and cleared first 53/53); summary at function scope 63; early `return
// false` / `return true` exits 143; result false in an else 153; result set at the top of the state block
// 98; member count read at each use 55; the live/0x5dd0 tail as an inline helper 55. With the preset
// pointers: untyped pair 49; player index declared with the buffers 49; a plain `for` copy loop gives
// retail's copy-loop scratch registers but folds the player loop's mask read (108, 477 bytes). A 96-form
// screen (opening test x summary x copy loop x count local x buffer order x player pointer) found nothing
// below 49; a 600 s permuter run (reduced image 143 against 49) found nothing. Round 82: existing live
// query in the outer tail 130 (451 bytes); in the limit helper 55; both 130; byte function result 95 (497);
// short limit value 55; byte limit-helper result 146 (430); unsigned machine countdown 55.
// Lane 17 r1: the 0x4994 setter's body visible in this file (diagnostic) 55, session still ESI; address
// pointer named before the mask store 55; summary read just before the player loop 76; copy loop as a
// signed-bound do-while 65; preset pointers reproduced 49; with that do-while 117.
// the aligned frame and every instruction agree apart from the colours: retail
// takes the session in EBX (its callers copy it there) and gives the player-loop index ESI and the
// player pointer EDI; ours rotates them (session ESI, index EDI, player pointer EBX). Deleting the
// whole tail (live test, 0x4994 setter, machine limit) moves the session to EBX (diagnostic 244,
// 321 bytes), deleting either setter alone does not; a /Gd first pass and a harness build give ESI
// for every body, even with either loop removed. The stored machine/player pointers preset to NULL
// and set under the flag (or the live and flag tests, 0x6ec80's form) give retail's scratch
// registers at the start, 49 (not kept: a gain of 6 from a flag-set-then-tested form); with them a
// plain `for` copy loop gives retail's copy-loop scratch registers but folds the player loop's mask
// read, 108 (477 bytes).
// Tried: countdown native object copy 55; native address copy 55; machine object reference 55;
// machine value copy 55; short machine alias 55; immutable member count 55; machine counter names
// 55; top-tested countdown 114; member count read before the clears 129, at each use 55; summary
// set after the clears 129; machine pointer before the clears 55; zero initialisers for the two
// buffers 151; summary as a conditional 131, NULL in an else 131, at function scope 63; as a
// session method 55; the address copy as a plain for loop 114 (477 bytes), memcpy 55, address
// cursor 59, members array pointer 55; machine array indexed at both uses 55; one index for both
// loops 55; bit-first mask test 55; nested mask and slot tests 55; short player-loop index 128 (492
// bytes); named player pointer 58 (52 with the preset pointers); output pointer walked beside the
// player loop 58; source/output player cursors 58; player buffer declared first 55; players cleared
// first / declared and cleared first 53/53; existing setter: void result, guard exits, test-derived
// result and direct success 55 each; native result 146 (out of line); shared live predicate,
// native receiver and input order 55 each; force-inline native/original result 55/55; limit helper
// as one `if` 55; live and 0x5dd0 tests nested 55; state macro for the tail's live test 221
// (setters stop inlining); the live/0x5dd0 tail as an inline helper 55; limit setter as a
// cross-file session method beside 0x5c490 55 (inlined, session still ESI); state read once in the
// limit helper 55; inline live predicate in the limit helper 55; early `return false` / `return
// true` exits 143; result false in an else 153; result set at the top of the live block 98; player
// index declared with the buffers 49 (preset form); untyped stored pair 49. A 96-form screen
// (opening test x summary x copy loop x count local x buffer order x player pointer) found nothing
// below 49; a 600 s permuter run found nothing. Round 82: existing live query in the outer tail 130
// (451 bytes), in the limit helper 55, both 130; byte function result 95 (497); short limit value
// 55; byte limit-helper result 146 (430); unsigned machine countdown 55. Lane 13 round 18: the
// function and its limit helper moved into unknown_058dd0.cpp beside 0x6e6b0 (/O2, retail's
// neighbour) 55 (same code); the player-loop index renamed 55, the session parameter renamed 55
// (same code: names don't move the registers); named player pointer 58 (same rotation); the
// flag-only preset pointers 49 (session still ESI).
// Round 127 (lane 4): the register parameter comes from a first pass with the session on the stack:
// ours loads it once into ESI at entry, so LTCG passes it in ESI; retail's first pass gave it EBX
// (its callers do `push ebx; mov ebx, esi; call`). A /Gd compile-only harness of this file with
// get_data_4a00, set_value_4994 and function_058d20 defined inline reproduces it: without the tail
// the pass gives the session EBX, the index ESI and the player pointer EDI (retail's, copy loop
// included); one setter alone, either setter, or the tail without the live test all give ESI; so do
// dropping the two address tests, the mask test or set_data_4f24 (diagnostics), while dropping the
// summary getter or the call's two session field arguments gives EBX. Real trials: the function as
// a session method 55 (same code); the limit helper in set_value_4994's shape 55; member count
// read before the summary 131; summary fetched after the copy loop 76; the copy loop through a
// members pointer 55. Harness-only: index declared at block top, the loop argument hoisted, the
// player loop as a forceinline helper, all ESI.
// Round 107: in the first-pass harness removing either in-loop session argument of the 0x6e910 call (the
// 0x4db0 address or the 0x49c8 value), or the member count read, gives EBX/ESI/EDI; named copies of them
// (inside the call block or before the loop) and a players array pointer are propagated back (ESI). Real
// (fast with 0x58d20/0x6e500/0x72700/0x6e6b0): limit setter only when the 0x4994 setter succeeds 154 (421
// bytes); players array pointer for the test and the call 55; dword player index 56; named argument
// pointer inside the call block 55; mask/slot test as `continue` 55.
// Lane 14 r19: diagnostics (reverted): without the three in-loop session uses the session is not
// enregistered at all (152); without only the 0x49c8 read in the call block it takes EBX with retail's
// index ESI and player pointer EDI (35, the rest is the constant), so the session outweighs the loop index
// by less than one call-block use. Forms (fast, listing the inlined callees 0x5c490 0x58d20 0x5b080
// 0x5c5c0 0x6e910, which reproduces the plain 55): variant address named before the loop 52 (substituted
// back); `continue` guards 55; `do { } while (++i < 16)` 221; `while (i < 16)` 221; the loops through
// the membership getter 0x5a680 55; 0x6e910's team parameter as `long` (no `char` truncation) 55.
// Round 32 (lane 11, plain check): the 0x49c8 value through the existing getter 142 (503 bytes); shift-first
// mask test `(mask >> i) & 1` 126 (475 bytes); `session->players + i` 55; the address copy as member and
// address pointer walks 59; `(mask & (1 << i)) != 0` 55. A compile-only first pass (this file /Gd with the
// setters and getters inline) reproduces ESI and the known flip (no 0x49c8 read: EBX/ESI/EDI); the member
// count or the 0x4f20 tests through force-inline helpers stay ESI (parameter copies are propagated), as do a
// named has-data test, the variant cast, unsigned/short-cast indices and the output as `players + i`.
// Round 21 (lane 14, fast with the inlined callees): the call's last argument through the existing
// get_value_49c8() 142 (503 bytes: the live test comes back); the copy countdown declared before its
// index 55 (same code); the result initialised from the state test 169 (507); the copy loop counting
// member_count itself down 118; the machine-address pointer written at both uses 55 (same code).
// Round 97 (lane 8, fast with the inlined callees): the mask test with the bit first 55; the machine mask
// stored after the copy loop 139 (495 bytes); the player-loop index declared with the buffers (no preset
// pointers) 55; a short copy-loop index 153 (467 bytes); the summary declared after the buffers 55.
// Round 51 (lane 16, fast with the inlined callees): the address copy as `if (n > 0) for (i = 0; i != n;
// i++)` (the rolled-countdown form of 0x2be050) 114 (477 bytes), as a plain `for (i < n)` 114 (477 bytes).
// Round 113 (lane 6, fast with the inlined callees): variant address and players base named before the
// player loop 52; address pointer declared with the buffers 55; const session parameter 55; short copy index
// 153 (467 bytes); 0x6ec80's preset stored pair plus the variant address named before the loop 46 (opening
// scratch registers retail's, session still ESI), plus a players base 46, with a plain `for` copy 108 (477
// bytes: retail's copy prologue, but the mask read folds into the test), with that the bit-first mask test,
// a distinct copy index name or a mask local in the loop 108 each. Diagnostic on the 46 form: the 0x49c8
// argument as a constant gives EBX/ESI/EDI with only the copy prologue (count spill before the leas) left, 25.
// Round 19 (lane 2, fast with the inlined callees): the summary read directly under its flag (no
// getter) 55; the summary as 0x72330's live-and-flag ternary 131; the opening test in conjunctive form
// 55; the player index declared at the function top 55; an explicit `continue` arm 55. Diagnostics: the
// summary as a constant NULL gives the session EBX (129 overall); the summary's address
// taken keeps ESI (55, same code), so the two summary field reads, not its stack home, tip the session.
// Round 30 (lane 13, fast): the call's variant address and 0x49c8 value both named before the player
// loop 130 (493 bytes): the session moves to EBX with retail's index ESI and player EDI, but the value
// takes a stack slot (frame 0xec0); both named after the summary getter 71, before the member count
// read 65 (483 bytes, the same registers; left the value slot, its store/load and the folded mask
// test); the value alone before the loop 137; the variant alone before the member count 52. Named
// copies inside the loop or a `long const *` to the value are propagated back (session ESI).
// Round 117 (lane 6, fast with the inlined callees): player loop bounded by `i != 16` 56; player index declared at
// function scope 55 (same code); the team argument as plain `i` 55 (same code); `dword` player index 56; `short`
// player index 128 (492 bytes).
// Round 183 (lane 3): `const byte *variant = session->data4db0;` declared before the summary getter and
// passed to 0x6e910 gives the session EBX (index ESI, player EDI) and 22 (kept in round 184 by coordinator
// ruling); with it 0x6e500 and 0x72700 match in their recorded forms. Left:
// the opening's second lea after the je and a one-step scratch rotation through the summary lea, the mask
// temp and the copy loop. The variant at function top, after the summary or in the loop block: ESI; the
// summary as `flag49fd ? data4a00 : NULL` with the variant in the loop block: EBX but an if/else summary
// 126. The preset stored-pointer opening (retail's lea schedule and opening registers) keeps ESI in every
// form tried (real 46, with the variant 46; ~600 first-pass harness forms); the plain `for` copy loop gives
// retail's copy code but folds the mask read into the test (100, 477 bytes; one extra temporary before the
// player loop unfolds it in the harness). Pointer locals to the mask or the 0x49c8 value are propagated
// back; a two-ternary stored-pointer opening keeps EBX and retail's summary/mask registers but adds the
// ternary code. First-pass harness: only in-loop uses decide the order (one session use fewer or one index
// use more flips it); out-of-loop uses, names and a fastcall receiver do not.
// Round 31 (lane 13, compile-only first-pass harness): 0x6e500 and 0x72700 both match once this function
// takes the session in EBX (see their notes), so this register is all that blocks them. Diagnostics that
// give retail's EBX/ESI/EDI: one more in-loop index use (`value49c8 + i`), the variant or summary argument
// NULL, the 49c8 argument replaced by a player field. The session cannot take EDI (live across `rep stosd`),
// so whichever of session and loop index ranks first takes ESI; retail's index outranks the session by less
// than one call-block use. No flip from: typed member-machine field, typed machine-list struct, both (6
// forms); the preset stored pair with or without the live test, with the summary moved or the variant named
// (10); 250 random combinations of 12 natural dimensions (opening, buffer/memset order, summary position,
// count local, index scope, mask/slot spellings, nested tests, `players + i`, nested tail); const parameter,
// function-scope or shared index, the size macro (5). Frame alignment comes from the players memset (no
// effect). Flags: only /O1 and /Os give EBX, with different code.
// Round 143 (lane 4, fast with the inlined callees and both callers, faithful 22): every opening that
// computes both stored addresses before testing them gives retail's opening schedule and summary/mask
// registers but the session ESI (0x6e500 69, 0x72700 94): NULL-preset pair 46, if/else pair 46, the pair
// assigned players first 46, a two-out-pointer inline getter 46; on the preset pair the variant named at
// the top of the state block 46 or just before the player loop 46.
// Round 122 (lane 6, fast with the inlined callees): the preset stored pair, a plain `for` copy loop and the
// 0x49c8 value named before the player loop 116 (493 bytes: every instruction retail's, session EBX, but the
// value takes a stack slot, frame 0xec0); preset pair + `for` copy + players base local 109 (477 bytes, session
// ESI); if/else stored pair + `for` copy 108; `for` copy + named player pointer 100 (477, the mask read folds);
// preset pair + `for` copy + named player pointer 108. First-pass harness (this file's body with the inlined
// callees, /Gd): the preset pair + `for` copy is retail's code up to the player loop with the session in ESI;
// deleting the whole tail (live test and both setters) gives EBX there, deleting one setter, the live test,
// the summary or the set_data_4f24 call does not; 176 statement orders of variant/summary/memsets/count/mask
// store with the `for` copy loop give nothing below the current order.
// Round 57 (lane 16, fast with the inlined callees): the two block addresses named inside a `flag4f20` block that
// jumps to the result store 22 (same code: the second lea still follows the first test); the buffers declared
// before the variant 22 (same code); a `for` copy loop bounded by `session->member_count` 138 (session ESI);
// 0x6ec80's live-and-flag opening as an inline out-pointer getter 46 (opening schedule, summary lea and mask temp
// become retail's; session ESI, index EDI, cursor EBX); that with a `for (i < member_count)` copy loop and one
// index shared by both loops 108 (477 bytes). Diagnostics: a plain `for` copy loop on this body 100 (477 bytes:
// the copy loop, its prologue and the out-cursor lea become retail's, the summary lea and mask temp stay rotated,
// the player loop folds the mask read); the getter opening with a `for` copy loop and the 0x49c8 argument as 0
// gives retail's EBX/ESI/EDI and retail's code up to the player loop (97): only the folded mask read and the
// player loop's argument temporaries (out ECX, summary EDX for retail's EAX/ECX) remain, so retail's form looks
// like that opening plus the `for` copy with the session ranked below the index.
// Round 43 (lane 15, fast with both callers): the two stored addresses passed to an inline presence helper 22
// (same code: the arguments are propagated back, the second lea stays after the je); the 0x6e910 call through an
// inline wrapper taking the session 22 (same code), with the variant read inside the wrapper 55 (session ESI); the
// pair named in the flag block ahead of a `missing` flag 218 (361 bytes); preset pair with the variant named just
// before the member count 46; preset pair with a plain `for` copy loop 108 (477 bytes; both callers lost in each
// preset form).
// Round 44 (m3 lane 2, plain check with both callers): the mask computed from the field with the count local
// read after it 22 (same code); the summary getter written in the call 147 (493 bytes, both callers lost); a
// `dword` copy index 22; the player buffer as an s_unknown_3648 block indexed through a cast 22; the player mask
// read into a local before the player loop 130 (493 bytes).
// Round 68 (m3 lane 11, fast with both callers; control 22): preset pair with the summary read under its flag 46; with a
// plain `for` copy 108 (477 bytes); both addresses through a small pair struct 46; first address by ternary, second plain
// 49; both by ternary 156 (499 bytes, the second flag test stays, callers keep matching); a named present bool 218 (361);
// a presence helper with local copies of both addresses 218 (361); a pair helper with local copies behind the flag test 22
// (same code). Both callers are lost in every 46, 49, 108 and 218 form.
// Round 75 (m3 lane 10, fast with both callers; control 22): the address pointer named before the clears 22, the
// copy index declared with the buffers 22, the mask stored after the address pointer 22 (all same code); the member
// count read before the clears 124; the variant and summary fetched before the opening test 32 (the summary lea then
// takes retail's EAX, the mask and copy-loop rotation stay). Plain `for` copy loop 100 (477 bytes: its copy loop and
// prologue are byte for byte retail's; the player loop then folds the mask read and rotates its argument registers);
// on it the bit-first mask test 100, the mask in a loop local 100, the bit in a loop local 100, nested mask and slot
// tests 100, the slot test first 99, the copy bounded by `session->member_count` 138 (both callers lost).
// Round 168 (m3 lane 6, fast with the inlined callees and both callers; control 22): re-measured the preset stored
// pair with the variant named before the summary getter 46 (both callers lost; only the session, index and player
// pointer homes and the copy temporaries differ). With a plain `for` copy loop on the preset pair: an output pointer
// walked in the player loop's increment 109, the variant named just before the player loop 108, the pair assigned
// players first 108, the summary fetched before the variant 108, the copy through memcpy per machine 108 (all 477
// bytes, the session in ESI, 0x6e500 69 and 0x72700 94). Every preset-pair / `for`-copy combination collapses to
// 108, so the next round should skip that base.
// Round 175 (m3 lane 5; real = fast with the inlined callees and both callers, control 22; screens on a compile-only
// first-pass harness, build/r175/fp, that reproduces the session register and the mask fold, about 1700 forms). Reading:
// retail's copy loop is the plain `for` (its count spill follows the two leas; the hand countdown spills first) and its
// opening computes both stored addresses before testing them, so the body below is not retail's source in either place.
// What blocks each: (1) with a `for` copy loop the player loop folds the mask read (`test [session + 0x111c], reg`)
// whenever the member count is a local, in every spelling of the loop (guarded, braced, `!=`, while, a named machine
// pointer, memcpy, walked output and player pointers); it stays unfolded only with the hand countdown, with the count
// read from the session at each use (a second count load, real 132), with the machine mask stored after the copy loop
// (real 133), with the summary fetched just before the player loop (real 134) or with the 0x49c8 value named before the
// loop (round 122, a stack slot). (2) The session takes EBX only with the original three-test opening and the variant
// named either immediately before the summary getter or at the top of the state block; every opening that computes the
// pair first (preset, live-and-flag, inline getter, `== NULL`, empty-then, early `return true`, the variant or the
// summary between the pair and its test) keeps ESI across about 600 combinations of variant position, summary
// position, buffer and memset order, count and address locals, index scope, mask spelling and named player pointers.
// Real: preset pair + `for` copy + a `long const *` to the 0x49c8 value named before the player loop 108 (477 bytes,
// propagated back, ESI); `for` copy + variant and summary just before the player loop 134 (479 bytes, EBX, callers
// match); `for` copy + mask stored after the copy loop 133 (495 bytes, EBX); the same on the preset pair 137 (ESI);
// preset pair + `for` copy + the count read at each use 132 (495 bytes, ESI).
// Round 162 (m3 lane 8, fast with the inlined callees and both callers; control 22, register-masked 4, all in the opening):
// diagnostics only, the variant fetched under the live-state test (0x6ec80's idiom; implied by `state == 5`, so it folds)
// before the summary getter or at the top of the state block 22/22 (same code). Reading: the copy loop's early count spill is
// part of the scratch rotation (our destination takes ECX, which holds the count; retail's takes EDX).
// Round 122 (m3 lane 9, fast with the inlined callees and both callers; control 22): `result = true` at the top of the
// state block 96 (491 bytes, both callers lost); one index shared by the copy countdown and the player loop 22 (same
// code); a walked output pointer in the player loop's increment 26; the preset pair with the variant under the
// live-state test 46; the preset pair with one shared index 46 (session ESI in both).
// Round 108 (m3 lane 10, fast with the inlined callees and both callers; control 22): the preset pair declared at
// function scope ahead of the result 46 (session ESI); member count and countdown `dword` 23 (`jbe`); the copy as
// `do { ...; i++; } while (i != member_count)` 42; the machine buffer zero-initialised in its declaration (no memset)
// 24, the player buffer the same way 148 (495 bytes). Same code: the opening in De Morgan form or with `== 0`/`== NULL`,
// the copy source through `&session->members[i]`, players declared first, no address pointer (the cast at both uses).
// M3 lane 10 r110 (fast with the inlined callees, 0x5b060 and both callers; control 22): the 0x49c8 argument through the
// existing get_value_49c8() getter 142 (503 bytes, session ESI, both callers lost); `if (session->state != 5) return false;`
// with `return true;` at the end 142 (a second epilogue for the early return); the machine mask in a named local 22 (same
// code); a walked output pointer in the copy countdown 26; a member base pointer for the copy source 22 (same code).
// Round 168 (m3 lane 7, fast with the inlined callees and both callers; control 22): the build block (summary, buffers,
// copy, player loop, set_data_4f24) as a `__forceinline` helper taking the session 218 (361 bytes: the two address
// tests fold away), as a plain `inline` helper 530 (kept out of line); the opening as a `stored` bool set from the flag
// and the two addresses 218 (361 bytes); the tail's 0x5dd0 test ahead of the live test 29; the address copy through
// walked member and address pointers 102 (477 bytes). Both callers are lost in the 218 and 530 forms.
// Round 102 (m3 lane 14, fast with 0x6e500 and 0x72700): diagnostic probes (a global load and store, reverted): between
// the opening test and the summary getter (before or after the variant) the summary and mask temporaries take retail's
// registers; at the top, before the opening or after the summary they do not, so one expression temporary is missing
// there. Forms: the two stored addresses named before the three-test opening 22 (same code); the preset pair with the
// variant between the pair and its test 152 (495 B, session ESI); the summary's result named before its cast 22; the
// machine copy through memcpy 22; the copy loop as `while (members_remaining-- > 0)` with its own index and no guard 100
// (477 B: session EBX, callers match, the copy loop and its spill are retail's; left the opening lea, the summary/mask
// rotation, the folded mask read and the player loop's argument temporaries); that loop with the preset pair 108 (every
// instruction up to the player loop retail's but the session ESI, both callers lost), with the variant before the pair
// 108, plus a members base pointer 108, a players base pointer 109, walked member and address pointers 110.
// M3 lane 8 r177 (fast with 0x6e910, 0x5c5c0 and both callers; control 22; aimed at the missing scratch temporary
// between the opening and the summary): the variant through a new inline accessor returning `session->data4db0` 22;
// the opening's two addresses through new inline accessors 22; the summary through a reference to a named result 22
// (all three the same code: address accessors fold before allocation); the summary through a new inline wrapper of
// get_data_4a00 154 (461 B, the getter stays a call; both callers lost); a plain `for` copy loop over a member count
// bound through a `long &` 100 (477 B, both callers kept).
// Round 194 (m3 lane 5, fast with 0x6e500 and 0x72700; control 22): the preset NULL pair set under the flag 46 (session
// ESI, both callers lost); on it the call's 0x49c8 value through a new inline accessor 46 and the mask test through a
// new inline `session_player_present(session, index)` 46 (accessor copies are propagated before the ranking); a const
// preset pair with the variant named after the summary getter 46; the three-test opening on the arrays
// (`!session->data4f24.data`) 22 (same code).
// Round 138 (m3 lane 12, fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; control 22): as a member of the session class
// (`this` the session, the header declaration moved into the class, both calls `a->function_06e720()`) 22 (same
// code: the session stays EBX), the member with the preset stored pair 46 (session ESI, both callers lost), so the
// `this` symbol does not break the tie; on the preset pair with the `while (remaining-- > 0)` copy (108, 477 B): the
// pair as two flag ternaries 147 (493 B, session EBX, callers match), a redundant `if (0 < member_count)` around the
// copy 108, the variant passed straight as `session->data4db0` 108 (both the same code).
// Round 198 (m3 lane 5, fast with the inlined callees and both callers; control 22): a hand countdown over walked
// address and member pointers with the count declared last (`*address = ...; member++; address++;`) 102 (477 bytes: the
// count spill follows the two leas as in retail, the mask read folds, callers match), the same on the preset pair 109
// (session ESI, both callers lost); the stored pair through a new inline getter returning bool with the outputs set only
// under the flag 46, with a `for` copy 108 (the preset pair's code in both); the variant through a new inline accessor
// with the live-state test (0x6ec80's idiom) before the summary getter 22 (same code), with a `for` copy 100.
// Compile-only screens (build/r198): the fold goes with the copy loop's scratch registers, not its spelling. Every copy
// loop that ends with retail's registers (output EDX, source EAX, count ECX: `for`, `while`, the walked countdown above)
// folds the player loop's mask read; the ones that end otherwise (the countdown on main with the output in ECX, the
// walked countdown with `address++` before `member++` and the output in EAX, `*address++ = ...`) keep it unfolded; a
// 16-trip or deleted copy loop keeps it unfolded with the session in ESI. Player-loop spellings on the `for` copy
// (do-while, `!= 0`, `1u`, an explicit continue, the slot test first, a player reference, plain `i` for the team
// argument) all fold; the variant accessor after the summary, before the count or between the loops gives ESI. So
// retail's unfolded read after its own copy registers needs one more scratch temporary between the copy loop and the
// mask test than any of these forms creates.
// Round 200 (m3 lane 5, fast with both callers; control 22): the 0x49c8 value named before the player loop on this body
// 130 (493 bytes); a named player pointer for the test and the call 25 (the loop step moves to ECX); the copy counters
// declared count first 22, a const reference to the copy source 22, the variant at the top of the state block 22 (all
// the same code).
// Round 202 (m3 lane 5, fast with 0x6e500, 0x72700, 0x5b080, 0x6d380, 0x5c5c0 and 0x6e910; control 22): the inlined
// summary getter 0x5b080 with its state read once into a local 22, with its two tests merged into one `&&` 22 (both the
// same code, the getter still matching); the tail's live and 0x5dd0 tests through a forceinline copy of the existing
// 0x5dd0 getter (0x63e00's form) 127 (451 bytes: the limit helper's live test merges; both callers lost); the two
// buffer clears swapped 26; the player buffer declared and cleared first 26.
// M3 lane 5 r207 (fast with 0x6e500 and 0x72700; control 22): the preset stored pair under the flag alone 46 (re-measured
// with the live test 46): register-masked it is retail but for one line, the copy loop's count spill ahead of the two
// leas, and the callee-saved permutation (ours session ESI, index EDI, player pointer EBX; retail index ESI, pointer EDI,
// session EBX); 1-7 dead initialised locals on that form 46 each (not a symbol-count tie); the pair set in both arms of
// the flag test 46; the opening's condition held in a named bool 218 (361 B: the address tests fold inside `&&`); the
// pair as two flag ternaries on the countdown copy 156 (499 B, callers match); the variant named between the pair and
// its test with the summary inside 152 (495 B).
// Round 116 (m3 lane 13, fast with 0x6e500, 0x72700, 0x6e910 and 0x5c5c0; control 22): the NULL-preset stored pair with a
// `const dword *` mask pointer named before the summary 46, with the member count read above the pair 136, with the
// summary fetched above the pair 55, with the variant and the summary above the pair 55; the summary fetched above the
// three-test opening with the variant after it 59 (session ESI and both callers lost in each). Re-measured: the variant
// and the summary above the opening 32. Compile-only first pass (lane 5's round 175 harness): a 0-16 dead-local sweep on
// the preset pair keeps ESI, and mask or 0x49c8 pointer locals named before the summary are propagated back (ESI), so
// the preset pair's flip is a ranking change, not a symbol-count tie.
// M3 lane 11 r129 (fast with 0x6e500, 0x72700, 0x6e910 and 0x5c5c0; control 22), aimed at retail's second opening `lea`
// placed ahead of the first branch: the two stored addresses passed to a new inline presence test taking both pointers
// 22, the same test in result style 22 (the address arguments fold before allocation, same code), the flag and both
// addresses through one such test 218 (361 bytes, both callers lost); the member count read ahead of the variant and the
// summary 126; the copy's output pointer declared with the machine buffer ahead of the summary 22 (same code).
// M3 lane 5 r210 (fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; control 22, register-masked 2): the two buffers declared
// at function scope 22 (same code); the NULL-preset stored pair in the state block re-measured 46 (masked 1, session ESI),
// with a plain `for` copy 108; the plain `for` copy alone 100 (477 B: its copy loop is then retail's exactly with the
// session in EBX; left one scratch step at the summary lea, at the player loop's mask/bit and argument temporaries and at
// the final players lea, plus the folded mask read and the opening's second lea), with the mask read into a loop local
// 100 (same code); a 600 s permuter run (context 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; without the callee
// contexts its reduced image scores the body 137) 58 tries, best 22.
// M3 lane 13 r125 (fast with 0x6e500, 0x72700, 0x6e910 and 0x5c5c0; control 22; the NULL-preset stored pair 46, aligned 51,
// register-masked 2): on the preset pair, the player loop's index renamed `player_index` 46, the copy index `member_index`
// 46, the pair renamed to sort first or last 46/46 (names do not move the session register); the member count bound by
// `long const &` 46; the slot test ahead of the mask test 59 (479 B). The opening as an out-parameter getter in this
// class's getter idiom (state range test, then the flag, the two addresses stored through the out pointers, bool result),
// called as `!get(&machines, &players) || !machines || !players` 46, with a NULL preset and the result ignored 46, with
// the live-state inline for the range test 46, without the range test 46 (the folded range test does not move it).
// M3 lane 16 r158 (fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; control 22), no-code forms aimed at the scratch
// rotation, each 22 (same code): the member count through a new trivial inline accessor; the machine mask through a
// new inline `(1 << count) - 1` helper; the mask stored through a new inline setter taking the machine buffer; the
// summary argument through a trivial inline identity; the summary passed through a `const &` to it; the copy
// countdown started from a `long const &` to the member count.
// M3 lane 8 r204 (fast with 0x6e500 and 0x72700; control 22): the summary read directly under its flag (the getter's body
// without its folded range test) 22 (same code), with a plain `for` copy 100; the opening's two addresses through two
// one-address inline getters in 0x5b080's shape (result preset 0, range test, flag, address) tested as `!first() ||
// !second()` 48 (483 B: the second flag test folds and the summary lea and mask temp take retail's EAX/EDX, but the second
// lea stays after the je and the session moves to ESI; both callers lost), with a `for` copy 110 (477 B), without the range
// test 48, with session_state_is_live() as the range test 48; on that getter opening the variant named at nine positions
// (top, state block, after the summary, the buffers, the clears, the count, the mask store, the address pointer) 48 each.
// The player loop through a copy of src/unknown_054fe0.cpp's present-player getter (pointer or NULL from the mask, then
// `player && player->unknown14 != NONE`, the call taking `player`) 110 (487 B), with a `for` copy 98 (483 B), on the getter
// opening 117 (487 B), on the getter opening with a `for` copy 107 (callers lost on the getter opening only).
// M3 lane 14 r127 (fast with 0x6e500 and 0x72700; control 22): a redundant `result = false;` at the top of the state arm
// 22; the opening as `!(flag && &machines && &players)` 22; the NULL-preset pair declared at the function top ahead of
// `result` 46; on the preset pair the first pointer read through a `const &` bound to it 46; the variant as a
// `const byte *const &` bound to the field 22 (the 22s the same code; both callers lost in each 46).
// M3 lane 6 r215 (fast with 0x6e500, 0x72700, 0x6e910 and 0x5c5c0; control 22): on the NULL-preset stored pair (re-measured
// 46: register-masked 2, the opening, the summary lea and every scratch register retail's, the session in ESI so both
// callers lost): the pair assigned players first 46; preset in an else arm instead of the initialisers 46 (both the same
// code); the 0x49c8 argument through the class's existing getter 136 (503 B: its range test re-reads the state in the
// loop); the variant read at the call with no local 49; a named player pointer for the test and the call 49 (the session
// stays in ESI in each). On the current body the scratch registers follow the opening: once the second address is formed
// in the first test's block (as retail and the pair form do), the rest of the rotation is retail's.
// M3 lane 8 r209 (fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22): the variant preset to 0 and set
// under SESSION_STATE_IS_LIVE (0x64d70's in-place spelling) 22 (same code); the mask from the field with the member count
// read after it 22; the member count read ahead of the two clears 124; the countdown started from the field 22; the
// output pointer named ahead of the mask store 22 (all the 22s the same code).
// M3 lane 8 r211 (fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22). LEADS, not kept (both need a parameter
// of 0x6e910 that its body never reads, and neither matches): (1) the plain `for` copy (100, 477 B: its copy loop is retail's)
// plus one more parameter of 0x6e910 fed `session->player_mask` 8 at 483 B, 0x6e910 and both callers matched: the link
// drops the unread argument, but the mask then has two uses in the loop body, so its read stays unfolded as in retail
// (`mov eax, [ebx + 0x111c]` ... `test eax, edx`), and the call's temporaries and the last players lea follow; left are
// the opening's second lea after the je and the summary lea / mask temp one scratch step off (build/r211/
// lead-6e720-8.patch). (2) That with the NULL-preset stored pair 36: every instruction is retail's but for the
// callee-saved permutation (ours session ESI, index EDI, player pointer EBX; retail EBX, ESI, EDI; both callers lost;
// build/r211/lead-6e720-36.patch); without the tail's live block the same form keeps the session in EBX with everything
// up to the tail retail's (diagnostic, 321 B), so the session and the loop index are one use-count step apart.
// On (1): the unread parameter fed the index 22 or the session 22 on main's countdown copy (a dropped argument does not
// move the ranking); also fed `&session->data4f24`, `&session->data4f90` or both 8 (an address is not hoisted by a second
// use); the mask through a new inline accessor 100, the bit through a new inline 100, the test through a new inline (mask,
// index) 100, `(1 << i) & mask` 100; a second mask test through a pointer-or-NULL getter at the call 100, as the test 98,
// at all three places 98, at the unknown14 read 100 (inline second uses fold before they count); a getter local for the
// test and the call 98 (483 B, a null test of its own). Openings on (1): only the second address named under the flag
// with the flag tested again (`if (flag) players = &...; if (!flag || !&machines || !players)`) 134 (session EBX, callers
// matched: retail but for an `xor ecx, ecx` preset and the summary lea in ECX), without the mask parameter 74 (477 B);
// the same for the first address 134; that local declared bare 149 (495 B); both addresses named unconditionally 8, with a
// named flag 8, in a `stored` bool set inside the flag arm 8, through a named base pointer to the flag byte 8 (all
// propagated: main's opening); the pair in an if/else 36, declared players first 36, with the flag tested again 153
// (495 B), from a named base pointer 36, as a bool getter with two out pointers 108 without the mask parameter; one
// named address tested first with the other direct 39/39; a third preset address 150 (495 B). On (2): the mask through a
// loop local for the test and the argument 36; a player pointer local 39, both 39; the session's player array named
// before the loop 37, beside the variant 37; the variant named after the summary 36, not named 39; one index for both
// loops 36; the variant named ahead of the pair in the state block 36, at the function top 36; the member array named
// before the copy 36; no addresses pointer (the cast at both uses) 36, that with no variant local 39; the member count
// read from the field at each use 132 (495 B) (none moves the session off ESI).
// M3 lane 16 r165 (fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; control 22): the two address tests joined with a
// non-short-circuit `|` 152 (495 B: both leas come ahead of the test, as negate/sbb flag arithmetic); the variant and
// the summary above the opening (round 75's 32, re-measured 32) with a plain `for` copy 109 (477 B, the mask read
// folds); the typed flag-only stored pair with the variant above it and a `for` copy 108 (both callers lost); the pair
// through a new inline getter that returns the machine address and stores the player address through an out pointer
// (live and flag test) 46; through one taking both out pointers with both preset NULL inside, the result ignored 46
// (register-masked 1 in both: only the session ESI against retail's EBX, both callers lost).
// M3 lane 15 r126 (h2m fast with 0x6e500, 0x72700, 0x6e910 and 0x5c5c0; control 22, NULL-preset stored pair 46): the
// opening's addresses as the arrays themselves (`!session->data4f24.data`) 22; the two addresses grouped as
// `!(&a && &b)` 22 (both the same code); the opening as `flag ? !&a || !&b : true` 218 (361 B: the address tests fold);
// one stored-players pointer from `flag && &a ? &b : NULL` 215 (361 B); the machine-limit helper's live test through
// session_state_is_live() 22 on this body, 46 on the preset pair; that helper reading the state once 46 on the preset
// pair (the tail's helper text does not move the session register).
// M3 lane 4 r243 (h2m fast with 0x6e500 and 0x72700; control 22): on the NULL-preset stored pair (re-measured 46: the
// opening and every scratch register retail's, the session ESI against retail's EBX, both callers lost): its test as
// two `== NULL` compares 46, as `!(machines && players)` 46, players tested first 46; the two buffers declared in the
// state block ahead of the pair 46. Diagnostic: a third, never-true term in the pair's test 152 (499 B).
// M3 lane 1 r151 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; controls 22, `for` copy 100, NULL-preset pair 46,
// preset + `for` 108): `inline` on 0x6e910's definition 22/100/46 and `static` on it 22/100/46 (same code each); the
// machine-limit helper moved beside 0x5c490 in 0x5c490's shape (only the link inlines it) 22/46/108; the preset pair with
// the summary as `flag49fd ? data4a00 : NULL` 130, with the variant passed at the call 130; a chained pair (`machines =
// flag ? &a : NULL; players = machines ? &b : NULL;`) 48, as nested ifs on a NULL preset 48 (session ESI, both callers lost).
// M3 lane 6 r227 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the NULL-preset stored pair
// re-measured 46, both callers lost): early returns in place of `result` (`if (state != 5) return false;`, `return true`)
// 142, on the pair 143; the pair tested positively with the work in an else arm 46; the summary declared ahead of the
// variant on this body 52 (the session moves to ESI, both callers lost: the session register is a declaration-order tie),
// on the pair 46; the pair under `session_state_is_live(session) && flag` 46, with the variant preset NULL and set under
// the same live test 46. Grid of the variant and summary declarations over the eight positions of the work block's first
// nine statements (63 forms each): on this opening 22 (5 forms, the same code), 120 to 132 otherwise (52 whenever the
// summary precedes the variant with the session in ESI); on the pair 46 at best (23 forms), 62/123/131 otherwise, every
// one with the session in ESI and both callers lost.
// M3 lane 7 r214 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080, all MATCH; control 22; the plain `for` copy
// re-measured 100 at 477 B: its copy loop is retail's, but the player loop's mask read folds into `test [ebx + 0x111c],
// eax` and the bit takes EAX): on the plain `for` copy, one index declared ahead of both loops 100; the mask test written
// `(mask & bit) != 0` 100; the player record named at the top of the player loop's body 100; the copy loop under main's
// `if (member_count > 0)` guard 100; the player loop as a do-while on a `player_index` 100 (all 477 B, the fold kept).
// M3 lane 8 r227 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the NULL-preset stored pair
// re-measured 46, session ESI, both callers lost): on the pair, 1 to 7 initialised dead locals at the top 46 each; the pair
// declared at the function top 46, players declared first 46, the variant declared ahead of the pair 46; a 15-form grid
// of summary before variant, one shared loop index, the addresses pointer before the clears and the member count before
// the clears 46 (or 127 with the count before the clears); without the variant local 49, without the addresses pointer
// 46, without both 49; a bool getter in 0x5b0d0's shape (both out pointers written under the live and flag tests) with
// its result tested ahead of the two pointers 46 (inline or forceinline). On this body the two address tests through an
// inline `a && b` helper 22, arguments swapped 23 (the addresses sink: the second lea stays after the `je`).
// Diagnostics (removed): `value49c8` read into a local before the player loop puts the session back in EBX with both
// callers matched (124 on the pair, 130 here, 493 B each), the player mask so 133 (493 B): the session's register
// follows its loop uses.
// M3 lane 4 r250 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the NULL-preset stored pair
// re-measured 46, session ESI, both callers lost): on the pair, the pair re-pointed at the two new buffers and passed to
// the setter 46; the pair declared at the function top ahead of `result` 46, after it 46, that with the variant at the
// top too 46; the variant declared inside the player loop's call block 49.
// M3 lane 13 r146 (h2m fast with 0x6e500, 0x72700; control 22): an identity forceinline pointer view (no code of its own)
// around one read at a time, aimed at the one-step scratch rotation: the member count's session read 133 (495 B), the mask
// store's machine buffer 130 (495 B), the summary getter's session 165 (511 B, both callers lost), the variant's session
// read 157 (506 B), the opening's flag read 150 (495 B), the copy loop's member read 132 (495 B); every placement grows the
// body by 12 B or more, so it changes more than the rotation.
// M3 lane 11 r159 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, all MATCH unless noted; control 22): `result`
// initialised from the state test (`bool result = session->state == 5; if (result)`, no `result = true`) 169; the player
// loop skipping by `if (!mask || index == NONE) continue;` 22; the opening as a positive test with an empty arm and the
// work in its else 22; the opening's address tests as `== NULL` 22; its flag test as `== false` 22; the machine mask as
// `(1u << member_count) - 1` 22 (all 22s the same code); re-measured the `short` player index 127 (0x6e500 69, 0x72700
// 94). Body unchanged.
// M3 lane 1 r162 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the NULL-preset stored pair
// re-measured 46, session ESI, both callers lost, its copy loop still one scratch step off): on the pair, the player record
// as `session->players + i` at both uses 46, the mask and slot tests as two nested ifs 46, the player buffer typed as the
// setter's 0x3648-byte block (cast at the 0x6e910 call) 46 (the three the same code), the summary passed through the
// getter at the call 146 (493 B), the copy loop walking a member pointer and an address pointer beside the countdown 50;
// on this body the two loop tests nested 22 and the 0x3648-byte player buffer 22 (same code each). Re-measured: the
// pair's NULL preset in an else arm 46.
// M3 lane 7 r223 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22): on the NULL-preset pair
// (re-measured 46) the allocator ranks the session at 95 against the player-loop index at 86 (the pair's block references
// four candidates), so the session is coloured first and takes ESI; with the index ranked first the pair form leaves 14
// (the copy loop's scratch step). On the pair: a named session player record pointer in the player loop 49; the pair set
// under the flag test alone 46; the pair assigned players first 46; the output buffer entry named 46.
// M3 lane 5 r243 (reduced image with 0x6e500, 0x72700, 0x6e910, 0x5c5c0 and 0x5b080; control 22). Reading of the
// allocator: on this opening the two opening addresses are scratch temporaries, so the EAX, ECX, EDX scratch cycle
// reaches the summary lea one step after retail's (EDX for EAX) and every later scratch pick follows. The NULL-preset
// pair makes both addresses register candidates, which restores retail's cycle, but its flag-arm block references the
// session twice among four candidates, so the session ranks 95 against the player index's 86 (85 against 86 here). With
// the index ranked first the pair leaves 14 (the copy loop: its output pointer takes ECX ahead of the member count, so the
// count's spill moves ahead of the two leas); with a `for` copy the output takes EDX but the player loop's bit takes EAX
// and the mask read folds (92 even with the ranking fixed). The player loop's latch is the only block that references the
// index and not the session, so one more candidate referenced there moves the index about 10 against the session. On the
// pair: the output buffer walked by its own pointer 54, that with the session player record walked too 54 (the latch's
// candidate count does not change), the member count bound by `long const &` 46, the countdown declared ahead of the copy
// index 46. On this opening: one stored players pointer set under `flag && &first` 215, the pair with the second address
// set under a test of the first 215 (the address tests fold).
// Roasted-Codes #1525 (released 2026-11-26; fast reduced-image scores with 0x6e910 and seven other neighbours, matched
// at every run; control 22). Their earlier 8 reached without the extra 0x6e910 parameter: the plain `for` copy plus, in
// the player loop, the mask read into a local bound by a const reference (`dword mask = session->player_mask;
// const dword &player_mask = mask;`, the test on `player_mask`) 8 (483 B); the mask read stays unfolded and what is left
// is the opening's scratch order. A one-use reference, not kept. On the flag-guarded stored pair with the `for` copy:
// the mask read once ahead of the player loop and again at the loop's end, the loop left by `if (i == 15) break;` before
// that re-read 115 (473 B; no mask spill, frame 0xeb8, one load site, the session still ESI); the re-read under `i < 15`
// with the ordinary bound kept 146 (508 B; the mask spilled, frame 0xec0). The permuter on the whole image, 93 tries:
// best 22.
// M3 lane 3 r312 (h2m fast with 0x6e500, 0x72700, 0x6e910, 0x5c5c0; control 22; the plain `for` copy with the
// reference-bound mask rebuilt 8). The NULL-preset stored pair on that 8 form gives 36: every instruction retail's
// but the callee-saved permutation (session ESI, index EDI, player pointer EBX against retail's EBX, ESI, EDI; both
// callers lost). Reading of the allocator: on main's body, the 8 form and this pair form the colouring is the same
// (session first and given the first free register, then the index, then the player pointer), yet only main's body
// and the 8 form come out with the session in EBX (the registers mapped session to EBX, index to ESI, pointer to EDI)
// and with retail's callers (`push ebx; mov ebx, esi` around the call). So the session's register is not a ranking
// tie here: it is decided after colouring, together with the callers. On the pair + `for` + reference form: the pair
// typed as the two buffers' types 36; the variant and the summary declared above the pair 45, the variant alone 36;
// the pair declared above `result` 36 (callers 69/94 in each).
// M3 lane 1 r173 (fast reduced-image screens with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22). Reading of
// the allocator with those inlined callees kept in its reduced image (without them it builds a different body, which is
// why round 312 saw one colouring for every form): the plain `for` copy with the reference-bound mask (8) ranks the player
// index 90 above the session 89 (retail's registers); the NULL-preset pair on it ranks the session 99 above the index 90
// (its flag arm references the session twice among four candidates), and a forced-ranking search moving either one past
// the other gives a byte match. Only the player loop's latch references the index and not the session. On that pair + 8
// form: the pair as two flag ternaries 156 (499 B); players assigned first 36; the pair tested positively with the work in
// the else 36; the mask read through a field pointer set before the loop 36 (propagated back), without the reference-bound
// copy 108; that pointer bound by a const reference 36 / 108; the players pointer formed from the machines pointer + 1 39;
// the variant declared ahead of the pair or at the top 36; the variant preset NULL and set under the live test, also with
// the pair under live && flag, 36 (session 99: the live test folds); the plain copy under `if (member_count > 0)` 36
// (session 97: the guard's block deducts 2), the guard doubled 36 (folds); main's countdown copy loop 46.
// M3 lane 4 r267 (fast reduced-image screens with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the 8 and 36
// forms rebuilt). Allocator ledger on the 36 form: the session's 99 includes 16 from the pair's flag arm, 16 from the
// variant's definition block and 16 + 24 from the player loop's mask and call blocks; an exact tie would go to the index.
// Forms: on the 36 form the variant read in place at the call 39 (session 105: its definition block drops 10, the call
// block gains 16), the summary declared above the variant 36, the variant declared just above the member count 36; the
// variant read in place on the 8 form 45 and on main 55 (both callers lost in each).
// M3 lane 8 r247 (fast reduced-image screens with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the 8 and 36
// forms rebuilt). Ledger: the 8 form's opening blocks give the session 2 + 2 (one candidate each), the pair's arm 16
// (two session uses among four candidates) less 2 for its test block, so the pair adds a net 10 to the session (99
// against 89, the index unchanged at 90; a tie goes to the index); the index gains over the session only in the latch
// (48), so one more candidate there or 9 fewer session points outside the loops is needed. Forms on the 36 form: the
// pair set under two separate flag tests 149 (495 B; the tests are not merged), the same with the copy under `if
// (member_count > 0)` 149; the NULL presets in an else arm 36; the pair through a file-local inline getter with
// out-parameters (bool result, outputs set under the flag) 36, a void one with NULL presets 36 (the outputs are
// promoted: the pair's code and ranking), on main's body 46/46; the member count read from the session at both uses (no
// local) 132 (495 B: reread in the copy loop), on the 8 form 138; the player loop's two tests as `continue`s 36; the
// opening test written `!(flag && &first && &second)` 22, on the 8 form 8 (same code each).
// M3 lane 11 r178 (fast screens with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the 8 and 36 forms
// rebuilt). On the 36 form: the pair with no NULL presets, set under the flag and tested `!flag || !machines ||
// !players` 153 (499 B: the two flag tests are not merged), with the flag read once into a named bool 153 (495 B); the
// pair typed as the setter's const pointers 36; the addresses pointer declared ahead of the clears 36; one index
// shared by both loops 36. Allocator reading with the callers in the image: the 36 form ranks the session 99 above the
// index 90, and a forced-ranking search gives a byte match with the session at 89 or the index at 100; the player
// loop's latch is the only block that references the index and not the session (six candidates, three of them
// constants), so one more candidate there would move the index 8 and the session 2. Body unchanged.
// M3 lane 4 r272 (fast screens with 0x6e500, 0x72700, 0x6e910, 0x5c5c0, 0x5b080; control 22; the 8 and 36 forms rebuilt).
// Reading: in the 8 form the two opening leas are scratch temporaries on the eax/ecx/edx cursor, which is why the summary
// lea gets EDX and the mask EAX; with the NULL-preset pair they are register candidates and the cursor gives retail's
// EAX/EDX. Ledger on the 36 form: session 99 (the pair's arm +16 over four candidates), index 90; one more candidate in
// the player loop's latch would give 98 against 97. On the 36 form: players assigned first 36; the presets in an else arm
// 36; the copy's source bound to a const reference 36; the shifted bit named ahead of the mask test 36; the mask in a
// plain local 108 (477 B); the variant declared at the top of the player loop body 39; the variant declared between the
// pair and its test 153 (495 B, the tests no longer threaded); an early `return false;` for the state test 144; the result
// set false in an else arm 153 (491 B).
// @retail 0x6e720
bool function_06e720(c_class_58d20 *session)
{
	bool result = false;
	if (session->state == 5)
	{
		if (!session->flag4f20 || !&session->data4f24 || !&session->data4f90)
		{
			const byte *variant = session->data4db0;
			s_session_summary *summary = (s_session_summary *)session->get_data_4a00();
			s_unknown_108 machines;
			s_session_player players[16];
			memset(&machines, 0, sizeof(machines));
			memset(players, 0, sizeof(players));
			long member_count = session->member_count;
			machines.data[0] = (1 << member_count) - 1;
			s_session_machine *addresses = (s_session_machine *)&machines.data[1];
			if (member_count > 0)
			{
				long i = 0;
				long members_remaining = member_count;
				do
				{
					addresses[i] = *(s_session_machine *)((byte *)session->members[i].words + 0xa);
					i++;
				} while (--members_remaining);
			}
			for (long i = 0; i < 16; i++)
			{
				if ((session->player_mask & (1 << i)) && session->players[i].unknown14 != NONE)
					function_6e910(&session->players[i], addresses,
						variant, summary, (char)i, &players[i], session->value49c8);
			}
			session->set_data_4f24(&machines, (s_unknown_3648 *)players);
			if (session_state_is_live(session) && session->value5dd0 != NONE)
			{
				session->set_value_4994(2);
				session_set_machine_limit(session, 1);
			}
		}
		result = true;
	}
	return result;
}

void function_148823();
void window_manager_reset_all_windows();
bool __stdcall dialog_callback_dashboard_network_configuration(long controller);
bool __stdcall dialog_callback_sign_out_to_main_menu(long controller);
bool __stdcall pause_game_quit_confirmed(long controller);
void network_session_check_tracking(c_class_58d20 *session);
void network_session_close(c_class_58d20 *session);

static inline bool session_dialog_is(c_class_1473c9 *screen, long id)
{
 bool result = false;
 if (screen && screen->screen_id >= 7 &&
     (screen->screen_id <= 8 || screen->screen_id == 0xf0))
  if (((c_dialog_screen *)screen)->dialog_id == id) result = true;
 return result;
}

// @retail 0x6d270
void function_6d270(void)
{
 function_148823();
 window_manager_reset_all_windows();
 long id = 0x38;
 dialog_choice_callback accept = dialog_callback_dashboard_network_configuration;
 bool choice = true;
 if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1 && !g_4e6948->flag134)
 {
  choice = false;
  id = 0xb8;
  accept = pause_game_quit_confirmed;
 }
 bool present = session_dialog_is(g_54d598.windows_1[4].current, id);
 if (!session_dialog_is(g_54d598.windows_1[4].next, id) && !present)
 {
  if (choice)
   dialog_choice_show(1, id, 4, (word)-1, accept, dialog_callback_sign_out_to_main_menu, 0);
  else
   dialog_ok_show(1, id, 4, (word)-1, accept, 0);
 }
 c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
 if (session->state)
 {
  network_session_check_tracking(session);
  network_session_close(session);
 }
 session = (c_class_58d20 *)g_527330.session_b;
 if (session->state)
 {
  network_session_check_tracking(session);
  network_session_close(session);
 }
}


struct s_session_join_request;
long network_session_evaluate_join_request(c_class_58d20 *session, const s_session_join_request *request);
bool function_630f0(c_class_58d20 *session, long address, const s_session_join_request *request, long reason);

// @retail 0x6dc60 standard
void __stdcall function_06dc60(c_session_client *client, long reason)
{
 s_session_request *request = client->requests;
 while (request)
 {
  s_session_request *next = request->next;
  long rejection = reason;
  if (!rejection)
   rejection = network_session_evaluate_join_request(client->session,
    (const s_session_join_request *)&request->remote);
  function_630f0(client->session, (long)request->key04,
   (const s_session_join_request *)&request->remote, rejection);
  session_client_remove_request(client, request);
  request = next;
 }
}

long function_19989d(void);

// @retail 0x6cad0
void __stdcall function_6cad0(long unused)
{
 long const *argument_reference = &unused;
 if (g_467214 != NONE)
  function_6d270();
 else
 {
  long state = function_19989d();
  if ((state == 2 || state == 3) && g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 2)
  {
   c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
   if (session->state)
   {
    network_session_check_tracking(session);
    network_session_close(session);
   }
   session = (c_class_58d20 *)g_527330.session_b;
   if (session->state)
   {
    network_session_check_tracking(session);
    network_session_close(session);
   }
   dialog_ok_show(1, 0x3a, 4, (word)-1, 0, 0);
  }
 }
}


struct s_match_result_data
{
 XNKEY key;
 XNKID id;
 XNADDR address;
 DWORD public_filled, public_open, private_filled, private_open;
 long properties[7];
};
struct s_qos_target
{
 XNKID kid;
 XNKEY key;
 XNADDR xna;
};
#include "network_qos.h"
bool function_90160(long task_index, s_match_result_data *result);
long online_match_session_find(XNKID const *session_id);
long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets);
bool qos_is_complete(long handle);
bool session_description_decode_buffer(long size, const byte *data, void *description);

PRIVATE __forceinline bool function_6f4b1(const s_qos_result *arg_0, s_session_description *arg_1)
{
 return session_description_decode_buffer(arg_0->data_size, arg_0->data, arg_1);
}

// @retail 0x6f4b0
void function_06f4b0(c_session_state_joining *self)
{
 s_session_state_joining_view *state = (s_session_state_joining_view *)self;
 if (state->unknowne9)
 {
  state->unknown104 = 1;
  return;
 }
 if (state->unknownf0 != NONE)
 {
  switch (online_task_poll(state->unknownf0))
  {
  case 0: goto local_1;
  case 1: return;
  case 2:
   {
    s_match_result_data result;
    if (function_90160(state->unknownf0, &result))
    {
     s_qos_target target;
     target.kid = result.id;
     target.key = result.key;
     target.xna = result.address;
     state->unknownf4 = qos_lookup(1, 1, g_network_configuration.value18c, &target);
     if (state->unknownf4 == NONE)
      state->unknown104 = 4;
    }
    else
     state->unknown104 = 3;
   }
   break;
  default: break;
  }
  function_6b640(state->unknownf0);
  state->unknownf0 = NONE;
 }
 else if (state->unknownf4 != NONE)
 {
  if (!qos_is_complete(state->unknownf4))
   return;
  s_qos_result result;
  if (qos_target_result(state->unknownf4, 0, &result))
  {
   struct { s_session_description description; byte remaining[0x714 - sizeof(s_session_description)]; } buffer;
   s_session_description *description = &buffer.description;
   if (function_6f4b1(&result, description))
   {
    self->function_06f2b0(description, state->entry_count);
    state->unknown10 = false;
    state->unknown11 = false;
    if (!state->unknown104)
    {
     state->part.unknown40 = description->unknown10;
     state->part.unknown00 = description->unknown02;
     *(XNKID *)state->part.unknown04 = description->kid;
     *(XNKEY *)state->part.unknown0c = description->key;
     *(XNADDR *)state->part.unknown1c = description->address;
     state->unknown68 = true;
    }
   }
   else
    state->unknown104 = 6;
  }
  else
   state->unknown104 = 5;
  qos_release(state->unknownf4);
  state->unknownf4 = NONE;
 }
 else
 {
  state->unknownf0 = online_match_session_find((const XNKID *)(state->target + 0x28));
  if (state->unknownf0 == NONE)
   state->unknown104 = 16;
 }
local_1:
 return;
}

bool __stdcall function_59e50(c_class_58d20 *session, long mode, long local,
 const XNKID *kid, const XNKEY *key, const s_session_member_identity *host,
 long count, const dword *identities, const long *values, const long *other_values,
 bool reserve, long timeout, const s_session_id *id, const void *extra);

// Copies the session's player identities into an aligned local table: by index with the count incremented after
// each copy when all players are taken, otherwise the stored entries in one copy whose byte size is bound to a
// const long & (that binding keeps the count in its register across the copy, as retail does).
// @retail 0x6fe90 standard
bool __stdcall function_6fe90(c_session_state_joining *state, bool all_players, bool reserve)
{
 c_class_58d20 *current = state->owner->session_a;
 c_class_58d20 *joining = state->owner->session_c;
 long count = 0;
 network_session_close(joining);
 __declspec(align(8)) dword identities[16][3];
 memset(identities, 0, sizeof(identities));
 if (all_players)
 {
  for (long i = 0; i < 16; i++)
   if (current->player_mask & (1 << i))
   {
    memcpy(identities[count], &current->players[i], 12);
    count++;
   }
 }
 else
 {
  count = *(long *)((byte *)state + 0xb0);
  const long &bytes = count * 12;
  memcpy(identities, (byte *)state + 0xb4, bytes);
 }
 long first[16];
 long second[16];
 for (long j = 0; j < 16; j++)
 {
  first[j] = NONE;
  second[j] = NONE;
 }
 s_session_id id;
 memset(&id, 0, sizeof(id));
 if (function_59e50(joining, *(long *)((byte *)state + 0xac), *(long *)((byte *)state + 0x6c),
  (const XNKID *)((byte *)state + 0x70), (const XNKEY *)((byte *)state + 0x78),
  (const s_session_member_identity *)((byte *)state + 0x88), count, identities[0], first, second,
  reserve, g_network_configuration.value180, &id, 0))
  return true;
 if (SESSION_STATE_IS_LIVE(current->state))
 {
  if (current->function_058d20())
   network_session_set_mode(current, 1);
 }
 *(long *)((byte *)state + 0x104) = 16;
 return false;
}

// @retail 0x6fcc0
void function_06fcc0(c_session_state_joining *self)
{
 s_session_state_joining_view *state = (s_session_state_joining_view *)self;
 if (state->unknowne9)
  state->unknown104 = 1;
 else if (!state->unknown68)
  state->unknown104 = 1;
 else
 {
  if (!state->unknownf9)
  {
   if (function_6fe90(self, false, true))
    state->unknownf9 = true;
  }
  if (state->unknownf9)
   session_state_joining_check_target(self);
 }
}

// @retail 0x6f940
void function_6f940(c_session_state_joining *self)
{
 s_session_state_joining_view *state = (s_session_state_joining_view *)self;
 c_class_58d20 *session = state->owner->session_a;
 state->unknowne8 = true;
 long value = session->state;
 if (value == 5 || value == 6 || value == 7 || value == 8)
 {
  if (session_state_is_live(session) && session->flag5dd8)
  {
   memcpy(&state->part, session->data5ddc, 0x44);
   state->unknown68 = true;
   if (function_6fe90(self, true, true))
    network_session_set_mode(session, 16);
  }
  else
   state->unknown104 = 16;
 }
 else
 {
  volatile long unused = value;
 }
}


bool network_session_players_match(c_class_58d20 *session, c_class_58d20 *other);

// @retail 0x6fa60
void function_6fa60(c_session_state_joining *self)
{
 s_session_state_joining_view *state = (s_session_state_joining_view *)self;
 c_class_58d20 *current = state->owner->session_a;
 c_class_58d20 *joining = state->owner->session_c;
 state->unknowne8 = true;
 if (current->type == 17)
 {
  if (current->function_058d20())
  {
   if (session_state_is_live(joining))
   {
    if (network_session_players_match(current, joining))
    {
     network_session_set_mode(current, 18);
     state->unknown100 = g_510548 ? g_51054c : GetTickCount();
    }
    else if (function_75890(state->unknownfc) > g_network_configuration.value1b8)
     state->unknown104 = 16;
   }
   else
    state->unknown104 = 16;
  }
  else
  {
   if (!joining->state && session_state_is_live(current) &&
    network_session_get_data5ddc(current, &state->part))
   {
    long local = current->current_member;
    state->entry_count = 0;
    long i = 0;
    do
    {
     if ((current->player_mask & (1 << i)) && current->players[i].member_index == local)
     {
      memcpy(state->entries + state->entry_count * 12, &current->players[i], 12);
      state->entry_count++;
     }
     i++;
    } while (i < 16);
    state->unknown68 = true;
    function_6fe90(self, false, false);
   }
  }
 }
}

// Enabling this queue processor currently changes shared call conventions.
#if 0
long network_session_find_player(c_class_58d20 *session, const dword *identity);
struct s_surface_description;
struct s_matchmaking_ratings;
s_surface_description *function_192e60(long index);
long function_193250(s_surface_description *variant);
bool function_7e210(c_class_58d20 *session, s_matchmaking_ratings *ratings);
long session_summary_find_machine_user(s_session_summary *summary, const XUID *user);
bool session_summary_add_machine(s_session_summary *summary, s_session_id *id, const XUID *machine_user,
 const long *values288, long player_count, XUID *players, const long *values248);
bool __stdcall function_5c720(c_class_58d20 *session, const s_session_member_identity *identity,
 long player_count, const dword *identities, const long *values, bool reserve,
 long timeout, const s_session_id *id, long *reason);

// Disabled implementation for 0x6d4c0.
void function_6d4c0(c_session_client *client)
{
 c_class_58d20 *session = client->session;
 s_session_summary *summary = 0;
 if (session_state_is_live(session) && session->flag49fd)
  summary = (s_session_summary *)session->data4a00;
 long state = session->state;
 if (!(state == 5 || state == 6 || state == 7 || state == 8))
 {
  volatile long unused = state;
  function_06dc60(client, 3);
  return;
 }
 if (function_058d90(session) || session->state == 7 || session->state == 6 || session->state == 8 || !summary)
 {
  function_06dc60(client, 3);
  return;
 }
 s_session_request *request = client->requests;
 while (request)
 {
  s_session_request *next = request->next;
  byte *data = (byte *)request;
  if (!*(bool *)(data + 0x164))
  {
   long count = *(long *)(data + 0x18);
   long reason = 0;
   if (count <= 0)
    reason = 12;
   else
   {
    for (long i = 0; i < count; i++)
    {
     if (function_063190(summary, (long)(data + 0x1c + i * 12)) == NONE)
     {
      reason = 11;
      break;
     }
    }
    if (!reason)
     reason = network_session_evaluate_join_request(client->session, (s_session_join_request *)(data + 0x18));
   }
   function_630f0(client->session, (long)request->key04, (s_session_join_request *)(data + 0x18), reason);
   session_client_remove_request(client, request);
  }
  request = next;
 }
 if (client->mode == 1)
  function_06dc60(client, 3);
 if (client->mode == 2)
 {
  s_session_summary current = *summary;
  for (long machine = 0; machine < current.machine_count; machine++)
  {
   if (memcmp(&client->session->members[client->session->current_member].id, &current.machine_ids[machine], sizeof(s_session_id)))
   {
    long now = g_510548 ? g_51054c : GetTickCount();
    if (now - current.machine_times[machine] >= g_network_configuration.value360)
    {
     for (long player = 0; player < current.player_count; player++)
     {
      if (current.player_machines[player] == machine &&
       network_session_find_player(client->session, (const dword *)&current.player_users[player]) == NONE)
      {
       function_06d380(client, &current.machine_ids[machine]);
       break;
      }
     }
    }
   }
  }
 }
 if (client->mode != 2) return;
 session = client->session;
 s_surface_description *variant = function_192e60(session_state_is_live(session) ? session->value49c8 : NONE);
 while (client->request_count > 0)
 {
  session = client->session;
  long reserved = 0;
  for (long r = 0; r < 16; r++)
  {
   byte *reservation = (byte *)session + 0x7668 + r * 0x24;
   if (reservation[0] && !reservation[1]) reserved++;
  }
  if (session->value4994 - session->player_count - reserved <= 0) break;
  summary = 0;
  if (session_state_is_live(session) && session->flag49fd)
   summary = (s_session_summary *)session->data4a00;
  __declspec(align(8)) byte ratings[0x364];
  bool valid = function_7e210(session, (s_matchmaking_ratings *)ratings);
  request = client->requests;
  while (request)
  {
   s_session_request *next = request->next;
   byte *data = (byte *)request;
   long reason = 0;
   if (!valid || !summary) reason = 3;
   else if (*(long *)(data + 0x16c) != 2) reason = 10;
   else if (*(long *)(data + 0x18) > function_193250(variant)) reason = 3;
   else if (!(*(dword *)(ratings + 0x35c) & (1 << (*(long *)(data + 0x18) - 1)))) reason = 4;
   else if (*(long *)variant == 5 && !memcmp(data + 0x18c, g_440070, 12)) reason = 14;
   else if (*(long *)variant == 5 && session_summary_find_machine_user(summary, (XUID *)(data + 0x18c)) != NONE) reason = 15;
   else reason = network_session_evaluate_join_request(client->session, (s_session_join_request *)(data + 0x18));
   if (reason)
   {
    function_630f0(client->session, (long)request->key04, (s_session_join_request *)(data + 0x18), reason);
    session_client_remove_request(client, request);
   }
   else
   {
    *(long *)(data + 0x1c4) = 0;
    long score = *(long *)(data + 0x178) * g_network_configuration.value368;
    if (score < 0) score = 0;
    else if (score > g_network_configuration.value364) score = g_network_configuration.value364;
    *(long *)(data + 0x1c4) = score;
    if (*(dword *)(ratings + 0x360) & (1 << (*(long *)(data + 0x18) - 1)))
     *(long *)(data + 0x1c4) += g_network_configuration.value36c;
    *(long *)(data + 0x1c4) -= abs(*(long *)(data + 0x180) - *(long *)ratings) * g_network_configuration.value370;
   }
   request = next;
  }
  s_session_request *best = 0;
  for (request = client->requests; request; request = request->next)
   if (!best || *(long *)request->unknown1c4 > *(long *)best->unknown1c4) best = request;
  if (!best) break;
  s_session_summary current = *summary;
  byte *data = (byte *)best;
  if (session_summary_add_machine(&current, (s_session_id *)(data + 0x15c), (XUID *)(data + 0x18c),
   (long *)(data + 0x11c), *(long *)(data + 0x18), (XUID *)(data + 0x1c), (long *)(data + 0xdc)))
  {
   network_session_host_set_summary(client->session, &current);
   session = client->session;
   long reason = 0;
   if (!function_5c720(session, (s_session_member_identity *)(data + 0x1a0), *(long *)(data + 0x18),
    (dword *)(data + 0x1c), (long *)(data + 0xdc), *(bool *)(data + 0x164), *(long *)(data + 0x168),
    (s_session_id *)(data + 0x15c), &reason))
   {
    long reply[3];
    memset(reply, 0, sizeof(reply));
    reply[0] = session->unknown1c; reply[1] = session->unknown20; reply[2] = reason;
    function_07b140(session->unknown04, (long)best->key04, 10, sizeof(reply), reply);
   }
  }
  else
  {
   session = client->session;
   long reply[3];
   memset(reply, 0, sizeof(reply));
   reply[0] = session->unknown1c; reply[1] = session->unknown20; reply[2] = 3;
   function_07b140(session->unknown04, (long)best->key04, 10, sizeof(reply), reply);
  }
  session_client_remove_request(client, best);
 }
}

#endif

// @retail 0x6f700
void function_06f700(c_session_state_joining *self)
{
 s_session_state_joining_view *state = (s_session_state_joining_view *)self;
 bool cancelled = state->unknowne9;
 c_class_58d20 *session = state->owner->session_a;
 if (cancelled)
 {
  bool local = false;
  if (session_state_is_live(session)) local = session->current_member == session->value50;
  if (local)
  {
   if (session->type == 1)
    state->unknown104 = 1;
   else
    session_state_joining_set_mode(self);
  }
  else
   goto failed;
 }
 else
 {
  switch (session->type)
  {
  case 15: function_6f940(self); return;
  case 16: session_state_joining_request_host_mode(self); return;
  case 17: function_6fa60(self); return;
  case 18: session_state_joining_check_ready(self); return;
  }
  bool local = false;
  if (session_state_is_live(session)) local = session->current_member == session->value50;
  if (!local)
  {
   state->unknown104 = session->type == 1 ? 1 : 16;
   return;
  }
  if (!state->unknowne8)
  {
   session_state_joining_send_target(self);
   return;
  }
failed:
  state->unknown104 = 16;
 }
}
