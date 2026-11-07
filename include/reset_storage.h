#ifndef RESET_STORAGE_H
#define RESET_STORAGE_H

#include "unknown_11c920.h"
#include <stddef.h>




struct s_reset_storage
{
    /* the shader pointer at 0x4858c4 shares this storage: callers reload it after a call that writes
       another member (0x3a550 after 0x1bd50, which writes input_masks) */
    byte *unknown_4858c4;
    byte unknown_4858c8[0x485b48 - 0x4858c8];
    union
    {
        byte data[0x1fc0];
        struct
        {
            byte unknown_0[0x30];
            dword input_masks[5];
            byte unknown_44[0x1740 - 0x44];
            dword texture_masks[17][32];
        } layout;
        struct { byte value[0x1fc0]; } field_0;
    };
};

extern s_reset_storage g_reset_storage;
#define g_485b48 (g_reset_storage.field_0.value)
#define g_4858c4 (g_reset_storage.unknown_4858c4)

typedef char assert_reset_size[(sizeof(s_reset_storage) == 0x1fc0 + 0x284) ? 1 : -1];

#define g_485b78 (g_reset_storage.layout.input_masks)
#define g_487288 (g_reset_storage.layout.texture_masks)
typedef char assert_reset_input_masks[(offsetof(s_reset_storage, layout.input_masks) == 0x284 + 0x30) ? 1 : -1];
typedef char assert_reset_texture_masks[(offsetof(s_reset_storage, layout.texture_masks) == 0x284 + 0x1740) ? 1 : -1];
#endif
