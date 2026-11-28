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
bool function_1faf80(long object_index, vector3f *facing, void const *settings_pointer, vector3f const *control,
	real threshold, real *turn);
void function_11f0d0(vector3f *forward, vector3f const *target, real rate, vector3f *position, real max_angle, real scale);
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
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (object->parent_index == NONE)
		*up = object->up;
	else
	{
		s_forward_object *parent = ((s_forward_object_header *)g_4e0300->data)[object->parent_index & 0xffff].object;
		transform4x3f *matrix = (transform4x3f *)((byte *)parent + parent->nodes_offset + object->parent_node * 0x34);
		real i = object->up.i, j = object->up.j, k = object->up.k;
		up->i = matrix->up.i * k + matrix->left.i * j + matrix->forward.i * i;
		up->j = matrix->up.j * k + matrix->left.j * j + matrix->forward.j * i;
		up->k = matrix->up.k * k + matrix->left.k * j + matrix->forward.k * i;
	}
}
/* object_up and object_forward are initialised eight local symbols apart on purpose (ahead of the zero velocity, and
   ahead of the position block): the compiler orders each agreement's i product by the field pointer's symbol number
   modulo 8, and retail's order (the local's component loaded first in both) needs both numbers at a multiple of 8 */
// @retail 0x1197b0
void function_1197b0(long object_index)
{
	s_forward_object *object = ((s_forward_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_facing_definition_1197b0 *definition = (s_facing_definition_1197b0 *)g_4e3b44[object->definition_index & 0xffff].bytes;
	if ((bool)((definition->flags >> 4) & 1))
	{
		function_1faf80(object_index, &object->requested_forward, definition->settings, &object->control, 0.9f, &object->turn);
	}
	else
	{
		vector3f turn_velocity;
		vector3f up, new_forward;
		vector3f *object_up;
		{
			vector3f target_up, first_left, basis_forward, left;
			real max_angle = definition->max_angle;
			real acceleration = definition->acceleration;
			if (object->speed_mode == 1)
			{
				max_angle *= definition->speed_scale;
				acceleration *= definition->speed_scale;
			}
			vector3f forward;
			if (max_angle == 0.0f && acceleration == 0.0f)
			{
				forward = object->requested_forward;
				turn_velocity = *g_4687a4;
			}
			else
			{
				function_118e80(object_index, &forward);
				turn_velocity = object->turn_velocity;
				function_11f0d0(&forward, &object->requested_forward, g_510c54->rate, &turn_velocity, max_angle, acceleration);
			}
			object_up = &object->up;
			vector3f temporary_velocity = *g_4687a4;
			world_up_1197b0(object_index, &up);
			function_11d180(&forward, &object->requested_up, &new_forward, &first_left, &target_up);
			function_11d180(&forward, &up, &basis_forward, &left, &up);
			if (function_11ce20(&target_up, &up) > max_angle * g_510c54->rate)
			{
				function_11f0d0(&up, &target_up, g_510c54->rate, &temporary_velocity, max_angle, acceleration * 2.0f);
				function_11d180(&forward, &up, &basis_forward, &left, &up);
			}
			else
				up = target_up;
		}
		vector3f *object_forward = &object->forward;
		{
			point3f position = object->position;
			object_dispatch_moved(object_index, (long)&position, (long)&new_forward, (long)&up);
		}
		if (dot3f(object_up, &up) < 0.9999f || dot3f(object_forward, &new_forward) < 0.9999f)
		{
			*object_up = up;
			*object_forward = new_forward;
			function_bba20(object_index);
		}
		object->turn_velocity = turn_velocity;
	}
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
    byte field_4[0x134];
    vector3f field_138;
    byte field_144[0xc];
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

// score 14 (was 14; 43 before): the 0x119bc0 result has its own named bool, which is stored into the move output's byte member
// and passed to 0x1c4b00 from that local, so the copy of the move output's velocity now loads the object before the
// source's first component and keeps the destination pointer in eax where this build keeps it in edx; retail still
// spills the result to a stack byte of its own and reloads the containing dword where this build keeps it in al.
// MiniMax D (11-20, run 131): no new form, no lower score (14, 1034 bytes): removing the move output's byte member store measures 90 and moving it after the +0x88 copy measures 11, and that one is a statement order kept only with a byte match, so both spellings are restored; retail hoists the copy's source component above the stores through the object pointer and spills the 0x119bc0 result to a stack byte that it pushes as a dword, where this build keeps the result in al and the tail needs 0x1c4b00's bool formal: BLOCKED-BY 0x1c4b00 (src/unknown_1c25a0.cpp).
// MiniMax H (11-20): no new form, no lower score (14): retail spills the 0x119bc0 result to a stack byte and pushes the dword; this build keeps it in AL; that needs 0x1c4b00's bool formal in other files.
// MiniMax D (11-20): no new form, no lower score (14, 1034 bytes): retail's block after the 0x119bc0 call still differs
// in the tail (retail spills the 0x119bc0 result to a stack byte in the dead definition pointer's slot and pushes the
// containing dword, this build keeps the result in al) and in the copy's load order (retail hoists the source's first
// component above the stores through the object pointer, this build does not).
// A typed vector member at the object's +0x88 (its unknown88, with the old value90 read rewritten as that member's k)
// and the copy written through the member give byte-identical code (14, 1034 bytes), so the copy's spelling is not what
// differs; the hoist needs the source local's address known not to escape, and the 0x1c4b00 call makes it escape.
// Still BLOCKED-BY 0x1c4b00 (bool formal and its 14 declarations, in other lanes' files), 0x1ec690 and 0x1eb6f0.
// MiniMax S (11-20): no new form, no lower score (14): the tail differs only in the force flag, ours keeping the 0x119bc0 result in al and pushing it zero extended where retail spills it to a byte home and pushes the containing dword, which needs the last parameter of 0x1c4b00 to be a bool in its definition and in its declarations, and those live in other files.
// M3 lane 3 r272 (fast with 0x1c4b00 and its callers; control 14, aligned 14, masked 6): the result read through a
// `bool const &` for the member store and the force 14 (same code); that reference without the member store 90; a `const &`
// to the re-fetched object pointer for the copy 41; a `vector3f const &` to the move output's velocity for the copy and the
// call 14 (same code). Diagnostic, HELD (build/r272/held-0x118700.patch, code back to main): the r258 held form (0x1c4b00's
// last parameter `bool` in its definition and all 13 declarations, the inline write helper, +0x88 typed as a vector)
// re-measured on this body 6 (aligned 3; contributor 0xe24f0 579 -> 601, 0x10b010 19 in the reduced image only); that form
// with the member store kept 91. Still BLOCKED-BY 0x1c4b00 (six of its declaration files are other lanes' this round),
// 0x1ec690 (its draft sits disabled in another lane's file) and 0x1eb6f0: the copy's j/k loads wait on the move output's
// escape through 0x1e55d0.
// M3 lane 14 r112 (fast; control 14): the member store after the 0x1c4b00 call 96 (1041 B); after the velocity copy,
// before the call 11 (three points, the force still widened by `movzx edx, al`; not kept); that with the member passed as
// the force 11; the object looked up before the member store 40; a named pointer to the move output's velocity for the
// copy and the call, member store after the copy, 11. The member store is never read afterwards (a dead store that only
// moves the scratch rotation). Still BLOCKED-BY 0x1c4b00 (its bool formal; declarations in other lanes' files this round).
// M3 lane 14 r114 (fast; control 14, aligned 14): the 0x1c4b00 linear argument as the object's own +0x88 copy 29 (aligned
// 18); the re-fetched object held as a byte pointer for the copy 14 (same code); the member and the local set by one chained
// assignment 14 (same code); the member set from the call and the force read back from the member after the lookup 97
// (1039 B); a named pointer to the move output's velocity for the copy and the call 14 (same code). Three of 0x1c4b00's
// declaration files (src/unknown_1eb8a0.cpp, src/unknown_0b7740.cpp, src/unknown_1e67a0.cpp) are other lanes' this round.
// MiniMax A (11-18): the 0x119bc0 result now has its own named bool, which is stored into the move output's byte
// member and passed to 0x1c4b00 from that local: 14 (was 43). The force argument's own zero extension and the member's
// slot are what still differ; retail still spills the result to a stack byte of its own and reloads the containing dword.
// Dropping that member store measures 90, so the store stays.
// Before, 43 (was 43): m3 lane 9 r134 (fast; the stacked score lines below merged into this one note). Ours differs only
// MiniMax A (11-18): no new form, no lower score (43): the tail still differs where retail spills the 0x119bc0 result to a stack byte and pushes the containing dword where this build keeps the result in al, and that shape needs 0x1c4b00's last parameter to be a bool in its definition and in its 14 declarations, which live in other files: BLOCKED-BY 0x1c4b00.
// Earlier at 43 (was 43): m3 lane 9 r134 (fast; the stacked score lines below merged into this one note). Ours differs only
// from the 0x119bc0 result on: retail keeps it in its own stack cell [esp+0x14] (shared with the rounding temporary),
// pushes that dword, and every later scratch register follows; ours stores it in the move output's byte member and
// zero-extends it, one scratch step ahead from there on. A bool local for it (long force) 90 (1034 bytes: the tail
// takes retail's scratch registers, but the value stays in AL for `movzx edx, al`, one row short), the same with an
// explicit `(long)` 90; the object re-fetched before the 0x119bc0 call 46 (1036 bytes); the copy through memcpy 43
// (same code); the member typed bool 43 (same code). Diagnostic, HELD (build/r134/held-0x118700.patch, code back to
// main): 0x1c4b00's force as bool in its definition and all 14 declarations with the bool local 40 fast (retail's
// spill and dword push; left are the copy loading the object before the source's i and the tail rotation; 0x1c4b00 and
// its matched callers still match, 0xe24f0 579 -> 601, 0x10b010 19 in the reduced image only as round 82 found).
// Still BLOCKED-BY 0x1c4b00 (its definition is in src/unknown_1c25a0.cpp, held by another lane this round).
// MiniMax S (11-16): 45 (was 92): the 0x119bc0 result is stored in the physics-move struct's byte member, so the byte store retail makes appears here too and the force argument is widened out of that member instead of straight out of al; the argument's own zero extension and the member's slot are what still differ.
// MiniMax E (11-16): no new form, no lower score (92): the frame is 0x1f0 as in retail and the first 200 aligned rows match; the tail still differs where retail spills the 0x119bc0 result to a stack byte and pushes the containing dword where this build keeps the result in al, and that shape needs 0x1c4b00's last parameter to be a bool in its definition (src/unknown_1c25a0.cpp) and in its declarations, which live in files this lane may not edit. Still BLOCKED-BY 0x1c4b00.
// MiniMax I (11-16): no new form, no lower score (92): the first 200 aligned rows match and the tail still
// diverges where retail spills the 0x119bc0 result to a stack byte and pushes the containing dword where this
// build keeps it in al; that shape needs 0x1c4b00's last parameter to be a bool, and its definition and its
// declarations live in files this lane may not edit. Still BLOCKED-BY 0x1c4b00.
// MiniMax C (11-15): no new form, no lower score (92): retail's 0x119020 pushes the force as a full dword
// (push 1) before its 0x1c4b00 call, so the two other call sites in this file compile against the long
// parameter and the bool form cannot be taken in this file alone.
// MiniMax G (11-14): no new form, no lower score (92): the frame is retail's (0x1f0) and the squared length is still
// summed in the compiler's own order (retail reads 0x1f0 before 0x1e8), and the 0x119bc0 result still travels in al
// where retail keeps it in a stack byte and pushes the containing dword; that shape needs 0x1c4b00's last parameter
// to be a bool in its definition and in its declarations, which live in files this lane may not edit.
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
// MiniMax A (11-14): no new form, no lower score (92): the frame 0x1f0 agrees, but the basis MOV/LEA, the
// squared-component loads and the force-result publication differ, and the force argument still needs the zero
// extension that 0x1c4b00's `long` formal cannot give (BLOCKED-BY that file).
// MiniMax D (11-15): no new form, no lower score (92): the score is still mostly the one-row shift from the movzx of
// the force argument, which needs 0x1c4b00's last parameter declared a bool in files this lane may not edit, and the
// squared length still sums the three components in the order the compiler picks where retail sums them from k down.
// MiniMax C (11-16): 92 (was 92): no new form, no lower score; the frame is 0x1f0 as in retail, but the
// score is still mostly the one-row shift from the zero extension of the force argument, which needs
// 0x1c4b00's last parameter declared a bool in files this lane may not edit (BLOCKED-BY that file), and the
// squared length still sums the three components in the order this build picks where retail sums them from
// k down.
// MiniMax E (11-17): no new form, no lower score (43): the tail still differs where retail spills the 0x119bc0 result to a stack byte and pushes the containing dword where this build keeps the result in al; that shape needs 0x1c4b00's last parameter to be a bool in its definition and in its 13 declarations, in files this lane may not edit (BLOCKED-BY 0x1c4b00).
// Earlier (43, was 45): frame 0x1f0 agrees; basis MOV/LEA, squared-component loads and force-result
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
// M3 lane 2 r82: HELD (needs the coordinator's OK): 0x1c4b00's last parameter as `bool force` (its two `(byte)force` casts
// dropped; the definition and all 13 declarations) 92 -> 42 plain at 1034 bytes, with 0x1c4b00 and every matched caller
// (0xb77d0, 0xe4770, 0xecdc0, 0x10b010, 0x119020, 0x119c10, 0x14e200, 0x1cf120, 0x1ed340, 0x2909b0) still matching in the
// plain check (the fast check showed 0x10b010 at 19, a reduced-image artifact); unmatched 0xe24f0 579 -> 594 (stack slots
// shift), 0xdd990/0xf8eb0/0x10b850/0x1e55d0 unchanged. Retail evidence: 0x1c4b00 tests only the flag's low byte and this
// caller pushes the found flag's stack dword without zero-extension. Two declarations sit in other lanes' queue files this
// round (src/unknown_1eb8a0.cpp, src/unknown_1cec30.cpp), so the code is back to main; build/r82/held-0x118700.patch. On
// the held form: the squared length through length_sq3f 42; the found flag and the re-fetched object declared at the
// block's top 42/42; the object fetched before the 0x119bc0 call 42; the squares in i, j, k order 42 (all the same code).
// Diagnostic: one to three unused `long` locals on it 42 each. Left there: the squares' i/k load order and the velocity
// copy into the object (retail loads the source's i before the object pointer, every later scratch register follows).
// MiniMax I (11-17): no new form, no lower score (45): the squared length's three components still load in the
// order the compiler picks, and the 0x119bc0 result still travels in al where retail pushes the containing dword,
// which needs 0x1c4b00's last parameter declared a bool in a file this lane may not edit.
// b12D (11-17) walk against retail, fast build on r846: every statement, offset, immediate, call argument and jcc
// sense lines up with retail (the one float branch is comiss/jbe at 0x118996/0x11899d, so NaN skips as in ours;
// constants 1.0f 0x45dbc0, 0.0f 0x45dbd8, 0.2f 0x45dc5c); no behaviour difference found. Kept, 45 -> 43 (1035
// bytes): the squared length through the shared length_sq3f (used elsewhere in this file) on a vector3f member at
// 0x138 of the local output struct gives retail's k, j, i loads (0x11895e, 0x11896e, 0x11897e); the hand-written sum
// via a pointer cast compiled i, j, k. Not kept (match-only, fast): the 0x119bc0 result in a bool/long union read as a
// dword gives retail's `mov [esp+0x14], al ... mov edx, [esp+0x14]; push edx` (0x1189d4, 0x1189f2), 1034 bytes, 40;
// a word local 12 (movzx ax, al); bool/long locals 90/91; a function-pointer cast to a bool formal changes 0x1c4b00's
// call convention, 145. Left on the union form: retail loads the copy's i into ECX (0x1189c9) before the esi reload
// and the byte store (0x1189d4) and takes the object into EAX; ours loads the object first, and the rest rotates.
// Tried there: the copy through a named pointer / memcpy / source pointer 40/40/40, per-member real stores 47/101,
// refetch before the 0x119bc0 call 40, before the squares 59. A match needs 0x1c4b00's bool formal (14 declarations).
// The velocity preload and force byte home still differ. Shared signed slot index 43, low-word slot 283,
// signed header byte offset 43, unsigned header byte offset 283. The same four with a separate Boolean force
// result: 90, 287, 90, 287. Narrowed slots move the first difference into the opening lookup. Body restored.
// M3 lane 3 r258 (fast with 0x1c4b00 and all its callers; control 43): HELD, needs the owners' OK
// (build/r258/held-0x118700.patch, code back to main). Retail's block after the 0x119bc0 call is one inlined helper: the
// object lookup, `if (linear)` / `if (angular)` copies into +0x88 / +0x94, then the 0x1c4b00 call with the caller's bool
// passed straight through (0x1189c9-0x1189f9: the bool spilled to its own cell and its dword pushed). Written that way
// (a file-local inline helper, the object's +0x88 typed as a vector) with 0x1c4b00's last parameter `bool` in its
// definition and all 13 declarations: 6 at 1034 bytes (aligned 2: the copy loads j and k one step later than retail);
// the helper with the parameter left `long` 93 (1034 bytes, the flag stays in AL for a movzx); the bool formal without
// the helper 40 (re-measure of the earlier held form). On the 6 form, same code each: a destination pointer local, raw
// casts, const pointers, forceinline, the 0x119bc0 call as the argument. The last two rows come from the output local's
// address: retail hoists the copy's loads above the stores through the object pointer, which this compiler does only
// when the source local's address is known not to escape (compile-only probe: a source passed to an opaque function
// gives our order, one that stays in visible code gives retail's). The local goes to 0x1e55d0, which hands it on:
// with every callee of 0x1e55d0 given NULL in its place (diagnostic, restored) this function MATCHES in the fast check;
// kept alone, the 0x1ec690 call (a stub, not yet written) or the 0x1eb6f0 call each brings the 6 back, the other five
// callees do not. A match therefore needs the bool formal (three declarations sit in other lanes' files this round and
// a contributor's 0xe24f0 moves 586 -> 601), this helper, 0x1ec690 written as real code and 0x1eb6f0's chain
// (0x1eb8a0, 0x109fd0) keeping its output to itself. BLOCKED-BY 0x1c4b00 (declarations), 0x1ec690 and 0x1eb6f0.
// MiniMax A (11-17): no new form tried this round, no lower score (43): the tail still differs where retail spills the 0x119bc0 result to a stack byte and pushes the containing dword where this build keeps the result in al; that shape needs 0x1c4b00's last parameter to be a bool in its definition and in its 14 declarations, which live in other files. BLOCKED-BY 0x1c4b00.
// M3 lane 8 r186 (fast with 0x119020/0x119c10/0x1c4b00/0xb7740, matched throughout; control 14): retail's block after the
// 0x119bc0 call (object looked up again, the linear copy into +0x88, no update call) has the shape of the existing
// velocity setter 0xb7740 inlined with its last argument true, followed by the 0x1c4b00 call, the pair the file's two
// other 0x1c4b00 sites write with false. Written so (`function_b7740(arg_0, &local_11.field_c, NULL, true);` for the
// lookup and copy) 307 at 1004 bytes: 0xb7740 stays a call (its file is built /Ob1, so it is inlined nowhere) and the
// shared masked index loses its slot; BLOCKED-BY 0xb7740's file flags for that form. The copy through a copy of that
// file's forceinline write helper (object, linear, NULL) 41; lookup, both writes and the 0x1c4b00 call in one inline
// helper with a bool force formal 41 (same code), without the member store 93, with a long force formal 41. All keep
// the flag in AL for a `movzx`; the stack byte and dword push still need 0x1c4b00's bool formal (four of its
// declarations sit in other lanes' files this round). Body unchanged.
// M3 lane 4 r220 (fast with 0x119bc0 and 0x1c4b00, control 14): diagnostic, restored: 0x1c4b00's last parameter `bool` in
// its definition and all 14 declarations (seven of those files are other lanes' this round) 90 with the member store
// (1038 bytes), 40 without it (retail's stack byte and dword push; the copy still loads the object before the source's i,
// 0x10b010 19 in the reduced image). On the kept form: the result as `bool const` 14; a named pointer to the move
// output's velocity for the copy and the call 14; memcpy for the copy 14 (same code each); the result assigned straight
// to the member and the member passed as the force 43 (the older form). Still BLOCKED-BY 0x1c4b00 (declarations),
// 0x1ec690 (its draft is disabled) and 0x1e55d0 (another lane's file this round).
// M3 lane 4 r223 (fast with 0x119bc0 and 0x1c4b00, control 14): the velocity copy into the first object pointer with no
// second lookup 302 (1009 bytes); the force bool declared bare at the top of the block 14; the re-fetched object pointer
// declared bare there 14; a named destination pointer for the +0x88 copy 14 (same code each); the 0x119bc0 result
// compared `!= false` at its declaration 97 (1039 bytes). Still BLOCKED-BY 0x1c4b00 (its bool formal: declarations in
// other lanes' files this round) and 0x1ec690 / 0x1eb6f0 (their file is another lane's this round).
// M3 lane 1 r133 (fast with 0x119bc0, 0x1c4b00, 0x119020, 0x119c10 and 0x118e80, all matched each time; control 14):
// the member store placed after the +0x88 copy 11 (the byte store then sinks past the force push; a statement order, 3
// points, not kept); the flag local typed byte 14 (same code); the object's +0x88 as a typed vector member for the copy
// 14 (same code); the member store after the 0x1c4b00 call 96 (1041 bytes: the flag is kept in a stack byte across the
// call and read back twice); no member store 90 (re-measure). Still BLOCKED-BY 0x1c4b00's bool formal: three of its 14
// declarations sit in files other lanes hold this round (src/bipeds.cpp, src/items.cpp, src/unknown_0a76b0.cpp), and by
// 0x1ec690 (still a disabled draft behind its stub) for the copy's hoisted loads; lane 3 r258's held form (6) stands.
// M3 lane 11 r138 (fast with 0x119bc0 and 0x1c4b00, matched throughout; control 14): the 0x119bc0 result held in
// a `byte` 14, a `char` 14 (same code), stored to the member and the local in one chained assignment 14 (same code),
// passed as `local_force != false` 95 (1038 B), the object's +0x88 copy through a named vector pointer 14 (same code).
// Still BLOCKED-BY 0x1c4b00: retail has no member store; it keeps the result in its own stack byte and pushes the
// containing dword, which only a bool last parameter gives, and that definition's file and seven of its declaration
// files are other lanes' this round.
// MiniMax H (11-20), NOT kept at review (statement order is match-only; code is main's): moving the move output's byte member store after the +0x88 copy scores 11 (was 14), so the byte store sinks past the force push as retail does; the tail still differs where retail keeps the copy's destination in EAX and spills the 0x119bc0 result to a stack byte that it pushes as a dword, which needs 0x1c4b00's bool formal in src/unknown_1c25a0.cpp and in its other declarations.
// MiniMax B (11-20): no new form, no lower score (14): 0x1c4b00's bool formal is declared in another file, so the call's argument setup stays as it is; what is left is the one-row shift from the zero extension of the force argument and the 0x119bc0 result's own named bool.
// MiniMax C (11-20): no new form, no lower score (14): retail computes the object-table destination in EAX and spills
// and reloads the 0x119bc0 result at [esp+0x14]; this build keeps it in EDX, stores the flag at [esp+0x48] and pushes
// the zero-extended flag. BLOCKED-BY 0x1c4b00: its last parameter is a long while the call hands it the bool force,
// and that declaration lives in src/unknown_1c25a0.cpp, which this lane may not edit.
// M3 lane 1 r160 (H2M_FAST with 0x119bc0, 0x1c4b00, 0x119020, 0x119c10 and 0x118e80, all matched each run; control 14):
// one bool for the moving test and then the 0x119bc0 result (two definitions) with no member store 90 (the code of the
// plain no-store form: nothing is spilled), with the member store 14 (same code); the force started false and set under
// `if (function_119bc0(...))` 111 (1045 B); declared false at the top of the block and assigned from the call 90; a
// function-scope force started false and OR-ed with the call result 90. Reading: without the member store the body is
// one instruction short of retail from the 0x119bc0 call on (retail's byte spill and dword reload against one `movzx`),
// so the store's 14 is a row realignment and not retail's shape. Still BLOCKED-BY 0x1c4b00's bool formal (this round one
// of its declarations sits in src/unknown_1cec30.cpp, another lane's file, and one in a contributor's src/bipeds.cpp)
// and by 0x1ec690 (not yet written) for the copy's hoisted loads. Body unchanged.
// M3 lane 13 r150 (h2m fast with 0x1c4b00 and 0x11f0d0, both MATCH; control 14; 0x1c4b00's formal kept `long`): the object
// write and the 0x1c4b00 call through a file-local inline (object, linear, angular, force) helper, without the member store 92,
// with it 16 (first +0x273); the +0x88 copy component by component 106 (1050 B); the force bool declared at the top and
// assigned at the call 14; the copy through a `vector3f *` to the re-fetched object's +0x88 14 (both the same code).
// Still BLOCKED-BY 0x1c4b00 (round 272's bool form stays held: it raises contributor 0xe24f0).
// M3 lane 1 r162 (h2m fast with 0x1c4b00, its callers and 0x1197b0's callees; control 14). Diagnostics, all restored:
// 0x1c4b00's last parameter `bool` in its definition and all 14 declarations (none of those files held this round)
// re-measured 90 with the member store (1038 B; 0xe24f0 579 -> 601) and 40 without it; on the 40 form the 0x119bc0 result
// passed straight to the call 98, the member store after the call 100 (1043 B), the object looked up before the 0x119bc0
// call 40, the result `bool const` 40 (both the same code); `inline` on 0xb7740's definition with
// `function_b7740(arg_0, &local_11.field_c, NULL, true)` for the lookup and copy 279 (ten callers lost: it then expands
// everywhere). Still BLOCKED-BY 0x1c4b00 and 0xb7740.
// M3 lane 5 r243 (reduced image with 0x119bc0 and 0x1c4b00; control 14): the member set from the call and the local read
// back from it 97; the re-fetched object held as a pointer to its +0x88 vector 14; the member and the local set in one
// assignment 14 (same code); the member store after the copy with that vector pointer 11, with the re-fetched pointer
// declared bare ahead of the call 11 (round 133's statement-order lead, a gain of 3: the force is still widened by
// `movzx`). Reading: the member store into the move output blocks the copy's source loads (same local), so retail's
// hoisted loads need the store gone, and without it only the bool formal gives retail's byte spill. Still BLOCKED-BY
// 0x1c4b00 (one of its declarations is in src/unknown_1cec30.cpp, another lane's file this round, and contributor
// 0xe24f0 rises with the bool formal).
// M3 lane 1 r166 (h2m fast with 0x1197b0, 0x11f0d0, 0x1faf80, 0x109290 and 0x1c4b00; control 14): the +0x88 copy by
// memcpy 14; the re-fetched object pointer `*const` 14 (both the same code); the move output's byte member passed as
// the force 43 (1035 B); the object looked up ahead of the 0x119bc0 call 16; the force passed as `local_force ? 1 : 0`
// 95 (1038 B). Still BLOCKED-BY 0x1c4b00 (three of its declarations sit in other lanes' files this round).
// M3 lane 8 r236 (h2m fast with 0x119bc0, 0x1c4b00, 0x119020, 0x119c10, 0x118e80, 0x1197b0 and 0x11f0d0, all but
// 0x1197b0 matched each run; control 14; body unchanged). Lead, not kept (an anonymous overlay and a one-caller helper,
// both match-only; build/r236/lead-0x118700-union-helper.patch): the 0x119bc0 result stored into the flag member of a
// local bool/long overlay (two members at one address) and its long member passed to a file-local inline (object_index, linear,
// angular, long force) helper that does the object lookup, the `if (linear)` / `if (angular)` copies and the 0x1c4b00
// call: 6 at 1034 bytes with 0x1c4b00's formal left `long` and no other file touched (retail's own stack byte, the
// dword push and the copy's early i load; left are the copy's j and k loads, which retail runs ahead of the stores
// through the object: the move output's escape through 0x1e55d0). It equals round 258's held form without the bool
// formal, so 0x1c4b00's signature is not what blocks the last rows. Other forms: that overlay without the helper 40
// (re-measure); the helper with the member store kept and the member's dword read through a `long const *` cast as the
// force 29 (1035 B); that cast read without the helper 38; the flag member typed `long` and passed as the force 96
// (1038 B). Still BLOCKED-BY 0x1ec690 (not yet written) and 0x1eb6f0 for the copy's hoisted loads.
// M3 lane 2 r151 (fast screen; control 14): a one-use `vector3f` copy of the move output's velocity as the +0x88 store's
// source (the call still takes the member's address) 35 (all three source loads now run ahead of the object pointer
// and the force is widened into ECX); that copy without the member store 93 (first +0x73); that copy with the member
// store after it 36; the copy declared after the re-fetched object pointer 35; the force local typed `byte` 14 (same
// code). Reading: a forced-ranking search keeps two semantic rows in every forced decision (the force's byte spill and
// its dword push), so register choice alone cannot reach retail while 0x1c4b00's formal is `long`. Still BLOCKED-BY
// 0x1c4b00.
// M3 lane 8 r239 (h2m fast with 0x119bc0, 0x1c4b00, 0x119020, 0x119c10 and 0x118e80, all MATCH each run; control 14):
// the force bool declared bare at function scope and assigned at the call 14; the re-fetched object pointer declared bare
// at function scope 14; the re-fetched pointer declared bare ahead of the 0x119bc0 call and assigned after the member
// store 14 (the three the same code); the move output declared at function scope 19 (first +0x225, its homes move);
// the +0x88 copy member by member through a pointer to the object's vector 40 (1046 B). Still BLOCKED-BY 0x1c4b00 (its
// bool formal: one declaration sits in src/items.cpp, another lane's file this round, and contributor 0xe24f0 rises)
// and 0x1ec690 (a stub; its disabled draft costs two matches) for the copy's hoisted loads.
// M3 lane 4 r263 (h2m fast with 0x1c4b00, 0x119020 and 0xb77d0, all MATCH each run; control 14). Diagnostic, HELD
// (held as a patch, code back to main; two of 0x1c4b00's declarations sit in other lanes' files this
// round): 0x1c4b00's last parameter `bool` in its definition and all 13 declarations with the member store kept 90
// (1038 B); that without the member store 40 (retail's byte spill at its own cell and the dword push, the copy's i load
// not hoisted); with the lookup and copy as an inline helper as well 6 (only the copy's j and k loads, which retail runs
// one store ahead). On that 6: the helper written in 0xb7740's spelling (masked index, pool local, copy apart from the
// call) 6; with a dead update mask 6; the copy through memcpy 6; the angular store ahead of the linear 6; the copy member
// by member 101 (1051 B). Without the bool formal: the helper with the member store kept 41; with the bool formal, a
// named pointer to the move output's velocity for the copy and the call (no helper) 40. Still BLOCKED-BY 0x1c4b00 (its
// bool formal) and the copy's j/k schedule.
// M3 lane 15 r150 (h2m fast with 0x119bc0, 0x1c4b00, 0x119020 and 0x119c10, all MATCH each run; control 14; 0x1c4b00's
// formal kept `long`): the force held in a `long` (narrowed for the member store) 38; that with the member store after the
// +0x88 copy 34; a `dword` force passed as `(long)` 38; the copy by memcpy with the member store after it 11 (round 133's
// statement-order lead, the same code; a gain of 3, not kept); the member passed as the force (re-measured) 43 (1035 B).
// Still BLOCKED-BY 0x1c4b00 (its bool formal: four of its declaration files are other lanes' this round) and 0xb7740.
// M3 lane 4 r267 (fast screen with 0x119bc0, 0x1c4b00, 0x119020 and 0x119c10, all MATCH each run; control 14): the force
// flag `bool const` 14; the +0x88 copy by memcpy with the member store before it 14; the copy and the call through a named
// const pointer to the move output's velocity 14; the re-fetched object pointer declared bare before the 0x119bc0 call
// 14 (all four the same code); the force passed as `force ? 1 : 0` 95 (1038 B). Still BLOCKED-BY 0x1c4b00 (its bool
// formal; one of its declarations is in another lane's file this round).
// M3 lane 5 r255 (fast screen with 0x119bc0, 0x1c4b00, 0x119020, 0x119c10 and 0xb7740, all MATCH each run; control 14;
// the 14 rows all follow the 0x119bc0 call): the re-fetched object and the +0x88 copy as the existing 0xb7740 with its
// update skipped (`function_b7740(arg_0, &local_11.field_c, NULL, true)`, the same writes) 307 (1004 B: retail expands
// that block, ours calls it); the length test passed straight to the flag setter (no named bool) 14; the re-fetched
// object written into the copy's target expression (no pointer local) 14; the force declared bare ahead of the flag
// setter and assigned at the call 14; the masked object index named once and used by both lookups 14 (the four the
// same code). Still BLOCKED-BY 0x1c4b00 (its bool formal; a declaration sits in src/items.cpp, another lane's file this
// round) and 0x1ec690 (a stub) for the copy's hoisted j/k loads.
// M3 lane 15 r156 (h2m fast with 0x119bc0, 0x1c4b00, 0x119020 and 0x119c10, all MATCH each run; control 14): the force
// as `function_119bc0(arg_0) != false` 97 (1039 B); as `function_119bc0(arg_0) ? true : false` 97 (1039 B); the member
// store moved after the re-fetched object pointer, ahead of the +0x88 copy 40; the member store written
// `(byte)local_force` 14 and the force local typed `char` 14 (both the same code). Still BLOCKED-BY 0x1c4b00 (its bool
// formal; two of its declarations sit in other lanes' files this round).
// M3 lane 15 r157 (fast screen with 0x119bc0, 0x1c4b00, 0x119020 and 0x119c10, all MATCH each run; control 14): the member
// store moved after the 0x1c4b00 call 96 (1041 B); no member store with the force read through a `bool const &` 90; no
// member store with the force a `long` 91; the member set from the call and the local read back from it 97 (1039 B);
// the +0x88 copy through the existing vector3f_assign with the member store 40 (1046 B), without it 97 (1042 B);
// re-measured, the member passed as the force 43 (1035 B). Retail's force shares the dead definition pointer's cell
// (+0x14) and is pushed as a whole dword with no widening, which a `long` formal cannot give. Still BLOCKED-BY 0x1c4b00
// (its bool formal; three of its declarations sit in other lanes' files this round).
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
        bool local_13 = length_sq3f(&local_3.field_138) > 0.0f;
        forward_flags_set(&local_0->flags, 0x10, local_13);
        bool local_force = function_119bc0(arg_0);
        local_11.field_8 = local_force;
        s_forward_object *local_15 = ((s_forward_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
        *(vector3f *)((byte *)local_15 + 0x88) = local_11.field_c;
        function_1c4b00(arg_0, &local_11.field_c, NULL, local_force);
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
