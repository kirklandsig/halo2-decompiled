// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_075870.CPP: the network observer's owners and channels (lane D,
   outside its regions: the session code sends through it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_059ad0.h"
#include "unknown_0662e0.h"
#include "unknown_0259d0.h"
#include "network_voice.h"
#include <xtl.h>
#include <string.h>
#include <float.h>

struct s_session_machine_address
{
	byte data[6];
};

/* the security keys (0x7a9a0) and the transport */
bool function_07ab10(long key_index, s_type_99af70 *address, long local, word port, const XNADDR *xnaddr);
bool function_07acc0(const s_type_99af70 *address);

// @retail 0x75870
long function_75870(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

// @retail 0x75890
long function_75890(long time)
{
	long now;
	if (g_510548)
		now = g_51054c;
	else
		now = GetTickCount();
	return now - time;
}

/* the sessions (g_510550; unknown_075800.cpp) */
struct s_network_session_list;
s_network_session_list *g_510550;
c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *session_id);

/* how long ago the session with this id started, or 0 */
// @retail 0x758c0
long network_session_time_since_start(const s_session_id *session_id)
{
	long result = 0;
	s_network_session_list *manager = g_510550;
	if (manager)
	{
		c_class_58d20 *session = network_session_manager_find_session(manager, session_id);
		if (session && session->flag78ac)
			result = GetTickCount() + session->time78b0;
	}
	return result;
}

/* the object a connection reports to */
struct s_network_connection_owner
{
	byte unknown00[0x30];
	bool active;
};

// @retail 0x75910
long network_connection_owner_active(s_network_connection *connection)
{
	s_network_connection_owner *owner = (s_network_connection_owner *)connection->callback;
	if (owner && owner->active)
		return 1;
	return 0;
}

// @retail 0x75930
bool network_connection_get_address(s_network_connection *connection, s_type_99af70 *address)
{
	bool result = false;
	if (connection->state != 0 && connection->state != 1)
	{
		*address = connection->address;
		result = true;
	}
	return result;
}

// @retail 0x75a90
void network_observer_set_owner_key(s_network_observer *observer, long owner, const s_network_session_id *id, const byte *key, long key_index, long local)
{
	if (key_index != NONE)
	{
		observer->owners[owner].key_index = key_index;
		s_network_observer_owner *record = &observer->owners[owner];
		record->local = local;
		record->id = *id;
		*(XNKEY *)record->key = *(const XNKEY *)key;
	}
	else
	{
		observer->owners[owner].key_index = NONE;
	}
}

// @retail 0x75ce0
long network_observer_find_channel(s_network_observer *observer, long owner, long remote_index)
{
	long result = NONE;

	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *channel = &observer->channels[i];
		if (channel->state && channel->connection_index == remote_index && (channel->owner_mask & (1 << owner)))
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x75d30
long network_observer_find_channel_by_machine(s_network_observer *observer, const s_session_machine_address *address, long owner)
{
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *channel = &observer->channels[i];
		if (channel->state && (channel->owner_mask & (1 << owner)))
		{
			s_session_machine_address machine = *(s_session_machine_address *)((byte *)channel->remote_id + 0xa);
			if (memcmp(address, &machine, sizeof(machine)) == 0)
				return i;
		}
	}
	return NONE;
}

// @retail 0x75e40
void network_observer_close_channel(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state > 2)
			network_connection_close(connection, 0x11);
	}
}

/* the transport address of an owner's machine, from its secure key */
// @retail 0x783d0
bool network_observer_get_owner_address(s_network_observer *observer, long owner, const XNADDR *xnaddr, s_type_99af70 *address, long *key_index, s_network_session_id *id, XNKEY *key)
{
	/* retail keeps the result in a stack slot (a volatile local reproduces it) */
	volatile bool result = false;

	if (observer->owners[owner].active && observer->owners[owner].key_index != NONE)
	{
		s_type_99af70 secure_address;
		if (function_07ab10(observer->owners[owner].key_index, &secure_address, observer->owners[owner].local, 1000, xnaddr) && function_07acc0(&secure_address))
		{
			*address = secure_address;
			*key_index = observer->owners[owner].key_index;
			*id = observer->owners[owner].id;
			*key = *(XNKEY *)observer->owners[owner].key;
			return true;
		}
	}
	memset(address, 0, sizeof(*address));
	return result;
}

// @retail 0x75e80
void network_observer_send_message(s_network_observer *observer, long owner, long channel_index, bool out_of_band, long message_type, long message_size, const void *message)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];

	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (out_of_band)
		{
			s_type_99af70 address;
			if (function_7af40(&channel->address))
			{
				address = channel->address;
			}
			else
			{
				long key_index;
				s_network_session_id id;
				XNKEY key;
				if (!network_observer_get_owner_address(observer, owner, (const XNADDR *)channel->remote_id, &address, &key_index, &id, &key))
					return;
			}
			function_07b140(observer->link, (long)&address, message_type, message_size, (void *)message);
		}
		else if (connection->state > 2)
		{
			unsigned __int64 bit = (unsigned __int64)1 << message_type;
			if (channel->message_mask & bit)
				channel->message_mask &= ~bit;
			function_095580(network_stream_get(connection->stream_index), message_type, message_size, message);
		}
	}
}

// @retail 0x768b0
bool network_observer_channel_ready(s_network_observer *observer, long channel_index, long message_type)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	bool result = false;

	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state == 5)
		{
			if (!(channel->message_mask & ((unsigned __int64)1 << message_type)))
				result = network_connection_send_capacity(connection) * 4 < 0x4000;
			else
				result = network_connection_send_capacity(connection) * 4 < 0xc000;
		}
	}
	return result;
}

// @retail 0x76930
void network_observer_mark_message(s_network_observer *observer, long channel_index, long message_type)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];

	if (channel->connection_index != NONE && function_x7665e0(channel->connection_index)->state == 5)
	{
		unsigned __int64 bit = (unsigned __int64)1 << message_type;
		if (!(channel->message_mask & bit))
			channel->message_mask |= bit;
	}
}

/* function_75870, which retail inlines here */
static inline long observer_time_get(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

// @retail 0x77330
void network_observer_set_channel_state(s_network_observer *observer, long state, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->state != state)
	{
		channel->state = state;
		channel->time = observer_time_get();
		if (channel->state == 1)
		{
			for (long i = 0; i < MAXIMUM_OBSERVER_OWNERS; i++)
			{
				if (channel->owner_mask & (1 << i))
					observer->owners[i].active->channel_closed(channel_index);
			}
		}
	}
}

// @retail 0x75c80
bool network_observer_get_bandwidth(s_network_observer *observer, long *value4e08, real *ratio, long *value4e0c)
{
	bool result = false;
	if (observer->value4e08 != NONE && observer->value4e0c != NONE)
	{
		long count = observer->value4e18;
		if (observer->value4e1c + count > 0)
		{
			real fraction = (real)observer->value4e1c / (real)(observer->value4e1c + count);
			*value4e08 = observer->value4e08;
			*ratio = fraction;
			*value4e0c = observer->value4e0c;
			result = true;
		}
	}
	return result;
}

// @retail 0x769a0
bool network_observer_channel_timed_out(s_network_observer *observer, long channel_index)
{
	s_network_observer *const *local_0 = &observer;
	s_network_observer_channel *channel = &(*local_0)->channels[channel_index];
	bool result = false;
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state == 5)
		{
			long last = connection->timers[1].time;
			long since = observer_time_get() - last;
			long time = 0;
			if (connection->state > 2)
				time = connection->timers[4].time;
			long now = observer_time_get();
			if (since < observer->configuration->timeout78 && now - time >= observer->configuration->timeout7c)
				result = false;
			else
				result = true;
		}
	}
	return result;
}

/* closes a channel's connection when nothing has come over it for too long */
// @retail 0x773a0
void network_observer_check_channel_activity(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->state)
	{
		if (channel->connection_index != NONE)
		{
			s_network_connection *connection = function_x7665e0(channel->connection_index);
			if (connection->state == 5)
			{
				long last = connection->timers[0].time;
				if (observer_time_get() - last < observer->configuration->timeout74)
				{
					long since_activity = function_75890(channel->time94);
					long local_0 = *(volatile long *)&connection->state;
					long local_1 = 0;
					if (local_0 > 2)
						local_1 = connection->timers[3].time;
					long since_timer = function_75890(local_1);
					if (since_activity >= observer->configuration->timeout80 && since_timer >= observer->configuration->timeout84)
						network_connection_close(connection, 0x10);
					return;
				}
			}
		}
		channel->time94 = observer_time_get();
	}
}

/* function_75890, which retail inlines here */
static inline long observer_time_since(long time)
{
	return observer_time_get() - time;
}

long network_connection_send_capacity(s_network_connection *connection);

/* whether a channel has to be given up: its connection's send window has
   stayed full (the connection is closed for the reason) or it has had no
   connection for too long, or its address has gone */
// @retail 0x77580
bool network_observer_channel_stalled(s_network_observer *observer, long channel_index, long reason)
{
	bool local_1 = false;
	bool blocked = true;
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state == 5)
		{
			if (network_connection_send_capacity(connection) < 0x4000)
				blocked = false;
			else
				network_connection_close(connection, reason);
		}
	}
	if (!channel->time98)
		channel->time98 = observer_time_get();
	bool expired = observer_time_since(channel->time98) > g_network_configuration.value152c;
	if (blocked)
	{
		if (!channel->time9c)
			channel->time9c = observer_time_get();
		long local_0 = observer_time_since(channel->time9c);
		if (expired || local_0 > g_network_configuration.value1528)
			{
				local_1 = true;
				goto local_2;
			}
	}
	else
	{
		channel->time9c = 0;
		if (expired)
			{
				local_1 = true;
				goto local_2;
			}
	}
	if (!function_7af40(&channel->address))
		{
			local_1 = true;
			goto local_2;
		}
local_2:
	return local_1;
}
/* the security code's connect status of an address (unknown_07a9a0.cpp) */
long function_07acf0(const s_type_99af70 *address);

/* the connect status of a channel's address (0 when it has none) */
// @retail 0x78580
long network_observer_channel_connect_status(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	s_type_99af70 *address = &channel->address;
	long result = 0;
	if (function_7af40(address))
		result = function_07acf0(address);
	return result;
}

