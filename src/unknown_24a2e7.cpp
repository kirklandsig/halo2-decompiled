#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "data_array.h"
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_2b116a.h"
#include "loop_allocator.h"
#include <wchar.h>
#include <string.h>

// @flags /O1 /Oi /Gr

struct s_entry_24a64d
{
	long index;
	byte field_04[8];
	word field_0c[32];
	char field_4c[16];
	byte field_5c[0x11c - 0x5c];
	byte field_11c[0x1e4 - 0x11c];
	word field_1e4[50];
};

short player_slot_count_active(void);
bool function_148c3e(long controller, long type);

// @retail 0x24a2e7
bool c_list_24a64d::function_24a2e7(long controller)
{
	long type = 0;
	if (!field_89)
		type = player_slot_count_active() ? 2 : 1;
	return function_148c3e(controller, type);
}

// @retail 0x24a64d
void *c_list_24a64d::function_24a64d(long index)
{
	long count;
	s_entry_24a64d *entries = (s_entry_24a64d *)get_items(&count);
	void *result = 0;
	for (long i = 0; i < count; ++i)
	{
		if (entries[i].index == index)
		{
			result = (byte *)&entries[i] + 0x11c;
			break;
		}
	}
	return result;
}

struct s_datum_24a763
{
	long field_00;
	long index;
};

// @retail 0x24a763
s_entry_24a64d *function_24a763(c_list_24a64d *list)
{
	s_entry_24a64d *result = 0;
	s_datum_24a763 *datum = (s_datum_24a763 *)record_pool_lookup(list->data, list->get_focused_datum());
	if (datum && datum->index != NONE)
	{
		long count;
		s_entry_24a64d *entries = (s_entry_24a64d *)list->get_items(&count);
		for (long i = 0; i < count; ++i)
		{
			if (entries[i].index == datum->index)
			{
				result = &entries[i];
				break;
			}
		}
	}
	return result;
}

class c_screen_24a7bf : public c_screen_with_menu
{
public:
	virtual void v19();
	virtual void v3();
	virtual void v25(s_screen_focus *arg_0);
	s_entry_24a64d *function_24a828();
	bool function_24a855();
	c_list_24a64d field_614;
	byte field_6a0[0x176c - 0x614 - sizeof(c_list_24a64d)];
	s_entry_24a64d field_176c;
	bool field_19b4;
};

bool network_session_manager_session_unready(void);
void function_253c3a(long block_index, long index, c_text_widget_45a5e0 *widget);

// @retail 0x24a7bf
void c_screen_24a7bf::v19()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 1, true);
	long index;
	if (network_session_manager_session_unready())
		index = 1;
	else
		index = 0;
	function_253c3a(0, index, text);
	c_class_1473c9::v19();
}

// @retail 0x24a855
bool c_screen_24a7bf::function_24a855()
{
	s_datum_24a763 *datum = (s_datum_24a763 *)record_pool_lookup(field_614.data, field_614.get_focused_datum());
	if (datum)
		return field_19b4 = datum->index == NONE;
	return field_19b4;
}

// @retail 0x24a828
s_entry_24a64d *c_screen_24a7bf::function_24a828()
{
	s_entry_24a64d *result = function_24a763(&field_614);
	if (result)
		field_176c = *result;
	else
		result = &field_176c;
	return result;
}

void unicode_string_copy(word *destination, word const *source, long maximum_count);
void ascii_string_to_unicode(char const *source, word *destination, long maximum_count);

bool function_24aae8();
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

// @retail 0x24a09b
void c_list_24a64d::v20(c_class_1a2c81 *item, long)
{
	long datum_index = ((c_class_14750b *)item)->value70;
	if (datum_index != NONE)
	{
		c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_child(6, 0, false);
		s_datum_24a763 *datum = &((s_datum_24a763 *)data->data)[datum_index & 0xffff];
		if (text)
		{
			s_widget_item layout;
			if (datum->index == NONE)
			{
				layout.value5e = true;
				layout.flags = 0x20;
				text->function_253b1a(0x12000195);
				text->value6e = true;
			}
			else
			{
				void const *appearance = function_24a64d(datum->index);
				layout.value4 = (long)function_24a691(datum->index, function_24aae8());
				layout.flags |= 1;
				if (appearance)
				{
					memcpy(layout.value48, appearance, sizeof(layout.value48));
					layout.flags |= 2;
				}
				text->value6e = false;
			}
			function_22f042(&layout, item, 1);
		}
	}
}

