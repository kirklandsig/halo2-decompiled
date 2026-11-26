#include "unknown_4cf790.h"
// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_059AD0.CPP: the network session (lane D). Its members and their
   channels, the leave and disband paths, the session parameters and the
   requests a peer sends its host to change them. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "globals.h"
#include "language.h"
#include "unknown_059ad0.h"
#include "unknown_075870.h"
#include "network_message_types.h"
#include "unknown_058dd0.h"
#include "unknown_0662e0.h"

struct s_session_join_request;

/* the session listener at +0x78a8 */
class c_network_session_listener
{
public:
	virtual void unknown00(const s_type_99af70 *address, const s_session_join_request *request);
	virtual void member_left(const s_session_id *id);
	virtual void session_closed();
	virtual bool player_can_join(const void *identity);
};


/* the session summary (network_session_membership.cpp) */
struct s_session_summary;
bool session_summary_valid(const s_session_summary *summary);

/* network_session_membership.cpp and unknown_062f40.cpp */
void network_session_add_player(c_class_58d20 *session, long member_index, const XUID *xuid, long player_index, long slot);
void network_session_remove_player(c_class_58d20 *session, long player_index);
void network_session_reset_membership(c_class_58d20 *session, bool reset_limits);
void network_session_disconnect(c_class_58d20 *session, long reason);
void network_session_clear_peer(c_class_58d20 *session, long peer_index);
void network_session_remove_player_and_update(c_class_58d20 *session, long player_index);
void network_session_reset_7620(c_class_58d20 *session);
long network_session_get_open_slot_count(c_class_58d20 *session);
void network_session_enter_state_5(c_class_58d20 *session);
struct s_reservation;
struct s_reservation_session;
bool function_062f40(s_reservation_session *session, const void *identity, s_reservation **reservation_out);

/* the transport's security keys (unknown_07a9a0.cpp, unknown_07a8a0.cpp) */
struct s_xnet_registry_entry
{
	bool valid;
	bool host;
	byte unknown2[2];
	long local;
	XNKID kid;
	XNKEY key;
};



bool transport_security_create_key(long local, long index, bool online);
bool transport_security_register_key(long index, long local, bool host, const XNKID *kid, const XNKEY *key);
bool function_07ab60(const s_type_99af70 *address, long local, long *index_out, XNKID *kid_out, XNKEY *key_out, XNADDR *xnaddr_out);

void network_observer_request_channel(s_network_observer *observer, long index);

// @retail 0x62240
void function_62240(c_class_58d20 *session)
{
	if (session->state)
	{
		for (long i = 0; i < session->member_count; i++)
		{
			s_network_session_member_state *member = &session->member_states[i];
			if (member->flag1)
			{
				long state = session->state;
				if (state != 5 && state != 6 && state != 7 && state != 8)
				{
					volatile long unused = state;
					if (function_058d90(session) || session->value4c == NONE)
						continue;
					if (((state > 2 && state <= 8) || state == 1) &&
						!session->members[i].properties_valid)
						continue;
				}
				long channel = member->unknown04;
				if (session->observer->channels[channel].state != 1)
					network_observer_request_channel(session->observer, channel);
			}
		}
	}
}

/* a member's machine address within its identity */
struct s_session_machine_address
{
	byte data[6];
};

/* the parameters a peer asks the host to change (parameters-request, 0x59c bytes) */
struct s_network_message_parameters_request
{
	s_session_id session_id;
	bool change_mode;
	byte unknown09[3];
	long mode;
	bool change_language;
	byte unknown11[3];
	long language;
	bool change_value498c;
	byte unknown19[3];
	long value498c;
	bool change_value49a4;
	byte unknown21[3];
	long value49a4;
	bool change_value49c4;
	bool value49c4;
	bool change_value49c8;
	byte unknown2b;
	long value49c8;
	bool change_flag49fc;
	bool flag49fc;
	bool change_value49f8;
	byte unknown33;
	long value49f8;
	bool change_summary;
	bool summary_valid;
	byte unknown3a[2];
	byte summary[0x308];
	bool change_value4d08;
	byte unknown345[3];
	long value4d08;
	long value4d0c;
	char string4d10[0x80];
	bool change_value4dac;
	byte unknown3d1[3];
	long value4dac;
	bool change_data4db0;
	byte unknown3d9[3];
	byte data4db0[0x130];
	bool change_name;
	byte unknown50d;
	wchar_t name[32];
	bool change_value49a1;
	byte value49a1;
	bool change_value5dd0;
	byte unknown551;
	short value5dd0;
	bool change_data5ddc;
	bool data5ddc_valid;
	byte unknown556[2];
	dword data5ddc[0x11];
};

/* player-add (0xb0 bytes) */
struct s_network_message_player_add
{
	s_session_id session_id;
	long slot;
	dword identity[3];
	long unknown18;
	byte properties[0x90];
	long unknownac;
};

/* player-properties (0xa4 bytes) */
struct s_network_message_player_properties
{
	s_session_id session_id;
	long slot;
	long unknown0c;
	byte properties[0x90];
	long unknowna0;
};

/* player-remove (0xc bytes) */
struct s_network_message_player_remove
{
	s_session_id session_id;
	long slot;
};

/* peer-properties (0xd0 bytes) */
struct s_network_message_peer_properties
{
	s_session_id session_id;
	byte properties[0xc8];
};

/* delegate-leader and boot-machine (0x2c bytes) */
struct s_network_message_peer_identity
{
	s_session_id session_id;
	s_session_member_identity identity;
};

/* countdown-timer (0x20 bytes) */
struct s_network_message_countdown_timer
{
	s_session_id session_id;
	bool start;
	byte unknown09[3];
	long countdown;
	long mode;
	long time[3];
};

/* the parts of a member record the peer-properties message carries */
struct s_session_member_properties
{
	bool valid;
	byte unknown01[3];
	byte data[0xc8];
};

static inline long network_session_time_now(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

static inline s_session_member_properties *session_member_properties(c_class_58d20 *session, long member_index)
{
	return (s_session_member_properties *)((byte *)&session->members[member_index] + 0x24);
}

/* ---- members and channels ---- */

/* network_session_find_member_by_channel (0x5f670) is in network_session_channel.cpp
   (/Ob1: most callers call it); the callers below that retail inlines it into
   use this copy */
long network_session_find_member_by_channel(c_class_58d20 *session, long channel_index);

static inline long network_session_find_member_by_channel_inline(c_class_58d20 *session, long channel_index)
{
	long result = NONE;

	if (channel_index != NONE)
	{
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if (session->member_states[i].unknown00 && session->member_states[i].unknown04 == channel_index)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x5f760
long network_session_find_member(c_class_58d20 *session, const s_session_member_identity *identity)
{
	long result = NONE;

	if (session->state && session->value4c != NONE)
	{
		for (long i = 0; i < session->member_count; i++)
		{
			if (memcmp(identity, session->members[i].words, sizeof(*identity)) == 0)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x5f6f0
long network_session_find_member_by_machine(c_class_58d20 *session, const s_session_machine_address *address)
{
	long result = NONE;

	if (session->state && session->value4c != NONE)
	{
		for (long i = 0; i < session->member_count; i++)
		{
			s_session_machine_address machine = *(s_session_machine_address *)((byte *)session->members[i].words + 0xa);
			if (memcmp(&machine, address, sizeof(machine)) == 0)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x5f890
long network_session_find_player(c_class_58d20 *session, const dword *identity)
{
	long result = NONE;

	if (session->state && session->value4c != NONE)
	{
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if ((session->player_mask & (1 << i)) && memcmp(identity, &session->players[i], 12) == 0)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x5f6a0
long network_session_find_member_by_address(c_class_58d20 *session, const s_type_99af70 *address)
{
	long result = NONE;

	if (session->state && session->flag24)
	{
		XNADDR xnaddr;
		if (function_07ab60(address, session->value3c, 0, 0, 0, &xnaddr))
			result = network_session_find_member(session, (const s_session_member_identity *)&xnaddr);
	}
	return result;
}

// @retail 0x5f600
bool network_session_channel_is_host(c_class_58d20 *session, long remote_index)
{
	bool result = false;

	if (session_state_is_live(session) || session->state == 1)
	{
		if (!session->function_058d20())
		{
			long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
			long member_index = network_session_find_member_by_channel(session, channel_index);
			if (member_index != NONE && member_index == session->member_index)
				result = true;
		}
	}
	return result;
}

// @retail 0x5f900
void network_session_member_state_initialize(c_class_58d20 *session, long member_index, bool connected, long channel_index)
{
	s_network_session_member_state *state = &session->member_states[member_index];
	memset(state, 0, sizeof(*state));
	state->unknown00 = true;
	state->flag1 = connected;
	state->unknown04 = channel_index;
	state->unknown08 = NONE;
	state->unknown0c = NONE;
	state->flag3 = false;
	if (connected && session->flag765c)
		state->flag2 = true;
	state->unknown10 = network_session_time_now();
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
PRIVATE __forceinline void function_5f972(s_network_observer_channel *arg_0, long arg_1)
{
 byte local_0 = (byte)(1 << arg_1);
 byte local_1 = arg_0->owner_mask;
 local_0 = ~local_0;
 arg_0->owner_mask = local_1 & local_0;
}

// @retail 0x5f970
void network_session_member_state_dispose(c_class_58d20 *session, long member_index)
{
	s_network_session_member_state *state = &session->member_states[member_index];

	if (session_state_is_live(session) && session->function_058d20() && state->flag1 && state->flag2)
	{
		long i = 0;
		const byte *local_0 = (const byte *)session->member_states + 2;
		for (; i < MAXIMUM_PLAYERS_PER_SESSION; i++, local_0 += sizeof(s_network_session_member_state))
		{
			_ReadWriteBarrier();
			if (i != member_index && *(const bool *)(local_0 - 2) && *(const bool *)(local_0 - 1) && *local_0)
				break;
		}
	}
	if (state->unknown04 != NONE)
	{
		function_5f972(&session->observer->channels[state->unknown04], session->value10);
		state->unknown04 = NONE;
	}
	memset(state, 0, sizeof(*state));
	state->unknown04 = NONE;
}
#pragma function(_ReadWriteBarrier)

/* ---- the session's secure key ---- */

// @retail 0x5fb60
void network_session_release_key(c_class_58d20 *session)
{
	if (session->flag24)
	{
		if (g_4cf8d4)
			XNetQosListen((XNKID *)&session->unknown1c, 0, 0, 0, XNET_QOS_LISTEN_RELEASE);
		s_xnet_registry_entry *entry = &g_4cf7d4[session->value38];
		if (entry->valid)
		{
			XNetUnregisterKey(&entry->kid);
			entry->valid = false;
		}
		session->observer->owners[session->value10].key_index = NONE;
		memset(&session->unknown1c, 0, 8);
		memset(session->data25, 0, sizeof(session->data25));
		session->value3c = NONE;
		session->flag24 = false;
	}
}

// @retail 0x5fb00
void network_session_set_key(c_class_58d20 *session, long local, const XNKID *kid, const XNKEY *key)
{
	session->flag24 = true;
	*(XNKID *)&session->unknown1c = *kid;
	*(XNKEY *)session->data25 = *key;
	session->value3c = local;
	network_observer_set_owner_key(session->observer, session->value10, session->value38, session->data25, (const s_network_session_id *)&session->unknown1c, local);
	if (g_4cf8d4)
		XNetQosListen((XNKID *)&session->unknown1c, 0, 0, 0, XNET_QOS_LISTEN_ENABLE);
}

// @retail 0x5fa30
bool network_session_create_key(c_class_58d20 *session, long local, long mode)
{
	bool result = false;
	volatile long *local_pointer = &local;
	long local_value = *local_pointer;

	network_session_release_key(session);
	if (transport_security_create_key(local_value, session->value38, mode == 2))
	{
		s_xnet_registry_entry *entry = &g_4cf7d4[session->value38];
		if (entry->valid)
		{
			XNKID kid = entry->kid;
			XNKEY key = entry->key;
			network_session_set_key(session, local_value, &kid, &key);
			result = true;
		}
	}
	return result;
}

// @retail 0x5fac0
bool network_session_join_key(c_class_58d20 *session, long mode, const XNKID *kid, const XNKEY *key, long local)
{
	const long *mode_reference = &mode;
	long mode_value = *mode_reference;
	(void)mode_value;
	network_session_release_key(session);
	if (transport_security_register_key(session->value38, local, false, kid, key))
	{
		network_session_set_key(session, local, kid, key);
		return true;
	}
	return false;
}

/* ---- members ---- */

static inline void function_xd81076(wchar_t *dest, const wchar_t *source, long count)
{
	wcsncpy(dest, source, count - 1);
	dest[count - 1] = 0;
}

// @retail 0x5fbd0
void network_session_add_member(c_class_58d20 *session, long member_index, const s_session_member_identity *identity, bool connected, long channel_index, const s_session_id *id)
{
	s_session_member *member = &session->members[member_index];
	session->member_count++;
	memset(member, 0, sizeof(*member));
	function_xd81076(member->properties.name, L"", 16);
	function_xd81076(member->properties.description, L"", 32);
	member->properties.unknown60 = 0;
	member->properties.unknown64 = 0;
	memset(member->player_indices, NONE, sizeof(member->player_indices));
	member->properties.unknown68 = 0;
	member->properties.unknown6c = 0;
	member->properties.unknown70 = 0;
	for (long i = 0; i < 16; i++)
		((long *)member->properties.unknown84)[i] = 0;
	member->properties.unknownc4 = 0;
	*(s_session_member_identity *)member->words = *identity;
	if (id)
		member->id = *id;
	network_session_member_state_initialize(session, member_index, connected, channel_index);
}

/* sends a message to a member: mode 0 on its channel, 1 out of band, 2 both
   when the channel is established, else out of band */
// @retail 0x62d20
void network_session_send_to_member(c_class_58d20 *session, long member_index, long mode, long message_type, long message_size, void *message)
{
	s_network_session_member_state *state = &session->member_states[member_index];

	if (state->flag1)
	{
		if (mode == 0 || mode == 2 && session->observer->channels[state->unknown04].state == 7)
			network_observer_send_message(session->observer, session->value10, state->unknown04, false, message_type, message_size, message);
		if (mode == 1 || mode == 2)
			network_observer_send_message(session->observer, session->value10, state->unknown04, true, message_type, message_size, message);
	}
}

// @retail 0x62da0
void network_session_send_to_members(c_class_58d20 *session, long mode, long message_type, long message_size, void *message)
{
	for (long i = 0; i < session->member_count; i++)
		network_session_send_to_member(session, i, mode, message_type, message_size, message);
}

// @retail 0x5a2e0
void network_session_check_parameters_acknowledged(c_class_58d20 *session)
{
	if (session_state_is_live(session) && session->function_058d20() && session->flag765c)
	{
		for (long i = 0; i < session->member_count; i++)
		{
			if (session->member_states[i].flag2)
				return;
		}
		session->flag765c = false;
	}
}

// @retail 0x5fe20
void network_session_remove_member(c_class_58d20 *session, long member_index)
{
	long following = session->member_count - member_index - 1;

	for (short slot = 0; slot < 4; slot++)
	{
		long player_index = session->members[member_index].player_indices[slot];
		if (player_index != NONE)
		{
			network_session_remove_player(session, player_index);
			session->value4c++;
			session->update7618++;
		}
	}
	for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
	{
		if ((session->player_mask & (1 << i)) && session->players[i].member_index > member_index)
			session->players[i].member_index--;
	}
	if (session->value50 == member_index)
		session->value50 = 0;
	network_session_member_state_dispose(session, member_index);
	if (following > 0)
	{
		memmove(&session->members[member_index], &session->members[member_index + 1], following * sizeof(s_session_member));
		memmove(&session->member_states[member_index], &session->member_states[member_index + 1], following * sizeof(s_network_session_member_state));
		if (session->member_index > member_index)
			session->member_index--;
		if (session->current_member > member_index)
			session->current_member--;
		if (session->value50 > member_index)
			session->value50--;
	}
	memset(&session->members[session->member_count - 1], 0, sizeof(s_session_member));
	memset(&session->member_states[session->member_count - 1], 0, sizeof(s_network_session_member_state));
	session->member_count--;
	session->value4c++;
	session->update7618++;
	network_session_check_parameters_acknowledged(session);
}
// @retail 0x5fd10
void network_session_boot_member(c_class_58d20 *session, long member_index)
{
	long state = session->state;
	if (state != 7 && state != 6 && state != 8)
	{
		s_session_id message = *(s_session_id *)&session->unknown1c;
		network_session_send_to_member(session, member_index, 2, _network_message_type_session_boot, sizeof(message), &message);
		network_session_remove_member(session, member_index);
	}
}

// @retail 0x5fda0
void network_session_disband_member(c_class_58d20 *session, long member_index)
{
	s_session_id message = *(s_session_id *)&session->unknown1c;
	network_session_send_to_member(session, member_index, 2, _network_message_type_session_disband, sizeof(message), &message);
	network_session_remove_member(session, member_index);
}

/* ---- the session's mode and the members' acknowledgements ---- */

// @retail 0x5a220
void network_session_set_mode(c_class_58d20 *session, long mode)
{
	if (session->type != mode)
	{
		bool waiting = false;
		for (long i = 0; i < session->member_count; i++)
		{
			if (session->member_states[i].flag1)
			{
				session->member_states[i].flag2 = true;
				waiting = true;
			}
		}
		session->value7660 = session->type;
		session->flag765c = true;
		session->time7664 = network_session_time_now();
		session->type = mode;
		session->value497c++;
		session->time4984 = network_session_time_now();
		session->update_count++;
		if (!waiting && session->flag765c)
			session->flag765c = false;
	}
}

struct s_network_message_mode_acknowledge
{
	s_session_id session_id;
	long mode;
	long mode_count;
};

// @retail 0x5a340
bool network_session_handle_mode_acknowledge(c_class_58d20 *session, const s_network_message_mode_acknowledge *message, long remote_index)
{
	long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
	long member_index = network_session_find_member_by_channel_inline(session, channel_index);
	bool result = false;

	if (member_index != NONE && member_index != session->current_member && session->flag765c &&
		message->mode_count == session->value497c && message->mode == session->type)
	{
		s_network_session_member_state *state = &session->member_states[member_index];
		if (state->flag2)
		{
			bool waiting = false;
			state->flag2 = false;
			result = true;
			for (long i = 0; i < session->member_count; i++)
			{
				if (session->member_states[i].flag1 && session->member_states[i].flag2)
				{
					waiting = true;
					break;
				}
			}
			if (!waiting && session->flag765c)
				session->flag765c = false;
		}
	}
	return result;
}

/* ---- leaving and closing ---- */

void network_session_leave_joining(c_class_58d20 *session);
void network_session_leave_join_request(c_class_58d20 *session);
void network_session_disband(c_class_58d20 *session);
void network_session_host_leave(c_class_58d20 *session, bool host, long peer_index);
void network_session_close(c_class_58d20 *session);

struct s_type_fd6c3d
{
	s_session_id session_id;
	long reason;
	byte unknown0c[0x34 - 0xc];
};

__declspec(noinline)
// @retail 0x5a400 standard
void c_class_58d20::leave(bool immediately)
{
	c_class_58d20 *session = this;
	long state = session->state;

	if (state && !function_058d90(session) && !session->flag48)
	{
		switch (state)
		{
		case 3:
			network_session_leave_join_request(session);
			break;
		case 1:
			network_session_leave_joining(session);
			break;
		case 5:
		case 7:
			if (immediately)
				network_session_disband(session);
			else if (state == 7)
				session->flag7420 = true;
			else
				network_session_host_leave(session, true, NONE);
			break;
		case 8:
			session->flag7420 = true;
			break;
		case 9:
			if (immediately)
			{
				s_type_fd6c3d message;
				memset(&message, 0, sizeof(message));
				message.session_id = *(s_session_id *)&session->unknown1c;
				message.reason = 10;
				network_session_send_to_members(session, 2, _network_message_type_election_refuse, sizeof(message), &message);
				break;
			}
			network_session_close(session);
			break;
		case 10:
			if (!immediately)
				network_session_close(session);
			break;
		default:
			__assume(0);
		}
	}
}

// @retail 0x5a520
void network_session_close(c_class_58d20 *session)
{
	if (session->state)
	{
		network_session_leave(session, true);
		if (session->listener)
			session->listener->session_closed();
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if (session->member_states[i].unknown00)
				network_session_member_state_dispose(session, i);
		}
		long local_0 = *(volatile long *)&session->update7650;
		session->current_member = NONE;
		session->update7650 = local_0 + 1;
		memset(session->data761c, 0, sizeof(session->data761c));
		session->value7654 = NONE;
		session->value7658 = NONE;
		session->flag78ac = false;
		session->flag765c = false;
		network_session_release_key(session);
		memset(&session->unknown1c, 0, 8);
		session->flag48 = false;
		session->member_index = NONE;
		memset(&session->value4c, 0, 0x2494);
		session->value4c = NONE;
		memset(&session->update_count, 0, 0x14b0);
		session->update_count = NONE;
		memset(session->reservations, 0, sizeof(session->reservations));
		session->state = 0;
	}
}

// @retail 0x5a600
s_session_id *network_session_get_id(c_class_58d20 *session)
{
	s_session_id *result = 0;

	if (session->state && session->flag24)
		result = (s_session_id *)&session->unknown1c;
	return result;
}

// @retail 0x5a620
bool network_session_get_key(c_class_58d20 *session, s_session_id *id, byte *key, long *key_index, long *local)
{
	bool result = false;

	if (session->state && session->flag24)
	{
		if (id)
			*id = *(s_session_id *)&session->unknown1c;
		if (key)
			*(XNKEY *)key = *(XNKEY *)session->data25;
		if (key_index)
			*key_index = session->value38;
		if (local)
			*local = session->value3c;
		result = true;
	}
	return result;
}

// @retail 0x5a6a0
bool network_session_channel_has_member(c_class_58d20 *session, long channel_index)
{
	return network_session_find_member_by_channel_inline(session, channel_index) != NONE;
}

/* ---- requests a member sends its host ---- */

static __forceinline void network_session_send_to_host(c_class_58d20 *session, long message_type, long message_size, void *message)
{
	s_network_session_member_state *state = &session->member_states[session->member_index];

	if (state->flag1)
		network_observer_send_message(session->observer, session->value10, state->unknown04, false, message_type, message_size, message);
}

// @retail 0x5a6e0
bool network_session_request_mode_acknowledge(c_class_58d20 *session)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->flag49fc = true;
		}
		else
		{
			s_network_message_parameters_request request;
			memset(&request, 0, sizeof(request));
			request.session_id = *(s_session_id *)&session->unknown1c;
			request.change_flag49fc = true;
			request.flag49fc = true;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5a790
bool network_session_set_local_properties(c_class_58d20 *session, const s_session_parameters *properties)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			session->members[session->current_member].properties_valid = true;
			session->members[session->current_member].properties = *properties;
			session->value4c++;
			session->update7618++;
		}
		else
		{
			s_network_message_peer_properties message;
			memset(&message, 0, sizeof(message));
			message.session_id = *(s_session_id *)&session->unknown1c;
			memcpy(message.properties, properties, sizeof(message.properties));
			network_session_send_to_host(session, _network_message_type_peer_properties, sizeof(message), &message);
		}
		result = true;
	}
	return result;
}

long network_session_add_local_player(c_class_58d20 *session, long member_index, long slot, const dword *identity);

// @retail 0x5a880
bool network_session_player_add(c_class_58d20 *session, long slot, const dword *identity, long unknown18, const byte *properties, long unknownac)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long player_index = network_session_add_local_player(session, session->current_member, slot, identity);
			if (player_index != NONE)
			{
				s_network_session_player *player = &session->players[player_index];
				player->unknown14 = unknown18;
				memcpy(player->properties18, properties, sizeof(player->properties18));
				player->unknown138 = unknownac;
				result = true;
			}
		}
		else
		{
			s_network_message_player_add message;
			memset(&message, 0, sizeof(message));
			message.session_id = *(s_session_id *)&session->unknown1c;
			message.slot = slot;
			message.identity[0] = identity[0];
			message.identity[1] = identity[1];
			message.identity[2] = identity[2];
			message.unknown18 = unknown18;
			memcpy(message.properties, properties, sizeof(message.properties));
			message.unknownac = unknownac;
			network_session_send_to_host(session, _network_message_type_player_add, sizeof(message), &message);
			result = true;
		}
	}
	return result;
}

// @retail 0x5a9c0
bool network_session_player_set_properties(c_class_58d20 *session, const byte *properties, long slot, long volatile unknown0c, long unknowna0)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long player_index = session->members[session->current_member].player_indices[slot];
			if (player_index != NONE)
			{
				s_network_session_player *player = &session->players[player_index];
				player->unknown14 = unknown0c;
				memcpy(player->properties18, properties, sizeof(player->properties18));
				player->unknown138 = unknowna0;
				session->value4c++;
				session->update7618++;
				result = true;
			}
		}
		else
		{
			s_network_message_player_properties message;
			memset(&message, 0, sizeof(message));
			s_session_id const *local_0 = (s_session_id const *)&session->unknown1c;
			*(__int64 *)&message.session_id = *(__int64 *)local_0;
			message.slot = slot;
			message.unknown0c = unknown0c;
			message.unknowna0 = unknowna0;
			memcpy(message.properties, properties, sizeof(message.properties));
			network_session_send_to_host(session, _network_message_type_player_properties, sizeof(message), &message);
			result = true;
		}
	}
	return result;
}

// @retail 0x5aae0
bool network_session_player_remove(c_class_58d20 *session, long slot)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long player_index = session->members[session->current_member].player_indices[slot];
			if (player_index != NONE)
			{
				network_session_remove_player(session, player_index);
				session->value4c++;
				session->update7618++;
				result = true;
			}
		}
		else
		{
			s_network_message_player_remove message;
			memset(&message, 0, sizeof(message));
			message.session_id = *(s_session_id *)&session->unknown1c;
			message.slot = slot;
			network_session_send_to_host(session, _network_message_type_player_remove, sizeof(message), &message);
			result = true;
		}
	}
	return result;
}

