// @flags /O2 /Ob1 /arch:SSE /Gr
#include "view_context_storage.h"
/* UNKNOWN_0D0690.CPP: object child iteration, object state helpers and the
   render model triangle interpolation (positions, texture coordinates,
   normals and colors at barycentric coordinates) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "globals.h"
#include "unknown_0d0690.h"
#include "geometry_cache.h"

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);
dword __cdecl pack_color3f(color3f const *color);

struct s_sky_surface_record
{
    vector3f direction;
    dword color0;
    dword color1;
};

PRIVATE __forceinline long sky_surface_tag(short index)
{
    byte *scenario = (byte *)g_4e0350;
    long result = NONE;
    if (index >= 0)
    {
        long count = *(long *)(scenario + 8);
        if (index < count)
            result = *(long *)(*(byte **)(scenario + 0xc) + index * 8 + 4);
    }
    return result;
}

real function_30bf0(vector3f *vector);

// @retail 0xd1a30
long __stdcall function_d1a30(long ignore_object_index, point3f const *point, s_sky_surface_record *record)
{
    byte collision[0x5c];
    *(short *)(collision + 0x24) = NONE;
    long result = 2;
    if (g_4b9ee9 && g_4b9eec != NONE)
    {
        long index = g_4b9eec;
        long sky_index = sky_surface_tag((short)index);
        if (sky_index != NONE)
        {
            byte *sky = g_4e3b44[sky_index & 0xffff].bytes;
            if (sky != NULL)
            {
                vector3f direction = *g_4687a4;
                color3f color = *(color3f *)g_468718;
                real strength = 0.0f;
                long volatile count = *(long *)(sky + 0x78);
                if (count > 0)
                {
                    byte *lights = *(byte **)(sky + 0x7c);
                    for (long i = 0; i < count; ++i)
                    {
                        byte *light = lights + i * 0x34;
                        if (*(long *)(light + 0x2c) > 0)
                        {
                            byte *entry = *(byte **)(light + 0x30);
                            real yaw = *(real *)(light + 0xc);
                            real pitch = *(real *)(light + 0x10);
                            real cosine = (real)cos(pitch);
                            direction.i += (real)cos(yaw) * cosine;
                            direction.j += (real)sin(yaw) * cosine;
                            direction.k += (real)sin(pitch);
                            if (i == 0)
                            {
                                color = *(color3f *)(entry + 4);
                                strength = *(real *)(entry + 0x10);
                            }
                            else if (*(real *)(entry + 0x10) > 0.0f)
                            {
                                real blend = 1.0f - strength / *(real *)(entry + 0x10) / ((real)i + 2.0f);
                                if (blend < 0.0f) blend = 0.0f;
                                else if (blend > 1.0f) blend = 1.0f;
                                function_131c20(&color, (color3f *)(entry + 4), 0, blend, &color);
                                strength += *(real *)(entry + 0x10);
                            }
                        }
                    }
                    function_30bf0(&direction);
                    vector3f ray;
                    ray.i = direction.i * 100.0f;
                    ray.j = direction.j * 100.0f;
                    ray.k = direction.k * 100.0f;
                    bool hit = function_1697c0(0x10000001, point, &ray, ignore_object_index, NONE,
                        (s_collision_result_1697c0 *)collision);
                    if (!hit || *(short *)(collision + 0x5a) != NONE)
                        color = *(color3f *)(sky + 0x34);
                }
                else
                {
                    color = *(color3f *)(sky + 0x34);
                    direction.i = direction.j = direction.k = -1.0f;
                    function_30bf0(&direction);
                }
                record->direction = direction;
                record->color0 = 0xff555555;
                if (color.red < 0.0f) color.red = 0.0f;
                else if (color.red > 1.0f) color.red = 1.0f;
                if (color.green < 0.0f) color.green = 0.0f;
                else if (color.green > 1.0f) color.green = 1.0f;
                if (color.blue < 0.0f) color.blue = 0.0f;
                else if (color.blue > 1.0f) color.blue = 1.0f;
                record->color1 = pack_color3f(&color);
                result = 0;
            }
        }
    }
    return result;
}

color3f *unpack_color3f(dword pixel, color3f *color);

struct s_object_tag
{
	byte unknown00[0x1d4];
	real value1d4;
	byte unknown1d8[4];
	real value1dc;
	byte unknown1e0[0x2c0 - 0x1e0];
	long count;
	struct s_object_range *ranges;
};

struct s_object_range
{
	byte unknown00[0xa];
	short maximum_a;
	short maximum_b;
};

struct s_object
{
	long tag_index;
	dword unknown04 : 26;
	dword flag26 : 1;
	dword unknown04b : 5;
	byte unknown08[4];
	long next_sibling;
	long first_child;
	long value14;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x15c - 0xab];
	vector3f vector15c;
	vector3f vector168;
	byte unknown174[0x180 - 0x174];
	union
	{
		vector3f vector180;
		struct
		{
			real unknown180;
			real value184;
			real unknown188;
		};
	};
	byte unknown18c[0x1fa - 0x18c];
	byte counter1fa;
	byte counter1fb;
	short value1fc;
	byte unknown1fe[0x212 - 0x1fe];
	char slot212;
	byte unknown213[5];
	long children[1];
	byte unknown21c[0x22a - 0x21c];
	short value22a;
	short value22c;
	byte unknown22e[0x2a0 - 0x22e];
	long value2a0;
	short value2a4;
	byte unknown2a6[2];
	long value2a8;
	long value2ac;
	byte unknown2b0[0x334 - 0x2b0];
	real value334;
	real value338;
	byte unknown33c[2];
	short offset33e;
};

struct s_object_entry
{
	byte unknown00[8];
	long value;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

#define OBJECT_FROM_INDEX(index) (((s_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define TAG_FROM_OBJECT(object) ((s_object_tag *)g_4e3b44[(object)->tag_index & 0xffff].bytes)


// @retail 0xd0690
bool function_d0690(s_object_child_iterator *iterator)
{
	bool result = false;

	if (iterator->root == iterator->current)
	{
		while (iterator->next != NONE)
		{
			long index = iterator->next;
			s_object *child = OBJECT_FROM_INDEX(index);
			iterator->next = child->next_sibling;
			if ((1 << child->type) & 3)
			{
				iterator->child_index = index;
				iterator->child_short = child->value1fc;
				iterator->child_value = child->value14;
				result = true;
				break;
			}
		}
		if (!result)
		{
			long index = OBJECT_FROM_INDEX(iterator->root)->first_child;
			while (index != NONE)
			{
				s_object *child = OBJECT_FROM_INDEX(index);
				if ((1 << child->type) & 3)
				{
					if (TEST_FIELD_BIT(child->flag26))
						break;
				}
				index = child->next_sibling;
			}
			iterator->current = index;
			if (index != NONE)
				iterator->next = OBJECT_FROM_INDEX(index)->first_child;
		}
	}

	if (iterator->root != iterator->current && iterator->current != NONE)
	{
		do
		{
			while (iterator->next != NONE)
			{
				long index = iterator->next;
				s_object *child = OBJECT_FROM_INDEX(index);
				iterator->next = child->next_sibling;
				if ((1 << child->type) & 3)
				{
					iterator->child_index = index;
					iterator->child_short = child->value1fc;
					iterator->child_value = child->value14;
					return true;
				}
			}
			if (result)
				return result;
			long index = OBJECT_FROM_INDEX(iterator->current)->next_sibling;
			while (index != NONE)
			{
				s_object *child = OBJECT_FROM_INDEX(index);
				if ((1 << child->type) & 3)
				{
					if (TEST_FIELD_BIT(child->flag26))
						break;
				}
				index = child->next_sibling;
			}
			iterator->current = index;
			if (index != NONE)
				iterator->next = OBJECT_FROM_INDEX(index)->first_child;
		} while (iterator->current != NONE);
	}

	return result;
}

// @retail 0xd0930
void function_d0930(long object_index, vector3f *vector)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	object->vector15c = *vector;
	object->vector180 = *vector;
}

// @retail 0xd0980
void __stdcall function_d0980(long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	object->value2a0 = NONE;
	object->value2a4 = NONE;
	object->value2a8 = NONE;
	object->value2ac = NONE;
}

// @retail 0xd0b70
void function_d0b70(long object_index, long a, long b, real value, long slot)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	long child_index = object->children[slot];

	if (child_index != NONE)
	{
		s_object *child = OBJECT_FROM_INDEX(child_index);
		s_object_tag *tag = TAG_FROM_OBJECT(child);

		child->value184 = value;
		if (tag->count > 0)
		{
			s_object_range *range = tag->ranges;

			if (a < 0)
				a = 0;
			else if (a > range->maximum_a)
				a = range->maximum_a;
			child->value22c = (short)a;
			if (b < 0)
				b = 0;
			else if (b > range->maximum_b)
				b = range->maximum_b;
			child->value22a = (short)b;
		}
	}
}

// @retail 0xd0dc0
void function_d0dc0(long object_index, long value)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	s_object_entry *entry = (s_object_entry *)((byte *)object + object->offset33e + 0x30);

	if (entry->value == value)
		entry->value = NONE;
}

// @retail 0xd1210
real function_d1210(long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	if (object->value334 > 0.0f)
	{
		s_object_tag *tag = TAG_FROM_OBJECT(object);

		if (object->value338 == 0.0f)
		{
			if (tag->value1d4 > object->value334)
				return object->value334 / tag->value1d4;
			return 1.0f;
		}
		if (object->value334 > object->value338)
			return (object->value334 - object->value338) / tag->value1dc;
	}
	return 0.0f;
}

// @retail 0xd1490
real function_d1490(long object_index, long child_index)
{
	real result = 1.0f;

	if (object_index != NONE && child_index != NONE)
	{
		s_object *object = OBJECT_FROM_INDEX(object_index);
		short slot = object->slot212;
		long current = NONE;
		byte counter;
		real value;

		if (slot != NONE)
			current = object->children[slot];
		if (current == child_index)
		{
			counter = object->counter1fb;
			if (counter == 0)
				return 0.0f;
			value = (real)counter * g_510c54->rate;
		}
		else
		{
			counter = object->counter1fa;
			if (counter == 0)
				return 0.0f;
			value = (real)counter * g_510c54->rate;
		}
		value *= 10.0f / 3.0f;
		if (0.0f > value)
			result = 0.0f;
		else if (value > 1.0f)
			result = 1.0f;
		else
			result = value;
	}
	return result;
}

/* the callbacks of the unit object type's table at 0x4679c0 that are
   decompiled (entry 17 is function_d0980) */
