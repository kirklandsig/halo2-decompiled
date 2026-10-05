// @flags /O2 /arch:SSE /Gr
/* FONT_LOADING.CPP: the fonts: the font table (font_table[_<language>].txt)
   names up to 11 font files, which are copied from the DVD (d:\maps\fonts\)
   to the utility drive (z:\fonts\) when the language or the files change, and
   whose headers (with the kerning pairs) are read asynchronously into a cache
   of 10 entries. */

#include "unknown_11c920.h"
#include "async.h"
#include "font_loading.h"
#include "language.h"
#include "unknown_120d80.h"
#include <xtl.h>
#include <string.h>
#include <stddef.h>

#define k_font_header_version 0xf0000001

struct s_type_acf665
{
	dword signature;
	word flags;
	short location;
	char path[256];
	byte unknown108[8];
};

char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char const *function_11cb00(long language);
void function_137320(char *path, const char *name);
void function_1373c0(char *path);
bool function_1368f0(s_type_acf665 *file);
char *function_122810(char *string, const char *suffix);
bool function_120ce0(long job, long priority);
void global_preferences_flush(void);


char const *g_4687f0 = "z:\\fonts\\";
char const *g_4687f4 = "d:\\maps\\fonts\\";
long g_4687f8 = NONE;
char const *g_55e718;

long g_4e28f4[11];
s_font_cache_entry g_4e2920[k_maximum_font_count];
bool g_4e3b40;

static inline void csstrncpy(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
}

static inline void function_x454397(s_type_acf665 *reference)
{
	memset(reference, 0, sizeof(*reference));
	reference->signature = 'filo';
	reference->location = NONE;
}

static inline void function_x73bce5(s_type_acf665 *reference, char const *name)
{
	if (reference->flags & 1)
		function_1373c0(reference->path);
	function_137320(reference->path, name);
	reference->flags |= 1;
}

// @retail 0x121570
char *font_table_get_name(char *buffer, long buffer_size)
{
	char name[256];
	s_type_acf665 reference;
	char path[256];
	char const *language;

	name[0] = 0;
	language = function_11cb00(get_current_language());
	if (g_4687f8 != 0)
	{
		csstrncpy(name, "font_table", sizeof(name));
		if (*language)
		{
			function_122810(name, "_");
			function_122810(name, language);
		}
		function_122810(name, ".txt");
		if (g_4687f8 == NONE)
		{
			path[0] = 0;
			csstrncpy(path, g_4687f4, sizeof(path));
			function_122810(path, name);
			function_x454397(&reference);
			function_x73bce5(&reference, path);
			if (function_1368f0(&reference))
				g_4687f8 = get_current_language();
			else
				g_4687f8 = 0;
		}
	}
	if (g_4687f8 == 0)
	{
		csstrncpy(name, "font_table", sizeof(name));
		function_122810(name, ".txt");
	}
	csstrncpy(buffer, name, buffer_size);
	return buffer;
}

// @retail 0x121730
void fonts_get_source_directory(s_type_acf665 *reference)
{
	char directory[256];

	g_55e718 = "d:\\maps\\";
	function_11c9c0(directory, sizeof(directory), "%sfonts\\", "d:\\maps\\");
	function_x454397(reference);
	function_137320(reference->path, directory);
}

bool function_136df0(s_type_acf665 *file, FILETIME *time);

