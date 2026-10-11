// @flags /O2 /Gr
/* UNKNOWN_0805E0.CPP: the cache of the configurations of the
   players met recently (entries of 0x68 bytes at 0x4cf98c, chained through
   their next index). Most of the file is outside lane D's region
   (0x7fb50..0x80be7); lane D has the recent-player iterator. */

#include "unknown_11c920.h"
#include "unknown_2312b4.h"
#include <string.h>
#include <xtl.h>

struct s_player_configuration_cache_entry
{
	s_recent_player player;
	short unknown58;
	short next;
	short previous_other;
	short next_other;
	dword flags;
	union
	{
		byte unknown64[4];
		dword field_64;
	};
};

s_player_configuration_cache_entry g_4cf98c[350];

/* whether a XUID is an offline (machine-local) one */
static inline bool xuid_is_offline(unsigned __int64 xuid)
{
	return (xuid >> 48) == 0xfefe;
}

/* the entry at the iterator, moving the iterator to the next one */
static inline s_player_configuration_cache_entry *player_configuration_cache_iterate(long *iterator)
{
	s_player_configuration_cache_entry *entry = NULL;
	if (*iterator != NONE)
	{
		entry = &g_4cf98c[*iterator];
		*iterator = entry->next;
	}
	return entry;
}

static inline bool player_configuration_is_excluded(s_recent_player const *player)
{
	return (player->unknown08[0] & 3) != 0;
}

/* the next recent player after the iterator's: a valid, online, known player
   with a name; iterator starts at the first entry and ends at NONE */
// @retail 0x805e0
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator)
{
	bool result = false;
	long index = *iterator;
 long local_0;
	if (index != NONE)
	{
		do
		{
			s_player_configuration_cache_entry *entry = &g_4cf98c[index];
			local_0 = entry->next;
			*iterator = local_0;
			if (!(entry->flags & 4) && (entry->flags & 0x10) &&
			*(unsigned __int64 *)entry->player.xuid != 0 &&
			!xuid_is_offline(*(unsigned __int64 *)entry->player.xuid) &&
			!player_configuration_is_excluded(&entry->player) &&
			*(word *)&entry->player.unknown08[4] != 0)
			{
				if (player)
					memcpy(player, &entry->player, sizeof(s_recent_player));
				return true;
			}
			index = local_0;
		} while (index != NONE);
	}
	return result;
}

long g_4cf97c;
long g_4cf980;
extern long g_4cf984;
long g_4cf988;
long g_4cf978;

long __stdcall function_7fc80(const void *identity, long *position);

struct s_cache_property
{
	long unknown00;
	long type;
	long unknown08;
	long unknown0c;
};

struct s_cache_property_record
{
	byte identity[12];
	long type;
	long count;
	s_cache_property *properties;
};

// @retail 0x80940
void function_80940(s_cache_property_record *records, long record_capacity,
	s_cache_property *properties, long property_capacity, long buffer_capacity,
	byte *buffers, long *record_count, long *property_count, long *buffer_count)
{
	long records_used = 0;
	long properties_used = 0;
	long buffers_used = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (entry->flags & 2)
		{
			if (record_capacity <= 0 || property_capacity < 4 || buffer_capacity <= 0)
				break;
			memcpy(records->identity, &entry->player, sizeof(records->identity));
			records->type = 0x61;
			records->count = 4;
			records->properties = properties;
			records++;
			record_capacity--;
			records_used++;
			*(short *)&properties->unknown00 = -3;
			properties->type = 4;
			properties->unknown08 = (long)buffers;
			properties++;
			property_capacity--;
			properties_used++;
			buffers += 0x40;
			buffer_capacity--;
			buffers_used++;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[j].unknown00 = (short)(j + 2);
				properties[j].type = 1;
				properties[j].unknown08 = 0;
			}
			properties += 3;
			property_capacity -= 3;
			properties_used += 3;
		}
	}
	if (record_count)
		*record_count = records_used;
	if (property_count)
		*property_count = properties_used;
	if (buffer_count)
		*buffer_count = buffers_used;
}

void function_80d70(s_cache_property_record *records, long count);

// @retail 0x7fd10
void function_7fd10(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->previous_other != NONE)
		g_4cf98c[entry->previous_other].next_other = entry->next_other;
	if (entry->next_other != NONE)
		g_4cf98c[entry->next_other].previous_other = entry->previous_other;
	if (g_4cf97c == index)
		g_4cf97c = entry->next_other;
	if (g_4cf980 == index)
		g_4cf980 = entry->previous_other;
}

