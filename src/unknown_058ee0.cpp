// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_058EE0.CPP: the session manager (0x527330): its state
   machine (the states at 0x52738c..0x527fd8, the owner at 0x527334), the
   game session (session_a) and the other session (session_b) (lane D).
   Everything is one struct global, s_session_states (unknown_058ee0.h). */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"

s_session_states g_527330;

long function_70570(c_session_state_matchmaking *state);
bool function_705f0(c_session_state_matchmaking *state, long *players, long *maximum, long *minimum);

/* the manager state as the matchmaking queries read it (0x59570 is a separate stub:
   making it real changes how its callers in the menus compile) */
static __forceinline long network_session_manager_get_state(void)
{
	long result = 0;
	switch (g_527330.state)
	{
	case 6: result = function_70570(&g_527330.state_matchmaking); break;
	case 7: result = 5; break;
	}
	return result;
}

// @retail 0x594a0
bool __stdcall function_594a0(long a, long b, long c)
{
	bool result = false;
	long mode = g_527330.state_matchmaking.mode;
	if (mode == 2)
		mode = g_527330.state_start_match.mode;
	if (mode == 3)
	{
		long state = network_session_manager_get_state();
		if (state == 2 || state == 4)
			result = function_705f0(&g_527330.state_matchmaking, (long *)a, (long *)b, (long *)c);
	}
	return result;
}

/* the session being tracked for debugging: when it changes, the flags are set */
long g_510518;
bool g_51051c;
bool g_51051d;
s_session_id g_510528;

/* src/unknown_058cb0.cpp, src/unknown_059670.cpp, src/unknown_0592d0.cpp */
/* the current and the other session, when they exist: retail inlines these
   here but calls them out of line from other files */
static inline bool session_manager_get_session_a(c_class_58d20 **session)
{
	bool result = false;
	if (g_527330.initialized)
	{
		c_class_58d20 *current = (c_class_58d20 *)g_527330.session_a;
		if (current->state)
		{
			if (session)
			{
				*session = current;
			}
			result = true;
		}
	}
	return result;
}

