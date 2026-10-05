// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <xmmintrin.h>

/* The collector base owns an eight-byte allocation. Its derived collector
   separately owns the array of 0x30-byte entries. */
class c_query_collector
{
public:
	virtual ~c_query_collector() {}
	virtual void collect() {}
	static void operator delete(void *block)
	{
		g_480118->allocate((long)block, 8, 0x1a);
	}
	real maximum;
};

// @retail 0x1827d0 deleting c_query_collector

struct s_query_collector_entries
{
	void *data;
	long count;
	dword capacity_and_flags;
	~s_query_collector_entries()
	{
		if (!(capacity_and_flags & 0x80000000))
			g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * 0x30, 0x12);
	}
};

class c_query_array_collector : public c_query_collector
{
public:
	virtual ~c_query_array_collector();
	virtual void collect() {}
	byte unknown08[8];
	s_query_collector_entries entries;
};

// @retail 0x181a10 deleting
c_query_array_collector::~c_query_array_collector()
{
}

struct s_node
{
	byte unknown00[0xc];
	s_node *next;
	s_node *get_last();
};

class hkEntity;
long havok_entity_component_index_get(hkEntity const *entity);

struct s_query_node : s_node
{
	byte unknown10[8];
	long type;
	long unknown1c;
	hkEntity *entity;
};

struct s_query_hit
{
	vector3f normal;
	byte unknown0c[8];
	real fraction;
};

struct s_query_result
{
	point3f origin;
	vector3f direction;
	real fraction;
	vector3f normal;
};

class c_query_callback
{
public:
	virtual void query(s_node *node, s_query_hit const *hit) {}
	virtual ~c_query_callback() {}
	real fraction;
};

// @retail 0x181a60 deleting c_query_callback

class c_nearest_query_callback : public c_query_callback
{
public:
	virtual void query(s_node *node, s_query_hit const *hit);
	s_query_result *result;
	long excluded_component;
	bool found;
};

// @retail 0x1829b0
void c_nearest_query_callback::query(s_node *node, s_query_hit const *hit)
{
	long excluded;
	if (hit->fraction < 1.0f)
	{
		real distance = hit->fraction;
		s_query_result *query_result = result;
		if (query_result->fraction > distance)
		{
			hkEntity *entity = 0;
			excluded = excluded_component;
			if (excluded != NONE)
			{
				s_query_node *last = (s_query_node *)node->get_last();
				if (last->type == 1)
					entity = last->entity;
			}
			if (!entity || havok_entity_component_index_get(entity) != excluded)
			{
				vector3f normal;
				normal.i = hit->normal.i;
				normal.j = hit->normal.j;
				normal.k = hit->normal.k;
				if (dot3f(&normal, &query_result->direction) < 0.0f)
				{
					found = true;
					query_result->fraction = distance;
					result->normal = normal;
					fraction = distance;
				}
			}
		}
	}
}

struct s_query_batch_result
{
	bool found;
	byte unknown01[3];
	real fraction;
	short material;
	byte unknown0a[0x24 - 0xa];
	long index;
	long other_index;
	bool flag2c;
	byte unknown2d[0x3c - 0x2d];
};

class c_query_batch_collector : public c_query_collector
{
public:
	virtual void collect() {}
	s_query_batch_result *results;
	real scale;
	long count;
	long component_index;
};

class c_query_shape
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual void slot5() {}
	virtual void slot6() {}
	virtual void slot7() {}
	virtual void slot8() {}
	virtual void slot9() {}
	virtual void slot10() {}
	virtual void query(c_query_collector *collector) {}
};

struct s_query_component
{
	long unknown00;
	struct
	{
		dword unknown : 5;
		dword has_shape : 1;
		dword other : 26;
	} flags;
	byte unknown08[0x98 - 8];
	c_query_shape *shape;
	long unknown9c;
};

extern short g_54e898;

// @retail 0x1821d0
void __stdcall function_1821d0(long component_index, real scale,
	s_query_batch_result *results, long count)
{
	s_query_component *component = &((s_query_component *)g_51e9b8->data)[component_index & 0xffff];
	c_query_shape *shape = TEST_FIELD_BIT(component->flags.has_shape) ? component->shape : 0;
	for (long i = 0; i < count; i++)
	{
		results[i].found = false;
		results[i].fraction = FLT_MAX;
		results[i].index = NONE;
		results[i].other_index = NONE;
		results[i].material = g_54e898;
		results[i].flag2c = false;
	}
	if (shape)
	{
		c_query_batch_collector collector;
		collector.maximum = FLT_MAX;
		collector.results = results;
		collector.scale = scale;
		collector.count = count;
		collector.component_index = component_index;
		_control87(0x9001f, 0x8001f);
		_mm_setcsr(_mm_getcsr() | 0x1f80);
		shape->query(&collector);
		_mm_setcsr(_mm_getcsr() & ~0x3f);
		_clearfp();
		_control87(0x9001f, 0xfffff);
	}
}

struct s_bounded_ray
{
	__m128 from;
	__m128 to;
	bool flag;
};

class c_bounded_query_world
{
public:
	void cast_ray(s_bounded_ray const *ray, c_query_callback *callback);
	byte unknown00[0x60];
	__m128 lower;
	__m128 upper;
};

static inline real query_coordinate_pin(real value, real lower, real upper)
{
	return lower > value ? lower : (value > upper ? upper : value);
}

struct s_vehicle_ray;

