#ifndef WORLD_OBJECT_30B340_H
#define WORLD_OBJECT_30B340_H
// Havok (third-party physics library): a reference-counted object that keeps
// a reference to the object it is given and a list whose owner it is
// (constructor 0x30b340).
#include "collidable_state_30b340.h"

struct list_30b340
{
	long field_0;
	void *owner;
	void *data;
	int size;
	unsigned long capacity;

	list_30b340() : field_0(0), owner(0), data(0), size(0), capacity(0x80000000) {}
};

// The value is initialized before the two handle words are cleared.
struct handle_value_30b340
{
	long value;
	handle_value_30b340(long initial) : value(initial) {}
};

struct handle_30b340
{
	long field_0;
	long field_4;
	handle_value_30b340 value;

	handle_30b340(long initial) : value(initial)
	{
		field_0 = 0;
		field_4 = 0;
	}
};

class class_30b340 : public referenced_2d88a0
{
public:
	long field_8;
	body_30b3a0 body;
	handle_30b340 handle;
	list_30b340 list;

	class_30b340(referenced_2d88a0 *object_, long value);
};

#endif
