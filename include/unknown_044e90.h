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

/* frees a list's indices unless they are its own storage (the capacity's top bit set) */
PRIVATE __forceinline void release_visibility_list(s_visibility_query_list *list)
{
    if (!(list->capacity & 0x80000000))
        g_480118->allocate((long)list->indices, (list->capacity & 0x7fffffff) * 4, 0x12);
}

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


/* the geometry data of a visible entry; shared by 0x458d0, 0x45a80 and 0x44e90 */
PRIVATE __forceinline byte *visible_entry_geometry(short index)
{
	s_44940_entry *entry = &g_4ba138[index];
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		return *(byte **)(sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
	}
	byte *structure = (byte *)g_4e0348;
	byte *data;
	if (!((byte)(entry->unknown00 >> 12) & 1))
	{
		byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
		data = *(byte **)(section + 0x50);
	}
	else
	{
		byte *instances = *(byte **)(structure + 0x144);
		short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
		data = (byte *)*(dword const *)(*(byte **)(structure + 0x13c) + definition_index * 0xc8 + 0x50);
	}
	return data;
}

void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);
void function_45a80(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);

#endif
