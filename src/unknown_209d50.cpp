// @flags /O2 /Gr
/* UNKNOWN_209D50.CPP: evaluates the arguments of a script function, one per
   call, into the thread's current frame (script_thread_runner; the front end of lane
   A's script functions) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "hs.h"

/* a frame of a thread's stack (unknown_209520.cpp): its data follows the
   size at +0xe */
struct s_hs_macro_frame
{
	s_hs_macro_frame *next;
	long expression_index;
	long *result;
	short size;
	byte data[2];
};

struct s_hs_macro_thread
{
	byte unknown00[0x10];
	s_hs_macro_frame *frame;
	byte unknown14[0x418 - 0x14];
};

/* an expression node (g_4f9394, 20 bytes) */
struct s_hs_macro_expression
{
	short salt;
	short value_type;
	short type;
	byte flags;
	byte unknown07;
	long next_index;
	byte unknown0c[4];
	long value;
};

extern s_record_pool *g_4f9384;
extern s_record_pool *g_4f9394;

void function_2099f0(long thread_index, long *result, long expression_index); /* unknown_209520.cpp */

static inline s_hs_macro_thread *hs_macro_thread_get(long thread_index)
{
	return (s_hs_macro_thread *)(g_4f9384->data + (thread_index & 0xffff) * sizeof(s_hs_macro_thread));
}

static inline s_hs_macro_expression *hs_macro_expression_get(long expression_index)
{
	return (s_hs_macro_expression *)(g_4f9394->data + (expression_index & 0xffff) * sizeof(s_hs_macro_expression));
}

/* room in the thread's current frame */
static inline void *function_xc5914d(long thread_index, short size)
{
	s_hs_macro_frame *frame = hs_macro_thread_get(thread_index)->frame;
	short *frame_size = &frame->size;
	void *result = frame->data + *frame_size;

	*frame_size += size;
	return result;
}

