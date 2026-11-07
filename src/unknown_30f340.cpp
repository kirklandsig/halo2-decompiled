// @flags /O2 /GL- /GX
// Havok (third-party physics library): two steps run on a member with the
// owner's flag at +0x12c set, then put back as it was; and adding a member
// with the constraints that come with it, noting the pairs of members that
// the member's constraints and actions now tie together.
#include "growable_array_2d91d0.h"
#include "stack_array_30d740.h"

struct member_30c8e0;
class island_30f380;

struct info_30f380
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

// a constraint between two members
class constraint_30f380
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
	virtual long type() { return 0; }
	virtual void slot_9() {}
	virtual void get_info(info_30f380 *info) {}

	char unknown4[8];
	member_30c8e0 *first;
	member_30c8e0 *second;
	island_30f380 *owner;
};

ELEMENTS_2D91D0(constraints_30f380, constraint_30f380 *);

class action_30f380;
ELEMENTS_2D91D0(actions_30f380, action_30f380 *);

// what keeps constraints and actions: an island, or the world
class island_30f380
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void add_constraint(constraint_30f380 *constraint, info_30f380 const *info) {}

	char unknown4[0x4c - 4];
	actions_30f380 owned;
	constraints_30f380 constraints;
};

// something acting on members, which can list them
class action_30f380
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void members(array_longs_30d740 *out) {}

	char unknown4[0x10 - 4];
	island_30f380 *owner;
};

struct member_30c8e0
{
	char unknown0[0x40];
	bool fixed;
	island_30f380 *island;
	char unknown48[0x88 - 0x48];
	constraint_30f380 **constraints;
	long link_total;
	char unknown90[0x94 - 0x90];
	action_30f380 **actions;
	long action_count;
};

struct member_316d60;
struct entry_3169c0;

class class_316c50
{
public:
	void function_316d60(member_316d60 *member);
};

class list_owner_3169c0
{
public:
	void function_3169c0(entry_3169c0 *entry);
};

// two members something ties together
struct pair_30f380
{
	member_30c8e0 *first;
	member_30c8e0 *second;
};

ELEMENTS_2D91D0(pairs_30f380, pair_30f380);

class owner_30c8e0
{
public:
	char unknown0[0x20];
	pairs_30f380 pairs;
	island_30f380 *fixed_island;
	char unknown30[0x12c - 0x30];
	char flag_12c;

	void function_30c8e0(member_30c8e0 *member);
	void function_30df80(member_30c8e0 *member);
	void function_30f340(member_30c8e0 *member);
	void function_30f380(member_30c8e0 *member, constraints_30f380 const *added);

	__forceinline void append_pair(member_30c8e0 *first, member_30c8e0 *second)
	{
		pairs.resize(pairs.size + 1);
		long first_count = pairs.size;
		pairs.data[first_count - 1].first = first;
		long second_count = pairs.size;
		pairs.data[second_count - 1].second = second;
	}

};

// @retail 0x30f340
void owner_30c8e0::function_30f340(member_30c8e0 *member)
{
	int flag = flag_12c;
	flag_12c = 1;
	function_30c8e0(member);
	function_30df80(member);
	flag_12c = flag;
}

// adds the member, gives its island the constraints added with it, and notes
// each member its constraints and actions tie it to that is not fixed; an
// action the fixed island held moves to the member's island
// @retail 0x30f380
void owner_30c8e0::function_30f380(member_30c8e0 *member, constraints_30f380 const *added)
{
	function_30f340(member);
	island_30f380 *island = member->island;
	for (int i = 0; i < added->size; i++)
	{
		island->constraints.append_copy(added->data[i]);
		info_30f380 info;
		added->data[i]->get_info(&info);
		island->add_constraint(added->data[i], &info);
		constraint_30f380 *last = added->data[i];
		last->owner = island;
	}
	for (int j = 0; j < member->link_total; j++)
	{
		if (member->constraints[j]->type() != 11)
		{
			constraint_30f380 *constraint = member->constraints[j];
			member_30c8e0 *first = constraint->first;
			member_30c8e0 *second;
			if (first == member && !(second = constraint->second)->fixed)
			{
				append_pair(member, second);
			}
			else if (!first->fixed)
			{
				append_pair(member, first);
			}
		}
	}
	longs_30d740 others(0x10);
	actions_30f380 *owned = &fixed_island->owned;
	for (int k = 0; k < member->action_count; k++)
	{
		for (int m = 0; m < owned->size; m++)
		{
			if (member->actions[k] == owned->data[m])
			{
				((class_316c50 *)island)->function_316d60((member_316d60 *)member->actions[k]);
				((list_owner_3169c0 *)fixed_island)->function_3169c0((entry_3169c0 *)owned->data[m]);
				action_30f380 *last = member->actions[k];
				last->owner = island;
				break;
			}
		}
		member->actions[k]->members(&others);
		for (int n = 0; n < others.size; n++)
		{
			member_30c8e0 *other = (member_30c8e0 *)others.data[n];
			if (!other->fixed && other != member)
			{
				append_pair(other, member);
			}
		}
	}
}
