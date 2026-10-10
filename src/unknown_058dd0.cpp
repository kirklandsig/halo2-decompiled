// @flags /O2 /Gr
/* UNKNOWN_058DD0.CPP: the game-session state machine states */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"
#include "unknown_0662e0.h"
#include <xtl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void function_07ad80(long count, byte *buffer);
bool network_session_host_leave_to_peer(c_class_58d20 *session, long peer_index);
struct s_surface_description;
s_surface_description *function_192e60(long index);


#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

/* ---- names ---- */

// @retail 0x58dd0
const char *c_session_state_none::function_58dd0()
{
	return "none";
}

// @retail 0x58de0
const char *c_session_state_pre_game::function_58dd0()
{
	return "pre-game";
}

// @retail 0x58df0
const char *c_session_state_start_match::function_58dd0()
{
	return "start-match";
}

// @retail 0x58e00
const char *c_session_state_start_game::function_58dd0()
{
	return "start-game";
}

// @retail 0x58e10
const char *c_session_state_in_match::function_58dd0()
{
	return "in-match";
}

// @retail 0x58e20
const char *c_session_state_in_game::function_58dd0()
{
	return "in-game";
}

// @retail 0x58e30
const char *c_session_state_post_game::function_58dd0()
{
	return "post-game";
}

// @retail 0x58e40
const char *c_session_state_joining::function_58dd0()
{
	return "joining";
}

// @retail 0x58e50
const char *c_session_state_matchmaking::function_58dd0()
{
	return "matchmaking";
}

// @retail 0x58e60
const char *c_session_state_post_match::function_58dd0()
{
	return "post-match";
}

/* ---- the default enter, shared by post-match, none and post-game ---- */

// @retail 0x6e1b0
void c_session_state::enter(long a, long b, long c)
{
	if (!skip_cleanup)
	{
		c_class_58d20 *s = owner->session_b;
		if (s->state != 0 && !function_058d90(s))
		{
			network_session_leave(s, false);
		}
	}
}

/* ---- none ---- */

// @retail 0x6e130
bool c_session_state_none::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *s = o->session_a;
	if (s->state != 0 && !function_058d90(s))
	{
		o->data_size = 0;
		o->failed = true;
		o->error_code = 1;
		long *d = o->data;
		*d = 0;
		if (o->data_size > 0)
		{
			memcpy(d, 0, o->data_size);
		}
	}
	else
	{
		s = o->session_b;
		if (s->state != 0 && !function_058d90(s))
		{
			network_session_leave(s, false);
			return false;
		}
	}
	return false;
}

/* ---- pre-game ---- */

// @retail 0x6e1e0
bool c_session_state_pre_game::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	bool result = function_06dfa0();
	if (!result && !o->failed)
	{
		switch (a->type)
		{
		case 0:
			if (a->state != 1)
			{
				function_06df60(o, 0, 0, 0);
			}
			break;
		case 1:
			if (function_058d50(a))
			{
				result = function_06e360();
			}
			if (a->function_058d20())
			{
				result = function_06e410();
			}
			break;
		case 2:
		case 3:
		case 4:
			if (function_058d70(a))
			{
				long kind = a->members[a->current_member].unknown88;
				if (kind == 3 || kind == 4)
				{
					function_06df60(o, 2, 0, 0);
				}
			}
			break;
		case 5:
			function_06df60(o, 4, 0, 0);
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			function_06df60(o, 6, 0, 0);
			break;
		case 15:
			function_06df60(o, 5, 0, 0);
			break;
		default:
			__assume(0);
		}
	}
	return result;
}

// @retail 0x6e310
void c_session_state_pre_game::enter(long a, long b, long c)
{
	if (!skip_cleanup)
	{
		c_class_58d20 *s = owner->session_b;
		if (s->state != 0 && !function_058d90(s))
		{
			network_session_leave(s, false);
		}
	}
	unknown18 = NONE;
	unknown14 = 0;
	unknown1c = 0;
}

/* ---- start-game ---- */

// @retail 0x6e500
bool c_session_state_start_game::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	bool result = function_06dfa0();
	if (!result && !o->failed && SESSION_STATE_IS_LIVE(a->state))
	{
		if (a->type == 4)
		{
			function_06df60(o, 3, 0, 0);
			return result;
		}
		if (a->type == 2)
		{
			if (a->function_058d20() && function_06e720(a) && function_06e6b0(a, &flag10))
			{
				network_session_stop_countdown(a);
				network_session_set_mode(a, 4);
				result = true;
			}
		}
		else
		{
			function_06df60(o, 1, 0, 0);
		}
	}
	return result;
}

// @retail 0x6e5d0
void c_session_state_start_game::enter(long a, long b, long c)
{
	c_class_58d20 *s = owner->session_a;
	if (!skip_cleanup)
	{
		c_class_58d20 *t = owner->session_b;
		if (t->state != 0 && !function_058d90(t))
		{
			network_session_leave(t, false);
		}
	}
	function_06e620(s);
	flag10 = 0;
}

/* ---- in-game ---- */

// @retail 0x6ea20
bool c_session_state_in_game::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	bool result = function_06dfa0();
	if (!result && !o->failed && SESSION_STATE_IS_LIVE(a->state))
	{
		if (a->type == 4)
		{
			if (function_0682c0())
			{
				if (a->function_058d20())
				{
					network_session_set_mode(a, 1);
					result = true;
				}
				else
				{
					network_session_leave(a, false);
					result = true;
				}
				return result;
			}
			if (a->function_058d20())
			{
				if (function_138a10())
				{
					network_session_set_mode(a, 5);
					result = true;
				}
				else if (a->get_value_49c4())
				{
					function_1388e0();
				}
			}
			if (function_138800())
			{
				s_game_options_view *options = g_4e6948;
				if (options->id_a == id_a && options->id_b == id_b)
				{
					long x = NONE;
					long y = NONE;
					byte *z;
					if (a->get_values_4d08(&x, &y, &z))
					{
						if (options->position_a != x || options->position_b != y)
						{
							function_06ec10(a);
						}
					}
				}
			}
		}
		else
		{
			function_06df60(o, a->type == 5 ? 4 : 1, 0, 0);
		}
	}
	return result;
}

