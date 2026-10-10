// @flags /O2 /Ob1 /Gr
/* UNKNOWN_136770.CPP: file enumeration and path helpers (0x136fc0..0x1374c0) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <xtl.h>

#define MAXIMUM_PATH_SIZE 256

struct s_type_b56524
{
	dword flags;
	short depth;
	short unknown06;
	char path[256];
	char root[256];
	HANDLE handles[16];
	WIN32_FIND_DATA data;
};

struct file_reference_data
{
	dword signature;
	byte flags;
	byte unknown05;
	word unknown06;
	char path[256];
	byte unknown108[8];
};

struct s_file_time
{
	dword low;
	dword high;
};

void function_1374c0(char *dest, const char *path);
void function_137320(char *path, const char *name);
void function_1373c0(char *path);

/* retail inlines function_137320 / function_1373c0 into function_137000 at two sites only and calls them elsewhere.
   Built as one translation unit LTCG inlines them everywhere (/Ob2, 889 bytes) or nowhere (/Ob1 alone, 513 bytes; retail is 798),
   so the two inlined sites keep these copies and /Ob1 keeps the rest out of line. */
inline void find_files_path_append(char *path, const char *name)
{
	if (*name)
	{
		size_t length = strlen(path);
		char *end = path + length;
		if (end != path && end[-1] != '\\')
		{
			*end++ = '\\';
			*end = 0;
			length++;
		}
		strncpy(end, name, 256 - length);
		path[255] = 0;
	}
}

inline void find_files_path_truncate(char *path)
{
	short index = (short)strlen(path);
	while (index > 0 && path[index] != '\\')
	{
		index--;
	}
	path[index] = 0;
}

static inline char *csstrncpy(char *destination, const char *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

// @retail 0x136f00
void function_136f00(s_type_b56524 *find, dword flags, file_reference_data *directory, const char *root)
{
	for (long i = 0; i < 16; i++)
	{
		find->handles[i] = INVALID_HANDLE_VALUE;
	}
	find->flags = flags;
	find->depth = 0;
	find->unknown06 = directory->unknown06;
	csstrncpy(find->path, directory->path, MAXIMUM_PATH_SIZE);
	csstrncpy(find->root, root, MAXIMUM_PATH_SIZE);
}

// @retail 0x136fc0
void function_136fc0(s_type_b56524 *find)
{
	short depth = find->depth;
	while (depth >= 0)
	{
		if (find->handles[depth] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(find->handles[depth]);
			find->handles[depth] = INVALID_HANDLE_VALUE;
		}
		depth--;
	}
}

// @retail 0x137000
bool function_137000(s_type_b56524 *find, file_reference_data *file, s_file_time *time)
{
	char buffer[256] = {0};
	short depth = find->depth;
	char *path = find->path;

	while (depth >= 0)
	{
		HANDLE *handle = &find->handles[depth];
		if (*handle == INVALID_HANDLE_VALUE)
		{
			function_1374c0(buffer, path);
			if (find->root[0])
			{
				find_files_path_append(buffer, find->root);
			}
			*handle = FindFirstFileA(buffer, &find->data);
			if (*handle == INVALID_HANDLE_VALUE)
			{
				find_files_path_truncate(path);
				depth--;
				continue;
			}
		}
		else if (!FindNextFileA(*handle, &find->data))
		{
			CloseHandle(*handle);
			*handle = INVALID_HANDLE_VALUE;
			find_files_path_truncate(path);
			depth--;
			continue;
		}

		if (find->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!strcmp(find->data.cFileName, ".") || !strcmp(find->data.cFileName, ".."))
			{
				continue;
			}
			bool descended = false;
			if (find->flags & 1)
			{
				function_137320(path, find->data.cFileName);
				depth++;
				descended = true;
			}
			if (find->flags & 6)
			{
				word unknown06 = find->unknown06;
				memset(file, 0, sizeof(*file));
				file->signature = 'filo';
				file->unknown06 = unknown06;
				function_137320(file->path, path);
				if (!descended)
				{
					function_137320(file->path, find->data.cFileName);
				}
				if (time)
				{
					time->low = find->data.ftLastWriteTime.dwLowDateTime;
					time->high = find->data.ftLastWriteTime.dwHighDateTime;
				}
				find->depth = depth;
				return true;
			}
		}
		else if ((find->flags & 4) || !(find->flags & 2))
		{
			word unknown06 = find->unknown06;
			memset(file, 0, sizeof(*file));
			file->signature = 'filo';
			file->unknown06 = unknown06;
			function_137320(file->path, path);
			if (file->flags & 1)
			{
				function_1373c0(file->path);
			}
			function_137320(file->path, find->data.cFileName);
			file->flags |= 1;
			if (time)
			{
				time->low = find->data.ftLastWriteTime.dwLowDateTime;
				time->high = find->data.ftLastWriteTime.dwHighDateTime;
			}
			find->depth = depth;
			return true;
		}
	}
	find->depth = depth;
	return false;
}

