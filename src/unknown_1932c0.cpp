// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "language.h"
#include "unknown_0259d0.h"
#include "files.h"
#include "pending_messages.h"
#include "unknown_19d220.h"
#include "physical_memory.h"

/* a game variant (0x614 bytes; the 16 of them are at 0x551ae8) */
struct s_surface_description
{
	long type;
	long field_4;
	dword field_8;
	char names[9][0x10];
	char descriptions[9][0x80];
	char field_51c[128];
	byte unknown59c[0x5a4 - 0x59c];
	long points[16];
	long field_5e4;
	bool flag_5e8;
	byte unknown5e9[3];
	long width;       // 0x5ec
	long height;      // 0x5f0
	long depth;       // 0x5f4
	long field_5f8;   // 0x5f8
	long field_5fc;
	long field_600;
	bool flag_604;    // 0x604
	byte unknown605[3];
	long field_608;   // 0x608
	long field_60c;   // 0x60c
	byte unknown610[4];
};

/* the game variant globals at 0x47d8f4 */
struct s_game_variant_globals
{
	dword unknown0;
	dword flags;
	word count;
	word state;
};

s_game_variant_globals g_47d8f4;
s_surface_description g_551ae8[16];
// @retail 0x001932c0
long function_1932c0(s_surface_description* p)
{
	switch (p->type)
	{
	case 1:
		return p->width;
	case 2:
		return p->width;
	case 3:
		return p->height * p->width;
	case 4:
		return p->height * p->width;
	case 5:
		return p->height * p->width;
	default:
		__assume(0);
	}
}

__forceinline long variant_upper_place(s_surface_description* p)
{
	switch (p->type)
	{
	case 1:
		return p->height;
	case 2:
		return p->height;
	case 3:
		return p->depth * p->width;
	case 4:
		return p->depth * p->width;
	case 5:
		return p->depth * p->width;
	default:
		__assume(0);
	}
}

// @retail 0x00193300
long function_193300(s_surface_description* p)
{
	return variant_upper_place(p);
}

// @retail 0x00193340
long function_193340(s_surface_description* p)
{
	switch (p->type)
	{
	case 1:
		return 1;
	case 2:
		return 1;
	case 3:
		return p->height;
	case 4:
		return p->height;
	case 5:
		return p->height;
	default:
		__assume(0);
	}
}

// @retail 0x00193370
long function_193370(s_surface_description* p)
{
	switch (p->type)
	{
	case 1:
		return 1;
	case 2:
		return 1;
	case 3:
		return p->depth;
	case 4:
		return p->depth;
	case 5:
		return p->depth;
	default:
		__assume(0);
	}
}

// @retail 0x001933a0
long function_1933a0(s_surface_description* p)
{
	long result = 0;
	if (p->type == 5)
	{
		result = p->depth;
	}
	else if (p->type == 4 && p->flag_604)
	{
		result = p->field_60c;
	}
	return result;
}

// @retail 0x001933d0
long function_1933d0(s_surface_description* p)
{
	long result = 0;
	if (p->type == 5)
	{
		result = p->height;
	}
	else if (p->type == 4 && p->flag_604)
	{
		result = p->field_608;
	}
	return result;
}

// @retail 0x00193400
long function_193400(s_surface_description* p)
{
	long result = 0;
	switch (p->type)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		if (*(byte*)&p->field_5f8)
		{
			result = 1;
		}
		break;
	case 4:
		result = p->field_5f8;
		break;
	case 5:
		result = p->field_5f8;
		break;
	default:
		__assume(0);
	}
	return result;
}

// @retail 0x00193440
long function_193440(s_surface_description* p)
{
	long result = NONE;
	switch (p->type)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		result = p->width;
		break;
	case 4:
		result = p->width;
		break;
	case 5:
		result = p->width;
		break;
	default:
		__assume(0);
	}
	return result;
}
// @retail 0x00193470
bool function_193470(s_surface_description* p)
{
	return p->type == 5 || p->type == 2 || p->type == 4;
}

// @retail 0x00193490
bool function_193490(s_surface_description* p)
{
	return p->type == 1 || p->type == 3;
}

