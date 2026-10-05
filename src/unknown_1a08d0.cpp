// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1A08D0.CPP: file operations queued on the asynchronous task queue
   (async.cpp): create, read, write, copy, size, flush and close, each a task
   callback and the function that queues it. Decompiled by lane L: the
   preferences and font code in 0x120000-0x12ffff call them with register
   arguments. */

#include "unknown_11c920.h"
#include "async.h"
#include <xtl.h>
#include <string.h>

char *function_122810(char *string, const char *suffix);

/* the largest read and write one step of a task makes */
dword g_46e488 = 0x40000;
dword g_46e48c = 0x40000;

static inline void csstrncpy(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
}

static inline bool file_set_position_inline(HANDLE file, long offset, LONG high)
{
	return SetFilePointer(file, offset, &high, FILE_BEGIN) != INVALID_SET_FILE_POINTER;
}

static inline char *function_xe9ecc4(char *string, char const *delimiters, char **next)
{
	char *end = string;
	char *token;

	if (end)
	{
		end += strspn(end, delimiters);
		if (!*end)
			end = NULL;
	}
	token = end;
	if (end)
	{
		end = strpbrk(end, delimiters);
		if (end)
			*end++ = 0;
	}
	*next = end;
	return token;
}

// @retail 0x1a08d0
DWORD function_1a08d0(HANDLE file, long offset)
{
	LONG high = 0;

	return SetFilePointer(file, offset, &high, FILE_BEGIN);
}

// @retail 0x1a08f0
long __stdcall function_1a08f0(s_async_task *task)
{
	s_create_file_task *create = &task->create_file;

	create->file->handle = CreateFileA(create->path, create->access, create->share_mode, NULL, create->creation_disposition, create->flags, NULL);
	if (create->file->handle == INVALID_HANDLE_VALUE && create->create_path)
	{
		char directory[256];
		char path[256];
		char *next;
		char *token;

		directory[0] = 0;
		csstrncpy(path, create->path, sizeof(path));
		token = function_xe9ecc4(path, "\\", &next);
		csstrncpy(directory, "", sizeof(directory));
		while (token)
		{
			function_122810(directory, token);
			token = function_xe9ecc4(next, "\\", &next);
			if (token)
				CreateDirectoryA(directory, NULL);
			function_122810(directory, "\\");
		}
		create->file->handle = CreateFileA(create->path, create->access, create->share_mode, NULL, create->creation_disposition, create->flags, NULL);
	}
	return 1;
}

// @retail 0x1a0b40
long function_1a0b40(
	char const *path,
	dword access_flags,
	long disposition,
	dword file_flags,
	long category,
	long priority,
	s_file_handle *file,
	bool volatile *done)
{
	bool create_path = false;
	dword access = 0;
	dword share_mode = 0;
	dword flags = 0;
	dword volatile creation_disposition;
	s_async_task task;

	memset(&task, 0, sizeof(task));

	switch (disposition)
	{
	case 0:
		creation_disposition = OPEN_EXISTING;
		break;
	case 1:
		creation_disposition = CREATE_ALWAYS;
		break;
	case 2:
		creation_disposition = CREATE_NEW;
		break;
	case 3:
	case 4:
		creation_disposition = OPEN_ALWAYS;
		if (disposition == 4)
			create_path = true;
		break;
	case 5:
		creation_disposition = TRUNCATE_EXISTING;
		break;
	default:
		__assume(0);
	}

	if (access_flags & 1)
		access = GENERIC_READ;
	if (access_flags & 2)
		access |= GENERIC_WRITE;
	else
		share_mode = FILE_SHARE_READ;

	if (file_flags & 1)
		flags = FILE_FLAG_NO_BUFFERING;
	if (file_flags & 2)
		flags |= FILE_FLAG_RANDOM_ACCESS;
	else if (file_flags & 4)
		flags |= FILE_FLAG_SEQUENTIAL_SCAN;

	task.create_file.path = path;
	task.create_file.access = access;
	task.create_file.share_mode = share_mode;
	task.create_file.creation_disposition = creation_disposition;
	task.create_file.flags = flags;
	task.create_file.file = file;
	task.create_file.create_path = create_path;
	return function_120ba0(priority, &task, category, function_1a08f0, done);
}

