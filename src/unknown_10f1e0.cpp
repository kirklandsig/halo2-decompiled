// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10F1E0.CPP: a unit's animation control: resetting the state at the
   object's offset +0x33e, and starting a unit's base animation, its weapon
   animations and its overlays on the animation state at offset +0x12a
   (unknown_1cafc0.cpp). The neighbouring queries are in unknown_10db60.cpp,
   unknown_10dc70.cpp and unknown_10ee20.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"
#include <string.h>
#include "object_markers.h"
#include <xmmintrin.h>
#include <math.h>
#include "unknown_11cc90.h"
#include "bit_vector.h"
#include "object_tag_definitions.h"

/* the unit (a view of the object data) */
struct s_unit_animation_object
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x10a - 0x18];
	word flag0 : 1;
	word flag1 : 1;
	word animation_frozen : 1;
	word : 13;
	byte unknown10c[0x116 - 0x10c];
	short node_matrices_offset;
	byte unknown118[0x12a - 0x118];
	short animation_state_offset;
	byte unknown12c[0x134 - 0x12c];
	byte unknown134[3];
	byte flags137;
	byte unknown138[4];
	long field_13c;
	byte unknown140[8];
	dword flags148;
	byte unknown14c[0x1f6 - 0x14c];
	char index1f6;
	char index1f7;
	byte unknown1f8[0x33e - 0x1f8];
	short control_offset;
};

struct s_unit_animation_object_header
{
	byte unknown00[8];
	s_unit_animation_object *object;
};

/* a 4 byte state of the control, five of them at +0x80 */
struct s_unit_animation_slot
{
	byte unknown0;
	byte unknown1;
	byte unknown2;
	byte unknown3;
};

/* the state at the unit's offset +0x33e */
struct s_unit_animation_control
{
	union
	{
		word flags;
		struct
		{
			word flag0 : 1;
			word overlay : 1;
			word : 5;
			word flag7 : 1;
			word flag8 : 1;
			word : 7;
		};
	};
	byte unknown02;
	byte unknown03;
	long unknown04[4];
	long unknown14[4];
	byte unknown24[0x28 - 0x24];
	long unknown28;
	byte unknown2c[0x7c - 0x2c];
	short marker_7c;
	short marker_7e;
	s_unit_animation_slot slots[5];
	long weapon_class;
	long weapon_type;
	c_animation_channel channel_9c;
	c_animation_channel channel_bc;
	c_animation_channel channel_dc;
	c_type_709360 animation_fc;
	c_type_709360 animation_100;
	c_type_709360 animation_104;
	c_type_709360 animation_108;
	c_type_709360 animation_10c;
	c_type_709360 animation_110;
	c_type_709360 animation_114;
	c_type_709360 overlays[3];
};

/* the unit's definition (its model at +0x38) and the model's (its animation
   graph at +0x14) */
struct s_unit_animation_definition
{
	byte unknown00[0x38];
	long model_tag_index;
	byte unknown3c[0xbc - 0x3c];
	dword flags;
};

struct s_unit_animation_model
{
	byte unknown00[0x14];
	long graph_tag_index;
};

#define UNIT_ANIMATION_OBJECT(index) (((s_unit_animation_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define UNIT_ANIMATION_STATE(unit) ((s_animation_state *)((byte *)(unit) + (unit)->animation_state_offset))
#define UNIT_ANIMATION_CONTROL(unit) ((s_unit_animation_control *)((byte *)(unit) + (unit)->control_offset))
#define TAG_BYTES(index) (g_4e3b44[(index) & 0xffff].bytes)
#define GRAPH_GET(index) ((s_graph_tag *)TAG_BYTES(index))

bool g_5107f8;
bool g_4686a4 = true;

bool function_0c7070(long object_index);

// score 6 (was 6): the parameter is masked in ECX where retail masks it in EAX; the argument is
// passed in the other register.
// @retail 0x111010
bool function_111010(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = true;
	if (function_0c7070(unit_index))
		result = false;
	else
	{
		switch (UNIT_ANIMATION_STATE(unit)->unknown7c)
		{
		case 0x400004a: case 0x400076c: case 0x400076d:
		case 0x500000a: case 0x5000024: case 0x50000c3:
		case 0x5000768: case 0x5000769: case 0x500076a: case 0x500076b:
		case 0x600005f: case 0x600008c: case 0x60000cc: case 0x60000cd: case 0x60006ac:
		case 0x700000d: case 0x700002c: case 0x7000543: case 0x700076e:
		case 0x80000c4: case 0x800061e: case 0x800076f:
		case 0x900000e: case 0x9000011: case 0x9000012: case 0x900001f: case 0x9000020:
		case 0xa00000f: case 0xa000010: case 0xa000013: case 0xa00002d: case 0xa00003e:
		case 0xa000040: case 0xa00022d: case 0xa0005b9: case 0xa000767:
		case 0xb00002e: case 0xb00022e: case 0xb0005b2:
		case 0xc000073: case 0xc000075: case 0xc000077: case 0xc0006b3: case 0xc0006cd:
		case 0xd000021: case 0xd00002b: case 0xd00022c:
		case 0xe00002a: case 0xe000038: case 0xe00003b: case 0xe0000c2: case 0xe0000c3: case 0xe00067d:
		case 0xf00003a: case 0x1000006c: case 0x11000074: case 0x11000076: case 0x140005b3:
			result = false;
		}
	}
	return result;
}

real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);

PRIVATE __forceinline short duration_ticks_1145f0(real seconds)
{
	long ticks;
	seconds *= g_510c54->field_2_3;
	__asm
	{
		fld seconds
		fistp ticks
	}
	return (short)ticks;
}

// @retail 0x1145f0
short function_1145f0(long definition_index, s_sound_permutation_reference const *reference, short type)
{
	if (definition_index != NONE)
		return duration_ticks_1145f0(sound_permutation_reference_duration(definition_index, reference));
	if (type == 12)
		return duration_ticks_1145f0(0.5f);
	return duration_ticks_1145f0(1.5f);
}

// @retail 0x113260
bool function_113260(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	bool result = false;
	if (!(bool)((definition->flags >> 11) & 1) && state->graph_tag_index != NONE &&
		state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE &&
		g_4686a4 && ((unit->flags148 & 0x8000) || (unit->flags137 & 1)) &&
		(unit->index1f6 != NONE || unit->index1f7 != NONE))
		result = true;
	return result;
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

// @retail 0x112dc0
void __stdcall function_112dc0(long unit_index, transform4x3f *matrix)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	if (control->unknown28 != NONE && function_badc0(control->unknown28, (dword)NONE))
	{
		s_unit_animation_object *parent = UNIT_ANIMATION_OBJECT(control->unknown28);
		transform4x3f *nodes = (transform4x3f *)((byte *)parent + parent->node_matrices_offset);
		function_142a60(nodes, matrix, matrix);
	}
}

/* Partial view of the callback table slot at 0x467a20. */
struct s_unit_matrix_callbacks
{
	void (__stdcall *update)(long, transform4x3f *);
};
s_unit_matrix_callbacks g_467a20 = { function_112dc0 };

void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_141ce0(real yaw, real pitch, real roll, transform4x3f *out);

PRIVATE __forceinline real angle_radians_112e20(real degrees)
{
	return degrees * 0.017453292f;
}

PRIVATE __forceinline real function_112e21(real arg_0)
{
    real local_0;
    __asm
    {
        rsqrtss xmm0, arg_0
        movss local_0, xmm0
    }
    return local_0;
}

// score 64 (was 133): the frame, the offset's arithmetic and the direction's registers (i, j, k
// in xmm3, xmm1, xmm2) now match. Retail clears and stores the direction in i, j, k order (ours
// j, k, i), and its normalisation scales j and k in place where ours copies the factor first
// (3 bytes longer), so every later address is shifted. Tried: relative.i = inverse * relative.i
// with vector3d_from_points3d (133, the compiler commutes it); the offset as a vector3f (71,
// the j row regrouped as (left.j * y + left.i * x) + left.k * z); the direction as three
// scalars (105); the clearing in i, j, k order (91, i, j, k land in xmm1, xmm2, xmm3); the
// normalisation written i = inverse * i, j = j * inverse, k = k * inverse (no change). Tried at
// 133 (lane): shared normalization scale 133, relative scale 136, both scales 136, reciprocal-root
// intrinsic 168, two-angle helper precision probe 119 (rejected).
// @retail 0x112e20
void function_112e20(transform4x3f *matrix, point3f const *target, transform4x3f const *orientation)
{
	static real yaw_limit = angle_radians_112e20(30.0f);
	static real pitch_limit = angle_radians_112e20(15.0f);
	real scale = matrix->scale;
	transform4x3f frame;
	function_1420f0(&frame, &matrix->position, &orientation->forward, &orientation->up);
	vector3f direction;
	if (frame.scale != 0.0f)
	{
		real x = target->x - frame.position.x;
		real y = target->y - frame.position.y;
		real z = target->z - frame.position.z;
		if (frame.scale != 1.0f)
		{
			real inverse = 1.0f / frame.scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		// summed k, j, i as retail does
		direction.i = frame.forward.k * z + frame.forward.j * y + frame.forward.i * x;
		direction.j = frame.left.k * z + frame.left.j * y + frame.left.i * x;
		direction.k = frame.up.k * z + frame.up.j * y + frame.up.i * x;
	}
	else
	{
		// cleared j, k, i: the order that gives retail's registers
		direction.j = 0.0f;
		direction.k = 0.0f;
		direction.i = 0.0f;
	}
	real squared = direction.k * direction.k + direction.j * direction.j + direction.i * direction.i;
	if (squared != 0.0f)
	{
		real inverse;
		inverse = function_112e21(squared);
		direction.i *= inverse;
		direction.j *= inverse;
		direction.k *= inverse;
	}
	s_view_angles angles;
	angles.yaw = (real)atan2(direction.j, direction.i);
	// the planar length stays on the x87 stack: fsqrt and fpatan see it unrounded
	angles.pitch = (real)atan2(direction.k, sqrt(direction.j * direction.j + direction.i * direction.i));
	if (0.0f - yaw_limit > angles.yaw)
		angles.yaw = 0.0f - yaw_limit;
	else if (angles.yaw > yaw_limit)
		angles.yaw = yaw_limit;
	if (0.0f - pitch_limit > angles.pitch)
		angles.pitch = 0.0f - pitch_limit;
	else if (angles.pitch > pitch_limit)
		angles.pitch = pitch_limit;
	transform4x3f rotation;
	function_141ce0(angles.yaw, angles.pitch, 0.0f, &rotation);
	function_142a60(&frame, &rotation, matrix);
	matrix->scale = scale;
}


PRIVATE __forceinline void channel_reset_114240(c_animation_channel *channel)
{
	channel->graph_tag_index = NONE;
	channel->animation_id.graph_index = NONE;
	channel->animation_id.index = NONE;
	channel->frame_position = 0.0f;
	channel->unknown10 = 0;
	channel->unknown11 = 0;
	channel->flags = 0;
	channel->rate = 1.0f;
	channel->unknown14 = 0;
	channel->unknown16 = 0;
	channel->unknown08 = NONE;
	channel->unknown0c = NONE;
	channel->unknown0d = NONE;
	channel->unknown0e = NONE;
}

// @retail 0x114240
void function_114240(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	channel_reset_114240(&control->channel_9c);
	channel_reset_114240(&control->channel_bc);
}

// @retail 0x110fc0
bool function_110fc0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = false;
	switch (UNIT_ANIMATION_CONTROL(unit)->channel_9c.unknown08)
	{
	case 0x9000008:
	case 0x9000009:
	case 0xa000066:
	case 0xf00067f:
		result = true;
	}
	return result;
}

/* the names whose animations mirror each other */
const long g_468684[4][2] =
{
	{ 0xa000014, 0x9000015 },
	{ 0x9000015, 0xa000014 },
	{ 0x9000016, 0xa000017 },
	{ 0xa000017, 0x9000016 },
};

void function_10f040(long object_index);
void unit_animation_channel_blend_reset(long object_index);
void unit_animation_periodic_layer_reset(long object_index);
long render_model_find_named_entry(long render_model_index, long name);
void function_11b710(long unit_index, long field_7c);
s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id);

// @retail 0x111590
bool function_111590(long unit_index, long bit)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = true;
	if (state->channels[0].animation_id.index != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(state->channels[0].graph_tag_index);
		s_animation *animation = function_1daea0(graph, state->channels[0].animation_id);
		if (animation && ((1 << bit) & *(word *)((byte *)animation + 0x18)))
			result = false;
	}
	if (control->channel_9c.animation_id.index != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(control->channel_9c.graph_tag_index);
		s_animation *animation = function_1daea0(graph, control->channel_9c.animation_id);
		if (animation && ((1 << bit) & *(word *)((byte *)animation + 0x18)))
			return false;
	}
	return result;
}
void function_ba350(long object_index, real seconds);

struct s_1d9240;
void function_1d9240(s_1d9240 *slot, char flag, real seconds);
bool function_1d9320(s_1d9240 *slot);

void function_1d90e0(long render_model_index, long node_index, transform4x3f *nodes, transform4x3f const *marker_matrix,
	transform4x3f const *target_matrix, real weight, long node_count);

// score 2 (was 2): only the back edge's two reloads differ (retail reloads the state, then the tag table; ours the
// other way round). The entry's node count and the arms' graph now come from the existing state members (same
// code). The pair's target name is still re-read into its local before the second lookup; round 163's lead below
// replaces that with an inline wrapper (build/r163/113980-wrapper.patch, 2, needs a ruling: a new helper called
// from this function only). The stacked notes below are in the order they were written.
// MiniMax b (11-06): 2 with the pair's target name as a two-use local, re-read from the iterator just before the
// second lookup, and the mask test (bit_vector_test) in nested ifs; not kept: the re-read is a no-op that only
// steers registers (without it 49; without the local, or the local only in the test, 42). On that form the
// permuter tried 40 further bodies with no lower score; a graph-index local (2), the mode and weapon reads above
// the graph lookup (98) and the graph lookup above the two arms (143) do not help.
// Earlier 37 (was 37): frame 0x104 agrees. Retail tests the visited bit with a mask and still ORs
// it into memory after computing the word address; with the shift-form test ours gets that update
// but not the test, and with a mask test (the existing bit_vector_test) the update becomes
// load/OR/store and two call setups change registers (42, 545 bytes). The back-edge reloads of
// state and the tag table run in the other order, and the second lookup and 0x1d90e0 argument
// setups use other scratch registers.
// Round 49: bit_vector_test with/without bit_vector_set 42/42; mask test with 1UL, dword, `x = x |`
// or divide/modulo sets 42-47 (screened); the state's existing pairs method for the alternate arm
// much worse; node count tested inline 33 aligned (worse); do/while and while loops 37; the
// iterator self-reference and the count reference removed 37; the existing bit_vector_set 37
// (kept with the removals).
// Earlier (37-191): branch-local graph; saved mode 74; const model; visited base; iterator-result
// scope; unsigned source/target/handles; model member and external model/unit/definition
// references; shared entry count; entry/all graph queries; typed model; source/visited/lookup
// continues; shared lookup count; class value 89; visited extent; target-object and blend-weight
// lifetimes; shared visited-word address 105; unsigned-int words; class/mode iterator noinline;
// marker query/transform noinline and inline; mask-first assignment 42; Boolean lookup results;
// shared node mask 118; shared word index 109; iterator stores before the graph 99; graph_tag_get
// 42; call results > 0 38; iterator a before b 41; arms swapped 40; markers in the loop 41; visited
// declared first 37; shared exit 37; state pair helper 148; state graph queries 37; mode/graph/
// class orders 44-110; shifted word expression 37; clear before object chain 191; entry marker
// workspaces 37; complete workspace boundaries 191; model after clear 191.
// Round 32 (all 545 bytes unless noted): the existing bit_vector_test 42 (its test is retail's
// exactly; the set after 0x1d90e0 then becomes load/OR/store instead of retail's `or [eax], edx`);
// that with a direct `|=` set 42; a written-out mask test with a direct set 42, with
// bit_vector_set 42; the existing graph_tag_get for the three graph reads 37 (539, back-edge
// reload order unchanged); bit_vector_test with the second lookup nested 42; with the set through
// an explicit word pointer 42. A compile-only build gives the opposite set forms, so screen with
// --fast here.
// Complete unvisited decision with shift/mask tests and bool/byte/short/long iterator results
// 55/42/55/42/83/88/81/86; back-edge reload order and later call carriers remain.
// Round 17: our code is 536 bytes plus 3 of fill. A mask-form test (any spelling) computes the
// update's mask first and splits its OR into load/OR/store, as the matched 0x29df30 does after its
// own mask test, so retail's fused `or [word], mask` after a mask test needs another spelling. Tried:
// bit_vector_test 42; with the update as `visited[i >> 5] |= 1 << (i & 31)` 42; the test written as
// `(word & mask) == 0` 42; the second lookup in a nested if 37 (same code); the shared graph/iterator
// setup hoisted with a ternary call 167 (retail duplicates the setup in both arms).
// Round 81: the two markers as one `s_object_marker markers[2]` (they sit 0x70 apart) 37, with
// bit_vector_test 42; the clear through an inline helper taking the vector, shift/mask test 37/42; the
// existing graph_tag_get with bit_vector_test 42. With the mask test our update reuses the test's
// mask-first shape (load/OR/store); 0x2c39f0 (matched) shows retail giving that shape after a mask test
// too, so the update follows the register state rather than its spelling.
// Round 83: bit_vector_test with the set moved ahead of 0x1d90e0 44 (the update is then index-first but
// still load/OR/store); a block-scoped graph pointer per arm, `found` declared before the loop, `return`
// at the walk's end, and a direct `|=` update with the shift test 37 each.
// Round 85: with the mask test, an opaque call taking the marker or the visited vector right after
// 0x1d90e0 gives retail's index-first `or [word], mask` update (diagnostic, 48); one inside 0x1d90e0 or
// 0xb8d30 on its pointer does not (42). Trials: the set ahead of the 0x1d90e0 call 46, a direct `|=` ahead
// of it 46, the iterator pair as named locals 53, only the target name 50, the pair names with the mask
// test 52.
// Round 235: complete/conditional native pair-admission bool/byte, shift/shared mask
// tests 37/42/37/42/37/42/37/42. The mask still splits the update; original admission
// and complete executable body restored.
// Round 115 (all with the mask test, 545 bytes; the set after 0x1d90e0 then computes the mask before the word
// address and splits into load/OR/store): signed visited words 42; target declared before marker 42; model
// as a `long const *` read as model[1] 42; hand mask test with the mask on the left 42, on the right 42.
// Round 25 (fast): bit_vector_test re-measured 42 (aligned 24); with a long node index local used by
// the test, 0x1d90e0 and the set (nested ifs) 128, the same with the shift test 122, with a short
// local 45; the first marker query's result named 42 (same code); iterator a tested before b 46.
// The set still reuses the test's value numbers across 0x1d90e0. Body restored.
// Round 42 (lane 1): retail's fused set after a mask test needs the visited words to be clobberable
// by the calls between the test and the set: with the mask test, the clear through the existing
// bit_vector_fill gives retail's index-first `or [eax], edx` set, but our LTCG keeps that call out of
// line 213 (466 bytes; also with a forceinline declaration 213); with the entry written as
// `state->node_count() != 0` the fill inlines and the set is load/OR/store again 42 (shift test 37,
// identical code); the visited words and both markers as one local aggregate also give the fused set
// but realign the frame 190 (with the inlined fill 190); 0x1d90e0's marker matrix non-const 42.
// Diagnostics: a volatile node-index read in the set 42, an opaque no-argument call before it 47.
// Round 8 (lane 18, fast): with the mask test, a fresh node index read at the set (volatile diagnostic,
// reverted) still gives the mask-first load/OR/store set, so it is not the index's value number;
// mask test with `mask & word` operands 42; mask test with the set as `mask | word` 42, both swapped
// 42; memset through `&visited` 42; first lookup in its own if around the test and second lookup 42;
// the markers as `markers[2]` with the mask test re-measured 42. 0x120220 (in 0x1d90e0's calls) is
// unmatched: a lead if retail's set needs a call that may write the vector.
// Round 44 (lane 1, fast): 0x120220 is at 2 (not the blocker); bit_vector_test re-measured 42; on the shift
// test: the model as `long const *` read as model[1] 37; the entry tests as guard-clause returns 37; the
// render-model argument named before the call 37; `if (graph && graph->node_count != 0)` for the ternary
// 121 (520 bytes); each arm breaking on its own call result (no `found`) 37.
// Round 118 (lane 5, fast with 0x113ba0/0x113c60): the visited words grouped with both markers in one
// local struct (so the markers' escape covers the vector) with the mask test 190 (frame realigned to 8;
// the set is still mask-first load/OR/store, so the vector's escape does not give retail's fused set and
// 0x120220, which can only reach the markers, is a weak lead); a `break` on each arm's iterator call 37
// (same code); the state parameter renamed to sort before the tag table's name 37 (back-edge reloads
// unchanged, so names do not order them); that per-arm break with the mask test 42; the graph and
// iterator setup shared ahead of an if/else call 167.
// Round 44 (lane 10, fast): bit_vector_test with `found` declared before the loop 42; the unvisited
// test repeated after the second lookup (diagnostic) 74; the memset size as `sizeof(dword) * (...)`
// 37, with bit_vector_test 42; visited declared before the markers with bit_vector_test 42.
// Round 140 (lane 4): with the mask test the set repeats the test's own mask-first sequence before its
// load/OR/store, and the 0x1d90e0 setup forms the model value early (retail last, after the pushes).
// Tried: mask test with the set through pointer arithmetic 42; written-out mask test with a direct `|=`
// set 42 (round 32's form again); visited declared before the markers with bit_vector_test 42; the target
// marker declared first 37 (same code); `if (found) { one combined test } else break;` 37 (same code);
// the iterator's weapon class stored before its mode in both arms 44.
// Round 109 (lane 8, fast; aligned = difflib count): bit_vector_test re-measured 42 (aligned 17 against the shift
// test's 21: the test is retail's; left are the second lookup's argument loads (ours loads the target index into
// ECX ahead of the pushes, retail reuses EAX after them), the 0x1d90e0 setup (ours loads the model value before
// the last push, retail after it) and the set's load/OR/store). `byte const *model` read as `long const` 37, with
// bit_vector_test 42; the two markers in one declaration 37, with bit_vector_test 42; with bit_vector_test, the
// clear's byte count written `sizeof(dword) * (...)` 42 (all same code as their bases).
// Lane 18 round 15 (fast with the matched callees and 0x113ba0/0x113c60): the visited words as a 256-bit
// record cleared by its own member (memset inside), with the mask test 42, with the shift test 37; the set
// through the existing 0xbfe20 on `&visited[i >> 5]` with the shift test 37, with the mask test 42 (inlined,
// mask-first load/OR/store again); visited and both markers as one record cleared by its own member
// (diagnostic for lane 1's escaped aggregate) 42: the frame stays 0x104, but the set is load/OR/store.
// Round 68 (lane 9, fast): with the mask test the split set reloads the word as an array element
// (`mov ecx, [esp + eax*4 + 0x14]` beside `lea eax, [same]`), i.e. it reuses the test's values. Any opaque
// call (through a volatile function-pointer local; diagnostic, reverted) between the test and the set, taking the marker, a const pointer to
// the target or nothing at all, gives retail's fused index-first set (diagnostics 70-78, call included);
// the markers as `markers[2]` with the mask test still split (42). So our LTCG knows the second 0xb8d30
// call and 0x1d90e0 leave the marker alone, and retail's compiler did not; retail's fresh reloads of
// iterator->b after the first query and of the tag table at the back edge fit the same reading. Every
// callee under both calls matches (0x120220 at 2), none is __asm except 0x142a60 (which only sees the
// target), and retail's call graph has no cycle through here. Trials: `found` as a byte 37; the marker
// declared before the definition and visited before the target 37; both with the mask test 42; the
// markers scoped to the iterator-pair block with the mask test 42, with the shift test 37; a 600 s
// permuter run, best 36 (a short render-model read, a byte `found` and declaration moves; not kept).
// Round 127 (lane 5, fast with 0x113ba0/0x113c60): bit_vector_test re-measured 42; the render-model
// index named before the 0x1d90e0 call 37 (same code); both markers declared in the pair block 37 (same
// code), with bit_vector_test 42; the unit lookup folded into the definition read 37 (same code).
// Round 48 (lane 10, fast): the alternate arm as the existing `state->pairs_iterate(iterator)` 148 (kept out
// of line; with a forceinline declaration, diagnostic only, 37 with identical code, so the arms are already
// its inlined shape); `return` for the loop's break 37 and a do/while on `found` 37 (both identical code);
// the clear through the existing bit_vector_fill re-measured with the shift/mask test 216/213 (still out of
// line). With the mask test the set loads the word through the folded address and stores through a
// separate `lea`, so the load is not fused with the store.
// Round 115 (lane 7, fast): the node index named once after the second lookup for 0x1d90e0 and the set 44
// (LTCG keeps it in ESI across the call; retail reloads the field), with the mask test 40 (542 bytes; the set
// stays mask-first load/OR/store, so its shape follows the test's form, not the index's value number); the
// model looked up at the 0x1d90e0 call 122, with the mask test 132; each arm's iterator stores first and the
// graph looked up in the iterator call's argument 110.
// Round 52: retail's inline clear is exactly the body of the existing bit_vector_fill (0x12afc0, out of line
// in retail too), so retail likely called it here and LTCG inlined it, which also escapes the visited words
// (lane 1's cause for retail's fused set). Our LTCG keeps it out of line: with bit_vector_test 213 (fast and
// plain), with an `inline` declaration and the shift test 216, `#pragma inline_depth(255)` diagnostic 213.
// Lead: BLOCKED-BY 0x12afc0 (its file was held this round; try `inline` on its definition, checking 0x1253d0).
// Also `while (true)` 37, explicit `!= 0` pair tests 37, weight against the zero global g_45dbd8 37.
// Round 45 (lane 14, fast): the split set needs both the mask test and the variable-size memset: with the mask
// test, a constant-size memset 164 or no clear 228 (diagnostics) give retail's fused set, while a set on another
// index (target's, diagnostic 46), the second lookup writing the first marker 45 and an out-of-line call on the
// visited words before the set 60 or after the clear 104 keep it split (so it is not value reuse or escape). Shift
// test spellings: `(word & mask) >> shift` 59, `(... & 1) == 0` 37, signed visited words 37, a bool cast 37 (no form
// gives the mask test); the clear's size as `<< 2` 155, with the mask test 145 (stosd only, set still split).
// With the mask test the size taken from the node-count parameter 140 or the graph's count 141 (diagnostics)
// still splits the set: any variable-size memset of the visited words does.
// Round 201 (fast): the clear through bit_vector_fill with a __forceinline declaration here 216 (463 bytes:
// still called out of line), with `inline` on its definition instead (diagnostic, plain check too) 216,
// 0x1253d0 unchanged at 3; each arm's iterator stores before its graph lookup 110 (523 bytes); `found`
// declared before the loop 37 (same code); the mask test written out with bit_vector_set 42 (545 bytes).
// Round 209 (m3 lane 3, fast; masked = difflib count with branch targets masked, baseline 25): with the
// mask test (bit_vector_test, 42, masked 19) everything matches retail except the back-edge reload order
// and the second lookup, where ours loads target_index into ECX ahead of the pushes (retail: EAX after
// them); that one register choice also shifts the 0x1d90e0 setup's scratch registers and gives the
// mask-first load/OR/store set. Lead: the mask test with the pair's target name as a two-use local
// (`long target_name = iterator->b;` for the pair test and the second lookup) 49 at 539 bytes, masked 12:
// the set is then retail's fused `or [eax], edx` and the 0x1d90e0 setup is retail's exactly; left are
// the name held in ESI across the first lookup (retail reads it into EAX for the test and re-reads
// [ebp + 4] before the second lookup, one instruction more) and the back-edge reload order. Tried: the
// existing bit_vector_fill with a prototype identical to its definition 213 (still out of line, 466 B); a
// forceinline copy of it (diagnostic) 42, so an inlined fill does not give the fused set (the 0x12afc0
// lead is closed); with the mask test a byte `found` 42, each arm breaking on its own call 42, the source
// name as a two-use local 48 (both names 51); unused initialised locals at four positions (probes) 42.
// MiniMax C (11-05, held at m3 review): the mask test (the existing bit_vector_test) with the pair's target
// name as a two-use local gives 49 at 539 bytes, retail's length, with retail's fused `or [eax], edx` set and
// its 0x1d90e0 setup; with the second lookup in its own nested if that first re-reads the target name into
// that local (target_name = iterator->b, the same value) 2 at 539 bytes, only the back edge differing
// (retail reloads the state before the tag table); iterator->b passed directly there 46. The same-value
// re-read is a form picked by score, kept only with a byte match: build/held/mmc-1106-113980-reread-local.patch.
// MiniMax A (11-04, run 45): no new form, no lower score (37); a mask test in either signedness still splits the
// visited update (42) and the 0x1d90e0 arguments through locals are the same code (37).
// MiniMax A (11-04, run 41): no new form, no lower score (37); the mask form for the visited-bit test (the
// existing bit_vector_test, and the same set written out inline) matches retail's test but turns the following
// update into load/OR/store and shifts two call setups (42).
// Round 93 (m3 lane 9, fast, all on the held re-read form, 539 bytes): the existing graph_tag_get at the three
// graph reads 2; `found` before the loop 2; `while (true)` 2; markers swapped 2; visited first 2; arms swapped
// 5; the target name declared then assigned 2; guard-clause return 2; direct `|=` set 2; a 37-form screen of
// render-model index named, per-arm graph local, named word count, `found` outside, unit lookup folded into
// the definition read, markers in the pair block and the target name at loop scope (7 singles, 30 mixes) all
// 2; the second lookup with `iterator->b` direct, with or without the test local, 42/42. Diagnostics: the tag
// table as a named local at all five reads gives retail's back-edge order (state first) but spills it to a
// frame slot (80, frame 0x108); state copied into a local 135, an opaque call in the loop 21/82, either or
// both 0xb8d30 calls removed, and 0x142a60 non-naked: order unchanged. In our image the reversed pair (global
// before stack) occurs only here and in 0x14b6a0; retail never has it.
// Round 69 (m3 lane 13, fast, all on the held re-read form at 2): the function moved ahead of every tag-table use in
// the file (diagnostic for the table global's symbol order) 2; the entry graph through graph_tag_get 2; both arms'
// graphs through it 2; a block-scoped graph local per arm 2, with graph_tag_get 2; `found` declared before the loop 2;
// the unit lookup folded into the definition read 2; entry and arm graph_tag_get together 2; a tag-table local at the
// loop top for the two arms' lookups 2; the same local at the walk's entry 147 (frame 0x108); the source name as a
// two-use local 11; a do/while on `found` with the pair block under `if (found)` 2. Every callee under the loop's calls
// is matched C (0x142a60 under 0x1d90e0 is naked asm in our source); the re-read and the back-edge order
// both look like retail's compiler assuming the calls may write the iterator and the table, which our LTCG rules out.
// MiniMax C (11-09): 2 (was 2): only the back edge's two reloads still differ (retail
// reloads the state before the tag table). The state's register copy would have to come before
// the table's load in the entry schedule; reading the graph index into a local ahead of the
// table lookup leaves the order as it is (2), and moving the loop's statements ahead of the
// stores breaks the body (107).
// MiniMax C (11-09): 2 (was 37): the visited-bit test is retail's mask test (the existing
// bit_vector_test) in its own nested if, and the pair's target name is a two-use local that is
// re-read from the iterator just before the second lookup; the visited word's fused OR and both
// lookup setups now match retail; only the back edge's reload order differs (retail reloads the
// state before the tag table).
// Round 163 (m3 lane 5, fast, plain for the kept form): pair names read from the iterator at each use with the
// nested tests and no local 42 (545 bytes: the second lookup loads the target object into ECX ahead of the pushes);
// both lookups through the inline wrapper object_marker_get_first (the query with count 1 and no flag) with the
// names read at each use 2 (539 bytes), so the same-value re-read is gone; on it the three inner tests joined with
// `&&` 2 and all seven in one condition 2 (same code: the mask test needs no nested if of its own); the entry's node
// count through the existing `state->node_count() != 0` 2 (same code: inlined at the test, called for the clear,
// as retail); the arms' graph through the existing `state->graph_get()` 2 (same code), arm-scoped graph locals 2;
// the two member calls alone on the re-read body 2 (fast and plain, 0x113ba0, 0x113c60, 0x1cbe90, 0x1cbce0 MATCH):
// kept, the wrapper form is not (a full check of it: 11833 matched, nothing lost);
// do/while (true) 2; an inline iteration-step function under `if (!step) break;` or `while (step)` 190 (495 bytes:
// kept out of line, called at the top and bottom of the rotated loop), forced inline (diagnostic) 2 with the for
// loop's code; the step written in a for loop's initialiser and increment (diagnostic, forced inline) 218 (602
// bytes: the two copies do not merge); the iterator setup shared ahead of `if (alternate)` 164 (499 bytes).
// Diagnostics for the pair: a neutral `if` with a body right after the break test leaves it (101, own lines); the
// same `if` at the loop's end moves both reloads into its head block in retail's order (122), so the order is not
// a fixed rank of the two values. Retail's order is the order the loop head uses them. A 600 s permuter run on the
// new form: 63 tries, best 2 (its baseline build took 1024 s).
// Round 114 (m3 lane 9, fast = plain 2): the unit and definition lookups folded into the model read 2 (same code); re-measured on this
// base, the target marker declared first 2 and the visited words declared before the markers 2 (same code). In 0x263210 the same kind of
// difference (a preheader reload pair in the other order) went away when a local copy of a pool global with a re-read after a call was
// replaced by the global read at each use; here the matching change, `iterator->b` read at each use with no re-read, is listed at 42/46.
// Levers left outside this body: 0xb8d30's side-effect summary (its file, src/unknown_0b8ca0.cpp, was another lane's this round).
// Round 113 (m3 lane 16, fast): `found == false` 2; each arm breaking on its own call with no `found` 2; the visited words as
// long 2; the graph local declared in the loop body 2; the render model read as `((long const *)model)[1]` 2 (all the same
// code: our back edge still reloads the table before the state). Every callee under the loop (0xb8d30, 0x1dcfa0, 0x1dcf20,
// 0x1d90e0 and theirs) is matched, so the summaries that order the reloads cannot be changed without touching matched code.
// Also: `alternate` copied into a bool local for the loop's test 2; each arm's graph lookup and iterator stores in a
// forceinline helper returning the graph, called inside each arm's call 2.
// Round 80 (m3 lane 15, fast with 0x113ba0, 0x113c60, 0x1cbe90, 0x1cbce0; still only the back edge's pair): both
// pair-iterator functions (0x1dcf20, 0x1dcfa0) declared (iterator, graph), changed at the definitions, declarations and
// all three calls 2 (same code, both still match; restored); a block-scope extern redeclaration of the tag table in
// the body 2 (same code); the graph lookup once ahead of `if (alternate)` with the stores still in each arm 143 (517
// bytes); the graph looked up in each iterator call's argument 107 (526 bytes). Diagnostics for the pair's order (each adds code, measured for the order only): one more use
// of the tag table right after the break test, where EBX no longer holds it (a second load of it into EDX), gives
// retail's order (state first); the same extra use at the loop's top, or at the end of the innermost block, one more
// use of the state at the loop's top, after the calls or before the loop, one fewer table use before the loop, another
// global read after the calls, the arms' iterator stores ahead of the graph lookup, the graph accessor 700 lines
// further down its header, and the file of 0x142a60 built without link-time code generation (one compiler-setting
// diagnostic, restored at once; 0xb8d30 then differs) all leave ours (table first). So the order is neither the
// position of the last use, nor the use counts, nor a callee's side-effect summary: it flips only when the table also
// has to be loaded a second time inside the loop on the every-pass path.
// @retail 0x113980
void __stdcall function_113980(long object_index, long target_index, bool alternate, real weight,
	s_animation_state *state, s_graph_pair_iterator *iterator, long node_count, transform4x3f *nodes)
{
	if (object_index != NONE && target_index != NONE && weight > 0.0f)
	{
		s_graph_tag *graph;
		if (state->node_count() != 0)
		{
			s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(object_index);
			s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
			byte *model = TAG_BYTES(definition->model_tag_index);
			s_object_marker marker;
			s_object_marker target;
			dword visited[8];
			memset(visited, 0, ((state->node_count() + 31) >> 5) * sizeof(dword));
			for (;;)
			{
				bool found;
				if (alternate)
				{
					graph = state->graph_get();
					iterator->mode = state->unknown70;
					iterator->weapon_class = state->unknown74;
					found = function_1dcfa0(graph, iterator);
				}
				else
				{
					graph = state->graph_get();
					iterator->mode = state->unknown70;
					iterator->weapon_class = state->unknown74;
					found = function_1dcf20(graph, iterator);
				}
				if (!found)
					break;
				long target_name = iterator->b;
				if (target_name != NONE && target_name && iterator->a != NONE && iterator->a)
				{
					if (function_b8d30(object_index, iterator->a, &marker, 1, false))
					{
						if (!bit_vector_test(visited, marker.node_index))
						{
							target_name = iterator->b;
							if (function_b8d30(target_index, target_name, &target, 1, false))
							{
								function_1d90e0(*(long *)(model + 4), marker.node_index, nodes, &marker.node_matrix,
									&target.matrix, weight, node_count);
								bit_vector_set(visited, marker.node_index, true);
							}
						}
					}
				}
			}
		}
	}
}
bool unit_animation_state_finished(s_animation_state *state);
bool function_10f630(long object_index, long *first, long *second);
long function_10f720(long object_index, bool first);