// @retail 0x7fd80
void function_7fd80(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->unknown58 != NONE)
		g_4cf98c[entry->unknown58].next = entry->next;
	if (entry->next != NONE)
		g_4cf98c[entry->next].unknown58 = entry->unknown58;
	if (g_4cf984 == index)
		g_4cf984 = entry->next;
	if (g_4cf988 == index)
		g_4cf988 = entry->unknown58;
}

// @retail 0x7fc80
long __stdcall function_7fc80(const void *identity, long *position)
{
	long middle = 0;
	long lower = 0;
	long upper = g_4cf978 - 1;
	long result = NONE;
	bool last = false;
	while (result == NONE && !last && lower <= upper)
	{
		last = lower == upper;
		middle = (lower + upper) >> 1;
		long comparison = memcmp(identity, &g_4cf98c[middle].player, 12);
		if (comparison == 0)
			result = middle;
		else if (comparison < 0)
			upper = middle - 1;
		else
			lower = ++middle;
	}
	if (position)
		*position = middle;
	return result;
}

struct s_cached_player_identity
{
	dword values[3];
};
struct s_cached_player_source
{
	word name[32];
	dword field40;
	union
	{
		dword field44_47;
		struct
		{
			byte field44;
			byte field45;
			byte field46;
			byte field47;
		};
	};
	unsigned __int64 field48;
	byte unknown50[0x70 - 0x50];
	long field70;
	long field74;
	long field78;
	byte unknown7c[3];
	byte field7f;
	byte unknown80[0x10];
};
struct s_cached_player_view
{
	s_cached_player_identity identity;
	word name[32];
	dword field4c;
	byte field50;
	byte field51;
	byte field52;
	byte field53;
	byte unknown54[2];
	byte field56;
	byte unknown57;
};

// @retail 0x7f8b0
void function_7f8b0(const s_cached_player_identity *identity, s_cached_player_view *result,
	const s_cached_player_source *source, const s_cached_player_view *previous)
{
	memset(result, 0, sizeof(*result));
	result->identity = *identity;
	memcpy(result->name, source->name, sizeof(result->name));
	result->field4c = source->field40;
	result->field50 = source->field44;
	result->field51 = source->field45;
	result->field52 = source->field46;
	result->field53 = source->field47;
	byte index = source->field7f;
	if (index == 0xff && previous)
		result->field56 = previous->field56;
	else
		result->field56 = index;
}

// @retail 0x7fdf0
void function_7fdf0(long index)
{
	if (g_4cf97c != index)
	{
		function_7fd10(index);
		if (g_4cf97c != NONE)
			g_4cf98c[g_4cf97c].previous_other = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->previous_other = NONE;
		entry->next_other = (short)g_4cf97c;
		g_4cf97c = index;
	}
}

// @retail 0x7fe40
void function_7fe40(long index)
{
	if (g_4cf984 != index)
	{
		function_7fd80(index);
		if (g_4cf984 != NONE)
			g_4cf98c[g_4cf984].unknown58 = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->unknown58 = NONE;
		entry->next = (short)g_4cf984;
		g_4cf984 = index;
	}
}

long g_4cf970;
long g_4cf974;

struct s_player_appearance;
bool function_153750(s_player_appearance *appearance);

