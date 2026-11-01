// @flags /O2 /GL-
// Havok (third-party physics library): tells each side of a new or removed
// pair about the other, when that side is of the kind that listens (2).

#include "pair_notifications_315e70.h"

// @retail 0x315e70
void dispatcher_315e70::slot_1(void const *value)
{
	pair_315e70 const *pair = (pair_315e70 const *)value;
	if (pair->a->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		handler_315e70 *handler = a->handler;
		handler->pair_added(b, flag_315e70(false));
	}
	if (pair->b->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		handler_315e70 *handler = b->handler;
		handler->pair_added(a, flag_315e70(false));
	}
}

// @retail 0x315ef0
void dispatcher_315e70::slot_2(void const *value)
{
	pair_315e70 const *pair = (pair_315e70 const *)value;
	if (pair->a->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		a->handler->pair_removed(b);
	}
	if (pair->b->type == 2)
	{
		entity_315e70 *a = static_cast<entity_315e70 *>(pair->a);
		entity_315e70 *b = static_cast<entity_315e70 *>(pair->b);
		b->handler->pair_removed(a);
	}
}
