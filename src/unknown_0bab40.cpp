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

// score 23 (was 23): retail gives the 1.0 case its own epilogue (`mov al, 1`, no store of the
// enabled byte); ours stores the byte and shares the final return (the three jumps to that return
// differ with it). Before round 55 the recursive call's parent
// index and name sat in ECX/EAX where retail has EAX/ECX. Tried: `enabled = true; return enabled;`
// 117 (358 bytes: forwarded to `mov al, 1` and cross-jumped with the 0x10aac0 return); the result set
// before the value store 117; the 1.0 case as the chain's final else 28; a second `return enabled`
// in the dispatch branch 28; a separate register result for the 1.0 case and the final return 117.
// A plain `return true` in the 1.0 case 117 (the early return jumps into its pops); the early
// 0x10aac0 result as `enabled = true` at the head of the chain 124; a switch on the name 104;
// the early result as a named bool returned when set 28, with the 1.0 `return true` 117. The
// recursive call's EAX/ECX swap is absent from a compile-only build (an LTCG effect).
// Round 55: the recursive result through `enabled = function_bab40(...); return enabled;` 23 (kept: the
// call's EAX/ECX as retail; a parent index local or the parent test nested: no gain in a one-file
// screen); with that kept, the 1.0 case's `return true` 117, `return true` with the early result in a
// `found` local 117, through a `real one` local 117, `enabled = true; return enabled;` 117, the name
// tests chained as an else of the early test 117 (each 1.0 return cross-jumps into the early return).
// Round 24 (m3 lane 2): `result = true;` ahead of the 1.0 store, `result = enabled;` for the other paths
// and one `return result;` 117 (the constant still reaches the return and the early return still
// jumps into the 1.0 block's pops); the override and 1.0 paths sharing one `return true` at the end 117;
// the constant names setting a bool, then one `*value = one ? 1.0f : 0.0f; return one;` 141 (380
// bytes, the value pointer leaves EBP). Lead: the 1.0 path and the not-found path through one shared
// `*value = constant; return result;` tail (a goto, diagnostic only) keep the early return apart and
// give retail's 1.0 block with the store between the pops (compile-only), but under LTCG the value
// pointer leaves EBP: 112 with the tail after the loop, 112 with it at the 1.0 case. Compile-only, all
// still merged: the early test nested, through a named bool, a single-exit result, a forceinline
// helper for the override test or for everything after it, the 1.0 return through `enabled`.
// Round 466: Enabled declared after override, declared at entry but initialized after override,
// initialized on complete name/lookup paths, and that graph with its own one-value exit:
// 51/51/119/131; with override index scoped to the actual flag branch 51/51/119/131.
// Original output lifetimes and separate recursive result restored; constant epilogue remains.
// Round 107 (lane 8, fast): the zero case as `enabled = false` into the shared return 95 (VC copies
// the byte-load return into it, store kept, nothing forwarded); also the not-found case as an else
// setting `enabled = false` 94 (377 bytes); the early path as `enabled = true; return enabled;` with
// the 1.0 `return true` 117, with the 1.0 case `enabled = true; *value = 1.0f; return enabled;` 117
// (the early store is gone before the merge, the early `jne` joins the 1.0 block's pops); the 1.0
// case last after `if (name != 0 && name != 0x030005a6) {...; return enabled;}` with `return true`
// 117. Every form whose 1.0 return is a constant loses the early return's own epilogue.
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
    if (name == 0 || name == 0x030005a6)
    {
        *value = 1.0f;
        enabled = true;
    }
    else if (name == 0x040005a7)
    {
        *value = 0.0f;
        return false;
    }
    else if (object_index != NONE)
    {
        bool found = object_dispatch_query_function_value(object_index, name, value, &enabled);
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
                return false;
            }
        }
    }
    return enabled;
}