// score 67 (was 67; m3 lane 5 r233 unchanged, its lines just above the marker; m3 lane 8 r221 unchanged): Retail keeps the entry base in the 0x28 frame and retains the unscaled ordinal through the appends.
// Earlier (67, was 67): Retail spills the entry-array base and recomputes the indexed
// terminator after the string calls.
// independent read cursor and indexed terminator 84; cursor search condition with
// indexed result after loop 59 (NOT kept: extra count test and no retail base spill);
// cursor search saves the found index 72; plain-name result assigned before the
// suffix guard 66 (NOT kept: gain below eight); opaque entry base with typed indexing
// at each use 84; found entry reference with indexed terminator 90; appended result
// from the last concatenation 60 (NOT kept: form requires a byte match); read-only
// cursor for all source fields 79; writable cursor with separate indexed terminator
// 84; writable while-loop cursor with indexed terminator 83; cursor with plain-name
// guard before the suffix 84; opaque base and writable cursor with indexed terminator
// 84.
// Field-derived string capacity 67, direct indexed destinations 84, wide empty
// suffix initialization 67, first-word clear with remaining zero fill 67,
// suffix initialization in the append branch 76, completed index search 80,
// plain-name arm first 67, guarded entry range with recovered index 89,
// conversion-terminated suffix 104. The entry-range form reaches frame 0x28
// but spills the result rather than the entry base and grows to 224 bytes.
// Indexed terminator with direct exits 83; plain exit first with indexed
// formatted result 83; separate search cursor with direct exits 83;
// skip unmatched entries before the direct exits 83; plain direct exit
// and shared fallback 83; formatted direct exit and shared fallback 77;
// indexed terminator with final concatenation result 85; shared plain-name
// input with direct exits 98. The entry base still has no stack slot.
// Indexed search with selected pointer/index after loop 80/72. Remaining-count
// pointer scan 114. Nested suffix guard 66. Optional suffix pointer 84.
// Writable result with fallback at return 76. Append-parts loop 70.
// Append return as next destination 83. Named suffix with plain-name arm 90.
// Nested guard with plain-name copy input 66, also with last append result 66.
// Early append result with named suffix 90. NOT kept: nested guards reserve
// 0x28 and gain one. The slot holds the result. The entry base stays in a register.
// DH401: One return with the empty name assigned on exhaustion. Indexed for/while
// and cursor for/while, formatted arm first 83/83/83/83,
// plain arm first 83/83/83/83. The EBP-8 base spill is absent.
// Round 624: writable result with indexed/direct terminator 84/67; const result through copy
// and appends with indexed terminator 84; publish at first append and terminate by index 70; publish before
// indexed terminator 84; wide-character suffix with original/indexed/result output 67/84/84. All hold
// the entry base in a register and reserve 0x24.
// Comparison-bound search with a shared exhaustion return 56; with an explicit cursor 56;
// guarded end pointer with live ordinal 74; counted comparison before formatting 59;
// skip mismatches with indexed terminator 84; formatted arm breaks 84; fixed-array
// destination 84; selected name then result 84. NOT kept: the 56 forms recover length
// but retain 46 differing instructions, no base spill and no final ordinal multiply.
// Postincrement comparison cursor with indexed strings/visited destination/visited
// inputs/visited strings: 84/84/69/75. Selected const entry with independent search
// cursor, then wide destination/field-end clear/array destination: 89/89/89/89.
// The visited-input form recovers frame 0x28 at 204 bytes but spills the cursor
// and keeps the derived output entry instead of reloading the array base.
// M3 lane 10 r119 (fast; plain where noted). Reading: retail's entry base sits in memory at [ebp-8] next to the count
// at [ebp-4] and is reloaded only after the library concatenations, so it behaves as an address-taken local.
// The indexed terminator alone 84; entries and count as one two-member local aggregate (count's address passed),
// everything indexed, 64 (plain 64; frame 0x28, suffix, base spill and terminator as retail; the base is reloaded
// after the ascii call where retail keeps ESI); on that, an entry pointer at the found point 72 (multiply ahead of
// the append test), an entry pointer inside the append arm 52, with the ascii argument indexed 39 (204 B), the same
// with `if (append) { entry; if (entry->name[0]) {...} }` nested 39 (natural spelling, same code); on the 39 form the
// terminator as `(entries + i)->`, entry as `entries + i` or as a byte offset `i * sizeof`, i declared outside the
// for, or the terminator after `result = buffer` 39 each; a found flag in the loop condition 75/70 (flag kept, no
// threading); search loop then `if (i < count)` 66 (210 B, extra count test); a cursor pointer loop 83 (i in EBX as
// retail, but the found block keeps the cursor in ESI); a per-iteration entry pointer 75/83; the 39 form without the
// aggregate 84. What is left on 39: ours keeps i * 0x248 in EBX across the calls (i itself in ECX in the loop), retail
// keeps i in EBX and multiplies again for the terminator and the plain arm. The aggregate is a grouping form
// (match-only); build/r119/lead-0x24a691-39.cpp.
// M3 lane 15 r101 (fast; main 67, aligned 23): a per-iteration `entry = &entries[i]` at the loop top used for the name
// test, ascii argument, buffer and copy source, with `entries[i].field_1e4[49] = 0` and the plain arm `entries[i].field_0c`
// indexed, 75 (196 B, aligned 8): frame 0x28, the entries spill at [ebp-8], i in EBX and the terminator and plain-arm code are
// retail's with no aggregate; left: our loop keeps the per-iteration pointer in ESI (pushed ahead of the loop) where retail
// runs a scratch cursor in ECX and forms the entry again from i after the append test. On it: the loop test indexed the same
// code; 32 subsets of the uses through the pointer best 75 (the name test through it is what keeps the terminator apart);
// the pointer at function scope 75; the found block after the loop behind a found flag 75 (threaded, same code), behind `i <
// count` 69 (201 B, aligned 5, the extra compare stays), `i != count` 76; the plain arm through the pointer 83; with the
// two-member aggregate 75. Other forms: the indexed terminator alone (re-measured) 84; the entries pointer written through
// its own address 84 (folded); an inline (entries, index) accessor for the arm's entry 84 nested / 90 combined (the multiply
// is still shared); entry pointer at the found block's top with the aggregate 72 (192 B, aligned 6: loop as retail, but the
// entry is formed ahead of the append test and shared by the terminator and plain arm), with plain locals 90. Body unchanged.
// The 0x28 frame, EBP-8 base spill and indexed clear return with the item pointer/count
// record. Common plain-name fallthrough 52, selected suffix 39; mismatch continue 52/39;
// direct found exits 71; explicit cursor with indexed fallback 52, while cursor 52,
// cursor and selected suffix 39; shared suffix source 39, final append's result 73.
// NOT kept: the grouping form requires a match. At 39, ECX carries the ordinal in the
// search, then EBX carries its scaled offset through the appends.
// Remaining count with a separate ordinal 80, with indexed clear 78, with
// a selected entry 95, and with both 69. Wide result with direct/indexed
// clear 67/84. Shared entry lookup 131. Clamped signed range 74.
// The countdown and clamped range can reserve 0x28 but the added slot
// still does not carry the entry base across the string calls.
// Indexed/cached library terminator 68/74, whole conversion buffer clear with indexed/cached store 94/95,
// wide destination and indexed/cached library terminator 68/74, library clear for both areas with
// indexed/cached terminator 79/92. With the indexed clear, form the destination before conversion 67
// or publish the result before the clear 80. NOT kept: the closest 208-byte forms still use frame 0x24
// and keep the list base in a register instead of reloading it after concatenation.
// M3 lane 16 r141 (fast with 0x24a09b): a loop-top entry pointer used for the search test only, the found uses and the
// terminator indexed 84 (197 B: the found address and the terminator share EBX); that with the plain arm through the
// pointer 90 (184 B); the pointer for the field_4c uses only 75 (196 B); for every append-arm use with the test indexed
// 75; for the test and the append arm 75 (196 B: frame, base spill, index in EBX, terminator and plain arm as retail;
// the pointer stays the induction register in ESI where retail runs a scratch cursor and re-forms the entry from the
// index), with the plain arm through it 83; the search loop then the found block under `if (i < count)` 69 (201 B: the
// cursor is a scratch ECX as retail but copied to ESI each pass, and the count test is extra); a test-in-condition
// search loop 87 (187 B).
// M3 lane 13 r114 (fast; aligned/masked [28/11] on this body). Lane 10 r119's two-member aggregate re-measured 39
// [17/3]; on it a plain `if (append && entries[i].field_4c[0]) { entry ... } else ...; break;` 39 (same code), every
// access indexed 64, the terminator through `buffer` 65 or `entry` 54, the ascii argument indexed 52, no `buffer`
// local 109 (256 B); `entries + i`, i declared outside the for, a writable result used as the buffer, the terminator
// after `result = buffer`, the plain arm first, unsigned byte-offset products for the entry or the terminator,
// `(dword)i` in the terminator: 39 each (the product i*0x248 stays shared with the terminator, so i never needs EBX
// across the calls); the permuter on it reached 32 differing instructions from 34 before it was stopped. A loop-top
// entry pointer (`s_entry_24a64d *entry = &entries[i]; if (entry->index == index)`) with the indexed terminator and
// no aggregate 75 [8/4] (196 B): frame 0x28, the base spill, i in EBX and the re-formed terminator are retail's;
// left is ours keeping the loop cursor live in ESI where retail re-forms the found entry from i (`imul`). On it:
// with the aggregate 75, the pointer declared outside the loop 75, a `continue` form 75, a found flag 75, the found
// path indexed 84, the field test indexed 73, the ascii argument indexed 71, both 74, the plain arm through `entry`
// 83, a second pointer in the arm 52, the terminator through `entry` 91. The search loop with the loop-top pointer
// and then `if (i < count)` 69 [5/5] (the cursor is copied to ESI each iteration and the count test is not
// threaded); an indexed search then `if (i < count)` with the entry formed after it 87 (product shared again); a
// found flag 90; `entry = NULL` then `if (entry)` 88. Body unchanged.
// The base still needs EBP-8 and the successful ordinal needs to survive the appends.
// Entry base named early/late, ordinal early/late, display result early, formatted
// destination late, and adjacent base/count ranked early/late all score 67.
// NOT kept: lexical order leaves the frame at 0x24 and the derived buffer live.
// Bound destination array with direct/indexed clear 67/84. Bound name array 88. Bound
// suffix array and derived conversion limit 84. Destination extent for indexed clear 84.
// Scan entry for suffix test 77, also for both input strings 67. Separate scan cursor 84.
// NOT kept: the 205-byte form recovers frame 0x28 and ties but spills the moving cursor
// at EBP-8, leaving the base in a register.
// Round 436. Indexed returned array after/before cached clear 67/68; indexed
// return and clear 84; destination array then indexed return 67; scan cursor
// with indexed return 67 and cursor inputs with indexed clear/return 79.
// Plain result as copy input 66; selected input reference with indexed
// clear/return 90. NOT kept: 66 gains one and spills the result; the other
// 0x28 frame form spills the cursor. Original body restored.
// M3 lane 4 r219 (fast with 0x24a7bf; control 67). Reading: everything but the found entry's lifetime is retail's in
// some form; retail forms the entry from i into ESI before the ascii call, keeps it across that call, drops it when ESI
// takes the 0x31 count for the copy, and forms it again from i and the spilled base for the terminator, so the entry is
// never shared with the terminator. Indexed terminator with the buffer local 84; the entries base passed by reference or
// by pointer to a forceinline getter 84/84, `(*&entries)[i]` for the terminator 84, a `s_entry_24a64d *&` alias for the
// base 84 (all fold: the base is promoted again); the found entry as byte arithmetic in the condition, in the arm, or
// nested 84/83/84; a loop-top entry used only in the found block 75 (the loop cursor becomes the entry in ESI);
// loop-top entry for the test with indexed found reads 84; loop-top entry for the test and the found reads 75;
// the base and count as a two-member local (re-measure of the grouping form) 64; the search loop then `if (i < count)`
// with the indexed terminator 59 (positional: the extra count test lines the found block up, the entry still stays in
// EBX; NOT kept: a mix of the buffer local and the indexed field), with every buffer use indexed 59, with the buffer
// terminator 80, `i != count` 59; the found block after a writable name local assigned to the result first 80;
// `result = entries[i].field_0c;` before the append test 66; the loop-top entry used only for the terminator 84, and
// for the terminator and the plain arm 96.
// M3 lane 12 r148 (fast with 0x24a64d/0x24a763/0x24a52b, all matched; main 67), on lane 10 r119's 39 form (re-measured 39:
// it differs from retail only in that the found index's product i*0x248 is shared by the entry pointer and the terminator,
// so EBX holds the product and i lives in ECX, where retail keeps i in EBX and multiplies again after the library calls):
// a `use_suffix` flag set under the append test and tested after it 68 (211 B: threaded on the append path, product still
// shared); the terminator's offset written size-first as a byte offset 39 (same code: the product is canonicalised); the
// terminator indexed through `long const &found = i;` 39 (same code); the buffer named before the suffix conversion with the
// suffix declared in the arm 44. Diagnostic (behaviour change, reverted): the append arm continuing the loop 59 (211 B).
// Body unchanged.
// M3 lane 4 r226 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf; control 67). Reading: with the terminator indexed
// (re-measured 84) ours keeps the whole entry address in EBX across the five calls, so neither i nor the base is needed
// after them; retail keeps i in EBX, spills the base and forms the address again, sharing neither the address nor the
// product. The base written by a forceinline getter through an out-pointer, indexed terminator 84 (resolved, no spill);
// that with the arm's entry pointer for the buffer and source 84; the suffix declared at the found block's top and
// cleared by memset 94 (187 B); the found body as a forceinline (entries, i, append) name helper 84; a found flag set
// before the break with the body after the loop (threaded) 84. Body unchanged.
// M3 lane 1 r129 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all matched throughout; main 67; body
// unchanged). Reading: retail's found entry in ESI serves the name test and the two copy arguments, dies when ESI takes
// the copy's count, and the terminator and the plain arm each multiply i again (three `imul`), with i in EBX from the
// loop's start; here any found-block entry spelled from i is shared with the terminator (address or product), so i
// never has to outlive the calls. Indexed terminator with the entry pointer in the arm (re-measure) 84; a found flag
// second in the loop condition (`i < count && !found`, flag set at the found block's top, no break) 72 (216 B: the
// loop keeps its latch around the found block); the flag first in the condition and set last 79 (200 B, frame 0x28);
// a NULL result in the loop condition with the empty name assigned after the loop 71 (207 B, frame 0x28, i in memory);
// returns straight from both arms and the empty name returned after the loop 83 (193 B); a `built` flag set at the
// arm's end with the plain name under `if (!built)` 84 (same code as the if/else: threaded before sharing), on the
// two-member lead 39 (same code). Diagnostic (behaviour change, reverted): the two-member lead's append arm continuing
// the loop 59 (211 B, re-measure: the calls then sit inside the loop and i goes to memory). Lane 10 r119's two-member
// lead re-measured 39 (204 B, aligned 16/17: everything but the shared product).
// Opening append returns result/destination with cached or indexed clear
// 77/66, 72/75. Suffix append returns result/destination with cached or indexed
// clear 83/73, 67/80. NOT kept: 66 regains the 0x28 frame and reaches 206 bytes
// but spills the result and gains one point. The base and ordinal remain missing.
// Search selects the display form before formatting. The conditional, branched, plain default and combined plain
// guard forms each reach 84 for the indexed scan and 77 for a separate cursor. NOT kept: the cursor keeps the
// ordinal in EBX during the scan but scales it before the calls. The 0x24 frame still lacks the base spill.
// M3 lane 15 r113 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b and the two string callees, all MATCH; main
// 67): lane 10 r119's two-member lead rebuilt 39 (204 B); on it the result taken as the indexed buffer after the
// terminator 51 (218 B: the product still shared, now holding the result too), the plain arm written as an else of both
// tests 78 (201 B). Round 101's loop-top entry form rebuilt 75 (196 B, aligned 9, register-masked 5); on it a second
// pointer to the found entry for the append arm 90 (184 B: folded back onto the loop pointer), the append test nested
// with the name test inside 75 (same code), the result taken as the indexed buffer after the terminator 71 (206 B).
// Single exit at a match, mismatch continue, formatted exit, early plain exit or plain break
// 67/67/67/67/67. Early plain exit with indexed clear 84, with advancing comparison cursor 67.
// Entry range with indexed clear and shared exit 64. NOT kept: the range groups base and
// count and gains three points at 207 bytes.
// On the per-iteration entry and indexed clear form, a copied bound 75,
// scoped out-count 75, native wide suffix 75, array extent for all limits 75,
// end-pointer bound 73, shared plain/copy name 93, count and suffix names 75,
// and result before indexed clear 75. The end bound restores the 0x28 frame
// and base spill at 213 bytes but holds the search pointer in ESI across the loop.
// M3 lane 16 r153 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; main 67, aligned 26). Reading:
// with the terminator through `buffer`, i is dead before the calls, so it stays in ECX and EBX becomes a zero register
// (`cmp byte [ebp+0xc], bl`, `mov word [edi+0x62], bx`); any indexed terminator shares `entries + i*0x248` (or the
// product) with the found entry across the calls, where retail keeps i in EBX and forms the address again. New: `result`
// assigned before an indexed terminator 84 (197 B, the entry address in EBX); i declared at function scope ahead of
// count 67 (same code); `result` used as the copy and append destination with an indexed terminator, no buffer local
// 84; the copy ahead of the suffix conversion 75 (201 B); the suffix typed as a wide-character array 67 (same code).
// Carry opening/suffix, opening/closing, suffix/closing or all append results:
// cached terminator 69/78/83/83, indexed terminator 79/73/81/80.
// NOT kept: the 0x24 frame persists and all scores rise. The entry base stack
// home and unscaled ordinal through the appends still differ.
// M3 lane 11 r134 (fast with 0x24a64d, 0x24a763, 0x24a52b, 0x24a7bf and 0x24a09b, all MATCH; control 67): the suffix
// declared at function scope 66 (195 B); `long count = 0;` 83 (192 B); the loop index as `short` 46 (202 B, aligned 20:
// i then stays in EBX and the found entry and plain arm are formed again from it as retail, but every use of i gains a
// `movsx`, which retail never has, so retail's index is 32-bit; diagnostic only); the entries pointer `const` 67 (same
// code); `buffer` named ahead of the suffix conversion 76 (196 B).
// Immutable suffix text with cached/indexed clear 95/90, named display choice 95/90, and plain-name
// break 95/90. Selecting an empty suffix when append is off gives 59/63; append-scoped suffix 67/84.
// NOT kept: the 59 form adds an empty-string selection absent in retail, keeps the 0x24 frame
// and omits the entry-base spill and the final ordinal multiply.
// Round 452: Scan produces the successful ordinal or NONE, tested for equality
// or signed validity 72/73, or the count, tested for equality or the bound 72/72.
// The same four forms with a separate comparison cursor also give 72/73/72/72.
// The terminator uses a direct zero and the body reaches 205–206 bytes, but the
// 0x24 frame still lacks the base spill and EBX carries the scaled entry address.
// M3 lane 7 r201 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; main 67). Lane 10 r119's two-member
// lead rebuilt 39 (204 B). The same code without the grouping: plain `count`/`entries` locals with the lead's body and an
// unreachable call taking the entries pointer's address (diagnostic) 39, so the lead needs only an entries pointer that
// calls may change, not the pair. On the lead: the arm's entry or the terminator indexed through a copy of the index read
// by const reference 39/39 (folded); a store of the index after the terminator (diagnostic) 85 (220 B: the shared product
// keeps EBX and the index goes to a stack word written on every pass, so a later plain use of the index does not
// unshare the product); every access indexed with no entry or buffer local 118 (255 B: the base is reloaded after each
// call). New reading: retail's three multiplies are what the compiler gives when the terminator is not dominated by the
// arm's entry. With plain locals, a `built` flag set at the end of the arm and, after the arm, one pointer to the found
// entry used by `if (built) terminator; else plain name;` 59 (207 B, first difference +0x20: the prologue, the entries
// spill at [ebp-8], the index in EBX, the scratch cursor in ECX and the loaded id in EDX are all retail's, and the
// terminator and the plain name are formed again from the index; left: the flag stays in CL with its test, and the
// pointer's multiply sits once ahead of that test where retail has it in each arm), the same on the two-member lead 59;
// without that pointer (`if (built) indexed terminator; else indexed plain name;`) 84 with plain locals and 39 on the
// lead (the flag is threaded before the sharing, so the arm's entry dominates again); the flag set ahead of the calls
// 84; the arm's condition named once and tested twice (build, then terminator or plain name) 83, with the result chosen
// by a conditional after the arm 84 (both threaded early); the built name in a pointer cleared at the top and tested
// after the arm 80 (216 B, the pointer test stays); the plain name assigned first and replaced in the arm 92 (182 B);
// the arm as a forced-inline helper returning whether it built (the result through a reference) 90 (184 B), returning
// the built name or null 79 (195 B) (plain `inline` leaves either out of line: 179/186); a `short` index with the
// indexed terminator 50 (205 B, each use sign-extends); the arm as a forced-inline helper taking the entries and the
// index (the entry formed inside, after the append test) and returning whether it built, with the indexed terminator
// and plain name in the caller's if/else, 84, the same with one result variable in the helper 84 (the returned flag
// is threaded before the sharing too); `register` on the index, on the lead, 39 (same code). Body unchanged.
// Guarded last ordinal with indexed/selected-entry formatting 73/75;
// guarded exact end ordinal with indexed/selected-entry formatting 67/38;
// guarded last address 79/79 and guarded exact end address 74/74.
// NOT kept: the 38 form restores the base spill and final ordinal multiply
// but adds a second count test, uses inequality at the latch and retains
// the scan cursor in ESI where retail reforms the entry after the append test.
// Separate nonpositive-count return with selected entry and inclusive/exact end
// 73/70, both 211 bytes. The base slot returns, but the save order differs and
// the exact end still adds a count test.
// Ordinal from cursor difference with selected end 84/75/93/80, or positive-count
// guard 82/74/95/76, indexing all found accesses, formatted destination, plain name
// and clear, or post-concatenation clear. NOT kept: all add an integer divide, and
// the extra frame word in the 0x28 forms does not preserve the unscaled scan ordinal.
// M3 lane 11 r142 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67, aligned 30, masked 19): the index
// typed `int` 67 (same code); the tag field named once (`char const *tag = entries[i].field_4c;`, tested and passed) 95
// (180 B), with the indexed terminator 90 (184 B); a mismatch `continue` with the found body at loop level 67 (same code);
// the found test as `index == entries[i].index` 67 (masked 21). Body unchanged.
// M3 lane 16 r165 (fast with 0x24a64d, 0x24a763, 0x24a52b, 0x24a7bf, 0x24a09b, all MATCH; control 67; "aligned" = rows left
// after aligning the listings with addresses masked). Re-measured round 101's form: plain locals, a per-iteration entry
// pointer (`entry = &entries[i]` at the loop top) used by the compare and the arm, with the terminator and the plain name
// indexed in an if/else: 75 (196 B, aligned 8): the terminator (`imul ebx` after the calls, the base reloaded from [ebp-8]), the plain name, the
// index in EBX and the base spill are then all retail's; left is the arm, where ours keeps the pointer as the loop cursor
// in ESI and retail recomputes `&entries[i]` from the index at the found block. The same with the search loop first and
// `if (i < count)` after it 69 (201 B, aligned 5; retail has no count test after the loop), with the arm indexed too 59
// (201 B, the arm's address shared with the terminator again). Lane 10 r119's two-member aggregate lead re-measured 39;
// on it: the terminator through `i * sizeof` byte arithmetic 39 (same code: the product is still shared); the plain name
// assigned before the append test 47 (190 B, aligned 7: the product is hoisted ahead of the append test); the plain name
// in both else arms 78 (aligned 7, hoisted the same way); the found index copied into a second local 39; a pointer cursor
// advanced with the index 83 (aligned 9: terminator and plain name exact, the arm uses the cursor); the per-iteration
// pointer 75; the found pointer preset NULL and tested after the loop 60; the arm's pointer named in the found block 72
// (aligned 6: one product hoisted and shared by all three uses where retail forms it three times).
// Separate copy or whole format scopes with an entry pointer/reference and indexed
// clear 83/83/83/83. Nested suffix with a comparison cursor, advancing on inequality
// or after the match arm, ordinal or cursor first, 84/84/84/84. The frame remains 0x24.
// The scaled entry remains live across the appends.
// M3 lane 5 r226 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67). Reading: retail's
// arm forms `&entries[i]` once into ESI through a scratch product (`mov ecx, ebx; imul ecx, ecx, 0x248`), keeps the
// index in EBX across the five calls and multiplies it again for the terminator and for the plain name; with the
// entries pointer in memory ours puts the arm's product in EBX and reuses it for the terminator, so the index dies at
// the arm. On a two-member view of the entries and the count (lane 10 r119's lead, re-measured): every access indexed
// 64 (207 B: frame 0x28 and the base spill, the product in EBX from the arm, the base read again after the first
// call), with the terminator through `buffer` 65 (195 B); every access through a member accessor taking the index by
// value or by const reference, or a forceinline free accessor on the view, 64 each (same code as indexed); the arm
// nested (`if (append) { entry = &entries[i]; if (entry->field_4c[0]) ...`) returning the buffer, terminator and
// plain name indexed, 39 (206 B: left are the product's register, EBX for retail's ECX, with the index in ECX for
// EBX, and the result in ESI for EDI), the same on plain locals 77 (199 B); the terminator's index, or both later
// indices, cast to unsigned 39/39 (same code; 77 on plain locals); the nested arm with the terminator through
// `buffer` 80 (193 B).
// M3 lane 2 r128 (H2M_FAST with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67): lane 10 r119's two-member
// lead rebuilt 39 (204 B; left: ours pushes ESI ahead of the loop for the loaded id, keeps the index in ECX and its product
// in EBX across the calls, where retail keeps the index itself in EBX from the start and pushes ESI in the found block).
// On it: a `short` index 70 (214 B); the result re-indexed (`result = entries[i].field_1e4`) 51 (218 B); the tag named
// once as a `char const *` for the test and the ascii call 39 (same code); the terminator after `result = buffer` 39
// (same code); the found test written `index == entries[i].index` 39 (same code). Body unchanged.
// The frame is still 0x24 and retains the scaled entry. Scope the native name and suffix
// arrays after conversion, at the copy, in separate blocks, or through concatenation,
// with an indexed clear 84/84/84/84. With grouped list state 64/64/64/64.
// NOT kept: grouping recovers the base spill and gains three points, but the ordinal
// stays scaled across the appends.
// M3 lane 12 r170 (h2m fast with 0x24a64d matched; control 67; E0 = the terminator indexed as `entries[i].field_1e4[49] = 0`
// on this body, 84 at 197 B: the whole entry address is then kept in EBX across the calls, where retail keeps the index in
// EBX, spills the entries pointer and multiplies again): E0 with the built name read as `entries[i].field_1e4` at every use
// 84, with the index declared ahead of the loop 84 (same code each); `buffer` declared ahead of the suffix conversion 69
// (203 B, two `lea edi` copies); `result = buffer;` ahead of the indexed terminator 84; the terminator after the if/else
// under `if (result == entries[i].field_1e4)` (diagnostic) 73 (209 B). Body unchanged.
// M3 lane 2 r133 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67): every arm access indexed
// with no buffer local and the terminator and result indexed 84 (197 B); the buffer local kept for the calls with the
// terminator and the result indexed 84 (same code); direct returns (`return buffer;`, `return entries[i].field_0c;`,
// `return L"";` after the loop, terminator indexed) 83 (193 B), the same with every arm access indexed 83; the suffix
// declared at function scope and cleared by memset in the arm 95 (182 B). Compile-only (no LTCG) gives a different
// frame and convention here, so it cannot screen this function. Body unchanged.
// M3 lane 12 r174 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67): `result` declared ahead of
// `count` 69; `long i = 0;` declared ahead of the items call with `for (; i < count; ++i)` 65 (191 B: i takes EDI, zeroed
// before the call); the found test as `if (... != index) continue;` with the body and `break` after it 67 and the if/else
// arms swapped 67 (same code each); the indexed terminator with that continue form 84, with i declared first 80, that with
// the result indexed 80. Body unchanged.
// M3 lane 8 r221 (h2m fast with 0x24a64d, MATCH each time; control 67): a redundant `if (0 < count)` guard ahead of the
// search loop 66 (189 B; one point, under the bar), with the terminator indexed 83 (198 B); the search as a guarded
// do-while 67 (same code), with the indexed terminator 84 (same code as the indexed for); lane 10 r119's two-member
// lead rebuilt 39 (204 B), with the guard 39 (same code). Body unchanged.
// M3 lane 5 r233 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67, aligned 24/26).
// Reading: retail's base spill at [ebp-8] does not need an escaping pointer: the suffix clear (`xor eax, eax; rep
// stosd`) and the calls destroy EAX, so the base is stored once and reloaded for the terminator; what has to be
// explained is only that the found entry's address (ESI, lost when ESI takes the 0x31 argument) is formed again from
// the index, which keeps the index in EBX across the calls and leaves no register for a zero. Tried: the found entry's
// name built by a forced-inline helper taking the list and the index by value, everything inside indexed, 84 (197 B,
// the indexed-terminator code: the entry address is kept in EBX), with direct returns in the caller 86; the search
// with its exhaustion exit inside the loop (`for (i = 0; ; ++i) { if (i >= count) return result; if (match) break; }`)
// and the found entry handled after it, indexed 87, through a per-iteration entry pointer 77 (195 B, aligned 13: the
// cursor is kept in ESI); round 101's search-then-`if (i < count)` form re-measured 69 (201 B, aligned 7: the cursor
// copied to ESI at the break and the extra count test), the same with the count copied into a plain local for the loop
// and the test 69 (same code); a found pointer cleared before the loop and tested after it 88; lane 10 r119's
// two-member lead re-measured 39 (204 B: index ECX, cursor EDX, loaded id ESI, product EBX against retail's index EBX,
// cursor ECX, id EDX, product ECX). Body unchanged.
// M3 lane 2 r138 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67, aligned 26). On the
// two-member lead only one choice is left: ours keeps the product `i * 0x248` in EBX across the string calls (EBX pushed
// after the loop), retail keeps i in EBX and multiplies again (ESI pushed after the loop). Tried to keep the three index
// expressions apart: a file-local inline getter taking the entries and the position at the arm, the terminator and the
// plain arm 84 on main's locals (197 B) and 39 on the lead (the getter's copies fold); on the lead `buffer` named before
// the ascii conversion 44, `result = buffer;` ahead of the indexed terminator 39, the terminator through the arm's
// entry pointer 54 (199 B). Body unchanged.
// M3 lane 11 r156 (H2M fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67, aligned 26): entries typed
// as the header's profile list entry (index, name, tag bytes and display buffer as its own fields) 67 (same code); the search
// as a forced-inline index finder returning the index or NONE, the found body under `if (i != NONE)` 84 (191 B); `entries`
// and `count` declared first with the items call as its own statement 69; the loop test as `count > i` 67 (aligned 36,
// other code); the items result kept as `void *items`, the arm's entry formed as `items + i` with the terminator indexed
// 83 (198 B). Body unchanged.
// M3 lane 11 r159 (H2M fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67). The file is /O1. Re-measured
// the indexed terminator 84 (the arm's entry address kept in EBX across the calls) and lane 10 r119's two-member lead 39
// (retail's code but for one choice: our product `i * 0x248` is kept in EBX from the arm to the terminator with `i` in
// ECX for the loop, where retail keeps `i` in EBX from the start and multiplies again). On main: the indexed terminator
// through `s_entry_24a64d *const &kept = entries` 84, with the plain arm through it too 84 (the reference folds). On the
// lead: the terminator's index through `long const &found = i` 39; the arm's entry as a reference 39 (same code each);
// the entry pointer formed ahead of the append test and used by the plain arm 72. Body unchanged.
// M3 lane 12 r190 (h2m fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67). Reading: on
// main's body EBX is a zero constant (`cmp b [ebp + 0xc], bl`, `mov w [edi + 0x62], bx`) because i dies at the arm; with
// the indexed terminator (84) the entry address shared by the arm and the terminator ranks 84 against i's 38, and a
// forced-ranking search that drops it to the bottom gives 62 with the terminator folded onto `buffer` again (no other
// single decision moves it). On lane 10 r119's two-member lead (39) the shared product `i * 0x248` ranks 81 against i's
// 39, but i forced to the top stays at 39 and the product forced to the bottom gives 69: retail's three multiplies are
// not a colouring outcome, the product is never shared in retail's code. Forms: `buffer` named at the found block's top
// ahead of the append test 94; the built result read back as `entries[i].field_1e4` after the buffer terminator 67
// (same code); the lead's nested append test with the built arm breaking and the plain name after it, on plain locals,
// 67 (same code); the conversion limit and terminator index written from the arrays' extents 67 (same code); i declared
// ahead of the for with `result = buffer;` before an indexed terminator 84. Body unchanged.
// M3 lane GA r10 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a09b, all MATCH; control 67 at 189/210 B, frame 0x24; 12 source
// trials + 4 flag diagnostics; body unchanged). Flag diagnostics on the indexed-terminator body: /O1 /Gr 84 (breaks 0x24a09b
// and 0x24a52b), /O1 /Ob1 /Oi /Gr 84, /O1 /Oi /arch:SSE /Gr 84, /Os without /Og 220 (332 B, breaks all peers), confirming
// /O1 /Oi /Gr. Source trials with indexed terminator `entries[i].field_1e4[49] = 0`: plain early-break before append 84 (197 B);
// append direct-return + plain break 77 (199 B); plain direct-return + append break 83 (193 B); `buffer` bound after copy 84
// or after first wcsncat 84; non-forceinline template string wrapper on field_1e4 86 (189 B, out-of-line method calls);
// `result = buffer` ahead of copy 84; nested `if (append)` with `entry` + indexed terminator 83 (190 B); `entry` bound after
// ascii call 84; NULL result with ternary fallback at return 76 (201 B); loop-top `entry` used only for copy/wcsncat 84;
// `long count = 0` + indexed terminator 78 (198 B). Every indexed-terminator form still CSEs `&entries[i]` into EBX across
// the five calls with frame 0x24 and no [ebp-8] base spill.
// M3 lane GA r19 (10 fast trials with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67; body unchanged):
// forceinline template `s_static_wchar_24a691<50>` wrapper 99 (182 B); nested `if (append)` with duplicated `else` + indexed
// terminator on plain locals 83 (190 B); swapped if/else + indexed terminator 84 (197 B); `while (i < count)` match-break-inc
// + indexed terminator 84 (197 B); byte-offset terminator on plain locals 84; third `wcsncat` indexed 84; `void *` raw items +
// indexed terminator 84; `entries = 0;` init + indexed terminator 84; `wcsncat` return used as `buffer` + indexed terminator 75
// (199 B); forceinline formatting helper followed by indexed terminator 83 (198 B). All stay at frame 0x24 and CSE `&entries[i]`.
// M3 lane 8 r241 (H2M_FAST with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67). Lane 10 r119's
// two-member lead re-measured 39 (204 B). Reading from the register allocator on it: the product `i * 0x248` is one
// candidate shared by the arm and the terminator (ranked 81, the arm and the build blocks) and takes EBX, i (39) takes
// ECX; retail's three separate multiplies mean its code never had that shared value, so no colouring change can give
// retail's i in EBX. Forms on the lead: the plain arm first as an early `break` 72 (192 B); the buffer formed indexed 49
// (208 B); the copy source indexed 50 (208 B); only the name test through the entry pointer 64 (207 B); the plain result
// stored ahead of the append test 47 (190 B); the suffix declared ahead of the name test 63 (209 B). Body unchanged.
// M3 lane 14 r161 (H2M_FAST with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67): the tag test written
// `!= 0` 67; the suffix initialiser written with a wide-character zero 67 (both the same code); the indexed terminator
// with `buffer` a `word *const` 84 (197 B); the terminator through `(entries + i)->` 84 (the indexed-terminator code).
// M3 lane 16 r196 (fast with 0x24a64d/0x24a763/0x24a52b/0x24a7bf, all MATCH; control 67). Reading: the found
// block's entry address and the terminator's are one shared value in every plain-locals form (retail forms it three
// times, with the base reloaded from its home after the calls), so `i` dies at the arm and takes ECX. Forms: `i`
// declared ahead of the items call with the terminator indexed 80 (197 B); round 101's per-iteration entry pointer with
// the terminator and plain name indexed re-measured 75 (196 B), with `i` declared ahead 77; the plain name stored
// ahead of the append test (the arm overwriting it) with the indexed terminator 66 (191 B; a redundant store, one
// point), on the per-iteration pointer form 97 (183 B). Body unchanged.
// M3 lane 7 r239 (H2M_FAST with 0x24a64d/0x24a763/0x24a52b/0x24a7bf/0x24a09b, all MATCH; control 67). A per-iteration
// `entry = &entries[i]` at the loop top with the loop test indexed, the name test, ascii argument, buffer and copy
// source through it and the terminator and plain name indexed 75 (196 B, aligned 9: the terminator and plain name are
// retail's; ours keeps the merged per-iteration pointer in ESI past the loop where retail runs a scratch cursor in ECX
// and forms the entry again from i after the append test); the loop test through it too 75 (same code); the terminator
// through `buffer` 97 (178 B); the plain name through it 83 (189 B); on plain locals the entry formed inside `if
// (append)` with a nested name test and the built arm ending in `break` 84 (197 B). A forced-ranking search on the 75
// form finds nothing lower (the pointer's register is the loop optimiser's choice). Body unchanged.
// @retail 0x24a691
word const *c_list_24a64d::function_24a691(long index, bool append)
{
	long count;
	s_entry_24a64d *entries = (s_entry_24a64d *)get_items(&count);
	word const *result = (word const *)L"";
	for (long i = 0; i < count; ++i)
	{
		if (entries[i].index == index)
		{
			if (append && entries[i].field_4c[0])
			{
				word suffix[16] = { 0 };
				ascii_string_to_unicode(entries[i].field_4c, suffix, 15);
				word *buffer = entries[i].field_1e4;
				unicode_string_copy(buffer, entries[i].field_0c, 0x31);
				wcsncat((wchar_t *)buffer, L" (", 0x31);
				wcsncat((wchar_t *)buffer, (wchar_t *)suffix, 0x31);
				wcsncat((wchar_t *)buffer, L")", 0x31);
				buffer[49] = 0;
				result = buffer;
			}
			else
				result = entries[i].field_0c;
			break;
		}
	}
	return result;
}

