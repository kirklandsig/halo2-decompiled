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
#include "bit_vector.h"
#include "object_tag_definitions.h"
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
	union
	{
		struct
		{
			word unknown0c0 : 6;
			word physics_active : 1;
			word flag7c0 : 1;
			word physics_disabled : 1;
			word : 2;
			word flag11c0 : 1;
			word : 4;
		};
		word flags0c0;
	};
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
	s_object_view *object = OBJECT_GET(object_index);
	long type = OBJECT_HEADER_GET(object_index)->type;

	if (((1 << type) & 0x1883) &&
		TEST_FIELD_BIT(object->physics_active) &&
		object->havok_component_index != NONE)
	{
		if (!disable)
		{
			// the pool's elements bound by reference: retail loads them before it scales the index
			s_havok_component *const &components = (s_havok_component *)g_51e9b8->data;
			havok_component_rigid_bodies_activate(&components[object->havok_component_index & 0xffff]);
		}
	}
	else if (disable)
		object->physics_disabled = true;
	if (!disable)
	{
		object->physics_disabled = false;
		function_b7360(object_index);
	}
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
void function_caf60(long unit_index, point3f *position);
short function_b8a80(long object_index, s_object_link_iterator *iterator);
short function_b8b20(s_object_link_iterator *iterator);
real function_c8880(long unit_index, short field_240);

static __forceinline long object_query_next_ab(s_record_pool *data, long index)
{
    return data_datum_index(data, function_16bc00(data, index == NONE ? 0 : (index & 0xffff) + 1));
}

// @retail 0xbba80
bool function_bba80(long object_index)
{
    bool result = false;
    {
        long root = NONE;
        long index = object_index;
        while (index != NONE)
        {
            root = index;
            index = *(long *)((byte *)OBJECT_GET(index) + 0x14);
        }
        s_object_header_view *header = OBJECT_HEADER_GET(root);
        byte *root_object = (byte *)header->object;
        if (!(((byte *)header)[2] & 1) || !(bool)((*(dword *)(root_object + 4) >> 8) & 1) || (bool)((*(dword *)(root_object + 4) >> 18) & 1))
            goto done;
        s_object_link_iterator links;
        dword const *visible_clusters = (dword const *)((byte *)g_4e6948 + 0x1138);
        short cluster = function_b8a80(root, &links);
        while (cluster != NONE)
        {
            if (bit_vector_test(visible_clusters, cluster))
            {
                byte *object = (byte *)OBJECT_GET(object_index);
                s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
                s_object_definition *const &kept_definition = definition;
                long model_index = kept_definition->model_tag_index;
                real radius_squared = *(real *)(object + 0x3c) * *(real *)(object + 0x3c);
                real range_squared;
                if (model_index != NONE)
                {
                    byte *model = g_4e3b44[model_index & 0xffff].bytes;
                    byte *const &kept_model = model;
                    real range = *(real *)(kept_model + 0x28);
                    if (range > 0.0f)
                    {
                        range *= 1.2f;
                        range_squared = range * range;
                    }
                    else
                        range_squared = 1600.0f;
                }
                else
                    range_squared = 0.0f;
                long player_index = object_query_next_ab(g_4e8c24, NONE);
                while (player_index != NONE)
                {
                    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
                    long unit_index = *(long *)(player + 0x2c);
                    if (unit_index != NONE)
                    {
                        point3f position;
                        function_caf60(unit_index, &position);
                        point3f *centre = (point3f *)(object + 0x30);
                        vector3f offset;
                        offset.i = centre->x - position.x;
                        offset.j = centre->y - position.y;
                        offset.k = centre->z - position.z;
                        real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
                        byte *unit = (byte *)OBJECT_GET(*(long *)(player + 0x2c));
                        long zoom_index = *(signed char *)(unit + 0x240);
                        real zoom = function_c8880(*(long *)(player + 0x2c), zoom_index);
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
                            real distance = function_30bf0(&offset);
                            real angle = (real)atan2((double)*(real *)(object + 0x3c), (double)distance) + 0.7853981852531433f;
                            if (offset.k * forward->k + offset.j * forward->j + offset.i * forward->i > cos((double)angle))
                            {
                                result = true;
                                goto done;
                            }
                        }
                    }
                    player_index = object_query_next_ab(g_4e8c24, player_index);
                }
                break;
            }
            cluster = function_b8b20(&links);
        }
    }
done:
    return result;
}


bool __stdcall function_1d44f0(s_havok_component *component, point3f *center, real *radius);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long unknown);

static __forceinline transform4x3f const *object_node_matrix_bdef0(byte const *object)
{
    return (transform4x3f const *)(object + *(short const *)(object + 0x116));
}

