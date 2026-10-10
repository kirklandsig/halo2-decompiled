#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>
#include <float.h>
#include <new>
#include "unknown_1eb350.h"
#include "havok_reference.h"

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

void __cdecl function_2fe730(const void *points, long count, long stride, void *output);

// @flags /O2 /arch:SSE /Gr

/* a shape holding an array of 16-byte aligned vertices starting at +0x20 */
struct s_shape_surface
{
	word field_0;
	word edge;
	byte flags;
	byte field_5;
	short material;
};

struct s_shape_edge
{
	word vertices[2];
	word next[2];
	short surfaces[2];
};

struct s_shape_vertex
{
	point3f point;
	long field_c;
};

struct s_shape_source
{
	byte field_0[0x2c];
	s_shape_surface *surfaces;
	long edge_count;
	s_shape_edge *edges;
	long vertex_count;
	s_shape_vertex *vertices;
};

struct s_shape_ray
{
	point3f start;
	long field_c;
	point3f end;
};

struct s_shape_ray_result
{
	vector3f normal;
	real field_c;
	long field_10;
	real fraction;
};

struct c_vertex_reference
{
	word allocation_size;
	word references;
	c_vertex_reference() { references = 1; }
};

struct c_vertex_shape : c_vertex_reference
{
	long user;
	real radius;
	long surface_index;
	long vertex_count;
	short material;
	byte flags;
	byte kind;
	long value;
	real vertices_raw[8 * 4];
	c_vertex_shape(s_shape_source *source, byte kind, long value, long surface_index,
		long user, const transform4x3f *matrix);

	__m128 *vertices() { return (__m128 *)this->vertices_raw; }

	virtual void get_supporting_vertex(const __m128 *direction, __m128 *out);
	virtual void gather_vertices(const word *indices, long count, __m128 *out);
	virtual void get_first_vertex(__m128 *out);
	virtual void v3();
	virtual void get_bounds(const void *matrix, real expansion, void *output);
	virtual void ray_test(bool *hit, const s_shape_ray *ray, s_shape_ray_result *result);
};

