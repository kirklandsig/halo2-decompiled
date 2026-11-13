/* CACHE_COPY_GLOBALS.H: the state of the copy of a map into the cache
   (0x55b4f0, 0xb40 bytes). Retail holds it as one object: function_2136c0
   clears it whole, and function_2141f0 keeps its stores in order around the
   clears of the header. */
#ifndef CACHE_COPY_GLOBALS_H
#define CACHE_COPY_GLOBALS_H

#include "unknown_11c920.h"
#include <stddef.h>
#include <xtl.h>
#include "job_queue.h"

/* a map waiting to be copied */
struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

/* the bytes copied so far and the size of the map */
struct s_cache_copy_progress
{
	long completed;
	volatile dword total;
};

/* the parts of a map's header the copy reads */
struct s_copy_header_view
{
	long signature;
	long version;
	long size;
	byte unknown00c[0x20 - 0xc];
	char path[256];
	byte unknown120[0x20];
	short type;
	byte unknown142[0x14c - 0x142];
	FILETIME last_used;
	byte unknown154[0x174 - 0x154];
	bool dependencies_required[3];
	byte unknown177;
	FILETIME source_time;
	FILETIME dependency_times[3];
	byte unknown198[0x2d0 - 0x198];
	dword checksum;
	byte unknown2d4[0x800 - 0x2d4];
};

struct s_cache_copy_globals
{
	s_copy_header_view header;
	dword bytes_transferred;
	long chunk_size;
	s_cache_copy_progress progress;
	long task;
	long state;
	long result;
	bool done;
	byte unknown81d[3];
	long slot;
	s_file_handle file;
	long buffer;
	long buffer_size;
	bool reclaim_requested;
	char path[0x103];
	/* the map being copied and the map waiting to be */
	s_cache_copy_request requests[2];
	dword checksum;
};

typedef char assert_cache_copy_globals_state[offsetof(s_cache_copy_globals, state) == 0x814 ? 1 : -1];
typedef char assert_cache_copy_globals_path[offsetof(s_cache_copy_globals, path) == 0x831 ? 1 : -1];
typedef char assert_cache_copy_globals_requests[offsetof(s_cache_copy_globals, requests) == 0x934 ? 1 : -1];
typedef char assert_cache_copy_globals_size[sizeof(s_cache_copy_globals) == 0xb40 ? 1 : -1];

extern s_cache_copy_globals g_55b4f0;

#endif
