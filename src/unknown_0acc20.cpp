// @flags /O2 /Gr
/* UNKNOWN_0ACC20.CPP: the session protocol message codecs
   (join, leave, handoff, session control, host decline, election, time
   synchronize) and the registration of their message types */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "globals.h"
#include "network_message_types.h"
#include <string.h>
#include <xtl.h>


struct s_message_join_request
{
	word id;
	byte unknown02[2];
	byte nonce[8];
	long player_count;
	byte players[16][12];
	long player_indices[16];
	long player_values[16];
	byte nonce3[8];
	bool has_secure_address;
	byte unknown159[3];
	dword secure_address;
	long join_type;
	dword address0;
	dword address1;
	dword address2;
	dword address3;
	long data0;
	long data1;
	long data2;
	byte xnaddr[12];
	byte nonce2[8];
	byte session_data[36];
};

struct s_message_join_abort
{
	byte nonce[8];
	byte nonce2[8];
};

struct s_message_join_refuse
{
	byte nonce[8];
	long reason;
};

/* every session message starts with the 64 bit session id */
struct s_session_id_message
{
	byte session_id[8];
};

struct s_handoff_message
{
	byte session_id[8];
	byte address[0x24];
	short peer_count;
};

struct s_host_decline_message
{
	byte session_id[8];
	bool has_reason;
	bool has_address_flag;
	bool has_address;
	byte unknownb;
	byte address[0x24];
};

struct s_election_message
{
	byte session_id[8];
	byte address0[0x24];
	byte address1[0x24];
	dword unknown50;
	dword unknown54;
	byte address2[4];
	long peer_count;
	byte peers[0x10][6];
	dword mask0;
	dword mask1;
};

struct s_election_refuse_message
{
	byte session_id[8];
	long reason;
	bool has_address;
	byte address[0x24];
};

struct s_time_synchronize_message
{
	byte session_id[8];
	dword unknown08;
	dword unknown0c;
	dword unknown10;
	dword unknown14;
	word type;
};

/* the messages that carry only the session id each had their own codecs,
   which the linker folded into 0xad5a0, 0xad2e0 and 0xad3f0 */
#define SESSION_ID_ENCODE(name) \
	void __stdcall name##_encode(s_bitstream *stream, long unknown, s_session_id_message const *message) \
	{ \
		function_1955d0(stream, message, 0x40); \
	}

#define LEAVE_DECODE(name) \
	bool __stdcall name##_decode(s_bitstream *stream, long unknown, s_session_id_message *message) \
	{ \
		function_195820(stream, message->session_id, 0x40); \
		bool result = !stream_overflowed(stream); \
		return result; \
	}

#define SESSION_CONTROL_DECODE(name) \
	bool __stdcall name##_decode(s_bitstream *stream, long unknown, s_session_id_message *message) \
	{ \
		function_195820(stream, message->session_id, 0x40); \
		if (!stream_overflowed(stream)) \
			return true; \
		return false; \
	}

static inline bool mask_fits(dword mask, dword count)
{
	return count >= 32 || (mask >> count) == 0;
}

// @retail 0x000acc20
void __stdcall message_join_request_encode(s_bitstream *stream, long size, s_message_join_request *message)
{
	stream_write_checked(stream, message->id, 16);
	function_1955d0(stream, &message->nonce, 64);
	function_1955d0(stream, &message->nonce2, 64);
	function_1955d0(stream, &message->nonce3, 64);
	function_1955d0(stream, &message->session_data, 288);
	stream_write_checked(stream, message->player_count, 5);
	for (long i = 0; i < message->player_count; i++)
	{
		function_1955d0(stream, &message->players[i], 96);
		stream_write_checked(stream, message->player_indices[i] + 1, 8);
		stream_write_checked(stream, message->player_values[i] + 1, 31);
	}
	stream_write_bit(stream, message->has_secure_address);
	if (message->has_secure_address)
		function_1955d0(stream, &message->secure_address, 32);
	stream_write_checked(stream, message->join_type, 2);
	if (message->join_type == 2)
	{
		stream_write_checked(stream, message->data0, 7);
		stream_write_checked(stream, message->data1, 7);
		stream_write_checked(stream, message->data2, 7);
		function_1955d0(stream, &message->address0, 32);
		function_1955d0(stream, &message->address1, 32);
		function_1955d0(stream, &message->address3, 32);
		function_1955d0(stream, &message->address2, 32);
		bool has_xnaddr = memcmp(&message->xnaddr, g_440070, sizeof(message->xnaddr)) != 0;
		stream_write_bit(stream, has_xnaddr);
		if (has_xnaddr)
			function_1955d0(stream, &message->xnaddr, 96);
	}
}