PRIVATE inline void shape_transform_point(const transform4x3f *matrix, const point3f *point, point3f *result)
{
	real x = point->x, y = point->y, z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	result->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	result->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	result->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// @retail 0x1edd00
c_vertex_shape::c_vertex_shape(s_shape_source *source, byte type, long data, long index,
	long user_data, const transform4x3f *matrix)
{
	s_shape_source *const *source_reference = &source;
	const volatile byte *type_reference = &type;
	const volatile long *data_reference = &data;
	const long *index_reference = &index;
	const long *user_reference = &user_data;
	user = 0;
	vertex_count = 0;
	*(byte volatile *)&kind = *type_reference;
	value = *data_reference;
	radius = 0.01f;
	surface_index = *index_reference;
	s_shape_surface *surface = &(*source_reference)->surfaces[*index_reference];
	long edge_index = surface->edge;
	long first_edge = edge_index;
	do
	{
		s_shape_edge *edge = &(*source_reference)->edges[edge_index];
		bool side = edge->surfaces[1] == *index_reference;
		const point3f *point = &(*source_reference)->vertices[edge->vertices[side]].point;
		point3f transformed;
		if (matrix)
		{
			shape_transform_point(matrix, point, &transformed);
			point = &transformed;
		}
		real *vertex = &vertices_raw[vertex_count * 4];
		real x = point->x, y = point->y, z = point->z;
		vertex[0] = x;
		vertex[1] = y;
		vertex[2] = z;
		vertex[3] = 0.0f;
		*(long *)&vertices_raw[vertex_count * 4 + 3] = vertex_count | 0x3f000000;
		vertex_count++;
		edge_index = edge->next[side];
	} while (edge_index != first_edge);
	material = surface->material;
	flags = surface->flags;
	user = *user_reference;
}

short function_120850(const vector3f *vector);
bool function_23a220(const point2f *point, short count, const point2f *points, real epsilon);

// @retail 0x1edf50
void c_vertex_shape::ray_test(bool *hit, const s_shape_ray *ray, s_shape_ray_result *result)
{
	vector3f direction;
	vector3d_from_points3d(&ray->start, &ray->end, &direction);
	vector3f first, second;
	vector3d_from_points3d((point3f *)&vertices_raw[0], (point3f *)&vertices_raw[4], &first);
	vector3d_from_points3d((point3f *)&vertices_raw[0], (point3f *)&vertices_raw[8], &second);
	vector3f normal;
	normal.i = first.j * second.k - first.k * second.j;
	normal.j = first.k * second.i - first.i * second.k;
	normal.k = first.i * second.j - first.j * second.i;
	bool found = false;
	real length = (real)sqrt(length_sq3f(&normal));
	if (!(fabs(length) < 0.0001f))
	{
		real scale = 1.0f / length;
		normal.i *= scale;
		normal.j *= scale;
		normal.k *= scale;
	}
	real fourth = 0.0f;
	vector3f offset;
	vector3d_from_points3d((point3f *)&vertices_raw[0], &ray->start, &offset);
	real dot = dot3f(&normal, &direction);
	real fraction = 0.0f;
	if (dot != 0.0f)
		fraction = dot3f(&normal, &offset) * (-1.0f / dot);
	if (fraction < result->fraction)
	{
		real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
		if (bounded == fraction)
		{
			short axis = function_120850(&normal);
			long side = axis * 2 + (normal.n[axis] > 0.0f ? 1 : 0);
			point3f point;
			point.x = fraction * direction.i + ray->start.x;
			point.y = fraction * direction.j + ray->start.y;
			point.z = fraction * direction.k + ray->start.z;
			long first_axis = g_440b94[side][0];
			long second_axis = g_440b94[side][1];
			point2f projected;
			projected.x = point.n[first_axis];
			projected.y = point.n[second_axis];
			point2f polygon[8];
			long i = 0;
			for (; i + 3 < vertex_count; i += 4)
			{
				polygon[i].x = vertices_raw[i * 4 + first_axis];
				polygon[i].y = vertices_raw[i * 4 + second_axis];
				polygon[(i + 1)].x = vertices_raw[(i + 1) * 4 + first_axis];
				polygon[(i + 1)].y = vertices_raw[(i + 1) * 4 + second_axis];
				polygon[(i + 2)].x = vertices_raw[(i + 2) * 4 + first_axis];
				polygon[(i + 2)].y = vertices_raw[(i + 2) * 4 + second_axis];
				polygon[(i + 3)].x = vertices_raw[(i + 3) * 4 + first_axis];
				polygon[(i + 3)].y = vertices_raw[(i + 3) * 4 + second_axis];
			}
			for (; i < vertex_count; ++i)
			{
				polygon[i].x = vertices_raw[i * 4 + first_axis];
				polygon[i].y = vertices_raw[i * 4 + second_axis];
			}
			if (function_23a220(&projected, (short)vertex_count, polygon, 0.0f))
			{
				result->fraction = fraction;
				result->normal.i = normal.i;
				result->normal.j = normal.j;
				result->normal.k = normal.k;
				result->field_c = fourth;
				found = true;
			}
		}
	}
	*hit = found;
}

// @retail 0x1eded0
void c_vertex_shape::get_bounds(const void *matrix, real expansion, void *output)
{
	c_transformed_point points[8];
	for (long i = 0; i < vertex_count; i++)
		points[i].transform(matrix, &vertices()[i]);
	function_2fe730(points, vertex_count, sizeof(c_transformed_point), output);
}

// @retail 0x1edf40
void c_vertex_shape::v3()
{
	__asm int 3
	__assume(0);
}

// @retail 0x1ee340
void c_vertex_shape::get_first_vertex(__m128 *out)
{
	*out = vertices()[0];
}

// @retail 0x1ee350
void c_vertex_shape::get_supporting_vertex(const __m128 *direction, __m128 *out)
{
	long best = 0;
	real best_dot = -FLT_MAX;
	for (long i = 0; i < vertex_count; i++)
	{
		__m128 p = _mm_mul_ps(vertices()[i], *direction);
		__m128 sum = _mm_add_ss(_mm_shuffle_ps(p, p, 0x55), p);
		sum = _mm_add_ss(_mm_shuffle_ps(p, p, 0xaa), sum);
		real dot;
		_mm_store_ss(&dot, sum);
		if (dot > best_dot)
		{
			best_dot = dot;
			best = i;
		}
	}
	*out = vertices()[best];
}

// @retail 0x1ee3d0
void c_vertex_shape::gather_vertices(const word *indices, long count, __m128 *out)
{
	for (long i = count - 1; i >= 0; i--)
	{
		out[i] = vertices()[indices[i]];
	}
}

struct s_tag_block_owner
{
	byte unknown00[0x5c];
	s_tag_block_entry *entries;
};

struct s_tag_instance_ref
{
	byte unknown00[8];
	s_tag_block_owner *data;
	byte unknown0c[4];
};

struct s_lookup_source
{
	byte unknown00[0x18];
	short index;
	short pad1a;
	long tag_index;
};

short g_54e898;

// @retail 0x1ee410
void function_1ee410(const s_lookup_source *source, short *result)
{
    short value = g_54e898;
	if (source->tag_index != NONE)
	{
		s_tag_instance_ref *tags = (s_tag_instance_ref *)g_4e3b44;
		value = tags[(word)source->tag_index].data->entries[source->index].value10;
	}
	else if (source->index != NONE)
	{
		value = g_4e0348->entries[source->index].value08;
	}
    *result = value;
}

struct s_count_entry
{
	dword unknown0;
	dword flags;
};

struct s_count_owner
{
	byte unknown00[0x28];
	long count;
	s_count_entry *entries;
};


struct c_count_interface
{
	byte unknown04[8];
	s_count_owner *owner;
	byte field_10[0x30 - 0x10];
	long shape_value;

	virtual void v0() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void get_bounds(const __m128 *matrix, real expansion, __m128 *output);
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual long get_count();
	virtual long test_count();
	virtual long next_index(dword index);
	virtual c_vertex_shape *make_shape(long index, void *storage);
	__declspec(noinline) c_vertex_shape *make_transformed_shape(long index, long user_data, void *storage,
		const transform4x3f *matrix);
};

void __cdecl function_1ce210(const __m128 *matrix, const __m128 *extent,
	const __m128 *center, real expansion, __m128 *output);

// @retail 0x1ef040
void c_count_interface::get_bounds(const __m128 *matrix, real expansion, __m128 *output)
{
	function_1ce210(matrix, (const __m128 *)(field_10 + 0x10),
		(const __m128 *)field_10, expansion, output);
}

// @retail 0x1ee470
long c_count_interface::get_count()
{
	s_count_owner *o = owner;
	long count = o->count;
	if (count > 0)
	{
		const s_count_entry *last = &o->entries[count - 1];
		if (last->flags & 0x10)
		{
			count--;
		}
	}
	return count;
}

// @retail 0x1ee490
long c_count_interface::test_count()
{
	return get_count() > 0 ? 0 : -1;
}

// @retail 0x1ee4a0
long c_count_interface::next_index(dword index)
{
	if (index != NONE && index < (dword)(get_count() - 1))
		return index + 1;
	return NONE;
}

// @retail 0x1ee4d0
c_vertex_shape *c_count_interface::make_shape(long index, void *storage)
{
	return new (storage) c_vertex_shape((s_shape_source *)owner, 0, shape_value, index, 0, NULL);
}

// @retail 0x1ee500
c_vertex_shape *c_count_interface::make_transformed_shape(long index, long user_data,
	void *storage, const transform4x3f *matrix)
{
	return new (storage) c_vertex_shape((s_shape_source *)owner, 0, shape_value, index, user_data, matrix);
}

struct s_surface_key_array
{
 dword *keys;
 long count;
};

struct c_query_transform
{
 __m128 axes[3];
 __m128 position;
 void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

struct c_query_point
{
 __m128 value;
 void inverse_transform(const c_query_transform *matrix, const __m128 *point);
};

struct s_query_bounds
{
 __m128 minimum, maximum;
};

struct c_surface_query_library
{
 void query_sphere(const __m128 *sphere, s_surface_key_array *keys);
 void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys);
 void query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys);
};

