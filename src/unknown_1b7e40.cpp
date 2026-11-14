// @flags /O2 /Ob1 /arch:SSE /Gr
// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "props.h"

/* slot type 0x12 */

struct s_slot_12
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long element_index;
	byte unknown14[0x1c - 0x14];
	long unknown1c;
	long unknown20;
	byte unknown24[0x40 - 0x24];
};

short __stdcall function_1b7e40(long actor_index);
short __stdcall function_1b81c0(long actor_index, s_slot *slot, bool active);
long __stdcall function_1b8070(long actor_index, s_slot *slot);
bool __stdcall function_1b82d0(long actor_index, s_slot *slot, long index);
struct s_invite_data;
short __stdcall function_1b83b0(long actor_index, long leader_index, s_slot *slot, s_invite_data *data);
void function_265c30(long prop_index, long actor_index, bool unknown);

struct s_invite_data
{
	long unknown0;
	long unknown4;
};


// @retail 0x1b8360
void __stdcall function_1b8360(long actor_index, s_slot *slot, long index)
{
	s_slot_12 *state = (s_slot_12 *)slot;

	state->unknown20--;
}

// @retail 0x1b8370
void __stdcall function_1b8370(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	actor->unknown449 = true;
	actor->unknown44a = true;
}

// @retail 0x1b8460
void __stdcall function_1b8460(long actor_index, s_slot *slot, long index)
{
	s_slot_12 *state = (s_slot_12 *)slot;
	s_502424_element *element = element_502424_get(state->element_index);

	if (element->target.unknown0 == index)
		element->target.unknown0 = NONE;
}

void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);

bool actor_has_joint_invitation(long actor_index, short type);
long function_25d810(long object_index, long actor_index, bool create);
bool function_25d9b0(long prop_index);

