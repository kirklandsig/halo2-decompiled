#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19c1d0.h"
#include "data_array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// @flags /O2 /Gr

struct s_entry_b
{
	byte unknown00[4];
	long key;
	byte unknown08[0xb50 - 8];
};

struct s_entry_c
{
	long key;
	byte unknown04[0xc50 - 4];
	byte flags;
	byte unknownc51[0xc64 - 0xc51];
};

struct s_table_b
{
	long unknown00;
	long unknown04;
	long count;
	s_entry_b *data;
};

struct s_table_a
{
	long count;
	s_entry_a *data;
};

struct s_table_c
{
	long unknown00[4];
	long count;
	s_entry_c *data;
};

static s_table_b *get_table_b()
{
	s_tag_header_globals *globals = g_4e034c;
	s_table_b *table;
	if (globals->b_valid)
		table = globals->b;
	else
		table = 0;
	return table;
}

static s_entry_b *get_entry_b(s_table_b *table, long index)
{
	s_entry_b *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

static s_table_a *get_table_a()
{
	s_table_a *table = 0;
	if (g_4e0350 && g_4e034c)
	{
		s_tag_header_globals *globals = g_4e034c;
		if (globals->a_valid)
			table = globals->a;
		else
			table = 0;
	}
	return table;
}

static s_entry_a *get_entry_a(s_table_a *table, long index)
{
	s_entry_a *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

static s_table_c *get_table_c()
{
	s_tag_header_globals *globals = g_4e034c;
	s_table_c *table;
	if (globals->b_valid)
		table = (s_table_c *)globals->b;
	else
		table = 0;
	return table;
}

static s_entry_c *get_entry_c(s_table_c *table, long index)
{
	s_entry_c *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

// @retail 0x19c1d0
void function_19c1d0()
{
	g_4ee4e8->valid = 0;
	g_4ee4e4->valid = 0;
}

// @retail 0x19c1f0
s_entry_b *function_19c1f0(long key)
{
	s_table_b *table = get_table_b();
	s_entry_b *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_b *entry = get_entry_b(get_table_b(), i);
			if (entry->key == key)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c270
s_entry_a *function_19c270(long key0, long key1)
{
	s_table_a *table = get_table_a();
	s_entry_a *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key1 == key1 && entry->key0 == key0)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c320
s_entry_a *function_19c320(const char *name)
{
	s_table_a *table = get_table_a();
	s_entry_a *result = 0;
	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			const char *a = strrchr(name, 0x5c);
			a = a ? a + 1 : name;
			const char *b = strrchr(entry->name, 0x5c);
			b = b ? b + 1 : entry->name;
			if (!strcmp(b, a))
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c440
long function_19c440(long key0, long key1)
{
	s_table_a *table = get_table_a();
	long result = NONE;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key0 == key0 && entry->key1 > key1)
			{
				if (result == NONE || entry->key1 < result)
					result = entry->key1;
			}
		}
	}
	return result;
}

// @retail 0x19c4e0
long function_19c4e0(long key0)
{
	s_table_a *table = get_table_a();
	long result = NONE;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key0 == key0 && entry->key1 != NONE && result <= entry->key1)
				result = entry->key1;
		}
	}
	return result;
}

// @retail 0x19c580
long function_19c580()
{
	long result = NONE;
	s_table_c *table = get_table_c();
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);
			long key = entry->key;
			if (key != NONE && (result == NONE || key < result))
				result = key;
		}
	}
	return result;
}

// @retail 0x19c5f0
s_entry_c *function_19c5f0(long key)
{
	s_table_c *table = get_table_c();
	s_entry_c *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);
			if (entry->key == key)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c670
s_record_pool *function_19c670()
{
	s_table_b *table = get_table_b();
	s_record_pool *result = 0;
	if (table)
		result = g_4ee4e8;
	return result;
}

// @retail 0x19c6a0
s_record_pool *function_19c6a0()
{
	s_table_b *table = get_table_b();
	s_record_pool *result = 0;
	if (table)
		result = g_4ee4e4;
	return result;
}

// @retail 0x19c7c0
bool __stdcall function_19c7c0(long a, long b, const void *context)
{
	bool result = a > b;
	return result;
}

// @retail 0x19c7d0
long __stdcall function_19c7d0(long a, long b, const void *context)
{
	return a - b;
}

// @retail 0x19c970
const char *function_19c970(long campaign_id, long map_id)
{
	const char *path = 0;
	if (campaign_id == NONE)
	{
		s_entry_c *level = function_19c5f0(map_id);
		if (level)
			path = (const char *)level + 0xb4c;
	}
	else
	{
		s_entry_a *level = function_19c270(campaign_id, map_id);
		if (level)
			path = level->name;
	}
	return path;
}

