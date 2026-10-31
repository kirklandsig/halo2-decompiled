// @flags /O2 /GL- /GX
// Havok (third-party physics library): a member's new pairs after it was
// placed: the broad phase gives the pairs its new bounds begin; a member in a
// group hands them to its group, and a fixed member has the dispatcher run
// them with a handler that notes the groups they reach, each of which then
// takes its own pending pairs.
#include "placed_shape_312800.h"
#include "holder_30dc80.h"

// a shape: its seventh slot gives its bounds under a transform, grown by a
// tolerance
class shape_30df80
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void get_bounds(void const *transform, float tolerance, bounds_312800 *bounds) {}
};

struct motion_30df80
{
	char unknown0[0x20];
	char transform[0x40];
};

// a group's pairs waiting to be run, each a pair of words
struct pair_30df80
{
	long first;
	long second;
};

ELEMENTS_2D91D0(pairs_30df80, pair_30df80);

// a group: the pairs it gathers come before its own pending pairs
class collector_3173f0
{
public:
	void function_3173f0(pair_30d870 *added, long added_count, pair_30d870 *removed, long removed_count);
};

struct group_30dc80
{
	char unknown0[0x80];
	pairs_30df80 pending;
};

// the member's shape, its motion, and the broad phase's handle for it
struct collidable_30df80
{
	shape_30df80 *shape;
	long field_4;
	motion_30df80 const *motion;
	long field_c;
	handle_315a10 handle;
};

struct member_30c8e0
{
	char unknown0[0xc];
	collidable_30df80 collidable;
	char unknown20[0x40 - 0x20];
	bool fixed;
	char unknown41[0x44 - 0x41];
	collector_3173f0 *group;
};

class owner_30c8e0 : public world_312590
{
public:
	void function_30df80(member_30c8e0 *member);
};

// Install the recorder on the current dispatcher, then reload the dispatcher
// after callbacks before restoring its normal handler.
// @retail 0x30df80
void owner_30c8e0::function_30df80(member_30c8e0 *member)
{
	if (member->collidable.shape == 0)
	{
		return;
	}
	pairs_30d870 added(pair_capacity_234);
	bounds_312800 bounds;
	collidable_30df80 *const collidable = &member->collidable;
	shape_30df80 *const shape = collidable->shape;
	float const tolerance_value = tolerance->value;
	shape->get_bounds(member->collidable.motion->transform, tolerance_value * 0.5f, &bounds);
	pair_finder->slot_1(&collidable->handle, &bounds, &added);
	if (added.size > 0)
	{
		if (member->fixed)
		{
			holder_30dc80 recorder(collidable);
			dispatcher_3728c0 *receiver_store = dispatcher;
			receiver_store->handler = (pair_handler_3173f0 *)&recorder;
			dispatcher->function_3728c0(added.data, added.size, 0, 0, pair_listener);
			dispatcher_3728c0 *receiver_restore = dispatcher;
			receiver_restore->handler = &receiver_restore->own_handler;
			for (int i = 0; i < recorder.values.size; i++)
			{
				group_30dc80 *group = recorder.values.data[i];
				pairs_30df80 *pending = &group->pending;
				((collector_3173f0 *)group)->function_3173f0((pair_30d870 *)group->pending.data, group->pending.size, 0, 0);
				pending->resize(0);
			}
		}
		else
		{
			collector_3173f0 *group = member->group;
			group->function_3173f0(added.data, added.size, 0, 0);
		}
	}
}
