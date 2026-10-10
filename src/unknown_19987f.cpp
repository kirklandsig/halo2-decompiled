// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_19987F.CPP: the menus' view of the network session: the network
   state the interface shows, the session members and the session queries
   (lane H) */

#include "unknown_11c920.h"
#include "data_array.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "unknown_120d80.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"
#include "unknown_19c1d0.h"
#include "unknown_19d220.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the membership block at +0x4c of the session (unknown_059670.cpp) */
struct s_network_session_membership
{
	long value4c;
	long value50;
	long member_count;
	s_session_member members[16];
	long player_count;
	dword player_mask;
	s_network_session_player players[16];
};

bool function_59670(c_class_58d20 **session);
bool function_596a0(c_class_58d20 **session);
bool function_592f0(void);
s_network_session_membership *function_5a680(c_class_58d20 *session, long *current_member, long *member_index);
bool function_058d70(c_class_58d20 *s);
dword function_0592d0(void);

long network_session_interface_get_value_18(void);
long network_session_interface_get_value_49a4(void);
long network_session_interface_get_value_498c(void);
byte *network_session_interface_get_data_49a1(void);
byte *network_session_interface_get_data_4db0(void);
bool network_session_interface_get_values_4d08(long *a, long *b, byte **c);
long network_session_interface_get_value_49b0(void);
bool network_session_interface_can_add_player(void);
long function_190262(long value);

bool network_session_get_membership(c_class_58d20 *session, long *value4c, long *host_member_index, long *local_member_index, long *value50, long *member_count, s_session_member **members, long *player_count, dword *player_mask, s_network_session_player **players);

extern bool g_4d8ba0; /* unknown_0820f0.cpp */

bool network_session_interface_set_value49a1(const byte *value);
bool network_session_interface_set_value498c(long value);
bool network_session_interface_set_value49c4(void);
bool network_session_interface_set_value4d08_and_stop_countdown(long value4d08, long value4d0c, const char *string);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
const char *function_19c970(long campaign_id, long map_id);

// @retail 0x19a84e
bool function_19a84e(long *a, long *b)
{
	byte *c;
	return network_session_interface_get_values_4d08(a, b, &c);
}

// @retail 0x19989d
long function_19989d(void)
{
	long default_result = NONE;
	long result;
	long value = network_session_interface_get_value_18();
	byte *data = network_session_interface_get_data_4db0();
	long a;
	long b;

	if (!function_19a84e(&a, &b))
	{
		a = NONE;
	}
	switch (value)
	{
	case 0:
	case 1:
		if (a != NONE)
		{
			result = value ? 2 : 0;
			goto done;
		}
		if (data)
		{
			long state = *(long *)(data + 0x44);
			if (state > 0 && (state <= 4 || state > 6 && state <= 9))
			{
				result = (value != 0) * 2 + 1;
				goto done;
			}
		}
		break;
	case 2:
		switch (network_session_interface_get_value_49a4())
		{
		case 1:
			if (a != NONE)
			{
				result = 4;
				goto done;
			}
			if (data)
			{
				long state = *(long *)(data + 0x44);
				if (state > 0 && (state <= 4 || state > 6 && state <= 9))
				{
					result = 5;
					goto done;
				}
			}
			break;
		case 2:
			result = 6;
			goto done;
		}
		break;
	}
	result = *(long const volatile *)&default_result;
done:
	return result;
}

// @retail 0x199951
bool function_199951(long state)
{
	return state == 1 || state == 3 || state == 5;
}

// @retail 0x199967
bool function_199967(void)
{
	return function_199951(function_19989d());
}

// @retail 0x199971
bool function_199971(void)
{
	return function_19989d() == 6;
}

// @retail 0x19997f
bool function_19997f(long state)
{
	return state == 0 || state == 2 || state == 4;
}

// @retail 0x199994
bool function_199994(void)
{
	return function_19997f(function_19989d());
}

// @retail 0x19999e
bool function_19999e(long state)
{
	return state == 0 || state == 1;
}

// @retail 0x1999b3
bool function_1999b3(void)
{
	return function_19999e(function_19989d());
}

// @retail 0x1999bf
bool function_1999bf(long state)
{
	return state == 2 || state == 3;
}

// @retail 0x1999d7
bool function_1999d7(void)
{
	return function_1999bf(function_19989d());
}

// @retail 0x1999e3
bool function_1999e3(long state)
{
	return state == 4 || state == 5 || state == 6;
}

// @retail 0x1999f9
bool function_1999f9(void)
{
	return function_1999e3(function_19989d());
}

// @retail 0x199eaa
byte function_199eaa(void)
{
	byte result = 0;
	byte *data = network_session_interface_get_data_49a1();
	if (data)
	{
		result = *data;
	}
	return result;
}

// @retail 0x199ebc
long function_199ebc(void)
{
	long result = 0;
	c_class_58d20 *session = NULL;

	if (function_59670(&session) && function_058d70(session))
	{
		long current_member;
		long member_index;
		result = function_5a680(session, &current_member, &member_index)->player_count;
	}
	return result;
}

// @retail 0x199ef8
long function_199ef8(void)
{
	long result = 0;
	c_class_58d20 *session = NULL;

	if (function_596a0(&session) && function_058d70(session))
	{
		long current_member;
		long member_index;
		result = function_5a680(session, &current_member, &member_index)->player_count;
	}
	return result;
}

// @retail 0x199f34
long function_199f34(void)
{
	long result = 0;
	c_class_58d20 *session = NULL;

	if (function_59670(&session) && function_058d70(session))
	{
		long current_member;
		long member_index;
		result = function_5a680(session, &current_member, &member_index)->member_count;
	}
	return result;
}

// @retail 0x199fd6
long function_199fd6(void)
{
	long count = 0;
	c_class_58d20 *session = NULL;

	if (function_59670(&session) && function_058d70(session))
	{
		long current_member;
		long member_index;
		count = function_5a680(session, &current_member, &member_index)->player_count;
	}
	return 16 - count;
}

// @retail 0x19a015
bool function_19a015(void)
{
	if (network_session_interface_can_add_player() && function_199fd6() > 0)
	{
		return true;
	}
	return false;
}

// @retail 0x19a161
long function_19a161(void)
{
	switch (network_session_interface_get_value_498c())
	{
	case 0:
		return 0;
	case 1:
		return 1;
	}
	return 2;
}

