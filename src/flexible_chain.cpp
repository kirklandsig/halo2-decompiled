// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_211060.h"
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

// score 179 (was 179): previous/axis homes, segment ownership and rotation scheduling differ;
// seven local-name sets, declaration reversals and entry/loop scopes 194;
// shared cross 207, both point differences 286, both helpers 280;
// scaled difference 212, all vector helpers 285.
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
// Lane 18 round 17 (an LTCG link of this file's generated unit, 180 objects, equal to the full listing; 172
// there for 179): the physics tag in a one-use local 172 and the physics index in a local used for the test and
// the lookup 172 (both identical; the tag address is still formed before the -1 push); 1/dt before the loop 182;
// a long loop index 289 (1358 bytes); the root test as `fabs(length) < .0001f` 172.
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
				real scale = (real)(segment->length / sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i));
				point3f target = { scale * delta.i + previous.x, scale * delta.j + previous.y, scale * delta.k + previous.z };
				position.x = (1.f - stiffness) * target.x + predicted.x * stiffness;
				position.y = (1.f - stiffness) * target.y + predicted.y * stiffness;
				position.z = (1.f - stiffness) * target.z + predicted.z * stiffness;
				direction.i = position.x - previous.x;
				direction.j = position.y - previous.y;
				direction.k = position.z - previous.z;
			}
			vector3f axis = { direction.k * 0.f - direction.j, direction.i - direction.k * 0.f, direction.j * 0.f - direction.i * 0.f };
			real length = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);
			if (.0001f > fabs(length))
				length = 0.f;
			else
			{
				real inverse = 1.f / length;
				axis.i = inverse * axis.i;
				axis.j = axis.j * inverse;
				axis.k = axis.k * inverse;
			}
			if (length == 0.f)
				axis = *g_4687ac;
			vector3f offset = segment->rest_offset;
			vector3f vertical = { 0.f, 0.f, 1.f };
			real angle = function_11ce20(&direction, &vertical);
			real sine = (real)sin(angle);
			real cosine = (real)cos(angle);
			real along = (axis.k * offset.k + axis.j * offset.j + axis.i * offset.i) * (1.f - cosine);
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
