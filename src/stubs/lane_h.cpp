// Stubs for game functions called by lane H's code that are not decompiled yet.

#include <xtl.h>
#include <xonline.h>

struct s_player_profile;
struct s_shapes;
union point3f;
struct s_controller_event;



struct s_event;
struct s_event_response;

struct s_dialog_definition;

struct s_screen_parameters;
class c_class_1473c9;















struct s_network_session_player;



typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);

/* the zlib inflate routines below src/unknown_2cb5b0.cpp's stream code */
struct s_inflate_stream;
struct s_block_state;

// @stub 0x2ce9e0
long __stdcall function_2ce9e0(s_block_state *blocks, s_inflate_stream *stream, long result)
{
    return -2;
}

// @stub 0x2ce7b0
unsigned long __stdcall function_2ce7b0(unsigned long checksum, unsigned char const *buffer, unsigned long size)
{
    return 0;
}

// @stub 0x2cb510
long __stdcall function_2cb510(unsigned char *destination, long *destination_size, unsigned char const *source, long source_size, long level, block_allocate allocate, block_free release, void *opaque)
{
    return -1;
}



