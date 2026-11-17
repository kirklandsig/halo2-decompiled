/* CACHE_FILE_TABLE.H: the open cache files, the current one and the maps
   copied to the utility drive (0x557c90, 0x385c bytes). Retail holds them as
   one object: its static initializer constructs it whole (0x2135c0) and
   function_2136c0 clears it with one memset. Include it after the source's
   own view of a cache file (s_cache_file, 0x804 bytes). */
#ifndef CACHE_FILE_TABLE_H
#define CACHE_FILE_TABLE_H

#define MAXIMUM_CACHE_FILES 6
#define MAXIMUM_CACHE_COPIES 8

/* a map copied to the utility drive (0x108 bytes) */
struct s_cache_copy
{
	char path[256];
	long last_used;
	long state;
};

struct s_cache_file_table
{
	s_cache_file files[MAXIMUM_CACHE_FILES];
	long current;
	s_cache_copy copies[MAXIMUM_CACHE_COPIES];
};

typedef char assert_cache_file_table_current[offsetof(s_cache_file_table, current) == 0x3018 ? 1 : -1];
typedef char assert_cache_file_table_size[sizeof(s_cache_file_table) == 0x385c ? 1 : -1];

extern s_cache_file_table g_557c90;

#endif
