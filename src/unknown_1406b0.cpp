// @flags /O2 /Gr
#include "font_loading_storage.h"
#include "glyphs_storage.h"
/* UNKNOWN_1406B0.CPP: the font character cache. Characters are looked up by
   (font file, character) in a hash table, loaded asynchronously from the font
   cache file in three steps (the offset of the character's header, the
   header, then its pixels) and their pixels are kept in a physical memory
   allocator over a 0x7800 byte buffer. */

#include "unknown_11c920.h"
#include "unknown_13eeb0.h"
#include "data_array.h"
#include "data_iterator.h"
#include "physical_memory.h"
#include "job_queue.h"
#include "font_loading.h"
#include "async.h"
#include "unknown_120d80.h"
#include <xtl.h>
#include <string.h>

/* the hash table (src/unknown_13e210.cpp) */
struct hash_node
{
	void *key;
	dword hash;
	hash_node *next;
	byte data[1];
};

struct hash_table
{
	byte unknown00[0x20];
	dword bucket_count;
	long maximum_count;
	long data_size;
	dword (__stdcall *hash_proc)(const void *key);
	bool (__stdcall *compare_proc)(const void *key_a, const void *key_b);
	c_data_allocator *allocator;
	hash_node *free_list;
	hash_node *buckets[1];
};

__declspec(noinline) hash_table *function_13e1a0(const char *name, long data_size, long bucket_count,
	dword (__stdcall *hash_proc)(const void *key), bool (__stdcall *compare_proc)(const void *key_a, const void *key_b),
	long maximum_count, c_data_allocator *allocator);
bool function_13e270(hash_table *table, void *key, const void *data);
hash_node *function_13e2d0(hash_table *table, void *key);
bool hash_table_remove(hash_table *table, void *key);

/* the font files (font_loading.cpp) and their asynchronous reads */
bool function_120ce0(long job, long priority);
long function_120bf0(void);
void global_preferences_flush(void);
long function_122610(long size, void const *pixels, void *destination);

extern c_data_allocator *g_468758;


/* the header of a character in the font cache file */
struct s_font_character_header
{
	word unknown00;
	word pixels_size;
	short width;
	short height;
	byte unknown08[4];
	dword pixels_offset;
};

struct s_font_character_key
{
	long font_index;
	long character;
};

/* a character (0x38 bytes) */
struct s_font_character
{
	short salt;
	word unknown02;
	s_font_character_key key;
	bool volatile header_offset_ready;
	bool volatile header_ready;
	byte unknown0e[2];
	long state;
	long task;
	long header_offset;
	s_font_character_header header;
	long pixels_index;
	long last_used_frame;
	long render_index;
};

/* a predicted character (0xc bytes) */
struct s_font_prediction
{
	short salt;
	word unknown02;
	long font_index;
	long character;
};

/* the pixels of a character (0xc bytes); the datum index is the physical
   memory block's */
struct s_font_pixels
{
	short salt;
	bool volatile ready;
	byte unknown03;
	long character_index;
	long task;
};

#include "font_render_cache.h"

enum
{
	_font_character_state_none,
	_font_character_state_reading_header_offset,
	_font_character_state_header_offset_ready,
	_font_character_state_reading_header,
	_font_character_state_ready,
};

hash_table *g_54d570;
s_record_pool *g_54d574;
s_record_pool *g_54d578;
s_record_pool *g_54d57c;
s_physical_object *g_54d580;
byte *g_54d584;
long g_54d588;


#define FONT_CHARACTER(datum_index) (&((s_font_character *)g_54d574->data)[(datum_index) & 0xffff])
#define FONT_PIXELS(datum_index) (&((s_font_pixels *)g_54d57c->data)[(datum_index) & 0xffff])
#define PHYSICAL_BLOCK(datum_index) (&((s_physical_block *)g_54d580->blocks->data)[(datum_index) & 0xffff])

static inline s_font_character_header *font_cache_character_get_header(long datum_index)
{
	s_font_character *character = FONT_CHARACTER(datum_index);
	s_font_character_header *header = NULL;

	if (character->state == _font_character_state_ready)
	{
		header = &character->header;
	}
	return header;
}

