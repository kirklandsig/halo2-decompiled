// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_190001.CPP: the four local controllers (g_54e8e0, 0xc70 bytes
   each): their sign-in state, profiles, online users, presence and the menu
   input they generate (lane H) */

#include "unknown_120d80.h"
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "unknown_1248b0.h"
#include "online_presence.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_24b5bc.h"

#define MAXIMUM_CONTROLLERS 4

/* the player profile a controller holds (0x1e0 bytes; unknown_18f576.cpp
   views it as raw dwords) */
struct s_player_profile
{
	dword flags;
	dword flag0 : 1;
	dword flag1 : 1;
	word name[32];
	char short_name[16];
	byte unknown058[0x5c - 0x58];
	long value5c;
	byte unknown060[0xe4 - 0x60];
	long valuee4;
	byte unknown0e8[0xec - 0xe8];
	long keys[4];
	byte unknown0fc[0x118 - 0xfc];
	dword value118[4];
	byte unknown128[0x150 - 0x128];
	bool appear_offline;
	byte unknown151[0x1e0 - 0x151];
};

/* the controller settings at +0x114 */
struct s_controller_settings
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	byte invert;
	byte look_mode;
	byte sensitivity;
	byte unknown007;
};

/* the identity at +0x4e0 (0x6a2 bytes; set when its xuid is not zero) */
#pragma pack(push, 2)
struct s_controller_identity
{
	XUID xuid;
	byte data[0x20];
	byte unknown2c[0x6a2 - 0x2c];
};
#pragma pack(pop)

struct s_controller
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword signed_in : 1;
	dword active : 1;
	dword flag6 : 1;
	long session_user;
	dword unknown008[3];
	long unknown014;
	s_player_profile profile;
	long profile_index;
	long unknown1fc;
	long unknown200;
	long unknown204;
	byte unknown208[0x46c - 0x208];
	bool unknown46c;
	bool unknown46d;
	byte unknown46e[2];
	XONLINE_USER user;
	s_controller_identity identity;
	byte unknownb82[0xc14 - 0xb82];
	long task_c14;
	long task_c18;
	word name[32];
	dword presence;
	dword notification_flags;
	bool has_session;
	bool notification_dirty;
	byte session_id[8];
	byte unknownc6e[2];
};

inline s_controller *controller_get(long index)
{
	return (s_controller *)&g_54e8e0[index];
}

inline long controller_next(long index)
{
	long next = NONE;

	if (index >= 0 && index < MAXIMUM_CONTROLLERS - 1)
	{
		next = index + 1;
	}
	return next;
}

/* the id of the dialog that asks the controller's player to reconnect */
__forceinline long controller_dialog_id(long index)
{
	long id;

	switch (index)
	{
	case 0:
		id = 0x1c;
		break;
	case 1:
		id = 0x1d;
		break;
	case 2:
		id = 0x1e;
		break;
	default:
		id = 0x1f;
		break;
	}
	return id;
}

/* the user flags of an XUID as bits */
struct s_online_user_flags
{
	dword guest_number : 2;
	dword unknown2 : 14;
	dword voice_not_allowed : 1;
};

inline XUID const *online_user_get_xuid(XONLINE_USER const *user)
{
	XUID const *xuid = NULL;

	if (user)
	{
		xuid = &user->xuid;
	}
	return xuid;
}

inline char const *online_user_get_gamertag(XONLINE_USER const *user)
{
	char const *gamertag = "";

	if (user)
	{
		gamertag = user->szGamertag;
	}
	return gamertag;
}

/* the menu event the controllers send (147dbe) */
struct s_controller_event
{
	long type;
	long controller;
	long value;
	short amount;
};

struct s_controller_input_globals
{
	short last_direction[MAXIMUM_CONTROLLERS][2];
	dword last_time[MAXIMUM_CONTROLLERS];
	bool active[MAXIMUM_CONTROLLERS];
};

s_controller_input_globals g_55e758;
bool g_551ae0[MAXIMUM_CONTROLLERS];

long function_190262(long value);
void function_6b640(long task_index);
void network_session_interface_clear_user(long index);
void online_get_logon_users(XONLINE_USER *users);
bool function_6c7e0();
void online_set_notification_state(const XNKID *session_id, DWORD user_index, BYTE *state_data, DWORD state_flags);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
void unicode_string_snprintf(word *buffer, long maximum_count, const word *format, ...);

/* the record the session keeps of each local player (unknown_153750.cpp) */
struct s_profile_record
{
	word name[0x20];
	dword value40[4];
	byte identity_data[0x20];
	XUID identity_xuid;
	char team;
	char value7d;
	char value7e;
	char value7f;
	char value80;
	char value81;
	byte unknown82[2];
	long value84;
	short value88;
	short value8a;
	long value8c;
};

