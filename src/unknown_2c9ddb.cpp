// @flags /O1 /Oi /Gr
/* UNKNOWN_2C9DDB.CPP: the custom game profile screen (vtable 0x45d6f8),
   which lists the saved variants of one game type, and its list (vtable
   0x45d768). The 21 create functions are the screen for each game type, in
   three modes. */

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"
#include "unknown_19b516.h"

void function_148a58();

/* unknown_2b116a.cpp */
struct s_list_item_iterator
{
	byte *item;
	s_record_pool_iterator iterator;
};

bool function_2b2327(s_list_item_iterator *iterator);

/* the last variant saved (lane L, not decompiled yet) */
long __stdcall function_120e70(byte *buffer);
/* a variant's name (not decompiled yet) */
word *function_215b50(long variant, word *buffer);

/* the variant being edited and the one chosen last */
long g_50933c;



/* an item of the list's data */
struct s_variant_item
{
	short salt;
	short value2;
	long variant;
	long string_handle;
};

#include "unknown_19b510.h"
#include "unknown_13fd90.h"

bool function_212bc0(long file_index, s_game_variant *variant);
bool __stdcall function_215f40(long type, byte *buffer);
long function_212380(long type, long controller_index, byte *buffer);
void ui_game_variant_build_default(long type, s_game_variant *variant);
void function_148aa3(long error, word controller_flags);
long saved_game_file_type_from_variant(s_game_variant *variant);
void function_238c69(long mode, long type, word *name, long maximum_count, long controller);
bool __stdcall function_19a728(s_game_variant *variant);
bool __stdcall function_2cb001(long controller_index);
c_class_1473c9 *__stdcall function_2baae7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bb31d(s_screen_parameters *parameters);
extern long g_55c154;