// @retail 0x001934b0
bool function_1934b0(s_surface_description* p)
{
	return p->type == 5 || p->type == 3 || p->type == 4;
}

// @retail 0x001934d0
bool function_1934d0(s_surface_description* p)
{
	return (p->type == 4 && p->flag_604) || p->type == 5;
}

// @retail 0x1931a0
long function_1931a0(long count, s_surface_description *p)
{
	long index = count;

	if (count > 0)
	{
		do
		{
			if (p->field_51c[index - 1] < count)
			{
				break;
			}
			index--;
		}
		while (index > 0);
	}
	return index;
}

// @retail 0x1931d0
long function_1931d0(s_surface_description *p, long place)
{
	long last = function_193300(p);
	long first = function_1932c0(p);

	if (place < first || place > last)
		return 0x7fffffff;
	if (place == last)
		return 0;
	return p->points[place - 1];
}

// @retail 0x193250
long function_193250(s_surface_description *p)
{
	switch (p->type)
	{
	case 1:
		goto upper_place;
	case 2:
		return 1;
	case 3:
		goto upper_place;
	case 4:
		return p->field_600;
	case 5:
		return p->depth;
	default:
		__assume(0);
	}
upper_place:
	return variant_upper_place(p);
}

bool matchmaking_hopper_verify_common(s_surface_description *p);
byte matchmaking_hopper_verify_players(s_surface_description *p);
byte matchmaking_hopper_verify_teams_party_limits(s_surface_description *p);
byte matchmaking_hopper_verify_teams(s_surface_description *p);
byte matchmaking_hopper_verify_teams_party_of_team(s_surface_description *p);
__declspec(noinline) bool function_1934f0(s_surface_description *p);

// @retail 0x1934f0
bool function_1934f0(s_surface_description *p)
{
	if (!matchmaking_hopper_verify_common(p))
	{
		return false;
	}
	switch (p->type)
	{
	case 1:
		return matchmaking_hopper_verify_players(p);
	case 2:
		return matchmaking_hopper_verify_players(p);
	case 4:
		return matchmaking_hopper_verify_teams_party_limits(p);
	case 3:
		return matchmaking_hopper_verify_teams(p);
	case 5:
		return matchmaking_hopper_verify_teams_party_of_team(p);
	default:
		__assume(0);
	}
}

static inline bool game_variants_available(void)
{
	return g_47d8f4.count && (g_47d8f4.flags & 2);
}

static inline bool game_variant_valid(long index)
{
	bool result = false;

	if (index >= 0 && index < 16 && game_variants_available())
	{
		result = function_1934f0(&g_551ae8[index]);
	}
	return result;
}

// @retail 0x192e60
s_surface_description *function_192e60(long index)
{
	s_surface_description *result = NULL;

	if (game_variant_valid(index))
	{
		result = &g_551ae8[index];
	}
	return result;
}

// @retail 0x192db0
bool function_192db0(long index)
{
	return game_variant_valid(index);
}

// @retail 0x193f50
bool function_193f50(void)
{
	return game_variants_available();
}

bool pending_message_request_get_progress(s_pending_message_header *header, real *progress);

/* g_47d8f4 starts with a pending message request (unknown_080f30.cpp) */
// @retail 0x193f70
bool function_193f70(real *progress)
{
	bool result = false;

	if (g_47d8f4.count && g_47d8f4.state == 1)
	{
		result = true;
		if (progress)
		{
			pending_message_request_get_progress((s_pending_message_header *)&g_47d8f4, progress);
		}
	}
	return result;
}

// @retail 0x1945c0
long function_1945c0(long index)
{
	long result = 0;

	if (game_variant_valid(index))
	{
		result = g_551ae8[index].field_5e4;
	}
	return result;
}

// @retail 0x194610
bool function_194610(long index)
{
	bool result = false;

	if (game_variant_valid(index))
	{
		result = g_551ae8[index].flag_5e8;
	}
	return result;
}

char const *g_46dd5c;

// @retail 0x193fa0
void function_193fa0(s_type_acf665 *file)
{
	memset(file, 0, sizeof(*file));
	file->signature = FILE_REFERENCE_SIGNATURE;
	file->location = NONE;
	function_137320(file->path, "n:\\");
	if (file->flags & 1)
	{
		function_1373c0(file->path);
	}
	function_137320(file->path, g_46dd5c);
	file->flags |= 1;
}