void function_1537f0(s_profile_record *record);
bool network_session_interface_get_user_properties(long index, long *unknown10, byte *properties, long *unknowna4);
void network_session_interface_set_user_properties(long index, long unknown10, const byte *properties, long unknowna4);
void network_session_interface_set_user_xuid(long index, const XUID *xuid);
dword voice_get_player_flags(long unknown14);
void function_23620d(long string_handle, word *buffer);
long __stdcall function_64610(dword *xuid);

/* the controller's xuid and its session record */
// @retail 0x18fd94
void function_18fd94(long index, dword *xuid, s_profile_record *record)
{
	s_controller *controller = controller_get(index);
	bool valid = controller->session_user >= 0 && controller->session_user < MAXIMUM_CONTROLLERS;

	if (xuid)
	{
		memcpy(xuid, controller->unknown008, sizeof(controller->unknown008));
	}
	if (record)
	{
		long unknown10;
		long unknowna4;
		long team;

		function_1537f0(record);
		if (!valid || !network_session_interface_get_user_properties(controller->session_user, &unknown10, (byte *)record, &unknowna4))
		{
			function_1537f0(record);
		}
		unicode_string_copy(record->name, controller->name, 0x20);
		memcpy(record->value40, controller->profile.value118, sizeof(record->value40));
		if (controller->identity.xuid.qwUserID != 0)
		{
			record->identity_xuid = controller->identity.xuid;
			memcpy(record->identity_data, controller->identity.data, sizeof(record->identity_data));
		}
		else
		{
			memset(&record->identity_xuid, 0, sizeof(record->identity_xuid));
			memset(record->identity_data, 0, sizeof(record->identity_data));
		}
		record->value7d = (char)controller->unknown200;
		record->value80 = controller->unknown46c;
		record->value81 = (char)controller->unknown204;
		team = controller->unknown1fc;
		if (team == NONE)
		{
			record->team = (char)team;
		}
		else
		{
			if (team < 0)
				team = 0;
			else if (team > 16)
				team = 16;
			record->team = (char)team;
		}
	}
}

/* sends the controller's record to the session */
// @retail 0x18fe9e
void function_18fe9e(long index)
{
	s_controller *controller = controller_get(index);

	if (controller->session_user != NONE)
	{
		s_profile_record record;
		dword xuid[3];

		function_18fd94(index, xuid, &record);
		network_session_interface_set_user_properties(controller->session_user, index, (byte *)&record, voice_get_player_flags(index));
	}
}

/* unknown_18f576.cpp */
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);

/* not decompiled yet (src/stubs/lane_e.cpp); screen_widgets.h's
   s_player_profile_settings is the same 0x1e0 profile */
struct s_player_profile_settings;
void __stdcall function_2153dd(long player, long profile_index, s_player_profile_settings *settings, long flags);

/* not decompiled yet (src/stubs/lane_h.cpp) */
void __stdcall function_18fcc4(long controller, s_player_profile *profile, long profile_index);
void function_147dbe(s_controller_event *event);
bool __stdcall function_148f36(long controller);
bool function_1a0660(long profile_index, s_player_profile *profile);
long function_abc70(long index, XUID const *xuid);

/* unknown_2172a0.cpp */
void function_2172a0(long handle);

/* forgets the profile the controller holds */
// @retail 0x18fc08
void function_18fc08(long index)
{
	s_controller *controller = controller_get(index);

	if (controller->signed_in)
	{
		if (controller->profile_index != NONE)
			function_2172a0(controller->profile_index);
		memset(&controller->profile, 0, sizeof(controller->profile));
		controller->signed_in = false;
		controller->profile_index = NONE;
	}
}

/* the controller's name: its online user's gamertag (numbered for a
   guest), else its profile's name */
// @retail 0x190eb3
void function_190eb3(long index)
{
	s_controller *controller = controller_get(index);

	if (TEST_FIELD_BIT(controller->active))
	{
		XONLINE_USER *user = &controller->user;
		XUID const *xuid = online_user_get_xuid(user);

		if (xuid->dwUserFlags & 3)
		{
			long guest_number = (char)online_user_get_xuid(user)->dwUserFlags & 3;
			word format[0x100];

			format[0] = 0;
			function_23620d(0x2d000231, format);
			unicode_string_snprintf(controller->name, 0x20, format, (short)guest_number, online_user_get_gamertag(user));
		}
		else
		{
			unicode_string_snprintf(controller->name, 0x20, (const word *)L"%hs", online_user_get_gamertag(user));
		}
	}
	else if (TEST_FIELD_BIT(controller->signed_in))
	{
		unicode_string_copy(controller->name, controller->profile.name, 0x20);
	}
	else
	{
		controller->name[0] = 0;
	}
	function_18fe9e(index);
}

