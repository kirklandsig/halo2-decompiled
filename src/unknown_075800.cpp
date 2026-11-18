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
void __stdcall function_094220(c_class_938e0 *handler, const s_type_99af70 *address, const s_session_id *message);
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
void __stdcall function_094220(c_class_938e0 *handler, const s_type_99af70 *address, const s_session_id *message)
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
// One switch in retail's block order. Retail keeps the address in EDI for the whole body (loaded from the stack at
// 0x938e5) and hands it to 0x94220 there, after that call's message goes into EAX: the address is that handler's
// parameter ahead of the message, so it stays live across the EAX setup and cannot share EAX with the three lookups.
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
		function_094220(this, address, (const s_session_id *)message);
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
	case 18:
		function_94420((const s_session_id *)message, address);
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
}
