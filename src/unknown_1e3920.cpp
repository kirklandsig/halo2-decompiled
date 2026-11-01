// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E3920.CPP: the radius within which an actor counts as arrived */

#include "unknown_11c920.h"
#include "unknown_1fb7e0.h"
#include "props.h"
#include "ai_actor.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"
#include "vector_products.h"
#include <math.h>
#include <string.h>
#include "unknown_1e46c0.h"

long function_1e4a50(long index);

struct s_actor_reset_view_x
{
	byte field_000[0x290];
	vector3f a;
	vector3f b;
	vector3f c;
	byte field_2b4[0x7fc - 0x2b4];
	long field_7fc;
	long field_800;
	byte field_804[0xc];
	dword flags;
	vector3f field_814;
	real field_820;
	real field_824;
	vector3f field_828;
	vector3f field_834;
	vector3f field_840;
};

// @retail 0x1e3860
void function_1e3860(long actor_index)
{
	s_actor_reset_view_x *actor = (s_actor_reset_view_x *)actor_get(actor_index);
	actor->field_828 = actor->a;
	actor->field_834 = actor->b;
	actor->field_840 = actor->c;
	actor->flags = 0;
	actor->field_820 = 0.0f;
	actor->field_824 = 0.0f;
	actor->field_814 = *g_4687a4;
	actor->field_800 = NONE;
	actor->field_7fc = 0x6000086;
}

/* the actor's movement block of its character tag (function_1e4a50) */
struct s_character_movement_view
{
	byte unknown0[8];
	real arrival_radius;
};

// @retail 0x1e3920
real function_1e3920(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real radius = actor->unknown4cc;

	if (radius == 0.0f)
	{
		s_moving_object *unit = moving_object_get(actor->unit_index);

		if (actor->unknown26c != NONE)
		{
			s_tag_element *element = function_1e5450(actor_index, moving_object_get(actor->unknown26c)->tag_index);

			if (element)
				radius = *(real *)((byte *)element + 0x18);
			if (radius <= 0.0f)
				radius = 0.5f;
		}
		else
		{
			s_tag_element *element;

			if (actor->unknown229 && (element = function_1e5450(actor_index, unit->tag_index)) != 0)
			{
				radius = *(real *)((byte *)element + 0x18);
				if (radius <= 0.0f) radius = 0.3f;
			}
			else
			{
				s_character_movement_view *movement = (s_character_movement_view *)function_1e4a50(actor->tag_index);

				if (movement)
					radius = movement->arrival_radius;
			}
			if (radius <= 0.0f)
				radius = 0.3f;
		}
	}

	if (radius > 0.2f)
		return radius;
	return 0.2f;
}

struct s_actor_point_request
{
	byte field_0[0x14];
	s_record_pool *pool;
	long iterator_index;
	long datum_index;
	long group_index;
	s_element_502420_member_iterator actors;
	short type;
	short mode;
	real radius;
	real radius_squared;
	long index;
	point3f point;
	long result_index;
	real squared_distance;
};

struct s_actor_point_group
{
	long field_0;
	point3f point;
	short field_10;
	short team;
	byte field_14[4];
	long first_actor;
	byte field_1c[0x50 - 0x1c];
};

bool function_1df560(short first, short second);

PRIVATE __forceinline s_actor_point_group *actor_point_next_group(s_actor_point_request *request)
{
	s_actor_point_group *result = NULL;
	if (g_4f55d0->active)
	{
		s_record_pool *pool = request->pool;
		long index = function_16bc00(pool, request->datum_index + 1);
		s_actor_point_group *group;
		if (index != NONE)
		{
			group = (s_actor_point_group *)(pool->data + pool->size * index);
			request->datum_index = index;
			request->iterator_index = (*(short *)group << 16) | index;
		}
		else
		{
			request->datum_index = pool->maximum_count;
			request->iterator_index = NONE;
			group = NULL;
		}
		*(s_actor_point_group **)((byte *)request + 0x10) = group;
		if (group)
			result = group;
		request->group_index = request->iterator_index;
	}
	return result;
}

// @retail 0x1e47d0
s_actor_moving *function_1e47d0(s_actor_point_request *request)
{
	s_actor_view *actor;
	for (;;)
	{
		if (request->index == NONE)
		{
			bool eligible = false;
			s_actor_point_group *group;
			while ((group = actor_point_next_group(request)) != NULL)
			{
				switch (request->type)
				{
				case 0: eligible = !function_1df560(group->team, request->mode); break;
				case 1: eligible = function_1df560(group->team, request->mode); break;
				case 2: eligible = true; break;
				}
				if (eligible)
				{
					vector3f delta;
					vector3d_from_points3d(&request->point, &group->point, &delta);
					if (sqrt(length_sq3f(&delta)) < request->radius + 6.0)
					{
						request->index = request->group_index;
						request->actors.next_index = ((s_actor_point_group *)g_502420->data)[request->index & 0xffff].first_actor;
						break;
					}
				}
			}
		}
		if (request->index == NONE)
		{
			actor = NULL;
			break;
		}
		while ((actor = element_502420_member_next(&request->actors)) != NULL)
		{
			vector3f delta;
			vector3d_from_points3d(&actor->position, &request->point, &delta);
			real squared = length_sq3f(&delta);
			if (squared < request->radius_squared)
			{
				request->result_index = request->actors.index;
				request->squared_distance = squared;
				goto done;
			}
		}
		request->index = NONE;
	}
done:
	return (s_actor_moving *)actor;
}

__forceinline void actor_point_iterator_initialize(s_record_pool_iterator *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->data = g_502420;
		iterator->index = NONE;
		iterator->datum_index = NONE;
	}
}

// @retail 0x1e4770
void function_1e4770(s_actor_point_request *request, short mode, const point3f *point, short type, real radius)
{
	const short *type_reference = &type;
	request->mode = mode;
	request->type = *type_reference;
	request->radius = radius;
	request->radius_squared = radius * radius;
	request->point = *point;
	request->index = NONE;
	request->result_index = NONE;
	actor_point_iterator_initialize((s_record_pool_iterator *)&request->pool);
	request->index = NONE;
}

long function_1e4a10(long index);
real function_30bf0(vector3f *vector);

struct s_actor_impulse_view
{
	byte field_0[0x18];
	long unit_index;
	byte field_1c[0x54 - 0x1c];
	long tag_index;
	byte field_58[0x3b8 - 0x58];
	bool active;
	byte field_3b9[3];
	real magnitude;
	vector3f direction;
};

struct s_character_impulse_view
{
	byte field_0[0x50];
	real threshold;
};

// @retail 0x1e28b0
void function_1e28b0(long actor_index, const vector3f *direction, real magnitude)
{
	s_actor_impulse_view *actor = (s_actor_impulse_view *)actor_moving_get(actor_index);
	s_character_impulse_view *definition = (s_character_impulse_view *)function_1e4a10(actor->tag_index);
	if (definition && magnitude > definition->threshold)
	{
		actor->active = true;
		actor->magnitude = magnitude;
		actor->direction = *direction;
		if (function_30bf0(&actor->direction) == 0.0f)
			actor->direction = *g_4687a8;
	}
}

__forceinline void actor_direction_between_points(const point3f *position, const s_type_c3b527 *target, vector3f *direction)
{
	function_210c90(position, target, direction);
}

// @retail 0x1e3370
bool function_1e3370(long actor_index, void *unknown)
{
	vector3f *direction = (vector3f *)unknown;
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;
	if (!actor->unknown007)
	{
		if (actor->unknown5d0)
			*direction = actor->unknown5ec;
		else if (actor->unknown50c)
		{
			const s_type_c3b527 *target = &actor->unknown4ec;
			const point3f *position = &actor->position;
			actor_direction_between_points(position, target, direction);
		}
		else
			goto done;
		result = true;
		if (function_30bf0(direction) == 0.0f)
			result = false;
	}
done:
	return result;
}

short actor_behavior_find_level(long owner_index, short type);

#pragma inline_depth(0)
// @retail 0x1e1e50
bool function_1e1e50(long actor_index, vector3f *direction)
{
	const long *index_reference = &actor_index;
	vector3f *const *direction_reference = &direction;
	s_actor_moving *actor = (s_actor_moving *)(g_4f55f0->data + (*index_reference & 0xffff) * sizeof(s_actor_moving));
	bool result = false;
	if (actor->prop_index != NONE)
	{
		s_type_f95cd3 *view = function_25d700(actor->prop_index);
		if (view && *(short *)view > 6 && actor_behavior_find_level(*index_reference, 14) != NONE && actor->unknown722 > 0)
		{
			**direction_reference = *(vector3f *)((byte *)actor + 0x758);
			vector3f *value = *direction_reference;
			result = value->i * value->i + value->j * value->j + value->k * value->k > 0.0f;
		}
	}
	return result;
}
#pragma inline_depth()


struct s_actor_periodic_view
{
	byte field_0[0x40];
	bool update_first;
	byte field_41;
	short first_ticks;
	bool update_second;
	byte field_45;
	short second_ticks;
};

struct s_ai_periodic_view
{
	byte field_0[6];
	short first_threshold;
	short first_maximum;
	bool first_used;
	byte field_b;
	short second_threshold;
	short second_maximum;
	bool second_used;
};

bool function_26c120(long actor_index);