// MiniMax A (11-13): no new form, no lower score (49); what still differs: retail calls function_26f060 with the
// actor index in ECX and the invitation type in EDI where this build passes them in EAX and ESI, and that moves the
// actor and the node between EBX and EBP through the body; the busy test then becomes a numeric select
// (neg/sbb/not/and) where retail branches into the success epilogue; the convention is set by the callee
// (src/unknown_26e370.cpp): BLOCKED-BY 0x26f060.
// MiniMax D (11-14): no new form, no lower score (49): retail passes the actor index to 0x26f060 in ecx and the
// invitation type in edi where this build passes them in eax and esi, and that convention is set by 0x26f060 in
// src/unknown_26e370.cpp, which this lane may not edit, so the busy selection and the zero and success epilogues
// still differ. BLOCKED-BY 0x26f060.
// score 49 (was 49): frame and actor/node homes agree; busy selection and zero/success epilogues differ.
// MiniMax R (11-01): the file's retail setting includes /Ob1 (`// @flags /O2 /Ob1 /arch:SSE /Gr`): 145 -> 49
// (557 bytes against retail's 551); the file's other seven functions stay matched and nothing is demoted.
// Round 107 kept form, 86 -> 49: the `priority > 0` gate after both branches gives retail's block order (no-invitation
// frame 0xc and actor/node homes agree; the busy test becomes a numeric select,
// shifting fallback and separate zero/success epilogues.
// Earlier: the `priority > 0` gate after both branches gives retail's block order (no-invitation
// the `priority > 0` gate after both branches gives retail's block order (no-invitation
// the `priority > 0` gate after both branches gives retail's block order (no-invitation
// branch last, jumping past the gate), priority in SI, view in EDI and the state spilled to the argument slot.
// Left: actor and node swap EBX/EBP through the body, and the zero exits share `xor esi, esi; mov ax, si` where
// retail has its own `pop ebp; xor eax, eax; pop ebx` epilogue (9 bytes short). Frame 0xc and invitation call agree.
// Round 30 (fast, file and callees in the image; with this function alone the image is unfaithful,
// 157 on the same source): the lookups through the existing actor_get/prop_node_get/
// element_502424_get and prop_node_view 125 (same code, kept in place of the written-out bodies);
// the found path breaking out of the loop 186; the evaluation inside the invitation branch with
// the fallback going to it 125; node declared first 125.
// Earlier: retail passes the actor index to function_26f060 in ECX and
// the invitation type in EDI, ours passes them in EAX and ESI; that choice also swaps
// the prop index and the node between ESI/EBP and EBP/EDI through the body. The
// convention is set by function_26f060 (src/unknown_26e370.cpp), not editable here.
// Tested iterator for with native priority/node-getters/length graph: fast
// 170/169/170/169/170/169/170/169 versus fast control 170; full baseline 131 retained; one-point
// reductions rejected.
// Round 371: actual participant array 170 reduced; long invitation index 168 reduced;
// shared result 154 reduced / 169 plain; long index/shared result 156 reduced;
// existing view helper ties each form. Opening body restored at 131.
// Round 86 (plain): the evaluation as one forceinline helper used by both branches
// 418 (768 bytes, the copies are not merged); plain `return 0` at the loop's end 172,
// with the for-form iterator loop 172; priority test inverted around the goto 131;
// invitation test named 131. Retail places the no-invitation branch last, jumping
// into the evaluation that follows the loop. BLOCKED-BY 0x26f060.
// State query after view/velocity/at consumer, or its field snapshot, with
// mutable/readonly actor 170/152/152/164 reduced in each case. Post-velocity
// plain 124 at 555 bytes is a seven-point gain and is restored; existing
// length/self-dot helpers with the two late queries all tie 152 reduced.
// Initial actor/prop/node registers and fallback placement still differ.
// Every-use property observations with late state query / long priority / both: reduced
// 170/152/169/151; the existing view getter ties all four. Both late-query/long-priority
// forms plain 124 at 554 bytes (seven below 131), rejected; original body restored.
// The invitation convention, initial property/node ranks and fallback placement remain.
// Round 223: complete eligibility bool/byte/short/long with short priority 150/150/150/150 reduced;
// with long priority 166/166/171/169. Minimal bool form plain 119 at 556 bytes (six below 125), rejected.
// Original body restored; current invitation call agrees, actor/node ranks and fallback placement remain.
// Round 20 (lane 15, fast): `short result = 0` and priority 0 at the top, loop `break`, `else if` fallback, gate with
// `result = priority` 137 (actor EBX/node EBP as retail, both locals on the stack); without result 145; priority as
// the result with `if (!ok) priority = 0` 127 (retail's `xor eax, eax` exit, priority spilled), with priority declared
// first 127, long 126; the loop's `goto evaluate` kept with that gate 83 (551 bytes, but a non-positive priority
// returned itself) and the gate's `else priority = 0` added 86 (kept, plain); result declared at the label 118;
// `if (ok) return priority; } return 0;` 105; result at the top with each failure `return result` 150; `return ok ?
// priority : 0` 125, on the old body 119 (retail's zero epilogue, `movsx eax, si`), with `(short)0` 118; the old body
// with `if (ok) result = priority; else result = 0;` 118 (`mov eax, esi`); on the kept form: node declared first 86,
// the fallback through actor->prop_index 86, priority declared first 86, no prop_index local 86, node from a
// conditional expression 167, node set in an if/else either way round 167/167, priority 0 at the top 86, plain
// `return 0` after the loop 119; loop-break forms with the else-if fallback and five tails 92/145/137/140/139;
// goto forms with direct return/result/conditional/if-else result tails 105/117/125/124.
// Round 33 (plain): no-invitation branch written first 154 (VC keeps that order); the evaluation
// written out in both branches 265 (730 bytes, copies not merged); the evaluation inside the loop
// body with the fallback's goto into it 125 and in the first arm with `return priority` after the
// if/else 125 (same code each); single exit with a per-arm short result and `goto done` 124 (the
// zero return becomes retail's `xor eax, eax`, layout unchanged); no gotos: `short priority = 0`,
// the found path breaking out, the fallback as `else if`, then `if (priority > 0) { evaluation }`
// 145 (578 bytes) -- the first form with retail's block order (evaluation after the loop, the
// fallback last jumping in past the priority test), but priority is spilled and the state kept in
// ESI (retail the other way) and the zero exit sits at the loop bottom; that without the top-level
// init plus `if (!entry) return entry` after the loop 186 (entry spilled, frame 0x10).
// Round 13 (fast): `short result = 0;` at the top with `if (ok) result = priority;` in the gate 117 (551 bytes:
// actor EBX and node EBP as retail, but result gets its own slot, frame 0x10, and priority is spilled to the
// argument slot), result declared before the invitation test 169, with `else result = 0;` 141; a separate
// pointer for the node the evaluation uses (node copied into it in the loop, the loop's and the fallback's
// lookups assigned to it) 110 (retail's head, actor EBX/node EBP, but that pointer spilled, frame 0x10),
// declared first 110. Any extra value live from entry, or a split node, gives retail's actor/node registers;
// retail gets them without a stack slot.
// Round 26 (lane 14, fast, file and callees in the image, control 86): the 0x26f060 call now agrees. One
// extra actor read inside the loop (diagnostic: `|| actor->unknown07c == 99` in the decline test) gives
// retail's actor EBX/node EBP at 551 bytes (116); an extra actor read in the evaluation or one node read
// fewer there keeps ours. The evaluation as early `return 0`s with `return priority` last 128 (570
// bytes); the near distance read once into a local 90; an invitation element pointer for the joint and
// participant reads, with or without the index local, 148/148 (aligned 78: the pointer is kept in EDI
// where retail recomputes the index); the loop's new prop stored in prop_index 86 (same code); the gate
// as `return priority` / `return 0` 105 (priority spilled, state in ESI, `xor ax, ax`).
// Round 451: Native iterator aggregate/entry scope/entry-result role-name subsets
// 1/2/1+2/4/1+4/2+4/all, then late entry aggregate: 88/86/155/86/88/86/155/151, with faithful
// callback/callee controls. Original descriptor and body restored.
// Round 53 (lane 9, fast): retail's `xor eax, eax` zero epilogue is a tail-duplicated `priority = 0; return
// priority;` (compile-only screens: `return 0` gives `xor ax, ax`). `if (ok) return priority; } return 0;` 105
// (priority spilled, `xor ax, ax`); `if (ok) goto done; } priority = 0; done: return priority;` 91 (554 bytes:
// retail's exact zero epilogue, but priority is spilled to the argument slot and the state kept in ESI, the
// other way from retail), with priority initialized at its declaration 91, also without the fallback's own
// zero store 91, with a long priority 91, with the state declared at the top 91.
// Round 119 (plain; a --fast image of the file's three loop functions is unfaithful here, 145 on this source):
// on this body a long priority 84 (priority moves to EDI, the view to ESI; not retail's shape); `return 0` at the
// loop's end 119 (551 bytes, the loop exit gets its own zero epilogue); both 125; the gate's arms as `return
// priority` inside and `return 0` after it 114 (567 bytes); the same with a long priority 140 (fast image).
// Round 465: default-first/branch-complete property acquisition result, short/long priority,
// existing/entry velocity workspace: 86/86/84/84/86/86/84/84. Two-count lead rejected; original body restored.
// Round 143 (lane 4, fast with the file and its callees, faithful 86): the gate as `if (ok) goto done; }
// priority = 0; done: return priority;` 91 again (retail's whole layout and zero epilogue; left: actor/node
// EBP/EBX and priority spilled to the argument slot with the state in ESI), with `return priority` in the
// gate and `priority = 0; return priority;` after it 105 (560 bytes), with the state fetched after the
// velocity declaration 91; on the kept form the gate inverted (`if (priority <= 0) priority = 0; else`) 86
// (same code) and the participant index named in the found path 86 (same code).
// Round 122 (lane 6, fast, file and callees in the image, control 86): `short result = 0` at the label with
// `result = priority` on success and a long priority 117 (554 bytes); the `goto done` zero-epilogue form with node
// declared first 91 (554), with the fallback through actor->prop_index 91 (in both the priority is spilled to the
// argument slot and the state kept in ESI); the loop's object compare written node-field first 87; the node's
// near distance read once into a local for both range tests 90 (541). The recorded goto form with an initialised
// priority and with a long priority re-measured 91 each.
// Round 57 (lane 16, fast with the file's loop functions): the fallback as an if/else choosing the priority (a
// `>= 2 ? 3 : 0` select) 110 (552 bytes); the iterator entry declared in the `while` condition with `return 0`
// after the loop 119 (551 bytes, actor/node still EBP/EBX); velocity declared before the state and view lookups
// 86, the decline test through a NONE-preset `new_prop` 86, a short participant-index local 86 (same code each).
// Round 107 (lane 7, plain, with the file and its callees in the image), kept 86 -> 76 -> 65: in the loop the
// already-held prop tested first (`if (node && target == node->object_index) priority = ...; else { acquisition,
// decline, new node, its own priority read }` before the existing `goto evaluate`; at review the lane's second
// `goto evaluate` in the first arm was folded into this if/else, same 49 plain) (retail's scratch registers
// in the acquisition and participant reads: both tails are cross-jumped into one); then the evaluation as
// `if (ok) return priority; } return 0;` (now 557 bytes: retail's zero exit is shared by every failure). Tried:
// the acquisition test inverted with one priority read 86, as `< 4 && != NONE` with the decline last 76, the
// acquisition result preset to NONE 86, float sqrt 86 (all on the old evaluation); on the found-first loop the
// round-53 `goto done` tail 76, `priority = 0; return priority;` 65, the negated test returning 0 65 (same code),
// `return 0` for the loop's exit 142 (it gets its own epilogue), no prop_index local 65, the compare operands
// swapped 66, `return false` 65. Then kept 65 -> 49: the busy-prop test (0x25d9b0) as its own
// `if (...) return 0; return priority;` inside the other tests' block: actor EBX/node EBP as retail through the whole
// body, but the two returns become a select (`neg al; sbb eax, eax; not eax; and eax, esi`) where retail tests
// (`test al, al; je` to a `return priority` placed last) and falls into the shared `xor eax, eax` exit. On that
// block: `if (!busy) return priority;` 65 (old registers), `if (busy) priority = 0; return priority;` 82, the whole
// test as one && with an extra `return 0` 65. Left: the select, the success/zero block order and `xor ax, ax`.
// Round 53 (lane 11, fast with the file and its callees): `short result = 0` at the top, `if (!busy) result =
// priority; return result;` with the loop's own `return result` 153 (`xor ax, ax` there, frame 0x10), keeping the
// loop's return 87 (result spilled); the busy test as `if (!busy) goto done; } } priority = 0; done: return
// priority;` 76 (542 bytes: retail's block order, zero exit after the busy test and the fallback after it, but
// actor EBP/node EBX and one shared `xor esi, esi; mov ax, si` return where retail has two epilogues), the same
// with the busy test in the && chain 76; `return priority` inside and `priority = 0; return priority;` after 65
// (success epilogue falls through, zero exit last as `xor ax, ax`), nested 65, with node or priority declared
// first 65/65. Diagnostic only (reverted): an extra actor read in the decline test flips the goto form to
// retail's actor EBX/node EBP, leaving only the shared return (12 aligned rows, 551 bytes); so the two causes
// left are the actor/node register tie and retail's duplicated epilogue (`xor eax, eax` copy for the zero path).
// Round 55 (fast with the file and its callees, faithful 49): VC if-converts the busy test (`neg al; sbb;
// not; and`) for `if (busy) return 0; return priority;` with a final `priority = 0; return priority;` 49,
// `if (!busy) return priority; return 0;` 49, a named bool 65 and `return busy ? 0 : priority` 83 (573);
// `if (!busy) return priority;` with the final `priority = 0` 65; busy joined with `&&` 65 (no select, but
// actor/node swap to EBP/EBX), with node declared first 65, a long priority 62, both 62, the fallback or
// every lookup through actor->prop_index 65/65; `goto done` after the `&&` test / after the busy test 65/49;
// the condition inverted returning 0 65; single exit with `priority = 0` through goto/if-else 76/76 (542);
// a short result at the label 91/108/94; the evaluation after `if (priority <= 0) return 0;` with early
// returns, `&&` busy, inverted test and busy in its own if 49/65/49/49; a goto-free form (found paths `break`,
// fallback `else if`, `short priority = 0`) with an if/else result 141 (574), `short result = 0` 201 (594),
// `&& !busy` returning priority 142, busy in its own if 118 (571); on the current loop an if/else short result
// with `&& !busy` 63 (555; no select, actor/node swap) and `if (busy) result = 0; else result = priority;` with
// else-arms `result = 0` 48 (556; retail's `xor eax, eax` zero exit, still a select; one point, not kept), with
// a long priority 60/53, with a long invitation index 141/140. A short result zeroed in an if/else is what
// gives retail's 32-bit `xor eax, eax`; `return 0` gives `xor ax, ax`.
// Round 278: full-width return result at entry/default, entry/complete, evaluation/default,
// evaluation/complete: short priority 87/64/94/64; long priority 81/53/90/53. Restored.
// P255: Actual invitation-record receiver with common priority/full-width result/conditional
// node subsets 0-7 172/145/156/155/168/126/149/147. All original reads, helpers and body restored;
// frame/actor/node homes agree, busy select and zero/success epilogues remain.
// P255 follow-up: View after velocity/at its predicate, with state-at-consumer and
// full-width priority interactions 100/78/78/80/102/80/81/81. Original view/state ownership and body
// restored. Earlier P255 record-receiver family overlaps old forms and is excluded from new credit.
// Round 86 (KIMI2): evaluation result with busy goto/single exit 91; the same without goto 91;
// scoped zero result 51; priority-initialized result 49; reversed branch-complete result 49;
// that with velocity declared first 49; selected_priority/outcome result names 49/49. Original body restored.
// Round 125 (lane 8, fast with the file): the existing length_sq3f for the speed 49; the existing magnitude3d 49;
// `short priority = 0` at the declaration in place of the fallback's store 49; the busy test as `== false`
// returning priority inside 65; the busy test as an if/else of the two returns 49 (still the select).
// Round 128 (lane 8, fast with the file and its callees, faithful 49; plain confirmed): kept 49 -> 30: the busy test
// (0x25d9b0) joined into the evaluation's && chain with `return priority` and the fallback's node test written as
// `prop_index != NONE` (the && chain alone 65 swaps actor/node to EBP/EBX; the fallback test through the prop
// index gives retail's actor EBX/node EBP and no select; `actor->prop_index != NONE` 30, same code). Tried: the busy
// call through the prop_index local 143; `priority = 0; return priority;` at the end 30 (same code); the whole test
// inverted returning 0 first 30 (same code); `return 0` for the loop's exit 123 (575 bytes); the old busy `if` with
// the prop-index fallback 50; a short result zeroed at the label 92.
// NOT kept (review): the 30 needs the fallback to test the prop index (`cmp esi, -1`) where retail tests the node
// (`test ebp, ebp`), a form picked by score against retail's test; the && chain alone is 65. Body restored at 49.
// Round 402: Single complete outcome including iterator exhaustion, short/long/word/dword, default-zero or complete busy branches.
// Scores 132/135/132/135/136/170/136/170. NOT kept: unmatched outcome forms; original complete body restored.
// Round 138: busy result in byte/short/long storage, Boolean at entry/evaluation scope 49 each;
// evaluation-scope Boolean with priority cleared 82; unsigned-short result initialized from priority 49
// (558 bytes), branch-complete unsigned-short result 49. Original tail restored; busy select and exits remain.
// AC545: Native ordered admission in nested predicate groups, eight boundary
// partitions, 49 each. Original busy call, zero/priority exits, iterator
// exhaustion and fallback node test restored; the select and epilogues remain.
// AC557: Busy provider char/signed-char/byte/short/word/int/long/dword returns,
// coherent definition and all prototypes: 49/49/49/51/51/49/49/49. NOT kept:
// no target gain; wider transports also demote provider and five other users.
// Round 398: Native velocity square/length producers, their paired/range-observation forms,
// immutable complete graph and shared squared-length producer: eight at 49 (556/557 bytes).
// An isolated range-copy control also scores 49 and is excluded from new-trial credit. NOT kept:
// no score gain; original velocity/range observations, busy select and complete body restored.
// M400: Original velocity/iterator defaults via aggregates or CRT, at original phases/entry,
// separately/together: 95/90/163/211/231/187/187/230. NOT kept: all regress;
// original twelve-byte workspace producers, busy test and all exits restored.
// MiniMax G (11-10): no new form, no lower score (49); five tail spellings (the busy call negated into the return, a named bool, an explicit == false, an explicit else arm and the busy test folded into the && chain) score 116, 145, 142, 145 and 156, so the busy call's result still becomes a mask where retail branches to its own epilogue that returns the priority in AX.
// MiniMax E (11-10): no new form, no lower score (49); our build still turns the busy call's result into a mask (neg al; sbb eax,eax; not eax; and eax,esi) where retail tests it and branches to its own epilogue that returns the priority in AX, and our shared "return 0" exit still sits after the fallback block where retail's sits before it, so the body stays six bytes longer (557 against 551) (the inverted test and the busy test folded into the chain with the negated call both score 65, and 61 permuter tries found nothing lower).
// MiniMax E (11-10): no new form, no lower score: the sequence matches retail's up to the last block, and
// what is left is its layout and the form of the final choice, which is a mask in our build and a
// branch to an epilogue of its own in retail, so our body is six bytes longer.
// MiniMax E (11-09): no new form, no lower score (49); the instruction sequence matches retail's up to the
// last block, and what is left is its layout and the form of the final choice: retail tests the 0x25d9b0 result
// and branches to an epilogue of its own that returns the priority in AX (pop edi; mov ax,si), where our build
// computes the choice as a mask (neg al; sbb eax,eax; not eax; and eax,esi) and returns it in EAX, and our
// shared "return 0" exit sits after the priority block where retail's sits before it, so our body is six bytes
// longer (557 against 551); the inverted test, a local for the result and an extra return all give the same 49.
// MiniMax R (11-09): no new form, no lower score (49): the busy call's tail is still the select
// neg al, sbb eax, eax, not eax and and eax, esi where retail tests the flag and branches to its
// own return-priority block, and the velocity's square is still built from the twelve-byte
// workspace; the native velocity producers, the paired observations and the busy select forms were
// all measured (49 each), and the form that removes the select (30) was dropped at review because
// it tests the prop index where retail tests the node.
// MiniMax A (11-11, second pass): no new form, no lower score (49); what still differs is unchanged: the busy call's tail is still the
// mask select (neg al, sbb eax, eax, not eax, and eax, esi) where retail tests the flag and branches to its own return-priority epilogue
// that returns the priority in AX, and our shared return-0 exit still sits after the priority block where retail's sits before it, so the
// body stays six bytes longer (557 against 551); every spelling that breaks the select swaps the actor and node registers from retail's
// EBX and EBP to ours EBP and EBX, which costs more than it gains.
// MiniMax A (11-11): no new form, no lower score (49); the busy call tail is still the mask select (neg al, sbb eax, eax, not eax, and eax, esi) where retail tests the flag and branches to its own return-priority epilogue, and our shared return-0 exit still sits after the priority block where retail sits before it, so the body stays six bytes longer (557 against 551); the inverted test, an explicit else, a goto, the early-return chain, a priority guard with return priority last and the negated busy test all compile to the same 49 bytes.
// MiniMax C (11-11): no new form, no lower score (49); the busy test folded into the && chain (65), the inverted busy test (65) and an explicit if/else (49) all leave the numeric select where retail branches to its own priority epilogue.
// MiniMax H (11-11): no new form, no lower score (49); the body is unchanged, because the busy call's
// result still becomes a numeric select where retail tests it and branches to its own epilogue that returns
// the priority, and the fallback still tests the prop index where retail tests the node.
// MiniMax I (11-08): no new form, no lower score (49); the busy-true path falling through to the shared zero exit gives 65 with the actor and node registers swapped.
// MiniMax R (11-07, run 38): no new form, no lower score (49); the busy test still becomes a numeric select
// where retail branches to a last `return priority` and falls into the shared `xor eax, eax` exit, and the
// fallback still tests the prop index where retail tests the node, so the block order and the two epilogues
// keep it at 49.
// MiniMax C (11-06): no new form, no lower score; the busy test still becomes a numeric select
// where retail branches to a last `return priority` and falls into the shared `xor eax, eax` exit,
// and the fallback still tests the prop index where retail tests the node, so the block order and
// the two epilogues keep it at 49.
// MiniMax H (11-05): no new form, no lower score; the busy test still becomes a numeric select
// where retail branches to a last `return priority` and falls into the shared `xor eax, eax` exit,
// and the fallback still tests the prop index where retail tests the node.
// MiniMax B (11-04): no new form, no lower score.
// MiniMax S (11-09): no new form, no lower score (49); the fallback still ends in retail's shared zero exit and
// the busy test still becomes a numeric select, so the block order and the two epilogues stand.
// MiniMax B (11-12): no new form, no lower score (49); the busy call's tail is still the mask select (neg al; sbb eax,eax; not eax; and eax,esi) where retail tests the flag and branches to its own return-priority epilogue (pop edi; mov ax,si), so the body stays six bytes longer (557 against 551); the ternary, the named result, the explicit else, the empty then-arm, the merged outer if and the busy test folded into the && chain score 83, 49, 145, 65, 152 and 150, and passing the node's object index instead of the actor's prop index scores 36 but is not what retail does (retail loads the actor's prop index from [ebx+0x338] before the call).
// M3 lane 12 r119 (fast with the file and 0x25d810/0x25d690/0x25d740/0x25d9b0, faithful 49): the busy call joined into
// the `&&` chain with `return priority` 65 (re-measure: no select, actor/node in EBP/EBX, and the success return is the
// busy test's fall-through where retail falls into the shared `xor eax, eax` exit and returns the priority last); on
// that form the node or the actor local renamed to sort first or last 65 each, byte-identical (local names do not break
// the actor/node register tie); the busy call reading `actor_get(actor_index)->prop_index` 213 (610 bytes).
// m3 lane 6 r178 (--fast with the file and 0x25d690/0x25d740/0x25d810/0x25d9b0, faithful 49; a one-file LTCG
// harness is not faithful here, 87): the busy call joined into the `&&` chain with `return priority` 65
// (re-measured: no select; actor/node in EBP/EBX, the zero exit last as `xor ax, ax`); on that form a folded
// `actor &&` before the busy call 65 and a folded `node &&` heading the chain 65 (same code: the actor/node tie
// does not follow these block weights); that with `short result = 0;` after the label, `result = priority;` and
// one `return result;` 91 (554 bytes). On this body: the busy test as `if (!actor || busy)` 49 and
// `if (actor && busy)` 49 (still the select). Body unchanged.
// @retail 0x1b7e40
short __stdcall function_1b7e40(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long prop_index = actor->prop_index;
	s_prop_node_view *node = NULL;
	if (prop_index != NONE)
		node = prop_node_get(prop_index);
	short priority;
	if (actor_has_joint_invitation(actor_index, 0x12))
	{
		s_slot_entry_iterator iterator;
		iterator.actor_index = actor_index;
		iterator.reference.unknown2 = 0x12;
		iterator.reference.unknown0 = NONE;
		s_slot_memory_entry *local_1;
		while ((local_1 = function_26f0c0(&iterator)) != NULL)
		{
			short invitation_index = iterator.reference.unknown0;
			s_502424_element *joint = element_502424_get(((s_slot_owner_entry *)actor)->joint_invitations[invitation_index].joint_index);
			if (node && joint->target.unknown0 == node->object_index)
			{
				priority = *(short *)((byte *)joint + ((s_slot_owner_entry *)actor)->joint_invitations[invitation_index].participant_index * 12 + 0xa);
			}
			else
			{
				long new_prop;
				if (actor->unknown086 >= 4 || (new_prop = function_25d810(joint->target.unknown0, actor_index, false)) == NONE)
				{
					joint_decline(actor_index, iterator.reference.unknown0);
					continue;
				}
				node = prop_node_get(new_prop);
				priority = *(short *)((byte *)joint + ((s_slot_owner_entry *)actor)->joint_invitations[invitation_index].participant_index * 12 + 0xa);
			}
			goto evaluate;
		}
		return (short)(long)local_1;
	}
	priority = 0;
	if (actor->unknown07c != NONE && node)
	{
		node = prop_node_get(prop_index);
		if (node->unknown27 >= 2)
			priority = 3;
	}
evaluate:
	if (priority > 0)
	{
		s_type_5cfb45 *state = function_25d690((s_prop_datum *)node);
		s_prop_view_fields *view = prop_node_view(node);
		vector3f velocity;
		function_ba1d0(node->object_index, &velocity, NULL);
		if (node->unknown28 > 1.6f && (real)sqrt((double)(velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k)) < 1.0f &&
			view->unknown54 < 0.31 && node->unknown28 <= 15.0f && !state->unknown64)
		{
			if (function_25d9b0(actor->prop_index))
				return 0;
			return priority;
		}
	}
	return 0;
}