// @retail 0x113e40
bool function_113e40(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	return state && state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && !(state->flags & 1) && !unit_animation_state_finished(state);
}

// @retail 0x114040
bool function_114040(long unit_index, long name)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = false;
	if (control->channel_9c.unknown08 == name)
		result = true;
	else if (control->channel_bc.unknown08 == name)
		result = true;
	else
	{
		long second, first;
		if (function_10f630(unit_index, &first, &second))
		{
			if (second == name)
				result = true;
		}
	}
	return result;
}

// @retail 0x1143d0
bool function_1143d0(long unit_index)
{
	bool result = true;
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		s_animation_state *state = UNIT_ANIMATION_STATE(unit);
		long name = state->unknown7c;
		if (state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE &&
			!(state->channels[0].unknown11 & 0xa))
		{
			if (name == 0x700005c)
			{
				if (function_10f720(unit_index, true) != 2)
					result = false;
			}
			else if (name == 0x700005d)
			{
				if (function_10f720(unit_index, true) != 1)
					result = false;
			}
		}
	}
	return result;
}

// @retail 0x113910
bool function_113910(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = function_1d9320((s_1d9240 *)&control->slots[0]);
	result |= function_1d9320((s_1d9240 *)&control->slots[1]);
	result |= function_1d9320((s_1d9240 *)&control->slots[2]);
	result |= function_1d9320((s_1d9240 *)&control->slots[3]);
	result |= function_1d9320((s_1d9240 *)&control->slots[4]);
	return result;
}

// @retail 0x113d20
void function_113d20(long unit_index, real seconds)
{
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		function_1d9240((s_1d9240 *)&UNIT_ANIMATION_CONTROL(unit)->slots[2], false, seconds);
	}
}

// @retail 0x113d60
void function_113d60(long unit_index, real seconds)
{
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		function_1d9240((s_1d9240 *)&UNIT_ANIMATION_CONTROL(unit)->slots[2], true, seconds);
	}
}

/* whether the slot is counting, up (bit 0 of its flags) or down; bit 1 is set
   once it has finished */
PRIVATE __forceinline bool animation_slot_running(s_unit_animation_slot const *slot, bool up)
{
	return slot->unknown1 && (slot->unknown3 & 1) == up;
}

// @retail 0x113da0
long function_113da0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	if (!control->slots[2].unknown1 ||
		(animation_slot_running(&control->slots[2], false) && (control->slots[2].unknown3 & 2)))
		return 1;
	return 0;
}

// @retail 0x113df0
long function_113df0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	if (animation_slot_running(&control->slots[2], true) && (control->slots[2].unknown3 & 2))
		return 1;
	return 0;
}

static __forceinline long mirrored_name_get(long name)
{
	for (long i = 0; i < 4; i++)
	{
		if (name == g_468684[i][0])
			return g_468684[i][1];
	}
	return NONE;
}

static inline bool name_is_mirrored(long name)
{
	return name == 0xa000014 || name == 0x9000015 || name == 0x9000016 || name == 0xa000017;
}

static inline void quad_clear(long *quad)
{
	quad[0] = 0;
	quad[1] = 0;
	quad[2] = 0;
	quad[3] = 0;
}

static inline bool channel_refresh_if_valid(s_animation_state *state, c_animation_channel *channel, long weapon_class,
	long weapon_type)
{
	bool result = false;

	if (state->graph_tag_index != NONE)
		result = state->channel_refresh(channel, weapon_class, weapon_type);
	return result;
}

static inline void slot_reset(s_unit_animation_slot *slot, byte value)
{
	slot->unknown0 = 0;
	slot->unknown1 = 0;
	slot->unknown2 = value;
	slot->unknown3 = 0;
}

PRIVATE __forceinline void function_113871(s_unit_animation_slot *arg_0)
{
	arg_0->unknown1 = 0;
	arg_0->unknown0 = 0;
	arg_0->unknown3 = 0;
}

// @retail 0x113870
void function_113870(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	function_113871(&control->slots[0]);
	function_113871(&control->slots[1]);
	function_113871(&control->slots[2]);
	function_113871(&control->slots[3]);
	function_113871(&control->slots[4]);
	control->slots[0].unknown2 = 1;
	control->slots[1].unknown2 = 1;
	control->slots[2].unknown2 = 2;
	control->slots[3].unknown2 = 1;
	control->slots[4].unknown2 = 1;
}


#include "channel_start_out_of_line.h"

// @retail 0x114330
void function_114330(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	long weapon[2] = { state->unknown74, state->unknown78 };

	if (state->graph_tag_index != NONE)
	{
		c_type_709360 animation_id = GRAPH_GET(state->graph_tag_index)->overlay_get(state->unknown70, weapon[0],
			weapon[1], 0x10000551, NULL, NULL, NULL);

		if (animation_id.index != NONE &&
			channel_start_out_of_line(state, &control->channel_dc, animation_id, 0x10000551, NONE, NONE, 1, 0x803f))
		{
			control->flag7 = false;
		}
	}
}

// @retail 0x113410
void function_113410(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);

	if (state)
	{
		long first_set = NONE;
		long second_set = NONE;
		long weapon_class = state->unknown74;
		long weapon_type = state->unknown78;
		long other_weapon_type = control->weapon_type;

		switch (state->unknown7c)
		{
		case 0x400000c:
		case 0x700005c:
		case 0x700005d:
		case 0x800001e:
		case 0x900000e:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xb000059:
		case 0xb00005a:
		case 0xc000058:
		case 0xc00005b:
			first_set = 0xc000026;
			second_set = 0xe000028;
			break;
		case 0x400004a:
		case 0x5000049:
			if (unit->parent_index == NONE)
			{
				first_set = 0xc000026;
				second_set = 0xe000028;
			}
			break;
		case 0x9000015:
		case 0x9000016:
		case 0xa000014:
		case 0xa000017:
		case 0xa000019:
		case 0xa00001a:
		case 0xa00002d:
		case 0xb000018:
		case 0xb00001b:
		case 0xb00002e:
		case 0xc000033:
		case 0xc000034:
		case 0xd000032:
		case 0xd000035:
			first_set = 0xb000027;
			second_set = 0xd000029;
			break;
		}

		control->animation_fc = state->overlay_or_animation_get(weapon_class, first_set, weapon_type);
		control->animation_100 = state->overlay_or_animation_get(weapon_class, second_set, weapon_type);
		control->animation_114 = state->overlay_or_animation_get(weapon_class, 0x400004b, weapon_type);
		if (weapon_class == 0x400054b)
		{
			control->animation_104 = state->overlay_or_animation_get(weapon_class, 0xb00054a, weapon_type);
			control->animation_108 = state->overlay_or_animation_get(weapon_class, 0xb00054a,
				other_weapon_type);
			control->animation_10c = state->overlay_or_animation_get(weapon_class, 0x400060b, weapon_type);
			control->animation_110 = state->overlay_or_animation_get(weapon_class, 0x400060b,
				other_weapon_type);
		}
		else
		{
			control->animation_104.graph_index = NONE;
			control->animation_104.index = NONE;
			control->animation_108.graph_index = NONE;
			control->animation_108.index = NONE;
			control->animation_10c = state->overlay_or_animation_get(weapon_class, 0x400060b, weapon_type);
			control->animation_110 = state->overlay_or_animation_get(weapon_class, 0x400060b, weapon_type);
		}
		function_114330(unit_index);

		bool any_overlay = false;
		for (long i = 0; i < 3; i++)
		{
			control->overlays[i] = state->overlay_kind_get(i);
			if (control->overlays[i].index != NONE)
				any_overlay = true;
		}
		if (any_overlay)
			control->overlay = true;
		else
			control->overlay = false;
	}
}

// @retail 0x113740
void function_113740(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);

	state->channel_refresh(&control->channel_9c, 0x7000101, 0x7000101);
	state->channel_refresh(&control->channel_dc, 0x7000101, 0x7000101);
	state->channel_refresh(&control->channel_bc, 0x400054b, control->weapon_type);
}

// @retail 0x1137b0
real function_1137b0(bool other, real minimum, bool changed, long set, long current_set)
{
	real result = 0.267f;

	if (set == 0x9000020 || set == 0x900001f || set == 0x90006b2)
		result = 0.1335f;
	if (changed || other)
		result = 0.267f;
	if (current_set == 0x400000c)
	{
		if (name_is_mirrored(set))
			result = 0.267f;
	}
	else if (name_is_mirrored(current_set) && (name_is_mirrored(set) || set == 0x400000c))
	{
		result = 0.267f;
	}
	return result > minimum ? result : minimum;
}

// @retail 0x10f260
void __stdcall function_10f260(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);

	function_10f040(unit_index);
	control->unknown28 = NONE;
	control->flags = 0;
	memset(control->unknown04, 0, sizeof(control->unknown04));
	memset(control->unknown14, 0, sizeof(control->unknown14));
	control->unknown02 = 0;
	control->unknown03 = 0;

	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	control->marker_7c = (short)render_model_find_named_entry(definition->model_tag_index, 0x80001a2);
	control->marker_7e = (short)render_model_find_named_entry(definition->model_tag_index, 0x90001a3);

	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state)
	{
		state->unknown74 = NONE;
		state->unknown78 = NONE;
	}
	unit_animation_channel_blend_reset(unit_index);
	unit_animation_periodic_layer_reset(unit_index);
	function_113870(unit_index);
	control->weapon_class = NONE;
	control->weapon_type = NONE;
}

// @retail 0x10f1e0
void function_10f1e0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	long graph_tag_index = ((s_unit_animation_model *)TAG_BYTES(definition->model_tag_index))->graph_tag_index;

	UNIT_ANIMATION_CONTROL(unit)->unknown03 = 0;
	if (state->graph_tag_index != graph_tag_index)
	{
		state->initialize(graph_tag_index, definition->model_tag_index, true);
		state->unknown74 = NONE;
		state->unknown78 = NONE;
		function_10f260(unit_index);
	}
}

// @retail 0x10f430
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend,
	bool force, long flags)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	long current_set = state->unknown7c;
	long old_mode = state->unknown70;
	bool result;

	if (current_set != NONE)
	{
		if (current_set == set || set == 0x7000101)
			flags |= 0x20;
		else if (current_set == mirrored_name_get(set))
			flags |= 0x40;
	}
	if (unit->field_13c != NONE && !g_5107f8)
		flags |= 0x200;
	if (mode == 0x7000101 && set == 0x7000101)
		result = true;
	else
		result = state->animation_set(mode, weapon_class, weapon_type, set, flags, 0x3f);
	if (result)
	{
		bool mode_changed = old_mode != mode && mode != 0x7000101 || force;
		bool set_changed = current_set != set && set != 0x7000101 || force;

		if (mode_changed)
		{
			function_113740(unit_index);
			if (state->overlay_exists())
				control->overlay = true;
			else
				control->overlay = false;
		}
		if (mode_changed || set_changed)
		{
			real seconds = function_1137b0(false, blend, mode_changed, set, current_set);

			if (seconds > 0.0f && !(bool)(((dword)(short)state->flags >> 4) & 1))
				function_ba350(unit_index, seconds);
			function_113410(unit_index);
		}
	}
	return result;
}

// @retail 0x1140b0
bool function_1140b0(long unit_index, long name)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = false;
	if (control->channel_9c.unknown08 == name)
	{
		control->channel_9c.clear();
		function_ba350(unit_index, 0.267f);
		result = true;
	}
	else if (control->channel_bc.unknown08 == name)
	{
		control->channel_bc.clear();
		function_ba350(unit_index, 0.267f);
		result = true;
	}
	else
	{
		long first, second;
		if (function_10f630(unit_index, &first, &second) && second == name)
		{
			function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x7000001, 0.0f, false, 0);
			result = true;
		}
	}
	return result;
}

/* kept out of line: retail calls it from every caller */
__declspec(noinline) bool __stdcall function_113e90(long unit_index, long name, real blend, c_animation_channel **output, long mode);

// @retail 0x113e90
bool __stdcall function_113e90(long unit_index, long name, real blend, c_animation_channel **output, long mode)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	c_animation_channel *channel = mode == 3 ? &control->channel_bc : &control->channel_9c;
	long weapon_type;
	long weapon_class = 0x7000101;
	weapon_type = 0x7000101;
	if (mode == 1 || mode == 3)
	{
		weapon_class = 0x400054b;
		weapon_type = control->weapon_type;
	}
	else if (mode == 2 && state->unknown74 == 0x400054b)
	{
		weapon_class = control->weapon_class;
		weapon_type = 0x7000101;
	}
	bool can_blend = true;
	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(channel->graph_tag_index);
		s_animation *animation = function_1daea0(graph, channel->animation_id);
		can_blend = !TEST_FIELD_BIT(animation->flag1);
	}
	if (state->overlay_play(channel, 0x803f, name, weapon_class, weapon_type))
	{
		if (blend > 0.0f && can_blend && channel->is_unflagged0())
			function_ba350(unit_index, blend);
		if (output)
			*output = channel;
		return true;
	}
	long first, second;
	if (function_10f630(unit_index, &first, &second) && second == name)
	{
		if (output)
			*output = &UNIT_ANIMATION_STATE(unit)->channels[0];
		return true;
	}
	if (function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, name, blend, false, 0))
	{
		if (output)
			*output = &UNIT_ANIMATION_STATE(unit)->channels[0];
		return true;
	}
	return false;
}



// @retail 0x10f5c0
bool function_10f5c0(long unit_index, real blend, long flags, long mode, long set)
{
	return function_10f430(unit_index, mode, 0x7000101, 0x7000101, set, blend, false, flags);
}

// @retail 0x10fd40
bool function_10fd40(long unit_index, long weapon_type, long weapon_class, bool flag)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long old_weapon_class = state->unknown74;
	long old_weapon_type = state->unknown78;
	bool take_other = !flag && weapon_type != 0x7000001;
	bool drop_other = !flag && weapon_type == 0x7000001;

	if (TEST_FIELD_BIT(unit->animation_frozen))
		return true;

	long type = weapon_type;
	if (take_other)
	{
		weapon_class = 0x400054b;
		type = 0x7000101;
	}
	else if (drop_other)
	{
		weapon_class = control->weapon_class;
		type = 0x7000101;
	}

	bool result = state->animation_set(0x7000101, weapon_class, type, 0x7000101, 0x26, 0x3f);
	if (result)
	{
		bool class_changed = old_weapon_class != weapon_class && weapon_class != 0x7000101;
		bool type_changed = old_weapon_type != type && type != 0x7000101;

		if (class_changed || type_changed)
		{
			real seconds = function_1137b0(true, 0.0f, false, state->unknown7c, state->unknown7c);

			channel_refresh_if_valid(state, &control->channel_9c, 0x7000101, 0x7000101);
			channel_refresh_if_valid(state, &control->channel_dc, 0x7000101, 0x7000101);
			if (seconds > 0.0f && !(bool)(((dword)(short)state->flags >> 4) & 1))
				function_ba350(unit_index, seconds);
			if (take_other)
			{
				control->weapon_class = old_weapon_class;
				control->weapon_type = weapon_type;
				control->channel_bc.clear();
				result = channel_refresh_if_valid(state, &control->channel_bc, 0x400054b, control->weapon_type);
			}
			else if (drop_other)
			{
				control->channel_bc.clear();
				control->weapon_type = NONE;
				control->weapon_class = NONE;
			}
			function_113410(unit_index);
		}
	}
	return result;
}

// @retail 0x1101e0
bool function_1101e0(long animation_graph_index, long unit_index, long animation_name, bool flag, bool global_flag)
{
	bool result = false;

	if (UNIT_ANIMATION_OBJECT(unit_index)->animation_state_offset != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(animation_graph_index);

		if (graph)
		{
			c_type_709360 first_id = graph->animation_by_name(animation_name);
			if (first_id.index == NONE)
				goto done_1101e0;

			s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
			s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
			s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
			s_animation_state *state = UNIT_ANIMATION_STATE(unit);

			function_11b710(unit_index, 0x7000101);
			if (state->graph_tag_index != animation_graph_index &&
				!state->initialize(animation_graph_index, definition->model_tag_index, true))
			{
				goto done_1101e0;
			}

			c_type_709360 animation_id = GRAPH_GET(state->graph_tag_index)->animation_by_name(animation_name);
			function_10f260(unit_index);

			dword channel_flags = flag ? 0x4003 : 0x4001;
			s_graph_tag *state_graph = GRAPH_GET(state->graph_tag_index);
			if (function_1daea0(state_graph, animation_id)->type)
			{
				control->channel_9c.clear();
				result = state->channel_play(&control->channel_9c, animation_id, channel_flags);
			}
			else
			{
				result = state->play(animation_id, channel_flags);
				if (result)
				{
					if (!global_flag)
						control->flag8 = true;
					else
						control->flag8 = false;
				}
			}
			function_114330(unit_index);
		}
	}
done_1101e0:
	return result;
}

real function_1d9370(s_1d9240 const *slot);

// @retail 0x113ba0
void function_113ba0(long unit_index, long target_index, long node_count, transform4x3f *nodes)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && target_index != NONE && control->slots[0].unknown1)
	{
		real weight = function_1d9370((s_1d9240 *)&control->slots[0]);
		if (!(fabs(weight) < 0.0001f))
		{
			s_graph_pair_iterator iterator;
			iterator.a = NONE;
			iterator.b = NONE;
			iterator.index = NONE;
			iterator.step = 0;
			iterator.mode = NONE;
			iterator.weapon_class = NONE;
			function_113980(unit_index, target_index, true, weight, state, &iterator, node_count, nodes);
		}
	}
}

// @retail 0x113c60
void function_113c60(long unit_index, long target_index, long node_count, transform4x3f *nodes)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && target_index != NONE && control->slots[1].unknown1)
	{
		real weight = function_1d9370((s_1d9240 *)&control->slots[1]);
		if (!(fabs(weight) < 0.0001f))
		{
			s_graph_pair_iterator iterator;
			iterator.a = NONE;
			iterator.b = NONE;
			iterator.index = NONE;
			iterator.step = 0;
			iterator.mode = NONE;
			iterator.weapon_class = NONE;
			function_113980(unit_index, target_index, false, weight, state, &iterator, node_count, nodes);
		}
	}
}

