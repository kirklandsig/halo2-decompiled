#include "unknown_4cf790.h"
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

void transport_get_random_bytes(long count, byte *buffer);
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

// score 5 (was 5): in the type 2-4 arm retail loads the member index and the
// member's kind into eax; ours puts the index in edx and the kind in ebp (the
// session's register, which dies there). Retail's case table runs to type 18
// (16-18 share the type 15 arm). Tried: the kind through a NONE default before
// the test 5; the kind tests in the 0x58d70 condition 5; the kind read through
// the member's parameters 5; a named member index 5. Round 60: an unsigned kind 5;
// the member index read before the 0x58d70 call 5 (the load is sunk, same code); the
// kind declared at function scope 5; the switch on a named type local 5; the
// `default: __assume(0)` label first 65 (the case blocks follow source order).
// Round 65: named 0x58d50, 0x58d70 and 0x58d20 results 5 each and combined 5;
// a case-1 session pointer 5; the type-0 arm with an early break 5; case-1 results
// from short-circuit tests 83, from conditional expressions 76.
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
		case 5:
			function_06df60(o, 4, 0, 0);
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
		case 15:
		case 16:
		case 17:
		case 18:
			function_06df60(o, 5, 0, 0);
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
		}
		else if (a->type == 2)
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
				goto local_6ea21;
			}
			if (a->function_058d20())
			{
				if (game_finish_countdown_done())
				{
					network_session_set_mode(a, 5);
					result = true;
				}
				else if (a->get_value_49c4())
				{
					game_finish();
				}
			}
			if (game_loop_running())
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
			if (a->type == 5)
				function_06df60(o, 4, 0, 0);
			else
				function_06df60(o, 1, 0, 0);
		}
	}
local_6ea21:
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
				result = true;
				goto local_0;
			}
			if (game_finish_countdown_done() && b->function_058d20())
			{
				network_session_set_mode(b, 5);
				result = true;
				goto local_0;
			}
		}
		else if (b->type == 5)
		{
			function_06df60(o, 9, 0, 0);
			result = true;
			goto local_0;
		}
		else
		{
			network_session_leave(b, false);
			result = true;
		}
	}
local_0:
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

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

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
	bool result;
	s_session_owner *o = owner;
	c_class_58d20 *a = o->session_a;
	c_class_58d20 *b = o->session_b;
	result = function_06dfa0();
	volatile bool local_0 = result;
	if (!result && !o->failed)
	{
		if (b->type == 5 && (a->type == 13 || a->type == 12))
		{
			if (!SESSION_STATE_IS_LIVE(b->state) || !a->function_058d20())
				goto local_1;
			long index = b->member_index;
			s_session_snapshot snapshot;
			s_session_snapshot compare;
			memset(&snapshot, 0, sizeof(snapshot));
			snapshot.unknown40 = 2;
			network_session_get_key(b, (s_session_id *)snapshot.unknown04, snapshot.unknown0c, NULL, &snapshot.unknown00);
			memcpy(snapshot.words, b->members[index].words, sizeof(snapshot.words));
			if (!network_session_get_data5ddc(a, (s_parameters_part *)&compare) || memcmp(&compare, &snapshot, sizeof(snapshot)) != 0)
				network_session_host_set_data5ddc(a, (const s_parameters_part *)&snapshot);
			result = local_0;
		}
		else
		{
			network_session_leave(b, false);
			result = true;
		}
	}
local_1:
	return result;
}

/* ---- joining ---- */

// @retail 0x6fd10
bool c_session_state_joining::update()
{
	s_session_owner *o = owner;
	c_class_58d20 *c = o->session_c;
	c_class_58d20 *a = o->session_a;
	if (unknown104 == 0 && flag10)
	{
		function_06f4b0(this);
	}
	if (unknown104 == 0 && !flag10)
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
	if (unknown104 != 0)
	{
		if (a->function_058d20())
			network_session_set_mode(a, 1);
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

struct s_session_join_request;
bool function_630f0(c_class_58d20 *session, long address, const s_session_join_request *request, long reason);

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
			if (remote->has_address && function_0632e0(address, remote->address144) != NONE)
			{
				return;
			}
			if (remote->has_address && function_063510(address, remote->address04, remote->id))
			{
				goto fallback;
			}
			if (function_06dd00(a, remote))
			{
				return;
			}
		}
	}
fallback:
	function_630f0(session, a, (const s_session_join_request *)remote, 3);
}

// @retail 0x6dbc0
void c_session_client::function_06dbc0()
{
	function_06dc60(this, 3);
}

