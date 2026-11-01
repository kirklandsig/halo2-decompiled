// @flags /O2 /arch:SSE /Gr
#include "font_loading_storage.h"
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

bool file_ref_modified_time(s_type_acf665 *file, FILETIME *time);

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
	return file_ref_modified_time(&reference, time);
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
bool file_read_string(s_type_acf665 const *reference, long size, char *buffer)
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

long function_136770(dword flags, file_reference_data *directory, long maximum_count, file_reference_data *files);
bool function_1367d0(s_type_acf665 *file);
bool file_ref_delete(s_type_acf665 *file);

static inline void font_copy_wait(bool volatile *done)
{
	if (!*done)
		while (!*done)
			SwitchToThread();
}

// score 4 (was 4): the three font file arrays sit in the frame in another
// order: retail puts the old directory's files first, then the source's,
// ours puts the source's first and the old ones in the middle.
// @retail 0x121b00
bool function_121b00(void)
{
	s_type_acf665 source;
	s_type_acf665 destination;
	s_type_acf665 table;
	s_type_acf665 output;
	s_type_acf665 source_files[11];
	s_type_acf665 destination_files[11];
	char name[256];
	char path[256];
	char table_name[256];
	char text[0x800];
	FILETIME time;
	FILETIME saved_time;
	bool done;
	s_file_handle input;
	dword bytes_written;
	bool result;
	fonts_get_source_directory(&source);
	function_x454397(&destination);
	function_137320(destination.path, g_4687f0);
	memcpy(&table, &source, 0x108);
	font_table_get_name(table_name, sizeof(table_name));
	function_x73bce5(&table, table_name);
	result = font_table_get_time(&time);
	if (result)
	{
		{
			long language = global_preferences_globals.current.unknown1c;
			memcpy(&saved_time, global_preferences_globals.current.unknown24, sizeof(saved_time));
			if (CompareFileTime(&saved_time, &time) == 0)
			{
				if (g_4687f8 == NONE) font_table_get_name(name, sizeof(name));
				if (language == g_4687f8) return result;
			}
		}
		if (global_preferences_globals.current.unknown1c != NONE)
		{
			global_preferences_globals.current.unknown1c = NONE;
			global_preferences_globals.dirty = true;
			global_preferences_flush();
		}
		global_preferences_flush();
		result = file_read_string(&table, sizeof(text), text);
		if (result)
		{
			long count = font_table_parse(text, &source, source_files, 11);
			font_table_parse(text, &destination, destination_files, 11);
			if (function_1368f0(&destination))
			{
				s_type_acf665 old_files[11];
				long old_count = function_136770(0, (file_reference_data *)&destination, 11, (file_reference_data *)old_files);
				for (long i = 0; i < old_count; i++) file_ref_delete(&old_files[i]);
			}
			else function_1367d0(&destination);
			path[0] = 0;
			font_table_get_name(name, sizeof(name));
			csstrncpy(path, g_4687f0, sizeof(path));
			function_122810(path, name);
			function_x454397(&output);
			function_x73bce5(&output, path);
			async_create_file_blocking((file_reference_data *)&output, 2, 2, 4, 7, &input);
			dword text_size = strlen(text) + 1;
			s_file_write_flags write_flags(0);
			function_1a1050(input, text, text_size, 0, write_flags, 7, 6, &bytes_written, &done);
			font_copy_wait(&done);
			async_flush_file_blocking(input, 7);
			function_1a1550(input, 7, 6, &done);
			font_copy_wait(&done);
			for (long i = 0; i < count; i++)
			{
				s_file_handle target;

				async_create_file_blocking((file_reference_data *)&source_files[i], 1, 0, 4, 7, &input);
				result = input.handle != INVALID_HANDLE_VALUE;
				if (!result) goto failed;
				async_create_file_blocking((file_reference_data *)&destination_files[i], 2, 2, 4, 7, &target);
				result = target.handle != INVALID_HANDLE_VALUE;
				if (!result) goto failed;
				result = async_copy_file(input, target, 7);
				if (!result) goto failed;
				async_flush_file_blocking(target, 7);
				function_1a1550(input, 7, 6, &done);
				font_copy_wait(&done);
				function_1a1550(target, 7, 6, &done);
				font_copy_wait(&done);
			}
			if (g_4687f8 == NONE) font_table_get_name(name, sizeof(name));
			global_preferences_globals.current.unknown1c = g_4687f8;
			memcpy(global_preferences_globals.current.unknown24, &time, sizeof(time));
			global_preferences_globals.dirty = true;
			return result;
		}
	}
failed:
	if (global_preferences_globals.current.unknown1c != NONE)
	{
		global_preferences_globals.current.unknown1c = NONE;
		global_preferences_globals.dirty = true;
		global_preferences_flush();
	}
	global_preferences_flush();
	return result;
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
			{
				if (header->kerning_pairs[index].first_character == first_character)
				{
					do
					{
						if (header->kerning_pairs[index].second_character >= second_character)
						{
							if (header->kerning_pairs[index].second_character == second_character)
								result = header->kerning_pairs[index].offset;
							break;
						}
						index++;
					}
					while (index < header->kerning_pair_count && header->kerning_pairs[index].first_character == first_character);
				}
				break;
			}
			index++;
		}
		while (index < header->kerning_pair_count);
	}
	return result;
}

