#ifndef JOINED_GROUPS_3720A0_H
#define JOINED_GROUPS_3720A0_H
// Havok (third-party physics library): sorting an island's bodies into the
// groups that joins connect, through a parent table (a root holds minus its
// group's size), and the list of groups that comes out.
#include "growable_array_2d91d0.h"

// an empty function; retail's linker folded the grouping's destructor into it
void function_175f40();

// one group: the indices of its bodies
struct members_372300
{
	long *data;
	long size;
	long written; // filled index slots while the group is exported
};

struct array_groups_372300
{
	members_372300 *data;
	long size;
	unsigned long capacity_and_flags;

	array_groups_372300() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_groups_372300()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(members_372300), 0x12);
		}
	}
};

// the groups, each group's index list freed with the list
struct groups_372300 : array_groups_372300
{
	groups_372300() {}
	~groups_372300();
};

class grouping_3720a0
{
public:
	array_2d91d0 *parents;
	array_2d91d0 *scratch;
	long group_count;

	grouping_3720a0(array_2d91d0 *parents, array_2d91d0 *scratch);
	__declspec(noinline) ~grouping_3720a0();

	long join(long first, long second);
	void compress_paths();

	// starts `count` bodies each in its own group, then joins the two
	// bodies of each pair
	void function_372200(array_2d91d0 const *pairs, long count);
	// lists the groups
	void function_372390(groups_372300 *groups);

	// whether every body ended in one group
	bool single_group() const
	{
		return ((long *)parents->data)[0] == -parents->size;
	}
private:
	long root(long index);
};

#endif
