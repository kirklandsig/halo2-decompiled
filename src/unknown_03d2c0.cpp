// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_03D2C0.CPP: the first-person view's object, and the lighting cache
   of an object (built with SSE, apart from its neighbours in
   unknown_02b5a0.cpp) */

#include "target_cache_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include <string.h>

extern s_record_pool *g_509434;
extern dword g_4ba034;

struct s_3dd10_object_header
{
	byte unknown00[8];
	byte *object;
};

void function_3dd10(long object_index, bool force);
struct s_interface_function_context;
void function_2c3b0(s_interface_function_context *context);
real __stdcall function_be8b0(long object_index, long name);
struct s_object_list;
extern s_object_list *g_4de2f4;
long function_baf80(long object_index);

struct s_effect_color_query
{
    real unknown00, unknown04, unknown08;
    dword color_a, color_b;
};
struct s_lighting_record
{
    color3f color0;
    vector3f direction0;
    real length, value1c;
    color3f color1;
    vector3f direction1;
    color3f color2;
    vector3f direction2;
    dword unknown50;
};
struct s_3dfe0_cache
{
    short identifier;
    bool valid, initialized;
    long parent;
    dword sample_time, update_time;
    point3f position;
    s_effect_color_query current, target;
    byte unknown44[0x10];
    s_lighting_record lighting;
    byte unknowna8[0x58];
};
long function_d1850(long object_index, bool value, s_effect_color_query *query);
void function_d4080(long type, s_effect_color_query const *query, long flag, s_lighting_record *record);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3dfe0(long cache_index, long object_index, real priority);

extern s_lighting_record g_55ee60;

struct s_first_person_model;
struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
long function_155760(long player_index);
long __stdcall first_person_weapons_get_models(long user_index, long unit_index, long maximum, s_first_person_model *models);
extern long g_4b72c0;

long __stdcall function_3ddd0(long object_index);

bool function_14ddc0(long local_player_index);
long function_16c2b0(void);

// @retail 0x3d2c0
void function_3d2c0()
{
    g_4c1bd0 = NONE;
    g_4c1bd4 = 0;
    g_4c1bd0 = function_16c2b0();
    if (g_4c1bd0 == NONE)
    {
        g_4c1bd0 = g_4b72c0;
        if (g_4c1bd0 == NONE && g_4b9ed8 != NONE && !function_155760(g_4b9ed8))
        {
            long unit = function_14ddc0(g_4b9ed8) ? *(long *)(g_4e8c24->data + (g_4e8c20->entries[g_4b9ed8] & 0xffff) * 0x21c + 0x2c) : NONE;
            g_4c1bd0 = unit;
            g_4c1bd4 = first_person_weapons_get_models(g_4b9ed8, unit, 4, (s_first_person_model *)g_4c1bd8);
        }
    }
}
// @retail 0x3db00
s_lighting_record *function_3db00(long object_index, real priority)
{
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    if (object[7] & 1) return (s_lighting_record *)((byte *)g_4de2f4 + 0x1c);
    short cluster = *(short *)(object + 0xd0);
    if (cluster != NONE)
        return (s_lighting_record *)(*(byte **)((byte *)g_4e0348 + 0x230) + cluster * 0x5c + 8);
    long parent_index = function_baf80(object_index);
    long cache_index = function_3ddd0(parent_index);
    if (cache_index != NONE)
    {
        s_3dfe0_cache *cache = (s_3dfe0_cache *)(g_509434->data + (cache_index & 0xffff) * 0x100);
        if (parent_index == object_index) function_3dfe0(cache_index, object_index, priority);
        else
        {
            long child_cache_index = function_3ddd0(object_index);
            if (child_cache_index != NONE)
            {
                s_3dfe0_cache *child = (s_3dfe0_cache *)(g_509434->data + (child_cache_index & 0xffff) * 0x100);
                child->lighting = cache->lighting;
                child->target = cache->current;
                child->current = cache->current;
            }
        }
        return &cache->lighting;
    }
    byte *parent = ((s_3dd10_object_header *)g_4e0300->data)[parent_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
    long special = 0;
    if ((bool)(((*(dword *)(parent + 4)) >> 13) & 1)) special = 1;
    if ((bool)((*(word *)(definition + 2) >> 1) & 1)) special |= 1;
    s_effect_color_query query;
    function_d1850(parent_index, 1, &query);
    function_d4080(*(short *)definition, &query, special, &g_55ee60);
    return &g_55ee60;
}
// score 77 (was 77): retail keeps the result in esi and tests it before the
// reload, ours copies it into eax at the merge point and puts the cold "no
// result" return block ahead of the initialization instead of after it
// (retail returns it with mov eax, esi; ours folds it to the constant).
// Tried: return NONE (77); the NONE test after the inner if (77); no early return (161);
// "if (result != NONE) initialize = true; else return" (83); one variable for the
// allocation and the result (141); result = cache_index at its declaration (148).
// @retail 0x3ddd0
long __stdcall function_3ddd0(long object_index)
{
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    long cache_index = *(long *)(object + 0xcc);
    long result;
    bool initialize = false;
    if (cache_index != NONE && *(long *)(g_509434->data + (cache_index & 0xffff) * 0x100 + 4) == object_index)
    {
        result = cache_index;
    }
    else
    {
        cache_index = record_pool_allocate(g_509434);
        result = cache_index;
        if (cache_index == NONE)
        {
            real oldest = -3.402823466e+38f;
            long index = data_datum_index(g_509434, function_16bc00(g_509434, 0));
            while (index != NONE)
            {
                byte *record = g_509434->data + (index & 0xffff) * 0x100;
                real age = (real)((long)g_4ba034 - *(long *)(record + 0xc));
                if (age < 0.0f) age = 1000.0f;
                if (age > oldest)
                {
                    oldest = age;
                    result = index;
                }
                index = data_datum_index(g_509434, function_16bc00(g_509434, index == NONE ? 0 : (index & 0xffff) + 1));
            }
            if (result == NONE) return result;
        }
        initialize = true;
    }
    if (result != NONE)
    {
        byte *record = g_509434->data + (result & 0xffff) * 0x100;
        if (initialize)
        {
            *(long *)(record + 4) = object_index;
            *(long *)(record + 8) = NONE;
            *(long *)(record + 0xc) = NONE;
            record[2] = 0;
            record[3] = 0;
            function_2c3b0((s_interface_function_context *)(record + 0x44));
            *(long *)(record + 0x44) = object_index;
            *(long *)(record + 0x48) = (long)function_be8b0;
            *(long *)(record + 0xbc) = NONE;
            byte *copy_object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
            byte *data = copy_object + *(short *)(copy_object + 0x11a);
            long bytes = *(short *)(copy_object + 0x118) / 10;
            for (long i = 0; i < 4; ++i)
                memcpy(record + 0xc0 + i * 16, data, bytes);
            *(long *)(object + 0xcc) = result;
            function_3dd10(object_index, true);
        }
    }
    return result;
}