/* a pixel of a character: 3 bits of alpha (widened to 4) over a 12 bit
   color */
static inline word font_character_pixel(long alpha, long color)
{
	return (((alpha << 1) | ((byte)alpha & 1)) << 12) | color;
}

/* decodes a character's run-length coded pixels to 16 bit pixels; returns
   how many pixels there are (with no destination it only counts them) */
// score 85 (was 85): the alpha parity is still masked in 16 bits (and cl, 1; movzx cx, cl) where
// retail zeroes a 32-bit register first (xor ecx, ecx; mov cl, al; and ecx, 1); every 32-bit form of
// the pixel helper reproduces retail's parity bytes but swaps the output and colour registers, so
// the 16-bit helper is kept.
// Frame 0xc agrees; alpha extension, literal assembly and run-two reuse differ.
// Alpha parity extension, literal-word assembly and run=2 reuse remain. Disjoint
// shifted/multiplied alpha fields 204/194; long/word/unsigned opacity nibble 172/236/172; word
// color-first/alpha-first and long color-first assembly 175/228/172. Original pixel helper and
// decoder restored.
// Earlier: retail zero-extends the alpha byte to 32 bits before the
// `& 1` (xor ecx, ecx; mov cl, al; and ecx, 1) where ours masks the byte and
// widens it to 16 bits (and cl, 1; movzx cx, cl); retail builds the colour
// word from two byte loads (movzx ax, byte [edi+2]; mov ch, byte [edi+1])
// where ours builds it in ah/al, and retail adds the type 3 run with a
// register (add eax, ebx) where ours folds it to a constant (add eax, 2).
// Tried: a cast-free helper (204: the switch's run moves to ebp and the three
// count updates merge into one tail); the fill after the switch instead of a
// goto to it (255 without the default case, 81 with it); a dword colour
// value (81, no change); a byte or unsigned char alpha parameter (201, and
// 209 with a long result); a byte local for the low alpha (81, no change);
// an int-typed parity term (81, no change); the colour spelled low byte
// first, as a long or in two steps (81, no change in every case).
// Word colour input 85; short alpha 91; Boolean parity and word result 85;
// named upper/lower alpha 85; two-byte payload memcpy 88;
// switch-fill endpoint 174; ordinary-fill endpoint 199.
// Retail zero-extends the alpha byte to 32 bits before the `& 1` in every
// branch (xor ecx, ecx; mov cl, al; and ecx, 1); ours does the `& 1` on the
// byte and widens it to 16 bits (and cl, 1; movzx cx, cl) where the pixel is
// stored straight to a word. A long return from the helper (108) fixes the
// run branch for type 0/1 but not the two stores; a named long for the
// single pixel of type 2 (234: widens there but merges the two fills); named
// longs for the type 3 stores (108); a byte alpha parameter (201-209); the
// byte casts at the call sites (108); (dword) or no casts at the stores (108).
// Round 182: Packed-class tests and positive input endpoints leave alpha extension and literal assembly unresolved
// Trials 1-8: 87, 87, 186, 89, 187, 188, 189, 188.
// Trials 9-16: 180, 184, 180, 210, 184, 210, 210, 210.
// Round 209: Split alpha/parity helper inputs: numeric and doubled numeric 200/200, Boolean and doubled
// Boolean 90/90; long-return counterparts 212/212/194/194. All restored; dword alpha parity, literal
// assembly and run-two reuse remain.
// Remainder alpha parity: signed OR/add 185/191; unsigned OR/add 204/228;
// each with one run workspace 185/191/204/228. Helper and decoder restored.
// Round 231: native byte-header class / high-alpha / low-alpha field subsets 1-7,
// then all fields with the native six-bit count, 85 each. Original input view restored.
// Round 448: Indexed two-pixel stores with one run-based advance; full-width parity,
// word fill pixels and default one-byte consumption, all eight combinations 161/213/153/217/131/203/122/211.
// Original body and helpers restored; Frame 0xc agrees; alpha extension, literal assembly and run-two reuse remain.
// Round 212: Conditional expansion, word/long results: one expanded-alpha result 216/220;
// complete-pixel result 205/210; two-path alpha assignment 197/205;
// separate complete-pixel returns 195/199. All restored; parity branches and
// pointer-convention changes regress. Alpha width, literal assembly and run-two reuse remain.
// Round 245: plain two-byte literal parser, word/dword accumulator, with full parity and
// native word fill interactions 85/85/204/204/107/107/208/208. Original helper and decoder restored.
// Round 270: Complete branch/case-owned run production with long/word run and pixel,
// existing fill boundary 92/92/164/164; common post-switch fill 149/149/137/137.
// Original run producer, switch, pixel helper and decoder restored.
// Round 470: signed helper/destination/both 85/85/85; signed fill carriers alone,
// with helper/destination/both 109 each; all with signed color 109. All restored;
// alpha extension, literal assembly and run-two reuse remain.
// Round 275: Pointer stores in switch/ordinary/both fill loops 172/172/164; full parity 228/228/236; both with word pixels 166, full parity and word pixels 240.
// Original executable source restored; alpha extension, literal assembly and two-run reuse remain.
// Round 277: Complete alpha-only pixel helper with word/long result and byte/numeric parity, OR
// color 110/204/105/212; disjoint-add color 113/206/108/214. Original helper and decoder restored;
// alpha extension, literal fusion and run-two reuse remain.
// @retail 0x122610
long function_122610(long size, void const *pixels, void *destination)
{
	long remaining = size;
	long count = 0;
	byte const *source = (byte const *)pixels;
	word *output = (word *)destination;
	long color = 0xfff;

	while (remaining > 0)
	{
		dword code = *source;
		dword type = code >> 6;
		long length;

		if ((long)type > 1)
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
					goto counted;
				case 4:
					run = 5;
					pixel = color;
					goto fill;
				case 1:
					run = 4;
					pixel = color | 0xf000;
					goto fill;
				case 6:
					run = 3;
					pixel = color;
					goto fill;
				case 2:
					run = 3;
					pixel = color | 0xf000;
					goto fill;
				case 3:
					pixel = color | 0xf000;
					goto fill;
				case 5:
					run = 4;
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
				counted:
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
					byte alpha = type == 0 ? 0 : 7;
					long pixel = font_character_pixel(alpha, color);

					for (long i = 0; i < run; i++)
						output[i] = (word)pixel;
					output += run;
				}
				count += run;
				length = 1;
			}
		}
		remaining -= length;
		source += length;
	}
	return count;
}

s_font_loading_storage g_font_loading_storage;