/* when the font table on the DVD was last written */
// @retail 0x1219b0
bool font_table_get_time(FILETIME *time)
{
	s_type_acf665 directory;
	s_type_acf665 reference;
	char name[256];

	time->dwLowDateTime = 0;
	fonts_get_source_directory(&directory);
	/* the reference without its file handle and position */
	memcpy(&reference, &directory, offsetof(s_type_acf665, unknown108));
	font_table_get_name(name, sizeof(name));
	function_x73bce5(&reference, name);
	return function_136df0(&reference, time);
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

static inline void csstrnlwr(char *string, dword size)
{
	for (char *c = string; *c && size-- > 0; c++)
	{
		char character = *c;

		if (character >= 'A' && character <= 'Z')
			character += 'a' - 'A';
		*c = character;
	}
}

static inline void file_path_add_name_inline(char *path, const char *name)
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

struct file_reference_data;
void async_create_file_blocking(file_reference_data const *s_type_acf665, dword access_flags, long disposition, dword file_flags, long category, s_file_handle *file);

/* reads a small text file (the font table) into a string; false when it
   is missing or empty */
// @retail 0x121a40
bool file_read_string(s_type_acf665 const *reference, char *buffer, long size)
{
	bool result = false;
	s_file_handle file;
	long bytes_read = 0;
	bool volatile done;

	async_create_file_blocking((file_reference_data const *)reference, 1, 0, 4, 7, &file);
	if (file.handle != (void *)NONE)
	{
		function_1a0f10(file, buffer, size, 0, 7, 6, (dword *)&bytes_read, &done);
		function_120d50(&done, false);
		function_1a1550(file, 7, 6, &done);
		function_120d50(&done, false);
		buffer[bytes_read > size - 1 ? size - 1 : bytes_read] = 0;
		result = bytes_read != 0;
	}
	return result;
}

/* the font files a font table names (up to 11, each once), in the given
   directory; returns how many it names */
// @retail 0x121790
long font_table_parse(char const *text, s_type_acf665 const *directory, s_type_acf665 *files, long maximum_count)
{
	long count = 0;
	char *names[11];
	char *next;
	char buffer[0x800];
	char *token;

	strncpy(buffer, text, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = 0;
	for (token = function_xe9ecc4(buffer, "\t\n\r ", &next); token; token = function_xe9ecc4(next, "\t\n\r ", &next))
	{
		bool found = false;

		for (long i = 0; i < (count > 11 ? 11 : count); i++)
		{
			csstrnlwr(token, 256);
			if (!strcmp(names[i], token))
			{
				found = true;
			}
		}
		if (!found && count < 11)
		{
			names[count] = token;
			if (count < maximum_count)
			{
				s_type_acf665 *file = &files[count];

				memcpy(file, directory, 0x108);
				if (file->flags & 1)
				{
					function_1373c0(file->path);
				}
				file_path_add_name_inline(file->path, token);
				file->flags |= 1;
			}
			count++;
		}
	}
	return count;
}

// @retail 0x1222d0
long __stdcall function_1222d0(s_async_task *task)
{
	s_font_cache_entry *entry = &g_4e2920[task->function_1223a0.font_index];
	bool finished = false;

	if (entry->file.handle == INVALID_HANDLE_VALUE)
	{
		char path[256];

		function_11c9c0(path, sizeof(path), "%s%s", g_4687f0, task->function_1223a0.name);
		entry->file.handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
		if (entry->file.handle == INVALID_HANDLE_VALUE)
		{
			entry->pending = finished;
			finished = true;
		}
	}
	else
	{
		DWORD bytes_read;

		SetFilePointer(entry->file.handle, 0x200, NULL, FILE_BEGIN);
		ReadFile(entry->file.handle, &entry->header, sizeof(entry->header), &bytes_read, NULL);
		finished = true;
	}
	return finished ? 1 : 0;
}

// @retail 0x1223a0
void function_1223a0(long font_index, char const *name, bool wait)
{
	s_font_cache_entry *entry = &g_4e2920[font_index];
	s_async_task task;

	*(volatile bool *)&entry->pending = true;
	memset(&task, 0, sizeof(task));
	csstrncpy(task.function_1223a0.name, name, sizeof(task.function_1223a0.name));
	task.function_1223a0.font_index = font_index;
	entry->file.handle = INVALID_HANDLE_VALUE;
	entry->task = function_120ba0(wait ? 6 : 2, &task, 7, function_1222d0, &entry->done);
	if (wait)
	{
		function_120d50(&entry->done, false);
		if (entry->header.version != k_font_header_version && global_preferences_globals.current.unknown1c != NONE)
		{
			global_preferences_globals.current.unknown1c = NONE;
			global_preferences_globals.dirty = true;
			global_preferences_flush();
		}
	}
}

// @retail 0x1224c0
s_font_header *font_get(long font_index)
{
	s_font_cache_entry *entry = &g_4e2920[font_index];
	s_font_header *result = NULL;

	if (font_index >= 0 && font_index < k_maximum_font_count && (entry->done || entry->pending))
	{
		if (!entry->done)
		{
			function_120ce0(entry->task, 6);
			function_120d50(&entry->done, false);
		}
		result = &entry->header;
	}
	return result;
}

// @retail 0x122540
long function_122540(long font)
{
	s_font_header *header = font_get(g_4e28f4[font]);
	long result = 10;

	if (header)
		result = header->leading_height + header->descending_height + header->ascending_height;
	return result;
}

// @retail 0x122570
short function_122570(s_font_header const *header, dword first_character, dword second_character)
{
	short result = 0;

	if (header && first_character && second_character && first_character <= 0xff && second_character <= 0xff &&
		(header->kerning_characters[first_character >> 5] & (1 << (first_character & 31))))
	{
		long index = 0;

		do
		{
			if (header->kerning_pairs[index].first_character >= first_character)
				break;
			index++;
		}
		while (index < header->kerning_pair_count);
		if (index < header->kerning_pair_count && header->kerning_pairs[index].first_character == first_character)
		{
			do
			{
				if (header->kerning_pairs[index].second_character >= second_character)
					break;
				index++;
			}
			while (index < header->kerning_pair_count && header->kerning_pairs[index].first_character == first_character);
			if (index < header->kerning_pair_count && header->kerning_pairs[index].first_character == first_character &&
				header->kerning_pairs[index].second_character == second_character)
			{
				result = header->kerning_pairs[index].offset;
			}
		}
	}
	return result;
}

/* a pixel of a character: 3 bits of alpha (widened to 4) over a 12 bit
   color */
static inline long font_character_pixel(long alpha, long color)
{
	return (((alpha << 1) | ((byte)alpha & 1)) << 12) | color;
}

/* decodes a character's run-length coded pixels to 16 bit pixels; returns
   how many pixels there are (with no destination it only counts them) */
// @retail 0x122610
long function_122610(long size, void *destination, void const *pixels)
{
	byte const *source = (byte const *)pixels;
	word *output = (word *)destination;
	long count = 0;
	long color = 0xfff;

	while (size > 0)
	{
		dword code = *source;
		dword type = code >> 6;
		long length;

		if (type > 1)
		{
			long run = 2;

			if (type != 2)
			{
				if (output)
				{
					*output++ = (word)font_character_pixel((code >> 3) & 7, color);
					*output++ = (word)font_character_pixel(*source & 7, color);
				}
				count += run;
			}
			else
			{
				long pixel;

				if (output)
					*output++ = (word)font_character_pixel((code >> 3) & 7, color);
				count++;
				switch (*source & 7)
				{
				case 0:
					run = 0;
					count += run;
					break;
				case 1:
					run = 4;
					pixel = color | 0xf000;
					goto fill;
				case 2:
					run = 3;
					pixel = color | 0xf000;
					goto fill;
				case 3:
					pixel = color | 0xf000;
					goto fill;
				case 4:
					run = 5;
					pixel = color;
					goto fill;
				case 5:
					run = 4;
					pixel = color;
					goto fill;
				case 6:
					run = 3;
					pixel = color;
					goto fill;
				case 7:
					pixel = color;
				fill:
					if (output)
					{
						for (long i = 0; i < run; i++)
							output[i] = (word)pixel;
						output += run;
					}
					count += run;
					break;
				default:
					__assume(0);
				}
			}
			length = 1;
		}
		else
		{
			long run = code & 0x3f;

			if (run == 0 && type == 0)
			{
				word value = (word)((source[1] << 8) | source[2]);

				color = value & 0xfff;
				if (output)
					*output++ = value;
				count++;
				length = 3;
			}
			else
			{
				if (output)
				{
					word pixel = (word)font_character_pixel((type == 0 ? 0 : 0xff) & 7, color);

					for (long i = 0; i < run; i++)
						output[i] = pixel;
					output += run;
				}
				count += run;
				length = 1;
			}
		}
		size -= length;
		source += length;
	}
	return count;
}