static __forceinline void async_wait(bool volatile *done)
{
	if (!*done)
	{
		while (!*done)
		{
			SwitchToThread();
		}
	}
}

/* function_16b790, which retail inlines here */
static inline void data_make_valid_inlined(s_record_pool *data)
{
	data->valid = true;
	record_pool_release_all(data);
}

static inline bool hash_table_find_data(hash_table *table, void *key, void *data)
{
	hash_node *node = function_13e2d0(table, key);

	if (node && NULL != data)
	{
		memcpy(data, node->data, table->data_size);
		return true;
	}
	return false;
}

/* the flags of a character lookup */
struct s_font_cache_flags
{
	dword wait : 1;
	dword create : 1;
};

long font_cache_get_character(long font_index, long character, s_font_cache_flags flags);
bool font_cache_character_load_pixels(long datum_index, s_font_cache_flags flags);
PRIVATE long font_cache_find_character(long font_index, long character);
PRIVATE long font_cache_new_character(long font_index, long character);
PRIVATE void font_cache_delete_character(long datum_index);
PRIVATE bool font_cache_character_in_use(long datum_index);
PRIVATE void font_cache_character_read_header_offset(s_font_character *character, bool wait);
PRIVATE void font_character_header_verify(s_font_character_header *header);
PRIVATE void font_cache_character_wait_header_offset(s_font_character *character, bool wait);
PRIVATE void font_cache_character_read_header(s_font_character *character, bool wait);
PRIVATE void font_cache_character_wait_header(s_font_character *character, bool wait);
PRIVATE bool font_cache_character_update(long datum_index, bool wait);
PRIVATE void *font_cache_pixels_get(long pixels_index, bool wait);
PRIVATE bool font_cache_character_allocate_pixels(long datum_index, bool wait);
PRIVATE byte *font_cache_pixels_get_buffer(long pixels_index);
PRIVATE long font_cache_read(long font_index, void *buffer, long size, dword offset, long priority, dword *bytes_read, bool volatile *done);

/* a new frame of the pixel memory: the clock advances and the per-type
   limits are lifted */
static inline void font_pixels_memory_new_frame(s_physical_object *physical)
{
	if (physical->time == 0x7fffffff)
	{
		physical_memory_reset_time(physical);
	}
	else
	{
		physical->time++;
	}

	for (short i = 0; i < 8; i++)
	{
		physical->limits[i] = 0x7fffffff;
	}
}

// @retail 0x1406b0
void font_cache_update(void)
{
	font_pixels_memory_new_frame(g_54d580);

	g_54d588++;

	s_data_datum_iterator iterator;
	iterator.data = g_54d578;
	iterator.index = NONE;

	while (data_datum_iterator_next_inlined(&iterator))
	{
		s_font_prediction *prediction = (s_font_prediction *)iterator.datum;
		long datum_index = iterator.datum_index;
		long character = prediction->character;
		s_font_cache_flags flags = { false, true };
		long character_index = font_cache_get_character(prediction->font_index, character, flags);

		if (character_index != NONE && font_cache_character_load_pixels(character_index, flags))
		{
			if (font_cache_character_get_header(character_index))
			{
				record_pool_release(g_54d578, datum_index);
			}
		}
	}
}

// @retail 0x1407d0
long font_cache_get_character(long font_index, long character, s_font_cache_flags flags)
{
	long font = g_4e28f4[font_index];
	long datum_index = font_cache_find_character(font, character);
	bool wait = TEST_FIELD_BIT(flags.wait);

	if (datum_index == NONE)
	{
		if (!TEST_FIELD_BIT(flags.create))
		{
			return NONE;
		}

		datum_index = font_cache_new_character(font, character);
		if (datum_index == NONE)
		{
			return NONE;
		}
	}

	s_font_character *local_d3550b = FONT_CHARACTER(datum_index);
	bool ready = local_d3550b->state == _font_character_state_ready;

	if (!ready)
	{
		do
		{
			ready = font_cache_character_update(datum_index, wait);
		}
		while (wait && !ready);
	}

	FONT_CHARACTER(datum_index)->last_used_frame = g_54d588;

	if (!ready)
	{
		return NONE;
	}
	return datum_index;
}

