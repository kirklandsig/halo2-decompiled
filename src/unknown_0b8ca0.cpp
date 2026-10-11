// @flags /O2 /arch:SSE /Gr
/* object markers and object physics state (0xb8ca0-0xb9c60) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include <math.h>

struct s_object_definition_view
{
	byte unknown00[2];
	byte flag0 : 1;
	byte flag1 : 1;
	byte flag2 : 1;
	byte flag3 : 1;
	byte flag4 : 1;
	byte : 3;
	byte unknown03[0x38 - 3];
	long model_index;
};

struct s_model_definition_view
{
	byte unknown00[4];
	long render_model_index;
};

/* the object (the object data) */
struct s_object_view
{
	long definition_index;
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	dword flag8 : 1;
	dword flag9 : 1;
	dword mirrored : 1;
	dword flag11 : 1;
	dword flag12 : 1;
	dword flag13 : 1;
	dword flag14 : 1;
	dword : 17;
	byte unknown08[0x14 - 8];
	long parent_index;
	byte unknown18[0xa0 - 0x18];
	real scale;
	byte unknown0a4[0xaa - 0xa4];
	byte type;
	byte unknown0ab[0xb4 - 0xab];
	long havok_component_index;
	byte unknown0b8[0xc0 - 0xb8];
	byte unknown0c0 : 6;
	byte physics_active : 1;
	byte flag7c0 : 1;
	byte physics_disabled : 1;
	byte : 7;
	byte unknown0c2[0xd4 - 0xc2];
	long field_x10a40f;
	byte unknown0d8[0x10e - 0xd8];
	short root_node_offset;
	short unknown110;
	short unknown112;
	short node_matrices_size;
	short node_matrices_offset;
	byte unknown118[0x11a - 0x118];
	short region_permutations_offset;
	byte unknown11c[0x12a - 0x11c];
	short animation_state_offset;
	byte unknown12c[0x154 - 0x12c];
	long unit_index;
};

struct s_object_header_view
{
	short identifier;
	byte flags;
	byte type;
	short unknown04;
	byte unknown06[2];
	s_object_view *object;
};

#define OBJECT_HEADER_GET(index) (&((s_object_header_view *)g_4e0300->data)[(index) & 0xffff])
#define OBJECT_GET(index) (OBJECT_HEADER_GET(index)->object)
#define TAG_DATA(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)

bool function_10cf50(long item_index);
long function_1d8f00(long render_model_index, long marker_name);
long function_1d8f50(long marker_group_index, long render_model_index, byte const *region_permutations,
	long const *node_remapping, transform4x3f const *field_50, bool mirrored, s_object_marker *markers, long count);
void function_b58c0(long index, dword mask);
void function_b7360(long object_index);
void havok_component_rigid_bodies_activate(s_havok_component *component);

void function_be650(long *list, long object_index);

__declspec(noinline) void function_b9890(long object_index);

// @retail 0xb9890
void function_b9890(long object_index)
{
	s_object_header_view *header = OBJECT_HEADER_GET(object_index);
	s_object_view *object = header->object;
	s_object_view *parent = OBJECT_GET(object->parent_index);
	function_be650((long *)((byte *)parent + 0x10), object_index);
	header->flags &= 0x7f;
	object->parent_index = NONE;
	*((char *)object + 0x18) = NONE;
}

// @retail 0xb8ca0
long function_b8ca0(long object_index)
{
    long const volatile *index_reference_r16 = &object_index;
    object_index = *index_reference_r16;
    long result_value = 0;
	s_object_view *object = OBJECT_GET(object_index);

	if (object->parent_index != NONE &&
		TEST_FIELD_BIT(TAG_DATA(s_object_definition_view, object->definition_index)->flag4))
	{
		{ result_value = function_b8ca0(object->parent_index); goto return_exit; }
	}
	if (TEST_FIELD_BIT(object->flag0) && ((1 << object->type) & 0x1c) && function_10cf50(object_index))
		{ result_value = object->unit_index; goto return_exit; }
	{ result_value = object_index; goto return_exit; }

return_exit:
    return result_value;
}

