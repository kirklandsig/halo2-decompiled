// @flags /O2 /GL- /GX
// Havok (third-party physics library): two islands of a world merged into
// one: the second island's bodies, lists and counts moved into the first,
// the two islands' lists of pairs (sorted by the ids of their two bodies)
// merged with the pairs both held kept once, and the second island taken out
// of the world's active list and deleted.
#include "growable_array_2d91d0.h"
#include "stack_array_30d740.h"

// a body of a pair, as the pair lists sort it
struct pair_body_30ed40
{
	long unknown0;
	unsigned long id;
};

// an agent of a pair; a pair both islands held calls slot 5 of its agent
// and slot 4 of the agent at +8
class pair_agent_30ed40
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}

	long unknown4;
	pair_agent_30ed40 *second;
};

// a pair of an island: its two bodies, sorted by id, and its agent
struct pair_30ed40
{
	pair_body_30ed40 const *first;
	pair_body_30ed40 const *second;
	pair_agent_30ed40 *agent;
	long field_c;
};

// whether a pair sorts before another: by its first body's id, then by its
// second's
inline bool pair_less_30ed40(pair_30ed40 const *a, pair_30ed40 const *b)
{
	if (a->first->id < b->first->id)
	{
		return true;
	}
	if (a->first->id == b->first->id)
	{
		return a->second->id < b->second->id;
	}
	return false;
}

// Retail reads the candidate ID again when testing equality after ordering.
inline unsigned long read_pair_id_30ed40(unsigned long const &id)
{
	return *(unsigned long const volatile *)&id;
}

ELEMENTS_2D91D0(pairs_30ed40, pair_30ed40);

// the pairs of `destination` and `source` merged in order into
// `destination`; a pair both hold is kept once, and its agent in `source`
// told through its slots
// The remaining differences concern the comparison loads, cursor registers,
// and the resulting stack frame; the merge and callback order follow retail.
// @retail 0x30ed40
static void __cdecl function_30ed40(pairs_30ed40 *destination, pairs_30ed40 const *source)
{
	vectors_30d9a0 copy_storage(destination->size);
	pairs_30ed40 *copy = (pairs_30ed40 *)&copy_storage;
	copy->resize(destination->size);
	for (int i = 0; i < destination->size; i++)
	{
		copy->data[i] = destination->data[i];
	}
	destination->resize(destination->size + source->size);
	int from_source = 0;
	int copy_cursor = 0;
	int to = 0;
	int both = 0;
	while (copy_cursor < copy->size && from_source < source->size)
	{
		pair_30ed40 const *b = &source->data[from_source];
		pair_30ed40 const *a = &copy->data[copy_cursor];
		if (pair_less_30ed40(a, b))
		{
			destination->data[to] = copy->data[copy_cursor];
			copy_cursor++;
			to++;
		}
		else if (a->first->id == read_pair_id_30ed40(b->first->id) && a->second->id == b->second->id)
		{
			pair_agent_30ed40 *agent = b->agent;
			pair_agent_30ed40 *second = agent->second;
			agent->slot_5();
			second->slot_4();
			from_source++;
			both++;
		}
		else
		{
			destination->data[to] = source->data[from_source];
			from_source++;
			to++;
		}
	}
	destination->resize(copy->size - both + source->size);
	if (copy_cursor == copy->size)
	{
		for (; from_source < source->size; from_source++, to++)
		{
			destination->data[to] = source->data[from_source];
		}
	}
	else
	{
		for (; copy_cursor < copy->size; copy_cursor++, to++)
		{
			destination->data[to] = copy->data[copy_cursor];
		}
	}
}

class c_havok_body_group;

// a body of an island: its island and its index in the island's list
struct island_body_30f030
{
	char unknown0[0x44];
	c_havok_body_group *island;
	char unknown48[0x58 - 0x48];
	long index;
};

struct island_entry_10_30f030
{
	char unknown0[0x10];
	c_havok_body_group *island;
};

