// @flags /O2 /Gr
/* UNKNOWN_18EC20.CPP: saved game and map loading helpers: pushing the
   physical memory heap, and the kind of a saved game */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_12b400.h"
#include "online_tasks.h"
#include "unknown_058ee0.h"
#include <string.h>

long g_4ed294;

// @retail 0x18ec20
void function_18ec20(bool keep)
{
	g_4ed294 = 1;
	if (!keep)
	{
		function_xe0ae94();
	}
}

void texture_cache_initialize_for_new_map(void); /* unknown_12c0d0.cpp */

/* starts the physical memory stage of a new map and the texture cache in it */
// @retail 0x18ee60
void function_18ee60(void)
{
	function_xe0ae94();
	texture_cache_initialize_for_new_map();
	g_4ed294 = 5;
}

/* one language's string table in the globals tag (0x1c bytes) */
struct s_main_game_string_table
{
	void *references;
	void *data;
	byte unknown08[0x18 - 8];
	bool loaded;
	byte unknown19[3];
};

struct s_main_game_string_tables
{
	byte unknown000[0x188];
	s_main_game_string_table tables[1];
};

long function_11ca80(long value);
void function_2186f0(void); /* unknown_218850.cpp */
void texture_cache_dispose_from_old_map(void); /* unknown_12c0d0.cpp */
void geometry_cache_dispose_from_old_map(void); /* unknown_12de70.cpp */
void function_1233f0(void);
void cache_files_dispose_map(void);

struct s_bsp3d;
struct s_slot_entry_list;
extern s_bsp3d *g_4e033c;
extern s_slot_entry_list *g_4e0340;
long g_4686c0;

static inline long current_language()
{
	if (g_47ff38 == NONE)
	{
		g_47ff38 = function_11ca80(XGetLanguage());
	}
	return g_47ff38;
}

/* forgets the current language's string table of the globals tag */
static inline void main_game_string_table_unload()
{
	s_main_game_string_table *table = &((s_main_game_string_tables *)g_4e034c)->tables[current_language()];

	if (table->loaded)
	{
		table->references = NULL;
		table->data = NULL;
		table->loaded = false;
	}
}

/* unloads the map's caches and tags and pops its physical memory stage */
// @retail 0x18ed00
void function_18ed00(bool unload)
{
	if (unload)
	{
		function_2186f0();
		texture_cache_dispose_from_old_map();
		geometry_cache_dispose_from_old_map();

		main_game_string_table_unload();
		function_1233f0();
		cache_files_dispose_map();
		g_4686c0 = NONE;
		g_4e0350 = NULL;
		g_4e034c = NULL;
		g_4686c4 = NONE;
		g_4e0348 = NULL;
		g_4e0344 = NULL;
		g_4e0340 = NULL;
		g_4e033c = NULL;
	}
	g_global_f9ae07.field_0--;
	g_4ed294 = 0;
}

char g_4ed298[0x104];

static inline char *function_x91aa57(char *destination, char const *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

/* unloads the map's caches and forgets the map's name */
// @retail 0x18edb0
void function_18edb0(void)
{
	function_2186f0();
	texture_cache_dispose_from_old_map();
	geometry_cache_dispose_from_old_map();
	main_game_string_table_unload();
	function_1233f0();
	g_4ed294 = 3;
	function_x91aa57(g_4ed298, "", sizeof(g_4ed298));
}

struct s_saved_game_header
{
	long version;
	char type;
	byte unknown05[0x12c - 5];
	bool flag12c;
};

// @retail 0x18eeb0
long function_18eeb0(s_saved_game_header const *header)
{
	long result = 1;

	if (header && header->version == 1 && !(header->type >= 4 && header->type <= 5))
	{
		if (online_logon_connected())
		{
			result = (header->flag12c != 0) + 2;
		}
		else
		{
			result = (header->flag12c != 0) + 4;
		}
	}
	return result;
}

struct s_callback_pair
{
	void (*dispose)(void);
	void (__stdcall *initialize)(long stage);
};

extern s_callback_pair g_453c00[8];
long g_4ed290;

// @retail 0x18ef00
void function_18ef00(s_saved_game_header const *header)
{
	long stage = function_18eeb0(header);

	if (g_4ed290 != stage)
	{
		long i;

		if (g_4ed290 > 0)
		{
			for (i = 0; i < sizeof(g_453c00) / sizeof(g_453c00[0]); i++)
			{
				if (g_453c00[i].dispose)
				{
					g_453c00[i].dispose();
				}
			}
			g_global_f9ae07.field_0--;
		}
		function_xe0ae94();
		g_4ed290 = stage;
		for (i = 0; i < sizeof(g_453c00) / sizeof(g_453c00[0]); i++)
		{
			if (g_453c00[i].initialize)
			{
				g_453c00[i].initialize(stage);
			}
		}
	}
}
/* the options a game starts with (0x1118 bytes) */
struct s_game_options
{
	long field_0_2;
	byte unknown04;
	bool flag05;
	byte unknown06[0x264 - 6];
	long difficulty;
	byte unknown268[0x1118 - 0x268];
};

/* unknown_12be90.cpp */
extern bool g_4ed39d;
extern bool g_4ed39e;
extern dword g_4ed3a0;
s_game_options g_4ed3a8;

void function_593e0(void);

// @retail 0x18e790
void function_18e790(s_game_options const *options)
{
	if (options)
	{
		g_4ed3a8 = *options;
	}
	g_4ed39e = options == NULL;
	g_4ed39d = true;
	g_4ed3a0 = GetTickCount();

	if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
	{
		if (options)
		{
			if (options->flag05)
			{
				return;
			}
			if (options->field_0_2 == 3 && options->difficulty >= 2 && options->difficulty <= 6)
			{
				return;
			}
		}
		function_593e0();
	}
}