#include "unknown_1c3b20.h"

c_shape_owner *g_51e9d4;

// @retail 0x1ee530 deleting c_shape_owner

// @retail 0x1ee560
c_shape_owner::~c_shape_owner()
{
	havok_reference_remove(object);
	object = 0;
	g_51e9d4 = 0;
}

#include "unknown_1efac0.h"

#include "unknown_1c3b70.h"

c_shape_global_owner *g_51e9d0;

struct s_bounds3d;
struct s_slot_entry_list;
extern s_bounds3d *g_4687e0;
extern s_slot_entry_list *g_4e0340;

struct s_vertex_bounds
{
	real x0, x1, y0, y1, z0, z1;
};

struct s_bounds_vertex
{
	point3f point;
	long field_c;
};

struct s_bounds_source
{
	byte field_0[0x38];
	long count;
	s_bounds_vertex *vertices;
};

PRIVATE __forceinline void bounds_half_extent_set(real maximum, real minimum, real volatile *destination)
{
    *destination = (maximum - minimum) * 0.5f;
}

// @retail 0x1eed40
c_shape_global_owner::c_shape_global_owner()
{
	field_8 = 0;
	g_51e9d0 = this;
	s_vertex_bounds bounds = *(s_vertex_bounds *)g_4687e0;
	s_bounds_source *source = (s_bounds_source *)g_4e0340;
	for (long i = 0; i < source->count; i++)
	{
		point3f *point = &source->vertices[i].point;
		if (point->x < bounds.x0) bounds.x0 = point->x;
		if (point->x > bounds.x1) bounds.x1 = point->x;
		if (point->y < bounds.y0) bounds.y0 = point->y;
		if (point->y > bounds.y1) bounds.y1 = point->y;
		if (point->z < bounds.z0) bounds.z0 = point->z;
		if (point->z > bounds.z1) bounds.z1 = point->z;
	}
	center.x = (bounds.x1 + bounds.x0) * 0.5f;
	center.y = (bounds.y1 + bounds.y0) * 0.5f;
	center.z = (bounds.z1 + bounds.z0) * 0.5f;
	center.w = 0.0f;
	bounds_half_extent_set(bounds.x1, bounds.x0, &extent.x);
	bounds_half_extent_set(bounds.y1, bounds.y0, &extent.y);
	bounds_half_extent_set(bounds.z1, bounds.z0, &extent.z);
	extent.w = 0.0f;
}

