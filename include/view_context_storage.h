#ifndef VIEW_CONTEXT_STORAGE_H
#define VIEW_CONTEXT_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"




struct s_view_context_storage
{
    long field_0;
    long field_4;
    long field_8;
    long field_c;
    long field_10;
    bool field_14;
    byte field_15;
    byte unknown_16[0x2];
    long field_18;
    /* 0x1c-0x13c: one 0x120-byte block, copied whole from a saved frame's records */
    union
    {
        struct
        {
            long field_1c;
            union
            {
                struct { byte value[0x11c]; } field_20;
                struct { byte unknown00[0x24]; real value; } field_44;
                struct { byte unknown00[0x64]; real value; } field_84;
                struct { byte unknown00[0x68]; long value; } field_88;
                struct { byte unknown00[0x84]; real value; } field_a4;
                struct { byte unknown00[0x88]; real value; } field_a8;
                struct { byte unknown00[0x8c]; real value; } field_ac;
                struct { byte unknown00[0x90]; real value; } field_b0;
                struct { byte unknown00[0x94]; byte value; } field_b4;
                struct { byte unknown00[0x98]; long value; } field_b8;
                struct { byte unknown00[0xa8]; real value; } field_c8;
                struct { byte unknown00[0xb0]; real value; } field_d0;
                struct { byte unknown00[0xdc]; real value; } field_fc;
                struct { byte unknown00[0xe0]; real value; } field_100;
                struct { byte unknown00[0xe4]; real value; } field_104;
                struct { byte unknown00[0xec]; dword value; } field_10c;
                struct { byte unknown00[0x104]; real value; } field_124;
                struct { byte unknown00[0x108]; real value; } field_128;
                struct { byte unknown00[0x110]; bool value; } field_130;
            } group_20;
        };
        byte frame_records[0x120];
    };
};