// @retail 0x6eb90
void c_session_state_in_game::enter(long a, long b, long c)
{
	c_class_58d20 *s = owner->session_a;
	if (!skip_cleanup)
	{
		c_class_58d20 *t = owner->session_b;
		if (t->state != 0 && !function_058d90(t))
		{
			network_session_leave(t, false);
		}
	}
	function_06ec10(s);
}

// @retail 0x6ebe0
void c_session_state_in_game::leave(long a)
{
	s_game_options_view *options = g_4e6948;
	if (options && options->flag1120 && options->id_a == id_a && options->id_b == id_b)
	{
		function_068750();
	}
}

/* ---- in-match ---- */

// @retail 0x729a0
bool c_session_state_in_match::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *b = o->session_b;
	c_class_58d20 *a = o->session_a;
	bool result = function_06dfa0();
	if (!result && !o->failed && SESSION_STATE_IS_LIVE(b->state))
	{
		if (b->type == 4)
		{
			if (function_0682c0())
			{
				network_session_leave(b, false);
				if (a->function_058d20())
				{
					network_session_set_mode(a, 1);
				}
				else
				{
					network_session_leave(a, false);
				}
				return true;
			}
			if (function_138a10() && b->function_058d20())
			{
				network_session_set_mode(b, 5);
				return true;
			}
		}
		else if (b->type == 5)
		{
			function_06df60(o, 9, 0, 0);
			return true;
		}
		else
		{
			network_session_leave(b, false);
			result = true;
		}
	}
	return result;
}

// @retail 0x72a90
void c_session_state_in_match::enter(long a, long b, long c)
{
	c_class_58d20 *s = owner->session_b;
	if (!skip_cleanup && s->state != 0 && !function_058d90(s))
	{
		network_session_leave(s, false);
	}
	id_a = NONE;
	id_b = NONE;
	if (function_06ec80(s, true))
	{
		s_game_options_view *options = g_4e6948;
		id_a = options->id_a;
		id_b = options->id_b;
	}
	else
	{
		network_session_leave(s, false);
	}
}

// @retail 0x72b00
void c_session_state_in_match::leave(long a)
{
	s_game_options_view *options = g_4e6948;
	if (options && options->flag1120 && options->id_a == id_a && options->id_b == id_b)
	{
		function_068750();
	}
	if (g_510548)
	{
		time = g_51054c;
	}
	else
	{
		time = GetTickCount();
	}
}

/* ---- start-match ---- */

void function_72950(c_class_58d20 *session);

// @retail 0x728b0
void c_session_state_start_match::enter(long a, long b, long c)
{
	c_class_58d20 *s = owner->session_b;
	if (!skip_cleanup && s->state != 0 && !function_058d90(s))
	{
		network_session_leave(s, false);
	}
	function_72950(s);
	mode = 3;
	unknown14 = 0;
	unknown18 = 0;
}

#pragma inline_depth(0)
// @retail 0x72900
void c_session_state_start_match::leave(long a)
{
	c_class_58d20 *s = owner->session_a;
	if (mode == 3)
	{
		mode = 1;
	}
	long state = s->state;
	if (state == 5 || state == 6 || state == 7 || state == 8)
	{
		network_session_host_set_value49f8(s, mode);
	}
	else
	{
		volatile long local_0 = state;
	}
}
#pragma inline_depth(255)

/* ---- matchmaking ---- */

// @retail 0x70a20
void c_session_state_matchmaking::enter(long a, long b, long c)
{
	c_class_58d20 *s = owner->session_a;
	if (!skip_cleanup)
	{
		c_class_58d20 *t = owner->session_b;
		if (t->state != 0 && !function_058d90(t))
		{
			network_session_leave(t, false);
		}
	}
	unknown968 = 0;
	unknowna7c = 0;
	unknown970 = 0;
	unknowna68 = 0;
	if (g_510548)
	{
		time = g_51054c;
	}
	else
	{
		time = GetTickCount();
	}
	mode = 3;
	long state = s->state;
	bool live = false;
	if (state == 5 || state == 6 || state == 7 || state == 8)
	{
		live = true;
	}
	else
	{
		volatile long local_0 = state;
	}
	flag10 = live;
	memset(unknown14, 0, sizeof(unknown14));
}

#pragma inline_depth(0)
// @retail 0x70ad0
void c_session_state_matchmaking::leave(long a)
{
	c_class_58d20 *s = owner->session_a;
	if (mode == 3)
	{
		mode = 1;
	}
	if (flag97c)
	{
		function_090c80(&flag97c);
	}
	if (flaga78)
	{
		function_070c50(mode == 2);
	}
	if (flaga64)
	{
		function_070d20(mode == 2);
	}
	long state = s->state;
	if (state == 5 || state == 6 || state == 7 || state == 8)
	{
		network_session_host_set_value49f8(s, mode);
	}
	else
	{
		volatile long local_0 = state;
	}
}
#pragma inline_depth(255)

/* ---- post-match ---- */

// @retail 0x72b50
bool c_session_state_post_match::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	c_class_58d20 *b = o->session_b;
	bool result = function_06dfa0();
	if (!result && !o->failed)
	{
		if (b->type == 5 && (a->type == 13 || a->type == 12))
		{
			if (!SESSION_STATE_IS_LIVE(b->state) || !a->function_058d20())
			{
				return result;
			}
			long index = b->member_index;
			s_session_snapshot snapshot;
			s_session_snapshot compare;
			memset(&snapshot, 0, sizeof(snapshot));
			snapshot.unknown40 = 2;
			network_session_get_key(b, (s_session_id *)snapshot.unknown04, snapshot.unknown0c, NULL, &snapshot.unknown00);
			memcpy(snapshot.words, b->members[index].words, sizeof(snapshot.words));
			if (!network_session_get_data5ddc(a, (s_parameters_part *)&compare) || memcmp(&compare, &snapshot, sizeof(snapshot)) != 0)
			{
				network_session_host_set_data5ddc(a, (const s_parameters_part *)&snapshot);
			}
			return result;
		}
		network_session_leave(b, false);
		return true;
	}
	return result;
}