// @retail 0x1e3790
void function_1e3790(long actor_index)
{
	s_actor_periodic_view *actor = (s_actor_periodic_view *)actor_moving_get(actor_index);
	actor->first_ticks++;
	actor->second_ticks++;
	bool first = false;
	bool eligible = function_26c120(actor_index);
	if (eligible)
	{
		s_game_time_globals *time = g_510c54;
		if (!((s_ai_periodic_view *)g_4f55d0)->first_used && actor->first_ticks > ((s_ai_periodic_view *)g_4f55d0)->first_threshold &&
			actor->first_ticks * time->rate > 0.5f)
		{
			actor->first_ticks = 0;
			((s_ai_periodic_view *)g_4f55d0)->first_used = true;
			first = true;
		}
		else if (actor->first_ticks > ((s_ai_periodic_view *)g_4f55d0)->first_maximum)
			((s_ai_periodic_view *)g_4f55d0)->first_maximum = actor->first_ticks;
	}
	actor->update_first = first;
	bool second = false;
	s_game_time_globals *time = g_510c54;
	if (!((s_ai_periodic_view *)g_4f55d0)->second_used && actor->second_ticks > ((s_ai_periodic_view *)g_4f55d0)->second_threshold &&
		actor->second_ticks * time->rate > 0.5f)
	{
		actor->second_ticks = 0;
		((s_ai_periodic_view *)g_4f55d0)->second_used = true;
		second = true;
	}
	else if (actor->second_ticks > ((s_ai_periodic_view *)g_4f55d0)->second_maximum)
		((s_ai_periodic_view *)g_4f55d0)->second_maximum = actor->second_ticks;
	actor->update_second = second;
}

struct s_actor_height_definition
{
	byte field_0[0x1a];
	short height_index;
};

struct s_height_table
{
	byte field_0[0x98];
	real heights[6];
};

struct s_height_globals
{
	byte field_0[0xc8];
	long count;
	s_height_table *table;
};

// @retail 0x1e20b0
real function_1e20b0(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_actor_height_definition *definition = (s_actor_height_definition *)function_1e4a50(actor->tag_index);
	short height_index = definition->height_index;
	real result;
	if (height_index == 7)
		result = 100000.0f;
	else
	{
		short index = (height_index < 6 ? height_index : 6) - 1;
		real height = 0.0f;
		s_height_globals *globals = (s_height_globals *)g_4e034c;
		if (index >= 0 && index < 6 && globals && globals->count > 0)
			height = globals->table->heights[index];
		result = (real)sqrt((height * 2.0f) * 6.417322635650635f);
	}
	return result;
}

struct s_actor_control_request
{
	long name;
	short mode;
	byte field_6[0x10 - 6];
	dword flags;
	vector3f movement;
	real first_scale;
	real second_scale;
	vector3f forward;
	vector3f first;
	vector3f second;
	vector3f third;
	byte field_58[0x7c - 0x58];
};

struct s_actor_control_view
{
	byte field_0[8];
	bool reset;
	byte field_9[0x18 - 9];
	long unit_index;
	byte field_1c[0x7fc - 0x1c];
	long name;
	byte field_800[0xc];
	short mode;
	byte field_80e[2];
	dword flags;
	vector3f movement;
	real first_scale;
	real second_scale;
	vector3f forward;
	vector3f first;
	vector3f second;
	vector3f third;
};

struct s_unit_control_view
{
	byte field_0[0x130];
	long simulation_index;
	byte field_134[8];
	long player_index;
};

struct s_unit_state_c6ef0;
class c_class_6a600;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);
void __stdcall function_cbf60(long unit_index, bool active);
void function_690d0(c_class_6a600 *world, long actor_index, const dword *state);

// @retail 0x1e4390
void function_1e4390(long actor_index)
{
	s_actor_control_view *actor = (s_actor_control_view *)actor_moving_get(actor_index);
	s_unit_control_view *unit = (s_unit_control_view *)moving_object_get(*(long volatile *)&actor->unit_index);
	if (unit->player_index == NONE || *((byte *)g_4e8c20 + 6))
	{
		s_actor_control_request state;
		function_c6ef0((s_unit_state_c6ef0 *)&state);
		state.name = actor->name;
		state.flags = actor->flags;
		state.movement = actor->movement;
		state.first_scale = actor->first_scale;
		state.second_scale = actor->second_scale;
		state.mode = actor->mode;
		state.forward = actor->forward;
		state.first = actor->first;
		state.second = actor->second;
		state.third = actor->third;
		if (actor->reset)
		{
			function_cbf60(*(long volatile *)&actor->unit_index, true);
			actor->reset = false;
		}
		function_c6de0(*(long volatile *)&actor->unit_index, &state);
		if (unit->simulation_index != NONE && !g_4cf772 && *(long *)((byte *)g_4cf77c + 8) != 0)
			function_690d0((c_class_6a600 *)g_4cf77c, unit->simulation_index, (const dword *)&state);
	}
}

#include "object_markers.h"
#include "location_zones.h"

struct s_actor_object_sample
{
	point3f point;
	point3f center;
	vector3f direction;
	dword location[2];
	vector3f velocity;
};

long function_1e4990(long index);
void function_1e3a00(long object_index, s_actor_object_sample *sample);
void function_28f8d0(long index);
void function_caf90(long unit_index, point3f *position);
long function_baf40(long object_index);
long __stdcall function_cbd80(long object_index, long *holder_index);
void *function_1e5280(long actor_index, long key);
void function_118e80(long object_index, vector3f *forward);
real normalize2d(point2f *vector);
real function_267370(long object_index);