// @retail 0x1eefe0 deleting
c_shape_global_owner::~c_shape_global_owner()
{
	g_51e9d0 = 0;
}

void function_1eece0(s_surface_key_array *array, void *volatile owner);
bool function_1ef3e0(dword key);

struct c_instance_surface_query
{
 virtual void v0() {}
 virtual void v1() {}
 virtual void v2() {}
 virtual void v3() {}
 virtual void v4() {}
 virtual void v5() {}
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void query_sphere(const __m128 *sphere, s_surface_key_array *keys) {}
 virtual void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys) {}
};

struct s_query_instance
{
 transform4x3f matrix;
 short owner;
 byte field_36[0x58 - 0x36];
};

struct s_query_owner
{
 byte field_0[0x98];
 long surface_count;
 byte field_9c[0xb4 - 0x9c];
 byte *shape;
 byte field_b8[0xc8 - 0xb8];
};

struct s_query_globals
{
 byte field_0[0x13c];
 s_query_owner *owners;
 long count;
 s_query_instance *instances;
};

PRIVATE __forceinline void query_transform(const transform4x3f *matrix, c_query_transform *output)
{
 real *a = (real *)output;
 a[0] = matrix->forward.i; a[1] = matrix->forward.j; a[2] = matrix->forward.k; a[3] = 0.0f;
 a[4] = matrix->left.i; a[5] = matrix->left.j; a[6] = matrix->left.k; a[7] = 0.0f;
 a[8] = matrix->up.i; a[9] = matrix->up.j; a[10] = matrix->up.k; a[11] = 0.0f;
 __m128 translation;
 ((real *)&translation)[0] = matrix->position.x;
 ((real *)&translation)[1] = matrix->position.y;
 ((real *)&translation)[2] = matrix->position.z;
 ((real *)&translation)[3] = 0.0f;
 output->position = translation;
}

PRIVATE __forceinline void query_remove_key(s_surface_key_array *keys, long index)
{
 --keys->count;
 for (long i = index; i < keys->count; ++i)
  keys->keys[i] = keys->keys[i + 1];
}

PRIVATE __forceinline void query_remap_keys(s_surface_key_array *keys, long first, long instance_index)
{
 for (long i = first; i < keys->count; ++i)
 {
  dword key = ((keys->keys[i] | 0xffffa000) << 16) | instance_index;
  if (function_1ef3e0(key)) keys->keys[i] = key;
  else { query_remove_key(keys, i); --i; }
 }
}

