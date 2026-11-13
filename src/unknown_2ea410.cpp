// @flags /O2 /GL-
// Havok (third-party physics library): queries of one shape against a shape
// holding a tree of parts that keep an agent for each part the first shape
// overlaps: the agents are brought up to date, then each runs its query
// against its part. Each phase is marked in the timing stream.
#include "pair_agents_2eab70.h"
#include "timer_mark_2ebc80.h"

class shape_2ea410
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	// a container's part, built in the buffer given when it must be
	virtual shape_2ea410 const *part(long key, char *buffer) const { return 0; }
};

// a shape holding a tree over the parts of a container
class tree_shape_2ea410 : public shape_2ea410
{
public:
	long field_4;
	long field_8;
	shape_2ea410 const *container;
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2ea410
{
	shape_2ea410 const *shape;
	long key;
	void const *placement;
	body_2ea410 const *parent;
};

// runs the queries of one part of a pair
class pair_agent_2ea410
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2(void const *a, void const *b, void const *input, void *collector) {}
	virtual void slot_3(void const *a, void const *b, void const *input, void *collector, void *second_collector) {}
	virtual void slot_4(void const *a, void const *b, void const *input, void *collector) {}
};

// score 79 (was 79): Frame 0x114 agrees; extent 270 versus 274. First +0x13
// saves this in ESI rather than EDI; the input stays live across the update,
// and the traversal starts with another table load. Cursor before end 81;
// input view earlier 79; traversal scope 79; parent first 79; indexed count 80;
// indexed end 84; buffer declared first 79; while cursor 79.
// NOT kept: none improves the original body.
// Whole child initialization 89; input record copy 106; scratch inside loop 79;
// record declaration before timing 79; direct input observations 79; immutable
// container pointer 79; per-iteration child record 88; key before shape call 78.
// NOT kept: key-before-call gains one, below threshold; other forms tie or regress.
// P428: parent-field placement 79; end after child stores 79; end after
// timing 78; mutable cursor 79; entry reference 79; vector scratch 79;
// input reference 79; direct countdown 81. NOT kept: the one-point drop
// shortens the body further; the other forms tie or regress.
// @retail 0x2ea410
void class_2ea410::function_2ea410(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("LtBvTree");
	((class_2e96a0 *)this)->function_2e96a0(a, b, input, function_2e90f0);
	agent_entry_2ea410 const *const end = entries + entry_count;
	body_2ea410 const *container_body = (body_2ea410 const *)b;
	body_2ea410 part;
	part.placement = container_body->placement;
	part.parent = container_body;
	timer_add_2ebc80("StNarrowPhase");
	shape_2ea410 const *container = ((tree_shape_2ea410 const *)container_body->shape)->container;
	__declspec(align(16)) char buffer[0x100];
	for (agent_entry_2ea410 const *entry = entries; entry != end; entry++)
	{
		part.shape = container->part(entry->key, buffer);
		part.key = entry->key;
		entry->agent->slot_4(a, &part, input, collector);
	}
	timer_add_2ebc80("lt");
}

// score 25 (was 25): Frame 0x114 and extent 272 agree; the end calculation
// lacks the cursor copy, and the descriptor and virtual-call carriers differ.
// Iterator before end 25; captured count 25; parent first 26; buffer first 25;
// while traversal 25; named key 25; named agent 25; whole initialization 59;
// end after descriptor 28; end after container 32; existing body type 25.
// NOT kept: no match or qualifying gain; original body restored.
// Round 94: mutable end 25; split end 25; vector buffer 25; mutable cursor
// 25; finish name 25; child_body name 25; paired cursor scope 32; direct
// parameter fields 25. NOT kept: these forms tie or regress.
// Shared begin/end methods 25; NOT kept: no gain.
// Serial search: 76 variants in 120 s, none below 25.
// Round 424: parent reference 25; container reference 25; entry reference 25;
// early parent view 25; scratch inside traversal 25; indexed end address 25;
// scratch/child name order 25; early child declaration 25. NOT kept: no gain;
// cursor/end staging and call carriers still differ.
// @retail 0x2ea530
void class_2ea530::function_2ea530(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	timer_add_2ebc80("LtBvTree");
	((class_2e9d90 *)this)->function_2e9d90(a, b, input, function_2e90f0);
	timer_add_2ebc80("StNarrowPhase");
	agent_entry_2ea410 const *const end = entries + entry_count;
	body_2ea410 const *container_body = (body_2ea410 const *)b;
	body_2ea410 part;
	part.placement = container_body->placement;
	part.parent = container_body;
	shape_2ea410 const *container = ((tree_shape_2ea410 const *)container_body->shape)->container;
	__declspec(align(16)) char buffer[0x100];
	for (agent_entry_2ea410 const *entry = entries; entry != end; entry++)
	{
		part.shape = container->part(entry->key, buffer);
		part.key = entry->key;
		entry->agent->slot_3(a, &part, input, collector, second_collector);
	}
	timer_add_2ebc80("lt");
}
