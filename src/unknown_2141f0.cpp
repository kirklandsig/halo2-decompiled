#include "unknown_11c920.h"
// @flags /O2 /Gr
#include "async.h"
#include "unknown_122870.h"
#include <string.h>
#include "map_file_path.h"
#include "cache_copy_globals.h"

struct s_cache_file
{
	s_file_handle file;
	s_copy_header_view header;
};

extern s_cache_file g_557c90[3];

s_cache_copy_globals g_55b4f0;

long const g_448fd4[7] = {0, 2, 2, 2, 2, 2, 2};
long const g_448ff0[7] = {0x40000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000};

bool cache_header_verify(s_cache_header const *header);
void file_ref_clock_time(FILETIME *time);
dword __stdcall checksum_buffer_mmx(void const *buffer, dword buffer_size);
bool cache_copy_complete(char const *map_name);
long map_location_get(char const *map_name);
long cache_file_slot_from_type(long type);
long cache_file_choose_slot(short type, dword size);
void cache_copy_buffer_resize(long size);
void cache_copy_buffer_release(void);
void cache_copy_forget(char const *map_name);
bool map_copy_request(char const *map_name, long priority);
void __stdcall cache_copy_record_use(char const *map_name);

// score 0 (was 29): matches retail (m3 lane 1 r99, Fable 5.2 xhigh; applied by the coordinator 11-13 under the
// maintainer's standing OK for held changes that reach into another owner's file). Four things together: the cache-copy
// globals 0x55b4f0..0x55c02c as members of one object (include/cache_copy_globals.h; src/unknown_12be90.cpp's g_55bd04 and
// g_55bd08 become its state and result members, 0x12bf40 still matches), state 19 in retail's order (release, then the
// slot store), state 7's slot handle read into a named local after the header clear, and state 11's chunk as
// `g_55bd1c <= remaining ? g_55bd1c : remaining`. The scratch register of each state's temporaries carries from block to
// block, so states 7, 9 and 11 are one chain. Earlier score history: git log of this file before this change.
// @retail 0x2141f0
bool function_2141f0(void)
{
	long priority = g_448fd4[g_55b4f0.requests[0].priority];
	long state = g_55b4f0.state;
	bool success = true;

	switch (state)
	{
	case 0:
		if (g_55b4f0.requests[1].map_name[0] &&
			!cache_copy_complete(g_55b4f0.requests[1].map_name) &&
			map_location_get(g_55b4f0.requests[1].map_name) != 3)
		{
			((s_map_name const *)g_55b4f0.requests[1].map_name)->map_file_path_get(g_55b4f0.path);
			g_55b4f0.requests[0] = g_55b4f0.requests[1];
			state = 1;
		}
		memset(&g_55b4f0.requests[1], 0, sizeof(g_55b4f0.requests[1]));
		g_55b4f0.result = 0;
		break;
	case 1:
		g_55b4f0.task = function_1a0b40(g_55b4f0.path, 1, 0, 1, 1, priority, &g_55b4f0.file, &g_55b4f0.done);
		state = 2;
		break;
	case 2:
		success = g_55b4f0.file.handle != INVALID_HANDLE_VALUE;
		state = 3;
		break;
	case 3:
		memset(&g_55b4f0.header, 0, sizeof(g_55b4f0.header));
		g_55b4f0.task = function_1a0f10(g_55b4f0.file, &g_55b4f0.header, 0x800, 0, 1, priority, &g_55b4f0.bytes_transferred, &g_55b4f0.done);
		state = 4;
		break;
	case 4:
		{
			file_ref_clock_time(&g_55b4f0.header.last_used);
			strncpy(g_55b4f0.header.path, g_55b4f0.path, 0x100);
			g_55b4f0.header.path[0xff] = 0;
			success = g_55b4f0.bytes_transferred == 0x800 && cache_header_verify((s_cache_header const *)&g_55b4f0.header);
			if (success)
			{
				for (long type = 0; type < 3; type++)
				{
					if (g_55b4f0.header.dependencies_required[type])
					{
						long slot = cache_file_slot_from_type(type);

						if (!g_557c90[slot].header.path[0] ||
							CompareFileTime(&g_557c90[slot].header.source_time, &g_55b4f0.header.dependency_times[type]) != 0)
						{
							success = false;
						}
					}
				}
			}
			state = 5;
		}
		break;
	case 5:
		g_55b4f0.task = function_1a15f0(g_55b4f0.file, 1, priority, (dword *)&g_55b4f0.progress.total, &g_55b4f0.done);
		state = 6;
		break;
	case 6:
		{
			bool size_known = g_55b4f0.progress.total != (dword)NONE;
			success = size_known && g_55b4f0.progress.total > 0x800;
		}
		g_55b4f0.checksum = 0;
		state = 7;
		break;
	case 7:
		{
			long slot = cache_file_choose_slot(g_55b4f0.header.type, g_55b4f0.header.size);
			if (slot != NONE)
			{
				s_cache_file *file = &g_557c90[slot];

				g_55b4f0.slot = slot;
				memset(&file->header, 0, sizeof(file->header));
				s_file_handle handle = file->file;
				g_55b4f0.task = function_1a1050(handle, &file->header, 0x800, 0, s_file_write_flags(0), 1, priority, &g_55b4f0.bytes_transferred, &g_55b4f0.done);
				state = 8;
			}
		}
		break;
	case 8:
		success = g_55b4f0.bytes_transferred == 0x800;
		state = 9;
		break;
	case 9:
		{
			s_cache_file *file = &g_557c90[g_55b4f0.slot];

			g_55b4f0.task = async_flush_file(file->file, 1, priority, &g_55b4f0.done);
		}
		state = 10;
		break;
	case 10:
		g_55b4f0.bytes_transferred = 0;
		g_55b4f0.progress.completed = 0x800;
		state = 11;
		break;
	case 11:
		cache_copy_buffer_resize(g_448ff0[g_55b4f0.requests[0].priority]);
		if (g_55b4f0.buffer)
		{
			long remaining = g_55b4f0.progress.total - g_55b4f0.progress.completed;
			long chunk = g_55b4f0.buffer_size <= remaining ? g_55b4f0.buffer_size : remaining;
			g_55b4f0.chunk_size = chunk;
			g_55b4f0.task = async_copy_position(g_55b4f0.file, g_557c90[g_55b4f0.slot].file, (void *)g_55b4f0.buffer,
				chunk, g_55b4f0.progress.completed, g_55b4f0.progress.completed, 1, priority, &g_55b4f0.bytes_transferred, &g_55b4f0.done);
			state = 12;
		}
		else
		{
			state = 11;
		}
		break;
	case 12:
		success = (dword)g_55b4f0.chunk_size == g_55b4f0.bytes_transferred;
		g_55b4f0.checksum ^= checksum_buffer_mmx((void const *)g_55b4f0.buffer, g_55b4f0.bytes_transferred);
		if (success)
		{
			g_55b4f0.progress.completed += g_55b4f0.bytes_transferred;
			if ((dword)g_55b4f0.progress.completed >= g_55b4f0.progress.total)
			{
				success = g_55b4f0.checksum == g_55b4f0.header.checksum;
				state = 13;
			}
			else
			{
				state = 11;
			}
		}
		if (g_55b4f0.reclaim_requested)
		{
			cache_copy_buffer_release();
			g_55b4f0.reclaim_requested = false;
		}
		break;
	case 13:
		{
			s_cache_file *file = &g_557c90[g_55b4f0.slot];

			g_55b4f0.task = async_flush_file(file->file, 1, priority, &g_55b4f0.done);
		}
		state = 14;
		break;
	case 14:
		{
			s_cache_file *file = &g_557c90[g_55b4f0.slot];
			file_ref_clock_time(&g_55b4f0.header.last_used);
			s_file_handle handle = file->file;
			g_55b4f0.task = function_1a1050(handle, &g_55b4f0.header, 0x800, 0, s_file_write_flags(0), 1, 0, &g_55b4f0.bytes_transferred, &g_55b4f0.done);
			state = 15;
		}
		break;
	case 15:
		success = g_55b4f0.bytes_transferred == 0x800;
		state = 16;
		break;
	case 16:
		memcpy(&g_557c90[g_55b4f0.slot].header, &g_55b4f0.header, sizeof(g_55b4f0.header));
		memset(&g_55b4f0.header, 0, sizeof(g_55b4f0.header));
		g_55b4f0.result = 1;
		state = 17;
		break;
	case 17:
		state = 18;
		break;
	case 18:
		if (g_55b4f0.file.handle != INVALID_HANDLE_VALUE)
		{
			g_55b4f0.task = function_1a1550(g_55b4f0.file, 0, 6, &g_55b4f0.done);
		}
		state = 19;
		break;
	case 19:
		{
			s_cache_copy_request request = g_55b4f0.requests[0];

			cache_copy_buffer_release();
			g_55b4f0.slot = NONE;
			memset(&g_55b4f0.header, 0, sizeof(g_55b4f0.header));
			memset(&g_55b4f0.requests[0], 0, sizeof(g_55b4f0.requests[0]));
			g_55b4f0.path[0] = 0;
			g_55b4f0.file.handle = INVALID_HANDLE_VALUE;
			switch (g_55b4f0.result)
			{
			case 1:
				cache_copy_forget(request.map_name);
				break;
			case 3:
				if (!cache_copy_complete(request.map_name))
				{
					map_copy_request(request.map_name, request.priority);
				}
				break;
			}
			state = 0;
		}
		break;
	default:
		__assume(0);
	}
	if (success)
	{
		g_55b4f0.state = state;
	}
	else
	{
		cache_copy_record_use(g_55b4f0.path);
		g_55b4f0.state = 17;
		g_55b4f0.result = 3;
	}
	return success;
}