// @retail 0xb8d30
short function_b8d30(long object_index, long marker_name, s_object_marker *markers, short count, bool flag)
{
    long marker_object_index = object_index;
    short volatile result = 0;

    if (!flag)
        marker_object_index = function_b8ca0(object_index);
    if (marker_object_index != NONE)
    {
        s_object_view *object = OBJECT_GET(marker_object_index);
        long model_index = TAG_DATA(s_object_definition_view, object->definition_index)->model_index;

        if (model_index != NONE)
        {
            byte const *region_permutations = (byte *)object + object->region_permutations_offset;
            transform4x3f const *field_50 = (transform4x3f *)((byte *)object + object->node_matrices_offset);
            bool mirrored = TEST_FIELD_BIT(object->mirrored);
            volatile long node_count = object->node_matrices_size / sizeof(transform4x3f);
            long render_model_index = TAG_DATA(s_model_definition_view, model_index)->render_model_index;

            result = function_1d8f50(function_1d8f00(render_model_index, marker_name), render_model_index,
                region_permutations, NULL, field_50, mirrored, markers, count);
            if (result)
                return result;
        }
    }

    {
        s_object_view *object = OBJECT_GET(object_index);

        markers->node_index = 0;
        markers->node_matrix.scale = 1.0f;
        markers->node_matrix.rotation.forward.i = 1.0f;
        markers->node_matrix.rotation.forward.j = 0.0f;
        markers->node_matrix.rotation.forward.k = 0.0f;
        markers->node_matrix.rotation.left.i = 0.0f;
        markers->node_matrix.rotation.left.j = 1.0f;
        markers->node_matrix.rotation.left.k = 0.0f;
        markers->node_matrix.rotation.up.i = 0.0f;
        markers->node_matrix.rotation.up.j = 0.0f;
        markers->node_matrix.rotation.up.k = 1.0f;
        markers->node_matrix.position.x = 0.0f;
        markers->node_matrix.position.y = 0.0f;
        markers->node_matrix.position.z = 0.0f;
        s_object_view *node_object = OBJECT_GET(object_index);
        markers->matrix = *(transform4x3f *)((byte *)node_object + node_object->node_matrices_offset);
        markers->unknown6c = 0.0f;
        if (TEST_FIELD_BIT(object->mirrored))
        {
            markers->matrix.rotation.left.i = 0.0f - markers->matrix.rotation.left.i;
            markers->matrix.rotation.left.j = 0.0f - markers->matrix.rotation.left.j;
            markers->matrix.rotation.left.k = 0.0f - markers->matrix.rotation.left.k;
        }
        if (!marker_name)
            return 1;
    }
    return result;
}

// Preserve the activation call boundary while inlining the component lookup.
__declspec(noinline) void __stdcall function_b9b90(long object_index, bool disable);
// @retail 0xb9b90
void __stdcall function_b9b90(long object_index, bool disable)
{
	struct s_physics_flags
	{
		unsigned short : 6;
		word active : 1;
		unsigned short : 1;
		word disabled : 1;
		unsigned short : 7;
	};
	s_object_header_view *header = OBJECT_HEADER_GET(object_index);
	s_object_view *object = header->object;
	dword type_mask = 1 << header->type;

	if ((type_mask & 0x1883) &&
		TEST_FIELD_BIT(((s_physics_flags *)((byte *)object + 0xc0))->active) &&
		object->havok_component_index != NONE)
	{
		if (!disable)
		{
			havok_component_rigid_bodies_activate(havok_component_get(object->havok_component_index));
		}
		else
			goto update;
	}
	else if (disable)
	{
		((s_physics_flags *)((byte *)object + 0xc0))->disabled = true;
		goto update;
	}
	((s_physics_flags *)((byte *)object + 0xc0))->disabled = false;
	function_b7360(object_index);

update:
	if (type_mask & 0x1c)
	{
		s_object_view *item = OBJECT_GET(object_index);
		if (item->field_x10a40f != NONE)
			function_b58c0(item->field_x10a40f, 0x400);
	}
	else if (type_mask & 0x20)
	{
		s_object_view *projectile = OBJECT_GET(object_index);
		if (projectile->field_x10a40f != NONE)
			function_b58c0(projectile->field_x10a40f, 0x400);
	}
}


