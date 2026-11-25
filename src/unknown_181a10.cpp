// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1cec30.h"
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

PRIVATE __forceinline s_batch_contact_node *batch_contact_root(s_batch_contact_node *node)
{
	while (node->next) node = node->next;
	return node;
}
PRIVATE __forceinline bool batch_shape_pointer(c_batch_contact_shape const *shape)
{
	long value = (long)shape->data;
	long limited = value < 1 ? 1 : (value > 16 ? 16 : value);
	return limited != shape->data;
}

// score 238 (was 238): 238 in round 95's baseline, m3 lane 11 note near the end, m3 lane 2 r79 and m3 lane 8 r164 at the end. frame 0x40 versus 0x44 (retail keeps origin y/z in reused slots and has two unused dwords below
// MiniMax B (11-20): no new form, no lower score (238): the frame is 0x40 where retail has 0x44, the node pointer's home is frame+0x14 where retail keeps frame+0x20, the 1..16 classifier folds its first compare where retail loads the word into ECX and compares the register, and the classifier's data is loaded once where retail reloads it; the form that reaches retail's frame needs the material output declared at function scope, a declaration move that is not keepable.
// MiniMax H (11-20): no new form, no lower score (238): the frame is still 0x40 where retail has 0x44, the second
// node is stored at frame+0x14 where retail keeps frame+0x20, and the node's type is still compared in memory
// where retail loads it into ECX and compares the register; a named local for the type measures the same 238, as
// the earlier rounds record.
// MiniMax S (11-20): no new form, no lower score (238): the frame is still 0x40 where retail has 0x44 and the second node's
// home is frame+0x14 where retail keeps frame+0x20; retail's 1..16 classifier holds the constant in the result register and
// compares it against the value register and re-reads the shape's data word three times, while ours folds the range check
// into immediate compares and loads the data once.
// MiniMax E (11-19): score 238 (was 238): no new form, no lower score. Read from the listing again: retail walks the
// node chain with the next pointer already in a register and tests the last node's type and entity, then scans the
// entity's property list for the key 0x2001; our body reads the root's type straight from the node and compares it
// with one, so the type's home and the compare's width differ. The frame stays 0x40 against retail's 0x44, the second
// node stays at frame+0x08 against retail's frame+0x14, and the shape's data pointer is still read twice where retail
// reads it three times and ends its test in a setne and a test pair. This round split the node-type guard five ways (the
// two tests apart, the type in a named local, the entity test first, and both named) and every one of them compiles to
// the same 238, so the load before the compare is not reached from the guard's spelling.
// frame 0x40 versus 0x44 (retail keeps origin y/z in reused slots and has two unused dwords below
// frame 0x40 versus 0x44. The second node and shape share the wrong stack slot.
// Retail keeps origin y/z in reused slots and has two unused dwords below the plane.
// Earlier at 238 in round 95. See the m3 lane 11, lane 2 r79 and lane 8 r164 forms below.
// frame 0x40 versus 0x44, with a different second-node slot.
// Retail reuses the origin y/z slots and leaves two dwords below the plane unused.
// Earlier score 238 in lane 11 round 95, lane 2 round 79 and lane 8 round 164.
// MiniMax E (11-17): no form kept; the score is unchanged at 238. New readings: the 1..16 classifier keeps retail's
// `mov ecx, 1; cmp ecx, edx; jg` form out of reach (an unsigned word, a signed long, the constant written first, two
// separate ifs and an if/else chain all compile to a compare with an immediate, 238 each, as in the earlier rounds);
// the plane-normal sum is evaluated right to left rather than in the written order, so writing the three products
// forward, left, up instead of up, left, forward flips the compiled tree to retail's (up + left) + forward and matches
// retail's rounding, but the score stays 238 since the load schedule and the register allocation still differ, so
// that edit was reverted; the origin and hit snapshot forms re-measured are 308 for three scalar origin coordinates
// with the aggregate hit, 309 for the descending z, y, x member assignment, and 362 when both are copied member by
// member, all above the aggregate form. The permuter's 35 tries in 240 s found nothing below the 235 differing
// instructions of the full image. Still differs: the frame is 0x40 against retail's 0x44, the second node's home is
// +0x14 against retail's +0x20, and retail reads the shape's data word three times in the classifier where ours
// reads it once.
// MiniMax E (11-15): no form kept; the permuter's 68 tries on the reduced image found no variant below the 235
// differing instructions of the full image (the reduced image agrees with the full one here). Still differs: the
// frame is 0x40 against retail's 0x44 (retail keeps the origin's y and z in reused slots and has two unused
// dwords below the plane), the second node's home is +0x14 against retail's +0x20, and retail reads the shape's
// data word three times in the 1..16 classifier (0x18237a-0x18239c) where ours reads it once.
// MiniMax R (11-14): no lower score (238): retail reads the origin and the contact position with float moves into separate
// slots, ours copies the two aggregates with dword moves; the origin as three real coordinate snapshots scores 308 in either
// read order, member-wise origin copies 309, a member-wise hit copy 325 and both 362.
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
// one dead second get_last walk (0x18235a), and calls them only in the else arm (0x182778, 0x1827b2), so the top keeps
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
// m3 lane 14 r93 (fast with 0x1829b0/0x1821d0/0x182800/0x183670, all matched throughout; control 238, register-masked 109): on
// the open helper-boundary lead, the 1..16 test as `limited = 1; if (!(limited > value)) { limited = 16; if (!(limited < value))
// limited = value; }` 320 (1243 B, masked 97: the constants are preloaded into ECX and compared register to register, `cmp eax,
// ecx; jl` for retail's `cmp ecx, edx; jg`, but the data is still copied to EAX and read once); the pointer test on the data
// value (`(v < 1 ? 1 : v > 16 ? 16 : v) != v` on a `long` argument, both uses) 320 (1243 B, masked 117: no copy, one read).
// A corpus scan of all matched retail code finds no other `mov reg, const; cmp reg, reg2; jg` clamp, so no matched source
// shows retail's form. Body unchanged.
// m3 lane 8 r168 (fast with 0x183670): matched 0x182b90's PIN compiles to our `cmp eax, 1; jge; mov ecx, 1` (tools/disasm.py), so
// retail's `mov ecx, 1; cmp ecx, edx; jg` here comes from another source shape. The classifier through a forced-inline pin(value,
// lower, upper) written `value < lower ? ...` or `lower > value ? ...` 238/238 (same code); `limited = 1; if (value >= 1) { limited =
// 16; if (value <= 16) limited = value; }` 320 (1243 B, `cmp eax, ecx; jl`); `limited = value; if (value < 1) ... else if (value >
// 16) ...` 238; an if/else chain on `1 > value` / `16 < value` 238; the 0x1e byte test through a forced-inline helper 238 (same code);
// a forced-inline material getter tested then fetched 377 (1284 B), inside a bool helper 318 (1245 B). The data word stays one load
// in every form. Body unchanged.
// m3 lane 15 r85 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238, masked 135/109): on lane 11
// r95's 235 lead, the origin as three reals read z, y, x through a `point3f const *position` 366 (1275 B: ours then reads them
// exactly as retail, `movss [eax+0x58]` ahead of `add eax, 0x50`, but packs all three into 4-byte slots and keeps contact in
// ESI); `surface` declared before `entry` 238 (same code), on the lead 235 (same code as the lead); the origin copied from a
// `point3f const *position` local 239 (1248 B); `index` and `other_index` declared separately 238 (same code). Re-measured:
// the decoded type named 238 (a scheduling shuffle only). Diagnostic (reverted, not a source form): a full memory barrier
// ahead of the pointer test and of the byte test gives retail's three data loads but also reloads second->shape before each
// (retail loads the shape once), 331 (1255 B): only a reload of the data field alone fits retail, and no permitted form gives it.
// m3 lane 15 r88 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238): the material test nested
// under the component/kind test 238 (same code); the classifier comparing its own copy (`limited != value`, one data read)
// 320 (1243 B); the fraction test as `fraction <= distance` 304 (1243 B); `valid` declared at the top 263 (1246 B); `hit`
// declared ahead of the origin copy, assigned after it, 238 (same code). Body unchanged.
// P516: Binary-enumeration result domains at carried validity/root/key/distance producers,
// positive then complementary encoding: 284/248/238/238/249/248/238/238.
// NOT kept: no gain or frame recovery; original byte result, guards, observations and collector restored.
// M3 lane 2 r87 (fast with 0x1829b0/0x1821d0/0x182800/0x183670, all MATCH): lane 11 r95's lead rebuilt, 235 (1243 B, frame
// 0x44; second in ESI and the walk in EBX); on it the hit set y, x, z 235, through real_point3d_set 235, `hit` declared ahead
// of `origin` 235 (all the same code), `second` declared before `first` 237; on main `second` before `first` 245 (1247 B).
// Retail's homes from its listing: hit +0x1c..+0x24 (set by movss, published by dword copy), origin.x +0x28 with +0x2c/+0x30
// untouched, origin.y/z in +4/+8, and 0x182b90's outputs in +8 (friction) and second's +0x14. Original body restored.
// M3 lane 1 r113 (fast with 0x1829b0/0x1821d0/0x182800/0x183670, all MATCH; control 238, aligned with offsets masked 148, registers
// too 94): first->next and its shape named before the kind-9 test (`next`, `next_shape`) 307 (1245 B); the second shape named for the
// material guard inside a `component != NONE && kind != 24` block 318 (1245 B); the plane declared at the top of the kind-24 block 238
// (same code); the distance written `radius + distance` 238 (same code); `short material` declared with valid/index at the top 230
// (frame 0x44 as retail, first difference +0x1a: first/second homes +8/+0xc against retail's +4/+0x14; a declaration moved by score,
// match-only). A scan of retail finds the `mov ecx, 1; cmp ecx, edx; jg; mov ecx, 0x10` clamp only here (both sites); matched
// 0x182b90 compiles the same PIN guard as ours (`cmp eax, 1; jge`, one load), so retail's three data loads come from another shape.
// M3 lane 12 r132 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238): the root's type in a named
// local before the test 238 (same code: still compared in memory, where retail loads it into ECX); the next node and its shape
// in locals declared ahead of nested tests 307 (1245 B); the origin as a const reference to the transform's position 381 (1208
// B); the hit as a const reference to the contact position 335 (1243 B); the contact normal published by aggregate assignment
// 271 (1243 B). Reading: in the key adjustment retail clamps with the constants in ECX and still compares ECX with EDX after
// `mov ecx, edx` on the unclamped path (the self-compare is not folded), so the compare's second operand was a separate read
// of the data word when the clamp was folded. Original body restored.
// Round 189 (m3 lane 6, fast with the file and 0x183da0/0x1cf280/0x182b90; control 238, masked 109): the root entity
// through `switch (root->type)` with case 1 and a NULL default 301 (a register load, but `dec ecx` for retail's `cmp
// ecx, 1`); the entity as a ternary with the type tested again (`root->type != 1 || !entity`) 346 (1255 B); one
// function-scope local for second's shape at all ten uses 350 (1251 B); the data test ahead of the kind call in the
// exclusion test 257 (1249 B); the first node's kind-4 test ahead of the root walk 342 (1243 B). Reading: retail's `mov
// ecx, [eax+0x18]; cmp ecx, 1` is the code matched 0x1388e0 shows for a field compared twice (one CSE'd load).
// Earlier context (238 in round 95's baseline, m3 lane 11 note near the end, m3 lane 2 r79 and m3 lane 8 r164 at the end).
// Shared plane-side distance with aggregate normal or setter, plane/collector scope 337/349 each;
// shared side predicate with the same publications and scopes 316/328 each. The shared publication
// shortens the body and keeps the 0x40 frame. The second node still needs retail's +0x20 slot.
// m3 lane 10 r115 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH throughout; control 238). KEPT, a
// behaviour fix at the same count (238, 1247 B): the three plane-normal rows written forward, left, up. VC orders a
// float sum's terms by its own ranking, not by the source: the old rows (up, left, forward) compiled to (forward +
// left) + up, where retail adds (up * n.k + left * n.j) + forward * n.i (0x182556-0x1825b4), a different rounding; the
// rows as written now compile to retail's association (checked on the listing's expression trees). Still different,
// not kept (it raises the positional count by one): the plane distance compiles to ((z + y) + x) + scale * d, where
// retail adds ((position.z * k + position.y * j) + scale * d) + position.x * i (0x1825bd-0x182608); written `((z + y)
// + scale * d) + x` (or `x + (z + y + scale * d)`, or with the x term in its own `+=` statement: all the same code)
// it compiles to retail's association at 239 (one positional line shifts; aligned 180 either way). The second arm's
// side test already adds as retail does (0x18267b-0x1826a7). New classifier forms, all worse: the 1..16 clamp through
// an inline pin taking const references and returning a const reference (the three reads then go through real stack
// temporaries), on a cast temporary 374 (1287 B), with the arms written value-first 374 (same code), bound to the data
// field 370 (1282 B), on a named local compared with itself 396 (1285 B), on a named local compared with the field 374
// (1287 B, other code); the same pin returning by value 238 (same code as the body). Reading: retail's `mov edx,
// [eax+8]` after the zero test, its constants in the result register and the unfolded `mov ecx, edx; cmp ecx, edx`
// say the clamp read the data through a form the first test's read was not merged with; a reference return is not it.
// Shifted upper value, split words, packed fields into longs and direct packed fields:
// 238/311/238/316; with material declared before plane processing 238/311/238/316.
// NOT kept: ties or regressions; the frame, second-node home and classifier reads still differ.
// AC564: Component property scan with end pointers < and !=, counted cursor,
// and indexed element pointer 327/327/339/341. Break with one result in the same
// forms 347/347/325/326. NOT kept: all raise the score and retain frame 0x40.
// The second-node spill, repeated classifier reads and plane homes remain.
// One material output with branch stores 250 or one later store 316, at
// either scope. Separate body material before/after the index lookups 261/258,
// also from branch entry 261/258. NOT kept: the shared output restores frame
// 0x44 but raises the score and grows the body; separate outputs also regress.
// DH393: Branch-owned plane with instance/level/both immutable returns 317/297/310.
// Both returned planes with shared pointer 310, branch-owned buffers with local/shared
// pointers 310/310, immutable returns 310, both with existing normal setter 305.
// NOT kept: frame 0x44 and 1243 bytes recover in several forms but every score regresses.
// Root classifier and output homes remain. Original collector restored.
// AC572: Full key selected by conditional or assigned in its two arms 238/238,
// optional unsigned adjustment set by branch or conditional 328/315. Same
// forms indexing with the validated clamp result 339/339/353/342.
// NOT kept: the frame and second-node spill remain. The distinct forms grow the body.
// Chained entity-index defaults 237; role names for entity indices/coefficients/both
// 238/238/238, with chained defaults 237/237/237. Separate named coefficients 238.
// NOT kept: the one-point gain leaves frame 0x40 and the second-node slot unchanged.
// Packed-contact shape reference 327. Material before shape or after data 327/327.
// Immutable shape pointer 327. Shared receiver used in the plane arm or both arms 327/327.
// Separate material-arm shape references 327. Shared receiver and early short material 328.
// NOT kept: the 0x44 frame form still spills the nodes to different slots and raises the score.
// Named signed distance in the instance/world/both plane arms 238/238/238;
// named projection in the same arms 238/238/238; all three products or both
// projection and distance stages 238/238. The frame and origin homes are unchanged.
// M3 lane 10 r119 (fast with the file; control 238), all in the 1..16 classifier: the data read through a `long const *`
// view 238 (same code: the loads still merge); `1 > value ? 1 : (16 < value ? 16 : value)` 238 (same code); `value > 16
// ? 16 : value` then `if (limited < 1) limited = 1;` 238; `1 > value ? 1 : value` then `16 < limited ? 16 : limited`
// 238; `limited = 1; if (!(limited > value)) { limited = 16; if (!(limited < value)) limited = value; }` 320 at retail's
// 1243 B (the bounds then sit in ECX as retail's `mov ecx, 1` / `mov ecx, 0x10`, but the compare is `cmp eax, ecx` on a
// copy of the zero test's load where retail reloads the data into EDX and compares `cmp ecx, edx`). Reading: matched
// 0x1c5400 and 0x1d4880 compile the same test with immediate compares and one load, so retail's three loads here go with
// the bounds-in-register form (the zero test's ECX is taken by the bound, so the data is reloaded, not copied).
// M3 lane 7 r179 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH each time; control 238, aligned 188, addresses
// masked 138). Reading: retail's three `cmp ecx, edx` in the 1..16 test have the same operands, the bound in the result register
// and the data in EDX, so the 1 and 16 arms were threaded past the final compare after the code was laid out (their `mov ecx`
// loads are dead), and the data is a memory operand loaded once per `&&` operand, not a copy of the zero test's load. Tried:
// the exclusion test through a helper that writes a bool through a pointer (stores between the reads) 333 (1255 bytes, one
// load); `limited = 1;` declared ahead of a `value` local with `!(limited > value)` tests 326; the same tests reading
// `shape->data` at each use, no value local 321 (`mov ecx, 1; mov eax, edx; cmp eax, ecx; jl`: the bounds are in ECX but the
// data is a copy of the zero test's load and the operands are swapped); those through a `long const &` to the field 320 (1244
// bytes); the at-each-use tests inside a multi-return bool helper on the node 263 (1247 bytes: retail's `setne al; test al, al`
// as well, still one load). Left as before: the zero test's load is shared with the classifier's in every form.
// Second node first 245, scoped root and entity 238, nested data guards 238,
// bound-specific classifier returns 336, validated result key 331,
// entity arm first 310 at 1243 bytes, named coefficient outputs 238,
// first-node scope ending at input capture 245. NOT kept: frame 0x40 and
// the second-node spill remain in all eight forms.
// M3 lane 3 r263 (fast; base 238; compile-only snippet harness for the classifier): every ternary, MIN/MAX nesting and if chain
// compares against an immediate; only the preset form (`limited = 1; if (limited <= value) { limited = 16; if (limited >= value)
// limited = value; }`, or a pin helper with that body) keeps the constants in the result register, and it compares
// `cmp value, limited` where retail has `cmp limited, value`. In the build: the preset clamp 320 (1243 B); the clamp compared
// with its own `value` 320 (1243 B); the classifier written inline on `second->shape->data` 320 (1243 B); the three early
// rejections after the component scan as one `||` chain 238 (same code); the component scan with a result local and `break`
// 333 (1257 B).
// M3 lane 4 r217 (fast with the file's other seven functions, all MATCH; control 238, aligned 138, masked 63). Kept: the 0x2001
// property scan through include/unknown_1cec30.h's existing havok_entity_property_get in place of the hand-written private
// copy (same code, 238; the copy and its two view types removed; the root walk stays written out, as 0x183da0's file is
// built /GL- and cannot be inlined). Tried: the exclusion's data test through a `dword const *` to the field 318 (still one
// data load: retail reads it three times, as three LTCG-inlined helpers would); the kind-24 shape in a local declared after
// the kind test and used for the material call 327 (the shape then takes EBX and its spill like retail's, but the second
// node moves to ESI where retail keeps both in EBX and one slot); that with the non-24 arm written first 330; the root's
// entity named once (`root->type == 1 ? root->entity : 0`) 238 (same code).
// Shared clamp with direct field reads, named or direct result and unsigned or signed
// data member: four at 238. Each with the short material output beside the input
// snapshots reaches 230 and frame 0x44. NOT kept: the declaration moves the first
// node out of its retail slot and does not recover the second-node and origin reuse.
// M3 lane 7 r187 (fast with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238). Reading: the byte test at
// +0x1e with `setne al; test al, al` occurs in retail only inline (here, 0x1c5400 and 0x1d4880; no standalone copy), and
// matched 0x1c5400 loads the data word once for its zero test, clamp and byte test, where this body loads it three times.
// Tried: the byte test through a file-local inline bool helper on the shape 238 (same code), the same helper `static`
// without forceinline 238 (same code); the clamp's data bound to a `dword const &` 294 (1249 B, still one data load);
// the exclusion's last three tests nested under `if (component != NONE && kind != 24)` 238 (same code); the clamp's
// ternary written upper bound first 238 (same code).
// M3 lane 10 r126 (fast with the file's seven matched functions, all MATCH; control 238, aligned 180). Compile-only
// frame screen (the asm lists each local's home): ours puts origin (9 references) below hit (6), retail hit below a
// 12-byte home that keeps only origin x, with y and z in shared 4-byte cells; origin by components (x, y, z or z, y, x),
// with the hit by components and published by assignment, three real snapshots, a setter, and the hit declared first
// all leave origin whole. Tried: origin from a forceinline accessor returning the point by value 355 (1284 B); both
// plane-side tests through a forceinline helper taking the point by value 391 (1298 B); the snapshot distance as
// radius + distance 238 (same code); the property scan on `root->type == 1 ? root->entity : NULL` 238 (same code);
// `valid` declared after the indices 238 (aligned 182).
// Classifier outside result from successive tests, positive inline-index result, upper test on
// the bounded value, enum root kind, admitted entity pointer, immutable node pointers, immutable
// origin and hit, and shared plane validity assigned by each side test: 336/238/238/238/238/238/238/331.
// NOT kept: The frame and second-node home still differ, as does the classifier data reload sequence.
// Packed-branch material receiver before data 238; before both snapshots 230;
// admitted upper result index 238; separate signed key guards 306;
// named radius contribution 248; immutable origin component view 238;
// plane-arm signed distances 238; material receiver between snapshots 230;
// exclusive result-count bound 307. NOT kept: The 230 form restores the frame size but shifts
// both saved node homes and remains four bytes long.
// M3 lane 5 r210 (fast with the file's seven other functions; control 238, register-masked 89): the dead second root walk
// ahead of the property get 341 (1243 B); the key read ahead of the kind-4 test 251; the key adjust as `(data - 1) * 8`
// 238 (same code); friction and restitution declared at function scope 238 (same code); the material output declared at
// function scope 230 (frame 0x44 as retail, first difference +0x1a: second/first homes +0x14/+0x18 for retail's
// +0x10/+0x20; a declaration moved by score, not kept); the 1..16 pin as a non-inline static function taking its bounds
// 238 (inlined at compile time, same code), with the material at function scope 230.
// M3 lane 15 r114 (fast with the file's other seven functions, all MATCH; control 238): the 1..16 classifier given the node
// and reading `node->shape->data` at each use (the sibling helper's accessor shape) 320 (1243 B; still one data load); the
// byte test as a bool helper on the shape 238 (same code: no setne); the classifier as a preset result compared with the
// value (`limited = 1; if (limited <= value) { limited = 16; if (limited >= value) limited = value; }`) 320 (1243 B: retail's
// `mov ecx, 1` / `mov ecx, 0x10` preloads and the `mov ecx, value; cmp ecx, value` end, but `cmp eax, ecx; jl` for retail's
// `cmp ecx, edx; jg` and the data copied from the first read instead of loaded again; aligned 179 against the control's 181),
// that spelled `!(limited > value)` / `!(limited < value)` 320 (same code), with the field read at each use 321. The
// positional count rises only because the block's rows shift; the data field is still loaded once.
// M3 lane 5 r213 (fast with the file's eight other functions, matched throughout; control 238, aligned 138): the origin
// copied member-wise z, y, x through a source pointer (retail's float copies, z first) 311 (aligned 127, frame still
// 0x40); the hit copied member-wise 323 (aligned 166); both 361 (1260 B); the distance sum written radius first 238 (same
// code); the valid flag and the two indices declared ahead of the fraction test 328 (1243 B, aligned 222); the hit copy
// after the distance 305 (1245 B, aligned 137).
// Independent hit coordinates with setter XYZ/YXZ 322/322, YXZ aggregate 355 and YXZ
// member stores 325. With material beside the defaults 324/324/357/321.
// NOT kept: the wider-scope material recovers 0x44 but the hit transport grows the body.
// The second-node home and classifier loads still differ.
// Scalar transformed plane offset 240; instance projection or signed distance 238 each; scalar offset plus signed
// distance 240; direct projection, both projections or both signed distances 238 each; scalar-coefficient side
// test before the normal stores 240. NOT kept: all retain frame 0x40 and the original length.
// Late result bindings for found/position, normal/fraction, entity indices (subsets 1 through 7):
// 270/268/279/236/277/275/378. One final result binding 396. NOT kept: the entity-index binding
// gains two at 1244 bytes and retains frame 0x40. The second-node slot and frame still differ.
// M3 lane 16 r158 (fast with 0x1829b0, 0x1821d0, 0x182800, all MATCH; control 238), the 1..16 classifier helper's
// body only: `limited = 1; if (limited <= value) { limited = 16; if (limited >= value) limited = value; }` 320 (1243 B:
// retail's `mov ecx, 1` / `mov ecx, 0x10` preloads, but `cmp value, limited` where retail compares `limited, value`, and
// still one data load); the same reading the data field at each compare 321; as early returns of the comparison 336
// (1228 B); the tests as `!(limited > value)` / `!(limited < value)` 320, as empty-then arms 320 (the compare operands
// stay value-first in every spelling).
// M3 lane 8 r204 (fast with the file's 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238): the hit distance
// accumulated (`distance = contact->distance; distance += radius;`) 248; both plane-side tests with the zero on the left 238
// (same code); the fraction test as `!(distance < fraction)` 238 (same code); the final test as `!(normal.k <= scale)` 240;
// the position published by aggregate assignment alone 340 (1227 B), with the hit snapshot by components 310 (1249 B).
// M3 lane 3 r282 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238): 1 to 7 dead
// longs at the top 238/238/238/238/239/238/238 (the symbol-count phase does not reach these integer compares or
// the frame); the preset 1..16 clamp with 0 to 7 dead longs 320 x7, 321 at 5 (the compare stays value-first);
// origin copied member-wise x, y, z from the transform 308 (1245 B), with the hit member-wise too 361 (1258 B);
// the decode's top bits in a named `kind` 238 (same code, the shift still folds into the mask compare).
// M3 lane 16 r162 (fast with 0x1829b0, 0x1821d0, 0x182800, all MATCH; control 238). Diagnostic (reverted, a compiler
// memory barrier is not a source form): the exclusion as a bool helper on the shape (zero test, barrier, preset clamp read
// from the field, barrier, byte test) 295 (1253 B) gives retail's three data loads, the `mov ecx, 1` / `mov ecx, 0x10`
// preloads and `setne al; test al, al`, but still compares value-first (`cmp eax, ecx; jl` for retail's `cmp ecx, edx;
// jg`); on it 1..8 initialised dead longs at the top of the helper or of the body 295 (one 296): the compare operand order
// is no symbol-phase effect; the clamp as a const-left ternary 331, through a pin(value, lower, upper) helper written
// `lower > value ? ...` 331 (both canonicalised to immediates); the preset on a named value declared after or before
// `limited` 258/258 (the equality keeps the source operand order, the two relational tests stay value-first). Real forms:
// the same two named-value helpers without the barrier 320/320; the body nested under `if (root->type == 1 &&
// root->entity)` as matched 0x1c5400 writes it 238 (same code); the entity through `entity = NULL; if (type == 1) entity
// = root->entity; if (!entity) return;` 238 (same code). Matched 0x1c5400 compiles the root test as `cmp dword ptr
// [eax + 0x18], 1` and immediate clamps, so this body's register load of the type, kept dead walk and unpropagated
// clamp bounds all point to one shape no natural form here reproduces.
// M3 lane 11 r144 (h2m fast with the file's seven other functions, 0x1c5400 and 0x1d4880, all MATCH unless said; control
// 238, aligned 180, masked 126). The 1..16 classifier only, new forms: a pin taking const references and returning by
// value (bound-first, value-first, upper-less-than spellings, a named result) 238 (same code); returning a const
// reference 374 (1287 B: the bounds become stack cells and the result an address); a template of that 386; pointer
// bounds to const locals (0x182800's shape) 238; a by-value one-member aggregate pin 238, with a converting
// constructor 382, returning long 248; a class-typed index with comparison operators 362-396; reference-returning
// minimum and maximum helpers over the field's address 365 (the field is then read three times, but the bounds stay in
// stack cells), returning by value 322; the data member overlaid as dword, long and byte pointer and read by role 238, with
// the preset pin 321, with a multi-return bool helper for the three tests 318-322 (still one data load: differently
// typed reads of one address are merged); the bounds as const objects (file-static or extern, by value or by
// reference, defined here or in src/unknown_1c25a0.cpp) 238 (folded to immediates). Calls to the existing helpers of
// src/unknown_1c25a0.cpp across files: its exclusion predicate contact_shape_has_material for the whole component
// test 252 (aligned 183, masked 123: retail's setne/test, still one load and immediates); its pin by value 238 (same
// code); that pin changed to const references 322 and 0x1c5400 then differs. Diagnostics (reverted): the bounds read
// from non-const objects (file-static, or globals defined in another file) 324 at 1246 B give retail's compare shape
// exactly (`mov ecx, <lower>; cmp ecx, value; jg; mov ecx, <upper>; cmp ecx, value; jl; mov ecx, value`, aligned 173,
// masked 114) with memory loads where retail has `mov ecx, 1` / `mov ecx, 0x10`: retail's bounds were operands the
// compiler could not fold when it shaped the compare and knew as constants afterwards; a pin as a plain function in
// another file stays a call (334); 64-512 dead locals 238/238/238/249 (no change of the classifier); an inline asm
// block 332 (no change of it); global optimization off for the function (the one compiler-setting diagnostic) 1266.
// M3 lane 6 r219 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238): the head written as
// the entity getter's ternary (`entity = root->type == 1 ? root->entity : NULL; if (!entity) return;`) 238; the type and the
// entity tested in two early returns through an entity local 238; the entity preset NULL and set under `if (root->type ==
// 1)` 238; the dead second walk's result assigned back to `root` 238 (all the same code: the type is still compared in
// memory); `switch (root->type)` with `case 1: break; default: return;` 301 (register load, `dec ecx`). Body unchanged.
// After distance rejection, defer origin only/hit only/both origin first/both hit first
// 237/306/306/306. Scope under the accepted distance test in the same order
// 237/306/306/306. NOT kept: deferred origin gains one at frame 0x40, and retail
// reads it before distance rejection. The second-node home still differs.
// M3 lane 7 r207 (h2m fast with 0x1829b0, 0x1821d0, 0x182800, 0x183670, all MATCH; control 238): diagnostic (never
// kept) global optimisation off around the body 1266 (1997 B, an EBP frame), so the head's register-held clamp bounds
// and data reloads are not that; the second node's shape in a local in the kind-24 arm 327 (1249 B); one shape local
// for the whole body 259 (1253 B); friction and restitution at function scope 238 (same code); the 1..16 helper
// comparing the clamp with the value it read 320 (1243 B), as `value < 1 || value > 16` 355 (1210 B).
// M3 lane 15 r129 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800, 0x183670, all MATCH; control 238): a shape local formed
// at the kind-24 test and used by both arms' material calls and the data read 362 (1206 B); the root's type through a
// `long const &` 238 (same code: still compared in memory); the 1..16 classifier as a preset result bound by reference
// (`limited = 1; long &result = limited; if (!(result > data)) { result = 16; if (!(result < data)) result = data; }`,
// the field read at each use) 321 (preloads as retail, compares still value-first, one data load); the classifier's
// bounds read from a file-static const two-member bounds aggregate 238 (folded to immediates); both plane side tests
// through the existing plane_distance_to_point 238.
// M3 lane 7 r209 (h2m fast with 0x183670, MATCH; control 238). Reading: retail's kind-24 arm loads the second node's
// shape into EBX and spills it into the second node's own home (+0x24 after the fourth push), passing it to 0x1ee410 in
// EDX; ours keeps the second node there. Tried: the 1..16 helper's bounds from a two-element local array set to 1 and 16
// (diagnostic) 238 (folded to immediates, same code); the helper's result through an if/else bool 238; the exclusion's
// data word named inside `if (component != NONE && kind != 24)` 238; the material buffer declared at the kind-24 arm's
// top 238 (all the same code); the arm-local shape for the decode and the 0x1ee410 call (re-measured for the slot
// reading above) 327 (1249 B).
// M3 lane 1 r152 (h2m fast, control 238): the kind decode named (`long kind = data >> 29;` then `if (kind != 1)`) 238
// (same code); `short material` declared at the top of the kind-24 block (ahead of `data`) 238 (frame stays 0x40); the
// two NONE outputs declared bare and set by one chained `index = other_index = NONE;` 237 (one point, under the bar);
// the hit read through real_point3d_set and published by aggregate assignment (retail's dword copy-out) 309 (1249 B);
// origin and hit declared `point3f const` 238 (same code).
// M3 lane 5 r230 (h2m fast with 0x183670, MATCH; control 238). Lead, not kept (a declaration moved, kept only with a
// match): `short material;` declared at the top of the body 230 (frame 0x44 as retail; first and second then sit at +8 and
// +0xc of the frame where retail has +4 and +0x14). On that lead: a kind-24 shape local for the data read and the material
// call 328 (1249 B); origin by components z, y, x from a position pointer 312; the hit through real_point3d_set with
// aggregate publication 311 (1249 B); friction and restitution declared beside the material 230 (same code). On the kept
// body: the plane declared at the top of the body 238; the entity declared at the top 238; `valid` declared after the two
// indices 238; the instance arm's side test written x, y, z 238 (same code each).
// M3 lane 12 r176 (h2m fast, control 238): origin as a brace initialiser through a position pointer 312 (1248 B);
// origin as three reals read z, y, x through a position pointer 310 (retail's load order, homes still packed in a 0x40
// frame); that with the hit through real_point3d_set and aggregate publication 348, with only the aggregate publication
// 350 (1243 B), with the hit read member-wise y, x, z and aggregate publication 346; origin through real_point3d_set from
// the position pointer re-measured 251 (1260 B: x, y, z loads into the one 12-byte home, no member leaves it). Body unchanged.
// M3 lane 16 r175 (H2M_FAST with 0x1829b0/0x1821d0/0x182800, all MATCH; control 238): retail's register-preset 1..16
// classifier occurs nowhere else in the retail text, and the 0x1e byte test only here and in matched 0x1c5400/0x1d4880 (both
// compiled as ours), so no existing out-of-line helper carries it. The origin through a `point3f const *` to the transform's
// position in both plane tests 381 (1208 B); the distance as `radius + contact->distance` 238 (same code); the root's type in
// a `long` local 238 (same code); the hit copy after the distance 305 (1245 B); the origin copy after the hit copy 237.
// M3 lane 6 r231 (h2m fast with 0x1829b0/0x1821d0/0x182800/0x183670, all MATCH; control 238): both plane-side tests as
// `valid = side > 0.0f; if (valid) ...` 331 (1253 B); with the normal copy hoisted after both arms 345 (1218 B); the kind-9
// key test with the next node's shape named for its null test and kind call 237 (1246 B, one point, under the bar), with the
// key's data read through it too 307 (1245 B: retail re-reads the shape there); the root test in the positive sense around
// the rest of the body 238 (same code). Body unchanged.
// M3 lane 7 r218 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238). Reading: retail
// gives the origin a 12-byte home at +0x38 of which only x is used (+0x3c/+0x40 are the two untouched dwords), y and z
// moving to shared spill slots, the same first-member-only home as 0x22cf0's offset. Tried: the instance arm's side test
// through plane_distance_to_point 238, the record arm's 238 (same code each); the origin's members assigned z, y, x from a
// position pointer 311 (movss copies into one contiguous home, frame 0x40); that with the instance arm through
// plane_distance_to_point 311; the hit's members assigned y, x, z 323 (1262 B). Body unchanged.
// M3 lane 6 r235 (H2M_FAST with 0x1829b0/0x1821d0/0x182800/0x183670, all MATCH; control 238, aligned 180, masked 104):
// the kind test as `if ((data >> 29) - 1)` 238 at retail's 1243 B (masked 102: retail's `shr 0x1d; dec`, but the surface
// index is still computed inside the instance arm where retail computes it ahead of the branch); the hit distance as
// `first->shape->radius + contact->distance` 238; the normal test as `scale < contact->normal.k` 238; the entity test as
// `root->entity == NULL` 238 (same code each); the fraction test as `results[key].fraction <= distance` 304 (1243 B).
// Reading: retail copies the origin and the hit with float moves into separate cells (origin z +0x18, y +0x14, x +0x38;
// hit +0x2c..+0x34), ours with dword moves into two 12-byte aggregates. Body unchanged.
// M3 lane 3 r303 (H2M_FAST with 0x1829b0/0x1821d0/0x182800/0x183670, all MATCH; control 238, aligned 180): the 1..16
// classifier with its result preset to each bound over one named value (`limited = 1; if (!(limited > value)) { limited =
// 16; if (!(limited < value)) limited = value; }`, compared with the value) 326 at retail's 1243 B (aligned 215: the
// `mov ecx, 1` / `mov ecx, 0x10` preloads, but `cmp eax, ecx; jl` for retail's `cmp ecx, edx; jg`, the third arm folded and
// one data load); on it 0 to 40 initialised dead longs at the top of the body 326 at every count but 26 to 28 (327): the
// compare's operand order is no symbol-count state up to 40 either. A clamp taking and returning `long const &` with
// separate returns, called on a named copy of the data, 347 (1275 B: the bounds become stack cells and the result an
// address, as listed for the ternary form); the classifier as a plain function for the link step to expand 377 (1212 B,
// it stays a call, with the ternary or the preset body); a plain by-value clamp called from the inline classifier 238
// (expanded and folded to the kept code). Reading of retail's exclusion test: the shape pointer stays in EAX and the data
// is loaded three times from it (into ECX for the zero test, into EDX for the classifier, into EAX for the byte test), so
// the classifier compares its ECX bound with EDX; ours keeps one load in EAX or EDX for all three. Body unchanged.
// M3 lane 16 r184 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238, aligned 180, masked
// 104). Frame packer reading: this build packs valid, first (shared with material and restitution), second (shared with
// friction), index, other_index, distance, then the origin (12 bytes, 9 references) below the hit (12 bytes, 6); retail
// has the hit below the origin and two more 4-byte cells for origin.y and origin.z, so retail's 12-byte origin object
// carries only x's references. In every form below the origin stays one 9-reference object (frame 0x40). Tried: the
// origin through a file-local point-copy helper 251 (1260 B), the hit through it too 347, with both plane tests through
// plane_distance_to_point 251; x stored directly with y and z through a pointer to the origin 315 (1256 B), that with
// both tests through plane_distance_to_point 315, written z, y then x 314; all three stored through the pointer with
// those tests 251; the origin's members assigned z, y, x from a position pointer with those tests 311 (aligned 170,
// masked 81); both tests through plane_distance_to_point alone 238 (same code); the hit's members assigned y, x, z 323;
// the origin through real_point3d_set from the position 251, from three casts 318. Body unchanged.
// The frame is still 0x40 and the second-node home is still displaced. Separate origin Y 308, Z 309, and both
// 310; extract Y and Z from a full snapshot 305, extract X 302. With the material receiver beside the output
// indices, split Y 311, Z 312, and both 313. Single-coordinate splits restore frame size but change the
// second-node spill and raise the score.
// M3 lane 6 r240 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238, aligned 138). Reading
// of retail's copies (0x1824d8 on): the origin's z is loaded first, straight from the transform (+0x58), and stored to its
// own cell, then y to the cell below it, then x to the first slot of the 12-byte home; the hit's z, y and x go to one
// aggregate in that order, loaded y, x, z. Both store orders are those of three scalar arguments evaluated right to
// left, and the origin's y and z live where by-value arguments would, not in the aggregate. Tried on that reading: the
// origin through real_point3d_set from a position pointer, re-measured, 251 (1260 B: loads and stores run x, y, z into
// one home), with both plane tests through plane_distance_to_point 251 (same code), the hit through it as well 347
// (1273 B) with or without those tests; origin (and hit) through a file-local forceinline wrapper taking the three reals
// by pointer and calling real_point3d_set, one level deeper, 251 / 347 (the same code as the direct calls); the origin
// returned by value from a member-wise copier defined in another file, so only the link step expands it (diagnostic,
// removed) 355 (1284 B: float stores to a second home, then a dword copy). Body unchanged.
// The frame is still 0x40 and the second-node home is still displaced. Separate origin Y 308, Z 309, and both
// 310; extract Y and Z from a full snapshot 305, extract X 302. With the material receiver beside the output
// indices, split Y 311, Z 312, and both 313. Single-coordinate splits restore frame size but change the
// second-node spill and raise the score.
// Round 467: component-index defaults in each arm, before/after mesh material lookup
// 251/306. Alternate-index defaults 253/308. Both defaults after mesh material with
// entity else defaults, pre-test defaults or conditional outputs 313/315/313. Both
// conditional outputs with defaults before mesh material 260. Late defaults shrink
// the frame to 0x3c or 0x38, farther from retail's 0x44.
// Friction/restitution/both before origin 238/238/238; either before hit 238/238;
// friction before origin and restitution before hit 238. Either early coefficient with
// mesh material before hit 230/230. NOT kept: frame 0x44 agrees, but first/second
// node homes become +0x14/+0x18 instead of retail's +0x10/+0x20.
// M3 lane 15 r145 (H2M_FAST with 0x1829b0, 0x1821d0, 0x182800 and 0x183670, all MATCH; control 238). Frame packer
// reading with retail's cells counted: retail's four-byte objects pack first, origin y, origin z, index, other_index,
// second, distance (y and z carry three references each and are their own objects; `second` has two home references,
// its store at entry and its reload in the non-24 arm, because the kind-24 arm replaces it in EBX with the shape, which
// is spilled into the same slot), then the hit and a 12-byte origin that carries only x's three references. Tried: the
// origin through a file-local force-inlined setter taking x, y and z by `real const &` 251 (1260 B, the same code as
// real_point3d_set: one home), with the hit through it too and aggregate publication 334; a kind-24 shape local for the
// data read and the material call with aggregate publication 359 (1261 B), that with `short material` at the top of the
// body 359; the material at the top with aggregate publication 330 (1227 B). Body unchanged.
// M3 lane AO r1 (fast; control 238): the instance plane's normal through vector_products.h's vector3f_rotate_into_axes
// on the instance rotation, in place 427 (1130 B), into a separate vector then copied back 420 (1138 B: the shared
// helper is then not expanded the way retail's rows are); the key's upper bound named once (`long last = count - 1`)
// 238 (same code); `valid` declared after the two indices 238 (same code); the distance computed ahead of the origin and
// hit copies 305 (1245 B); one `shape` local for the second node's shape in the kind test, the data read and both
// material calls 362 (1206 B); the found flag stored last in the result block 255 (1251 B); the key clamp as `limited =
// key` with an if/else-if 238 (same code). Body unchanged.
// The 0x40 frame, second-node spill and root-kind comparison still differ.
// Query/target node names 238; distance and index names 238; shared admitted root
// entity 238; const hit/origin copies 238/238; instance normal setter 238, with
// const coefficients 238; direct plane normal setter 264.
// NOT kept: the direct setter adds six bytes and raises the score.
// @retail 0x1822f0
void c_query_batch_collector::collect(s_batch_contact const *contact)
{
	s_batch_contact_node *first = contact->first;
	s_batch_contact_node *second = contact->second;
	s_batch_contact_node *root = batch_contact_root(second);
	if (root->type != 1 || !root->entity) return;
	long component = havok_entity_property_get(root->entity, HAVOK_PROPERTY_COMPONENT_INDEX);
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
				/* VC sums these three products last term first: written forward, left, up it adds (up + left) + forward,
				   retail's order (0x182556-0x1825b4) */
				real normal_i = matrix->forward.i * plane.n.i + matrix->left.i * plane.n.j + matrix->up.i * plane.n.k;
				real normal_j = matrix->forward.j * plane.n.i + matrix->left.j * plane.n.j + matrix->up.j * plane.n.k;
				real normal_k = matrix->forward.k * plane.n.i + matrix->left.k * plane.n.j + matrix->up.k * plane.n.k;
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