/* the slot's status: 0x10 when the prop node's distance is under 0.8; continue (g_46fbe8)
   when the prop's object moves slower than 1, the view's unknown54 is under 0.31 and the
   distance is at most 15; finished (g_46fbe4) otherwise. Retail calls the view getter out
   of line here; LTCG inlines it unless the node test guards the call. */
// MiniMax A (11-13): no new form, no lower score (73); what still differs: retail calls function_25d740 out of line
// and sinks the ESI/EDI saves into three separate default exits, where this build expands the call at this site and
// keeps one exit and the saves in the prologue; the inline-ness of that helper is set in src/unknown_25d690.cpp:
// BLOCKED-BY 0x25d740.
// MiniMax D (11-14): no new form, no lower score (73): our build still expands the call to 0x25d740 at this
// site, so the prologue still saves esi and edi before the seat count test where retail tests the count first and
// keeps that call out of line; the inline-ness of that helper is declared in src/unknown_25d690.cpp, which this
// lane may not edit. BLOCKED-BY 0x25d740.
// score 73 (was 73): retail calls function_25d740 out of line and sinks the ESI/EDI saves; our
// build inlines it, so the prologue differs at +0x13. This round: reading the slot's expected
// index into a local before the actor lookup, the reversed compare, a named actor local and the
// separate-exit forms compile byte-identically (73) or worse (85-87 at 272-277 bytes); holding
// the call out of line needs a noinline declaration in src/unknown_25d690.cpp.
// Earlier: retail calls function_25d740 out of line (ECX node) and sinks the
// ESI/EDI saves past each early exit; our build inlines it through the prop_node_view
// wrapper, so the blocks are laid out differently. A single exit (`result = 0x10;` in the
// near arm, `result = g_46fbe8;` in the far arm) with the slot's prop index read before the
// actor lookup (sibling 0x1b82d0's order) gives retail's code everywhere but the inlined
// call (90 at 292 bytes) and matches byte for byte with the call held out of line; in that
// form the direct call, a noinline declaration in this file, `unknown20 >= 1` and the
// swapped prop compare all stay inlined (90/90/96/91). Earlier: typed slot payload 73;
// payload/result first 73; payload/wide result 74; payload/single result 88, with wide
// result 92; native node/view plus payload/self-dot/final-result graphs 73/73/73/73/78/78/
// 78/78; early returns 74; direct function_25d740 call 73, with early returns 74; named
// actor 74; prop test nested again 79; with the call kept out of line the tails merge (102
// at 231 bytes); a separate view-getter noinline with default status, direct proximity
// return, native payload, unsigned-word status and double speed consumer, eight forms all
// 102 at 231 bytes; whole-slot forms 87/90/87/90 at 277 bytes; complete final rejection
// with native payload/self-dot 87/87; staged final failures 90/90;
// early returns with `return 0x10` 74 (`mov ax`); the slot field on the left of the prop
// compare 74; a named actor with that compare 91; the existing magnitude3d for the speed
// 73, with early returns 74.
// retail calls function_25d740 out of line (ECX node) and sinks the ESI/EDI
// saves past each early exit; our build inlines it through the prop_node_view wrapper, so the
// blocks, the late ESI/EDI saves and the per-level tails differ. The single-exit body with the slot's
// prop index read before the actor lookup equals retail everywhere else (90; with inlining switched
// off around the call it matches), so LTCG's inlining of this one call site is the whole difference.
// Tried: typed slot payload 73; payload/result first 73; payload/wide result 74; payload/single result
// 88, with wide result 92; native node/view plus payload/self-dot/final-result graphs
// 73/73/73/73/78/78/78/78; early returns 74 (retail's late ESI/EDI saves, view still inlined); direct
// function_25d740 call 73, with early returns 74; named actor 74; prop test nested again 79; with the
// call kept out of line the tails merge (102 at 231 bytes); a separate view-getter noinline with
// default status, direct proximity return, native payload, unsigned-word status and double speed
// consumer: eight forms all 102 at 231 bytes; early returns with `return 0x10` 74 (still `mov ax`,
// retail `mov eax`); the slot field on the left of the prop compare 74; a named actor with that
// compare 91; the existing magnitude3d for the speed 73, with early returns 74, with per-level returns
// 87; a noinline declaration in this file 73 (ignored across files: still inlined; one on the
// definition would change the matched functions that inline it as retail does); per-level returns
// 87 (277 bytes), with
// `return 0x10` 87; complete final rejection with native payload/self-dot 87/87; staged final failures
// 90/90; whole-slot forms 87/90/87/90 (277 bytes); `unknown20 >= 1` 96; the prop compare operands
// swapped 91. Round 20 (fast): the first two tests joined with `&&`, slot field read at the top 112,
// read in the test 109 (290 bytes), with the early returns 76 (264 bytes); the 0.8 test inverted with
// the call in its then-arm 103; the single-exit body again 88 (278 bytes): inlined every time.
// 0x25d740 written through the existing prop_ref_view does not change the decision (harness). Matched
// 0x1b57f0 and 0x1fbff0 keep the same wrapper call out of line without any pragma; both call it twice.
// blocks are laid out differently. Typed slot payload 73; payload/result first 73; payload/wide
// result 74; payload/single result 88, with wide result 92; native node/view plus
// payload/self-dot/final-result graphs 73/73/73/73/78/78/78/78; early returns 74 (retail's late
// ESI/EDI saves, view still inlined); direct function_25d740 call 73, with early returns 74; named
// actor 74; prop test nested again 79; with the call kept out of line the tails merge (102 at 231
// bytes); a separate view-getter noinline with default status, direct proximity return, native
// payload, unsigned-word status and double speed consumer: eight forms all 102 at 231 bytes.
// Early returns with `return 0x10` 74 (still `mov ax`, retail `mov eax`); the slot field on the
// left of the prop compare 74; a named actor with that compare 91; the existing magnitude3d for
// the speed 73, with early returns 74.
// retail calls function_25d740, our build inlines it
// retail calls function_25d740 (LTCG keeps it out of line here, as in
// 0x1af810/0x1c9c80); our build inlines it through the prop_node_view wrapper, so the
// blocks, the late ESI/EDI saves and the per-level tails differ.
// Tried: typed slot payload 73; payload/result first 73; payload/wide result 74;
// payload/single result 88, with wide result 92; early returns 74 (retail's late
// ESI/EDI saves, view still inlined); direct function_25d740 call 73, with early
// returns 74; named actor 74; prop test nested again 79; with the call kept out of
// line the tails merge (102 at 231 bytes); native node/view plus payload/self-dot/
// final-result graphs 73/78; a separate view-getter noinline with eight payload and
// result forms 102 (231 bytes). Round 75: a noinline declaration in this file 73
// (ignored across files: still inlined); per-level returns 87 (277 bytes), with
// `return 0x10` 87; the existing magnitude3d for the speed 73 (plain; inlined, same
// code), with per-level returns 87. A noinline on the definition would change the
// 21 matched functions that inline it in retail.
// through the prop_node_view wrapper, so the two blocks are laid out
// differently. Round 371: typed slot payload 73; payload/result first 73;
// payload/wide result 74; payload/single result 88, with wide result 92.
// Round 86: early returns 74 (retail's late ESI/EDI saves, view still
// inlined); direct function_25d740 call 73, with early returns 74; named
// actor 74; prop test nested again 79. Even with the call kept out of line
// the tails merge (102 at 231 bytes).
// through the
// prop_node_view wrapper, so the two blocks are laid out differently.
// Native node/view plus payload/self-dot/final-result graphs 73/73/73/73/78/78/78/78; delayed
// saves and getter boundary remain.
// Round 371: typed slot payload 73; payload/result first 73; payload/wide result 74;
// payload/single result 88, with wide result 92. Opening body restored.
// Round 162: separate view-getter noinline with immutable default status and direct proximity return,
// combined with native payload, unsigned-word status and complete double speed consumer: eight forms
// all 102 at 231/264 bytes. Entire body, payload and getter declaration restored.
// Complete final rejection with native payload/self-dot 87/87; staged final failures 90/90;
// whole-slot forms 87/90/87/90, all 277 bytes. Original 264-byte body and getter restored.
// call and matches byte for byte with the call held out of line; in that form the direct
// call, a noinline declaration in this file, `unknown20 >= 1` and the swapped prop compare
// all stay inlined (90/90/96/91). Earlier: typed slot payload 73; payload/result first 73;
// payload/wide result 74; payload/single result 88, with wide result 92; native node/view
// plus payload/self-dot/final-result graphs 73/73/73/73/78/78/78/78; early returns 74;
// direct function_25d740 call 73, with early returns 74; named actor 74; prop test nested
// again 79; with the call kept out of line the tails merge (102 at 231 bytes); a separate
// view-getter noinline with default status, direct proximity return, native payload,
// unsigned-word status and double speed consumer, eight forms all 102 at 231 bytes;
// whole-slot forms 87/90/87/90 at 277 bytes; complete final rejection with native
// payload/self-dot 87/87; staged final failures 90/90; early returns with `return 0x10` 74
// (`mov ax`); the slot field on the left of the prop compare 74; a named actor with that
// compare 91; the existing magnitude3d for the speed 73, with early returns 74; a noinline
// declaration in this file 73 (ignored across files: still inlined); per-level returns 87
// (277 bytes), with `return 0x10` 87. A noinline on the definition would change the 21
// matched functions that inline it in retail.
// Round 37 (the single-exit form, all 88 at 278 bytes, the call still inlined): as is; with
// the existing magnitude3d; with the direct function_25d740 call; with the actor and prop
// node lookups written out (as matched 0x1b7b90 does); written out with magnitude3d.
// Round 27 (lane 11, on the single-exit form, 90 at 292 bytes): a const view 90; velocity declared
// before the view 90; `view != NULL` 90; the velocity query before the view lookup 59 at 264 bytes
// (the lookup still inlined; only the calls' new order realigns the listing, not kept). Our LTCG
// keeps 0x25d740 out of line in 0x1aa0d0/0x1aab50/0x1ca2d0/0x262ba0/0x264b50 and inlines it only
// here and in 0x261510.
// Round 20 (lane 15, fast): single exit with the slot's prop index compared in memory and the node indexed by it
// 109 (290 bytes); the slot field read first, actor prop index compared and used, `else` arm for the far tests 88
// (278 bytes), with the view declared beside the node and assigned late 88, with the slot field read after the actor
// lookup 88, with the view tested in its own nested if 88; diagnostics (not kept): the file built /Ob1 changes
// nothing (109, 0x1b7e40 unchanged), a second velocity query 108 and the view call before the near test 93 stay
// inlined. So neither the caller file's /Ob1, the call's position nor the caller's size moves LTCG's decision here.
// Round 110 (fast): 0x25d740 through the existing tracking_get with &tracking->view 73 (its code unchanged, still
// inlined here); `node->unknown28 < 0.8f` 73; `!(node->unknown28 >= 0.8f)` 73; `!(state->unknown20 <= 0)` 73.
// One-file LTCG harness (this file and src/unknown_25d690.cpp, reproduces the inlining): the site is on the edge of
// the inliner's frequency test: one more dominating test (an `if (!active) return` diagnostic) keeps the call out of
// line, while single exit, guard returns, the call in the 0.8 test's then-arm, `&&`-joined tests, slot field read
// first, `>= 1`, `<`, `!(>=)`, an inverted prop compare and deeper inline lookups all stay inlined, and so do the
// direct call and caller-size padding. In 0x25d740 a named index, tracking_get, a tracking_get early exit and an
// `&&` lookup keep it inlined; an early return, a ternary result, if/else results and an else that clears the
// result keep it out of line but change 0x25d740's own code (no leading xor, or a second NULL path), so retail's
// extra coldness comes from the call site, not from the callee.
// Round 13 (fast): the slot's data block typed through `slot->unknownc` (the slot-data-block form) 73 (same
// code: retail reads both fields off the slot pointer); 0x25d740's body through the existing prop_ref_view 73
// (still inlined here, its other callers unchanged), with the single-exit form 88; the single-exit form 88
// (278 bytes); the two outer tests joined with `&&` and the call in the else arm 88. BLOCKED-BY 0x25d740
// (LTCG inlines it at this call site only). The slot cast at each use, as 0x1b82d0 reads it, 73.
// Round 55 (fast with the file and its callees): the definition moved to the top / the end of the file 73/73;
// view and velocity declared at function scope 73; `if (!view) return result;` after the velocity query 73;
// the near arm as `result = 0x10;` with the far tests in its else 78. Our build also inlines 0x25d740 in
// unmatched 0x261510, where retail calls it.
// Round 55 (lane 10, fast; 0x25d740 listed, call still inlined in every form): velocity declared at the
// function's top 73; the far result assigned (`result = g_46fbe8;`) and returned at the end 78; view and
// velocity declared right after the node lookup 73; the near test with the call in its else arm and no early
// return 78; the prop compare as a guard return (`!=`) 73.
// Round 28 (m3 lane 18, fast; the call still inlined in every form): the node's distance field in a local for
// both tests 78; the speed named before the final test 73; the near result in an if/else with the far tests in
// its else 78; the prop compare as a guard return 73; the payload count as a guard return 79 (265 bytes).
// Round 125 (lane 8, fast with the file; the call still inlined in every form): the far distance test as
// `15.0f >= d`, `!(d > 15.0f)` and `!(15.0f < d)` 73 each; the speed cast to real 73; `view != NULL` 73.
// Round 128 (lane 8, fast with the file and its callees; 0x25d740 still inlined in every form): both outer tests as
// guard returns (`unknown20 <= 0`, `prop_index != unknown1c`) 74; with `if (!view) return result;` 74; with the
// existing magnitude3d 74; with the far result assigned (`result = g_46fbe8;`) 72; the near/far arms as an if/else
// assigning result with one return 88 (278 bytes).
// Round 69 (lane 11, fast; 0x25d740 inlined in every kept-shape form): a named near flag with two ifs, single exit 88 (278 bytes); the same
// with the near arm returning 112 (230 bytes, the flag test folds, the call goes out of line, exits merge); the two outer tests as a named
// flag retested 84; guard returns with the slot field read first 74; nested with it 73; an actor local with it 73. Diagnostic (reverted): 0x25d740
// pinned out of line 102 (231 bytes): ESI/EDI are still pushed at entry and the exits merge, where retail pushes ESI after the first test, EDI
// at the call, and has five separate exits, so the inlining is not the only cause.
// Round 609: actual count/expected-prop lvalues on payload/whole-slot graphs, then native node identity,
// 73/73/73/73/73/73/73/73. NOT kept: alias forms require a match; delayed saves and getter expansion remain.
// Round 394: sole-return goto, short/word/long/dword status, payload/whole-slot
// 88/88/92/92 in both (earlier single-result lowering, confirmations only).
// Actual status/prop-index eight-byte homes: status signed/unsigned 83/83, index
// signed/unsigned 92/92, four joint domains 106 each. Original body restored.
// MiniMax G (11-10): no new form, no lower score (73); the length (264) agrees, but our build still expands the call to 0x25d740, whose inline-ness is set in src/unknown_25d690.cpp, a file this lane may not edit, so the prologue's sunk ESI/EDI saves cannot be reached from here.
// MiniMax E (11-10): no new form, no lower score (73); the length (264) agrees, but our build saves esi and edi in the prologue before the seat's count test and still expands the call to 0x25d740, whose inline-ness is set in src/unknown_25d690.cpp.
// MiniMax E (11-10): no new form, no lower score: our build saves esi and edi in the prologue and
// tests the seat's count after the saves where retail tests it before any save, and retail calls
// 0x25d740 out of line where our build inlines it; the inline-ness is set in another file.
// MiniMax E (11-09): no new form, no lower score (73); our build saves esi and edi in the prologue and tests the
// seat's count after the saves, while retail tests it before any save and keeps esi as a scratch register (it
// pushes esi only after the count check and does not push edi at the top), so every instruction after the
// prologue is shifted; retail also calls 0x25d740 out of line where our build inlines it.
// MiniMax R (11-09): no new form, no lower score (73): the call to 0x25d740 is still expanded at
// this site, so the prologue still differs from retail's sunk ESI/EDI saves; the inline-ness is set
// in src/unknown_25d690.cpp, which this lane may not edit, and the forms that hold the call out of
// line (a noinline declaration or the file's own /Ob1 setting) all compile to the same bytes.
// MiniMax I (11-08): no new form, no lower score (73); still BLOCKED-BY function_25d740's inline-ness.
// MiniMax R (11-07, run 38): no new form, no lower score (73); still BLOCKED-BY function_25d740's inline-ness,
// which is set in src/unknown_25d690.cpp: retail calls it out of line and sinks the ESI/EDI saves, while our
// build inlines it at this site, so the prologue differs at +0x13.
// MiniMax C (11-06): no new form, no lower score; still BLOCKED-BY function_25d740's inline-ness,
// which is set in src/unknown_25d690.cpp: retail calls it out of line and sinks the ESI/EDI saves,
// while our build inlines it at this site, so the prologue differs at +0x13.
// MiniMax H (11-05): no new form, no lower score; retail calls function_25d740 out of line and sinks
// the ESI/EDI saves, while our build inlines it at this site only, and the inline-ness of that
// helper is set in src/unknown_25d690.cpp.
// MiniMax B (11-04): no new form, no lower score.
// MiniMax R (11-01): no lower score; reading the slot's expected index into a local before the actor lookup
// and the file's /Ob1 setting both leave it at 73 (0x25d740 is still inlined here); calling the view getter
// twice (76) does not hold it out of line either.
// MiniMax S (11-09): no new form, no lower score (73); the call to function_25d740 is still expanded at this
// site, so the prologue still differs from retail's sunk ESI/EDI saves.
// M3 lane 14 r83 (fast with the file, 0x25d740 and 0x261510; 0x25d740 still inlined in every form): the far tests with the
// distance first 73 (different code); the view's distance field test before the speed test 73 (different code); the
// velocity query before the view lookup on this early-return body 88 (252 B), with the distance first too 91 (252 B); a
// `state->unknown20 <= 0` guard return with the prop compare kept nested 79 (265 B); the view test as its own nested if 73
// (same code). The slot-data-block form was measured in round 13 (73, same code). src/unknown_25d690.cpp is open this
// round, but every recorded 0x25d740 spelling that keeps it out of line changes its own bytes: still BLOCKED-BY 0x25d740.
// MiniMax A (11-11, second pass): no new form, no lower score (73); what still differs is unchanged: our build still expands the call
// to 0x25d740 at this site, so the prologue still saves esi and edi before the seat count test where retail tests the count before any
// save and keeps that call out of line; the inline-ness of that helper is set in src/unknown_25d690.cpp, a file this lane may not edit,
// and the 16-bit absolute load of the global that retail ends with cannot be masked by the checker.
// MiniMax A (11-11): no new form, no lower score (73); our build still expands the call to 0x25d740 at this site, so the prologue still saves esi and edi before the seat count test where retail tests the count before any save and keeps that call out of line; the inline-ness of that helper is set in src/unknown_25d690.cpp, a file this lane may not edit.
// MiniMax H (11-11): no new form, no lower score (73); the body is unchanged, because retail calls
// function_25d740 out of line and sinks the ESI/EDI saves while our build inlines it at this site; the
// inline-ness of that helper is set in src/unknown_25d690.cpp, which this lane may not edit.
// M3 lane 12 r119 (fast with the file and 0x25d810/0x25d690/0x25d740/0x25d9b0): with the call held out of line by one
// diagnostic `#pragma inline_depth(0)`, this body MATCHES byte for byte: single exit (`short result = g_46fbe4;`, the
// near arm `result = 0x10;`, the view lookup, velocity query and far tests in its else, `result = g_46fbe8;`), the slot
// read as 0x1b82d0 reads it (`((s_slot_12 *)slot)->unknown20 > 0`, then `long local_0 = ((s_slot_12 *)slot)->unknown1c;`
// ahead of `s_actor_view *actor = actor_get(actor_index);`, `actor->prop_index == local_0`, the node from
// `actor->prop_index`). With a `state` local and the slot field read into a local (or on the left of the compare) the
// field folds into the compare: 90 at 250 bytes even out of line. Without the pragma that form is 90 at 292 bytes
// (re-measure of round 27's single-exit score). New: `__assume(node != NULL)` after the node lookup 73 on this body,
// 113 (290 bytes) on the matching form; the call stays inlined in both. Diagnostics: 60 extra one-line callers of the
// view getter in this file, held out of line or inlined, leave the site inlined (90, 292 bytes), so neither the
// helper's call-site count nor a shared inlining budget decides it. LTCG's inlining of 0x25d740 at this one site is
// the whole difference: BLOCKED-BY 0x25d740. Apply the form above once the call stays out of line. Also (callee side,
// reverted): 0x25d740's body through the existing tracking_get with that getter made `__forceinline` (one more forced
// inline level; 0x25d740 still matched) 90 (292 bytes, still inlined).
// @retail 0x1b81c0
short __stdcall function_1b81c0(long actor_index, s_slot *slot, bool active)
{
	s_slot_12 *state = (s_slot_12 *)slot;
	short result = g_46fbe4;

	if (state->unknown20 > 0)
	{
		long expected_prop_index = state->unknown1c;
		s_actor_view *actor = actor_get(actor_index);

		if (actor->prop_index == expected_prop_index)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (0.8f > node->unknown28)
				result = 0x10;
			else
			{
				s_prop_view_fields *view = node ? prop_node_view(node) : NULL;
				vector3f velocity;

				function_ba1d0(node->object_index, &velocity, NULL);
				if (view && sqrt(length_sq3f(&velocity)) < 1.0f && view->unknown54 < 0.31 && node->unknown28 <= 15.0f)
					result = g_46fbe8;
			}
		}
	}
	return result;
}

