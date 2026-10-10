// @flags /O2 /Ob1 /Gr /arch:SSE
/* UNKNOWN_29ED40.CPP: recorded input direction controllers.
   Existing field readers and function_29ed00 remain in
   unknown_29ec30.cpp with their upstream definitions. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_type_f72fa2
{
	short yaw;
	short pitch;
};

struct s_type_7e8dd0
{
	short yaw;
	short pitch;
};

// @retail 0x29ed40
PRIVATE void function_29ed40(s_type_f72fa2 const *data, s_type_7e8dd0 *controller)
{
	controller->yaw += data->yaw;
	if (controller->yaw > 1000)
		controller->yaw -= 1000;
	else if (controller->yaw < -1000)
		controller->yaw += 1000;
	controller->pitch += data->pitch;
}

// @retail 0x29ed80
PRIVATE void function_29ed80(vector3f *vector, s_type_7e8dd0 const *controller)
{
	real yaw = (real)controller->yaw * 0.00314159272f;
	real pitch = (real)controller->pitch * 0.00314159272f;
	vector->i = (real)(cos(yaw) * cos(pitch));
	vector->j = (real)(sin(yaw) * cos(pitch));
	vector->k = (real)sin(pitch);
}

// The callback table stores four stack arguments for each event handler.
struct s_type_339e8b
{
    s_type_7e8dd0 facing;
    s_type_7e8dd0 aiming;
    s_type_7e8dd0 looking;
};

struct playback_unit_control_view
{
    byte fields[0x28];
    vector3f facing;
    vector3f aiming;
    vector3f looking;
    byte fields4c[0x30];
};

struct s_type_02a46c
{
    byte type_and_time;
};

void function_29ed00(const char *data, short *controller);

// @retail 0x29edc0
PRIVATE void __stdcall function_29edc0(s_type_339e8b *controller,
    playback_unit_control_view *control, s_type_02a46c const *header, byte const **cursor)
{
    char const *data = (char const *)*cursor;
    short mask = (header->type_and_time >> 2) - 7;
    long facing = mask & 1;
    if (facing)
    {
        function_29ed00(data, (short *)&controller->facing);
        real yaw = (real)controller->facing.yaw * 0.00314159272f;
        real pitch = (real)controller->facing.pitch * 0.00314159272f;
        control->facing.i = (real)(cos(yaw) * cos(pitch));
        control->facing.j = (real)(sin(yaw) * cos(pitch));
        control->facing.k = (real)sin(pitch);
    }
    short aiming = mask & 2;
    if (aiming)
    {
        if (facing)
        {
            controller->aiming = controller->facing;
            control->aiming = control->facing;
        }
        else
        {
            function_29ed00(data, (short *)&controller->aiming);
            function_29ed80(&control->aiming, &controller->aiming);
        }
    }
    if (mask & 4)
    {
        if (facing)
        {
            controller->looking = controller->facing;
            control->looking = control->facing;
        }
        else if (aiming)
        {
            controller->looking = controller->aiming;
            control->looking = control->aiming;
        }
        else
        {
            function_29ed00(data, (short *)&controller->looking);
            function_29ed80(&control->looking, &controller->looking);
        }
    }
    *cursor += 2;
}

// @retail 0x29ef20
PRIVATE void __stdcall function_29ef20(s_type_339e8b *controller,
    playback_unit_control_view *control, s_type_02a46c const *header, byte const **cursor)
{
    s_type_f72fa2 const *data = (s_type_f72fa2 const *)*cursor;
    short mask = (header->type_and_time >> 2) - 15;
    long facing = mask & 1;
    if (facing)
    {
        function_29ed40(data, &controller->facing);
        real yaw = (real)controller->facing.yaw * 0.00314159272f;
        real pitch = (real)controller->facing.pitch * 0.00314159272f;
        control->facing.i = (real)(cos(yaw) * cos(pitch));
        control->facing.j = (real)(sin(yaw) * cos(pitch));
        control->facing.k = (real)sin(pitch);
    }
    short aiming = mask & 2;
    if (aiming)
    {
        if (facing)
        {
            controller->aiming = controller->facing;
            control->aiming = control->facing;
        }
        else
        {
            function_29ed40(data, &controller->aiming);
            function_29ed80(&control->aiming, &controller->aiming);
        }
    }
    if (mask & 4)
    {
        if (facing)
        {
            controller->looking = controller->facing;
            control->looking = control->facing;
        }
        else if (aiming)
        {
            controller->looking = controller->aiming;
            control->looking = control->aiming;
        }
        else
        {
            function_29ed40(data, &controller->looking);
            function_29ed80(&control->looking, &controller->looking);
        }
    }
    *cursor += 4;
}

// Existing field readers retain their upstream declarations.
void __stdcall function_29ec30(long a, byte *dest, long c, char **cursor);
void __stdcall function_29ec50(long a, byte *dest, long c, char **cursor);
void __stdcall function_29ec70(long a, byte *dest, long c, long **cursor);
void __stdcall function_29ec90(long a, byte *dest, long c, byte **cursor);
void __stdcall function_29ecb0(long a, byte *dest, long c, byte **cursor);
void __stdcall function_29ecd0(long a, byte *dest, long c, real **cursor);

typedef void (__stdcall *animation_event_handler)(s_type_339e8b *,
    playback_unit_control_view *, s_type_02a46c const *, byte const **);

// Retail event dispatch table: type is the header's upper six bits.
animation_event_handler const g_4710f0[24] =
{
    NULL,
    NULL,
    (animation_event_handler)function_29ec30,
    (animation_event_handler)function_29ec50,
    (animation_event_handler)function_29ec70,
    (animation_event_handler)function_29ec90,
    (animation_event_handler)function_29ecd0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29edc0,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    function_29ef20,
    (animation_event_handler)function_29ecb0
};

void function_2c4e10(playback_unit_control_view *control,
    byte const **cursor, byte version);

// @retail 0x29f080
void __stdcall function_29f080(s_type_339e8b *controller,
    playback_unit_control_view *control, byte const **cursor, byte version)
{
    function_2c4e10(control, cursor, version);
    *controller = *(s_type_339e8b const *)*cursor;
    *cursor += sizeof(s_type_339e8b);
}

// @retail 0x29f0c0
bool __stdcall function_29f0c0(s_type_339e8b *controller,
    playback_unit_control_view *control, long *remaining_ticks, byte const **cursor)
{
    s_type_02a46c const *header;
    unsigned short ticks;
    for (;;)
    {
        header = (s_type_02a46c const *)*cursor;
        unsigned short header_size;
        switch (header->type_and_time & 3)
        {
        case 0:
            ticks = 0;
            header_size = 1;
            break;
        case 1:
            ticks = 1;
            header_size = 1;
            break;
        case 2:
            ticks = (*cursor)[1];
            header_size = 2;
            break;
        case 3:
            ticks = *(unsigned short const *)(*cursor + 1);
            header_size = 3;
            break;
        }
        if (*remaining_ticks < ticks || (header->type_and_time >> 2) == 1)
            break;
        *cursor += header_size;
        animation_event_handler handler = g_4710f0[header->type_and_time >> 2];
        if (handler)
            handler(controller, control, header, cursor);
        *remaining_ticks -= ticks;
    }
    return (header->type_and_time >> 2) != 1 || *remaining_ticks != ticks;
}

struct recorded_animation_playback_functions
{
    void (__stdcall *initialize)(s_type_339e8b *, playback_unit_control_view *,
        byte const **, byte);
    bool (__stdcall *apply)(s_type_339e8b *, playback_unit_control_view *,
        long *, byte const **);
};

// Retail's current-format codec pair; the legacy pair starts at 0x46fd54.
recorded_animation_playback_functions const g_46fd4c =
{
    function_29f080,
    function_29f0c0
};
