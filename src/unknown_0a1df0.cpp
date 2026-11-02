// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A1DF0.CPP: the impact effect event of a projectile (built with SSE,
   apart from its neighbours in unknown_09b910.cpp) */

#include "unit_requests.h"
#include "effects.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>
#include <xmmintrin.h>

__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
    point3f const *point, vector3f const *normal, word index, bool attached, long object_index,
    long node_index, bool alternate);

// @retail 0xa1df0
bool c_projectile_object_impact_effect_event::v11(long a, long const *entities, long c, void const *data)
{
    long object = function_a58d0(entities[0]);
    if (object == NONE)
        return true;
    {
        byte const *event = (byte const *)data;
        if (*(long const *)event != NONE && function_122c10(0x70726f6a, *(long const *)event) &&
            !(0.0f > *(real const *)(event + 4) || *(real const *)(event + 4) > 1.0f ||
            0.0f > *(real const *)(event + 8) || *(real const *)(event + 8) > 1.0f))
            function_fd0e0(*(long const *)event, *(real const *)(event + 4), *(real const *)(event + 8),
                (vector3f const *)(event + 0xc), (point3f const *)(event + 0x18),
                (vector3f const *)(event + 0x24), *(word const *)(event + 0x30), true,
                object, *(long const *)(event + 0x38), *(bool const *)(event + 0x34));
    }
    return true;
}