void __stdcall function_bd020(long object_index);

/* a node's orientation as the animation state blends it (0x20 bytes) */
struct s_object_node_orientation
{
	real quaternion[4];
	point3f translation;
	real scale;
};

struct s_animation_state;
struct s_blend_orientation;
bool function_1cb5f0(long node_count, s_animation_state *state, real seconds, s_blend_orientation *orientations,
	s_blend_orientation const *targets);

/* blends an object's node orientations toward their targets over the given
   time, and updates the object's nodes */
// @retail 0xba350
void function_ba350(long object_index, real seconds)
{
	s_object_view *object = OBJECT_GET(object_index);

	if (OBJECT_GET(object_index)->unknown112 != NONE &&
		TAG_DATA(s_object_definition_view, object->definition_index)->model_index != NONE)
	{
		function_1cb5f0(object->unknown110 / sizeof(s_object_node_orientation),
			(s_animation_state *)((byte *)object + object->animation_state_offset), seconds,
			(s_blend_orientation *)((byte *)object + object->root_node_offset),
			(s_blend_orientation *)((byte *)object + object->unknown112));
		function_b7360(object_index);
	}
}

/* the root node's state at the object's offset +0x10e */
struct s_object_root_node
{
	byte unknown00[0x10];
	vector3f vector;
	real value_1c;
};

// @retail 0xb7680
void function_b7680(long object_index, real scale, real seconds)
{
	if (object_index != NONE && scale > 0.0f)
	{
		s_object_view *object = OBJECT_GET(object_index);
		real old_scale = object->scale;

		object->scale = scale;
		if (OBJECT_GET(object_index)->unknown112 != NONE)
		{
			s_object_root_node *node;
			real ratio;

			function_ba350(object_index, seconds);
			node = (s_object_root_node *)((byte *)object + object->root_node_offset);
			ratio = old_scale / scale;
			node->value_1c *= ratio;
			node->vector.i *= ratio;
			node->vector.j *= ratio;
			node->vector.k *= ratio;
		}
		function_bd020(object_index);
	}
}

// @retail 0xb9d20
bool function_b9d20(long object_index)
{
	long root_index = NONE;

	while (object_index != NONE)
	{
		root_index = object_index;
		object_index = OBJECT_GET(object_index)->parent_index;
	}
	return (OBJECT_HEADER_GET(root_index)->flags >> 6) & 1;
}

void function_b8b70(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);

// @retail 0xb9c60
void function_b9c60(long object_index, bool flag)
{
    bool const *flag_reference = &flag;
    s_object_view *object = OBJECT_GET(object_index);
    if (*flag_reference)
    {
        if (!TEST_FIELD_BIT(object->flag0))
        {
            if (function_b9d20(object_index))
                function_bef30(object_index, 1, 0, 0, false);
            object->flag0 = true;
            function_b8b70(object_index);
        }
    }
    else if (TEST_FIELD_BIT(object->flag0))
    {
        object->flag0 = false;
        if (function_b9d20(object_index))
            function_bef30(object_index, 0, 1, 0, false);
        function_b8b70(object_index);
    }
}

/* the list of objects that count (g_4de2f4, unknown_0bb760.cpp) */
struct s_object_list_view
{
	byte unknown00[4];
	short count;
};

struct s_object_list;
extern s_object_list *g_4de2f4;
void function_1c3850(long object_index);

