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

extern s_xnet_registry_entry g_4cf7d4[8];

bool transport_security_create_key(long local, long index, bool online);
bool transport_security_register_key(long index, long local, bool host, const XNKID *kid, const XNKEY *key);
bool function_07ab60(const s_type_99af70 *address, long local, long *index_out, XNKID *kid_out, XNKEY *key_out, XNADDR *xnaddr_out);

/* the session states (0x7420, 0x1f8 bytes) */
#define SESSION_STATE_DATA_SIZE 0x1f8

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
	state->unknown04 = channel_index;
	state->unknown00 = true;
	state->flag1 = connected;
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
void network_session_set_key(c_class_58d20 *session, const XNKID *kid, const XNKEY *key, long local)
{
	session->flag24 = true;
	*(XNKID *)&session->unknown1c = *kid;
	*(XNKEY *)session->data25 = *key;
	session->value3c = local;
	network_observer_set_owner_key(session->observer, session->value10, (const s_network_session_id *)&session->unknown1c, session->data25, session->value38, local);
	if (g_4cf8d4)
		XNetQosListen((XNKID *)&session->unknown1c, 0, 0, 0, XNET_QOS_LISTEN_ENABLE);
}

// @retail 0x5fa30
bool network_session_create_key(c_class_58d20 *session, long local, long mode)
{
	bool result = false;

	network_session_release_key(session);
	if (transport_security_create_key(local, session->value38, mode == 2))
	{
		s_xnet_registry_entry *entry = &g_4cf7d4[session->value38];
		if (entry->valid)
		{
			XNKID kid = entry->kid;
			XNKEY key = entry->key;
			network_session_set_key(session, &kid, &key, local);
			return true;
		}
	}
	return result;
}

// @retail 0x5fac0
bool network_session_join_key(c_class_58d20 *session, long mode, const XNKID *kid, const XNKEY *key, long local)
{
	network_session_release_key(session);
	if (transport_security_register_key(session->value38, local, false, kid, key))
	{
		network_session_set_key(session, kid, key, local);
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
bool network_session_player_add(c_class_58d20 *session, const byte *properties, const dword *identity, long slot, long unknown18, long unknownac)
{
	volatile bool result = false;

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
				return true;
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
			return true;
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
		long member_index = network_session_find_member(session, identity);
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
		long member_index = network_session_find_member(session, identity);
		if (member_index != NONE && member_index != session->current_member)
		{
			if (session->function_058d20())
			{
				network_session_boot_member(session, member_index);
				return true;
			}
			s_network_message_peer_identity message;
			memset(&message, 0, sizeof(message));
			message.session_id = *(s_session_id *)&session->unknown1c;
			message.identity = *identity;
			network_session_send_to_host(session, _network_message_type_boot_machine, sizeof(message), &message);
			return true;
		}
	}
	return result;
}

// @retail 0x5ade0
bool network_session_host_boot_member(c_class_58d20 *session, long member_index)
{
	if (session->function_058d20())
	{
		if (session->current_member == member_index)
			network_session_disconnect(session, 1);
		else
			network_session_boot_member(session, member_index);
		return true;
	}
	return false;
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

// @retail 0x5afc0
bool network_session_players_fit(c_class_58d20 *session, const dword *identities, long count, bool ignore_reservations)
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
	for (s_network_session_reservation *reservation = session->reservations; reservation < session->reservations + MAXIMUM_PLAYERS_PER_SESSION; reservation++)
	{
		if (reservation->active && !reservation->joined)
			pending++;
	}
	return count <= session->value4994 - session->player_count - pending + reserved;
}

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
		session->update_count++;
		session->value49c4 = result;
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
	if (session_state_is_live(session) && function_058d50(session))
	{
		if (summary && !session_summary_valid(summary))
			return false;
		if (session->function_058d20())
		{
			if (summary)
			{
				session->flag49fd = true;
				memcpy(session->data4a00, summary, sizeof(session->data4a00));
			}
			else
			{
				session->flag49fd = false;
				memset(session->data4a00, 0, sizeof(session->data4a00));
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
		return true;
	}
	return false;
}

// @retail 0x5b790
bool network_session_parameters_set_value4d08(c_class_58d20 *session, const char *string, long value4d08, long value4d0c)
{
	bool result = false;

	if (session_state_is_live(session) && function_058d50(session))
	{
		if (session->function_058d20())
		{
			session->value4d08 = value4d08;
			session->value4d0c = value4d0c;
			strncpy(session->string4d10, string, sizeof(session->string4d10));
			session->string4d10[sizeof(session->string4d10) - 1] = 0;
			session->update_count++;
		}
		else
		{
			s_network_message_parameters_request request;
			parameters_request_initialize(session, &request);
			request.change_value4d08 = true;
			request.value4d08 = value4d08;
			request.value4d0c = value4d0c;
			strncpy(request.string4d10, string, sizeof(request.string4d10));
			request.string4d10[sizeof(request.string4d10) - 1] = 0;
			network_session_send_to_host(session, _network_message_type_parameters_request, sizeof(request), &request);
		}
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
bool network_session_start_countdown(c_class_58d20 *session, long countdown, bool start, long mode, const long *time)
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
	bool local_0 = false;
	if (session_state_is_live(session))
	{
		if (data && !network_session_id_differs(session, data))
			goto local_1;
		if (session_state_is_live(session) && function_058d50(session))
		{
			if (session->state == 5 || session->state == 6 || session->state == 7 || session->state == 8)
			{
				if (data)
				{
					session->flag5dd8 = true;
					memcpy(session->data5ddc, data, sizeof(session->data5ddc));
				}
				else
				{
					session->flag5dd8 = false;
					memset(session->data5ddc, 0, sizeof(session->data5ddc));
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
			local_0 = true;
		}
	}
local_1:
	return local_0;
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

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (summary)
			{
				if (!session_summary_valid(summary))
					return result;
				session->flag49fd = true;
				memcpy(session->data4a00, summary, sizeof(session->data4a00));
			}
			else
			{
				session->flag49fd = false;
				memset(session->data4a00, 0, sizeof(session->data4a00));
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

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			if (data)
			{
				if (!network_session_id_differs(session, data))
					return result;
				session->flag5dd8 = true;
				memcpy(session->data5ddc, data, sizeof(session->data5ddc));
			}
			else
			{
				session->flag5dd8 = false;
				memset(session->data5ddc, 0, sizeof(session->data5ddc));
			}
			session->update_count++;
			result = true;
		}
	}
	return result;
}

/* ---- peers ---- */

bool network_session_channel_is_host_address(c_class_58d20 *session, const s_type_99af70 *address);

// @retail 0x5f7c0
bool network_session_address_is_peer(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (network_session_channel_is_host_address(session, address))
		return true;
	if (session_state_is_live(session) || session->state == 1)
	{
		long member_index = network_session_find_member_by_address(session, address);
		if (member_index != NONE && member_index != session->current_member && member_index == session->member_index)
			result = true;
	}
	return result;
}

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
				message.times[0] = NONE;
				message.times[1] = NONE;
				message.times[2] = NONE;
				message.times[3] = NONE;
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
			session->time78b0 = (message->times[2] - message->times[0] + message->times[3] - message->times[1]) / 2;
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
	if (session_state_is_live(other) && session_state_is_live(session))
	{
		bool result = true;
		for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if ((session->player_mask & (1 << i)) && network_session_find_player(other, (const dword *)&session->players[i]) == NONE)
				return false;
		}
		return result;
	}
	return false;
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

// @retail 0x5c8f0
bool network_session_channel_is_host_address(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (session->state == 1)
		result = transport_address_match(address, &((s_session_state_joining_data *)&session->value7420)->host_address);
	return result;
}

// @retail 0x5c940
bool network_session_address_is_leaving_host(c_class_58d20 *session, const s_type_99af70 *address)
{
	bool result = false;

	if (session->state == 2)
		result = transport_address_match(address, &((s_session_state_leaving_join_data *)&session->value7420)->host_address);
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
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	memcpy(&session->value7420, &data, sizeof(data));
	session->state = 2;
	network_session_update_leaving_join(session);
}

// @retail 0x61330
void network_session_leave_join_request(c_class_58d20 *session)
{
	s_session_state_leaving_data data;

	memset(&data, 0, sizeof(data));
	data.time = network_session_time_now();
	data.last_request_time = NONE;
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	memcpy(&session->value7420, &data, sizeof(data));
	session->state = 4;
	network_session_update_leaving(session);
}

// @retail 0x61450
void network_session_disband(c_class_58d20 *session)
{
	s_session_id message = *(s_session_id *)&session->unknown1c;
	network_session_send_to_members(session, 2, _network_message_type_session_disband, sizeof(message), &message);
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	session->state = 6;
}

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
		memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
		memcpy(&session->value7420, &data, sizeof(data));
		session->state = 7;
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

// @retail 0x62ab0
void network_session_host_lost(c_class_58d20 *session)
{
	switch (session->state)
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
	}
	if (session_state_is_live(session) && !session->function_058d20() && session->value4c != NONE && session->member_count > 1)
	{
		network_observer_close_channel(session->observer, session->member_states[session->member_index].unknown04);
		network_session_enter_state_9(session);
	}
	else
	{
		network_session_close(session);
	}
}

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
bool network_session_handle_countdown_timer(c_class_58d20 *session, long remote_index, const s_network_message_countdown_timer *message)
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
				return true;
			}
			return false;
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
			{
				network_session_close(session);
				return true;
			}
		}
		else
		{
			network_session_host_lost(session);
		}
		return true;
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
			{
				network_session_close(session);
				return true;
			}
		}
		else
		{
			network_session_disconnect(session, 1);
		}
		return true;
	}
	return result;
}