// @retail 0x1ee820
void c_shape_owner::query_box(const c_query_transform *matrix, const __m128 *extent,
 real tolerance, s_surface_key_array *keys)
{
 void *volatile owner_home = this;
 ((c_surface_query_library *)this)->query_box(matrix, extent, tolerance, keys);
 function_1eece0(keys, this);
 for (long i = 0; i < keys->count; ++i)
 {
  dword key = keys->keys[i];
  long instance_index = key & 0xffff;
  if ((key & 0xe0000000) == 0x40000000)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[instance_index];
   s_query_owner *owner = &globals->owners[instance->owner];
   if (owner->surface_count <= 0x2000)
   {
    byte *volatile shape = owner->shape;
    c_query_transform transform, local;
    query_transform(&instance->matrix, &transform);
    local.inverse_product(&transform, matrix);
    query_remove_key(keys, i);
    long first = keys->count;
    --i;
    ((c_instance_surface_query *)(shape + 0x50))->query_box(&local, extent, tolerance, keys);
    if (first < keys->count)
    {
     void *volatile filtered_owner = (byte *)owner_home + 0x14;
     query_remap_keys(keys, first, instance_index);
    }
   }
  }
 }
}

// @retail 0x1ee5b0
void function_1ee5b0(c_shape_owner *owner, const __m128 *sphere, bool expand_instances,
 bool filter, s_surface_key_array *keys)
{
 c_shape_owner *const *owner_reference = &owner;
 ((c_surface_query_library *)*owner_reference)->query_sphere(sphere, keys);
 if (filter) function_1eece0(keys, *owner_reference);
 if (expand_instances)
 {
  for (long i = 0; i < keys->count; ++i)
  {
   dword key = keys->keys[i];
   long instance_index = key & 0xffff;
   if ((key & 0xe0000000) == 0x40000000)
   {
    s_query_globals *globals = (s_query_globals *)g_4e0348;
    s_query_instance *instance = &globals->instances[instance_index];
    s_query_owner *definition = &globals->owners[instance->owner];
    if (definition->surface_count <= 0x2000)
    {
     c_instance_surface_query *query = (c_instance_surface_query *)(definition->shape + 0x50);
     c_query_transform transform;
     c_query_point center;
     center.value = *sphere;
     query_transform(&instance->matrix, &transform);
     center.inverse_transform(&transform, &center.value);
     query_remove_key(keys, i);
     __m128 local = center.value;
     ((real *)&local)[3] = ((const real *)sphere)[3];
     long first = keys->count;
     --i;
     query->query_sphere(&local, keys);
     query_remap_keys(keys, first, instance_index);
    }
   }
  }
 }
}

// @retail 0x1eea50
void c_shape_owner::query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys)
{
 ((c_surface_query_library *)this)->query_bounds(bounds, keys);
 function_1eece0(keys, this);
 for (long i = 0; i < keys->count; ++i)
 {
  dword key = keys->keys[i];
  long instance_index = key & 0xffff;
  if ((key & 0xe0000000) == 0x40000000)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[instance_index];
   s_query_owner *owner = &globals->owners[instance->owner];
   if (owner->surface_count <= 0x2000)
   {
    c_instance_surface_query *query = (c_instance_surface_query *)(owner->shape + 0x50);
    c_query_transform transform, world, local;
    query_transform(&instance->matrix, &transform);
    __m128 extent = _mm_mul_ps(_mm_sub_ps(bounds->maximum, bounds->minimum), _mm_set1_ps(0.5f));
    world.axes[0] = _mm_setzero_ps();
    world.axes[1] = _mm_setzero_ps();
    world.axes[2] = _mm_setzero_ps();
    ((real *)&world.axes[0])[0] = 1.0f;
    ((real *)&world.axes[1])[1] = 1.0f;
    ((real *)&world.axes[2])[2] = 1.0f;
    world.position = _mm_add_ps(bounds->minimum, extent);
    local.inverse_product(&transform, &world);
    query_remove_key(keys, i);
    long first = keys->count;
    --i;
    query = (c_instance_surface_query *)(owner->shape + 0x50);
    query->query_box(&local, &extent, 0.0001f, keys);
    query_remap_keys(keys, first, instance_index);
   }
  }
 }
}


