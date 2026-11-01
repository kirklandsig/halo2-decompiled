#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0805E0.CPP: the cache of the configurations of the
   players met recently (entries of 0x68 bytes at 0x4cf98c, chained through
   their next index). Most of the file is outside lane D's region
   (0x7fb50..0x80be7); lane D has the recent-player iterator. */

#include "unknown_11c920.h"
#include "unknown_2312b4.h"
#include <string.h>
#include <xtl.h>

struct s_player_configuration_cache_entry
{
	s_recent_player player;
	short unknown58;
	short next;
	short previous_other;
	short next_other;
	dword flags;
	union
	{
		byte unknown64[4];
		dword field_64;
	};
};



/* whether a XUID is an offline (machine-local) one */
static inline bool xuid_is_offline(unsigned __int64 xuid)
{
	return (xuid >> 48) == 0xfefe;
}

/* the entry at the iterator, moving the iterator to the next one */
static inline s_player_configuration_cache_entry *player_configuration_cache_iterate(long *iterator)
{
	s_player_configuration_cache_entry *entry = NULL;
	if (*iterator != NONE)
	{
		entry = &g_4cf98c[*iterator];
		*iterator = entry->next;
	}
	return entry;
}

static inline bool player_configuration_is_excluded(s_recent_player const *player)
{
	return (player->unknown08[0] & 3) != 0;
}

/* the next recent player after the iterator's: a valid, online, known player
   with a name; iterator starts at the first entry and ends at NONE */
// score 43 (was 43): early ESI save, EBX successor, mask hoist and loop exit differ;
// retail lets the entry pointer reuse the index register (imul esi, esi) and keeps the
// successor apart in EBX (back-edge mov esi, ebx), which leaves AL free for the & 3 mask;
// ours shares one register for index and successor.
// native successor 51; persistent entry while/for 57; complete admission 43; identity predicate 53;
// native player assignment 43; native successor with identity 57; admission result 59.
// Native identity OR/logical OR 83/78; each with native name 83/78; native name alone 43;
// shared player receiver 43, with native OR/name 83, with native logical OR/name 78.
// existing iterator while 79; iterator for 56; shared identity 50; shared admission flags 43; combined mask 50;
// direct exclusion bits 43; current-index name 43; rejected entries continue 43.
// iterator-owned walk 71; iterator success exit 73; native/byte results 57/61;
// result-terminated loop 52; iterator result termination 44; output outcomes 68; native termination 47.
// Round 85: successor local with early returns 49; every field read through the table index
// (no entry pointer) 71; iterator store re-read as the index 51; loop over *iterator 71;
// successor read through the table after the entry 60 (EBX successor as retail, 137 bytes);
// short index 64 (entry reuses the index register as retail, 138 bytes).
// Round 7 (lane 15): retail hoists the `& 3` mask into AL (`mov al, 3` before the loop), which is what
// leaves the successor to EBX; the compiler hoists it only when the loop holds two uses of the constant.
// Early returns with a successor local 49, with `index = *iterator` 49, as a for loop 49, index reassigned
// from the successor 52, guarded do-while 49; harness: mask on the dword, inline, as two bit tests or
// through a char cast, `continue` filters, short index/successor mixes - the mask stays an immediate.
// The mask is the XDK guest mask (`XOnlineIsUserGuest`, as this function's caller tests it).
// Round 119: retail's result is not live in the loop (false is made again at the loop exit), so AL is
// free for the mask. Plain early returns (no successor local) 52 (no EBX at all: next in ECX, flags in
// EAX, mask an immediate); the caller's XUID view with XOnlineIsUserGuest 43, with early returns 52;
// the filter as an inline predicate with early returns 40 (`setne cl` for the last test), with the
// xuid tests split 40, with every test an early return 43, the predicate with early returns in the
// loop 54.
// Round 34: the hoisted mask in AL is the one cause: it makes the result constant at each exit (two
// false exits), the successor take EBX and the early ESI save; with early returns everything else
// is retail's. A compile-only screen confirms VC7.1 hoists the 3 only with two uses in the loop and
// retail keeps one, so the second use was removed after hoisting. Tried: the zero-XUID test as a
// null-identity comparison (`!(xuid == 0 && guest number == 0)`) 67 (hoists, but the extra guest
// test stays, 140 bytes); guarded do-while with a named successor and early returns 49; successor
// local with early returns 49; the XDK `XOnlineIsUserGuest` on the flags dword 43, with the
// do-while 49; harness only: the guest test twice (merged before hoisting), dword and byte forms
// together (both stay), a use of 3 outside the loop (no hoist).
// Round 311: trailing name/exclusion/online/identity tests produce the actual result
// 42/42/42/54; native full-width result 57/52/54/56. No match; original body restored.
// Round 106: one inline online-XUID test (set, not a guest, not machine-local) for the three xuid tests 47; with
// the machine-local test before the guest test 43; either with the exclusion test kept too 67/71 (140 bytes: the
// mask is then hoisted into AL as retail, but both guest tests stay); the existing iterate helper with that
// test 79/79 (102 bytes).
// Round 443: Two-use admission flags in byte/char/word/short storage 44/44/43/43;
// direct-success-return counterparts 52/52/51/51. All restored; early ESI, independent
// successor, mask hoist and failure exit remain.
// Round 45 (lane 16), `--fast`: struct assignment for the player copy 43; force-inline offline and
// exclusion helpers 43; the 64-bit id in a two-use local 50 (its load moves above the flag tests);
// the id test as an OR of its two dwords 83 (98 bytes); the result declared after the index 43.
// Compile-only screens (one use of the mask never hoists it): a do-while with a named successor, a
// `continue` filter, an admitted flag, the XUID view, and the null-identity test before or after the
// guest test (both hoist the 3 into BL but keep both guest tests).
// Round 111 (LTCG harness equal to the full listing): the one-use mask is hoisted into AL, with the result
// rematerialized at the exit as retail, when the guest test comes first in the chain, with `continue` filters in
// the while loop, or with `while (!result && index != NONE)`; those loops stay top-tested with index and successor
// sharing one register. A successor local with that loop condition gives retail's EBX successor and ECX/EDX
// scratch but no hoist, and moves the player argument to the stack. `--fast`: `continue` filters 57; the `!result`
// loop 57; it with a successor local 37 (0x1a303b and 0x2b393c then lose their matches: player passed on the
// stack, ret 8); the successor assigned in an else 57; a guarded do-while with a successor and early returns 49.
// Harness only: for loops with `index = next` (no hoist), an inline entry getter (identical), short index or
// successor, the found copy after the loop, result set before the copy.
// Round 98 (lane 7), `--fast`: `*iterator = index = entry->next` with an early return 52; the successor stored after
// the test on both paths 51; the `!result` loop with a successor local and `break` 57; a for loop with `continue`
// filters and `break` 51; one index with `continue` filters and an early return 55. The successor read through the
// table (`long next = g_4cf98c[index].next;` before the entry pointer, or `*iterator = g_4cf98c[index].next;` with the
// index read back) in the `!result` loop with `break` or with `else index = next` gives retail's AL mask and EBX
// successor, 36 at 127 bytes (not kept: an always-true loop test or a mixed read, no match); left are the top-tested
// loop (retail: guard, then push ebx and the mask, latch `cmp ebx, -1; je; mov esi, ebx`) and the entry computed after
// the successor load. Harness: the rotated loop with that successor and a `break` hoists the mask only with the guest
// test first (so it depends on the test block's frequency); test spellings, result and successor types change nothing.
// Harness: an `if (index != NONE)` guard or a do-while around the `!result` loop, the existing iterate helper on a local
// index, booleans computed before the test and (in the rotated loop) the copy after the loop all lose the hoist.
// Round 28 (lane 14), `--fast`: a for loop stepping the index from a successor local with `continue` filters and an
// early return 49 (aligned 27: index and successor share EAX, mask an immediate), with the && chain and an early
// return 49; the while loop with the chain inverted into a `continue` and an early return 52, with the found block
// returning directly 52; the successor for loop with `continue` filters, the result and `break` 51. The XDK's own
// XOnlineAreUsersIdentical is the only header macro with two uses of the guest mask (its null-identity form is the
// round 34 trial). Callers 0x1a303b/0x2b393c kept their matches in every preview.
// Round 99 (lane 8), one-file LTCG harness exact against the build: the mask is hoisted in every `!result` loop
// whatever the guest test's position, in `break` loops only with the guest test first, never with early returns.
// Real: `while (!result && index != NONE)` with a successor local and `index = next` after the test 37, also as a for
// loop with that increment 37 (both move the player argument to the stack, ret 8, and lose 0x1a303b and 0x2b393c);
// the found entry copied after the loop 43; a successor local with `break` 51; a guarded do-while with a successor 51.
// Harness: early returns with a successor 49, `while (true)` with a successor test 52/57, the loop ended by
// `index = NONE` in the found arm 43 (no hoist), every field through the table 69-71.
// Round 28 (m3 lane 2, fast with 0x1a303b/0x2b393c): `while (!result && *iterator != NONE)` indexing through
// the iterator 79 (102 bytes: the mask is hoisted into AL, but the loop reloads the iterator, which moves to
// EBX with `ret`, both callers lose); the `!result` loop with `index = *iterator` re-read at the end 42 (iterator
// in EBX, player on the stack, callers lose); `!result` loop with a successor local, result declared after the
// index 37 (the listed form: player on the stack, ret 8, `!result` not threaded, mask an immediate); the same
// with `index != NONE && !result` 53; a for loop stepping `index = *iterator` with `break` 51 (convention kept,
// index and successor share ECX, mask an immediate).
// Round 59 (fast; 0x1a303b/0x2b393c stayed matched): 1-8 unused locals at the top (diagnostic) 43 each;
// result set before the copy 43; the entry pointer declared at function scope 43; `*iterator = index =
// entry->next` without early returns 43; `g_4cf98c + index` 43; a two-use dword flags local 43 (all same code).
// Round 17 (lane 17, fast): the guest mask as a byte parameter of the exclusion helper 43 (propagated, still an
// immediate); `while (!result && index != NONE)` with a successor local and `else index = next` 57 (AL mask, but
// the loop stays top-tested in ECX); the record's four tests as one inline `&&` predicate 43 (identical), with
// early returns 40 (round 119's form, re-measured), that plus a successor stepped at the loop's end 48; a guarded
// do-while with a successor, `else index = next` and the `!result` test 51 (no hoist).
// Round 44 (lane 13, fast): `for (long index = *iterator; index != NONE; index = *iterator)` with
// `*iterator = entry->next;` in the body 51 (index and successor still share one register); `while (!result &&
// index != NONE)` without break, stepping by `index = *iterator` 42 (AL mask hoisted, but the iterator takes a
// register and the player goes to the stack: 0x1a303b and 0x2b393c stop matching), stepping by `index =
// entry->next` 49 (player in EBX, result kept in AL and tested each pass; callers lose their matches), that with
// `*player = entry->player` 49; no index local, `*iterator` read at each use 71 (109 B). All restored.
// Round 127 (lane 6, fast, with 0x1a303b/0x2b393c): `while (!result && index != NONE)` with a successor local and
// `index = next` at the end 37 again (no hoist in this build; player read from the stack, ret 8, callers 15/27);
// its test as `index != NONE && !result` 53; the test chain assigned to result, copy under `if (result && player)`
// 53 (callers 2/75); a const entry pointer 43 (identical); a for loop stepping `index = entry->next` with the
// entry declared outside 56 (callers 17/74). AL holds the result through our loop, so the mask has no free register.
// Round 73 (fast, with 0x1a303b/0x2b393c, both still matched): round 34's null-identity form
// `!(xuid == 0 && (xuid_flags & 3) == 0)` re-measured 67 as the base, then: on the byte flags 67, with the guest test on the dword too 67, as
// `xuid != 0 || (xuid_flags & 3) != 0` 67, with `(xuid_flags & 3) == 0` for the guest test 67, with the guest test
// moved ahead of the machine-local test 66 (all 140 bytes: the mask is hoisted into AL and false made again at the
// exits as in retail, but the inner guest test stays even right next to the outer one; VC does not thread it).
// Round 37 (lane 15, fast): the result assigned from the test chain on every pass (`result = tests;
// if (result) { copy; break; }`) hoists the mask exactly as retail (`push ebx; mov al, 3` after the guard,
// false remade at the loop exit) 59, but index and successor still share one register (entry copied into
// ESI at the loop top) and the loop head gets a jump over 16-byte padding (135 bytes); on it a successor
// local 59, the successor read through the table first 62 (successor EBX, index ECX), `index = *iterator`
// at the end 59, a for loop stepping `index = *iterator` 59 or from a successor 59, the entry pointer at
// function scope 59, every field read through the table 75, the loop over `*iterator` 70 (iterator then
// passed in a register, callers lose). `while (index != NONE && !result)` with a successor 53; `continue`
// filters with an early return 55; the `!result` loop assigning the result each pass with the copy after
// the loop 71 (top-tested, no hoist); the guest test 3rd/4th (diagnostic) 48/47, no hoist; the parameters
// swapped to (iterator, player) with both callers 43 (identical: player stays in EAX).
// Round 127 (lane 6, fast, with 0x1a303b/0x2b393c): `while (!result && index != NONE)` with a successor local and
// `index = next` at the end 37 again (no hoist in this build; player read from the stack, ret 8, callers 15/27);
// its test as `index != NONE && !result` 53; the test chain assigned to result, copy under `if (result && player)`
// 53 (callers 2/75); a const entry pointer 43 (identical); a for loop stepping `index = entry->next` with the
// entry declared outside 56 (callers 17/74). AL holds the result through our loop, so the mask has no free register.
// @retail 0x805e0
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator)
{
	bool result = false;
	long index = *iterator;
	while (index != NONE)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		index = entry->next;
		*iterator = index;
		if (!(entry->flags & 4) && (entry->flags & 0x10) &&
			*(unsigned __int64 *)entry->player.xuid != 0 &&
			!xuid_is_offline(*(unsigned __int64 *)entry->player.xuid) &&
			!player_configuration_is_excluded(&entry->player) &&
			*(word *)&entry->player.unknown08[4] != 0)
		{
			if (player)
				memcpy(player, &entry->player, sizeof(s_recent_player));
			result = true;
			break;
		}
	}
	return result;
}







