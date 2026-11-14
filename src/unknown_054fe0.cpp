#include "unknown_4cf790.h"
// @flags /O2 /Ob1 /Gr
/* UNKNOWN_054FE0.CPP: the session interface globals (0x4cd868)
   and the queries on the current game session (the manager's session_a) (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"
#include "online_tasks.h"
#include "unknown_0662e0.h"

struct s_member_quality_collection;
void function_7e100(long current, const s_member_quality_collection *collection,
	long *selected, long *first, long *second, long *level);

// @retail 0x63ec0
long function_63ec0(void)
{
	long result = 0;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		if (session->state)
			function_7e100(session->member_index, (const s_member_quality_collection *)((byte *)session + 0x4c), 0, 0, 0, &result);
	}
	return result;
}

/* one local user's state (0xd0 bytes) */
#pragma pack(push, 1)
struct s_session_interface_user
{
	bool valid;
	XUID xuid;
	byte unknown0d[3];
	long unknown10;
	byte properties[0x90];
	long unknowna4;
	long unknowna8[3];
	long unknownb4[3];
	long unknownc0[3];
	byte unknowncc[4];
};
#pragma pack(pop)

struct s_session_interface_globals
{
	bool initialized;
	byte unknown01;
	wchar_t machine_name[16];
	wchar_t session_name[32];
	bool unknown62;
	byte unknown63;
	union
	{
		byte unknown64[32];
		struct
		{
			byte unknown64_00[0x14];
			long unknown78;
		};
	};
	long unknown84;
	long unknown88;
	long unknown8c;
	long unknown90;
	long unknown94;
	long unknown98;
	s_session_interface_user users[4];
	long update3dc[3];
	long unknown3e8[3];
	long value3f4[3];
	byte data400[3][0x130];
	byte unknown790[0x7d4 - 0x790];
	void *session_manager;
};

s_session_interface_globals g_4cd868;

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

static inline c_class_58d20 *network_session_get_live(void)
{
	c_class_58d20 *result = 0;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		long state = session->state;
		if (state && SESSION_STATE_IS_LIVE(state))
			result = session;
	}
	return result;
}

/* compares two Xbox Live user ids, and their guest numbers if asked to */
// @retail 0x63d00
inline bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number)
{
	bool result = false;

	if (a && b)
	{
		if (compare_guest_number)
			result = XOnlineAreUsersIdentical(a, b);
		else
			result = a->qwUserID == b->qwUserID;
	}
	return result;
}

// @retail 0x63d50
bool network_session_interface_initialize(void *session_manager)
{
	memset(&g_4cd868, 0, sizeof(g_4cd868));
	g_4cd868.session_manager = session_manager;
	g_4cd868.initialized = true;
	g_4cd868.unknown98 = NONE;
	return true;
}

// @retail 0x63dc0
void network_session_interface_clear_user_slot(long index)
{
	g_4cd868.unknown3e8[index] = 0;
	for (short i = 0; i < 4; i++)
	{
		g_4cd868.users[i].unknowna8[index] = 0;
		g_4cd868.users[i].unknownb4[index] = 0;
		g_4cd868.users[i].unknownc0[index] = 0;
		g_4cd868.users[i].unknowncc[index] = 0;
	}
}

static inline bool network_session_get_current_if_valid(c_class_58d20 **session)
{
	bool result = false;
	if (g_527330.initialized)
	{
		c_class_58d20 *current = (c_class_58d20 *)g_527330.session_a;
		if (current->state)
		{
			*session = current;
			result = true;
		}
	}
	return result;
}

// @retail 0x63f00
bool network_session_interface_local_machine_is_host(void)
{
	bool result = false;
	c_class_58d20 *session;
	if (network_session_get_current_if_valid(&session) && session_state_is_live(session))
	{
		if (function_058d50(session) && session->type == 1)
			result = true;
	}
	return result;
}

// @retail 0x64160
long network_session_interface_get_value_49a4(void)
{
	long result = 0;
	c_class_58d20 *session = network_session_get_live();
	if (session && SESSION_STATE_IS_LIVE(session->state))
		result = session->value49a4;
	return result;
}

struct s_game_variant;
struct s_session_data4db0;
struct s_161c90;
long function_161c90(const s_161c90 *value);
bool network_session_parameters_set_data4db0(c_class_58d20 *session, const s_session_data4db0 *data);
bool network_session_parameters_set_value4dac(c_class_58d20 *session, long value);

static inline bool session_interface_stop_countdown(c_class_58d20 *session)
{
	return network_session_start_countdown(session, false, 0, 0, 0);
}

// @retail 0x64060
bool __stdcall function_64060(s_game_variant *variant)
{
	bool result = false;
	if (network_session_interface_local_machine_is_host())
	{
		c_class_58d20 *session = 0;
		network_session_get_current_if_valid(&session);
		long mode;
		if (session->value18 == 0)
			mode = 0;
		else if (!variant)
			mode = 1;
		else
			mode = function_161c90((const s_161c90 *)variant);
		result = network_session_parameters_set_data4db0(session, (const s_session_data4db0 *)variant) &&
			network_session_parameters_set_value4dac(session, mode) &&
			session_interface_stop_countdown(session);
	}
	return result;
}

inline long network_session_interface_get_value_49c8(void);

// @retail 0x641f0
long network_session_interface_get_value_49c8(void)
{
	long result = NONE;
	c_class_58d20 *session = network_session_get_live();
	if (session)
		result = session->get_value_49c8();
	return result;
}

// @retail 0x64260
long network_session_interface_get_value_498c(void)
{
	long result = 0;
	c_class_58d20 *session = network_session_get_live();
	if (session && SESSION_STATE_IS_LIVE(session->state))
		result = session->value498c;
	return result;
}

// @retail 0x642d0
byte *network_session_interface_get_data_49a1(void)
{
	byte *result = 0;
	c_class_58d20 *session = network_session_get_live();
	if (session && SESSION_STATE_IS_LIVE(session->state))
		result = session->data49a1;
	return result;
}

// @retail 0x645d0
void network_session_interface_set_local_name(const wchar_t *machine_name, const wchar_t *session_name)
{
	wcsncpy(g_4cd868.machine_name, machine_name, 15);
	g_4cd868.machine_name[15] = 0;
	wcsncpy(g_4cd868.session_name, session_name, 31);
	g_4cd868.session_name[31] = 0;
}

// @retail 0x64690
void network_session_interface_clear_user(long index)
{
	memset(&g_4cd868.users[index], 0, sizeof(s_session_interface_user));
}

struct s_type_fb9815;
inline void machine_identifier_build(s_type_fb9815 *identifier, long index);

// @retail 0x64610
long __stdcall function_64610(dword *xuid)
{
	long index = NONE;
	XUID generated;
	dword *const *xuid_reference = &xuid;
	for (long i = 0; i < 4; i++)
	{
		if (!g_4cd868.users[i].valid)
		{
			index = i;
			break;
		}
	}
	if (!xuid)
	{
		machine_identifier_build((s_type_fb9815 *)&generated, index);
		xuid = (dword *)&generated;
	}
	s_session_interface_user *user = &g_4cd868.users[index];
	memset(user, 0, sizeof(*user));
	g_4cd868.users[index].valid = true;
	g_4cd868.users[index].xuid = *(XUID *)xuid;
	return index;
}

/* the membership getter below, expanded at the user-state query */
inline bool network_session_get_membership(c_class_58d20 *session, long *value4c, long *host_member_index, long *local_member_index, long *value50, long *member_count, s_session_member **members, long *player_count, dword *player_mask, s_network_session_player **players);

/* how far a local user has got into the session: 0 none, 1 the session is
   not live, 2/3 no player yet, 4 no slot, 5 player out of date, 6 up to date */
