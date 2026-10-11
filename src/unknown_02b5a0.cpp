// @flags /O2 /Gr
/* UNKNOWN_02B5A0.CPP: pool and caption-cache lifecycle */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include <string.h>

s_record_pool *g_509434;
double g_4ba040;
byte g_4c5018[0x6a8];
extern long g_4ba134;
extern long g_4b9970[12];
extern byte g_5093fc;
long g_4c1bd0;

bool object_or_parent_hidden(long object_index);

// @retail 0x3d7c0
bool function_3d7c0(long object_index, bool *out)
{
	bool result = false;
	/* Retail materializes the false fallback in a stack byte. */
	volatile bool fallback = false;
	bool current;
	if (!object_or_parent_hidden(object_index))
	{
		result = true;
		current = object_index == g_4c1bd0;
	}
	else
		current = fallback;
	if (out)
		*out = current;
	return result;
}

void function_3b950(void);

// @retail 0x2b5c0
void function_2b5c0(void)
{
	function_3b950();
	g_509434 = 0;
}

// @retail 0x2b540
void function_2b540(void)
{
	g_4ba040 = 0.0;
	*(volatile long *)&g_4ba134 = 0;
	g_509434->valid = true;
	record_pool_release_all(g_509434);
	memset(g_4b9970, 0, sizeof(g_4b9970));
	memset(g_4c5018, 0, sizeof(g_4c5018));
	g_4b9970[0] = NONE;
	g_5093fc = false;
}

// @retail 0x2b5a0
void function_02b5a0(void)
{
	if (g_509434)
	{
		if (g_509434->valid)
		{
			g_509434->valid = false;
		}
	}
}


bool function_bad50(long object_index, long index, point3f *out);
dword __cdecl pack_color3f(color3f const *color);

struct s_3dd10_object_header
{
	byte unknown00[8];
	byte *object;
};

struct s_3dd10_cache
{
	byte unknown00[0xa8];
	dword colors[4];
	byte count;
	byte unknownb9[0x47];
};

// @retail 0x3dd10
void function_3dd10(long object_index, bool force)
{
	(void)&object_index;
	(void)&force;
	byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	long cache_index = *(long *)(object + 0xcc);
	if (cache_index != NONE && (force || (g_4e3b44[*(long *)object & 0xffff].bytes[0x1c] & 1)))
	{
		s_3dd10_cache *cache = &((s_3dd10_cache *)g_509434->data)[cache_index & 0xffff];
		cache->count = 0;
		for (long i = 0; i < 4; ++i)
		{
			point3f color;
			if (function_bad50(object_index, i, &color))
			{
				cache->colors[i] = pack_color3f((color3f const *)&color);
				++cache->count;
			}
		}
	}
}

extern dword g_4ba034;
long g_4ba050;
struct s_interface_function_context;
void function_2c3b0(s_interface_function_context *context);
real __stdcall function_be8b0(long object_index, long name);
struct s_object_list;
extern s_object_list *g_4de2f4;
long function_baf80(long object_index);

// @retail 0x3ddd0
long __stdcall function_3ddd0(long object_index)
{
    (void)&object_index;
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    long volatile result = *(long *)(object + 0xcc);
    bool initialize = false;
    if (result == NONE || *(long *)(g_509434->data + (result & 0xffff) * 0x100 + 4) != object_index)
    {
        result = record_pool_allocate(g_509434);
        if (result == NONE)
        {
            real oldest = -3.402823466e+38f;
            long absolute = function_16bc00(g_509434, 0);
            long index = data_datum_index(g_509434, absolute);
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
                absolute = function_16bc00(g_509434, (index & 0xffff) + 1);
                index = data_datum_index(g_509434, absolute);
            }
        }
        if (result == NONE) return result;
        initialize = true;
    }
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
    return result;
}

// @retail 0x3d430
signed char *function_3d430(long object_index)
{
    (void)&object_index;
    signed char *result = NULL;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
            result = (signed char *)(g_509434->data + (cache & 0xffff) * 0x100 + 0xc0 + g_4ba050 * 16);
    }
    return result;
}

// @retail 0x4c640
void function_4c640(long object_index, signed char value)
{
    (void)&object_index;
    (void)&value;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        byte *slot = NULL;
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
            slot = g_509434->data + (cache & 0xffff) * 0x100 + 0xbc;
        slot[g_4ba050] = (byte)value;
    }
}

// @retail 0x3dc90
byte *__stdcall function_3dc90(long object_index)
{
    (void)&object_index;
    byte *result = NULL;
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    if (object[7] & 1)
        result = (byte *)g_4de2f4 + 0x1c;
    else if (*(long *)(object + 0x14) != NONE)
        result = function_3dc90(function_baf80(object_index));
    else
    {
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
        {
            byte *record = g_509434->data + (cache & 0xffff) * 0x100;
            if (record[3]) result = record + 0x54;
        }
    }
    return result;
}

