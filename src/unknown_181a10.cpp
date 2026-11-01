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
		ray.from.m128_f32[i] = function_182801(origin.n[i], &world->lower.m128_f32[i], &world->upper.m128_f32[i]);
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

// score 8 (was 8): the three values of the query loop are kept in the stack slots in the
// opposite order (retail j, first, i; ours first, i, j). Shared component base 8; i, j, k
// declared at the top 8; chained FLT_MAX stores 8; contact pointer walk 8; shape calls
// through the field pointers 168; about 300 natural forms compiled alone keep the slot order.
// Indexed second/first/both shape reads 24/79/8; indexed second kind 24; both/second entry
// shape reads 135/135; indexed second/both shape addresses 24/8; first body through an entry
// pointer 8; contact index declared outside the loops 8; first body read in the outer loop 211;
// second's body array in a local 158; second's count in a local 151. Round 60: the loop indices
// renamed outer/inner 8 (same slots: these are spill homes, names do not order them); the collector
// and contact buffer declared before the loops 181; i and j declared before the loops 8; an early
// NONE return in place of the enclosing test 87; second declared before first, assigned in order 8.
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
					collector.entries.capacity_and_flags = 0x80000008;
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
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
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

// score 321 (was 321): frame 0x48 versus 0x44 (retail writes the transformed normal into the plane and has two
// unused dwords below it); classifier clamp form, transformed-plane and output homes remain.
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
// Round 241: Complete normal/distance/side multiplicand ownership, seven nonempty
// subsets 321 each; all with successive original-group normal/distance sums 321.
// Original producers, helper contracts and complete body restored.
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
	real origin_x = ((point3f *)(first->transform + 0x50))->x;
	real origin_y = ((point3f *)(first->transform + 0x50))->y;
	real origin_z = ((point3f *)(first->transform + 0x50))->z;
	real hit_x = contact->position.x;
	real hit_y = contact->position.y;
	real hit_z = contact->position.z;
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
					results[key].flag2c = function_181db0(*(short *)(record + 6), &results[key].surface_vector);
				side = plane.n.j * origin_y + plane.n.k * origin_z + plane.n.i * origin_x - plane.d;
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
				side = plane.n.k * origin_z + plane.n.j * origin_y + plane.n.i * origin_x - plane.d;
			}
			if (side > 0.0f) results[key].normal = plane.n;
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
		real_point3d_set(&results[key].position, hit_x, hit_y, hit_z);
		real_point3d_set((point3f *)&results[key].normal, contact->normal.i, contact->normal.j, contact->normal.k);
		results[key].fraction = distance;
		results[key].index = index;
		results[key].other_index = other_index;
	}
}
