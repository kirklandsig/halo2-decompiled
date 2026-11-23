// @flags /O2 /Oi /Gr
#include "unknown_11c920.h"
#include "language.h"
#include "unknown_120d80.h"
#include <string.h>
#include "cache_copy_globals.h"

struct s_cache_file
{
    HANDLE handle;
    byte unknown04[0x800];
};

#include "cache_file_table.h"

void __stdcall function_2138f0(long arg_0, long arg_1);
void function_213970(void);

/* stores a preference that has changed, after telling the cache files */
static inline void preference_update(long *field, long value)
{
    if (*field != value)
    {
        function_2138f0(0, 6);
        *field = value;
        global_preferences_globals.dirty = true;
    }
}

/* resets the cache files' table and the cache-copy state, then brings the stored language and version up to date.
   Retail clears each of the two objects with one memset (0x385c bytes at 0x557c90, 0xb40 at 0x55b4f0), reads the
   cached language again after filling it, and compares each preference with its new value before saving that value
   across the call, which is what one update helper taking the value gives. */
// @retail 0x2136c0
void function_2136c0(void)
{
    memset(&g_557c90, 0, sizeof(g_557c90));
    memset(&g_55b4f0, 0, sizeof(g_55b4f0));
    g_557c90.current = NONE;
    g_55b4f0.done = true;
    g_55b4f0.task = NONE;
    g_55b4f0.slot = NONE;
    preference_update(&global_preferences_globals.current.unknown18, get_current_language());
    preference_update(&global_preferences_globals.current.unknown1e8, 0x2651);
    function_213970();
}
