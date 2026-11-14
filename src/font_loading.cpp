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
static inline long font_character_pixel(long alpha, long color)
{
	return (((alpha << 1) | ((byte)alpha & 1)) << 12) | color;
}

/* decodes a character's run-length coded pixels to 16 bit pixels; returns
   how many pixels there are (with no destination it only counts them) */
// MiniMax G (11-14): no new form, no lower score (59): the alpha parity is still masked in sixteen
// bits (and cl, 1; movzx cx, cl) where retail widens it to thirty-two (xor ecx, ecx; mov cl, al; and
// ecx, 1), the literal word is still built from two byte loads and a multiply (movzx eax, byte [edi+1];
// movzx edx, byte [edi+2]; shl eax, 8; add eax, edx) where retail loads the low byte into ax and the
// high byte into ch, the type 3 run is still folded to the constant 2 (add eax, 2; add esi, 2) where
// retail adds the run register (add eax, ebx; add esi, ebx), and the body is eight bytes longer than
// retail's.
// MiniMax G (11-13): no new form, no lower score (85): the alpha parity is still masked in sixteen
// bits (and cl, 1; movzx cx, cl) where retail widens it (xor ecx, ecx; mov cl, al; and ecx, 1); a
// permuter run (165 tries) found nothing under the baseline and a thirty-two-bit parity local
// scores 170 and swaps the output and colour registers.
// MiniMax H (11-13): lower score, 59 from 85: the pixel helper now returns a long so the colour is ored in
// thirty-two bits and the fill loop matches retail's, the literal word is a multiply of the high byte so the
// decoder's row count matches retail's again, and case 1 assigns the colour before the run; what still differs
// is the parity width, the literal word's exact loads, the run-2 reuse and the body's length.
// MiniMax H (11-13): no new form, no lower score (85): the check's right column is retail, so retail widens the pixel's parity in thirty-two bits (xor ecx,ecx; mov cl,al; and ecx,1) where this build narrows it to sixteen bits (mov cl,al; and cl,1; movzx cx,cl); a word-typed parity (180), a byte-typed parity (222), a byte-truncated parity (85) and a separate parity helper (85) all widen it, and the same source linked on its own gives retail's form, so the width follows the link context and not the source.
// MiniMax B (11-13): no new form, no lower score (85): the pixel in a long local inside the word helper (226), the same without the byte cast (172, retail's length) and the parity in a long local (230) all widen the parity to thirty-two bits and score worse.
// MiniMax A (11-13): no new form, no lower score (85): the permuter (1800 s, 165 tries, with annealing) found nothing under the baseline; a long-returning helper now reaches retail's 504-byte length but keeps the parity in sixteen bits (105), a byte alpha parameter scores 201/209, and writing the type-2 count as the literal 2 leaves the body unchanged (85), so the alpha's parity is still masked in sixteen bits where retail widens it to thirty-two.
// MiniMax B (11-12): no new form, no lower score (85): the body is 512 bytes against retail's 504 and the first
// difference is the parity test, retail zeroing ECX before the shift where this build shifts first and tests the low bit
// of the shifted value; the nine decode spellings with the case-1 order swapped measure 83 and the multiplication forms 109.
// MiniMax H (11-11): no new form, no lower score (85): retail widens the alpha's parity in thirty-two bits (xor ecx,ecx; mov cl,al; add eax,eax; and ecx,1; or ecx,eax) where ours narrows it to sixteen bits (and cl,1; movzx cx,cl), and the body is eight bytes longer than retail's; a helper returning a long scores 105, a parity kept in a long local 170, a pixel kept in a local at the second site 234 and a byte alpha parameter 209, so the word helper and the committed body stay.
// MiniMax S (11-11): no new form, no lower score (85); the pixel helper returning a long scores 105, taking the alpha as a byte scores 201, widening the parity with a long cast scores 105 and dropping the byte cast scores 204, so the word helper stays.
// MiniMax I (11-10, run 56): no new form; a long-returning helper (105) and a byte alpha parameter (209)
// keep the parity masked in sixteen bits, so the word helper stays the best.
// MiniMax I (11-10): no new form, no lower score (85): the alpha parity is still masked in sixteen bits
// (and cl, 1; movzx cx, cl) where retail widens it (xor ecx, ecx; mov cl, al; and ecx, 1); a count
// increment shared by both sub-branches scores 203 and puts the run in ebp, an unsigned modulo parity 212
// and a thirty-two-bit parity local 170, which swaps the output and colour registers.
// MiniMax G (11-06): no new form, no lower score: routing the helper's result through a long local and
// casting it back to word gives 226, and the parity stays masked in 16 bits (and cl, 1; movzx cx, cl)
// where retail widens it to 32 bits (xor ecx, ecx; mov cl, al; and ecx, 1).
// MiniMax D (11-06): no new form, no lower score: the alpha parity is still masked in 16 bits where retail widens
// it to 32 bits, and the literal word is still assembled in one register; a swapped add literal 110, a long-typed
// literal 85, a dword count 204, a separate switch-run variable 158 and a cast-free helper 204 all leave the
// alpha parity masked in 16 bits (and cl, 1; movzx cx, cl) where retail zeroes a 32-bit register first
// (xor ecx, ecx; mov cl, al; and ecx, 1).
// MiniMax S (11-05): no new form, no lower score: a long helper result 105, an unmasked parity with a word run pixel
// 208, an int or a long cast on the parity 85, a byte parity local 85 and the OR operands swapped 85 all leave the
// alpha parity masked in 16 bits where retail widens it to 32 bits.
// MiniMax B (11-04): no new form, no lower score: the alpha parity is still masked in 16 bits where retail
// widens it to 32 bits; a `(long)(byte)` cast 85, a byte alpha parameter 201, a named long parity 176, a long
// helper return 212 and a long alpha 105 keep the structure but not the parity bytes, and the best form found
// (the case-1 pixel assigned before the run, with the permuter's four other edits) gives 83, below the keep
// threshold, so the original body was restored.
// MiniMax A (11-03): no new form, no lower score: the alpha parity is still masked in 16 bits where retail
// widens it to 32 bits, and the literal word assembly and the run-two reuse still differ; dropping the byte
// cast gives 204.
// MiniMax H (11-12): no new form, no lower score (85): a 600 s permuter run (79 differing instructions at its baseline) found nothing lower, and the first branch still leaves for a different place than retail's, which jumps into the shared exit further down.
// score 59 (was 85): the alpha parity is still masked in sixteen bits (and cl, 1; movzx cx, cl) where
// retail zeroes a thirty-two bit register first (xor ecx, ecx; mov cl, al; and ecx, 1), the literal word
// is still built from two byte loads and a multiply (movzx eax, byte [edi+1]; movzx edx, byte [edi+2];
// shl eax, 8; add eax, edx) where retail loads the low byte into ax and the high byte into ch, the type 3
// run is still folded to a constant (add eax, 2; add esi, 2) where retail adds the run register
// (add eax, ebx; add esi, ebx), and the body is still eight bytes longer than retail's; the frame, the
// fill loops, the switch layout and the exit agree.
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
// Round 489: signed byte alpha input with word/long helper result, long/word fills
// and byte/full parity, eight combinations 201/205/205/205/201/209/205/205.
// Original alpha type, helper, fill values and complete decoder restored.
// Round 324: persistent color as word/short, each with original/full parity and
// long/word fill pixels, 94/107/201/208/94/107/201/201. Original helper and body
// restored; alpha extension, literal assembly and run-two reuse remain.
// Round 331: Count accumulation before two-pixel/single-and-literal/ordinary-run production, subsets
// 1-7: 133/172/209/176/170/178/213; all with full-width pixel helper 204. Original count lifetimes,
// helper and decoder restored; alpha extension, literal assembly and two-run reuse remain.
// Round 489: signed byte alpha input with word/long helper result, long/word fills
// and byte/full parity, eight combinations 201/205/205/205/201/209/205/205.
// Original alpha type, helper, fill values and complete decoder restored.
// Round 216: Plain packed-alpha triplet loop, long/short shift counter with word/long helper result and byte/full parity:
// 164/218/172/222/238/222/183/230. Original executable source restored.
// Round 275: Pointer stores in switch/ordinary/both fill loops 172/172/164; full parity 228/228/236; both with word pixels 166, full parity and word pixels 240.
// Original executable source restored; alpha extension, literal assembly and two-run reuse remain.
// Round 277: Complete alpha-only pixel helper with word/long result and byte/numeric parity, OR
// color 110/204/105/212; disjoint-add color 113/206/108/214. Original helper and decoder restored;
// alpha extension, literal fusion and run-two reuse remain.
// Round 280: native pixel split color/parity/alpha fields, word/dword and both
// production orders 201/201/195/195; complete opacity field with byte/numeric
// parity, word/dword 211/201/239/195. Exact-only records, helper and full decoder
// restored; alpha extension, literal assembly and run-two reuse remain.
// Round 296: Consumed spans short/word/byte/signed-char with long/dword counts, then byte/signed-
// char/short/word spans with the long pixel-helper result. Scores
// 101/101/91/91/101/101/91/91/98/98/99/99. Original complete body restored.
// Round 489: signed byte alpha input with word/long helper result, long/word fills
// and byte/full parity, eight combinations 201/205/205/205/201/209/205/205.
// Original alpha type, helper, fill values and complete decoder restored.
// Round 216: Plain packed-alpha triplet loop, long/short shift counter with word/long helper result and byte/full parity:
// 164/218/172/222/238/222/183/230. Original executable source restored.
// Round 285: short fill counters switch/ordinary/both 181/172/191; word counterparts
// 181/172/191; both byte/signed-byte 178/178. Original helper, decoder and counters restored;
// alpha extension, literal assembly and two-run reuse remain.
// Round 290: paired word color with byte/signed-byte/short/word alpha and word/long helper result,
// 201/209/201/205/92/91/92/91; frame 0xc agrees throughout. Original helper and decoder restored;
// parity extension, literal assembly and two-run residency remain.
// Round 296: Consumed spans short/word/byte/signed-char with long/dword counts, then byte/signed-
// char/short/word spans with the long pixel-helper result. Scores
// 101/101/91/91/101/101/91/91/98/98/99/99. Original complete body restored.
// Round 450: Literal color/store/count orders SCN/SNC/NSC/NCS, word then long pixel helper:
// 183/180/182/121/143/171/142/116. Original literal publication, helper, aliased source reads and
// complete decoder restored; parity extension, literal assembly and two-run reuse remain.
// Round 302: native word store owned by existing pixel helper, pointer/reference output and inline/force-inline:
// byte parity 107/107/107/107; full parity 208/208/208/208. Original helper and decoder restored;
// alpha widening, literal assembly and two-run reuse remain.
// DL231: Shared first packed-pixel output, full-width helper/word fills/common first count combinations:
// 188/196/195/195/188/200/199/199. Alpha extension, literal assembly and two-run reuse remain; original
// executable source restored.
// Round 249: Standard fill_n/fill independently at switch/ordinary word-fill boundaries:
// 173/165/246/173/138/199/194/214. All but ordinary fill also disturb the matched caller in
// preview. Original loops/header/helper/decoder restored; parity extension, literal assembly and
// run-two reuse remain.
// N round 296: Standard endpoint/count-based fill contracts independently in switch/ordinary
// run families, all eight nonoriginal combinations 138/165/173/214/199/173/194/246. Original
// decoder, helper and includes restored; alpha extension, literal assembly and run-two reuse remain.
// Round 299: Native literal word/dword byte views, high/low-first and original/full helper parity:
// 141/141/137/137/183/183/180/180. Exact-only representation forms and the entire original helper/body
// restored; alpha extension, literal assembly and run-two reuse remain.
// N302: Literal owns two payload bytes, skip after publication / read-consuming cursor,
// crossed with full parity and native word fill values: 131/135/207/202/119/125/211/207. Original
// decoder/helper restored; alpha extension, literal assembly and run-two reuse remain.
// N306: Native byte header/class and word run spans, seven subsets, then all with signed-word
// runs: 85/186/189/171/180/174/176/176. Original carriers, helper and decoder restored; alpha widening,
// literal assembly and run-two reuse remain.
// N308: Independent low/high literal parts word/word, word/long, long/word, long/long,
// 85/85/85/85; with numeric parity in the existing word helper 204/204/204/204. Original
// literal expression, helper and decoder restored; alpha extension, literal assembly and run-two reuse remain.
// Round 310: Existing pixel writer returns the actual next output cursor, with numeric parity/
// statement advance/word packed-run subsets 0..7: 188/214/189/214/185/181/183/179.
// Original scalar producer, ordinary fill and decoder restored; alpha widening,
// literal assembly and constant-two reuse remain.
// N313: Shared actual run workspace under the retained word pixel helper; numeric parity/word fill
// subsets 0..3: 85/204/107/208; with native byte consumed span 91/204/110/208. Shared run alone
// ties the opening body; all helper and domain combinations regress. Original private helper,
// independent runs and complete decoder restored; parity extension, literal fusion and two-run reuse remain.
// N316: Coherent signed source octets with numeric parity/word fill/unsigned count combinations:
// 110/192/103/200/110/192/103/200. Original source octets, helper, counts and fills restored; alpha
// width, literal fusion and two-run reuse remain.
// N318: Literal command completes all three input bytes and continues; fixed one-byte common
// tail, numeric parity/word fill/unsigned count masks 0..7: 128/188/119/192/128/188/119/192.
// Original consumed span, helper and decoder restored; alpha width, literal fusion and two-run reuse remain.
// P280: Existing scalar pixel helper static force-inline/ordinary static/external
// inline/external force-inline: original parity 85/85/85/85, numeric parity
// 204/204/204/204. Original helper and decoder restored; parity/literal/two-run gaps remain.
// N339: Existing alpha formal as complete expansion accumulator, byte/long parity before shift,
// shifted-source parity and conditional low-bit publication, word then long result:
// 179/222/169/227/136/230/216/236. Original helper and full decoder restored;
// alpha extension, literal assembly and run-two reuse remain.
// NOT kept: every accumulator form regresses and remains unmatched.
// M341: Native literal word plus network-order conversion, direct/memcpy and word/long
// values, original/numeric parity: 104/104/121/121/196/196/192/192. Original literal, helper and
// decoder restored; alpha widening, literal assembly and two-run reuse remain.
// N342: Single-first upper dispatch / lower-first dispatch / flat pair-single / flat single-pair,
// original word then full-width pixel result: 139/172/182/183/163/166/174/175.
// NOT kept: dispatch ownership regresses; original helper, cases, observations and body restored.
// J248: Unsigned dword alpha with word/short/dword/unsigned-int color,
// each with word/dword pixel result and the original byte-parity expression:
// 85/106/85/106/85/105/85/105. NOT kept: promotion/interface nonmatches; original
// helper and decoder restored; parity, literal assembly and run-two reuse remain.
// Diagnostic scoped size preference 198; NOT kept: compiler setting.
// F361: Complete command-class dispatch, shared 0/1 arm, PSR/SPR/RPS/RSP groups:
// word pixel result 204/221/235/249; long result 196/214/220/225.
// NOT kept: larger extents and unresolved alpha/literal/run-two schedule; original decoder restored.
// DH331: C-runtime word fill at switch/ordinary/both boundaries, long then word fill values,
// then both with full-width pixel result and long/word fills: 86/86/87/108/107/108/106/108.
// Original fills, helper and body restored; alpha widening, literal assembly and two-run reuse remain.
// N348: Stable write/count-only mode shared by all five output checks, bool/byte/short/long:
// 206/206/241/241 in either truth channel. NOT kept: mode storage regresses the frame,
// length and alpha/literal schedules. Original pointer checks, helper and body restored.
// N351: Packed-run case orders 40162357/14062357/61402357/26140357/32614057/53261407/65432107/13502467:
// 90/91/94/93/167/115/116/97. NOT kept: no exact match or qualifying gain; complete original body restored.
// Remaining: alpha extension, literal assembly and run-two reuse.
// N356: Canonical consumed-span for loop, long/byte/signed-char/short with word/long pixel result:
// 87/90/90/100/104/100/100/101. NOT kept: unmatched storage/result-lifetime forms.
// Original executable source restored; Alpha parity widening, literal fusion and constant-two reuse remain.
// N358: Existing color formal owns complete pixel assembly, packed OR/add and high/low-first bit publication, byte/numeric parity:
// 226/230/226/226/172/178/172/172. NOT kept: unmatched source/representation forms; original executable body restored.
// Alpha extension, literal assembly and run-two reuse remain.
// N360: Separate expanded-alpha result accumulator, word/long, disjoint parity/color OR/add:
// 226/226/226/226/226/230/226/226. NOT kept: unmatched source/result-storage forms; original executable source restored.
// Alpha extension, literal assembly and run-two reuse remain.
// N362: Full-width numeric pixel result with eight actual count/remaining/color/run
// lexical families, the last two including source/output roles: 212/212/212/212/212/212/212/212.
// NOT kept: unmatched home/representation forms; original helper and decoder restored.
// Alpha parity widening, literal assembly and constant-two reuse remain.
// N365: Actual byte output cursor with complete word-fill/numeric-parity/bounded-byte-run
// masks 0..7: 85/107/204/208/179/161/172/156. Original word cursor, helper and decoder restored;
// NOT kept: unmatched cursor/domain forms; parity/literal/two-run gaps remain.
// B437: Pure command-pixel producers before output guards, pair/single/fill
// subsets 1/2/4/3/5/6/7 then numeric parity: 180/207/180/211/180/222/230/235.
// NOT kept: all regress; original helper, alpha/literal/two-run graph restored.
// N378: Complete default run pixel, color/opaque, long/word and byte/numeric parity:
// 231/216/231/216/231/182/231/182. NOT kept: no byte equality or qualifying native-shape gain;
// original executable source restored; alpha extension, literal assembly and run-two reuse remain.
// N380: Complete per-command emitted-count result, long/dword/short/word/byte/signed-char,
// then word fills/full-width pixel helper: 164/164/150/150/153/153/173/173.
// NOT kept: no native count/parity match; original helper and complete decoder restored.
// Frame 0xc agrees; alpha widening, literal assembly and run-two reuse remain.
// N382: Pixel-before-run producers in cases 2/4/5/6, joint 2456/12456, then
// 12456/2456 with word packed pixels: 86/85/85/85/86/162/162/86. NOT kept: unmatched schedules; original
// helper, literal, case order, fills and complete decoder restored. Alpha widening, literal
// assembly and two-run reuse remain; frame 0xc agrees.
// N385: Existing standard uninitialized_fill_n/uninitialized_fill at packed/ordinary run
// boundaries, eight nonoriginal indexed/counted/endpoint pairs: 165/184/173/242/198/183/203/214.
// NOT kept: all regress; original indexed fills, helper, includes and entire decoder restored.
// Frame 0xc and alpha extension/literal assembly/two-run differences remain.
// M401: Complete whole-expression pixel result before the original word return, word/short/long/dword/int/unsigned-int/signed-64/unsigned-64:
// 85/85/226/226/226/226/210/210. NOT kept: ties or regressions; original helper and complete decoder restored.
// Frame 0xc agrees; alpha parity widening, literal assembly and run-two reuse remain.
// N389: Pixel alpha word/uint, color short/uint, result short/uint: 92/91/92/80/85/106/85/105.
// NOT kept: word alpha with uint color/result 80 gains only five; other type forms tie or regress.
// Original helper and decoder restored; alpha widening, literal assembly and run-two reuse remain.
// MiniMax E (11-10): no new form, no lower score (85): retail's own case table confirms the committed switch
// cases (the second case runs three with the plain colour and the third keeps the run of two with the opaque
// colour), so the earlier reorder of those cases was wrong and is reverted; a cast-free helper with a long
// result reproduces retail's thirty-two-bit parity at all three pixel sites but moves the run constant into
// ebp and scores 212; a parity kept in a long local inside the word returning helper scores 176 and still
// masks the parity in sixteen bits.
// N391: Case orders 04651237/04615237/04165237/01465237/06415237/06451237/
// 06145237/01645237: 115/116/113/113/117/116/115/114. NOT kept: all regress;
// original cases/body restored. Alpha extension, literal assembly and run-two reuse remain.
// N393: Packed/ordinary indexed, descending-copy forward cursor and reverse real cursor
// fill pairs 01/02/10/11/12/20/21/22: 172/174/172/164/166/175/174/199. NOT kept: unmatched traversal forms;
// original loops/helper/body restored. Alpha widening, literal assembly and run-two reuse remain.
// N398: Scoped existing pixel macro with word/numeric result, byte/numeric parity, byte/long ordinary alpha 165/218/224/236/132/136/226/234.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// MiniMax E (11-08): no new form, no lower score: retail widens the alpha parity with a 32-bit xor and masks it with 1, ours narrows it to sixteen bits and shifts the byte count instead.
// MiniMax E (11-09): no new form, no lower score: retail widens the alpha's parity with a 32-bit xor and adds the constant 2 through ebx, ours narrows the parity to sixteen bits and folds the constant; five helper forms and the switch reorder all scored worse.
// T76: Remaining/source/output formals own their cursors, seven subsets then
// complete consumed-span for ownership: 103/180/87/182/101/180/182/181.
// NOT kept: all regress; most also lose the native frame or incoming roles.
// Original copies, word helper and decoder restored; parity/literal/two-run gaps remain.
// MiniMax D (11-09): no new form, no lower score: the committed body is the best form of this round (85 in the fast and the full check); retail computes the alpha's parity in thirty-two bits (xor ecx,ecx; mov cl,al; add eax,eax; and ecx,1; or ecx,eax) where ours narrows it to sixteen bits (and cl,1; movzx cx,cl), and the long-returning helper, the byte local and the removed casts all scored worse (105, 209).
// MiniMax G (11-11): no new form, no lower score: the parity only widens to thirty-two bits when the byte cast
// leaves the helper, which shrinks the function to 444 bytes and moves the run into ebp (204); a long returning
// helper scores 105 (byte cast) and 212 (no cast); swapping case 1's two statements gives 83, below the gain
// needed to keep it.
// MiniMax R (11-11): no new form tried this round, no lower score (85): the body's first branch is at +0x24, the first argument's zero test, and it leaves for a different place than retail's, which jumps into the shared exit further down; the body is 8 bytes longer than retail's.
// P465: On the long-result/multiplied-literal graph, numeric parity 203; unsigned alpha 203;
// expanded opacity 239; named parity 239; split numeric bands 203; scaled byte-parity field 62;
// disjoint addition 228; split byte-parity bands 59. NOT kept: no native parity/run/literal
// equality; original helper and complete decoder restored.
// P481: On the long-result/multiplied-literal graph, shared actual fill-pixel/run
// workspaces at iteration/entry and consumed-span entry lifetime, eight forms all 59.
// NOT kept: no native parity/literal/run-two recovery; original helper, runs,
// pixels, span and complete executable body restored.
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
					pixel = color | 0xf000;
					run = 4;
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
				dword value = (dword)source[1] * 256 + source[2];

				color = value & 0xfff;
				if (output)
					*output++ = (word)value;
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
