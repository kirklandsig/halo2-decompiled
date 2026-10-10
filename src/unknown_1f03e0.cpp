// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F03E0.CPP: the state of a moving physics shape (0x74 bytes) and
   the side of a contact it touches */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include <math.h>
#include "object_list.h"
#include "unknown_1efac0.h"

struct s_table;
struct s_iterator
{
    s_table *table;
    signed char bytes[16];
    union
    {
        dword position;
        struct { byte first, item, sub, group; };
    };
    void *current;
    bool advance();
};

struct s_mix_output;
struct s_mix_source;
struct s_bsp3d;
struct s_1de2c1
{
    long field_0;
    long field_4[256];
};
struct s_1de2c2
{
    s_1de2c1 field_0;
    s_1de2c1 field_404;
    s_1de2c1 field_808;
    s_1de2c1 field_c0c;
};
struct s_245aa0;
struct s_source_245400;
struct s_collection_245270;
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices);
void function_141590(transform4x3f const *in, transform4x3f *out);
bool function_1dde10(s_bsp3d const *bsp, short count, dword const *mask,
    point3f const *point, real radius, s_1de2c2 *hits);
void function_245aa0(s_245aa0 const *hits, s_source_245400 const *source,
    transform4x3f const *matrix, real height, real thickness,
    long object_index, long position, s_collection_245270 *collection);

// @retail 0x1f0230
bool __stdcall function_1f0230(s_lookup *lookup, point3f const *point, real radius,
    real height, real thickness, s_collection_245270 *collection)
{
    bool result = false;
    s_iterator iterator;
    function_1efbd0((dword)lookup->tag_b, (s_mix_output *)&iterator,
        (s_mix_source *)lookup->tag_a, (signed char *)lookup->pointer_a);
    while (iterator.advance())
    {
        dword position = iterator.position;
        transform4x3f const *matrix = (transform4x3f const *)lookup->pointer_b + (position & 0xff);
        transform4x3f inverse;
        function_141590(matrix, &inverse);
        point3f scaled;
        scaled.x = point->x;
        scaled.y = point->y;
        scaled.z = point->z;
        if (inverse.scale != 1.0f)
        {
            scaled.x *= inverse.scale;
            scaled.y *= inverse.scale;
            scaled.z *= inverse.scale;
        }
        point3f local;
        local.x = inverse.up.i * scaled.z + inverse.left.i * scaled.y + inverse.forward.i * scaled.x + inverse.position.x;
        local.y = inverse.up.j * scaled.z + inverse.left.j * scaled.y + inverse.forward.j * scaled.x + inverse.position.y;
        local.z = inverse.up.k * scaled.z + inverse.left.k * scaled.y + inverse.forward.k * scaled.x + inverse.position.z;
        s_1de2c2 hits;
        if (function_1dde10((s_bsp3d *)iterator.current, 0, NULL, &local, inverse.scale * radius, &hits))
        {
            function_245aa0((s_245aa0 *)&hits, (s_source_245400 *)iterator.current,
                matrix, height, thickness, lookup->handle, position, collection);
            result = true;
        }
    }
    return result;
}

#define PIN(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

struct s_shape_state
{
	point3f point;
	long unknown0c;
	byte unknown10[0x1c - 0x10];
	long unknown1c;
	long unknown20;
	transform4x3f matrix;
	long unknown58;
	long unknown5c;
	short material;
	bool unknown62;
	byte unknown63;
	vector3f normal;
	real unknown70;
};

transform4x3f *g_4687d0;
extern short g_47d8e0;
extern short g_54e898;

// @retail 0x1f03e0
void function_1f03e0(s_shape_state *state)
{
	*(volatile long *)&state->unknown0c = NONE;
	state->point = *(point3f *)g_4687a4;
	*(volatile bool *)&state->unknown62 = false;
	state->normal = *g_4687b0;
	*(volatile short *)&state->material = g_47d8e0;
	*(volatile long *)&state->unknown58 = NONE;
	*(volatile long *)&state->unknown5c = NONE;
	*(volatile long *)&state->unknown1c = NONE;
	*(volatile long *)&state->unknown20 = NONE;
	*(volatile real *)&state->unknown70 = 0.0f;
	state->matrix = *g_4687d0;
}

/* the contact at a surface and the side it faces */
struct s_shape_contact
{
	byte unknown00[0xdc];
	vector3f normal;
};

struct s_shape_side
{
	long unknown0;
	long side;
};

// @retail 0x1f1930
void function_1f1930(s_shape_contact const *contact, s_shape_side *side)
{
	if (contact->normal.i != 0.0f || contact->normal.j != 0.0f || contact->normal.k != 0.0f)
	{
		if (fabs(contact->normal.i) < fabs(contact->normal.j))
		{
			if (contact->normal.j < 0.0f)
				side->side = 2;
			else
				side->side = 3;
		}
		else
		{
			if (contact->normal.i < 0.0f)
				side->side = 5;
			else
				side->side = 4;
		}
	}
}

/* the havok components (g_51e9b8, 0xa0 bytes) and their materials */
struct s_component_material
{
	byte unknown00[0x12];
	short material;
	byte unknown14[0x48 - 0x14];
};