// @retail 0x111490
bool function_111490(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = false;
	if (unit->parent_index != NONE && *(short *)((byte *)unit + 0x1fc) != NONE)
	{
		result = true;
		if (function_0c7070(unit_index))
			result = false;
		else
		{
			switch (UNIT_ANIMATION_STATE(unit)->unknown7c)
			{
			case 0x400004a: case 0x400076c: case 0x400076d:
			case 0x50000c3: case 0x5000768: case 0x5000769: case 0x500076a: case 0x500076b:
			case 0x600008c: case 0x700076e: case 0x80000c4: case 0x800076f:
			case 0xa000767: case 0xc000043: case 0xd00002b: case 0xd000042:
			case 0xe00002a: case 0xe00003b: case 0xe0000c3: case 0xf00003a:
				result = false;
			}
		}
		if (!function_111590(unit_index, 2))
			result = false;
	}
	return result;
}


// score 1 (was 1): the argument is copied into EAX where retail passes it in ECX, because the
// callee at 0xc7070 keeps its parameter in ECX. Split pointer declarations 1; read-only pointees 1;
// const pointer homes 1; an argument-address read 1; unsigned state selector 5;
// default-first channel/state switches 1 each; scoped favor-speed-off 114.
// @retail 0x1112f0
bool function_1112f0(long unit_index)
{
	bool result = true;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	switch (control->channel_9c.unknown08)
	{
	case NONE: case 0x500000a: case 0x6000006: case 0x6000007:
	case 0x9000004: case 0x9000005: case 0x9000008: case 0x9000009:
	case 0xa000040: case 0xa0005b9: case 0xa0005bb: case 0xb0005b2:
	case 0xc000073: case 0xc000075: case 0xc000077: case 0xc0006b3: case 0xc0006cd:
	case 0xe000038: case 0xe000607: case 0xe000608: case 0xe000609: case 0xe00060a: case 0xe00067d:
	case 0x11000074: case 0x11000076: case 0x130005bc: case 0x140005b3:
		break;
	default:
		result = false;
		break;
	}
	if (function_0c7070(unit_index))
		result = false;
	else
	{
		switch (state->unknown7c)
		{
		case 0x400004a: case 0x600008c: case 0xa00003e: case 0xb000059: case 0xb00005a:
		case 0xc000043: case 0xc000058: case 0xc00005b: case 0xd00003f: case 0xd000042:
		case 0xe00003b: case 0xe0000c2: case 0xf00003a: case 0x1000003c:
			result = false;
		}
	}
	if (!function_111590(unit_index, 3))
		result = false;
	return result;
}


bool function_c70b0(long unit_index);
long function_cbd50(long unit_index, short weapon_index);

// MiniMax B (11-12): no new form, no lower score (164); the permuter (57 tries, run with the 0xcbd50 context
// because retail inlines that callee) finds nothing better than the baseline, and retail keeps the pool data
// pointer in EBP, the unit index in ECX and the depleted flag in BL where ours keeps the data pointer in EDI
// and pushes ESI and EDI.
// MiniMax H (11-13): no new form, no lower score (164); retail holds index times twelve in EDX and adds it to the pool data in EBP, while this build folds the index into the address expression, and the two stack slots are a dword apart.
// score 164 (was 164):
// MiniMax B (11-11): no new form, no lower score (164); the body is unchanged, and the forms listed above are exhausted.
// MiniMax H (11-11): no new form, no lower score (164); retail keeps index*12 as a value and re-reads the unit and
// the pool's data for the second depletion query, and writes the first depletion with setle; the permuter (600 s) found nothing.
// retail keeps index*12 as a value, re-reads the unit (and the pool's data for the second query)
// for both depletion queries, writes the first depletion with setle and returns the result byte unnormalised.
// MiniMax S (11-09): no new form, no lower score (164); retail keeps index*12 as a value and re-reads
// the unit through that offset for both depletion queries, and writes the first depletion with setle.
// MiniMax H (11-06): the index masked in a local of its own gives 322, the unit pointer dropped in favour of
// the macro at every use gives 344 and the volatile cast on the argument read dropped gives 271; the slot
// argument taken from the unit local gives 164, the same code as retail's re-derived read.
// MiniMax H (11-06): the pool pointer, the header array and index*12 as a byte offset in locals reproduce
// retail's entry shape (headers in EBP, index*12 in EDX, the pool re-read for the second query) but the index
// still lands in EAX and the pool in EDX, 377; the score stays 164.
// MiniMax R (11-04): the case list with 0xb000061 (170), the split depletion queries (330) and a bool
// result (194) all lose against 164; splitting only the first query (328) and the retail-shaped second
// query that clears the flag on a positive value (330) lose too, though the latter reaches setle; no
// new form, no lower score.
// MiniMax R (10-31): no new form, no lower score.
// MiniMax I (11-08): no new form, no lower score (164): retail keeps the pool pointer in EAX and its data
// in EBP, re-reads the unit pointer for both depletion queries and writes the first depletion with setle;
// ours keeps the unit in EAX and builds the result with a branch.
// this round found no gain - retail keeps index*12 as a value (lea/shl) and re-reads
// Earlier (164, was 164): this round found no gain - retail keeps index*12 as a value (lea/shl) and re-reads
// the unit through that offset for both depletion queries, and it writes the first depletion with setle;
// the byte-offset entry with re-derived queries gives 325 and the if-form depletion gives 330.
// this round found no gain - retail keeps the pool pointer in EAX and the data in
// EBP, materialises index*12 and rematerialises the unit pointer for the first query, while ours keeps
// the unit in EAX, folds index*3 with a scale of 4 and builds the result with a branch instead of setle;
// the byte-offset header, a pool local, re-read queries and retail-shaped flag joins all score the same or worse.
// frame eight agrees; entry pool/state registers, signed-count joins and result
// Round 141: retail returns the result byte unnormalised (a bool) and keeps the depletion in BL with the
// interruptibility in memory; round 75's first-sets/second-clears depletion 330, with a bool result
// 361 (1243 bytes, aligned 132 -> 102: retail's setle and second test, but depleted in memory and
// interruptible in BL, no unit re-read), with interruptible/depleted/result declaration orders 362/363;
// controlling read first 164; name before overlay 164.
// normalization differ. Retail keeps the pool global in EAX and its data in EBP from entry, the state
// in EDI, re-reads the unit pointer for both held-weapon queries (the second through a fresh data load)
// and keeps the depletion's initial false in a stack byte (+0x12) beside the result (+0x11) and the
// interruptibility (+0x13); 0x1103a0 shows the same pattern.
// Tried: direct parameter plus word/native interruptibility 376/378, second depletion 337 (plain:
// thirteen caller matches lost), native result 382, held helper inline 358; second predicate 194; native
// interruptible 341; status names 347; size policy 602; overlay-first exits 415; conditional first 336,
// with native interruptible 335; original-unit slot reads 194; early interruptibility 316; guarded second
// count 338; byte result 164; unsigned identifiers 218; byte interruptible 164; signed-byte depletion 164;
// state-first receiver 164; direct interruptibility 348; query result first 332; signed-byte result 322;
// short result 355; entry held outputs 322; state-switch default 370; branch-local status defaults 253;
// entry byte result 164; valid first count 328; guarded second clear 328; state identifier before
// overlay 164; shared held query at both/first/second sites 164 each; signed-byte/native helper slot
// 169/327; helper result selection 363, with byte/native slot 357/348; native first/second/both slots
// 164 each; native first/second/both counts 164 each; combined native fields 164; existing
// channel-state member 164; complete first/second/both depletion selections 314/357/357; rejection
// predicates first/second/both 164 each; canonical true return 205, with selections 355; explicit name
// default/definite result 354/354; native/unsigned-word/signed-word result 381/377/377; separate overlay
// byte 183; natural overlay exits 302; definite result plus overlay exits 475; first/second/both query
// scopes 164 each; state/channel/both identifier lexical homes 164 each, with first/both scopes 164/164;
// on the success-byte graph short/long depletion 200/173, short/long interruptibility 304/172, paired
// 309/201/304/174; complementary depletion bool/byte/short/long 335/335/346/345; complementary
// interruptibility bool/byte/short/long 240/240/245/245; direct native control/state/unit receivers
// 164/165/344, control+state 165, unit+control 344, unit+state 367, all 367, plus direct controlling
// field 367. Round 75: the existing 0xcbd50 in place of a hand-written copy 164 (identical code; kept,
// also in 0x1103a0); depletion as `if (first != NONE) depleted = ...; if (second != NONE && count > 0)
// depleted = false;` 330 (frame four), with a byte depleted 330; without the parameter's volatile
// re-read at the 0xc70b0 call 322; depletion through an inline helper writing a pointer 330.
// Round 80: bool result 194 (head unchanged; the exits lose the normalisation but still lack retail's
// ebp pop); second query as `if (second != NONE && count > 0) depleted = false;` 345 (all four registers
// at entry, but frame four, depletion in memory, interruptibility in BL, unit kept live); with the
// overlay read at its switch 331; with cached-unit slot arguments 345; with short slot locals 345.
// Retail's unit reload at each query (`mov ecx, ebp; mov ecx, [ecx + edx + 8]`) occurs in only one
// other retail function (0x100130, unmatched). Original body restored.
// Round 162: pool/data receivers 378/345; shared word unit slot 368; pool/data plus word
// slot 400/368; data plus dword slot 350; pool plus long slot 390; data with native result
// 329. Original lookup observations, status graph and argument handling restored.
// Round 402: native unsigned first/second/controlling handle domains, seven subsets,
// all 164; all handles with both unsigned identifier domains 188. Original signed
// domains and body restored; pool/state allocation, count joins and byte normalization remain.
// Round 81: an object-headers local (the pool's data) for the unit and both depletion checks with a
// bool result 334 (1273 bytes; headers EBP, state EDI and name ESI as in retail, aligned 86 -> 58, but
// retail keeps the pool pointer in EAX for a fresh data load in the second query, which shifts the entry
// one instruction), with a byte result 339, with the flag set through an inline pointer helper 334, with
// a ternary depletion and both slot reads through the headers 340, with both slot reads through the unit
// 329. Retail re-reads the unit in both queries and the data in the second: loads are not reused there.
// Round 83: one depletion helper for 0x110ab0 and 0x1103a0, static inline (kept out of line) 419, forceinline
// 334 (frame four); the depletion flag renamed to sort between the other two 164; interruptible declared
// before depleted 165; result declared after the two flags 165.
// Round 85: `unit_index = *unit_reference;` at entry (0x1130f0's form) with a plain 0xc70b0 argument
// 271; an object-headers local with if-form depletion and bool/byte result 336/325, with a byte depletion
// 336; the headers local with the `&&` depletion, bool result and byte depletion 334.
// Round 210: Boolean success with depletion/interruptibility bool-short, bool-long, short-bool,
// short-short, short-long, long-bool, long-short and long-long 315/187/209/314/208/189/315/188.
// Native pairs reserve frame twelve and 1269 bytes; entry pool/state allocation and count joins
// remain. Original byte success, canonical statuses, signed counts, queries and argument handling
// restored.
// Round 243: Complete success/depletion/interruptibility status enums, subsets
// 1..7 381/173/386/172/403/174/384; all with entry success constant 384. Original byte
// result, Boolean statuses, queries, parameter observation and complete body restored.
// Round 45 (lane 14, fast): the three flags declared before the unit lookup 164, after the overlay/name reads
// with the controlling field tested directly 164, between the name and controlling reads 164, before the
// overlay/name reads 164, between the unit and control/state lookups 164 (all same code). Retail stores all three
// flags' zeroes at entry and re-derives the unit inside both inlined 0xcbd50 queries.
// Round 273: Complete complementary permission bool/byte/long/dword at the original
// phase 281/281/277/277; entry bool/byte/short/dword 281/281/290/277. Original byte
// result, status graph and argument observation restored. Two other reduced links
// produced no comparison and are excluded; pool/state/count/return differences remain.
// Round 297: existing generic header accessor at entry/paired slots/paired
// counts, all seven subsets, 164 each; complete header graph with the existing
// typed paired-slot accessor 164. Original body and includes restored; early
// pool/state allocation, signed-count materialization and byte normalization remain.
// Round 454: Nullable held-object lookups first/second/both, conditional and branch-produced
// pointers 328/321/344/328/321/344; both with native success 320/320. Original queries, canonical
// statuses, own-parameter observation, case sets and entire executable body restored; pool/state
// allocation and count joins remain.
// Round 305: plain two-slot traversal, long/short/word/byte index with original
// conjunction or guarded second publication 472/451/424/462/424/462/423/462.
// Original paired queries, signed-count domains, argument observation and body restored.
// AC460: Canonical permission assigned by status tests in primary dispatch, byte/bool/short/long
// 164/194/377/381; both dispatches 164/194/377/381. Original expression producers, byte result,
// unconditional queries, signed observations, parameter handling and complete body restored.
// Round 77 (m3 lane 12, fast; difflib-aligned counts in brackets, control 164 [132, registers masked 49]):
// if-form depletion (`if (first != NONE) depleted = count <= 0; if (second != NONE && count > 0) depleted =
// false;`) 330 [132]; with an object-headers local for the unit and both weapon lookups 325 [116]; with a bool
// result too 336 (1259 bytes) [92, masked 30]; the first query's slot read through the headers 340 [101];
// headers local for the two weapon lookups only, unit from the entry lookup, bool result 349 (1257 bytes)
// [83, masked 25]. The last reproduces retail's frame, pushes, EBP headers, ESI name, EDI state, the flag slots
// and the first query's re-derived unit (`mov eax, ebp; mov ecx, [ecx + eax + 8]`); left: retail loads the
// controlling field into EBX before its compare and reads the name before the state pointer, keeps depleted in
// BL (initial false reloaded from +2) with interruptible in memory where ours swaps them, and re-derives the
// unit from a fresh pool data load in the second query (29 bytes). Original body restored.
// Round 531: One shared held-object handle, controlled/entry scope, byte/bool result and guarded second
// clear 164/164/194/194/328/328/336/336. Original queries and complete body restored; pool/state
// ownership and count joins remain.
// N331: Read-only native unit/control/state receiver subsets 1..7 and complete
// read-only identifiers 164/164/164/164/164/164/164/164. Original receivers, byte
// result and complete body restored; pool/SETLE/default homes and normalization remain.
// Round 62 (m3 lane 10): result/depleted/interruptible names in all six sort orders 164 each, on the
// if-form depletion 330 each; if-form with the second clear nested/`depleted &&`-guarded/as a ternary
// 330/325/330; `if (first != NONE)` first with the `&&` second 328. Original body restored.
// Round 82 (m3 lane 12, fast): if-form first depletion with `depleted = depleted && count <= 0` under
// `if (second != NONE)` 349 (1301 B); interruptibility as `(flags & 0x10) != 0` 348 (1321 B); overlay and
// name read at their switches 381 (1232 B); both held lookups first, then one `&&` depletion 352 (1319 B);
// both held queries through `*unit_reference` 164 (same code). Original body restored.
// Round 39 (m3 lane 18, fast): a headers local for the entry unit and the first slot read, the second
// query on the global, 349 (1302 B); with both weapon-object lookups through it 349; plus the if-form
// depletion 325 (frame four, the three flags at retail's +0x11..+0x13, unit not re-derived, data in EDI);
// with the controlling field tested directly 325; with the overlay read at its switch 346 (1264 B).
// Round 209 (m3 lane 3, fast; masked = difflib count over the listing with branch targets masked, baseline
// 85): the pool's data as a headers local at entry used for both weapon-object counts, if-form depletion and
// a bool result 349 [65] (round 77's form: the first query then re-derives the unit through the headers
// copy as retail does); the headers local scoped to the controlled block 341 [72]; the depletion written
// through a pointer to it 349 (identical code: the address is resolved before allocation, so it is not
// what reloads the data in retail's second query); interruptible declared before depleted 351 [57]
// (depleted then in BL and interruptible in memory, as in retail; left: no memory home for depleted's
// initial false, retail's controlling field in EBX, the second query's fresh data load, result slot);
// the controlling field tested directly 351 (same code); with a byte result 322 [65].
// AC508: Joint false-default ownership in first conditional/valid-handle assignment and
// second conditional/depleted-valid assignment, bool then byte status: 357/327/349/322 twice.
// NOT kept: no exact default/SETLE producer match. Original complete body restored; pool/state
// ownership, signed-count joins and byte normalization remain.
// N352: Actual unit/control/state record-reference families 1..7 and complete
// read-only bindings: 164 each. NOT kept: pool/state/SETLE/result homes and
// byte normalization remain; original receivers, scalar argument read and body restored.
// Y580: Actual unit-header element in mutable/readonly pointer/reference forms,
// at entry/controlled scope: 347/347/347/347/344/344/344/344. NOT kept: all regress
// and lengthen to 1300/1299 versus 1286; original lookup observations and body restored.
// Native pool/displacement ownership, SETLE/default homes and byte normalization remain.
// P345: Joint false-default chain, success/depletion/interruptibility-owned defaults,
// byte then bool success: 164/165/165/164/194/193/195/194. NOT kept: no byte match;
// original complete body restored; pool/displacement, SETLE/default homes and normalization remain.
// P350: Shared negative depletion/base expressions, bool/byte/short/long, direct/branch producers:
// 397/397/424/424/404/404/434/434. NOT kept: all regress and lengthen; original status graph and body restored.
// N389: Paired held-presence bool/byte/short/long with conjunction/guarded count stores:
// 164/330/164/330/164/330/164/330. NOT kept: pool/SETLE/default/result homes remain;
// original predicates, queries and complete body restored.
// Round 93 (m3 lane 9, fast with all six queue functions): 0x1103a0's if-form depletion with a plain 0xc70b0
// argument (no parameter address) 324 (1287 B); if-form with byte depleted/interruptible 330 (1292 B); if-form
// with the interruptibility test first in the controlling block 330 (1292 B), plus the plain argument 327
// (1279 B); if-form, plain argument and bool result 367 (1238 B).
// Round 69 (m3 lane 13, fast; aligned/masked difflib counts in brackets, control [132/49]): a headers local (the pool's
// data) in the controlled block for both weapon-object reads with the if-form depletion 326 (1292 B) [100/61]: the flags
// take retail's +0x11..+0x13 slots and entry stores and the first query re-derives the unit through the data, but
// depleted stays in memory; the headers local at entry (unit read through it) 325 (1286 B, retail's length, EBP data
// from entry) [120/64], with interruptible declared before depleted 324 (only the entry store order moves), with the
// controlling field tested directly 325 (same code), with the first depletion as an expression 328 (1293 B) [118/60].
// On these forms the overlay keeps EBX for the whole body and depleted has no register; retail spills the overlay and
// splits depleted (memory home for the initial false, BL from the first query on) because EAX (pool) and EDX (index*12)
// stay live for the second query's fresh data load, which none of these forms reproduce. On the headers-at-entry form:
// the controlling read before the overlay and name reads 325; the second weapon object through the pool macro 325.
// Round 163 (m3 lane 5, fast): a headers local in the controlling block for both weapon-object reads with if-form
// depletion and a bool result 341 (1269 bytes, frame 4, all three flags in memory, overlay keeps EBX); if-form
// depletion with a bool result and the macro reads, flags declared in the six orders result/depleted/interruptible
// 361, r/i/d 362, i/d/r 363, d/i/r 362, d/r/i 361, i/r/d 363 (1241 to 1243 bytes, frame 4). Retail has the unit in
// ECX up to the three flag stores and derives it again right after them (`mov ecx, ebp; mov ecx, [ecx + edx + 8]`),
// so the re-derivation is not register pressure; the data pointer is copied from EBP there and re-read from the pool
// for the second query. The seven scores above re-measure rounds 141 and 209. New, the queries through inline
// functions with by-value parameters (0x113980's lead): the unit lookup as an inline function at both slot reads 164
// (same code); the weapon count as an inline function of the weapon index 164 (same code); a slot wrapper
// `(unit_index, slot offset)` doing its own unit lookup and the 0xcbd50 call 363 (1317 bytes), with the count
// function 363; slot wrapper with if-form depletion and a bool result 342 (1268 bytes, frame 8), plus the count
// function 342; count function alone with if-form and bool 361 (1243, frame 4); slot wrapper, count function,
// if-form and byte result 316 (1291, frame 8). In the wrapper forms 0xcbd50 stays out of line, and that call
// between the queries gives retail's second-query shape (the data re-read from the pool, the unit through the
// kept index*12, overlay spilled at +0x14): retail's second query looks like one compiled after a memory kill
// that the first query does not see.
// M3 lane 13 r72 (fast; brackets: aligned count with registers and stack offsets masked, control 164 [33]). Retail's
// second query reloads the pool's data through the pool pointer kept in EAX from entry and re-reads the unit, while
// both weapon-object reads use the entry's data in EBP; the first query copies EBP. Re-measured with the if-form
// depletion (earlier rounds' forms, scores agree): headers local at entry for the unit and both weapon objects, bool
// result 336 (1259 B) [26], byte result 325 (1286 B) [37], with the `&&` depletion 334 (1273 B) [26]; headers for the
// weapon objects only 349 (1257 B) [20]; the depletion written through a pointer local 349 (same code). New, each
// judged by whether a second `[pool + 0x44]` load appears after the first query (none did): result/interruptible/
// depleted declaration order on the entry-headers form 335; that form without the parameter's re-read 362 (1232 B)
// [33]; on the weapon-headers form: the depletion flag made memory-resident (diagnostic, reverted) 334 [23]; both
// handles in a two-element local array filled before the tests 330 (1278 B) [34]; both lookups ahead of both tests
// 330 (same code); the handles returned through an out-parameter inline getter 364 (1239 B: 0xcbd50 then stays out
// of line, and only then is the data reloaded, after the call); the second slot read as a byte-offset expression
// 349, the first 349, both 349 (same code as the macro). So neither a store to the flag nor another spelling of the
// lookup stops the sharing; only a real call between the queries did. NOT kept: body below unchanged.
// M3 lane 1 r88 (fast with 0xcbd50, 0xc70b0 and 0x1103a0, which stays 453): a headers local (the pool's data) for the
// two weapon-object reads only, combined with the result type and depletion form: in the controlled block with a bool
// result and the `&&` depletion 326 (1280 B); at entry with the byte result and the if-form depletion 326 (1280 B); at
// entry with the byte result and the `&&` depletion 351 (1309 B); re-measured in the controlled block with a bool result
// and the if-form 341 (1269 B) and at entry with a bool result and the if-form 349 (1257 B). Re-measured: the plain
// parameter at the 0xc70b0 call (no `unit_reference`, no volatile read) 322. New: that with a bool result 372 (1236 B);
// the interruptibility test spelled `state->channels[0].unknown14 & 0x10` as 0x1103a0 does 164 (same code).
// M3 lane 9 r105 (fast; "masked" = aligned count with registers and stack offsets ignored, control 164 masked 49): a
// grid of depletion (`&&` / if-form: first sets with `<= 0`, second clears on `> 0`) x an object-headers local (none /
// in the controlling block for the two weapon-object lookups / at entry for the unit and both lookups) x byte/bool
// result x slot reads (macro / entry unit): 24 forms 164-361, none below 164 (the bool result 194/1262, if-form 330/1292,
// if-form with bool 361/1243, entry headers 334/1273, 339 and 325/1286 cells re-measure listed forms); best masked the `&&` depletion with the
// block headers local and a bool result 326 (masked 23). The headers taken at entry for the weapon lookups only (unit
// by the macro) with the if-form and bool result 349 (masked 25; round 77's form, re-measured), other cells 323-351; on it the three flags in all six declaration orders
// 341-351, interruptible declared before depleted 351 (1255 bytes, masked 18: depleted then takes BL with retail's
// setle and interruptible goes to memory, as retail; left: retail loads the controlling field into EBX before the
// flag stores, stores all three zeroes to memory and reloads depleted's false from +0x12 at both joins, and re-reads
// the pool's data in the second query); with the controlling field tested directly 351 (same code); with a byte
// result 322 (masked 40).
// Round 153 (m3 lane 8, fast with 0xcbd50 and 0xc70b0): the controlling block as a forceinline helper writing the two
// flags through `bool *` out-parameters 164 (same code: the pointers resolve before allocation, no memory kill between the
// queries); the 0xc70b0 test negated with the switch in its then-arm and `result = true` in the else 325 (1295 B); a bool
// result with byte depletion/interruptibility 194 (1262 B, as the bool result alone); the overlay field read at its switch
// (no overlay local) 172 (1287 B); the controlling field tested directly 164 (same code). The helper and the direct test were
// measured before only on other bases (the one-flag pointer helper 334, the direct test 351); both re-measured on this body.
// Round 87 (m3 lane 11, fast): the two slot reads through slot_handler.h's object_get view 164 (same code, round 297's
// accessor re-measured); result set from 0xc70b0's return with the switches under `if (!result)` 332 (re-measure). New: the
// interruptibility test between the two queries 362 (1314 B); the second query as `if (second != NONE) depleted &= count
// <= 0;` 343 (1299 B); the 0xc70b0 test negated, switches first, `else result = true` 325 (1295 B). On lane 9 r105's
// entry-headers/if-form/bool-result/interruptible-first base (re-built: 351 at 1255 B): `controlling` read before the
// overlay and name 351 (same code); the second clear as `depleted &=` under `if (second != NONE)` 350 (1256 B).
// Round 172 (m3 lane 5, fast; aligned/masked counts in brackets, control [85/33]): a pool copy `objects` at entry for
// the entry unit and both slot reads (0xcbd50 keeps its own lookup) 362 (1315 B) [75/40]; at entry for the entry unit
// and both weapon-object reads 351 (1309 B) [67/42]; declared in the controlling block for both slot reads and both
// weapon-object reads 349 (1303 B) [69/32], the slot reads only 355 (1311 B) [60/31], the weapon-object reads only
// 331 (1292 B) [87/36]; at entry for everything 378 (round 162's pool receivers). See 0x152660's note for the
// copy/global load-sharing observation.
// Round 77 (m3 lane 15, fast with the five queue functions; brackets: aligned/masked difflib counts, control [132/49]):
// the queries through the existing unit_get_current_weapon and `function_cbd50(unit_index,
// unit_weapon_view_get(unit_index)->next_weapon_index)` 164 (same code); the depletion pair in a plain static helper
// writing through `bool *` 396 (it stays out of line); the entry lookup as a pool local, a header byte pointer and an
// index*12 byte offset 164 (same code); those existing helpers on the if-form depletion 330 (same code as the if-form);
// re-measured: the 0xc70b0 argument without the volatile 271 (MiniMax H), the three flags as one local array 194
// (scalar-replaced, round 209's record). A pool copy `objects` for the entry unit and both weapon-object lookups with
// the if-form: the first query written out over the copy (the current-weapon helper's body) 328 (1293 B) [79/47]: EBP
// data, the pool and index*12 kept, the second query's fresh `[pool + 0x44]` load and the +0x11..+0x13 flag homes as in
// retail, but the first query reuses the entry's unit; the same with a byte-offset entry or a forceinline header-array
// helper 328 (same code); with the first query back on 0xcbd50 326 (1280 B) [105/47] (lane 1 r88's cell): the first
// query is then retail's exact code (the data copied from EBP, the unit re-derived), but the second query merges with
// it, also with its slot read through unit_weapon_view_get (same code). Both queries through a non-inline `(unit_index,
// hand)` helper in another file 330 (LTCG inlines and merges it). The header array as a named local inside 0xcbd50's
// own lookup (contributor file, measured and restored) changes nothing. Retail's second query needs a lookup that none
// of these spellings give. Body unchanged.
// @retail 0x110ab0
bool __stdcall function_110ab0(long unit_index)
{
	// Retail keeps this argument on the stack.
	long const *unit_reference = &unit_index;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long overlay = control->channel_9c.unknown08;
	long name = state->unknown7c;
	long controlling = unit->field_13c;
	byte result = false;
	bool depleted = false;
	bool interruptible = false;
	if (controlling != NONE)
	{
		long first = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
		depleted = first != NONE && *(short *)((byte *)UNIT_ANIMATION_OBJECT(first) + 0x17c) <= 0;
		long second = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x213));
		depleted = depleted && (second == NONE || *(short *)((byte *)UNIT_ANIMATION_OBJECT(second) + 0x17c) <= 0);
		if (*(byte *)((byte *)state + 0x14) & 0x10)
			interruptible = true;
	}
	if (function_c70b0(*(long const volatile *)unit_reference))
		result = true;
	else
	{
		switch (name)
		{
		case 0x400004a:
		case 0x400076c:
		case 0x400076d:
		case 0x50000c3:
		case 0x5000768:
		case 0x5000769:
		case 0x500076a:
		case 0x500076b:
		case 0x600005f:
		case 0x600008c:
		case 0x60000cc:
		case 0x60000cd:
		case 0x60006ac:
		case 0x700002c:
		case 0x7000543:
		case 0x700076e:
		case 0x80000c4:
		case 0x800076f:
		case 0x9000011:
		case 0x9000012:
		case 0xa000010:
		case 0xa000013:
		case 0xa00002d:
		case 0xa00003e:
		case 0xa00022d:
		case 0xa000767:
		case 0xb00002e:
		case 0xb00022e:
		case 0xb0005b8:
		case 0xc000043:
		case 0xc0005b7:
		case 0xd00002b:
		case 0xd000042:
		case 0xd00022c:
		case 0xe00002a:
		case 0xe00003b:
		case 0xe0000c3:
		case 0xf00003a:
		case 0x1000003c:
			result = !interruptible;
			break;
		case 0x500000a:
		case 0x5000024:
		case 0xa000040:
		case 0xa0005b9:
		case 0xb0005b2:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xc0006b3:
		case 0xc0006cd:
		case 0xd000021:
		case 0xe000038:
		case 0xe00067d:
		case 0x1000006c:
		case 0x11000074:
		case 0x11000076:
		case 0x140005b3:
			result = !depleted;
			break;
		}
	}
	switch (overlay)
	{
	case 0xa00022d:
	case 0xb00022e:
	case 0xd00022c:
		result = !interruptible;
		break;
	case 0x500000a:
	case 0xa000040:
	case 0xa0005b9:
	case 0xb0005b2:
	case 0xc000073:
	case 0xc000075:
	case 0xc000077:
	case 0xc0006b3:
	case 0xc0006cd:
	case 0xd000021:
	case 0xe000038:
	case 0xe00067d:
	case 0x1000006c:
	case 0x11000074:
	case 0x11000076:
	case 0x140005b3:
		result = !depleted;
		break;
	}
	return result;
}