void *g_4679c0[29] =
{
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, (void *)function_d0980, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0
};

struct s_vertex_block
{
	byte type;
	byte unknown01[3];
	long offset4;
	long offset;
	byte *data;
	byte unknown10[0x10];
};

struct s_triangle
{
	word a, b, c;
};

struct s_mesh
{
	byte unknown00[0x24];
	s_triangle *triangles;
	byte unknown28[0x38 - 0x28];
	long block_count;
	s_vertex_block *blocks;
};

struct s_rgb24
{
	byte r, g, b;
};

struct s_surface_colour_bucket
{
	word unknown00;
	word resource_index;
	byte unknown04[4];
	word *first_vertices;
};

struct s_surface_colour_resource
{
	byte flags;
	byte unknown01[0xc - 1];
	s_geometry_block_info block;
	byte unknown30[4];
	struct s_surface_colour_runtime *runtime;
};

struct s_surface_colour_runtime
{
	long count;
	struct s_surface_colour_data
	{
		byte unknown00[0x20];
		s_vertex_block block;
	} *data;
};

struct s_surface_colour_group
{
	byte unknown00[0x44];
	s_surface_colour_resource *resources;
	byte unknown48[0x54 - 0x48];
	s_surface_colour_bucket *buckets_by_instance;
	byte unknown58[0x64 - 0x58];
	s_surface_colour_bucket *other_buckets;
};

// @retail 0xd35a0
s_vertex_block *function_d35a0(long index, s_surface_colour_group *group, bool instance, long vertex_index)
{
	s_vertex_block *result = NULL;
	s_surface_colour_bucket *bucket;
	word *first_vertex;
	if (instance)
	{
		bucket = &group->buckets_by_instance[index];
		first_vertex = bucket->first_vertices;
	}
	else
	{
		bucket = &group->other_buckets[index];
		first_vertex = &bucket->first_vertices[vertex_index];
	}
	s_surface_colour_resource *resource = &group->resources[bucket->resource_index];
	if ((resource->flags & 2) && function_12de70(&resource->block, 3))
	{
		result = &resource->runtime->data->block;
		long offset;
		if (result->type == 0x2e)
			offset = *first_vertex * 4;
		else
			offset = *first_vertex * 3;
		result->offset4 = offset;
		result->offset = offset;
	}
	return result;
}

struct s_structure_lightmap_triangle
{
	long cluster_index;
	long instance_index;
	byte unknown08[0x18 - 0x08];
	long unknown18;
	byte unknown1c[4];
	long part_index;
	long part_offset;
	long lightmap_part_index;
	byte unknown2c[4];
	real u;
	real v;
};

struct s_surface_geometry_resource
{
	byte unknown00[0x28];
	s_geometry_block_info block;
	byte unknown4c[4];
	s_mesh *sections;
};

struct s_16e290_resource;
long function_16e290(s_16e290_resource *resource);

// @retail 0xd1d70
bool function_d1d70(s_structure_lightmap_triangle const *triangle, s_mesh **out)
{
	bool result = false;
	byte *bsp = (byte *)g_4e0348;
	if (triangle->instance_index != NONE)
	{
		byte *instances = *(byte **)(bsp + 0x144);
		short definition_index = *(short *)(instances + triangle->instance_index * 0x58 + 0x34);
		byte *definitions = *(byte **)(bsp + 0x13c);
		s_surface_geometry_resource *resource = (s_surface_geometry_resource *)(definitions + definition_index * 0xc8);
		if (function_12de70(&resource->block, 3))
		{
			result = true;
			*out = resource->sections;
		}
	}
	else if (*(long const *)triangle->unknown08 == NONE)
	{
		byte *clusters = *(byte **)(bsp + 0xa0);
		s_surface_geometry_resource *resource = (s_surface_geometry_resource *)(clusters + triangle->cluster_index * 0xb0);
		if (function_12de70(&resource->block, 3))
		{
			result = true;
			*out = (s_mesh *)function_16e290((s_16e290_resource *)resource);
		}
	}
	return result;
}

#define MESH_BLOCK_UV_FLOAT 0x18
#define MESH_BLOCK_UV_A 0x1e
#define MESH_BLOCK_UV_B 0x1f
#define MESH_BLOCK_COLOR_A 0x2e
#define MESH_BLOCK_COLOR_B 0x2f
#define MESH_BLOCK_NORMAL 0x30

