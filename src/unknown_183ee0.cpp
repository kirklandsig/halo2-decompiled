// @flags /O2 /Ob1 /Gr
/* UNKNOWN_183EE0.CPP: the structure bsp bit vectors (g_4ed280, allocated by
   183e40) and the eight slot identifiers (g_4eca60) with the tables that map
   to them (g_4ea960, g_4eaa60, g_4eca80); the lifecycle callbacks of the
   entry with 183e40 (initialize_for_new_structure_bsp and
   dispose_from_old_structure_bsp) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

#define FALSE 0
#define TRUE 1

/* g_4ed280's 0x4201 bytes: a flag, then per structure bsp a 256 bit vector
   and a block of 0x400 bytes */
struct s_bsp_bit_vectors
{
	byte unknown0;
	byte global_bits[16][0x20];
	byte bits[16][0x400];
};

extern byte *g_4ed280;
extern long g_4ea95c;

struct s_slot_pair
{
	long a;
	long b;
};

byte g_4ea960[0x100];
byte g_4eaa60[0x400][8];
s_slot_pair g_4eca80[0x100];

/* the view of the match globals the slot code reads */
struct s_slot_entry
{
	byte unknown0[4];
	byte flags;
	byte slot;
	byte unknown6[2];
};

struct s_slot_entry_list
{
	byte unknown00[0x28];
	long count;
	s_slot_entry *entries;
};

struct s_slot_owner
{
	byte unknown00[0x70];
	s_slot_entry_list list;
	byte unknown_a0[0xc8 - 0xa0];
};

struct s_slot_reference
{
	byte unknown00[0x34];
	short owner_index;
	byte unknown36[0x58 - 0x36];
};

struct s_match_globals_slot_view
{
	byte unknown00[0x13c];
	s_slot_owner *owners;
	long reference_count;
	s_slot_reference *references;
};

s_slot_entry_list *g_4e0340;

void function_b5920(long identifier);
void function_185630(void);

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

static inline byte *bsp_bit_vector(long index)
{
	if (index == NONE)
	{
		return ((s_bsp_bit_vectors *)g_4ed280)->global_bits[g_4686c4];
	}
	return &((s_bsp_bit_vectors *)g_4ed280)->bits[g_4686c4][index];
}

// @retail 0x185a30
void function_185a30(void)
{
	for (long i = 0; i < NUMBEROF(g_4eca60); i++)
	{
		if (g_4eca60[i] != NONE && g_4e6948->mode != 4)
		{
			function_b5920(g_4eca60[i]);
		}
		g_4eca60[i] = NONE;
	}
	g_4ea95c = 0;
}

// @retail 0x185a70
void function_185a70(void)
{
	for (long i = 0; i < 0x100; i++)
	{
		g_4eca80[i].b = NONE;
		g_4eca80[i].a = NONE;
	}
	memset(g_4ea960, 0xff, sizeof(g_4ea960));
	memset(g_4eaa60, 0xff, sizeof(g_4eaa60));
}

// @retail 0x183ee0
void function_183ee0(void)
{
	long mode = g_4e6948->mode;

	if (mode >= 4 && mode <= 5)
	{
		function_185a30();
		function_185630();
	}
}

// @retail 0x183f00
void function_183f00(void)
{
	function_185a30();
	function_185a70();
}

// @retail 0x183f10
void function_183f10(void)
{
	g_4ea95c = 0;
	for (long i = 0; i < NUMBEROF(g_4eca60); i++)
	{
		g_4eca60[i] = NONE;
	}
}

