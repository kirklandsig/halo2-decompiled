// @flags /O2 /Ob1 /GL- /GX
#include "lane_b_constraint_leaves.h"
#include "lane_b_constraint_entities.h"
bool __cdecl function_279740(long value);

inline void lane_b_release_entity(void *entity)
{
	referenced_2d88a0 *reference = (referenced_2d88a0 *)entity;
	if (--reference->reference_count == 0)
	{
		if (function_279740((long)reference))
			reference->reference_count = 1;
		else
			delete reference;
	}
}

void *lane_a_constraint::v3() { return &g_480b90; }

// @retail 0x31bac0
void lane_a_constraint::set_entity_a(hkEntity *entity)
{
	if (entity)
	{
		((referenced_2d88a0 *)entity)->reference_count++;
		((class_30bdc0 *)entity)->function_30bdc0((long)this);
	}
	if (body_a)
	{
		((entity_30bcc0 *)body_a)->function_30bcc0(this);
		lane_b_release_entity(body_a);
	}
	body_a = entity;
}
// @retail 0x31bb30
void lane_a_constraint::set_entity_b(hkEntity *entity)
{
	if (entity)
	{
		((referenced_2d88a0 *)entity)->reference_count++;
		((class_30bdc0 *)entity)->function_30bdc0((long)this);
	}
	if (body_b)
	{
		((entity_30bcc0 *)body_b)->function_30bcc0(this);
		lane_b_release_entity(body_b);
	}
	body_b = entity;
}
// @retail 0x31b990
void lane_a_constraint::event_2(hkEntity *entity)
{
	if (owner) ((lane_b_constraint_owner *)owner)->slot1(this);
}
// Entity unlinking precedes the shared registry-reference teardown.
// @retail 0x31b9e0
lane_a_constraint::~lane_a_constraint()
{
	if (body_a)
	{
		((entity_30bcc0 *)body_a)->function_30bcc0(this);
		lane_b_release_entity(body_a);
	}
	if (body_b)
	{
		((entity_30bcc0 *)body_b)->function_30bcc0(this);
		lane_b_release_entity(body_b);
	}
}
// @retail 0x31bba0 deleting lane_a_constraint
// @retail 0x31bab0 adjustor lane_a_constraint 8

// Retail constructor 0x31bbd0. Active marker deferred: stock generated
// placement-new stand-in cannot instantiate this genuinely abstract base.
lane_a_constraint::lane_a_constraint(const lane_a_constraint_description *description)
{
	reference_count = 1;
	owner = 0;
	body_a = 0;
	body_b = 0;
	set_entity_a((hkEntity *)description->body_a);
	set_entity_b((hkEntity *)description->body_b);
	priority = description->priority;
}
