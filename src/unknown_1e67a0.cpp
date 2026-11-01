// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E67A0.CPP: character physics update input datum setters */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"

struct s_shape_contact;
struct s_shape_side;
struct s_contact;
struct s_contact_result;
struct s_tracking_result;
struct s_tracking_source;
struct s_tracking_target;
struct s_unknown_object;
struct s_unknown_output;

struct s_biped_physics_output;
struct s_biped_physics_input;
struct s_biped_physics_result
{
	long object_index;
	dword flags;
	long contact_index;
	long other_index;
};
struct s_shape_state;
struct s_state_c570;
#if 0 // Disabled: retail shape type virtuals and capsule construction need complete shared interfaces.
// These declarations describe the missing interfaces without installing shortened vtables.
struct c_movement_shape { long type(); };
struct __declspec(align(16)) c_movement_capsule {
	c_movement_capsule(void const *first, void const *second, real radius);
	~c_movement_capsule();
	byte storage[0x30];
};
struct s_shape_carrier_contact;
struct s_motion_adjustment_result;
struct s_motion_adjustment;
struct s_ease_input;
struct s_ease_output;
struct s_ease_state;
struct s_tracking_state;
struct s_tracking_output;
struct s_tracked_point;
struct s_contact_dispatch_output;
struct s_biped_physics_move;
struct s_havok_component;
extern real g_47f05c;
void __stdcall function_1f0450(s_shape_state *state, byte *out, s_shape_carrier_contact const *input);
void __stdcall function_1fa810(long *ticks, s_motion_adjustment_result *out, s_motion_adjustment const *input);
void function_1ec690(byte const *input, s_state_c570 *state, byte *output);
void function_1eb6f0(s_ease_input *input, s_ease_state *state, s_ease_output *output);
void function_1fc4b0(s_tracked_point *state, s_tracking_state const *input, s_tracking_output *output);
void function_1e5b50(byte const *input, s_contact_dispatch_output *output, byte const *state);
void function_b7740(long object_index, vector3f const *linear, vector3f const *angular, bool skip);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long mode);
void function_1c4a80(long object_index, bool first, bool second);
void function_b58c0(long object_index, dword mask);
void function_1d08e0(short index, s_havok_component *component, transform4x3f *matrix);
bool function_1c5210(transform4x3f const *matrix, void *shape, long component, long filter);

