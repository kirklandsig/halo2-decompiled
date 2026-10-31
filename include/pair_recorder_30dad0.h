#ifndef PAIR_RECORDER_30DAD0_H
#define PAIR_RECORDER_30DAD0_H
// Shared layout of the existing pair/group recording operations.
#include "growable_array_2d91d0.h"

struct pair_30dc30
{
	struct link_30dad0 *first;
	struct link_30dad0 *second;
};

class class_30dc30
{
public:
	void function_30dc30(pair_30dc30 const *pair);
};

ELEMENTS_2D91D0(pairs_30dad0, pair_30dc30);

struct group_30dad0
{
	char unknown0[0x80];
	pairs_30dad0 pairs;
};

struct owner_30dad0
{
	char unknown0[0x40];
	bool fixed;
	char unknown41[3];
	group_30dad0 *group;
};

// the part of an entity a pair points to
struct link_30dad0
{
	long field_0;
};

struct head_30dad0
{
	char unknown0[0x10];
};

struct entity_30dad0 : head_30dad0, link_30dad0
{
	char unknown14[0x20 - 0x14];
	owner_30dad0 *owner;
};

struct groups_30dad0
{
	ELEMENTS_2D91D0_MEMBERS(group_30dad0 *)

	__forceinline long index_of(group_30dad0 *group) const
	{
		long const &count = size;
		for (long i = 0; i < count; i++)
		{
			if (data[i] == group)
			{
				return i;
			}
		}
		return -1;
	}

	// appends the group when it isn't in the list yet
	__forceinline void add(group_30dad0 *group)
	{
		if (index_of(group) == -1)
		{
			append_copy(group);
		}
	}
};

class class_30dad0
{
public:
	long field_0;
	entity_30dad0 *body;
	groups_30dad0 groups;

	// the entity in the pair that isn't this one's
	__forceinline entity_30dad0 *other(pair_30dc30 const *pair) const
	{
		link_30dad0 *body_link = static_cast<link_30dad0 *>(body);
		link_30dad0 *link = pair->first == body_link ? pair->second : pair->first;
		return static_cast<entity_30dad0 *>(link);
	}


	void function_30dad0(pair_30dc30 const *pair);
	void function_30dba0(pair_30dc30 const *pair);
};

#endif