// @retail 0x000acfa0
bool __stdcall message_join_request_decode(s_bitstream *stream, long size, s_message_join_request *message)
{
	message->id = (word)function_1959c0(stream, 16);
	function_195820(stream, &message->nonce, 64);
	function_195820(stream, &message->nonce2, 64);
	function_195820(stream, &message->nonce3, 64);
	function_195820(stream, &message->session_data, 288);
	message->player_count = function_1959c0(stream, 5);
	for (long i = 0; i < message->player_count; i++)
	{
		function_195820(stream, &message->players[i], 96);
		message->player_indices[i] = function_1959c0(stream, 8) - 1;
		message->player_values[i] = function_1959c0(stream, 31) - 1;
	}
	message->has_secure_address = stream_read_bit(stream);
	if (message->has_secure_address)
		function_195820(stream, &message->secure_address, 32);
	message->join_type = function_1959c0(stream, 2);
	if (message->join_type == 2)
	{
		message->data0 = function_1959c0(stream, 7);
		message->data1 = function_1959c0(stream, 7);
		message->data2 = function_1959c0(stream, 7);
		function_195820(stream, &message->address0, 32);
		function_195820(stream, &message->address1, 32);
		function_195820(stream, &message->address3, 32);
		function_195820(stream, &message->address2, 32);
		if (stream_read_bit(stream))
			function_195820(stream, &message->xnaddr, 96);
		else
			memset(&message->xnaddr, 0, sizeof(message->xnaddr));
	}
	if (stream_overflowed(stream) || message->player_count < 0)
		return false;
	return true;
}

// @retail 0x000ad1c0
void __stdcall message_join_abort_encode(s_bitstream *stream, long size, s_message_join_abort *message)
{
	function_1955d0(stream, &message->nonce, 64);
	function_1955d0(stream, &message->nonce2, 64);
}