// MiniMax S (11-14): no new form, no lower score (445): retail's frame is 0xc against our 8, it keeps the pool pointer in ECX and the pool data in ESI where ours loads the global into EAX and the data into EDX, it forms the control pointer as base+offset where ours stores the bare offset, and it writes the true default at the entry where ours starts from the switch; declaring the two flags before the pointers gives 446 and dropping the switch's default arm gives 489, so neither spelling reaches retail's frame.
// MiniMax G (11-12): no new form, no lower score (445): the frame is still 0x8 against retail's 0xc — retail keeps the result byte in
// the first argument's dead slot, the two flag bytes at the frame bottom and the control and controlling words above them, and it writes
// the true default at the entry; declaring the two flags before the pointers gives 446, and the entry default with the switch's default
// arm dropped gives 489, so neither spelling reaches retail's frame.
// MiniMax H (11-13): no new form, no lower score (445); retail's frame is 0xc with four pushes against our 8 with two, it keeps the globals pointer in ECX and the pool data in ESI where ours loads them into EAX and EDX, and it holds index times twelve in EDX where ours folds the index into the address.
// score 445 (was 445): earlier 445 (was 453): retail keeps the result byte in the first argument's dead slot and re-derives the unit in both seat
// MiniMax E (11-11): no new form, no lower score (445); the frame's slots sit at retail's offsets one dword off
// and retail keeps its frame pointer and the early pointer where ours does not.
// MiniMax S (11-11): no new form, no lower score (445); retail's frame is 0xc against our 8, it keeps the pool pointer in ECX and the pool data in ESI where ours loads the global into EAX and the data into EDX, it forms the control pointer as base+offset where ours stores the bare offset, and it writes the true default at the entry where ours starts from the switch; the entry default is the known lead but it costs the matched caller 0x10ff40 its score.
// MiniMax C (11-11): no new form, no lower score (445); `requested` still takes edi and the unit pointer is kept where retail keeps the pool data in esi and the index times twelve in edx.
// queries (rounds 85, 149 and 93 below).
// MiniMax R (11-04): no new form, no lower score.
// MiniMax R (10-31): no new form, no lower score.
// MiniMax H (11-06): the result initialised to true at the declaration gives 450, the same code as the
// uninitialised one; a second 900 s permuter search (seed 7) found no better form either.
// MiniMax H (11-06): the permuter's 300 s search found no better form (reduced 467, full 447, both the
// baseline); the frame word and the requested-in-EDI lifetime still differ.
// b4D walk (11-06, statement by statement against retail 0x1103a0-0x110aa0, with an emulation of retail's
// state-switch compare tree over every case constant of both files): no logic difference. The state switch
// (all 12 groups: interruptible, `requested == 0x600008c ? true : interruptible`, depleted, 0x600008c's
// requested set, the 0x600008d/e + 0xc000043 + 0xd000042 group, allow, false, default true), the
// 0xa000010 jump table, the depletion pair (first `<= 0` sets, second `> 0` clears, via the inlined
// function_cbd50 slot test) and both 11-value requested sets of the overlay switch (overlay 0xb0 bit 4 /
// depleted) all agree. Changes: `result = interruptible; if (requested == 0x600008c) result = true;`
// (value set then overridden) is now the ternary, 453 (1793 B, was 450 at 1789 B); the raw +0x14/+0xb0
// flag tests use the named `channels[0].unknown14` / `channel_9c.unknown14`, same code. Tried:
// `bool result = true;` with the default arm dropped 483 (1785 B).
// earlier 453 (was 450): this round found no gain - retail keeps the result byte in the first argument's dead
// slot (round 85's `bool result = true;` with no default arm is the spelling that lands it there) and holds
// index*12 in EDX so the unit is re-derived for the query; ours keeps the unit in a register.
// Earlier: this round found no gain - retail keeps the pool pointer live in ECX and the data in
// ESI, rematerialises the unit pointer for the query, reloads the data from the pool and writes the
// depletion with setle into the flag slot, while ours keeps the unit in a register and initialises the
// result byte differently; the retail-shaped query joins score 330 or worse.
// Earlier: Frame eight versus twelve, requested/state ownership and result joins remain.
// Earlier 453 (was 478): frame 8 versus 12 and requested/state registers still differ.
// Predicate names 478; byte result 538; byte depleted/overlay 453; global optimization off 1195;
// merged interruptible cases 473; byte depleted alone 458; byte overlay alone 477; merged cases with bytes 456.
// Earlier 451 (was 451): frame 8 versus 12 and requested/state homes remain.
// Byte-context word/native interruptible 456/453; named channel words 453;
// size policy 827; common requested override 754 (unchanged fast preview 453).
// Second depletion accumulation 468; bitwise depletion 482.
// Earlier 452 (was 451): Frame 8 versus 12 and requested/state homes remain; default first 451, overlay depletion cases first 451, opposite predicate arms 470, first/second count scalars 451/451.
// Earlier 430 (was 453): Frame 8 versus 12 and requested/state homes remain; requested success override 430 (plain); signed-byte result 506; initialized result/unit scope 451/451; early byte overlay 533; paired allow override 434; three override groups 454.
// Frame 8 versus 12 and requested/state homes remain;
// byte result 506; byte interruptibility 453; separate overlay result 508;
// direct inner membership 486; held-index names 451 (fast control 451). Opening body retained.
// Explicit true initialization / isolated two-request override / both 450/470/470;
// overlay-time control alone/with true initialization/with override/with both 513/513/530/530; entry result with both 530.
// Frame 8 versus 12, requested/state carriers and result joins remain.
// frame 8 versus 12 and requested/state registers still differ.
// frame 8 versus 12 and requested/state homes remain.
// Frame 8 versus 12 and requested/state homes remain; default first 451, overlay depletion cases first 451, opposite predicate arms 470, first/second count scalars 451/451.
// Frame 8 versus 12 and requested/state homes remain; requested success override 430 (plain); signed-byte result 506; initialized result/unit scope 451/451; early byte overlay 533; paired allow override 434; three override groups 454.
// Byte return with byte result 482; byte return alone 450; byte return/result with Boolean overlay 482;
// with controlling field at use 482; with separate final overlay byte 482. Original Boolean interface retained.
// Native signed count/slot/both fields 450/450/450; read-only receivers alone/with count/with slots/with both
// 450 each; typed read-only inputs plus native result 554. Original record and body restored.
// Round 222: retained slot-owner 450; controlling field at both uses 450; Boolean depletion/overlay/both 482/466/482; both with retained owner/entry result 482/482; pointer-home names 450.
// Round 224: First/second/both query scopes and branch-entry output declarations 450/450/450/450;
// branch-owned status defaults alone/with first/second/both scopes 485/485/485/485. Frame eight versus
// twelve and requested/state/result homes remain.
// Round 226: Shared held-query conditional/rejection returns 450/450; native signed-byte slot 451;
// byte slot with conditional/rejection returns 451/451; ordinary inline conditional/rejection
// returns 450/450, with byte conditional result 451. Frame/request/state homes remain.
// Round 241: Complete complementary permission bool/byte/short/long 572/572/616/609;
// with complementary base interruptibility 593/593/632/619. Original body restored;
// frame eight versus twelve and requested/state/result homes remain.
// Round 220: Shared eleven-request overlay membership bool/byte/short/long through logical OR
// 641/641/633/637; conditional-result counterparts 627/627/619/623. Original body restored.
// Round 30: Complete depletion complement bool/byte/short/long 450/450/466/477; with late overlay
// complement 458/458/445/477. The short pair recovers frame twelve but gains only five (dropped).
// On that frame: true result here/at entry 445/445; entry byte/signed byte 548/548; live entry true
// default bool/byte/short/long 511/563/559/548. All statuses, defaults and lifetimes restored.
// Round 168: Complete unsigned key/controlling/held-handle domains in seven subsets
// 451/450/450/451/451/450/451; all with unsigned unit 451. Original signed domains restored.
// Round 34: Native mutable/readonly object-header bases, with repeated/retained unit-slot
// lookup inputs 487/487/487/487; the same four with complete short depletion/overlay
// complements 504/504/504/504. Original pool observations, statuses and body restored.
// Native pool bases recover frame twelve at 487; the paired short graph reserves sixteen.
// On the native pool graph, default success bool/entry-bool/byte/entry-byte
// 474/474/514/514. Original result defaults and lifetimes restored.
// Round 169: whole primary result groups 0x400076c/0x500000a/0x50000c3/0x600008c/
// 0x600008d/0x60000cc/0x700005c moved first, and the false group after the first
// interruptibility group: eight forms 450 each. Original dispatch order and body restored.
// Round 389: Plain two-held-slot traversal with long/short/word/byte indices, slot-first
// validity joins 470/494/494/523; shared validity-first joins 470/494/494/491 (fast control
// 450). Original paired queries and complete body restored; frame eight and state homes remain.
// Round 174: transposed overlay/request dispatch with bool/byte/short/long
// permission 663/725/829/792; first/second/both overlay defaults before inner
// dispatch 497/478/581; both with byte permission 545. Original graph restored;
// frame eight versus twelve and requested/state/result homes remain.
// Round 81: an object-headers local for the unit and both depletion checks 446, but the unit index then
// travels in ECX (ret 8, retail ret 0xc) and both callers lose (0x10ff40 44 -> 157, 0x111650 666 -> 673);
// with a bool depletion 480; with the slot reads through the unit 487; the headers local for the two
// checks only, after/before the unit read, 480/480; with the unit index read once through its address
// 474 (callers unchanged); that address read alone 450.
// Round 83: depletion/interruptible declared before the unit lookup 450 (same code); that with `result = true`
// at its declaration and no state-switch default 483; interruptible before depleted 463; the overlay flag
// declared with the others 493; the 0xc7070 test inverted 473.
// Round 85: retail's state default reads a true byte kept in the unit index's dead slot (`bool result =
// true;` with no default arm): that with one read of the unit index through its address 474 (1793 bytes,
// callers unchanged, frame still 8), plus an object-headers local 478; with a short depletion 506, short
// depletion and overlay 510, both with the headers local 495; the depletion written through a forceinline
// `byte *` helper 474 (same code).
// Round 209: Base-permission observation before both read-only held queries, bool/byte/short/long
// 451/451/471/473; with the early overlay observation 506/495/512/504. Short/long overlay forms
// recover frame twelve but regress to 1833/1825 bytes. Overlay moves were exact-match-only.
// Original observations, paired queries, results, dispatch and complete body restored.
// Round 243: Reused long/word/dword header ordinal and long byte offset,
// at entry 491/469/491/489 and within controlled admission 495/461/495/476.
// Original pool observations and body restored; frame/state/request/result homes remain.
// Round 115 (lane 7, fast): slot bytes read through the unit local 450 (same code); the first depletion as one
// assignment 481; `bool result = true;` without the default arm re-measured 483 (the unit index then arrives in
// EDI); `controlling` read before the control/state pointers 444 (1793 bytes, a gain of 6 only), with bool
// depleted/byte interruptible/flags declared in reverse/bool overlay 470/472/461/453; state before control 450.
// Retail 0x110ab0 re-reads g_4e0300->data for the second slot block the same way with no store in between.
// Round 273: Reused two-request primary membership bool/byte/short/long before the
// predicate call 497/497/474/472; inside dispatch 482/482/478/482. Original request
// comparisons, case results and complete body restored; frame eight versus twelve remains.
// AC460: Live true permission with complete conditional failure stores for base status,
// depletion and conditional-request families, seven subsets, then all at entry:
// 487/491/499/483/475/499/511/511. Entry unit EDI/ret 8 and both caller scores regress;
// executable differences also rise. Original result graph, defaults and body restored.
// Round 297: finite depleted/base/overlay status domains, masks 1..7,
// 481/488/488/430/485/492/492; full finite result domain 695. Overlay-only
// 430 confirmed plain at 1793 bytes, but code differences remain 424; decoded
// trailing-table differences fall 22 to 6. Table/extent-only gain rejected; all
// original statuses, body and types restored. Frame eight versus twelve remains.
// Round 305: selected real header pointer/value, mutable/readonly at entry
// 497/497/512/512 and in the held-query admission 476/476/491/491. Original live
// pool observations, statuses and dispatch restored; frame and state/request homes remain.
// Round 531: Existing object-header accessor at entry/paired slots/paired counts, seven subsets
// 450/450/450/450/450/450/450; full accessor graph with read-only receivers 450. Original body and
// includes restored; frame eight versus twelve and header/state/request homes remain.
// N331: Nullable first/second/both held-object receivers, conditional/branch producers,
// 441/441/471/441/441/471; paired Boolean depletion 457/457. NOT kept: raw 441
// raises code differences 424 -> 431, adds null guards absent from retail, and only
// improves trailing data. Frame remains eight versus twelve. Original body restored.
// Round 62 (m3 lane 10): depleted/interruptible/overlay names in six sort orders 450 each; the
// 0x600008d group as a nested requested switch 519; `requested != 0x400000c || allow` 468; the 0xc7070
// test inverted with the switch first 473; first depletion as `first != NONE && count <= 0` 481.
// Round 82 (m3 lane 12, fast): bool depleted 482 (1773 B); bool overlay interruptibility 466 (1785 B);
// `result = requested == 0x600008c || interruptible;` 477 (1773 B); overlay test re-reading the controlling
// field 450 (same code); first depletion as `first != NONE && count <= 0` 481 (1777 B). Original body restored.
// Round 39 (m3 lane 18, fast): overlay flag computed inside the controlled block 519 (1749 B); case
// 0x600008c as `result = interruptible;` then an inner switch with no default 451; the state name read
// into a local at entry 530 (1865 B); the overlay read into a local at entry 464 (1793 B, but the unit
// index then arrives in EDI); a headers local for the unit, first slot and both weapon lookups 476.
// Round 209 (m3 lane 3, fast; masked = difflib count with branch targets masked, baseline 217): retail
// also keeps the result in DL (`mov al, dl` at the exits) and re-reads `requested` from its stack slot at
// every use, where ours holds `requested` in EDI and builds the result in AL. Tried: the pool's data as a
// headers local for both weapon counts with the parameter kept on the stack as in 0x110ab0 (address local
// plus the volatile read at the 0xc7070 call; a volatile diagnostic, not keepable) 454 (1793 B, masked 212;
// the first query then re-derives the unit; callers unchanged); that with `bool result = true;` and no
// state-switch default 480 (1801 B); that
// with interruptible declared before depleted 456; the headers local with `controlling` read before the
// control/state pointers 482 (unit index in ECX, ret 8; 0x10ff40 157, 0x111650 673); `result` declared
// first 450 (same code). A local record holding the three flags (diagnostic on 0x110ab0) is scalar-replaced
// with no memory home, so retail's flag homes are not an aggregate's.
// Y561: Complete uninitialized result short/word/long/dword at original
// declaration 574/574/554/554; before lookups 574/574/554/554. NOT kept:
// all regress and grow away from retail; original Boolean result/body restored.
// N352: Actual unit/control/state record-reference families 1..7 and complete
// read-only bindings: 450 each. NOT kept: frame stays eight versus twelve,
// with 424 code differences; original pointer receivers and full body restored.
// P327: Complete base/depletion/overlay true-false result producers, masks 1/2/4/3/5/6/7,
// then all with result at entry: 465/430/450/465/465/430/465/465. NOT kept: 430
// still has 424 code differences; only table/extent improves. Original full body restored.
// Y599: Terminal first/second/both overlay decisions return directly, original/Boolean
// overlay, then both with Boolean depletion: 450/560/560/466/567/567/575/581. NOT kept:
// no verified native frame/result-join gain; original full body restored.
// Best code prefix stays 424 differences; all frames remain eight versus twelve.
// DH372: First/second held-query presence bool/byte/short/long: 450 each.
// NOT kept: ties, 424 code/22 tail differences and frame eight versus twelve.
// Original body and queries restored; state/request/result homes remain.
// P350: Allow parameter byte/signed-byte/short/word/long/dword, then unsigned request
// with bool/byte allow: 441/441/441/441/441/441/451/444. NOT kept: 430/433 code
// differences versus 424; apparent gain is table/extent only. Original Boolean contract restored.
// N389: Actual base query owns the result or the true-default producer, bool/byte/short/long:
// 450/495/574/554/490/477/540/514. NOT kept: missing frame word and state/request/result
// homes remain; original query, permission graph and full body restored.
// Round 93 (m3 lane 9, fast with all six queue functions): 0x110ab0's `&&` depletion 512 (1829 B), with a bool
// depleted 490 (1817 B); the state name read into a local at entry for the first switch 530 (1865 B); the
// overlay identifier read into a local at entry for the second switch 464 at 1793 B, but the unit index then
// arrives in EDI (0x10ff40 218, 0x111650 686); name and overlay at entry plus the `&&` depletion 594 (1905 B).
// Round 163 (m3 lane 6, fast with 0x10ff40/0x111650, controls 12/621): retail keeps depleted at +0x12 and
// interruptible's home at +0x13 (reloaded into BL around the seat lookups, which use BX), the true result in the
// unit index's slot, `requested` never in a register, state in EDI and g_4e0300 in ECX for the second lookup's
// reload of ->data; ours holds `requested` in EDI and state in ESI. `requested` read through a const pointer to it
// 453 (same code); `controlling` read first with `bool result = true;` and no default arm 483 (1785 B, unit index
// in EDI, callers 217/655), that with `requested` through the pointer 483; depleted, interruptible and the overlay
// flag as char 448 (1801 B), interruptible alone as char 448 (the whole gain; a type picked by score, five points),
// the overlay flag alone as char 453; the overlay identifier read into a local before its flag 461 (1789 B);
// interruptible assigned directly (`= (... & 0x10) != 0`) 445 plain (1793 B, callers unchanged; state moves to EDI as
// retail, but the flag becomes `mov bl, [edi + 0x14]; shr; and` where retail has `test; je; mov bl, 1`, aligned 343 ->
// 351: not kept); the overlay flag initialized directly 453; the second depletion test as nested ifs 453; the 0x600008c
// and 0x400000c ternaries as if/else 453 each; the interruptible test ahead of the two lookups 453. All restored.
// Round 149 (m3 lane 8, fast; callers 0x10ff40/0x111650 in the same run): retail stores the `true` default in the unit
// index's dead slot at entry and reloads it at the joins (0x11053a/0x11053e); the 0x400076c group's true and the 0x700005c
// group's non-0x400000c result jump there, the 0x600008d group and the inner 0x600008c switch store an immediate 1. Every
// form with `bool result = true;` makes our first pass load the unit index into EDI, so it becomes a register argument
// (0x10ff40 12 -> 217). Re-measured `bool result = true;` with no default arm 483 (1785 bytes); on it: state before
// control 483, the result declared first 483 (same code); an object-headers local for both weapon lookups 489 (1809 bytes,
// 0x10ff40 154), that also for the unit 472 (1793 bytes, 0x10ff40 154, 0x111650 596); the 0x400076c and 0x700005c arms as
// `if (requested != 0x600008c) result = interruptible;` / `if (requested == 0x400000c) result = allow;` 519 (1749 bytes).
// Round 168 (m3 lane 5, fast, callers 0x10ff40/0x111650 checked in each): kept `interruptible = (state->channels[0].
// unknown14 & 0x10) != 0;` in place of the false-then-if flag (round 163's listed 445; it drops a flag set inside a
// test): 445 at 1793 bytes, callers 12/566 unchanged, state then in EDI and the index in EBP as retail; left: ours
// gives ESI to `requested` and keeps the unit pointer, where retail keeps the pool data in ESI and index*12 in EDX
// and reloads the unit for the seat reads (the second from g_4e0300 again). The slot lookups through the existing
// unit_get_current_weapon/unit_weapon_view_get (include/unknown_1e1f20.h) 453 (same code: LTCG merges the unit load
// across views). Re-measured: `bool result = true;` with the 0x400076c/0x700005c if-arms and no default 519 (1749 B;
// retail's default reloads that byte, `jne 0x11053e`), index in EDI, 0x10ff40 217; the direct overlay flag on 445
// 445, alone 453; a headers local for the unit and both weapon lookups on 445 445 (1789 B; data ESI and the seat
// reload as retail, but the index arrives in ECX: 0x10ff40 154, 0x111650 596). On 445: the headers local for the
// entry unit only 445; for the unit and both seat reads 478 (callers 154/596); for the seat reads and the weapon
// lookups 504 (154/596); the seat bytes read through the `unit` local 445 (round 115's listed form); the flag as a
// `? true : false` ternary 445; the flag as `(bool)((state->channels[0].unknown14 >> 4) & 1)` 452.
// Round 93 (m3 lane 10, fast with 0x10ff40/0x111650, unchanged unless given; al = difflib count of aligned lines, base
// 351): the first slot byte through `unit` with the second re-derived (0x82b70's mixed form) 445 (same code); the
// 0x600008d group as `(requested == 0xc000043 || requested == 0xd000042) ? true : interruptible` 505 (1749 B), as an
// if/else 505 (same code); `control` declared at the overlay section 507 (1749 B; unit index in ECX, 0x10ff40 154,
// 0x111650 596); the weapon counts through slot_handler.h's object_get 445 (same code). Lead, exact-only (it needs one
// read of the unit index through its address, as 0x1130f0 does): `bool result = true;` with the 0x400076c/0x700005c
// groups as `if (requested != 0x600008c) result = interruptible;` / `if (requested == 0x400000c) result = allow;` and no
// default 471 (1805 B, al 214: frame 0xc and the true byte in the unit index's home as retail, callers unchanged); with
// the 0x600008d group as that if/else 486 (1769 B, al 203); with a bool depleted 498 (1757 B, al 199); with the
// interruptible flag as the if-form 539. Left there: every first-switch arm stores to the true byte's slot, where retail
// keeps the result in DL and reloads the slot only on the default edge; `requested` takes EDI (retail: memory, ECX per
// region) because retail re-derives the unit in both seat queries (index*12 kept in EDX, the pool data in ESI).
// Round 77 (m3 lane 15, fast with 0x10ff40/0x111650): `result = true;` before the state switch with the default arm
// dropped 506 (1833 B); `bool result = true;` declared just before the 0xc7070 test, no default arm, 474 (1781 B; the
// unit index becomes a register argument in ESI: 0x10ff40 208, 0x111650 638); a pool copy for the entry lookup only 445
// (same code, round 168's re-measured); the pool copy for the entry and both weapon-object lookups 482 (1805 B: frame
// 0xc, the data in ESI and the first query re-derived through a copy of ESI as in retail, but the unit index arrives in
// ECX: callers 154/596); for the two weapon-object lookups only 454 (1797 B, callers 154/596); `result = interruptible
// || requested == 0x600008c` 468 (1789 B). Body unchanged.
// P443: Shared controlling truth, complete first depletion producer, independent held outcomes,
// state/control accessors at all uses, native-use primary/overlay keys and named base query:
// 474/464/500/534/521/445/445/445. NOT kept: no native frame/result repair; original complete body and macros restored.
// P445: Actual allow receiver, shared overlay-policy dispatch, existing field-test macro,
// declared set-field view, overlay-channel receiver, single/two-request local producers and immutable controlling handle:
// 445/644/445/445/445/445/445/445. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P447: First/second signed slot arguments; first/second count decisions; overlay/allow admissions; primary and paired-overlay inner request case order:
// 445/445/445/445/445/445/445/445. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P461: Complementary controlling-absence outcome bool/char/signed-char/byte
// 474 each, short/word 451/451, long/dword 467/467. NOT kept: frame stays eight
// versus twelve and every score regresses; original handle, queries and full body restored.
// @retail 0x1103a0
bool __stdcall function_1103a0(long unit_index, long requested, bool allow)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long controlling = unit->field_13c;
	bool result;
	byte depleted = false;
	bool interruptible = false;
	if (controlling != NONE)
	{
		long first = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
		if (first != NONE)
			depleted = *(short *)((byte *)UNIT_ANIMATION_OBJECT(first) + 0x17c) <= 0;
		long second = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x213));
		if (second != NONE && *(short *)((byte *)UNIT_ANIMATION_OBJECT(second) + 0x17c) > 0)
			depleted = false;
		interruptible = (state->channels[0].unknown14 & 0x10) != 0;
	}
	if (function_0c7070(unit_index))
		result = interruptible;
	else
	{
		switch (state->unknown7c)
		{
		case 0x400004a:
		case 0xe0000c2:
			result = interruptible;
			break;
		case 0x400076c:
		case 0x400076d:
		case 0x5000768:
		case 0x5000769:
		case 0x500076a:
		case 0x500076b:
		case 0x600005f:
		case 0x700002c:
		case 0x7000543:
		case 0x700076e:
		case 0x800076f:
		case 0xa00003e:
		case 0xa00022d:
		case 0xa000767:
		case 0xb00022e:
		case 0xd000021:
		case 0xd00002b:
		case 0xd00022c:
		case 0xe00002a:
		case 0xe0000c3:
		case 0x1000006c:
			result = requested == 0x600008c ? true : interruptible;
			break;
		case 0x500000a:
		case 0x5000024:
		case 0xa000040:
		case 0xa0005b9:
		case 0xb0005b2:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xc0006b3:
		case 0xc0006cd:
		case 0xe000038:
		case 0xe00067d:
		case 0x11000074:
		case 0x11000076:
		case 0x140005b3:
			result = depleted;
			break;
		case 0x50000c3:
		case 0x80000c4:
			result = interruptible;
			break;
		case 0x600008c:
			switch (requested)
			{
			case 0x600008d: case 0x600008e: case 0x900001f: case 0x9000020: case 0x90006b2:
			case 0xc000043: case 0xd000042:
				result = true;
				break;
			default:
				result = interruptible;
				break;
			}
			break;
		case 0x600008d:
		case 0x600008e:
		case 0xc000043:
		case 0xd000042:
			result = requested == 0xc000043 || requested == 0xd000042 || interruptible;
			break;
		case 0x60000cc:
		case 0x60000cd:
		case 0x60006ac:
		case 0x9000011:
		case 0x9000012:
		case 0x90006b2:
		case 0xa000010:
		case 0xa000013:
		case 0xa00002d:
		case 0xb00002e:
			result = interruptible;
			break;
		case 0x700005c:
		case 0x700005d:
		case 0x800061e:
		case 0x900000e:
		case 0xa00000f:
			result = requested == 0x400000c ? allow : true;
			break;
		case 0x800001c:
		case 0xa00001d:
		case 0xb0005b8:
		case 0xc0005b7:
		case 0xe00003b:
		case 0xf00003a:
			result = false;
			break;
		default:
			result = true;
			break;
		}
	}
	byte overlay_interruptible = false;
	if (controlling != NONE && (control->channel_9c.unknown14 & 0x10))
		overlay_interruptible = true;
	switch (control->channel_9c.unknown08)
	{
	case 0xa00022d:
	case 0xb00022e:
	case 0xd00022c:
		switch (requested)
		{
		case 0x400000c:
		case 0x600008c:
		case 0x900000e:
		case 0x9000015:
		case 0x9000016:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xa000014:
		case 0xa000017:
			result = true;
			break;
		default:
			result = overlay_interruptible;
			break;
		}
		break;
	case 0x500000a:
	case 0xa000040:
	case 0xa0005b9:
	case 0xb0005b2:
	case 0xc000073:
	case 0xc000075:
	case 0xc000077:
	case 0xc0006b3:
	case 0xc0006cd:
	case 0xd000021:
	case 0xe000038:
	case 0xe00067d:
	case 0x1000006c:
	case 0x11000074:
	case 0x11000076:
	case 0x140005b3:
		switch (requested)
		{
		case 0x400000c:
		case 0x600008c:
		case 0x900000e:
		case 0x9000015:
		case 0x9000016:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xa000014:
		case 0xa000017:
			result = true;
			break;
		default:
			result = depleted;
			break;
		}
		break;
	}
	return result;
}