/* ---- joining ---- */

// @retail 0x6fd10
bool c_session_state_joining::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *c = o->session_c;
	c_class_58d20 *a = o->session_a;
	if (unknown104 == 0)
	{
		if (flag10)
		{
			function_06f4b0(this);
		}
		if (unknown104 == 0)
		{
			if (!flag10)
			{
				if (a->state != 0)
				{
					function_06f700(this);
				}
				else
				{
					function_06fcc0(this);
				}
			}
		}
	}
	if (unknown104 != 0)
	{
		long state = a->state;
		if (state == 5 || state == 6 || state == 7 || state == 8)
		{
			network_session_set_mode(a, 1);
		}
	}
	if (flagf8)
	{
		c_class_58d20 *swap = o->session_a;
		o->session_a = o->session_c;
		o->session_c = swap;
		c = swap;
		unknown104 = 2;
	}
	if (unknown104 != 0)
	{
		network_session_leave(c, false);
		o->failed = true;
		o->error_code = 1;
		o->data_size = 0;
		long *d = o->data;
		*d = 0;
		if (o->data_size > 0)
		{
			memcpy(d, 0, o->data_size);
		}
	}
	return false;
}

// @retail 0x6fe20
void c_session_state_joining::enter(long a, long b, long c)
{
	if (!skip_cleanup)
	{
		c_class_58d20 *s = owner->session_b;
		if (s->state != 0 && !function_058d90(s))
		{
			network_session_leave(s, false);
		}
	}
	unknownfc = 0;
	unknown100 = 0;
	unknown104 = 0;
}

// @retail 0x6fe80
void c_session_state_joining::leave(long a)
{
	function_06f0f0();
}

/* ---- post-game ---- */

#pragma inline_depth(0)
// @retail 0x6f050
bool c_session_state_post_game::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	bool result = function_06dfa0();
	if (!result && !o->failed && a->type != 5)
	{
		function_06df60(o, 1, 0, 0);
		result = true;
	}
	return result;
}
#pragma inline_depth(255)


/* ---- the session client ---- */

// @retail 0x6daa0
bool c_session_client::function_06daa0(long a)
{
	c_class_58d20 *s = session;
	bool result = false;
	if (SESSION_STATE_IS_LIVE(s->state) && s->flag49fd)
	{
		void *address = s->data4a00;
		if (address)
		{
			result = function_063190(address, a) != NONE;
		}
	}
	return result;
}

// @retail 0x6dae0
void c_session_client::function_06dae0(long a, s_session_remote *remote)
{
	c_class_58d20 *s = session;
	if (SESSION_STATE_IS_LIVE(s->state) && s->flag49fd)
	{
		void *address = s->data4a00;
		if (address && function_06dcc0(remote))
		{
			if (function_06de10(remote))
			{
				return;
			}
			byte has_address = remote->has_address;
			if (has_address)
			{
				if (function_0632e0(address, remote->address144) != NONE)
				{
					return;
				}
			}
			if (has_address)
			{
				if (function_063510(remote->address04, address, remote->id))
				{
					goto fallback;
				}
			}
			if (function_06dd00(a, remote))
			{
				return;
			}
		}
	}
fallback:
	s = session;
	long local[3];
	local[0] = 0;
	local[1] = 0;
	local[0] = s->unknown1c;
	local[2] = 0;
	local[1] = s->unknown20;
	local[2] = 3;
	function_07b140(s->unknown04, a, 10, 12, local);
}

// @retail 0x6dbc0
void c_session_client::function_06dbc0()
{
	function_06dc60(this, 3);
}

// @retail 0x6dbd0
void c_session_client::function_06dbd0(const s_session_id *id)
{
	c_class_58d20 *s = session;
	switch (s->state)
	{
	case 2:
		return;
	case 3:
		break;
	case 4:
		return;
	case 5:
		break;
	case 6:
		return;
	case 7:
		if (s->flag7420)
		{
			return;
		}
		break;
	case 8:
		if (s->flag7420)
		{
			return;
		}
		break;
	}
	if (mode == 2 && s->type == 1)
	{
		if (memcmp(&s->members[s->current_member].id, id, sizeof(*id)) != 0)
		{
			function_06d380(this, id);
		}
	}
}

/* ---- shared by the states (lane D, round 4) ---- */

/* ends the hosted game early (network configuration) */
bool g_4cf95c;

static inline long session_time_get(void)
{
	long time;
	if (g_510548)
	{
		time = g_51054c;
	}
	else
	{
		time = GetTickCount();
	}
	return time;
}

// @retail 0x6dfa0 standard
bool c_session_state::function_06dfa0()
{
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	c_class_58d20 *b = o->session_b;
	bool result = false;
	if (b->state == 10)
	{
		if (b->value7420 == 1)
		{
			g_4ee4c4.session_booted = true;
		}
		network_session_leave(b, false);
		network_session_leave(a, false);
	}
	if (a->state == 10)
	{
		if (a->value7420 == 1)
		{
			g_4ee4c4.session_booted = true;
		}
		network_session_leave(b, false);
		network_session_leave(a, false);
	}
	if (skip_cleanup)
	{
		if (b->state == 0)
		{
			function_06df60(o, 1, 0, 0);
		}
		else if (function_058d90(b))
		{
			function_06df60(o, 1, 0, 0);
		}
	}
	else
	{
		if (b->state != 0 && !function_058d90(b) && index != 5)
		{
			network_session_leave(b, false);
			result = true;
		}
	}
	if (unknown0d)
	{
		if (a->state == 0 || function_058d90(a))
		{
			function_06df60(o, 0, 0, 0);
		}
	}
	return result;
}

