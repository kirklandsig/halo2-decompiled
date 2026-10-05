// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_075870.CPP: the network observer's owners and channels (lane D,
   outside its regions: the session code sends through it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_059ad0.h"
#include "unknown_0662e0.h"
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
	s_network_observer *const *observer_reference = &observer;
	s_network_observer_channel *channel = &(*observer_reference)->channels[channel_index];

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
	s_network_observer *const *observer_reference = &observer;
	s_network_observer_channel *channel = &(*observer_reference)->channels[channel_index];
	bool result = false;
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state == 5)
		{
			long last = connection->timers[1].time;
			long since = observer_time_get() - last;
			long time = connection->state > 2 ? connection->timers[4].time : 0;
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
	s_network_observer *const *observer_reference = &observer;
	s_network_observer_channel *channel = &(*observer_reference)->channels[channel_index];
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
					long since_timer = function_75890(connection->state > 2 ? connection->timers[3].time : 0);
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
	s_network_observer_channel *channel = &observer->channels[channel_index];
	bool blocked = true;
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
	if (!blocked)
	{
		channel->time9c = 0;
		if (expired)
			return true;
	}
	else
	{
		if (!channel->time9c)
			channel->time9c = observer_time_get();
		if (expired || observer_time_since(channel->time9c) > g_network_configuration.value1528)
			return true;
	}
	if (!function_7af40(&channel->address))
		return true;
	return false;
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

/* whether one of a channel's owners treats it as a host or local channel,
   once its connection has been up long enough */
// @retail 0x77940
bool network_observer_channel_has_host(s_network_observer *observer, long channel_index)
{
	s_network_observer_channel *channel = &observer->channels[channel_index];
	bool result = false;
	if (channel->connection_index != NONE)
	{
		s_network_connection *connection = function_x7665e0(channel->connection_index);
		if (connection->state >= 4)
		{
			long time = connection->state > 2 ? connection->timers[1].time : 0;
			if (observer_time_get() - time >= observer->configuration->timeout70)
			{
				for (long owner = 0; owner < MAXIMUM_OBSERVER_OWNERS; owner++)
				{
					if ((channel->owner_mask & (1 << owner)) && observer->owners[owner].active->channel_is_host_or_local(channel_index))
						return true;
				}
			}
			return false;
		}
	}
	return result;
}

/* the video refresh rate (unknown_12b070.cpp) */
extern short g_485ac0;

/* the game's frame rate: 25 on a 50 Hz display, otherwise 30 */
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
// @retail 0x78190
bool network_observer_rate_below(s_network_observer *observer, real rate, bool check_configured, bool check_frame_rate, bool half)
{
	real frame_rate = network_frame_rate();
	bool result = false;
	if (check_configured && rate + 0.0001f < observer->configuration->real110 * frame_rate)
		result = true;
	if (check_frame_rate)
	{
		if (half)
			frame_rate *= 0.5f;
		if (rate + 0.0001f < frame_rate)
			result = true;
	}
	return result;
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
	long channel_index = 0;
	do
	{
		if (observer->channels[channel_index].state)
			observer_channel_clear_flag48c(&observer->channels[channel_index]);
		channel_index++;
	} while (channel_index < MAXIMUM_OBSERVER_CHANNELS);
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
		if (qos_target_result(handle, result, 0))
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
