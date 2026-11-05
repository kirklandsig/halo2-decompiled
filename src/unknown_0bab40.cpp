// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0BAB40.CPP: reading the value of a named object function (built with
   SSE, apart from its neighbours in unknown_0bad50.cpp) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "object_iterator.h"
#include "unknown_16d180.h"
#include <string.h>
#include "object_masks_storage.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"

struct s_object
{
	byte unknown00[4];
	dword : 26;
	dword flag26 : 1;
	dword : 5;
	byte unknown08[0xc];
	long parent_index;
	byte unknown18[0xc0 - 0x18];
    word flags_c0;
    byte unknownc2[0x124 - 0xc2];
	short markers_size;
	short markers_offset;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_tag_data
{
    long size;
    byte *address;
};
struct s_object_function_ab
{
    dword flags;
    long input_name;
    long output_name;
    long enable_name;
    real threshold;
    s_tag_data curve;
    long scale_name;
};
struct s_object_functions_definition_ab
{
    byte unknown00[0x64];
    long count;
    s_object_function_ab *functions;
};
bool object_dispatch_query_function_value(long object_index, long name, real *value, bool *enabled);
bool function_10aac0(long object_index, long name, real *value, long *index);
bool function_ba8c0(s_object_function_ab const *function, long object_index, real *out);

// @retail 0xbab40
bool __stdcall function_bab40(long object_index, long name, real *value)
{
    bool enabled = false;
    long index;
    *value = 0.0f;
    s_object_header const *headers = (s_object_header const *)g_4e0300->data;
    s_object *object = headers[object_index & 0xffff].object;
    if ((bool)(((dword)object->flags_c0 >> 9) & 1) && function_10aac0(object_index, name, value, &index))
        return true;
    // found: the value came from the object itself (or there is none) and `enabled` is returned;
    // otherwise the value is a constant and `result` is returned
    bool found = true;
    bool result;
    if (name == 0 || name == 0x030005a6)
    {
        *value = 1.0f;
        result = true;
        found = false;
    }
    else if (name == 0x040005a7)
    {
        *value = 0.0f;
        return false;
    }
    else if (object_index != NONE)
    {
        found = object_dispatch_query_function_value(object_index, name, value, &enabled);
        if (!found)
        {
            object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
            s_object_functions_definition_ab *definition = (s_object_functions_definition_ab *)g_4e3b44[*(long *)object & 0xffff].bytes;
            for (long i = 0; i < definition->count; ++i)
            {
                s_object_function_ab *function = &definition->functions[i];
                if (function->output_name == name)
                {
                    enabled = function_ba8c0(function, object_index, value);
                    found = true;
                }
            }
            if (!found)
            {
                if (TEST_FIELD_BIT(object->flag26) && object->parent_index != NONE)
                {
                    enabled = function_bab40(object->parent_index, name, value);
                    return enabled;
                }
                *value = 0.0f;
                result = false;
            }
        }
    }
    if (!found)
        return result;
    return enabled;
}