#include "render_view_storage.h"
#define g_view_context_storage (g_render_view_storage.view_context)
#define g_4ba010 (g_render_view_storage.field_270)
#define g_4b9ed4 (g_view_context_storage.field_0)
#define g_4b9ed8 (g_view_context_storage.field_4)
#define g_4b9edc (g_view_context_storage.field_8)
#define g_4b9ee0 (g_view_context_storage.field_c)
#define g_4b9ee4 (g_view_context_storage.field_10)
#define g_4b9ee8 (g_view_context_storage.field_14)
#define g_4b9ee9 (g_view_context_storage.field_15)
#define g_4b9eec (g_view_context_storage.field_18)
#define g_4b9ef0 (g_view_context_storage.field_1c)
#define g_4b9ef4 (g_view_context_storage.group_20.field_20.value)
#define g_4b9f18 (g_view_context_storage.group_20.field_44.value)
#define g_4b9f58 (g_view_context_storage.group_20.field_84.value)
#define g_4b9f5c (g_view_context_storage.group_20.field_88.value)
#define g_4b9f78 (g_view_context_storage.group_20.field_a4.value)
#define g_4b9f7c (g_view_context_storage.group_20.field_a8.value)
#define g_4b9f80 (g_view_context_storage.group_20.field_ac.value)
#define g_4b9f84 (g_view_context_storage.group_20.field_b0.value)
#define g_4b9f88 (g_view_context_storage.group_20.field_b4.value)
#define g_4b9f8c (g_view_context_storage.group_20.field_b8.value)
#define g_4b9f9c (g_view_context_storage.group_20.field_c8.value)
#define g_4b9fa4 (g_view_context_storage.group_20.field_d0.value)
#define g_4b9fd0 (g_view_context_storage.group_20.field_fc.value)
#define g_4b9fd4 (g_view_context_storage.group_20.field_100.value)
#define g_4b9fd8 (g_view_context_storage.group_20.field_104.value)
#define g_4b9fe0 (g_view_context_storage.group_20.field_10c.value)
#define g_4b9ff8 (g_view_context_storage.group_20.field_124.value)
#define g_4b9ffc (g_view_context_storage.group_20.field_128.value)
#define g_4ba004 (g_view_context_storage.group_20.field_130.value)
typedef char assert_view_context_offset_0[(offsetof(s_view_context_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_view_context_offset_4[(offsetof(s_view_context_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_view_context_offset_8[(offsetof(s_view_context_storage, field_8) == 0x8) ? 1 : -1];
typedef char assert_view_context_offset_c[(offsetof(s_view_context_storage, field_c) == 0xc) ? 1 : -1];
typedef char assert_view_context_offset_10[(offsetof(s_view_context_storage, field_10) == 0x10) ? 1 : -1];
typedef char assert_view_context_offset_14[(offsetof(s_view_context_storage, field_14) == 0x14) ? 1 : -1];
typedef char assert_view_context_offset_15[(offsetof(s_view_context_storage, field_15) == 0x15) ? 1 : -1];
typedef char assert_view_context_offset_18[(offsetof(s_view_context_storage, field_18) == 0x18) ? 1 : -1];
typedef char assert_view_context_offset_1c[(offsetof(s_view_context_storage, field_1c) == 0x1c) ? 1 : -1];
typedef char assert_view_context_offset_20[(offsetof(s_view_context_storage, group_20.field_20.value) == 0x20) ? 1 : -1];
typedef char assert_view_context_offset_44[(offsetof(s_view_context_storage, group_20.field_44.value) == 0x44) ? 1 : -1];
typedef char assert_view_context_offset_84[(offsetof(s_view_context_storage, group_20.field_84.value) == 0x84) ? 1 : -1];
typedef char assert_view_context_offset_88[(offsetof(s_view_context_storage, group_20.field_88.value) == 0x88) ? 1 : -1];
typedef char assert_view_context_offset_a4[(offsetof(s_view_context_storage, group_20.field_a4.value) == 0xa4) ? 1 : -1];
typedef char assert_view_context_offset_a8[(offsetof(s_view_context_storage, group_20.field_a8.value) == 0xa8) ? 1 : -1];
typedef char assert_view_context_offset_ac[(offsetof(s_view_context_storage, group_20.field_ac.value) == 0xac) ? 1 : -1];
typedef char assert_view_context_offset_b0[(offsetof(s_view_context_storage, group_20.field_b0.value) == 0xb0) ? 1 : -1];
typedef char assert_view_context_offset_b4[(offsetof(s_view_context_storage, group_20.field_b4.value) == 0xb4) ? 1 : -1];
typedef char assert_view_context_offset_b8[(offsetof(s_view_context_storage, group_20.field_b8.value) == 0xb8) ? 1 : -1];
typedef char assert_view_context_offset_c8[(offsetof(s_view_context_storage, group_20.field_c8.value) == 0xc8) ? 1 : -1];
typedef char assert_view_context_offset_d0[(offsetof(s_view_context_storage, group_20.field_d0.value) == 0xd0) ? 1 : -1];
typedef char assert_view_context_offset_fc[(offsetof(s_view_context_storage, group_20.field_fc.value) == 0xfc) ? 1 : -1];
typedef char assert_view_context_offset_100[(offsetof(s_view_context_storage, group_20.field_100.value) == 0x100) ? 1 : -1];
typedef char assert_view_context_offset_104[(offsetof(s_view_context_storage, group_20.field_104.value) == 0x104) ? 1 : -1];
typedef char assert_view_context_offset_10c[(offsetof(s_view_context_storage, group_20.field_10c.value) == 0x10c) ? 1 : -1];
typedef char assert_view_context_offset_124[(offsetof(s_view_context_storage, group_20.field_124.value) == 0x124) ? 1 : -1];
typedef char assert_view_context_offset_128[(offsetof(s_view_context_storage, group_20.field_128.value) == 0x128) ? 1 : -1];
typedef char assert_view_context_offset_130[(offsetof(s_view_context_storage, group_20.field_130.value) == 0x130) ? 1 : -1];

typedef char assert_view_context_size[(sizeof(s_view_context_storage) == 0x13c) ? 1 : -1];

#endif
