#ifndef LANE_A_CONTACT_MANAGER_H
#define LANE_A_CONTACT_MANAGER_H
#include "reference_base_318970.h"
#include "listener_world_315d20.h"
#include <string.h>
struct listener_world_318320;
void __cdecl function_3183b0(listener_world_318320 *, event_315d20 *);
void __cdecl function_315d90(listener_world_315d20 *, event_315d20 *);
void __cdecl function_318440(listener_world_318320 *, event_315d20 *);
void __cdecl function_315e00(listener_world_315d20 *, event_315d20 *);
// Reuse the canonical listener prefix. The delay occupies the low two bytes
// of its opaque capacity word at5c; byte-copy access avoids an incompatible
// second complete definition of the listener object.
typedef listener_world_315d20 lane_a_contact_entity;
static __forceinline unsigned short lane_a_contact_delay(lane_a_contact_entity const *entity)
{
	unsigned short value;
	memcpy(&value,(unsigned char const *)entity + 0x5c,sizeof(value));
	return value;
}
// Existing generic event_315d20 reserves the first16 bytes as opaque event
// payload. Populate that payload without constructing an incompatible object.
struct lane_a_contact_removed_payload
{
	unsigned short id;
	unsigned short unused;
	void *manager;
	lane_a_contact_entity *a, *b;
};
struct lane_a_contact_body;
struct lane_a_contact_processed_payload
{
	void *manager;
	lane_a_contact_body const *a, *b;
	void const *input;
};
struct lane_a_contact_body
{
	unsigned char unknown0[0x20];
	lane_a_contact_entity *entity;
};
struct lane_a_contact_point;
void __cdecl function_318950(reference_base_318970 *);
class lane_a_simple_contact_manager : public reference_base_318970
{
public:
	static void *operator new(unsigned int size)
	{
		reference_base_318970 *p = (reference_base_318970 *)g_memory_2d88a0->allocate_block(size,0x1d);
		p->memory_size = (unsigned short)size;
		return p;
	}
	static void __cdecl operator delete(void *p) { function_318950((reference_base_318970 *)p); }
	lane_a_simple_contact_manager(listener_world_318320 *owner, lane_a_contact_entity *first, lane_a_contact_entity *second)
		: reference_base_318970(count_untouched)
	{
		// the earlier of the two delays first, then the fields (retail's order)
		unsigned short delay = lane_a_contact_delay(second);
		if (lane_a_contact_delay(first) < delay) delay = lane_a_contact_delay(first);
		world = owner;
		a = first;
		b = second;
		remaining = delay + 1;
	}
	listener_world_318320 *world;
	lane_a_contact_entity *a, *b;
	long remaining;
	virtual unsigned short add(lane_a_contact_body const *, lane_a_contact_body const *, void const *, lane_a_contact_point const *);
	virtual void remove(unsigned short id);
	virtual void processed(lane_a_contact_body const *, lane_a_contact_body const *, void const *, void const *);
	virtual void cleanup();
	virtual bool reserve(void *) { return false; }
	virtual bool has_toi(void *) { return false; }
	virtual void process_toi(void *) {}
};
#endif
