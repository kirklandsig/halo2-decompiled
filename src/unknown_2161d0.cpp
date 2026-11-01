// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_2accd0.h"
#include <string.h>

struct s_profile_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
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
extern long g_55c154;

void function_125d60(void);
bool function_216800(long file_index, void *location);
const char *saved_game_type_get_storage_name(long type);
char *function_122810(char *path, const char *suffix);
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path);
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task);

// @retail 0x2161d0
bool function_2161d0(long file_index, void *buffer, long size)
{
	bool result = false;
	s_saved_game_file_task task;

	if (function_2162b0(file_index, buffer, size, &task))
	{
		if (!*(volatile bool *)&task.done)
		{
			while (!*(volatile bool *)&task.done)
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

// score 10 (was 10): local path address retained after append and cached ESI-pop timing remain;
// tried type at use 40f; type after copy 40f; cached result first 20f; completion bool order 11f;
// cached progress first 13f; location guard nested 10f; result in each path 10f; completion from success 11f;
// complete location object 10f.
// path address reuse and cached-completion pop order differ;
// bounded copy 10; append result local/expression 27/27;
// bounded copy plus append result 27/27; location result 53;
// copy result with original/returned read path 85/85.
// Cached direct return 89; function-scope files 17; late path pointer 10;
// append/copy first-element forms 10/10; result after done 19; state before done 12;
// shared location type 10.
// Unsigned copy size 10; profile pointer local 10; result between the task stores 10; index before the
// cache pointer 10; done flag stored last 13. Retail 0x121570 keeps esi across 0x122810, so the reload
// after it here is not a property of the callee.
// Round 85: retail 0x216da0/0x216e50 also recompute the buffer address after an ESI-argument callee
// (0x217a80); a `do {} while (false)` join after the append 10; path built in an inline helper
// returning the path 10; result set inside `if (read)` 34; chained done/success assignment 11;
// a volatile done store (diagnostic only, reverted) 10.
// Short/long/byte/signed-char completion 96/93/110/110; done/success result chains 19/10;
// bounded-index statements 10, with long completion 93; size guard first 75; shared upper bound 104.
// Round 88: the path built through the existing copy/terminate/append helper shape 10; separate
// location and path locals 19 (frame 0x140: retail keeps 4 bytes between them); a named path
// pointer 10; result read back from the task 10; the build as an inline member of the record 10.
// Round 10: separate location and plain path array locals 19 (the reuse stays: mov edx, esi); the
// append through an inline member of the record 10; through a static inline function 10; a path pointer
// for the append only 10; the storage name in its own local first 10. (Matched 0x216da0 recomputes after
// an inlined member's call; here that form keeps the reuse.)
// Round 102: a const cache pointer 10; a const path pointer 10; a multi-use location pointer 10; a location
// reference 10; the location pointer/reference each with a path pointer 10/10; a const type 10; const cached
// indices 10. Every form compiles to the baseline's code.
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
		struct
		{
			s_profile_file_location field_0;
			byte field_40[4];
			char field_44[0x100];
		} local_0;
		local_0.field_0.name[0] = 0;
		local_0.field_0.display_name[0] = 0;
		local_0.field_44[0] = 0;
		if (function_216800(file_index, &local_0.field_0) && local_0.field_0.language != -1)
		{
			long type = local_0.field_0.type;
			strncpy(local_0.field_44, local_0.field_0.name, sizeof(local_0.field_44));
			local_0.field_44[sizeof(local_0.field_44) - 1] = 0;
			function_122810(local_0.field_44, saved_game_type_get_storage_name(type));
			result = saved_game_file_read_begin(buffer, size, false, task, local_0.field_44);
		}
	}
	return result;
}