static inline long session_get_countdown(c_class_58d20 *session)
{
	long result = NONE;
	if (session_state_is_live(session) && session->flag49a8)
	{
		result = session->value49ac;
	}
	return result;
}

// @retail 0x6e360
bool c_session_state_pre_game::function_06e360()
{
	c_class_58d20 *session = owner->session_a;
	long countdown = session_get_countdown(session);
	if (countdown != unknown18)
	{
		unknown14 = session_time_get();
		unknown18 = countdown;
	}
	if (countdown >= 0 && session_time_get() - unknown14 >= 1000 && countdown > 0)
	{
		unknown14 = session_time_get();
		unknown18 = --countdown;
		network_session_start_countdown(session, countdown, true, 0, NULL);
	}
	return false;
}

// @retail 0x6e620
void function_06e620(c_class_58d20 *s)
{
	if (s->function_058d20() && s->type == 2)
	{
		long now = time(NULL);
		long seed = rand() ^ GetTickCount() ^ now;
		long id[2];
		function_07ad80(sizeof(id), (byte *)id);
		s->set_values_4da0(id[0], id[1]);
		s->set_value_4da8(seed);
		s->clear_value_49c4();
		s->set_data_4f24(NULL, NULL);
	}
}

// @retail 0x6e6b0
bool function_06e6b0(c_class_58d20 *s, byte *p)
{
	if (!s->function_058d20())
	{
		*p = false;
		return false;
	}
	if (!g_4cf95c)
		goto local_0;
	{
		if (s->state == 7 && !s->flag7420)
		{
			return false;
		}
		if (s->state != 5)
		{
			return false;
		}
		if (*p)
		{
			goto local_0;
		}
		*p = true;
		return network_session_host_leave_to_peer(s, NONE) ? false : true;
	}
local_0:
	return true;
}

// @retail 0x6dcc0
bool c_session_client::function_06dcc0(s_session_remote *remote)
{
	bool result = true;
	if (!function_192e60(session->get_value_49c8()) || remote->unknown170 > remote->unknown16c)
	{
		result = false;
	}
	return result;
}

// @retail 0x70160
long function_70160(c_class_58d20 *session)
{
	long started = session->time4984;
	return session_time_get() - started;
}

// @retail 0x70c50
void c_session_state_matchmaking::function_070c50(bool flag)
{
	if (flag)
	{
		long started = unknowna7c;
		long elapsed = session_time_get() - started;
		s_session_state_matchmaking_view *state = (s_session_state_matchmaking_view *)this;
		state->unknowna90++;
		state->unknowna94 += elapsed;
	}
	flaga78 = false;
}

// @retail 0x70d20
void c_session_state_matchmaking::function_070d20(bool flag)
{
	if (flag)
	{
		long started = unknowna68;
		long elapsed = session_time_get() - started;
		s_session_state_matchmaking_view *state = (s_session_state_matchmaking_view *)this;
		state->unknowna84++;
		state->unknowna88 += elapsed;
	}
	flaga64 = false;
}

// @retail 0x70c00
void function_70c00(c_session_state_matchmaking *state)
{
	network_session_set_mode(state->owner->session_a, 7);
	state->unknown968 = time(NULL);
	state->unknowna7c = session_time_get();
	((s_session_state_matchmaking_view *)state)->unknowna8c++;
	state->flaga78 = true;
}

PRIVATE __forceinline void function_70c90(c_session_state_matchmaking *arg_0, long arg_1)
{
	*(long *)((byte *)arg_0 + 0xa70) = arg_1;
	*(long *)((byte *)arg_0 + 0xa74) = 0;
}

// @retail 0x70c90
void function_70c90(c_session_state_matchmaking *state)
{
	c_session_state_matchmaking *const *state_reference = &state;
	network_session_set_mode((*state_reference)->owner->session_a, 10);
	long now = session_time_get();
	long attempts = ((s_session_state_matchmaking_view *)state)->unknowna80;
	state->unknowna68 = now;
	((s_session_state_matchmaking_view *)state)->unknowna80 = attempts + 1;
	state->flaga64 = true;
	*(long *)((byte *)state + 0xa6c) = 0;
	function_70c90(state, session_time_get());
}

// @retail 0x72140
bool function_72140(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_b;
	bool result = true;
	for (long index = 0; index < session->member_count && result; index++)
	{
		if (session->members[index].properties.unknownc4 != 2)
			result = false;
	}
	return result;
}

extern s_session_id g_510520;
extern s_session_id g_510540;
extern long g_510518;
extern bool g_51051c;
extern bool g_51051d;
long __stdcall function_73b10(long a, long b);
void function_73bc0(const s_session_id *session_id, const s_session_id *round_key,
	long first, long second, bool free_for_all);

#pragma inline_depth(0)
PRIVATE __forceinline __int64 function_72170(c_class_58d20 *arg_0)
{
	return arg_0->get_values_4da0();
}
#pragma inline_depth(255)

// @retail 0x72170
void function_72170(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_b;
	if (session_state_is_live(session))
	{
		if (session_state_is_live(session) && session->flag49e8)
		{
			s_session_id key;
			key.a = session->value49f0;
			key.b = session->value49f4;
			if (g_510520.a != key.a || g_510520.b != key.b ||
				!function_73b10(g_510540.a, g_510540.b))
			{
				byte *variant = session_state_is_live(session) ? session->data4db0 : NULL;
				bool free_for_all = (variant[0x48] & 1) != 0;
				const s_session_id *id = session->state && session->flag24 ?
					(const s_session_id *)&session->unknown1c : NULL;
				__int64 values = function_72170(session);
				function_73bc0(id, &key, (long)values, (long)(values >> 32), free_for_all);
			}
		}
		else if (g_510518)
		{
			g_51051c = true;
			g_51051d = true;
		}
	}
}