static inline real dequantize16(short value)
{
	return ((real)value * 2.0f + 1.0f) * (1.0f / 65535.0f);
}

static inline void unpack_point_at(short *p, point2f *point)
{
	point->x = dequantize16(p[0]);
	point->y = dequantize16(p[1]);
}

static inline void unpack_point(byte *data, long index, point2f *point)
{
	short *p = (short *)data + index * 2;

	point->x = dequantize16(p[0]);
	point->y = dequantize16(p[1]);
}

// score 4 (was 4): in the compressed branch retail reads the triangle's b and
// c indices in the other order, so the two unpacked points land in the other
// pair of SSE registers and u and v swap registers.
// @retail 0xd2bf0
void function_d2bf0(s_mesh *mesh, long triangle_index, real u, real v, real *out_a, real *out_b)
{
	long i = 3;
	bool compressed = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_UV_A)
				break;
			if (block->type == MESH_BLOCK_UV_B)
			{
				compressed = true;
				break;
			}
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;

		if (!compressed)
		{
			byte *data = found->data;
			point2f *pa = (point2f *)data + triangle->a;
			point2f *pb = (point2f *)data + triangle->b;
			point2f *pc = (point2f *)data + triangle->c;

			real ax = pa->x, bx = pb->x, cx = pc->x;
			*out_a = (cx - ax) * v + (bx - ax) * u + ax;
			real ay = pa->y, by = pb->y, cy = pc->y;
			*out_b = (by - ay) * u + (cy - ay) * v + ay;
		}
		else
		{
			byte *data = found->data;
			point2f a, b, c;

			unpack_point(data, triangle->a, &a);
			unpack_point(data, triangle->b, &b);
			unpack_point(data, triangle->c, &c);

			real ax = a.x, ay = a.y;
			real bx = b.x, by = b.y;
			real cx = c.x, cy = c.y;
			*out_a = (cx - ax) * v + (bx - ax) * u + ax;
			*out_b = (cy - ay) * v + (by - ay) * u + ay;
		}
	}
}

static inline real pin_real(real value, real minimum, real maximum)
{
	if (minimum > value)
		return minimum;
	if (value > maximum)
		return maximum;
	return value;
}

PRIVATE __forceinline real uv_coordinate_pin(real const &value, real minimum, real maximum)
{
	if (value < minimum) return minimum;
	if (value > maximum) return maximum;
	return value;
}

// @retail 0xd2dc0
void function_d2dc0(s_mesh *mesh, long triangle_index, real u, real v, real *out_a, real *out_b)
{
	s_vertex_block *block = mesh->blocks + 1;
	s_triangle *triangle = mesh->triangles + triangle_index;
	byte *data = block->data + block->offset;

	if (block->type == MESH_BLOCK_UV_FLOAT)
	{
		point2f *pa = (point2f *)data + triangle->a;
		point2f *pb = (point2f *)data + triangle->b;
		point2f *pc = (point2f *)data + triangle->c;

		real ax = pa->x, bx = pb->x, cx = pc->x;
		*out_a = (cx - ax) * v + (bx - ax) * u + ax;
		real ay = pa->y, by = pb->y, cy = pc->y;
		*out_b = (by - ay) * u + (cy - ay) * v + ay;
	}
	else
	{
		short *pa = (short *)data + triangle->a * 2;
		short *pb = (short *)data + triangle->b * 2;
		short *pc = (short *)data + triangle->c * 2;
		real ax = (real)pa[0], ay = (real)pa[1];
		real bx = (real)pb[0], by = (real)pb[1];
		real cx = (real)pc[0], cy = (real)pc[1];
		point2f a, b, c;
		a.x = (ax * 2.0f + 1.0f) * (1.0f / 65535.0f);
		a.y = (ay * 2.0f + 1.0f) * (1.0f / 65535.0f);
		b.x = (bx * 2.0f + 1.0f) * (1.0f / 65535.0f);
		b.y = (by * 2.0f + 1.0f) * (1.0f / 65535.0f);
		c.x = (cx * 2.0f + 1.0f) * (1.0f / 65535.0f);
		c.y = (cy * 2.0f + 1.0f) * (1.0f / 65535.0f);

		*out_a = (c.x - a.x) * v + (b.x - a.x) * u + a.x;
		*out_b = (c.y - a.y) * v + (b.y - a.y) * u + a.y;
	}
	*out_a = uv_coordinate_pin(*out_a, -1000.0f, 1000.0f);
	*out_b = uv_coordinate_pin(*out_b, -1000.0f, 1000.0f);
}

static inline void unpack_normal(dword value, vector3f *vector)
{
	vector->i = ((real)(long)(value << 21) * (1.0f / 1048576.0f) + 1.0f) * (1.0f / 2047.0f);
	value >>= 11;
	vector->j = ((real)(long)(value << 21) * (1.0f / 1048576.0f) + 1.0f) * (1.0f / 2047.0f);
	value >>= 11;
	vector->k = ((real)(long)(value << 22) * (1.0f / 2097152.0f) + 1.0f) * (1.0f / 1023.0f);
}

// @retail 0xd31c0
bool function_d31c0(s_mesh *mesh, real u, real v, long triangle_index, vector3f *out)
{
	long i = 3;
	bool result = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_NORMAL)
				break;
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;
		dword *data = (dword *)found->data;
		vector3f a, b, c;

		unpack_normal(data[triangle->a], &a);
		unpack_normal(data[triangle->b], &b);
		unpack_normal(data[triangle->c], &c);

		out->i = (b.i - a.i) * u + (c.i - a.i) * v + a.i;
		out->j = (b.j - a.j) * u + (c.j - a.j) * v + a.j;
		out->k = (b.k - a.k) * u + (c.k - a.k) * v + a.k;
		result = true;
	}
	return result;
}

// @retail 0xd33a0
bool function_d33a0(s_mesh *mesh, long triangle_index, real u, real v, color3f *out)
{
	long i = 3;
	bool result = false;
	bool compressed = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;
		color3f a, b, c;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_COLOR_A)
				break;
			if (block->type == MESH_BLOCK_COLOR_B)
			{
				compressed = true;
				break;
			}
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;

		if (!compressed)
		{
			dword *data = (dword *)found->data;

			unpack_color3f(data[triangle->a], &a);
			unpack_color3f(data[triangle->b], &b);
			unpack_color3f(data[triangle->c], &c);
		}
		else
		{
			s_rgb24 *data = (s_rgb24 *)found->data;
			s_rgb24 pa = data[triangle->a];
			s_rgb24 pb = data[triangle->b];
			s_rgb24 pc = data[triangle->c];

			a.red = (real)pa.r * (1.0f / 255.0f);
			a.green = (real)pa.g * (1.0f / 255.0f);
			a.blue = (real)pa.b * (1.0f / 255.0f);
			b.red = (real)pb.r * (1.0f / 255.0f);
			b.green = (real)pb.g * (1.0f / 255.0f);
			b.blue = (real)pb.b * (1.0f / 255.0f);
			c.red = (real)pc.r * (1.0f / 255.0f);
			c.green = (real)pc.g * (1.0f / 255.0f);
			c.blue = (real)pc.b * (1.0f / 255.0f);
		}

		out->red = (c.red - a.red) * v + (b.red - a.red) * u + a.red;
		out->green = (c.green - a.green) * v + (b.green - a.green) * u + a.green;
		out->blue = (c.blue - a.blue) * v + (b.blue - a.blue) * u + a.blue;
		result = true;
	}
	return result;
}