// @retail 0x5aba0 standard
bool __stdcall network_session_delegate_leader(c_class_58d20 *session, const s_session_member_identity *identity)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		long member_index = network_session_find_member(session, *(const s_session_member_identity *const volatile *)&identity);
		if (member_index != NONE && member_index != session->value50)
		{
			if (session->function_058d20())
			{
				session->value50 = member_index;
				session->value4c++;
				session->update7618++;
			}
			else
			{
				s_network_message_peer_identity message;
				memset(&message, 0, sizeof(message));
				message.session_id = *(s_session_id *)&session->unknown1c;
				message.identity = *identity;
				network_session_send_to_host(session, _network_message_type_delegate_leader, sizeof(message), &message);
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x5acc0 standard
bool __stdcall network_session_boot_machine(c_class_58d20 *session, const s_session_member_identity *identity)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		long member_index = network_session_find_member(session, *(const s_session_member_identity *const volatile *)&identity);
		if (member_index != NONE && member_index != session->current_member)
		{
			if (session->function_058d20())
			{
				network_session_boot_member(session, member_index);
			}
			else
			{
				s_network_message_peer_identity message;
				memset(&message, 0, sizeof(message));
				message.session_id = *(s_session_id *)&session->unknown1c;
				message.identity = *identity;
				network_session_send_to_host(session, _network_message_type_boot_machine, sizeof(message), &message);
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x5ade0
bool network_session_host_boot_member(c_class_58d20 *session, long member_index)
{
	bool local_0 = false;
	if (session->function_058d20())
	{
		if (session->current_member == member_index)
			network_session_disconnect(session, 1);
		else
			network_session_boot_member(session, member_index);
		local_0 = true;
	}
	return local_0;
}

// @retail 0x5ae50
bool network_session_host_become_leader(c_class_58d20 *session)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (session->value50 != session->current_member)
			{
				session->value50 = session->current_member;
				session->value4c++;
				session->update7618++;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x5aeb0
bool network_session_host_leave_to_peer(c_class_58d20 *session, long peer_index)
{
	bool result = false;

	if (session->state == 5)
	{
		network_session_host_leave(session, false, peer_index);
		result = true;
	}
	return result;
}

// @retail 0x5aed0
bool network_session_host_set_player_properties(c_class_58d20 *session, long player_index, const byte *properties)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long state = session->state;
			if (state != 7 && state != 6 && state != 8)
			{
				memcpy(session->players[player_index].propertiesa8, properties, sizeof(session->players[player_index].propertiesa8));
				session->value4c++;
				session->update7618++;
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5af50
bool network_session_is_leaving(c_class_58d20 *session)
{
	long state = session->state;
	if (state == 7 || state == 6 || state == 8)
		return true;
	return false;
}

// @retail 0x5af70
bool network_session_is_full(c_class_58d20 *session, long peer_count, long player_count)
{
    bool result = false;
    if (session_state_is_live(session))
    {
        if (session->member_count + peer_count > session->value4990)
            goto local_0;
        if (session->player_count + player_count <= session->value4994)
            return false;
local_0:
        result = true;
    }
    return result;
}

#pragma inline_depth(0)
// @retail 0x5afc0
bool network_session_players_fit(c_class_58d20 *session, long count, const dword *identities, bool ignore_reservations)
{
	long reserved = 0;

	if (!ignore_reservations)
	{
		for (long i = 0; i < count; i++)
		{
			if (function_062f40((s_reservation_session *)session, &identities[i * 3], 0))
				reserved++;
		}
	}

	long pending = 0;
	s_network_session_reservation *reservation = session->reservations;
	s_network_session_reservation *end = reservation + MAXIMUM_PLAYERS_PER_SESSION;
	for (; reservation < end; reservation++)
	{
		if (reservation->active && !reservation->joined)
			pending++;
	}
	return count <= session->value4994 - session->player_count - pending + reserved;
}
#pragma inline_depth()

/* ---- the session parameters: the host sets them, a member asks the host ---- */

// @retail 0x5b150
long network_session_get_language(c_class_58d20 *session)
{
	long result = get_current_language();
	if (session->state > 2 && session->state <= 8)
		result = session->value4988;
	return result;
}

// @retail 0x5b180
long network_session_get_maximum_players(c_class_58d20 *session)
{
	long result = MAXIMUM_PLAYERS_PER_SESSION;
	if (session->state > 2 && session->state <= 8)
		result = session->value4994;
	return result;
}

// @retail 0x5b1a0
bool network_session_get_data5ddc(c_class_58d20 *session, s_parameters_part *data)
{
	bool result = false;
	if (session->state > 2 && session->state <= 8 && session->flag5dd8)
	{
		memcpy(data, session->data5ddc, sizeof(session->data5ddc));
		result = true;
	}
	return result;
}

static inline void parameters_request_initialize(c_class_58d20 *session, s_network_message_parameters_request *request)
{
	memset(request, 0, sizeof(*request));
	request->session_id = *(s_session_id *)&session->unknown1c;
}

// @retail 0x5b1e0
bool network_session_parameters_set_mode(c_class_58d20 *session, long mode)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			network_session_set_mode(session, mode);
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_mode = true;
			request.mode = mode;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5b2c0
bool network_session_parameters_set_value49a4(c_class_58d20 *session, long value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value49a4 = value;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value49a4 = true;
			request.value49a4 = value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

PRIVATE __forceinline bool function_5b3a1(c_class_58d20 *session)
{
	bool result = true;
	if (session->function_058d20())
	{
		long local_0 = session->update_count;
		session->value49c4 = result;
		session->update_count = local_0 + 1;
	}
	else
	{
		s_network_message_parameters_request request;
		parameters_request_initialize(session, &request);
		request.change_value49c4 = result;
		request.value49c4 = result;
		network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
	}
	return result;
}

// @retail 0x5b3a0
bool network_session_parameters_set_value49c4(c_class_58d20 *session)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
		result = function_5b3a1(session);
	return result;
}

// @retail 0x5b490
bool network_session_parameters_set_value49c8(c_class_58d20 *session, long value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value49c8 = value;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value49c8 = true;
			request.value49c8 = value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5b570
bool network_session_parameters_set_value49f8(c_class_58d20 *session, long unused_value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value49f8 = 0;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value49f8 = true;
			request.value49f8 = 0;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5b650
bool network_session_parameters_set_summary(c_class_58d20 *session, const s_session_summary *summary)
{
	bool local_0 = false;
	if (session_state_is_live(session) && function_058d50(session))
	{
		if (!summary || session_summary_valid(summary))
		{
			if (session->function_058d20())
			{
				if (!summary)
				{
					session->flag49fd = false;
					memset(session->data4a00, 0, sizeof(session->data4a00));
				}
				else
				{
					session->flag49fd = true;
					memcpy(session->data4a00, summary, sizeof(session->data4a00));
				}
				session->update_count++;
			}
			else
			{
				s_network_message_parameters_request request;
				parameters_request_initialize(session, &request);
				request.change_summary = true;
				if (summary)
				{
					request.summary_valid = true;
					memcpy(request.summary, summary, sizeof(request.summary));
				}
				network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
			}
			local_0 = true;
		}
	}
	return local_0;
}

static inline char *function_x91aa57(char *destination, const char *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

// @retail 0x5b790
bool network_session_parameters_set_value4d08(c_class_58d20 *session, long value4d08, long value4d0c, const char *string)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->value4d08 = value4d08;
			session->value4d0c = value4d0c;
			function_x91aa57(session->string4d10, string, sizeof(session->string4d10));
			session->update_count++;
			return true;
		}
		s_network_message_parameters_request request;
		parameters_request_initialize(session, &request);
		request.change_value4d08 = true;
		request.value4d08 = value4d08;
		request.value4d0c = value4d0c;
		function_x91aa57(request.string4d10, string, sizeof(request.string4d10));
		network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		result = true;
	}
	return result;
}

// @retail 0x5b8e0
bool network_session_parameters_set_value4dac(c_class_58d20 *session, long value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value4dac = value;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value4dac = true;
			request.value4dac = value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

struct s_session_data4db0
{
	long unknown00;
	byte unknown04[0x40];
	long unknown44;
	byte unknown48[0x130 - 0x48];
};

// @retail 0x5b9d0
bool network_session_parameters_set_data4db0(c_class_58d20 *session, const s_session_data4db0 *data)
{
	s_session_data4db0 value;
	bool result = false;

	if (data && data->unknown44)
		value = *data;
	else
		memset(&value, 0, sizeof(value));
	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			memcpy(session->data4db0, &value, sizeof(value));
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_data4db0 = true;
			memcpy(request.data4db0, &value, sizeof(value));
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5bb20
bool network_session_parameters_set_value49a1(c_class_58d20 *session, const byte *value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->data49a1[0] = *value;
			session->update_count++;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value49a1 = true;
			request.value49a1 = *value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5bc10
bool network_session_parameters_set_value5dd0(c_class_58d20 *session, short value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value5dd0 = value;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value5dd0 = true;
			request.value5dd0 = value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5bd00
bool network_session_parameters_set_language(c_class_58d20 *session, long language)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value4988 = language;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_language = true;
			request.language = language;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x5bde0
bool network_session_parameters_set_value498c(c_class_58d20 *session, long value)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->update_count++;
			session->value498c = value;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value498c = true;
			request.value498c = value;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
		result = true;
	}
	return result;
}

/* ---- the countdown ---- */

// @retail 0x5df30
bool network_session_set_countdown(c_class_58d20 *session, bool start, long countdown, long mode, long member_index, const long *time)
{
	bool apply = false;
	bool changed = false;

	if (member_index != session->value50)
		start = session->flag49a8;
	if (session->value49b0 && session->value49b4 > countdown)
	{
		session->value49b0 = 0;
		session->value49b4 = NONE;
		memset(session->data49b8, 0, sizeof(session->data49b8));
		session->update_count++;
		changed = true;
	}
	if (mode && start && session->flag49a8)
		apply = true;
	if (session->flag49a8 != start || session->value49ac != countdown || session->value49b0 != mode)
	{
		session->flag49a8 = start;
		session->value49ac = countdown;
		if (apply)
		{
			session->value49b0 = mode;
			session->value49b4 = countdown;
			if (mode == 1)
			{
				memcpy(session->data49b8, time, sizeof(session->data49b8));
				session->update_count++;
				return true;
			}
		}
		else
		{
			session->value49b0 = 0;
			session->value49b4 = NONE;
		}
		memset(session->data49b8, 0, sizeof(session->data49b8));
		session->update_count++;
		changed = true;
	}
	return changed;
}

// @retail 0x5bec0
bool network_session_stop_countdown(c_class_58d20 *session)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			network_session_set_countdown(session, false, 0, 0, session->value50, 0);
			result = true;
		}
	}
	return result;
}

// @retail 0x5bf10
bool network_session_start_countdown(c_class_58d20 *session, bool start, long countdown, long mode, const long *time)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			network_session_set_countdown(session, start, countdown, mode, session->current_member, time);
		}
		else
		{
			s_network_message_countdown_timer message;
			memset(&message, 0, sizeof(message));
			message.session_id = *(s_session_id *)&session->unknown1c;
			message.start = start;
			message.countdown = countdown;
			message.mode = mode;
			if (mode == 1)
			{
				message.time[0] = time[0];
				message.time[1] = time[1];
				message.time[2] = time[2];
			}
			network_session_send_to_host(session, _network_message_type_countdown_timer, sizeof(message), &message);
		}
		result = true;
	}
	return result;
}

// @retail 0x5c190
bool network_session_id_differs(c_class_58d20 *session, const s_parameters_part *part)
{
	bool result = true;

	if (session->state && part)
	{
		if (memcmp(part->unknown04, &session->unknown1c, sizeof(part->unknown04)) == 0)
			result = false;
	}
	return result;
}

// @retail 0x5c010
bool network_session_parameters_set_data5ddc(c_class_58d20 *session, const s_parameters_part *data)
{
	bool result = false;
	if (session_state_is_live(session))
	{
		if (!data || network_session_id_differs(session, data))
		{
			if (function_058d50(session))
			{
				if (session->function_058d20())
				{
					if (!data)
					{
						session->flag5dd8 = false;
						memset(session->data5ddc, 0, sizeof(session->data5ddc));
					}
					else
					{
						session->flag5dd8 = true;
						memcpy(session->data5ddc, data, sizeof(session->data5ddc));
					}
					session->update_count++;
				}
				else
				{
					s_network_message_parameters_request request;
					parameters_request_initialize(session, &request);
					request.change_data5ddc = true;
					if (data)
					{
						request.data5ddc_valid = true;
						memcpy(request.data5ddc, data, sizeof(request.data5ddc));
					}
					network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
				}
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5c1c0
bool network_session_host_set_id49f0(c_class_58d20 *session, const s_session_id *id)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			session->flag49e8 = id != 0;
			if (session->flag49e8)
			{
				session->value49f0 = id->a;
				session->value49f4 = id->b;
			}
			else
			{
				s_session_id *local_0 = (s_session_id *)&session->value49f0;
                memset(local_0, 0, sizeof(*local_0));
			}
			session->update_count++;
			result = true;
		}
	}
	return result;
}

// @retail 0x5c240
bool network_session_host_clear_flag49fc(c_class_58d20 *session)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (session->flag49fc)
			{
				session->flag49fc = false;
				session->update_count++;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x5c290
bool network_session_host_set_value49f8(c_class_58d20 *session, long value)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (session->value49f8 != value)
			{
				session->value49f8 = value;
				session->update_count++;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x5c2e0
bool network_session_host_set_data49cc(c_class_58d20 *session, const dword *data)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (memcmp(data, session->data49cc, sizeof(session->data49cc)) != 0)
			{
				memcpy(session->data49cc, data, sizeof(session->data49cc));
				session->update_count++;
			}
			result = true;
		}
	}
	return result;
}
// @retail 0x5c350
bool network_session_host_set_summary(c_class_58d20 *session, const s_session_summary *summary)
{
	bool result = false;
	if (session_state_is_live(session) && session->function_058d20())
	{
		if (!summary || session_summary_valid(summary))
		{
			if (!summary)
			{
				session->flag49fd = false;
				memset(session->data4a00, 0, sizeof(session->data4a00));
			}
			else
			{
				session->flag49fd = true;
				memcpy(session->data4a00, summary, sizeof(session->data4a00));
			}
			session->update_count++;
			result = true;
		}
	}
	return result;
}

// @retail 0x5c3f0
bool network_session_host_set_data5ddc(c_class_58d20 *session, const s_parameters_part *data)
{
	bool result = false;
	if (session_state_is_live(session) && session->function_058d20())
	{
		if (!data || network_session_id_differs(session, data))
		{
			if (!data)
			{
				session->flag5dd8 = false;
				memset(session->data5ddc, 0, sizeof(session->data5ddc));
			}
			else
			{
				session->flag5dd8 = true;
				memcpy(session->data5ddc, data, sizeof(session->data5ddc));
			}
			session->update_count++;
			result = true;
		}
	}
	return result;
}

/* ---- peers ---- */

bool network_session_channel_is_host_address(c_class_58d20 *session, const s_type_99af70 *address);

#pragma inline_depth(0)
// @retail 0x5f7c0
bool network_session_address_is_peer(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (network_session_channel_is_host_address(session, address))
	{
		result = true;
	}
	else
	{
		long local_0;
		local_0 = session->state;
		if ((local_0 > 2 && local_0 <= 8) || local_0 == 1)
		{
			long member_index = network_session_find_member_by_address(session, address);
			if (member_index != NONE && (long)member_index != session->current_member && member_index == session->member_index)
				result = true;
		}
	}
	return result;
}
#pragma inline_depth()

/* the time synchronization between a peer and the host (type 0x18, 0x1c
   bytes): the peer sends type 0, the host echoes it as type 1, and the peer
   takes its clock offset from the four times */
struct s_type_dd6490
{
	s_session_id session_id;
	long times[4];
	word type;
	byte unknown1a[2];
};

long function_75890(long time);

// @retail 0x62990
void function_62990(c_class_58d20 *session)
{
	s_network_session_member_state *member = &session->member_states[session->member_index];
	if (session->observer->channels[member->unknown04].state == 7 && !function_058d90(session))
	{
		long last_reply = session->value7654;
		if (last_reply == NONE || network_session_time_now() - last_reply > g_network_configuration.value1498)
		{
			if (session->value7658 == NONE || function_75890(session->value7658) > g_network_configuration.value149c)
			{
				s_type_dd6490 message;
				memset(&message, 0, sizeof(message));
				message.type = 0;
				message.session_id = *(s_session_id *)&session->unknown1c;
				memset(message.times, 0xff, 8);
				memset(message.times + 2, 0xff, 8);
				network_observer_send_message(session->observer, session->value10, member->unknown04, true,
					_network_message_type_time_synchronize, sizeof(message), &message);
				session->value7658 = network_session_time_now();
			}
		}
	}
}

// @retail 0x612c0
void function_612c0(c_class_58d20 *session)
{
	long now = network_session_time_now();
	session->update7650++;
	memset(session->data761c, 0, sizeof(session->data761c));
	memset(&session->value7654, 0xff, sizeof(session->value7654));
	memset(&session->value7658, 0xff, sizeof(session->value7658));
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	session->value7420 = now;
	session->state = 3;
	function_62990(session);
}

// @retail 0x5e030
bool __stdcall network_session_handle_time_synchronize(const s_session_id *data, c_class_58d20 *session, const s_type_99af70 *address)
{
	const s_type_dd6490 *message = (const s_type_dd6490 *)data;
	bool result = false;

	if (session_state_is_live(session) || session->state == 1)
	{
		if (session->function_058d20())
		{
			long member_index = network_session_find_member_by_address(session, address);
			if (member_index != NONE && member_index != session->current_member && message->type == 0 && !function_058d90(session))
			{
				s_type_dd6490 reply = *message;
				reply.type = 1;
				function_07b140(session->unknown04, (long)address, _network_message_type_time_synchronize, sizeof(reply), &reply);
				result = true;
			}
		}
		else if (network_session_address_is_peer(session, address) && message->type == 1)
		{
			const long &first = message->times[2] - message->times[0];
			const long &second = message->times[3] - message->times[1];
			session->time78b0 = (first + second) / 2;
			session->flag78ac = true;
			session->value7654 = network_session_time_now();
			result = true;
		}
	}
	return result;
}

// @retail 0x5f810
bool network_session_players_match(c_class_58d20 *session, c_class_58d20 *other)
{
	bool result = false;
	if (session_state_is_live(other) && session_state_is_live(session))
	{
		result = true;
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if ((session->player_mask & (1 << i)) && network_session_find_player(other, (const dword *)&session->players[i]) == NONE)
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

/* ---- the states' data (0x7420) ---- */

/* joining (state 1) */
struct s_session_state_joining_data
{
	long unknown00;
	s_session_member_identity host_identity;
	s_type_99af70 host_address;
	s_session_remote remote;
	long time;
	long unknown1ec;
	long request_count;
	long request_time;
};

/* leaving a join (state 2) */
struct s_session_state_leaving_join_data
{
	s_session_member_identity host_identity;
	s_type_99af70 host_address;
	s_session_member_identity key;
	long nonce[2];
	long join_time;
	long time;
	long last_send_time;
};

/* leaving a join request (state 4) */
struct s_session_state_leaving_data
{
	long time;
	long last_request_time;
};

/* the host leaving (state 7) */
struct s_session_state_host_leaving_data
{
	bool leave;
	byte unknown01[3];
	dword peer_mask;
	long time;
	long unknown0c;
	byte unknown10[0x24 - 0x10];
};

static inline s_session_state_joining_data *session_state_joining(c_class_58d20 *session)
{
	return (s_session_state_joining_data *)&session->value7420;
}

static inline bool transport_address_match(const s_type_99af70 *a, const s_type_99af70 *b)
{
	short length = a->address_length < b->address_length ? a->address_length : b->address_length;
	return a->address_length > 0 && a->address_length == b->address_length && memcmp(a, b, length) == 0;
}

PRIVATE __forceinline void function_5c8f1(short arg_0, short arg_1, short *arg_2)
{
	if (arg_0 > arg_1)
		*arg_2 = arg_1;
	else
		*arg_2 = arg_0;
}

PRIVATE __forceinline bool function_5c8f2(const s_type_99af70 *arg_0, const s_type_99af70 *arg_1)
{
	short local_0;
	function_5c8f1(arg_0->address_length, arg_1->address_length, &local_0);
	return arg_0->address_length > 0 && arg_0->address_length == arg_1->address_length && memcmp(arg_0, arg_1, local_0) == 0;
}

// @retail 0x5c8f0
bool network_session_channel_is_host_address(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (session->state == 1)
		result = function_5c8f2(address, &((s_session_state_joining_data *)&session->value7420)->host_address);
	return result;
}

// @retail 0x5c940
bool network_session_address_is_leaving_host(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (session->state == 2)
		result = function_5c8f2(address, &((s_session_state_leaving_join_data *)&session->value7420)->host_address);
	return result;
}

// @retail 0x60000
long network_session_add_local_player(c_class_58d20 *session, long member_index, long slot, const dword *identity)
{
	long player_index = session->members[member_index].player_indices[slot];

	if (player_index != NONE)
	{
		if (memcmp(&session->players[player_index], identity, 12) == 0)
			return player_index;
		network_session_remove_player(session, player_index);
		session->value4c++;
		session->update7618++;
		player_index = NONE;
	}
	if (network_session_find_player(session, identity) == NONE &&
		(!session->listener || session->listener->player_can_join(identity)))
	{
		s_reservation *reservation;
		if (function_062f40((s_reservation_session *)session, identity, &reservation) || network_session_get_open_slot_count(session) > 0)
		{
			for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
			{
				if (!(session->player_mask & (1 << i)))
				{
					player_index = i;
					break;
				}
			}
			network_session_add_player(session, member_index, (const XUID *)identity, player_index, slot);
			session->value4c++;
			session->update7618++;
		}
	}
	return player_index;
}

void network_session_update_leaving_join(c_class_58d20 *session);
void network_session_update_leaving(c_class_58d20 *session);

// @retail 0x61180
void network_session_leave_joining(c_class_58d20 *session)
{
	s_session_state_leaving_join_data data;

	memset(&data, 0, sizeof(data));
	data.host_identity = session_state_joining(session)->host_identity;
	data.host_address = session_state_joining(session)->host_address;
	data.key = *(s_session_member_identity *)session_state_joining(session)->remote.key188;
	data.nonce[0] = *(long *)&session_state_joining(session)->remote.unknown14d[0x180 - 0x14d];
	data.nonce[1] = *(long *)&session_state_joining(session)->remote.unknown14d[0x184 - 0x14d];
	data.join_time = session_state_joining(session)->time;
	data.time = network_session_time_now();
	network_session_reset_7620(session);
	memset(&session->update_count, 0, 0x14b0);
	session->update_count = NONE;
	memset(&session->value4c, 0, 0x2494);
	session->value4c = NONE;
	session->member_index = NONE;
	for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
	{
		if (session->member_states[i].unknown00)
			network_session_member_state_dispose(session, i);
	}
	memset(&session->current_member, 0xff, sizeof(session->current_member));
	network_session_enter_state_with_data(session, 2, &data, sizeof(data));
	network_session_update_leaving_join(session);
}

// @retail 0x61330
void network_session_leave_join_request(c_class_58d20 *session)
{
	s_session_state_leaving_data data;

	memset(&data, 0, sizeof(data));
	data.time = network_session_time_now();
	data.last_request_time = NONE;
	network_session_enter_state_with_data(session, 4, &data, sizeof(data));
	network_session_update_leaving(session);
}

#pragma inline_depth(0)
// @retail 0x61450
void network_session_disband(c_class_58d20 *session)
{
	s_session_id message = *(s_session_id *)&session->unknown1c;
	network_session_send_to_members(session, 2, _network_message_type_session_disband, sizeof(message), &message);
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	session->state = 6;
}
#pragma inline_depth()

// @retail 0x614a0
void network_session_host_leave(c_class_58d20 *session, bool leave, long peer_index)
{
	if (session->member_count > 1)
	{
		s_session_state_host_leaving_data data;
		memset(&data, 0, sizeof(data));
		data.leave = leave;
		data.unknown0c = NONE;
		data.time = network_session_time_now();
		if (peer_index == NONE)
			data.peer_mask = ((1 << session->member_count) - 1) & ~(1 << session->current_member);
		else
			data.peer_mask = 1 << peer_index;
		network_session_enter_state_with_data(session, 7, &data, sizeof(data));
	}
	else if (leave)
	{
		network_session_leave(session, true);
	}
}

static inline long network_session_time_since(long time)
{
	return network_session_time_now() - time;
}

struct s_network_message_join_abort
{
	s_session_id session_id;
	long nonce[2];
};

// @retail 0x623e0
void network_session_update_leaving_join(c_class_58d20 *session)
{
	s_session_state_leaving_join_data *data = (s_session_state_leaving_join_data *)&session->value7420;

	if (network_session_time_since(data->last_send_time) > g_network_configuration.value1460)
	{
		s_network_message_join_abort message;
		memset(&message, 0, sizeof(message));
		message.session_id = *(s_session_id *)&session->unknown1c;
		message.nonce[0] = data->nonce[0];
		message.nonce[1] = data->nonce[1];
		function_07b140(session->unknown04, (long)&data->host_address, _network_message_type_join_abort, sizeof(message), &message);
		data->last_send_time = network_session_time_now();
	}
}

// @retail 0x62480
void network_session_update_leaving(c_class_58d20 *session)
{
	s_session_state_leaving_data *data = (s_session_state_leaving_data *)&session->value7420;
	long last_request_time = data->last_request_time;

	if (last_request_time == NONE || network_session_time_since(last_request_time) > g_network_configuration.value1470)
	{
		s_session_id message = *(s_session_id *)&session->unknown1c;
		network_session_send_to_member(session, session->member_index, 2, _network_message_type_leave_session, sizeof(message), &message);
		data->last_request_time = network_session_time_now();
	}
}

// @retail 0x618d0
void network_session_update_leaving_join_timeout(c_class_58d20 *session)
{
	s_session_state_leaving_join_data *data = (s_session_state_leaving_join_data *)&session->value7420;

	if (network_session_time_since(data->time) > g_network_configuration.value1464)
		network_session_close(session);
	else
		network_session_update_leaving_join(session);
}

// @retail 0x61910
void network_session_update_leaving_timeout(c_class_58d20 *session)
{
	s_session_state_leaving_data *data = (s_session_state_leaving_data *)&session->value7420;

	if (network_session_time_since(data->time) > g_network_configuration.value146c)
		network_session_close(session);
	else
		network_session_update_leaving(session);
}

void network_session_enter_state_9(c_class_58d20 *session);

static __forceinline bool network_session_state_is_host_local(long state)
{
	bool result = false;
	if (state == 5 || state == 6 || state == 7 || state == 8)
		result = true;
	else
	{
		volatile long unused = state;
	}
	return result;
}

#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x62ab0
void network_session_host_lost(c_class_58d20 *session)
{
	long state = session->state;
	switch (state)
	{
	case 2:
	case 4:
	case 6:
		network_session_close(session);
		return;
	case 7:
	case 8:
		if (session->flag7420)
		{
			network_session_close(session);
			return;
		}
		break;
	case 3:
		_ReadWriteBarrier();
		break;
	case 5:
		break;
	}
	if (state > 2 && state <= 8 && !network_session_state_is_host_local(state) && session->value4c != NONE && session->member_count > 1)
	{
		network_observer_close_channel(session->observer, session->member_states[session->member_index].unknown04);
		network_session_enter_state_9(session);
	}
	else
	{
		network_session_close(session);
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x62eb0
bool network_session_add_reservation(const dword *identity, c_class_58d20 *session, const s_session_id *id, long timeout, long unknown18)
{
	bool local_0 = false;
	s_network_session_reservation *reservations = session->reservations;

	for (s_network_session_reservation *reservation = reservations; reservation < reservations + MAXIMUM_PLAYERS_PER_SESSION; reservation++)
	{
		if (!reservation->active)
		{
			memcpy(reservation->identity, identity, sizeof(reservation->identity));
			reservation->time = network_session_time_now();
			reservation->timeout = timeout;
			memcpy(reservation->id, id, sizeof(reservation->id));
			reservation->unknown18 = unknown18;
			reservation->active = true;
			reservation->joined = network_session_find_player(session, (const dword *)reservation->identity) != NONE;
			local_0 = true;
			goto local_1;
		}
	}
local_1:
	return local_0;
}

// @retail 0x62e70
bool network_session_add_reservations(c_class_58d20 *session, const dword *identities, const s_session_id *id, long count, long timeout, const long *values)
{
	bool result = true;

	for (long i = 0; i < count && result; i++)
		result = network_session_add_reservation(&identities[i * 3], session, id, timeout, values[i]);
	return result;
}

/* ---- the messages the host handles ---- */

struct s_session_peer_map;
struct s_session_member_header;
bool session_peer_map_set_connected(s_session_peer_map *map, const s_session_member_header *member, bool connected);

static inline long network_session_member_from_remote(c_class_58d20 *session, long remote_index)
{
	long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
	return network_session_find_member_by_channel(session, channel_index);
}

static inline long network_session_member_from_remote_inline(c_class_58d20 *session, long remote_index)
{
	long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
	return network_session_find_member_by_channel_inline(session, channel_index);
}

// @retail 0x5dea0
bool network_session_handle_countdown_timer(const s_network_message_countdown_timer *message, c_class_58d20 *session, long remote_index)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote(session, remote_index);
			if (member_index != NONE && member_index != session->current_member)
			{
				network_session_set_countdown(session, message->start, message->countdown, message->mode, member_index, message->mode == 1 ? message->time : 0);
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5e150
bool network_session_handle_session_disband(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;
	if (network_session_address_is_peer(session, address))
	{
		if (network_session_channel_is_host_address(session, address))
		{
			if (network_session_channel_is_host_address(session, address))
				network_session_close(session);
		}
		else
			network_session_host_lost(session);
		result = true;
	}
	return result;
}

// @retail 0x5e1a0
bool network_session_handle_session_boot(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;
	if (network_session_address_is_peer(session, address))
	{
		if (network_session_channel_is_host_address(session, address))
		{
			if (network_session_channel_is_host_address(session, address))
				network_session_close(session);
		}
		else
			network_session_disconnect(session, 1);
		result = true;
	}
	return result;
}

// @retail 0x5e5b0
bool network_session_handle_channel_closed(c_class_58d20 *session, long remote_index)
{
	long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
	long member_index = network_session_find_member_by_channel_inline(session, channel_index);

	if (session_state_is_live(session) && !session->function_058d20())
	{
		if (member_index == session->member_index)
			network_session_host_lost(session);
	}
	else if (session->state == 1 && member_index == session->member_index)
	{
		network_session_leave(session, false);
	}
	return true;
}

// @retail 0x5e640
bool network_session_handle_peer_ready(c_class_58d20 *session, long remote_index)
{
	bool result = false;

	if (session->state == 8)
	{
		long member_index = network_session_member_from_remote(session, remote_index);
		if (member_index != NONE && member_index != session->current_member && member_index != session->member_index)
		{
			*(dword *)&session->flag7430 |= 1 << member_index;
			session->member_states[member_index].flag3 = result;
			result = true;
		}
	}
	return result;
}

/* the reply to a peer's establishment that the session can't take (type
   0x13, 0x30 bytes) */
struct s_type_222535
{
	s_session_id session_id;
	bool unknown08;
	bool member_found;
	bool has_identity;
	byte unknown0b;
	s_session_member_identity identity;
};

/* a peer establishes its channel: an established session marks the member's
   channel up, any other session declines */
// @retail 0x5e6b0
bool network_session_handle_peer_establish(c_class_58d20 *session, long remote_index)
{
	bool result = false;
	long owner = session->value10;
	s_network_observer *observer = session->observer;
	long channel_index = network_observer_find_channel(observer, owner, remote_index);
	long member_index = network_session_find_member_by_channel_inline(session, channel_index);

	if (session->function_058d20() && member_index != NONE && member_index != session->current_member)
	{
		session->member_states[member_index].flag3 = result;
		result = true;
		goto done;
	}
	if (!session->function_058d20())
	{
		s_type_222535 message;
		memset(&message, 0, sizeof(message));
		message.session_id = *(s_session_id *)&session->unknown1c;
		result = true;
		message.unknown08 = result;
		message.member_found = member_index != NONE;
		if (session->state > 2 && session->state <= 8)
		{
			message.has_identity = result;
			message.identity = *(s_session_member_identity *)session->members[session->member_index].words;
		}
		network_observer_send_message(observer, owner, channel_index, false, _network_message_type_host_decline, sizeof(message), &message);
	}
done:
	return result;
}

struct s_network_message_player_refuse
{
	s_session_id session_id;
	long slot;
	dword identity[3];
};

// @retail 0x5f120
bool network_session_handle_player_refuse(c_class_58d20 *session, const s_network_message_player_refuse *message, long remote_index)
{
	if (session_state_is_live(session) && network_session_channel_is_host(session, remote_index))
	{
		long slot = message->slot;
		if (slot >= 0 && slot < 4 && session->members[session->current_member].player_indices[slot] == NONE)
		{
			session->data761c[slot * 13] = true;
			memcpy(&session->data761c[message->slot * 13 + 1], message->identity, sizeof(message->identity));
		}
	}
	return false;
}

// @retail 0x5f190
bool network_session_handle_player_remove(c_class_58d20 *session, long remote_index, const s_network_message_player_remove *message)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote(session, remote_index);
			if (member_index != NONE && member_index != session->current_member)
			{
				long slot = message->slot;
				if (slot >= 0 && slot < 4)
				{
					long player_index = session->members[member_index].player_indices[slot];
					if (player_index != NONE)
					{
						network_session_remove_player_and_update(session, player_index);
						result = true;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x5f360
bool c_class_58d20::channel_is_host_or_local(long channel_index)
{
	c_class_58d20 *session = this;
	bool result = false;

	if (session_state_is_live(session))
	{
		long member_index = network_session_find_member_by_channel(session, channel_index);
		if (member_index == session->member_index)
			result = true;
		else if (session->current_member == session->member_index && member_index != NONE)
			result = true;
	}
	return result;
}

// @retail 0x5f3b0
bool c_class_58d20::channel_is_trusted(long channel_index)
{
	return channel_is_host_or_local(channel_index);
}

// @retail 0x5f3c0
bool c_class_58d20::channel_may_send(long channel_index, bool force)
{
	c_class_58d20 *session = this;
	long member_index = NONE;

	if (channel_index != NONE)
	{
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if (session->member_states[i].unknown00 && session->member_states[i].unknown04 == channel_index)
			{
				member_index = i;
				break;
			}
		}
	}
	bool result = false;
	if (member_index != NONE)
	{
		s_session_member *member = NULL;
		if (session_state_is_live(session))
			member = &session->members[member_index];
		if (session->function_058d20() && !force && (!member || !member->player_count))
			return false;
		if (function_058d90(session) && !session->function_058d20())
			return false;
		result = true;
	}
	return result;
}

// @retail 0x5ece0
bool network_session_handle_peer_reestablish(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (session->state == 9)
	{
		long member_index = network_session_find_member_by_address(session, address);
		if (member_index != NONE && member_index != session->current_member)
		{
			dword bit = 1 << member_index;
			dword *masks = (dword *)((byte *)&session->value7420 + 0xd0);
			if (masks[0] & bit)
				masks[0] &= ~bit;
			if (!(masks[1] & bit))
				masks[1] |= bit;
			if (session_peer_map_set_connected((s_session_peer_map *)&session->flag7430, (const s_session_member_header *)session->members[member_index].words, false))
				session->time7428 = network_session_time_now();
			result = true;
		}
	}
	return result;
}

// @retail 0x5ed90 standard
bool __stdcall network_session_handle_peer_properties(c_class_58d20 *session, long remote_index, const s_network_message_peer_properties *message)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote(session, remote_index);
			if (member_index != NONE && member_index != session->current_member)
			{
				s_session_member *member = &session->members[member_index];
				bool changed = false;
				if (!member->properties_valid)
				{
					member->properties_valid = true;
					changed = true;
				}
				if (memcmp(&member->properties, message->properties, sizeof(member->properties)) != 0)
				{
					memcpy(&member->properties, message->properties, sizeof(member->properties));
				}
				else if (!changed)
				{
					goto success;
				}
				session->value4c++;
				session->update7618++;
			success:
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5ee70 standard
bool __stdcall network_session_handle_delegate_leader(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote(session, remote_index);
			long leader_index = network_session_find_member(session, &message->identity);
			if (member_index == session->value50 && member_index != session->current_member && leader_index != NONE)
			{
				if (session->value50 != leader_index)
				{
					session->value50 = leader_index;
					session->value4c++;
					session->update7618++;
				}
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5e340
bool network_session_handle_host_handoff_acknowledge(c_class_58d20 *session, long remote_index, const byte *message)
{
	bool result = false;

	if (session->state == 7 && session->flag7430)
	{
		long member_index = network_session_member_from_remote(session, remote_index);
		if (member_index != NONE && member_index != session->current_member && member_index != session->member_index)
		{
			short index = *(short *)(message + 0x2c);
			if (index >= 0 && index < session->member_count && index == session->index742c &&
				memcmp(message + 8, session->members[index].words, sizeof(s_session_member_identity)) == 0)
			{
				*(dword *)((byte *)&session->value7420 + 0x18) |= 1 << member_index;
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5ef20 standard
bool __stdcall network_session_handle_boot_machine(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote(session, remote_index);
			long boot_index = network_session_find_member(session, &message->identity);
			if (member_index == session->value50 && member_index != session->current_member && boot_index != NONE && boot_index != member_index)
			{
				if (boot_index == session->current_member)
					network_session_disconnect(session, 1);
				else
					network_session_boot_member(session, boot_index);
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x5c990
void network_session_handle_join_abort_reply(c_class_58d20 *session, const s_type_99af70 *address)
{
	if (session->state == 2)
	{
		const s_type_99af70 *host_address;
		host_address = &((s_session_state_leaving_join_data *)&session->value7420)->host_address;
		byte local_0 = false;
		unsigned long length;
		length = !(address->address_length <= host_address->address_length) ? host_address->address_length : address->address_length;
		if (address->address_length > 0)
			local_0 = address->address_length == host_address->address_length && memcmp(address, host_address, length) == 0;
		if (local_0 != 0)
			network_session_close(session);
	}
}

// @retail 0x5c9e0
bool network_session_member_leave(c_class_58d20 *session, long member_index)
{
	bool result = false;

	if (!network_session_is_leaving(session))
	{
		s_session_id member_id = session->members[member_index].id;
		s_session_id message = *(s_session_id *)&session->unknown1c;
		network_observer_send_message(session->observer, session->value10, session->member_states[member_index].unknown04, true, _network_message_type_leave_acknowledge, sizeof(message), &message);
		network_session_remove_member(session, member_index);
		if (session->listener)
			session->listener->member_left(&member_id);
		result = true;
	}
	else
	{
		long state = session->state;
		if (state == 7)
		{
			network_session_clear_peer(session, member_index);
		}
		else if (state == 8)
		{
			dword &mask = *(dword *)((byte *)&session->value7420 + 0x14);
			if (!(mask & (1 << member_index)))
				mask |= 1 << member_index;
		}
	}
	return result;
}

/* the update that carries the session parameters that changed (network_session_membership.cpp builds it) */
// @retail 0x5cac0
void session_parameters_apply_update(s_session_parameters *parameters, const s_session_parameters_update *update)
{
	if (update->name_changed)
	{
		function_xd81076(parameters->name, update->name, 16);
		function_xd81076(parameters->description, update->description, 32);
	}
	if (update->unknown60_changed)
	{
		parameters->unknown60 = update->unknown60;
		parameters->unknown64 = update->unknown64;
	}
	if (update->unknown68_changed)
	{
		parameters->unknown68 = update->unknown68;
		parameters->unknown6c = update->unknown6c;
		parameters->unknown70 = update->unknown70;
		memcpy(parameters->unknown74, update->unknown74, sizeof(parameters->unknown74));
	}
	if (update->unknown84_changed)
		memcpy(parameters->unknown84, update->unknown84, sizeof(parameters->unknown84));
	if (update->unknownc4_changed)
		parameters->unknownc4 = update->unknownc4;
}

/* a channel of a member connected (a peer tells the host it is there) or
   closed */
// @retail 0x5f480
void c_class_58d20::channel_connection_changed(long channel_index, long remote_index, bool connected)
{
	c_class_58d20 *session = this;
	long member_index = network_session_find_member_by_channel_inline(session, channel_index);

	if (member_index == NONE)
		return;
	if (connected)
	{
		if (session->state > 2 && session->state <= 8 && member_index == session->member_index)
		{
			s_session_id id = *(s_session_id *)&session->unknown1c;
			s_network_session_member_state *state = &session->member_states[member_index];
			if (state->flag1)
				network_observer_send_message(session->observer, session->value10, state->unknown04, false, _network_message_type_peer_establish, sizeof(id), &id);
		}
	}
	else
	{
		s_network_session_member_state *state = &session->member_states[member_index];
		if (network_session_state_is_host_local(session->state))
		{
			state->unknown08 = NONE;
			state->unknown0c = NONE;
			state->flag3 = true;
		}
		else if (member_index == session->member_index)
			network_session_reset_7620(session);
		if (session->state == 8)
		{
			dword mask = ~(1 << member_index);
			*(dword *)&session->index742c &= mask;
			*(dword *)&session->flag7430 &= mask;
		}
	}
}

// @retail 0x5f5c0
long c_class_58d20::find_member_by_channel(long channel_index)
{
	c_class_58d20 *session = this;
	long result = NONE;
	if (channel_index != NONE)
	{
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if (session->member_states[i].unknown00 && session->member_states[i].unknown04 == channel_index)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

struct s_session_summary;
bool session_summary_valid(const s_session_summary *summary);

// @retail 0x5dac0
bool network_session_handle_parameters_request(const s_network_message_parameters_request *message, c_class_58d20 *session, long remote_index)
{
	bool result = false;
	if (session_state_is_live(session) && session->function_058d20())
	{
	long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
	long member_index = network_session_find_member_by_channel(session, channel_index);
	if (member_index != NONE && member_index != session->current_member)
	{
	bool changed = false;
	result = true;
	if (member_index == session->value50)
	{
		if (message->change_mode && session->type != message->mode)
			network_session_set_mode(session, message->mode);
		if (message->language && session->value4988 != message->language)
		{
			session->value4988 = message->language;
			changed = true;
		}
		if (message->change_value498c && session->value498c != message->value498c)
		{
			session->value498c = message->value498c;
			changed = true;
		}
		if (message->change_value49a4 && session->value49a4 != message->value49a4)
		{
			session->value49a4 = message->value49a4;
			changed = true;
		}
		if (message->change_value49c4 && session->value49c4 != message->value49c4)
		{
			session->value49c4 = message->value49c4;
			changed = true;
		}
		if (message->change_value49c8 && session->value49c8 != message->value49c8)
		{
			session->value49c8 = message->value49c8;
			changed = true;
		}
		if (message->change_value49f8 && session->value49f8 != message->value49f8)
		{
			session->value49f8 = message->value49f8;
			changed = true;
		}
		if (message->change_summary)
		{
			bool valid = message->summary_valid;
			if (valid != session->flag49fd || memcmp(session->data4a00, message->summary, sizeof(session->data4a00)) != 0)
			{
				if (!valid)
				{
					session->flag49fd = valid;
					memset(session->data4a00, 0, sizeof(session->data4a00));
					changed = true;
				}
				else if (session_summary_valid((const s_session_summary *)message->summary))
				{
					session->flag49fd = true;
					memcpy(session->data4a00, message->summary, sizeof(session->data4a00));
					changed = true;
				}
				else
				{
					result = false;
				}
			}
		}
		if (message->change_value4d08)
		{
			if (session->value4d08 != message->value4d08 || session->value4d0c != message->value4d0c || strncmp(session->string4d10, message->string4d10, sizeof(session->string4d10)) != 0)
			{
				session->value4d08 = message->value4d08;
				session->value4d0c = message->value4d0c;
				function_x91aa57(session->string4d10, message->string4d10, sizeof(session->string4d10));
				changed = true;
			}
		}
		if (message->change_value4dac && session->value4dac != message->value4dac)
		{
			session->value4dac = message->value4dac;
			changed = true;
		}
		if (message->change_data4db0 && memcmp(session->data4db0, message->data4db0, sizeof(session->data4db0)) != 0)
		{
			memcpy(session->data4db0, message->data4db0, sizeof(session->data4db0));
			changed = true;
		}
		if (message->change_name && wcsncmp(session->name4ee0, message->name, 32) != 0)
		{
			wchar_t *name = session->name4ee0;
			wcsncpy(name, message->name, 31);
			name[31] = 0;
			changed = true;
		}
		if (message->change_value49a1 && memcmp(session->data49a1, &message->value49a1, sizeof(message->value49a1)) != 0)
		{
			session->data49a1[0] = message->value49a1;
			changed = true;
		}
		if (message->change_value5dd0 && session->value5dd0 != message->value5dd0)
		{
			session->value5dd0 = message->value5dd0;
			changed = true;
		}
		if (message->change_data5ddc)
		{
			bool valid = message->data5ddc_valid;
			if (valid != session->flag5dd8 || memcmp(session->data5ddc, message->data5ddc, sizeof(session->data5ddc)) != 0)
			{
				if (!valid)
				{
					session->flag5dd8 = valid;
					memset(session->data5ddc, 0, sizeof(session->data5ddc));
					changed = true;
				}
				else if (network_session_id_differs(session, (const s_parameters_part *)message->data5ddc))
				{
					session->flag5dd8 = true;
					memcpy(session->data5ddc, message->data5ddc, sizeof(session->data5ddc));
					changed = true;
				}
				else
				{
					result = false;
				}
			}
		}
	}
	if (message->change_flag49fc && session->flag49fc != message->flag49fc)
	{
		session->flag49fc = message->flag49fc;
		changed = true;
	}
	if (changed)
		session->update_count++;
	}
	}
	return result;
}

/* host-handoff and peer-handoff (0x2e bytes) */
#pragma pack(push, 2)
struct s_network_message_handoff
{
	s_session_id session_id;
	s_session_member_identity identity;
	short member_index;
};
#pragma pack(pop)

// @retail 0x5e210 standard
bool __stdcall network_session_handle_host_handoff(c_class_58d20 *session, const s_network_message_handoff *message)
{
	bool result = false;
	if (session_state_is_live(session))
	{
		short member_index = message->member_index;
		if (member_index >= 0 && member_index < session->member_count)
		{
			if (memcmp(&message->identity, session->members[member_index].words, sizeof(message->identity)) == 0 &&
				member_index != session->member_index)
			{
				if (member_index != session->current_member || !function_058d90(session))
				{
					s_network_message_handoff reply;
					memset(&reply, 0, sizeof(reply));
					reply.session_id = *(s_session_id *)&session->unknown1c;
					session->value44 = member_index;
					reply.member_index = message->member_index;
					reply.identity = message->identity;
					network_session_send_to_host(session, _network_message_type_peer_handoff, sizeof(reply), &reply);
				}
				result = true;
			}
		}
	}
	else
		network_session_leave(session, false);
	return result;
}

// @retail 0x5f230
bool network_session_handle_player_properties(c_class_58d20 *session, long remote_index, const s_network_message_player_properties *message)
{
	bool result = false;
	if (session_state_is_live(session) && session->function_058d20())
	{
		long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
		long member_index = network_session_find_member_by_channel(session, channel_index);
		if (member_index != NONE && member_index != session->current_member)
		{
			long slot = message->slot;
			if (slot >= 0 && slot < 4)
			{
				long player_index = session->members[member_index].player_indices[slot];
				if (player_index != NONE)
				{
					s_network_session_player *player = &session->players[player_index];
					bool changed = false;
					if (player->unknown14 != message->unknown0c)
					{
						player->unknown14 = message->unknown0c;
						changed = true;
					}
					if (memcmp(player->properties18, message->properties, sizeof(player->properties18)) != 0)
					{
						memcpy(player->properties18, message->properties, sizeof(player->properties18));
						changed = true;
					}
					if (player->unknown138 != message->unknowna0)
					{
						player->unknown138 = message->unknowna0;
						changed = true;
					}
					if (changed)
					{
						session->value4c++;
						session->update7618++;
					}
					result = true;
				}
			}
		}
	}
	return result;
}

/* a join request as the host evaluates it */
struct s_session_join_request
{
	long player_count;
	dword identities[0x52];
	bool ignore_reservations;
	byte unknown14d[0x188 - 0x14d];
	s_session_member_identity identity;
};

bool network_session_is_full(c_class_58d20 *session, long peer_count, long player_count);
bool network_session_players_fit(c_class_58d20 *session, long count, const dword *identities, bool ignore_reservations);

// @retail 0x62fe0
long network_session_evaluate_join_request(c_class_58d20 *session, const s_session_join_request *request)
{
 long result = 0;
 if (!session->function_058d20())
  result = 3;
 else if (session->value498c > 1)
  result = 2;
 else
 {
  long state = session->state;
  if (state == 7 || state == 6 || state == 8)
   result = 3;
  else if (network_session_find_member(session, &request->identity) == NONE)
  {
   long player_count = request->player_count;
   if (network_session_is_full(session, 1, player_count) || !network_session_players_fit(session, player_count, request->identities, request->ignore_reservations))
    result = 4;
  }
 }
 return result;
}

/* the state data of a host transition */
struct s_session_transition_state
{
	long unknown00;
	long host_index;
	long time;
	dword sent_mask;
	dword mask10;
	dword mask14;
};

static __forceinline bool function_62550(s_session_transition_state *arg_0, long arg_1)
{
	dword local_0 = arg_0->sent_mask;
	dword local_1 = 1 << arg_1;
	return (bool)(local_0 & local_1);
}

// @retail 0x62550
void network_session_send_host_reestablish(c_class_58d20 *session)
{
	s_session_transition_state *transition = (s_session_transition_state *)&session->value7420;
	bool timed_out = false;
	dword done = (1 << transition->host_index) | transition->mask14 | transition->mask10;
	if (done == (1 << session->member_count) - 1)
		timed_out = true;
	else
	{
		long start = transition->time;
		if (network_session_time_now() - start >= g_network_configuration.value1484)
			timed_out = true;
	}
	for (long i = 0; i < session->member_count; i++)
	{
		if (i == transition->host_index && !timed_out)
			continue;
		dword bit = 1 << i;
		if (!function_62550(transition, i) && session->member_states[i].flag1)
		{
			long channel_index = session->member_states[i].unknown04;
			s_network_observer *observer = session->observer;
			if (observer->channels[channel_index].state == 7)
			{
				s_session_id id = *(s_session_id *)&session->unknown1c;
				network_observer_send_message(observer, session->value10, channel_index, false, _network_message_type_host_reestablish, sizeof(id), &id);
				transition->sent_mask |= bit;
			}
		}
	}
}

/* the local machine's address (unknown_07a9a0.cpp) */
// @retail 0x61570
void __stdcall function_061570(c_class_58d20 *session, bool flag)
{
	if (session->member_count > 1)
	{
		bool hosting = session->function_058d20();
		long previous_host = session->member_index;
		if (!hosting)
		{
			network_session_reset_membership(session, true);
			session->member_index = session->current_member;
		}
		s_session_transition_state transition;
		memset(&transition, 0, sizeof(transition));
		((byte *)&transition.unknown00)[0] = 0;
		transition.time = network_session_time_now();
		transition.host_index = previous_host;
		((byte *)&transition.unknown00)[1] = flag;
		transition.sent_mask = 1 << session->current_member;
		transition.mask10 = transition.sent_mask;
		network_session_enter_state_with_data(session, 8, &transition, sizeof(transition));
		for (long i = 0; i < session->member_count; i++)
		{
			s_network_session_member_state *member = &session->member_states[i];
			if (member->flag1 && session->observer->channels[member->unknown04].state == 1)
				network_observer_request_channel(session->observer, member->unknown04);
		}
		network_session_send_host_reestablish(session);
	}
	else
		network_session_enter_state_5(session);
}



bool function_07a9b0(void);
void transport_get_random_bytes(long count, byte *buffer);
void network_session_enter_state_5(c_class_58d20 *session);
extern "C" DWORD WINAPI XGetLanguage(void);

// @retail 0x59bd0
bool __stdcall network_session_host(c_class_58d20 *session, long mode, long local, const XNKID *kid, const XNKEY *key, long count, const dword *identities, const long *values, const s_session_id *id, long timeout)
{
	bool result = true;
	s_session_member_identity identity;
	if (!mode)
	{
		memset(&identity, 0, sizeof(identity));
	}
	else
	{
		if (g_transport_globals.initialized && g_transport_globals.started)
			function_07a9b0();
		*(XNADDR *)&identity = g_4cf793;
		if (!g_4cf792)
			result = false;
		else if (kid && key)
		{
			if (!network_session_join_key(session, mode, kid, key, local))
				result = false;
		}
		else if (!network_session_create_key(session, local, mode))
			result = false;
	}
	if (result)
	{
		session->value18 = mode;
		memset(&session->update_count, 0, 0x14b0);
		session->type = 1;
		session->value497c = 0;
		session->time4984 = network_session_time_now();
		session->update_count = 0;
		session->value4988 = get_current_language();
		session->value498c = 0;
		session->value4990 = 16;
		session->value4994 = 16;
		session->value49a4 = 0;
		session->value49c8 = NONE;
		session->value4d08 = NONE;
		session->value4d0c = NONE;
		transport_get_random_bytes(8, (byte *)&session->value4da0);
		session->value5dd0 = NONE;
		session->value49b4 = NONE;
		session->value5e20 = NONE;
		session->value4da8 = 0xdeadbeef;
		session->value4dac = 1;
		session->flag49a8 = false;
		session->value49ac = 0;
		session->value49b0 = 0;
		memset(&session->value4c, 0, 0x2494);
		session->flag48 = false;
		session->member_index = 0;
		session->current_member = 0;
		network_session_add_member(session, session->member_count, &identity, false, NONE, id);
		session->update7618++;
		session->value50 = 0;
		session->value4c = 0;
		memset(&session->value24e0, 0, 0x2494);
		session->value24e0 = NONE;
		memset(&session->value5e28, 0, 0x14b0);
		session->value5e28 = NONE;
		long clock = -network_session_time_now();
		session->flag78ac = true;
		session->time78b0 = clock;
		session->time78b4 = time(NULL);
		network_session_enter_state_5(session);
		if (timeout == NONE || timeout > 0)
			network_session_add_reservations(session, identities, id, count, timeout, values);
	}
	return result;
}

/* sets a session up as owner owner_index of the observer */
struct s_59ad0
{
 byte field_0[0x14];
 c_network_channel_owner *field_14;
 long field_18;
 long field_1c;
};

static __forceinline s_59ad0 *function_59ad0(s_59ad0 *arg_0, c_class_58d20 *arg_1)
{
 arg_0->field_18 = NONE;
 arg_0->field_1c = NONE;
 arg_0->field_14 = (c_network_channel_owner *)arg_1;
 return arg_0;
}

// @retail 0x59ad0
bool network_session_initialize(long owner_index, s_network_observer *observer, c_class_58d20 *session, c_class_58d20 **sessions, long value14, long value38, void *unknown04)
{
	session->value14 = value14;
	session->value38 = value38;
	session->unknown04 = unknown04;
	session->observer = observer;
	*(c_class_58d20 ***)session->unknown0c = sessions;
	session->value10 = owner_index;
	sessions[owner_index] = session;
	s_59ad0 *local_0 = (s_59ad0 *)((byte *)observer + sizeof(s_network_observer_owner) * session->value10);
	function_59ad0(local_0, session);
	session->value18 = NONE;
	session->member_index = NONE;
	session->flag48 = false;
	memset(&session->value4c, 0, 0x2494);
	memset(&session->value24e0, 0, 0x2494);
	session->value24e0 = NONE;
	session->value4c = NONE;
	memset(&session->update_count, 0, 0x14b0);
	memset(&session->value5e28, 0, 0x14b0);
	session->update_count = NONE;
	session->value5e28 = NONE;
	session->state = 0;
	session->current_member = NONE;
	session->update7618 = 0;
	memset(session->member_states, 0, sizeof(session->member_states));
	memset(session->data761c, 0, sizeof(session->data761c));
	session->update7650 = 0;
	session->value7654 = NONE;
	session->value7658 = NONE;
	memset(session->reservations, 0, sizeof(session->reservations));
	session->listener = 0;
	session->flag78ac = false;
	return true;
}

long network_observer_channel_connect_status(s_network_observer *observer, long channel_index);

struct s_session_join_message
{
	word version;
	word unknown02;
	s_session_id session_id;
	s_session_remote remote;
};

// @retail 0x62300
void function_62300(c_class_58d20 *session)
{
	s_session_state_joining_data *data = session_state_joining(session);
	if (network_observer_channel_connect_status(session->observer, data->unknown00) == 2)
	{
		if (network_session_time_since(data->request_time) > g_network_configuration.value1454)
		{
			s_session_join_message message;
			memset(&message, 0, sizeof(message));
			message.version = 2;
			message.session_id = *(s_session_id *)&session->unknown1c;
			message.remote = data->remote;
			function_07b140(session->unknown04, (long)&data->host_address,
				_network_message_type_join_request, sizeof(message), &message);
			data->request_count++;
			data->request_time = network_session_time_now();
		}
	}
}

// @retail 0x617c0
void function_617c0(c_class_58d20 *session)
{
	s_session_state_joining_data *data = session_state_joining(session);
	if (network_observer_channel_connect_status(session->observer, data->unknown00) != 2)
	{
		if (network_session_time_since(data->time) > g_network_configuration.value1458)
		{
			session->leave(false);
			return;
		}
	}
	else
	{
		if (data->unknown1ec == 0)
			data->unknown1ec = network_session_time_now();
		if (session->value4c != NONE && session->update_count != NONE)
		{
			function_612c0(session);
			s_session_id id = *(s_session_id *)&session->unknown1c;
			s_network_session_member_state *member = &session->member_states[session->value50];
			if (member->flag1)
				network_observer_send_message(session->observer, session->value10, member->unknown04,
					false, _network_message_type_peer_establish, sizeof(id), &id);
		}
		else if (network_session_time_since(data->unknown1ec) > g_network_configuration.value145c)
		{
			session->leave(false);
			return;
		}
	}
	if (session->state == 1)
		function_62300(session);
}

void function_07ad50(byte *buffer);

#pragma inline_depth(0)
static __forceinline void function_61051(byte *buffer)
{
 function_07ad50(buffer);
}
#pragma inline_depth(255)

// @retail 0x61050
void function_61050(c_class_58d20 *session, long channel_index,
	const s_session_member_identity *host, const s_session_remote *remote, const s_type_99af70 *address)
{
	s_session_state_joining_data data;
	memset(&data, 0, sizeof(data));
	data.unknown00 = channel_index;
	data.host_identity = *host;
	data.host_address = *address;
	data.time = network_session_time_now();
	data.unknown1ec = 0;
	data.remote = *remote;
	function_61051(&data.remote.unknown14d[0x180 - 0x14d]);
	session->update7650++;
	memset(session->data761c, 0, sizeof(session->data761c));
	session->member_index = 0;
	session->value7654 = NONE;
	session->value7658 = NONE;
	network_session_add_member(session, 0, host, true, channel_index, 0);
	network_session_add_member(session, 1, (const s_session_member_identity *)remote->key188, false, NONE, 0);
	session->current_member = 1;
	network_session_enter_state_with_data(session, 1, &data, sizeof(data));
	function_62300(session);
}

long network_observer_attach_channel(s_network_observer *observer, long owner_index, const XNADDR *address);

// @retail 0x5c720
bool __stdcall function_5c720(c_class_58d20 *session, const s_session_member_identity *identity,
	long player_count, const dword *identities, const long *values, bool reserve,
	long timeout, const s_session_id *id, long *reason)
{
	bool result = true;
	s_session_machine_address machine = *(const s_session_machine_address *)((const byte *)identity + 0xa);
	long member = network_session_find_member_by_machine(session, &machine);
	long channel;
	if (member == NONE)
	{
		long owner = session->value10;
		s_network_observer *observer = session->observer;
		channel = network_observer_attach_channel(observer, owner, (const XNADDR *)identity);
		if (channel == NONE)
		{
			result = false;
			*reason = 7;
			goto done;
		}
	}
	else
	{
		s_network_session_member_state *state = &session->member_states[member];
		if (!state->flag1)
		{
			result = false;
			*reason = 5;
			goto done;
		}
		if (!memcmp(session->members[member].words, identity, sizeof(*identity)))
		{
			long time = state->unknown10;
			long now = network_session_time_now();
			if (!session->members[member].properties_valid && now - time < g_network_configuration.value1468)
				goto done;
		}
		network_session_remove_member(session, member);
		channel = network_observer_attach_channel(session->observer, session->value10, (const XNADDR *)identity);
		if (channel != NONE)
			network_observer_close_channel(session->observer, channel);
		else
		{
			result = false;
			*reason = 7;
			goto done;
		}
	}
	if (reserve)
	{
		for (long i = 0; i < player_count; i++)
		{
			s_reservation *reservation;
			if (function_062f40((s_reservation_session *)session, identities + i * 3, &reservation))
			{
				reservation = 0;
				if (function_062f40((s_reservation_session *)session, identities + i * 3, &reservation))
					*(bool *)reservation = false;
			}
			network_session_add_reservation(identities + i * 3, session, id, timeout, values[i]);
		}
	}
	network_session_add_member(session, session->member_count, identity, true, channel, id);
	network_observer_request_channel(session->observer, channel);
	session->value4c++;
	session->update7618++;
done:
	return result;
}

// @retail 0x630f0
bool function_630f0(c_class_58d20 *session, long address,
 const s_session_join_request *request, long reason)
{
 bool result = false;
 if (!reason && function_5c720(session, &request->identity, request->player_count,
  request->identities, (const long *)((const byte *)request + 0xc4),
  request->ignore_reservations, *(const long *)((const byte *)request + 0x150),
  (const s_session_id *)((const byte *)request + 0x144), &reason))
  result = true;
 else
 {
  long message[3];
  memset(message, 0, sizeof(message));
  message[0] = session->unknown1c;
  message[1] = session->unknown20;
  message[2] = reason;
  function_07b140(session->unknown04, address, 10, sizeof(message), message);
 }
 return result;
}

bool network_observer_channel_timed_out(s_network_observer *observer, long owner, long channel_index);

/* retail calls 0x5f6a0 and 0x58d20 here instead of inlining them */
#pragma inline_depth(0)
// Both callers of 0x769a0 load session+0x10 and store it dead ahead of the call (0x5e469/0x5e473; the same in 0x5e7f0),
// so 0x769a0 takes an unread owner parameter ahead of the channel index (m3 lane 14 r108 from round 65's form).
// @retail 0x5e3f0
bool __stdcall function_05e3f0(c_class_58d20 *session, const s_type_99af70 *address)
{
 long member = network_session_find_member_by_address(session, address);
 bool was_host = false;
 bool leave = false;
 if (member != NONE && member != session->current_member)
 {
  long state = session->state;
  bool take = false;
  if (state == 9)
   take = true;
  else if (state > 2 && state <= 8)
  {
   long host = session->member_index;
   if (member != host)
   {
    if (member == session->value44)
     take = true;
    else if (!session->function_058d20())
    {
     s_network_session_member_state *host_state = &session->member_states[host];
     __assume(host_state != NULL);
     long owner = session->value10;
     if (!network_observer_channel_timed_out(session->observer, owner, host_state->unknown04))
      take = true;
    }
   }
  }
  if (take)
  {
   if (session->function_058d20())
   {
    struct s_handoff_announcement
    {
     s_session_id id;
     byte flags[4];
     s_session_member_identity identity;
    } message;
    memset(&message, 0, sizeof(message));
    message.id = *(s_session_id *)&session->unknown1c;
    message.flags[0] = 1;
    message.flags[1] = 1;
    message.flags[2] = 1;
    memcpy(&message.identity, session->members[member].words, sizeof(message.identity));
    network_session_send_to_members(session, 0, 0x13, sizeof(message), &message);
    was_host = true;
    if (session->state == 7 && *(byte *)&session->value7420)
     leave = true;
   }
   session->member_index = member;
   session->value44 = NONE;
   if (was_host)
    network_session_reset_membership(session, false);
   if (session->state != 4)
    function_612c0(session);
  }
 }
 if (network_session_address_is_peer(session, address))
 {
  s_session_id id = *(s_session_id *)&session->unknown1c;
  s_network_session_member_state *member_state = &session->member_states[session->member_index];
  if (member_state->flag1)
   network_observer_send_message(session->observer, session->value10, member_state->unknown04,
    false, 0x14, sizeof(id), &id);
  if (leave)
   session->leave(false);
  return true;
 }
 return false;
}
#pragma inline_depth()

struct s_network_session_list;
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *id);
bool function_07ab10(long key_index, long local, word port, const XNADDR *xnaddr, s_type_99af70 *address);

__forceinline long network_session_time_now(void);

// @retail 0x59e50
bool __stdcall function_59e50(c_class_58d20 *session, long mode, long local,
 const XNKID *kid, const XNKEY *key, const s_session_member_identity *host,
 long count, const dword *identities, const long *values, const long *other_values,
 bool reserve, long timeout, const s_session_id *id, const void *extra)
{
 bool result = false;
 c_class_58d20 *existing = network_session_manager_find_session(
  *(s_network_session_list **)session->unknown0c, (const s_session_id *)kid);
 if (existing)
  network_session_close(existing);
 if (g_transport_globals.initialized && g_transport_globals.started)
  function_07a9b0();
 XNADDR identity = g_4cf793;
 if (g_4cf792)
 {
  network_session_release_key(session);
  if (transport_security_register_key(session->value38, local, false, kid, key))
  {
   network_session_set_key(session, local, kid, key);
   s_type_99af70 address;
   if (function_07ab10(session->value38, local, 0x3e9, (const XNADDR *)host, &address))
   {
    bool loopback = false;
    if (address.address_length == 4)
     loopback = address.ipv4_address == 0x7f000001;
    if (!loopback)
    {
     long owner = session->value10;
     s_network_observer *observer = session->observer;
     long channel = network_observer_attach_channel(observer, owner, (const XNADDR *)host);
     if (channel == NONE)
      result = false;
     else
     {
      session->value18 = mode;
      memset(&session->update_count, 0, 0x14b0);
      session->update_count = NONE;
      session->flag48 = false;
      memset(&session->value4c, 0, 0x2494);
      session->value4c = NONE;
      session->member_index = NONE;
      memset(session->member_states, 0, sizeof(session->member_states));
      session->current_member = NONE;
      session->time78b0 = -network_session_time_now();
      session->flag78ac = true;
      s_session_remote remote;
      memset(&remote, 0, sizeof(remote));
      remote.id = count;
      memcpy(remote.address04, identities, count * 12);
      memcpy((byte *)&remote + 0xc4, values, count * 4);
      memcpy((byte *)&remote + 0x104, other_values, count * 4);
      remote.has_address = reserve;
      if (reserve)
       *(long *)((byte *)&remote + 0x150) = timeout;
      memcpy(remote.address144, id, sizeof(*id));
      memcpy(remote.key188, &identity, sizeof(identity));
      if (extra)
       memcpy((byte *)&remote + 0x154, extra, 0x2c);
      function_61050(session, channel, host, &remote, &address);
      result = true;
     }
    }
   }
  }
 }
 if (!result)
  network_session_release_key(session);
 return result;
}

struct s_network_message_session_query;

// @retail 0x63080
bool __stdcall function_063080(c_class_58d20 *session, const s_network_message_session_query *message,
 const s_type_99af70 *address)
{
 s_session_join_request request;
 memcpy(&request, (const byte *)message + 0xc, sizeof(request));
 long reason = network_session_evaluate_join_request(session, &request);
 if (!reason && session->listener)
  session->listener->unknown00(address, &request);
 else
  function_630f0(session, (long)address, &request, reason);
 return true;
}

inline long count_bits(dword value);

bool function_619b0(const s_session_member *members, long first_index, long second_index, long player_count);

struct s_session_host_choice
{
 bool leave;
 byte unknown01[3];
 dword candidates;
 long search_time;
 long selected;
 bool proposed;
 byte unknown11[3];
 long proposal_time;
 dword acknowledged;
 bool committed;
 byte unknown1d[3];
 long commit_time;
};

#pragma pack(push, 1)
struct s_session_host_proposal
{
 s_session_id id;
 s_session_member_identity member;
 short index;
};
#pragma pack(pop)

// @retail 0x61ac0
void __stdcall function_61ac0(c_class_58d20 *session)
{
 s_session_host_choice *choice = (s_session_host_choice *)&session->value7420;
 bool complete = false;
 if (choice->selected == NONE)
 {
  if (choice->candidates)
  {
   long selected = choice->leave ? NONE : session->current_member;
   dword eligible = 0;
   for (long i = 0; i < session->member_count; i++)
   {
    if ((choice->candidates & (1 << i)) &&
     session->member_states[i].unknown08 == session->value4c &&
     session->member_states[i].unknown0c == *(long *)((byte *)session + 0x4978) &&
     (choice->leave || *(dword *)((byte *)&session->members[i] + 0x9c) == (1u << session->member_count) - 1))
    {
     eligible |= 1 << i;
     if (selected == NONE || function_619b0(session->members, i, selected, session->member_count))
      selected = i;
    }
   }
   if (selected == session->current_member)
   {
    if (eligible == choice->candidates)
     complete = true;
   }
   else if (selected != NONE)
   {
    choice->selected = selected;
    choice->candidates &= ~(1 << selected);
   }
  }
  else
   complete = true;
 }
 if (choice->selected != NONE)
 {
  if (!choice->proposed)
  {
   s_session_host_proposal message;
   memset(&message, 0, sizeof(message));
   message.id = *(s_session_id *)&session->unknown1c;
   message.index = (short)choice->selected;
   message.member = *(s_session_member_identity *)&session->members[choice->selected];
   network_session_send_to_members(session, 0, 15, sizeof(message), &message);
   choice->proposed = true;
   choice->proposal_time = network_session_time_now();
   choice->acknowledged = 1 << session->current_member;
  }
  if (choice->proposed && !choice->committed)
  {
   long local_0 = choice->proposal_time;
   long elapsed = network_session_time_now() - local_0;
   bool commit = false;
   if (choice->acknowledged == (1u << session->member_count) - 1)
    commit = true;
   else if (elapsed >= g_network_configuration.value1474)
   {
    if (choice->acknowledged & (1 << choice->selected))
     commit = true;
    else
     network_session_clear_peer(session, choice->selected);
   }
   if (commit)
   {
    s_session_id id = *(s_session_id *)&session->unknown1c;
    s_network_session_member_state *member = &session->member_states[choice->selected];
    if (member->flag1)
     network_observer_send_message(session->observer, session->value10, member->unknown04, false, 17, sizeof(id), &id);
    choice->committed = true;
    choice->commit_time = network_session_time_now();
    session->value44 = choice->selected;
   }
  }
  if (choice->committed)
  {
   long commit_time = choice->commit_time;
   if (network_session_time_now() - commit_time >= g_network_configuration.value147c)
    network_session_clear_peer(session, choice->selected);
  }
 }
 else
 {
  long local_2 = choice->search_time;
  if (network_session_time_now() - local_2 >= g_network_configuration.value1478)
   complete = true;
 }
 if (complete)
 {
  bool leave = choice->leave;
  network_session_enter_state_5(session);
  if (leave)
   session->leave(true);
 }
}

struct s_session_peer_transition
{
 long start_time;
 long send_time;
 long selection_time;
 long selected;
 byte map[0xc0];
 dword pending;
 dword failed;
 long times[16];
};
long network_session_build_peer_map(c_class_58d20 *session, s_session_peer_map *map);
long session_peer_map_find_machine(s_session_peer_map *map, const s_session_member_header *member);

// @retail 0x61ef0
void __stdcall function_61ef0(c_class_58d20 *session)
{
 long one = 1;
 bool flag = true;
 s_session_peer_transition *transition = (s_session_peer_transition *)&session->value7420;
 for (long i = 0; i < session->member_count; i++)
 {
  if (i != session->current_member)
  {
   /* retail reads pending from memory here, not from a cached register */
   dword pending = *(volatile dword *)&transition->pending;
   dword bit = one << i;
   if (pending & bit)
   {
    if (network_session_time_since(transition->times[i]) >= g_network_configuration.value148c)
    {
     transition->pending &= ~bit;
     transition->failed |= bit;
    }
    if (transition->pending & bit)
     continue;
   }
   if (!(transition->failed & bit) && network_session_time_since(transition->start_time) >= g_network_configuration.value148c)
    transition->failed |= bit;
  }
 }
 if (transition->selected == session->current_member)
 {
  long failed = 0;
  long missing = 0;
  s_session_peer_map *map = (s_session_peer_map *)transition->map;
  for (long i = 0; i < session->member_count; i++)
  {
   if (transition->failed & (one << i))
    failed++;
   else
   {
    long index = session_peer_map_find_machine(map, (const s_session_member_header *)&session->members[i]);
    bool present = false;
    if (index != NONE) present = (*(dword *)((byte *)map + 0xbc) & (one << index)) != 0;
    if (!present) missing++;
   }
  }
  if ((failed <= 0 || (network_session_time_since(transition->start_time) >= g_network_configuration.value148c &&
    network_session_time_since(transition->selection_time) >= g_network_configuration.value1490)) && !missing)
   function_061570(session, flag);
 }
 else if (transition->failed & (one << transition->selected))
 {
  transition->selected = network_session_build_peer_map(session, (s_session_peer_map *)transition->map);
  transition->selection_time = network_session_time_now();
 }
 if (session->state == 9)
 {
  if (!transition->send_time || network_session_time_since(transition->send_time) >= g_network_configuration.value1494 ||
   (transition->selection_time - transition->send_time > 0 && network_session_time_since(transition->send_time) >= 500))
  {
   struct { s_session_id id; byte map[0xc0]; } message;
   memset(&message, 0, sizeof(message));
   message.id = *(s_session_id *)&session->unknown1c;
   memcpy(message.map, transition->map, sizeof(message.map));
   for (long i = 0; i < session->member_count; i++)
   {
    s_network_session_member_state *member = &session->member_states[i];
    if (member->flag1)
     network_observer_send_message(session->observer, session->value10, member->unknown04, flag, 22, sizeof(message), &message);
   }
   transition->send_time = network_session_time_now();
  }
  if (network_session_time_since(transition->start_time) >= g_network_configuration.value1488)
   network_session_close(session);
 }
}

struct s_session_property_value;
void session_property_value_set_kind1(s_session_property_value *value, long a, long b, long c, long d);
void session_property_value_set_kind2(s_session_property_value *value, long value14);
void session_property_value_set_kind3(s_session_property_value *value, long value18);
void session_property_value_set_kind4(s_session_property_value *value);

// @retail 0x5d5a0
bool __stdcall function_5d5a0(c_class_58d20 *session, const byte *update, byte *output)
{
 bool result = false;
 memcpy(output, &session->update_count, 0x14b0);
 if (*(const long *)(update + 0xc) != NONE && *(const long *)(update + 0xc) != session->update_count)
  goto done;
 *(long *)output = *(const long *)(update + 8);
 volatile bool valid = true;
 if (update[0x10])
 {
  *(long *)(output + 8) = *(const long *)(update + 0x14);
  *(long *)(output + 4) = *(const long *)(update + 0x18);
  *(long *)(output + 0xc) = network_session_time_now();
 }
 if (update[0x1c]) memcpy(output + 0x10, update + 0x20, 4);
 if (update[0x24]) memcpy(output + 0x14, update + 0x28, 4);
 if (update[0x2c]) memcpy(output + 0x18, update + 0x30, 8);
 if (update[0x38])
 {
  output[0x20] = update[0x39];
  if (update[0x39]) memcpy(output + 0x21, update + 0x3a, 8);
 }
 if (update[0x42]) memcpy(output + 0x2c, update + 0x44, 4);
 if (update[0x48]) memcpy(output + 0x4c, update + 0x49, 1);
 if (update[0x4a]) memcpy(output + 0x50, update + 0x4c, 4);
 if (update[0x50]) memcpy(output + 0x84, update + 0x51, 1);
 if (update[0x70]) memcpy(output + 0x80, update + 0x74, 4);
 if (update[0x52])
 {
  switch (*(const long *)(update + 0x54))
  {
  case 1: session_property_value_set_kind1((s_session_property_value *)(output + 0x54), *(const long *)(update + 0x58), *(const long *)(update + 0x5c), *(const long *)(update + 0x60), *(const long *)(update + 0x64)); break;
  case 2: session_property_value_set_kind2((s_session_property_value *)(output + 0x54), *(const long *)(update + 0x68)); break;
  case 3: session_property_value_set_kind3((s_session_property_value *)(output + 0x54), *(const long *)(update + 0x6c)); break;
  case 4: session_property_value_set_kind4((s_session_property_value *)(output + 0x54)); break;
  default: *(long *)(output + 0x54) = 0; *(long *)(output + 0x58) = 0; *(long *)(output + 0x5c) = 0; *(long *)(output + 0x60) = 0; *(long *)(output + 0x64) = 0; *(long *)(output + 0x68) = 0; *(long *)(output + 0x6c) = 0; break;
  }
 }
 if (update[0x88])
 {
  if (!update[0x89])
  {
   output[0x85] = false;
   memset(output + 0x88, 0, 0x308);
  }
  else if (session_summary_valid((const s_session_summary *)(update + 0x8c)))
  {
   output[0x85] = true;
   memcpy(output + 0x88, update + 0x8c, 0x308);
  }
  else valid = false;
 }
 if (update[0x78])
 {
  byte flag = update[0x79];
  output[0x70] = flag;
  if (!flag) memset(output + 0x78, 0, 8);
  else memcpy(output + 0x78, update + 0x80, 8);
 }
 if (update[0x394])
 {
  *(long *)(output + 0x390) = *(const long *)(update + 0x398);
  *(long *)(output + 0x394) = *(const long *)(update + 0x39c);
  char *name = (char *)output + 0x398;
  strncpy(name, (const char *)update + 0x3a0, 0x80);
  name[0x7f] = 0;
 }
 if (update[0x420]) memcpy(output + 0x428, update + 0x428, 0x8);
 if (update[0x430]) memcpy(output + 0x430, update + 0x434, 0x4);
 if (update[0x438]) memcpy(output + 0x434, update + 0x43c, 0x4);
 if (update[0x440]) memcpy(output + 0x438, update + 0x444, 0x130);
 if (update[0x574])
 {
  wchar_t *name = (wchar_t *)(output + 0x568);
  wcsncpy(name, (const wchar_t *)(update + 0x576), 31);
  name[31] = 0;
 }
 if (update[0x5b6])
 {
  output[0x5a8] = update[0x5b7];
  memcpy(output + 0x5ac, update + 0x5b8, 0x6c);
  memcpy(output + 0x618, update + 0x624, 0xe40);
 }
 if (update[0x1464]) output[0x29] = update[0x1465];
 if (update[0x1466]) *(word *)(output + 0x1458) = *(const word *)(update + 0x1468);
 if (update[0x146a])
 {
  output[0x30] = update[0x146b];
  *(long *)(output + 0x34) = *(const long *)(update + 0x146c);
  long mode = *(const long *)(update + 0x1470);
  *(long *)(output + 0x38) = mode;
  if (mode == 1)
  {
   *(long *)(output + 0x3c) = *(const long *)(update + 0x1474);
   memcpy(output + 0x40, update + 0x1478, 12);
  }
  else
  {
   if (mode == 2) *(long *)(output + 0x3c) = *(const long *)(update + 0x1474);
   else *(long *)(output + 0x3c) = NONE;
   memset(output + 0x40, 0, 12);
  }
 }
 if (update[0x1484])
 {
  if (!update[0x1485])
  {
   output[0x1460] = false;
   memset(output + 0x1464, 0, 0x44);
  }
  else
  {
   const byte *data = update + 0x1488;
   if (network_session_id_differs(session, (const s_parameters_part *)data))
   {
   output[0x1460] = true;
   memcpy(output + 0x1464, data, 0x44);
   }
   else valid = false;
  }
 }
 if (update[0x14cc]) *(long *)(output + 0x14a8) = *(const long *)(update + 0x14d0);
 result = valid;
done:
 return result;
}

// @retail 0x609e0
void __stdcall function_609e0(c_class_58d20 *session, const byte *current, const byte *previous, byte *update)
{
 memset(update, 0, 0x14d8);
 memcpy(update, &session->unknown1c, 8);
 *(long *)(update + 8) = *(const long *)current;
 long local_1;
 if (!previous)
  local_1 = NONE;
 else
  local_1 = *(const long *)previous;
 *(long *)(update + 0xc) = local_1;
 if (!previous || (*(const long *)(current + 4) != *(const long *)(previous + 4) || *(const long *)(current + 0x8) != *(const long *)(previous + 0x8)))
 {
  update[0x10] = true;
  memcpy(update + 0x14, current + 8, 4);
  memcpy(update + 0x18, current + 4, 4);
  update[0x11] = *((byte *)session + 0x765c);
 }
 if (!previous || memcmp(current + 0x10, previous + 0x10, 0x4))
 {
  update[0x1c] = true;
  memcpy(update + 0x20, current + 0x10, 0x4);
 }
 if (!previous || memcmp(current + 0x14, previous + 0x14, 0x4))
 {
  update[0x24] = true;
  memcpy(update + 0x28, current + 0x14, 0x4);
 }
 if (!previous || (*(const long *)(current + 0x18) != *(const long *)(previous + 0x18) || *(const long *)(current + 0x1c) != *(const long *)(previous + 0x1c)))
 {
  update[0x2c] = true;
  memcpy(update + 0x30, current + 0x18, 0x8);
 }
 if (!previous || current[0x20] != previous[0x20] || memcmp(current + 0x21, previous + 0x21, 8))
 {
  update[0x38] = true;
  byte local_2 = current[0x20];
  update[0x39] = local_2;
  if (local_2) memcpy(update + 0x3a, current + 0x21, 8);
 }
 if (!previous || memcmp(current + 0x2c, previous + 0x2c, 0x4))
 {
  update[0x42] = true;
  memcpy(update + 0x44, current + 0x2c, 0x4);
 }
 if (!previous || memcmp(current + 0x4c, previous + 0x4c, 0x1))
 {
  update[0x48] = true;
  memcpy(update + 0x49, current + 0x4c, 0x1);
 }
 if (!previous || memcmp(current + 0x50, previous + 0x50, 0x4))
 {
  update[0x4a] = true;
  memcpy(update + 0x4c, current + 0x50, 0x4);
 }
 if (!previous || memcmp(current + 0x84, previous + 0x84, 0x1))
 {
  update[0x50] = true;
  memcpy(update + 0x51, current + 0x84, 0x1);
 }
 if (!previous || memcmp(current + 0x80, previous + 0x80, 0x4))
 {
  update[0x70] = true;
  memcpy(update + 0x74, current + 0x80, 0x4);
 }
 if (!previous || memcmp(current + 0x54, previous + 0x54, 0x1c))
 {
  update[0x52] = true;
  *(long *)(update + 0x54) = *(const long *)(current + 0x54);
  *(long *)(update + 0x68) = 0;
  *(long *)(update + 0x6c) = 0;
  *(long *)(update + 0x58) = 0;
  *(long *)(update + 0x60) = 0;
  *(long *)(update + 0x5c) = 0;
  *(long *)(update + 0x64) = 0;
  if (*(const long *)(update + 0x54) == 1)
  {
   *(long *)(update + 0x58) = *(const long *)(current + 0x58);
   *(long *)(update + 0x60) = *(const long *)(current + 0x60);
   *(long *)(update + 0x5c) = *(const long *)(current + 0x5c);
   *(long *)(update + 0x64) = *(const long *)(current + 0x64);
  }
  else if (*(const long *)(update + 0x54) == 2) memcpy(update + 0x68, current + 0x68, 4);
  else if (*(const long *)(update + 0x54) == 3) memcpy(update + 0x6c, current + 0x6c, 4);
 }
 if (!previous || current[0x85] != previous[0x85] || memcmp(current + 0x88, previous + 0x88, 0x308))
 {
  update[0x88] = true;
  update[0x89] = current[0x85];
  memcpy(update + 0x8c, current + 0x88, 0x308);
 }
 if (!previous || current[0x70] != previous[0x70] || memcmp(current + 0x78, previous + 0x78, 8))
 {
  update[0x78] = true;
  update[0x79] = current[0x70];
  memcpy(update + 0x80, current + 0x78, 8);
 }
 if (!previous || (*(const long *)(current + 0x390) != *(const long *)(previous + 0x390) || *(const long *)(current + 0x394) != *(const long *)(previous + 0x394)) ||
  strncmp((const char *)current + 0x398, (const char *)previous + 0x398, 0x80))
 {
  update[0x394] = true;
  memcpy(update + 0x398, current + 0x390, 8);
  char *local_3 = (char *)update + 0x3a0;
  strncpy(local_3, (const char *)current + 0x398, 0x80);
  local_3[0x7f] = 0;
 }
 if (!previous || (*(const long *)(current + 0x428) != *(const long *)(previous + 0x428) || *(const long *)(current + 0x42c) != *(const long *)(previous + 0x42c)))
 {
  update[0x420] = true;
  memcpy(update + 0x428, current + 0x428, 0x8);
 }
 if (!previous || memcmp(current + 0x430, previous + 0x430, 0x4))
 {
  update[0x430] = true;
  memcpy(update + 0x434, current + 0x430, 0x4);
 }
 if (!previous || memcmp(current + 0x434, previous + 0x434, 0x4))
 {
  update[0x438] = true;
  memcpy(update + 0x43c, current + 0x434, 0x4);
 }
 if (!previous || memcmp(current + 0x438, previous + 0x438, 0x130))
 {
  update[0x440] = true;
  memcpy(update + 0x444, current + 0x438, 0x130);
 }
 if (!previous || wcsncmp((const wchar_t *)(current + 0x568), (const wchar_t *)(previous + 0x568), 32))
 {
  update[0x574] = true;
  wchar_t *local_6 = (wchar_t *)(update + 0x576);
  wcsncpy(local_6, (const wchar_t *)(current + 0x568), 31);
  local_6[31] = 0;
 }
 if (!previous || current[0x5a8] != previous[0x5a8] || memcmp(current + 0x5ac, previous + 0x5ac, 0x6c) || memcmp(current + 0x618, previous + 0x618, 0xe40))
 {
  update[0x5b6] = true;
  update[0x5b7] = current[0x5a8];
  memcpy(update + 0x5b8, current + 0x5ac, 0x6c);
  memcpy(update + 0x624, current + 0x618, 0xe40);
 }
 const byte *local_11 = previous;
 if (!local_11 || memcmp(current + 0x29, local_11 + 0x29, 0x1))
 {
  update[0x1464] = true;
  memcpy(update + 0x1465, current + 0x29, 0x1);
 }
 if (!local_11 || memcmp(current + 0x1458, local_11 + 0x1458, 0x2))
 {
  update[0x1466] = true;
  memcpy(update + 0x1468, current + 0x1458, 0x2);
 }
 if (!local_11 || current[0x30] != local_11[0x30] || (*(const long *)(current + 0x34) != *(const long *)(local_11 + 0x34) || *(const long *)(current + 0x38) != *(const long *)(local_11 + 0x38) || *(const long *)(current + 0x3c) != *(const long *)(local_11 + 0x3c) || memcmp(current + 0x40, local_11 + 0x40, 0xc)))
 {
  update[0x146a] = true;
  update[0x146b] = current[0x30];
  *(long *)(update + 0x146c) = *(const long *)(current + 0x34);
  *(long *)(update + 0x1470) = *(const long *)(current + 0x38);
  *(long *)(update + 0x1474) = *(const long *)(current + 0x3c);
  memcpy(update + 0x1478, current + 0x40, 0xc);
 }
 if (!local_11 || current[0x1460] != local_11[0x1460] || memcmp(current + 0x1464, local_11 + 0x1464, 0x44))
 {
  update[0x1484] = true;
  update[0x1485] = current[0x1460];
  memcpy(update + 0x1488, current + 0x1464, 0x44);
 }
 if (!local_11 || memcmp(current + 0x14a8, local_11 + 0x14a8, 0x4))
 {
  update[0x14cc] = true;
  memcpy(update + 0x14d0, current + 0x14a8, 0x4);
 }
}

bool session_peer_map_update_reachable(s_session_member_header *a, s_session_peer_map *map, s_session_member_header *b);
bool session_peer_map_set_connected(s_session_peer_map *map, const s_session_member_header *member, bool connected);

/* retail calls 0x5f6a0, 0x58d20, 0x58d90 and 0x75870 here instead of
   inlining them; only the time read for the peer's reply time is inline */
#pragma inline_depth(0)
// score 295 (was 295; r239, r126, r286, r275 and r234 unchanged, 305 -> 295 in r208): frame 0xd8 and the length agree; what is left
// is in the paragraph below (the two stacked score lines merged in r286).
// earlier header line (merged in r126): 295 (was 295): Frame and length agree. Rejection branches still lead to separate exits,
// and acknowledgement is laid out early. Rounds 275 and 234 kept the round 208 score.
// earlier header line (merged in r126): 295 (was 295; r275 and r234 unchanged, 305 -> 295 in r208): frame 0xd8 and the length agree; the tie-break written as separate statements puts the three
// MiniMax D (11-20): no new form, no lower score (295): retail reads the accepted byte back at every exit where this build folds it,
// the acknowledgement block comes last in retail, and the three boolean homes stay rotated; the timeout spill needs 0x769a0's third
// argument, whose declaration lives in another file: BLOCKED-BY 0x769a0.
// earlier header line (merged in r126): 295 (was 295): r275 unchanged. Frame 0xd8 and the length agree; the tie-break written as separate statements puts the three
// MiniMax H (11-20): no new form, no lower score (295): retail reads the accepted byte back at every exit where this build folds it; the acknowledgement block comes last in retail; the timeout spill needs 0x769a0.
// Earlier at 295 (was 305): frame 0xd8 and the length agree; the tie-break written as separate statements puts the three
// flag bytes on retail's homes (better +9, same/identity +0xa, result +0xb) and the exit after the disconnect returns
// directly (1264 B). Left: retail reads the accepted byte back at every exit where ours folds it at the reason-1/2/9
// exits, the acknowledgement block is laid out last in retail, the timeout call lacks retail's owner spill (the earlier
// BLOCKED-BY 0x769a0 reading is superseded: see the round 208 paragraph at the end, which also records a match-only 96
// form), and the end of the received block differs. The rounds' notes follow in the order they were written.
// MiniMax H (11-20): no new form, no lower score (295): retail reads the accepted byte back at every exit where this build folds it; the acknowledgement block comes last in retail; the timeout spill needs 0x769a0.
// MiniMax C (11-16): no new form, no lower score (305): the frame and the length agree, but the three boolean homes
// stay rotated against retail's (retail: better +9, same +0xa, result +0xb) and retail's call to the matched 0x769a0
// passes three arguments - the observer on the stack (push eax), the session's value10 in EDX (dead at the callee) and
// the channel index in EAX - where our declaration of that callee takes two, so the extra load and the spill it causes
// are missing here too; that parameter list lives in include/unknown_075870.h and src/unknown_075870.cpp, which this
// lane may not edit.
// MiniMax E (11-15): no new form, no lower score (305): the call boundaries agree with retail's (twenty-one calls in the same order), so what is left is the rotation of the three boolean stack homes and the timeout spill, which needs 0x769a0's unread owner parameter in another file. BLOCKED-BY 0x769a0.
// MiniMax E (11-16): no new form, no lower score (305): the frame and the length agree, but the three boolean
// homes stay rotated against retail's (retail: better +9, same +0xa, result +0xb) and the 0x769a0 call misses
// retail's third argument for the same reason as in 0x5e3f0; that parameter list lives in
// include/unknown_075870.h and src/unknown_075870.cpp, which this lane may not edit. BLOCKED-BY 0x769a0.
// MiniMax C (11-17): no new form, no lower score (305, was 305): the frame (0xd8) and the length agree, but the boolean homes are still a rotation from retail's (the acceptance byte at +0xa against retail's +0xb, the identity byte at +0x9 against retail's +0xa), retail still reads the acceptance byte back where this build folds it to a constant, and the same timeout spill as 0x5e3f0 needs 0x769a0, whose definition is in src/unknown_075870.cpp: BLOCKED-BY 0x769a0.
// MiniMax G (11-18): no new form, no lower score (305): the frame 0xd8 and the length agree, but the three boolean stack homes are still a
// rotation from retail's (retail: better +9, same +0xa, result +0xb) and the timeout spill needs 0x769a0's unread owner parameter, whose
// definition lives in src/unknown_075870.cpp: BLOCKED-BY 0x769a0.
// Earlier 305 (was 305): frame 0xd8 and length agree; the byte homes differ (retail: better +9,
// frame 0xd8 and length agree; result/identity/selection byte homes and
// acknowledgement placement differ.
// Earlier 305 (was 310): the invalid-state exits and the missing-sender exit return the result directly
// instead of going through the shared exit, so each one gets its own epilogue and the extent matches
// retail's 1264 bytes. The three boolean stack homes are still rotated relative to retail.
// Earlier 310 (was 318): the acknowledgement block returns the result directly instead of going through
// the shared exit, so its epilogue is the one retail emits. The three boolean stack homes are still
// rotated relative to retail and the invalid-state exit is still emitted twice.
// Earlier 318 (was 325): the channel-state test keeps the identity comparison in place instead of copying
// it into a named flag, so the comparison is tested once as retail does. The three boolean stack homes
// are still rotated relative to retail and the invalid-state exit is still emitted twice.
// Earlier 325 (was 335): the acceptance test is written as the no-sender case followed by the
// acknowledgement, so the acknowledgement block stays out of line at the end of the function instead of
// being pulled into the middle of it. The three boolean stack homes are still rotated relative to retail.
// Earlier 344 (was 348): the state test is a separate if/else-if chain so the invalid-state exit is a
// distinct arm, the peer-map update takes the remote member first (the callee is symmetric in its two
// member arguments), and the channel-state test is split into a named flag. The three boolean stack
// homes are still rotated relative to retail and the invalid-state exit is still duplicated.
// Earlier 348 (was 356): the valid-state range is an else-if chain, so the invalid-state exit is a
// separate arm instead of a merged join, and the identity pointer is taken before the member-state
// pointer. The acceptance/identity byte homes and the duplicated invalid-state exit remain.
// Byte homes differ (retail: better +9,
// identity/same +0xa, result +0xb; ours: identity/same +9, result +0xa, better +0xb), retail reads
// `result` back from its home at every return (ours folds the two reason-2 returns to `xor al, al`
// and keeps them as two blocks, where retail has one), and the acknowledgement block is laid out
// after the first identity test in ours, last in retail.
// Kept along the way (356 -> 305): the valid-state range as an else-if chain, the identity pointer taken
// before the member-state pointer, the peer-map update with the remote member first (the callee is
// symmetric in its two members), the acceptance test as the no-sender case followed by the acknowledgement
// (which then stays at the end), the channel-state test on the identity comparison in place, and the
// acknowledgement, invalid-state and missing-sender exits returning the result directly (own epilogues,
// extent 1264).
// Tried: Round 87: one `state <= 2 || state > 8` reason-2 test 368 (the block moves to the end, 1248
// bytes), reversed `state > 8 || state <= 2` 370; renamed result/better/identity flag 305 each (names do
// not move these slots); acknowledgement block after `done:` 305; same declared before better 305;
// volatile result (diagnostic only, reverted) 317 at 1278 bytes. Earlier: the identity comparison as a
// predicate with the two timeout reasons as separate arms 358 (1265 bytes).
// shared exit 407, result first 411, role names 411, complete reason arms 411,
// separate rejection tests 424, positive state arm 411.
// Member pointers before received copy 396; host workspaces swapped 413;
// isolated sender rename 407; preference results first 407; old version first 407.
// Round 113: Equality-name 396; shared-result name 396; entry host-equality scope 396; direct version fields 396; nested preference tie-break 395 (one-point gain dropped).
// Identifier entry workspace 398; received entry workspace 396; long identity equality 402;
// long preference results 406; equal-version decision first 422.
// Result byte +9 versus +b, identity/host homes and early acknowledgement/rejection remain. Raw multi-use
// comparison 384 kept; preference-only long 405; equality-only long 405; integer acceptance 395; peer before
// identifier 396; chained peer clears 396.
// Raw-comparison slot name 384; reversed comparison buffers 384; library identity outputs 384;
// direct state observations 387; result after state 384. Frame/Boolean homes and block placement remain.
// Word identity result 384; word preference/equality states 396/396; local member first 386;
// selection flags after map updates 384; word reachability counts 383.
// Negative identity predicate 396; signed-byte/unsigned raw identity result 384/384; direct
// map/transition/peer fields 384/384/370; preferred-first processing 384; selection declaration
// order 384.
// Round 356: Byte acceptance 387; word acceptance 394; paired byte selection 384; complete version admission 410; admission with byte selection 410; isolated invalid-state/role/missing-selection exits 386 each.
// Native received/stored/both map records 384/412/412; native input alone/with stored fields
// 384/412; native output/both assignments 412/412; native records with identity equality 420.
// Native identifier initialization/high-word-first construction 370/370; direct host-channel fields 376;
// direct fields with high-word-first identifier 376; equality on direct-acknowledgement base 382;
// equality with native identifier/high-word-first/both direct fields 382/382/383. Original body restored.
// Round 225: On direct acknowledgement base, all returns raw/equality 395/408; identity-output returns
// 374/386; completed-success returns 389/402; direct selection results 377/383. Acceptance/identity homes
// remain.
// Round 227: Newer preference from predicate 386; equality from predicate 363 (seven-point gain
// dropped); both 385; tie preference from predicate 368 (two-point gain dropped); all three 380; single
// long/byte/short selection status 396/404/394.
// Round 227: Follow-up equality plus tie predicates 372; newer plus tie predicates 371. Neither
// combination improves the original 370; original body restored.
// Round 229: Signed-byte acceptance/preference/equality/paired selection/all results 401/370/370/370/401;
// identity equality with signed-byte preference/equality/all results 382/382/392. Original body restored.
// Round 341: Incoming payload before lookup with stored/direct identity and raw/equality results 397/402/394/408; after lookup 390/398/392/396. Original body restored.
// Round 206: Four complete acceptance/preference/equal-choice role-name allocations 370 each;
// all four allocations with native identity equality 382 each. Raw comparison and original names restored.
// Round 211: Entry preference/equality/both lifetimes 381/381/381; success default alone/with
// preference/equality/both entry lifetimes 381/391/391/391; native identifier library copy 370.
// Round 145: Equal-version rejection before preference: original predicates 396; newer predicate
// 393; equality predicate 394; both predicates 391. Identity-equality counterparts
// 406/403/404/401; all restored.
// Round 214: Native-equality/distinct-exit graph: acceptance, selection, both, identity, identity/acceptance,
// identity/selection and all Boolean home names 305 each; all names with entry selection constants 372.
// All executable forms restored; result/identity/selection homes and acknowledgement placement remain.
// Round 89: every exit through `done:` 318 (1278 bytes; every return then reads `result` from its home
// as in retail, but the state <= 2 exit keeps its own block); only the three state-test exits through
// `done:` 308; all exits through `done:` with one `state <= 2 || state > 8` test 357 (1264 bytes, the
// reason-2 block placed last); better/same declared at entry 367, with every exit through `done:` 381.
// Round 72: sender test inverted ahead of reason 5 317; acknowledgement after the shared exit 305
// (same code); its result set before the state-7 test 305; preference/equal flags swapped 305;
// both declared after the member lookup 314. Retail reloads the result byte on every exit.
// On native-equality/distinct-policy exits: byte/char acceptance, with original or paired-byte
// selection, 406/406/406/406; word/short acceptance with either selection domain 410 each.
// All larger than retail; original truth carriers and body restored.
// Round 301: Complementary acceptance/preference/equality result storage, seven nonempty
// combinations 429/305/429/305/429/305/429; all three with complementary identity 429.
// Original truth carriers and complete body restored; Boolean homes and acknowledgement placement remain.
// Round 209: Positive policy interval before/inside state-nine admission 375/369; with immutable
// repeated identity 375/369; with one completion-result exit 365/358; with both 365/358.
// Completion exits recover length 1264 but regress; opening policy graph, acknowledgement paths,
// result lifetime and held timeout interface restored.
// Round 453: Complete version-arm preference/equality/both outputs 304/306/300;
// explicit identity-result arms on those three 304/306/300; both outputs with delayed
// selection declarations/entry identity declaration 300/300. Both output homes with
// all/initial-state shared exits 318/302. The 300 form recovers result +b but gains only five; dropped.
// Round 30: the timeout call also lacks retail's dead owner spill (BLOCKED-BY 0x769a0, held file).
// Acknowledgement as a forceinline helper at both sites 506 (1121 bytes: the inline_depth(0) region keeps
// it out of line); with every exit through `done:` 498; written out at both sites 622 (1478 bytes, no
// cross-jump); selected-index compare through `map` 317 (1244 bytes); tie-break compare through `map` 317.
// Round 32: an unused owner parameter on 0x769a0 alone does not give the owner store (305, scratch copy);
// an owner-taking channel-state accessor after the call stays a call (305). `better`/`same` at the top of
// the received block 305; `result` before the sender lookup 310; `result` between sender and state 305;
// `!memcmp` identity test 305; state-8 sender test inverted 305. Owner also on 0x616e0 (enter state 9) with
// an `owner` local 374 (scratch copy; loaded at the top).
// Round 287: Shared timeout identity publication with conditional reason and original/newer/
// equal/both selection predicates 322/331/324/330; explicit reason arms with those same four forms
// 322/331/324/330. Original timeout publication, selection producers and body restored.
// Round 34: state-8 site as `if (sender == NONE) { result = true; goto done; } goto acknowledge;` 305 (same code);
// acknowledgement falling into `done:` 311 (1270 bytes: retail's store-and-reload exit, block still mid-function);
// state-8 `return result` 345; acknowledgement written at the host site 305 and at the state-8 site 305 (identical
// code: VC lays the block out after the host branch's reason-5 exit wherever it sits in source).
// Round 215: complete decision outputs recover result +b; dword/byte/short counts
// on that graph 298/314/299; native received packet 298, with those domains 298/314/299;
// complete native stored fields 323. Seven Boolean-home name combinations 298 each;
// all names without redundant selection defaults 298. Seven-point leads rejected;
// original body, packet types, output states and held interfaces restored.
// Identity/selection byte homes and acknowledgement/rejection placement remain.
// Round 34: state-8 site as `if (sender == NONE) { result = true; goto done; } goto acknowledge;` 305 (same code);
// acknowledgement falling into `done:` 311 (1270 bytes: retail's store-and-reload exit, block still mid-function);
// state-8 `return result` 345; acknowledgement written at the host site 305 and at the state-8 site 305 (identical
// code: VC lays the block out after the host branch's reason-5 exit wherever it sits in source).
// Round 139 (m3 lane 5, fast): identity flag declared at entry 305 (same code); `result` after the state-0/10 exit
// 307; payload address named before the identity compare 305; `result` before the sender lookup 310; `state <= 2`
// arm first in the range chain 373 (1246 bytes).
// Round 58 (lane 14, fast; aligned counts in brackets, this body [245]): host site as `if (sender != NONE) goto acknowledge;` 317 [265] (the block then follows
// directly); reason 9 as the fall-through after the received block, retail's order 305 [244] (1265 bytes), with an else goto at the host site 305 [244], with the
// acknowledgement falling out of its block 313 [245] (1272 bytes); `done:` ahead of the acknowledgement 305 [245], with reason 9 last 305 [244]; a flag set at both sites
// and tested at the end 305 [245]. The block stays right after the reason-5 exit in all but the first: source position and label order do not move it.
// Round 65 (m3 lane 13, plain; 0x769a0 with the unread owner parameter from 0x5e3f0's note, scratch): the owner store appears with a named owner local,
// so this needs that same line (BLOCKED-BY 0x769a0). Layout, not registers, is what's left. Opening body with the owner local 365, masked 481 (reason 2
// sinks: `jg` where retail falls into it; it stays in place only with the unnamed argument, 305); nested single return 362 (masked 513), exits read
// `result` from its home as retail because the shared return is tail-duplicated late; all exits through one `done:` and `state <= 2 || state > 8` 358
// (masked 181), reason 2 sinks; acknowledgement written at both sites 604 (1472 bytes, no cross-jump); `__assume(!result)` 365; returns with that assume and
// one `||` 386 (1231 bytes). Pointer to `result` for its stores 305 (same code, it folds away). Retail's later blocks come in source order once a trace ends
// (reason 1 right after the first version arm's `jmp`), and the acknowledgement's two entries differ by `mov edi, [esp+0x10]`.
// Walk b3N (11-06), statement by statement against retail 0x5e7f0-0x5ecdd: every reason code (1-9), the state tests,
// the host identity compare (9 dwords at message+8), the timeout/channel-state tests, the version/preference
// tie-break, the pending/failed bit updates, the reply time, the acknowledgement and the clear_peer tail agree;
// no field, direction or constant differs. Two differences: (1) retail calls 0x62c20 as (local, received, remote)
// at 0x5ea21 and (local, map, remote) at 0x5ea36 (edx = local both times, remote pushed first), so the second call is
// `(local, map, remote)`; the earlier `(remote, map, local)` was a score-only swap (callee is symmetric in its two
// members), it scored 305 with `remote` declared first and 306 (1265 bytes) in retail order; declaring `local`
// before `remote` with retail order gives 305. (2) retail stores session->value10 into [esp+0x1c] at 0x5e91b before
// the 0x769a0 call (its unread owner parameter, see the 0x5e3f0 note); not expressible until 0x769a0 takes it.
// MiniMax C (11-15): no new form, no lower score (305): the frame and the body length agree, but retail keeps its
// flag results in stack bytes (the result byte at +0xb where this build frames it at +0xa) and stores the session's
// value10 into a slot before its 0x769a0 call, whose unread owner parameter the callee in src/unknown_075870.cpp does
// not take, so the extra store cannot be written from here.
// MiniMax B (11-11): no new form, no lower score (305); retail keeps its flag results in stack slots (the byte
// at esp+0xb, where this build keeps it in al and frames it at esp+0xa) and stores session->value10 into a slot
// before its 0x769a0 call, whose unread owner parameter this build's callee in src/unknown_075870.cpp does not
// take, so the extra store in the caller cannot be written here.
// MiniMax A (11-11): no new form, no lower score (305); the two byte flags sit one slot lower than retail's and the second pair of byte slots one higher, so the difference is a slot permutation this build does not reproduce from the source.
// MiniMax R (11-11): no new form, no lower score (305): the frames agree (0xd8 with the same two pushes) and the
// body length agrees, but the flag results are kept in registers and al where retail keeps them in stack bytes
// (the byte at esp+0xb, this build at esp+0xa), and retail stores the session's value10 into a slot before its
// 0x769a0 call, whose unread owner parameter the callee in another file does not take.
// MiniMax H (11-13): no new form, no lower score (305): the three boolean stack homes are still rotated (the
// result byte at +0xa where retail keeps it at +0xb), and the same timeout spill as 0x5e3f0 needs 0x769a0, whose
// definition is in another file.
// MiniMax G (11-12): no new form, no lower score (305); the same timeout spill as 0x5e3f0 sits here and
// needs 0x769a0, whose definition is in src/unknown_075870.cpp; what is left in this file is the
// rotation of the three boolean stack homes (the result byte at +0xa where retail keeps it at +0xb).
// MiniMax S (11-11): no new form, no lower score (305); the frames agree (0xd8 with the same two pushes) but our
// body is one byte longer than retail's (1265 against 1264), and the result byte still lives at esp+0xa where
// retail keeps it at esp+0xb.
// MiniMax R (11-10): no new form, no lower score (305); the three flag bytes keep their homes a rotation apart
// from retail's (retail: the peer block's better flag at esp+0x9, the same flag sharing the identity test's slot at
// esp+0xa, the result at esp+0xb; ours: the same flag at esp+0x9, the result at esp+0xa, better at esp+0xb), and the
// two peer flags written as bytes instead of bools measure the same code (305), so the body stands.
// MiniMax S (11-10): no new form, no lower score (305): retail stores the session's value10 into [esp+0x1c] before the 0x769a0 call, which needs that callee to take its unread owner parameter, and retail calls 0x62c20 the second time as (local, map, remote) where ours passes the map in the other place; both need a file this lane may not edit.
// MiniMax D (11-10): no new form, no lower score (305).
// MiniMax h (11-05): the result byte home is +0xa in ours and +0xb in retail, where the better flag shares +0xa with the identity-equal flag; exchanging the better and same declarations gives 305, spelling out the better decision gives 314 and the same decision 306, and the shared acknowledgement block sits after the reason-5 exit in ours but at the end of retail's.
// MiniMax h (11-04): no new form, no lower score; the result byte stays at +0xa where retail keeps it at +0xb.
// MiniMax B (11-13): no new form, no lower score (305): the boolean homes stay a rotation from retail's (the
// acceptance byte at +0xa against retail's +0xb, the identity byte at +0x9 against retail's +0xa, and the two peer
// bytes are zeroed at +0x11/+0x13 where retail zeroes its pair at +0x11/+0x12), and retail reads the acceptance byte
// back where this build folds it to a constant.
// MiniMax I (11-14): no new form, no lower score (305): the frame 0xd8 and the body length agree with retail's, but the three boolean homes stay a rotation from retail's (the acceptance byte at +0xa against retail's +0xb, the identity byte at +0x9 against retail's +0xa), retail reads the acceptance byte back where this build folds it to a constant, and the 0x769a0 and 0x62c20 call setups need files this lane may not edit.
// M3 lane 3 r242 (fast, base 305; retail's two reason-2 exits are one block, `state <= 2` jumping into the `state > 8`
// fall-through): the valid range as one `state > 2 && state <= 8` test with the exit as its else 366 (1264 B, the shared exit
// laid out after the body); the reason-3 test as one `&&` 305 (same code); the channel-state test as one `&&` 305 (same code);
// the timed-out arm with one identity copy and `*reason = identity_equal ? 4 : 6` 320 (1234 B); the `state > 8` test inverted
// with the exit as its else 367 (1269 B). Body unchanged.
// MiniMax D (11-16): no new form, no lower score (305).
// MiniMax S (11-16): no new form, no lower score (305): the frame and the body length agree with retail's, but
// the boolean homes stay a rotation from retail's (the acceptance byte at +0xa against retail's +0xb, the
// identity byte at +0x9 against retail's +0xa) and the two peer bytes are zeroed at +0x11/+0x13 where retail
// zeroes its pair at +0x11/+0x12.
// Earlier 305 (was 305): MiniMax D (11-17): no new form, no lower score (305): the frame and the body length agree with
// retail's, but the three boolean homes stay a rotation from retail's (this build's acceptance byte at +0xa against
// retail's +0xb, its identity byte at +0x9 against retail's +0xa, and its third byte at +0xb against retail's +0x9),
// retail reads the acceptance byte back where this build folds it to a constant, and this build zeroes its pair of peer
// bytes at +0x11/+0x13 where retail zeroes its pair at +0x11/+0x12.
// m3 lane 16 r132 (fast with 0x5e3f0; base 305): `return result;` in place of `goto done;` after the disconnect 301
// (1264 B, retail's length; a four-point gain, not kept); `return result;` for the same/better exit 313 (1273 B); `result =
// true;` set before the pending/failed bit block 337 (1232 B); `if (!same && !better) goto disconnected; ... goto done;`
// 305; `bool better = false, same = false;` 305. The acknowledgement as a forceinline helper at both sites re-measured 506
// (round 30's form: the inline_depth(0) region keeps it a call). Still BLOCKED-BY 0x769a0 for the timeout spill.
// M3 lane 13 r110 (fast with 0x5e3f0, which matched in each; control 305): the three flags as members of one local
// aggregate (better, same, accepted; the identity test reusing `same`) 305, with all three cleared at entry 305, with
// the members in the other order 305; the three as one local array in either order 305 and 305 (all the same code:
// members and constant-index elements are scalar-replaced). Reading: retail's flag bytes are contiguous (+9, +0xa,
// +0xb) and are stored and read back from memory at every exit with no constant propagated, which no plain local,
// aggregate member or array element gives here; their addresses must escape (out-parameters of a helper that retail
// expands inline), and this body cannot expand a helper inside its inline_depth(0) region.
// M3 lane 1 r124 (fast with 0x5e3f0/0x5f6a0/0x58d90/0x58d20/0x769a0/0x62c20/0x5f760/0x63ca0, all MATCH each run; control
// 305, difflib-aligned 470, masked 462): the aligned count is almost all one block placed elsewhere: ours lays the
// acknowledgement block straight after the host path's reason-5 exit (behind its unconditional `goto acknowledge`),
// retail keeps it at its source place after the reason-9 exit; everything else is in retail's order. Forms: the host
// path as `if (sender != NONE) goto acknowledge;` ahead of the reason-5 code (the state-8 test's own form) 317 (the
// block is then laid inline after the test), with reason 5 ending `return result;` 317; the base with reason 5 ending
// `return result;` 308 (1264 B); `result` declared first 310; `same` declared ahead of `better` 305 (same code); the
// owner passed through a named `long owner = session->value10;` as 0x5e3f0 does 363 (the caller's dead owner store,
// `mov edx, [ebx + 0x10]` / `mov [esp + 0x1c], edx`, still does not appear: 0x769a0 already declares the unread owner
// parameter and this body passes it, so the earlier BLOCKED-BY note now reads: the link drops the unread argument).
// M3 lane 1 r127 (fast with 0x5e3f0, MATCH each run; control 305): the peer flags in a block scope that ends before
// `refused:` 305; `bool result;` then `result = false;` 305 (same code); a `member_state` pointer for the acknowledgement's
// six member-state uses 317 (1264 B); the peer flags declared at function scope 305 (the byte homes do not move: they
// follow liveness, not scope); `volatile` result (diagnostic, reverted at once) 319 (1280 B; it rotates the homes again,
// result +9, identity +0xa, a peer flag +0xb). Re-measured: renamed result/better/same/identity flag (Round 87's form) 305
// each, the host path as `if (sender != NONE) goto acknowledge;` 317. Reading: the three byte homes follow the reference
// weights, and ours folds `result` to a constant at the reason-1/2/9 exits where retail reads its home, so the weights and
// the homes' order differ. Body unchanged.
// M3 lane 5 r208 (fast with 0x5e3f0, MATCH each run; kept forms confirmed plain with the 13 callees; control 305,
// difflib-aligned 244). KEPT 305 -> 295 (plain, 1264 B, aligned 90): the tie-break as separate `better = true`
// statements (the count, then the +0x50 field, then the selected index, each its own if; 299 alone, aligned 100: the
// flag bytes take retail's homes, they follow how often each flag is referenced) and `return result;` in place of
// `goto done;` after the disconnect (lane 16's 301 form; 295 with the tie-break). On that: the old mask read through
// `map` 299; the kept-arms exit as `return result;` 307 (1273 B), with the mask read 298.
// LEAD, match-only, 96 (plain, 1264 B, aligned 14: the first 284 and the last 65 lines are retail's;
// build/r208/lead-0x5e7f0-96.patch in the m3-lane5 worktree). Its parts, found one cause at a time on the tie-break
// form: the old mask read through `map` (retail's `[esi + 0xb8]`: the map then stays in ESI over the two count calls and
// the pending-mask update is retail's `or [mem], eax`) 306 (1264 B, aligned 107); `remote` declared ahead of `local`
// 310 (aligned 92) and `map` declared between the two update calls 310 (aligned 90: the received block is retail's from
// the copy to the reply time; the copy ahead of the member pointers rotates the byte homes again, 314); one `return
// result;` with the reasons as an else-if chain and two flags tested after it (`acknowledge`, set at the state-8 and
// host sites, and `proceed`) 348 (1264 B, aligned 52: the acknowledgement block last, reason 1 laid behind the received
// block's first unconditional jump, the three `result = true` exits one block, every exit reading the result byte);
// main's nested range test in that chain 295 (1273 B, aligned 35; as `&&` with an else 348, as two else-if arms 356,
// nested under `state != 9` 348); a named `long owner = session->value10;` ahead of the timeout check, the sibling
// 0x5e3f0's form, 196 (1280 B, aligned 14: retail's dead owner store appears and the scratch rotation behind it agrees,
// so the store never needed a change to 0x769a0, only the local on this graph); the range test held in a named bool
// (`bool invalid = state <= 2 || state > 8;`) 96: one inline reason-2 block reached by `jle` and by fall-through. Flags
// set in a test, a one-use local, a test held in a bool and moved declarations are match-only, so none of that is kept.
// Left at 96, the end of the received block only: retail writes `mov al, 1; mov [result], al`, tests same/better
// through CL and jumps both to a bare epilogue with AL = 1, and after the disconnect call reads the result back, tests
// it and jumps to that same epilogue (`mov al, [esp + 0xb]; test al, al; jne`), falling into a second bare epilogue;
// ours tests through AL, jumps to the shared `mov al, [result]` exit and has no test after the disconnect. That test is
// the only one of its kind in the image (a byte scan finds no other flag read back, tested and returned on both arms).
// Tried for it on the 96 form: an empty `if (!result) {}` or `if (result) {}` ahead of the return 196/196 on the 196
// form (same code); the acknowledgement under `if (!result && acknowledge)` or nested under `if (!result)` 196/196
// (same code); `if (result) return result;` ahead of the acknowledgement 208 (any extra reference to `result` moves its
// byte down); `if (result) result = true;` after the block 229; a dead flag store under `if (!result)` or `if (result)`
// 196/196 (same code); direct returns in the block: kept-arms and tail `return result;` 113, kept-arms `return true;`
// 100, tail `return result;` alone 109, `if (!result) return false;` after the disconnect 48 (retail's read, test and
// `jne`, but a stray `xor al, al`, the kept-arms still through AL and the byte homes rotated: register-masked 23), `if
// (!result) return result;` 48 (same code: the false edge folds it to zero), `if (result) return result;` 119 (the
// test folds away), with the acknowledgement under `!result` 119, `if (result) return true;` 56 (a `mov al, 1` arm),
// `if (result) goto` a label at the final return 96 (same code), the kept-arms jumping to the failure return's test
// 48; `if (!result) result = false;` 114 (the store stays); `__assume(result || *reason != 0)` at the end, after the
// disconnect or ahead of the acknowledgement test, and `__assume(result || *reason == 7)` 96 each (same code); the
// assumption nested as `if (!result) __assume(*reason != 0);` ahead of the return, and as `if (!result)
// __assume(*reason == 7);` after the disconnect, with the kept-arms jumping past it or to it, 96 each (same code);
// diagnostic, restored at once: an inlined empty function under the result test with one inlining level allowed 434
// (no test left). Reading: both arms of retail's test return the byte just read, with nothing folded and nothing
// merged, which no statement form tried here leaves behind. The source permuter on the 96 form (52 tries, 602 s): best 42, from swapped call arguments and a
// volatile store, discarded; its plain parts alone (`better != 0`, the state read into a local, the two NONE stores
// swapped, the owner declared then assigned) 96 each. Re-measured on the control: every exit through `done:` 320
// (1280 B), with the tie-break 313.
// M3 lane 3 r275 (fast with 0x5e3f0/0x769a0/0x58d90/0x58d20 MATCH): round 208's 96 lead rebuilt on the current body
// (96, difflib-aligned 8). On it: `if (result) return result;` after the disconnect 119 (aligned 30, as before); `if
// (result) goto received_done; return result;` there 48 (retail's length, but aligned 23: the read, test and `jne`
// appear, the fall-through returns a folded `xor al, al` and the byte homes rotate, result +9); that with `(void)&result;`
// 48 (same code: the address-taking is dropped); `if (!result) goto` a second `return result;` after the acknowledgement
// 48 (same code); the result cleared by `memset(&result, 0, sizeof(result))` on that form 48 (same code: scalar-
// replaced). Reading: retail's fall-through returns the byte it just tested without folding it to zero, and every
// exit reads the byte back, which is what an escaped (address-taken) result gives; nothing in this inline_depth(0)
// region takes its address.
// M3 lane 14 r123 (fast with 0x5e3f0, MATCH): round 208's lead body re-measured 96 on current main (1264 B, first difference
// +0x94). On it: the acknowledgement under `if (!result && acknowledge)` 96 (same code: the proceed path threads the whole test
// away); the final return as `result ? true : false` 410 (1310 B), `result != false` 396 (1313 B), `result == true` 406
// (1323 B); the kept arms `return result;` with the acknowledgement under `!result && acknowledge` 100 (the kept arms then
// jump to their own `mov al, 1` return). Reading: retail's return at 0x5e891 (the state-8 exit's epilogue) gathers every path
// on which the result is true (state 8, the kept arms with AL = 1, the post-disconnect `jne`) and the false edge has its own
// epilogue, as if the final return were threaded on the result by a test whose other part is unknown on the proceed path.
// Direct reason 3/5/8 returns 345/301/297, both timeout returns 299, all five policy returns 362.
// Shared range rejection or admission 370/370, shared rejection with five direct returns 387.
// The result byte at +0xb holds, but the first branch displacement and the return layout still differ.
// M3 lane 11 r133 (fast with 0x5e3f0, MATCH each run; body unchanged at 295). Round 208's 96 body re-measured 96 (1264 B,
// aligned 20: only the end of the received block differs). Reading of retail's end: the refused path (result still false)
// and the accepted path with neither kept arm (result just set) meet at the disconnect, so the read, test and `jne` after
// it is a real test of the result on merged paths; its true edge and the two kept arms all enter the state-8 exit's
// epilogue behind its load with AL already holding the result (`mov al, 1; mov [result], al` ahead of the kept-arm tests,
// which therefore go through CL), and its false edge returns the byte just read without `xor al, al`, so the false edge's
// return is not a return that only that edge reaches. The acknowledgement block's two entries four bytes apart (one
// reloads the sender) and this file's matched 0x5e3f0 (a `take` flag set in a chain and tested after it) fit the flag
// form. Tried on the 96 body: the kept arms as `if (same) return result; if (better) return result;` with `if (result)
// return result;` after the disconnect 150; the kept arms as one `return result;` with that test 123 (the kept arms get a
// `mov al, 1` block of their own and the test folds away); `if (result) goto received_done; return result;` after the
// disconnect 48 (retail's read, test and `jne`, but `xor al, al` on the false edge, the kept arms still through AL and
// the result byte at +9: each added reference to the result moves its home); that with the kept arms returning 113; the
// block as `for (; !result && proceed; proceed = false)` 389 or `while (!result && proceed)` 392 (1230 B: the layout
// goes); the acknowledgement under `if (!result && acknowledge)` 96 (same code); `if (result) return result;` ahead of
// the acknowledgement test 119; the block as an inline helper returning its own flag, called under `if` or assigned 908
// to 921 (this file's inline depth of 0 keeps it out of line); a second flag `success` inside the block with `if
// (success) result = true;` after it 239 (1273 B: the test is there but the two flags take two homes), `result =
// success;` 227, `return success;` 243; the false edge and the reason-9 arm sharing a `return result;` inside the block
// with the true edge jumping to a label after the block or at the final return 119 each (the two returns merge and the
// test goes); `if (!result) goto` a second final `return result;` 48 (the 48 form again).
// M3 lane 4 r234 (fast with 0x5e3f0, MATCH; main's body 295, unchanged): round 208's 96 lead rebuilt (96, first
// difference +0x94). On it, after the disconnect: `bool const &accepted = result; if (accepted) goto received_done; return
// result;` 48 (same code as the plain goto form: the false edge still folds to `xor al, al`, result byte +9); `if (result
// == true) return result;` 102 (a `cmp byte, 1` and a `mov al, 1` arm); that with the kept arms returning `result` 114.
// Diagnostics, reverted at once: every `result = true` through a forced-inline setter taking the result's address 338
// (1273 B: the region's inline depth of 0 keeps it out of line); through a pointer local to the result 48 (the pointer
// folds). Reading: retail's kept arms, the post-disconnect true edge and its false edge each act as a `return result;` of
// their own in one extended block (the store's AL or the test's load reused, then the epilogues cross-jumped); any test
// of `result` here lets our IL fold the false edge to zero.
// M3 lane 14 r129 (fast with 0x5e3f0, MATCH each run; body unchanged at 295). Round 208's 96 body re-applied 96. Retail's
// state-8 `result = true` block (store, reload, epilogue) is also the target of the channel-7 exit and of the
// acknowledgement block's end when the state is not 7; the kept arms and the post-disconnect test enter its epilogue
// behind the reload. On the 96 body: the result declared `volatile` (diagnostic) 96 (the same code: our result is already
// re-read at every exit); the kept arms as `return true;` 100 or `return result;` 100 (both a `mov al, 1` return block of
// their own at the end); `result = true` set with `acknowledge` at the two sender sites and the acknowledgement under
// `if (result && acknowledge)` 325 (retail's acknowledgement block does end in `result = true`); after the disconnect,
// a test through `bool const &kept = result;` as `if (kept) goto received_done; return result;`, `if (!kept) return
// result;` or `if (!kept) return kept;` 48 each (retail's read, test and `jne`, but `xor al, al` on the false edge and
// the result byte at +9); the same two tests through `bool *const result_address = &result;` 48 each (the address-taking
// folds away).
// MiniMax G (11-20): no new form, no lower score (295): the exits still sit at other offsets from the first
// branch on, so every later branch differs as well, the invalid-type exit still returns the constant where retail
// reads the result byte back, and the acknowledgement block is still placed before retail's: the byte homes of the
// result, the identity and the better flag are not yet recovered.
// M3 lane 3 r286 (single-address fast, control 295): the peer's reply time through the existing 0x75870 call 310
// (1240 B: called out of line, where retail expands it at this one site); that time read as an if/else store 295 (same
// code); the reason-9 test written `sender == session->current_member || sender == NONE` 295 (same code); the newer-version
// arm as `better = selected != NONE;` 314 (1241 B); the two bit counts taken new first 295 (same code). All restored.
// Four-way switch with shared reason-1 and reason-2 exits, varying inactive, state-nine, invalid and
// active case order, scored 366/366/366/366/366/367/366/366. Retail still uses a comparison chain and reloads
// the acceptance byte on the rejection paths. NOT kept: the dispatch table and earlier first difference.
// M3 lane 1 r144 (single-address fast; main's body 295, round 208's 96 lead rebuilt from its patch 96). LEAD,
// match-only, 44 (1264 B, first difference +0x94 as the 96 form; diff in build/r144/lead-0x5e7f0-44.diff of the
// m3-lane1 worktree): on the 96 form, the kept arms `goto accept;` (after their `result = true;`), `if (result) goto
// accept;` after the disconnect, the acknowledgement block ending in `goto accept;` in place of its `result = true;`,
// and `accept: result = true; return result;` after the final return. Left: our accept is folded to `mov al, 1` in
// its own epilogue, the post-disconnect false edge jumps to an earlier epilogue, and the kept arms store an immediate
// where retail stores AL = 1 and enters the state-8 epilogue behind its reload. On the 96 form: after the disconnect
// `if (result) return true; return false;` 110, `return result ? true : false;` 110, `return result != false;` 51;
// the accept label reached by the kept arms and the post-disconnect test only, its own return 56; the acknowledgement
// under `if (!result && acknowledge)` 96 (same code); the accept label ahead of a shared `done: return result;` with
// the acknowledgement skipped by `if (!acknowledge) goto done;` 108 (1268 B: result's byte moves to +9, and the post-
// disconnect true edge gets its own store/reload copy); that without the post-disconnect test 96; the accept label at
// the acknowledgement block's own `result = true;` 108 (same two causes). On main's body: a shared `accept:` exit
// for the state-8, channel-7, kept-arm, post-disconnect and acknowledgement exits 303 (1276 B: the state-8 store and
// reload are then pulled up behind the state-8 test as in retail, but the reason exits fold to `xor al, al`), with
// every exit through `done:` 309 (1284 B), with the post-disconnect return kept 318. Diagnostic (reverted at once):
// the result declared volatile on the 44 and 108 forms 114/108 (its byte moves; a memory-resident result alone is
// not retail's shape).
// On the 44 form: the kept arms `return true;` or `return result;` in place of the jump 44/44, as two tests each
// jumping 44, the accept block as `return true;` 44 (all the same code: the true return is folded to `mov al, 1` and
// not merged into the state-8 epilogue); `if (result) return result;` after the disconnect 122 (1244 B).
// M3 lane 2 r125 (H2M_FAST with 0x5e3f0, MATCH each run; main's body 295, unchanged; round 208's 96 lead rebuilt from its
// patch, 96). On the 96 lead, after the disconnect: `if (result) goto accepted;` with the label on the acknowledgement's
// final `result = true;` 107 (1264 B: retail's read, test, `jne` and bare false-edge epilogue appear with no `xor al, al`,
// but the result byte moves to +9 and the better flag to +0xb); with the kept arms also jumping there 108 (1268 B); LEAD,
// match-only, 94 (1264 B, build/r125/lead-0x5e7f0-94.patch): that test with the state-8 `result = true` written as `goto
// accepted` (one result store fewer; the byte homes return to retail's), the same with the channel-7 store also a jump
// 94, only the channel-7 store a jump 94. Left on it: the kept arms store the result as an immediate and jump to the
// shared store-and-load block, where retail stores it through AL (`mov al, 1`) and enters the bare epilogue, and the
// shared `result = true` block sits at the end instead of after the state-8 test. On the 94 lead: the kept arms jumping
// to `accepted` with no store ahead of them 246 (1276 B), with the store kept ahead 95 (1268 B: the post-disconnect test
// turns into `je`); the kept arms `return true;` or `return result;` 103/103 (1270 B, a return block of their own).
// M3 lane 4 r242 (H2M_FAST with 0x5e3f0, MATCH each run; main's body 295, unchanged; lane 1 r144's 44 lead rebuilt, 44).
// Reading of retail on the 44 lead: retail's state-8 block (store 1, reload, epilogue) is the shared accept exit: the
// acknowledgement's state-not-7 edge jumps to its start, and the kept arms and the post-disconnect true edge jump past
// its reload with AL already 1 (the kept arms test `same`/`better` in CL and store `result` through AL), so on those
// edges the return value is the constant; ours folds the accept exit to `mov al, 1` in an epilogue of its own. On the
// 44 lead: the kept arms `goto done` with `done:` on the shared `return result;` 44 (same code), with the
// post-disconnect test also `goto done` 105 (1248 B), with the acknowledgement ending `result = true;` and falling
// into `done` as well 96; the accept exit written inside the state-8 arm (`else { accept: result = true; return
// result; }`) 328 (1247 B: VC still lays it out at the end); the kept arms `return result;` with the acknowledgement
// still `goto accept` 44 (same code); on the 96 form the kept arms `return result;` 100, as `if (same) return true;
// if (better) return true;` 129.
// M3 lane 15 r126 (h2m fast; on m3 lane 5 r208's structured lead, 96 there with only the end of the received block
// left): after the disconnect, an own `return result;` 109; `if (result) return true; return false;` 110; `return result ?
// true : false;` 110; `if (result) return result; return false;` 109; `if (!result) return false; return true;` 110 (each
// moves the first difference to +0x26; none gives retail's `mov al, [result]; test al, al; jne <bare epilogue>`). Body
// unchanged; the stacked score header lines above merged into this one note.
// M3 lane 8 r218 (H2M_FAST with 0x5e3f0, MATCH each run; main's body 295, unchanged). Lane 1 r144's 44 lead rebuilt
// from its diff, 44 (1264 B, +0x94). Reading: retail's kept arms store the result through AL (`mov al, 1` then the byte
// store) and jump past the state-8 block's reload into its bare epilogue, so the result is a register value with a stack
// home there, not a folded constant. On the 44 lead: the kept arms `goto done;` with the label on the final `return
// result;` 44 (the arms then reach a reload block, the store stays an immediate); with the post-disconnect test also
// jumping there 105 (1248 B); that with the acknowledgement block ending in `result = true;` and no accept label 96; the
// three flags declared ahead of the sender lookup 50 (+0x10); the kept arms and the post-disconnect test as `return
// result;` 122 (1244 B).
// M3 lane 14 r141 (H2M_FAST with 0x5e3f0, MATCH each run; main's body 295, unchanged; lane 1 r144's 44 lead rebuilt, 44,
// aligned 21). On the 44 lead: the acknowledgement ending `result = true;` and falling into the final return 56 (+0x26); the
// kept arms `goto received_done;` 99 (aligned 21); that without the post-disconnect test 105 (1248 B, aligned 11); with the
// acknowledgement also falling into the final return (no accept block) 96 (1264 B, aligned 17); the post-disconnect `if
// (result) goto accepted;` with the label on the acknowledgement's final `result = true;` 107 (+0x26, the byte homes rotate);
// that with the state-8 `result = true` also written `goto accepted;` 94 (1264 B, aligned 11: one block left, retail's
// kept arms store the result through AL, test `same`/`better` in CL and jump into the state-8 copy's bare epilogue, and its
// post-disconnect true edge does the same; ours stores an immediate, tests in AL and jumps to the acknowledgement's copy;
// build/r141/lead-5e7f0-g7-94.patch in m3-lane14). On that 94 form: the kept arms `return result;` 103 (1270 B: a `mov al,
// 1` return block of their own), `goto accepted;` 95 (1268 B, aligned 9); the accepted label inside the state-8 arm with the
// acknowledgement jumping back to it 403 (frame 0xe0: the backward jump stops the flag threading). Also on the 94 form:
// after the disconnect `if (result) return result;` 113 (+0x26: one more reference moves the result byte to +9), that with
// the kept arms `return result;` 117, with the kept arms `goto accepted;` 113; that test with the channel-7 `result = true`
// written `goto accepted;` 100 (homes back, but the test folds into the final return); the channel-7 jump alone 94 (same
// code); the kept arms `return true;` 103 (1270 B), as two tests each `return true;` 131 (1269 B), as `if (same || better)
// return result = true;` ahead of the plain store 237 (frame 0xe0).
// M3 lane 16 r180 (H2M_FAST with 0x5e3f0, MATCH each run; main 295, aligned 149). Lane 1 r144's 44 lead rebuilt from its
// L9 body, 44 (aligned 21). On it: the result as the element of a one-element array bound by reference 44 (same code:
// scalar-replaced); diagnostic, reverted at once: the result's address held in a volatile pointer local 359 (every exit
// then reads the result byte back as retail does, but the homes rotate (result +9) and the kept arms and the accept block
// are unchanged, so an escaped result is not what retail has). On main's body: the result declared ahead of the state
// read 295 (same code); the identity compare moved down to the timeout test 384 (1231 B); the timed-out arm with one
// identity copy and `*reason = identity_equal ? 4 : 6` 323 (1229 B, as lane 3 r242 found); the acknowledgement block
// through a named member-state pointer 309 (1246 B). Body unchanged.
// M3 lane 5 r239 (H2M_FAST with 0x5e3f0, MATCH each run; main 295; lane 1 r144's 44 lead rebuilt from its diff, 44). On the
// 44 lead: the kept arms `result = true; if (same || better) return true;` 44 (same code). On main: the kept arms `return
// true;` 307 (1273 B); the state-8 exit `return true;` 339 (1266 B); the refused block written at its test, no label, 295
// (same code); every exit through `done:` 313 (1280 B), with the post-disconnect `if (result) goto done; return result;`
// 328 (1286 B). Reading: retail's final return is a join that holds the result in AL (the state-8 edge stores 1 and
// reloads it, the kept arms and the post-disconnect true edge arrive with AL set, the reason exits reload the byte).
// Body unchanged.
// M3 lane 8 r233 (h2m fast with 0x5e3f0, MATCH each run; main 295). Both builds keep the result byte in its home from
// entry; ours rematerialises the false at the reason exits where retail reads the home back, the behaviour of a local
// whose constant is not propagated. On main: the result cleared by `memset(&result, 0, sizeof(result))` 321 (+0x1b,
// scalar-replaced); the acknowledgement block moved after the final `done:` return 295 (same code); the sibling
// 0x5e3f0's `long owner = session->value10;` ahead of the timeout call 360, with its `__assume(host_state != NULL)`
// 360; the session state read at each test with no state local 299 (+0x16). Diagnostic (compiler setting, reverted):
// global optimisation off around the body 809 (1643 B), so retail's reloads are not a whole-function setting. Body
// unchanged.
// M3 lane 1 r165 (fast screen with 0x5e3f0, MATCH each run; main 295; lane 1 r144's 44 lead rebuilt from its diff, 44; the
// single-return form under it rebuilt from the lead, 96). Aligned against retail (jump targets masked) the 96 form
// differs in two places only: the kept arms (retail `mov cl, [same]; test cl, cl; mov al, 1; mov [result], al; jne <bare
// epilogue>`, then `better` in CL the same way: the result is a register value there, stored to its home through AL;
// ours tests in AL and stores an immediate) and the two instructions after the disconnect (retail `test al, al; jne <bare
// epilogue>` on the reloaded result, then an epilogue of its own with no reload and no `xor al, al`). That epilogue sits
// inside the received block, ahead of the reason-9 arm. No forced allocator decision on the result (its cost, rank,
// register or any of its 41 range cuts, 53 tried) changes the 44 lead, so both are shapes of the code, not picks. New
// on the 96 form: the acknowledgement nested as `if (!result) if (acknowledge)` 96 (same code, as the `&&` form); after
// the disconnect `if (!result) return result;` 48 (aligned 26: retail's test and `jne` appear and the failed edge gets
// its own epilogue in retail's place, but with an `xor al, al`, the result byte moves to +9 and the kept arms are
// unchanged), `if (!result) return false;` 48 and `if (result) goto received_done; return result;` 48 (same code each),
// that test placed after the `received_done` label so the kept arms reach it 48 (same code: the kept edges are threaded
// past it), `if (result) return result;` 119 (the two returns merge and the homes rotate), also behind the label 119;
// `return result != false;` with the kept arms `return true;` 104; the received block with no goto (a refused flag, the
// accepted and refused arms as if/else, the disconnect under `!same && !better`) 219 (1230 B: the refused arm moves to
// the end), that with the disconnect under `!result || (!same && !better)` 219, either with the `!result &&` tail 219
// (same code each). Re-measured: the kept arms `return true;` or `return result;` on the 96 form 100 (a `mov al, 1`
// return block of their own: the constant is not held in AL across the tests). Body unchanged.
// Initial refusal code before the active gate, conditional/explicit 383/383; after state-nine bypass 386/386;
// active range first 393/376; admitted range through state nine 394/381.
// NOT kept: the common refusal shortens the body and does not recover both reason-2 edges or the acceptance reload.
// M3 lane 2 r148 (fast screen with 0x5e3f0, 0x58d90, 0x58d20, 0x769a0, 0x62ca0 and 0x75870, all MATCH each run; main 295,
// aligned 90, 75 with registers and offsets masked; lane 1 r144's 44 lead rebuilt from lane 1 r165's copy, 44, aligned 18
// [14]; body unchanged). Reading of retail's exits: the function has one return of the result byte; the state-8 arm's
// `result = true` is followed by a copy of it (store, reload, epilogue at +0x98), the channel-7 arm and the acknowledgement's
// not-state-7 edge are merged into that same store, and the kept arms and the post-disconnect true edge enter the same
// copy behind its reload with the result already in AL (the kept arms load 1 into AL, store the byte from AL and test
// same and better through CL). So every accepting exit ends in the block that follows the state-8 test, and the result is
// a register value from its store to those two exits. On the 44 lead: the state-8 arm `goto accept;` 337 (+0x2a: the byte
// homes rotate, aligned 38), the channel-7 arm `goto accept;` 49, both 339 (1247 B). The 96 form rebuilt from the lead (no
// accept block, the acknowledgement ending in `result = true;`, the kept arms and the post-disconnect test jumping to a
// label on the final return) 96, aligned 14 [10]: only the kept arms (an immediate store, tests through AL, the jump to
// the reload) and retail's `test al, al; jne` after the disconnect differ. On that 96 form: the result stored ahead of the
// update arms 280 (1208 B, first difference +0x8); the post-disconnect test as `if (!result) goto received_done; goto
// done;` 96, no test after the disconnect 96 and the kept arms as two tests 96 (the same code each). Forced decisions on
// the result's 20 range cuts on the 96 form (a diagnostic, not source): none changes the code, as lane 1 r165 found on
// the 44 lead; the result is
// never a register candidate in this build (its cost is below zero in every pass), so the kept arms need a form that gives
// the result a use at each of the two exits, not a pick.
// M3 lane GA r8 (fast with 0x5e3f0, 0x58d90, 0x58d20, 0x769a0, 0x62ca0 and 0x75870, all MATCH each run; control 295, body
// unchanged). On main's 295 body: all stored map fields through `map` 311 (1240 B, aligned 137), that with kept arms returning
// `result` directly 310 (1245 B), `memcpy` ahead of `remote`/`local` 303 (+0x22), zero-goto `valid_version` bool inside the
// received block 315 (1264 B, aligned 221), single-return `goto done` without `acknowledge`/`proceed` flags 367 (with named
// `owner` 360, 1264 B). On r208's 96 single-return lead (exact 96 rebuilt with `owner` inside the timeout arm, aligned 17 [13];
// 129 with `owner` hoisted before `function_058d20`): `bool disconnect` with `if (false) { refused: ... }` 129 (same code as
// 129 base), `do { ... } while (false)` with `break` on kept and `if (result) break;` 129 (same code), `switch (result)` after
// disconnect 149 (1275 B), all true paths returning directly with `if (result) return result; return result;` 344 (1215 B),
// state-8 and kept returning directly 327/337, `if (result) goto done; else return result;` after disconnect 80 (r208's 48 form
// with hoisted `owner`); on the exact 96 lead, end-of-function result tests after `if (acknowledge)`: `if (result) return result;
// return result;` 364 (1242 B), `if (!result) return result; return result;` 356 (1247 B), `if (result) return true; return
// result;` 377 (1225 B), `if (result) goto ok; return result; ok: return result;` 364 (1242 B). Body unchanged at 295.
// M3 lane 8 r242 (fast screen with 0x5e3f0, MATCH each run; main 295; lane 1 r144's 44 lead rebuilt, 44). On the 44 lead:
// the accept block as `return result = true;` 44 (same code); the kept arms as `if (same || better) return result =
// true;` ahead of the plain store 244 (1248 B); the kept test inverted, `if (!same && !better) goto disconnected;` with
// the arm falling to the accept jump, 44 (same code); the acknowledgement ending `result = true;` into the final return
// with the accept block `return true;` 56 (+0x26); after the disconnect `if (!result) return result; goto accept;` 44
// (same code). Retail's kept arms still need the result stored from AL with the return value already in AL. Body
// unchanged.
// M3 lane 16 r195 (H2M fast with 0x5e3f0, MATCH each run; main 295; round 208's 96 form and lane 1 r144's 44 form
// rebuilt, 96 and 44). Reading of the 96 form: ours keeps two copies of the `result = true` store-and-reload exit (one at
// the state-8 position, one at the end shared by the channel-7, acknowledgement and kept edges) where retail keeps only
// the state-8 copy and every other accepting edge jumps into it, the kept arms past its reload with AL already 1; the
// result byte is never a register candidate on that form (priority -58, its tail piece -7). On main: the peer reply time
// through the file's existing time helper 309 (1240 B: the inline-depth region keeps it a call); diagnostic, restored at
// once: the body outside the file's inline-depth region 419 (1309 B). On the 96 form: an `exit:` label after the state-8
// arm's `result = true` (its own `return result;`), reached by the kept arms and by `if (result)` after the disconnect,
// 345 (1244 B: that return folds to `mov al, 1`); the kept arms `return true;` with `if (result) return true;` after the
// disconnect 56 (result byte +9, better +0xb); the range test as two arms (`state <= 2`, `state > 8`) 368 (1276 B); the
// reason-1 test as two arms (`!state`, `state == 10`) 357, on the 44 form 355. Body unchanged.
// @retail 0x5e7f0
bool __stdcall function_05e7f0(c_class_58d20 *session, const s_type_99af70 *address, const void *message,
 long *reason, bool *has_identity, s_session_member_identity *identity)
{
 long sender = network_session_find_member_by_address(session, address);
 long state = session->state;
 bool result = false;
 if (!state || state == 10)
 {
  *reason = 1;
  return result;
 }
 if (state != 9)
 {
  if (state > 2)
  {
   if (state > 8)
   {
    *reason = 2;
    return result;
   }
   else
   {
    if (function_058d90(session))
    {
     if (!session->function_058d20())
     {
      *reason = 3;
      goto done;
     }
    }
    if (state == 8)
    {
     if (sender != NONE)
      goto acknowledge;
     result = true;
     goto done;
    }
    long host = session->member_index;
    const s_session_member_identity *host_identity = (const s_session_member_identity *)&session->members[host];
    s_network_session_member_state *host_state = &session->member_states[host];
    bool identity_equal = memcmp(host_identity, (const byte *)message + 8, 0x24) == 0;
    if (session->function_058d20())
    {
     if (sender == NONE)
     {
      *reason = 5;
      *has_identity = true;
      *identity = *host_identity;
      goto done;
     }
     goto acknowledge;
    }
    if (network_observer_channel_timed_out(session->observer, session->value10, host_state->unknown04))
    {
     if (identity_equal)
     {
      *reason = 4;
      *has_identity = true;
      *identity = *host_identity;
      goto done;
     }
     *reason = 6;
     *has_identity = true;
     *identity = *host_identity;
     goto done;
    }
    if (session->value4c == NONE)
    {
     *reason = 8;
     goto done;
    }
    if (session->observer->channels[host_state->unknown04].state == 7)
    {
     if (!identity_equal)
     {
      result = true;
      goto done;
     }
    }
   }
  }
  else
  {
   *reason = 2;
   return result;
  }
 }
 if (session->state != 9)
  network_session_enter_state_9(session);
 if (sender == NONE || sender == session->current_member)
 {
  *reason = 9;
  return result;
 }
 {
  byte received[0xc0];
  s_session_member_header *local = (s_session_member_header *)&session->members[session->current_member];
  s_session_member_header *remote = (s_session_member_header *)&session->members[sender];
  memcpy(received, (const byte *)message + 8, sizeof(received));
  s_session_peer_transition *transition = (s_session_peer_transition *)&session->value7420;
  s_session_peer_map *map = (s_session_peer_map *)transition->map;
  bool better = false;
  bool same = false;
  session_peer_map_update_reachable(local, (s_session_peer_map *)received, remote);
  session_peer_map_update_reachable(local, map, remote);
  long selected = network_session_find_member(session, (const s_session_member_identity *)(received + 0x24));
  long version = *(long *)(received + 0x48);
  long old_version = *(long *)(transition->map + 0x48);
  if (version > old_version)
  {
   if (selected != NONE)
    better = true;
  }
  else if (version == old_version)
  {
   if (selected == NONE || selected != *(long *)(received + 0x4c))
    goto refused;
   if (selected == *(long *)(transition->map + 0x4c))
    same = true;
   else
   {
    long old_count = count_bits(*(dword *)(transition->map + 0xb8));
    long new_count = count_bits(*(dword *)(received + 0xb8));
    if (new_count > old_count)
     better = true;
    else if (new_count == old_count)
    {
     if (*(long *)(received + 0x50) > *(long *)(transition->map + 0x50))
      better = true;
     else if (*(long *)(received + 0x50) == *(long *)(transition->map + 0x50) &&
      *(long *)(received + 0x4c) < *(long *)(transition->map + 0x4c))
      better = true;
    }
   }
  }
  {
   dword bit = 1 << sender;
   transition->pending |= bit;
   transition->failed &= ~bit;
   transition->times[sender] = g_510548 ? g_51054c : GetTickCount();
   if (same)
   {
    if (session_peer_map_set_connected(map, remote, true))
     transition->selection_time = function_75870();
   }
   else if (better)
   {
    memcpy(transition->map, received, sizeof(received));
    transition->selected = selected;
    transition->selection_time = function_75870();
    session_peer_map_set_connected(map, local, true);
   }
   result = true;
   if (same || better)
    goto done;
  }
  goto disconnected;
 refused:
  {
   dword bit = 1 << sender;
   transition->pending &= ~bit;
   transition->failed |= bit;
   *reason = 7;
  }
 disconnected:
  if (session_peer_map_set_connected(map, remote, false))
   transition->selection_time = function_75870();
  return result;
 }
acknowledge:
 {
  s_session_id id = *(s_session_id *)&session->unknown1c;
  if (session->member_states[sender].flag1)
  {
   if (session->observer->channels[session->member_states[sender].unknown04].state == 7)
    network_observer_send_message(session->observer, session->value10, session->member_states[sender].unknown04, false, 18, sizeof(id), &id);
   network_observer_send_message(session->observer, session->value10, session->member_states[sender].unknown04, true, 18, sizeof(id), &id);
  }
  session->member_states[sender].unknown08 = NONE;
  session->member_states[sender].unknown0c = NONE;
  session->member_states[sender].flag3 = true;
  if (session->state == 7)
   network_session_clear_peer(session, sender);
  result = true;
  return result;
 }
done:
 return result;
}
#pragma inline_depth()


// @retail 0x5d9e0
bool __stdcall function_05d9e0(c_class_58d20 *session, const void *message_)
{
 const byte *message = (const byte *)message_;
 byte parameters[0x14b0];
 bool result = function_5d5a0(session, message, parameters);
 if (result)
 {
  memcpy(&session->update_count, parameters, sizeof(parameters));
  if (message[0x10] && message[0x11])
  {
   long reply[4];
   memset(reply, 0, sizeof(reply));
   reply[0] = session->unknown1c;
   reply[1] = session->unknown20;
   reply[2] = session->type;
   reply[3] = session->value497c;
   network_session_send_to_member(session, session->member_index, 0, 0x24, sizeof(reply), reply);
  }
 }
 else
  network_session_host_lost(session);
 return result;
}


void __stdcall function_60400(c_class_58d20 *session, const byte *current, const byte *previous, byte *update);

// Earlier: missing owner spill leaves frame 0x9148 versus 0x914c (BLOCKED-BY 0x76930:
// retail passes the owner to the mark-message call as an unused argument; its file is held);
// mask/counter role names 122, first-loop cursor 122, eligibility guard 122,
// member bit live across readiness 159, named readiness result 122.
// Round 50: early continues for the eligibility tests 122; sequence tests swapped 121;
// channel/observer/sequence read before the tests 136; ready test negated with the mark
// call in the else arm 119; empty arm for the state-8 test 122.
// Round 403: live sequence 138; live channel 137; readonly state 122; default buffer 135; default
// live sequence 138; default live channel 150; readonly live sequence 138; readonly live channel 137.
// Opening body restored.
// Round 299: member-entry channel/observer/both/sequence/channel-sequence/observer-sequence
// snapshots 138/110/137/137/134/137; native unsigned sequence 122; all snapshots
// with unsigned sequence 136. Uniform observer snapshot 110 plain, 417/416 bytes, kept.
// Round 30: second-loop member-state pointer at the body's top 123 (416 bytes, retail's flag-byte walk);
// masks declared delta first 111; observer read after the eligibility tests 122; with the pointer 123;
// with the mask order 123. Opening body restored.
// Round 32 (scratch copy; src/unknown_075870.cpp is held): the owner store is a spill of the owner live across
// the readiness call for the mark call, left behind when LTCG deletes both unused arguments. Inline wrapper
// with an unused owner (by value / const reference) 110/110; unused owner on 0x768b0 only (by value / const
// reference) 110/110; on 0x768b0 and 0x76930 108; with a two-use `owner` local 108; with that local, the
// observer read after the eligibility tests, the second loop's member-state pointer and the masks declared
// delta first: MATCH. Applied in the tree (coordinator, round 32): MATCH, 0 lost.
// @retail 0x62640
void function_62640(c_class_58d20 *session)
{
 dword delta_mask = 0;
 dword full_mask = 0;
 byte delta[0x489c];
 byte full[0x489c];
 for (long i = 0; i < session->member_count; i++)
 {
  s_network_session_member_state *state = &session->member_states[i];
  if (state->flag1 && !state->flag3 && session->observer->channels[state->unknown04].state == 7 &&
   state->unknown08 != session->value4c)
  {
   long channel = state->unknown04;
   s_network_observer *observer = session->observer;
   long sequence = state->unknown08;
   long owner = session->value10;
   if (network_observer_channel_ready(observer, owner, channel, 0x19))
    network_observer_mark_message(observer, owner, channel, 0x19);
   else if (session->state != 8)
   {
    if (sequence != NONE && sequence == session->value24e0)
     delta_mask |= 1 << i;
    else
     full_mask |= 1 << i;
   }
  }
 }
 if (full_mask)
  function_60400(session, (const byte *)&session->value4c, 0, full);
 if (delta_mask)
  function_60400(session, (const byte *)&session->value4c, (const byte *)&session->value24e0, delta);
 for (long j = 0; j < session->member_count; j++)
 {
  s_network_session_member_state *state = &session->member_states[j];
  byte *message;
  if (delta_mask & (1 << j))
   message = delta;
  else if (full_mask & (1 << j))
   message = full;
  else
   continue;
  state->unknown08 = session->value4c;
  network_session_send_to_member(session, j, 0, 0x19, 0x489c, message);
 }
 memcpy(&session->value24e0, &session->value4c, 0x2494);
}

// Earlier (98): the two masks are cleared in the other order and live in
// the other stack slots, the frame is four bytes shorter, the bit is shifted
// in the other register and merged in a register instead of in memory, and the
// second loop walks the member states from the flag byte
// Read-only first-loop member / unsigned sequence / immutable observer binding,
// all seven subsets 98; signed-int cursors in both ordinary loops 98. Original
// metadata domains and body restored; owner spill, mask homes and send base remain.
// Round 422: current payload base 105; previous payload base 142; both payload bases 148; opaque
// message 98; current base/opaque message 105; previous base/opaque message 142; both
// bases/opaque message 148; both bases/current at encoding 134. Opening body restored.
// the frame is four bytes shorter: retail loads the session's
// owner before the readiness call and spills it (BLOCKED-BY 0x76930, the unused owner
// argument of the mark-message call, as for 0x62640); then the two masks are cleared
// in the other order and live in the other stack slots, the bit is shifted in the other
// register and merged in a register instead of in memory, and the second loop walks the
// member states from the flag byte.
// Tried: read-only first-loop member / unsigned sequence / immutable observer binding,
// all seven subsets 98; signed-int cursors in both ordinary loops 98; the second loop's
// member-state pointer at the top of its body 118 (retail's flag-byte walk, one
// instruction shorter); that pointer after the `continue`s 98 (same code); a
// NULL-initialised message with one `continue` 98 (same code); masks declared delta
// first, buffers swapped, renamed masks and counter (same slots); arms inverted (same
// code); compared operands swapped and the sequence read first (one-file harness worse).
// Round 30: observer read once before the eligibility test 128; long masks 98; one bit local for both
// arms 133 (407 bytes); readiness test negated with the mark call in the else arm 95 (aligned worse, the
// arms move); equality-first sequence compare 97; negated test with it 95, with long masks 95.
// Round 32 (scratch copy; src/unknown_075870.cpp is held): unused owner on 0x768b0 only (by value / const
// reference) 98/98; on 0x768b0 and 0x76930 35 (frame 0x29c4, but the field's address spilled); with a
// two-use `owner` local 34 (the owner store exact); plus the second loop's member-state pointer 2 (only the
// order of the two mask clears); plus the masks declared delta first: MATCH. Applied in the tree
// (coordinator, round 32): MATCH, 0 lost.
// @retail 0x627e0
void function_627e0(c_class_58d20 *session)
{
 dword delta_mask = 0;
 dword full_mask = 0;
 byte delta[0x14d8];
 byte full[0x14d8];
 for (long i = 0; i < session->member_count; i++)
 {
  s_network_session_member_state *state = &session->member_states[i];
  if (state->flag1 && !state->flag3 && session->observer->channels[state->unknown04].state == 7 &&
   state->unknown0c != session->update_count)
  {
   long channel = state->unknown04;
   s_network_observer *observer = session->observer;
   long sequence = state->unknown0c;
   long owner = session->value10;
   if (network_observer_channel_ready(observer, owner, channel, 0x21))
    network_observer_mark_message(observer, owner, channel, 0x21);
   else if (session->state != 8)
   {
    if (sequence != NONE && sequence == *(long *)((byte *)session + 0x5e28))
     delta_mask |= 1 << i;
    else
     full_mask |= 1 << i;
   }
  }
 }
 if (full_mask)
  function_609e0(session, (const byte *)&session->update_count, NULL, full);
 if (delta_mask)
  function_609e0(session, (const byte *)&session->update_count, (const byte *)session + 0x5e28, delta);
 for (long j = 0; j < session->member_count; j++)
 {
  s_network_session_member_state *state = &session->member_states[j];
  byte *message;
  if (delta_mask & (1 << j))
   message = delta;
  else if (full_mask & (1 << j))
   message = full;
  else
   continue;
  state->unknown0c = session->update_count;
  network_session_send_to_member(session, j, 0, 0x21, 0x14d8, message);
 }
 memcpy((byte *)session + 0x5e28, &session->update_count, 0x14b0);
}

PRIVATE __forceinline bool function_61e02(byte *arg_0)
{
 long local_0 = *(long *)(arg_0 + 8);
 return network_session_time_now() - local_0 >= g_network_configuration.value1480;
}

// @retail 0x61e00
void function_61e00(c_class_58d20 *session)
{
 network_session_send_host_reestablish(session);
 byte *transition = (byte *)&session->value7420;
 if ((*(dword *)(transition + 0x10) | *(dword *)(transition + 0x14)) == (1u << session->member_count) - 1 ||
  function_61e02(transition))
 {
  bool leave = *(bool *)transition;
  bool migrate = *(bool *)(transition + 1);
  dword remove = (((1u << session->member_count) - 1) & ~*(dword *)(transition + 0x10)) | *(dword *)(transition + 0x14);
  network_session_enter_state_5(session);
  if (remove)
  {
   for (long i = session->member_count - 1; i >= 0; i--)
    if (i != session->current_member && (remove & (1 << i)))
     network_session_disband_member(session, i);
  }
  function_62640(session);
  function_627e0(session);
  if (leave)
   session->leave(false);
  else if (migrate)
   network_session_host_leave(session, false, NONE);
 }
}

struct s_membership_update_snapshot
{
	long update;
	long host;
	long member_count;
	s_session_member members[16];
	long player_count;
	dword player_mask;
	s_network_session_player players[16];
};

struct s_membership_member_change
{
	short old_index;
	short new_index;
	dword words[9];
	bool has_properties;
	bool properties_valid;
	byte unknown2a[2];
	s_session_parameters_update properties;
};

struct s_membership_player_change
{
	short player_index;
	short type;
	dword xuid[3];
	short member_index;
	short slot;
	byte unknown14[4];
	long unknown18;
	byte properties18[0x90];
	byte propertiesa8[0x90];
	long unknown13c;
};

struct s_membership_update
{
	byte unknown00[8];
	long version;
	long base;
	short member_change_count;
	short player_change_count;
	s_membership_member_change member_changes[32];
	s_membership_player_change player_changes[32];
	bool has_host;
	byte unknown4895[3];
	long host;
};

// @retail 0x5cb80
bool __stdcall function_05cb80(c_class_58d20 *session, const void *message)
{
	const s_membership_update *update = (const s_membership_update *)message;
	bool result = update->base == NONE || update->base == session->value4c;
	if (result)
	{
		s_membership_update_snapshot previous;
		memcpy(&previous, &session->value4c, sizeof(previous));
		s_network_session_member_state previous_states[16];
		memcpy(previous_states, session->member_states, sizeof(previous_states));
		session->value4c = update->version;
		session->update7618++;
		dword added = 0;
		dword moved = 0;
		dword removed = 0;
		dword retained = 0;
		long old_to_new[16];
		memset(old_to_new, NONE, sizeof(old_to_new));
		if (update->base != NONE)
			for (long member_index = 0; member_index < session->member_count; member_index++)
				old_to_new[member_index] = member_index;
		long i;
		long count = update->member_change_count;
		for (i = 0; result && i < count; i++)
		{
			const s_membership_member_change *change = &update->member_changes[i];
			if (change->old_index == NONE)
			{
				if (change->new_index < 0 || change->new_index >= 16 || !change->has_properties)
				{
					result = false;
					break;
				}
			}
			else if (change->new_index == NONE)
			{
				if (update->base == NONE || change->old_index < 0 || change->old_index >= previous.member_count ||
					change->has_properties || memcmp(previous.members[change->old_index].words, change->words, 0x24) ||
					old_to_new[change->old_index] != change->old_index)
				{
					result = false;
					break;
				}
				old_to_new[change->old_index] = NONE;
			}
			else
			{
				if (update->base == NONE || change->old_index < 0 || change->old_index >= previous.member_count ||
					change->new_index < 0 || change->new_index >= 16 ||
					memcmp(previous.members[change->old_index].words, change->words, 0x24) ||
					old_to_new[change->old_index] != change->old_index)
				{
					result = false;
					break;
				}
				old_to_new[change->old_index] = change->new_index;
			}
		}
		if (result)
		{
			if (update->base != NONE)
				retained = (1 << previous.member_count) - 1;
			for (i = 0; result && i < update->member_change_count; i++)
			{
				const s_membership_member_change *change = &update->member_changes[i];
				bool valid = false;
				if (change->old_index == NONE)
				{
					if (change->new_index >= 0 && change->new_index < 16 && change->has_properties)
					{
						s_session_member *member = &session->members[change->new_index];
						bool occupied = false;
						if (update->base != NONE)
							for (long j = 0; j < previous.member_count; j++)
								if (old_to_new[j] == change->new_index)
									occupied = true;
						if (!occupied)
						{
							memset(member, 0, sizeof(*member));
							member->properties_valid = change->properties_valid;
							memcpy(member->words, change->words, 0x24);
							memset(member->player_indices, NONE, sizeof(member->player_indices));
							session_parameters_apply_update(&member->properties, &change->properties);
							added |= 1 << change->new_index;
							valid = true;
						}
					}
				}
				else if (change->new_index == NONE)
				{
					if (update->base != NONE && change->old_index >= 0 && change->old_index < previous.member_count &&
						!change->has_properties && !memcmp(previous.members[change->old_index].words, change->words, 0x24))
					{
						removed |= 1 << change->old_index;
						retained &= ~(1 << change->old_index);
						valid = true;
					}
				}
				else
				{
					if (update->base != NONE && change->old_index >= 0 && change->old_index < previous.member_count &&
						change->new_index >= 0 && change->new_index < 16)
					{
						const s_session_member *previous_member = &previous.members[change->old_index];
						if (!memcmp(previous_member->words, change->words, 0x24))
						{
							s_session_member *member = &session->members[change->new_index];
							if (change->old_index != change->new_index)
								*member = *previous_member;
							if (change->has_properties)
							{
								member->properties_valid = change->properties_valid;
								session_parameters_apply_update(&member->properties, &change->properties);
							}
							if (change->old_index != change->new_index)
							{
								old_to_new[change->old_index] = change->new_index;
								moved |= 1 << change->new_index;
								retained &= ~(1 << change->old_index);
							}
							valid = change->has_properties || change->old_index != change->new_index;
						}
					}
				}
				if (!valid)
				{
					result = false;
					break;
				}
			}
		}
		if (result)
		{
			if (update->base == NONE)
			{
				count = update->member_change_count;
				if (added == (dword)((1 << count) - 1))
				{
					session->member_count = count;
					for (long previous_index = 0; previous_index < previous.member_count; previous_index++)
					{
						for (long j = 0; j < session->member_count; j++)
							if (!memcmp(previous.members[previous_index].words, session->members[j].words, 0x24))
								old_to_new[previous_index] = j;
						if (old_to_new[previous_index] == NONE)
							removed |= 1 << previous_index;
					}
				}
				else
					result = false;
			}
			else
			{
				dword mask = retained | moved | added;
				long first_free = NONE;
				for (long member_index = 0; member_index < 16; member_index++)
				{
					if (mask & (1 << member_index))
					{
						if (first_free != NONE)
						{
							result = false;
							break;
						}
					}
					else if (first_free == NONE)
						first_free = member_index;
				}
				if (first_free == NONE)
					first_free = 16;
				session->member_count = first_free;
			}
		}
		if (result)
		{
			long new_to_old[16];
			memset(new_to_old, NONE, sizeof(new_to_old));
			for (long previous_index = 0; previous_index < previous.member_count; previous_index++)
				if (!(removed & (1 << previous_index)))
					new_to_old[old_to_new[previous_index]] = previous_index;
			for (long previous_index = 0; previous_index < previous.member_count; previous_index++)
				if (removed & (1 << previous_index))
					network_session_member_state_dispose(session, previous_index);
			memset(session->member_states, 0, sizeof(session->member_states));
			for (long member_index = 0; member_index < session->member_count; member_index++)
			{
				long old_index = new_to_old[member_index];
				if (old_index != NONE)
					session->member_states[member_index] = previous_states[old_index];
				else
				{
					memset(&session->member_states[member_index], 0, sizeof(session->member_states[member_index]));
					long owner = session->value10;
					s_network_observer *observer = session->observer;
					long channel = network_observer_attach_channel(observer, owner, (const XNADDR *)session->members[member_index].words);
					if (channel != NONE)
						network_session_member_state_initialize(session, member_index, true, channel);
					else
						result = false;
				}
			}
		}
		if (result)
		{
			if (old_to_new[session->member_index] == NONE)
				result = false;
			else
				session->member_index = old_to_new[session->member_index];
			if (old_to_new[session->current_member] == NONE)
				result = false;
			else
				session->current_member = old_to_new[session->current_member];
			if (update->base != NONE)
			{
				for (long player_index = 0; player_index < 16; player_index++)
				{
					if (session->player_mask & (1 << player_index))
					{
						long member = old_to_new[session->players[player_index].member_index];
						if (member == NONE)
						{
							session->player_mask &= ~(1 << player_index);
							session->player_count--;
						}
						else
							session->players[player_index].member_index = member;
					}
				}
			}
		}
		if (result)
		{
			if (update->base == NONE && !update->has_host)
				result = false;
			else if (update->has_host)
			{
				long host = update->host;
				if (host < 0 || host >= session->member_count)
					result = false;
				else
					session->value50 = host;
			}
		}
		if (result)
		{
			if (update->base == NONE)
			{
				session->player_count = 0;
				session->player_mask = 0;
				for (long member_index = 0; member_index < session->member_count; member_index++)
				{
					memset(session->members[member_index].player_indices, NONE, sizeof(session->members[member_index].player_indices));
					session->members[member_index].player_count = 0;
				}
			}
			for (long change_index = 0; result && change_index < update->player_change_count; change_index++)
			{
				const s_membership_player_change *change = &update->player_changes[change_index];
				if (change->player_index < 0 || change->player_index >= 16)
					result = false;
				else
				{
					switch (change->type)
					{
					case 0:
					{
						long player_index = change->player_index;
						if (session->player_mask & (1 << player_index))
							network_session_remove_player(session, player_index);
						else
							result = false;
						break;
					}
					case 1:
					{
						long player_index = change->player_index;
						const XUID *id = (const XUID *)&change->xuid;
						if ((session->player_mask & (1 << player_index)) || change->member_index < 0 ||
							change->member_index >= session->member_count || change->slot < 0 || change->slot >= 4 ||
							session->members[change->member_index].player_indices[change->slot] != NONE ||
							network_session_find_player(session, (const dword *)id) != NONE)
							result = false;
						else
							network_session_add_player(session, change->member_index, id, player_index, change->slot);
						if (!result)
							break;
					}
					case 2:
					{
						long player_index = change->player_index;
						if (session->player_mask & (1 << player_index))
						{
							s_network_session_player *player = &session->players[player_index];
							player->unknown14 = change->unknown18;
							memcpy(player->properties18, change->properties18, 0x90);
							memcpy(player->propertiesa8, change->propertiesa8, 0x90);
							player->unknown138 = change->unknown13c;
						}
						else
							result = false;
						break;
					}
					default:
						result = false;
						break;
					}
				}
			}
		}
		if (!result)
		{
			session->flag48 = true;
			if (!result)
				network_session_close(session);
		}
	}
	else
		network_session_host_lost(session);
	return result;
}

void network_session_expire_reservations(c_class_58d20 *session);

// @retail 0x5a090
void function_5a090(c_class_58d20 *session)
{
 if (session->state != 0 && session->state != 10)
 {
  switch (session->state)
  {
  case 1: function_617c0(session); break;
  case 2: network_session_update_leaving_join_timeout(session); break;
  case 4: network_session_update_leaving_timeout(session); break;
  case 6: network_session_close(session); break;
  case 7: function_61ac0(session); break;
  case 8: function_61e00(session); break;
  case 9: function_61ef0(session); break;
  }
  if (session->function_058d20())
  {
   network_session_expire_reservations(session);
   if (session->state != 7 && session->state != 6 && session->state != 8)
   {
    for (long i = 0; i < session->member_count; i++)
    {
     s_network_session_member_state *member = &session->member_states[i];
     if (member->flag1 && session->observer->channels[member->unknown04].state == 1)
      network_session_disband_member(session, i);
    }
   }
  }
  else
  {
   if ((session->state > 2 && session->state <= 8))
   {
    if (session->observer->channels[session->member_states[session->member_index].unknown04].state == 1)
     network_session_host_lost(session);
    if ((session->state > 2 && session->state <= 8)) function_62990(session);
   }
  }
  function_62240(session);
  if (session->function_058d20())
  {
   function_62640(session);
   function_627e0(session);
  }
 }
}
