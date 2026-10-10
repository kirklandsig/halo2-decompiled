// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1321F0.CPP */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "object_queries.h"
#include <math.h>
#include <string.h>
#include <xtl.h>

#define k_pi 3.14159265359f

struct s_1321f0
{
	byte unknown000[0x21c];
	dword flags[1];
};

// @retail 0x1321f0
bool function_1321f0(long index, s_1321f0 const *data)
{
	return (data->flags[index >> 5] & (1 << (index & 31))) != 0;
}

struct s_sort_entry
{
	short value;
	byte unknown02[0x1a - 2];
};

struct s_sort_context
{
	byte unknown000[0xa6c];
	short count;
	s_sort_entry entries[128];
	byte unknown176e[2];
	long order[0x200];
};

// @retail 0x134950
bool __stdcall function_134950(long a, long b, void const *context)
{
	s_sort_context const *data = (s_sort_context const *)context;
	return data->entries[a].value > data->entries[b].value;
}

struct s_132d60
{
	s_sort_context *context;
	byte unknown04[4];
	short cluster_index;
	byte unknown0a[0x2a68 - 0xa];
	point3f point;
	real radius;
};

extern long g_4e7414;
extern bool g_4e7411;
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, long maximum_count, short *clusters);
void sort_4byte(long *elements, unsigned long count, void *unused, bool (__stdcall *compare)(long, long, const void *), const void *context);

// @retail 0x132d60
void function_132d60(s_132d60 *data)
{
	real radius = data->radius;
	short clusters[0x200];
	short count = 0;

	if (data->cluster_index != NONE)
	{
		if (radius > 0.0f)
		{
			g_4e7414++;
			g_4e7411 = true;
			count = function_14a5b0(data->cluster_index, &data->point, radius, 0x200, clusters);
			g_4e7411 = false;
			if (count > 0x200)
			{
				count = 0x200;
			}
		}
		else
		{
			count = 1;
			clusters[0] = data->cluster_index;
		}
	}

	data->context->count = count;
	for (long i = 0; i < data->context->count; i++)
	{
		s_sort_entry *entry = &data->context->entries[i];
		entry->value = clusters[i];
		*(short *)&entry->unknown02[0] = 0;
		data->context->order[i] = i;
	}
	sort_4byte(data->context->order, data->context->count, &radius, function_134950, data->context);
}

/* ---- bit vector pools ---- */

/* the entry list of unknown_163110.cpp (its constructor and swap are there) */
class c_entry_list
{
public:
	c_entry_list(long maximum_count);
	~c_entry_list()
	{
		delete[] shorts_b;
		shorts_b = NULL;
		delete[] longs_a;
		longs_a = NULL;
		delete[] longs_c;
		longs_c = NULL;
		delete[] shorts_d;
		shorts_d = NULL;
	}
	void swap(short index0, short index1);
	bool add(long a, short b, long c, short d);
	short find(long a);

	long maximum_count;
	short count;
	short *shorts_b;
	long *longs_a;
	long *longs_c;
	short *shorts_d;
};

struct s_bit_vector_pool_sizes
{
	short unknown0;
	short list_sizes[4];
	short record_count;
};

struct s_bit_vector_pool
{
	void *context;
	s_location location;
	c_entry_list *lists[4];
	long indices[0x80];
	dword flags[0x10];
	dword pool[0x200];
	word pool_used;
	word entry_count;
	dword entries[0x200][4];
	dword flags2a60;
	short frustum_count;
	byte unknown2a66[2];
	point3f center;
	real radius;
	long mode;
	long query_type;
	byte unknown2a80[4];
	real plane_offset;
	plane3f plane;
	long counters[13];
	byte *records;
	void *filter;
	s_bit_vector_pool_sizes sizes;
};

/* the records are 0x9c bytes each */
#define k_bit_vector_pool_record_size 0x9c

struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index, point3f const *center, real radius, bool *contained);
long function_461c0(point3f const *a, point3f const *b, real radius);
bool function_c3110(long index);
bool function_c3140(long index);
extern s_record_pool *g_4e030c;
extern long g_4e0308;

struct s_132a30_light
{
	byte unknown00[0xc];
	long stamp;
	byte unknown10[8];
	point3f center;
	real radius;
	byte unknown28[0x110 - 0x28];
};

// @retail 0x132a30
void function_132a30(s_bit_vector_pool *data, long index, long section_index, bool visible)
{
	if (function_c3140(index))
	{
		s_132a30_light *light = &((s_132a30_light *)g_4e030c->data)[index & 0xffff];
		long local_1 = *(long volatile const *)&light->stamp;
		if (local_1 != g_4e0308)
		{
			if (!visible)
			{
				point3f center = light->center;
				real radius = light->radius;
				visible = false;
				char intersects = true;
				if (data->query_type == 0)
					intersects = function_165010((s_frustum_set_view *)data->context, section_index, &center, radius, &visible);
				else if (data->query_type == 1)
					intersects = (char)function_461c0(&data->center, &center, data->radius + radius);
				if (!(char)intersects && ((char)data->flags2a60 >> 7) == 0)
					return;
			}
			if (data->lists[1]->add(index, 0, 0, NONE))
				function_c3110(index);
		}
	}
}