// @retail 0x78150
long network_observer_scaled_size(s_network_observer *observer, bool flag, real scale)
{
	long count = flag ? observer->configuration->value10c : observer->configuration->value108;
	long result;

	/* rounds as the x87 does (unknown_0259d0's fld/fistp idiom) */
	scale = (real)(count * 8 + 0x168) * scale;
	__asm
	{
		fld scale
		fistp result
	}
	return result;
}

/* the message gateway's outgoing packet (unknown_07b330.cpp) */
struct s_network_message_gateway;
void network_message_gateway_send_pending_messages_to_address(s_network_message_gateway *gateway, s_type_99af70 const *address);

/* forgets a channel's address: closes its connection, flushes the messages
   waiting for the address and, when asked, marks its owner */
// @retail 0x784a0
void network_observer_channel_forget_address(s_network_observer *observer, long channel_index, bool mark_owner, long reason)
{
	long const *reason_reference = &reason;
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state > 2)
			network_connection_close(connection, *reason_reference);
	}
	s_type_99af70 *address = &channel->address;
	if (function_7af40(address))
	{
		network_message_gateway_send_pending_messages_to_address((s_network_message_gateway *)observer->link, address);
		if (mark_owner && channel->owner_index >= 0 && channel->owner_index < MAXIMUM_OBSERVER_OWNERS)
			channel->owner_flags |= 1 << channel->owner_index;
		dword ipv4_address;
		if (function_07aec0(address, &ipv4_address))
			XNetConnect(*(IN_ADDR *)&ipv4_address);
		memset(address, 0, sizeof(*address));
	}
}
/* the quality of service probes (unknown_07b4c0.cpp) */
void qos_release(long handle);

/* closes a channel and clears it */
// @retail 0x76ef0
void network_observer_channel_dispose(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	network_observer_channel_forget_address(observer, channel_index, false, 0xe);
	if (channel->connection_index != NONE)
	{
		network_connection_dispose(function_x7665e0(channel->connection_index));
		channel->connection_index = NONE;
	}
	if (channel->qos_handle != NONE)
	{
		qos_release(channel->qos_handle);
		channel->qos_handle = NONE;
	}
	memset(channel, 0, sizeof(*channel));
	channel->connection_index = NONE;
	channel->qos_handle = NONE;
}

/* disposes every open channel and forgets the owners */
// @retail 0x75a40
void network_observer_dispose_channels(s_network_observer *observer)
{
	for (long channel_index = 0; channel_index < MAXIMUM_OBSERVER_CHANNELS; channel_index++)
	{
		if (observer->channels[channel_index].state != 0)
			network_observer_channel_dispose(observer, channel_index);
	}
	memset(observer->owners, 0, sizeof(observer->owners));
	observer->configuration = NULL;
	observer->unknown0c = NULL;
	observer->unknown04 = NULL;
}

/* a connecting channel whose address stopped connecting starts over */
// @retail 0x76f50
void network_observer_channel_check_connect(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	long state = channel->state;
	if (state == 0)
		return;
	if (state > 0)
	{
		if (state <= 2)
			return;
		if (state == 3)
		{
			long status = network_observer_channel_connect_status(observer, channel_index);
			if (status == 2)
			{
				network_observer_set_channel_state(observer, 4, channel_index);
			}
			else if (status != 1)
			{
				network_observer_channel_forget_address(observer, channel_index, true, 0);
				network_observer_set_channel_state(observer, 2, channel_index);
				channel->attempts++;
			}
			return;
		}
	}
	if (network_observer_channel_connect_status(observer, channel_index) != 2)
	{
		network_observer_channel_forget_address(observer, channel_index, true, 0xd);
		network_observer_set_channel_state(observer, 2, channel_index);
		channel->attempts = 0;
	}
}

/* finds an address for a channel: its current one while it connects, or the
   first of its owners' secure keys not tried yet */
// @retail 0x78330
bool network_observer_channel_find_address(s_network_observer *observer, long channel_index)
{
	volatile bool result = false;
	s_network_observer_channel *channel = &observer->channels[channel_index];
	long status = network_observer_channel_connect_status(observer, channel_index);
	if (status > 0 && status <= 2)
		return true;
	network_observer_channel_forget_address(observer, channel_index, false, 0);
	for (long owner = 0; owner < MAXIMUM_OBSERVER_OWNERS; owner++)
	{
		if ((channel->owner_mask & (1 << owner)) && !(channel->owner_flags & (1 << owner)) &&
			network_observer_get_owner_address(observer, owner, (const XNADDR *)channel->remote_id, &channel->address, &channel->key_index, &channel->id, &channel->key))
		{
			channel->owner_index = owner;
			return true;
		}
	}
	return result;
}
/* the channel that carries a connection (NONE when none does) */
static inline long network_observer_find_channel_by_connection(s_network_observer *observer, long connection_index)
{
	long result = NONE;
	for (long channel_index = 0; channel_index < MAXIMUM_OBSERVER_CHANNELS; channel_index++)
	{
		s_network_observer_channel *channel = &observer->channels[channel_index];
		if (channel->state && channel->connection_index == connection_index)
		{
			result = channel_index;
			break;
		}
	}
	return result;
}

/* retail does not use the channel it finds */
// @retail 0x76640
void s_network_observer::connection_updated(long connection_index, long value)
{
	long channel_index = network_observer_find_channel_by_connection(this, connection_index);
}
/* a packet went out on a connection */
// @retail 0x76670
void s_network_observer::packet_sent(long connection_index, long size, bool flag)
{
	long channel_index = network_observer_find_channel_by_connection(this, connection_index);
	network_statistics_add(&statistics_sent, size);
	if (channels[channel_index].flag48c)
	{
		channels[channel_index].value4a4 += size;
		if (flag)
			channels[channel_index].flag4a1 = true;
	}
}

void function_79660(s_network_observer *observer, long index, long bytes, long delay, long stream_delay);

// @retail 0x76720
void s_network_observer::packet_received(long connection_index, long size, long delay)
{
	long channel_index = network_observer_find_channel_by_connection(this, connection_index);
	s_network_observer_channel *channel = &channels[channel_index];
	s_network_connection *connection = function_x7665e0(channel->connection_index);
	network_statistics_add(&channel->statistics_received, size);
	if (channel->flag48c)
	{
		long stream_delay = 0;
		if (connection->state == 5 && (connection->flags & 8))
			stream_delay = *(long *)((byte *)network_reliable_stream_get(connection->reliable_stream_index) + 0x96c);
		function_79660(this, channel_index, size, delay, stream_delay);
	}
}

void function_797b0(s_network_observer *observer, long index, bool received);
struct s_network_samples;
void network_samples_add(s_network_samples *samples, long value);

// @retail 0x76810
void s_network_observer::connection_v03(long connection_index, bool received, bool flag)
{
	long channel_index = network_observer_find_channel_by_connection(this, connection_index);
	s_network_observer_channel *channel = &channels[channel_index];
	network_samples_add((s_network_samples *)channel->samples250, !received);
	network_samples_add((s_network_samples *)channel->samples360, flag);
	if (channel->flag48c)
		function_797b0(this, channel_index, received);
}

bool function_59670(c_class_58d20 **session);
bool function_596a0(c_class_58d20 **session);
bool voice_member_is_route_target(long member);
bool voice_player_has_channel(long player);
long voice_player_get_bandwidth(long player);

static inline bool observer_voice_enabled(void)
{
	bool result = false;
	if (g_4c9878.initialized && g_476fc8.initialized)
		result = g_4c9878.mode != 0;
	return result;
}

// @retail 0x76520
bool function_76520(long channel_index, bool target, long *bandwidth)
{
	bool result = false;
	c_class_58d20 *session = NULL;
	bool found = false;
	if (observer_voice_enabled())
	{
		switch (g_4c9878.session_kind)
		{
		case 1:
			found = function_59670(&session);
			break;
		case 2:
			found = function_596a0(&session);
			break;
		}
	}
	if (found)
	{
		long member = session->find_member_by_channel(channel_index);
		if (member != NONE)
		{
			if (target)
				result = voice_member_is_route_target(member);
			else if (voice_player_has_channel(member))
			{
				result = true;
				*bandwidth = voice_player_get_bandwidth(member);
			}
		}
	}
	return result;
}

/* whether one of a channel's owners treats it as a host or local channel,
   once its connection has been up long enough */
// @retail 0x77940
bool network_observer_channel_has_host(s_network_observer *observer, long channel_index)
{
	bool result = false;
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state >= 4)
		{
			long time = 0;
			if (connection->state > 2)
				time = connection->timers[1].time;
			if (observer_time_get() - time >= observer->configuration->timeout70)
			{
				for (long owner = 0; owner < MAXIMUM_OBSERVER_OWNERS; owner++)
				{
					if ((channel->owner_mask & (1 << owner)) && observer->owners[owner].active->channel_is_host_or_local(channel_index))
					{
						result = true;
						goto local_0;
					}
				}
			}
			goto local_0;
		}
	}
local_0:
	return result;
}

/* the video refresh rate (unknown_12b070.cpp) */
extern short g_485ac0;

extern s_connection_counter g_4e6398;
void network_samples_reset(s_network_samples *samples);

static inline void observer_statistics_reset(s_network_statistics *statistics);

