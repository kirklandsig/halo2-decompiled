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

const char *function_216b60(long type);
long function_217480(long type);
char *function_122810(char *path, const char *name);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
bool function_1249f0(long memory_unit, char *drive_letter);
bool function_1367d0(s_type_acf665 *file);
bool function_136c40(s_type_acf665 *file, dword position);
bool function_216da0(wchar_t *name, long type, const wchar_t *display_name, long language);
bool function_216f80(long type, s_saved_game_file_location *location);
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
	return function_136d00(file, buffer, size);
}

inline bool function_xab2909(s_type_acf665 *file, dword size, bool silent, void *buffer)
{
	return function_136ca0(file, buffer, size, silent);
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
void saved_game_file_get_path(char *path, const s_saved_game_file *file);

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
		if (!error)
		{
			char path[0x100];
			s_type_acf665 s_type_acf665;

			parameters->task->state = 4;
			path[0] = 0;
			saved_game_file_get_path(path, file);
			function_x454397(&s_type_acf665);
			function_x73bce5(&s_type_acf665, path);
			saved_game_file_write(&s_type_acf665, parameters->buffer, parameters->size, parameters->non_roamable, parameters->task);
		}
		result = 1;
	}
	else if (error)
	{
		parameters->task->state = 0;
		result = 1;
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
void saved_game_file_new(s_saved_game_file *file, long flags, const s_saved_game_file_location *location)
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
}

// @retail 0x2acf80
s_saved_game_file *saved_game_file_new_from_id(s_saved_game_file *file, long flags, const wchar_t *display_name, const s_saved_game_file_location *location, void *buffer, bool *done)
{
	file->flags = flags;
	file->location.name[0] = 0;
	file->location.display_name[0] = 0;
	wcsncpy(file->display_name, display_name, 0x10);
	file->display_name[0x10] = 0;
	file->id = *(const s_saved_game_file_id *)location;
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
	file->location = *location;
	return file;
}

// @retail 0x2ad020
bool saved_game_file_copy_create(s_saved_game_file *file)
{
	long type = file->flags & 0xf;
	bool result = false;
	wchar_t name[0x80];
	char root[8];
	char path[0x100] = { 0 };
	char file_path[0x100];
	s_saved_game_file_id id;
	s_type_acf665 s_type_acf665;
	dword error;

	name[0] = 0;
	function_216da0(name, type, file->display_name, get_current_language());
	saved_game_file_get_root(file, root);
	if (XCreateSaveGame(root, name, CREATE_NEW, 0, path, sizeof(path)) == ERROR_SUCCESS)
	{
		file_path[0] = 0;
		function_x91aa57(id.data, path, sizeof(id.data));
		function_x91aa57(file_path, id.data, sizeof(file_path));
		function_122810(file_path, function_216b60(type));
		function_xbb02d2(&s_type_acf665, file_path);
		if (function_1367d0(&s_type_acf665) && function_136970(&s_type_acf665, 2, &error))
		{
			if (function_136c40(&s_type_acf665, function_217480(type)))
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
	dword size = copy->size - offset;
	dword bytes;

	if (size >= FILE_COPY_CHUNK_SIZE)
	{
		size = FILE_COPY_CHUNK_SIZE;
	}
	if (SetFilePointer(copy->source, offset, NULL, FILE_BEGIN) == copy->read_offset)
	{
		if (ReadFile(copy->source, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			copy->read_offset += bytes;
			result = true;
		}
	}
	return result;
}

// @retail 0x2ad400
bool file_copy_write_chunk(s_saved_game_file *copy)
{
	bool result = false;
	dword offset = copy->write_offset;
	dword size = copy->size - offset;
	dword bytes;

	if (size >= FILE_COPY_CHUNK_SIZE)
	{
		size = FILE_COPY_CHUNK_SIZE;
	}
	if (SetFilePointer(copy->destination, offset, NULL, FILE_BEGIN) == copy->write_offset)
	{
		if (WriteFile(copy->destination, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			copy->write_offset += bytes;
			result = true;
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
		return true;
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
				return true;
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
	return result;
}
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
		function_216da0(name, type, file->location.display_name, file->location.language);
		XDeleteSaveGame(root, name);
		wcsncpy(file->location.display_name, file->display_name, 0x10);
		file->location.display_name[0x10] = 0;
		file->location.language = (char)get_current_language();
		function_2168b0(&file->location, file->flags);
	}
	else
	{
		function_216da0(name, type, file->display_name, get_current_language());
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
		if (function_216f80(file->flags & 0xf, &file->location))
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
	function_122810(path, function_216b60(type));
}

// @retail 0x2ad760
void saved_game_file_get_path(char *path, const s_saved_game_file *file)
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

bool g_5020d4;

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

// @retail 0x2ad7c0
PRIVATE long __stdcall signed_file_read_work(s_async_task *task, s_signed_file_read_parameters *parameters, long parameters_size)
{
	dword total = parameters->body_size + parameters->header_size + sizeof(XCALCSIG_SIGNATURE);
	real total_size = (real)total;

	parameters->task->state = 4;
	if (!parameters->task->unknown1)
	{
		switch (parameters->state)
		{
		case _signed_file_read_header:
		{
			dword bytes;
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
				dword bytes;
				if (parameters->body)
				{
					buffer = parameters->body + parameters->body_offset;
					size = parameters->body_size - parameters->body_offset;
				}
				else
				{
					buffer = (byte *)g_5020c8.unknown04;
					size = g_5020c8.unknown08;
					g_5020c8.unknown00 = (dword)task;
					g_5020d4 = g_5020c8.unknown04 != 0x4fa0c8;
					if (size > parameters->body_size - parameters->body_offset)
					{
						size = parameters->body_size - parameters->body_offset;
					}
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

		default:
		{
			dword position = parameters->body_size + parameters->header_size;
			XCALCSIG_SIGNATURE stored;
			XCALCSIG_SIGNATURE computed;
			dword bytes;

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
bool signed_file_read_begin(void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task, const char *path)
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
	return async_task_add_work((async_task_work_function)signed_file_read_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

/* the job thread's scratch buffer, claimed by a task */
inline byte *job_thread_buffer_get(s_async_task *task, dword *size)
{
	byte *buffer = (byte *)g_5020c8.unknown04;
	g_5020d4 = buffer != (byte *)0x4fa0c8;
	*size = g_5020c8.unknown08;
	g_5020c8.unknown00 = (dword)task;
	return buffer;
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

			dword buffer_size;
			byte *buffer = job_thread_buffer_get(task, &buffer_size);
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
			dword buffer_size;
			byte *buffer = job_thread_buffer_get(task, &buffer_size);
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
bool signed_file_write_begin(void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task, const char *path)
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
					function_136d50(file, size, sizeof(stored), false, &stored) &&
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
					function_136d90(file, size, sizeof(signature), &signature))
				{
					task->succeeded = true;
					task->state = 0;
				}
			}
		}
		function_x8231f3(file);
	}
}
