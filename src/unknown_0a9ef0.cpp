#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /arch:SSE /Gr

bool function_aa970(long index);
transform4x3f *function_ba160(long index, transform4x3f *matrix);
void function_ba1d0(long index, vector3f *linear, vector3f *angular);
void function_aaca0(long index, point3f const *position, vector3f const *forward, vector3f const *up,
	vector3f const *linear, vector3f const *angular, vector3f *linear_result, vector3f *angular_result);
void function_aa9e0(long index, point3f const *previous_position, vector3f const *velocity);
void __stdcall function_b77d0(long index, vector3f const *linear, vector3f const *angular);
void function_aab40(long index, point3f const *position, vector3f const *forward,
	vector3f const *up, vector3f const *linear, vector3f const *angular);

// Keep this draft disabled: its matrix-query caller changes 0xba160's
// nested 0x1420f0 convention and loses the existing 0xba160 match.
#if 0
point3f *function_b9dd0(long index, point3f *result);
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
	void function_1c2390();
	c_part *function_1c2460(long index);
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

class __declspec(align(16)) c_z_capsule_temp
{
public:
	c_z_capsule_temp(void const *first, void const *second, real radius);
	~c_z_capsule_temp();
	byte unknown00[0x30];
};

extern real g_47f05c;

long __stdcall function_a9f70(long object_index, long mode, point3f const *position)
{
	byte *header = g_4e0300->data + (object_index & 0xffff) * 12;
	byte *object = *(byte **)(header + 8);
	point3f current;
	bool far = false;
	bool near = false;
	function_b9dd0(object_index, &current);
	bool type_one = (bool)(((dword)(1 << object[0xaa]) >> 1) & 1);
	if (position)
	{
		real near_limit = 0.2f;
		if (function_aa970(object_index))
		{
			byte *active_object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			far = (bool)(((dword)*(dword *)(active_object + 0x134) >> 27) & 1);
			near_limit = far ? 1.5f : 0.8f;
		}
		real far_limit;
		if (mode == 1) far_limit = type_one ? 8.0f : 3.0f;
		else far_limit = type_one ? 5.0f : 2.0f;
		vector3f difference;
		vector3d_from_points3d(&current, position, &difference);
		far = difference.k * difference.k + difference.j * difference.j + difference.i * difference.i > far_limit * far_limit;
		near = difference.k * difference.k + difference.j * difference.j + difference.i * difference.i > near_limit * near_limit;
	}
	long selection = 0;
	if (*(long *)(object + 0x14) == NONE)
	{
		switch (mode)
		{
		case 0:
			goto choose;
		case 1:
			selection = far ? 1 : 0;
			break;
		case 2:
			if (!far) goto choose;
			break;
		default:
			__assume(0);
		}
	}
	goto collision;
choose:
	if (near) selection = 1;
	else
	{
		header = g_4e0300->data + (object_index & 0xffff) * 12;
		selection = ((1 << header[3]) & 0x803) ? 2 : 1;
	}
collision:
	if (g_4e6948->mode != 4 && position)
	{
		header = g_4e0300->data + (object_index & 0xffff) * 12;
		if (header[3] == 0 && selection == 1)
		{
			object = *(byte **)(header + 8);
			byte *definition = *(byte **)((byte *)g_4e3b44 + (*(long *)object & 0xffff) * 16 + 8);
			c_z_shape_view *shape = (c_z_shape_view *)((s_part_table *)(definition + 0x264))->function_1c2460(0);
			if (shape->v5() == 7)
			{
				real radius = *(real *)((byte *)shape + 0xc) - (g_47f05c + 0.001f);
				if (radius < g_47f05c) radius = g_47f05c;
				bool hit;
				{
					c_z_capsule_temp capsule((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
					transform4x3f matrix_storage;
					transform4x3f *matrix = function_ba160(object_index, &matrix_storage);
					matrix->position = *position;
					hit = function_1c5210(matrix, &capsule, *(long *)(object + 0xb4), 9);
				}
				if (hit) selection = 2;
			}
		}
	}
	return selection;
}
#endif

// The selection result must be supplied by the caller in addition to mode.
// Keep this candidate disabled until the caller's interface supplies it.
#if 0
void function_aa260(long selection, vector3f const *linear, long object_index, long mode,
	point3f const *position, vector3f const *forward, vector3f const *up, vector3f const *angular)
{
	switch (selection)
	{
	case 1:
		function_aab40(object_index, position, forward, up, linear, angular);
		break;
	case 2:
		{
			bool change_linear = linear != NULL;
			bool change_angular = angular != NULL;
			if (!function_aa970(object_index) && (change_linear || change_angular))
			{
				transform4x3f current;
				function_ba160(object_index, &current);
				point3f local_010335 = position ? *position : current.position;
				vector3f local_de61cc = forward ? *forward : current.forward;
				vector3f local_523673 = up ? *up : current.up;
				vector3f desired_linear, desired_angular;
				function_ba1d0(object_index, &desired_linear, &desired_angular);
				if (change_linear)
				{
					vector3f difference;
					difference.i = linear->i - desired_linear.i;
					difference.j = linear->j - desired_linear.j;
					difference.k = linear->k - desired_linear.k;
					if (mode == 2 && difference.k * difference.k + difference.i * difference.i +
						difference.j * difference.j <= 0.06f)
						change_linear = false;
					else desired_linear = *linear;
				}
				if (change_angular)
				{
					vector3f difference;
					difference.i = angular->i - desired_angular.i;
					difference.j = angular->j - desired_angular.j;
					difference.k = angular->k - desired_angular.k;
					if (mode == 2 && difference.k * difference.k + difference.j * difference.j +
						difference.i * difference.i <= 0.01f)
						change_angular = false;
					else desired_angular = *angular;
				}
				vector3f adjusted_linear, adjusted_angular;
				function_aaca0(object_index, &local_010335, &local_de61cc, &local_523673,
					&desired_linear, &desired_angular, &adjusted_linear, &adjusted_angular);
				if (change_linear)
					function_aa9e0(object_index, NULL, &adjusted_linear);
				if (change_linear || change_angular)
					function_b77d0(object_index, change_linear ? &adjusted_linear : NULL,
						change_angular ? &adjusted_angular : NULL);
			}
		}
		break;
	}
}
#endif


bool function_a76b0(long object_index, long flag);
struct s_location;
void function_a96d0(long object_index, vector3f const *position);
void function_a9770(long object_index, vector3f const *up, vector3f const *forward);
void function_a9640(long object_index);
void function_b75a0(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, s_location const *location, bool unknown);

// @retail 0xaab40
void function_aab40(long object_index, point3f const *position, vector3f const *forward,
    vector3f const *up, vector3f const *linear, vector3f const *angular)
{
    bool update = true;
    vector3f const *const *angular_reference = &angular;
    s_record_pool *objects = g_4e0300;
    byte *header = objects->data + (object_index & 0xffff) * 12;
    byte *object = *(byte **)(header + 8);
    if (position)
    {
        vector3f delta;
        vector3d_from_points3d((point3f *)(object + 0x64), position, &delta);
        bool type_zero = header[3] == 0;
        bool moved = delta.k * delta.k + delta.i * delta.i + delta.j * delta.j > 1.0f;
        bool parent = *(long *)(object + 0x14) != NONE;
        bool flag = function_a76b0(object_index, false);
        update = !moved && !parent && !flag && type_zero;
    }
    function_aa9e0(object_index, position, linear);
    if (update)
    {
        if (position)
        {
            byte *current = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            function_a96d0(object_index, (vector3f *)(current + 0x64));
        }
        if (forward && up)
        {
            byte *current = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            function_a9770(object_index, (vector3f *)(current + 0x70), (vector3f *)(current + 0x7c));
        }
    }
    else function_a9640(object_index);
    function_b75a0(object_index, position, forward, up, NULL, false);
    function_b77d0(object_index, linear, *angular_reference);
}
