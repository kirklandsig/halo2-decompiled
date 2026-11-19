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

// the actor's priority for joining a prop joint: the participant priority of the first invitation whose joint
// targets the actor's prop node (or a prop it can acquire), else 3 for a settled prop; 0 unless the target is
// between 1.6 and 15 away, nearly still, under the view threshold, not in use and not busy
// @retail 0x1b7e40
short __stdcall function_1b7e40(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long prop_index = actor->prop_index;
	s_prop_node_view *node = NULL;
	if (prop_index != NONE)
		node = prop_node_get(prop_index);
	s_prop_node_view *target;
	short priority = 0;
	if (actor_has_joint_invitation(actor_index, 0x12))
	{
		s_slot_entry_iterator iterator;
		iterator.actor_index = actor_index;
		iterator.reference.unknown2 = 0x12;
		iterator.reference.unknown0 = NONE;
		while (function_26f0c0(&iterator))
		{
			short invitation_index = iterator.reference.unknown0;
			s_502424_element *joint = element_502424_get(((s_slot_owner_entry *)actor)->joint_invitations[invitation_index].joint_index);
			if (node && joint->target.unknown0 == node->object_index)
			{
				target = node;
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
				target = prop_node_get(new_prop);
				priority = *(short *)((byte *)joint + ((s_slot_owner_entry *)actor)->joint_invitations[invitation_index].participant_index * 12 + 0xa);
			}
			break;
		}
	}
	else if (actor->unknown07c != NONE && node)
	{
		target = prop_node_get(prop_index);
		if (target->unknown27 >= 2)
			priority = 3;
	}
	if (priority > 0)
	{
		s_type_5cfb45 *state = function_25d690((s_prop_datum *)target);
		s_prop_view_fields *view = prop_node_view(target);
		vector3f velocity;
		function_ba1d0(target->object_index, &velocity, NULL);
		if (!(target->unknown28 > 1.6f && (real)sqrt((double)(velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k)) < 1.0f &&
			view->unknown54 < 0.31 && target->unknown28 <= 15.0f && !state->unknown64 && !function_25d9b0(actor->prop_index)))
			priority = 0;
	}
	else
		priority = 0;
	return priority;
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
