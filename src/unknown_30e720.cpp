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
	void split_island_30e720(island_30e720 *island);
};



// an island's pairs: room for four for each action, one for each join and link
struct island_pairs_30e720 : pairs_30d870
{
	island_pairs_30e720(long links, long joins, long actions) : pairs_30d870(actions * 4 + joins + links) {}
};

inline long capacity_30e720(islands_30e720 const &list)
{
	return list.capacity_and_flags & 0x7fffffff;
}

inline long size_30e720(islands_30e720 const &list)
{
	return list.size;
}

static __forceinline void append_island_30e720(islands_30e720 &list, island_30e720 *value)
{
	if (size_30e720(list) == capacity_30e720(list))
	{
		function_2d91d0(&list, sizeof(island_30e720 *));
	}
	list.data[list.size] = value;
	list.size++;
}

// whether the grouping left every body in one group, as a stored flag
static __forceinline bool one_group_30e720(grouping_3720a0 const &grouping)
{
	long const count = grouping.parents->size;
	bool const single = ((long *)grouping.parents->data)[0] == -count;
	return single;
}

// the island of the body a linked pair's side belongs to
static __forceinline body_30e720 *side_body_30e720(side_30e720 *side)
{
	char *address = (char *)side;
	address -= sizeof(head_30e720);
	return ((linked_30e720 *)address)->body;
}

// Matched (was 87): the joins loop reads its start from the list through a
// const-reference size accessor and each join's two bodies through helpers;
// the accessor leaves the loop's `c >= 0` test as one more use of the shared
// zero, which makes the zero's long piece cheaper to reload than to keep in
// ebp (retail reloads it), and the two returned bodies are candidates that are
// coloured before the counter, which then takes edi. In the island section
// the bodies count is read through an accessor, the group count is named in
// the single-group test, and the action is addressed through a helper.
// Before, 80 (was 86): a linked pair's side gives its body through a helper and
// the caller reads the island from it, the second side is read again from the
// list, and the link counter starts before the loop's entry test: the second
// side's loads, the address order and the entry test's place agree with retail.
// Before, 86 (was 189): an island is appended by comparing its list's size
// with its capacity through two const-reference accessors, the moved island is
// read through the list's data pointer, the counter starts at the list's size
// less one, and the bodies loop has its own entry test like the other lists:
// the opening, the island creation and the empty-island arm now agree with
// retail. Left: the zero register is not yet live before the outer loop, a
// scratch register parity after the pair list, and the action section's
// registers.
// Before, 189 (was 216): a created island's index is stored through a helper taking
// it by value; the value is one more candidate in the island-creation block,
// which lifts the created island above the moved body in the colouring order,
// so the created island takes ebp and the body ebx as in retail.
// Before, 227 (was 345): the pair list's room comes from a constructor taking the
// three counts (links, joins, actions), which loads all three before the sum as
// retail does, and a link's island is found by stepping a byte pointer back from
// the side, which gives retail's separate subtraction. Left: the second side's
// load is still folded, ebx/ebp are swapped in the split section, and the opening
// decrement.
// Before, 345 (was 396): rewritten from retail: the joins loop reads each join's
// owner through the join entry, the list and link loops are guarded by their own
// entry tests (`if (0 < n)`), the single-group test is a file-local helper with a
// named result, and the islands are appended by a file-local helper. The empty
// grouping destructor is defined with its class (src/unknown_3720a0.cpp), so this
// function calls it with its own EH state as retail does.
// Before, 396 (was 396): Frame 0xc8 agrees; the initial counter load and
// saved pointer slots differ, and the cleanup call is absent.
// Byte result on owner tests 396; deletion guard 396; body while 396;
// link reference 396; direct join reads 417; action cursor 461;
// earlier transfer-buffer declaration 396; reference indexers 396.
// Pair-list rename 396; action-buffer rename 396; grouping rename 396;
// direct body array 461; join next-slot 397; direct owner tests 458;
// group while 396. NOT kept: island next-slot 387 puts the size update
// before the element store; retail updates it afterwards.
// Outer postdecrement 461; counter scope 396; current pointer name 396;
// action scan snapshots 396; capacity call plus size store 443;
// group entry reference 456; reverse transfer postdecrements 494;
// forced owner inline 396. Original body and helper restored.
// a created island's place in the island list
static __forceinline void set_index_30e720(island_30e720 *island, long index)
{
	island->index = index;
}