// @retail 0x76ff0
void network_observer_update_connection(s_network_observer *volatile observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->state)
	{
		s_network_connection *connection = NULL;
		if (channel->connection_index != NONE)
		{
			connection = function_x7665e0(channel->connection_index);
			switch (channel->state)
			{
			case 1: break;
			case 2: break;
			case 3: break;
			case 4: break;
			case 5: break;
			case 6:
			case 8:
				if (connection->state == 5)
					network_observer_set_channel_state(observer, 7, channel_index);
				else if (connection->state <= 2)
				{
					switch (connection->close_reason)
					{
					case 9:
						network_observer_channel_forget_address(observer, channel_index, true, 0);
						network_observer_set_channel_state(observer, 2, channel_index);
						channel->attempts = 0;
						break;
					case 1:
					case 2:
					case 5:
						network_observer_channel_forget_address(observer, channel_index, true, 15);
						network_observer_set_channel_state(observer, 1, channel_index);
						goto state_updated;
					}
					if (channel->state == 6)
					{
						network_observer_set_channel_state(observer, 5, channel_index);
						channel->attempts++;
					}
					else
					{
						network_observer_set_channel_state(observer, 9, channel_index);
						channel->attempts++;
					}
				}
				break;
			case 7:
				if (connection->state != 5)
				{
					if (connection->state > 2)
						network_observer_set_channel_state(observer, 8, channel_index);
					else
					{
						network_observer_set_channel_state(observer, 9, channel_index);
						channel->attempts = 0;
					}
				}
				break;
			case 9: break;
			default: __assume(0);
			}
		}
		else
		{
			network_observer_channel_forget_address(observer, channel_index, true, 0);
			network_observer_set_channel_state(observer, 1, channel_index);
		}
	state_updated:
		if (channel->flags & 2)
		{
			if (!connection || connection->state != 5 || connection->local_sequence != channel->unknown10)
			{
				for (long i = 0; i < MAXIMUM_OBSERVER_OWNERS; i++)
					if (channel->owner_mask & (1 << i))
						observer->owners[i].active->channel_connection_changed(channel_index, channel->unknown10, false);
				channel->flags &= ~2;
				channel->unknown10 = NONE;
			}
		}
		if (!(channel->flags & 2) && channel->state == 7)
		{
			channel->time94 = observer_time_get();
			observer_statistics_reset(&channel->statistics_sent);
			observer_statistics_reset(&channel->statistics_received);
			network_samples_reset((s_network_samples *)channel->samples250);
			network_samples_reset((s_network_samples *)channel->samples360);
			*(long *)((byte *)channel + 0x480) = NONE;
			*(long *)((byte *)channel + 0x488) = NONE;
			*(real *)((byte *)channel + 0x484) = -1.0f;
			*(long *)((byte *)channel + 0x470) = observer_time_get() - 1000;
			long refresh = g_485ac0;
			if (refresh <= 0) refresh = 60;
			union { s_connection_counter parts; __int64 value; } counter;
			counter.parts = g_4e6398;
			*(__int64 *)((byte *)channel + 0x478) = counter.value - refresh;
			channel->flags |= 6;
			channel->unknown10 = connection->local_sequence;
			for (long i = 0; i < MAXIMUM_OBSERVER_OWNERS; i++)
				if (channel->owner_mask & (1 << i))
					observer->owners[i].active->channel_connection_changed(channel_index, channel->unknown10, true);
		}
	}
}

/* the game's frame rate: 25 on a 50 Hz display, otherwise 30 */
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_11c9e0(char *buffer, long maximum_count, const char *format, ...);
long network_connection_allocate(long unused_owner, dword flags);

static inline void observer_statistics_initialize(s_network_statistics *statistics, long interval)
{
	statistics->interval = interval;
	statistics->period = interval / NUMBER_OF_STATISTICS_SAMPLES;
	statistics->rate_scale = 1000.0f / interval;
	observer_statistics_reset(statistics);
}

// @retail 0x76aa0
long network_observer_attach_channel(s_network_observer *observer, long owner_index, const XNADDR *address)
{
	long result = NONE;
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		if (observer->channels[i].state && !memcmp(address, observer->channels[i].remote_id, sizeof(*address)))
		{
			result = i;
			break;
		}
	}
	if (result == NONE)
	{
		long index = NONE;
		for (long j = 0; j < MAXIMUM_OBSERVER_CHANNELS; j++)
		{
			if (!observer->channels[j].state)
			{
				index = j;
				break;
			}
		}
		if (index == NONE)
		{
			for (long j = 0; j < MAXIMUM_OBSERVER_CHANNELS; j++)
			{
				if (observer->channels[j].state && !observer->channels[j].owner_mask)
				{
					index = j;
					s_network_observer_channel *channel = &observer->channels[j];
					network_observer_channel_forget_address(observer, j, false, 0xe);
					if (channel->connection_index != NONE)
					{
						network_connection_dispose(function_x7665e0(channel->connection_index));
						channel->connection_index = NONE;
					}
					if (channel->qos_handle != NONE)
					{
						qos_release(channel->qos_handle);
						channel->qos_handle = NONE;
					}
					memset(channel, 0, sizeof(*channel));
					channel->connection_index = NONE;
					channel->qos_handle = NONE;
					break;
				}
			}
		}
		if (index != NONE)
		{
			s_network_observer_channel *channel = &observer->channels[index];
			XNADDR remote = *address;
			dword ip = remote.ina.s_addr;
			ip = (((ip & 0xff0000) | (ip >> 16)) >> 8) | (((ip << 16) | (ip & 0xff00)) << 8);
			char name[256];
			function_11c9c0(name, sizeof(name), "%hd.%hd.%hd.%hd", ip >> 24, (ip >> 16) & 255, (ip >> 8) & 255, ip & 255);
			function_11c9e0(name, sizeof(name), "!MAC=%02X:%02X:%02X:%02X:%02X:%02X", remote.abEnet[0], remote.abEnet[1], remote.abEnet[2], remote.abEnet[3], remote.abEnet[4], remote.abEnet[5]);
			long connection_index = network_connection_allocate((long)name, 0x38);
			if (connection_index != NONE)
			{
				channel->connection_index = connection_index;
				function_x7665e0(connection_index)->owner = (c_connection_owner *)observer;
				channel->state = 2;
				channel->time = observer_time_get();
				channel->unknown10 = NONE;
				memcpy(channel->remote_id, address, sizeof(*address));
				channel->time94 = observer_time_get();
				channel->time98 = 0;
				channel->time9c = 0;
				observer_statistics_initialize(&channel->statistics_sent, observer->configuration->statistics_interval);
				observer_statistics_initialize(&channel->statistics_received, observer->configuration->statistics_interval);
				*(long *)channel->samples250 = *(long *)((byte *)observer->configuration + 0xfc);
				network_samples_reset((s_network_samples *)channel->samples250);
				*(long *)channel->samples360 = *(long *)((byte *)observer->configuration + 0xfc);
				network_samples_reset((s_network_samples *)channel->samples360);
				channel->attempts = 0;
				channel->owner_flags = 0;
				channel->owner_index = NONE;
				network_observer_channel_forget_address(observer, index, false, 0);
				result = index;
			}
		}
	}
	if (result != NONE)
	{
		s_network_observer_channel *channel = &observer->channels[result];
		channel->owner_mask |= (byte)(1 << owner_index);
		if (channel->flags & 2)
			observer->owners[owner_index].active->channel_connection_changed(result, channel->unknown10, true);
	}
	return result;
}

static inline real network_frame_rate(void)
{
	long refresh = g_485ac0;
	if (refresh <= 0)
		refresh = 60;
	return refresh == 50 ? 25.0f : 30.0f;
}

/* the first of the configuration's rates (per frame) that a packet of this
   size per frame reaches, else the last one */
// @retail 0x78090
real network_observer_rate_for_size(s_network_observer *observer, long size, bool large, bool limit)
{
	real frame_rate = network_frame_rate();
	long count = large ? observer->configuration->value10c : observer->configuration->value108;
	real ratio = (real)size / (real)(count * 8 + 0x168);
	long rate_count = observer->configuration->rate_count;
	real result = 0.0f;
	for (long i = 0; i < rate_count - 1; i++)
	{
		real rate = observer->configuration->rates[i] * frame_rate;
		if (ratio >= rate)
		{
			result = rate;
			break;
		}
	}
	if (result == 0.0f)
		result = observer->configuration->rates[rate_count - 1] * frame_rate;
	if (limit)
	{
		real maximum = frame_rate * 0.5f;
		if (result > maximum)
			result = maximum;
	}
	return result;
}

/* whether a rate is below the configured rate or the frame rate */
PRIVATE __forceinline bool function_78190(real arg_0, real arg_1, s_network_observer *arg_2, bool arg_3, bool arg_4, bool arg_5)
{
	bool local_0 = false;
	if (arg_3 && arg_1 + 0.0001f < arg_2->configuration->real110 * arg_0)
		local_0 = true;
	if (arg_4)
	{
		if (arg_5)
			arg_0 *= 0.5f;
		if (arg_1 + 0.0001f < arg_0)
			local_0 = true;
	}
	return local_0;
}

// @retail 0x78190
bool network_observer_rate_below(s_network_observer *observer, real rate, bool check_configured, bool check_frame_rate, bool half)
{
	real frame_rate = network_frame_rate();
	return function_78190(frame_rate, rate, observer, check_configured, check_frame_rate, half);
}

/* the smallest of the configuration's rates (per frame) above a minimum, or
   the minimum when none is */
// @retail 0x78210
real network_observer_rate_above(s_network_observer *observer, real minimum, bool limit)
{
	real frame_rate = network_frame_rate();
	real result = FLT_MAX;
	for (long i = observer->configuration->rate_count - 1; i >= 0; i--)
	{
		real rate = frame_rate * observer->configuration->rates[i];
		if (rate > minimum && result > rate)
			result = rate;
	}
	if (result == FLT_MAX)
		result = minimum;
	if (limit)
	{
		real maximum = frame_rate * 0.5f;
		if (result > maximum)
			result = maximum;
	}
	return result;
}

/* sets the observer up: its link, its configuration and its owners and
   channels cleared */
// @retail 0x75970
bool network_observer_initialize(s_network_observer *observer, void *unknown04, s_network_observer_configuration *configuration, void *link, void *unknown0c)
{
	observer->unknown04 = unknown04;
	observer->configuration = configuration;
	observer->unknown0c = unknown0c;
	observer->link = link;
	memset(observer->owners, 0, sizeof(observer->owners));
	network_statistics_initialize(&observer->statistics_sent, observer->configuration->statistics_interval);
	observer->flag4f3c = true;
	observer->flag4f3d = false;
	observer->flag4f3e = false;
	observer->value4f38 = NONE;
	observer->time4f30 = observer_time_get();
	observer->time4f34 = observer_time_get();
	for (long channel_index = 0; channel_index < MAXIMUM_OBSERVER_CHANNELS; channel_index++)
	{
		s_network_observer_channel *channel = &observer->channels[channel_index];
		memset(channel, 0, sizeof(*channel));
		channel->connection_index = NONE;
		channel->qos_handle = NONE;
	}
	observer->flag4e00 = false;
	return true;
}

