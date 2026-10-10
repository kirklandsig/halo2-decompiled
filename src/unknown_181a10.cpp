// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <xmmintrin.h>

/* The collector base owns an eight-byte allocation. Its derived collector
   separately owns the array of 0x30-byte entries. */
struct s_batch_contact;

class c_query_collector
{
public:
	virtual ~c_query_collector() {}
	virtual void collect(s_batch_contact const *) {}
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
	virtual void collect(s_batch_contact const *) {}
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
	virtual void collect(s_batch_contact const *contact);
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
	c_query_shape *shape = NULL;
	if (TEST_FIELD_BIT(component->flags.has_shape))
		shape = component->shape;
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

__forceinline real function_182801(real arg_0, real const *arg_1, real const *arg_2)
{
    return *arg_1 > arg_0 ? *arg_1 : (arg_0 > *arg_2 ? *arg_2 : arg_0);
}

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
		ray.from.m128_f32[i] = function_182801(result->origin.n[i], &world->lower.m128_f32[i], &world->upper.m128_f32[i]);
		ray.to.m128_f32[i] = function_182801(end.n[i], &world->lower.m128_f32[i], &world->upper.m128_f32[i]);
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

__forceinline void function_183672(point3f *arg_0, point3f const *arg_1, __m128 const *arg_2, real arg_3)
{
    vector3f local_0;
    local_0.i = arg_2->m128_f32[0];
    local_0.j = arg_2->m128_f32[1];
    local_0.k = arg_2->m128_f32[2];
    arg_0->x = local_0.i * arg_3 + arg_1->x;
    arg_0->y = local_0.j * arg_3 + arg_1->y;
    arg_0->z = local_0.k * arg_3 + arg_1->z;
}

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
					*(dword volatile *)&collector.entries.capacity_and_flags = 0x80000008;
					collector.maximum = FLT_MAX;
					collector.entries.data = buffer;
					collector.entries.count = 0;
					s_distance_query_body *body_a = first->bodies[i].body;
					s_distance_query_body *body_b = second->bodies[j].body;
					c_distance_query_shape **local_0 = &body_a->shape;
					c_distance_query_shape **local_1 = &body_b->shape;
					c_distance_query_shape *body_a_shape = *local_0;
					c_distance_query_shape *body_b_shape = *local_1;
					long type_b = body_b_shape->shape_kind();
					long type_a = body_a_shape->shape_kind();
					context.dispatch->query[type_a][type_b](local_0, local_1, &context, &collector);
					s_distance_query_contact *contacts = (s_distance_query_contact *)collector.entries.data;
					for (long k = 0; k < collector.entries.count; k++)
					{
						if (*distance > contacts[k].position_distance.m128_f32[3])
						{
							*distance = contacts[k].position_distance.m128_f32[3];
							real px = contacts[k].position_distance.m128_f32[0];
							real pz = contacts[k].position_distance.m128_f32[2];
							real py = contacts[k].position_distance.m128_f32[1];
							b->x = px;
							b->z = pz;
							b->y = py;
							function_183672(a, b, &contacts[k].normal, *distance);
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

class c_material_shape;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
struct s_lookup_source;
struct s_bsp3d;
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
void function_1ee410(s_lookup_source const *source, short *result);
void function_182b90(c_material_shape *shape, hkEntity const *entity, real *friction, real *restitution, short *material);
long havok_entity_property_2002_get(hkEntity const *entity);
bool function_181db0(long index, vector3f *result);

class c_batch_contact_shape
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual long shape_kind() { return 0; }
	long field_4;
	dword data;
	real radius;
};
struct s_batch_contact_node
{
	c_batch_contact_shape *shape;
	long key;
	byte *transform;
	s_batch_contact_node *next;
	byte field_10[8];
	long type;
	long field_1c;
	hkEntity *entity;
};
struct s_batch_contact
{
	point3f position;
	real distance;
	vector3f normal;
	real field_1c;
	s_batch_contact_node *first;
	s_batch_contact_node *second;
};
struct s_batch_entity_property
{
	long key;
	long value;
};
struct s_batch_entity_view
{
	byte field_0[0x30];
	s_batch_entity_property *properties;
	long count;
};

PRIVATE __forceinline s_batch_contact_node *batch_contact_root(s_batch_contact_node *node)
{
	while (node->next) node = node->next;
	return node;
}
PRIVATE __forceinline long batch_contact_component(hkEntity *entity)
{
	s_batch_entity_view *view = (s_batch_entity_view *)entity;
	for (long i = 0; i < view->count; ++i)
		if (view->properties[i].key == 0x2001) return view->properties[i].value;
	return 0;
}
PRIVATE __forceinline bool batch_shape_pointer(dword data)
{
	long value = (long)data;
	long limited = value < 1 ? 1 : (value > 16 ? 16 : value);
	return limited != value;
}

// @retail 0x1822f0
void c_query_batch_collector::collect(s_batch_contact const *contact)
{
	s_batch_contact_node *first = contact->first;
	s_batch_contact_node *second = contact->second;
	s_batch_contact_node *root = batch_contact_root(second);
	if (root->type != 1 || !root->entity) return;
	long component = batch_contact_component(root->entity);
	batch_contact_root(second);
	if (component != NONE && second->shape->shape_kind() != 24 && second->shape->data &&
		batch_shape_pointer(second->shape->data) && ((byte *)second->shape->data)[0x1e] != 0xff) return;
	if (first->shape->shape_kind() != 4) return;
	if (component_index != NONE && component_index == component) return;
	long key = first->key;
	if (first->next && first->next->shape && first->next->shape->shape_kind() == 9 && !batch_shape_pointer(second->shape->data))
		key += first->next->shape->data * 8 - 8;
	long limited = key < 0 ? 0 : (key > count - 1 ? count - 1 : key);
	if (limited != key) return;
	point3f origin = *(point3f *)(first->transform + 0x50);
	point3f position = contact->position;
	real distance = contact->distance + first->shape->radius;
	if (!(results[key].fraction > distance)) return;
	bool valid = true;
	long index = NONE, other_index = NONE;
	if (second->shape->shape_kind() == 24)
	{
		c_batch_contact_shape *shape = second->shape;
		dword data = shape->data;
		if (data)
		{
			long entry = data & 0xffff;
			long surface = (data >> 16) & 0x1fff;
			plane3f plane;
			real side;
			if ((data >> 29) == 1)
			{
				byte *record = *(byte **)((byte *)g_4e0340 + 0x2c) + entry * 8;
				bsp3d_get_plane((s_bsp3d *)g_4e0340, *(short *)record, &plane);
				if (record[4] & 0x20)
					results[key].flag2c = function_181db0(*(short *)(record + 6), (vector3f *)((byte *)&results[key] + 0x30));
				side = plane.n.j * origin.y + plane.n.k * origin.z + plane.n.i * origin.x - plane.d;
			}
			else
			{
				byte *bsp = (byte *)g_4e0348;
				byte *instance = *(byte **)(bsp + 0x144) + entry * 0x58;
				byte *definition = *(byte **)(bsp + 0x13c) + *(short *)(instance + 0x34) * 0xc8;
				short plane_index = *(short *)(*(byte **)(definition + 0x9c) + surface * 8);
				bsp3d_get_plane((s_bsp3d *)(definition + 0x70), plane_index, &plane);
				transform4x3f *matrix = (transform4x3f *)instance;
				vector3f normal;
				normal.i = matrix->up.i * plane.n.k + matrix->left.i * plane.n.j + matrix->forward.i * plane.n.i;
				normal.j = matrix->up.j * plane.n.k + matrix->left.j * plane.n.j + matrix->forward.j * plane.n.i;
				normal.k = matrix->up.k * plane.n.k + matrix->left.k * plane.n.j + matrix->forward.k * plane.n.i;
				plane.d = matrix->position.z * normal.k + matrix->position.y * normal.j + matrix->scale * plane.d + matrix->position.x * normal.i;
				plane.n = normal;
				side = plane.n.k * origin.z + plane.n.j * origin.y + plane.n.i * origin.x - plane.d;
			}
			if (side > 0.0f) *(vector3f *)((byte *)&results[key] + 0x18) = plane.n;
			else valid = false;
		}
		short material;
		function_1ee410((s_lookup_source *)shape, &material);
		results[key].material = material;
	}
	else
	{
		s_query_node *node = (s_query_node *)((s_node *)second)->get_last();
		hkEntity *entity = node->type == 1 ? node->entity : NULL;
		real friction, restitution;
		function_182b90((c_material_shape *)second->shape, entity, &friction, &restitution, &results[key].material);
		if (entity)
		{
			index = havok_entity_component_index_get(entity);
			other_index = havok_entity_property_2002_get(entity);
		}
	}
	if (valid && contact->normal.k > scale)
	{
		results[key].found = true;
		*(point3f *)((byte *)&results[key] + 0xc) = position;
		*(vector3f *)((byte *)&results[key] + 0x18) = contact->normal;
		results[key].fraction = distance;
		results[key].index = index;
		results[key].other_index = other_index;
	}
}
