/* UNKNOWN_058DD0.H: the game-session state machine states (the vtables at
   0x450990) and the session structure they drive */

#ifndef UNKNOWN_058DD0_H
#define UNKNOWN_058DD0_H

#include "unknown_11c920.h"
#include "unknown_059ad0.h"

/* the object the states act on (state +8): sessions and a message buffer */
struct s_session_owner
{
	byte unknown00[0x30];
	c_class_58d20 *session_a;
	c_class_58d20 *session_c;
	c_class_58d20 *session_b;
	byte unknown3c[0x4a - 0x3c];
	byte failed;
	byte unknown4b;
	long error_code;
	long data_size;
	long data[1];
};

/* the remote machine a session client talks to (only the fields it reads) */
struct s_session_remote
{
	long id;
	byte address04[0x140];
	byte address144[8];
	byte has_address;
	byte unknown14d[0x16c - 0x14d];
	long unknown16c;
	long unknown170;
	byte unknown174[0x188 - 0x174];
	dword key188[9];
};

/* a request the client queues (0x1c8 bytes; lane D, network_session_client.cpp) */
struct s_session_request
{
	s_session_request *next;
	dword key04[5];
	s_session_remote remote;
	byte unknown1c4[4];
};

/* the client of a session: slot 0 connects, slot 1 checks an id, slot 3 asks
   whether the address is known */
class c_session_client
{
public:
	virtual void function_06dae0(long a, s_session_remote *remote);
	virtual void function_06dbd0(const s_session_id *id);
	virtual void function_06dbc0();
	virtual bool function_06daa0(long a);

	bool function_06dcc0(s_session_remote *remote);
	bool function_06de10(s_session_remote *remote);
	bool function_06dd00(long a, s_session_remote *remote);

	c_class_58d20 *session;
	long mode;
	s_session_request *requests;
	long request_count;
};

/* a snapshot of a session member, built by 05a620 and compared with the live one */
struct s_session_snapshot
{
	long unknown00;
	byte unknown04[8];
	byte unknown0c[0x10];
	dword words[9];
	long unknown40;
};

/* the common base of the states: slot 0 updates the state, slot 1 enters it,
   slot 2 leaves it and slot 3 names it */
class c_session_state
{
public:
	virtual bool update() { return false; }
	virtual void enter(long a, long b, long c);
	virtual void leave(long a) {}
	virtual const char *function_58dd0() { return 0; }

	bool function_06dfa0();
	void function_06ec10(c_class_58d20 *s);

	long index;
	s_session_owner *owner;
	bool skip_cleanup;
	bool unknown0d;
};

/* in-game */
class c_session_state_in_game : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual void leave(long a);
	virtual const char *function_58dd0();

	long id_a;
	long id_b;
};

/* in-match */
class c_session_state_in_match : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual void leave(long a);
	virtual const char *function_58dd0();

	long time;
	long unknown14;
	long id_a;
	long id_b;
};

/* start-match */
class c_session_state_start_match : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual void leave(long a);
	virtual const char *function_58dd0();

	void function_072950();

	long mode;
	byte unknown14;
	byte unknown15[3];
	long unknown18;
};

/* pre-game */
class c_session_state_pre_game : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual const char *function_58dd0();

	bool function_06e360();
	bool function_06e410();

	byte unknown0d[4];
	long unknown14;
	long unknown18;
	long unknown1c;
};

/* matchmaking */
class c_session_state_matchmaking : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual void leave(long a);
	virtual const char *function_58dd0();

	void function_070c50(bool flag);
	void function_070d20(bool flag);

	byte flag10;
	byte unknown11[3];
	long unknown14[0x254];
	long time;
	long unknown968;
	byte unknown96c[4];
	long unknown970;
	byte unknown974[4];
	long mode;
	byte flag97c;
	byte unknown97d[0xa64 - 0x97d];
	byte flaga64;
	byte unknowna65[3];
	long unknowna68;
	byte unknowna6c[0xa78 - 0xa6c];
	byte flaga78;
	byte unknowna79[3];
	long unknowna7c;
};

/* the matchmaking state's player summary and rank values (0x72330, 0x72540) */
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

/* post-match */
class c_session_state_post_match : public c_session_state
{
public:
	virtual bool update();
	virtual const char *function_58dd0();
};

/* none */
class c_session_state_none : public c_session_state
{
public:
	virtual bool update();
	virtual const char *function_58dd0();
};

/* start-game */
class c_session_state_start_game : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual const char *function_58dd0();

	byte flag10;
};

/* joining */
class c_session_state_joining : public c_session_state
{
public:
	virtual bool update();
	virtual void enter(long a, long b, long c);
	virtual void leave(long a);
	virtual const char *function_58dd0();

	void function_06f0f0();
	void function_06f1a0();
	void function_06f200(bool flag, const void *target, long count, const void *entries);
	void function_06f2b0(const struct s_session_description *description, long count);
	void function_06f3a0(const struct s_session_description *description, long count, const void *entries);

	byte flag10;
	byte unknown11[0xe9 - 0x11];
	bool flage9;
	byte unknownea[0xf8 - 0xea];
	byte flagf8;
	byte unknownf9[3];
	long unknownfc;
	long unknown100;
	long unknown104;
};

/* post-game */
class c_session_state_post_game : public c_session_state
{
public:
	virtual bool update();
	virtual const char *function_58dd0();
};

/* callees that are not decompiled yet */
long __stdcall function_063190(void *p, long a);
long __stdcall function_0632e0(void *p, void *q);
bool function_063510(void *p, void *a, long x);
void __stdcall function_06d380(c_session_client *client, const s_session_id *id);
void __stdcall function_06dc60(c_session_client *client, long n);
bool __stdcall function_07b140(void *x, long a, long ten, long twelve, void *local);
bool function_058d90(c_class_58d20 *s);
bool function_06ec80(c_class_58d20 *s, bool flag);
bool function_06e6b0(c_class_58d20 *s, byte *p);
bool function_06e720(c_class_58d20 *s);
void function_06df60(s_session_owner *o, long a, long b, long c);
bool function_0682c0();
bool function_058d50(c_class_58d20 *s);
void __stdcall function_090c80(byte *p);
void function_06f4b0(c_session_state_joining *self);
void function_06f700(c_session_state_joining *self);
void function_06fcc0(c_session_state_joining *self);
bool function_058d70(c_class_58d20 *s);

/* the session is live (its state is 3..8): the session code inlines this
   test, while other code calls function_058d70 */
static inline bool session_state_is_live(c_class_58d20 *s)
{
	bool result = false;
	long state = s->state;
	if (state > 2)
	{
		if (state <= 8)
			result = true;
	}
	return result;
}
void function_06e620(c_class_58d20 *s);
bool game_loop_running();
bool game_finish_countdown_done();
void game_finish();
void function_068750();

#endif