/* marks an object without a parent as connected (header flag 0) */
// @retail 0xb7290
void function_b7290(long object_index)
{
	s_object_header_view *header = OBJECT_HEADER_GET(object_index);

	if (!(header->flags & 1) && header->unknown04 != NONE)
	{
		s_object_view *object = header->object;

		if (object->parent_index == NONE)
		{
			header->flags |= 1;
			if ((1 << header->type) & 0x1883)
				function_1c3850(object_index);
			if (TEST_FIELD_BIT(object->flag14))
				((s_object_list_view *)g_4de2f4)->count++;
		}
	}
}

void __stdcall function_1c38a0(long object_index);

// @retail 0xb7300
void function_b7300(long object_index)
{
	s_object_header_view *header = OBJECT_HEADER_GET(object_index);
	if (header->flags & 1)
	{
		s_object_view *object = header->object;
		header->flags &= ~5;
		if ((1 << header->type) & 0x1883)
			function_1c38a0(object_index);
		if (TEST_FIELD_BIT(object->flag14))
			((s_object_list_view *)g_4de2f4)->count--;
	}
}


real function_30bf0(vector3f *vector);
vector3f *function_11d090(vector3f const *vector, vector3f *out);

static __forceinline void object_cross_ab(vector3f const *a, vector3f const *b, vector3f *out)
{
    real k = a->i * b->j - a->j * b->i;
    real j = a->k * b->i - a->i * b->k;
    real i = a->j * b->k - a->k * b->j;
    out->i = i;
    out->j = j;
    out->k = k;
}

// @retail 0xb91d0
void function_b91d0(long object_index, vector3f *forward, vector3f *up)
{
    vector3f left;
    object_cross_ab(forward, up, &left);
    object_cross_ab(&left, forward, up);
    if (function_30bf0(forward) > 0.0f)
    {
        if (!(function_30bf0(up) > 0.0f))
            function_11d090(forward, up);
    }
    else
    {
        byte *object = (byte *)OBJECT_GET(object_index);
        *forward = *(vector3f *)(object + 0x70);
        *up = *(vector3f *)(object + 0x7c);
    }
}


static __forceinline void object_rotate_ab(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
    real x = vector->i;
    real y = vector->j;
    real z = vector->k;
    out->i = matrix->rotation.up.i * z + matrix->rotation.left.i * y + matrix->rotation.forward.i * x;
    out->j = matrix->rotation.up.j * z + matrix->rotation.left.j * y + matrix->rotation.forward.j * x;
    out->k = matrix->rotation.up.k * z + matrix->rotation.left.k * y + matrix->rotation.forward.k * x;
}

// @retail 0xb9fc0
void function_b9fc0(long object_index, vector3f *forward, vector3f *up)
{
    s_object_view *object = OBJECT_GET(object_index);
    if (object->parent_index == NONE)
    {
        if (forward) *forward = *(vector3f *)((byte *)object + 0x70);
        if (up) *up = *(vector3f *)((byte *)object + 0x7c);
    }
    else
    {
        s_object_view *parent = OBJECT_GET(object->parent_index);
        long node = *(signed char *)((byte *)object + 0x18);
        transform4x3f *matrix = (transform4x3f *)((byte *)parent + *(short *)((byte *)parent + 0x116) + node * 0x34);
        if (forward) object_rotate_ab(matrix, (vector3f *)((byte *)object + 0x70), forward);
        if (up) object_rotate_ab(matrix, (vector3f *)((byte *)object + 0x7c), up);
    }
}


void function_cc590(long object_index);
void function_b9a90(long object_index);

void __stdcall function_b9a50(long object_index);

#if 0
// Retail 0xb9a50. Activating this dispatcher changes shared unit-call conventions.
void __stdcall function_b9a50(long object_index)
{
    s_object_view *object = OBJECT_GET(object_index);
    if (object->parent_index != NONE)
    {
        if ((1 << object->type) & 3)
            function_cc590(object_index);
        else
            function_b9a90(object_index);
    }
}
#endif

