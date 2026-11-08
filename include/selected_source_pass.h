#ifndef SELECTED_SOURCE_PASS_H
#define SELECTED_SOURCE_PASS_H

/* One render pass over a record-source group, written for a constant mode: the shape retail's
   per-mode pass routine (0x44370) takes where it is inlined with its mode table folded. */

#include "unknown_11c920.h"
#include "render_source_storage.h"

extern byte g_485a75, g_485a76;
extern long g_485898, g_467130, g_4858b4;
extern void *g_467134;
extern s_record_sources *g_467138;
extern bool g_4ba022, g_4ba023;
void function_14bc0(short index, short element, bool use_depth);
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil);
void function_15680(short mode);
void function_15370(short mode);
void function_44550();
void function_445d0();
void function_0496f0();

PRIVATE __forceinline void draw_selected_source_pass(long group, long mode, bool depth, bool normal_cull, long target, bool filter)
{
    byte enabled = (g_4c1a48[group].flags >> mode) & 1;
    if (enabled)
    {
        switch (mode)
        {
        case 2: enabled = g_4ba023; break;
        case 8: enabled = g_4ba022; break;
        }
    }
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = (void *)group;
        g_467138 = &g_4c1a48[group];
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (mode == 1 && !g_485a75 && !g_485a76) function_14b60(1, 0, 0.0f, 0);
        if (g_485898 < 4 || g_485898 > 7) function_15680(normal_cull);
        else function_15680(0);
        if (filter) function_0496f0();
        switch (mode)
        {
        case 3: case 16: enabled = true; break;
        }
    }
    else g_467130 = 0;
    function_15370(depth ? 1 : 0);
    if (enabled) { function_44550(); function_445d0(); }
}

#endif