// @retail 0x1b82d0
bool __stdcall function_1b82d0(long actor_index, s_slot *slot, long index)
{
	bool result = true;
	long local_0 = ((s_slot_12 *)slot)->unknown1c;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != local_0)
		return false;

	bool local_1 = actor->unknown040;
	if (local_1)
	{
		if (((s_slot_12 *)slot)->unknown0c)
			local_1 = function_1f4810(actor_index, actor->prop_index, 0.3f, 0);
		else
			local_1 = function_1f4810(actor_index, actor->prop_index, 3.5f, 0);
		if (!local_1)
		{
			function_265c30(actor->prop_index, actor_index, true);
			actor_get(actor_index)->unknown040 = local_1;
		}
		result = local_1;
	}
	return result;
}

PRIVATE __forceinline s_actor_view *actor_step_1b83b0(long *index, long *current)
{
    if (*index == NONE)
        return NULL;
    *current = *index;
    s_actor_view *actor = actor_get(*index);
    *index = actor->next_index;
    return actor;
}

// @retail 0x1b83b0
short __stdcall function_1b83b0(long actor_index, long leader_index, s_slot *slot, s_invite_data *data)
{
	long count = 0;
	s_actor_view *actor = actor_get(actor_index);

	data->unknown4 = ((s_slot_12 *)slot)->unknown1c;
	if (actor->unknown07c == NONE)
		goto done;
	{
		long index = element_502420_get(actor->unknown07c)->first_actor_index;

		long other_index;
		s_actor_view *other;
		while ((other = actor_step_1b83b0(&index, &other_index)) != NULL)
		{
			if (actor != other && function_26eae0(leader_index, other_index, 3, 1.0f))
				count++;
		}
	}
done:
	return (short)count;
}