extern point2f *g_4687c4;
struct s_unit_seat_definition;
s_unit_seat_definition *function_c8fc0(long unit_index);
bool unit_animation_control_channel_playing(long object_index);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
vector2f *function_11df30(vector2f *angles, vector3f const *v);

PRIVATE __forceinline void bounds_112070(s_aiming_screen const *screen, real *bounds)
{
	bounds[0] = 0.0f - screen->right_frame_count * screen->right_yaw_per_frame;
	bounds[1] = screen->left_frame_count * screen->left_yaw_per_frame;
	bounds[2] = 0.0f - screen->down_frame_count * screen->down_pitch_per_frame;
	bounds[3] = screen->up_frame_count * screen->up_pitch_per_frame;
}

PRIVATE __forceinline void project_112070(vector3f const *v, vector3f const *forward,
	vector3f const *side, vector3f const *up, vector3f *result)
{
	vector3f input_direction = *v;
	result->i = forward->k * input_direction.k + forward->j * input_direction.j + forward->i * input_direction.i;
	result->j = side->k * input_direction.k + side->j * input_direction.j + side->i * input_direction.i;
	result->k = up->k * input_direction.k + up->j * input_direction.j + up->i * input_direction.i;
}

// MiniMax S (11-14): no new form, no lower score (776): the body is 3418 bytes against retail's 3403 and our frame is 0xcc against retail's 0xd4, the unit and the state are in the opposite registers, the first angle conversion is expanded here and called out of line in retail, and retail's prologue still holds a chain of otherwise unused tag loads that ours does not have; the entry-default spelling that would take 0x1103a0's frame to retail's shape costs 0x10ff40 its 12.
// MiniMax A (11-13): no new form, no lower score (776); our frame is still 0xcc against retail's 0xd4, the
// first angle is still expanded where retail calls out of line, and retail's prologue chain of otherwise
// unused tag loads is still missing.
// MiniMax H (11-13): no new form, no lower score (776); the frame is short of retail's 0xd4, the unit and the state are in the opposite registers, the first angle conversion is a call here and expanded in retail, and retail does an extra model-linked lookup.
// score 776 (was 776): frame 0xcc versus 0xd4 and first angle expansion remain; retail calls channel_play
// MiniMax D (11-11): no new form, no lower score (776); the frame is still 0xcc against retail's 0xd4, and
// retail's prologue still holds the lookup chain ours does not have: the unit's definition index, then the
// definition's model tag at +0x38, then that tag's bytes, then +4 and one more tag, all stored into a frame
// slot that is overwritten before any read. Writing that chain out in the source only produces the dead
// store the compiler drops (the earlier round's unused-local form was eliminated as well), and the eight
// missing frame bytes are the same ones the earlier rounds' angle and overlay forms could not recover.
// MiniMax E (11-11): no new form, no lower score (776); retail's frame is 0xcc bytes where ours is 0xd4 and its
// first angle is expanded out of line, so every stack slot sits eight bytes off.
// MiniMax C (11-11): no new form, no lower score (776); the loop's channel play is still expanded two levels where retail calls 0x1cb490 out of line.
// (0x1cb490) out of line at five sites and expands only the final animation_10c start, ours expands the
// overlay loop's call. At entry retail also loads an otherwise unused render-model tag into a stack slot.
// Earlier at 790: frame 0xcc versus 0xd4 and first angle expansion remain. At entry retail also
// frame 0xcc/0xd4 and first angle/overlay inline allocation remain.
// Earlier: frame 0xcc versus 0xd4 and first angle expansion remain. At entry retail also
// frame 0xcc versus 0xd4 and first angle expansion remain. At entry retail also
// frame 0xcc versus 0xd4 and first angle/overlay inline allocation remain.
// frame 0xcc versus 0xd4 and first angle expansion remain. At entry retail also
// frame 0xcc versus 0xd4 and first angle expansion remain.
// computes the graph of the definition's +0x38 tag (+4) into a frame slot that is overwritten before any
// read (about 30 bytes ours lacks); retail keeps `active` in a stack byte cleared by a conditional store,
// computes the centred test once with x87 fabs before the blend block, copies the default angles as a
// struct and duplicates the seat-weight call setup in both arms. 0x1c6470 takes graph in ECX in
// both builds; its save placement and store scheduling still differ at the four calls.
// Tried: depth zero 1428, depth one 833, projection inline 833, bounds inline 1055, native centered 834,
// separate secondary activity 833; two inline levels 833, sequential channel construction 833, activity
// byte 833, short overlay counter 833, channels before curve weights 833; seat-weight branches 874,
// overlay pointer countdown 863, refreshed object 833, direct bounds fallback 859, size preference 1557,
// object pointer name 833, control-first declaration 833; primary direction snapshot 863; secondary 829
// (a one-use copy); both 865; native initial angles 848; centered predicate after channel preparation
// 871; partial j/k inputs 919; partial i input 899; early screen pointers 934; named state predicate 873;
// unsigned counter 879; direct eligibility accumulation 875 (kept then); state-guard accumulation 892;
// entry blend channels 1057; yaw/pitch values 875; centered tests at use 866; early blend products 853;
// whole projection output 880; centered tests plus early products 841; native projection return 889
// (frame 0xd4); float bound fields 841; byte overlay ordinal 841; joined bound receiver 912; named blend
// scalars 841; native projection inputs by value 1112; native projection output reference 841; native
// bounds-screen reference 841; signed-word activity 844; readonly retained tag 841; shared dot products
// with basis/direction first 841/841; uniform running projection sums 841; output-first 841; native
// input references 841; references plus output-first 841; running sums plus output-first/references
// 841/841. Round 181: direction input value/native copy 810/810; basis values/native copies 1067/1073;
// all native input copies 1125; direction/basis/all value inputs with vector return 900/1181/1166;
// direction copied once inside projection retained (810). Round 171: uniform named k/j/i product subsets
// all 810; uniform first k+j row sums 810. Round 188: immutable direction 810; ordinary inline 1151;
// complete result publication 900 (frame 0xd4); pointer result 810; direction/basis/all input
// references 810; output-first 810; row orders ikj/jik/jki/kij/kji 810/829/829/839/840; uniform k/j/all
// product operand order 810. Round 75: entry pointers in retail's load order 810; separate second
// projection vector 830; default angles copied as a struct 826; `active` cleared by a conditional
// assignment 884 (3358 bytes, aligned 846 -> 622); that plus the centred test as a named bool 875
// (aligned 584).
// Round 187: basis matrix 807; matrix/shared cross 858; matrix pointer/reference/value
// inputs 807/807/842; aggregate/member basis copies 1102/1102; matrix reference/shared
// cross 851. The three-count gain is dropped; original independent vectors restored.
// Round 7: graph looked up right after the state 805 (kept); with the final flags as a ternary 804;
// final test with its arms swapped 810; activity as a logical and 849; a fresh object pointer for the
// final test 810; definition first 810; definition before control 805; channel tests through a
// pointer helper 838; overlay ratio clamp as a ternary 805. Retail inlines the first 0x11df30 call
// and reads the projection input from the object in place.
// Round 26 (lane 9): ours inlines 0x1cb490 (and 0x1cb410 below it) in the overlay loop, where retail
// calls 0x1cb490 and inlines the first 0x11df30 instead (an inliner budget trade; 0x1cb490 can't be
// noinline: matched 0x105e80/0x1ccae0 inline it). Unused model-tag local for retail's dead entry
// lookup 805 (eliminated); ratio as 0.5f + x * 0.5f, x / 2.0f + 0.5f, (x + 1.0f) / 2.0f, 0.5f * x + 0.5f
// 805 each (all compile to (x + 1) * 0.5; retail multiplies first); overlay loop as a for 875 (3338
// bytes: the first 0x11df30 is then inlined as in retail, 0x1cb490 still inlined in the loop);
// `++i < 3` / short index 805/805; `active` declared at the top of the aiming block 805.
// Round 237: Native real bound fields/array-reference/screen-reference 805/805/805; screen
// value/native copy/typed copy/const copy 1003 each; native remote box fields 805. Original
// fields, helper inputs and outputs restored.
// Complementary activity across both aim stages, bool/byte/signed-byte/short at the
// original declaration 850/850/850/824; constant entry initialization 850/850/850/836.
// Original activity, input snapshots, helper boundaries and all smoothing values restored.
// Round 210: Animation-ID fields at every use: overlay/primary pair/secondary 867/863/833;
// overlay-primary/overlay-secondary/primary-secondary/all 879/857/856/874;
// graph at every use 873. Original ID ownership and all executable source restored.
// Round 380: on the complete-direction/early-graph body, Boolean definition bit 11/10/both
// 835/838/830; early two-use centered bool alone/with bit 11/with bit 10/both
// 858/854/859/790; combined byte predicate 790. Keep the Boolean combined form,
// plain confirmed at 790 (3386/3403); original floating grouping and all call roles retained.
// Round 258: Eight consistent basis/output formal orders at the existing projection boundary all
// 790. Original helper contract and both calls restored; frame 0xcc versus 0xd4 and first
// conversion expansion remain.
// Round 386: shared projected-vector setter/direction memcpy/both 790/790/790;
// default-angle memcpy alone/with setter/with direction/all 856/856/856/856;
// all with ordinary projection inline 1165. Original bodies and copies restored.
// Round 168: Native unit direction/ratio/weight fields in seven subsets and all with side bytes
// 790 each; on the newer flag/center graph, plain overlay for 882, with native default angles
// 911, with conditional activity clear 901, with both 897. Original body and records restored.
// Round 184: Complete aiming flag word/native tag value/encoded byte/separate predicates at entry
// 855/836/877/857; after persistent samples 848/880/867/850. Original live flags, native direction
// copy, helpers, arithmetic and call roles restored.
// Round 393: native external sample-channel references in seven subsets
// 793/793/839/790/793/793/839; all with scratch construction at first use 841. Original body restored.
// Round 172: complete bounds value return, field construction with pointer/reference/value
// screen inputs 1027/1027/1123; aggregate construction 1027/1027/1123;
// ordinary inline field/aggregate returns 1010/1010. Frame 0xcc remains;
// original bounds helper, direction snapshot and all consumers restored.
// Round 400: Actual bounds outputs as four-real reference/pointer, native interval pair and
// separate yaw/pitch intervals 790/790/790/790; each output-first
// 790/790/790/790. Original output contract and every arithmetic tree restored.
// Round 269: Existing projection boundary includes the angle-conversion call,
// caller pointer/reference, private vector and native angle value: forced
// 928/928/961/933; ordinary inline 1180/1180/1184/1230. Original two helpers,
// separate conversion calls, direction snapshot and complete body restored.
// DL176: Independent ID/ratio cursors and paired indexed traversal 882/882/903; ID-end with
// indexed ratio 897; paired ID-end/ratio-end 889/861; paired short/word ordinal 903/903. Original
// indexed do-loop, all input observations, helpers and complete body restored; frame and first-
// angle/overlay expansion remain.
// Staged side cross, overlap-safe direction, both, overlap-safe default angles, side/angles,
// direction/angles, all, then all with independent secondary projection: 820/842/824/891/847/892/869/882.
// Original body, complete input capture, helper and includes restored.
// Round 81: retail calls 0x11df30 once (its first use inlined), ours twice; every other callee's count
// agrees. The whole body in a helper the stdcall entry calls: static inline 4236 (kept out of line),
// forceinline 790 (same code), inline 4236; only the part after the overlay-flag clear in a helper:
// static inline 3727, forceinline 913.
// Round 83: the final slot test inverted with its arms swapped 790 (same code; retail lays the then-arm
// first); the relative angles declared with the defaults 790; the scratch channel declared first 801; the
// two aiming channels at the aiming block's top 789; the graph looked up after the definition 795.
// Round 85: the body in a forceinline helper taking the render model as an unused parameter 790 (same
// code); the overlay loop as `while` 790 (same code), as `for` with the play call under its own `if` 882,
// the play call under its own `if` in the do-loop 790 (same code), a `for` that skips empty ids with
// `continue` 882.
// Round 209: Main/blend-pair/all native channel value construction 790/790/790; immutable repeated
// curve values alone/main/pair/all 790 each; all with immutable animation IDs 790. Complete
// comparison listings unchanged. Original constructors, helpers, inputs and float grouping
// restored; frame 0xcc versus 0xd4 and first-angle/overlay expansion remain.
// Round 243: Complete two-use center predicate byte/short/long/dword storage
// 790/848/786/786; complementary bool/byte/short/long 844/844/836/831. Long/dword gain
// only four at 3392 bytes and are dropped. Original predicate, helper and complete body restored.
// Round 115 (lane 7, fast; aligned = difflib count, 584 at 790): the overlay ratio as the product then
// `ratio += 0.5f` 859 (retail's mulss/addss on one 0.5 register, aligned 577); the overlay `for` loop
// re-measured 882 (first 0x11df30 inlined as in retail, frame 0xc8), with that ratio 867; the projection
// reading its input in place 833 (3403 bytes, aligned 573), with the for loop 922, with the ratio 848; the
// projection's copy written field by field 911 (aligned 561), with the for loop 856 (aligned 542: the
// projection is then SSE in place as in retail), copy orders k,j,i / j,k,i with the for loop 859/857.
// Retail's frame also holds unreferenced bytes at +0x28, +0x70 and +0x98..+0xa3 (after the basis vectors).
// Round 273: Repeated native slot receivers 4/3/0, pairs 4-3/4-0/3-0, all and
// all declared at entry 790/796/793/796/793/799/799/796. Original receiver expressions,
// native direction copy, helpers and body restored; frame 0xcc versus 0xd4 remains.
// Round 305: paired pitch/yaw comparison values, bool logical/bitwise in both orders
// 854/828/824/825; byte equivalents 854/828/825/826. Original combined predicate,
// complete direction capture, helpers and body restored; frame and first-angle expansion remain.
// AC460: Both blend products inside the positive-weight admission, canonical activity
// bool/byte/short/long 881/881/857/861; both products inside the two-screen admission
// 881/881/857/861. Original product lifetimes, activity and complete body restored.
// M292: Exhaustive nonempty screen cases both/first, both/second, first/both, second/both, with
// original/direct bounds fallback:
// 853/886/859/858/856/894/861/861. Original executable body restored.
// Y522: Primary/secondary converter-output transfer modes 01/02/10/11/12/20/21/22
// 790/824/840/840/846/838/838/849; complete native component transfers and returned-value snapshots.
// Mixed primary transfer/secondary value reaches frame 0xd4 at 846, first +0x23, but regresses.
// Original escaped angle outputs, default values, helpers and complete body restored.
// Round 137 (m3 lane 6, fast control 790 = plain): ours calls 0x11df30 twice where retail inlines the first (two
// fpatan) and calls the second; every other callee count agrees. bounds_112070 as plain inline 1064 (3171 bytes,
// kept out of line), project_112070 as plain inline 1146, both 1378; a `__forceinline` redeclaration of 0x11df30
// 790 (still a call); the projection reading its input in place (no copy) 833 (3403 bytes, both 0x11df30 still calls).
// DH304: Existing bounds producer returns the following eligibility query, bool/byte/long/dword with forced/ordinary inline, 840/809/809/809/1057/1050/1050/1050.
// Original helper contract and complete body restored; frame 0xcc/0xd4 and first converter expansion remain.
// N333: Common terminal sample from start/set result, bool/byte/short/long,
// entry false/guarded calls 773/773/779/777; branch-owned results 793/793/793/793.
// NOT kept: one-use call-result/conditional-state locals and a retained branch test require a match.
// The 773 previews recover retail length only; original body restored; frame/expansion remain.
// Round 62 (m3 lane 10): post-centre activity as `if (...) active = false;` 856; bounds fallback as one
// call on `screen_a ? screen_a : screen_b` 912; each aiming flag set before its angle conversion 790/792;
// seat weight test as nested ifs 790. Original body restored.
// Round 82 (m3 lane 12, fast): centred test i first 790 (same code); overlay id copied inside a
// `control->overlays[i].index != NONE` test 875 (3357 B: 0x1cb490 then stays out of line in the loop as in
// retail, 0x11df30 still called twice), that with a `for` loop 905 (3305 B); final slot test through the
// entry unit pointer 815 (3365 B); seat weight as one ternary 792 (3382 B). Original body restored.
// Round 39 (m3 lane 18, fast): the final animation_10c start through the existing channel_play in place
// of its body written out 776 (3418 B, kept); before it: definition and render model in a local struct 802
// (frame 0xc8, the store still dropped); overlay loop without the id local 872; angles copied whole from
// g_4687c4 856; definition looked up right after the unit 795. On 776: id copied inside the index test
// 842; final slot test inverted with arms swapped 776 (same code); overlay loop as `for` 869 (first
// 0x11df30 inlined as retail, frame 0xc8); `for` with the id inside the test 873; ratio `0.5f + 0.5f * x` 776.
// Round 209 (m3 lane 3, fast; masked = difflib count with branch targets masked, baseline 423): retail's
// call list differs from ours only in the first 0x11df30 (inlined in retail) and the overlay loop's
// channel_play (inlined two levels deep in ours, a call in retail); retail's dead entry store of the render
// model's tag shares its frame slot with the overlay loop's `id`, whose own store is dead too. Tried: `id`
// declared before the loop 776 (same code); named unit fields for +0x168/+0x18c/+0x208/+0x212/+0x288 776
// (same code); `for` loop with the id copied inside the index test and the projection reading its input in
// place 928 (3312 B, frame 0xb8, masked 420: the first 0x11df30 inlines but the loop's channel_play stays
// inlined); the aiming channels renamed to sort after `channel` 776 (same code); both vector2f values
// declared together after the basis vectors 776 (same code).
// Y565: Existing channel_start at each direct set site, early/late pairs/all, then all with
// the original graph guard: 775/775/775/776/776/775/775/776. NOT kept: at most one
// point; frame stays 0xcc and first-angle/overlay expansion remains. Original body restored.
// F357: Complete persistent-channel admission results in bool/byte/long/dword:
// logical conjunctions 776/776/776/776; eager conjunctions 823/823/808/808.
// NOT kept: no gain; original direct guards, scratch lifetime, helpers and samples restored.
// B398: Overlay lower/upper independent clamps, upper-first exclusive/independent
// clamps 776/841/841; complete overlay center-admission bool/byte/short/long/dword
// storage 776 each. NOT kept: no gain or exact predicate/branch-layout form.
// Original body restored; frame 0xcc/0xd4 and first-angle/overlay expansion remain.
// F378: Secondary relative-sample eligibility after positive-weight admission, sample then skip
// bool/byte/short/long: 807/807/807/807/776/776/776/776. NOT kept: one-use decisions remain unmatched;
// original projection captures, angle tests, helpers and complete body restored.
// P326: Existing primary blend scalars/secondary weight/overlay ratio declarations at their
// owning phase, subsets 1..7, then all at entry: 776 each. Assignments stayed at their
// producers. NOT kept: no gain; original body restored, frame and first-angle expansion remain.
// N368: Nested persistent-valid/direct-set/default-and-overlay admissions, seven family
// subsets, then all with split outer definition/graph guard: 776 each. NOT kept: no gain;
// original admission graph restored, frame 0xcc/0xd4 and opposite selective expansion remain.
// Y597: Complete common primary sampling weights, both-first/single-first producers,
// sample-admission bool/byte/short/long: 860/860/834/837/869/869/842/851.
// NOT kept: no exact common-sample graph; original weights, decisions and complete body restored.
// AC533: Test-produced whole terminal paired-slot gate, missing/present bool/byte/short/long:
// 724 each (3413/3403; frame 0xcc). NOT kept: one-use/test-produced state needs equality;
// first-angle/overlay calls stay 2/4 versus native 1/5. Original complete body restored.
// N373: Ordered primary/secondary null-or-found screen producers, conditional/default/total-if forms
// 830/775/835/830/775/835/835/835. NOT kept: at most one point; original full body restored.
// Frame 0xcc/0xd4, entry computation and first-angle/overlay expansion remain.
// DL265: Existing set/sample/play inline policy subsets, then aiming-sample inline:
// 776/776/776/776/776/776/776/776. NOT kept: no qualifying provider contract; every provider/header restored.
// Frame 0xcc/0xd4 and first-angle/overlay expansion remain.
// Y597: Complete common primary sampling weights, both-first/single-first producers,
// sample-admission bool/byte/short/long: 860/860/834/837/869/869/842/851.
// NOT kept: no exact common-sample graph; original weights, decisions and complete body restored.
// P350: Complete unit/state/graph/control/definition lexical home orders, eight forms
// 776 each. NOT kept: no native frame or first-angle/overlay expansion repair; original body restored.
// AC552: Existing shared real clamp at the overlay-ratio producer, long/short/dword/word
// ordinals with ordinary/inline declaration: eight at 776. NOT kept: no gain; original
// direct clamp, ordinal, declarations and complete body restored; frame/expansion gaps remain.
// F395: All four native bound results before publication, mutable/immutable with yaw-first,
// pitch-first and two interleaved producer orders: 829/828/828/829/829/828/828/829.
// NOT kept: every form regresses; 3403-byte length agrees but frame stays 0xcc/0xd4.
// Original private helper and full body restored; first-angle/overlay expansion remains.
// b2B (11-06) walk against retail, all 849 instructions: no behaviour difference (fields, conditions, callees and
// arguments agree). Still differing: frame 0xcc versus 0xd4; retail inlines the first 0x11df30 and calls channel_play
// out of line; the x*0.5f+0.5f operand order. Score 776, unchanged.
// Round 163 (m3 lane 6, fast; aligned/masked = difflib counts with addresses, or also registers and stack offsets,
// masked: control 587/288): in the overlay loop ours inlines channel_play and channel_start (a direct set call)
// where retail calls channel_play; retail's dead entry store of the model's graph and the loop's dead `id` store
// share one slot (+0x20), ours stores `id` at +0x44. The overlay loop moved into a forceinline helper 776 (590/288,
// same calls); the ratio as `x * 0.5f` then `ratio += 0.5f` 847 (3414 B, 580/281: retail's mulss/addss on one 0.5
// register and its merged clamp store, rest shifted); `active` declared at function scope 776 (same code); the
// direction vectors declared at function scope 776 (same code); the id copied inside the index test with that
// ratio 856 (3385 B, 581/280; the loop still inlines both levels in the reduced build). The seat weight as an if/else
// 770 (3414 B; six points); the two relative-angle subtractions j first 773 (a statement order, not kept); the 0x114
// animation read from the control at both uses 843 (3425 B); the relative centred test j first 775; the final flags
// as an if/else 788 (3388 B, masked 250, tail layout unchanged); the default overlay's play call under its own if
// 776 (same code); the side vector through the existing cross3f 863 (3441 B); the default angles copied j first 776;
// the 0x20 flag set before the projection 776. All restored.
// Round 149 (m3 lane 8, fast): retail tests channels 0x9c/0xbc through the channel pointer (`lea eax, [ebp + 0x9c]`,
// then `cmp word ptr [eax + 6]`); the file's channel test function_111651 at the four channel tests gives that 819
// (3405 bytes; the local channel's test stays a call), with a `__forceinline` declaration of it 823 (3414 bytes), pointer
// locals for the first two channels 823 (3414 bytes, register-masked aligned 288 -> 286); the side vector through the
// existing cross3f 863 (3441 bytes); on round 39's `for` loop with the id inside the index test (re-measured 873:
// 41 calls and two fpatan as retail, frame 0xc4, 3337 bytes, but channel_play still expanded two levels in the loop and
// at the final start, where retail calls 0x1cb490 and 0x1cb410) the forced channel test 902 (3324 bytes) and retail's
// dead render-model lookup as an unused local 873 (eliminated). Re-measured: the id inside the index test with the
// projection reading its input in place 833 (3403 bytes, the in-place form's listed score; the loop's channel_play stays
// expanded). The function_111651 form may repeat round 7's pointer helper (838 then). The two blend products computed
// after the two aiming channel_play calls 769 at retail's 3403 bytes (length term only: differing lines 761 -> 769,
// aligned 288 -> 296); on it: the in-place projection 838 (3409), channel pointer locals 822 (3390), the cross3f side
// 837 (3417), the products inside the weight test 847 (3391), the forced channel test 822 (3390).
// Round 168 (m3 lane 5, fast then plain; masked = difflib count with registers and stack offsets masked, 162 at
// 776): the seat-weight block as two sample calls in an if/else (retail duplicates the push setup in both arms and
// cross-jumps them) with the overlay ratio as `x * 0.5f` then `ratio += 0.5f` (retail's mulss/addss on one 0.5
// register) 762 at 3403 bytes, length term only (`!!` lines 761 -> 762, aligned 400 -> 405, masked 150, frame 0xc8;
// not kept); alone 809 (3404 B, masked 153) and 847 (3414 B, masked 157). The forms after "On 762:" were measured on it.
// The overlay body in a plain inline helper 975 (3264 B, the helper stays out of line). Diagnostic: with the loop's
// body disabled the first 0x11df30 is still a call (845), so the loop's expansion does not block it. On 762: the 0x114
// id copied before the combined test, the test on id.index 830 (3407 B, masked 148: retail tests the copy, `mov edi,
// [ebp+0x114]; shr; cmp; mov [esp+0x20], edi`); the 0x114 test nested (unit test outside, the id copied and tested
// inside) 829 (3405 B, masked 145); that with the conditional `active` clear 834 (3403 B, masked 146), also with the
// default angles as one aggregate copy 847 (3385 B, masked 135); the blend products with their operands swapped 762
// (same code). Re-measured on 762: the `for` loop 881 (3348 B), the aggregate default angles 836 (3389 B, masked 142),
// the in-place projection 836 (3407 B, masked 157), the conditional `active` clear 840 (3391 B, masked 145),
// function_111651 at the four channel tests 767 (masked 154). Masked keeps falling with the retail-shaped forms while
// the positional count rises; the loop's channel_play still expands two levels where retail calls 0x1cb490.
// Round 93 (m3 lane 10, fast; call counts named through the reduced image's map): one `c_type_709360 id` at function
// scope for the overlay loop and the 0x114 block (retail stores both ids and its dead entry lookup to +0x20) 776 (only XMM
// choices move), at the top of the 0xe0000c2 block 776 (same code); a named `real const *ratios` base for the +0x288 reads
// 847 (3391 B). Re-measured: the do-loop with the id copied inside the index test 842 (3389 B; the loop's channel_play
// still inlined: set x5, channel_play x4, 0x11df30 x2); the `for` loop with it 873 (3337 B; the first 0x11df30 inlines,
// two fpatan, but the loop's call stays inlined two levels down to set). Retail also reloads `transforms` from its home on
// every loop pass, where ours hoists it into ESI, the register retail gives the ratio cursor. No other untried natural
// form was found for these two causes: no existing helper covers the twice-written `function_1d9240(slot, active, active ?
// 0.267f : 0.1335f)` call, the slot-flag-then-weight reads or the two-axis centred test (grep of include/ and this file).
// Round 77 (m3 lane 15, fast; brackets: aligned/masked difflib counts, control [587/288]): the definition through the
// typed unit definition (`unit.flags`) 776 (same code; the file's own view kept); the final slot test through
// unit_weapon_view_get 776 (same code, round 209's re-measured); the seat-weight update as an if/else with a constant
// per arm 833 (3403 B) [624/302]; the two screen pointers as ternaries 835 (3403 B) [643/293]; the bounds call's `else
// if (screen_b)` as a plain `else` 835 (3408 B) [596/298]; `wa < 0.0001f` and `wb < 0.0001f` for the two `0.0001f >`
// tests 776; re-measured `0.5f * x + 0.5f` 776 (same code). Body unchanged.
// MiniMax B (11-13): no new form, no lower score (776): the frame is retail's 0xd4 against our 0xcc and the first angle
// expansion still differs; the forms listed above are exhausted, and the retail-shaped ones measured before either
// restructured the function or left the score unchanged.
// @retail 0x112070
void __stdcall function_112070(long unit_index, dword const *node_mask, long node_count,
	real_quaternion_transform *transforms)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_graph_tag *graph = GRAPH_GET(state->graph_tag_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	c_animation_channel channel;
	if (control->channel_9c.graph_tag_index != NONE && control->channel_9c.animation_id.index != NONE)
		control->channel_9c.sample(1.0f, node_mask, node_count, transforms);
	if (control->channel_bc.graph_tag_index != NONE && control->channel_bc.animation_id.index != NONE)
		control->channel_bc.sample(1.0f, node_mask, node_count, transforms);
	if (control->channel_dc.graph_tag_index != NONE && control->channel_dc.animation_id.index != NONE)
	{
		s_unit_seat_definition *seat = function_c8fc0(unit_index);
		real weight = 1.0f;
		if (unit_animation_control_channel_playing(unit_index) && seat && *(real *)((byte *)seat + 0x1c) > 0.0f)
			weight = *(real *)((byte *)seat + 0x1c);
		control->channel_dc.sample(weight, node_mask, node_count, transforms);
	}
	if (control->animation_104.index != NONE && state->graph_tag_index != NONE &&
		channel.set(state->graph_tag_index, 0x400, control->animation_104, NONE, NONE, NONE, NONE))
		channel.sample(1.0f, node_mask, node_count, transforms);
	if (control->animation_108.index != NONE && state->graph_tag_index != NONE &&
		channel.set(state->graph_tag_index, 0x800, control->animation_108, NONE, NONE, NONE, NONE))
		channel.sample(1.0f, node_mask, node_count, transforms);
	control->flags &= ~0x60;
	if (!(bool)((definition->flags >> 11) & 1) && state->graph_tag_index != NONE)
	{
		if (*(real *)((byte *)unit + 0x208) > 0.0f &&
			*(short *)((byte *)UNIT_ANIMATION_CONTROL(UNIT_ANIMATION_OBJECT(unit_index)) + 0x36) == NONE)
		{
			c_type_709360 id = state->overlay_default_get();
			if (id.index != NONE && state->channel_play(&channel, id, 0))
				channel.sample(*(real *)((byte *)unit + 0x208), node_mask, node_count, transforms);
		}
		if (state->unknown7c != 0xe0000c2)
		{
			if (TEST_FIELD_BIT(control->overlay))
			{
				long i = 0;
				do
				{
					c_type_709360 id = control->overlays[i];
					if (id.index != NONE)
					{
						real ratio = *(real *)((byte *)unit + 0x288 + i * 4) * 0.5f + 0.5f;
						if (ratio < 0.0f) ratio = 0.0f;
						else if (ratio > 1.0f) ratio = 1.0f;
						if (!(fabs(ratio - 0.5f) < 0.0001f) && state->channel_play(&channel, id, 0))
							channel.sample_ratio(ratio, 1.0f, node_count, transforms, node_mask);
					}
					i++;
				} while (i < 3);
			}
			if (!(bool)((definition->flags >> 10) & 1))
			{
				vector2f angles;
				angles.i = g_4687c4->x;
				angles.j = g_4687c4->y;
				vector3f forward, up, side, projected;
				function_b9fc0(unit_index, &forward, &up);
				side.i = forward.k * up.j - up.k * forward.j;
				side.j = up.k * forward.i - forward.k * up.i;
				side.k = forward.j * up.i - up.j * forward.i;
				c_type_709360 first = control->animation_fc;
				c_type_709360 second = control->animation_100;
				bool active = false;
				if (first.index != NONE || second.index != NONE)
				{
					s_aiming_screen const *screen_a = NULL;
					s_aiming_screen const *screen_b = NULL;
					if (first.index != NONE)
						screen_a = (s_aiming_screen const *)function_1daf30(graph, first);
					if (second.index != NONE)
						screen_b = (s_aiming_screen const *)function_1daf30(graph, second);
					if (screen_a || screen_b)
					{
						project_112070((vector3f *)((byte *)unit + 0x168), &forward, &side, &up, &projected);
						function_11df30(&angles, &projected);
						control->flags |= 0x20;
						if (screen_a) bounds_112070(screen_a, (real *)control->unknown04);
						else if (screen_b) bounds_112070(screen_b, (real *)control->unknown04);
						active = function_111590(unit_index, 4);
						active &= !((state->unknown7c == 0x700005c || state->unknown7c == 0x700005d) && !unit_animation_state_finished(state));
						bool angles_centered = fabs(angles.j) < 0.0001f && fabs(angles.i) < 0.0001f;
						if (control->slots[4].unknown1)
						{
							real weight = function_1d9370((s_1d9240 *)&control->slots[4]);
							real blend = function_1d9370((s_1d9240 *)&control->slots[2]);
							real wa = (1.0f - blend) * weight;
							real wb = blend * weight;
							c_animation_channel a, b;
							state->channel_play(&a, first, 0);
							state->channel_play(&b, second, 0);
							if (weight > 0.0001f)
							{
								if (screen_a && screen_b)
								{
									if (0.0001f > wa) wb = 1.0f;
									else
									{
										if (0.0001f > wb) { wa = 1.0f; wb = 0.0f; }
										if (wa > 0.0001f && (wa < 0.9999f || !(angles_centered)))
											a.sample_aiming(angles.i, angles.j, wa, node_mask, node_count, transforms);
									}
									if (wb > 0.0001f)
										b.sample_aiming(angles.i, angles.j, wb, node_mask, node_count, transforms);
								}
								else if (screen_a)
								{
									if (weight < 0.9999f || !(angles_centered))
										a.sample_aiming(angles.i, angles.j, weight, node_mask, node_count, transforms);
								}
								else if (screen_b)
									b.sample_aiming(angles.i, angles.j, weight, node_mask, node_count, transforms);
							}
						}
					}
				}
				function_1d9240((s_1d9240 *)&control->slots[4], active, active ? 0.267f : 0.1335f);
				active = false;
				if ((*(char *)((byte *)unit + 0x212) != NONE || unit->field_13c != NONE) && control->animation_114.index != NONE)
				{
					c_type_709360 id = control->animation_114;
					state->channel_play(&channel, id, 0);
					if (channel.graph_tag_index != NONE && channel.animation_id.index != NONE)
					{
						s_aiming_screen const *screen = (s_aiming_screen const *)function_1daf30(graph, id);
						if (screen)
						{
							project_112070((vector3f *)((byte *)unit + 0x18c), &forward, &side, &up, &projected);
							vector2f relative;
							function_11df30(&relative, &projected);
							control->flags |= 0x40;
							relative.i -= angles.i;
							relative.j -= angles.j;
							bounds_112070(screen, (real *)control->unknown14);
							active = function_111590(unit_index, 5);
							if (control->slots[3].unknown1)
							{
								real weight = function_1d9370((s_1d9240 *)&control->slots[3]);
								if (weight > 0.0001f && (weight < 0.9999f ||
									!(fabs(relative.i) < 0.0001f && fabs(relative.j) < 0.0001f)))
									channel.sample_aiming(relative.i, relative.j, weight, node_mask, node_count, transforms);
							}
						}
					}
				}
				function_1d9240((s_1d9240 *)&control->slots[3], active, active ? 0.267f : 0.1335f);
			}
		}
	}
	unit = UNIT_ANIMATION_OBJECT(unit_index);
	if (*(char *)((byte *)unit + 0x212) == NONE || *(char *)((byte *)unit + 0x213) == NONE)
	{
		if (control->animation_10c.index != NONE)
		{
			dword flags = 0;
			if (control->slots[0].unknown1 && function_1d9370((s_1d9240 *)&control->slots[0]) < 0.0001f)
				flags = 0x400;
			if (state->channel_play(&channel, control->animation_10c, flags))
				channel.sample(1.0f, node_mask, node_count, transforms);
		}
	}
	else
	{
		if (control->animation_10c.index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0x400, control->animation_10c, NONE, NONE, NONE, NONE))
			channel.sample(1.0f, node_mask, node_count, transforms);
		if (control->animation_110.index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0x800, control->animation_110, NONE, NONE, NONE, NONE))
			channel.sample(1.0f, node_mask, node_count, transforms);
	}
}

