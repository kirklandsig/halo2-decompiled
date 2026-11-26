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

/* the entry at the iterator, moving the iterator to the next one; false at the
   end of the chain */
static inline bool player_configuration_cache_iterate(long *iterator, s_player_configuration_cache_entry **entry)
{
	long index = *iterator;
	if (index == NONE)
		return false;
	*entry = &g_4cf98c[index];
	*iterator = (*entry)->next;
	return true;
}

static inline bool player_configuration_is_excluded(s_recent_player const *player)
{
	return (player->unknown08[0] & 3) != 0;
}

/* the next recent player after the iterator's: a valid, online, known player
   with a name; iterator starts at the first entry and ends at NONE */
// @retail 0x805e0
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator)
{
	bool result = false;
	s_player_configuration_cache_entry *entry;
	for (bool more = player_configuration_cache_iterate(iterator, &entry); more;
		more = player_configuration_cache_iterate(iterator, &entry))
	{
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
struct s_cached_player_record
{
	s_cached_player_identity identity;
	byte source_bytes[0x90];
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

// score 18 (was 18): the loop's entry pointer is based at the entry's link run (EDX = entry + 0x5c) where
// retail bases it at the view's field 0x4c (entry + 0x4c), so every entry store's displacement differs, and the
// view's byte stores and the link stores come out in another order (ours makes the 0x52 byte and the NONE first).
// MiniMax D (11-04): no new form, no lower score; the identity as three named dwords instead of an array
// 53 (same code), it still folds into registers and the frame stays 0x98.
// MiniMax E (11-05): no new form, no lower score; splitting the source record's 8-byte field into two
// dwords to make it 4-aligned leaves the record at +0x18 and the frame at 0x98, and an index loop gives 78.
// MiniMax C (11-08): no new form, no lower score.
// Earlier (score 53, was 53): frame 0x98 versus 0xa0: the three identity words fold away (retail stores them at
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
// Round 327: Carried entry pointer with direct long/unsigned-long/short/word count-up and
// countdown bounds, 53 each at 255 bytes. Identity homes still fold; original pointer walk and
// all initializers restored.
// Round 36 (fast, with 0x7f8b0/0x1537f0): retail forwards the record's memset zeros into EBP/EBX but stores and
// re-reads the identity, and its `cmp edx, end; jl` (signed) is the strength-reduced test of an index loop; ours
// counts 350 down in a frame slot. Index loop with every field through `g_4cf98c[i]` 77 (238 bytes); the identity
// zeroed after the record initializer in an index loop 78 (238); the links as four statements in retail's store
// order on the pointer walk 53 (same code); flags and timestamp as two stores on the pointer walk 62 (253); the index
// loop with both 80 (236). The compile-only build cannot inline 0x1537f0 (another file), so it does not reproduce this.
// DL233: Actual four-aligned producer record on the pointer/no-tail graph, native/library
// identity zeroing, identity before/after the producer, destination tail before/after view:
// 53/53/53/53/62/62/63/63. Original source type, forward declarations and full body
// restored; identity still folds and frame remains 0x98 versus 0xa0.
// Complete cache-entry construction including metadata, native/library full-record copies,
// live/ended input scopes and metadata before/after payload: 92/92/92/92/81/81/81/81.
// Original complete body restored; identity homes and signed end-bound remain.
// Round 343: Four-aligned profile/property sources with original/late mutable or immutable
// identity: 53/74/53/74/53/74/53/74. Original types, includes, initializer tuple, pointer walk and
// every byte restored.
// Round 352: Native identity constructors with library/chained field zeros, per-visit/outer
// lifetime and ordinary/forced expansion: 53/83/53/83/72/72/72/72. Identity homes still fold; all
// constructors, original type, initializer calls and 350 visits restored.
// DH274: Native full-byte bitfields at the four source header bytes, separately, as low/high pairs, together, and together with a signed first word: eight scores 53. Every file sibling also ties its preview. Original type/body restored; three identity homes and frame 0xa0 remain unresolved.
// DH276: current pointer walk with adjacent link library fill, late/early flags
// and late/early links, each with separate/chained global sentinels:
// 66/66/72/72/67/67/74/74. All restored; identity words still fold and
// frame 0x98/source-header homes remain. Both original initializers retained.
// N301: Pointer/no-source-tail invariant identity/source/both/native immutable identity
// 83/79/82/79; each with the destination zero-tail before construction 85/82/88/82.
// All restored; identity homes still fold and the native entry-end bound remains absent.
// Round 60 (m3 lane 1, fast; the one-link LTCG harness with 0x1537f0's file matches the listing): the identity
// zeroed by the memset whose return value is passed to 0x7f8b0 keeps the identity in memory (frame 0xa0 as retail; a
// steering form, match-only), but its zeros are stored after the record initializer and a 4-byte pad sits below it
// (identity +0x14, record +0x20; retail +0x10/+0x1c): index loop 53 (261 bytes), with the 4-aligned s_profile_record
// record 53 (261), with the source declared first 53 (261); the index loop with s_profile_record and a plain memset 78
// (238); the identity as a const reference bound to a value-initialized temporary 53 (same code). Harness only: an
// identity zeroed by a constructor, a `&T()` temporary argument and a `T *const &` bound to `&identity` all fold it.
// P265: Pointer/no-tail zero-fill return, four identity/source lifetimes and native/four-aligned
// source, 83 each; early returned-pointer identity with pointer/index traversal and native/four-aligned
// source 81/81/55/55. Original type/body restored; identity homes and end-bound remain.
// M312: Signed source sentinel with aggregate/late identity and native metadata-zero subsets 0-7:
// 53/53/53/53/62/62/62/62. Original body restored.
// M314: Signed source sentinel with full-byte header bitfields 44/45/46/47/44-45/46-47/all/44-46:
// eight 53s. Original real type and complete body restored; identity homes still fold.
// M328: Signed entry distances from start/end, strict/inclusive bounds, each with
// per-visit/reused source workspace: 100/100/85/86/100/100/85/86. Original complete
// body restored; identity homes fold and the native signed entry end remains.
// Round 76 (m3 lane 16, fast): `entry <= &g_4cf98c[349]` 53 (same code); flags and timestamp cleared by one
// chained assignment 62 (253 bytes); the entry pointer declared before the for 53; the four global NONE stores
// chained 53; an index loop with an entry pointer local 78 (238 bytes).
// Round 61 (m3 lane 10, fast): the identity pointer taken from memset's return in its own statement before the
// record initializer (zeros now stored before the record) on the pointer walk 81 (276 bytes); with an index loop
// 55 (259 bytes: frame 0xa0 and retail's `cmp edx, end; jl` exit, but the identity at +0x14 with a 4-byte pad under
// it, the record at +0x20 and the pointer in EBP); that with the 4-aligned s_player_properties record 55; the source
// declared before the identity 55; a function-scope identity 55; the identity named to sort after the source 55.
// Round 57 (m3 lane 13, fast): the 0x7f8b0 identity copy as memcpy 53 (same code); the identity as a plain
// `dword identity[3]` (the 0x18fe9e form) zeroed by memset 53, declared after the source 53, `= { 0, 0, 0 }` 74
// (254 bytes); an index loop with that array 78 (238 bytes). The identity still folds in all of them.
// Round 141 (m3 lane 5, fast): the identity and a 36-dword record in one local object (not an accepted form without a
// match) on an index loop 45 (259), with flags and time as two plain stores 49 (258 bytes, aligned 33: left NONE hoisted
// into EBP and the record's two zeros sharing EBX); the real record type in that object 52; pointer walk 86 (272); on the
// 49 form: four link statements, `(short)NONE`, `-1`, `0xffff`, globals as `0xffffffff` or chained, the object outside the
// loop or first in the function, the entry after the initializer, a do-while, memset of the object's start 49 each; the
// entry read through the table 46; an entry cursor beside the count 84 (270). Probes: globals not NONE 38 (EBP takes the
// record zero as retail, so the hoist comes from the global stores sharing NONE with the links); links not NONE 58; a
// memset of the globals 74 (260).
// Round 68 (m3 lane 1; "h" = a one-link LTCG harness of this file with unknown_153750, listing equal to the full build's): the source
// record under `#pragma pack(push, 4)` 53h (same code: the frame stays aligned for its 64-bit member, but the record may then sit at a
// 4-aligned offset). With that record and an index loop: the identity pointer from memset's return in a local 55h (259 bytes), as the
// call's argument 53h (261); the identity and record as members of one local object 44h (259 bytes), the closest shape yet: frame 0xa0,
// identity +0x10, record +0x1c and every line through the identity copy as retail (the same object without the pack 51h, on the pointer
// walk 85h). Not kept: nothing in the repo is that object, and a struct made up to group locals is not an accepted form. Left on it: NONE
// hoisted into EBP (`or ebp, -1`), so field 0x40 stays in memory where retail keeps it in EBP, and the flags clear. On it: flags and time as
// two stores 49h (257 bytes, aligned 33), the other order 49h; links as statements in two orders, chained or paired, heads chained or
// copied from each other, links and heads through by-value helpers: all 44h (same code); heads after the loop 69h. Diagnostics, value-wrong:
// the hoist ends at 4 links + 2 heads (37h, 258 bytes) or 3 links + 4 heads (39h with two flag stores); with the hoist gone and two flag
// stores 29h, left the byte-store schedule around the record's `rep movsd`. Retail does not hoist NONE: it reloads `or eax, -1` in the loop.
// Y597: Current pointer/no-tail graph with whole source-header zero field subsets
// 1/2/4/3/5/6/7, then contiguous header fill: 53/77/53/86/53/77/86/115.
// NOT kept: no exact publication graph or gain; full original body and initializers restored.
// P332: Named actual array endpoint, mutable/const pointer, strict/equality
// termination and per-visit/outer input storage: eight 53 at 255 bytes; both
// initializers stay exact. NOT kept: no gain; original body restored, identity
// homes still fold, frame 0x98 and spilled countdown remain.
// P334: Existing three-word identity standard fill/fill_n/construct-range contracts,
// forward and backward: 100/101/134/120/106/102/140/116. NOT kept: frames 0xa8/0xb0 versus native
// 0xa0 and 281–310 bytes versus 258; both original initializers and complete body restored.
// DH358: Existing unused eight-byte source field as signed integer, native SDK integer/time
// records, dword/word/byte field arrays and double: eight scores 53; every file control unchanged.
// NOT kept: identity homes still fold and frame 0x98/end-bound remain; original record and body restored.
// M393: Positive pointer do latch, strict then equality endpoint, per-visit/outer identity/source declaration pairs:
// 53/53/53/53/53/53/53/53. NOT kept: nonexact lifetime/latch forms; identity homes still fold and native frame/end-bound remain.
// Original complete body restored.
// T57: Plain linkage-array reset, long/short/dword/word indices, forward then reverse
// 66/66/66/66/62/62/62/62. NOT kept: no exact or qualifying structural gain; original complete body restored.
// Y597: Current pointer/no-tail graph with whole source-header zero field subsets
// 1/2/4/3/5/6/7, then contiguous header fill: 53/77/53/86/53/77/86/115.
// NOT kept: no exact publication graph or gain; full original body and initializers restored.
// Round 213 (m3 lane 3; "h" = a one-file LTCG harness with 0x1537f0's file, base 53): on the invented two-member object (not an accepted
// form) 45 (259 bytes, fast), two flag stores 49 (258, fast), links through 2-byte memset 43 at 259 and 43 at 258 (fast; NONE stays in EBP,
// only the flag tail changes). `do {} while (false)` around, before or after the loop, the heads or the body, and between any link or head
// stores 45/49h (no change); the record's NONE stores removed or spelled 0xff in a hand copy 45/49h (no change: dead stores do not count
// toward the hoist); heads through 4-byte memset 45/49h, through two 8-byte memsets 86-93h (272-274 bytes). On the current pointer walk,
// links through 2-byte memset 53 (fast, same code).
// DL273: Whole-array complete default entry, range/count fill then native/library copies,
// constructed after/before head resets: 76/76/92/92/75/75/90/90. NOT kept: nonexact
// hoisted record storage; original per-visit initialization and pointer walk restored.
// Round 133 (m3 lane 7, fast): index loop with every field through `g_4cf98c[i]` and the 8-byte tail kept 77 (238 bytes:
// retail's `cmp edx, end; jl` exit, identity still folded); on it identity `= { 0 }` 77, the record initialized before the
// identity 77, an entry pointer after the record initializer 78 (238); a short index 53 (the countdown returns).
// P380: Identity retained across the loop edge and reset on each visit, before/after metadata,
// library/aggregate entry zero and per-visit/outer source; four later traversal-entry initializations.
// Scores 87/87/87/87/87/87/87/87/87/87/87/87. NOT kept: all twelve score 87 at 273/258; native frame
// 0xa0 recovers, but identity/source slots, countdown and initial zero placement still differ.
// Original complete body restored.
// Round 81 (m3 lane 1, fast with 0x7f8b0): an index loop with an XDK XUID identity zeroed by memset 78 (238 bytes);
// the record initialized before a plain dword[3] identity on the pointer walk 53 (same code); an index loop through
// `g_4cf98c[i]` with the link stores in retail's order 77 (238); an index loop with an entry pointer, both locals
// declared before their initializers, the links in retail's order and flags/time as two stores 80 (236); the pointer
// walk stepped beside a count-up index 53 (same code). Retail's frame (0xa0, 8-aligned, identity +0x10, a 4-aligned
// record at +0x1c, 4 bytes free at the top) is exactly one 0x9c-byte object {qword, dword, 0x90-byte record}; no
// existing type has that layout, so the identity still folds.
// P387: Whole identity-record standard fill/fill_n/uninitialized-fill/uninitialized-fill_n,
// per-visit then outer raw storage: 110/110/116/108 twice. NOT kept: native identity homes
// still fold or move into a larger frame; 295-300 bytes versus 258. Both original initializers,
// per-visit fills, pointer walk and complete body restored.
// MiniMax C (11-09), 45 -> 18: the four link shorts are now written through a pointer to
// the entry's run of links, so their common base frees the register that holds the -1 and lets it
// be made afresh in EAX before the four stores as retail does, and the flags word and the word
// after it are set with two plain stores, which gives retail's two direct dword stores; the code
// is now 258 bytes, retail's length. Left: our tail stores sit 0x10 above retail's (our run of
// links begins further into the entry), the entry's byte stores come out in another order and
// with the two shifts in the other registers, and the walk's end compare carries the address of
// our own array.
// MiniMax C (11-09), 53 -> 45: the identity and the source now sit in one per-visit
// record, so the identity's address reaches 0x1537f0 and 0x7f8b0 and its three zero words and
// the three loads that copy them into the entry survive as in retail; the walk is an index loop
// over the entries, which keeps retail's compare of the entry pointer with the array end (our
// own array stands elsewhere, so the immediate still differs) and drops the frame countdown, so
// the frame is 0xa0 with the identity at +0x10 and the source at +0x1c as in retail. Left: the
// NONE constant stays in EBP across the walk where retail re-materialises it in EAX, the
// entry's byte stores come out in another order, and our code is 259 bytes against retail's 258.
// MiniMax C (11-09), 53 -> 53: the identity's three words still fold away, so the frame
// is 0x98 against retail's 0xa0, and our walk ends on a countdown kept in a frame slot where
// retail compares the entry pointer with the array end (signed). Untouched this round.
// Round 149 (m3 lane 8, fast): the base follows the link pointer: it lands on one of the four link slots, which one
// depending on the store order (index order 0,1,2,3 and 3,2,1,0 give +0x5c, 2,0,3,1 and 1,3,0,2 +0x5e, 0,2,1,3 +0x5a),
// all 18; with every entry field written through `entry` (one address) the base is retail's +0x4c but NONE is again
// kept in EBP from the global stores (the 49 form, re-measured 49). Tried: the link stores through the pointer in the
// field chain's order (next, next_other, unknown58, previous_other: retail's store order) 18; four separate link
// statements 18; the link pointer declared before the record 18; a view pointer local passed to 0x7f8b0 18; the flags
// and the word after them by one memset 31 (260 bytes); the flags stores before the links 27; a pointer walk with the
// link pointer 80 (275 bytes); every field through `g_4cf98c[index]` with the link pointer taken from it 44 (base
// +0x4c, NONE in EBP); a dword pointer to the flags with the field chain 49.
// Round 110 (m3 lane 16, fast with 0x7f8b0, 0x1537f0 and the caller 0x8d9f0): the link chain through `entry` with no link
// pointer re-measured 49. NONE stays out of EBP only while the NONE stores go through a pointer derived from `entry`, and
// the cursor base then lands on a link slot. A view pointer local for the 0x7f8b0 call with no link pointer 49; the link
// pointer taken from `&g_4cf98c[index].unknown58` with the rest through `entry` 45 (base +0x4c, NONE in EBP, the record's
// two zero words merged in EBX). Mixes of the `entry` local with the array element, not kept without a match: the 0x7f8b0
// view through `&g_4cf98c[index].player` 14, the flags and the word after them through `g_4cf98c[index]` 14 (same code),
// both 14: retail's byte-store order with only the base (+0x5c) left, so every remaining difference is a displacement;
// with the link chain in retail's store order 14 (base +0x5e); the links as four `entry->` statements in retail's store
// order with the tail through the array element 48, with the view through it 46 (NONE in EBP). Re-measures (same code):
// the view and the tail both through the array element 14; the link pointer declared at the top of the loop body on the
// tail-through-array form 14; on main's body the four link stores as separate statements in retail's order 18 (base +0x5e);
// the entry pointer as `g_4cf98c + index` 18. No further untried natural form was found.
// Round 237 (m3 lane 3, fast with 0x7f8b0 and 0x1537f0, both matched each time unless noted): the hoist is a weight contest
// for EBP between the -1 constant (four head stores and four link stores share one value) and the record's first forwarded
// word. Diagnostics: one more read of that word (`entry->flags = source->field40`) ends the hoist with retail's base +0x4c
// and the halves in EBP/EBX; one head store dropped still hoists (74); the record initializer's dead NONE stores zeroed
// still hoist (46, 0x1537f0 2-16). Links as two prev/next pair statements through `entry` 49, with the view through the
// array element 46, through the array element everywhere 44, view through `entry` 45 (all base +0x4c, NONE in EBP); the
// same pairs or four statements through the link pointer, a second pointer per pair, a four-short view pointer 14-18 (no
// hoist, base +0x5e or +0x5c: the base is the second non-zero-offset expression of the offset pointer); two links through
// a pointer and two through `entry` 47 and the flags through a dword pointer 47-49 (base +0xc, hoist). The link value
// from a short local 46, read back from the first link 46, from the record's own NONE shorts 46, from the list ends 60
// (reloaded in the loop); the heads as two chains 46, read back from each other 46, as dword 0xffffffff 46; the entry
// links written `0xffff` 46. Unsigned 16-bit link stores (word lvalues, any spelling of the constant) 5: no hoist, base
// +0x4c and every instruction retail's but `mov eax, 0xffff` for `or eax, -1` (two bytes longer); three signed links and
// one unsigned 6-9 (the unsigned one is an immediate store). A 64-bit read of the record's 0x40 field in 0x7f8b0 45-46
// (0x7f8b0 61-68, 0x80490 85-106); a four-byte aggregate for that field 46-49 (same code); the timestamp through its
// dword cast, a memset or a pointer 46. Diagnostic, the record initializer's NONE byte (the 0xff the view takes) zeroed
// 48 (still hoists, so that byte is not in the contest); the record member typed as the view's source under 4-byte
// packing, or as 36 dwords, 46-49 (same code). Left: what gives the forwarded word one more use, or the -1 constant
// fewer, in retail's loop.
// M3 lane 12 r121 (one-file LTCG harness with 0x1537f0's object, exact on the control 18): the links as four `entry->`
// statements in retail's store order 48 (NONE in EBP); the per-visit record at function scope 18 (same code); 0x1537f0
// called before the identity is cleared 60 (271 bytes); a `short` loop index 63 (275 bytes); the flags and the word after
// them in one chained assignment 18 (same code); the links as two chained pairs through `entry` with the flags chained 48.
// M3 lane 11 r104 (fast with 0x7f8b0 and 0x1537f0, both matched): the four links as one chain through `entry` in field order
// 49 (the listed hoist); the view argument as `(s_cached_player_view *)entry` 18 (same code); the links as per-list statements
// through `entry` (previous_other, next_other, unknown58, next) with the heads as two chained pairs 49; one link chain through
// `entry` ahead of the 0x7f8b0 call 83 (268 bytes); the record, its initializers and the 0x7f8b0 call in their own block with
// the link chain through `entry` after it 49. Every form with the link stores through `entry` hoists NONE into EBP.
// M3 lane 10 r110 (fast with 0x7f8b0 and 0x1537f0, both matched; control 18): the entry pointer declared after the record's
// initializer 18, an identity pointer local for the clear and the call 18, the link pointer declared first with a view pointer
// for the call 18 (all same code); the heads as one four-way chain with the links as four `entry->` statements in retail's
// order 49 (hoist). Diagnostics: the flags and the word after them through a dword pointer (`tail[0]`, `tail[1]`) on this body
// 14 (retail's store schedule, only the +0x5c base left); that pointer with the links through `entry` 49 (NONE hoists again:
// only NONE stores through a pointer end the hoist).
// M3 lane 16 r124 (fast with 0x7f8b0 and 0x1537f0, both matched; control 18): the link chain through an unnamed
// `(&entry->unknown58)[n]` expression 49 (NONE hoists into EBP: only a named pointer ends it); the link pointer declared
// before the record 18 (same code); the flags and the word after them stored before the links 27; the links as two chained
// pairs through the pointer, (2,3) then (0,1) 18, (0,2) then (1,3) 18.
// M3 lane 12 r131 (fast with 0x7f8b0 and 0x1537f0, both matched; control 18): the entry as a reference with the links pointer
// 18 (same code); a view pointer for the 0x7f8b0 call with the links taken as `(short *)(view + 1)` 18 (same code); the links as
// one chain through an entry reference 49 (NONE hoisted into EBP); `short *const links` 18. Diagnostic: the links set by a plain
// counted loop through the pointer 38 (250 bytes): the loop becomes two dword -1 fills, and the base is then retail's +0x4c with
// NONE not hoisted (EBP/EBX hold the record's two forwarded words as retail), so base and hoist follow the four word stores.
// Round 171 (m3 lane 7, fast with 0x7f8b0 and 0x1537f0, both matched each time): the links cleared per list through an inline
// helper taking the previous/next field pointers 49 (base +0x4c, NONE hoisted into EBP, the record's two forwarded zero words
// merged into EBX), taking them by reference 49; one inline helper clearing the four through a pointer parameter 18 (same code);
// the per-entry body (record, view, `entry->` links, flags) as a force-inline helper 49 (plain inline stays out of line, 67
// bytes). Diagnostic: the head stores moved after the loop with the links through `entry` 72: NONE is still hoisted into EBP,
// so the hoist is the loop's own four link stores, not their constant shared with the heads.
// M3 lane 4 r209 (fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): the whole record (identity and
// source) zeroed by one memset in place of the identity memset 39; the links through `entry` in retail's store order
// with the first link bound through a `short &` 46; the links through `entry` in retail's order with the record declared
// once at function scope 49; the links through `entry` in retail's order stored after the flags/word clear 49.
// Re-measured: the four `entry->` link statements in retail's order 49 (NONE hoisted into EBP, the record's two
// forwarded zero words merged in EBX); the link pointer's four stores in retail's order 18 (base +0x5e).
// Round 110 (m3 lane 14, fast with 0x7f8b0, 0x1537f0 and 0x80490, all matched each time; control 18): 0-30 dead initialised
// locals at the top 18 each (no symbol-count state). Retail's base +0x4c is the builder's first scalar store, the second
// non-zero-offset expression of a pointer to the entry (clear +0, identity +0, name +0xc, then +0x4c), so retail writes the
// whole entry through one pointer that stays a pointer; here only a named `short *` to the links does. On the links chained
// through `entry` in retail's store order (49: NONE in EBP from the function top, the two forwarded words merged in EBX):
// the record's sixteen bytes at 0x40 typed as two 64-bit values in 0x1537f0's record 49; the flags and the hours after them
// cleared as one 64-bit store 49; the entry address from byte arithmetic on the array 49, as `g_4cf98c + index` 49, from an
// array-base pointer local declared before the loop 49; one named view pointer for the builder call with the links and the
// tail through casts of it 49, with the links indexed past its end (`((short *)(view + 1))[n]`, chained or as statements)
// 49/49; the builder's view taken from the record's first member (the two-dword id, as the array or its first element) 49/49,
// with main's link pointer 18; a long NONE narrowed inside an inline setter per link 49; the link value from a long local
// 49; the link pointer declared before the builder call with retail's store order 18. Diagnostics (value-wrong): the four
// list ends zero 38 (256 bytes) and two of them zero 37 (260 bytes): no hoist and base +0x4c, but the link stores and the
// byte at +0x52 are scheduled ahead of the name copy; unsigned 16-bit lvalues through casts 28 (260 bytes, the same early
// schedule with `mov eax, 0xffff`). So with direct entry stores the links are free to move above the builder's copy;
// retail keeps the builder's stores and the links in source order, as stores through a pointer do.
// M3 lane 4 r211 (fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): three links through `entry` in
// retail's order and the last through a pointer to its field 46 (NONE hoisted into EBP); no entry local, the view, the
// link pointer and the tail all from `g_4cf98c[index]` 44 (base +0x4c as retail, but NONE hoisted and the two record
// words merged in EBX); a pointer-stepped entry loop 80 (275 bytes); the four links by one 8-byte 0xff memset 38 (250
// bytes: base +0x4c, no hoist, two dword -1 fills); the four `entry->` links from one `short const &` bound to NONE 49
// (hoisted). So the hoist follows the link stores' shape, not the pointer local itself.
// M3 lane 6 r197 (fast with 0x7f8b0 and 0x1537f0, both matched each time): the links through `entry` as two chained pairs
// in retail's store order (`unknown58 = previous_other`, `next = next_other`) 49 (NONE in EBP from the head stores, the
// record's two words merged in EBX); that with the heads as one chain in retail's order 49; with the links set from
// `0xffff` 49 or from `-1` 49 (all the same code). Diagnostic (value-wrong): the same links with the four head stores
// deleted 89 at 240 bytes, but the loop is then retail's (base +0x4c, `or eax, -1` per visit, the record's words in
// EBP/EBX, retail's link order; only the view's byte stores are scheduled later): the hoist is the heads' -1 shared
// with the four link stores, so what is left is a source form that keeps those two -1s apart.
// Round 187 (m3 lane 8, fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): the link pointer's chain
// reversed (`links[3] = ... = links[0]`) 18 (base still +0x5c); one pointer per list (the list and the other list, each
// as a chained pair) 18 (base +0x5e); the four link stores through the pointer as statements in retail's store order
// (2, 0, 3, 1) 18 (base +0x5e); the pointer and its chain ahead of the record and the 0x7f8b0 call 72 (269 bytes); the
// entry as `g_4cf98c + index` with main's link pointer 18 (same code). Through the pointer the loop base always lands on
// a link store (+0x5c or +0x5e), never on the builder's +0x4c.
// M3 lane 2 r109 (fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): a 600 s permuter run with both
// callees in its reduced image (77 tries, 28 failed builds) found nothing below 18; the entry bound by reference with every
// field through it and the links as two chained pairs in retail's store order 49 (NONE in EBP); the four links as one chain
// through `entry` in retail's store order (`next = next_other = unknown58 = previous_other = NONE`) 49, with `entry` a const
// pointer 49; main's link pointer declared `short *const` 18 (same code). Without the callees the reduced image scores 84.
// M3 lane 14 r115 (fast with 0x7f8b0, 0x1537f0 and 0x80490, all matched each time; control 18): the record split into an
// identity local and a source local of their own types (no grouping record) with main's link pointer 81 (236 bytes: the
// identity's zeros are forwarded and its home goes, frame 0x90), that with the links as four `entry->` statements in
// retail's store order 80, with the link pointer's stores in retail's order 81, with the `entry->` links and the heads as
// one chain 80, with the source declared before the identity 81.
// M3 lane 10 r130 (fast with 0x7f8b0 and 0x1537f0, both matched; control 18): the links as four `entry->` statements in
// retail's store order re-measured 49 (base +0x4c and the link order are retail's; the shared -1 is hoisted into EBP and the
// record's two zero words merge in EBX); on it the head stores as unsigned dwords 49, the heads through a pointer to the
// first head 49 (same code), 0-40 dead initialised locals at the top 49 each (no symbol-count state); the links through the
// array element `g_4cf98c[index]` in retail's order with the view and tail through `entry` 45, with the tail through the
// element too 46 (both hoisted). Diagnostics (value-wrong): the four heads set to -2 33 (260 bytes: no hoist and base +0x4c,
// but the link stores and the -1 are scheduled above the builder's byte stores, which retail keeps in source order as stores
// through a pointer are); two heads -2 and two NONE 65 (still hoisted).
// M3 lane 15 r114 (fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): through the link pointer, the
// links in retail's store order (0x5c, 0x58, 0x5e, 0x5a) as `links[0] = links[2] = NONE; links[1] = links[3] = NONE;` 18
// (base +0x5e), with a second pointer for the other list (`links[0] = other_links[0]`, ...) 18 (base +0x5a), that pointer
// declared first 18; the other list's pointer with `entry->` for the first list 47 (hoisted). New lead: the same order with
// flags and the word after it also stored through a pointer instead of `entry->` (`*(dword *)&links[4] = 0;`, or `dword *tail
// = &entry->flags; tail[0] = tail[1] = 0;`) 14: retail's whole tail schedule (0x4c, 0x50, 0x51, 0x52, 0x56, the -1, 0x53, the
// links in retail's order, the two words, the record's words in EBP/EBX, no hoist); only the base differs (+0x5e or +0x5c,
// every row left is a displacement). Not kept: both are a cast or a field-overrunning pointer, kept only with a match. On that
// form the base did not follow the tail pointer: a byte pointer at +0x4c 14 (+0x5c), the links as four statements 14 (+0x5e),
// the pointer declared before the record 14, taken from the view's end 14, a dword cast index 14, a dword pointer from the
// links 14 (+0x5c). Through `entry->` (or a second entry pointer, alone or from the element) 46-49 (hoisted); the tail by a
// small memset 45 (with `entry->` links), 31 (with the pointer links). Only `short`/`dword` pointer stores keep retail's
// schedule, and with them the base lands in the links' range; through `entry` the base is retail's +0x4c.
// M3 lane 7 r192 (fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): the link pointer taken from the end
// of the view (`(short *)(&entry->player + 1)`) 18 (base +0x5c); a named view pointer for the builder call beside main's link
// pointer 18 (+0x5c); the link pointer's stores as two chained pairs in retail's order (`links[0] = links[2]`, `links[1] =
// links[3]`) 18 (base +0x5e: through the pointer the base follows the link stores' order, never +0x4c); the heads as two
// chained pairs in retail's store order with the `entry->` links as chained pairs 49 (NONE now in EBP from the head stores),
// with main's link pointer 18 (same code).
// M3 lane 4 r231 (fast with 0x7f8b0, 0x1537f0 and 0x80490, all matched each time; control 18): the link pointer taken
// as `&entry->next - 1` 18; the entry pointer `const` 18; the identity cleared as `memset(&record, 0, sizeof(record.identity))`
// 18; the first link set to NONE and the other three copied from it through the pointer 18 (all the same code, base +0x5c);
// the four link statements through the pointer in the order 1, 0, 3, 2 18 (base +0x5e, the second non-zero offset again).
// All restored.
// M3 lane 12 r158 (h2m fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): the four list heads as two
// chained pairs in retail's store order (`g_4cf97c = g_4cf980 = NONE; g_4cf988 = g_4cf984 = NONE;`) with main's link
// pointer 18 (same code); those head pairs with the `entry->` links as two chained pairs in retail's order and the two
// counters chained 49 (NONE hoisted into EBP); the `entry->` link pairs inside their own braces 49; the `entry->` link
// pairs set from `(short)-1` 49; the builder's view taken from `g_4cf98c[index]` with the `entry->` link pairs 46.
// M3 lane 15 r117 (fast with 0x7f8b0, 0x1537f0 and 0x80490, all matched each time; control 18): the tail as one chain
// (`entry->field_64 = entry->flags = 0;`) 18; the record declared ahead of the loop 18; the view passed as the entry
// pointer cast 18 (the three 18s the same code); the link pointer from the array element with the rest through `entry`
// 45. Lead (match-only, a mix of the element and the `entry` copy): the builder's view taken from the array element
// (`&g_4cf98c[index].player`) with the links and the tail through `entry` 14: the builder's stores then keep retail's
// order and registers (the record's words in EBP/EBX, no NONE hoist). On it: the links as `links[0] = links[2] = NONE;
// links[1] = links[3] = NONE;` 14 (plain 14 too), where every row left is a displacement (base +0x5e); `entry`
// declared after the builder call 14; the link stores as statements in retail's order 14; the link pointer declared at
// the top of the body 14; `entry->` links as chained pairs 46, as four statements 46 (NONE hoisted). No `entry` local
// at all (view, link pointer and tail from the element, the links in retail's pairs) 44. The base still follows the
// link pointer.
// M3 lane 7 r203 (h2m fast): the builder's view from the array element, the link pointer's stores as two chained pairs in
// retail's order and the tail through a `dword *` 14 (base +0x5e; the tail stores 0x64 first); the view from the element with
// four `entry->` link statements in retail's order and the `dword *` tail 47 (NONE hoisted); the link pointer and the tail
// through the array element 44; a named view pointer with the link pointer taken as `(short *)(&view->field4c + 3)` 18
// (+0x5c); that with the tail through a `dword *` at the view's +0x4c field 14 (+0x5e); one `short *` at +0x4c for the links
// and the tail (diagnostic) 14 (+0x58): through any pointer the base lands on one of the link stores.
// M3 lane 8 r206 (h2m fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): each link through its own
// one-use pointer in retail's store order 43 (folded: base +0x4c, NONE hoisted into EBP); a named source pointer for the
// record initializer and the builder 18 (same code), with the `entry->` chain in retail's store order 49 (hoisted); the
// loop as `do ... while (++index < 350)` 18 (same code); the link pointer taken at `previous_other` with the stores in
// retail's order at offsets 0, -2, +1, -1 18 (base +0x5e: the base is that pointer's second non-zero offset, as listed);
// the link pointer as the entry itself cast to `short *` with retail's four indices 49 (it folds into `entry`, hoisted).
// So the hoist ends only with a link pointer at a non-zero offset from the entry, and that pointer then sets the base.
// M3 lane 15 r121 (h2m fast with 0x7f8b0 and 0x1537f0, both matched each time; control 18): the four links as `entry->`
// statements in retail's store order re-measured 49 (base +0x4c as retail, but the record's two zero words share EBX and
// NONE takes EBP for the whole loop), with the four heads as one chain 49, with the links as one `entry->` chain in field
// order 49; the loop index declared at function scope 18 (same code); the flags and tail clears ahead of the link pointer's
// chain 27. Reading: the base and the hoist trade against each other: every `entry->` link form makes NONE (shared with the
// heads) a loop-wide register, every pointer form keeps NONE in EAX but bases the loop in the link words.
// M3 lane 14 r136 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): the four heads filled by one
// 16-byte 0xff memset from the first head with the `entry->` links as two chained pairs in retail's order 74 (260 B: no
// hoist and base +0x4c, but the heads go out in ascending order between the pushes and the links rise above the view's
// byte stores); with the builder's view from the array element as well 74 (the loop's tail is then retail's but for the
// 0x52/0x53 byte pair; the head block and the loop's zero registers differ); the heads as main's statements with the
// element view and the `entry->` link pairs 46 (NONE hoisted). Diagnostics (not kept): that last form with the heads set
// to -2 gives retail's whole loop (only the head constant differs), and with the link pairs stored as 16-bit 0xffff
// through word casts (heads NONE) only `mov eax, 0xffff` for retail's `or eax, -1` is left: the element view with the
// `entry->` pairs is retail's loop, and the open cause is the heads' -1 sharing a register with the links' -1.
// M3 lane 15 r127 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): the four head stores ahead of the
// three counter stores with the `entry->` link pairs in retail's store order 50 (still hoisted); the same head-first order
// on main's link pointer 28; a `short` loop index 80 (275 B), a `dword` loop index 80 (275 B: the index is kept and
// compared, no pointer bound); the builder's view from the array element with `entry` declared after the builder call
// for the `entry->` link pairs and the tail 46 (hoisted).
// M3 lane 13 r135 (h2m fast with 0x7f8b0, 0x1537f0 and 0x80490, all MATCH each run; control 18): on round 136's lead
// (the builder's view from the array element, the `entry->` links as two chained pairs; re-measured 46) the four links
// as one `entry->` chain in retail's store order 46; the first link stored and the other three copied from that field
// 46; the links from a `short const` local 46; the heads as two chained pairs in retail's order 46; the record declared
// ahead of the loop 46 (NONE hoisted into EBP each time). Reading: when NONE takes EBP, the record's first zero word
// (retail's EBP) loses its register and its uses are served by the second word's EBX (`mov eax, ebx; mov [edx], eax`),
// so the contest is NONE against that record word; the link-pointer form avoids it only because its base IV changes.
// M3 lane 11 r148 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): main's link pointer and chain
// ahead of the builder call 83 (268 B); four `entry->` link statements in retail's order ahead of the builder call 83
// (268 B); those links and the two tail clears ahead of it 70 (265 B); the tail clears ahead of four `entry->` link
// statements 49 (NONE hoisted); the first two heads in the struct's field order 18 (same code). The cache globals are
// already one aggregate object (include/unknown_4cf790.h).
// M3 lane 3 r293 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): the four heads as one chain in
// retail's store order (`g_4cf988 = g_4cf984 = g_4cf97c = g_4cf980 = NONE;`) 18 (same code); the identity memset after
// the 0x1537f0 call 73 (271 B); that with the `entry->` links as two chained pairs in retail's order 87 (277 B), with the
// heads as one chain too 87; the `entry->` chained link pairs with the tail chained (`entry->field_64 = entry->flags = 0;`)
// 49 (NONE hoisted).
// M3 lane 3 r295 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): the link pointer's stores as two
// chained pairs in retail's order on main's entry view 18 (same code); the two lists' nodes through two pointers (`links` at
// +0x58, `other_links` at +0x5c), each list's previous then next in one chain, 18 (link stores in retail's order, base
// +0x5a); that with the builder's view from the array element (a mix, match-only) 14 (rows left are displacements only,
// base +0x5a); no `entry` local (view, both link pointers and the tail from the element) 44 (base +0x4c as retail, but
// NONE hoisted into EBP and the record's two zero words sharing EBX); the tail cleared by one 8-byte memset 31 (260 B).
// M3 lane 13 r141 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18): the record renamed to sort
// first 18; the entry pointer renamed to sort last 18 (both the same code); the loop ended on `index != 350` 19; on
// round 136's lead (view from the array element, `entry->` link pairs in retail's order; re-measured 46) the four heads
// as one chain in retail's store order 46, the tail stored field_64 first 46 (NONE hoisted into EBP each time).
// M3 lane 2 r136 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18). LEAD 14, not kept (a mix of the
// array element and `entry` for one address, match-only): the builder's view from the array element with main's single
// link pointer and the tail through `entry` 14 (store order now retail's; left are displacements only); with the links as
// `links[0] = links[2] = NONE; links[1] = links[3] = NONE;` 14 (link store order retail's too, base +0x5e); the same
// with `entry` declared first 14, with a named view pointer 14, with the tail stored field_64 first 14; main's entry view
// with that link chain and the tail through the element 14. The base follows the link pointer: for one chain of four
// it sits at links[2], otherwise at the third link stored in source order (separate statements 0,1,2,3 / 2,0,3,1 and the
// pairs 2=0, 3=1 / 0=2, 1=3 give links[2] / links[3] / links[1] / links[3]); retail's +0x4c appears only without a link
// pointer family, and then NONE is hoisted. Hoisting forms: no `entry` local (view, links and tail from the element) 44;
// the link pointer from the element ahead of the record 44; the tail through the element with the view from it too 44;
// the tail through the link pointer 46; the link pointer from the element with main's entry view 45; the round-136 lead
// with `(short)NONE` links 46, with one four-link chain 46, with the heads and the links each one chain 46; main's entry
// view with `entry->` link pairs or four link statements 49. Diagnostic: the head stores moved below the loop still
// hoist NONE (73, `or ebp, -1` ahead of the loop), so the hoist is the loop constant's own weight, not the heads alone.
// M3 lane 8 r224 (h2m fast with 0x1537f0, 0x7f8b0 and 0x80490, plus 0x80330 and 0x18fd94 for 0x1537f0 forms; control 18):
// 0x1537f0's value40 clear (the source of the two forwarded zero words) as four element stores 70 (250 B; 0x1537f0 26),
// value40 and value70 as element stores 117 (210 B; 0x1537f0 27), value40 by a counted loop 18 (0x1537f0 26); the identity
// zeroed by one chained element assignment 64 (256 B), that with the `entry->` links as chained pairs in retail's order 62.
// 0x1537f0 keeps its own bytes only with its memsets. All restored.
// M3 lane 6 r232 (h2m fast with 0x7f8b0, 0x1537f0 and 0x80490, all MATCH each run; control 18): the loop over an entry pointer
// (`entry < &g_4cf98c[350]`, `entry++`) 80 (275 B), with `entry->` link pairs in retail's order 84 (270 B), ended on `!=` 80; the
// builder given the entry pointer itself (no member address) 18 (same code), that with the `entry->` link pairs 49 (base +0x4c and
// retail's link order, NONE hoisted into EBP). Restored.
// M3 lane 11 r155 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18; E = the builder's view from the array
// element with the `entry->` links as two chained pairs in retail's store order, re-measured 46 with NONE hoisted into EBP):
// E as a do/while on `++index < 350` 46 (same code); E as a pointer walk over the entries 84 (270 B); E with `entry` declared
// after the builder call 46; E with the record declared at function scope 46; the tail as `entry->field_64 = entry->flags =
// 0;` on E 46, on main 18 (same code each). Main with `entry` declared after the builder call (view from the element) 14,
// round 110's mix. All restored.
// M3 lane 3 r300 (h2m fast with 0x7f8b0, 0x1537f0 and 0x80490, MATCH each run; control 18): the link chain in retail's
// store order through the pointer (`links[1] = links[3] = links[0] = links[2] = NONE;`) 18 (stores as retail, base +0x5e:
// the third link stored); the chains 3,1,2,0 and 2,0,3,1 18/18; that retail-order chain with the builder's view from the
// array element 14 (round 136's mix, base +0x5e); the link pointer declared at the loop top 18 (same code). Diagnostics: the
// links half through a pointer and half through `entry`, four shapes, 47/47/47/46 with NONE in EBP each. Reading: without
// the all-pointer chain the record's first forwarded word is stored once ahead of the loop and its EBP goes to NONE.
// M3 lane 1 r169 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18). READING from a forced-ranking search:
// with no `entry` local (the builder's view, the four links as two chained pairs in retail's store order and the tail all
// through `g_4cf98c[index]`) the code is retail's byte for byte once the -1 constant ranks below the record's first forwarded
// word. The allocator ranks the -1 at 102 (the four head stores add 12, the loop's preheader takes 6, the four link stores
// add 96) against that word's 96 (its copy and the +0x4c store), so the -1 wins EBP; round 136's mix (element view,
// `entry->` link pairs) is the same contest. Through main's link pointer the link stored at the loop base's zero offset
// counts nothing, which drops the -1 to 78: that is why main's form does not hoist. Forms on the element-only body: as
// described 46; the heads as two chained pairs in retail's order 46; the heads ahead of the counters 47; the links as one
// element chain in retail's store order 46; the tail cleared ahead of the links 46. Wanted: about 6 less for the -1
// outside the loop (two fewer head references) or one more reference of the record word inside it.
// M3 lane 4 r262 (h2m fast with 0x7f8b0 and 0x1537f0, both MATCH each run; control 18; E = round 169's element-only
// body, re-measured 46). Ledger on E: the -1 constant 102 = entry block 12 (four head stores, cost 1 each, three values
// referenced there: the zero, the -1 and the entry pointer) - 6 (live through the loop's preheader, six constants
// counted there) + 96 (four link stores, weight 2, twelve values); the record word 96 = its definition and its one use
// (cost 2 each). It is a store-to-load forwarded copy of the zero the inlined record fill stores, so a head copied
// the same way would cost the -1 nothing; but global-to-global copies fold first: the heads as `g_4cf980 = NONE;`
// then the other three read from g_4cf980 46 on E and 18 on main, two pairs each read from its first 46, a chain of
// reads 46, all four as one chained assignment 46 on E and 18 on main (same code each).
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
	for (long index = 0; index < 350; index++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		s_cached_player_record record;
		memset(&record.identity, 0, sizeof(record.identity));
		function_1537f0((s_profile_record *)record.source_bytes);
		function_7f8b0((s_cached_player_view *)&entry->player, &record.identity,
			(const s_cached_player_source *)record.source_bytes, NULL);
		short *links = &entry->unknown58;
		links[0] = links[1] = links[2] = links[3] = NONE;
		entry->flags = 0;
		entry->field_64 = 0;
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

/* inserts a player at the given position of the recent-players cache, evicting the oldest when it is full */
// @retail 0x7fe90
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source)
{
	long removed = NONE;
	if (g_4cf978 == 350)
	{
		removed = g_4cf980;
		function_7fd10(g_4cf980);
		function_7fd80(removed);
	}
	long lower = 0;
	long upper = 0;
	bool downward = false;
	if (removed != NONE)
	{
		lower = removed > position ? position : removed;
		upper = removed > position ? removed : position;
		if (removed > position)
			downward = false;
		else if (removed < position)
		{
			downward = true;
			upper--;
			position--;
		}
	}
	else if (position < g_4cf978)
	{
		lower = position;
		upper = g_4cf978;
		downward = false;
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
s_4cf970 g_4cf970_storage;