void function_1630b0(long *values, long value);
bool player_profile_load(long file_index, s_player_profile_settings *settings);
word *function_215b50(long file_index, word *name);

// @retail 0x24a52b
void c_list_24a64d::v24()
{
	long count;
	s_entry_24a64d *entries = (s_entry_24a64d *)get_items(&count);
	long capacity = count;
	long indices[16];
	function_1630b0(indices, NONE);
	long i = 0;
	for (c_class_1a2c81 *item = child; item && i < 16; item = item->next, ++i)
	{
		long datum_index = ((c_class_14750b *)item)->value70;
		if (datum_index != NONE)
		{
			s_datum_24a763 *datum = (s_datum_24a763 *)record_pool_lookup(data, datum_index);
			if (datum)
				indices[i] = datum->index;
		}
	}
	for (i = 0; i < count; ++i)
	{
		long index = entries[i].index;
		long j;
		for (j = 0; j < 16; ++j)
		{
			if (index == indices[j] && index != g_54e5d0.profile_index)
			{
				indices[j] = NONE;
				break;
			}
		}
		if (j == 16 && index != NONE)
			entries[i].index = NONE;
	}
	for (i = 0; i < 16; ++i)
	{
		long index = indices[i];
		if (index != NONE)
		{
			long j;
			for (j = 0; j < capacity; ++j)
				if (entries[j].index == NONE)
					break;
			if (j >= 0 && j < capacity)
			{
				s_entry_24a64d *entry = entries + j;
				entry->index = index;
				s_player_profile_settings *settings = (s_player_profile_settings *)((byte *)entry + 4);
				if (index == g_54e5d0.profile_index)
					*settings = g_54e5d0.settings;
				else if (!player_profile_load(index, settings))
				{
					memset(settings, 0, sizeof(*settings));
					function_215b50(entry->index, entry->field_0c);
				}
			}
		}
	}
}