// @retail 0x140870
bool font_cache_character_load_pixels(long datum_index, s_font_cache_flags flags)
{
	bool wait = TEST_FIELD_BIT(flags.wait);
	bool result = false;

	if (datum_index != NONE)
	{
		s_font_character *character = FONT_CHARACTER(datum_index);

		if (character->pixels_index == NONE)
		{
			font_cache_character_allocate_pixels(datum_index, wait);
		}

		if (character->pixels_index != NONE)
		{
			void *pixels = font_cache_pixels_get(character->pixels_index, wait);

			PHYSICAL_BLOCK(character->pixels_index)->time = g_54d580->time;
			result = pixels != NULL;
		}
	}

	return result;
}

// @retail 0x140900
s_font_character_header *font_cache_get_character_header(long font_index, long character, s_font_cache_flags flags)
{
	s_font_character_header *result = NULL;
	long datum_index = font_cache_get_character(font_index, character, flags);

	if (datum_index != NONE && font_cache_character_load_pixels(datum_index, flags))
	{
		result = font_cache_character_get_header(datum_index);
	}

	return result;
}

// @retail 0x140960
bool font_cache_character_copy_pixels(long datum_index, void *destination)
{
	s_font_cache_flags flags = { true, true };
	bool result = font_cache_character_load_pixels(datum_index, flags);

	if (result)
	{
		s_font_character *character = FONT_CHARACTER(datum_index);

		function_122610(character->header.pixels_size, font_cache_pixels_get_buffer(character->pixels_index), destination);
	}

	return result;
}

PRIVATE dword __stdcall font_cache_hash(const void *key);
PRIVATE bool __stdcall font_cache_compare(const void *key_a, const void *key_b);

// @retail 0x1409d0
void font_cache_initialize(void)
{
	g_54d574 = data_new_inlined("character data", 0x200, sizeof(s_font_character), 0, g_468758);
	g_54d578 = data_new_inlined("predicted data", 0x100, sizeof(s_font_prediction), 0, g_468758);
	g_54d570 = function_13e1a0("font character hash table", sizeof(long), 0x400, font_cache_hash, font_cache_compare, 0x200, g_468758);

	data_make_valid_inlined(g_54d574);
	data_make_valid_inlined(g_54d578);
}

// @retail 0x140aa0
void font_cache_dispose(void)
{
	if (g_54d570)
	{
		g_54d570->allocator->deallocate(g_54d570);
		g_54d570 = NULL;
	}

	if (g_54d578)
	{
		// Read the pool receiver through a const pointer at this later use.
		data_dispose(*(s_record_pool *const *)&g_54d578);
		g_54d578 = NULL;
	}

	if (g_54d574)
	{
		// Read the second pool through a const pointer at this later use.
		data_dispose(*(s_record_pool *const *)&g_54d574);
		g_54d574 = NULL;
	}
}

// @retail 0x140b20
PRIVATE long font_cache_find_character(long font_index, long character)
{
	long datum_index = NONE;
	s_font_character_key key;

	key.font_index = font_index;
	key.character = character;
	hash_table_find_data(g_54d570, &key, &datum_index);

	return datum_index;
}

// @retail 0x140b80
PRIVATE long font_cache_new_character(long font_index, long character)
{
	long datum_index = font_cache_find_character(font_index, character);

	if (datum_index == NONE)
	{
		datum_index = record_pool_allocate(g_54d574);
		if (datum_index == NONE)
		{
			long oldest_age = 0;
			s_data_datum_iterator iterator;

			iterator.data = g_54d574;
			iterator.index = NONE;
			while (data_datum_iterator_next_called(&iterator))
			{
				s_font_character *candidate = (s_font_character *)iterator.datum;

				if (!font_cache_character_in_use(iterator.datum_index))
				{
					long age = g_54d588 - candidate->last_used_frame;

					if (age > oldest_age)
					{
						oldest_age = age;
						datum_index = iterator.datum_index;
					}
				}
			}

			if (datum_index != NONE)
			{
				font_cache_delete_character(datum_index);
				datum_index = datum_new_at_index_with_salt(g_54d574, datum_index);
			}
		}

		if (datum_index != NONE)
		{
			s_font_character *local_d3550b = FONT_CHARACTER(datum_index);

			memset(&local_d3550b->unknown02, 0, sizeof(s_font_character) - 2);
			local_d3550b->key.character = character;
			local_d3550b->header_offset_ready = false;
			local_d3550b->header_ready = false;
			local_d3550b->state = _font_character_state_none;
			local_d3550b->task = NONE;
			local_d3550b->header_offset = NONE;
			local_d3550b->pixels_index = NONE;
			local_d3550b->key.font_index = font_index;
			// Read the frame counter and the table through const pointers at these later uses.
			local_d3550b->last_used_frame = *(long const *)&g_54d588;
			local_d3550b->render_index = NONE;

			function_13e270(*(hash_table *const *)&g_54d570, &local_d3550b->key, &datum_index);
		}
	}

	return datum_index;
}