// score 98 (was 98): the water-plane test is 0x11c120's body (location_point_in_zone_inline) and the marker
// read existing 0xcaf90; left: the NONE resets keep -1 in EDI, which the inlined plane compares
// then share (retail compares with immediates, gives EDI to the cluster table and forms -1 in ECX
// at the resets for them and the carrier compare), the carrier ternary lands in EDI instead of EAX,
// and both crosses' multiply operand orders.
// Round 106: the plane test through the out-of-line 0x11c120 with its noinline declaration dropped
// (locate-only) keeps the same -1 sharing, so LTCG inlining is not the cause; without the
// `(void)&actor_index` line 98 (same code); the second parent fetch in its own local 104; the
// point declared before the sample call 98 (same code); plain cross3f for both crosses 123; the
// entry parent ternary with the found arm first 102.
// frame 0x8c versus 0x90; early NONE and held-bit byte home differ.
// Reduced trials: point memcpy 371; early plane result 334; immutable inputs 371;
// direct actor receiver 371; forward scalar names 371; sampled radius 371;
// word/native plane key 308/308; plain word 385, early result 379, native key 385.
// frame 0x8c versus 0x90; early NONE and held-bit byte home differ;
// frame pointer 468; inline depth zero 608; index-clear schedule fast 376/plain 272;
// direct plane result 418; named held-direction result 371 (fast control 371).
// frame and held-direction byte home remain.
// Field-zero chains 272; sentinel chains 273; signed mode range 343;
// native seat index 322; early holder output 350 (all plain scores).
// frame 0x8c versus 0x90 and held-direction byte home remain.
// Byte plane result 273; left/actor references 273/273;
// motion-direction reference 277; up reference 273 (plain).
// Flag-range memset 441; movement-mode branches 369; named activity time 371;
// per-use geometry fields 371; predicate/index role names 371 (fast control 371).
// Held-direction pointer/reference 386/386; complete explicit plane-result arms 418;
// true-first plane result 371; explicit arms with pointer/reference 425/425;
// true-first result with pointer/reference 386/386 (fast control 371). All restored.
// Round 167: Plane products, complete forward product terms, and both 371 each;
// uniform four-use movement flags alone/with plane/with forward/with both 428 each;
// complete named plane distance 371. Fast control 371; full baseline 273.
// Original arithmetic, repeated flag observations and protected interfaces restored.
// Early held-bit flag at function/active scope: bool 362/362, byte 362/362,
// short 319/319, long 368/368 (fast control 371, callee caveat); all exact-only and restored.
// Round 375: Complete native plane value, mutable/const plane with mutable/const point,
// 452 each; the same four graphs with the shared plane-distance helper 452 each.
// Native snapshot expands the frame; every form and the original float tree restored.
// Round 377: Type-gated held-direction result bool/byte/short/long 367/367/385/382;
// default-first producers 362/362/369/368 (fast control 371). Guarded bool restores
// frame 0x90 but plain 267 gains only six, rejected; second cross helper 367 fast;
// recovered-frame signed plane key 233 fast/373 plain. All executable forms restored.
// Round 176: Shifted held bit on the retained first-cross graph 319 fast/272 plain;
// complete inverse bool/byte/short/long 319 each; staged inverse bool/byte/short
// 368/368/388 (fast control 371). Missing frame dword persists; all forms restored.
// Round 351: native carrier control/held bit/both/seat/control-seat/held-seat/all
// 273/272/272/273/273/272/272; complete readonly metadata 272. One-point gains rejected;
// original external record observations restored; missing frame dword and held-bit byte remain.
// Round 250: Entry receiver declarations, ordinal/type declarations, marker/point/predicate
// declarations, each paired group, all groups, and all groups with phase-role names: 273 each. Every
// value birth and original phase declaration restored. Frame 0x8c/0x90, early NONE and held-direction
// byte home remain.
// Round 111: existing 0xcaf90 marker read 190; inline header copy of the water test 192 (out of line
// 506); existing 0x11c120 with its noinline declaration dropped 132; existing 0xe70b0 action getter
// 313 (its unit lookup is not merged); seat field read in the test 113 (with a staged cross);
// staged cross (every component before the stores) for both crosses 107, with the (second, first)
// parameter order 107; NONE-initialised carrier with an if/else chain 107. Locate-only: the NONE
// resets as 0 give retail's plane block exactly; resets as unsigned 0xffffffff 107. Staged cross
// with retail's first-cross operand order 107 (canonicalised); the 0x118e80 call written in both
// arms of the flag test (retail forms the vector address before the select) 98. Dropping
// 0x11c120's noinline also inlined it into matched 0x25ccd0, so its body moved to a header
// forceinline helper for the callers retail inlines it into (98, 0x11c120 and 0x25ccd0 still match).
// Round 114: NONE-initialised carrier with an if/else chain 98; the chain with an explicit NONE else 98;
// chained NONE resets 99; both crosses through the actor fields at each use 100; named forward
// output pointer 96. Locate-only: the plane test's two compares against another constant give the
// plane block exactly (54): the -1 shared between them and the resets is the cause; the resets then
// take -1 in EAX (retail ECX) and the carrier stays in EDI.
// Round 46: both NONE resets delayed across carrier selection 420 fast; unknown26c alone 415 fast/372 plain;
// unknown274 alone 416 fast; the whole reset group 421 fast; initial/parent/held header getters and
// bool held bit 416 fast each (reduced control 416). Original body restored.
// Round 92 (plain checks; --fast builds this function at 416): helper reading the cluster/zone index
// fields directly 96; the vehicle index split from the carrier (copy before 0xbaf40, set to the root)
// 101, which gives retail's carrier block exactly (EAX, compare against the -1 register, `mov edi, eax`)
// with the -1 still in EDI from the plane test; that with a NONE-initialised if/else carrier 101.
// Locate-only on the split: plane compares against -2 52; cluster and zone keys as words against 0xffff
// 33 (no sharing; left the zone compare's imm16 encoding, one byte, the plane x*i order and three
// second-cross products); cluster word only 358 (the zone -1 joins the reset web); `(short)NONE` 87;
// existing 0x11c120 through a forceinline redeclaration stays out of line 510. All restored.
// Lane 17 round 2 (plain checks; the fast image gives 416 for the unchanged body): retail inlines 0x11c120
// here only (five other callers call it), so the plane compares stay immediates there; our header copy is
// optimized with this body and shares the resets' -1 in EDI. A swapped-operand staged cross (diagnostic)
// compiles to the same products (canonicalised). Parent through an if 352; the later class test through
// `type` 242; carrier chosen before the resets 376; type read after the plane test 127; point declared at
// the top of the block 98.
// Lane 9 r52 (plain): cross3f_staged staged i, j, k 108; left declared before up 98; a named second-cross
// output pointer 98; a named sample pointer for 0x1e3a00 and the zone test 254; up/left declared at the top
// of the block 112. The compile-only /FAs model forms retail's -1 web (`or ecx, -1` at the resets, the
// carrier compared against ECX) but keeps `type` in EDI there, so it does not explain the LTCG sharing.
// Round 23 (plain checks, 98): retail's two crosses order their product operands differently from
// each other (first cross: the first three products flipped against ours; second: the last four),
// while ours compiles both alike. Locate-only: the first cross's global axis through a local pointer
// 98 (same code). The marker point and zone test in their own block 98; `left` declared before `up`
// 98; the zone result through a bool local 100; `carrier` declared first in the block 98; the first
// cross plain (cross3f) with the second staged 114 (1727 bytes); the first staged with the second
// plain 112.
// Round 58 (lane 12, plain checks; --fast builds this function at 416): the carrier's first arm
// re-reading the parent index from the object 403 (frame 0x8c); the carrier's first test on the
// parent index instead of the pointer 335 (1712 bytes); one 0x118e80 call with a ternary index
// argument 107; the held bit as one bool expression 194; a named location pointer for the zone test
// 100. Diagnostic (fast only, unfaithful here): `(short)NONE` in the shared zone helper leaves the
// `or edi, -1` / `cmp ax, di` sharing unchanged.
// Round 147 (m3 lane 4, plain checks; --fast differs only at the 0x28f8d0 call, an aligned diff shows 28 real
// instructions left): a compile-only build of this file without /GL gives retail's immediate zone compares and
// `or ecx, -1` at the resets, so the NONE sharing comes from the LTCG allocation, not the spelling. The shared
// zone helper reading the cluster index at each use 96 (the sharing stays; three cross products flip to retail's
// operand order and three away, so those orders follow internal numbering); the parent pointer set by an `if`
// 352; typed byte fields for 0x265/0x267 98 (same code); the zone result in a named bool 100; `left` declared
// before the up copy 98; type read before the parent 129. Re-measured: without `(void)&actor_index` 98 (same
// code, removed: it only took the parameter's address); the parent ternary with the found arm first 102.
// @retail 0x1e3b60
void function_1e3b60(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	byte *actor_bytes = (byte *)actor;
	if (actor->unknown007)
	{
		function_28f8d0(*(long *)(actor_bytes + 0x1c));
	}
	else
	{
		byte *movement = (byte *)function_1e4990(actor->tag_index);
		byte *object = (byte *)moving_object_get(actor->unit_index);
		long parent_index = *(long *)(object + 0x14);
		byte *parent = parent_index == NONE ? NULL : (byte *)moving_object_get(parent_index);
		short type = *(signed char *)(object + 0xaa);
		function_1e3a00(actor->unit_index, (s_actor_object_sample *)(actor_bytes + 0x22c));
		point3f point;
		function_caf90(actor->unit_index, &point);
		actor_bytes[0x265] = location_point_in_zone_inline((s_location *)(actor_bytes + 0x250), &point, NULL);
		actor->unknown229 = (movement[0] & 2) != 0 ||
			(type == 0 && *((byte *)moving_object_get(actor->unit_index) + 0x3dc) == 2);
		actor->unknown266 = false;
		actor_bytes[0x267] = 0;
		actor->unknown268 = false;
		actor_bytes[0x269] = 0;
		actor->unknown26c = NONE;
		*(short *)(actor_bytes + 0x270) = 0;
		actor->unknown274 = NONE;
		long carrier = parent && *(byte *)(parent + 0xaa) == 1 ? parent_index :
			*(byte *)(object + 0xaa) == 1 ? actor->unit_index : NONE;
		if (carrier != NONE)
		{
			long root = function_baf40(carrier);
			if (((s_moving_object_header *)g_4e0300->data)[root & 0xffff].type == 1)
				carrier = root;
			byte *carrier_object = (byte *)moving_object_get(carrier);
			actor->unknown26c = carrier;
			if (*(long *)(carrier_object + 0x248) == actor->unit_index)
			{
				byte *definition = g_4e3b44[*(long *)carrier_object & 0xffff].bytes;
				if (*(short *)(definition + 0x1f0) != 6)
				{
					actor->unknown266 = true;
					*(short *)(actor_bytes + 0x270) = 1;
					if ((bool)((*(dword *)(definition + 0x1ec) >> 11) & 1))
					{
						if ((bool)((*(dword *)(definition + 0x1ec) >> 12) & 1))
						{
							actor->unknown229 = true;
							*(short *)(actor_bytes + 0x270) = ((bool)((*(dword *)(definition + 0x1ec) >> 13) & 1)) ? 2 : 4;
						}
						else if ((bool)((*(dword *)(definition + 0x1ec) >> 13) & 1))
							*(short *)(actor_bytes + 0x270) = 3;
						else
						{
							short mode = *(short *)(definition + 0x1f0);
							if (mode == 1 || mode == 0 || mode == 2)
								*(short *)(actor_bytes + 0x270) = 5;
						}
					}
				}
			}
		}
		parent_index = *(long *)(object + 0x14);
		if (parent_index != NONE)
		{
			s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[parent_index & 0xffff];
			if ((1 << header->type) & 3)
			{
				parent = header->object;
				if (*(long *)(parent + 0x24c) == actor->unit_index)
				{
					long holder = NONE;
					long held = function_cbd80(parent_index, &holder);
					if (held != NONE && function_1e5280(actor_index, *(long *)moving_object_get(held)))
					{
						actor->unknown268 = true;
						actor->unknown274 = holder;
					}
				}
				if (!actor->unknown268 && !actor->unknown266 && *(short *)(object + 0x1fc) != NONE)
				{
					byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
					actor_bytes[0x269] = (byte)((*(dword *)(*(byte **)(definition + 0x1cc) + *(short *)(object + 0x1fc) * 0xb0) >> 11) & 1);
				}
			}
		}
		if (*(long *)(object + 0x14) != NONE)
			actor_bytes[0x267] = !actor->unknown266;
		if (function_26c120(actor_index))
		{
			actor->unknown28c = NONE;
			actor_bytes[0x278] = 0;
		}
		actor->unknown264 = false;
		if (*(byte *)(object + 0xaa) == 0 && actor->unknown26c == NONE)
		{
			byte *unit = (byte *)moving_object_get(actor->unit_index);
			if (*(signed char *)(unit + 0x399) * g_510c54->rate >= 0.2f &&
				*(short *)(unit + *(short *)(unit + 0x346) + 0x36) == 0)
				actor->unknown264 = true;
		}
		if (actor->unknown266)
			function_118e80(actor->unknown26c, &actor->unknown290);
		else
			function_118e80(actor->unit_index, &actor->unknown290);
		if (!actor->unknown229)
		{
			if (normalize2d((point2f *)&actor->unknown290) > 0.0f)
				actor->unknown290.k = 0.0f;
			else
				actor->unknown290 = *g_4687a8;
		}
		if (actor->unknown268)
		{
			long held_index = actor->unknown274;
			s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[held_index & 0xffff];
			byte *held = header->object;
			byte held_bit = 0;
			if (header->type == 1)
				held_bit = (byte)((*(dword *)(g_4e3b44[*(long *)held & 0xffff].bytes + 0x1ec) >> 8) & 1);
			if (held_bit)
				function_118e80(held_index, (vector3f *)(actor_bytes + 0x29c));
			else
				*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(held + 0x168);
		}
		else
			*(vector3f *)(actor_bytes + 0x29c) = *(vector3f *)(object + 0x168);
		vector3f *up = (vector3f *)(actor_bytes + 0x2a8);
		*up = *(vector3f *)(object + 0x18c);
		vector3f *left = (vector3f *)(actor_bytes + 0x2b4);
		cross3f_staged(g_4687b0, up, left);
		function_30bf0(left);
		cross3f_staged(up, left, (vector3f *)(actor_bytes + 0x2c0));
		*(long *)(actor_bytes + 0x2d0) = *(long *)(object + 0xec);
		*(long *)(actor_bytes + 0x2d4) = *(long *)(object + 0xf0);
		*(long *)(actor_bytes + 0x2d8) = *(long *)(object + 0x100);
		*(long *)(actor_bytes + 0x2dc) = *(long *)(object + 0xfc);
	}
	*(real *)(actor_bytes + 0x2cc) = function_267370(actor->unit_index);
	actor_bytes[0x2e0] = 0;
}

