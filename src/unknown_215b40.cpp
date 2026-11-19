#include "unknown_11c920.h"
#include "unknown_19d220.h"
#include "saved_game_storage.h"
#include <string.h>

// @flags /O2 /Ob1 /Gr

class c_location_record
{
public:
	c_location_record();
	char path[0x14];
	word name[0x16];
};

struct s_location_record_table
{
	long count;
	c_location_record entries[0x1000];
};

extern void *g_51ea14;
bool function_216800(long file_index, void *location);

// @retail 0x215b40
c_location_record::c_location_record()
{
	path[0] = 0;
	name[0] = 0;
}

// @retail 0x216060
bool function_216060(long file_index, word const *name)
{
	bool result = false;
	struct
	{
		char path[0x14];
		word name[0x16];
	} location;
	location.path[0] = 0;
	location.name[0] = 0;
	if (function_216800(file_index, &location) && name && name[0])
	{
		if (wcsncmp(location.name, name, 0x80))
			result = true;
	}
	return result;
}

// @retail 0x216990
bool function_216990(long unit, c_location_record const *record, long *index)
{
	bool result = false;
	if (g_51ea14)
	{
		s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
		if (table->count != 0x1000)
		{
			long slot = table->count++;
			table->entries[slot] = *record;
			*index = table->count - 1;
			result = true;
		}
	}
	return result;
}

/* removes a file's entry from its storage device's list in the cache */
// @retail 0x2169e0
void saved_game_storage_remove_location(long file_index)
{
	if (g_51ea14)
	{
		long unit = (file_index >> 4) & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		s_saved_game_location_table *table = &saved_game_storage_get_cache()->locations + unit;
		long count = table->count;
		long bounded_index = index < 0 ? 0 : index > count - 1 ? count - 1 : index;
		if (bounded_index == index)
		{
			if (index < count - 1)
			{
				s_saved_game_file_location *location = &table->entries[index];
				memmove(location, location + 1, (count - index - 1) * sizeof(s_saved_game_file_location));
			}
			table->count--;
		}
	}
}

// @retail 0x217300
long function_217300(long file_index, long removed_index)
{
	if (!((bool)(((dword)file_index >> 21) & 1)))
	{
		void *storage = g_51ea14;
		if (storage)
		{
			long unit = (file_index >> 4) & 0xf;
			long removed_unit = (removed_index >> 4) & 0xf;
			if (removed_unit == unit)
			{
				long index = (file_index >> 8) & 0x1fff;
				long removed = (removed_index >> 8) & 0x1fff;
				if (index > removed)
				{
					long generation = *(long *)((byte *)storage + 0x4befc);
					file_index = (file_index & 0xf) | ((unit & 0xf) << 4) |
						(((index - 1) & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
				}
			}
		}
	}
	return file_index;
}

struct s_cached_location_entry
{
	long type;
	byte data[0x1e0];
};

struct s_cached_location_table
{
	long count;
	s_cached_location_entry entries[1];
};

PRIVATE __forceinline word *copy_location_name(word *destination, word const *source)
{
	wcsncpy(destination, source, 0x7f);
	destination[0x7f] = 0;
	return destination;
}

// @retail 0x215b50
word *function_215b50(long file_index, word *name)
{
	word *result = name;
	s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
	name[0] = 0;
	if (files && (bool)(((dword)file_index >> 21) & 1))
	{
		long type = file_index & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		long bounded_index = index < 0 ? 0 : index > files->count - 1 ? files->count - 1 : index;
		if (bounded_index == index)
		{
			s_cached_location_entry *entry = &files->entries[index];
			if (type == 0)
			{
				return copy_location_name(result, (word *)(entry->data + 8));
			}
			else
			{
				long bounded_type = type < 1 ? 1 : type > 9 ? 9 : type;
				if (bounded_type == type)
				{
					return copy_location_name(result, (word *)(entry->data + 4));
				}
			}
		}
	}
	else
	{
		struct
		{
			char path[0x14];
			word name[0x16];
		} location;
		location.path[0] = 0;
		location.name[0] = 0;
		if (function_216800(file_index, &location))
		{
			return copy_location_name(result, location.name);
		}
	}
	return result;
}

struct s_player_profile;
void function_1a0750(s_player_profile *profile);
const word *saved_game_storage_get_default_profile_name(long language);
long g_55c280;

// @retail 0x216760
void function_216760(void)
{
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		s_cached_location_entry entry;
		word name[0x100];
		entry.type = 0;
		name[0] = 0;
		word const *default_name = saved_game_storage_get_default_profile_name(g_55c280);
		word const *const *name_reference = &default_name;
		wcsncpy(name, *name_reference, 0xff);
		name[0xff] = 0;
		function_1a0750((s_player_profile *)entry.data);
		wcsncpy((word *)(entry.data + 8), name, 0x1f);
		((word *)(entry.data + 8))[0x1f] = 0;
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		long index = files->count++;
		files->entries[index] = entry;
	}
}

long saved_game_file_type_from_variant(s_game_variant *variant);

// @retail 0x217080
void __stdcall function_217080(s_game_variant const *variant)
{
	s_game_variant const *const *variant_reference = &variant;
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		union
		{
			s_cached_location_entry entry;
			unsigned __int64 words[0x1e8 / 8];
		} value;
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		*(s_game_variant *)value.entry.data = **variant_reference;
		value.entry.type = saved_game_file_type_from_variant((s_game_variant *)value.entry.data);
		long index = files->count;
		files->count = index + 1;
		files->entries[index] = value.entry;
	}
}

