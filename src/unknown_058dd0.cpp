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
}

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
	flag10 = live;
	memset(unknown14, 0, sizeof(unknown14));
}

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
}

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

// @retail 0x6dfa0
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
	if (g_4cf95c)
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
			return true;
		}
		*p = true;
		return network_session_host_leave_to_peer(s, NONE) ? false : true;
	}
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
	*(long *)((byte *)state + 0xa70) = session_time_get();
	*(long *)((byte *)state + 0xa74) = 0;
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
				return false;
			function_06df60(owner, next, 0, 0);
			state->mode = 2;
			return true;
		}
	}
	return false;
}

bool network_session_is_leaving(c_class_58d20 *session);

// @retail 0x72950
void function_72950(c_class_58d20 *session)
{
	long current = session->state;
	if (current == 5 || current == 6 || current == 7 || current == 8)
	{
		time_t now = time(NULL);
		long seed = rand();
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
				network_session_leave(session, false);
		}
		else
		{
			volatile long unused = current;
			network_session_leave(session, false);
		}
	}
}