// @retail 0x5e5b0
bool network_session_handle_channel_closed(c_class_58d20 *session, long remote_index)
{
	long member_index = network_session_member_from_remote_inline(session, remote_index);

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
			return true;
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
		return true;
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

/* the host takes a player a peer adds, or refuses it: 0x5efd0, kept out of
   the build. Retail keeps all three arguments on the stack (ret 0xc) and its
   only caller, the message handler's 0x94700, pushes them; built here, our
   LTCG passes the session in eax, which breaks the matched 0x94700. The
   handler calls a stub of it (src/stubs/lane_j.cpp) until this matches. */
#if 0
bool __stdcall network_session_handle_player_add(c_class_58d20 *session, long remote_index, const void *data)
{
	const s_network_message_player_add *message = (const s_network_message_player_add *)data;
	bool result = false;

	if (session_state_is_live(session) && session->function_058d20())
	{
		long channel_index = network_observer_find_channel(session->observer, session->value10, remote_index);
		long member_index = network_session_find_member_by_channel(session, channel_index);
		if (member_index != NONE && member_index != session->current_member)
		{
			long player_index = network_session_add_local_player(session, member_index, message->slot, message->identity);
			if (player_index != NONE)
			{
				s_network_session_player *player = &session->players[player_index];
				player->unknown14 = message->unknown18;
				memcpy(player->properties18, message->properties, sizeof(player->properties18));
				player->unknown138 = message->unknownac;
				return true;
			}
			s_network_message_player_refuse refuse;
			memset(&refuse, 0, sizeof(refuse));
			refuse.session_id = *(s_session_id *)&session->unknown1c;
			refuse.slot = message->slot;
			refuse.identity[0] = message->identity[0];
			refuse.identity[1] = message->identity[1];
			refuse.identity[2] = message->identity[2];
			s_network_session_member_state *state = &session->member_states[member_index];
			if (state->flag1)
				network_observer_send_message(session->observer, session->value10, state->unknown04, false, _network_message_type_player_refuse, sizeof(refuse), &refuse);
			return true;
		}
	}
	return result;
}
#endif

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
			return true;
		}
	}
	return result;
}

/* retail calls 0x5f670 here; the inlined copy keeps the stack convention its
   caller 0x94640 (lane J) matches with, until this body matches */
// @retail 0x5ed90
bool network_session_handle_peer_properties(c_class_58d20 *session, long remote_index, const s_network_message_peer_properties *message)
{
	bool result = false;

	if (session_state_is_live(session))
	{
		if (session->function_058d20())
		{
			long member_index = network_session_member_from_remote_inline(session, remote_index);
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
					return true;
				}
				session->value4c++;
				session->update7618++;
				return true;
			}
		}
	}
	return result;
}

// @retail 0x5ee70
bool network_session_handle_delegate_leader(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message)
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
				return true;
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

// @retail 0x5ef20
bool network_session_handle_boot_machine(c_class_58d20 *session, long remote_index, const s_network_message_peer_identity *message)
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
				return true;
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
		const s_type_99af70 *host_address = &((s_session_state_leaving_join_data *)&session->value7420)->host_address;
		short length = address->address_length < host_address->address_length ? address->address_length : host_address->address_length;
		if (address->address_length > 0 && address->address_length == host_address->address_length && memcmp(address, host_address, length) == 0)
			network_session_close(session);
	}
}