/* counts the periods the traffic stayed over or under the configured share
   of the bandwidth, and tracks the largest and the smoothed value */
// @retail 0x77f90
void network_observer_update_bandwidth(s_network_observer *observer, long count, long value, long size)
{
	if (count > 0)
	{
		bool over;
		if ((real)size > (real)count * observer->configuration->real1ac)
			over = true;
		else
			over = false;
		if (observer->value4e10 == NONE || observer->flag4e14 != over)
		{
			observer->flag4e14 = over;
			observer->value4e10 = observer_time_get();
		}
		if (observer_time_since(observer->value4e10) >= observer->configuration->time1b0)
		{
			observer->counts4e18[observer->flag4e14]++;
			observer->value4e10 = observer_time_get();
		}
	}
	if (value > observer->value4e08)
		observer->value4e08 = value;
	if (observer->value4e0c != NONE && value > observer->value4e0c)
		observer->value4e0c += (value - observer->value4e0c) >> observer->configuration->shift1b4;
}

/* clears a direction's traffic (as unknown_092870.cpp's helper) */
static inline void observer_statistics_reset(s_network_statistics *statistics)
{
	statistics->packets = 0;
	statistics->bytes = 0;
	statistics->period_start = 0;
	memset(&statistics->current, 0, sizeof(statistics->current));
	statistics->sample_index = 0;
	memset(statistics->samples, 0, sizeof(statistics->samples));
	memset(&statistics->total, 0, sizeof(statistics->total));
}

static inline void observer_channel_clear_flag48c(s_network_observer_channel *channel)
{
	if (channel->flag48c)
		channel->flag48c = false;
}

/* starts the bandwidth estimate over */
// @retail 0x75af0
void network_observer_reset_bandwidth(s_network_observer *observer)
{
	observer->value4e20 = observer->configuration->value138;
	observer->value4e24 = 0x400;
	observer->flag4e14 = false;
	observer->value4e08 = NONE;
	observer->value4e0c = NONE;
	observer->value4e10 = NONE;
	memset(observer->counts4e18, 0, sizeof(observer->counts4e18));
	observer_statistics_reset(&observer->statistics_sent);
	observer->value4e28 = observer->value4e04 / 1024 - observer->configuration->value134;
	observer->value4e28 = observer->value4e28 > observer->value4e20 ? observer->value4e28 : observer->value4e20;
	observer->time4f08 = observer_time_get();
	observer->time4f0c = observer_time_get();
	observer->value4f10 = 0;
	observer->value4f14 = 0;
	observer->flag4f18 = false;
	observer->value4f1c = 0;
	observer->value4f20 = 0;
	observer->real4f24 = 0.0f;
	observer->real4f28 = 0.0f;
	observer->time4f30 = NONE;
	observer->time4f34 = NONE;
	observer->value4f38 = NONE;
	observer->flag4f3c = true;
	observer->time4f2c = observer_time_get();
	volatile s_network_observer_channel *local_0 = &observer->channels[0];
	long local_1 = MAXIMUM_OBSERVER_CHANNELS;
	do
	{
		if (local_0->state && local_0->flag48c)
			local_0->flag48c = false;
		local_0++;
	} while (--local_1);
}

/* a probe target (as unknown_07b4c0.cpp's) */
struct s_qos_target
{
	XNKID kid;
	XNKEY key;
	XNADDR xna;
};

/* src/unknown_07b4c0.cpp */
long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets);
bool qos_is_complete(long handle);

/* probes a channel's machine once, with the first of its owners' secure
   keys, and keeps the result */
// @retail 0x77480
void network_observer_channel_probe(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->state && channel->qos_handle == NONE && !(channel->flags & 8))
	{
		for (long owner = 0; owner < MAXIMUM_OBSERVER_OWNERS; owner++)
		{
			if ((channel->owner_mask & (1 << owner)) && observer->owners[owner].key_index != NONE)
			{
				s_qos_target target;
				*(s_network_session_id *)&target.kid = observer->owners[owner].id;
				target.key = *(XNKEY *)observer->owners[owner].key;
				target.xna = *(XNADDR *)channel->remote_id;
				channel->qos_handle = qos_lookup(0, 1, NONE, &target);
				break;
			}
		}
	}
	long handle = channel->qos_handle;
	if (handle != NONE && qos_is_complete(handle))
	{
		s_qos_result *result = &channel->field_x31a738;
		s_qos_result *const *local_0 = &result;
		if (qos_target_result(handle, *local_0, 0))
		{
			channel->flags |= 0x10;
			channel->field_x31a738.data = NULL;
			channel->field_x31a738.data_size = 0;
		}
		qos_release(channel->qos_handle);
		channel->qos_handle = NONE;
		channel->flags |= 8;
	}
}

/* A local view of the channel's measurement and probe fields. */
struct s_observer_bandwidth_channel
{
	long state;
	byte unknown04[0x250 - 4];
	long sample_count;
	byte unknown254[0x358 - 0x254];
	long sample_losses;
	byte unknown35c[0x48c - 0x35c];
	bool active;
	bool wanted;
	bool has_callback;
	bool callback_active;
	bool callback_inactive;
	byte unknown491[3];
	long budget;
	long burst;
	real rate;
	bool rate_limited;
	bool budget_limited;
	bool burst_limited;
	byte unknown4a3;
	long sent_bytes;
	long received_bytes;
	long stream_delay;
	long smoothed_delay;
	long smoothed_interval;
	long loss_count;
	dword loss_window;
	bool backoff;
	byte unknown4c1[3];
	long backoff_time;
	long backoff_delay;
	long backoff_budget;
	long backoff_burst;
	real backoff_rate;
	long loss_penalty;
	long probe_state;
	long probe_reset_time;
	long sample_time;
	long probe_time;
	long probe_failures;
	long baseline_delay;
	bool probe_pending;
	byte unknown4f5[3];
	long saved_budget;
	long saved_burst;
	real saved_rate;
	long probe_delay;
	long probe_received_rate;
	bool probe_budget_limited;
	bool probe_rate_limited;
	bool probe_burst_limited;
	byte unknown50f;
	long measured_sent_rate;
	long measured_received_rate;
	long constrained_cycles;
	long unconstrained_cycles;
};

void function_79c00(s_network_observer *observer, long index, bool pass_on);

// @retail 0x79a10
void function_79a10(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	long elapsed = NONE;
	long last = observer->time4f34;
	if (last != NONE)
		elapsed = observer_time_get() - last;
	if (elapsed == NONE || elapsed >= *(long *)((byte *)observer->configuration + 0x1bc))
	{
		bool has_callback = channel->has_callback;
		bool selected_callback = has_callback;
		long selected = NONE;
		long maximum = channel->measured_received_rate > channel->measured_sent_rate ? channel->measured_received_rate : channel->measured_sent_rate;
		maximum += *(long *)((byte *)observer->configuration + 0x1b8);
		for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
		{
			s_observer_bandwidth_channel *candidate = (s_observer_bandwidth_channel *)&observer->channels[i];
			if (candidate->state && candidate->active)
			{
				long rate = candidate->measured_received_rate > candidate->measured_sent_rate ? candidate->measured_received_rate : candidate->measured_sent_rate;
				if (rate > maximum)
				{
					selected_callback = candidate->has_callback;
					selected = i;
					maximum = rate;
				}
			}
		}
		if (selected != NONE && (!selected_callback || has_callback))
		{
			function_79c00(observer, selected, false);
			observer->time4f34 = observer_time_get();
		}
	}
}

void function_79600(s_network_observer *observer, long index, long budget, long burst, real rate);
void function_79d90(s_network_observer *observer, long index);
void function_7a110(s_network_observer *observer, long index);

static __forceinline long observer_round(real value)
{
	long result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

// @retail 0x79850
void function_79850(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	channel->backoff = true;
	channel->backoff_budget = channel->budget;
	channel->backoff_burst = channel->burst;
	channel->backoff_rate = channel->rate;
	channel->backoff_delay = channel->smoothed_delay;
	channel->backoff_time = observer_time_get();
	channel->probe_reset_time = channel->backoff_time;
	if (channel->probe_state == 2 || channel->probe_state == 3)
		function_7a110(observer, index);
	long budget = observer_round(channel->budget * *(real *)((byte *)observer->configuration + 0x180));
	long burst = observer_round(channel->burst * *(real *)((byte *)observer->configuration + 0x180));
	long minimum_budget = *(long *)((byte *)observer->configuration + 0x14c);
	long minimum_burst = *(long *)((byte *)observer->configuration + 0xe0);
	if (budget <= minimum_budget) budget = minimum_budget;
	if (burst <= minimum_burst) burst = minimum_burst;
	real rate = network_observer_rate_for_size(observer, budget, channel->has_callback, channel->callback_inactive);
	long delay = channel->baseline_delay;
	long intervals = real_truncate(delay * rate * 0.001f) + 1;
	long local_0 = intervals + 1;
	long local_1 = delay * budget / (intervals * 8000);
	long limited_burst = local_0 * local_1;
	long local_2 = *(volatile long *)((byte *)observer->configuration + 0xe0);
	limited_burst = local_2 > limited_burst ? local_2 : limited_burst;
	if (burst > limited_burst) burst = limited_burst;
	function_79600(observer, index, budget, burst, rate);
	channel->loss_penalty += *(long *)((byte *)observer->configuration + 0x184);
	if (channel->loss_penalty >= *(long *)((byte *)observer->configuration + 0x188))
	{
		function_79c00(observer, index, false);
		channel->loss_penalty = 0;
	}
}

// @retail 0x797b0
void function_797b0(s_network_observer *observer, long index, bool received)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	dword oldest = 1 << (*(long *)((byte *)observer->configuration + 0x174) - 1);
	if (channel->loss_window & oldest) channel->loss_count--;
	channel->loss_window = (channel->loss_window & ~oldest) << 1;
	if (!received)
	{
		channel->loss_window |= 1;
		channel->loss_count++;
		if ((real)channel->loss_count >=
			(real)*(long *)((byte *)observer->configuration + 0x174) * *(real *)((byte *)observer->configuration + 0x178))
		{
			function_79850(observer, index);
			channel->loss_count = 0;
			channel->loss_window = 0;
		}
	}
}

