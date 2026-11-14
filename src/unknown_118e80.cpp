// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_118E80.CPP: an object's forward vector in the world (lane I's
   outside function; the command scripts' 0x25a130 calls it). The position's
   counterpart is 0xb9dd0 (unknown_0b58c0.cpp). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <string.h>
#include "object_markers.h"
#include "unknown_1cafc0.h"
#include "object_queries.h"
#include <math.h>
#include "vector_products.h"
#include "normalize3d.h"

/* the object as this reads it */
struct s_forward_flags
{
	union
	{
		word value;
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word flag5 : 1;
			word flag6 : 1;
			word : 9;
		};
	};
};

struct s_forward_object
{
	long definition_index;
	byte unknown04[0x14 - 4];
	long parent_index;
	char parent_node;
	byte unknown19[0x30 - 0x19];
	point3f center;
	byte unknown3c[0x64 - 0x3c];
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown88[0x90 - 0x88];
	real value90;
	vector3f angular_velocity;
	byte unknowna0[0xf0 - 0xa0];
	real valuef0;
	byte unknownf4[0x10a - 0xf4];
	word : 2;
	word frozen : 1;
	word : 13;
	byte unknown10c[0x116 - 0x10c];
	short nodes_offset;
	byte unknown118[0x12a - 0x118];
	short animation_state_offset;
	s_forward_flags flags;
	byte unknown12e[0x146 - 0x12e];
	char speed_mode;
	byte unknown147;
	vector3f control;
	vector3f requested_forward;
	vector3f requested_up;
	vector3f turn_velocity;
	real turn;
	char mode;
};

struct s_forward_object_header
{
	byte unknown0[8];
	s_forward_object *object;
};

// @retail 0x118f70
byte function_118f70(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	byte flags = (byte)object->flags.value;
	flags >>= 1;
	flags &= 1;
	return flags;
}

// @retail 0x119250
void function_119250(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_forward_flags *flags = &object->flags;
	flags->flag6 = true;
}

/* sets or clears the given bits of an object's flags */
PRIVATE __forceinline void forward_flags_set(s_forward_flags *flags, word mask, bool enabled)
{
	if (enabled)
		flags->value |= mask;
	else
		flags->value &= ~mask;
}

__declspec(noinline) void function_118fe0(long object_index, bool enabled);

// @retail 0x118fe0
void function_118fe0(long object_index, bool enabled)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (TEST_FIELD_BIT(object->frozen))
		enabled = false;
	forward_flags_set(&object->flags, 1, enabled);
}

__declspec(noinline) bool function_119bc0(long object_index);

// @retail 0x119bc0
bool function_119bc0(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	bool result = false;
	if (!TEST_FIELD_BIT(object->frozen) && (TEST_FIELD_BIT(object->flags.flag5) || TEST_FIELD_BIT(object->flags.flag4)))
		result = true;
	return result;
}

struct s_orientation_request_118f90
{
	long name0;
	long name4;
	bool enabled;
	byte unknown09[2];
	bool force;
	byte unknown0c[0xc];
	vector3f forward;
	vector3f up;
};

__declspec(noinline) void function_118f90(s_orientation_request_118f90 *request);

// @retail 0x118f90
void function_118f90(s_orientation_request_118f90 *request)
{
	memset(request, 0, sizeof(*request));
	request->name0 = 0x6000086;
	request->name4 = 0x400000c;
	request->forward = *g_4687a8;
	request->up = *g_4687b0;
}

// @retail 0x119500
void function_119500(long object_index, long state, s_orientation_request_118f90 *request)
{
	if (request->enabled)
	{
		s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		if (TEST_FIELD_BIT(object->flags.flag5) && (object->mode == 1 || object->mode == 3) && object->parent_index == NONE)
		{
			if (object->value90 > 0.0f)
				request->name4 = 0xd00003f;
			else
				request->name4 = 0x800001e;
			return;
		}
		switch (state)
		{
		case 1: request->name4 = 0x400000c; break;
		case 2: request->name4 = 0xa000017; break;
		case 3: request->name4 = 0x9000016; break;
		case 4: request->name4 = 0xa000014; break;
		case 5: request->name4 = 0x9000015; break;
		}
	}
}

void function_ba350(long object_index, real seconds);

void __stdcall function_b9b90(long object_index, bool disable);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity, bool skip_update);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void function_b7360(long object_index);
void function_bba20(long object_index);
real function_30bf0(vector3f *vector);
void function_118e80(long object_index, vector3f *forward);
bool function_1faf80(long object_index, void const *settings_pointer, vector3f const *control,
	real threshold, vector3f *facing, real *turn);
void function_11f0d0(vector3f *position, vector3f *forward, vector3f const *target, real rate, real max_angle, real scale);
void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);
real function_11ce20(vector3f const *a, vector3f const *b);
void object_dispatch_moved(long object_index, long a, long b, long c);

struct s_facing_definition_1197b0
{
	byte unknown00[0xc4];
	real max_angle;
	real acceleration;
	real speed_scale;
	byte unknownd0[4];
	dword flags;
	byte unknownd8[0x13c - 0xd8];
	byte settings[1];
};