// @retail 0x5c9e0
bool network_session_member_leave(c_class_58d20 *session, long member_index)
{
	bool result = false;

	if (network_session_is_leaving(session))
	{
		long state = session->state;
		if (state == 7)
		{
			network_session_clear_peer(session, member_index);
		}
		else if (state == 8)
		{
			dword bit = 1 << member_index;
			if (!(*(dword *)((byte *)&session->value7420 + 0x14) & bit))
				*(dword *)((byte *)&session->value7420 + 0x14) |= bit;
		}
		return result;
	}

	s_session_id member_id = session->members[member_index].id;
	s_session_id message = *(s_session_id *)&session->unknown1c;
	network_observer_send_message(session->observer, session->value10, session->member_states[member_index].unknown04, true, _network_message_type_leave_acknowledge, sizeof(message), &message);
	network_session_remove_member(session, member_index);
	if (session->listener)
		session->listener->member_left(&member_id);
	return true;
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
	long member_index = NONE;

	if (channel_index == NONE)
		return;
	for (long i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
	{
		if (session->member_states[i].unknown00 && session->member_states[i].unknown04 == channel_index)
		{
			member_index = i;
			break;
		}
	}
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
		long session_state = session->state;
		if (session_state != 5 && session_state != 6 && session_state != 7 && session_state != 8)
		{
			volatile long unused = session_state;
			if (member_index == session->member_index)
				network_session_reset_7620(session);
		}
		else
		{
			state->unknown08 = NONE;
			state->unknown0c = NONE;
			state->flag3 = true;
		}
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

static inline char *function_x91aa57(char *destination, const char *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

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
		if (message->change_value49a1)
		{
			byte current = *(volatile byte *)&session->data49a1[0];
			byte value = message->value49a1;
			if ((byte)(current - value))
			{
				session->data49a1[0] = value;
				changed = true;
			}
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

// @retail 0x5e210
bool network_session_handle_host_handoff(c_class_58d20 *session, const s_network_message_handoff *message)
{
	bool result = false;
	if (!session_state_is_live(session))
	{
		network_session_leave(session, false);
		return result;
	}
	short member_index = message->member_index;
	if (member_index < 0)
		return result;
	if (member_index >= session->member_count)
		return false;
	if (memcmp(&message->identity, session->members[member_index].words, sizeof(message->identity)) != 0 || member_index == session->member_index)
		return false;
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
	return true;
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
bool network_session_players_fit(c_class_58d20 *session, const dword *identities, long count, bool ignore_reservations);

// @retail 0x62fe0
long network_session_evaluate_join_request(c_class_58d20 *session, const s_session_join_request *request)
{
	long result = 0;
	if (!session->function_058d20())
		return 3;
	if (session->value498c > 1)
		return 2;
	long state = session->state;
	if (state == 7 || state == 6 || state == 8)
		return 3;
	if (network_session_find_member(session, &request->identity) != NONE)
		return result;
	long player_count = request->player_count;
	if (network_session_is_full(session, 1, player_count) || !network_session_players_fit(session, request->identities, player_count, request->ignore_reservations))
		return 4;
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
		bool hosting = false;
		long state = session->state;
		if (state == 5 || state == 6 || state == 7 || state == 8)
			hosting = true;
		else
		{
			volatile long unused = state;
		}
		long previous_host = session->member_index;
		if (!hosting)
		{
			network_session_reset_membership(session, true);
			session->member_index = session->current_member;
		}
		s_session_transition_state transition;
		memset(&transition, 0, sizeof(transition));
		transition.time = network_session_time_now();
		transition.host_index = previous_host;
		((byte *)&transition.unknown00)[0] = 0;
		((byte *)&transition.unknown00)[1] = flag;
		transition.sent_mask = 1 << session->current_member;
		transition.mask10 = transition.sent_mask;
		transition.mask14 = 0;
		memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
		*(s_session_transition_state *)&session->value7420 = transition;
		session->state = 8;
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

extern bool g_4cf792;
extern XNADDR g_4cf793;
bool function_07a9b0(void);
void function_07ad80(long count, byte *buffer);
void network_session_enter_state_5(c_class_58d20 *session);
extern "C" DWORD WINAPI XGetLanguage(void);

// @retail 0x59bd0 standard
bool __stdcall network_session_host(c_class_58d20 *session, long mode, long local, const XNKID *kid, const XNKEY *key, long count, const dword *identities, const long *values, const s_session_id *id, long timeout)
{
	volatile bool result = true;
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
			return false;
		if (kid && key)
		{
			if (!network_session_join_key(session, mode, kid, key, local))
				return false;
		}
		else
		{
			if (!network_session_create_key(session, local, mode))
				return false;
		}
	}
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
	function_07ad80(8, (byte *)&session->value4da0);
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
	session->flag78ac = true;
	session->time78b0 = -network_session_time_now();
	session->time78b4 = time(NULL);
	network_session_enter_state_5(session);
	if (timeout == NONE || timeout > 0)
		network_session_add_reservations(session, identities, id, count, timeout, values);
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
	function_07ad50(&data.remote.unknown14d[0x180 - 0x14d]);
	session->update7650++;
	memset(session->data761c, 0, sizeof(session->data761c));
	session->member_index = 0;
	session->value7654 = NONE;
	session->value7658 = NONE;
	network_session_add_member(session, 0, host, true, channel_index, 0);
	network_session_add_member(session, 1, (const s_session_member_identity *)remote->key188, false, NONE, 0);
	session->current_member = 1;
	memset(&session->value7420, 0, SESSION_STATE_DATA_SIZE);
	memcpy(&session->value7420, &data, sizeof(data));
	session->state = 1;
	function_62300(session);
}

long network_observer_attach_channel(s_network_observer *observer, long owner_index, const XNADDR *address);

// @retail 0x5c720
bool __stdcall function_5c720(c_class_58d20 *session, const s_session_member_identity *identity,
	long player_count, const dword *identities, const long *values, bool reserve,
	long timeout, const s_session_id *id, long *reason)
{
	volatile bool result = true;
	s_session_machine_address machine = *(const s_session_machine_address *)((const byte *)identity + 0xa);
	long member = network_session_find_member_by_machine(session, &machine);
	long channel;
	if (member == NONE)
	{
		channel = network_observer_attach_channel(session->observer, session->value10, (const XNADDR *)identity);
		if (channel == NONE)
		{
			*reason = 7;
			return false;
		}
	}
	else
	{
		s_network_session_member_state *state = &session->member_states[member];
		if (!state->flag1)
		{
			*reason = 5;
			return false;
		}
		s_session_member *existing = &session->members[member];
		if (!memcmp(existing->words, identity, sizeof(*identity)))
		{
			long time = state->unknown10;
			long now = network_session_time_now();
			if (!existing->properties_valid && now - time < g_network_configuration.value1468)
				return result;
		}
		network_session_remove_member(session, member);
		channel = network_observer_attach_channel(session->observer, session->value10, (const XNADDR *)identity);
		if (channel == NONE)
		{
			*reason = 7;
			return false;
		}
		network_observer_close_channel(session->observer, channel);
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
	return result;
}

// @retail 0x630f0
bool function_630f0(c_class_58d20 *session, const s_session_join_request *request,
 long address, long reason)
{
 bool result = false;
 if (!reason && function_5c720(session, &request->identity, request->player_count,
  request->identities, (const long *)((const byte *)request + 0xc4),
  request->ignore_reservations, *(const long *)((const byte *)request + 0x150),
  (const s_session_id *)((const byte *)request + 0x144), &reason))
  return true;
 long message[3];
 memset(message, 0, sizeof(message));
 message[0] = session->unknown1c;
 message[1] = session->unknown20;
 message[2] = reason;
 function_07b140(session->unknown04, address, 10, sizeof(message), message);
 return result;
}

bool network_observer_channel_timed_out(s_network_observer *observer, long channel_index);

// @retail 0x5e3f0
bool __stdcall function_05e3f0(c_class_58d20 *session, const s_type_99af70 *address)
{
 long member = network_session_find_member_by_address(session, address);
 bool was_host = false;
 bool leave = false;
 if (member != NONE && member != session->current_member)
 {
  long state = session->state;
  if (state == 9 || ((state > 2 && state <= 8) && member != session->member_index &&
   (member == session->value44 || (!session->function_058d20() &&
    !network_observer_channel_timed_out(session->observer, session->member_states[session->member_index].unknown04)))))
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
 if (!network_session_address_is_peer(session, address))
  return false;
 s_session_id id = *(s_session_id *)&session->unknown1c;
 s_network_session_member_state *member_state = &session->member_states[session->member_index];
 if (member_state->flag1)
  network_observer_send_message(session->observer, session->value10, member_state->unknown04,
   false, 0x14, sizeof(id), &id);
 if (leave)
  session->leave(false);
 return true;
}

struct s_network_session_list;
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *id);
bool function_07ab10(long key_index, s_type_99af70 *address, long local, word port, const XNADDR *xnaddr);

// @retail 0x59e50
bool __stdcall function_59e50(c_class_58d20 *session, long mode, long local,
 const XNKID *kid, const XNKEY *key, const s_session_member_identity *host,
 long count, const dword *identities, const long *values, const long *other_values,
 bool reserve, long timeout, const s_session_id *id, const void *extra)
{
 volatile bool result = false;
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
   network_session_set_key(session, kid, key, local);
   s_type_99af70 address;
   if (function_07ab10(session->value38, &address, local, 0x3e9, (const XNADDR *)host))
   {
    const byte *raw_address = (const byte *)&address;
    if (*(word *)(raw_address + 0x12) != 4 || *(dword *)raw_address != 0x7f000001)
    {
     long channel = network_observer_attach_channel(session->observer, session->value10, (const XNADDR *)host);
     if (channel != NONE)
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
      return true;
     }
     result = false;
    }
   }
  }
 }
 network_session_release_key(session);
 return result;
}

struct s_network_message_session_query;

// @retail 0x63080
void __stdcall function_063080(c_class_58d20 *session, const s_network_message_session_query *message,
 const s_type_99af70 *address)
{
 __declspec(align(8)) s_session_join_request request;
 memcpy(&request, (const byte *)message + 0xc, sizeof(request));
 long reason = network_session_evaluate_join_request(session, &request);
 if (!reason && session->listener)
  session->listener->unknown00(address, &request);
 else
  function_630f0(session, &request, (long)address, reason);
}

inline long count_bits(dword value);

bool function_619b0(const s_session_member *arg_0, long arg_1, long arg_2, long arg_3);

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
     if (selected == NONE || function_619b0(session->members, selected, i, session->member_count))
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
   if (choice->acknowledged == (1u << session->member_count) - 1 || elapsed >= g_network_configuration.value1474)
   {
    if (!(choice->acknowledged & (1 << choice->selected)))
     network_session_clear_peer(session, choice->selected);
    else
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
  }
  long local_1 = choice->commit_time;
  if (choice->committed && network_session_time_now() - local_1 >= g_network_configuration.value147c)
   network_session_clear_peer(session, choice->selected);
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
 s_session_peer_transition *transition = (s_session_peer_transition *)&session->value7420;
 for (long i = 0; i < session->member_count; i++)
 {
  if (i != session->current_member)
  {
   dword bit = 1 << i;
   if (transition->pending & bit)
   {
    if (network_session_time_now() - transition->times[i] >= g_network_configuration.value148c)
    {
     transition->pending &= ~bit;
     transition->failed |= bit;
    }
    if (transition->pending & bit)
     continue;
   }
   if (!(transition->failed & bit) && network_session_time_now() - transition->start_time >= g_network_configuration.value148c)
    transition->failed |= bit;
  }
 }
 if (transition->selected == session->current_member)
 {
  long failed = 0;
  long missing = 0;
  for (long i = 0; i < session->member_count; i++)
  {
   if (transition->failed & (1 << i))
    failed++;
   else
   {
    long index = session_peer_map_find_machine((s_session_peer_map *)transition->map, (const s_session_member_header *)&session->members[i]);
    if (index == NONE || !(*(dword *)(transition->map + 0xbc) & (1 << index)))
     missing++;
   }
  }
  if ((!failed || (network_session_time_now() - transition->start_time >= g_network_configuration.value148c &&
    network_session_time_now() - transition->selection_time >= g_network_configuration.value1490)) && !missing)
   function_061570(session, true);
 }
 else if (transition->failed & (1 << transition->selected))
 {
  transition->selected = network_session_build_peer_map(session, (s_session_peer_map *)transition->map);
  transition->selection_time = network_session_time_now();
 }
 if (session->state == 9)
 {
  if (!transition->send_time || network_session_time_now() - transition->send_time >= g_network_configuration.value1494 ||
   (transition->selection_time - transition->send_time > 0 && network_session_time_now() - transition->send_time >= 500))
  {
   struct { s_session_id id; byte map[0xc0]; } message;
   memset(&message, 0, sizeof(message));
   message.id = *(s_session_id *)&session->unknown1c;
   memcpy(message.map, transition->map, sizeof(message.map));
   for (long i = 0; i < session->member_count; i++)
   {
    s_network_session_member_state *member = &session->member_states[i];
    if (member->flag1)
     network_observer_send_message(session->observer, session->value10, member->unknown04, true, 22, sizeof(message), &message);
   }
   transition->send_time = network_session_time_now();
  }
  if (network_session_time_now() - transition->start_time >= g_network_configuration.value1488)
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

struct s_609e1
{
 const byte *field_0;
 s_609e1(const byte *arg_0) : field_0(arg_0) {}
};

// @retail 0x609e0
void __stdcall function_609e0(c_class_58d20 *session, const byte *current, s_609e1 arg_0, byte *update)
{
 const byte *local_0 = *(const byte *volatile *)&arg_0.field_0;
 memset(update, 0, 0x14d8);
 memcpy(update, &session->unknown1c, 8);
 *(long *)(update + 8) = *(const long *)current;
 long local_1;
 if (!local_0)
  local_1 = NONE;
 else
  local_1 = *(const long *)local_0;
 *(long *)(update + 0xc) = local_1;
 if (!local_0 || (*(const long *)(current + 4) != *(const long *)(local_0 + 4) || *(const long *)(current + 0x8) != *(const long *)(local_0 + 0x8)))
 {
  update[0x10] = true;
  memcpy(update + 0x14, current + 8, 4);
  memcpy(update + 0x18, current + 4, 4);
  update[0x11] = *((byte *)session + 0x765c);
 }
 if (!local_0 || memcmp(current + 0x10, local_0 + 0x10, 0x4))
 {
  update[0x1c] = true;
  memcpy(update + 0x20, current + 0x10, 0x4);
 }
 if (!local_0 || memcmp(current + 0x14, local_0 + 0x14, 0x4))
 {
  update[0x24] = true;
  memcpy(update + 0x28, current + 0x14, 0x4);
 }
 if (!local_0 || (*(const long *)(current + 0x18) != *(const long *)(local_0 + 0x18) || *(const long *)(current + 0x1c) != *(const long *)(local_0 + 0x1c)))
 {
  update[0x2c] = true;
  memcpy(update + 0x30, current + 0x18, 0x8);
 }
 if (!local_0 || current[0x20] != local_0[0x20] || memcmp(current + 0x21, local_0 + 0x21, 8))
 {
  update[0x38] = true;
  byte local_2 = current[0x20];
  update[0x39] = local_2;
  if (local_2) memcpy(update + 0x3a, current + 0x21, 8);
 }
 if (!local_0 || memcmp(current + 0x2c, local_0 + 0x2c, 0x4))
 {
  update[0x42] = true;
  memcpy(update + 0x44, current + 0x2c, 0x4);
 }
 if (!local_0 || memcmp(current + 0x4c, local_0 + 0x4c, 0x1))
 {
  update[0x48] = true;
  memcpy(update + 0x49, current + 0x4c, 0x1);
 }
 if (!local_0 || memcmp(current + 0x50, local_0 + 0x50, 0x4))
 {
  update[0x4a] = true;
  memcpy(update + 0x4c, current + 0x50, 0x4);
 }
 if (!local_0 || memcmp(current + 0x84, local_0 + 0x84, 0x1))
 {
  update[0x50] = true;
  memcpy(update + 0x51, current + 0x84, 0x1);
 }
 if (!local_0 || memcmp(current + 0x80, local_0 + 0x80, 0x4))
 {
  update[0x70] = true;
  memcpy(update + 0x74, current + 0x80, 0x4);
 }
 if (!local_0 || memcmp(current + 0x54, local_0 + 0x54, 0x1c))
 {
  update[0x52] = true;
  unsigned __int64 kind = *(const dword *)(current + 0x54);
  *(long *)(update + 0x54) = (long)kind;
  *(long *)(update + 0x68) = 0;
  *(long *)(update + 0x6c) = 0;
  *(long *)(update + 0x58) = 0;
  *(long *)(update + 0x60) = 0;
  *(long *)(update + 0x5c) = 0;
  *(long *)(update + 0x64) = 0;
  if (kind == 1)
  {
   *(long *)(update + 0x58) = *(const long *)(current + 0x58);
   *(long *)(update + 0x60) = *(const long *)(current + 0x60);
   *(long *)(update + 0x5c) = *(const long *)(current + 0x5c);
   *(long *)(update + 0x64) = *(const long *)(current + 0x64);
  }
  else if (kind == 2) memcpy(update + 0x68, current + 0x68, 4);
  else if (kind == 3) memcpy(update + 0x6c, current + 0x6c, 4);
 }
 if (!local_0 || current[0x85] != local_0[0x85] || memcmp(current + 0x88, local_0 + 0x88, 0x308))
 {
  update[0x88] = true;
  update[0x89] = current[0x85];
  memcpy(update + 0x8c, current + 0x88, 0x308);
 }
 if (!local_0 || current[0x70] != local_0[0x70] || memcmp(current + 0x78, local_0 + 0x78, 8))
 {
  update[0x78] = true;
  update[0x79] = current[0x70];
  memcpy(update + 0x80, current + 0x78, 8);
 }
 if (!local_0 || (*(const long *)(current + 0x390) != *(const long *)(arg_0.field_0 + 0x390) || *(const long *)(current + 0x394) != *(const long *)(arg_0.field_0 + 0x394)) ||
  strncmp((const char *)current + 0x398, (const char *)arg_0.field_0 + 0x398, 0x80))
 {
  update[0x394] = true;
  memcpy(update + 0x398, current + 0x390, 8);
  char *local_3 = (char *)update + 0x3a0;
  strncpy(local_3, (const char *)current + 0x398, 0x80);
  local_3[0x7f] = 0;
 }
 if (!arg_0.field_0 || (*(const long *)(current + 0x428) != *(const long *)(arg_0.field_0 + 0x428) || *(const long *)(current + 0x42c) != *(const long *)(arg_0.field_0 + 0x42c)))
 {
  update[0x420] = true;
  memcpy(update + 0x428, current + 0x428, 0x8);
 }
 if (!arg_0.field_0 || memcmp(current + 0x430, arg_0.field_0 + 0x430, 0x4))
 {
  update[0x430] = true;
  memcpy(update + 0x434, current + 0x430, 0x4);
 }
 if (!arg_0.field_0 || memcmp(current + 0x434, arg_0.field_0 + 0x434, 0x4))
 {
  update[0x438] = true;
  memcpy(update + 0x43c, current + 0x434, 0x4);
 }
 if (!arg_0.field_0 || memcmp(current + 0x438, arg_0.field_0 + 0x438, 0x130))
 {
  update[0x440] = true;
  memcpy(update + 0x444, current + 0x438, 0x130);
 }
 if (!arg_0.field_0 || wcsncmp((const wchar_t *)(current + 0x568), (const wchar_t *)(arg_0.field_0 + 0x568), 32))
 {
  update[0x574] = true;
  wchar_t *local_6 = (wchar_t *)(update + 0x576);
  wcsncpy(local_6, (const wchar_t *)(current + 0x568), 31);
  local_6[31] = 0;
 }
 if (arg_0.field_0)
 {
  const byte *local_4 = *(const byte *volatile *)&arg_0.field_0;
  if (current[0x5a8] == local_4[0x5a8])
  {
   local_4 = *(const byte *volatile *)&arg_0.field_0;
   if (!memcmp(current + 0x5ac, local_4 + 0x5ac, 0x6c) && !memcmp(current + 0x618, local_4 + 0x618, 0xe40))
    goto local_5;
  }
 }
 {
  update[0x5b6] = true;
  update[0x5b7] = current[0x5a8];
  memcpy(update + 0x5b8, current + 0x5ac, 0x6c);
  memcpy(update + 0x624, current + 0x618, 0xe40);
 }
local_5:
 if (!arg_0.field_0 || memcmp(current + 0x29, arg_0.field_0 + 0x29, 0x1))
 {
  update[0x1464] = true;
  memcpy(update + 0x1465, current + 0x29, 0x1);
 }
 if (!arg_0.field_0 || memcmp(current + 0x1458, arg_0.field_0 + 0x1458, 0x2))
 {
  update[0x1466] = true;
  memcpy(update + 0x1468, current + 0x1458, 0x2);
 }
 if (!arg_0.field_0 || current[0x30] != arg_0.field_0[0x30] || (*(const long *)(current + 0x34) != *(const long *)(arg_0.field_0 + 0x34) || *(const long *)(current + 0x38) != *(const long *)(arg_0.field_0 + 0x38) || *(const long *)(current + 0x3c) != *(const long *)(arg_0.field_0 + 0x3c) || memcmp(current + 0x40, arg_0.field_0 + 0x40, 0xc)))
 {
  update[0x146a] = true;
  update[0x146b] = current[0x30];
  *(long *)(update + 0x146c) = *(const long *)(current + 0x34);
  *(long *)(update + 0x1470) = *(const long *)(current + 0x38);
  *(long *)(update + 0x1474) = *(const long *)(current + 0x3c);
  memcpy(update + 0x1478, current + 0x40, 0xc);
 }
 if (!arg_0.field_0 || current[0x1460] != arg_0.field_0[0x1460] || memcmp(current + 0x1464, arg_0.field_0 + 0x1464, 0x44))
 {
  update[0x1484] = true;
  update[0x1485] = current[0x1460];
  memcpy(update + 0x1488, current + 0x1464, 0x44);
 }
 if (!arg_0.field_0 || memcmp(current + 0x14a8, arg_0.field_0 + 0x14a8, 0x4))
 {
  update[0x14cc] = true;
  memcpy(update + 0x14d0, current + 0x14a8, 0x4);
 }
}

bool session_peer_map_update_reachable(s_session_member_header *a, s_session_member_header *b, s_session_peer_map *map);
bool session_peer_map_set_connected(s_session_peer_map *map, const s_session_member_header *member, bool connected);

// @retail 0x5e7f0
bool __stdcall function_05e7f0(c_class_58d20 *session, const s_type_99af70 *address, const void *message,
 long *reason, bool *has_identity, s_session_member_identity *identity)
{
 long sender = network_session_find_member_by_address(session, address);
 long state = session->state;
 if (!state || state == 10)
 {
  *reason = 1;
  return false;
 }
 if (state != 9)
 {
  if (state <= 2 || state > 8)
  {
   *reason = 2;
   return false;
  }
  if (function_058d90(session) && !session->function_058d20())
  {
   *reason = 3;
   return false;
  }
  if (state == 8)
  {
   if (sender == NONE) return true;
   goto acknowledge;
  }
  const s_session_member_identity *host = (const s_session_member_identity *)&session->members[session->member_index];
  s_network_session_member_state *host_state = &session->member_states[session->member_index];
  bool same_host = !memcmp(host, (const byte *)message + 8, 0x24);
  if (session->function_058d20())
  {
   if (sender != NONE) goto acknowledge;
   *reason = 5;
   *has_identity = true;
   *identity = *host;
   return false;
  }
  if (network_observer_channel_timed_out(session->observer, host_state->unknown04))
  {
   *reason = same_host ? 4 : 6;
   *has_identity = true;
   *identity = *host;
   return false;
  }
  if (session->value4c == NONE)
  {
   *reason = 8;
   return false;
  }
  if (session->observer->channels[host_state->unknown04].state == 7 && !same_host)
   return true;
 }
 if (session->state != 9) network_session_enter_state_9(session);
 if (sender == NONE || sender == session->current_member)
 {
  *reason = 9;
  return false;
 }
 {
  byte received[0xc0];
  memcpy(received, (const byte *)message + 8, sizeof(received));
  s_session_member_header *remote = (s_session_member_header *)&session->members[sender];
  s_session_member_header *local = (s_session_member_header *)&session->members[session->current_member];
  s_session_peer_transition *transition = (s_session_peer_transition *)&session->value7420;
  s_session_peer_map *map = (s_session_peer_map *)transition->map;
  session_peer_map_update_reachable(local, remote, (s_session_peer_map *)received);
  session_peer_map_update_reachable(local, remote, map);
  long selected = network_session_find_member(session, (const s_session_member_identity *)(received + 0x24));
  bool better = false;
  bool same = false;
  bool accepted = true;
  long version = *(long *)(received + 0x48);
  long old_version = *(long *)(transition->map + 0x48);
  if (version > old_version)
   better = selected != NONE;
  else if (version == old_version)
  {
   if (selected == NONE || selected != *(long *)(received + 0x4c))
    accepted = false;
   else if (selected == *(long *)(transition->map + 0x4c))
    same = true;
   else
   {
    long old_count = count_bits(*(dword *)(transition->map + 0xb8));
    long new_count = count_bits(*(dword *)(received + 0xb8));
    if (new_count > old_count || (new_count == old_count &&
     (*(long *)(received + 0x50) > *(long *)(transition->map + 0x50) ||
      (*(long *)(received + 0x50) == *(long *)(transition->map + 0x50) &&
       *(long *)(received + 0x4c) < *(long *)(transition->map + 0x4c)))))
     better = true;
   }
  }
  dword bit = 1 << sender;
  if (accepted)
  {
   transition->pending |= bit;
   transition->failed &= ~bit;
   transition->times[sender] = network_session_time_now();
   if (same)
   {
    if (session_peer_map_set_connected(map, remote, true))
     transition->selection_time = network_session_time_now();
   }
   else if (better)
   {
    memcpy(transition->map, received, sizeof(received));
    transition->selected = selected;
    transition->selection_time = network_session_time_now();
    session_peer_map_set_connected(map, local, true);
   }
   if (same || better) return true;
  }
  else
  {
   transition->pending &= ~bit;
   transition->failed |= bit;
   *reason = 7;
  }
  if (session_peer_map_set_connected(map, remote, false))
   transition->selection_time = network_session_time_now();
  return accepted;
 }
acknowledge:
 {
  s_session_id id = *(s_session_id *)&session->unknown1c;
  s_network_session_member_state *peer = &session->member_states[sender];
  if (peer->flag1)
  {
   if (session->observer->channels[peer->unknown04].state == 7)
    network_observer_send_message(session->observer, session->value10, peer->unknown04, false, 18, sizeof(id), &id);
   network_observer_send_message(session->observer, session->value10, peer->unknown04, true, 18, sizeof(id), &id);
  }
  peer->unknown08 = NONE;
  peer->unknown0c = NONE;
  peer->flag3 = true;
  if (session->state == 7) network_session_clear_peer(session, sender);
  return true;
 }
}


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
   if (network_observer_channel_ready(session->observer, state->unknown04, 0x19))
    network_observer_mark_message(session->observer, state->unknown04, 0x19);
   else if (session->state != 8)
   {
    if (state->unknown08 != NONE && state->unknown08 == session->value24e0)
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
  byte *message;
  if (delta_mask & (1 << j))
   message = delta;
  else if (full_mask & (1 << j))
   message = full;
  else
   continue;
  session->member_states[j].unknown08 = session->value4c;
  network_session_send_to_member(session, j, 0, 0x19, 0x489c, message);
 }
 memcpy(&session->value24e0, &session->value4c, 0x2494);
}

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
   if (network_observer_channel_ready(session->observer, state->unknown04, 0x21))
    network_observer_mark_message(session->observer, state->unknown04, 0x21);
   else if (session->state != 8)
   {
    if (state->unknown0c != NONE && state->unknown0c == *(long *)((byte *)session + 0x5e28))
     delta_mask |= 1 << i;
    else
     full_mask |= 1 << i;
   }
  }
 }
 if (full_mask)
  function_609e0(session, (const byte *)&session->update_count, s_609e1(0), full);
 if (delta_mask)
  function_609e0(session, (const byte *)&session->update_count, s_609e1((const byte *)session + 0x5e28), delta);
 for (long j = 0; j < session->member_count; j++)
 {
  byte *message;
  if (delta_mask & (1 << j))
   message = delta;
  else if (full_mask & (1 << j))
   message = full;
  else
   continue;
  session->member_states[j].unknown0c = session->update_count;
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

// @retail 0x5cb80
bool __stdcall function_05cb80(c_class_58d20 *session, const void *message)
{
	bool result = true;
	const byte *update = (const byte *)message;
	if (*(const long *)(update + 0xc) != NONE && *(const long *)(update + 0xc) != session->value4c)
		goto stale_update;
	s_membership_update_snapshot previous;
	memcpy(&previous, &session->value4c, sizeof(previous));
	s_network_session_member_state previous_states[16];
	memcpy(previous_states, session->member_states, sizeof(previous_states));
	session->value4c = *(const long *)(update + 8);
	session->update7618++;
	dword added = 0;
	dword moved = 0;
	dword removed = 0;
	dword retained = 0;
	long old_to_new[16];
	memset(old_to_new, NONE, sizeof(old_to_new));
	if (*(const long *)(update + 0xc) != NONE)
		for (long i = 0; i < session->member_count; i++)
			old_to_new[i] = i;
	for (long i = 0; i < *(const short *)(update + 0x10); i++)
	{
		const byte *entry = update + 0x14 + i * 0x104;
		short from = *(const short *)entry;
		short to = *(const short *)(entry + 2);
		if (from == NONE)
		{
			if (to < 0 || to >= 16 || !entry[0x28]) goto failed;
		}
		else if (to == NONE)
		{
			if (*(const long *)(update + 0xc) == NONE || from < 0 || from >= previous.member_count ||
				entry[0x28] || memcmp(previous.members[from].words, entry + 4, 0x24) || old_to_new[from] != from)
				goto failed;
			old_to_new[from] = NONE;
		}
		else
		{
			if (*(const long *)(update + 0xc) == NONE || from < 0 || from >= previous.member_count ||
				to < 0 || to >= 16 || memcmp(previous.members[from].words, entry + 4, 0x24) || old_to_new[from] != from)
				goto failed;
			old_to_new[from] = to;
		}
	}
	if (*(const long *)(update + 0xc) != NONE)
		retained = (1 << previous.member_count) - 1;
	for (long i = 0; i < *(const short *)(update + 0x10); i++)
	{
		const byte *entry = update + 0x14 + i * 0x104;
		short from = *(const short *)entry;
		short to = *(const short *)(entry + 2);
		if (from == NONE)
		{
			if (to < 0 || to >= 16 || !entry[0x28]) goto failed;
			s_session_member *member = &session->members[to];
			bool occupied = false;
			if (*(const long *)(update + 0xc) != NONE)
				for (long j = 0; j < previous.member_count; j++)
					if (old_to_new[j] == to) occupied = true;
			if (occupied) goto failed;
			memset(member, 0, sizeof(*member));
			member->properties_valid = *(const bool *)(entry + 0x29);
			memcpy(member->words, entry + 4, 0x24);
			for (long j = 0; j < 4; j++) member->player_indices[j] = NONE;
			session_parameters_apply_update(&member->properties, (const s_session_parameters_update *)(entry + 0x2c));
			added |= 1 << to;
		}
		else if (to == NONE)
		{
			if (*(const long *)(update + 0xc) == NONE || from < 0 || from >= previous.member_count ||
				entry[0x28] || memcmp(previous.members[from].words, entry + 4, 0x24)) goto failed;
			removed |= 1 << from;
			retained &= ~(1 << from);
		}
		else
		{
			if (*(const long *)(update + 0xc) == NONE || from < 0 || from >= previous.member_count ||
				to < 0 || to >= 16 || memcmp(previous.members[from].words, entry + 4, 0x24)) goto failed;
			s_session_member *member = &session->members[to];
			if (from != to) *member = previous.members[from];
			if (entry[0x28])
			{
				member->properties_valid = *(const bool *)(entry + 0x29);
				session_parameters_apply_update(&member->properties, (const s_session_parameters_update *)(entry + 0x2c));
			}
			if (from != to)
			{
				old_to_new[from] = to;
				moved |= 1 << to;
				retained &= ~(1 << from);
			}
			if (!entry[0x28] && from == to) goto failed;
		}
	}
	if (*(const long *)(update + 0xc) == NONE)
	{
		long count = *(const short *)(update + 0x10);
		if (added != (dword)((1 << count) - 1)) goto failed;
		session->member_count = count;
		for (long i = 0; i < previous.member_count; i++)
		{
			for (long j = 0; j < session->member_count; j++)
				if (!memcmp(previous.members[i].words, session->members[j].words, 0x24)) old_to_new[i] = j;
			if (old_to_new[i] == NONE) removed |= 1 << i;
		}
	}
	else
	{
		dword mask = retained | moved | added;
		long count = NONE;
		for (long i = 0; i < 16; i++)
		{
			if (mask & (1 << i))
			{
				if (count != NONE) { result = false; break; }
			}
			else if (count == NONE) count = i;
		}
		if (count == NONE) count = 16;
		session->member_count = count;
		if (!result) goto finish;
	}
	{
		long new_to_old[16];
		memset(new_to_old, NONE, sizeof(new_to_old));
		for (long i = 0; i < previous.member_count; i++)
			if (!(removed & (1 << i))) new_to_old[old_to_new[i]] = i;
		for (long i = 0; i < previous.member_count; i++)
			if (removed & (1 << i)) network_session_member_state_dispose(session, i);
		memset(session->member_states, 0, sizeof(session->member_states));
		for (long i = 0; i < session->member_count; i++)
		{
			if (new_to_old[i] != NONE) session->member_states[i] = previous_states[new_to_old[i]];
			else
			{
				memset(&session->member_states[i], 0, sizeof(session->member_states[i]));
				long channel = network_observer_attach_channel(session->observer, session->value10, (const XNADDR *)session->members[i].words);
				if (channel != NONE) network_session_member_state_initialize(session, i, true, channel);
				else result = false;
			}
		}
	}
	if (!result) goto finish;
	if (old_to_new[session->member_index] == NONE) result = false;
	else session->member_index = old_to_new[session->member_index];
	if (old_to_new[session->current_member] == NONE) result = false;
	else session->current_member = old_to_new[session->current_member];
	if (*(const long *)(update + 0xc) != NONE)
	{
		for (long i = 0; i < 16; i++)
		{
			if (session->player_mask & (1 << i))
			{
				long member = old_to_new[session->players[i].member_index];
				if (member == NONE)
				{
					session->player_mask &= ~(1 << i);
					session->player_count--;
				}
				else session->players[i].member_index = member;
			}
		}
	}
	if (!result) goto finish;
	if (*(const long *)(update + 0xc) == NONE && !update[0x4894]) goto failed;
	if (update[0x4894])
	{
		long host = *(const long *)(update + 0x4898);
		if (host < 0 || host >= session->member_count) goto failed;
		session->value50 = host;
	}
	if (*(const long *)(update + 0xc) == NONE)
	{
		session->player_count = 0;
		session->player_mask = 0;
		for (long i = 0; i < session->member_count; i++)
		{
			for (long j = 0; j < 4; j++) session->members[i].player_indices[j] = NONE;
			session->members[i].player_count = 0;
		}
	}
	for (long i = 0; result && i < *(const short *)(update + 0x12); i++)
	{
		const byte *entry = update + 0x2094 + i * 0x140;
		short player = *(const short *)entry;
		if (player < 0 || player >= 16) { result = false; continue; }
		switch (*(const short *)(entry + 2))
		{
		case 0:
			if (!(session->player_mask & (1 << player))) { result = false; break; }
			network_session_remove_player(session, player);
			break;
		case 1:
			{
				short member = *(const short *)(entry + 0x10);
				short slot = *(const short *)(entry + 0x12);
				if ((session->player_mask & (1 << player)) || member < 0 || member >= session->member_count ||
					slot < 0 || slot >= 4 || session->members[member].player_indices[slot] != NONE ||
					network_session_find_player(session, (const dword *)(entry + 4)) != NONE)
				{ result = false; break; }
				network_session_add_player(session, member, (const XUID *)(entry + 4), player, slot);
			}
		case 2:
			if (!(session->player_mask & (1 << player))) { result = false; break; }
			session->players[player].unknown14 = *(const long *)(entry + 0x18);
			memcpy(session->players[player].properties18, entry + 0x1c, 0x90);
			memcpy(session->players[player].propertiesa8, entry + 0xac, 0x90);
			session->players[player].unknown138 = *(const long *)(entry + 0x13c);
			break;
		default: result = false; break;
		}
	}
	if (result) goto local_0;
	finish:
	session->flag48 = true;
	if (!result) network_session_close(session);
	local_0:
	return result;
	failed:
	result = false;
	goto finish;
	stale_update:
	network_session_host_lost(session);
	result = false;
	goto local_0;
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
  long state = session->state;
  if (state == 5 || state == 6 || state == 7 || state == 8)
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
   volatile long unused = state;
   if ((session->state > 2 && session->state <= 8))
   {
    if (session->observer->channels[session->member_states[session->member_index].unknown04].state == 1)
     network_session_host_lost(session);
    if ((session->state > 2 && session->state <= 8)) function_62990(session);
   }
  }
  function_62240(session);
  state = session->state;
  if (state == 5 || state == 6 || state == 7 || state == 8)
  {
   function_62640(session);
   function_627e0(session);
  }
  else
  {
   volatile long unused = state;
  }
 }
}
