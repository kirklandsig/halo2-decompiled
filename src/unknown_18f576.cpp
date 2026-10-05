// @flags /O1 /Oi /Gr
/* UNKNOWN_18F576.CPP: the local player slots (g_54e8e0, four slots of 0xc70
   bytes): the profile each slot holds, and queries over the slots */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_18f576.h"
#include "screen_widgets.h"
#include "unknown_24b5bc.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <new>

/* the profile a slot holds (0x1e0 bytes at +0x18) */
struct s_player_profile
{
	dword data[0x78];
};

/* the identity at +0x4e0 (0x6a2 bytes; set when the 64 bit id is not zero) */
#pragma pack(push, 2)
struct s_player_identity
{
	unsigned __int64 id;
	byte data[0x6a2 - 8];
};
#pragma pack(pop)

/* the head of a slot (0x470 bytes) */
struct s_player_slot_data
{
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword flag3 : 1;
			dword flag4 : 1;
			dword flag5 : 1;
			dword flag6 : 1;
		};
	};
	long controller_id;
	XUID xuid;
	byte unknown14[4];
	s_player_profile profile;
	union
	{
		long profile_index;
		struct
		{
			dword : 21;
			dword profile_flag21 : 1;
		};
	};
	long value1fc;
	byte value200;
	byte unknown201[3];
	byte value204;
	byte unknown205[3];
	s_player_slot_profile sign_in_profile;
	byte unknown46c[4];
};

/* a view of g_54e8e0's slots: the head, the online user at +0x470 (whose
   flags 1900a5 reads), then the identity */
struct s_player_slot_view : s_player_slot_data, XONLINE_USER
{
	s_player_slot_view();
	s_player_identity identity;
	s_player_slot_blockb82 blockb82;
	long valuec14;
	long valuec18;
	byte unknownc1c[0xc70 - 0xc1c];

};
#define k_player_slot_count 4

extern dword g_4e61cc[k_player_slot_count];

long function_190262(long value);
bool function_1900a5(long player);

static inline s_player_slot_view *player_slots(void)
{
	return (s_player_slot_view *)g_54e8e0;
}

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

// @retail 0x18f564
s_player_slot_view::s_player_slot_view()
{
}

// @retail 0x18f548
s_player_slot_view *__stdcall function_18f548(s_player_slot_view *slots)
{
	return new (slots) s_player_slot_view[k_player_slot_count];
}

// @retail 0x18f8b8
bool controller_is_connected(short index)
{
	return g_4e61cc[index] != 0;
}

// @retail 0x18f8ce
long player_slot_find_controller(long controller_id)
{
	long result = NONE;

	if (controller_id != NONE)
	{
		long index = 0;
		do
		{
			if (TEST_FIELD_BIT(player_slots()[index].flag4) && player_slots()[index].controller_id == controller_id)
			{
				result = index;
				break;
			}
			index = player_slot_next(index);
		} while (index != NONE);
	}
	return result;
}

// @retail 0x18f911
short player_slot_count_active(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].flag4))
		{
			count++;
		}
		index = function_190262(index);
	} while (index != NONE);
	return count;
}

// @retail 0x18f93a
short function_18f93a(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].flag4) && !function_1900a5(index))
		{
			count++;
		}
		index = function_190262(index);
	} while (index != NONE);
	return count;
}

// @retail 0x18fa4d
long function_18fa4d(long mode)
{
	long result = NONE;
	long index = 0;

	do
	{
		bool excluded = false;
		if (mode == 1)
		{
			excluded = function_1900a5(index);
		}
		if (TEST_FIELD_BIT(player_slots()[index].flag4) && !excluded)
		{
			result = index;
			break;
		}
		index = function_190262(index);
	} while (index != NONE);
	return result;
}

// @retail 0x18fa94
long function_18fa94(XONLINE_USER *users)
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
		index = function_190262(index);
	} while (index != NONE);
	return result;
}

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
		if (player_slots()[index].flags & 0x10)
		{
			count++;
			result = index;
		}
		index = function_190262(index);
	} while (index != NONE);

	return count == 1 ? result : NONE;
}

// @retail 0x18fb0d
long player_slot_get_single_profile_index(void)
{
	long result = NONE;
	long index = player_slot_get_single_profile();

	if (index != NONE)
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & 0x10)
		{
			result = slot->profile_index;
		}
	}
	return result;
}

// @retail 0x18fc44
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index)
{
	s_player_slot_view *slot = player_slot_get(index);

	if (slot && (slot->flags & 0x10))
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

// @retail 0x18fc75
bool player_slot_profile_in_use(long profile_index)
{
	bool result = false;

	for (long index = 0; !result && index != NONE; index = player_slot_next(index))
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & 0x10)
		{
			result = !TEST_FIELD_BIT(slot->profile_flag21) && slot->profile_index == profile_index;
		}
	}
	return result;
}

// @retail 0x18fd7c
long player_slot_get_value1fc(long index)
{
	s_player_slot_view *slot = &player_slots()[index];
	long result = NONE;

	if (slot)
	{
		result = slot->value1fc;
	}
	return result;
}

// @retail 0x18ff47
void function_18ff47(long player, dword *out)
{
	*(XONLINE_USER *)out = player_slots()[player];
}

// @retail 0x18ff64
bool function_18ff64(long index)
{
	s_player_slot_view *slot = &player_slots()[index];

	return slot->valuec14 != NONE || slot->valuec18 != NONE;
}

// @retail 0x18ff88
bool player_slot_get_identity(long index, s_player_identity *identity)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.id != 0;

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

// @retail 0x18ffc3
bool function_18ffc3(long index, s_player_slot_blockb82 *block)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.id != 0;

	if (valid)
	{
		*block = slot->blockb82;
	}
	else
	{
		memset(block, 0, sizeof(*block));
	}
	return valid;
}

struct s_type_fb9815;
void machine_identifier_build(s_type_fb9815 *identifier, long index);
long __stdcall function_64610(dword *xuid);
void function_2172a0(long handle);
void function_190c34(long index);
void function_190eb3(long index);
void function_53810(long voice_mask, long controller_index);
void function_54fc0(long controller_index, long voice_through_tv);

struct s_55c164
{
	void *field0;
	byte unknown04[0x40];
};
extern long g_55c160;
extern s_55c164 g_55c164[16];

// @retail 0x18fb34
void __stdcall function_18fb34(long index, s_player_profile_settings *settings, long profile_index)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool changed = !(slot->flags & 0x10) || slot->profile_index != profile_index;

	if (!(slot->flags & 0x10))
	{
		machine_identifier_build((s_type_fb9815 *)&slot->s_player_slot_data::xuid, index);
		slot->controller_id = function_64610((dword *)&slot->s_player_slot_data::xuid);
		slot->profile_index = NONE;
	}
	if (changed && slot->profile_index != NONE)
	{
		function_2172a0(slot->profile_index);
	}
	slot->profile = *(s_player_profile *)settings;
	slot->flags |= 0x10;
	slot->profile_index = profile_index;
	function_190c34(index);
	function_190eb3(index);
	if (changed && slot->profile_index != NONE)
	{
		g_55c160++;
		g_55c164[g_55c160 - 1].field0 = (void *)slot->profile_index;
	}
	function_53810(settings->voice_mask, index);
	function_54fc0(index, settings->voice_through_tv);
}