struct s_component
{
	byte unknown00[0x88];
	s_component_material *materials;
	byte unknown8c[0xa0 - 0x8c];
};

// @retail 0x1f1df0
void function_1f1df0(long component_index, long material_index, vector3f const *normal, s_shape_state *state)
{
	union { short field_0; word field_2; } local_0;
	local_0.field_0 = 0;
	s_component *component = (s_component *)(g_51e9b8->data + (component_index & 0xffff) * sizeof(s_component));

	state->normal = *normal;
	if (material_index != NONE)
		local_0.field_0 = component->materials[material_index].material;
	else
		local_0.field_0 = g_54e898;
	state->material = (short)local_0.field_2;
}

struct s_shape_carrier_contact
{
	byte unknown00[0x10];
	long object_index;
	byte unknown14[0x58 - 0x14];
	transform4x3f transform;
	byte unknown8c[0xe8 - 0x8c];
	point3f point;
};

struct s_shape_carrier_state
{
	vector3f previous_velocity;
	byte unknown0c[0x24 - 0xc];
	transform4x3f matrix;
};

bool function_182020(long component_index, vector3f *previous_velocity, point3f const *point,
	transform4x3f const *transform, transform4x3f *matrix, vector3f *velocity,
	vector3f *delta_velocity, matrix3x3 *rotation);

// @retail 0x1f1e50
bool function_1f1e50(s_shape_carrier_state *state, s_shape_carrier_contact const *contact,
	vector3f *velocity, vector3f *delta_velocity, matrix3x3 *rotation)
{
	bool result = false;
	long object_index = contact->object_index;
	if (object_index != NONE)
	{
		s_object_list_state *objects = g_5107f0;
		for (long i = 0; i < objects->object_count; i++)
		{
			if (objects->object_indices[i] == object_index)
			{
				struct s_carrier_header { byte unknown00[8]; byte *object; };
				byte *object = ((s_carrier_header *)g_4e0300->data)[object_index & 0xffff].object;
				result = function_182020(*(long *)(object + 0xb4), &state->previous_velocity,
					&contact->point, &contact->transform, &state->matrix, velocity, delta_velocity, rotation);
				break;
			}
		}
	}
	return result;
}

/* the surfaces' minimum and maximum heights: whether the shape stands at a
   height it can step to */
struct s_shape_ground
{
	byte unknown00[0x1c];
	point3f point;
	byte unknown28[0x34 - 0x28];
	real height;
};

// @retail 0x1f2e60
bool function_1f2e60(bool moving, s_shape_ground const *ground, vector3f const *velocity, bool stepping, point3f const *base, real height)
{
	real top = base->z + height;

	if (top - 0.001f > base->z)
	{
		point3f point = ground->point;
		point3f center;
		vector3f v;
		real distance;
		real lower, upper;

		if (moving)
		{
			point.x = velocity->i * g_510c54->rate + point.x;
			point.y = velocity->j * g_510c54->rate + point.y;
			point.z = velocity->k * g_510c54->rate + point.z;
		}
		center.x = base->x;
		center.y = base->y;
		center.z = height + base->z;
		vector3d_from_points3d(&center, &point, &v);
		distance = (real)sqrt(v.k * v.k + v.j * v.j + v.i * v.i) - height;
		lower = moving ? -0.25f : -0.1f;
		upper = stepping ? 0.1f : 0.2f;
		return PIN(distance, lower, upper) == distance;
	}

	return PIN(ground->height, -0.1f, 0.0328f) == ground->height;
}

struct s_havok_component;
void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, transform4x3f *matrix);
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
real function_f5e00(long vehicle_index);
extern real g_47f05c;

