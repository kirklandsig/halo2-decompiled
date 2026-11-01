#include "unknown_440dd8.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
// @flags /O2 /Gr
#include "counters_storage.h"

struct s_structure_bsp_reference;
struct s_bsp3d;
struct s_slot_entry_list;
extern s_bsp3d *g_4e033c;
extern s_slot_entry_list *g_4e0340;
extern s_connection_counter g_4e6398;

void function_d4e60(short arg_0);
void function_137da0();
void function_137d40();
bool cache_files_load_structure_bsp(s_structure_bsp_reference *arg_0);
void cache_files_unload_structure_bsp(s_structure_bsp_reference *arg_0);
void function_16e2c0(long arg_0);

/* switches the loaded structure bsp to bsp_index (NONE unloads it); bsp_index then holds the
   game's current bsp index, read again after function_137da0 */
// @retail 0x11c1b0 standard
bool __stdcall function_11c1b0(short bsp_index, bool arg_1)
{
    short requested_index = bsp_index;
    bsp_index = g_4e6948->index;
    short local_0 = bsp_index;
    bool result = false;
    if (requested_index == NONE || (requested_index >= 0 && requested_index < *(long *)((byte *)g_4e0350 + 0x210)))
    {
        if (g_4686c4 == requested_index && local_0 == requested_index)
            return true;
        for (dword local_2 = 0; local_2 < 68; local_2++)
        {
            if (g_440dd8[local_2].field_18)
                g_440dd8[local_2].field_18(requested_index);
        }
        if (local_0 != NONE && local_0 != requested_index)
        {
            function_137da0();
            bsp_index = g_4e6948->index;
        }
        if (g_4686c4 != NONE && g_4686c4 != requested_index)
        {
            if (arg_1)
            {
                cache_files_unload_structure_bsp((s_structure_bsp_reference *)(*(byte **)((byte *)g_4e0350 + 0x214) + g_4686c4 * 0x44));
            }
            g_4686c4 = NONE;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
        }
        bool local_1 = true;
        if (g_4686c4 != requested_index)
        {
            byte *local_4 = *(byte **)((byte *)g_4e0350 + 0x214) + requested_index * 0x44;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
            g_4686c4 = requested_index;
            if (cache_files_load_structure_bsp((s_structure_bsp_reference *)local_4))
            {
                function_16e2c0(*(long *)(local_4 + 0x14));
                long local_6 = *(long *)(local_4 + 0x1c);
                if (local_6 != NONE)
                    g_4e0344 = (s_structure_bsp_globals *)g_4e3b44[local_6 & 0xffff].bytes;
                else
                    g_4e0344 = NULL;
                g_4e0348 = (s_match_globals *)g_4e3b44[*(long *)(local_4 + 0x14) & 0xffff].bytes;
                g_4e0340 = *(s_slot_entry_list **)((byte *)g_4e0348 + 0x18);
                g_4e033c = (s_bsp3d *)g_4e0340;
            }
            else
            {
                g_4686c4 = NONE;
                local_1 = false;
                goto local_5;
            }
        }
        if (bsp_index != requested_index)
            function_137d40();
    local_5:
        g_4e6398 = g_485ab0;
        result = local_1;
    }
    return result;
}
