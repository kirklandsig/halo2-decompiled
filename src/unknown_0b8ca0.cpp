// @flags /O2 /arch:SSE /Gr
#include "unknown_0b68c0.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"
/* object markers and object physics state (0xb8ca0-0xb9c60) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "unknown_16d180.h"
#include "matrix_transform.h"
#include <math.h>
#include <xmmintrin.h>
#include <string.h>

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
	word unknown0c0 : 6;
	word physics_active : 1;
	word flag7c0 : 1;
	word physics_disabled : 1;
	word : 2;
	word flag11c0 : 1;
	word : 4;
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

// @retail 0xb8ca0 standard
long __stdcall function_b8ca0(long object_index)
{
    long const volatile *index_reference_r16 = &object_index;
    object_index = *index_reference_r16;
    long result_value = 0;
	s_object_view *object = OBJECT_GET(object_index);

	if (object->parent_index != NONE)
	{
		word parent_flags = *(word const *)((byte const *)TAG_DATA(s_object_definition_view, object->definition_index) + 2);
		if ((bool)(((dword)parent_flags >> 4) & 1))
	{
		{ result_value = function_b8ca0(object->parent_index); goto return_exit; }
	}
	}
	if (TEST_FIELD_BIT(object->flag0) && ((1u << object->type) & 0x1c) && function_10cf50(object_index))
		{ result_value = object->unit_index; goto return_exit; }
	{ result_value = object_index; goto return_exit; }

return_exit:
    return result_value;
}

// @retail 0xb8d30
short function_b8d30(long object_index, long marker_name, s_object_marker *markers, short count, bool flag)
{
    long marker_object_index = object_index;
    short result = 0;

    if (!flag)
        marker_object_index = function_b8ca0(object_index);
    if (marker_object_index != NONE)
    {
        s_object_view *object = OBJECT_GET(marker_object_index);
        long model_index = TAG_DATA(s_object_definition_view, object->definition_index)->model_index;

        if (model_index != NONE)
        {
            s_model_definition_view const *model = TAG_DATA(s_model_definition_view, model_index);
            byte const *region_permutations = (byte *)object + object->region_permutations_offset;
            transform4x3f const *field_50 = (transform4x3f *)((byte *)object + object->node_matrices_offset);
            bool mirrored = TEST_FIELD_BIT(object->mirrored);
            volatile long node_count = object->node_matrices_size / sizeof(transform4x3f);

            long marker_group_index = function_1d8f00(model->render_model_index, marker_name);

            result = function_1d8f50(marker_group_index, model->render_model_index,
                region_permutations, NULL, field_50, mirrored, markers, count);

        }
    }

    if (!result)
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
            vector3f const *left = &markers->matrix.rotation.left;
            markers->matrix.rotation.left.i = 0.0f - left->i;
            markers->matrix.rotation.left.j = 0.0f - left->j;
            markers->matrix.rotation.left.k = 0.0f - left->k;
        }
        if (!marker_name)
            result = 1;
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
	s_object_view *object = OBJECT_GET(object_index);
	long type = OBJECT_HEADER_GET(object_index)->type;

	if (((1 << type) & 0x1883) &&
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
	if ((1 << type) & 0x1c)
	{
		s_object_view *item = OBJECT_GET(object_index);
		if (item->field_x10a40f != NONE)
			function_b58c0(item->field_x10a40f, 0x400);
	}
	else if ((1 << type) & 0x20)
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
bool function_1cb5f0(s_animation_state *state, real seconds, long node_count, s_blend_orientation *orientations,
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
		function_1cb5f0((s_animation_state *)((byte *)object + object->animation_state_offset), seconds,
			object->unknown110 / sizeof(s_object_node_orientation),
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

__declspec(noinline) void function_b7300(long object_index);
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

__declspec(noinline) void __stdcall function_b9a50(long object_index);
// @retail 0xb9a50
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
dword function_bee60(long object_index);
void function_146bf0();
void havok_object_detach(long object_index);
void function_278f00();
void object_dispatch_remove(long object_index);
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
    object_dispatch_remove(object_index);
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
    s_object_view *object = OBJECT_GET(object_index);
    s_object_view *parent = OBJECT_GET(object->parent_index);
    bool connected = function_b9d20(object->parent_index);
    s_object_view *node_parent = OBJECT_GET(*(long volatile *)&object->parent_index);
    long node_offset = *(char *)((byte *)object + 0x18) * sizeof(transform4x3f);
    transform4x3f const *matrix = (transform4x3f *)((byte *)node_parent + node_offset + node_parent->node_matrices_offset);
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
    s_object_link_owner const *owner;
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
    long result = false;
    {
        long root = NONE;
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
            goto done;
        s_object_link_iterator links;
        short cluster = function_b8a80(root, &links);
        while (cluster != NONE)
        {
            if (((dword *)((byte *)g_4e6948 + 0x1138))[cluster >> 5] & (1 << (cluster & 31)))
                break;
            cluster = function_b8b20(&links);
        }
        if (cluster == NONE)
            goto done;

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
                    goto done;
                }
                if (distance_squared < range_squared)
                {
                    offset.i = centre->x - position.x;
                    offset.j = centre->y - position.y;
                    offset.k = centre->z - position.z;
                    vector3f *forward = (vector3f *)((byte *)OBJECT_GET(*(long *)(player + 0x2c)) + 0x15c);
                    double radius_value = *(real *)(object + 0x3c);
                    double const &radius = radius_value;
                    real magnitude = function_30bf0(&offset);
                    if (offset.k * forward->k + offset.j * forward->j + offset.i * forward->i >
                        cos(atan2((double)magnitude, radius) + 0.7853981852531433f))
                    {
                        result = true;
                        goto done;
                    }
                }
            }
            players = g_4e8c24;
            player_index = object_query_next_ab(players, player_index);
        }
    }
done:
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
    long component_index = *(long *)(object + 0xb4);
    bool active = (bool)((*(dword *)(object + 4) >> 8) & 1);
    bool from_physics = false;
    if (component_index != NONE && ((1 << object[0xaa]) & 0x800) && TEST_FIELD_BIT(((s_object_view *)object)->physics_active))
    {
        s_havok_component *component = (s_havok_component *)(g_51e9b8->data +
            (component_index & 0xffff) * 0xa0);
        from_physics = function_1d44f0(component, (point3f *)(object + 0x30), (real *)(object + 0x3c));
    }
    if (!from_physics)
    {
        byte *node_object = (byte *)OBJECT_GET(object_index);
        transform4x3f const *matrix = (transform4x3f const *)(node_object + *(short *)(node_object + 0x116));
        point3f center;
        real_point3d_set(&center, *(real const *)(definition + 8),
            *(real const *)(definition + 0xc), *(real const *)(definition + 0x10));
        point3f *output_center = (point3f *)(object + 0x30);
        if (matrix->scale != 1.0f)
        {
            center.x *= matrix->scale;
            center.y *= matrix->scale;
            center.z *= matrix->scale;
        }
        output_center->x = matrix->up.i * center.z + matrix->left.i * center.y +
            matrix->forward.i * center.x + matrix->position.x;
        output_center->y = matrix->up.j * center.z + matrix->left.j * center.y +
            matrix->forward.j * center.x + matrix->position.y;
        output_center->z = matrix->up.k * center.z + matrix->left.k * center.y +
            matrix->forward.k * center.x + matrix->position.z;
        *(real *)(object + 0x3c) = *(real *)(object + 0xa0) * *(real const *)(definition + 4);
    }
    real secondary_radius = *(real const *)(definition + 0x20);
    if (secondary_radius > 0.0f)
    {
        *(real *)(object + 0x50) = *(real *)(object + 0x30) + *(real const *)(definition + 0x24);
        *(real *)(object + 0x54) = *(real *)(object + 0x34) + *(real const *)(definition + 0x28);
        *(real *)(object + 0x58) = *(real *)(object + 0x38) + *(real const *)(definition + 0x2c);
        *(real *)(object + 0x5c) = *(real *)(object + 0xa0) * secondary_radius;
    }
    else
    {
        *(point3f *)(object + 0x50) = *(point3f *)(object + 0x30);
        *(real *)(object + 0x5c) = *(real *)(object + 0x3c);
    }
    point3f center;
    real radius;
    if ((bool)(((dword)*(word const *)(definition + 2) >> 12) & 1))
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
        if (delta.k * delta.k + delta.i * delta.i + delta.j * delta.j > 0.1f * 0.1f ||
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


void object_dispatch_physics_detached(long object_index);
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
    object_dispatch_physics_detached(object_index);
    function_146bf0();
    function_1c35f0(object_index);
    function_278f00();
    function_146bf0();
}



void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference);

// @retail 0xb87b0
void __stdcall function_b87b0(long object_index)
{
    s_object_header_view *header = OBJECT_HEADER_GET(object_index);
    s_object_view *object = header->object;
    function_bef30(object_index, 1, 0, 0, false);
    s_cluster_partition *partition = TEST_FIELD_BIT(object->flag9)
        ? &g_partition_colliding_storage
        : &g_partition_noncolliding_storage;
    function_1cae40(partition, object_index, (long *)((byte *)object + 0x60));
    object->flag8 = false;
    header->flags &= ~0x40;
}



struct s_object_link_owner
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};
struct s_object_cluster_link
{
    short salt;
    short unknown02;
    short cluster;
    short unknown06;
    long next;
};

// Disabled: the persistent partition address changes M3-owned 1697c0 and loses eight matched callers/wrappers.
#if 0
// Retail 0xb8a80
short function_b8a80(long object_index, s_object_link_iterator *iterator)
{
    long root = NONE;
    while (object_index != NONE)
    {
        root = object_index;
        object_index = OBJECT_GET(object_index)->parent_index;
    }
    s_object_view *object = OBJECT_GET(root);
    // Later partition reads use a const view of the persistent storage.
    iterator->owner = (s_object_link_owner const *)(TEST_FIELD_BIT(object->flag9) ? &g_4de2e0 : &g_4de2d4);
    iterator->index = *(long *)((byte *)object + 0x60);
    s_record_pool *pool = iterator->owner->cluster_references;
    if (iterator->index != NONE)
    {
        s_object_cluster_link *reference = (s_object_cluster_link *)(pool->data + (iterator->index & 0xffff) * pool->size);
        long next = reference->next;
        if (next != NONE)
            _mm_prefetch((char *)(pool->data + (next & 0xffff) * pool->size), _MM_HINT_T0);
        iterator->index = next;
        return reference->cluster;
    }
    return NONE;
}
#endif


#include "object_queries.h"
void function_b75a0(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, s_location const *location, bool unknown);

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);

// @retail 0xbc1d0
bool __stdcall function_bc1d0(long object_index, point3f const *point, long ignore_unit_index)
{
    s_object_view *object = OBJECT_GET(object_index);
    vector3f displacement;
    byte collision_storage[0x5c];
    s_location *collision_location = (s_location *)(collision_storage + 0x1c);
    *(short *)(collision_storage + 0x24) = NONE;
    displacement.i = *(real *)((byte *)object + 0x64) - point->x;
    displacement.j = *(real *)((byte *)object + 0x68) - point->y;
    bool result = false;
    displacement.k = *(real *)((byte *)object + 0x6c) - point->z;
    if (!function_1697c0(0x20800005, point, &displacement, object_index, ignore_unit_index, (s_collision_result_1697c0 *)collision_storage)
        && *(short *)((byte *)object + 0x2c) != NONE)
        result = true;
    else if (collision_location->cluster_index != NONE)
    {
        function_b75a0(object_index, (point3f *)(collision_storage + 8), 0, 0, collision_location, false);
        result = true;
    }
    return result;
}





// Round 18: recheck after bef30 became exact and new queue callers were written.
bool object_or_parent_hidden(long object_index);
void function_b8b70(long object_index);
void __stdcall function_10a250(long object_index);
void function_bf090(long object_index);
void function_15b220(long object_index, long slot);
void __stdcall object_list_erase(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long object_index);
void object_interpolated_values_clear_all(long object_index);

// Disabled: round 18 reactivation with exact bef30 still breaks matched 11a320; retain the complete 14-difference, 190/189-byte draft
// Disabled: round 35: five new fruitless trials, 14 differences; activation loses matched d6bc0 and 11a320
#if 0
// Retail 0xb8540
void __stdcall function_b8540(long object_index)
{
    long const *object_reference = &object_index;
    s_object_header_view *header = OBJECT_HEADER_GET(*object_reference);
    s_object_view *object = header->object;
    if (!object_or_parent_hidden(object_index) && function_b9d20(object_index))
        function_bef30(object_index, 1, 0, 0, false);
    header->flags |= 0x10;
    function_b8b70(object_index);
    if ((1 << header->type) & 0x40) function_10a250(object_index);
    function_bf090(object_index);
    *((byte *)object + 0xab) = 0xff;
    *(word *)((byte *)object + 0xa8) = 0xffff;
    *(long *)((byte *)object + 0xa4) = NONE;
    signed char slot = *(signed char *)((byte *)object + 0xaf);
    if (slot != NONE) function_15b220(object_index, slot);
    object_list_erase(object_index);
    function_bb950(object_index, false, NONE);
    function_a7a60(object_index);
    object_interpolated_values_clear_all(object_index);
}
#endif



#include "unit_hit.h"
struct s_event_a9260
{
    long value00, value04, value08, value0c;
    short value10;
    short unknown12;
    real amount;
    byte kind;
    byte unknown19[3];
};
long g_4cef30 = 0;
bool function_a76b0(long object_index, long flag);
void __stdcall function_b5a70(long player, long type, long count, long objects, long size, void const *data, long timeout);

// @retail 0xa9260
void __stdcall function_a9260(long unit_index, s_unit_melee_hit const *hit, long value)
{
    if (!function_a76b0(unit_index, true)) return;
    bool missing = false;
    if (hit->object_index == NONE)
    {
        if (hit->unknown0c == NONE) return;
    }
    else missing = OBJECT_GET(hit->object_index)->field_x10a40f == NONE;
    if (missing) return;
    long objects[2] = { unit_index, hit->object_index };
    s_event_a9260 event;
    memset(&event, 0, sizeof(event));
    event.value00 = value;
    event.value04 = hit->unknown04;
    event.value08 = hit->unknown08;
    event.value0c = hit->unknown0c;
    event.value10 = hit->material_index;
    event.amount = hit->unknown14;
    event.kind = hit->unknown18;
    function_b5a70(NONE, 0x17, 2, (long)objects, sizeof(event), &event, g_4cef30);
}




// Disabled: protected 0x142a60 uses implicit registers without binding its formal inputs.
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

static __forceinline void b98e0_basis(transform4x3f *out, vector3f const *forward, vector3f const *up)
{
    out->scale = 1.0f;
    out->forward = *forward;
    vector3f left;
    left.k = up->i * forward->j;
    left.k -= up->j * forward->i;
    left.j = up->k * forward->i - up->i * forward->k;
    left.i = up->j * forward->k;
    left.i -= up->k * forward->j;
    out->left.k = left.k;
    out->left.j = left.j;
    out->left.i = left.i;
    out->up = *up;
    out->position.x = 0.0f;
    out->position.y = 0.0f;
    out->position.z = 0.0f;
}

static __forceinline void b98e0_translation(transform4x3f *out, point3f const *position)
{
    out->scale = 1.0f;
    out->forward.i = 1.0f;
    out->forward.j = 0.0f;
    out->forward.k = 0.0f;
    out->left.i = 0.0f;
    out->left.j = 1.0f;
    out->left.k = 0.0f;
    out->up.i = 0.0f;
    out->up.j = 0.0f;
    out->up.k = 1.0f;
    out->position = *position;
}

// score 69 (was 69): the zero float is set up before the table load in ours, retail loads the table first and the float after it
// @retail 0xb98e0
void __stdcall function_b98e0(long object_index, transform4x3f const *matrix)
{
    s_object_view *object = OBJECT_GET(object_index);
    transform4x3f translation;
    b98e0_translation(&translation, (point3f *)((byte *)object + 0x64));
    vector3f const *forward = (vector3f *)((byte *)object + 0x70);
    vector3f const *up = (vector3f *)((byte *)object + 0x7c);
    transform4x3f orientation;
    b98e0_basis(&orientation, forward, up);
    transform4x3f result;
    function_142a60(matrix, &translation, &result);
    function_142a60(&result, &orientation, &result);
    function_b75a0(object_index, &result.position, &result.forward, &result.up, 0, false);
}




#include "object_iterator.h"
#include "object_queries.h"
#include <string.h>
struct s_object_list;
extern s_object_list *g_4de2f4;
void function_c0110();
void function_1089d0(long object_index);

// score 3 (was 3): the record pointer is materialised in esi in ours, retail loads the table into ecx and computes the record address with lea
// @retail 0xb6c50
void function_b6c50()
{
    struct s_object_flags
    {
        dword unknown00;
        dword unknown0 : 8;
        dword flag8 : 1;
        dword unknown9 : 5;
        dword flag14 : 1;
        dword unknown15 : 17;
    };
    function_c0110();
    struct
    {
        byte *object;
        s_type_f1af8e iterator;
    } iteration;
    iteration.iterator.signature = 0x86868686;
    iteration.iterator.type_mask = NONE;
    iteration.iterator.flags = 0;
    iteration.iterator.index = 0;
    iteration.iterator.object_index = NONE;
    iteration.object = (byte *)function_baeb0(&iteration.iterator);
    while (iteration.object)
    {
        long index = iteration.iterator.object_index;
        s_record_pool *pool = g_4e0300;
        byte *records = pool->data;
        long offset = (index & 0xffff) * 12;
        s_object_header_view *header = (s_object_header_view *)(records + offset);
        if (header->flags & 1)
        {
            byte *current = *(byte **)(*(byte *volatile *)((byte *)pool + 0x44) + offset + 8);
            header->flags &= ~5;
            if ((1 << header->type) & 0x1883) function_1c38a0(index);
            if ((bool)((s_object_flags *)current)->flag14) --*(short *)((byte *)g_4de2f4 + 4);
        }
        if ((bool)((s_object_flags *)iteration.object)->flag8) function_b87b0(index);
        function_1089d0(index);
        *(short *)((byte *)header + 4) = NONE;
        *(long *)(iteration.object + 0x28) = NONE;
        *(short *)(iteration.object + 0x2c) = NONE;
        *(short *)(iteration.object + 0x2e) = g_4686c4;
        iteration.object = (byte *)function_baeb0(&iteration.iterator);
    }
    *(byte *)g_4de2f4 = 0;
}

struct s_partition_location;
struct s_object_partition_record_ab
{
    short type;
    word flags;
    point3f centre;
    real radius;
};
void function_b88e0(long object_index, s_object_partition_record_ab *record);
void function_11bed0(s_location *location, point3f const *point);
void function_1cac60(dword const *bits, s_cluster_partition *partition, long data_index,
    long *first_cluster_reference, point3f const *point, real radius, s_partition_location const *location,
    long payload_size, void const *payload, bool *overflow);
bool function_b6d60(long object_index, dword const *clusters);
bool function_a7670(long object_index);
void __stdcall function_b8540(long object_index);
void __stdcall function_1c36f0(long parent_index, long object_index);

// @retail 0xb8600
void __stdcall function_b8600(long object_index, volatile long supplied_location)
{
    s_object_header_view *header = OBJECT_HEADER_GET(object_index);
    byte *object = (byte *)header->object;
    struct s_flags_b8600 { unsigned long : 17; unsigned long field_17 : 1; };
    bool unplaced = *(short *)((byte *)header + 4) == NONE;
    s_location local_location;
    s_location const *location = (s_location const *)supplied_location;
    if (!location)
    {
        function_11bed0(&local_location, (point3f *)(object + 0x40));
        location = &local_location;
        if (*(short *)((byte *)&local_location + 4) == NONE)
            function_11bed0(&local_location, (point3f *)(object + 0x64));
    }
    if (*(short const *)((byte const *)location + 4) != NONE)
    {
        *(s_location *)(object + 0x28) = *location;
        *(short *)((byte *)header + 4) = *(short *)(object + 0x2c);
        *(dword *)(object + 4) &= ~0x40000;
    }
    else *(dword *)(object + 4) |= 0x40000;
    dword cluster_bits[16];
    dword *bits = 0;
    if ((bool)((*(dword *)(object + 4) >> 21) & 1))
    {
        long size = (*(long *)((byte *)g_4e0348 + 0x9c) + 31) >> 5;
        memset(cluster_bits, 0xff, size * sizeof(dword));
        bits = cluster_bits;
    }
    s_object_partition_record_ab record;
    function_b88e0(object_index, &record);
    s_cluster_partition *partition = TEST_FIELD_BIT(((s_object_view *)object)->flag9) ?
        (s_cluster_partition *)&g_partition_colliding_storage :
        (s_cluster_partition *)&g_partition_noncolliding_storage;
    bool overflow;
    function_1cac60(bits, partition, object_index, (long *)(object + 0x60),
        (point3f *)(object + 0x40), *(real *)(object + 0x4c), (s_partition_location *)(object + 0x28),
        sizeof(record), &record, &overflow);
    *(dword *)(object + 4) |= 0x100;
    header->flags |= 0x40;
    function_bef30(object_index, 0, 1, 0, false);
    if (function_b6d60(object_index, (dword *)((byte *)g_4e6948 + 0x11b8)))
        function_b7290(object_index);
    else if (TEST_FIELD_BIT(((s_flags_b8600 *)(object + 4))->field_17) && !function_a7670(object_index))
        function_b8540(object_index);
    else function_b7300(object_index);
    if (unplaced && *(short *)((byte *)header + 4) != NONE)
    {
        function_146bf0();
        function_1c36f0(object_index, object_index);
        function_278f00();
        function_146bf0();
    }
}

long __stdcall function_c0230(long tag_index, long object_index, short node_index, long name, short value);
long looping_sound_new_attached(long tag_index, long object_index, short marker_index, long value);
long function_1766b0(long object_index, long tag_index, long unknown34, long unknown38, short unknown3c);
long function_17bab0(long object_index, short marker_index, long tag_index);

// @retail 0xbeca0
void __stdcall function_beca0(long object_index)
{
    byte *object = (byte *)OBJECT_GET(object_index);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    if (*(long *)(definition + 0x94) > 0)
    {
        byte *attachments = object + *(short *)(object + 0x11e);
        for (long i = 0; i < *(long *)(definition + 0x94); ++i)
        {
            byte *entry = *(byte **)(definition + 0x98) + i * 0x18;
            short kind = NONE;
            long index = kind;
            long tag = *(long *)(entry + 4);
            if (tag != NONE)
            {
                switch (*(dword *)entry)
                {
                case 0x4d475332: case 0x6c656e73: case 0x7464746c: kind = 5; break;
                case 0x65666665: kind = 2; break;
                case 0x636f6e74: kind = 3; break;
                case 0x6c736e64: kind = 1; break;
                case 0x6c696768: kind = 0; break;
                }
            }
            switch (kind)
            {
            case 0:
                index = function_c0230(tag, object_index, (short)i, *(long *)(entry + 0x10), *(word *)(entry + 0xc));
                if (index != NONE) *(dword *)(object + 4) |= 0x10;
                break;
            case 1:
                index = looping_sound_new_attached(tag, object_index, (short)i, *(long *)(entry + 0x10));
                if (index != NONE) *(dword *)(object + 4) |= 0x20;
                break;
            case 2:
                index = function_1766b0(object_index, tag, *(long *)(entry + 0x10), *(long *)(entry + 0x14), *(word *)(entry + 0xc));
                break;
            case 3: index = function_17bab0(object_index, (short)i, tag); break;
            }
            attachments[i * 8] = (byte)kind;
            *(long *)(attachments + i * 8 + 4) = index;
        }
    }
}


bool function_bf510(long object_index, s_location *location);
void function_108960(long object_index);
void function_bfcc0();
void function_c00a0();
void function_1c4a80(long object_index, bool position_unchanged, bool force);

// @retail 0xb6bb0
void function_b6bb0()
{
    struct s_object_flags
    {
        dword unknown00;
        dword unknown0 : 7;
        dword flag7 : 1;
        dword unknown8 : 24;
    };
    *(byte *)g_4de2f4 = 1;
    struct
    {
        byte *object;
        s_type_f1af8e iterator;
    } iteration;
    iteration.iterator.signature = 0x86868686;
    iteration.iterator.type_mask = NONE;
    iteration.iterator.flags = 0;
    iteration.iterator.index = 0;
    iteration.iterator.object_index = NONE;
    iteration.object = (byte *)function_baeb0(&iteration.iterator);
    while (iteration.object)
    {
        long index = iteration.iterator.object_index;
        if (*(long *)(iteration.object + 0x14) == NONE && !((bool)((s_object_flags *)iteration.object)->flag7))
        {
            s_location location;
            function_bf510(index, &location);
            function_b8600(index, (long)&location);
        }
        function_108960(index);
        iteration.object = (byte *)function_baeb0(&iteration.iterator);
    }
    function_bfcc0();
    function_c00a0();
}

bool __stdcall function_b7430(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, s_location const *location, bool update, bool physics, bool force, bool keep_partition);



int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_141590(transform4x3f const *in, transform4x3f *out);

// @retail 0xb92d0
void function_b92d0(long object_index, s_object_marker const *old_parent, transform4x3f const *new_parent)
{
    byte *object = (byte *)OBJECT_GET(object_index);
    transform4x3f local, inverse;
    function_1420f0(&local, (point3f *)(object + 0x64), (vector3f *)(object + 0x70),
        (vector3f *)(object + 0x7c));
    function_141590(&local, &inverse);
    function_142a60(&inverse, &old_parent->matrix, &inverse);
    function_141590(&inverse, &inverse);
    function_142a60(new_parent, &inverse, &local);
    vector3f forward = local.forward, up = local.up;
    function_b91d0(object_index, &forward, &up);
    function_b7430(object_index, &local.position, &forward, &up, 0, true, true, false, false);
}




#include "unknown_1cafc0.h"
#include "effects.h"
void function_108a40(long *placement);
long __stdcall function_bc280(short size);
void __stdcall function_bc300(long index);
bool function_bc380(long index, long field, long size, long alignment);
bool function_bf9a0(long index, long *render_model, long *arg_0e6cbc_2);
void function_b8b70(long index);
bool object_dispatch_create(long index, long placement, long failed);
void object_dispatch_finish_creation(long index);
void object_dispatch_remove(long index);
void function_ba540(long index, string_handle name);
void __stdcall function_ba590(long index, dword mask);
void __stdcall function_be240(long index, dword colors, color3f const *values);
long function_d5b60(long index);
bool __stdcall function_be8e0(long index);
void function_ba3d0(long index);
void function_be1d0(long index);
void __stdcall function_b8890(long index);
void function_be690(long index);
void function_2e1e0(long index);
void object_list_insert(long index);
long function_b8820();
bool havok_object_type_can_have_component(long tag);



struct s_entity_info
{
	long field0;
	long definition_index;
	union
	{
		long field8;
		byte byte8;
		byte unknown08[8];
	};
	union
	{
		long identifier;
		byte vehicle_data[0x20];
	};
};
void function_bf090(long index);
void function_b7740(long index, vector3f const *linear, vector3f const *angular, bool skip_update);
void function_141590(transform4x3f const *in, transform4x3f *out);
#include "object_markers.h"
struct s_type_4f0dcc;
struct s_scenario_block;
struct s_object_placement_data;
struct s_effect_object_placement;
long __stdcall function_b7b40(void *creation);
extern long *g_4de2d0;
bool function_d5060(s_scenario_block *palette, volatile long type, long index,
    s_type_4f0dcc const *datum, bool enabled, s_object_placement_data *data);
bool function_a7640(s_effect_object_placement *data);
void object_dispatch_place(long index, long datum);
void function_bf050(long object_index, short name_index);
void __stdcall function_a7870(long index);
long function_bf760(long const *unique_id);
void function_a7ab0(long index);

// @retail 0xbf0f0
long function_bf0f0(s_type_4f0dcc const *datum, long type, long index,
    s_scenario_block *palette, bool test_placement, bool enabled)
{
    long result = NONE;
    short name;
    byte creation[0xc4];
    byte const *fields = (byte const *)datum;
    if (*(short *)fields == NONE) goto done;
    name = *(short *)(fields + 2);
    if (name != NONE && name >= 0 && name < 0x280 && g_4de2d0[name] != NONE) goto done;
    if (!function_d5060(palette, type, index, datum, enabled, (s_object_placement_data *)creation)) goto done;
    if (test_placement && !function_a7640((s_effect_object_placement *)creation)) goto done;
    result = function_b7b40(creation);
    if (result != NONE)
    {
        object_dispatch_place(result, (long)datum);
        if (*(short *)(fields + 2) != NONE) function_bf050(result, *(short *)(fields + 2));
        function_a7870(result);
    }
done:
    return result;
}

// score 26 (was 26): the name index parameter is read as a dword, retail reads it as a word, and the placement helper is called with one argument more
// @retail 0xbb670
long __stdcall function_bb670(short name_index, bool force)
{
    short remaining_name_index = name_index;
    long result = NONE;
    long existing = name_index >= 0 && name_index < 0x280 ? g_4de2d0[name_index] : NONE;
    if (force && existing != NONE)
    {
        s_object_header_view *header = OBJECT_HEADER_GET(existing);
        byte *object = (byte *)header->object;
        long type = header->type;
        function_bf090(existing);
        *(short *)(object + 0x1a) = NONE;
        *(long *)((byte *)g_4de2f4 + 0x18) += 1;
        object[0xaa] = (byte)type;
        object[0xab] = 2;
        *(short *)(object + 0xa8) = NONE;
        *(long *)(object + 0xa4) = *(long *)((byte *)g_4de2f4 + 0x18);
        remaining_name_index = *(short volatile const *)&name_index;
        object[0xb0] = 0;
    }
    else if (existing != NONE) return result;
    byte *scenario = (byte *)g_4e0350;
    byte *name = *(byte **)(scenario + 0x4c) + remaining_name_index * 0x24;
    short placement_index = *(short *)(name + 0x22);
    if (placement_index != NONE)
    {
        long type = *(short *)(name + 0x20);
        byte *definition = (byte *)g_468630[type];
        short palette_offset = *(short *)(definition + 0xc);
        s_scenario_block *palette = (s_scenario_block *)(scenario + palette_offset);
        short placement_size = *(short *)(definition + 0xe);
        short placement_offset = *(short *)(definition + 0xa);
        byte *placements = *(byte **)(scenario + placement_offset + 4);
        result = function_bf0f0((s_type_4f0dcc *)(placements + placement_index * placement_size),
            type, placement_index, palette, true, false);
    }
    return result;
}

// score 84 (was 84): the two fields of the definition are read in the other order and the -1 sentinel is kept in edi instead of ecx
// @retail 0xa73b0
long __stdcall function_a73b0(s_entity_info *info)
{
    long result = NONE;
    long type = *(short *)g_4e3b44[info->definition_index & 0xffff].bytes;
    byte *definition = (byte *)g_468630[type];
    long stride = *(short *)(definition + 0xe);
    short placement_offset = *(short *)(definition + 0xa), palette_offset = *(short *)(definition + 0xc);
    if (placement_offset != NONE && palette_offset != NONE)
    {
        byte *scenario = (byte *)g_4e0350;
        byte *placements = scenario + placement_offset;
        long index = info->field0;
        long clamped = index < 0 ? 0 : index > *(long *)placements - 1 ? *(long *)placements - 1 : index;
        if (clamped == index)
        {
            byte *datum = *(byte **)(placements + 4) + index * stride;
            long old_object = function_bf760((long const *)(datum + 0x28));
            if (old_object != NONE) function_a7ab0(old_object);
            definition = (byte *)g_468630[type];
            byte *palette = scenario + *(short *)(definition + 0xc);
            short palette_index = *(short *)datum;
            if (palette_index != NONE)
            {
                clamped = palette_index < 0 ? 0 : palette_index > *(long *)palette - 1 ? *(long *)palette - 1 : palette_index;
                if (clamped == palette_index)
                {
                    if (*(long *)(*(byte **)(palette + 4) + palette_index * 0x28 + 4) == info->definition_index)
                        result = function_bf0f0((s_type_4f0dcc *)datum, type, info->field0, (s_scenario_block *)palette, false, false);
                }
            }
        }
    }
    return result;
}

struct s_havok_component;
bool havok_component_any_rigid_body_active(s_havok_component *component);
bool havok_component_main_rigid_body_movable(s_havok_component *component);
void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
void havok_component_rigid_body_linear_velocity_get(s_havok_component *component, long body, vector3f *velocity);
void havok_component_rigid_body_angular_velocity_get(s_havok_component *component, long body, vector3f *velocity);
void function_1d96d0(long index);

// @retail 0xbc5e0
void __stdcall function_bc5e0(long index)
{
    s_object_view *object = OBJECT_GET(index);
    bool disabled;
    if (TEST_FIELD_BIT(object->physics_active))
    {
        s_havok_component *component = havok_component_get(object->havok_component_index);
        bool active = havok_component_any_rigid_body_active(component);
        if (!active)
        {
            if (!TEST_FIELD_BIT(object->physics_disabled)) goto motion_done;
        }
        else component->unknown04 &= ~0x10000;
        if (component->rigid_bodies.size > 0)
        {
            if (!TEST_FIELD_BIT(component->flag8) && havok_component_main_rigid_body_movable(component))
            {
                transform4x3f matrix;
                function_1d4360(component, &matrix);
                vector3f *forward = &matrix.forward, *up = &matrix.up;
                if (TEST_FIELD_BIT(component->flag1)) up = forward = 0;
                else function_b91d0(index, forward, up);
                function_b7430(index, &matrix.position, forward, up, 0, false, false, false, true);
                if (active)
                {
                    vector3f linear, angular;
                    vector3f *angular_pointer = 0;
                    havok_component_rigid_body_linear_velocity_get(component, 0, &linear);
                    if (!TEST_FIELD_BIT(component->flag1))
                    {
                        havok_component_rigid_body_angular_velocity_get(component, 0, &angular);
                        angular_pointer = &angular;
                    }
                    function_b7740(index, &linear, angular_pointer, false);
                }
                else function_b7740(index, g_4687a4, g_4687a4, false);
            }
            else function_b7740(index, g_4687a4, g_4687a4, false);
        }
    motion_done:
        disabled = !active;
        if (((byte *)g_4e6948)[0xc] != 4 && TEST_FIELD_BIT(object->flag11c0))
        {
            function_1d96d0(object->havok_component_index);
            ((byte *)object)[0xc1] &= ~8;
        }
    }
    else
    {
        long parent = object->parent_index;
        disabled = parent != NONE ?
            TEST_FIELD_BIT(OBJECT_GET(parent)->physics_disabled) : true;
    }
    object->physics_disabled = disabled ? 1 : 0;
}

PRIVATE __forceinline void object_attachment_point(transform4x3f const *matrix, point3f const *point, point3f *result)
{
    real x = point->x;
    real y = point->y;
    real z = point->z;

    real scale = matrix->scale;
    if (scale != 1.0f)
    {
        x *= scale;
        y *= scale;
        z *= scale;
    }
    result->x = matrix->forward.i * x + matrix->left.i * y + matrix->up.i * z + matrix->position.x;
    result->y = matrix->forward.j * x + matrix->left.j * y + matrix->up.j * z + matrix->position.y;
    result->z = matrix->forward.k * x + matrix->left.k * y + matrix->up.k * z + matrix->position.z;
}
void function_b8840(long index);
long function_baf80(long index);
void function_bba20(long index);

// @retail 0xb93b0
void __stdcall function_b93b0(long parent_index, volatile long index, volatile short node)
{
    s_record_pool *pool = g_4e0300;
    long offset = (index & 0xffff) * 12;
    byte *object = *(byte **)(pool->data + offset + 8);
    bool allowed = *(long *)(object + 0x14) == NONE && *(long *)(object + 0xc) == NONE;
    if (((1 << object[0xaa]) & 0x1c) && (object[0x12c] & 1) && *(long *)(object + 0x154) != parent_index) allowed = false;
    for (long ancestor = parent_index; ancestor != NONE; ancestor = *(long *)(*(byte **)(pool->data + (ancestor & 0xffff) * 12 + 8) + 0x14))
        if (ancestor == index) return;
    if (!allowed) return;
    s_object_header_view *header = (s_object_header_view *)(pool->data + offset);
    volatile bool was_active = (bool)((header->flags >> 2) & 1);
    byte *parent = (byte *)OBJECT_GET(parent_index);
    if (*(dword *)(object + 4) & 0x80)
    {
        *(dword *)(object + 4) &= ~0x80;
        if (!(*(dword *)(object + 4) & 0x100) && g_4de2f4 && *(byte *)g_4de2f4) function_b8600(index, 0);
    }
    function_b7300(index);
    if (*(dword *)(object + 4) & 0x100) function_b87b0(index);
    if ((object[0xc0] & 0x40) && header->type != 7) function_b8840(index);
    parent = (byte *)OBJECT_GET(parent_index);
    transform4x3f *nodes = (transform4x3f *)(parent + *(short *)(parent + 0x116));
    *(long *)(object + 0x14) = parent_index;
    short node_index = node;
    object[0x18] = (byte)node_index;
    *(long *)((byte *)OBJECT_GET(index) + 0xc) = *(long *)(parent + 0x10);
    *(long *)(parent + 0x10) = index;
    header->flags |= 0x80;
    *(long *)(object + 0x28) = NONE;
    *(short *)(object + 0x2c) = NONE;
    *(short *)(object + 0x2e) = g_4686c4;
    transform4x3f inverse;
    function_141590(&nodes[node_index], &inverse);
    point3f position;
    vector3f forward, up;
    object_attachment_point(&inverse, (point3f *)(object + 0x64), &position);
    matrix_transform_vector(&inverse, (vector3f *)(object + 0x70), &forward);
    matrix_transform_vector(&inverse, (vector3f *)(object + 0x7c), &up);
    function_b91d0(index, &forward, &up);
    function_b7430(index, &position, &forward, &up, 0, true, true, false, false);
    function_b9b90(index, true);
    if (function_b9d20(parent_index)) function_bef30(index, 0, 1, 0, false);
    function_bd020(index);
    header->flags |= 0x20;
    long root = function_baf80(index);
    s_object_header_view *root_header = OBJECT_HEADER_GET(root);
    short cluster = *(short *)((byte *)root_header + 4);
    if (!(root_header->flags & 1) && cluster != NONE &&
        (*(dword *)((byte *)g_4e6948 + 0x11b8 + (cluster >> 5) * 4) & (1 << (cluster & 31)))) function_b7290(root);
    if (was_active && !(root_header->flags & 4)) function_bba20(root);
    function_b7360(parent_index);
    function_b7360(index);
    if (*(long *)(object + 0xb4) == NONE)
    {
        function_146bf0(); function_1c36f0(index, index); function_278f00(); function_146bf0();
    }
}

void function_b92d0(long index, s_object_marker const *old_parent, transform4x3f const *new_parent);

// @retail 0xb8ee0
void __stdcall function_b8ee0(long parent_index, long parent_marker_name, long index, long child_marker_name)
{
    byte *object = (byte *)OBJECT_GET(index);
    function_b7300(index);
    if ((bool)((*(dword *)(object + 4) >> 8) & 1)) function_b87b0(index);
    s_object_marker parent_marker, name_9ae4a2;
    function_b8d30(parent_index, parent_marker_name, &parent_marker, 1, false);
    function_b8d30(index, child_marker_name, &name_9ae4a2, 1, false);
    if (child_marker_name != NONE && child_marker_name != 0)
        function_b92d0(index, &name_9ae4a2, &parent_marker.matrix);
    else
    {
        transform4x3f inverse;
        function_141590(&name_9ae4a2.node_matrix, &inverse);
        point3f position;
        vector3f forward, up;
        object_attachment_point(&inverse, &parent_marker.matrix.position, &position);
        matrix_transform_vector(&inverse, &parent_marker.matrix.forward, &forward);
        matrix_transform_vector(&inverse, &parent_marker.matrix.up, &up);
        function_b91d0(index, &forward, &up);
        function_b7430(index, &position, &forward, &up, 0, true, true, false, false);
    }
    function_b93b0(parent_index, index, (short)parent_marker.node_index);
}

void function_b7930(void *data, long tag, long object, s_effect_owner const *owner);
point3f *function_b9dd0(long object, point3f *result);

// @retail 0xbe760
void __stdcall function_be760(long index)
{
    byte *object = (byte *)OBJECT_GET(index);
    signed char variant_index = (signed char)object[0xb1];
    if (variant_index == NONE) return;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
    byte *variant = *(byte **)(model + 0x54) + variant_index * 0x38;
    for (long i = 0; i < *(long *)(variant + 0x1c); ++i)
    {
        byte *attachment = *(byte **)(variant + 0x20) + i * 0x10;
        long tag = *(long *)(attachment + 0xc);
        if (tag == NONE) continue;
        object = (byte *)OBJECT_GET(index);
        s_effect_owner owner;
        owner.unknown4 = *(long *)(object + 0xc8);
        owner.unknown0 = *(long *)(object + 0xc4);
        owner.unknown8 = *(short *)(object + 0xc2);
        byte creation[0xc4];
        function_b7930(creation, tag, index, &owner);
        function_b9dd0(index, (point3f *)(creation + 0x1c));
        long child = function_b7b40(creation);
        if (child != NONE)
        {
            byte *local_f86fb0_2 = (byte *)OBJECT_GET(child);
            function_b8ee0(index, *(long *)attachment, child, *(long *)(attachment + 4));
            *(dword *)(local_f86fb0_2 + 4) |= 0x4000000;
        }
    }
}
