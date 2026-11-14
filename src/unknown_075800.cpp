#include "unknown_4cf790.h"
// @flags /O2 /Gr
/* UNKNOWN_075800.CPP: the handler the message gateway (0x93aa0)
   passes each incoming session message to. Every session message begins
   with the identifier of the session it is for; the handler looks the
   session up in the session manager and passes the message on to the
   session code (src/unknown_059ad0.cpp, lane D). */

#include "unknown_11c920.h"
#include <string.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_0820f0.h"
#include "unknown_07aec0.h"
#include "unknown_058dd0.h"
#include "unknown_067e10.h"

/* the messages (src/unknown_059ad0.cpp) */
struct s_network_message_handoff;
struct s_network_message_peer_properties;
struct s_network_message_peer_identity;
struct s_network_message_player_refuse;
struct s_network_message_player_remove;
struct s_network_message_player_properties;
struct s_network_message_parameters_request;
struct s_network_message_countdown_timer;
struct s_network_message_mode_acknowledge;

/* src/unknown_059ad0.cpp */
long network_session_find_member_by_address(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_channel_is_host(c_class_58d20 *session, long remote_index);
bool network_session_handle_mode_acknowledge(c_class_58d20 *session, const s_network_message_mode_acknowledge *message, long remote_index);
void network_session_close(c_class_58d20 *session);
bool network_session_channel_is_host_address(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_address_is_leaving_host(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_handle_countdown_timer(const s_network_message_countdown_timer *message, c_class_58d20 *session, long remote_index);
bool network_session_handle_session_disband(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_handle_session_boot(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_handle_channel_closed(c_class_58d20 *session, long remote_index);
bool network_session_handle_peer_ready(c_class_58d20 *session, long remote_index);
bool network_session_handle_player_refuse(c_class_58d20 *session, const s_network_message_player_refuse *message, long remote_index);
bool network_session_handle_player_remove(c_class_58d20 *session, long remote_index, const s_network_message_player_remove *message);
bool network_session_handle_peer_reestablish(c_class_58d20 *session, const s_type_99af70 *address);
bool __stdcall network_session_handle_peer_properties(c_class_58d20 *session, long remote_index, const s_network_message_peer_properties *message);
bool __stdcall network_session_handle_delegate_leader(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message);
bool network_session_handle_host_handoff_acknowledge(c_class_58d20 *session, long remote_index, const byte *message);
bool __stdcall network_session_handle_boot_machine(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message);
void network_session_handle_join_abort_reply(c_class_58d20 *session, const s_type_99af70 *address);
bool network_session_member_leave(c_class_58d20 *session, long member_index);
bool network_session_handle_parameters_request(const s_network_message_parameters_request *message, c_class_58d20 *session, long remote_index);
bool __stdcall network_session_handle_host_handoff(c_class_58d20 *session, const s_network_message_handoff *message);
bool network_session_handle_player_properties(c_class_58d20 *session, long remote_index, const s_network_message_player_properties *message);

/* the link's send (src/stubs/session.cpp) */
bool __stdcall function_07b140(void *x, long a, long ten, long twelve, void *local);

/* src/unknown_0820f0.cpp */
void link_connection_close(s_link_connection *connection, long reason);

/* lane D's session handlers, not decompiled yet (src/stubs/lane_j.cpp) */
void __stdcall function_061570(c_class_58d20 *session, bool flag);
bool __stdcall function_05e3f0(c_class_58d20 *session, const s_type_99af70 *address);
bool __stdcall function_05cb80(c_class_58d20 *session, const void *message);
bool __stdcall network_session_handle_player_add(c_class_58d20 *session, long remote_index, const void *volatile message);
bool __stdcall function_05d9e0(c_class_58d20 *session, const void *message);
bool network_session_handle_peer_establish(c_class_58d20 *session, long remote_index);
bool __stdcall function_05e7f0(c_class_58d20 *session, const s_type_99af70 *address, const void *message, long *reason, bool *has_identity, s_session_member_identity *identity);

/* the session disband and boot handlers below are kept out of the build (see
   there); the dispatcher calls stubs of them (src/stubs/lane_j.cpp) */
class c_class_938e0;
void __stdcall function_094310(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address);
void __stdcall function_094330(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address);

/* src/unknown_075870.cpp, src/unknown_0820f0.cpp */
inline __declspec(noinline) bool link_connection_get_address(s_link_connection *connection, s_type_99af70 *address);
void link_connection_establish(s_link_connection *connection, long remote_sequence);

/* lane D's region: a connection's reconnect (src/stubs/lane_j.cpp) */
void link_connection_connect(const s_type_99af70 *address, s_link_connection *connection, bool initiator);

struct s_type_3de205
{
	long identifier;
	long remote_identifier;
};

void network_message_handle_connect_establish(long connection_index, const s_type_3de205 *message);

/* the system link globals (0x4d8eb4): whether this machine advertises a
   game, and the game's session */
struct s_system_link_globals
{
	bool active;
	byte unknown01[7];
	s_session_id session_id;
};
#include "system_link_globals.h"
s_network_link_globals g_4d8ea8;

/* a broadcast query about a session: identifier 2, then the session */
struct s_network_message_session_query
{
	word identifier;
	byte unknown02[2];
	s_session_id session_id;
};

/* not decompiled yet (src/stubs/lane_j.cpp) */
void __stdcall function_0b2fc0(const s_network_message_session_query *message);
bool __stdcall function_063080(c_class_58d20 *session, const s_network_message_session_query *message, const s_type_99af70 *address);
bool __stdcall network_session_handle_time_synchronize(const s_session_id *message, c_class_58d20 *session, const s_type_99af70 *address);
void __stdcall function_0785d0(void *unknown10, const s_type_99af70 *address, const void *message);
class c_class_938e0;
void __stdcall function_093fa0(c_class_938e0 *handler, const void *message);
void __stdcall function_094220(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address);
void __stdcall function_0942d0(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address);

/* src/unknown_092870.cpp */
class c_class_93590;
long network_link_find_connection(c_class_93590 *link, long kind, const s_type_99af70 *address);

bool network_session_manager_get_hosted_session(c_class_58d20 **arg_0);
struct s_session_browser_summary;
bool __stdcall function_64d70(s_session_browser_summary *arg_0);
bool function_07a9b0(void);


/* the sessions the session manager owns */
struct s_network_session_list
{
	c_class_58d20 *sessions[3];
};

class c_class_938e0
{
public:
	void handle_join_abort(const s_session_id *message, const s_type_99af70 *address);
	void function_94270(const s_session_id *message, const s_type_99af70 *address);
	void function_x769765(const s_session_id *message, const s_type_99af70 *address);
	void handle_session_disband(const s_session_id *message, const s_type_99af70 *address);
	void handle_session_boot(const s_session_id *message, const s_type_99af70 *address);
	void handle_host_handoff(const s_network_message_handoff *message, long remote_index);
	void handle_host_handoff_acknowledge(const byte *message, long remote_index);
	void handle_channel_closed(const s_session_id *message, long remote_index);
	void handle_peer_ready(const s_session_id *message, long remote_index);
	void handle_peer_reestablish(const s_session_id *message, const s_type_99af70 *address);
	void handle_peer_properties(const s_network_message_peer_properties *message, long remote_index);
	void handle_delegate_leader(const s_network_message_peer_identity *message, long remote_index);
	void handle_boot_machine(const s_network_message_peer_identity *message, long remote_index);
	void handle_player_refuse(const s_network_message_player_refuse *message, long remote_index);
	void handle_player_remove(const s_network_message_player_remove *message, long remote_index);
	void handle_player_properties(const s_network_message_player_properties *message, long remote_index);
	void handle_parameters_request(const s_network_message_parameters_request *message, long remote_index);
	void handle_countdown_timer(const s_network_message_countdown_timer *message, long remote_index);
	void handle_mode_acknowledge(const s_network_message_mode_acknowledge *message, long remote_index);
	void function_943d0(const s_session_id *message, long remote_index);
	void function_94420(const s_session_id *message, const s_type_99af70 *address);
	void function_944a0(const s_session_id *message, long remote_index);
	void function_94520(const s_session_id *message, const s_type_99af70 *address);
	void function_94610(const s_session_id *message, long remote_index);
	void function_94700(const s_session_id *message, long remote_index);
	void function_94800(const s_session_id *message, long remote_index);
	void function_93aa0(long channel_index, long message_type, long message_size, const void *message);
	void function_938e0(const s_type_99af70 *address, long message_type, const void *message);
	void function_94100(const struct s_network_message_session_query *message, const s_type_99af70 *address);
	void function_945f0(const s_session_id *message, const s_type_99af70 *address);
	void handle_view_establishment(const struct s_type_22101d *message, long channel_index);
	void handle_player_acknowledge(const struct s_type_0bbd2f *message, long channel_index);
	void handle_synchronous_update(const s_simulation_block_data *message, long channel_index);
	void handle_player_update(const struct s_network_message_player_update *message, long channel_index);
	void handle_join_data_begin(const struct s_network_message_join_data_begin *message, long channel_index);
	void handle_join_data(long channel_index, const struct s_network_message_join_data *message, long data_size, const byte *data);
	void handle_baseline_update(const struct s_network_message_baseline_update *message, long channel_index);

	byte unknown00[4];
	class c_class_93590 *field_4_5;
	byte unknown08[4];
	void *link;
	void *unknown10;
	s_network_session_list *session_manager;
};

// @retail 0x75800
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *session_id)
{
	c_class_58d20 *local_0 = NULL;
	for (dword i = 0; i < sizeof(manager->sessions) / sizeof(manager->sessions[0]); i++)
	{
		c_class_58d20 *session = manager->sessions[i];
		if (session && session->state != 0 && session->flag24)
		{
			s_session_id id = *(s_session_id *)&session->unknown1c;
			if (memcmp(&id, session_id, sizeof(s_session_id)) == 0)
			{
				local_0 = manager->sessions[i];
				break;
			}
		}
	}
	return local_0;
}

/* the reply to a connect request */
struct s_connect_request
{
	word identifier;
	byte unknown02[2];
	long sequence;
};

struct s_network_message_connect_reply
{
	word identifier;
	byte unknown02[2];
	long sequence;
	long reason;
};

// @retail 0x93f60
void network_message_handler_refuse_connect(const s_connect_request *request, c_class_938e0 *handler, long address)
{
	s_network_message_connect_reply reply;
	reply.identifier = request->identifier;
	reply.sequence = request->sequence;
	reply.reason = 2;
	function_07b140(handler->link, address, 1, sizeof(reply), &reply);
}

// @retail 0x940b0
void __stdcall network_message_handler_handle_session_query(const s_network_message_session_query *message, const s_type_99af70 *address)
{
	const s_type_99af70 *const *address_reference = &address;
	(void)*address_reference;
	if (message->identifier == 2 && g_4d8eb4.active)
	{
		s_session_id id = g_4d8eb4.session_id;
		if (memcmp(&message->session_id, &id, sizeof(s_session_id)) == 0)
			function_0b2fc0(message);
	}
}

struct s_93fa0
{
	word field_0;
	word field_2;
	s_session_id field_4;
	byte field_c[0x714];
};

// @retail 0x93fa0
void __stdcall function_093fa0(c_class_938e0 *arg_0, void const *arg_1)
{
	s_network_message_session_query const *local_0 = (s_network_message_session_query const *)arg_1;
	if (local_0->identifier == 2)
	{
		bool local_7 = false;
		if (g_4d8eb4.active)
		{
			s_session_id local_1 = g_4d8eb4.session_id;
			local_7 = memcmp(&local_0->session_id, &local_1, sizeof(local_1)) == 0;
		}
		c_class_58d20 *local_2;
		if (!local_7 && network_session_manager_get_hosted_session(&local_2))
		{
			__int64 local_3[0xe3];
			if (function_64d70((s_session_browser_summary *)local_3))
			{
				if (g_transport_globals.initialized && g_transport_globals.started)
					function_07a9b0();
				if (g_4cf792)
				{
					s_93fa0 local_4;
					memset(&local_4, 0, sizeof(local_4));
					local_4.field_4 = local_0->session_id;
					void *local_6 = arg_0->link;
					local_4.field_0 = 2;
					memcpy(local_4.field_c, local_3, sizeof(local_4.field_c));
					s_type_99af70 local_5;
					local_5.address_length = 4;
					local_5.ipv4_address = NONE;
					local_5.port = 1001;
					function_07b140(local_6, (long)&local_5, 3, sizeof(local_4), &local_4);
				}
			}
		}
	}
}

// @retail 0x94100
void c_class_938e0::function_94100(const s_network_message_session_query *message, const s_type_99af70 *address)
{
	if (message->identifier == 2)
	{
		c_class_58d20 *session = network_session_manager_find_session(session_manager, &message->session_id);
		if (session && session->function_058d20())
			function_063080(session, message, address);
	}
}

/* the reply to a peer leaving a session */
struct s_type_f7e40b
{
	s_session_id session_id;
	long reason;
};

PRIVATE __forceinline void function_94151(s_type_f7e40b *reply, const s_session_id *message, long reason)
{
    memset(reply, 0, sizeof(*reply));
    reply->session_id = *message;
    reply->reason = reason;
}

/* Answers a request to leave a session: 8 when the requesting member has left
   or has nothing to leave (no member, or this machine's own), 9 when leaving failed. */
// @retail 0x94150
void network_message_handler_handle_leave_request(c_class_938e0 *handler, long address, const s_session_id *message)
{
	c_class_58d20 *session = network_session_manager_find_session(handler->session_manager, message);
	if (session && function_058d70(session) && session->function_058d20())
	{
		long member_index = network_session_find_member_by_address(session, (const s_type_99af70 *)address);
		bool success = true;
		if (member_index != NONE && member_index != session->current_member)
			success = network_session_member_leave(session, member_index);
		long reason;
		if (success)
			reason = 8;
		else
			reason = 9;
		s_type_f7e40b reply;
		function_94151(&reply, message, reason);
		function_07b140(handler->link, address, 10, sizeof(reply), &reply);
	}
}

// @retail 0x94220
void __stdcall function_094220(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(handler->session_manager, message);
	if (session)
	{
		if (network_session_channel_is_host_address(session, address))
		{
			if (network_session_channel_is_host_address(session, address))
				network_session_close(session);
		}
		else if (network_session_address_is_leaving_host(session, address))
		{
			network_session_handle_join_abort_reply(session, address);
		}
	}
}

// @retail 0x94270
void c_class_938e0::function_94270(const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && session->function_058d20())
	{
		long member_index = network_session_find_member_by_address(session, address);
		if (member_index != NONE && member_index != session->current_member)
			network_session_member_leave(session, member_index);
	}
}

// @retail 0x942d0
void __stdcall function_0942d0(c_class_938e0 *handler, const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(handler->session_manager, message);
	if (session && function_058d90(session) && session->state == 4 &&
		network_session_find_member_by_address(session, address) == session->member_index)
	{
		network_session_close(session);
	}
}

// @retail 0x94350
void c_class_938e0::handle_host_handoff(const s_network_message_handoff *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && network_session_channel_is_host(session, remote_index))
		network_session_handle_host_handoff(session, message);
}

// @retail 0x94380
void c_class_938e0::handle_host_handoff_acknowledge(const byte *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && function_058d70(session) && session->function_058d20())
		network_session_handle_host_handoff_acknowledge(session, remote_index, message);
}

// @retail 0x943d0
void c_class_938e0::function_943d0(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && network_session_channel_is_host(session, remote_index) && session->value44 == session->current_member)
		function_061570(session, session->type == 4);
}