// score 12 (was 12): retail copies the argument pointer into EAX and returns it in a
// register, ours reloads the result from the stack. Tried: no volatile 95 (one register for
// both, no spill); result NULL first and set on the else path 90; result declared first
// and assigned after the allocation 95; the slot address passed straight to the call 21;
// the result cleared before the call 31. Retail stores the pointer to its slot and copies
// the live register, as for an address-taken local.
// Round 11 (lane 18), all plain checks: the result taken from the allocation and the volatile pointer
// copied from it 71 (277 bytes; the pointer is computed straight into EAX, retail computes it in EBX
// and copies it); no volatile with the result cleared first in the success block 83 (277 bytes: no
// spill and the expression pool is no longer held in EBX); no volatile with the allocation helper
// reading the frame size without its size pointer 95 (266 bytes); no volatile, the result copied after
// all three allocations and cleared before the call 83; no volatile and no frame-reference local 78
// (278 bytes: the spill goes to the index pointer instead); no volatile with the argument index read
// into a local for the test and the element 95 (266 bytes); no volatile with the result kept as void *
// 95 (266 bytes).
// Round 116: retail computes the pointer in EBX while the frame holds EAX, stores it to its slot and later copies
// EBX into EAX (a spill plus a register copy: result and the pointer are separate live ranges); ours reads the
// slot back. `result = arguments = allocation` with the volatile 12 (same code); no volatile with the result
// cleared before the call and the element passed directly 83; the volatile with the element passed directly
// (no argument local) 21; an early `return NULL` and `return arguments` at the end, with/without the volatile
// 88/90, without it returning a result copy 90. Diagnostics: the pointer's address taken (`(void)&arguments`,
// or a pointer to it used for the element) without the volatile 95/95: VC still promotes it.
// Round 58 (m3 lane 11, fast): the result copy after the other two allocations 93 (292 bytes); `return NULL`
// in the evaluated arm with `return result` at the end 12 (identical); the element taken from `result` 32
// (277 bytes); the frame reference declared after the result copy 25 (281 bytes); the type test nested in
// the count test 12 (identical).
// Round 130: result declared first and assigned after the second allocation 90 (293 bytes); direct allocation result
// 71 (277 bytes, allocation/result still coalesce in EAX); one-pointer array 95 (266); allocator parameter order 71;
// result before the frame 71; frame field reference 71; wide allocator size formal 71; one-pointer storage record 95
// (266). Both wrappers are promoted and lose the spill; byte-return direct allocation 71. Original 12 form restored.
// Round 61 (fast): the listing differs only in `mov eax, ebx` against ours' reload, so retail's home is a non-volatile memory value the store forwards. The allocator
// with an out-parameter stored before the size update 95 (266, promoted); a const pointer 95; the allocation named with the volatile and the result copied from it
// 71/71 (277: the allocation takes EAX, the frame ESI), in one declaration 71; the result copy after the second allocation 71, no volatile 95; the volatile home after
// the second allocation 71; the frame reference after the first allocation 25 (281); a done flag with a ternary return 102 (291, the pointer does spill unforced), an
// if on the flag 90; `else return arguments` 90.
// Round 89 (fast): no volatile with `long *const &arguments` bound to the allocation 95 (266: the temporary folds), from a void local 95, declared before the frame 95; an unused const pointer to
// the local and a reference local to it (diagnostics) 95/95, still promoted; the allocation as dword 95, as byte * 95; result from the allocation with a const reference for the element 95, a const-reference
// base beside plain arguments 95; no volatile with the result cleared right after the call 93 (268).
// M3 lane 11 r76 (plain): the volatile kept without the frame-pointer local 112 (306 bytes); no volatile, no frame
// local and the result cleared first in the evaluated block 74 (280); no volatile with the result set in an else
// arm 90 (277); the allocator bumping the frame size directly 12 (same code), that with no volatile and the result
// cleared first 83 (277).
// Round 80 (m3 lane 10, fast): the only difference is `mov eax, [esp + 0x10]` (the volatile reload, 4 bytes) for retail's `mov eax, ebx` (2).
// Without the volatile, sixteen statement orders (the frame reference before or after each allocation crossed with the result copy after each
// allocation or after the initialisation block): the copy's place never matters (it coalesces with the pointer); the frame reference first or
// after the first allocation 95 (266 bytes), after the second 98 (264), after the third 68 (280: the pressure spill moves to the argument-index
// pointer); the result as a long integer 95; the frame reference after the allocations with `return NULL` in the arm and `return arguments` at
// the end 90 (290).
// Round 177 (m3 lane 4, fast): retail's reload in the evaluated arm goes into EAX while EAX still holds the same pointer (nothing
// writes EAX between the copy and the reload), so retail's build treats the result (EAX from the copy on, kept clear of every
// temporary up to the return) and the argument pointer (stack home, loaded again) as two values, and the early copy is the
// home's load forwarded inside its block. Without the volatile our build merges the two into one register. New: one variable
// for both (`arguments = NULL` in the arm, `return arguments`) 95 (266 bytes); the arm stepping the pointer (`arguments +=
// argument_index`) and passing it 95; the allocation stored through `*(void **)&arguments` 95; the result read as
// `*(long *const *)&arguments` 95; the arm's element taken through that cast 95 (all promoted, 266 bytes). Re-measured: the
// result copy declared after the three allocations 95; result cleared first in the arm 83 (277: the pointer is computed
// straight into EAX and the two still merge); the result set in an else arm 90 (277: the pointer in EDX, the frame reference
// in EAX, no home).
// Round 85 (m3 lane 14, fast): a thread-pointer local in place of the frame-field reference 12 (same code); with the
// volatile, `result = NULL` first in the arm 105 (301 bytes), the element passed directly 105 (same code); the result
// copy declared just before the test 90 (270); `result = NULL` between the call and the expression update 23 (280);
// the frame reference declared after the three allocations 118 (308).
// Re-measured, not counted: the type test nested inside the count test compiles the same as the `&&` test in every
// form; without the volatile, `result = NULL` at the start of the arm or before the call gives 83 at retail's 277 bytes
// (no frame slot: our build keeps the frame in ESI, the pointer in EAX, and loads the expression pool in each block
// where retail holds it in EBX across the initialisation block and the test), at the end of the arm 95 (266).
// Round 87 (m3 lane 14, fast; volatile-free forms only, since a match cannot keep the volatile): control 95 (266);
// `result = NULL` first in the arm 83 (277 bytes, no stack home: the frame is read straight into ESI and the pointer
// computed in EAX, where retail reads the frame through the frame reference into EAX and computes the pointer in EBX),
// after the element pointer 83, with the element passed directly 83, with the result copy after the three allocations
// 83, with a thread pointer local for the frame reference 83; null-first with no frame local 74 (280).
// Round 157 (m3 lane 7, fast): the type test with its operands the other way round (`parameter_types[argument_index] ==
// ...->type`) 96 (300 bytes); the count test as `parameter_count > argument_index` 14 (279 bytes). The volatile-free control
// re-measured 95 holds the expression pool in EBX across the initialisation block and the test as retail does, so the pool's
// loads are not the open cause; the pointer is computed straight into EAX (the returned result) and the frame goes to ESI,
// where retail reads the frame through the frame reference into EAX and computes the pointer in EBX. Stopped after two: the
// result/pointer spellings are enumerated in rounds 80, 87 and 177 above.
// Round 105 (m3 lane 10, fast; volatile-free forms): the test inverted with an early `return arguments` and `return NULL`
// at the end 90 (277 bytes, no spill); the same with the count and type tests as two early returns 95 (266); the first
// allocation written through the frame reference (0x208310's count form) with the result copy after it 95 (266), with
// 0x207cf0's size local 95 (266); the failing test assigning `result = arguments` in its arm and the evaluation in the
// else arm 90 (277: two epilogues, the pointer kept in EDX and copied to EAX at the return, frame reference in EAX).
// Reading: retail needs the result (EAX) and the pointer (spilled) as two interfering values; every copy form here
// coalesces them or defers the copy to the return.
// Round 125 (m3 lane 9, fast; volatile-free forms only): the expression pool read once into a local for the
// initialisation block, with `result = NULL` first in the arm 83 (277 bytes); a const result copy with `return NULL`
// in the arm 90; the element as `arguments + argument_index` with `result = NULL` first 83; the result copy declared
// just before the initialisation test with `result = NULL` first in the arm 83; the element passed straight to the
// call 95 (266 bytes). Every volatile-free form still gives the result and the pointer one register.
// M3 lane 9 r127 (fast; volatile-free forms): the init block's lookups as named expression/name pointers 95 (266 bytes),
// with `result = NULL` first in the arm 83 (277); the frame reference declared after the result copy, null first, 83; the
// element taken from `result` 95; the frame reference from a thread pointer local 95. Diagnostic: `&arguments` passed to
// an allocator defined in another file, so only LTCG inlines it, 95 (still promoted after the cross-module inline).
// M3 lane 8 r173 (fast; volatile-free forms): the result copy after the second allocation with `result = NULL` first in
// the arm 83 (277 bytes); the two index slots as plain pointers with `result = NULL` first 83; `long *result;` declared
// first and assigned after the third allocation, null first 83; the element as `arguments + argument_index` with `result
// = NULL` between it and the call 83; `long *result;` declared above the frame reference and assigned right after the
// first allocation 95 (266). In each null-first form the frame goes to ESI and the pointer to EAX from the first block on.
// M3 lane 12 r131 (fast; volatile-free forms, control 95 at 266 bytes): the first allocation read through `*frame` with a named
// frame pointer (0x208310's form) 95, with a `short size` local (0x207cf0's form) 95, with the reference declared `*const *` 95;
// the `*const *` reference alone 95; the frame field bound as a reference and the first allocation through it 95 (all the same
// code). Our frame load stays folded into `[eax+ecx+0x10]` ahead of the reference's lea, so the frame takes ESI and the pointer
// EAX; retail and the matched siblings 0x208310 and 0x207cf0 load the frame through `[edx]` after the lea, which leaves retail's
// frame in EAX and the pointer in EBX (spilled). The `result = NULL` first form (83, 277 bytes) has the same folded frame load.
// Round 171 (m3 lane 7, fast with 0x2099f0; volatile-free forms): the first allocation written out as the matched 0x208310 does
// (`frame = *frame_reference;` with a const reference, the pointer from `frame->data + frame->size`, then `frame->size +=`) 95
// (266 bytes: the frame load still folds into `[eax + ecx + 0x10]` and goes to ESI, the pointer to EAX), with `result = NULL`
// first in the arm 83 (277); 0x207cf0's `short size` form with `result = NULL` first 83 (277, same frame fold); the allocation
// named `result` and copied into `arguments` 95; the arm's evaluation through an inline helper taking the arguments pointer 95.
// Reading: retail's frame takes EAX (loaded through EDX), which keeps the pointer out of EAX, so the pointer (EBX, spilled) and
// the result (EAX) are two webs; in every volatile-free form here the pointer and the result share EAX and the frame folds.
// Round 116 (m3 lane 1, fast with 0x2099f0; volatile-free forms): `result = arguments = allocation` chained 95 (266 bytes);
// `long *&result = arguments` 95; the arm as an inline helper returning NULL with `return helper(...)` in the arm and
// `return arguments` at the end 90 (277 bytes; pointer EDX, frame ESI); the two index slots as plain pointers with the
// result copy after the allocation 95; the result copied in the init block and in an else arm 87 (278 bytes: the frame
// then loads through the reference, `lea ebx; mov ecx, [ebx]`, as retail's does, but into ECX, with the pointer in EDX).
// M3 lane 10 r117 (fast with 0x2099f0; volatile-free forms): the allocation stored through a two-member aggregate overlay
// (void pointer and long pointer) and read through the other member 95 (266 bytes, promoted); the frame reference after
// the three allocations with `result = NULL` first in the arm 79 (282), that with the result copy after the three
// allocations 79 (282); the frame reference and the result copy both after them 68 (280, re-measured); no result local,
// `return NULL` in the arm and `return arguments` at the end 90 (277). Re-measured: `result = NULL` first in the arm 83.
// M3 lane 6 r192 (fast with 0x2099f0; volatile-free forms): the result copy as `&arguments[0]` 95 (266 bytes), through
// `(long *)(void *)arguments` 95 (266), `&arguments[0]` with `result = NULL` first in the arm 83 (277); the allocator
// `__forceinline` with null first 83, with the volatile home 12 (same code); both pool getters `__forceinline` with null
// first 83. Re-measured: `result = NULL` first in the arm 83.
// M3 lane 9 r141 (fast with 0x2099f0): the expression pool read once into a named local used by every lookup (init block,
// type test, arm update), volatile-free with `result = NULL` first in the arm 93 (268 bytes), volatile-free with the
// result cleared last 95 (266), with the volatile home 29 (279); volatile-free null-first with the count test written
// `parameter_count > argument_index` 83 (277, same code), with the type test's operands swapped 89 (285); volatile-free
// with the count test swapped 95 (266). In the null-first form the pointer takes EAX for the whole function (it feeds the
// return), the NULL goes to ESI and is copied at the end, and the pool is reloaded in each block, so nothing presses the
// pointer into a spill; retail's result is a copy that is never propagated into the return.
// M3 lane 12 r142 (fast with 0x2099f0, which stays matched; volatile-free forms; the kept volatile body re-measured 12
// in the baseline): a grid of the allocator's own locals against the body's result form, to see whether the set of
// inlined locals decides the spill (one shared loop index did for 0x12eb00's homes). Body forms: the result copy at
// the top / one variable set to NULL in the arm / `result = NULL` first in the arm / two returns / the result set in an
// else arm. With the current allocator (frame, size pointer, result) 95/95/83/90/90; without the size pointer the
// same five; with a thread pointer local as well the same five; with the result started at the frame's data and
// stepped by the size the same five; with a size local and no result local (`frame->size = offset + size; return
// data + offset`) 92/92/83/92/92 (264, 264, 272, 264, 264 bytes); with no frame local (the frame read at each use)
// 112/112/106/103/112 (248-256 bytes). No form spills the pointer: the allocator's locals do not decide it.
// M3 lane 5 r207 (fast with 0x2099f0, which stays matched; volatile-free forms; control 12, volatile-free 95 at 266 B):
// the arm stepping the pointer to the current slot (`arguments += argument_index`) with the result kept separate 95,
// that with `result = NULL` first in the arm 83 (277 B); the result taken from the frame data ahead of the allocation 115
// (298 B); the arm as a forced-inline step returning whether it evaluated, `if (step(...)) result = NULL;` 95; `result =
// NULL` right after the evaluation call 93 (268 B). Re-measured: a done flag with a ternary return 95 (284 B; the
// convention then takes the parameter count in EDX, `ret 0xc`, and the frame loads through the reference in EBX).
// Reading: retail's frame takes EAX because the result copy is made only after the second frame load; every volatile-
// free form gives the pointer and the result one web, so the pointer takes EAX and the frame ESI.
// Round 116 (m3 lane 13, fast with 0x2099f0; volatile-free forms, control re-measured 95 at 266 bytes): a `long *const *`
// to the pointer with the element read through it 95; the result filled by `memcpy(&result, &arguments, sizeof(result))`
// 95; `(void)frame;` ahead of the initialisation test 95; the pointer declared `long *const` 95; the size as
// `(long)parameter_count * sizeof(long)` 95 (12 with the volatile, the same code). All promoted into one register.
// M3 lane 10 r130 (fast with 0x2099f0; volatile-free forms): a dead `(void)&arguments;` 95 (266 bytes) and the pointer held
// in a one-element array 95 (both promoted like a scalar, diagnostics); `register` on the result copy 95; the pointer declared
// `long *const` 95. The first difference of every volatile-free form is the first allocation's frame load: ours folds it as
// `mov esi, [eax + ecx + 0x10]` ahead of the reference's lea and gives EAX to the merged pointer/result, where retail loads
// the frame through the reference into EAX, so its result copy can take EAX only after the frame dies. A permuter run from
// the volatile-free body (180 tries, 2100 s) found nothing below 70 (full-image baseline 86).
// M3 lane 15 r114 (fast with 0x2099f0, 0x2084c0, 0x208570; volatile-free forms, testing whether a CSE copy
// (two locals with one initialiser) keeps the result and the pointer apart): the result taken from a frame-top
// read (`frame->data + frame->size`) ahead of the allocation 120 (303 bytes: the two size loads are not merged,
// both pointers spill, `ret 0xc` with the thread index in EAX, and 0x2084c0/0x208570 lose their matches in the
// reduced image); that read written through the allocator's size pointer 120 (same code); the allocator taking its
// result from that read 85 (284 bytes, still two size loads); the first allocation written out in 0x207cf0's
// `short size` form with the pointer and the result both initialised from `data + size` 94 (264 bytes: CSE merges
// the two into one lea, which goes to EAX for both, no copy, no spill), with the result declared first 94 (same
// code). The 16-bit size loads are never merged, and a merged register initialiser is coalesced like a named copy.
// M3 lane 7 r192 (fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): the element pointer
// computed ahead of the count/type test 95; `result = NULL` between the expression update and the index increment 95; the
// pointer named `parameters` 95 and the result named `values` 95 (names do not decide the merge); the element ahead of the
// test with `result = NULL` first in the arm 83 (277 B, the null-first shape); the type test nested under the count test
// with the expression node named 95. Every form still gives the pointer and the result one web in EAX.
// M3 lane 4 r231 (fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): `long *result = NULL;`
// at the top with `result = arguments;` after the first allocation 95; that assignment after the third allocation, just before
// the initialisation test 95; the frame reference as a `s_hs_macro_frame *&` bound to the thread's field, the initialisation
// reading through it 95; the result copied through a `long *const &` binding of the pointer 95; the frame reference declared
// after the result copy 95 (all 266 B: the pointer and the result still share EAX, no spill). All restored.
// M3 lane 12 r158 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): the three
// allocations in an inline helper returning the arguments block and handing back the two index slots through out-pointers
// 124 (237 B); the parameter count read once into a `long count` for the size and the count test 87 (280 B); `result =
// NULL` first in the arm with the index increment ahead of the expression update 83 (277 B); `long *result;` assigned
// from the arguments after the initialisation block, just before the test 95; both pointers initialised NULL at their
// declarations and assigned afterwards 95. Re-measured: the argument index read into a local for the test and the
// element 95. In the null-first shape the expression pool is reloaded per block (not held in EBX), so nothing presses the
// pointer into retail's spill.
// M3 lane 15 r117 (fast with 0x2099f0, which stays matched; volatile-free forms): the expression pool read once into a
// local after the allocations for the init block and the type test 95 (266 B); the allocation named `result` and copied
// into the arm's pointer, which is then cleared before the call, 83 (277 B: the pointer in EAX for the whole function,
// the NULL in ESI copied at the end); a redundant `result = arguments;` at the top of the init block 108 (305 B), just
// before the count/type test 95; the result returned through `long *const &kept = result` 95. Diagnostics: the
// volatile-free body built /O2 /arch:SSE /Gr 95 and /O2 /Ob1 /Gr 95 (the file's flags do not decide the merge). Retail
// never writes EAX between the early copy and the arm's reload of the home into EAX, so its result and pointer are two
// variables where every volatile-free form here makes them one.
// M3 lane 5 r221 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): the init block's frame expression index
// read into a named local 95 (266 B); result NULL first and set to the pointer in an else arm of the test, with that local, 90
// (277 B); the init block reading the frame through a re-fetched thread pointer instead of the frame reference 78 (278 B: the
// spill moves to the argument-index pointer and the three allocations rotate); `long *result;` assigned from the pointer just
// before the count/type test 95 (266 B); the argument index widened once into a `long index` for the compare, the type lookup
// and the element 96 (265 B).
// M3 lane 3 r285 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): the result set
// in an else arm of the evaluation test with the frame reference declared after the three allocations 90 (290 B); two
// returns (`return NULL` in the arm, `return arguments` at the end) with the frame reference after the allocations 90
// (290 B); the result copied in both arms of the initialisation test with `result = NULL` first in the evaluation arm 92
// (280 B), with the frame reference after the allocations instead 96 (287 B); a thread pointer local with the init block
// reading its frame field directly (no frame reference) and `result = NULL` first in the arm 83 (277 B).
// M3 lane 16 r159 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): the frame reference declared after
// the second allocation, the element taken from `result` and `result = NULL` between them 89 (272 B); the result copy made
// after the initialisation block with `result = NULL` first in the arm 83 (277 B); `long *result;` declared first and
// set only in an else arm of the test, NULL first in the arm 89 (277 B); the parameter count read once into a `long
// count` for the size and the test, NULL first 90 (282 B), into a `short count` 83 (277 B). Every form keeps the first
// allocation's frame load folded (`mov esi, [eax + ecx + 0x10]`) and has no spill slot.
// M3 lane 16 r161 (h2m fast with 0x2099f0, which stays matched): with the volatile home, the result copy made just before
// the initialisation test 93 (292 B), declared after the second allocation 90 (293 B), a `long *const` result with the
// arm returning NULL 12 (same code); volatile-free, a `long *const` result with the arm returning NULL 90 (277 B), the
// size passed as a short product with `result = NULL` between the element and the call 83 (277 B).
// M3 lane 6 r220 (h2m fast with 0x2099f0, which stays matched; volatile-free forms with the result copy at the top and
// the arm's `result = NULL` last, control 95 at 266 B): the allocator `__forceinline` 95; the allocator bumping `frame->size`
// directly (no size pointer) 95; the element as `arguments + argument_index` 95; `register` on the pointer 95; the
// allocator's size formal widened to `long` 95 (all the same code: the pointer and the result still share EAX, no spill).
// M3 lane 13 r136 (h2m fast with 0x2099f0, MATCH; volatile-free forms, control 95 at 266 B): the size as `sizeof(long) *
// parameter_count` 95; the init block's frame through a `*const &` bound to `*frame` 95; arguments and result declared apart
// from their assignments 95; a named thread pointer for the frame reference 95 (all four the same code); the frame value read
// once at the top for the init block 101 (260 B).
// M3 lane 15 r125 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): the result copied
// at the top of the init block and in an else arm 87 (278 B), at the end of the init block and in an else arm 89 (278 B);
// `__assume(arguments != NULL)` after the allocation 95 (same code); the type mismatch as an early `return result;` inside the
// count test 83 (284 B, first difference +0x22: the pointer gets the spill slot but stays in EAX with no separate result, and
// the second epilogue reloads it).
// M3 lane 16 r169 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): the expression pool read once into a
// local for the init block and the type test, the arm re-reading the global, `result = NULL` first in the arm 93 (268 B);
// the count test and the type test as two nested ifs with the argument index read once into a `short index` for the type
// lookup and the element and stored back as `index + 1` 95 (290 B); the argument-index slot as a plain pointer with the
// expression-index slot kept a reference 95 (266 B), that with `result = NULL` first in the arm 83 (277 B); the expression-
// index slot as a plain pointer with the argument-index slot kept a reference 95 (266 B).
// M3 lane 16 r171 (h2m fast; volatile-free forms, control 12, volatile-free 95 at 266 B): the arm's element read through
// the pointer's own address, `(*(long *const *)&arguments)[argument_index]` 95, and as `(long *)*(dword const *)&arguments
// + argument_index` 95; the allocation stored through a type-punned `*(void **)&arguments` 95 (all promoted to one register
// with the result, no spill); the parameter types copied into a local at the top 95 (same code); `long *result =
// arguments = allocation` with a plain pointer 95. VC promotes every non-volatile address form tried here.
// M3 lane 1 r151 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): `if (!result) return NULL; return
// arguments;` 103 (286 B); the count test as an early `return result;` ahead of the type test 83 (284 B); the test held in
// a named `evaluate` bool 95 (266 B); the tests nested with `result = NULL` first in the inner arm and the element passed
// straight 83 (277 B); re-measured: `result = NULL` first with the element passed straight 83, the copy declared just
// before the test 95.
// M3 lane 6 r227 (h2m fast with 0x2099f0, which stays matched; control 12): one, two and three redundant parenthesis
// pairs around the first allocation's size on the volatile-free body 95/95/95 and on the null-first body (`result = NULL`
// first in the arm) 83/83/83 (no paren phase here); the first allocation in matched 0x208310's spelling (the frame read
// through the frame reference, `top->size += ...` in place of the allocator call): with the volatile home 21 (279 B),
// volatile-free 95, volatile-free null-first 83; the same with the size read into a `short size` first 21 / 95 / 83. In
// the null-first listing the expression pool is rematerialised per block (`mov ecx, [pool]` in the init block, again in
// the test), the pointer keeps EAX and the result ESI: the pool temp, not the pointer, loses the register contest.
// M3 lane 4 r249 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, control 95 at 266 B): the three slot
// pointers kept in a local three-member record 95 (promoted); the allocation named `result` with the pointer copied from
// it 95; no frame local with the result copy after the second allocation 78 (278 B, as without the copy move).
// Diagnostics (a probe global taking the pointer's address, reverted): copy at the top 83 (287 B: the pointer gets its
// stack home but is still computed straight into EAX and the frame load still folds), copy after the second allocation
// 118 (302 B, the convention changes). A memory home alone does not give retail's EBX pointer and late `mov eax, ebx`.
// M3 lane 7 r214 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): the three slots as plain pointers
// declared bare in reverse order (`expression_index`, `argument_index`, `result`, `arguments`) and assigned in retail's
// order 95 (266 B); the allocator as a plain function (no `inline`, still expanded) 95 (266 B); `result = NULL` first in
// the arm with the element through a `long *const &` bound to the pointer 83 (277 B); `result = NULL` first with the
// element passed as `arguments + argument_index` and the index stepped ahead of the expression update 83 (277 B).
// Diagnostic, reverted at once: one global store of the pointer after the call (the pointer live across it) 102 (292 B:
// the pointer spills to a frame slot, but the thread index then arrives in EDX), so a use past the call is not what
// keeps retail's result and pointer apart.
// M3 lane 1 r157 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, control 12, volatile-free 95 at 266 B
// re-measured): the allocation returned by value from a second file-local inline allocator as a one-pointer aggregate,
// the pointer read out at once 95; that aggregate kept as the local and read at the result copy and at the element 95;
// the aggregate copied whole into a second local for the element 95 (all 266 B, promoted to one register with the
// result); a two-member aggregate (pointer and the frame offset) kept as the local 80 (285 B: an 8-byte frame, the
// unused offset member is stored and copied on the stack while the pointer still shares EAX with the result, no
// home for it); a class-type value with a converting constructor 95, with a copy constructor as well 95, with an
// empty destructor 95 (all three 266 B: the construction is folded and the pointer promoted). No by-value or
// class-type form gives the pointer retail's stack home.
// M3 lane 13 r145 (h2m fast with 0x2099f0, which stays matched; control 12): one to four unused initialised locals at the
// top of the kept body 12 each, of the volatile-free body with `result = NULL` first in the arm 83 each (no symbol-count
// effect); the first allocation's size named in a `long size` local ahead of the frame reference on the kept body 12, on
// the null-first body 83, on the volatile-free control 95 (all three the same code). Diagnostic (reverted at once): the
// home on the result copy instead of the pointer 81 (277 B). In the null-first listing there is no frame slot at all
// (no `push ecx`): the pointer keeps EAX for the whole function and the result ESI, so retail's spill needs more
// pressure than any volatile-free form here gives.
// M3 lane 12 r179 (h2m fast with 0x2099f0, which stays matched; control 12): the file's allocator taking the frame
// reference (`s_hs_macro_frame *const *`) and reading the frame through it at each of the three allocations,
// volatile-free 109 (253 B: no per-allocation pool reload), with the volatile home 99 (291 B), volatile-free with
// `result = NULL` first in the arm 107 (255 B); the first allocation written out through `*frame` with the result copy
// after the third allocation and `result = NULL` first in the arm, volatile-free, 83 (277 B, the null-first code); on
// the kept body the argument index stepped ahead of the expression update 19 (279 B).
// M3 lane 13 r150 (h2m fast with 0x2099f0, which stays matched; volatile-free forms, `result = NULL` first in the arm unless
// noted): the frame reference after the three allocations with the expression pool read once into a pool local for the
// init block and the test 80 (282 B); the frame reference first with the pool's data read once into a byte pointer for
// them 85 (277 B, the pointer still in EAX); the arm's element taken from `result` with `result = NULL` right after it 93
// (268 B), with `result = NULL` last 95 (266 B); the frame bound as a `s_hs_macro_frame *&` with the first allocation
// written through it 83 (277 B, the clear-first code).
// M3 lane 5 r247 (fast screen with 0x2099f0, which stays matched): volatile-free, the result read back at the return
// through a `long *const &` bound to it 95; a bool set at the end of the arm and `if (...) result = NULL;` before the
// return 95; the allocation kept in a void pointer with the array pointer and the result each cast from it 95; the arm
// reading the array through a block-scoped const copy with `result = NULL` first 83 (277 B). With the volatile home:
// the volatile pointer and the result each cast from a void allocation local 71 (277 B). The allocator's colouring
// order for the volatile-free body has the pointer, the allocator's return and the result as one candidate (the result
// copy is sunk into the two exits), so exit-side spellings do not separate them.
// M3 lane 8 r240 (fast screen with 0x2099f0, which stays matched; control 12). The kept body's colouring order is already
// retail's (the first allocation's return in EBX, the result in EAX, the first frame in EAX), so the 12 is the volatile
// reload alone; in every volatile-free body the merged pointer/result candidate's return preference scores EAX against
// the first frame, so the frame takes ESI and the pointer EAX. Volatile-free: the arm stepping the pointer with the
// result copy kept 95 (266 B); a `long *const` copy of the pointer made just before the test, the result copied from it
// 95; `return NULL;` in the arm 90 (277 B); a thread pointer local, the init block reading its frame field 95; `long
// *result;` declared first and set in an else arm 90 (277 B). A forced-ranking search on the kept body: nothing below 12.
// M3 lane 7 r232 (h2m fast with 0x2099f0, which stays matched; control 12): the result and the volatile home set by one
// chained initialiser (`long *volatile arguments = result = ...`) 71 (277 B: the allocation computed straight into EAX, as
// in the direct-allocation form); `long *result = &arguments[0];` 12; `++argument_index` 12; the element as `arguments +
// argument_index` alone 12 (all three the same code); the count test as `parameter_count > argument_index` 14.
// M3 lane 11 r174 (h2m fast with 0x2099f0, which stays matched; control 12): the result held as a `dword` copy of the
// pointer (`(dword)arguments`, cleared to 0, cast back at the return) volatile-free 95 (266 B), with `result = NULL`
// first in the arm 83 (277 B), with the volatile home 12 (same code); the frame reference declared `*const *const`
// volatile-free 95, `*const *` with the volatile home 12. A forced-ranking search on the null-first body (43 single
// decisions) gives nothing below 82: its folded frame load is not a colouring or scheduling choice.
// M3 lane 3 r319 (h2m fast with 0x2099f0, which stays matched; volatile-free forms): the result copy made after the
// expression-index slot with `result = NULL` first in the arm 83 (277 B); the first size as `(short)(parameter_count <<
// 2)`, null first, 83 (same code); the copy after the slots with `return NULL` from the arm 90 (277 B); the index stepped
// inside the call's element argument (`&arguments[argument_index++]`), null first, 80 (277 B, the frame load still folded);
// the frame reference and the result copy after the three slots, null first, 79 (282 B). All restored.
// M3 lane 2 r161 (h2m fast with 0x2099f0, which stays matched; volatile-free forms on the result copied in the init block
// and in an else arm, re-measured 87 at 278 B): the frame reference after the three allocations 86 (281 B); the init
// block's copy at its end 89; `result = NULL` first in the arm 95 (288 B); the element passed straight 87; the
// argument-index slot as a plain pointer bound to the reference after the allocations 87. A forced-ranking search on
// the volatile-free control (40 decisions) gives nothing below 94. Reading: retail's result is a load of the pointer's
// stack home that is forwarded from the store after registers are chosen (`mov eax, ebx`), so retail's pointer is a
// memory variable in the IL; the volatile home gives the memory variable but forbids the forwarding.
// @retail 0x209d50
long *__stdcall function_209d50(long thread_index, short parameter_count, short const *parameter_types, bool initialize)
{
	s_hs_macro_frame **frame = &hs_macro_thread_get(thread_index)->frame;
	long *volatile arguments = (long *)function_xc5914d(thread_index, parameter_count * sizeof(long));
	long *result = arguments;
	short &argument_index = *(short *)function_xc5914d(thread_index, sizeof(short));
	long &expression_index = *(long *)function_xc5914d(thread_index, sizeof(long));

	if (initialize)
	{
		argument_index = 0;
		expression_index = hs_macro_expression_get(hs_macro_expression_get((*frame)->expression_index)->value)->next_index;
	}
	if (argument_index < parameter_count &&
		hs_macro_expression_get(expression_index)->type == parameter_types[argument_index])
	{
		long *argument = &arguments[argument_index];
		function_2099f0(thread_index, argument, expression_index);
		expression_index = hs_macro_expression_get(expression_index)->next_index;
		(argument_index)++;
		result = NULL;
	}
	return result;
}