PRIVATE __forceinline void world_up_1197b0(long object_index, vector3f *up)
{
	s_forward_object_header *headers = (s_forward_object_header *)g_4e0300->data;
	s_forward_object *object = headers[object_index & 0xffff].object;
	if (object->parent_index == NONE)
		*up = object->up;
	else
	{
		s_forward_object *parent = headers[object->parent_index & 0xffff].object;
		transform4x3f *matrix = (transform4x3f *)((byte *)parent + parent->nodes_offset + object->parent_node * 0x34);
		real i = object->up.i, j = object->up.j, k = object->up.k;
		up->i = matrix->up.i * k + matrix->left.i * j + matrix->forward.i * i;
		up->j = matrix->up.j * k + matrix->left.j * j + matrix->forward.j * i;
		up->k = matrix->up.k * k + matrix->left.k * j + matrix->forward.k * i;
	}
}
// MiniMax D (11-06, not kept at m3 review): 241 -> 208 with the temporary velocity copied only in the else
// arm (then unset on the max_angle == 0 && acceleration == 0 path, which can still reach 0x11f0d0: a
// behaviour change), the position copied into that same local, the first 0x11d180 call's left output
// written into the later basis-forward local and the 0x11ce20/dot3f arguments swapped; copying the velocity
// after the if/else, as retail, gives retail's slot packing (--fast 99) but 264 in the full build. The
// frame stays 0x70 against retail's 0x6c. build/held/mmd-1106-1197b0-slot-steering.patch
// MiniMax H (11-04): the temporary velocity scoped so the position copy reuses its slot 256 at
// retail's 1030 bytes; not kept, the frame stays 0x78 and the score rises.
// MiniMax E (11-01): no new form, no lower score; left: frame 0x78 against retail's 0x6c, the
// basis/position homes and the early ESI save (the 36-point form needs 0x11f0d0 in
// src/unknown_11eed0.cpp).
// score 220 (was 220): BLOCKED-BY 0x11f0d0. Our frame is 0x78 with ESI saved in the prologue; retail's
// is 0x6c with ESI pushed after the 0x1faf80 exit and popped after the dispatch call. The cause is
// 0x11f0d0's named `double s` (and `double angle`, and its acos helper's double return): under LTCG a
// callee's double local makes this function keep 8-byte stack parity, so it pads its frame and cannot
// save ESI late. Retail's 0x11f0d0 divides in float (fdivr [step]) and multiplies each component in
// double without folding the last multiply (fld; fmul st(1); ...; fstp st(0)); a double parameter of an
// inlined helper gives that code without the alignment need, a double local or any float form does not.
// Earlier: retail's first 11d180 call writes the cross product into the stack slot its later two calls
// write the rotated forward into: the position in a nested block (242), that first call's fourth
// argument (260), both (258). The basis and position stack homes still differ; the eight vector locals
// keep the homes they have here. Frame 0x78/0x6c, early ESI save and basis/position homes remain.
// Earlier baseline: Frame 0x78 versus 0x6c, early ESI save and basis/position homes remain.
// Earlier: Frame 0x78 versus 0x6c; early ESI save and basis/position homes remain.
// frame 0x78 versus 0x6c; early ESI save and basis/position homes remain.
// Depth zero 536; depth one 235; target size 429; ordinary world-up inline 465;
// shared-left output with one exit 233 (two-point gain dropped).
// Branch-scoped velocity 248; scalar point setter 255; parent-transform first 237;
// entry flag mask 278; active-angle first 235.
// Frame and basis/position homes still differ.
// normalized forward shared 265; basis scratch roles reused 269; velocity/basis scratch shared 252;
// shared forward and scoped velocity 257; early position snapshot 263.
// Frame 0x78 versus 0x6c; basis/position homes remain. Shared speed scale 226 kept; left/velocity scratch 252; zero-source memcpy 235; angle-limit lifetime 233; accepted-up first 255 (plain).
// Frame 0x78 versus 0x6c; basis/position homes remain. Speed-arm scalars 270;
// acceleration first 226; scalar names 226; early turn-velocity capture 226; zero velocity after
// world-up 220 kept (plain).
// Frame 0x78 versus 0x6c and basis/position homes remain. Selected up buffer
// 269; normalized forward at third call 222; at second call 265; all calls with retained scale
// and late zero 265; selected up plus shared normalized forward 301 (plain).
// Frame 0x78 versus 0x6c and basis/position homes remain.
// Angular/basis scope 220; angular-only scope 220; scoped angular/position names 220;
// scoped branch zero 242; basis copies 220; angular copies 220; common exit 218;
// scoped point setter 242 (plain); two-point gain dropped.
// Frame 0x78 versus 0x6c; velocity/basis/position homes remain.
// zero after first basis 240; zero after second basis 240; turn velocity before guard 274; basis role
// reuse with scale 260; turn zero before guard 220.
// Existing world-up object input 269; header-base input 220; vector return 285; real rotation member 247;
// position aggregate initializer 242.
// Round 110: direct scale on late-zero frame 232; native velocity aggregate 237; const metadata 220; inline basis 220; inline forward 220 with caller loss; symmetric angle order 218 (two-point gain dropped).
// Round 112: First left/output shared 259; later shared 260; all shared 262; second only 258;
// third only 219 (one-point gain dropped); shared default velocity 242; native world-up input 295.
// Both local-first final dot products 222; world-up source-field rereads 273; read-only parent/matrix
// views 220; position callback scope 242 (retail length); CRT unparented up copy 220.
// Existing node getter 283; native matrix advance 220; both default CRT copies 220; getter plus
// default copies 283; turn transfers 220; turn transfers plus late zero 220; world-up pointer
// result 220; world-up parameter order 220. Original body/private helper restored; frame 0x78
// versus 0x6c and early ESI/basis/position homes remain.
// Round 132: separate input-up and parent metadata 275; native carriers 276; output-first
// 275/276; actual header input 275/276; header input plus output-first 275/276.
// All input-splitting contracts regress; existing producer, late zero and basis roles restored.
// Round 250: Entry basis/position/all native workspaces 220/220/220; discarded-forward name 220;
// complete basis/phase names 220/220; entry basis plus basis names 220; entry workspaces plus phase
// names 220. Frame 0x78/0x6c and early ESI/basis/position homes remain.
// Round 153: Actual up/forward/both/turn/up-turn/forward-turn/all three field receivers
// 249/253/253/276/261/283/260; requested-forward/all four 266/256. Default-vector pointer observed
// once, alone/with turn/with all three fields 240/251/292. Whole original body restored.
// Round 252: Final orientation predicates in bool/byte/short/long domains 251/251/259/257; short-circuit
// refined results 222/222/222/222. Original comparisons and every vector/callback role restored; frame
// 0x78 versus 0x6c and early ESI/basis/position homes remain.
// Round 80: retail's frame packs the position copy into the temporary velocity's slot and gives the
// first basis call its own left output (sharing the later forward output's slot). Position copied
// inside an inline helper gives the 0x6c frame under --fast, 269 plain (object re-looked-up); from the
// cached object 243 (1030 bytes); plus the zero velocity before world-up 247 (zero-vector copies now
// as retail); plus a separate first left 259; plus the speed scale read at each use 238. The early
// ESI save and a 4-byte pad come from 0x11f0d0's double locals (angle, s) once the caller 0x118700 is
// in the image: with both real, --fast with the caller gives retail's late ESI. `real angle` keeps
// 0x11f0d0 identical; `real s` or the quotient written per component fold its last multiply (2 -> 89).
// Lane 9 r29 (plain): `real angle` in 0x11f0d0 220 (identical); `real s` there 220 here
// (--fast showed 200 with the late ESI, a reduced-image artifact; 0x11f0d0 2 -> 89), also as
// `step / (real)sqrt(dist2)`; the four basis vectors declared in reverse 220; position renamed 220.
// Round 58 (fast, reduced baseline 200 versus plain 220): angle-helper inline declaration 200;
// static-inline vector position setter 217; world-up position output before/after the up result 206/205;
// world-up output order 205; position declaration order 205; entry position with inline store 205;
// scoped temporary velocity with inline position store 205. Original helper and body restored.
// Round 30: Complete parent-handle long/dword lifetimes 220/220; transform-first arms with those
// handles 227/227; separate outlined producer original/transform-first 472/472; outlined handle
// long/dword pair 472/472. Original forced producer, input copies and all body code restored.
// Round 34: Complete parent-transform value/const-value/reference/const-reference inputs
// 329/329/220/220; those four with complete native up inputs 353/353/295/295.
// Original producer and body restored; frame, early save and basis/position homes remain.
// Round 390: Complete period/default-vector observations and per-use definition acquisition,
// seven subsets plus native double period 279/242/313/263/296/297/321/341. Original body and helper restored.
// Round 188: native point/vector position with memcpy/memmove, original then entry
// uninitialized declarations: 220/220/223/223/220/220/223/223. Memmove reaches retail
// 1030-byte length but regresses; original position transfer and body restored.
// Round 211: outgoing up/forward/turn overlap-safe transfers in all seven subsets, then all three
// with the late position transfer, scores 224/223/233/223/233/230/241/251. Original executable
// form restored.
// Round 230: complete object-element observations alone 326; with saved header base 294;
// with named slot 305; with both 292; those four with per-use tag elements
// 382/349/422/332. Original body and producer restored.
// DL round 206: existing matrix/vector, matrix3x3/vector and axes-vector producers
// 220/296/296; early-arm callee inline 220; producer/inline pairs 220/296/296;
// early-arm callee forced 1340. All producer, body and external declarations restored.
// Round 100 (lane 7, plain unless noted): the temporary velocity (with world-up and the basis calls),
// the first basis call's left, the later forward/left pair and the position each in their own block
// give retail's slot packing (0x6c) but a 4-byte pad and the early ESI 260; without the position block
// 266 (--fast 94 with the late ESI, a reduced-image artifact); 0x11ce20 called (&target_up, &up), as
// retail's ECX/EDX setup implies, 218 (--fast 200); the speed scale read at each use 232 (--fast 190);
// both 230. Diagnostics with the block-scope form: 0x11f0d0's double locals and acos return as real 73
// (frame 0x6c, retail's late ESI; 0x11f0d0 2 -> 89, its last multiply folds); angle and acos real with
// `double s` kept 260 (0x11f0d0 2); the difference written per use with a real scale 73 (0x11f0d0 81,
// 0xc60c0 401 -> 416); an inline helper with a double scale parameter for the three position components
// 73 with 0x11f0d0 unchanged at 2 (0xc60c0 401 -> 416), not kept: a new one-caller helper without a match.
// On that helper form (0x11f0d0 with `real angle` and the double-parameter step): the block scopes 73;
// plus both final dots with the local first 73; plus 0x11ce20 called (&target_up, &up) 71; with that
// call alone 71; plus the speed scale read at each use 37 (plain; 0xc60c0 416); plus the final stores
// forward-first 38; plus the dots local-first 37; plus world-up's two lookups read straight from the
// pool, no table local (--fast) 36. Left at 36: the forward dot's i product order, the final stores
// (retail interleaves both copies, ours writes the forward copy through `lea ecx`), and the second
// 0x11f0d0 call re-deriving &up (`lea esi`) where ours copies EBX.
// Round 244: complete ordinary-path requested-forward values via native/const,
// aggregate/const, memcpy/memmove and entry copy/memmove constructions
// 295/295/264/264/295/314/295/314. Original input observations and body restored.
// Round 288: plain world-up component rows with long/short/dword/word counters,
// scalar/native-vector captures 224/318/224/318/295/300/295/300.
// Original producer and body restored; frame, early-save and basis homes remain.
// Round 126 (lane 8, plain): 0x11ce20 called (&target_up, &up) 218 (re-measured; under the bar); on it: speed
// scale read at each use 230; forward stored before up 219; frame rate read once into a local for its three uses
// 279 (1052 bytes); the angle test inverted with the copy arm first 240. Still BLOCKED-BY 0x11f0d0's double local.
// Round 207 (lane 3, fast): on 0x11f0d0's position step, the existing scale_vector_by_double into the
// difference then vector3f_add 220 here (0x11f0d0 2 -> 162, 933 bytes: each product is rounded and stored);
// the existing vector3f_add_scaled with a real quotient 220 (0x11f0d0 89, its `double angle` still pads
// this frame); here: turn velocity declared before forward 224; world-up computed before the forward
// branch 285 (1010 bytes); the second turn's scale as `acceleration + acceleration` 224.
// Round 400: complete ordinary zero-limit/admission results in bool, byte,
// short and long domains 249 each, 1030/1030 bytes. NOT kept: no gain or
// match; original scalar tests and body restored. Frame and early save remain.
// DH384: shared xyz requested/default/turn/orientation/all native transfers
// 256/262/289/260/361; complete point-helper transfers 361; all-output zero
// construction 392, with shared transfers 491. NOT kept: all regress and grow;
// the native 0x6c frame, late ESI and basis/position homes remain unresolved.
// World-up row first sums/results at each publication 220/220; all first sums/results
// before stores 224/225; sequential rows 220; early first products 225; all sequential
// results 225; native output aggregate 299. NOT kept: ties or regressions; original restored.
// @retail 0x1197b0
void function_1197b0(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_facing_definition_1197b0 *definition = (s_facing_definition_1197b0 *)g_4e3b44[object->definition_index & 0xffff].bytes;
	if ((bool)((definition->flags >> 4) & 1))
	{
		function_1faf80(object_index, definition->settings, &object->control, 0.9f, &object->requested_forward, &object->turn);
		return;
	}
	real max_angle = definition->max_angle;
	real acceleration = definition->acceleration;
	if (object->speed_mode == 1)
	{
		real speed_scale = definition->speed_scale;
		max_angle *= speed_scale;
		acceleration *= speed_scale;
	}
	vector3f forward, turn_velocity;
	if (max_angle == 0.0f && acceleration == 0.0f)
	{
		forward = object->requested_forward;
		turn_velocity = *g_4687a4;
	}
	else
	{
		function_118e80(object_index, &forward);
		turn_velocity = object->turn_velocity;
		function_11f0d0(&turn_velocity, &forward, &object->requested_forward, g_510c54->rate, max_angle, acceleration);
	}
	vector3f up;
	world_up_1197b0(object_index, &up);
	vector3f temporary_velocity = *g_4687a4;
	vector3f new_forward, target_up, left, forward2;
	function_11d180(&forward, &object->requested_up, &new_forward, &left, &target_up);
	function_11d180(&forward, &up, &forward2, &left, &up);
	if (function_11ce20(&up, &target_up) > max_angle * g_510c54->rate)
	{
		function_11f0d0(&temporary_velocity, &up, &target_up, g_510c54->rate, max_angle, acceleration * 2.0f);
		function_11d180(&forward, &up, &forward2, &left, &up);
	}
	else
		up = target_up;
	point3f position = object->position;
	object_dispatch_moved(object_index, (long)&position, (long)&new_forward, (long)&up);
	if (dot3f(&object->up, &up) < 0.9999f || dot3f(&object->forward, &new_forward) < 0.9999f)
	{
		object->up = up;
		object->forward = new_forward;
		function_bba20(object_index);
	}
	object->turn_velocity = turn_velocity;
}