// @retail 0x646b0
long network_session_interface_get_user_state(c_class_58d20 *session, long user_index)
{
	if (g_4cd868.users[user_index].valid &&
		!(online_logon_connected() && g_4cd868.users[user_index].xuid.dwUserFlags == 0xbad00000))
	{
		long host_member_index;
		s_session_member *members;
		s_network_session_player *players;
		if (network_session_get_membership(session, NULL, &host_member_index, NULL, NULL, NULL, &members, NULL, NULL, &players))
		{
			long player_index = members[host_member_index].player_indices[user_index];
			if (player_index != NONE)
			{
				s_network_session_player *player = &players[player_index];
				if (player->unknown14 != NONE)
				{
					if (player->unknown14 == g_4cd868.users[user_index].unknown10 &&
						memcmp(player->properties18, g_4cd868.users[user_index].properties, sizeof(player->properties18)) == 0 &&
						player->unknown138 == g_4cd868.users[user_index].unknowna4)
						return 6;
					return 5;
				}
				return 4;
			}
			return g_4cd868.users[user_index].unknowncc[session->value10] ? 3 : 2;
		}
		return 1;
	}
	return 0;
}

// @retail 0x647a0
bool network_session_interface_get_user_xuid(long index, XUID *xuid)
{
	bool result = false;
	if (g_4cd868.users[index].valid)
	{
		*xuid = g_4cd868.users[index].xuid;
		result = true;
	}
	return result;
}

// @retail 0x647d0
bool network_session_interface_get_user_properties(long index, long *unknown10, byte *properties, long *unknowna4)
{
	bool result = false;
	if (g_4cd868.users[index].valid)
	{
		if (unknown10)
			*unknown10 = g_4cd868.users[index].unknown10;
		if (properties)
			memcpy(properties, g_4cd868.users[index].properties, sizeof(g_4cd868.users[index].properties));
		if (unknowna4)
			*unknowna4 = g_4cd868.users[index].unknowna4;
		result = true;
	}
	return result;
}

// @retail 0x64820
void network_session_interface_set_user_xuid(long index, const XUID *xuid)
{
	g_4cd868.users[index].xuid = *xuid;
}

// @retail 0x64840
void network_session_interface_set_user_properties(long index, long unknown10, const byte *properties, long unknowna4)
{
	g_4cd868.users[index].unknown10 = unknown10;
	memcpy(g_4cd868.users[index].properties, properties, sizeof(g_4cd868.users[index].properties));
	g_4cd868.users[index].unknowna4 = unknowna4;
}




// @retail 0x64870
void network_session_interface_set_unknown64(const byte *data, long unknown84)
{
	g_4cd868.unknown62 = true;
	memcpy(g_4cd868.unknown64, data, sizeof(g_4cd868.unknown64));
	g_4cd868.unknown84 = unknown84;
	g_4cd868.unknown88 = g_4cf968;
	g_4cd868.unknown8c = g_4cf96c;
}

// @retail 0x648b0
bool network_session_interface_get_unknown64(byte *data, long *unknown84, long *unknown88, long *unknown8c)
{
	bool result = false;
	if (g_4cd868.unknown62)
	{
		memcpy(data, g_4cd868.unknown64, sizeof(g_4cd868.unknown64));
		*unknown84 = g_4cd868.unknown84;
		*unknown88 = g_4cd868.unknown88;
		*unknown8c = g_4cd868.unknown8c;
		result = true;
	}
	return result;
}

/* the session the current mode works on (unknown_01cf50.cpp) */
bool function_597d0(s_597d0_object **out);

static inline bool network_session_get(c_class_58d20 **session)
{
	return function_597d0((s_597d0_object **)session);
}

// @retail 0x64970
bool network_session_interface_get_values_4d08(long *a, long *b, byte **c)
{
	bool result = false;
	c_class_58d20 *session = 0;
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
		result = session->get_values_4d08(a, b, c);
	return result;
}

static inline byte *session_get_data_4db0(c_class_58d20 *session)
{
	byte *result = 0;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->data4db0;
	return result;
}

// @retail 0x649c0
byte *network_session_interface_get_data_4db0(void)
{
	byte *result = 0;
	c_class_58d20 *session = 0;
	bool found = network_session_get(&session);
	if (found)
	{
		long state = session->state;
		if (state > 2)
		{
			if (state <= 8)
				result = session_get_data_4db0(session);
		}
	}
	return result;
}

static inline short session_get_value_5dd0(c_class_58d20 *session)
{
	short result = NONE;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->value5dd0;
	return result;
}

// @retail 0x64a10
short network_session_interface_get_value_5dd0(void)
{
	c_class_58d20 *session = 0;
	short result = NONE;
	bool found = network_session_get(&session);
	if (found)
	{
		long state = session->state;
		if (state > 2)
		{
			if (state <= 8)
				result = session_get_value_5dd0(session);
		}
	}
	return result;
}

// @retail 0x64a60
long network_session_interface_get_value_49ac(void)
{
	long result = NONE;
	c_class_58d20 *session = network_session_get_live();
	if (session)
	{
		result = NONE;
		if (SESSION_STATE_IS_LIVE(session->state) && session->flag49a8)
			result = session->value49ac;
	}
	return result;
}

// @retail 0x64ab0
long network_session_interface_get_value_49b0(void)
{
	long result = 0;
	c_class_58d20 *session = network_session_get_live();
	if (session)
	{
		result = 0;
		if (SESSION_STATE_IS_LIVE(session->state) && session->flag49a8)
			result = session->value49b0;
	}
	return result;
}

static inline byte *session_get_data_49b8(c_class_58d20 *session)
{
	byte *result = 0;
	if (SESSION_STATE_IS_LIVE(session->state) && session->flag49a8 && session->value49b0 == 1)
		result = session->data49b8;
	return result;
}

// @retail 0x64b00
long network_session_interface_find_player_49b8(void)
{
	long result = NONE;
	c_class_58d20 *session = network_session_get_live();
	if (session)
	{
		byte *key = session_get_data_49b8(session);
		if (key)
		{
			for (long i = 0; i < 16; i++)
			{
				if ((session->player_mask & (1 << i)) && !memcmp(&session->players[i], key, 12))
				{
					result = i;
					break;
				}
			}
		}
	}
	return result;
}

bool session_get_id(c_class_58d20 *session, s_session_id *id, byte *key)
{
	bool result = false;
	if (session->state && session->flag24)
	{
		if (id)
		{
			id->a = session->unknown1c;
			id->b = session->unknown20;
		}
		if (key)
			memcpy(key, session->data25, sizeof(session->data25));
		result = true;
	}
	return result;
}

// @retail 0x64bc0
bool network_session_interface_get_id(s_session_id *id, byte *key)
{
	bool result = false;
	c_class_58d20 *session = 0;
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
	{
		result = false;
		if (session->state && session->flag24)
		{
			if (id)
			{
				id->a = session->unknown1c;
				id->b = session->unknown20;
			}
			if (key)
				memcpy(key, session->data25, sizeof(session->data25));
			result = true;
		}
	}
	return result;
}

// @retail 0x64c40
long network_session_interface_get_value_18(void)
{
	long result = NONE;
	c_class_58d20 *session = network_session_get_live();
	if (session)
		result = session->value18;
	return result;
}

static inline s_network_session_player *session_get_player(c_class_58d20 *session, dword index)
{
	s_network_session_player *result = 0;
	if (session->player_mask & (1 << index))
		result = &session->players[index];
	return result;
}

// @retail 0x64c70
bool network_session_interface_has_user(const XUID *xuid)
{
	union { byte field_0; bool field_1; } local_0;
 local_0.field_0 = 0;
	c_class_58d20 *session = 0;
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
	{
		for (dword i = 0; i < 16; i++)
		{
			s_network_session_player *player = session_get_player(session, i);
			if (player && xuid && player->user_id == xuid->qwUserID && XOnlineUserGuestNumber(player->user_flags) == XOnlineUserGuestNumber(xuid->dwUserFlags))
				{ local_0.field_0 = 1; goto local_1; }
		}
	}
local_1:
	return local_0.field_1;
}

static inline s_long_pair *session_get_data_4999(c_class_58d20 *session)
{
	s_long_pair *result = 0;
	if (session->flag4998)
		result = &session->data4999;
	return result;
}

// @retail 0x64cf0
s_long_pair *network_session_interface_get_data_4999(void)
{
	s_long_pair *result = 0;
	if (g_527330.initialized)
	{
		long state = ((c_class_58d20 *)g_527330.session_a)->state;
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		if (state && SESSION_STATE_IS_LIVE(state) && session->flag4998)
			result = &session->data4999;
	}
	return result;
}

