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
};
long __stdcall function_063190(void *summary, long user);
long session_summary_get_player_value(s_session_summary *summary, long player_index, long variant_index);
long network_session_find_member(c_class_58d20 *session, const s_session_member_identity *identity);
void network_session_disband_member(c_class_58d20 *session, long member_index);
__declspec(noinline) long network_session_cancel_reservations(c_class_58d20 *session, const s_session_id *id);
bool session_summary_remove_machine(s_session_summary *summary, s_session_id *id);
bool network_session_host_set_summary(c_class_58d20 *session, const s_session_summary *summary);

// score 9 (was 9): Frame and both scans agree; final summary address is reused rather than rematerialized.
// only the summary address still differs: retail recomputes `lea eax,[esp+0x24]`
// after the push, ours reuses the `ebx` copy of `&summary`.
// Round 296: Explicit inline/noinline call-summary subsets for cancellation/removal/publication (all seven combinations) 9 each; named removal result 9. Complete body and declarations restored.
// Round 44: post-remove pointer/reference 9 each; read-only source 9; one-element summary array 9;
// output-pointer copy 9; direct tail session reads 9; final-call session local 9;
// direct summary initialization 9. Opening body restored.
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
	c_class_58d20 *session = client->session;
	s_session_summary *source = 0;
	if (session_state_is_live(session) && session->flag49fd)
		source = (s_session_summary *)session->data4a00;
	if (source)
	{
		s_session_summary summary = *source;
		if (session_summary_remove_machine(&summary, (s_session_id *)id))
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

// score 55 (was 55): aligned frame agrees; the code is retail's apart from the colours. Retail takes the
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
// Round 30 (lane 13, fast): the call's variant address and 0x49c8 value both named before the player
// loop 130 (493 bytes): the session moves to EBX with retail's index ESI and player EDI, but the value
// takes a stack slot (frame 0xec0); both named after the summary getter 71, before the member count
// read 65 (483 bytes, the same registers; left the value slot, its store/load and the folded mask
// test); the value alone before the loop 137; the variant alone before the member count 52. Named
// copies inside the loop or a `long const *` to the value are propagated back (session ESI).
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
// @retail 0x6e720
bool function_06e720(c_class_58d20 *session)
{
	bool result = false;
	if (session->state == 5)
	{
		if (!session->flag4f20 || !&session->data4f24 || !&session->data4f90)
		{
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
						session->data4db0, summary, (char)i, &players[i], session->value49c8);
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

// score 9 (was 9): the four unrolled copies of the member's twelve bytes now match
// retail; what is left is that our destination's base address is computed before the
// branch, so the flag byte is loaded into cl instead of al and the player mask into
// ecx instead of eax, while retail computes the base after the mask has been stored.
// Tried: the copies through a struct, a local and byte or dword source pointers (all
// 40); the count incremented separately (244), before the copy (331) or through a
// running destination pointer (9, kept); the destination pointer assigned inside the
// branch (195) and a local player mask (359).
// source and the destination in the other registers, and the two stores that
// follow are scheduled in the other order.
// Retail aligns its frame; 0x6fcc0, 0x6f940 and 0x6fa60 push all three arguments.
// The body still differs; this boundary preserves those callers.
// destination in the other registers, ours copies from esi to ecx and retail
// from ecx to esi, and the count's increment sits before the loop counter's
// reload in ours and after it in retail
// Round 15: the destination declared or assigned inside the branch (also at function top) 195/195/195 (the count
// leaves ebx for the state pointer); with a mask local 357; indexed destination `identities[count]` 195, through a
// destination local 195, `identities[count++]` 40 (retail's setup, the copy's registers swapped); the destination
// set before the memset 216; the destination inside the branch with the count first/last 203/216; the count
// after the pointer step 17.
// @retail 0x6fe90 standard
bool __stdcall function_6fe90(c_session_state_joining *state, bool all_players, bool reserve)
{
 c_class_58d20 *current = state->owner->session_a;
 c_class_58d20 *joining = state->owner->session_c;
 long count = 0;
 network_session_close(joining);
 __declspec(align(8)) dword identities[16][3];
 memset(identities, 0, sizeof(identities));
 dword *dest = identities[0];
 if (all_players)
 {
  for (long i = 0; i < 16; i++)
   if (current->player_mask & (1 << i))
   {
    memcpy(dest, &current->players[i], 12);
    count++;
    dest += 3;
   }
 }
 else
 {
  count = *(long *)((byte *)state + 0xb0);
  memcpy(identities, (byte *)state + 0xb4, count * 12);
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