typedef bool (__stdcall *t_location_compare)(long, long, void const *);
void sort_4byte(void *unused, long *elements, unsigned long count, t_location_compare compare, void const *context);
bool function_1249f0(long memory_unit, char *drive_letter);

// @retail 0x2163d0
bool __stdcall function_2163d0(long first, long second, void const *context)
{
	bool result;
	dword first_cached = (dword)first >> 21;
	dword second_cached = (dword)second >> 21;
	if (!((first_cached ^ second_cached) & 1))
		result = first > second;
	else
		result = (bool)((byte)first_cached & 1);
	return result;
}

// score 114 (was 114): BLOCKED-BY the `/GL-` flag of src/unknown_215880.cpp (see round 141 below);
// check with 0x2157e0 in the image (it defines g_51ea14;
// check with 0x2157e0 in the image (it defines g_51ea14;
// check with 0x2157e0 in the image (it defines g_51ea14;
// without it the fast check builds differently). The iterator struct gives
// retail's frame: table at [esp+0x24], the index's slot [esp+0x28] unused, and
// index << 6 recomputed for each copy (a plain table pointer and index
// strength-reduce the entry pointer instead, 156). Still differs: retail keeps
// the old index in eax and packs ((gen << 14 | index) << 4 | unit) << 4 | type,
// we pack (index << 8) - 1 & 0x1fff00; retail reloads the table at the loop
// top (we reload it in the match block) and reloads g_51ea14 into edx at the
// bottom of the outer loop; prologue zeroes ebp before ebx.
// Tried: long old_index = iterator.index++ (167), index read before the copy
// (170, 195), for (; count < *capacity && index < count; index++) without the
// struct (151, but adds a count test at entry: 580 bytes), memcpy copy (156),
// local cache pointer (153, spilled: 584 bytes), unit/count initialized in
// other places (167).
// M3 lane 7 r177 (plain, 30 functions, 27 MATCH throughout; aligned counts by sequence, base 66): retail packs from the
// index read before the increment, ((generation << 14 | index) << 4 | unit) << 4 | type. The header's location iterator
// with `long current = iterator.index++;` (as 0x2170e0) and the or-of-shifts packing from `current` 170 (aligned 67): the
// packing becomes retail's chain, but the old index takes ECX (retail EAX) and the accumulator EAX (retail ESI), and the
// table reload stays in the match block; 0x2170e0's handle builder moved to the header with a unit nibble 170 (same code,
// 0x2170e0 still matched), also in the cached loop 170 (same code); packing from `iterator.index` before an increment at
// the loop's end 170 (aligned 70); `type == location.type` 170 (same code).
// (This note's first line read 110, was 156, before round 141's baseline.)
// M3 lane 12 r141 (fast with every function of this file and src/unknown_2157e0.cpp, the callers and callees; that
// image reproduces the plain 114): BLOCKED-BY the `/GL-` flag of src/unknown_215880.cpp. That object is native and
// names g_51ea14 (0x215880 stores it), so the link takes every store through a pointer to modify the global: it is
// reloaded at each use and never carried. Retail carries it in EDX from the entry test to the cached block (reload
// at the pass latch, none after the loop), which the compiler only does for a global no native object names (screen:
// a native object that reads or writes the global, or one `&`, is enough). Diagnostic (not kept, flags are not a
// lane's to change): with that file built as an LTCG object this function MATCHES byte for byte in the plain check
// with the body below (0x215880 then 28 and 0x215b40 5; with 0x215880's table as a real array member 0x215b40
// matches again and 0x215880 is 34: retail calls 0x20670 there and re-reads g_55c15c after its store).
// The body that matches then (52 in the real build, aligned 35 against 66; not kept: without a match its count
// placement, one-use `current` and declaration order are steering forms, and without the count placement it is 170):
// the header's s_saved_game_location_iterator with `do { if (!saved_game_location_iterator_get(&iterator,
// &location)) break; long current = iterator.index++; if (location.type == type) indices[count++] = chain; }
// while (count < *capacity);`, chain = `(((((cache->generation & 0x1ff) << 14) | (current & 0x1fff)) << 4 |
// (unit & 0xf)) << 4) | (type & 0xf)` with the generation read through the getter inside it (a `generation` local
// keeps the old registers); the count declared without a value and zeroed first thing inside the cache block and
// in an else (the compiler hoists the first zeroed long of the block above the entry test and uses its register
// for every zero: retail's is the count's EBX, ours picked the unit's EBP); in the cached block `long index = 0;`
// declared before `cached_count` and `for (; ...)`. Full diff: build/r141/held-215900.patch.
// Scores on the way (real build): cached table read before the include test 113; a typed cache local through the
// function 187 (584 bytes), as the entry local only 184 (588); unit/pass zero placements (for-init, before the
// unit count, with the count, pass first, pass from the count) 114 each; the iterator + `current` + chain with a
// `generation` local 170 (aligned 67), as a while with a break 170, increment at the loop's end 170 (aligned 70),
// a for with the increment in its header 170 (69); generation at its use 170 (aligned 42), with the compare's sides
// swapped 170 (43); on that: cached block with the table before the test, a typed cache local or the getter at each
// use 170/170/171; unit count by if/else 170 (42), preset then if 170 (44), short unit count 163 (35), short pass
// 222 (512 bytes), short unit 215 (520), short count 250 (644), `!= NULL` or the getter in the entry test 170,
// a while with the increment at the end 170, the loop condition's halves swapped 55 (aligned 39; retail tests the
// unit count first); renamed locals 170 each; count zeroed in both branches 52 (35), a second `count = 0;` 52, the
// count inside the block with `*capacity = 0` in an else 200 (596 bytes); on the both-branches form: table before
// the test 51, table local at the block top or entry 186/185 (584 bytes; retail's EDX and latch reload, but from a
// stack home); cached block with a typed cache local, the getter at each use or split declarations 52 each, with
// `long index = 0;` before `cached_count` 52 (the form that matches under the diagnostic).
// Device capacity break in for or while 114/114; built-in bound inside for or while 114/114;
// current index for copy and handle 198, also for the bounds check 151; typed built-in table 114;
// separate bounds in both loops 114; declared location iterator with the original copy loop 114.
// NOT kept: neither current-index form recovers the cache and count registers.
// M3 lane 7 r177 (plain, 30 functions, 27 MATCH throughout; aligned counts by sequence, base 66): retail packs from the
// index read before the increment, ((generation << 14 | index) << 4 | unit) << 4 | type. The header's location iterator
// with `long current = iterator.index++;` (as 0x2170e0) and the or-of-shifts packing from `current` 170 (aligned 67): the
// packing becomes retail's chain, but the old index takes ECX (retail EAX) and the accumulator EAX (retail ESI), and the
// table reload stays in the match block; 0x2170e0's handle builder moved to the header with a unit nibble 170 (same code,
// 0x2170e0 still matched), also in the cached loop 170 (same code); packing from `iterator.index` before an increment at
// the loop's end 170 (aligned 70); `type == location.type` 170 (same code).
// (The first line read 114, was 110, before round 261.)
// M3 lane 3 r261 (fast with 0x2157e0, 0x215880, 0x2163d0, 0x1249f0, 0x13de30 and the five callers, all MATCH;
// control 114; on the current body, not the iterator chain): the unit loop's test as `count < *capacity && pass <
// units` 107 (the entry test then reads the capacity word first; retail tests the unit count first, so the gain is
// elsewhere and under the bar); that with the cached loop's halves swapped as well 139 (576 B); the count declared
// bare and zeroed in the cache block and in an else 140; the unit count by if/else 114 (same code); the cached
// generation through saved_game_storage_get_cache() 138; the entry test through that getter 114 (same code). Our
// build still loads g_51ea14 into EAX at entry with EBP as the zero register, retail into EDX with EBX.
// Shared packed handle type bits 210; named completed disk handle 114, built-in handle 114, both 114;
// copied location declared outside the inner loop 114; built-in record cursor 117;
// no-controller cases outside the switch 157, with a shared controller-selected predicate 186.
// The entry and pass-latch cache registers still differ. The shared type bits grow the frame.
// @retail 0x215900
void __stdcall function_215900(long controller, long type, word *capacity, long *indices, bool include_cached)
{
	long count = 0;
	if (g_51ea14)
	{
		long units = controller == NONE || controller == 0xff ? 1 : 3;
		long unit = 0;
		for (long pass = 0; pass < units && count < *capacity; pass++)
		{
			char drive_letter;
			if (function_1249f0(unit, &drive_letter))
			{
				/* walks the memory unit's list of files */
				struct
				{
					s_saved_game_location_table *table;
					long index;
				} iterator;
				iterator.table = &saved_game_storage_get_cache()->locations + unit;
				iterator.index = 0;
				do
				{
					if (iterator.index >= iterator.table->count)
						break;
					s_saved_game_file_location location = iterator.table->entries[iterator.index];
					iterator.index++;
					if (location.type == type)
					{
						long generation = saved_game_storage_get_cache()->generation;
						indices[count++] = (type & 0xf) | ((unit & 0xf) << 4) |
							(((iterator.index - 1) & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
					}
				} while (count < *capacity);
			}
			switch (controller)
			{
			case 0: unit = pass + 1; break;
			case 1: unit = pass + 3; break;
			case 2: unit = pass + 5; break;
			case 3: unit = pass + 7; break;
			case NONE:
			case 0xff: unit++; break;
			default: __assume(0); break;
			}
		}
		if (include_cached)
		{
			s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
			long cached_count = files->count;
			for (long index = 0; count < *capacity && index < cached_count; index++)
			{
				if (files->entries[index].type == type)
				{
					long generation = *(long *)((byte *)files + 0x4befc);
					indices[count++] = (type & 0xf) | ((index & 0x1fff) << 8) |
						(1 << 21) | ((generation & 0x1ff) << 22);
				}
			}
			sort_4byte(&cached_count, indices, count, function_2163d0, NULL);
		}
	}
	*capacity = (word)count;
}

bool function_2161d0(long file_index, void *buffer, long size);
bool function_19d650(s_game_variant *variant);

// @retail 0x212bc0
bool function_212bc0(long file_index, s_game_variant *variant)
{
	bool result = false;
	if (file_index != NONE && function_2161d0(file_index, variant, sizeof(*variant)))
	{
		union
		{
			s_game_variant value;
			unsigned __int64 words[0x130 / 8];
		} copy;
		copy.value = *variant;
		result = function_19d650(&copy.value);
	}
	return result;
}

// @retail 0x212c20
bool __stdcall function_212c20(word const *name, s_game_variant *variant, long *file_index)
{
	bool result = false;
	if (file_index)
		*file_index = NONE;
	if (name[0])
	{
		for (long type = 1; type <= 9 && !result; type++)
		{
			word candidate[0x80];
			long indices[0x1000];
			long capacity = 0x1000;
			function_215900(0xff, type, (word *)&capacity, indices, true);
			for (long index = 0; index < (word)capacity; index++)
			{
				word const *candidate_name = function_215b50(indices[index], candidate);
				if (!_wcsicmp(candidate_name, name))
				{
					result = true;
					if (variant)
						result = function_212bc0(indices[index], variant);
					if (result && file_index)
						*file_index = indices[index];
					break;
				}
			}
		}
	}
	else if (variant)
	{
		variant_defaults_initialize(variant, 0);
		result = true;
	}
	return result;
}


struct s_variant_preset_212f40 { word const *name; long index; long type; };
PRIVATE s_variant_preset_212f40 const g_445958[28] =
{
    { L"Slayer Duel", 0, 0 },
    { L"Rumble Slayer", 1, 0 },
    { L"Team Slayer", 2, 0 },
    { L"Rumble Rockets", 3, 0 },
    { L"Team Rockets", 4, 0 },
    { L"Rumble Shotguns", 5, 0 },
    { L"Pistols", 6, 0 },
    { L"Rifle Duel", 7, 0 },
    { L"Brutes", 8, 0 },
    { L"Sword Duel", 9, 0 },
    { L"Snipers", 10, 0 },
    { L"Elimination", 11, 0 },
    { L"Multi Flag CTF", 12, 4 },
    { L"Classic CTF", 13, 4 },
    { L"Shotgun CTF", 14, 4 },
    { L"1 Flag CTF", 15, 4 },
    { L"1 Flag CTF Fast", 16, 4 },
    { L"Multi Bomb", 17, 5 },
    { L"Assault", 18, 5 },
    { L"Rapid Assault", 19, 5 },
    { L"Major Assault", 20, 5 },
    { L"Minor Assault", 21, 5 },
    { L"Crazy King", 22, 3 },
    { L"Team Crazy King", 23, 3 },
    { L"Oddball", 24, 1 },
    { L"Teamball", 25, 1 },
    { L"Three Plots", 26, 6 },
    { L"Land Grab", 27, 6 },
};

void function_0bfe20(dword *flags, long bit, bool value);

// @retail 0x212f40
void function_212f40(long preset, s_game_variant *variant)
{
    s_variant_preset_212f40 const *entry = &g_445958[preset];
    word const *source = entry->name;
    function_19d220(variant, entry->type);
    wchar_t *name = variant->name;
    wcsncpy(name, source, 31);
    name[31] = 0;
    variant->unknown78 = 16;
    variant->unknown74 = 16;
    switch (preset)
    {
    case 0: variant->unknown50 = 10; break;
    case 2: function_0bfe20(&variant->flags48, 0, true); break;
    case 7:
        variant->unknown50 = 10; variant->unknownd4 = 11; variant->unknownd6 = 13; variant->unknownd7 = 4;
        variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        break;
    case 3:
    case 4:
        function_0bfe20(&variant->flags48, 0, preset == 4);
        variant->unknown54 = 360;
        variant->unknownd4 = 2;
        function_0bfe20(&variant->flags48, 1, false);
        variant->unknown84 = 0;
        variant->unknownd6 = 8;
        variant->unknownd7 = 1;
        break;
    case 5:
        variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        variant->unknown54 = 360; variant->unknownd4 = 3; variant->unknownd6 = 9; variant->unknownd7 = 1;
        variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 9:
        variant->unknownd4 = 4; variant->unknown50 = 10; variant->unknownd6 = 17; variant->unknownd7 = 1;
        variant->unknown88 = 2; variant->unknowncd = 7; variant->unknownce = 7; variant->unknowncf = 4;
        variant->unknownd0 = 4; variant->unknownd1 = 4; variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 6: variant->unknownd4 = 14; variant->unknownd6 = 4; variant->unknownd7 = 1; break;
    case 8: variant->unknownd4 = 5; variant->unknownd6 = 11; variant->unknownd7 = 1; break;
    case 10:
        variant->unknownd4 = 12; function_0bfe20(&variant->flags48, 1, false); variant->unknownd7 = 4; variant->unknownd6 = 10;
        variant->unknowncd = 7; variant->unknownce = 7; variant->unknowncf = 4; variant->unknownd0 = 4;
        variant->unknownd1 = 4; variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 11: variant->unknown50 = 0; variant->unknown4c = 5; variant->unknown7c = 1; break;
    case 1: variant->unknown54 = 360; break;
    case 24: variant->unknown54 = 360; break;
    case 25: function_0bfe20(&variant->flags48, 0, true); variant->unknowna8 = 0; variant->unknown80 = 10; break;
    case 22: variant->unknowna8 = 0; variant->unknown54 = 360; break;
    case 23:
        function_0bfe20(&variant->flags48, 0, true); variant->unknowna8 = 0; variant->unknown80 = 10;
        variant->engine_flags |= 1; variant->unknown50 = 60;
        break;
    case 13: variant->engine_flags |= 12; break;
    case 14:
        variant->engine_flags |= 12; variant->unknownd4 = 3; variant->unknownd6 = 9; variant->unknownd7 = 1;
        variant->unknowncf = 4; variant->unknownd1 = 4;
        break;
    case 15:
        variant->unknown4c = 2; variant->unknown54 = 180; variant->unknown50 = 1; variant->flag.unknown104 = 1;
        break;
    case 16:
        variant->unknown4c = 2; variant->unknown54 = 120; variant->unknown80 = 5; variant->unknown50 = 1;
        variant->unknown84 = 5; variant->flag.unknown104 = 1; variant->flag.unknownf4 = 15;
        break;
    case 18:
        variant->unknown4c = 2; variant->unknown50 = 1; variant->unknown54 = 180; variant->flag.unknown104 = 1;
        break;
    case 19:
        variant->unknown4c = 2; variant->unknown80 = 5; variant->unknown54 = 120; variant->unknown50 = 1;
        variant->unknown84 = 5; variant->flag.unknown104 = 1; variant->flag.unknownf4 = 20;
        break;
    case 21: variant->flag.unknown104 = 2; variant->flag.unknown108 = 10; break;
    case 12: variant->unknown54 = 720; break;
    case 17: variant->unknown54 = 720; break;
    case 20: variant->flag.unknown104 = 2; variant->flag.unknown108 = 10; variant->unknown54 = 720; break;
    case 26:
        function_0bfe20(&variant->flags48, 0, true); variant->unknowna8 = 0; variant->unknown84 = 5;
        variant->territories.unknownf2 = 3; variant->unknown50 = 180;
        break;
    case 27:
        function_0bfe20(&variant->flags48, 0, true); variant->unknown54 = 720; variant->territories.unknownf0 = 5; variant->unknowna8 = 0;
        break;
    }
}


#include <xtl.h>

extern bool g_55c14c;
void function_125d60(void);
void function_120d50(bool volatile *done, bool idle);

// @retail 0x2160b0
bool function_2160b0(long file_index, char *path)
{
    bool result = false;
    if (!((bool)(((dword)file_index >> 21) & 1)))
    {
        struct
        {
            char path[0x14];
            word name[0x16];
        } location;
        location.path[0] = 0;
        location.name[0] = 0;
        function_120d50(&g_55c14c, true);
        if (function_216800(file_index, &location))
        {
            strncpy(path, location.path, 0x100);
            path[0xff] = 0;
            result = true;
        }
    }
    return result;
}



char *function_216400(long memory_unit, char *buffer, short capacity);
bool function_216da0(long type, wchar_t *name, wchar_t const *display_name, long language);

// score 2 (was 11): only the argument setup before 0x216da0 differs: retail masks the type (`and ecx, 0xf`) before
// pushing display_name, ours after. Earlier: argument-setup scheduling and call-layout differences. Permuter:
// 292 tries in 600 s, best 4 only by swapping the name/display_name arguments
// at the matched 0x216da0 and reading language through a volatile char, both
// rejected; the honest form stays at 11.
// M3 lane 8 r182 (fast): kept: the name buffer as s_saved_game_name (in local_0, as before) filled through its fill(),
// as 0x215c00, 0x215f40 and 0x216120 do, so its address is formed again after 0x216da0 (11 -> 2). Tried: the same
// through a `s_saved_game_name &` bound to local_0.name 11; the type in a named local beside unit or just before the
// call 2/2; fill's display_name and language read through local_0 instead of the location reference 2.
// @retail 0x215e60
void __stdcall function_215e60(long file_index)
{
    if (file_index != NONE)
    {
        function_120d50(&g_55c14c, true);
        long unit = (file_index >> 4) & 15;
        struct
        {
            s_saved_game_file_location location;
            byte unknown40[4];
            s_saved_game_name name;
        } local_0;
        s_saved_game_file_location &location = local_0.location;
        location.name[0] = 0;
        location.display_name[0] = 0;
        if (function_216800(file_index, &location))
        {
            if (!((bool)(((dword)file_index >> 21) & 1)))
            {
                char root[8] = "";
                char drive;
                if (function_1249f0(unit, &drive))
                {
                    local_0.name.text[0] = 0;
                    local_0.name.fill(file_index & 15, location.display_name, location.language);
                    XDeleteSaveGame(function_216400(unit, root, 8), local_0.name.text);
                }
            }
            saved_game_storage_remove_location(file_index);
        }
    }
}


#include "files.h"

extern long g_55c154;
long function_217590(long memory_unit);
bool function_216d70(const char *directory, char *path, long type);
s_type_acf665 *file_ref_create(s_type_acf665 *file, bool replace, const char *name);
bool saved_game_file_write_save_image(long type, s_saved_game_file_location *location);
long saved_game_file_type_size_in_blocks(long type);
bool saved_game_storage_has_free_blocks(long blocks);

// @retail 0x215c00
long function_215c00(long type, wchar_t const *display_name)
{
    long result = NONE;
    function_120d50(&g_55c14c, true);
    if (g_51ea14)
    {
        long count = function_217590(0);
        if ((!g_51ea14 || *(long *)((byte *)g_51ea14 + 0xbef8) < 0x1000) && count < 0x1000)
        {
            long index = g_55c280;
            s_saved_game_name name;
            name.text[0] = 0;
            if (name.fill(type, display_name, index))
            {
                char directory[256] = "";
                char root[8];
                function_216400(0, root, 8);
                if (!XCreateSaveGame(root, name.text, 1, 0, directory, sizeof(directory)))
                {
                    struct
                    {
                        s_saved_game_file_location location;
                        byte unknown40[4];
                    } local_0;
                    s_saved_game_file_location &location = local_0.location;
                    memset(&location, 0, sizeof(location));
                    char path[256];
                    path[0] = 0;
                    wcsncpy(location.display_name, display_name, 16);
                    location.display_name[16] = 0;
                    location.type = type;
                    location.language = (char)index;
                    strncpy(location.name, directory, sizeof(location.name));
                    location.name[sizeof(location.name) - 1] = 0;
                    function_216d70(location.name, path, type);
                    struct
                    {
                        dword signature;
                        word flags;
                        short location;
                        char path[256];
                        __int64 state;
                    } file;
                    if (file_ref_create((s_type_acf665 *)&file, false, path) &&
                        function_1367d0((s_type_acf665 *)&file) &&
                        saved_game_file_write_save_image(type, (s_saved_game_file_location *)directory) &&
                        function_216990(0, (c_location_record const *)&location, &index))
                    {
                        long generation = *(long *)((byte *)g_51ea14 + 0x4befc);
                        result = (((generation & 0x1ff) << 14 | (index & 0x1fff)) << 8) | (type & 15);
                    }
                    else
                    {
                        s_saved_game_name remove_name;
                        remove_name.text[0] = 0;
                        remove_name.fill(type, display_name, location.language);
                        XDeleteSaveGame(root, remove_name.text);
                    }
                }
                if (result == NONE)
                    g_55c154 = saved_game_storage_has_free_blocks(saved_game_file_type_size_in_blocks(type)) ? 4 : 2;
                else
                    g_55c154 = 0;
            }
        }
        else
        {
            g_55c154 = 1;
        }
    }
    return result;
}


struct s_saved_game_read;
struct s_saved_game_file_task;
char *function_122810(char *path, const char *suffix);
bool signed_file_read_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task);
bool signed_file_write_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task);

