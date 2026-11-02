// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0B7B40.CPP: creating an object from its placement data: the
   header and object datum are allocated and filled, the per-object blocks
   (nodes, regions, collision, animation) are sized, and the type's own
   creation runs. */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"
#include "unknown_1cafc0.h"
#include "effects.h"
#include "unknown_16d180.h"
#include <string.h>

struct s_object_list;
extern s_object_list *g_4de2f4;

struct s_object_create_header
{
	short identifier;
	byte flags;
	byte type;
	short unknown04;
	byte unknown06[2];
	byte *object;
};

#define OBJECT_HEADER_GET(index) (&((s_object_create_header *)g_4e0300->data)[(index) & 0xffff])
#define OBJECT_GET(index) (OBJECT_HEADER_GET(index)->object)

void __stdcall function_b8600(long object_index, long location);
void function_b7360(long object_index);
void __stdcall function_bd020(long object_index);
bool function_bf510(long object_index, s_location *location);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);
void __stdcall function_b8540(long object_index);
void function_146bf0();
void function_108a40(long *placement);
long __stdcall function_bc280(short size);
void __stdcall function_bc300(long index);
bool function_bc380(long index, long field, short size, long alignment);
bool function_bf9a0(long index, long *render_model, long *second_index);
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

// Disabled: 550 differences (2108 of 2150 bytes); enabling it reorders the object call cycle and costs matched b8890, 1c35f0, 1c3770, 1c39c0 and 1d1b60 (build/codex-report.md, round 25).
#if 0
// @retail 0xb7b40 disabled
long __stdcall function_b7b40(void *creation)
{
    byte *placement = (byte *)creation;
    long result = NONE;
    bool failed = false;
    if (!(placement[0x18] & 0x10))
    {
        if (*(long *)placement == NONE) goto done;
        function_108a40((long *)placement);
    }
    long tag = *(long *)placement;
    if (tag == NONE) goto done;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    short type = *(short *)definition;
    byte *object_type = (byte *)g_468630[type];
    byte *model = 0;
    long model_tag = *(long *)(definition + 0x38);
    if (model_tag != NONE) model = g_4e3b44[model_tag & 0xffff].bytes;
    if ((1 << type) & 0x1883) function_146bf0();
    result = function_bc280(*(word *)(object_type + 8));
    if (result == NONE) goto done;
    s_object_create_header *header = OBJECT_HEADER_GET(result);
    byte *object = (byte *)header->object;
    header->flags |= 8;
    header->type = *definition;
    *(long *)object = *(long *)placement;
    if (placement[0xb] == 0xff)
    {
        *(long *)((byte *)g_4de2f4 + 0x18) += 1;
        object[0xaa] = (byte)*(short *)definition;
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
    bool has_collision = model && *(long *)(model + 0xc) != NONE;
    if (has_collision) *(dword *)(object + 4) |= 0x200;
    else *(dword *)(object + 4) &= ~0x200;
    long render_model, second_index;
    if (function_bf9a0(result, &render_model, &second_index)) *(dword *)(object + 4) |= 0x80000000;
    else *(dword *)(object + 4) &= 0x7fffffff;
    header->unknown04 = NONE;
    *(long *)(object + 0x28) = NONE;
    *(short *)(object + 0x2c) = NONE;
    *(short *)(object + 0x2e) = g_4686c4;
    *(long *)(object + 0x60) = NONE;
    object[0xb0] = placement[0x14];
    *(long *)(object + 0x14) = NONE;
    *(long *)(object + 0xc) = NONE;
    *(long *)(object + 0x10) = NONE;
    *(short *)(object + 0xac) = NONE;
    object[0xaf] = 0xff;
    object[0x108] = 0xff;
    object[0x109] = 0xff;
    if (definition[2] & 1) *(dword *)(object + 4) |= 0x10000;
    byte *created = (byte *)OBJECT_GET(result);
    if (*(dword *)(created + 4) & 1)
    {
        *(dword *)(created + 4) &= ~1;
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
            if (temporary.initialize(*(long *)(model + 0x14), *(long *)(definition + 0x38), true))
            {
                animated = true;
                orientations = !((1 << *definition) & 0x7e0);
                if (((1 << *definition) & 0x380) && (g_4e3b44[*(long *)placement & 0xffff].bytes[0xbc] & 4)) orientations = true;
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
        havok_object_type_can_have_component(*(long *)placement);
    if (!allocated) failed = true;
    object = (byte *)OBJECT_GET(result);
    if (!allocated) goto fail_allocation;
    if (animated)
    {
        s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
        state->reset();
        if (state->initialize(*(long *)(model + 0x14), *(long *)(definition + 0x38), true)) *(dword *)(object + 4) |= 0x800;
        else *(dword *)(object + 4) &= ~0x800;
    }
    else object[0xb3] = 0;
    if (*(long *)(definition + 0x94) > 0)
        memset(object + *(short *)(object + 0x11e), 0xff, (*(short *)(object + 0x11c) >> 3) << 3);
    if (!object_dispatch_create(result, (long)placement, (long)&failed))
    {
        object_dispatch_remove(result);
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
        object_dispatch_finish_creation(result);
        long effect = *(long *)(definition + 0x50);
        if (effect != NONE) function_176780((s_effect_owner const *)(placement + 0x68), result, 0.0f, effect, 0.0f, 0, 0);
        function_2e1e0(result);
        if (hidden) *(dword *)(object + 4) |= 0x20000;
        else *(dword *)(object + 4) &= ~0x20000;
        object_list_insert(result);
        if ((*(dword *)(object + 4) & 0x20000) && !(header->flags & 1) && function_b8820() &&
            !(placement[0x18] & 2) && (!(placement[0x18] & 4) || *(short *)(object + 0x2c) != NONE))
            function_b8540(result);
    }
    goto done;
fail_allocation:
    function_bc300(result);
    result = NONE;
done:
    return result;
}
#endif
