#ifndef ONLINE_ROUND_STATE_H
#define ONLINE_ROUND_STATE_H

/* ONLINE_ROUND_STATE.H: the online arbitration round of the current session (0x510514..0x510547). One object in
   retail: 0x73bc0 sets every member in one sequence when it registers a round; 0x73b10 reads the round's status
   from it, 0x73dc0 ends it, 0x73ca0 reports the round and 0x755d0 polls the register, report and extend tasks.
   0x59940 passes the address of the session id at 0x510528 to the session lookup (0x75800) and reads the state at
   0x510518 again after that call; 0x598f0 compares through the same address. */

#include "unknown_059ad0.h"
#include <stddef.h>

struct s_online_round_state
{
	/* why the last round ended, read by 0x73b10 while no round runs: 0 reported, 1 registering failed,
	   2 reporting failed, 3 extending failed, 4 replaced by a new round (0x73bc0) */
	long end_reason;
	/* 0 no round, 1 registering, 2 registered (extended on a timer), 3 reporting */
	long state;
	/* set while a round runs when it has to end early: 0x755d0 then reports a registered round with no results */
	bool unknown_51051c;
	/* passed with the report (online_round_report's last argument) */
	bool unknown_51051d;
	/* the round key 0x73bc0 registers the round with, passed again to the report (0x73ca0) and extend (0x755d0) calls */
	s_session_id round_key;
	/* the id of the session the round belongs to */
	s_session_id session_id;
	/* the pending register or report task */
	long task;
	/* when the round was registered or last extended */
	long extend_time;
	/* the pending extend task */
	long extend_task;
	long unknown_51053c;
	/* the 64-bit value 0x73bc0's caller passes; a game's results (+0x130) belong to this round only when they carry it */
	s_session_id unknown_510540;
};

extern s_online_round_state g_online_round_state;
#define g_510514 (g_online_round_state.end_reason)
#define g_510518 (g_online_round_state.state)
#define g_51051c (g_online_round_state.unknown_51051c)
#define g_51051d (g_online_round_state.unknown_51051d)
#define g_510520 (g_online_round_state.round_key)
#define g_510528 (g_online_round_state.session_id)
#define g_510530 (g_online_round_state.task)
#define g_510534 (g_online_round_state.extend_time)
#define g_510538 (g_online_round_state.extend_task)
#define g_510540 (g_online_round_state.unknown_510540)

typedef char assert_online_round_state[(offsetof(s_online_round_state, state) == 4 &&
	offsetof(s_online_round_state, unknown_51051c) == 8 && offsetof(s_online_round_state, unknown_51051d) == 9 &&
	offsetof(s_online_round_state, round_key) == 0xc && offsetof(s_online_round_state, session_id) == 0x14 &&
	offsetof(s_online_round_state, task) == 0x1c && offsetof(s_online_round_state, extend_time) == 0x20 &&
	offsetof(s_online_round_state, extend_task) == 0x24 && offsetof(s_online_round_state, unknown_51053c) == 0x28 &&
	offsetof(s_online_round_state, unknown_510540) == 0x2c && sizeof(s_online_round_state) == 0x34) ? 1 : -1];

#endif
