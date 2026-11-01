#ifndef UNKNOWN_044E90_H
#define UNKNOWN_044E90_H

#include "material_records_storage.h"
#include "globals.h"

struct s_render_object_header
{
	byte unknown00[8];
	long *object;
};

struct s_geometry_visibility_list
{
	dword *indices;
	long count;
};

struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index, point3f const *center, real radius, bool *contained);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
struct s_visibility_plane_query
{
    long count;
    plane3f *planes;
};
struct s_visibility_query_list
{
    dword *indices;
    long count;
    long capacity;
};
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);
template<long N> struct __declspec(align(16)) s_visibility_plane_storage
{
    s_visibility_query_list list;
    dword indices[N];
};
struct s_visibility_query_workspace
{
    s_visibility_plane_storage<1024> first, second;
    s_visibility_plane_storage<2048> third;
    union
    {
        s_visibility_plane_storage<2048> fourth;
        transform4x3f transforms[255];
    };
};


void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);
void function_45a80(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);

#endif
