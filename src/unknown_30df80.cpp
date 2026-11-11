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
// score 17 (was 29): the collidable's address is now a local of its own, so
// retail's ebx and edi hold the member and the collidable as retail has them;
// the dispatcher and the pair's data still swap registers, and the motion is
// still read through the member instead of the collidable
// an external object reference with direct array reads 15; inherited array
// storage 15; early pointer declarations with either form 15; a count saved
// before the branch 131, with an inline float argument 134; inline depth one 15.
// Further trials: common pair type 17; that type with the motion read
// through the embedded record 35; shared shape/transform types 17, with that
// motion read 35; a count sampled after the handler store 17; an external
// reference for restoration 17; a receiver base of its own type 17, with that motion
// read 35. The original body remains; frame 0x68 and length 442 agree.
// further trials: bounds first 17; pending data/count through the group 19;
// listener conversion 17, before the handler 24; tolerance argument 34;
// member group directly 36; pending member receiver 19; empty-pair guard 17.
// scoped bounds/array and initial pointers 127; inline tolerance and shape
// reread 34; motion through collidable 53; direct pending operands 55;
// object names or mutable pointers 55; placed after array/recorder helpers
// 55, with original body 17
// separate receiver snapshots 3; direct group fields with those snapshots 1;
// motion through the pointer then 19 (the last receiver moves to ecx);
// outer receiver declaration, shared shape types, first pointer declaration,
// renamed last receiver or reference receiver 19; early shape snapshot 64;
// second receiver use 15, two multiple-use receivers 63, restore-only receiver 15;
// scaled float local 133; inline factor or combined outer scope 68;
// 1/3 forms require one-use field pointers and remain rejected
// Review cleanup score 19: removed unsupported volatile/one-use tuning;
// the prior scored trials above remain available for future matching work.
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
			dispatcher->handler = (pair_handler_3173f0 *)&recorder;
			dispatcher->function_3728c0(added.data, added.size, 0, 0, pair_listener);
			dispatcher->handler = &dispatcher->own_handler;
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