/* a 12-byte key compared from its last dword to its first */
struct s_sort_key
{
	long low;
	long middle;
	long high;
};

// @retail 0x64d30
bool __stdcall sort_key_less_than(const s_sort_key *a, const s_sort_key *b, void *context)
{
 if (a->high < b->high)
  return true;
 else if (a->high > b->high)
  return false;
 else
 {
  if (a->middle < b->middle)
   return true;
  else if (a->middle > b->middle)
   return false;
  else if (a->low < b->low)
   return false;
  else
  {
   byte local_0 = a->low > b->low;
   return local_0;
  }
 }
}

// @retail 0x65280
long network_session_interface_get_value_90(long *value94)
{
	if (value94)
		*value94 = g_4cd868.unknown94;
	return g_4cd868.unknown90;
}

// @retail 0x652a0
long network_session_interface_get_members_status(long *progress)
{
	c_class_58d20 *session = 0;
	long status = 0;
	long lowest = 0;
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
	{
		long member_count = session->member_count;
		status = 4;
		lowest = 100;
		long i = 0;
		if (member_count > 0)
		{
			long *local_0 = &session->members[0].unknown88;
			do
			{
				long member_status = local_0[0];
				if (member_status == 1)
				{
					status = 1;
					lowest = 0;
					break;
				}
				if (member_status != 4)
				{
					if (member_status == 3)
					{
						if (status == 4)
							status = member_status;
					}
					else if (member_status == 2)
					{
						status = member_status;
						if (lowest > local_0[1])
							lowest = local_0[1];
					}
				}
				i++;
				local_0 = (long *)((byte *)local_0 + 0x10c);
			} while (i < member_count);
		}
	}
	if (progress)
		*progress = lowest;
	return status;
}

static inline long session_get_value_498c(c_class_58d20 *session)
{
	long result = 0;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->value498c;
	return result;
}

static inline long session_get_maximum_player_count(c_class_58d20 *session)
{
	long result = 16;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->value4994;
	return result;
}

static __forceinline short session_get_value_5dd0_inline(c_class_58d20 *session)
{
	short result = NONE;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->value5dd0;
	return result;
}

// @retail 0x63e00
bool network_session_interface_can_add_player(void)
{
	bool result = false;
	c_class_58d20 *session;
	if (network_session_get_current_if_valid(&session) && session_state_is_live(session))
		result = session_get_value_498c(session) == 0 &&
			session->player_count < session_get_maximum_player_count(session) &&
			g_527330.state != 6 && g_527330.state != 7 && g_527330.state != 8 && g_527330.state != 9 &&
			session_get_value_5dd0_inline(session) == NONE;
	return result;
}

// @retail 0x656e0
bool network_session_get_membership(c_class_58d20 *session, long *value4c, long *host_member_index, long *local_member_index, long *value50, long *member_count, s_session_member **members, long *player_count, dword *player_mask, s_network_session_player **players)
{
	bool result = false;
	if (SESSION_STATE_IS_LIVE(session->state))
	{
		long host = session->current_member;
		long local = session->member_index;
		if (value4c)
			*value4c = session->value4c;
		if (host_member_index)
			*host_member_index = host;
		if (local_member_index)
			*local_member_index = local;
		if (value50)
			*value50 = session->value50;
		if (member_count)
			*member_count = session->member_count;
		if (members)
			*members = session->members;
		if (player_count)
			*player_count = session->player_count;
		if (player_mask)
			*player_mask = session->player_mask;
		if (players)
			*players = session->players;
		result = true;
	}
	return result;
}


/* passes the member record of a player's machine to 0x5aba0, when the
   session's current member is its value50 member */
// @retail 0x64310
bool network_session_interface_kick_player(long player_index)
{
	bool result = false;
	c_class_58d20 *session;
	long host_member_index;
	long value50;
	s_session_member *members;
	dword player_mask;
	s_network_session_player *players;
	if (network_session_get_current_if_valid(&session) &&
		network_session_get_membership(session, NULL, &host_member_index, NULL, &value50, NULL, &members, NULL, &player_mask, &players) &&
		host_member_index == value50 && (player_mask & (1 << player_index)))
	{
		s_network_session_player const &player = players[player_index];
		if (player.member_index != host_member_index &&
			network_session_delegate_leader(session, (const s_session_member_identity *)members[player.member_index].words))
			result = true;
	}
	return result;
}

/* the same, through 0x5acc0 */
// @retail 0x643f0
bool network_session_interface_ban_player(long player_index)
{
	bool result = false;
	c_class_58d20 *session;
	long host_member_index;
	long value50;
	s_session_member *members;
	dword player_mask;
	s_network_session_player *players;
	if (network_session_get_current_if_valid(&session) &&
		network_session_get_membership(session, NULL, &host_member_index, NULL, &value50, NULL, &members, NULL, &player_mask, &players) &&
		host_member_index == value50 && (player_mask & (1 << player_index)))
	{
		s_network_session_player const &player = players[player_index];
		if (player.member_index != host_member_index &&
			network_session_boot_machine(session, (const s_session_member_identity *)members[player.member_index].words))
			result = true;
	}
	return result;
}


static inline c_class_58d20 *network_session_get_current(void)
{
	c_class_58d20 *result = 0;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		if (session->state)
			result = session;
	}
	return result;
}

static inline bool session_is_established(c_class_58d20 *session)
{
	if (SESSION_STATE_IS_LIVE(session->state))
	{
		return true;
	}
	return false;
}

static inline bool session_is_leader(c_class_58d20 *session)
{
	bool result = false;
	if (SESSION_STATE_IS_LIVE(session->state))
	{
		result = session->current_member == session->value50;
	}
	return result;
}

// @retail 0x63f50
bool network_session_interface_set_value4d08(long value4d08, long value4d0c, const char *string)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_current();
	if (session && session_state_is_live(session))
	{
		if (function_058d50(session))
			result = network_session_parameters_set_value4d08(session, value4d08, value4d0c, string);
		else
			result = true;
	}
	return result;
}

// @retail 0x63fb0
bool network_session_interface_set_value49a4(long value)
{
	bool result = false;
	if (network_session_interface_local_machine_is_host())
		result = network_session_parameters_set_value49a4(network_session_get_current(), value);
	return result;
}

// @retail 0x63ff0
bool network_session_interface_set_value4d08_and_stop_countdown(long value4d08, long value4d0c, const char *string)
{
	bool result = false;
	if (network_session_interface_local_machine_is_host())
	{
		c_class_58d20 *session = network_session_get_current();
		result = network_session_parameters_set_value4d08(session, value4d08, value4d0c, string) && network_session_start_countdown(session, false, 0, 0, 0);
	}
	return result;
}

// @retail 0x64100
bool network_session_interface_set_value5dd0(short value)
{
	bool result = false;
	if (network_session_interface_local_machine_is_host())
	{
		c_class_58d20 *session = network_session_get_current();
		result = network_session_parameters_set_value5dd0(session, value) &&
			network_session_start_countdown(session, false, 0, 0, 0);
	}
	return result;
}

bool network_session_parameters_set_value49c8(c_class_58d20 *session, long value);
void function_074aa0(void);
void network_session_interface_update_session(c_class_58d20 *session);

// @retail 0x641a0
bool function_641a0(long value)
{
	bool result = false;
	if (network_session_interface_local_machine_is_host())
	{
		c_class_58d20 *session = network_session_get_current();
		result = network_session_parameters_set_value49c8(session, value);
		function_074aa0();
		network_session_interface_update_session(session);
	}
	return result;
}

// @retail 0x64230
bool network_session_interface_set_value498c(long value)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_current();
	if (session && session_is_established(session))
		result = network_session_parameters_set_value498c(session, value);
	return result;
}

// @retail 0x642a0
bool network_session_interface_set_value49a1(const byte *value)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_current();
	if (session && session_is_established(session))
		result = network_session_parameters_set_value49a1(session, value);
	return result;
}

// @retail 0x643a0
bool network_session_interface_set_value49c4(void)
{
	bool result = false;
	if (g_527330.initialized && g_527330.state == 3)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		if (session->state && function_058d50(session) && network_session_parameters_set_value49c4(session))
			result = true;
	}
	return result;
}

