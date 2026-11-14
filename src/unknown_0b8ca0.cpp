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
// score 4 (was 4): the component pool data loads after the index scaling, into eax (retail: before it, into ecx); tried a flags pointer local 4, a component index local tested and passed 13, a goto-free bool form 55, NONE on the left 4, a component pointer local 4
// component data loads into eax after index scaling, retail ecx before it.
// Signed bitset 11; unsigned bitset 11; external object reference 4; object reference/bitset 11.
// External header reference 18; header reference/bitset 20; type read first 4; bitset before object reference 11.
// The view's own physics bits 4; the tail written out in both arms 106; disable tested first 4;
// both index reads through the havok object getter 51; a word index 14; in a one-file LTCG
// build the data stays in eax for every lookup spelling, even with the update tail deleted.
// Round 55: no noinline 4; inline target 4; inline activation/update/list-update declarations 4/4/4;
// component reference 4; const object and type 4; explicitly masked getter index 4.
// Round 31: the pool data read into a component-array local before indexing 4; `if (disable) goto
// update;` ahead of the activation 4; the address written index-first, `index * size + data` (diagnostic,
// identical code, so the evaluation order of the lookup is not the cause) 4; the type read from the
// header at each of its three uses 61; the component index re-read through OBJECT_GET 55. Matched
// 0x11bce0 makes the same lookup with the data loaded after the index scaling, as ours does here.
// Round 57 (lane 10): matched 0x1c4b00 has retail's order here (data into ECX, then the mask) with the
// same getter spelling, because its next load takes EAX; matched 0x1ed340 has ours. An index local
// inside the activation arm 4; a data local then a masked index local 4; the masked index local then
// the data local 4; a scaled offset local 4; a typed elements local plus the masked index 4 (all the
// same code). Nothing visible here keeps EAX busy across the data load.
// Round 32 (lane 17, fast): the component index in a local under a nested NONE test 70 (221 bytes); a named
// `1 << type` mask 11; the physics bit without TEST_FIELD_BIT 48; a byte type 49. Kept: the two `goto update`
// jumps replaced by a final `if (!disable)` block 4 (same code).
// Round 135: forced-inline component getter 4; forced-inline getter without target noinline 4;
// const local object pointer 4; activation result reference 4; final type arms reversed 6;
// inverted outer type test 17; existing object in only the 0x1c arm 80 (217 bytes);
// unsigned component-get parameter 4. All restored; the pool data load schedule is unchanged.
// Round 147 (lane 5, fast): one header local for the object and its type 18; a typed pointer add on the
// pool data 4; the component pointer local before the `disable` test 4 (sunk into the arm); the
// component index read into a local at entry 13. Diagnostics: 1-16 unused locals at entry 4 each (not a
// symbol-count tie); unused mentions of the pool data/pool/index ahead of the lookup 4/4/4/4. Lead: a
// compile-only copy reproduces our order for every lookup spelling, and only a second use of the pool
// data after the add (diagnostic store, 75 at 210 bytes) gives retail's exact data-first sequence, so
// retail's register allocator saw the data live past the add.
// Round 54 (lane 2, fast): `do { } while (false)` around the activation arm, around the call, ahead of the
// test 4/4/4; a const reference to the component index for the test and the lookup 4; a component local inside
// the arm 4; `disable == false` 4. Diagnostics, reverted at once: one to three volatile probes at the top, ahead
// of the `disable` test or inside it leave the data load after the `lea` in EAX, so unlike 0x15adb0 this is no
// missing rotation step: retail allocates the data load while the masked index still holds EAX.
// Round 77 (lane 10, fast): a dword type 4; a short type 11 (201 bytes). Diagnostics: without the
// `else if (disable)` store 41, without the 0x1c/0x20 tail 73, without the final `if (!disable)` block 79:
// the data still loads into EAX after the scaling in each, so no other block of the body sets it.
// Round 156 (lane 5, fast): a direct `bool has_physics = ...` with `if (disable) ... else ...` 80 (228 bytes,
// three callers lose); a component local at function scope assigned in the arm 4; the component pool data
// read into a typed local at entry 4 (sunk into the arm); the two disabled-bit stores through the flags
// word 4; the component index field as dword 4. Matched 0xbc470 makes this exact lookup index-first from
// the same call-argument form, and the data-first sites with one data use (0xbc5e0, 0x1c35f0) keep the
// component in a multi-use local, so retail's ECX data points to a second use of the component here.
// Round 137 (lane 7, fast; a one-file LTCG harness reproduces 4): a second real use of the component gives retail's
// data-first lookup (pool in EDX, data in ECX before the mask): a NULL test of it 42 (204 bytes), a test of its object field
// 46 (209 bytes), both diagnostics; uses that leave no code do not (harness: an empty inline taking it 4, a dead field read 4,
// a self-compare 4, cleared after the call 4). A `* const` component local 4; a `long const` index local in the arm 4; a
// reference to the component 4.
// Round 100 (m3 lane 9, fast): the activation function 0x1d1c60 as a member of the component type (definition
// and all four calls, diagnostic) 4, its callers unchanged; a component pointer started NULL, set under the three
// tests and tested afterwards 43 (202 bytes); the outer test inverted with the `disable` store first 4; the three
// tests in a named bool with `disable` tested first 80 (228 bytes, callers 0xb73b0 and 0x1ed340 lose 2 each); a
// typed components local and a component index local inside the arm 4.
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
			havok_component_rigid_bodies_activate(havok_component_get(object->havok_component_index));
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
    long absolute = function_16bc00(data, index == NONE ? 0 : (index & 0xffff) + 1);
    if (absolute == NONE)
        return NONE;
    return (*(short *)(data->data + data->size * absolute) << 16) | absolute;
}