bool function_cd660(long unit_index);

// @retail 0x1130f0
void __stdcall function_1130f0(long unit_index, long node_count, transform4x3f *nodes)
{
	// Retail keeps the object argument on the stack.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (!(bool)((definition->flags >> 11) & 1) && state->graph_tag_index != NONE &&
		state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE)
	{
		bool parent = function_111490(unit_index);
		bool weapon = function_1112f0(unit_index);
		if (function_cd660(unit_index))
			weapon = false;
		function_1d9240((s_1d9240 *)&control->slots[1], parent, parent ? 0.267f : 0.06675f);
		function_1d9240((s_1d9240 *)&control->slots[0], weapon, weapon ? 0.267f : 0.06675f);
		function_113c60(unit_index, unit->parent_index, node_count, nodes);
		if (*(char *)((byte *)unit + 0x212) != NONE && state->unknown74 != NONE && state->unknown78 != NONE)
		{
			long held = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
			function_113ba0(unit_index, held, node_count, nodes);
			control->flag0 = false;
		}
	}
}

short function_20ba00(short index);
bool function_114520(long unit_index, short entry_index, short priority, long *previous, long *tag_index);
void function_114680(long unit_index, void const *request);

struct s_request_extra_114b60
{
	short marker;
	short unknown02;
	long object_index;
	long flags;
	byte unknown0c[0x1c - 0xc];
};

struct s_request_114b60
{
	short priority;
	short entry_index;
	long definition_index;
	short unknown08;
	short delay;
	s_sound_permutation_reference permutation;
	short unknown0e;
	long sound_index;
	s_request_extra_114b60 extra;
};

__declspec(noinline) bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra);

// score 16 (was 16): only the callee-saved roles differ (ours index ebx,
// NONE ebp, request priority esi; retail esi, ebx, ebp). Tried: the default
// extra fill as a shared inline helper 16; other parameter orders 16; the
// 0x114520 result in a named bool returned once 95; the fallback test joined
// to the 0x114520 test 22; a success flag set after the fallback block 96.
// Round 32: the definition index as its own local (priority then a register argument, frame
// 0x38) 46; entry_index reassigned in place of the index local 16; request priority as long 16;
// request priority declared before the index 16; NONE-first comparisons 16.
// Round 15: `priority = NONE;` before the fallback test 23; entry_index reassigned with the request
// priority declared first 16; the index from a ternary with one NONE test 16; the request record
// declared at function scope 16; the fallback test un-nested (a second plain NONE test) 16.
// Round 105: const priority capture 16; combined declarations 16; const final index 16; priority input through its
// own address 16; chained default NONE stores 16; output NONE between fallback and the common guard 22;
// entry/priority stores swapped 16; definition stored first 17. Original body restored.
// Round 135: the 26 combinations of two or more of: request priority declared first, entry_index
// reassigned, request priority as long, second NONE test un-nested, request record at function scope:
// 16 each. Extra NONE stores, diagnostic: 54, 59, 66 (251, 256, 261 bytes) with the roles unchanged, so
// the NONE register is whatever the index and the priority leave free.
// Round 199: retail's calls set EAX (entry) before EDX (fallback), so the declared order stands. An early
// `if (!0x114520) return false;` with the request block flat 98 (215 bytes); request priority captured after the
// fallback block 85 (252 bytes); the request record zero-initialised with `= {0}` 65 (248 bytes); the index
// declared then assigned after the priority 16; the index from an if/else on entry_index 16.
// Round 54 (m3 lane 15; one-link harness with src/unknown_1130a0.cpp and src/unknown_20ba00.cpp, same roles, 22 for the check's
// 16): the colouring order is the cause: the heaviest value takes EBX, the next ESI, the last EBP, and retail colours NONE first,
// ours index, priority, NONE. Diagnostics with extra volatile NONE stores in the request block: 2 no change, 4 NONE ESI and
// priority EBP, 8 retail's exact roles (index ESI, NONE EBX, priority EBP). Two more priority stores swap index and priority.
// Overwritten NONE stores do not count; removing the copy, the memset or the test of extra moves the roles but not to retail's.
// Real (fast): request setup through a forceinline helper 16; extra fill through one 16; both 16; entry_index tested and reassigned
// then index declared 16; index from a ternary 16; index set in both arms of a test of entry_index 16; entry store first 16;
// comparisons against `(short)NONE` 16; output cleared ahead of the fallback test 23; extra copied by struct assignment 16, through
// a typed pointer 16; memmove 51; memcpy with a literal size 16. Harness (22 base): the call and return in both arms 41, the call in
// both arms 41, the else as a fall-through 41, inverted arms 32; implied NONE tests in five places 22 to 28 (threaded, roles
// unchanged); `!= false` 22, `== true` 24, a named bool 22; an empty `do {} while (false);` at all 16 statement boundaries 22;
// every spelling of each NONE use (-1, typed casts, 0xffffffff) 22, word forms 83 to 88, a long index 31; index defaulting to NONE then set in
// both arms 22; 72 combinations of an extra local, `!= NULL`, declaration order, memset spelling and priority casts keep our roles, with a
// one-exit result 91 (224 bytes; index ESI, priority EBP, NONE EAX).
// A 600 s permuter run (52 tries) none below 16.
// Round 69 (m3 lane 14, fast): the fallback lookup assigned inside the NONE test's condition 16 (same code); the sound
// index stored before the delay 23, first after the memset 24; the definition index stored after the delay 44; the
// output written through a `long &` reference to the priority parameter 16 (same code).
// Round 69 (m3 lane 13, fast): the fallback result in its own short local copied to the index after its NONE test 17;
// the extra arms inverted (defaults under `!extra` first) 29; the request value-initialised (`= s_request_114b60()`)
// for the memset 105 (272 bytes, temporary plus copy); the default arm's NONE stores chained 16 (same code); the index
// from an if/else on entry_index with extra copied as an aggregate 16; the request cleared before the 0x114520 query 83
// (254 bytes); the extra filled before the common fields 85 (228 bytes).
// Round 99 (m3 lane 12, fast): a short priority parameter with a separate `long definition_index = NONE` out
// value (the out value would then reuse the priority's home, as retail's early priority load suggests) 46 (LTCG then
// passes the priority and entry in registers, frame 0x38); the sound index stored after the extra block 71 (223 bytes);
// the submit call written in both extra arms 48 (238 bytes); the 0x114520 test as `== true` 18; the entry-first
// forwarding function defined above this one 16 (same code).
// Round 163 (m3 lane 5, fast): the default fill wrapped in `do { } while (false)` 16 (same code); in a one-trip
// counted loop 16 (same code, the loop is gone before allocation); the fallback lookup joined with `||` around the
// whole body with one `return false` 16 (same code); request priority declared first re-measured 16 (its load only
// follows its register's push); request priority captured after the fallback block re-measured 85 (252 bytes).
// Diagnostic: the default fill inside a real loop on a run-time count 62 (256 bytes), roles unchanged, so the loop
// depth of the constant's uses does not raise its rank. Plain: the siblings' request idiom (0x1147e0, 0x114710:
// the three extra defaults stored unconditionally after the sound index) followed by `if (extra) memcpy` 73 (221
// bytes: the defaults stay unconditional, roles unchanged).
// Round 91 (m3 lane 11, fast): the delay computed into a local ahead of the clear 45; the delay stored first after the
// clear 44. Ours colours the NONE constant last (lighter than the index and the priority despite seven uses), so
// ours treats it as a rematerialisable constant where retail weighs it as a variable; no natural form found that
// gives a NONE-valued variable (a constant-only local is a reverted form). Every other statement, test and type
// form above is listed.
// Round 237 (m3 lane 3, fast): the out value cleared at the top, after or before the index copy, 23/23 (it is stored to
// the parameter's slot at once: an address-taken parameter is not kept in a register, and the roles stay); the sibling
// 0x1147e0's order (out value cleared, the fallback lookup without an early return, then `index != NONE && 0x114520`)
// 23, with the clear after the lookup 22, with the lookup as one conditional expression 22 (the implied test threads
// away, roles unchanged). Diagnostics (each drops or adds real work) for the colouring order, which is index, request
// priority, NONE by weight, the first taking EBX, the second ESI, the third EBP: without the request's priority store
// the priority falls to third (index EBX, NONE ESI, priority EBP, loaded first as in retail) 63; without the entry
// store the index stays in AX and NONE takes ESI 84; without both NONE takes EBX 88; two more NONE stores 38 (no
// change of roles), four more 72 (NONE second). So NONE sits just under the priority here and would need the weight of
// about eight more stores to pass the index, as retail's order (NONE, index, priority) needs.
// @retail 0x114b60
bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra)
{
	short index = entry_index;
	short request_priority = (short)priority;
	if (index == NONE)
	{
		index = function_20ba00(fallback_index);
		if (index == NONE)
			return false;
	}
	priority = NONE;
	if (function_114520(unit_index, index, request_priority, NULL, &priority))
	{
		s_request_114b60 request;
		memset(&request, 0, sizeof(request));
		request.priority = request_priority;
		request.entry_index = index;
		request.definition_index = priority;
		request.delay = duration_ticks_1145f0(0.25f);
		request.sound_index = NONE;
		if (extra)
		{
			memcpy(&request.extra, extra, sizeof(request.extra));
		}
		else
		{
			request.extra.marker = NONE;
			request.extra.object_index = NONE;
			request.extra.flags = 0;
		}
		function_114680(unit_index, &request);
		return true;
	}
	return false;
}

/* the same request with the entry first, for callers written in that order */
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra)
{
	return unit_submit_entry_request(fallback_index, entry_index, unit_index, priority, extra);
}

real function_259a0(dword *seed);
void function_c96b0(long unit_index, long *result, real a, real b);

// @retail 0x1147e0
bool function_1147e0(long unit_index, bool a, real b, real c, long definition_index, bool hard)
{
	bool result = false;
	char outside = 0;
	short priority;
	short entry;
	long tag_index;
	bool local_0 = hard;
	bool local_1 = a;
	// Retail preserves the object argument on the stack throughout the request.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	byte *state = (byte *)unit + *(short *)((byte *)unit + 0x342);
	short type = 0;
	bool special = false;
	if (definition_index != NONE)
	{
		byte *definition = TAG_BYTES(definition_index);
		type = *(short *)(definition + 0x12);
		special = (*(dword *)(definition + 0x14) >> 7) & 1;
	}
	short selection;
	if (local_0)
	{
		long actor_index = *(long *)((byte *)unit + 0x12c);
		bool strong = false;
		if (actor_index != NONE)
			strong = *(short *)((byte *)g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x86) >= 5;
		else if (*(real *)((byte *)unit + 0x100) > b + 0.2f)
			strong = true;
		if (type == 7 || special)
			selection = 3;
		else if (type == 1)
			selection = g_4e6948->mode != 4;
		else if (type == 3 || type == 4 || type == 9)
			selection = 2;
		else if (!strong && type == 6)
			selection = 5;
		else
			selection = local_1 ? 4 : 0;
	}
	else
	{
		if (*(short *)(state + 0x42))
			goto local_2;
		local_1 = *(real *)((byte *)unit + 0x100) > 0.0f;
		long range;
		function_c96b0(unit_index, &range, c, b);
		outside = range == 2;
		if (type == 1)
			selection = 11;
		else if (outside)
		{
			if (local_1)
				selection = function_259a0(&g_4e7408->unknown0) < 0.6667f ? 8 : 9;
			else
				selection = 10;
		}
		else
		{
			if (*(short *)(state + 0x40) || *(short *)(state + 0x3e) >= 3)
				goto local_2;
			if (*(short *)(state + 0xc) && !(function_259a0(&g_4e7408->unknown0) < 0.4f))
				goto local_2;
			selection = local_1 ? 7 : 10;
		}
	}
	tag_index = NONE;
	priority = local_0 ? 15 : 12;
	entry = function_20ba00(selection);
	if (entry != NONE && function_114520(unit_index, entry, priority, NULL, &tag_index))
	{
		s_request_114b60 request;
		memset(&request, 0, sizeof(request));
		request.priority = priority;
		request.entry_index = entry;
		request.definition_index = tag_index;
		request.delay = duration_ticks_1145f0(0.25f);
		request.sound_index = NONE;
		request.extra.marker = NONE;
		request.extra.object_index = NONE;
		request.extra.flags = 0;
		function_114680(unit_index, &request);
		result = true;
		if (outside)
			*(short *)(state + 0x42) = duration_ticks_1145f0(2.0f);
		else
		{
			*(short *)(state + 0x40) = duration_ticks_1145f0(0.5f);
			*(short *)(state + 0x3c) = duration_ticks_1145f0(1.5f);
			if (++*(short *)(state + 0x3e) == 3)
				*(short *)(state + 0x40) = duration_ticks_1145f0(2.0f);
		}
	}
local_2:
	return result;
}


void function_20d220(long record_index, long object_index, short type, long const *target);

// @retail 0x114710
void __stdcall function_114710(long arg_0, long arg_1, long arg_2)
{
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    byte *local_1 = (byte *)local_0 + *(short *)((byte *)local_0 + 0x342);
    s_request_114b60 local_2;
    memset(&local_2, 0, sizeof(local_2));
    local_2.priority = 14;
    local_2.entry_index = NONE;
    local_2.definition_index = arg_1;
    local_2.delay = duration_ticks_1145f0(0.8f);
    local_2.sound_index = NONE;
    local_2.extra.marker = NONE;
    local_2.extra.object_index = NONE;
    local_2.extra.flags = 0;
    function_114680(arg_0, &local_2);
    if (*(long *)(local_1 + 0x10) == arg_1)
    {
        *(long *)(local_1 + 0x54) = arg_2;
        local_1[0x48] = 1;
        function_20d220(NONE, arg_0, NONE, (long const *)(local_1 + 0x20));
    }
}

void (__stdcall *g_467a2c)(long, long, long) = function_114710;


long players_first_active_local_player();
__declspec(noinline) s_player_state *function_16f3a0(long arg_0);
bool function_0bfe60(dword const *arg_0, long arg_1);

// @retail 0x1132f0
void function_1132f0(long arg_0, dword const *arg_1, long arg_2, transform4x3f *arg_3)
{
    (void)&arg_1;
    (void)&arg_2;
    (void)&arg_3;
    if (function_113260(arg_0))
    {
        s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
        point3f const *local_1;
        union
        {
            long field_0;
            char field_4[4];
        } local_6;
        local_6.field_0 = local_0->flags148;
        if ((local_6.field_4[1] & (char)0x80) != 0)
            local_1 = (point3f *)((byte *)local_0 + 0x1a4);
        else
        {
            if (!(local_0->flags137 & 1))
                return;
            long local_2 = players_first_active_local_player();
            if (local_2 == NONE)
                return;
            local_1 = (point3f const *)function_16f3a0(local_2);
        }
        if (local_1)
        {
            s_object_marker local_3;
            if (function_b8d30(arg_0, 0x4000095, &local_3, 1, false) > 0)
            {
                char local_4 = local_0->index1f6;
                if (local_4 >= 0 && local_4 < arg_2 && (!arg_1 || function_0bfe60(arg_1, local_4)))
                    function_112e20(&arg_3[local_4], local_1, &local_3.matrix);
                char local_5 = local_0->index1f7;
                if (local_5 >= 0 && local_5 < arg_2 && (!arg_1 || function_0bfe60(arg_1, local_5)))
                    function_112e20(&arg_3[local_5], local_1, &local_3.matrix);
            }
        }
    }
}

struct s_sound_label_play;
struct s_114c60
{
    long field_0;
    long field_4;
    real field_8;
    void const *field_c;
};

void sound_choose_permutation(long arg_0, s_sound_permutation_reference *arg_1, bool *arg_2);
long function_1891d0(long arg_0, long arg_1, s_sound_label_play const *arg_2);
void __stdcall function_20d570(short arg_1, long arg_0, long arg_2, void *arg_3);
void function_20fd50(long arg_0, long arg_1);