// MiniMax A (11-05): no new form, no lower score; the second half's redundant selected re-read dropped 150, and retail's two 0x212bc0 call sites and its out-of-line 0x13fac0 copy still need another lane's file.
// MiniMax H (11-04): no new form, no lower score; the two 0x212bc0 arms with a copy in each (174) and the selection read as a long after the call (77) are worse than the one call site with the classification spill, and retail's out-of-line 0x13fac0 copy is inlined into our build, which lives in another lane's file.
// MiniMax R (11-01): no new form, no lower score; the permuter (900 s) found only 60, so the creation
// classification spill and the expanded string copy remain.
// MiniMax G (11-04): no new form, no lower score; the creation classification spill and the
// expanded string copy remain.
// MiniMax C (11-11): no new form, no lower score (61): the link still expands the name copy in
// place where retail calls 0x13fac0 out of line, and the default/existing choice is still spilled
// to a byte slot where retail branches on the selected index itself.
// MiniMax H (11-13): no new form, no lower score (61). Retail's two load call sites (the default arm inline,
// the selected arm out of line at 0x2ca467) still cost 174 in a two-arm rewrite, because the frame flips to
// `mov ebp, esp` with `sub esp, 0x254` and `this` leaves EBX; declaring 0x238c69 __fastcall with the
// controller first (retail's own convention, as the sibling 0x238c21 has it) gives 75, since the reduced image
// holds no body to take the convention from. The name copy is still expanded in place where retail calls 0x13fac0.
// The permuter ran over this body for 300 s (six tries) and never beat its baseline of 56 differing instructions,
// and rewriting the flag so that it is read again from `selected` after the load (instead of living in a byte
// slot) scores 77, so the byte slot stays.
// score 61 (was 61): Round 32: retail splits the block into two 0x212bc0 call sites (the default arm inline with
// `saved` in EDX, the selected arm out of line at 0x2ca467), each with its own g_54e49c store, its own copy into
// g_54e4a0 and its own 0x238c69 call (mode 5 / mode 7); ours keeps one call site and materialises the choice in a
// byte temporary. Two arms with one call each 174 (VC moves `saved` to EBX, spills `this` to [ebp-4] and flips the
// frame to unbiased); the permuter (26 tries in 300 s) found nothing better than the opening body.
// Round 32 (second pass): our first half matches retail instruction for instruction; the twelve internal jumps
// differ only because our second block is six bytes shorter, so the epilogue sits six bytes earlier. Retail's arms
// are `if (selected == NONE) {default} else {selected}` (the test is `cmp esi,-1` and the jump goes to the
// out-of-line selected arm). Retail calls 0x13fac0 out of line and reuses its ESI/EDI argument registers for the
// 0x216a50 call that follows; our link inlines the copy into a wcsncpy call plus a terminator store, and 0x13fac0
// lives in another lane's file. The two-arm rewrite again 174 (595 bytes, `this` in EDI spilled to [ebp-4], EBX
// takes the 1<<index shift, `variant` moves to [ebp-0x154], the frame flips to `mov ebp,esp` with `sub esp,0x254`).
// Earlier at 61 (from 61): biased 0x250 frame agrees; creation classification spill
// and expanded string copy remain. File-type result 61; word classification 69;
// direct source argument 61; unsigned saved handle 61; result plus word 69.
// creation classification spill and copy inline boundary;
// inline depths zero 88, one 61; shared request 90; integer classification 71;
// separate load inputs with shared publication 178.
// native entry bitfield 61; source-origin comparison/load-result/native view 79/79/79;
// loaded-name pointer before default/at copy 88/88 (copy becomes a call);
// origin plus name/native-origin-name 94/94. Creation split and copy boundary remain.
// request-mode classification 161; immutable classification 61; explicit live source 62;
// entry reference 61; creation output 99; immutable output 99; shared mode request 88;
// failed-creation join 61. Classification spill and copy expansion still differ.
// byte classification 61; signed-byte classification 61; initialized variant 170; initialized request 155;
// frame pointer 98; global optimization disabled 518; controller-index snapshot 61; speed preference 466.
// Biased 0x250 frame agrees; shared-load classification spill and string-copy expansion
// still differ. source default override 62; source existing override 61; source handle
// lifetime 76; separate current selection 61; name generation result 61; saved handle
// workspace 61; native generated name 61; native name source override 62.
// classification spill and selective copy expansion remain;
// destination name after/before default 88/88; destination record late/early 88/88;
// record/name receivers 88; publication order 174; named second load 61;
// common name receiver 88; split loads plus common/default name receiver 199/199.
// classification spill and copy expansion remain;
// external controller-slot / item-handle / list-receiver references 61/61/61;
// controller-item / controller-list / item-list / all references 61/61/61/61;
// all references with live type-field binding 61. Original inputs and body restored.
// classification spill and selective copy expansion remain;
// default-first enum/source arms/override/exclusive consumer 71/71/72/176;
// selected-first enum/source arms/override/exclusive consumer 175/175/175/175.
// creation classification spill and selective copy expansion remain;
// live entry-handle/edit-mode/type fields 61/61/61; handle-mode/handle-type/mode-type
// field pairs 61/61/61; three live fields and external record combination 61/61.
// retail has two load arms (default/selected) and an out-of-line copy call.
// separate noinline declaration of the copy 61 (still inlined); single load re-testing the
// selection 77; two load arms 174; two arms plus a shared default-name pointer 179 (the copy
// becomes retail's register call, but zero is kept in ebx and this spills); arms on direct
// entry reads 179.
// Entry declarations with in-place assignments: source carrier 61; classification 61; both 61;
// reversed pair 61; pair plus selected carrier 61; pair plus request workspace 61; pair plus
// both buffers 61; pair with exchanged buffer declaration order 61.
// Round 32 (lane 16): our link step inlines 0x13fac0 here because all three arguments fold (a
// global destination, a local source, a constant count); 0x148c3e on main hits the same and reads
// its count through a pointer. Two load arms (default first, selected first) 174/174, both with an
// unbiased frame and the constant 1 in EBX; one load per arm into a shared result then the
// original arms 110; 0x13fac0's terminator through a pointer to the last element 61 (callers kept,
// copy still inlined); /arch:SSE on this file (diagnostic only) 72, still inlined.
// Round 77: noinline declaration of the copy in its own file (retail's ecx/edi/esi call) 88
// (one arm is short); two load arms 174, with that call 179 (EBX takes zero, saved and the
// 0x54e4a0 constant, `this` spills to [ebp-4], unbiased frame); selected arm first 178;
// saved declared at the top 179.
// Round 21 (lane 10): a named count shared by the copy and the 0x238c69 call (`long maximum_count =
// 0x20;`, as matched 0x148c3e passes it) keeps unicode_string_copy out of line with retail's
// ecx/edi/esi call and no noinline. Two load arms with the inline copy 174; with the named count 179
// (EBX holds saved and then &g_54e4a0, `this` moves to ESI and spills, unbiased frame); the count
// declared at the arm's top 179; selected arm first 178; one load with the named count 88. Original
// body restored.
// Round 35: two load arms 174 again (`this` spills to [ebp-4], EBX takes the constant 1, unbiased
// frame), with the shift bit test 174; the bit test as a signed shift 62 (sar); `selected == NONE`
// tested twice without the classification local 77 (574 bytes). BLOCKED-BY 0x13fac0: our link
// inlines the copy at this all-constant call site, where retail calls it.
// Round 97 (lane 7): a named destination `word *variant_name = g_54e4a0.name;` for the copy and the
// 0x238c69 call keeps 0x13fac0 out of line with retail's ecx/edi/esi setup: on the one-load body 88 (562
// bytes; aligned differences 25 -> 19, only the duplicated selected-arm load left); on two load arms 179.
// Two load arms with the named count: 179 (`this` in ESI and spilled, EBX holds a zero constant, `saved`
// and the &g_54e4a0 address in turn); with the count named for both calls 179; with the variant global as
// its own extern symbol 179 (same allocation).
// Round 149 (lane 4, fast; all on two load arms with the named destination, 179): a fresh local for the second
// half's selection 179; the named destination shared by both arms' 0x238c69 calls 179; the game type read once
// into a local for the three calls 168 (573 bytes; `this` moves to EDI unspilled, frame still unbiased); the
// controller index read once into a local 176 (576); the default arm with its own failure call 179. In every
// two-arm form the callee-saved registers rotate (`this` ESI and spilled, entry EDI, saved EBX, EBX also caching
// zero in the first half and &g_54e4a0 in the second, where retail pushes immediates): retail's EBX `this`,
// ESI entry, EDI saved only appear with the one-load body.
// Round 69 (m3 lane 9, fast): retail's two load arms on the plain body 174 (595 bytes: EBX takes the constant
// 1 in the first half and `saved` in the second, `this` moves to EDI and spills, unbiased frame); on it: the
// first half's selection in its own local with a fresh `selected` in the second half 174, entry->variant read
// directly in the second half 174, the name buffer declared just before the second half 174, `saved`
// declared at the top 174 (all the same code).
// Round 122 (fast, two load arms): plain 174 (595 bytes); under a named bool 174; entry field read directly in both
// arms 174; a success flag with no returns in the second half 174; saved declared ahead of the first half 174; game
// type read once into a local 160 (586: `this` EDI, the type in the item's stack home). Retail and our one-load body
// colour saved EDI, entry ESI, `this` EBX; every two-arm body flips to `this` EDI, entry ESI, saved EBX, and EBX then
// also takes the hoisted constant 1; the spill or home reuse goes with the unbiased frame. Original body restored.
// Round 65 (m3 lane 10, fast): two load arms with a controller-index local 172 (589 B), with g_54e4a0 copied before the
// g_54e49c store 181, plain 174 (595 B), with game-type and controller-index locals 162 (580 B); the one-load body with
// a controller-index local 89 (569 B). Every two-arm form flips the frame to unbiased at +0x1.
// Round 140 (m3 lane 5, fast; all on two load arms written `if (selected == NONE) { if (load(saved)) {...} } else if
// (load(selected)) {...}` with the game type read once into a local for its three uses): with the controller index
// read once into a local 161 (587 B); with the named destination for the copy 185 (571 B); selected arm first 160
// (586 B); under a named `create_default` bool 160 (586 B); controller local and named destination 184 (572 B).
// Round 145 (m3 lane 6, fast): two load arms 174 (595 bytes); with the saved-index store after each copy 195
// (600); count as sizeof(name)/sizeof(name[0]) 61; arms on entry->variant directly 174; arms with their own
// scoped variant 174. Compile-only (/O1 /Oi /Gr): every two-arm shape (either order, nested, no else, a mode
// variable with one 0x238c69 call, one copy, fewer constant-1 uses) hoists the constant 1 into EBX and moves
// `this` out of EBX; the one-load body with a second load call or per-branch copies does not.
// Round 55 (m3 lane 15, fast): cause of the two-arm flip found. `this` and the hoisted constants (1, then &g_54e4a0) must survive `rep movs`, so
// they fight for EBX; retail hoists neither. One extra `this` read in the block after the NONE test (diagnostic only, `flag_c = true;` or a
// user_flags read) gives `this` EBX, entry ESI, saved EDI, the 0x250 frame and retail's whole two-arm layout (173 with the probe, frame still
// unbiased and &g_54e4a0 hoisted in EAX); the same read deeper, at the first half's top or the second half's, does nothing. Retail evidence
// for `function_238c69(long controller, long mode, long type, word *name, long maximum_count)`: 0x2ba9f4 and this function load ECX after the
// 0x216a50 call, as the sibling 0x238c21 declares it. That order keeps 0x238c69 and 0x2ba9f4 matched and gives retail's `push eax; push 7; mov eax,
// [controller]` tail: one load 76 (574 bytes), two arms 167 (590), with a named destination and/or a named count (0x13fac0 out of line as retail) 183
// (575) each, with the copy written through variant_bytes 183. Not kept (each rises above 61). Two arms: if/else instead of the first half's return 174;
// `if (*item != NONE)` around the body 174, both 174; a failure call in each failing arm or in two (tail-merged) 174/174/174;
// top test nested or as `!flag_a || !bit` or `selected == NONE || ...` with an empty arm 174 each; `memcpy` for the copies (the sibling 0x2c4e9c's form)
// 174, one load 61; the bit test through a one-bit field with TEST_FIELD_BIT 174 (one load 61), bare in the `&&` 184. Diagnostics, not kept: dropping
// either `saved` store 164-166, all shifts 160, all &g_54e4a0 uses 256 (`this` ESI, no spill). Next: why retail does not hoist &g_54e4a0.
// Round 55, second pass: the id and variant stores through a same-file forceinline helper at all three sites, two arms 174; that with the 0x238c69 order and both named locals 183.
// T62: complete sole exit and target inline metadata 61/61; conditional load-call
// result direct/named 178/178, short-circuit result direct/named 110/110; complete
// exit with conditional/short-circuit load 178/110. NOT kept: no gain; load-result
// graphs lose the biased frame. Classification spill and copy expansion remain.
// Round 153 (m3 lane 5, fast with 0x13fac0/0x212bc0/0x238c69): the game type read once into a local at the function top on
// the one-load body 178 (575 B), the controller index read once at the top 188 (559 B); on the two load arms: game type at
// the top 177 (589 B), controller index at the top 172 (580 B), both 174 (575 B). Compile-only screen (32 two-arm forms:
// store order, assignment or memcpy copy, named count, arm order, entry field or local selection): every two-arm form takes
// the unbiased frame with `this` in ESI or EDI; none gives retail's `lea ebp, [esp - 0x70]` with `this` in EBX.
// DH384: byte-array record publication, first/creation/both: memmove 139/143/157,
// memcpy 61/61/61; bounded loaded-name memcpy/memmove input 99/104. NOT kept:
// no gain; name snapshots lose the biased frame, even at retail length.
// Round 394: actual selected/saved/source-index eight-byte homes 177/74/61;
// selected+saved/selected+source/saved+source/all 182/187/104/193; all unsigned 193.
// Original narrow values and complete body restored; classification spill/copy expansion remain.
// MiniMax S (11-08): no new form, no lower score (61); every form with a second load call site still flips the frame to `mov ebp, esp` and moves `this` out of EBX, the link still expands the name copy in place where retail calls 0x13fac0, and the default/existing choice is still spilled to a byte slot.
// MiniMax E (11-06): no lower score this round; the body is unchanged. Retail's second half
// holds two load call sites (the default arm inline, the selected arm out of line at +0x1e3),
// each with its own 0x54e49c store and its own 0x4c-byte copy into 0x54e4a0, and one shared
// 0x238c69 request call that both arms reach with the mode (5 or 7), the file type, the name
// and the count pushed as values. Forms measured (fast): the load index chosen by an if 77 and
// by the inverse test 81, the index chosen by a bool plus a conditional 62, one load with a
// shared request call 90, one load with a named destination, a named count and a shared
// request call 104 (546 bytes), two loads feeding one request call 92, two loads with one copy
// and one request call 178, the screen parameters local moved to the top 92. The cause of the
// gap is now isolated: every form with a second load call site flips the frame to `mov ebp, esp`
// with `sub esp, 0x254` and moves `this` out of EBX, so retail's biased 0x250 frame and the
// instruction-identical first half cannot be kept at the same time as retail's two call sites.
// Still different: our link expands the name copy in place where retail calls 0x13fac0, our
// copy's destination address is hoisted into EAX where retail uses the immediate 0x54e4a0, and
// the default/existing choice is spilled to a byte slot where retail branches on the selected
// index itself. The permuter was started for 300 s and the harness stopped it before it
// printed a result.
// MiniMax D (11-06): no lower score this round; the body is unchanged. Retail's second half
// holds two load call sites, the default arm inline and the selected arm out of line at
// +0x1e3, each with its own 0x54e49c store, its own 0x4c-byte copy into 0x54e4a0 and its own
// 0x216a50 call, and one shared 0x238c69 request call with the mode and the file type as
// values sunk into the two arms. Forms that split that half score worse: two load calls 174,
// one load with a per-arm copy and two request calls 167, one load with a per-arm copy and a
// mode/type pair feeding one request call 176, two loads feeding one request call 103; each
// of them reads its arguments at [ebp + 0x8] and its locals at [ebp - 0x150] instead of
// retail's [ebp + 0x78] and [ebp - 0xe0], and leaves `this` out of EBX. A named count keeps
// the copy out of line with retail's edi/ecx/esi call but scores 88. Still different: our
// link expands the copy in place where retail calls 0x13fac0, and the default/existing
// classification is spilled to a byte slot where retail tests the selected index directly.
// R223 (lane 3, fast with 0x13fac0/0x212bc0/0x238c69, control 61; aligned count in brackets, control [25]): the saved index
// and the variant as two separate extern symbols instead of the storage record's members (diagnostic) 61 [25] on the one-load
// body and 174 [67] on two arms: the address constant is hoisted either way, so the record is not the cause. One load with the
// saved test and the load joined by `&&` 65 [28]; two arms setting a `started` flag in place of the returns 174 [67]; two arms
// with the selected arm first 174 [79]; plain two arms 174 [67]. Size preference off on two arms (one diagnostic) 467 (808
// bytes): the missing hoist is not a speed-preference artefact.
// R223, second pass (one-load body): the name buffer as word[0x80] 61 [25] (same code); the arms swapped under `!create_default`
// 60 [30] (one point, not kept); `long saved = NONE;` set under the name test with the saved test after it 61 [25] (same code);
// no bool or index local, `selected == NONE ? saved : selected` in the load call and `selected == NONE` at the arms 77 (574
// bytes) [25]; the same on entry->variant with no selected re-read 154 (577 bytes) [25].
// Round 86 (m3 lane 11, fast; two load arms 174 as control). New forms: the game-type local (160, re-measured) read
// after the g_55c154 store 160 (same code); that local declared inside the 0x215f40 success block for its two later
// uses 161; the first half's bit test on `selected` instead of a second entry read 61 on the one-load body, 174 on two
// arms (same code each); the default flag cleared as `g_54e4a0.flags &= ~1` 61 / 174 (the same byte `and`, so the
// field name could replace the raw byte array once the body matches); two arms as `selected == NONE && load(saved)` /
// `else if (selected != NONE && load(selected))` 174. Re-measures: the failure call at each of the four failure points
// 174, with the game-type local 161. Diagnostic: `flag_c = true;` after the saved test now 153 (605 bytes) with `this`
// still in EDI, so round 55's probe no longer reproduces. In every two-arm form EBX is shared by non-overlapping values
// (the type or constant 1, then saved, then the &g_54e4a0 constant) and `this` takes EDI.
// MiniMax A (11-12, run 63): no new form, no lower score (61); the body already has retail's two load call sites
// (the default one out of line, the selected one inline) and retail's frame, so what is left here is the copy's
// destination hoisted into EAX before `pop ecx` where retail uses the immediate, and our link's inlined copy.
// MiniMax A (11-12, run 62): no new form, no lower score (61); the second half's call split into the two arms
// retail has 174 (595 bytes) and the default choice tested without the byte local 77 (574 bytes), so the spill
// stays; the first half still agrees to the second half's entry, where ours spills the default/existing choice to
// a byte slot and retail branches on the selected index, and our link still inlines the 0x13fac0 name copy where
// retail calls it out of line (BLOCKED-BY the copy's own file).
// MiniMax A (11-05): no new form, no lower score; the second half's redundant selected re-read dropped 150, and retail's two 0x212bc0 call sites and its out-of-line 0x13fac0 copy still need another lane's file.
// MiniMax H (11-04): no new form, no lower score; the two 0x212bc0 arms with a copy in each (174) and the selection read as a long after the call (77) are worse than the one call site with the classification spill, and retail's out-of-line 0x13fac0 copy is inlined into our build, which lives in another lane's file.
// MiniMax R (11-01): no new form, no lower score; the permuter (900 s) found only 60, so the creation
// classification spill and the expanded string copy remain.
// MiniMax G (11-04): no new form, no lower score; the creation classification spill and the
// expanded string copy remain.
// MiniMax C (11-11): no new form, no lower score (61): the link still expands the name copy in
// place where retail calls 0x13fac0 out of line, and the default/existing choice is still spilled
// to a byte slot where retail branches on the selected index itself.
// M3 lane 12 r119: no trial. Retail's second half is two load call sites (default arm inline, selected arm out of
// line) feeding one shared 0x238c69 call, with 0x13fac0 called (ECX source, EDI destination, ESI count) and every
// &g_54e4a0 use an immediate; ours has one load site, the inlined copy and the destination address in a register.
// The two-arm forms (lane 5 r153's 32-form grid, R223, rounds 32-97) all flip the frame to unbiased with `this` out
// of EBX, and the named-count form that keeps 0x13fac0 out of line is listed (88/179). Still BLOCKED-BY 0x13fac0.
// MiniMax I (11-14): no new form, no lower score (61): the link still expands the name copy where
// retail calls 0x13fac0 out of line (its own file, src/unicode_strings.cpp, is not this round's), and
// the default/existing choice is still spilled to a byte slot where retail branches on the selected
// index itself.
// @retail 0x2ca284
void c_class_2c9e69::handle_item(s_controller_reference **controller, long *item)
{
	if (*item == NONE) return;

	s_variant_item *entry = &((s_variant_item *)data->data)[*item & 0xffff];
	s_game_variant variant;
	byte name[0x100];
	long selected = entry->variant;
	if (selected != NONE && !(flag_a && (bool)(((dword)entry->variant >> 21) & 1)))
	{
		if (entry->value2 != NONE)
		{
			if (function_212bc0(selected, &variant))
			{
				s_screen_parameters parameters;
				if (flag_a)
				{
					g_54e49c = entry->variant;
					g_54e4a0 = variant;
					parameters.field_c = 0;
					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index,
						3, 4, (long)function_2baae7);
					parameters.load(&parameters);
				}
				else if (flag_b)
				{
					parameters.field_c = 0;
					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index,
						5, 4, (long)function_2bb31d);
					parameters.load(&parameters);
				}
				else
				{
					function_19a728(&variant);
					get_screen()->start_animation(3);
				}
				return;
			}
			flag_c = true;
			g_50933c = entry->variant;
		}
		else g_50933c = selected;
		dialog_choice_show(1, 0x79, 4, 1 << (*controller)->controller_index, function_2cb001, 0, 0);
		return;
	}
	g_55c154 = 0;
	if (function_215f40(game_type, name))
	{
		long saved = function_212380(game_type, (*controller)->controller_index, name);
		if (saved != NONE)
		{
			selected = entry->variant;
			bool create_default = selected == NONE;
			long source_index = create_default ? saved : selected;
			if (function_212bc0(source_index, &variant))
			{
				g_54e49c = saved;
				g_54e4a0 = variant;
				if (create_default)
				{
					ui_game_variant_build_default(game_type, &g_54e4a0);
					unicode_string_copy(g_54e4a0.name, variant.name, 0x20);
					function_238c69(5, saved_game_file_type_from_variant(&g_54e4a0),
						g_54e4a0.name, 0x20, (*controller)->controller_index);
				}
				else
				{
					g_54e49c_storage.variant_bytes[0] &= 0xfe;
					function_238c69(7, saved_game_file_type_from_variant(&g_54e4a0),
						g_54e4a0.name, 0x20, (*controller)->controller_index);
				}
				return;
			}
		}
	}
	function_148aa3(g_55c154, user_flags);
}