// @retail 0x19c9a0
int __cdecl function_19c9a0(const void *a, const void *b)
{
	s_entry_c *x = function_19c5f0(*(const long *)a);
	s_entry_c *y = function_19c5f0(*(const long *)b);
	if (*(long *)((byte *)x + 0xc4c) > *(long *)((byte *)y + 0xc4c))
		return 1;
	return *(long *)((byte *)x + 0xc4c) < *(long *)((byte *)y + 0xc4c) ? -1 : 0;
}

/* the sort routines (0x13dcd0); sort_4byte's first parameter
   is never read */
typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
typedef long (__stdcall *t_search_4byte_compare_function)(long, long, const void *);
void sort_4byte(void *unused, long *elements, unsigned long count, t_sort_4byte_compare_function compare, const void *context);
long binary_search_4byte(long key, const long *base, long count, t_search_4byte_compare_function compare, const long *context);

/* the elements of the data arrays these functions fill (8 bytes) */
struct s_level_datum
{
	short identifier;
	bool flag;
	byte unknown03;
	long value;
};

static inline s_level_datum *level_datum_get(s_record_pool *data, long index)
{
	return (s_level_datum *)(data->data + index * sizeof(s_level_datum));
}

// @retail 0x19c6d0
void function_19c6d0(s_record_pool *data, long key0)
{
	long values[20];
	s_table_a *table = get_table_a();
	long count = 0;

	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);

			if (entry->key0 == key0 && entry->key1 != NONE)
				values[count++] = entry->key1;
		}
	}

	sort_4byte(&key0, values, count, function_19c7c0, 0);
	record_pool_release_all(data);
	for (long i = 0; i < count; i++)
	{
		long datum_index = record_pool_allocate(data);

		if (datum_index != NONE)
			level_datum_get(data, datum_index & 0xffff)->value = values[i];
	}
}

