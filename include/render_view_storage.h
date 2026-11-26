#ifndef RENDER_VIEW_STORAGE_H
#define RENDER_VIEW_STORAGE_H

#include <stddef.h>
#include "camera_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"

struct s_render_view_storage
{
    s_camera_storage camera;
    s_projection_storage projection;
    s_view_context_storage view_context;
    bool field_270;
    byte unknown_271[3];
};

extern s_render_view_storage g_render_view_storage;

typedef char assert_render_view_offset_0[(offsetof(s_render_view_storage, camera) == 0x0) ? 1 : -1];
typedef char assert_render_view_offset_74[(offsetof(s_render_view_storage, projection) == 0x74) ? 1 : -1];
typedef char assert_render_view_offset_134[(offsetof(s_render_view_storage, view_context) == 0x134) ? 1 : -1];
typedef char assert_render_view_offset_270[(offsetof(s_render_view_storage, field_270) == 0x270) ? 1 : -1];
typedef char assert_render_view_size[(sizeof(s_render_view_storage) == 0x274) ? 1 : -1];

#endif