// @retail 0xd3630
bool function_d3630(s_vertex_block *block, long triangle_index, s_mesh *mesh, real u, real v, color3f *out)
{
	bool result = false;

	if (block)
	{
		s_triangle *triangle = mesh->triangles + triangle_index;
		byte *data = block->data + block->offset4;
		color3f a, b, c;

		if (block->type == MESH_BLOCK_COLOR_A)
		{
			unpack_color3f(((dword *)data)[triangle->a], &a);
			unpack_color3f(((dword *)data)[triangle->b], &b);
			unpack_color3f(((dword *)data)[triangle->c], &c);
		}
		else
		{
			s_rgb24 *colors = (s_rgb24 *)data;
			s_rgb24 pa = colors[triangle->a];
			s_rgb24 pb = colors[triangle->b];
			s_rgb24 pc = colors[triangle->c];

			a.red = (real)pa.r * (1.0f / 255.0f);
			a.green = (real)pa.g * (1.0f / 255.0f);
			a.blue = (real)pa.b * (1.0f / 255.0f);
			b.red = (real)pb.r * (1.0f / 255.0f);
			b.green = (real)pb.g * (1.0f / 255.0f);
			b.blue = (real)pb.b * (1.0f / 255.0f);
			c.red = (real)pc.r * (1.0f / 255.0f);
			c.green = (real)pc.g * (1.0f / 255.0f);
			c.blue = (real)pc.b * (1.0f / 255.0f);
		}

		out->red = (c.red - a.red) * v + (b.red - a.red) * u + a.red;
		out->green = (c.green - a.green) * v + (b.green - a.green) * u + a.green;
		out->blue = (c.blue - a.blue) * v + (b.blue - a.blue) * u + a.blue;
		result = true;
	}
	return result;
}

// @retail 0xd0620
void function_d0620(long object_index, s_object_child_iterator *iterator)
{
	s_object *object;

	for (;;)
	{
		object = OBJECT_FROM_INDEX(object_index);
		long parent_index = object->value14;
		if (parent_index == NONE || !TEST_FIELD_BIT(object->flag26) || !((1 << OBJECT_FROM_INDEX(parent_index)->type) & 3))
			break;
		object_index = parent_index;
	}

	iterator->root = object_index;
	iterator->current = object_index;
	iterator->next = object->first_child;
}

struct s_effect_color_query
{
	real unknown00;
	real unknown04;
	real unknown08;
	dword color_a;
	dword color_b;
};

long __stdcall function_d2a50(long flags, long object_index, bool value, s_effect_color_query *query,
    long unused, point3f const *point);
long function_e5670(long unit_index);

bool function_14af40(bool value, point3f const *point, vector3f const *vector, long object_index,
    bool a, void *surface, point3f *hit_point);
long __stdcall function_d1e10(void const *surface, s_effect_color_query *query, long flags, bool value);
/* the collision a surface query starts from, passed by value */
struct s_surface_query_source
{
    void *collision;
};

bool function_14b120(s_surface_query_source source, void *surface);

extern vector3f const g_4406e4 = { 0.0f, 0.0f, -10.0f };
extern vector3f const g_4406f0[5] =
{
    { -10.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 0.0f, 0.0f, -40.0f }
};

// @retail 0xd2a50
long __stdcall function_d2a50(long flags, long object_index, bool value, s_effect_color_query *query,
    long supplied_direction, point3f const *point)
{
    long result = 2;
    vector3f const *directions;
    long count;
    if (flags & 1) { directions = g_4406f0; count = 5; }
    else
    {
        directions = (flags & 4) ? (vector3f const *)supplied_direction : &g_4406e4;
        count = 1;
    }
    if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
    {
        byte *bsp = (byte *)g_4e0344->locations;
        if (*(long *)(bsp + 0x1c) != NONE && *(long *)(bsp + 4) == *(long *)((byte *)g_4e0348 + 8))
        {
            vector3f const *direction = directions;
            struct { long i; point3f start, hit_point; byte surface[0x38]; } frame;
            for (frame.i = 0; frame.i < count;)
            {
                if (result != 2) break;
                point3f &start = frame.start;
                point3f &hit_point = frame.hit_point;
                byte *surface = frame.surface;
                start.x = point->x - direction->i * 0.0010000000474974513f;
                start.y = point->y - direction->j * 0.0010000000474974513f;
                start.z = point->z - direction->k * 0.0010000000474974513f;
                if (function_14af40((bool)value, &start, direction, object_index, true, surface, &hit_point))
                    result = function_d1e10(surface, query, flags, value);
                long next = frame.i + 1;
                frame.i = next;
                direction++;
            }
        }
    }
    return result;
}

// @retail 0xd2bb0
long function_d2bb0(void *source, s_effect_color_query *query)
{
    byte surface[0x38];
    s_effect_color_query **query_reference = &query;
    long result = 2;
    s_surface_query_source query_source = { source };
    if (function_14b120(query_source, surface))
        result = function_d1e10(surface, *query_reference, 0, 0);
    return result;
}

PRIVATE __forceinline bool surface_status_failed(long status)
{
    return status == 1 || status == 2 || status == 3;
}

// score 44 (was 44): early object-flag register and pointer/flags homes
// differ, followed by tag-bit extraction. Zero then OR 148; const tag 44;
// byte tag bitfield 135; dword object bitfield 44; named query failure 44;
// explicit final exits 181; inverse failure result 44; retry flag update 44.
// @retail 0xd1850
long function_d1850(long object_index, bool value, s_effect_color_query *query)
{
    bool failed = false;
    if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
    {
        byte *bsp = (byte *)g_4e0344->locations;
        if (*(long *)(bsp + 0x1c) != NONE && *(long *)(bsp + 4) == *(long *)((byte *)g_4e0348 + 8))
        {
            s_object *object = OBJECT_FROM_INDEX(object_index);
            dword object_flags = *(dword *)((byte *)object + 4);
            byte *volatile definition = g_4e3b44[object->tag_index & 0xffff].bytes;
            long volatile flags;
            bool object_bit = (object_flags >> 13) & 1;
            if (object_bit) flags = 1;
            else flags = 0;
            byte tag_flags = definition[2];
            bool tag_bit = (tag_flags >> 1) & 1;
            if (tag_bit) flags |= 1;
            point3f *point = (point3f *)((byte *)object + 0x30);
            long status = function_d2a50(flags, object_index, value, query, 0, point);
            if (surface_status_failed(status))
            {
                failed = true;
                long retry_flags = flags;
                if (retry_flags & 1)
                {
                    status = function_d2a50(retry_flags & ~1, object_index, value, query, 0, point);
                    if (surface_status_failed(status)) failed = true;
                }
                if (status != 3)
                {
                    short type = *(short *)definition;
                    if (type == 1)
                    {
                        byte *unit_tag_bytes = g_4e3b44[OBJECT_FROM_INDEX(object_index)->tag_index & 0xffff].bytes;
                        if (!((1 << unit_tag_bytes[0x1f0]) & 0x28)) goto no_sky;
                    }
                    else if (type == 0) { if (function_e5670(object_index) != 2) goto no_sky; }
                    else if (type == 0xc)
                    {
                        byte *unit_tag_bytes = g_4e3b44[OBJECT_FROM_INDEX(object_index)->tag_index & 0xffff].bytes;
                        if (!(bool)((*(dword *)(unit_tag_bytes + 0xd4) >> 4) & 1)) goto no_sky;
                    }
                    else goto no_sky;
                    failed = surface_status_failed(function_d1a30(object_index, point, (s_sky_surface_record *)query));
                no_sky:;
                }
            }
        }
    }
    return failed ? 0 : 1;
}

