// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_219110.CPP: sound permutation selection and animation state accessors */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

struct s_permutation_group
{
	byte unknown00[0xe];
	short count;
};

struct s_set_ref
{
	byte unknown00[8];
	short base;
};

// @retail 0x219110
short function_219110(
	s_set_ref *ref,
	short offset,
	dword *used_mask,
	short previous,
	dword *seed,
	bool *all_used,
	bool mirror)
{
	s_permutation_set *set = &g_51ebd4->sets[ref->base + offset];
	long count = set->count;
	long limit = count * 2;
	long tries;
	short index;

	if (all_used)
	{
		*all_used = false;
	}

	tries = 0;

	while (true)
	{
		index = random_index(seed, count);

		if (!(~*used_mask & ((1 << set->count) - 1)))
		{
			*used_mask = 0;
			if (set->count > 1 && previous != NONE)
			{
				*used_mask = 1 << previous;
			}
			previous = NONE;
		}

		if (*used_mask & (1 << index))
		{
			continue;
		}

		*used_mask |= 1 << index;
		if (all_used && !(~*used_mask & ((1 << count) - 1)))
		{
			*all_used = true;
		}

		real chance = g_51ebd4->chances[set->first_index + index].chance * (1.0f / 65535.0f);
		if (mirror && chance >= 0.5f)
		{
			chance = 1.0f - chance;
		}

		if ((real)random_next(seed) * (1.0f / 65535.0f) >= chance)
		{
			break;
		}
		if (++tries == limit)
		{
			break;
		}
	}

	return index;
}

// @retail 0x219290
long function_219290(s_permutation_group *group, short index)
{
	if (group && index < group->count - 1)
	{
		return index + 1;
	}
	return NONE;
}

struct s_animation_entry
{
	byte unknown00[0xa];
	/* flags word: Bungie tested these as (bool)bitfield, which compiles to
	   mov al,[..]; shr al,N; test al,1 (see TEST_FLAG_BIT in unknown_11c920.h) */
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
	byte unknown0c[0x5c - 0xc];
};

struct s_animation_tag_data
{
	byte unknown00[4];
	s_animation_entry *entries;
};

struct s_animation_state
{
	byte unknown00[4];
	char a;
	char b;
	word c;
};

struct s_animation_ref
{
	byte unknown00[2];
	char index;
};

PRIVATE s_animation_entry *get_animation_entry(s_animation_ref *ref)
{
	s_tag_header_globals *globals = g_4e034c;
	s_tag_header *header = globals->header ? globals->header_alt : 0;
	return &g_4e3b44[header->datum_index & 0xffff].data->entries[ref->index];
}

// score 3: the final store puts the pointer in edx and the value in cl;
// retail puts the pointer in ecx and the value in dl. The permuter exhausted
// its mutations at this score.
// @retail 0x2192b0
void function_2192b0(s_animation_state *state, s_animation_ref *ref, byte value)
{
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		state->a = value;
	}
}

// score 3: same final-store register swap as 0x2192b0; manual variants could
// not change it.
// @retail 0x219310
void function_219310(s_animation_state *state, s_animation_ref *ref, short value)
{
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		state->b = value;
	}
}

// @retail 0x219370
long function_219370(s_animation_state *state, s_animation_ref *ref)
{
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		return state->a;
	}
	return NONE;
}

// @retail 0x2193d0
long function_2193d0(s_animation_state *state, s_animation_ref *ref)
{
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		return state->b;
	}
	return NONE;
}

// @retail 0x219430
long function_219430(s_animation_state *state, s_animation_ref *ref)
{
	if (TEST_FIELD_BIT(get_animation_entry(ref)->flag5))
	{
		word value = state->c;
		if (value == 0xffff)
		{
			return NONE;
		}
		return value;
	}
	return NONE;
}

struct s_object_ref
{
	byte unknown00[0xd];
	char index;
};

// @retail 0x2194a0
byte *function_2194a0(s_object_ref *ref)
{
	if (ref->index == NONE)
	{
		return 0;
	}
	return g_51ebd4->entries34 + ref->index * 0x34;
}

struct s_packed_value
{
	byte unknown00[6];
	word bit_offset;
	byte unknown08[2];
	short bit_count;
};

// @retail 0x2194c0
dword function_2194c0(s_packed_value *value, s_animation_ref *ref)
{
	dword result = 0;
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		if (value->bit_count > 1)
		{
			memcpy(&result, g_51ebd4->bits + (value->bit_offset >> 3), (dword)(value->bit_count + 7) >> 3);
		}
	}
	return result;
}

// @retail 0x219560
void function_219560(s_packed_value *value, s_animation_ref *ref, dword data)
{
	if (!(TEST_FIELD_BIT(get_animation_entry(ref)->flag5)))
	{
		if (value->bit_count > 1)
		{
			memcpy(g_51ebd4->bits + (value->bit_offset >> 3), &data, (dword)(value->bit_count + 7) >> 3);
		}
	}
}

/* 0x2195f0 (unknown_2197f0.cpp): decibels to a linear gain */
