#ifndef CAMERA_STORAGE_H
#define CAMERA_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"




struct short_rectangle2d { short top, left, bottom, right; };

struct s_camera_storage
{
    point3f field_0;
    union
    {
        struct { vector3f value; } field_c;
        struct { byte unknown00[0x8]; real value; } field_14;
    } group_c;
    vector3f field_18;
    dword field_24;
    real field_28;
    real field_2c;
    short field_30;
    short field_32;
    short field_34;
    short field_36;
    short_rectangle2d field_38;
    real field_40;
    real field_44;
    dword field_48[4];
    dword field_58;
    real field_5c;
    real field_60;
    real field_64;
    real field_68;
    real field_6c;
    real field_70;
};

#include "render_view_storage.h"
#define g_camera_storage (g_render_view_storage.camera)
#define g_4b9da0 (g_camera_storage.field_0)
#define g_4b9dac (g_camera_storage.group_c.field_c.value)
#define g_4b9db4 (g_camera_storage.group_c.field_14.value)
#define g_4b9db8 (g_camera_storage.field_18)
#define g_4b9dc4 (g_camera_storage.field_24)
#define g_4b9dc8 (g_camera_storage.field_28)
#define g_4b9dcc (g_camera_storage.field_2c)
#define g_4b9dd0 (g_camera_storage.field_30)
#define g_4b9dd2 (g_camera_storage.field_32)
#define g_4b9dd4 (g_camera_storage.field_34)
#define g_4b9dd6 (g_camera_storage.field_36)
#define g_4b9dd8 (g_camera_storage.field_38)
#define g_4b9de0 (g_camera_storage.field_40)
#define g_4b9de4 (g_camera_storage.field_44)
#define g_4b9de8 (g_camera_storage.field_48)
#define g_4b9df8 (g_camera_storage.field_58)
#define g_4b9dfc (g_camera_storage.field_5c)
#define g_4b9e00 (g_camera_storage.field_60)
#define g_4b9e04 (g_camera_storage.field_64)
#define g_4b9e08 (g_camera_storage.field_68)
#define g_4b9e0c (g_camera_storage.field_6c)
#define g_4b9e10 (g_camera_storage.field_70)
typedef char assert_camera_offset_0[(offsetof(s_camera_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_camera_offset_c[(offsetof(s_camera_storage, group_c.field_c.value) == 0xc) ? 1 : -1];
typedef char assert_camera_offset_14[(offsetof(s_camera_storage, group_c.field_14.value) == 0x14) ? 1 : -1];
typedef char assert_camera_offset_18[(offsetof(s_camera_storage, field_18) == 0x18) ? 1 : -1];
typedef char assert_camera_offset_24[(offsetof(s_camera_storage, field_24) == 0x24) ? 1 : -1];
typedef char assert_camera_offset_28[(offsetof(s_camera_storage, field_28) == 0x28) ? 1 : -1];
typedef char assert_camera_offset_2c[(offsetof(s_camera_storage, field_2c) == 0x2c) ? 1 : -1];
typedef char assert_camera_offset_30[(offsetof(s_camera_storage, field_30) == 0x30) ? 1 : -1];
typedef char assert_camera_offset_32[(offsetof(s_camera_storage, field_32) == 0x32) ? 1 : -1];
typedef char assert_camera_offset_34[(offsetof(s_camera_storage, field_34) == 0x34) ? 1 : -1];
typedef char assert_camera_offset_36[(offsetof(s_camera_storage, field_36) == 0x36) ? 1 : -1];
typedef char assert_camera_offset_38[(offsetof(s_camera_storage, field_38) == 0x38) ? 1 : -1];
typedef char assert_camera_offset_40[(offsetof(s_camera_storage, field_40) == 0x40) ? 1 : -1];
typedef char assert_camera_offset_44[(offsetof(s_camera_storage, field_44) == 0x44) ? 1 : -1];
typedef char assert_camera_offset_48[(offsetof(s_camera_storage, field_48) == 0x48) ? 1 : -1];
typedef char assert_camera_offset_58[(offsetof(s_camera_storage, field_58) == 0x58) ? 1 : -1];
typedef char assert_camera_offset_5c[(offsetof(s_camera_storage, field_5c) == 0x5c) ? 1 : -1];
typedef char assert_camera_offset_60[(offsetof(s_camera_storage, field_60) == 0x60) ? 1 : -1];
typedef char assert_camera_offset_64[(offsetof(s_camera_storage, field_64) == 0x64) ? 1 : -1];
typedef char assert_camera_offset_68[(offsetof(s_camera_storage, field_68) == 0x68) ? 1 : -1];
typedef char assert_camera_offset_6c[(offsetof(s_camera_storage, field_6c) == 0x6c) ? 1 : -1];
typedef char assert_camera_offset_70[(offsetof(s_camera_storage, field_70) == 0x70) ? 1 : -1];

typedef char assert_camera_size[(sizeof(s_camera_storage) == 0x74) ? 1 : -1];

#endif