// @retail 0x190074
void function_190074(long index, bool active)
{
	s_controller *controller = controller_get(index);

	if (active)
		controller->active = true;
	else
		controller->active = false;
	if (!active)
		function_190eb3(index);
}

/* signs the online user in on the controller */
// @retail 0x18fee9
void function_18fee9(XONLINE_USER const *user, long index)
{
	s_controller *controller = controller_get(index);

	controller->user = *user;
	function_190eb3(index);
	if (controller->session_user == NONE)
	{
		controller->session_user = function_64610(controller->unknown008);
	}
	memcpy(controller->unknown008, online_user_get_xuid(&controller->user), sizeof(controller->unknown008));
	network_session_interface_set_user_xuid(controller->session_user, (XUID const *)controller->unknown008);
}

// @retail 0x190001
void function_190001(long index, char const *name)
{
	if (TEST_FIELD_BIT(controller_get(index)->signed_in) && !TEST_FIELD_BIT(controller_get(index)->profile.flag0))
	{
		s_player_profile profile;
		long profile_index;

		player_slot_get_profile(index, &profile, &profile_index);
		if (profile_index != NONE)
		{
			strncpy(profile.short_name, name, 16);
			profile.short_name[15] = 0;
			function_18fcc4(index, &profile, profile_index);
		}
	}
}

/* the controller's online user is a guest */
// @retail 0x1900a5
bool function_1900a5(long index)
{
	XUID const *xuid = online_user_get_xuid(&controller_get(index)->user);
	bool guest = XOnlineIsUserGuest(xuid->dwUserFlags);

	return guest;
}

// @retail 0x1900be
short function_1900be(void)
{
	long count = 0;
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->active) && !function_1900a5(index))
		{
			count++;
		}
	}
	return (short)count;
}

// @retail 0x1900ff
short function_1900ff(long controller)
{
	short count = 0;

	if (!function_1900a5(controller))
	{
		long index;

		for (index = 0; index != NONE; index = controller_next(index))
		{
			if (index != controller)
			{
				s_controller *other = controller_get(index);

				if (TEST_FIELD_BIT(other->active))
				{
					XUID const *a = online_user_get_xuid(&controller_get(controller)->user);
					XUID const *b = online_user_get_xuid(&other->user);

					if (a && b && a->qwUserID == b->qwUserID)
					{
						count++;
					}
				}
			}
		}
	}
	return count;
}

void function_190d4b(long controller);

// @retail 0x190186
void function_190186(long controller)
{
	if (!function_1900a5(controller))
	{
		long index;

		for (index = 0; index != NONE; index = function_190262(index))
		{
			if (index != controller && TEST_FIELD_BIT(controller_get(index)->active))
			{
				XUID const *a = online_user_get_xuid(&controller_get(controller)->user);
				XUID const *b = online_user_get_xuid(&controller_get(index)->user);

				if (a && b && a->qwUserID == b->qwUserID)
				{
					function_190d4b(index);
				}
			}
		}
	}
}

// @retail 0x1901fc
word function_1901fc(void)
{
	dword mask = 0;
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->signed_in))
		{
			mask |= 1 << index;
		}
	}
	return (word)mask;
}

// @retail 0x19022f
word function_19022f(void)
{
	dword mask = 0;
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->active))
		{
			mask |= 1 << index;
		}
	}
	return (word)mask;
}

// @retail 0x19028d
bool function_19028d(void)
{
	bool result = false;
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->active))
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x1902c1
word *function_1902c1(long index)
{
	word *result = NULL;

	if (TEST_FIELD_BIT(controller_get(index)->signed_in))
	{
		result = controller_get(index)->name;
	}
	return result;
}

// @retail 0x1902de
long function_1902de(long index)
{
	long result = NONE;

	if (TEST_FIELD_BIT(controller_get(index)->signed_in))
	{
		result = controller_get(index)->profile.valuee4;
	}
	return result;
}

// @retail 0x1902fc
void function_1902fc(long index, long value)
{
	if (TEST_FIELD_BIT(controller_get(index)->signed_in))
	{
		s_player_profile profile;
		long profile_index;

		player_slot_get_profile(index, &profile, &profile_index);
		profile.valuee4 = value;
		function_18fcc4(index, &profile, profile_index);
	}
}