// @retail 0x140cd0
PRIVATE void font_cache_delete_character(long datum_index)
{
	s_font_character *character = FONT_CHARACTER(datum_index);
	long render_index = character->render_index;

	if (render_index != NONE)
	{
		long other_index = g_4b62b0[render_index].character_index;

		if (other_index != NONE)
		{
			FONT_CHARACTER(other_index)->render_index = NONE;
		}
		g_4b62b0[render_index].character_index = NONE;
	}

	if (character->pixels_index != NONE)
	{
		g_54d580->block_delete(character->pixels_index);
	}

	hash_table_remove(g_54d570, &character->key);
	record_pool_release(g_54d574, datum_index);
}

// @retail 0x140d50
PRIVATE bool font_cache_character_in_use(long datum_index)
{
	s_font_character *character = FONT_CHARACTER(datum_index);
	bool in_use = g_54d588 - character->last_used_frame <= 1 ||
		character->state == _font_character_state_reading_header_offset ||
		character->state == _font_character_state_reading_header;

	if (character->pixels_index != NONE)
	{
		s_font_pixels *pixels = FONT_PIXELS(character->pixels_index);

		if (!in_use && pixels->ready)
		{
			return false;
		}
		return true;
	}

	return in_use;
}

// @retail 0x140dc0
PRIVATE void font_cache_character_read_header_offset(s_font_character *character, bool wait)
{
	character->task = font_cache_read(character->key.font_index, &character->header_offset, sizeof(long),
		character->key.character * sizeof(long) + 0x400, wait ? 6 : 2, 0, &character->header_offset_ready);
	character->state = _font_character_state_reading_header_offset;
}

// @retail 0x140e40
PRIVATE void font_character_header_verify(s_font_character_header *header)
{
	if (header->width < 0 || header->width > 0x80 ||
		header->height < 0 || header->height > 0x80 ||
		header->pixels_size <= 0 || header->pixels_size > 0x1000)
	{
		header->width = header->width < 0 ? 0 : header->width > 0x80 ? 0x80 : header->width;
		header->height = header->height < 0 ? 0 : header->height > 0x80 ? 0x80 : header->height;
		header->pixels_size = header->pixels_size < 1 ? 1 : header->pixels_size > 0x1000 ? 0x1000 : header->pixels_size;

		if (global_preferences_globals.current.unknown1c != NONE)
		{
			global_preferences_globals.current.unknown1c = NONE;
			global_preferences_globals.dirty = true;
			global_preferences_flush();
		}
	}
}

// @retail 0x140f00
PRIVATE void font_cache_character_wait_header_offset(s_font_character *character, bool wait)
{
	if (!character->header_offset_ready && wait)
	{
		function_120ce0(character->task, 6);
		async_wait(&character->header_offset_ready);
	}

	if (character->header_offset_ready)
	{
		character->state = _font_character_state_header_offset_ready;
		font_cache_character_read_header(character, wait);
	}
}

// @retail 0x140f50
PRIVATE void font_cache_character_read_header(s_font_character *character, bool wait)
{
	character->task = font_cache_read(character->key.font_index, &character->header, sizeof(s_font_character_header),
		(character->header_offset + 0x4040) << 4, wait ? 6 : 2, 0, &character->header_ready);
	character->state = _font_character_state_reading_header;
}

// @retail 0x140fc0
PRIVATE void font_cache_character_wait_header(s_font_character *character, bool wait)
{
	if (!character->header_ready && wait)
	{
		function_120ce0(character->task, 6);
		async_wait(&character->header_ready);
	}

	if (character->header_ready)
	{
		font_character_header_verify(&character->header);
		character->state = _font_character_state_ready;
	}
}