// score 65 (was 108): GB r28 kept a two-field counted stack `s_flagged_keys` (`long count; long elements[50];`) with
// `long index = flagged.count; flagged.count = index + 1; flagged.elements[index] = entry->key;` and the callers'
// `long temporary;` for `sort_4byte` (108 -> 65, +0x91, 400/393 B, sub esp 0x198, ret 4, all peers including 0x19c190
// and 0x19c6d0 MATCH). The struct places `flagged.count` at [esp+0x14] directly ahead of `flagged.elements` at
// [esp+0x18] (below `keys` at [esp+0xe0]), prevents caching `flagged.count` across `sort_4byte`/`binary_search_4byte`
// (freeing ESI in Loop 3), and borrows EBX in Loop 1 with a `[esp+0x1ac]` reload of `data` so `data` wins EBX from
// entry while staying a stack parameter (`ret 4`). Remaining diffs: 4-instruction order of `entry->key` load vs
// `flagged.count++`, `ecx`/`edx` swap at `qsort`/Loop 2/`sort_4byte`, and Loop 3 hoisting `data->high_water_index`
// into EDI where retail reloads `[ebx+0x38]` inside the inner bitmap loop.
// Earlier at 108 (was 119): retail keeps the pool parameter in EBX from entry (the allocator's first pass; ours
// colours it only in its second pass, from the release call on, after its first-pass priority ends at -40 with -72 of
// live-not-referenced blocks across the first loop), borrows EBX for the flagged count (kept in memory, so a zero
// register serves the zero store and tests), passes sort_4byte a separate scratch slot shared with the spilled valid
// flag, and places the flagged keys below the sorted keys in the frame. M3 lane 13 r160 (fast; control 108): the
// flagged keys renamed to sort after the sorted keys 108 (same code: the frame order does not follow the names); the
// walk's iterator declared and set up at the top 123 (frame 0x1a4: the iterator goes to memory); a `long scratch;`
// declared at the sort_4byte call and passed in place of the count's address 149 (375 B); the two array stores split
// from their increments 108 (same code); the counts declared ahead of the table lookup 156 (371 B). All restored.
// M3 lane 6 r247 (fast): the callers' idiom for sort_4byte's unused first argument, an uninitialised `long
// temporary;` in its own block, 149 (375 B: frame 0x194; the flagged count is then spilled as in retail, but the
// hoisted valid flag takes EBX), the same declared bare at the call or at the top 149, with the counts declared ahead
// of the table lookup 157 (370 B); the two arrays declared the other way round 108 (same code; with the scratch
// 149). The allocator ranks the pool's first-pass candidate at -40 under the flagged count's -30 (with the scratch
// argument: -43 under the valid flag's -35); a forced raise to -34 changes nothing.
// GB r25 (fast with 9 peers MATCH, 18 fresh trials): data_find_index walk 200 (with temporary 188); direct
// data_next_absolute_index_inlined walk 125 (with temporary 146); shared loop index i 108 (with temporary 149, al=36;
// &i as sort_4byte arg 135, 393 B); long temporary = 0 at top 131 (393/393 B, frame 0x198, al=43: b_valid still takes
// EBX and flagged_count moves into EBX after loop 1); while loop 1/2 with temporary 149; data_iterator_new inline 149;
// iterator declared at top initialized late 149; interleaved flagged/keys 157; __stdcall with temporary 149; indexed
// datum store 149; for(table && i<table->count) / if(table=get_table_c()) 157; dword flagged 149. Body unchanged.
// M3 lane 1 r173 (fast with 0x19c6d0, 0x19c190; control 108): on the callers' `long temporary;` form (re-measured 149,
// 375 B) the allocator ranks the hoisted valid flag -35 and the globals pointer -36 above the pool -43; a forced-ranking
// search putting the pool at -34 gives retail's frame (0x198) and a shorter instruction distance but 152, because the pool
// then becomes a register parameter (no entry load from the stack, `ret` without 4) where retail keeps the stack parameter
// and loads it into EBX at entry. The two arrays have equal frame density (3 references each); retail's slot order needs
// its scratch slot at least as dense as the flagged count's. Forms: on the temporary form, the walk's datum declared in the
// while condition 149, the flagged count `unsigned long` 149, the entry key read once into a named local 152 (371 B); on
// this body the flagged count `unsigned long` 108 (same code), the named entry key 134.
// M3 lane GA r25 (25 fast trials with 0x19c6d0/0x19c190/0x19c5f0/0x19c580/0x19c7c0/0x19c7d0/0x19c9a0/0x19c120 MATCH unless
// noted; control 108 at 393/393 B, +0x11, al 43, mk 27; body unchanged): s_data_datum_iterator-style walk struct 108 (with
// temporary 149); sort_4byte(&datum_index, ...) 133 (393 B); sort_4byte(&data, ...) 142 (406 B, keeps ret 4 and 0x19c190
// MATCH but reloads data in loops 2-3); direct/ternary get_table_c 154/132 (breaks 0x19c5f0/0x19c580; direct+temp 153);
// PRIVATE linkage 108 (+temp 149); for(;;) break walk+temp 149; for(datum=next;datum;datum=next) walk 226 (476 B: hoists
// data at entry and xor edx,edx as retail, but changes data to register arg EAX and breaks 0x19c190 to 6); if(table &&
// table->count > 0) 108; entry->flags bitfield 108; direct datum store 108; 2x50 array frame order+temp 137 (383 B);
// unsigned long count 108 (breaks 0x19c6d0 to 2); data_find_index direct/in iterator helper 109/108 (+temp 149/149);
// drop dead iterator.datum_index init 108 (+temp 149); out-of-line record_pool_iterator_step 151 (368 B); if(table)+temp
// +find 157 (370 B); diagnostic high_water_index reload+temp 177 (348 B: promotes data to incoming EBX, breaking 0x19c190).
// GB r28 (58 fast trials with 8 peers MATCH unless noted; control 108 at 393/393 B, +0x11, al=43, mk=27; kept 65 plain-confirmed):
// `long temporary;` alone 149 (375 B), + early-return / while / direct `iterator->data` bitmap helpers 149/151/151, + scoped `keys` 147 (148 without temp), + table-first 149, + `continue` 149, + split `flagged_count` inc 137 (393 B, frame 0x198, ret 4, 0x19c190 MATCH; without temp 125), + Loop 2 `data->size` 136 (383 B, ret, breaks 0x19c190; with split inc 111 at 417 B, +0x33, ret 4, 0x19c190 MATCH), + `datum_get_absolute_inlined` 186, + `for` iter 230, + `data_iterator_next_calling` 153, + keys after flagged 135;
// counted stack `s_flagged_keys` + `long temporary;`: post-inc `flagged.elements[flagged.count++]` 153 (methods / flagged-first / table-first 153, without temp 154, pre-inc 152, Loop 2 `data->size` 142 breaking 0x19c190); kept split `long index = flagged.count; flagged.count = index + 1; flagged.elements[index] = entry->key;` **65** (400/393 B, +0x91, al=50, mk=9, all peers MATCH; `push_index()` / `push_ptr()` / `push_ref()` / `push_cref()` 65; 3-use `long key` 97 at 393 B); value-taking `s_flagged_keys::push(long)` (one-caller method, not kept) **51** (400/393 B, +0x11d: first 285 B through `sort_4byte` + epilogue + search tail byte-exact, al=39, mk=5; `data_find_index` / double-guard / inline-Loop-3 / `s_data_datum_iterator` / early-ret / `bitmap` local 50-51; `iterator->index` in loop 145; `while &&` 138; `for (index>=0 && ...)` 131; `>>` shift test 79); diagnostic non-hoisted `high_water_index` read on `s_flagged_keys::push` **0** (MATCH, all 9 match; not kept: `volatile` forbidden).
// M3 lane GA r30 (18 fast trials across 2 batches with 8 matched peers MATCH + 32 standalone CL.Exe/LTCG probes; control
// 65 at 400/393 B, +0x91; body unchanged): clean push-order variants without one-caller method: `flagged.count++` 65,
// `long *slot` 66, `flagged.elements[flagged.count] = entry->key; flagged.count++` / `++flagged.count` / `+= 1` 153
// (370 B), `index` store-then-inc 97 (393 B), 3-use `key` + store-then-inc 154, 2-use `key` inside `if` 97, both stacks
// as `s_flagged_keys` 198 (454 B); on GB28's `s_flagged_keys::push(long)` (51): `iterator` at top / `pool_iter_new` /
// `found` local 51, `bitmap` local 50, `while &&` / flat iter / `iterator->index` update 82 (outlined, 368 B), `index+1`
// latch 170 (350 B, breaks 0x19c190), bool out-param 58 (405 B). Standalone CL.Exe/LTCG probes confirmed all 14 flag
// sets (`/O2`, `/O1`, `/Ox`, `/Os`, `/Oa`, `/Ow`, `/G5`-`/G7`, `/Op`, `/Oy-`, `/arch:SSE`, `/Ob1`), cross-TU LTCG
// (`/O2`, `/O1`) and 18 non-volatile iterator/loop forms hoist `data->high_water_index` once `iterator` is scalar-promoted.
struct s_flagged_keys
{
	long count;
	long elements[50];
};

