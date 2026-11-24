// @flags /O1 /Oi /Gr
/* PLAYER_SLOTS.CPP: the four local player slots (g_54e8e0, one per
   controller): signing a slot in with a profile, saving the profile when it
   changes, and queries over the slots (player_slots.h) */

#include "player_slots.h"
#include "saved_game_storage.h"
#include <string.h>
#include <new>

extern dword g_4e61cc[k_player_slot_count];

/* the controller's online user is a guest (unknown_190001.cpp) */
bool function_1900a5(long index);

static inline s_player_slot_view *player_slots(void)
{
	return (s_player_slot_view *)g_54e8e0;
}

/* the slot after this one (NONE after the last, and after NONE) */
static inline long player_slot_next(long index)
{
	long next = NONE;

	if (index >= 0 && index < k_player_slot_count - 1)
	{
		next = index + 1;
	}
	return next;
}

/* player_slot_get.cpp: retail built it without LTCG, so its callers see
   the standard convention (it may clobber edx) */
s_player_slot_view *player_slot_get(long index);

/* constructs a slot (its sign-in profile) */
// @retail 0x18f564
s_player_slot_view::s_player_slot_view()
{
}

/* constructs the four slots in place */
// @retail 0x18f548
s_player_slot_view *__stdcall player_slots_construct(s_player_slot_view *slots)
{
	__assume(slots != 0);
	return new (slots) s_player_slot_view[k_player_slot_count];
}

/* whether the controller is plugged in */
// @retail 0x18f8b8
bool controller_is_connected(short index)
{
	// Retail keeps the controller argument on the stack.
	short const *index_reference = &index;
	return g_4e61cc[*index_reference] != 0;
}

/* the signed in slot that added this session user (NONE if none) */
// @retail 0x18f8ce
long player_slot_find_session_user(long session_user_index)
{
	long result = NONE;

	if (session_user_index != NONE)
	{
		long index = 0;
		do
		{
			if (TEST_FIELD_BIT(player_slots()[index].signed_in) && player_slots()[index].session_user_index == session_user_index)
			{
				result = index;
				break;
			}
			index = player_slot_next(index);
		} while (index != NONE);
	}
	return result;
}

/* how many slots are signed in */
// @retail 0x18f911
short player_slot_count_active(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].signed_in))
		{
			count++;
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	return count;
}

/* how many slots are signed in with an online user that is not a guest */
// @retail 0x18f93a
short player_slot_count_signed_in_non_guests(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].signed_in) && !function_1900a5(index))
		{
			count++;
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	return count;
}

/* the first signed in slot (mode 1 skips guests; NONE if none) */
// @retail 0x18fa4d
long player_slot_find_signed_in(long mode)
{
	long result = NONE;
	long index = 0;

	do
	{
		bool guest = false;
		if (mode == 1)
		{
			guest = function_1900a5(index);
		}
		if (TEST_FIELD_BIT(player_slots()[index].signed_in) && !guest)
		{
			result = index;
			break;
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	return result;
}

/* the first of these per-slot online users that has a xuid, a gamertag and
   no error, and is not a guest (NONE if none) */
// @retail 0x18fa94
long player_slot_find_online_user(XONLINE_USER *users)
{
	long result = NONE;
	long index = 0;

	do
	{
		if (users[index].xuid.qwUserID != 0 && users[index].szGamertag[0] && SUCCEEDED(users[index].hr) && !function_1900a5(index))
		{
			result = index;
			break;
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	return result;
}

/* the only signed in slot (NONE unless exactly one is signed in) */
// @retail 0x18fada
long player_slot_get_single_profile(void)
{
	long count;
	long result;
	long index;

	result = NONE;
	count = 0;
	index = 0;

	do
	{
		if (player_slots()[index].flags & _player_slot_flag_signed_in)
		{
			count++;
			result = index;
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);

	return count == 1 ? result : NONE;
}

/* the profile file of the only signed in slot (NONE if none) */
// @retail 0x18fb0d
long player_slot_get_single_profile_index(void)
{
	long result = NONE;
	long index = player_slot_get_single_profile();

	if (index != NONE)
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & _player_slot_flag_signed_in)
		{
			result = slot->profile_index;
		}
	}
	return result;
}

/* copies the slot's profile and its file (zeros and NONE when the slot is
   not signed in) */
// @retail 0x18fc44
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index)
{
	s_player_slot_view *slot = player_slot_get(index);

	if (slot && (slot->flags & _player_slot_flag_signed_in))
	{
		*profile = slot->profile;
		*profile_index = slot->profile_index;
	}
	else
	{
		memset(profile, 0, sizeof(*profile));
		*profile_index = NONE;
	}
}

/* whether a signed in slot holds this profile file */
// @retail 0x18fc75
bool player_slot_profile_in_use(long profile_index)
{
	bool result = false;

	for (long index = 0; !result && index != NONE; index = player_slot_next(index))
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & _player_slot_flag_signed_in)
		{
			result = !TEST_FIELD_BIT(slot->profile_index_flag) && slot->profile_index == profile_index;
		}
	}
	return result;
}

/* the slot's team */
// @retail 0x18fd7c
long player_slot_get_team(long index)
{
	long result = NONE;
	s_player_slot_view *slot = &player_slots()[index];

	if (slot)
	{
		result = slot->team;
	}
	return result;
}

/* copies the slot's online user */
// @retail 0x18ff47
void function_18ff47(long player, dword *out)
{
	s_player_slot_view *slot = &player_slots()[player];
	*(XONLINE_USER *)out = slot->user;
}