static inline bool session_manager_get_session_b(c_class_58d20 **session)
{
	bool result = false;
	if (g_527330.initialized)
	{
		c_class_58d20 *other = (c_class_58d20 *)g_527330.session_b;
		if (other->state)
		{
			if (session)
			{
				*session = other;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x59670
bool function_59670(c_class_58d20 **session)
{
	return session_manager_get_session_a(session);
}

// @retail 0x596a0
bool function_596a0(c_class_58d20 **session)
{
	return session_manager_get_session_b(session);
}
dword function_0592d0(void);

/* src/unknown_054fe0.cpp, src/unknown_1932c0.cpp */
long network_session_interface_get_value_49c8(void);
struct s_surface_description;
s_surface_description *function_192e60(long index);
bool function_193470(s_surface_description *variant);

/* src/unknown_059ad0.cpp */
void network_session_close(c_class_58d20 *session);
s_session_id *network_session_get_id(c_class_58d20 *session);
bool network_session_parameters_set_value49f8(c_class_58d20 *session, long value);
bool network_session_parameters_set_mode(c_class_58d20 *session, long mode);
bool network_session_request_mode_acknowledge(c_class_58d20 *session);

#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : ((value) > (upper) ? (upper) : (value)))

static inline void session_tracking_changed(void)
{
	if (g_510518)
	{
		g_51051c = true;
		g_51051d = true;
	}
}

static inline c_class_58d20 *session_manager_session_a(void)
{
	return (c_class_58d20 *)g_527330.session_a;
}

static inline c_class_58d20 *session_manager_session_b(void)
{
	return (c_class_58d20 *)g_527330.session_b;
}

// @retail 0x59260
bool network_session_manager_is_joining(void)
{
	bool result = false;
	if (g_527330.initialized && g_527330.state == 5)
		result = true;
	return result;
}

PRIVATE __forceinline void function_59281(const c_class_58d20 *arg_0, bool *arg_1)
{
	long local_0 = arg_0->state;
	if (!local_0)
		*arg_1 = true;
	else if (local_0 > 2 && local_0 <= 8)
		*arg_1 = arg_0->current_member == arg_0->value50;
}

// @retail 0x59280
void network_session_manager_check_joining_leader(void)
{
	if (g_527330.initialized && g_527330.state == 5 && !g_527330.state_joining.unknown104)
	{
		c_class_58d20 *session = g_527330.state_joining.owner->session_a;
		bool local_0 = false;
		function_59281(session, &local_0);
		if (local_0)
			g_527330.state_joining.flage9 = true;
	}
}

// @retail 0x59330
bool network_session_manager_session_unready(void)
{
	bool result = false;
	c_class_58d20 *session;
	if (session_manager_get_session_a(&session))
	{
		if (!function_058d90(session))
			result = true;
	}
	return result;
}

// @retail 0x598f0
void network_session_check_tracking(c_class_58d20 *session)
{
	if (g_510518)
	{
		if (!memcmp(&g_510528, network_session_get_id(session), sizeof(s_session_id)))
			session_tracking_changed();
	}
}

struct s_helper_source;

struct s_helper_output
{
	long l0;
	long l4;
	long l8;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	long l24;
	long l28;
};


struct s_online_match_session_info
{
	XNKEY key;
	XNKID session_id;
	XNADDR address;
	s_helper_output output;
};

class c_helper_a
{
public:
	virtual void v0(long a, long b);
	virtual void v1();
	virtual void v2();
	virtual void v3(s_helper_output *out);
	virtual long v4();
	virtual long v5(long size, void *data);
	virtual long v6();
	virtual bool v7();
	virtual void v8(long a);

	s_helper_source *source;
	bool b8;
	bool b9;
	bool ba;
	byte unknownb;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	bool listening;
	byte unknown25[3];
	long listen_time;
	long l2c;
	bool b30;
	byte unknown31[3];
	s_online_match_session_info info;
};

struct s_session_search
{
	long unknown00;
	s_session_owner *owner;
	bool active;
	bool unknown09;
	bool unknown0a;
	byte unknown0b;
	long task0c;
	long value10;
	long task14;
	long value18;
	long value1c;
	long value20;
	byte unknown24[8];
	long type;
	bool unknown30;
	byte unknown31[0x9c - 0x31];
};

struct s_session_searches
{
	bool initialized;
	s_session_search search_a;
	c_session_state_matchmaking *matchmaking;
	s_session_search search_b;
};

extern s_session_searches g_477088;

struct s_network_session_list;
void function_6dec0(s_session_owner_view *owner);
void function_737b0(c_helper_a *helper);
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, s_session_id const *id);

// score 20 (was 20): after the call to 0x75800, retail rereads g_510518 and
// stores an immediate 0 into g_51051d; ours drops the repeated check, since
// the callee does not write g_510518 (a separate global here), and stores al,
// the call's zero result. A _ReadWriteBarrier before the check brings the
// reread back with other registers (23, also with nested ifs); an inner-block
// barrier and a session local stay at 20. The 600 s permutation run found no
// improvement.
// b13N (11-17) walk against retail, fast build on r846: logic equals retail (flag test 0x59972, call 0x59988 with the
// session list in ebx and &g_510528 pushed, result test 0x5998d, stores 0x5999b and 0x599a2). Only the reread at
// 0x59992 differs (the 11 missing bytes: mov eax, test, je, plus an immediate 0 store instead of al). Retail reads
// g_510518 a second time after the call. A volatile qualifier on the definition makes 0x59940 MATCH but turns matched
// 0x598f0 into 18 (74 bytes vs 65), so retail reads it plainly there; reading only the second check through
// `*(volatile long *)&g_510518` also MATCHES with the other 31 functions of this file unchanged, but is a volatile
// cast on a global (not kept). No score change from: g_510518..g_510528 merged into one struct (20), its address
// stored in another global (20), a session_tracking_changed(bool) helper for the inner block (20).
// Roasted-Codes (#1527 comment, 2026-11-26; fast reduced-image scores): the four tracking globals defined in
// src/unknown_072db0.cpp with externs here 20 (the reread stays folded); a `const bool &` on the first test 19 (106 B,
// the state still cached in ESI).
// @retail 0x59940
void function_59940(void)
{
	if (g_527330.initialized)
	{
		function_6dec0((s_session_owner_view *)&g_527330.state);
		((c_helper_a *)&g_477088.search_a)->c_helper_a::v2();
		((c_helper_a *)&g_477088.search_b)->c_helper_a::v2();
		function_737b0((c_helper_a *)&g_477088.search_b);
		if (g_510518 && !network_session_manager_find_session(
			(s_network_session_list *)g_527330.unknown30, &g_510528))
		{
			if (g_510518)
			{
				g_51051c = true;
				g_51051d = false;
			}
		}
	}
}

// @retail 0x59360
void network_session_manager_leave_session_a(bool close)
{
	c_class_58d20 *session = session_manager_session_a();
	if (session->state)
	{
		network_session_check_tracking(session);
		if (close)
			network_session_close(session);
		else
			network_session_leave(session, false);
	}
}

// @retail 0x593a0
void network_session_manager_leave_session_b(bool close)
{
	c_class_58d20 *session = session_manager_session_b();
	if (session->state)
	{
		network_session_check_tracking(session);
		if (close)
			network_session_close(session);
		else
			network_session_leave(session, false);
	}
}

/* called by unknown_03d380.cpp under its old name */
// @retail 0x593e0
void function_593e0(void)
{
	c_class_58d20 *session_b = session_manager_session_b();
	if (session_b->state)
	{
		network_session_check_tracking(session_b);
		network_session_close(session_b);
	}
	c_class_58d20 *session_a = session_manager_session_a();
	if (session_a->state)
	{
		network_session_check_tracking(session_a);
		network_session_close(session_a);
	}
	long state = g_527330.state;
	g_527330.unknown4d = false;
	g_527330.unknown4c = true;
	if (state)
	{
		c_session_state *previous = g_527330.states[state];
		c_session_state *next = g_527330.states[0];
		previous->leave((long)next);
		g_527330.state = 0;
		next->enter((long)previous, 0, 0);
	}
	g_527330.unknown4c = false;
}

// @retail 0x59470
bool network_session_manager_session_a_established(void)
{
	c_class_58d20 *session = session_manager_session_a();
	if (session_state_is_live(session) && !session->value18)
		return true;
	return false;
}

// @retail 0x594f0
long network_session_manager_get_match_mode(void)
{
	long mode = g_527330.state_matchmaking.mode;
	if (mode == 2)
		mode = g_527330.state_start_match.mode;
	return mode;
}

static inline long session_get_value49f8(c_class_58d20 *session)
{
	long result = 0;
	if (session_state_is_live(session))
		result = session->value49f8;
	return result;
}

// @retail 0x59500
long network_session_manager_get_value49f8(void)
{
	long local_0 = 1;
    c_class_58d20 *session = session_manager_session_a();
	if (session_state_is_live(session))
        local_0 = session_get_value49f8(session);
    return local_0;
}

// @retail 0x59530
void network_session_manager_set_value49f8(long value)
{
	c_class_58d20 *session = session_manager_session_a();
	if (session_state_is_live(session) && function_058d50(session))
		network_session_parameters_set_value49f8(session, 0);
}

static inline long session_get_value49ac(c_class_58d20 *session)
{
	long result = NONE;
	if (session_state_is_live(session) && session->flag49a8)
		result = session->value49ac;
	return result;
}

static inline long session_get_established_value49ac(c_class_58d20 *session)
{
	long result = NONE;
	if (session_state_is_live(session))
		result = session_get_value49ac(session);
	return result;
}

// @retail 0x59590
long network_session_manager_get_value49ac(void)
{
	long result = NONE;
	if (g_527330.state == 7)
		result = session_get_established_value49ac(g_527330.state_start_match.owner->session_b);
	return result;
}

// @retail 0x595e0
void network_session_manager_request_mode_acknowledge(void)
{
	if (g_527330.state == 6)
	{
		session_tracking_changed();
		network_session_request_mode_acknowledge(g_527330.state_matchmaking.owner->session_a);
	}
}

PRIVATE __forceinline void function_59615(c_class_58d20 *arg_0, bool *arg_1)
{
	if (network_session_parameters_set_mode(arg_0, 1))
		*arg_1 = true;
	else
		*arg_1 = *(volatile bool *)arg_1;
}

// @retail 0x59610
bool network_session_manager_set_mode(void)
{
	bool result = false;
	long local_0 = g_527330.state;
	if (!local_0)
		goto local_1;
	if (local_0 == 1)
		goto local_2;
	{
		c_class_58d20 *session = session_manager_session_a();
		if (!function_058d50(session))
			goto local_1;
		function_59615(session, &result);
		goto local_1;
	}
local_2:
	result = true;
local_1:
	return result;
}

// @retail 0x596d0
bool network_session_manager_get_session(c_class_58d20 **session)
{
	bool result = false;
	long state = 0;
	if (g_527330.initialized)
		state = g_527330.state;
	switch (state)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		result = function_59670(session);
		break;
	case 6:
	{
		long index = network_session_interface_get_value_49c8();
		result = function_59670(session);
		if (index != NONE)
		{
			s_surface_description *variant = function_192e60(index);
			if (variant && !function_193470(variant))
			{
				c_class_58d20 *other;
				if (function_596a0(&other))
				{
					result = true;
					if (session)
						*session = other;
				}
			}
		}
		break;
	}
	case 7:
	case 8:
	case 9:
		result = function_596a0(session);
		break;
	}
	return result;
}

// @retail 0x59780
bool network_session_manager_get_any_session(c_class_58d20 **session)
{
	bool result = session_manager_get_session_b(session);
	if (!result)
		result = session_manager_get_session_a(session);
	return result;
}

// @retail 0x59840
bool network_session_manager_get_hosted_session(c_class_58d20 **session)
{
	bool result = false;
	if (g_527330.initialized)
	{
		c_class_58d20 *current = session_manager_session_a();
		if (current->function_058d20() && current->value18 == 1)
		{
			if (session)
				*session = current;
			result = true;
		}
	}
	return result;
}

/* a value of the session's properties: a kind and the values it uses, each
   pinned to its range */
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

// @retail 0x599b0
void session_property_value_set_kind2(s_session_property_value *value, long value14)
{
	value->value04 = 0;
	value->value08 = 0;
	value->value0c = 0;
	value->value10 = 0;
	value->value14 = 0;
	value->value18 = 0;
	value->kind = 2;
	value->value14 = PIN(value14, 0, 1023);
}

// @retail 0x599f0
void session_property_value_set_kind3(s_session_property_value *value, long value18)
{
	value->value04 = 0;
	value->value08 = 0;
	value->value0c = 0;
	value->value10 = 0;
	value->value14 = 0;
	value->value18 = 0;
	value->kind = 3;
	value->value18 = PIN(value18, 0, 1023);
}

// @retail 0x59a30
void session_property_value_set_kind1(s_session_property_value *value, long value04, long value08, long value0c, long value10)
{
	value->value04 = 0;
	value->value08 = 0;
	value->value0c = 0;
	value->value10 = 0;
	value->value14 = 0;
	value->value18 = 0;
	value->kind = 1;
	value->value04 = PIN(value04, 0, 15);
	value->value08 = PIN(value08, 0, 15);
	value->value0c = PIN(value0c, 0, 63);
	value->value10 = PIN(value10, 0, 1023);
}

// @retail 0x59ab0
void session_property_value_set_kind4(s_session_property_value *value)
{
	value->value04 = 0;
	value->value08 = 0;
	value->value0c = 0;
	value->value10 = 0;
	value->value14 = 0;
	value->value18 = 0;
	value->kind = 4;
}

/* src/unknown_072c80.cpp */
void session_searches_initialize(s_session_owner *owner, c_session_state_matchmaking *matchmaking);
void session_searches_dispose(void);

#define SESSION_OWNER ((s_session_owner *)&g_527330.state)
#define SESSION_STATE(state) ((s_session_state_view *)&(state))

// score 51 (was 51): register allocation around the calls: retail passes the
// owner (0x527334) to session_owner_initialize as an immediate and loads it into
// edi only after the call, and passes &state_matchmaking to
// session_searches_initialize as an immediate again; ours loads edi before the
// first call, and copies eax (which 0x70190 leaves unchanged) into esi. Tried:
// no owner local, the owner local first and passed to every call, a matchmaking
// local, 0x70190 returning its state, owner->states[index] in
// session_state_initialize, and linking network_session_client.cpp after this
// file (all 51).
// Roasted-Codes (#1527 comment, 2026-11-26; fast on main bfb863d1, a plain full check on 071eb14b6): a MATCH that costs
// matched 0x54590, held (a new loss of 0x54590 needs the maintainer's word). Form: the two initializers as members of
// their existing classes, bodies unchanged, 0x6de50 on s_session_owner and 0x70190 on c_session_state_matchmaking,
// called as `SESSION_OWNER->session_owner_initialize(...)` and
// `g_527330.state_matchmaking.session_state_matchmaking_initialize(SESSION_OWNER)`: target 0 (455 B, 68 address fields
// verified); 0x6de50, 0x70190 and this file's other functions still MATCH; the full check gave +0x58ee0, -0x54590,
// nothing else. 0x54590 becomes 2 at +0xff (the image-following reload pair its notes describe), and that follows
// 0x70190 being a member: with 0x70190 a free routine taking the state by reference, 0x54590 matches again and this
// target is 22 (+309). 0x54590 does not move with an explicit __fastcall on that member, the owner passed by reference
// or as the view pointer, 0x6de50's body in the caller's file, or the two member declarations' place in their classes
// (address order, after the data). A form that keeps both is the open lead.
// @retail 0x58ee0
bool network_session_manager_initialize(long unknown40, long unknown44, void *unknown2c, c_class_58d20 *session_a, c_class_58d20 *session_c, c_class_58d20 *session_b)
{
	g_527330.client.session = session_b;
	g_527330.client.requests = NULL;
	g_527330.client.request_count = 0;
	g_527330.client.mode = 1;
	session_b->listener = (c_network_session_listener *)&g_527330.client;
	session_owner_initialize(SESSION_OWNER, unknown40, unknown44, unknown2c, session_a, session_c, session_b, &g_527330.client);
	s_session_owner_view *owner = (s_session_owner_view *)SESSION_OWNER;
	session_state_initialize(SESSION_STATE(g_527330.state_none), owner, 0, false, false);
	session_state_initialize(SESSION_STATE(g_527330.state_pre_game), owner, 1, true, false);
	SESSION_STATE(g_527330.state_pre_game)->unknown10 = &g_527330.state_matchmaking;
	session_state_initialize(SESSION_STATE(g_527330.state_start_game), owner, 2, true, false);
	session_state_initialize(SESSION_STATE(g_527330.state_in_game), owner, 3, true, false);
	session_state_initialize(SESSION_STATE(g_527330.state_post_game), owner, 4, true, false);
	session_state_joining_initialize(&g_527330.state_joining, SESSION_OWNER);
	session_state_matchmaking_initialize(&g_527330.state_matchmaking, SESSION_OWNER);
	session_state_initialize(SESSION_STATE(g_527330.state_start_match), owner, 7, true, true);
	g_527330.state_start_match.mode = 1;
	session_state_initialize(SESSION_STATE(g_527330.state_in_match), owner, 8, true, true);
	g_527330.state_in_match.time = 0;
	session_state_initialize(SESSION_STATE(g_527330.state_post_match), owner, 9, true, true);
	session_searches_initialize(SESSION_OWNER, &g_527330.state_matchmaking);
	g_527330.initialized = true;
	return true;
}

// @retail 0x590b0
void network_session_manager_dispose(void)
{
	session_searches_dispose();
	session_state_dispose(SESSION_STATE(g_527330.state_none));
	session_state_dispose(SESSION_STATE(g_527330.state_pre_game));
	session_state_dispose(SESSION_STATE(g_527330.state_start_game));
	session_state_dispose(SESSION_STATE(g_527330.state_in_game));
	session_state_dispose(SESSION_STATE(g_527330.state_post_game));
	g_527330.state_joining.function_06f0f0();
	session_state_dispose(SESSION_STATE(g_527330.state_joining));
	if (g_527330.state_matchmaking.flag97c)
		function_090c80(&g_527330.state_matchmaking.flag97c);
	session_state_dispose(SESSION_STATE(g_527330.state_matchmaking));
	session_state_dispose(SESSION_STATE(g_527330.state_start_match));
	session_state_dispose(SESSION_STATE(g_527330.state_in_match));
	session_state_dispose(SESSION_STATE(g_527330.state_post_match));
	function_06dc60(&g_527330.client, 3);
	g_527330.client.session->listener = NULL;
	g_527330.client.session = NULL;
	g_527330.initialized = false;
}

// @retail 0x59200
void network_session_manager_join(const void *target, long count, const void *entries, bool flag)
{
	session_tracking_changed();
	g_527330.state_joining.function_06f200(flag, target, count, entries);
}

// @retail 0x59230
void network_session_manager_join_description(const s_session_description *description, long count, const void *entries)
{
	session_tracking_changed();
	g_527330.state_joining.function_06f3a0(description, count, entries);
}

/* src/unknown_059ad0.cpp */
bool __stdcall network_session_host(c_class_58d20 *session, long mode, long local, const XNKID *kid, const XNKEY *key, long count, const dword *identities, const long *values, const s_session_id *id, long timeout);

/* not decompiled yet */
void network_session_interface_update_session(c_class_58d20 *session);

// @retail 0x59890 standard
bool __stdcall network_session_manager_host_session(long mode, const XNKID *kid, const XNKEY *key)
{
	long state = session_manager_session_a()->state;
	c_class_58d20 *session = session_manager_session_a();
	bool result = false;
	if (!state)
	{
		s_session_id id;
		memset(&id, 0, sizeof(id));
		if (network_session_host(session_manager_session_a(), mode, 0, kid, key, 0, NULL, NULL, &id, 0))
		{
			result = true;
			network_session_interface_update_session(session);
		}
	}
	return result;
}

// @retail 0x591e0
bool network_session_manager_host_offline(void)
{
	return network_session_manager_host_session(0, NULL, NULL);
}

// @retail 0x591f0
bool network_session_manager_host_online(void)
{
	return network_session_manager_host_session(1, NULL, NULL);
}
