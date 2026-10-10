#include "unknown_11c920.h"
// @flags /O2 /Gr
#include "async.h"
#include "unknown_122870.h"
#include <string.h>

struct s_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

struct s_cache_copy_progress
{
	long completed;
	volatile dword total;
};

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

extern s_cache_file g_557c90[3];
extern s_cache_copy_request g_55be24[2];
extern s_cache_copy_progress g_55bcf8;
extern long g_55bd04;
extern long g_55bd08;
extern bool g_55bd0c;
extern long g_55bd18;
extern long g_55bd1c;
extern bool g_55bd20;
extern char g_55bd21[0x103];

s_copy_header_view g_55b4f0;
dword g_55bcf0;
long g_55bcf4;
long g_55bd00;
long g_55bd10;
s_file_handle g_55bd14;
dword g_55c02c;

long const g_448fd4[7] = {0, 2, 2, 2, 2, 2, 2};
long const g_448ff0[7] = {0x40000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000};

bool cache_header_verify(s_cache_header const *header);
void function_136e70(FILETIME *time);
dword __stdcall checksum_buffer_mmx(void const *buffer, dword buffer_size);
bool cache_copy_complete(char const *map_name);
long map_location_get(char const *map_name);
void map_file_path_get(char const *map_name, char *path);
long cache_file_slot_from_type(long type);
long cache_file_choose_slot(short type, dword size);
void cache_copy_buffer_resize(long size);
void cache_copy_buffer_release(void);
void cache_copy_forget(char const *map_name);
bool map_copy_request(char const *map_name, long priority);
void __stdcall cache_copy_record_use(char const *map_name);

__forceinline s_file_handle copy_slot_handle(long index)
{
	return *(s_file_handle *)&g_557c90[index].handle;
}