// @retail 0x1a0c90
long __stdcall async_copy_file_callback(s_async_task *task)
{
	s_copy_file_task *copy = &task->copy_file;
	bool finished = false;
	bool error;

	switch (copy->state)
	{
	case 0:
		if (copy->source.handle == INVALID_HANDLE_VALUE)
		{
			error = true;
			break;
		}
		copy->state = 1;
		copy->size = GetFileSize(copy->source.handle, NULL);
		error = function_1a08d0(copy->source.handle, 0) != 0;
		break;
	case 1:
		copy->state = 2;
		if (copy->destination.handle == INVALID_HANDLE_VALUE)
		{
			error = true;
			break;
		}
		error = function_1a08d0(copy->destination.handle, 0) != 0;
		break;
	case 2:
		error = !ReadFile(copy->source.handle, copy->buffer, 0x10000, &copy->bytes_read, NULL);
		copy->state = 3;
		break;
	case 3:
	{
		DWORD bytes_written = 0x10000;

		error = !WriteFile(copy->destination.handle, copy->buffer, copy->bytes_read, &bytes_written, NULL);
		copy->bytes_written += bytes_written;
		copy->state = 2;
		if (copy->bytes_read < 0x10000)
			copy->state = 4;
		break;
	}
	case 4:
		finished = true;
		*copy->success = finished;
		return finished;
	default:
		return finished;
	}

	if (error)
	{
		finished = true;
		*copy->success = false;
	}
	return finished;
}

// @retail 0x1a0da0
bool async_copy_file(s_file_handle source, s_file_handle destination, long category)
{
	byte buffer[0x10000];
	bool success = false;
	bool volatile done = false;

	if (source.handle != INVALID_HANDLE_VALUE && destination.handle != INVALID_HANDLE_VALUE)
	{
		s_async_task task;

	memset(&task, 0, sizeof(task));

		task.copy_file.state = 0;
		task.copy_file.source = source;
		task.copy_file.destination = destination;
		task.copy_file.buffer = buffer;
		task.copy_file.success = &success;
		function_120ba0(6, &task, category, async_copy_file_callback, &done);
		function_120d50(&done, false);
	}
	return success;
}

