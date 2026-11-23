/* SAVED_GAME_STORAGE.H: the saved game files (profiles, game variants and
   playlists) on the hard disk and memory units, the cache that lists them
   and the state the subsystem keeps (src/saved_game_storage.cpp) */

#ifndef SAVED_GAME_STORAGE_H
#define SAVED_GAME_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* what a saved game file holds; the low nibble of a file index. The values
   index the file-name and type-name tables. */
enum e_saved_game_type
{
	_saved_game_type_profile,
	_saved_game_type_slayer,
	_saved_game_type_king_of_the_hill,
	_saved_game_type_race,
	_saved_game_type_oddball,
	_saved_game_type_juggernaut,
	_saved_game_type_headhunter,
	_saved_game_type_capture_the_flag,
	_saved_game_type_assault,
	_saved_game_type_territories,
	_saved_game_type_playlist,

	k_saved_game_file_type_count,

	/* the game variant types run from slayer to territories */
	k_saved_game_file_first_variant_type = _saved_game_type_slayer,
	k_saved_game_file_last_variant_type = _saved_game_type_territories
};

/* one saved game found on a storage device: its directory and display name
   (the display name carries the type's localized name as a prefix) */
struct s_saved_game_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
};
typedef char assert_saved_game_file_location_size[(sizeof(s_saved_game_file_location) == 0x40) ? 1 : -1];
typedef char assert_saved_game_file_location_type[(offsetof(s_saved_game_file_location, type) == 0x38) ? 1 : -1];

enum
{
	k_saved_game_builtin_file_count = 101,
	k_saved_game_location_count = 0x1000
};

/* a file that lives in memory instead of on a device (file index bit 21):
   a profile (type 0) or a game variant */
struct s_saved_game_builtin_file
{
	long type;
	byte data[0x1e0];
};
typedef char assert_saved_game_builtin_file_size[(sizeof(s_saved_game_builtin_file) == 0x1e4) ? 1 : -1];

struct s_saved_game_location_table
{
	long count;
	s_saved_game_file_location entries[k_saved_game_location_count];
};
typedef char assert_saved_game_location_table_size[(sizeof(s_saved_game_location_table) == 0x40004) ? 1 : -1];

/* walks one storage device's list of files: get copies the current file's
   location, and the caller advances the index */
struct s_saved_game_location_iterator
{
	s_saved_game_location_table *table;
	long index;
};

/* copies the current file's location, false past the last */
inline bool saved_game_location_iterator_get(s_saved_game_location_iterator *iterator, s_saved_game_file_location *location)
{
	bool result = false;

	if (iterator->index < iterator->table->count)
	{
		*location = iterator->table->entries[iterator->index];
		result = true;
	}
	return result;
}

/* the cache g_51ea14 points to, allocated by 0x215880: the built-in files,
   the files found on the hard disk (memory unit 0), the generation stamped
   into file indices, and a buffer for copying files */
struct s_saved_game_storage_cache
{
	long builtin_file_count;
	s_saved_game_builtin_file builtin_files[k_saved_game_builtin_file_count];
	s_saved_game_location_table locations;
	long generation;
	byte copy_buffer[0x20000];
};
typedef char assert_saved_game_storage_cache_size[(sizeof(s_saved_game_storage_cache) == 0x6bf00) ? 1 : -1];
typedef char assert_saved_game_storage_cache_locations[(offsetof(s_saved_game_storage_cache, locations) == 0xbef8) ? 1 : -1];
typedef char assert_saved_game_storage_cache_generation[(offsetof(s_saved_game_storage_cache, generation) == 0x4befc) ? 1 : -1];
typedef char assert_saved_game_storage_cache_copy_buffer[(offsetof(s_saved_game_storage_cache, copy_buffer) == 0x4bf00) ? 1 : -1];

/* g_55c160_table: up to 4 files with an entry each (the lock follows the
   fourth entry in the subsystem's state, 0x55c274) */
struct s_55c164
{
	void *field0;
	byte unknown04[0x40];
};
/* the count and the entries are one structure: 0x2172a0 reads the count
   again after copying an entry */
struct s_55c160
{
	long count;
	s_55c164 entries[4];
};
typedef char assert_55c160_size[(sizeof(s_55c160) == 0x114) ? 1 : -1];

/* a saved-game file lock: free flag, the locked file's index and the owning
   thread (handed out by address in src/unknown_217370.cpp) */
