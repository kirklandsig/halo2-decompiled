// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a kind of bounded object
// (which takes the value as its fourth argument), and a query against a
// container's triangles that keeps one contact for each plane they share.
#include "bounded_2e9530.h"
#include "growable_array_2d91d0.h"
#include "pair_agents_2eab70.h"
#include "pair_agent_2ed170.h"
#include <xmmintrin.h>

class bounded_2fb0f0 : public bounded_2e9530
{
public:
	bounded_2fb0f0(long value_) : bounded_2e9530(value_) {}
};

// @retail 0x2fb0f0
bounded_2fb0f0 *__cdecl function_2fb0f0(long arg_0, long arg_1, long arg_2, long value)
{
	return new bounded_2fb0f0(value);
}

// a shape: its kind, and for a container, its parts; a shape that wraps
// another holds it in its fourth word
class shape_2fb150
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long kind() const { return 0; }
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	// a container's part, built in the buffer given when it must be
	virtual shape_2fb150 const *part(long key, char *buffer) const { return 0; }

	char unknown4[8];
	shape_2fb150 const *child;
};

// one side of a query: a shape, a key, where it sits, and its parent side
struct body_2fb150
{
	shape_2fb150 const *shape;
	long key;
	void const *placement;
	body_2fb150 const *parent;
};

struct settings_2fb150
{
	char unknown0[0xc];
	// how far behind a closer contact's plane another may lie and still be
	// dropped
	float plane_tolerance;
};

struct input_2fb150
{
	dispatcher_2ed170 const *dispatcher;
	long field_4;
	float tolerance;
	settings_2fb150 const *settings;
};

// the object kept for a part of the container side: the id its manager
// handed out for the part's contact (0xffff for none), and for a triangle,
// the terms that solve for a point's weights in its plane (0x2f0e70)
class part_agent_2fb150
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4(void const *a, void const *b, void const *input, void *collector) {}

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;
	unsigned short id;
	float terms[4];
};

// hands out and takes back the ids of contacts
class contact_manager_2fb150
{
public:
	virtual void slot_0() {}
	virtual unsigned short add(void const *a, void const *b, void const *input, void const *hit) { return 0; }
	virtual void remove(unsigned short id) {}
};

class agents_2fb150
{
public:
	char unknown0[8];
	contact_manager_2fb150 *manager;
	agent_entry_2ea410 *entries;
	long entry_count;
};

// a triangle part's contact: its point with the separation in the fourth
// lane, its normal, the part's agent, shape and key, and what the query
// found (0 none, 1 a contact, 2 one with the triangle's other closest parts)
// a vector, copied a lane group at a time
struct vector_2fb150
{
	__m128 value;

	vector_2fb150() {}
	vector_2fb150(vector_2fb150 const &other) : value(other.value) {}
	void operator=(vector_2fb150 const &other)
	{
		value = other.value;
	}
};

struct contact_2fb150
{
	vector_2fb150 point;
	vector_2fb150 normal;
	part_agent_2fb150 *agent;
	shape_2fb150 const *part;
	long key;
	long found;
};

struct hit_2fb150
{
	vector_2fb150 point;
	vector_2fb150 normal;
	unsigned short id;
	char unknown22[0x30 - 0x22];
};

struct hits_2fb150
{
	hit_2fb150 *data;
	long size;
	long capacity_and_flags;
};

// an array of 48-byte elements whose storage comes from the allocator's stack
// area (src/unknown_2faeb0.cpp)
struct array_2faeb0
{
	contact_2fb150 *data;
	long size;
	unsigned long capacity_and_flags;

	array_2faeb0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_2faeb0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(contact_2fb150), 0x12);
		}
	}
};

struct stack_array_2faeb0 : array_2faeb0
{
	contact_2fb150 *stack_block;
	long stack_capacity;

	stack_array_2faeb0(long capacity);
	~stack_array_2faeb0();
};

struct body_2f0f70;
struct input_2f0f70;
struct contact_2f0f70;
// the contact of a sphere with a triangle (src/unknown_2f0e70.cpp)
int __cdecl function_2f0f70(body_2f0f70 const *a, body_2f0f70 const *b, input_2f0f70 const *input, float const *terms, contact_2f0f70 *contact);

class triangle_terms_2f0e70;
struct triangle_2f0e70;
// makes the agent for a pair one of which is a triangle (src/unknown_2f0e70.cpp)
triangle_terms_2f0e70 *__cdecl function_2f0e70(description_2ed170 const *description, triangle_2f0e70 const **other, dispatcher_2ed170 **dispatcher, long arg_3);

// the agent that stands for none (src/unknown_301a40.cpp)
struct object_4805a4;
object_4805a4 *__cdecl function_301a40();

// a yes or no held in a byte
class answer_2fb150
{
public:
	answer_2fb150(bool value_) : value(value_) {}

	char value;
};

inline void swap_2fb150(contact_2fb150 &first, contact_2fb150 &second)
{
	contact_2fb150 const held = first;
	first = second;
	second = held;
}

