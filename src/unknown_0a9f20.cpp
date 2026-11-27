// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A9F20.CPP: the destructor of a reference object (vtable 0x452c1c)
   whose enable/disable virtual is the library's c_a::v2 (0x2d7240), over a
   base (vtable 0x452c30) that frees itself to the allocator at 0x480118 with
   the size it keeps in its first word */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"
#include "unknown_0a9f20.h"

// @retail 0xa9f20 deleting c_reference_a9f20

// @retail 0xdc370
c_reference_a9f20::~c_reference_a9f20()
{
    if (flags & 0x8000)
    {
        flags &= 0x7fff;
        /* the same object as the library's c_a, whose virtual retail calls directly */
        ((c_a *)(void *)this)->c_a::v2(false);
    }
}

#include "unknown_0a58d0.h"

bool function_aa970(long index);
point3f *function_b9dd0(long object_index, point3f *result);
inline transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
bool function_1c5210(transform4x3f const *matrix, void *shape, long excluded_component, long filter);

class c_part;
struct s_pair_element;
struct s_single_element;
struct s_part_table
{
	byte unknown00[0x20];
	long pair_a_count;
	s_pair_element *pairs_a;
	long single_count;
	s_single_element *singles;
	long pair_b_count;
	s_pair_element *pairs_b;
	void construct_shapes();
	c_part *get_shape(long index);
};

class c_z_shape_view
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual long v5() = 0;
};

extern real g_47f05c;

// Roasted-Codes (draft #1589, released 2026-11-27; body unchanged, 172 at 743 of 738 B, first difference +60): a
// short-circuit `!near && header-type-mask` test that picks 2 only when both allow it gave 164 (738 B, first difference
// still +60), and explicit `real` casts on the products give retail's rounding at the same score, but this body still
// shares one distance sum where retail computes two. Testing the hit inside the capsule's scope, before its cleanup,
// gave 205 (693 B): it drops the selection spill and merges the returns, but the result test then follows the
// destruction, against retail's order. Retail saves the hit to a stack byte, passes the capsule in ECX and calls the
// matched base destructor 0xdc370; this image calls a generated cleanup with the object in ESI and the hit in BL.
// Moving the inlining boundary of that cleanup is untested.
// @retail 0xa9f70
long __stdcall function_a9f70(long object_index, long mode, point3f const *position)
{
	s_object_view *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	bool far = false;
	bool near = false;
	point3f current;
	function_b9dd0(object_index, &current);
	bool type_one = (bool)(((dword)(1 << object->type) >> 1) & 1);
	if (position)
	{
		real near_distance = 0.2f;
		if (function_aa970(object_index))
		{
			s_object_view *active = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			far = (bool)((*(dword *)((byte *)active + 0x134) >> 27) & 1);
			near_distance = far ? 1.5f : 0.8f;
		}
		real far_distance;
		if (mode == 1)
			far_distance = type_one ? 8.0f : 3.0f;
		else
			far_distance = type_one ? 5.0f : 2.0f;
		vector3f delta;
		vector3d_from_points3d(&current, position, &delta);
		far = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > far_distance * far_distance;
		near = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > near_distance * near_distance;
	}
	long selection = 0;
	if (object->field14 == NONE)
	{
		if (mode == 1)
			selection = far ? 1 : 0;
		else if (mode == 0 || !far)
		{
			if (near)
				selection = 1;
			else
			{
				s_object_header *header = &((s_object_header *)g_4e0300->data)[object_index & 0xffff];
				selection = ((1 << header->unknown03[0]) & 0x803) ? 2 : 1;
			}
		}
	}
	if (g_4e6948->mode != 4 && position)
	{
		s_object_header *header = &((s_object_header *)g_4e0300->data)[object_index & 0xffff];
		if (header->unknown03[0] == 0 && selection == 1)
		{
			object = header->object;
			byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
			c_z_shape_view *shape = (c_z_shape_view *)((s_part_table *)(definition + 0x264))->get_shape(0);
			if (shape->v5() == 7)
			{
				real radius = *(real *)((byte *)shape + 0xc) - (g_47f05c + 0.001f);
				if (g_47f05c > radius)
					radius = g_47f05c;
				bool hit;
				{
					c_z_capsule_temp capsule((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
					transform4x3f matrix;
					function_ba160(object_index, &matrix);
					matrix.position = *position;
					hit = function_1c5210(&matrix, &capsule, *(long *)((byte *)object + 0xb4), 9);
				}
				if (hit)
					selection = 2;
			}
		}
	}
	return selection;
}