struct s_impulse_definition_119020
{
	byte unknown00[0xd4];
	dword flags;
};

// @retail 0x119020
void function_119020(long object_index, vector3f const *impulse)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_impulse_definition_119020 *definition = (s_impulse_definition_119020 *)g_4e3b44[object->definition_index & 0xffff].bytes;
	function_b9b90(object_index, false);
	vector3f velocity_before_impulse;
	function_ba1d0(object_index, &velocity_before_impulse, NULL);
	vector3f velocity;
	velocity.i = impulse->i + velocity_before_impulse.i;
	velocity.j = impulse->j + velocity_before_impulse.j;
	velocity.k = impulse->k + velocity_before_impulse.k;
	if (TEST_FIELD_BIT(object->frozen) || (bool)((definition->flags >> 3) & 1) || object->mode == 2)
	{
		vector3f axis;
		cross3f(g_4687b0, impulse, &axis);
		function_30bf0(&axis);
		real first_axis = axis.i;
		real fraction = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
		real magnitude = (real)sqrt(impulse->i * impulse->i + (impulse->j * impulse->j + impulse->k * impulse->k));
		real scale = fraction * magnitude * 1.57079637f;
		object->angular_velocity.i += scale * first_axis;
		object->angular_velocity.j += scale * axis.j;
		object->angular_velocity.k += scale * axis.k;
	}
	function_b7740(object_index, &velocity, NULL, false);
	function_1c4b00(object_index, &velocity, NULL, 1);
	if (length_sq3f(&velocity) > 0.0001f)
	{
		function_b9b90(object_index, false);
		function_b7360(object_index);
		function_bba20(object_index);
	}
}

PRIVATE __forceinline bool direction_state_119650(long name)
{
	return name == 0xa000014 || name == 0x9000015 || name == 0x9000016 || name == 0xa000017;
}

// @retail 0x119650
bool function_119650(long object_index, long mode, long set, real seconds, long flags)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_animation_state *state = (s_animation_state *)((byte *)object + object->animation_state_offset);
	long old_mode = state->unknown70;
	long old_set = state->unknown7c;
	bool result;
	if (mode != 0x7000101 || set != 0x7000101)
		result = state->animation_set(mode, 0x7000101, 0x7000101, set, flags, 0x3f);
	else
		result = true;
	if (result)
	{
		bool mode_changed = old_mode != mode && mode != 0x7000101;
		bool set_changed = old_set != set && set != 0x7000101;
		if (mode_changed || set_changed)
		{
			real blend = 0.267f;
			if (mode_changed)
				blend = 0.267f;
			if ((old_set == 0x400000c && direction_state_119650(set)) ||
				(direction_state_119650(old_set) && (direction_state_119650(set) || set == 0x400000c)))
				blend = 0.267f;
			if (!(blend > seconds))
				blend = seconds;
			if (blend > 0.0f)
				function_ba350(object_index, blend);
		}
	}
	return result;
}

// @retail 0x1195c0
void function_1195c0(long object_index, s_orientation_request_118f90 const *request)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_animation_state *state = (s_animation_state *)((byte *)object + object->animation_state_offset);
	bool changed = false;
	if (request->name0 != state->unknown70 && request->name0 != 0x7000101)
		changed = true;
	if (request->name4 != state->unknown7c && request->name4 != 0x7000101)
		changed = true;
	if (request->force || state->channels[0].graph_tag_index == NONE || state->channels[0].animation_id.index == NONE || changed)
	{
		if (!function_119650(object_index, request->name0, request->name4, 0.0f, 0) && request->name4 == 0x400000c)
			state->channels_clear();
	}
}

// @retail 0x118e10
void function_118e10(long object_index, point3f *point)
{
	s_object_marker marker;
	if (function_b8d30(object_index, 0x4000095, &marker, 1, false) > 0)
		*point = marker.matrix.position;
	else
		*point = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object->center;
}

struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

struct s_type_1e6529
{
	long definition_index;
	union { byte unknown04[4]; dword flags; };
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	long unknown1c;
	short unknown20;
	short unknown22;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	vector3f cone_direction;
	real unknown78;
	short unknown7c;
	short unknown7e;
	byte unknown80[4];
	bool unknown84;
	byte unknown85[3];
};

void function_d6660(s_type_1e6529 *data, long definition_index);
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index, vector3f const *unknown14);

struct s_parent_effect_definition_11a140
{
	byte unknown00[0x16c];
	long effect_index;
	byte unknown170[4];
	long alternate_effect_index;
};