// @retail 0x19034d
void function_19034d(long index, long state, long unknown08, long unknown04, short minutes_a, short minutes_b)
{
	s_controller *controller = controller_get(index);

	if (TEST_FIELD_BIT(controller->active))
	{
		s_online_presence_source source;
		s_online_presence presence;
		s_online_presence *built = &presence;

		memset(&source, 0, sizeof(source));
		source.state = state;
		source.minutes_a = minutes_a;
		source.minutes_b = minutes_b;
		source.unknown08 = unknown08;
		source.unknown04 = unknown04;
		online_presence_build(built, &source);
		if (controller->presence != *(dword *)built)
		{
			controller->presence = *(dword *)&presence;
			controller->notification_dirty = true;
		}
	}
}

// @retail 0x1903c2
void function_1903c2(long index, long value)
{
	if (value != NONE)
	{
		s_player_profile profile;
		long profile_index;

		player_slot_get_profile(index, &profile, &profile_index);
		if (profile_index != NONE && profile.value5c != value)
		{
			profile.value5c = value;
			function_18fcc4(index, &profile, profile_index);
		}
	}
}

// @retail 0x19040d
void function_19040d(long value)
{
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		function_1903c2(index, value);
	}
	global_preferences_globals.current.unknown20 = value;
	global_preferences_globals.dirty = true;
}

/* raises the profile's key of the given kind to the value */
// @retail 0x19043f
void function_19043f(long index, long kind, long value)
{
	if (value != NONE)
	{
		s_player_profile profile;
		long profile_index;

		player_slot_get_profile(index, &profile, &profile_index);
		if (profile_index != NONE && profile.keys[kind] < value)
		{
			profile.keys[kind] = value;
			function_18fcc4(index, &profile, profile_index);
		}
	}
}

// @retail 0x19048c
void function_19048c(long index)
{
	s_player_profile profile;
	long profile_index;

	player_slot_get_profile(index, &profile, &profile_index);
	if (profile_index != NONE)
	{
		profile.flag1 = true;
		function_18fcc4(index, &profile, profile_index);
	}
}

// @retail 0x1904cb
bool function_1904cb(long index)
{
	s_player_profile profile;
	long profile_index;
	bool result = false;

	player_slot_get_profile(index, &profile, &profile_index);
	if (profile_index != NONE)
	{
		result = TEST_FIELD_BIT(profile.flag1);
	}
	return result;
}

// @retail 0x1904ff
long function_1904ff(long index)
{
	s_player_profile profile;
	long profile_index;
	long result = 1;

	player_slot_get_profile(index, &profile, &profile_index);
	if (profile_index != NONE)
	{
		result = profile.value5c;
	}
	return result;
}

/* the profile's keys (unknown_1a06a0.cpp) */
struct s_key_set
{
	byte unknown00[0xec];
	long keys[4];
};

void function_1a06f0(s_key_set *set, long *best_key, long *best_index);

/* the best key of the controller's profile */
// @retail 0x19052c
void function_19052c(long index, long *best_key, long *best_index)
{
	s_player_profile profile;
	long profile_index;

	player_slot_get_profile(index, &profile, &profile_index);
	if (profile_index != NONE)
	{
		function_1a06f0((s_key_set *)&profile, best_key, best_index);
	}
	else
	{
		*best_key = NONE;
		*best_index = 1;
	}
}

/* the best key of the signed in controllers' profiles */
// @retail 0x190565
long function_190565(void)
{
	long best = NONE;
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->signed_in))
		{
			long key;
			long key_index;

			function_19052c(index, &key, &key_index);
			if (key > best)
			{
				best = key;
			}
		}
	}
	return best;
}

bool g_54d5a0;

bool function_8d7c0(void);
c_class_1473c9 *function_149ef3(word user_flags, long load);
c_class_1473c9 *__stdcall function_18f42d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_18f474(s_screen_parameters *parameters);

// @retail 0x1905bf
void function_1905bf(long controller, bool flag)
{
	if (g_54d5a0 && !function_8d7c0())
	{
		dialog_ok_show(1, 0x33, 4, function_1901fc(), 0, 0);
	}
	else
	{
		function_149ef3(1 << controller, (long)(flag ? function_18f474 : function_18f42d));
	}
}

short player_slot_count_active(void);
bool function_1a0540(s_player_profile_settings *settings, long file_index);
void function_120df0(long index, wchar_t const *name);
c_class_1473c9 *__stdcall function_24b4a9(s_screen_parameters *parameters);

/* signs the controller in with a saved profile; when online, or when no
   player slot is active yet, opens the gamertag selection screen instead */
