// @flags /O2 /GL-
// Havok (third-party physics library): adding a new pair of overlapping
// objects while merging it into a list of pairs sorted by the two objects'
// ids: the old pairs that sort before it are copied across, a pair already
// there is skipped, the islands of the two objects are told, and the pair
// gets the agent the dispatcher's tables pick for the two shapes.

class hkBool
{
public:
	hkBool() {}

	char m_bool;
};

class c_havok_body_group;

class hkWorld
{
public:
	void function_30ccd0(c_havok_body_group *island);
};

class class_30d160
{
public:
	void function_30d160(long first, long second);
};

struct pair_316c50;

class class_316c50
{
public:
	void function_316c50(pair_316c50 const *pair);
};

// an object's shape: its type from its sixth slot
class shape_316dd0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long type() { return 0; }
};

// how an entity moves: its type from its seventh slot
class motion_316dd0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual long type() { return 0; }
};

// the island an entity is in
struct island_316dd0
{
	char unknown0[0x24];
	long index;
	char unknown28;
	bool field_29;
	char unknown2a[0x30 - 0x2a];
	bool field_30;
};

struct entity_316dd0
{
	char unknown0[0x3c];
	motion_316dd0 *motion;
	bool fixed;
	char unknown41[0x44 - 0x41];
	island_316dd0 *island;
	signed char type;
};

struct shaped_316dd0
{
	shape_316dd0 *shape;
	char unknown4[0x10 - 4];
};

// the part of an object the pair list holds: its id
struct handle_316dd0
{
	long field_0;
	unsigned long id;
	char unknown8[0x10 - 8];
};

struct collidable_316dd0 : shaped_316dd0, handle_316dd0
{
	entity_316dd0 *entity;
};

// what the agent's creator hands the agent's constructor, given back when no
// agent is made
class agent_info_316dd0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void release() {}
};

class creator_316dd0
{
public:
	virtual void slot_0() {}
	virtual agent_info_316dd0 *create(collidable_316dd0 *a, collidable_316dd0 *b, void *dispatcher) { return 0; }
};

struct agent_316dd0
{
	char unknown0[8];
	long field_8;
};

typedef agent_316dd0 *(__cdecl *handler_316dd0)(collidable_316dd0 *a, collidable_316dd0 *b, void *dispatcher, agent_info_316dd0 *info);

// the dispatcher's tables: creators by the two entities' types, and agent
// constructors by the two shapes' types
struct tables_316dd0
{
	char unknown0[0xc];
	creator_316dd0 *creators[12][8];
	handler_316dd0 handlers[32][32];
};

struct dispatcher_316dd0
{
	tables_316dd0 *tables;
};

// tells whether two objects collide
class filter_316dd0
{
public:
	virtual void slot_0() {}
	virtual hkBool collides(collidable_316dd0 *a, collidable_316dd0 *b) { return hkBool(); }
};

struct world_316dd0
{
	char unknown0[0x250];
	filter_316dd0 *filter;
};

struct handle_pair_316dd0
{
	handle_316dd0 *a;
	handle_316dd0 *b;
};

// a pair in the list: the two objects, the agent, and whether it collides
struct record_316dd0
{
	handle_316dd0 *a;
	handle_316dd0 *b;
	agent_316dd0 *agent;
	long collides;
};

// whether a pair sorts before another by its objects' ids
inline bool sorts_before_316dd0(record_316dd0 const *record, handle_pair_316dd0 const *pair)
{
	if (pair->a->id > record->a->id)
	{
		return true;
	}
	if (record->a->id != pair->a->id)
	{
		return false;
	}
	return record->b->id < pair->b->id;
}

class merger_316dd0
{
public:
	long field_0;
	world_316dd0 *world;
	char unknown8[0x50 - 8];
	long skip_existing;
	char unknown54[0x7c - 0x54];
	dispatcher_316dd0 *dispatcher;
	record_316dd0 *cursor;
	record_316dd0 *end;
	record_316dd0 *out;

	void function_316dd0(handle_pair_316dd0 const *pair);
};

// The remaining byte differences are the two first-handle loads in the
// duplicate check. Capture the selected callback before passing its arguments.
// @retail 0x316dd0
void merger_316dd0::function_316dd0(handle_pair_316dd0 const *pair)
{
	while (cursor < end && sorts_before_316dd0(cursor, pair))
	{
		*out = *cursor;
		out++;
		cursor++;
	}
	if (skip_existing != 0 && cursor != end && pair->a->id == cursor->a->id && cursor->b->id == pair->b->id)
	{
		return;
	}
	record_316dd0 *record = out;
	collidable_316dd0 *a = static_cast<collidable_316dd0 *>(pair->a);
	collidable_316dd0 *b = static_cast<collidable_316dd0 *>(pair->b);
	record->a = a;
	record->b = b;
	entity_316dd0 *first = a->entity;
	entity_316dd0 *second = b->entity;
	if (first->island != second->island)
	{
		// the island that is not the fixed one, which holds this object
		island_316dd0 *island = first->island == (island_316dd0 *)((char *)this - 0x1c) ? second->island : first->island;
		if (island->index != -1 && !island->field_29)
		{
			((hkWorld *)world)->function_30ccd0((c_havok_body_group *)island);
		}
		if (!island->field_30)
		{
			((class_316c50 *)island)->function_316c50((pair_316c50 const *)pair);
			return;
		}
		if (!first->fixed && !second->fixed)
		{
			((class_30d160 *)world)->function_30d160((long)first, (long)second);
		}
	}
	agent_info_316dd0 *info = dispatcher->tables->creators[first->type][second->type]->create(a, b, dispatcher);
	dispatcher_316dd0 *dispatch = dispatcher;
	tables_316dd0 *tables = dispatch->tables;
	long const first_shape = a->shape->type();
	long const second_shape = b->shape->type();
	handler_316dd0 handler = tables->handlers[first_shape][second_shape];
	agent_316dd0 *agent = handler(a, b, dispatch, info);
	record->agent = agent;
	if (agent != 0 && agent->field_8 != 0)
	{
		entity_316dd0 *first_entity = a->entity;
		entity_316dd0 *second_entity = b->entity;
		if ((first_entity->motion->type() == 6 || first_entity->fixed) &&
			(second_entity->motion->type() == 6 || second_entity->fixed))
		{
			record->collides = -1;
		}
		else
		{
			record->collides = (world->filter->collides(a, b).m_bool != 0) - 1;
		}
	}
	else
	{
		record->agent = 0;
		info->release();
	}
	out++;
}