// @retail 0x11a140
void function_11a140(long object_index)
{
	s_forward_object_header *headers = (s_forward_object_header *)g_4e0300->data;
	s_forward_object *object = headers[object_index & 0xffff].object;
	s_parent_effect_definition_11a140 *definition = (s_parent_effect_definition_11a140 *)g_4e3b44[object->definition_index & 0xffff].bytes;
	s_forward_object *parent = headers[object->parent_index & 0xffff].object;
	long effect_index = definition->effect_index;
	if (parent->valuef0 > 0.0f && definition->alternate_effect_index != NONE)
		effect_index = definition->alternate_effect_index;
	if (effect_index != NONE)
	{
		s_type_1e6529 data;
		data.unknown7c = NONE;
		function_d6660(&data, effect_index);
		object_get_damage_owner(object_index, &data.owner);
		data.origin = object->center;
		data.position = object->center;
		function_d7b80(&data, object->parent_index, NONE, NONE, NONE, NULL);
	}
}

/* the object's forward vector, turned by its parent's node when it has a
   parent */
// @retail 0x118e80
void function_118e80(long object_index, vector3f *forward)
{
	s_forward_object_header *headers = (s_forward_object_header *)g_4e0300->data;
	s_forward_object *object = headers[object_index & 0xffff].object;

	if (object->parent_index == NONE)
	{
		if (forward)
		{
			*forward = object->forward;
		}
		return;
	}

	s_forward_object *parent = headers[object->parent_index & 0xffff].object;
	transform4x3f *matrix = (transform4x3f *)((byte *)parent + parent->nodes_offset + object->parent_node * 0x34);

	if (forward)
	{
		real i = object->forward.i;
		real j = object->forward.j;
		real k = object->forward.k;

		forward->i = matrix->rotation.up.i * k + matrix->rotation.left.i * j + matrix->rotation.forward.i * i;
		forward->j = matrix->rotation.up.j * k + matrix->rotation.left.j * j + matrix->rotation.forward.j * i;
		forward->k = matrix->rotation.up.k * k + matrix->rotation.left.k * j + matrix->rotation.forward.k * i;
	}
}


bool function_211680(long arg_0, point3f *arg_1);
void __stdcall function_b77d0(long arg_0, vector3f const *arg_1, vector3f const *arg_2);

// @retail 0x118b10
bool function_118b10(long arg_0)
{
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    bool local_1 = false;
    vector3f local_2;
    if ((*((byte *)local_0 + 0x144) & 0x10) && !TEST_FIELD_BIT(local_0->flags.flag5)
        && *(long *)((byte *)local_0 + 0x134) != NONE
        && function_211680(arg_0, (point3f *)&local_2))
    {
        function_b77d0(arg_0, &local_2, NULL);
        local_1 = true;
    }
    return local_1;
}


struct s_119380
{
    long field_0;
    long field_4;
    bool field_8;
    bool field_9;
    bool field_a;
    bool field_b;
};

void __stdcall function_bf600(long arg_0, real arg_1, s_animation_frame_event const *arg_2);
void function_2116c0(long arg_0, long arg_1, long arg_2);

// @retail 0x119380
bool function_119380(long arg_0, s_119380 *arg_1)
{
    bool local_2 = false;
    (void)&arg_0;
    (void)&arg_1;
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    s_animation_state *local_1 = (s_animation_state *)((byte *)local_0 + local_0->animation_state_offset);
    memset(arg_1, 0, sizeof(*arg_1));
    arg_1->field_0 = 0x7000101;
    arg_1->field_4 = 0x7000101;
    if (local_1->channels[0].graph_tag_index != NONE && local_1->channels[0].animation_id.index != NONE)
    {
        s_forward_object *local_3 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
        long local_4 = local_1->unknown7c;
        local_1->update(function_bf600, arg_0,
            (unsigned long)(long)*(short *)((byte *)local_3 + 0x110) >> 5,
            (s_blend_orientation *)((byte *)local_3 + *(short *)((byte *)local_3 + 0x10e)),
            (s_blend_orientation *)((byte *)local_3 + *(short *)((byte *)local_3 + 0x112)));
        if ((local_1->channels[0].flags & 1) && !(local_1->channels[0].unknown11 & 9)
            && (local_1->channels[0].unknown11 & 2))
        {
            if (local_4 == 0xc000043)
            {
                local_0->flags.flag3 = true;
                local_1->flags |= 1;
            }
            if (TEST_FIELD_BIT(local_0->flags.flag1) && !TEST_FIELD_BIT(local_0->flags.flag2))
            {
                *((byte *)local_0 + 0x12c) &= ~2;
                arg_1->field_b = true;
                if (*(long *)((byte *)local_0 + 0x134) != NONE)
                    function_2116c0(arg_0, local_1->unknown70, local_1->unknown7c);
            }
            local_2 = true;
        }
    }
    if (!TEST_FIELD_BIT(local_0->frozen))
    {
        arg_1->field_0 = *(long *)((byte *)local_0 + 0x13c);
        arg_1->field_4 = *(long *)((byte *)local_0 + 0x140);
        word local_5 = *(word *)((byte *)local_0 + 0x144);
        if (local_5 & 1)
        {
            arg_1->field_9 = true;
            arg_1->field_a = (bool)((*((byte *)local_0 + 0x144) >> 1) & 1);
            local_0->flags.flag1 = true;
            return true;
        }
        if (TEST_FIELD_BIT(local_0->flags.flag1) && !(local_5 & 4))
        {
            arg_1->field_4 = local_1->unknown7c;
            return true;
        }
        arg_1->field_8 = true;
        local_2 = true;
    }
    return local_2;
}


void function_1e54d0(void *arg_0, long arg_1);
bool function_bc380(long arg_0, long arg_1, short arg_2, long arg_3);

// @retail 0x118510
bool __stdcall function_118510(long arg_0, void const *arg_1, bool *arg_2)
{
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    byte *local_1 = g_4e3b44[local_0->definition_index & 0xffff].bytes;
    *(short *)((byte *)local_0 + 0x12e) = NONE;
    *((byte *)local_0 + 0x130) = 0;
    *((byte *)local_0 + 0x131) = 0;
    local_0->turn = 0.0f;
    byte *local_2 = (byte *)local_0 + 0x17c;
    *(long *)(local_2 + 8) = NONE;
    *(long *)(local_2 + 0xc) = NONE;
    *(long *)(local_2 + 4) = arg_0;
    *local_2 = 0;
    if ((bool)((*(dword *)(local_1 + 0xd4) >> 4) & 1))
        function_1e54d0(local_2, 2);
    else
    {
        local_0->flags.value |= 0xa0;
        function_1e54d0(local_2, 1);
    }
    *(short *)((byte *)local_0 + 0x206) = 0;
    function_118f90((s_orientation_request_118f90 *)((byte *)local_0 + 0x13c));
    s_forward_object *local_3 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    bool local_4;
    if (local_3->animation_state_offset != NONE)
        local_4 = true;
    else
        local_4 = false;
    if (local_4)
    {
        short local_5 = *(short const *)((byte const *)arg_1 + 0xb4);
        if (local_5 != NONE)
        {
            *(long *)((byte *)local_0 + 0x134) = local_5;
            local_4 = function_bc380(arg_0, 0x138, *(short const *)((byte const *)arg_1 + 0xb6), 0) != 0;
            local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
        }
        else
            *(long *)((byte *)local_0 + 0x134) = NONE;
        if (local_4)
        {
            short local_6 = *(short const *)((byte const *)arg_1 + 0x64);
            *(short *)((byte *)local_0 + 0x12e) = local_6;
            if (g_4e6948->state == 1 && (local_6 == 0 || local_6 == NONE))
                *(short *)((byte *)local_0 + 0x12e) = *(short *)(local_1 + 0xc0);
            function_119650(arg_0, 0x7000001, 0x7000001, 0.0f, 2);
        }
        else
            *arg_2 = true;
    }
    return local_4;
}

bool (__stdcall *g_468594)(long, void const *, bool *) = function_118510;



struct s_havok_component;
struct s_character_physics_update_input_datum_a;
struct s_source_a;
struct s_biped_physics_result;
struct s_biped_physics_input;

struct s_118b80
{
    byte field_0[0x14];
    bool field_14;
    byte field_15[0x44 - 0x15];
    long field_44;
    bool field_48;
    byte field_49[3];
    point3f field_4c;
};

struct s_118b81
{
    long field_0;
    dword field_4;
    long field_8;
    long field_c;
};