struct s_matchmaking_player_values
{
	byte unknown00[0x18];
	long selected;
	byte unknown1c[0x630 - 0x1c];
	byte summary[0x184];
	long player_count;
	byte unknown7b8[0x878 - 0x7b8];
	long ranks[16];
	long values[16];
	byte unknown8f8[0x938 - 0x8f8];
	long limit;
	bool common_found;
	byte common[12];
	byte unknown949[3];
	long first;
	long second;
	bool ready;
	byte unknown955[3];
	long maximum;
	long minimum;
	long average;
};

struct s_player_rating_properties
{
	byte unknown00[0x12c];
	long selected;
	short unknown130;
	short rank;
	long value;
};

long __stdcall function_063190(void *summary, long user);

// @retail 0x72540
void function_72540(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	s_matchmaking_player_values *ratings = (s_matchmaking_player_values *)state;
	if (state->mode == 3)
	{
		if (ratings->player_count == session->player_count)
		{
			long missing = 0;
			for (long i = 0; i < 16; i++)
			{
				if (session->player_mask & (1 << i))
				{
					s_network_session_player *player = &session->players[i];
					long index = function_063190(ratings->summary, (long)&player->user_id);
					if (index != NONE)
					{
						if (!(player->user_flags & 3))
						{
							if (ratings->ranks[index] == NONE || ratings->values[index] == NONE)
							{
								s_player_rating_properties *properties = (s_player_rating_properties *)player;
								missing++;
								if (properties->selected == ratings->selected && properties->value != NONE && properties->rank != NONE)
								{
									ratings->ranks[index] = properties->rank;
									ratings->values[index] = properties->value;
									missing--;
								}
							}
						}
						else
						{
							ratings->ranks[index] = NONE;
							ratings->values[index] = NONE;
						}
					}
					else
						state->mode = 6;
				}
			}
			ratings->ready = missing == 0;
		}
		else
			state->mode = 6;
		if (state->mode == 3)
		{
			for (long i = 0; i < session->member_count; i++)
			{
				if (*(long *)((byte *)&session->members[i] + 0xac + ratings->selected * sizeof(long)) != 1)
				{
					state->mode = 6;
					break;
				}
			}
		}
		if (state->mode == 3 && ratings->ready)
		{
			long maximum = 0;
			long total = 0;
			long count = 0;
			long minimum = 127;
			for (long i = 0; i < ratings->player_count; i++)
			{
				long rank = ratings->ranks[i];
				if (rank != NONE)
				{
					if (rank < minimum) minimum = rank;
					if (rank >= maximum) maximum = rank;
					total += rank;
					count++;
				}
			}
			if (count > 0)
			{
				ratings->minimum = minimum;
				ratings->maximum = maximum;
				ratings->average = total / count;
			}
			else
				state->mode = 1;
		}
	}
}

bool network_session_host_clear_flag49fc(c_class_58d20 *session);
struct s_member_quality_collection;
struct s_message_identities;
struct s_message_identity;
void function_7e100(long current, const s_member_quality_collection *collection,
	long *selected, long *first, long *second, long *level);
bool function_7ed20(const s_message_identities *message, s_message_identity *common, bool *missing, bool *different);
long function_1932c0(s_surface_description *variant);
long function_193300(s_surface_description *variant);
long function_193340(s_surface_description *variant);
long function_193370(s_surface_description *variant);
long function_193440(s_surface_description *variant);

static inline long matchmaking_variant_width(s_surface_description *variant)
{
	switch (*(long *)variant)
	{
	case 1: return NONE;
	case 2: return NONE;
	case 3: return *(long *)((byte *)variant + 0x5ec);
	case 4: return *(long *)((byte *)variant + 0x5ec);
	case 5: return *(long *)((byte *)variant + 0x5ec);
	default: __assume(0);
	}
}

// @retail 0x72330
void function_72330(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	long member = session->member_index;
	s_member_quality_collection *members = (s_member_quality_collection *)&session->value4c;
	s_matchmaking_player_values *values = (s_matchmaking_player_values *)state;
	network_session_host_clear_flag49fc(session);
	if (state->mode == 3)
	{
		const byte *summary = session_state_is_live(session) && session->flag49fd ? session->data4a00 : NULL;
		if (summary)
		{
			memcpy(values->summary, summary, 0x308);
			if (!values->player_count) state->mode = 1;
		}
		else state->mode = 1;
	}
	if (state->mode == 3)
	{
		long selected = session_state_is_live(session) ? session->value49c8 : NONE;
		if (selected != NONE) values->selected = selected;
		else state->mode = 1;
		if (state->mode == 3)
		{
			s_surface_description *variant = function_192e60(values->selected);
			if (variant) memcpy(values->unknown1c, variant, 0x614);
			else state->mode = 1;
		}
	}
	if (state->mode == 3)
	{
		long level;
		function_7e100(member, members, NULL, NULL, NULL, &level);
		values->limit = NONE;
		s_surface_description *variant = (s_surface_description *)values->unknown1c;
		long type = *(long *)variant;
		if (type == 5 || type == 3 || type == 4)
		{
			if (matchmaking_variant_width(variant) > 0 && function_193340(variant) > 0)
			{
				long count = level / function_193440(variant);
				if (count >= function_193340(variant))
				{
					if (count > function_193370(variant)) count = function_193370(variant);
					values->limit = function_193440(variant) * count;
				}
			}
		}
		else if (level >= function_1932c0(variant))
		{
			if (level > function_193300(variant)) level = function_193300(variant);
			values->limit = level;
		}
	}
	if (state->mode == 3)
	{
		values->common_found = function_7ed20((const s_message_identities *)members,
			(s_message_identity *)values->common, NULL, NULL);
		function_7e100(member, members, NULL, &values->first, &values->second, NULL);
		if (state->mode == 3) *(bool *)state->unknown14 = true;
	}
}

