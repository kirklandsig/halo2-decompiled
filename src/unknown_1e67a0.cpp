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
	s_physics_movement_input *input = (s_physics_movement_input *)physics;
	const bool *airborne_reference = &airborne;
	const bool *limited_reference = &limited;
	const bool *mode_reference = &mode;
	const real *crouch_reference = &crouch;
	state->index = input->index;
	state->matrix = input->matrix;
	state->vector = input->vector;
	state->rate = rate;
	state->field_a8 = *mode_reference;
	state->direction = state->default_direction;
	state->enabled = true;
	real blend = 0.0f;
	if (!(state->flags & 4))
		blend = *crouch_reference;
	if ((bool)((state->definition->flags >> 2) & 1) && !*airborne_reference)
	{
		s_physics_movement_settings *settings = ((s_physics_movement_globals *)g_4e034c)->settings;
		vector3f control = state->control;
		real squared = length_sq3f(&control);
		if (squared > 1.0f)
		{
			real scale = 1.0f / (real)sqrt(squared);
			control.i = (real)(control.i * scale);
			control.j = (real)(control.j * scale);
		}
		real remaining = 1.0f - blend;
		real first, second;
		if (control.i > 0.0f)
		{
			first = settings->forward;
			second = settings->crouched_forward;
		}
		else
		{
			first = settings->backward;
			second = settings->crouched_backward;
		}
		state->velocity.i = first * remaining + second * blend;
		state->velocity.j = settings->sideways * remaining + settings->crouched_sideways * blend;
		state->velocity.k = 0.0f;
		state->speed = settings->speed * remaining + settings->crouched_speed * blend;
		state->field_148 = settings->field_4c;
		if (*limited_reference)
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

void function_1f1930(const s_shape_contact *contact, s_shape_side *side);
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
		case 1: function_1f1930((const s_shape_contact *)contact, (s_shape_side *)output); break;
		case 2: function_1faeb0((const s_contact *)contact, (s_contact_result *)output); break;
		case 3: break;
		case 4: function_1fc620((s_tracking_result *)output, (const s_tracking_source *)(state + 0x10), (const s_tracking_target *)contact); break;
		case 5: function_1fc620((s_tracking_result *)output, (const s_tracking_source *)(state + 0x10), (const s_tracking_target *)contact); break;
		case 6: function_1ec3f0((s_unknown_object *)contact, (s_unknown_output *)output); break;
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
	point3f *point = &component->position;
	if (!component->has_position)
	{
		point = (point3f *)g_4687b0;
	}
	datum->pointac = *point;
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
	datum->unknown10 = source->unknown00;
	datum->unknown00 = a1;
	datum->unknown40 = a2;
	datum->flags = 0;
	datum->unknown14 = 0;
	datum->flags = b0 ? 1 : 0;
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
