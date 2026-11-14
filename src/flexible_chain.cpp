// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_211060.h"
#include "vector_products.h"
#include "normalize3d.h"
#include <math.h>

// Layouts follow the pool and tag accesses in the retail callback group.
struct s_chain_node
{
	point3f position;
	vector3f velocity;
	real texture_scale;
	short ticks;
	short unknown1e;
};

struct s_chain_record
{
	short salt;
	short unknown02;
	byte unknown04;
	bool fixed;
	short idle_ticks;
	long tag_index;
	long object_index;
	point3f origin;
	s_chain_node nodes[21];
};

struct s_chain_segment
{
	real stiffness;
	byte unknown04[0x20];
	real length;
	short sequence;
	byte unknown2a[0x4a];
	vector3f rest_offset;
};

struct s_chain_definition
{
	long marker_name;
	long unknown04;
	long bitmap_index;
	long unknown0c;
	long physics_index;
	byte unknown14[0x50];
	real stiffness;
	byte unknown68[0x30];
	long segment_count;
	s_chain_segment *segments;
};

struct s_chain_frame
{
	short bitmap;
	byte unknown02[6];
	real left;
	real right;
};

struct s_chain_sequence
{
	byte unknown00[0x34];
	long frame_count;
	s_chain_frame *frames;
};

struct s_chain_bitmap_view
{
	byte unknown00[0x38];
	short border;
	short unknown3a;
	long sequence_count;
	s_chain_sequence *sequences;
};

s_record_pool *g_4e0334;
struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
void function_11bed0(s_location *location, point3f const *point);
real function_11ce20(vector3f const *a, vector3f const *b);
typedef void (__stdcall *chain_draw_callback)(long, long);
void function_47870(long object_index, long chain_index, point3f const *origin, chain_draw_callback draw);

void __stdcall function_1161f0(s_chain_record *record, s_chain_definition *definition, real dt);
void function_116750(s_chain_record *record, s_chain_definition *definition, s_location *location, point3f *origin, vector3f *direction);

// @retail 0x115d10
void __stdcall function_115d10()
{
	g_4e0334 = data_new_inlined("antenna", 12, sizeof(s_chain_record), 0, g_510c2c);
}

// @retail 0x115d50
void __stdcall function_115d50()
{
	g_4e0334->valid = 1;
	record_pool_release_all(g_4e0334);
}

// @retail 0x115d70
void __stdcall function_115d70()
{
	g_4e0334->valid = 0;
}

// @retail 0x115d80
void __stdcall function_115d80()
{
	if (g_4e0334)
		g_4e0334 = 0;
}

// @retail 0x115da0
long __stdcall function_115da0(long tag_index, long object_index)
{
	long result = NONE;
	if (tag_index != NONE && object_index != NONE)
	{
		s_chain_definition *definition = (s_chain_definition *)g_4e3b44[tag_index & 0xffff].bytes;
		result = record_pool_allocate(g_4e0334);
		if (result != NONE)
		{
			s_chain_record *record = (s_chain_record *)(g_4e0334->data + (result & 0xffff) * sizeof(s_chain_record));
			record->unknown04 = 0;
			record->fixed = definition->segment_count < 2;
			point3f *origin = &record->origin;
			origin->x = origin->y = origin->z = 0.f;
			record->tag_index = tag_index;
			record->object_index = object_index;
			record->idle_ticks = 0;
			point3f position = record->origin;
			short i;
			for (i = 0; i < definition->segment_count; ++i)
			{
				s_chain_node *node = &record->nodes[i];
				s_chain_segment *segment = &definition->segments[i];
				node->position = position;
				node->velocity.i = node->velocity.j = node->velocity.k = 0.f;
				node->ticks = 0;
				node->texture_scale = 0.f;
				if (definition->bitmap_index != NONE)
				{
					s_chain_bitmap_view *bitmap = (s_chain_bitmap_view *)g_4e3b44[definition->bitmap_index & 0xffff].bytes;
					if (segment->sequence >= 0 && segment->sequence < bitmap->sequence_count)
					{
						s_chain_sequence *sequence = &bitmap->sequences[segment->sequence];
						if (sequence->frame_count)
						{
							s_chain_frame *frame = sequence->frames;
							s_type_7ba8e9 *image = function_137550(definition->bitmap_index, frame->bitmap);
							if (image)
								node->texture_scale = segment->length / ((frame->right - frame->left) * *(short *)((byte *)image + 4) - bitmap->border * 2.f - 1.f);
						}
					}
				}
				position.x = segment->rest_offset.i + position.x;
				position.y = segment->rest_offset.j + position.y;
				position.z = segment->rest_offset.k + position.z;
			}
			s_chain_node *terminal = &record->nodes[i];
			terminal->position = position;
			terminal->velocity.i = terminal->velocity.j = terminal->velocity.k = 0.f;
		}
	}
	return result;
}

// @retail 0x115ff0
void __stdcall function_115ff0(long index)
{
	record_pool_release(g_4e0334, index);
}