// @retail 0x64900
bool network_session_interface_start_countdown(long user_index, bool start, long countdown, long mode)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_current();
	if (session && session_is_established(session))
	{
		s_session_interface_user *user = &g_4cd868.users[user_index];
		if (user->valid)
		{
			if (network_session_start_countdown(session, start, countdown, mode, (const long *)&user->xuid))
				result = true;
		}
	}
	return result;
}

/* ---- the local users' players in a session (lane D, round 4) ---- */

bool network_session_player_add(c_class_58d20 *session, long slot, const dword *identity, long unknown18, const byte *properties, long unknownac);
bool network_session_player_set_properties(c_class_58d20 *session, const byte *properties, long slot, long volatile unknown0c, long unknowna0);
bool network_session_player_remove(c_class_58d20 *session, long slot);

static inline long session_interface_time_get(void)
{
	long time;
	if (g_510548)
		time = g_51054c;
	else
		time = GetTickCount();
	return time;
}

/* a reserved place of a local user (session +0x761c, 13 bytes each) */
#pragma pack(push, 1)
struct s_session_user_reservation
{
	bool valid;
	XUID xuid;
};
#pragma pack(pop)

// @retail 0x65a60
void network_session_interface_add_user(c_class_58d20 *session, long user_index)
{
	s_session_interface_user *user = &g_4cd868.users[user_index];
	long owner = session->value10;
	long last = user->unknowna8[owner];
	long elapsed = session_interface_time_get() - last;
	if (SESSION_STATE_IS_LIVE(session->state) && (session->member_count > session->value4990 || session->player_count + 1 > session->value4994))
	{
		user->unknowncc[owner] = true;
	}
	else if (elapsed >= g_network_configuration.valuec84)
	{
		if (network_session_player_add(session, user_index, (const dword *)&user->xuid, user->unknown10, user->properties, user->unknowna4))
			user->unknowna8[owner] = session_interface_time_get();
		else
			user->unknowncc[owner] = true;
	}
	else
	{
		if (((s_session_user_reservation *)session->data761c)[user_index].valid &&
			!memcmp(&user->xuid, &((s_session_user_reservation *)session->data761c)[user_index].xuid, sizeof(XUID)))
			user->unknowncc[owner] = true;
	}
}

// @retail 0x65b80
void network_session_interface_remove_user(c_class_58d20 *session, long user_index, long slot)
{
	s_session_interface_user *user = &g_4cd868.users[user_index];
	long owner = session->value10;
	if (slot != NONE)
	{
		long last = user->unknownc0[owner];
		if (session_interface_time_get() - last >= g_network_configuration.valuec88 && network_session_player_remove(session, user_index))
			user->unknownc0[owner] = session_interface_time_get();
	}
}

// @retail 0x65c10
void network_session_interface_update_user(long user_index, c_class_58d20 *session)
{
	s_session_interface_user *user = &g_4cd868.users[user_index];
	long owner = session->value10;
	long last = user->unknownb4[owner];
	if (session_interface_time_get() - last >= g_network_configuration.valuec8c && network_session_player_set_properties(session, user->properties, user_index, user->unknown10, user->unknowna4))
		user->unknownb4[owner] = session_interface_time_get();
}
/* ---- the periodic update of a session's local users and properties (lane D, round 13) ---- */

#include "unknown_075870.h"

long samples_trimmed_mean(const long *samples, long count);
long game_variant_get_map_status(long index);
long __stdcall function_73b10(long a, long b);
bool __stdcall function_07f660(wchar_t *name, long length, const wchar_t *requested, long count, const wchar_t **names);
long function_11cae0(void);
long network_session_get_language(c_class_58d20 *session);
bool network_session_parameters_set_language(c_class_58d20 *session, long language);
bool network_session_set_local_properties(c_class_58d20 *session, const s_session_parameters *properties);
bool network_session_host_set_player_properties(c_class_58d20 *session, long player_index, const byte *properties);

s_session_id g_510520;
s_session_id g_510540;

/* the round trip times to the session's other members */
struct s_session_member_latency
{
	dword member_mask;
	long minimum;
	long average;
	long maximum;
};

static inline s_network_observer *network_session_observer_get(void)
{
	s_network_observer *result = 0;
	if (g_527330.initialized)
		result = (s_network_observer *)g_527330.unknown48;
	return result;
}

static inline s_network_session_member_state *session_member_state_get(c_class_58d20 *session, long member_index)
{
	s_network_session_member_state *result = 0;
	if (member_index >= 0 && member_index < session->member_count && session->member_states[member_index].flag1)
		result = &session->member_states[member_index];
	return result;
}

static inline long session_member_channel_get(c_class_58d20 *session, long member_index)
{
	long result = NONE;
	if (member_index >= 0 && member_index < session->member_count && session->member_states[member_index].flag1)
		result = session->member_states[member_index].unknown04;
	return result;
}

static inline bool observer_channel_get_qos(s_network_observer *observer, long channel_index, s_qos_result *qos)
{
	bool result = false;
	s_network_observer_channel *channel = &observer->channels[channel_index];
	if (channel->state == 7 && (channel->flags & 0x10))
	{
		*qos = channel->field_x31a738;
		result = true;
	}
	return result;
}

