// @flags /O2 /arch:SSE /Gr
#include "saved_frames_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_02C560.CPP: nested view setup and auxiliary rendering */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "unknown_030290.h"
#include "unknown_234c64.h"
#include "object_queries.h"
#include <string.h>

struct s_2f800_source;
struct s_2f800_size;
struct s_2f800_view;
struct s_2f970_view;
struct s_frame_parameters_12a50
{
    long mode;
    dword identifier;
    double time;
    dword unknown10, unknown14;
};

struct short_rect { short v0, v1, v2, v3; };
struct short_rect_pair { short_rect a, b; };

extern bool g_4ba019;
extern long g_4ba038;
extern dword g_4ba034, g_467014, g_467018;
extern point3f *g_468718;
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)
extern char const *g_4e9bb8;
bool g_4ba010;
long g_4ba030;
long g_46701c = NONE;
long g_4e9194;

void function_2fd90(s_2f800_view const *view, box2f const *clip, byte *out);
bool function_2f970(s_2f970_view const *view, real *out);
void function_2f800(s_2f800_view *view, s_2f800_source const *source, s_2f800_size const *first, s_2f800_size const *second);
void function_12a50(s_frame_parameters_12a50 const *parameters);
void __stdcall function_132d0(s_frame_view_2c560 const *frame);
void function_13c20(void);
void function_13cd0(void);
void function_13c780(void);
void function_13e635(void);
void function_147af1(long index, long fallback_index, c_render_window *window);
void function_156040(void);
long map_location_get(char const *map_name);
real cache_copy_progress_for_map(char const *map_name);
void function_11bed0(s_location *location, point3f const *point);
void function_3d2c0(void);
void function_3ea60(void);
void function_c15a0(void);
void __stdcall function_2bcd0(long first, bool second, bool enabled, bool fourth,
    long fifth, long sixth, real value);

/* Store each existing view fragment within its declared object. */
PRIVATE __forceinline void store_current_view(byte const *view)
{
    g_4b9da0 = *(point3f const *)view;
    g_4b9dac = *(vector3f const *)(view + 0xc);
    g_4b9db4 = *(real const *)(view + 0x14);
    g_4b9db8 = *(vector3f const *)(view + 0x18);
    g_4b9dc4 = *(dword const *)(view + 0x24);
    g_4b9dc8 = *(real const *)(view + 0x28);
    g_4b9dcc = *(real const *)(view + 0x2c);
    g_4b9dd0 = *(short const *)(view + 0x30);
    g_4b9dd2 = *(short const *)(view + 0x32);
    g_4b9dd4 = *(short const *)(view + 0x34);
    g_4b9dd6 = *(short const *)(view + 0x36);
    g_4b9dd8 = *(short_rectangle2d const *)(view + 0x38);
    g_4b9de0 = *(real const *)(view + 0x40);
    g_4b9de4 = *(real const *)(view + 0x44);
    memcpy(g_4b9de8, view + 0x48, 0x10);
    g_4b9df8 = *(dword const *)(view + 0x58);
    g_4b9dfc = *(real const *)(view + 0x5c);
    g_4b9e00 = *(real const *)(view + 0x60);
    g_4b9e04 = *(real const *)(view + 0x64);
    g_4b9e08 = *(real const *)(view + 0x68);
    g_4b9e0c = *(real const *)(view + 0x6c);
    g_4b9e10 = *(real const *)(view + 0x70);
}

PRIVATE __forceinline void store_current_projection(byte const *projection)
{
    g_4b9e14 = *(s_camera const *)projection;
    g_4b9e20 = *(real const *)(projection + 0xc);
    g_4b9e2c = *(real const *)(projection + 0x18);
    g_4b9e38 = *(real const *)(projection + 0x24);
    g_4b9e44 = *(real const *)(projection + 0x30);
    memcpy(g_4b9eb4, projection + 0xa0, sizeof(g_4b9eb4));
    g_4b9ecc = *(point2f const *)(projection + 0xb8);
    g_4b9ed0 = *(real const *)(projection + 0xbc);
}

// @retail 0x2c790
void __stdcall function_2c790(byte const *source)
{
    g_4b9ed4 = NONE;
    memcpy(&g_camera_storage, source + 0xc, sizeof(g_camera_storage));
    function_2fd90((s_2f800_view const *)&g_camera_storage, 0, (byte *)&g_projection_storage);
    s_frame_view_2c560 frame;
    memset(&frame, 0, sizeof(frame));
    memcpy(frame.view, source + 0x80, sizeof(frame.view));
    function_2fd90((s_2f800_view const *)frame.view, 0, frame.projection);
    frame.mode = 0;
    if (*(long const *)source == 1) frame.location = 0;
    else { frame.location = 1; frame.position = *g_468718; }
    long saved_index = ++g_46701c;
    frame.index = NONE;
    g_4850d0[saved_index] = frame;
    function_132d0(&g_4850d0[saved_index]);
    /* retail stores the view's mode to the stack and never reads it back */
    long volatile unused_view_mode = *(long const *)source;
    function_13c780();
    function_13e635();
    function_147af1(4, NONE, (c_render_window *)&g_4b9dd0);
    function_13c20();
}

typedef void (__stdcall *t_aux_view_callback)(void *context);