// score 43 (was 43): this stays in edx where retail copies it to ebx at entry, so
// the mode and member index take esi; separate case 7 and 8 labels (retail's direct
// table) with an inline compare-and-update helper 47, an inline id-compare helper 48,
// a member-index local 47, an id copy 47, the mode in a local 47; a state local 43;
// cases grouped by outcome 43; a do/while (false) join 43; an early return on an equal
// id 43; a goto single exit 43; copies of this before, after or at the call 43 (all
// compile-only). With 0x58d90 inlined by the link (its file at /Ob1, a diagnostic) the
// table is retail's but this stays in edx (47); only /Os or /G7, which compare the mode
// in memory, give retail's push ebx.
// Round 81: the 0x6d380 call written twice (a diagnostic) gives retail's head exactly (this in
// ebx, id in ebp, mode and index in edx): retail allocates as if this and id were live across a
// call; with only this live across one, id still takes edx. Separate case 7 and 8 bodies 47; a
// busy flag set by the switch 47; an early return on an equal id 43; session at each use 46; one
// combined condition 43; harness: a memcmp result local (worse), a sized compare helper, a __stdcall
// member or __cdecl callee, /arch:SSE, /G7, /Ob1, /Ox or /G5 all keep this in edx.
// Round 84: an early return on the mode and type 43; session read in the switch and the local
// after it 43; the type tested before the mode 43; memcmp operands swapped 47; a return after the
// call 43; harness: one && condition with the memcmp, cases 3 and 5 left to the default, an id
// copy: no change. Calling the existing busy-state test out of line puts this in ebx (as any call
// before the compare does), but retail has that switch inline.
// in memory, give retail's push ebx. Round 11: the existing 0x58d90 called in the test 100 (stays
// out of line, 84 bytes); an early return on the mode/type test, the mode test outer with the type
// and id tests inside and a member id pointer local 43 each (same code); the session read at each
// use (no local) worse (this in eax). Compile-only, all with this in edx: 32 combinations of the
// condition shape (also the type test first), truthy memcmp, a copy of this as the call argument and
// grouped or separate case 7/8; an inline copy of 0x58d90's body gives retail's direct table only.
// Lane 13 r14: the existing 0x58d90 called with a `__forceinline` declaration in this file stays
// out of line (its /Ob0 definition) 100, as a plain call 100; the type test first 43; memcmp with
// the id first 47; the memcmp folded into the mode/type condition 43.
// Round 84: an early return on the mode and type 43; session read in the switch and the local
// after it 43; the type tested before the mode 43; memcmp operands swapped 47; a return after the
// call 43; harness: one && condition with the memcmp, cases 3 and 5 left to the default, an id
// copy: no change. Calling the existing busy-state test out of line puts this in ebx (as any call
// before the compare does), but retail has that switch inline.
// Round 90 (fast): the switch as 0x58d90's own result-variable form (retail's direct table) with an
// early return or joined into the condition 45/45 (diagnostic: a hand copy of the helper); memcmp size
// as sizeof(s_session_id), an equal-id return before the call and a return after it 43 each
// (identical). BLOCKED-BY 0x58d90: retail inlines that existing helper, ours stays out of line (/Ob0).
// Round 183 (lane 3, fast): a member pointer named for the memcmp 43; a client copy of this carrying
// the session read and the call 43; case 7/8 as `if (!flag) break; return;` 43; the mode/type test as an
// early return before the compare 43; a named differs result with sizeof(s_session_id) 43 (all the same
// code). The function is virtual (standard thiscall), so no caller or convention lever applies.
// Round 31 (lane 13): compile-only, a second 0x6d380 call or 0x58d90 called out of line give retail's
// `push ebx; mov ebx, ecx`; 0x58d90's result-variable switch as a forceinline helper (early return,
// joined, nested, session per use) gives the direct table with this still in EDX; descending cases, an
// explicit default, grouped returns with 3/5 to the default, a member-index local, `&*id`: no change. Real
// (fast): grouped returns 43, member-index local 43, descending cases 43 (identical code).
// Round 113 (lane 8): separate case 7 and case 8 bodies (0x58d90's own form) re-measured 47 (plain check
// agrees): retail's direct 7-entry jump table, and the only difference left is `this` in EDX where retail
// moves it to EBX at entry (mode/index then take ESI for retail's EDX, id EBX for retail's EBP). On that
// base: the compare reading `session->` while the switch uses `s` 50; id read once through its address 47;
// a member pointer local 47; a `client` copy of this 47; cases 2/4/6 grouped 47. The file is /O2 /Gr and
// 0x58d90's own file /O2 /Ob0, which keeps it out of line here where retail inlines it (a flags lead for
// the user, not tried).
// Round 25 (lane 17, fast): 0x6d380 as a __stdcall member of c_session_client (definition, declaration and
// both calls) 43 (same code); the compare and call inside the switch's default with cases 2/4/6 breaking 43;
// early returns on the mode/type test and on an equal id 43; those early returns with separate case 7 and 8
// bodies 47; the in-switch compare with separate case 7 and 8 bodies 47. In every form mode and member index
// take a callee-saved register because this already holds EDX; retail colours mode/index first (EDX).
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