// @retail 0x94420
void c_class_938e0::function_94420(const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session)
		function_05e3f0(session, address);
}

// @retail 0x94440
void c_class_938e0::handle_channel_closed(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session)
		network_session_handle_channel_closed(session, remote_index);
}

// @retail 0x94460
void c_class_938e0::handle_peer_ready(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && session->function_058d20())
		network_session_handle_peer_ready(session, remote_index);
}

/* the refusal of a join request (0x30 bytes) */
struct s_type_67cfcd
{
	s_session_id session_id;
	byte unknown08[0x30 - 8];
};

// @retail 0x944a0
void c_class_938e0::function_944a0(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (!session || !network_session_handle_peer_establish(session, remote_index))
	{
		s_type_67cfcd reply;
		memset(&reply, 0, sizeof(reply));
		reply.session_id = *message;
		function_095580(network_stream_get(link_connection_get(remote_index)->stream_index), 0x13, sizeof(reply), &reply);
	}
}

/* a member's identity, copied as bytes */
struct s_member_identity_bytes
{
	byte bytes[0x24];
};

/* the reply to a peer's request (0x34 bytes) */
struct s_network_message_peer_reply
{
	s_session_id session_id;
	long reason;
	bool has_identity;
	s_member_identity_bytes identity;
};

// @retail 0x94520
void c_class_938e0::function_94520(const s_session_id *message, const s_type_99af70 *address)
{
	s_member_identity_bytes identity;
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	long reason = 0;
	bool has_identity = false;
	bool skip_reply = false;
	if (session)
	{
		if (function_05e7f0(session, address, message, &reason, &has_identity, (s_session_member_identity *)&identity))
			skip_reply = true;
	}
	else
		reason = 1;
	if (skip_reply)
		return;
	s_network_message_peer_reply reply;
	memset(&reply, 0, sizeof(reply));
	reply.session_id = *message;
	reply.reason = reason;
	reply.has_identity = has_identity;
	if (has_identity)
		reply.identity = identity;
	function_07b140(link, (long)address, 0x17, sizeof(reply), &reply);
}

// @retail 0x945d0
void c_class_938e0::handle_peer_reestablish(const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session)
		network_session_handle_peer_reestablish(session, address);
}

// @retail 0x945f0
void c_class_938e0::function_945f0(const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session)
		network_session_handle_time_synchronize(message, session, address);
}

// @retail 0x94610
void c_class_938e0::function_94610(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && network_session_channel_is_host(session, remote_index))
		function_05cb80(session, message);
}

// @retail 0x94640
void c_class_938e0::handle_peer_properties(const s_network_message_peer_properties *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_peer_properties(session, remote_index, message);
}

// @retail 0x94680
void c_class_938e0::handle_delegate_leader(const s_network_message_peer_identity *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_delegate_leader(session, remote_index, message);
}

// @retail 0x946c0
void c_class_938e0::handle_boot_machine(const s_network_message_peer_identity *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_boot_machine(session, remote_index, message);
}

// @retail 0x94700
void c_class_938e0::function_94700(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && session->function_058d20())
		network_session_handle_player_add(session, remote_index, message);
}

// @retail 0x94740
void c_class_938e0::handle_player_refuse(const s_network_message_player_refuse *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && network_session_channel_is_host(session, remote_index))
		network_session_handle_player_refuse(session, message, remote_index);
}

// @retail 0x94780
void c_class_938e0::handle_player_remove(const s_network_message_player_remove *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_player_remove(session, remote_index, message);
}

// @retail 0x947c0
void c_class_938e0::handle_player_properties(const s_network_message_player_properties *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_player_properties(session, remote_index, message);
}

