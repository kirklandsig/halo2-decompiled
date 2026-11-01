// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query of one shape against a shape
// holding a tree of parts: the first shape's bounds, taken into the tree's
// frame, give the keys of the parts they overlap, and every part a filter
// accepts is passed with the first shape to the query function for the two
// kinds. Each phase is marked in the timing stream.
#include "reference_counted_2d88a0.h"
#include "physics_allocator.h"
#include "child_query_2f9950.h"
#include "timer_mark_2ebc80.h"
#include "pair_agents_2eab70.h"
#include "side_bounds_2e9140.h"
#include <xmmintrin.h>

__forceinline void timer_add_2ea940(char const *label)
{
	timer_mark_2ebc80 *mark = (timer_mark_2ebc80 *)g_480154;
	bool const room = (char *)mark < g_480158;
	if (room)
	{
		mark->label = label;
		unsigned long time;
		__asm
		{
			rdtsc
			mov time, eax
		}
		mark->time = time;
		g_480154 = (char *)(mark + 1);
	}
}

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
	void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

// where a body sits: two vectors, then its transform
struct placement_2ea940
{
	__m128 field_0;
	__m128 field_10;
	c_query_transform transform;
};

// a box given by its two corners
typedef bounds_2e9140 bounds_2ea940;

// the keys of the parts a query found, held in place for up to 128 of them
struct key_array_base_2ea940
{
	long *data;
	long size;
	long capacity_and_flags;

	key_array_base_2ea940(long *buffer, unsigned long capacity)
		: data(buffer), size(0), capacity_and_flags(capacity | 0x80000000) {}
	~key_array_base_2ea940()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			havok_allocator()->release(data,
				(capacity_and_flags & 0x7fffffff) * 4, 0x12);
		}
	}
};

struct key_array_2ea940 : key_array_base_2ea940
{
	long storage[128];

	key_array_2ea940() : key_array_base_2ea940(storage, 128) {}
};

class shape_2ea940
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	// the shape's kind
	virtual long kind() const { return 0; }
	// the shape's bounds under a transform, widened by a tolerance
	virtual void bounds(c_query_transform const *transform, float tolerance, bounds_2ea940 *out) const {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	// the keys of the parts whose bounds overlap a box
	virtual void overlapping_keys(bounds_2ea940 const *bounds, key_array_2ea940 *keys) const {}
	// a container's part, built in the buffer given when it must be
	virtual shape_2ea940 const *part(long key, char *buffer) const { return 0; }
};

// a shape holding a tree over the parts of a container
class tree_shape_2ea940 : public shape_2ea940
{
public:
	long field_4;
	long field_8;
	shape_2ea940 const *container;
};

// the one-byte answer of a filter, returned through a hidden pointer
class answer_2ea940
{
public:
	answer_2ea940() {}

	char value;
};

struct input_2ea940;

// whether one part of a container may meet the other side
class filter_2ea940
{
public:
	virtual answer_2ea940 accepts(input_2ea940 const *input, body_2ea940 const *other, body_2ea940 const *container_body, shape_2ea940 const *container, long key) const = 0;
};

typedef void (__cdecl *pair_query_2ea940)(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);
// a query along a path, which also reports where the sides start touching
typedef void (__cdecl *path_query_2ea640)(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector, void *second_collector);

// per-kind flags, then tables of query functions indexed by the kinds of the
// two sides
struct dispatcher_2ea940
{
	char unknown0[0x218c];
	pair_query_2ea940 queries_218c[32][32];
	path_query_2ea640 path_queries_318c[32][32];
};

// a query's input: the dispatcher, the filter and the tolerance
struct input_2ea940
{
	dispatcher_2ea940 *dispatcher;
	filter_2ea940 *filter;
	float tolerance;
};

class tree_part_2ea830
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	// a container's part, built in the buffer given when it must be
	virtual tree_part_2ea830 const *part(long key, char *buffer) const { return 0; }
};

// runs the queries of one part of a pair
class pair_agent_2ea410
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2(void const *a, void const *b, void const *input, void *collector) {}
};

struct part_body_2ea830
{
	tree_part_2ea830 const *shape;
	long key;
	void const *placement;
	void const *parent;
};

