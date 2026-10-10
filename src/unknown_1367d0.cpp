// @flags /O2 /Gr
/* UNKNOWN_1367D0.CPP: file operations */

#include "unknown_11c920.h"
#include <xtl.h>
#include "files.h"

#define MAXIMUM_PATH_SIZE 256

void function_1374c0(char *dest, const char *path);

// @retail 0x1367d0
bool function_1367d0(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	function_1374c0(path, file->path);

	if (file->flags & 1)
	{
		HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
			return true;
		}
	}
	else if (CreateDirectoryA(path, NULL))
	{
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

// @retail 0x136860
bool function_136860(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	function_1374c0(path, file->path);

	if (file->flags & 1)
	{
		if (SetFileAttributesA(path, FILE_ATTRIBUTE_NORMAL) && DeleteFileA(path))
		{
			return true;
		}
	}
	else if (RemoveDirectoryA(path))
	{
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

// @retail 0x1368f0
bool function_1368f0(s_type_acf665 *file)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	bool success = false;
	function_1374c0(path, file->path);

	if (GetFileAttributesA(path) != 0xFFFFFFFF)
	{
		success = true;
	}
	else if (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x136970
bool function_136970(s_type_acf665 *file, dword flags, dword *error)
{
	bool local_1 = false;
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	dword access = 0;
	dword share = 0;
	dword attributes = FILE_ATTRIBUTE_NORMAL;

	*error = 0;
	function_1374c0(path, file->path);

	if (flags & 1)
	{
		access = GENERIC_READ;
	}
	if (flags & 2)
	{
		access |= GENERIC_WRITE;
	}
	if (!(flags & 2) || (flags & 8))
	{
		share = FILE_SHARE_READ;
	}
	if (flags & 0x20)
	{
		attributes = FILE_ATTRIBUTE_TEMPORARY;
	}
	if (flags & 0x40)
	{
		attributes = FILE_FLAG_DELETE_ON_CLOSE;
	}
	if (flags & 0x80)
	{
		attributes = FILE_FLAG_RANDOM_ACCESS;
	}
	if (flags & 0x100)
	{
		attributes = FILE_FLAG_SEQUENTIAL_SCAN;
	}

	HANDLE handle = CreateFileA(path, access, share, NULL, OPEN_EXISTING, attributes, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND: *error = 1; break;
		case ERROR_PATH_NOT_FOUND: *error = 3; break;
		case 15: *error = 4; break;
		case ERROR_ACCESS_DENIED: *error = 2; break;
		case ERROR_SHARING_VIOLATION: *error = 5; break;
		case ERROR_INVALID_NAME: *error = 6; break;
		default: *error = 6; break;
		}
	}
    else
    {
        file->handle = handle;
        file->position = 0;
        local_1 = true;
        if (flags & 4)
        {
            file->position = SetFilePointer(handle, 0, NULL, FILE_END);
            if (file->position == INVALID_SET_FILE_POINTER)
            {
                CloseHandle(file->handle);
                file->handle = 0;
                file->position = 0;
                local_1 = false;
            }
        }
    }

	if (!local_1 && !(flags & 0x10))
	{
		GetLastError();
		SetLastError(0);
	}
	return local_1;
}

// @retail 0x136bb0
bool function_136bb0(s_type_acf665 *file)
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

// @retail 0x136bf0
bool function_136bf0(s_type_acf665 *file, dword position, bool silent)
{
	if (file->position == position)
	{
		return true;
	}
	dword result = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
	bool success = result != INVALID_SET_FILE_POINTER;
	file->position = result;
	if (!success && !silent)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x136c40
bool function_136c40(s_type_acf665 *file, dword position)
{
	bool result = false;
	bool success = true;
	if (file->position != position)
	{
		dword result = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
		file->position = result;
		success = result != INVALID_SET_FILE_POINTER;
		if (!success)
		{
			GetLastError();
			SetLastError(0);
		}
	}
	if (success)
	{
		if (SetEndOfFile(file->handle))
		{
			return true;
		}
	}
	GetLastError();
	SetLastError(0);
	return result;
}

// @retail 0x136ca0
bool function_136ca0(s_type_acf665 *file, void *buffer, dword size, bool silent)
{
	dword bytes_read;
	bool success = false;
	if (ReadFile(file->handle, buffer, size, &bytes_read, NULL))
	{
		if (bytes_read == size)
		{
			success = true;
		}
		else
		{
			SetLastError(ERROR_HANDLE_EOF);
		}
	}
	file->position += bytes_read;
	if (!success && !silent)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x136d00
bool function_136d00(s_type_acf665 *file, const void *buffer, dword size)
{
	dword bytes_written;
	bool success = false;
	BOOL result = WriteFile(file->handle, buffer, size, &bytes_written, NULL);
	if (result && bytes_written == size)
	{
		success = true;
	}
	file->position += bytes_written;
	if (!success)
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x136df0
bool function_136df0(s_type_acf665 *file, FILETIME *time)
{
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA data;

	memset(time, 0, sizeof(FILETIME));
	function_1374c0(path, file->path);
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &data))
	{
		time->dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
		time->dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
		return true;
	}

	GetLastError();
	SetLastError(0);
	return false;
}

// @retail 0x136e70
void function_136e70(FILETIME *time)
{
	FILETIME now;
	GetSystemTimeAsFileTime(&now);
	*time = now;
}

// @retail 0x136e90
bool function_136e90(s_type_acf665 *file, dword *size)
{
	bool result = false;
	char path[MAXIMUM_PATH_SIZE] = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA data;

	function_1374c0(path, file->path);
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &data))
	{
		*size = data.nFileSizeLow;
		result = true;
	}
	else
	{
		GetLastError();
		SetLastError(0);
	}
	return result;
}
