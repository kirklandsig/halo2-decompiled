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

static __forceinline long font_table_language_121b00(void)
{
	if (g_4687f8 == NONE)
	{
		char name[256];
		font_table_get_name(name, sizeof(name));
	}
	return g_4687f8;
}

// score 4 (was 4): the three font file arrays sit in the frame in another
// order: retail puts the old directory's files first, then the source's,
// ours puts the source's first and the old ones in the middle.
// b14F (11-17) walk against retail, fast build on r847: the instructions match; the 4 differing lines are only the
// three arrays' frame offsets (retail 0x121c5e, 0x121ca3, 0x121e4d: old 0xf70, source 0x1b20, destination 0x26d0).
// Declaration order and scope of the arrays, the delete loop's spelling and the parse order change nothing. Only an
// extra use of the old array in the pointer loop `old_file < old_files + old_count` (score 227) moves it first, with
// different loop code.
// source file address from the record byte displacement 4; destination file address from the record byte
// displacement 4; shared per-file byte displacement for both copy lists 4; shared advancing record
// displacement with the ordinal bound 6; single copy displacement bounded by the parsed byte extent 281; old-
// file deletion by bounded record displacement 252; separate old-list and paired copy-list byte displacements
// 252; copy result formed after the two guarded opens 178. The indexed byte forms tie at four and leave the
// same slot exchange. The advancing displacement gains no slot and adds two differences. The extent bounds
// and joined copy result regress. Opening executable source restored.
// Matched in a full build with stand-in SDK headers (was 4): the language read with its lazy name load is one
// file-local helper used at both sites, and the name buffer of the path lives in its own block.
// @retail 0x121b00
bool function_121b00(void)
{
	s_type_acf665 source;
	s_type_acf665 destination;
	s_type_acf665 table;
	s_type_acf665 output;
	s_type_acf665 source_files[11];
	s_type_acf665 destination_files[11];
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
				if (language == font_table_language_121b00()) return result;
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
			{
				char name[256];
				font_table_get_name(name, sizeof(name));
				csstrncpy(path, g_4687f0, sizeof(path));
				function_122810(path, name);
			}
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
			global_preferences_globals.current.unknown1c = font_table_language_121b00();
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
	return (word)((((alpha << 1) | (alpha & 1)) << 12) | color);
}

/* decodes a character's run-length coded pixels to 16 bit pixels; returns
   how many pixels there are (with no destination it only counts them) */
