// @flags /O2 /GL- /GX
// Havok (third-party physics library): listing the pairs of ids of bodies
// that are joined: the bodies each joining object lists, paired with the
// first movable one; each constraint's two bodies; and each linked pair's two
// bodies; in every case only when both bodies are movable.
#include "growable_array_2d91d0.h"

// a body: whether it is fixed, and its id
struct body_317d90
{
	char unknown0[0x40];
	bool fixed;
	char unknown41[0x58 - 0x41];
	long id;
};

struct id_pair_317d90
{
	long first;
	long second;
};

ELEMENTS_2D91D0(id_pairs_317d90, id_pair_317d90);

// room for one more pair at the end
inline id_pair_317d90 *added_pair_317d90(id_pairs_317d90 *pairs)
{
	long const size = pairs->size;
	pairs->resize(size + 1);
	return &pairs->data[pairs->size - 1];
}

// a list of bodies that starts out in the room for ten held inside it
struct bodies_317d90
{
	body_317d90 **data;
	long size;
	long capacity_and_flags;
	body_317d90 *embedded[10];

	body_317d90 *at(long index) const { return data[index]; }
	bodies_317d90()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x8000000a;
	}
	~bodies_317d90()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(body_317d90 *), 0x12);
		}
	}
};

// an object joining bodies, which lists them through its seventh slot
class joint_317d90
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void list_bodies(bodies_317d90 *bodies) {}
};

struct constraint_317d90
{
	char unknown0[0xc];
	body_317d90 *first;
	body_317d90 *second;
};

struct end_317d90
{
	char unknown0[0x10];
	body_317d90 *body;
};

struct link_317d90
{
	end_317d90 *first;
	end_317d90 *second;
	char unknown8[8];
};

class class_317d90
{
public:
	char unknown0[0x4c];
	joint_317d90 **joints;
	long joint_count;
	char unknown54[4];
	constraint_317d90 **constraints;
	long constraints_used;
	char unknown60[8];
	link_317d90 *links;
	long link_count;

	long joints_used() const { return joint_count; }
	void function_317d90(id_pairs_317d90 *pairs) const;
};

// score 59 (was 59): initial count load and scan width/registers still differ.
// Earlier at 59: retail loads the joint count into a register before
// comparing it, ours compares the member in place; retail compares the found
// index as a full word, ours as a sixteen bit value, so ours sign extends it
// when it indexes the bodies
// count/data snapshots 112, reverse declarations 112, count alone 106, data
// alone 112; stored char 59; forced append 59, shared resize 59, element
// result 59; typed members 59, full scan 106, direct members 106, body
// indexer 106. Initial count load and scan allocation still differ.
// Ordered full-width while 106; array base/ref fill 106; size/index accessors
// 106; byte accessor 161; pair reference 161; bool storage 161; separate
// counter scopes 128; stored byte object 128; scan-only accessor 161;
// plain bool accessor 106. Bottom joint tests already reload the member.
// Bound-first negative sentinel 208; int counts and full-width scan 106;
// cdecl append 59; append member 59; renamed full-width counters 106;
// distinct outer counters with short scan 59; break scan 147;
// one element loop 216. Initial count load and scan allocation remain.
// Separate constructor/destructor/both definitions 59/59/59; capacity check then append 191;
// with wide scan 202; slot before size store 59, with wide scan 192; abstract fill 59.
// List before reset 69; index before list 64; both 81; reset and scan order 70; reset/full-width sentinel 202;
// reset/ordered wide 203; early-index/ordered wide 198; all initialization boundaries 203. Narrow-assignment
// width diagnostics were exact-only.
// score 0 (was 59): matched with `first` a long, the joint count and the body list read through
// accessors, and the pair appended by resizing from the old size.
// Before, 59 (was 59): initial count load and full-width scan allocation remain.
// Short while/consuming scan/counter-first declarations 59/62/60; list member initialization 59;
// old-index append 243; full-width old-index append 248; initialized full-width append 248; counter-first append 243.
// @retail 0x317d90
void class_317d90::function_317d90(id_pairs_317d90 *pairs) const
{
	pairs->size = 0;
	long i;
	{
		bodies_317d90 bodies;
		for (i = 0; i < joints_used(); i++)
		{
			bodies.size = 0;
			joints[i]->list_bodies(&bodies);
			long j = 0;
			long first = -1;
			for (; first == -1 && j < bodies.size; j++)
			{
				if (!bodies.at(j)->fixed)
				{
					first = j;
				}
			}
			for (; j < bodies.size; j++)
			{
				if (!bodies.data[j]->fixed)
				{
					id_pair_317d90 *pair = added_pair_317d90(pairs);
					pair->first = bodies.data[first]->id;
					pair->second = bodies.at(j)->id;
				}
			}
		}
	}
	for (i = 0; i < constraints_used; i++)
	{
		if (!constraints[i]->first->fixed && !constraints[i]->second->fixed)
		{
			id_pair_317d90 *pair = added_pair_317d90(pairs);
			pair->first = constraints[i]->first->id;
			pair->second = constraints[i]->second->id;
		}
	}
	for (i = 0; i < link_count; i++)
	{
		body_317d90 *first = links[i].first->body;
		body_317d90 *second = links[i].second->body;
		if (!first->fixed && !second->fixed)
		{
			id_pair_317d90 *pair = added_pair_317d90(pairs);
			pair->first = first->id;
			pair->second = second->id;
		}
	}
}