struct s_actor_child_limit_view
{
	byte field_0[0xbc];
	dword : 3;
	dword limited : 1;
	dword : 28;
	byte field_c0[0xe6 - 0xc0];
	short limit;
};

long function_25d810(long object_index, long actor_index, bool create);
long function_25c3a0(long actor_index, long prop_ref_index, short mode);
void function_264210(long actor_index, long prop_ref_index);
void function_25b9f0(long actor_index, long prop_ref_index, vector3f const *direction);
short ai_player_index_get(long player_index);
void function_10e9f0(long object_index, short channel, real value, real time);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

// @retail 0x1e2570
void __stdcall function_1e2570(long actor_index, word type, long object_index, real amount, long direction)
{
	long prop_ref_index = NONE;
	if (object_index != NONE)
	{
		prop_ref_index = function_25d810(object_index, actor_index, true);
		if (prop_ref_index != NONE)
		{
			s_prop_datum *reference = prop_ref_get(prop_ref_index);
			if (reference->tracking_index == NONE && g_470f10[reference->type].unknown8 == 2)
				function_25c3a0(actor_index, prop_ref_index, 3);
			function_264210(actor_index, prop_ref_index);
			s_type_f95cd3 *view = NULL;
			if (reference->tracking_index != NONE)
			{
				s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
				if (tracking)
					view = &tracking->view;
			}
			s_type_76cf92 *prop = prop_get(reference->prop_index);
			if (view)
			{
				if (prop->unknown25)
					view->unknown58 = amount * 0.2f + view->unknown58;
				else
					view->unknown58 = amount + view->unknown58;
				*(short *)((byte *)view + 0x28) = 0;
				view->unknown2a = true;
			}
			*(real *)((byte *)prop + 0x2c) += amount > 1.0f ? 1.0f : amount;
			if (reference->state < 1)
				prop_ref_index = NONE;
			if (prop->unknown25 && !prop->unknown23)
			{
				s_actor_view *actor = actor_get(actor_index);
				if (actor->unknown26c != NONE)
				{
					long player_index = *(long *)((byte *)ai_object_get(object_index) + 0x13c);
					if (player_index != NONE)
					{
						short player = ai_player_index_get(player_index);
						if (player == actor->unknown31c || player == NONE)
						{
							actor->unknown31c = player;
							real scaled_ticks = (real)g_510c54->field_2_3 * 0.6f;
							long rounded_ticks;
							__asm
							{
								fld scaled_ticks
								fistp rounded_ticks
							}
							actor->unknown31e += (short)rounded_ticks;
						}
					}
				}
			}
		}
		function_25b9f0(actor_index, prop_ref_index, (vector3f const *)direction);
	}
	byte *actor = (byte *)actor_get(actor_index);
	if (amount > 0.0f)
	{
		long time = g_510c54->game_time;
		/* the interval test looks the actor up again (retail forms the actor pointer with a lea) */
		if (*(long *)((byte *)actor_get(actor_index) + 0x308) == NONE
			|| time - *(long *)((byte *)actor_get(actor_index) + 0x308) > g_510c54->field_2_3)
		{
			if (*(long *)(actor + 0x18) != NONE)
				function_20ba60(3, *(long *)(actor + 0x18), object_index, NONE, type, NULL);
			*(long *)(actor + 0x308) = time;
		}
		*(real *)(actor + 0x318) += amount;
		if (*(long *)(actor + 0x18) != NONE)
		{
			actor = (byte *)actor_get(actor_index);
			short ticks = g_510c54->field_2_3 * 2;
			function_10e9f0(*(long *)(actor + 0x18), 12, 1.0f, 0.5f);
			*(short *)(actor + 0x6ce) = ticks;
		}
	}
	if (type == 11)
	{
		actor = (byte *)actor_get(actor_index);
		if (*(long *)(actor + 0x18) != NONE)
		{
			long child_index = *(long *)((byte *)ai_object_get(*(long *)(actor + 0x18)) + 0x10);
			s_actor_child_limit_view *first_definition = NULL;
			short count = 0;
			short limit = 50;
			while (child_index != NONE)
			{
				byte *child = (byte *)ai_object_get(child_index);
				if (child[0xaa] == 5)
				{
					s_actor_child_limit_view *definition = (s_actor_child_limit_view *)g_4e3b44[*(long *)child & 0xffff].bytes;
					if (TEST_FIELD_BIT(definition->limited))
					{
						if (!first_definition)
						{
							limit = definition->limit;
							first_definition = definition;
						}
						count++;
					}
				}
				child_index = *(long *)(child + 0xc);
			}
			if (count >= limit)
				function_1a8220(actor_index, 12, 1, 3, 1, NONE, 0);
		}
	}
}