// @retail 0x19a250
bool function_19a250(void)
{
	bool result = false;

	if (g_4d8ba0)
	{
		long mode = function_0592d0();

		if (mode <= 1 || mode != 2 && (mode <= 4 || mode > 8))
			result = true;
		else
			result = false;
	}
	return result;
}

// @retail 0x19a279
long function_19a279(void)
{
	long result = 0;

	if (g_4d8ba0)
	{
		switch (function_0592d0())
		{
		case 0:
			result = 0;
			break;
		case 5:
			result = 7;
			break;
		case 6:
		case 7:
			result = 2;
			break;
		case 1:
		case 2:
			result = 3;
			break;
		case 3:
		case 8:
			result = 4;
			break;
		case 4:
		case 9:
			result = 6;
			break;
		default:
			__assume(0);
		}
	}
	return result;
}

// @retail 0x19a8ef
bool function_19a8ef(void)
{
	bool result = false;
	if (network_session_interface_get_value_49b0() == 2)
	{
		result = true;
	}
	return result;
}

// @retail 0x19a935
bool function_19a935(void)
{
	return network_session_interface_get_value_18() == 2;
}

bool network_session_manager_get_session(c_class_58d20 **session);
bool network_session_interface_get_id(s_session_id *id, byte *key);
void __fastcall function_805d0(s_network_session_player *player);

/* when the session or the local member changed, refreshes the properties of
   the players on the other machines */
// @retail 0x19b304
void function_19b304(void)
{
	c_class_58d20 *session;
	long host_member_index;
	long member_value;
	dword player_mask;
	s_network_session_player *players;
	s_session_id id;

	if (network_session_manager_get_session(&session)
		&& network_session_get_membership(session, &member_value, &host_member_index, NULL, NULL, NULL, NULL, NULL, &player_mask, &players)
		&& network_session_interface_get_id(&id, NULL))
	{
		if (!g_4ee4c4.field_1_2 || memcmp(&id, g_4ee4c4.session_id, sizeof(id)) != 0 || member_value != g_4ee4c4.membership_value)
		{
			long index;

			for (index = 0; index < 16; index++)
			{
				s_network_session_player *player = &players[index];

				if ((player_mask & (1 << index)) && player->member_index != host_member_index && !(player->user_flags & 3) && player->user_flags != 0xbad00000)
				{
					function_805d0(player);
				}
			}
		}
		g_4ee4c4.field_1_2 = true;
		g_4ee4c4.membership_value = member_value;
		memcpy(g_4ee4c4.session_id, &id, sizeof(id));
	}
	else
	{
		g_4ee4c4.field_1_2 = false;
	}
}
// @retail 0x19b3e3
long function_19b3e3(void)
{
	long count = 0;
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		if (TEST_FIELD_BIT(g_54e8e0[index].flag4))
		{
			count++;
		}
	}
	return count;
}

/* the scenario's type, as the presence sees it */
struct s_presence_scenario_view
{
	byte unknown00[0x10];
	short type;
};

extern dword g_54d5b8;
bool network_session_manager_session_unready(void);
long function_19a161(void);
bool function_19a0c5(void);
bool function_19a0e6(void);
void function_19034d(long index, long state, long unknown08, long unknown04, short minutes_a, short minutes_b);

inline long presence_controller_next(long index)
{
	long next = NONE;

	if (index >= 0 && index < 3)
	{
		next = index + 1;
	}
	return next;
}

/* publishes each controller's presence: in a menu, a campaign or a
   multiplayer game, for how many minutes, and how full the session is */
// @retail 0x19b40b
void function_19b40b(void)
{
	long state;
	dword minutes;
	long players;
	long mode;
	long index;
	s_presence_scenario_view *scenario = (s_presence_scenario_view *)g_4e0350;

	if (scenario)
	{
		switch (scenario->type)
		{
		default:
			state = 1;
			break;
		case 2:
			state = 1;
			break;
		case 1:
		case 3:
			state = 6;
			break;
		case 0:
		case 4:
			state = 5;
			break;
		}
	}
	else
	{
		state = 1;
	}
	if (state != g_4ee4c4.presence_state)
	{
		g_4ee4c4.presence_state = state;
		g_4ee4c4.presence_start_time = g_54d5b8;
	}
	minutes = (g_54d5b8 - g_4ee4c4.presence_start_time) / 60000;
	if (network_session_manager_session_unready())
	{
		players = function_19a161() + 1;
	}
	else
	{
		players = 0;
	}
	if (state == 6)
	{
		if (function_19a0c5())
		{
			mode = 2;
		}
		else
		{
			mode = function_19a0e6() ? 3 : 1;
		}
	}
	else
	{
		mode = 0;
	}
	for (index = 0; index != NONE; index = presence_controller_next(index))
	{
		function_19034d(index, state, mode, players, (short)minutes, 0);
	}
	g_4ee4c4.unknown16 = 0;
	g_4ee4c4.presence_state = state;
	g_4ee4c4.presence_minutes = (short)minutes;
}

// @retail 0x199f6d
long function_199f6d(void)
{
	long count = 0;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		long host_member_index;
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, &host_member_index, NULL, NULL, NULL, NULL, NULL, &player_mask, &players))
		{
			long index;

			for (index = 0; index < 16; index++)
			{
				if ((player_mask & (1 << index)) && players[index].member_index == host_member_index)
				{
					count++;
				}
			}
		}
	}
	return count;
}

// @retail 0x19a951
bool function_19a951(long player_index)
{
	bool result = false;

	if (player_index != NONE)
	{
		c_class_58d20 *session = NULL;
		long index = player_index & 0xffff;

		if (function_59670(&session) && function_058d70(session))
		{
			long current_member;
			long member_index;
			dword player_mask = function_5a680(session, &current_member, &member_index)->player_mask;

			if (index >= 0 && index < 16 && (player_mask & (1 << index)))
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	return result;
}

// @retail 0x19a9b4
bool function_19a9b4(long player_index)
{
	bool result = false;

	if (player_index != NONE)
	{
		c_class_58d20 *session = NULL;
		long index = player_index & 0xffff;

		if (function_596a0(&session) && function_058d70(session))
		{
			long current_member;
			long member_index;
			dword player_mask = function_5a680(session, &current_member, &member_index)->player_mask;

			if (index >= 0 && index < 16 && (player_mask & (1 << index)))
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	return result;
}

// @retail 0x19aa17
long function_19aa17(long value)
{
	long result = NONE;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		long host_member_index;
		long member_count;
		s_session_member *members;
		long player_count;
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, &host_member_index, NULL, NULL, &member_count, &members, &player_count, &player_mask, &players))
		{
			long index;

			for (index = 0; index < 4; index++)
			{
				long player_index = members[host_member_index].player_indices[index];
				if (player_index != NONE && players[player_index].unknown14 == value)
				{
					result = player_index;
					break;
				}
			}
		}
	}
	return result;
}

// @retail 0x19aaa5
byte *function_19aaa5(long player_index)
{
	long index = player_index & 0xffff;
	byte *result = NULL;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &player_mask, &players) && (player_mask & (1 << index)))
		{
			result = players[index].propertiesa8;
		}
	}
	return result;
}

