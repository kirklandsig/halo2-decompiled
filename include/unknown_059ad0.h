/* UNKNOWN_059AD0.H: the network session object (at least 0x78a8 bytes; the state of
   the session is at +0x741c). The state machine (unknown_058dd0.cpp), the
   getters of unknown_05b040.cpp and the setters of unknown_05c490.cpp all
   work on this one type. Only the fields they touch are named. */

#ifndef UNKNOWN_059AD0_H
#define UNKNOWN_059AD0_H

#include "unknown_11c920.h"
#include <wchar.h>
#include "network_channel_owner.h"

#define MAXIMUM_PLAYERS_PER_SESSION 16

/* the session options (unknown_138180.cpp): the settings a session is created with */
struct s_session_machine
{
	byte address[6];
};

struct s_session_player_id
{
	byte index;
	byte unknown1;
	byte unknown2;
	byte unknown3;
	byte unknown4[8];
};

struct s_session_player
{
	byte active;
	byte flag1;
	short index;
	long controller;
	s_session_machine machine;
	s_session_player_id id;
	byte unknown1a[2];
	wchar_t name[32];
	byte unknown5c[16];
	byte unknown6c[0x98 - 0x6c];
	byte flag98;
	byte unknown99[0xe4 - 0x99];
};

struct s_session_options
{
	long type;
	char unknown4;
	byte unknown5;
	short unknown6;
	byte unknown8[0x1c - 0x8];
	char name[0x104];
	short unknown120;
	byte unknown122[0x12a - 0x122];
	short unknown12a;
	byte unknown12c;
	byte unknown12d[0x134 - 0x12d];
	byte unknown134[0x130];
	byte unknown264[4];
	long machine_mask;
	s_session_machine machines[MAXIMUM_PLAYERS_PER_SESSION];
	byte local_machine_valid;
	s_session_machine local_machine;
	byte unknown2d3;
	s_session_player players[MAXIMUM_PLAYERS_PER_SESSION];
};

struct s_session_id
{
	long a;
	long b;
};

/* a session's parameters (0xc8 bytes): also a member's properties */
struct s_session_parameters
{
	wchar_t name[16];
	wchar_t description[32];
	long unknown60;
	long unknown64;
	long unknown68;
	long unknown6c;
	long unknown70;
	byte unknown74[16];
	long unknown84[16];
	long unknownc4;
};

/* the update that carries the session parameters that changed */
struct s_session_parameters_update
{
	bool name_changed;
	byte unknown01;
	wchar_t name[16];
	wchar_t description[32];
	bool unknown60_changed;
	byte unknown63;
	long unknown60;
	long unknown64;
	bool unknown68_changed;
	byte unknown6d[3];
	long unknown68;
	long unknown6c;
	long unknown70;
	byte unknown74[16];
	bool unknown84_changed;
	byte unknown8d[3];
	byte unknown84[0x40];
	bool unknownc4_changed;
	byte unknownd1[3];
	long unknownc4;
};

/* one of the session's 0x10c-byte member records */
struct s_session_member
{
	dword words[9];
	union
	{
		struct
		{
			byte unknown24[0x88 - 0x24];
			long unknown88;
			long unknown8c;
			long unknown90;
			long unknown94;
			byte unknown98[0xf0 - 0x98];
		};
		struct
		{
			bool properties_valid;
			byte unknown25[3];
			s_session_parameters properties;
		};
	};
	s_session_id id;
	long player_count;
	long player_indices[4];
};

struct s_long_pair
{
	long a;
	long b;
};

struct s_unknown_108
{
	long data[27];
};

struct s_unknown_3648
{
	long data[0x390];
};

/* a player's properties (0x90 bytes) */
struct s_player_properties
{
	wchar_t name[32];
	long unknown40[4];
	byte unknown50[32];
	long unknown70[3];
	char team;
	byte unknown7d;
	byte unknown7e;
	byte unknown7f;
	byte unknown80;
	byte unknown81;
	byte unknown82[2];
	long unknown84;
	short unknown88;
	short unknown8a;
	long unknown8c;
};

#pragma pack(push, 1)
/* one of the session's 0x13c-byte player records: an Xbox Live user id first */
struct s_network_session_player
{
	unsigned __int64 user_id;
	dword user_flags;
	long member_index;
	long slot;
	long unknown14;
	byte properties18[0x90];
	byte propertiesa8[0x90];
	long unknown138;
};

/* the per-member state at +0x72dc (0x14 bytes) */
struct s_network_session_member_state
{
	byte unknown00;
	bool flag1;
	bool flag2;
	bool flag3;
	long unknown04;
	long unknown08;
	long unknown0c;
	long unknown10;
};

/* a reserved place in the session (0x24 bytes; unknown_062f40.cpp) */
struct s_network_session_reservation
{
	bool active;
	bool joined;
	byte id[8];
	byte identity[12];
	byte unknown16[2];
	long unknown18;
	long time;
	long timeout;
};

class c_class_58d20 : public c_network_channel_owner
{
public:
	bool channel_is_host_or_local(long channel_index);
	bool channel_is_trusted(long channel_index);
	bool channel_may_send(long channel_index, bool force);
	void channel_connection_changed(long channel_index, long remote_index, bool connected);
	long find_member_by_channel(long channel_index);