#pragma inline_depth(0)
PRIVATE __forceinline void function_6dfa1(s_session_owner *arg_0)
{
 function_06df60(arg_0, 1, 0, 0);
}
#pragma inline_depth(255)

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
			function_06df60(owner, 1, 0, 0);
		}
		else if (function_058d90(b))
		{
			function_6dfa1(owner);
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
			function_06df60(owner, 0, 0, 0);
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
	if (countdown >= 0)
	{
		long local_0 = unknown14;
		if (session_time_get() - local_0 >= 1000 && countdown > 0)
		{
			unknown14 = session_time_get();
			unknown18 = --countdown;
			network_session_start_countdown(session, true, countdown, 0, NULL);
		}
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
		transport_get_random_bytes(sizeof(id), (byte *)id);
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

#pragma inline_depth(0)
// @retail 0x6dcc0
bool c_session_client::function_06dcc0(s_session_remote *remote)
{
	bool result = true;
	c_class_58d20 *current = session;
	long index = NONE;
	if (current->state > 2 && current->state <= 8)
		index = current->value49c8;
	if (!function_192e60(index) || remote->unknown170 > remote->unknown16c)
	{
		result = false;
	}
	return result;
}
#pragma inline_depth(255)

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
	__int64 arg_2, bool free_for_all);

#pragma inline_depth(0)
PRIVATE __forceinline __int64 function_72170(c_class_58d20 *arg_0)
{
	return arg_0->get_values_4da0();
}
#pragma inline_depth(255)

PRIVATE __forceinline bool function_72170(s_session_id arg_0, s_session_id arg_1)
{
	return *(__int64 *)&arg_0 != *(__int64 *)&arg_1;
}

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
			if (function_72170(g_510520, key) ||
				!function_73b10(g_510540.a, g_510540.b))
			{
				byte *variant = NULL;
				if (session_state_is_live(session))
					variant = session->data4db0;
				bool free_for_all = (variant[0x48] & 1) != 0;
				const s_session_id *id = NULL;
				if (session->state && session->flag24)
					id = (const s_session_id *)&session->unknown1c;
				function_73bc0(id, &key, function_72170(session), free_for_all);
			}
		}
		else if (g_510518)
		{
			g_51051c = true;
			g_51051d = true;
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

#pragma inline_depth(0)
static __forceinline void function_72349(c_class_58d20 *arg_0)
{
	network_session_host_clear_flag49fc(arg_0);
}

static __forceinline long function_724a6(s_surface_description *arg_0)
{
	return function_1932c0(arg_0);
}
#pragma inline_depth(255)

// 0x1932c0 stays out of line through the wrapper above (inlined in the default arm otherwise),
// and the barrier keeps the inlined 0x193440 reloading the type.
// @retail 0x72330
void function_72330(c_session_state_matchmaking *state)
{
	s_session_owner *owner = state->owner;
	c_class_58d20 *session = owner->session_a;
	long member = session->member_index;
	s_member_quality_collection *members = (s_member_quality_collection *)&session->value4c;
	s_matchmaking_player_values *values = (s_matchmaking_player_values *)state;
	function_72349(session);
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
		bool depth_type = type == 5 || type == 3 || type == 4;
		if (depth_type)
		{
			_ReadWriteBarrier();
			if (function_193440(variant) > 0 && function_193340(variant) > 0)
			{
				long count = level / function_193440(variant);
				if (count >= function_193340(variant))
				{
					if (count > function_193370(variant)) count = function_193370(variant);
					values->limit = function_193440(variant) * count;
				}
			}
		}
		else
		{
			if (level >= function_724a6(variant))
			{
				if (level > function_193300(variant)) level = function_193300(variant);
				values->limit = level;
			}
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
#pragma function(_ReadWriteBarrier)

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
			switch (session->type)
			{
			case 2:
			case 3:
			case 4:
				function_06df60(owner, 7, 0, 0);
				state->mode = 2;
				local_0 = true;
				break;
			case 5:
				function_06df60(owner, 9, 0, 0);
				state->mode = 2;
				local_0 = true;
				break;
			}
		}
	}
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
		long &value = seed;
		value ^= GetTickCount();
		value ^= (long)now;
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
	bool result = false;
	s_session_owner *owner = state->owner;
	c_class_58d20 *session = owner->session_a;
	c_class_58d20 *other = owner->session_b;
	if (session->type == 10)
	{
		function_70b70(state);
		if (session->function_058d20())
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
						network_session_parameters_set_value4d08(other, NONE, map_id, (const char *)map + 0xb4c);
						network_session_host_set_summary(other, (const s_session_summary *)((byte *)state + 0x630));
						network_session_parameters_set_value4dac(other, value);
						network_session_parameters_set_data4db0(other, (const s_session_data4db0 *)settings);
						s_session_id random_id;
						transport_get_random_bytes(8, (byte *)&random_id);
						other->set_values_4da0(random_id.a, random_id.b);
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

static __forceinline void function_702c0(byte *arg_0, s_matchmaking_summary *arg_1)
{
	arg_1->player_count = 0;
	dword local_0 = *(dword *)(arg_0 + 0x10d0);
	for (long local_1 = 0; local_1 < 16; local_1++)
	{
		if (local_0 & (1 << local_1))
		{
			arg_1->player_users[arg_1->player_count] = *(s_matchmaking_identity *)(arg_0 + 0x10d4 + local_1 * 0x13c);
			arg_1->first[arg_1->player_count] = NONE;
			arg_1->second[arg_1->player_count] = NONE;
			arg_1->third[arg_1->player_count] = 0;
			arg_1->player_count++;
		}
	}
}

// @retail 0x70280
void function_70280(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	byte *local_0 = (byte *)&session->value4c;
	s_matchmaking_summary summary;
	memset(&summary, 0, sizeof(summary));
	function_702c0(local_0, &summary);
	summary.machine_count = 1;
	const s_session_id *id = NULL;
	if (session->state && session->flag24)
		id = (const s_session_id *)&session->unknown1c;
	summary.machine_ids[0] = *id;
	function_7ed20((const s_message_identities *)&session->value4c,
		(s_message_identity *)&summary.machine_users[0], NULL, NULL);
	state->mode = 1;
	if (session->function_058d20())
	{
		network_session_host_set_summary(session, (const s_session_summary *)&summary);
		network_session_set_mode(session, 6);
		state->mode = 3;
	}
	else
	{
		if (network_session_parameters_set_summary(session, (const s_session_summary *)&summary) &&
			network_session_parameters_set_mode(session, 6))
			state->mode = 3;
	}
}

long g_55e6fc;
long online_get_nat_type(void);
long function_75890(long time);

/* once the live session's search is done, picks hosting or joining and
   starts it: true when it made the choice */
// @retail 0x70d60
bool function_70d60(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	bool result = false;
	function_70b70(state);
	if (session->function_058d20() && state->mode == 3)
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
			if (!host)
			{
				bool restricted = g_network_configuration.flag1ac;
				if (*(long *)((byte *)state + 0x938) == NONE)
					host = true;
				if (!host && restricted && online_get_nat_type() == 3)
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
            bool ready = function_06e720(b) && function_06e6b0(b, &unknown14);
            if (ready)
            {
                if (!unknown18)
                    unknown18 = function_75870();
                long remaining_raw = 10000 - function_75890(unknown18);
                long remaining = remaining_raw < 0 ? 0 : remaining_raw;
                network_session_host_become_leader(b);
                network_session_start_countdown(b, true, (remaining + 999) / 1000, 0, NULL);
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
    if (a->function_058d20())
        network_session_host_set_value49f8(a, mode);
    return result;
}

long function_66050(c_class_58d20 *session, long variant_index);
bool network_session_members_ready(c_class_58d20 *session, dword *unready_mask);

// @retail 0x6e410
bool c_session_state_pre_game::function_06e410()
{
 c_class_58d20 *session = owner->session_a;
 long countdown = session_get_countdown(session);
 if (!countdown)
 {
  long remaining = NONE;
  if (SESSION_STATE_IS_LIVE(session->state))
   remaining = session->value49a4;
  if (remaining > 1)
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
   if (network_session_members_ready(session, NULL))
    network_session_set_mode(session, 2);
   else
    network_session_start_countdown(session, true, 1, 2, 0);
  }
 }
 return false;
}