// @retail 0x19ab0e
byte *function_19ab0e(long player_index)
{
	long index = player_index & 0xffff;
	byte *result = NULL;
	c_class_58d20 *session = NULL;

	if (function_596a0(&session))
	{
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &player_mask, &players) && (player_mask & (1 << index)))
		{
			result = players[index].propertiesa8;
		}
	}
	return result;
}

// @retail 0x19ab77
bool function_19ab77(long player_index)
{
	bool result = false;
	long index = player_index & 0xffff;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		long host_member_index;
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, &host_member_index, NULL, NULL, NULL, NULL, NULL, &player_mask, &players) &&
			(player_mask & (1 << index)) && players[index].member_index == host_member_index)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x19abe4
bool function_19abe4(long player_index)
{
	bool result = false;
	long index = player_index & 0xffff;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		long value50;
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, NULL, NULL, &value50, NULL, NULL, NULL, &player_mask, &players) &&
			(player_mask & (1 << index)) && players[index].member_index == value50)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x19ac53
short function_19ac53(void)
{
	short result = NONE;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		long value50;
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, NULL, NULL, &value50, NULL, NULL, NULL, &player_mask, &players))
		{
			short index;

			for (index = 0; index < 16; index++)
			{
				if ((player_mask & (1 << index)) && players[index].member_index == value50)
				{
					result = index;
					break;
				}
			}
		}
	}
	return result;
}

// @retail 0x19b4e4
long function_19b4e4(void)
{
	long result = NONE;
	long index;

	for (index = 0; index < 16; index++)
	{
		if (function_19a951(index) && !function_19ab77(index))
		{
			result = index;
			break;
		}
	}
	return result;
}

/* the game variants (unknown_1932c0.cpp) */
struct s_surface_description;
long network_session_interface_get_value_49c8(void);
s_surface_description *function_192e60(long index);
bool function_193470(s_surface_description *p);
bool function_193490(s_surface_description *p);
bool function_1934b0(s_surface_description *p);
bool network_session_interface_kick_player(long player_index);
bool network_session_interface_ban_player(long player_index);

// @retail 0x19a0c5
bool function_19a0c5(void)
{
	bool result = false;
	long index = network_session_interface_get_value_49c8();

	if (index != NONE)
	{
		s_surface_description *variant = function_192e60(index);
		if (variant)
		{
			result = function_193470(variant);
		}
	}
	return result;
}

// @retail 0x19a0e6
bool function_19a0e6(void)
{
	bool result = false;
	long index = network_session_interface_get_value_49c8();

	if (index != NONE)
	{
		s_surface_description *variant = function_192e60(index);
		if (variant)
		{
			result = function_193490(variant);
		}
	}
	return result;
}

// @retail 0x19a107
bool function_19a107(void)
{
	bool result = false;
	long index = network_session_interface_get_value_49c8();

	if (index != NONE)
	{
		s_surface_description *variant = function_192e60(index);
		if (variant)
		{
			result = *(long *)variant == 5;
		}
	}
	return result;
}

// @retail 0x19a127
bool function_19a127(void)
{
	bool result = false;
	long index = network_session_interface_get_value_49c8();

	if (index != NONE)
	{
		s_surface_description *variant = function_192e60(index);
		if (variant)
		{
			result = function_1934b0(variant);
		}
	}
	return result;
}