/* whether either of the slot's online tasks is still running */
// @retail 0x18ff64
bool player_slot_live_tasks_pending(long index)
{
	s_player_slot_view *slot = &player_slots()[index];

	return slot->live_task != NONE || slot->live_task2 != NONE;
}

/* copies the slot's identity (zeros when it has none) */
// @retail 0x18ff88
bool player_slot_get_identity(long index, s_player_identity *identity)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.xuid != 0;

	if (valid)
	{
		*identity = slot->identity;
	}
	else
	{
		memset(identity, 0, sizeof(*identity));
	}
	return valid;
}

/* copies the slot's online status (zeros when it has no identity) */
// @retail 0x18ffc3
bool function_18ffc3(long index, s_player_slot_blockb82 *block)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.xuid != 0;

	if (valid)
	{
		*block = slot->online_status;
	}
	else
	{
		memset(block, 0, sizeof(*block));
	}
	return valid;
}

struct s_type_fb9815;
inline void machine_identifier_build(s_type_fb9815 *identifier, long index);
/* adds a network session interface user with this xuid; its index */
long __stdcall function_64610(dword *xuid);
void function_2172a0(long handle);
void function_190c34(long index);
void function_190eb3(long index);
void function_53810(long voice_mask, long controller_index);
void function_54fc0(long controller_index, long voice_through_tv);

/* signs the slot in with a profile (adding its session user on first sign
   in), then applies the profile's controller, name and voice settings */
// @retail 0x18fb34
void __stdcall player_slot_set_profile(long index, s_player_profile_settings *settings, long profile_index)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool changed = !(slot->flags & _player_slot_flag_signed_in) || slot->profile_index != profile_index;

	if (!(slot->flags & _player_slot_flag_signed_in))
	{
		machine_identifier_build((s_type_fb9815 *)&slot->xuid, index);
		slot->session_user_index = function_64610((dword *)&slot->xuid);
		slot->profile_index = NONE;
	}
	if (changed && slot->profile_index != NONE)
	{
		function_2172a0(slot->profile_index);
	}
	slot->profile = *(s_player_profile *)settings;
	slot->flags |= _player_slot_flag_signed_in;
	slot->profile_index = profile_index;
	function_190c34(index);
	function_190eb3(index);
	if (changed && slot->profile_index != NONE)
	{
		g_55c160_table.count++;
		g_55c160_table.entries[g_55c160_table.count - 1].file_index = (void *)slot->profile_index;
	}
	function_53810(settings->voice_mask, index);
	function_54fc0(index, settings->voice_through_tv);
}

void saved_game_storage_update();
long function_2170e0(long file_index);
void online_mutelist_reset(long controller_index);
void function_190d4b(long index);
extern bool g_551ae4;

/* updates the saved game storage, then the profile file each signed in
   slot holds */
// @retail 0x18f5e3
void player_slots_refresh_profile_indices()
{
	saved_game_storage_update();
	long index = 0;
	do
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & _player_slot_flag_signed_in)
		{
			long *profile_index = &slot->profile_index;
			if (*profile_index != NONE)
				*profile_index = function_2170e0(*profile_index);
		}
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	g_55c14f = 0;
}

/* clears the slots: no session user, no online tasks, no muted players */
// @retail 0x18f58c
void player_slots_initialize()
{
	// the four slots and the 0x48 bytes after them
	memset(g_54e8e0, 0, 0x3208);
	long index = 0;
	do
	{
		s_player_slot_view *slot = &player_slots()[index];
		slot->session_user_index = NONE;
		slot->live_task = NONE;
		slot->live_task2 = NONE;
		online_mutelist_reset(index);
		function_190d4b(index);
		index = player_slot_iterate_next(index);
	} while (index != NONE);
	g_551ae4 = false;
}

bool game_loop_running(void);
/* stores the profile in its file (unknown_2153dd.cpp) */
void __stdcall function_2153dd(long player, long profile_index, s_player_profile_settings *settings, long flags);
void __stdcall player_slot_store_profile(long index, s_player_profile_settings *settings, long profile_index);

/* changes the slot's profile: during a game only in the slot, marking it
   unsaved; otherwise it saves it at once */
// @retail 0x18fcc4
void __stdcall player_slot_change_profile(long index, s_player_profile *profile, long profile_index)
{
	if (game_loop_running() && g_4e6948->state != 3)
	{
		s_player_slot_view *slot = &player_slots()[index];
		if ((slot->flags & _player_slot_flag_signed_in) && memcmp(&slot->profile, profile, sizeof(s_player_profile)))
		{
			player_slot_set_profile(index, (s_player_profile_settings *)profile, profile_index);
			slot->flags |= _player_slot_flag_profile_unsaved;
		}
	}
	else
		player_slot_store_profile(index, (s_player_profile_settings *)profile, profile_index);
}

/* sets the signed in slot's profile and saves it to its file when it is
   unsaved or differs */
// @retail 0x18fd20
void __stdcall player_slot_store_profile(long index, s_player_profile_settings *settings, long profile_index)
{
	s_player_slot_view *slot = &player_slots()[index];
	if (slot->flags & _player_slot_flag_signed_in)
	{
		if ((slot->flags & _player_slot_flag_profile_unsaved) || memcmp(&slot->profile, settings, sizeof(s_player_profile)))
		{
			player_slot_set_profile(index, settings, profile_index);
			if (profile_index != NONE)
				function_2153dd(index, profile_index, settings, 0);
			slot->flags &= ~_player_slot_flag_profile_unsaved;
		}
	}
}
