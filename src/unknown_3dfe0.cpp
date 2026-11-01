// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_3dfe0.h"
#include "unknown_0259d0.h"

extern s_record_pool *g_509434;
extern dword g_4ba034;
extern bool g_467464;
long function_d1850(long object_index, bool value, s_effect_color_query *query);
void function_d4080(long type, s_effect_color_query const *query, s_lighting_record *record, long flag);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
real magnitude3d(vector3f const *vector);
real distance3d(point3f const *first, point3f const *second);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3e5a0(vector3f *current, vector3f const *target, real step);
void function_3e4e0(dword *current, dword const *target, real step);

// score 27 (was 27): the object pointer and force flag swap stack homes (retail object 0x14 in ebx,
// force 0x10), update's false store sits after the force test, and special takes esi for edi.
// update = false before force 218 (homes as retail, sample/update timestamps swap esi/edi);
// that plus update elapsed after the object 186; update before the object 216;
// object, update, update elapsed, force 186; an inline object getter for both lookups 27;
// force/object renamed (reset, stale; entity, body) 27; special declared before type 26 (tail
// order as retail, special still in esi); named sample/update timestamps 27, with update first
// 218; the other entry orders of update elapsed/object/force/update 169-240.
// Round 57: read-only byte object 27; the existing object type and flag field 27;
// object declaration hoisted 27; update declaration hoisted 27; object/update declarations
// hoisted 27; object/update/force declarations hoisted 27; special/type declarations split
// with original assignment order 27; a multi-use clamped sample interval 27. Body restored.
// Round 55: the object lookup written at each use 285 (873 bytes); `bool update = false;` as the first
// statement 227 (object loads straight into ebx as retail, but update leaves the parameter slot and
// the sample/update timestamps swap esi/edi), after the cache lookup 227, before the elapsed return
// 227, after it 216; force as a flag set in an if 55 (830 bytes); `update = force` with
// `if (!force && (object[0xc1] & 1)) return;` 228; force as `!(initialized && valid)` 27 (same code).
// @retail 0x3dfe0
void function_3dfe0(long cache_index, long object_index, real priority)
{
    s_3dfe0_cache *cache = (s_3dfe0_cache *)(g_509434->data + (cache_index & 0xffff) * 0x100);
    long elapsed = g_4ba034 - cache->sample_time;
    if (!elapsed) return;
    long update_elapsed = g_4ba034 - cache->update_time;
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    bool force = !cache->initialized || !cache->valid;
    bool update = false;
    if (elapsed < 0) elapsed = 1;
    if (force) update = true;
    else if (object[0xc1] & 1) return;
    bool sample = true;
    if (cache->sample_time != (dword)NONE)
    {
        if (priority > 400.0f) sample = elapsed > 3;
        else if (priority > 200.0f) sample = elapsed > 6;
        else if (priority > 100.0f) sample = elapsed > 9;
        else if (priority > 50.0f) sample = elapsed > 15;
        else if (priority > 10.0f) sample = elapsed > 45;
        else sample = elapsed > 60;
    }
    if (cache->update_time == (dword)NONE || update_elapsed > 1) update = true;
    if (sample)
    {
        bool stationary = false;
        if (cache->valid)
        {
            vector3f delta;
            vector3d_from_points3d(&cache->position, (point3f *)(object + 0x30), &delta);
            stationary = dot3f(&delta, &delta) < 0.002500000176951289f;
        }
        if (!stationary)
        {
            cache->sample_time = g_4ba034;
            if ((byte)function_d1850(object_index, force, &cache->target))
            {
                cache->valid = true;
                cache->position = *(point3f *)(object + 0x30);
            }
        }
    }
    if (!cache->valid) return;
    if (cache->initialized && g_467464 && !update)
    {
        s_effect_color_query *current = &cache->current;
        s_effect_color_query const *target = &cache->target;
        vector3f velocity;
        function_ba1d0(object_index, &velocity, 0);
        real blend = (magnitude3d(&velocity) - 0.15000000596046448f) * 0.03350083902478218f;
        if (blend < 0.0f) blend = 0.0f;
        else if (blend > 1.0f) blend = 1.0f;
        if (blend > 0.0f ||
            (distance3d(&cache->position, (point3f *)(object + 0x30)) > 0.05000000074505806f && blend > 0.0f) ||
            function_badc0(object_index, 0x80))
        {
            real half = blend * 0.5f;
            function_3e5a0((vector3f *)current, (vector3f const *)target, blend * 0.009999999776482582f);
            function_3e4e0(&current->color_b, &target->color_b, half);
            function_3e4e0(&current->color_a, &target->color_a, half);
        }
    }
    else
        cache->current = cache->target;
    long type = NONE;
    long special = 0;
    if (cache->parent != NONE)
    {
        byte *parent = ((s_3dd10_object_header *)g_4e0300->data)[cache->parent & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
        type = *(short *)definition;
        if ((bool)(((*(dword *)(parent + 4)) >> 13) & 1)) special = 1;
        if ((bool)((*(word *)(definition + 2) >> 1) & 1)) special |= 1;
    }
    function_d4080(type, &cache->current, &cache->lighting, special);
    cache->initialized = true;
}
