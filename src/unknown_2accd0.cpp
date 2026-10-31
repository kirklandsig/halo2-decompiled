// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2ACCD0.CPP: saved game files on the Xbox hard disk: signed reads
   and writes run as asynchronous tasks, and copying files */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_2accd0.h"
#include "async.h"
#include "language.h"

/* the location and type of a saved game file (0x40 bytes) */
struct s_saved_game_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
};

struct s_saved_game_file_id
{
	char data[0x14];
};

enum
{
	_saved_game_file_copy_create = 0,
	_saved_game_file_copy_open,
	_saved_game_file_copy_read,
	_saved_game_file_copy_write,
	_saved_game_file_copy_finish,
	_saved_game_file_copy_failed,
	_saved_game_file_copy_done
};

/* a saved game file (0xa0 bytes), and the state of copying it in chunks */
struct s_saved_game_file
{
	long flags;
	s_saved_game_file_location location;
	wchar_t display_name[0x11];
	s_saved_game_file_id id;
	bool unknown7a;
	byte unknown7b;
	bool temporary;
	byte unknown7d[3];
	bool *done;
	HANDLE source;
	HANDLE destination;
	void *buffer;
	dword size;
	dword read_offset;
	dword write_offset;
	long copy_state;
};

struct s_saved_game_file_read_parameters
{
	void *buffer;
	dword size;
	bool non_roamable;
	s_saved_game_file_task *task;
};

#define FILE_COPY_CHUNK_SIZE 0x20000
#define MIN(a, b) ((a) > (b) ? (b) : (a))

const char *saved_game_type_get_storage_name(long type);
long function_217480(long type);
char *function_122810(char *path, const char *name);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
bool function_1249f0(long memory_unit, char *drive_letter);
bool function_1367d0(s_type_acf665 *file);
bool file_ref_truncate(s_type_acf665 *file, dword position);
bool function_216da0(long type, wchar_t *name, const wchar_t *display_name, long language);
bool saved_game_file_write_save_image(long type, s_saved_game_file_location *location);
bool function_2168b0(s_saved_game_file_location *location, long flags);

/* the root of the memory unit a saved game file is on, or the hard disk's */
inline void saved_game_file_get_root(const s_saved_game_file *file, char *root)
{
	char drive_letter;
	if (function_1249f0((file->flags >> 4) & 0xf, &drive_letter))
	{
		function_11c9c0(root, 8, "%c:\\", drive_letter);
	}
	else
	{
		root[0] = 0;
	}
}

void saved_game_file_read(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task);
void saved_game_file_write(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task);

