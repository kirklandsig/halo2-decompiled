#ifndef PROJECTION_STORAGE_H
#define PROJECTION_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"


struct s_camera
{
	real scale;
	vector3f right;
	vector3f up;
	vector3f forward;
	point3f position;
	byte unknown34[0x44];
	real unknown78;
	byte unknown7c[0x10];
	real unknown8c;
	byte unknown90[8];
	real unknown98;
	real unknown9c;
};

struct s_projection_storage
{
    union
    {
        struct { s_camera value; } field_0;
        struct { byte unknown00[0xc]; real value; } field_c;
        struct { byte unknown00[0x18]; real value; } field_18;
        struct { byte unknown00[0x24]; real value; } field_24;
        struct { byte unknown00[0x30]; real value; } field_30;
        struct { byte unknown00[0x38]; vector3f value; } field_38;
        struct { byte unknown00[0x44]; vector3f value; } field_44;
        struct { byte unknown00[0x50]; vector3f value; } field_50;
    } group_0;
    byte field_a0[0x18];
    union
    {
        struct { point2f value; } field_b8;
        struct { byte unknown00[0x4]; real value; } field_bc;
    } group_b8;
};

#include "render_view_storage.h"
#define g_projection_storage (g_render_view_storage.projection)
#define g_4b9e14 (g_projection_storage.group_0.field_0.value)
#define g_4b9e20 (g_projection_storage.group_0.field_c.value)
#define g_4b9e2c (g_projection_storage.group_0.field_18.value)
#define g_4b9e38 (g_projection_storage.group_0.field_24.value)
#define g_4b9e44 (g_projection_storage.group_0.field_30.value)
#define g_4b9e4c (g_projection_storage.group_0.field_38.value)
#define g_4b9e58 (g_projection_storage.group_0.field_44.value)
#define g_4b9e64 (g_projection_storage.group_0.field_50.value)
#define g_4b9eb4 (g_projection_storage.field_a0)
#define g_4b9ecc (g_projection_storage.group_b8.field_b8.value)
#define g_4b9ed0 (g_projection_storage.group_b8.field_bc.value)
typedef char assert_projection_offset_0[(offsetof(s_projection_storage, group_0.field_0.value) == 0x0) ? 1 : -1];
typedef char assert_projection_offset_c[(offsetof(s_projection_storage, group_0.field_c.value) == 0xc) ? 1 : -1];
typedef char assert_projection_offset_18[(offsetof(s_projection_storage, group_0.field_18.value) == 0x18) ? 1 : -1];
typedef char assert_projection_offset_24[(offsetof(s_projection_storage, group_0.field_24.value) == 0x24) ? 1 : -1];
typedef char assert_projection_offset_30[(offsetof(s_projection_storage, group_0.field_30.value) == 0x30) ? 1 : -1];
typedef char assert_projection_offset_38[(offsetof(s_projection_storage, group_0.field_38.value) == 0x38) ? 1 : -1];
typedef char assert_projection_offset_44[(offsetof(s_projection_storage, group_0.field_44.value) == 0x44) ? 1 : -1];
typedef char assert_projection_offset_50[(offsetof(s_projection_storage, group_0.field_50.value) == 0x50) ? 1 : -1];
typedef char assert_projection_offset_a0[(offsetof(s_projection_storage, field_a0) == 0xa0) ? 1 : -1];
typedef char assert_projection_offset_b8[(offsetof(s_projection_storage, group_b8.field_b8.value) == 0xb8) ? 1 : -1];
typedef char assert_projection_offset_bc[(offsetof(s_projection_storage, group_b8.field_bc.value) == 0xbc) ? 1 : -1];

typedef char assert_projection_size[(sizeof(s_projection_storage) == 0xc0) ? 1 : -1];

#endif