// @retail 0x2ca00
void function_2ca00(point3f const *position, t_aux_view_callback callback, void *context)
{
    struct s_mode_parameters : s_frame_parameters_12a50
    {
        s_mode_parameters(long frame_mode)
        {
            memset(this, 0, sizeof(*this));
            mode = frame_mode;
        }
    };
    ++g_4ba034;
    function_12a50(&s_mode_parameters(2));
    s_frame_view_2c560 frame;
    memset(&frame, 0, sizeof(frame));
    frame.mode = 0;
    point3f const *source = g_468718;
    if (position) source = position;
    frame.position = *source;
    frame.location = 1;
    function_2f800((s_2f800_view *)frame.view, 0, 0, 0);
    memcpy(frame.view + 0x30, &g_485a8a, sizeof(g_485a8a));
    function_2fd90((s_2f800_view const *)frame.view, 0, frame.projection);
    long saved_index = ++g_46701c;
    g_camera_storage = *(s_camera_storage const *)frame.view;
    g_4850d0[saved_index] = frame;
    function_132d0(&g_4850d0[saved_index]);
    if (callback) callback(context);
    function_13c20();
    function_13cd0();
}

// @retail 0x2c8a0 standard
void __stdcall function_2c8a0(byte const *source)
{
    s_frame_parameters_12a50 parameters;
    g_4ba030 = 1;
    ++g_4ba034;
    memset(&parameters, 0, sizeof(parameters));
    parameters.mode = 1;
    function_12a50(&parameters);
    s_frame_view_2c560 frame;
    memset(&frame, 0, sizeof(frame));
    memcpy(&g_camera_storage, source + 0xc, sizeof(g_camera_storage));
    function_2fd90((s_2f800_view const *)&g_camera_storage, 0, (byte *)&g_projection_storage);
    memcpy(frame.view, source + 0x80, sizeof(frame.view));
    function_2fd90((s_2f800_view const *)frame.view, 0, frame.projection);
    frame.position = *g_468718;
    frame.mode = 0;
    frame.location = 1;
    long saved_index = ++g_46701c;
    g_4850d0[saved_index] = frame;
    function_132d0(&g_4850d0[saved_index]);
    if (g_4e9194) function_156040();
    *(long *)g_54d598.unknown00 = NONE;
    function_147af1(4, NONE, (c_render_window *)(source + 0xb0));
    if (g_4e9bb8)
    {
        long location = map_location_get(g_4e9bb8);
        if (location != 3 && location != 4) cache_copy_progress_for_map(g_4e9bb8);
    }
    function_13c20();
    function_13cd0();
    g_4ba030 = 0;
}

// @retail 0x2c560
void function_2c560(short left, short top, short right, short bottom,
    vector3f const *up, real angle, vector3f const *forward, point3f const *position)
{
    s_location location;
    function_11bed0(&location, position);
    s_frame_view_2c560 frame;
    *(vector3f *)(frame.view + 0x18) = *forward;
    *(vector3f *)(frame.view + 0xc) = *up;
    *(short *)(frame.view + 0x30) = top;
    *(short *)(frame.view + 0x32) = left;
    *(short *)(frame.view + 0x34) = bottom;
    *(short *)(frame.view + 0x36) = right;
    memcpy(frame.view + 0x38, frame.view + 0x30, sizeof(short_rectangle2d));
    /* the two clip distances are reals: read them as such (retail copies them with movss) */
    *(real *)(frame.view + 0x40) = *(real const *)&g_467014;
    *(real *)(frame.view + 0x44) = *(real const *)&g_467018;
    *(real *)(frame.view + 0x28) = angle;
    *(real *)(frame.view + 0x2c) = 1.0f;
    *(point3f *)frame.view = *position;
    frame.view[0x24] = false;
    frame.view[0x58] = false; frame.view[0x68] = false;
    *(long *)(frame.unknown26c + 0x24) = 0;
    frame.index = NONE; frame.mode = 0; frame.flag06 = false; frame.location = 0;
    memset(frame.records, 0, sizeof(frame.records));
    *(long *)(frame.unknown26c + 4) = 0;
    *(long *)(frame.unknown26c + 0x1c) = 0;
    frame.unknown26c[0x28] = false;
    union { real clip[4]; double clip_alignment; } clip_storage;
    real *clip = clip_storage.clip;
    function_2f970((s_2f970_view const *)frame.view, clip);
    function_2fd90((s_2f800_view const *)frame.view, (box2f const *)clip, frame.projection);
    g_4ba019 = true;
    ++g_4ba038;
    g_4b9ed4 = frame.index;
    g_4b9ee4 = 0; g_4b9ee0 = 0; g_4b9ee8 = false;
    g_4b9ed8 = NONE; g_4b9edc = NONE; g_4b9eec = NONE; g_4b9ee9 = false;
    memcpy(g_view_context_storage.frame_records, frame.records, sizeof(frame.records));
    g_4ba010 = true;
    memcpy(&g_camera_storage, frame.view, sizeof(g_camera_storage));
    function_2fd90((s_2f800_view const *)&g_camera_storage, (box2f const *)clip, (byte *)&g_projection_storage);
    function_3d2c0(); function_3ea60(); function_c15a0();
    long saved_index = ++g_46701c;
    g_4850d0[saved_index] = frame;
    function_132d0(&g_4850d0[saved_index]);
    function_2bcd0(2, 1, false, 0, 0, 0, 0.0f);
    function_13c20();
    g_4ba010 = false;
}

s_view_context_storage g_view_context_storage;

s_saved_frames_storage g_saved_frames_storage;