// @retail 0x94800
void c_class_938e0::function_94800(const s_session_id *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session && network_session_channel_is_host(session, remote_index))
		function_05d9e0(session, message);
}

// @retail 0x94830
void c_class_938e0::handle_parameters_request(const s_network_message_parameters_request *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_parameters_request(message, session, remote_index);
}

// @retail 0x94870
void c_class_938e0::handle_countdown_timer(const s_network_message_countdown_timer *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && session->function_058d20())
		network_session_handle_countdown_timer(message, session, remote_index);
}

// @retail 0x948c0
void c_class_938e0::handle_mode_acknowledge(const s_network_message_mode_acknowledge *message, long remote_index)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, (const s_session_id *)message);
	if (session && function_058d70(session) && session->function_058d20())
		network_session_handle_mode_acknowledge(session, message, remote_index);
}

/* the simulation messages: each goes to the world's view of the channel it
   arrived on (src/unknown_067e10.cpp, src/unknown_085540.cpp) */
c_simulation_view *function_6adc0(c_class_6a600 *world, long value);
bool simulation_world_queue_block(c_class_6a600 *world, const s_simulation_block_data *data);


#define SIMULATION_WORLD ((c_class_6a600 *)g_4cf77c)

struct s_type_22101d
{
	long state;
	long id;
};

struct s_type_0bbd2f
{
	dword player_mask;
	dword valid_mask;
	t_player_key keys[16];
};

struct s_network_message_player_update
{
	long a;
	long b;
	bool failed;
	byte unknown09[3];
	dword controller_mask;
	s_simulation_player_state states[4];
};

struct s_network_message_join_data_begin
{
	long field_0_4;
};

struct s_network_message_join_data
{
	long offset;
	long size;
};

struct s_network_message_baseline_update
{
	long id;
	long sequence;
	byte data[1];
};

/* the view of the simulation world that faces a channel, or none */
static inline c_simulation_view *simulation_get_view_by_channel(long channel_index)
{
	c_simulation_view *view = 0;
	if (g_4cf770 && SIMULATION_WORLD->state)
		view = function_6adc0(SIMULATION_WORLD, channel_index);
	return view;
}

// @retail 0x94910
void c_class_938e0::handle_view_establishment(const s_type_22101d *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view)
		view->handle_establishment(message->state, message->id);
}

// @retail 0x94940
void c_class_938e0::handle_player_acknowledge(const s_type_0bbd2f *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view && ((1 << view->type) & 0x14))
		view->update_player_mask(message->player_mask, message->valid_mask, message->keys);
}

/* not matched: our simulation_world_queue_block (0x698e0, lane D) takes the
   world in another register than retail's esi */
// @retail 0x94990
void c_class_938e0::handle_synchronous_update(const s_simulation_block_data *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view && view->type == 1)
	{
		c_class_6a600 *world = view->world;
		if (world->flag24)
			simulation_world_queue_block(world, message);
	}
}

// @retail 0x949d0
void c_class_938e0::handle_player_update(const s_network_message_player_update *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view && view->type == 2)
		view->handle_player_update(message->failed, message->a, message->b, message->controller_mask, message->states);
}

/* not matched: retail's join_data_begin (0x85dc0, lane D) takes the update
   number on the stack, ours in a register */
// @retail 0x94a10
void c_class_938e0::handle_join_data_begin(const s_network_message_join_data_begin *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view && view->type == 1)
		view->join_data_begin(message->field_0_4);
}

// @retail 0x94a50
void c_class_938e0::handle_join_data(long channel_index, const s_network_message_join_data *message, long data_size, const byte *data)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (data_size == message->size && view && view->type == 1)
		view->join_data_receive(message->offset, data, message->size);
}

/* not matched: 0x85e70 is a stub */
// @retail 0x94aa0
void c_class_938e0::handle_baseline_update(const s_network_message_baseline_update *message, long channel_index)
{
	c_simulation_view *view = simulation_get_view_by_channel(channel_index);
	if (view && view->type == 3)
		view->baseline_update(message->id, message->sequence, (const s_input_update *)message->data);
}

/* the connection messages: the identifier of the connection and, for a
   close, its reason */
struct s_type_2c88a0
{
	long identifier;
	long reason;
};

struct s_type_ac8d96
{
	long identifier;
};

#define CONNECTION(index) (&((s_link_connection *)g_4d87d4)[index])

static __forceinline void function_94ae1(s_type_2c88a0 const *arg_0, s_link_connection *arg_1)
{
	switch (arg_0->reason)
	{
	case 0:
	case 1:
	case 2:
	case 5:
	case 6:
		link_connection_close(arg_1, 5);
		break;
	case 3:
	case 4:
	case 7:
		break;
	default:
		__assume(0);
	}
}

static __forceinline void function_94ae2(s_link_connection *arg_0, long arg_1)
{
	switch (arg_1)
	{
	case 0:
	case 1:
	case 2:
	case 5:
	case 6:
		link_connection_close(arg_0, 5);
		break;
	case 3:
	case 4:
	case 7:
		break;
	default:
		__assume(0);
	}
}

// @retail 0x94ae0
void network_message_handle_connect_refuse(long connection_index, const s_type_2c88a0 *message)
{
	s_link_connection *connection = CONNECTION(connection_index);
	long state = connection->state;
	if (state > 2 && state != 5)
	{
		state = connection->local_sequence;
		if (state == message->identifier)
		{
			state = message->reason;
			function_94ae2(connection, state);
		}
	}
}

// @retail 0x94b40
void network_message_handle_connect_establish(long connection_index, const s_type_3de205 *message)
{
	s_link_connection *connection = CONNECTION(connection_index);
	long state = connection->state;
	if (state > 2 && connection->local_sequence == message->identifier)
	{
		if (state >= 4 && connection->remote_sequence != message->remote_identifier)
		{
			s_type_99af70 address;
			link_connection_get_address(connection, &address);
			link_connection_close(connection, 6);
			link_connection_connect(&address, connection, false);
		}
		link_connection_establish(connection, message->remote_identifier);
	}
}

// @retail 0x94bb0
void network_message_handle_connect_closed(long connection_index, const s_type_ac8d96 *message)
{
	s_link_connection *connection = CONNECTION(connection_index);
	if (connection->state > 2 && connection->local_sequence == message->identifier)
		link_connection_close(connection, 10);
}

/* the message types a connection's channel delivers to the handler; the
   session messages need an established connection (state 5) */
// @retail 0x93aa0
void c_class_938e0::function_93aa0(long channel_index, long message_type, long message_size, const void *message)
{
	s_link_connection *connection = CONNECTION(channel_index);
	s_type_99af70 address;
	switch (message_type)
	{
	case 11:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			function_94270((const s_session_id *)message, &address);
		return;
	case 13:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			function_094310(this, (const s_session_id *)message, &address);
		return;
	case 14:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			function_094330(this, (const s_session_id *)message, &address);
		return;
	case 15:
		if (connection->state == 5)
			handle_host_handoff((const s_network_message_handoff *)message, channel_index);
		return;
	case 16:
		if (connection->state == 5)
			handle_host_handoff_acknowledge((const byte *)message, channel_index);
		return;
	case 17:
		if (connection->state == 5)
			function_943d0((const s_session_id *)message, channel_index);
		return;
	case 18:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			function_94420((const s_session_id *)message, &address);
		return;
	case 19:
		if (connection->state == 5)
			handle_channel_closed((const s_session_id *)message, channel_index);
		return;
	case 20:
		if (connection->state == 5)
			handle_peer_ready((const s_session_id *)message, channel_index);
		return;
	case 21:
		if (connection->state == 5)
			function_944a0((const s_session_id *)message, channel_index);
		return;
	case 22:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			function_94520((const s_session_id *)message, &address);
		return;
	case 23:
		if (connection->state == 5 && link_connection_get_address(connection, &address))
			handle_peer_reestablish((const s_session_id *)message, &address);
		return;
	case 25:
		if (connection->state == 5)
			function_94610((const s_session_id *)message, channel_index);
		return;
	case 26:
		if (connection->state == 5)
			handle_peer_properties((const s_network_message_peer_properties *)message, channel_index);
		return;
	case 27:
		if (connection->state == 5)
			handle_delegate_leader((const s_network_message_peer_identity *)message, channel_index);
		return;
	case 28:
		if (connection->state == 5)
			handle_boot_machine((const s_network_message_peer_identity *)message, channel_index);
		return;
	case 29:
		if (connection->state == 5)
			function_94700((const s_session_id *)message, channel_index);
		return;
	case 30:
		if (connection->state == 5)
			handle_player_refuse((const s_network_message_player_refuse *)message, channel_index);
		return;
	case 31:
		if (connection->state == 5)
			handle_player_remove((const s_network_message_player_remove *)message, channel_index);
		return;
	case 32:
		if (connection->state == 5)
			handle_player_properties((const s_network_message_player_properties *)message, channel_index);
		return;
	case 33:
		if (connection->state == 5)
			function_94800((const s_session_id *)message, channel_index);
		return;
	case 34:
		if (connection->state == 5)
			handle_parameters_request((const s_network_message_parameters_request *)message, channel_index);
		return;
	case 35:
		if (connection->state == 5)
			handle_countdown_timer((const s_network_message_countdown_timer *)message, channel_index);
		return;
	case 36:
		if (connection->state == 5)
			handle_mode_acknowledge((const s_network_message_mode_acknowledge *)message, channel_index);
		return;
	case 37:
		if (connection->state == 5)
			handle_view_establishment((const s_type_22101d *)message, channel_index);
		return;
	case 38:
		if (connection->state == 5)
			handle_player_acknowledge((const s_type_0bbd2f *)message, channel_index);
		return;
	case 39:
		if (connection->state == 5)
			handle_synchronous_update((const s_simulation_block_data *)message, channel_index);
		return;
	case 40:
		if (connection->state == 5)
			handle_player_update((const s_network_message_player_update *)message, channel_index);
		return;
	case 41:
		if (connection->state == 5)
			handle_join_data_begin((const s_network_message_join_data_begin *)message, channel_index);
		return;
	case 43:
		if (connection->state == 5)
			handle_baseline_update((const s_network_message_baseline_update *)message, channel_index);
		return;
	case 6:
		network_message_handle_connect_establish(channel_index, (const s_type_3de205 *)message);
		return;
	case 42:
		if (connection->state == 5)
			handle_join_data(channel_index, (const s_network_message_join_data *)message, message_size - 8, (const byte *)message + 8);
		return;
	case 44:
		return;
	}
}

