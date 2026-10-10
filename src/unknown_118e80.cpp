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

PRIVATE __forceinline void object_flag_set_118fe0(s_forward_flags *flags, bool enabled)
{
	if (enabled)
		flags->flag0 = true;
	else
		flags->flag0 = false;
}

// @retail 0x118fe0
void function_118fe0(long object_index, bool enabled)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (TEST_FIELD_BIT(object->frozen))
		enabled = false;
	object_flag_set_118fe0(&object->flags, enabled);
}

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
void function_11d180(vector3f *left, vector3f const *in, vector3f *out, vector3f const *up, vector3f *forward);
real function_11ce20(vector3f const *a, vector3f const *b);
void function_109290(long object_index, long a, long b, long c);

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
		max_angle *= definition->speed_scale;
		acceleration *= definition->speed_scale;
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
	vector3f temporary_velocity = *g_4687a4;
	vector3f up;
	world_up_1197b0(object_index, &up);
	vector3f new_forward, target_up, left1, left2, forward2;
	function_11d180(&left1, &forward, &target_up, &object->requested_up, &new_forward);
	function_11d180(&left2, &forward, &up, &up, &forward2);
	if (function_11ce20(&up, &target_up) > max_angle * g_510c54->rate)
	{
		function_11f0d0(&temporary_velocity, &up, &target_up, g_510c54->rate, max_angle, acceleration * 2.0f);
		function_11d180(&left2, &forward, &up, &up, &forward2);
	}
	else
		up = target_up;
	point3f position = object->position;
	function_109290(object_index, (long)&position, (long)&new_forward, (long)&up);
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
		axis.i = g_4687b0->j * impulse->k - g_4687b0->k * impulse->j;
		axis.j = g_4687b0->k * impulse->i - g_4687b0->i * impulse->k;
		axis.k = g_4687b0->i * impulse->j - g_4687b0->j * impulse->i;
		function_30bf0(&axis);
		real magnitude = (real)sqrt(impulse->i * impulse->i + impulse->j * impulse->j + impulse->k * impulse->k);
		real scale = function_x82e52f(&g_4e7408->unknown0, NULL, 0) * magnitude * 1.57079637f;
		object->angular_velocity.i += scale * axis.i;
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
	bool result = true;
	if (mode != 0x7000101 || set != 0x7000101)
		result = state->animation_set(mode, 0x7000101, 0x7000101, set, flags, 0x3f);
	if (result)
	{
		bool mode_changed = old_mode != mode && mode != 0x7000101;
		bool set_changed = old_set != set && set != 0x7000101;
		if (mode_changed || set_changed)
		{
			volatile real blend = 0.267f;
			if (mode_changed)
				blend = 0.267f;
			if ((old_set == 0x400000c && direction_state_119650(set)) ||
				(direction_state_119650(old_set) && (direction_state_119650(set) || set == 0x400000c)))
				blend = 0.267f;
			if (blend <= seconds)
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
    (void)&arg_0;
    (void)&arg_1;
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    s_animation_state *local_1 = (s_animation_state *)((byte *)local_0 + local_0->animation_state_offset);
    memset(arg_1, 0, sizeof(*arg_1));
    arg_1->field_0 = 0x7000101;
    arg_1->field_4 = 0x7000101;
    bool local_2 = false;
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
    if (local_3->animation_state_offset == NONE)
        return false;
    bool local_4 = true;
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
            if (local_11 || local_12)
                function_ba350(arg_0, 0.267f);
        }
    }
    else
        *arg_2 = true;
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
    byte field_4;
    byte field_5[3];
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
void function_1e5a60(s_biped_physics_result *arg_0, s_biped_physics_input const *arg_1, void *arg_2);

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
            long local_9 = NONE;
            if ((bool)((local_8 >> 2) & 1) || (bool)((local_8 >> 1) & 1))
                local_9 = *(long *)((byte *)local_1 + 0xb8);
            s_118b80 local_10;
            s_118b81 local_11;
            byte *local_12 = local_4 + 0xd4;
            byte *local_13 = (byte *)local_1 + 0x17c;
            function_1e68a0((s_character_physics_update_input_datum_a *)&local_10,
                (s_source_a *)local_13, (long)local_12, *(long *)((byte *)local_1 + 0xb4),
                local_9, TEST_FIELD_BIT(local_1->flags.flag5), false, false,
                (bool)((local_8 >> 8) & 1), local_7, &local_6,
                (point3f *)&local_1->forward, (point3f *)&local_1->control);
            local_10.field_14 = true;
            if (!(bool)((*(dword *)local_12 >> 4) & 1))
                local_10.field_44 = *(long *)((byte *)local_1 + 0x1a8);
            function_1e5a60((s_biped_physics_result *)&local_11,
                (s_biped_physics_input const *)&local_10, local_13);
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
            if (local_11.field_4 & 1)
                *((byte *)local_1 + 0x12d) |= 1;
            else
                *((byte *)local_1 + 0x12d) &= ~1;
            if (local_11.field_4 & 1)
                *((byte *)local_1 + 0x12c) |= 0x80;
            else
                *((byte *)local_1 + 0x12c) &= ~0x80;
            if (TEST_FIELD_BIT(local_1->flags.flag5))
            {
                real local_16 = (*(dword *)local_12 & 8) ? 3.0f : 1.0f;
                if (*((byte *)local_1 + 0x131) * g_510c54->rate > local_16 * 0.15f)
                    local_1->flags.flag5 = false;
            }
            else
            {
                real local_17 = (*(dword *)local_12 & 8) ? 3.0f : 1.0f;
                if (local_7 || *((byte *)local_1 + 0x130) * g_510c54->rate > local_17 * 0.15f)
                    local_1->flags.flag5 = true;
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
void __stdcall function_b93b0(long arg_0, long arg_1, long arg_2);

// @retail 0x119c10
long function_119c10(long arg_0)
{
    (void)&arg_0;
    s_forward_object *local_0 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
    long local_1 = NONE;
    vector3f local_2;
    function_ba1d0(arg_0, &local_2, NULL);
    real local_3 = (real)sqrt(local_2.i * local_2.i + local_2.k * local_2.k + local_2.j * local_2.j);
    if (local_3 > 0.0001f)
    {
        real local_4 = *(real *)((byte *)local_0 + 0x3c) + 0.1f;
        real local_5 = local_4 / local_3;
        vector3f local_6;
        local_6.i = local_5 * local_2.i;
        local_6.j = local_2.j * local_5;
        local_6.k = local_2.k * local_5;
        real local_7 = 3.402823466e+38f;
        long local_8[16];
        long local_9 = function_bb050(1, 3, (byte *)local_0 + 0x28, &local_0->center, local_4, local_8, 16);
        short local_10;
        short local_11;
        short local_12;
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
                function_11d180(&local_26, &local_2, &local_6, &local_0->up, &local_2);
                function_b75a0(arg_0, &local_13, &local_2, &local_6, &local_15, false);
                function_b93b0(local_1, arg_0, local_11);
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
        byte *local_8 = local_1 + 0xd4;
        byte *local_9 = (byte *)local_0 + 0x17c;
        word local_10 = *(word *)((byte *)local_0 + 0xc0);
        function_1e5bb0((s_biped_physics_output *)&local_3, local_9, local_2, 1.0f,
            *(long *)((byte *)local_0 + 0xb4), arg_0, local_8, 0xf,
            (bool)((local_10 >> 6) & 1), false, true, false,
            (bool)((local_10 >> 8) & 1), TEST_FIELD_BIT(local_0->flags.flag5),
            (bool)((local_0->flags.value >> 8) & 1), false, g_51e9c4->unknown0, 1.0f,
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
        if (local_13)
            local_0->flags.flag4 = true;
        else
            local_0->flags.flag4 = false;
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
            if (!--*((byte *)local_0 + 0x204))
            {
                function_11a140(arg_0);
                real local_16 = g_510c54->field_2_3 * 0.2f;
                long local_17;
                __asm
                {
                    fld local_16
                    fistp local_17
                }
                *((byte *)local_0 + 0x204) = (byte)local_17;
            }
            local_4 = true;
        }
        else
            *((byte *)local_0 + 0x204) = 0xff;
        if (!TEST_FIELD_BIT(local_0->frozen) || !(*((byte *)local_0 + 0xc1) & 1))
        {
            *(long *)((byte *)local_0 + 0xbc) = g_510c54->game_time;
            function_bba20(arg_0);
            local_4 = true;
        }
        long local_18 = *(word *)((byte *)local_0 + 0x206);
        if ((short)local_18)
        {
            local_18--;
            *(short *)((byte *)local_0 + 0x206) = (short)local_18;
            if ((short)local_18 <= 0)
                function_d6bc0(arg_0);
            local_4 = true;
        }
        local_0->flags.flag6 = false;
    }
    return local_4;
}

bool (__stdcall *g_4685a8)(long) = function_118700;