extern void (__stdcall *g_468664[8])(long object_index);
void __stdcall function_b8540(long object_index);

// @retail 0xb83b0
void __stdcall function_b83b0(long object_index, bool unused)
{
    s_object_view *object = OBJECT_GET(object_index);
    function_b8540(object_index);
    for (dword i = 0; i < 8; i++)
        g_468664[i](object_index);
    long child = *(long *)((byte *)object + 0x10);
    while (child != NONE)
    {
        s_object_view *local_f86fb0 = OBJECT_GET(child);
        long next = *(long *)((byte *)local_f86fb0 + 0xc);
        bool attached = false;
        if (g_4e6948->mode == 4)
            attached = local_f86fb0->field_x10a40f != NONE;
        if (attached)
            {
                if (local_f86fb0->parent_index != NONE)
                {
                    if ((1 << local_f86fb0->type) & 3)
                        function_cc590(child);
                    else
                        function_b9a90(child);
                }
            }
        else
            function_b83b0(child, false);
        child = next;
    }
}

void __stdcall function_b87b0(long object_index);
void object_widgets_delete(long object_index);
void function_bee60(long object_index);
void function_146bf0();
void havok_object_detach(long object_index);
void function_278f00();
void function_108bf0(long object_index);
void __stdcall function_bc300(long object_index);

// @retail 0xb8460
void __stdcall function_b8460(long object_index, bool detach)
{
    s_object_view *object = OBJECT_GET(object_index);
    if (detach)
    {
        if (object->parent_index != NONE)
            function_b9890(object_index);
        function_b7300(object_index);
        if (TEST_FIELD_BIT(object->flag8))
            function_b87b0(object_index);
    }
    object_widgets_delete(object_index);
    function_bee60(object_index);
    long child = *(long *)((byte *)object + 0x10);
    while (child != NONE)
    {
        long next = *(long *)((byte *)OBJECT_GET(child) + 0xc);
        function_b8460(child, false);
        child = next;
    }
    bool physics = TEST_FIELD_BIT(OBJECT_GET(object_index)->physics_active);
    if (physics)
        function_146bf0();
    havok_object_detach(object_index);
    if (physics)
    {
        function_278f00();
        function_146bf0();
    }
    function_108bf0(object_index);
    function_bc300(object_index);
}


void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);
void __stdcall function_b98e0(long object_index, transform4x3f const *matrix);
void __stdcall function_b8600(long object_index, long location);
void __stdcall function_b8890(long object_index);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);

// @retail 0xb9a90
void function_b9a90(long object_index)
{
    s_record_pool *objects = g_4e0300;
    s_object_view *object = ((s_object_header_view *)objects->data)[object_index & 0xffff].object;
    s_object_view *parent = ((s_object_header_view *)objects->data)[object->parent_index & 0xffff].object;
    bool connected = function_b9d20(object->parent_index);
    s_object_view *node_parent = ((s_object_header_view *)objects->data)[object->parent_index & 0xffff].object;
    transform4x3f const *matrix = (transform4x3f *)((byte *)node_parent + node_parent->node_matrices_offset) + *(char *)((byte *)object + 0x18);
    if (connected)
        function_bef30(object_index, 1, 0, 0, false);
    function_b9890(object_index);
    function_b98e0(object_index, matrix);
    if (connected)
        function_b8600(object_index, 0);
    if (!TEST_FIELD_BIT(object->flag7c0) && !((1 << OBJECT_HEADER_GET(object_index)->type) & 0x80))
        function_b8890(object_index);
    function_b77d0(object_index, (vector3f *)((byte *)parent + 0x88), (vector3f *)((byte *)parent + 0x94));
    function_b7290(object_index);
    function_b7360(object_index);
    *(dword *)((byte *)object + 4) &= ~0x4000000;
}

struct s_object_link_owner;
struct s_object_link_iterator
{
    s_object_link_owner *owner;
    long index;
};
short function_b8a80(long object_index, s_object_link_iterator *iterator);
short function_b8b20(s_object_link_iterator *iterator);
real function_c8880(long unit_index, short field_240);

