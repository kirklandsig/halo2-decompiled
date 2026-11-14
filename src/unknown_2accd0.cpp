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
	g_4fa0c8.external = g_4fa0c8.buffer != g_4fa0c8.storage;
	*buffer = g_4fa0c8.buffer;
	*size = g_4fa0c8.size;
}

// MiniMax G (11-14): no new form kept, no lower score (34): the C form (the total inlined into the fild, the file
// handle read into a local) scores 28, but that local is one-use - the same 28 form the 11-09 notes record as not
// kept - and without it the same body scores 88. Still differs: the entry total lands in eax where retail keeps it
// in edx, the hoisted body size in esi where retail keeps it in ecx, the signature position as a one-byte-shorter
// add where retail uses lea, and the size/position pair is esi/edi where retail has edi/esi.
// score 34 (was 34): round 103 unchanged; the scratch buffer is one address-taken record (storage, owner, buffer,
// size, flag; the init points the buffer at the storage) claimed by a getter that returns the buffer
// and size through pointers; with the `void *` parameters view and one function-scope byte count the
// length, both sums' order, the dispatch hoist and the reloads after the claim are retail's. Left:
// the entry total in EAX (retail EDX) and size/position in ESI/EDI (retail EDI/ESI; the hoisted
// body_size in ESI, retail ECX): retail computes each sum into a fresh register.
// MiniMax D (11-11): no lower score (34): the total converted without its own local puts the entry sum in EDX as retail but
// reorders the signature case's file-handle load (88); declaring the position before the size changes nothing (34).
// MiniMax C (11-09): NOT kept: 28 (was 34), I's form again (a one-use local is kept only with a match): the entry total
// folded into its float conversion (no total local) and the signature case naming the file handle once, which puts the entry sum in EDX with the task pointer in EAX and the
// signature case's header size in EDX with the handle in EAX, as retail has them. Left: the dispatch still hoists
// body_size into ESI where retail reuses the switch register ECX, so the signature sum stays a one-byte-shorter add
// where retail has `lea esi, [ecx + edx]`, every later branch shifts by that byte, and the body's size and position
// stay in ESI and EDI where retail has them in EDI and ESI. Also tried and reverted: the position declared before
// the handle (28, unchanged), the signature sum's operands in the other order (28, unchanged), the position computed
// before the buffer arms (152), a separate capacity variable for the scratch getter (158). Also tried and reverted here:
// a local for the header size (28, unchanged), the position written as two statements (28, unchanged), the sum with a
// zero addend (28, unchanged), and the position computed at the top of the function (228: the entry sum then shares the
// position's addend and the body size load does reach ECX, but the entry no longer matches).
// MiniMax I (11-09): NOT kept: 28 (was 34) with the entry total inline for the float conversion plus a one-use
// HANDLE local for SetFilePointer only (the sum then lands in EDX, task pointer in EAX, as retail has it); a one-use
// local is kept only with a match, and the same local used by ReadFile too scores 203. The dispatch still hoists
// body_size into ESI where retail reuses the switch register ECX, and the signature position still folds into ESI
// where retail has `lea esi, [ecx + edx]`.
// MiniMax S (11-08): no new form, no lower score (34); the entry total still lands in EAX where retail has it in EDX with the task pointer in EAX, the dispatch still hoists body_size into ESI where retail reuses the switch register ECX, and the signature position still folds into ESI where retail has `lea esi, [ecx + edx]`.
// MiniMax D (11-07): score 34 (was 34): no new form and no lower score; dropping the named total still gives
// retail's head (the sum in EDX, the task pointer in EAX) but the dispatch then hoists body_size into ESI where
// retail reuses the switch register ECX, so the signature position is a one-byte-shorter add and every later
// branch shifts by one byte; the signature case made the switch default with that total scores 177.
// MiniMax E (11-06): no new form, no lower score.
// The head's total needs a fresh register (retail EDX): dropping the named total fixes it
// (the sum then lands in EDX) but reorders the header case's loads and scores 88; two size
// locals with the total from them score 91, the position declared first scores 34 with the
// scratch getter's stores in retail's order, the position computed first scores 158, and a
// function-scope position scores 34. Left: the hoisted body_size in ESI (retail ECX), the
// body pointer in ECX (retail EDX) and the size/position pair swapped between ESI and EDI.
// MiniMax b (11-04): no new form, no lower score; the entry total still lands in EAX (retail EDX) and the body's size/position still in ESI/EDI (retail EDI/ESI); an unnamed total fixes the head but reorders the header case's loads and scores 88, and the state store moved ahead of the total scores 232; the permuter's 188 annealed tries found nothing below 34, and the scratch clamp reading g_4fa0c8.size directly scores 159.
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
// Round 12 (fast; 0x2adb10 129 throughout): the scratch registers follow VC's EAX -> ECX -> EDX
// round-robin for temporaries; the `total` local is coalesced into the body-size load's EAX where retail
// sums into a fresh temporary (EDX). No total local (sum inside the conversion) 88: the first 20
// instructions are then retail's (total EDX, task EAX), but the signature case loads body_size straight
// into position's ESI (retail: temporaries ECX/EDX and `lea esi, [ecx+edx]`), rotating every later
// temporary, and the body case keeps size ESI / position EDI. On that: one function-scope position 88;
// buffer and size declared at the top of the body case 88; the signature sum body first 88; the
// signature case before the body case 88 (same code each). Retail's hoisted body_size is a temporary
// shared by both cases (its body arm ends `sub ecx, eax` + the clamp's `mov edi, ecx`), as if hoisted
// before allocation; ours is hoisted after allocation from two identical coalesced loads.
// Round 34 (lane 13, LTCG harness, base 22 = 34): locals renamed (size as count/read_size, position
// as file_offset/seek_position, buffer, total/total_size, bytes) 34 each and the direct arm's size
// before its buffer 34 (all identical code: names do not break these register ties); the body case as
// an early-out on `body_offset >= body_size` harness 88 against 22 (682 bytes); `size == bytes` 34 (identical).
// Round 125 (lane 6, fast with 0x2adb10 129 throughout): `long position` in the signature case, the body
// case or both 34 each (identical code); one exit through `long result = 1;` with `result = 0;` on the
// success paths and the cleanup under `if (result)` (0x2adb10's form) 34, with result declared before the
// view 34 (identical code); the signature case as `default:` without `__assume(0)` 177 (688 bytes); the
// view declared `* const` 34 (identical).
// Round 106 (lane 7; compile-only exact here, aligned in brackets, base [22]): retail's body arm ends in the
// scratch clamp's `mov edi, ecx` (one `size = remaining` reached from both arms). One size assignment after
// the arms with a body pointer local, `size = body ? remaining : MIN(size, remaining)`, gives retail's size
// EDI / position ESI / buffer EBP and the whole scratch arm, 161 (689 bytes) [16: the body arm loads the
// size first, so the dispatch hoist goes]; the same as if/else 161 [16]; with the local at case scope 161
// [16]; per-arm remaining with `if (body || size > remaining) size = remaining;` 160 (689) [20: body arm
// retail's but buffer/size EBP/EDI swapped]; `if (body || size > remaining expression)` 157 (691) [16];
// the body local alone 161 [25]; capacity from each arm then MIN or if after the join [22-27, cmova];
// names/declaration order/body-test operand order/total and signature sum orders on these: no change.
// Diagnostic (a mixed read, not kept): the join plus a body-size local read before the switch for the
// body test 24 (687 bytes) [12]: the body arm onward is retail's; left the entry total (EAX, retail EDX),
// the switch in EDX with the load before the first test (retail ECX, load after `dec`), and the
// signature sum folded into ESI (retail `lea esi, [ecx+edx]`). Without the total local on that [46].
// Compile-only, no change [22]: position/size at function or switch scope, initialised size or buffer,
// `size = body_size; size -= offset`, an explicit if clamp, the signature position spelled
// `p = a; p += b`, as long, declared after the signature buffers or compared reversed, the cases
// swapped; a forceinline seek helper for either position [31-65]. Flags (diagnostic): /O1, /Ox, /Oy-,
// /Ob1, no /arch:SSE: none gives retail's colours.
// Round 42 (lane 11, fast): a separate scratch-size local as 0x2adb10 writes it (`dword buffer_size;`
// getter into it, `size = MIN(buffer_size, remaining)`) gives retail's body arm (the remaining count in
// ECX from the body-size load, `sub ecx, eax` jumping into the clamp's `mov size, ecx`; position ESI) but
// the sums turn header first and the hoist goes: 160 (689 bytes); with a named remaining in the scratch
// arm 158 (689; the entry sum turns body first again, the sig position still takes the body-size
// register), the remaining set in both arms 158 (689); `size = buffer_size; if (size > remaining)` with
// `body_size > body_offset` 35; size set to the remaining before the `parameters->body` test 164 (691).
// Compile-only (exact here, base 22 aligned): about 900 combinations of those arms with the total local
// or none, both test orders, both operand orders of each position, buffer/size declaration and statement
// orders, the size local's placement and name, and the getter's store orders: none below 22.
// Lane 12 r66 (fast; a /GL- compile-only screen matched the build): a body_size local read once before the
// switch for the body test, the direct arm and the signature sum (the scratch clamp still reading the
// fields after the claim) 33: the body arm becomes retail's (`sub ecx, eax` then the shared `mov reg, ecx`),
// but the entry sums header first, the dispatch loads it before the header case and size/position stay
// ESI/EDI (a mixed read, not kept); with the clamp on the local too worse (compile-only); renamed or with the
// total summed body first, same code. A remaining-count inline helper for both arms 34 (same code); the
// clamp as `if (size > body_size - body_offset) size = ...` 34 (same code); per-case body_size locals
// (body case, both cases) 162 (689 bytes); the body test as `body_size > body_offset` 162 (689 bytes).
// Round 484: native header/body/signature file/read failure ownership, subsets
// H/B/S/HB/HS/BS/HBS and HBS with header-signature failure guards, all 34.
// Total EAX/EDX and size/position ESI/EDI roles remain; complete original body restored.
// Round 41 (lane 13, --fast; 0x2adb10 129 throughout): the task state store ahead of the total 232; the total and its
// conversion declared before the byte count, `size` declared before `buffer`, `byte *buffer = NULL`, and position
// declared with buffer/size and assigned later: 34 each (same code). All restored.
// Lane 12 r72 (fast; 0x2adb10 in brackets): the signature case as `default:` with no `__assume(0)` arm
// 177 (688 bytes); `parameters->task->state = 4;` ahead of the total 232; the body case's position
// written at both uses with no local 213; the signature case's likewise 221 (694 bytes); the getter
// returning the buffer with the size through `dword *` (owner, size, flag from the buffer field) 34
// [279, 944 bytes]. Compile-only: both positions written at both uses 111, with no total local 101;
// about 450 random rewrites of the current form found nothing below it.
// Round 23 (lane 17, fast; 0x2adb10 129 throughout): the getter returning a two-field (buffer, size)
// record by value 170 (695 bytes); a scratch-arm `buffer_size` local with one function-scope position for
// both cases 160 (689; sums header first again); the signature position written at both uses (no local)
// 221 (694); the body position at both uses 213; the signature case as the switch's `default:` label 177
// (688). Diagnostic: 1-7 dead long temps leave the code byte-identical (34), so these colour ties do not
// follow the temp count.
// Round 442: complete output-task/file-handle/signature-handle observations, seven subsets
// 196/169/177/255/252/245/255; complete graph with conversion after state publication 255.
// Original handles, observations and body restored; total and seek/size coalescing still differ.
// Round 64 (lane 16, fast): the signature state as the switch's `default:` 177 (688 bytes); the scratch clamp
// through a forceinline unsigned-min function 161 (689 bytes); the body-case I/O tests as nested ifs 34 (same
// code); the total converted after the task state store 42; the seek position written at each use in the
// signature case / body case / both 221 (694 bytes) / 213 / 221 (695 bytes).
// Round 41 (lane 13, --fast; 0x2adb10 129 throughout): the task state store ahead of the total 232; the total and its
// conversion declared before the byte count, `size` declared before `buffer`, `byte *buffer = NULL`, and position
// declared with buffer/size and assigned later: 34 each (same code). All restored.
// Round 69 (lane 16, fast; 0x2adb10 in brackets, 129 at the start): getter statement orders owner, buffer, flag,
// size 34 [138]; buffer, flag, size, owner 36 [131]; buffer, flag, owner, size 36 [139]; buffer, size, owner, flag
// 34 [129]; owner, size, buffer, flag 34 [129]. Getter restored.
// Round 484: native header/body/signature file/read failure ownership, subsets
// H/B/S/HB/HS/BS/HBS and HBS with header-signature failure guards, all 34.
// Total EAX/EDX and size/position ESI/EDI roles remain; complete original body restored.
// Round 44 (lane 13, --fast): the total folded into its conversion (no total local) 88; the buffer arms swapped
// (`if (!parameters->body)` first) 41; the file position computed before the buffer arms 224; a body-case
// `remaining` set in each arm (direct arm `size = remaining;`, scratch arm `if (size > remaining) size =
// remaining;`) 161 (689 B, entry sum header first and no dispatch hoist); that with `body_size > body_offset`
// and the signature position summed body first 35 (687 B, the hoist returns in ESI). All restored.
// Round 64 (lane 16, fast): the signature state as the switch's `default:` 177 (688 bytes); the scratch clamp
// through a forceinline unsigned-min function 161 (689 bytes); the body-case I/O tests as nested ifs 34 (same
// code); the total converted after the task state store 42; the seek position written at each use in the
// signature case / body case / both 221 (694 bytes) / 213 / 221 (695 bytes).
// Round 484: native header/body/signature file/read failure ownership, subsets
// H/B/S/HB/HS/BS/HBS and HBS with header-signature failure guards, all 34.
// Total EAX/EDX and size/position ESI/EDI roles remain; complete original body restored.
// Round 59 (m3 lane 10; compile-only, base 22, exact here): reasons for the fresh sums, each with two callers. An inline
// signature-position helper (the sum returned; inline or forceinline, either sum order) used at the signature case
// alone 22; with the total through it 22; in place 67, helper first in the sum 67. A forceinline seek-and-read helper
// taking the parameters (the file is read twice, as retail) at the body and signature cases 27 aligned (235 instructions,
// 155 positional), without the total local 60 (67); signature case only 22, without the total local 60 (67); its body as
// early returns, both cases 35 (239), without the total 76, signature only without the total 71 (237).
// Real --fast (0x2adb10 115 throughout): position helper with the total through it 34, total in place 88; seek-and-read helper at
// both cases 161 (689 bytes), without the total 88, signature only without the total 88; early-return body at both cases 216 (699 bytes).
// Round 56 (m3 lane 10; compile-only exact here, real --fast in brackets): retail's entry total and signature sum are
// compiler temporaries (sum into a fresh EDX; body size ECX, header EDX, `lea esi`). The total converted in place (no
// total local) with the signature position passed to a forceinline (file, position) seek helper gives both: 16 [28];
// left only the body case, whose body_size load goes straight into size's ESI (retail a temporary ECX), so the
// signature sum is coalesced again and the head merge carries ESI. Not kept: a new one-caller helper, and the helper
// at 0x2adb10's five seeks [0x2adb10 308, 956 bytes] or at its signature seek only [315, 960 bytes]. The helper in the
// body case too 174 (no total local 149, long parameter 174); the position written in place with the equality reversed
// (signature/body/both) 210/208/212 (the call still runs first and the sum is reloaded); reversed equality on the local
// 23. On the helper form: remaining set in both arms 22 (copy-propagated, sums header first); one size assignment after
// the arms (`body ? remaining : MIN`) 148 positional (aligned 9: buffer/size/position as retail, but body_size loaded
// into size's EDI and the hoist gone); remaining in both arms with `if (body || size > remaining)` 151 (body_size into
// the temporary ECX, but body_offset loaded first, so no head merge). Random search of 400 forms (helper type and use,
// total local, total and signature sum orders, bytes placement, six body-arm forms, buffer/size declaration and
// statement orders, function-scope position, test operand order): none below 16; 250 forms without the helper (adding
// signature position spellings `p = a; p += b`, function scope, long, const, after the buffers): none below 22.
// Round 246: Byte/short/bool/word common task results, declared before the request
// view or after total conversion, 269/264/257/252/269/264/257/252. Original returns
// and cleanup restored; fresh total EDX and seek/size ESI/EDI allocation remain.
// Round 309: Natural HBS/HSB/BHS/BSH/SHB/SBH state if-chains
// 210/216/210/224/229/219; consistent three-use state capture in HBS/SBH 212/220.
// Original switch, byte-count storage, I/O, default and cleanup restored; total and seek/size allocation remains.
// Round 249: Twice-used pointer in the current combined scratch getter, early/late observation and
// classification crossed with flag/size output phases: eight forms all 161. Flag-before-size forms
// also raise the other user from 129 to 138. Original helper/body restored; fresh total and
// size/seek carriers remain.
// Round 259: Header/body/signature ReadFile results at their actual producers, seven subsets,
// then all with native signature-finalization status: 34/34/34/34/34/34/34/34. All one-use
// result forms restored; entry total and size/seek coalescing remain.
// Round 333: Shared successful header/body progress producer, dword/real numerator,
// named/direct total conversion and early/late return storage: 156/156/214/214/114/114/156/156.
// Original progress stores, return paths and entire body restored; fresh total and size/seek carriers remain.
// Lane 16 r80 (fast; 0x2adb10 102 throughout): permuter 600 s, 51 tries, best 34; byte count declared in each
// case 161 (689 bytes); computed/stored signature declarations swapped 34; header test `header_size == bytes` 36;
// total grouped `header + (body + sizeof)` 34; signature position declared after the two signatures 34. Restored.
// Round 61 (m3 lane 11, fast; 0x2adb10 102 throughout): no existing seek wrapper fits (grep of SetFilePointer callers). The body offset
// in a local used three times 34 (same code); the signature's body size in a local 34 (same code); the body test as `!=` 161 (689 bytes). On
// round 56's helper form (a new one-caller seek helper, not kept): `remaining` in both arms with `body_size > body_offset` 29; remaining with
// one `if (body || size > remaining)` after the arms 166 (698 bytes). No existing helper gives the sums' fresh registers.
// N340: Complete header/body extent snapshots, mutable/const individually and
// together, then const total/denominator: 218/219/193/218/219/193/193/193.
// NOT kept: unmatched extent observation/storage changes; original full body restored.
// Round 65 (m3 lane 11, fast; 0x2adb10 102 throughout): total converted in place with a two-site forceinline seek helper
// 155 at 689 (aligned 30: both sums load the header size first and the body-size hoist is lost); total written body first
// 155, signature position written body first 155, both 155 (same code); a test of the total ahead of its conversion
// 244 at 701.
// Round 66 (m3 lane 10, fast; 0x2adb10 102 throughout): the header read test as `header_size == bytes` 36; the body
// progress sum offset first 34 (same code); the header case's open test as its own if around the read 34 (same code);
// the scratch clamp as `MIN(remaining, size)` 35; the signature-state test as `!(offset < size)` 34 (same code).
// Round 85 (m3 lane 9; compile-only count against retail, base 32): 0x2adb10 matched by reading the record's fields again for its memset and offset, so
// the getter now stores owner, flag, buffer, size (same code here, 34). Same lead here: the scratch arm's clamp `MIN(g_4fa0c8.size, ...)` 20 (its registers
// become retail's; left: total in EDX, the hoist needing the signature sum as `lea esi, [ecx + edx]`), exact-only since it reads the field beside a local
// copy. On it: no total local 64 (total in EDX as retail, later registers rotate); the signature sum repeated 81/76, body first 20, through a body, header
// or both locals 20; on the plain body: a named remaining 36, the clamp as an if 33, total body first 32.
// B403: Actual seek-return dword at body/signature/both, seek-equality Boolean at
// body/signature/both, signed seek result at body/both: 34/34/34/34/34/34/34/34.
// NOT kept: one-use results remain unmatched; original full body restored.
// Captured unsigned total with header/body denominator conversions at entry, case
// or successful progress producers (eight nonoriginal pairs): 248/233/247/261/260/233/252/207.
// All regress; original eager conversion, all field observations, I/O and body restored.
// P309: Guarded header/body/finalization signing-status masks 1/2/4/3/5/6/7, then all signed:
// 83/34/34/83/83/34/83/83. Header result adds frame/length; other outcomes tie. NOT kept:
// unmatched one-use/test-produced status; original full body restored, total/seek/size homes remain.
// M352: Actual header/body progress and integer signature-comparison producers,
// seven subsets 34/34/34/202/202/202/202; all with one shared progress scalar 202.
// NOT kept: one-use output/result storage does not match; original body and getter restored.
// Round 148 (m3 lane 5, fast; 0x2adb10 stays matched; aligned counts from a line diff of the listing, base about 22): a conversion helper for the
// total 34, for all three conversions 34, with the total folded in 88; a `do { } while (false)` over the body case's failure path 34; `default: break;`
// 218 (695 bytes). Closest yet, not kept (score rises until it matches): forceinline `(file, position)` seek helper for the signature sum + total folded
// into its conversion 28 (aligned 17) + the sibling's `dword buffer_size;` local + `dword remaining` + `size = buffer_size; if (buffer_size > remaining)
// size = remaining;` 153 (691 bytes, aligned 6: size EDI, position ESI, the sums, the direct arm's `sub ecx, eax` jumping to the shared `mov edi, ecx`, all
// retail's). Left: the scratch size loads into a temporary EDX and is copied (retail loads EDI directly), and the signature sum folds body_size into ESI
// (retail `mov ecx`, head-merged with the body case, then `lea esi, [ecx + edx]`). On it: `if (size > remaining)` or the out parameter into size 156 (689,
// aligned 20: size ESI, sums header first again); MIN 153 (693, aligned 9); if/else or a ternary clamp 153 (693, aligned 9); a getter returning the size 156,
// with buffer_size 153 (aligned 6); named body and header locals in the signature case, sum written body first, `register` on the helper parameter 153
// (same code); the body test as `body_size > body_offset` 157 (aligned 11); a body_size local read before the switch 142 (688, aligned 9: T in ECX, but the
// switch temp moves to EDX and the signature sum still folds). Harness cross of direct-arm order, six clamps, total local, helper none/signature/both, test
// order (110 forms): none below aligned 6. Seek helper with `g_4fa0c8.size` clamp 153 (689); that clamp alone 159, without the total local 175.
// P327: API-returned file/signing handles, original/mutable/const producer pairs
// excluding both original: 34/34/34/161/161/34/161/161. NOT kept: ties or larger
// 689-byte bodies; original API publications, field observations and full body restored.
// Round 212 (m3 lane 3; compile-only exact here, real --fast where given, aligned in brackets, base [23]): no total local with the signature
// seek through a new inline (file, position) helper 28 fast (687) [17]; the same code from no total local plus a one-use `HANDLE file` in the
// signature case 17 compile-only (one-use local, not kept); that file local used for the ReadFile too 195 (682) [53]; plus the clamp through
// the record's field 153 fast (689) [3]: only retail's dispatch hoist and `lea esi, [ecx + edx]` are then left (the signature sum still
// coalesces body_size into ESI). On that form: the seek helper with a long or const-reference parameter, forceinline, an internal copy, or
// named/cast parts for the sum 148-149 compile-only (no change); a body-size local in the body case 152 [7] (sums turn header first and the
// compare flips); with an offset local too 152 [7]; one multi-definition body-size local across both cases 152 [7]; the body test as
// `body_size > body_offset` 152 [9]; the sum repeated in the equality through named parts 171 [45]; `dword const &position` 171 [43], in the
// helper 148 [4]; a shared signature-offset helper at the entry and the signature case 118 [47]. No total local alone 68 [57]; the signature
// position declared after or between the buffers 68 [57].
// Round 212, later (real --fast): no total local plus a one-use `HANDLE file` 28 [17]; plus a named remaining in the clamp, direct arm's size first and
// the flipped body test 29 [18]; the same with the seek helper and a file local 29 [18]; no total cast, the clamp re-reading the record's size twice, seek helper 28 [17].
// Grid of 143 compile-only forms over these: lowest positional 17, lowest aligned 4 at 148. Not kept: each needs a new one-caller helper or a one-use local.
// DH370: Actual successful-read transfer-count equality producers, header/body/signature
// subsets 1/2/4/3/5/6/7 and all complementary incompleteness: eight at 34.
// NOT kept: no exact comparison-result form; original body, getter, API boundaries
// and cleanup restored. Entry total and seek/size carriers remain.
// F403: Actual completed-offset/cancellation/signing-flags producer subsets 1/2/4/3/5/6/7,
// then positive admission on the complete graph: 126/161/161/82/82/161/82/223. NOT kept: all regress;
// original scalar producers, observations, getter, API tuples and complete body restored.
// Fresh total and independent extent/seek/transfer carriers remain.
// Round 57 (m3 lane 15, fast; brackets aligned with registers kept, stack masked; base [23]): cross of total, signature
// sum and clamp (36 forms): no total local, helper at the signature only, `MIN(g_4fa0c8.size, remaining)` with a buffer_size local 153 (689) [4], left: the signature
// sum coalesces body_size into ESI, so its load is not head-merged with the body case; the same with the original clamp 28 (687) [17] (one-caller helper, not kept); field
// clamp alone 161 [12]; two-site helper 155 [31], with remaining 151 [27], with the field clamp 152 [23]; helper at the signature with the field clamp 156 [7]. On
// the [4] form: sum repeated with named parts 175, by field 217; named body local in the equality 214, in the sum 175; two-statement position 175; `default:` for the
// case 156; flipped body test 157; named position with `__assume(position >= body_size)` 175; `__assume` on the state, file or body size at the dispatch, the
// signature or the body case 153/156.
// AC553: Existing unsigned-total and real-denominator lexical identities, four
// independent names each: 34/34/34/34/34/34/34/34. NOT kept: no native fresh-register
// or size/seek allocation change; original scalar names and complete body restored.
// DL269: Shared scratch task/buffer/size reference interfaces, subsets 1..7: 34 each;
// all with ordinary inline 174 (fast/full control 34). NOT kept: no gain or equality.
// Original helper, all three uses and callback restored; total/seek/size homes remain.
// DL271: Standard-library capacity minimum, shared/separate capacity and direct/named
// remaining expressions, both native bound directions: eight at 207, 717/719 bytes.
// NOT kept: no equality; original macro, capacity output, total/seek homes and body restored.
// Round 76 (m3 lane 10; fast, a compile-only harness in brackets, base [27]): a two-caller inline read-at-position
// helper (seek, read, byte-count test) for the body and signature cases 212 (691 bytes); no total local with the
// position on the left of each seek compare 89; the signature seek and read as nested ifs 34; a const signature
// position 34; the body pointer as `&body[offset]` 34. Harness only, all [27] (same code): the signature position
// declared after the buffers, `body != NULL`, a const total or total_size; the position on the compare's left [28];
// on the no-total base [82] each of those, the byte count declared later, the total summed body first, sizeof first
// or through a dword cast; the signature sum written in both the seek and its compare [96] (it reloads after the call).
// Retail's body_size temporary shares ECX with the body arm's remaining count, so the signature sum cannot fold into it.
// Round 57 (m3 lane 2, fast; 0x2adb10 matched throughout): the matched sibling's clamp (a `buffer_size` local, `size =
// MIN(remaining, buffer_size)`) 161 (691 bytes), with the total and the signature position written body first 161; the signature
// position written body first alone 34 (same code); the direct arm's size before its buffer with `MIN(remaining, size)` 35; the
// body case's file handle in a two-use local 168 (695 bytes).
// Round 160 (m3 lane 5, fast; 0x2adb10 matched throughout), each on the no-total-local form (88 alone): `bytes` declared in the
// `!unknown1` block 88; both signature buffers declared at the top of that block 177 (688 bytes); the body case as an early out on
// `body_offset >= body_size` 122; the buffer arms swapped 93; the scratch clamp as an explicit if on the field difference 88. The
// matched sibling's signature seek is `mov edi, [header]; add edi, [body]` (coalesced) where this retail sums two temporaries with `lea`.
// Round 97 (m3 lane 9, fast; 0x2adb10 matched throughout; aligned counts in brackets): the in-place total with a new (file, position)
// seek helper at the signature seek re-measured 28 [17]; on it: the direct arm's size before its buffer 28 (same code); size declared
// before the buffer 28 (same code); the body test as `body_size > body_offset` 157 (689 bytes); the helper at the body seek too 155 (689);
// the scratch clamp through the record's size field 153 (689) [4: only the signature sum is left, ours `mov esi, [body_size]` and
// `add esi, edx`, retail `mov ecx` head-merged with the body case and `lea esi, [ecx + edx]`]. On that [4] form, all 153 [4] (same
// code): the signature sum as the total minus the signature size (the constants fold before the add is formed); the helper's parameters
// as (position, file); the sum written body first; the helper plain inline; a three-parameter helper adding (start, offset) inside.
// Retail's sum keeps the body size in its own temporary, as if it were read again after the sum; no form found that keeps it apart.
// Nothing kept (one-caller helper; the field clamp reads the field beside a local copy).
// Round 66 (m3 lane 15, fast; 0x2adb10 matched throughout): the dispatch hoist is a merge of two identical loads into a callee-saved register (ours ESI): with no
// total local, the record's size field in the clamp and the body test flipped both cases start `mov ecx, [body_size]` and nothing is hoisted, so retail's ECX there was
// allocated before the scratch rotation. The parameter record's size fields as `unsigned int` (all three, body only, header only) 34 each (same code); one inline size
// function (body + header + a trailer argument, sizeof at the entry and 0 at the signature) 34, with no total local 88, with the field clamp 175/159 (same code as
// without it: the zero folds before the sum is formed); an inline (file, start, offset) seek for the signature case with no total local and the field clamp 153 (689
// bytes), summed offset first 153, at the body case too 154, on the plain body 178 (688), with no total local 154; a seek with an unused third (limit) argument given
// the body size 153; that limit tested in the seek (`position >= limit &&`) 217 (693), the same test written in the case 216 (692), on the plain body 220 (693): even
// with the body size read twice the sum is `mov esi, [header]; add esi, eax`, so a second reference alone does not give retail's `lea esi, [ecx + edx]`.
// Round 161 (m3 lane 6, fast; 0x2adb10 matched throughout): the total as `sizeof + (header + body)` 34; the result as
// `!memcmp(...)` 34; the body progress summed offset first 34; the header case as two early `break`s on the failure
// tests 34; the signature seek and read as nested ifs 34 (all the same code; the first and last re-measure listed forms).
// New on top: the total as header + sizeof + body 34 (same code); the bool passed straight as XCalculateSignatureBegin's
// flag (its value is 1) 71.
// Round 103 (m3 lane 16, fast with 0x2adb10 matched; the compile-only /Gr harness gives the same code as fast for each): both
// positions `dword const` 34; the signature position as `position = header_size; position += body_size;` 34; the signature
// seek compare as `position == SetFilePointer(...)` 35; the direct arm's size as `size = body_size; size -= body_offset;` 34; the
// body position as a `long` 34 (harness: the signature position as a `long`, the body position through `+=`, both the same code).
// Lane 12 r105 (fast; 0x2adb10 and 0x2ada70 matched throughout): the total converted in place re-measured 88 (entry agrees to
// +0x3f; the signature case then hoists body_size into ESI, position's register, where retail loads it into ECX). Retail gives both
// cases' positions ESI and the body case's size EDI (ours size ESI, position EDI). New: one function-scope position for both
// cases 34 (same code), with the total converted in place 88; the body position computed at the top of the body block 158; a
// `remaining` count computed before the two buffer arms 161, with the total converted in place 178; the body case as an early
// return once the body is complete 103.
// Round 69 (m3 lane 15): no new form; three of the 34 lines are the same float constants on both sides, flagged only where
// the code is a byte out of line; the rest is the one rotation above (entry total EAX/EDX, hoisted body_size ESI/ECX,
// size/position ESI/EDI).
// M3 lane 9 r110 (fast with 0x2adb10 and 0x2ada70, both matched; control 34): retail's body case computes `body_size -
// body_offset` in ECX (the hoisted body_size) and copies it to EDI, a copy the direct arm shares with the scratch arm's
// clamp by a cross-jump; ours coalesces body_size, the remaining count and size into ESI, so no copy is left to share
// (retail gives the position ESI and the size EDI; ours the reverse). The direct arm's size before its buffer 34 (only
// the buffer's lea moves); the signature update given the requested size instead of the transfer count 136; the buffer
// arms swapped on `!parameters->body` 41. The explicit if clamp re-measured 34 (same code). The compile-only harness
// gives 30 aligned lines for this body.
// M3 lane 3 r231 (fast with 0x2adb10 and 0x2ada70, both matched in each; base 34, round 103 also unchanged at 34; a compile-only
// /Gr harness with retail-normalized operands gives 28 aligned lines on this body, all register rotation). New: the buffer arms
// reduced to the buffer and one shared `if (parameters->body || size > remaining-expression) size = remaining-expression;` 177
// (702 bytes); the signature position assigned inside the seek call (`SetFilePointer(file, position = ..., ...) == position`)
// 34 (same code; harness 58 lines on the folded total). Re-measured listed forms: the total folded into its conversion 88
// (harness 58: the head is retail's, then the hoisted body_size coalesces into the signature position's ESI and the signature
// case's scratch registers fall one step behind retail's ECX/EDX/EAX); a `remaining` count set in each arm with an if-clamp 161
// (689 bytes, round 112's); on the folded total the body test as `body_size > body_offset` 178 (688), the two-statement
// signature position 88 and the direct arm's size before its buffer 88 (both same code). Harness only, on the folded total:
// `position == SetFilePointer(...)` 61 lines, the sum in the seek and its compare 67 lines. No other untried natural form is
// left for the rotation: the levers are the hoisted body_size's register and the total's coalescing, and every form listed here
// leaves both or moves only one (the one-use HANDLE local that fixes the folded form's signature case is reverted unless the
// function matches).
// m3 lane 11 r90: no new form run (baseline 34, masked aligned 2: register rotation only). Each candidate checked against this
// note is listed: getter references, size/position spellings, total local and orders, clamp forms, case/test orders, field
// types, the signature file-handle local, the existing MIN and remaining helpers, the body early-out. A scan of include/ for
// helpers this body writes out found none.
// M3 lane 2 r72 (fast with 0x2adb10 and 0x2ada70, both matched throughout; masked = registers/slots-masked aligned count, base 2):
// the task's state store first, then the total and its conversion 232 (masked 33); the total, then the state store, then the
// conversion 42 (masked 6); the body case's `bytes == size` as `size == bytes` 34 (same code); the direct arm's size before
// its buffer on this base 34 (masked 4); the scratch clamp as `MIN(body_size - body_offset, size)` 35 (masked 3).
// M3 lane 4 r190 (fast with 0x2adb10 and 0x2ada70, both matched throughout): the byte count declared in each case 161
// (689 bytes); both positions `dword const` 34; the clamp as `if (body_size - body_offset < size) size = ...;` 35; the
// signature position written at both uses (no local) 221 (694 bytes), both cases' positions so 221 (695 bytes).
// MiniMax A (11-12): no new form, no lower score (34); the difference is still the register rotation (the total in EAX against retail's EDX, the hoisted body_size in ESI against ECX) and the join copy at retail's 0x2ad911 that ours lacks because it coalesces the remaining count into ESI.
// M3 lane 2 r77 (fast with 0x2adb10 and 0x2ada70, both matched; control 34): re-measured the total folded into its
// conversion 88 (the entry is retail's to +0x3f; the dispatch then hoists body_size into ESI where retail keeps ECX, and the
// signature case's scratch registers fall one step behind). No unlisted natural form found: total, size, position, clamp,
// getter, case and scope forms are all listed above. Diagnostic, reverted at once: one dead volatile read of a parameters
// field before the total, before its conversion, after the state store, before the switch or at the header, signature or
// body case's start, on this body and on the folded total: the total stays in EAX here (EDX only when folded) and the
// hoisted body_size stays in ESI everywhere, so the residue is the hoisted value's coalescing with position and size,
// not a missing scratch-rotation step. Body unchanged.
// M3 lane 7 r156 (fast with the file's 20 functions; 0x2ad480 stays 19): the direct arm's count as `size = body_size; size -=
// body_offset;` 34 (same code); the body test as `body_size > body_offset` on this base 162 (689 B); the entry total summed header,
// signature size, body 34 (same code); the state dispatch as an if / else-if / else chain 210 (687 B) and the switch without its
// `default: __assume(0);` arm 218 (695 B, range check): in both the entry sum lands in EDX with the task pointer in EAX as in
// retail (first difference +0x3f), so the unreachable default is what puts our entry sum in EAX; without it the dispatch no
// longer hoists body_size and the signature sum still folds into ESI (masked aligned 23). The default placed first 34 (same
// code); `case _signed_file_read_signature: default:` 177 (688 B, the listed default form); the body progress as `(real)(position
// + bytes) / total_size` 107; re-measured on this body: the listed early out on `body_offset >= body_size` 103. No fifth unlisted
// natural form found (the `__assume` on the state at the dispatch is listed too). Body unchanged.
// M3 lane 5 r179: no trial; the residue is register rotation only (masked 2), and the rounds above (lane 3 r231, lane 2
// r77, lane 7 r156) list every natural total, size, position, clamp, getter, case and scope form. Body unchanged.
// M3 lane 16 r116 (fast with 0x2adb10/0x2ada70, both matched throughout): the entry total folded into its conversion
// re-measured 88 (entry retail's to +0x3f; the signature case then loads the file handle into ECX after the add where
// retail loads it into EAX ahead of the pushes). On that folded form: no `default: __assume(0);` 220 (695 bytes); the
// scratch clamp as an `if` 88; the signature position summed body size first 88; the two signatures declared ahead of the
// position 88; the body case's position summed body offset first 88; the state store ahead of the folded conversion 231.
// Reading: retail's direct arm computes the remaining count in ECX and jumps to the clamp's `mov edi, ecx` (the two arms'
// `size = ...` tails are cross-jumped), so its hoisted body_size never joins size or position; ours coalesces them in ESI.
// Body unchanged.
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

// @retail 0x2adb10
PRIVATE long __stdcall signed_file_write_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_signed_file_write_parameters *parameters = (s_signed_file_write_parameters *)parameters_view;
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
			memset(g_4fa0c8.buffer, 0, MIN(g_4fa0c8.size, parameters->header_size));

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
			dword size;
			job_thread_buffer_get(task, &buffer, &size);
			long offset = parameters->body_size - g_4fa0c8.size + parameters->header_size;
			offset = offset > 0 ? offset : 0;
			memset(g_4fa0c8.buffer, 0, g_4fa0c8.size);
			size = MIN(parameters->body_size - offset + parameters->header_size, size);
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
			const BYTE *body = (const BYTE *)parameters->body;
			dword body_size = parameters->body_size;
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
	return async_task_add_work(signed_file_write_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
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