inline char *function_x91aa57(char *destination, const char *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

inline void function_x454397(s_type_acf665 *file)
{
	memset(file, 0, sizeof(*file));
	file->signature = FILE_REFERENCE_SIGNATURE;
	file->location = NONE;
}

inline void function_x73bce5(s_type_acf665 *file, const char *name)
{
	if (file->flags & 1)
	{
		function_1373c0(file->path);
	}
	function_137320(file->path, name);
	file->flags |= 1;
}

inline void function_xbb02d2(s_type_acf665 *file, const char *path)
{
	function_x454397(file);
	function_x73bce5(file, path);
}

/* Bungie's argument order */
inline bool function_xe8ca68(s_type_acf665 *file, dword size, const void *buffer)
{
	return function_136d00(file, size, buffer);
}

inline bool function_xab2909(s_type_acf665 *file, dword size, bool silent, void *buffer)
{
	return file_ref_read(file, size, buffer, silent);
}

inline bool function_x8231f3(s_type_acf665 *file)
{
	bool success = false;
	if (CloseHandle(file->handle))
	{
		file->handle = 0;
		file->position = 0;
		success = true;
	}
	else
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x2accd0
PRIVATE long __stdcall saved_game_file_read_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_saved_game_file_read_parameters *parameters = (s_saved_game_file_read_parameters *)parameters_view;
	s_type_acf665 file;

	parameters->task->succeeded = false;
	parameters->task->state = 4;
	function_xbb02d2(&file, parameters->task->path);
	saved_game_file_read(&file, parameters->buffer, parameters->size, parameters->non_roamable, parameters->task);
	return 1;
}

// @retail 0x2acd60
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path)
{
	s_saved_game_file_read_parameters parameters;

	parameters.buffer = buffer;
	char *task_path = task->path;
	bool *pending = &task->unknown1;
	*pending = false;
	parameters.size = size;
	parameters.non_roamable = non_roamable;
	parameters.task = task;
	task->succeeded = false;
	task->progress = -1.0f;
	function_x91aa57(task_path, path, sizeof(task->path));
	return async_task_add_work(saved_game_file_read_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

bool saved_game_file_copy_update(s_saved_game_file *file, long *error);
void saved_game_file_get_path(const s_saved_game_file *file, char *path);

/* a task copying a saved game file (kept in the status block in place of
   its path) to the hard disk, then writing it */
inline s_saved_game_file *saved_game_file_task_get_file(s_saved_game_file_task *task)
{
	return (s_saved_game_file *)task->path;
}

// @retail 0x2acde0
PRIVATE long __stdcall saved_game_file_copy_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_saved_game_file_read_parameters *parameters = (s_saved_game_file_read_parameters *)parameters_view;
	s_saved_game_file *file = saved_game_file_task_get_file(parameters->task);
	long result = 0;
	long error;

	if (saved_game_file_copy_update(file, &error))
	{
		result = 1;
		if (!error)
		{
			char path[0x100];
			s_type_acf665 s_type_acf665;

			parameters->task->state = 4;
			path[0] = 0;
			saved_game_file_get_path(file, path);
			function_x454397(&s_type_acf665);
			function_x73bce5(&s_type_acf665, path);
			saved_game_file_write(&s_type_acf665, parameters->buffer, parameters->size, parameters->non_roamable, parameters->task);
		}
	}
	else if (error)
	{
		result = 1;
		parameters->task->state = 0;
	}
	return result;
}

// @retail 0x2acee0
bool saved_game_file_copy_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	s_saved_game_file_read_parameters parameters;

	parameters.buffer = buffer;
	parameters.size = size;
	parameters.non_roamable = non_roamable;
	parameters.task = task;
	task->unknown1 = false;
	task->succeeded = false;
	task->progress = -1.0f;
	return async_task_add_work(saved_game_file_copy_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}
// @retail 0x2acf40
s_saved_game_file *saved_game_file_new(s_saved_game_file *file, long flags, const s_saved_game_file_location *location)
{
	file->flags = flags;
	s_saved_game_file_location *file_location = &file->location;
	file_location->name[0] = 0;
	file_location->display_name[0] = 0;
	wchar_t *display_name = file->display_name;
	*display_name = 0;
	file->id.data[0] = 0;
	file->unknown7a = false;
	file->temporary = false;
	file->done = NULL;
	file->buffer = NULL;
	file->copy_state = _saved_game_file_copy_done;
	*file_location = *location;
	return file;
}

__forceinline void saved_game_file_copy_display_name(wchar_t *destination, wchar_t const *source)
{
	wcsncpy(destination, source, 0x10);
	destination[0x10] = 0;
}

// @retail 0x2acf80
s_saved_game_file *saved_game_file_new_from_id(s_saved_game_file *file, long flags, const wchar_t *display_name, const s_saved_game_file_location *location, void *buffer, bool *done)
{
	const s_saved_game_file_location *const *location_reference = &location;
	file->flags = flags;
	s_saved_game_file_location *file_location = &file->location;
	memset(file_location->name, 0, sizeof(file_location->name[0]));
	memset(file_location->display_name, 0, sizeof(file_location->display_name[0]));
	saved_game_file_copy_display_name(file->display_name, display_name);
	file->id = *(const s_saved_game_file_id *)*location_reference;
	file->done = done;
	file->temporary = false;
	file->source = INVALID_HANDLE_VALUE;
	file->destination = INVALID_HANDLE_VALUE;
	file->size = NONE;
	file->read_offset = 0;
	file->write_offset = 0;
	file->copy_state = _saved_game_file_copy_create;
	file->unknown7a = true;
	file->buffer = buffer;
	*file_location = **location_reference;
	return file;
}

/* creates the saved game's directory on the file's device */
__forceinline bool saved_game_file_create_directory(const s_saved_game_file *file, const wchar_t *name, char *path)
{
	char root[8];

	saved_game_file_get_root(file, root);
	return XCreateSaveGame(root, name, CREATE_NEW, 0, path, 0x100) == ERROR_SUCCESS;
}

// @retail 0x2ad020
bool saved_game_file_copy_create(s_saved_game_file *file)
{
	bool result = false;
	long type = file->flags & 0xf;
	long language = get_current_language();
	char path[0x100] = { 0 };
	wchar_t name[0x80];
	char file_path[0x100];
	s_saved_game_file_id id;
	s_type_acf665 s_type_acf665;

	name[0] = 0;
	function_216da0(type, name, file->display_name, language);
	if (saved_game_file_create_directory(file, name, path))
	{
		dword error;

		file_path[0] = 0;
		function_x91aa57(id.data, path, sizeof(id.data));
		function_x91aa57(file_path, id.data, sizeof(file_path));
		function_122810(file_path, saved_game_type_get_storage_name(type));
		function_xbb02d2(&s_type_acf665, file_path);
		if (function_1367d0(&s_type_acf665) && function_136970(&s_type_acf665, 2, &error))
		{
			if (file_ref_truncate(&s_type_acf665, function_217480(type)))
			{
				*(s_saved_game_file_id *)file->location.name = id;
				result = true;
			}
			function_136bb0(&s_type_acf665);
		}
	}
	return result;
}
// @retail 0x2ad380
bool file_copy_read_chunk(s_saved_game_file *copy)
{
	bool result = false;
	dword offset = copy->read_offset;
	dword remaining = copy->size - offset;
	dword size = remaining < FILE_COPY_CHUNK_SIZE ? remaining : FILE_COPY_CHUNK_SIZE;
	dword bytes;

	if (SetFilePointer(copy->source, offset, NULL, FILE_BEGIN) == copy->read_offset)
	{
		if (ReadFile(copy->source, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			result = true;
			copy->read_offset += bytes;
		}
	}
	return result;
}

// @retail 0x2ad400
bool file_copy_write_chunk(s_saved_game_file *copy)
{
	bool result = false;
	dword offset = copy->write_offset;
	dword remaining = copy->size - offset;
	dword size = remaining < FILE_COPY_CHUNK_SIZE ? remaining : FILE_COPY_CHUNK_SIZE;
	dword bytes;

	if (SetFilePointer(copy->destination, offset, NULL, FILE_BEGIN) == copy->write_offset)
	{
		if (WriteFile(copy->destination, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			result = true;
			copy->write_offset += bytes;
		}
	}
	return result;
}

// @retail 0x2ad220
bool file_copy_open(s_saved_game_file *copy, bool *exists)
{
	bool result = false;
	char source_path[0x100];
	char destination_path[0x100];

	function_x91aa57(source_path, copy->id.data, sizeof(source_path));
	function_122810(source_path, "auxilary.bin");
	copy->source = CreateFileA(source_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (copy->source == INVALID_HANDLE_VALUE)
	{
		*exists = false;
		result = true;
		goto done;
	}

	function_x91aa57(destination_path, copy->location.name, sizeof(destination_path));
	function_122810(destination_path, "auxilary.bin");
	copy->destination = CreateFileA(destination_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (copy->destination != INVALID_HANDLE_VALUE)
	{
		copy->size = GetFileSize(copy->source, NULL);
		if (copy->size != INVALID_FILE_SIZE)
		{
			SetFilePointer(copy->destination, copy->size, NULL, FILE_BEGIN);
			if (GetLastError() == ERROR_SUCCESS && SetEndOfFile(copy->destination))
			{
				*exists = true;
				result = true;
				goto done;
			}
		}
	}

	if (copy->source != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->source);
		copy->source = INVALID_HANDLE_VALUE;
	}
	if (copy->destination != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->destination);
		copy->destination = INVALID_HANDLE_VALUE;
	}
done:
	return result;
}
// score 19 (was 19): the second argument of the save call is a pointer held in a register here and
// the address of the local buffer in retail.
// @retail 0x2ad480
void saved_game_file_copy_finish(s_saved_game_file *file, bool succeeded)
{
	long type = file->flags & 0xf;
	char root[8];
	wchar_t name[0x80];

	saved_game_file_get_root(file, root);
	name[0] = 0;
	if (succeeded)
	{
		function_216da0(type, name, file->location.display_name, file->location.language);
		XDeleteSaveGame(root, name);
		saved_game_file_copy_display_name(file->location.display_name, file->display_name);
		file->location.language = (char)get_current_language();
		function_2168b0(&file->location, file->flags);
	}
	else
	{
		function_216da0(type, name, file->display_name, get_current_language());
		XDeleteSaveGame(root, name);
	}
	*file->done = true;
}
// @retail 0x2ad5b0
void file_copy_close(s_saved_game_file *copy)
{
	if (copy->source != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->source);
		copy->source = INVALID_HANDLE_VALUE;
	}
	if (copy->destination)
	{
		CloseHandle(copy->destination);
		copy->source = INVALID_HANDLE_VALUE;
	}
}

// @retail 0x2ad5f0
bool saved_game_file_copy_update(s_saved_game_file *file, long *error)
{
	bool result = false;
	bool exists;
	long type;

	*error = 0;
	switch (file->copy_state)
	{
	case _saved_game_file_copy_create:
		if (saved_game_file_copy_create(file))
			file->copy_state = _saved_game_file_copy_finish;
		else
			*error = 2;
		break;

	case _saved_game_file_copy_finish:
		type = file->flags & 0xf;
		if (saved_game_file_write_save_image(type, &file->location))
			file->copy_state = _saved_game_file_copy_open;
		else
			*error = 2;
		break;

	case _saved_game_file_copy_open:
		exists = false;
		if (file_copy_open(file, &exists))
		{
			if (exists)
			{
				file->read_offset = 0;
				file->write_offset = 0;
				file->copy_state = _saved_game_file_copy_read;
			}
			else
			{
				file->copy_state = _saved_game_file_copy_failed;
			}
		}
		else
		{
			*error = 2;
		}
		break;

	case _saved_game_file_copy_read:
		if (file_copy_read_chunk(file))
			file->copy_state = _saved_game_file_copy_write;
		else
			*error = 4;
		break;

	case _saved_game_file_copy_write:
		if (file_copy_write_chunk(file))
		{
			if (file->size > file->write_offset)
			{
				file->copy_state = _saved_game_file_copy_read;
			}
			else
			{
				file_copy_close(file);
				file->copy_state = _saved_game_file_copy_failed;
			}
		}
		else
		{
			*error = 4;
		}
		break;

	case _saved_game_file_copy_failed:
		saved_game_file_copy_finish(file, true);
		file->copy_state = _saved_game_file_copy_done;
		break;

	case _saved_game_file_copy_done:
		result = true;
		break;
	}

	if (*error)
	{
		file_copy_close(file);
		saved_game_file_copy_finish(file, false);
	}
	return result;
}
inline void saved_game_file_build_path(char *path, const char *folder, long type)
{
	strncpy(path, folder, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
}

// @retail 0x2ad760
void saved_game_file_get_path(const s_saved_game_file *file, char *path)
{
	if (file->temporary)
	{
		strncpy(path, "t:\\blam", 0x100);
		path[0xff] = 0;
	}
	else
	{
		saved_game_file_build_path(path, file->location.name, file->location.type);
	}
}

s_thread_stack g_4fa0c8;

enum
{
	_signed_file_read_header = 0,
	_signed_file_read_body,
	_signed_file_read_signature
};

/* a signed saved game file read in steps: a header, then a body, then the
   signature after both */
#pragma pack(push, 1)
struct s_signed_file_read_parameters
{
	void *header;
	dword header_size;
	byte *body;
	dword body_size;
	HANDLE file;
	dword body_offset;
	HANDLE signature_handle;
	s_saved_game_file_task *task;
	bool non_roamable;
	char state;
};
#pragma pack(pop)

/* claims the job thread's scratch buffer for a task: the buffer in use and
   its size */
__forceinline void job_thread_buffer_get(s_async_task *task, byte **buffer, dword *size)
{
	g_4fa0c8.owner = task;
	*buffer = g_4fa0c8.buffer;
	*size = g_4fa0c8.size;
	g_4fa0c8.external = *buffer != g_4fa0c8.storage;
}

// score 34 (was 53): the scratch buffer is one address-taken record (storage, owner, buffer,
// size, flag; the init points the buffer at the storage) claimed by a getter that returns the buffer
// and size through pointers; with the `void *` parameters view and one function-scope byte count the
// length, both sums' order, the dispatch hoist and the reloads after the claim are retail's. Left:
// the entry total in EAX (retail EDX) and size/position in ESI/EDI (retail EDI/ESI; the hoisted
// body_size in ESI, retail ECX): retail computes each sum into a fresh register.
// Round 14 (lane 2; 0x2adb10 in brackets): that getter as `owner; *buffer; *size; flag` 34 [129],
// kept; returning the size (`owner; *buffer; flag; return size`) 34 [138]; that with the view in
// 0x2adb10 too 34 [138]; total written body first 34; no total local 88; compile-only (exact here):
// position declared at the top of the body block or first, size declared first, size computed
// before the buffer in the body arm, one function-scope position, the clamp as MIN or a one-line if:
// 34 each; without the view or without the shared byte count 156-ish (235 instructions); a named
// remaining count in the scratch arm (also declared with size, also set in the body arm, also
// with MIN) 156-ish (235 instructions: the sums turn header first and the hoist goes).
// Earlier (53): retail sums the two sizes body first everywhere (entry `mov eax, [ebx+0xc];
// mov ecx, [ebx+4]; lea edx, [eax+ecx+0x14]`, total in EDX, task in EAX; signature case `lea esi,
// [ecx+edx]` with body_size in ECX); ours sums header first (total in EAX), so the signature case does
// not start with `mov ecx, [ebx+0xc]` like the body case and VC7.1 cannot hoist that common first
// instruction between `dec ecx` and `je` as retail does. Retail also re-reads body_offset/body_size
// after the scratch getter's stores, as if the scratch record were address-taken (its 0x8000-byte
// storage at 0x4fa0c8 sits right before the owner/pointer/size words and the flag byte at 0x5020d4).
// That record (storage, owner, buffer, size, flag; init `buffer = storage`; defined outside
// async.cpp, or 0x120a90's loop end past async_globals lands in it), claim and size helpers (claim
// stores the owner, then the flag, and returns a fresh read of the buffer) and a per-arm
// `size = MIN(buffer size, body_size - body_offset)` make the getter, reloads and whole body case
// retail's: aligned 12 left (the sum order and the hoist), but 161 positional at 689 bytes (0x2adb10
// 279, aligned 122 -> 45). No source form
// moved the sum order: written order, `+=`, a total helper (value or pointer argument), nested header/body records, signed or long size types, no packing, declarations at
// the top, case order, inline min function, each reference removed in turn; only a body_size read once
// in the whole function sums body first. Tried: the result stored as one plain `memcmp == 0` store
// 53 (kept, was an output local and a true/false if/else 59); short-lived body/header/both size reads
// 182/174/182; reversed signature equality 68; Boolean equality 68; inline depth one 68; global
// optimization off 491; frame pointer 246; per-arm remaining written into size by the getter 179,
// capacity clamped by remaining 181, remaining clamped by capacity 234; per-arm clamp without the
// address-taken record 173-177; total as sizeof first 68; body size grouped with sizeof 68; shared
// byte count, seek-position home, signature outputs and role names 68 each; the getter storing the
// owner before the flag 59 (0x2adb10 141); the getter reading the pointer at each use 168 (0x2adb10
// 278); split claim/size helpers without the record 163; size-first body test 176; body size first in
// the total, sizeof first, the total as a long or without a dword local 68 each (same code).
// Lane 13 r14: VC orders the two size loads by an internal order that follows the function's
// locals, not the written order; the record pointer as a local cast from a `void *` parameter (the
// form of this file's other work callbacks) gives the body-first total and signature sum, 51
// (aligned 31 -> 29). With the address-taken record above, that view, one function-scope byte count
// and the scratch arm `size = buffer size; if (size > remaining) size = remaining;` (remaining read
// again after the claim), retail's dispatch hoist of body_size appears and the body case is retail's
// but for ESI/EDI swapped between size and position and the total in EAX: 34 at 687 bytes (aligned
// 24), but 0x2adb10 134 -> 279 (944 bytes); not kept. The same with MIN in the scratch arm 159-161
// (689 bytes), the record alone 161, view and shared byte count without the record 53, the clamp
// without the record 180. Compile-only screens (they reproduce the full build here): no total local
// gives retail's entry but rotates every later temp (43+); partly shared byte counts 28-32; size and
// position declaration order, scope, names, a shared position, a remaining local, flipped clamp
// compares, an out-parameter getter, getter store orders: no better. On the 34 form the claim and
// size helpers as plain, static or inline functions, or in another file (inlined by the link): 34
// each, 0x2adb10 279 each.
// Lane 14 r20, with the record (0x2adb10 in brackets): the view alone 51 [133]; the current getter
// 159 [131], with the view 157 [130], with per-arm size 156 [130]; a getter returning the size with
// the buffer through `byte **` 50 [139], with the view 49 [139], plus per-arm size 36 (aligned 27:
// the total's sum order, the dispatch hoist's register and the size/position ESI/EDI swap left)
// [139]; the same getter with the flag from a fresh read 36 [280]; claim/size helpers with a local
// buffer 156/154 [130-138], with the claim returning a fresh read 34/34/36 [278-280, 944 bytes];
// that getter called out of line (plain function here or in async.cpp) 77 (672 bytes).
// DL round 206: shared header/body extent 236; retained total as signature offset 226;
// integer denominator converted at both divisions 207; per-use size fields 235;
// guarded conversion initialized/separately assigned 64/64; shared extent with per-use
// conversion/guarded conversion 233/235. Original body restored.
// Lane 16 r46: `void *` view 51; with the total converted in place 73; total operand orders on the view
// 51/51; `real total_size;` declared first 51. On the view, storage and owner/buffer/size/flag as one
// record (init `buffer = storage`) 157 (the reloads appear; 0x2adb10 131, 0x120a90 matched); plus the
// per-arm size 161 (689 B); plus `body_size > body_offset` 157 (dispatch hoist appears, into ESI); plus a
// function-scope position 157; size/position renamed 157 each: size keeps ESI, position EDI (retail
// the reverse), so the hoisted body-size load lands in the size register instead of ECX.
// Round 112 (real --fast; compile-only aligned in brackets, base [22]): retail's body arm computes the
// remaining count in ECX (the hoisted body_size) and copies it to size with a `mov edi, ecx` shared with
// the scratch clamp; ours computes it in place in size's register. One MIN after both arms (the direct arm
// setting size to the remaining count) 158 (690 bytes) [20: size EDI, position ESI as retail, but the
// clamp becomes cmova and the body case loads body_offset first, so the hoist goes]; the same with `if
// (size > remaining) size = remaining;` after the arms 158 [20, also cmova]; a capacity local for it 27
// [compile-only]; a remaining local set in both arms (size = remaining) 161 (689) [25: copy-propagated,
// sums header first]; the arms swapped (`if (!parameters->body)`) 41 (687) [24]; a named remaining in the
// scratch arm 161 (689) [25]. Compile-only, all 22 (same code): size/position at function scope in any
// order, bytes before/after total/total_size, both position sums' written operand order, the signature
// buffers at function scope or swapped; worse: the body pointer local 25, buffer initialised from the body
// 28, the total converted in place 60, the shared MIN with the total in place or the view dropped 53+.
// Round 34 (lane 13, LTCG harness, base 22 = 34): locals renamed (size as count/read_size, position
// as file_offset/seek_position, buffer, total/total_size, bytes) 34 each and the direct arm's size
// before its buffer 34 (all identical code: names do not break these register ties); the body case as
// an early-out on `body_offset >= body_size` harness 88 against 22 (682 bytes); `size == bytes` 34 (identical).
// @retail 0x2ad7c0
PRIVATE long __stdcall signed_file_read_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_signed_file_read_parameters *parameters = (s_signed_file_read_parameters *)parameters_view;
	dword bytes;
	dword total = parameters->header_size + parameters->body_size + sizeof(XCALCSIG_SIGNATURE);
	real total_size = (real)total;

	parameters->task->state = 4;
	if (!parameters->task->unknown1)
	{
		switch (parameters->state)
		{
		case _signed_file_read_header:
		{
			parameters->file = CreateFileA(parameters->task->path, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
			if (parameters->file != INVALID_HANDLE_VALUE &&
				ReadFile(parameters->file, parameters->header, parameters->header_size, &bytes, NULL) &&
				bytes == parameters->header_size)
			{
				parameters->signature_handle = XCalculateSignatureBegin(parameters->non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
				if (parameters->signature_handle != INVALID_HANDLE_VALUE &&
					(!parameters->header_size || XCalculateSignatureUpdate(parameters->signature_handle, (const BYTE *)parameters->header, parameters->header_size) == ERROR_SUCCESS))
				{
					parameters->task->progress = (real)parameters->header_size / total_size;
					parameters->state = _signed_file_read_body;
					return 0;
				}
			}
			break;
		}

		case _signed_file_read_body:
			if (parameters->body_offset < parameters->body_size)
			{
				byte *buffer;
				dword size;
				if (parameters->body)
				{
					buffer = parameters->body + parameters->body_offset;
					size = parameters->body_size - parameters->body_offset;
				}
				else
				{
					job_thread_buffer_get(task, &buffer, &size);
					size = MIN(size, parameters->body_size - parameters->body_offset);
				}

				dword position = parameters->header_size + parameters->body_offset;
				if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
					ReadFile(parameters->file, buffer, size, &bytes, NULL) && bytes == size &&
					XCalculateSignatureUpdate(parameters->signature_handle, buffer, bytes) == ERROR_SUCCESS)
				{
					parameters->body_offset += bytes;
					if (parameters->body_offset >= parameters->body_size)
					{
						parameters->state = _signed_file_read_signature;
					}
					parameters->task->progress = (real)(parameters->header_size + parameters->body_offset) / total_size;
					return 0;
				}
				break;
			}
			parameters->state = _signed_file_read_signature;
			return 0;

		case _signed_file_read_signature:
		{
			dword position = parameters->header_size + parameters->body_size;
			XCALCSIG_SIGNATURE stored;
			XCALCSIG_SIGNATURE computed;

			if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
				ReadFile(parameters->file, &stored, sizeof(stored), &bytes, NULL) && bytes == sizeof(stored))
			{
				if (XCalculateSignatureEnd(parameters->signature_handle, &computed) == ERROR_SUCCESS)
				{
					parameters->task->succeeded = memcmp(&computed, &stored, sizeof(stored)) == 0;
				}
				parameters->signature_handle = INVALID_HANDLE_VALUE;
			}
			parameters->task->progress = 1.0f;
			break;
		}

		default:
			__assume(0);
		}
	}

	if (parameters->file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(parameters->file);
	}
	if (parameters->signature_handle != INVALID_HANDLE_VALUE)
	{
		XCalculateSignatureEnd(parameters->signature_handle, NULL);
	}
	return 1;
}

// @retail 0x2ada70
bool signed_file_read_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task)
{
	s_signed_file_read_parameters parameters;

	*(volatile bool *)&task->unknown1 = false;
	*(volatile bool *)&task->succeeded = false;
	task->progress = 0.0f;
	task->state = 4;
	function_x91aa57(task->path, path, sizeof(task->path));
	parameters.header = header;
	parameters.header_size = header_size;
	parameters.body = (byte *)body;
	parameters.body_size = body_size;
	parameters.file = INVALID_HANDLE_VALUE;
	parameters.body_offset = 0;
	parameters.signature_handle = INVALID_HANDLE_VALUE;
	parameters.task = task;
	parameters.non_roamable = non_roamable;
	parameters.state = _signed_file_read_header;
	return async_task_add_work(signed_file_read_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

enum
{
	_signed_file_write_header = 0,
	_signed_file_write_extend,
	_signed_file_write_body,
	_signed_file_write_signature
};

/* a signed saved game file write in steps; without a header it only makes
   sure the file has its full size */
#pragma pack(push, 1)
struct s_signed_file_write_parameters
{
	long state;
	void *header;
	dword header_size;
	void *body;
	dword body_size;
	HANDLE file;
	HANDLE signature_handle;
	s_saved_game_file_task *task;
	bool non_roamable;
	bool no_header;
};
#pragma pack(pop)

// score 129 (was 134): with the address-taken scratch record and the two-pointer getter (see
// 0x2ad7c0) the header site loads the buffer straight into EDI as retail; left: retail loads the
// scratch size into EAX and copies it to EBX after the memset count (ours loads it into EBX), and
// the extend arm keeps the offset in EBX and the size in EBP with the buffer spilled (ours spills the
// offset, buffer in EBP) and sums body first (ours header first in every spelling).
// Round 14 (lane 2): the header loop's chunk as `MIN(buffer_size, header_size - offset)` 127 (aligned
// 82 -> 88, a swap picked by score: not kept); the `void *` view here, with the size-returning
// getter, 138 (no change);
// compile-only: the 16 spellings of the extend arm's two sums compile the same; function-scope
// buffer, size, byte count or success, all 16 combinations, no better; a getter-local buffer
// (any order) gives 0x2ad7c0 156.
// Earlier (134): frame 0x18 and length agree; scratch-owner versus header/minimum scheduling, opposite buffer/count homes and extension I/O residency remain.
// Earlier: retail keeps every `parameters` load after the scratch getter's
// stores, as if the scratch object were address-taken (its 0x8000-byte storage at
// 0x4fa0c8 sits right before the owner/pointer/size words and the flag byte); ours
// hoists the loads. Storage as the object's first member, init `pointer = storage`
// 132; flag byte folded in as well 131 (0x2ad7c0 173); then the getter as size,
// owner, flag returning a fresh read of the pointer: later cases' registers all
// retail's but two copies per getter site, 279 at 944 bytes (0x2ad7c0 63); split
// size/claim helpers 131/139; write count declared before the getter 147; named
// body offset in the extend sum 131.
// DL180: case order 2013 324; case order 3012 331; case order 0213 192; case order 0312 273; case
// order 1203 313; case order 2103 314; case order 2301 332; case order 3210 328. Original
// executable body restored.
// Lane 13 r14 (with 0x2ad7c0's record form, claim/size helpers): 279 at 944 bytes, aligned 72 -> 32;
// the getter sites copy the buffer and size between registers where retail loads them in place.
// Round 187: body I/O fields at each use/parallel remaining-byte parts/both native API result
// carriers/fields plus parts/fields plus carriers/parts plus carriers/all three/all plus native
// signature value clear 261/271/163/275/269/279/283/340. Original getter, storage, observations,
// success types, library clears, zero-capacity loop and complete body restored.
// Lane 14 r11: the header arm's buffer goes to the parameters home and the byte count to the task
// home (retail the other way round). Locals renamed (scratch buffer, scratch size, written count;
// header arm or both arms) 134 each, identical code; one byte count shared by every case 150 (also
// with it declared first). Diagnostic: the getter reaching the scratch object through its address
// 132 (aligned 113 -> 109; 0x2ad7c0 68 -> 173 though its aligned count improves 55 -> 49), so both
// bodies look compiled with the object address-taken; a natural source of that is not found yet.
// Round 107 (fast; 0x2ad7c0 53 at the start): the record (storage, owner, pointer, size, flag, the
// init setting the pointer to the storage; defined in this file, since defined after async_globals
// its storage catches 0x120a90's node-loop end address) 131, 0x2ad7c0 159; with the getter storing
// size, owner, then flag (0x2ad7c0's retail order) 129, 0x2ad7c0 160; that plus 0x2ad7c0's size
// computed in each arm (MIN after the getter) 129, 0x2ad7c0 168 at 693 bytes (aligned 21/17); the
// reordered getter alone 134, 0x2ad7c0 161; the record without the flag 132, 0x2ad7c0 160.
// Retail schedules the getter differently at each site (site 1 flag, size, owner; site 2 owner,
// size, flag; 0x2ad7c0 size, owner, flag), with every parameters load after its stores.
// Lane 15 r17: the record defined in a /GL- file 134 (no change: LTCG still sees it); the getter
// through a record pointer 132 (header_size now loaded after the owner store, the flag store still
// sinks); the flag byte folded in as the record's fourth field (0x5020d4 = 0x5020c8 + 0xc) 131
// (both getter stores now ahead of every parameters load, as retail); with the parameters view cast
// from a `void *` parameter as this file's other work callbacks 130 (extend sum body first as
// retail); the view alone 133. With the record 0x2ad7c0 53 -> 159; its view, per-arm size (scratch
// arm clamps by the remaining count read after the claim) and one function-scope byte count take it
// to retail's length, sum order and dispatch hoist but 156 (buffer through eax, size/position
// registers swapped). Left: getter store/load order per site, extend offset spilled where retail
// spills the buffer.
// Lane 14 r20 (one record of storage, owner, buffer, size and flag, `buffer = storage` at init,
// defined in this file; 0x2ad7c0 in brackets): the current getter 131 [159]; with the view 130
// [157]; the getter returning the size, buffer through `byte **`, 139 [50], with the view 139
// [49], plus 0x2ad7c0's per-arm size 139 [36]; claim (buffer read into a local, flag, owner) and
// size helpers 138/130 by call order [156/156], owner first 137 [154], local read first 137 [154];
// claim storing the owner then the flag and returning a fresh read 279/278 by call order (944
// bytes, aligned 41) [34/34], flag first 280 [36]; the size/`byte **` getter with the flag from a
// fresh read 280 (944, aligned 51) [36], also returning a fresh size 280 [36]; that getter as a
// plain function in async.cpp 313 (called: /Ob1 file) [77, called], without `__forceinline` or with
// `inline` here 139 [77, called]. In the 944-byte forms the header site loads the buffer into EAX
// and copies it to EDI and puts the size in EBX, where retail loads the buffer straight into EDI
// and the size into EAX (copied to EBX before the memset's zero); the 940-byte forms keep retail's
// buffer register but give the buffer the parameters home (retail the task home).
// @retail 0x2adb10
PRIVATE long __stdcall signed_file_write_work(s_async_task *task, s_signed_file_write_parameters *parameters, long parameters_size)
{
	long result = 1;

	parameters->task->state = 4;
	switch (parameters->state)
	{
	case _signed_file_write_header:
		parameters->file = CreateFileA(parameters->task->path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
		if (parameters->file != INVALID_HANDLE_VALUE)
		{
			if (parameters->no_header)
			{
				dword file_size = GetFileSize(parameters->file, NULL);
				if (file_size != INVALID_FILE_SIZE)
				{
					if (file_size < parameters->body_size + parameters->header_size + sizeof(XCALCSIG_SIGNATURE))
					{
						parameters->state = _signed_file_write_extend;
					}
					else
					{
						parameters->task->state = 0;
						parameters->task->succeeded = true;
						break;
					}
				}
			}
			else
			{
				parameters->signature_handle = XCalculateSignatureBegin(parameters->non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
				if (parameters->signature_handle != INVALID_HANDLE_VALUE &&
					XCalculateSignatureUpdate(parameters->signature_handle, (const BYTE *)parameters->header, parameters->header_size) == ERROR_SUCCESS)
				{
					parameters->state = _signed_file_write_extend;
				}
			}

			byte *buffer;
			dword buffer_size;
			job_thread_buffer_get(task, &buffer, &buffer_size);
			memset(buffer, 0, MIN(buffer_size, parameters->header_size));

			bool success = true;
			for (dword offset = 0; offset < parameters->header_size && success; offset += buffer_size)
			{
				dword size = MIN(parameters->header_size - offset, buffer_size);
				dword bytes;
				success = SetFilePointer(parameters->file, offset, NULL, FILE_BEGIN) == offset &&
					WriteFile(parameters->file, buffer, size, &bytes, NULL) && bytes == size;
			}
			if (success)
			{
				result = 0;
			}
		}
		break;

	case _signed_file_write_extend:
		if (parameters->no_header)
		{
			byte *buffer;
			dword buffer_size;
			job_thread_buffer_get(task, &buffer, &buffer_size);
			long offset = parameters->body_size - buffer_size + parameters->header_size;
			offset = offset > 0 ? offset : 0;
			memset(buffer, 0, buffer_size);
			dword size = MIN(parameters->body_size - offset + parameters->header_size, buffer_size);
			dword bytes;
			if (SetFilePointer(parameters->file, offset, NULL, FILE_BEGIN) == offset &&
				WriteFile(parameters->file, buffer, size, &bytes, NULL) && size == bytes)
			{
				parameters->state = _signed_file_write_body;
				result = 0;
			}
		}
		else
		{
			dword position = parameters->header_size;
			dword body_size = parameters->body_size;
			const BYTE *body = (const BYTE *)parameters->body;
			dword bytes;
			if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
				WriteFile(parameters->file, body, body_size, &bytes, NULL) && bytes == body_size &&
				XCalculateSignatureUpdate(parameters->signature_handle, body, bytes) == ERROR_SUCCESS)
			{
				parameters->state = _signed_file_write_body;
				result = 0;
			}
		}
		break;

	case _signed_file_write_body:
	{
		dword bytes;
		if (parameters->no_header ||
			SetFilePointer(parameters->file, 0, NULL, FILE_BEGIN) == 0 &&
			WriteFile(parameters->file, parameters->header, parameters->header_size, &bytes, NULL) && bytes == parameters->header_size)
		{
			parameters->state = _signed_file_write_signature;
			result = 0;
		}
		break;
	}

	case _signed_file_write_signature:
	{
		XCALCSIG_SIGNATURE signature;
		dword bytes;
		bool success = true;
		if (parameters->no_header)
		{
			memset(&signature, 0, sizeof(signature));
		}
		else
		{
			success = XCalculateSignatureEnd(parameters->signature_handle, &signature) == ERROR_SUCCESS;
			parameters->signature_handle = INVALID_HANDLE_VALUE;
		}
		dword position = parameters->header_size + parameters->body_size;
		if (success &&
			SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
			WriteFile(parameters->file, &signature, sizeof(signature), &bytes, NULL) && bytes == sizeof(signature))
		{
			parameters->task->state = 0;
			parameters->task->succeeded = true;
		}
		break;
	}

	default:
		__assume(0);
	}

	if (result)
	{
		if (parameters->file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(parameters->file);
		}
		if (parameters->signature_handle != INVALID_HANDLE_VALUE)
		{
			XCalculateSignatureEnd(parameters->signature_handle, NULL);
		}
	}
	return result;
}

// @retail 0x2adec0
bool signed_file_write_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task)
{
	s_signed_file_write_parameters parameters;

	*(volatile bool *)&task->unknown1 = false;
	*(volatile bool *)&task->succeeded = false;
	task->progress = -1.0f;
	task->state = 4;
	function_x91aa57(task->path, path, sizeof(task->path));
	parameters.state = _signed_file_write_header;
	parameters.header = header;
	parameters.header_size = header_size;
	parameters.body = body;
	parameters.body_size = body_size;
	parameters.file = INVALID_HANDLE_VALUE;
	parameters.signature_handle = INVALID_HANDLE_VALUE;
	parameters.task = task;
	parameters.non_roamable = non_roamable;
	parameters.no_header = header == NULL;
	return async_task_add_work((async_task_work_function)signed_file_write_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

// @retail 0x2adf70
void saved_game_file_read(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	dword error;

	if (function_136970(file, 0x11, &error))
	{
		task->state = 3;
		if (function_xab2909(file, size, true, buffer))
		{
			HANDLE signature_handle = XCalculateSignatureBegin(non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
			if (signature_handle != INVALID_HANDLE_VALUE)
			{
				XCALCSIG_SIGNATURE computed;
				XCALCSIG_SIGNATURE stored;

				XCalculateSignatureUpdate(signature_handle, (const BYTE *)buffer, size);
				if (XCalculateSignatureEnd(signature_handle, &computed) == ERROR_SUCCESS &&
					file_ref_read_at(file, size, sizeof(stored), false, &stored) &&
					memcmp(&computed, &stored, sizeof(stored)) == 0)
				{
					task->succeeded = true;
					task->state = 0;
				}
			}
		}
		function_x8231f3(file);
	}
}

// @retail 0x2ae090
void saved_game_file_write(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	dword error;

	if (function_136970(file, 2, &error))
	{
		task->state = 2;
		if (function_xe8ca68(file, size, buffer))
		{
			HANDLE signature_handle = XCalculateSignatureBegin(non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
			if (signature_handle != INVALID_HANDLE_VALUE)
			{
				XCALCSIG_SIGNATURE signature;

				XCalculateSignatureUpdate(signature_handle, (const BYTE *)buffer, size);
				if (XCalculateSignatureEnd(signature_handle, &signature) == ERROR_SUCCESS &&
					file_ref_write_at(file, size, sizeof(signature), &signature))
				{
					task->succeeded = true;
					task->state = 0;
				}
			}
		}
		function_x8231f3(file);
	}
}
