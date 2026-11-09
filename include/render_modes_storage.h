#ifndef RENDER_MODES_STORAGE_H
#define RENDER_MODES_STORAGE_H

#include "unknown_11c920.h"


struct s_render_mode_44370
{
    long target;
    dword clear_flags, field_8_7;
    real clear_depth;
    byte clear_stencil, unknown11[7];
    short cull_mode, unknown1a;
    long group, unknown20;
};

extern s_render_mode_44370 const g_43fa1c[25];
typedef char assert_render_mode_size[(sizeof(s_render_mode_44370) == 0x24) ? 1 : -1];

#endif