// @retail 0x249f0d
c_list_24a64d::c_list_24a64d(word arg_0) :
	c_list_widget_with_items(arg_0),
	field_88(0),
	field_89(false),
	field_8a(false),
	field_8c(NONE),
	field_90(this, (list_item_method)&c_list_24a64d::function_24a30e)
{
	data = user_interface_data_new("player profile list", 0x1001, 8);
	function_16b790(data);
	delegate_register(&item_handlers, &field_90);
}

void function_24c166(c_class_1474e8 *arg_0);
void __stdcall function_215900(long arg_0, long arg_1, word *arg_2, long *arg_3, bool arg_4);
bool player_slot_profile_in_use(long arg_0);

// @retail 0x24a190
void function_24a190(void *arg_0)
{
	c_list_24a64d *local_0 = (c_list_24a64d *)arg_0;
	record_pool_release_all(local_0->data);
	function_24c166(local_0);
	bool local_1 = !local_0->field_89;
	word local_2 = 0x1000;
	long local_3[0x1000];
	long local_4 = 0;
	long local_5 = 0;
	c_class_14750b *local_6 = (c_class_14750b *)local_0->get_item_data();
	long local_7 = local_0->get_item_count();
	function_215900(local_0->get_controller_index(), 0, &local_2, local_3, local_1);
	if (!network_session_manager_session_unready())
	{
		long local_8 = record_pool_allocate(local_0->data);
		((s_datum_24a763 *)local_0->data->data)[local_8 & 0xffff].index = NONE;
		if (local_7 > 0)
			local_0->add_child(&local_6[0]);
		local_4 = 1;
	}
	for (; local_5 < local_2; ++local_5)
	{
		if (local_0->field_89 || !player_slot_profile_in_use(local_3[local_5]))
		{
			long local_8 = record_pool_allocate(local_0->data);
			((s_datum_24a763 *)local_0->data->data)[local_8 & 0xffff].index = local_3[local_5];
			if (local_4 < local_7)
				local_0->add_child(&local_6[local_4]);
			++local_4;
		}
	}
	{
		long local_11;
		s_entry_24a64d *local_9 = (s_entry_24a64d *)local_0->get_items(&local_11);
		for (long local_10 = 0; local_10 < local_11; ++local_10)
		{
			local_9[local_10].index = NONE;
			memset((byte *)&local_9[local_10] + 4, 0, 0x1e0);
		}
	}
	function_24c0c4((c_widget *)local_0);
}

