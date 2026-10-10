// @flags /O2 /Gr
/* UNKNOWN_1E5240.CPP: the entries of the actor's character tag (lane M;
   called by the behaviors of 0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "ai_actor.h"

struct s_entry_d4
{
	long field_00;
	long key;
	byte field_08[8];
};

struct s_tag_view_d4
{
	byte field_00[8];
	long parent_index;
	byte field_0c[0xd4 - 0xc];
	long count;
	s_entry_d4 *entries;
};

// @retail 0x1e5300
void *function_1e5300(long actor_index, long key)
{
	void *matched_entry_ptr = NULL;
	long index = actor_get(actor_index)->unknown054;
	while (index != NONE)
	{
		s_tag_view_d4 *tag = (s_tag_view_d4 *)g_4e3b44[index & 0xffff].bytes;
		short i = 0;
		while (i < tag->count)
		{
			s_entry_d4 *entry = &tag->entries[i];
			if (entry->key == key)
			{
				matched_entry_ptr = entry;
				goto done;
			}
			i++;
		}
		index = tag->parent_index;
	}
done:
	return matched_entry_ptr;
}

/* an entry (0xcc bytes) of the character's block at +0xcc, keyed by the
   tag index at +8 */
struct s_character_entry
{
	byte unknown00[8];
	long key;
	byte unknown0c[0xcc - 0xc];
};

struct s_character_view
{
	byte unknown00[8];
	long parent_index;
	byte unknown0c[0xcc - 0xc];
	long entry_count;
	s_character_entry *entries;
};

// @retail 0x1e5240
void *function_1e5240(long actor_index)
{
	void *result = NULL;
	long weapon_index = function_1e1f20(actor_index);

	if (weapon_index != NONE)
		result = function_1e5280(actor_index, ai_object_get(weapon_index)->definition_index);
	return result;
}

// @retail 0x1e5280
void *function_1e5280(long actor_index, long key)
{
	void *result = NULL;
	long character_index = actor_get(actor_index)->unknown054;

	while (character_index != NONE)
	{
		s_character_view *character = (s_character_view *)g_4e3b44[character_index & 0xffff].bytes;
		short i = 0;

		while (i < character->entry_count)
		{
			s_character_entry *entry = &character->entries[i];
			if (entry->key == key)
			{
				result = entry;
				goto done;
			}
			i++;
		}
		character_index = character->parent_index;
	}
done:
	return result;
}

/* an entry (0x3c bytes) of the character's block at +0xdc, keyed by the
   short at +4 */
struct s_character_entry_dc
{
	byte unknown00[4];
	short key;
	byte unknown06[0x3c - 0x6];
};

struct s_character_view_dc
{
	byte unknown00[8];
	long parent_index;
	byte unknown0c[0xdc - 0xc];
	long entry_count;
	s_character_entry_dc *entries;
};

void *function_1e53e0(long character_index, short key);

// @retail 0x1e5380
void *function_1e5380(long actor_index)
{
	void *result = NULL;
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;

	if (unit_index != NONE)
	{
		short index = ai_object_get(unit_index)->unknown23c;
		if (index != NONE)
			result = function_1e53e0(actor->unknown054, index);
	}
	return result;
}

// @retail 0x1e53e0
void *function_1e53e0(long character_index, short key)
{
	void *result = NULL;

	while (character_index != NONE)
	{
		s_character_view_dc *character = (s_character_view_dc *)g_4e3b44[character_index & 0xffff].bytes;
		short i = 0;

		while (i < character->entry_count)
		{
			s_character_entry_dc *entry = &character->entries[i];
			if (entry->key == key)
			{
				result = entry;
				goto done;
			}
			i++;
		}
		character_index = character->parent_index;
	}
done:
	return result;
}

struct s_actor_looking_properties;

// @retail 0x1e5160
s_actor_looking_properties *function_1e5160(long index)
{
	s_actor_looking_properties *result = NULL;
	if (index != NONE)
	{
		do
		{
			byte *data = g_4e3b44[index & 0xffff].bytes;
			if (*(long *)(data + 0x54) > 0)
			{
				result = *(s_actor_looking_properties **)(data + 0x58);
				break;
			}
			index = *(long *)(data + 8);
		} while (index != NONE);
	}
	return result;
}

struct s_select_tag
{
	byte field_00[8];
	long parent_index;
	byte field_0c[0x4c - 0xc];
	long count;
	byte *entries;
};

// @retail 0x1e51a0
void *function_1e51a0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long index = actor->unknown054;
	void *result = NULL;
	while (index != NONE)
	{
		s_select_tag *tag = (s_select_tag *)g_4e3b44[index & 0xffff].bytes;
		if (tag->count > 1 && actor->unknown26c != NONE)
		{
			result = tag->entries + 0x34;
			break;
		}
		if (tag->count > 2 && actor->unknown086 >= 4)
		{
			result = tag->entries + 0x68;
			break;
		}
		if (tag->count > 3 && actor->unknown086 <= 1)
		{
			result = tag->entries + 0x9c;
			break;
		}
		if (tag->count > 0)
		{
			result = tag->entries;
			break;
		}
		index = tag->parent_index;
	}
	return result;
}