// @retail 0x114c60
void function_114c60(long arg_0)
{
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    byte *local_1 = (byte *)local_0 + *(short *)((byte *)local_0 + 0x342);
    short local_2 = *(short *)(local_1 + 0x3c);
    if ((short)local_2 > 0)
    {
        local_2--;
        *(short *)(local_1 + 0x3c) = (short)local_2;
        if (!(short)local_2)
        {
            short local_3 = *(short *)(local_1 + 0x3e);
            if ((short)local_3 > 0)
            {
                *(short *)(local_1 + 0x3e) = (short)(local_3 - 1);
                *(short *)(local_1 + 0x3c) = duration_ticks_1145f0(1.5f);
            }
        }
    }
    short local_4 = *(short *)(local_1 + 0x42);
    if ((short)local_4 > 0)
        *(short *)(local_1 + 0x42) = (short)(local_4 - 1);
    short local_5 = *(short *)(local_1 + 0x40);
    if ((short)local_5 > 0)
        *(short *)(local_1 + 0x40) = (short)(local_5 - 1);
    if (*(short *)(local_1 + 0xc) > 0)
    {
        if (!local_1[0x48])
        {
            long local_6 = *(long *)(local_1 + 0x10);
            if (local_6 != NONE)
            {
                long local_7 = *(long *)(local_1 + 8);
                if (*(long *)(local_1 + 0x1c) == NONE)
                    sound_choose_permutation(local_6, (s_sound_permutation_reference *)(local_1 + 0x18), NULL);
                s_114c60 local_8;
                local_8.field_4 = *(long *)(local_1 + 0x10);
                local_8.field_8 = 1.0f;
                local_8.field_0 = local_7;
                local_8.field_c = local_1 + 0x18;
                *(long *)(local_1 + 0x54) = function_1891d0(arg_0, 0x4000095, (s_sound_label_play const *)&local_8);
            }
            function_20d220(*(long *)(local_1 + 0x1c), arg_0, *(word *)(local_1 + 0xe), (long const *)(local_1 + 0x20));
            local_1[0x48] = true;
        }
        short local_9 = *(short *)(local_1 + 0x4e);
        if ((short)local_9 > 0)
            *(short *)(local_1 + 0x4e) = (short)(local_9 - 1);
        short local_10 = *(short *)(local_1 + 0x4c);
        if ((short)local_10 > 0)
        {
            local_10--;
            *(short *)(local_1 + 0x4c) = (short)local_10;
            if (!(short)local_10)
                *(long *)(local_1 + 0x54) = NONE;
        }
        else
        {
            if (!local_1[0x4a])
                local_1[0x4a] = true;
            short local_11 = *(short *)(local_1 + 0x50);
            if ((short)local_11 > 0)
                *(short *)(local_1 + 0x50) = (short)(local_11 - 1);
        }
    }
    if (*(short *)(local_1 + 0xc) > 0 && !*(short *)(local_1 + 0x4e) && !local_1[0x49])
    {
        function_20d570(*(short *)(local_1 + 0xe), *(long *)(local_1 + 0x1c), arg_0, local_1 + 0x20);
        local_1[0x49] = true;
    }
    if (*(short *)(local_1 + 0xc) > 0 && !*(short *)(local_1 + 0x4c) && !*(short *)(local_1 + 0x50))
    {
        if (!local_1[0x49])
        {
            function_20d570(*(short *)(local_1 + 0xe), *(long *)(local_1 + 0x1c), arg_0, local_1 + 0x20);
            local_1[0x49] = true;
        }
        if (*(long *)(local_1 + 0x1c) != NONE)
            function_20fd50(arg_0, *(long *)(local_1 + 0x1c));
        *(short *)(local_1 + 0xc) = 0;
    }
    real local_12 = 0.0f - *(real *)((byte *)local_0 + 0x208);
    if (-0.1f > local_12)
        local_12 = -0.1f;
    else if (local_12 > 0.1f)
        local_12 = 0.1f;
    *(real *)((byte *)local_0 + 0x208) += local_12;
}

struct s_bit_owner;
long function_10eef0(long arg_0, bool arg_1, bool arg_2);
void __stdcall function_bf600(long arg_0, real arg_1, s_animation_frame_event const *arg_2);
void function_e6a20(long arg_0, long arg_1, bool arg_2, bool arg_3);
void function_e6b00(long arg_0, long arg_1, long *arg_2, long *arg_3);
bool function_bfc90(long arg_0);
void c_animation_channel_data_get(c_animation_channel const *arg_0, s_animation_data *arg_1);
bool function_10eea0(s_bit_owner *arg_0, short arg_1);
byte *function_10ee70(s_bit_owner *arg_0);
long bit_vector_count_bits(dword const *arg_0, long arg_1);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
void c_animation_channel_node_position_get(c_animation_channel const *arg_0, point3f *arg_1, real arg_2, short arg_3);
point3f *transform4x3f_apply_point(transform4x3f const *arg_0, point3f const *arg_1, point3f *arg_2);
transform4x3f *function_b8bd0(long arg_0, short arg_1);
void function_e4300(long arg_0);
void function_e5840(long arg_0, vector3f const *arg_1, bool arg_2);
long function_10f5f0(long arg_0);
bool function_a7670(long arg_0);
void __stdcall function_d6bc0(long arg_0);
void function_1060a0(long arg_0, long arg_1);
void function_ba350(long arg_0, real arg_1);
bool function_10dc70(long arg_0);
bool function_10eaf0(long arg_0);
long g_55e70c;

PRIVATE __forceinline bool function_111651(c_animation_channel const *arg_0)
{
    return arg_0->graph_tag_index != NONE && arg_0->animation_id.index != NONE;
}

// score 557 (was 566): earlier 566 (was 621), earlier 511 (was 670; 713 before round 92): frame 0x7c versus native 0x80; request/control/tag homes,
// request-byte normalization and selected validity expansion remain. Both arguments stay on the stack; ret 8 agrees.
// Earlier 666 (was 666): this round found no gain - the request pointer's three stack re-reads still need a
// pointer-to-parameter or volatile spelling the lane rules do not allow, and the const-reference spelling
// costs both callers in other lanes' files.
// Earlier 666 (was 666): this round found no gain - the request pointer's three stack re-reads need a
// pointer-to-parameter or volatile spelling the lane rules do not allow, and the const-reference spelling
// (663) costs both callers in other lanes' files.
// MiniMax R (10-31): no new form, no lower score.
// Earlier (666): this round found no gain - retail re-reads the request pointer from the stack three
// times and never registers it, spills the control pointer to [esp+0x24] and keeps the state in EBP, while
// ours caches the request pointer in EBP and the state in ESI, giving a 0x88 frame; spelling the argument as
// a const reference gains 3 but costs the two callers, so it was dropped.
// Earlier (666): Frame 0x80 agrees; request/control homes, validity expansion and ret 4 versus ret 8 remain.
// Earlier: frame 0x80 agrees; request/control homes, selected validity calls and ret 4 versus ret 8 differ.
// frame 0x80 agrees; request/control homes, selected validity calls and ret 4 versus ret 8 differ.
// Round 55: retail never enregisters the request pointer (esi holds the control pointer, spilled to
// +0x24) and copies the request bools at +8/+9 as plain bytes; its transition flag is a byte (+0x11).
// Request bools read as bool 707 (2545 bytes); with a bool transition flag 723 (frame 0x7c, but aligned
// diffs 496 -> 383); bool transition flag alone 710 (aligned 386); declared after the result 709; with
// only the +9 bool read 742. Retail's frame keeps a dword at +0x18 that ours does not.
// Earlier at 666 (was 666): Frame 0x80 agrees; request/control homes, selected validity calls and ret 4 versus ret 8 remain.
// frame 0x80 agrees; request/control homes and ret 4 versus ret 8 differ;
// ordinary validity inline 679; native final/override/validity 688/712/699; depth one 707;
// requested-name snapshots 709; inline plus native final/validity 696/707.
// ordinary-helper Boolean transition 712; native helper result 706;
// explicit helper result 679; size policy 1026; byte validity 679. Request/control
// homes and the second stack argument remain unresolved.
// request/control homes and second stack argument remain;
// force decision at use 723; late validity 677; final activity values 676;
// entry output scalars 679; early-invalid helper 710; late validity plus activity values 674.
// Request/control homes and second stack argument remain; native node selector 704, node ordinal 679, separate transformed point 686, completion cases first 679, early data construction 679.
// Request/control homes and the second stack argument remain;
// indexed three-channel completion 743; position field receiver 704; byte transition 711;
// direct eligibility 705; byte request flags 705. Opening body retained.
// Frame 0x80 agrees; request/control carriers and ret 4 versus ret 8 remain.
// Unsigned requested names 674; short held-slot count 674; word activity result 690; transition
// cases first 674; internal validity linkage 674. Original helper declaration restored.
// Forced validity on late-result body 707; reversed validity fields 722; fallback vector memcpy 674;
// current-frame request/workspace names 674; shared channel flag word 674. Opening body and helper retained.
// Validity tag/index scalars 702; native identifier value/reference 820/699; scalar independent tests 729;
// scalar/value/reference forced expansion 707/868/707; identifier-first scalar formals 702. Original helper and
// twelve calls restored.
// Round 222: word transition/override 672/714; byte permission/activity 694/674; separate final accumulator 666 kept plain; logical final OR 680; both native validity tests 716, with word transition 714; call-first/combined accumulator 666/666; byte activity with accumulator 669.
// Round 226: Boolean transition 710; byte override 666; byte permission 693; transition with
// override 710; transition with permission 662; both byte inputs 693; Boolean transition with both
// bytes 662; word transition with both bytes 704.
// Complete request value after/before initial activity 689/706; with all three request results
// constructed directly 705/712; complete library input copy after/before 689/706,
// with direct results 705/712. Native request record and copies restored; original interface retained.
// Late changed/forced status byte-bool, word-bool, bool-byte, bool-word, byte-byte, byte-word, word-byte,
// word-word: 666/676/666/693/666/693/676/682 (fast); all restored.
// Eight complete entry object/control/state/tag receiver lexical orders all 666; frame
// 0x80 unchanged, request ESI capture and tag/control homes remain. All names restored.
// Round 146: Complete channel-reference validity with bool/byte/short/long results 666/666/677/672;
// forced expansion 702/702/698/699. Original pointer helper, calls and body restored.
// frame 0x80 agrees; ours passes the request pointer in EAX (ret 4) where retail
// keeps both arguments on the stack (ret 8) and reloads the request pointer three times; retail keeps the
// control pointer in ESI early, the force flag in a byte at +0x11 and the force result in a byte at +0x1b,
// inlines every channel-validity test (ours calls it at the three 0x700005c checks) and passes the raw
// channel words to 0xe6a20 (`movzx`), whose contributor-owned definition takes bools: BLOCKED-BY 0xe6a20.
// Round 55: retail never enregisters the request pointer and copies the request bools at +8/+9 as plain
// bytes; its transition flag is a byte (+0x11). Request bools read as bool 707 (2545 bytes); with a bool
// transition flag 723 (frame 0x7c, aligned 496 -> 383); bool transition flag alone 710 (aligned 386);
// declared after the result 709; with only the +9 bool read 742.
// Tried: ordinary validity inline 679; native final/override/validity 688/712/699; depth one 707;
// requested-name snapshots 709; inline plus native final/validity 696/707; ordinary-helper Boolean
// transition 712; native helper result 706; explicit helper result 679; size policy 1026; byte validity
// 679; force decision at use 723; late validity 677; final activity values 676; entry output scalars 679;
// early-invalid helper 710; late validity plus activity values 674; native node selector 704, node
// ordinal 679, separate transformed point 686, completion cases first 679, early data construction 679;
// indexed three-channel completion 743; position field receiver 704; byte transition 711; direct
// eligibility 705; byte request flags 705; unsigned requested names 674; short held-slot count 674; word
// activity result 690; transition cases first 674; internal validity linkage 674; forced validity on
// late-result body 707; reversed validity fields 722; fallback vector memcpy 674; current-frame request/
// workspace names 674; shared channel flag word 674; validity tag/index scalars 702; native identifier
// value/reference 820/699; scalar independent tests 729; scalar/value/reference forced expansion
// 707/868/707; identifier-first scalar formals 702; word transition/override 672/714; byte permission/
// activity 694/674; separate final accumulator 666; logical final OR 680; both native validity tests
// 716, with word transition 714; call-first/combined accumulator 666/666; byte activity with
// accumulator 669; Boolean transition 710; byte override 666; byte permission 693; transition with
// override 710; transition with permission 662; both byte inputs 693; Boolean transition with both bytes
// 662; word transition with both bytes 704; complete request value after/before initial activity
// 689/706, with all three request results constructed directly 705/712; complete library input copy
// after/before 689/706, with direct results 705/712; late changed/forced status byte-bool, word-bool,
// bool-byte, bool-word, byte-byte, byte-word, word-byte, word-word 666/676/666/693/666/693/676/682;
// eight entry object/control/state/tag receiver lexical orders 666. Round 75: force-inline validity
// helper 702 (aligned 487 -> 476); validity result computed before the changed flag 668 (aligned 468);
// definition pointer computed second 663; both 665; with a Boolean force flag 706 (frame 0x7c, aligned
// 385); plus the force result as a flag set in a test 708 (aligned 361, frame still 0x7c).
// Round 187: primary/secondary request producers, original/input-first/complete branch:
// original/input 694; original/branch 708; input/original 722; input/input 727;
// input/branch 727; branch/original 713; branch/input 690; branch/branch 690.
// On the secondary input-first form (ret 8): tag second 694; state before control 694;
// Boolean transition 702; tag second with Boolean transition 702. Original body restored.
// Round 220: Control/state/both/tag/control-tag/state-tag/all receiver fields at every use
// 719/928/977/674/693/956/960; all with object-pool observations 1946. Original body restored.
// DH round 215: Eager native field comparisons with Boolean/byte/short/long
// results, ordinary inline 717/717/807/766; forced inline 726/726/872/829. Original
// short-circuit helper, interface, request values and complete body restored.
// Round 30: Complete transition complement bool/byte/short/long 727/727/668/667; coupled with
// permission-override complement 727/727/668/667. Original flags, helper and body restored;
// request/control homes and ret 4 versus ret 8 remain.
// Round 258: Complete native channel-value validity input, ordinary bool/byte/short/long results
// 994/994/1001/993; forced 1000/1000/1013/1004. Original pointer helper and twelve calls
// restored; request/control homes and validity expansion remain.
// Round 386: existing native pair-validity helper by primary/case/both/tail/primary-tail/
// case-tail/all phases 701/666/701/745/751/747/753; native triple state guard 762.
// Original private validity helper, request/control homes and body restored.
// Round 169: native incoming record, mutable/readonly byte flags 666/666; word flags
// 675/675; dword flags 666/666; unsigned keys with byte/word flags 666/675. Original
// flat input accesses, external signature, request homes and complete body restored.
// Round 172: shared point-difference producer on the corrected frame 699;
// with actual transform/point/both/direction/transform-direction/point-direction/
// all workspaces uninitialized at entry, 699 each. Frame 0x80 and ret 4
// remain; original displacement statements and every lifetime restored.
// Round 174: control/state/tag receiver initialization at the completed request
// phase, seven subsets, 709/708/714/675/702/704/705; all with Boolean transition
// 725. Original initializers and body restored; request/control homes and ret 4
// versus retail ret 8 remain.
// Round 81: the request kept on the stack by a const reference to the parameter, read at the first use,
// the last use, the first two, the first and last, or once into a local: 663 each (ret 8 as in retail,
// first difference +0x23), but the request pointer is then cached in ESI where retail re-reads it, and the
// callers lose (0xdd360 281 -> 282, 0xefde0 715 -> 721).
// Round 83: the held-slot calls as a plain counted loop 666 (same code, kept); the 0xc000043 flag as one
// expression 700; the first request flag copied as a bool 673; the change test as one `||` 673; the
// validity and the forced flag ahead of the change test (retail's order) 686.
// Round 85: the validity helper as static inline or plain 666 (same code); the 0x700005c case's three
// bit sets in a forceinline helper 702 (every validity test then inline, 47 calls as retail); the request
// read through its address at every use 663 (ret 8; callers 0xdd360 282, 0xefde0 721), with the bit-set
// helper 691.
// Round 68 (lane 9, fast; aligned = difflib over the listing's instructions, baseline 705 instructions
// against retail's 703, opcode ratio 0.790): BLOCKED-BY 0xe6b00 too: retail's 0xe6b00 takes the unit index
// in EBX, ours in ESI, so its three call sites differ (its file was held by another lane this round). The
// channel-clear test `local_5 == 0x7000101 ? unknown7c == 0x400000c : local_5 == 0x400000c` always
// materialises a sete (also as its own enclosing if, 666, same code); retail's threaded branches come from
// `((local_5 == 0x7000101 && unknown7c == 0x400000c) || local_5 == 0x400000c)`, which gives retail's exact
// shape and instruction count (703) but 2553 bytes and 688 positional (opcode ratio 0.794); with the
// validity of channel 0 computed before the change flag as well (retail's order) 690 (ratio 0.804). The
// select compared once, `(local_5 == 0x7000101 ? unknown7c : local_5) == 0x400000c`, 714 (no threading);
// the existing vector3d_from_points3d for the node offset 699. Retail also re-tests the state's frozen bit
// before each channel update (ours drops it as already tested) and threads each block's frozen exit to the
// end; the four-block pattern is the same knowledge difference as in 0x113980.
// Round 209 (m3 lane 3, fast with the six queue functions and both callers; masked = difflib count with
// branch targets masked, baseline 333): no pointer to 0x111650 exists in the retail image, so its
// all-stack convention is LTCG's own choice. Retail holds the control pointer in ESI through the opening
// block and re-reads both parameters from their stack homes at each use; ours receives the request
// pointer in EAX and keeps it in ESI. Tried: the request through a const reference to the parameter
// (ret 8; a parameter-address form, keepable only with a match) with the +8/+9 flags read as bools 684 (callers 0xdd360 224 -> 223, 0xefde0 565 -> 562); the
// reference with the threaded channel-clear test `((local_5 == 0x7000101 && unknown7c == 0x400000c) ||
// local_5 == 0x400000c)` 665 (callers 223/562; the request is still loaded into ESI at entry); all three
// 692; bool flags with the threaded test 698; the definition pointer computed second with the threaded
// test 685. BLOCKED-BY 0xe6a20 (contributor) and 0xe6b00 (held file) as before.
// P349: Complete transition producers on the all-stack graph, bool/byte/signed-char/short, positive/complementary:
// 670/670/670/670/670/670/708/682. Kept the original-polarity Boolean transition, plain 670 at 2579/2577.
// Its zero/one/comparison stores use native byte home +0x11; the other forms are restored.
// Round 93 (m3 lane 9, fast with all six queue functions): retail keeps the second decision (local_6 ||
// !local_28) in its own byte at +0x1b (the dword ours lacks), cleared with the change flag and set by a
// conditional store; ours folds it into local_6's dead slot. As a flag cleared with the change flag and set by
// `if (local_6 || !local_28)` 662 (2584 B, frame 0x80; a flag set in a test, so not kept without a match);
// plus the +8/+9 request bytes read as plain bools 591 (2570 B, diagnostic); the channel test moved ahead of
// the change flag 743, with the flag form 722/723 (declared second/first), cleared late 734; the decision in
// the `if` condition 672 (frame 0x80), with the channel test inline too 671; `!local_28 || local_6` 656 (2565
// B, still the reused slot and retail's opposite order, not kept); `local_6 ? true : !local_28` 669.
// Kept: the +8/+9 request bytes read as plain bools 620 (retail copies them unnormalised), then the channel
// flags read once into a long local for the 0x4000/2 decision 511 (retail's movzx and 32-bit test); plain
// confirmed. Not kept: the bools with the decision in the `if` condition 607 (plain; it moves the counter
// block away from retail's layout, aligned 233 -> 503), with the channel test inline too 627; __forceinline on
// function_111651 (retail expands it at every site, ours keeps three calls) 565 at 2577 B on the bool form,
// with the flags local 621 (aligned 213); channel test first with either/both 692/698/694; the flag-set
// decision with both (diagnostic) 575, aligned 128. On 511: channel_9c flags as word/long local 510/512;
// result accumulated in local_10 513, with the word local 512; `return local_10 | ... | ...` 509. Left: the
// frame dword (the decision's own byte), BLOCKED-BY 0xe6a20 (contributor; retail passes the +0x14/+0x16
// words whole), the buffer address re-formed after 0x1c7380, NONE kept in ESI late, the tail's OR order. The
// request wants a two-name, two-bool record type for the parameter (declared in bipeds.cpp and vehicles.cpp).
// Walk b3A2 (2026-11-06, statement by statement against 0x111650..0x11205e, fast + align.py): no logic difference
// found. Checked: the request words (+0, +4, +8, +9), the parent test and its four-way local_4 chain (0x1116f1-0x1117af),
// every 0xe6b00 out slot (0x1118b4), the switch tree (0xa00001d, 0x9000020, 0x700005c, 0x800001c and the 0xc000043 /
// 0xd000042 / 0xe0000c2 half) and each case body, the node-offset float code (0x111a13-0x111a43, no x87 and no rounding),
// the 0x111010 / 0xc7070 tail, the decision (0x111c69-0x111cbb), the 0x10f430 call (pushes at 0x111cf5-0x111d05), the
// periodic block (0x111d6f-0x111dd8) and the three channel blocks (0x111ddf-0x112037), the 0.267f / 0.1335f constants
// (0x3e88b439 / 0x3e08b439) and the OR tail. Kept: the node offset through vector3d_from_points3d(&local_22,
// &local_23->position, &local_19) (retail takes the position pointer once, `add eax, 0x28` at 0x111a1e, then [eax+4],
// [eax+8]; the written-out subtractions re-add the offsets); a NEW use of that shared inline in this file (visible through
// globals.h already) for the full check. The fast score reads 621 (was 511) only because the listing is positional and
// ours now has 701 instructions against 703 (align.py: 113 hunks, ratio 0.779, unchanged).
// Precision/type findings NOT kept (each needs other files or is a score-only form), with fast numbers on this base:
//  - 0xe6a20 (called only here, 0x11182a, 0x111ec6, 0x111fce) gets the channels' +0x14/+0x16 words whole, zero-extended
//    (movzx), not bools: its `active`/`keep` are integers (it only tests bit 0) and the channel fields are words.
//    With unknown_0a76b0.cpp `long active, long keep` (0xe6a20 stays MATCH), unknown_1c62f0.h `word unknown14; word
//    unknown16;` and the calls passing the fields raw, together with the two forms below: 345 (branch agent/b3A2-xfile).
//    `word` parameters alone gave `xor/mov` instead of retail's movzx (624).
//  - retail's decision byte is its own stack slot (+0x1b, cleared at 0x111c87, set at 0x111cb6) and function_111651
//    is expanded at all 8 sites: `bool local_30 = false; ... if (local_6 || !local_28) local_30 = true;` gives frame
//    0x80 and the first 0x165 bytes identical (align ratio 0.78 -> 0.91), `__forceinline` on function_111651 expands
//    the three calls in the 0x700005c case (ratio 0.93). A flag set in a test and a forced-inline helper are not
//    behaviour fixes, so they are not kept without a match.
//  - the channel-clear test as `((local_5 == 0x7000101 && unknown7c == 0x400000c) || local_5 == 0x400000c)` is retail's
//    threaded branch shape (0x111d14-0x111d33, the ternary always makes a sete); same behaviour, 622 against 621.
// Left (codegen only): the control pointer is cached in EBX where retail re-reads [esp+0x24] at each block, and
// retail keeps the `graph_tag_index != NONE && !(flags & 1)` guards of the three channel blocks (0x111e03-0x111e0c)
// that ours folds into the block's own test; `mov edx, edi` for the s_bit_owner where retail re-forms `lea edx` (0x1119b5,
// 0x1119ca); local_28 (0x111c69) is computed before the change flag in retail but moving it up costs 684-707 (slot homes
// swap); the OR order of the tail (0x112045).
// Round 149 (m3 lane 8, plain): `__forceinline` on function_111651's definition (retail expands it at all eight sites)
// 621 -> 566 at retail's 2577 bytes (differing lines 609 -> 566, length term 12 -> 0; offset-masked aligned 138 -> 127,
// register-masked 98 -> 87). Kept although the line above did not keep it: the base changed, the length now equals
// retail's, and a forceinline declaration on an existing helper is an accepted form.
// Round 77 (m3 lane 15, fast with the five queue functions, plain confirmed 557 at 2573 B; aligned/masked 213/87 ->
// 204/78): kept 566 -> 557: the tail accumulated in local_10 (`local_10 |= function_10dc70(arg_0); local_10 |=
// function_10eaf0(arg_0); return local_10;`, retail's exact tail, 559 alone) and the channel word read once into `short
// active_flags` at each 0xe6a20 site, tested and passed (retail's `mov cx, [ebp + 0x14]; test cx, cx`, 564 alone). Also
// kept, same code: the typed biped/unit definition fields for +0x254/+0x1cc, and the existing
// s_animation_state::channel_update in place of its body written out in the three channel blocks. Tried on the tail
// base: local_28 computed first with local_30 as a flag 709 (2607 B); local_30 as a flag set in a test 579 (2579 B,
// frame 0x80, aligned 122, masked 80); local_29 as one expression 601. On 557: the request names through stored_names,
// directly or by a pointer, 557 (same code); the 0x800001c case's `local_5 != local_14` named once 638; the 0x9c
// block's flags read once into a long under a nested test 598; the flags word declared before the name in the 0x9c/0xbc
// blocks 597; re-measured the threaded clear test 629 (2560 B, masked 73) and `return local_10 | ... | ...` on 566 566
// (2579 B). Left: frame 0x80 (local_30's flag byte), the 0xe6a20 word arguments (BLOCKED-BY 0xe6a20), the bit owner's
// lea, local_28's position, NONE in ESI for the request guard and the weapon loop, and the unit index in EBX with
// control re-read across the channel blocks.
// @retail 0x111650
bool __stdcall function_111650(long arg_0, long *arg_1)
{
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    s_unit_animation_control *local_1 = UNIT_ANIMATION_CONTROL(local_0);
    s_animation_state *local_2 = UNIT_ANIMATION_STATE(local_0);
    s_unit_animation_definition *local_3 = (s_unit_animation_definition *)TAG_BYTES(local_0->definition_index);
    long local_4 = 0x7000101;
    long local_5 = 0x7000101;
    bool local_6 = false;
    bool local_7 = false;
    bool local_8 = false;
    long local_9 = 0;
    bool local_10 = function_113910(arg_0);
    if (arg_1[0] != NONE && arg_1[0])
        local_4 = arg_1[0];
    if (arg_1[1] != NONE && arg_1[1])
    {
        local_5 = arg_1[1];
        local_8 = ((bool *)arg_1)[8];
    }
    if (local_0->parent_index == NONE)
    {
        if (local_2->unknown70 == 0x20001b3)
            local_4 = 0x20001b3;
        else if (TEST_FIELD_BIT(local_0->animation_frozen))
            local_4 = 0x6000086;
        else
        {
            byte *local_11 = (byte *)UNIT_ANIMATION_OBJECT(arg_0);
            if (*(short *)(local_11 + *(short *)(local_11 + 0x346) + 0x36))
                local_5 = 0x7000101;
            else
            {
                if (local_4 == 0x7000101)
                    local_4 = function_10eef0(arg_0, ((bool *)arg_1)[9], false);
                if (*((byte *)local_0 + 0x1f4) > 0)
                    local_4 = 0x7000039;
                if (*(long *)((byte *)local_1 + 0x2c) != NONE)
                    local_4 = *(long *)((byte *)local_1 + 0x2c);
            }
        }
    }
    if (function_111651(&local_2->channels[0]))
    {
        byte *local_12 = (byte *)UNIT_ANIMATION_OBJECT(arg_0);
        local_2->update(function_bf600, arg_0,
            (unsigned long)(long)*(short *)(local_12 + 0x110) >> 5,
            (s_blend_orientation *)(local_12 + *(short *)(local_12 + 0x10e)),
            (s_blend_orientation *)(local_12 + *(short *)(local_12 + 0x112)));
        long local_13 = local_2->unknown70;
        long local_14 = local_2->unknown7c;
        short active_flags = local_2->channels[0].unknown14;
        if (active_flags)
            function_e6a20(arg_0, local_14, (active_flags & 1) != 0, (local_2->channels[0].unknown16 & 1) != 0);
        if (!(local_2->flags & 1) && (local_2->channels[0].flags & 1) &&
            !(local_2->channels[0].unknown11 & 9) && local_2->channels[0].function_1c6440()->frame_count > 1)
        {
            local_10 = true;
            if (local_2->channels[0].unknown11 & 0xa)
            {
                long channel_flags = local_2->channels[0].flags;
                if ((channel_flags & 0x4000) && (channel_flags & 2))
                    local_6 = false;
                else if ((channel_flags & 0x4000) && !(local_1->flags & 0x100))
                {
                    local_2->flags |= 1;
                    local_6 = false;
                }
                else
                {
                    long local_15 = 0x7000101;
                    long local_16 = 0x7000101;
                    function_e6b00(arg_0, local_14, &local_15, &local_16);
                    if (local_15 != 0x7000101 || local_16 != 0x7000101)
                    {
                        local_4 = local_15;
                        local_5 = local_16;
                        local_7 = true;
                    }
                    switch (local_14)
                    {
                    case 0x800001c:
                    case 0xa00001d:
                        if (!*((byte *)local_0 + 0xaa) && !function_bfc90(arg_0))
                        {
                            long local_17 = ((s_biped_definition *)TAG_BYTES(UNIT_ANIMATION_OBJECT(arg_0)->definition_index))->biped.pelvis_node_index;
                            if (local_17 != NONE)
                            {
                                s_animation_data local_18;
                                c_animation_channel_data_get(&local_2->channels[0], &local_18);
                                vector3f local_19;
                                if (function_10eea0((s_bit_owner *)&local_18, (short)local_17))
                                {
                                    long local_20 = bit_vector_count_bits((dword *)function_10ee70((s_bit_owner *)&local_18), local_17 + 1) - 1;
                                    transform4x3f local_21;
                                    function_ba160(arg_0, &local_21);
                                    point3f local_22;
                                    c_animation_channel_node_position_get(&local_2->channels[0], &local_22, 0.0f, (short)local_20);
                                    transform4x3f_apply_point(&local_21, &local_22, &local_22);
                                    transform4x3f *local_23 = function_b8bd0(arg_0, (short)local_17);
                                    vector3d_from_points3d(&local_22, &local_23->position, &local_19);
                                }
                                else
                                    local_19 = *g_4687a4;
                                function_e5840(arg_0, &local_19, local_5 != local_14 || local_4 != local_13);
                                local_6 = local_5 != local_14;
                            }
                        }
                        break;
                    case 0x700005c:
                    case 0x700005d:
                        if (function_111651(&local_2->channels[2]) && (local_2->channels[2].flags & 1))
                            local_2->channels[2].unknown11 |= 1;
                        if (function_111651(&local_2->channels[0]) && (local_2->channels[0].flags & 1))
                            local_2->channels[0].unknown11 |= 1;
                        if (function_111651(&local_2->channels[1]) && (local_2->channels[1].flags & 1))
                            local_2->channels[1].unknown11 |= 1;
                        break;
                    case 0x900001f:
                    case 0x9000020:
                    case 0x90006b2:
                        if (!*((byte *)local_0 + 0xaa))
                        {
                            function_e4300(arg_0);
                            local_6 = true;
                        }
                        break;
                    case 0xe0000c2:
                        function_11b710(arg_0, local_4);
                        local_7 = true;
                        break;
                    case 0xd000042:
                        local_6 = false;
                        break;
                    case 0xc000043:
                        {
                            bool local_24 = false;
                            if ((bool)((local_3->flags >> 1) & 1))
                            {
                                if (function_10f5f0(arg_0) != 0xd000042 || (*((byte *)local_0 + 0xc1) & 1))
                                    local_24 = true;
                            }
                            if (!function_a7670(arg_0) && local_24)
                                function_d6bc0(arg_0);
                            else
                            {
                                local_1->flags |= 4;
                                local_2->flags |= 1;
                            }
                        }
                        break;
                    }
                    if (!function_111010(arg_0))
                    {
                        local_6 = true;
                        if (local_5 == 0x7000101)
                            local_5 = 0x400000c;
                    }
                    long local_25 = local_0->parent_index;
                    if (local_25 != NONE && function_0c7070(arg_0))
                    {
                        s_unit_definition *local_26 = (s_unit_definition *)TAG_BYTES(UNIT_ANIMATION_OBJECT(local_25)->definition_index);
                        byte *local_27 = (byte *)local_26->unit.seats.address + *(short *)((byte *)local_0 + 0x1fc) * 0xb0;
                        if ((bool)((*(dword *)local_27 >> 11) & 1) && *(short *)(local_27 + 0x3e) != NONE)
                        {
                            local_9 = 4;
                            local_4 = 0x7000101;
                            local_5 = 0x400000c;
                        }
                    }
                }
            }
        }
    }
    bool local_29 = false;
    if (local_4 != local_2->unknown70 && local_4 != 0x7000101)
        local_29 = true;
    if (local_5 != local_2->unknown7c && local_5 != 0x7000101)
        local_29 = true;
    bool local_28 = function_111651(&local_2->channels[0]);
    bool local_30 = local_6 || !local_28;
    if ((local_29 && (local_7 || function_1103a0(arg_0, local_5, local_8))) || local_30)
    {
        if (!function_10f430(arg_0, local_4, 0x7000101, 0x7000101, local_5, 0.0f, false, local_9 | 0x401))
        {
            if ((local_5 == 0x7000101 ? local_2->unknown7c == 0x400000c : local_5 == 0x400000c) && !(local_2->unknown6e & 1))
                local_2->channels_clear();
        }
        if (local_2->graph_tag_index != NONE && function_111651(&local_2->channels[0]) && !unit_animation_state_finished(local_2))
            local_10 = true;
    }
    else if (!((++g_55e70c ^ arg_0) & 0xf))
    {
        if (local_2->graph_tag_index != NONE && !(local_2->flags & 1))
            local_2->resources_request(local_2->unknown70, local_2->unknown74, local_2->unknown78, true, true);
        for (long local_32 = 0; local_32 < 4; local_32++)
        {
            long local_31 = ((long *)((byte *)local_0 + 0x218))[local_32];
            if (local_31 != NONE)
                function_1060a0(local_31, arg_0);
        }
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_33 = &local_1->channel_dc;
        if (function_111651(local_33))
        {
            local_2->channel_update(local_33, function_bf600, arg_0);
            if ((local_33->unknown11 & 0xa) && TEST_FIELD_BIT(local_1->flag7))
            {
                local_33->clear();
                local_1->flag7 = false;
                function_114330(arg_0);
            }
            if (function_111651(local_33) && (local_33->flags & 1) && !(local_33->unknown11 & 9))
                local_10 = true;
        }
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_34 = &local_1->channel_9c;
        if (function_111651(local_34))
        {
            local_2->channel_update(local_34, function_bf600, arg_0);
            long local_35 = local_34->unknown08;
            short active_flags = local_34->unknown14;
            if (active_flags)
                function_e6a20(arg_0, local_35, (active_flags & 1) != 0, (local_34->unknown16 & 1) != 0);
            if ((local_34->unknown11 & 0xe) && !((local_34->flags & 0x4000) && (local_34->flags & 2)))
            {
                if ((bool)((local_34->flags >> 14) & 1))
                    function_11b710(arg_0, local_2->unknown70);
                function_e6b00(arg_0, local_35, NULL, NULL);
                if (local_34->unknown08 == local_35 && !function_110fc0(arg_0))
                {
                    local_34->clear();
                    if (*((byte *)local_0 + 0xaa) != 1)
                        function_ba350(arg_0, 0.267f);
                }
            }
        }
        if (function_111651(local_34) && (local_34->flags & 1) && !(local_34->unknown11 & 9))
            local_10 = true;
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_36 = &local_1->channel_bc;
        if (function_111651(local_36))
        {
            local_2->channel_update(local_36, function_bf600, arg_0);
            long local_37 = local_36->unknown08;
            short active_flags = local_36->unknown14;
            if (active_flags)
                function_e6a20(arg_0, local_37, (active_flags & 1) != 0, (local_36->unknown16 & 1) != 0);
            if (local_36->unknown11 & 0xe)
            {
                function_e6b00(arg_0, local_37, NULL, NULL);
                if (local_36->unknown08 == local_37 && !function_110fc0(arg_0))
                {
                    local_36->clear();
                    if (*((byte *)local_0 + 0xaa) != 1)
                        function_ba350(arg_0, 0.1335f);
                }
            }
            if (function_111651(local_36) && (local_36->flags & 1) && !(local_36->unknown11 & 9))
                local_10 = true;
        }
    }
    local_10 |= function_10dc70(arg_0);
    local_10 |= function_10eaf0(arg_0);
    return local_10;
}