// @retail 0x3e420
void function_3e420(long tag, long object_index, signed char *output, byte level)
{
    (void)&level;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    signed char *current = NULL;
    if (object_index != NONE) current = function_3d430(object_index);
    for (long i = 0; i < *(long *)(definition + 0x1c); ++i)
    {
        byte *group = *(byte **)(definition + 0x20) + i * 16;
        long index = current ? current[i] : 0;
        if (index != NONE)
        {
            if (index < 0) index = 0;
            else if (index > *(long *)(group + 8) - 1) index = *(long *)(group + 8) - 1;
            byte *variant = *(byte **)(group + 0xc) + index * 16;
            output[i] = variant[4 + level * 2];
        }
        else output[i] = (signed char)0xff;
    }
}

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
bool g_467464 = true;
long function_d1850(long object_index, long value, s_effect_color_query *query);
void function_d4080(s_effect_color_query const *query, long type, s_lighting_record *record, bool flag);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
real magnitude3d(vector3f const *vector);
real distance3d(point3f const *first, point3f const *second);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3e5a0(vector3f *current, vector3f const *target, real step);
void function_3e4e0(dword *current, dword const *target, real step);

// @retail 0x3dfe0
void function_3dfe0(long cache_index, long object_index, real priority)
{
    (void)&cache_index; (void)&object_index;
    s_3dfe0_cache *cache = (s_3dfe0_cache *)(g_509434->data + (cache_index & 0xffff) * 0x100);
    long elapsed = g_4ba034 - cache->sample_time;
    if (!elapsed) return;
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    long update_elapsed = g_4ba034 - cache->update_time;
    bool volatile force = !cache->initialized || !cache->valid;
    bool &update = *(bool *)&cache_index;
    update = false;
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
            delta.i = *(real *)(object + 0x30) - cache->position.x;
            delta.j = *(real *)(object + 0x34) - cache->position.y;
            delta.k = *(real *)(object + 0x38) - cache->position.z;
            stationary = delta.k * delta.k + delta.i * delta.i + delta.j * delta.j < 0.002500000176951289f;
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
        vector3f velocity;
        function_ba1d0(object_index, &velocity, 0);
        real blend = (magnitude3d(&velocity) - 0.15000000596046448f) * 0.03350083902478218f;
        if (blend < 0.0f) blend = 0.0f;
        else if (blend > 1.0f) blend = 1.0f;
        if (blend > 0.0f ||
            (distance3d(&cache->position, (point3f *)(object + 0x30)) > 0.05000000074505806f && blend > 0.0f) ||
            function_badc0(object_index, 0x80))
        {
            function_3e5a0((vector3f *)&cache->current, (vector3f const *)&cache->target, blend * 0.009999999776482582f);
            function_3e4e0(&cache->current.color_b, &cache->target.color_b, blend * 0.5f);
            function_3e4e0(&cache->current.color_a, &cache->target.color_a, blend * 0.5f);
        }
    }
    else cache->current = cache->target;
    long type = NONE;
    bool special = false;
    if (cache->parent != NONE)
    {
        byte *parent = ((s_3dd10_object_header *)g_4e0300->data)[cache->parent & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
        type = *(short *)definition;
        special = (bool)(((*(dword *)(parent + 4)) >> 13) & 1) || (bool)((definition[2] >> 1) & 1);
    }
    function_d4080(&cache->current, type, &cache->lighting, special);
    cache->initialized = true;
}

s_lighting_record g_55ee60;

// @retail 0x3db00
s_lighting_record *function_3db00(long object_index, real priority)
{
    (void)&priority;
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
    bool special = (bool)(((*(dword *)(parent + 4)) >> 13) & 1) || (bool)((definition[2] >> 1) & 1);
    s_effect_color_query query;
    function_d1850(parent_index, 1, &query);
    s_lighting_record lighting;
    function_d4080(&query, *(short *)definition, &lighting, special);
    g_55ee60 = lighting;
    return &g_55ee60;
}


// Disabled: camera fallback passes shared cache storage to an outside-owner producer.
#if 0
extern long g_4c1bd4;
extern long g_4c1bd8[4];
extern long g_4b72c0;
struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
void *function_badc0(long object_index, dword mask);
bool function_155760(long player_index);
long __stdcall function_165f16(long player_index, long object_index, long count, long *cache);

// Retail 0x3d2c0
void function_3d2c0()
{
    g_4c1bd0 = NONE;
    g_4c1bd4 = 0;
    long object_index = NONE;
    byte *camera = (byte *)g_510c6c;
    if (camera[0] && *(short *)(camera + 2) == 4)
    {
        long target = *(long *)(camera + 0x3c);
        if (function_badc0(target, 3)) object_index = target;
    }
    g_4c1bd0 = object_index;
    if (object_index == NONE)
    {
        g_4c1bd0 = g_4b72c0;
        if (g_4c1bd0 == NONE && g_4b9ed8 != NONE && !function_155760(g_4b9ed8))
        {
            long player = g_4e8c20->entries[g_4b9ed8];
            long unit = player != NONE ? *(long *)(g_4e8c24->data + (player & 0xffff) * 0x21c + 0x2c) : NONE;
            g_4c1bd0 = unit;
            g_4c1bd4 = function_165f16(g_4b9ed8, unit, 4, g_4c1bd8);
        }
    }
}
#endif

