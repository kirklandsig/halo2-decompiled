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
bool function_1faf80(vector3f *facing, long object_index, void const *settings_pointer,
	vector3f const *control, real threshold, real *turn);
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
// @retail 0x1197b0
void function_1197b0(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_facing_definition_1197b0 *definition = (s_facing_definition_1197b0 *)g_4e3b44[object->definition_index & 0xffff].bytes;
	if ((bool)((definition->flags >> 4) & 1))
	{
		function_1faf80(&object->requested_forward, object_index, definition->settings, &object->control, 0.9f, &object->turn);
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
bool function_bc380(long arg_0, long arg_1, long arg_2, long arg_3);

// score 83 (was 83): retail puts the word at +0xb4 in CX and the word at +0xb6 in AX, so the size
// reaches 0xbc380 as a 16-bit argument; our shared declaration takes a long, and the table base
// and the looked-up object stay in other registers. Also: the object table base and the looked-up
// object are kept in different registers, one word field is loaded zero-extended here and as a plain
// word in retail, and the
// value stored at the 0x134 slot is sign-extended differently. Retail passes the size to 0xbc380
// as a 16-bit argument (xor eax, eax; mov ax, ...), which needs that callee's shared declaration
// changed (BLOCKED-BY 0xbc380: a contributor's file declares it with a long size).
// Round 81: success path inside if (!= NONE) with return false after 88
// (registers right, false epilogue laid out last); return false at the guard 96 (laid out in
// line); result from the guard test 108; || on the two bools 95; result set in both arms 88
// Four-use long ordinal with retained/per-use tag and bitwise/logical predicates
// 83/89/129/136; native word ordinal 151/164/144/155. Initial table/object and
// result homes still differ; original body and shared allocator interface restored.
// Round 414: Post-reset table/object/result homes, allocation-size acquisition and saved-key tests
// remain. Native external input fields at selected/controller/allocation subsets 1-7 scored 83 each;
// complete native input with promoted repeated selection 90. Original void input, callback declaration
// and byte-offset reads restored.
// Round 46: inner success return 132; positive guard with direct false exit and inner return 88;
// separate predicate tests on that graph 95; initial/post-reset/post-allocation/final/all object getters
// 95 each. Original body restored.
// Round 104: false result in an else arm of the guard 88 (retail's registers and false epilogue, but that
// block is laid out after the other out-of-line arms where retail puts it first); with || on the two
// bools 98; the false arm written first 96 (laid out in line); the bools declared the other way 88; the
// two state fields read the other way 88 (load order and compare order swap together).
// Round 100 (lane 7): the animation block is 0x119650 inlined with constant arguments (mode and set
// 0x7000001, 0.0 seconds, flags 2): calling the existing function gives retail's block code, but NONE
// is then hoisted into EBX and the index left in its stack home (frame 8) 120; with the early false
// return 119; with the success path inside `if (!= NONE)` 123. That success-path form alone 88 with
// `||` on the two bools 95. The goto to the single exit replaced by an `if (!= NONE)` block 83 (kept).
// Round I (MiniMax lane I, fast): the guard as `if (... != NONE) { ...; return local_4; } return false;`
// with the +0xb6 word read into a one-use word local 35 (match-only form: retail zero-extends the word
// with xor eax,eax before the load); the same without the local 81. On the 35 form the tail predicate as
// `||` over two named bools 56, `|` over two named bools 48, `||` inline 56, `else if` with the call
// duplicated 56, the guard inverted with an early `return false` 76. Original body restored.
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
    bool local_4 = false;
    if (local_3->animation_state_offset != NONE)
    {
        local_4 = true;
        short local_5 = *(short const *)((byte const *)arg_1 + 0xb4);
        if (local_5 != NONE)
        {
            *(long *)((byte *)local_0 + 0x134) = local_5;
            local_4 = function_bc380(arg_0, 0x138, *(word const *)((byte const *)arg_1 + 0xb6), 0) != 0;
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
            s_forward_object *local_7 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
            s_animation_state *local_8 = (s_animation_state *)((byte *)local_7 + local_7->animation_state_offset);
            long local_9 = local_8->unknown70;
            long local_10 = local_8->unknown7c;
            if (local_8->animation_set(0x7000001, 0x7000101, 0x7000101, 0x7000001, 2, 0x3f))
            {
                bool local_11 = local_9 != 0x7000001;
                bool local_12 = local_10 != 0x7000001;
                if (local_11 | local_12)
                    function_ba350(arg_0, 0.267f);
            }
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
// score 156 (was 156): retail keeps the quotient in a register and computes the scaled direction
// before the sweep query; ours spills the quotient, computes it after the query, and the sweep
// and point homes are swapped. Computing the scaled direction before the query gives 292.
// Frame 0x124 agrees; quotient/query, sweep/impact/location homes and word loads remain.
// Round 67: query count declared after saved outputs 156; existing assignment helper after the
// query 156 and before it 292; pre-query helper with later best fraction 283; explicit pre-query
// stores with saved outputs before count 283; pre-query long saved words 285 and first/third/
// second acquisition 285; query-first long saved words with first/third/second acquisition 153
// (three-point gain restored).
// Retail's homes (frame offsets): selected 0xc, best 0x10, second word 0x14, third 0x18, normal 0x1c,
// first word 0x28, location 0x2c, point 0x34, sweep 0x40, count 0x4c; ours keep the count at 0x2c and
// the sweep before the point (0x38/0x44). Round 83: sweep before the query 292 (its products now
// schedule after a hoisted constant); count declared before the query 156; sweep declared before the
// buffer 156; saved records declared before the sweep 156; descriptive sweep/impact names 156; first
// and second word names swapped 156 (homes unchanged).
// Round 44: short/long-unit adjacent word bitfields 151/339; first-third-second words 153;
// word/handle acquisition order 218; normal between words 162; sweep in radius/center argument
// evaluation 293/293; named radius on the current frame 151. Opening body restored.
// Frame 0x124 agrees; quotient/query, sweep/impact/location homes, word loads and owner-call transport remain.
// Earlier: Frame 0x124 agrees; quotient spill, sweep/impact/location homes and owner-call setup remain.
// frame 0x124 agrees; query/sweep timing and owner-call setup differ.
// frame 0x124 agrees; query/sweep timing and owner-call setup differ.
// Complete request type 278; scale at each use 278; aggregate sweep 278;
// point/normal/location copies first 278; target size optimization 536.
// Query before sweep 159; distinct later-up output 288; double best fraction 255;
// named radius field 278; const sweep with distinct later-up output 288.
// Sweep/impact homes and store scheduling still differ.
// targeted sweep name 278; sweep/impact names exchanged 278; completed normal reused 285; candidate
// countdown 284; header flag at use 278.
// Frame 0x124 agrees; quotient spill and sweep/impact homes remain. Native collision result 159; later best fraction 161; later quotient 281; candidate cursor 393; bool return 159 (fast).
// Frame 0x124 agrees; quotient/sweep/impact homes and owner-call setup remain.
// Long saved identifiers first 152, second 152, third 151 kept (plain); all three 154; early
// fraction 161; first/third 157; second/third 157; first/second 158 (fast).
// Quotient spill, sweep/impact homes and owner-call setup remain.
// Sweep components before query i/j/k/ij/ik/jk each 159; all components 293;
// aggregate sweep before query 293, with retained long third saved identifier.
// Frame 0x124 agrees; quotient/sweep/location homes and word loads remain.
// second identifier first 149; location home name 151; native request after lifetime gains 151; distinct
// up after lifetime gains 180; location before identifiers 218; long first with second first 154; long
// second with second first 154; both long with second first 150.
// quotient spill, sweep/impact/location homes and owner-call setup remain.
// Repeated quotient after query 287; native sweep array loop 731; exhausted-selection exit 290;
// selected handle after tuple 162; normal before point 160.
// Existing combined velocity helper 390; readonly candidate/tag/both 151/151/151; selected object before default index 151;
// separate collision result 151; helper with readonly paths 390; result/object/tag lifetimes 151.
// Private unscaled input/native in-place copy/shared in-place scale 337 each; point/location
// between words 162/164; second word then point 161; identifier declarations 151; timer home name
// 151. Quotient/sweep/impact homes and protected owner-call setup remain.
// Candidate index short/word 366/366; native query count 153; reflection conditional 151; with native count
// 153; ordinary while scan 151; fraction-first comparison 151; nested candidate admission 151. All restored.
// Complete speed square/all component squares 151/151; each with saved-word lexical homes
// 151/151; each with scalar homes 151/151; complete magnitude with both home groups 151;
// complete speed square with existing out-of-place scale 151 (fast).
// response flag result arms 361; complete reflection dot 151; shared reflection products 164; native reflection
// vector 164; complete response vector 185; existing rounding helper 151; response arms and collision fill 361;
// dot with native reflection vector 164; rounding with reflection products 163; rounding with reflection vector
// 163; rounding with native query count 154; rounding/count/reflection products 166.
// Numerator-first quotient 151; uniform sweep products 151; post-query native sweep copy 329;
// quotient/products and quotient/copy 151/329; early sweep with products/quotient/both
// 151/151/151.
// Immutable speed/radius/quotient/all scalars 151/151/151/151; native entry success 397;
// speed-test-produced success 317; immutable query count and candidate 151; native selected
// collision record 345 at retail length, with wrong entry frame. All restored.
// Probe/input/both workspace scopes 151/175/175; complete request library copies 151;
// probe/input/both scopes with request copies 151/175/175; actual selected-header byte 151.
// root accessor 151; candidate accessor 151; selected accessor 151; root candidate accessors 151; root
// selected accessors 151; candidate selected accessors 151; all object accessors 151; all native header
// 151.
// Round 137: double speed/radius/quotient, speed-radius/speed-quotient/radius-quotient/all 334,339,332,259,332,329,261;
// all with double selected fraction 337 (exact-only precision diagnostics; all restored).
// Round 332: Candidate/collision/eligibility inline contracts, each and every
// combination, 151; candidate/collision noinline pair 151. All restored.
// Round 149: Native unsigned first/second identifier words and third full-word carrier, all combinations;
// unsigned pair with second capture first [151, 151, 151, 151, 151, 151, 151, 149]; all restored.
// Round 220: Collision/effect/both request declarations before the scan 200/200/200;
// both at entry 203. Real request names, candidate-buffer name and both name groups 151/151/151;
// combined pre-scan requests with names 200. Wider request lifetimes grow frame to 0x180;
// original request ownership, query-first sweep and saved identifiers restored.
// Round 199: Ordinary input/scalars/count/saved words/vector names/late quotient/early sweep/readonly
// receivers 151/151/153/154/151/287/293/151. Consistent pool/header observations, alone, with word
// order, with independent saved words and with early sweep: 346/339/346/339/348/341/291/275.
// Round 201: Response storage: normal/sweep reorientation and sweep reflection 176/160/177; paired
// direction/reflection 193/174; consumed-input left output alone/with sweep reflection 176/194; saved-
// velocity left output 176. All restored.
// Round 352: native stored-velocity fields with direct, scalar-first, named-product and combined
// products 151 each; each with one three-use velocity snapshot 374 (frame 0x128). Original
// external record, response expressions, query-first sweep and saved identifiers restored.
// Round 80: sweep stored before the query 283 (count and location homes as retail, but the point and
// sweep homes swapped and j/k folded as q*j/q*k where retail loads j/k); with the point/sweep renamed
// 283; with the capture declarations first 283; through the existing vector3f_scale 283. The tail is
// 0xb77d0's body, which retail inlines here: calling the existing 0xb77d0 391 (stays a call), with a
// forceinline declaration 391. Original body restored.
// Round 7: response minimum as a ternary, response products scale-first, speed test summed j/k/i 156
// each (identical code); sweep before the query 283 (now the start through the call matches; the
// sweep stores land at 0x3c not retail's 0x48 and the candidate loop gains a jump over its alignment),
// with quotient-first products 283. The zero register (retail XMM3) decides the response arm.
// Lane 9 r29 (fast): role names for every local 156 and names flipping the 12-byte/8-byte
// alphabetical order 156 (identical code); sweep before the query with the captures declared
// first 292 (1329 bytes); sweep declared beside the scale 156; separate response vector 156.
// Round 12 (lane 10): sweep before the query with the maximum-fraction constant declared after it 283
// (the constant's early load and slot then as retail; the extra count is our loop entry jumping over
// 7 bytes of alignment where retail pads 6 and falls through, shifting every later line: retail
// computes and stores the sweep one component at a time in one register, j and k loaded then
// multiplied, where ours folds j/k as q*j/q*k); on that, component-first products 283 (same code),
// point declared ahead of the sweep 283 (same code). On the current body: the sweep renamed to sort
// before the point 156 and declared at the top of the block 156 (same code and homes: neither the
// name nor the declaration order moves these slots). All restored.
// DL175: Actual candidate-pointer countdown on query-first/long-third graph, with native short
// remaining, pre-query sweep and readonly candidate subsets [281, 287, 208, 278, 281, 287, 208, 278].
// Original indexed scan, query-first sweep and saved words restored.
// Round 230: unsigned source-record first/second/both words 156/156/156;
// signed full-width third bitfield, alone/with those source words, 156/156/156/156;
// all three signed full-width short bitfields 156. Original source record and body restored;
// owner-query argument setup now agrees; query/sweep, selected homes and merged word reads remain.
// Round 240: speed-failure exit/complementary response/shared double scale and their pairs/all
// 316/227/307/319/308/305/307; all with native query count 307. Original body restored;
// frame 0x124 agrees, quotient/query and selected workspace homes remain.
// Round 100 (lane 7): retail's sweep is i = q * v.i (copy q), j and k = v.j * q / v.k * q (load the
// component, multiply q), each stored before the next, q never consumed; with the sweep before the
// query every statement/operand order (48) and magnitude spelling (12) compiles to the same folded
// q * v code (compile-only screen). Magnitude through length_sq3f 310; the tail test through
// length_sq3f 156 and dot3f 156; the effect data as the file's typed damage-effect record 156; the
// parameter-address statement removed 156 (kept).
// @retail 0x119c10
long function_119c10(long arg_0)
{
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    long local_1 = NONE;
    vector3f local_2;
    function_ba1d0(arg_0, &local_2, NULL);
    real local_3 = (real)sqrt(local_2.i * local_2.i + local_2.k * local_2.k + local_2.j * local_2.j);
    if (local_3 > 0.0001f)
    {
        real local_4 = *(real *)((byte *)local_0 + 0x3c) + 0.1f;
        real local_5 = local_4 / local_3;
        real local_7 = 3.402823466e+38f;
        long local_8[16];
        long local_9 = function_bb050(1, 3, (byte *)local_0 + 0x28, &local_0->center, local_4, local_8, 16);
        vector3f local_6;
        local_6.i = local_5 * local_2.i;
        local_6.j = local_2.j * local_5;
        local_6.k = local_2.k * local_5;
        short local_10;
        short local_11;
        long local_12;
        point3f local_13;
        vector3f local_14;
        s_location local_15;
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
                        local_10 = local_19.field_44;
                        local_11 = local_19.field_46;
                        local_12 = local_19.field_5a;
                        local_13 = local_19.field_8;
                        local_14 = local_19.field_28;
                        local_15 = local_19.field_1c;
                        local_7 = local_19.field_4;
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
                local_2.i = 0.0f - local_14.i;
                local_2.j = 0.0f - local_14.j;
                local_2.k = 0.0f - local_14.k;
                vector3f local_26;
                function_11d180(&local_2, &local_0->up, &local_2, &local_26, &local_6);
                function_b75a0(arg_0, &local_13, &local_2, &local_6, &local_15, false);
                function_b93b0(local_1, arg_0, (short)(local_11));
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
                real local_29 = (0.0f - (local_14.k * local_2.k + local_14.j * local_2.j + local_14.i * local_2.i)) * 1.7f;
                if (0.4f > local_29)
                    local_29 = 0.4f;
                local_2.i = local_14.i * local_29 + *(real *)((byte *)local_0 + 0x88);
                local_2.j = local_14.j * local_29 + *(real *)((byte *)local_0 + 0x8c);
                local_2.k = local_14.k * local_29 + local_0->value90;
                function_b7740(arg_0, &local_2, NULL, false);
                function_1c4b00(arg_0, &local_2, NULL, 1);
                if (local_2.k * local_2.k + local_2.j * local_2.j + local_2.i * local_2.i > 0.0001f)
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
void function_1e5af0(void *arg_1, s_biped_physics_output *arg_0, vector3f const *arg_2, vector3f const *arg_3);
void function_1e6120(s_biped_physics_output *arg_0, void *arg_1, real arg_2, bool arg_3, bool arg_4,
    bool arg_5, real arg_6);
void __stdcall function_1e55d0(s_biped_physics_move *arg_0, void *arg_1, s_biped_physics_output *arg_2);

// score 94 (was 94): retail pushes the whole found-flag dword to 0x1c4b00 and keeps its MOV/LEA
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
            function_1e5af0(local_9, (s_biped_physics_output *)&local_3, &local_0->up, &local_0->forward);
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
            function_1e5af0(local_9, (s_biped_physics_output *)&local_3, &local_0->up, &local_0->forward);
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
