// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_260670.CPP: scoring an actor's candidate positions and choosing the
   reference to follow */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"
#include <float.h>
#include "unknown_25fc30.h"

bool function_260160(long actor_index, s_prop_search *search, s_261d20_entry *entry);
#include "props.h"
#include <string.h>
typedef bool (__stdcall *t_reference_compare)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_reference_compare compare, const void *context);
bool __stdcall function_2600e0(long a, long b, void *unused);
void __stdcall function_25f7b0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index);

struct firing_position_post_evaluator
{
	short flags;
	bool (__stdcall *proc)(long, s_type_967e20 *, s_type_b36ac5 *);
};
extern firing_position_post_evaluator g_44adf0[12];
extern s_type_b36ac5 *g_51eca0;
short g_51ec9c;

struct s_reference_candidate_view;
struct s_reference_direction_request;
real function_30bf0(vector3f *vector);
void function_260530(s_reference_candidate_view *entry, s_reference_direction_request const *request);
void function_260060(long actor_index, short count, s_type_967e20 *context, s_type_b36ac5 *position);
bool function_2600a0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
long function_1e4a50(long index);
long function_262a30(s_reference reference);

// score 528 (was 528): frame 0x8a8 against 0x8a0, 3097 bytes against 3077; site 2 inlines 0x2600a0 where retail calls it, and 0x262a30 is
// called where retail inlines it around a call to 0x13e2d0 (see the r111 lines below the notes).
// Earlier 700 (was 700): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// a bool output in a register across a later call, as retail (0x1b0540 matched with plain output locals).
// Round 178 (m3 lane 4, plain checks; aligned = difflib count with addresses masked, 293 here): the call sequence is
// retail's except two callees: retail calls 0x2600a0 in the sorted loop and 0x13e2d0 for the owner, ours inlines both
// (the first evaluator pass is inline in both). The aggregate of five locals keeps retail's frame; as plain locals
// the result/bool/word stores take retail's forms but the frame grows to 0x8a4 (840), with the sort scratch word
// scoped to its block 0x8a0 again (832, aligned 272). Tried: plain locals with the 4c store before 0x270590, the
// first evaluator pass as a 0x2600a0 call and the owner store as if/else 847; those three without plain locals 719;
// plain locals and the evaluator call 832; scoped scratch with the four 841 (aligned 251); plus the history reset as
// an inline helper used twice, the segment test through the existing 0x11e000 and a float fabs 862 (frame 0x8ac,
// the helper outlined at one site); the helper __forceinline 849; without 0x11e000 856 (aligned 248); then the tail
// nested under `selected != NONE`, a position pointer and the pathfinding select as an if 854 (aligned 255); the
// sorted loop's body nested under the continue test 854. 0x2600a0 and 0x13e2d0 stay inlined in every form.
// 700 before r75. M3 lane 13 r77: 611 -> 528 plain (3097 bytes) by two changes together: the five state values as
// plain locals (the grouping aggregate removed), and 0x260060's second parameter as `short` (retail's call pushes count's raw dword with no
// movsx, and 0x260060 hands it to the pre-evaluator procs, whose typedef takes `short`; every 16-bit count load here then becomes
// retail's 32-bit load). Before r77: the owner is read through the existing 0x262a30 (its file is /Ob1, so the hash lookup 0x13e2d0 stays a call
// as in retail; the private copy of its lookup let 0x13e2d0 inline here): 700 -> 611 plain at 3072 bytes. Left: the five state values'
// stack homes (retail: bools 0x6/0x7, result 0x8, a dword-stored selected at 0xc, best score 0x18; ours keep them in one aggregate at
// 0x4..0x10), 0x2600a0 still inlined at the sorted-loop call where retail calls it, retail inlines 0x262a30's body (with 0x13e2d0
// called) where ours calls it, the random path's jump to the shared `selected == NONE` test, and x87/SSE operand orders in the loops.
// Round 75 (m3 lane 13, fast; aligned count in brackets, masked+stack, base 610 [205-270 masked]): first evaluator pass as a call to
// 0x2600a0 708 (613 with the owner call; retail's `result && proc` loop shape, but site 2 stays inlined); perpendicular distance through the
// existing 0x11e000 752 (frame 0x8ac); all three 629; `inline` on 0x262a30's definition 726 (0x13e2d0 inlines with it), with the evaluator
// call 721, with 0x11e000 756, both 750; the `selected == NONE` test after the if/else (retail's flow) 623 [262], the tail nested under
// `selected != NONE` 628, both with the inline 724/737; the sorted loop as a while 610, its call test nested 610, the 25f7b0/score/
// 2600a0 steps as an inline helper 610 (same code: site 2 stays inlined in every form); the five state values as plain locals 742
// (frame 0x8a4, retail's dword store for selected and best at 0x18), in another order 742, with the sort scratch word declared in its
// block 734 [240] (frame 0x8a0), with the sorted array there too 730, plus the shared selected test 752 [238], plus normalize3d and the
// evaluator call 766 [230]; normalize3d for the unknown40 loop 610 [268] (retail's x87 length order exactly; only the scale products
// differ). Earlier: score 700 (was 697): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done`
// as returns 700 (697 with them). Enabling it lets callers keep
// Round 77 (m3 lane 13, fast unless noted; masked aligned count, then masked+stack, in brackets; base 528 [278/168], frame 0x8a8 against
// 0x8a0, 3097 bytes): the short parameter alone 707 [226/172] (3083), plain locals alone 742 (the r75 form). Retail packs the two
// bools into one dword (0x6/0x7) and the sort scratch word into the actor offset's dead slot (0x14); ours gives each its own dword.
// The bools declared first 528 (same code); the sort scratch word declared in its block 528 [277/171] (frame 0x8a4); the first
// evaluator pass as the existing 0x2600a0 610 [243/162] (frame 0x8a4: the written-out loop's `possible` bool had split the two bools,
// which now pack at 0x2/0x3; 3057 bytes, site 2 still inlined, the loop head padded where retail's is not); both 605 [198/156] (frame
// 0x8a0 as retail, 3041 bytes); the `selected == NONE` test after the if/else on the 528 base 567 [271/161] (3045), on the 605 form
// 624 [197/155] (3021). The score is lockstep by instruction, so these structural gains read as rises while the body stays short of
// retail's length: retail inlines 0x262a30 (calling 0x13e2d0) and calls 0x2600a0 at site 2; ours does the opposite (file flags).
// (v2-v6 above are forms of the r75 list re-measured on the new base.) New on the 528 base: both squared lengths through the
// existing length_sq3f 528 (same code), the projection dot and both perpendicular squared lengths through dot3f/length_sq3f 632
// [274/164], the projection update through vector3f_add_scaled 528, the delta's scale through vector3f_scale 528, the reported
// entry as an aggregate assignment in place of memcpy 528 (all same code but the second). Aligned counts of the kept change,
// plain: exact 393 -> 413, masked 266 -> 278, masked+stack 205 -> 168; frame 0x8a0 -> 0x8a8, length 3072 -> 3097 (retail 3077).
// MiniMax S (11-10): no new form, no lower score (700): retail keeps the two flags as separate byte locals at F+6/F+7,
// the reference at F+8 and the selection in a four-byte slot at F+0xc (a dword -1 store with word compares), while our
// state aggregate puts the flags at F+0xc/F+0xd and the selection in a word at F+0xe. Splitting the aggregate into
// standalone locals gives retail's F+8 and F+0xc but leaves the flags at F+2/F+3 and the frame at 0x8a4 against retail's
// 0x8a0 (841), the same split with a four-byte selection scores 836, and an aggregate whose flags come first is kept
// in registers by the compiler (836), so the aggregate stays.
// M3 lane 12 r111 (plain; 0x2605d0, 0x2613d0, 0x2600a0, 0x262a30, 0x13e2d0 and the other users of 0x11e000/0x2600a0 stay matched;
// aligned = difflib count, then with registers masked, base 278/106). The perpendicular block is 0x11e000's body: calling
// `function_11e000(target, actor position, &delta)` inside the sqrt 629 (3097 B, 284/108; it inlines, but with the contributor's
// unmatched statement order and a spill); with the first evaluator pass as `function_2600a0(actor_index, context, NULL)` as well 529
// (3072 B, frame 0x8b0, 246/100: site 1 then compiles to retail's inline `call edx` loop); plus the random pick through the existing
// random_index 529 (246/99); site-1 call and random_index without 0x11e000 543 (3056 B, frame 0x8a4, 240/101); on the base, the
// delta's length block (normalize3d's body with its arms swapped) as `position->unknown18 = normalize3d(&delta)` 576 (frame 0x8ac,
// 277/103: the block itself becomes retail's, float constant, `ja` and the zero store). Diagnostic, not kept: /Ob1 on this file
// 624 (site 2 still inlined: LTCG inlines by the callee's file, so retail's site-2 call needs 0x2600a0's file built without
// auto-inlining, a flags decision). Next: the helper calls together (0x2600a0, random_index, normalize3d, and 0x11e000 once
// 0x11e000 matches, BLOCKED-BY 0x11e000) on one base; most of the positional count is the frame size moving every parameter
// offset. Body restored.
// @retail 0x260670
s_reference __stdcall function_260670(long actor_index, s_2605d0_request const *request,
	s_261d20_entry *entries, short count, long reported_entry, long reported_owner, byte *scratch, bool *has_path)
{
	byte *actor = (byte *)actor_get(actor_index);
	s_type_967e20 *context = (s_type_967e20 *)request;
	byte *parameters = (byte *)context;
	s_type_b36ac5 *positions = (s_type_b36ac5 *)entries;
	s_reference result = g_470fa0;
	bool within_limit = false;
	bool finite_distance = false;
	short selected = NONE;
	real best_score = 0.0f;
	s_path_source source;
	s_path_settings settings;
	long sorted_indices[512];
	long sort_scratch;
	*has_path = false;
	((byte *)g_4f55d0)[3] = true;
	if (count == 0)
	{
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		return g_470fa0;
	}
	if (parameters[0x618] && parameters[0x53])
	{
		if (parameters[0x56])
		{
			for (short i = 0; i < count; i++)
				function_260530((s_reference_candidate_view *)&positions[i], (s_reference_direction_request *)parameters);
		}
		else if (*(long *)(parameters + 0x64c) != NONE)
		{
			memset(&source, 0, sizeof(source));
			source.radius = *(real *)((byte *)function_1e4a50(*(long *)(actor + 0x54)) + 4);
			source.unknown04 = actor[0x3e4];
			source.object_index = NONE;
			source.unknown0c = NONE;
			function_270590(&source, (s_type_c3b527 *)(parameters + 0x650), *(long *)(parameters + 0x64c));
			source.unknown4c = 0.0f;
			source.unknown45 = true;
			source.unknown48 = 20.0f;
			function_1f9240(actor_index, &settings);
			byte *buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
			function_2715a0(buffer);
			for (short i = 0; i < count; i++)
			{
				s_type_b36ac5 *position = &positions[i];
				if ((position->unknown58 && parameters[0x5a]) || (position->unknown59 && parameters[0x58]))
					function_260530((s_reference_candidate_view *)position, (s_reference_direction_request *)parameters);
				else
					function_270750(buffer, position->definition->unknown14, (s_actor_point_target *)position->definition,
						&position->unknown28, 0, parameters[0x54] ? (long)&position->unknown34 : 0);
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	else if (parameters[0x54])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown34);
			function_30bf0(&positions[i].unknown34);
		}
	}
	if (parameters[0x55])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown40);
			normalize_inline(&positions[i].unknown40);
		}
	}
	if (!parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		source.unknown45 = true;
		source.unknown48 = *(real *)(parameters + 0x1c);
		if (parameters[0x46] && parameters[0x618])
		{
			long target_object = NONE;
			if (*(long *)(parameters + 0x664) != NONE)
				target_object = prop_ref_get(*(long *)(parameters + 0x664))->object_index;
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(parameters + 0x620);
			*(long *)(source.unknown28 + 0x10) = target_object;
			*(real *)(source.unknown28 + 0x14) = *(real *)(parameters + 0x4c);
			source.unknown44 = !parameters[0x50];
			*(real *)(source.unknown28 + 0x18) = *(real *)(parameters + 0x48);
		}
		else if (*(short *)(actor + 0x358) > 0)
		{
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(actor + 0x370);
			*(real *)(source.unknown28 + 0x14) = *(real *)(actor + 0x36c);
			*(long *)(source.unknown28 + 0x10) = *(long *)(actor + 0x360);
			source.unknown44 = true;
			*(real *)(source.unknown28 + 0x18) = 10.0f;
		}
		function_1f9240(actor_index, &settings);
		function_271300((s_type_f17a25 *)scratch, NULL, &settings, &source, 0);
		if (function_2715a0(scratch))
			*has_path = true;
	}
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];
		if (parameters[0x618])
		{
			vector3f delta;
			vector3d_from_points3d((point3f *)(parameters + 0x620), &position->position, &delta);
			position->unknown30 = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		}
		vector3f delta;
		vector3d_from_points3d((point3f *)(actor + 0x238), &position->position, &delta);
		real distance_squared = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		real range = *(real *)(parameters + 0x1c);
		if (range * range > distance_squared)
		{
			if (!parameters[0x56] && !(position->unknown58 && parameters[0x5a]) && !(position->unknown59 && parameters[0x58]))
				function_270750(scratch, position->definition->unknown14, (s_actor_point_target *)position->definition,
					&position->unknown18, (long)&position->unknown2c, parameters[0x51] ? (long)&position->unknown1c : 0);
			else
			{
				real perpendicular_squared;
				if (distance_squared > 0.0001f)
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(actor + 0x238), (point3f *)(parameters + 0x620), &target_delta);
					real t = (target_delta.k * delta.k + target_delta.j * delta.j + target_delta.i * delta.i) / distance_squared;
					t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;
					t = 0.0f - t;
					target_delta.i = t * delta.i + target_delta.i;
					target_delta.j = t * delta.j + target_delta.j;
					target_delta.k = t * delta.k + target_delta.k;
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				else
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(parameters + 0x620), (point3f *)(actor + 0x238), &target_delta);
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				position->unknown2c = (real)sqrt((double)perpendicular_squared);
				real length = (real)sqrt((double)distance_squared);
				if ((double)0.0001f > fabs((double)length))
					length = 0.0f;
				else
				{
					real scale = 1.0f / length;
					delta.i *= scale;
					delta.j *= scale;
					delta.k *= scale;
				}
				position->unknown18 = length;
				if (parameters[0x51])
					position->unknown1c = delta;
			}
		}
		if (*(real *)(parameters + 0x1c) > position->unknown18)
			within_limit = true;
		else
		{
			position->unknown4c = false;
			if (FLT_MAX > position->unknown18)
				finite_distance = true;
		}
	}
	if (!within_limit && !finite_distance && !parameters[0x56] && !actor[0x264] &&
		(*(long *)(actor + 0x28c) == NONE || (*has_path && count > 0 && *(short *)(scratch + 0xae) < 8)))
		++*(short *)(actor + 0x5e8);
	else
		*(short *)(actor + 0x5e8) = 0;
	if (parameters[0x15] && !within_limit)
	{
		dword *seed = (dword *)g_4e7408;
		*seed = *seed * 0x19660d + 0x3c6ef35f;
		selected = (short)(((*seed >> 16) * count) >> 16);
		*has_path = false;
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		if (!function_260160(actor_index, (s_prop_search *)context, &entries[selected]))
			return result;
	}
	else
	{
		function_260060(actor_index, count, context, positions);
		for (short i = 0; i < count; i++)
			sorted_indices[i] = i;
		g_51ec9c = count;
		g_51eca0 = positions;
		sort_4byte(sorted_indices, count, &sort_scratch, (t_reference_compare)function_2600e0, NULL);
		context->unknown680 = 0.0f;
		bool possible = true;
		for (long evaluator = 0; g_44adf0[evaluator].proc && possible; evaluator++)
		{
			if ((1 << context->type) & g_44adf0[evaluator].flags)
				possible = g_44adf0[evaluator].proc(actor_index, context, NULL);
		}
		parameters[0x67c] = possible;
		for (short i = 0; i < count; i++)
		{
			short index = (short)sorted_indices[i];
			s_type_b36ac5 *position = &positions[index];
			if (!position->unknown4c || (parameters[0x67c] && best_score >= position->score + context->unknown680))
				break;
			if (parameters[0x618])
				function_25f7b0(actor_index, context, position);
			position->unknown50 = position->score;
			if (function_2600a0(actor_index, context, position) && position->score > best_score)
			{
				selected = index;
				best_score = position->score;
			}
		}
		if (selected == NONE)
			return result;
	}
	if (reported_entry)
		memcpy((void *)reported_entry, &positions[selected], sizeof(s_type_b36ac5));
	result = positions[selected].reference;
	if (reported_owner)
	{
		long owner = function_262a30(result);
		*(long *)reported_owner = owner != actor_index ? owner : NONE;
	}
	if (actor[0x3f2] && !actor[0x220] && !positions[selected].unknown5b)
		actor[0x3f2] = false;
	if (parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		s_type_f17a25 *path = (s_type_f17a25 *)scratch;
		path->unknown54 = false;
		path->unknown90 = NONE;
		path->unknownae = 0;
		path->heap_count = 0;
		path->location.unknown00 = 0;
		path->location.unknown02 = 0;
		path->pathfinding = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ? *(void **)((byte *)g_4e0348 + 0xc8) : NULL;
		path->flags = 0;
		path->unknownac = false;
		path->source = source;
		memset(&path->settings, 0, sizeof(path->settings));
		path->unknown14188 = 0;
		memcpy(path->unknown140d4, parameters + 0x6a0, 0xb6);
		*has_path = true;
	}
	return result;
}
