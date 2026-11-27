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
#include "bit_vector.h"
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
void __stdcall function_b8540(long object_index);
void function_146bf0();
void function_108a40(long *placement);
long __stdcall function_bc280(short size);
void __stdcall function_bc300(long index);
bool function_bc380(long index, long field, short size, long alignment);
bool function_bf9a0(long index, long *render_model, long *second_index);
bool object_dispatch_create(long index, long placement, long failed);
void object_dispatch_finish_creation(long index);
void object_dispatch_remove(long index);
void function_ba540(long index, string_handle name);
void __stdcall function_ba590(long index, dword mask);
void __stdcall function_be240(long index, dword colors, color3f const *values);
bool __stdcall function_be8e0(long index);
void function_ba3d0(long index);
void function_be1d0(long index);
void __stdcall function_b8890(long index);
void function_be690(long index);
void function_2e1e0(long index);
void object_list_insert(long index);
long function_b8820();
bool havok_object_type_can_have_component(long tag);
void function_b9c60(long object_index, bool flag);
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality);

#define TEST_FLAG(flags, bit) ((bool)(((flags) >> (bit)) & 1))

/* sets or clears one bit of a flags word (as in scenery.cpp and unknown_0b9fc0.cpp; the word one is word_flags_set of
   bit_vector.h) */
static inline void flags_set_bit(dword *flags, long bit, bool value)
{
	if (value)
		*flags |= 1 << bit;
	else
		*flags &= ~(1 << bit);
}

// Not matched yet (2150 of 2150 bytes). Retail inlines function_b9c60 and function_d5c90 here. Left: retail goes
// straight to the exit when the placement has no definition, and it does not load NONE again after function_b9c60.
// @retail 0xb7b40
long __stdcall function_b7b40(void *creation)
{
    byte *placement = (byte *)creation;
    long result = NONE;
    bool failed = false;
    if (!(*(dword *)(placement + 0x18) & 0x10) && *(long *)placement != NONE) function_108a40((long *)placement);
    long tag = *(long *)placement;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        short type = *(short *)definition;
        byte *object_type = (byte *)g_468630[type];
        byte *model = 0;
        long model_tag = *(long *)(definition + 0x38);
        if (model_tag != NONE) model = g_4e3b44[model_tag & 0xffff].bytes;
        if ((1 << type) & 0x1883) function_146bf0();
        result = function_bc280(*(word *)(object_type + 8));
        if (result != NONE)
        {
            s_object_create_header *header = OBJECT_HEADER_GET(result);
            byte *object = (byte *)OBJECT_GET(result);
            bool animated = false;
            header->flags |= 8;
            header->type = *definition;
            *(long *)object = *(long *)placement;
            if (placement[0xb] == 0xff)
            {
                long identifier_type = *(short *)definition; /* read before the counter moves, as retail does */
                *(long *)((byte *)g_4de2f4 + 0x18) += 1;
                object[0xaa] = (byte)identifier_type;
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
                object[0xae] = (byte)*(short *)(object + 0xa8);
            }
            *(point3f *)(object + 0x64) = *(point3f *)(placement + 0x1c);
            *(vector3f *)(object + 0x70) = *(vector3f *)(placement + 0x28);
            *(vector3f *)(object + 0x7c) = *(vector3f *)(placement + 0x34);
            *(vector3f *)(object + 0x88) = *(vector3f *)(placement + 0x40);
            *(vector3f *)(object + 0x94) = *(vector3f *)(placement + 0x4c);
            *(real *)(object + 0xa0) = *(real *)(placement + 0x58);
            flags_set_bit((dword *)(object + 4), 10, TEST_FLAG(*(dword *)(placement + 0x18), 0));
            flags_set_bit((dword *)(object + 4), 9, model && *(long *)(model + 0xc) != NONE);
            long render_index, animation_index;
            flags_set_bit((dword *)(object + 4), 31, function_bf9a0(result, &render_index, &animation_index));
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
            function_b9c60(result, false);
            *(short *)(object + 0xc2) = *(short *)(placement + 0x70);
            *(long *)(object + 0xc4) = *(long *)(placement + 0x68);
            *(long *)(object + 0xc8) = *(long *)(placement + 0x6c);
            object[0xb1] = 0xff;
            *(long *)(object + 0xcc) = NONE;
            *(short *)(object + 0xd0) = NONE;
            *(word *)(object + 0xc0) = 0;
            word_flags_set((word *)(object + 0xc0), 8, TEST_FLAG(*(dword *)(placement + 0x18), 3));
            *(long *)(object + 0xb4) = NONE;
            *(long *)(object + 0xd4) = NONE;
            object[0xd8] = 0;
            *(short *)(object + 0xe0) = *(short *)(placement + 0xb0);
            *(short *)(object + 0xe2) = *(short *)(placement + 0xb2);
            if (!((1 << header->type) & 0x80) && g_510c54->game_time >= 6) object[0xb3] = 6;
            else object[0xb3] = 0;
            long node_count = 1, region_count = 1, collision_count = 0;
            bool orientations = false;
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
            if (allocated)
            {
                if (animated)
                {
                    s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
                    state->reset();
                    flags_set_bit((dword *)(object + 4), 11, state->initialize(*(long *)(model + 0x14), *(long *)(definition + 0x38), true));
                }
                else object[0xb3] = 0;
                if (*(long *)(definition + 0x94) > 0)
                    memset(object + *(short *)(object + 0x11e), 0xff, (*(short *)(object + 0x11c) / 8u) * 8);
                bool created = object_dispatch_create(result, (long)placement, (long)&failed) != 0;
                object = (byte *)OBJECT_GET(result);
                if (created)
                {
                    bool hidden = TEST_FLAG(*(dword *)(object + 4), 17);
                    if ((*(dword *)(placement + 0x18) & 2) || (*(dword *)(placement + 0x18) & 4)) flags_set_bit((dword *)(object + 4), 17, false);
                    function_ba540(result, *(string_handle *)(placement + 0xc));
                    function_ba590(result, *(dword *)(placement + 0xac));
                    function_be240(result, *(dword *)(placement + 0x74), (color3f const *)(placement + 0x78));
                    function_d5c90(result, 0, 0);
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
                    word_flags_set((word *)(object + 0xc0), 1, TEST_FLAG(*(dword *)(placement + 0x18), 5));
                    function_b8890(result);
                    function_be690(result);
                    object_dispatch_finish_creation(result);
                    long effect = *(long *)(definition + 0x50);
                    if (effect != NONE) function_176780(effect, (s_effect_owner const *)(placement + 0x68), result, 0.0f, 0.0f, 0, 0);
                    function_2e1e0(result);
                    flags_set_bit((dword *)(object + 4), 17, hidden);
                    object_list_insert(result);
                    if (TEST_FLAG(*(dword *)(object + 4), 17) && !(header->flags & 1) && (byte)function_b8820() &&
                        !(*(dword *)(placement + 0x18) & 2) &&
                        (!(*(dword *)(placement + 0x18) & 4) || *(short *)(object + 0x2c) != NONE))
                        function_b8540(result);
                    goto done;
                }
                object_dispatch_remove(result);
            }
            function_bc300(result);
            result = NONE;
        }
    }
done:
    return result;
}