struct s_lighting_record
{
	color3f color0;
	vector3f direction0;
	real length;
	real value1c;
	color3f color1;
	vector3f direction1;
	color3f color2;
	vector3f direction2;
	dword unknown50;
};

struct s_tag_data
{
	long size;
	byte *address;
};

struct s_lighting_parameters
{
	dword flags;
	real bias;
	color3f lower1;
	color3f upper1;
	real angle1;
	s_tag_data scale1;
	color3f lower2;
	color3f upper2;
	color3f lower_base;
	color3f upper_base;
	real angle2;
	s_tag_data scale2;
	color3f lower0;
	color3f upper0;
	s_tag_data scale0;
	s_tag_data value1c;
};

struct s_lighting_parameter_block
{
	long count;
	s_lighting_parameters *entries;
};

void function_117c80(vector3f const *input, vector3f *output, real angle);
real function_13b390(void const *function, real input, real range);

#define lighting_pin(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

PRIVATE __forceinline void lighting_pin_color(color3f const *input, color3f const *lower, color3f const *upper, color3f *out)
{
	out->red = lighting_pin(input->red, lower->red, upper->red);
	out->green = lighting_pin(input->green, lower->green, upper->green);
	out->blue = lighting_pin(input->blue, lower->blue, upper->blue);
}

PRIVATE __forceinline real lighting_function(s_tag_data const *function, real input)
{
	real value = function_13b390(function, input, 1.0f);
	byte *header = function->address;
	if (!(header[1] & 0xf0))
	{
		real lower = *(real *)(header + 4);
		real upper = *(real *)(header + 8);
		value = lighting_pin(value, 0.0f, 1.0f);
		value = (upper - lower) * value + lower;
	}
	return value;
}

PRIVATE __forceinline void lighting_negate(vector3f *vector)
{
	vector->i = 0.0f - vector->i;
	vector->j = 0.0f - vector->j;
	vector->k = 0.0f - vector->k;
}

PRIVATE __forceinline void lighting_rotate(vector3f const *input, real angle, vector3f *out)
{
	real sine = (real)sin(angle);
	real cosine = (real)cos(angle);
	vector3f axis = *g_4687b0;
	vector3f perpendicular;
	cross3f(input, &axis, &perpendicular);
	real projection = (axis.k * input->k + axis.j * input->j + axis.i * input->i) * (1.0f - cosine);
	out->i = input->i * cosine + axis.i * projection - perpendicular.i * sine;
	out->j = input->j * cosine + axis.j * projection - perpendicular.j * sine;
	out->k = input->k * cosine + axis.k * projection - perpendicular.k * sine;
}

PRIVATE __forceinline real lighting_normalize_ordered(vector3f *v, real square)
{
    real magnitude = (real)sqrt(square);
    if (!(fabs(magnitude) < 0.0001f))
    {
        real inverse = 1.0f / magnitude;
        v->i = inverse * v->i;
        v->j = v->j * inverse;
        v->k = v->k * inverse;
        return magnitude;
    }
    return 0.0f;
}

// score 539 (was 539): frame 0x2c versus 0x28, stored result and block/query registers differ.
// Late success return 577; short result 585; direct primary output 598; early square 579;
// negate normal before copy 546; late return plus parameter order 577; ordered countdown
// traversal 560 (retail length); countdown with tested result 566.
// @retail 0xd37f0
bool function_d37f0(s_lighting_parameter_block *block, long type, s_effect_color_query const *query,
	s_lighting_record *record, color3f const *base, color3f const *lightmap)
{
	s_lighting_parameters *parameters = NULL;
	long count = block->count;
	if (count > 0)
	{
		dword mask = 1 << (type + 1);
		for (long i = 0; i < count; i++)
		{
			if ((block->entries[i].flags & mask) || (block->entries[i].flags & 1))
				parameters = &block->entries[i];
		}
	}
	bool result = parameters != NULL;
	if (result)
	{
		vector3f const *input_normal = (vector3f const *)query;
		vector3f normal = { input_normal->i, input_normal->j, input_normal->k };
		real length = (real)sqrt(input_normal->i * input_normal->i + input_normal->j * input_normal->j + input_normal->k * input_normal->k);
		record->length = length;
		lighting_normalize_ordered(&normal, (normal.k * normal.k + normal.j * normal.j) + normal.i * normal.i);
		color3f adjusted;
		adjusted.red = lightmap->red + parameters->bias;
		adjusted.green = lightmap->green + parameters->bias;
		adjusted.blue = lightmap->blue + parameters->bias;
		color3f primary;
		lighting_pin_color(&adjusted, &parameters->lower1, &parameters->upper1, &primary);
		record->color1 = primary;
		function_117c80(&normal, &record->direction1, parameters->angle1 * 0.01745329238474369f);
		lighting_negate(&record->direction1);
		color3f limited_base;
		color3f limited_lightmap;
		lighting_pin_color(base, &parameters->lower_base, &parameters->upper_base, &limited_base);
		lighting_pin_color(&adjusted, &parameters->lower2, &parameters->upper2, &limited_lightmap);
		real square = length * length;
		record->color2.red = ((limited_base.red * limited_lightmap.red) * square) * 2.0f + record->color1.red * (1.0f - square);
		record->color2.green = ((limited_base.green * limited_lightmap.green) * square) * 2.0f + record->color1.green * (1.0f - square);
		record->color2.blue = ((limited_base.blue * limited_lightmap.blue) * square) * 2.0f + record->color1.blue * (1.0f - square);
		record->direction2 = record->direction1;
		lighting_negate(&record->direction2);
		lighting_rotate(&record->direction2, parameters->angle2 * 0.01745329238474369f, &record->direction2);
		lighting_pin_color(&adjusted, &parameters->lower0, &parameters->upper0, &record->color0);
		record->direction0 = normal;
		lighting_negate(&record->direction0);
		if (record->direction0.k > -0.75f)
		{
			record->direction0.k = lighting_pin(record->direction0.k, -1.0f, -0.75f);
			lighting_normalize_ordered(&record->direction0,
			(record->direction0.i * record->direction0.i + record->direction0.j * record->direction0.j) +
			record->direction0.k * record->direction0.k);
		}
		real luminance = 2.0f * (0.11f * lightmap->blue + 0.59f * lightmap->green + 0.3f * lightmap->red);
		record->value1c = lighting_function(&parameters->value1c, lighting_pin(luminance, 0.0f, 1.0f));
		real scale1 = lighting_function(&parameters->scale1, length);
		real scale2 = lighting_function(&parameters->scale2, length);
		record->color1.red *= scale1;
		record->color1.green *= scale1;
		record->color1.blue *= scale1;
		record->color2.red *= scale2;
		record->color2.green *= scale2;
		record->color2.blue *= scale2;
		luminance = 0.114f * adjusted.blue + 0.587f * adjusted.green + 0.299f * adjusted.red;
		real scale0 = (lighting_function(&parameters->scale0, luminance) * (1.0f - (0.1f * length + 0.6f))) * 1.4f;
		record->color0.red *= scale0;
		record->color0.green *= scale0;
		record->color0.blue *= scale0;
	}
	return result;
}

real g_467484 = 15.0f;
real g_467494 = 5.0f;
real function_30bf0(vector3f *vector);