// score 53 (was 53): the component lookup is written as the argument of the physics query and
// the node matrix comes through a small helper on the fresh object lookup: the lookup
// is then index-first between the pushes, and the scale is reloaded for each product,
// as in retail. Left: the secondary sphere's sum order and which callee-saved register
// holds the definition.
// Left at 53: the secondary block (retail re-reads the definition's secondary radius from memory for the product and
// loads the primary centre first in each sum), the three-dword centre copy's store order, the movement test's i and j
// difference registers (retail k, i, j; ours k, j, i) and the `return false` exit, which retail lays out after the
// `return true` epilogue (the radius test falls through into the 0xb87b0 call). M3 lane 11 r170 (h2m fast with
// 0xbc820, 0xbd020, 0xb8600, 0xb87b0, 0x1d44f0, all MATCH; control 53): the secondary radius read at each use 66 (retail's
// re-read and xmm0, but the definition and header-offset registers swap); 0-16 dead locals at the top of that form 66
// (176 at 1 and 9) and of the body 53 (160 at 1), so these orders are not symbol-count ties; the movement test as
// `if (!(... || ...)) return false;` 53 (same code); the delta through the existing point-difference helper with
// the squared length through the existing length helper 53, the length helper alone 53 (same code); the delta
// assigned k, i, j 55, j, i, k 53, i, k, j 55; a single-exit `moved` result 59 (the return-false block stays inline);
// `<= && ==` with an early return 67 (739 B). Diagnostic (not keepable): the return false behind a label at the end 53.
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
        from_physics = function_1d44f0(havok_component_get(component_index), (point3f *)(object + 0x30), (real *)(object + 0x3c));
    }
    if (!from_physics)
    {
        transform4x3f const *matrix = object_node_matrix_bdef0((byte const *)OBJECT_GET(object_index));
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
    point3f *primary_center = (point3f *)(object + 0x30);
    point3f *secondary_center = (point3f *)(object + 0x50);
    real secondary_radius = *(real const *)(definition + 0x20);
    if (secondary_radius > 0.0f)
    {
        secondary_center->x = primary_center->x + *(real const *)(definition + 0x24);
        secondary_center->y = primary_center->y + *(real const *)(definition + 0x28);
        secondary_center->z = primary_center->z + *(real const *)(definition + 0x2c);
        *(real *)(object + 0x5c) = *(real *)(object + 0xa0) * secondary_radius;
    }
    else
    {
        *secondary_center = *primary_center;
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
            *(real *)(object + 0x4c) != radius)
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
    real k = up->i * forward->j - up->j * forward->i;
    real j = up->k * forward->i - up->i * forward->k;
    real i = up->j * forward->k - up->k * forward->j;
    out->left.i = i;
    out->left.j = j;
    out->left.k = k;
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

// @retail 0xb98e0
void __stdcall function_b98e0(long object_index, transform4x3f const *matrix)
{
    s_object_view *object = OBJECT_GET(object_index);
    transform4x3f translation;
    b98e0_translation(&translation, (point3f *)((byte *)object + 0x64));
    transform4x3f orientation;
    b98e0_basis(&orientation, (vector3f *)((byte *)object + 0x70), (vector3f *)((byte *)object + 0x7c));
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
        s_object_header_view *header = OBJECT_HEADER_GET(index);
        byte flags = OBJECT_HEADER_GET(index)->flags;
        if (flags & 1)
        {
            byte *current = (byte *)OBJECT_GET(index);
            header->flags = flags & ~5;
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
bool function_bc380(long index, long field, short size, long alignment);
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
bool function_d5060(s_scenario_block *palette, long type, long index,
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

/* makes the scenario object a name refers to (forced: resets the existing
   object's placement first; otherwise nothing when it already exists) and
   returns its index; NONE when the name has no placement. The palette offset
   is read through the type's definition, and the definition is read again
   through the name entry's type for the datum size and offset, so the first
   read and the second share one load with a register copy. */
// @retail 0xbb670
long __stdcall function_bb670(short name_index, bool force)
{
    long existing = name_index >= 0 && name_index < 0x280 ? g_4de2d0[name_index] : NONE;
    long result = NONE;
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
        object[0xb0] = 0;
    }
    else if (existing != NONE) return result;
    byte *scenario = (byte *)g_4e0350;
    byte *name = *(byte **)(scenario + 0x4c) + name_index * 0x24;
    short placement_index = *(short *)(name + 0x22);
    if (placement_index != NONE)
    {
        long type = *(short *)(name + 0x20);
        long palette_offset = *(short *)((byte *)g_468630[type] + 0xc);
        byte *definition = (byte *)g_468630[*(short *)(name + 0x20)];
        long placement_size = *(short *)(definition + 0xe);
        long placement_offset = *(short *)(definition + 0xa);
        byte *placements = *(byte **)(scenario + placement_offset + 4);
        byte *datum = placements + placement_index * placement_size;
        result = function_bf0f0((s_type_4f0dcc *)datum,
            type, placement_index, (s_scenario_block *)(scenario + palette_offset), true, false);
    }
    return result;
}

// @retail 0xa73b0
long __stdcall function_a73b0(s_entity_info *info)
{
    long type = *(short *)g_4e3b44[info->definition_index & 0xffff].bytes;
    byte *definition = (byte *)g_468630[type];
    long result = NONE;
    if (*(short *)(definition + 0xa) != NONE && *(short *)(definition + 0xc) != NONE)
    {
        byte *scenario = (byte *)g_4e0350;
        byte *placements = scenario + *(short *)((byte *)g_468630[type] + 0xa);
        long stride = *(short *)((byte *)g_468630[type] + 0xe);
        long const &index = info->field0;
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
                    {
                        long placement_index = info->field0;
                        result = function_bf0f0((s_type_4f0dcc *)datum, type, placement_index, (s_scenario_block *)palette, false, false);
                    }
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
    word *flags = &object->flags0c0;
    if (disabled) *flags |= 0x100;
    else *flags &= ~0x100;
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

transform4x3f *function_b8bd0(long object_index, short node_index);
void function_0bfe20(dword *flags, long bit, bool value);

// score 192 (was 192; 285 before m3 lane 7 r195): the three volatiles are gone. Left: the parent's node-matrix base (retail takes it ahead of the
// link stores and indexes it at the 0x141590 call; with a local for it there the count is 166, a one-use local
// evaluated early, so match-only), the first vector rotation (retail reads the object's forward from the object again
// for each row, ours copies its three components to frame slots first, 16 bytes), the table pointer's and the
// parent's frame homes swapped ([esp+0x14] / [esp+0x18]) with the parent load's base and index the other way round,
// the up vector's two first loads swapped, and the root's cluster word (retail reads it from the header at both
// uses, ours once).
// Earlier at 285 (was 285): the first difference is still the ancestry loop's exit at +0x76 (retail's shared epilogue);
// MiniMax A (11-19): no new form, no lower score (285): the pool's data pointer is still in EDI here and in EBX in retail, and the unit's flag byte is still read through a 32-bit copy instead of AL, so every later instruction inherits that difference; the body is unchanged. BLOCKED-BY 0xb8bd0 and 0x142570.
// behind it retail re-reads the pool data after the walk and takes the parent, the header and the node-matrix base
// from it. Round 199 (m3 lane 5, fast; aligned / register-masked counts, the body 215 / 152): `index` and `node` not
// volatile with the object, walk and header through OBJECT_GET/OBJECT_HEADER_GET 315 (1202 bytes, 112 / 84); plus the
// bit 7/8/6 tests by shift and the type test as `!((1 << type) & 0x80)` 308 (1217 bytes, 76 masked); the bit tests through
// the file's flag7/flag8/physics_active bitfields instead 304 (1222 bytes, 73 masked); plus one node-matrix pointer taken
// before the link stores 303 (1252 bytes); plus a node-matrices base before the stores indexed at the 0x141590 call 303
// (1249 bytes, 110 / 56: retail's link-store block and registers after the walk; a one-use base ahead of stores);
// that with `was_active` volatile 300 (1253 bytes, 61 masked); with the clear and inner bit-8 test as dword mask/shift
// 303 (same code); the clear in a pointer-taking inline helper 303 (same code: retail's reload of the flag word before
// the clear is not a CSE boundary); round 189's 245 chain re-measured 308 (1244 bytes). Diagnostics: the first vector
// rotated with direct field reads 376 (1153 bytes), through a temporary 330 (frame 0x70).
// MiniMax D (11-17): no new form, no lower score (285): the body is still four bytes longer than retail's 1236 and
// the head is retail's to +0x76, where the ancestry loop's match jump reaches this build's own epilogue and retail
// jumps to its shared one; the marker pointer retail inlines belongs to 0xb8bd0 and the header's field read to
// 0x142570, both /Ob1 bodies in files this lane may not edit. BLOCKED-BY 0xb8bd0, 0x142570.
// MiniMax R (11-16): no new form, no lower score (285): re-measured in this round's nine-address set, the body is still four bytes longer than retail's 1236 and the head is retail's to +0x76; the ancestry walk's record read comes out in EDX where retail keeps EBX and adds the header's field at EBP+4, the object word's bit-7 test comes out as a shift and a mask on ECX where retail shifts the word in EDX by seven and masks it, and the object's byte lands in CL at +0x13 where retail keeps it in AL.
// MiniMax C (11-16): no new form, no lower score (285): the head is retail's to +0x76, where the ancestry loop jumps to this build's own epilogue and retail jumps to its shared one; the body is still four bytes longer than retail's 1236.
// MiniMax I (11-16): no new form, no lower score (285); the bit-7 test of the object word written in retail's
// shift form (">> 7" and "& 1") compiles to the same sign test as the mask, so the body is unchanged: the first
// difference is still the ancestry loop's jump to our own epilogue at +0x76 where retail jumps to its shared one at
// 0xb987a, and retail still inlines the marker pointer's helper, which needs 0xb8bd0's file flag.
// MiniMax H (11-14): no new form, no lower score (285); the two state bits of the object word were written as
// shifts (">> 7" and ">> 8" instead of the masks 0x80 and 0x100), which is the form retail uses, but the score stayed
// 285 and the body stayed four bytes longer than retail's 1236, so the shift form was reverted; the first difference is
// still at +0x76, where retail keeps the pool pointer in ebx and the index in ebp and reloads the pool data into ebx,
// while ours reloads it into edx and computes the ancestry index again.
// MiniMax H (11-11): no new form, no lower score (285); retail still reaches one shared epilogue at 0xb987a from the ancestry-loop match, the !allowed test and the later branch where ours duplicates it, so the first difference is still the jump target at +0x76, and retail still inlines the marker pointer's helper, which needs that other file's /Ob1 flag. BLOCKED-BY 0xb8bd0.
// MiniMax E (11-10, run 73): no new form, no lower score (285); retail reaches one shared epilogue at 0xb987a from the ancestry-loop match, the !allowed test and the later branch where we duplicate it, so the first difference is still the jump target at +0x76, and retail inlines function_b8bd0's marker pointer, which needs that other file's /Ob1 flag.
// MiniMax D (11-11): no new form, no lower score (285): the ancestry loop's match jump still targets our own epilogue where retail jumps to its shared one at 0xb987a, and retail still inlines the marker pointer's helper from a file this lane may not edit.
// MiniMax D (11-10): no new form, no lower score (285): the header's activity bit as a byte test gives 281
// (four under the gate, so the body stands) and the ancestry match as a flag with a break gives 295; retail's
// one shared epilogue at 0xb987a and the marker pointer it inlines still need the other file's /Ob1 flag.
// MiniMax G (11-11): no new form, no lower score (285): the first difference is at +0x76, in the branch that reads the object's field at +0x44 - retail keeps the object base in ebx and the index in ebp where ours holds them the other way round; our body is three bytes longer than retail's 1236.
// MiniMax S (11-10): no new form, no lower score (285): retail reaches one shared epilogue at 0xb987a from the ancestry-loop match, the !allowed test and the later branch where ours duplicates it, so the first difference is still the jump target at +0x76, and retail inlines function_b8bd0's marker pointer, which needs that other file's /Ob1 flag.
// MiniMax E (11-10): no new form, no lower score (285): dropping the volatile from the activity flag reproduces
// retail's eight-bit shift of the header's flags byte, but the pool pointer then moves out of ebx and the score
// rises to 289; writing the same bit as `(header->flags & 4) != 0` gives the same 289, and the compiler rewrites
// both shift-form bit tests back into a sign test and a mask test.
// MiniMax C (11-09): no new form, no lower score (285): retail reaches one shared epilogue at 0xb987a from the
// ancestry-loop match, the !allowed test and the later branch, ours duplicates it, so the first difference is
// still the jump target at +0x76; retail also inlines function_b8bd0's marker pointer, and that file is /Ob1
// and outside this lane, so LTCG keeps our call.
// MiniMax I (11-08): no new form, no lower score (285): the shared epilogue at retail's 0xb987a and the
// marker pointer retail inlines (that other file is /Ob1, so LTCG keeps our call) still leave the first
// difference at +0x76.
// MiniMax R (11-15): no new form, no lower score (285): retail has one shared epilogue at 0xb987a reached from the ancestry loop and this build duplicates it, so the first difference is the jump target at +0x76.
// MiniMax I (11-15): no new form, no lower score (285): the shared epilogue at retail's 0xb987a is still not reached here, and the marker pointer retail inlines belongs to 0xb8bd0 in another file, which is /Ob1.
// MiniMax R (11-16): no new form, no lower score (285): the shared exit at retail's 0xb987a is still not reached here, so the first difference is the jump target at +0x76, and the marker pointer retail inlines belongs to 0xb8bd0 in another file, which is /Ob1.
// MiniMax I (11-17): no new form, no lower score (285): the shared epilogue at retail's 0xb987a is still not
// reached here, so the first difference is the jump target at +0x76; the marker pointer retail inlines belongs to
// 0xb8bd0 in another file, which is /Ob1, so LTCG keeps the call here and it cannot be changed from this lane.
// MiniMax E (11-17): no new form, no lower score (285): the permuter's best variant (283) came from an argument swap and a constant local, which the rules rule out, so the body stands.
// MiniMax H (11-18): at 285 (was 285): no new form, no lower score: the prologue and the frame now agree with
// retail's (the index read before the frame, the 0x64 frame, the pool in ebx and the pool's data in edi), and the
// first difference is still the jump target at +0x76, retail's shared epilogue at 0xb987a against our duplicated
// one; behind it retail keeps the pool in ebx and the object's data in edi where this build reloads the pool's
// data into edx and the index into ecx. The permuter's best variant (283) swaps the arguments of 0x141590 and
// 0xb8bd0 and of 0xb9b90, turns one of 0xb7430's true arguments into false, and adds a volatile pointer and a
// constant local, so it is wrong as well as under the gate and was restored. Retail still inlines the marker
// pointer of 0xb8bd0, whose file is /Ob1 and outside this lane.
// Earlier 285 (was 285): retail has one shared epilogue at 0xb987a, reached from the ancestry-loop
// match, the !allowed test and the later branch; ours duplicates it, so the first difference is the
// jump target at +0x76. A shared `goto done` gives retail's epilogue but shrinks our frame to 0x60
// (307; 308 with retail's shift-form flag tests), a flag with one `if` gives 323, and the byte
// activity bit `(header->flags & 4) != 0` gives retail's `shr al, 2; and al, 1` at 281 (a
// four-point gain, under the gate, so not kept). Retail also inlines function_b8bd0's marker
// pointer; that file is /Ob1, so LTCG keeps our call and it cannot be changed here. This round:
// the shared exit together with the byte activity bit is 308, and marking the declaration of
// function_b8bd0 in this file __forceinline changes nothing (the call stays).
// MiniMax S (11-06): no new form, no lower score (285): the pool and permission registers and the 0xb8bd0
// inline boundary still differ, and retail's inlined marker pointer needs that other file's /Ob1 flag.
// MiniMax E (11-05): no new form, no lower score; no new form for the shared epilogue or for the inlined
// 0xb8bd0 marker pointer (that file is /Ob1).
// MiniMax H (11-04): no new form, no lower score; the body still differs from its first instruction.
// Earlier: retail has one shared epilogue at 0xb987a, reached from the ancestry-loop
// Another round (at 285, was 285): retail has one shared epilogue at 0xb987a, reached from the ancestry-loop
// match, the !allowed test and the later branch; ours duplicates it, so the first difference is the
// jump target at +0x76. This round: a `goto done` shared exit (both early exits) keeps the frame at
// 0x64 and does give retail's single epilogue, but the score stays 285 - retail re-derives
// pool->data over the pool pointer in ebx and reads the header byte through that copy, ours keeps
// the first copy in edi and re-derives a second one into edx; folding the loop's early exit into the
// allowed flag instead gives 340. The permuter found nothing below 285.
// Earlier: frame 0x64 and entry agree; accepted header table, node-query expansion and SIMD homes remain.
// Another round (at 285, was 285): frame 0x64 and entry agree; accepted header table, node-query expansion and SIMD homes remain.
// Round 189 (m3 lane 3): retail has no call to 0xb8bd0 here: it reads the parent's node-matrix base before the
// link stores (`movsx eax, word [edi + 0x116]` precedes the parent-index store) and indexes it by the node at the
// 0x141590 call, and takes the header and the re-fetched object from a fresh `g_4e0300->data` plus the entry offset
// (the offset is CSE'd, so `index` is not volatile). Best form 245 plain (1249 bytes): `index` not volatile, the
// header from OBJECT_HEADER_GET(index) defined with was_active before the parent, the bit-7/8/6 tests in shift
// form, the type test as `!((1 << type) & 0x80)`, and `byte *node_parent = OBJECT_GET(parent_index);
// transform4x3f *node_matrices = (transform4x3f *)(node_parent + *(short *)(node_parent + 0x116));` before the
// stores with `node_matrices + node_index` at the call. Not kept: node_matrices is a one-use local evaluated ahead
// of stores, and without it the rest scores 304 (b8bd0 at the use) / 317 (index volatile again). Steps (fast): node
// base before the stores 350 (1283 bytes), the whole node matrix there 349, the 0xb8bd0 call moved there 285; with
// the child link through `g_4e0300->data + offset` 354, plus that header 341; non-volatile index + macro header
// 296 (1236 bytes), without the node base 320; on 296 the bit-7/8 shift tests 302, bit-6 shift 296, type mask
// 304, all three 253; header and was_active before the parent 245 (header first alone 253). On 245/253: object
// and loop through OBJECT_GET 268/276, object through OBJECT_GET with pool kept 304/287, object through the header
// view 245; cluster read at each use 285; node not volatile 290/300, also used directly 283; bit 6 through the
// physics_active bitfield 276; flag7/flag8 bitfields 245 (same code); the bit-7 test named before was_active 245;
// `allowed` first or offset before pool 253; node_parent as a two-use local with the offset read at the call 297;
// `parent` renamed (parent_object, new_parent, attach_parent) 245 (homes unchanged).
// Earlier: the frame (0x64) agrees; 1239 bytes against 1236. The entry now matches up to
// +0x76 (pool in EBX, -1 in EAX). Retail inlines the node matrix lookup (0xb8bd0's body, here called
// out of line because its file is /Ob1) and re-derives pool->data into EBX after the ancestry loop
// for both the parent and the header, where ours takes the header from the entry's pool->data.
// Round 27: the parent lookup written through pool->data, the activity bit carried in a byte, the
// shift-form bit-7 test and the type-below-eight test each regress (298/285/292/292 against 285);
// the original body is restored.
// Round 89: the node matrix through function_b8bd0 at its use, the link through the early parent
// pointer (retail's two parent lookups) kept, 305 -> 285 (a local for the matrix 285 too); on it the
// same fresh lookup written in place as 0xbdef0 does 350; the header from g_4e0300->data 298; index
// not volatile 319; the bit 7 and byte 0xc0 bit 6 tests by shift 290; the header and its active flag
// read before the parent 294.
// Earlier (at 305): retail keeps the pool pointer in EBX and re-derives pool->data from it,
// ours keeps the pool in ECX and pool->data in EDI, so retail holds -1 in EAX and the object's
// parent link in ECX where ours holds them in EBX and EAX.
// Retail also shift-tests the object's flag bit 7 where ours masks it, takes the parent pointer
// before the calls for the child-list link and again afterwards for the node matrix where ours
// takes it once, and keeps the attachment point's x, y and z in different SIMD registers.
// native point snapshot 385; native orientation snapshots 341; all snapshots 362; inverse
// workspace scope 319; spatial workspace names 319. byte permission 319; byte activity 319; both
// byte carriers 319; vector transforms before point 385; byte permission with vectors first 385.
// Ancestry before permission 335; long node index 335; guarded long cluster 324; up before forward
// 319; direct node array 323. Unsigned header offset 319; short permission 320; native location
// fields 330; pooled root reference 319; guarded native cluster 319. Native child 319; native
// parent 319; both native views 319; header flag pointer 402; object flag pointer 330. Positive
// acceptance scope 358; sibling-first sentinels 319; hidden Boolean shift 318; connection Boolean
// shift 318; unsigned masks 319; both Boolean shifts 323. Dead parent lookup removed 319;
// header-data base 336; node before link 319; separate output phases 319; bitset indexing 319
// (fast). Offset before pool 322; pool at use 325; computed type mask 330; six-byte sentinel fill
// 318; isolated byte-bit-six test 319; pool at use with unsigned offset 325. Permission scope 319;
// complete attachment scope 358; entry permission declaration 319; entry activity declaration 319;
// root-cluster scope 319 (fast).
// Round 214: signed-byte permission 319; unsigned-word permission 320; signed-word saved activity 319; native-long saved activity 319; word permission/activity homes 319.
// Isolated pool, offset, object, permission, ancestor, header, parent and cluster names each 319.
// Round 377: ancestry result at loop exit 300; with pool reads/unsigned offset/hidden shift/
// component shift 330/300/307/300; pool+offset/both shifts/all 330/307/332;
// 300 at 1236 bytes gains five only, rejected. Original body restored; pool/parent/SIMD homes remain.
// Round 244: complete rejection/positive/negative/acceptance branch results in bool
// 311/311/311/309; long 320/320/320/310 (fast control 305).
// Pool/NONE carriers stay different; original permission production restored.
// Round 234: native initial header, inline offset, ancestry getter, both 305/319/307/315;
// those four with consistent Boolean bit-seven/eight tests 312/327/314/324. All restored.
// Round 87: retail takes the parent before the calls (kept at +0x18 for the child link) and reads it
// again after them for the node matrices. That split with the node matrices from the object getter 354
// (1268 bytes); with the first parent read through `pool->data` 335 (pool in ebx and `pool->data` re-read
// after the walk as retail, but the flag spills); plus the file's bitfield tests (flag7, physics_active)
// and a type-mask test 339; plus offset declared first 335; plus the ancestor walk through the object
// getter 324 (aligned 159/159 against 154/162).
// Round 172 DL: Shared post-ancestry header table 321; native resident pose record
// 285; both 321. Native header activity bit with shared table/pose/both
// 323/281/323; table with both shifted flag tests 313; all forms 318.
// The four-count 281 gain fails the eight-count gate; original type layouts,
// all helper bodies, node query and complete body restored.
// Round 175: Existing point-producer native input captures, point/transform/both/rotation/
// point-rotation/translation/rotation-translation/all 366/348/384/339/376/360/371/416;
// all lose the shared sibling. Real header activity-bit producer with hidden/component/byte-active
// shift subsets 281/285/285/290/281/285/285/290; four-point gain rejected. On native activity,
// ancestry exit result alone/unsigned offset/uniform pool reads/both 290/290/299/299.
// Complete original header, private helper and body restored; pool-data, node lookup and SIMD homes remain.
// Round 191: complete entry/ancestry and accepted-refresh table graphs, reused byte/native
// receivers 321/329, separate byte/native phase tables 304/328; with the physical slot
// instead of the byte offset 320/320/303/303. All regress; original table reads restored.
// Round 270 M: ordinary saved activity on the retained node-query graph, alone/with
// ordinary index/ordinary node/both 289/314/289/317; the same four with the actual
// header activity member 289/314/289/317. Original qualifiers, header and body restored.
// Round 419: caller-side node-query force-inline declaration 285; with individual native
// position/forward/up input pointers, masks 1/2/3/4/5/6/7, 285/283/283/283/283/283/283.
// The two-point pointer forms are exact-match-only and rejected; original declaration and body
// restored. Accepted header-table reload, expanded node query and transform carriers remain.
// Round 194: Node-address/inverse phases after node, link and header writes 290/281/289/291/292/287; before first parent write 281/285. Native activity on inverse phases 278/286/283/280. On 278, hidden/connection/component shift subsets 285/281/278/284/285/281/284. Best 278 gains seven, rejected; original header and body restored.
// Round 205: Shared rotation/axis providers 332/332; shared point provider alone/rotation/axis 542/431/447;
// rotation/axis with the native activity member 315/330; rotation with the earlier inverse phase 334.
// All restored; frame, accepted table transport and node-query expansion remain.
// Round 30 (lane 11, fast = full here): after the ancestry loop retail derives the header from the
// fresh data load it uses for the parent (EBX, data spilled at +0x14), tests bits 7 and 8 of the object
// flags in shift form and computes the header bit in 8-bit arithmetic. A compile-only harness gives
// each piece: the header as `g_4e0300->data + offset`, `(header->flags & 4) != 0` (`shr al, 2; and
// al, 1`), and `>> 7`/`>> 8` shift tests. In the build: the header from g_4e0300->data 298 (pool moves
// to EDI at entry), with both shift tests 304, plus the byte bit 300; the byte bit alone 281 (4, under
// the gate), with both shift tests 290 (aligned 190 -> 174). All restored.
// Round 4 (lane 18, fast): the header's active bit as a byte bitfield of the header view 281 (retail's
// byte shr/and; four-point gain, not kept); on it the bit-7 flag test by shift 285, the header from
// OBJECT_HEADER_GET(index) 302, the type test as `!((1 << type) & 0x80)` 290, the active flag not
// volatile 289, the 0xc0 bit 6 test by shift 281. The slot_handler.h node-matrix helper can't be
// included here (its header view clashes with this file's). Restored.
// Round 205: Shared rotation/axis providers 332/332; shared point provider alone/rotation/axis 542/431/447;
// rotation/axis with the native activity member 315/330; rotation with the earlier inverse phase 334.
// All restored; frame, accepted table transport and node-query expansion remain.
// Round 247: both real header stores through native byte members, with saved activity,
// complete root flags and native cluster-word subsets, 285/281/285/281/285/281/285/281.
// Four-point lead is below the gate; original header type, byte stores, activity
// observation, root fields, node query, shared transforms and full body restored.
// Round 497: Native root-cluster reads at every use, crossed with native saved activity/common
// exit/closed spatial scope, 284/282/284/282/307/310/307/310. Three-point best is below the keep
// gate; complete original body restored. Accepted table reload, node-query expansion and SIMD homes
// remain.
// Round 54 (m3 lane 1, fast): a caller-side `__forceinline` declaration of 0xb8bd0 with the node matrix taken
// before the link stores 285 (1240 bytes: still called; retail computes the parent's node-table base before the
// parent link store, so its source took the matrix there); `node` not volatile 285; the 0xc0 bit 6 test through
// s_object_view::physics_active 285 (folds to the same byte test); flag bits 7/8 through s_object_view's
// flag7/flag8 303 (1222 bytes); index and node both not volatile 323 (1202 bytes).
// Round 46 (lane 14, fast): retail inlines 0xb8bd0 here and also 0x142570 (this file's attachment-point helper
// writes that body); both callees' files are /Ob1. Round 189's form with the node matrix taken through 0xb8bd0
// defined `inline`, before the link stores, 300. A data local at entry for the object and
// the ancestry walk, the parent and header from `pool->data` after it 302, header first 302, the walk alone 292;
// the object through the header view, header at entry 285.
// Round 231: complete point-result/rotation staging 284/284; per-row/all partial pairs
// 293/293; all row products 283; conditional scaling alone 285, with complete
// point-result/rotation staging 284/284. Every row-staging form loses the matched
// sibling; original two-caller helper and complete body restored.
// Round 118: 0xb8bd0 declared inline in its own file (diagnostic; retail inlines it here and reads the parent's
// node-matrix offset before the link stores) 342, and matched 0xbcc20/0x11b710 stop matching: BLOCKED-BY 0xb8bd0
// (its file is /Ob1); `allowed` as one && expression 317; the ancestry walk as a while loop 285; the two link tests
// in one || 293; the rest of the body nested under `if (allowed)` 307 (1213 bytes).
// Round 289: existing point-helper coordinate captures yxz/xzy/zyx/yzx/zxy
// 284/285/293/288/293; yxz/xzy/zyx with native activity 282/283/294. The shared sibling
// regresses 62/127/125/50/127/62/127/125; original helper and full body restored.
// M305: Terminal missing-component negative guard with shared-exit/bit-vector/ordinary-node subsets 285/285/291/291/285/285/291/291.
// Lane 16 r80 (fast): BLOCKED-BY 0xb8bd0 (inlined by retail, /Ob1 file) as before. At +0x76 retail builds the header
// from the fresh data load it uses for the parent; the header with the index recomputed 303; header declared before
// the parent 285 (identical); the parent read through `pool->data` 298 (1240 bytes); header and saved activity ahead of
// the parent 294 (1240 bytes); `node` read at each use with no node_index local 295 (1249 bytes). Restored.
// Round 147 (lane 5, fast): slot_handler.h's existing object_node_matrix/object_get can be used here once
// this file's header view is renamed (the two views clash); with that rename: the parent's node matrix
// through object_node_matrix(object_get(parent_index), node_index) at the call 342 (1269 bytes); the same
// pointer taken before the link stores, as retail reads it, 358 (1288 bytes); that with `index` not
// volatile 340 (1254 bytes). Without it: node_index read at entry 346 (1260 bytes); `allowed` as a long 297.
// Round 156 (lane 5, fast): BLOCKED-BY 0xb8bd0 again (matched 0xbcc20, an /O2 file, calls it out of line in retail
// too, so the inline here is not a file-flag effect). All three volatiles dropped 317 (1200 bytes); the root tail
// as an outer `if (!(root flags & 1))` with the cluster word read at each use inside 284 (the tail is then
// retail's, aligned 198 -> 151); on it `index` not volatile 324 (1201 bytes), the byte activity bit `(flags & 4)
// != 0` 282 (1237 bytes), and on 282 the inner bit-8 test by shift 284, the outer bit-7 test by shift 286, the
// type test as `!((1 << type) & 0x80)` 291, all four retail forms 298 (1254 bytes), the 0xc0 bit-6 test by
// shift 282. Best 282 is three under, not kept.
// Round 100 (m3 lane 9, fast; exact-aligned / register- and slot-masked instruction counts, main 190 / 131): the
// shared vector-rotate helper reading the vector at each use (retail re-reads the first vector per row) 338 (1182
// bytes, 282 / 212; sibling 0xb8ee0 lost); the first vector through the existing direct-read rotate helper 340
// (1277 bytes, 252 / 146); `index` not volatile 320 (1205 bytes, 179 / 117); plus the round 156 root tail and the
// byte activity bit 326 (1199 bytes, 151 / 93); plus `node` not volatile 325 (1200 bytes, 125 / 86: the
// structural lead); the tail and the byte bit on main's volatiles 282 (160 / 102); the 325 form plus the header
// from OBJECT_HEADER_GET(index) and the saved activity ahead of the parent, shift-form bit 7/8/6 tests and the
// type test as a mask 394 (1169 bytes, 152 / 102; 0xb8ee0 lost); plus the parent's node-matrix base read ahead of
// the link stores (0xb8bd0's body by hand, diagnostic) 303 at 1236 bytes (158 / 115). Nothing reaches 277; body
// unchanged. BLOCKED-BY 0xb8bd0 as before.
// MiniMax A (11-11): no new form, no lower score (285); the body is unchanged: the parent's node matrix still needs 0xb8bd0's retail body, which this lane may not edit.
// Round 174 (m3 lane 5, fast): the root tail's cluster test through the existing bit_vector_test (it replaces that helper's
// body written out; retail's exact `mov ecx, [g_4e6948]; sar eax, 5; test` sequence) with the cluster local 291 (1225 bytes:
// the attachment point's x/y homes reshuffle), with the cluster word read at each use 285 (1240 bytes; kept: same score, no
// hand-written helper), nested under an outer root-flags test 285 (same code). Re-measured on that base: `allowed` as one &&
// expression 317 (as listed), the byte activity bit 281. BLOCKED-BY 0xb8bd0 (ours: its file's /Ob1 keeps it out of line;
// `inline` there loses 0xbcc20/0x11b710) and 0x142570 (/Ob1, inlined by retail). No untried natural form is left.
// Round 244 (m3 lane 3, fast): 0xb8bd0's body moved into this file (diagnostic, restored) 348 (1272 bytes, first
// difference +0x0): LTCG then inlines it into 0x18c3b0 too, which stops matching, so retail's single inline site is not
// a same-file effect. Object, ancestry walk and header through OBJECT_GET/OBJECT_HEADER_GET with no pool local 330 (the
// parent and header then share one fresh data load as retail does, but the volatile index re-derives the offset); that
// with the byte activity bit `(header->flags & 4) != 0` 327; the ancestry walk alone through OBJECT_GET 328; the walk
// through OBJECT_GET with the header through OBJECT_HEADER_GET and `index` not volatile 324 (1203 bytes); the node
// copied into a long local 288 (1228 bytes). All restored.
// M3 lane 16 r150 (fast with this body's 15 matched callees and 0xb8ee0, all MATCH; control 285, aligned 215, register-
// masked 126). The parent's node-matrix base formed by hand ahead of the link stores on this body (0xb8bd0's body written
// out, diagnostic) 362 (1285 bytes, the head changes); `index` declared plain with volatile reads only at the first
// half's uses (diagnostic) 303 (1215 bytes, aligned 203: retail keeps `index` in ESI from the 0xb7430 setup on and
// re-reads it from the stack only before the link stores); the forward vector rotated through a private copy of the
// rotate that reads the vector at each use (diagnostic) 315 (1206 bytes: retail re-reads the forward vector per row and
// loads the up vector once, but its rows take k, j, then i where this build takes i, j, k), with its sums written up
// first 315 (same code); `allowed` initialised from the first two tests 317 (1246 bytes). Restored.
// M3 lane 10 r129 (fast with 0xb8ee0/0xb8bd0/0x141590/0x142570; base 285, 1240 B): a pool-data local read after the walk
// for both the parent and the header 318 (1224 B); the same local read from g_4e0300 300 (1228 B); only the header from
// g_4e0300->data 298 (1241 B); the `allowed` test ahead of the ancestry walk 314 (1238 B); the walk's match clearing
// `allowed` and breaking, one return after it 343 (1290 B). BLOCKED-BY 0xb8bd0 as before.
// M3 lane 12 r149 (fast with 0xb8ee0 and 0xb8bd0's matched callers; control 285): `inline` on 0xb8bd0's own definition
// 348 (1272 B) and it loses 0xbcc20, 0x11b710 and 0x18c3b0 (reverted); the node matrix through this file's existing
// node-matrix inline on a fresh parent lookup 348 (1272 B), on the early parent pointer 324 (1245 B); the ancestry walk
// as a while loop 285 (same code); the `allowed` exit ahead of the walk 314 (1238 B).
// M3 lane 7 r195 (fast with 0xb8ee0/0xb8bd0/0x141590/0x142570/0xbe760, all matched throughout; control 285, aligned 216):
// `node` not volatile 285 (same code); `was_active` not volatile 289; both 289; `index` not volatile 324; none volatile
// 316 (1201 B, aligned 153). Kept, 285 -> 192 (1254 B, aligned 104), none volatile and: the bit-7 clear through the
// existing 0xbfe20 (retail tests bit 7 on one flags load and clears it on a second, then tests bit 8 on the value it
// stored: the clear does not share the test's load), bit 7, the inner bit 8 and the word at +0xc0's bit 6 tested by
// shift, the header type as `(1 << type) & 0x80`, the parent's node matrix through the file's existing
// object_node_matrix_bdef0 on a fresh lookup at the 0x141590 call (0xb8bd0 is no longer called here, as in retail), and
// the parent read from `pool->data`. Lead, NOT kept (a one-use local evaluated ahead of the link stores): that
// helper's result in a `node_matrices` local ahead of the stores, indexed at the call, which is retail's order, 166
// (1252 B, aligned 67); from the early `parent` pointer 276 (1237 B). Steps on the way: object, walk
// and header through the macros with those forms 301 (aligned 141), plus the 0xbfe20 clear 230, plus the word shift
// 303, plus the node matrices 299 (aligned 98), all three 195 (1251 B), with the header declared first 194; the pool
// local for the object and walk 212, for the parent and header too 203 (1238 B, aligned 111: the head is then retail's
// to +0x76 and beyond); the 0xb8bd0 call kept at the 0x141590 call 293 / 301, taken ahead of the link stores 282 /
// 290; then one more named local (the node_index or the offset local main already had, or both) 166: the attachment
// point's x / y / z then take retail's XMM2 / XMM5 / XMM6 (XMM5 / XMM6 / XMM2 without, XMM5 / XMM2 / XMM6 on the
// macro form). With the helper at the call instead of the early local 192; through the existing 0xb8bd0 call there
// 306 (1211 B), through `parent` there 294 (1219 B). On 166: the header declared ahead of the parent 165 (a
// declaration move, one point); `parent` renamed
// a / zz, `header` renamed 203 on the 203 form (same code: the two frame homes do not follow names); a named local for
// the table pointer 330; the root tail's cluster test nested under the flags test 166 (same code), its second read
// through a fresh header lookup 164 (a mixed read, two points); the object's point and axes through a typed view 166
// (same code). Reading: retail's rows re-read [object + 0x70..0x78] nine times while the up vector's components sit
// in registers, which is what the shared rotate helper's three copies give when they get no register and are read
// back from their source; ours spills them to the frame instead. BLOCKED-BY 0x142570 as before (/Ob1, inlined by
// retail; this file's attachment-point helper writes that body).
// MATCH (r924 stand-in image) on top of the nested `if (allowed) { ... }`: `header` declared before `parent`, the parent's
// node matrices taken into a local ahead of the link stores (retail loads the parent's +0x116 there), and the bit-vector
// test reading the root header's unknown04 again (retail re-loads it after the NONE compare).
// M3 lane 1 r136 (fast with 0xb8ee0/0xbe760, both MATCH each run; control 192, lead 166): the forward vector through the
// existing in-place rotate helper (`&inverse.rotation`) 280 (1262 B: its aliasing copy branch stays, frame 0x70); a
// `data` local for `pool->data` after the walk, for the parent and the header, 332 (the parent load then takes retail's
// `[edi + ebx + 8]` order, but the local merges with the entry's data load and the head changes); the parent through
// `((s_object_header_view *)pool->data)[...].object` 192 (same code); the root tail through OBJECT_HEADER_GET(root) at each
// use 291; on the 166 lead the two vectors declared ahead of `position` 166 (same code). Reading: retail stores each forward
// row before the next one and reloads the object's components per row, so its rotate's output is treated as possibly
// aliasing the source (ours defers the three stores and spills the copies). Body unchanged.
// score 67 (was 192, r924 stand-in image): the body runs inside `if (allowed) { ... }` (one return at the end) instead
// of `if (!allowed) return;` before it; 1240 bytes (retail 1236). The BLOCKED-BY note above still describes the rest.
// @retail 0xb93b0
void __stdcall function_b93b0(long parent_index, long index, short node)
{
    s_record_pool *pool = g_4e0300;
    long offset = (index & 0xffff) * 12;
    byte *object = *(byte **)(pool->data + offset + 8);
    bool allowed = true;
    if (*(long *)(object + 0x14) != NONE) allowed = false;
    else if (*(long *)(object + 0xc) != NONE) allowed = false;
    if (((1 << object[0xaa]) & 0x1c) && (object[0x12c] & 1) && *(long *)(object + 0x154) != parent_index) allowed = false;
    for (long ancestor = parent_index; ancestor != NONE; ancestor = *(long *)(*(byte **)(pool->data + (ancestor & 0xffff) * 12 + 8) + 0x14))
        if (ancestor == index) return;
    if (allowed)
    {
        s_object_header_view *header = (s_object_header_view *)(pool->data + offset);
        byte *parent = *(byte **)(pool->data + (parent_index & 0xffff) * 12 + 8);
        bool was_active = (bool)((header->flags >> 2) & 1);
        if ((bool)((*(dword *)(object + 4) >> 7) & 1))
        {
            function_0bfe20((dword *)(object + 4), 7, false);
            if (!(bool)((*(dword *)(object + 4) >> 8) & 1) && g_4de2f4 && *(byte *)g_4de2f4) function_b8600(index, 0);
        }
        function_b7300(index);
        if ((bool)((*(dword *)(object + 4) >> 8) & 1)) function_b87b0(index);
        if ((bool)((*(word *)(object + 0xc0) >> 6) & 1) && !((1 << header->type) & 0x80)) function_b8840(index);
        transform4x3f const *parent_matrices = object_node_matrix_bdef0((byte const *)OBJECT_GET(parent_index));
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
        function_141590(&parent_matrices[node_index], &inverse);
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
        if (!(root_header->flags & 1) && root_header->unknown04 != NONE &&
            bit_vector_test((dword const *)((byte *)g_4e6948 + 0x11b8), OBJECT_HEADER_GET(root)->unknown04)) function_b7290(root);
        if (was_active && !(root_header->flags & 4)) function_bba20(root);
        function_b7360(parent_index);
        function_b7360(index);
        if (*(long *)(object + 0xb4) == NONE)
        {
            function_146bf0(); function_1c36f0(index, index); function_278f00(); function_146bf0();
        }
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