// @retail 0x78e60
void function_78e60(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	s_network_connection *connection = function_x7665e0(observer->channels[index].connection_index);
	long total = 0;
	long count = 0;
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_observer_bandwidth_channel *other = (s_observer_bandwidth_channel *)&observer->channels[i];
		if (other->state && other->active)
		{
			count++;
			total += other->budget;
		}
	}
	long budget = *(long *)((byte *)observer->configuration + 0x158);
	long requested = budget + total;
	long limit;
	long available;
	if (observer->unknown4e01[0])
	{
		limit = *(long *)((byte *)observer->configuration + 0x160);
		available = observer->value4e04 * 3 / 4;
	}
	else
	{
		limit = *(long *)((byte *)observer->configuration + 0x15c);
		available = observer->value4e04 / 2;
	}
	if (available <= limit) limit = available;
	long last = observer->time4f2c;
	if (observer_time_get() - last <= *(long *)((byte *)observer->configuration + 0x154))
	{
		long initial = (count + 1) * *(long *)((byte *)observer->configuration + 0x164);
		if (limit <= initial) limit = initial;
	}
	if (requested > limit)
	{
		long pool = total > limit ? total : limit;
		budget = pool / (count + 1);
		long floor = *(long *)((byte *)observer->configuration + 0x14c);
		if (budget <= floor) budget = floor;
		if (count > 0 && (real)total > 0.0f)
		{
			real scale = (real)(pool - budget) / (real)total;
			for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
			{
				s_observer_bandwidth_channel *other = (s_observer_bandwidth_channel *)&observer->channels[i];
				if (other->state && other->active)
				{
					long reduced = observer_round(other->budget * scale);
					long burst = observer_round(other->burst * scale);
					long minimum = *(long *)((byte *)observer->configuration + 0x14c);
					long minimum_burst = *(long *)((byte *)observer->configuration + 0xe0);
					if (reduced <= minimum) reduced = minimum;
					if (burst <= minimum_burst) burst = minimum_burst;
					real rate = network_observer_rate_for_size(observer, reduced, channel->has_callback, channel->callback_inactive);
					function_79600(observer, i, reduced, burst, rate);
				}
			}
		}
	}
	memset(&channel->active, 0, 0x94);
	channel->active = true;
	channel->wanted = false;
	channel->has_callback = connection->callback != NULL;
	channel->callback_active = connection->callback && connection->callback->active;
	channel->callback_inactive = connection->callback && !connection->callback->active;
	channel->probe_reset_time = NONE;
	channel->sample_time = NONE;
	channel->probe_time = NONE;
	channel->probe_state = 0;
	long delay = 0;
	if (connection->state == 5 && (connection->flags & 8))
		delay = *(long *)((byte *)network_reliable_stream_get(connection->reliable_stream_index) + 0x96c);
	channel->baseline_delay = delay;
	long minimum_delay = *(long *)((byte *)observer->configuration + 0x16c);
	if (minimum_delay < delay) minimum_delay = delay;
	channel->baseline_delay = minimum_delay;
	channel->probe_failures = 0;
	channel->smoothed_delay = minimum_delay;
	channel->smoothed_interval = 0;
	real rate = network_observer_rate_for_size(observer, budget, channel->has_callback, channel->callback_inactive);
	long intervals = real_truncate(minimum_delay * rate * 0.001f) + 1;
	long burst = (intervals + 1) * (minimum_delay * budget / (intervals * 8000));
	long minimum_burst = *(long *)((byte *)observer->configuration + 0xe0);
	if (burst <= minimum_burst) burst = minimum_burst;
	real frame_rate = network_frame_rate();
	bool limited = false;
	if (channel->wanted && rate + 0.0001f < observer->configuration->real110 * frame_rate)
		limited = true;
	if (channel->has_callback)
	{
		if (channel->callback_inactive) frame_rate *= 0.5f;
		if (rate + 0.0001f < frame_rate) limited = true;
	}
	channel->budget = budget;
	channel->rate = rate;
	channel->rate_limited = limited;
	channel->burst = burst;
}

// @retail 0x79c00
void function_79c00(s_network_observer *observer, long index, bool pass_on)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	if (pass_on)
		function_79a10(observer, index);
	if (channel->probe_pending)
		function_7a110(observer, index);
	channel->probe_failures = 0;
	long old_budget, old_burst;
	if (channel->backoff)
	{
		old_budget = channel->backoff_budget;
		old_burst = channel->backoff_burst;
	}
	else
	{
		old_budget = channel->budget;
		old_burst = channel->burst;
	}
	long reduction = observer_round(old_budget * *(real *)((byte *)observer->configuration + 0x198));
	long cap = *(long *)((byte *)observer->configuration + 0x194);
	if (reduction > cap)
		reduction = cap;
	long remaining = old_budget - reduction;
	long budget = *(long *)((byte *)observer->configuration + 0x14c);
	if (remaining > budget)
		budget = remaining;
	long burst = budget * old_burst / old_budget;
	long floor = *(long *)((byte *)observer->configuration + 0xe0);
	if (burst <= floor)
		burst = floor;
	real rate = network_observer_rate_for_size(observer, budget, channel->has_callback, channel->callback_inactive);
	if (channel->backoff)
	{
		long limited_budget = channel->budget <= budget ? channel->budget : budget;
		long limited_burst = channel->burst > burst ? burst : channel->burst;
		real limited_rate = channel->rate > rate ? rate : channel->rate;
		function_79600(observer, index, limited_budget, limited_burst, (real)(long)limited_rate);
		channel->backoff_budget = budget;
		channel->backoff_burst = burst;
		channel->backoff_rate = rate;
	}
	else
		function_79600(observer, index, budget, burst, rate);
	observer->flag4f3c = true;
	observer->flag4f3d = true;
	if (pass_on)
		observer->flag4f3e = true;
	function_79d90(observer, index);
}

// @retail 0x7a160
void function_7a160(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	channel->probe_failures++;
	if (channel->probe_failures >= *(long *)((byte *)observer->configuration + 0x1e4))
		channel->baseline_delay += *(long *)((byte *)observer->configuration + 0x1e8);
	else if (channel->probe_failures % *(long *)((byte *)observer->configuration + 0x1e0) == 0)
		function_79c00(observer, index, false);
}

long function_79560(s_network_observer *observer, long index);

// @retail 0x7a1c0
long function_7a1c0(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	long started = observer->value4f38;
	if (observer_time_get() - started < *(long *)((byte *)observer->configuration + 0x1a0))
		return 1;
	long received = function_79560(observer, index);
	long tolerance;
	if (channel->has_callback)
		tolerance = *(long *)((byte *)observer->configuration + 0x1d8);
	else
		tolerance = *(long *)((byte *)observer->configuration + 0x1d4);
	bool acceptable = channel->smoothed_delay - channel->smoothed_interval <= channel->baseline_delay + tolerance;
	if (channel->smoothed_delay > channel->probe_delay + tolerance)
		return 2;
	if (received > channel->probe_received_rate && acceptable)
		return 0;
	long result = 1;
	if (!acceptable)
		function_7a160(observer, index);
	return result;
}

// @retail 0x79560
long function_79560(s_network_observer *observer, long index)
{
	long result = 0;
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	long started = observer->value4f38;
	long elapsed = observer_time_get() - started;
	if (elapsed > 0)
		result = channel->received_bytes * 8000 / elapsed;
	return result;
}

// @retail 0x795b0
long function_795b0(s_network_observer *observer, long index)
{
	long result = 0;
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	long started = observer->value4f38;
	long elapsed = observer_time_get() - started;
	if (elapsed > 0)
		result = channel->sent_bytes * 8000 / elapsed;
	return result;
}

// @retail 0x79de0
bool function_79de0(s_network_observer *observer, long index, bool *exhausted_out)
{
	long budget_limit = 0;
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	bool exhausted = false;
	bool changed = false;
	long increase = observer_round(channel->budget * *(real *)((byte *)observer->configuration + 0x190));
	long step = *(long *)((byte *)observer->configuration + 0x18c);
	if (increase <= step) step = increase;
	long cap = *(long *)((byte *)observer->configuration + 0x150);
	budget_limit = channel->budget + step;
	if (budget_limit > cap) budget_limit = cap;
	bool can_raise_budget = budget_limit > channel->budget;
	real amount = 0.0f;
	if (channel->rate > 0.0f) amount = (real)channel->budget / (channel->rate * 8.0f);
	long burst_limit = channel->burst + (long)amount;
	bool can_raise_burst = burst_limit > channel->burst;
	real rate_limit = network_observer_rate_above(observer, channel->rate, channel->callback_inactive);
	bool can_raise_rate = rate_limit - channel->rate > 0.0001f;
	long started = observer->value4f38;
	long elapsed = observer_time_get() - started;
	if (!can_raise_budget && !can_raise_burst && !can_raise_rate)
		exhausted = true;
	else if (elapsed >= *(long *)((byte *)observer->configuration + 0x1a0))
	{
		if (*(long *)((byte *)observer + 0x4f40) >= *(long *)((byte *)observer + 0x4f44))
		{
			if (*(bool *)((byte *)observer->configuration + 0x144))
			{
				function_795b0(observer, index);
				function_79560(observer, index);
			}
		}
		else
		{
			real rate = channel->rate;
			long budget = channel->budget;
			long burst = channel->burst;
			if (can_raise_burst && channel->burst_limited)
			{
				burst = burst_limit;
				changed = true;
			}
			else if (can_raise_budget && channel->budget_limited)
			{
				budget = budget_limit;
				changed = true;
			}
			else if (can_raise_rate && channel->rate_limited)
			{
				rate = rate_limit;
				changed = true;
			}
			if (rate > channel->rate)
			{
				real required = (real)network_observer_scaled_size(observer, channel->has_callback, rate);
				real current = (real)budget;
				budget = (long)(current > required ? current : required);
			}
			if (budget > channel->budget)
			{
				long scaled = channel->burst * budget / channel->budget;
				if (burst <= scaled) burst = scaled;
			}
			if (changed)
			{
				function_795b0(observer, index);
				long received = function_79560(observer, index);
				channel->probe_pending = true;
				channel->saved_budget = channel->budget;
				channel->saved_burst = channel->burst;
				channel->saved_rate = channel->rate;
				channel->probe_delay = channel->smoothed_delay;
				channel->probe_received_rate = received;
				channel->probe_budget_limited = false;
				channel->probe_rate_limited = false;
				channel->probe_burst_limited = false;
				function_79600(observer, index, budget, burst, rate);
				channel->probe_state = 2;
				long active = *(long *)((byte *)observer + 0x4f40);
				observer->flag4f3c = true;
				*(long *)((byte *)observer + 0x4f40) = active + 1;
			}
			channel->probe_time = observer_time_get();
		}
	}
	*exhausted_out = exhausted;
	return changed;
}