/* the game variants file: the 16 variants, then one block per variant */
/* one map of a variant's map list (0x37c bytes) */
struct s_game_variant_map
{
	long unknown000;
	long map_id;
	char names[9][64];  /* UTF-16 text, the same name in each language */
	long weight;
	byte settings[0x130];
};

struct s_game_variant_block
{
	byte unknown0000[0x44];
	long map_count;
	s_game_variant_map maps[100];
};

struct s_game_variants_file
{
	s_surface_description variants[16];
	s_game_variant_block blocks[16];
};

/* size before buffer: the order the arguments are set up in */
PRIVATE inline bool file_write_sized(s_type_acf665 *file, dword size, void const *buffer)
{
	return function_136d00(file, size, buffer);
}

// @retail 0x193ff0
bool game_variants_file_write(s_game_variants_file *variants_file)
{
	bool result = false;
	s_type_acf665 file;

	function_193fa0(&file);
	if (function_1367d0(&file))
	{
		dword error;

		if (function_136970(&file, 2, &error))
		{
			if (file_write_sized(&file, sizeof(*variants_file), variants_file))
				result = true;
			function_136bb0(&file);
		}

		if (!result)
			file_ref_delete(&file);
		else
			memcpy(g_551ae8, variants_file->variants, sizeof(g_551ae8));
	}

	return result;
}

// @retail 0x1943d0
bool game_variant_block_read(long index, s_game_variant_block *block)
{
	bool result = false;

	if (game_variants_available() && function_1934f0(&g_551ae8[index]))
	{
		s_type_acf665 file;
		dword error;

		function_193fa0(&file);
		if (function_136970(&file, 1, &error))
		{
			if (file_ref_seek(&file, sizeof(g_551ae8) + index * sizeof(s_game_variant_block), true) &&
				file_ref_read(&file, sizeof(*block), block, true))
			{
				result = true;
			}
			function_136bb0(&file);
		}
	}

	return result;
}

void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count);

// @retail 0x1944c0
bool game_variant_get_name(long index, word *name)
{
	bool result = false;

	if (game_variant_valid(index))
	{
		utf8_string_to_utf16_string(g_551ae8[index].names[get_current_language()], name, 0x10);
		result = true;
	}
	return result;
}

// @retail 0x194540
bool game_variant_get_description(long index, word *description)
{
	bool result = false;

	if (game_variant_valid(index))
	{
		utf8_string_to_utf16_string(g_551ae8[index].descriptions[get_current_language()], description, 0x80);
		result = true;
	}
	return result;
}

struct s_entry_c;
s_entry_c *function_19c5f0(long map_id);

// @retail 0x194660
long game_variant_check_maps(long index)
{
	s_game_variant_block block;
	long status = 0;

	if (game_variants_available())
	{
		if (game_variant_block_read(index, &block))
		{
			status = 1;
			for (long i = 0; i < block.map_count; i++)
			{
				if (!function_19c5f0(block.maps[i].map_id))
					status = 2;
			}
		}
		else
		{
			status = 2;
		}
	}

	g_551ae8[index].field_4 = status;
	return status;
}

// @retail 0x192df0
long game_variant_get_map_status(long index)
{
	long status = 0;

	if (index != NONE)
	{
		if (game_variant_valid(index))
		{
			s_surface_description *variant = &g_551ae8[index];

			if (function_1934f0(variant) && variant->field_4 == 0)
				game_variant_check_maps(index);
			if (function_1934f0(variant))
			{
				status = variant->field_4;
				goto done;
			}
		}
		status = 3;
	}

done:
	return status;
}

static inline short random_range(dword *seed, short lower, short upper)
{
	return (short)(lower + ((random_next(seed) * (upper - lower)) >> 16));
}

