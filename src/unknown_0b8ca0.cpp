// @flags /O2 /arch:SSE /Gr
/* object markers and object physics state (0xb8ca0-0xb9c60) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "unknown_16d180.h"
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
// Retail 0xb9a50
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


struct s_cluster_partition
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};
extern void *g_4de2d4, *g_4de2d8, *g_4de2dc;
extern void *g_4de2e0, *g_4de2e4, *g_4de2e8;
void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference);

// @retail 0xb87b0
void __stdcall function_b87b0(volatile long object_index)
{
    s_object_header_view *header = OBJECT_HEADER_GET(object_index);
    s_object_view *object = header->object;
    function_bef30(object_index, 1, 0, 0, false);
    s_cluster_partition partition;
    if (TEST_FIELD_BIT(object->flag9))
    {
        partition.cluster_first_data_references = (long *)g_4de2e0;
        partition.data_references = (s_record_pool *)g_4de2e4;
        partition.cluster_references = (s_record_pool *)g_4de2e8;
    }
    else
    {
        partition.cluster_first_data_references = (long *)g_4de2d4;
        partition.data_references = (s_record_pool *)g_4de2d8;
        partition.cluster_references = (s_record_pool *)g_4de2dc;
    }
    function_1cae40(&partition, object_index, (long *)((byte *)object + 0x60));
    object->flag8 = false;
    header->flags &= ~0x40;
}



// Disabled: the iterator persists a shared partition address after this function returns.
#if 0
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
    iterator->owner = (s_object_link_owner *)(TEST_FIELD_BIT(object->flag9) ? &g_4de2e0 : &g_4de2d4);
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

// Disabled: retail consumes a third collision input missing from all existing callers.
#if 0
#include "unknown_0259a0.h"

// Retail 0xbc1d0
bool __stdcall function_bc1d0(long object_index, point3f const *point, long ignore_unit_index)
{
    s_object_view *object = OBJECT_GET(object_index);
    vector3f displacement;
    s_collision_result_1697c0 collision;
    collision.unknown24 = NONE;
    displacement.i = *(real *)((byte *)object + 0x64) - point->x;
    displacement.j = *(real *)((byte *)object + 0x68) - point->y;
    displacement.k = *(real *)((byte *)object + 0x6c) - point->z;
    bool result = false;
    if (!function_1697c0(0x20800005, point, &displacement, object_index, ignore_unit_index, &collision)
        && *(short *)((byte *)object + 0x2c) != NONE)
        return true;
    if (collision.unknown24 != NONE)
    {
        function_b75a0(object_index, &collision.point, 0, 0, (s_location *)&collision.unknown24, false);
        result = true;
    }
    return result;
}
#endif



// Round 18: recheck after bef30 became exact and new queue callers were written.
bool object_or_parent_hidden(long object_index);
void function_b8b70(long object_index);
void __stdcall function_10a250(long object_index);
void function_bf090(long object_index);
void function_15b220(long object_index, long slot);
void __stdcall function_109400(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long object_index);
void function_10ace0(long object_index);

// Disabled: round 18 reactivation with exact bef30 still breaks matched 11a320; retain the complete 14-difference, 190/189-byte draft
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
    function_109400(object_index);
    function_bb950(object_index, false, NONE);
    function_a7a60(object_index);
    function_10ace0(object_index);
}
#endif

// Disabled: protected unit_object_type caller supplies two inputs; retail requires the third stack value.
#if 0

struct s_hit_a9260
{
    long object_index;
    long value04, value08, value0c;
    short value10;
    short unknown12;
    real amount;
    byte kind;
};
struct s_event_a9260
{
    long value00, value04, value08, value0c;
    short value10;
    short unknown12;
    real amount;
    byte kind;
    byte unknown19[3];
};
extern long g_4cef30;
void __stdcall function_b5a70(long player, long type, long count, long objects, long size, void const *data, long timeout);

// Retail 0xa9260
void __stdcall function_a9260(long unit_index, s_hit_a9260 const *hit, long value)
{
    if (!function_a76b0(unit_index, true)) return;
    if (hit->object_index == NONE)
    {
        if (hit->value0c == NONE) return;
    }
    else if (OBJECT_GET(hit->object_index)->field_x10a40f == NONE)
        return;
    long objects[2] = { unit_index, hit->object_index };
    s_event_a9260 event;
    memset(&event, 0, sizeof(event));
    event.value00 = value;
    event.value04 = hit->value04;
    event.value08 = hit->value08;
    event.value0c = hit->value0c;
    event.value10 = hit->value10;
    event.amount = hit->amount;
    event.kind = hit->kind;
    function_b5a70(NONE, 0x17, 2, (long)objects, sizeof(event), &event, g_4cef30);
}
#endif



// Disabled: protected 0x142a60 uses implicit registers without binding its formal inputs.
#if 0
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

// Retail 0xb98e0
void __stdcall function_b98e0(long object_index, transform4x3f const *matrix)
{
    s_object_view *object = OBJECT_GET(object_index);
    transform4x3f translation;
    translation.scale = 1.0f;
    translation.forward.i = 1.0f;
    translation.forward.j = 0.0f;
    translation.forward.k = 0.0f;
    translation.left.i = 0.0f;
    translation.left.j = 1.0f;
    translation.left.k = 0.0f;
    translation.up.i = 0.0f;
    translation.up.j = 0.0f;
    translation.up.k = 1.0f;
    translation.position = *(point3f *)((byte *)object + 0x64);
    transform4x3f orientation;
    orientation.scale = 1.0f;
    orientation.forward = *(vector3f *)((byte *)object + 0x70);
    vector3f const *forward = (vector3f *)((byte *)object + 0x70);
    vector3f const *up = (vector3f *)((byte *)object + 0x7c);
    orientation.left.i = up->j * forward->k - up->k * forward->j;
    orientation.left.j = up->k * forward->i - up->i * forward->k;
    orientation.left.k = up->i * forward->j - up->j * forward->i;
    orientation.up = *up;
    orientation.position.x = 0.0f;
    orientation.position.y = 0.0f;
    orientation.position.z = 0.0f;
    transform4x3f result;
    function_142a60(matrix, &translation, &result);
    function_142a60(&result, &orientation, &result);
    function_b75a0(object_index, &result.position, &result.forward, &result.up, 0, false);
}
#endif



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
    function_c0110();
    s_type_f1af8e iterator;
    iterator.type_mask = NONE;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    iterator.signature = 0x86868686;
    byte *object = (byte *)function_baeb0(&iterator);
    while (object)
    {
        long index = iterator.object_index;
        s_object_header_view *header = OBJECT_HEADER_GET(index);
        if (header->flags & 1)
        {
            byte *current = (byte *)OBJECT_GET(index);
            header->flags &= ~5;
            if ((1 << header->type) & 0x1883) function_1c38a0(index);
            if (*(dword *)(current + 4) & 0x4000) --*(short *)((byte *)g_4de2f4 + 4);
        }
        if (*(dword *)(object + 4) & 0x100) function_b87b0(index);
        function_1089d0(index);
        *(short *)((byte *)header + 4) = NONE;
        *(long *)(object + 0x28) = NONE;
        *(short *)(object + 0x2c) = NONE;
        *(short *)(object + 0x2e) = g_4686c4;
        object = (byte *)function_baeb0(&iterator);
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
        *(short *)((byte *)header + 4) = *(short const *)((byte const *)location + 4);
        *(dword *)(object + 4) &= ~0x40000;
    }
    else *(dword *)(object + 4) |= 0x40000;
    dword cluster_bits[16];
    dword *bits = 0;
    if (*(dword *)(object + 4) & 0x200000)
    {
        long size = (*(long *)((byte *)g_4e0348 + 0x9c) + 31) >> 5;
        memset(cluster_bits, 0xff, size * sizeof(dword));
        bits = cluster_bits;
    }
    s_object_partition_record_ab record;
    function_b88e0(object_index, &record);
    s_cluster_partition partition;
    if (*(dword *)(object + 4) & 0x200)
    {
        partition.cluster_first_data_references = (long *)g_4de2e0;
        partition.data_references = (s_record_pool *)g_4de2e4;
        partition.cluster_references = (s_record_pool *)g_4de2e8;
    }
    else
    {
        partition.cluster_first_data_references = (long *)g_4de2d4;
        partition.data_references = (s_record_pool *)g_4de2d8;
        partition.cluster_references = (s_record_pool *)g_4de2dc;
    }
    bool overflow;
    function_1cac60(bits, &partition, object_index, (long *)(object + 0x60),
        (point3f *)(object + 0x40), *(real *)(object + 0x4c), (s_partition_location *)(object + 0x28),
        sizeof(record), &record, &overflow);
    *(dword *)(object + 4) |= 0x100;
    header->flags |= 0x40;
    function_bef30(object_index, 0, 1, 0, false);
    if (function_b6d60(object_index, (dword *)((byte *)g_4e6948 + 0x11b8)))
        function_b7290(object_index);
    else if ((*(dword *)(object + 4) & 0x20000) && !function_a7670(object_index))
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
            long tag = *(long *)(entry + 4), index = NONE;
            short kind = NONE;
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
void function_1c4a80(long object_index, long position_unchanged, long force);

// @retail 0xb6bb0
void function_b6bb0()
{
    *(byte *)g_4de2f4 = 1;
    s_type_f1af8e iterator;
    iterator.signature = 0x86868686;
    iterator.type_mask = NONE;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    byte *object = (byte *)function_baeb0(&iterator);
    while (object)
    {
        long index = iterator.object_index;
        if (*(long *)(object + 0x14) == NONE && !(*(dword *)(object + 4) & 0x80))
        {
            s_location location;
            function_bf510(index, &location);
            function_b8600(index, (long)&location);
        }
        function_108960(index);
        object = (byte *)function_baeb0(&iterator);
    }
    function_bfcc0();
    function_c00a0();
}

bool __stdcall function_b7430(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, s_location const *location, bool update, bool physics, bool force, bool keep_partition);

// Disabled: two source forms break matched b75a0 and 14e970; retain closest complete body and original stub
#if 0
// Retail 0xb7430
bool __stdcall function_b7430(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, s_location const *location, bool update, bool physics, bool force, bool keep_partition)
{
    byte *object = (byte *)OBJECT_GET(object_index);
    bool placed = (*(dword *)(object + 4) & 0x100) != 0;
    bool result = true;
    if (placed && !keep_partition) function_b87b0(object_index);
    if (position)
    {
        if (*(long *)(object + 0x14) == NONE &&
            (position->x < -32768.0f || 32768.0f < position->x || position->y < -32768.0f ||
                32768.0f < position->y || position->z < -32768.0f || 32768.0f < position->z)) result = false;
        else
        {
            *(point3f *)(object + 0x64) = *position;
            long component = *(long *)((byte *)OBJECT_GET(object_index) + 0xd4);
            if (component != NONE) function_b58c0(component, 2);
        }
    }
    if (forward)
    {
        *(vector3f *)(object + 0x70) = *forward;
        *(vector3f *)(object + 0x7c) = *up;
        object = (byte *)OBJECT_GET(object_index);
        long component = *(long *)(object + 0xd4);
        if (component != NONE) function_b58c0(component, 4);
    }
    if (physics) function_1c4a80(object_index, position == 0, force);
    if (update) function_bd020(object_index);
    if (placed && !keep_partition) function_b8600(object_index, (long)location);
    return result;
}
#endif

// Disabled: protected matrix-composition helper 0x142a60 does not bind its formal inputs to retail registers.
#if 0
// Retail 0xb92d0
void function_b92d0(long object_index, transform4x3f const *old_parent, transform4x3f const *new_parent)
{
    byte *object = (byte *)OBJECT_GET(object_index);
    transform4x3f local, inverse;
    function_1420f0((point3f *)(object + 0x64), (vector3f *)(object + 0x70),
        (vector3f *)(object + 0x7c), &local);
    function_141590(&local, &inverse);
    function_142a60(&inverse, (transform4x3f const *)((byte const *)old_parent + 0x38), &inverse);
    function_141590(&inverse, &inverse);
    function_142a60(new_parent, &inverse, &local);
    vector3f forward = local.forward, up = local.up;
    function_b91d0(object_index, &forward, &up);
    function_b7430(object_index, &local.position, &forward, &up, 0, true, true, false, false);
}
#endif



#include "unknown_1cafc0.h"
#include "effects.h"
void function_108a40(long *placement);
long __stdcall function_bc280(short size);
void __stdcall function_bc300(long index);
bool function_bc380(long index, long field, long size, long alignment);
bool function_bf9a0(long index, long *render_model, long *arg_0e6cbc_2);
void function_b8b70(long index);
bool function_108a90(long index, long placement, long failed);
void function_108b80(long index);
void function_108bf0(long index);
void function_ba540(long index, string_handle name);
void __stdcall function_ba590(long index, dword mask);
void __stdcall function_be240(long index, dword colors, color3f const *values);
long function_d5b60(long index);
bool __stdcall function_be8e0(long index);
void function_ba3d0(long index);
void function_be1d0(long index);
void __stdcall function_b8890(long index);
void function_be690(long index);
long function_176780(long index, s_effect_owner const *owner, real scale_a,
    long tag, real scale_b, point3f const *point, vector3f const *direction);
void function_2e1e0(long index);
void function_109390(long index);
long function_b8820();
bool havok_object_type_can_have_component(long tag);

// Disabled: Two active creation forms change matched b8890 and 1c3770 through the protected 1d1540 register convention; retain the complete body and original stub.
#if 0
// Retail 0xb7b40
long __stdcall function_b7b40(void *creation)
{
    byte *placement = (byte *)creation;
    long result = NONE;
    bool failed = false;
    if (!(placement[0x18] & 0x10))
    {
        if (*(long *)placement == NONE) return result;
        function_108a40((long *)placement);
    }
    long tag = *(long *)placement;
    if (tag == NONE) return result;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    short type = *(short *)definition;
    byte *local_33f9fd = (byte *)g_468630[type];
    byte *model = 0;
    long model_tag = *(long *)(definition + 0x38);
    if (model_tag != NONE) model = g_4e3b44[model_tag & 0xffff].bytes;
    if ((1 << type) & 0x1883) function_146bf0();
    result = function_bc280(*(word *)(local_33f9fd + 8));
    if (result == NONE) return result;
    s_object_header_view *header = OBJECT_HEADER_GET(result);
    byte *object = (byte *)header->object;
    header->flags |= 8;
    header->type = (byte)type;
    *(long *)object = tag;
    if (placement[0xb] == 0xff)
    {
        *(long *)((byte *)g_4de2f4 + 0x18) += 1;
        object[0xaa] = (byte)type;
        object[0xab] = 2;
        *(short *)(object + 0xa8) = NONE;
        *(long *)(object + 0xa4) = *(long *)((byte *)g_4de2f4 + 0x18);
        *(short *)(object + 0x1a) = NONE;
        object[0xae] = (byte)g_4686c4;
    }
    else
    {
        *(long *)(object + 0xa4) = *(long *)(placement + 4);
        *(long *)(object + 0xa8) = *(long *)(placement + 8);
        *(short *)(object + 0x1a) = *(short *)(placement + 0x10);
        object[0xae] = placement[8];
    }
    *(point3f *)(object + 0x64) = *(point3f *)(placement + 0x1c);
    *(vector3f *)(object + 0x70) = *(vector3f *)(placement + 0x28);
    *(vector3f *)(object + 0x7c) = *(vector3f *)(placement + 0x34);
    *(vector3f *)(object + 0x88) = *(vector3f *)(placement + 0x40);
    *(vector3f *)(object + 0x94) = *(vector3f *)(placement + 0x4c);
    *(real *)(object + 0xa0) = *(real *)(placement + 0x58);
    if (placement[0x18] & 1) *(dword *)(object + 4) |= 0x400;
    else *(dword *)(object + 4) &= ~0x400;
    if (model && *(long *)(model + 0xc) != NONE) *(dword *)(object + 4) |= 0x200;
    else *(dword *)(object + 4) &= ~0x200;
    long render_model, arg_0e6cbc_2;
    if (function_bf9a0(result, &render_model, &arg_0e6cbc_2)) *(dword *)(object + 4) |= 0x80000000;
    else *(dword *)(object + 4) &= 0x7fffffff;
    header->unknown04 = NONE;
    *(long *)(object + 0x28) = NONE;
    *(short *)(object + 0x2c) = NONE;
    *(short *)(object + 0x2e) = g_4686c4;
    *(long *)(object + 0x60) = NONE;
    object[0xb0] = placement[0x14];
    *(long *)(object + 0x14) = *(long *)(object + 0xc) = *(long *)(object + 0x10) = NONE;
    *(short *)(object + 0xac) = NONE;
    object[0xaf] = object[0x108] = object[0x109] = 0xff;
    if (definition[2] & 1) *(dword *)(object + 4) |= 0x10000;
    if (*(dword *)(object + 4) & 1)
    {
        *(dword *)(object + 4) &= ~1;
        if (function_b9d20(result)) function_bef30(result, 0, 1, 0, false);
        function_b8b70(result);
    }
    *(short *)(object + 0xc2) = *(short *)(placement + 0x70);
    *(long *)(object + 0xc4) = *(long *)(placement + 0x68);
    *(long *)(object + 0xc8) = *(long *)(placement + 0x6c);
    object[0xb1] = 0xff;
    *(long *)(object + 0xcc) = NONE;
    *(short *)(object + 0xd0) = NONE;
    *(word *)(object + 0xc0) = 0;
    if (placement[0x18] & 8) object[0xc1] |= 1;
    else object[0xc1] &= ~1;
    *(long *)(object + 0xb4) = *(long *)(object + 0xd4) = NONE;
    object[0xd8] = 0;
    *(short *)(object + 0xe0) = *(short *)(placement + 0xb0);
    *(short *)(object + 0xe2) = *(short *)(placement + 0xb2);
    object[0xb3] = !((1 << header->type) & 0x80) && g_510c54->game_time >= 6 ? 6 : 0;
    long node_count = 1, region_count = 1, collision_count = 0;
    bool animated = false, orientations = false;
    if (model)
    {
        if (*(long *)(model + 0x78) >= 1) node_count = *(long *)(model + 0x78);
        if (*(long *)(model + 0x70) >= 1) region_count = *(long *)(model + 0x70);
        if (*(long *)(model + 0x60) > 0) collision_count = *(long *)(*(byte **)(model + 0x64) + 0xbc);
        if (*(long *)(model + 0x14) != NONE)
        {
            s_animation_state temporary;
            if (temporary.initialize(*(long *)(model + 0x14), model_tag, true))
            {
                animated = true;
                orientations = !((1 << type) & 0x7e0);
                if (((1 << type) & 0x380) && (definition[0xbc] & 4)) orientations = true;
            }
            temporary.channels_clear_partial();
        }
    }
    bool allocated = function_bc380(result, 0x11c, (word)(*(word *)(definition + 0x94) * 8), 0) &&
        function_bc380(result, 0x120, collision_count * 8, 0) &&
        function_bc380(result, 0x124, (word)(*(word *)(definition + 0xac) * 0x18), 0) &&
        function_bc380(result, 0x114, node_count * 0x34, 0) &&
        function_bc380(result, 0x118, region_count * 10, 0) &&
        function_bc380(result, 0x110, orientations ? node_count * 0x20 : 0, 4) &&
        function_bc380(result, 0x10c, orientations ? node_count * 0x20 : 0, 4) &&
        function_bc380(result, 0x128, animated ? 0x90 : 0, 0) &&
        havok_object_type_can_have_component(tag);
    if (!allocated) failed = true;
    object = (byte *)OBJECT_GET(result);
    if (!allocated) goto fail_allocation;
    if (animated)
    {
        s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
        state->reset();
        if (state->initialize(*(long *)(model + 0x14), model_tag, true)) *(dword *)(object + 4) |= 0x800;
        else *(dword *)(object + 4) &= ~0x800;
    }
    else object[0xb3] = 0;
    if (*(long *)(definition + 0x94) > 0)
        memset(object + *(short *)(object + 0x11e), 0xff, (*(short *)(object + 0x11c) >> 3) << 3);
    if (!function_108a90(result, (long)placement, (long)&failed))
    {
        function_108bf0(result);
        goto fail_allocation;
    }
    object = (byte *)OBJECT_GET(result);
    {
        bool hidden = (*(dword *)(object + 4) & 0x20000) != 0;
        if (placement[0x18] & 6) *(dword *)(object + 4) &= ~0x20000;
        function_ba540(result, *(string_handle *)(placement + 0xc));
        function_ba590(result, *(dword *)(placement + 0xac));
        function_be240(result, *(dword *)(placement + 0x74), (color3f const *)(placement + 0x78));
        byte *properties = (byte *)function_d5b60(result);
        object = (byte *)OBJECT_GET(result);
        real first = properties ? *(real *)(properties + 0x28) : 0.0f;
        real second = properties ? *(real *)(properties + 0x8c) : 0.0f;
        *(real *)(object + 0xe4) = first;
        *(real *)(object + 0xe8) = second;
        *(real *)(object + 0xec) = first > 0.0f ? 1.0f : 0.0f;
        *(real *)(object + 0xf0) = second > 0.0f ? 1.0f : 0.0f;
        function_be8e0(result);
        *(long *)(object + 0x24) = *(long *)(placement + 0xa8);
        if (*(short *)((byte *)OBJECT_GET(result) + 0x12a) != NONE) function_ba3d0(result);
        function_bd020(result);
        if (g_4de2f4 && *(byte *)g_4de2f4)
        {
            if (!placement[0xb8]) placement[0xb8] = function_bf510(result, (s_location *)(placement + 0xbc));
            function_b8600(result, placement[0xb8] ? (long)(placement + 0xbc) : 0);
        }
        function_be1d0(result);
        function_b7360(result);
        if (placement[0x18] & 0x20) object[0xc0] |= 2;
        else object[0xc0] &= ~2;
        function_b8890(result);
        function_be690(result);
        function_108b80(result);
        long effect = *(long *)(definition + 0x50);
        if (effect != NONE) function_176780(result, (s_effect_owner const *)(placement + 0x68), 0.0f, effect, 0.0f, 0, 0);
        function_2e1e0(result);
        if (hidden) *(dword *)(object + 4) |= 0x20000;
        else *(dword *)(object + 4) &= ~0x20000;
        function_109390(result);
        if ((*(dword *)(object + 4) & 0x20000) && !(header->flags & 1) && function_b8820() &&
            !(placement[0x18] & 2) && (!(placement[0x18] & 4) || *(short *)(object + 0x2c) != NONE))
            function_b8540(result);
    }
    return result;
fail_allocation:
    function_bc300(result);
    return NONE;
}
#endif


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
void function_108b10(long index, long datum);
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
        function_108b10(result, (long)datum);
        if (*(short *)(fields + 2) != NONE) function_bf050(result, *(short *)(fields + 2));
        function_a7870(result);
    }
done:
    return result;
}

// @retail 0xbb670
long __stdcall function_bb670(short name_index, bool force)
{
    long result = NONE;
    long existing = name_index >= 0 && name_index < 0x280 ? g_4de2d0[name_index] : NONE;
    if (force && existing != NONE)
    {
        s_object_header_view *header = OBJECT_HEADER_GET(existing);
        byte *object = (byte *)header->object;
        byte type = header->type;
        function_bf090(existing);
        *(short *)(object + 0x1a) = NONE;
        *(long *)((byte *)g_4de2f4 + 0x18) += 1;
        object[0xaa] = type;
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
        byte *definition = (byte *)g_468630[type];
        byte *placements = *(byte **)(scenario + *(short *)(definition + 0xa) + 4);
        s_scenario_block *palette = (s_scenario_block *)(scenario + *(short *)(definition + 0xc));
        result = function_bf0f0((s_type_4f0dcc *)(placements + placement_index * *(short *)(definition + 0xe)),
            type, placement_index, palette, true, false);
    }
    return result;
}

// @retail 0xa73b0
long __stdcall function_a73b0(s_entity_info *info)
{
    long type = *(short *)g_4e3b44[info->definition_index & 0xffff].bytes;
    byte *definition = (byte *)g_468630[type];
    short placement_offset = *(short *)(definition + 0xa), palette_offset = *(short *)(definition + 0xc);
    if (placement_offset == NONE || palette_offset == NONE) return NONE;
    byte *scenario = (byte *)g_4e0350;
    byte *placements = scenario + placement_offset;
    long index = info->field0;
    long clamped = index < 0 ? 0 : index > *(long *)placements - 1 ? *(long *)placements - 1 : index;
    if (clamped != index) return NONE;
    byte *datum = *(byte **)(placements + 4) + index * *(short *)(definition + 0xe);
    long old_object = function_bf760((long const *)(datum + 0x28));
    if (old_object != NONE) function_a7ab0(old_object);
    definition = (byte *)g_468630[type];
    byte *palette = scenario + *(short *)(definition + 0xc);
    short palette_index = *(short *)datum;
    if (palette_index == NONE) return NONE;
    clamped = palette_index < 0 ? 0 : palette_index > *(long *)palette - 1 ? *(long *)palette - 1 : palette_index;
    if (clamped != palette_index) return NONE;
    if (*(long *)(*(byte **)(palette + 4) + palette_index * 0x28 + 4) != info->definition_index) return NONE;
    return function_bf0f0((s_type_4f0dcc *)datum, type, info->field0, (s_scenario_block *)palette, false, false);
}

struct s_havok_component;
bool havok_component_any_rigid_body_active(s_havok_component *component);
bool havok_component_main_rigid_body_movable(s_havok_component *component);
void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
void havok_component_rigid_body_linear_velocity_get(long body, s_havok_component *component, vector3f *velocity);
void havok_component_rigid_body_angular_velocity_get(long body, s_havok_component *component, vector3f *velocity);
void function_1d96d0(long index);

// @retail 0xbc5e0
void __stdcall function_bc5e0(long index)
{
    byte *object = (byte *)OBJECT_GET(index);
    bool disabled;
    if (object[0xc0] & 0x40)
    {
        byte *component = g_51e9b8->data + (*(long *)(object + 0xb4) & 0xffff) * 0xa0;
        bool active = havok_component_any_rigid_body_active((s_havok_component *)component);
        if (active) *(dword *)(component + 4) &= ~0x10000;
        if ((active || (object[0xc1] & 1)) && *(long *)(component + 0x74) > 0)
        {
            if (!(*(dword *)(component + 4) & 0x100) && havok_component_main_rigid_body_movable((s_havok_component *)component))
            {
                transform4x3f matrix;
                function_1d4360((s_havok_component *)component, &matrix);
                vector3f *forward = &matrix.forward, *up = &matrix.up;
                if (*(dword *)(component + 4) & 2) forward = up = 0;
                else function_b91d0(index, forward, up);
                function_b7430(index, &matrix.position, forward, up, 0, false, false, false, true);
                if (active)
                {
                    vector3f linear, angular;
                    vector3f *angular_pointer = 0;
                    havok_component_rigid_body_linear_velocity_get(0, (s_havok_component *)component, &linear);
                    if (!(*(dword *)(component + 4) & 2))
                    {
                        havok_component_rigid_body_angular_velocity_get(0, (s_havok_component *)component, &angular);
                        angular_pointer = &angular;
                    }
                    function_b7740(index, &linear, angular_pointer, false);
                }
                else function_b7740(index, g_4687a4, g_4687a4, false);
            }
            else function_b7740(index, g_4687a4, g_4687a4, false);
        }
        disabled = !active;
        if (((byte *)g_4e6948)[0xc] != 4 && (*(word *)(object + 0xc0) & 0x800))
        {
            function_1d96d0(*(long *)(object + 0xb4));
            object[0xc1] &= ~8;
        }
    }
    else
    {
        long parent = *(long *)(object + 0x14);
        disabled = parent == NONE || (((byte *)OBJECT_GET(parent))[0xc1] & 1);
    }
    if (disabled) object[0xc1] |= 1;
    else object[0xc1] &= ~1;
}

PRIVATE inline vector3f object_attachment_vector_r18(transform4x3f const &matrix, vector3f const &value)
{
    real const *m = (real const *)&matrix;
    vector3f result;
    result.i = value.k * m[7] + value.j * m[4] + value.i * m[1];
    result.j = value.k * m[8] + value.j * m[5] + value.i * m[2];
    result.k = value.k * m[9] + value.j * m[6] + value.i * m[3];
    return result;
}
PRIVATE inline point3f object_attachment_point_r18(transform4x3f const &matrix, point3f const &value)
{
    vector3f scaled = *(vector3f const *)&value;
    if (matrix.scale != 1.0f)
    {
        scaled.i *= matrix.scale; scaled.j *= matrix.scale; scaled.k *= matrix.scale;
    }
    vector3f result = object_attachment_vector_r18(matrix, scaled);
    point3f point;
    point.x = result.i + matrix.position.x; point.y = result.j + matrix.position.y; point.z = result.k + matrix.position.z;
    return point;
}
void function_b8840(long index);
long function_baf80(long index);
void function_bba20(long index);

// @retail 0xb93b0
void __stdcall function_b93b0(volatile long parent_index, volatile long index, volatile long node)
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
    object[0x18] = (byte)node;
    *(long *)((byte *)OBJECT_GET(index) + 0xc) = *(long *)(parent + 0x10);
    *(long *)(parent + 0x10) = index;
    header->flags |= 0x80;
    *(long *)(object + 0x28) = NONE;
    *(short *)(object + 0x2c) = NONE;
    *(short *)(object + 0x2e) = g_4686c4;
    transform4x3f inverse;
    function_141590(&nodes[(short)node], &inverse);
    point3f position = object_attachment_point_r18(inverse, *(point3f *)(object + 0x64));
    vector3f forward = object_attachment_vector_r18(inverse, *(vector3f *)(object + 0x70));
    vector3f up = object_attachment_vector_r18(inverse, *(vector3f *)(object + 0x7c));
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

void function_b92d0(long index, transform4x3f const *old_parent, transform4x3f const *new_parent);

// @retail 0xb8ee0
void __stdcall function_b8ee0(long parent_index, long parent_marker_name, long index, long child_marker_name)
{
    byte *object = (byte *)OBJECT_GET(index);
    function_b7300(index);
    if (*(dword *)(object + 4) & 0x100) function_b87b0(index);
    s_object_marker parent_marker, name_9ae4a2;
    function_b8d30(parent_index, parent_marker_name, &parent_marker, 1, false);
    function_b8d30(index, child_marker_name, &name_9ae4a2, 1, false);
    if (child_marker_name != NONE && child_marker_name != 0)
        function_b92d0(index, (transform4x3f const *)&name_9ae4a2, &parent_marker.matrix);
    else
    {
        transform4x3f inverse;
        function_141590(&name_9ae4a2.node_matrix, &inverse);
        point3f position = object_attachment_point_r18(inverse, parent_marker.matrix.position);
        vector3f forward = object_attachment_vector_r18(inverse, parent_marker.matrix.forward);
        vector3f up = object_attachment_vector_r18(inverse, parent_marker.matrix.up);
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