// @retail 0x19a203
bool function_19a203(void)
{
	bool result = false;

	if (g_4d8ba0 && function_592f0())
	{
		switch ((long)function_0592d0())
		{
		case 0:
		case 2:
		case 5:
		case 6:
		case 7:
			result = false;
			break;
		case 1:
			result = true;
			break;
		case 8:
			result = !function_19a0c5();
			break;
		default:
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x19a1d4
bool function_19a1d4(long player_index)
{
	bool result = false;

	if (function_19a203() && function_19a951(player_index) && !function_19ab77(player_index))
	{
		result = network_session_interface_ban_player(player_index);
	}
	return result;
}

// @retail 0x19a179
bool function_19a179(long player_index)
{
	bool result = false;

	if (function_592f0())
	{
		if (player_index == NONE)
		{
			player_index = function_19b4e4();
		}
		if (player_index != NONE && function_19a951(player_index) && !function_19ab77(player_index))
		{
			result = network_session_interface_kick_player(player_index);
		}
	}
	return result;
}

/* the voice/peer list (unknown_0b35e0.cpp) and its state */
struct s_0b35e0_entry;
s_0b35e0_entry *function_b35e0(long index);
extern long g_4d8f14;

s_peer_list_globals g_4ee4c4;

/* the user interface's allocator (unknown_1a4742.cpp) */
class c_user_interface_allocator : public c_data_allocator
{
public:
	virtual void *allocate(long size);
	virtual void deallocate(void *block);
};

extern c_user_interface_allocator g_47d92c;

/* unknown_0b3570.cpp */
bool function_b3570(long count, c_data_allocator *allocator, bool flag);
void function_b35a0(void);
void function_b35d0(bool start);

// @retail 0x199b33
void function_199b33(bool flag)
{
	g_4ee4c4.active = function_b3570(0x20, &g_47d92c, flag);
}

// @retail 0x199b45
void function_199b45(void)
{
	if (g_4ee4c4.active)
	{
		function_b35a0();
		g_4ee4c4.active = false;
	}
}

// @retail 0x199b5b
void function_199b5b(bool start)
{
	if (g_4ee4c4.active)
		function_b35d0(start);
}

// @retail 0x199b6a
long function_199b6a(long start)
{
	long result = NONE;

	if (g_4ee4c4.active)
	{
		long index;

		for (index = start != NONE ? start + 1 : 0; index < g_4d8f14; index++)
		{
			if (function_b35e0(index))
			{
				result = index;
				break;
			}
		}
	}
	return result;
}

// @retail 0x199ba5
bool function_199ba5(long index)
{
	bool result = false;

	if (g_4ee4c4.active && function_b35e0(index))
	{
		result = true;
	}
	return result;
}

// @retail 0x199bbf
byte *function_199bbf(long index)
{
	byte *result = NULL;

	if (g_4ee4c4.active)
	{
		byte *entry = (byte *)function_b35e0(index);
		if (entry)
		{
			byte *data = entry + 0x70;
			short count = *(short *)(data + 0xbc);
			if (count >= 0 && count <= 16)
			{
				result = data;
			}
		}
	}
	return result;
}

void unicode_string_copy(word *destination, const word *source, long maximum_count);
void unicode_string_snprintf(word *buffer, long maximum_count, const word *format, ...);
void network_session_interface_set_local_name(const wchar_t *machine_name, const wchar_t *session_name);

// @retail 0x199bef
bool function_199bef(const word *machine_name, const word *session_name)
{
	bool result = false;

	if (g_4d8ba0)
	{
		word machine[16];
		word session[32];

		if (machine_name)
		{
			unicode_string_copy(machine, machine_name, 16);
		}
		else
		{
			unicode_string_snprintf(machine, 16, (const word *)L"%S", "");
		}
		unicode_string_copy(session, session_name, 32);
		network_session_interface_set_local_name((const wchar_t *)machine, (const wchar_t *)session);
		result = true;
	}
	return result;
}

bool network_session_interface_get_user_xuid(long index, XUID *xuid);
void network_session_manager_join(const void *target, long count, const void *entries, bool flag);
void network_session_manager_join_description(const s_session_description *description, long count, const void *entries);
bool __stdcall network_session_manager_host_session(long mode, const XNKID *kid, const XNKEY *key);
bool network_session_manager_host_offline(void);
bool network_session_manager_host_online(void);
void function_24f9d4();
void function_199e2e(bool close);
void network_session_manager_check_joining_leader(void);

typedef bool (__stdcall *dialog_choice_callback)(long controller_index);
class c_class_1473c9;
typedef bool (__stdcall *dialog_closed_callback)(c_class_1473c9 *screen, long dialog_id);
void dialog_ok_show(long a, long dialog_id, long b, short user_flags, dialog_choice_callback chosen, dialog_closed_callback closed);

/* joins the session the search found at the index, with the local users */
// @retail 0x199c47
void function_199c47(long index)
{
	byte *description;
	XUID users[4];
	long count;
	long i;

	network_session_manager_check_joining_leader();
	description = function_199bbf(index);
	count = 0;
	for (i = 0; i < 4; i++)
	{
		if (network_session_interface_get_user_xuid(i, &users[count]))
			count++;
	}
	network_session_manager_join_description((s_session_description *)description, count, users);
	function_24f9d4();
}

/* joins the target with the local users, or says there are none */
// @retail 0x199c94
void function_199c94(const void *target, long controller, bool flag)
{
	if (function_19b3e3() > 0)
	{
		XUID users[4];
		long count = 0;
		long i;

		network_session_manager_check_joining_leader();
		for (i = 0; i < 4; i++)
		{
			if (network_session_interface_get_user_xuid(i, &users[count]))
				count++;
		}
		network_session_manager_join(target, count, users, flag);
		function_24f9d4();
	}
	else
	{
		dialog_ok_show(1, 0x44, 4, 1 << controller, 0, 0);
	}
}

long network_session_manager_get_match_mode(void);
long function_59570(void);
bool __stdcall function_594a0(long a, long b, long c);

/* the session state as the user interface shows it */
// @retail 0x199d7c
long function_199d7c(void)
{
	long result;

	switch (network_session_manager_get_match_mode())
	{
	case 1:
		return 1;
	case 2:
		return 8;
	case 3:
		switch (function_59570())
		{
		case 0:
			return 0;
		case 1:
			result = 3;
			break;
		case 2:
			result = 4;
			break;
		case 3:
			result = 5;
			break;
		case 4:
			result = 6;
			break;
		case 5:
			result = 7;
			break;
		default:
			return 0;
		}
		break;
	case 4:
		result = 2;
		break;
	default:
		return 0;
	}
	return result;
}

// @retail 0x199dc9
bool function_199dc9(long a, long b, long c)
{
	bool result = false;

	if (function_199d7c() == 4 || function_199d7c() == 6)
	{
		result = function_594a0(a, b, c);
	}
	return result;
}

void function_1487c3(long a, long b, long load, long c, long d);
class c_online_task_screen;
void __stdcall function_2523bc(c_online_task_screen *screen);

/* leaves the sessions and goes back to the main menu. Retail keeps close in
   a stack slot and pushes it from there, as its address being taken does */
// @retail 0x199e3c
void function_199e3c(long controller)
{
	bool close = function_199f34() <= 1;
	bool const *close_reference = &close;

	function_199e2e(close);
	function_1487c3(controller, NONE, (long)function_2523bc, 0, 0);
}

long __stdcall function_63e90(long index);

// @retail 0x199e6d
bool function_199e6d(long index)
{
	return function_63e90(index) == 0;
}

bool function_641a0(long value);
void function_121040(long value);

long function_687e0(void);

/* the string and progress the joining screen shows for the session's
   state */
// @retail 0x19a02d
void __stdcall function_19a02d(long *string_handle, real *progress)
{
	switch (function_687e0())
	{
	case 2:
		*string_handle = 0x1300079e;
		break;
	case 1:
	case 3:
		*string_handle = 0xf00079f;
		break;
	case 9:
	case 11:
		*string_handle = 0x120007a0;
		break;
	case 4:
	case 10:
		*string_handle = 0xf0007a1;
		break;
	case 5:
	case 7:
	case 8:
		*string_handle = 0x160007a2;
		break;
	case 6:
	case 12:
	case 13:
		*string_handle = 0xf0007a3;
		break;
	default:
		*string_handle = 0xf00079d;
		break;
	}
	*progress = 0.0f;
}
// @retail 0x19a0af
void function_19a0af(long value)
{
	if (function_641a0(value))
		function_121040(value);
}

void __stdcall function_7f0d0(const byte *data);
void network_session_interface_set_unknown64(const byte *data, long unknown84);

// @retail 0x19adf6
void function_19adf6(const byte *data, long value)
{
	function_7f0d0(data);
	network_session_interface_set_unknown64(data, value);
}

/* the menu mode remembered for the next session change, NONE when none */
long g_47ff90 = NONE;

word function_1901fc(void);
bool function_0682c0();
bool function_138840();
bool function_6c7e0();
void __stdcall function_18f1c0(long a);
void network_session_manager_request_mode_acknowledge(void);
bool function_148044(long channel, long index, long value);
bool window_manager_window_has_pause_screen_for_user(long channel, long index, long user_index);
void function_1906b4(void);
void __stdcall function_1483c3(long reason);
void dialog_choice_show(long a, long dialog_id, long b, word user_flags, dialog_choice_callback first_chosen, dialog_choice_callback second_chosen, dialog_closed_callback closed);
bool __stdcall function_23690b(long controller);
bool __stdcall function_236917(long controller_index);
bool __stdcall function_236953(long controller);
c_class_1473c9 *__stdcall function_2524a8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_25240c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_253185(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2310b7(s_screen_parameters *parameters);
extern bool g_4ed39d;

/* reacts to a change of the network session: remembers the menu mode while
   the change is pending, then shows the dialog or the menu that follows it.
   Retail passes all three arguments on the stack: reading the screen id
   through its address keeps it there */
// @retail 0x19ae0f
void function_19ae0f(long change, long pending, long error)
{
	long const volatile *error_reference = &error;

	if (pending)
	{
		long mode = function_19989d();

		if (mode != NONE)
		{
			g_47ff90 = mode;
		}
	}
	else if (!g_4ed39d)
	{
		word user_flags = function_1901fc();
		long mode = g_47ff90;
		bool volatile show_game_menu = false;
		bool show_lobby = false;
		bool show_main_menu = false;
		s_screen_parameters parameters;
		screen_load_proc load;

		g_47ff90 = NONE;
		parameters.field_c = 0;
		switch (change)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			network_session_manager_request_mode_acknowledge();
			if (function_148044(5, 4, 0xba))
			{
				return;
			}
		case 3:
		{
			bool handled = false;

			switch (mode)
			{
			case 0:
			case 1:
				if (*error_reference == 0x1e)
				{
					return;
				}
				if (*error_reference == 0xe)
				{
					show_game_menu = true;
					handled = true;
				}
				break;
			case 2:
			case 3:
				if (*error_reference == 0xe)
				{
					if (function_148044(5, 4, 0xd) || function_148044(5, 4, 0x1e))
					{
						return;
					}
					show_lobby = true;
					handled = true;
				}
				break;
			case 4:
			case 5:
				if (*error_reference == 0xe)
				{
					return;
				}
				break;
			case 6:
				if (*error_reference == 0xe)
				{
					handled = function_148044(5, 4, 0xba);
				}
				else if (*error_reference == 0xdb || *error_reference == 0xed || *error_reference == 0xdc)
				{
					handled = true;
				}
				break;
			}
			if (handled)
			{
				break;
			}
		}
		case 6:
			if (*error_reference == 0x10)
			{
				switch (mode)
				{
				case 0:
				case 1:
					show_game_menu = true;
					break;
				case 2:
				case 3:
					show_lobby = true;
					break;
				case 4:
				case 5:
				case 6:
					show_main_menu = true;
					break;
				}
			}
			else
			{
				show_main_menu = true;
			}
			break;
		case 4:
		{
			long dialog_id;
			bool choice = false;
			dialog_choice_callback first_chosen = function_23690b;
			dialog_choice_callback second_chosen = 0;

			if (!function_0682c0() && !function_138840())
			{
				return;
			}
			if (g_4ee4c4.unknown1c)
			{
				function_18f1c0(0);
				return;
			}
			if (g_4ee4c4.session_booted)
			{
				dialog_id = 0xb9;
				g_4ee4c4.session_booted = false;
			}
			else
			{
				switch (mode)
				{
				case 0:
				case 1:
					function_18f1c0(0);
					return;
				case 2:
				case 3:
					dialog_id = 5;
					break;
				case 4:
				case 5:
				case 6:
					if (function_6c7e0())
					{
						dialog_id = 2;
					}
					else
					{
						dialog_id = 0x38;
						choice = true;
						first_chosen = function_236917;
						second_chosen = function_236953;
					}
					break;
				default:
					return;
				}
			}
			if (!window_manager_window_has_pause_screen_for_user(1, 4, dialog_id))
			{
				if (choice)
				{
					dialog_choice_show(1, dialog_id, 4, function_1901fc(), first_chosen, second_chosen, 0);
				}
				else
				{
					dialog_ok_show(1, dialog_id, 4, function_1901fc(), first_chosen, 0);
				}
			}
			return;
		}
		default:
			show_main_menu = true;
			break;
		}
		if (show_game_menu)
		{
			load = function_19997f(mode) ? function_2524a8 : function_25240c;
		}
		else if (show_lobby)
		{
			load = function_253185;
		}
		else if (show_main_menu)
		{
			if (!function_6c7e0())
			{
				function_1906b4();
				function_1483c3(0);
				return;
			}
			load = function_2310b7;
		}
		else
		{
			return;
		}
		function_149f49((s_message *)&parameters, 7, 0, user_flags, 5, 4, (long)load);
		parameters.load(&parameters);
	}
}

bool function_138800();

/* which of the network menus a screen belongs to */
// @retail 0x19b0e1
long function_19b0e1(long screen_id)
{
	long result = 0;

	if (function_138800() && g_4e6948->state != 3)
	{
		result = 4;
	}
	else
	{
		long mode = function_19989d();

		switch (screen_id)
		{
		case 0x0d:
			result = 1;
			break;
		case 0x0e:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xac:
		case 0xad:
		case 0xaf:
		case 0xb0:
		case 0xbf:
		case 0xdc:
			result = 3;
			break;
		case 0x0f:
		case 0x11:
		case 0x3c:
		case 0xce:
		case 0xcf:
		case 0xd0:
			result = mode != NONE ? 3 : 0;
			break;
		case 0x10:
			result = 6;
			break;
		case 0xb3:
		case 0xdb:
		case 0xed:
			result = 0;
			break;
		case 0xd2:
			result = 2;
			break;
		}
	}
	return result;
}

bool function_68290(void);
short player_slot_count_active(void);
void function_1484f4(void);
long function_147f4f();
void network_session_manager_leave_session_a(bool close);
void network_session_manager_leave_session_b(bool close);
c_class_1473c9 *function_149f1e(word user_flags, long load);
c_class_1473c9 *__stdcall function_2521f8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2519bb(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_24fa4c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_24f8c6(s_screen_parameters *parameters);

/* when the session state no longer fits the menu a screen belongs to, opens
   the menu that does, or leaves the sessions */
// @retail 0x19b1c9
void function_19b1c9(long screen_id, long menu, long state)
{
	if (menu == 4 && function_68290() && function_138840())
	{
		if (screen_id != 0xe7)
		{
			function_149f1e(function_1901fc(), (long)function_2521f8);
		}
	}
	else if (!state || state == menu)
	{
		if (screen_id != 9 && screen_id != 0x1e && function_138800() && g_4e6948->state == 3 && !player_slot_count_active())
		{
			function_18f1c0(0);
		}
	}
	else if (menu == 4)
	{
		switch (state)
		{
		case 5:
			break;
		case 6:
			function_18f1c0(6);
			break;
		default:
			function_18f1c0(0);
			break;
		}
	}
	else
	{
		switch (state)
		{
		case 1:
			if (menu == 1)
			{
				return;
			}
			break;
		case 2:
			if (menu == 2 || function_149f1e(function_1901fc(), (long)function_2519bb))
			{
				return;
			}
			break;
		case 3:
		{
			bool in_game = menu == 0 || menu == 1 || menu == 2 || menu == 6;
			bool players = player_slot_count_active() > 0;

			if (in_game && players)
			{
				function_149f1e(0xffff, (long)function_24fa4c);
				return;
			}
			break;
		}
		case 4:
			break;
		case 5:
			return;
		case 6:
			function_1484f4();
			return;
		case 7:
			if (screen_id != 0xcc)
			{
				function_149f1e(function_1901fc(), (long)function_24f8c6);
			}
			return;
		}
		network_session_manager_leave_session_a(false);
		network_session_manager_leave_session_b(false);
	}
}

/* follows the session for the screen on top */
// @retail 0x199b08
void function_199b08(void)
{
	long screen_id = function_147f4f();
	long menu = function_19b0e1(screen_id);
	long state = function_19a279();

	function_19b1c9(screen_id, menu, state);
	function_19ae0f(menu, state, screen_id);
}

bool __stdcall function_64060(s_game_variant *variant);
bool network_session_interface_set_value5dd0(short value);
void function_120e40(wchar_t const *name);
bool function_19a76d(short index);
bool __stdcall function_19a728(s_game_variant *variant);

/* makes the variant the session's, if it is valid.
   Standard convention (see docs/DECOMPILING.md):
   1. Retail keeps it __stdcall (the variant on the stack, ret 4). With the
      marker this body matches byte for byte; without it LTCG passes the
      variant in ecx.
   2. No data or code in retail holds its address. Its callers (0x199a57,
      0x19a76d, 0x19a864, 0x2c8159, 0x2ca284) are all LTCG game code and push
      the variant.
   3. Tried: taking the variant's address (it still arrives in ecx, the body
      unchanged), and the order of its mutually recursive caller 0x19a76d. */
// @retail 0x19a728 standard
bool __stdcall function_19a728(s_game_variant *variant)
{
	bool result = false;

	if (!variant || !variant->field_xcb8724 || function_19d620(variant))
	{
		result = function_64060(variant);
		if (result && variant)
		{
			function_19a76d(NONE);
			function_120e40(variant->name);
		}
	}
	return result;
}

// @retail 0x19a76d
bool function_19a76d(short index)
{
	bool result = network_session_interface_set_value5dd0(index);

	if (result && index != NONE)
	{
		function_19a728(NULL);
	}
	return result;
}

extern long g_51098c;
long function_19c580(void);
bool function_19a6f2(long campaign_id, long map_id);
long __stdcall function_120e70(byte *buffer);

/* opens the last chosen map, or the first one, with the saved variant or a
   default one */
// @retail 0x19a864
void function_19a864(void)
{
	s_game_variant variant;
	long map_id = g_51098c;

	if (map_id == NONE || !function_19a6f2(NONE, map_id))
	{
		map_id = function_19c580();
		function_19a6f2(NONE, map_id);
	}
	if (function_120e70((byte *)&variant) == NONE || !function_19a728(&variant))
	{
		function_19d220(&variant, 0);
		function_19a728(&variant);
	}
}

long g_54e7bc;
long g_54e7c0;
long g_54e7c4;
long g_54e7c8;
bool g_54e7b8;

// @retail 0x199a57
void function_199a57(void)
{
	g_54e7bc = NONE;
	g_54e7c0 = NONE;
	g_54e7c4 = NONE;
	g_54e7c8 = 0;
	g_54e7b8 = false;
	if (function_592f0())
	{
		function_19a728(NULL);
	}
	function_19a0af(NONE);
}

// @retail 0x199cfc
long function_199cfc(void)
{
	switch (g_527330.state_joining.unknown104)
	{
	case 0:
		return 0;
	case 1:
		return 2;
	case 2:
		return 3;
	case 3:
		return 4;
	case 4:
		return 5;
	case 5:
		return 5;
	case 6:
		return 6;
	case 7:
		return 7;
	case 8:
		return 8;
	case 9:
		return 9;
	case 11:
		return 9;
	case 12:
		return 10;
	case 13:
		return 10;
	case 16:
		return 11;
	default:
		return 11;
	}
}

/* the string id that names each network session state */
// @retail 0x19a50f
long function_19a50f(long state)
{
	long result = 0;

	switch (state)
	{
	case 0:
		result = 0x0a0001e3;
		break;
	case 1:
		result = 0x0b0001e4;
		break;
	case 2:
		result = 0x0f000205;
		break;
	case 5:
		result = 0x11000209;
		break;
	case 6:
		result = 0x190007bb;
		break;
	case 4:
		result = 0x0a000208;
		break;
	case 9:
		result = 0x240001ef;
		break;
	case 11:
		result = 0x0c0001f2;
		break;
	case 12:
		result = 0x0e0001f3;
		break;
	case 13:
	case 14:
		result = 0x210001f4;
		break;
	case 15:
		result = 0x230001f5;
		break;
	case 16:
		result = 0x0f0001f7;
		break;
	case 17:
		result = 0x0f0001f8;
		break;
	case 18:
		result = 0x140001f9;
		break;
	case 19:
		result = 0x190001fa;
		break;
	case 20:
		result = 0x160001fb;
		break;
	case 21:
		result = 0x0f0001fc;
		break;
	case 22:
		result = 0x1c0001f6;
		break;
	case 3:
	case 24:
		result = 0x0f000206;
		break;
	case 25:
		result = 0x1a000207;
		break;
	}

	return result;
}

long network_session_interface_get_members_status(long *progress);
long network_session_interface_get_value_90(long *value94);
long network_session_interface_get_value_49ac(void);
long function_19a5fd(long state);
struct s_entry_c;
s_entry_c *function_19c5f0(long key);

/* a map's entry as the session state sees it: the most players for each
   of its player count settings */
struct s_session_map_view
{
	byte unknown000[0xc54];
	byte maximum_players[10];
};

/* the session's state for the interface: 0 none, 1 not in a session, 4 and
   5 the members' status, 6 ready, 7 the wrong players, 8 to 10 the host's
   states; the members' progress goes to progress. Retail keeps progress on
   the stack: reading it through its address keeps it there */
// @retail 0x19a2ce
long function_19a2ce(real *progress)
{
	real *const *progress_reference = &progress;
	long state = function_19a279();
	long result;
	c_class_58d20 *session = NULL;

	if (*progress_reference)
	{
		**progress_reference = 0.0f;
	}
	if (!state || !function_59670(&session) || !function_058d70(session))
	{
		result = 0;
		goto done;
	}
	if (state != 3)
	{
		result = 1;
		goto done;
	}
	result = 9;
	if (function_199971())
	{
		result = function_19a5fd(function_63e90(network_session_interface_get_value_49c8()));
	}
	else
	{
		long mode = function_19989d();
		bool host = mode == 0 || mode == 2 || mode == 4;
		long percent;
		long status = network_session_interface_get_members_status(&percent);
		real members_progress = (real)PIN(percent, 0, 100) * 0.01f;
		long maximum = 1;

		switch (status)
		{
		case 1:
			if (network_session_interface_get_value_90(&percent) == 1)
			{
				result = 6;
				goto done;
			}
			result = 5;
			break;
		case 2:
			result = 4;
		case 3:
		case 4:
			if (*progress_reference)
			{
				**progress_reference = members_progress;
			}
			break;
		}
		if (result == 9 || result == 4)
		{
		if (host)
		{
			long ready = 0;
			long present = 0;
			long index;

			for (index = 0; index < 16; index++)
			{
				if (function_19a951(index))
				{
					present++;
					if (function_19ab77(index))
					{
						ready++;
					}
				}
			}
			if (ready != 2 || present != ready)
			{
				result = 7;
			}
		}
		else
		{
			byte *data = network_session_interface_get_data_4db0();

			if (data && (*(dword *)(data + 0x48) & 1))
			{
				long count = 0;
				dword teams = 0;
				long a;
				long b;
				s_session_map_view *map;
				long index;

				function_19a84e(&a, &b);
				map = (s_session_map_view *)function_19c5f0(b);
				if (map)
				{
					long setting = *(long *)(data + 0x44);

					if (setting >= 1 && setting <= 9)
					{
						maximum = map->maximum_players[setting];
						if (maximum <= 1)
						{
							maximum = 1;
						}
					}
				}
				for (index = 0; index < 16; index++)
				{
					if (function_19a951(index))
					{
						byte *player = function_19aaa5(index);

						if (player)
						{
							char team = (char)player[0x7c];

							if (team >= 0 && team < 16 && !(teams & (1 << team)))
							{
								teams |= 1 << team;
								count++;
							}
						}
					}
				}
				if (count < 1 || count > maximum)
				{
					result = 7;
				}
			}
		}
		}
	}
	if (result == 9 || result == 4)
	{
		if (network_session_interface_get_value_49ac() >= 0)
		{
			result = 10;
		}
		else if (function_592f0())
		{
			result = 9;
		}
		else if (result == 9)
		{
			result = 8;
		}
	}
done:
	return result;
}
bool network_session_interface_start_countdown(long user_index, bool start, long countdown, long mode);
long function_19a8d0(void);

/* a controller's session user (the controllers are 0xc70 bytes apart) */
struct s_countdown_controller_view
{
	dword flags;
	long session_user;
	byte unknown008[0xc70 - 8];
};

/* starts the countdown for the controller's user: when ready, from the
   given seconds; while it runs, one second down when above the minimum */
// @retail 0x19a78e
bool function_19a78e(long controller, long countdown, long minimum)
{
	bool result = false;
	long state = function_19a2ce(NULL);
	long user = ((s_countdown_controller_view *)g_54e8e0)[controller].session_user;

	if (user != NONE)
	{
		if (state == 9)
		{
			if (function_592f0() && network_session_interface_start_countdown(user, true, countdown, 0))
			{
				result = true;
			}
		}
		else if (state == 10)
		{
			long seconds = function_19a8d0();

			if (seconds > minimum && network_session_interface_start_countdown(user, true, seconds - 1, 0))
			{
				result = true;
			}
		}
	}
	return result;
}

/* stops the countdown for the controller's user (or starts it while it
   runs) */
// @retail 0x19a7e9
bool function_19a7e9(long controller, long value)
{
	bool result = false;
	long user = ((s_countdown_controller_view *)g_54e8e0)[controller].session_user;

	if (user != NONE && network_session_interface_get_value_49ac() != NONE)
	{
		bool start;
		long mode = 0;

		if (function_592f0())
		{
			start = false;
		}
		else
		{
			if (function_19a2ce(NULL) != 10)
			{
				goto done;
			}
			start = true;
			mode = 1;
		}
		if (network_session_interface_start_countdown(user, start, value, mode))
		{
			result = true;
		}
	}
done:
	return result;
}
/* the seconds of the host's countdown, while it runs */
// @retail 0x19a8d0
long function_19a8d0(void)
{
	long result = 0;

	if (function_19a2ce(NULL) == 10)
	{
		result = network_session_interface_get_value_49ac();
		if (result <= 0)
		{
			result = 0;
		}
	}
	return result;
}

/* whether the countdown runs, or the session's mode is 2 or 3 */
// @retail 0x19a902
bool function_19a902(void)
{
	long mode = 0;
	long state = function_19a2ce(NULL);
	byte result = 0;

	if (g_4d8ba0)
	{
		mode = function_0592d0();
	}
	if (state == 10 || mode == 3 || mode == 2)
	{
		result = true;
	}
	return result;
}
/* the interface state for each network session state */
// @retail 0x19a5fd
long function_19a5fd(long state)
{
	volatile long default_value = 0;
	long entries[16][2] =
	{
		{ 0, 0x9 },
		{ 1, 0xb },
		{ 2, 0xc },
		{ 3, 0xd },
		{ 4, 0xe },
		{ 5, 0xf },
		{ 6, 0x10 },
		{ 7, 0x11 },
		{ 8, 0x12 },
		{ 9, 0x13 },
		{ 0xa, 0x14 },
		{ 0xb, 0x15 },
		{ 0xc, 0x16 },
		{ 0xd, 0x17 },
		{ 0xe, 0x18 },
		{ 0xf, 0x19 },
	};


	long result;
	for (dword i = 0; i < 16; i++)
	{
		if (entries[i][0] == state)
		{
			result = entries[i][1];
			goto done;
		}
	}

	result = default_value;
done:
	return result;
}

// @retail 0x199e7e
bool function_199e7e(byte value)
{
	bool result = false;

	if (network_session_interface_get_data_49a1())
	{
		byte copy = value;

		if (network_session_interface_set_value49a1(&copy))
			result = true;
	}
	return result;
}

// @retail 0x19a148
bool function_19a148(long privacy)
{
	long value;

	switch (privacy)
	{
	case 0:
		value = 0;
		break;
	case 1:
		value = 1;
		break;
	default:
		value = 2;
		break;
	}
	return network_session_interface_set_value498c(value);
}

// @retail 0x19a1bd
bool function_19a1bd(void)
{
	bool result = false;

	if (function_592f0())
		result = network_session_interface_set_value49c4();
	return result;
}

// @retail 0x19a6f2
bool function_19a6f2(long campaign_id, long map_id)
{
	bool result = false;
	const char *path = function_19c970(campaign_id, map_id);

	if (path)
	{
		result = network_session_interface_set_value4d08_and_stop_countdown(campaign_id, map_id, path);
		if (result)
		{
			global_preferences_globals.current.unknown16c = map_id;
			global_preferences_globals.dirty = true;
		}
	}
	return result;
}

// @retail 0x19acc6
bool function_19acc6(XUID const *xuid)
{
	bool found = false;
	c_class_58d20 *session = NULL;

	if (function_59670(&session))
	{
		dword player_mask;
		s_network_session_player *players;

		if (network_session_get_membership(session, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &player_mask, &players))
		{
			long index;

			for (index = 0; index < 16; index++)
			{
				if (found)
					break;
				if (player_mask & (1 << index))
					found = xuid_equal((XUID const *)&players[index].user_id, xuid, true);
			}
		}
	}
	return found;
}

// @retail 0x19ad39
long function_19ad39(c_class_58d20 *session, XUID const *xuid)
{
	long result = NONE;

	if (session && xuid && function_058d70(session))
	{
		long index;

		for (index = 0; index < 16; index++)
		{
			if ((session->player_mask & (1 << index)) && xuid_equal((XUID const *)&session->players[index].user_id, xuid, true))
			{
				result = index;
				break;
			}
		}
	}
	return result;
}
/* unknown_01cf50.cpp's view of the session */
struct s_597d0_object;
bool function_597d0(s_597d0_object **out);

// @retail 0x19ad9c
long function_19ad9c(XUID const *xuid)
{
	c_class_58d20 *session = NULL;
	long result = NONE;

	if (function_597d0((s_597d0_object **)&session))
		result = function_19ad39(session, xuid);
	return result;
}

// @retail 0x19adca
long function_19adca(XUID const *xuid)
{
	c_class_58d20 *session = NULL;
	long result = NONE;

	if (function_59670(&session))
		result = function_19ad39(session, xuid);
	return result;
}
/* the session manager (unknown_058ee0.cpp) */
void network_session_manager_check_joining_leader(void);
void network_session_manager_leave_session_a(bool close);
void network_session_manager_leave_session_b(bool close);
bool network_session_manager_set_mode(void);

/* resets the peer list's state */
// @retail 0x19987f
void function_19987f(void)
{
	network_session_manager_check_joining_leader();
	memset(&g_4ee4c4, 0, sizeof(g_4ee4c4));
}

void function_19b304(void);
void function_19b40b(void);

/* the peer list's update: the remote players' properties, then the presence */
// @retail 0x199893
void function_199893(void)
{
	function_19b304();
	function_19b40b();
}


// @retail 0x19a942
void function_19a942(void)
{
	if (function_592f0())
		network_session_manager_set_mode();
}
long function_1910d9(void);
s_record_pool *function_19c670();
void function_148d42(long value);
bool g_54e7cc;

/* picks the campaign map to play: the signed in profile's, at least the
   first one (0x69), and makes it the session's */
// @retail 0x199a92
void function_199a92(void)
{
	g_54e7c0 = 1;
	if (function_19c670())
	{
		long map_id = function_1910d9();
		s_entry_a *level;

		if (map_id < 0x69)
			map_id = 0x69;
		if (!function_19c270(1, map_id))
			map_id = 0x69;
		level = function_19c270(1, map_id);
		if (level)
		{
			g_54e7c4 = level->key1;
			function_148d42(g_54e7c8);
			g_54e7cc = false;
			if (function_592f0())
			{
				function_19a6f2(1, level->key1);
				function_19a76d(1);
			}
		}
	}
}

word *function_1902c1(long index);
bool network_session_interface_set_value49a4(long value);
void function_19a864(void);

/* names the session after the first signed in controller and opens the
   campaign (0) or the matchmaking (2) lobby */
// @retail 0x199a03
void function_199a03(long mode)
{
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		word *name = function_1902c1(index);

		if (name)
		{
			function_199bef(name, name);
			break;
		}
	}
	if (mode == 0)
	{
		network_session_interface_set_value49a4(1);
		function_199a92();
	}
	else if (mode == 2)
	{
		network_session_interface_set_value49a4(1);
		function_19a864();
	}
}