// @retail 0x192eb0
bool game_variant_choose_map(long index, long *map_id, byte *settings)
{
	s_game_variant_block block;
	long weights[100];
	bool result = false;

	if (game_variant_block_read(index, &block))
	{
		long selected = NONE;
		long total = 0;
		long i;

		for (i = 0; i < block.map_count; i++)
		{
			long weight = 0;

			if (function_19c5f0(block.maps[i].map_id))
			{
				weight = block.maps[i].weight;
				if (weight < 1)
					weight = 1;
				else if (weight > 1000)
					weight = 1000;
			}
			total += weight;
			weights[i] = weight;
		}

		if (total > 0)
		{
			long choice = random_range(&g_4e7408->seed, 1, (short)(total + 1));
			long sum = 0;

			for (i = 0; i < block.map_count && selected == NONE; i++)
			{
				sum += weights[i];
				if (sum >= choice)
					selected = i;
			}

			if (selected >= 0 && selected < block.map_count)
			{
				*map_id = block.maps[selected].map_id;
				memcpy(settings, block.maps[selected].settings, sizeof(block.maps[selected].settings));
				result = true;
			}
		}
	}

	return result;
}


struct s_map_name_193010
{
    long map_id;
    char name[64];
};

PRIVATE s_map_name_193010 const g_444cd0[11] = {
    { 50, { 76, 0, 111, 0, 99, 0, 107, 0, 111, 0, 117, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 20, { 77, 0, 105, 0, 100, 0, 115, 0, 104, 0, 105, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 10, { 67, 0, 121, 0, 99, 0, 108, 0, 111, 0, 116, 0, 114, 0, 111, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 30, { 90, 0, 97, 0, 110, 0, 122, 0, 105, 0, 98, 0, 97, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 40, { 87, 0, 97, 0, 116, 0, 101, 0, 114, 0, 119, 0, 111, 0, 114, 0, 107, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 80, { 65, 0, 115, 0, 99, 0, 101, 0, 110, 0, 115, 0, 105, 0, 111, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 70, { 67, 0, 111, 0, 108, 0, 111, 0, 115, 0, 115, 0, 117, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 60, { 66, 0, 117, 0, 114, 0, 105, 0, 97, 0, 108, 0, 32, 0, 77, 0, 111, 0, 117, 0, 110, 0, 100, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 110, { 67, 0, 111, 0, 97, 0, 103, 0, 117, 0, 108, 0, 97, 0, 116, 0, 105, 0, 111, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 100, { 66, 0, 97, 0, 116, 0, 116, 0, 108, 0, 101, 0, 32, 0, 67, 0, 114, 0, 101, 0, 101, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 800, { 72, 0, 101, 0, 97, 0, 100, 0, 108, 0, 111, 0, 110, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};

struct s_map_preset_193010 { long preset; long map_id; long weight; };
PRIVATE s_map_preset_193010 const g_444fc0[14] = {
    { 1, 10, 16 },
    { 1, 50, 14 },
    { 1, 70, 12 },
    { 1, 80, 10 },
    { 1, 80, 3 },
    { 3, 50, 1 },
    { 5, 50, 1 },
    { 5, 20, 1 },
    { 3, 10, 1 },
    { 3, 70, 1 },
    { 24, 50, 1 },
    { 24, 80, 1 },
    { 24, 80, 1 },
    { 22, 80, 1 },
};
PRIVATE s_map_preset_193010 const g_445068[24] = {
    { 15, 30, 10 },
    { 13, 70, 6 },
    { 19, 60, 5 },
    { 23, 80, 5 },
    { 21, 110, 4 },
    { 25, 50, 3 },
    { 25, 70, 3 },
    { 18, 30, 3 },
    { 21, 20, 3 },
    { 26, 20, 3 },
    { 26, 50, 3 },
    { 2, 10, 2 },
    { 2, 50, 2 },
    { 16, 10, 2 },
    { 16, 60, 2 },
    { 13, 100, 2 },
    { 2, 70, 2 },
    { 13, 20, 1 },
    { 14, 20, 1 },
    { 16, 50, 1 },
    { 19, 10, 1 },
    { 26, 70, 1 },
    { 26, 80, 1 },
    { 16, 80, 1 },
};
PRIVATE s_map_preset_193010 const g_445188[12] = {
    { 15, 800, 8 },
    { 18, 800, 4 },
    { 15, 30, 4 },
    { 15, 40, 3 },
    { 12, 40, 3 },
    { 20, 110, 2 },
    { 27, 30, 2 },
    { 27, 800, 2 },
    { 17, 40, 1 },
    { 18, 30, 1 },
    { 20, 40, 1 },
    { 27, 70, 1 },
};
PRIVATE s_map_preset_193010 const g_445218[5] = {
    { 2, 10, 8 },
    { 2, 40, 4 },
    { 2, 50, 6 },
    { 2, 30, 1 },
    { 4, 10, 1 },
};
PRIVATE s_map_preset_193010 const g_445258[6] = {
    { 0, 50, 16 },
    { 0, 80, 12 },
    { 0, 20, 6 },
    { 7, 50, 2 },
    { 9, 50, 1 },
    { 9, 20, 1 },
};

void function_212f40(long preset, s_game_variant *variant);

/* sets or clears one bit of a flags word */
static inline void flags_set_bit(dword *flags, long bit, bool value)
{
	if (value)
		*flags |= 1 << bit;
	else
		*flags &= ~(1 << bit);
}

/* memcpy, with its arguments set up source first, the order retail uses */
static __forceinline void *copy_bytes(void *destination, void const *source, long size)
{
	return memcpy(destination, source, size);
}

// @retail 0x193010
void function_193010(s_game_variant_block *block, long type)
{
    s_map_preset_193010 const *presets = NULL;
    long count = 0;
    memset(block, 0, sizeof(*block));
    if (type == 2)
    {
        presets = g_445258;
        count = 6;
    }
    else if (type == 0 || type == 13)
    {
        presets = g_444fc0;
        count = 14;
    }
    else if (type == 1 || type == 6 || type == 14 || type == 4)
    {
        presets = g_445068;
        count = 24;
    }
    else if (type == 3 || type == 15 || type == 5)
    {
        presets = g_445188;
        count = 12;
    }
    else if (type == 11 || type == 12)
    {
        presets = g_445218;
        count = 5;
    }
    if (presets)
    {
    block->map_count = count;
    for (long i = 0; i < count; i++)
    {
        s_game_variant_map *map = &block->maps[i];
        char name[64];
        memset(name, 0, sizeof(name));
        for (dword j = 0; j < 11; j++)
        {
            if (g_444cd0[j].map_id == map->map_id)
            {
                memcpy(name, g_444cd0[j].name, sizeof(name));
                break;
            }
        }
        map->map_id = presets[i].map_id;
        /* the name in every language */
        char *names = map->names[0];
        memcpy(names, name, sizeof(name));
        copy_bytes(names + sizeof(name), names, 8 * sizeof(name));
        map->weight = presets[i].weight;
        function_212f40(presets[i].preset, (s_game_variant *)map->settings);
        flags_set_bit((dword *)(map->settings + 0x48), 4, false);
        flags_set_bit((dword *)(map->settings + 0x48), 5, false);
        *(long *)(map->settings + 0x58) = 0;
    }
    }
    else
    {
    block->maps[0].map_id = 30000;
    block->map_count = 1;
    function_19d220((s_game_variant *)block->maps[0].settings, 4);
    *(long *)(block->maps[0].settings + 0x50) = 5;
    flags_set_bit((dword *)(block->maps[0].settings + 0x48), 4, false);
    }
}

struct s_built_variant;
void function_193c70(s_built_variant *variant, long index);

// @retail 0x1940d0
void game_variants_file_initialize(s_game_variants_file *variants_file)
{
	(void)&variants_file;
	s_surface_description variant;
	memset(variants_file, 0, sizeof(*variants_file));
	for (long i = 0; i < 16; i++)
	{
		function_193c70((s_built_variant *)&variant, i);
		if (function_1934f0(&variant))
		{
			variants_file->variants[i] = variant;
			function_193010(&variants_file->blocks[i], i);
		}
	}
}

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
void function_12d520(long address);
double timing_ticks_to_seconds(__int64 ticks);
dword function_199560(void *data, dword size);
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size);

static __int64 variants_read_tsc(void)
{
	volatile __int64 ticks = 0;
	__asm rdtsc
}

static __forceinline s_game_variants_file *variants_buffer_allocate(void)
{
	s_game_variants_file *result = NULL;
	__int64 start = variants_read_tsc();
	if (g_4e6464->page_count > 0)
	{
		long retries = 0;
		do
		{
			result = (s_game_variants_file *)function_12d2f0(sizeof(*result), 0, 0, 0);
			if (result)
				break;
			if (retries < 90)
			{
				retries++;
				function_12c600();
				continue;
			}
			__int64 elapsed = variants_read_tsc() - start;
			if (elapsed < 0)
				elapsed = 0;
			if (!((real)timing_ticks_to_seconds(elapsed) < 1.0f))
				break;
			D3DDevice_KickPushBuffer();
			D3DDevice_IsBusy();
			SwitchToThread();
		}
		while (!result);
	}
	return result;
}

/* the result 0x81050 recorded for a request's last message */
static inline short pending_request_last_result(s_pending_message_header const *header)
{
	return ((short const *)&header->unknown0c)[1];
}

/* a received game variants file: decompressed into a fresh buffer and saved */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x194260
bool __stdcall game_variants_received(s_pending_message_header *header)
{
	bool result = false;
	if (header->kind && (header->flags & 2) && pending_request_last_result(header) == 1)
	{
		byte *data = NULL;
		if (header->data && (dword)header->size > 16)
			data = (byte *)header->data + 16;
		dword size = header->size - 16;
		if (data && size > 0 && function_199560(data, size) == sizeof(s_game_variants_file))
		{
			_ReadWriteBarrier();
			s_game_variants_file *file = variants_buffer_allocate();
			if (file)
			{
				long decompressed_size = 0;
				if (function_199740(data, size, (byte *)file, &decompressed_size) && game_variants_file_write(file))
					result = true;
				function_12d520((long)file);
			}
		}
	}
	return result;
}
#pragma function(_ReadWriteBarrier)

// score 3 (was 3): first tick zero store precedes EDI save and false-result store;
// timer before buffer 3; success rename 3; buffer rename 3; timestamp rename 3;
// pointer declared first 3; retries before timer 17; write result assigned 53; long result 103.
// unsigned retries 4; retry cap equality 4; elapsed clamp expression 3;
// success before write 48; branch result initialization 104; allocation guard return 88;
// prior-result expression 3; immutable start tick 3; allocator pointer name 3.
// _ReadWriteBarrier (as 0x194260 has it) moves the zero xor after the pushes: after
// the result 4; before the result 6; in the allocator before the counter read 4.
// condition-declared buffer 3; assignment condition 3; immutable buffer 3;
// split buffer assignment 3; nested buffer scope 3; cleanup goto 3;
// buffer-guarded return 56; named write result 3; byte result 99.
// Complemented result at bool/byte/short/long widths 102/102/106/100;
// true entry result with explicit failures at those widths 100/80/84/87.
// Round 69: shared header allocator (90 pumps, 1.0 s) 3; with the barrier after the result 4
// (stores in order, `xor ebx` after the pushes); shared header tick helper 3; force-inline tick
// helper with a separate zero assignment 3; plain inline allocator 297 (kept out of line).
// Every spelling hoists the first zero store above the last push, as in 0x223b60.
// Result declared after the allocation 70; tick reader __forceinline 3, inline 3; start
// tick read before the null result 3; null result in an else arm 3; tick reader without
// volatile 127 (the zero stores vanish).
// Round 117: a barrier inside the tick reader after the zero store 3, before it 4 (the zero xor
// again after the pushes); the header's address taken 3; the page count read before the tick
// read 17; the reader with external linkage 3.
// Round 133: the tick as `volatile long ticks[2]` 3; as two volatile longs 10; unsigned 3; as a
// volatile LARGE_INTEGER 3; this function placed before 0x194260 3; the retry counter at the
// allocator's top 17 (0x194260 then 8).
// @retail 0x194160
bool __stdcall game_variants_retry(s_pending_message_header *header)
{
	bool result = false;
	s_game_variants_file *file = variants_buffer_allocate();
	if (file)
	{
		game_variants_file_initialize(file);
		if (game_variants_file_write(file))
			result = true;
		function_12d520((long)file);
	}
	return result;
}

bool (__stdcall *g_46dd74)(s_pending_message_header *) = game_variants_retry;
bool (__stdcall *g_46dd7c)(s_pending_message_header *) = game_variants_received;
