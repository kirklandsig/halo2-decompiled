// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_189CD0.CPP: plays a looping sound slot's sound, positioned on an
   object or not. Retail keeps the standard convention with all seven
   arguments (callers push seven words, ret 0x1c), though the fourth and
   fifth are unused. */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "sound_records.h"
#include "object_markers.h"
#include "local_cameras.h"

struct s_sound_source_description
{
	char value0;
	byte flags;
	byte unknown02[2];
	point3f position;
	vector3f direction;
	byte unknown1c[8];
	long value24;
};

long function_189ee0(long tag_index);
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);
void object_dispatch_sound_started(long object_index, long tag_index, long sound_index);
extern s_sound_source_callbacks const g_444b3c;
void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description);
long function_18d5b0(long label);
long function_189fe0(s_sound_request const *request, long tag_index);
long function_189340(long object_index, long tag_index, long audible, s_sound_marker const *marker, s_sound_source_callbacks const *source, real scale, long platform_playback, char const *variant);

// score 11 (was 11): every object-index read is plain but the started test, which reads
// through the reference; the code is retail's except that test (`cmp [esp+0xb8], -1`
// where retail compares the late EBX load, 526 bytes). Before round 173 (at 21): retail reads the object index once, after the playback call, into
// ebx and copies it to esi for the call; ours reads it into esi at the call and again
// into ebx for the started test (526 bytes). With plain reads at the call, test and
// dispatch (round 45, 90) the late read is one load as in retail, but into esi (the
// call's register) with the sound index in edi, and the NONE branch stores esi for the
// object index. Round 45: the early object local for every later use 116; the started
// test nested 21, with plain late reads 90; plain call read with the test and dispatch
// through the reference 28. Earlier: late object carrier/reload still adds five bytes;
// qualified object refresh 100f/85p;
// playback then refresh 101f/35p; chained request zeros 121f; initial result 118f; refresh with zeros
// 103f/85p; object-first admission 118f; byte positioned state 118f; refreshed nested admission 100f/85p.
// Argument-list assignment 85; plain reads
// throughout 116; second half plain 90; all three through the reference 28; first half
// plain 41; the first test alone through the reference 169; non-volatile reference 116.
// frame 0xa0 agrees; +0x115 consumes the slot pointer
// for the variant, then reloads it; object playback setup and source-result
// allocation also differ. Earlier pointer/result scopes tied at 38; standalone
// argument playback gave 83/523. Ordinary trials: indexed final store 156/537;
// descriptive homes 35; shared source mode 97/521; slot reference 35; argument
// playback with early request variant 81/523; argument playback with indexed
// final store 148/539; failed-duration tail 35; early variant and argument
// playback with descriptive homes 81/523; object retained during argument
// evaluation 85/523. Opening executable body restored.
// slot base/result registers at +0x115 and object playback setup remain;
// direct duration 35; duration with argument playback 85; shared permutation 123;
// permutation with argument playback 123; completion result 55; request variant first 41;
// callback fields first 60; separate standard noinline declaration 35 (ordinary scores).
// With plain reads throughout the object index takes ebp and the class spatialization is
// spilled (frame 0xa4); retail keeps spatialization in edi and the object index in memory.
// Call plain with test and started call through the reference 28; the existing class
// spatialization helper 154 (plain reads) and 126 (as here), its ternary losing the NULL
// store; spatialization declared first with plain reads 130; test through the reference 28.
// Shared spatial query 134f; native positioned value 114f/109p; both 143f;
// outer description/priority scale 118f each; spatial/playback/request call summaries 118f each.
// Native description bit 141f; native positioned bit 118f; both bits 141f; request workspace 118f;
// request and native bit 141f; native bit and result scope 141f; native bit and request admission
// 141f; native bit with both workspaces 141f (fast scores; unchanged fast 118).
// Post-playback notification object 118f; two-phase object 118f; full source-result join 118f;
// result with notification object 118f; positioned-source-first admission 118f; admission with
// notification 118f; admission with result 118f; all source/object lifetimes 118f (control 118f).
// Round 55 plain: const spatialization data 21; const slot 21; const class 21; const tag data 21;
// const class/spatialization 21; const slot/spatialization 21; const slot/class 21; all four 21.
// Round 118: plain reads throughout 114f/116p; the started test and dispatch through the
// early object local 149f/103p; the call too 114f/116p; the object local read in the
// positioned branch only 160f/147p; the call and started dispatch in a by-value inline
// helper 127f/57p (521 bytes; the playback label call then runs before the variant push).
// Round 134 (plain): `object = *object_reference` inside the playback call's arguments, `object` for the test and
// dispatch 85 (the NONE request's zero stores change); a named local read through the reference before the call
// for all three uses 85 (it takes ESI, the sound index EDI); the call and dispatch in a by-value inline helper
// taking the label 135 (482 bytes); no reference with the object-marker half of the description in an inline helper
// 152 (frame 0x88); the started test through the reference, the dispatch plain 28. A one-file LTCG harness (this
// file, the callees' files, the ballast) gives the reduced image's 118, not the full build's 21. Retail reads the
// object index once after the 0x18d5b0 call into EBX (a memory-resident parameter with its load reused across
// the call); the address-taken reference forces a second read after the call, plain reads enregister it in EBP.
// Round 75 (plain): the call passing the early `object` local 100; the started test on `object` 105; the
// dispatch on `object` 105; `sound_index = NONE` at its declaration 21; the description at the slot block
// scope 21; the spatialization flag if/else swapped 24. Retail's callees 0x18d4f0, 0x189fe0 and 0x189340 all
// keep ESI, so the EBX copy is an allocation choice, not a callee summary.
// Round 135 (plain): direct duration address 21; const early object 21; object declared before
// spatialization 21; object declared at slot-block scope 21; sound result declared after the clamp 21;
// inline started-dispatch declaration 21 (0x109220 MATCH); inline spatial playback declaration 21
// (0x189340 MATCH); nested late playback scope 21. All restored; the late object load still differs.
// Round 51 (plain): `(void)&object_index` with plain reads 116 (531 bytes); `inline` on 0x189340's definition with the plain
// late read 90 (the callers keep their match, no change); the call and dispatch in a by-value helper taking the label, plain
// inline 156 (468 bytes, stays out of line), __forceinline 57 (521 bytes, retail's call order, object in ESI, sound index in EDI);
// the plain late read with `sound_index = NONE` at its declaration 90, with the test nested and the final test on the stored
// field 90; plain reads with spatialization declared before the class 116, with an if/else class getter forced inline 116.
// The plain-reads listing (116) already has the sound index in ESI and the copy to ESI at the call: there the spatialization
// pointer is the spilled variable (frame 0xa4) and the object holds EBP, where retail spills the object to its parameter home
// and keeps spatialization in EDI and the tag in EBP. A form that ranks spatialization above the object is the next thing to find.
// Round 90 (m3 lane 9, plain): a fresh else-scope local assigned from the reference inside the playback call's arguments and
// used for the started test and the dispatch 85 (523 bytes; the one late read takes ESI, the sound index EDI, the NONE branch
// changes); reference read at the call and the dispatch with a plain test 28 (531 bytes); plain call and test with the
// dispatch through the reference 95 (528 bytes); plain call and dispatch with the test through the reference 96 (528 bytes);
// the slot store and the return test written out in both branches 90 (530 bytes), the same with plain late reads 87 (525
// bytes). Every one-load form gives the late object ESI before the sound index is coloured; retail colours the sound index
// ESI and copies an EBX object into ESI for the call. Plain reads throughout (diagnostic, 116) put the object in EBP and
// the tag in EDI from the early test on and spill spatialization. Restored.
// M3 lane 11 r76 (plain): the two arms swapped (the positioned arm as the `if`) 21 (same code), with plain late
// reads 90; the playback result named for the test, the dispatch and the slot 65 (526 bytes: the merged sound index
// drops to EAX in the request arm), that with plain late reads 57 (521 bytes, first difference at +0xd1: the late
// object keeps ESI, the named result EDI); plain late reads with the audible value named before the call 90.
// Round 73 (lane 14, plain): the sound index declared first in the function 21; the early object local declared at the
// top 21; the sound index first with plain late reads 90 (523 bytes, the request arm's sound index takes EDI); no
// reference, an early plain copy for the test and marker calls with plain late reads 116 (531 bytes), that copy declared
// at the top 116. 0x109220 takes the object in EBX and 0x189340 in ESI, so retail's EBX load plus ESI copy is a colouring
// order between the late object and the sound index.
// Round 64 (m3 lane 2, plain): one late local read through the reference as its own statement before the call, used
// for the call, test and dispatch 85 (it takes ESI and copies to EBX for 0x109220; the sound index takes EDI); that
// with the sound index declared first in the function 85 (same code); a late local assigned a plain read inside the
// call's arguments 90; the positioned arm (description, marker, call, dispatch) as a __forceinline helper taking the
// object by value: given the early local 154 (528 bytes, frame 0xa4), given the plain parameter 168 (545 bytes). Every
// single-load form also changes the request arm (zero stores as immediates, the NONE stored from ESI).
// Round 113 (m3 lane 9, plain; four aligned lines left at 21: ours loads the object for 0x189340 into ESI before the
// label push and reloads it into EBX after the call, retail loads it once into EBX after the positioned test and copies
// it to ESI): the started test as object first 41 (528 bytes); the slot store at the end of each arm with one return
// test after the join 76 (529 bytes), with the call's object read plain 95 (530 bytes); re-measured (round 90's
// both-branch forms): each arm with its own sound index local, slot store and return test 90, with the call's object
// read plain 87 (525 bytes). The request's object index set from the early `object` local 21 (same code); the scale
// clamp at the function's top 166 (540 bytes).
// Round 173 (m3 lane 6, plain): with the early uses (NONE test, marker test, both marker calls) read plain instead
// of through an early local, and the playback call reading plain: the started test through the reference 11 (kept;
// the early plain reads share one ESI load, the late plain reads one EBX load copied to ESI, as retail), the dispatch
// through the reference instead 121 (536 bytes, frame 0xa4), a late local read through the reference for the call,
// test and dispatch 85, the test through a non-volatile reference 116 (plain code); the first NONE test alone through
// the reference with plain reads after it 169 (540 bytes); an early local for the first test only 169; the reference
// declared and unused, volatile or not 116/116 (diagnostic). Every volatile read is one load more than retail, whose
// two object loads each serve several uses; retail behaves as if the object index were memory-resident without one.
// On the kept form: the call's operand through the reference with the test plain 41 (the volatile load goes straight into
// ESI and the test and dispatch reload), call and test through it 28, test and dispatch 28, all three 28; the started test
// nested 11 and `sound_index = NONE` at its declaration 11 (same code each). A volatile read never shares its load, so no
// placement of it can match; a match needs a plain form that keeps the object index out of a register.
// Round 112 (m3 lane 16, plain; every form without the reference reads plainly): a named `attached` test for the
// early, marker and started tests 107 (530 bytes); for the marker and started tests only 162 (540 bytes); the
// positioned test written out at both uses 131 (528 bytes), with the reference kept 132 (527 bytes); no duration
// local (the parameter cast at both uses) 116 (531 bytes, the plain-reads code).
// Round 113 (m3 lane 16, plain; this body's listing differs only in the started test, `cmp [esp+0xb8], -1` for retail's
// `cmp ebx, -1`): the started test and dispatch in a forceinline helper given `&object_index` (read through the pointer)
// 116 (the address folds away, object in EBP, spatialization spilled); the started test and dispatch after the arms' join
// with plain reads 109 (533 bytes, not threaded past the request arm); plain reads with the request's object index set from
// the parameter 116; an arm-scoped object copy for every use in the positioned arm 116 (copy-propagated); the marker test
// alone through the reference with the started test plain 96 (523 bytes); plain reads with `positioned` started false and
// set under the spatialization test 141 (531 bytes).
// Round 430, plain object reads: address-first class pointer 128f/120p;
// direct class fields 119f/124p; shared class index 152f/144p; shared
// class flags 115f/113p; success predicate 141f/148p; request input
// 114f/116p; early tag exit 114f/116p; early slot exit 114f/116p.
// Unchanged preview 94f against 11p; every executable form restored.
// P432: plain object reads with marker/request/description scopes, source admission,
// class selection, independent/upper-first scale limits, native description bit;
// plain scores 116/116/116/148/154/116/120/120.
// NOT kept: no permitted confirmed gain; original executable body restored.
// Round 80 (m3 lane 15, plain; fast does not reproduce this file): the difference is a contest for EBP between the tag
// index (8 uses over the whole function) and the object index (7 uses over its second half). Diagnostics, all with every
// object read plain and no reference: the object part of the started test removed 27 at 521 bytes (retail's code but for
// the missing compare); ONE extra use of the tag index in the positioned arm (a store of it into the description) 48 at
// 525 bytes, which is retail's code exactly (tag EBP, spatialization EDI, object ESI then EBX, `cmp ebx, -1`) plus that
// store; one far extra use of the object index (in the tail) 158 (568 bytes: tag EBP, object EDI in pieces). So retail's
// compiler saw one more use of the tag index, or one fewer of the object index, than this body gives it; the volatile
// read stands in for the latter. Forms tried, all reads plain: the started test in a by-value inline wrapper around the
// dispatch 116; the marker lookup and description fill in a by-value inline helper 190 (470 bytes, out of line), forced
// inline 116 (an inlined by-value parameter is propagated before allocation, so no wrapper takes a use off the
// parameter); the spatialization pointer and the positioned test through two tag-taking inline helpers 121 (the
// predicate stays out of line), both forced inline 162 (544 bytes, the second lookup is not merged); the function's
// unused fourth and fifth arguments forwarded to 0x189340 as two parameters it ignores 116 (0x189340 and its callers
// still match). Lead: an argument LTCG drops still counts in the caller's allocation, so a callee of this function
// that takes the tag index in a parameter it never reads would give the extra use without code; no retail call shows
// which, so none was added.
// M3 lane 13 r92 (plain, every object read plain, no reference): an implied `tag_index != NONE` added to the started test
// 116 (folded before allocation: a folded test is no use); a named permutation pointer for the duration, request variant and
// playback variant 149 (528 bytes); the clamp into its own local in place of the parameter 133; the started test object first
// 118; one exit (sound index at function scope set NONE, the duration reset tested on it after the blocks) 116. In the plain
// listing the tag takes EDI and is reloaded from its home for the dispatch after 0x189340 takes EDI for the source, so the
// allocator splits a variable across regions; the object holds EBP from its first test to the dispatch.
// Round 164 (m3 lane 7, plain): on the plain-read form (no reference) the tag parameter named sound_tag_index 116, the
// object parameter named source_object_index 116, both 116, the tag named definition_index 116 (parameter names do not
// move the EBP contest); the started test and dispatch in a forceinline helper taking the object by value with its own
// parameter's address taken 116 (the address folds away).
// Round 166 (m3 lane 7, plain, every object read plain, control 116): the positioned arm first under `object_index != NONE
// || positioned` 116; the clamp through the existing PIN_126660 macro 116; the description bit as one ternary assignment
// 116; the sound index declared per arm, each arm storing it and returning on success 165 (537 bytes); the tag index
// copied once into a local used by every later read 116.
// MiniMax H (11-13): no new form, no lower score (11): the early object reads through a named local filled from the
// reference with the started test read plain scores 90 (plain build), and the reduced image does not reproduce retail's
// shape for this file (the permuter's reduced image scores the original 83 against the full image's 7), so the plain
// check is the only reliable one here; what still differs is the started test, which reads the reference
// (`cmp dword ptr [esp + 0xb8], -1`) where retail compares its late EBX copy (`cmp ebx, -1`).
// Round 186 (m3 lane 6, plain): two reads through the reference mirroring retail's two loads (an early local for the NONE
// test, the marker test and both marker calls, a late local for 0x189340, the started test and the dispatch) 85 (523 bytes); no
// duration local 11 (same code); `positioned` started false and set under the spatialization test 146 (536 bytes); the class
// index through the existing sound_definition_get 11 (same code); the request's object index from the parameter 11 (same
// code); the slot pointer declared before the sound index 11 (same code).
// M3 lane 2 r90 (plain): a late local declared at the function's top, assigned one read through the reference before
// 0x189340 and used for the call, the started test and the dispatch 85 (523 bytes); the reference as a `long const volatile &`
// variable 11 (same code); no pointer local, the started test reading `*(long const volatile *)&object_index` 11 (same code);
// the pointer declared in the arm just before the started test 11 (same code); the reference read for 0x1892a0's object
// argument instead, the started test plain 85 (524 bytes). The listing on main differs from retail only in that test.
// M3 lane 13 r101 (plain; every form without the reference, object reads plain, control 116 per the notes above): the request
// test written `!positioned && object_index == NONE` 118 (531 bytes); the spatialization picked by one ternary 154 (541 bytes);
// the sound tag read inline where the classes table is indexed (no tag local) 116; the marker declared at the positioned arm's top with
// the description 116; the started test from a bool set just before 0x189340 116. Plain: tag in EDI, object in EBP, the
// spatialization spilled (frame 0xa4); retail keeps the tag in EBP and the spatialization in EDI (frame 0xa0).
// M3 lane 5 r193 (plain; every object read plain, no reference): an ignored trailing parameter on 0x189340 passed the tag
// index (its other callers NONE; diagnostic) 116, the same code: LTCG drops the argument before allocation, so round 80's
// dropped-argument lead does not add a use; the object index as a by-value one-long class-type parameter (converting
// constructor, declarations updated; diagnostic) 116, promoted like a long, and the five callers lose their matches; the
// description flag tested as `spatialization && (spatialization->flags & 7)` (one more spatialization use; diagnostic)
// 131 (540 bytes; the object keeps EBP); the description declared at the slot block's scope 116; the request arm in a
// forceinline helper taking the tag, scale, label and slot 116.
// M3 lane 12 r137 (plain; no reference, every object read plain): the request test and the description test through one
// named `attached` test 96 (523 bytes: from the 0x18d5b0 call on, the code is retail's, the late object loaded into EBX,
// copied to ESI for 0x189340 and compared `cmp ebx, -1`; the only extra code is the bool's `setne al; test al, al`, and the
// description test becomes `test al, al`), so one object compare fewer is all retail's allocation needs; the same with the
// positioned arm first under `attached || positioned` 96 (same code); the request test as `!(attached || positioned)` 96;
// the polarity flipped (`detached = object_index == NONE`) 98 (525 bytes); the named test for the request and started tests
// instead 151 (547 bytes).
// M3 lane 5 r198 (plain; every form without the reference, object reads plain, control 116): the object index's address
// passed to an ignored trailing pointer parameter of 0x189340 (diagnostic; its other callers pass NULL) 116, the same
// code: the dropped argument leaves the parameter in a register, so an unread pointer does not keep it in memory;
// 0x109220 (this function is its only retail caller) declared (tag, sound, object) 116 and (tag, object, sound) 116, the
// same code with 0x109220 still matching (its parameter order does not move the contest); `inline` on 0x109220's
// definition 116 and on 0x1892a0's 116 (both stay out of line, same code). Reading of the plain listing: the tag index
// and the object index can each only take EBP (ESI, EDI and EBX are the register arguments of 0x1892a0, 0x189340 and
// 0xb8d30 while both are live), so whichever ranks first takes it and the other is read from its home in pieces; with
// rounds 80 and 137 the object ranks first at 7 uses against the tag's 8 and second at 6 against 8 or 7 against 9. The
// fourth and fifth arguments are a pair of decibel values the callers pass by value (0x2a9e60 builds them); a by-value
// aggregate holding the object index with them would keep it in memory, not tried: the declaration in
// include/unknown_29f5b0.h and the calls in src/unknown_29f5b0.cpp belong to another lane this round.
// M3 lane 2 r99 (plain; every form without the reference, object reads plain): `__assume(tag_index != NONE)` at the
// positioned arm's top (an extra tag use with no code) 152 (535 bytes; tag still EDI, spatialization spilled); the late
// uses (call, started test, dispatch) through a `long &` binding of the parameter 116 (folds); the request declared at
// the slot block's scope 116; spatialization assigned in an if/else and positioned after it 154 (541 bytes); positioned
// held in a char 116. In the plain listing the request arm stores the object's EBP for its NONE field and the dispatch
// reloads the tag from its home (EDI goes to 0x189340's source); retail never enregisters the object index.
// M3 lane 4 r214 (plain, with the five matched callers and seven matched callees; every form with no reference,
// all object reads plain): control 116 (531 bytes, frame 0xa4, aligned 67 / masked 14); the started test nested 116;
// positioned as `spatialization ? (spatialization->flags & 1) != 0 : false` 137 (530 bytes); the return test on the
// stored slot field 116; the request test as `!(object_index != NONE || positioned)` 116; `sound_index = NONE` at its
// declaration 116 (all but the ternary the same code). The kept body's one difference (the started test reading the
// parameter's home) comes from its volatile read; a match has to come from a plain form.
// M3 lane 6 r197 (plain): 0xb8d30's object argument read through the reference with the started test plain 75 (528 bytes);
// no reference, one `bool attached` for the request, marker and started tests 107 (530 bytes); the unused fourth and fifth
// parameters unnamed 11 (same code); `long audible = positioned ? 2 : 1;` named at the arm's top 136 (530 bytes); the
// marker declared at the arm's top 11 (same code). Diagnostic: on the plain-reads body (no reference), 1 to 7 dead
// initialised locals at the top 116 each, so the EBP contest between the object and the tag is not a symbol-count state.
// M3 lane 2 r109 (plain; every object read plain, no reference; control 116 at 531 bytes): a named `attached` test for the
// request test only 116 (same code); `attached` for the marker test only 134 (536 bytes); `positioned` written out at the
// request test and at the audible argument (no local) 131 (528 bytes); the duration reset after the blocks tested on a
// `started` bool set from the sound result 122 (536 bytes); the spatialization member bound to a reference before its flag
// test 128 (526 bytes, frame 0xa4). The object keeps EBP and the tag EDI in each.
// M3 lane 14 r115 (plain; every form without the reference, object reads plain, control 116, 531 bytes): the tag lookup,
// sound_class row and spatialization through a tag-taking forceinline helper (if-form NULL start) 116; a named `attached` for the
// request test alone 116; the description bit through a SET_FLAG-style conditional macro 116; the slot lookup outside the
// tag test (`slot_index = NONE; if (tag_index != NONE) slot_index = ...;` then one slot test) 116, as a ternary 116 (all
// the same code: tag EDI, object EBP, spatialization spilled to [esp+0x18]).
// M3 lane 10 r130 (plain; control 11): the object index as a by-value one-member aggregate parameter (declarations and
// the five calls through a maker, every read plain; diagnostic) 116 (531 bytes: promoted like a long, the plain-reads code;
// callers 0x2760a0 58, 0x2a9e00 7, 0x2a9e60 5, 0x2a9ed0 10); every read plain with one real early object use at the top
// (value-wrong diagnostic) 160 (535 bytes: the tag then takes EBP as retail, but the object holds ESI for the whole
// function, so a longer object range alone does not demote it); every read plain with `__assume(tag_index != NONE)` before
// the started dispatch 152, before the request arm's 0x189fe0 call 152, at both 152 (535 bytes each: tag EDI, object EBP).
// M3 lane 15 r114 (plain; every form without the reference, object reads plain, control 116 at 531 bytes): the object
// index, the scale and the two unused words as one 16-byte by-value aggregate parameter built by the five callers
// (diagnostic for a memory-resident object index) 116 (scalarised like the plain parameters; all five callers lose their
// match); the spatialization pointer bound to a const reference for the description test 116; two sound_class locals (one for
// the spatialization flag test, one for the address) 116; the positioned arm's object reads through a const reference
// bound at the arm's top 116; the tag's reads after the slot lookup through a const reference 116 (all the plain code).
// M3 lane 7 r192 (plain; every form without the reference, object reads plain, control 116 at 531 B): no duration local
// (both uses through the parameter) 116; `bool attached` (request and marker tests) declared just before the clamp 107
// (523 B: the first 0x92 bytes are then retail's, tag EBP, spatialization EDI, and the late half is retail's; only the bool's
// `cmp esi, -1; setne al` and its two `test al, al` remain, the request's zero bytes stored from AL); that bool declared at
// the slot block's top 153 (537 B); the bool before the clamp also for the started test 117 (530 B); a plain `long` copy of
// the object used only by the two early tests 116 (folds back into the parameter).
// M3 lane 4 r231 (plain, with the five matched callers and 0x109220/0x189340/0x1892a0/0xb8d30; every form without the
// reference, object reads plain; the reduced image builds this function differently, 94): the playback call, started test and
// dispatch in a forceinline helper taking the object, tag, positioned flag, description, scale, label and slot 154 (528 bytes,
// frame 0xa4, tag EDI); the started test inside a forceinline notify helper around 0x109220 116; `__assume(object_index !=
// NONE)` at the top of the marker block 152, before the dispatch 152; `__assume(object_index == NONE)` in the request arm
// 152 (535 bytes each). All restored.
// M3 lane 12 r158 (plain; every form without the reference, object reads plain, control 116 at 531 bytes): `bool
// attached` at the positioned arm's top for the marker test and the started test 162 (540 bytes); the started test and
// dispatch moved out of the positioned arm, after both arms (the request arm's object is NONE, so it never passes) 109 (533
// bytes), that with the object compared first 118 (531 bytes); the request's object field set from the parameter (NONE
// on that path) 116 (same code: folded); the clamp as two separate tests 116 (same code). Reading: the object's EBP win
// follows its uses packed into the second half (fewer blocks than the tag spans); moving the started test to the join
// lengthens the object's range but adds the request path's compare.
// M3 lane 15 r117 (plain, with the five matched callers, all matched each time): the compare's operand side, on the
// plain-read form (no reference): the started test `NONE != object_index` 116, the request test `NONE == object_index`
// 116, the marker test `NONE != object_index` 116, all three 116 (531 B, the plain-reads code); on the kept form the
// request and marker tests constant-first 11 (same code). The operand side does not move the object/tag contest for EBP.
// M3 lane 5 r221 (plain; every form without the reference, object reads plain, control 116 at 531 B). Diagnostics: one
// conditional store (`if (a == 5) description.value24 = b;`) at the positioned arm's top 93 (542 B) is retail's code exactly
// plus its own four instructions, two such stores there 109, two unconditional stores 150 (so a basic block, not a use);
// the same block ahead of the clamp 171, in the request arm 175, after the marker block 144, inside the marker block 181;
// an empty `if` there 116 (folds), `__assume(slot != NULL)` there 152. Forms: `long audible = 1; if (positioned) audible = 2;`
// at the arm's top 136 (533 B); `audible` set by if/else just before the call 122 (533 B); the marker test under an implied
// `if (spatialization || object_index != NONE)` 160 (560 B); a repeated `if (object_index != NONE)` around the 0xb8d30 call
// 116; the top `flags = 0` kept with a redundant `else { description.flags = 0; }` 128 (538 B); `flags = 0` written in both
// arms of the marker test 34 (527 B), arms swapped (`if (object_index == NONE) { flags = 0; } else { flags = 0; marker }`) 20
// (523 B); `flags = 0` moved after the marker block 105 (500 B, folded into the bit). Reading of the 20/34 forms: retail's
// allocation and late half everywhere (tag EBP, spatialization EDI, late object EBX copied to ESI, `cmp ebx, -1`); left: the
// two stores are not merged into retail's one store at the arm's top, and the request test's edges are threaded past the
// marker compare (object edge into the marker block, positioned edge to the no-object store), so that compare, one object
// use, is what retail's allocation does not count while its code still has it.
// M3 lane 8 r208 (fast with the eight callees listed, which reproduces the plain 11 and the plain-reads 116; every form
// without the reference, object reads plain, control 116): the lookup in 0x18c250's form (tag view `.data`, a class-index
// local, a classes local, the existing spatialization helper) 154 (541 bytes); a second tag lookup for the spatialization flags test
// in the same block (CSE'd away; diagnostic for a pre-CSE tag use) 118 (530 bytes, tag still EDI); an early object copy
// bound by a const reference for the early tests and marker calls 116; the late uses (call, started test, dispatch) through
// a const reference bound to the parameter 116; positioned read through the spatialization bit-field 116 (same code each).
// M3 lane 8 r210 (fast with the eight callees; control 11, plain-reads control 116): `positioned` computed after the clamp
// with every object read plain 141, on the kept form 146 (536 bytes); the spatialization pointer `const` (plain reads) 116
// (same code); the request arm's result chained into the slot store (plain reads) 127 (534 bytes); the started test read
// through the reference with its volatile cast away (`*(long const *)object_reference`) 116 (the address folds, object EBP).
// M3 lane 16 r159 (plain, with the five matched callers, all matched each time; on the kept form): the started test
// written `NONE != *object_reference` 11 (same code); an early `if (tag_index == NONE)` exit with its own duration reset
// in place of the outer tag block 11 (same code, the resets merge); the playback label call named before 0x189340 16
// (526 bytes); the audible argument as `positioned + 1` 40 (521 bytes: the flag is read with `movzx` and the late
// object load moves ahead of the push, where retail reads the flag into BL first); the audible argument as an if-set
// `long audible` 37 (532 bytes).
// M3 lane 8 r213 (H2M_FAST with the eight callees; kept-form control 11, plain-reads control 116): every form below with no
// reference and every object read plain: the audible argument as `positioned + 1` 110 (525 bytes); `long audible =
// positioned ? 2 : 1;` at the positioned arm's top 131 (525 bytes); `long audible = 1; if (positioned) audible = 2;` there
// 136 (533 bytes); `bool attached` (request and marker tests) before the clamp with `positioned + 1` 106 (521 bytes, retail's
// length and allocation: left are the bool's `setne al`/`test al, al`, the request's zero bytes stored from AL, and the
// flag read by `movzx` with the late object load ahead of the push); the flag reset and marker block in a forceinline
// helper with an early return on NONE 116 (same code); the marker block split into two `object_index != NONE` tests
// (0xb8d30 under one, 0x1892a0 under the next) 116 (same code); `bool attached` declared after the clamp 96 (523 bytes).
// @retail 0x189cd0 standard
void __stdcall function_189cd0(long tag_index, long object_index, real scale, long a, long b, long label, long duration_address)
{
	long const volatile *object_reference = &object_index;
	real *duration = (real *)duration_address;

	if (tag_index != NONE)
	{
		long slot_index = function_189ee0(tag_index);
		if (slot_index != NONE)
		{
			long sound_index;
			s_looping_sound_slot *slot = &g_4ed288->slots[slot_index];
			if (duration)
			{
				*duration = sound_permutation_reference_duration(tag_index, &slot->permutation);
			}
			s_sound_tag *tag = (s_sound_tag *)g_4e3b44[tag_index & 0xffff].bytes;
			s_sound_class_view *sound_class = &((s_sound_globals_view *)g_51ebd4)->classes[tag->class_index];
			s_sound_class_spatialization *spatialization = NULL;
			if (sound_class->spatialization.flags & 7)
				spatialization = &sound_class->spatialization;
			bool positioned = spatialization && (spatialization->flags & 1);

			if (scale < 0.0f)
				scale = 0.0f;
			else if (scale > 1.0f)
				scale = 1.0f;

			if (object_index == NONE && !positioned)
			{
				s_sound_request request;
				request.location.unknown02 = 0;
				request.location.flags = 0x400;
				request.location.audible = 0;
				request.location.requested_audible = 0;
				request.location.scale = scale;
				request.location.unknown08 = 0;
				request.object_index = NONE;
				request.platform_playback = function_18d5b0(label);
				request.variant = (char const *)&slot->permutation;
				request.marker = NULL;
				request.source = &g_444b3c;
				sound_index = function_189fe0(&request, tag_index);
			}
			else
			{
				s_sound_source_description description;
				description.flags = 0;
				if (object_index != NONE)
				{
					s_object_marker local_4f80fe;
					function_b8d30(object_index, 0x4000095, &local_4f80fe, 1, false);
					function_1892a0(local_4f80fe.node_index, &local_4f80fe.node_matrix.position,
						&local_4f80fe.node_matrix.forward, tag_index, object_index, &description);
				}
				if (spatialization)
					description.flags |= 1;
				else
					description.flags &= ~1;
				sound_index = function_189340(object_index, tag_index, positioned ? 2 : 1, (s_sound_marker *)&description,
					&g_444b3c, scale, function_18d5b0(label), (char const *)&slot->permutation);
				if (sound_index != NONE && *object_reference != NONE)
				{
					object_dispatch_sound_started(object_index, tag_index, sound_index);
				}
			}
			slot->source_index = sound_index;
			if (sound_index != NONE)
				return;
		}
	}
	if (duration)
	{
		*duration = 0.0f;
	}
}
