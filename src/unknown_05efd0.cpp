// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_05EFD0.CPP: the host takes a player a peer adds, or refuses it.
   Retail keeps all three arguments on the stack (ret 0xc); its only caller
   is the message handler's 0x94700. The disabled copy in
   src/unknown_059ad0.cpp is the same body. */
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_075870.h"
#include "network_message_types.h"
#include "unknown_058dd0.h"
#include "unknown_0662e0.h"

struct s_network_message_player_add
{
	s_session_id session_id;
	long slot;
	dword identity[3];
	long unknown18;
	byte properties[0x90];
	long unknownac;
};

struct s_network_message_player_refuse
{
	s_session_id session_id;
	long slot;
	dword identity[3];
};

long network_session_find_member_by_channel(c_class_58d20 *session, long channel_index);
long network_session_add_local_player(c_class_58d20 *session, long member_index, long slot, const dword *identity);

// Matched in a full build with stand-in SDK headers (was 68), without the volatile: data is a plain pointer, the two
// per-arm re-assignments of message are gone, and result is set once after the if/else instead of in each arm. With one
// assignment after the join nothing jumps into the success arm's epilogue, so its store is scheduled between the pops
// as in retail (0x5f083) and the refuse arm gets its own tail.
// @retail 0x5efd0 standard
bool __stdcall network_session_handle_player_add(c_class_58d20 *session, long remote_index, const void *data)
{
	bool result = false;

	if (session_state_is_live(session) && session->function_058d20())
	{
		long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
		long member_index = network_session_find_member_by_channel(session, channel_index);
		if (member_index != NONE && member_index != session->current_member)
		{
			const s_network_message_player_add *message = (const s_network_message_player_add *)data;
			const dword *identity = message->identity;
			long player_index = network_session_add_local_player(session, member_index, message->slot, identity);
			if (player_index != NONE)
			{
				s_network_session_player *player = &session->players[player_index];
				player->unknown14 = message->unknown18;
				memcpy(player->properties18, message->properties, sizeof(player->properties18));
				player->unknown138 = message->unknownac;
			}
			else
			{
			s_network_message_player_refuse refuse;
			memset(&refuse, 0, sizeof(refuse));
			refuse.session_id = *(s_session_id *)&session->unknown1c;
			refuse.slot = message->slot;
			refuse.identity[0] = identity[0];
			refuse.identity[1] = identity[1];
			refuse.identity[2] = identity[2];
			s_network_session_member_state *state = &session->member_states[member_index];
			if (state->flag1)
				network_observer_send_message(session->observer, session->value10, state->unknown04, false, _network_message_type_player_refuse, sizeof(refuse), &refuse);
			}
			result = true;
		}
	}
	return result;
}