// @retail 0x19060a
void function_19060a(long profile_index, long controller)
{
	bool select;
	s_player_profile_settings settings;
	s_player_slot_profile *slot;

	if (function_6c7e0() || (function_8d7c0() && !player_slot_count_active()))
		select = true;
	else
		select = false;
	slot = player_slot_profile_get(controller);
	function_1a0540(&settings, profile_index);
	slot->initialize(controller);
	slot->set_profile_index(profile_index);
	function_120df0(controller, (wchar_t const *)settings.name);
	if (select)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 6, 0, 1 << controller, 3, 4, (long)function_24b4a9);
		parameters.load(&parameters);
	}
	else
	{
		slot->sign_in(NULL);
	}
}

__forceinline bool logon_user_voice_allowed(long index)
{
	XONLINE_USER users[XONLINE_MAX_LOGON_USERS];
	XONLINE_USER *user;

	online_get_logon_users(users);
	user = &users[index];
	if (user && (user->xuid.qwUserID != 0) && !XOnlineIsUserGuest(user->xuid.dwUserFlags) && !TEST_FIELD_BIT(((s_online_user_flags *)&user->xuid.dwUserFlags)->voice_not_allowed))
	{
		return true;
	}
	return false;
}

void function_6cb60(void);
void function_199e2e(bool close);

/* signs every controller out */
// @retail 0x1906b4
void function_1906b4(void)
{
	long index;

	function_6cb60();
	for (index = 0; index != NONE; index = function_190262(index))
	{
		function_190d4b(index);
	}
	function_199e2e(false);
}