// the same query along a path: the first shape's bounds, stretched along the
// path, give the parts, and each part a filter accepts is passed on to the
// path query for the two kinds
// @retail 0x2ea640
void __cdecl function_2ea640(void const *a_, void const *b_, void const *input_, void *collector, void *second_collector)
{
	body_2ea940 const *a = (body_2ea940 const *)a_;
	body_2ea940 const *b = (body_2ea940 const *)b_;
	input_2ea940 const *input = (input_2ea940 const *)input_;
	timer_add_2ea940("LtBvTree");
	bounds_2e9140 bounds;
	function_2e9140(a, b, input, &bounds);
	tree_shape_2ea940 const *tree = (tree_shape_2ea940 const *)b->shape;
	key_array_2ea940 keys;
	tree->overlapping_keys(&bounds, &keys);
	timer_add_2ea940("StNarrowPhase");
	long const kind_a = ((shape_2ea940 const *)a->shape)->kind();
	body_2ea940 child;
	long const *key = keys.data;
	long const *const end = key + keys.size;
	shape_2ea940 const *container = tree->container;
	child.parent = b;
	child.placement = b->placement;
	__declspec(align(16)) char buffer[0x100];
	for (; key != end; ++key)
	{
		if (input->filter->accepts(input, a, b, container, *key).value)
		{
			shape_2ea940 const *shape = container->part(*key, buffer);
			geometry_2ea640 const geometry = { shape, *key };
			static_cast<geometry_2ea640 &>(child) = geometry;
			long const kind_b = shape->kind();
			path_query_2ea640 query = input->dispatcher->path_queries_318c[kind_a][kind_b];
			query(a, &child, input, collector, second_collector);
		}
	}
	timer_add_2ea940("lt");
}
// the agents of the tree's parts brought up to date, then each runs its
// query against its part
// @retail 0x2ea830
void class_2ea830::function_2ea830(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("LtBvTree");
	((class_2e96a0 *)this)->function_2e96a0(a, b, input, function_2e90f0);
	timer_add_2ebc80("StNarrowPhase");
	agent_entry_2ea410 const *const end = entries + entry_count;
	part_body_2ea830 const *container_body = (part_body_2ea830 const *)b;
	part_body_2ea830 part;
	part.placement = container_body->placement;
	part.parent = container_body;
	tree_part_2ea830 const *container = (tree_part_2ea830 const *)((tree_shape_2ea940 const *)container_body->shape)->container;
	__declspec(align(16)) char buffer[0x100];
	for (agent_entry_2ea410 const *entry = entries; entry != end; entry++)
	{
		part.shape = container->part(entry->key, buffer);
		part.key = entry->key;
		entry->agent->slot_2(a, &part, input, collector);
	}
	timer_add_2ebc80("lt");
}
// @retail 0x2ea940
void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector)
{
	timer_add_2ea940("LtBvTree");
	c_query_transform a_in_b;
	c_query_transform const *transform_b = &((placement_2ea940 const *)b->placement)->transform;
	c_query_transform const *transform_a = &((placement_2ea940 const *)a->placement)->transform;
	a_in_b.inverse_product(transform_b, transform_a);
	bounds_2ea940 bounds;
	((shape_2ea940 const *)a->shape)->bounds(&a_in_b, input->tolerance, &bounds);
	tree_shape_2ea940 const *tree = (tree_shape_2ea940 const *)b->shape;
	key_array_2ea940 keys;
	tree->overlapping_keys(&bounds, &keys);
	timer_add_2ea940("StNarrowPhase");
	long const kind_a = ((shape_2ea940 const *)a->shape)->kind();
	{
		body_2ea940 child;
		long const *key = keys.data;
		long const *const end = key + keys.size;
		child.parent = b;
		child.placement = b->placement;
		shape_2ea940 const *container = tree->container;
		__declspec(align(16)) char buffer[0x100];
		for (; key != end; ++key)
		{
			if (input->filter->accepts(input, a, b, container, *key).value)
			{
				shape_2ea940 const *shape = container->part(*key, buffer);
				geometry_2ea640 const geometry = { shape, *key };
				static_cast<geometry_2ea640 &>(child) = geometry;
				long const kind_b = shape->kind();
				pair_query_2ea940 query = input->dispatcher->queries_218c[kind_a][kind_b];
				query(a, &child, input, collector);
			}
		}
	}
	timer_add_2ea940("lt");
}