// @retail 0x1f1ee0
bool function_1f1ee0(s_shape_carrier_contact const *contact, s_shape_state *state,
    vector3f *velocity, matrix3x3 *rotation)
{
    byte const *data = (byte const *)contact;
    byte *shape = *(byte **)(data + 8);
    byte *components = g_51e9b8->data;
    byte *component = components + (*(long *)(data + 0x18) & 0xffff) * 0xa0;
    long count = *(long *)(component + 0x8c);
    bool result = false;
    long best = NONE;
    real best_dot = -3.402823466e38f;
    real height = *(real *)(shape + 0xc);
    if (count > 0)
    {
        byte *contacts = *(byte **)(component + 0x88);
        for (long i = 0; i < count; i++)
        {
            byte *entry = contacts + i * 0x48;
            if (contact->point.z + height - 0.001f > *(real *)(entry + 0x24))
            {
                bool stepping = ((*(dword *)(data + 0x14) >> 2) & 1) != 0;
                bool valid;
                if (contact->point.z + height - 0.001f > contact->point.z)
                {
                    point3f point = *(point3f *)(entry + 0x1c);
                    vector3f delta;
                    delta.i = point.x - contact->point.x;
                    delta.j = point.y - contact->point.y;
                    delta.k = point.z - (contact->point.z + height);
                    real distance = (real)sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i) - height;
                    real upper = stepping ? 0.1f : 0.2f;
                    valid = PIN(distance, -0.1f, upper) == distance;
                }
                else
                {
                    real distance = *(real *)(entry + 0x34);
                    valid = PIN(distance, -0.1f, g_47f05c) == distance;
                }
                if (valid)
                {
                    vector3f *normal = (vector3f *)(entry + 0x28);
                    vector3f const *up = (vector3f const *)(data + 0x8c);
                    real dot = normal->k * up->k + normal->i * up->i + up->j * normal->j;
                    if (dot > 0.8660253882408142f && dot > best_dot)
                    {
                        best_dot = dot;
                        best = i;
                    }
                }
            }
        }
        if (best != NONE)
        {
            byte *entry = contacts + best * 0x48;
            long component_index = *(long *)(entry + 0x14);
            if (component_index != NONE)
            {
                s_havok_component *other = (s_havok_component *)(components + (component_index & 0xffff) * 0xa0);
                transform4x3f matrix;
                long body_index = *(signed char *)(entry + 0x46);
                havok_component_rigid_body_matrix_get(body_index, other, &matrix);
                if (component_index == *(long *)(data + 0x54))
                {
                    long object_index = *(long *)(entry + 0x18);
                    struct s_contact_object_header { byte unknown0[3]; byte type; byte unknown4[8]; };
                    s_contact_object_header *headers = (s_contact_object_header *)g_4e0300->data;
                    if (object_index == NONE || !( (1 << headers[object_index & 0xffff].type) & 2) || function_f5e00(object_index) != 1.0f)
                    {
                        transform4x3f inverse;
                        transform4x3f relative;
                        function_141590(&matrix, &inverse);
                        function_142a60(&contact->transform, &inverse, &relative);
                        havok_component_rigid_body_point_velocity_get(body_index, other, &contact->point, velocity);
                        function_141590(&relative, &relative);
                        *rotation = relative.rotation;
                        result = true;
                    }
                }
                state->matrix = matrix;
                state->unknown1c = component_index;
                state->unknown5c = *(long *)(entry + 0x18);
                state->unknown20 = body_index;
            }
        }
    }
    return result;
}

real function_30bf0(vector3f *v);

PRIVATE inline vector3f contact_cross(vector3f const &a, vector3f const &b)
{
    vector3f result;
    result.i = a.j * b.k - a.k * b.j;
    result.j = a.k * b.i - a.i * b.k;
    result.k = a.i * b.j - a.j * b.i;
    return result;
}

// @retail 0x1f2a80
void function_1f2a80(vector3f const *velocity, byte const *request, bool moving,
    vector3f const *desired, long const *indices, long count, vector3f *out)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    long selected = NONE;
    real lowest = 0.0f;
    for (long i = 0; i < count; i++)
    {
        byte *entry = *(byte **)(component + 0x88) + indices[i] * 0x48;
        real z = *(real *)(entry + 0x30);
        if (lowest > z && z > -0.999f && function_1f2e60(moving, (s_shape_ground *)entry,
            velocity, ((*(dword *)(request + 0x18) >> 1) & 1) != 0,
            (point3f const *)(request + 0x1c), *(real *)(*(byte *const *)request + 0xc)))
        {
            lowest = z;
            selected = i;
        }
    }
    *out = *g_4687b0;
    byte *contacts = *(byte **)(component + 0x88);
    switch (count)
    {
    case 1:
        *out = *(vector3f *)(contacts + indices[0] * 0x48 + 0x28);
        break;
    case 2:
        {
            vector3f const *a = (vector3f *)(contacts + indices[0] * 0x48 + 0x28);
            vector3f const *b = (vector3f *)(contacts + indices[1] * 0x48 + 0x28);
            if (selected != NONE)
            {
                vector3f axis = contact_cross(*a, *b);
                vector3f const *normal = (vector3f *)(contacts + indices[selected] * 0x48 + 0x28);
                if (selected == 0)
                    *out = contact_cross(axis, *normal);
                else
                    *out = contact_cross(*normal, axis);
            }
            else
            {
                vector3f average;
                average.i = b->i + a->i;
                average.j = b->j + a->j;
                average.k = b->k + a->k;
                function_30bf0(&average);
                real dot_a = a->i * desired->i + desired->k * a->k + a->j * desired->j;
                real dot_average = desired->i * average.i + desired->k * average.k + desired->j * average.j;
                *out = dot_average > dot_a ? *a : average;
            }
        }
        break;
    case 3:
        if (selected != NONE)
        {
            vector3f const *normal = (vector3f *)(contacts + indices[selected] * 0x48 + 0x28);
            real scale = -normal->k;
            out->i = normal->i * scale + g_4687b0->i;
            out->j = normal->j * scale + g_4687b0->j;
            out->k = scale * normal->k + g_4687b0->k;
        }
        else
            *out = *g_4687b0;
        break;
    }
    real length = (real)sqrt(out->i * out->i + out->j * out->j + out->k * out->k);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        out->i *= inverse;
        out->j = inverse * out->j;
        out->k *= inverse;
    }
    else
        *out = *g_4687b0;
}

