/* UNKNOWN_058EE0.H: the session manager's owner (0x527334) and its
   states as the manager's functions see them (lane D) */

#ifndef UNKNOWN_058EE0_H
#define UNKNOWN_058EE0_H

#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_058dd0.h"
#include "network_message_types.h"

/* a session found by a search (only the fields the joining state reads) */
struct s_session_description
{
	short unknown00;
	short unknown02;
	long unknown04;
	long unknown08;
	long unknown0c;
	short unknown10;
	byte unknown12[2];
	short unknown14;
	byte unknown16[0x58 - 0x16];
	XNKID kid;
	XNKEY key;
	XNADDR address;
	short unknown94;
	short unknown96;
	byte unknown98[6];
	short unknown9e;
};

/* the session owner (0x527334) as these functions see it */
struct s_session_owner_view
{
	long mode;
	c_session_state *states[10];
	void *unknown2c;
	c_class_58d20 *session_a;
	c_class_58d20 *session_c;
	c_class_58d20 *session_b;
	void *unknown3c;
	long unknown40;
	long unknown44;
	bool unknown48;
	bool unknown49;
	bool failed;
	byte unknown4b;
	long error_code;
	long data_size;
	byte data[1];
};

/* the joining state's fields */
struct s_session_state_joining_view
{
	void *vtable;
	long index;
	s_session_owner_view *owner;
	bool skip_cleanup;
	bool unknown0d;
	byte unknown0e[2];
	bool unknown10;
	bool unknown11;
	byte target[0x56];
	bool unknown68;
	byte unknown69[3];
	s_parameters_part part;
	long entry_count;
	byte entries[0x30];
	long unknowne4;
	bool unknowne8;
	bool unknowne9;
	byte unknownea[2];
	long unknownec;
	long unknownf0;
	long unknownf4;
	bool unknownf8;
	bool unknownf9;
	byte unknownfa[2];
	long unknownfc;
	long unknown100;
	long unknown104;
};

/* the fields every state has */
struct s_session_state_view
{
	void *vtable;
	long index;
	s_session_owner_view *owner;
	bool skip_cleanup;
	bool unknown0d;
	byte unknown0e[2];
	void *unknown10;
	long unknown14;
};

/* the matchmaking state's fields */
struct s_session_state_matchmaking_view
{
	void *vtable;
	long index;
	s_session_owner_view *owner;
	bool skip_cleanup;
	bool unknown0d;
	byte unknown0e[2];
	byte unknown10[0x96c - 0x10];
	long unknown96c;
	long unknown970;
	long unknown974;
	long mode;
	bool unknown97c;
	byte unknown97d[0x9ec - 0x97d];
	long unknown9ec;
	long unknown9f0;
	long unknown9f4;
	byte unknown9f8[0xa08 - 0x9f8];
	long unknowna08;
	long unknowna0c;
	byte unknowna10[0xa1c - 0xa10];
	long unknowna1c;
	byte unknowna20[0xa64 - 0xa20];
	bool unknowna64;
	byte unknowna65[0xa78 - 0xa65];
	bool unknowna78;
	byte unknowna79[0xa80 - 0xa79];
	long unknowna80;
	long unknowna84;
	long unknowna88;
	long unknowna8c;
	long unknowna90;
	long unknowna94;
};

struct s_597d0_object;

/* the session manager (0x527330): its owner (+0x04, the states' owner),
   embedded states and client */
struct s_session_states
{
	byte initialized;
	byte unknown01[3];
	long state;
	c_session_state *states[10];
	void *unknown30;
	s_597d0_object *session_a;
	s_597d0_object *session_c;
	s_597d0_object *session_b;
	void *unknown40;
	long unknown44;
	long unknown48;
	bool unknown4c;
	bool unknown4d;
	bool failed;
	byte unknown4f;
	long error_code;
	long data_size;
	byte data58[4];
	c_session_state_none state_none;
	c_session_state_pre_game state_pre_game;
	c_session_state_start_game state_start_game;
	c_session_state_in_game state_in_game;
	c_session_state_post_game state_post_game;
	c_session_state_joining state_joining;
	c_session_state_matchmaking state_matchmaking;
	byte unknownc50[0xc68 - 0xc50];
	c_session_state_start_match state_start_match;
	byte unknownc84[4];
	c_session_state_in_match state_in_match;
	c_session_state_post_match state_post_match;
	c_session_client client;

	s_session_states();
};

extern s_session_states g_527330;

/* sets up a state of the owner */
inline void session_state_initialize(s_session_state_view *state, s_session_owner_view *owner, long index, bool unknown0d, bool skip_cleanup)
{
	state->owner = owner;
	state->index = index;
	state->unknown0d = unknown0d;
	state->skip_cleanup = skip_cleanup;
	state->owner->states[index] = (c_session_state *)state;
}

/* takes a state out of its owner */
inline void session_state_dispose(s_session_state_view *state)
{
	state->owner->states[state->index] = NULL;
	state->owner = NULL;
}

/* src/network_session_client.cpp */
void session_owner_initialize(s_session_owner *owner_, long unknown40, long unknown44, void *unknown2c, c_class_58d20 *session_a, c_class_58d20 *session_c, c_class_58d20 *session_b, void *unknown3c);
void session_state_joining_initialize(c_session_state_joining *state_, s_session_owner *owner);
void session_state_matchmaking_initialize(c_session_state_matchmaking *state_, s_session_owner *owner);

#endif