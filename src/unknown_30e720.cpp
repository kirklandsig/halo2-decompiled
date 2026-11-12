// @flags /O2 /GL- /GX
// Havok (third-party physics library): splitting a world's islands. An island
// with no bodies left is removed; one marked for checking has its bodies
// sorted into the groups their joins connect, and every group after the
// first moves to a new island with its bodies, the actions and joins that
// reach it and the linked pairs that touch it.
#include "growable_array_2d91d0.h"
#include "stack_array_30d740.h"
#include "joined_groups_3720a0.h"

// an array of 4-byte values whose storage comes from the allocator's stack
// area (its constructor and destructor are in unknown_2e18e0.cpp)
struct array_longs_2e1ab0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_longs_2e1ab0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_longs_2e1ab0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct longs_2e1ab0 : array_longs_2e1ab0
{
	long *stack_block;
	long stack_capacity;

	longs_2e1ab0(long capacity);
	~longs_2e1ab0();
};

// an island as it is made (the constructor is in unknown_3171a0.cpp)
class object_3171a0
{
public:
	REFERENCED_NEW_2D88A0(0x2d)

	char unknown0[0xb0];

	object_3171a0(long value);
};

struct id_pairs_317d90;

class class_317d90
{
public:
	void function_317d90(id_pairs_317d90 *pairs) const;
};

class island_30e720;

// what a body holds that says how much it adds to its island
class shape_30e720
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long slot_5() { return 0; }
};

struct body_30e720
{
	char unknown0[0x3c];
	shape_30e720 *shape;
	char unknown40[4];
	island_30e720 *island;
	char unknown48[0x58 - 0x48];
	long index;
};

ELEMENTS_2D91D0(bodies_30e720, body_30e720 *);

// a list of bodies with room for sixteen inside it
struct listed_bodies_30e720
{
	body_30e720 **data;
	long size;
	unsigned long capacity_and_flags;

	listed_bodies_30e720() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~listed_bodies_30e720()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(body_30e720 *), 0x12);
		}
	}
};

struct inline_bodies_30e720 : listed_bodies_30e720
{
	body_30e720 *storage[16];

	inline_bodies_30e720()
	{
		data = storage;
		size = 0;
		capacity_and_flags = 16 | 0x80000000;
	}
};

// an action on bodies: it lists them, and is kept by the island of the first
// one still in an island
class action_30e720
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void list_bodies(listed_bodies_30e720 *bodies) {}

	char unknown4[0xc];
	island_30e720 *island;
};

ELEMENTS_2D91D0(actions_30e720, action_30e720 *);

// what a join hands its islands when it moves
struct join_info_30e720
{
	long values[4];
};

// a join between two bodies
class join_30e720
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
	virtual void fill_move_info(join_info_30e720 *info) {}

	char unknown4[8];
	body_30e720 *first;
	body_30e720 *second;
	island_30e720 *island;
};

ELEMENTS_2D91D0(joins_30e720, join_30e720 *);

// a linked pair's side: a part 0x10 into the object that holds the body
struct head_30e720
{
	char unknown0[0x10];
};

struct side_30e720
{
	char unknown0[0x10];
};

struct linked_30e720 : head_30e720, side_30e720
{
	body_30e720 *body;
};

__forceinline void *__cdecl byte_offset_30e720(void *address, int offset)
{
	return static_cast<char *>(address) + offset;
}

struct link_30e720
{
	side_30e720 *first;
	side_30e720 *second;
	long field_8;
	long field_c;
};

ELEMENTS_2D91D0_ALIGNED(links_30e720, link_30e720);

class island_30e720
{
public:
	virtual ~island_30e720() {}
	virtual void slot_1() {}
	virtual void join_added(join_30e720 *join, join_info_30e720 *info) {}
	virtual void join_removed(join_30e720 *join, join_info_30e720 *info) {}

	char unknown4[0x10];
	long weight;
	char unknown18[0x24 - 0x18];
	long index;
	bool needs_split;
	char unknown29[0x30 - 0x29];
	char field_30;
	char unknown31[0x3c - 0x31];
	bodies_30e720 bodies;
	char unknown48[4];
	actions_30e720 actions;
	joins_30e720 joins;
	char unknown64[4];
	links_30e720 links;
	char unknown74[0xae - 0x74];
	char flag_ae;

	// whether the island was taken out of its world
	bool taken_out_of_world() const
	{
		bool const taken = index == -1;
		return taken;
	}
};

ELEMENTS_2D91D0(islands_30e720, island_30e720 *);

class c_311100
{
public:
	char unknown0[8];
	islands_30e720 islands;