PRIVATE __forceinline void function_1f1461(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
	arg_2->j = arg_0->k * arg_1->i - arg_0->i * arg_1->k;
	arg_2->k = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
}

PRIVATE __forceinline byte function_1f1462(dword arg_0)
{
	return (arg_0 >> 3) & 1;
}

// @retail 0x1f1460
void function_1f1460(byte const *state, s_shape_state const *ground, vector3f *arg_9650d9, vector3f *arg_3a7661)
{
    struct s_1f1466 { vector3f *field_0; byte const *field_4; } local_0;
    local_0.field_0 = arg_3a7661;
    local_0.field_4 = state;
    byte *settings = *(byte **)(local_0.field_4 + 8);
    if (function_1f1462(*(dword *)settings))
    {
        vector3f const *old_up = (vector3f const *)(local_0.field_4 + 0x100);
        vector3f up = ground->normal;
        vector3f axis;
        function_1f1461(old_up, &up, &axis);
        bool rotate = true;
        if (function_30bf0(&axis) == 0.0f)
        {
            real dot = old_up->k * up.k + old_up->j * up.j + old_up->i * up.i;
            if (dot > 0.0f)
                rotate = false;
            else
                axis = *old_up;
        }
        if (rotate)
        {
            vector3f limited = *old_up;
            real angle = g_510c54->rate * 5.235987663269043f;
            real sine = (real)sin(angle);
            real cosine = (real)cos(angle);
            real dot = (limited.k * axis.k + limited.j * axis.j + limited.i * axis.i) * (1.0f - cosine);
            vector3f cross;
            function_1f1461(&limited, &axis, &cross);
            limited.k = axis.k * dot + limited.k * cosine - cross.k * sine;
            limited.i = dot * axis.i + limited.i * cosine - cross.i * sine;
            limited.j = axis.j * dot + limited.j * cosine - cross.j * sine;
            function_1f1461(&limited, &up, &cross);
            if (cross.k * axis.k + cross.j * axis.j + cross.i * axis.i > 0.0f)
                up = limited;
        }
        vector3f const *old_forward = (vector3f const *)(local_0.field_4 + 0xf4);
        vector3f cross;
        function_1f1461(old_forward, &up, &cross);
        vector3f forward;
        function_1f1461(&up, &cross, &forward);
        if (function_30bf0(&forward) == 0.0f)
        {
            function_1f1461(&up, old_up, &cross);
            function_1f1461(&up, &cross, &forward);
            if (function_30bf0(&forward) == 0.0f)
            {
                up = *g_4687b0;
                forward = *g_4687a8;
            }
        }
        *arg_9650d9 = forward;
        *local_0.field_0 = up;
    }
    else
    {
        *arg_9650d9 = *(vector3f const *)(local_0.field_4 + 0xf4);
        *local_0.field_0 = *g_4687b0;
        arg_9650d9->k = 0.0f;
        if (function_30bf0(arg_9650d9) == 0.0f)
            *arg_9650d9 = *g_4687a8;
    }
}

struct s_unknown_1eb550;
extern s_unknown_1eb550 *g_51e9c4;
real g_47ffb0 = 0.5f;
real normalize2d(point2f *v);
bool __stdcall function_a75d0(vector3f *vector, real maximum);

PRIVATE inline vector3f shape_add(vector3f const &a, vector3f const &b)
{
    vector3f r; r.i = a.i + b.i; r.j = a.j + b.j; r.k = a.k + b.k; return r;
}
PRIVATE inline vector3f shape_subtract(vector3f const &a, vector3f const &b)
{
    vector3f r; r.i = a.i - b.i; r.j = a.j - b.j; r.k = a.k - b.k; return r;
}
PRIVATE inline vector3f shape_scale(vector3f const &a, real scale)
{
    vector3f r; r.i = a.i * scale; r.j = a.j * scale; r.k = a.k * scale; return r;
}
PRIVATE inline real shape_dot(vector3f const &a, vector3f const &b)
{
    return a.k * b.k + a.j * b.j + a.i * b.i;
}

#pragma optimize("s", on)
PRIVATE __forceinline vector3f function_1f0873(vector3f const &arg_0, vector3f const &arg_1)
{
    vector3f local_0;
    local_0.i = arg_0.j * arg_1.k - arg_0.k * arg_1.j;
    local_0.j = arg_0.k * arg_1.i - arg_0.i * arg_1.k;
    local_0.k = arg_0.i * arg_1.j - arg_0.j * arg_1.i;
    return local_0;
}