// @retail 0x141020
PRIVATE bool font_cache_character_update(long datum_index, bool wait)
{
	s_font_character *character = FONT_CHARACTER(datum_index);
	bool result = false;

	switch (character->state)
	{
	case _font_character_state_none:
		font_cache_character_read_header_offset(character, wait);
		break;
	case _font_character_state_reading_header_offset:
		font_cache_character_wait_header_offset(character, wait);
		break;
	case _font_character_state_header_offset_ready:
		font_cache_character_read_header(character, wait);
		break;
	case _font_character_state_reading_header:
		font_cache_character_wait_header(character, wait);
		break;
	}

	if (character->state == _font_character_state_ready)
	{
		result = true;
	}
	return result;
}

// @retail 0x141090
PRIVATE dword __stdcall font_cache_hash(const void *key)
{
	s_font_character_key const *character_key = (s_font_character_key const *)key;

	return character_key->font_index * 3 + character_key->character;
}

// @retail 0x1410b0
PRIVATE bool __stdcall font_cache_compare(const void *key_a, const void *key_b)
{
	s_font_character_key const *a = (s_font_character_key const *)key_a;
	s_font_character_key const *b = (s_font_character_key const *)key_b;

	if (a->font_index == b->font_index && a->character == b->character)
	{
		return true;
	}
	return false;
}

PRIVATE void __stdcall font_cache_pixels_delete(long pixels_index);
PRIVATE bool __stdcall font_cache_pixels_locked(long pixels_index);

static __forceinline s_physical_object *font_pixels_memory_create(c_data_allocator *allocator)
{
    s_physical_object *physical = (s_physical_object *)allocator->allocate(0x30fc);
    function_13d170(physical, "font pixel data", 0x780, 4, 0x200,
        font_cache_pixels_delete, font_cache_pixels_locked, 0, allocator);
    return physical;
}

// @retail 0x1410d0
void font_cache_pixels_initialize(void)
{
	byte *buffer = (byte *)VirtualAlloc(NULL, 0x7800, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!buffer)
	{
		GetLastError();
	}
	g_54d584 = buffer;

	g_54d57c = data_new_inlined("font pixel data", 0x200, sizeof(s_font_pixels), 0, g_468758);

	s_physical_object *physical = font_pixels_memory_create(g_468758);
	g_54d580 = physical;
	// Read the physical receiver through a const pointer at this later use.
	(*(s_physical_object *const *)&g_54d580)->state = 2;

	data_make_valid_inlined(g_54d57c);
}

// @retail 0x141190
void font_cache_pixels_dispose(void)
{
	if (g_54d580)
	{
		(*(c_data_allocator **)((byte *)g_54d580 + 0x6c))->deallocate(g_54d580);
		g_54d580 = NULL;
	}

	if (g_54d57c)
	{
		// Read the pool through a const pointer at this later use.
		data_dispose(*(s_record_pool *const *)&g_54d57c);
		g_54d57c = NULL;
	}
}

// @retail 0x1411e0
PRIVATE void __stdcall font_cache_pixels_delete(long pixels_index)
{
	s_font_character *character = FONT_CHARACTER(FONT_PIXELS(pixels_index)->character_index);

	while (font_cache_pixels_locked(pixels_index))
	{
		async_globals.tasks_added = function_120bf0();
	}

	character->pixels_index = NONE;
	record_pool_release(g_54d57c, pixels_index);
}

// @retail 0x141290
PRIVATE bool __stdcall font_cache_pixels_locked(long pixels_index)
{
	bool locked = false;
	s_font_pixels *pixels = FONT_PIXELS(pixels_index);

	if (!pixels->ready)
	{
		locked = true;
	}
	return locked;
}

// @retail 0x1412c0
PRIVATE void *font_cache_pixels_get(long pixels_index, bool wait)
{
	void *result = NULL;

	if (pixels_index != NONE)
	{
		s_font_pixels *pixels = FONT_PIXELS(pixels_index);

		if (!pixels->ready && wait)
		{
			function_120ce0(pixels->task, 7);
			async_wait(&pixels->ready);
		}

		if (pixels->ready)
		{
			result = font_cache_pixels_get_buffer(pixels_index);
		}
	}

	return result;
}