// @retail 0x1e55d0 disabled
void __stdcall function_1e55d0(s_biped_physics_move *move, void *physics, s_biped_physics_output *output)
{
	byte *out = (byte *)move;
	byte *state = (byte *)physics;
	byte const *input = (byte const *)output;
	*(long *)out = 0;
	*(vector3f *)(out + 0x24) = *(vector3f const *)(input + 0x100);
	*(vector3f *)(out + 0x18) = *(vector3f const *)(input + 0xf4);
	vector3f &velocity = *(vector3f *)(out + 0xc);
	velocity = *g_4687a4;
	*(long *)(out + 4) = 0;
	out[8] = false;
	out[0x54] = false;
	*(vector3f *)(out + 0x58) = *(vector3f *)g_468788;
	*(vector3f *)(out + 0x64) = *g_4687a4;
	if (state[1] > 0) --state[1];
	switch (*state)
	{
	case 1: function_1f0450((s_shape_state *)(state + 0x10), out, (s_shape_carrier_contact const *)input); break;
	case 2: function_1fa810((long *)(state + 0x10), (s_motion_adjustment_result *)out, (s_motion_adjustment const *)input); break;
	case 3: function_1ec690(input, (s_state_c570 *)(state + 0x10), out); break;
	case 4:
	case 5: function_1fc4b0((s_tracked_point *)(state + 0x10), (s_tracking_state const *)input, (s_tracking_output *)out); break;
	case 6:
		state[1] = (byte)real_to_long((real)g_510c54->field_2_3 * 0.3f);
		function_1eb6f0((s_ease_input *)input, (s_ease_state *)(state + 0x10), (s_ease_output *)out);
		break;
	default: __assume(0);
	}
	function_1e5b50(input, (s_contact_dispatch_output *)out, state);
	c_movement_shape *shape = *(c_movement_shape **)(input + 0x14c);
	if (shape)
	{
		byte *component = g_51e9b8->data + (*(long *)(input + 0x18) & 0xffff) * 0xa0;
		byte *body = *(byte **)(*(byte **)(component + 0x70) + 0x40);
		c_movement_shape *current = *(c_movement_shape **)(body + 0xc);
		bool same = current == shape;
		if (current->type() == 0x17) same = same || *(c_movement_shape **)((byte *)current + 0x30) == shape;
		if (same) *(dword *)out |= 2;
		else if (shape->type() == 7)
		{
			real margin = g_47f05c + 0.001f;
			real radius = *(real *)((byte *)shape + 0xc) - margin;
			if (g_47f05c > radius) radius = g_47f05c;
			c_movement_capsule capsule((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
			transform4x3f matrix;
			function_1d08e0(0, (s_havok_component *)component, &matrix);
			matrix.position.z += margin * 0.5f;
			if (!function_1c5210(&matrix, &capsule, *(long *)(input + 0x18), *(long *)(input + 0x24))) *(dword *)out |= 2;
			else *(dword *)out &= ~2;
		}
		else if (shape->type() == 4 || (shape->type() == 0x15 && (*(c_movement_shape **)((byte *)shape + 0xc))->type() == 4)) *(dword *)out |= 2;
		else *(dword *)out &= ~2;
	}
	if (**(dword const **)(input + 8) & 0x20)
	{
		long object_index = *(long *)(state + 4);
		function_b7740(object_index, &velocity, NULL, false);
		function_1c4b00(object_index, &velocity, NULL, 1);
		if (&velocity && velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
		{
			function_b9b90(object_index, false);
			function_b7360(object_index);
			function_bba20(object_index);
		}
		point3f position;
		position.x = g_510c54->rate * velocity.i + *(real *)(input + 0xe8);
		position.y = g_510c54->rate * velocity.j + *(real *)(input + 0xec);
		position.z = g_510c54->rate * velocity.k + *(real *)(input + 0xf0);
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		bool active = (*(dword *)(object + 4) & 0x100) != 0;
		if (active) function_b87b0(object_index);
		if (*(long *)(object + 0x14) != NONE ||
			!(position.x < -32768.0f || 32768.0f < position.x || position.y < -32768.0f ||
			32768.0f < position.y || position.z < -32768.0f || 32768.0f < position.z))
		{
			*(point3f *)(object + 0x64) = position;
			object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			if (*(long *)(object + 0xd4) != NONE) function_b58c0(*(long *)(object + 0xd4), 2);
		}
		function_1c4a80(object_index, false, false);
		if (active) function_b8600(object_index, 0);
		function_bba20(object_index);
	}
}
#endif
void function_1f19c0(byte const *request, s_shape_state *history, byte *out);
void function_1ed4a0(s_state_c570 *state, dword *output, byte const *input);
struct s_ease_state;
struct s_tracked_point;
void function_1f03e0(s_shape_state *state);
void function_1ec570(s_state_c570 *state);
void function_1eb640(s_ease_state *state);
void function_1fc2f0(s_tracked_point *tracked, point3f const *point, bool unknown);
void function_1fc350(long object_index, s_tracked_point *tracked);
point3f *function_b9dd0(long object_index, point3f *result);
void __stdcall function_1c3770(long object_index, dword flags);

/* Cleans up the previous state mode, initializes the new mode, and handles mode-transition updates. */
// @retail 0x1e54d0
void function_1e54d0(void *state, long mode)
{
	byte *physics_state = (byte *)state;
	byte old_mode_byte = physics_state[0];
	long old_mode = old_mode_byte;

	if (old_mode_byte != 0)
	{
		switch (old_mode)
		{
		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
		case 5:
			function_1fc350(*(long *)(physics_state + 4), (s_tracked_point *)(physics_state + 0x10));
			break;

		case 6:
			break;

		default:
			__assume(0);
		}
	}

	byte local_27f376 = (byte)mode;
	physics_state[0] = local_27f376;

	switch (local_27f376)
	{
	case 1:
		function_1f03e0((s_shape_state *)(physics_state + 0x10));
		break;

	case 2:
		*(long *)(physics_state + 0x10) = 0;
		*(vector3f *)(physics_state + 0x14) = *g_4687a4;
		break;

	case 3:
		function_1ec570((s_state_c570 *)(physics_state + 0x10));
		break;

	case 6:
		function_1eb640((s_ease_state *)(physics_state + 0x10));
		break;

	case 4:
	case 5:
	{
		point3f point;
		function_b9dd0(*(long *)(physics_state + 4), &point);
		function_1fc2f0((s_tracked_point *)(physics_state + 0x10), &point, local_27f376 == 5);
		break;
	}

	default:
		__assume(0);
	}

	byte local_792ecd_2 = physics_state[0];

	if (old_mode != local_792ecd_2 &&
		(old_mode == 3 || local_792ecd_2 == 3 ||
		 old_mode == 4 || old_mode == 5 ||
		 local_792ecd_2 == 4 || local_792ecd_2 == 5))
	{
		function_1c3770(*(long *)(physics_state + 4), 0);
	}
}

// @retail 0x1e5a60
void function_1e5a60(void *physics, s_biped_physics_result *result, s_biped_physics_input const *input)
{
	byte *state = (byte *)physics;
	result->contact_index = NONE;
	result->other_index = NONE;
	result->object_index = NONE;
	result->flags = 1;
	switch (*state)
	{
	case 1: function_1f19c0((byte const *)input, (s_shape_state *)(state + 0x10), (byte *)result); break;
	case 2: break;
	case 3: function_1ed4a0((s_state_c570 *)(state + 0x10), (dword *)result, (byte const *)input); break;
	case 4:
	case 5: *(vector3f *)(state + 0x10) = *(vector3f const *)((byte const *)input + 0x4c); break;
	case 6: break;
	default: __assume(0);
	}
	long old_index = *(long *)(state + 8);
	long object_index = result->object_index;
	if (object_index == old_index)
		*(long *)(state + 0xc) = object_index;
	else if (old_index == NONE)
		*(long *)(state + 0xc) = old_index;
}

struct s_physics_movement_definition
{
	dword flags;
};

struct s_physics_movement_input
{
	byte field_0[0x2c];
	long index;
	byte field_30[4];
	transform4x3f matrix;
	byte field_68[0xc];
	vector3f vector;
};

struct s_physics_movement_output
{
	long field_0;
	bool enabled;
	byte field_5[3];
	s_physics_movement_definition *definition;
	byte field_c[8];
	dword flags;
	byte field_18[0x54 - 0x18];
	long index;
	transform4x3f matrix;
	vector3f vector;
	real rate;
	vector3f direction;
	bool field_a8;
	byte field_a9[0xdc - 0xa9];
	vector3f control;
	byte field_e8[0xc];
	vector3f default_direction;
	byte field_100[0xc];
	vector3f active_direction;
	byte field_118[0x138 - 0x118];
	vector3f velocity;
	real speed;
	long field_148;
};

struct s_physics_movement_settings
{
	byte field_0[0x24];
	real limited_speed;
	byte field_28[4];
	real forward;
	real backward;
	real sideways;
	real speed;
	real crouched_forward;
	real crouched_backward;
	real crouched_sideways;
	real crouched_speed;
	long field_4c;
};

struct s_physics_movement_globals
{
	byte field_0[0x134];
	s_physics_movement_settings *settings;
};

// @retail 0x1e6120
void function_1e6120(s_biped_physics_output *output, void *physics, real rate,
	bool airborne, bool limited, bool mode, real crouch)
{
	s_physics_movement_output *state = (s_physics_movement_output *)output;
	bool is_limited = limited;
	s_physics_movement_input *input = (s_physics_movement_input *)physics;
	state->index = input->index;
	state->matrix = input->matrix;
	state->vector = input->vector;
	state->rate = rate;
	state->field_a8 = mode;
	state->direction = state->default_direction;
	state->enabled = true;
	real blend = (state->flags & 4) ? 0.0f : crouch;
	if ((bool)((state->definition->flags >> 2) & 1) && !airborne)
	{
		s_physics_movement_settings *settings = ((s_physics_movement_globals *)g_4e034c)->settings;
		vector3f control = state->control;
		real squared = control.k * control.k;
		squared += control.j * control.j;
		squared += control.i * control.i;
		if (squared > 1.0f)
		{
			real scale = 1.0f / (real)sqrt(squared);
			control.i = (real)(control.i * scale);
			control.j = (real)(control.j * scale);
		}
		real remaining = 1.0f - blend;
		if (control.i > 0.0f)
			state->velocity.i = settings->forward * remaining + settings->crouched_forward * blend;
		else
			state->velocity.i = settings->backward * remaining + settings->crouched_backward * blend;
		state->velocity.j = settings->sideways * remaining + settings->crouched_sideways * blend;
		state->velocity.k = 0.0f;
		state->speed = settings->speed * remaining + settings->crouched_speed * blend;
		state->field_148 = settings->field_4c;
		if (is_limited)
		{
			state->velocity.i = control.i > 0.0f ? settings->limited_speed : 0.0f;
			state->velocity.j = 0.0f;
		}
		state->velocity.i *= control.i;
		state->velocity.j *= control.j;
		state->direction = state->active_direction;
	}
	else if (!(state->flags & 8) && !(state->flags & 4))
		state->speed = 3.4028234663852886e+38f;
}

void function_1f1930(s_shape_side *side, const s_shape_contact *contact);
void function_1faeb0(const s_contact *contact, s_contact_result *result);
void function_1fc620(s_tracking_result *result, const s_tracking_source *source, const s_tracking_target *target);
void function_1ec3f0(s_unknown_object *object, s_unknown_output *output);

struct s_contact_dispatch_output
{
	long field_0;
	long direction;
	bool forced;
};

// @retail 0x1e5b50
void function_1e5b50(const byte *contact, s_contact_dispatch_output *output, const byte *state)
{
	if (contact[0x14] & 0x20)
	{
		output->direction = 1;
		output->forced = true;
	}
	else
	{
		switch (*state)
		{
		case 1: function_1f1930((s_shape_side *)output, (const s_shape_contact *)contact); break;
		case 2: function_1faeb0((const s_contact *)contact, (s_contact_result *)output); break;
		case 3: break;
		case 6: function_1ec3f0((s_unknown_object *)contact, (s_unknown_output *)output); break;
		case 4: function_1fc620((s_tracking_result *)output, (const s_tracking_source *)(state + 0x10), (const s_tracking_target *)contact); break;
		case 5: function_1fc620((s_tracking_result *)output, (const s_tracking_source *)(state + 0x10), (const s_tracking_target *)contact); break;
		default: __assume(0);
		}
	}
}


struct s_character_physics_component
{
	byte unknown00[0x10];
	point3f position;
	byte unknown1c;
	byte has_position;
};

struct s_type_94656b
{
	long unknown00;
	byte unknown04;
	byte unknown05[0x27];
	point3f point2c;
	point3f point38;
	point3f point44;
	long unknown50;
	byte unknown54[0x58];
	point3f pointac;
	byte byteb8;
	byte byteb9;
	byte unknownba[2];
	byte bytebc;
	byte unknownbd[3];
	point3f pointc0;
	point3f pointcc;
	real valued8;
};

struct s_character_physics_update_input_datum_a
{
	long unknown00;
	long unknown04;
	long unknown08;
	long unknown0c;
	long unknown10;
	byte unknown14;
	byte unknown15[3];
	unsigned long flags;
	point3f point1c;
	point3f point28;
	point3f point34;
	long unknown40;
};

struct s_source_a
{
	byte unknown00;
	byte unknown01[7];
	long unknown08;
	long unknown0c;
};

struct s_time_entry
{
	long time;
	short a;
	short b;
};

// @retail 0x1e67a0
void function_1e67a0(s_character_physics_component *component, s_type_94656b *datum, byte a, byte b)
{
	byte has_position = component->has_position;
	/* the parameter itself moves on to the selected position */
	component = (s_character_physics_component *)&component->position;
	if (!has_position)
		component = (s_character_physics_component *)g_4687b0;
	datum->pointac = *(point3f *)component;
	datum->byteb8 = a;
	datum->byteb9 = b;
	datum->unknown04 = 1;
}

// @retail 0x1e67f0
void function_1e67f0(s_type_94656b *datum, s_character_physics_component *component, long animation_id, point3f *p1, point3f *p2, point3f *p3)
{
	datum->unknown50 = animation_id;
	datum->point2c = *p1;
	datum->point38 = *p2;
	datum->point44 = *p3;
	datum->unknown04 = 1;
}

// @retail 0x1e6850
void function_1e6850(s_type_94656b *datum, byte a, point3f *p1, point3f *p2, real v)
{
	datum->bytebc = a;
	datum->pointc0 = *p1;
	datum->pointcc = *p2;
	datum->valued8 = v;
	datum->unknown04 = 1;
}

// @retail 0x1e68a0
void function_1e68a0(s_character_physics_update_input_datum_a *datum, s_source_a *source, long a1, long a2, long a3, bool b0, bool b1, bool b2, bool b3, bool b4, point3f *p1, point3f *p2, point3f *p3)
{
	datum->unknown04 = source->unknown08;
	datum->unknown08 = source->unknown0c;
	datum->unknown0c = a3;
	long source_mode = source->unknown00;
	*(long volatile *)&datum->unknown00 = a1;
	datum->unknown40 = a2;
	datum->unknown10 = source_mode;
	*(dword volatile *)&datum->flags = 0;
	datum->unknown14 = 0;
	if (b0) datum->flags = 1; else datum->flags = 0;
	if (b1) datum->flags |= 2; else datum->flags &= ~2;
	if (b2) datum->flags |= 4; else datum->flags &= ~4;
	if (b3) datum->flags |= 8; else datum->flags &= ~8;
	if (b4) datum->flags |= 16; else datum->flags &= ~16;
	datum->point1c = *p1;
	datum->point28 = *p2;
	datum->point34 = *p3;
}

// @retail 0x1e6980
void function_1e6980(s_time_entry *entries, short a, byte b)
{
	long index = 0;
	long oldest = entries[0].time;
	if (entries[1].time < oldest)
	{
		index = 1;
		oldest = entries[1].time;
	}
	if (entries[2].time < oldest)
	{
		index = 2;
		oldest = entries[2].time;
	}
	if (entries[3].time < oldest)
	{
		index = 3;
	}
	entries[index].time = g_510c54->game_time;
	entries[index].a = a;
	entries[index].b = b;
}

#include "unknown_1cafc0.h"
#include <math.h>
#include <xmmintrin.h>

real magnitude3d(vector3f const *v);

bool g_46fcc4 = true;
bool g_51e9bc;

/* the approximate reciprocal square root, read from and written to memory */
PRIVATE __forceinline real inverse_sqrt_1e6360(real squared)
{
 real inverse;
 __asm
 {
  rsqrtss xmm0, squared
  movss inverse, xmm0
 }
 return inverse;
}

// score 132 (was 132): 1.0 sits in xmm4 (retail xmm3) through the body; retail keeps the jitter bits in eax
// (tests al/ah) and tests the on/off bits 1, 2, 4 before the sign bits 8, 16, 32, spilling the z on/off factor and
// y to the limits' spill slots. Plain bits 170; on/off first with `*=` 233, with the factor on the left 233 (both
// 1020 bytes; aligned difference 129 against this body's 154); on/off first in a jitter vector 163 (1048 bytes,
// aligned 110, closest), its products added straight into the velocity 239.
// Round 16 (later): the jitter block's shape, the constant's XMM ownership and the limit spills
// are the whole difference; the plain score is 132 and the reduced image scores 310 because the
// reduced build does not inline the magnitude helper retail inlines.
// Round 16: the near-distance value named in a local, the comparison against it and the second
// magnitude in the branch (plain 132 -> 130; dropped, a gain under 8); the sign factor on the left of
// the on/off factor 131, the scale factor on the left of the jitter value 130, both 131, and the scaled
// products added straight into the velocity 163 (1051 bytes against retail's 1077); the jitter block's
// own body restored.
// Round 36: single-workspace jitter 163; shared jitter add 157; shared movement add 156; all shared updates 156;
// jitter rename 156; shared offset scale 139; scale with direct updates 155; scale with shared jitter add 149.
// Native word/short/dword bit samples 168/168/170; on/off-first samples 231/231/233;
// word/dword native jitter workspaces 161/163 (exact-only). Original sample and body restored.
// Round 308: Complete control/speed-scale/projection-input value subsets 288/256/294/195/287/259/296;
// all with immutable native limits 296. All regress; original observations, projection stores,
// frame and inherited jitter qualifier restored. Constant/limit ownership and jitter spills remain.
// Round 430: native definition/output fields and real view of the existing dual-use output field,
// all seven subsets 132; ordinary bit sample on that complete field graph 170. Original declarations
// and body restored; constant/limit XMM ownership, jitter order and spill homes remain.
// Lane 15 r19 (fast checks need 0x24590 listed): jitter in retail's order (on/off, signs, scales,
// `*=`) with a plain long 233 (1020 bytes; the jitter block then has retail's shape, bits in EAX and
// retail's test order), with the volatile long 202 (1053 bytes); the blend factor as one
// if/else-if/else chain 215 (1081 bytes); factor declared before input_fraction 165;
// `1 + (def - 1) * blend` and `blend * (def - 1) + 1` 132/132 (identical code).
// Round 91: velocity updates through the existing vector3f_add/vector3f_add_scaled (in/out on
// &state->velocity; retail reloads velocity.j/k after the velocity.i store, as those give): all three
// 182, movement add 181, projection and movement 177, jitter add 130 (1077 bytes, retail's length;
// not kept, -2); on the jitter add, plain bits with the on/off bits first 231 (1021 bytes; structural
// 34 against 47), the same with volatile bits 201, on/off factor on the left 231; plus the movement
// add 174 (1064 bytes, 251 instructions as retail, structural 28, aligned 96 against 105: closest
// shape; limits.j/k then share one register where retail keeps two copies, and on_z/y spill
// differently); plus a chained limits.j/k 258, both 208.
// Round 116 (lane 6, fast, 0x24590 listed): projection through the existing vector3f_add_scaled 181 (1119
// bytes); that with the jitter add 176; movement and jitter adds 182; the jitter add alone 130 again (1077
// bytes, gain 2, not kept); on it, named on/off factors first with the volatile bits 201 (1054 bytes), with
// plain bits 231 (1021 bytes: the jitter block takes retail's shape, bits in EAX, tests 1, 2, 4 first, but the
// movement block above shifts). Retail reuses the limits.j/k spill slots for y and the z on/off factor.
// Round 53 (lane 9, fast): jitter as on/off factors first with a plain long 233 (1020 bytes: limits.j/k
// then share one register, so on_z and y are not spilled); `limits.k = limits.j` 221 (1045 bytes); the
// projection dot with direction first in each product 132 (same code); the movement scaled by the field
// at each use 172; length_squared summed k, j, i 144. Compile-only screens (magnitude3d inline, 512
// spellings: scale local, length/projection/offset/control/limit/acceleration operand orders, volatile,
// jitter factor and offset forms) never give retail's 1.0 in XMM3 or its k, j, i projection order.
// Round 12 (lane 18, fast, 0x24590 listed): 1-7 unused initialised locals at the top 132 each (the XMM3/XMM4
// constant choice is no numbering tie); the two `(void)&parameter;` statements were dead (removed: identical
// code); without them, the jitter add through the existing vector3f_add 130 (1077 bytes, as round 91: not kept);
// the movement scaled through the existing vector3f_scale 132; the direction normalised through it 132.
// Round 47 (m3 lane 1, fast, 0x24590 listed; aligned counts against this body's 154): retail order
// jitter (on/off, signs, scale-on-left selects) with a plain long and its products added straight into the
// velocity plus the movement add through vector3f_add 202 (1055 bytes, aligned 111); round 91's 174 form
// plus the projection through vector3f_add_scaled 191 (aligned 117); the jitter add through vector3f_add
// with a plain long in the old order 163 (1045 bytes), plus the movement add 147 (1087 bytes), plus the
// projection through vector3f_add_scaled too 147 (1086 bytes). Reconfirmed:
// jitter add 130, with retail-order plain jitter 231, plus movement add 174 (aligned 129), retail order alone 233.
// Round 62 (fast, 0x24590 listed; jitter on/off first with `*=` and a plain long in each): projection, movement
// and jitter adds through the existing helpers 191 (1066 bytes); projection and movement 189 (1063, aligned
// 110); projection and jitter 171 (1064); projection alone 188 (1068); the same on/off order keeping the
// volatile long with projection and jitter 199 (1097), with all three 205 (1099). In each, limits.k is
// memory-resident from its definition (retail keeps it in XMM7 and spills on_z and y instead).
// Round 126 (lane 5, fast, 0x24590 listed): written operand and sum orders do not reach the code: the
// direction scaled as `inverse * i`, `j * inverse`, `k * inverse` (retail's register pattern) 132, the
// projection summed i, j, k 132, `0.0f <` on the definition and blend tests 132, `1.0f == blend` 132,
// `1.0f > magnitude` in the clamp 132 (all identical); speed/acceleration stores before the limits 212
// (1065 bytes); limits in k, j, i order 136, in j, k, i order 134; retail-order jitter with a plain long
// re-measured 233 (limits.k then shares limits.j's register and is stored at its definition).
// @retail 0x1e6360
void function_1e6360(s_biped_physics_output *output, real rate, long object_index, real blend)
{
 s_physics_movement_output *state = (s_physics_movement_output *)output;
 *(real *)((byte *)state + 0x28) = rate;
 state->enabled = true;
 byte *definition = (byte *)state->definition;
 real input_fraction = 1.0f - (magnitude3d(&state->control) < 1.0f ? magnitude3d(&state->control) : 1.0f);
 real factor = 1.0f;
 if (*(real *)(definition + 0x90) > 0.0f)
 {
  if (blend == 1.0f) factor = *(real *)(definition + 0x90);
  else if (blend > 0.0f) factor = (*(real *)(definition + 0x90) - 1.0f) * blend + 1.0f;
 }
 factor *= *(real *)((byte *)state + 0x134);
 vector3f limits;
 limits.i = (*(real *)(definition + 0x78) * *(real *)((byte *)state + 0x20)) * factor;
 limits.j = (*(real *)(definition + 0x7c) * *(real *)((byte *)state + 0x20)) * factor;
 limits.k = (*(real *)(definition + 0x7c) * *(real *)((byte *)state + 0x20)) * factor;
 real acceleration = ((1.0f - input_fraction) * *(real *)(definition + 0x80) +
  *(real *)(definition + 0x84) * input_fraction) * *(real *)((byte *)state + 0x20) * factor;
 state->speed = acceleration;
 *(real *)&state->field_148 = acceleration;
 state->velocity.i = state->control.i * limits.i;
 state->velocity.j = state->control.j * limits.j;
 state->velocity.k = state->control.k * limits.k;
 if (g_46fcc4)
 {
  vector3f movement;
  real angular;
  ((s_animation_state *)*(long *)((byte *)state + 0x1c))->movement_rate_get(object_index, &movement, &angular);
  real scale = *(real *)((byte *)state + 0x20);
  movement.i *= scale;
  movement.j *= scale;
  movement.k *= scale;
  vector3f direction = movement;
  real length_squared = direction.i * direction.i + direction.j * direction.j + direction.k * direction.k;
  if (length_squared != 0.0f)
  {
   real inverse = inverse_sqrt_1e6360(length_squared);
   direction.i *= inverse;
   direction.j *= inverse;
   direction.k *= inverse;
  }
  real projection = 0.0f - (state->velocity.k * direction.k + state->velocity.j * direction.j + state->velocity.i * direction.i);
  state->velocity.i += projection * direction.i;
  state->velocity.j += projection * direction.j;
  state->velocity.k += projection * direction.k;
  state->velocity.i += movement.i;
  state->velocity.j += movement.j;
  state->velocity.k += movement.k;
 }
 if (g_51e9bc)
 {
  volatile long bits = g_510c54->game_time * *(long *)((byte *)state + 0x18);
  real x = ((bits & 8) ? 1.0f : -1.0f) * ((bits & 1) ? 1.0f : 0.0f);
  real y = ((bits & 16) ? 1.0f : -1.0f) * ((bits & 2) ? 1.0f : 0.0f);
  real z = ((bits & 32) ? 1.0f : -1.0f) * ((bits & 4) ? 1.0f : 0.0f);
  x *= (bits & 64) ? 1.0f : 0.5f;
  y *= (bits & 128) ? 1.0f : 0.5f;
  z *= (bits & 256) ? 1.0f : 0.5f;
  vector3f offset;
  offset.i = x * 0.2f * limits.i;
  offset.j = y * 0.2f * limits.j;
  offset.k = z * 0.2f * limits.k;
  state->velocity.i += offset.i;
  state->velocity.j += offset.j;
  state->velocity.k += offset.k;
 }
}

struct s_physics_initial_state
{
 long mode;
 bool enabled;
 byte field_5[3];
 const void *definition;
 long physics8;
 long physicsc;
 dword flags;
 long component_index;
 s_animation_state *animation;
 real scale;
 long value;
 real height;
 byte field_2c[0xdc - 0x2c];
 vector3f control;
 point3f position;
 vector3f forward;
 vector3f up;
 vector3f facing_goal;
 vector3f facing;
 vector3f ground_velocity;
 real gravity;
 real boost;
 vector3f velocity;
 real speed;
 real field_148;
 long material;
};

PRIVATE inline void initial_cross(const vector3f *a, const vector3f *b, vector3f *result)
{
 result->i = a->j * b->k - a->k * b->j;
 result->j = a->k * b->i - a->i * b->k;
 result->k = a->i * b->j - a->j * b->i;
}

PRIVATE inline real initial_dot(const vector3f *a, const vector3f *b)
{
 return a->k * b->k + a->j * b->j + a->i * b->i;
}

// Round 16: retail takes the animation state in EAX, the component index in EDX, the physics
// pointer in ECX and the output pointer in EDI, then the object index, the definition, the value
// and the flags on the stack; this file's declaration describes another order and the callers are
// in other files. Retail keeps the axis pointer in EBP, the sampled vector at [esp+0xc] and the
// sine and the cosine in the dead argument slots.
// MiniMax R (10-31): shared sample/forward vector 160 with retail's `!(0.0001f > fabs(angle))` guard, 198 with `>=`; in-place rotated 166, old-body retail guard 173; turn block on the axis 227/210, cross dots on facing_goal 194; no lower score.
// score 155 (was 155): Frame 0x18 agrees; sampled-vector slot and rotation/output scheduling remain.
// movement/rotation homes and cross/result lifetimes still differ.
// Frame 0x18 agrees; sampled movement/rotation homes and SSE/output scheduling still differ.
// Frame 0x18 agrees; sampled-vector homes and x87/SSE rotation scheduling remain.
// Earlier: movement/rotation homes and cross/result lifetimes still differ.
// early side products 249f; single turn cross 249f.
// rotation cross as old 155f; rotation cross as new 153f.
// one rotation cross for both 245f; saved forward reads 220f.
// saved forward plus shared cross 232f; indexed equal rotation rows 181f.
// movement/original stack homes and rotation scheduling remain; forward scalar
// inputs early/late 201/216; shared cross/forward/both scales 206/190/190; projected axis 214;
// projected axis plus cross scale 212; common forward vector result 234.
// movement slot and rotation/output allocation still differ.
// Existing velocity scale 185; scale from field 186; first dot products commuted 155;
// side projection values 166; rotated alignment value 166; axis field reference 155;
// output record reference 155; scale plus dot flow 185. Opening body retained.
// movement/rotation homes and scalar scheduling remain;
// i/j/k/all cosine terms 168/177/192/208;
// i/j/k/all axis projection terms 186/168/168/218. Opening body retained.
// Complementary cosine 166; guarded early alignment 205; early moved-away result 231;
// completed side product 169; complement with early result 231; indexed velocity scale 220;
// actual animation reference 155; actual forward-field reference 248. Movement/rotation homes remain.
// sampled-vector slot and rotation/exit schedule still differ.
// Cross before projection 201; late axis 185; split axis projection 166; goal snapshot 247;
// direct selection tests 204; turn-first guard 156; scalar final writes 192; library copies 155.
// sampled-vector slot and scalar/rotation/output scheduling remain.
// Native cross/rotation/both coordinate initializers; immutable original, cross,
// rotation, both computed vectors and all three vector values
// Fast scores 155/155/155/155/155/155/155/155; original body retained.
// Round 361: complete/row-local additive sums 174/174; complete/row-local compound rows 218/185;
// aggregate additive construction 174; complete/row-local additive and weighted parts 185/185;
// additive vector plus plain subtraction loop 204 (fast). Complete opening body restored.
// Cross before/between trigonometric values 202/200; direct axis fields 247; immutable rotation scalars 155; native scaled velocity 214; with early cross 238; early/mid cross plus direct axis 261/236.
// Projection through private dot / uniform private sums / uniform shared dot / shared turn
// dot 185/185/185/155; those four with all three shared crosses 185/185/185/157. All restored.
// All-row cosine/axis/sine multiplicand orders, seven nonempty combinations, 155 each; rate-first
// rotation product 155. Existing private dot/cross/both force-inline declarations 155/155/155,
// without replacing the rotation cross. Original products, helpers and declarations retained.
// double scale 237; double scale helper 223; movement inline 155; movement noinline 155; movement inline
// noinline 155; inline double helper 223; noinline double helper 223; inline noinline double helper 223.
// Round 330: Native rotation 155; paired double trig 281; rotation plus trig 281; double projection 276;
// rotation/projection 276; trig/projection 281; all native precision 281; native output velocity sampling
// 246. Exact-match-only forms restored.
// Native float scale to output/in-place/returned output 186/206/248;
// the same three with sample scope 186/206/248; uniform axis value with direct/in-place scale
// 266/288. Direct native scale reaches retail length but regresses; original sampled scaling retained.
// Round 332: Sample-angle, turn-angle and paired trig scalar home names, each
// and every combination, 155; all with projected-axis scalar home 155. All restored.
// Round 150: Existing cross producer first/second value inputs and returned output, seven combinations 305/307/379/273/234/228/261; both input references 155. All restored.
// Sampled-vector slot and rotation/output allocation remain.
// Native rotation producer with value/reference/pointer direction and pointer/reference axis,
// output forms 220/220/220/220; returned vector forms 258/256/256/258. All restored.
// Round 220: Five/all input flag packing 382/387; five/all flags from native ordered tests
// into a real result word 368/398; corresponding conditional result words 391/374;
// complete result at entry with either role name 398/398. Earlier initialization now differs;
// original field-by-field flag production, sampled vectors and all rotation expressions restored.
// Round 199: Canonical turn-result byte/short/long 155/161/159; side-test outputs and
// result declared before sampling, bool/byte/short/long 155/155/161/159; all vector
// workspaces declared before sampling with distinct side-output names 155.
// Round 201: Shared sample/rotation-cross workspace 201; with original-side/rotated-side/sequential
// side reuse 207/199/246; the same four with existing rotation-cross helper 201/207/199/246. Original
// workspaces restored.
// Round 227: Original fields at use, compound projection/cross, their pairs/all,
// direct fields plus compound projection and existing cross helper 202/171/185/200/204/153/195/262. Restored.
// Complete rotation rows jki/jik/kij/kji/ikj 204/202/204/206/167;
// complete rotation and cross rows jki/kij/kji 206/200/203. All restored;
// sampled-vector slot and rotation/output scheduling remain.
// Round 56: Sample/forward workspace 187 (sample home and length agree); with ordered guard 178;
// ordered guard alone 173; sampled_movement/direction names 155/155; single sample/forward/rotated
// workspace with scalar inputs 195; reversed angle comparison 155; sample/forward workspace with
// existing cross helper 187. Original body restored.
// Round 194: dot producer first/second/both value inputs 320/385/525;
// first/second/both input references 155/155/155; value/reference and
// reference/value inputs 320/385. Original dot interface and body restored.
// Round 111 (fast): 25 name pairs for the sampled vector and the forward copy 155 each; the sample in
// its own closed block 155; one shared vector for sample and forward copy 187 (retail slot and length),
// plus `!(fabs(angle) < 0.0001f)` 178, plus an in-place rotate of it 155 at 1382 B (retail re-reads
// each axis component and puts cosine in the angle's dead slot +0x34, sine at +0x44; ours caches the
// axis and swaps them); cosine declared first 155; five sine/cosine name pairs 155. Original restored.
// Round 206: Complete scalar first/second cross inputs and rotation-axis observations, seven subsets
// [194, 202, 192, 298, 339, 306, 336]; all with immutable original 336. Original restored.
// Round 308: Complete three-row indexed rotation, original/projection-first additive operands with long,
// unsigned-long, short and byte coordinates: 176/176/176/176/296/296/296/296. Original rows and
// body restored; sampled-vector homes and rotation/output scheduling remain.
// Round 430: staged rotation cross/paired side crosses/both 184/153/184; those with common
// completion 184/153/184; both with early side-output storage, with/without common completion 184/184.
// Two-point side-product gains rejected; original helpers, storage, ordered guard and body restored.
// Round 207: completed sample copied to velocity then scaled in place, original/rate-angle/
// per-use scale/both 208/265/211/262; the same four through the shared float scale
// 220/207/220/207. Original sample, field stores, angle storage and includes restored.
// Round 53 (lane 9, fast; the score is a lockstep count, so a one-instruction shift in a region costs
// the rest of it): `!(fabs(angle) < 0.0001f)` 173 (retail's exact `fld; fcompi; ja` test at 1382 bytes;
// the rotation is then one instruction short and the turn block two short of retail); with moved_away
// preset false and set in the test 176 (retail's `xor dl, dl` ... `mov dl, 1`; flag form, needs a match);
// with the rotation done in place on the forward copy 178; with sin/cos of `angle * rate` and no rotation
// local 156, the same on the original body 169. A /FAs frame probe (compile only) groups movement with
// new_cross at -12 and original/rotated at -24 (retail: movement with original/rotated, new_cross alone);
// declaration order and names never move it, one extra early read of original swaps movement and original.
// Extra arithmetic tuples anywhere rotate the turn-block dot sums toward retail's j,k,i / j,i,k orders.
// Round 26 (lane 14, fast): the sampled vector's slot follows reference counts: two extra reads of it
// (diagnostic only) put it at retail's [esp+0xc], sharing the forward copy's slot, 236 at 1415 B. Side
// cross names cross_a/cross_b and z_ names, and the two crosses declared apart, 155 each (slot unchanged);
// existing vector3f_scale into velocity 186 (1382 B, slot unchanged); vector3f_scale in place then a
// copy 206 (slot fixed, the scale goes through memory); the same by hand 203; in place plus
// `!(fabs(angle) < 0.0001f)` 200; copy then scale in place 220; `moved_away = false` set inside the
// test 159 (retail's `xor dl, dl` ... `mov dl, 1`, a flag set in a test: only with a match).
// @retail 0x1e5bb0
void function_1e5bb0(s_biped_physics_output *output, void *physics, void *animation_state, real speed_scale,
 long component_index, long object_index, void const *definition, long value, bool b, bool turning,
 bool c, bool landing, bool d, bool grounded, bool e, bool f, real gravity, real boost,
 vector3f const *control, point3f const *position, vector3f const *forward, vector3f const *up,
 vector3f const *facing_goal, vector3f const *facing, vector3f const *ground_velocity, long material)
{
 s_physics_initial_state *state = (s_physics_initial_state *)output;
 s_animation_state *animation = (s_animation_state *)animation_state;
 byte *component = g_51e9b8->data + (component_index & 0xffff) * 0xa0;
 state->mode = *(byte *)physics;
 state->definition = definition;
 state->component_index = component_index;
 state->physics8 = *(long *)((byte *)physics + 8);
 state->physicsc = *(long *)((byte *)physics + 0xc);
 state->value = value;
 state->flags = 0;
 state->animation = animation;
 state->scale = speed_scale;
 if (b) state->flags |= 1; else state->flags &= ~1;
 if (d) state->flags |= 2; else state->flags &= ~2;
 if (grounded) state->flags |= 4; else state->flags &= ~4;
 if (e) state->flags |= 8; else state->flags &= ~8;
 if (f) state->flags |= 16; else state->flags &= ~16;
 if (!((bool)((*(dword *)(component + 4) >> 1) & 1))) state->flags |= 64;
 else state->flags &= ~64;
 state->gravity = gravity;
 state->boost = boost;
 state->control = *control;
 state->position = *position;
 state->forward = *forward;
 state->up = *up;
 state->facing = *facing;
 state->facing_goal = *facing_goal;
 state->ground_velocity = *ground_velocity;
 state->material = material;
 state->velocity = *g_4687a4;
 state->speed = 4.8f;
 state->field_148 = 0.0f;
 state->enabled = false;
 if (animation->graph_tag_index != NONE && *(long *)animation != NONE &&
  *(short *)((byte *)animation + 6) != NONE && !landing)
 {
  vector3f movement;
  real angle;
  animation->movement_rate_get(object_index, &movement, &angle);
  real scale = state->scale;
  state->velocity.i = movement.i * scale;
  state->velocity.j = movement.j * scale;
  state->velocity.k = movement.k * scale;
  if (fabs(angle) >= 0.0001f)
  {
   vector3f original = state->forward;
   vector3f const *axis = &state->up;
   real rotation = angle * g_510c54->rate;
   real sine = (real)sin(rotation);
   real cosine = (real)cos(rotation);
   real projection = (original.j * axis->j + original.k * axis->k + original.i * axis->i) * (1.0f - cosine);
   vector3f cross;
   cross.i = original.j * axis->k - original.k * axis->j;
   cross.j = original.k * axis->i - original.i * axis->k;
   cross.k = original.i * axis->j - original.j * axis->i;
   vector3f rotated;
   rotated.i = original.i * cosine + projection * axis->i - cross.i * sine;
   rotated.j = projection * axis->j + original.j * cosine - cross.j * sine;
   rotated.k = projection * axis->k + original.k * cosine - cross.k * sine;
   if (c && turning)
   {
    real alignment = initial_dot(&state->forward, facing_goal);
    if (alignment > 0.5f)
    {
     vector3f old_cross, new_cross;
     initial_cross(facing_goal, &state->forward, &old_cross);
     initial_cross(facing_goal, &rotated, &new_cross);
     bool moved_away = alignment > 0.99f && alignment > initial_dot(&rotated, facing_goal);
     if (initial_dot(&state->up, &old_cross) * initial_dot(&state->up, &new_cross) <= 0.0f || moved_away)
     {
      state->forward = *facing_goal;
      state->flags |= 32;
      return;
     }
    }
   }
   state->forward = rotated;
  }
 }
}

struct s_shape_state;
struct s_direction_rotation_input;
void function_1f1460(byte const *state, s_shape_state const *ground, vector3f *up, vector3f *forward);
void function_1faa20(vector3f *up, byte const *state, vector3f *forward, long *ticks);
void function_1ecf50(vector3f *forward, vector3f *up, s_direction_rotation_input *input);

// @retail 0x1e5af0
void function_1e5af0(void *physics, s_biped_physics_output *output, vector3f const *up, vector3f const *forward)
{
 switch (*(byte *)physics)
 {
 case 1: function_1f1460((byte *)output, (s_shape_state *)((byte *)physics + 0x10), (vector3f *)up, (vector3f *)forward); break;
 case 2: function_1faa20((vector3f *)up, (byte *)output, (vector3f *)forward, (long *)((byte *)physics + 0x10)); break;
 case 3: function_1ecf50((vector3f *)forward, (vector3f *)up, (s_direction_rotation_input *)output); break;
 case 4: break;
 case 5: break;
 case 6: break;
 default: __assume(0);
 }
}