long __stdcall function_7fc80(const void *identity, long *position);

struct s_cache_property
{
	long unknown00;
	long type;
	long unknown08;
	long unknown0c;
};

struct s_cache_property_record
{
	byte identity[12];
	long type;
	long count;
	s_cache_property *properties;
};

// @retail 0x80940
void function_80940(long record_capacity, s_cache_property_record *records,
	long property_capacity, s_cache_property *properties, long buffer_capacity,
	byte *buffers, long *record_count, long *property_count, long *buffer_count)
{
	long record_index = 0;
	long buffer_index = 0;
	long property_index = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (entry->flags & 2)
		{
			if (record_capacity <= 0 || property_capacity < 4 || buffer_capacity <= 0)
				break;
			memcpy(records[record_index].identity, &entry->player, sizeof(records[record_index].identity));
			s_cache_property *first_property = &properties[property_index];
			records[record_index].type = 0x61;
			records[record_index].count = 4;
			records[record_index].properties = first_property;
			record_index++;
			record_capacity--;
			*(short *)&first_property->unknown00 = -3;
			properties[property_index].type = 4;
			properties[property_index].unknown08 = (long)&buffers[0x40 * buffer_index];
			property_index++;
			buffer_index++;
			property_capacity--;
			buffer_capacity--;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[property_index].unknown00 = (short)(j + 2);
				properties[property_index].type = 1;
				properties[property_index].unknown08 = 0;
				property_index++;
				property_capacity--;
			}
		}
	}
	if (record_count)
		*record_count = record_index;
	if (property_count)
		*property_count = property_index;
	if (buffer_count)
		*buffer_count = buffer_index;
}