// @retail 0x1f0870
void function_1f0870(byte const *state, byte *out, byte *history, vector3f const *current)
{
    byte *settings = *(byte **)(state + 8);
    dword flags = *(dword *)(state + 0x14);
    bool flying = ((*(dword *)settings >> 3) & 1) != 0;
    bool gravity = ((flags >> 3) & 1) != 0;
    bool damping = ((flags >> 4) & 1) != 0;
    vector3f const &input = *(vector3f const *)(state + 0x138);
    vector3f const &facing = *(vector3f const *)(state + 0x9c);
    vector3f const &up = *(vector3f const *)(state + 0x100);
    vector3f const &normal = *(vector3f const *)(state + 0x8c);
    vector3f const &basis = *(vector3f const *)(state + 0x118);
    vector3f *velocity = (vector3f *)(out + 0xc);
    real scale = *(real *)(state + 0x20);
    if (!(flags & 1))
    {
        vector3f left;
        left.i = facing.k * up.j - up.k * facing.j;
        left.j = up.k * facing.i - up.i * facing.k;
        left.k = up.i * facing.j - up.j * facing.i;
        real local_0 = input.i * scale;
        real local_1 = input.j * scale;
        real local_2 = input.k * scale;
        velocity->i = facing.i * local_0;
        velocity->j = facing.j * local_0;
        velocity->k = facing.k * local_0;
        velocity->i += left.i * local_1;
        velocity->j += left.j * local_1;
        velocity->k += left.k * local_1;
        velocity->i += up.i * local_2;
        velocity->j += up.j * local_2;
        velocity->k += up.k * local_2;
        gravity = false;
    }
    else if (flags & 4)
    {
        vector3f delta;
        delta.i = scale * (facing.i * input.i - facing.j * input.j) - current->i;
        delta.j = scale * (facing.j * input.i + facing.i * input.j) - current->j;
        point2f limited = { delta.i, delta.j };
        real step = *(real *)(state + 0x148) * g_510c54->rate;
        if (normalize2d(&limited) > step)
        {
            limited.x *= step;
            limited.y *= step;
        }
        else
        {
            limited.x = delta.i;
            limited.y = delta.j;
        }
        velocity->i = current->i + limited.x;
        velocity->j = current->j + limited.y;
        velocity->k = current->k - *(real *)(state + 0x130) * g_510c54->rate;
    }
    else
    {
        real magnitude = (real)sqrt(input.i * input.i + input.j * input.j + input.k * input.k);
        vector3f desired;
        if (flying)
        {
            vector3f left = function_1f0873(normal, basis);
            if (function_30bf0(&left) == 0.0f)
            {
                left = function_1f0873(normal, *g_4687b0);
                if (function_30bf0(&left) == 0.0f)
                {
                    left = function_1f0873(normal, *g_4687a8);
                    function_30bf0(&left);
                }
            }
            vector3f forward = function_1f0873(left, normal);
            function_30bf0(&forward);
            desired.i = left.i * input.j + forward.i * input.i;
            desired.j = left.j * input.j + forward.j * input.i;
            desired.k = left.k * input.j + forward.k * input.i;
            desired.k += input.k;
            function_30bf0(&desired);
        }
        else
        {
            if (normal.k > 0.001f)
            {
                desired.i = facing.i * input.i - facing.j * input.j;
                desired.j = facing.j * input.i + facing.i * input.j;
                desired.k = input.k - (normal.j * desired.j + normal.i * desired.i) / normal.k;
            }
            else
            {
                vector3f left = function_1f0873(*g_4687b0, basis);
                function_30bf0(&left);
                real local_3 = -shape_dot(normal, basis);
                vector3f forward;
                forward.i = basis.i + normal.i * local_3;
                forward.j = basis.j + normal.j * local_3;
                forward.k = basis.k + normal.k * local_3;
                real local_4 = -shape_dot(normal, left);
                left.i += normal.i * local_4;
                left.j += normal.j * local_4;
                left.k += normal.k * local_4;
                desired.i = left.i * input.j + forward.i * input.i;
            desired.j = left.j * input.j + forward.j * input.i;
            desired.k = left.k * input.j + forward.k * input.i;
                desired.k = (desired.k + input.k) * 5.0f;
            }
            function_30bf0(&desired);
            if (*(real *)(settings + 0x58) >= desired.k)
                magnitude *= *(real *)(settings + 0x4c);
            else if (*(real *)(settings + 0x58) > desired.k)
                magnitude *= 1.0f + (*(real *)(settings + 0x4c) - 1.0f) * (desired.k - *(real *)(settings + 0x58)) /
                    (*(real *)(settings + 0x5c) - *(real *)(settings + 0x58));
            else if (desired.k >= *(real *)(settings + 0x64))
                magnitude *= *(real *)(settings + 0x50);
            else if (desired.k > *(real *)(settings + 0x60))
                magnitude *= 1.0f + (desired.k - *(real *)(settings + 0x60)) * (*(real *)(settings + 0x50) - 1.0f) /
                    (*(real *)(settings + 0x64) - *(real *)(settings + 0x60));
        }
        vector3f delta = shape_subtract(shape_scale(desired, scale * magnitude), *current);
        vector3f limited = delta;
        real step = *(real *)(state + 0x144) * g_510c54->rate;
        if (function_30bf0(&limited) > step)
        {
            limited = shape_scale(limited, step);
            if (flying)
                gravity = false;
        }
        else
        {
            limited = delta;
            gravity = false;
        }
        if (!state[0xa8] && *(long *)((byte *)g_51e9c4 + 0x18) - g_510c54->game_time <= 0)
            limited = shape_subtract(limited, shape_scale(normal, 0.234f));
        if (damping)
        {
            real fraction = 1.0f - *(real *)(state + 0x98);
            limited.i *= fraction;
            limited.j *= fraction;
        }
        if (gravity)
            limited.k -= *(real *)(state + 0x130) * g_510c54->rate;
        *velocity = shape_add(*current, limited);
    }
    if (!((*(dword *)settings >> 2) & 1) && !(flags & 4))
    {
        if (*(long *)(history + 0xc) != NONE && *(long *)(history + 0xc) == g_510c54->game_time - 1)
        {
            vector3f change = shape_add(shape_subtract(input, *(vector3f *)(history + 0x10)), *(vector3f *)(out + 0x64));
            real maximum = (real)sqrt(change.k * change.k + change.j * change.j + change.i * change.i) + g_47ffb0;
            vector3f offset = shape_subtract(*(vector3f const *)(state + 0x124), *(vector3f *)(out + 0x64));
            vector3f relative = shape_subtract(*velocity, offset);
            function_a75d0(&relative, maximum);
            *velocity = shape_add(relative, offset);
        }
        *(vector3f *)(history + 0x10) = input;
        *(long *)(history + 0xc) = g_510c54->game_time;
    }
    if (gravity)
        *(dword *)out |= 1;
    else
        *(dword *)out &= ~1;
}
#pragma optimize("", on)