// @retail 0x141350
PRIVATE bool font_cache_character_allocate_pixels(long datum_index, bool wait)
{
	bool result = false;
	s_font_character *character = FONT_CHARACTER(datum_index);

	if (character->pixels_index != NONE)
	{
		result = true;
	}
	else if (character->state == _font_character_state_ready)
	{
		long size = character->header.pixels_size;
		long block_index = function_13d370(g_54d580, size, 1);

		if (block_index != NONE)
		{
			long pixels_index = datum_new_at_index_with_salt(g_54d57c, block_index);

			if (pixels_index != NONE)
			{
				s_font_pixels *pixels = FONT_PIXELS(pixels_index);
				byte *buffer = font_cache_pixels_get_buffer(pixels_index);

				character->pixels_index = pixels_index;
				pixels->character_index = datum_index;
				pixels->task = font_cache_read(character->key.font_index, buffer, size, character->header.pixels_offset, wait ? 6 : 2, 0, &pixels->ready);

				if (wait)
				{
					async_wait(&pixels->ready);
				}

				result = true;
			}
		}
	}

	return result;
}

// @retail 0x141450
PRIVATE byte *font_cache_pixels_get_buffer(long pixels_index)
{
	// Read the pixel buffer through a const pointer, as the file's other pools are.
	return *(byte *const *)&g_54d584 + (PHYSICAL_BLOCK(pixels_index)->offset << g_54d580->page_shift);
}

// @retail 0x141480
PRIVATE long font_cache_read(long font_index, void *buffer, long size, dword offset, long priority, dword *bytes_read, bool volatile *done)
{
	s_file_handle file;
	/* the copy keeps retail's register use in the callers that inline this */
	long read_priority = priority;

	file.handle = INVALID_HANDLE_VALUE;
	if (font_get(font_index))
	{
		file = g_4e2920[font_index].file;
	}

	return function_1a0f10(file, buffer, size, offset, 7, read_priority, bytes_read, done);
}

// score 14 (was 14): only registers differ: ours keeps font_index in EBX and the bitmap
// mask in EBP, retail the other way round. Tried: result flag from the header test (76);
// found flag with break (14); if/else with a result variable (94); local copies of the
// arguments or reading `.field_0` (14); the s_data_datum_iterator form (14); a separate
// pointer for the new prediction (14); early `return true` (72); nested ifs in the loop
// (14); plain `long` parameters with the caller's declaration changed (86, and 0x13eeb0
// drops to 18). Round 41: only the character as a plain long 86 (0x13eeb0 18); only the font
// as a plain long 88 (0x13eeb0 22); iterator and prediction at function scope 14; header
// result in a pointer local 14; break out of the search and allocate only when not found 14.
// Round 41 (lane 10): match test with the parameters on the left 16 (the font compare then
// reads the field as its memory operand); iterator index set before its data 14; explicit
// `(long)` conversions at the uses 14; prediction declared before the iterator 14; return
// early when the allocation fails 14. Round 64: shared pool local 49; allocation pointer
// arithmetic 14; null-initialized prediction 14; byte entry local 14; non-short-circuit
// comparison 85; mismatch-continue loop 119; unsigned datum index 14; named masked index 14.
// @retail 0x1414d0
bool font_cache_predict_character(s_13eeb1 font_index, s_13eeb1 character)
{
	s_font_cache_flags flags = { false, true };

	if (!font_cache_get_character_header(font_index, character, flags))
	{
		s_record_pool_iterator iterator;
		s_font_prediction *prediction;

		iterator.data = g_54d578;
		iterator.index = NONE;
		while ((prediction = (s_font_prediction *)data_iterator_next_inlined(&iterator)) != NULL)
		{
			if (prediction->font_index == font_index && prediction->character == character)
			{
				return false;
			}
		}

		long datum_index = record_pool_allocate(g_54d578);
		if (datum_index != NONE)
		{
			prediction = (s_font_prediction *)&((s_font_prediction *)g_54d578->data)[datum_index & 0xffff];
			prediction->font_index = font_index;
			prediction->character = character;
		}

		return false;
	}

	return true;
}