// @retail 0x116010
void __stdcall function_116010(long unused, long index)
{
	s_chain_record *record = (s_chain_record *)(g_4e0334->data + (index & 0xffff) * sizeof(s_chain_record));
	s_chain_definition *definition = (s_chain_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
	if (!record->fixed)
	{
		if (record->idle_ticks > 5)
		{
			function_1161f0(record, definition, .05f);
			function_1161f0(record, definition, .05f);
			function_1161f0(record, definition, .05f);
		}
		record->idle_ticks = 0;
	}
}

// Retail submits the empty ret-8 body at 0x24dc50. The same body is already
// represented by a virtual method elsewhere; this ordinary callback needs
// the two-argument function-pointer form used by the submission queue.
PRIVATE void __stdcall chain_draw_empty(long, long)
{
}

// @retail 0x116080
void __stdcall function_116080(short mode, long index)
{
	if (mode == 0)
	{
		s_chain_record *record = (s_chain_record *)(g_4e0334->data + (index & 0xffff) * sizeof(s_chain_record));
		function_47870(record->object_index, index, &record->origin, chain_draw_empty);
	}
}

// @retail 0x1160c0
void __stdcall function_1160c0(real dt)
{
	long index = data_datum_index(g_4e0334, function_16bc00(g_4e0334, 0));
	while (index != NONE)
	{
		long slot = index & 0xffff;
		s_chain_record *record = (s_chain_record *)(g_4e0334->data + slot * sizeof(s_chain_record));
		s_chain_definition *definition = (s_chain_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
		if (!record->fixed)
		{
			++record->idle_ticks;
			if (record->object_index != NONE && record->idle_ticks < 5)
			{
				function_1161f0(record, definition, dt > .06666667f ? .06666667f : dt);
			}
		}
		index = data_datum_index(g_4e0334, data_find_index(g_4e0334, index == NONE ? 0 : slot + 1));
	}
}

// score 171 (was 171): previous/axis homes, segment ownership and rotation scheduling differ;
// seven local-name sets, declaration reversals and entry/loop scopes 194;
// shared cross 207, both point differences 286, both helpers 280;
// scaled difference 212, all vector helpers 285.
// MiniMax B (11-12): no new form, no lower score (171): the body is retail's length (1368 bytes) and the first difference
// is the register pair at the segment's axis load, retail keeping the value in EDX where this build keeps it in EDI and
// then in ECX; the permuter on this address was killed by its time limit without a result.
// MiniMax H (11-12): no new form, no lower score (171): the rotation's y and z cross terms are still scheduled after the x row where retail puts them right after the dot, and retail loads all six axis and offset components into registers first where this build reads offset k from memory at the dot and at two later products.
// frame 0xa4 agrees; attachment homes now agree. Retail keeps the rest offset
// below previous and the scalar temporaries below the axis; those still differ. The segment
// pointer lives in edx (ours edi) and the rotation's products are scheduled in another order.
// MiniMax D (11-08): no new form, no lower score (171); retail builds the segment address in EDX and keeps it at
// +0x14 and the loop's previous values at +0x5c, ours borrows EDI with the segment at +0x1c and the previous values
// at +0x50; the segment as a base pointer plus an index, as a named index and as an if/else all give the same
// code, and the base-then-add form (which puts the segment in EDX) scores 281.
// MiniMax I (11-06): no new form, no lower score; the stiffness product with the operands swapped keeps 171 and the node position read into its three components gives 220.
// MiniMax A (11-05): no new form, no lower score; the segment selected before the node 279 (as before),
// and our segment base takes EDI with the segment pointer in ECX where retail keeps both in EDX, the
// node-position copy uses EDX where retail uses ECX, and the physics index is masked in EAX before the
// pushes where retail masks ECX after them.
// MiniMax S (11-01): the segments and segment-index locals leave the first difference at +0x37; no lower score.
// frame 0xa4 agrees; attachment homes now agree. Retail keeps the rest offset
// below previous and the scalar temporaries below the axis; those still differ. The segment
// pointer lives in edx (ours edi) and the rotation's products are scheduled in another order.
// Round 62: the existing normalize3d for the axis and dot3f for the projection give the same code (194)
// but cost 0x73360 its match (this file links ahead of unknown_058dd0.cpp), so not kept; forward
// declared before origin 194; the three rotation cross terms as one vector 275; previous/predicted at
// function scope 194. Earlier: origin/forward, previous/offset and axis/length/scale names, singly and
// combined, 194 each.
// Round 371: first-node position copied before direction 179, kept; previous-first
// publication, complete scalar cross/axial terms and their combinations 179 each.
// Round 249: Complete rest-offset/recurrent-previous/working-position CRT copies,
// all seven nonempty combinations 179 each; all overlap-safe copies 419 (frame
// 0xa0, 1266 bytes). Original native copies, homes and arithmetic restored.
// Complete previous-position scalars, with complete axis/rest-offset/prediction
// coordinate sets 345, 335, 332, 323, 345, 335, 332, 323.
// Native segment index before lookup, explicit last-segment arm, uniform
// rest-offset fields and invariant reciprocal combinations 179, 179, 336, 336, 189, 189, 338, 338.
// Round 219: Exhaustive segment-pointer result, inner/outer invariant vertical and sine/cosine
// order 297/297/309/309; default-pointer counterparts 281/281/309/309. All restored;
// segment/previous/axis homes and rotation schedule remain.
// Round 380: matched symmetric angle, segment-first, tick-before-stiffness and delayed
// rest-offset combinations 177/276/181/279/213/271/216/273; two-point best dropped.
// Original angle arguments, producers and complete executable body restored.
// Round 382: complete delta/axis squared-product phases, three-use free blend weight,
// all combinations and full projection products 179/179/179/179/179/179/179/179.
// Original product phases and complete executable body restored.
// Round 384: native node/segment element references, readonly segment, their combinations and
// two-use physics-index capture 179 each (eight forms).
// Original complete body and all provider interfaces restored; frame 0xa4, segment/previous/axis
// homes and rotation schedule remain.
// Round 223: Existing target/interpolation/prediction point constructors, seven subsets 179/179/179/179/179/179/179; all with complete direction/velocity setters 183. Original producers and body restored.
// Round 386: preserve native previous aggregate; independent complete rest-offset,
// axis and prediction coordinate domains in all seven subsets, then native role names
// 280/197/289/216/273/193/293/293; original native domains and complete body restored.
// Round 387: Complete delta/axis squared sums 179/179/179; native double distance-root alone and with those sums 249/249/249/249, both named roots 249. All expression stages restored.
// Round 32: existing in-place axis scale 179; original-angle segment-first 276;
// tiny-axis-owned fallback 294; scale/segment 276; scale/fallback 294; segment/fallback 247;
// all 247, cosine-first all 247. Original producers, fallback and include set restored.
// Round 251: interpolation-owned stiffness 315; velocity-owned reciprocal 294;
// both 320; rest offset after readonly angle/trigonometry 229; with stiffness/
// reciprocal/both 299/288/310; all with the native prediction constructor 310.
// Original producer points and complete body restored; segment/previous/axis homes remain.
// Round 393: published-node previous, three-use old-position snapshot and native velocity
// publication subsets 179/263/263/262/262/390/390; all with immutable snapshot 390. Body restored.
// Round 262: complete segment/node/both receivers at uses 350/193/359;
// repeated stiffness alone/segment/node/both 301/475/309/480 (restored);
// separate segment base 278. Original receivers and computed stiffness restored.
// Round 226: complete rotation base sums, sine-scaled transverse terms and rotated components,
// all seven combinations and full graph with existing point publication 179 each.
// Original body restored; segment/previous/axis homes and rotation scheduling remain.
// Round 395: native node cursor 329; with readonly segment 329, invariant reciprocal 327, both 327;
// invariant vertical on those four graphs 338/338/326/326. All original induction and scopes restored.
// Round 397: one direction workspace; complete target scalars, noninitial branch first and readonly segment masks 0..7
// 246/318/277/323/246/318/277/323; all restored.
// Round 185: Plain target/interpolation/velocity coordinate-loop subsets
// 188/194/200/639/194/200/634; all with complete direction loop 615. Original native arithmetic
// statements, copies, segment loop, homes and every call tuple restored.
// Round 181: outer/per-selection count 298/179; inclusive bound live/selection/outer
// 302/302/235; ordered terminal selection live/selection/outer 180/180/298. Original body restored.
// Y423: Existing staged cross with paired differences, scaled target and velocity
// helper subsets 192/265/186/265/204/273/198/273 fast (control 179);
// all regress. Original complete body and include list restored.
// N226: Current position owns the constrained target; direction-workspace/axis-scale/
// blend-owned-stiffness masks 0..7 give 208/252/208/252/285/334/285/334.
// All shorten the body and lose the original frame; complete original body restored.
// Round 46: retail's low frame slots run length +0, segment/scale +4, axis +8..+0x10, rest offset
// +0x40, previous +0x4c; ours run axis, segment/scale, length and previous +0x40 (the order is
// reversed). Retail scales axis j/k in place on reloaded values (`mulss xmm3, xmm0`). Length declared
// before axis 179; inverse time step before the loop 189; segment pointer before node 276; vertical
// before the loop 301; existing normalize3d for the axis 179 (identical code, not kept: round 62
// found it costs 0x73360 in the full build).
// Round 208: Prediction rows xzy/yxz/yzx/zxy/zyx: 217/217/187/187/218; reverse prediction with
// velocity rows jik/jki/kji: 218/217/217. Original row order restored; two forms keep 0xa4 but
// regress, the others enlarge the frame. Segment/previous/axis homes and rotation schedule remain.
// Round 242: Long/word/signed-byte/byte node ordinals 290/180/282/282;
// with invariant reciprocal 296/190/279/279. Original short induction,
// reciprocal scope and complete body restored; segment/previous/axis homes remain.
// Round 43 (one-file LTCG link of this file with the callee closure and the inlining ballast object,
// equal to the full listing): renaming axis/length/segment/previous/offset (five name sets, both
// orders of each pair) leaves every stack home and instruction unchanged, so the reversed slot order
// does not follow local names; the rotation through an inline (vector, axis, sine, cosine) helper
// on an offset copy, explicit or staged cross terms, about 240 (harness 218 against 161, 1370 bytes);
// stiffness computed after the tick increment about 183 (harness 165); the axis scaled with *= 179 (same code).
// Round 53: the axis j/k (or all three) scaled with `*=` 179/179 (same code); the segment index in a short
// local 290; `definition->segments + (...)` 179; length declared then assigned 179.
// Round 61 (lane 16, fast): the float library forms for the roots and the angle: sinf/cosf for the rotation 182
// (aligned 198 -> 200), sqrtf for the axis length 179 and for the segment-length scale 179 (same code), fabsf
// for the length test 179, all five together 182. Retail's frame runs length, segment, axis upward from
// [esp+0x10]; ours axis, segment, length; the axis j/k scaling reloads the stored component in retail.
// Lane 18 round 17 (an LTCG link of this file's generated unit, 180 objects, equal to the full listing; 172
// there for 179): the physics tag in a one-use local 172 and the physics index in a local used for the test and
// the lookup 172 (both identical; the tag address is still formed before the -1 push); 1/dt before the loop 182;
// a long loop index 289 (1358 bytes); the root test as `fabs(length) < .0001f` 172.
// Round 35 (lane 13): rest-offset copy at the loop top (before the tick) 289 (1354 bytes; offset still homes
// above previous); length/axis declared at the loop-body top 179 (same code); previous renamed last_position
// 179 (no slot moves); the dot named before the complement product 179 (same code); a 600 s permuter run
// (55 tries) none below 179.
// Round 264: Existing initial/normalized axis setters and const native delta/target/offset,
// subsets 1/2/4/3/5/6/7, then all with const assigned-once scalars 192/179/179/192/192/179/192/192.
// Original producers and complete body restored; segment/previous/axis homes remain.
// Round 54 (m3 lane 1, fast): retail's angle call passes direction in ECX and the up vector in EDX, which is
// `function_11ce20(&vertical, &direction)` (0x11ce20 matches; the angle is symmetric): 177, the call setup then
// agrees (two points, not kept). The rest offset declared at function scope before previous 179 (identical code:
// the homes do not follow scope); on the swapped call: the prediction rows after the velocity and position
// publication 213; the rest offset copied right after the segment lookup 292 (1354 bytes); the blend complement
// named and used three times 177. Retail mirrors two groups of homes: length/cosine +0x10, segment/scale/sine
// +0x14, axis +0x18 (ours axis +0x10, length +0x20) and offset +0x50, previous +0x5c (ours reversed).
// Round 49 (lane 10, fast): the angle call as function_11ce20(&vertical, &direction) (retail's ECX/EDX setup:
// direction in ECX, vertical in EDX) 177; on that: the rotation through a private inline Rodrigues helper on the
// offset copy (v*cos + projection*axis - cross*sine) with predicted = position + offset 236 (1370 bytes); the dot
// and cross operands written offset-first 177 (same code); the axis through the existing cross3f(&vertical,
// &direction, &axis) 192, with node/segment/stiffness/inverse step/tick order 192; vertical declared before the
// axis 177. Body restored.
// Round 45 (lane 14, fast), on the retail-order angle call (177): the vertical declared before the loop 301; the
// vertical and the inverse step before the loop 190; the angle taken after the rest-offset copy with the vertical
// before the loop 301; the first node's direction copied before its position 188. The segment/scale temp slot
// sits above the axis in our frame and below it in retail's.
// Round 132 (m3 lane 5, fast): retail's rest offset sits below previous (+0x50/+0x5c; ours +0x5c/+0x50) and
// cosine below sine (+0x10/+0x14; ours sine +0x1c, cosine +0x20). Rest offset declared at function scope 179;
// cosine declared before sine 179; both 179; `rest_offset`, `last_position` and sine/cosine angle names 179
// each (all identical code: neither names nor declaration order place these homes).
// Round 37 (lane 15, fast): every slot is shared alike except two mirrored groups: retail runs length/cosine +0,
// segment/scale/sine +4, axis +8, rest offset +0x40, previous +0x4c; ours runs axis +0, segment +0xc, length
// +0x10, previous +0x40, offset +0x4c (the axis, length and normalization code is otherwise retail's). The axis
// is `vertical x direction`: the existing cross3f(&vertical, &direction, &axis) with the vertical declared just
// before it 192; with the existing normalize3d for the length 190 (1371 bytes); normalize3d alone 184 (first
// difference +0xc1, homes unchanged); cross3f_staged 192; cross3f with the vertical declared at the loop top 266.
// Diagnostic (initialised dead long temps at the top, k = 1..7): 179 179 181 181 181 179 179 (homes unchanged).
// Round 298: Normalized axis store orders ikj/jik/jki/kij/kji 179/179/179/178/178; complete scalar
// axis producers with jki/kij/kji normalization 192/190/192. One-point gains rejected; original
// axis producers and body restored. Segment/previous/axis homes and rotation schedule remain.
// Round 132 (m3 lane 5, fast): retail's rest offset sits below previous (+0x50/+0x5c; ours +0x5c/+0x50) and
// cosine below sine (+0x10/+0x14; ours sine +0x1c, cosine +0x20). Rest offset declared at function scope 179;
// cosine declared before sine 179; both 179; `rest_offset`, `last_position`, sine/cosine angle names 179
// each (all identical code: neither names nor declaration order place these homes).
// Round 49 (lane 10, fast): the angle call as function_11ce20(&vertical, &direction) (retail's ECX/EDX setup:
// direction in ECX, vertical in EDX) 177; on that: the rotation through a private inline Rodrigues helper on the
// offset copy (v*cos + projection*axis - cross*sine) with predicted = position + offset 236 (1370 bytes); the dot
// and cross operands written offset-first 177 (same code); the axis through the existing cross3f(&vertical,
// &direction, &axis) 192, with node/segment/stiffness/inverse step/tick order 192; vertical declared before the
// axis 177. Body restored.
// Round 74 (m3 lane 12, fast; 0x73360 kept its match in each): `previous = position` right after the i == 0
// if/else 200, before the rest-offset copy 194, before the velocity stores 192 (previous stays at +0x40 in all three:
// the mirrored homes do not follow where previous's live range ends); `previous = node->position` after the
// publication 179 (same code); `segment->stiffness * definition->stiffness` 179 (same code).
// Round 291: Angle before axis/after axis/after root, with original/early paired trig,
// 314/307/293/319/294/316; early angle with short selected ordinal 326/332. Original producers restored.
// Round 293: Initial-node position/direction/degenerate-axis fallback memmove subsets 1/2/4/3/5/6/7:
// 371/380/363/400/373/382/402; all with rest-offset memmove 409. Original copies and file restored;
// segment/previous/axis homes remain.
// Round 154 (lane 4, relink screen of the reduced image, exact at 179): target through vector3f_add_scaled(&previous, &delta,
// scale, &target) 173 (6 points, not kept; with j/k rows reordered, the lerp as target*(1-stiffness), direction through
// vector3d_from_points3d 173 each). Segment index as a named long, `segments + (...)`, const segment, count local, reversed test,
// operands of the stiffness product swapped 179 each; index with an if short/long 319/179; two-arm segment assignment 297 (1379);
// named segments base 174; ticks before stiffness 177; segment before node 279. Names of offset and previous, declaration order,
// offset at outer scope 179 (homes do not move). Rotation through a forceinline pointer helper with written cross terms 236 (1370,
// frame 0xa8, homes nearly retail's), in-place trig args plus dot3f squared 217 (frame 0xa4, offset below delta); dot3f either
// order, cross3f, either final add, normalize3d 271 (1390). normalize3d(&axis) in place of the written normalize 178; dot3f squared,
// `/= length` rows, vector3f_scale rows 173 (same code). Dead-mention diagnostics on axis and offset (scratch only) 172 at best.
// Left: segment takes ECX where retail takes EDX, which rotates ECX/EDX through the call setup; axis below the scalars and previous
// below offset in the frame; axis k kept in memory where retail loads it.
// Round 298: Native initial-input/working-node/publication range-copy families
// and pairs/all 393/251/362/403/387/349/363; all with fallback/rest-offset ranges 391.
// Original copies, short induction, body and includes restored;
// segment/axis/previous/rest-offset homes and rotation schedule remain.
// Round 300: Complete indexed rotation consumer with ordinary/staged existing cross,
// direct/complete axial phase and original/scaled-difference velocity:
// 268/268/266/266/268/268/265/265. Original body and include set restored; segment/axis/previous/offset homes remain.
// N316: Working attachment position/direction defaults, with scaled target and paired differences:
// 265/200/273/268/194/273/303/303. Original conditional producers, helpers and body restored;
// segment/axis/previous/rest-offset homes remain.
// M321: Existing returned current/target/recurrent-prediction point-setter receivers, masks 1/2/4/3/5/6/7 then immutable target
// 342/179/300/342/312/299/309/309. NOT kept: local output receivers require a match. Original body restored; segment/axis/previous homes remain.
// Y547: Native double length-ratio/axis-reciprocal/paired-trig residence, seven subsets
// then all with double returned angle: 328/236/348/317/430/372/440/494. Exact-only precision
// forms restored; segment/previous/axis homes and rotation scheduling remain.
// Round 38 (m3 lane 18, fast): axis renamed to sort last 179; length declared at the loop body top 179; length
// and segment renamed to sort first 179; `point3f predicted, previous;` 179 (all identical code); previous stored
// before the node position 179 (repeat of round 371); the retail-order angle call with the blend complement named
// for the three rows 177 (the two listed 177s do not add up).
// Round 201 (fast): retail's 12-byte rest offset sits below previous (0x50 against 0x5c) and its
// length spill below the segment spill, ours the reverse; local names do not move them: segment,
// axis, length (two names), offset and previous renamed 179 each. Rest offset as a const reference
// 337 (1334 bytes); previous copied from the node's stored position 179; the step reciprocal ahead of
// the loop 189; stiffness computed in the non-initial arm 316. Original body restored.
// Round 79 (m3 lane 16, fast): the segment as a ternary of two element addresses 297 (1379 bytes); previous copied
// before the node's position store 179; position/direction declared at the loop-body top 179; the velocity rows as
// `inverse_dt * (...)` 179; the last-segment index as `i - 1` 183. The first real difference is the segments base:
// retail loads it into EDX and keeps the segment in EDX (ours EDI, then ECX), which shifts the scratch rotation of
// the physics call's argument setup (retail forms the tag address after pushing -1 and the location).
// Round 35 (m3 lane 17, fast): `if (record->fixed || !(dt > 0.f)) return;` guard 179 (same code); position and
// direction declared beside previous/predicted 179; location declared before origin/forward 179; a const vertical
// 179; the const vertical with the angle call as (&vertical, &direction) 177 (two points, not kept).
// Round 143 (m3 lane 5): the target through the existing vector3f_add_scaled together with the
// angle call as function_11ce20(&vertical, &direction) (retail's ECX/EDX setup) 171 (kept, plain;
// each alone was 173/177). On 171 (fast): physics index in a local for test and lookup 179 (on 179,
// same code); direction through vector3d_from_points3d 171 (same code); delta through it 271 (1357);
// velocity through scaled_difference3f 193; blend as vector3f_scale then vector3f_add_scaled 240
// (1369); blend complement named and used three times 275 (1347); `previous = node->position =
// position` 179 (on 179, same code); segment indexed by i and stepped back for the last node 292.
// B413: Complete constraint/normalization/rotation temporary end scopes, seven subsets,
// then all with the two-use angle scoped to paired trig: 171/171/171/171/171/171/171/171.
// Original complete body restored; segment/previous/axis homes and rotation scheduling remain.
// Round 49 (m3 lane 2, fast): the physics tag through a private inline getter 171 (same code: retail's late
// tag-address formation follows its EDX/ECX allocation, not the argument's position); the rotation in place
// on the offset copy with named projection and cross scalars (0x11d180's form), then predicted = offset +
// position, 235 (1370 bytes, frame 0xa8); node as `record->nodes + i` 171; sine-first cross products 171;
// `(1.f - cosine) * dot` 171 (all same code); the inverse step computed just before the velocity rows 297.
// AC518: Complete normalization admission bool/byte/short/long, direct predicate
// and success-owned result, 292/292/292/292/292/292/292/292 (1355 bytes).
// NOT kept: decision-result lifetime does not match; original body restored.
// AC522: Plain axis scaling long/int/short/word/byte/signed-byte loops,
// native component cursor and long loop with complete shared cross:
// 187/187/212/212/212/212/212/198. NOT kept: regressions; original body and homes restored.
// B421: Root-node classification across metadata/tick producers, bool/byte/short/long,
// positive/complementary results 277/277/289/288/277/277/289/288. NOT kept: early
// cmp/sete differs from retail; original direct word test and complete body restored.
// P316: Complete normalization/trigonometric/constraint scalar declarations at loop
// entry, seven phase subsets, then all at function entry: 171 each. Every initializer
// stays at its original computation. No gain; complete body restored. Segment EDX
// and scalar/axis/previous/rest-offset homes remain.
// N355: Existing early-return normalizer with native square/projection/staged-cross and complete setters:
// 174/186/174/174/174/186/187/187. NOT kept: no exact match from the tested structural forms.
// Complete original executable source restored; Segment EDX and axis/previous/rest-offset homes remain.
// J253: Actual shared rotation providers on rest offset 196/236/234, on recurrent
// prediction 246/295/295; direct paired trig arguments 196/236. Explicit provider
// units resolve every call. All regress or move frame/length away; original body restored.
// F374: Optional-physics presence/absence at the original post-position test,
// bool/byte/short/long: 171/171/171/171/171/171/171/171. NOT kept: no native
// segment/home recovery; original field test and complete body restored.
// DH350: Output-owned velocity difference/scale, interpolation contribution
// and prediction translation phases, seven subsets then compound axis:
// 171/171/171/171/171/171/171/171. NOT kept: no native byte match; original complete body restored.
// DH354: Complete outer fixed/timestep admission result in bool/char/signed-char/
// byte/short/word/int/long: 171/171/171/171/171/171/171/171. NOT kept: no exact admission residence;
// direct guard/body restored; segment EDX and scalar/axis/previous/rest-offset homes remain.
// Round 63 (m3 lane 14, fast): axis rows as statements in the orders ikj/jik/jki/kij/kji 183/174/186/183/186 (ijk 171);
// length_sq3f for the axis length 171, dot3f for the projection, either argument order, 171/171, both 171, length_sq3f
// for the segment scale 171, all 171 (same code); the tail blocks (prediction P, velocity V, node position N, previous
// R) ordered PVRN 171, PRVN 184, VPNR 208, VPRN 208, VNPR 207, VNRP 207, VRPN 205, VRNP 207, RPVN 183, RVPN 205, RVNP 207.
// Slot use in the listing shows the homes follow the registers: retail reads the offset's k four times from memory where
// ours keeps it in xmm1, so offset (8 uses) ranks below previous (7) in retail and above it in ours; retail's axis has 20
// uses where ours has 23, which puts it above the length and segment slots. Retail also leaves the prediction's z store in
// xmm5 to the end of the block in every order tried above.
// AC540: Complete axial projection term staging, six producer orders, then direct/staged dot accumulators:
// 171/171/171/171/171/171/171/171. NOT kept: unmatched product residence; segment/axis/previous/rest-offset
// homes remain.
// Y594: Persistent segment cursor with preceding-segment terminal selection,
// for/while short/word/long/int node traversals: 252/252/317/317/252/252/317/317.
// NOT kept: cursor/domain forms enlarge the frame to 0xa8 and body to
// 1386/1380 bytes; native EDX/axis/previous homes remain. Original body restored.
// F381: Existing complete setters at recurrent previous/rest offset/current and initial
// position-direction boundaries, subsets 1/2/4/3/5/6/7, then node publication:
// 345/277/367/336/352/354/339/345. NOT kept: regressions and nonnative
// homes or extent; original complete copies/body restored, segment/axis homes remain.
// F388: Existing axis/rest-offset/recurrent-previous component-array views,
// subsets 1/2/4/3/5/6/7: 171 each; all with working direction/position/prediction 183.
// NOT kept: no gain; original body restored, segment and mirrored homes remain.
// P334: Native k-j-i distance/axis/projection reduction contracts, seven subsets
// then plain component reductions: 380/384/306/507/471/469/589/614. NOT kept: regressions and larger
// extents; original complete body restored; segment/axis/previous homes remain.
// M389: Double blend/time/projection coefficient receivers, seven subsets, then
// full graph with double axis root: 375/339/382/348/464/465/481/484.
// NOT kept: all regress and lose native frame or extent. Original real coefficients,
// axis root, arithmetic trees and complete body restored; segment and vector homes remain.
// N376: Separate raw/normalized rotation axes with complete scalar setters and staged-cross
// producers, eight forms 308/307/291/303/291/303/239/263. NOT kept: no exact match; original
// axis storage, body and homes restored. Segment EDX and mirrored vector/scalar homes remain.
// Round 130 (m3 lane 7, fast; 0x116750 and 0x73360 kept matching): the rotation through a local helper writing an
// output, then `+= position`, 165, with the cross terms named in 0x170d70's order 163 (aligned 132 -> 123; one-caller
// helper, match-only); position added inside the helper 171; the angle passed in 218/224; in place on the offset 235.
// Named cross terms in the body, four statement orders, 171 (same code). Axis through cross3f or cross3f_staged, crossed
// with normalize3d and the helper, 18 forms 163-196. `segment = definition->segments; segment += index` 281 (aligned 115):
// the segment then takes EDX and every later scratch register is retail's, but the load sits ahead of the ternary. With
// the index computed first (long, block helper, byte offset, reference, const, bound element) 171 (same code), short
// index 290 (1356); stiffness read first 270; stiffness through the element 279; segment before node 279.
// J261: Existing double-factor axis/target/velocity scale subsets 236/287/199/336/282/315/331; independent
// double angle 258. All regress or move frame/extent away.
// NOT kept: no eligible native repair; opening executable source restored. Segment EDX, mirrored
// scalar/axis/previous/rest-offset homes and rotation scheduling remain.
// Round 91 (m3 lane 9, fast): the segment chosen as a ternary of two element addresses 297 (1379 bytes), as an
// if/else 297; inverse_dt declared once before the loop 181; the node pointer taken after the segment 279;
// inverse_dt after the node and segment 171 (same code); `++node->ticks` before the stiffness 175. Retail builds the
// segment address in EDX (the node offset's register) and keeps it at +0x14, ours borrows EDI and keeps it at +0x1c.
// Round 66 (m3 lane 14, fast): the rotation in 0x146320's statement shape (dot k/j/i, named cross j and k, cross i inline in
// row i, offset-first products) 171; with cross i named too 171; in 0x170d70's order (cross i, projection, cross j, k) 171;
// the cosine complement named 171 (all same code); `segment = &segments[i]` then reassigned for the last node 284 (1358 bytes);
// the fixed/time-step guard as two nested ifs 171; the node index declared ahead of its loop 171 (same code).
// P394: Actual terminal-segment decision before reciprocal/node acquisition, terminal/interior
// bool/byte/short/long results: 171/171/171/171/171/171/171/171. NOT kept: no byte equality; original
// direct selection and complete body restored. Segment EDX and mirrored scalar/vector homes remain.
// M3 lane 16 r97 (fast, 0x116750 matched each time; body restored). Retail's homes from low to high: length +0x10,
// segment (later the sine) +0x14, axis +0x18, rest offset +0x50, previous +0x5c; ours: axis +0x10, segment +0x1c,
// length +0x20, previous +0x50, rest offset +0x5c. Declaration placement does not move them: `predicted, previous`
// order 171; the rest offset declared bare at the loop top 171; length and axis bare at the loop top 171; length,
// axis and offset bare ahead of previous in the outer block 171; offset, previous/predicted, axis, length in the
// outer block 171 (all five the same code).
// M3 lane 11 r81 (fast): delta, axis or vertical as three statements instead of an aggregate initializer 171 each, delta and axis
// together 171, the node as `record->nodes + i` 171 (all same code); diagnostic: target rows written without vector3f_add_scaled
// (previous no longer address-taken) 177 with the homes unchanged.
// m3 lane 3 r227: the first real difference is still the segments base in EDI and the segment in ECX where
// retail uses EDX for both (`mov edx, [ebx + 0x9c]; ... add eax, edx; mov edx, eax`), after the ternary index. Every natural
// spelling of the index, base and stiffness read is listed above, and the in-place rotation through the existing rotate_direction
// (src/unknown_1eb8a0.cpp) or 0x170d70's helper is J253's 196-236; round 130's output-writing helper (163) would be a one-caller
// helper: match-only.
// New segment spellings (fast, 0x116750 matched): the segments base hoisted above the loop 301 (1370 bytes); `&segments[i]` then
// `segment--` for the last node 292 (1357); the base, `segment += i`, then `segment--` 292; the index as `i < count ? i : count - 1`
// 172; as `i < count - 1 ? i : count - 1` 179. None gives the EDX base after the index.
// Lane 3 r230 (plain 171, 1368 bytes, 0x73360 unchanged at 33): the existing helpers in place of their bodies written
// out, each byte-identical here: normalize3d for the axis (no length local), length_sq3f for the delta's magnitude, dot3f
// for the projection, vector3d_from_points3d for the direction (171 each and together). Not kept: with all four (and the
// normalize3d.h include) the full check lost matched 0x54590 (network_voice.cpp, then 2 at +0xff), as round 62 lost
// 0x73360: this file links ahead of both. The delta through vector3d_from_points3d as well 272 (1357 bytes; the
// listed both-differences form). New: `previous = node->position = position;` 171, the z row as `along * axis.k` like
// the others 171, `node->ticks++` 171 (all same score); re-measured listed forms: delta as three statements 171,
// location declared first 171.
// Round 104 (m3 lane 9, fast): names do not order these homes: `offset` renamed `rest_offset` 171, with `segment`
// renamed `current_segment` too 171, `length` renamed `axis_length` 171 (same code; the compile-only harness's frame
// offsets do not move either); the normalization with `*=` in all three rows 171; `fabs(length) < .0001f` 171. Lead:
// 0x170d70's file-private rotate_vector_170d70 has this rotation's shape (round 130 measured it as a local helper,
// 163); using that existing helper needs it in a shared header, and its file is one of the held files this round.
// M3 lane 10 r89 (fast, 0x116750/0x73360 still matched; fast agrees with plain at 171): the segments base in a
// named pointer local re-measured 172 (round 154's 174); cosine computed before sine re-measured 172 (round 132's
// 179); the existing normalize3d for the axis re-measured 178
// (0x73360 kept its match on the fast image); dot3f for the projection re-measured 171 (same code); the x row's
// first product as `axis.i * along` 171 (same code); the cosine product first in each rotation row 171 (same code);
// the blend rows as `predicted.x * stiffness + (1.f - stiffness) * target.x` 171 (same code); each cross term
// negated and added (`+ (axis.j * offset.k - axis.k * offset.j) * sine`) 209; the length test's arms inverted
// (`if (!(.0001f > fabs(length)))` scales, else zero) 171; a 600 s permuter run (66 tries) none below 171. Retail's
// rotation evaluates the written expressions but schedules the y and z cross terms right after the dot, before
// the x row, loading all six axis/offset components into registers first (and reading offset k from memory twice
// more later); ours reads its offset k from memory at the dot and at two later products.
// Lane 4 r186 (fast with the file and 0x73360/0x54590; control 171 [136/79]): kept, byte-identical: normalize3d for the axis, length_sq3f
// for the delta's magnitude, dot3f for the projection and vector3d_from_points3d for the direction in place of their bodies written out
// (round 230's 171 form; it was dropped then for losing matched 0x54590, which is near at 2 on main now; plain: 0x54590 2 and 0x73360 33
// byte-identical). On it: `++node->ticks` at the loop's end 270; the first node's copies direction first 182 (first difference +0x17);
// the bound as `i <= definition->segment_count` 305 (1360 B); the node velocity through vector3d_from_points3d and three `*= inverse_dt`
// 208 (1392 B); the y and z cross terms named ahead of the rows 171 (match-only, six lines move, homes unchanged). Same code:
// dot3f(&offset, &axis); the scale as `length / (real)sqrt(...)`; `previous = node->position`; position added first in the predicted rows.
// Round 188 (m3 lane 4, fast with 0x116750/0x73360/0x54590): the axis through the existing cross3f(&vertical, &direction,
// &axis) re-measured on this body 179 (masked 79 -> 76); the node velocity through vector3d_from_points3d and vector3f_scale
// 207 (1391 B); sinf/cosf re-measured 174; same code (171): a named versine, `inverse_dt * (...)` velocity rows, `if
// (!normalize3d(&axis))`, the guard as `0.f < dt`, `previous` stored before `node->position`. Code-changing: the rest-offset
// copy before the axis 186; the angle call (with the vertical) before the axis 314 (1353 B); a long loop index 290 (1358 B);
// the stiffness product declared in the branch that uses it 315 (1348 B; retail forms it at the loop top). Retail's homes
// run length, segment, axis upward; ours exactly reversed.
// m3 lane 15 r72 (fast with 0x116750/0x73360, both matched throughout): round 130's same-file rotation helper (0x170d70's
// statement shape, writing an output, then `+= position`) re-measured 163 (match-only: a one-caller helper). On it: the named
// terms in three orders, the dot axis-first, the rows projection-first, `(1.f - cosine) * dot`, sine-first products, the result
// into a `rotated` local then added to position (either order), and the axis normalization rows all inverse-first, all
// axis-first or all `*=`: 163 each (same code); the axis passed by value 334 (1399 bytes). 0x170d70's own helper is in a file
// held for the sort_4byte change this round.
// m3 lane 10 r99 (fast with 0x116750 matched; register-masked aligned count 112 for this body): the axis length named
// (`real length = normalize3d(&axis); if (length == 0.f)`) 178 [160], first difference +0xc1; the physics index in a named
// local for its test and the tag lookup 171; the target point declared with the loop's other locals 171; the direction
// declared before the position 171; the delta's squared length through dot3f(&delta, &delta) 171 (all four the same code).
// M3 lane 13 r89 (fast with 0x116750/0x73360/0x54590, all matched; control 171): `previous = position;` ahead of the velocity
// rows 184; the guard as `dt > 0.f && !record->fixed` 176; the rotation into a `rotated` vector, then `predicted = rotated +
// position` per row, 165 (aligned 187 -> 156: the rest offset's home moves to retail's +0x50; the same count as Round 130's
// helper 165; one-use named parts, so match-only, and under 8 points). On that: the cross terms through cross3f(&offset,
// &axis, &cross) 270 (1390 bytes); the rows reading the rest offset through the segment 342 (1329 bytes); same code (165):
// vector3f_add for `predicted`, an aggregate initializer, `position + rotated`, `previous = position` first, the cosine
// products first, the three cross terms named after the dot or in 0x170d70's order, the rotation written into `predicted`
// and then `predicted.c += position.c` (no named local), the sine as each cross term's first factor; the segment length read
// before the physics call 289 (1381 bytes); `++node->ticks` after the first-node/physics block 252; the target computed in
// place in `delta` 190 (1360 bytes); the vertical declared at the loop's top 165 (same code). Left: segment EDX, mirrored homes.
// MiniMax A (11-13): no new form, no lower score (171): the first real difference is still the segments base in edi and the segment in ecx where retail uses edx for both, and retail's homes run length, segment, axis upward where ours are reversed; the rotation's cross terms and their loads follow from those homes.
// M3 lane 16 r116 (fast with 0x116750, matched throughout): `inverse_dt` computed once ahead of the loop 181; the vertical
// declared ahead of the loop 304; both 184; the axis built by three component stores (no initializer) 171, declared at
// the loop's top 171, declared ahead of the loop 171 (all three the same code). In the loop the low homes are {axis
// 0x10-0x18, segment/scale 0x1c, cosine 0x20} where retail has {cosine 0x10, segment/scale 0x14, axis 0x18-0x20}: the
// 12-byte axis home and the two scalar slots trade ends whatever the axis declaration's place.
// P475: Standard target/velocity/interpolation coordinate contracts, seven subsets,
// then complete graph with forward seeded distance reduction: 331/309/425/333/411/420/395/437.
// NOT kept: no exact native segment/vector-home repair; original complete body restored.
// MiniMax H (11-14): no new form, no lower score (171): the segment pointer is still in EDI where retail keeps it in EDX, and retail still loads all six axis and offset components into registers where this build reads offset k from memory at the dot and at two later products.
// @retail 0x1161f0
void __stdcall function_1161f0(s_chain_record *record, s_chain_definition *definition, real dt)
{
	point3f origin;
	vector3f forward;
	s_location location;
	function_116750(record, definition, &location, &origin, &forward);
	if (!record->fixed && dt > 0.f)
	{
		point3f previous, predicted;
		for (short i = 0; i < definition->segment_count + 1; ++i)
		{
			real inverse_dt = 1.f / dt;
			s_chain_node *node = &record->nodes[i];
			s_chain_segment *segment = &definition->segments[i == definition->segment_count ? definition->segment_count - 1 : i];
			real stiffness = definition->stiffness * segment->stiffness;
			++node->ticks;
			point3f position;
			vector3f direction;
			if (i == 0)
			{
				position = origin;
				direction = forward;
			}
			else
			{
				position = node->position;
				if (definition->physics_index != NONE)
					function_211060(&node->velocity, 0, g_4e3b44[definition->physics_index & 0xffff].bytes, &location, NONE, &position, 0, 0, 0, .02f, dt);
				vector3f delta = { position.x - previous.x, position.y - previous.y, position.z - previous.z };
				real scale = (real)(segment->length / sqrt(length_sq3f(&delta)));
				point3f target;
				vector3f_add_scaled((vector3f *)&previous, &delta, scale, (vector3f *)&target);
				position.x = (1.f - stiffness) * target.x + predicted.x * stiffness;
				position.y = (1.f - stiffness) * target.y + predicted.y * stiffness;
				position.z = (1.f - stiffness) * target.z + predicted.z * stiffness;
				vector3d_from_points3d(&previous, &position, &direction);
			}
			vector3f axis = { direction.k * 0.f - direction.j, direction.i - direction.k * 0.f, direction.j * 0.f - direction.i * 0.f };
			if (normalize3d(&axis) == 0.f)
				axis = *g_4687ac;
			vector3f offset = segment->rest_offset;
			vector3f vertical = { 0.f, 0.f, 1.f };
			real angle = function_11ce20(&vertical, &direction);
			real sine = (real)sin(angle);
			real cosine = (real)cos(angle);
			real along = dot3f(&axis, &offset) * (1.f - cosine);
			predicted.x = (along * axis.i + cosine * offset.i) - (axis.k * offset.j - axis.j * offset.k) * sine + position.x;
			predicted.y = (along * axis.j + cosine * offset.j) - (axis.i * offset.k - axis.k * offset.i) * sine + position.y;
			predicted.z = (axis.k * along + cosine * offset.k) - (axis.j * offset.i - axis.i * offset.j) * sine + position.z;
			node->velocity.i = (position.x - node->position.x) * inverse_dt;
			node->velocity.j = (position.y - node->position.y) * inverse_dt;
			node->velocity.k = (position.z - node->position.z) * inverse_dt;
			node->position = position;
			previous = position;
		}
	}
}

// @retail 0x116750
void function_116750(s_chain_record *record, s_chain_definition *definition, s_location *location, point3f *origin, vector3f *direction)
{
	s_object_marker marker;
	function_b8d30(record->object_index, definition->marker_name, &marker, 1, false);
	*origin = marker.matrix.position;
	*direction = marker.matrix.forward;
	function_11bed0(location, &marker.matrix.position);
	vector3f delta = { origin->x - record->origin.x, origin->y - record->origin.y, origin->z - record->origin.z };
	if (fabs(delta.i) > 1.f || fabs(delta.j) > 1.f || fabs(delta.k) > 1.f)
	{
		for (short i = 0; i < definition->segment_count + 1; ++i)
		{
			s_chain_node *node = &record->nodes[i];
			node->position.x += delta.i;
			node->position.y += delta.j;
			node->position.z += delta.k;
		}
	}
	record->origin = *origin;
}

// Nine populated slots and three null slots in the retail callback table.
void *g_4674a0[12] =
{
	(void *)function_115d10, (void *)function_115d50,
	(void *)function_115d70, (void *)function_115d80,
	(void *)function_115da0, (void *)function_115ff0,
	(void *)function_1160c0, (void *)function_116010,
	0, 0, 0, (void *)function_116080
};