/* the out-of-band messages: those that arrive from an address rather than on
   an established channel */
// score 143 (was 153; m3 lane 3 r239 C1, kept by the coordinator 11-13): case 11 sits in the first switch after case 24.
// Earlier (153): native input roles agree; delayed ESI save and late classifier differ (464/444 bytes).
// MiniMax B (11-13): no new form, no lower score (153); the address and the message still keep different homes, and the caller that passes them lives in another file.
// Earlier 153 (was 153): native input roles agree; delayed ESI save and late classifier differ (464/444 bytes).
// MiniMax G (11-12): no new form, no lower score (196); retail's convention is eax = the message type,
// ecx = the message (copied to ebx), edx = the receiver and the address on the stack (copied to edi at
// the entry), with the esi save delayed past the ja. Declaring the parameters in retail's order (the
// type, the message, the address) puts the selector in eax and the jump table at retail's address
// (fast 197 against the control 196), but the message and the address still trade places, and the only
// caller (src/unknown_07b330.cpp:204) passes them in this file's order, so the change cannot be made
// here. 0x94420 marked noinline in the handler's member declaration keeps 0x94420 and 0x93aa0 matched
// and gives 211 (428 bytes): the inline of 0x94420 is not the blocker, the convention is.
// MiniMax A (11-12): no new form, no lower score (196); the length already agrees with retail's 444 and the first difference is at the entry, where retail keeps the stacked message in edi and this build keeps it in esi, and that one choice fixes the push order and the argument registers of the handler calls.
// MiniMax S (11-10): no new form, no lower score (153 in the wider set, 196 in the two-source reduced
// one); the case bodies call the same functions as retail (0x93f60, 0x93fa0, 0x940b0 and the rest are
// in this file), but the reduced build does not pair their call targets, so most of the rows are that
// pairing and not a real difference. What is left is the register that carries the address argument
// (retail keeps it in EDI and pushes ESI for the case bodies, ours keeps it in ESI) and the jump
// table's address, which follows the data layout of the build.
// MiniMax H (11-05): no new form, no lower score; forcing 0x94420 inline gives 106 with retail's
// convention and the delayed ESI save, but 0x93aa0 then inlines it too and loses its match, and
// retail's final code has no use that crosses a call to home the address in EDI.
// MiniMax A (11-04, run 41): no new form, no lower score (196 in the 16-address set, 153 in the wider one);
// the switch's cases still take their arguments in registers other than retail's (it pushes edi where ours
// pushes esi), so the callee conventions decide this one.
// MiniMax C (11-08): no new form, no lower score.
// Earlier note (153, was 153): native input roles agree; delayed ESI save and late classifier differ (464/444 bytes).
// Earlier 153 (was 153), merged from a second score line: native input roles agree; delayed ESI save and late classifier differ (464/444 bytes).
// Round 147 (m3 lane 8): no unlisted form. Retail's convention needs the address live across a call, but retail calls
// 0x94420 and 0x94270 out of line and has no such use. Its connection handlers take no address (retail passes them the
// index in EAX and the message in EDX), so an unread address parameter on them has no retail evidence and was not tried.
// Earlier: retail takes the type in EAX, the message in ECX (kept in EBX), this in EDX and
// MiniMax lane E (11-01): no new form, no lower score; retail's parameter order is (message_type,
// message, address) with the receiver in EDX, which this file's declaration cannot change.
// Earlier at 193 (was 193): retail takes the type in EAX, the message in ECX (kept in EBX), this in EDX and
// the address on the stack (loaded into EDI, which 0x94220 takes it in); ours the type in ECX, the
// address in EAX and the message on the stack, in every declared order. Retail lays the case bodies out
// in one switch's order 0,2,3,4,8,9,10,12,13,14,22,23,24,18,11,5,6,7; with the unified switch and the
// address read once through its address only type/message stay swapped (ECX/EAX for retail EAX/ECX), and
// the 24 missing bytes are retail's EDI copy of the address and its three-pop exits. --fast builds this
// function differently (control 191 against plain 193), so the "fast" scores below are previews only.
// Round 67 (all plain): unified retail-order switch 217 (420 bytes, same convention); declared
// (type, address, message) with the call site 193 (convention unchanged); free function (type, address,
// handler, message) on the two-switch body 193 (unchanged); the address read once through its address
// 195 (address stacked, type ECX, message EAX); that with (address, message, type) declared 195.
// Round 9 (lane 10, fast control 191): free function (type, address, handler, message) on the two-switch
// body 191 (type still ECX, message on the stack); case-scoped connection index 191; direct cases
// 0,2,3,4 196; residual cases in numeric order 191; first switch without its default 191.
// Round 76 (fast, control 191): unified retail-order switch 218 (aligned 252 -> 83); plus the address read
// through its address 220 (aligned 104); five parameter orders on it and the message/address/type order
// 219-220; free function in type/address/handler/message and handler-first orders 220/220; cases 2/4/8/5
// first 220/216/219/216; message-typed local first 190 (444 bytes, one lower, not kept); link pointer
// first 242 (this moves to ECX); explicit default/scoped lookups/returns 220 each; message test first
// 212 (diagnostic). Case-subset bisect: one-case bodies take the type in EAX, any full body keeps ECX. An
// unused fourth size parameter is dropped by LTCG and leaves the registers as they are, 220.
// Round 28 (lane 12): single retail-order switch 218 fast; with type/message/address declared 218; one
// read of the address through its address 220; that with type/message/address 219 (message also on the
// stack, 0x7afd0 3 -> 35), with type/address/message 220. Lane 4 r98: one numeric-order switch with
// returns, as 0x93aa0, 208 (432 bytes, same convention); an unused message size parameter after the type
// 191; formals (type, message, address) 191; direct cases 0,2,3,4 196; connection cases after 11 205.
// Earlier plain checks (full 198 then): isolated endpoint/leave/query cases 195/198/206; separate
// connection phase 258; two session cases first 199; connection order reversed 198; direct lookup guard
// 198; residual phase reversed 207; lookup/direct refusal/query/information inline and noinline 198 each;
// combined session handler declarations 198; peer-first partition 195; direct order 2,4,0,3 193 (kept).
// Single-root/one-root/complete-root family orders (query, connection, session, peer in every tried
// permutation) 202-222; unified switch 208; retail-order switch with lookup exits 218; missing-connection
// exits 196; inline depth one 196; size preference 243; direct/full if ladders 278/241; link snapshot
// 215; unsigned connection handle 196; typed session payload 196; session cases before lookup cases 210;
// reversed direct cases 195; adjacent lookup range after the direct phase 205; instance/standalone/lookup/
// query/connection/peer/acknowledgment/reconnect cases first 227/250/239/209/277/190/211/233 (fast).
// Direct-case orders (all 24) 191-196 fast; lookup orders 7,5,6 / 6,7,5 / 6,5,7 / 5,7,6 196/195/191/191;
// acknowledgment first in the residual phase 196; residual session/peer/numeric/reverse/24-first/
// 23-first orders 189/189/191/191/189/191; session-ordinal order 193 plain; second-phase lookup/session,
// connection/peer, connection/query and connection/free-case interleaves 192-195 fast.
// Round 364: shared lookup inside the connection cases 261; unified direct/numeric/observed orders
// 272/272/271; residual group last 252; failure break 261; observed failure/scoped result 271/271.
// Round 366: common direct exit/residual returns/failure breaks/case-local results 216 each; common-exit
// nested defaults last/first 191/191; complete common-exit switch retained/observed orders 208/218.
// Round 245: single classifier with internal connection orders 5,7,6 / 6,5,7 / 6,7,5 / 7,5,6 / 7,6,5
// 208/208/207/208/208; reverse connections with direct orders 0,2,4,3 / 0,3,4,2 / 0,4,2,3 208/208/207.
// Full-root internal direct orders 207-208; early-phase connection cases 5/6/7 216/224/230, pairs
// 5+6/5+7/6+7 241/249/253, 5+8 and 6+8 229/231; five coherent formal orders on the two-phase body 191
// each; unsigned selector in three orders 191 each (all fast). Inline/noinline/forceinline declarations
// on the dispatcher, the lookup, every direct, connection, leave, acknowledgment, abort, query, update and
// reconnect handler, singly, grouped and combined, all 191-196 fast (198 plain where checked).
// selector/message/address convention and dispatch root remain. Round 76: with the
// Round 9 (lane 10, fast control 191): free function in retail's register order (type, address, handler,
// message) on the two-switch body, call site updated, 191 (type still ECX, message on the stack);
// case-scoped connection index 191; direct cases 0,2,3,4 196; residual cases in numeric order 191;
// first switch without its default 191. All restored.
// unified retail-order switch and the address read once through its address, the address goes on the stack
// as in retail and only type/message are swapped (ecx/eax for retail eax/ecx); the 24 missing bytes are
// exactly retail's edi copy of the address and its three-pop exits, so that body matches once the convention does.
// incoming selector/message/address allocation and dispatch root remain.
// Single-root family orders query/connection/session/peer 210; query/session/connection/peer 219;
// query/peer/connection/session 211; connection/query/session/peer 207; connection/session/query/peer 208;
// session/query/connection/peer 220; session/connection/peer/query 216; peer/query/session/connection 222.
// All restored; reduced preview 191 versus full 193 with differing-callee caveats.
// selector/message/address convention differs; full baseline 198, fast preview 196.
// Unified switch 208; retail-order switch with lookup exits 218; missing-connection exits 196; inline depth
// one 196; size preference 243.
// selector/message/address allocation and dispatch root remain.
// No local frame; selector/message/address allocation and dispatch root differ.
// Earlier: selector/message/address convention differs; full baseline 198, fast preview 196.
// Earlier: selector/message/address allocation and dispatch root remain.
// Isolated endpoint/leave/query cases 195p/198p/206p; separate connection phase 258p;
// two session cases first 199p; connection order reversed 198p; direct lookup guard 198p;
// residual phase reversed 207p. All eight plain checked; fast preview 196 versus full 198.
// Inline declarations on query, acknowledgment, update and reconnect handlers,
// singly and paired, all 196 fast; combined session handlers 198 plain, no gain.
// Direct/full if ladders 278/241; link snapshot 215; unsigned connection handle 196;
// typed session payload 196; session cases before lookup cases 210; reversed direct
// cases 195; adjacent lookup range after direct phase 205 (fast control 196).
// selector/message/address allocation still differs.
// Earlier: selector/message/address allocation still differs.
// Lookup inline/noinline 198/198; direct refusal 198/198; direct query 198/198;
// direct information 198/198, all plain-confirmed. Fast control 195 versus full 198.
// Original bodies, signatures, declarations and every game-call tuple restored.
// selector/address/payload convention and dispatch schedule still differ.
// instance cases first 227f; standalone cases first 250f.
// lookup cases first 239f; query cases first 209f.
// connection cases first 277f; peer cases first 190f.
// acknowledgment first 211f; reconnect cases first 233f.
// Peer-first partition plain 195; three-count gain rejected.
// incoming selector/message/address allocation remains.
// Six separate noinline handler declarations and query/ack or peer/session groups
// all 195 in the reduced image, tying its unchanged 195 preview. All declarations restored.
// selector/message/address convention still differs; executable rows 155 to 150.
// Direct-case orders 3,4,0,2 / 4,3,0,2 / 2,4,0,3 / 3,2,4,0 score 196/196/191/195 fast;
// keep 2,4,0,3 at plain 193, retail length. Lookup orders 7,5,6 / 6,7,5 give 196/195;
// acknowledgment first in residual phase 196; unified observed order 218. All call inputs preserved.
// Earlier at 153: register inputs, the delayed ESI save and the late case layout still differ.
// Earlier (153, was 153): register inputs, the delayed ESI save and the late case layout still differ.
// Direct orders 0,4,2,3 / 0,4,3,2 / 2,0,4,3 / 2,3,0,4 give 193/196/193/195 fast;
// 2,3,4,0 / 3,0,2,4 / 4,0,2,3 / 4,2,0,3 give 194/193/193/191 fast.
// Last order is 193 plain, tying baseline; fast control 191. All rejected.
// Earlier: selector/message/address convention still differs; executable rows 155 to 150.
// Earlier: incoming selector/message/address allocation remains.
// Connection refusal/establishment/closure inline and noinline declarations,
// individually and as a group, all 191 fast, tying preview. All restored.
// selector/message/address convention and dispatch root remain.
// Residual session/peer/numeric/session-reverse/24-first/23-first orders 189/189/191/191/189/191 fast;
// lookup orders 6,5,7 and 5,7,6 191/191; session-ordinal order 193 plain, tying baseline.
// Leave/acknowledgment/abort inline and noinline declarations, singly and together,
// all 191 fast, tying preview; no declaration or handler-body change retained.
// Dispatcher inline/noinline/forceinline/inline-outofline declarations all 191 fast, tying reduced control 191;
// direct orders 0,2,4,3 / 0,3,2,4 / 0,3,4,2 / 2,0,3,4 give 193/193/194/196 fast.
// Round 144: complete-root priorities query/connection/peer/session 207; query/session/peer/connection 218;
// query/peer/session/connection 220; connection/query/peer/session 202; connection/session/peer/query 207;
// peer/query/connection/session 212; peer/session/query/connection and peer/session/connection/query 220.
// All restored; reduced control 191 versus plain 193; incoming selector/message/address roles remain.
// Round 364: Shared lookup inside the connection cases: retained phases 261; unified direct/numeric/
// observed orders 272/272/271; residual group last 252; failure break 261; observed failure/scoped result 271/271.
// Round 366: Common direct exit/residual returns/failure breaks/case-local results 216 each;
// common-exit nested defaults last/first 191/191 fast (ties reduced control);
// complete common-exit switch retained/observed handler orders 208/218. Original phases restored.
// One-root family orders connection/peer/query/session and connection/peer/session/query
// 204/205; session/connection/query/peer 207; session/query/peer/connection,
// session/peer/query/connection and session/peer/connection/query 220/220/220;
// peer/connection/query/session and peer/connection/session/query 206/207. All restored.
// Remaining direct orders 2,4,3,0 / 3,0,4,2 / 3,2,0,4 / 3,4,2,0 /
// 4,0,3,2 / 4,2,3,0 give 193/194/196/195/196/193 fast; genuine
// residual default scope last/first 191/191, tying the reduced control.
// Combined inline/out-of-line declarations on lookup, direct refusal/query, three connection handlers,
// and leave/acknowledgment handlers: all eight 191 fast, tying reduced control 191 (full 193).
// Every declaration and handler call restored.
// Round 167: Second-phase lookup/session interleaves 5,12,6,13,7,14 / 5,12,6,14,7,13 /
// 5,13,6,12,7,14 / 5,13,6,14,7,12 / 5,14,6,12,7,13 / 5,14,6,13,7,12 all 195;
// 12,5,13,6,14,7 and 14,5,13,6,12,7 both 194. Reduced control 191, full 193;
// original phases, case bodies, caller and handler tuples restored.
// Single-root retained/observed orders with lookup inline/noinline or all three
// connection-handler inline/noinline boundaries: 208/208/208/208/218/218/218/218 fast; original graph and declarations restored.
// Full-root internal direct orders 4,2,0,3 / 4,2,3,0 / 3,4,0,2 / 3,4,2,0 /
// 2,3,4,0 / 2,0,3,4 / 0,4,3,2 / 0,3,2,4 score 208/208/208/208/208/208/207/208
// fast, against reduced control 191 (full 193). Original phases and call tuples restored.
// Early phase connection cases 5/6/7 216/224/230; pairs 5+6/5+7/6+7
// 241/249/253; 5+8 and 6+8 229/231 (fast, control 191).
// Five coherent formal orders on the two-phase body 191 each; unsigned selector
// in address/selector/payload, selector/address/payload and payload/selector/address 191 each.
// Reduced control 191; full 193. All signatures, caller arguments and bodies restored.
// Round 245 (fast control 191): Single classifier with internal connection orders 5,7,6 / 6,5,7 /
// 6,7,5 / 7,5,6 / 7,6,5 208/208/207/208/208; reverse connections with direct orders
// 0,2,4,3 / 0,3,4,2 / 0,4,2,3 208/208/207. All regress at 432/444 bytes;
// original phases restored. Incoming selector/message/address roles and dispatch root remain.
// Round 76 (fast, control 191): unified retail-order switch 218 (aligned 252 -> 83); plus address read through
// its address 220 (aligned 104); five parameter orders on it and the message/address/type order 219-220;
// free function in type/address/handler/message and handler-first orders 220/220; cases 2/4/8/5 first
// 220/216/219/216; message-typed local first 190 (444 bytes, one lower, not kept); link pointer first 242
// (this moves to ecx); explicit default/scoped lookups/returns 220 each. Diagnostic message test first 212.
// Earlier at 193 (was 193): retail takes the selector in eax, the message in ecx (kept in ebx), this in
// edx and the address on the stack (edi), with one switch over 0..24; ours the selector in ecx, the
// address in eax and the message on the stack, and two switches.
// Case-subset bisect: one-case bodies take the type in eax, any full body keeps ecx. An unused fourth size
// parameter is dropped by LTCG (the caller stops passing it) and leaves the registers as they are, 220.
// Round 188: Residual connection/peer interleaves 5,22,6,23,7,24 / 22,5,23,6,24,7 /
// 5,23,6,24,7,22 / 24,5,23,6,22,7; connection/query interleaves 5,8,6,9,7,10 /
// 8,5,9,6,10,7 / 5,9,6,10,7,8 / 10,5,9,6,8,7:
// 193,192,193,192,195,194,195,195 fast; reduced control 191, full 193. Original executable source restored.
// Lane 4 r98 (fast control 191): retail takes the selector in eax, the message in ecx (kept in ebx) and
// this in edx with the address on the stack (edi); ours the selector in ecx, the address in eax and the
// message on the stack. One numeric-order switch with returns, as 0x93aa0, 208 (432 bytes, same
// convention); an unused message size parameter after the type (dropped by the link) 191; formals
// (type, message, address) 191, same convention; direct cases 0,2,3,4 196; connection cases after 11 205.
// Round 224: Residual connection/free-case interleaves 5,11,6,18,7,8 / 11,5,18,6,8,7 /
// 5,18,6,8,7,11 / 18,5,8,6,11,7 / 5,11,18,6,8,7 / 11,18,5,8,6,7 /
// 18,11,5,6,8,7 / 8,5,11,6,18,7 195/194/195/194/195/192/192/194 fast. Original phases and handler tuples restored.
// Round 28 (m3 lane 12): single retail-order switch 218 fast (type ecx, address eax, message stack); with
// type/message/address declared 218 (identical); one read of the address through its address 220 (address
// on the stack as retail, type ecx/message eax, retail eax/ecx); that with type/message/address 219 (message
// also on the stack, 0x7afd0 3 -> 35), with type/address/message 220. Declared order does not move type to eax.
// Round 50: unified retail-order slot reads 220/220; sender reference 218; full sender value 242
// (484 bytes, ret 0x14, caller 76); pointer reference 221. Four-byte aggregate with const,
// register, declaration-swapped or per-case-result pointer forms 242 each (484 bytes, caller MATCH).
// All restored; selector/message/address convention and repeated sender loads remain.
// Round 217: Reversed-peer/connection residual orders 24,5,23,6,22,7 / 5,24,6,23,7,22 /
// 24,23,5,22,6,7 / 5,24,23,6,22,7 / 24,5,6,23,7,22 / 24,5,23,22,6,7 /
// 5,24,6,7,23,22 / 24,23,22,5,6,7 give 192/193/190/193/192/192/192/189 fast;
// reduced control 191, full 193. Original phases and every case/call restored.
// Round 402: residual three-family priorities in connection/query/session,
// connection/peer/session and connection/query/peer orders, eight forms,
// 195/189/194/193/187/192/193/188 fast; control 191, full 193. All restored;
// selector/payload/sender convention and single dispatch root remain.
// Round 39 (m3 lane 12): the address read once through its address (on the stack as retail) with
// orders (address, message, type) 198, (message, type, address)/(message, address, type) 201;
// one retail-order switch with it 220, also with (type, address, message) 220 (fast; selector ecx,
// message eax in every form). All restored.
// Round 219: Residual singleton handlers 8/11/18 before the connection family, all six orders,
// 191 each; split singleton placements 192/191 (reduced control 191, full 193). All restored;
// incoming selector/message/sender allocation and dispatch root remain.
// Round 442: Four-family residual interleaves 191/188/195/195/190/199/187/196 fast, versus
// control 191 and full 193. Best four-point lead rejected; all exact cases, phases, caller and
// declarations restored.
// Round 255: non-connection cases before final connection classifier, first groups
// D(2,4,0,3), Q(8,9,10), S(12,13,14,18,11), P(22,23,24) in orders
// DSQP/DPQS/QSDP/QPDS/SDQP/SQDP/PDSQ/PSDQ: 298/281/300/280/302/300/299/287.
// Added byte remapping and stacked selector regress; original phases restored.
// Round 479: accepted direct root with separate residual classifiers for
// query/session/peer/singleton families 207/247/196/232; connection paired
// with query/session/peer 200/221/221; query plus session 203 (fast control
// 191, full 193); every original case operation and handler input preserved.
// Consolidated first-root/late-family forms 208/208/196/208/203/196/203/194f;
// observed first-root priority with late 18/11 is 153 plain, kept, and matches
// the unchanged caller. All other source forms restored; no interface change.
// Round 483: late singleton switch inside default, both orders 196/196;
// late case returns in both orders 196/196; default/terminal conditional chains
// 196/196; singleton-first switch/conditional chain 201/202 (fast control 196).
// Full baseline 153; original native-role graph and matched caller preserved.
// Round 238: on the consolidated native-input root, common completion for all/direct/connection
// cases, default-owned late cases, case-local lookup results on those three completion graphs,
// and common completion with failed-lookup breaks: eight 196 fast, tying control 196.
// All-case common completion is 153 plain, tying baseline; original complete body restored.
// Round 445: Accepted first classifier with connection orders 5,7,6 / 6,5,7 / 6,7,5 / 7,5,6 /
// 7,6,5 and direct priorities 2,4,0,3 / 4,2,0,3 / 0,3,4,2: 195/196/196/195/196/196/194/195.
// Original complete executable body restored.
// Round 494: late case-18/case-11 inline boundaries on the native-input graph
// inline/ordinary, ordinary/inline, inline/inline, noinline/ordinary, ordinary/noinline,
// noinline/noinline, inline/noinline, noinline/inline: 196/196/196/211/196/211/196/211 fast.
// Case-18 noinline moves the input roles and loses the matched caller; all declarations restored.
// Round 302: explicit late-case admission and unknown-type return, four group positions and both late orders
// 211/211/211/211/212/212/211/211 against fast 196; each loses the caller.
// Entry save lifetime and late-handler inlining remain; original dispatch restored.
// Round 336: primary D(0,2,3,4),Q(8,9,10),S(12,13,14),P(22,23,24),C(5,6,7)
// orders DQPSC/DSQPC/DSPQC/DPQSC/DPSQC/DQSCP/DQCSP/DCQSP:
// 198/195/193/195/193/199/209/208 fast (control 196; full 153).
// Three-point preview reductions rejected; original complete classifiers restored.
// Delayed ESI save, late case layout and full-only handler expansion remain.
// Round 267: shared connection lookup on the accepted primary root, successful/
// failure-first lookup guards and inner orders 567/576/657/765: 212 each reduced,
// against control 196 (full 153). All lose caller 0x7afd0 at three differences;
// original classifier, separate lookups, native input roles and caller match restored.
// Round 456: Native-input primary family orders DQPCS/DCSPQ/DCPSQ/DCPQS/DCSQP/
// DPCQS/DPCSQ/DSCPQ, fast 205/206/205/208/208/208/205/206. D direct, Q query, S session, P peer, C connection;
// original complete classifiers restored; delayed save and late expansion remain.
// Round 247: Native-input root with one three-use link observation, scoped/shared results,
// success/failure guards and direct/common completion: 257/257/257/257/257/257/257/257; original body
// and helpers restored.
// Round 72 (lane 16, plain unless marked): every case in one switch in retail's code order 217 at 420 bytes: `push esi`
// is then delayed past the range check as retail, but type is ECX, address EAX and message on the stack (caller 0x7afd0
// 3). On it (fast): an early unsigned range return 218, a signed range test 207 (432 bytes), a named type 218, a dword
// switch 218, one extra type test 209 (428 bytes), all the same convention; an unused address parameter on the three
// connection handlers 217 (plain, same code). The six parameter orders with the caller in the reduced image give type EAX
// there, 213/214 at 460 bytes, but the plain check does not agree, so screen this function plain. Restored.
// Round 523: Individual connection cases interleaved across intact primary families, eight
// native-input priorities 209/209/199/203/200/204/198/197 fast versus control 196 (full 153).
// Original root, terminal 18/11 cases, all lookups and call tuples restored; delayed ESI save remains.
// DH285: Native-input non-direct-first primary families QDSPC/QDPSC/SDQPC/SDPQC/
// PDQSC/PDSQC/CQSPD/CPQSD: 192/194/191/189/191/189/204/205 fast. Original complete dispatch restored;
// delayed ESI save, late case layout and full-only handler expansion remain.
// Round 287: Original family positions, query priorities 8-10-9/9-8-10/9-10-8/10-8-9/10-9-8,
// peer priorities 23-24-22/24-22-23, then reversed query/session/peer interiors: 192/196/194/192/194/196/196/194 fast.
// Original classifiers restored; delayed ESI save and full-only late-handler expansion remain.
// Round 45 (m3 lane 15; fast unless marked, control 196, full 153): the one retail-order switch 218 (420 bytes) keeps the
// address in EAX and the type in ECX because the address never crosses a call; retail homes it in EDI, on the stack.
// (type, message, address) with the caller, plain 217 (same code). Behind `if (type == 18)` the inlined 0x94420 body makes
// the address cross a call and gives retail's convention, 196 (444 bytes, 0x94420 inlined where retail calls it); 0x94270
// behind an `if` 216 (424 bytes, ECX); one `if` on an unused type 216, two 212 (460 bytes, EAX); an empty out-of-line call
// taking the address after a connection handler 218 (removed before the convention is chosen); a byte pointer local for
// the message 218 (same code). So retail's first pass saw the address live across a call that its final code does not have.
// M312: Native-input session interior priorities and remaining peer orders:
// 196/196/196/196/196/196/196/196. Original body restored.
// M314: Native-input first-family priorities QDCSP/QDCPS/QDSCP/QDPCS/SDQCP/SDPCQ/SDCQP/SDCPQ:
// 205/203/195/201/194/194/204/202 against reduced 196. Original classifiers restored;
// delayed ESI save, late-case layout and full-only handler expansion remain.
// M332: Repeated typed session/peer/late-message input boundaries, seven subsets, then
// all with query-message input: 196/196/196/196/196/196/196/196 reduced, tying control 196 (full 153). NOT kept:
// parameter aliases require a match. Complete classifiers and all handler tuples restored.
// Round 65 (m3 lane 1, plain): an unsigned `> 24` early return ahead of the two switches 121 (464 bytes; retail's
// prologue, delayed ESI save and jump table, caller matches; only the inlined case-18 body and late classifier
// differ), with the late pair as an if/else-if chain 125; NOT kept (the test is implied by the switches). The same
// return on the one retail-order switch 217 (same code as without it); that switch with the address read once
// through its address plus a signed `> 24` return 212 (428 bytes); 11/18 admitted as `break` cases with `default:
// return` and an if/else tail 216 (0x94420 then stays out of line and the convention is lost: retail's convention
// appears only when 0x94420 is inlined).
// Round 63 (m3 lane 10, all plain, base 153): retail's 0x7afd0 call sets type EAX, message ECX, this EDX and pushes the
// address; the one retail-order switch 217 (420 bytes: this moves to the stack, address EAX, type ECX, and case 10 saves
// EDI around 0x94220, which takes the address in EDI; retail homes the address in EDI for the whole body); that with
// (type, message, address) declared at the definition, both declarations and the call 217 (same code); with `break` in
// every case 217; the late pair as a switch nested in the first switch's default 153 (same code); as `if (type == 18)
// ... else if (type == 11)` 155. Our convention only matches when the address stays live across a call.
// M335: Rejection-owned connection exits on the native-input graph, subsets
// 1/2/4/3/5/6/7, then all with case-local results: 196/196/196/196/196/196/196/196 reduced.
// Original complete body restored; delayed ESI save and late expansion remain.
// Lane 14 r60 (all plain, base 153): an explicit object parameter (type, address, object, message) on the one retail-order switch
// 217 (420 bytes, same convention as the method form: type ECX, address EAX, message on the stack), on the kept two switches 153
// (same code); a method with an unused size parameter, one switch 217 (same code). One switch with `inline` on 0x94420, on 0x94270,
// on both 217 (not inlined, same code); with __forceinline on 0x94420 106 (452 bytes): retail's exact convention, prologue,
// delayed ESI save and every case but 18, whose body is inlined where retail calls it, and caller 0x7afd0 matches, but 0x93aa0
// inlines it too (349, match lost; a forwarding helper for this one call is not allowed). The address read once through its
// own address on one switch, with an explicit object parameter in the orders (type, address, object, message), (address, type,
// object, message), method 219 (424 bytes), (type, object, message, address), (object, type, message, address) 221; type
// still ECX and message EAX because the address sits in EAX at the dispatch, and 0x7afd0 stops matching. So the address must
// be homed in EDI by a use that crosses a call; no such use is left in retail's final code.
// Y561: Current native-input direct priorities 0324/0423/0432/2034/2043/
// 2304/2340/2430: 196/194/192/196/196/196/196/196 fast (control 196,
// full 153). NOT kept: four-point best; original classifiers and caller restored.
// B400: Explicit lookup-success results in connection cases 5/6/7, seven subsets, then
// all with case-local handles: 196 eight times fast (control 196, full 153). NOT kept:
// no exact match or gain; original classifiers, native input roles and all call tuples restored.
// M355: Typed member-call late boundaries on the native/complete classifiers, 18/11/both,
// 211/196/211/223/223/223; shared late/all-compatible selection 206/213.
// NOT kept: unmatched callable ownership; original dispatcher and all call tuples restored.
// M357: Coherent five native-input formal orders and three unsigned-selector orders:
// 196/196/197/197/197/196/197/196. NOT kept: no exact producer/contract form; original complete body restored.
// M359: Common no-op message completions for individual/residual/all hole families,
// return and break: 196/196/196/196/196/196/196/196 reduced (control 196).
// NOT kept: no exact classifier/save improvement; original complete body restored.
// DH329: Primary priorities DQCPS/DSQCP/DSPCQ/DSCQP/DPQCS/DPSCQ/DCQPS/QSDPC
// 207/198/198/208/202/198/206/197 reduced, control 196 (full 153). Original classifiers restored;
// NOT kept: all regress; delayed ESI save and late-handler expansion remain.
// M374: Partial late-handler ownership, only 18/11 at entry, early-return/else
// remainder and terminal/default remaining handler: 195/195/195/195/189/189/189/189 fast (control 196, full 153).
// NOT kept: seven-point best is below the gate and entry/save shape still differs;
// original classifiers, all handler tuples and matched caller restored.
// M377: Individual connection cases 5/6/7 inserted after direct 0/2/3 on
// native-input primary root (last 7-after-3 excluded): 199/197/198/199/197/198/199/199 fast, control 196.
// NOT kept: all regress and grow to 448/444; original dispatch/caller restored.
// M379: Intact native-input family priorities QSPDC/QSPCD/QPSDC/QPSCD/SQPCD/SQPDC/SPQDC/SPQCD
// 195/202/195/202/202/195/195/201 reduced (control 196, full 153). NOT kept: no qualifying gain;
// original classifiers, all handler tuples and body restored; delayed ESI/late expansion remain.
// M383: Native-input peer-first primary priorities PDQCS/PDSCQ/PDCQS/PDCSQ/PQDSC/
// PQDCS/PQSDC/PQSCD: 198/194/204/201/197/204/195/202 (reduced control 196; full 153).
// NOT kept: at most two points; original classifiers, all handler tuples and body restored.
// m3 lane 1 r71 (all plain, on the one retail-order switch, base 217/420): CAUSE of the convention: the three direct address arguments to
// 0x92ef0 (EAX) outweigh the one EDI argument to 0x94220, so address takes EAX and type ECX. Diagnostics, wrong values, never kept: the three
// lookups given message instead 3/444 (0x7afd0 MATCH; only those three `mov eax, edi` differ), two of them 213/424, one 213/424; five EDI calls
// against three lookups 185/380 (retail's convention), three against three 213/404; two or six other address uses removed 217/420. So a use
// crossing a call is one way to lose EAX, not the only one. Correct-value forms, no gain: lookup through a forced-inline helper taking the link
// or the handler 218/468 (retail's convention, because 0x92ef0 then inlines; this ECX, message EDX->ESI), one such helper per connection case
// 247/508, with 0x92ef0's `inline` removed 217/420 (copies propagated; 0x92d10 97); `&address[0]`, the first field's address, a read through its own
// address, a function-scope copy or `long` copy, a forced-inline function returning it, an inline link getter, `address ? address : NULL` 217/420
// each (all folded); a pointer-to-pointer local read at the lookups 219/424; cases in numeric source order 210/432. Needed: a correct form of the
// lookup argument that is not a direct EAX use at allocation.
// Round 150 (m3 lane 5, plain, on the one retail-order switch, control 217/420, 0x7afd0 3): lookup argument through a const ref bound to a cast temporary in each connection case
// 219/424 (address still EAX); connection cases as plain file-local handlers or `inline` ones 270/372 each (stay out of line, 0x7afd0 6); a forced-inline lookup helper taking the
// address by const ref 218/468 (0x92ef0 inlines, 0x7afd0 5).
// Round 74 (m3 lane 10, all plain; caller 0x7afd0 and 0x93aa0, 0x92ef0, 0x92d10 checked): on the one retail-order switch (control 217, 420 bytes, 0x7afd0 3): 0x92ef0 declared in the order its callers set its registers up, (kind, address, link), with definition,
// declarations and every call 236 (404, 0x7afd0 6); (address, kind, link) 236 (404, 0x7afd0 6); `default: return;` as the switch's first label 217; case 10 first 217; case 10 last 220. A harness without the ballast object: `__assume` on the address, the message,
// the type or `this` at entry or around the lookups, and nesting the lookups under extra tests, left the address's EAX home alone; not representative, so listed only as a lead that block depth alone doesn't look like the cause.
// MiniMax D (11-06): no new form, no lower score: the message parameter is in esi here and in edi in
// retail, and the switch saves the other register before the jump table; the connection index in each
// case block (196) and the two switches merged into one (213) leave the same choice.
// P382: Complete complementary lookup-failure outcomes in bool/char/signed-char/byte/
// short/word/long/dword: 196 each reduced (control 196; full 153). NOT kept:
// no exact delayed-save or late-handler repair; original classifiers and all call tuples restored.
// P390: Query-first primary priorities QSDCP/QSCDP/QSCPD/QPDSC/QPDCS/QPCDS/QPCSD/QCDSP:
// 200/208/206/197/204/208/206/204 reduced, control 196, full 153. Original case interiors and late
// classifier restored; early ESI/late expansion remain. NOT kept: no qualifying exact/native-shape
// gain; original complete body restored.
// Round 139 (m3 lane 7, all plain with 0x7afd0, 0x93aa0, 0x92ef0, 0x92d10, 0x94420, 0x94270; control: the one retail-order switch 217/420,
// 0x7afd0 3): on that switch, the address parameter declared `long` (cast at each pointer use, the call in 0x7afd0 passing `(long)address`)
// 217 (same code); declared as a `const s_type_99af70 &` (cases passing `&address`, the call passing `*address`) 217 (same code); the lookup
// declared `inline` in this file as its definition is 217 (same code); the function defined ahead of 0x93aa0 instead of last in the file 217
// (same code). The kept two switches defined ahead of 0x93aa0 153 (same code). The address keeps EAX in each one-switch form.
// P400: Connection-first primary priorities CDQSP/CDQPS/CDSQP/CDSPQ/CDPQS/CDPSQ/CQDSP/CQDPS:
// 207/205/207/205/207/204/202/200 reduced, control 196, full 153. NOT kept:
// no exact delayed-save/late-handler repair; original classifiers, all case interiors
// and call tuples restored. Every trial is 456 bytes against native 444.
// Round 85 (m3 lane 10, all plain, on the one retail-order switch, control 217/420, 0x7afd0 3, 0x93aa0 matched): case 4's receiver field in a named local
// ahead of the call 217 (same code: retail loads that field before its pushes, ours after them because the address holds EAX); the connection index
// initialised to NONE at function scope 217 (the dead store goes before allocation); cases 18 and 11 under one label with an inner type test 217/420 (the
// test is threaded away before allocation and adds no weight to the type); a `message_type < 0` early return 212 (428 bytes, same convention); the link
// read once into a function-scope local for the three lookups 239 (400 bytes, 0x7afd0 6: the link takes ESI at entry). Reading: the type must outrank the
// address for EAX at allocation; two extra type tests do it (round 45) but leave their code behind, and every test that folds, folds before allocation.
// M3 lane 1 r88 (one retail-order switch re-measured 217 plain / 218 fast at 420 bytes, 0x7afd0 3; the rest fast with
// 0x7afd0, 0x93aa0, 0x92ef0, 0x92d10, 0x94420, 0x94270): on the one switch, cases 18 and 11 first 218; the address parameter
// copied into a local at entry (body unchanged) 218; an explicit empty `case 1` 218; an unused trailing address parameter on
// the case 5 and 7 handlers (0x94ae0, 0x94bb0, both stay matched: the link drops it) 218; the connection index declared
// in each of the case 5, 6 and 7 blocks 218 (same code). Diagnostic (not a trial: it inlines a function retail calls and
// breaks 0x93aa0): __forceinline on
// 0x94270 251 (500 bytes) with retail's convention (type EAX, message ECX kept in EBX, address on the stack), as round 60's
// forced 0x94420 106: inlining either session case, whose body uses the address after a call, is what makes the address a
// stack argument kept in a callee-saved register; retail calls both, so something else in retail keeps the address live.
// Round 228 (m3 lane 3, all plain with 0x7afd0, 0x93aa0, 0x92ef0, 0x92d10, 0x94420, 0x94270; control 153 at 464
// bytes): every first-switch case ending in `break` 153; each connection case returning early when the lookup fails
// 153; the late switch's cases returning, with `default: return;` 153; the connection index declared inside the first
// switch's body 153 (same code each); the three connection cases moved into the late switch ahead of 18 and 11 225
// (470 bytes, 0x7afd0 3: the convention moves). All restored.
// Round 91 (m3 lane 1, all plain with 0x7afd0, 0x93aa0, 0x94420, 0x94270; control 153 at 464 bytes): the late switch as
// `if (type == 18) ... else if (type == 11)` 155; the late switch with case 11 ahead of 18 153 (same code); the late switch
// nested in the first switch's `default` 153 (same code); `case 11: case 18: break; default: return;` in the first switch
// with an if/else dispatch after it 216 (420 bytes, 0x7afd0 3: the compiler threads both labels into the jump table, so this
// is the unified switch and its convention); `case 11: case 18:` added over the first switch's `default: break;` 153 (same
// code). Body restored.
// Round 148 (m3 lane 7, fast; this function builds differently in --fast, control 196 against plain 153): re-measured the
// one retail-order switch 218 (420 bytes; type ECX, address EAX, message on the stack) and the late pair nested in the first
// switch's default 196 (same code as the control). New: 18 and 11 admitted as `break` cases of the first switch with
// `default: return;` and the late switch kept as the tail 211 fast, 202 plain (432 bytes, the same convention loss as the one
// switch; caller 0x7afd0 3); the late pair as two separate ifs 155 plain (the listed if/else-if chain's count). Ours
// inlines 0x94420 at case 18 because the late switch compiles to a compare chain (one test deep); retail calls it. No
// unlisted natural form gives the address a use across a call, which our build needs for retail's convention.
// Round 188 (m3 lane 4, all plain with 0x7afd0/0x93aa0/0x92ef0/0x92d10/0x94420/0x94270): retail's one jump table covers
// 0-24 with `ja` straight to a two-pop exit and the ESI save after it; ours inlines 0x94420 in the late compare chain.
// The late switch with 11 ahead of 18 153 (same code); the late pair as a switch ahead of the main one 224 (480 B); as two
// ifs with returns ahead of it 222; cases 5, 6, 7 moved after 18 and 11 in the late switch 222 (470 B, 0x7afd0 3); no
// `default: break;` 153 (same code); the late switch inside `if (message_type == 18 || message_type == 11)` 166 (472 B);
// the late pair as an if/else-if chain with 11 first 155. Plain re-measures of listed orders: cases 5, 6, 7 in numeric
// place 220 (476 B), case 0 last 199 (468 B).
// m3 lane 3 r233 (all plain with the file's functions, 0x7afd0, 0x93aa0, 0x92ef0 and the case callees; control 153 at
// 464 bytes, aligned 55, register-masked 34; --fast reads 196 at 444 bytes here with 0x94220 and 0x942d0 differing, so it is
// no judge). A second switch holding 18, 11, 5, 6, 7 in retail's layout order behind the first switch's other cases 222 (470 B,
// 0x7afd0 3); explicit `return` cases for the in-band types 1, 15, 16, 17, 19, 20, 21 in the first switch 146 (464 B, masked
// 34 -> 38: only the jump table's data moves, so positional); the same cases on the one retail-order switch 217 (420 B,
// 0x7afd0 3); a free function declared (type, message, handler, address), retail's EAX/ECX/EDX/stack order, on the one
// switch with 0x7afd0's call updated 217 (same convention: type ECX, address EAX kept there, message loaded into EBX from the
// stack); the late switch with 11 ahead of 18 153 (same code). On the one switch the address stays in EAX because no use
// crosses a call and the three lookups want it there; retail homes it in EDI and the message in EBX. Body unchanged.
// m3 lane 1 r98 (control 153 plain, 196 fast): no new form run. The listing agrees with the notes above: this two-switch body
// already has retail's convention (type EAX, message ECX kept in EBX, receiver EDX, address on the stack kept in EDI) because
// the late compare chain inlines 0x94420; what is left is the early ESI save (the late chain's inlined body needs ESI ahead of
// the range test) and that chain itself. The candidates checked against this note are all listed: the shared lookup for the
// three connection cases (round 364), an empty call taking the address (round 45), parameter orders with the call in 0x7afd0
// updated (rounds 63 and 233), and the unified switch's case orders. The open lead of m3 lane 1 r71 (a correct form of the
// lookup argument that is not a direct EAX use at allocation) has its whole family listed: r71 (address spellings, copies,
// getters, forced-inline helpers), round 150 (const reference to a cast temporary, file-local handlers), round 139 (address
// declared long or as a reference), m3 lane 1 r88 (address copied at entry), round 85 (link read once), round 74 (0x92ef0's
// parameter orders); no unlisted member was found, so no trial was run (0 trials: no unlisted natural form).
// m3 lane 3 r239 (all plain with 0x7afd0, 0x93aa0, 0x94420, 0x94270, 0x92ef0, 0x92d10, all MATCH unless noted; control 153).
// Kept (coordinator 11-13; the lane held it as a score-picked case order): case 11 moved from the late switch into the
// first, after case 24. That follows retail, whose jump table dispatches 11 itself
// and its block follows 24 there), the late switch keeping only 18 (as a plain `if (message_type == 18)`, the same code),
// 143 at 460 B (aligned 89 -> 77, register-masked 53 -> 35; build/r239/lead-0x938e0-143-if.patch). The same with case 11 in
// numeric place (after 10) 157 (masked 35: only the block layout moves): the case's position is retail's block order,
// which the source order gives (no code runs between the two switches, so behaviour is unchanged). Also: 18 in the first switch with 11 alone late 213 (424 B, 0x7afd0 3: 0x94420 stops inlining and
// the convention moves); on the lead, 18 tested ahead of the switch with its own return 216, `case 18: break; default: return;`
// with the call after the switch 217 (420 B, 0x7afd0 3), `case 18:` labelled onto `default: break;` 143 (same code as the lead).
// P451 (full 153, reduced control 196): Individual late cases in the primary root 216/196;
// individual default-owned late cases 196/195; separate connection interval 197;
// unsigned connection result / void call-return expressions / connection result name 196/196/196.
// NOT kept: one-point best; native delayed ESI and outlined late-handler shape remain.
// M3 lane 12 r121 (one-file LTCG harness with the caller's and callees' objects: it builds the plain 460 bytes, harness
// 126 for the plain 143, where --fast builds 444 bytes and 196 here): both switches over `(dword)message_type`, the late 18
// path as an early return, the connection index declared in case 5's block, the late 18 path as the first switch's default
// arm: same code each; case 11 last in the first switch 130 harness (fast 195).
// @retail 0x938e0
void c_class_938e0::function_938e0(const s_type_99af70 *address, long message_type, const void *message)
{
	long connection_index;
	switch (message_type)
	{
	case 0:
		network_message_handler_refuse_connect((const s_connect_request *)message, this, (long)address);
		return;
	case 2:
		function_093fa0(this, message);
		return;
	case 3:
		network_message_handler_handle_session_query((const s_network_message_session_query *)message, address);
		return;
	case 4:
		function_0785d0(unknown10, address, message);
		return;
	case 8:
		function_94100((const s_network_message_session_query *)message, address);
		return;
	case 9:
		network_message_handler_handle_leave_request(this, (long)address, (const s_session_id *)message);
		return;
	case 10:
		function_094220(this, (const s_session_id *)message, address);
		return;
	case 12:
		function_0942d0(this, (const s_session_id *)message, address);
		return;
	case 13:
		function_094310(this, (const s_session_id *)message, address);
		return;
	case 14:
		function_094330(this, (const s_session_id *)message, address);
		return;
	case 22:
		function_94520((const s_session_id *)message, address);
		return;
	case 23:
		handle_peer_reestablish((const s_session_id *)message, address);
		return;
	case 24:
		function_945f0((const s_session_id *)message, address);
		return;
	case 11:
		function_94270((const s_session_id *)message, address);
		return;
	case 5:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_refuse(connection_index, (const s_type_2c88a0 *)message);
		return;
	case 6:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_establish(connection_index, (const s_type_3de205 *)message);
		return;
	case 7:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_closed(connection_index, (const s_type_ac8d96 *)message);
		return;
	default: break;
	}
	switch (message_type)
	{
	case 18:
		function_94420((const s_session_id *)message, address);
		break;
	}
}
