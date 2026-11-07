// @flags /O2 /GL-
// Havok (third-party physics library): recording a pair of entities with the
// group of the entity on the other side, and noting that group once in the
// recorder's list.
#include "pair_recorder_30dad0.h"

// @retail 0x30dad0
void class_30dad0::function_30dad0(pair_30dc30 const *pair)
{
	owner_30dad0 *owner = other(pair)->owner;
	if (!owner->fixed)
	{
		group_30dad0 *group = owner->group;
		group->pairs.append_copy(*pair);
		groups.add(group);
	}
}

// @retail 0x30dba0
void class_30dad0::function_30dba0(pair_30dc30 const *pair)
{
	owner_30dad0 *owner = other(pair)->owner;
	if (!owner->fixed)
	{
		group_30dad0 *group = owner->group;
		((class_30dc30 *)group)->function_30dc30(pair);
		groups.add(group);
	}
}