// @retail 0x1ee800
void c_shape_owner::query_sphere(const __m128 *sphere, s_surface_key_array *keys)
{
 function_1ee5b0(this, sphere, true, true, keys);
}

struct s_type_1a7926
{
 byte unknown00[8];
 transform4x3f root_matrix;
 byte unknown3c[0x48 - 0x3c];
 short *node_indices;
 byte unknown4c[4];
 transform4x3f *field_50;
};

bool function_20a9a0(s_type_1a7926 *matrices, long object_index);
extern long *g_51e9cc;

struct c_child_transform : c_havok_reference_counted
{
 dword user;
 long field_c;
 c_query_transform transform;
 c_child_transform(c_havok_reference_counted *child);
};

// @retail 0x1ef070
c_child_transform *function_1ef070(dword key, void *storage)
{
 (void)&key;
 (void)&storage;
 long object_index = g_51e9cc[key & 0xffff];
 byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
 byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
 byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
 long position = (key >> 16) & 0x1fff;
 byte *region = *(byte **)(model + 0x74) + (position & 31) * 16;
 byte *choice = *(byte **)(region + 0xc) + ((position >> 5) & 255) * 8;
 if ((key & 0xe0000000) == 0x80000000)
 {
  byte *collision = g_4e3b44[*(long *)(model + 0xc) & 0xffff].bytes;
  byte *group = *(byte **)(collision + 0x20) + *(signed char *)(region + 4) * 12;
  byte *entry = *(byte **)(group + 8) + *(signed char *)(choice + 5) * 20;
  c_havok_reference_counted *child = (c_havok_reference_counted *)(*(byte **)(entry + 0x10) + 0x40);
  c_child_transform *result = new (storage) c_child_transform(child);
  havok_reference_remove(child);
  object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
  const transform4x3f *matrix = (const transform4x3f *)(object + *(short *)(object + 0x116));
  c_query_transform transform;
  query_transform(matrix, &transform);
  result->transform = transform;
  result->user = key;
  return result;
 }
 else
 {
  s_type_1a7926 info;
  function_20a9a0(&info, object_index);
  byte *physics = (byte *)info.node_indices;
  byte *group = *(byte **)(physics + 0xc4) + *(signed char *)(region + 5) * 12;
  byte *entry = *(byte **)(group + 8) + *(signed char *)(choice + 6) * 12;
  long index = **(short **)(entry + 8);
  byte *body = *(byte **)(physics + 0x3c) + index * 0x90;
  c_havok_reference_counted *child = *(c_havok_reference_counted **)(body + 0x38);
  c_child_transform *result = new (storage) c_child_transform(child);
  havok_reference_remove(*(c_havok_reference_counted **)(body + 0x38));
  long node = *(short *)body;
  const transform4x3f *matrix = node == NONE ? &info.root_matrix : &info.field_50[node];
  c_query_transform transform;
  query_transform(matrix, &transform);
  result->transform = transform;
  result->user = key;
  return result;
 }
}

struct s_havok_transform;
void function_181f80(s_havok_transform *transform, const transform4x3f *matrix);
c_a *surface_empty_shape(void *storage);

// @retail 0x1ef950
void *c_shape_global_owner::make_shape(dword key, void *storage)
{
 // The retail virtual preserves its owner in a stack slot before dispatch.
 c_shape_global_owner *volatile owner = this;
 if (function_1ef3e0(key))
 {
  long index = key & 0xffff;
  long surface = (key >> 16) & 0x1fff;
  long kind = key >> 29;
  if (kind == 1)
   return new (storage) c_vertex_shape((s_shape_source *)g_4e0340, kind, NONE, index, key, NULL);
  else if (kind == 2)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[index];
   c_havok_reference_counted *child = (c_havok_reference_counted *)(globals->owners[instance->owner].shape + 0x40);
   c_child_transform *result = new (storage) c_child_transform(child);
   havok_reference_remove(child);
   function_181f80((s_havok_transform *)&result->transform, &instance->matrix);
   return result;
  }
  else if (kind == 5)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[index];
   c_count_interface *owner = (c_count_interface *)globals->owners[instance->owner].shape;
   return owner->make_transformed_shape(surface, key, storage, &instance->matrix);
  }
  else return function_1ef070(key, storage);
 }
 return surface_empty_shape(storage);
}