// score 364 (was 429): frame 0x4c versus 0x40 and normal/color scheduling still differ;
// copied limited color kept; early squared length 428, decoded-color names 429,
// in-place rotation 464, normal-square scalar 429, copy plus early square 453 (fast).
// Base clamped in place 444; outer limited-color lifetime 364; copied color plus
// in-place rotation 435 (retail length); scales at first use 381; shared surface clamp 481;
// shared axis view 515, with in-place rotation 463, with in-place base clamp 542
// (all three axis views lose the matched caller). Original helper and bodies restored.
// @retail 0xd4080
void function_d4080(long type, s_effect_color_query const *query, s_lighting_record *record, long flag)
{
	long const *const flag_reference = &flag;
	long flag_value = *flag_reference;
	(void)flag_value;
	byte *context = (byte *)g_4e0350;
	vector3f const *input_normal = (vector3f const *)query;
	real length = (real)sqrt(input_normal->i * input_normal->i + input_normal->j * input_normal->j + input_normal->k * input_normal->k);
	volatile bool mobile = false;
	if (type == 0 || type == 1 || type == 2)
		mobile = true;
	vector3f normal = *input_normal;
	record->length = lighting_normalize_ordered(&normal, (normal.i * normal.i + normal.k * normal.k) + normal.j * normal.j);
	color3f base, lightmap;
	unpack_color3f(query->color_a, &base);
	unpack_color3f(query->color_b, &lightmap);
	long tag_index = *(long *)(context + 0x33c);
	if (tag_index == NONE)
		tag_index = *(long *)((byte *)g_4e034c + 0x184);
	s_lighting_parameter_block *block = (s_lighting_parameter_block *)g_4e3b44[tag_index & 0xffff].bytes;
	if (!block || !function_d37f0(block, type, query, record, &base, &lightmap))
	{
		record->color1 = lightmap;
		function_117c80(&normal, &record->direction1, g_467484);
		lighting_negate(&record->direction1);
		color3f limited = base;
		limited.red = limited.red < 0.25 ? 0.25 : limited.red > 0.75 ? 0.75 : limited.red;
		limited.green = limited.green < 0.25 ? 0.25 : limited.green > 0.75 ? 0.75 : limited.green;
		limited.blue = limited.blue < 0.25 ? 0.25 : limited.blue > 0.75 ? 0.75 : limited.blue;
		record->color2.red = lighting_pin((lightmap.red * limited.red) * 2.0f, 0.0f, 1.0f);
		record->color2.green = lighting_pin((lightmap.green * limited.green) * 2.0f, 0.0f, 1.0f);
		record->color2.blue = lighting_pin((lightmap.blue * limited.blue) * 2.0f, 0.0f, 1.0f);
		if (!(record->color2.red > 0.0f)) record->color2.red = 0.0f;
		if (!(record->color2.green > 0.0f)) record->color2.green = 0.0f;
		if (!(record->color2.blue > 0.0f)) record->color2.blue = 0.0f;
		real square = length * length;
		record->color2.red = (1.0f - square) * record->color1.red + record->color2.red * square;
		record->color2.green = (1.0f - square) * record->color1.green + record->color2.green * square;
		record->color2.blue = (1.0f - square) * record->color1.blue + record->color2.blue * square;
		vector3f turned = record->direction1;
		lighting_negate(&turned);
		lighting_rotate(&turned, g_467494 * 0.01745329238474369f, &record->direction2);
		record->color0 = lightmap;
		record->direction0.i = 0.0f - normal.i;
		record->direction0.j = 0.0f - normal.j;
		record->direction0.k = 0.0f - normal.k;
		record->direction0.k *= 2.0f;
		function_30bf0(&record->direction0);
		real k = mobile ? 1.18f : 0.75f;
		real luminance = 0.114f * lightmap.blue + 0.587f * lightmap.green + 0.299f * lightmap.red;
		real scale0 = (1.0f - (0.1f * length + 0.6f)) * ((k - 0.7f) * luminance + 0.7f);
		real scale1 = 0.05f * length + 0.4f;
		real scale2 = 0.4f - 0.05f * length;
		record->color0.red *= scale0;
		record->color0.green *= scale0;
		record->color0.blue *= scale0;
		record->color1.red *= scale1;
		record->color1.green *= scale1;
		record->color1.blue *= scale1;
		record->color2.red *= scale2;
		record->color2.green *= scale2;
		record->color2.blue *= scale2;
		record->value1c = 1.0f;
	}
	color3f lower = { 0.0f, 0.0f, 0.0f };
	color3f upper = { 1.0f, 1.0f, 1.0f };
	lighting_pin_color(&record->color1, &lower, &upper, &record->color1);
	lighting_pin_color(&record->color2, &lower, &upper, &record->color2);
	lighting_pin_color(&record->color0, &lower, &upper, &record->color0);
}

#undef lighting_pin

#include "unknown_03bcb0.h"

PRIVATE inline D3DTexture *surface_bitmap_cached(s_bitmap_data *bitmap)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = NULL;
    if (view->last_frame > g_4e6488)
        texture = view->texture;
    return texture;
}

PRIVATE __forceinline D3DTexture *surface_bitmap_texture(s_bitmap_data *bitmap, dword flags, real bias)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = surface_bitmap_cached(bitmap);
    if (!texture)
    {
        _mm_prefetch((char const *)&view->flags, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset1, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset2, _MM_HINT_T0);
        _mm_prefetch((char const *)view->texture, _MM_HINT_T0);
        texture = texture_cache_bitmap_get_texture(bitmap, flags, bias);
        if (!texture)
            texture = function_12ce00(bitmap, flags, bias);
    }
    return texture;
}

// @retail 0xd15e0
D3DTexture *function_d15e0(s_bitmap_data *bitmap, real bias)
{
    return surface_bitmap_texture(bitmap, 0, bias);
}

// @retail 0xd1630
D3DTexture *function_d1630(s_bitmap_data *bitmap)
{
    return surface_bitmap_texture(bitmap, 6, 0.0f);
}

// @retail 0xd1680
D3DTexture *function_d1680(s_bitmap_data *bitmap)
{
    return surface_bitmap_texture(bitmap, 4, 0.0f);
}

struct s_type_7ba8e9;
struct s_bitmap_view;
struct D3DSurface;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
D3DTexture *function_12360(s_bitmap_view *bitmap, real bias);
D3DTexture *function_1cfb0(s_bitmap_view *bitmap);
bool __stdcall function_1d2f0(D3DSurface *surface, byte *bitmap);
dword function_1362b0(s_type_7ba8e9 const *bitmap, point2f const *uv, real detail);
dword function_015cd0(long index, dword entry);