	void *unknown04;
	struct s_network_observer *observer;
	byte unknown0c[4];
	long value10;
	long value14;
	long value18;
	long unknown1c;
	long unknown20;
	byte flag24;
	byte data25[16];
	byte unknown35[3];
	long value38;
	long value3c;
	long member_index;
	long value44;
	bool flag48;
	byte unknown49[3];
	long value4c;
	long value50;
	long member_count;
	s_session_member members[16];
	long player_count;
	dword player_mask;
	s_network_session_player players[16];
	long value24e0;
	byte data24e4[0x4974 - 0x24e4];
	byte unknown4974[4];
	long update_count;
	long value497c;
	long type;
	long time4984;
	long value4988;
	long value498c;
	long value4990;
	long value4994;
	byte flag4998;
	s_long_pair data4999;
	byte data49a1[3];
	long value49a4;
	bool flag49a8;
	byte unknown49a9[3];
	long value49ac;
	long value49b0;
	long value49b4;
	byte data49b8[12];
	byte value49c4;
	byte unknown49c5[3];
	long value49c8;
	dword data49cc[7];
	bool flag49e8;
	byte unknown49e9[7];
	long value49f0;
	long value49f4;
	long value49f8;
	bool flag49fc;
	byte flag49fd;
	byte unknown49fe[2];
	byte data4a00[0x308];
	long value4d08;
	long value4d0c;
	union
	{
		byte flag4d10;
		char string4d10[0x80];
	};
	byte unknown4d90[0x4da0 - 0x4d90];
	long value4da0;
	long value4da4;
	long value4da8;
	long value4dac;
	byte data4db0[0x130];
	wchar_t name4ee0[32];
	byte flag4f20;
	byte unknown4f21[3];
	s_unknown_108 data4f24;
	s_unknown_3648 data4f90;
	short value5dd0;
	byte unknown5dd2[6];
	bool flag5dd8;
	byte unknown5dd9[3];
	dword data5ddc[0x11];
	long value5e20;
	byte unknown5e24[4];
	long value5e28;
	byte data5e2c[0x72d8 - 0x5e2c];
	long current_member;
	s_network_session_member_state member_states[16];
	long state;
	union
	{
		bool flag7420;
		long value7420;
	};
	dword mask7424;
	long time7428;
	long index742c;
	bool flag7430;
	byte unknown7431[0x743c - 0x7431];
	bool flag743c;
	byte unknown743d[0x7618 - 0x743d];
	long update7618;
	byte data761c[0x34];
	long update7650;
	long value7654;
	long value7658;
	bool flag765c;
	byte unknown765d[3];
	long value7660;
	long time7664;
	s_network_session_reservation reservations[16];
	class c_network_session_listener *listener;
	bool flag78ac;
	byte unknown78ad[3];
	long time78b0;
	long time78b4;

	/* getters (unknown_05b040.cpp) */
	byte get_value_49c4();
	long get_value_49c8();
	byte *get_data_4a00();
	long get_value_5e20();
	bool get_values_4d08(long *a, long *b, byte **c);
	__int64 get_values_4da0();

	/* setters (unknown_05c490.cpp) */
	bool set_value_4994(long value);
	bool set_values_4da0(long a, long b);
	bool clear_value_49c4();
	bool set_value_4da8(long value);
	bool set_data_4f24(const s_unknown_108 *a, const s_unknown_3648 *b);
	bool set_data_4999(const s_long_pair *data);
	bool set_value_5e20(long value);

	/* state query (unknown_058cb0.cpp) */
	bool function_058d20();

	/* leaves the session (unknown_059ad0.cpp) */
	void leave(bool immediately);
};
#pragma pack(pop)

/* a member's identity: the first 0x24 bytes of its record */
struct s_session_member_identity
{
	dword words[9];
};

/* src/unknown_059ad0.cpp */
inline void network_session_leave(c_class_58d20 *session, bool immediately)
{
	session->leave(immediately);
}
bool network_session_stop_countdown(c_class_58d20 *session);
bool __stdcall network_session_delegate_leader(c_class_58d20 *session, const s_session_member_identity *identity);
bool __stdcall network_session_boot_machine(c_class_58d20 *session, const s_session_member_identity *identity);
bool network_session_parameters_set_value49a4(c_class_58d20 *session, long value);
bool network_session_parameters_set_value49c4(c_class_58d20 *session);
bool network_session_parameters_set_value4d08(c_class_58d20 *session, long value4d08, long value4d0c, const char *string);
bool network_session_parameters_set_value49a1(c_class_58d20 *session, const byte *value);
bool network_session_parameters_set_value5dd0(c_class_58d20 *session, short value);
bool network_session_parameters_set_value498c(c_class_58d20 *session, long value);
bool network_session_start_countdown(c_class_58d20 *session, bool start, long countdown, long mode, const long *time);
void network_session_set_mode(c_class_58d20 *session, long mode);
bool network_session_host_set_value49f8(c_class_58d20 *session, long value);
bool network_session_get_key(c_class_58d20 *session, s_session_id *id, byte *key, long *key_index, long *local);
struct s_parameters_part;
bool network_session_get_data5ddc(c_class_58d20 *session, s_parameters_part *data);
bool network_session_host_set_data5ddc(c_class_58d20 *session, const s_parameters_part *data);

#endif