// MiniMax S (11-14): no new form, no lower score (165): the 71-point form of round 11-12 rested on spellings that are not allowed, so the body stands; the frame is retail's (0xb4, 895 bytes) but the cluster bit table's base is read inside our loop where retail reads it once before the first query, the zoom argument is narrowed to a short where retail passes it in a full register, the angle's x87 sequence loads the radius before the distance and takes the cosine before the dot product where retail does the opposite, and the definition pointer sits in edx where retail holds it in eax.
// MiniMax B (11-12): score 71 (was 165): the step helper now takes its result after the call, the cluster
// loop ends on a visible flag instead of a second test on the cluster, the header's flags are read
// through the root object, the range's zero is the model-NONE arm and the angle's sum is the float one
// retail adds; the frame and its slots are retail's now (`and esp, -8`, 0xb4 bytes, the result byte at
// [esp+0x13], the link iterator at [esp+0x40], the player index at [esp+0x14]).
// MiniMax B (11-13): no new form, no lower score (165): the permuter cannot search this function on the reduced
// image (248 differences there against the full image's 165), and the earlier 71-point form rested on match-only
// forms, so the body stands as it is.
// score 165 (was 165): the body is main's again (the 71-point form was reverted, its shapes only hold with a
// byte match); the frame is ours (0xac, unaligned) where retail realigns to eight bytes with a 0xb4 frame, the
// result byte sits at [esp+0xf] where retail keeps it at [esp+0x13] and the link iterator at [esp+0x30] where
// retail keeps it at [esp+0x40], so the first difference is at +0x1a and every later offset shifts with the
// frame order; the alignment comes from the call chain of the two callers 0xbf2c0 and 0x268c90, whose files
// are not mine to edit.
// Earlier (the reverted 71-point form, was 165): the frame is retail's and the body is retail's length (895 bytes), but the cluster
// bit table's base is read inside our loop where retail reads it once before the first query, the zoom
// argument is narrowed to a short where retail passes it in a full register, the angle's x87 sequence
// loads the radius before the distance and takes the cosine before the dot product where retail does the
// opposite, and the definition pointer sits in edx where retail holds it in eax.
// bool result byte) while ours keeps EBP for the global base and a dword result.
// MiniMax E (11-11): no new form, no lower score (165): the full build now self-aligns the frame like
// retail (`and esp, -8`, 0xb4 bytes, a result byte), but the locals sit in other slots (the result byte
// at [esp+0xf] against retail's [esp+0x13], the cluster index at [esp+0x30] against [esp+0x40]), so the
// first difference is at +0x1a and every later offset shifts with the frame order.
// MiniMax B (11-11): no new form, no lower score (165); splitting the header test so that the byte test
// branches before the flags are read scores 232 at 874 bytes, and the frame slots still differ: our result
// byte sits at 0xf against retail's 0x13 and our link iterator at 0x30 against retail's 0x40.
// MiniMax D (11-08): ours has an unaligned 0xac frame where retail realigns to eight
// bytes with a 0xb4 frame, so the register homes and the block layout differ from the first byte; the
// realignment is decided by the whole-program view of the callers.
// MiniMax b (11-06): no new form, no lower score; the angle's x87 sequence is retail's byte for byte
// and only the frame differs (our double home at 0x40 in a four-aligned 0xac frame, retail's at 0x48
// in a self-aligned 0xb4 frame), and that alignment comes from the call chain of the two callers
// 0xbf2c0 and 0x268c90, whose files are not mine to edit.
// MiniMax G (11-05): no new form, no lower score; retail's result byte at 0x13 and the player index at 0x14 need retail's exact local set, which the reachable code cannot produce here.
// MiniMax H (11-04): no new form, no lower score; retail's frame pointer and 8-byte self-alignment are not ours (frame 0xac against 0xb4).
// MiniMax H (11-04): no new form, no lower score (230); the earlier rounds' forms still leave the
// caller-dependent frame alignment and the dword result, so the body stands.
// MiniMax R (11-01): no new form, no lower score: our frame is still 0xac against retail's self-aligned 0xb4; the alignment is decided by LTCG from the call chain (callers 0xbf2c0 and 0x268c90), not by this body.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax A (11-07, run 54): no new form, no lower score (230); the frame is still ours 0xac against
// retail's self-aligned 0xb4, and that alignment is decided by LTCG from the call chain, not here.
// MiniMax R (11-01): no new form, no lower score: our frame is still 0xac against retail's self-aligned 0xb4; the alignment is decided by LTCG from the call chain (callers 0xbf2c0 and 0x268c90), not by this body.
// Earlier round: the root-walk loop and the table read are the only places
// left that could give retail's frame shape, so each was tried alone and together: the table
// read moved inside the root loop 216 fast (237 plain), a bool result instead of the dword
// 264, and both together 220; every one is worse than the 231 we keep, so the body stands.
// The remaining difference is the frame (retail self-aligns to 0xb4, ours is 0xac with
// EBP-based globals) and the result width; nothing in the reachable code moves it.
// Earlier: retail's x87 block computes atan2(radius, magnitude) (swapping ours scores 230).
// Round 89: the reduced image self-aligns like retail, so trials were measured there (baseline 247)
// and confirmed plain: the existing function_baf80 for the root walk 280 (stays out of line; retail
// calls it out of line from 48 other functions, so this loop is its own); the table read inside the
// root loop (retail's read after the NONE test, the table re-read after the loop) 216, plain 237;
// plus a bool result (retail's stack byte) 220; plus the existing function_caf60 for the marker
// position (retail inlines it) 167 at 895 B, with retail's threaded cluster loop, but plain 288 at
// 855 B; plus the angle sum in real 191. Lead: apply those three once the full build self-aligns
// (BLOCKED-BY 0x268c90: without `and esp, -8` EBP is free and holds the object table).
// caller-dependent frame alignment and scalar homes differ;
// Caller-dependent eight-byte alignment and scalar/angle homes remain.
// Earlier 231 (from 231): caller-dependent frame alignment and scalar homes differ;
// direct double with ancestry for-loop 235; first offset loop 235; second offset
// loop 235; marker-position loop 235; initial radius snapshot 235.
// direct radius with double magnitude 231; real radius 239; both widths 231;
// byte result with real radius 270; word result with double magnitude 237.
// Caller-dependent eight-byte alignment and angle homes remain. Direct marker position 262; displacement initializer 235; scaled range 235; successive square sum 235; unsigned model handle 235 (ordinary radius in all).
// Caller-dependent eight-byte alignment and scalar homes remain. Native root
// fields 235; radius field pointer 235; unit slot pointer 247; long cluster 246; early centre
// address 235 (ordinary radius in all).
// Caller-dependent eight-byte alignment and scalar homes remain. Positive
// root scope 239; unsigned root handles 239; compact cluster search 239; far-range arm first 239;
// squared-bound names/order 239 (direct radius operand, both callers included in all).
// Caller-dependent eight-byte alignment and angle homes remain. Player for-loop 235; cluster for-loop 235; squared zoom 235; position library copy 235; direct unit receiver 234 (fast, ordinary radius in all).
// Caller-dependent eight-byte alignment and scalar/angle homes remain. Narrow marker lifetime 251; shared root bit reader 275; cluster reader 239; native tag fields 239; long-result success breaks 239; direct unit guard 239 (all without the inherited radius alias and snapshot).
// Byte/char return with direct radius 269/269; byte return with bool/byte/char
// result 292/292/292; byte return with angle/double magnitude/angle sum 261/257/261.
// Both caller declarations and every executable statement restored.
// Earlier 231 (from 231): Caller-dependent eight-byte alignment and angle homes remain. Direct marker position 262; displacement initializer 235; scaled range 235; successive square sum 235; unsigned model handle 235 (ordinary radius in all).
// Earlier 231 (from 231): Caller-dependent eight-byte alignment and scalar homes remain. Native root
// Earlier 231 (from 231): Caller-dependent eight-byte alignment and scalar homes remain. Positive
// Earlier 231 (from 231): Caller-dependent eight-byte alignment and scalar/angle homes remain. Narrow marker lifetime 251; shared root bit reader 275; cluster reader 239; native tag fields 239; long-result success breaks 239; direct unit guard 239 (all without the inherited radius alias and snapshot).
// shared square/dot/both 239/239/239; raw/viewed distance 242; distance/facing partials 239/239;
// shared square with facing partial 239; both partials with viewed distance 242 (all without the
// inherited radius alias and snapshot).
// Round 144: shared salted-index construction in the existing scan helper, direct/result-first
// results and conditional/explicit next index 257/259/257/256; ordinary inline versions 257/259/257/256.
// All restore the helper, input forms and inherited radius exactly; caller-dependent alignment remains.
// Real calling/step/datum iterator state 326/286/286; with Boolean result
// 355/328/328; sole calling/datum pool receivers 326/282. All original
// code restored; caller-dependent self-alignment and full-handle spills remain.
// inline 231; noinline 231; inline noinline 231; inline direct radius 239; noinline direct radius 239; inline
// noinline direct radius 239; cdecl direct radius 239; stdcall direct radius 239.
// Round 329: native position/radius fields, readonly object and direct center 239/239/239;
// direct marker/both points 266/266; shared differences/readonly 239/239; native root observations 252.
// All use ordinary direct radius operands; both known callers included. Original type and body restored.
// Round 17: named float angle term 231; separate facing vector 231; second point-difference helper
// with that vector 231; first point-difference helper 231; second helper with the shared vector 231;
// component-assignment helper 231; dot-product helper with component assignment 231; point setter 234.
// All include both known callers. Original position, differences, dot expression and angle restored.
// Lane 10 r11: BLOCKED-BY 0x268c90 for the frame: the fast build (which self-aligns like retail)
// scores this body 247; there a bool result 276, short result 235 (plain 239, no self-alignment),
// marker/position/offset declared at function scope 247 each. Retail also keeps the player-index
// NONE test at the loop latch and widens the zoom argument to 32 bits. Original body restored.
// Round 189: actual initial/next cluster-query ordinary/inline/separate-noinline
// declaration pairs (1,0)/(2,0)/(0,1)/(0,2)/(1,1)/(1,2)/(2,1)/(2,2), with direct radius,
// 239 each. Original declarations and radius form restored; caller-dependent alignment remains.
// Round 176: Comparison-produced near/facing/both long results 236/235/237; Boolean results 287/283/277;
// one per-player success exit with comparison-produced long/Boolean result 237/277. Direct original
// radius observations throughout; original result, inherited radius storage and all body statements restored.
// Round 409: complete pool observations 251; one admitted unit handle throughout 262; both 251;
// unsigned full handle alone/pool/unit/all 257/257/247/247; all with Boolean result 274.
// Direct radius in all; original complete body restored.
// Round 293: Consumer-stage visibility receiver/word ordinal/bit, all seven subsets, then all with unsigned bit production: 239 each. Both known callers preserved; direct original radius in each trial. Complete original body, inherited radius storage and interfaces restored.
// Round 205: Existing marker-position provider with long/Boolean/word/byte results, inherited/direct radius:
// 230/251, 280/295, 238/255, 260/272. One-point 230 rejected; all original forms restored.
// Caller-dependent self-alignment, full-handle spills and scalar/angle homes remain.
// Round 7 (lane 17): the angle now computes retail's value: retail loads the magnitude, then the radius,
// then `fxch; fpatan`, which is atan2(radius, magnitude), and adds pi/4 as a float (`fadd dword` of the
// float constant at 0x45e01c); ours computed atan2(magnitude, radius). The swap alone 232 plain (radius
// loaded first, no fxch); with the angle sum in real (`(real)atan2(radius, (double)magnitude) + pi/4`)
// retail's exact x87 sequence, 230 plain (kept). With the callers in the reduced image it no longer
// self-aligns (fast now agrees with plain); without them it does (246): either caller alone gives LTCG
// the alignment guarantee retail's lacked, so the frame is a call-chain property, not this body's.
// On 230 (fast, callers included): the radius as a plain double (no alias) 235 (the field is re-read,
// no double home); a bool result 273 (861 bytes), on the next form too 273; the header table read
// inside the root walk 236; the range's zero as the model-NONE else arm 228 (retail's memory-zero
// compare and else-arm xorps; below the keep bar, lead); on that, a single-result iterator step 273
// (927 bytes), both offsets through vector3d_from_points3d 228, the typed visible_clusters field 228,
// the step's ternary as `index != NONE ? ... : 0` 227 (below the bar, lead), and on that the outer
// scope block removed 227, the link iterator at function scope 227, the squared radius before the
// model index 227, the marker at loop-body scope 227, the near test as `radius_squared >
// distance_squared` 227. Retail's latch NONE re-test and abs-index spill follow from its frame
// pointer (the handle lives in memory); the zoom argument's 32-bit widening needs 0xc8880's
// parameter (contributor, left alone).
// Round 38 (m3 lane 14): both callers now match and the old BLOCKED-BY 0x268c90 no longer explains the frame:
// with either caller alone in the image (0x268c90, or the table-called 0xbf2c0) ours never self-aligns, though
// both call sites sit 4 bytes off an 8-byte boundary (0xbf2c0 after four pushes, 0x268c90 after a 0x50 frame and
// four pushes) in retail and ours alike; 0xbf380's frame (0x2a04 against our 0x2a08) and
// the table entry make no difference (diagnostics). Our LTCG self-aligns this body only with no caller in the
// image, so the frame is BLOCKED by the toolchain's alignment rule, not by any caller's source. Fast, caller
// included: the cluster test through the existing bit_vector_test 230 (same code, kept: it replaces that
// helper's body written out); the distance through length_sq3f 230; the range test as an else-if 230; the
// model index read at each use 227 (below the bar); a named radius for the square 230.
// Y499: closed ancestry/range/marker phase subsets on the real-angle tree with direct double
// radius operand 246/246/246/247/247/247/247; all with bool result 260. Reduced forms self-align
// with both callers present, but frame 0xac versus 0xb4 and scalar homes remain. Original body restored.
// M305: Paired radius/distance/normalization scalar lexical identities, eight direct-radius combinations 246 each.
// Round 126 (lane 8, fast, both callers matched): unit index local at every use 232; player pool global read at
// each use 229; that as a for loop 260 (926 bytes); pool at each use with the range zero as the model-NONE else arm
// 227; centre pointer declared after the radius square 230.
// Round 147 (lane 5, fast, both callers in the image; without them every form below reads 243): the
// range's zero as the model-NONE else arm 228; that plus a single-result query step (`long result = NONE;
// if (absolute != NONE) result = ...`) 273 (927 bytes; frame 0xb4 and the player index spilled as
// retail's); the single-result step alone 276 (927 bytes); else-arm zero + the step's ternary as
// `index != NONE ? ... : 0` + the model index read at each use 224 (895 bytes, aligned 170 -> 108: the
// lead, six under the keep bar); on it a bool result 272 (862 bytes), the player pool global at each use
// 229, the near test reversed 224, the link iterator at function scope 224, the outer scope block removed
// 224, the marker at function scope 229, the centre pointer before the marker 224, the unit index read
// from the player at the marker call 224; else-arm zero + ternary 227; else-arm zero + model index at each
// use 225; else-arm zero + ternary with the radius square before the model index 227; the rest of the
// body inside the cluster loop's found branch 230 (the found path then skips the `cluster == NONE`
// re-test as retail's does), with the 224 forms 239.
// Round 115 (fast, both callers matched): single-result query 259 (926 bytes); with range/model forms 261 (927);
// ternary query with those forms 225; guarded root table 231; delayed flags 241; if/else query result 272 (928);
// named ternary result 264 (929); non-NONE-first input with the direct ternary 224 (895); one-variable query result
// 258 (923). NOT kept: 224 is only a six-point gain; every other form regresses or moves the frame/length away.
// Original helper, range, model, root and flag forms restored.
// Round 156 (lane 5, fast, both callers in the image): rebuilt the 224 lead (else-arm zero + `index != NONE ? ...
// : 0` step + model index at each use; aligned 98 with stack offsets masked) and on it: the ancestry walk through
// OBJECT_GET at each step (no entry table local; retail's fresh table read after the walk) 230 (aligned 94); the
// cluster search as first query, NONE exit, then `while (!visible) { next; if NONE exit }` 240; one step site
// `while ((player_index = step(players, player_index)) != NONE)` 289 (841 bytes: the step is not duplicated);
// the definition read at each use 227; the step returning one ternary 224 (same code). Retail keeps the
// loop-bottom step's `index != NONE` test (its player index lives in memory); main and the 224 lead fold it.
// Round 82 (m3 lane 10, fast, both callers in the image): the marker position through the existing
// function_caf60 (inlined by the link) 260 (926 bytes); on the 224 lead (else-arm zero, `index != NONE ?
// ... : 0` step, model index read at each use; re-measured 224) with function_caf60 259 (926 bytes); the
// model test inverted (`== NONE` arm first) 224; the radius square moved after the range block 222 (not kept:
// a statement order chosen by score on top of a six-point lead); without the lead, the 1600 default inside the
// model arm with the range test overwriting it 242 (907 bytes). Observation: relative to each caller's own
// entry, retail's direct callers 0xbf2c0 (table-called) and 0x268c90 both call with esp four bytes off an
// eight-byte boundary, and so do the callers of the matched self-aligning register-convention functions
// 0x26a860 (from 0x269580, after 0xbadc0's `ret 4`) and 0x69790 (from 0x69350), which self-align in our build.
// On the 222 form: the model test inverted 213, without the step ternary 222, a named radius for the square and
// the double 245 (908 bytes), the centre pointer before the marker call 222, the player pool global at each
// use 226; the lead without the else-arm zero (initialised 0) with the moved square 217. Without the moved
// square: the radius square written at its single use (no local) with the inverted model test and the lead
// 222, without the step ternary 224, with the lead but not inverted 231, lead without ternary 233, alone 232,
// with the model index at each use only 229. Every gain rests on the moved square or the arm inversion.
// Round 100 (m3 lane 9, fast with both callers in the image, confirmed by the full check): 230 -> 165 at 895 bytes,
// retail's `and esp, -8`, 0xb4 frame and 248 instructions. Cause of the missing self-alignment: the named double local
// (and its const reference). With a caller in the image a named double never self-aligns here, whatever holds it
// (diagnostics, 230 each: a double member of a local aggregate, a double array, the double at function scope; an
// unused `__int64` 308 at 956 bytes); with no named double the frame self-aligns: the radius cast in the atan2
// argument 246 (0xac frame). Kept on that: the magnitude call written inside the atan2 argument (retail's qword
// spill of the radius across the call) 246; plus the existing function_caf60 for the marker position 283 (934
// bytes); plus a bool result 246 (905 bytes); plus the ancestry walk through OBJECT_GET 191 (921 bytes); plus the
// atan2 sum left in double (no `(real)` cast: the dot product and the compare go to x87) 165. Not kept, on 165:
// the range's zero as the model-NONE else arm 167 (aligned 118 against 122); a named `real angle` for the sum 191
// (921 bytes), with `cos((double)angle)` 167; the compare reversed with the dot product cast to double 158 (a cast
// on a comparison side, and the frame is 0xac again); the angle through an inline helper taking the radius as a
// double parameter 276 (927 bytes, self-aligned). Still differs: retail evaluates the atan2 sum (a float add) ahead
// of the dot product and applies fcos after it; it keeps the player index in memory, re-tests it against NONE at
// the latch and keeps the masked index from the loop top for the step; it loads the cluster bit table ahead of
// the first query, and has no NONE re-test after the cluster loop.
// Round 170 (m3 lane 6, fast with 0xbf2c0/0x268c90, both matched in each row; masked = register-masked aligned count, 93 for
// this body), forms listed on the 230 body re-measured on this one: the step helper's test as `index != NONE ? ... : 0` 165
// (masked 91); the model index read at each use 176; the player pool read at each use and the near test reversed 165 (same
// code); the rest of the body inside the cluster loop's found branch (no re-test after the loop, as retail) 212 (873 B,
// masked 80), with the step test inverted too 212 (masked 79). New: a named `double angle` ahead of the test 286 (857 B, frame
// no longer self-aligns); the sum as `cos((double)((real)atan2(...) + 0.785...f))` 165 (retail's `fadd dword`, masked 92).
// Round 174 (m3 lane 5, fast with 0xbf2c0/0x268c90): the definition read typed (s_object_definition, model_tag_index) 165
// (same score, kept). Stacked forms, each on the one before: the root test through a root-object pointer with the flags
// read in the test (retail reads the object pointer before the byte test and the flags after it) 164; the cluster bit
// table pointer read before the first query (retail's `mov edi` placement; a one-use pointer) 161; a `visible` flag set in
// the search loop instead of the NONE re-test (retail has no re-test) 208 (873 B); the step helper with a single result
// (`result = NONE; if (absolute != NONE) result = ...`, the shape of matched 0xc3f90/0x176cb0, whose latches are
// retail's) 118 (898 B: player index in memory, latch NONE test, 0xbc frame); the range zero as the model-NONE else arm
// 90; the test as `cos(...) < dot` 67 (895 B), with `(real)atan2 + 0.785f` 67 (fadd dword); instead a named `real angle`
// before `dot > cos((double)angle)` 69 (masked aligned 5: angle, dot, then fxch/fcos/fxch as retail; 0x30bf0 normalises
// the offset, so the dot must follow the call), with `cos((double)angle) < dot` 67; atan2 without the double casts 267
// (the radius is no longer spilled, frame not self-aligned); the zoom call through an inline wrapper taking the level as
// a long 67 (retail's `movsx ebx`; 0xc8880 is a contributor's and tests `cmp bx`, so its short stays); on that, the
// constant first in the angle sum 67, `(double)(real)` on the magnitude 67, the radius square before the definition 67,
// the angle through an inline helper with a real radius 67, a named `real radius` for the square 67. Left at 67: the definition pointer in EDX (retail EAX), and
// after the call retail stores and loads the magnitude before reloading the spilled radius (one more fxch). Without the
// match-only pieces (flag, one-use table pointer, wrapper) the same body reads 210 (910 B; the re-test stays), so nothing
// is kept (lead for the next round: all of them together, then the two causes above). Diagnostics: a `(short)(long)` cast
// on the zoom argument folds (still `movsx bx`); the search as a for loop with the test in its condition 206 (re-test stays).
// Also measured on the stack: `dot > cos((real)atan2(...) + 0.785f)` 126 (930 bytes: the float cos overload moves the dot to
// SSE); the model index through TAG_DATA with no definition local 69 (same code); without the one-use table pointer but with
// the flag 74, without the flag but with the pointer 206 (910 bytes). On the 67 form: the magnitude assigned inside the atan2
// argument (diagnostic) 267 (873 bytes, frame no longer self-aligns); a named `double radius` read before the call and a named
// `real distance` 250 (895 bytes): retail's exact x87 order (`fld distance; fld qword radius; fxch; fpatan; fadd dword; dot;
// fxch; fcos; fxch`) but the frame no longer self-aligns - lead: retail holds the radius in memory across the call as a value,
// where our build self-aligns only for a spill temp; the angle through an inline helper (double radius, real distance) 267
// (873 bytes), with the normalising call inside the helper 67 (same as the 67 form), (double, double) 262 (877 bytes).
// NOT kept (coordinator review, MiniMax B 11-12): 165 -> 71 rested on match-only forms (a `visible` flag set in the cluster loop, a (real) cast on atan2, the swapped comparison); without them the full build gives 215, worse than main's 165. The body is main's again.
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
        dword flags = *(dword *)((byte *)header->object + 4);
        if (!(((byte *)header)[2] & 1) || !(bool)((flags >> 8) & 1) || (bool)((flags >> 18) & 1))
            goto done;
        s_object_link_iterator links;
        short cluster = function_b8a80(root, &links);
        while (cluster != NONE)
        {
            if (bit_vector_test((dword const *)((byte *)g_4e6948 + 0x1138), cluster))
                break;
            cluster = function_b8b20(&links);
        }
        if (cluster == NONE)
            goto done;

        byte *object = (byte *)OBJECT_GET(object_index);
        s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
        long model_index = definition->model_tag_index;
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
                point3f position;
                function_caf60(unit_index, &position);
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
                    if (offset.k * forward->k + offset.j * forward->j + offset.i * forward->i >
                        cos(atan2((double)*(real *)(object + 0x3c), (double)function_30bf0(&offset)) + 0.7853981852531433f))
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

