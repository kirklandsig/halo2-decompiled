#include <wchar.h>
// stubs for the game functions outside 0x60000..0x6ffff that lane D's code
// calls and that are not decompiled yet

class c_class_58d20;
struct s_session_member;

// @stub 0x74aa0
void function_074aa0(void)
{
}

// @stub 0x65340
void function_065340(void)
{
}

// @stub 0x95580
void __stdcall function_095580(void *stream, long message_type, long message_size, const void *message)
{
}


struct s_voice_routing;
struct s_voice_route;

// @stub 0x565c0
void __stdcall function_565c0(s_voice_routing *routing, unsigned long members, s_voice_route *route)
{
}

class c_class_6a600;

// @stub 0x693a0
void __stdcall function_693a0(c_class_6a600 *world)
{
}

// @stub 0x137fe0
void function_137fe0(void)
{
}

// @stub 0x7a840
void function_07a840(void)
{
}

// @stub 0x7f660
bool __stdcall function_07f660(wchar_t *name, long length, const wchar_t *requested, long count, const wchar_t **names)
{
	return false;
}

// @stub 0x199740
bool __stdcall function_199740(unsigned char *buffer, long size, unsigned char *destination, long *decompressed_size)
{
	return false;
}

/* lane J's region: the next message of a connection's unreliable stream */
struct s_network_stream_header;
// @stub 0x95840
bool __stdcall function_095840(s_network_stream_header *stream, long *message_type, long *message_size, void *message)
{
	return false;
}