void function_80d70(s_cache_property_record *records, long count);

// @retail 0x7fd10
void function_7fd10(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->previous_other != NONE)
		g_4cf98c[entry->previous_other].next_other = entry->next_other;
	if (entry->next_other != NONE)
		g_4cf98c[entry->next_other].previous_other = entry->previous_other;
	if (g_4cf97c == index)
		g_4cf97c = entry->next_other;
	if (g_4cf980 == index)
		g_4cf980 = entry->previous_other;
}

// @retail 0x7fd80
void function_7fd80(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->unknown58 != NONE)
		g_4cf98c[entry->unknown58].next = entry->next;
	if (entry->next != NONE)
		g_4cf98c[entry->next].unknown58 = entry->unknown58;
	if (g_4cf984 == index)
		g_4cf984 = entry->next;
	if (g_4cf988 == index)
		g_4cf988 = entry->unknown58;
}

// @retail 0x7fc80
long __stdcall function_7fc80(const void *identity, long *position)
{
	long middle = 0;
	long lower = 0;
	long upper = g_4cf978 - 1;
	long result = NONE;
	bool last = false;
	while (result == NONE && !last && lower <= upper)
	{
		last = lower == upper;
		middle = (lower + upper) >> 1;
		long comparison = memcmp(identity, &g_4cf98c[middle].player, 12);
		if (comparison == 0)
			result = middle;
		else if (comparison < 0)
			upper = middle - 1;
		else
			lower = ++middle;
	}
	if (position)
		*position = middle;
	return result;
}

struct s_cached_player_identity
{
	dword values[3];
};
struct s_cached_player_source
{
	word name[32];
	dword field40;
	union
	{
		dword field44_47;
		struct
		{
			byte field44;
			byte field45;
			byte field46;
			byte field47;
		};
	};
	unsigned __int64 field48;
	byte unknown50[0x70 - 0x50];
	long field70;
	long field74;
	long field78;
	byte unknown7c[3];
	byte field7f;
	byte unknown80[0x10];
};
struct s_cached_player_view
{
	s_cached_player_identity identity;
	word name[32];
	dword field4c;
	byte field50;
	byte field51;
	byte field52;
	byte field53;
	byte unknown54[2];
	byte field56;
	byte unknown57;
};

// @retail 0x7f8b0
void function_7f8b0(s_cached_player_view *result, const s_cached_player_identity *identity,
	const s_cached_player_source *source, const s_cached_player_view *previous)
{
	memset(result, 0, sizeof(*result));
	result->identity = *identity;
	memcpy(result->name, source->name, sizeof(result->name));
	result->field4c = source->field40;
	result->field50 = source->field44;
	result->field51 = source->field45;
	result->field52 = source->field46;
	result->field53 = source->field47;
	byte index = source->field7f;
	if (index == 0xff && previous)
		result->field56 = previous->field56;
	else
		result->field56 = index;
}

// @retail 0x7fdf0
void function_7fdf0(long index)
{
	if (g_4cf97c != index)
	{
		function_7fd10(index);
		if (g_4cf97c != NONE)
			g_4cf98c[g_4cf97c].previous_other = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->previous_other = NONE;
		entry->next_other = (short)g_4cf97c;
		g_4cf97c = index;
	}
}

// @retail 0x7fe40
void function_7fe40(long index)
{
	if (g_4cf984 != index)
	{
		function_7fd80(index);
		if (g_4cf984 != NONE)
			g_4cf98c[g_4cf984].unknown58 = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->unknown58 = NONE;
		entry->next = (short)g_4cf984;
		g_4cf984 = index;
	}
}




struct s_player_appearance;
bool function_153750(s_player_appearance *appearance);