// @retail 0x79d90
void function_79d90(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	if (channel->probe_state != 0)
	{
		channel->probe_state = 0;
		channel->probe_reset_time = observer_time_get();
	}
}

// @retail 0x78a10
bool function_78a10(long index, s_network_observer *observer, long *delay, real *rate, long *received_rate, long *loss_percent)
{
	bool result = false;
	if (index >= 0 && index < MAXIMUM_OBSERVER_CHANNELS)
	{
		s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
		if (channel->state && channel->active)
		{
			*delay = channel->smoothed_delay;
			*rate = channel->rate;
			*received_rate = channel->measured_received_rate;
			*loss_percent = (long)((real)channel->sample_losses / (real)channel->sample_count * 100.0f);
			result = true;
		}
	}
	return result;
}

// @retail 0x79600
void function_79600(s_network_observer *observer, long index, long budget, long burst, real rate)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	bool limited = network_observer_rate_below(observer, rate, channel->wanted, channel->has_callback, channel->callback_inactive);
	channel->budget = budget;
	channel->rate = rate;
	channel->rate_limited = limited;
	channel->burst = burst;
}

// @retail 0x7a2a0
real function_7a2a0(s_network_observer *observer, long index)
{
	real result = 0.0f;
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	if (channel->probe_state == 1)
	{
		if (channel->probe_time != NONE)
		{
			long last = channel->probe_time;
			result = (real)((observer_time_get() - last) * *(long *)((byte *)observer->configuration + 0x1cc));
			if (result > *(real *)((byte *)observer->configuration + 0x1c8))
				result = *(real *)((byte *)observer->configuration + 0x1c8);
		}
		else
			result = *(real *)((byte *)observer->configuration + 0x1c8);
		result -= (real)channel->budget * *(real *)((byte *)observer->configuration + 0x1d0);
	}
	return result;
}

// @retail 0x7a110
void function_7a110(s_network_observer *observer, long index)
{
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
	if (channel->probe_pending)
	{
		function_79600(observer, index, channel->saved_budget, channel->saved_burst, channel->saved_rate);
		channel->probe_pending = false;
		channel->probe_state = 1;
	}
}

struct s_7a330
{
	byte field_0[0xa8];
	s_observer_bandwidth_channel field_a8;
};

// @retail 0x7a330
void function_7a330(s_network_observer *observer, long index)
{
	long const *index_reference = &index;
	s_7a330 *local_0 = (s_7a330 *)((byte *)observer + *index_reference * sizeof(s_network_observer_channel));
	if (local_0->field_a8.probe_state == 0)
	{
		long elapsed = 0;
		long last = local_0->field_a8.probe_reset_time;
		if (last != NONE) elapsed = observer_time_get() - last;
		if (local_0->field_a8.probe_reset_time == NONE || elapsed >= *(long *)((byte *)observer->configuration + 0x1f0))
		{
			if (local_0->field_a8.constrained_cycles >= *(long *)((byte *)observer->configuration + 0x1ec))
			{
				local_0->field_a8.probe_state = 1;
				observer->flag4f3c = true;
			}
		}
	}
	else if (local_0->field_a8.unconstrained_cycles > 0)
		function_79d90(observer, index);
	else if (local_0->field_a8.probe_state == 2)
	{
		switch (function_7a1c0(observer, index))
		{
		case 0:
			local_0->field_a8.probe_state = 3;
			break;
		case 1:
			function_7a110(observer, index);
			break;
		default:
			function_79c00(observer, index, true);
			break;
		}
	}
	else
	{
		long tolerance;
		if (local_0->field_a8.has_callback) tolerance = *(long *)((byte *)observer->configuration + 0x1d8);
		else tolerance = *(long *)((byte *)observer->configuration + 0x1d4);
		bool exhausted = false;
		if (!local_0->field_a8.backoff && local_0->field_a8.loss_penalty <= 0)
		{
			if (local_0->field_a8.smoothed_delay - local_0->field_a8.smoothed_interval > local_0->field_a8.baseline_delay + tolerance)
				function_7a160(observer, index);
			else if (!function_79de0(observer, index, &exhausted) && exhausted)
				function_79d90(observer, index);
		}
	}
}

// @retail 0x79480
void function_79480(s_network_observer *observer)
{
	s_network_observer *const *observer_reference = &observer;
	long now = observer_time_get();
	observer_time_get();
	for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
	{
		s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&(*observer_reference)->channels[index];
		if (channel->state && channel->active)
		{
			long sent_rate = function_795b0(*observer_reference, index);
			function_79560(*observer_reference, index);
			long delay = channel->smoothed_delay;
			if (sent_rate > 0)
			{
				channel->probe_budget_limited |= channel->budget_limited;
				channel->probe_rate_limited |= channel->rate_limited;
				channel->probe_burst_limited |= channel->burst_limited;
			}
			if (channel->probe_budget_limited || channel->probe_rate_limited || channel->probe_burst_limited)
			{
				channel->constrained_cycles++;
				channel->unconstrained_cycles = 0;
			}
			else
			{
				channel->unconstrained_cycles++;
				channel->constrained_cycles = 0;
			}
			if (delay < channel->baseline_delay)
			{
				channel->baseline_delay = delay;
				channel->probe_failures = 0;
			}
			channel->received_bytes = 0;
			channel->sent_bytes = 0;
			channel->budget_limited = false;
			channel->burst_limited = false;
		}
	}
	observer->value4f38 = now;
}

// @retail 0x79660
void function_79660(s_network_observer *observer, long index, long bytes, long delay, long stream_delay)
{
	s_network_observer *const *observer_reference = &observer;
	long const *index_reference = &index;
	long const *stream_delay_reference = &stream_delay;
	s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&(*observer_reference)->channels[*index_reference];
	long local_0 = *(volatile long *)&channel->received_bytes;
	channel->received_bytes = bytes + local_0;
	channel->stream_delay = *stream_delay_reference;
	long interval = 0;
	long last = channel->sample_time;
	if (last != NONE)
		interval = observer_time_get() - last;
	channel->sample_time = observer_time_get();
	channel->smoothed_delay += (delay - channel->smoothed_delay) >> *(long *)((byte *)observer->configuration + 0x170);
	channel->smoothed_interval += (interval - channel->smoothed_interval) >> *(long *)((byte *)observer->configuration + 0x170);
	if (channel->backoff)
	{
		long delay = channel->smoothed_delay > channel->backoff_delay ? channel->smoothed_delay : channel->backoff_delay;
		long started = channel->backoff_time;
		if (observer_time_get() - started >= *(long *)((byte *)observer->configuration + 0x17c) * delay)
		{
			function_79600(observer, index, channel->backoff_budget, channel->backoff_burst, channel->backoff_rate);
			channel->backoff = false;
		}
	}
	else if (channel->loss_penalty > 0)
		channel->loss_penalty--;
}

// @retail 0x79260
void function_79260(s_network_observer *observer)
{
	long count;
	long total;
	long started = observer->value4f38;
	long elapsed = observer_time_get() - started;
	if (observer->flag4f3c || elapsed >= *(long *)((byte *)observer->configuration + 0x19c))
	{
		if (observer->flag4f3d)
		{
			for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
			{
				s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
				if (channel->state && channel->active && (channel->probe_state == 2 || channel->probe_state == 3))
					function_7a110(observer, index);
			}
		}
		else if (elapsed >= *(long *)((byte *)observer->configuration + 0x1a4))
		{
			long penalized = 0;
			count = 0;
			long qualifying = 0;
			total = 0;
			for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
			{
				s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
				if (channel->state && channel->active)
				{
					long sent = function_795b0(observer, index);
					long received = function_79560(observer, index);
					bool enough = !channel->budget_limited || received >= *(long *)((byte *)observer->configuration + 0x1a8);
					count++;
					penalized += channel->loss_penalty > 0;
					if (enough && !channel->rate_limited)
						qualifying++;
					channel->measured_sent_rate = sent;
					channel->measured_received_rate = received;
					total += received;
				}
			}
			if (penalized == 0)
				network_observer_update_bandwidth(observer, count, total, qualifying);
		}
		if (observer->flag4f3e)
		{
			count = 0;
			total = 0;
			for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
			{
				s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
				if (channel->state && channel->active)
				{
					count++;
					long rate = 0;
					long start = observer->value4f38;
					long age = observer_time_get() - start;
					if (age > 0)
						rate = channel->sent_bytes * 8000 / age;
					total += rate;
				}
			}
			if (count > 0)
			{
				if (observer->value4e0c != NONE)
					total = observer->value4e0c + ((total - observer->value4e0c) >> observer->configuration->shift1b4);
				observer->value4e0c = total;
			}
		}
		function_79480(observer);
	}
	observer->flag4f3c = false;
	observer->flag4f3d = false;
	observer->flag4f3e = false;
}

// @retail 0x7a740
void function_7a740(s_network_observer *observer)
{
	long upper = 0;
	long lower = 0;
	bool found_lower = false;
	bool found_upper = false;
	s_network_observer *const *observer_reference = &observer;
	for (long index = 16; index > 0; index--)
	{
		if (!found_upper && (*observer_reference)->value4e04 >= g_network_configuration.levels[1].values[index])
		{
			upper = index;
			found_upper = true;
		}
		if (!found_lower && (*observer_reference)->value4e04 >= g_network_configuration.levels[0].values[index])
		{
			lower = index;
			found_lower = true;
		}
	}
	if (g_4c9878.initialized)
	{
		g_4c9878.unknown08 = upper;
		g_4c9878.unknown0c = lower;
	}
}