// @retail 0x2141f0
bool function_2141f0(void)
{
	long priority = g_448fd4[g_55be24[0].priority];
	long state = g_55bd04;
	bool success = true;

	switch (state)
	{
	case 0:
		if (g_55be24[1].map_name[0] &&
			!cache_copy_complete(g_55be24[1].map_name) &&
			map_location_get(g_55be24[1].map_name) != 3)
		{
			map_file_path_get(g_55be24[1].map_name, g_55bd21);
			g_55be24[0] = g_55be24[1];
			state = 1;
		}
		memset(&g_55be24[1], 0, sizeof(g_55be24[1]));
		g_55bd08 = 0;
		break;
	case 1:
		g_55bd00 = function_1a0b40(g_55bd21, 1, 1, 0, 1, priority, &g_55bd14, &g_55bd0c);
		state = 2;
		break;
	case 2:
		success = g_55bd14.handle != INVALID_HANDLE_VALUE;
		state = 3;
		break;
	case 3:
		memset(&g_55b4f0, 0, sizeof(g_55b4f0));
		g_55bd00 = function_1a0f10(g_55bd14, &g_55b4f0, 0x800, 0, 1, priority, &g_55bcf0, &g_55bd0c);
		state = 4;
		break;
	case 4:
		{
			function_136e70(&g_55b4f0.last_used);
			strncpy(g_55b4f0.path, g_55bd21, 0x100);
			g_55b4f0.path[0xff] = 0;
			success = g_55bcf0 == 0x800 && cache_header_verify((s_cache_header const *)&g_55b4f0);
			if (success)
			{
				for (long type = 0; type < 3; type++)
				{
					if (g_55b4f0.dependencies_required[type])
					{
						s_copy_header_view const *header = (s_copy_header_view const *)g_557c90[cache_file_slot_from_type(type)].unknown04;
						if (!header->path[0] || CompareFileTime(&header->source_time, &g_55b4f0.dependency_times[type]) != 0)
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
		g_55bd00 = function_1a15f0(g_55bd14, 1, priority, (dword *)&g_55bcf8.total, &g_55bd0c);
		state = 6;
		break;
	case 6:
		success = g_55bcf8.total != (dword)NONE && g_55bcf8.total > 0x800;
		g_55c02c = 0;
		state = 7;
		break;
	case 7:
		{
			long slot = cache_file_choose_slot(g_55b4f0.type, g_55b4f0.size);
			if (slot != NONE)
			{
				g_55bd10 = slot;
				memset(g_557c90[slot].unknown04, 0, 0x800);
				g_55bd00 = function_1a1050(copy_slot_handle(slot), g_557c90[slot].unknown04, 0x800, 0, 0, 1, priority, &g_55bcf0, &g_55bd0c);
				state = 8;
			}
		}
		break;
	case 8:
		success = g_55bcf0 == 0x800;
		state = 9;
		break;
	case 9:
		g_55bd00 = async_flush_file(copy_slot_handle(g_55bd10), 1, priority, &g_55bd0c);
		state = 10;
		break;
	case 10:
		g_55bcf0 = 0;
		g_55bcf8.completed = 0x800;
		state = 11;
		break;
	case 11:
		cache_copy_buffer_resize(g_448ff0[g_55be24[0].priority]);
		if (g_55bd18)
		{
			long remaining = g_55bcf8.total - g_55bcf8.completed;
			g_55bcf4 = g_55bd1c < remaining ? g_55bd1c : remaining;
			g_55bd00 = async_copy_position(g_55bd14, copy_slot_handle(g_55bd10), (void *)g_55bd18,
				g_55bcf4, g_55bcf8.completed, g_55bcf8.completed, 1, priority, &g_55bcf0, &g_55bd0c);
			state = 12;
		}
		break;
	case 12:
		success = g_55bcf0 == (dword)g_55bcf4;
		g_55c02c ^= checksum_buffer_mmx((void const *)g_55bd18, g_55bcf0);
		if (success)
		{
			g_55bcf8.completed += g_55bcf0;
			if ((dword)g_55bcf8.completed >= g_55bcf8.total)
			{
				success = g_55c02c == g_55b4f0.checksum;
				state = 13;
			}
			else
			{
				state = 11;
			}
		}
		if (g_55bd20)
		{
			cache_copy_buffer_release();
			g_55bd20 = false;
		}
		break;
	case 13:
		g_55bd00 = async_flush_file(copy_slot_handle(g_55bd10), 1, priority, &g_55bd0c);
		state = 14;
		break;
	case 14:
		{
			s_cache_file *file = &g_557c90[g_55bd10];
			function_136e70(&g_55b4f0.last_used);
			g_55bd00 = function_1a1050(*(s_file_handle *)&file->handle, &g_55b4f0, 0x800, 0, 0, 1, 0, &g_55bcf0, &g_55bd0c);
			state = 15;
		}
		break;
	case 15:
		success = g_55bcf0 == 0x800;
		state = 16;
		break;
	case 16:
		memcpy(g_557c90[g_55bd10].unknown04, &g_55b4f0, 0x800);
		memset(&g_55b4f0, 0, sizeof(g_55b4f0));
		g_55bd08 = 1;
		state = 17;
		break;
	case 17:
		state = 18;
		break;
	case 18:
		if (g_55bd14.handle != INVALID_HANDLE_VALUE)
		{
			g_55bd00 = function_1a1550(g_55bd14, 0, 6, &g_55bd0c);
		}
		state = 19;
		break;
	case 19:
		{
			char request_name[256];
			long request_priority = g_55be24[0].priority;
			memcpy(request_name, g_55be24[0].map_name, sizeof(request_name));
			cache_copy_buffer_release();
			g_55bd10 = NONE;
			memset(&g_55b4f0, 0, sizeof(g_55b4f0));
			memset(&g_55be24[0], 0, sizeof(g_55be24[0]));
			g_55bd21[0] = 0;
			g_55bd14.handle = INVALID_HANDLE_VALUE;
			switch (g_55bd08)
			{
			case 1:
				cache_copy_forget(request_name);
				break;
			case 3:
				if (!cache_copy_complete(request_name))
				{
					map_copy_request(request_name, request_priority);
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
		g_55bd04 = state;
	}
	else
	{
		cache_copy_record_use(g_55bd21);
		g_55bd04 = 17;
		g_55bd08 = 3;
	}
	return success;
}
