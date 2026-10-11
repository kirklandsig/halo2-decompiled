// stubs for the game functions that lane W's code calls and that are not
// decompiled yet

#include "unknown_11c920.h"

// @stub 0x1f2b0
void function_1f2b0() {}
#include "unknown_0259d0.h"

struct s_frame_view_2c560;
// @stub 0x132d0
void __stdcall function_132d0(s_frame_view_2c560 const *)
{
}

// @stub 0x13c20
void function_13c20(void)
{
}

// @stub 0x3d2c0
void function_3d2c0(void)
{
}

// @stub 0x2bcd0
void __stdcall function_2bcd0(long, long, bool, long, long, long, float)
{
}

// @stub 0x14b60
void __stdcall function_14b60(short, bool, bool, bool)
{
}

// @stub 0x44370
bool __stdcall function_44370(long)
{
    return false;
}

// @stub 0x174220
void __stdcall function_174220(bool)
{
}


struct s_2f970_view;
struct s_speed_result;
// @stub 0x2ba10
void function_2ba10(s_2f970_view const *, long, long, unsigned char const *, bool,
	long, bool, vector3f const *, long, long, long, unsigned char, long,
	unsigned char const *, bool, long, unsigned char const *, long, s_speed_result const *)
{
}

struct s_bit_vector_pool;
// @stub 0x1320f0
s_bit_vector_pool *function_1320f0(long, long, bool, long, unsigned char const *,
	unsigned char const *, float, float, float, long)
{
	return 0;
}


// @stub 0x1f490
void __stdcall function_1f490(long, long, long, long, long,
    float, float, float, float, float, float, float, long, long)
{
}

// @stub 0x41cc0
void __stdcall function_41cc0(void *payload)
{
}


struct s_visibility_plane_query;
struct s_visibility_query_list;
// @stub 0x2dd930
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first)
{
}


struct s_visibility_sphere_query;
// @stub 0x2ddbd0
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first)
{
}







// @stub 0x52d40
void function_52d40(long bitmap_index, unsigned char *shader, long count, bool mode)
{
}


// @stub 0x176cb0
void function_176cb0() {}

// @stub 0x391f0
bool __stdcall function_391f0(void *state) { return false; }

// @stub 0x12560
void function_12560() {  }

// @stub 0x32350
bool __stdcall function_32350(long index, real alpha) { return false; }

struct transform4x3f;
// @stub 0xb92d0
void function_b92d0(long index, transform4x3f const *old_parent, transform4x3f const *new_parent) {}

class __declspec(align(16)) c_z_capsule_temp
{
public:
    c_z_capsule_temp(void const *first, void const *second, real radius);
    ~c_z_capsule_temp();
    byte unknown00[0x30];
};

// @stub 0xdc370
c_z_capsule_temp::~c_z_capsule_temp() {}