// @retail 0x132e60
bool function_132e60(s_bit_vector_pool *data, point3f const *center, real radius,
	c_entry_list *list, long index, short section, bool visible, bool *contained, bool *first, bool *second)
{
	bool result = true;
	*first = true;
	*second = true;
	*contained = false;
	if (data->query_type == 0)
	{
		if (!visible)
			result = function_165010((s_frustum_set_view *)data->context, section, center, radius, contained);
		if (result && !*contained && data->mode == 0 && radius <= 2.5f)
			*contained = true;
	}
	else if (data->query_type == 1)
	{
		vector3f delta;
		vector3d_from_points3d(&data->center, center, &delta);
		real sum = radius + data->radius;
		if (!(sum * sum >= delta.k * delta.k + delta.i * delta.i + delta.j * delta.j))
			return false;
		double x = (double)data->center.x - center->x;
		double y = (double)data->center.y - center->y;
		double z = (double)data->center.z - center->z;
		*contained = sqrt(z * z + y * y + x * x) + radius <= data->radius;
	}
	if (result && list && (data->flags2a60 & 1))
	{
		bool found = list->find(index) != NONE;
		*second = found;
		*first = found;
	}
	return result;
}

struct s_132b80_frustum
{
	byte unknown00[0x74];
	plane3f plane;
	byte unknown84[0x1bc - 0x84];
};

void function_11bed0(s_location *location, point3f const *point);

// @retail 0x132b80
long function_132b80(s_bit_vector_pool *data, long mode, s_132b80_frustum const *frusta,
	short cluster, void *filter, point3f const *center, real radius, bool use_plane,
	real plane_offset, long count, dword flags)
{
	data->location.cluster_index = cluster;
	data->location.bsp_index = g_4686c4;
	data->location.leaf_index = NONE;
	data->mode = mode;
	data->flags2a60 = flags;
	data->frustum_count = (short)count;
	data->center = *center;
	data->radius = radius;
	data->plane_offset = plane_offset;
	if (use_plane)
	{
		data->flags2a60 |= 0x200;
		data->plane = frusta->plane;
		data->plane.d += plane_offset;
	}
	else
		data->flags2a60 &= ~0x200;
	void *destination = (byte *)data->context + 4;
	if (frusta != destination)
		memcpy(destination, frusta, count * sizeof(s_132b80_frustum));
	*(short *)data->context = (short)count;
	if (cluster == NONE)
		function_11bed0(&data->location, center);
	data->filter = filter;
	if (filter)
	{
		data->flags2a60 |= 1;
		for (long i = 0; i < 13; i++)
			data->counters[i] = 0;
	}
	if (data->mode == 0 && (char)data->flags2a60 < 0)
		data->query_type = 2;
	else if (count && (radius >= 3.0f || (data->flags2a60 & 0x100)))
		data->query_type = 0;
	else
		data->query_type = 1;
	return data->query_type;
}

/* takes enough dwords from the pool for a bit vector of this many bits */
// @retail 0x1332b0
dword *function_1332b0(long bit_count, s_bit_vector_pool *data)
{
	dword *result = NULL;
	if (bit_count > 0)
	{
		word used = data->pool_used;
		long count = (bit_count + 31) >> 5;
		result = data->pool;
		if (used + count < 0x200)
		{
			result = &data->pool[used];
			used += count;
			data->pool_used = used;
		}
	}
	return result;
}

// @retail 0x134300
long function_134300(s_bit_vector_pool *data, short bit)
{
	long index = data->entry_count++;
	if (index >= 0 && index < 0x200)
	{
		data->entries[index][3] = 0;
		data->entries[index][2] = 0;
		data->entries[index][1] = 0;
		data->entries[index][0] = 0;
		data->entries[(short)index][bit >> 5] |= 1 << (bit & 31);
		return index;
	}
	data->entry_count = 0x200;
	return 0;
}

// @retail 0x1348e0
void function_1348e0(s_bit_vector_pool *data)
{
	data->lists[0]->count = 0;
	data->lists[1]->count = 0;
	data->lists[2]->count = 0;
	data->lists[3]->count = 0;
	memset(data->flags, 0, sizeof(data->flags));
	memset(data->pool, 0, data->pool_used * sizeof(dword));
	data->pool_used = 0;
	data->entry_count = 0;
	for (long i = 0; i < 0x80; i++)
	{
		data->indices[i] = NONE;
	}
}

