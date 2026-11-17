// @flags /O2 /GL- /GX
// Havok (third-party physics library): adding an object to the world (the
// broad phase gives the pairs it begins, which are put in order and passed
// on; the world takes a reference to it and tells its listeners) and
// removing one (the same for the pairs it ends; the world lets go of it).
#include "placed_shape_312800.h"
#include "listener_world_315d20.h"

bool __cdecl function_279740(long value);
void __cdecl function_318160(listener_world_315d20 *world, void *event);
void __cdecl function_3181d0(listener_world_315d20 *world, void *event);

class deletable_30e4b0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// the object as the world sees it: its slot 6 gives its bounds
class added_30e4b0 : public overlapped_312800
{
public:
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void get_bounds(bounds_312800 *bounds) {}
};

// where an object is in the world's list, or -1
inline int index_of_30e5e0(objects_30e4b0 const &objects, overlapped_312800 const *object)
{
	for (int i = 0; i < objects.size; i++)
	{
		if (objects.data[i] == object)
		{
			return i;
		}
	}
	return -1;
}

class c_contact_query_world : public world_312590
{
public:
	void *add(void *volume);
	void remove(void *volume);
};

// @retail 0x30e4b0
void *c_contact_query_world::add(void *volume)
{
	added_30e4b0 *object = (added_30e4b0 *)volume;
	object->field_8 = (long)this;
	pairs_30d870 added(pair_capacity_234);
	bounds_312800 bounds;
	object->get_bounds(&bounds);
	collidable_315a10 *collidable = (collidable_315a10 *)&object->body.shape;
	handle_315a10 *handle = collidable;
	pair_finder->slot_1(handle, &bounds, &added);
	long id = next_id++;
	object->handle.field_4 = id;
	if (added.size != 0)
	{
		function_372b50(added.data, added.size);
		dispatcher->function_3728c0(added.data, added.size, 0, 0, pair_listener);
	}
	object->reference_count++;
	objects.append(object);
	function_318160((listener_world_315d20 *)this, object);
	return object;
}

// @retail 0x30e5e0
void c_contact_query_world::remove(void *volume)
{
	added_30e4b0 *object = (added_30e4b0 *)volume;
	pairs_30d870 removed(pair_capacity_234);
	collidable_315a10 *collidable = (collidable_315a10 *)&object->body.shape;
	handle_315a10 *handle = collidable;
	pair_finder->slot_3(handle, &removed);
	if (removed.size != 0)
	{
		function_372b50(removed.data, removed.size);
		dispatcher->function_3728c0(0, 0, removed.data, removed.size, pair_listener);
	}
	function_3181d0((listener_world_315d20 *)this, object);
	object->field_8 = 0;
	object->reference_count--;
	if (object->reference_count == 0)
	{
		if (function_279740((long)object))
		{
			object->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_30e4b0 *)object)->destroy(1);
		}
	}
	int index = index_of_30e5e0(objects, object);
	objects.size--;
	objects.data[index] = objects.data[objects.size];
}