/* the custom game profile screen (vtable 0x45d6f8) */
class c_custom_game_profile_screen : public c_screen_with_menu
{
public:
	c_custom_game_profile_screen(long screen_id, long a, long b, word user_flags);

	virtual void v3();
	virtual void v17();
	__declspec(noinline) void update_variant_info();
	virtual screen_load_proc get_load_proc();

	c_class_2c9e69 list;
	long game_type;
	bool flag_a;
	bool flag_b;
};

c_class_1473c9 *__stdcall function_2ca4cd(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca525(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca580(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca5d8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca630(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca68b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca6e3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca73b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca796(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca7ee(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca846(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca8a1(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca8f9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca951(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca9ac(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caa04(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caa5c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caab7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cab0f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cab67(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cabc2(s_screen_parameters *parameters);

// @retail 0x2c9ddb
c_custom_game_profile_screen::c_custom_game_profile_screen(long screen_id, long a, long b, word user_flags) :
	c_screen_with_menu(screen_id, a, b, user_flags, &list),
	list(user_flags),
	game_type(1),
	flag_a(false),
	flag_b(false)
{
}

// @retail 0x2c9e26
void c_custom_game_profile_screen::v17()
{
	list.game_type = game_type;
	list.flag_a = flag_a;
	list.flag_b = flag_b;
}

// @retail 0x2c9e4b deleting c_custom_game_profile_screen
// @retail 0x2c9e9f destructor c_custom_game_profile_screen
// @retail 0x2c9f55 deleting c_class_2c9e69
// @retail 0x2c9e69 destructor c_class_2c9e69

// @retail 0x2c9eb4
c_class_2c9e69::c_class_2c9e69(word user_flags) :
	c_class_1474e8(user_flags),
	game_type(1),
	value49a0(NONE),
	value49a4(NONE),
	flag_a(false),
	flag_b(false),
	flag_c(false),
	handler(this, (list_item_method)&c_class_2c9e69::handle_item)
{
	data = user_interface_data_new("custom game profile list", 0x1066, sizeof(s_variant_item));
	function_16b790(data);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c9f73
void c_class_2c9e69::v1()
{
	fill_and_select_first();
	function_148a58();
	((c_widget *)this)->c_widget::v9();
	if (!flag_a && !flag_b)
	{
		select_last_variant();
	}
}

// @retail 0x2c9fa2
void c_class_2c9e69::select_last_variant()
{
	byte buffer[0x130];
	long variant = function_120e70(buffer);

	if (variant != NONE)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = data;
		while (function_2b2327(&iterator))
		{
			if (((s_variant_item *)iterator.item)->variant == variant)
			{
				select_datum(iterator.iterator.datum_index);
				break;
			}
		}
	}
}

/* shows the item's variant name */
// @retail 0x2c9ff4
void c_class_2c9e69::v20(c_class_1a2c81 *item, long unused)
{
	long datum = ((c_class_14750b *)item)->value70;
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_child(6, 0, false);
	c_class_1a2c81 *icon = item->find_child(8, 2, false);

	if (datum != NONE)
	{
		s_variant_item *entry = &((s_variant_item *)data->data)[datum & 0xffff];

		if (entry->variant == NONE)
		{
			if (text)
			{
				text->function_253b1a(0x120001db);
			}
			if (icon)
			{
				icon->value6e = false;
			}
		}
		else
		{
			if (text)
			{
				word name[0x80];

				function_215b50(entry->variant, name);
				text->function_22f52e()->set_text(name);
			}
			if (icon)
			{
				icon->value6e = false;
			}
		}
	}
}

// @retail 0x2ca091
void c_class_2c9e69::v3()
{
	if (value49a4 != NONE && g_54e49c == NONE || flag_c && g_50933c == NONE)
	{
		fill_and_keep_focus();
		flag_c = false;
	}
	value49a4 = g_54e49c;
	((c_widget *)this)->c_widget::v11();
}

// @retail 0x2ca225
void c_class_2c9e69::fill_and_select_first()
{
	fill();
	select_datum(record_pool_next_used(data, NONE));
}

// @retail 0x2ca244
void c_class_2c9e69::fill_and_keep_focus()
{
	long index = get_focused_datum() & 0xffff;

	fill();
	if (index < 0)
	{
		index = 0;
	}
	else if (index > data->actual_count - 1)
	{
		index = data->actual_count - 1;
	}
	select_datum(index_to_datum_index(data, index));
}

// @retail 0x2cac1a
screen_load_proc c_custom_game_profile_screen::get_load_proc()
{
    if (game_type != 1) {
        if (game_type != 2) {
            if (game_type > 4) {
                if (game_type != 5) {
                    if (game_type > 7) {
                        if (game_type != 8) {
                            if (flag_a) return function_2cab0f;
                            if (flag_b) return function_2cabc2;
                            return function_2cab67;
                        } else {
                            if (flag_a) return function_2caa04;
                            if (flag_b) return function_2caab7;
                            return function_2caa5c;
                        }
                    } else {
                        if (flag_a) return function_2ca8f9;
                        if (flag_b) return function_2ca9ac;
                        return function_2ca951;
                    }
                } else {
                    if (flag_a) return function_2ca7ee;
                    if (flag_b) return function_2ca8a1;
                    return function_2ca846;
                }
            } else {
                if (flag_a) return function_2ca6e3;
                if (flag_b) return function_2ca796;
                return function_2ca73b;
            }
        } else {
            if (flag_a) return function_2ca5d8;
            if (flag_b) return function_2ca68b;
            return function_2ca630;
        }
    } else {
        if (flag_a) return function_2ca4cd;
        if (flag_b) return function_2ca580;
        return function_2ca525;
    }
}

/* the create functions: the screen for one game type */

// @retail 0x2ca4cd
c_class_1473c9 *__stdcall function_2ca4cd(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 1;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca525
c_class_1473c9 *__stdcall function_2ca525(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 1;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca580
c_class_1473c9 *__stdcall function_2ca580(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 1;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca5d8
c_class_1473c9 *__stdcall function_2ca5d8(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 2;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca630
c_class_1473c9 *__stdcall function_2ca630(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 2;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca68b
c_class_1473c9 *__stdcall function_2ca68b(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 2;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca6e3
c_class_1473c9 *__stdcall function_2ca6e3(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 4;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca73b
c_class_1473c9 *__stdcall function_2ca73b(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 4;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca796
c_class_1473c9 *__stdcall function_2ca796(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 4;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca7ee
c_class_1473c9 *__stdcall function_2ca7ee(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 5;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca846
c_class_1473c9 *__stdcall function_2ca846(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 5;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca8a1
c_class_1473c9 *__stdcall function_2ca8a1(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 5;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca8f9
c_class_1473c9 *__stdcall function_2ca8f9(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 7;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca951
c_class_1473c9 *__stdcall function_2ca951(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 7;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ca9ac
c_class_1473c9 *__stdcall function_2ca9ac(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 7;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2caa04
c_class_1473c9 *__stdcall function_2caa04(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 8;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2caa5c
c_class_1473c9 *__stdcall function_2caa5c(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 8;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2caab7
c_class_1473c9 *__stdcall function_2caab7(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 8;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2cab0f
c_class_1473c9 *__stdcall function_2cab0f(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 9;
	screen->flag_a = true;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2cab67
c_class_1473c9 *__stdcall function_2cab67(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xd0, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 9;
	screen->flag_a = false;
	screen->flag_b = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2cabc2
c_class_1473c9 *__stdcall function_2cabc2(s_screen_parameters *parameters)
{
	c_custom_game_profile_screen *screen = new c_custom_game_profile_screen(0xf, parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->game_type = 9;
	screen->flag_a = false;
	screen->flag_b = true;
	screen->function_147f6d(parameters);
	return screen;
}


#include <string.h>
#include "unknown_19d220.h"
void __stdcall function_215900(long controller, long type, word *capacity, long *indices, bool include_cached);
bool function_212bc0(long file_index, s_game_variant *variant);
long function_19fd00(long index);
void function_24c0c4(c_widget *widget);

// @retail 0x2ca0d9
void c_class_2c9e69::fill()
{
    long indices[0x1065];
    word count = 0x1065;
    function_215900(get_controller_index(), game_type, &count, indices, true);
    record_pool_release_all(data);
    memset(variants, 0xff, sizeof(variants));
    if (flag_a)
    {
        long index = record_pool_allocate(data);
        s_variant_item *item = &((s_variant_item *)data->data)[index & 0xffff];
        item->variant = NONE;
        item->value2 = 0;
        item->string_handle = 0x1b000289;
    }
    long i = 0;
    long limit = count < 0x1065 ? count : 0x1065;
    for (; i < limit; ++i)
    {
        long index = record_pool_allocate(data);
        s_variant_item *item = &((s_variant_item *)data->data)[index & 0xffff];
        long file_index = indices[i];
        item->value2 = 0;
        item->variant = file_index;
        item->string_handle = 0x0400021c;
        s_game_variant variant;
        if (function_212bc0(indices[i], &variant) && (bool)(((dword)indices[i] >> 21) & 1))
        {
            char value = *((char *)&variant + 3);
            long clamped = value < 0 ? 0 : (value > 100 ? 100 : value);
            if (clamped == value)
                item->string_handle = function_19fd00(value);
        }
        variants[i] = index;
    }
    function_24c0c4((c_widget *)this);
}

void __stdcall function_148b27(long index);

// @retail 0x2cb001
bool __stdcall function_2cb001(long unused)
{
    /* retail keeps the unused argument, so it returns with ret 4 */
    long const *unused_reference = &unused;
    (void)unused_reference;
    function_148b27(g_50933c);
    g_50933c = NONE;
    return true;
}

void function_253af2(c_class_1a2c81 *widget, word *string);
void __stdcall function_253b65(c_class_1a2c81 *widget, long string_handle);
struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short frame);

// @retail 0x2caf0f
void c_custom_game_profile_screen::update_variant_info()
{
    long selected = list.get_focused_datum();
    s_record_pool *data = list.data;
    if (selected != NONE && data)
    {
        s_variant_item *entry = &((s_variant_item *)data->data)[selected & 0xffff];
        if (entry)
        {
            c_class_1a2c81 *saved = find_child(6, 6, false);
            c_class_1a2c81 *bitmap = find_child(8, 2, false);
            c_class_1a2c81 *name = find_child(6, 3, false);
            c_class_1a2c81 *description = find_child(6, 4, false);
            bool is_saved = false;
            if (entry->variant != NONE && (bool)(((dword)entry->variant >> 21) & 1))
                is_saved = true;
            if (saved) saved->value6e = is_saved;
            if (bitmap) bitmap->value6e = false;
            if (name)
            {
                if (entry->variant == NONE)
                    function_253b65(name, 0x120001db);
                else
                {
                    word text[128];
                    function_215b50(entry->variant, text);
                    function_253af2(name, text);
                }
            }
            if (description) function_253b65(description, entry->string_handle);
        }
    }
}

// @retail 0x2cad45
void c_custom_game_profile_screen::v3()
{
    c_text_widget_45a5e0 *title = (c_text_widget_45a5e0 *)find_child(6, 2, false);
    c_text_widget_45a5e0 *help = (c_text_widget_45a5e0 *)find_child(6, 5, false);
    c_class_1a2c81 *bitmap = find_child(8, 4, false);
    if (title) title->value6e = true;
    if (help) help->value6e = true;
    if (bitmap) bitmap->value6e = true;
    switch (game_type)
    {
    case 8:
        if (title) title->function_253b1a(0x180001ce);
        if (help) help->function_253b1a(0x110001d8);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 7);
        break;
    case 7:
        if (title) title->function_253b1a(0x210001cd);
        if (help) help->function_253b1a(0x1a0001d7);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 6);
        break;
    case 1:
        if (title) title->function_253b1a(0x170001c7);
        if (help) help->function_253b1a(0x100001d1);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 0);
        break;
    case 4:
        if (title) title->function_253b1a(0x180001ca);
        if (help) help->function_253b1a(0x110001d4);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 3);
        break;
    case 2:
        if (title) title->function_253b1a(0x210001c8);
        if (help) help->function_253b1a(0x1a0001d2);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 1);
        break;
    case 3:
    case 5:
    case 6:
        if (title) title->function_253b1a(0x1b0001cb);
        if (help) help->function_253b1a(0x140001d5);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 4);
        break;
    case 9:
        if (title) title->function_253b1a(0x1c0001cf);
        if (help) help->function_253b1a(0x150001d9);
        if (bitmap) function_2b0a14((s_widget_view_2b0a *)bitmap, 8);
        break;
    }
    update_variant_info();
    c_class_1a2c81::v3();
}
