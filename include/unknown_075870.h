/* UNKNOWN_075870.H: the network observer (lane D): up to four owners (the
   sessions), each with its secure key, and fifteen channels (0x528 bytes each)
   that carry their messages over a connection (0x4d87d4, 0xf8 bytes each) */

#ifndef UNKNOWN_075870_H
#define UNKNOWN_075870_H

#include "unknown_11c920.h"
#include "unknown_07aec0.h"
#include "unknown_0820f0.h"
#include "network_channel_owner.h"
#include "unknown_092870_2.h"
#include "network_qos.h"
#include <xtl.h>

#define MAXIMUM_OBSERVER_OWNERS 4
#define MAXIMUM_OBSERVER_CHANNELS 15

struct s_network_session_id
{
	long a;
	long b;
};



/* the secure key an owner registered with the transport (0x24 bytes) */
struct s_network_observer_owner
{
	c_network_channel_owner *active;
	long key_index;
	long local;
	s_network_session_id id;
	byte key[16];
};

/* one channel to a remote machine (0x528 bytes) */
struct s_network_observer_channel
{
	long state;
	long time;
	byte flags;
	byte owner_mask;
	short attempts;
	long connection_index;
	long unknown10;
	dword remote_id[9];
	dword owner_flags;
	long owner_index;
	long key_index;
	s_network_session_id id;
	XNKEY key;
	s_type_99af70 address;
	long qos_handle;
	s_qos_result field_x31a738;
	long time94;
	long time98;
	long time9c;
	s_network_statistics statistics_sent;
	s_network_statistics statistics_received;
	byte samples250[0x110];
	byte samples360[0x110];
	byte unknown470[0x48c - 0x470];
	bool flag48c;
	byte unknown48d;
	bool has_callback;
	byte unknown48f[0x4a1 - 0x48f];
	bool flag4a1;
	byte unknown4a2[2];
	long value4a4;
	byte unknown4a8[0x4b0 - 0x4a8];
	long smoothed_delay;
	long smoothed_interval;
	byte unknown4b8[0x4c0 - 0x4b8];
	bool backoff;
	byte unknown4c1[0x4d8 - 0x4c1];
	long loss_penalty;
	long probe_state;
	long probe_reset_time;
	byte unknown4e4[0x4f0 - 0x4e4];
	long baseline_delay;
	byte unknown4f4[0x518 - 0x4f4];
	long constrained_cycles;
	long unconstrained_cycles;
	unsigned __int64 message_mask;
};

/* the observer's timeouts and limits (0x4cf4e0) */
struct s_network_observer_configuration
{
	byte unknown00[0x70];
	long timeout70;
	long timeout74;
	long timeout78;
	long timeout7c;
	long timeout80;
	long timeout84;
	long timeout88;
	long timeout8c;
	byte unknown90[0x9c - 0x90];
	long rate_count;
	real rates[(0xf8 - 0xa0) / 4];
	long statistics_interval;
	byte unknownfc[0x108 - 0xfc];
	long value108;
	long value10c;
	real real110;
	byte unknown114[0x134 - 0x114];
	long value134;
	long value138;
	byte unknown13c[0x1ac - 0x13c];
	real real1ac;
	long time1b0;
	long shift1b4;
};

/* what the connections report their traffic to (the observer, vtable
   0x450e10) */
class c_link_connection_listener
{
public:
	virtual void packet_sent(long connection_index, long size, bool flag) {}
	virtual void connection_updated(long connection_index, long value) {}
	virtual void packet_received(long connection_index, long a, long size) {}
	virtual void connection_v03(long connection_index, bool a, bool b) {}
	virtual void connection_v04(long connection_index) {}
	virtual bool connection_v05(long connection_index, bool a, bool b, bool c, bool *arg_0, bool *arg_1, long *arg_2, long *arg_3, long arg_4, byte *arg_5) { return false; }
};

struct s_network_observer : public c_link_connection_listener
{
	void function_79c00(long index, bool pass_on);
	void function_776a0(long channel_index);
	s_network_observer();
	~s_network_observer();
	void packet_sent(long connection_index, long size, bool flag);
	void connection_updated(long connection_index, long value);
	void packet_received(long connection_index, long size, long delay);
	void connection_v03(long connection_index, bool received, bool flag);
	bool connection_v05(long arg_1, bool arg_2, bool arg_3, bool arg_4, bool *arg_5, bool *arg_6, long *arg_7, long *arg_8, long arg_9, byte *arg_10);
	void *unknown04;
	void *link;
	void *unknown0c;
	s_network_observer_configuration *configuration;
	s_network_observer_owner owners[MAXIMUM_OBSERVER_OWNERS];
	byte unknowna4[4];
	s_network_observer_channel channels[MAXIMUM_OBSERVER_CHANNELS];
	byte flag4e00;
	byte unknown4e01[3];
	long value4e04;
	long value4e08;
	long value4e0c;
	long value4e10;
	byte flag4e14;
	byte unknown4e15[3];
	union
	{
		struct
		{
			long value4e18;
			long value4e1c;
		};
		long counts4e18[2];
	};
	long value4e20;
	long value4e24;
	long value4e28;
	byte unknown4e2c[4];
	s_network_statistics statistics_sent;
	long time4f08;
	long time4f0c;
	long value4f10;
	long value4f14;
	bool flag4f18;
	byte unknown4f19[3];
	long value4f1c;
	long value4f20;
	real real4f24;
	real real4f28;
	long time4f2c;
	long time4f30;
	long time4f34;
	long value4f38;
	bool flag4f3c;
	bool flag4f3d;
	bool flag4f3e;
};

long function_75870(void);
long function_75890(long time);

void network_observer_set_owner_key(s_network_observer *observer, long owner, long key_index, const byte *key, const s_network_session_id *id, long local);
long network_observer_find_channel(s_network_observer *observer, long owner, long remote_index);
void network_observer_close_channel(s_network_observer *observer, long channel_index);
void network_observer_send_message(s_network_observer *observer, long owner, long channel_index, bool out_of_band, long message_type, long message_size, const void *message);
bool network_observer_channel_ready(s_network_observer *observer, long owner, long channel_index, long message_type);
void network_observer_mark_message(s_network_observer *observer, long owner, long channel_index, long message_type);
bool network_observer_channel_timed_out(s_network_observer *observer, long channel_index);

#endif
