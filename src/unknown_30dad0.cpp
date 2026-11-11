// @flags /O2 /GL-
// Havok (third-party physics library): recording a pair of entities with the
// group of the entity on the other side, and noting that group once in the
// recorder's list.
#include "holder_30dc80.h"

// @retail 0x30dad0
void holder_30dc80::slot_1(void const *input)
{
	pair_30dc30 const *pair = (pair_30dc30 const *)input;
	entity_30dad0 *entity = (entity_30dad0 *)body;
	link_30dad0 *link = pair->first == static_cast<link_30dad0 *>(entity) ? pair->second : pair->first;
	owner_30dad0 *owner = static_cast<entity_30dad0 *>(link)->owner;
	if (!owner->fixed)
	{
		group_30dad0 *group = owner->group;
		group->pairs.append_copy(*pair);
		((groups_30dad0 *)&values)->add(group);
	}
}

// @retail 0x30dba0
void holder_30dc80::slot_2(void const *input)
{
	pair_30dc30 const *pair = (pair_30dc30 const *)input;
	entity_30dad0 *entity = (entity_30dad0 *)body;
	link_30dad0 *link = pair->first == static_cast<link_30dad0 *>(entity) ? pair->second : pair->first;
	owner_30dad0 *owner = static_cast<entity_30dad0 *>(link)->owner;
	if (!owner->fixed)
	{
		group_30dad0 *group = owner->group;
		((class_30dc30 *)group)->function_30dc30(pair);
		((groups_30dad0 *)&values)->add(group);
	}
}
