// @flags /O2 /Ob1 /Gr /arch:SSE
/* Legacy recorded animation streams. Existing field readers remain in
   unknown_29f180.cpp with their upstream definitions. */
#include "unknown_11c920.h"

struct s_type_339e8b;
struct playback_unit_control_view;

struct s_type_4ac09b
{
    short type;
    unsigned short ticks;
};

void __stdcall function_29f180(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f1a0(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f1c0(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f1e0(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f200(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f220(byte *dest, real *src, byte **cursor);
void __stdcall function_29f250(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f280(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f2b0(byte *dest, byte *src, byte **cursor);
void __stdcall function_29f2e0(byte *dest, word *src, byte **cursor);
void __stdcall function_29f370(byte *dest, word *src, byte **cursor);

void function_2c4e10(playback_unit_control_view *control,
    byte const **cursor, byte version);

typedef void (__stdcall *legacy_event_handler)(playback_unit_control_view *,
    s_type_4ac09b const *, byte const **);

legacy_event_handler const g_471220[24] =
{
    NULL,
    NULL,
    (legacy_event_handler)function_29f180,
    (legacy_event_handler)function_29f1a0,
    (legacy_event_handler)function_29f1c0,
    (legacy_event_handler)function_29f1e0,
    (legacy_event_handler)function_29f220,
    NULL,
    NULL,
    (legacy_event_handler)function_29f250,
    (legacy_event_handler)function_29f280,
    (legacy_event_handler)function_29f2b0,
    (legacy_event_handler)function_29f370,
    (legacy_event_handler)function_29f370,
    (legacy_event_handler)function_29f370,
    (legacy_event_handler)function_29f370,
    (legacy_event_handler)function_29f200,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0,
    (legacy_event_handler)function_29f2e0
};

// @retail 0x29f3e0
void __stdcall function_29f3e0(s_type_339e8b *controller,
    playback_unit_control_view *control, byte const **cursor, byte version)
{
    function_2c4e10(control, cursor, version);
}

// @retail 0x29f400
bool __stdcall function_29f400(s_type_339e8b *controller,
    playback_unit_control_view *control, long *remaining_ticks, byte const **cursor)
{
    s_type_4ac09b const *event = (s_type_4ac09b const *)*cursor;
    while (*remaining_ticks >= event->ticks)
    {
        if (event->type == 1)
            break;
        legacy_event_handler handler = g_471220[event->type];
        if (handler)
            handler(control, event, cursor);
        else
            *cursor = (byte const *)(event + 1);
        *remaining_ticks -= event->ticks;
        event = (s_type_4ac09b const *)*cursor;
    }
    bool result = event->type != 1 || *remaining_ticks != event->ticks;
    return result;
}

struct legacy_playback_functions
{
    void (__stdcall *initialize)(s_type_339e8b *, playback_unit_control_view *,
        byte const **, byte);
    bool (__stdcall *apply)(s_type_339e8b *, playback_unit_control_view *,
        long *, byte const **);
};

// Retail's legacy codec pair; the newer format's pair is at 0x46fd4c.
legacy_playback_functions const g_46fd54 =
{
    function_29f3e0,
    function_29f400
};