// @retail 0x1906da
bool function_1906da(long index)
{
	bool function_1999d7(void);

	if (function_1999d7())
	{
		return true;
	}
	else if (logon_user_voice_allowed(index))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// @retail 0x190728
void function_190728(long index)
{
	s_controller *controller = controller_get(index);

	if (controller->task_c14 != NONE)
	{
		function_6b640(controller->task_c14);
		controller->task_c14 = NONE;
	}
	if (controller->task_c18 != NONE)
	{
		function_6b640(controller->task_c18);
		controller->task_c18 = NONE;
	}
	memset(&controller->identity, 0, sizeof(controller->identity));
	memset(controller->unknownb82, 0, sizeof(controller->unknownb82));
	if (TEST_FIELD_BIT(controller->active) && !function_1900a5(index))
	{
		controller->task_c14 = function_abc70(index, online_user_get_xuid(&controller->user));
	}
}

// @retail 0x1907bf
bool function_1907bf(byte frames_down, word msec_down)
{
	bool result = false;

	if (frames_down == 1 || msec_down >= 250)
	{
		result = true;
	}
	return result;
}

#define SIGN(value) ((value) == 0 ? 0 : ((value) >= 0 ? 1 : -1))

// @retail 0x190a3d
void function_190a3d(long controller, short *direction, dword time, long value)
{
	short x = direction[0];

	if (x == 0 && direction[1] == 0)
	{
		g_55e758.last_time[controller] = 0;
	}
	else
	{
		if (abs(x) < 0x7332 || abs(g_55e758.last_direction[controller][0]) < 0x7332)
		{
			if (abs(direction[1]) < 0x7332 || abs(g_55e758.last_direction[controller][1]) < 0x7332)
			{
				goto send;
			}
		}
		if (time - g_55e758.last_time[controller] < 250)
		{
			goto done;
		}
send:
		{
			s_controller_event event;

			event.amount = NONE;
			event.controller = controller;
			event.value = value;
			if (abs(x) >= 0x7332)
			{
				event.type = SIGN(x) > 0 ? 4 : 2;
			}
			else
			{
				short y = direction[1];
				if (abs(y) < 0x7332)
				{
					goto done;
				}
				event.type = SIGN(y) <= 0 ? 3 : 1;
			}
			function_147dbe(&event);
			g_55e758.last_time[controller] = time;
		}
	}
done:
	*(dword *)g_55e758.last_direction[controller] = *(dword *)direction;
}

// @retail 0x190b65
void function_190b65(long controller, byte button)
{
	s_controller_event event;

	event.type = 5;
	event.controller = controller;
	switch (button)
	{
	case 0:
		event.value = 0;
		break;
	case 1:
		event.value = 1;
		break;
	case 2:
		event.value = 2;
		break;
	case 3:
		event.value = 3;
		break;
	case 4:
		event.value = 4;
		break;
	case 5:
		event.value = 5;
		break;
	case 6:
		event.value = 6;
		break;
	case 7:
		event.value = 7;
		break;
	case 12:
		event.value = 12;
		break;
	case 13:
		event.value = 13;
		break;
	case 14:
		event.value = 14;
		break;
	case 15:
		event.value = 15;
		break;
	default:
		__assume(0);
	}
	event.amount = 0xff;
	function_147dbe(&event);
}

// @retail 0x1907d6
void function_1907d6(dword time)
{
	short controller;

	for (controller = 0; controller != NONE; controller = (short)controller_next(controller))
	{
		s_type_ff3a2a const *state;

		if (!g_4e61cc[controller])
		{
			continue;
		}
		state = function_1249a0(controller);
		if (!state)
		{
			continue;
		}

		short direction[2] = { 0, 0 };
		long value = 16;

		if (state->analog_button_frames_down[0] == 1)
		{
			function_190b65(controller, 0);
			return;
		}
		if (state->analog_button_frames_down[1] == 1)
		{
			function_190b65(controller, 1);
			return;
		}
		if (state->analog_button_frames_down[2] == 1)
		{
			function_190b65(controller, 2);
			return;
		}
		if (state->analog_button_frames_down[3] == 1)
		{
			function_190b65(controller, 3);
			return;
		}
		if (state->analog_button_frames_down[4] == 1)
		{
			function_190b65(controller, 4);
			return;
		}
		if (state->analog_button_frames_down[5] == 1)
		{
			function_190b65(controller, 5);
			return;
		}
		if (state->button_frames_down[4] == 1)
		{
			function_190b65(controller, 12);
			return;
		}
		if (state->button_frames_down[5] == 1)
		{
			function_190b65(controller, 13);
			return;
		}
		if (state->analog_button_frames_down[6] == 1)
		{
			function_190b65(controller, 6);
			return;
		}
		if (state->analog_button_frames_down[7] == 1)
		{
			function_190b65(controller, 7);
			return;
		}
		if (state->button_frames_down[6] == 1)
		{
			function_190b65(controller, 14);
			return;
		}
		if (state->button_frames_down[7] == 1)
		{
			function_190b65(controller, 15);
			return;
		}

		if (state->thumbsticks[0] != 0 || state->thumbsticks[1] != 0)
		{
			direction[0] = state->thumbsticks[0];
			if (abs(direction[0]) < 0x7332)
			{
				direction[0] = 0;
			}
			direction[1] = state->thumbsticks[1];
			if (abs(direction[1]) < 0x7332)
			{
				direction[1] = 0;
			}
		}
		if (direction[0] == 0 && direction[1] == 0)
		{
			if (function_1907bf(state->button_frames_down[3], state->button_msec_down[3]))
			{
				direction[0] = 1;
				direction[1] = 0;
				value = 11;
			}
			else if (function_1907bf(state->button_frames_down[2], state->button_msec_down[2]))
			{
				direction[0] = -1;
				direction[1] = 0;
				value = 10;
			}
			else if (function_1907bf(state->button_frames_down[0], state->button_msec_down[0]))
			{
				direction[1] = 1;
				value = 8;
				direction[0] = 0;
			}
			else if (function_1907bf(state->button_frames_down[1], state->button_msec_down[1]))
			{
				direction[1] = -1;
				value = 9;
				direction[0] = 0;
			}
			direction[0] *= 0x7fff;
			direction[1] *= 0x7fff;
		}
		if (direction[0] != 0 || direction[1] != 0)
		{
			function_190a3d(controller, direction, time, value);
			g_55e758.active[controller] = true;
		}
		else if (g_55e758.active[controller])
		{
			function_190a3d(controller, direction, time, value);
			g_55e758.active[controller] = false;
		}
	}
}

// @retail 0x190d0a
void function_190d0a(long index)
{
	s_controller *controller = controller_get(index);

	if (controller)
	{
		if (controller->task_c14 != NONE)
		{
			function_6b640(controller->task_c14);
			controller->task_c14 = NONE;
		}
		if (controller->task_c18 != NONE)
		{
			function_6b640(controller->task_c18);
			controller->task_c18 = NONE;
		}
	}
}

// @retail 0x190da5
void function_190da5(long index)
{
	s_controller *controller = controller_get(index);

	controller->active = false;
	controller->unknown204 = 0;
	controller->unknown46c = false;
	controller->unknown46d = false;
	memset(&controller->user, 0, sizeof(controller->user));
	memset(&controller->identity, 0, sizeof(controller->identity));
	memset(controller->unknownb82, 0, sizeof(controller->unknownb82));
	function_190d0a(index);
	controller->notification_dirty = false;
	memset(&controller->presence, 0, sizeof(controller->presence));
	controller->has_session = false;
	memset(controller->session_id, 0, sizeof(controller->session_id));
	memcpy(controller->name, controller->profile.name, sizeof(controller->name));
}

// @retail 0x190d4b
void function_190d4b(long index)
{
	s_controller *controller = controller_get(index);

	function_190da5(index);
	if (controller->session_user != NONE)
	{
		network_session_interface_clear_user(controller->session_user);
		controller->session_user = NONE;
	}
	function_18fc08(index);
	controller->session_user = NONE;
	*(dword *)controller = 0;
	memset(controller->unknown008, 0, sizeof(controller->unknown008));
	controller->unknown014 = NONE;
	controller->unknown1fc = 0;
	controller->unknown200 = 0;
	controller->name[0] = 0;
}

// @retail 0x190e37
void function_190e37(void)
{
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		s_controller *controller = controller_get(index);

		if ((*(dword *)controller & 0x10) && (*(dword *)controller & 0x40))
		{
			function_1a0660(controller->profile_index, &controller->profile);
		}
	}
}