// how far a contact lies in front of a closer one's plane
__forceinline float along_normal_2fb150(contact_2fb150 const *other, contact_2fb150 const *closer)
{
	__m128 const product = _mm_mul_ps(_mm_sub_ps(other->point.value, closer->point.value), closer->normal.value);
	float along;
	_mm_store_ss(&along, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return along;
}

// whether a contact only repeats a closer one's plane: it does not lie deep,
// and lies no further in front of the closer one's plane than the tolerance
__forceinline answer_2fb150 repeats_plane_2fb150(contact_2fb150 const *other, contact_2fb150 const *closer, float tolerance)
{
	bool const apart = other->point.value.m128_f32[3] < -0.1f ? true : along_normal_2fb150(other, closer) > tolerance;
	return answer_2fb150(!apart);
}

// a query against a container's triangles, each through its part's agent:
// the triangles' contacts are gathered, sorted closest first, those that
// only repeat a closer one's plane dropped, and the rest passed to the
// collector with ids from the contact manager
// score 330 (was 344): receiver register, body homes and plane branch still differ.
// Object/entry/body/output/selection references 344; const list view 344; settings pointer 452;
// one-exit answer 470; swap pointers 344; void vector assignment 330; direct plane tests 368;
// shared contact type 345, with void assignment 332; parent-first and swap assignment 330.
// On 330: body constructor 330; agent reads at use 427; shape reference
// 330; reused selection pointer 423; declaration swap 330; tolerance
// reference 369; vector-input references 330; implicit vector copy 496.
// Equality operands exchanged 330, with agent reads at use 427.
// New: immutable body/shape pointers 330; later receiver alias 330;
// input argument at use 330; entry cursor 470; final contact reference 364;
// minimum-search cursor 405; int dispatch ordinals 330.
// @retail 0x2fb150
void class_2fb150::function_2fb150(void const *a_, void const *b_, void const *input_, void *collector)
{
	body_2fb150 const *a = (body_2fb150 const *)a_;
	body_2fb150 const *b = (body_2fb150 const *)b_;
	input_2fb150 const *input = (input_2fb150 const *)input_;
	agents_2fb150 *agents = (agents_2fb150 *)this;
	body_2fb150 container_side;
	container_side.shape = b->shape->child;
	container_side.key = b->key;
	container_side.placement = b->placement;
	container_side.parent = b;
	((class_2e96a0 *)this)->function_2e96a0(a, &container_side, input, function_2e90f0);
	stack_array_2faeb0 contacts(agents->entry_count);
	shape_2fb150 const *container = container_side.shape;
	body_2fb150 part;
	part.parent = b;
	part.placement = b->placement;
	__declspec(align(16)) char buffer[0x100];
	for (long i = 0; i < agents->entry_count; i++)
	{
		agent_entry_2ea410 const *entry = &agents->entries[i];
		if (function_301a40() == (object_4805a4 *)entry->agent)
		{
			continue;
		}
		part_agent_2fb150 *agent = (part_agent_2fb150 *)entry->agent;
		part.shape = container->child->part(entry->key, buffer);
		shape_2fb150 const *a_shape = a->shape;
		part.key = entry->key;
		long const kind_b = part.shape->kind();
		long const kind_a = a_shape->kind();
		if (input->dispatcher->handlers[kind_a][kind_b] == (handler_2ed170)function_2f0e70)
		{
			if (contacts.size == (long)(contacts.capacity_and_flags & 0x7fffffff))
			{
				function_2d91d0(&contacts, sizeof(contact_2fb150));
			}
			long const index = contacts.size++;
			contacts.data[index].agent = agent;
			contacts.data[index].part = part.shape;
			contacts.data[index].key = entry->key;
			contacts.data[index].point.value.m128_f32[3] = 3.40282347e+38f;
			contact_2fb150 *contact = &contacts.data[index];
			contact->found = function_2f0f70((body_2f0f70 const *)a, (body_2f0f70 const *)&part,
				(input_2f0f70 const *)input, agent->terms, (contact_2f0f70 *)contact);
		}
		else
		{
			agent->slot_4(a, &part, input, collector);
		}
	}

	// closest first; a contact with the triangle's other parts that only
	// repeats a closer one's plane is dropped to the end
	long count = contacts.size;
	for (long i = 0; i < count; i++)
	{
		contact_2fb150 *first = &contacts.data[i];
		float closest_distance = first->point.value.m128_f32[3];
		long closest = i;
		for (long j = i; j < count; j++)
		{
			if (contacts.data[j].point.value.m128_f32[3] < closest_distance)
			{
				closest_distance = contacts.data[j].point.value.m128_f32[3];
				closest = j;
			}
		}
		swap_2fb150(contacts.data[closest], *first);
		contact_2fb150 const *closer = &contacts.data[i];
		for (long j = i + 1; j < count; )
		{
			if (contacts.data[j].found == 2 && repeats_plane_2fb150(&contacts.data[j], closer, input->settings->plane_tolerance).value)
			{
				contacts.data[j].found = 0;
				swap_2fb150(contacts.data[j], contacts.data[count - 1]);
				count--;
				continue;
			}
			j++;
		}
	}

	// the contacts kept go to the collector with ids, and the dropped ones
	// give theirs back
	for (long k = 0; k < contacts.size; k++)
	{
		if (contacts.data[k].found)
		{
			hits_2fb150 *hits = *(hits_2fb150 **)collector;
			if (hits->size == (hits->capacity_and_flags & 0x7fffffff))
			{
				function_2d91d0(hits, sizeof(hit_2fb150));
			}
			hit_2fb150 *hit = &hits->data[hits->size++];
			hit->point = contacts.data[k].point;
			hit->normal = contacts.data[k].normal;
			if (contacts.data[k].agent->id == 0xffff)
			{
				part.parent = b;
				part.placement = b->placement;
				shape_2fb150 const *shape = container->child->part(contacts.data[k].key, buffer);
				part.key = contacts.data[k].key;
				part.shape = shape;
				contacts.data[k].agent->id = agents->manager->add(a, &part, input, hit);
			}
			unsigned short const id = contacts.data[k].agent->id;
			if (id != 0xffff)
			{
				hit->id = id;
			}
			else
			{
				(*(hits_2fb150 **)collector)->size--;
			}
		}
		else
		{
			unsigned short const id = contacts.data[k].agent->id;
			if (id != 0xffff)
			{
				agents->manager->remove(id);
				contacts.data[k].agent->id = 0xffff;
			}
		}
	}
}