/* a bit field from seven flags */
// @retail 0x132b10
dword function_132b10(bool b, bool d, bool c, bool f, bool g, bool e, bool a)
{
	return (b ? 0x4000 : 0) | (c ? 0x2000 : 0) | (d ? 0x1000 : 0) | (e ? 0x800 : 0) | (f ? 0x400 : 0) | (g ? 0x200 : 0) | (a ? 0x8000 : 0);
}

/* a 16 bit real: sign, ten bits of mantissa, then five of exponent */
// @retail 0x135880
real function_135880(word value)
{
	union s_135881
	{
		dword field_0;
		real field_00;
		struct { dword field_0 : 13; dword field_d : 10; dword field_17 : 8; dword field_1f : 1; } field_000;
	};
	s_135881 local_1;
	local_1.field_0 = 0;
	local_1.field_000.field_d = value >> 5;
	local_1.field_000.field_1f = value >> 15;
	local_1.field_000.field_17 = (value & 0x1f) + 0x70;
	return local_1.field_00;
}
// @retail 0x134980
void function_134980(s_bit_vector_pool *data, s_bit_vector_pool_sizes const *sizes, void *context)
{
	data->context = context;
	data->sizes = *sizes;
	data->lists[0] = new c_entry_list(data->sizes.list_sizes[0]);
	data->lists[2] = new c_entry_list(data->sizes.list_sizes[2]);
	data->lists[3] = new c_entry_list(data->sizes.list_sizes[3]);
	data->lists[1] = NULL;
	if (data->sizes.list_sizes[1])
	{
		data->lists[1] = new c_entry_list(data->sizes.list_sizes[1]);
	}
	data->pool_used = 0;
	data->entry_count = 0;
	data->records = NULL;
	if (data->sizes.record_count)
	{
		data->records = (byte *)operator new(data->sizes.record_count * k_bit_vector_pool_record_size);
	}
}

// @retail 0x134b20
void function_134b20(s_bit_vector_pool *data)
{
	delete data->lists[0];
	data->lists[0] = NULL;
	delete data->lists[1];
	data->lists[1] = NULL;
	delete data->lists[2];
	data->lists[2] = NULL;
	delete data->lists[3];
	data->lists[3] = NULL;
	operator delete(data->records);
	data->records = NULL;
}

/* the two bit vector pools, each with a scratch buffer carved from one
   virtual allocation (ai.cpp borrows the same buffer) */
extern byte *g_510c44;
s_bit_vector_pool g_547f88;
s_bit_vector_pool g_54aa68;