// @retail 0x137320
void function_137320(char *path, const char *name)
{
	if (*name)
	{
		size_t length = strlen(path);
		char *end = path + length;
		if (end != path && end[-1] != '\\')
		{
			*end++ = '\\';
			*end = 0;
			length++;
		}
		strncpy(end, name, MAXIMUM_PATH_SIZE - length);
		path[MAXIMUM_PATH_SIZE - 1] = 0;
	}
}

// @retail 0x137370
void function_137370(char *path, const char *extension)
{
	if (*extension)
	{
		size_t length = strlen(path);
		char *end = path + length;
		if (end != path)
		{
			*end++ = '.';
			*end = 0;
			length++;
		}
		strncpy(end, extension, MAXIMUM_PATH_SIZE - length);
		path[MAXIMUM_PATH_SIZE - 1] = 0;
	}
}

// @retail 0x1373c0
void function_1373c0(char *path)
{
	short index = (short)strlen(path);
	while (index > 0 && path[index] != '\\')
	{
		index--;
	}
	path[index] = 0;
}

// @retail 0x137400
void function_137400(char *path, char **a, char **b, char **c, char **d, bool flag)
{
	short length = (short)strlen(path);
	char *p = path + length;
	*c = p;
	*d = p;
	*a = p;
	*b = p;
	if (length > 0)
	{
		dword n = (word)length;
		do
		{
			if (*p == '.')
			{
				if (flag && **a == 0 && **b == 0)
				{
					*p = 0;
					*b = p + 1;
				}
			}
			else if (*p == '\\')
			{
				if (flag && **a == 0)
				{
					*p = 0;
					*a = p + 1;
				}
				else if (**d == 0)
				{
					*d = p + 1;
				}
			}
			p--;
			n--;
		} while (n);
	}
	if (flag && **a == 0)
	{
		*a = path;
	}
	else if (*a != path)
	{
		*c = path;
	}
}

char g_453588[256];

// @retail 0x1374c0
void function_1374c0(char *dest, const char *path)
{
	dest[0] = 0;
	char c = path[0];
	if (!(((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) && path[1] == ':' && path[2] == '\\'))
	{
		strncpy(dest, g_453588, MAXIMUM_PATH_SIZE);
		dest[MAXIMUM_PATH_SIZE - 1] = 0;
	}
	const char *end = dest;
	dword length;
	for (length = 0; length < MAXIMUM_PATH_SIZE; length++)
	{
		if (!*end++)
		{
			break;
		}
	}
	strncpy(dest + length, path, MAXIMUM_PATH_SIZE - length);
	dest[MAXIMUM_PATH_SIZE - 1] = 0;
}

// @retail 0x136770
long function_136770(file_reference_data *directory, dword flags, long maximum_count, file_reference_data *files)
{
	s_type_b56524 find;
	long count = 0;

	function_136f00(&find, flags, directory, "*.*");
	for (; count < maximum_count; count++)
	{
		if (!function_137000(&find, &files[count], NULL))
		{
			break;
		}
	}
	function_136fc0(&find);
	return count;
}
