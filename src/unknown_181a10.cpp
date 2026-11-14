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

struct source_2df020;
class class_2df020
{
public:
	void function_2df020(source_2df020 const *source);
};

class c_query_array_collector : public c_query_collector
{
public:
	virtual ~c_query_array_collector();
	/* retail table 0x453b90 slot 1 (0x2df020): the contact appended to
	   the array at +0x10; empty, every query here found nothing (0x183910
	   gave explosions no closest point, so their damage fell off as if
	   from far away) */
	virtual void collect(s_batch_contact const *contact)
	{
		((class_2df020 *)this)->function_2df020((source_2df020 const *)contact);
	}
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

/* the entity behind the last node of a hit's chain, if that node is one */
static __forceinline hkEntity *query_node_entity(s_node *node)
{
	s_query_node *last = (s_query_node *)node->get_last();
	if (last->type == 1)
		return last->entity;
	return 0;
}

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
			excluded = excluded_component;
			hkEntity *entity = excluded != NONE ? query_node_entity(node) : 0;
			bool include = true;
			if (entity && havok_entity_component_index_get(entity) == excluded)
				include = false;
			if (include)
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
	byte unknown0a[2];
	point3f position;
	vector3f normal;
	long index;
	long other_index;
	bool flag2c;
	byte unknown2d[3];
	vector3f surface_vector;
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
	c_query_shape *shape = TEST_FIELD_BIT(component->flags.has_shape) ? component->shape : NULL;
	for (long i = 0; i < count; i++)
	{
		results[i].found = false;
		results[i].fraction = FLT_MAX;
		results[i].index = NONE;
		results[i].other_index = NONE;
		// read through a pointer: retail reloads the default material on every pass
		results[i].material = *(short const *)&g_54e898;
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
	bool function_182800(struct s_vehicle_ray *ray_data, long excluded_component);
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

/* casts a ray through the bounded world and reports the nearest hit. A member: retail's callers load the
   world straight into ESI and test it there (0xf6670, 0x206090), which a free function's argument does not give */
// @retail 0x182800
bool c_bounded_query_world::function_182800(s_vehicle_ray *ray_data, long excluded_component)
{
	s_query_result *result = (s_query_result *)ray_data;
	point3f end;
	end.x = result->origin.x + result->direction.i;
	end.y = result->origin.y + result->direction.j;
	end.z = result->origin.z + result->direction.k;
	s_bounded_ray ray;
	ray.flag = false;
	c_nearest_query_callback collector;
	collector.fraction = 1.0f;
	collector.result = result;
	collector.excluded_component = excluded_component;
	collector.result->fraction = FLT_MAX;
	point3f origin;
	origin.x = result->origin.x;
	origin.y = result->origin.y;
	origin.z = result->origin.z;
	ray.from.m128_f32[3] = 0.0f;
	ray.to.m128_f32[3] = 0.0f;
	collector.found = false;
	for (long i = 0; i < 3; i++)
	{
		ray.from.m128_f32[i] = function_182801(origin.n[i], &lower.m128_f32[i], &upper.m128_f32[i]);
		ray.to.m128_f32[i] = function_182801(end.n[i], &lower.m128_f32[i], &upper.m128_f32[i]);
	}
	_control87(0x9001f, 0x8001f);
	_mm_setcsr(_mm_getcsr() | 0x1f80);
	cast_ray(&ray, &collector);
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

/* the closest-point collector with room for eight contacts in place */
struct s_distance_query_collector
{
	c_query_array_collector collector;
	s_distance_query_contact contacts[8];
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

/* The collector and its eight in-place contacts are one aggregate, as 0x183910 declares them;
   that puts j, first and i in retail's spill slots. */
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
					s_distance_query_collector query;
					c_query_array_collector &collector = query.collector;
					collector.entries.capacity_and_flags = 0x80000008;
					collector.maximum = FLT_MAX;
					collector.entries.data = query.contacts;
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
							real_point3d_set(b, contacts[k].position_distance.m128_f32[0], contacts[k].position_distance.m128_f32[1], contacts[k].position_distance.m128_f32[2]);
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
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, long plane_index, plane3f *plane);
short *function_1ee410(s_lookup_source const *source, short *result);
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
PRIVATE __forceinline bool batch_shape_pointer(c_batch_contact_shape const *shape)
{
	long value = (long)shape->data;
	long limited = value < 1 ? 1 : (value > 16 ? 16 : value);
	return limited != shape->data;
}

// score 238 (was 238; 238 in round 95's baseline, m3 lane 11 note near the end, m3 lane 2 r79 and m3 lane 8 r164 at the end): frame 0x40 versus 0x44 (retail keeps origin y/z in reused slots and has two unused dwords below
// MiniMax A (11-13): no new form, no lower score (239); the frame is still 0x40 against retail's 0x44, the second
// node's home is still +0x14 against retail's +0x20, retail still reads the shape's data word three times where
// ours reads it once, and the plane index still leaves the frame as a short.
// Earlier 239 (was 239): frame 0x40 versus 0x44 (retail keeps origin y/z in reused slots and has two unused dwords below
// the plane); classifier clamp form, origin and hit copies, transformed-plane and output homes remain.
// MiniMax H (11-11): no new form, no lower score (239); the frame is still 0x40 against retail's 0x44, retail still re-reads
// the shape's data word three times in the clamp and ours copies it once, and the plane index still goes out as a short.
// MiniMax I (11-06): the node-type guard as a named local, as a field reference, against a
// constant held in a local, with an unsigned field and as an if/else all keep the folded memory
// compare; no lower score.
// MiniMax lane A (11-04): no new form, no lower score.
// normal component loop 319; material-output scope 319; direct validity 351;
// key-clamp result arms 319; optional entity result 319.
// j92: frame 0x48 versus 0x44 and node/plane/output homes remain; delayed reads and native rotation copy regress; coordinate names and positive root guard tie.
// hit snapshot after test 353; first node after root 352; origin coordinate homes 319; hit coordinate homes
// 319; native rotation copy 442; root positive body 319.
// Shape-input classifier with field equality 314 plain (1250/1243 bytes); root entity 319;
// native plane input 354; rotation/translation/both bases 319/319/319; final normal value 356;
// native plane output 348; shape-input sampled equality 319; ordinary inline classifier 356.
// Frame and plane/output homes remain. classifier bound result first 329; native classifier signed domain
// 321; classifier direct bounds 345; existing entity query boundary 321; native destination record 388;
// native output index values 321; validity after index defaults 321; readonly first node input 321.
// Plane normal setter 334; native matrix reference 321; entry/surface/both words
// 324/323/326; staged plane distance 321; origin field base 323; shape field reference 321.
// Native signed data, live classifier fields, both 321/321/321; material output names/scope/both 321/321/321; short material home 321; signed data with output names 321.
// Origin fields at uses 318 (three-count gain rejected); hit fields at publication 329; both 366;
// first/second node at uses 379/357; byte/long classifier results 321/321;
// classifier reference input 321. All existing input snapshots and helper contracts restored.
// Round 152: All normal-row partial sums/products 321/321; normal with distance products/stages 321/321; native normal library copy 321; copy with all row products/partials 321/321; all distance products 321.
// Component result 370; search index 338; property cursor 336; cursor result 338;
// bounded search condition 327; output index homes 321; hit distance home 321;
// component result with output homes 370. Original entry scan and scalar homes restored.
// Separate root guards 321; exclusion equality first 324; direct key-clamp test 321;
// publication/validity rejection guards 321/321; readonly instance transform 321;
// immutable material receiver/short material output before plane 321/321. Original body restored.
// Scalar origin Z/Y/X, hit Y/X/Z and both producer orders 321/321/321;
// side before normal publication 324; immutable normal 321, with late publication 324;
// encoded field at uses 358; consistent readonly second-node/root contract 321.
// Round 54: in-place normal (x/y/z then plane.n, d) 334 at retail length (frame 0x3c, contact in ESI);
// summed distance 334; shared matrix_transform_vector in place/into normal 334/321; constant-first
// clamp 321; clamp through an inline pin function 321.
// Round 350: complete plane helper 353; with origin fields, hit snapshot, both 324/337/335;
// transformed arm first 354, with origin fields/hit snapshot/both 343/339/350.
// Complete helper input context used; all forms restored, frame/plane/output homes remain.
// Round 96: retail lays out the instance-plane arm first and tests the type as `shr 29; dec; je` (a
// switch's shape; VC folds an if on (data >> 29) into a mask compare). Arms swapped as an if 323; with
// the plane distance in a local 323; distance local alone 321; switch with case 1 first/default first
// 316/315 (not kept: a switch for an if).
// Round 230: shared discarded coefficient output with branch/entry scratch and original/shared
// material scope 319/319/319/319; consuming the material return on those four graphs
// 317/317/320/320. Two/four-point gains rejected; original outputs and scopes restored.
// Lane 5 r77: retail's 1..16 clamp keeps each constant in the result register (`mov ecx, 1;
// cmp ecx, edx; jg`) and threads both clamped arms straight to the byte test, which keeps a
// `setne; test`. Entity as `type == 1 ? entity : NULL` 321; clamp as if/else-if on the local,
// constant-first if/else-if and the PIN macro 321/321/321 (identical code); side test written in
// each arm without the side local 352 (1289 bytes). All restored.
// Round 23: property count/base/both observations 321/338/338; next-link snapshot 364;
// link plus count/base/both 364/348/347; all with native property reference 346.
// Frame stays 0x48 versus 0x44; original input scans and helper contracts restored.
// Round 203: final native vector setter with transformed-normal construction, plane-normal
// publication and complete named exclusion combinations 321/321/334/334/321/321/334/334.
// Plane publication reaches 1243 bytes but regresses; frame 0x48 remains. All restored.
// Round 227: Positive admission byte/short/long/dword, rejected bool/byte/short/long 321/331/331/331/321/321/331/331.
// Original body and all helpers restored.
// Round 237: root/component/classifier noinline 359/358/349; root-component/root-classifier/
// component-classifier/all 384/365/401/414; all ordinary inline 401 (control 321).
// Entire original helper graph restored; frame and plane/output homes remain.
// Round 390: Native first/second shape values and admitted computed last-key observation,
// seven subsets plus signed encoded data: 369/321/359/320/359/322/358/358. Original body restored.
// Round 181: common exit with original/separate/compound/nested admission 321 each; reversed material arms on those four graphs 316 each. Five-count lead rejected; original body restored.
// Round 397: shared initial component query; marked getter, second-first acquisition and two-use entity masks 0..7
// 321/358/328/362/321/358/328/362; all restored.
// Key-continuation node/shape 322/353; shared admitted normal Z 317; native-position
// combinations 313/378/340/373 (exact-only grouping, rejected); position alone 344;
// shape plus shared Z 349. Original body restored; frame and plane/output homes remain.
// Round 427: complete scalar transformed normals with plane/side/material entry-scope
// masks 0..7, 334 each (1243 bytes, frame 0x3c); native shape entry/surface/kind bitfields
// with uniform at-use encoded data, masks 0..7, 358 each. Original shape layout, snapshots,
// normal workspace and full body restored; frame 0x48 versus 0x44 and classifier homes remain.
// Native unsigned interval/complement, material-case order and discarded coefficient masks
// 353/351/349/347/355/353/351/349; original classifier, outputs and complete body restored.
// Round 465: intact native normal at plane/entry/material/admitted scopes, each with existing/entry
// queried-plane scope, 321 each on the scalar-origin graph. Original declarations and full body restored.
// Round 221: material arm first through the common exit plus direct lookup-return consumption, with
// separate/shared discarded coefficient outputs, 379/377; both at entry scope 379/377; the four graphs
// with native byte validity 379/377/379/377. Frame grows to 0x4c and extents to 1288/1286 versus 1243.
// All thirteen matched controls survive. Original independent coefficient outputs, geometry-first arm,
// material receiver, Boolean validity, exits, normal/plane arithmetic and full body restored; frame 0x48
// versus 0x44 and classifier/plane/output homes remain.
// Round 45 (lane 13, fast): retail loads the shape data three times in the exclusion test and ends it
// with setne/test. data as a dword/byte-pointer union with the byte test through the pointer 321 (one
// shared load); pointer check and byte test as one && bool helper 319; the condition as a named bool 321
// (same code); a bool helper with early returns 331 (retail's setne/test, loads still shared); that as a
// single-exit result 331. Original body restored.
// Round 40 (m3 lane 15, fast = plain): the component check is the inline sequence of src/unknown_1c25a0.cpp's
// contact_shape_has_material (matched 0x1c5400/0x1d4880), but retail here loads the data field three times and clamps
// with the constant in the result register (`mov ecx, 1; cmp ecx, edx; jg`), unlike those siblings. A same-file copy
// of that helper shape 335 (1258 bytes, retail's setne/test), plus the key test through its pin 335; clamp
// `1 > v ? 1 : (16 < v ? 16 : v)` 321 (same code); MIN(16, MAX(1, v)) written out 341; result-register if form 329
// (both comparison spellings); two-step clamp 347.
// Round 23 (lane 18, fast control 321): in the 1..16 classifier, MIN(16, MAX(1, v)) as two named steps
// 321 (same code); max then an if for the upper bound 321; `limited = 1; if (v >= 1) limited = v > 16 ?
// 16 : v;` 332 (retail's `mov ecx, 1` preload); the nested preset (`limited = 1; if (v >= 1) { limited = 16;
// if (v <= 16) limited = v; } `) 329 and with constant-first tests 329 (both bounds preloaded as retail, but
// `cmp value, const` and the data copied to EAX; retail reads the data field three times, ours once);
// origin through a point pointer local read x, y, z / z, y, x 323/322. Diagnostics: the byte test in a bool
// helper 321 (folds); data test, classifier and byte test in a multi-return bool helper 331 (gives retail's
// `setne al; test al, al`); the data field as a dword/pointer union read as a pointer for the byte 321.
// Round 443: phase-specific final/surface/material output receivers, seven
// pointer subsets and full graph with final reference, 344/393/329/321/344/393/329/329.
// Original output observations, scalar samples, plane math and full body restored.
// Round 284: existing shared axis-rotation producer, separate/in-place plane output,
// without/with all four named distance products: 419/419/448/448; shared row-rotation
// producer 321/321/352/353. Grouping changes were exact-only; all helpers, includes,
// normal/plane expressions and complete body restored. Frame 0x48 versus 0x44 remains.
// Round 74 (m3 lane 12, fast): the material read back through 0x1ee410's returned pointer (retail's
// `mov ax, word ptr [eax]`) 319; the root entity as `root->type == 1 ? root->entity : NULL` then tested 321 (retail
// loads the type into ECX first; ours still compares memory); both 319; distance computed before the origin
// reads 322; index/other_index declared before valid 321.
// Round 451: Existing initial-entity/repeated-root/material-entity helper boundaries,
// subsets 1..7 and complete graph with shared discarded coefficient: 343/350/360/321/343/350/360/362. Original traversals,
// coefficient outputs, helper uses and full body restored; frame/classifier/plane homes remain.
// Round 241: Complete normal/distance/side multiplicand ownership, seven nonempty
// subsets 321 each; all with successive original-group normal/distance sums 321.
// Original producers, helper contracts and complete body restored.
// Round 458: Complete actual transform input ownership, mutable/immutable, with
// original/native plane input and original/memcpy normal publication:
// 468/468/514/514/468/468/514/514. Original matrix observations, plane inputs and publication
// restored; frame 0x48 versus 0x44, classifier and output homes remain.
// Round 77 (m3 lane 12; fast = plain): the instance-plane arm first as `if ((data >> 29) != 1)` (retail's block
// order) 323 alone but register-masked aligned 204 -> 154; origin as one point3f copy 310 alone (1247 bytes, frame
// 0x4c, aligned 212); with the hit point as one point3f copy too 310 (1243 bytes); both copies on the swapped arms
// 308 at 1243 bytes (kept, aligned 162); swapped arms with origin copy 313, with the hit copy only 312 (aligned
// 151); kept form plus the material read through 0x1ee410's returned pointer 310 (311 without the hit copy, 322 on
// the swapped arms alone); kept form with scalar transformed normals 357 (1208 bytes); scalar normals alone 334
// (frame 0x3c), with the origin copy 350, with origin and hit copies 368; origin assigned by components 322,
// through real_point3d_set 324; hit copied out by struct assignment 333. Retail copies origin and hit with movss
// (ours: dword moves) and keeps origin x at +0x28 above the hit point.
// On the kept form: the hit point through real_point3d_set 313; the hit copy declared before the origin copy
// 306 (two-count gain rejected), with the material through the returned pointer 308; distance, hit, origin in
// that order 307.
// Round 464: direct native normal-field accumulations, rowwise/leading-pair/leading-product/
// termwise schedules, with original/early plane-normal publication: 321/321/321/321/321/320/321/319.
// Early leading-pair/termwise publication gains one/two counts, rejected; original normal and plane restored.
// Current aggregate origin/hit graph: captured normal input/direct plane output, early
// normal publication, memcpy and vector setter 335/308/308/357; plane at entry 335/308/308/357.
// AC477: Native three-float origin/hit/completed-normal copy loops, subsets 1..7, then
// short copy ordinal: 315/313/326/357/344/329/342/307. One-point lead rejected; original
// aggregate copies, setters, plane math and body restored. Frame/point/plane homes remain.
// M323: Native instance/mesh-surface/direct-surface layouts, seven subsets and complete readonly inputs, all 308. Original input observations, types and body restored; frame 0x4c versus 0x44 and classifier/plane/output homes remain.
// Round 56 (m3 lane 13, fast): retail keeps origin as three scalars (x at +0x38, y and z in reused slots +0x14/+0x18)
// and copies the hit point out with dword moves. Origin as three named reals through a position pointer 315 (1251
// bytes, frame 0x48); read straight from the transform 312 (1248, frame 0x48); each with the hit copied out by
// struct assignment 322/319 (1228/1231); the struct assignment alone 331 (1218, frame 0x4c). Every 0x14a300 call
// here sign-extends the plane index in retail (a long parameter; needs src/unknown_17d9a0.cpp, avoided this round).
// Round 44 (m3 lane 2, fast = plain): retail's slots: origin y/z at +0x14/+0x18 (reused by the material and
// friction), hit +0x2c..+0x34, origin x +0x38, +0x3c/+0x40 never touched, plane +0x44..+0x50 (normal written
// in place); the untouched pair looks like a reserved aggregate home. Origin and hit copied by components
// 326 (1263 bytes); scalar origin read z/y/x with the hit by components 325; hit by components alone 313
// (1250); that with the in-place normal 328 (frame 0x40), with the normal copied into the plane by components
// 329; origin through real_point3d_set 315, with the in-place normal 344; the in-place normal on the kept form
// 357 (1208 bytes, frame 0x40); all 18 origin/hit/normal combinations at or above 308.
// Round 130 (m3 lane 8, fast): retail keeps first->next->shape in ECX for its test and the call (`mov ecx,
// [eax]; test ecx, ecx; mov eax, ecx`), ours compares memory and reloads. A next pointer and next-shape local
// assigned in the test 309 (gives the register load, EAX not ECX); a next pointer local alone 312; the clamp
// helper comparing with its own `value` 332 (1232 B); the second shape in a local for the exclusion test 346;
// the key range test as `key < 0 || key > count - 1` 308 (different code, aligned worse).
// Round 144 (m3 lane 6, fast; register-masked aligned [169]): `plane3f plane`, the transformed normal, and
// material/friction/restitution each declared at function scope 308 (same code); the classifier reading
// `shape->data` at each use 308 (same code); the exclusion test as a single-exit bool helper on the shape (result =
// byte test under the kind/data/classifier test) 320 [178] (retail's `setne al; test al, al`, but the shape stays in
// ESI where retail re-reads [ebx] after the virtual call); that helper on the contact node, reading `node->shape` at
// each use, 319 [162]. Retail's three loads of the data and its preloaded clamp constants are reproduced by no form;
// both matched siblings (0x1c5400, 0x1d4880) use the single-load clamp.
// P307: Existing plane-output returns at transformed/direct/both consumers, with original/returned
// material and earlier native outputs: 341/306/346/343/325/348/346/348. Direct return 306 at 1243
// bytes NOT kept: two-count gain and unmatched mixed receiver graph. Original body restored.
// DH331: Native origin/hit/position library-copy subsets 1..7, then all with completed-normal
// library publication: 308/308/308/331/331/331/331/331. Original copy boundaries and entire body restored;
// frame 0x4c versus 0x44 and classifier/plane/output homes remain.
// DH335: Standard backward origin/hit/normal transfers, subsets 1..7, then
// complete final position transfer: 312/303/372/305/376/352/354/377. NOT kept:
// hit-only gain five and paired gain three; original complete body restored.
// AC515: Complete exclusion result on the aggregate origin/hit graph, bool/byte/short/long,
// at computation or with entry-owned uninitialized storage: 308/308/308/308/308/308/308/308.
// NOT kept: unmatched result storage; original guards, observations and complete body restored.
// AC519: Standard forward origin/hit/completed-normal subsets 1..7, then final
// position transfer: 312/303/305/378/382/355/357/383. NOT kept: best gain five
// is below eight; normal forms shorten away from retail. Original body restored.
// AC524: Complete root admission bool/byte/short/long, then complementary
// rejection, 308 each at 1243 bytes. NOT kept: frame 0x4c versus 0x44, root-kind
// transport and classifier/plane/output homes remain; original collector restored.
// Lane 9 r87 (fast; aligned against 308/210): the classifier as `long limited = 1; if (limited <= data) { limited = 16; if (limited >= data)
// limited = data; }` keeps the constants in the result register and the unthreaded `mov ecx, eax` as retail 327/205 (retail also reads the
// data three times where ours shares one); data through a local 339 (1227 bytes), through a long reference 326; the byte test in a bool
// helper or a bool local, on either 326-327 (setne still folds). Plane normal as three scalars, stored before or after the side, 357/216,
// 357/216, 359/221 (frame 0x40, 1208 bytes). Root type in a local 308 (same code).
// AC539: Post-clamp bounded-key rejection result, positive then complementary
// bool/byte/short/long: eight at 308. NOT kept: unmatched one-use decision storage;
// original clamp, snapshots, plane math and full collector restored.
// Coordinated long plane-index formal/declarations: 308; callee matches but a caller
// rises 1 -> 103 (580 -> 576 bytes). NOT kept: shared-user regression; all declarations restored.
// AC541: Complete final validity/normal-threshold publication or rejection,
// bool/byte/short/long: eight at 308. NOT kept: no equality/gain;
// original guards, snapshots, plane math, publication and full body restored.
// AC543: Strict distance-improvement admission/complement at the original capture
// boundary, bool/byte/short/long: eight at 308, 1243 bytes. NOT kept: one-use
// outcomes unmatched; frame 0x4c versus 0x44 and classifier/plane/output homes remain.
// Original unordered rejection, snapshots, plane math, calls and full body restored.
// M381: Explicit plane-side-produced admission/rejection, bool/byte/short/long on
// the aggregate origin/hit graph: 325/325/311/309/328/328/315/316. NOT kept: all regress;
// original implicit success, plane publication, material outputs and full body restored.
// Round 64 (m3 lane 14, fast; rank on register- and stack-masked aligned counts, here 127 at 308): the side test in each plane arm with
// scalar normals, hit set by components, hit published by struct assignment 243 (1243 bytes, aligned 106); the shape read from second->shape at
// each use instead of a local gives retail's entry registers (second EBX, next EDI, ESI saved later) but with those copies 312 (1251, 175); with the hit as one
// copy and set at publication 242 (1249, 113, register-sensitive 178), plus the material through 0x1ee410's returned pointer 239 (1247, 112, 177, kept). On that: origin
// by components z, y, x 311 (1247, aligned 92, retail's loads, register-sensitive 166), x, y, z 312 (101), through real_point3d_set 252 (1260, 105); hit copied ahead of origin 238
// (one point, not kept); `limited = 1; if (limited <= v) { limited = 16; if (limited >= v) limited = v; }` 320 (1243, aligned 100: retail's preload and block
// order, `cmp edx, ecx` still reversed; negated tests the same code), with origin by components 319 (1243, aligned 80, the lowest), plus a
// long root type 319 (80), plus a next-shape local 319 (79), plus a one-use bool for the byte test 323 (79). Normal row terms in six orders 239 each (aligned 106-116).
// long plane index locals 239 (same `mov dx`: the short formal narrows; the callee's formal is the real blocker, a caller rose before). Hit through
// a contact position pointer 310 (folds). Left: contact takes a register once the hit is read by components (retail keeps it on the stack), three data reads.
// Round 153 (m3 lane 5, fast): a shape local inside the kind-24 block (retail keeps second->shape in EBX there for the data
// read and the 0x1ee410 call) 327 (1249 B); index/other_index declared at the top 274 (1244 B); that shape local with the hit
// read by components and published by aggregate assignment 248 (1243 B, entry registers move to EBP/ESI); hit by components
// with aggregate publication 312; aggregate publication alone 340 (1227 B). Retail's frame reads like a scalarized origin: x keeps
// the 12-byte home (+0x28, y/z slots unused) while y and z are spilled into first's and friction's slots (+4, +8); no form
// here (aggregate copy, components x/y/z or z/y/x, through a position pointer) does that in a compile-only screen.
// Y603: Separate queried planes, earlier uninitialized material/coefficient outputs and
// source-arm plane names: 239/231/239/231/239/231/239/231/231/239/231. NOT kept: 231 recovers frame 0x44
// but moves the first-node home to +0x14 (retail +0x10); unmatched frame-only steering.
// Material-only isolation also gives 231; original body and all declarations restored.
// DH377: On the scalar-normal/actual-node graph, typed final-normal setter, complete plane-normal
// setter and both 239/239/239; native hit publication alone/with either/both 340 each (1227 bytes),
// standard hit memcpy 340. NOT kept: no gain; original publications, captures and full body restored.
// Frame 0x40 versus 0x44, classifier observations and plane/point homes remain.
// DH380: Original predecessor/shape/query key-admission boundaries, seven partitions and complete
// partition with separate original root guards: 239/239/239/239/239/239/239/239. NOT kept: frame/receiver
// homes remain; original tests, observations, query contracts and complete collector restored.
// N395: Actual first/second/root/component/key/bounded-key names individually, then
// two complete node/key identity orders: 239/239/239/239/239/239/239/239. NOT kept: no equality or
// frame/home recovery; original names and complete collector restored.
// N397: Complete scalar origin xzy/yxz/yzx/zxy orders, original then early material output 308/308/308/308/311/311/311/311.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// b5I (11-06) retail walk, statement by statement (0x1822f0..0x1827c6): root walk and 0x2001 property scan, the
// kind 24 / data / 1..16 clamp exclusion, kind 4 test, component_index test, key adjust (+ next->shape->data * 8 - 8),
// key clamp, origin (+0x50), distance + radius, fraction test, the kind 24 data decode (shr 0x1d; dec; je), both
// plane arms (operand orders and 0.0f tests), 0x1ee410 material, the final scale test and result stores: no
// behaviour difference found. Retail inlines 0x183da0 (get_last) and 0x1cf280 (property 0x2001 scan) at the top, plus
// one dead second get_last walk (0x18235a), and calls them only in the else arm (0x182776, 0x1827b2), so the top keeps
// their bodies written out (batch_contact_root / batch_contact_component). Calling the real helpers there instead
// (query_node_entity, havok_entity_component_index_get) stays out of line: 392 (1200 bytes); index_get alone 368,
// query_node_entity alone 361, with the dead walk inline 384, without it 404. The else arm calls query_node_entity and
// the normal publish uses vector3f_assign instead of a (point3f *) cast (same code, 239). `results[key].normal =
// contact->normal` 272 (1243 bytes), `results[key].position = hit` 340.
// Round 68 (m3 lane 2, fast): both plane side tests through the existing plane_distance_to_point 239 (aligned 183; arm 2's
// hand-written test is that helper's body, identical code; arm 1 alone 239, aligned 183); `first->shape->radius + contact->distance`
// 239 (same code); the normal rows written forward, left, up with plane.d's terms reversed 239 (masked 112 -> 107): writing the rows
// forward-first makes VC compute them as retail does, (up*k + left*j) + forward*i (lane 14 r64's row orders, re-measured: rows alone
// 239, masked 107); a shape local for the kind test and the kind-24 block 362 (1206 bytes); the structure global read at each use
// 239 (same code); re-measured: the normal through matrix_transform_vector 332 (1265 bytes).
// Round 163 (m3 lane 6, fast; aligned/masked = difflib counts, addresses or also registers and stack offsets
// masked, control 177/112): retail's 12-byte home at +0x38 holds only origin x, y and z sit in first's and
// friction's slots, as if the point were split into scalars; the clamp idiom (`mov ecx, 1; cmp ecx, edx; jg`)
// appears nowhere else in retail. Origin by components z, y, x from the transform 309 (1245 B, 167/90, origin
// still one 12-byte home); that with the hit by components, aggregate hit publication and a kind-24 shape local
// 361 (1273 B); the distance sum operands swapped 239 (same code); both side tests through the existing
// plane_distance_to_point 239 (183/112); origin z, y, x through a position pointer with the hit by components
// (y, x, z) and aggregate publication 346 (1247 B, 265/173); the dead second walk through the real get_last 357
// (1227 B: called out of line); our build pads that walk's loop head (`lea ebx, [ebx]`) where retail has none.
// The decode as `switch (data >> 29)` with default/case 1 (0xdd990 switches on the same key) 239 at 1243 B
// (179/114: retail's `shr 0x1d; dec; je`, but the surface index still sinks into the instance arm where retail
// computes it before the branch; a switch for an if, not kept); the structure global read at both uses in place of
// the `bsp` local 239 (same code). All restored.
// M3 lane 10 r88 (fast with 0x1829b0, 0x1821d0, 0x182800, 0x183670; base 239, aligned 177, masked 112): the kind-24 block reading
// `second->shape->data` at each use instead of the `data` local 237 (1245 bytes; two points, not kept; retail holds the shape in EBX
// there and decodes with `shr ecx, 0x10; shr eax, 0x1d; ... dec eax; je`, ours with `and eax, 0xe0000000; cmp`); the distance written
// radius-first 239; the 1..16 classifier as `value > 16 ? 16 : (value < 1 ? 1 : value)` 239 (same code); the classifier with no value
// local, `(long)shape->data` at each use, re-measured 239 (same code).
// m3 lane 15 r72 (fast with 0x1829b0/0x1821d0/0x182800): the material output declared at function scope re-measured 231
// (Y603's lead: frame 0x44 as retail, but first's home moves to +0x14 where retail has +0x10, second's to +0x18 where retail
// has +0x20; a declaration move, match-only), the same after `valid` 231. On it: the material renamed surface_material or
// material_index 231 each, the plane or friction/restitution at function scope too 231 each (all same code); the hit copy
// declared at entry with it 340. On main: friction and restitution at function scope 239 (same code). The other listed
// lever, a long plane-index parameter for 0x14a300 (retail sign-extends it at every call), needs src/unknown_17d9a0.cpp,
// which another lane holds this round.
// M3 lane 1 r89 (fast with 0x1829b0, 0x1821d0, 0x182800; control 239, aligned 109, masked 71): the origin through
// real_point3d_set from a position pointer local (the two listed parts together) 252 (1260 B, aligned 105; the inlined setter
// reads x, y, z into one 12-byte home, where retail reads z, y, x and keeps only x at +0x28); the setter with the cast written
// at each argument (the listed setter form, re-measured) 318; the node's transform as a typed record with a point3f member at
// +0x50, origin copied from it, 239 (same code); origin and hit both through real_point3d_set 348 (1273 B); origin as a brace
// initializer from the position pointer 312 (1248 B, aligned 99, origin still one home in x, y, z order), with the hit as a
// brace initializer too 362 (1261 B); the final test as `valid && scale < contact->normal.k` 239 (same code). Diagnostic,
// not kept: the dead second walk as an empty-body `next` chase 239 (the loop-head pad stays).
// Round 171 (m3 lane 5, fast with 0x1829b0, 0x1821d0, 0x182800; control 239, aligned 137, masked 71): the origin as
// three reals read z, y, x 308 (1245 B, 135/64); that plus the hit assigned by components 362 (1258 B); the hit by
// components alone 325 (1262 B); the hit as `point3f const &hit = contact->position` 335 (1243 B), with the scalar
// origin 333 (1243 B); the component scan at the top through the existing header havok_entity_property_get 239 (same
// code).
// M3 lane 13 r83 (fast with 0x1829b0, 0x1821d0 and 0x182800, all MATCH; control 239, fully masked aligned 71): the decoded
// key type named (`long type = data >> 29;` tested `!= 1`) 239 (same code: still folded to the mask compare); first->shape
// in a local for the kind test and the radius 329 (1264 B) [85]. Retail's two classifier uses read the data field three
// times and thread the 1 and 16 arms straight to the exits, while 0x182b90 (matched, same file family) reads it once
// through its PIN macro, so this one looks like a different helper boundary. Otherwise no untried natural form: the origin/hit copies, scalar origin
// orders, classifier clamp spellings, result receivers, plane-arm orders, helper boundaries and root/key forms are all listed
// above; what remains is the frame (0x40 versus 0x44, retail's two untouched dwords below the plane), the 1..16 classifier's
// three data reads with the constants in the result register, and the dead second walk's loop pad.
// MiniMax A (11-11, second pass): no new form, no lower score (239); what still differs is unchanged: the frame is 0x40 against retail's
// 0x44 (retail's two untouched dwords below the plane), the origin and hit copies keep their own homes, and the plane arm hands 0x14a300
// the plane index in the width our 0x14a300 takes where retail sign-extends it at every caller; that width is set in another file.
// MiniMax A (11-11): no new form, no lower score (239); our frame is still 0x40 against retail 0x44 (the two untouched dwords below the plane), the origin and hit copies keep their own homes, and the plane arm still hands 0x14a300 the plane index in the width our 0x14a300 takes while retail sign-extends it at every caller; that width is set in another file, so the arm cannot be finished from here.
// m3 lane 4 r191 (plain for the first two, fast with 0x1829b0/0x1821d0/0x182800 after; aligned/masked 177/97): a long
// plane index for bsp3d_get_plane (definition and all five declarations) 238 (174/94), with 0x245850 1 -> 103, 0x10cf80
// 202 -> 200, 0x1fa3a0 80 -> 79 (restored; two of those files hold contributor code); `inline` on 0x1cf280's definition
// (its file is /Ob1) with the top scan as a call to it 368 (1210 B; still called, and 0x1829b0 MATCH -> 34); both plane
// side tests' arms swapped (`!(d > 0.0f)` clears valid) 315, the record arm's alone 248; `contact->normal.k > scale &&
// valid` 284 (1243 B); the final block as an early return 239 (2 lines differ).
// M3 lane 11 r95 (fast with the file's functions; base 238, masked aligned 95). LEAD, 235 at 1243 bytes (masked 89; a gain of 3, kept
// only with a match): a `c_batch_contact_shape *shape = second->shape;` local in the kind-24 block for the data read and the
// 0x1ee410 call, the hit set by components and published by aggregate assignment, and `short material` declared beside
// index/other_index; the frame is then 0x44 as retail's and the hit copies with movss, but the entry registers move (second
// ESI, the walk EBX; retail EBX/EDI) and the origin still copies as one 12-byte home with dword moves at +0x2c, the hit at
// +0x38 (retail: hit +0x2c, origin x +0x38). On the lead: no shape local 313 (1249 B); the hit copied as one aggregate 359
// (1261 B); origin by components x, y, z 357 (1273 B); the decode as `switch (data >> 29)` with the instance arm as default
// first 243 (a switch for an if); the hit set before origin is copied 237 (masked 83). On main: the hit by components through
// real_point3d_set with aggregate publication and the material beside index/other_index 311 (1249 B); the exclusion test
// split into `if (component, kind, data) { if (pointer && byte) return; }` 238 (same code: the data field still loads once).
// The lead's diff is saved as build/r95/lead-0x1822f0-235.patch in m3-lane11's worktree.
// M3 lane 2 r79 (fast with the file's functions, all MATCH; base 238, register-masked aligned 135). Retail's frame, as offsets from
// the frame base: valid byte +3, first +4 (later origin.y, then the material), origin.z +8, index +0xc, other_index +0x10, second
// +0x14 (later the kind-24 shape, then friction), distance +0x18, hit +0x1c, a 12-byte origin home at +0x28 of which only x is
// stored (+0x2c/+0x30 never touched), plane +0x34. Tried: the data word through a small inline accessor 238 (same code: the three
// retail loads are not accessor temps); origin by components from a position pointer with both side tests through
// plane_distance_to_point(&plane, &origin) 312 (1248 B; origin keeps one home); on lane 11 r95's 235 lead (re-measured 235)
// origin by components z, y, x 364 (1275 B); z, y, x origin with `short material` beside index/other_index 311 (frame 0x44,
// masked 119; first/second at +8/+0xc); that plus the hit by components with aggregate publication 345. Diagnostic grid (origin
// aggregate / x,y,z / z,y,x / setter, side tests hand-written / helper, hit aggregate / components): none below 238; z, y, x
// components give retail's movss read sequence exactly (311) and plane_distance_to_point is the same code as the hand-written
// tests every time. The root entity as `root->type == 1 ? root->entity : NULL` re-measured 238 (the type compare still folds).
// Also: the hit by components in retail's y, x, z read order with aggregate publication 312 (1249 B), with main's setter
// publication 325 (1262 B; each a different code from the x, y, z forms at the same scores). The accessor form is the same
// code as the listed per-use reads and the plane_distance_to_point form the same as lane 14 r64's x, y, z origin (312).
// m3 lane 8 r164 (fast with 0x183670, MATCH each time; base 238): one `s_query_batch_result *result = &results[key]` for
// all 13 uses 438 (1148 B: retail forms results + key * 0x3c at each use); the hit distance as `radius + contact distance`
// 238 (same code); hit and distance copied before origin 238 (same code); lane 11 r95's kind-24 `shape` local alone on
// main 327 (1249 B); `component == component_index` 239. Diagnostic (reverted): the classifier reading the data word
// through a `long const *` view 238 (same code: still one load where retail loads it three times).
// P472: Overlap-safe origin/hit/paired-input/position/paired-plane-normal/origin+position/hit+position/all record transfers: 229/233/310/261/249/266/264/338.
// NOT kept: origin transfer adds a call absent from retail; hit-only gains five. Other forms regress; original body/includes restored.
// Frame, node and point homes remain.
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
		batch_shape_pointer(second->shape) && ((byte *)second->shape->data)[0x1e] != 0xff) return;
	if (first->shape->shape_kind() != 4) return;
	if (component_index != NONE && component_index == component) return;
	long key = first->key;
	if (first->next && first->next->shape && first->next->shape->shape_kind() == 9 && !batch_shape_pointer(second->shape))
		key += first->next->shape->data * 8 - 8;
	long limited = key < 0 ? 0 : (key > count - 1 ? count - 1 : key);
	if (limited != key) return;
	point3f origin = *(point3f *)(first->transform + 0x50);
	point3f hit = contact->position;
	real distance = contact->distance + first->shape->radius;
	if (!(results[key].fraction > distance)) return;
	bool valid = true;
	long index = NONE, other_index = NONE;
	if (second->shape->shape_kind() == 24)
	{
		dword data = second->shape->data;
		if (data)
		{
			long entry = data & 0xffff;
			long surface = (data >> 16) & 0x1fff;
			plane3f plane;
			if ((data >> 29) != 1)
			{
				byte *bsp = (byte *)g_4e0348;
				byte *instance = *(byte **)(bsp + 0x144) + entry * 0x58;
				byte *definition = *(byte **)(bsp + 0x13c) + *(short *)(instance + 0x34) * 0xc8;
				short plane_index = *(short *)(*(byte **)(definition + 0x9c) + surface * 8);
				bsp3d_get_plane((s_bsp3d *)(definition + 0x70), plane_index, &plane);
				transform4x3f *matrix = (transform4x3f *)instance;
				real normal_i = matrix->up.i * plane.n.k + matrix->left.i * plane.n.j + matrix->forward.i * plane.n.i;
				real normal_j = matrix->up.j * plane.n.k + matrix->left.j * plane.n.j + matrix->forward.j * plane.n.i;
				real normal_k = matrix->up.k * plane.n.k + matrix->left.k * plane.n.j + matrix->forward.k * plane.n.i;
				plane.d = matrix->position.z * normal_k + matrix->position.y * normal_j + matrix->scale * plane.d + matrix->position.x * normal_i;
				plane.n.i = normal_i;
				plane.n.j = normal_j;
				plane.n.k = normal_k;
				if (plane.n.k * origin.z + plane.n.j * origin.y + plane.n.i * origin.x - plane.d > 0.0f)
					results[key].normal = plane.n;
				else
					valid = false;
			}
			else
			{
				byte *record = *(byte **)((byte *)g_4e0340 + 0x2c) + entry * 8;
				bsp3d_get_plane((s_bsp3d *)g_4e0340, *(short *)record, &plane);
				if (record[4] & 0x20)
					results[key].flag2c = function_181db0(*(short *)(record + 6), &results[key].surface_vector);
				if (plane.n.j * origin.y + plane.n.k * origin.z + plane.n.i * origin.x - plane.d > 0.0f)
					results[key].normal = plane.n;
				else
					valid = false;
			}
		}
		short material;
		results[key].material = *function_1ee410((s_lookup_source *)second->shape, &material);
	}
	else
	{
		hkEntity *entity = query_node_entity((s_node *)second);
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
		real_point3d_set(&results[key].position, hit.x, hit.y, hit.z);
		vector3f_assign(&results[key].normal, contact->normal.i, contact->normal.j, contact->normal.k);
		results[key].fraction = distance;
		results[key].index = index;
		results[key].other_index = other_index;
	}
}