// @retail 0x80660
bool function_80660(void)
{
	bool valid = true;
	if (g_4cf970 != 8)
		valid = false;
	if (g_4cf978 < 0 || g_4cf978 > 350)
		valid = false;
	for (long i = 0; valid && i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (i > 0 && memcmp(&entry[-1].player, &entry->player, 12) >= 0)
			valid = false;
		if (valid)
		{
			byte appearance[16];
			memset(appearance, 0, sizeof(appearance));
			s_cached_player_view *view = (s_cached_player_view *)&entry->player;
			*(dword *)appearance = view->field4c;
			appearance[4] = view->field50;
			appearance[5] = view->field51;
			appearance[6] = view->field52;
			appearance[7] = view->field53;
			valid = function_153750((s_player_appearance *)appearance);
			if (valid)
				valid = !(view->identity.values[2] & 3) && view->identity.values[2] != 0xbad00000;
		}
	}
	if ((g_4cf97c == NONE || g_4cf980 == NONE) && g_4cf97c != g_4cf980)
		valid = false;
	if (g_4cf97c != NONE && (g_4cf97c < 0 || g_4cf97c >= g_4cf978))
		valid = false;
	if (g_4cf980 != NONE && (g_4cf980 < 0 || g_4cf980 >= g_4cf978))
		valid = false;
	if ((g_4cf984 == NONE || g_4cf988 == NONE) && g_4cf984 != g_4cf988)
		valid = false;
	if (g_4cf984 != NONE && (g_4cf984 < 0 || g_4cf984 >= g_4cf978))
		valid = false;
	if (g_4cf988 != NONE && (g_4cf988 < 0 || g_4cf988 >= g_4cf978))
		valid = false;
	if (valid && g_4cf97c != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf97c;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			if (visited[index >> 5] & (1 << (index & 31)))
				valid = false;
			else
			{
				visited[index >> 5] |= 1 << (index & 31);
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0)
				{
					if (entry->previous_other != NONE)
						valid = false;
				}
				else if (entry->previous_other < 0 || entry->previous_other >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next_other != NONE || index != g_4cf980)
						valid = false;
				}
				else if (entry->next_other < 0 || entry->next_other >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next_other;
					if (g_4cf98c[next].previous_other != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	if (valid && g_4cf984 != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf984;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			if (visited[index >> 5] & (1 << (index & 31)))
				valid = false;
			else
			{
				visited[index >> 5] |= 1 << (index & 31);
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0)
				{
					if (entry->unknown58 != NONE)
						valid = false;
				}
				else if (entry->unknown58 < 0 || entry->unknown58 >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next != NONE || index != g_4cf988)
						valid = false;
				}
				else if (entry->next < 0 || entry->next >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next;
					if (g_4cf98c[next].unknown58 != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	return valid;
}

struct s_profile_record;
void function_1537f0(s_profile_record *record);

// score 53 (was 67): frame 0x98 versus 0xa0: the three identity words fold away (retail stores them at
// +0x10 and reads them back for the copy, with the source record right after at +0x1c); ours spills a
// 350 countdown to +0x14 where retail compares the entry cursor with the array end (signed `jl`);
// packed source-byte carriers and initialization homes differ.
// Forward entry pointer 133; chained source resets 149; link-word fill 157; signed byte sentinel 149; unsigned entry index 133 (plain).
// Existing record initializer with zero tail 130; short/word counters 133/133;
// counters-before-version stores 132; separate link stores 133; initializer and store order 129 (plain).
// Array-end bound 129; workspace names 129; native identity zero 129; signed induction 122; array-end/native zero 129; signed/native zero 122; signed identity-array fill 122.
// Contiguous tail clear with sentinel restored 73; identity after record helper 122;
// fixed do loop with record helper 122; tail clear plus late identity 73;
// function-scope identity with record helper 122 (fast); tail clear 73 plain.
// Round 167: actual initializer record 73; native identity in that record context 74;
// source function scope/declaration-first 73/73; ordinary inline view on native/original
// source 73/73; out-of-line view on native/original source 80/79. Original body restored.
// With contiguous tail reset: identity/source/both names 73 each; signed byte sentinel 73;
// word ordinal 94; record/view/both force-inline declarations 73 each.
// Immutable identity with tail clear 74; source-before-identity names 73; source-only outer scope 73; identity between initializer and tail 73; tail-clear entry-end traversal 94.
// Complete destination clear 73; contiguous destination-tail clear 67 kept; destination first/after identity 75/75; links before view 98; tail before view 95; direct indexed destination 73; tail fill with early links 100.
// Ordinary record inline/native identity 67/67; immutable source/identity/both once 95/83/86;
// immutable source with native identity 95; split source-tail fills 115, with native identity 115.
// Contiguous link fill 67; link fill with native identity/late identity/intermediate identity 67/67/67;
// link fill with outer source/lexical source/unsigned ordinal/NONE byte 67/67/87/67.
// Complete zero suffix at field44/field40/field48/unknown50 67/67/67/67;
// suffixes at field70/field74/field78 give 99/89/83; whole source suffix 125. Frame remains 0x90.
// Native header zero fields 40/44/40+44/48/40+48/44+48/all 74/90/87/67/74/90/87;
// complete sixteen-byte header fill 154; source/identity homes remain unchanged.
// Entry zero/first sentinel pair/second pair chains and all pair subsets 67 each;
// complete endpoint sentinel chain 67. Frame 0x90 and packed source spills remain.
// Round 136: library copy of the identity in 0x7f8b0 67 (same code); XUID-typed identity 67;
// XUID id/flags zeroed by field 95 (flags store hoisted). Source byte 0x7f declared signed char
// with the tail clear and sentinel removed 78 at 238 bytes (no tail stores, source bytes shifted
// out of EBX as retail; identity still folded); with flags/timestamp as two stores 80 at 236;
// on that: record-typed source local 80, source declared first 80, memset through a sized helper 80,
// 64-bit id + flags identity type 80; per-entry force-inline helper 132.
// Round 146: identity byte/word/signed-word/signed-dword/header/byte-identity/word-flags/byte-
// flags views 67 each.
// Complete shared view library/native copies 128/128; per-visit library/native copies 128/128; shared-view array-end 126; ended input scopes 128; contiguous links 130; complete destination clear 148. Original body retained.
// Round 76 (a one-file harness with the record initializer's file reproduces this listing exactly):
// tail clear and sentinel removed 78 (238 bytes); source 64-bit field as two dwords 67 (the frame still
// realigns); identity zeroed after the record initializer 67; a one-element identity array 67; identity
// declared before the loop and zeroed in it 67; harness only: `= { 0 }`, XUID-based identity types,
// identity zeroed by its XUID fields (only the flags word stays in memory, its store hoisted) - none
// keeps the identity in memory.
// Round 7 (lane 15): identity zeroed by one chained assignment 95 (282 bytes); source declared first 67;
// both declared before the zeroing 67; `= { 0 }` in the real build 67; 0x7f8b0 defined in another file
// (inlined across files) 67. The
// harness also folds a dword[3]/byte[12]/union identity, a zeroing loop, a 64-bit identity type and an
// opaque entry pointer.
// Round 34: retail's frame is one 0x9c-byte object padded to 0xa0 (identity at +0x10, record right
// after at +0x1c): with the identity and the record as two members of one local struct the record's
// block store stops the zeros being forwarded, and the identity is stored and read back as retail
// does: 101 with the tail clear, 45 at 259 bytes without it and with the record's 64-bit field as two
// dwords (4-aligned record), 52 with it 8-aligned; links in retail's store order 45, the global NONE
// stores chained 45. Not kept: no existing type has that layout, and an invented grouping struct is
// not an accepted form. What then remains is NONE hoisted into EBP across the loop (retail keeps the
// record's first zero dword there).
// Round 12 (lane 13): tail clear and sentinel removed with the byte type kept (the initializer's NONE
// byte is forwarded as retail does) 78 at 238 bytes (aligned 43 against 51); with the flags as two
// stores / links as separate statements / the 64-bit field as two dwords 80 each at 236; the entry
// pointer declared first 67, with the tail removed 78.
// DL199: Signed high header bytes separately/pair, low pair and their three-group combinations all
// 67; all four signed bytes plus a signed first word 67. Missing identity homes and frame 0x90
// remain. Every real source type, initializer, initialized byte and all 350 visits restored.
// Round 45 (lane 16): the source as the existing 4-aligned player properties type (0x90, no 64-bit
// member), tail clear and sentinel removed 78 (238 bytes); with an XUID identity 78; with the tail
// clear kept 67, plus an XUID identity 67 (identity still folded each time; retail's +0x1c source
// needs that 4-aligned type); the entry pointer as the loop variable 98 (292 bytes).
// Round 106: identity value-initialized 67; that without the tail clear and sentinel 78 (238 bytes); identity
// declared and zeroed after the entry pointer 67; no tail clear with the links and flags stored before the
// 0x7f8b0 call 81; the entry pointer passed as the view 67.
// Round 6 (lane 17): identity zeroed once before the loop 83 (280 bytes); view built in a local view and
// copied into the entry 128 (311 bytes); without the tail clear and sentinel: links as four statements in
// retail's store order 78, identity zeroed after the entry pointer 78, identity zeroed once before the
// loop 81 (244 bytes). Original body restored.
// Round 111: flags and timestamp as two plain stores 73 (273 bytes); identity declared and zeroed right before
// the entry pointer 67; the link stores as four statements in retail's order 67; the identity as the XDK XUID
// type 67; the entry pointer as `g_4cf98c + i` 67. An LTCG harness copy of round 34's two-member local object
// again gives retail's identity stores and offsets (259 bytes), with NONE then hoisted into EBP.
// Round 98 (lane 7), `--fast`: the XUID identity zeroed by its two fields with the existing 4-aligned
// `s_player_properties` source and no tail clear 80 (249 bytes: retail's 0xa0 frame, but only the flags dword stays in
// memory, its store hoisted out of the loop, and NONE moves into EBP); the flags field zeroed first 75 (251); the
// identity zeroed dword by dword 80 (249); the XUID by memset with flags and timestamp as two stores 80 (236); the
// first form with those two stores 85 (241).
// Round 28 (lane 14), `--fast`: the tail clear dropped with the 0xff sentinel kept 78 (238 bytes); the identity
// initialized `= { { 0, 0, 0 } }` 95 (282); the source as a one-element array 67 (same code); the tail cleared as
// two memsets around the sentinel byte 125 (308); a plain `dword[3]` identity cast at the call 67 (same code, the
// real build folds it as the harness did). Every existing struct with a 0x90-byte properties member was checked
// (s_network_message_player_add, s_session_interface_user, s_network_session_player, the player settings
// source/entry): none puts a 12-byte identity immediately before it, so round 34's two-member layout has no
// existing type.
// Round 99 (lane 8): identity declared and zeroed once before the loop 83 (280 bytes, still folded), with the XUID
// type 83 (280); the source declared before the identity 67; the XUID identity per iteration 67; the identity zeroed
// after the entry pointer 67. No existing type has an identity followed by the 0x90-byte record.
// Round 44 (lane 1, fast): an XUID identity with the source declared first 67; the link stores before the
// identity and the 0x7f8b0 call 100 (284 bytes); the entry's flags/time clear first 71 (273); a function-scope
// XUID identity zeroed in the loop 67; the XUID identity zeroed right before the call 67. A compile-only build
// folds the identity even with the record initializer out of line, so only an identity inside a larger local
// object stays in memory (round 34); no existing 0x9c-byte type was found.
// Round 101 (lane 7), `--fast`: identity `= { 0, 0, 0 }` 95 (282 bytes); a const identity `= { 0 }` 67; assigned from a
// value-initialized temporary 67; memset of its member array 67; the zero identity copied into the entry first and the
// entry's own identity passed to 0x7f8b0 97 (284 bytes).
// MiniMax E (10-31): the record's own tail clear and byte sentinel dropped (the record initializer
// already leaves field 0x7f at 0xff) 78 (238 bytes, plain); the entries walked with a pointer 53 (255 bytes).
// MiniMax E (10-31): the link words' -1 is the same constant as the globals' NONE and so stays in EBP; retail
// keeps it in EAX and gives the record's field 0x40 its own register; 45 (259 bytes).
// MiniMax E (10-31): the identity and the record in one two-member local struct (identity, then record) and the
// entries walked with a signed index 45 (259 bytes, plain); not kept (a made-up struct that sets the stack homes;
// no existing type pairs them). The record's field 0x40 then shares a register with the field 0x44 word and the
// NONE constant lives in EBP, so the entry-tail clear is an LEA with two ECX stores.
// Round 49 (lane 10): the entries walked with a pointer `for (entry = g_4cf98c; entry < g_4cf98c + 350;
// entry++)` with the record's tail clear and 0xff sentinel kept 98 (292 bytes); the identity declared and
// zeroed after the entry pointer 67; the existing 4-aligned s_player_properties as the record with the tail
// clear dropped 78 (238 bytes), with an XUID identity zeroed after the record initializer 78 (238); the index
// loop with the tail clear dropped 78 (238). Kept: the pointer walk with the tail clear and sentinel dropped
// (the tail clear zeroed fields 0x7c-0x8f the inlined view copy never reads, and the sentinel re-stored the 0xff
// the record initializer already leaves in field 0x7f) 53 (255 bytes, plain). On that: `entry !=
// g_4cf98c + 350` 53, the record before the identity 53, an XUID identity 53, the identity `= { 0 }` 53. The
// identity is still folded in every form.
// @retail 0x7f930
void function_7f930(void)
{
	g_4cf974 = 0;
	g_4cf978 = 0;
	g_4cf970 = 8;
	g_4cf980 = NONE;
	g_4cf97c = NONE;
	g_4cf984 = NONE;
	g_4cf988 = NONE;
	for (s_player_configuration_cache_entry *entry = g_4cf98c; entry < g_4cf98c + 350; entry++)
	{
		s_cached_player_identity identity;
		memset(&identity, 0, sizeof(identity));
		s_cached_player_source source;
		function_1537f0((s_profile_record *)&source);
		function_7f8b0((s_cached_player_view *)&entry->player, &identity, &source, NULL);
		entry->next = entry->next_other = entry->unknown58 = entry->previous_other = NONE;
		memset(&entry->flags, 0, sizeof(entry->flags) + sizeof(entry->unknown64));
	}
}

#include "files.h"
#include "crc.h"
#include "async.h"

struct file_reference_data;
void async_create_file_blocking(file_reference_data const *s_type_acf665, dword access_flags, long disposition, dword file_flags, long category, s_file_handle *file);
bool function_1368f0(s_type_acf665 *file);

char const g_450c68[] = "z:\\player_configuration_cache.dat";
bool g_467260 = true;
s_file_handle g_467264 = { INVALID_HANDLE_VALUE };

/* loads the cache from its file; true when the file has the cache's size,
   its checksum and a consistent cache */
// @retail 0x7fa40
bool function_7fa40(void)
{
	bool local_0 = false;
	s_type_acf665 local_1;
	memset(&local_1, 0, sizeof(local_1));
	local_1.signature = FILE_REFERENCE_SIGNATURE;
	local_1.location = NONE;
	if ((byte)local_1.flags & 1)
		function_1373c0(local_1.path);
	function_137320(local_1.path, g_450c68);
	*(byte *)&local_1.flags |= 1;
	if (function_1368f0(&local_1))
	{
		dword local_2 = 0;
		if (file_ref_size(&local_1, &local_2) && local_2 == 0x8e4c)
		{
			dword local_3;
			if (function_136970(&local_1, 1, &local_3))
			{
				if (file_ref_read(&local_1, 0x8e4c, &g_4cf970, true))
				{
					dword local_4 = NONE;
					function_163ba0(&local_4, &g_4cf978, 0x8e44);
					if (local_4 == (dword)g_4cf974 && function_80660())
						local_0 = true;
				}
				function_136bb0(&local_1);
			}
		}
	}
	return local_0;
}

/* starts writing the cache to its file, opening the file the first time;
   false when the file can't be opened */
// @retail 0x7fb50
bool function_7fb50(void)
{
	bool local_0 = false;
	s_type_acf665 local_1;
	file_ref_create(&local_1, false, g_450c68);
	if (g_467264.handle == INVALID_HANDLE_VALUE)
	{
		async_create_file_blocking((file_reference_data const *)&local_1, 2, 4, 4, 2, &g_467264);
		g_467260 = true;
		if (g_467264.handle != INVALID_HANDLE_VALUE)
			function_1a1310(g_467264, 0, 2, 6, NULL, &g_467260);
	}
	if (g_467264.handle != INVALID_HANDLE_VALUE)
	{
		g_4cf974 = NONE;
		function_163ba0((dword *)&g_4cf974, &g_4cf978, 0x8e44);
		g_4cf970 = 8;
		function_120d50(&g_467260, false);
		function_1a1050(g_467264, &g_4cf970, 0x8e4c, 0, s_file_write_flags(0), 2, 6, NULL, &g_467260);
		local_0 = true;
	}
	return local_0;
}

// @retail 0x80bf0
void function_80bf0(long record_capacity, s_cache_property_record *records,
	long property_capacity, s_cache_property *properties, byte *buffers,
	long *record_count, long *property_count, long *buffer_count)
{
	long record_index = 0;
	long property_index = 0;
	long buffer_index = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		if (g_4cf98c[i].flags & 1)
		{
			if (record_capacity <= 0 || property_capacity < 4)
				break;
			s_player_configuration_cache_entry *entry = &g_4cf98c[i];
			long values[3];
			memcpy(values, (byte *)&entry->player + 0x4c, sizeof(values));
			memcpy(records[record_index].identity, &entry->player, sizeof(records[record_index].identity));
			records[record_index].type = 0x61;
			records[record_index].count = 4;
			s_cache_property *record_properties = &properties[property_index];
			records[record_index].properties = record_properties;
			record_index++;
			record_capacity--;
			memcpy(&buffers[0x40 * buffer_index], (byte *)&entry->player + 0xc, 0x40);
			*(short *)&record_properties->unknown00 = -3;
			properties[property_index].type = 4;
			properties[property_index].unknown08 = (long)&buffers[0x40 * buffer_index];
			property_index++;
			buffer_index++;
			property_capacity--;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[property_index].unknown00 = (short)(j + 2);
				properties[property_index].type = 1;
				properties[property_index].unknown08 = values[j];
				property_index++;
				property_capacity--;
			}
		}
	}
	if (record_count)
		*record_count = record_index;
	if (property_count)
		*property_count = property_index;
	if (buffer_count)
		*buffer_count = buffer_index;
}

#include <xtl.h>
#include <wchar.h>

// @retail 0x80aa0
void function_80aa0(s_cache_property_record *records, long count)
{
	long position;
	if (count <= 0) return;
 s_cache_property *const *local_0 = &records->properties;
 long local_2 = count;
 do
	{
		const s_cached_player_identity *local_1 = (const s_cached_player_identity *)((byte const *)local_0 - 0x14);
		if (*(long const *)((byte const *)local_0 - 8) == 0x61 && *(long const *)((byte const *)local_0 - 4) == 4)
		{
			long index = function_7fc80(local_1, &position);
			if (index != NONE)
			{
				s_cache_property *properties = *local_0;
				long values[3];
				long present = 0;
				long absent = 0;
				if (properties[0].type == 0)
					absent = 1;
				else if (properties[0].type == 4 && properties[0].unknown08 != 0)
					present = 1;
				for (long j = 0; j < 3; j++)
				{
					if (properties[j + 1].type == 0)
						absent++;
					else if (properties[j + 1].type == 1)
					{
						values[j] = properties[j + 1].unknown08;
						present++;
					}
				}
				if (absent + present == 4)
				{
					s_player_configuration_cache_entry *entry = &g_4cf98c[index];
					if (present == 4)
					{
						wchar_t *name = (wchar_t *)((byte *)&entry->player + 0xc);
						wcsncpy(name, (const wchar_t *)properties[0].unknown08, 31);
						name[31] = 0;
						memcpy((byte *)&entry->player + 0x4c, values, sizeof(values));
						entry->flags &= ~8;
					}
					unsigned __int64 time = 0;
					GetSystemTimeAsFileTime((FILETIME *)&time);
					*(dword *)entry->unknown64 = (dword)(time / 3600000000ULL);
					entry->flags &= ~2;
				}
			}
		}
  local_0 = (s_cache_property *const *)((byte const *)local_0 + 0x18);
	} while (--local_2);
}

bool g_51055c;

struct s_profile_record;
void function_1537f0(s_profile_record *record);
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source);

#pragma inline_depth(0)
static __forceinline void function_80491(long arg_0)
{
 function_7fe40(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80490
void function_80490(const s_cached_player_identity *identity, const s_cached_player_source *source,
	bool local, bool recent)
{
	bool time_changed = false;
	long position = 0;
	long index = function_7fc80(identity, &position);
	bool changed;
	if (index == NONE)
	{
		index = function_7fe90(identity, position, source);
		changed = true;
	}
	else
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		s_cached_player_view player;
		function_7f8b0(&player, identity, source, (const s_cached_player_view *)&entry->player);
		changed = memcmp(&entry->player, &player, sizeof(entry->player)) != 0;
		if (changed)
			memcpy(&entry->player, &player, sizeof(entry->player));
		entry->flags &= ~10;
		unsigned __int64 time = 0;
		GetSystemTimeAsFileTime((FILETIME *)&time);
		dword hours = (dword)(time / 3600000000ULL);
		if (hours != *(dword *)entry->unknown64)
			time_changed = true;
		*(dword *)entry->unknown64 = hours;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (local)
	{
		if (changed)
			entry->flags |= 1;
		entry->flags |= 4;
	}
	else
		entry->flags &= ~4;
	if (recent)
	{
		changed = changed || !(entry->flags & 16);
		entry->flags |= 16;
	}
	function_80491(index);
	if (changed || time_changed)
		g_51055c = true;
}

#pragma inline_depth(0)
// @retail 0x80330
long __stdcall function_80330(const s_cached_player_identity *identity, long position)
{
	s_cached_player_source source;
	function_1537f0((s_profile_record *)&source);
	((byte *)&source.field40)[2] = 0;
	((byte *)&source.field40)[3] = 0;
	source.field45 = 0;
	source.field46 = 0;
	((byte *)&source.field40)[0] = 10;
	((byte *)&source.field40)[1] = 10;
	long index = function_7fe90(identity, position, &source);
	if (index != NONE)
 {
  s_player_configuration_cache_entry *local_0 = g_4cf98c + index;
  local_0->flags |= 10;
 }
	return index;
}
#pragma inline_depth(8)

// score 49 (was 49): frame 0xc and the whole tail agree; the admission prefix (a `cache_full` bool, retail
// tests the count directly and loads the removed index into EDX) and the upper bound's register (retail ECX,
// so the forward copy index takes EAX) remain.
// Paired range arms 373;
// native FILETIME 314; short adjustment 300; unsigned repair counter 364;
// range/time/short combination 375; native time and short adjustment 357.
// Three relative-position cases 243; early bounds 377; explicit upper bound 361;
// forward destination index 366; reverse destination index 360; widened
// global-index adjustment 358.
// First removal-call acquisition 243; real repair-table cursor 819; short repair counter 819;
// long direction flag 244; equality-first cases 243; forward CRT memmove 385.
// Existing complete view initializer 240 (three-point gain rejected); reverse memmove 380; native-entry memcpy 243; signed repair while 243; isolated signed-min lower bound 306 (fast).
// Paired signed min/max 302; independent bound overrides 362; forward entry cursor 369; reverse entry cursor 394; both entry cursors 406.
// Branch-produced removed index 390; neutral lower/both bounds 309/243; shared signed direction 343;
// identity memcpy 243; chained sentinel stores 243; new-entry predicate 381; branch/native bounds 390;
// signed link values 815 (repair unrolling lost); original body restored.
// Full-width adjustment with three-case bounds 256; adjustment with shared initializer 253; pre-copy short adjustment 378; adjusted insertion local 390; less-first cases 243.
// Frame 0xc agrees; removed-index EDX/ESI and upper ECX/EAX remain. Direction value 342; signed-byte direction
// 243; range role names 243; direct indexed repair 243; signed min/max with long adjustment 370.
// Upper-first repair tests 375; nested tests 243; four head values 238 kept; branch-produced bounds
// 243; signed-byte adjustment 445; upper-first/head values 377; bounds/byte 445; nested/head values
// 238.
// Insertion destination before shifts 384; earlier view 243; both addresses 384;
// insertion/time role names 243; early destination with names 384; both with names 384;
// early destination/time workspace 393; all lifetimes/names 393. Original body restored.
// Round 124: No-removal-first range 371; uninitialized time output 385; packed metadata memcpy 257;
// real-array repair bound 375; consistent table base 394; first/second/both removal inline declarations 243/243/243.
// All five body forms and all three declaration trials restored.
// Round 232: native link-block loop 812; unsigned link-block cursor 812.
// native link array 238; memory-order native links 238.
// native links/shared initializer 240; native links/signed snapshots 834.
// native loop/full adjustment 826; native links/live heads 243.
// Constant removal result/shared full predicate with retained cases 385/381; with signed bounds 391/181 kept.
// Existing initializer on those four forms 383/380/393/184; bounds alone 304; removal-first lower 181;
// nonfull polarity 190; full-width admission result 402. Kept form is 1185/1171 bytes, plain 181.
// Current admission/bounds with long adjustment 66; byte admission 181; native time 194;
// long/byte 66, long/time 137, byte/time 194, all three 137; mutable removed/long 66.
// Round 10: existing initializer 98; direct/branch-shared removal 370/370;
// separate admission 394; removal-first lower forms 66/66/64; forward source entry 66;
// forward memmove 380; sentinel-first links 157; early view/bound declaration swap/
// const admission with the 64-point lower 64/64/64; default insertion range 388;
// three current-graph cases 386. The two-point lower gain was not kept; original body restored.
// Round 149: plain entry (removed = NONE, set inside the full test) 370; with lower/upper as
// `removed > position ? ...` and an explicit `>` arm 355; plus the flag cleared in the count path
// (retail tail-merges that arm with it) 74 at 1171 bytes, with the flag set before the decrements and
// upper assigned first there 71: entry, min/max and three-way test then match, but upper takes EAX
// (retail ECX, copied from the count) and the forward index EBP (retail EAX). On 71: one shared shift
// index 74; range locals in three orders 74; named count 71; entry-pointer forward copy 71, copy from
// lower 364; upper before lower 75; bounds zeroed in an else arm 71. Original body restored.
// Round 215: native timestamp dword member 66; closed bounds workspace 66;
// entry time workspace 66; pre-copy long adjustment 382; indexed destination 168;
// mutable removal with upper-first declarations 66; shared range-direction relation 381;
// indexed destination with native timestamp member 168. Original body restored.
// Round 7 (lane 15): link stores next_other/unknown58 first 65, with chained NONE stores 65, NONE stores
// first 157; round 149's entry rebuilt 71 (entry matches but for the g_4cf980 load target and upper in EAX
// coalesced with the count); on it: those link stores 70, count path lower first 72, named count 71,
// flag declared first 71, flag set after the decrements 73, flag declared after the removal 71.
// Round 183: actual endpoint 389; plain endpoint 396; one count 89 (frame 0x10); endpoint/count 380; plain endpoint/count 394; endpoint relation 387; plain relation 394; combined count/relation 384. Original body restored.
// Round 12 (lane 14), all on round 149's 71 form (its entry and three-way test match; upper stays in EAX,
// coalesced with the count, where retail copies the count into ECX): rebuilt 71; count path lower first 72;
// `g_4cf978 > position` 73; upper declared before lower 71; the flag declared first 71. Original body restored.
// Round 119 (lane 4), on round 149's entry (71, aligned 39 against the current 55): retail's entry
// (load into EDX, copy to ESI, spill, call) comes only from `removed = g_4cf980;
// function_7fd10(g_4cf980); function_7fd80(removed);` 69 (aligned 37; a mixed read, kept only with a
// match); the calls through an inline unlink helper 71, the helper returning the index 74 (spill after
// the calls), the global passed to the helper 73, a separate `last` local 76; on the 69 form: lower
// first in the count path 70, upper declared first 69, `g_4cf978 > position` 71, upper as `<` 70,
// upper initialized from lower 69 (upper stays in EAX with the count); link stores next_other,
// unknown58, previous_other, next 68 (next_other then stored first as retail, but the g_4cf988 load
// moves ahead of the NONE stores into DX), previous_other first 161, unknown58 third 69, next first 161.
// Round 80: plain entry (removed = NONE, set in the full test), `removed <= position ? removed : position`,
// an explicit `>` arm clearing the flag, the flag set before the decrements and the count path upper
// first with its flag cleared 73 (aligned 41 against 58: the entry and the three-way test match; upper
// still takes EAX, so the forward copy index takes EBP and a jump over the loop padding shifts the
// rest); on it: lower first in the count path 74, upper declared first 73, upper before lower 75, the
// lower ternary on `position < removed` 73, bounds left unset and zeroed in a final else 73.
// Round 47: on the current body, `lower = removed > position ? position : removed` 64 (retail's
// cmovle; upper still EAX because the `cache_full` sete puts the count in ECX); the three-way test
// with the flag cleared in the `>` arm and the count path 380, upper first there 380; plain entry
// with that lower 368 (entry as retail but the EDX load, upper coalesced with the count in EAX);
// plain entry with the three-way test 74, with round 119's mixed read 72.
// Round 45 (lane 16), `--fast`: round 149's 71 form rebuilt (71); on it: one shift index for both
// loops 71, bounds without zero initializers and the count arm as a plain else 398 (1155 bytes,
// EBP pushed late), the insertion bounds as defaults replaced for a removal 395 (1155); the existing
// 0x7f8b0 call for the new entry's fill (inlined; the aulldiv, identity and copy registers then match
// retail) on the current body 98 (1174 bytes), on the 71 form 104 (the four head repairs swap EAX/ECX
// and next_other is stored from AX), with the heads adjusted in place 104.
// Round 106: round 149's plain entry rebuilt 71 (aligned 39); it without the flag initialization (each arm sets
// the direction) 390; time stored through the dword member 66; time read through a zeroed FILETIME 66; the NONE
// link stores as `(short)NONE` 66; g_4cf97c named once for the next_other store 66.
// Round 6 (lane 17): round 149's plain-entry form rebuilt 71 (upper coalesced with the count in EAX);
// on it the `<` arm first with `>` as its else-if 336, bounds initialized to position 71; the hour
// stamp through an inline helper 66 (71 on the plain-entry form); the shift block as a forceinline
// helper taking (lower, upper, downward) 66 (71); flags cleared before the link stores 157 (1167
// bytes); the zeroed time local declared before the entry pointer 70 (75). Original body restored.
// Round 111: the four head repairs on the globals directly (no named copies) 49 kept: the link stores, the
// g_4cf988 load and the time and identity-copy registers then match. On it: round 149's plain entry with
// `removed > position` bounds, an explicit `>` arm and the count path upper first clearing the flag 54 (only
// upper in EAX, the EDX load and the forward-copy addressing differ), the flag set after the decrements 56, the
// count path lower first 55; the count path upper first without the flag 50; the 0x7f8b0 call for the fill 98.
// Harness screens on the 54 form, all with upper still in EAX: every min/max spelling and three-way order, the
// declaration orders, a named count, one copy index for both loops, the copy and repair loops as force-inline
// helpers, a plain else for the count path (1155 bytes). Permuter on the 49 form: nothing better in 60 tries.
// Round 98 (lane 7), `--fast`: round 149's plain entry with the explicit flag arms, the count path upper first and
// `downward = true` first in the `<` arm 54 (aligned 23: retail's `mov bl, 1` before the spills); that with the 0x7f8b0
// call for the fill 104 (1174 bytes); the current body with `downward = true` first 50; the plain entry form with
// `lower < upper` 54, with the count path's flag store first 54. Harness, all with upper in EAX: four copy-loop index
// spellings, a function-scope copy index, the eight declaration orders above the removal block (373), the count path
// first, an unconditional count path (1155 bytes), short bounds, bounds without initializers, removed set in an else.
// Round 28 (lane 14), `--fast`: round 149's plain entry with `removed > position` bounds, the explicit `>` arm and
// the count path upper first clearing the flag 56 (aligned 25: entry load into ESI then EDX where retail loads EDX
// and copies to ESI, upper in EAX where retail copies the count into ECX); on it the bounds renamed
// last/first, high/low or upper_bound/lower_bound, `removed` renamed, the flag renamed 56 each (same code: the
// names do not move registers); the count path without its flag store 355 (1162 bytes); the `>` arm without its
// flag store 370 (1162); bounds and flag declared above the removal 378; the count path as a nested if with the
// flag cleared in the outer else 391 (1178).
// Round 99 (lane 8): the tail (head repairs, links, time, fill) already matches; on round 111's plain-entry form
// (rebuilt 54): bounds zeroed in a final else 54, `g_4cf978 > position` 56, upper before lower in the removal arm 58,
// the count path's flag store first 54; harness: count path lower first 55, the mixed `function_7fd10(g_4cf980)` read
// 52, lower and upper initialized to position 369 (1162 bytes); the existing 0x7f8b0 call for the fill 100 (1174 bytes:
// the head repairs reorder and carry g_4cf97c in EAX, field56 through AL).
// Round 44 (lane 1, fast): round 149's plain-entry form rebuilt with `downward = true` first 54. A compile-only
// /Gd build reproduces this prefix exactly (upper in EAX, forward copy index EBP); deleting the backward copy
// (diagnostic) frees EAX: its index `i = upper - 1` coalesces with upper (`dec eax`, retail `lea eax, [ecx - 1]`).
// On the current body: backward copy as `for (i = upper; i > lower; i--) g[i] = g[i - 1]` 373 (upper ECX, but the
// copy's strength reduction changes); backward/forward copies through a per-iteration element pointer 107/66;
// forward copy as `for (i = lower; i < upper; i++) g[i] = g[i + 1]` 371; the copy arms swapped 70. Screened only
// (upper stays EAX): count-path orders, declaration orders, ternary spellings, a shared index, `--i` loops.
// Round 101 (lane 7), `--fast`: round 111's plain-entry form rebuilt 54 (upper coalesced with the count in EAX; retail
// copies the count into ECX for upper); copy loops indexed by the destination (`i = lower; i < upper`, `i = upper;
// i > lower`, both) on the current body 371/373/373, on the plain-entry form 364/361/361; on the plain-entry form the
// direction step computed before the copy loops 358, the flag set from `removed < position` 390 (1162 bytes).
// Round 44 (lane 10, fast): round 111's plain-entry form rebuilt 56 (upper coalesced with the count in EAX; on the
// current body the `cache_full` sete puts the count in ECX and upper in EAX, retail has count EAX, upper ECX); on it
// the count path reading the count into a block-local `count` for the test and the bound 56, with lower assigned
// first and no flag store 355 (1162 bytes); the current body with that block-local count 49 (identical); the current
// body's three-way test as `position > removed` 50; the plain entry with the bounds and flag declared above the
// removal block 378.
// Round 140 (lane 4): on the current body, lower as `removed <= position ? removed : position` 49 (same code);
// upper as `position < removed ? removed : position` 51; no `cache_full` flag (the removal calls guarded by
// `removed != NONE`) 387 (1161 bytes); the plain entry alone, re-measured on this body, 370 (1162 bytes), with the
// cmovle-shaped lower 370; the direction step as `change = 1; if (downward) change = -1;` 82.
// Round 28 (m3 lane 2, fast with 0x80330/0x80490): on the current body, the count path assigning upper before
// lower 50; the zero-initialized bounds declared above the full-cache test 49 (same code: their zero values are
// threaded away, so they never interfere with the count and upper still shares its register); `cache_full`
// kept with `removed` set to NONE and assigned inside the test 370 (1162 bytes); the flag declared before the
// bounds 49; the shift test written `lower < upper` 49. Retail's upper takes ECX because the backward copy's
// index `i = upper - 1` gets EAX (`lea eax, [ecx - 1]`); ours puts upper in EAX and decrements it in place.
// Round 59 (fast): 1-8 unused locals at the top (diagnostic) 49 each. Round 149's plain entry rebuilt (cmovle lower,
// explicit `>` arm, count path upper first with the flag cleared, flag set before the decrements) 54: only the
// EDX/ESI load of the removed index and upper in EAX (coalesced with the count and the backward index) differ.
// On it: backward copy as `for (i = upper; i > lower; i--) g[i] = g[i - 1]` 361 (1165 bytes, upper still EAX);
// the copy arms tested as `!downward` 54; the two calls passed g_4cf980 with `removed` assigned after them 373
// (1177 bytes); the count test as `g_4cf978 > position` 56; lower as `removed <= position ? removed : position` 56.
// Round 17 (lane 17, fast): round 149's plain entry with the explicit `>` arm and the count path upper first
// rebuilt 56; with round 119's mixed read 54 (first difference +0x4c: the entry and EDX load are retail's);
// plus `downward = true` first in the `<` arm 52 (only upper in EAX, against retail's ECX, and the forward
// copy index in EBP, retail EAX, remain: retail's forward-copy index outranks upper for EAX); on 52: one
// function-scope index for the copy and repair loops 52 (identical), upper as `position < removed ? removed
// : position` 54, the no-removal arm reading the count into a local for its test and upper 52 (identical).
// Round 37 (lane 15, fast; current body): the backward copy as `long i = upper; while (--i >= lower)` 49, the
// forward copy as `long i = lower; while (++i <= upper)` 49, both 49 (all identical: the loop spelling is
// canonicalised, upper still coalesces with the backward index); the flag declared before the bounds 49; the four
// entry-link repairs through one inline (short *, lower, upper, change) adjust helper 49 (identical).
// Round 127 (lane 6, fast with 0x80490/0x80330): a compile-only /Gd harness scored against retail (a normalized diff,
// 47 for the build's 49) reproduces upper taking EAX; there six declaration orders of lower/upper/downward, upper as
// `position < removed ? ...`, lower as `position >= removed ? ...`, upper assigned before lower, the count path
// lower first, `lower < upper`, upper declared first in the function and one block-scope copy index all leave it
// in EAX (34-35 on the plain-entry form). Real: round 149's plain entry with the explicit flag arms and the count
// path upper first 56, with `lower = removed > position ? position : removed` 54 (only the EDX/ESI entry load and
// upper in EAX left); the plain entry alone 370 (1162 bytes), with the explicit flag arms only 358, with the count
// path's flag store only 372.
// Round 37 (lane 15, fast; current body): the backward copy as `long i = upper; while (--i >= lower)` 49, the
// forward copy as `long i = lower; while (++i <= upper)` 49, both 49 (all identical: the loop spelling is
// canonicalised, upper still coalesces with the backward index); the flag declared before the bounds 49; the four
// entry-link repairs through one inline (short *, lower, upper, change) adjust helper 49 (identical).
// @retail 0x7fe90
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source)
{
	bool cache_full = g_4cf978 == 350;
	long const removed = cache_full ? g_4cf980 : NONE;
	if (cache_full)
	{
		function_7fd10(removed);
		function_7fd80(removed);
	}
	long lower = 0;
	long upper = 0;
	bool downward = false;
	if (removed != NONE)
	{
		lower = position < removed ? position : removed;
		upper = removed > position ? removed : position;
		if (removed < position)
		{
			upper--;
			position--;
			downward = true;
		}
	}
	else if (position < g_4cf978)
	{
		lower = position;
		upper = g_4cf978;
	}
	if (upper > lower)
	{
		if (downward)
		{
			for (long i = lower + 1; i <= upper; i++)
				g_4cf98c[i - 1] = g_4cf98c[i];
		}
		else
		{
			for (long i = upper - 1; i >= lower; i--)
				g_4cf98c[i + 1] = g_4cf98c[i];
		}
		long change = downward ? -1 : 1;
		for (long i = 0; i < 350; i++)
		{
			s_player_configuration_cache_entry *entry = &g_4cf98c[i];
			if (entry->previous_other >= lower && entry->previous_other <= upper) entry->previous_other += change;
			if (entry->next_other >= lower && entry->next_other <= upper) entry->next_other += change;
			if (entry->unknown58 >= lower && entry->unknown58 <= upper) entry->unknown58 += change;
			if (entry->next >= lower && entry->next <= upper) entry->next += change;
		}
		if (g_4cf97c >= lower && g_4cf97c <= upper) g_4cf97c += change;
		if (g_4cf980 >= lower && g_4cf980 <= upper) g_4cf980 += change;
		if (g_4cf984 >= lower && g_4cf984 <= upper) g_4cf984 += change;
		if (g_4cf988 >= lower && g_4cf988 <= upper) g_4cf988 += change;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[position];
	entry->next_other = (short)g_4cf97c;
	entry->previous_other = NONE;
	entry->next = NONE;
	entry->unknown58 = (short)g_4cf988;
	entry->flags = 0;
	unsigned __int64 now = 0;
	GetSystemTimeAsFileTime((FILETIME *)&now);
	*(dword *)entry->unknown64 = (dword)(now / 3600000000ui64);
	memset(&entry->player, 0, sizeof(entry->player));
	s_cached_player_view *player = (s_cached_player_view *)&entry->player;
	player->identity = *identity;
	memcpy(player->name, source->name, sizeof(player->name));
	player->field4c = source->field40;
	player->field50 = source->field44;
	player->field51 = source->field45;
	player->field52 = source->field46;
	player->field53 = source->field47;
	player->field56 = source->field7f;
	if (g_4cf97c != NONE && g_4cf97c != position)
		g_4cf98c[g_4cf97c].previous_other = (short)position;
	g_4cf97c = position;
	if (g_4cf980 == NONE) g_4cf980 = position;
	if (g_4cf988 != NONE && g_4cf988 != position)
		g_4cf98c[g_4cf988].next = (short)position;
	g_4cf988 = position;
	if (g_4cf984 == NONE) g_4cf984 = position;
	g_51055c = true;
	if (removed == NONE) g_4cf978++;
	return position;
}

struct s_network_session_player;

// @retail 0x805d0
void __fastcall function_805d0(s_network_session_player *player)
{
 function_80490((const s_cached_player_identity *)player,
  (const s_cached_player_source *)((const byte *)player + 0xa8), false, true);
}

#pragma inline_depth(0)
static __forceinline void function_80441(long arg_0)
{
 function_7fdf0(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80440
void function_80440(const s_cached_player_identity *identity, s_recent_player *player)
{
 long position = 0;
 long index = function_7fc80(identity, &position);
 if (index == NONE)
  index = function_80330(identity, position);
 memcpy(player, &g_4cf98c[index].player, sizeof(*player));
 function_80441(index);
}

bool g_510554;
dword g_510558;
bool function_7fb50(void);

// @retail 0x80390
void function_80390(void)
{
 if (g_51055c && g_510554)
 {
  dword local_0 = GetTickCount();
  if (local_0 - g_510558 > 10000)
  {
   if (function_7fb50())
    g_51055c = false;
   g_510558 = local_0;
  }
 }
 unsigned __int64 local_1 = 0;
 GetSystemTimeAsFileTime((FILETIME *)&local_1);
 dword local_2 = (dword)(local_1 / 3600000000ULL);
 long local_3 = 0;
 s_player_configuration_cache_entry *local_4 = g_4cf98c;
 if (g_4cf978 > 0)
 {
  do
  {
   if (local_2 - local_4->field_64 > 72)
    local_4->flags |= 2;
   local_3++;
   local_4++;
  } while (local_3 < g_4cf978);
 }
}

s_4cf790 g_4cf790_storage;
