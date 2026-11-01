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

// score 49 (was 86): the `priority > 0` gate after both branches gives retail's block order (no-invitation
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

// score 73 (was 73): retail calls function_25d740 out of line and sinks the ESI/EDI saves; our
// build inlines it, so the prologue differs at +0x13. This round: reading the slot's expected
// index into a local before the actor lookup, the reversed compare, a named actor local and the
// separate-exit forms compile byte-identically (73) or worse (85-87 at 272-277 bytes); holding
// the call out of line needs a noinline declaration in src/unknown_25d690.cpp.
// score 73 (was 73): retail calls function_25d740 out of line (ECX node) and sinks the
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
// @retail 0x1b81c0
short __stdcall function_1b81c0(long actor_index, s_slot *slot, bool active)
{
	s_slot_12 *state = (s_slot_12 *)slot;
	short result = g_46fbe4;

	if (state->unknown20 > 0)
	{
		long prop_index = actor_get(actor_index)->prop_index;

		if (prop_index == state->unknown1c)
		{
			s_prop_node_view *node = prop_node_get(prop_index);

			if (0.8f > node->unknown28)
			{
				result = 0x10;
				return result;
			}

			s_prop_view_fields *view = prop_node_view(node);
			vector3f velocity;

			function_ba1d0(node->object_index, &velocity, NULL);
			if (view && sqrt(length_sq3f(&velocity)) < 1.0f && view->unknown54 < 0.31 && node->unknown28 <= 15.0f)
				return g_46fbe8;
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
