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
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number)
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
	return network_session_start_countdown(session, 0, false, 0, 0);
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
void machine_identifier_build(s_type_fb9815 *identifier, long index);

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

/* how far a local user has got into the session: 0 none, 1 the session is
   not live, 2/3 no player yet, 4 no slot, 5 player out of date, 6 up to date */
// @retail 0x646b0
long network_session_interface_get_user_state(c_class_58d20 *session, long user_index)
{
	if (g_4cd868.users[user_index].valid &&
		!(online_logon_connected() && g_4cd868.users[user_index].xuid.dwUserFlags == 0xbad00000))
	{
		if (SESSION_STATE_IS_LIVE(session->state))
		{
			long player_index = session->members[session->current_member].player_indices[user_index];
			if (player_index != NONE)
			{
				s_network_session_player *player = &session->players[player_index];
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

long g_4cf968;
long g_4cf96c;

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
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
		result = session_get_data_4db0(session);
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
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
		result = session_get_value_5dd0(session);
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
					return i;
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
	c_class_58d20 *session = network_session_get_live();
	if (session)
		result = session_get_data_4999(session);
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
	long status = 0;
	long lowest = 0;
	c_class_58d20 *session = 0;
	if (network_session_get(&session) && SESSION_STATE_IS_LIVE(session->state))
	{
		long member_count = session->member_count;
        long *local_0 = &session->members[0].unknown88;
		status = 4;
		lowest = 100;
		for (long i = 0; i < member_count; i++, local_0 = (long *)((byte *)local_0 + 0x10c))
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

static inline short session_get_value_5dd0_inline(c_class_58d20 *session)
{
	short result = NONE;
	if (SESSION_STATE_IS_LIVE(session->state))
		result = session->value5dd0;
	return result;
}

// @retail 0x63e00
bool network_session_interface_can_add_player(void)
{
	c_class_58d20 *session = network_session_get_live();
	if (!session)
		return false;
	if (session_get_value_498c(session) != 0)
		return false;
	if (session->player_count >= session_get_maximum_player_count(session))
		return false;
	if (g_527330.state == 6 || g_527330.state == 7 || g_527330.state == 8 || g_527330.state == 9)
		return false;
	if (session_get_value_5dd0_inline(session) != NONE)
		return false;
	return true;
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


// @retail 0x64310
bool network_session_interface_kick_player(long player_index)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_live();
	if (session)
	{
		long host_member = session->current_member;
		if (host_member == session->value50 && (session->player_mask & (1 << player_index)))
		{
			long member_index = session->players[player_index].member_index;
			if (member_index != host_member && network_session_delegate_leader(session, (const s_session_member_identity *)session->members[member_index].words))
				result = true;
		}
	}
	return result;
}

// @retail 0x643f0
bool network_session_interface_ban_player(long player_index)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_live();
	if (session)
	{
		long host_member = session->current_member;
		if (host_member == session->value50 && (session->player_mask & (1 << player_index)))
		{
			long member_index = session->players[player_index].member_index;
			if (member_index != host_member && network_session_boot_machine(session, (const s_session_member_identity *)session->members[member_index].words))
				result = true;
		}
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

PRIVATE __forceinline bool function_63f51(const char *arg_0, c_class_58d20 *arg_1, long arg_2, long arg_3)
{
 return network_session_parameters_set_value4d08(arg_1, arg_0, arg_2, arg_3);
}

// @retail 0x63f50
bool network_session_interface_set_value4d08(long value4d08, long value4d0c, const char *string)
{
	bool result = false;
	c_class_58d20 *session = network_session_get_current();
	if (session && session_is_established(session))
	{
		if (session_is_leader(session))
			{ result = function_63f51(string, session, value4d08, value4d0c); goto local_0; }
		result = true;
	}
local_0:
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
		if (network_session_parameters_set_value4d08(session, string, value4d08, value4d0c) && network_session_start_countdown(session, 0, false, 0, 0))
			return true;
		return false;
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
		if (network_session_parameters_set_value5dd0(session, value) && network_session_start_countdown(session, 0, false, 0, 0))
			return true;
		return false;
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
		if (session->state && session_is_established(session) && session->current_member == session->value50 && network_session_parameters_set_value49c4(session))
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
			if (network_session_start_countdown(session, countdown, start, mode, (const long *)&user->xuid))
				result = true;
		}
	}
	return result;
}

/* ---- the local users' players in a session (lane D, round 4) ---- */

bool network_session_player_add(c_class_58d20 *session, const byte *properties, const dword *identity, long slot, long unknown18, long unknownac);
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
		if (network_session_player_add(session, user->properties, (const dword *)&user->xuid, user_index, user->unknown10, user->unknowna4))
			user->unknowna8[owner] = session_interface_time_get();
		else
			user->unknowncc[owner] = true;
	}
	else
	{
		s_session_user_reservation *reservation = &((s_session_user_reservation *)session->data761c)[user_index];
		if (reservation->valid && !memcmp(&user->xuid, &reservation->xuid, sizeof(XUID)))
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

// @retail 0x64480
void network_session_get_member_latency(s_session_member_latency *latency, c_class_58d20 *session)
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
			if (i < 0 || i >= session->member_count)
				continue;
			s_network_session_member_state *member = &session->member_states[i];
			if (!member->flag1 || member->unknown04 == NONE)
				continue;
			s_network_observer_channel *channel = &observer->channels[member->unknown04];
			if (channel->state != 7 || !(channel->flags & 0x10))
				continue;
			s_qos_result qos = channel->field_x31a738;
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

static inline long network_session_time_matches(long a, long b)
{
	long result = 0;
	if (g_510520.a == a && g_510520.b == b)
		result = function_73b10(g_510540.a, g_510540.b);
	return result;
}

static inline long session_get_value_49f0(c_class_58d20 *session)
{
	long result = 0;
	if (SESSION_STATE_IS_LIVE(session->state) && session->flag49e8)
		result = network_session_time_matches(session->value49f0, session->value49f4);
	return result;
}

/* sends the local machine's properties to the session when they change */
// @retail 0x658c0
void network_session_interface_update_local_properties(c_class_58d20 *session, s_session_member *member)
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
		network_session_get_member_latency((s_session_member_latency *)properties.unknown74, session);
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
	byte *previous = g_4cd868.data400[owner];
	if (!memcmp(previous, parameters, sizeof(g_4cd868.data400[owner])) && g_4cd868.value3f4[owner] == session->value4c)
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
			const s_player_properties *requested = (const s_player_properties *)session->players[i].properties18;
			const s_player_properties *current = (const s_player_properties *)session->players[i].propertiesa8;
			s_player_properties properties;

			memset(&properties, 0, sizeof(properties));
			properties.unknown40[0] = 0;
			properties.unknown40[1] = 0;
			properties.unknown40[2] = 0;
			properties.unknown40[3] = 0;
			properties.unknown70[0] = 0;
			properties.unknown70[1] = 0;
			properties.unknown70[2] = 0;
			properties.team = 0;
			properties.unknown7d = 0;
			properties.unknown7e = 0xff;
			properties.unknown7f = 0xff;
			properties.unknown84 = NONE;
			properties.unknown88 = NONE;
			properties.unknown8a = NONE;
			properties.unknown8c = NONE;

			if (wcslen(requested->name) > 0 && wcslen(current->name) == 0)
			{
				function_07f660(properties.name, 32, requested->name, name_count, names);
			}
			else
			{
				wcsncpy(properties.name, current->name, 31);
				properties.name[31] = 0;
			}
			memcpy(properties.unknown40, requested->unknown40, sizeof(properties.unknown40));
			memcpy(properties.unknown70, requested->unknown70, sizeof(properties.unknown70));
			memcpy(properties.unknown50, requested->unknown50, sizeof(properties.unknown50));
			if (session->value14 == 1)
			{
				char team = requested->team;
				if (team == NONE)
					properties.team = team;
				else if (parameters[0x48] & 1)
				{
					if (team < 0)
						properties.team = 0;
					else if (team > 7)
						properties.team = 7;
					else
						properties.team = team;
				}
				else
					properties.team = (char)i;
				properties.unknown84 = requested->unknown84;
				properties.unknown8c = requested->unknown8c;
				properties.unknown8a = requested->unknown8a;
				properties.unknown88 = requested->unknown88;
			}
			else
			{
				properties.team = current->team;
				properties.unknown84 = current->unknown84;
				properties.unknown8c = current->unknown8c;
				properties.unknown8a = current->unknown8a;
				properties.unknown88 = current->unknown88;
			}
			properties.unknown7f = requested->unknown7f;
			properties.unknown7e = requested->unknown7e;
			properties.unknown7d = requested->unknown7d;
			properties.unknown81 = requested->unknown81;
			properties.unknown80 = requested->unknown80;
			if (memcmp(current, &properties, sizeof(properties)) != 0)
				network_session_host_set_player_properties(session, i, (const byte *)&properties);
		}
	}
	g_4cd868.value3f4[owner] = session->value4c;
	memcpy(previous, parameters, sizeof(g_4cd868.data400[owner]));
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
 long levels[16];
 memset(levels, NONE, sizeof(levels));
 long maximum = NONE;
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
 s_session_interface_user *user = &g_4cd868.users[user_index];
 user->properties[0x7f] = (byte)maximum;
 if (g_4cd868.unknown98 >= 0 && g_4cd868.unknown98 < 16)
  user->properties[0x7e] = (byte)levels[g_4cd868.unknown98];
 else
  user->properties[0x7e] = 0xff;
 c_class_58d20 *session = network_session_get_live();
 long variant = NONE;
 if (session && SESSION_STATE_IS_LIVE(session->state))
  variant = session->value49c8;
 long value, level;
 if (variant != NONE && function_73a10(variant, user_index, 0, &value) &&
  function_73a10(variant, user_index, 1, &level))
 {
  *(long *)&user->properties[0x84] = variant;
  *(long *)&user->properties[0x8c] = value;
  *(short *)&user->properties[0x8a] = (short)level;
 }
 else
 {
  *(long *)&user->properties[0x84] = NONE;
  *(long *)&user->properties[0x8c] = NONE;
  *(short *)&user->properties[0x8a] = NONE;
 }
 *(word *)&user->properties[0x88] = 0xffff;
 if (!(user->xuid.dwUserFlags & 3) && user->xuid.dwUserFlags != 0xbad00000)
  function_80490((const s_cached_player_identity *)&user->xuid,
   (const s_cached_player_source *)user->properties, true, false);
}

bool function_138800(void);
long map_location_progress_get(char const *map_name);
real __stdcall function_122dd0(byte *map_name, long mode, long type);
bool __stdcall function_163890(char const *map_name, long mode);

// @retail 0x65340
void function_065340(void)
{
	long status = 0;
	real progress = 0.0f;
	long variant_index = NONE;
	c_class_58d20 *session = 0;
	bool found = false;
	long state = g_527330.initialized ? g_527330.state : 0;
	switch (state)
	{
	case 1:
	case 2:
	case 3:
	case 4:
		if (g_527330.initialized)
		{
			c_class_58d20 *current = (c_class_58d20 *)g_527330.session_a;
			if (current->state)
			{
				session = current;
				found = true;
			}
		}
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		if (g_527330.initialized)
		{
			c_class_58d20 *current = (c_class_58d20 *)g_527330.session_b;
			if (current->state)
			{
				session = current;
				found = true;
			}
		}
		break;
	default:
		break;
	}
	if (found && session && SESSION_STATE_IS_LIVE(session->state))
	{
		long first_id;
		long second_id;
		byte *name;
		if (session->get_values_4d08(&first_id, &second_id, &name) &&
			second_id != NONE && name[0])
		{
			if (function_138800() && g_4e6948->position_a == first_id &&
				g_4e6948->position_b == second_id &&
				!strncmp((char const *)name, g_4e6948->name, 0x104))
			{
				status = 4;
				progress = 1.0f;
			}
			else
			{
				long location = map_location_progress_get((char const *)name);
				progress = function_122dd0(name, 0, first_id == NONE);
				switch (location)
				{
				case 1: status = 2; break;
				case 2: status = 3; break;
				case 3: status = 1; break;
				default:
					function_163890((char const *)name, 1);
					break;
				}
			}
		}
		if (SESSION_STATE_IS_LIVE(session->state))
			variant_index = session->value49c8;
	}
	g_4cd868.unknown90 = status;
	g_4cd868.unknown94 = (long)(progress * 100.0f);
	g_4cd868.unknown98 = variant_index;
	s_session_interface_user *user = g_4cd868.users;
	for (long i = 0; i < 4; i++, user++)
		if (user->valid)
			function_65530(i);
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

#pragma optimize("s", on)
#pragma inline_depth(0)
// @retail 0x66050
long function_66050(c_class_58d20 *session, long variant_index)
{
 volatile long result = variant_index;
 long local_1 = result;
 bool missing = false;
 bool compatible;
 long current = session->member_index;
 s_66050 *local_0 = (s_66050 *)&session->value4c;
 compatible = function_7ed20((const s_message_identities *)local_0, 0, &missing, &compatible);
 s_surface_description *variant = function_192e60(local_1);
 result = 0;
 if (!variant)
 {
  result = 2;
  return result;
 }
 if (variant->type == 5 || variant->type == 2 || variant->type == 4)
 {
  for (long i = 0; i < 16; i++)
   if ((local_0->field_10d0 & (1 << i)) && (local_0->field_10d4[i].user_flags & 3))
   {
    result = 12;
    return result;
   }
 }
 for (long i = 0; i < 16; i++)
  if ((local_0->field_10d0 & (1 << i)) && local_0->field_10d4[i].user_flags == 0xbad00000)
  {
   result = 13;
   return result;
  }
 for (long member = 0; member < local_0->field_8; member++)
 {
  long state = *(long *)((byte *)&local_0->field_c[member] + 0xac + local_1 * 4);
  if (state != 1)
  {
   if (!state)
    result = 14;
   else
    result = state == 3 ? 2 : 11;
   return result;
  }
 }
 for (long i = 0; i < 16; i++)
  if ((local_0->field_10d0 & (1 << i)) && (signed char)local_0->field_10d4[i].propertiesa8[0x81] < variant->field5e4)
  {
   result = 15;
   return result;
  }
 if (variant->type == 5 && !compatible)
 {
  result = missing ? 8 : 9;
  return result;
 }
 long minimum;
 if (variant->type <= 3)
  minimum = 1;
 else if (variant->type == 4)
  minimum = variant->field5fc;
 else
  minimum = variant->field5f0;
 long count = local_0->field_10cc;
 if (count < minimum)
 {
  result = 6;
  return result;
 }
 if (count > function_193250(variant))
 {
  result = 7;
  return result;
 }
 if (variant->type == 5)
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

// @retail 0x64d70
bool __stdcall function_64d70(s_session_browser_summary *output)
{
 bool result = false;
 if (g_527330.initialized)
 {
  c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
  if (session->state && session->value18 && SESSION_STATE_IS_LIVE(session->state))
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
   for (long j = 0; j < count; j++)
   {
    s_summary_player_sort_entry *entry = &entries[j];
    memcpy(output->player_keys[j], entry->key, sizeof(entry->key));
    memcpy(output->player_names[j], entry->properties, sizeof(output->player_names[j]));
    output->scores[j] = entry->sort.middle;
    output->player_teams[j] = *(signed char *)(entry->properties + 0x7c);
    memcpy(output->field_5e0[j], entry->properties + 0x40, sizeof(output->field_5e0[j]));
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
 return result;
}
