// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E67A0.CPP: character physics update input datum setters */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "reciprocal_normalize.h"

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

#include "unknown_1dacb0.h"

struct s_type_94656b
{
	long unknown00;
	byte unknown04;
	byte unknown05[0x27];
	point3f point2c;
	point3f point38;
	point3f point44;
	c_type_709360 unknown50;
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
void function_1e67f0(s_type_94656b *datum, s_character_physics_component *component, c_type_709360 animation_id, point3f *p1, point3f *p2, point3f *p3)
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
#include "normalize3d.h"
#include <math.h>
#include <xmmintrin.h>

real magnitude3d(vector3f const *v);

bool g_46fcc4 = true;
bool g_51e9bc;

// The operand orders here follow the count of named locals and inlined calls: the speed is stored and read back,
// the projection goes straight into the scaled add, the squared length is one expression and the jitter's 0.2
// factor is applied per component (not through vector3f_scale); each of these is needed for the byte match.
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
 state->speed = ((1.0f - input_fraction) * *(real *)(definition + 0x80) +
  *(real *)(definition + 0x84) * input_fraction) * *(real *)((byte *)state + 0x20) * factor;
 *(real *)&state->field_148 = state->speed;
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
  real squared = length_sq3f(&direction);
  if (squared != 0.0f)
  {
   real inverse = inverse_sqrt_2b96(squared);
   scale_particle_vector_2b(inverse, &direction);
  }
  vector3f_add_scaled(&state->velocity, &direction, 0.0f - dot3f(&state->velocity, &direction), &state->velocity);
  vector3f_add(&state->velocity, &movement, &state->velocity);
 }
 if (g_51e9bc)
 {
  long bits = g_510c54->game_time * *(long *)((byte *)state + 0x18);
  vector3f offset;
  offset.i = (bits & 1) ? 1.0f : 0.0f;
  offset.j = (bits & 2) ? 1.0f : 0.0f;
  offset.k = (bits & 4) ? 1.0f : 0.0f;
  real sign_x = (bits & 8) ? 1.0f : -1.0f;
  offset.i = sign_x * offset.i;
  real sign_y = (bits & 16) ? 1.0f : -1.0f;
  offset.j = sign_y * offset.j;
  real sign_z = (bits & 32) ? 1.0f : -1.0f;
  offset.k = sign_z * offset.k;
  real scale_x = (bits & 64) ? 1.0f : 0.5f;
  offset.i = scale_x * offset.i;
  real scale_y = (bits & 128) ? 1.0f : 0.5f;
  offset.j = scale_y * offset.j;
  real scale_z = (bits & 256) ? 1.0f : 0.5f;
  offset.k = scale_z * offset.k;
  offset.i *= 0.2f;
  offset.j *= 0.2f;
  offset.k *= 0.2f;
  offset.i *= limits.i;
  offset.j *= limits.j;
  offset.k *= limits.k;
  vector3f_add(&state->velocity, &offset, &state->velocity);
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

void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* src/unknown_1eb8a0.cpp */

PRIVATE inline void initial_cross(const vector3f *a, const vector3f *b, vector3f *result)
{
 result->i = a->j * b->k - a->k * b->j;
 result->j = a->k * b->i - a->i * b->k;
 result->k = a->i * b->j - a->j * b->i;
}

// score 114 (was 114; 114 at r76): Frame 0x18 and sample home agree; rotation products, side-dot and exit scheduling remain.
// MiniMax A (11-12): no new form, no lower score (114, the reduced image and the full build agree): naming the
// sine and the cosine before the rotation call scores 150 (1382 bytes) against 114, so the call keeps its
// expressions; the first difference is still the inlined rotation's dot and cross products, whose body is in
// another file: retail multiplies the vector's component by the axis's from memory and sums the terms j, k, i,
// ours multiplies the axis's component by the vector's and sums them i, j, k.
// MiniMax S (11-07): no new form, no lower score; the crosses declared apart, declared at first use or
// declared at the top of the body give 114, the two cross calls swapped give 124 and the goal copy by
// components gives 120, so the body stays; the rotation products, the side-dot order and the exit copies differ.
// MiniMax R (11-05): no new form, no lower score; writing the two turn-block crosses out with retail's
// differing third terms gives 140 at retail length (the rotation block is re-generated), so the body stays.
// MiniMax S (11-10): no new form, no lower score (114); the reduced image lays this function out differently from retail as well (fast 409,
// 1167 bytes against 1382), and the rotation products, the side-dot and the exit scheduling are unchanged; the full check agrees with the
// queue (114, ours 1389 bytes against retail's 1382).
// Earlier 114 (was 114), merged from a second score line: Frame 0x18 and sample home agree; rotation products, side-dot and exit scheduling remain.
// Round 16: retail takes the animation state in EAX, the component index in EDX, the physics
// pointer in ECX and the output pointer in EDI, then the object index, the definition, the value
// and the flags on the stack; this file's declaration describes another order and the callers are
// in other files. Retail keeps the axis pointer in EBP, the sampled vector at [esp+0xc] and the
// sine and the cosine in the dead argument slots.
// Earlier 114 (was 155): 1389 bytes against 1382, aligned 61 against the old body's 97. The existing rotate_direction on a
// Then (was 155): 1389 bytes against 1382, aligned 61 against the old body's 97. The existing rotate_direction on a
// separate copy of forward, with sin and cos as direct arguments and `!(fabs(angle) < 0.0001f)`, gives retail's sample
// slot, guard, schedule and rotation registers except four products: retail copies v.j/v.k and multiplies by axis.j/axis.k
// from memory, ours loads the axis (the same for goal.j against rotated.j/k in the turn block, and from them the
// last dot's order). Tried on this body (fast with a local copy of the helper, the plain build inlines the existing one
// the same): `moved_away` preset false and set in the test 92 at 1387 bytes (retail's `xor dl, dl`/`mov dl, 1`; a flag set in a
// test, match only); the side dots as (up, new) * (up, old) 106 (aligned 51; NOT kept: an operand order picked by score, retail
// forms the old-cross dot first at 0x1e607d; body restored at 114), (new, up) * (up, old) 107 (aligned 44), (old, up) * (new, up) 111,
// named side values 150 (1382 bytes, the rotation products all load first), moved_away computed before the crosses 190, the
// crosses declared apart or swapped 106 (same). Compile-only: parameter orders of the helper, cross or dot orders and
// operand sides, component locals, named sine/cosine (worse), a named rotation or rate, `angle *= rate`, a named axis,
// helper-returned pointers, indexed or cast access, by-value vector: about 6000 forms, none gives the four products with
// retail's registers. Past a size limit of the tail block (15 extra one-field reads, diagnostic) the products come out
// as retail's, but v.j/v.k take XMM1/XMM2 where retail has XMM5/XMM6.
// Earlier: Frame 0x18 agrees; sampled-vector slot and rotation/output scheduling remain.
// MiniMax R (10-31): shared sample/forward vector 160 with retail's `!(0.0001f > fabs(angle))` guard, 198 with `>=`; in-place rotated 166, old-body retail guard 173; turn block on the axis 227/210, cross dots on facing_goal 194; no lower score.
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
// Round 56 (m3 lane 1, fast): the sample vector in its own block closed after the velocity stores (angle outside)
// 155 (same code: scope does not place the slot); that with `!(0.0001f > fabs(angle))` 173 (the test and the
// forward copy then retail's, the sample still at +0x18); one vector for the sample, the forward copy and the
// rotation done in place (cross and projection first) with that test 155 at 1382 bytes (everything up to the
// rotation is then retail's: sample at +0xc, test, copy; left the axis read into registers where retail uses
// memory operands, and sine in the angle's dead slot +0x34 where retail puts cosine there and sine at +0x44);
// on that, cosine before sine 156, the moved-away flag preset false and set in its test 178.
// Round 191 (lane 3, fast): on the 1382-byte shared sample/forward vector with the
// `!(fabs(angle) < 0.0001f)` guard and an in-place rotation (155, everything before the rotation
// matches): rows assigned j, i, k 149 (a compile-only screen of the rotation block also ranks j,i,k
// first; dot term/operand orders, cross component orders and sine/cosine order change nothing);
// with dot3f for the four turn-block dots 149 (VC still forms the alignment dot k, j, i; retail j,
// k, i); plus dot3f for the projection 154; plus cross3f for the turn crosses 154, and for the
// rotation cross 154. The shared-vector in-place rotation through the sibling 0x1ecf50's rotate form
// 176 (1385 bytes); a separate forward local rotated in place with the sample in its own block 169
// (the slot is not shared). Retail keeps the vector components in XMM and multiplies by the axis from
// memory; ours loads the axis into the destination registers.
// Round 293: Complete scalar velocity-product schedules ijk/ikj/jik/jki/kij/kji
// 219 each (frame 0x24, retail length); all-input then all-product and per-row compound
// producers 213/213. Original sampled-vector, frame 0x18 and full body restored.
// DL220: Native scalar rotation-cross storage, crossed with original/axis scalar observations and complementary cosine:
// 196/203/248/236/196/221/247/228. Sample home and rotation/output scheduling remain. Original body restored.
// DL224: Final direction decision in bool/long/byte storage, bit before/after publication:
// 173/166/173/166/173/166; selected direction pointer consumed twice, bit before/after publication
// 213/215. All restored; sampled-vector slot and rotation/output schedule remain.
// Round 76 (lane 12, fast, 0x24590 listed; al = aligned count against this body's 97): the sample and rotated vector as
// one local, with an in-place rotate helper (projection first or cross_i first) and the `up` parameter as axis,
// `!(0.0001f > fabs(angle))` 204/204, `>=` 215/215; with `&state->up` as axis 165/165 (1383 bytes, al 123), `>=`
// 197/197; retail still reads v.j/v.k into registers and multiplies by memory for the axis.j/k products. 120 random
// spellings of that helper (sum order, operand sides, cross order, compile-only) stay flat: 37 of the 78 rotation lines differ. None kept.
// DL226: Existing complete vector setter at goal output, rotated output and original-input
// copy boundaries, subsets 1..7 then all with the complete rotated producer: 164/201/193/218/210/224/216/213.
// Both output setters reach 1382 bytes at 193; all restored. Sample/rotation/output homes remain.
// DL235: Native output bitfields at input/support/turn boundaries, subsets 1..7,
// then all with word bit storage: 372/206/398/155/372/206/398/396.
// Original record and body restored; sampled-vector and rotation/output homes remain.
// Round 510: overlap-safe original-input/goal-output/rotated-output library-copy subsets
// 202/162/222/155/225/168/228; all plus initial native copies 386. Rotated-output-only reaches retail
// length but ties 155; broad copies raise the unmatched caller. Original body restored; sample home
// and rotation/output schedule remain.
// DL237: Existing complete scalar setter at sampled velocity, crossed with complete
// cross/rotation setters and uniform live scale: 186/155/186/157/209/182/187/182. All restored;
// sample home and rotation/output scheduling remain.
// DL239: Complete native up-vector for rotation and side tests, four acquisition phases with
// original/multiply-used uniform forward ownership: 266/268/268/268/255/256/256/256. Original
// complete body, observations, arithmetic and ordered angle guard restored; sampled-vector and
// rotation/output homes remain.
// J242: Final direction pointer record/scalar 199/193; final bool/byte/long decision 184 each,
// delayed publication arms 140; native vector result/common exit 114/114; coherent rotation
// inline/force-inline 114/114. NOT kept: no gain. Original body and every declaration restored; j/k
// rotation ownership and turn/output scheduling remain.
// Earlier: 1389 bytes against 1382, aligned 61 against the old body's 97. The existing rotate_direction on a
// separate copy of forward, with sin and cos as direct arguments and `!(fabs(angle) < 0.0001f)`, gives retail's sample
// slot, guard, schedule and rotation registers except four products: retail copies v.j/v.k and multiplies by axis.j/axis.k
// from memory, ours loads the axis (the same for goal.j against rotated.j/k in the turn block, and from them the
// last dot's order). Tried on this body (fast with a local copy of the helper, the plain build inlines the existing one
// the same): `moved_away` preset false and set in the test 92 at 1387 bytes (retail's `xor dl, dl`/`mov dl, 1`; a flag set in a
// test, match only); the side dots as (up, new) * (up, old) 106 (aligned 51; NOT kept: an operand order picked by score, retail
// forms the old-cross dot first at 0x1e607d; body restored at 114), (new, up) * (up, old) 107 (aligned 44), (old, up) * (new, up) 111,
// named side values 150 (1382 bytes, the rotation products all load first), moved_away computed before the crosses 190, the
// crosses declared apart or swapped 106 (same). Compile-only: parameter orders of the helper, cross or dot orders and
// operand sides, component locals, named sine/cosine (worse), a named rotation or rate, `angle *= rate`, a named axis,
// helper-returned pointers, indexed or cast access, by-value vector: about 6000 forms, none gives the four products with
// retail's registers. Past a size limit of the tail block (15 extra one-field reads, diagnostic) the products come out
// as retail's, but v.j/v.k take XMM1/XMM2 where retail has XMM5/XMM6.
// B384: Current rotation-helper input with shared sample/rotation workspace, native/CRT/forward/backward
// copies; independent CRT/forward/backward/setter copies: 176/176/193/189/114/188/188/205. Original helper input,
// separate sample and rotated values and complete body restored; rotation product allocation remains.
// AC502: Three nested animation admissions, four independent failure-prefix lengths and
// complete admission with small-angle failure: 114 each. Original guard, shared rotation call,
// vector storage and complete body restored; rotation multiplicands and exit scheduling remain.
// F351: Exhaustive nested/conjunction moved-away results, bool/byte/short/long
// 114/114/119/118/114/114/119/118. NOT kept: no match for result/domain forms;
// original rotation graph and complete body restored; j/k products and output scheduling remain.
// Lane 17 r37 (plain): `moved_away` as `alignment > 0.99f ? ... : false` 114; its dot as (rotated, goal) 114; the
// side crosses declared with the sampled vector 114; both side crosses through the existing cross3f_staged 112 (1389
// bytes, same cross schedule; gain 2, not kept); on that, `dot < alignment` 112. Body restored.
// B394: Native first-channel fields, direct/reference/pointer ownership 114/140/140;
// those with existing native physics input 140/150/150; native fields/output reference 114;
// channel/input/output identities 150. NOT kept: ties or regressions; original complete body restored.
// DH319: Complete final turn-test stop/continue results in bool/byte/short/long
// storage: 140/140/140/140/140/140/140/140, all 1382 bytes. Original expression
// restored; native j/k products, side-dot and output scheduling remain.
// Round 66 (m3 lane 11, fast with 0x24590, 0x1cd6c0 and 0x1ecf50 listed): `turning && c` 116; the
// alignment dot as (forward, goal) 116; `if (c) if (turning)` 114; `rate * angle` 114; the side crosses
// declared at the top of the turn block 114; `0.5f < alignment` 114. Unused-local probes (1-8, diagnostic,
// same counts anywhere before the rotation): 140/150/150/139/163/166/144/112, 1-6 at 1382 bytes; with the
// moved-away flag preset and set in its test 94 (probes 145-167). Both exits: retail loads all three
// forward dwords (ESI/EDX/EAX, flags in EBX) before its stores, ours copies interleaved.
// Y562: Native real absolute/sine/cosine APIs, subsets 1/2/4/3/5/6/7, then all
// with a double angle producer: 201/196/196/106/106/101/131/131. NOT kept: combined API gains need a byte match; isolated APIs regress. Original body restored.
// Y564: Existing complete vector setter on the current rotation graph at goal/rotated/initial
// copy boundaries, subsets 1/2/4/3/5/6/7: 122/176/205/171/197/203/195.
// Both final overlap-safe library copies 99 at 1382 bytes (plain 99). NOT kept: adds
// library calls where retail copies three components inline. Original entire body restored.
// P298: Actual turn admission at entry/sample/scaled/rotation boundaries, short-circuit
// 382/335/203/192; eager Boolean inputs 336/183/183/183. All restored; rotation
// multiplicands and exit scheduling remain. NOT kept: early decision lifetimes did not match.
// F371: Uniform side-cross row-local leading/trailing/both product parts
// 112/112/93; complete product phases 112/112/93; complete i/j/k
// component results mutable/const 112/112. NOT kept: the 93 product-lifetime
// forms still miss native rotation multiplicands, side-product residence
// and final load-all copies; allocation-only gain requires equality. Original
// private producer and full body restored; rotation provider stays exact.
// B422: Actual goal-alignment cutoff admission, bool/byte/short/long, positive/complementary:
// 142/142/142/142/142/142/142/142, all 1382 bytes. NOT kept: no byte match; decision residence worsens rotation allocation.
// Original complete body restored; vector/axis products, side dots and final copy scheduling remain.
// F373: High-alignment predicate bool/byte/short/long, after/before
// the side crosses: 140/140/140/140/172/172/176/175. NOT kept: no match;
// original body restored, rotation multiplicands, side-dot residence and
// final load-all copies remain.
// F379: Isolated oriented-side crossing/complement outcomes, bool/byte/short/long,
// 140/140/140/140/140/140/140/140 at retail length. NOT kept: regressions;
// original complete body restored, native j/k multiplicands and final copy scheduling remain.
// N366: Native direction-output copy ranges at goal/rotated/initial-forward boundaries and complete
// backward ranges do not recover eager inline copies. The 100-point output forms add out-of-line copy
// calls absent from retail, including the retail-length form. Original direction copies, shared rotation
// and full body restored; j/k products, side-dot and exit scheduling remain. Scores
// 100/110/100/303/308/307/312/318. NOT kept: unmatched result-ownership forms.
// Round 588: Complete scalar captures at goal/turned final copies and rotation input,
// seven subsets then all immutable: 150/174/192/181/206/223/221/221. NOT kept: one-use
// input captures miss equality. Original shared rotation and complete body restored;
// j/k rotation products, side-dot residence and final load-all copies remain.
// P337: Complementary moved-away bool/byte/short/long, complete logical producers
// 95/95/101/99; exhaustive arms 115/115/119/118. NOT kept: inverted result/test
// branches miss native early zero; j/k multiplicands and eager final copies remain.
// Original complete body and every existing helper restored.
// P339: Complete absolute-angle producer at sample/scale boundaries, mutable/const
// real/double: 144/144/172/172/144/144/172/172. NOT kept: all regress;
// original complete body restored. Native rotation multiplicands, side dots and
// eager final copies remain; shared rotation and movement providers stay exact.
// P341: Existing native component getter/support input, pointer/reference and mutable/read-only, shifted
// word/native bitfield: 114/114/114/114/114/114/114/114. NOT kept: no native rotation-product or
// side/output allocation recovery; original raw input, include set and complete body restored.
// Round 64 (m3 lane 14, fast with 0x24590, 0x1cd6c0, 0x1ecf50 listed; masked aligned 55 at 114): the goal copy by components 120
// (aligned 51; direct [edi+N] stores, retail keeps ECX), the rotated copy by components 169 (aligned 129, forwards the products),
// both with the preset flag 176; memcpy for either or both exits 114 (identical to the struct copy); dead reads ahead of the turn test
// 8/15/20/30/45 112/144/150/140/140, ahead of the crosses 8/15/30 112/145/140 (the axis loads first in all of them). Retail's exits load
// all three dwords into ESI/EDX/EAX before any store; no copy form gives three registers. Body unchanged.
// T72: Existing side-cross pointer/reference output and ignored pointer/reference results,
// reference-output consumed results and all-reference input/output contracts:
// 114/114/114/114/114/150/114/114. NOT kept: no native rotation/side/output equality;
// original private helper, both invocations and complete body restored.
// Lane 9 r98 (fast, 0x24590/0x1cd6c0/0x1ecf50 listed, control 114): the flags store before the goal copy 107 (1384 bytes; ours
// then ORs the flags in memory, retail loads them into EBX among the goal loads; a statement order picked by score, gain 7, not
// kept); the side crosses through the existing cross3f 114 (same code); diagnostic only, that flags order with the moved-away flag
// preset and set in its test and the side dots as (up, new) * (up, old) 90 at 1382 bytes (rotation products, the turn crosses'
// registers, the alignment and side dots and the load-all exits left); the moved-away test written inline in the condition 160 (1367);
// a named `axis` pointer for the rotation and both side dots 148 (1382). Body restored.
// T77: Complete goal-output record staging mutable/read-only, immediate or flags-between
// input capture and publication 140/134/140/134; turned-output record staging or full
// coordinate initializer, mutable/read-only 140/189/140/189. NOT kept: eager copy-boundary
// diagnostics miss equality; original full body and shared rotation call restored.
// Lane 1 r83 (fast, 0x24590/0x1cd6c0/0x1ecf50 listed; al = aligned lines left / the same with registers masked, control 114, al 61/37).
// LEAD, both exit tails then retail's (ESI/EDX/EAX loaded before the stores, flags in EBX, last 32 lines equal): the goal copied into the
// rotated local (`rotated = *facing_goal; state->flags |= 32;`) with no early return and one `state->forward = rotated;` at the end:
// 114 (1389 bytes), al 47/31. Not kept (the count ties). On that form: the moved-away flag preset false and set in its test 77 (1387, al
// 45/29, a flag form), plus the side dots as (up, new) * (up, old) 75 (al 35/27); the side dots alone 106 (al 37/29), their arguments swapped
// 111; the side test in a named bool 123 (1382), named before moved_away 205, after it with moved_away computed last 205; `moved_away ||`
// first 104 (1382, no flag byte); the moved-away test inline 160 (1367); its dot as (rotated, goal) or `dot < alignment` 114; alignment as
// (forward, goal) 116; the new cross first 124 (1382); cross3f_staged for both or the old cross 112, for the new one or cross3f for both
// 114; `turning && c` 116; moved_away as if/else or a ternary 114; a `snap` bool or'ed from both tests 123; rotated declared beside the
// sample 114. Both exits through the existing by-value vector3f_assign on the old body 171 (1383, the components go through XMM).
// Unused-local probes 1-8 on the lead (diagnostic): 123/133/133/122/164/167/144/112, 1-6 at 1382 bytes with every rotation product
// load-first; period 8. On the lead, the existing float trig calls: sinf and cosf for the rotation 84 (1384, al 52/40: the rotation is
// unchanged and the turn block's products move, a phase change; each alone 196/196, fabsf for the guard 201, all three 132), with the flag
// form 106, with the swapped side dots 115, with both 87; the time step read through a new inline getter (diagnostic, plain or forced,
// either operand order) 127 (1382, al 103/66: every rotation product load-first), with the flag form 170, with the side dots too 168.
// Left on the lead: the four rotation products and the new cross's goal.j products (retail copies the register and
// multiplies from memory), the old cross's two forward.i products (retail loads forward.i), the side-dot order and the flag byte.
// Lane 5 r166 (fast, 0x24590/0x1cd6c0/0x1ecf50 listed, control 114): `__forceinline` on initial_cross 114 (same code; last
// measured on the 155 body); the angle guard as `fabs(angle) >= 0.0001f` 205 (1391 bytes); the rotation angle computed before
// the forward copy 125 (1388); angle declared before movement 114; the side test as `0.0f >= product` 114. Body restored.
// Lane 12 r104 (fast, 0x24590/0x1cd6c0/0x1ecf50 listed, control 114): lane 1's lead (goal copied into `rotated`, one store
// at the end) re-measured 114; on it the flag store before the goal copy 107 (1384 bytes, lane 9's number for that order on
// this body); that with `moved_away ||` tested first 112 (1374); both orders on this body with its early return 111 (1374);
// `moved_away ||` first alone on this body 104 (1382; it short-circuits past the side products, which retail evaluates
// first (its jae at 0x1e60d1, then the flag), so not retail's order and not kept); the sine and cosine named before the rotation call 150 (1382).
// Also: both side crosses with their arguments reversed (the product's sign is unchanged) 138 (1394); the side dots' arguments
// swapped on this body 111 (1393; lane 1's number for that swap on the lead); moved_away preset false and assigned the second
// comparison inside the 0.99 test 117 (1391).
// Round 148 (m3 lane 7, fast; masked = register-masked aligned count, control 114 [55]): lane 1's lead with sinf/cosf
// re-measured 84 [60] (1384 bytes); new on this body: sinf/cosf for the rotation 101 [72] (1384; the rotation products are
// unchanged and the turn crosses move, a positional gain, not kept), with fabsf for the guard too 131 [83] (1375), with the
// sine and cosine named before the call 167 [129], with `angle * rate` passed to both and no rotation local 186 [91] (1365);
// initial_cross taking references 114 (same code); the side test as `!(product > 0.0f)` 104 [61] (1390; it compares the
// product with a zero constant where retail and this body compare a zeroed register with jae, so the gain is positional,
// not kept). After it: the moved_away comparisons written constant first (`0.99f < alignment && dot < alignment`) 114,
// initial_cross `__forceinline` with references 114, a selected-forward pointer (`new_forward = facing_goal;` in the snap
// case, one `state->forward = *new_forward;`) 199 [129] (1352 bytes), the angle guard as `if (fabs(angle) < 0.0001f) return;`
// 114, the animation test as an early return 114 (all 114s same code); re-measured: the side dots as (new, up) * (old, up)
// 111 (1393, the count and length of the listed (old, up) * (new, up)), `0.5f < alignment` 114. The rotation products
// come from rotate_direction (an Avoid file this round).
// m3 lane 1 r92 (fast, 0x24590/0x1cd6c0/0x1ecf50 listed, control 114, register-masked aligned 55): no trial run. The first
// difference (+0x1f3) is the inlined rotate_direction's dot and cross products (ours loads axis.j/axis.k and multiplies by the
// vector's registers, retail copies v.j/v.k and multiplies by the axis from memory); that body sits in src/unknown_1eb8a0.cpp,
// an Avoid file again this round, and a private copy here would be a twin of an existing helper. The rest (initial_cross's
// schedule, the side-dot order, the moved-away flag byte, the exits) is the commutative-operand group of round 83's lead; every
// natural form for those found here (cross helpers, cross order and declaration, dot sides, named dots, flag spellings, the
// joined exit) is listed above. Blocked: rotate_direction's body in src/unknown_1eb8a0.cpp is held this round.
// M3 lane 15 r76 (fast with 0x24590/0x1cd6c0/0x1ecf50, and for the helper forms every rotate_direction user and
// src/unknown_1eb8a0.cpp's functions; aligned/masked in brackets, control 114 [88/55]): rotate_direction (not held this round) with
// its dot as dot3f(axis, vector), its cross rows written axis first, or `(1.0f - cosine) * dot` 114 each (same code in every user;
// round 16's compile-only screen had covered operand sides, this is their first LTCG measurement);
// its cross through the existing cross3f into a local 180 (1382 bytes; 0x1ecf50 166, 0xe3910 190, 0xd0080 93, restored). In this
// body: the rotated copy written per component, or as an aggregate initializer from the three fields, 192/192 (1388); the velocity
// rows through the existing vector3f_scale(&movement, scale, &state->velocity) 166 (1382 bytes, the stores are unchanged and the
// rotation recolours), with `state->scale` passed directly 166, on lane 1 r83's single-exit lead (re-measured 114 [68/43]) 167.
// MiniMax D (11-11): no new form, no lower score (114): the first difference is still inside the inlined
// `rotate_direction`, whose definition is in another file, and the moved-away flag spelling that the notes above record
// at 92 needs a flag set in a test, so it is not keepable.
// m3 lane 4 r191 (fast with 0x24590/0x1cd6c0/0x1ecf50, and 0xe3910/0xd0080 for helper forms; control 114, aligned/masked
// 89/55): the six flag statements through bit_vector_set 365 (1404 B); moved_away as `alignment > dot && alignment > 0.99f`
// 96 (1383 B, masked 67: positional; retail tests 0.99 first); a new no-argument tick-step inline getter for the rotation
// rate on this body 144, with sinf/cosf 186, with that moved_away 174. rotate_direction body forms (control 0x1ecf50 MATCH,
// 0xe3910 79, 0xd0080 58): an input copy 190 (every user worse); a named versine 112 (1393 B, 53/40: the turn-block crosses
// and side dots become retail's, the rotation products do not; 0x1ecf50 14); the dot written out 111 (73/57); grouped
// (j + k) + i 109 (1382 B, 105/83; 0x1ecf50 6), {k,i} + j 124, {j,i} + k 117 (0xd0080 38, 0x1ecf50 112); rows grouped
// `v * cos + (p * a - c * s)` 208; a cross aggregate local 180; crosses before projected 154; rows into locals 174; no named
// dot (`dot3f(v, a) * (1.0f - cosine)`) 94 at 1382 B (84/52; 0xd0080 36, but 0x1ecf50 91, 0xe3910 144); on that a named
// versine 128, the dot written out inside the projection 112 (0x1ecf50 MATCH, 0xd0080 44, 0xe3910 144), that grouped
// (j + k) + i 126, crosses first 187, rows into locals 177; rows `v * cos - c * s + p * a` 169, grouped so 171. The four
// rotation products (dot j/k, cross_i) never move.
// Round 178 (m3 lane 5, fast with 0x24590/0x1cd6c0/0x1ecf50; al = aligned differing lines, control 114, al 89; about 80
// forms). The local-count scan that matched 0x1e6360 (n unused locals, n = 0..7, diagnostic): this body 114, 140, 150, 150,
// 139, 163, 166, 144; lane 1's single-store lead (the goal copied into the rotated local, one store at the end) 114 (al 68),
// 123, 133, 133, 122, 164, 167, 144; the lead with moved_away preset false and set in its test 77 (1387 bytes, al 65), 166,
// 163, 163, 165, 170, 170, 145; that with the side dots multiplied new first 75 (al 48), 164, 164, 164, 162, 164, 164, 146.
// Zero is the best count on every structure, a placeholder scores the same wherever it is declared (function top, before
// the forward copy, after the rotation, inside the turn block or the alignment block: 164 / 166), and one more local turns
// the whole rotation, so the count is a global state and not a per-site lever here. Per-site forms on the lead with the
// flag and the new-first product (75): the new side dot as (new_cross, up) 69 (1387 bytes, al 38, the closest body: both
// exits, the flag, the old cross's registers and all but one side-dot product are retail's), the old side dot as
// (old_cross, up) 108 (1391, al 49), both 108, the moved-away dot as (rotated, goal) 75 (same code), with sinf/cosf 87
// (1383). On the 69 form: the cross helper's pointer parameters declared second operand first, at the new cross 69 (same
// code), at the old cross or both 108 (it moves the side dots, not the cross); no scale local 166 (1393); a named axis
// pointer 103 (1383, al 55); a named pointer to the state's forward vector 161 (1382); the moved-away dot named inside the
// 0.99 test 195 (1382), with no scale local to keep the count 188 (1385, al 47); no component local 404 (1421: the flag
// tests change, it is not a count step). Without the flag form (match-only): the lead with the new-first product 106 (al
// 51), plus (new_cross, up) 107 (al 41), (new_cross, up) alone 111; the new-first product and (new_cross, up) on this body
// 107 (al 62). None kept: the gains come from the flag form or from operand orders picked by score, and retail's
// `mulss xmm1, xmm0` multiplies old by new. Left on the 69 form: the rotation's dot j/k and cross_i products (retail
// copies the vector's component and multiplies by the axis from memory), the old cross's two forward.i products (retail
// loads forward.i), the new cross's rotated.k * goal.j, the moved-away dot's term order (retail j, i, k) and the old side
// dot's up.i * old.i. In retail the rotation's operands order as vector j, k, then axis k, j, then vector i, axis i (ours:
// axis k, j, then the vector, then axis i); rotate_direction with its cross rows ahead of the dot gives 154 here at retail's
// length with the first difference at +0x221, but loses matched 0x1ecf50 (160) and raises 0xe3910 and 0x1ff360, so the
// helper stays.
// P440: Plain coordinate-copy loops at goal/turned/initial boundaries, seven subsets then word ordinals: 161/207/215/172/206/206/205/155.
// NOT kept: Native rotation j/k products, side dots and eager output loads remain. Original complete body restored.
// P458: Complete rotation/small-angle outcomes, bool/byte/short/long:
// 144/144/144/144/144/144/144/144. NOT kept: native rotation j/k products, side dots and eager output copies not recovered; original complete body restored.
// Round 177 (m3 lane 6, fast with 0x24590/0x1cd6c0/0x1ecf50): the rotated copy from the `forward` parameter 194 (1395 bytes);
// the rotation axis from the `up` parameter 277 (1396); rotate_direction (body restored) with its dot as dot3f(axis, vector),
// its cross rows axis-first, or its rows projected-first 114 each (same code; 0x1ecf50 MATCH, 0xe3910 79, 0xd0080 58). A
// compile-only copy with rotate_direction pasted into the file reproduces this rotation exactly; in it the state pointer as
// the parameter itself, the crosses before the dot and a named versine do not give retail's dot j/k operand order.
// P468: Plain sampled-movement/output coordinate loops on current shared-rotation
// graph, long/dword/short/byte for then while: 150/150/217/217/150/150/217/217.
// NOT kept: no native rotation-product equality; original complete body and all
// helper/provider definitions and declarations restored.
// P483: Initial input-vector memcpy families for frame/control/ground velocity,
// seven subsets then all with default velocity: 126/126/196/101/114/114/111/112. NOT kept: 101 is
// combination-only (both individual families 126); 111/112 gain only three/two.
// Native rotation products/side dots/output loads remain; full body and headers restored.
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
  if (!(fabs(angle) < 0.0001f))
  {
   vector3f rotated = state->forward;
   real rotation = angle * g_510c54->rate;
   rotate_direction(&rotated, &state->up, (real)sin(rotation), (real)cos(rotation));
   if (c && turning)
   {
    real alignment = dot3f(facing_goal, &state->forward);
    if (alignment > 0.5f)
    {
     vector3f old_cross, new_cross;
     initial_cross(facing_goal, &state->forward, &old_cross);
     initial_cross(facing_goal, &rotated, &new_cross);
     bool moved_away = alignment > 0.99f && alignment > dot3f(facing_goal, &rotated);
     if (dot3f(&state->up, &old_cross) * dot3f(&state->up, &new_cross) <= 0.0f || moved_away)
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
void function_1f1460(s_shape_state const *ground, vector3f *up, vector3f *forward, byte const *state);
void function_1faa20(vector3f *up, byte const *state, vector3f *forward, long *ticks);
void function_1ecf50(vector3f *forward, vector3f *up, s_direction_rotation_input *input);

/* parameter order from retail's call setups, which load the register
   arguments last to first after pushing forward: 0xe3c90 and 0x118700 load
   output (EAX), up (ECX), then physics (EDX); 0xdd360 loads up before output */
// @retail 0x1e5af0
void function_1e5af0(void *physics, vector3f const *forward, vector3f const *up, s_biped_physics_output *output)
{
 switch (*(byte *)physics)
 {
 case 1: function_1f1460((s_shape_state *)((byte *)physics + 0x10), (vector3f *)up, (vector3f *)forward, (byte *)output); break;
 case 2: function_1faa20((vector3f *)up, (byte *)output, (vector3f *)forward, (long *)((byte *)physics + 0x10)); break;
 case 3: function_1ecf50((vector3f *)forward, (vector3f *)up, (s_direction_rotation_input *)output); break;
 case 4: break;
 case 5: break;
 case 6: break;
 default: __assume(0);
 }
}