// @retail 0x183f50
bool function_183f50(long owner_index, long entry_index, long slot)
{
	bool result = false;
	s_slot_entry_list *list;

	if (owner_index == NONE)
	{
		list = g_4e0340;
	}
	else
	{
		s_match_globals_slot_view *globals = (s_match_globals_slot_view *)g_4e0348;
		if (owner_index >= 0 && owner_index < globals->reference_count)
		{
			list = &globals->owners[globals->references[owner_index].owner_index].list;
		}
		else
		{
			list = NULL;
		}
	}

	if (list && entry_index >= 0 && entry_index < list->count)
	{
		s_slot_entry *entry = &list->entries[entry_index];
		if ((entry->flags & 8) && entry->slot == slot)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x183fc0
byte *function_183fc0(long index)
{
	return bsp_bit_vector(index);
}

// @retail 0x184000
long function_184000(long bit, long index)
{
	if (bit != NONE)
	{
		dword *vector = (dword *)bsp_bit_vector(index);
		if (!(vector[bit >> 5] & (1 << (bit & 31))))
		{
			return FALSE;
		}
	}
	return TRUE;
}

// @retail 0x184400
long function_184400(long a, long b)
{
	long result = NONE;
	byte value = 0xff;

	if (a != NONE)
	{
		if (b == NONE)
		{
			value = g_4ea960[a];
		}
		else
		{
			value = g_4eaa60[b][a];
		}
	}
	if (value != NONE)
	{
		result = g_4eca60[value >> 5];
	}
	return result;
}

void function_a8e90(long slot);
long g_4ea958;

struct s_slot_group
{
	byte field_0[0x98];
	long count;
	short *references;
	byte field_a0[0x10];
};

struct s_slot_groups_view
{
	byte field_0[0x9c];
	long count;
	s_slot_group *groups;
};

PRIVATE inline long slot_identifier(long entry, long owner)
{
	long result = NONE;
	byte value = -1;
	if (entry != NONE)
	{
		if (owner == NONE) value = g_4ea960[entry];
		else value = g_4eaa60[owner][entry];
	}
	if (value != NONE) result = g_4eca60[value >> 5];
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

__forceinline void slot_assign(long owner, long entry, long index, bool *used)
{
	long slot = index / 32;
	if (owner == NONE) g_4ea960[entry] = (byte)index;
	else g_4eaa60[owner][entry] = (byte)index;
	g_4eca80[index].b = entry;
	g_4eca80[index].a = owner;
	if (!used[slot])
	{
		function_a8e90(slot);
		long count = g_4ea95c;
		_ReadWriteBarrier();
		s_game_options_view *options = g_4e6948;
		g_4ea95c = count + 1;
		_ReadWriteBarrier();
		char mode = options->mode;
		used[slot] = true;
		if (mode == 4 && slot >= 0 && slot < 8)
			g_4eca60[slot] = slot;
	}
}

// @retail 0x185630
void function_185630()
{
	s_match_globals_slot_view *globals = (s_match_globals_slot_view *)g_4e0348;
	s_slot_entry_list *list = g_4e0340;
	long count = 0;
	bool used[8] = { false };
	for (long i = 0; i < list->count; ++i)
	{
		s_slot_entry *entry = &list->entries[i];
		if ((entry->flags & 8) && slot_identifier(entry->slot, NONE) == NONE)
		{
			slot_assign(NONE, entry->slot, count, used);
			++count;
		}
	}
	s_slot_groups_view *groups = (s_slot_groups_view *)globals;
	for (long group_index = 0; group_index < groups->count; ++group_index)
	{
		s_slot_group *group = &groups->groups[group_index];
		for (long i = 0; i < group->count; ++i)
		{
			long owner = group->references[i];
			s_slot_owner *definition = &globals->owners[globals->references[owner].owner_index];
			for (long j = 0; j < definition->list.count; ++j)
			{
				s_slot_entry *entry = &definition->list.entries[j];
				if ((entry->flags & 8) && slot_identifier(entry->slot, owner) == NONE)
				{
					slot_assign(owner, entry->slot, count, used);
					++count;
				}
			}
		}
	}
	if (g_4e6948->mode == 4)
	{
		g_4ea95c = 0;
		for (long i = 0; i < 8; ++i) g_4eca60[i] = NONE;
	}
	g_4ea958 = count;
}