// @retail 0x182800
bool function_182800(s_vehicle_ray *ray_data, long excluded_component, void *world_data)
{
	s_query_result *result = (s_query_result *)ray_data;
	c_bounded_query_world *world = (c_bounded_query_world *)world_data;
	point3f end;
	end.x = result->origin.x + result->direction.i;
	end.y = result->origin.y + result->direction.j;
	end.z = result->origin.z + result->direction.k;
	c_nearest_query_callback collector;
	collector.fraction = 1.0f;
	collector.result = result;
	collector.excluded_component = excluded_component;
	collector.found = false;
	result->fraction = FLT_MAX;
	s_bounded_ray ray;
	ray.flag = false;
	ray.from.m128_f32[3] = 0.0f;
	ray.to.m128_f32[3] = 0.0f;
	for (long i = 0; i < 3; i++)
	{
		ray.from.m128_f32[i] = query_coordinate_pin(result->origin.n[i], world->lower.m128_f32[i], world->upper.m128_f32[i]);
		ray.to.m128_f32[i] = query_coordinate_pin(end.n[i], world->lower.m128_f32[i], world->upper.m128_f32[i]);
	}
	_control87(0x9001f, 0x8001f);
	_mm_setcsr(_mm_getcsr() | 0x1f80);
	world->cast_ray(&ray, &collector);
	_mm_setcsr(_mm_getcsr() & ~0x3f);
	_clearfp();
	_control87(0x9001f, 0xfffff);
	return collector.found;
}

class c_distance_query_shape
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual long shape_kind() { return 0; }
};

struct s_distance_query_body
{
	byte field_0[0xc];
	c_distance_query_shape *shape;
};

struct s_distance_query_entry
{
	byte field_0[0x40];
	s_distance_query_body *body;
	byte field_44[0x60 - 0x44];
};

struct s_distance_query_component
{
	long field_0;
	struct { dword field_4_0 : 5; dword field_4_5 : 1; } flags;
	byte field_8[0x70 - 8];
	s_distance_query_entry *bodies;
	long count;
	byte field_78[0xa0 - 0x78];
};

struct s_distance_query_context;
typedef void (__cdecl *t_distance_query)(void const *, void const *, s_distance_query_context *, c_query_collector *);
struct s_distance_query_dispatch
{
	byte field_0[0x218c];
	t_distance_query query[32][32];
};

struct __declspec(align(16)) s_distance_query_context
{
	s_distance_query_dispatch *dispatch;
	long field_4;
	real maximum;
	long field_c;
};

struct s_distance_query_world
{
	byte field_0[0xcc];
	s_distance_query_context *context;
};

struct s_distance_query_contact
{
	__m128 position_distance;
	__m128 normal;
	__m128 field_20;
};

class hkWorld;
extern hkWorld *g_51e9a4;
void voice_fpu_enter();

// @retail 0x183670
bool __stdcall function_183670(long component_a, long component_b, point3f *a, point3f *b, real *distance)
{
	bool result = false;
	if (component_a != NONE && component_b != NONE)
	{
		s_distance_query_component *first = &((s_distance_query_component *)g_51e9b8->data)[component_a & 0xffff];
		s_distance_query_component *second = &((s_distance_query_component *)g_51e9b8->data)[component_b & 0xffff];
		if (TEST_FIELD_BIT(first->flags.field_4_5) && TEST_FIELD_BIT(second->flags.field_4_5))
		{
			s_distance_query_context context = *((s_distance_query_world *)g_51e9a4)->context;
			voice_fpu_enter();
			*distance = FLT_MAX;
			context.maximum = FLT_MAX;
			for (long i = 0; i < first->count; i++)
			{
				for (long j = 0; j < second->count; j++)
				{
					__declspec(align(16)) c_query_array_collector collector;
					s_distance_query_contact buffer[8];
					collector.maximum = FLT_MAX;
					collector.entries.data = buffer;
					collector.entries.count = 0;
					collector.entries.capacity_and_flags = 0x80000008;
					s_distance_query_body *body_a = first->bodies[i].body;
					s_distance_query_body *body_b = second->bodies[j].body;
					c_distance_query_shape *shape_a = body_a->shape;
					c_distance_query_shape *shape_b = body_b->shape;
					long type_b = shape_b->shape_kind();
					long type_a = shape_a->shape_kind();
					context.dispatch->query[type_a][type_b](&body_a->shape, &body_b->shape, &context, &collector);
					s_distance_query_contact *contacts = (s_distance_query_contact *)collector.entries.data;
					for (long k = 0; k < collector.entries.count; k++)
					{
						if (*distance > contacts[k].position_distance.m128_f32[3])
						{
							*distance = contacts[k].position_distance.m128_f32[3];
							real px = contacts[k].position_distance.m128_f32[0];
							real py = contacts[k].position_distance.m128_f32[1];
							real pz = contacts[k].position_distance.m128_f32[2];
							b->x = px;
							b->y = py;
							b->z = pz;
							real scale = *distance;
							real x = contacts[k].normal.m128_f32[0];
							real y = contacts[k].normal.m128_f32[1];
							real z = contacts[k].normal.m128_f32[2];
							a->x = x * scale + b->x;
							a->y = y * scale + b->y;
							a->z = z * scale + b->z;
							result = true;
						}
					}
				}
			}
			_mm_setcsr(_mm_getcsr() & ~0x3f);
			_clearfp();
			_control87(0x9001f, 0xfffff);
		}
	}
	return result;
}