// slot `index` of an island list's data given the element at `last`
static __forceinline void move_island_30e720(island_30e720 **const &data, long last, long index)
{
	data[index] = data[last];
}

// the number of bodies in an island
static __forceinline long body_count_30e720(island_30e720 const *island)
{
	return island->bodies.size;
}

// an island's action at `index`
static __forceinline action_30e720 *action_at_30e720(island_30e720 const *island, long index)
{
	return island->actions.data[index];
}

// the number of joins in a list
inline long size_30e720(joins_30e720 const &list)
{
	return list.size;
}

// the second body of a join
static __forceinline body_30e720 *second_of_30e720(join_30e720 const *join)
{
	return join->second;
}

// the first body of a join
static __forceinline body_30e720 *first_of_30e720(join_30e720 const *join)
{
	return join->first;
}

// @retail 0x30e720
void c_311100::function_30e720()
{
	int original_count = islands.size;
	for (int i = original_count - 1; i >= 0; i--)
	{
		island_30e720 *island = islands.data[i];
		if (island->bodies.size == 0)
		{
			move_island_30e720(islands.data, islands.size - 1, i);
			islands.data[i]->index = i;
			islands.size--;
			delete island;
		}
		else if (island->needs_split)
		{
			island_pairs_30e720 pairs(island->links.size, island->joins.size, island->actions.size);
			((class_317d90 *)island)->function_317d90((id_pairs_317d90 *)&pairs);
			longs_2e1ab0 parents(body_count_30e720(island));
			longs_2e1ab0 scratch(pairs.size);
			grouping_3720a0 grouping((array_2d91d0 *)&parents, (array_2d91d0 *)&scratch);
			grouping.function_372200((array_2d91d0 const *)&pairs, body_count_30e720(island));
			if (!one_group_30e720(grouping))
			{
				groups_372300 groups;
				grouping.function_372390(&groups);
				for (int g = 1; g < groups.size; g++)
				{
					island_30e720 *created = (island_30e720 *)new object_3171a0((long)this);
					append_island_30e720(islands, created);
					set_index_30e720(created, islands.size - 1);
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
						created->bodies.data[j] = body;
					}
				}
				int k = 0;
				if (0 < island->bodies.size)
				{
					for (; k < island->bodies.size;)
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
				}
				for (int a = island->actions.size - 1; a >= 0; a--)
				{
					inline_bodies_30e720 listed;
					island->actions.data[a]->list_bodies(&listed);
					island_30e720 *owner = 0;
					int n = 0;
					if (0 < listed.size)
					{
						for (; n < listed.size; n++)
						{
							owner = listed.data[n]->island;
							if (!owner->taken_out_of_world())
							{
								break;
							}
						}
					}
					action_at_30e720(island, a)->island = owner;
					island_30e720 *kept_by = island->actions.data[a]->island;
					if (kept_by != island)
					{
						kept_by->actions.append_copy(island->actions[a]);
						island->actions.remove_at(a);
					}
				}
				for (int c = size_30e720(island->joins) - 1; c >= 0; c--)
				{
					island_30e720 *owner = first_of_30e720(island->joins[c])->island;
					if (owner->taken_out_of_world())
					{
						owner = second_of_30e720(island->joins[c])->island;
					}
					if (owner != island)
					{
						join_info_30e720 info;
						join_30e720 *join = island->joins[c];
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
				int l = 0;
				if (0 < links.size)
				{
					for (; l < links.size; l++)
					{
						link_30e720 &link = links.data[l];
						island_30e720 *owner = side_body_30e720(link.first)->island;
						if (owner->taken_out_of_world())
						{
							owner = side_body_30e720(links.data[l].second)->island;
						}
						if (owner != island)
						{
							owner->links.append_copy(link);
						}
						else
						{
							links.data[kept] = link;
							kept++;
						}
					}
				}
				links.resize(kept);
			}
			island->needs_split = false;
		}

	}
}