/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);
bool function_e4050(long object_index);
const long g_4408f8[8] = { NONE, 0, 1, 2, 2, NONE, NONE, 3 };
const long g_440918[8] = { NONE, NONE, 0x600008c, 0xc000043, 0xc000043, 0xd000042, 0xc000043, 0xc000043 };
const long g_440938[8] = { NONE, NONE, NONE, 0x6000086, 0x6000086, 0x6000086, 0x6000086, 0x6000086 };
const long g_46fc30[4] = { 0x600008b, 0x600008c, 0x600008d, 0x600008e };

/* the time a channel has played, in seconds (none when it plays nothing) */
static inline real channel_elapsed_seconds(c_animation_channel const *channel)
{
    real elapsed = 0.0f;

    if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
        elapsed = channel->frame_position * (1.0f / 30.0f);
    return elapsed;
}

// MiniMax S (11-14): no new form, no lower score (12): the definition and the control still take each other's stack homes (retail keeps the control at +0x1c and the definition at +0x20) and the two flag bytes are still cleared after the set test where retail clears them in the entry block; declaring the definition before the control gives 20, moving the two flags ahead of the lookup rebuilds the function at 682 bytes and takes the neighbouring 0x112070 to 3418, and the permuter (15 tries) found no candidate, so the body stands.
// MiniMax A (11-12): no new form, no lower score (12); the definition and the control still take each
// other's homes (retail keeps the control at +0x1c and the definition at +0x20, ours the other way round) and
// the two flag bytes are still cleared after the set test where retail clears them in the entry block; this
// round's forms (the definition declared before the control, the overlay declared before the animation, the
// force flag declared first, and the two flags declared ahead of the lookup, which rebuilds the function at
// 682 bytes) all leave 12 or more, and the permuter found no candidate.
// MiniMax H (11-13): no new form, no lower score (12); the definition and control stack slots are swapped and the two flag bytes are cleared after the test here and before it in retail.
// score 12 (was 12): earlier 12 (was 44): earlier BLOCKED-BY 0xe70b0 (its return type) for the frame 0x3c versus 0x38, lifted in lane 16 r97. Measured from
// MiniMax b (11-06): no new form, no lower score; the action short's four-byte slot is the whole frame
// difference, and it can only be a two-byte compiler temporary if 0xe70b0 returns its two-byte result
// through a hidden pointer in EAX, as retail's 0xe70b0 does (its definition is in src/unknown_0a76b0.cpp
// and its caller 0xc98a0 is contributor code), so this stays BLOCKED-BY 0xe70b0.
// MiniMax A (11-04, run 41): no new form, no lower score (44); the frame is still 0x3c against retail's 0x38,
// which waits on 0xe70b0's return type.
// MiniMax D (11-01): no new form, no lower score.
// MiniMax A (11-07, run 54): no new form, no lower score (44); the frame is still 0x3c against retail's
// 0x38 (the action short keeps a 4-aligned slot and the two bytes under the flags go unused), which
// waits on 0xe70b0's return type.
// the frame top every slot agrees except the action short (ours 4-aligned below the 4-byte slots,
// retail 2-aligned with the two flag bytes under it) and the control/definition pair; VC7.1 gives a
// lone 2-byte local, word, char[2], one-short struct or inlined short parameter a 4-aligned slot, so
// the packing needs a 4-byte aggregate. Round 17: success exit as `*flag = result = true` 52 (kept,
// retail's AL store and length); control declared before state 61; definition declared last 55; flag
// initializations before the lookup 206; force flag declared first 52; block-scoped action 52;
// overlay initialized at its declaration 54. Round 19: the action query in an inline helper owning
// the short 167 (kept out of line, so the frame is 0x38 but the bytes stay at +2/+3), forceinline 145
// (0xe70b0 then inlines too), forceinline with 0xe70b0 declared noinline 52 (the inlined short is
// 4-aligned as well); control renamed so it sorts after definition 52 (slot unchanged); only the force
// flag initialized before the lookup 206.
// Returned action comparison plus a separate existing channel-copy statement recover
// native query/play ordering at retail length; the shared result exit is retained.
// frame 0x3c versus 0x38; the escaped action short has its own
// aligned slot. The action and seat queries match; returned action-pointer comparison
// and channel-copy/play argument scheduling still differ.
// frame 0x3c versus 0x38; action/decision packing and copy schedule differ.
// Retail packs the address-taken action short at +2 beside the two flag bytes;
// ours gives it a 4-byte slot. The action query now matches and returns its output pointer.
// Tried: native channel workspace 139; copy return to play 152; accumulated channel failure 177;
// unsigned word result 173; short table index 188; dword channel backing 152; direct copy chain 152;
// mode at use 196 (frame 56); repeated table index 152; mode after animation 157; direct action
// decision 163; actual overlay-bit result 224; converged play operations 152; control/tag/table role
// names 152 each; selected-overlay initialization 178; word play flags 150; signed success/force/both
// bytes 147/152/147; word channel backing 152; mode fallback 154; unsigned play mask, const tag view
// and success fallthrough 152 each; language-char result 147; unsigned-word force 164;
// signed-byte/word animated result 147/156; base success test 153; success/force/both initialized
// before selection 187 each; separate copy-return boundaries 142 (exact-only, discarded);
// base-channel entry/selection 147/147; animation-result home 147; receiver/workspace copy split
// 162/162, with base channel 162/162; success/force/both role names 147 each; unsigned
// lookup/animation/mode 147 each; action-result assignment/positive completion 161/161; direct
// success return 154; copy statement before the play call 162 (aligned diff 71 -> 65); copy with
// direct success return 165; bool result 152, with the copy 162, with copy and return 162.
// Returned action query with Boolean-result/copy-boundary/direct-return combinations
// 196/207/198/209/200/200/202/202; three-field decision records 198/212/220 (exact-only, discarded);
// returned query with entry result/result-and-force/result-and-action homes 200/200/200.
// Complementary success/overlay decisions, individually and together with native widths
// and constant entry initialization: 207/198/207/207/207/207/215/207; restored.
// Unchanged reduced control 198/684 differs from full 155/668; original code retained.
// Round 210: State/control/tag reads at every use 196/202/222; state-control/state-tag/control-tag/all
// 205/216/202/217; unit at every use 188, plain 195. Reduced unchanged control 198;
// all restored against full baseline 155. Action query now matches; its return-use forms
// previously grew the frame and remain excluded.
// DH round 215: Mutually exclusive base/overlay eligibility join with signed-byte/Boolean/
// byte/short success storage 172/165/172/159; paired true publication chained
// 172/166/172/159. Original decisions, copy expression and complete body restored.
// Round 30: Immutable set/mode/lookup results at entry in seven subsets, then all with the constant
// animation default at entry: 173/173/167/194/189/199/189/173 (expanded control 155).
// Original table observations and all lifetimes restored; frame 0x3c versus 0x38 remains.
// Round 258: Immutable mode at every use on the repaired query graph, plus lookup/query/native-
// channel subsets 191/191/192/192/182/182/181/181. Frame 0x38 forms regress; original
// observations, action test, copy chain and body restored.
// Round 386: action/set-decision/channel-backing lifetimes in seven subsets
// 155/156/156/155/155/156/156; all with native constructed channel 157.
// Fixed caller/request roots reproduce full 155; original body restored.
// Round 168: Native paired timing fields/header status/parent slot in seven subsets and all
// with a shared absolute object index 155 each. Original records, copy chain and body restored.
// Round 169: returned query with action/force/both/backing/action-backing/force-backing/
// all scopes 149/150/150/149/149/150/150; all plus native channel 265. Scoped query
// plus direct true exit 148, Boolean result 151, separate copy 129, actual flag value 149;
// separate copy with original shared exit 122, also with original action home 122 (plain, kept).
// Local action comparison on copy graph 132; Boolean result on query/copy graph 127.
// Round 172: consistent inline copy/indexed-play/both/base-play/copy-base/
// indexed-base/all summaries 155 each; all with the actual returned-action word
// test 149. The six-count gain fails the eight-count gate; every declaration,
// provider contract and the complete caller restored. Frame 0x3c remains.
// Round 172 DL: Escaped action-role name 122; complete query/play/overlay role names 122;
// Boolean aggregate with completed-success exit 127, also with those names 127.
// Whole availability decision on returned-query/separate-copy graph, signed/Boolean
// aggregate and shared/direct success exit 156/158/163/158. All restored;
// frame 0x3c versus 0x38 and small-home packing remain.
// Round 174: chained/result-first successful publication with signed/Boolean aggregate
// 122/122/128/128; two-use base-channel entry/set lifetimes with original/chained
// publication 122 each. Original body restored; frame 0x3c and short packing remain.
// DL176: Selected-set guard/call carrier with signed/Boolean aggregate and shared/direct success
// 69/61/71/61; consistent mode-at-use combinations 171/194/200/194; single renamed set with
// Boolean/signed aggregate 127/122. Keep the two-use selected-set carrier and Boolean aggregate,
// plain confirmed 61; frame 0x3c versus 0x38 and action-short packing remain.
// Round 81: control declared before definition and state 44 (kept; the entry schedule then
// matches); force before result 44; the flags (either order) before the animation default 207, after it
// 207/207; the animation default before the three table reads 44. Retail packs the action short
// 2-aligned beside the two flag bytes, which VC7.1 does for a compiler temporary: 0xe70b0 written as
// returning a two-byte class with a constructor by value (hidden return pointer in EAX, returned
// unchanged, as retail's 0xe70b0 does) gives frame 0x38 and 20 here, 12 with the declaration order
// kept above (the control/definition slots and the two flag stores before the lookup's branch remain).
// Round 83: animation chosen by a ternary 189; the action short declared first 218; the no-overlay and
// overlay-busy exits as `return false` 225/225; the animation default before the table reads with both flags
// cleared before the lookup's `if` 232 (the clears then sit where retail has them, but NONE goes to EDI).
// Round 85: flags cleared before a ternary animation lookup 210; force then result between the NONE
// default and the lookup's test 207; only the result declared first 207; the lookup table read at each use
// 44 (same code); the overlay pointer declared before the flags 44 (same code). The return-type fix stays
// blocked: 0xe70b0's caller 0xc98a0 is contributor code.
// Round 36 (plain checks; --fast builds this function differently, 219 on unchanged source): the
// action short declared after the overlay pointer 52 (identical); the action query in an inline
// bool helper owning the short 167 (stays a call; frame 0x38), force-inlined 145 (frame 0x38, flags
// still at +2/+3, and LTCG then inlines 0xe70b0, which retail calls). Declared first, between or
// before the flags: identical under --fast.
// Round 85: flags cleared before a ternary animation lookup 210; force then result between the NONE
// default and the lookup's test 207; only the result declared first 207; the lookup table read at each use
// 44 (same code); the overlay pointer declared before the flags 44 (same code). The return-type fix stays
// blocked: 0xe70b0's caller 0xc98a0 is contributor code.
// Round 68 (lane 9, plain checks; --fast gives 218-219 at 687 bytes for any edit, even one giving the same
// code): 0xe70b0 and 0xc98a0 now count as ours, but 0xe70b0's definition sits in a file another lane held this
// round, so the class return (frame 0x38) waits for the next round. The overlay test through the file's
// channel-validity helper 44 (same code); the `type < 3` test as its own enclosing if 44; the force decisions
// as an else-if ladder 44; play flags as a ternary 99; the selected mode as a ternary 205.
// N303: Disjoint action/channel scratch unions, byte/word/dword at entry/set/paired scopes,
// 17 each: frame 0x38 recovered, action still in channel scratch (exact-only). Native
// two-byte/word decision records with byte-order and constant-init point/order combinations
// 134/135/240/238/134/135/238/240. All grouped storage restored; original action/flag homes remain.
// Round 90: one-short array with returned query 44; two-short output first/second 39 each
// (action in channel scratch; NOT kept: under eight and wrong frame); scalar/one-short local
// comparisons 60 each; set-scoped scalar 44; set-scoped second-short output 39, with the
// predicate declarations swapped 39. Original executable body restored.
// Round 65 (m3 lane 1, plain): overlay initialised to NULL at its declaration 46; `*flag = false` after the
// three table reads 65; the action short in the innermost else block 44 (same code); the force flag in the
// set block 46, with the action short there too 46. The 0xe70b0 two-byte return still waits on held files.
// Round 209 (m3 lane 3, fast with all six queue functions; a fast check of this address alone builds it
// differently, 204-218 for any edit): still BLOCKED-BY 0xe70b0 (its file was held again). Tried: the
// e4050 test's header byte read through the object-header struct 44 (same code); the overlay-busy test
// on the bit field without TEST_FIELD_BIT 204 (681 B); `mode != NONE` tested before the parent/seat test
// 97; the definition as the typed definition struct with fields at +0x108/+0x110 44 (same code); the
// overlay pointer declared with the entry pointers 44 (same code).
// Round 93 (m3 lane 9, fast with all six queue functions): still BLOCKED-BY 0xe70b0 (its file avoided again).
// The channel workspace at function scope 44; `result = force_overlay = false;` after plain declarations 44;
// `*flag = false;` first, before the unit lookup 79 (669 B); the state looked up before the definition 54;
// action and overlay declared before the flags with force before result 44.
// Round 69 (m3 lane 13, plain; --fast still builds any edit at 687 bytes): still BLOCKED-BY 0xe70b0 (its file avoided
// again). The state looked up before control and definition 55; definition declared before control 52; control declared
// after state 52; the action short declared before the two flags 44; the force flag declared before the result 44;
// the `type < 3` test after the state test 84; the set passed directly with no selected-set local 120.
// M3 lane 16 r97: kept 44 -> 12 (plain): 0xe70b0 now returns its two-byte action state by value (a record with a
// converting constructor), so the action is the call's temporary, the frame is retail's 0x38 and the BLOCKED-BY is
// gone. Left: the set and mode homes exchanged (ours set +0x1c, mode +0x20), and the two flag bytes cleared after the
// animation lookup where retail clears them (+0x11, then +0x10) ahead of its test. Plain trials on it: the two flags
// declared ahead of the animation lookup, result first 204 or force first 205 (682 bytes each: any edit to these
// declarations rebuilds the function at 682 bytes); the flags between the animation's NONE and its lookup 203; set
// and mode declared mode first with the flags early 205.
// MiniMax C (11-09): score 44 (was 44): the reduced image's 0xe70b0 writes its two-byte result
// through the pointer it is handed in EAX, so the word at +0x14 is that result's home and the
// action short we still declare keeps a two-byte home at +0x10 that nothing reads; retail has no
// such home, which is the whole frame difference (0x3c against 0x38). Dropping the dereference
// (207), merging the two play calls into one ternary (75) and hoisting the query's call out of
// the test (60) all keep the frame at 0x3c; the short's home needs 0xe70b0's file, which is not
// in this round's list.
// MiniMax C (11-09): score 44 (was 44): the frame is 0x3c against retail's 0x38: retail packs
// the action short beside the two flag bytes (the two-byte value 0xe70b0 returns) and clears the
// two flags before the lookup's branch; the short's own home needs 0xe70b0's file, which is not
// Round 77 (m3 lane 15; fast with the five queue functions builds it at 668 B, alone any edit builds 687 B): kept the
// definition typed as the unit definition, with the hard/soft ping interrupt times for +0x110/+0x108 (same code).
// `definition` renamed to sort after `control`, and `control` renamed to sort after `definition` (re-measured): same
// code, so names do not order these two slots; definition declared before control 20 (re-measured on frame 0x38); the
// two flags at the top, before `*flag = false`, before the set read or before the lookup read, either order, 204/205,
// and between the NONE and the `if` 203 (all 682 B, lane 16 r97's shapes: the clears move into the entry block as
// retail, but animation goes to memory, NONE to EDX and control to EBX); control re-derived through
// UNIT_ANIMATION_CONTROL(unit) at the overlay address / the flag7 test / the flag7 set 111/117/218, in pairs
// 117/226/233; the definition read at each use 211 (re-measured). No untried natural form found for the slot pair:
// retail's order needs definition at least as heavy as control by reference count.
// MiniMax B (11-13): no new form, no lower score (12): aligned the two listings instruction by instruction: the control and
// definition homes stay swapped (retail 0x0c and 0x10 against our 0x10 and 0x0c) and the two flag clears stay after the
// lookup's branch where retail clears them before it; retail also clears them in the other order (its 0x11 before its
// 0x10); declaring the definition first changes the prologue but not the homes (20), moving the clears before the branch
// restructures the function (203 to 205, and 204 with the clears before the animation), and swapping the two flag
// declarations changes nothing (12); the flags moved between the lookup read and the animation measure 204 at 682
// bytes, and the permuter cannot search the function at all, because its reduced image scores 202 against the full
// image's 12.
// @retail 0x10ff40
bool function_10ff40(long unit_index, long type, short side, short value,
    bool *flag, short *side_out, short *value_out)
{
    s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
    s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
    s_unit_definition *definition = (s_unit_definition *)TAG_BYTES(unit->definition_index);
    s_animation_state *state = UNIT_ANIMATION_STATE(unit);
    *flag = false;
    long set = g_440918[type];
    long mode = g_440938[type];
    long lookup = g_4408f8[type];
    long animation = NONE;
    if (lookup != NONE) animation = g_46fc30[lookup];
    bool result = false;
    bool force_overlay = false;
    c_animation_channel *overlay;
    if (set != NONE)
    {
        overlay = NULL;
        if (type < 3 &&
            (state->unknown7c != 0x600008c || !(channel_elapsed_seconds(&state->channels[0]) > definition->unit.hard_ping_interrupt_time)))
        {
            if (!function_1103a0(unit_index, set, true) || function_c8fc0(unit_index))
                force_overlay = true;
            else
            {
                if (function_e70b0(unit_index).value != 0 ||
                    (*((byte *)g_4e0300->data + (unit_index & 0xffff) * 12 + 3) == 0 && function_e4050(unit_index)))
                    force_overlay = true;
            }
        }
        if (animation != NONE)
        {
            byte channel_storage[sizeof(c_animation_channel)];
            c_animation_channel *channel = (c_animation_channel *)channel_storage;
            channel->copy_from(&state->channels[0]);
            if (!state->channel_play_indexed(channel, animation, side, value, 0, 0))
                force_overlay = true;
        }
        if (force_overlay)
        {
            if (animation == 0x600008c) animation = 0x600008b;
            else if (animation == 0x600008e) animation = 0x600008d;
            else goto done;
            set = NONE;
        }
    }
    if (set == NONE)
    {
        overlay = &control->channel_dc;
        if (TEST_FIELD_BIT(control->flag7) && overlay->graph_tag_index != NONE &&
            overlay->animation_id.index != NONE &&
            definition->unit.soft_ping_interrupt_time >= overlay->frame_position * (1.0f / 30.0f))
            goto done;
    }
    long selected_set = set;
    if (selected_set != NONE)
    {
        long selected_mode = 0x7000101;
        long play_flags = 2;
        if (animation != NONE)
            play_flags = 14;
        if ((unit->parent_index == NONE || *(short *)((byte *)unit + 0x1fc) == NONE) && mode != NONE)
            selected_mode = mode;
        result = function_10f430(unit_index, selected_mode, 0x7000101, 0x7000101, selected_set, 0.0f, false, play_flags);
    }
    if (animation != NONE)
    {
        bool animated;
        if (overlay)
        {
            animated = state->channel_play_indexed(overlay, animation, side, value, side_out, value_out);
            if (animated)
                control->flag7 = true;
        }
        else
            animated = state->play_indexed(animation, side, value, side_out, value_out);
        if (animated)
        {
            *flag = result = true;
            goto done;
        }
    }
done:
    return result;
}
