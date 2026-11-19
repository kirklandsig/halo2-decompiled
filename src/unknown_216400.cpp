#include "unknown_11c920.h"
#include "files.h"
#include <string.h>

// @flags /O2 /Gr

bool function_1249f0(long memory_unit, char *drive_letter);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_122810(char *string, const char *suffix);
const char *saved_game_type_get_storage_name(long type);
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
bool function_216d70(const char *directory, char *path, long type)
{
	strncpy(path, directory, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
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



void __stdcall function_216e50(word const *display_name, long type, char *language, word *name);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);

struct s_save_entry_2164c0
{
    char directory[20];
    word name[17];
    byte unknown36[2];
    long type;
    byte unknown3c;
    char language;
    byte unknown3e[2];
};

struct s_save_list_2164c0
{
    long count;
    s_save_entry_2164c0 entries[4096];

    bool full() const { return count == 4096; }
    long new_index() { return count++; }
    s_save_entry_2164c0 *get(long index) { return &entries[index]; }
};

/* appends entries to a caller's list */
struct s_save_list_appender_2164c0
{
    s_save_list_2164c0 *list;

    s_save_list_appender_2164c0(s_save_list_2164c0 *target) : list(target) {}
    bool add(s_save_entry_2164c0 const *entry)
    {
        bool result = !list->full();
        if (result)
            *list->get(list->new_index()) = *entry;
        return result;
    }
};

/* lists the saved games on a memory unit: each save's directory is matched
   against the eleven storage types, and a recognised one is appended to the
   caller's list through an appender that stops at 4096 entries */
// @retail 0x2164c0
void function_2164c0(long memory_unit, s_save_list_2164c0 *list)
{
    char drive;
    if (function_1249f0(memory_unit, &drive))
    {
        s_save_list_appender_2164c0 appender(list);
        char root[8] = "";
        function_216400(memory_unit, root, sizeof(root));
        XGAME_FIND_DATA data;
        HANDLE find = XFindFirstSaveGame(root, &data);
        if (find != INVALID_HANDLE_VALUE)
        {
            do
            {
                s_save_entry_2164c0 entry;
                memset(&entry, 0, sizeof(entry));
                strncpy(entry.directory, data.szSaveGameDirectory, sizeof(entry.directory));
                entry.directory[19] = 0;
                long type;
                for (type = 0; type < 11; type++)
                {
                    char path[256];
                    path[0] = 0;
                    function_216d70(entry.directory, path, type);
                    s_storage_file_reference file;
                    memset(&file, 0, sizeof(file));
                    file.signature = FILE_REFERENCE_SIGNATURE;
                    file.location = NONE;
                    if (file.flags & 1)
                    {
                        short length = (short)strlen(file.path);
                        while (length > 0 && file.path[length] != '\\') length--;
                        file.path[length] = 0;
                    }
                    if (*path)
                    {
                        size_t length = strlen(file.path);
                        char *end = file.path + length;
                        if (end != file.path && end[-1] != '\\')
                        {
                            *end++ = '\\';
                            *end = 0;
                            length++;
                        }
                        strncpy(end, path, sizeof(file.path) - length);
                        file.path[255] = 0;
                    }
                    file.flags |= 1;
                    if (function_1368f0((s_type_acf665 *)&file))
                        goto found;
                }
                {
                    word description[256];
                    unicode_string_snprintf(description, 256, L"unrecognized saved game file found by XFindNextSaveGame(): display name= '%s' path= '%hs'", data.szSaveGameName, data.wfd.cFileName);
                    char text[256];
                    unicode_string_to_ascii(description, text, 256);
                }
                goto next;
found:
                {
                    entry.type = type;
                    function_216e50(data.szSaveGameName, type, &entry.language, entry.name);
                    if (!appender.add(&entry))
                        break;
                }
next:;
            } while (XFindNextSaveGame(find, &data));
            XFindClose(find);
        }
    }
}