point3f *function_b9dd0(long arg_0, point3f *arg_1);
long function_baf80(long arg_0);
bool function_1ec500(long arg_0);
bool havok_component_any_rigid_body_active(s_havok_component *arg_0);
void function_1e68a0(s_character_physics_update_input_datum_a *arg_0, s_source_a *arg_1,
    long arg_2, long arg_3, long arg_4, bool arg_5, bool arg_6, bool arg_7,
    bool arg_8, bool arg_9, point3f *arg_10, point3f *arg_11, point3f *arg_12);
void function_1e5a60(void *arg_0, s_biped_physics_result *arg_1, s_biped_physics_input const *arg_2);
// @retail 0x118b80
void __stdcall function_118b80(long arg_0)
{
    s_forward_object_header *local_0 = (s_forward_object_header *)g_4e0300->data;
    s_forward_object *local_1 = local_0[arg_0 & 0xffff].object;
    long local_2 = *(long *)((byte *)local_1 + 0xb4);
    if (local_2 != NONE)
    {
        s_havok_component *local_3 = (s_havok_component *)(g_51e9b8->data + (local_2 & 0xffff) * 0xa0);
        byte *local_4 = g_4e3b44[local_1->definition_index & 0xffff].bytes;
        s_forward_object *local_5 = local_0[function_baf80(arg_0) & 0xffff].object;
        if (!(*((byte *)local_5 + 0xc1) & 1) && function_1ec500(arg_0))
            *((byte *)local_1 + 0x10a) |= 0x20;
        point3f local_6;
        function_b9dd0(arg_0, &local_6);
        bool local_7 = function_118b10(arg_0);
        if (havok_component_any_rigid_body_active(local_3) || local_7
            || !(bool)((*(word *)((byte *)local_1 + 0xc0) >> 6) & 1))
        {
            word local_8 = *(word *)((byte *)local_1 + 0xc0);
            long local_9;
            if ((bool)((local_8 >> 2) & 1) || (bool)((local_8 >> 1) & 1))
                local_9 = *(long *)((byte *)local_1 + 0xb8);
            else
                local_9 = NONE;
            s_118b80 local_10;
            s_118b81 local_11;
            byte *local_12 = local_4 + 0xd4;
            byte *local_13 = (byte *)local_1 + 0x17c;
            function_1e68a0((s_character_physics_update_input_datum_a *)&local_10,
                (s_source_a *)local_13, (long)local_12, *(long *)((byte *)local_1 + 0xb4),
                local_9, (bool)((local_8 >> 8) & 1), TEST_FIELD_BIT(local_1->flags.flag5), false,
                false, local_7, &local_6,
                (point3f *)&local_1->forward, (point3f *)&local_1->control);
            if (!(bool)((*(dword *)local_12 >> 4) & 1))
            {
                local_10.field_14 = true;
                local_10.field_44 = *(long *)((byte *)local_1 + 0x1a8);
            }
            else
            {
                local_10.field_14 = true;
            }
            function_1e5a60(local_13, (s_biped_physics_result *)&local_11,
                (s_biped_physics_input const *)&local_10);
            if (local_11.field_4 & 1)
            {
                long local_14 = *((byte *)local_1 + 0x130) + 1;
                if (local_14 > 0xfe)
                    local_14 = 0xfe;
                *((byte *)local_1 + 0x130) = (byte)local_14;
                *((byte *)local_1 + 0x131) = 0;
            }
            else
            {
                long local_15 = *((byte *)local_1 + 0x131) + 1;
                *((byte *)local_1 + 0x130) = 0;
                if (local_15 > 0xfe)
                    local_15 = 0xfe;
                *((byte *)local_1 + 0x131) = (byte)local_15;
            }
            forward_flags_set(&local_1->flags, 0x100, (bool)(local_11.field_4 & 1));
            forward_flags_set(&local_1->flags, 0x80, (bool)(local_11.field_4 & 1));
            if (TEST_FIELD_BIT(local_1->flags.flag5))
            {
                real threshold_scale = (bool)((*(dword *)local_12 >> 3) & 1) ? 3.0f : 1.0f;
                if (*((byte *)local_1 + 0x131) * g_510c54->rate > threshold_scale * 0.15f)
                    forward_flags_set(&local_1->flags, 0x20, false);
            }
            else
            {
                real threshold_scale = (bool)((*(dword *)local_12 >> 3) & 1) ? 3.0f : 1.0f;
                if (local_7 || *((byte *)local_1 + 0x130) * g_510c54->rate > threshold_scale * 0.15f)
                    forward_flags_set(&local_1->flags, 0x20, true);
            }
        }
    }
}

void (__stdcall *g_4685ac)(long) = function_118b80;


struct s_biped_ground_collision;
struct s_type_1e6529;
struct s_damage_owner;
struct s_119c10
{
    long field_0;
    real field_4;
    point3f field_8;
    byte field_14[8];
    s_location field_1c;
    short field_24;
    byte field_26[2];
    vector3f field_28;
    byte field_34[0xc];
    long field_40;
    short field_44;
    short field_46;
    byte field_48[0x12];
    short field_5a;
};

struct s_119c11
{
    long field_0;
    byte field_4[0x84];
};

short __stdcall function_bb050(long arg_0, dword arg_1, void const *arg_2, point3f const *arg_3,
    real arg_4, long *arg_5, short arg_6);
bool function_1df560(short arg_0, short arg_1);
bool function_1696d0(long arg_0, s_biped_ground_collision *arg_1, long arg_2,
    point3f const *arg_3, vector3f const *arg_4, long arg_5, long arg_6);
void function_d6660(s_type_1e6529 *arg_0, long arg_1);
void __stdcall object_get_damage_owner(long arg_0, s_damage_owner *arg_1);
void __stdcall function_d7b80(s_type_1e6529 *arg_0, long arg_1, short arg_2, short arg_3, short arg_4, vector3f const *arg_5);
void __stdcall function_d6bc0(long arg_0);
void function_b75a0(long arg_0, point3f const *arg_1, vector3f const *arg_2, vector3f const *arg_3,
    s_location const *arg_4, bool arg_5);