// score 41 (was 59): 508 B (retail 504); first difference at +0x24. With length = 1 written in each arm of the type test (a
// statement written per arm, kept only with a byte match) it reads 12 at 504 B, where only the 4-instruction literal
// assembly at 0x122744 differs (movzx eax/edx; shl; add vs retail movzx ax, [edi+2]; xor ecx, ecx; mov ch, [edi+1]; or eax, ecx).
// MiniMax G (11-20): no new form, no lower score (59): the alpha parity is still masked in sixteen bits where retail masks it in a thirty-two bit register; the colour literal is still built with a shift and an add where retail keeps its two bytes apart; the run of two is still folded into the count as an immediate where retail adds the register.
// Earlier 59 (was 85): The 0xc frame agrees. Dword opacity parity, literal two-byte assembly and the packed run
// register still differ.
// Earlier score 59 (was 85): the alpha parity is still masked in sixteen bits (and cl, 1; movzx cx, cl) where
// retail zeroes a thirty-two bit register first (xor ecx, ecx; mov cl, al; and ecx, 1), the literal word
// is still built from two byte loads and a multiply (movzx eax, byte [edi+1]; movzx edx, byte [edi+2];
// shl eax, 8; add eax, edx) where retail loads the low byte into ax and the high byte into ch, the type 3
// run is still folded to a constant (add eax, 2; add esi, 2) where retail adds the run register
// (add eax, ebx; add esi, ebx), and the body is still eight bytes longer than retail's; the frame, the
// fill loops, the switch layout and the exit agree.
// MiniMax B (11-18, round 2): no new form, no lower score (59): retail widens the alpha's parity to thirty-two bits
// at all three decode sites, and dropping the cast from the shared helper reproduces retail's pixel sequence exactly
// (xor ecx, ecx; mov cl, al; add eax, eax; and ecx, 1; or ecx, eax), but that moves the run to EBP and the count
// update to a read-modify-write, which measures 203 at 444 bytes, and with the parameter typed as a byte 205 at 456;
// the two-byte value read from the character data as (source[1] << 8) | source[2] measures 103 at retail's 504 bytes,
// the run declared out of the type branch 60, and the permuter over the body (two tries in 328 s) found nothing below
// the baseline, so the body stands.
// MiniMax B (11-18): no new form, no lower score (59): the alpha's parity is still masked in sixteen bits at the
// three decode sites where retail widens it to thirty-two, and the type 3 run still stays in EBP where retail keeps
// it in EBX, so the two fills merge; measured this round: the cast dropped (203), the run and the fill run split (183),
// an explicit run of two in case 3 (164) and a word-truncated pixel (192) all regress, the permuter (28 tries in
// 300 s) found nothing below the baseline, and a twin of the shared helper (45) is not kept.
// MiniMax I (11-16): no new form, no lower score (59): the alpha's parity is masked in sixteen bits (mov cl, al; and cl, 1; movzx
// cx, cl) where retail widens it to thirty-two (xor ecx, ecx; mov cl, al; and ecx, 1); dropping the byte cast
// from the shared helper gives retail's parity at every site but moves the run out of EBX into EBP and merges
// the two fill loops (203), and a twin of the shared helper is not kept.
// MiniMax D (11-15): score 45 (was 59): the two decode sites of the type 3 class now take the alpha in a
// full register, so the parity is masked in thirty-two bits (xor ecx, ecx; mov cl, al; and ecx, 1) as
// retail does; still differs in the type 2 decode site, the literal word's loads, the type 3 run (add eax, 2)
// and the switch case 1 order, and the body is four bytes longer than retail's.
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
// MiniMax E (11-17): no new form, no lower score (59): the alpha's parity is still masked in sixteen bits at the three decode sites where retail widens it to thirty-two, and the type 0 value's two bytes still merge into one register (xor eax, eax; mov ah, byte [edi+1]; mov al, byte [edi+2]) where retail merges them across two (movzx ax, byte [edi+2]; xor ecx, ecx; mov ch, byte [edi+1]; or eax, ecx). New this round: widening the cast instead of dropping it ((((long)(byte)alpha) << 1) | ((long)(byte)alpha & 1)) gives retail's parity at the sites but collapses the body (192), widening only the parity ((long)(byte)alpha & 1) narrows back to sixteen bits (59), a byte or unsigned char alpha parameter without the inner cast keeps the run out of ebx (205), the colour spelled with a sixteen bit low half ((dword)(word)source[2] | ((dword)source[1] << 8), or with the low byte in a word local) still merges the two bytes into one register (103, retail's length of 504), and the case 1 order (run = 4 before the pixel) swaps the case 1 and case 2 blocks (61). A permuter run over this body (26 tries, 307 s) found nothing below the baseline.
// Earlier score 59 (was 85): The 0xc frame agrees. Alpha parity widening, literal word assembly and the EBX packed-run update still differ.
// the alpha parity is still masked in sixteen bits (and cl, 1; movzx cx, cl) where
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
// P492: Native MAKEWORD literal, dword/word literal and long/word fills,
// byte then long ordinary alpha: 103/103/105/105/108/108/110/110.
// NOT kept: literal extent recovery does not match parity/run transport;
// original helper and complete decoder restored at 59.
// MiniMax I (11-14): no new form, no lower score (59): the alpha's parity is still masked in sixteen bits at the
// three pixel sites where retail masks it in thirty-two, the colour literal is still built with a shift and an add
// where retail keeps its two bytes in separate registers, and the run of two is still folded into the count and the
// cursor as an immediate where retail adds the register.
// P505: Existing pixel helper owns encoded-input extraction, long/dword/word/byte input with ordinary then forced
// expansion: 97/96/96/96/97/96/96/96. NOT kept: All eight regress despite four-byte shortening; native parity,
// literal assembly and run-two transport are not recovered. All calibrated file and matched-caller controls stay
// unchanged. Original complete executable body restored.
// P517: Complete packed-run orders 07362514/07326154/07362541/07653214/07162534/70416235/67401235/37401625,
// moving case 7 with its real shared fill/count tail; 65/70/66/69/69/59/69/164.
// NOT kept: no exact or qualifying native-shape gain; original executable body restored.
// MiniMax D (11-15), NOT kept: 45 (was 59) added font_character_pixel_full, a twin of the existing
// font_character_pixel without the (byte) cast on the low alpha bit, used at this function's two pixel stores
// only. A twin helper picked by score is not kept; if retail differs, change the shared helper and keep its other
// callers matched. Code back to main, notes kept.
// P530: Plain switch/ordinary fill induction pairs (for/while/body increment),
// eight nonoriginal pairs: 59/59/59/59/59/59/59/59. NOT kept: no native
// parity/literal/run-register repair; original indexed fills and complete body restored.
// MiniMax E (11-16): no new form, no lower score (59): dropping the byte cast from the shared pixel helper widens the alpha's parity to thirty-two bits as retail does and makes the two type 3 pixel sites match, but the allocator then keeps the type 3 run in EBP where retail keeps it in EBX, which shifts the whole body (203); the twin helper at the two type 3 sites (45) is not kept.
// MiniMax E (11-15): no new form, no lower score (59): the alpha's parity is still masked in sixteen bits (mov cl, al; and cl, 1; movzx cx, cl) where retail widens it to thirty-two (xor ecx, ecx; mov cl, al; and ecx, 1); the twin helper without the byte cast measures 45 but a twin of a shared helper is not kept.
// P576: Long-result helper static forced/ordinary and external ordinary/forced expansion: original byte parity 59/59/59/59;
// full-width parity 203/203/203/203. Earlier expansion contracts used the word-result graph.
// NOT kept: no byte equality; original helper, decoder, alias-sensitive reloads and frame restored. Parity/literal/run-two gaps remain.
// P590: Immutable original size/code/class/literal/alpha/ordinary-pixel bindings
// and source/output parameter identities: eight 59 at 512/504. Pointer-formal
// fixed bindings also tie 59 but leave the caller unresolved; excluded. NOT kept:
// no exact native parity/literal/run-two recovery; original full body restored.
// P592: Eight-level opacity enum at the existing pixel helper, byte/numeric parity,
// long/word result and signed/unsigned color: 59/203/109/192/59/203/109/192. NOT kept: no native
// parity/literal/run-two equality; original helper, calls and complete decoder restored.
// MiniMax R (11-17): no new form, no lower score (59): the alpha's parity is still masked in sixteen bits at the three decode sites where retail widens it to thirty-two, and the two fills still merge into one block; a helper without the byte cast gives retail's parity at every site but moves the run out of EBX into EBP (203), a byte alpha in the helper narrows it further (209) and a word result for the two bytes of the type 0 value changes nothing else (103).
// P594: Separate word-store/cursor-advance boundaries at packed first/second/both, single,
// literal, single-literal, all packed and all stores: 59/59/59/59/59/59/59/59. NOT kept: no exact native
// parity/literal/run-two recovery; original scalar helper and complete decoder restored.
// MiniMax S (11-17): no new form, no lower score (59): the shared helper without the byte cast on the low alpha bit (the
// lead's form) makes all three decode sites match retail's parity (xor ecx, ecx; mov cl, al; and ecx, 1) and the pixel
// blocks are retail's, but the allocator then keeps the type 3 run in EBP where retail keeps it in EBX (and the fill's
// duplicated pixel in EBX where retail has EBP), so the body shifts (203); the base keeps retail's registers (EBX run,
// EBP duplicated pixel) with the sixteen bit parity. Measured with the cast dropped: a dword run (200), a dword run of
// the type 0/1 branch (200), a dword colour (203), a word pixel in the type 0/1 fill (200), a word fill pixel (203), a
// pointer walk fill (187), a literal count of two (203) - every one keeps the run in EBP. On the base: a byte alpha
// parameter (205), a long pixel local at a decode site (226 and 234, the parity is retail's there but the output and
// colour registers rotate), a dword pixel helper result (59), count += 2 as a literal (59), a dword run (60), and the
// colour literal as (source[1] << 8) | source[2] (103, retail's length of 504 but three instructions where retail has
// four: movzx ax, byte [edi+2]; xor ecx, ecx; mov ch, byte [edi+1]; or eax, ecx).
// Header-first cursor with a retained command pointer and common length, header-first count and literal payload split,
// and advanced-cursor rereads with tail or loop-update header count: 199/222/182/182.
// The same with full-width parity in the original helper: 203/206/214/214.
// NOT kept: the trial layouts disturb the native run and fill allocation.
// Raw header masked in pixel helper, byte/full-width parity 96/208; doubled-opacity mask
// 161/232. Long and dword inputs give the same scores. NOT kept: the 504-byte forms regress;
// full-width parity after raw masking recovers the pixel sequence but moves the run to EBP
// and merges the fills.
// The 0xc frame agrees. Parity still narrows before masking and the packed run is folded.
// Literal by whole header, type first, or combined zero fields 207/121/111. Literal after packed
// commands, before dispatch, or after the nonzero lower command 173/180/182. Shared consumed
// length from the raw header before dispatch or in the lower class 212/204.
// NOT kept: the 504-byte form changes the cursor allocation and all eight scores rise.
// Literal color constructed from twelve bits, high/low-first, pixel from both octets or from
// color plus opacity: 201/190/201/190. With full-width pixel parity: 212/232/212/232.
// The packed run and parity layout still differ.
// Joined packed count with advancing/indexed pair stores and byte/full parity: pair-first 207/203/211/213,
// single-first 205/212/209/213. NOT kept: the joined count shortens the body below retail and leaves
// the run register and literal assembly different.
// Align the upper opacity bits and parity separately 203, Align upper opacity and byte parity separately 59,
// Obtain parity by masking the aligned opacity 203, Obtain parity from the aligned unsigned opacity 203, Align a
// Boolean low opacity bit 203, Join prealigned opacity and color before parity 203, Join masked aligned parity
// and color before upper opacity 203, Compose upper opacity and parity as a word 222. NOT kept: The byte parity
// split ties. The full-width variants shorten the decoder to 444 bytes and move the run out of EBX. The word
// opacity variant scores 222 at 456 bytes. The original helper and body are restored.
// Share the case 2/3 opacity, case 5/7 color, both, or both with case 1 setting
// the run between color and opacity. Byte parity 183/183/183/184; dword parity
// 240 each. Typed helper parity, expanded alpha, both, or typed parity with
// shared cases 265/265/265/276. NOT kept: dword parity and typed alpha shorten
// the body, and the shared cases change the opening register allocation.
// Conditional opacity increment, byte parity set, or selected parity 224/216/240.
// Unsigned or signed parity remainder 240/187. Descending packed bit offset
// with byte parity 163, wide parity 181, or short offset and wide parity 226.
// Parity still narrows in the opening packed stores, and the two-byte
// literal assembly and EBX run are unresolved.
// Packed input class 59/203, packed upper and lower opacity fields with the fresh header observation
// retained 157/201, both views 157/201, both views and the ordinary six-bit count 157/201, using the
// existing byte parity and then dword parity. NOT kept: the parity width still differs on the tied form,
// and dword parity moves the run out of EBX and shortens the two fills.
// Share the opaque pixel 172, carry selected opacity 229, zero-default opacity 185,
// and an opaque predicate 222. With dword parity 224/244/215/236. NOT kept: the
// shared opaque pixel reaches 504 bytes but changes cursor allocation. The dword
// parity forms shorten both fill paths and leave the packed run and literal gaps.
// common long command count 165, common word command count 151, shared run count 172, shared run count and
// default span 135, long command count with full parity 170, word command count with full parity 162, shared run
// count with full parity 174, shared run count and default span with full parity 152. NOT kept: A separate word
// count reaches 504 bytes at 151. The other forms change the extent, and all eight raise the score.
// MiniMax H (11-19): no new form, no lower score (59): retail masks the alpha's parity in thirty-two bits at all
// three decode sites (xor ecx, ecx; mov cl, al; and ecx, 1) where this build masks it in sixteen (and cl, 1; movzx cx,
// cl), so the shared pixel helper was changed in place (it has no other caller) to drop the byte cast on the low alpha
// bit: the parity then matches retail exactly, but the allocator moves the type 3 run from EBX to EBP and the body
// shortens to 444 bytes (203); the same change with the count update shared after the branch, with the length
// hoisted above it (152 at retail's 504 bytes but a frame of 0x10 against retail's 0xc) and with the length defaulted
// at the loop head (203) were all measured, and the two arm's twin of the helper (45) is not kept because a twin of a
// shared helper is not kept. The colour literal was built as a word, as two word halves, as a shifted byte, as a byte
// cast shift or, low byte first and as two statements (103 in each, the body then has retail's 504 bytes but the
// literal is one byte shorter than retail's four instructions, so the rest of the body pairs against the wrong
// instruction). The colour literal is still built with a shift and an add where retail keeps its two bytes in
// separate registers, and the run of two is still folded into the count as an immediate where retail adds the
// register; the original helper and body stand.
// Compare with packed run 59, run to pair end 163, pair class arity 193, shared packed prefix arity 171.
// With full-width parity 203/213/191/219. NOT kept: the run comparison ties, and the other forms raise the
// score. Packed parity width, run register reuse and literal byte assembly still differ.
// full parity with counted down packed fill 236; full parity with counted down ordinary fill 236;
// full parity with both fills counted down 240; full parity with packed fill consumes output to end 187;
// full parity with ordinary fill consumes output to end 207;
// full parity with both fills consume output to end 231;
// full parity with both fills publish end after stores 226;
// full parity with packed countdown and ordinary end fill 209.
// NOT kept: Full-width parity with these fill traversals shortens the decoder and changes the packed-run allocation. The literal-byte assembly still differs.
// Forward fill countdown at packed/ordinary/both 175/175/167, with dword parity 236/236/240. Dword parity with
// packed countdown and ordinary end pointer 209; packed end pointer and ordinary countdown 187.
// NOT kept: the countdowns change the opening register assignment. Dword parity gives the packed bit
// operations but shortens the body and displaces the packed run. Literal assembly still differs.
// The parity mask width, literal byte assembly and packed run reuse still differ.
// Two-field pair loop/output-end pair loop and ordinary endpoint selection/set opaque bits
// 163/183/237/237; with dword parity 181/178/256/256.
// NOT kept: reaching retail length with the pair loops increases the score; endpoint forms shorten away from retail.
// Distinct names for the packed and ordinary run, both pixels, all four,
// and the output extents and fill pixels: 59 each. With dword parity: 203 each.
// NOT kept: dword masking restores the opacity width, but moves the packed run
// and shortens the body to 444 bytes. Literal assembly and run reuse still differ.
// Ordinary alpha in both assignment arms, transparent default, opaque default or opaque predicate:
// 59/171/172/59 with byte parity; 203/240/240/203 with dword parity.
// NOT kept: The defaults reach 504 bytes with byte parity but move the cursor registers. Dword parity
// shortens the fills and changes run allocation. The original helper and body were restored.
// Full literal in the color state, then mask after the optional store, with high-first,
// low-first, split-byte or combined assembly: byte parity 61/61/61/119, dword parity
// 207/207/207/204. NOT kept: the byte forms assemble into one register, while widening
// parity still merges the fills and moves the packed run out of EBX.
// Separate literal color from high and low bytes with a shifted high mask, an unshifted
// high mask, scaled nibble addition, or mixed color OR and literal addition: byte parity
// 118/131/119/119, full parity 204/183/204/204. NOT kept: the separate color parts
// lengthen the byte-parity decoder and the full-width variants still shorten the fill paths.
// The twelve-byte frame agrees. The packed parity mask still narrows and the pair count folds.
// Count after each conditional packed store 186, count on the store/absent paths 174, count by
// written extent or endpoint 210/210, separate packed range/exact admissions 192/185. Dword
// parity with store-path count or exact admissions 198/232. NOT kept: both dword forms shorten
// the two fills and change the packed run allocation. Literal assembly still differs.
// MiniMax H (11-20): no new form, no lower score (59): the alpha parity is still masked in sixteen bits where retail
// masks it in thirty-two (dropping the byte cast in the shared helper gives retail's parity exactly but moves the type 3
// run from EBX to EBP and shortens the body to 444 bytes, 203); the colour literal is still built with a shift and an
// add (the word, two-word-half, low-byte-first and two-statement forms all reach retail's 504 bytes but keep the
// literal two bytes short of retail's four instructions, 103); the type 3 count and the type 2 advance are still
// immediates where retail adds the run register (a loop-carried run, a split fill run, a merged count update and a goto
// out of the switch were measured at 182 to 228).
// Shared word fill taking a long or word pixel, with caller advance or a returned
// end pointer, 59 each. With dword parity in the pixel producer, 203 each.
// NOT kept: the parity widens but moves the packed run to EBP and shortens
// the body to 444 bytes. The literal-byte assembly still differs.
// Next input per arm or defaulted 190/219. Next byte count per arm or defaulted 116/223.
// With full-width parity 207/204/212/208. NOT kept: the wider parity still shortens the fills
// and the new cursor forms leave the literal and packed-run differences.
// Nonempty packed, ordinary or both fill guards and the ordinary inner guard:
// byte parity 138/111/144/59, dword parity 200/200/192/199.
// NOT kept: dword parity shortens the fills and changes run allocation. The byte-parity tie retains the narrow packed mask.
// The parity width, literal byte assembly and packed run register still differ. Word byte swap after a sized
// copy or an unaligned read scored 59 with byte parity and 207 with dword parity. Network-order conversion
// scored 119 and 204 for either load. NOT kept because the byte swap ties and the network-order form adds a call.
// Packed opacity still narrows before masking. Pixel helper with inferred opacity width, using the
// original producers, byte high, byte low or both byte-sized: 265/265/269/269 with either parity mask.
// NOT kept: inferred widths shorten the decoder to 404–408 bytes and raise the score.
// Mask the upper opacity or whole payload before shifting 59/59; share the decoded payload
// with the ordinary count by masks or by quotient and remainder 201/201. With dword parity
// the same four forms score 203/203/168/168. NOT kept: the shared dword forms reach 504
// bytes but change the output, color and run registers. The byte-parity forms tie or regress.
// The packed parity width, literal assembly and packed-run register still differ.
// Dword parity with ordinary counted/endpoint standard fills 183/180; packed counted/endpoint
// standard fills 180/184; both fills counted/count-endpoint/endpoint-count/both endpoint
// 241/201/199/218. NOT kept: the fill changes displace the packed run and raise the score.
// The 0xc frame agrees. Dword parity, literal bytes and the EBX packed run still differ.
// Shifted or divided command enum, masked opcode enum, or opcode minus its payload:
// 59/59/201/208 with byte parity and 203/203/203/173 with dword parity.
// NOT kept: the shifted/divided types tie. The other forms change fill allocation and raise the score.
// Encoded bitfield view for packed fields, opcode and packed fields, opcode and run, or all fields
// 59 each with byte parity and 203 each with dword parity. NOT kept: the widened mask matches
// the first parity sequence but shortens the fills and moves the packed run. Literal assembly still differs.
// M3 lane GB r2 (33 compiled trials, fast control 59 at 512 B with 0x140960 and 11 file mates MATCH throughout; standalone --fast 0x122610 also 59; body restored):
// Literal forms: word raw = *(word const *)(source+1) as (raw>>8)|((raw&0xff)<<8) 115 (emits xor edx,edx; mov dh,al; shr eax,8; or eax,edx), (word)((raw>>8)|(raw<<8)) 125, (raw>>8)|((dword)(byte)raw<<8) 120; word+mul256 forms (word)source[2]+(word)((word)source[1]<<8) 63 and (word)source[2]|(word)(source[1]*256) 63 (both emit 16-bit movzx ax/dx), (word)source[2]|((dword)source[1]*256) 59 (or for add, same 512 B); union lo/hi 132 (frame 0x10); word-cast/short/two-step/inline-be16/ptr-pair <<8| forms all 103 at 504 B (aligned 27, masked 26: fuse into xor eax,eax; mov ah; mov al).
// Alpha parity & fill separation: ((dword)(byte)alpha & 1) 59; dropping (byte) gives retail's 32-bit parity at sites 1-3 and add esi,reg at site 3, but moves packed run to EBP and tail-merges both fills to 444-456 B across base 203, (alpha&0xff)&1 203, explicit run=2 in cases 3/7 199, if(type==2) first 212, switch(type) 204, *output++ fill 236, case 1 order+bswap 204, (word) helper return 192, (word) shifted alpha 200 (also matches site 4's xor ecx,ecx..shl ecx,0xc), (word) 4-bit alpha 204, and all-3 long pixel locals 226; (alpha+alpha) 204; case 1 order alone 61; bswap+type==3 119.
// Parity from the even part (shift pair or mask), removed by subtraction or XOR.
// Long opacity 192/240/192/240, byte opacity 192/183/192/183.
// NOT kept: all raise the score. Subtraction also changes the pixel-copy caller.
// Lane GA r3: word-returning font_character_pixel without (byte) cast + word pixel at ordinary fill + run scoped to type==2 (count += 2 in type!=2, goto-free switch with if (run > 0 && output), case 1 run before pixel) + length = 1 per arm -> 12 at retail's 504 B (machine 3 review: length = 1 back after the type test, as before, gives 41 at 508 B; the per-arm form is not kept) (matches all 4 pixel sites, EBX/EBP fill registers, switch, jump table and exits byte-for-byte; only the 4-instruction literal at 0x122744 differs). Tried on score-12 base: switch(type) 13; word color 51; run=0 init 176; run!=0 41; word switch pixel 41; split count+=2 155; direct/masked/word byte-swap macros 76-125; bitfield/union/MAKEWORD/shift-or/inline-helper/nibble-split literal forms 90-97 (all fuse to 8-byte mov ah/al, even with count++ between 76, or under /G6 /G7 /Ox /Ob1 90); word-bitfield read p->lo|(p->hi<<8) 89 (emits movzx ax, [edi+2]; mov ah, [edi+1]); color-first then &=0xfff 37; no value local 180; (word)source[2] | (word)(source[1]*256) 30; (dword)source[2] | ((dword)source[1]*256) 12; ((word)source[2] & 0xfff) | ((dword)source[1] << 8) 81 (breaks fusion -> movzx ax, [edi+2] + xor/mov dh, [edi+1] + and 0xfff); diagnostic (word)source[2] ^ ((dword)source[1] << 8) 4 at 504/504 B (breaks OR fusion -> movzx dx, [edi+2]; xor eax, eax; mov ah, [edi+1]; xor eax, edx; not kept since retail uses or).
// The shared packed length changes the count join, and the literal still assembles in different registers.
// Word halves with wide OR, wide high half with word result, separate color and literal consumers,
// or color after the optional store: 95/95/106/192. With per-arm length: 90/90/91/183.
// NOT kept: the word forms reach retail length only with the shared length, and all forms raise the score.
// The packed count join and separate literal loads still differ. Signed high-byte
// add/OR, narrowed at the complete pixel or at the high contribution, each 45.
// Each with per-arm length reaches 30. NOT kept: the per-arm forms still differ,
// and the signed conversion leaves a 508-byte body with different literal loads.
// Extending the literal from its decoded header by shift/OR, byte places, base-256
// addition, or a word accumulator: 103/103/103/95 with the shared length,
// 84/84/84/90 with per-arm lengths. NOT kept: the word form reaches 504 bytes
// with the shared length but still fuses the literal byte loads, and the packed count join differs.
// M3 lane GA r5 (93 fast trials on score-41 and score-12 bases with 0x140960/0x122540/0x122570 MATCH; body restored):
// Clean length=1 on score-41 base: long length = 1 at loop top 182 (516 B); if/else-if/else chain with length=1 at top 119 (512 B).
// Literal hunt on score-12 base (504 B): 3-part nibble split ((dword)(source[1]>>4)<<12)|((dword)(source[1]&0xf)<<8)|(word)source[2]
// and 4-nibble ARGB4444 in dword both break OR-fusion to 11 at 504/504 B (movzx ecx,[edi+1]; movzx eax,[edi+2]; shl ecx,8; or eax,ecx);
// source[2] nibble split in dword (((dword)(source[2]>>4)<<4)|(dword)(source[2]&0xf))|((dword)source[1]<<8) (either operand order) breaks
// OR-fusion with zero extra instructions -> movzx eax,[edi+2]; xor edx,edx; mov dh,[edi+1]; or eax,edx (30 at 504 B, 1 B shorter from 32-bit
// movzx eax vs retail's 16-bit movzx ax, shifting trailing pad/jump table; casting to word inhibits the nibble collapse -> 102-106 at 520-524 B).
// Other forms on score-12 base: word/short/dword/long/maskff00/shl24>>16/two-step/shift-assign/s_pix_bf_only/maskf0|0f OR forms all fuse to
// 496 B (90); & 0xffff / & 0xfff / dword union bitfield write break fusion but keep `and` -> 82 (508 B); word union bitfield write spills to
// stack -> 33 (508 B); byte union -> 92 (512 B); word ptr bswap -> 82 (504 B); word+mul256 / word+word<<8 -> 30 (508 B); color-first alpha|color -> 182 (512 B).
// The 0xc frame agrees. The packed length/count join and literal low-word/shifted-high OR still differ.
// Common nonliteral count with zero header, nested tests or dispatch switch, branch or selected span 188/187/188/179;
// with zero type and payload 176/181/183/193. NOT kept: an early literal branch changes the prologue and the caller.
// The pair count load still precedes the last pixel store. Descending bit offsets,
// upper boundaries, post-tested offsets and pre-stepped boundaries score
// 175/192/175/196 with shared length and 175/192/175/196 with per-arm length.
// NOT kept: the offset loops change the pair layout. The boundary walks also
// change the matched pixel-copy caller.
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
			if (type != 2)
			{
				if (output)
				{
					*output++ = font_character_pixel((code >> 3) & 7, color);
					*output++ = font_character_pixel(*source & 7, color);
				}
				count += 2;
			}
			else
			{
				long run;
				long pixel;

				if (output)
					*output++ = font_character_pixel((code >> 3) & 7, color);
				count++;
				switch (*source & 7)
				{
				case 0:
					run = 0;
					break;
				case 4:
					run = 5;
					pixel = color;
					break;
				case 1:
					run = 4;
					pixel = color | 0xf000;
					break;
				case 6:
					run = 3;
					pixel = color;
					break;
				case 2:
					run = 3;
					pixel = color | 0xf000;
					break;
				case 3:
					run = 2;
					pixel = color | 0xf000;
					break;
				case 5:
					run = 4;
					pixel = color;
					break;
				case 7:
					run = 2;
					pixel = color;
					break;
				default:
					__assume(0);
				}
				if (run > 0 && output)
				{
					for (long i = 0; i < run; i++)
						output[i] = (word)pixel;
					output += run;
				}
				count += run;
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
					word pixel = font_character_pixel(alpha, color);

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