// score 17 (was 17): entry agrees; in the placement tail retail keeps the type in ecx, the palette offset in
// ebx and the size in eax (sign-extended at its load, the definition copied to esi first); ours keeps the
// type in esi and the size as a word in bx (237 bytes against 236).
// Tried: no volatile (name index stays in ebp, result spilled) 76; function_bb760 call for the lookup, same
// 76; result declared after the force block 81; long/inline definition fields with a block local 43, 34;
// long size only 38. Copy + volatile name reload 26; result declared before the lookup 83; no result
// variable 78; result declared in the tail 85; long size 27; long offsets + palette at the call + datum
// local 27; the 0xb7300 placement-block helpers (both orders, both product orders) 23; lookup via 0xbb760
// 17. Test-produced result 78; with wide stride/call-use palette/both 76/44/42; with direct
// offsets/type name/read-only descriptor/read-only wide 78/78/78/76. Canonical descriptor fields through
// mutable/const pointers and references all 17; the same four contracts with a promoted stride all 27
// (236 bytes). Shared type home 17; promoted placement index 28; both 28; shared type plus stride 27;
// promoted index/stride 36; shared type/index/stride 36; shared type with call-use palette 37; shared
// type/index with call-use palette 26. Plain/typed aliases 17/17; typed name 17; promoted datum index
// 19/236; typed block 17; direct name-index reads 31/236; early field/base declarations 17/17; definition
// declaration/initialization first 17/17; register type/size 17/17; alias with wide stride 27/236;
// definition/type-definition before guard 17/17; placement-type name 17.
// Round 14 (0xa73b0's lookup forms): definition read at each use with a datum local 33 (236 bytes);
// definition local with a datum local 33; long stride with the placements pointer 27 (236 bytes, the
// product then extends the index into eax); the product's operands either way round, no effect (17/27).
// Round 101 (lane 7), `--fast`: palette address at the call 37 (240 bytes); a screen of palette offset short/long x
// palette local/at the call x size short/long x placement offset short/long 17/17/27/27/37/37/26/26 (short palette
// offset) and 17/17/27/27/25/25/32/32 (long); short type 115 (267 bytes); a named datum local 33 (236 bytes: size
// sign-extended into EAX at its load and the palette added just before its push as retail, but scenario ESI,
// definition ECX, type EDI, index BX), and on it the same width/palette screen with both product orders and the
// palette declared before or after the datum 27/33 (palette local) and 17/27 (palette at the call).
// Round 80: a second descriptor receiver for size and placement offset 17; with wide size 27 (236 bytes);
// with all offsets wide 27; all offsets wide and palette at the call 32; with a wide index 32; with a
// datum local 31; the second receiver for size only 17, for placement offset only 17. Restored.
// Round 57 (lane 10): retail reads palette, size, offset, with the type in ECX, the definition copied
// to ESI after its first read, and the palette added and the size multiplied late. All offsets long
// with the palette at the call 32 (236 bytes); the same with a long index declared at each of seven
// points 30/31/26/26/27/27/32 (236 bytes; after the definition: size in EAX, palette in EBX, index in
// EDI as retail, but type ESI, definition ECX, no copy, which shifts the tail), the product either way
// round the same; first table read in place of the definition local, on those, 31/26/26/27/27/32; table
// read at each use 31/26/27/27; definition re-assigned after the palette read, at seven index points
// 26/31/26/31/27/27/32 (the copy propagates); first table read in place on this body 17 (237 bytes);
// the 0xb7300 helpers again with a long index at each point 26/23/23/26/23/23, with a short index
// 23/23, the global passed to them 25/21/23/23, 23, scenario local kept for the name 25/21 (21: scenario
// EBX and a dead block add); inline datum helpers over the definition 26/26/27/27/27/27, over the type
// 31/27, an elements helper with an out size 31/27.
// Round 147 (lane 5, fast): the type read from the name entry at each use, no type local, 17 (same code);
// every placement read at the call with only the definition local 67 (236 bytes, EBX pushed lazily); the
// type at each use with the palette added at the call 37 (240 bytes), with a long size 27 (236 bytes);
// the type local with the definition table read at each use, all at the call, 67 (236 bytes).
// Round 54 (lane 2, fast; palette added at the call, long size; all 236 bytes): type local or read at each use x
// definition local, read at each use or first read in place x offset long or inline: placements local 27 (long
// index), 32 (short index), placements at the call 28 (long index), 28 or 34 (short index). With the type and
// definition locals, a long index at seven points 30/31/26/26/27/27/32 (26: size EAX, palette EBX, index EDI as
// retail; left: type ESI, definition ECX, no copy). Palette and block through local helpers of 0xd5260's shape
// (out-parameter size), palette first or last, global or scenario local, inline or forced: short or late long
// index 23 (scenario ESI, index BX, a dead block add), long index ahead of the helpers 25/26. The palette offset
// through an inline read, then the definition local, long index at five points: placements local 31/26/26/27/27,
// at the call 32/28/28/28/34; the palette pointer through it: local 33/32/32/33/33, at the call 33/32/32/33/27.
// Lead: retail's `mov esi, eax` fits a definition local taking a value already read, with the size coalesced to
// EAX for the product; every form above propagates the copy, so the type never reaches ECX.
// Round 77 (lane 10, fast): the type read beside the placement index, ahead of the NONE test, 23 (238 bytes);
// that with long offsets and size and the palette at the call 32 (236 bytes); the palette pointer formed after
// the placements read 26 (237 bytes); the placement block address named before its element read 17 (same
// code); long palette offset and size with the palette local where it is 27 (236 bytes).
// Round 156 (lane 5, fast): on a datum local (retail's statement order: size into EAX at its load, product before
// the pushes, palette added at its push), the scenario from the global at each use 33, with the type re-read too
// 33 (index back in DI; scenario EBX, type ESI, definition ECX); a screen of scenario local/global x type
// local/re-read x definition local/table at each use x palette local/at the call x size short/long: palette
// local 33 in all sixteen (236 bytes), palette at the call 17 with the scenario local and 25 with the global (237).
// Round 81 (m3 lane 1, fast with 0x29fd40/0x2a12f0): palette added at the call with a long size and the placement
// offset read inline 26 (scenario moves to ECX, index to SI); the type and definition locals renamed to sort the
// other way 17 (same code); the definition bound as a const reference to its table entry 17 (same code); a named
// datum with the palette added at the call 17; palette and size declared ahead of the definition 17.
// Round 67 (m3 lane 2, fast): a datum local (size * index) with the palette added at the call, re-measured 17 (short
// palette offset) and 27 (long palette offset, 236 bytes). The 27 form is retail's code but for two registers: scenario
// EDX, placements EBP, index DI, the product before the pushes and the palette add at its push all agree; only the type
// takes ESI (retail ECX), so the definition takes ECX and the size is read first (retail loads the definition into EAX,
// reads +0xc, copies it to ESI). With the type push replaced by 0 (diagnostic) the type takes ECX. On the 27 form, forms
// listed above for other bases re-measured: a long index at its read 31; no type local (name field at both uses) 27 (same
// code); the first table read in place 27 (same code); the definition through an inline getter 27 (same code); the type
// read ahead of the NONE test 30; palette and block both through helpers, palette first, 33 (scenario ESI, index BX). New:
// the size and datums through a block helper of 0xd5260's shape with the palette offset read in the caller, from the
// definition local or the table, 27/27 (same code).
// Round 174 (m3 lane 5, fast): matched 0xa5bd0 shows retail's `mov edi, ebx` definition copy before its +0xe/+0xa reads
// (its source reads the table again at the later uses); that idiom here (definition local for the palette offset, size and
// placement offset through g_468630[type] again) on the 27 form 27 (same code), with the palette pointer as a local at its
// read 27, on main's body 17 (same code): the re-reads CSE into the definition and the copy never survives. On the 27 form:
// the palette offset read after the datum 27; the definition renamed to sort after the type 27 (same code, re-measure);
// an early `return result` on NONE with the call returned directly 81 (244 bytes).
// Round 86 (m3 lane 13, fast): the 27 form re-measured (long offsets and size, datum local, palette added at the call):
// it reads size, palette offset, placement offset through the definition in ECX with the type in ESI, where retail keeps
// the type in ECX, loads the definition into EAX for the palette offset and moves it to ESI only when the size takes EAX
// (a definition that is not planned around the size's EAX). On it: the table indexed by the type at each read 27 (same
// code); the datum as one expression (product first) after the palette offset local 27 (same code). Every table read
// inside the call statement 67 (236 bytes: loads after the pushes, EBX never used); palette then block helpers with an
// out size into locals 23; datum and palette helpers called in the argument list, taking the scenario 23, reading the
// scenario global themselves 23 (all three: scenario ESI or EDI, index BX, the datum built after the pushes).
// Diagnostic: the type not pushed 28 (type EAX, definition still ECX).
// m3 lane 14 r90 (fast): on main's body the call's result returned inside the placement test 78 (245 bytes). On the 27
// form (re-measured 27 at 236 bytes): the type local `long const` 27, the name entry read as a short array (fields
// [0x10]/[0x11]) 27, the three definition reads in the orders size, palette, placement and placement, size, palette
// 27/27 (all four the 27 form's code); the call returned inside the test 81 (244 bytes, round 174's code).
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
// score 285 (was 285): retail has one shared epilogue at 0xb987a, reached from the ancestry-loop
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
// @retail 0xb93b0
void __stdcall function_b93b0(long parent_index, volatile long index, volatile short node)
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
    if (!allowed) return;
    byte *parent = (byte *)OBJECT_GET(parent_index);
    s_object_header_view *header = (s_object_header_view *)(pool->data + offset);
    volatile bool was_active = (bool)((header->flags >> 2) & 1);
    if (*(dword *)(object + 4) & 0x80)
    {
        *(dword *)(object + 4) &= ~0x80;
        if (!(*(dword *)(object + 4) & 0x100) && g_4de2f4 && *(byte *)g_4de2f4) function_b8600(index, 0);
    }
    function_b7300(index);
    if ((bool)((*(dword *)(object + 4) >> 8) & 1)) function_b87b0(index);
    if ((object[0xc0] & 0x40) && header->type != 7) function_b8840(index);
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
    function_141590(function_b8bd0(parent_index, node_index), &inverse);
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
        bit_vector_test((dword const *)((byte *)g_4e6948 + 0x11b8), root_header->unknown04)) function_b7290(root);
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