#include "unknown_1fb7e0.h"
// @retail 0x1e18f0
void __fastcall function_1e18f0(long actor_index, long old_weapon, long player_index, long new_weapon)
{
 long const *player_reference = &player_index;
 long const *weapon_reference = &new_weapon;
 byte *player = g_4e8c24->data + (*player_reference & 0xffff) * 0x21c;
 real old_value = 0.0f;
 real new_value = 0.0f;
 if (old_weapon != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (old_weapon & 0xffff) * 12 + 8);
  old_value = *(real *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0x238);
 }
 if (*weapon_reference != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (*weapon_reference & 0xffff) * 12 + 8);
  new_value = *(real *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0x238);
 }
 if (new_value > old_value)
 {
  if (actor_get(actor_index)->unknown018 != NONE)
   function_20ba60(0xbc, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (old_value > new_value)
 {
  if (actor_get(actor_index)->unknown018 != NONE)
   function_20ba60(0xbd, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
 }
 else if (actor_get(actor_index)->unknown018 != NONE)
  function_20ba60(0xbe, actor_get(actor_index)->unknown018, *(long *)(player + 0x2c), NONE, NONE, 0);
}


/* Actor equipment cleanup uses retail SSE arithmetic. */

extern bool g_4f55e8;
long function_1e4a10(long character_index);
long function_1e4990(long character_index);
void function_d5c90(long unit_index, real *body, real *shield);
void __stdcall function_ccff0(long unit_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
short function_cccd0(long unit_index, short type, short count);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void __stdcall function_b8540(long object_index);
void *character_grenade_settings_get(long character_index, short key);
struct s_203780;
union vector2f;
struct s_28dab2;
struct s_type_c3b527;
struct s_unit_request;
long __stdcall function_28dab0(long character_index, s_28dab2 const *placement);
void function_2036c0(long character_index, short mode, bool *enabled, bool *forced, real *value);
bool function_203900(short squad_index, real probability);
long __stdcall function_1e02f0(long squad_index, long entry_index, long unit_index, long perception_index, bool flag);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);
long function_1e06b0(long tag_index);
point3f *function_210850(s_type_c3b527 const *point, point3f *out);
void function_11dfb0(vector2f const *rotation, vector3f *forward, vector3f *up);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
real function_30bf0(vector3f *vector);
point3f *function_b9dd0(long object_index, point3f *position);
long function_cbd50(long unit_index, short weapon_index);
void function_b9c60(long object_index, bool flag);
bool function_e6900(long unit_index, s_unit_request *request);
void function_a7b30(long index);
void __stdcall swarm_delete(long perception_index);
void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type);


// score 320 (was 320): Frame 0x118 agrees; persistent sentinel EDI/ESI and squad/substitution homes remain.
// Earlier: Frame 0x118 agrees; sentinel EDI/ESI, squad/substitution homes and success layout remain.
// Earlier 345 (was 364): frame 0x11c versus 0x118; sentinel and success blocks differ.
// frame 0x118 agrees; entry sentinel, squad and substitution/result homes differ.
// Earlier form: frame 0x11c versus 0x118; sentinel and success blocks differ.
// Frame 0x118 agrees; entry sentinel EDI/ESI and squad/settings/substitution homes remain.
// frame 0x11c versus 0x118; sentinel and success blocks differ.
// Facing products 345/1272; split squad lookup 388; direct substitution gates 369;
// guarded Boolean 384; typed actor fields 364.
// frame 0x11c versus 0x118; missing-index register and squad/settings homes differ.
// Byte creation predicate 383; word predicate 361; short grenade 377; depth one 364; grenade first 364; word pair 375.
// Round 156: depth zero 348; target size 504; table-base reads with complete
// equipment results 346, entry cleanup outputs 342, substitution output order 342.
// Three-point gains rejected; original frame and body retained.
// Equipment base read at use 342 (frame 0x118; three-point gain rejected);
// count reads 350; both 352; selected-character local 380; outer creation buffer 348;
// base read plus Boolean predicate 365, shifted Boolean 364, combined loop guard 438,
// or full-width shifted predicate 350.
// Earlier 320 (was 345): frame 0x118 agrees; sentinel and later register homes differ.
// Base reads with explicit success/cleanup 320; positive flags guard 342;
// complete equipment arms 346; squad-mode word 342; guarded long predicate 350.
// Frame 0x11c versus 0x118 and missing-index homes remain; repaired frame with vertical-first, second-horizontal-first, vector initializer, word creation or word request each 342; three-point gains rejected.
// Earlier 320 (was 320): Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Earlier 320 (was 320): Frame 0x118 agrees; sentinel and settings/squad homes remain.
// Earlier form: frame 0x118 agrees; sentinel and later register homes differ.
// Earlier form: Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Earlier form: Frame 0x118 agrees; sentinel and settings/squad homes remain.
// frame 0x118 agrees; sentinel and later register homes differ.
// Frame 0x118 agrees; sentinel and scalar homes remain. Unit-first repaired join 354; null-first squad 344; group/settings names 320; shared object getters 320; early equipment count 332.
// Frame 0x118 agrees; sentinel and settings/squad homes remain.
// Native Boolean flag 357; guarded Boolean flag 356; late cleanup indices 365; entry placement
// view 320; complete replacement arms 358; late actor result 367; zero grenade guards 339;
// short grenade result 338.
// Frame 0x118 agrees; sentinel, squad/settings homes and success layout remain. Tried scoped
// perception cleanup 366; unit producer cleanup 363; both producer cleanups 347; outer perception cleanup 366;
// both with outer index 347; late actor result 345; Boolean creation mode 378; late unit sentinel 356; perception
// first cleanup 320; independent perception first 337; independent unit first 337; perception first with Boolean
// mode 357.
// Round 152: Shared squad slot 324; retained next character 369; both 342; read-only metadata 320; both/read-only 342; next/query workspace 369; both/query workspace 341; all substitution forms 341.
// Chained cleanup defaults/all three defaults/reverse chain/perception-first defaults 320 each;
// unsigned actor/cleanup/all handles and unsigned chained defaults 320 each.
// Short squad admission 382; identity first 360; horizontal/vertical/both angle inputs 337/321/338; angles plus admission 383; vertical-first inputs 338; angle-home names 338.
// Separate actor/unit/perception creation producers from persistent outputs 320/320/320;
// actor-unit/actor-perception/unit-perception/all producer splits 320/320/320/320;
// all splits with delayed cleanup publication 331. Original output and cleanup ownership restored.
// Round 377: Complete clear-mode bool/byte/short/long results 358/358/337/320;
// guarded producers 349/349/345/328 (fast control 320). Both creation-call values
// preserved; polarity does not recover sentinel or squad homes. All forms restored.
// Native creation/request/both payloads 320/320/320; direct variant field alone/with
// creation/with request/with both 339/339/339/339; direct variant with entry placement view 339.
// Round 176: Complementary substitution result bool/byte/short/long, guarded
// producer 362/362/364/363; complete conditional producer 372/372/375/374.
// Frame 0x118 agrees; sentinel and query-result homes persist. All forms restored.
// Lane 5 r77: retail's substitution loop is top-tested and not rotated (its jmp only skips the
// head's padding), computes the result as `forced ? 0x203900(...) : enabled`, keeps the flag as a
// byte `(flags >> 10) & 1` in the squad pointer's slot, tests grenade indices as ints and reads the
// palette count at each use. Each alone: int grenade tests 349 (frame changes), count at each use
// 336, byte shifted flag 359, ternary result 358 (its block then matches retail); all four 379
// (aligned 233 against 243). All restored.
// Round 149: Substitution producer/test ordinary inline, noinline, paired and mixed contracts [320, 320,
// 320, 320, 320, 320, 320, 320]; all restored.
// Round 196: Initial definition/cleanup/squad/lookup observation schedules
// [331, 333, 368, 342, 368, 342, 336, 348]; original observation order and all mutable work restored.
// Round 30: Squad parameter reference 320; unit re-observation 340; separate early failure 338;
// uninitialized/late-scoped actor result 338 each; settings lookup noinline 320; unit re-observation
// with a second guard or separate phases 340 each; cleanup indices first 320. Opening body restored.
// Round 231: independent query-output initialization orders 320 each in five forms;
// enabled-output default omitted 348; corrected-join Boolean/probability home identities 320/320.
// Original query defaults and all executable source restored; sentinel and homes remain.
// DL167: Native unsigned character/squad/returned handle interfaces, seven nonempty
// subsets, and all with the unsigned actor result: 320 each. Every declaration, original
// handle domain and complete body restored; frame 0x118 and sentinel EDI/ESI remain.
// Round 414: direct success-path object/weapon/metadata views, their pairs and all three
// 320 each; immutable placement view at each use 339. Original observations and full body restored.
// Round 92 (--fast, reproduces 320): pairs of lane 5 r77's block shapes (a int grenade tests, b palette
// count at each use, c the squad flag as a bool set from `(flags >> 10) & 1` under `if (squad)`, d the
// ternary substitution result): c 356; c+d 363; b+d 350; b+c 357; a+b 346; a+d 355; a+c 350; b+c+d 365;
// all four 369. First difference unchanged: retail's -1 web (ESI) reaches the loop-entry
// `squad_index != NONE` compare with squad_index in ECX; ours keeps squad_index in ESI and -1 in EDI.
// Lane 9 r52 (fast): an unrotated top-tested substitution loop gives retail's whole entry (-1 in ESI, the
// squad index in ECX): `bool replace = true; while (replace && next != NONE)` with the ternary result 302
// (a flag loop, kept only with a match); with the byte squad flag 322; with the mode switch 324; unit arm
// first on those 324 aligned worse. Flag-free `if (substitute && squad) while (squad_index != NONE &&
// next != NONE)` gives the same loop shape but LTCG then inlines 0x1e4990 at the in-loop call 423 (also
// with the advance under `if (ternary) ... else break;` 423). Unit field read at each use: two sequential
// guards 310 (a repeated test), one guard 333. VC places the unit arm right after the perception arm
// whatever the source order (goto-ordered arms, cleanup first, an inlined unit arm, a switch on the bit).
// @retail 0x1e0850
long __stdcall function_1e0850(long character_index, long squad_index, s_203780 const *placement, bool substitute)
{
	long actor_index = NONE;
	{
		byte *definition = g_4e3b44[character_index & 0xffff].bytes;
		long unit_index = NONE, perception_index = NONE;
		byte *flags = (byte *)function_1e4990(character_index);
		byte *squad = squad_index == NONE ? NULL : *(byte **)((byte *)g_4e0350 + 0x164) + (squad_index & 0xffff) * 0x74;
		if (substitute && squad && squad_index != NONE)
			while (*(long *)(definition + 0x28) != NONE)
			{
				bool enabled = false, forced = false;
				real probability = 0.0f;
				function_2036c0(character_index, *(short *)(squad + 0x30), &enabled, &forced, &probability);
				bool replace = enabled;
				if (forced) replace = function_203900((short)squad_index, probability);
				if (!replace) break;
				character_index = *(long *)(definition + 0x28);
				definition = g_4e3b44[character_index & 0xffff].bytes;
				flags = (byte *)function_1e4990(character_index);
			}
		if (!flags) goto done;
		long flag = squad && (*(dword *)(squad + 0x20) & 0x400) != 0;
		byte const *data = (byte const *)placement;
		if (*flags & 1)
		{
			perception_index = function_28dab0(character_index, (s_28dab2 const *)placement);
			if (perception_index == NONE) goto done;
			actor_index = function_1e02f0(squad_index, character_index, NONE, perception_index, flag);
		}
		else
		{
			byte creation[0xc4];
			function_b7930(creation, *(long *)(definition + 0x10), NONE, 0);
			function_210850((s_type_c3b527 const *)(data + 4), (point3f *)(creation + 0x1c));
			long variant = *(long *)(data + 0x30);
			if (!variant) variant = function_1e06b0(character_index);
			*(long *)(creation + 0xc) = variant;
			function_11dfb0((vector2f const *)(data + 0x14), (vector3f *)(creation + 0x28), (vector3f *)(creation + 0x34));
			unit_index = function_b7b40(creation);
			if (unit_index == NONE) goto done;
			long primary = NONE, secondary = NONE, grenade = NONE;
			short primary_index = *(short *)(data + 0x22);
			if (primary_index == NONE && squad) primary_index = *(short *)(squad + 0x3c);
			long count = *(long *)((byte *)g_4e0350 + 0x98);
			if (primary_index != NONE && primary_index >= 0 && primary_index < count) primary = *(long *)(*(byte **)((byte *)g_4e0350 + 0x9c) + primary_index * 0x28 + 4);
			short secondary_index = *(short *)(data + 0x24);
			if (secondary_index == NONE && squad) secondary_index = *(short *)(squad + 0x3e);
			if (secondary_index != NONE && secondary_index >= 0 && secondary_index < count) secondary = *(long *)(*(byte **)((byte *)g_4e0350 + 0x9c) + secondary_index * 0x28 + 4);
			short grenade_index = *(short *)(data + 0x2c);
			if ((short)(grenade_index - 1) != NONE) grenade = grenade_index - 1;
			else if (squad && (short)(*(short *)(squad + 0x40) - 1) != NONE) grenade = (word)*(short *)(squad + 0x40) - 1;
			function_1e04a0(primary, character_index, unit_index, secondary, (short)grenade);
			actor_index = function_1e0160(squad_index, character_index, unit_index, flag);
		}
		if (actor_index != NONE)
		{
			byte *actor = (byte *)actor_get(actor_index);
			if (data[0x1c] & 1) *(short *)(actor + 0x86) = 0;
			if (*(real *)(data + 0x38) > 0.0f)
			{
				point3f position;
				function_210850((s_type_c3b527 const *)(data + 4), &position);
				vector3f direction;
				direction.i = (real)(cos(*(real *)(data + 0x14)) * cos(*(real *)(data + 0x18)));
				direction.j = (real)(sin(*(real *)(data + 0x14)) * cos(*(real *)(data + 0x18)));
				direction.k = (real)sin(*(real *)(data + 0x18));
				function_1f34b0(actor_index, &position, &direction, *(real *)(data + 0x38), NONE);
			}
			long unit = *(long *)(actor + 0x18);
			if (unit != NONE)
			{
				short mode = *(short *)(data + 0x3e);
				byte request[0x20];
				if (mode == 2)
				{
					byte *movement = (byte *)function_1e4a50(*(long *)(actor + 0x54));
					if (movement && (*movement & 0x20))
					{
						*(long *)request = 0x2c;
						request[4] = true;
						function_e6900(unit, (s_unit_request *)request);
					}
				}
				else if (mode == 1)
				{
					vector3f forward;
					function_b9fc0(unit, &forward, NULL);
					forward.k = 0.0f;
					if (function_30bf0(&forward) > 0.0f)
					{
						*(long *)request = 0x22;
						function_b9dd0(unit, (point3f *)(request + 4));
						*(real *)(request + 0x10) = forward.i * -1.0f;
						*(real *)(request + 0x14) = forward.j * -1.0f;
						request[0x18] = true;
						function_e6900(unit, (s_unit_request *)request);
					}
				}
				byte *object = *(byte **)(g_4e0300->data + (unit & 0xffff) * 12 + 8);
				long weapon = function_cbd50(unit, (char)object[0x212]);
				if (weapon != NONE)
				{
					byte *weapon_object = *(byte **)(g_4e0300->data + (weapon & 0xffff) * 12 + 8);
					byte *local_67e06b_3 = g_4e3b44[*(long *)weapon_object & 0xffff].bytes;
					if (*(long *)(local_67e06b_3 + 0x2f4) != NONE) actor[0x22a] = true;
				}
				if (data[0x1c] & 0x10) function_b9c60(unit, true);
			}
			function_a7b30(unit_index);
		}
		else
		{
			if (unit_index != NONE) function_b8540(unit_index);
			else if (perception_index != NONE) swarm_delete(perception_index);
		}
	}
done:
	return actor_index;
}

// @retail 0x1e04a0
void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type)
{
	real *limits = (real *)function_1e4a10(character_index);
	if (limits)
	{
		short difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : (short)1;
		real body, shield;
		switch (difficulty)
		{
		case 3:
			body = limits[3];
			shield = limits[4];
			if (body <= 0.0f && shield <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			break;
		case 2:
			if (limits[3] <= 0.0f && limits[4] <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			else
			{
				body = (limits[1] + limits[3]) * 0.5f;
				shield = (limits[4] + limits[2]) * 0.5f;
			}
			break;
		default:
			body = limits[1];
			shield = limits[2];
			break;
		}
		if (g_4e6948->state == 1 && g_4f55e8)
		{
			body *= 2.0f;
			shield *= 2.0f;
		}
		if (body > 0.0f || shield > 0.0f)
			function_d5c90(unit_index, &body, &shield);
	}
	byte placement[0xc4];
	if (primary_tag != NONE)
	{
		function_ccff0(unit_index);
		function_b7930(placement, primary_tag, unit_index, 0);
		long weapon = function_b7b40(placement);
		if (weapon != NONE && !function_cd0c0(unit_index, weapon, 1))
			function_b8540(weapon);
	}
	if (secondary_tag != NONE)
	{
		function_b7930(placement, secondary_tag, unit_index, 0);
		long weapon = function_b7b40(placement);
		if (weapon != NONE)
		{
			byte *settings = (byte *)function_1e4990(character_index);
			if (!((settings[0] & 4) && function_cd0c0(unit_index, weapon, 5)) &&
				!function_cd0c0(unit_index, weapon, 1))
				function_b8540(weapon);
		}
	}
	if (grenade_type != NONE)
	{
		byte *settings = (byte *)character_grenade_settings_get(character_index, grenade_type);
		if (settings)
		{
			short lower = *(short *)(settings + 0x34);
			short upper = (short)(*(short *)(settings + 0x36) + 1);
			long count = ((random_next(&g_4e7408->unknown0) * (upper - lower)) >> 16) + lower;
			function_cccd0(unit_index, grenade_type, count);
		}
	}
}

bool __stdcall function_25d020(long actor_index, long object_index, long type,
	long prop_index, long unknown, long reference_index);

extern long g_46fc80;
void function_1a6f70(long actor_index);
void function_1e3790(long actor_index);
void function_262ba0(long actor_index);
void function_204ca0(long actor_index);
void __stdcall function_26dde0(long actor_index);
void function_266640(long actor_index);
void function_267c50(long actor_index);
void function_1e36d0(long actor_index);
void function_258660(long actor_index);
void __stdcall function_1a7cc0(long actor_index);
void actor_behavior_current_update44(long actor_index);
void function_1e3860(long actor_index);
void actor_behavior_current_update48(long actor_index);
void __stdcall swarm_update(long actor_index);
void function_25a380(long actor_index);
void function_1f38c0(long actor_index);
void function_298370(long actor_index);
void __stdcall function_1fc7e0(long actor_index);
void function_1e4390(long actor_index);
void function_2959c0();
void function_1e2a90(long actor_index);
bool function_1e35f0(long actor_index);
void function_1e3b60(long actor_index);

// score 26 (was 26): retail passes the actor index to function_262ba0 and to
// function_1f38c0 in EAX; our build passes the first on the stack and the
// second in ECX, because those two callees' argument registers are fixed in
// their own translation units.
// inline/noinline callee declaration 26; noinline declaration 26;
// unsigned callee input 26; inline unsigned callee declaration 26;
// unsigned caller input 26; inline caller 26; inline depth two 26; depth three 26.
// Round 41: the written 0x1f38c0 and 0x1f6090 bodies enabled 90 (a frame slot; 0x1e2f50
// and 0x1c83c0 stop matching); in 0x262ba0, which spills the actor because a loop bool
// takes EBX: 0x2641c0 inlinable 26 (0x264210 lost), plain maximum word 26, call result
// only clearing the sample flag 26, sample/update flags at the loop top 26.
// Round 21 (m3 lane 2, fast): in 0x262ba0 the `update` bool takes EBX (the only callee-saved
// byte register) across the 0x2640c0/0x2641c0 calls, so the actor stays a stack parameter; retail
// keeps the three flags in stack bytes. Tried there: sample/update as two assignments 26 (0x262ba0
// 424), declared after the node call 26, update declared first 26 (422), sampled_one before
// maximum 26, the update test nested 26.
// BLOCKED-BY 0x262ba0 (stacked actor) and 0x1f38c0 (stub until #581 is solved).
// @retail 0x1e34e0
void function_1e34e0(long actor_index)
{
	byte *actor = (byte *)actor_get(actor_index);
	g_46fc80 = actor_index;
	if (actor[6])
	{
		function_1a6f70(actor_index);
		function_1e3790(actor_index);
		function_262ba0(actor_index);
		function_204ca0(actor_index);
		function_26dde0(actor_index);
		function_266640(actor_index);
		function_267c50(actor_index);
		function_1e36d0(actor_index);
		byte *current = (byte *)actor_get(actor_index);
		memset(current + 0x41c, 0, 0x90);
		*(long *)(current + 0x450) = NONE;
		*(short *)(current + 0x454) = NONE;
		function_258660(actor_index);
		function_1a7cc0(actor_index);
		actor_behavior_current_update44(actor_index);
		function_1e3860(actor_index);
		actor_behavior_current_update48(actor_index);
		if (actor[7])
		{
			if (*(long *)(actor + 0x1c)) swarm_update(actor_index);
		}
		else
		{
			function_25a380(actor_index);
			function_1f3190(actor_index);
			function_1f38c0(actor_index);
			function_298370(actor_index);
			function_1fc7e0(actor_index);
			function_1e4390(actor_index);
		}
	}
	g_46fc80 = NONE;
}

// @retail 0x1e2f50
void function_1e2f50()
{
	byte *globals = (byte *)g_4f55d0;
	*(short *)(globals + 6) = *(short *)(globals + 8);
	globals[3] = false;
	globals[4] = false;
	globals[0xa] = false;
	globals[0x10] = false;
	*(short *)(globals + 8) = 0;
	*(short *)(globals + 0xc) = *(short *)(globals + 0xe);
	*(short *)(globals + 0xe) = 0;
	long last = 0;
	short remaining = 5;
	s_actor_iterator iterator;
	if (globals[1])
	{
		iterator.iterator.data = g_4f55f0;
		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.active_only = true;
	}
	byte *actor;
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL) actor[0x48] = false;
	if (globals[1])
	{
		iterator.iterator.data = g_4f55f0;
		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.active_only = true;
	}
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
	{
		if (remaining <= 0) break;
		if ((short)iterator.actor_index > *(short *)(globals + 0x368)) { actor[0x48] = true; --remaining; }
		last = iterator.actor_index;
	}
	if (remaining > 0)
	{
		if (globals[1])
		{
			iterator.iterator.data = g_4f55f0;
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			iterator.active_only = true;
		}
		while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
		{
			if (remaining <= 0) break;
			if ((short)iterator.actor_index > *(short *)(globals + 0x368)) break;
			last = iterator.actor_index;
			actor[0x48] = true;
			--remaining;
		}
		if (remaining > 0) last = NONE;
	}
	*(short *)(globals + 0x368) = (short)last;
	function_2959c0();
	function_x66da2b(&iterator, true);
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
	{
		if (actor[0xd])
		{
			long index = iterator.actor_index;
			byte *current = (byte *)actor_get(index);
			long unit = *(long *)(current + 0x18), proxy = *(long *)(current + 0x1c);
			function_1e2a90(index);
			if (unit != NONE) function_b8540(unit);
			else if (proxy != NONE) swarm_delete(proxy);
		}
		else if (*(short *)(actor + 0x84) > 0)
		{
			long index = iterator.actor_index;
			g_46fc80 = index;
			if (function_1e35f0(index)) function_1e3b60(index);
		}
	}
	function_x66da2b(&iterator, true);
	while ((actor = (byte *)function_1e46c0(&iterator)) != NULL)
		if (*(short *)(actor + 0x84) > 0) function_1e34e0(iterator.actor_index);
}

struct s_actor_position_state
{
	s_actor_position_state();
	volatile long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real field14, field18;
	long field1c;
	short field20, field22, field24;
	byte unknown26[2];
	short field28, field2a, field2c, field2e;
	long field30, field34;
	real field38;
	short field3c, field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
};
bool function_1e11b0(long actor_index, bool active);
bool function_e58e0(long unit_index);
bool function_109e00(long object_index, vector3f *velocity, bool required);
void swarm_apply_impulse(long perception_index, vector3f const *velocity);

// @retail 0x1e0fe0
long function_1e0fe0(point3f const *point, long unit_index, long character_index, short mode)
{
	byte *object = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
	short const *mode_reference = &mode;
	long squad_index = NONE;
	byte cleanup = false;
	s_actor_position_state placement;
	placement.position = *point;
	placement.field10 = NONE;
	placement.field14 = 0.0f;
	placement.field18 = 0.0f;
	placement.field1c = 2;
	short local_792ecd = *mode_reference;
	placement.field2e = local_792ecd > 0 ? local_792ecd : 3;
	long previous = *(long *)(object + 0x12c);
	if (previous != NONE)
	{
		byte *actor = (byte *)actor_get(previous);
		squad_index = *(long *)(actor + 0x30);
		cleanup = actor[0xc];
	}
	long result = function_1e0850(character_index, squad_index, (s_203780 const *)&placement, false);
	if (result != NONE)
	{
		byte *actor = (byte *)actor_get(result);
		if (*(long *)(actor + 0x30) == NONE)
		{
			*(short *)(actor + 0x2c) = g_510c54->field_2_3;
			actor[0x3c] = true;
			function_1e11b0(result, true);
		}
		actor[0xc] = cleanup;
		if (actor[7] && *(long *)(actor + 0x1c) != NONE)
		{
			dword mask = 1;
			byte *type_object = object;
			mask <<= type_object[0xaa];
			if ((mask & 1) && function_e58e0(unit_index))
			{
				vector3f velocity;
				if (function_109e00(unit_index, &velocity, true))
					swarm_apply_impulse(*(long *)(actor + 0x1c), &velocity);
			}
		}
	}
	return result;
}

// @retail 0x1e2930
void __stdcall function_1e2930(long object_index, long actor_index)
{
	long const *actor_reference = &actor_index;
	s_ai_object *object = ai_object_get(object_index);
	if (*(real *)(g_4e3b44[object->definition_index & 0xffff].bytes + 0xd0) > 0.0f)
		function_25d020(*actor_reference, object_index, 4, NONE, NONE, NONE);
}

void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void __stdcall function_b8540(long object_index);
void __stdcall swarm_delete(long index);
void function_1e2a90(long actor_index);
bool actor_update_bsp_locations(long actor_index);
void function_203ed0(long actor_index, bool keep_count);
void function_203f60(long actor_index);

// @retail 0x1e35f0
bool function_1e35f0(long actor_index)
{
	bool actor_active = actor_get(actor_index)->unknown007;
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;
	if (actor_active && *(long *)((byte *)actor + 0x1c) == NONE)
	{
		long object_index = actor->unknown018;
		long const *other_field = (long *)((byte *)actor + 0x1c);
		volatile long other_index = *other_field;
		function_1e2a90(actor_index);
		if (object_index != NONE)
			function_b8540(object_index);
		else
		{
			long captured_index = other_index;
			if (captured_index != NONE)
				swarm_delete(captured_index);
		}
		result = false;
	}
	else
		*((byte *)actor + 0x506) = 0;
	if (*(short *)((byte *)actor + 0x3e) != g_4686c4 && !actor_update_bsp_locations(actor_index))
	{
		*(long *)((byte *)actor + 0x34) = actor->unknown030;
		function_203ed0(actor_index, false);
		function_203f60(actor_index);
		s_actor_view *current = actor_get(actor_index);
		if (current->unknown009)
		{
			current->unknown009 = false;
			*(long *)((byte *)current + 0x10) = g_510c54->game_time;
			--*(short *)((byte *)g_4f55d0 + 0x36a);
		}
		result = false;
	}
	*((byte *)actor + 6) = result;
	return result;
}

class c_1e2990
{
public:
	__declspec(noinline) void function_1e2990(bool preserve);
};

// @retail 0x1e2990
void c_1e2990::function_1e2990(bool preserve)
{
	s_actor_view *actor = actor_get((long)this);
	long object_index = actor->unknown018;
	long other_index = *(long *)((byte *)actor + 0x1c);
	function_1e2a90((long)this);
	if (object_index != NONE)
	{
		if (preserve)
		{
			function_b83b0(object_index, true);
			function_b8460(object_index, true);
		}
		else
			function_b8540(object_index);
	}
	else if (other_index != NONE)
		swarm_delete(other_index);
}

#include "unknown_0259a0.h"
extern bool g_4f55e5;
extern bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);
void __stdcall function_1e1a00(long actor_index, long value);
long function_1e1f20(long actor_index);
void *function_1e5240(long actor_index);
PRIVATE inline long equipment_current_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = unit->current_weapon;
	return slot != NONE ? unit->weapons[slot] : NONE;
}

