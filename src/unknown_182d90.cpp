// @flags /O2 /arch:SSE /Gr
#include "unknown_182d90.h"

struct s_extent_bounds
{
	__m128 lower;
	__m128 upper;
};

class c_extent_shape
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual long shape_kind() { return 0; }
	virtual void bounds(s_extent_transform const *transform, real tolerance, s_extent_bounds *result) {}
	virtual void slot7() {}
	virtual void slot8() {}
	virtual void slot9() {}
	virtual void slot10() {}
	virtual long first_key() { return NONE; }
	virtual long next_key(long key) { return NONE; }
	virtual c_extent_shape *child(long key, void *buffer) { return 0; }
	virtual void slot14() {}
	virtual long vertex_count() { return 0; }
};

struct s_extent_shape_data
{
	void *vtable;
	byte field_4[8];
	union { real radius; long count; c_extent_shape *nested; } field_c;
	__m128 vectors[4];
};

PRIVATE __forceinline real extent_length(__m128 vector)
{
	__m128 square = _mm_mul_ps(vector, vector);
	__m128 y = _mm_shuffle_ps(square, square, 0x55);
	__m128 z = _mm_shuffle_ps(square, square, 0xaa);
	__m128 sum = _mm_add_ss(z, _mm_add_ss(y, square));
	sum = _mm_sqrt_ss(sum);
	real result;
	_mm_store_ss(&result, sum);
	return result;
}

#define EXTENT_MIN(a, b) ((a) > (b) ? (b) : (a))
#define EXTENT_MAX(a, b) ((a) > (b) ? (a) : (b))

// @retail 0x182d90
real __stdcall function_182d90(c_extent_shape *shape, real radius,
	s_extent_transform const *transform, real *minimum, real *maximum)
{
	real result = radius;
	s_extent_shape_data *data = (s_extent_shape_data *)shape;
	s_extent_transform local;
	local.rows[0] = transform->rows[0];
	local.rows[1] = transform->rows[1];
	local.rows[2] = transform->rows[2];
	local.rows[3] = transform->rows[3];
	if (shape->shape_kind() == 21)
	{
		s_extent_transform combined;
		combined.compose(&local, (s_extent_transform *)data->vectors);
		result = function_182d90(data->field_c.nested, radius, &combined, minimum, maximum);
	}
	else if (shape->shape_kind() == 9)
	{
		for (long i = 0; i < data->field_c.count; i++)
		{
			real extent = data->vectors[i].m128_f32[3] + extent_length(data->vectors[i]);
			if (extent > result) result = extent;
			*minimum = EXTENT_MIN(*minimum, extent * 2.0f);
			*maximum = EXTENT_MAX(*maximum, extent * 2.0f);
		}
	}
	else
	{
		long type = shape->shape_kind();
		if (type == 2 || (type > 9 && type <= 11))
		{
			long key = shape->first_key();
			while (key != NONE)
			{
				__m128 buffer[16];
				c_extent_shape *child = shape->child(key, buffer);
				// The collection keeps its incoming radius; children update the bounds.
				function_182d90(child, result, &local, minimum, maximum);
				key = shape->next_key(key);
			}
		}
		else if (shape->shape_kind() == 19)
			result = function_182d90(data->field_c.nested, radius, &local, minimum, maximum);
		else if (shape->shape_kind() == 17)
			result = function_182d90(data->field_c.nested, radius, &local, minimum, maximum);
		else if (shape->shape_kind() == 20)
			result = function_182d90(data->field_c.nested, radius, &local, minimum, maximum);
		else if (shape->shape_kind() == 23)
			result = function_182d90(*(c_extent_shape **)((byte *)shape + 0x30), radius, &local, minimum, maximum);
		else
		{
			switch (shape->shape_kind())
			{
			case 4:
				result = data->field_c.radius + extent_length(local.rows[3]);
				*minimum = EXTENT_MIN(*minimum, data->field_c.radius);
				*maximum = EXTENT_MAX(*maximum, data->field_c.radius * 2.0f);
				break;
			case 5:
				{
					real extent = 0.0f;
					for (long i = 0; i < shape->vertex_count(); i++)
						{ real length = extent_length(data->vectors[i]); extent = EXTENT_MAX(extent, length); }
					result = extent_length(local.rows[3]) + extent;
					*minimum = EXTENT_MIN(*minimum, 0.0f);
					*maximum = EXTENT_MAX(*maximum, 0.0f);
				}
				break;
			case 6:
				{
					real extent = extent_length(data->vectors[0]);
					real offset = extent_length(local.rows[3]);
					real width = EXTENT_MIN(data->vectors[0].m128_f32[0], EXTENT_MIN(data->vectors[0].m128_f32[1], data->vectors[0].m128_f32[2])) * 2.0f;
					result = offset + extent;
					*minimum = EXTENT_MIN(*minimum, width);
					*maximum = EXTENT_MAX(*maximum, width);
				}
				break;
			case 7:
				{
					real extent = extent_length(_mm_sub_ps(data->vectors[0], data->vectors[1]));
					real offset = extent_length(local.rows[3]);
					result = extent * 0.5f + offset + data->field_c.radius;
					real width = data->field_c.radius * 2.0f;
					*minimum = EXTENT_MIN(*minimum, width);
					*maximum = EXTENT_MAX(*maximum, width);
				}
				break;
			case 8:
				{
					real width = EXTENT_MIN(data->vectors[0].m128_f32[0], EXTENT_MIN(data->vectors[0].m128_f32[1], data->vectors[0].m128_f32[2])) * 2.0f;
					real extent = extent_length(data->vectors[0]);
					real offset = extent_length(_mm_add_ps(data->vectors[1], local.rows[3]));
					result = offset + extent;
					*minimum = EXTENT_MIN(*minimum, width);
					*maximum = EXTENT_MAX(*maximum, width);
				}
				break;
			case 24:
				{
					s_extent_transform identity;
					identity.rows[0] = identity.rows[1] = identity.rows[2] = identity.rows[3] = _mm_setzero_ps();
					identity.rows[0].m128_f32[0] = identity.rows[1].m128_f32[1] = identity.rows[2].m128_f32[2] = 1.0f;
					s_extent_bounds bounds;
					shape->bounds(&identity, 0.0f, &bounds);
					bounds.lower = _mm_sub_ps(bounds.upper, bounds.lower);
					real width = EXTENT_MAX(bounds.lower.m128_f32[0], EXTENT_MAX(bounds.lower.m128_f32[1], bounds.lower.m128_f32[2]));
					*minimum = EXTENT_MIN(*minimum, width);
					*maximum = EXTENT_MAX(*maximum, width);
					result = extent_length(local.rows[3]) + width;
				}
				break;
			default: __assume(0);
			}
		}
	}
	return result;
}
