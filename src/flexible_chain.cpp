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
#include "axis_rotation.h"
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

// score 95 (was 144): The 0xa4 frame and attachment agree. The selected segment still uses EDI/ECX and +0x20 against EDX and +0x14, and the rotation homes differ.
// MiniMax H (11-20): no new form, no lower score (144): retail keeps the selected segment pointer in EDX and spills it at esp+0x14 while ours keeps it in EDI and ECX and spills it at esp+0x20, and that one choice drives the rest: retail's copy of the node position uses ECX as its base and stores z before y, ours uses EDX and stores x, y, z in order, and the 0x211060 set-up takes the mover-settings index in EAX in ours and ECX in retail; the call's ten stack arguments and the EAX first argument are the same on both sides and the body is 1371 bytes against retail 1368.
// earlier 171 (was 171): previous/axis homes, segment ownership and rotation scheduling differ;
// Frame 0xa4 and attachment setup agree. The first exit span differs, followed by the segment address in EDI and ECX instead of EDX and different rotation homes.
// Earlier score 171 (was 171): previous/axis homes, segment ownership and rotation scheduling differ;
// seven local-name sets, declaration reversals and entry/loop scopes 194;
// shared cross 207, both point differences 286, both helpers 280;
// scaled difference 212, all vector helpers 285.
// MiniMax R (11-16): no new form, no lower score (171): re-measured in this round's nine-address set, the body is retail's 1368 bytes and the frame 0xa4 with the attachment homes agreeing; what is left is the carrier rotation (the segment's axis load keeps the value in EDI where retail keeps it in EDX and the segment pointer follows in ECX where retail keeps EDX), the scalar spill at +0x1c against retail's +0x14, and the previous/rest-offset and axis/scalar homes exchanging at +0x28 and +0x2c.
// MiniMax S (11-14): no new form, no lower score (171): the body is still retail's length and the first difference is the register pair at the segment's axis load, retail keeping the value in EDX where this build keeps it in EDI and then in ECX, so only the branch targets and the block layout after it differ; the segment's base, its ownership and the frame's homes are the rest of the difference.
// MiniMax E (11-16): no new form, no lower score (171): the segment base and the segment pointer take EDI/ECX where retail uses EDX for both, and the low stack slots are in another order.
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
// m3 lane 14 r93 (fast with 0x116750/0x73360/0x54590, 0x116750 matched throughout; control 171): the rest-offset copy after the
// sine and cosine 223 (1370 B); the target as a copy of previous scaled in place by vector3f_add_scaled 359 (1399 B); the node
// position read at the declaration, the first node overwriting it 187; the last-segment test as `i >=` 172 (one line, `jl` for
// retail's `jne`). Same code (not counted): `(1.f - cosine) * dot3f(...)`. Diagnostic (image search): a 5-file reduced image
// (this file and its callees' files) gives retail's EDX segment base, and adding src/unknown_1689b0.cpp alone (even with every
// body but 0x1697c0's removed) flips it back to EDI; but in that small image 0x211060 takes its mover settings in EDI instead of
// on the stack, and with 0x211060's own body out of the image the base is EDI again, so the EDX base there comes from a
// different 0x211060 convention, not from anything in this body. Not a lead.
// m3 lane 12 r125 (fast with 0x116750/0x73360/0x54590, control 171): the low slots are retail's in exact reverse and
// the per-slot reference counts fit heaviest-lowest in both builds (ours reads the axis from memory more, retail the
// cosine), so they follow the rotation's allocation. 0x211060 takes EAX plus the stack here as in retail (full image).
// The rest-offset copy at the loop top 289 (1354 bytes), after the angle call 207; position and direction declared at
// the loop top 171, location declared before origin and forward 171, `previous, predicted` at function scope 171 (all
// three the same code).
// M3 lane 3 r243 (fast with 0x116750, matched throughout; base 171): on lane 13 r89's match-only rotation-
// into-`rotated` form (re-measured 165): `inverse_dt` computed once ahead of the loop 175; the guard as `dt > 0.f &&
// !record->fixed` 170 (first difference +0x30); the vertical declared ahead of the loop 301; `++node->ticks` ahead of
// the stiffness 169; `++node->ticks` just before the position store 266; `previous = position` ahead of the velocity
// rows 165 (same code). On the body itself: the vertical declared `const` 171 (same code). The segment base still
// takes EDI where retail takes EDX. Body unchanged.
// M3 lane 13 r95 (fast with 0x116750/0x73360/0x54590, 0x116750 matched throughout; control 171): `++node->ticks` ahead
// of the segment selection 274 (first difference +0x51); the vertical declared ahead of the rest-offset copy 171 (same
// code, not counted). On lane 13 r89's match-only rotation into `rotated` (re-measured 165), the three rows in the order
// y, z, x 158 (1368 bytes; the rest offset takes retail's +0x50 home; one-use named parts and a row order picked by
// score, so match-only: build/r95/lead-1161f0-158.patch in the m3-lane13 worktree); the other orders z, y, x 179,
// y, x, z 179, x, z, y 179, z, x, y 158 (other code). On the y, z, x form: the three cross terms named ahead of the
// rows in that order 158 (other code); the rest-offset copy after the angle call 181 (1370 bytes); cosine computed
// ahead of sine 159; same code as it: `predicted` through vector3f_add, the projection named, `rotated` declared beside
// the vertical. Body unchanged.
// M3 lane 7 r165 (fast with 0x116750/0x73360/0x54590, 0x116750 matched and 0x54590 2 throughout; control 171): the segment
// index in a named long local 171 (same code); the segment chosen by if/else (last segment, else segment i) 297 (1379 B);
// the segment selected ahead of the node pointer 279; `++node->ticks` ahead of the stiffness 175; the segment as
// `segments + index` 171 (same code). Body unchanged.
// M3 lane 8 r172 (fast with the file's ten matched functions, all kept): as a __stdcall member of the record (`this` = the
// record; its callers are contributor code, so only a match could be held) 173 (two reloads swap); the rotation through a
// forceinline helper taking the angle, sine and cosine its locals, predicted through a pointer 224; the else arm's delta,
// scale and target declared at the loop body's top 171 (same code); the whole prediction (axis, normalize, offset copy,
// vertical, angle, sine/cosine, predicted) as one forceinline helper 171 (same code: inlining moves no home); the segment
// as a pointer to const 171 (same code). Body unchanged.
// MiniMax R (11-16): no new form, no lower score (171): the segments base and the segment pointer sit in edi and ecx
// where retail uses edx for both, and the low slots come out in the other order (here the axis at 0x10-0x18 against
// retail's cosine at 0x10), which moves the whole rotation block.
// MiniMax I (11-15): the rotation written into a named vector and the position added to it afterwards scores 165 (was
// 171, 1368 bytes): the three rotation rows keep their expressions and the position addition becomes its own statement.
// The dot stays the shared helper; written out in retail's k, j, i order it gives the same 165. The segment index as a
// named short 288 (1359 bytes), the segment as the base plus the index 165 (the same code) and a permuter run (300 s,
// two tries) give nothing lower. What still differs: the segments base is loaded into edi and the segment pointer kept
// in ecx where retail uses edx for both, the segment's home is +0x1c where retail has +0x14, and the low slots are
// reversed (here the axis at 0x10-0x18, the segment and the scale at 0x1c and the cosine at 0x20; retail the cosine at
// 0x10, the segment and the scale at 0x14 and the axis at 0x18-0x20), which moves the whole rotation block.
// MiniMax I (11-15), NOT kept: 165 (was 171), a 6-point gain, under the 8-point bar for a function above 30.
// P567: Standard bounded-segment minimum, long/int, both argument boundaries,
// implicit/explicit ordering provider: 280/280/309/309/280/280/309/309. NOT kept: no
// native EDX/segment/axis/previous homes or exact match; original complete body restored.
// b9A (11-17) walk against retail, fast build on r839: statement by statement, no behaviour difference found. Checked:
// the 0x116750 call (record EDI, forward EAX, three pushed: 0x11621b); the fixed/dt guard (0x116225
// jne, 0x11623a jbe: NaN dt skips, as `dt > 0.f`); bound count + 1 (0x116248, 0x11671e); 1/dt and the {0,0,1} vertical
// hoisted (0x116263, 0x116280); node stride 0x20 from +0x1c, segment stride 0x80, stiffness = definition +0x64 times
// segment +0 (0x1162b6-0x1162c2); last-node pick (0x1162a5-0x1162ad); the 0x211060 arguments (0x116350-0x11638b); delta
// sum k, j, i and length / sqrt rounded to float (0x1163c8-0x1163f9); blend rows (0x116434-0x116479); axis (k*0 - j,
// i - k*0, j*0 - i*0: 0x1164bc-0x1164eb) with the 1e-4 / ==0 fallback to g_4687ac (0, 1, 0) (0x11651b ja, 0x11652f
// ucomiss, 0x11655d jp: NaN keeps the scaled axis); rest-offset copy (segment +0x74, 0x116578); sine before cosine
// (0x1165af, 0x116600); dot k, j, i, each rotation row's grouping and trailing + position (0x1165c9-0x1166ca); velocity
// rows times 1/dt, node and previous stores (0x1166ce-0x116731). Callers 0x116010, 0x1160c0 and callee 0x116750 stay
// MATCH. Only allocation differs: segment base and pointer in EDX (0x1162b0-0x1162c0) and the low slot order.
// Round 189 (m3 lane 6, fast with the file, 0x73360 and 0x54590; control 171, masked 56): the rows through bound
// references (`real &x = predicted.x`, built as the cosine part, minus the cross term, plus position) 227 (1374 B);
// `if (0 < definition->segment_count + 1)` around the node loop 180; the velocity rows through `vector3f &velocity =
// node->velocity` 178; the definition's stiffness read once ahead of the loop 301 (1374 B); the vertical set by
// component stores (`vertical.i = vertical.j = 0.f; vertical.k = 1.f;`) 173. Same code (not counted): the prediction
// in its own `{}` block; the segment through a forceinline accessor. Diagnostic dead volatile loads (reverted at
// once) before the node, after the node, after the segment and before the velocity rows: none moves the segments
// base out of EDI, so that base is not a scratch-rotation step.
// P595: The selected segment still needs EDX and the scalar/axis and previous/offset homes. for with
// refreshed count, original selector/branch index/terminal decrement/index plus base 171/171/175/171,
// while with refreshed count, same four selectors 171/171/175/171. NOT kept: the refresh and selection
// forms leave the segment register and stack allocation unresolved.
// m3 lane 10 r115 (fast with 0x116750, 0x116010 and 0x1160c0, matched throughout; control 171; body unchanged). Reading
// from retail's operand orders: VC 7.1 puts the younger of two fields first in each float product, and here retail has
// every axis component ahead of every offset component, the reciprocal ahead of axis i but behind axis j and k, the
// projection ahead of axis i but behind axis k, and the cosine ahead of the offset. That fits an axis reached only
// through helper addresses (never named component by component in this function), an offset named directly in this
// function, and a projection and cosine that live inside an inlined helper: the axis built by cross3f(&vertical,
// &direction, &axis), normalize3d, and the rest offset copy rotated in place by a helper of 0x170d70's shape (v, axis,
// angle; sine and cosine taken inside), then `predicted.x = offset.i + position.x` rows here. LEAD, not kept (a new
// helper with one caller; the same-shaped existing helper is file-private in src/unknown_170d70.cpp, held this round):
// that form with the helper's statements projection, cross_k, cross_j and cross_i written in row i 113 (1369 B; every
// stack home then agrees with retail, the normalization block agrees, and the rotation starts as retail does: dot,
// `axis.j * offset.i` in place, sine and cosine, projection; left are the segment base in EDI/ECX for retail's EDX with
// the ECX/EDX swap it carries through the first node's copies and the 0x211060 set-up, retail finishing cross_j ahead
// of cross_k, and its component load order axis k, offset j, offset k, axis j, axis i with offset i loaded late). The
// patch is build/r115/lead-1161f0-113.patch in the m3-lane10 worktree. Other statement orders of that helper (all with
// cross_j and cross_k named, which an in-place rotation needs): cross_k, projection, cross_j 122 (1369 B); projection,
// cross_j, cross_k (0x170d70's order) 144 (1371 B, with cross_i named first the same code); cross_j or cross_k ahead
// of the projection with the other after it, or both ahead, 147 (1371 B). Sine and cosine as the call's arguments the
// same code as taking them inside; as locals of this function ahead of the call 169 (1373 B: the cosine is then older
// than the offset and `offset * cosine` comes out). New components held in three locals before the stores 213. A
// helper that takes the three offset components as reals and writes position + rotation through a pointer 202 (1381
// B; its expression trees are retail's, its schedule is not). Not usable: cross_j or cross_k written in their rows of
// an in-place helper read the already rotated components (126 at 1368 B, wrong arithmetic). On the lead: the segment
// as `segment = definition->segments; segment += index` 274 (1368 B, retail's length; aligned 109 against the lead's
// positional 113: the base takes EDX but loads ahead of the ternary); the index named first then base and `+=` the
// same code as the lead; `position.x + offset.i` rows and the vertical declared after the axis the same code. On main:
// the index named, then `segment = definition->segments; segment += index` 171 (same code as the body). A compile-only
// screen of this file reproduces the rotation block of the fast build (but for the angle's reload), and 1,200 helper
// forms on it (statement sets and orders, sine/cosine order, a named dot, dot3f either way, row groupings) never give
// retail's schedule, so the helper's shape is not the whole cause. Also on the lead: the cross terms through
// cross3f(v, axis, &cross) inside the helper 143 (1368 B); the cosine taken ahead of the sine 121 (1369 B); the helper
// writing a separate `rotated` vector that the rows add to position 164 (1368 B); the cosine complement named, or the
// dot named ahead of the sine and cosine, the same code as the lead.
// M3 lane 8 r181 (fast; base 171): the projection as `(1.f - cosine) * dot3f(&axis, &offset)` 171; the cross terms
// as `sine * (...)` in all three rows 171; the dot named ahead of the complement (`projection * (1.f - cosine)`) 171;
// a `vector3f const` vertical 171; a `vector3f const` rest offset 171 (all the same code). The segments base still
// takes EDI and the segment ECX where retail uses EDX for both.
// Round 563. The selected segment still uses EDI/ECX and +0x1c against EDX and +0x14. point setter for the
// anchor position 356; vector setter for the anchor direction 306; point setter for the interpolated position
// 171; point setter for the predicted position 171; vector setter for the node velocity 175; point setters for
// the committed and previous positions 359; vector setter for the angle reference 171; point and vector
// setters for both attachment outputs 380. NOT kept: the anchor-position and commit setters shorten the body
// and demote the update caller.
// terminal decrement of default ordinal 175; replace default ordinal at terminal node 171; terminal
// ordinal as default then interior 275; velocity member throughout node step 256; position member
// throughout node step 171; position and velocity members throughout step 256; early rejection before
// node traversal 171; early rejection with decremented terminal ordinal 175. NOT kept: the original
// ordinal and member access score as well as or better than all trials. The segment spill and rotation
// schedule remain.
// Round 415. target-owned blend 329, shared in-place target scale/add 323, indexed target-owned blend 332,
// weighted correction point 234, shared unit-scale displacement 278, scalar target with in-place blend 329,
// indexed velocity displacement 193, target blend with shared velocity difference 332.
// m3 lane 13 r109 (fast with 0x116750, 0x116010 and 0x1160c0, all MATCH; plain confirmed): kept 171 -> 144 (1371 B): the
// rotation of the rest offset through the shared axis rotation of include/axis_rotation.h (the inline 0x2551f0 and 0x117dd0
// use, given sin and cos of the angle) with `predicted = offset + position` per row, and the axis as cross3f(&vertical,
// &direction, &axis) (the vertical declared ahead of it). Every call now sits at retail's instruction index and the axis
// block is retail's but for its home (+0x14 for retail's +0x18); left: the segment base in EDI/ECX for retail's EDX, the
// 4-byte cell shared by the segment, the scale and the sine at +0x20 above the axis (retail +0x14 below it), and the
// rotation's schedule (retail loads axis k, offset j, offset k, axis j, axis i and finishes cross_j and cross_k before the
// rows). Measured: the shared rotation alone, axis written out as before, 216; sine and cosine named ahead of the call 169
// (1373 B); the rows written `position + offset` 144 (same code); the angle call and the rotation in their own `{}` block
// 144 (same code); the scale written into the vector3f_add_scaled call 144 (same code); the rest-offset copy after the angle
// call 180 (1375 B).
// Conditional segment pointers from one base 291; ordinal minus terminal test 285;
// terminal pointer relative to block end 304; segment/axis/offset names 144;
// recurring point and rotation names 144; shared in-place displacement add 214;
// conditional default axis pointer 304; default axis pointer assigned after normalization 294.
// The segment still uses EDI/ECX and +0x20 against EDX and +0x14.
// The widened addressing ordinal leaves the segment spill at +0x20 and the low rotation slots differ.
// Shared node ordinal with direct count, captured count, terminal decrement, captured count plus pointer advance, and decrement plus advance 144/146/147/146/147.
// Using the widened ordinal at the anchor test as well, with direct count, captured count plus advance, and decrement plus advance 283/283/170.
// The segment EDX carrier and low scalar home still differ. Base-node identity 267, interior-node identity 275,
// terminal-node identity 306, both node identities 305. Position-derived ordinal for root 300, terminal test 307,
// full segment selection 292, and root plus segment selection 295. NOT kept: the address-based tests regress and
// change the loop comparison.
// M3 lane 15 r101 (fast with 0x116750, matched throughout; base 144): the node as `record->nodes + i` 144 (same code); the
// last segment chosen as `i - 1` 145 (1369 B); the stiffness product written segment first 144; `++node->ticks` ahead of
// the stiffness 148; `predicted` through vector3f_add(&position, &offset, &predicted) 204 (retail's 1368 B, but aligned 151
// against the body's 109). Diagnostic (not counted): 1-8 dead initialised locals at the top 144/144/146/146/146/144/144/144,
// so the homes and the rotation are no symbol-count tie on this body either. Body unchanged.
// The segment address still uses EDI/ECX and spills at +0x20 rather than EDX and +0x14.
// Direct displacement reused as the scaled-add target 162, scaled then added in place 238,
// scaled into a separate vector before addition 238. Shared point difference followed by
// those three target forms 273/270/223. Native vector target 144, with direction used for
// the unclamped displacement 298. NOT kept: the separate vector target ties, and forms
// that shorten the target workspace change the frame from retail's 0xa4.
// The segment still needs EDX and +0x14. Conditional attachment point by value/address/interior first
// 304/304/304, segment reference with named ordinal 144, terminal-predicate subtraction 285,
// conditional segment addresses against the interior bound 304, preceding-ordinal loop bound 294,
// shared prediction add 204, predicted z after the node update 157.
// NOT kept: the retail-length forms still raise the score and leave the segment and rotation homes different.
// M3 lane 3 r265 (fast with 0x116750, matched throughout; base 144): the segments base named at the loop top 145; the
// segment bound as `s_chain_segment const &` 144; vertical, axis, the rotation and the predicted rows in their own `{}`
// block 144; the segment count named at the loop top for the selection 144; the stiffness product written segment
// first 144 (all four the same code). Retail's ECX/EDX roles run one step behind ours from the segments base on (base
// EDX for our EDI, segment EDX for our ECX, the loop end's definition reload EDX for our ECX). Body unchanged.
// Separate physics point 314, with shared difference 311, reused for the constraint 304, with direction
// workspace 272, or with complete corrected point 409; in-place length quotient 144, compound stiffness
// product 144, and default attachment direction with separate physics point 312.
// NOT kept: the separate physics point expands the frame or worsens the register schedule at retail length.
// The scalar forms preserve the EDI/ECX segment and the displaced shared spill.
// The segment still needs EDX and the +0x14 spill. Separate array and signed ordinal,
// used directly for stiffness, length and rest offset. Ordinal first, terminal count,
// terminal ordinal, interior first and terminal decrement 272/269/272/269.
// Array first, same selectors 275/273/275/273. NOT kept: direct indexing
// moves the stiffness read ahead of the address addition and raises the score.
// Separate physics point with fields, point setter or shared point difference 314/361/311.
// Updating the physics point before publication, same forms 391/391/406. Early tag index
// with separate or updated point 314/391. NOT kept: every split widens the frame and leaves the segment spill different.
// M3 lane 4 r221 (fast with the file's ten matched functions): the axis renamed to sort after the segment and scale
// (`turn_axis`) 144; the scale renamed `ratio` 144, `along` 144; the segment renamed `used_segment` 144; the axis declared
// ahead of the vertical 144 (all five the same code: the segment/scale/sine cell stays at +0x20 above the axis, so the
// cell's place follows neither names nor declaration order). A compile-only build lays this frame out differently from
// the link-time build, so it cannot screen the homes.
// M3 lane 10 r126 (fast with the file's ten other functions, all MATCH; control 144, aligned 159): round 115's lead
// re-measured on this base, the rest offset rotated by a file-local forceinline helper taking the angle (sine and cosine
// inside; projection, cross_k, cross_j, cross_i written in row i) 113 (1369 B, aligned 138; a one-caller helper, match
// only). On that lead: the 0x211060 call through a file-local wrapper doing the tag lookup inside, or taking the looked-up
// settings, 113 each (same code); the segment selected before the node pointer 286 (aligned 126: the segment and its base
// then take retail's EDX and the first node's copies ECX, but i and the count swap EAX/ECX and the loop top reorders);
// `i < count ? i : count - 1` 114; ticks before the stiffness 117; inverse_dt above the loop 123; ticks before the segment
// 293; the index as a local set by an if 113; `!i` 113; a segments-base local 114; a ternary of two addresses 303; node
// before inverse_dt 113; `nodes + i` 113; `i != count ? i : count - 1` 113. Retail's 0x17c540 (another owner, 34) shows
// the same late tag lookup and early EAX set-up at its 0x211060 call. Also on the lead: the rest offset copied before
// the vertical 162; after the angle call 204 (1373 B); `!normalize3d(&axis)` 113; the arms of `i == 0` swapped 217
// (1371 B). The delta through the existing vector3d_from_points3d 259 on main's body and on the lead (1368 B).
// Round 639 forms: traversal node array base or fixed extent 301/301, shared node velocity 241,
// double constraint distance 260, spring segment or reach ratio name 144/144,
// explicit sine or cosine 144/172, completed constrained point 336.
// NOT kept: the node array forms reach retail length with a 0xa8 frame. The best forms tie
// and leave the selected segment in EDI/ECX and at +0x20.
// Reserve segment, node and stiffness in solver scope, seven subsets and a reordered
// full declaration, 144 each. NOT kept: the segment stays in EDI/ECX and spills at
// +0x20, while retail uses EDX and +0x14.
// M3 lane 5 r210 (fast with 0x116750, 0x116010, 0x1160c0, 0x73360, 0x54590; control 144, register-masked 54): the
// rest offset rotated by a trial copy of 0x146320's file-private rotation taking the axis as three reals (projection
// k, j, i first, cross_j and cross_k named, cross_i in row i) 221 (1368 B); with sine and cosine named before the call
// 167 (1368 B, masked 50: the segment cell moves to +0x10, the base still EDI); that plus the node pointer ahead of
// `inverse_dt` 167; on the body the node pointer ahead of `inverse_dt` 144 and the first-node test as `!i` 144 (both
// the same code). Retail's scratch choice is one step off ours at both ends of the loop (segments base and segment EDX
// for our EDI/ECX at the top, the definition reload EDX for our ECX at the bottom).
// Round 642 forms: branch-owned segment offsets 289, typed rest-vector copy 273, physics
// index before the node snapshot 145, tag entry reference 144, named constraint length 144,
// widened offset scope 144, loop-owned axis 144, terminal selection by node count 181.
// NOT kept: the near-length rest copy changes the saves and the ties retain the EDI/ECX carrier.
// Vector storage for prior, prior/target, prior/predicted, predicted, target/predicted, and
// all three 332/332/332/144/144/332. All three with aggregate prior copy 144 and
// prediction setter 332. NOT kept: the vector forms tie or move the frame and extent
// farther from retail, with the segment spill still different.
// M3 lane 16 r152 (fast with 0x116750, 0x116010 and 0x1160c0, matched throughout; control 144; body unchanged): the
// segment through a file-local inline accessor taking the definition and the index (`(byte *)segments + index * size`)
// 144; that accessor doing the last-segment clamp on its own index parameter 144; `segment` declared ahead of the node
// pointer and assigned after it 144; the non-initial position read as `record->nodes[i].position` 144 (all four the
// same code); the tick increment as `++record->nodes[i].ticks` 275 (1372 B) and the physics call's velocity as
// `&record->nodes[i].velocity` 276 (1372 B): both keep `record + i * 32` in EDX and the segments base then takes EBP;
// the last-segment index named ahead of the node pointer 280 (the base still takes EDI although EDX is unused in that
// block). In every form here the base goes to a free callee-saved register (EDI, else EBP) ahead of ECX and EDX, so
// its register is not a scratch-rotation step and does not come from EDX being busy.
// Shared rotation first cross at its store 144, with the later crosses staged k then j 113.
// Complete vector, axis or both inputs 216/144/216. Staged projection 143, complete output
// before stores 213, three cross products staged j/k/i 144.
// NOT kept: the 113 form gets within one byte of retail, but raises three shared users’ scores
// and loses two matches. The selected segment and length-ratio spill still differ.
// M3 lane 13 r126 (fast with 0x116750, 0x116010, 0x1160c0 and 0x115ff0, matched throughout; control 144): the dt test
// as `0.f < dt` 144 and `node->ticks++` 144 (both the same code); the first node's copies with the direction first 152;
// the loop bound as `i <= definition->segment_count` 286 (1368 B: the guard becomes a compare, the rest unchanged).
// Diagnostic: the segments base read through a file-local forceinline accessor 144 (same code: the base stays in EDI,
// so the one-step scratch rotation is not set by a temporary at the base's own load).
// M3 lane 12 r159 (fast, control 144 at 1371 B), re-measured on the 144 body: the redundant `if (0 < count + 1)` loop
// guard 153; `segment = definition->segments; segment += index` 272 (1369 B: the segment then takes EDX and every later
// scratch register and the i == 0 / node-copy registers are retail's, but the base load sits ahead of the branch and the
// add folds into the stiffness load); the index computed into a local first, then base, then `+=` 144 (same code: the
// base copy is propagated); the ternary of two element addresses 304 (1387 B). New: the selected pointer read through a
// `s_chain_segment *const &` binding 144 (same code). Reading: retail's segment variable takes EDX with ECX free; ours
// takes ECX unless the variable is already live where ECX holds the count.
// Squared separation first, reach first, root first and root after reach all 144;
// with rest-offset components through the vector setter all 273.
// NOT kept: the ratio stages tie. The setter changes the entry saves while retaining
// the 0xa4 frame. The segment carrier and ratio/sine home remain.
// Chain stiffness before the node, after the node, or before the split ordinal 277 each; stiffness after the split
// ordinal 144; both inputs after the addresses 144, before segment selection 277, between selected ordinal and
// address 144, or before the node 277. NOT kept: the early read forms grow to 1375 bytes and the other forms tie.
// Node position reference and both references 144 each, short selected segment ordinal 252,
// rest-vector reference with late copy 246, fixed-record exit and separate exits 144 each,
// node base plus advance 144, final node point setter 335, signed tick assignment 144.
// NOT kept: the short ordinal reaches retail length but changes the saves and regresses.
// The frame agrees and the selected segment still needs EDX and its +0x14 spill.
// MiniMax A (11-20): no new form, no lower score (144); the first difference is the loop head's alignment padding (the three-byte lea esp, [esp]), so the body is 1371 bytes against retail's 1368 and every later jump target is off by three; retail keeps the selected segment in EDX and spills it at +0x14 where ours holds the segments base in EDI and the segment in ECX and spills at +0x20, and the 113 lead needs a file-local one-caller helper, which is match-only and raises three shared users.
// Direct node indexing at input, physics and publication 275/276/242; paired boundaries 281/243/243;
// all accesses via the record or named array base 167/280. NOT kept: these forms do not recover
// the EDX segment and low spill at retail length.
// Coordinate-array loops for interior position, anchor position, anchor direction and
// both anchor values 161/161/161/643; either anchor with interior or all three
// 161/161/643; one combined anchor pass 169. NOT kept: all regress and shift
// attachment output homes. The segment still needs EDX and the +0x14 spill.
// M3 lane 2 r122 (fast with 0x116750, 0x116010 and 0x1160c0, matched throughout; control 144; body unchanged). Lead for the
// EDX base: retail's base, segment and loop-end definition reload all take EDX because ECX is still held when the segment is
// formed. Diagnostics (a count local captured ahead of the tick and stored to the record later; each reverted): used in the
// first-node arm 264, in the other arm 249, or after the tick 265, the segments base and the segment both take EDX (`mov edx,
// [ebx+0x9c]; add eax, edx; mov edx, eax`), the first-node copies and the node-position copy use ECX and the loop end reloads
// the definition into EDX, all as retail (the spill cell stays at +0x20 and the 0x211060 set-up keeps the index in EAX);
// stored ahead of the tick 291 (base EBP) and after the if/else 321 (count callee-saved). So one value living in ECX across
// the segment's definition is the whole ECX/EDX difference; retail shows no instruction for it. Natural spellings that do
// not hold ECX (144, same code, unless a count is given): the tick as `node->ticks = node->ticks + 1`, `(short)(node->ticks
// + 1)`, `+= 1`; `!i`; `i <= 0` 145; `i < 1` 291 (1373 B); `bool first = i == 0` ahead of the tick; ahead of the segment 290
// (1376 B: the flag takes a register, segment EDX, base EBP); `bool last = i == count` for the selection; a count local for
// the selection; a named long index; the tick written in both arms 291 (1373 B); a long copy of `i` for the node, the
// selection and the first-node test 283 (retail's 1368 B: `i` then stays in ECX to a `test ecx, ecx`, the count in EAX,
// segment EDX, base EBP); the physics index in a local read ahead of the first-node test 160, ahead of the tick 162, ahead of
// the segment 270 (1370 B), at the top of its arm 145 (base EBP in the first three, segment still ECX: the physics index is
// not what holds ECX). The segment through accessors taking (count, base, index) by value in three parameter orders, the
// definition and index, or clamping inside 144/144/144/144/145; the count and base as a by-value two-field block local 282
// (1375 B), passed by value 309 (1384 B), by const reference 144; `segment = base; segment += index` re-measured 272 (1369
// B). Diagnostics on the 0x211060 site: no call 418 (1261 B), the call without its test 295, a constant tag 262 (base EDI,
// segment ECX in each).
// M3 lane 11 r138 (fast with 0x116750 and 0x1160c0, matched throughout; control 144): the delta length through the
// existing magnitude3d 144 (same code: it expands); through distance3d(&previous, &position) 313 (1408 B, a call);
// `inverse_dt` declared at the velocity rows 292; the loop counter declared ahead of the loop 144 (same code); the
// stiffness computed in the non-initial arm after the target 303 (1356 B).
// MiniMax B (11-20): no new form, no lower score (144): the selected segment still sits in EDI and ECX at +0x20 where retail keeps it in EDX at +0x14; this round's permuter (76 tries in 604.8 s) found nothing below its baseline of 141 on the reduced image.
// Last-ordinal clamp, branch clamp or bound reference 151/151/151. Indexed fields through
// a carried array and clamped index 280/280. Array-end selection, pointer step-back or
// end-selected reference 301/296/301. NOT kept. Bounds and pointer tests regress.
// M3 lane 11 r142 (fast with 0x116750/0x116010/0x1160c0, all MATCH; control 144, aligned 159, masked 86, frame-blind 41):
// retail is this body with ECX and EDX exchanged from the segment on (segment EDX, here ECX with its base in EDI; then the
// first-node copy, the position copy, the physics index and the offset copy each take the other register), plus the low
// slots. The physics index read once into a `long` local 144 (same code); `1.f / dt` ahead of the loop 154; the vertical
// ahead of the loop 299; the segment selected ahead of the node pointer 278; the last-segment test as
// `definition->segment_count == i` 145; the rotation through the existing rotate_vector_170d70 (k read at each use) 144
// (same code), and every include/axis_rotation.h variant measured for 0x29b2e0 (k local or not, dot or cross order) 144.
// Body unchanged.
// The segment base and address still use EDI and ECX and spill at +0x20.
// Anchor selection by value with test before node/before segment/after segment 185/182/177,
// point source address 182. Segment scope ending at the rest-offset read 144, offset declared
// first in that scope 144, offset declared before segment selection 144, constraint-and-blend
// scope 144. NOT kept: the anchor forms restore length but move the first difference earlier.
// The scope forms tie.
// M3 lane 12 r167 (h2m fast with 0x116750 and 0x1160c0, MATCH in each; control 144): the axis through the existing
// normalize_inline 172 (1368 B, first difference +0xc1: its x87/SSE interleave differs); through cross3f_staged 144 and the
// vertical through vector3f_assign 144 (same code each); `predicted` through vector3f_add 204 (1368 B); the node velocity
// through vector3d_from_points3d and three `*= inverse_dt` 196 (1400 B). Retail's cell shared by the segment, the scale and
// the sine sits at +0x14 below the axis (+0x18..+0x20), ours at +0x20 above it (axis +0x14..+0x1c); retail's rotation
// keeps all three axis components in XMM5/XMM4/XMM6 where ours reloads axis.i and axis.j. Body unchanged.
// The 0xa4 frame and attachment agree. The selected segment still uses EDI/ECX and +0x20 against EDX
// and +0x14. Tried rest offset at constraint join 176, rest offset after vertical setup 176, rest
// offset after cross product 186, rest offset produced by both constraint exits 290, rest offset
// produced before reach by each constraint arm 322, segment base advanced with offset at constraint
// join 272, segment base advanced with offset after cross product 267, selected arm advances segment
// base with arm-owned rest offset 335. NOT kept: Every rest-offset schedule raises the score. Advancing
// the base recovers EDX, but the stiffness read stays folded into the address and the spill remains at
// +0x20.
// Tick read ahead of segment as short or long 288/289, increment computed first 292/291.
// The same four forms after selecting the ordinal 144 each.
// NOT kept: the early read consumes EDX and adds a separate load and store. With a selected ordinal
// the memory increment returns and the segment still spills at ESP+0x20.
// M3 lane 3 r290 (H2M_FAST, control 144): the segment count read once into a `long` ahead of the loop for the bound and the
// selection 304 (first difference +0x2); the selection as `i > last ? last : i` with `long last = count - 1` 151 (1370 B);
// `segment` declared in the outer block and assigned in the loop 144 (same code); `segments + i`, then reassigned to the
// last segment when `i == count`, 274 (1368 B, first difference +0x6); a `long index = i` lowered by an `if` for the last
// node 144 (same code). Retail's loop end reloads the definition into EDX and the count into ECX, ours both into ECX.
// Displacement before position publication with velocity setter 150, existing point
// difference 150, or component stores 150. Complete velocity before commit 346, scaled
// difference output before commit 343, saved old position 346, next prediction after
// commit 175, and coordinate loop for the staged velocity 170. NOT kept: all regress,
// and the segment carrier and +0x20 spill persist in the closest forms.
// Joint velocity/position coordinate pass alone, with prediction, with previous point,
// and with both 363/375/370/393; with the reused coordinate 337/350/349/363.
// NOT kept: the joint pass changes the frame and shortens the function.
// Round 666. Segment selection still uses EDI/ECX and spills at +0x20.
// Immutable segment pointer 144, immutable node and segment pointers 144, terminal segment
// as the fallback arm 144, immutable reciprocal and stiffness 144, segment-stiffness name 144,
// typed point setter at the target 144, at the blend 144, and at both boundaries 144.
// M3 lane 12 r174 (h2m fast with 0x116750 and 0x1160c0, MATCH throughout; control 144). Shared rotation helper forms
// screened on all four users (0x2551f0 and 0x1ff360 MATCH, 0x117dd0 130): cross terms declared up front (i/j/k or k/j/i)
// and assigned i, j, k 144; declared k, j, assigned j, k with cross_i in row i 144; declared ahead of the projection 144
// (same code each: declaration order is inert); any form that assigns cross_k before cross_j 113 (1369 B; 0x2551f0 15,
// 0x1ff360 60, 0x117dd0 158); crosses then projection 147; cross_i, projection, cross_j, cross_k 144 (0x117dd0 152);
// k/j/i then projection 147; j/k/i then projection 147; projection, j, i, k 144 (same code); rows j/i/k 138, i/k/j 143
// (1368 B), k/j/i 174 (every row order loses both matched users). All 64 operand orders of the six cross products 144
// (same code). Trial copies of other files' rotations: the angle-taking one with crosses in each row 320 (1442 B), the
// result-vector one with mixed operand orders 399 (1457 B), the scalar-axis one written with an axis pointer 144. Body
// statements: the angle call ahead of cross3f 320 (1353 B); the tick after the if/else 231, at the loop end 263.
// M3 lane 4 r246 (H2M_FAST with 0x116750 and 0x1160c0, MATCH each time; control 144): the selected segment through a
// file-local inline element getter taking the definition and the index 144, through a forced-inline one 144 (same code: the
// base still takes EDI and the segment ECX); the segments base read into a local ahead of the selection 145; the selection as
// `i < count ? i : count - 1` 145; the tick increment ahead of the selection 285 (1376 B). Retail also ends each pass with a
// store of the rotated offset's middle term to +0x70 from XMM5 where this build stores it inside the rotation. Body unchanged.
// M3 lane 16 r177 (h2m fast with 0x116750 and 0x1160c0, MATCH each; control 144): the name-order lever for the shared
// cell at +0x20 (retail +0x14, below the axis): `axis` renamed `turn_axis` 144 and `unit_axis` 144, `scale` renamed
// `along` 144 (same code each: names do not order these homes); the normalise test as `!normalize3d(&axis)` 144 (same
// code); sine and cosine named ahead of the rotation call 169 (1373 B, first difference +0x2). Body unchanged.
// M3 lane 15 r133 (h2m fast; control 144 at 1371 B, aligned 159 unmatched rows of 322; every rotation form below was a
// trial copy of the shared helper, measured and removed: a second spelling of an existing helper is not keepable). The
// rotation has three states here whatever the dot's spelling (written out i, j, k; through the existing dot3f either way
// round) and with or without the k local, 30 forms: the three cross terms named i, j, k, or the i term at its row with j
// then k named, 144 (1371 B, aligned 159: main's code); the cross through the existing cross3f or cross3f_staged into a
// local vector 143 (1368 B, retail's length, aligned 138: the shared cell moves to +0x1c and the axis to +0x10); the i
// term at its row with k then j named 113 (1369 B, aligned 138: retail's frame with the shared cell at +0x14 and the
// axis at +0x18, all six inputs in registers as retail, but each in another XMM register and the segment still in
// EDI/ECX). Also 144 (same code): the dot written k first; the axis components read into three locals. On the 113 form
// 1..32 dead initialised locals at the top give 113, or 115 for 3, 4 and 5 modulo 8: no state nearer retail.
// Segment selection, with aligned rows: `segment = definition->segments; segment += <the ternary>` 272 (1369 B, aligned
// 134); the index set by an `if` into a local, then the base, then `+=` 144 (same code); that index with the element
// address 144 (same code); the base with `+=` written in both arms of the test 289 (1381 B, aligned 193). Diagnostic,
// the first of those with the 113 rotation: 274 at retail's 1368 B, aligned 108 (the base load still sits ahead of the
// branch and the XMM registers stay permuted). Body unchanged.
// m3 lane 4 r251 (H2M_FAST with 0x116750 and 0x1160c0, MATCH throughout; control 144): the prediction rows after the
// velocity, position and previous stores 175 (1380 B); the axis declared ahead of the vertical 144 and `predicted`
// declared ahead of `previous` 144 (same code each); the sine and cosine named ahead of the rotation call 169 (1373 B);
// the blend weight `1.f - stiffness` named once for the three rows 262 (1355 B).
// M3 lane 13 r147 (h2m fast with 0x116750 and 0x1160c0, MATCH each run; control 144, difflib-aligned 35 groups): the
// velocity rows through the existing vector3d_from_points3d and vector3f_scale 195 (1399 B); the predicted point through
// the existing vector3f_add 204 (1368 B, retail's length, aligned 46); the chain delta through vector3d_from_points3d in
// place of its initializer 259 (1368 B); `previous = node->position = position;` 144 (same code); vector3f_add with the
// helper delta 282 (1355 B). Body unchanged.
// M3 lane 15 r139 (h2m fast with 0x116750 and 0x1160c0, MATCH each; control 144). Reading: retail's integer registers are
// this build's moved one step along EAX, ECX, EDX (physics index ECX for EAX, node copy pointer ECX for EDX, segment EDX for
// ECX, base EDX for EDI). Forms: the node through a file-local inline getter 144 and the tick through a file-local inline
// setter 144 (same code each: neither shifts the rotation); the node as a reference 144 (same code); the node position
// copied member by member 198 (1384 B); the segment pointer declared const 144 and register 144 (same code). Body unchanged.
// Segment still uses EDI/ECX and ESP+0x20. Node position/velocity/both/tick/all field
// pointers 144/241/241/144/240. Direct indexed node fields 167, with position and
// velocity pointers 294, with all three field pointers 294. The velocity-pointer forms
// change the entry saves. The position-only and tick-only pointers tie.
// M3 lane 3 r304 (fast = plain 144, 1371 B; body unchanged). HELD (needs the owner's OK: 0x211060 and its other caller
// 0x17c540 are a contributor's): 0x211060's velocity parameter declared after the position parameter instead of first
// (definition, declaration and both calls; build/r304/held-1161f0.patch in the m3-lane3 worktree) gives this function 95
// (1371 B) and makes 0x17c540 match (34 -> 0), plain-confirmed; 0x211060 itself stays 269 either way. Retail evidence at
// both calls: the tag index is kept out of EAX from its test on (ECX), the position's address goes through EDX, and the
// EAX argument is loaded between the pushes of the settings pointer and the flags; with velocity declared first the
// callers keep the tag index in EAX. The caller reserves EAX from the point where the register argument is evaluated
// (last to first in declared order), so the tag index takes ECX and the segment EDX, and the first node's copies, the
// call set-up and the segment cell at +0x14 follow. Positions measured (this function / 0x17c540 / 0x211060): second
// 142 / 32 / 269; third 97 / 2 / 269; after the position, the force, the normal or the material 95 / 0 / 269 each (the
// two callers cannot tell these four apart); after the radius 95 / 7 / 269; last 250 / 102 / 352.
// Frame reading: this frame (over 0x80 bytes) is ordered by memory references per byte, so the segment/scale/sine cell
// sits below the axis only when the axis has at most 20 references (ours 23, from the rotation's schedule).
// On the held order: r115/r126's file-local rotation helper (projection, cross_k, cross_j, the i cross written in its
// row) 59 (1369 B; only the rotation block and the three exit jumps then differ: retail finishes cross_j ahead of
// cross_k and keeps offset j in a register where ours keeps offset k; lead patch build/r304/lead-1161f0-59.patch), that
// helper alone on main's order 113 (re-measured). Statement orders of that helper on the held order, the i cross inline,
// named first or named last (same code each): projection/cross_k/cross_j 59, cross_k/projection/cross_j 68,
// projection/cross_j/cross_k 95, the three others 98. The vector's k, j or both read into locals first 59 (same code),
// its i 124, i with j 180; the dot's terms in the five other orders 59 (same code). Rows stored i, k, j with the six
// statement orders 70/94/76/94/96/96 (the first at retail's 1368 B, sine and cosine cells exchanged). Other shapes on
// 59's order: new components in three locals 172 (1368 B); each row as `a + (b - c)` 144 (1368 B), as `(a + b) - c` 59;
// the projection term first in each row 59; a named dot 59; a named `1 - cosine` 59; axis-first cross products 59; the
// negated cross added 90 (1368 B); all three crosses named 59.
// With the dot named and its product with `1 - cosine` as a later statement: dot, cross_k, cross_j, product 98; dot,
// cross_k, product, cross_j 68; dot, cross_j, cross_k, product 98; dot, cross_j, product, cross_k 98; dot, product,
// cross_j, cross_k 95. Diagnostic: cross_k's first product named ahead of the projection with cross_j between 95 (with
// the dot named too 95). The k row's cross taken from saved i and j components (cross_j alone named) 178 (1395 B). The
// sine factor written first in each row 59 (same code).
// Forced single allocator decisions on main's body (diagnostic, not source): the tag index in ECX 115, the segment in
// EDX 120; nothing else of 84 tried lowers 144.
// M3 lane 15 r143 (fast = 144, 1371 B; body unchanged; BLOCKED-BY 0x211060, a contributor's function, as lane 3 r304 found).
// Re-measured on this base with 0x116750, 0x1160c0, 0x17c540 and 0x211060 listed: the held parameter order 95 (0x17c540
// MATCH, 0x211060 269), with the file-local rotation helper on it 59 (1369 B, aligned 62; everything outside the
// rotation block then agrees with retail). On that 59 form: 1 to 8 unused locals at the top (diagnostic)
// 59/59/64/64/64/59/59/59, so the rotation block is no symbol-count state; forty forced single allocator decisions
// (float priorities one step up or down, the dot's and the crosses' register picks; diagnostic, not source), none
// below 59 (the nearest 62). Left on it: retail loads the offset's i late into the register of a dead dot temporary and
// keeps axis j in a register through the dot, where ours loads all six components ahead of the first product.
// Separate recurrent prior coordinates, reconstructed before or after physics by
// aggregate initialization or point setter, 331 each. With separate prediction
// coordinates, 332 each. NOT kept: the frame and extent shrink and the
// selected segment still has a different spill.
// M3 lane 5 r245 (h2m fast with 0x116750, 0x116010, 0x1160c0 and 0x211060 listed; control 144, sequence-aligned 159
// rows exact, 41 with registers and stack offsets masked; body unchanged; still BLOCKED-BY 0x211060's parameter order).
// Caller-side forms on main's order: sine and cosine named after the offset copy 169 (1373 B); the angle taken ahead
// of the offset copy 180 (1375 B); the trig arguments through explicit double casts 158; the predicted point through
// the existing vector3f_add 204 (retail's 1368 B, masked 35); the velocity through the existing scaled_difference3f
// 171 (1369 B); the blend's complement named once 262 (1355 B). A forced pick of EDX for the segment (diagnostic, not
// source) 120, as lane 3 r304 measured.
// M3 lane 8 r241 (H2M_FAST with 0x116750, 0x1160c0, 0x17c540 and the helpers' files listed; control 95 at 1371 B, now that
// 0x211060 takes the position ahead of the velocity): only the rotation block and the segment/scale/sine cell (+0x20, retail
// +0x14) differ. Existing rotation helpers called in place of the header's: 0x1ecf50's file's direction rotation 95, 0x170d70's
// file's vector rotation 95 (same code each), 0x146320's file's scalar-axis rotation 192 (1368 B, one cell fewer). The header
// rotation itself spelled projection, cross_k, cross_j with cross_i in its row 59 (1369 B) but matched 0x2551f0 15 and
// 0x1ff360 60 (0x117dd0 158, 0x1e5bb0 129, 0x26d570 42); without its vector_k local 59 (0x2551f0 123, 0x1ff360 111);
// projection, cross_j, cross_k (retail's computation order here) with cross_i in its row 95, both matched users kept.
// Diagnostic: 1-7 dead locals at the top 95/95/100/100/100/95/95, ahead of the rotation 95 each.
// The header rotation's spelling across its six users (this, 0x2551f0, 0x1ff360, 0x1e5bb0, 0x26d570, 0x117dd0; main
// 95/0/0/111/26/130), 20 spellings: with vector_k and the projection first, cross_j named before cross_k (any place for
// cross_i) is main's state; cross_k before cross_j 59/15/60/128/42/158; the projection after the crosses 98 here and
// 0x2551f0 54-87, 0x1ff360 79-113; without vector_k 0x2551f0 122-123. No spelling keeps both matched users and lowers
// another. Body unchanged.
// M3 lane 8 r243 (H2M_FAST with 0x116750, 0x1160c0 and 0x17c540, all MATCH; control 95 at 1371 B): the offset declared
// uninitialised at the loop top and assigned at its copy 95; the axis declared at the loop top 95 (same code each);
// the vertical declared once ahead of the loop 300; the angle taken ahead of the cross product 314 (1353 B); the sine
// named before the rotation call with the cosine in the argument 95; the predicted point's rows stored z, y, x 104.
// M3 lane 16 r196 (fast with 0x116750, 0x1160c0 and 0x17c540, all MATCH; control 95). Reading of the low cells:
// the frame (over 0x80 bytes) is packed by density, references x 1000 / size; the segment/scale/sine cell has 7
// references (1750) and the axis 23 (1916), so the axis sits lower. Retail's listing has the same 7 references on the
// cell but 20 instruction references on the axis against our 22 (one axis i load and one axis j load fewer: retail
// keeps axis i in a register through the whole rotation), which is a tie at 1750; so the cell's place follows the
// rotation block's float allocation and is not a separate cause. Forms: the header rotation's rows with the projection
// term first 95 (same code in all six users); its rows as `v * cosine + (projection * a - cross * sine)` 153 (every
// user worse); its dot through the existing dot3f, either operand order, 95 (0x2551f0 122, 0x1ff360 96, 0x26d570 71);
// the rest-offset copy ahead of the vertical and the axis 273 (1370 B); the velocity rows ahead of the predicted rows
// 128 (1380 B). Body unchanged.
// M3 lane 7 r239 (H2M_FAST with 0x116750, 0x1160c0 and 0x17c540, all MATCH; control 95). Retail's rotation forms the dot
// k, j, i as ours and cross_j and cross_k right after the projection, but holds axis i in a register through the block
// where ours loads it again. Forms on today's base: the inverse time step ahead of the loop 105; the tick increment ahead
// of the stiffness product 102; cosine then sine named ahead of the rotation call 135 (1373 B), sine then cosine 132
// (1373 B); angle, sine and cosine ahead of the rest-offset copy 296 (1377 B). A forced-ranking search (40 decisions)
// finds nothing below 95. Body unchanged.
// M3 lane 1 r180 (fast with 0x116750, 0x1160c0 and 0x17c540, all MATCH; control 95). The rotation block from the
// angle call to the loop end is one allocation block with 18 to 22 float candidates; the axis i value is not held in a
// register through it (retail keeps it in XMM6), which leaves the axis at 22 references and the segment/scale/sine cell
// above it. Forms: the velocity rows through vector3d_from_points3d and an in-place vector3f_scale 148 (1399 B); the
// predicted point as `predicted = position;` then three `+=` rows 176 (1389 B); the normalization test as
// `!normalize3d(&axis)` 95 and the guard as `0.f < dt` 95 (same code each); the last-segment test written
// `definition->segment_count == i` 96; the listed angle, sine and cosine ahead of the rest-offset copy re-measured 296.
// Body unchanged.
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
					function_211060(0, g_4e3b44[definition->physics_index & 0xffff].bytes, &location, NONE, &position, &node->velocity, 0, 0, 0, .02f, dt);
				vector3f delta = { position.x - previous.x, position.y - previous.y, position.z - previous.z };
				real scale = (real)(segment->length / sqrt(length_sq3f(&delta)));
				point3f target;
				vector3f_add_scaled((vector3f *)&previous, &delta, scale, (vector3f *)&target);
				position.x = (1.f - stiffness) * target.x + predicted.x * stiffness;
				position.y = (1.f - stiffness) * target.y + predicted.y * stiffness;
				position.z = (1.f - stiffness) * target.z + predicted.z * stiffness;
				vector3d_from_points3d(&previous, &position, &direction);
			}
			vector3f vertical = { 0.f, 0.f, 1.f };
			vector3f axis;
			cross3f(&vertical, &direction, &axis);
			if (normalize3d(&axis) == 0.f)
				axis = *g_4687ac;
			vector3f offset = segment->rest_offset;
			real angle = function_11ce20(&vertical, &direction);
			function_x84d9e8(&offset, &axis, (real)sin(angle), (real)cos(angle));
			predicted.x = offset.i + position.x;
			predicted.y = offset.j + position.y;
			predicted.z = offset.k + position.z;
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