PRIVATE inline void shape_rotate_vector(matrix3x3 const *matrix, vector3f const *input, vector3f *out)
{
    vector3f copy;
    if (input == out)
    {
        copy = *input;
        input = &copy;
    }
    out->i = input->j * matrix->left.i + input->k * matrix->up.i + input->i * matrix->forward.i;
    out->j = input->i * matrix->forward.j + input->j * matrix->left.j + input->k * matrix->up.j;
    out->k = input->i * matrix->forward.k + input->j * matrix->left.k + input->k * matrix->up.k;
}

// @retail 0x1f0450
void __stdcall function_1f0450(s_shape_state *history, byte *out, s_shape_carrier_contact const *contact)
{
    byte const *data = (byte const *)contact;
    vector3f current = *(vector3f *)(data + 0x124);
    byte *component = g_51e9b8->data + (*(long *)(data + 0x18) & 0xffff) * 0xa0;
    history->unknown1c = NONE;
    history->unknown5c = NONE;
    history->unknown20 = NONE;
    matrix3x3 rotation = *(matrix3x3 *)((byte *)g_4687d0 + 4);
    vector3f velocity = *g_4687a4;
    vector3f carrier_velocity, delta_velocity;
    bool override_velocity = *(*(byte **)(component + 0x70) + 0x44) != 0;
    bool carrier = function_1f1e50((s_shape_carrier_state *)history, contact, &carrier_velocity, &delta_velocity, &rotation);
    bool support;
    if (!carrier && function_1f1ee0(contact, history, &velocity, &rotation))
        support = true;
    else
    {
        support = false;
        if (carrier)
        {
            velocity = carrier_velocity;
            *(dword *)out |= 8;
            *(matrix3x3 *)(out + 0x30) = rotation;
        }
    }
    if (override_velocity)
    {
        *(vector3f *)(out + 0x64) = *(vector3f *)(*(byte **)(component + 0x70) + 0x30);
        current.i -= *(real *)(out + 0x64);
        current.j -= *(real *)(out + 0x68);
        current.k -= *(real *)(out + 0x6c);
    }
    else if (support || carrier)
    {
        current.i -= velocity.i;
        current.j -= velocity.j;
        current.k -= velocity.k;
        *(vector3f *)(out + 0x64) = velocity;
        *(matrix3x3 *)(out + 0x30) = rotation;
        if (carrier && *(byte *)((byte *)history + 0x62) > 1)
        {
            current.i += delta_velocity.i;
            current.j += delta_velocity.j;
            current.k += delta_velocity.k;
        }
    }
    function_1f0870(data, out, (byte *)history, &current);
    if (override_velocity)
    {
        *(real *)(out + 0xc) += *(real *)(out + 0x64);
        *(real *)(out + 0x10) += *(real *)(out + 0x68);
        *(real *)(out + 0x14) += *(real *)(out + 0x6c);
    }
    else if (support || carrier)
    {
        *(real *)(out + 0xc) += velocity.i;
        *(real *)(out + 0x10) += velocity.j;
        *(real *)(out + 0x14) += velocity.k;
        shape_rotate_vector(&rotation, (vector3f *)(out + 0x18), (vector3f *)(out + 0x18));
        shape_rotate_vector(&rotation, (vector3f *)(out + 0x24), (vector3f *)(out + 0x24));
    }
    long count = *(byte *)((byte *)history + 0x62) + 1;
    if (count > 0xfe) count = 0xfe;
    *(byte *)((byte *)history + 0x62) = (byte)count;
}


extern vector3f *g_4687bc;
bool function_109fd0(long object_index, vector3f *velocity);