void __stdcall function_b93b0(long arg_0, long arg_1, short arg_2);
static __inline real squared_length_jk(vector3f const *v)
{
    real length_squared = v->j * v->j;
    length_squared += v->k * v->k;
    return length_squared;
}
static __inline void vector3f_negate(vector3f const *v, vector3f *r)
{
    r->i = 0.0f - v->i;
    r->j = 0.0f - v->j;
    r->k = 0.0f - v->k;
}
// @retail 0x119c10
long function_119c10(long arg_0)
{
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    long local_1 = NONE;
    vector3f local_2;
    function_ba1d0(arg_0, &local_2, NULL);
    real local_3 = (real)sqrt(squared_length_jk(&local_2) + local_2.i * local_2.i);
    if (local_3 > 0.0001f)
    {
        real local_4 = *(real *)((byte *)local_0 + 0x3c) + 0.1f;
        real local_7;
        long local_10;
        long local_11;
        long local_12;
        point3f local_13;
        vector3f local_14;
        s_location local_15;
        /* the scan's scope: its scaled velocity gives up its frame slot to the later output vector */
        {
            vector3f local_6;
            real local_5 = local_4 / local_3;
            vector3f_scale(&local_2, local_5, &local_6);
            local_7 = 3.402823466e+38f;
            long local_8[16];
            long local_9 = function_bb050(1, 3, (byte *)local_0 + 0x28, &local_0->center, local_4, local_8, 16);
            for (long local_16 = 0; local_16 < local_9; local_16++)
            {
                long local_17 = local_8[local_16];
                s_forward_object *local_18 = ((s_forward_object_header *)g_4e0300->data)[local_17 & 0xffff].object;
                if (!TEST_FIELD_BIT(local_18->frozen)
                    && function_1df560(*(short *)((byte *)local_0 + 0x12e), *(short *)((byte *)local_18 + 0x138)))
                {
                    s_119c10 local_19;
                    local_19.field_24 = NONE;
                    if (function_1696d0(0x1800038, (s_biped_ground_collision *)&local_19, local_17,
                        &local_0->center, &local_6, NONE, NONE) && local_19.field_0 == 4)
                    {
                        byte *local_20 = g_4e0300->data + (local_19.field_40 & 0xffff) * 12;
                        if (!local_20[3] && local_7 > local_19.field_4)
                        {
                            local_1 = local_19.field_40;
                            local_11 = local_19.field_46;
                            local_10 = local_19.field_44;
                            local_12 = local_19.field_5a;
                            local_13 = local_19.field_8;
                            local_14 = local_19.field_28;
                            local_15 = local_19.field_1c;
                            local_7 = local_19.field_4;
                        }
                    }
                }
            }
        }
        if (local_1 != NONE)
        {
            byte *local_21 = g_4e3b44[local_0->definition_index & 0xffff].bytes;
            long local_22 = *(long *)(local_21 + 0x16c);
            s_forward_object *local_23 = ((s_forward_object_header *)g_4e0300->data)[local_1 & 0xffff].object;
            bool local_24 = false;
            if (local_23->valuef0 > 0.0f)
            {
                local_24 = (bool)((*(dword *)(local_21 + 0xbc) >> 4) & 1);
                if (*(long *)(local_21 + 0x174) != NONE)
                    local_22 = *(long *)(local_21 + 0x174);
            }
            if (local_22 != NONE)
            {
                s_119c11 local_25;
                *(short *)((byte *)&local_25 + 0x7c) = NONE;
                function_d6660((s_type_1e6529 *)&local_25, local_22);
                object_get_damage_owner(arg_0, (s_damage_owner *)((byte *)&local_25 + 8));
                *(s_location *)((byte *)&local_25 + 0x1c) = local_15;
                *(point3f *)((byte *)&local_25 + 0x24) = local_13;
                *(point3f *)((byte *)&local_25 + 0x30) = local_0->center;
                function_d7b80((s_type_1e6529 *)&local_25, local_1, local_11, local_10, local_12, &local_14);
            }
            if (local_24)
                function_d6bc0(arg_0);
            else if ((bool)((*(dword *)(local_21 + 0xbc) >> 5) & 1))
            {
                vector3f_negate(&local_14, &local_2);
                vector3f local_26;
                vector3f local_30;
                function_11d180(&local_2, &local_0->up, &local_2, &local_26, &local_30);
                function_b75a0(arg_0, &local_13, &local_2, &local_30, &local_15, false);
                function_b93b0(local_1, arg_0, (short)local_11);
                real local_27 = g_510c54->field_2_3 * 0.2f;
                long local_28;
                __asm
                {
                    fld local_27
                    fistp local_28
                }
                *((byte *)local_0 + 0x204) = (byte)local_28;
            }
            else
            {
                real local_29 = (0.0f - dot3f(&local_14, &local_2)) * 1.7f;
                if (0.4f > local_29)
                    local_29 = 0.4f;
                vector3f_add_scaled((vector3f const *)((byte *)local_0 + 0x88), &local_14, local_29, &local_2);
                function_b7740(arg_0, &local_2, NULL, false);
                function_1c4b00(arg_0, &local_2, NULL, 1);
                if (length_sq3f(&local_2) > 0.0001f)
                {
                    function_b9b90(arg_0, false);
                    function_b7360(arg_0);
                    function_bba20(arg_0);
                }
            }
        }
    }
    return local_1 != NONE;
}


#include "unknown_1eb550.h"

struct s_biped_physics_output;
struct s_biped_physics_move;
struct s_118700
{
    long field_0;
    byte field_4[0x14c];
};

struct s_118701
{
    byte field_0;
    byte field_1[3];
    long field_4;
    byte field_8;
    byte field_9[3];
    vector3f field_c;
    vector3f field_18;
    vector3f field_24;
    byte field_30[0x40];
};

void function_1e5bb0(s_biped_physics_output *arg_0, void *arg_1, void *arg_2, real arg_3,
    long arg_4, long arg_5, void const *arg_6, long arg_7, bool arg_8, bool arg_9,
    bool arg_10, bool arg_11, bool arg_12, bool arg_13, bool arg_14, bool arg_15,
    real arg_16, real arg_17, vector3f const *arg_18, point3f const *arg_19,
    vector3f const *arg_20, vector3f const *arg_21, vector3f const *arg_22,
    vector3f const *arg_23, vector3f const *arg_24, long arg_25);
void function_1e6360(s_biped_physics_output *arg_0, real arg_1, long arg_2, real arg_3);
void function_1e5af0(void *arg_1, vector3f const *arg_3, vector3f const *arg_2, s_biped_physics_output *arg_0);
void function_1e6120(s_biped_physics_output *arg_0, void *arg_1, real arg_2, bool arg_3, bool arg_4,
    bool arg_5, real arg_6);
void __stdcall function_1e55d0(s_biped_physics_move *arg_0, void *arg_1, s_biped_physics_output *arg_2);