static inline bool session_member_get_qos(s_network_observer *observer, c_class_58d20 *session, long member_index, s_qos_result *qos)
{
	bool result = false;
	long channel_index = session_member_channel_get(session, member_index);
	if (channel_index != NONE)
		result = observer_channel_get_qos(observer, channel_index, qos);
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x64480
void network_session_get_member_latency(c_class_58d20 *session, s_session_member_latency *latency)
{
	s_network_observer *observer = 0;
	long count = 0;
	long maximum = 0x80000000;
	long minimum = 0x7fffffff;
	long times[16];
	if (g_527330.initialized)
		observer = (s_network_observer *)g_527330.unknown48;
	long host = session->current_member;
	memset(latency, 0, sizeof(*latency));
	for (long i = 0; i < session->member_count; i++)
	{
		if (i != host)
		{
			s_qos_result qos;
			/* the member count is read again for the member check */
			_ReadWriteBarrier();
			if (!session_member_get_qos(observer, session, i, &qos))
				continue;
			long time = qos.rtt_median;
			if (time < 50)
				time = 50;
			else if (time > 2000)
				time = 2000;
			times[count++] = time;
			if (maximum <= time)
				maximum = time;
			if (minimum > time)
				minimum = time;
		}
		latency->member_mask |= 1 << i;
	}
	if (count > 0)
	{
		latency->minimum = minimum;
		latency->maximum = maximum;
		latency->average = samples_trimmed_mean(times, count);
	}
}

static inline long network_session_time_matches(s_session_id key)
{
	long result = 0;
	if (*(__int64 *)&g_510520 == *(__int64 *)&key)
		result = function_73b10(g_510540.a, g_510540.b);
	return result;
}

static inline long session_get_value_49f0(c_class_58d20 *session)
{
	long result = 0;
	if (SESSION_STATE_IS_LIVE(session->state) && session->flag49e8)
	{
		s_session_id key;
		key.a = session->value49f0;
		key.b = session->value49f4;
		result = network_session_time_matches(key);
	}
	return result;
}

/* sends the local machine's properties to the session when they change */
// @retail 0x658c0
static void network_session_interface_update_local_properties(c_class_58d20 *session, s_session_member *member)
{
	long owner = session->value10;
	long last = g_4cd868.unknown3e8[owner];
	if (session_interface_time_get() - last >= g_network_configuration.valuec80)
	{
		s_session_parameters properties;
		wcsncpy(properties.name, g_4cd868.machine_name, 15);
		properties.name[15] = 0;
		wcsncpy(properties.description, g_4cd868.session_name, 31);
		properties.description[31] = 0;
		properties.unknown68 = 0;
		properties.unknown6c = 0;
		properties.unknown70 = 0;
		network_session_get_member_latency(session, (s_session_member_latency *)properties.unknown74);
		if (g_4cd868.unknown62)
		{
			properties.unknown68 = g_4cd868.unknown78;
			properties.unknown70 = g_4cd868.unknown84;
		}
		properties.unknown6c = g_4cf968;
		properties.unknown60 = g_4cd868.unknown90;
		properties.unknown64 = g_4cd868.unknown94;
		for (long i = 0; i < 16; i++)
			((long *)properties.unknown84)[i] = game_variant_get_map_status(i);
		properties.unknownc4 = session_get_value_49f0(session);
		if (!member->properties_valid || memcmp(&member->properties, &properties, sizeof(properties)) != 0)
		{
			if (network_session_set_local_properties(session, &properties))
				g_4cd868.unknown3e8[owner] = session_interface_time_get();
		}
	}
}

/* the host gives the players their properties: unique names, and teams when
   the game has teams */
// @retail 0x65cb0
void network_session_interface_update_player_properties(c_class_58d20 *session)
{
	long state = session->state;
	if (state == 7 || state == 6 || state == 8)
		return;

	long owner = session->value10;
	byte *parameters = session_get_data_4db0(session);
	if (parameters)
    {
	if (!memcmp(g_4cd868.data400[owner], parameters, sizeof(g_4cd868.data400[owner])) && g_4cd868.value3f4[owner] == session->value4c)
		return;

	const wchar_t *names[16];
	long name_count = 0;
	long i;
	dword mask = session->player_mask;
	for (i = 0; i < 16; i++)
	{
		if (mask & (1 << i))
			names[name_count++] = (const wchar_t *)session->players[i].propertiesa8;
	}
	for (i = 0; i < 16; i++)
	{
		if (session->player_mask & (1 << i))
		{
			s_player_properties properties;

			memset(&properties, 0, sizeof(properties));
			memset(properties.unknown40, 0, sizeof(properties.unknown40));
			memset(properties.unknown70, 0, sizeof(properties.unknown70));
			properties.team = 0;
			properties.unknown7e = 0xff;
			properties.unknown7f = 0xff;
			properties.unknown84 = NONE;
			properties.unknown8c = NONE;
			properties.unknown88 = NONE;
			properties.unknown8a = NONE;
			properties.unknown7d = 0;

			if (wcslen(((const s_player_properties *)session->players[i].properties18)->name) > 0 && wcslen(((const s_player_properties *)session->players[i].propertiesa8)->name) == 0)
			{
				function_07f660(properties.name, 32, ((const s_player_properties *)session->players[i].properties18)->name, name_count, names);
			}
			else
			{
				wcsncpy(properties.name, ((const s_player_properties *)session->players[i].propertiesa8)->name, 31);
				properties.name[31] = 0;
			}
			memcpy(properties.unknown40, ((const s_player_properties *)session->players[i].properties18)->unknown40, sizeof(properties.unknown40));
			memcpy(properties.unknown70, ((const s_player_properties *)session->players[i].properties18)->unknown70, sizeof(properties.unknown70));
			memcpy(properties.unknown50, ((const s_player_properties *)session->players[i].properties18)->unknown50, sizeof(properties.unknown50));
			if (session->value14 == 1)
			{
				char team = ((const s_player_properties *)session->players[i].properties18)->team;
				if (team == NONE)
					properties.team = team;
				else if (parameters[0x48] & 1)
				{
					if (team < 0)
						properties.team = 0;
					else
					{
						long team_value = team;
						if (team_value > 7)
							properties.team = 7;
						else
							properties.team = (char)team_value;
					}
				}
				else
					properties.team = (char)i;
				properties.unknown84 = ((const s_player_properties *)session->players[i].properties18)->unknown84;
				properties.unknown8c = ((const s_player_properties *)session->players[i].properties18)->unknown8c;
				properties.unknown8a = ((const s_player_properties *)session->players[i].properties18)->unknown8a;
				properties.unknown88 = ((const s_player_properties *)session->players[i].properties18)->unknown88;
			}
			else
			{
				properties.team = ((const s_player_properties *)session->players[i].propertiesa8)->team;
				properties.unknown84 = ((const s_player_properties *)session->players[i].propertiesa8)->unknown84;
				properties.unknown8c = ((const s_player_properties *)session->players[i].propertiesa8)->unknown8c;
				properties.unknown8a = ((const s_player_properties *)session->players[i].propertiesa8)->unknown8a;
				properties.unknown88 = ((const s_player_properties *)session->players[i].propertiesa8)->unknown88;
			}
			properties.unknown7e = ((const s_player_properties *)session->players[i].properties18)->unknown7e;
			properties.unknown7f = ((const s_player_properties *)session->players[i].properties18)->unknown7f;
			properties.unknown7d = ((const s_player_properties *)session->players[i].properties18)->unknown7d;
			properties.unknown80 = ((const s_player_properties *)session->players[i].properties18)->unknown80;
			properties.unknown81 = ((const s_player_properties *)session->players[i].properties18)->unknown81;
			if (memcmp(((const s_player_properties *)session->players[i].propertiesa8), &properties, sizeof(properties)) != 0)
				network_session_host_set_player_properties(session, i, (const byte *)&properties);
		}
	}
	g_4cd868.value3f4[owner] = session->value4c;
	memcpy(g_4cd868.data400[owner], parameters, sizeof(g_4cd868.data400[owner]));
    }
	else
	{
		memset(g_4cd868.data400[owner], 0, sizeof(g_4cd868.data400[owner]));
	}
}

/* the periodic update of one session: the local users' players, the local
   machine's properties and, on the host, the players' properties */
// @retail 0x65770
void network_session_interface_update_session(c_class_58d20 *session)
{
	long owner = session->value10;
	long update = session->update7650;
	if (update != g_4cd868.update3dc[owner])
	{
		network_session_interface_clear_user_slot(owner);
		g_4cd868.update3dc[owner] = update;
	}
	if (session_state_is_live(session) && !function_058d90(session))
	{
		s_session_member *member = &session->members[session->current_member];
		if (function_058d50(session) && session->value14 == 1)
		{
			long language = function_11cae0();
			if (language != network_session_get_language(session))
				network_session_parameters_set_language(session, language);
		}
		network_session_interface_update_local_properties(session, member);
		for (long user_index = 0; user_index < 4; user_index++)
		{
			long user_state = network_session_interface_get_user_state(session, user_index);
			long player_index = member->player_indices[user_index];
			s_network_session_player *volatile player = 0;
			if (player_index != NONE)
				player = &session->players[player_index];
			switch (user_state)
			{
			case 2:
				network_session_interface_add_user(session, user_index);
				break;
			case 4:
				network_session_interface_update_user(user_index, session);
				break;
			case 5:
				network_session_interface_update_user(user_index, session);
				break;
			case 0:
				network_session_interface_remove_user(session, user_index, player_index);
				break;
			case 1:
				break;
			case 3:
				break;
			case 6:
				break;
			default:
				__assume(0);
			}
		}
		if (session->function_058d20())
			network_session_interface_update_player_properties(session);
	}
}

void function_065340(void);

// @retail 0x63d80
void network_session_interface_update(void)
{
	function_065340();
	for (long i = 0; i < 3; i++)
	{
		c_class_58d20 *session = ((c_class_58d20 **)g_4cd868.session_manager)[i];
		if (session->state)
			network_session_interface_update_session(session);
	}
}

dword voice_get_player_flags(long index);

// @retail 0x54fe0
void voice_update_local_properties(long controller_index)
{
	long const *controller_reference = &controller_index;
	long user_index = *(long *)g_54e8e0[controller_index].unknown004;
	if (user_index != NONE && g_4cd868.users[user_index].valid)
	{
		unsigned __int64 properties[0x12];
		memcpy(properties, g_4cd868.users[user_index].properties, sizeof(properties));
		dword flags = voice_get_player_flags(controller_index);
		g_4cd868.users[user_index].unknown10 = controller_index;
		memcpy(g_4cd868.users[user_index].properties, properties, sizeof(properties));
		g_4cd868.users[user_index].unknowna4 = flags;
	}
}

struct s_surface_description
{
 long type;
 byte unknown04[0x5e4 - 4];
 long field5e4;
 byte unknown5e8[4];
 long field5ec;
 long field5f0;
 long field5f4;
 byte unknown5f8[4];
 long field5fc;
 byte unknown600[0x14];
};
struct s_game_variant_globals
{
 dword unknown0;
 dword flags;
 word count;
 word state;
};
extern s_surface_description g_551ae8[16];
extern s_game_variant_globals g_47d8f4;
long game_variant_get_map_status(long index);
bool function_1934f0(s_surface_description *variant);
bool function_73a10(long player, long group_index, long field, long *value);
struct s_cached_player_identity;
struct s_cached_player_source;
void function_80490(const s_cached_player_identity *identity, const s_cached_player_source *source,
 bool local, bool recent);

// @retail 0x65530
void __stdcall function_65530(long user_index)
{
 long maximum = NONE;
 long levels[16];
 memset(levels, NONE, sizeof(levels));
 for (long i = 0; i < 16; i++)
 {
  if (game_variant_get_map_status(i) == 1)
  {
   s_surface_description *variant = 0;
   if (i >= 0 && i < 16 && g_47d8f4.count && (g_47d8f4.flags & 2) &&
    function_1934f0(&g_551ae8[i]))
    variant = &g_551ae8[i];
   if (variant && (variant->type == 5 || variant->type == 2 || variant->type == 4))
   {
    if (!function_73a10(i, user_index, 1, &levels[i]))
    {
     maximum = NONE;
     break;
    }
    if (maximum <= levels[i])
     maximum = levels[i];
   }
  }
 }
 g_4cd868.users[user_index].properties[0x7f] = (byte)maximum;
 if (g_4cd868.unknown98 >= 0 && g_4cd868.unknown98 < 16)
  g_4cd868.users[user_index].properties[0x7e] = (byte)levels[g_4cd868.unknown98];
 else
  g_4cd868.users[user_index].properties[0x7e] = 0xff;
 long variant = network_session_interface_get_value_49c8();
 long value, level;
 if (variant != NONE && function_73a10(variant, user_index, 0, &value) &&
  function_73a10(variant, user_index, 1, &level))
 {
  *(long *)&g_4cd868.users[user_index].properties[0x84] = variant;
  *(long *)&g_4cd868.users[user_index].properties[0x8c] = value;
  *(short *)&g_4cd868.users[user_index].properties[0x8a] = (short)level;
 }
 else
 {
  *(long *)&g_4cd868.users[user_index].properties[0x84] = NONE;
  *(long *)&g_4cd868.users[user_index].properties[0x8c] = NONE;
  *(short *)&g_4cd868.users[user_index].properties[0x8a] = NONE;
 }
 *(word *)&g_4cd868.users[user_index].properties[0x88] = 0xffff;
 if (!(g_4cd868.users[user_index].xuid.dwUserFlags & 3) && g_4cd868.users[user_index].xuid.dwUserFlags != 0xbad00000)
  function_80490((const s_cached_player_identity *)&g_4cd868.users[user_index].xuid,
   (const s_cached_player_source *)g_4cd868.users[user_index].properties, true, false);
}

struct s_message_identities;
struct s_message_identity;
bool function_7ed20(const s_message_identities *message, s_message_identity *common, bool *missing, bool *different);
s_surface_description *function_192e60(long index);
long function_193250(s_surface_description *variant);

struct s_66050
{
    byte field_0[8];
    long field_8;
    s_session_member field_c[16];
    long field_10cc;
    dword field_10d0;
    s_network_session_player field_10d4[16];
};

#pragma optimize("t", on)
#pragma inline_depth(0)
struct s_type_66052
{
 byte unknown00[0xac];
 long field_ac[16];
 byte unknownec[0x20];
};
// @retail 0x66050
long function_66050(c_class_58d20 *session, long variant_index)
{
 long result;
 long local_1 = variant_index;
 bool different;
 bool compatible;
 bool missing;
 long current = session->member_index;
 missing = false;
 s_66050 *local_0 = (s_66050 *)&session->value4c;
 compatible = function_7ed20((const s_message_identities *)local_0, 0, &missing, &different);
 s_surface_description *variant = function_192e60(local_1);
 result = 0;
 if (!variant)
 {
  result = 2;
  goto local_66050;
 }
 {
 if (variant->type == 5 || variant->type == 2 || variant->type == 4)
 {
  for (long i = 0; i < 16; i++)
   if ((local_0->field_10d0 & (1 << i)) && (local_0->field_10d4[i].user_flags & 3))
   {
    result = 12;
    goto local_66050;
   }
 }
 dword mask = local_0->field_10d0;
 for (long i = 0; i < 16; i++)
  if ((mask & (1 << i)) && local_0->field_10d4[i].user_flags == 0xbad00000)
  {
   result = 13;
   goto local_66050;
  }
 for (long member = 0; member < local_0->field_8; member++)
 {
  const s_type_66052 *entry = (const s_type_66052 *)&local_0->field_c[member];
  if (entry->field_ac[local_1] != 1)
  {
   long state = entry->field_ac[local_1];
   if (!state)
    result = 14;
   else
    {
     long outcome = state == 3 ? 2 : 11;
     result = outcome;
     goto local_66050;
    }
   goto local_66050;
  }
 }
 for (long i = 0; i < 16; i++)
  if ((mask & (1 << i)) && (signed char)local_0->field_10d4[i].propertiesa8[0x81] < variant->field5e4)
  {
   result = 15;
   goto local_66050;
  }
 const long &type = variant->type;
 if (type == 5 && !compatible)
 {
  long outcome = missing ? 8 : 9;
  result = outcome;
  goto local_66050;
 }
 long minimum;
 if (type > 3)
 {
  if (type != 4)
   minimum = variant->field5f0;
  else
   minimum = variant->field5fc;
 }
 else
  minimum = 1;
 long count = local_0->field_10cc;
 if (count < minimum)
 {
  result = 6;
  goto local_66050;
 }
 if (count > function_193250(variant))
 {
  result = 7;
  goto local_66050;
 }
 if (type == 5)
 {
  function_7e100(current, (const s_member_quality_collection *)local_0, 0, 0, 0, &current);
  long required;
  switch (variant->type)
  {
  case 1: required = variant->field5f0; break;
  case 2: required = variant->field5f0; break;
  case 3: required = variant->field5ec * variant->field5f4; break;
  case 4: required = variant->field5ec * variant->field5f4; break;
  case 5: required = variant->field5ec * variant->field5f4; break;
  default: __assume(0);
  }
  if (current < required)
   result = 10;
 }
 }
local_66050:
 return result;
}
#pragma inline_depth(8)
#pragma optimize("", on)

// @retail 0x63e90
long __stdcall function_63e90(long index)
{
 if (g_527330.initialized)
 {
  c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
  if (session->state)
   return function_66050(session, index);
 }
 return 1;
}


typedef bool (__stdcall *t_session_summary_compare)(const void *, const void *, const void *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size,
 t_session_summary_compare compare, const void *context);
long network_session_get_maximum_players(c_class_58d20 *session);
long player_index_from_absolute_index(long index);
long function_1587b0(long player_index);
long function_1587f0(long team);
void function_158850(long *type, long *count);

#pragma pack(push, 1)
struct s_session_browser_summary
{
 short version;
 short field_02;
 long protocol;
 long build;
 long compatible_build;
 short type;
 short local;
 short mode;
 short field_16;
 wchar_t name[32];
 s_session_id id;
 byte key[16];
 dword identity[9];
 short public_open;
 short private_open;
 short public_used;
 short private_used;
 short variant;
 short state;
 long game_type;
 long map;
 long option;
 long map_variant;
 bool teams;
 byte field_b1[3];
 long score_type;
 long score_limit;
 short player_count;
 byte player_keys[16][12];
 wchar_t player_names[16][32];
 short field_57e;
 long scores[16];
 short player_teams[16];
 byte field_5e0[16][16];
 dword team_mask;
 long team_scores[8];
 byte field_704[16];
};
#pragma pack(pop)

struct s_summary_player_sort_entry
{
 s_sort_key sort;
 dword key[3];
 byte properties[0x90];
};

// MiniMax B (11-14): no new form, no lower score (339): retail's conditional move for the state store is still out of
// reach - it needs the SSE architecture flag this file's flags do not set, and the flags stay as they are - and the
// frame stays 0xa9c against retail's 0xaa8, so the type, membership and member-index homes keep their different
// offsets. The body stands.
// MiniMax H (11-13): no new form, no lower score (339): the frame is still 0xa9c against retail's 0xaa8; retail's
// conditional move for output->state needs /arch:SSE, which this file's flags do not set, and the inlined 0x5a680
// twin is not allowed, so the state home and the frame stay as they are.
// MiniMax H (11-12): no new form, no lower score (339); the frame is still 0xa9c against retail's 0xaa8, and the file's flags still keep retail's inlining of 0x5a680 and its cmovne out of reach.
// MiniMax S (11-11): no new form, no lower score (339); retail holds the session pointer in ecx and the state
// word in edx, ours holds the pointer in ebx and the state in ecx, and the frames differ by twelve bytes
// (0xa9c against 0xaa8), so every local home after the prologue differs.
// MiniMax A (11-10): no new form, no lower score (339): retail's conditional move for
// output->state needs the SSE architecture flag that this file's flags do not set, and the frame is
// 0xa9c against retail's 0xaa8 with the type, membership and member-index homes still different.
// MiniMax C (11-11): no new form, no lower score (339): retail's cmovne needs /arch:SSE and its three
// extra frame homes follow from 0x5a680 being inlined into this /Ob1 file; both are outside this lane's
// files, so the frame (0xa9c against 0xaa8) and the type, membership and member-index homes stay as
// they are.
// MiniMax B (11-13): no new form, no lower score (339): the frame is still 0xa9c against retail's 0xaa8 and the
// saved-register set differs (retail holds the connection in ecx and the state in edx where ours keeps them in ebx
// and ecx), and retail's conditional move for output->state needs the SSE architecture flag this file's flags do
// not set.
// score 339 (was 339): Frame 0xa9c versus 0xaa8; type/membership/member-index homes and saved registers remain.
// MiniMax R (11-10): no new form, no lower score (339): retail writes output->state with a conditional move, which needs /arch:SSE that this file's flags do not set, and the frame is 0xa9c against retail's 0xaa8 with the type, membership and member-index homes still different.
// MiniMax D (11-10): no new form, no lower score (339).
// Walk vs retail 0x64d70-0x65278 (b3G, 11-06, statement by statement): no behaviour difference found. Checked: state/value18/live
// range guards; get_values_4d08 outputs (map, map_variant, map_data) and the pre-call name pointer; memset 0x714; header
// words; type/local/mode; wcsncpy(.., 0x1f) + name[31]; id/key behind `state && flag24` (retail's per-destination null
// tests are the inlined getter's, equal in effect); identity from member_index; full-session vs open-slot branch and the
// mode 0/1/2 stores (0x98/0x94, 0x9a/0x96, 0x9a); variant 2/1; state; game_type/map/option/map_variant/teams;
// statistics gate (+0x1120, state 2, state 3); player loop (mask, stride 0x13c, sort fields, i != NONE && i >= 0 &&
// i < high_water, +0xc0 team, 0xff); sort; copy-out strides; team scores/mask. No float code.
// New evidence on the cmovne (b3G, fast): `// @flags /O2 /Ob1 /arch:SSE /Gr` on this file leaves the other 58 functions
// in it MATCH (fast) and turns `short state = 0; if (g_527330.initialized) state = (short)g_527330.state;` into
// retail's `xor esi,esi / test al,al / cmovne esi, [0x527334]` (the `?:` form still compiles to neg/sbb/and). 64d70
// then scores 353 (positional: the prologue and membership homes remain), so it is not kept here; the flag line is
// outside this function's edit scope, so it is for the coordinator to full-check.
// MiniMax I (11-05, run 37): no new form, no lower score (339); retail's cmovne needs /arch:SSE, which this file's flags do not set, and retail's inlined 0x5a680 is out of line here.
// MiniMax R (11-01): no new form; the retail prologue shape (global pointer then field at 0x741c,
// then the 2/8 range tests) is not reachable from this body without the inlined membership helper.
// Earlier (round 31, lane D; 339, was 339): unchanged this round; the permuter run was stopped by the background time
// limit before it produced a result. Still differs in retail's conditional move (cmovne at +0x23c, which
// needs /arch:SSE), in the frame (0xa9c against retail's 0xaa8) and in the type, membership and
// member-index homes and the saved registers.
// Earlier (339, was 339): unchanged. Retail writes output->state with a conditional move (cmovne at +0x23c), which needs
// /arch:SSE; this file has no such flag and the build flags are out of this lane's reach. The frame is
// 0xa9c against retail's 0xaa8 and the type, membership and member-index homes and the saved registers differ.
// Round 31 (lane D): unchanged this round; the permuter run was stopped by
// the background time limit before it produced a result. Still differs in retail's conditional move
// (cmovne at +0x23c, which needs /arch:SSE), in the frame (0xa9c against retail's 0xaa8) and in
// the type, membership and member-index homes and the saved registers.
// Earlier: unchanged. Retail writes output->state with a conditional move (cmovne at
// +0x23c), which needs /arch:SSE; this file has no such flag and the build flags are out of this
// lane's reach. The frame is 0xa9c against retail's 0xaa8 and the type, membership and
// member-index homes and the saved registers differ.
// Round 102 (lane 7, fast): the member index read into a local after the state tests 353, at the block top
// 352; the type through a pointer to value18 339; both 353; the member index from 0x5a680 with a NULL
// current-member output 353 (the reduced build inlines it: frame 0xaa0, index stored early). Retail's
// three extra homes match 0x5a680 inlined with (NULL, &index) and its membership result kept; round 5
// found the full build calls it from this /Ob1 file and a local inline copy would be a twin, so the function
// stays BLOCKED-BY 0x5a680 as well as by the cmovne (/arch:SSE) noted below.
// Earlier: Frame 0xa9c versus 0xaa8; type/membership/member-index homes and saved registers differ.
// Round 216: complete state-field observations 378; with native membership/player/selected-member
// references 378/378/377; membership-player/membership-member/player-member 378/377/377;
// all three 377. Frame remains 0xa9c. Original complete body and external type declarations restored.
// Round 434: scalar type snapshot 353; with existing variant getter/statistics producer 371/381.
// Scalar type with member-index capture 393; with existing membership provider 353; provider/index
// 393; provider/index with statistics/getter 380/369. Frames 0xaa0/0xaa4 still miss 0xaa8; all restored.
// Round 46: Complete admission predicate returning bool/byte/short/long 505/576/576/576; nested state/type then range-result producers 380/442/442/441. Frames 0xa9c/0xaa0/0xaa4; original body and cursor restored.
// Round 35: retail sets output->state with a conditional move (cmovne at +0x23c), so this body needs
// /arch:SSE, which this file lacks (lead only; no flag change). The listing is positional and ours is about
// six instructions short in the prologue, so retail-shaped local fixes raise the count until the three homes
// exist: id/key through the file's session_get_id made inline 390 (stays a call), __forceinline 393 (1323
// bytes; retail's per-destination null tests); full-session arm as `long used` plus a switch on the mode
// with `case 1: case 2:` 355 (1291 bytes; retail's movsx and single player_count load); both 380; the file's
// live-state getters for variant/mode/option 390 (1259 bytes).
// Round 5 (lane 18): the 12-byte frame gap is three stack values ours lacks: &session->value18 (read
// back for the type), the membership at +0x4c and the member index read before the flags are cleared;
// name, identity, counts, mask and players all go through the membership pointer, as an inlined
// membership query would give (retail's other /O2 callers 0x71480 and 0x83220 call 0x5a680). The id/key
// copy is null-checked per destination, as network_session_get_key's body is (not yet tried).
// Membership from 0x5a680 with both outputs 353 (called: this /Ob1 file keeps it out of line); with an
// inline/forceinline declaration here 353/353 (still called); `inline` on 0x5a680's definition 353 (still
// called here; 0x83220 and the 0x199ebc family lose); member index and the +0x4c pointer written as
// 0x66050 does 353 (frame 0xaa0: the pointer folds into the session register). All restored.
// Round 35 (m3 lane 2, fast = plain here): the id/key copy through the existing network_session_get_key
// (session, &output->id, output->key, NULL, NULL) 393 (1323 bytes; LTCG inlines it in the plain build too and
// gives retail's per-destination null tests, but the six-instruction prologue gap makes it positional); the
// manager state as `long state = 0; if (initialized) state = g_527330.state;` 375; the variant through the
// existing session_get_data_4db0 357 (1291 bytes, retail's length), with the option through
// session_get_value_5dd0 too 373 (1275 bytes), with the key getter instead 375 (1307 bytes). Diagnostic only
// (changes behaviour, not a valid form): get_values_4d08's third output initialised to `(byte *)&session->value18`
// and the type read through it, which forces a &value18 pointer into the frame as retail has one, 373 (frame
// 0xaa0, EDI pushed early); the membership from 0x5a680 (NULL, &member_index; inlined here, the index stored early
// at retail's +0x2c) with the key getter 396 (frame 0xaa0), with that diagnostic too 384 (1339 bytes, frame
// 0xaa0). Retail keeps &session->value18 in its own slot across the get_values call (the outputs go to other
// slots) and the membership pointer (+0x4c) in another; ours folds both into the session register.
// Round 111 (lane 7, fast): map_variant declared before map 339 (same code); the maximum, the option and the
// mode each as one ternary 375/375/375; the state as `short state = 0; if (initialized) state = ...;` 375 (all
// at 1307 bytes). Still BLOCKED by the cmovne (/arch:SSE) and by retail's inlined 0x5a680 (membership at +0x4c
// and member index kept in the frame; name, identity and counts read through the membership).
// Round 284: all three map-query output identities, six lexical priorities 360 each;
// two complete output declaration scopes 360 each, with the sorted cursor stepping in its body.
// Original body restored; frame and saved metadata homes remain.
// Round 77 (m3 lane 16, fast): the variant as an if/else 339 (same code); the four nested state guards as one
// `&&` test 339 (same code); the variant pointer as one conditional expression 375; `statistics` declared at its
// test 372; `return true` from the block with `return false` at the end 396 (1320 bytes). Still needs the cmovne.
// Round 66 (m3 lane 1, fast): name pointer read after get_values_4d08 356 (1291 bytes); memset before
// get_values_4d08 339 (same code); `statistics` declared at its test 372; team_mask beside the player count 375;
// get_values, memset, then the name pointer 356 (1291 bytes).
// Round 55 (m3 lane 2, fast; [n] = register-masked aligned count, 339 is [101]): diagnostics with a file-level view of the fields from
// +0x4c: type pointer, early member index, view pointer for the name, identity, counts, mask and players 353 [93] (frame 0xa9c: the
// pointers fold into the session register, the same code at every subset); with the existing key getter 396 [80] (1323 bytes, frame
// 0xaa0, only the member index gains a home); both pointers from inline getters 396 [80], as const references to their results 396
// [82], without the key getter 353 [95]. Retail's two pointers survive where every spelling folds; a form must first stop that.
// Round 107 (m3 lane 9, fast with the file's 59 functions, 339): the copy-out loop indexed (`&entries[j]` in the
// body, no comma increment) 356 (1312 B); the pointer step moved to the end of the body (`sorted_entry++;`) 360 (1314
// B), so the comma increment stays. Every other natural form I could name is listed. Levers left and why each is out:
// the cmovne needs /arch:SSE (a file-flag change); 0x5a680 inlined needs it inlined into this /Ob1 file (`inline` on
// its definition loses 0x83220 and the 0x199ebc family, a local inline copy would be a twin); retail's three extra
// frame homes follow from that inlining.
// MiniMax A (11-12): no new form, no lower score (339): retail holds the session pointer in ecx and the state word in edx
// where ours holds them in ebx and ecx, the frame is 0xa9c against retail's 0xaa8, and retail's cmovne for output->state
// needs /arch:SSE, which this file's flags do not set.
// @retail 0x64d70
bool __stdcall function_64d70(s_session_browser_summary *output)
{
 bool result = false;
 if (g_527330.initialized)
 {
  c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
  long state = session->state;
  if (state)
  {
   if (session->value18)
   {
    if (state > 2)
    {
     if (state <= 8)
     {
   long map = NONE;
   long map_variant = NONE;
   byte *map_data;
   bool statistics = false;
   dword team_mask = 0;
   byte *variant = 0;
   if (SESSION_STATE_IS_LIVE(session->state)) variant = session->data4db0;
   const wchar_t *name = session->members[session->value50].properties.description;
   session->get_values_4d08(&map, &map_variant, &map_data);
   memset(output, 0, sizeof(*output));
   output->version = 2;
   output->field_02 = 0;
   output->protocol = 4;
   output->build = 0x2651;
   output->compatible_build = 0x2651;
   output->type = (short)session->value18;
   output->local = (short)session->value14;
   long mode = 0;
   if (SESSION_STATE_IS_LIVE(session->state)) mode = session->value498c;
   output->mode = (short)mode;
   if (*name)
   {
    wcsncpy(output->name, name, 31);
    output->name[31] = 0;
   }
   if (session->state && session->flag24)
   {
    output->id = *(s_session_id *)&session->unknown1c;
    memcpy(output->key, session->data25, sizeof(output->key));
   }
   memcpy(output->identity, session->members[session->member_index].words, sizeof(output->identity));
   long maximum = 16;
   if (SESSION_STATE_IS_LIVE(session->state)) maximum = session->value4990;
   if (session->member_count >= maximum)
   {
    if (output->mode == 0) output->public_used = (short)session->player_count;
    else if (output->mode > 0 && output->mode <= 2) output->private_used = (short)session->player_count;
   }
   else
   {
    long used = session->player_count;
    long available = network_session_get_maximum_players(session) - used;
    switch (output->mode)
    {
    case 0: output->public_used = (short)used; output->public_open = (short)available; break;
    case 1: output->private_used = (short)used; output->private_open = (short)available; break;
    case 2: output->private_used = (short)used; break;
    }
   }
   output->variant = network_session_interface_get_value_49a4() == 2 ? 2 : 1;
   short state = (short)(g_527330.initialized ? g_527330.state : 0);
   output->state = state;
   output->game_type = *(long *)(variant + 0x44);
   output->map = map;
   short option = NONE;
   if (SESSION_STATE_IS_LIVE(session->state)) option = session->value5dd0;
   output->option = option;
   output->map_variant = map_variant;
   output->teams = (variant[0x48] & 1) != 0;
   if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 2 && state == 3)
   {
    statistics = true;
    function_158850(&output->score_type, &output->score_limit);
   }
   s_summary_player_sort_entry entries[16];
   long count = 0;
   for (long i = 0; i < 16; i++)
   {
    if (session->player_mask & (1 << i))
    {
     s_summary_player_sort_entry *entry = &entries[count];
     memcpy(entry->key, &session->players[i], sizeof(entry->key));
     memcpy(entry->properties, session->players[i].propertiesa8, sizeof(entry->properties));
     entry->sort.low = i;
     entry->sort.middle = 0;
     entry->sort.high = 0;
     if (statistics && i != NONE && i >= 0 && i < g_4e8c24->high_water_index)
     {
      byte *player = (byte *)g_4e8c24->data + i * g_4e8c24->size;
      if (*(word *)player && player[0xc0] != 0xff)
      {
       entry->sort.middle = function_1587b0(player_index_from_absolute_index(i));
       entry->sort.high = function_1587f0(*(signed char *)(player + 0xc0));
       team_mask |= 1 << player[0xc0];
      }
     }
     count++;
    }
   }
   function_13da70(entries, count, sizeof(entries[0]), (t_session_summary_compare)sort_key_less_than, 0);
   output->player_count = (short)count;
   s_summary_player_sort_entry *sorted_entry = entries;
   for (long j = 0; j < count; j++, sorted_entry++)
   {
    memcpy(output->player_keys[j], sorted_entry->key, sizeof(sorted_entry->key));
    memcpy(output->player_names[j], sorted_entry->properties, sizeof(output->player_names[j]));
    output->scores[j] = sorted_entry->sort.middle;
    output->player_teams[j] = *(signed char *)(sorted_entry->properties + 0x7c);
    memcpy(output->field_5e0[j], sorted_entry->properties + 0x40, sizeof(output->field_5e0[j]));
   }
   if ((variant[0x48] & 1) && statistics)
   {
    output->team_mask = 0;
    for (long team = 0; team < 8; team++)
    {
     long score = function_1587f0(team);
     output->team_scores[team] = score;
     if (score || (team_mask & (1 << team))) output->team_mask |= 1 << team;
    }
   }
   result = true;

     }
    }
   }
  }
 }
 return result;
}