// @retail 0x2174b0
bool __stdcall function_2174b0(long file_index, void *header, long header_size, void *body, long body_size, s_saved_game_read *task)
{
    bool result = false;
    char path[256];
    path[0] = 0;
    if (function_2160b0(file_index, path))
    {
        function_122810(path, "auxilary.bin");
        result = signed_file_read_begin(path, header, header_size, body, body_size, false, (s_saved_game_file_task *)task);
    }
    return result;
}

// @retail 0x217520
bool __stdcall function_217520(long file_index, void const *header, long header_size, void const *body, long body_size, s_saved_game_read *task)
{
    bool result = false;
    char path[256];
    path[0] = 0;
    if (function_2160b0(file_index, path))
    {
        function_122810(path, "auxilary.bin");
        result = signed_file_write_begin(path, (void *)header, header_size, (void *)body, body_size, false, (s_saved_game_file_task *)task);
    }
    return result;
}

bool saved_game_file_read_synchronous(long file_index, void *buffer, long size, wchar_t *display_name);

// @retail 0x212ec0
bool function_212ec0(s_game_variant *arg_0, long arg_1)
{
    long const *local_0 = &arg_1;
    bool local_1 = true;
    if (*local_0 != NONE)
    {
        function_19d650(arg_0);
        union
        {
            s_game_variant field_0;
            unsigned __int64 field_8[0x130 / 8];
        } local_2;
        local_2.field_0 = *arg_0;
        local_1 = saved_game_file_read_synchronous(*local_0, &local_2.field_0, sizeof(local_2), arg_0->name);
    }
    return local_1;
}