// @retail 0x7a4a0
void function_7a4a0(s_network_observer *observer)
{
	bool backoff = false;
	bool active_callback = false;
	bool limited_callback = false;
	long mode = 0;
	for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
	{
		s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[index];
		if (channel->state && channel->active)
		{
			backoff |= channel->backoff;
			active_callback |= channel->has_callback && channel->callback_active;
			limited_callback |= channel->has_callback && (channel->probe_budget_limited || channel->probe_burst_limited);
		}
	}
	if (active_callback)
		mode = 1 + (backoff != false);
	else if (g_network_configuration.flag16fc && (backoff || limited_callback))
		mode = 1;
	if (g_4c9878.initialized)
		g_4c9878.unknown04 = mode;
}


typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_sort_4byte_compare_function compare, const void *context);

// @retail 0x78a90
bool __stdcall function_78a90(long first, long second, const void *context)
{
	const real *values = (const real *)context;
	return values[first] < values[second];
}

// @retail 0x78ac0
void function_78ac0(s_network_observer *observer)
{
	bool enabled = observer->flag4e00;
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *base = &observer->channels[i];
		s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)base;
		if (channel->state)
		{
			s_network_connection *connection = function_x7665e0(base->connection_index);
			if (channel->active && (!enabled || connection->state != 5 ||
				channel->has_callback != (connection->callback != NULL) ||
				channel->callback_active != (connection->callback && connection->callback->active)))
			{
				channel->active = false;
				observer->flag4f3c = true;
			}
			if (connection->state == 5 && enabled && !channel->active)
			{
				function_78e60(observer, i);
				observer->flag4f3c = true;
			}
			if (channel->active)
			{
				bool wanted = false;
				for (long owner = 0; owner < MAXIMUM_OBSERVER_OWNERS; owner++)
				{
					if ((base->owner_mask & (1 << owner)) && observer->owners[owner].active->channel_is_trusted(i))
					{
						wanted = true;
						break;
					}
				}
				c_class_58d20 *session = NULL;
				bool found = false;
				if (observer_voice_enabled())
				{
					switch (g_4c9878.session_kind)
					{
					case 1: found = function_59670(&session); break;
					case 2: found = function_596a0(&session); break;
					}
				}
				if (found)
				{
					long member = session->find_member_by_channel(i);
					if (member != NONE && voice_member_is_route_target(member)) wanted = true;
				}
				if (wanted != channel->wanted) channel->wanted = wanted;
			}
		}
	}
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[i];
		if (channel->state && channel->active && channel->probe_pending)
		{
			long tolerance = *(long *)((byte *)observer->configuration + (channel->has_callback ? 0x1dc : 0x1d4));
			if (channel->smoothed_delay - channel->smoothed_interval > channel->probe_delay + tolerance)
				function_79c00(observer, i, true);
		}
	}
	long last = observer->time4f30;
	if (observer_time_get() - last >= *(long *)((byte *)observer->configuration + 0x148))
	{
		real values[MAXIMUM_OBSERVER_CHANNELS];
		long indices[MAXIMUM_OBSERVER_CHANNELS];
		long count = 0;
		long scratch;
		memset(values, 0, sizeof(values));
		*(long *)((byte *)observer + 0x4f40) = 0;
		for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
		{
			s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[i];
			if (channel->state && channel->active)
			{
				indices[count++] = i;
				values[i] = function_7a2a0(observer, i);
			}
		}
		long maximum = observer_round(count * *(real *)((byte *)observer->configuration + 0x1c4));
		*(long *)((byte *)observer + 0x4f44) = maximum;
		if (maximum < 1) maximum = 1;
		else if (maximum > *(long *)((byte *)observer->configuration + 0x1c0))
			maximum = *(long *)((byte *)observer->configuration + 0x1c0);
		*(long *)((byte *)observer + 0x4f44) = maximum;
		sort_4byte(indices, count, &scratch, function_78a90, values);
		for (long i = 0; i < count; i++) function_7a330(observer, indices[i]);
		function_79260(observer);
		for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
		{
			s_observer_bandwidth_channel *channel = (s_observer_bandwidth_channel *)&observer->channels[i];
			if (channel->state && channel->active && channel->probe_state == 3)
			{
				channel->probe_state = 1;
				channel->probe_failures = 0;
			}
		}
		observer->time4f30 = observer_time_get();
	}
	function_79260(observer);
}

// @retail 0x81440
s_network_observer::s_network_observer()
{
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		channels[i].statistics_sent.interval = 0;
		channels[i].statistics_sent.period = 0;
		channels[i].statistics_sent.rate_scale = 0.0f;
		channels[i].statistics_received.interval = 0;
		channels[i].statistics_received.period = 0;
		channels[i].statistics_received.rate_scale = 0.0f;
		*(long *)channels[i].samples250 = 0;
		*(long *)channels[i].samples360 = 0;
	}
	*(volatile long *)&statistics_sent.interval = 0;
	*(volatile long *)&statistics_sent.period = 0;
	*(volatile real *)&statistics_sent.rate_scale = 0.0f;
	*(void *volatile *)&unknown04 = 0;
	*(void *volatile *)&unknown0c = 0;
	memset(owners, 0, sizeof(owners));
}

static inline real observer_budget_rate(s_network_observer *observer, long size, bool limit)
{
	real frame_rate = network_frame_rate();
	long count = observer->configuration->value10c;
	real ratio = (real)size / (real)(count * 8 + 0x168);
	long rate_count = observer->configuration->rate_count;
	real result = 0.0f;
	for (long i = 0; i < rate_count - 1; i++)
	{
		real rate = observer->configuration->rates[i] * frame_rate;
		if (ratio >= rate)
		{
			result = rate;
			break;
		}
	}
	if (result == 0.0f) result = observer->configuration->rates[rate_count - 1] * frame_rate;
	if (limit && result > frame_rate * 0.5f) result = frame_rate * 0.5f;
	return result;
}

// @retail 0x779f0
void network_observer_update_channel_budgets(s_network_observer *observer)
{
	if (observer_time_since(observer->time4f08) >= *(long *)((byte *)observer->configuration + 0x118))
	{
		bool congested = false;
		if (observer->flag4e00)
		{
			long count = 0;
			long losses = 0;
			for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
			{
				s_network_observer_channel *channel = &observer->channels[i];
				if (channel->state == 7 && *(long *)((byte *)channel + 0x46c) <= *(long *)((byte *)observer->configuration + 0x100))
				{
					count++;
					if ((real)*(long *)((byte *)channel + 0x468) / (real)*(long *)((byte *)channel + 0x360) >=
						*(real *)((byte *)observer->configuration + 0x104)) losses++;
				}
			}
			if (count > 0 && (real)losses / (real)count >= *(real *)((byte *)observer->configuration + 0x11c))
				congested = true;
		}
		if (congested)
		{
			observer->value4f10 = 0;
			long count = observer->value4f14 + 1;
			long limit = *(long *)((byte *)observer->configuration + 0x130);
			observer->value4f14 = count > limit ? limit : count;
		}
		else
		{
			observer->value4f14 = 0;
			long count = observer->value4f10 + 1;
			long limit = *(long *)((byte *)observer->configuration + 0x130);
			observer->value4f10 = count > limit ? limit : count;
		}
		if (observer->flag4e00)
		{
			s_network_statistics *statistics = &observer->statistics_sent;
			network_statistics_update(statistics);
			long rate = real_truncate((real)statistics->total.bytes * statistics->rate_scale * 8.0f * (1.0f / 1024.0f));
			if (observer->value4f14 >= *(long *)((byte *)observer->configuration + 0x120))
			{
				if (observer_time_since(observer->time4f0c) >= *(long *)((byte *)observer->configuration + 0x128))
				{
					long reduced = rate - *(long *)((byte *)observer->configuration + 0x124);
					long budget = observer->value4e20 > reduced ? reduced : observer->value4e20;
					long minimum = observer->configuration->value138;
					budget = budget > minimum ? budget : minimum;
					observer->value4e20 = budget;
					observer->value4e28 = budget;
					observer->value4e24 = observer->value4e24 > rate ? rate : observer->value4e24;
					observer->time4f0c = observer_time_get();
				}
			}
			else if (observer->value4f10 >= *(long *)((byte *)observer->configuration + 0x12c))
			{
				if (rate > observer->value4e20) observer->value4e20 = rate;
				long base = observer->flag4f18 ? observer->value4e28 : rate;
				long budget = base + *(long *)((byte *)observer->configuration + 0x140);
				if (budget > observer->value4e28 && budget + *(long *)((byte *)observer->configuration + 0x13c) < observer->value4e24)
				{
					observer->value4e28 = budget;
					observer->value4f10 = 0;
				}
			}
		}
		long indices[MAXIMUM_OBSERVER_CHANNELS];
		long count = 0;
		for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
		{
			s_network_observer_channel *channel = &observer->channels[i];
			*(long *)((byte *)channel + 0x480) = NONE;
			*(real *)((byte *)channel + 0x484) = -1.0f;
			*(long *)((byte *)channel + 0x488) = NONE;
			if (channel->state == 7 && channel->connection_index != NONE && function_x7665e0(channel->connection_index)->callback)
				indices[count++] = i;
		}
		if (count > 0)
		{
			observer->value4f1c = count;
			if (observer->flag4e00)
			{
				observer->value4f20 = (observer->value4e28 << 10) / count;
				observer->real4f24 = observer_budget_rate(observer, observer->value4f20, false);
				observer->real4f28 = observer_budget_rate(observer, observer->value4f20, true);
				real frame_rate = network_frame_rate();
				bool limited = false;
				if (observer->real4f24 + 0.0001f < observer->configuration->real110 * frame_rate) limited = true;
				if (observer->real4f24 + 0.0001f < frame_rate) limited = true;
				observer->flag4f18 = limited;
			}
			else
			{
				observer->value4f20 = NONE;
				observer->real4f24 = -1.0f;
				observer->real4f28 = -1.0f;
				observer->flag4f18 = false;
			}
		}
		for (long j = 0; j < count; j++)
		{
			s_network_observer_channel *channel = &observer->channels[indices[j]];
			s_network_connection *connection = function_x7665e0(channel->connection_index);
			*(long *)((byte *)channel + 0x480) = observer->value4f20;
			*(real *)((byte *)channel + 0x484) = connection->callback && connection->callback->active ? observer->real4f24 : observer->real4f28;
			long budget = *(long *)((byte *)channel + 0x480);
			if (budget >= 0)
			{
				long delay = 0;
				if (connection->state == 5 && (connection->flags & 8))
					delay = *(long *)((byte *)network_reliable_stream_get(connection->reliable_stream_index) + 0x96c);
				long interval = 0;
				if (connection->state == 5 && (connection->flags & 8))
					interval = *(long *)((byte *)network_reliable_stream_get(connection->reliable_stream_index) + 0x970);
				long burst = (delay + 2 * interval) * budget / 8000;
				long minimum = *(long *)((byte *)observer->configuration + 0xe0);
				*(long *)((byte *)channel + 0x488) = burst > minimum ? burst : minimum;
			}
			else *(long *)((byte *)channel + 0x488) = NONE;
		}
		observer->time4f08 = observer_time_get();
	}
}

