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
// score 193 (was 193): retail takes the type in EAX, the message in ECX (kept in EBX), this in EDX and
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
// @retail 0x938e0
void c_class_938e0::function_938e0(const s_type_99af70 *address, long message_type, const void *message)
{
	long connection_index;
	switch (message_type)
	{
	case 2:
		function_093fa0(this, message);
		return;
	case 4:
		function_0785d0(unknown10, address, message);
		return;
	case 0:
		network_message_handler_refuse_connect((const s_connect_request *)message, this, (long)address);
		return;
	case 3:
		network_message_handler_handle_session_query((const s_network_message_session_query *)message, address);
		return;
	default: break;
	}
	switch (message_type)
	{
	case 5:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_refuse(connection_index, (const s_type_2c88a0 *)message);
		break;
	case 6:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_establish(connection_index, (const s_type_3de205 *)message);
		break;
	case 7:
		connection_index = network_link_find_connection(field_4_5, 0, address);
		if (connection_index != NONE)
			network_message_handle_connect_closed(connection_index, (const s_type_ac8d96 *)message);
		break;
	case 8:
		function_94100((const s_network_message_session_query *)message, address);
		break;
	case 9:
		network_message_handler_handle_leave_request(this, (long)address, (const s_session_id *)message);
		break;
	case 10:
		function_094220(this, (const s_session_id *)message, address);
		break;
	case 12:
		function_0942d0(this, (const s_session_id *)message, address);
		break;
	case 13:
		function_094310(this, (const s_session_id *)message, address);
		break;
	case 14:
		function_094330(this, (const s_session_id *)message, address);
		break;
	case 22:
		function_94520((const s_session_id *)message, address);
		break;
	case 23:
		handle_peer_reestablish((const s_session_id *)message, address);
		break;
	case 24:
		function_945f0((const s_session_id *)message, address);
		break;
	case 18:
		function_94420((const s_session_id *)message, address);
		break;
	case 11:
		function_94270((const s_session_id *)message, address);
		break;

	}
}