struct island_entry_14_30f030
{
	char unknown0[0x14];
	c_havok_body_group *island;
};

struct pair_value_30f030
{
	void *first;
	long second;
};

ELEMENTS_2D91D0(bodies_30f030, island_body_30f030 *);
ELEMENTS_2D91D0(entries_10_30f030, island_entry_10_30f030 *);
ELEMENTS_2D91D0(entries_14_30f030, island_entry_14_30f030 *);
ELEMENTS_2D91D0(values_30f030, pair_value_30f030);

class c_havok_body_group
{
public:
	// the deleting destructor, first in the vtable, called without the null
	// test `delete` adds
	virtual void destroy(unsigned int flags) {}

	long unknown4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	char unknown1c[0x24 - 0x1c];
	long index;
	char field_28;
	bool active;
	char unknown2a[0x3c - 0x2a];
	bodies_30f030 bodies;
	char unknown48[0x4c - 0x48];
	entries_10_30f030 entries_4c;
	entries_14_30f030 entries_58;
	char unknown64[0x68 - 0x64];
	pairs_30ed40 pairs;
	values_30f030 values;
	char unknown80[0xae - 0x80];
	bool field_ae;
};

ELEMENTS_2D91D0(islands_30f030, c_havok_body_group *);

// the world, as 0x30ccd0 sees it
class hkWorld
{
public:
	void function_30ccd0(c_havok_body_group *island);
};

// the world
class class_30f030
{
public:
	char unknown0[8];
	islands_30f030 active_islands;

	void *function_30f030(void *island, void *other);
};

// `other` merged into `island`, both made active first; `other` is deleted
// @retail 0x30f030
void *class_30f030::function_30f030(void *island_, void *other_)
{
	c_havok_body_group *island = (c_havok_body_group *)island_;
	c_havok_body_group *other = (c_havok_body_group *)other_;
	if (island != other)
	{
		if (!island->active)
		{
			((hkWorld *)this)->function_30ccd0(island);
		}
		if (!other->active)
		{
			((hkWorld *)this)->function_30ccd0(other);
		}
		function_30ed40(&island->pairs, &other->pairs);

		int index = island->bodies.size;
		island->bodies.resize(index + other->bodies.size);
		for (int i = 0; i < other->bodies.size; i++, index++)
		{
			island->bodies.data[index] = other->bodies.data[i];
			other->bodies.data[i]->island = island;
			other->bodies.data[i]->index = index;
		}

		int const first_4c = island->entries_4c.size;
		island->entries_4c.resize(first_4c + other->entries_4c.size);
		for (int i = 0; i < other->entries_4c.size; i++)
		{
			island->entries_4c.data[first_4c + i] = other->entries_4c.data[i];
			other->entries_4c.data[i]->island = island;
		}

		int const first_58 = island->entries_58.size;
		island->entries_58.resize(first_58 + other->entries_58.size);
		for (int i = 0; i < other->entries_58.size; i++)
		{
			island_entry_14_30f030 *entry = other->entries_58.data[i];
			island->entries_58.data[first_58 + i] = entry;
			entry->island = island;
		}

		island->field_8 = island->field_8 > other->field_8 ? island->field_8 : other->field_8;
		island->field_c += other->field_c;
		island->field_10 += other->field_10;
		island->field_18 += other->field_18;
		island->field_14 += other->field_14;

		int const first_value = island->values.size;
		island->values.resize(first_value + other->values.size);
		int destination_index = first_value;
		for (int i = 0; i < other->values.size; i++, destination_index++)
		{
			island->values.data[destination_index] = other->values.data[i];
		}

		active_islands.data[other->index] = active_islands.data[active_islands.size - 1];
		active_islands.data[other->index]->index = other->index;
		active_islands.size--;

		island->field_28 = island->field_28 || other->field_28;
		if (other->field_ae)
		{
			island->field_ae = true;
		}
		other->destroy(1);
	}
	return island;
}