// @retail 0x80660
bool function_80660(void)
{
	bool valid = g_4cf970 == 8;
	if (g_4cf978 < 0 || g_4cf978 > 350)
		valid = false;
	for (long i = 0; valid && i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (entry > g_4cf98c && memcmp(&entry[-1].player, &entry->player, 12) >= 0)
			valid = false;
		if (valid)
		{
			byte appearance[16];
			memset(appearance, 0, sizeof(appearance));
			s_cached_player_view *view = (s_cached_player_view *)&entry->player;
			*(dword *)appearance = view->field4c;
			appearance[4] = view->field50;
			appearance[5] = view->field51;
			appearance[6] = view->field52;
			appearance[7] = view->field53;
			valid = function_153750((s_player_appearance *)appearance);
			if (valid)
				valid = !(view->identity.values[2] & 3) && view->identity.values[2] != 0xbad00000;
		}
	}
	if ((g_4cf97c == NONE || g_4cf980 == NONE) && g_4cf97c != g_4cf980)
		valid = false;
	if (g_4cf97c != NONE && (g_4cf97c < 0 || g_4cf97c >= g_4cf978))
		valid = false;
	if (g_4cf980 != NONE && (g_4cf980 < 0 || g_4cf980 >= g_4cf978))
		valid = false;
	if ((g_4cf984 == NONE || g_4cf988 == NONE) && g_4cf984 != g_4cf988)
		valid = false;
	if (g_4cf984 != NONE && (g_4cf984 < 0 || g_4cf984 >= g_4cf978))
		valid = false;
	if (g_4cf988 != NONE && (g_4cf988 < 0 || g_4cf988 >= g_4cf978))
		valid = false;
	if (valid && g_4cf97c != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf97c;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			dword bit = 1 << (index & 31);
			if (visited[index >> 5] & bit)
				valid = false;
			else
			{
				visited[index >> 5] |= bit;
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0 ? entry->previous_other != NONE :
					entry->previous_other < 0 || entry->previous_other >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next_other != NONE || index != g_4cf980)
						valid = false;
				}
				else if (entry->next_other < 0 || entry->next_other >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next_other;
					if (g_4cf98c[next].previous_other != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	if (valid && g_4cf984 != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf984;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			dword bit = 1 << (index & 31);
			if (visited[index >> 5] & bit)
				valid = false;
			else
			{
				visited[index >> 5] |= bit;
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0 ? entry->unknown58 != NONE :
					entry->unknown58 < 0 || entry->unknown58 >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next != NONE || index != g_4cf988)
						valid = false;
				}
				else if (entry->next < 0 || entry->next >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next;
					if (g_4cf98c[next].unknown58 != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	return valid;
}

// @retail 0x7f930
void function_7f930(void)
{
	g_4cf970 = 8;
	g_4cf974 = 0;
	g_4cf978 = 0;
	g_4cf980 = NONE;
	g_4cf97c = NONE;
	g_4cf984 = NONE;
	g_4cf988 = NONE;
	for (long i = 0; i < 350; i++)
	{
		s_cached_player_identity identity;
		memset(&identity, 0, sizeof(identity));
		s_cached_player_source source;
		memset(&source, 0, sizeof(source));
		source.field40 = 0;
		source.field44_47 = 0;
		source.field48 = 0;
		source.field70 = 0;
		source.field74 = 0;
		source.field78 = 0;
		source.field7f = 0xff;
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		function_7f8b0(&identity, (s_cached_player_view *)&entry->player, &source, NULL);
		entry->next = entry->next_other = entry->unknown58 = entry->previous_other = NONE;
		entry->flags = 0;
		*(long *)entry->unknown64 = 0;
	}
}

// @retail 0x80bf0
void function_80bf0(s_cache_property_record *records, long record_capacity,
	s_cache_property *properties, long property_capacity, byte *buffers,
	long *record_count, long *property_count, long *buffer_count)
{
	long records_used = 0;
	long properties_used = 0;
	long buffers_used = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (entry->flags & 1)
		{
			if (record_capacity <= 0 || property_capacity < 4)
				break;
			long values[3];
			memcpy(values, (byte *)&entry->player + 0x4c, sizeof(values));
			memcpy(records->identity, &entry->player, sizeof(records->identity));
			records->type = 0x61;
			records->count = 4;
			records->properties = properties;
			records++;
			record_capacity--;
			records_used++;
			memcpy(buffers, (byte *)&entry->player + 0xc, 0x40);
			*(short *)&properties->unknown00 = -3;
			properties->type = 4;
			properties->unknown08 = (long)buffers;
			properties++;
			property_capacity--;
			properties_used++;
			buffers += 0x40;
			buffers_used++;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[j].unknown00 = (short)(j + 2);
				properties[j].type = 1;
				properties[j].unknown08 = values[j];
			}
			properties += 3;
			property_capacity -= 3;
			properties_used += 3;
		}
	}
	if (record_count)
		*record_count = records_used;
	if (property_count)
		*property_count = properties_used;
	if (buffer_count)
		*buffer_count = buffers_used;
}

#include <xtl.h>
#include <wchar.h>

// @retail 0x80aa0
void function_80aa0(s_cache_property_record *records, long count)
{
	long position;
	if (count <= 0) return;
 s_cache_property *const *local_0 = &records->properties;
 long local_2 = count;
 do
	{
		const s_cached_player_identity *local_1 = (const s_cached_player_identity *)((byte const *)local_0 - 0x14);
		if (*(long const *)((byte const *)local_0 - 8) == 0x61 && *(long const *)((byte const *)local_0 - 4) == 4)
		{
			long index = function_7fc80(local_1, &position);
			if (index != NONE)
			{
				s_cache_property *properties = *local_0;
				long values[3];
				long present = 0;
				long absent = 0;
				if (properties[0].type == 0)
					absent = 1;
				else if (properties[0].type == 4 && properties[0].unknown08 != 0)
					present = 1;
				for (long j = 0; j < 3; j++)
				{
					if (properties[j + 1].type == 0)
						absent++;
					else if (properties[j + 1].type == 1)
					{
						values[j] = properties[j + 1].unknown08;
						present++;
					}
				}
				if (absent + present == 4)
				{
					s_player_configuration_cache_entry *entry = &g_4cf98c[index];
					if (present == 4)
					{
						wchar_t *name = (wchar_t *)((byte *)&entry->player + 0xc);
						wcsncpy(name, (const wchar_t *)properties[0].unknown08, 31);
						name[31] = 0;
						memcpy((byte *)&entry->player + 0x4c, values, sizeof(values));
						entry->flags &= ~8;
					}
					unsigned __int64 time = 0;
					GetSystemTimeAsFileTime((FILETIME *)&time);
					*(dword *)entry->unknown64 = (dword)(time / 3600000000ULL);
					entry->flags &= ~2;
				}
			}
		}
  local_0 = (s_cache_property *const *)((byte const *)local_0 + 0x18);
	} while (--local_2);
}

bool g_51055c;

struct s_profile_record;
void function_1537f0(s_profile_record *record);
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source);

#pragma inline_depth(0)
static __forceinline void function_80491(long arg_0)
{
 function_7fe40(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80490
void function_80490(const s_cached_player_identity *identity, const s_cached_player_source *source,
	bool local, bool recent)
{
	bool time_changed = false;
	long position = 0;
	long index = function_7fc80(identity, &position);
	bool changed;
	if (index == NONE)
	{
		index = function_7fe90(identity, position, source);
		changed = true;
	}
	else
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		s_cached_player_view player;
		function_7f8b0(identity, &player, source, (const s_cached_player_view *)&entry->player);
		changed = memcmp(&entry->player, &player, sizeof(entry->player)) != 0;
		if (changed)
			memcpy(&entry->player, &player, sizeof(entry->player));
		entry->flags &= ~10;
		unsigned __int64 time = 0;
		GetSystemTimeAsFileTime((FILETIME *)&time);
		dword hours = (dword)(time / 3600000000ULL);
		if (hours != *(dword *)entry->unknown64)
			time_changed = true;
		*(dword *)entry->unknown64 = hours;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (local)
	{
		if (changed)
			entry->flags |= 1;
		entry->flags |= 4;
	}
	else
		entry->flags &= ~4;
	if (recent)
	{
		changed = changed || !(entry->flags & 16);
		entry->flags |= 16;
	}
	function_80491(index);
	if (changed || time_changed)
		g_51055c = true;
}

#pragma inline_depth(0)
// @retail 0x80330
long __stdcall function_80330(const s_cached_player_identity *identity, long position)
{
	s_cached_player_source source;
	function_1537f0((s_profile_record *)&source);
	((byte *)&source.field40)[2] = 0;
	((byte *)&source.field40)[3] = 0;
	source.field45 = 0;
	source.field46 = 0;
	((byte *)&source.field40)[0] = 10;
	((byte *)&source.field40)[1] = 10;
	long index = function_7fe90(identity, position, &source);
	if (index != NONE)
 {
  s_player_configuration_cache_entry *local_0 = g_4cf98c + index;
  local_0->flags |= 10;
 }
	return index;
}
#pragma inline_depth(8)

// @retail 0x7fe90
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source)
{
	long removed = NONE;
	if (g_4cf978 == 350)
	{
		removed = g_4cf980;
		function_7fd10(removed);
		function_7fd80(removed);
	}
	long lower, upper;
	bool downward = false;
	if (removed != NONE)
	{
		lower = removed < position ? removed : position;
		upper = removed > position ? removed : position;
		if (removed < position)
		{
			upper--;
			position--;
			downward = true;
		}
	}
	else
	{
		lower = position;
		upper = g_4cf978;
	}
	if (upper > lower)
	{
		if (downward)
		{
			for (long i = lower + 1; i <= upper; i++)
				g_4cf98c[i - 1] = g_4cf98c[i];
		}
		else
		{
			for (long i = upper - 1; i >= lower; i--)
				g_4cf98c[i + 1] = g_4cf98c[i];
		}
		long change = downward ? -1 : 1;
		for (long i = 0; i < 350; i++)
		{
			s_player_configuration_cache_entry *entry = &g_4cf98c[i];
			if (entry->previous_other >= lower && entry->previous_other <= upper) entry->previous_other += (short)change;
			if (entry->next_other >= lower && entry->next_other <= upper) entry->next_other += (short)change;
			if (entry->unknown58 >= lower && entry->unknown58 <= upper) entry->unknown58 += (short)change;
			if (entry->next >= lower && entry->next <= upper) entry->next += (short)change;
		}
		if (g_4cf97c >= lower && g_4cf97c <= upper) g_4cf97c += change;
		if (g_4cf980 >= lower && g_4cf980 <= upper) g_4cf980 += change;
		if (g_4cf984 >= lower && g_4cf984 <= upper) g_4cf984 += change;
		if (g_4cf988 >= lower && g_4cf988 <= upper) g_4cf988 += change;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[position];
	entry->next_other = (short)g_4cf97c;
	entry->previous_other = NONE;
	entry->next = NONE;
	entry->unknown58 = (short)g_4cf988;
	entry->flags = 0;
	unsigned __int64 now = 0;
	GetSystemTimeAsFileTime((FILETIME *)&now);
	*(dword *)entry->unknown64 = (dword)(now / 3600000000ui64);
	memset(&entry->player, 0, sizeof(entry->player));
	s_cached_player_view *player = (s_cached_player_view *)&entry->player;
	player->identity = *identity;
	memcpy(player->name, source->name, sizeof(player->name));
	player->field4c = source->field40;
	player->field50 = source->field44;
	player->field51 = source->field45;
	player->field52 = source->field46;
	player->field53 = source->field47;
	player->field56 = source->field7f;
	if (g_4cf97c != NONE && g_4cf97c != position)
		g_4cf98c[g_4cf97c].previous_other = (short)position;
	g_4cf97c = position;
	if (g_4cf980 == NONE) g_4cf980 = position;
	if (g_4cf988 != NONE && g_4cf988 != position)
		g_4cf98c[g_4cf988].next = (short)position;
	g_4cf988 = position;
	if (g_4cf984 == NONE) g_4cf984 = position;
	g_51055c = true;
	if (removed == NONE) g_4cf978++;
	return position;
}

struct s_network_session_player;

// @retail 0x805d0
void __fastcall function_805d0(s_network_session_player *player)
{
 function_80490((const s_cached_player_identity *)player,
  (const s_cached_player_source *)((const byte *)player + 0xa8), false, true);
}

#pragma inline_depth(0)
static __forceinline void function_80441(long arg_0)
{
 function_7fdf0(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80440
void function_80440(const s_cached_player_identity *identity, s_recent_player *player)
{
 long position = 0;
 long index = function_7fc80(identity, &position);
 if (index == NONE)
  index = function_80330(identity, position);
 memcpy(player, &g_4cf98c[index].player, sizeof(*player));
 function_80441(index);
}

bool g_510554;
dword g_510558;
bool function_7fb50(void);

// @retail 0x80390
void function_80390(void)
{
 if (g_51055c && g_510554)
 {
  dword local_0 = GetTickCount();
  if (local_0 - g_510558 > 10000)
  {
   if (function_7fb50())
    g_51055c = false;
   g_510558 = local_0;
  }
 }
 unsigned __int64 local_1 = 0;
 GetSystemTimeAsFileTime((FILETIME *)&local_1);
 dword local_2 = (dword)(local_1 / 3600000000ULL);
 long local_3 = 0;
 s_player_configuration_cache_entry *local_4 = g_4cf98c;
 if (g_4cf978 > 0)
 {
  do
  {
   if (local_2 - local_4->field_64 > 72)
    local_4->flags |= 2;
   local_3++;
   local_4++;
  } while (local_3 < g_4cf978);
 }
}
