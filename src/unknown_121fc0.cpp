// @flags /O2 /arch:SSE /Gr
#include "font_loading_storage.h"
#include "unknown_11c920.h"
#include "font_loading.h"
#include "async.h"
#include "main_globals.h"
#include "unknown_058ee0.h"
#include <string.h>

extern bool g_4e3b40;
extern bool g_51ea00;
extern bool g_4ed39e;
extern bool g_4ed39d;
extern dword g_4ed3a0;
extern char const *g_4687f0;
bool function_121b00(void);
char *font_table_get_name(char *arg_0, long arg_1);
char *function_122810(char *arg_0, char const *arg_1);
void function_1223a0(long arg_0, char const *arg_1, bool arg_2);
void font_cache_initialize(void);
void font_cache_pixels_initialize(void);
void function_593e0(void);

/* the next token of a string separated by any of the delimiters (NULL when
   there is none); the token is terminated in place and *next set to the text
   after it */
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

// score 128 (was 128): frame 0xa44 agrees; EBP is saved at token traversal rather than the
// outer block, and read-size/count/cursor homes differ. Round 28: the scalar homes sit 4 below retail's (the token
// array agrees); EBP is saved and restored around the token loop only. Clamp as a MIN macro / an if / a PIN macro
// 128/128/128 (same code); 0x1a1480's size and success parameters swapped everywhere (retail's call setup order,
// ebx/eax/ecx, and 0x1a1480 still matches) 128; on it the read size declared at the top of the block / of the
// function / with both flags at the block top 128/128/128. Unsigned token / unique / both counts
// outer block, and read-size/count/cursor homes differ. Round 8: in-loop homes are retail
// cursor/size/next/count/unique at 0x14/0x18/0x1c/0x20/0x24, ours cursor/next/count/size/unique;
// retail reads the size three times around its clamp (a memory-resident size: a volatile size,
// diagnostic only, gives the clamp, 102, with EBP still saved late); behavioural local names
// (size/next/count/unique/token) 128, names in either sort order 128; the token loop after the
// success test with a NULL first token 127; the walk as a for loop with the first token as init 128. Unsigned token / unique / both counts
// 128/128/128; signed-word token / unique / both 232/144/231; returned copy destination 126,
// with unsigned counts 126; returned name 199, both returned paths 124 (one-use result, restored).
// Tried: clamp, terminator and first token before the local_7 test, 118 (not kept: retail
// tests local_7 first at 0x1220ef and only then clamps and tokenizes; this ran them on a failed read).
// Earlier: retail saves ebp together with esi and edi at the start of the
// !g_4e3b40 block and pops ebp after the g_4e3b40 store; ours saves ebp inside
// the token loop, so every stack offset in the block differs by four. The size
// clamp also differs: retail stores the clamped size and reloads it. Tried:
// local_12 declared before the if and the token array after the counters, 126
// (not kept: gain under 8).
// Round 292: Native duplicate search: pointer cursor/ordinal result 205/216; ordinal cursor/pointer
// result 150/187; pointer cursor/null result 217/221; pointer cursor/end result 208/218 (duplicate/new
// first); original search restored.
// (not kept: gain under 8). Round 38: the loop outside success 127; early token 147 (EBP moves out,
// with extra address setup); token as read destination 144; separate read size with it 164; early cursor
// 127; early token with read size 155; loop with read size 216; role-named size 147; early token and
// cursor 172; nested early token 128; reversed read outputs 128; early token with reversed outputs 147.
// Original body restored.
// Mapped cache-index search result, long/unsigned-long/word/signed-byte 133/133/140/137;
// new-result arm first 188/188/185/188. All restored.
// Round 231: existing tokenizer null guard / empty guard / nested result / result-first
// 217/206/213/221; each with current table-entry token observations 217/206/213/221.
// Original tokenizer and body restored; outer EBP preservation and read-size homes remain.
// Round 28 (lane 10): the read size, cursor and count homes are also rotated (retail, low to high:
// size, cursor, count; ours cursor, count, size). Token declared at the block top 128; tokenizer as a
// for loop 128; token = NULL at the block top 128; size/cursor renamed (size, token_end) 128 (same
// code); clamp in MIN order 128. EBP's save stays at the loop in every form.
// Round 231: existing tokenizer null guard / empty guard / nested result / result-first
// 217/206/213/221; each with current table-entry token observations 217/206/213/221.
// Original tokenizer and body restored; outer EBP preservation and read-size homes remain.
// Round 53: the next/token pointers declared before the success test 128; the found index declared
// outside the count test 128; the clamp as `if (size >= sizeof) size = sizeof - 1` 128; `while (token != NULL)`
// 128; the token array cleared by memset 256 (724 bytes, rep stosd). Moving the token loop out of the
// success block (counters set inside it) drops EBP altogether (724 bytes, harness).
// Round 40 (lane 11, fast): our EBP push sits at the first token's definition and its pop at the loop exit,
// so the failed-read exit and the no-token exit stay apart (retail joins both before `pop edi` and restores
// EBP with ESI/EDI at the block end). The tokenizer as __forceinline 128; the next pointer / the token / the
// count and unique counters / the read size declared at the block top 128/128/128/128 (all the same code).
// Round 251: Delayed token-table publication / predecessor-first equality / both / exhausted ordinal / delayed-
// ordinal / equality-ordinal / all / all with returned path 215/130/217/132/236/134/236/234. Original body
// restored; early EBP and read-size/count/cursor homes remain.
// Round 188 (fast): the code inside the token loop is retail's except that every scalar home sits
// 4 below (EBP pushed just before the loop, popped at its exit; retail pushes it with ESI/EDI at
// the block start and joins the failed-read and no-token exits before the pops). A single goto from
// the success test to the block's exit 128 (same code); the clamp and terminator before the token
// array and counters 129; before the counters only 129; before the token array only 129; clamp,
// terminator and first token before all three 177.
// Round 64 (lane 12; one-link LTCG env with the stand-in caller, equal to the full listing; env positional/
// aligned 120/51 for this body): the clamped size in a new local used as the index 202/53 (763 bytes); the clamp
// before the token table's initialisation 121/59; both 203/58; the clamp written inside the subscript 202/53;
// the done flag set at the block top (diagnostic) 185/68; the next pointer initialised to NULL and the
// terminator stored through pointer arithmetic: identical code.
// Round 36 (lane 13, fast): retail reads the size three times around its clamp (an unconditional select and
// store, then a reload for the terminator), the reads of a size that is never held in a register; that extra
// weight would also put its home under next/count as retail has it. Every non-volatile form here keeps it in
// EAX. The clamped size in a separate `length` for the terminator 216 (763 bytes); the clamp and terminator
// before the token array and counters 129; the done flag set right after the wait 154 (766 bytes); the
// counters declared and zeroed at the block top 127 (zeroed before the read call); with the token array too
// 221 (756 bytes); NULL first token with the loop after the success block 127 (EBP still pushed at the loop).
// Round 33 (lane 14, fast): the token loop after the success test with a NULL first token at block scope 127
// (the NULL path is threaded to the end, EBP still saved at the loop guard); the success test as one goto to
// the block's exit 128; next declared at the block top 128 (same code); counters declared before the token
// array 126 (retail's zero-store order: counters, first, last element); with 0x1a1480's size and success
// parameters swapped (retail's ebx/eax/ecx setup) 126. Diagnostic only (reverted at once): the three read
// outputs volatile 102, size then takes retail's lowest home and three-read clamp, so the clamp and the size
// home need a volatile size, a form the rules forbid; EBP's late save is independent of it.
// Round 60 (lane 16, fast): retail treats the read size as an unpromoted memory variable (read, select
// `mov eax, 0x7ff; ja`, reload in the else arm, store, reload for the index); ours promotes it after the
// wait (the compile-only build does too, so it is not a callee summary). `if (!success) goto` the block end
// 128; the size as `long` passed as `(dword *)&size` 128; the read and wait in an inline blocking-read
// wrapper taking the size pointer 128; the clamp and terminator before the token-array/counter
// initialisation 129; the size zero-initialised (`long size = 0` + cast, as file_read_string 0x121a40 does)
// 122 (6-point gain: the top zero becomes `xor ebx, ebx`, aligned count 51 -> 52; not kept); `dword size = 0` 122.
// Round 49 (lane 11, fast): retail's clamp reads like `if (size > 0x7ff) size = 0x7ff;` on a size that is never
// enregistered (load for the compare, select reloading it, store, reload for the index), the way 0x121a40 keeps its
// out-count in memory; ours caches it in EAX. The zero-initialised `long` size form above with the clamp as an if
// 122 (same score: the clamp spelling changes nothing; the zero store before the read call is not retail's); the
// three read outputs declared at the block top 128; declared size/success/done 128 (the reversed outputs again);
// the tokenizer `inline` without `static` 128; the path copy through a file-local csstrncpy (as 0x1a08d0's file has
// it) 128; the new-index arm first (`== NONE`) 188 (764 bytes); the token stored after the duplicate search 215 (756
// bytes). Diagnostic only, not keepable: the done flag `bool volatile` (the wait's parameter type) 128. The
// compile-only screen does not reproduce this body (the wait helper is not inlined across files there).
// Round 31 (m3 lane 2, fast): the token walk with one tokenizer call in the while condition (next
// starting at the buffer) 287 (690 bytes); the done flag declared `bool volatile` as file_read_string
// 0x121a40 does 128 (same code); that with the size zero-initialised as there 122 (the earlier 6-point
// form); the walk as a do-while under a first-token test 128 (same code); the new-font arm first
// (`local_14 == NONE`) 188 (764 bytes). EBP is still saved at the first token's test.
// Round 64 (lane 9, fast; a compile-only build reproduces the late EBP save and the slot order): the token loop
// as `if (token) do { ... } while ((token = next token) != NULL);` 128; `if (!local_7) {} else { ... }` 128;
// the first token taken in the success block with the loop under `if (token)` at block level 130; a block-
// level `while (local_7 && token)` with a NULL token 215 (781 bytes); `dword size = 0` with the done flag
// `bool volatile` as 0x121a40 declares it 122 (as the zero alone), with the counters declared before the token
// array 122. Diagnostic: the token stored to a global after the loop 149. Compile-only: a use of the token
// after the success block moves `push ebp` to the block's start (retail's place); renaming the size, next,
// count and unique locals (four orders, prefixes included) never changes their slot order.
// Round 20 (lane 17, fast): round 60's `long size = 0` passed as `(dword *)&size`, re-measured with round
// 33's counters declared before the token array, 122 (the gain is the zero store, which retail does not
// have); the counters before the token array alone re-measured 126; diagnostic, the token loop hand-inverted (`if (token) do ... while (token)`) 128
// (same code; reverted); the found index declared at the token loop's top 128; one tokenizer call in the
// loop condition 287 (690 bytes: retail inlines the tokenizer twice); the search index declared before
// its for loop 128; the found test's arms swapped (`== NONE` first) 188 (764 bytes).
// @retail 0x121fc0
void function_121fc0(void)
{
	for (long local_0 = 0; local_0 < 11; local_0++)
		g_4e28f4[local_0] = -2;
	if (!g_4e3b40)
	{
		memset(g_4e2920, 0, k_maximum_font_count * sizeof(s_font_cache_entry));
		if (!function_121b00())
		{
			g_51ea00 = true;
			main_globals.unknown29 = true;
			g_4ed39e = true;
			g_4ed39d = true;
			g_4ed3a0 = GetTickCount();
			if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
				function_593e0();
		}
		char local_3[256];
		char local_4[256];
		char local_5[0x800];
		local_3[0] = 0;
		font_table_get_name(local_4, sizeof(local_4));
		strncpy(local_3, g_4687f0, sizeof(local_3));
		local_3[255] = 0;
		function_122810(local_3, local_4);
		bool local_6;
		bool local_7;
		dword local_8;
		function_1a1480(local_3, local_5, sizeof(local_5) - 1, 7, 6, &local_7, &local_8, &local_6);
		function_120d50(&local_6, false);
		if (local_7)
		{
			char *local_9[11] = {0};
			long local_10 = 0;
			long local_11 = 0;
			local_8 = local_8 > sizeof(local_5) - 1 ? sizeof(local_5) - 1 : local_8;
			local_5[local_8] = 0;
			char *local_12;
			char *local_13 = function_xe9ecc4(local_5, "\t\n\r ", &local_12);
			while (local_13)
			{
				if (local_10 < 11)
				{
					long local_14 = NONE;
					local_9[local_10] = local_13;
					for (long local_15 = 0; local_15 < local_10; local_15++)
					{
						if (!strcmp(local_13, local_9[local_15]))
						{
							local_14 = local_15;
							break;
						}
					}
					if (local_14 != NONE)
						g_4e28f4[local_10] = g_4e28f4[local_14];
					else
					{
						g_4e28f4[local_10] = local_11;
						function_1223a0(local_11, local_13, true);
						local_11++;
					}
					local_10++;
				}
				local_13 = function_xe9ecc4(local_12, "\t\n\r ", &local_12);
			}
		}
		g_4e3b40 = true;
	}
	font_cache_initialize();
	font_cache_pixels_initialize();
}