s_slot_handler_2x g_47e898 =
{
	{
		{
			0x12, 2, 0, -2, 0,
			function_1b7e40, function_1b81c0, joint_initiate, joint_leave, NONE, {0},
			0, 0, function_1b8460, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1b8070, 0, (t_slot_release)function_1b82d0, function_1b8360, function_1b8370, (t_slot_proc4)function_1b83b0,
	1, 10, 1.0f, 0
};


void function_26ee40(s_joint_behavior_state *behavior);
void function_267770(long prop_index, long actor_index);

// @retail 0x1b8070
long __stdcall function_1b8070(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    s_slot_entry_iterator iterator;
    iterator.actor_index = actor_index;
    iterator.reference.unknown2 = 0x12;
    iterator.reference.unknown0 = NONE;
    s_slot_memory_entry *entry;
    register s_slot_12 *state = (s_slot_12 *)slot;
    long joint_index = NONE;
    while ((entry = function_26f0c0(&iterator)) != NULL)
    {
        if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
        {
            joint_index = entry->unknown4;
            break;
        }
    }
    if (joint_index == NONE)
    {
        long candidate = function_26e940(actor_index);
        if (candidate != NONE)
        {
            state->unknown0c = true;
            state->element_index = candidate;
        }
        joint_index = candidate;
    }
    if (joint_index != NONE)
    {
        s_502424_element *element = element_502424_get(joint_index);
        if (state->unknown0c)
        {
            element->target.unknown0 = prop_node_get(actor->prop_index)->object_index;
            state->unknown1c = actor->prop_index;
        }
        else
        {
            long prop_index = function_25d810(element->target.unknown0, actor_index, false);
            if (prop_index != NONE)
            {
                state->unknown1c = prop_index;
                if (prop_index != actor->prop_index)
                    function_267770(prop_index, actor_index);
            }
            else
                goto failed;
        }
        real time = (real)g_510c54->field_2_3 * 10.0f;
        long ticks;
        __asm
        {
            fld time
            fistp ticks
        }
        state->unknown20 = ticks;
    }
done:
    return joint_index;
failed:
    if (joint_index != NONE)
    {
        function_26ee40((s_joint_behavior_state *)slot);
        return NONE;
    }
    goto done;
}