	void function_30e720();
};

grouping_3720a0::~grouping_3720a0()
{
}

// score 396 (was 396): Frame 0xc8 agrees; the initial counter load and
// saved pointer slots differ, and the cleanup call is absent.
// Byte result on owner tests 396; deletion guard 396; body while 396;
// link reference 396; direct join reads 417; action cursor 461;
// earlier transfer-buffer declaration 396; reference indexers 396.
// @retail 0x30e720
void c_311100::function_30e720()
{
	int original_count = islands.size;
	for (int i = original_count - 1; i >= 0; i--)
	{
		island_30e720 *island = islands.data[i];
		if (island->bodies.size == 0)
		{
			islands.data[i] = islands.data[islands.size - 1];
			islands.data[i]->index = i;
			islands.size--;
			delete island;
		}
		else if (island->needs_split)
		{
			pairs_30d870 pairs(island->actions.size * 4 + island->links.size + island->joins.size);
			((class_317d90 *)island)->function_317d90((id_pairs_317d90 *)&pairs);
			longs_2e1ab0 parents(island->bodies.size);
			longs_2e1ab0 scratch(pairs.size);
			grouping_3720a0 grouping((array_2d91d0 *)&parents, (array_2d91d0 *)&scratch);
			grouping.function_372200((array_2d91d0 const *)&pairs, island->bodies.size);
			if (!grouping.single_group())
			{
				groups_372300 groups;
				grouping.function_372390(&groups);
				for (int g = 1; g < groups.size; g++)
				{
					island_30e720 *created = (island_30e720 *)new object_3171a0((long)this);
					islands.append(created);
					created->index = islands.size - 1;
					created->field_30 = island->field_30;
					bodies_30e720 &moved = created->bodies;
					if ((moved.capacity_and_flags & 0x7fffffff) < groups.data[g].size)
					{
						function_2d9160(&moved, groups.data[g].size, sizeof(body_30e720 *));
					}
					moved.resize(groups.data[g].size);
					if (island->flag_ae)
					{
						created->flag_ae = 1;
					}
					for (int j = 0; j < groups.data[g].size; j++)
					{
						body_30e720 *body = island->bodies.data[groups.data[g].data[j]];
						body->island = created;
						body->index = j;
						long weight = body->shape->slot_5();
						island->weight -= weight;
						created->weight += weight;
						moved[j] = body;
					}
				}
				for (int k = 0; k < island->bodies.size;)
				{
					body_30e720 *body = island->bodies.data[k];
					if (body->island != island)
					{
						island->bodies.remove_at(k);
					}
					else
					{
						body->index = k;
						k++;
					}
				}
				for (int a = island->actions.size - 1; a >= 0; a--)
				{
					inline_bodies_30e720 listed;
					island->actions.data[a]->list_bodies(&listed);
					island_30e720 *owner = 0;
					for (int n = 0; n < listed.size; n++)
					{
						owner = listed.data[n]->island;
						if (!owner->taken_out_of_world())
						{
							break;
						}
					}
					island->actions.data[a]->island = owner;
					island_30e720 *kept_by = island->actions.data[a]->island;
					if (kept_by != island)
					{
						kept_by->actions.append_copy(island->actions[a]);
						island->actions.remove_at(a);
					}
				}
				for (int c = island->joins.size - 1; c >= 0; c--)
				{
					join_30e720 *const &entry = island->joins[c];
					island_30e720 *owner = entry->first->island;
					if (owner->taken_out_of_world())
					{
						owner = entry->second->island;
					}
					if (owner != island)
					{
						join_info_30e720 info;
						join_30e720 *join = entry;
						island->joins.remove_at(c);
						owner->joins.append(join);
						join->island = owner;
						join->fill_move_info(&info);
						owner->join_added(join, &info);
						island->join_removed(join, &info);
					}
				}
				links_30e720 &links = island->links;
				long kept = 0;
				for (int l = 0; l < links.size; l++)
				{
					link_30e720 *link = &links[l];
					island_30e720 *owner = static_cast<linked_30e720 *>(byte_offset_30e720(link->first, -int(sizeof(head_30e720))))->body->island;
					if (owner->taken_out_of_world())
					{
						owner = static_cast<linked_30e720 *>(byte_offset_30e720(link->second, -int(sizeof(head_30e720))))->body->island;
					}
					if (owner != island)
					{
						owner->links.append_copy(*link);
					}
					else
					{
						links[kept] = *link;
						kept++;
					}
				}
				links.resize(kept);
			}
			island->needs_split = false;
		}
	}
}
