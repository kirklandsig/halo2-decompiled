#include "unknown_11c920.h"
#include "files.h"
#include <string.h>

// @flags /O2 /Gr

bool function_1249f0(long memory_unit, char *drive_letter);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_122810(char *string, const char *suffix);
const char *function_216b60(long type);
bool function_1368f0(s_type_acf665 *file);

// @retail 0x216400
char *function_216400(long memory_unit, char *buffer, short capacity)
{
	char drive_letter;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(buffer, capacity, "%c:\\", drive_letter);
	else
		buffer[0] = 0;
	return buffer;
}

// @retail 0x216d70
bool function_216d70(char *path, const char *directory, long type)
{
	strncpy(path, directory, 0x100);
	path[0xff] = 0;
	function_122810(path, function_216b60(type));
	return true;
}

struct s_storage_file_reference
{
	dword signature;
	word flags;
	short location;
	char path[0x100];
	__int64 state;
};

// @retail 0x216440
bool __stdcall function_216440(char const *path)
{
	bool result = true;
	s_storage_file_reference file;
	memset(&file, 0, sizeof(file));
	file.signature = FILE_REFERENCE_SIGNATURE;
	file.location = NONE;
	function_137320(file.path, path);
	if (!function_1368f0((s_type_acf665 *)&file))
	{
		if (!function_1367d0((s_type_acf665 *)&file))
			result = false;
	}
	return result;
}

// @retail 0x217590
long function_217590(long memory_unit)
{
	char root[8] = "";
	char drive_letter;
	long count = 0;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(root, sizeof(root), "%c:\\", drive_letter);
	else
		root[0] = 0;
	XGAME_FIND_DATA data;
	HANDLE find = XFindFirstSaveGame(root, &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			count++;
		}
		while (XFindNextSaveGame(find, &data));
		XFindClose(find);
	}
	return count;
}
