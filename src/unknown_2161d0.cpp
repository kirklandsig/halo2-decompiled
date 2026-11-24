// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_2accd0.h"
#include "saved_game_storage.h"
#include <string.h>

struct s_profile_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
	/* four more bytes follow the record in every retail frame that holds one (here, 0x215c00, 0x215e60) */
	byte unknown40[4];
};

struct s_cached_profile_files
{
	long count;
	struct
	{
		long unknown0;
		byte profile[0x1e0];
	} entries[1];
};

extern void *g_51ea14;

void function_125d60(void);
bool function_216800(long file_index, void *location);
const char *saved_game_type_get_storage_name(long type);
char *function_122810(char *path, const char *suffix);
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path);
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task);
bool function_216d70(const char *directory, char *path, long type);

/* a saved game file's path, built from its directory and type */
struct s_saved_game_path_2162b0
{
	char text[0x100];

	bool build(const char *directory, long type)
	{
		return function_216d70(directory, text, type);
	}
};

// @retail 0x2161d0
bool function_2161d0(long file_index, void *buffer, long size)
{
	bool result = false;
	s_saved_game_file_task task;

	if (function_2162b0(file_index, buffer, size, &task))
	{
		if (!task.done)
		{
			while (!task.done)
			{
				SwitchToThread();
				function_125d60();
			}
		}
		result = task.succeeded;
		if (!result)
			g_55c154 = task.state;
	}
	return result;
}

// @retail 0x2162b0
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task)
{
	bool result = false;

	if ((bool)(((dword)file_index >> 21) & 1) && g_51ea14)
	{
		s_cached_profile_files *files = (s_cached_profile_files *)g_51ea14;
		long index = (file_index >> 8) & 0x1fff;
		long bounded_index = index < 0 ? 0 : index > files->count - 1 ? files->count - 1 : index;
		if (bounded_index == index && (dword)size <= 0x1e0)
		{
			memcpy(buffer, files->entries[index].profile, size);
			task->done = true;
			task->succeeded = true;
			task->state = 0;
			task->progress = 1.0f;
			result = true;
		}
	}
	else
	{
		s_profile_file_location location;
		s_saved_game_path_2162b0 path;
		location.name[0] = 0;
		location.display_name[0] = 0;
		path.text[0] = 0;
		if (function_216800(file_index, &location) && location.language != -1)
		{
			path.build(location.name, location.type);
			result = saved_game_file_read_begin(buffer, size, false, task, path.text);
		}
	}
	return result;
}