// @retail 0x78880
void __stdcall function_78880(void *p)
{
	s_network_observer *observer = (s_network_observer *)p;
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *channel = &observer->channels[i];
		if (channel->state)
		{
			s_network_connection *connection = function_x7665e0(channel->connection_index);
			if (connection->state > 2)
			{
				network_connection_close(connection, 3);
				network_observer_update_connection(observer, i);
			}
			network_connection_dispose(function_x7665e0(channel->connection_index));
			channel->connection_index = NONE;
		}
	}
}

void network_connection_connect(s_type_99af70 const *address, s_network_connection *connection, bool initiator);
long function_75890(long time);

// @retail 0x776a0
void network_observer_retry_channel(s_network_observer *observer, long channel_index)
{
	long const *index_reference = &channel_index;
	s_network_observer_channel *channel = &observer->channels[*index_reference];
	if (!channel->state || channel->connection_index == NONE) return;
	s_network_connection *connection = function_x7665e0(channel->connection_index);
	if (channel->state == 1)
	{
		long started = channel->time;
		if (observer_time_get() - started >= *(long *)((byte *)observer->configuration + 0x6c))
		{
			network_observer_set_channel_state(observer, 2, channel_index);
			channel->attempts = 0;
			channel->owner_flags = 0;
			channel->owner_index = NONE;
			network_observer_channel_forget_address(observer, channel_index, false, 0);
			network_observer_channel_check_connect(observer, channel_index);
			network_observer_update_connection(observer, channel_index);
			network_observer_retry_channel(observer, channel_index);
		}
	}
	if (channel->state == 4 && (channel->flags & 1))
	{
		network_observer_set_channel_state(observer, 5, channel_index);
		channel->attempts = 0;
	}
	if (channel->state == 2 || channel->state == 5 || channel->state == 9)
	{
		bool wanted = false;
		long count;
		long *delays;
		byte *config = (byte *)observer->configuration;
		if (channel->state == 2)
		{
			count = *(long *)config;
			delays = (long *)(config + 4);
		}
		else if (!(channel->flags & 4))
		{
			count = *(long *)(config + 0x24);
			delays = (long *)(config + 0x28);
		}
		else
		{
			count = *(long *)(config + 0x48);
			delays = (long *)(config + 0x4c);
		}
		if (channel->attempts || (channel->flags & 4))
		{
			for (long i = 0; i < MAXIMUM_OBSERVER_OWNERS; i++)
				if ((channel->owner_mask & (1 << i)) && observer->owners[i].active->channel_may_send(channel_index, (channel->flags >> 2) & 1))
					wanted = true;
			if (!wanted)
			{
				if (network_observer_channel_stalled(observer, channel_index, 0))
				{
					network_observer_channel_forget_address(observer, channel_index, false, 0);
					network_observer_set_channel_state(observer, 1, channel_index);
				}
				return;
			}
		}
		channel->time98 = 0;
		channel->time9c = 0;
		if (channel->attempts >= 0 && channel->attempts < count)
		{
			long delay = delays[channel->attempts];
			if (function_75890(channel->time) >= delay)
			{
				if (channel->state == 2)
				{
					if (network_observer_channel_find_address(observer, channel_index))
						network_observer_set_channel_state(observer, 3, channel_index);
					else
						network_observer_set_channel_state(observer, 1, channel_index);
				}
				else
				{
					network_connection_connect(&channel->address, connection, true);
					network_observer_set_channel_state(observer, channel->state == 5 ? 6 : 8, channel_index);
				}
			}
		}
		else
		{
			network_observer_channel_forget_address(observer, channel_index, false, 0);
			network_observer_set_channel_state(observer, 1, channel_index);
		}
	}
}

bool function_07ab60(const s_type_99af70 *address, long local, long *index_out, XNKID *kid_out, XNKEY *key_out, XNADDR *xnaddr_out);
bool network_connection_flags_valid(dword flags);
void network_connection_establish(s_network_connection *connection, long remote_sequence);

// @retail 0x785d0
void __stdcall function_0785d0(void *unknown10, s_type_99af70 const *address, void const *message)
{
	s_network_observer *observer = (s_network_observer *)unknown10;
	const long *request = (const long *)message;
	long failure = 4;
	if (function_07acf0(address) == 2)
	{
		failure = 1;
		dword flags = request[1];
		if (!(address->address_length == 4 && address->ipv4_address == 0x7f000001) && !(flags & 0xc0))
		{
			dword connection_flags = (flags >> 1) & 1;
			if (flags & 1) connection_flags |= 2; else connection_flags &= ~2;
			if (flags & 4) connection_flags |= 4; else connection_flags &= ~4;
			if (flags & 8) connection_flags |= 8; else connection_flags &= ~8;
			if (flags & 16) connection_flags |= 16; else connection_flags &= ~16;
			if (flags & 32) connection_flags |= 32; else connection_flags &= ~32;
			if (network_connection_flags_valid(connection_flags))
			{
				long key_index;
				XNKID id;
				XNKEY key;
				XNADDR remote;
				if (function_07ab60(address, false, &key_index, &id, &key, &remote))
				{
					for (long index = 0; index < MAXIMUM_OBSERVER_CHANNELS; index++)
					{
						s_network_observer_channel *channel = &observer->channels[index];
						if (channel->state && !memcmp(&remote, channel->remote_id, sizeof(remote)) && channel->owner_mask)
						{
							s_network_connection *connection = function_x7665e0(channel->connection_index);
							s_type_99af70 previous;
							network_connection_get_address(connection, &previous);
							dword sequence_delta = request[0] - connection->remote_sequence;
							if (connection->state > 2)
							{
								if (!function_7af80(&previous, address, false))
								{
									XNKID old_id;
									XNADDR old_remote;
									function_07ab60(&previous, false, NULL, &old_id, NULL, &old_remote);
									function_07acf0(&previous);
									network_connection_close(connection, 6);
								}
								else if (connection->remote_sequence != NONE && sequence_delta > 0)
									network_connection_close(connection, 6);
							}
							if (connection->state <= 2)
							{
								function_07acc0(address);
								channel->address = *address;
								channel->key_index = key_index;
								channel->id = *(s_network_session_id *)&id;
								channel->key = key;
								channel->owner_index = NONE;
								network_connection_connect(address, connection, false);
								network_observer_set_channel_state(observer, 6, index);
								channel->attempts = 0;
								network_observer_update_connection(observer, index);
							}
							network_connection_establish(connection, request[0]);
							network_observer_channel_check_connect(observer, index);
							network_observer_update_connection(observer, index);
							return;
						}
					}
				}
				failure = 3;
			}
		}
	}
	struct { long sequence, reason; } response;
	response.sequence = request[0];
	response.reason = failure;
	function_07b140(observer->link, (long)address, 5, sizeof(response), &response);
}

// @retail 0x76a40
void network_observer_request_channel(s_network_observer *observer, long index)
{
	s_network_observer_channel *channel = &observer->channels[index];
	if (channel->connection_index != NONE)
	{
		if (channel->state == 1)
		{
			network_observer_set_channel_state(observer, 2, index);
			channel->attempts = 0;
		}
		channel->flags |= 1;
		network_observer_channel_check_connect(observer, index);
		network_observer_update_connection(observer, index);
		network_observer_retry_channel(observer, index);
	}
}

// @retail 0x75da0
void network_observer_update(s_network_observer *observer)
{
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *channel = &observer->channels[i];
		if (channel->state)
		{
			if (!channel->owner_mask && network_observer_channel_stalled(observer, i, 14))
				network_observer_channel_dispose(observer, i);
			if (channel->state)
			{
				network_observer_channel_check_connect(observer, i);
				network_observer_update_connection(observer, i);
				network_observer_retry_channel(observer, i);
				network_observer_check_channel_activity(observer, i);
				network_observer_channel_probe(observer, i);
			}
		}
	}
	network_observer_update_channel_budgets(observer);
	function_78ac0(observer);
	function_7a4a0(observer);
}

// @retail 0x78900
void network_observer_recreate_connections(s_network_observer *observer)
{
	for (long i = 0; i < MAXIMUM_OBSERVER_CHANNELS; i++)
	{
		s_network_observer_channel *channel = &observer->channels[i];
		if (channel->state)
		{
			XNADDR remote = *(XNADDR *)channel->remote_id;
			dword ip = remote.ina.s_addr;
			ip = (((ip & 0xff0000) | (ip >> 16)) >> 8) | (((ip << 16) | (ip & 0xff00)) << 8);
			char name[256];
			function_11c9c0(name, sizeof(name), "%hd.%hd.%hd.%hd", ip >> 24, (ip >> 16) & 255, (ip >> 8) & 255, ip & 255);
			long connection = network_connection_allocate((long)name, 0x38);
			channel->connection_index = connection;
			if (connection != NONE) function_x7665e0(connection)->owner = (c_connection_owner *)observer;
			network_observer_channel_check_connect(observer, i);
			network_observer_update_connection(observer, i);
			network_observer_retry_channel(observer, i);
		}
	}
}