// @retail 0x000ad1e0
bool __stdcall message_join_abort_decode(s_bitstream *stream, long size, s_message_join_abort *message)
{
	function_195820(stream, &message->nonce, 64);
	function_195820(stream, &message->nonce2, 64);
	if (!stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0x000ad230
void __stdcall message_join_refuse_encode(s_bitstream *stream, long size, s_message_join_refuse *message)
{
	function_1955d0(stream, &message->nonce, 64);
	stream_write_checked(stream, message->reason, 4);
}

// @retail 0x000ad290
bool __stdcall message_join_refuse_decode(s_bitstream *stream, long size, s_message_join_refuse *message)
{
	function_195820(stream, &message->nonce, 64);
	message->reason = function_1959c0(stream, 4);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0xad2e0
bool __stdcall leave_session_decode(s_bitstream *stream, long unknown, s_session_id_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	bool result = !stream_overflowed(stream);
	return result;
}

// @retail 0xad320
void __stdcall handoff_encode(s_bitstream *stream, long unknown, s_handoff_message const *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->peer_count, 4);
	function_1955d0(stream, message->address, 0x120);
}

// @retail 0xad390
bool __stdcall handoff_decode(s_bitstream *stream, long unknown, s_handoff_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	message->peer_count = (short)function_1959c0(stream, 4);
	function_195820(stream, message->address, 0x120);
	if (!stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0xad3f0
bool __stdcall session_disband_decode(s_bitstream *stream, long unknown, s_session_id_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	if (!stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0xad430
void __stdcall host_decline_encode(s_bitstream *stream, long unknown, s_host_decline_message const *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_bit(stream, message->has_reason);
	if (message->has_reason)
	{
		stream_write_bit(stream, message->has_address_flag);
		stream_write_bit(stream, message->has_address);
		if (message->has_address)
			function_1955d0(stream, message->address, 0x120);
	}
}

// @retail 0xad530
bool __stdcall host_decline_decode(s_bitstream *stream, long unknown, s_host_decline_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	message->has_reason = function_1957d0(stream);
	if (message->has_reason)
	{
		message->has_address_flag = function_1957d0(stream);
		message->has_address = function_1957d0(stream);
		if (message->has_address)
			function_195820(stream, message->address, 0x120);
	}
	if (!stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0xad5a0
void __stdcall leave_session_encode(s_bitstream *stream, long unknown, s_session_id_message const *message)
{
	function_1955d0(stream, message, 0x40);
}

// @retail 0xad5c0
void __stdcall election_encode(s_bitstream *stream, long unknown, s_election_message const *message)
{
	function_1955d0(stream, message, 0x40);
	function_1955d0(stream, message->address0, 0x120);
	function_1955d0(stream, message->address1, 0x120);
	function_195720(stream, message->unknown50, 0x20);
	stream_write_checked(stream, message->unknown54, 4);
	function_1955d0(stream, message->address2, 0x20);
	stream_write_checked(stream, message->peer_count, 5);
	for (long index = 0; index < message->peer_count; index++)
		function_1955d0(stream, message->peers[index], 0x30);
	stream_write_checked(stream, message->mask0, 16);
	stream_write_checked(stream, message->mask1, 16);
}

// @retail 0xad710
bool __stdcall election_decode(s_bitstream *stream, long unknown, s_election_message *message)
{
	bool valid = true;
	function_195820(stream, message->session_id, 0x40);
	function_195820(stream, message->address0, 0x120);
	function_195820(stream, message->address1, 0x120);
	message->unknown50 = function_1959c0(stream, 0x20);
	message->unknown54 = function_1959c0(stream, 4);
	function_195820(stream, message->address2, 0x20);
	message->peer_count = function_1959c0(stream, 5);
	if (message->peer_count >= 0 && (dword)message->peer_count <= 0x10)
	{
		for (long index = 0; index < message->peer_count; index++)
			function_195820(stream, message->peers[index], 0x30);
	}
	else
		valid = false;
	message->mask0 = function_1959c0(stream, 0x10);
	message->mask1 = function_1959c0(stream, 0x10);
	if (valid && !stream_overflowed(stream))
	{
		if (mask_fits(message->mask0, message->peer_count) && mask_fits(message->mask1, message->peer_count))
			return true;
	}
	return false;
}

// @retail 0xad820
void __stdcall election_refuse_encode(s_bitstream *stream, long unknown, s_election_refuse_message const *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->reason, 4);
	stream_write_bit(stream, message->has_address);
	if (message->has_address)
		function_1955d0(stream, message->address, 0x120);
}

// @retail 0xad8d0
bool __stdcall election_refuse_decode(s_bitstream *stream, long unknown, s_election_refuse_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	message->reason = function_1959c0(stream, 4);
	message->has_address = function_1957d0(stream);
	if (message->has_address)
		function_195820(stream, message->address, 0x120);
	if (!stream_overflowed(stream) && message->reason > 0 && message->reason < 11)
		return true;
	return false;
}

// @retail 0xad940
void __stdcall time_synchronize_encode(s_bitstream *stream, long unknown, s_time_synchronize_message const *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->type, 1);
	switch (message->type)
	{
	case 0:
	{
		dword time = GetTickCount();
		function_195720(stream, time, 0x20);
		return;
	}
	case 1:
	{
		dword time = network_session_time_since_start((const s_session_id *)message);
		function_195720(stream, message->unknown08, 0x20);
		function_195720(stream, message->unknown10, 0x20);
		function_195720(stream, time, 0x20);
		return;
	}
	default:
		__assume(0);
	}
}

// @retail 0xad9e0
bool __stdcall time_synchronize_decode(s_bitstream *stream, long unknown, s_time_synchronize_message *message)
{
	function_195820(stream, message->session_id, 0x40);
	message->type = (word)function_1959c0(stream, 1);
	if (!stream_overflowed(stream) && message->type < 2)
	{
		volatile bool result = true;
		switch (message->type)
		{
		case 0:
			message->unknown08 = function_1959c0(stream, 0x20);
			message->unknown0c = NONE;
			message->unknown10 = network_session_time_since_start((const s_session_id *)message);
			message->unknown14 = NONE;
			break;
		case 1:
			message->unknown08 = function_1959c0(stream, 0x20);
			message->unknown0c = GetTickCount();
			message->unknown10 = function_1959c0(stream, 0x20);
			message->unknown14 = function_1959c0(stream, 0x20);
			break;
		default:
			__assume(0);
		}
		return result;
	}
	return false;
}

// @retail 0xada90
bool __stdcall time_synchronize_clear(s_bitstream *stream, long unknown, s_time_synchronize_message *message)
{
	memset(&message->unknown08, 0xff, 8);
	memset(&message->unknown10, 0xff, 8);
	return true;
}

SESSION_ID_ENCODE(leave_acknowledge)
SESSION_ID_ENCODE(session_disband)
SESSION_ID_ENCODE(session_boot)
SESSION_ID_ENCODE(host_transition)
SESSION_ID_ENCODE(host_reestablish)
SESSION_ID_ENCODE(peer_reestablish)
SESSION_ID_ENCODE(peer_establish)

LEAVE_DECODE(leave_acknowledge)

SESSION_CONTROL_DECODE(session_boot)
SESSION_CONTROL_DECODE(host_transition)
SESSION_CONTROL_DECODE(host_reestablish)
SESSION_CONTROL_DECODE(peer_reestablish)
SESSION_CONTROL_DECODE(peer_establish)

// @retail 0xadab0
void function_adab0(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_join_request, "join-request", 0x1b8, message_join_request_encode, message_join_request_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_join_abort, "join-abort", 0x10, message_join_abort_encode, message_join_abort_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_join_refuse, "join-refuse", 0xc, message_join_refuse_encode, message_join_refuse_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_leave_session, "leave-session", 8, leave_session_encode, leave_session_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_leave_acknowledge, "leave-acknowledge", 8, leave_acknowledge_encode, leave_acknowledge_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_session_disband, "session-disband", 8, session_disband_encode, session_disband_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_session_boot, "session-boot", 8, session_boot_encode, session_boot_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_host_handoff, "host-handoff", 0x2e, handoff_encode, handoff_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_peer_handoff, "peer-handoff", 0x2e, handoff_encode, handoff_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_host_transition, "host-transition", 8, host_transition_encode, host_transition_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_host_reestablish, "host-reestablish", 8, host_reestablish_encode, host_reestablish_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_host_decline, "host-decline", 0x30, host_decline_encode, host_decline_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_peer_reestablish, "peer-reestablish", 8, peer_reestablish_encode, peer_reestablish_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_peer_establish, "peer-establish", 8, peer_establish_encode, peer_establish_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_election, "election", 0xc8, election_encode, election_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_election_refuse, "election-refuse", 0x34, election_refuse_encode, election_refuse_decode);
	collection->function_x5c51c9(_network_message_type_time_synchronize, "time-synchronize", 0, 0x1c, 0x1c,
		(t_message_encode)time_synchronize_encode, (t_message_decode)time_synchronize_decode, (t_message_compare)time_synchronize_clear);
}