extern byte g_54eae8[4][0xc70];
extern long g_470a60;
bool function_8d7c0();
void function_120df0(long arg_0, wchar_t const *arg_1);
bool __stdcall function_24ab0c(long arg_0, XONLINE_USER *arg_1, char const *arg_2);
bool online_user_requires_passcode(XONLINE_USER const *arg_0);
c_class_1473c9 *__stdcall function_2baa9b(s_screen_parameters *arg_0);
c_class_1473c9 *__stdcall function_2ba45b(s_screen_parameters *arg_0);
c_class_1473c9 *__stdcall function_24b4a9(s_screen_parameters *arg_0);
bool __stdcall function_24aad1(long arg_0);

// @retail 0x24a30e
void c_list_24a64d::function_24a30e(s_controller_reference **arg_0, long *arg_1)
{
	s_datum_24a763 *local_0 = (s_datum_24a763 *)record_pool_lookup(data, *arg_1);
	if (local_0)
	{
		if (local_0->index == NONE)
		{
			if (function_24a2e7((*arg_0)->controller_index) && !field_89)
				function_14800c(v11(), v12());
		}
		else
		{
			s_player_profile_settings local_1;
			if (player_profile_load(local_0->index, &local_1))
			{
				if (field_89)
				{
					s_screen_parameters local_2;
					local_2.field_c = 0;
					profile_edit_begin((*arg_0)->controller_index, &local_1, local_0->index);
					function_149f49((s_message *)&local_2, 0, 0, 1 << (*arg_0)->controller_index, 3, 4, (long)function_2baa9b);
					c_class_1473c9 *local_3 = local_2.load(&local_2);
					if (local_3)
					{
						*(long *)((byte *)local_3 + 0x614) = 0;
						*(long *)((byte *)local_3 + 0x6a0) = 0;
					}
				}
				else
				{
					s_player_slot_profile *local_4 = (s_player_slot_profile *)g_54eae8[(*arg_0)->controller_index];
					local_4->initialize((*arg_0)->controller_index);
					local_4->set_profile_index(local_0->index);
					function_120df0((*arg_0)->controller_index, (wchar_t const *)local_1.name);
					if (field_88)
					{
						if (function_8d7c0())
						{
							XONLINE_USER local_5 = {0};
							if (function_24ab0c((*arg_0)->controller_index, &local_5, (char const *)&local_1 + 0x48))
							{
								local_4->user = local_5;
							}
							else
							{
								s_screen_parameters local_6;
								local_6.field_c = 0;
								function_149f49((s_message *)&local_6, 6, 0, 1 << (*arg_0)->controller_index, 3, 4, (long)function_24b4a9);
								local_6.load(&local_6);
								return;
							}
							if (online_user_requires_passcode(&local_5))
							{
								s_screen_parameters local_6;
								local_6.field_c = 0;
								function_149f49((s_message *)&local_6, 6, 0, 1 << (*arg_0)->controller_index, 3, 4, (long)function_2ba45b);
								local_6.load(&local_6);
								return;
							}
						}
						else
						{
							function_14800c(v11(), v12());
							return;
						}
					}
					local_4->sign_in(0);
					function_14800c(v11(), v12());
				}
			}
			else
			{
				g_470a60 = local_0->index;
				field_8a = true;
				dialog_choice_show(1, 0x77, 4, 1 << (*arg_0)->controller_index, function_24aad1, 0, 0);
			}
		}
	}
}

