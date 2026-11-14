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
extern long g_55c154;

void function_125d60(void);
bool function_216800(long file_index, void *location);
const char *saved_game_type_get_storage_name(long type);
char *function_122810(char *path, const char *suffix);
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path);
bool function_2162b0(long file_index, void *buffer, long size, s_saved_game_file_task *task);
bool function_216d70(char *path, const char *directory, long type);

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
// Round 134 (fast): the path built by calling the existing 0x216d70 (copy, terminate, append; LTCG inlines
// it here as retail does) 10, kept in place of its body written out, with its type argument named first 10;
// the profile copied from the bounded index 110 (309 bytes); result set before the task stores 20; progress
// stored first 13; state first 12; succeeded first 11.
// Round 138: if on the path helper's bool 10; `&&` of helper and read 23 (290 bytes); cache pointer before the bit
// test 17; inverted first test 10; separate result assignment 10.
// Round 53 (m3 lane 15, fast): the read's path as `&local_0.field_44[0]` 10; the index and size tests nested
// 10; the size test first 75 (291 bytes, re-measured); the profile copied from `&...profile` 10; the location
// record declared at function scope 10; the read's result returned directly from the else arm 102 (271 bytes;
// 0x2161d0 33). Retail's other callers of 0x122810 (0x2164c0, 0x216f80, 0x2ad020) do not reuse the string
// register after it either, but none passes the path again before other code.
// Round 115: explicit cached exit 10; branch-local cache pointer 10; helper result in result/named bool 10/10;
// separate read result 10; named cached predicate 10; result before the copy 46; result from done 10.
// NOT kept: no gain; the one-use result forms were exact-only and the original body is restored.
// Round 65 (m3 lane 14, fast): a built flag read after a join 10 (same code, threaded early); an inline path helper with its own bool
// result 68 (257 bytes, not inlined); `if (!helper) {}` around the build 10; empty tests between the build and the read 10/10; the read's
// path as `(const char *)&local + 0x44` 10; `succeeded` as byte 10 (0x2161d0 18, lost); `state` as dword 10; `progress` through a long
// field 10. `done` as byte or char does not compile (the async call takes bool *). Retail evidence: 0x121570's third append reloads with
// `lea esi` only after a real join, while its second keeps ESI, so retail's `lea edx` needs a join these forms fold away.
// Round 95 (m3 lane 16, fast): the record's two clears as its constructor (retail's location record type has that
// constructor) 10; task stores done/state/succeeded/progress 12, done/succeeded/progress/state 12, done/progress/succeeded/state 13
// (the stores keep source order, `pop esi` is always hoisted); the shared location type re-measured 10. Diagnostics: the path in
// its own two-field local 10; a `_ReadWriteBarrier()` between the build and the read 10 (the reuse is register tracking, not a memory
// CSE: what unifies the inlined helper's path register with the read's address is still open); the cached index as `short` 117
// (309 bytes).
// Round 158 (fast, 0x2161d0 checked): 0x122810 made an out-of-line member of a 256-character string (diagnostic, its
// definition and 0x216d70's call changed) 10, `mov edx, esi` stays; the build arm setting `result = true` with the read
// under a later `if (result)` 10 (threaded early); the location arm as a forceinline helper 90 (0x2161d0 33); the location
// and path local as a named file-scope record 10, with the read as its inline member 10; result set before the task stores
// re-measured 20; permuter 600 s (35 tries) none better.
// Round 84 (m3 lane 11, fast): 0x216800's `long count = table->count;` local for the bound 17; the cache through the
// header's storage-cache type (builtin file count, data, sizeof bound) 10; the task's four stores through an inline
// finishing helper 10; the cache through the existing saved_game_storage_get_cache() with an entry pointer 10 (all
// same code but the first); the bounds as `index >= 0 && index < files->count` 87 (294 bytes); re-measured, the cache
// entry's data through a pointer local 10. A retail scan of matched
// functions with `rep movs; pop edi; <store>; pop esi` against `pop edi; pop esi; <store>` (0x614a0 against 0x616e0,
// 0x216800 against 0x1a6bf0): the store sits between the pops when its only later constraint is the frame release
// (`add esp`), and after both pops when it feeds a longer chain; here the stores go through EBP, which `pop ebp`
// follows, so the open question is what in retail lets the done store issue before `pop esi`.
// Round 172 (m3 lane 6, fast with 0x2161d0, which stays matched): the bit test as `file_index & 0x200000` 91 (as
// `(file_index >> 21) & 1` without the bool cast, the same code); the path build under an else arm that returns false,
// the read after it 31 (the tracker still gives `mov edx, esi`; the inverted early return compiles the same); the path
// cleared before the name 13; `index == bounded_index` 11; the cached arm's task stores before the copy 21. Same code:
// the read as the arm of a ternary on the path helper's
// result; the location call given the whole record. Of the 10, 8 follow from the 2-byte `mov edx, esi` (the four
// jumps to the failure block, the read's call and the padding shift by 2); the pop order is the other 2.
// Round 98 (m3 lane 10, fast with 0x2161d0, 0x216d70, 0x122810, 0x2acd60, 0x216800, 0x121570, which all stay matched): `inline`
// on 0x216d70's definition 10 and on 0x122810's 10 (same code); `result = helper && read` 20, the helper as the last term of
// the condition 10, `helper ? read : false` 10, the whole arm as one `&&` chain 50 (none gives the `lea`); the location and
// the path as plain separate locals (the location type 0x44 bytes) in place of the record that grouped them with a pad field
// 10, the same code, kept; a 0x40 location with a 0x104 path 14. Diagnostics, reverted: a real join (a test of a global)
// after the path helper gives retail's `lea edx, [esp + 0x50]`, a plain store there keeps `mov edx, esi`, so the reuse is a
// block-local register tracker and retail has a block boundary after the append call that leaves no code; the `done` and
// `succeeded` stores both volatile give retail's `pop edi; store; pop esi` exactly (8; `done` alone 10; a join after the
// `done` store 68): retail's task type has those flags volatile, as main's stores of `succeeded` and `unknown1` in the
// 0x2acd60 family (through casts) also show. HELD (needs a ruling on volatile in a shared type): with done, unknown1 and
// succeeded declared volatile where the task type is declared, main's six volatile casts on them and 0x2acd60's one-use
// pointer removed, every function of the nine files that use the type reads as before (fast and plain) and this one is 8:
// build/r98/held-2162b0-task-flags.patch. Not applied here.
// Round 163 (m3 lane 8, fast with 0x2161d0, which stays matched): `__assume(task != NULL)` between the path build and
// the read 10, `__assume(path[0xff] == 0)` there 10 (neither leaves a block boundary: `mov edx, esi` stays); the path
// declared before the location record 10 (same layout); `__assume(task != NULL)` ahead of the cached arm's stores 10
// (`pop esi` still first); `result = helper && read(..., &path[0])` 23 (290 bytes). Retail's 0x216da0 recomputes its
// buffer after 0x217a80 because that callee takes and changes ESI; 0x122810 keeps ESI, so the cause here is still a
// block boundary with no code.
// Also round 163: the path declared inside the read block and cleared there 102 (296 bytes; 0x2161d0 33); the record's
// two clears as `display_name[0] = name[0] = 0` 10 (same code).
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
		char path[0x100];
		location.name[0] = 0;
		location.display_name[0] = 0;
		path[0] = 0;
		if (function_216800(file_index, &location) && location.language != -1)
		{
			function_216d70(path, location.name, location.type);
			result = saved_game_file_read_begin(buffer, size, false, task, path);
		}
	}
	return result;
}