PRIVATE inline long equipment_secondary_weapon_r21(long unit_index)
{
	s_ai_object *unit = ai_object_get(unit_index);
	short slot = *((signed char *)unit + 0x213);
	return slot != NONE ? unit->weapons[slot] : NONE;
}

// score 251 (was 251): Frame 0x14 and the actor index in EBX agree; the actor, options and object-pool registers
// rotate (ours actor EDI, options ESI, pool never held; retail actor ESI, options EBP, pool global in EDI) and scalar homes remain.
// Earlier: Frame 0x14 and actor ESI agree; the probability and delay clamps now match
// retail's code; the actor index stays in EDI instead of EBX and the object pool is reloaded instead
// Frame 0x14 and actor ESI agree; input EBX/object-pool EDI allocation and scalar/ammunition homes remain.
// Earlier: probability/delay clamps agree; the actor index stays in EDI instead of EBX and the object pool is reloaded instead
// of kept in a register.
// Tried ammo clear before value 314; two weapon for 314; shared fraction range 308; probability clamp branches 322; delay clamp branches 345; shared range native homes 308.
// Equipment-phase unit handle 308; weapon-pair entry workspace 308; rounds-pair entry workspace 308;
// word two-weapon counter 299 kept (plain); promoted minimum 316.
// Round 115: Probability field reads 288 kept (plain); delay field reads 292; weapon array initializer 299; initial lookup scope 299; increased-probability selection 299; both field clamps 291.
// Round 124: Signed tick result 288; weapon-array cursor 302; empty-weapon continue 288; group handle 290;
// consistent minimum field reads 284 kept plain; with word random or unsigned-word ordinal 284/284;
// with native long ordinal 328; lower-default probability join 288, with minimum reads 284.
// Round 126: Weapon entry/native unit fields/existing random step/nested grenade guard 284 each; optional
// settings initialization 317; word admission minimum 288; magazine memset 282; complete probability
// initialization 309; memset with long ordinal 328 or word minimum 279 (plain kept); combined candidate
// with long ordinal 300 or weapon entry 279 (fast).
// force-inline existing pair of equipment queries 279; native byte current equipment slot 296; native
// signed byte secondary equipment slot 296; both native byte equipment slots 299; both equipment query
// conditional results 270; both equipment query rejection returns 275; existing equipment queries consume
// actual unit record 279; actor record lexical identity on native frame 279.
// Round 224: Native/unsigned-word ordinal 318/270; complete output initialization 271; consistent
// delay-field reads 282; settings default after query 272; signed-word duration 270; branch-owned
// probability 279; native ordinal with delay reads 295. Frame agrees; index/pool and scalar homes
// remain.
// Round 226: Actual weapon/grenade/both metadata 270/270/270; native unit fields 270;
// unit fields with weapon/grenade/both metadata 270/270/270; complete readonly metadata 270.
// Round 206: Shared unit receiver 270; with entry weapon/ammunition/both output lifetimes 270/270/270;
// both output lifetimes 270; shared unit with entry grenade receiver, weapon/grenade or all workspaces 270 each.
// Round 212: Consistent actor getter observations 352; with long ordinal 327; fixed-two for 352;
// long ordinal with for 327; output initializer 351; initializer with long/for/both 327/351/327.
// Round 145: Both ammunition bounds captured, lower/upper/ordinal short-short-short 326, short-
// short-long 339, long-short-short 333, long-short-long 347, short-long-short 332, short-long-long
// 340, long-long-short 334, long-long-long 348; all restored.
// Round 230: Actor after weapon/settings queries 277/308; shared current/secondary/both
// slot queries 329/317/349; both queries with later actor 352/308; direct shared queries 279.
// Uninitialized probability/delay at entry 270/270; constant probability at entry/after query 295/286;
// with the other scalar workspaces 295/286; scalar-role names with early constant 295,
// with uninitialized entry workspaces 270. Original declarations, initializers and names restored.
// Scalar-home names for probability, fraction, delay, ticks, minimum and loop index,
// plus both grouped renames: 270 each.
// Round 351: bounded equipment for, actual pair initializer, early constant ordinal and
// their four combinations 270/270/270/270/270/270/270; complete graph with word ordinal 270. All restored.
// Round 250: Conditional-result query record inputs: current/secondary/both/both const 270/270/270/270;
// both with long slots mutable/const 293/293; both with signed-byte slots mutable/const 268/268 (fast
// control 270). Two-point gain rejected; original scalar-index inputs and short slots restored. Frame
// 0x14 agrees; index/pool roles and scalar/ammunition homes remain.
// Round 22: existing object/header/moving/nested-object accessors 263 each; actor-index
// address read 263; split/reversed entry declarations 263/263; const actor index 263.
// Round 280: shared random-state receiver at entry/after actor/before query/after
// query 340/340/340/333; shared seed-field receiver at those same acquisition
// points 340/340/340/333. Every draw retained; all original receivers restored.
// Round 29: Current/secondary result and actual weapon-pair dword domains in all seven combinations, then the real word ordinal: 263 each. Original result/pair types, ordinal and all code restored.
// Round 111: opening through existing 0x1e5240 (retail inlines it) 263, kept; tail through existing
// element_502420_get 272; current slot through existing unit_get_current_weapon with a result-first
// secondary query 275 (retail's query shape, but ours keeps the unit across 0x1e53e0 where retail
// re-reads it, so the pool global never gets EDI), current slot alone 270; weapons array declared
// first/second 263/263.
// Round 164: nonredundant actor/unit gates, draw-first ordered admission, their seven
// combinations and all with readonly actor 263 each; all original observations restored.
// Ammunition span named at its arithmetic producer, with options/probability/delay result-production combinations 263/310/282/315/263/310/282/315; original expressions and result producers restored.
// Round 249: plain native two-slot shared-query traversal, long/short/word/byte
// ordinal 275/324/324/324; existing unit receiver with those domains 275/324/324/324.
// Original private queries, native object declaration and complete body restored.
// Round 414: current loop weapon captured 263; short/long scalar ammunition count 266/265;
// weapon plus those counts 266/265; private output clear before draw, alone/with weapon/with
// weapon and short count 263/263/266. Original producers, clear order and complete body restored.
// Tried (MiniMax I run 17): 0x1e5240's body (weapon index, record lookup, 0x1e5280) and ai_object_get's pool
// indexing written out by hand 231; hand copies of existing functions are not kept without a match.
// DL210: byte/signed-byte/unsigned-dword ammunition ordinals 263/263/323; paired real-array
// role names 263, with those ordinals 263/263/323; readonly real weapon input pair 263.
// Original ordinal, array names, mutability and complete body restored.
// Round 52 (lane 16): the ammunition minimum read from the options field at each use (no `minimum` local) 263 ->
// 251 plain (kept; the actor index moves to retail's EBX). On 251 (fast): the random draw written inside the rounds
// expression 251; the two slot helpers reading the slot field at each use 251; probability defaulting to 0.1 before
// the options test 288; the weapon pair as an initializer 251; the options lookup before the actor pointer 300; a
// named seed pointer for every random draw (on 263) 333; the counter increment as `+= 1` (on 263) 263 (same code);
// the weapon pair declared at function scope (on 263) 263 (same code). Our callee closure has no stub and nothing
// writes or takes the address of the object-pool global, so retail's held pool register is allocation, not a blocker.
// Lane 9 r52 (fast): retail loads the object-pool global once into EDI above the inlined 0x1e5240's NONE
// test and re-reads its data field at every lookup; ours CSEs the data field and reloads the global (the
// callees in that span are leaves, so it is not a callee summary). `inline` on 0x1e53e0 265 (0x2758b0
// 21 -> 113); the pool read into a local before 0x1e5240's test 263 (same code); NONE-first weapon query
// results 275; the lookups through a plain static getter taking the pool 263 (same code).
// Round 42 (m3 lane 13, fast): retail loads the pool global between the inlined 0x1e5240's NONE compare and its
// branch and holds it in EDI to the end (actor ESI, options EBP, the random state EBX from its first draw); ours
// never carries the global across 0x1e5280 (it reloads it at each lookup, so the actor takes EDI, the options ESI
// and the random state EBP). Diagnostics with the lookups indexed by hand through a pool local (not keepable):
// the local at the top 283 (1237 bytes; pool held in ESI but spilled, options EBP as retail), after the actor 283,
// after the 0x1e5240 call 278 (loaded after 0x1e5280, spilled). A pool local before the NONE test inside 0x1e5240
// (0x1e5240 still MATCH) 251 (same code here: the caller's lookups stay separate loads). The unit-index test split
// from the actor state tests 251 (same code).
// @retail 0x1e2a90
void function_1e2a90(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	byte *weapon_options = (byte *)function_1e5240(actor_index);
	if (actor->unknown084 == 4 && actor->unknown086 >= 3 && actor->unknown018 != NONE)
	{
		byte *unit = (byte *)ai_object_get(actor->unknown018);
		if ((unit[0x134] & 1) && *(signed char *)(unit + 0x1f5) > 0 &&
			(function_cbd50(actor->unknown018, ai_object_get(actor->unknown018)->current_weapon) != NONE ||
			function_cbd50(actor->unknown018, *((signed char *)ai_object_get(actor->unknown018) + 0x213)) != NONE))
		{
			real probability;
			if (weapon_options)
			{
				probability = *(real *)(weapon_options + 0x58) < 0.1f ? 0.1f :
					(*(real *)(weapon_options + 0x58) > 0.6f ? 0.6f : *(real *)(weapon_options + 0x58));
			}
			else
				probability = 0.1f;
			if (actor->unknown225 || (*(short *)((byte *)actor + 0x722) > 0 &&
				*(real *)((byte *)actor + 0x764) < 3.0f))
			{
				real increased = probability * 4.0f;
				if (increased > 0.6f)
					increased = 0.6f;
				probability = probability > increased ? probability : increased;
			}
			if (probability > function_259a0(&g_4e7408->unknown0))
			{
				real delay;
				if (weapon_options && *(real *)(weapon_options + 0x5c) != 0.0f)
				{
					delay = *(real *)(weapon_options + 0x5c) < 0.8f ? 0.8f :
						(*(real *)(weapon_options + 0x5c) > 1.3f ? 1.3f : *(real *)(weapon_options + 0x5c));
				}
				else
					delay = function_259d0(&g_4e7408->unknown0, NULL, 0, 0.8f, 1.3f);
				long ticks = function_1469f0(delay);
				function_c6f80(actor->unknown018, (short)ticks, 0x210000);
				unit[0x1f5] = (byte)ticks;
			}
		}
	}
	if (actor->unknown018 != NONE)
	{
		byte *unit = (byte *)ai_object_get(actor->unknown018);
		byte *grenade_options = (byte *)character_grenade_settings_get(actor->unknown054,
			*(signed char *)(unit + 0x23c));
		long weapons[2];
		weapons[0] = equipment_current_weapon_r21(actor->unknown018);
		weapons[1] = equipment_secondary_weapon_r21(actor->unknown018);
		if (!(g_4e6948->state == 1 && g_4f55e5) &&
			(!*((byte *)g_4f55d0 + 0x340) || (grenade_options &&
			*(real *)(grenade_options + 0x38) > function_x82e52f(&g_4e7408->unknown0, NULL, 0))))
			*(short *)(unit + 0x23e) = 0;
		if (weapon_options)
		{
			short i = 0;
			do
			{
				if (weapons[i] != NONE)
				{
					if (*(real *)(weapon_options + 0x8c) > 0.0f || *(real *)(weapon_options + 0x90) > 0.0f)
					{
						real fraction = function_259d0(&g_4e7408->unknown0, NULL, 0,
							*(real *)(weapon_options + 0x8c), *(real *)(weapon_options + 0x90));
						if (g_4e6948->state == 1 && g_4f55e0)
							fraction *= 0.5f;
						function_101a10(weapons[i], fraction);
					}
					if (*(short *)(weapon_options + 0x94) > 0 || *(short *)(weapon_options + 0x96) > 0)
					{
						dword random = _random(&g_4e7408->unknown0, NULL, 0);
						short rounds[2];
						memset(rounds, 0, sizeof(rounds));
						rounds[0] = (short)((((short)(*(short *)(weapon_options + 0x96) + 1) - *(short *)(weapon_options + 0x94)) * random >> 16) + *(short *)(weapon_options + 0x94));
						if (g_4e6948->state == 1 && g_4f55e0)
							rounds[0] = (short)(rounds[0] * 0.5f);
						function_1018b0(weapons[i], rounds);
					}
				}
			} while (++i < 2);
		}
	}
	if (actor->unknown07c != NONE)
		++*(short *)(g_502420->data + (actor->unknown07c & 0xffff) * 0x50 + 0x3e);
	function_1e1a00(actor_index, 1);
}