PRIVATE inline real contact_normalize(vector3f *vector)
{
    real length = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabs(length) < 0.0001f)
        return 0.0f;
    real inverse = 1.0f / length;
    vector->i *= inverse;
    vector->j *= inverse;
    vector->k *= inverse;
    return length;
}

// @retail 0x1f2250
bool function_1f2250(vector3f const *velocity, byte const *request, vector3f const *input,
    bool moving, vector3f *out, long *surface, real *speed, long *object_index)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    byte *settings = *(byte **)request;
    vector3f relative = *input;
    *speed = 0.0f;
    *out = *g_4687b0;
    *surface = NONE;
    *object_index = NONE;
    long count = 0;
    long indices[3];
    if (moving)
    {
        relative.i -= velocity->i;
        relative.j -= velocity->j;
        relative.k -= velocity->k;
    }
    else if (*(long *)(request + 4) != NONE)
    {
        vector3f object_velocity;
        if (function_109fd0(*(long *)(request + 4), &object_velocity))
        {
            relative.i -= object_velocity.i;
            relative.j -= object_velocity.j;
            relative.k -= object_velocity.k;
        }
    }
    vector3f desired = relative;
    bool stepping = ((*(dword *)(request + 0x18) >> 1) & 1) != 0;
    if (*(long *)(request + 0x44) != NONE)
    {
        for (long i = 0; i < *(long *)(component + 0x8c); ++i)
        {
            byte *contact = *(byte **)(component + 0x88) + i * 0x48;
            if (*(long *)(contact + 0x14) == *(long *)(request + 0x44) &&
                *(real *)(contact + 0x30) > *(real *)(settings + 0x54) &&
                function_1f2e60(moving, (s_shape_ground *)contact, velocity, stepping,
                    (point3f const *)(request + 0x1c), *(real *)(settings + 0xc)))
            {
                desired = shape_scale(*(vector3f *)(contact + 0x28), -1.0f);
                break;
            }
        }
    }
    if ((*(dword *)settings >> 3) & 1)
    {
        if (stepping)
            desired = shape_subtract(desired, *(vector3f *)((byte *)velocity + 0x64));
    }
    else
    {
        if (!stepping)
        {
            if (*(long *)(request + 8) == *(long *)(request + 4))
                desired = *g_4687bc;
            else if (desired.k < 0.0f)
                desired.k *= 30.0f;
        }
        if (stepping)
            desired = shape_subtract(desired, *(vector3f *)((byte *)velocity + 0x64));
        else if (length_sq3f((vector3f const *)(request + 0x34)) == 0.0f)
            desired = *g_4687bc;
    }
    if (contact_normalize(&desired) != 0.0f)
    {
        vector3f direction = desired;
        for (long iteration = 0; iteration < 3; ++iteration)
        {
            real minimum = 3.4028234663852886e+38f;
            long selected = NONE;
            long contacts = *(long *)(component + 0x8c);
            for (long i = 0; i < contacts; ++i)
            {
                byte *contact = *(byte **)(component + 0x88) + i * 0x48;
                if (function_1f2e60(moving, (s_shape_ground *)contact, velocity, stepping,
                    (point3f const *)(request + 0x1c), *(real *)(settings + 0xc)) &&
                    (!stepping || *(long *)(request + 0x44) == NONE || *(long *)(request + 0x44) == *(long *)(contact + 0x14)))
                {
                    real dot = dot3f(&direction, (vector3f *)(contact + 0x28));
                    if (dot < -0.025f && dot < minimum)
                    {
                        minimum = dot;
                        selected = i;
                    }
                }
            }
            if (selected == NONE)
            {
                if (count || !((*(dword *)settings >> 3) & 1) || !contacts)
                    break;
                desired = shape_scale(*(vector3f *)(*(byte **)(component + 0x88) + 0x28), -1.0f);
                direction = desired;
                minimum = -1.0f;
                selected = 0;
            }
            *surface = selected;
            indices[count++] = selected;
            byte *contact = *(byte **)(component + 0x88) + selected * 0x48;
            if (count == 1)
            {
                direction.i = desired.i - *(real *)(contact + 0x28) * minimum;
                direction.j = desired.j - *(real *)(contact + 0x2c) * minimum;
                direction.k = desired.k - *(real *)(contact + 0x30) * minimum;
            }
            else if (count == 2)
            {
                byte *contacts = *(byte **)(component + 0x88);
                vector3f tangent = contact_cross(*(vector3f *)(contacts + indices[0] * 0x48 + 0x28),
                    *(vector3f *)(contacts + indices[1] * 0x48 + 0x28));
                contact_normalize(&tangent);
                direction = shape_scale(tangent, dot3f(&tangent, &desired));
            }
        }
        function_1f2a80(velocity, request, moving, &desired, indices, count, out);
    }
    real height = *(real *)(request + 0x24) + *(real *)(settings + 0xc);
    for (long i = 0; i < *(long *)(component + 0x8c); ++i)
    {
        byte *contact = *(byte **)(component + 0x88) + i * 0x48;
        if (function_1f2e60(moving, (s_shape_ground *)contact, velocity, stepping,
            (point3f const *)(request + 0x1c), *(real *)(settings + 0xc)))
        {
            real projection = 0.0f - shape_dot(*(vector3f *)(contact + 0x28), relative);
            if (stepping && height - 0.001f > *(real *)(contact + 0x24) && projection > *speed)
                *speed = projection;
        }
        long object = *(long *)(contact + 0x18);
        if (object != NONE && (*object_index == NONE || ((1 << g_4e0300->data[(object & 0xffff) * 12 + 3]) & 1)))
            *object_index = object;
    }
    return count && out->k > *(real *)(settings + 0x54);
}