// @retail 0x190e71
void function_190e71(long value)
{
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		s_controller *controller = controller_get(index);

		if ((*(dword *)controller & 0x10) && (*(dword *)controller & 0x40))
		{
			function_2153dd(index, controller->profile_index, (s_player_profile_settings *)&controller->profile, value);
		}
	}
}

/* a controller's gamepad preferences (0x1c bytes; unknown_218420.cpp) */
struct s_gamepad_preferences
{
	real look_sensitivity_horizontal;
	real look_sensitivity_vertical;
	char button_map[16];
	short unknown18;
	bool unknown1a;
	bool unknown1b;
};

void input_preferences_set_defaults(s_gamepad_preferences *preferences);

/* the input users' gamepad preferences (unknown_217c20.cpp) */
extern byte g_51ea18[];

/* the look speeds of the ten sensitivity settings */
const real g_444bdc[10] = { 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f, 120.0f, 130.0f };
const real g_444c04[10] = { 80.0f, 100.0f, 120.0f, 140.0f, 160.0f, 180.0f, 200.0f, 220.0f, 240.0f, 260.0f };

#define PIN(value, low, high) ((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))

/* sets the controller's gamepad preferences from its profile's settings */
// @retail 0x190c34
void function_190c34(long index)
{
	s_controller_settings *settings = (s_controller_settings *)((byte *)controller_get(index) + 0x114);
	s_gamepad_preferences preferences;

	short vertical_index;
	short horizontal_index;

	input_preferences_set_defaults(&preferences);
	vertical_index = PIN(settings->sensitivity, 1, 10) - 1;
	horizontal_index = PIN(settings->sensitivity, 1, 10) - 1;
	preferences.look_sensitivity_vertical = g_444bdc[vertical_index];
	preferences.look_sensitivity_horizontal = g_444c04[horizontal_index];
	preferences.unknown18 = settings->look_mode > 3 ? 3 : settings->look_mode;
	switch (settings->invert)
	{
	case 1:
		preferences.button_map[6] = 7;
		preferences.button_map[7] = 6;
		break;
	case 2:
		preferences.button_map[4] = 6;
		preferences.button_map[6] = 1;
		break;
	case 3:
		preferences.button_map[4] = 15;
		preferences.button_map[11] = 1;
		break;
	}
	preferences.unknown1a = settings->flag0;
	preferences.unknown1b = settings->flag2;
	((s_gamepad_preferences *)g_51ea18)[index] = preferences;
}

// @retail 0x1910b8
long function_1910b8(long controller)
{
	switch (controller)
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
		return 3;
	}
	return 4;
}

// @retail 0x1910d9
long function_1910d9(void)
{
	long index;

	for (index = 0; index != NONE; index = function_190262(index))
	{
		if (TEST_FIELD_BIT(controller_get(index)->signed_in))
		{
			long result = function_1904ff(index);
			if (result != NONE)
			{
				return result;
			}
			break;
		}
	}
	if (global_preferences_globals.current.unknown20 == NONE)
	{
		return 1;
	}
	return global_preferences_globals.current.unknown20;
}

// @retail 0x191234
void function_191234(long index)
{
	if (!TEST_FIELD_BIT(controller_get(index)->signed_in) && !function_148f36(index))
	{
		g_551ae0[index] = false;
	}
	else
	{
		g_551ae0[index] = true;
	}
}

/* the choice callback of a dialog: false while a controller that is signed
   in (or 148f36) has no 4e61cc value */