void function_120e20(long *arg_1, long arg_0);

// @retail 0x249fcc
void function_249fcc(c_unknown_249fa3 *arg_0)
{
	long local_1;
	c_list_24a64d *local_0 = (c_list_24a64d *)arg_0;
	function_120e20(&local_1, local_0->get_controller_index());
	if (NONE != (long)local_1)
	{
		s_list_item_iterator local_2;
		local_2.iterator.data = local_0->data;
		local_2.iterator.index = NONE;
		local_2.iterator.datum_index = NONE;
		while (function_2b2327(&local_2))
		{
			byte *local_3 = local_2.item;
			if (((s_datum_24a763 *)local_3)->index == local_1)
			{
				local_0->select_datum(local_2.iterator.datum_index);
				break;
			}
		}
	}
}

// @retail 0x24a80d
void c_screen_24a7bf::v25(s_screen_focus *arg_0)
{
	c_class_1473c9::v25(arg_0);
	function_249fcc((c_unknown_249fa3 *)&field_614);
}

void function_2b10a3(c_class_2b0b5e *widget, dword const *appearance, long type);

// @retail 0x24a7ec
void function_24a7ec(dword const *arg_0, c_class_2b0b5e *arg_1, long arg_2, bool arg_3)
{
	if (arg_1)
	{
		if (arg_3)
			arg_1->value6e = false;
		else
			function_2b10a3(arg_1, arg_0, arg_2);
	}
}