// @retail 0x70570
long function_70570(c_session_state_matchmaking *state)
{
	long result = 0;
	c_class_58d20 *session = state->owner->session_b;
	switch (state->owner->session_a->type)
	{
	case 6:
		result = 1;
		break;
	case 7:
		result = 1;
		break;
	case 8:
		result = 3;
		break;
	case 9:
		result = 3;
		break;
	case 10:
		result = 2;
		break;
	case 11:
	case 12:
	case 13:
		result = session_state_is_live(session) && session->flag49e8 ? 4 : 2;
		break;
	}
	return result;
}

// @retail 0x71f80
bool function_71f80(c_session_state_matchmaking *state)
{
	bool local_0 = false;
	s_session_owner *owner = state->owner;
	c_class_58d20 *session = owner->session_b;
	long current = session->state;
	if (current != 0)
	{
		if (!function_058d90(session) && current > 2 && current <= 8)
		{
			long next;
			long type = session->type;
			if (type >= 2 && type <= 4)
				next = 7;
			else if (type == 5)
				next = 9;
			else
				goto local_1;
			function_06df60(owner, next, 0, 0);
			state->mode = 2;
			local_0 = true;
		}
	}
local_1:
	return local_0;
}

bool network_session_is_leaving(c_class_58d20 *session);

struct s_session_search;
struct s_search_session;
void session_search_mark_session(s_session_search *search, const s_search_session *session);

// @retail 0x71210
bool function_71210(c_session_state_matchmaking *state)
{
	bool result = false;
	s_session_owner *owner = state->owner;
	c_class_58d20 *session = owner->session_a;
	c_class_58d20 *other = owner->session_b;
	if (session->function_058d20() && session->type == 8)
	{
		if (other->state == 0)
		{
			if (state->flag97c)
				session_search_mark_session((s_session_search *)((byte *)state + 0x97c), (const s_search_session *)((byte *)state + 0xa20));
			network_session_set_mode(session, 7);
			result = true;
		}
		else if (session_state_is_live(other))
		{
			network_session_set_mode(session, 9);
			state->unknown970 = g_510548 ? g_51054c : GetTickCount();
			result = true;
		}
	}
	return result;
}

long function_1932c0(s_surface_description *variant);
long function_193300(s_surface_description *variant);
long network_session_get_maximum_players(c_class_58d20 *session);

// @retail 0x705f0
bool function_705f0(c_session_state_matchmaking *state, long *players, long *maximum, long *minimum)
{
	bool result = false;
	if (state->mode == 3)
	{
		long type = state->owner->session_a->type;
		if (type >= 11 && type <= 13)
		{
			c_class_58d20 *session = state->owner->session_b;
			if (session && session_state_is_live(session) && !function_058d90(session))
			{
				byte *membership = (byte *)session + 0x4c;
				s_surface_description *variant = function_192e60(session->get_value_49c8());
				if (variant)
				{
					if (membership)
					{
						if (minimum)
							*minimum = function_1932c0(variant);
						if (maximum)
							*maximum = function_193300(variant);
						if (players)
							*players = *(long *)(membership + 0x10cc);
					}
				}
				else if (membership)
				{
					long limit = network_session_get_maximum_players(session);
					if (players)
						*players = *(long *)(membership + 0x10cc);
					if (maximum)
					{
						*maximum = limit;
						*minimum = limit;
					}
				}
				result = true;
			}
		}
	}
	return result;
}

long network_session_find_member(c_class_58d20 *session, const s_session_member_identity *identity);

// @retail 0x72260
void function_72260(c_session_state_matchmaking *state)
{
	c_class_58d20 *other = state->owner->session_b;
	c_class_58d20 *session = state->owner->session_a;
	bool waiting = false;
	bool failed = false;
	if (session_state_is_live(other))
	{
		for (long i = 0; i < other->member_count; i++)
		{
			if (network_session_find_member(session, (const s_session_member_identity *)other->members[i].words) != NONE)
			{
				long status = other->members[i].properties.unknownc4;
				if (status != 0)
				{
					if (status > 0 && status <= 2)
						waiting = true;
					else
						failed = true;
				}
			}
		}
	}
	if (failed)
		state->mode = 17;
	else if (!waiting && session_state_is_live(session) && session->flag49fc)
		state->mode = 4;
}

#pragma inline_depth(0)
// @retail 0x72950
void function_72950(c_class_58d20 *session)
{
	long seed = 0;
	long current = session->state;
	if (current == 5 || current == 6 || current == 7 || current == 8)
	{
		time_t now = time(NULL);
		seed = rand();
		seed ^= GetTickCount();
		seed ^= (long)now;
		session->set_value_4da8(seed);
		session->clear_value_49c4();
	}
	else
	{
		volatile long unused = current;
	}
}
#pragma inline_depth(255)

#pragma inline_depth(0)
// @retail 0x70b70
void function_70b70(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_b;
	long current = session->state;
	if (current != 0 && !function_058d90(session))
	{
		if (current == 5 || current == 6 || current == 7 || current == 8)
		{
			if (session->type != 14 && !network_session_is_leaving(session))
				network_session_set_mode(session, 14);
			if (session->member_count <= 1 ||
				(session->type == 14 && function_70160(session) >= g_network_configuration.value1a8))
				session->leave(false);
		}
		else
		{
			volatile long unused = current;
			session->leave(false);
		}
	}
}
#pragma inline_depth(255)