// MiniMax B (11-13): no new form, no lower score (92): aligned the two listings instruction by instruction: the two
// squares come out of the other two homes (retail reads 0x1f0 before 0x1e8, this build reads 0x1e8 before 0x1f0), and
// retail keeps the 0x119bc0 result in a stack byte it stores and reloads around the three writes where this build keeps
// it in al; that shape comes from 0x1c4b00's last parameter, which is a long in the files this lane may not edit.
// MiniMax S (11-14): no new form, no lower score (92): the frame is retail's (0x1f0) and the squared length is still summed in the compiler's own order, the re-fetched object's copy still loads the object before the first component where retail loads the component first, and the 0x119bc0 result still travels in al where retail keeps it in a stack byte and pushes the containing dword; that shape needs 0x1c4b00's last parameter to be a bool in its definition and in its thirteen declarations, which live in files this lane may not edit. Still BLOCKED-BY 0x1c4b00.
// MiniMax B (11-12): no new form, no lower score (92); re-read the listing against the body: the only
// logical difference left is the 0x119bc0 result, which still travels in al where retail keeps it in a stack
// byte and pushes the containing dword; that shape comes from 0x1c4b00's last parameter being a bool in its
// definition and in its thirteen declarations, which live in files this lane may not edit, and the squared
// length is still summed in the compiler's own order. Still BLOCKED-BY 0x1c4b00.
// MiniMax B (11-11): no new form, no lower score (92); still BLOCKED-BY 0x1c4b00's force parameter, declared long in files
// this lane may not edit, so the result byte's shape cannot be produced here.
// MiniMax C (11-11): no new form kept (92); the squared length written as three statements with an accumulator
// gives retail's k, j, i product order and 90, and the shared length_sq3f gives the same 92 as the expression, but
// a two-point gain under the keep bar was reverted; the re-fetched object's copy still loads the object before the
// first component where retail loads the component first, and 0x1c4b00's force parameter is still declared long in
// src/unknown_1c25a0.cpp, so its movzx cannot be produced here.
// MiniMax I (11-10): no new form, no lower score (92); the copy's destination is still reached with a
// separate add where retail folds the index into the base load, and the 0x119bc0 result still travels in
// al where retail keeps it in a stack byte; the latter needs 0x1c4b00's last parameter to be a bool in a
// file this lane may not edit.
// MiniMax D (11-10): no new form, no lower score; the 0x119bc0 result still travels in al where retail
// stores it to a stack byte and pushes the containing dword, and that shape comes from 0x1c4b00's last
// parameter being a bool, whose declarations sit in files this lane may not edit; the squared length is
// still summed in the compiler's own order. This round: the 0x119bc0 result in a dword local 93 and the
// squared-length sum written the other way round 92, both reverted.
// score 92 (was 94): frame 0x1f0 agrees; basis MOV/LEA, squared-component loads and force-result
// publication differ; the force formal still requires a zero extension. Round 85 (m3 lane 16): 0x1e5af0's
// parameters as (physics, forward, up, output) give the first 0x1e5af0 call retail's EAX/ECX/EDX setup (94 -> 92).
// MiniMax H (11-04): the squared length written as i*i + j*j + k*k 94; the component order
// does not move.
// Earlier (94, was 94): frame 0x1f0 agrees; basis MOV/LEA, squared-component loads and force-result
// publication differ; the force formal still requires a zero extension.
// Earlier: retail pushes the whole found-flag dword to 0x1c4b00 and keeps its MOV/LEA
// order at the first 0x1e5af0 call; both callees live in files this lane may not edit
// (BLOCKED-BY 0x1c4b00 and 0x1e5af0). Frame 0x1f0 agrees; call setup, movement loads and
// force-call publication still differ.
// Retail spills the found flag to a stack byte and pushes the whole dword to 0x1c4b00 (no zero-extension), so
// 0x1c4b00 takes a bool; ours declares a long and zero-extends, and every later instruction is one line off.
// Round 37: 0x1c4b00's flag as a bool in its definition and every declaration gives 44 (0x1c4b00 and its matched
// callers keep matching; 0xe24f0's resting call then has retail's shape too, but its count rises 742 -> 748 by a
// shift, so it was reverted); with it, the squared length through length_sq3f on a vector3f field at output
// offset 0x138 (retail's k, j, i loads) 42; that read alone 92. Then left: the MOV/LEA order at the first
// 0x1e5af0 call and, from the velocity copy into the re-fetched object on, retail loading the source's i into
// ECX before the object pointer, with every later scratch register following that rotation. On the 42 form:
// the object fetched inside the copy (cast or a `velocity` field at 0x88) 42/42; a velocity field through the
// object local 42; a pointer to the stepped velocity used by the copy and the call 42; the copy written per
// component 103 (1051 bytes).
// Earlier, with the long force flag: stationary/byte/word movement result 107/98/111; complete flag value 98;
// initial-state/parent/update/countdown activity values 265/119/98/101; branch flag store 131; long force 130;
// native velocity copy 131; long activity 164; reversed result OR 131; parent result earlier 123;
// update/countdown result earlier 131; with the original flag store, early parent result 109; long force 101;
// both 110; native motion/input/state fields 98 each; request/output/all basis copies 98 each; movement-output and
// orientation-output scopes 98 each; the flag-4 store through the shared flags setter, the countdown reset stored
// as NONE (EBX holds NONE as in retail) and the countdown decremented on the field give retail's code there; a
// named output pointer 92; squared length summed k, j, i as statements 2 fewer; bitfield store with the NONE reset
// 108; branch store 119; ternary store 119; force byte/word/signed-byte 94/16/94; readonly metadata 94; metadata
// with byte/word force 94/16; absent-parent first 94, with byte force 94; result homes/movement scope/early
// activity/direct velocity receiver/refresh names/read-only movement 94 each; conditional orientation success 112;
// short activity 168; squared length in i, j, k order, through the shared squared-length or dot helper 94 each; a
// vector3f field at 0x138 of the output read directly 92, with a word force result 14; named output, up and
// forward pointers 94/92 with the field; word force with ordered squares/native publication/output receiver
// 14/16/16, and the pairs and all three 14/14/16/14, with the final mask setter 14; direct force query 101;
// velocity-input/complete-input/nested-input scopes 94/94/94, with the direct query 101 each, with reversed
// orientation result 101; existing rounding helper 101; chained sentinel words 96; native sentinel library fill
// 278; byte parent countdown 94; postfix parent countdown 100; shared final flag clear 94; signed short force 16;
// rounding helper with signed force 62; object/input/both flags read at each use 156/132/121; existing basis
// helper inline/noinline declarations 94/94, inline with those observations 156/132/121; existing vector
// assignment at requested/output-basis/velocity copy masks 1-7 269/149/290/101/280/162/302, all copies with the
// terminal flag setter 302.
// Round 104 (long force flag): first movement-flag test inverted 175; physics block address at each use 94;
// parent countdown arms inverted 94; motion test passed straight to the flag setter 94; word found flag 16
// (same lines, a length coincidence). Still BLOCKED-BY 0x1c4b00's long force parameter.
// Round 47: real output workspace with pointer/direct movement field 94/94; real movement workspace 94;
// two-use velocity copy 109; read-only query result 94; split query declaration 94; late declarations
// swapped 94; complete movement workspace 103 (frame 0x1ec). Original body restored.
// Round 43 (lane 16): still BLOCKED-BY 0x1c4b00 (its definition is in a file held by another lane this round;
// retail spills the found flag to a stack byte and pushes the whole dword) and the MOV/LEA order at the first
// 0x1e5af0 call follows that callee's register setup (BLOCKED-BY 0x1e5af0, also in a held file). Squared length
// grouped (k + j) + i / (j + k) + i 94/94 (the inner pair stays j, k); word found flag with the grouped squares
// 16 (exact-only: left are the MOV/LEA pair, the squares and the flag's stack byte); the squared-length test
// passed straight to the flag setter 94; the +0x17c block address at each use 94.
// Round 100 (lane 7, plain unless noted): 0x1c4b00's force as bool in its definition and all 13
// declarations 44 (every matched caller still matches; 0xe24f0 648 -> 654, its over-long listing
// shifted by the retail-shaped stack-byte push), with length_sq3f on the 0x138 vector through a
// pointer 44, through a vector3f field of the output (--fast) 42; on the long form the 0x138 squared
// length through dot3f 94; the found flag held in a long local (--fast) 95.
// Round I (MiniMax lane I, fast): its 42 came from dropping the function_119bc0 call and passing the flag
// uninitialised (a behaviour change, not kept); with the call kept and the squared length summed as three
// statements 92, as one expression 94. Original body restored.
// Round 115 (fast, agreeing with the full count here): BLOCKED-BY 0x1c4b00 still: its long force parameter is
// declared in three files other lanes hold this round (projectiles.cpp, unknown_0a76b0.cpp, unknown_1eb8a0.cpp).
// 0x1e5af0's parameters as (physics, up, output, forward), definition, declarations and all five calls, 92: the
// MOV/LEA order at the first call becomes retail's and 0x1e5af0 and its other callers keep their counts (retail's
// setups at 0x118700 and 0xe3c90 load EAX, ECX, EDX in that order; 0xdd360's loads ECX first); length_sq3f on a
// vector3f field at 0x138 of the output 92 (retail's k, j, i loads), through the existing pointer 94; both 90;
// the countdown at 0x206 in a short local 94; as a word local with `(short)countdown <= 0` 90 (retail's
// `xor eax, eax; mov ax` load and the first difference moves to +0x1bd, but `add eax, 0xffff` for retail's
// `dec eax`); a short `remaining = word - 1` local, with or without a word local, 94 (same code).
// J228: query/reset inline contracts singly and paired 94/94/94; each with all three squared
// components named 94/94/94; the pair with the existing final flag setter, alone/staged, 94/94.
// All five matched controls survive. Original declarations and body restored; the byte force
// contract still needs coordinated declarations in protected contributor files.
// Round 226: overlap-safe library copies at requested/output-basis/velocity masks 1-7:
// 263/148/272/54/265/154/275; contiguous paired axes 314. Velocity-only memmove 54/1034 not kept:
// retail copies inline, so the memmove call is one retail does not make. Original copy restored;
// basis MOV/LEA, component loads and force publication remain.
// Round 278: final frozen/flag/parent conjunction orders 021/102/120/201/210
// 101/101/100/96/96; reversed timestamp predicate with 012/201/210 94/96/96. Restored.
// M305: Complete physics-input owner/object-flags/input-flags/settings declaration orders 0132/0312/1023/1203/1230/2013/2103/3120 132/121/94/94/94/143/143/121.
// Round 93: bool/dword force union 44; with component stores 51; existing vector/point setters 56/56;
// memcpy/direct receiver 44/44; staged squares 42; preloaded-i component stores 49; complete updated vector 135.
// NOT kept: the exact-only union wrapper, one-use velocity local and value snapshot. Original body restored.
// Round 65 (m3 lane 1, fast, control 94): the 0x119bc0 result passed straight into the 0x1c4b00 call 101; the moving
// test written into the flag-setter call 94; object flags read at each use (no snapshot) 156; the physics word at
// 0xc0 read at each use 132; the requested up/forward copies swapped 98. The bool force flag still waits on files
// other lanes hold.
// M3 lane 14 r59: 0x1c4b00's last parameter as `bool force` (its `(byte)force` casts dropped, every declaration
// changed) 44 plain at 1034 bytes: the 0x119bc0 result is then stored to its stack byte and pushed as a whole
// dword as retail does, with 0x1c4b00 and all nine matched callers still matching (0xe24f0 648 -> 654). Not kept:
// the declarations sit in files other lanes held this round (held change, BLOCKED-BY 0x1c4b00's declarations).
// What remains at 44 is the scratch-register order of the forward copy into the object (Round 93's union forms).
// On the current body (fast): the existing length_sq3f for the squared length 94 (same code: VC orders the
// products itself); one physics-output pointer local for its six casts 94 (same code); up/forward pointer locals for
// the stores and both 0x1e5af0 calls 289 (1042 bytes); the definition-flags pointer declared beside the tag 273.
// Round 159 (lane 5, fast with the file's 18 other functions, none moved): still BLOCKED-BY 0x1c4b00's `long force` parameter,
// declared in unknown_0a76b0.cpp, unknown_1e67a0.cpp and unknown_0b7740.cpp, which other lanes hold this round (the bool form
// was not re-measured). Retail keeps the 0x119bc0 result in a stack byte to free EAX for the copy's destination and loads two
// of the copy's components before the first store, with the force push ahead of the last store. A vector3f member at 0x138 of
// the output with length_sq3f 90, that passed straight to the flag setter 90, through a named real 90 (two points, not kept);
// the velocity copy through the first object pointer (no second lookup) 307; the 0x119bc0 result as `!= false` 93. The two
// stacked score lines above are merged into this one note.
// Round 62 (m3 lane 2; plain unless noted, the fast check carries a CAVEAT for callee 0x1197b0): the score is mostly a
// one-line shift: the `long force` argument is one `movzx` where retail spills the bool to a stack byte and pushes the
// dword (two instructions), so every row after the 0x1c4b00 call sits one row off. The velocity copy as three component
// stores 103 (fast, 1046 bytes); the object pointer for the copy written in place (no one-use local) 92 (same code); a
// vector3f member at 0x138 of the output through length_sq3f 90 (retail's k, j, i loads; two points, as round 159 found);
// the force argument as `found ? 1 : 0` 90 (two extra rows that happen to realign the tail: a redundant conversion, not
// kept); the member form with the in-place object pointer 90; the copy through memcpy 92; the squared-length test written
// `0.0f < sum` 92. Still BLOCKED-BY 0x1c4b00's `long force` parameter (src/unknown_1c25a0.cpp and its declarations,
// held by other lanes this round).
// MiniMax R (11-10): no new form, no lower score (92); the squared length still sums the three components in the order the compiler picks (i, j, k) where retail sums them from k down to i, and three component locals that follow retail's order score 90, under the bar; the 0x119bc0 result still travels in al where retail stores it to a stack byte and pushes the containing dword. This round: declaring the force a bool in this file alone gives 44, but it breaks the two other calls in this file (0x119020 0 -> 141 and 0x119c10 0 -> 13), because the definition in src/unknown_1c25a0.cpp still declares it a long and the call sites then take a different register shape; the definition and all fourteen declarations of 0x1c4b00 have to change together.
// MiniMax R (11-10): no new form, no lower score; the squared length still sums the three components in the order the compiler picks (i, j, k) where retail sums them from k down to i, and three component locals that follow retail's order score 90, under the bar; the 0x119bc0 result still travels in al where retail stores it to a stack byte and pushes the containing dword, which needs 0x1c4b00's last parameter declared as a bool in a file this lane may not edit.
// Round 86 (m3 lane 11, fast; aligned / register-masked 17 / 16 at 92, so the score is mostly the one-row shift from the
// `movzx` of the force argument): `if (load) result = true;` for the 0x119380 `|=` 110 (masked 21); the frozen test as
// `result = (flags & 1) != 0; if (!result)` 272; the definition's physics bit read once into a named bool for both tests
// 271 (1019 bytes); a named pointer to the countdown word at 0x206 for its three uses 92 (same code); an early `return
// false` for the 0xb4 test 276 (1039 bytes). Still BLOCKED-BY 0x1c4b00's `long force` parameter (the bool form, 44,
// needs declarations in src/bipeds.cpp, a contributor's, and in four files other lanes hold this round).
// @retail 0x118700
bool __stdcall function_118700(long arg_0)
{
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    byte *local_1 = g_4e3b44[local_0->definition_index & 0xffff].bytes;
    s_animation_state *local_2 = (s_animation_state *)((byte *)local_0 + local_0->animation_state_offset);
    s_118700 local_3;
    *(short *)((byte *)&local_3 + 0x50) = NONE;
    *(short *)((byte *)&local_3 + 0x52) = NONE;
    bool local_4 = false;
    if (*(long *)((byte *)local_0 + 0xb4) != NONE)
    {
        point3f local_5;
        function_b9dd0(arg_0, &local_5);
        if (local_0->flags.value & 1)
            local_4 = true;
        else
        {
            function_118f90((s_orientation_request_118f90 *)((byte *)local_0 + 0x13c));
            local_0->requested_forward = local_0->forward;
            local_0->requested_up = local_0->up;
        }
        s_119380 local_6;
        local_4 |= function_119380(arg_0, &local_6);
        if (local_0->parent_index == NONE)
            function_1197b0(arg_0);
        vector3f local_7;
        function_ba1d0(arg_0, &local_7, NULL);
        byte *local_9 = (byte *)local_0 + 0x17c;
        s_forward_flags object_flags = local_0->flags;
        word local_10 = *(word *)((byte *)local_0 + 0xc0);
        byte *local_8 = local_1 + 0xd4;
        function_1e5bb0((s_biped_physics_output *)&local_3, local_9, local_2, 1.0f,
            *(long *)((byte *)local_0 + 0xb4), arg_0, local_8, 0xf,
            (bool)((local_10 >> 6) & 1), false, true, false,
            (bool)((local_10 >> 8) & 1), TEST_FIELD_BIT(object_flags.flag5),
            (bool)((object_flags.value >> 8) & 1), false, g_51e9c4->unknown0, 1.0f,
            &local_0->control, &local_5, &local_0->forward, &local_0->up,
            &local_0->forward, &local_0->forward, &local_7, 0);
        if ((bool)((*(dword *)local_8 >> 4) & 1))
        {
            function_1e6360((s_biped_physics_output *)&local_3, local_0->turn, arg_0, 0.0f);
            function_1e5af0(local_9, &local_0->forward, &local_0->up, (s_biped_physics_output *)&local_3);
        }
        else
            function_1e6120((s_biped_physics_output *)&local_3, local_9, 0.0f, false, false,
                TEST_FIELD_BIT(local_0->flags.flag6), 0.0f);
        s_118701 local_11;
        function_1e55d0((s_biped_physics_move *)&local_11, local_9, (s_biped_physics_output *)&local_3);
        if ((bool)((*(dword *)local_8 >> 4) & 1))
        {
            local_0->forward = local_11.field_18;
            local_0->up = local_11.field_24;
            function_1e5af0(local_9, &local_0->forward, &local_0->up, (s_biped_physics_output *)&local_3);
        }
        vector3f *local_12 = (vector3f *)((byte *)&local_3 + 0x138);
        bool local_13 = local_12->k * local_12->k + local_12->j * local_12->j + local_12->i * local_12->i > 0.0f;
        forward_flags_set(&local_0->flags, 0x10, local_13);
        bool local_14 = function_119bc0(arg_0);
        s_forward_object *local_15 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
        *(vector3f *)((byte *)local_15 + 0x88) = local_11.field_c;
        function_1c4b00(arg_0, &local_11.field_c, NULL, local_14);
        function_119500(arg_0, local_11.field_4, (s_orientation_request_118f90 *)&local_6);
        function_1195c0(arg_0, (s_orientation_request_118f90 *)&local_6);
        if (!TEST_FIELD_BIT(local_0->frozen) && (*((byte *)local_0 + 0x144) & 8)
            && local_0->parent_index == NONE)
            function_119c10(arg_0);
        if (local_0->parent_index != NONE)
        {
            if (!--*((char *)local_0 + 0x204))
            {
                function_11a140(arg_0);
                real local_16 = g_510c54->field_2_3 * 0.2f;
                long local_17;
                __asm
                {
                    fld local_16
                    fistp local_17
                }
                *((char *)local_0 + 0x204) = (char)local_17;
            }
            local_4 = true;
        }
        else
            *((char *)local_0 + 0x204) = NONE;
        if (!TEST_FIELD_BIT(local_0->frozen) || !(*((byte *)local_0 + 0xc1) & 1))
        {
            *(long *)((byte *)local_0 + 0xbc) = g_510c54->game_time;
            function_bba20(arg_0);
            local_4 = true;
        }
        if (*(short *)((byte *)local_0 + 0x206))
        {
            if (--*(short *)((byte *)local_0 + 0x206) <= 0)
                function_d6bc0(arg_0);
            local_4 = true;
        }
        local_0->flags.flag6 = false;
    }
    return local_4;
}

bool (__stdcall *g_4685a8)(long) = function_118700;