static __forceinline long object_query_next_ab(s_record_pool *data, long index)
{
    long absolute = function_16bc00(data, index == NONE ? 0 : (index & 0xffff) + 1);
    if (absolute == NONE)
        return NONE;
    return (*(short *)(data->data + data->size * absolute) << 16) | absolute;
}

// @retail 0xbba80
bool function_bba80(long object_index)
{
    volatile bool result = false;
    volatile long root = NONE;
    long index = object_index;
    s_object_header_view *headers = (s_object_header_view *)g_4e0300->data;
    while (index != NONE)
    {
        root = index;
        index = *(long *)((byte *)headers[index & 0xffff].object + 0x14);
    }
    s_object_header_view *header = OBJECT_HEADER_GET(root);
    dword flags = *(dword *)((byte *)header->object + 4);
    if (!(((byte *)header)[2] & 1) || !(bool)((flags >> 8) & 1) || (bool)((flags >> 18) & 1))
        return result;
    s_object_link_iterator links;
    short cluster = function_b8a80(root, &links);
    while (cluster != NONE)
    {
        if (((dword *)((byte *)g_4e6948 + 0x1138))[cluster >> 5] & (1 << (cluster & 31)))
            break;
        cluster = function_b8b20(&links);
    }
    if (cluster == NONE)
        return result;

    byte *object = (byte *)OBJECT_GET(object_index);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long model_index = *(long *)(definition + 0x38);
    real radius_squared = *(real *)(object + 0x3c) * *(real *)(object + 0x3c);
    real range_squared = 0.0f;
    if (model_index != NONE)
    {
        real range = *(real *)(g_4e3b44[model_index & 0xffff].bytes + 0x28);
        if (range > 0.0f)
        {
            range *= 1.2f;
            range_squared = range * range;
        }
        else
            range_squared = 1600.0f;
    }
    s_record_pool *players = g_4e8c24;
    long player_index = object_query_next_ab(players, NONE);
    while (player_index != NONE)
    {
        byte *player = players->data + (player_index & 0xffff) * 0x21c;
        long unit_index = *(long *)(player + 0x2c);
        if (unit_index != NONE)
        {
            s_object_marker marker;
            function_b8d30(unit_index, 0x04000095, &marker, 1, false);
            point3f position = marker.matrix.position;
            point3f *centre = (point3f *)(object + 0x30);
            vector3f offset;
            offset.i = centre->x - position.x;
            offset.j = centre->y - position.y;
            offset.k = centre->z - position.z;
            real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
            byte *unit = (byte *)OBJECT_GET(*(long *)(player + 0x2c));
            real zoom = function_c8880(*(long *)(player + 0x2c), *(signed char *)(unit + 0x240));
            if (zoom > 0.0001f)
                distance_squared /= zoom * zoom;
            if (distance_squared < radius_squared)
            {
                result = true;
                break;
            }
            if (distance_squared < range_squared)
            {
                offset.i = centre->x - position.x;
                offset.j = centre->y - position.y;
                offset.k = centre->z - position.z;
                vector3f *forward = (vector3f *)((byte *)OBJECT_GET(*(long *)(player + 0x2c)) + 0x15c);
                double radius = *(real *)(object + 0x3c);
                real magnitude = function_30bf0(&offset);
                if (offset.k * forward->k + offset.j * forward->j + offset.i * forward->i >
                    cos(atan2((double)magnitude, radius) + 0.7853981852531433f))
                {
                    result = true;
                    break;
                }
            }
        }
        players = g_4e8c24;
        player_index = object_query_next_ab(players, player_index);
    }
    return result;
}


bool __stdcall function_1d44f0(s_havok_component *component, point3f *center, real *radius);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long unknown);