struct s_entry_c;
struct s_161c90;
struct s_session_data4db0;
struct s_network_session_membership;
struct s_session_summary;
bool game_variant_choose_map(long index, long *map_id, byte *settings);
s_entry_c *function_19c5f0(long key);
long function_161c90(const s_161c90 *settings);
bool __stdcall network_session_host(c_class_58d20 *session, long mode, long local,
	const XNKID *kid, const XNKEY *key, long count, const dword *identities,
	const long *values, const s_session_id *id, long timeout);
s_network_session_membership *function_5a680(c_class_58d20 *session, long *current_member, long *member_index);
bool network_session_parameters_set_value49c8(c_class_58d20 *session, long value);
long network_session_get_language(c_class_58d20 *session);
bool network_session_parameters_set_language(c_class_58d20 *session, long language);
bool network_session_host_set_summary(c_class_58d20 *session, const s_session_summary *summary);
bool network_session_parameters_set_value4dac(c_class_58d20 *session, long value);
bool network_session_parameters_set_data4db0(c_class_58d20 *session, const s_session_data4db0 *data);

// @retail 0x71480
bool function_71480(c_session_state_matchmaking *state)
{
	s_session_owner *owner = state->owner;
	c_class_58d20 *session = owner->session_a;
	c_class_58d20 *other = owner->session_b;
	bool result = false;
	if (session->type == 10)
	{
		function_70b70(state);
		long current = session->state;
		if (current == 5 || current == 6 || current == 7 || current == 8)
		{
			if (other->state == 0)
			{
				long map_id;
				byte settings[0x130];
				s_entry_c *map = NULL;
				if (!game_variant_choose_map(*(long *)((byte *)state + 0x18), &map_id, settings))
				{
					state->mode = 13;
					result = true;
				}
				if (state->mode == 3)
				{
					map = function_19c5f0(map_id);
					if (!map)
					{
						state->mode = 12;
						result = true;
					}
				}
				if (state->mode == 3)
				{
					long value = function_161c90((const s_161c90 *)settings);
					s_parameters_part part;
					memset(&part, 0, sizeof(part));
					part.unknown00 = 0;
					part.unknown40 = 2;
					const s_session_id *id = NULL;
					if (session->state && session->flag24)
						id = (const s_session_id *)&session->unknown1c;
					s_session_id original = *id;
					if (network_session_host(other, 2, 0, NULL, NULL,
						*(long *)((byte *)state + 0x7b4), (const dword *)((byte *)state + 0x7b8),
						(const long *)((byte *)state + 0x878), &original, NONE))
					{
						byte *manager = *(byte **)((byte *)owner + 0x3c);
						long current_member, member_index;
						byte *membership = (byte *)function_5a680(other, &current_member, &member_index);
						if (other->state && other->flag24)
						{
							*(s_session_id *)part.unknown04 = *(s_session_id *)&other->unknown1c;
							memcpy(part.unknown0c, other->data25, 16);
						}
						memcpy(part.unknown1c, membership + 12 + member_index * 0x10c, 36);
						network_session_host_set_data5ddc(session, &part);
						network_session_parameters_set_value49c8(other, *(long *)((byte *)state + 0x18));
						network_session_parameters_set_language(other, network_session_get_language(session));
						other->set_value_4994(*(long *)((byte *)state + 0x938));
						network_session_parameters_set_value4d08(other, (const char *)map + 0xb4c, NONE, map_id);
						network_session_host_set_summary(other, (const s_session_summary *)((byte *)state + 0x630));
						network_session_parameters_set_value4dac(other, value);
						network_session_parameters_set_data4db0(other, (const s_session_data4db0 *)settings);
						function_07ad80(8, (byte *)&original);
						other->set_values_4da0(original.a, original.b);
						if (*(long *)(manager + 8)) *(long *)(manager + 8) = 0;
						network_session_set_mode(session, 11);
						result = true;
					}
					else
					{
						state->mode = 10;
						result = true;
					}
				}
			}
		}
		else
		{
			volatile long unused = current;
		}
	}
	return result;
}

#pragma pack(push, 4)
struct s_matchmaking_identity
{
	unsigned __int64 user;
	dword flags;
};
struct s_matchmaking_summary
{
	long machine_count;
	s_session_id machine_ids[16];
	s_matchmaking_identity machine_users[16];
	long machine_times[16];
	long player_count;
	s_matchmaking_identity player_users[16];
	long first[16];
	long second[16];
	long third[16];
};
#pragma pack(pop)
bool network_session_parameters_set_summary(c_class_58d20 *session, const s_session_summary *summary);
bool network_session_parameters_set_mode(c_class_58d20 *session, long mode);

// @retail 0x70280
void function_70280(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	s_matchmaking_summary summary;
	memset(&summary, 0, sizeof(summary));
	dword player_mask = session->player_mask;
	summary.player_count = 0;
	for (long i = 0; i < 16; i++)
	{
		if (player_mask & (1 << i))
		{
			summary.player_users[summary.player_count] = *(s_matchmaking_identity *)&session->players[i].user_id;
			summary.first[summary.player_count] = NONE;
			summary.second[summary.player_count] = NONE;
			summary.third[summary.player_count] = 0;
			summary.player_count++;
		}
	}
	summary.machine_count = 1;
	const s_session_id *id = NULL;
	if (session->state && session->flag24)
		id = (const s_session_id *)&session->unknown1c;
	summary.machine_ids[0] = *id;
	function_7ed20((const s_message_identities *)&session->value4c,
		(s_message_identity *)&summary.machine_users[0], NULL, NULL);
	state->mode = 1;
	long current = session->state;
	if (current == 5 || current == 6 || current == 7 || current == 8)
	{
		network_session_host_set_summary(session, (const s_session_summary *)&summary);
		network_session_set_mode(session, 6);
		state->mode = 3;
	}
	else
	{
		volatile long unused = current;
		if (network_session_parameters_set_summary(session, (const s_session_summary *)&summary) &&
			network_session_parameters_set_mode(session, 6))
			state->mode = 3;
	}
}

long g_55e6fc;
long online_get_nat_type(void);
long function_75890(long time);