struct s_saved_game_file_lock
{
	bool free;
	long file_index;
	dword thread;
};
typedef char assert_saved_game_file_lock_size[(sizeof(s_saved_game_file_lock) == 0xc) ? 1 : -1];

/* the cache, NULL until 0x215880 allocates it */
extern void *g_51ea14;
/* the subsystem's state (0x55c030..0x55c284): one structure, which
   saved_game_storage_initialize clears in one go (0x2156a9) */
struct s_saved_game_storage_globals
{
	/* no code reads these first 0x11c bytes */
	byte reserved[0x11c];
	/* g_55c14c: the flag the subsystem's dispose waits on */
	bool unknown11c;
	/* g_55c14d: set while the subsystem is initialized */
	bool unknown11d;
	byte unknown11e;
	byte unknown11f;
	/* g_55c150, bit 0: the hard disk's files need listing again */
	long unknown120;
	/* g_55c154: the last error */
	long unknown124;
	/* g_55c158: the string list that holds the types' names, NONE when there is none */
	long unknown128;
	/* g_55c15c: the generation the next cache gets */
	long unknown12c;
	s_55c160 table;
	s_saved_game_file_lock file_lock;
	/* g_55c280: the language new files' names are in */
	long language;
};
typedef char assert_saved_game_storage_globals_size[(sizeof(s_saved_game_storage_globals) == 0x254) ? 1 : -1];
typedef char assert_saved_game_storage_globals_table[(offsetof(s_saved_game_storage_globals, table) == 0x130) ? 1 : -1];
typedef char assert_saved_game_storage_globals_file_lock[(offsetof(s_saved_game_storage_globals, file_lock) == 0x244) ? 1 : -1];
extern s_saved_game_storage_globals g_55c030;

/* the state's members by their retail addresses */
#define g_55c14c g_55c030.unknown11c
#define g_55c14d g_55c030.unknown11d
#define g_55c14e g_55c030.unknown11e
#define g_55c14f g_55c030.unknown11f
#define g_55c150 g_55c030.unknown120
#define g_55c154 g_55c030.unknown124
#define g_55c158 g_55c030.unknown128
#define g_55c15c g_55c030.unknown12c
#define g_55c160_table g_55c030.table
#define g_saved_game_file_lock g_55c030.file_lock
#define g_55c280 g_55c030.language

/* written only by the subsystem's initialize (0x215690) and dispose (0x215730),
   which both clear it */
struct s_51e9f8
{
	dword field_0;
	bool field_4;
};
extern s_51e9f8 g_51e9f8;

inline s_saved_game_storage_cache *saved_game_storage_get_cache(void)
{
	return (s_saved_game_storage_cache *)g_51ea14;
}

void saved_game_storage_initialize(void);
void saved_game_storage_update(void);
bool saved_game_file_read_synchronous(long file_index, void *buffer, long size, wchar_t *display_name);
bool __stdcall saved_game_storage_ensure_directory(char const *path);
void saved_game_storage_remove_location(long file_index);
const char *saved_game_type_get_storage_name(long type);
long saved_game_type_get_name_id(long type);
bool saved_game_file_write_save_image(long type, s_saved_game_file_location *location);
wchar_t const *saved_game_storage_get_default_profile_name(long language);
wchar_t const *saved_game_type_get_profile_name(long language);
wchar_t const *saved_game_type_get_slayer_name(long language);
wchar_t const *saved_game_type_get_king_of_the_hill_name(long language);
wchar_t const *saved_game_type_get_race_name(long language);
wchar_t const *saved_game_type_get_oddball_name(long language);
wchar_t const *saved_game_type_get_juggernaut_name(long language);
wchar_t const *saved_game_type_get_headhunter_name(long language);
wchar_t const *saved_game_type_get_capture_the_flag_name(long language);
wchar_t const *saved_game_type_get_assault_name(long language);
wchar_t const *saved_game_type_get_territories_name(long language);
wchar_t const *saved_game_type_get_playlist_name(long language);

/* writes a file's name: the type's localized name and ": " when the language
   has one, then the display name (0x216da0) */
bool function_216da0(long type, wchar_t *name, wchar_t const *display_name, long language);

/* a file name buffer, filled by 0x216da0 (0x215c00, 0x215e60, 0x215f40, 0x216120) */
struct s_saved_game_name
{
	word text[128];

	bool fill(long type, word const *display_name, long language)
	{
		return function_216da0(type, text, display_name, language);
	}
};

#endif
