#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// @flags /O2 /arch:SSE /Gr

struct s_collision_result_1697c0;
bool function_1691a0(long arg_0, dword arg_1, dword arg_2, point3f const *arg_3,
	vector3f const *arg_4, s_collision_result_1697c0 *arg_5);
#include "instance_marks_storage.h"

struct s_244c10
{
	long field_00;
	long field_04;
};

class c_244c10_base
{
public:
	virtual void function_244c10(s_244c10 const *arg_0, void const *arg_1) = 0;
	virtual ~c_244c10_base() {}
	real field_04;
};

class c_244c10 : public c_244c10_base
{
public:
	virtual void function_244c10(s_244c10 const *arg_0, void const *arg_1);
	virtual ~c_244c10() {}
	dword field_08;
	dword field_0c;
	point3f const *field_10;
	vector3f const *field_14;
	bool field_18;
	s_collision_result_1697c0 *field_1c;
};

// @retail 0x244c10
void c_244c10::function_244c10(s_244c10 const *arg_0, void const *arg_1)
{
	long local_0 = arg_0->field_04;
	if (g_4e7c20[(short)local_0] != g_4e7c1c)
	{
		s_collision_result_1697c0 *local_1 = field_1c;
		g_4e7c20[(short)local_0] = g_4e7c1c;
		if (function_1691a0(local_0, field_08, field_0c, field_10, field_14, local_1))
		{
			if (field_08 & 0x10000000)
				field_04 = 0.f;
			else
				field_04 = field_04 > ((real *)field_1c)[1] ? ((real *)field_1c)[1] : field_04;
			field_18 = true;
		}
	}
}

#include "unknown_1efac0.h"
#include "unknown_1eb350.h"
#include "unknown_1cec30.h"

#include "unknown_244980.h"

struct s_244980_ray
{
    hkVector4 first;
    hkVector4 second;
    dword field_20;
    dword field_24;
};
struct s_244980_workspace
{
    hkVector4 minimum;
    hkVector4 maximum;
    hkTransform transform;
};
struct s_244980_output
{
    c_shape_library_base_a *shape;
    long index;
    s_244980_workspace *workspace;
    long field_0c;
    long field_10;
    long field_14;
    long field_18;
    long field_1c;
    long field_20;
};

// @retail 0x244980
bool function_244980(long cluster_index, point3f const *point, vector3f const *vector,
    dword flags, dword test_flags, s_collision_result_1697c0 *result)
{
    bool found = false;
    byte *cluster = *(byte **)((byte *)g_4e0348 + 0xa0) + cluster_index * 0xb0;
    if (*(long *)(cluster + 0xa8) > 0)
    {
        c_marker_object reference((dword)cluster, (dword)g_4e0348);
        c_shape_library_base_a query((c_havok_reference_counted *)&reference, *(long *)(cluster + 0xac));
        point3f end;
        s_244980_workspace workspace;
        s_244980_ray ray;
        s_244980_output output;
        ray.field_24 = 0;
        output.field_0c = 0;
        output.index = NONE;
        output.field_10 = 0;
        output.field_18 = 0;
        output.field_14 = 0;
        output.field_1c = 0;
        output.field_20 = 0;
        output.shape = &query;
        output.workspace = &workspace;
        c_244c10 callback;
        workspace.minimum.m_quad = _mm_setzero_ps();
        workspace.minimum.m_quad.m128_f32[3] = 1e30f;
        workspace.maximum.m_quad = _mm_setzero_ps();
        workspace.maximum.m_quad.m128_f32[3] = 1e30f;
        workspace.transform.m_rotation.m_col0.m_quad = _mm_setzero_ps();
        workspace.transform.m_rotation.m_col1.m_quad = _mm_setzero_ps();
        workspace.transform.m_rotation.m_col2.m_quad = _mm_setzero_ps();
        callback.field_08 = flags;
        callback.field_0c = test_flags;
        callback.field_10 = point;
        callback.field_14 = vector;
        callback.field_18 = false;
        callback.field_1c = result;
        callback.field_04 = ((real *)result)[1];
        end.x = point->x + vector->i;
        end.y = point->y + vector->j;
        end.z = point->z + vector->k;
        reference.flags = 0;
        query.allocation_size = 0;
        ray.first.m_quad.m128_f32[0] = point->x;
        ray.first.m_quad.m128_f32[1] = point->y;
        ray.first.m_quad.m128_f32[2] = point->z;
        ray.first.m_quad.m128_f32[3] = 0.0f;
        ray.second.m_quad.m128_f32[0] = end.x;
        ray.second.m_quad.m128_f32[1] = end.y;
        ray.second.m_quad.m128_f32[2] = end.z;
        ray.second.m_quad.m128_f32[3] = 0.0f;
        workspace.transform.m_rotation.m_col0.m_quad.m128_f32[0] = 1.0f;
        workspace.transform.m_rotation.m_col1.m_quad.m128_f32[1] = 1.0f;
        workspace.transform.m_rotation.m_col2.m_quad.m128_f32[2] = 1.0f;
        workspace.transform.m_translation.m_quad = _mm_setzero_ps();
        query.function_2df720(&ray, &output, &callback);
        if (callback.field_18)
            found = true;
    }
    return found;
}
