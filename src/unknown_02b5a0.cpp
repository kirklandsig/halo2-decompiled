// @flags /O2 /Gr
#include "target_cache_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_02B5A0.CPP: pool and caption-cache lifecycle */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include "unknown_3dfe0.h"
#include <string.h>

s_record_pool *g_509434;
double g_4ba040;
byte g_4c5018[0x6a8];
extern long g_4ba134;
extern long g_4b9970[12];
extern byte g_5093fc;

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
long __stdcall function_3ddd0(long object_index);

// @retail 0x3d430
signed char *function_3d430(long object_index)
{
    signed char *result = NULL;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
        {
            // Reload the shared channel index after cache setup.
            result = (signed char *)(g_509434->data + (cache & 0xffff) * 0x100 + 0xc0 + *(long const *)&g_4ba050 * 16);
        }
    }
    return result;
}

__declspec(noinline)
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
    signed char *current = NULL;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    if (object_index != NONE) current = function_3d430(object_index);
    for (long i = 0; i < *(long *)(definition + 0x1c); ++i)
    {
        byte *group = *(byte **)(definition + 0x20) + i * 16;
        bool missing_current = current == NULL;
        long index = missing_current ? 0 : current[i];
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

bool g_467464 = true;
long function_d1850(long object_index, bool value, s_effect_color_query *query);
void function_d4080(long type, s_effect_color_query const *query, long flag, s_lighting_record *record);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
real magnitude3d(vector3f const *vector);
real distance3d(point3f const *first, point3f const *second);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3e5a0(vector3f *current, vector3f const *target, real step);
void function_3e4e0(dword *current, dword const *target, real step);

void function_3dfe0(long cache_index, long object_index, real priority);

s_lighting_record g_55ee60;


// Disabled: camera fallback passes shared cache storage to an outside-owner producer.
#if 0
extern long g_4b72c0;
struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
struct s_object;
s_object *function_badc0(long object_index, dword mask);
long function_155760(long player_index);
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
            g_4c1bd4 = function_165f16(g_4b9ed8, unit, 4, (long *)g_4c1bd8);
        }
    }
}
#endif


struct s_first_person_model;
struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
long function_155760(long player_index);
long __stdcall first_person_weapons_get_models(long user_index, long unit_index, long maximum, s_first_person_model *models);
long g_4b72c0;