// @retail 0xbdef0
bool __stdcall function_bdef0(long object_index)
{
    byte *object = (byte *)OBJECT_GET(object_index);
    byte const *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    bool active = (*(dword *)(object + 4) & 0x100) != 0;
    bool from_physics = false;
    long component_index = *(long *)(object + 0xb4);
    if (component_index != NONE && ((1 << object[0xaa]) & 0x800) && (object[0xc0] & 0x40))
    {
        s_havok_component *component = (s_havok_component *)(g_51e9b8->data +
            (component_index & 0xffff) * 0xa0);
        from_physics = function_1d44f0(component, (point3f *)(object + 0x30), (real *)(object + 0x3c));
    }
    if (!from_physics)
    {
        byte *node_object = (byte *)OBJECT_GET(object_index);
        transform4x3f const *matrix = (transform4x3f const *)(node_object + *(short *)(node_object + 0x116));
        point3f center = *(point3f const *)(definition + 8);
        if (matrix->scale != 1.0f)
        {
            center.x *= matrix->scale;
            center.y *= matrix->scale;
            center.z *= matrix->scale;
        }
        *(real *)(object + 0x30) = matrix->up.i * center.z + matrix->left.i * center.y +
            matrix->forward.i * center.x + matrix->position.x;
        *(real *)(object + 0x34) = matrix->up.j * center.z + matrix->left.j * center.y +
            matrix->forward.j * center.x + matrix->position.y;
        *(real *)(object + 0x38) = matrix->up.k * center.z + matrix->left.k * center.y +
            matrix->forward.k * center.x + matrix->position.z;
        *(real *)(object + 0x3c) = *(real *)(object + 0xa0) * *(real const *)(definition + 4);
    }
    if (*(real const *)(definition + 0x20) > 0.0f)
    {
        *(real *)(object + 0x50) = *(real *)(object + 0x30) + *(real const *)(definition + 0x24);
        *(real *)(object + 0x54) = *(real *)(object + 0x34) + *(real const *)(definition + 0x28);
        *(real *)(object + 0x58) = *(real *)(object + 0x38) + *(real const *)(definition + 0x2c);
        *(real *)(object + 0x5c) = *(real *)(object + 0xa0) * *(real const *)(definition + 0x20);
    }
    else
    {
        *(point3f *)(object + 0x50) = *(point3f *)(object + 0x30);
        *(real *)(object + 0x5c) = *(real *)(object + 0x3c);
    }
    point3f center;
    real radius;
    if (*(word const *)(definition + 2) & 0x1000)
    {
        center = *(point3f *)(object + 0x50);
        radius = *(real *)(object + 0x5c);
    }
    else
    {
        center = *(point3f *)(object + 0x30);
        radius = *(real *)(object + 0x3c);
    }
    if (active)
    {
        vector3f delta;
        delta.i = center.x - *(real *)(object + 0x40);
        delta.j = center.y - *(real *)(object + 0x44);
        delta.k = center.z - *(real *)(object + 0x48);
        if (delta.k * delta.k + delta.i * delta.i + delta.j * delta.j > 0.01f ||
            radius != *(real *)(object + 0x4c))
            function_b87b0(object_index);
        else
            return false;
    }
    *(point3f *)(object + 0x40) = center;
    *(real *)(object + 0x4c) = radius;
    if (active)
        function_b8600(object_index, 0);
    return true;
}


void function_1091b0(long object_index);
void __stdcall function_1c35f0(long object_index);

// @retail 0xb73b0
void function_b73b0(long object_index)
{
    vector3f const *velocity = g_4687a4;
    function_b77d0(object_index, velocity, velocity);
    function_b9b90(object_index, false);
    bool active = TEST_FIELD_BIT(OBJECT_GET(object_index)->physics_active);
    if (active) function_146bf0();
    havok_object_detach(object_index);
    if (active)
    {
        function_278f00();
        function_146bf0();
    }
    function_1091b0(object_index);
    function_146bf0();
    function_1c35f0(object_index);
    function_278f00();
    function_146bf0();
}