// @retail 0x19c7e0
void function_19c7e0(s_record_pool *data)
{
	long keys[50];
	s_flagged_keys flagged;
	s_table_c *table = get_table_c();
	long count = 0;
	flagged.count = 0;

	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);

			if (entry->key != NONE)
			{
				keys[count++] = entry->key;
				if (entry->flags & 1)
				{
					long index = flagged.count;
					flagged.count = index + 1;
					flagged.elements[index] = entry->key;
				}
			}
		}
	}

	qsort(keys, count, sizeof(long), function_19c9a0);
	record_pool_release_all(data);
	for (long i = 0; i < count; i++)
	{
		long datum_index = record_pool_allocate(data);

		if (datum_index != NONE)
			level_datum_get(data, datum_index & 0xffff)->value = keys[i];
	}

	{
		long temporary;
		sort_4byte(&temporary, flagged.elements, flagged.count, function_19c7c0, 0);
	}
	s_record_pool_iterator iterator;
	s_level_datum *datum;

	iterator.data = data;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((datum = (s_level_datum *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (binary_search_4byte(datum->value, flagged.elements, flagged.count, function_19c7d0, 0) != NONE)
			datum->flag = true;
	}
}

/* a path of up to 259 characters */
struct s_level_path
{
	char string[0x104];
};

// @retail 0x19c9e0
char *level_path_print(s_level_path *path, char const *format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	_vsnprintf(path->string, sizeof(path->string) - 1, format, arguments);
	path->string[sizeof(path->string) - 1] = 0;

	return path->string;
}

/* retail inlines function_16b790 here (unknown_16b570.cpp is /Ob1) */
static inline void data_make_valid_inlined(s_record_pool *data)
{
	data->valid = 1;
	record_pool_release_all(data);
}

/* the scenario's type at +0x10 */
struct s_level_scenario_view
{
	byte unknown00[0x10];
	short type;
};

/* a found content package: its directory and display name (online_feedback.cpp) */
struct s_content_item
{
	char directory[0x104];
	word display_name[0x80];
};

bool content_find_first(s_content_item *item, void **find_handle);
bool content_find_next(void *find_handle, s_content_item *item);
bool __stdcall function_19bfd0(s_content_item *item);

extern bool g_54e7f8;

/* reads the map files the disc holds; remembers whether one failed */
// @retail 0x19c120
void function_19c120(void)
{
	g_54e7f8 = false;
	if (((s_level_scenario_view *)g_4e0350)->type == 2)
	{
		void *handle;
		s_content_item item;

		item.directory[0] = 0;
		item.display_name[0] = 0;
		if (content_find_first(&item, &handle) != 0)
		{
			do
			{
				if (!function_19bfd0(&item))
					*(volatile bool *)&g_54e7f8 = true;
			}
			while (content_find_next((void *)handle, &item));
		}
	}
}

/* rebuilds both level lists */
// @retail 0x19c190
void function_19c190(void)
{
	data_make_valid_inlined(g_4ee4e8);
	data_make_valid_inlined(g_4ee4e4);
	function_19c120();
	function_19c6d0(g_4ee4e8, 1);
	function_19c7e0(g_4ee4e4);
}