// @retail 0xd2f90
long function_d2f90(long flags, short bitmap_index, long palette_index, real u, real v, color3f *out)
{
    long result = 1;
    byte temporary[0x74];
    if (g_4e0344 && g_4e0344->count > 0)
    {
        long tag = *(long *)((byte *)g_4e0344->locations + 0x1c);
        if (tag != NONE)
        {
            s_bitmap_data *bitmap = (s_bitmap_data *)function_137550(tag, bitmap_index);
            if ((flags & 2) && bitmap && !function_d1680(bitmap))
                function_1cfb0((s_bitmap_view *)bitmap);
            if (bitmap)
            {
                if (!function_d15e0(bitmap, 0.0f))
                {
                    if (!*(void **)((byte *)bitmap + 0x54))
                    {
                        function_12360((s_bitmap_view *)bitmap, 0.0f);
                        result = 1;
                    }
                    else
                        result = 0;
                }
                else if (function_d1680(bitmap))
                    result = 0;
                else
                {
                    function_d1630(bitmap);
                    if (function_1d2f0((D3DSurface *)function_d15e0(bitmap, 0.0f), temporary))
                    {
                        result = 3;
                        bitmap = (s_bitmap_data *)temporary;
                    }
                    else
                        bitmap = NULL;
                }
                if (result != 1 && bitmap)
                {
                    point2f uv = { u, v };
                    dword pixel = function_1362b0((s_type_7ba8e9 const *)bitmap, &uv, 1.0f);
                    if (*(short *)((byte *)bitmap + 0xc) == 0x12)
                        pixel = function_015cd0(palette_index, pixel);
                    unpack_color3f(pixel, out);
                    goto done;
                }
            }
            out->blue = 0.0f;
            out->green = 0.0f;
            out->red = 0.0f;
        }
    }
done:
    return result;
}

// @retail 0xd30d0
bool function_d30d0(long tag, long flags, real u, real v, color3f *out)
{
    bool result = true;
    byte temporary[0x74];
    s_bitmap_data *sample_bitmap = NULL;
    s_bitmap_data *bitmap = NULL;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (definition && *(long *)(definition + 0x44) > 0)
            bitmap = *(s_bitmap_data **)(definition + 0x48);
    }
    if ((flags & 2) && bitmap && !function_d1680(bitmap))
        function_1cfb0((s_bitmap_view *)bitmap);
    if (bitmap)
    {
        if (!function_d15e0(bitmap, 0.0f))
        {
            if (*(void **)((byte *)bitmap + 0x54))
                sample_bitmap = bitmap;
            else
                function_12360((s_bitmap_view *)bitmap, 0.0f);
        }
        else if (function_1d2f0((D3DSurface *)function_d15e0(bitmap, 0.0f), temporary))
            sample_bitmap = (s_bitmap_data *)temporary;
    }
    if (sample_bitmap)
    {
        point2f uv = { u, v };
        unpack_color3f(function_1362b0((s_type_7ba8e9 const *)sample_bitmap, &uv, 1.0f), out);
    }
    else
    {
        out->blue = 0.0f;
        out->green = 0.0f;
        out->red = 0.0f;
        result = false;
    }
    return result;
}

struct s_33a0b_default
{
    dword unknown0;
    vector3f vector;
};
extern s_33a0b_default *g_4686d4;
extern color3f *g_468714;

/* an instance's or cluster's lightmap bitmap (4 bytes) */
struct s_d16d0_bitmap_entry
{
    short bitmap_index;
    char palette_index;
    byte unknown3;
};

// @retail 0xd16d0
bool function_d16d0(s_structure_lightmap_triangle const *triangle, color3f *out, real *weight)
{
    bool result = false;
    /* never assigned: retail hands back whatever this slot holds (the one
       caller ignores it) */
    real sample_weight;
    if (*(long const *)triangle->unknown08 == NONE)
    {
        s_mesh *mesh = NULL;
        if (function_d1d70(triangle, &mesh))
        {
            byte *bsp = (byte *)g_4e0344->locations;
            long index = triangle->instance_index;
            real v = 0.0f, u = 0.0f;
            long palette_index;
            long bitmap_index;
            if (index != NONE)
            {
                s_d16d0_bitmap_entry const *entry = &(*(s_d16d0_bitmap_entry const **)(bsp + 0x4c))[index];
                palette_index = entry->palette_index;
                bitmap_index = entry->bitmap_index;
            }
            else
            {
                s_d16d0_bitmap_entry const *entry = &(*(s_d16d0_bitmap_entry const **)(bsp + 0x2c))[triangle->cluster_index];
                palette_index = entry->palette_index;
                bitmap_index = entry->bitmap_index;
            }

            color3f color = *(color3f *)g_468714;
            function_d2bf0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
            if (function_d2f90(0, bitmap_index, palette_index, u, v, &color) == 0)
            {
                result = true;
                *weight = sample_weight;
                if (0.0f > color.red)
                    color.red = 0.0f;
                else if (color.red > 1.0f)
                    color.red = 1.0f;
                if (0.0f > color.green)
                    color.green = 0.0f;
                else if (color.green > 1.0f)
                    color.green = 1.0f;
                if (0.0f > color.blue)
                    color.blue = 0.0f;
                else if (color.blue > 1.0f)
                    color.blue = 1.0f;
                *out = color;
            }
        }
    }
    return result;
}

struct s_cluster_query;
struct s_structure_collision_result;
bool structure_get_lightmap_triangle(s_structure_collision_result const *query, s_structure_lightmap_triangle *triangle);

// @retail 0xd47d0
bool __stdcall function_d47d0(s_cluster_query const *query, color3f *out)
{
    s_structure_lightmap_triangle triangle;
    *out = *(color3f *)&g_4686d4->vector;
    if (g_4e0344 && structure_get_lightmap_triangle((s_structure_collision_result const *)query, &triangle))
    {
        real weight;
        function_d16d0(&triangle, out, &weight);
    }
    return false;
}

struct s_sample_point
{
    real index;
    real weight[4];
};
struct s_24490_definition;
struct s_13da30_owner;
bool function_0241c0(long key, long block, vector3f *scale_out, long index,
    s_sample_point const *a, s_sample_point const *b, s_sample_point const *c,
    real u, real v, vector3f *result);
void *function_24490(long key, s_24490_definition *definition, long entry_key);
real magnitude3d(vector3f const *vector);
bool function_13d9f0(long type);
long function_13da30(s_13da30_owner const *owner);
extern color3f const *g_468734;
extern color3f *g_46872c;

PRIVATE __forceinline void surface_pin_color(color3f *color)
{
    color->red = pin_real(color->red, 0.0f, 1.0f);
    color->green = pin_real(color->green, 0.0f, 1.0f);
    color->blue = pin_real(color->blue, 0.0f, 1.0f);
}

PRIVATE __forceinline void surface_sample_unpack(short const *source, s_sample_point *point)
{
    point->index = (real)source[0];
    point->weight[0] = dequantize16(source[1]);
    point->weight[1] = dequantize16(source[2]);
    point->weight[2] = dequantize16(source[3]);
    point->weight[3] = dequantize16(source[4]);
}