struct s_player_profile;
struct s_entry_b;
struct s_localized_name;
void player_profile_get_highest_key(s_player_profile *arg_0, long *arg_1, long *arg_2);
long function_19c4e0(long arg_0);
long function_19c440(long arg_0, long arg_1);
s_entry_b *function_19c1f0(long arg_0);
wchar_t *localized_name_get(s_localized_name *arg_0);
s_text_block *function_253c06(c_class_1a2c81 *arg_0);

// @retail 0x24a886
void c_screen_24a7bf::v3()
{
	c_class_1a2c81::v3();
	bool local_0 = function_24a855();
	s_entry_24a64d *local_1 = function_24a828();
	dword local_2[4];
	memcpy(local_2, local_1->field_11c, sizeof(local_2));
	s_widget_item local_3;
	if (local_0)
	{
		local_3.value5e = true;
		local_3.flags = 0x20;
	}
	else
	{
		local_3.value4 = (long)local_1->field_0c;
		memcpy(local_3.value48, local_2, sizeof(local_2));
		local_3.flags = 3;
	}
	function_22f042(&local_3, this, 1);
	set_child_value6e(6, 3, false);
	c_text_widget_45a5e0 *local_4 = (c_text_widget_45a5e0 *)find_child(6, 4, false);
	if (local_4)
	{
		long local_5 = 0x7000001;
		if (!local_0)
		{
			switch (((byte *)local_1)[0x105])
			{
			case 0: local_5 = 0x7000001; break;
			case 1: local_5 = 0x80002f5; break;
			case 2: local_5 = 0x60002f6; break;
			case 3: local_5 = 0xf0002f7; break;
			default: local_5 = 0; break;
			}
		}
		local_4->function_253b1a(local_5);
		local_4->value6e = true;
	}
	local_4 = (c_text_widget_45a5e0 *)find_child(6, 5, false);
	if (local_4)
	{
		long local_5 = 0x7000001;
		if (!local_0)
		{
			switch (((byte *)local_1)[0x104])
			{
			case 0: local_5 = 0x7000001; break;
			case 1: local_5 = 0x90003e4; break;
			case 2: local_5 = 0x50003e5; break;
			case 3: local_5 = 0xb0003e6; break;
			default: local_5 = 0; break;
			}
		}
		local_4->function_253b1a(local_5);
		local_4->value6e = true;
	}
	local_4 = (c_text_widget_45a5e0 *)find_child(6, 6, false);
	if (local_4)
	{
		if (!local_0)
		{
			long local_5, local_6;
			player_profile_get_highest_key((s_player_profile *)((byte *)local_1 + 4), &local_5, &local_6);
			if (local_5 != function_19c4e0(1))
				local_5 = function_19c440(1, local_5);
			if (local_5 != NONE)
			{
				s_entry_b *local_7 = function_19c1f0(local_5);
				if (local_7)
				{
					word *local_8 = (word *)localized_name_get((s_localized_name *)local_7);
					local_4->function_22f52e()->set_text(local_8);
					goto local_9;
				}
			}
		}
		long local_10 = function_253c06(local_4)->string_handle;
		local_4->function_253b1a(local_10);
	}
local_9:
	local_4 = (c_text_widget_45a5e0 *)find_child(6, 7, false);
	if (local_4)
	{
		long local_5 = 0x60000b8;
		if (!local_0)
		{
			long local_6, local_7;
			player_profile_get_highest_key((s_player_profile *)((byte *)local_1 + 4), &local_6, &local_7);
			switch (local_7)
			{
			case 0: local_5 = 0x400028e; break;
			case 1: local_5 = 0x60000b8; break;
			case 2: local_5 = 0x600028f; break;
			case 3: local_5 = 0x9000290; break;
			}
		}
		local_4->function_253b1a(local_5);
	}
	set_child_value6e(8, 6, local_0);
	function_24a7ec(local_2, (c_class_2b0b5e *)find_child(7, 0, false), 0, local_0);
	function_24a7ec(local_2, (c_class_2b0b5e *)find_child(7, 1, false), 1, local_0);
}