real function_1201a0(vector3f *vector, vector3f const *fallback);
bool havok_component_any_rigid_body_active(s_havok_component *component);

// @retail 0x1f19c0
void function_1f19c0(byte const *request, s_shape_state *history, byte *out)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    dword flags = *(dword *)(request + 0x18);
    bool preserve = (flags & 4) || ((flags & 1) && !(flags & 2)) || (flags & 0x18);
    bool moving = sqrt(history->point.x * history->point.x + history->point.y * history->point.y + history->point.z * history->point.z) != 0.0f;
    *(dword *)(out + 4) &= ~1;
    if (!preserve)
    {
        vector3f const *velocity = (vector3f const *)(*(byte **)(component + 0x70) + 0xc);
        history->normal = *g_4687b0;
        history->material = g_47d8e0;
        history->unknown58 = NONE;
        history->unknown70 = 0.0f;
        bool flying = ((**(dword **)request >> 3) & 1) != 0;
        long selected = NONE;
        long highest = NONE;
        real best_height = -3.4028234663852886e+38f;
        real best_speed = -3.4028234663852886e+38f;
        real highest_z = -3.4028234663852886e+38f;
        bool best_supported = false;
        bool best_flag = false;
        byte *contacts = *(byte **)(component + 0x88);
        for (long i = 0; i < *(long *)(component + 0x8c); ++i)
        {
            byte *contact = contacts + i * 0x48;
            vector3f *normal = (vector3f *)(contact + 0x28);
            real speed = 0.0f - shape_dot(*normal, *velocity);
            real approach = 0.0f - shape_dot(*normal, *(vector3f *)(request + 0x28)) * *(real *)(request + 0x34);
            bool flag = ((*(byte *)(contact + 0x44) >> 1) & 1) != 0;
            bool supported = (flying || (*(byte *)(contact + 0x44) & 4)) && approach >= -0.001f;
            if (normal->k > highest_z)
            {
                highest = i;
                highest_z = normal->k;
            }
            if (*(long *)(contact + 0x14) != NONE || speed > 0.0f)
            {
                bool accept;
                if (supported)
                {
                    accept = (flying || normal->j * velocity->j + normal->i * velocity->i <= 0.5f) &&
                        (!best_supported || speed > best_speed) && !(fabs(*(real *)(contact + 0x38)) < 0.0001f);
                }
                else if (flag)
                    accept = !best_flag || normal->k > best_height;
                else
                    accept = !best_supported && !best_flag && normal->k > best_height;
                if (accept)
                {
                    selected = i;
                    best_height = normal->k;
                    best_supported = supported;
                    best_speed = speed;
                    best_flag = flag;
                }
            }
        }
        if ((short)selected != NONE && best_supported)
        {
            vector3f normal = *(vector3f *)(contacts + (short)selected * 0x48 + 0x28);
            function_1201a0(&normal, g_4687b0);
            function_1f1df0(*(long *)(request + 0x40), (short)selected, &normal, history);
        }
        else
        {
            vector3f normal;
            long surface;
            if (function_1f2250((vector3f *)history, request, velocity, moving, &normal, &surface,
                &history->unknown70, (long *)(out + 0xc)))
            {
                function_1f1df0(*(long *)(request + 0x40), highest, &normal, history);
                if ((short)selected != NONE)
                {
                    if (history->unknown70 > 0.0f)
                        *(dword *)(out + 4) |= 2;
                    if (!best_supported)
                        *(long *)(out + 8) = (short)selected;
                }
            }
            else
                *(dword *)(out + 4) |= 1;
        }
    }
    else
    {
        bool airborne = (flags & 0x18) != 0;
        if ((flags & 2) || airborne)
        {
            history->normal = *g_4687b0;
            history->material = g_47d8e0;
            history->unknown58 = NONE;
            history->unknown70 = 0.0f;
            if (airborne)
                *(dword *)(out + 4) |= 1;
        }
    }
    if (!havok_component_any_rigid_body_active((s_havok_component *)component))
        *(dword *)(out + 4) &= ~1;
    if (*(long *)(request + 8) == NONE)
    {
        *(vector3f *)history = *g_4687a4;
        if (!(*(byte *)(out + 4) & 1) && *(long *)(out + 8) != NONE)
        {
            byte *contact = *(byte **)(component + 0x88) + *(long *)(out + 8) * 0x48;
            if (*(long *)(contact + 0x18) == *(long *)(request + 4) && *(long *)(request + 8) == NONE)
            {
                *(long *)out = *(long *)(request + 4);
                *(vector3f *)history = *g_4687a4;
            }
        }
    }
}