// @retail 0x131ff0
void function_131ff0(void)
{
	s_bit_vector_pool_sizes sizes0;
	s_bit_vector_pool_sizes sizes1;
	byte *buffer;

	sizes0.unknown0 = 1;
	sizes0.list_sizes[0] = 0x80;
	sizes0.list_sizes[1] = 0x40;
	sizes0.list_sizes[2] = 0x100;
	sizes0.list_sizes[3] = 0x180;
	sizes0.record_count = 0;
	sizes1.unknown0 = 6;
	sizes1.list_sizes[0] = 0x40;
	sizes1.list_sizes[1] = 1;
	sizes1.list_sizes[2] = 0x80;
	sizes1.list_sizes[3] = 0x60;
	sizes1.record_count = 0x100;

	buffer = (byte *)VirtualAlloc(NULL, 0x452e8, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
	if (!buffer)
	{
		GetLastError();
	}
	g_510c44 = buffer;

	function_134980(&g_547f88, &sizes0, g_510c44);
	function_134980(&g_54aa68, &sizes1, g_510c44 + 0x22974);
}

// @retail 0x1320b0
void function_1320b0(void)
{
	function_134b20(&g_547f88);
	function_134b20(&g_54aa68);

	if (g_510c44)
	{
		if (!VirtualFree(g_510c44, 0, MEM_RELEASE))
		{
			GetLastError();
		}
		g_510c44 = NULL;
	}
}

struct s_134240_object
{
	long definition_index;
};

struct s_134240_object_header
{
	byte unknown00[8];
	s_134240_object *object;
};

/* moves the objects whose model has nodes to the front of the third list */
// @retail 0x134240
void function_134240(s_bit_vector_pool *data)
{
	long kept = 0;
	for (long i = 0; i < data->lists[2]->count; i++)
	{
		long object_index = data->lists[2]->longs_a[(short)i];
		if (object_index == NONE)
		{
			continue;
		}
		s_134240_object *object = ((s_134240_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		if (!object || object->definition_index == NONE)
		{
			continue;
		}
		byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
		if (!definition || *(long *)(definition + 0x38) == NONE)
		{
			continue;
		}
		byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
		if (!model || *(long *)(model + 4) == NONE)
		{
			continue;
		}
		byte *render_model = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
		if (*(long *)(render_model + 0x74) > 0)
		{
			if (i != kept)
			{
				data->lists[2]->swap((short)i, (short)kept);
			}
			kept++;
		}
	}
}

static inline real transition_cosine(real x)
{
	real t;
	if (0.0f > x)
	{
		t = 0.0f;
	}
	else if (x > 1.0f)
	{
		t = 1.0f;
	}
	else
	{
		t = x;
	}
	return (real)(0.5f - cos(t * k_pi) * 0.5f);
}

// @retail 0x134c50
real function_134c50(real x)
{
	return 0.0f > transition_cosine(x) ? 0.0f : (transition_cosine(x) > 1.0f ? 1.0f : transition_cosine(x));
}

/* which sides of the pool's plane a sphere touches (both when the plane is
   off or the sphere crosses it) */
// @retail 0x132fd0
void function_132fd0(s_bit_vector_pool const *data, point3f const *center, bool *behind, bool *in_front, real radius)
{
	if (data->flags2a60 & 0x200)
	{
		real distance = plane_distance_to_point(&data->plane, center);

		*in_front = false;
		*behind = false;
		if (0.0f > distance)
		{
			*behind = true;
		}
		if (distance > 0.0f)
		{
			*in_front = true;
		}
		if ((real)fabs(distance) <= radius)
		{
			*in_front = true;
			*behind = true;
		}
	}
	else
	{
		*in_front = true;
		*behind = true;
	}
}

long g_4b9ef0;
long g_4b9f8c;
real g_4b9ffc;
bool g_4ba004;

/* the render flags of an entry from its own flags and the pool's */
// @retail 0x1332f0
dword function_1332f0(s_bit_vector_pool const *data, word flags)
{
	union { word field_0; byte field_1[2]; } local_1;
	local_1.field_0 = flags;
	dword result = ((((local_1.field_0 >> 2) & 0x2c00) | (local_1.field_0 & 0x4000)) >> 9) | ((local_1.field_0 & 0x200) << 5);

	if (((char)data->flags2a60 >> 7) != 0)
	{
		result |= 1;
	}
	if (g_4b9ef0 == 3)
	{
		long tag_index = g_4b9f8c;

		if (tag_index != NONE && g_4b9ffc == 0.0f && !(local_1.field_1[1] & 4) && !(*g_4e3b44[tag_index & 0xffff].bytes & 2))
		{
			result |= 0x100;
		}
		if (!g_4ba004 || (local_1.field_1[1] & 4))
		{
			result |= 0x80;
		}
	}

	return result;
}

extern long g_4de2fc;
extern long g_4de300[0x800];
bool function_bec70(long object_index);

struct s_visibility_object
{
	long definition;
	union
	{
		dword flags;
		struct { dword low_flags : 31; dword flag31 : 1; };
	};
	byte unknown08[0x30 - 8];
	point3f center;
	real radius;
	byte unknown40[0x10];
	point3f alternate_center;
	real alternate_radius;
};
struct s_visibility_object_header
{
	byte unknown00[8];
	s_visibility_object *object;
};

// @retail 0x133050
void function_133050(s_bit_vector_pool *data, long object_index, short section, bool flag_a, bool flag_b)
{
	if (g_4de300[object_index & 0xffff] != g_4de2fc)
	{
		bool first = true;
		bool second = true;
		bool contained = true;
		s_visibility_object *object = ((s_visibility_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		c_entry_list *filter = (c_entry_list *)data->filter;
		if (filter)
			filter = *(c_entry_list **)((byte *)filter + 0x14);
		point3f const *center;
		real radius;
		if (!data->mode)
		{
			center = &object->center;
			radius = object->radius;
		}
		else
		{
			center = &object->alternate_center;
			radius = object->alternate_radius;
		}
		bool visible = function_132e60(data, center, radius, filter, object_index, section, flag_b, &contained, &first, &second) || ((char)data->flags2a60 >> 7) != 0;
		if (data->mode && second)
			second = !(object->flags & 0x10000);
		if (visible)
		{
			bool in_front;
			bool behind;
			function_132fd0(data, center, &behind, &in_front, radius);
			dword flags = function_132b10(in_front, first, second, flag_a, flag_b, contained, behind);
			long entry = NONE;
			if (!(flags & 0x800) && TEST_FIELD_BIT(object->flag31))
				entry = function_134300(data, section);
			if (data->lists[2]->add(object_index, (short)flags, 0, (short)entry))
				function_bec70(object_index);
		}
	}
	else
	{
		c_entry_list *list = data->lists[2];
		short index = NONE;
		bool found = false;
		for (long i = 0; i < list->count && !found; i++)
		{
			found = list->longs_a[(short)i] == object_index;
			index = (short)i;
		}
		if (!(list->shorts_b[index] & 0x800))
		{
			short entry = list->shorts_d[index];
			if (entry != NONE)
				data->entries[entry][section >> 5] |= 1 << (section & 31);
		}
		if (flag_a)
		{
			list = data->lists[2];
			for (long i = 0; i < list->count; i++)
			{
				if (list->longs_a[(short)i] == object_index)
				{
					short *flags = &list->shorts_b[(short)i];
					*flags |= 0x400;
					break;
				}
			}
		}
		if (flag_b)
		{
			list = data->lists[2];
			for (long i = 0; i < list->count; i++)
			{
				if (list->longs_a[(short)i] == object_index)
				{
					short *flags = &list->shorts_b[(short)i];
					*flags |= 0x200;
					break;
				}
			}
		}
	}
}

extern long g_4e7c1c;
long g_4e7c20[1024];
bool function_16e210(long cluster_index, long value);

struct s_visible_cluster
{
	byte unknown00[0x24];
	word surface_count;
	byte unknown26[0x54 - 0x26];
	real bounds[3][2];
	byte unknown6c[0x98 - 0x6c];
	long instance_count;
	word *instances;
	byte unknowna0[0xb0 - 0xa0];
};
struct s_visible_instance
{
	byte unknown00[0x34];
	short definition;
	byte unknown36[6];
	point3f center;
	real radius;
	byte unknown4c[0x58 - 0x4c];
};
struct s_visibility_bsp
{
	byte unknown00[0xa0];
	s_visible_cluster *clusters;
	byte unknowna4[0x13c - 0xa4];
	byte *definitions;
	long instance_count;
	s_visible_instance *instances;
};

extern real g_4b9e20, g_4b9e2c, g_4b9e38, g_4b9e44, g_4b9ed0;
short function_2cbf0(long arg_1, byte arg_2, long arg_3, dword arg_4,
    long arg_5, dword arg_6, long arg_7, byte *arg_8, short arg_9, byte arg_10,
    void *arg_11, long arg_12, real arg_13, long arg_14, long arg_15,
    bool arg_16, bool arg_17, bool arg_18, real arg_19, real arg_20,
    real arg_21, byte arg_22, point3f const *arg_23, real arg_24);

// @retail 0x133390
void function_133390(s_bit_vector_pool const *arg_1)
{
	c_entry_list *local_1 = arg_1->lists[0];
	for (long local_2 = 0; local_2 < local_1->count; local_2++)
	{
		dword local_3 = function_1332f0(arg_1, local_1->shorts_b[(short)local_2]);
		long local_4 = local_1->longs_a[(short)local_2];
		s_visible_cluster const *local_5 = &((s_visibility_bsp *)g_4e0348)->clusters[local_4];
		point3f local_6;
		local_6.x = (local_5->bounds[0][0] + local_5->bounds[0][1]) * 0.5f;
		local_6.y = (local_5->bounds[1][0] + local_5->bounds[1][1]) * 0.5f;
		local_6.z = (local_5->bounds[2][0] + local_5->bounds[2][1]) * 0.5f;
		volatile real local_7 = local_5->bounds[0][1] - local_6.x;
		volatile real local_8 = local_5->bounds[1][1] - local_6.y;
		volatile real local_9 = local_5->bounds[2][1] - local_6.z;
		real local_10 = (real)sqrt((double)local_7 * local_7 + (double)local_8 * local_8 + (double)local_9 * local_9);
		real local_11 = local_6.z * g_4b9e38 + local_6.x * g_4b9e20 + local_6.y * g_4b9e2c + g_4b9e44;
		if (!(local_11 >= 0.0f))
			local_11 = 0.0f - local_11;
		if (!(local_11 > 0.1f))
			local_11 = 0.1f;
		real local_12 = (g_4b9ed0 / local_11) * local_10 * 2.0f;
		short local_13 = ((signed char const *)arg_1->indices)[(short)local_4];
		function_2cbf0(local_4, 0, NONE, local_3 | 0x40, NONE, NONE, 0, NULL,
			local_13, 0xff, NULL, NONE, local_12, NONE, NONE, false, false,
			false, 0.0f, 0.0f, 0.0f, 0, g_468788, 0.0f);
		local_1 = arg_1->lists[0];
	}
}

// @retail 0x133520
void __stdcall function_133520(s_bit_vector_pool *arg_1)
{
	s_visibility_bsp *local_1 = (s_visibility_bsp *)g_4e0348;
	for (long local_2 = 0; local_2 < (*(c_entry_list *volatile const *)&arg_1->lists[3])->count; local_2++)
	{
		c_entry_list *local_3 = arg_1->lists[3];
		long local_4 = (short)local_3->longs_a[(short)local_2];
		s_visible_instance *local_5 = &local_1->instances[(short)local_4];
		dword local_6 = function_1332f0(arg_1, local_3->shorts_b[(short)local_2]);
		point3f const *local_12 = &local_5->center;
		real local_7 = local_12->y * g_4b9e2c + local_12->z * g_4b9e38 + local_12->x * g_4b9e20 + g_4b9e44;
		if (!(local_7 >= 0.0f))
			local_7 = 0.0f - local_7;
		if (!(local_7 > 0.1f))
			local_7 = 0.1f;
		short local_8 = (*(c_entry_list *volatile const *)&arg_1->lists[3])->shorts_d[(short)local_2];
		real local_9 = (local_5->radius / local_7) * g_4b9ed0 * 2.0f;
		if (local_5->radius < 0.13f)
			local_9 *= 1.75f;
		real local_10 = (local_9 - 25.0f) * (1.0f / 7.0f);
		local_10 = 0.0f > local_10 ? 0.0f : (local_10 > 1.0f ? 1.0f : local_10);
		byte local_11 = (byte)(long)(local_10 * 255.0f);
		if (!(arg_1->lists[3]->shorts_b[(short)local_2] & 0x800))
			local_6 |= 0x40;
		else
			local_6 |= 1;
		if (local_11 > 0)
		{
			function_2cbf0(0, 0, local_4, local_6 | 0x1008, NONE, 0xffffffff,
				0, NULL, local_8, local_11, local_5, NONE, local_9, NONE, NONE,
				false, false, false, 0.0f, 0.0f, 0.0f, 0, local_12, local_5->radius);
		}
		else
			arg_1->lists[3]->shorts_b[(short)local_2] |= 1;
	}
}

__forceinline bool visibility_append(c_entry_list *list, long index, short flags, short entry)
{
	if ((word)list->count < list->maximum_count - 1)
	{
		list->longs_a[(word)list->count] = index;
		list->shorts_b[(word)list->count] = flags;
		list->longs_c[(word)list->count] = 0;
		list->shorts_d[(word)list->count] = entry;
		list->count++;
		return true;
	}
	return false;
}

// @retail 0x134390
void __stdcall function_134390(s_bit_vector_pool *data)
{
	if (data->flags2a60 & 0x20)
		return;
	for (long i = 0; (short)i < ((s_sort_context *)data->context)->count; i++)
	{
		s_sort_entry *item = &((s_sort_context *)data->context)->entries[(short)i];
		s_visibility_bsp *bsp = (s_visibility_bsp *)g_4e0348;
		s_visible_cluster *cluster = &bsp->clusters[item->value];
		data->flags[item->value >> 5] |= 1 << (item->value & 31);
		((byte *)data->indices)[item->value] = (byte)i;
		if (cluster->surface_count > 0)
		{
			long cluster_index = item->value;
			s_visible_cluster *bounds = &bsp->clusters[cluster_index];
			point3f center;
			center.x = (bounds->bounds[0][0] + bounds->bounds[0][1]) * 0.5f;
			center.y = (bounds->bounds[1][0] + bounds->bounds[1][1]) * 0.5f;
			center.z = (bounds->bounds[2][0] + bounds->bounds[2][1]) * 0.5f;
			real x = bounds->bounds[0][1] - center.x;
			real y = bounds->bounds[1][1] - center.y;
			real z = bounds->bounds[2][1] - center.z;
			double radius = sqrt((double)z * z + (double)y * y + (double)x * x);
			bool behind = true;
			bool in_front = true;
			if (data->flags2a60 & 0x200)
			{
				real distance = plane_distance_to_point(&data->plane, &center);
				behind = distance < 0.0f;
				in_front = distance > 0.0f;
				if (fabs(distance) <= radius)
					behind = in_front = true;
			}
			if (data->filter && (data->flags2a60 & 1))
			{
				c_entry_list *list = *(c_entry_list **)((byte *)data->filter + 0xc);
				short index = NONE;
				for (long j = 0; j < (word)list->count; j++)
				{
					if (list->longs_a[j] == cluster_index)
					{
						index = (short)j;
						break;
					}
				}
				if (index == NONE)
					continue;
			}
			short flags = (function_16e210(cluster_index, g_4b9f8c) ? 0x400 : 0) |
				(behind ? 0x8000 : 0) | 0x3000 | (in_front ? 0x4000 : 0);
			visibility_append(data->lists[0], cluster_index, flags, NONE);
		}
		for (long j = 0; j < cluster->instance_count; j++)
		{
			word instance_index = cluster->instances[j];
			s_visibility_bsp *current_bsp = (s_visibility_bsp *)g_4e0348;
			s_visible_instance *instance = &current_bsp->instances[instance_index];
			byte *definition = current_bsp->definitions + instance->definition * 0xc8;
			if (g_4e7c20[(short)instance_index] != g_4e7c1c)
			{
				c_entry_list *filter = data->filter ? *(c_entry_list **)((byte *)data->filter + 0x18) : NULL;
				if (*(word *)(definition + 0x24) > 0)
				{
					bool contained, first, second;
					if (function_132e60(data, &instance->center, instance->radius, filter, instance_index, (short)i, false, &contained, &first, &second) || (char)data->flags2a60 < 0)
					{
						bool behind = true;
						bool in_front = true;
						if (data->flags2a60 & 0x200)
						{
							real distance = plane_distance_to_point(&data->plane, &instance->center);
							behind = distance < 0.0f;
							in_front = distance > 0.0f;
							if (fabs(distance) <= instance->radius)
								behind = in_front = true;
						}
						short flags = (function_16e210(item->value, g_4b9f8c) ? 0x400 : 0) |
							(in_front ? 0x4000 : 0) | (second ? 0x2000 : 0) | (first ? 0x1000 : 0) |
							(contained ? 0x800 : 0) | (behind ? 0x8000 : 0);
						long entry = NONE;
						if (!(flags & 0x800))
							entry = function_134300(data, (short)i);
						if (visibility_append(data->lists[3], instance_index, flags, (short)entry))
							g_4e7c20[(short)instance_index] = g_4e7c1c;
					}
				}
			}
			else
			{
				c_entry_list *list = data->lists[3];
				short index = NONE;
				bool found = false;
				for (long k = 0; k < list->count && !found; k++)
				{
					found = list->longs_a[(short)k] == instance_index;
					index = (short)k;
				}
				if (!(list->shorts_b[index] & 0x800))
					data->entries[list->shorts_d[index]][(short)i >> 5] |= 1 << ((short)i & 31);
				if (function_16e210(item->value, g_4b9f8c))
					list->shorts_b[index] |= 0x400;
			}
		}
	}
}

#include <xmmintrin.h>
extern void *g_4de2e0;
extern void *g_4de2e4;
extern void *g_4de2d4;
extern void *g_4de2d8;
extern void *g_4e0310;
extern void *g_4e0314;
extern bool g_4b9ee9;
extern long g_4b9eec;
extern long g_4b9ed4;
long function_c3950(long object_index, long *indices, long maximum);

struct s_visibility_link
{
	long salt;
	long object_index;
	long next;
};
__forceinline long visibility_next(s_record_pool *pool, long *next)
{
	long result = NONE;
	if (*next != NONE)
	{
		s_visibility_link *link = (s_visibility_link *)(pool->data + (*next & 0xffff) * pool->size);
		*next = link->next;
		if (*next != NONE)
			_mm_prefetch((char const *)(pool->data + (*next & 0xffff) * pool->size), _MM_HINT_T0);
		result = link->object_index;
	}
	return result;
}

// @retail 0x132220
void __stdcall function_132220(s_bit_vector_pool *data)
{
	s_sort_context *context = (s_sort_context *)data->context;
	short extra_cluster = NONE;
	for (long i = 0; i < context->count; i++)
	{
		short cluster = context->entries[i].value;
		s_record_pool *pool = (s_record_pool *)g_4de2e4;
		bool selected = function_16e210(cluster, g_4b9f8c);
		long next;
		long index;
		if (!(data->flags2a60 & 0x40))
		{
			next = ((long *)g_4de2e0)[cluster];
			for (index = visibility_next(pool, &next); index != NONE; index = visibility_next(pool, &next))
			{
				if (!(data->flags2a60 & 0x400) || *((byte *)((s_visibility_object_header *)g_4e0300->data)[index & 0xffff].object + 0xaa) == 7)
				{
					function_133050(data, index, (short)i, selected, false);
					pool = (s_record_pool *)g_4de2e4;
				}
			}
			next = ((long *)g_4de2d4)[context->entries[i].value];
			for (index = visibility_next((s_record_pool *)g_4de2d8, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2d8, &next))
				function_133050(data, index, (short)i, selected, false);
		}
		if (data->mode == 0 && !(data->flags2a60 & 0x10))
		{
			next = ((long *)g_4e0310)[context->entries[i].value];
			for (index = visibility_next((s_record_pool *)g_4e0314, &next); index != NONE; index = visibility_next((s_record_pool *)g_4e0314, &next))
				function_132a30(data, index, i, false);
		}
		if (extra_cluster == NONE && data->mode == 0 && g_4b9ee9 && g_4b9eec != NONE && g_4686c4 != NONE)
		{
			byte *bsp = (byte *)g_4e0348;
			if (*(long *)(bsp + 0xac) > 0)
				extra_cluster = (*(short **)(bsp + 0xb0))[g_4b9eec];
		}
		context = (s_sort_context *)data->context;
	}
	if (data->mode == 0 && !(data->flags2a60 & 0x10) && data->location.cluster_index != NONE)
	{
		long indices[4];
		long count = function_c3950(g_4b9ed4, indices, 4);
		for (long i = 0; i < count; i++)
		{
			long index = indices[i];
			s_132a30_light *light = &((s_132a30_light *)g_4e030c->data)[index & 0xffff];
			if (*(short *)((byte *)light + 0x48) == NONE && function_c3140(index) && light->stamp != g_4e0308)
			{
				if (data->lists[1]->add(index, 0, 0, NONE))
					function_c3110(index);
			}
		}
	}
	if (extra_cluster != NONE && !(data->flags2a60 & 0x40))
	{
		long next = ((long *)g_4de2e0)[extra_cluster];
		long index;
		for (index = visibility_next((s_record_pool *)g_4de2e4, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2e4, &next))
			function_133050(data, index, NONE, false, true);
		next = ((long *)g_4de2d4)[extra_cluster];
		for (index = visibility_next((s_record_pool *)g_4de2d8, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2d8, &next))
			function_133050(data, index, NONE, false, true);
	}
}

struct s_132780
{
	long field_0;
	long field_4[6];
	long field_1c[6];
	byte *field_34;
};

struct s_view;
struct s_frustum_1648d0;
struct s_clip_1648d0;
struct s_projection_1648d0;
bool function_1648d0(s_frustum_1648d0 const *source, s_view *view, s_clip_1648d0 const *clip, s_projection_1648d0 *result, bool make_hull);

// @retail 0x132780
bool function_132780(s_sort_context const *arg_1, s_sort_context const *arg_2, long arg_3, long arg_4, long arg_5, long arg_6, long arg_7, s_132780 *arg_8)
{
	(void)&arg_1;
	(void)&arg_2;
	(void)&arg_3;
	(void)&arg_4;
	(void)&arg_5;
	s_sort_entry const *local_1 = &arg_1->entries[arg_1->order[arg_6]];
	s_sort_entry const *local_2 = &arg_2->entries[arg_2->order[arg_3]];
	bool local_3 = false;
	for (long local_4 = 0; local_4 < ((short const *)local_2)[arg_7 + 1]; local_4++)
	{
		byte const *local_5 = (byte const *)arg_2 + 0x1974 + (((short const *)local_2)[arg_7 + 7] + local_4) * 0x108;
		for (long local_6 = 0; local_6 < ((short const *)local_1)[arg_4 + 1]; local_6++)
		{
			byte const *local_7 = (byte const *)arg_1 + 0x1974 + (((short const *)local_1)[7] + local_6) * 0x108;
			if (arg_8->field_0 >= arg_5)
				return false;
			byte *local_8 = arg_8->field_34 + arg_8->field_0 * 0x9c;
			if (function_1648d0((s_frustum_1648d0 const *)local_7, (s_view *)((byte const *)arg_2 + 4 + *(long const *)local_5 * 0x1bc), (s_clip_1648d0 const *)local_5, (s_projection_1648d0 *)local_8, false))
			{
				long local_9 = arg_8->field_0++;
				*(long *)(local_8 + 0x94) = ((short const *)local_2)[arg_7 + 7] + local_4;
				*(long *)(local_8 + 0x98) = arg_7;
				if (arg_8->field_1c[arg_7] == NONE)
					arg_8->field_1c[arg_7] = local_9;
				arg_8->field_4[arg_7]++;
				local_3 = true;
			}
		}
	}
	return local_3;
}

// @retail 0x132900
void function_132900(s_bit_vector_pool *arg_1, s_sort_context const *arg_2, s_sort_context const *arg_3)
{
	(void)&arg_1;
	(void)&arg_3;
	long local_1 = 0;
	long local_2 = 0;
	while (local_1 < arg_2->count && local_2 < arg_3->count)
	{
		s_sort_entry const *local_3 = &arg_2->entries[arg_2->order[local_1]];
		s_sort_entry const *local_4 = &arg_3->entries[arg_3->order[local_2]];
		short local_6 = *(volatile short const *)&local_3->value;
		if (local_4->value > local_6)
			local_1++;
		else if (local_4->value < local_6)
			local_2++;
		else
		{
			if (*(short const *)local_4->unknown02 > 0)
			{
				for (long local_5 = 0; local_5 < *(short const *)arg_2; local_5++)
				{
					if (((short const *)local_3->unknown02)[local_5] > 0)
						function_132780(arg_3, arg_2, local_1, 0, *(short *)((byte *)arg_1 + 0x2ade), local_2, local_5, (s_132780 *)((byte *)arg_1 + 0x2a98));
				}
			}
			local_1++;
			local_2++;
		}
	}
}