// score 819 (was 835): the long per-vertex flag and the sample points declared at the top of
// their scope; mesh, UV and color homes and helper expansion still differ (frame 0xa8 versus 0x90).
// Native bitmap word 835; palette byte 852; both 852; UV role names 835; inline depth zero 2009;
// metadata dispatch 847; long per-vertex alone 831; dynamic entry lifetime 835; model sample
// lifetime 825; depth one 835; material noinline 870/866.
// Dynamic validity long 863; base validity short 819; accumulated sampled color 828;
// weighted normal loop 892; both lengths before scaling 837; one broad sample scope
// with the other two narrowed (first/second/third) 831/831/831.
// @retail 0xd1e10
long __stdcall function_d1e10(void const *surface, s_effect_color_query *query, long flags, bool value)
{
    s_structure_lightmap_triangle const *triangle = (s_structure_lightmap_triangle const *)surface;
    long result = 2;
    if (*(long const *)triangle->unknown08 == NONE)
    {
        s_mesh *mesh = NULL;
        if (function_d1d70(triangle, &mesh))
        {
            byte *part = *(byte **)((byte *)mesh + 4) + triangle->part_index * 0x48;
            if (function_13d9f0(*(word *)part))
            {
                if ((byte)value)
                {
                    query->unknown00 = 0.0f;
                    query->unknown04 = 0.0f;
                    query->unknown08 = -1.0f;
                    query->color_a = 0xff404040;
                    query->color_b = 0xff404040;
                }
                return 0;
            }
            byte *bsp = (byte *)g_4e0344->locations;
            long index = triangle->instance_index;
            real u = 0.0f, v = 0.0f;
            real base_u = 0.0f, base_v = 0.0f;
            long bitmap_index = NONE;
            long palette_index = NONE;
            long per_vertex = index != NONE &&
                *(word *)(*(byte **)((byte *)g_4e0348 + 0x144) + index * 0x58 + 0x56) != 0;
            if (!per_vertex)
            {
                byte *entry = index != NONE ? *(byte **)(bsp + 0x4c) + index * 4 :
                    *(byte **)(bsp + 0x2c) + triangle->cluster_index * 4;
                bitmap_index = *(short *)entry;
                palette_index = (signed char)entry[2];
            }
            vector3f normal = *g_4687b0;
            color3f base = *g_468734;
            color3f lightmap = *g_46872c;
            long material = function_13da30((s_13da30_owner const *)(
                *(byte **)((byte *)g_4e0348 + 0xa8) + *(short *)(part + 4) * 0x20));
            if (!per_vertex)
            {
                function_d2bf0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
                result = function_d2f90(flags, bitmap_index, palette_index, u, v, &lightmap);
            }
            else
            {
                s_vertex_block *block = function_d35a0(index, (s_surface_colour_group *)bsp, true, NONE);
                if (block)
                {
                    function_d3630(block, triangle->lightmap_part_index, mesh, triangle->u, triangle->v, &lightmap);
                    result = 0;
                }
                else
                    result = 1;
            }
            function_d2dc0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &base_u, &base_v);
            if (!function_d30d0(material, flags, base_u, base_v, &base))
                base.red = base.green = base.blue = 0.5f;
            function_d31c0(mesh, triangle->u, triangle->v, triangle->lightmap_part_index, &normal);
            surface_pin_color(&base);
            surface_pin_color(&lightmap);
            query->color_a = pack_color3f(&base);
            query->color_b = pack_color3f(&lightmap);
            *(vector3f *)query = normal;
        }
    }
    else
    {
        byte const *record = (byte const *)surface;
        long section_index = *(long const *)(record + 0x14);
        if (section_index == NONE)
            return result;
        byte *definition = g_4e3b44[*(long const *)(record + 0xc) & 0xffff].bytes;
        s_mesh *mesh = *(s_mesh **)(*(byte **)(definition + 0x28) + section_index * 0x5c + 0x34);
        word const *vertices = (word const *)((byte *)mesh->triangles + triangle->part_offset * 2);
        s_sample_point point_a;
        s_sample_point point_b;
        s_sample_point point_c;
        bool dynamic_valid = false;
        color3f dynamic_color;
        vector3f dynamic_normal;
        if (*(long *)(definition + 0x74) > 0)
        {
            byte *block = (byte *)function_24490(*(long const *)(record + 8),
                *(s_24490_definition **)(definition + 0x78), section_index);
            if (block)
            {
                byte *data = *(byte **)(block + 0xc) + *(long *)(block + 4);
                s_sample_point const *a, *b, *c;
                if (block[0] != 0x39)
                {
                    a = (s_sample_point *)(data + vertices[0] * 20);
                    b = (s_sample_point *)(data + vertices[1] * 20);
                    c = (s_sample_point *)(data + vertices[2] * 20);
                }
                else
                {
                    surface_sample_unpack((short *)(data + vertices[0] * 10), &point_a);
                    surface_sample_unpack((short *)(data + vertices[1] * 10), &point_b);
                    surface_sample_unpack((short *)(data + vertices[2] * 10), &point_c);
                    a = &point_a; b = &point_b; c = &point_c;
                }
                dynamic_valid = function_0241c0(*(long const *)(record + 8), *(long const *)triangle->unknown2c,
                    &dynamic_normal, *(long const *)(record + 0x1c), a, b, c, triangle->u, triangle->v,
                    (vector3f *)&dynamic_color);
            }
            result = dynamic_valid ? 0 : 1;
        }
        color3f base;
        vector3f normal;
        bool base_valid = function_d33a0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &base)
            && function_d31c0(mesh, triangle->u, triangle->v, triangle->lightmap_part_index, &normal);
        if (dynamic_valid && base_valid)
        {
            real dynamic_weight = dynamic_color.blue * 0.11f + dynamic_color.green * 0.59f + dynamic_color.red * 0.3f;
            real base_weight = base.blue * 0.11f + base.green * 0.59f + base.red * 0.3f;
            if (dynamic_weight > 0.0001f && base_weight > 0.0001f)
            {
                color3f combined;
                combined.red = base.red + dynamic_color.red;
                combined.green = base.green + dynamic_color.green;
                combined.blue = base.blue + dynamic_color.blue;
                surface_pin_color(&combined);
                query->color_b = pack_color3f(&combined);
                real base_length = magnitude3d(&normal);
                if (base_length > 0.0001f)
                {
                    real scale = 1.0f / base_length;
                    for (long component = 0; component < 3; ++component)
                        normal.n[component] *= scale;
                }
                real dynamic_length = magnitude3d(&dynamic_normal);
                if (dynamic_length > 0.0001f)
                {
                    real scale = 1.0f / dynamic_length;
                    for (long component = 0; component < 3; ++component)
                        dynamic_normal.n[component] *= scale;
                }
                vector3f *out = (vector3f *)query;
                out->i = dynamic_normal.i * dynamic_weight + normal.i * base_weight;
                out->j = dynamic_normal.j * dynamic_weight + normal.j * base_weight;
                out->k = dynamic_normal.k * dynamic_weight + normal.k * base_weight;
                function_30bf0(out);
                real length = dynamic_length > base_length ? dynamic_length : base_length;
                for (long component = 0; component < 3; ++component)
                    out->n[component] *= length;
            }
            else if (dynamic_weight > 0.0001f)
            {
                surface_pin_color(&dynamic_color);
                query->color_b = pack_color3f(&dynamic_color);
                *(vector3f *)query = dynamic_normal;
            }
            else
            {
                surface_pin_color(&base);
                query->color_b = pack_color3f(&base);
                *(vector3f *)query = normal;
            }
        }
        else if (dynamic_valid)
        {
            surface_pin_color(&dynamic_color);
            query->color_b = pack_color3f(&dynamic_color);
            *(vector3f *)query = dynamic_normal;
        }
        else if (base_valid)
        {
            surface_pin_color(&base);
            query->color_b = pack_color3f(&base);
            *(vector3f *)query = normal;
        }
        else
        {
            query->color_a = query->color_b = 0;
            *(vector3f *)query = *g_4687b0;
        }
        real u, v;
        function_d2dc0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
        byte *part = *(byte **)((byte *)mesh + 4) + triangle->part_index * 0x48;
        long material = function_13da30((s_13da30_owner const *)(
            *(byte **)(definition + 0x64) + *(short *)(part + 4) * 0x20));
        color3f sampled;
        if (function_d30d0(material, 0, u, v, &sampled))
        {
            surface_pin_color(&sampled);
            query->color_a = pack_color3f(&sampled);
        }
        else
            query->color_a = 0xffc0c0d0;
    }
    return result;
}