// @retail 0x1a0e70
long __stdcall function_1a0e70(s_async_task *task)
{
	s_read_position_task *read = &task->read_position;
	LONG high = 0;
	DWORD bytes_read = 0;
	dword size = read->size - read->bytes_read > g_46e488 ? g_46e488 : read->size - read->bytes_read;

	if (SetFilePointer(read->file.handle, read->offset, &high, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
	{
		ReadFile(read->file.handle, (byte *)read->buffer + read->bytes_read, size, &bytes_read, NULL);
		read->bytes_read += bytes_read;
		read->offset += bytes_read;
		if (read->bytes_read_out)
			*read->bytes_read_out = read->bytes_read;
	}
	if (read->size == read->bytes_read || bytes_read < size)
		return 1;
	return 0;
}

// @retail 0x1a0f10
long function_1a0f10(
	s_file_handle file,
	void *buffer,
	dword size,
	dword offset,
	long category,
	long priority,
	dword *bytes_read,
	bool volatile *done)
{
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.read_position.file = file;
	task.read_position.buffer = buffer;
	task.read_position.size = size;
	task.read_position.offset = offset;
	task.read_position.bytes_read_out = bytes_read;
	return function_120ba0(priority, &task, category, function_1a0e70, done);
}

// @retail 0x1a0fb0
long __stdcall function_1a0fb0(s_async_task *task)
{
	s_write_position_task *write = &task->write_position;
	DWORD bytes_written = NONE;
	LONG high = 0;

	if (SetFilePointer(write->file.handle, write->offset, &high, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
	{
		dword size = g_46e48c;

		if (write->size - write->bytes_written <= size)
			size = write->size - write->bytes_written;
		WriteFile(write->file.handle, (byte const *)write->buffer + write->bytes_written, size, &bytes_written, NULL);
		write->offset += bytes_written;
		write->bytes_written += bytes_written;
		if (write->size != write->bytes_written && bytes_written >= size)
			return 0;
		if (write->bytes_written_out)
			*write->bytes_written_out = write->bytes_written;
	}
	if (write->flags & 1)
		FlushFileBuffers(write->file.handle);
	return 1;
}

// @retail 0x1a1050
long function_1a1050(
	s_file_handle file,
	void const *buffer,
	dword size,
	dword offset,
	dword flags,
	long category,
	long priority,
	dword *bytes_written,
	bool volatile *done)
{
	if (bytes_written)
		*bytes_written = 0;
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.write_position.file = file;
	task.write_position.buffer = buffer;
	task.write_position.size = size;
	task.write_position.offset = offset;
	task.write_position.bytes_written_out = bytes_written;
	task.write_position.flags = flags;
	return function_120ba0(priority, &task, category, function_1a0fb0, done);
}

// @retail 0x1a1100
long __stdcall async_copy_position_callback(s_async_task *task)
{
	s_copy_position_task *copy = &task->copy_position;
	dword size = g_46e488;
	long finished = false;
	DWORD bytes = 0;
	LONG high = 0;

	if (g_46e48c <= size)
		size = g_46e48c;
	if (copy->size - copy->bytes_copied <= size)
		size = copy->size - copy->bytes_copied;

	if (!copy->writing)
	{
		if (SetFilePointer(copy->source.handle, copy->source_offset, &high, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			bytes = ReadFile(copy->source.handle, (byte *)copy->buffer + copy->bytes_copied, size, &bytes, NULL) ? bytes : 0;
			copy->source_offset += bytes;
		}
		if (bytes < size)
			finished = true;
		copy->writing = true;
	}
	else
	{
		if (SetFilePointer(copy->destination.handle, copy->destination_offset, &high, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			bytes = WriteFile(copy->destination.handle, (byte *)copy->buffer + copy->bytes_copied, size, &bytes, NULL) ? bytes : 0;
			copy->bytes_copied += bytes;
			copy->destination_offset += bytes;
		}
		if (copy->size == copy->bytes_copied || bytes < size)
			finished = true;
		copy->writing = false;
	}
	if (copy->bytes_copied_out)
		*copy->bytes_copied_out = copy->bytes_copied;
	return finished;
}

// @retail 0x1a1210
long async_copy_position(
	s_file_handle source,
	s_file_handle destination,
	void *buffer,
	dword size,
	dword source_offset,
	dword destination_offset,
	long category,
	long priority,
	dword *bytes_copied,
	bool volatile *done)
{
	if (bytes_copied)
		*bytes_copied = 0;
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.copy_position.source = source;
	task.copy_position.destination = destination;
	task.copy_position.buffer = buffer;
	task.copy_position.size = size;
	task.copy_position.bytes_copied_out = bytes_copied;
	task.copy_position.source_offset = source_offset;
	task.copy_position.destination_offset = destination_offset;
	return function_120ba0(priority, &task, category, async_copy_position_callback, done);
}

// @retail 0x1a12c0
long __stdcall async_set_file_size_callback(s_async_task *task)
{
	s_set_file_size_task *set = &task->set_file_size;
	bool success;

	success = file_set_position_inline(set->file.handle, set->size, 0);

	if (success)
		success = SetEndOfFile(set->file.handle) != 0;
	if (set->success)
		*set->success = success;
	return 1;
}

// @retail 0x1a1310
long function_1a1310(s_file_handle file, dword size, long category, long priority, bool *success, bool volatile *done)
{
	if (success)
		*success = false;
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.set_file_size.file = file;
	task.set_file_size.size = size;
	task.set_file_size.success = success;
	return function_120ba0(priority, &task, category, async_set_file_size_callback, done);
}

// @retail 0x1a13a0
long __stdcall async_read_entire_file_callback(s_async_task *task)
{
	s_read_entire_file_task *read = &task->read_entire_file;
	bool finished = false;
	bool success;

	if (read->file.handle == INVALID_HANDLE_VALUE)
	{
		read->file.handle = CreateFileA(read->path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		success = read->file.handle != INVALID_HANDLE_VALUE;
	}
	else if (read->size == NONE)
	{
		read->size = GetFileSize(read->file.handle, NULL);
		success = read->size != NONE;
		if (success && read->size_out)
			*read->size_out = read->size;
	}
	else
	{
		dword size = read->buffer_size;
		DWORD bytes_read;

		if (read->size <= size)
			size = read->size;
		success = ReadFile(read->file.handle, read->buffer, size, &bytes_read, NULL) && bytes_read == size;
		finished = true;
	}

	if (!success || finished)
	{
		if (read->file.handle != INVALID_HANDLE_VALUE)
			CloseHandle(read->file.handle);
		if (read->success)
			*read->success = success;
	}
	return finished || !success;
}

// @retail 0x1a1480
long function_1a1480(
	char const *path,
	void *buffer,
	dword buffer_size,
	long category,
	long priority,
	bool *success,
	dword *size,
	bool volatile *done)
{
	if (success)
		*success = false;
	if (size)
		*size = 0;
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.read_entire_file.path = path;
	task.read_entire_file.success = success;
	task.read_entire_file.file.handle = INVALID_HANDLE_VALUE;
	task.read_entire_file.size = NONE;
	task.read_entire_file.buffer = buffer;
	task.read_entire_file.buffer_size = buffer_size;
	task.read_entire_file.size_out = size;
	return function_120ba0(priority, &task, category, async_read_entire_file_callback, done);
}

// @retail 0x1a1530
long __stdcall async_close_file_callback(s_async_task *task)
{
	CloseHandle(task->file.file.handle);
	return 1;
}

// @retail 0x1a1550
long function_1a1550(s_file_handle file, long category, long priority, bool volatile *done)
{
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.file.file = file;
	return function_120ba0(priority, &task, category, async_close_file_callback, done);
}

// @retail 0x1a15d0
long __stdcall async_get_file_size_callback(s_async_task *task)
{
	*task->file.size_out = GetFileSize(task->file.file.handle, NULL);
	return 1;
}

// @retail 0x1a15f0
long function_1a15f0(s_file_handle file, long category, long priority, dword *size, bool volatile *done)
{
	s_async_task task;

	*size = 0;
	memset(&task, 0, sizeof(task));
	task.file.file = file;
	task.file.size_out = size;
	return function_120ba0(priority, &task, category, async_get_file_size_callback, done);
}

// @retail 0x1a1680
long __stdcall async_flush_file_callback(s_async_task *task)
{
	FlushFileBuffers(task->file.file.handle);
	return 1;
}

// @retail 0x1a16a0
long async_flush_file(s_file_handle file, long category, long priority, bool volatile *done)
{
	s_async_task task;

	memset(&task, 0, sizeof(task));

	task.file.file = file;
	return function_120ba0(priority, &task, category, async_flush_file_callback, done);
}

// @retail 0x1a0af0
void async_flush_file_blocking(s_file_handle file, long category)
{
	bool volatile done = false;

	async_flush_file(file, category, 6, &done);
	function_120d50(&done, false);
}

/* file reads of equal priority run in order of their position */
// @retail 0x1a1720
bool async_task_should_run_before(
	long priority,
	long other_priority,
	s_async_task const *task,
	s_async_task const *other_task,
	s_async_insert_state *state,
	async_work_callback callback,
	async_work_callback other_callback)
{
	bool result = false;
	s_async_insert_state *const *state_reference = &state;

	if (priority > other_priority)
	{
		result = true;
	}
	else if (priority < other_priority)
	{
		result = false;
	}
	else if (callback == function_1a0e70 && other_callback != function_1a0e70)
	{
		result = true;
	}
	else if (callback != function_1a0e70 && other_callback == function_1a0e70)
	{
		result = false;
	}
	else if (callback == function_1a0e70 && other_callback == function_1a0e70)
	{
		if (!(*state_reference)->valid)
		{
			result = false;
		}
		else if ((dword)task->read_position.file.handle - state->file < (dword)other_task->read_position.file.handle - state->file)
		{
			result = true;
		}
		else if ((dword)task->read_position.file.handle - state->file > (dword)other_task->read_position.file.handle - state->file)
		{
			result = false;
		}
		else
		{
			result = task->read_position.offset - state->offset < other_task->read_position.offset - state->offset;
		}
	}

	if (other_callback == function_1a0e70)
	{
		state->file = (dword)other_task->read_position.file.handle;
		state->offset = other_task->read_position.offset;
		state->valid = true;
	}
	return result;
}

/* unknown_1a1870.cpp */
struct file_reference_data;
char *file_reference_get_path(file_reference_data const *file, char *path);

/* opens or creates a file reference's file and waits for it */
// @retail 0x1a0a70
void async_create_file_blocking(file_reference_data const *s_type_acf665, dword access_flags, long disposition, dword file_flags, long category, s_file_handle *file)
{
	char path[256];
	bool volatile done;

	file_reference_get_path(s_type_acf665, path);
	function_1a0b40(path, access_flags, disposition, file_flags, category, 6, file, &done);
	function_120d50(&done, false);
}