// @retail 0x191135
bool __stdcall function_191135(long controller_index)
{
	bool result = true;
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		if ((TEST_FIELD_BIT(controller_get(index)->signed_in) || function_148f36(index)) && !g_4e61cc[(short)index])
		{
			result = false;
			goto done;
		}
	}
	if (g_4e6948->state == 1)
	{
		g_510c54->unknown01 = false;
	}
done:
	return result;
}

/* the closed callback of the same dialog: shows the next controller's
   message instead */
// @retail 0x19119c
bool __stdcall function_19119c(c_class_1473c9 *screen, long dialog_id)
{
	long index;

	for (index = 0; index != NONE; index = controller_next(index))
	{
		if ((TEST_FIELD_BIT(controller_get(index)->signed_in) || function_148f36(index)) && !g_4e61cc[(short)index])
		{
			((c_dialog_screen *)screen)->set_dialog(controller_dialog_id(index), false);
			screen->set_user_flags(1 << index);
		}
	}
	if (g_4e6948->state == 1)
	{
		g_510c54->unknown01 = true;
	}
	return false;
}

char const *function_19c970(long campaign_id, long map_id);

bool window_manager_window_has_pause_screen_for_user(long channel, long index, long user_index);

/* one of the controllers' reconnect dialogs is showing */
// @retail 0x18f973
bool function_18f973(void)
{
	if (window_manager_window_has_pause_screen_for_user(0, 4, 0x1c)
		|| window_manager_window_has_pause_screen_for_user(0, 4, 0x1d)
		|| window_manager_window_has_pause_screen_for_user(0, 4, 0x1e)
		|| window_manager_window_has_pause_screen_for_user(0, 4, 0x1f))
	{
		return true;
	}
	return false;
}

/* the scenario's type at +0x10 */
struct s_scenario_type_view
{
	byte unknown00[0x10];
	short type;
};

bool function_2365f7(void);
long function_146840(void);
void function_125a90(long value);

/* asks the first controller that lost its connection to reconnect, and
   pauses the game */
// @retail 0x18f9be
void function_18f9be(void)
{
	dword index;

	for (index = 0; index < MAXIMUM_CONTROLLERS; index++)
	{
		if (g_551ae0[index])
		{
			if (!function_18f973() && function_2365f7())
			{
				short type;

				dialog_ok_show(0, controller_dialog_id(index), 4, 1 << index, function_191135, function_19119c);
				type = g_4e0350 ? ((s_scenario_type_view *)g_4e0350)->type : NONE;
				if (type == 0 && !function_146840())
				{
					g_510c54->unknown01 = true;
					function_125a90(0);
				}
			}
			return;
		}
	}
}

// @retail 0x191117
char const *function_191117(void)
{
	long map_id = function_1910d9();

	if (map_id != NONE)
	{
		char const *path = function_19c970(1, map_id);

		if (path)
			return path;
	}

	return "";
}

struct s_long_pair;
s_long_pair *network_session_interface_get_data_4999(void);
bool voice_port_can_talk(long port);
bool function_19a015(void);

/* updates the friends' view of the controller's user: online, playing,
   joinable, with voice, and the session to join */
// @retail 0x190f9b
void function_190f9b(long index)
{
	if (!function_1900a5(index) && function_6c7e0())
	{
		s_controller *controller = controller_get(index);
		XNKID const *session_id = (XNKID const *)network_session_interface_get_data_4999();
		dword flags = !controller->profile.appear_offline;

		if (!session_id)
		{
			if (controller->has_session)
			{
				memset(controller->session_id, 0, sizeof(controller->session_id));
				controller->has_session = false;
				controller->notification_dirty = true;
			}
		}
		else
		{
			if (!controller->has_session || memcmp(controller->session_id, session_id, sizeof(controller->session_id)) != 0)
			{
				controller->has_session = true;
				*(XNKID *)controller->session_id = *session_id;
				controller->notification_dirty = true;
			}
			if (controller->presence & 0xc0000000)
				flags |= XONLINE_FRIENDSTATE_FLAG_PLAYING;
			if (function_19a015())
				flags |= XONLINE_FRIENDSTATE_FLAG_JOINABLE;
		}
		if (voice_port_can_talk(index))
			flags |= XONLINE_FRIENDSTATE_FLAG_VOICE;
		if (controller->notification_flags != flags)
		{
			controller->notification_flags = flags;
			controller->notification_dirty = true;
		}
		if (controller->notification_dirty)
		{
			online_set_notification_state(controller->has_session ? (XNKID const *)controller->session_id : NULL, index, (BYTE *)&controller->presence, controller->notification_flags);
			controller->notification_dirty = false;
		}
	}
}