// @retail 0x70d60
bool function_70d60(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	bool result = false;
	function_70b70(state);
	long current = session->state;
	if (current == 5 || current == 6 || current == 7 || current == 8)
	{
		if (state->mode == 3)
		{
			if (!*(bool *)((byte *)state + 0x954))
			{
				long started = state->time;
				if (session_time_get() - started >= 5000)
					state->mode = 18;
			}
			if (state->mode == 3 && *(bool *)((byte *)state + 0x954))
			{
				bool host = false;
				if (!state->unknowna7c || function_75890(state->unknowna7c) >= function_75890(state->unknowna68))
					host = true;
				else
				{
					bool restricted = g_network_configuration.flag1ac;
					if (*(long *)((byte *)state + 0x938) == NONE)
						host = true;
					else if (restricted && online_get_nat_type() == 3)
					{
						if (g_55e6fc++ < g_network_configuration.value1b0)
							host = true;
						else
							g_55e6fc = 0;
					}
				}
				if (state->flaga78)
					state->flaga78 = false;
				if (state->flaga64)
					state->flaga64 = false;
				if (host)
					function_70c00(state);
				else
					function_70c90(state);
				result = true;
			}
		}
	}
	else
	{
		volatile long unused = current;
	}
	return result;
}

struct s_match_player_list
{
	byte unknown00[12];
	long count;
	byte unknown10[4];
	byte identities[16][12];
	long valuesd4[16];
	long values114[16];
	long values154[16];
};
struct s_match_player_properties
{
	word name[32];
	long field40;
	byte field44[4];
	unsigned __int64 field48;
	unsigned __int64 field50;
	byte unknown58[0x7c - 0x58];
	byte field7c;
	byte unknown7d[7];
	long field84;
	short field88;
	short field8a;
	long field8c;
};
bool network_session_host_set_player_properties(c_class_58d20 *session, long player_index, const byte *properties);

// @retail 0x71ff0
void function_71ff0(c_session_state_matchmaking *state, long value,
	const s_match_player_list *list, const long *indices)
{
	c_class_58d20 *session = state->owner->session_b;
	for (long player = 0; player < 16; player++)
	{
		if (session->player_mask & (1 << player))
		{
			long first = NONE;
			long second = NONE;
			long third = NONE;
			long index = NONE;
			for (long i = 0; i < list->count; i++)
			{
				if (!memcmp(&session->players[player].user_id, list->identities[i], 12))
				{
					index = indices[i];
					first = list->values154[i];
					second = list->valuesd4[i];
					third = list->values114[i];
					break;
				}
			}
			s_match_player_properties properties = *(s_match_player_properties *)session->players[player].propertiesa8;
			properties.field7c = (byte)index;
			properties.field84 = value;
			properties.field8c = third;
			properties.field8a = (short)second;
			properties.field88 = (short)first;
			network_session_host_set_player_properties(session, player, (const byte *)&properties);
		}
	}
}

long function_75870(void);
long function_75890(long time);

bool network_session_host_become_leader(c_class_58d20 *session);

#pragma inline_depth(0)
// @retail 0x72700
bool c_session_state_start_match::update()
{
    s_session_owner *o = owner;
    c_class_58d20 *a = o->session_a;
    c_class_58d20 *b = o->session_b;
    bool result = function_06dfa0();
    if (!result && !o->failed && SESSION_STATE_IS_LIVE(b->state))
    {
        if (b->type == 4)
        {
            function_06df60(o, 8, 0, 0);
            mode = 2;
        }
        else if (b->type == 2 || b->type == 3)
        {
            bool ready = false;
            if (function_06e720(b) && function_06e6b0(b, &unknown14))
            {
                if (!unknown18)
                    unknown18 = function_75870();
                long remaining = 10000 - function_75890(unknown18);
                if (remaining < 0)
                    remaining = 0;
                network_session_host_become_leader(b);
                network_session_start_countdown(b, (remaining + 999) / 1000, true, 0, NULL);
                ready = remaining == 0;
            }
            else
                unknown18 = 0;
            if (mode == 3)
            {
                if (ready)
                {
                    network_session_set_mode(b, 4);
                    result = true;
                }
            }
            else
            {
                c_class_58d20 *primary = o->session_a;
                b->leave(false);
                primary->leave(false);
            }
        }
        else if (b->type == 5)
        {
            function_06df60(o, 9, 0, 0);
            mode = 2;
        }
        else
        {
            function_06df60(o, 1, 0, 0);
            mode = 1;
        }
    }
    long state = a->state;
    if (state == 5 || state == 6 || state == 7 || state == 8)
        network_session_host_set_value49f8(a, mode);
    else
    {
        volatile long unused = state;
    }
    return result;
}
#pragma inline_depth(255)

long function_66050(c_class_58d20 *session, long variant_index);

// @retail 0x6e410
bool c_session_state_pre_game::function_06e410()
{
 c_class_58d20 *session = owner->session_a;
 if (SESSION_STATE_IS_LIVE(session->state) && session->flag49a8 && !session->value49ac)
 {
  if (SESSION_STATE_IS_LIVE(session->state) && session->value49a4 > 1)
  {
   network_session_stop_countdown(session);
   if (session->value18 == 2)
   {
    long variant = NONE;
    if (SESSION_STATE_IS_LIVE(session->state))
     variant = session->value49c8;
    if (!function_66050(session, variant))
     function_70280(*(c_session_state_matchmaking **)((byte *)this + 0x10));
   }
  }
  else
  {
   bool ready = true;
   dword mask = 0;
   for (long i = 0; i < session->member_count; i++)
    if (session->members[i].unknown88 <= 2)
    {
     mask |= 1 << i;
     ready = false;
    }
   if (ready)
    network_session_set_mode(session, 2);
   else
    network_session_start_countdown(session, 1, true, 2, 0);
  }
 }
 return false;
}
