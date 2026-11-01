// @flags /O2 /GL- /GX
// Havok (third-party physics library): the world's destructor. Pending work is
// flushed, every volume and body is taken out of the world, the bodies waiting
// for removal, the bodies' groups the step kept and the world's other counted
// objects are let go, the owned helpers are freed, and then the arrays.
#include "unknown_1cec30.h"
#include "unknown_1efac0.h"
#include "growable_array_2d91d0.h"

bool __cdecl function_279740(long value);

struct listener_world_318320;
void __cdecl function_3184d0(listener_world_318320 *world);

class class_30f030
{
public:
	void function_30f6b0();
};

class class_30d320
{
public:
	void function_30d400();
};

class c_contact_query_world
{
public:
	void *add(void *volume);
	void remove(void *volume);
};

// the deleting destructor, first in the vtable
class deletable_310a70
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// drops a reference; the last one deletes the object unless the game keeps it
inline void release_310a70(referenced_2d88a0 *counted)
{
	counted->reference_count--;
	if (counted->reference_count == 0)
	{
		if (function_279740((long)counted))
		{
			counted->reference_count = 1;
		}
		else
		{
			((deletable_310a70 *)counted)->destroy(1);
		}
	}
}

#include "slot_table_372a40.h"

// the step's settings: a block of its own and itself, both with tag 0x1a
struct settings_310a70
{
	char unknown0[0xc];
	void *block;
};

#define ARRAY_310A70(name, type) \
	struct name \
	{ \
		type *data; \
		long size; \
		unsigned long capacity_and_flags; \
		name() : data(0), size(0), capacity_and_flags(0x80000000) {} \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				type *block = data; \
				g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(type), 0x12); \
			} \
		} \
	};

struct pair_310a70
{
	long first;
	long second;
};

ARRAY_310A70(groups_310a70, c_havok_body_group *)
ARRAY_310A70(pairs_310a70, pair_310a70)
ARRAY_310A70(pointers_310a70, void *)
ARRAY_310A70(counted_310a70, referenced_2d88a0 *)

// a counted object that may hold a switch to turn off when it goes
class switched_310a70 : public referenced_2d88a0
{
public:
	virtual ~switched_310a70()
	{
		if (memory_size & 0x8000)
		{
			memory_size &= 0x7fff;
			// the same object as the library's c_a, whose virtual retail calls directly
			((c_a *)(void *)this)->c_a::v2(false);
		}
	}
};

class class_310a70 : public switched_310a70
{
public:
	REFERENCED_NEW_2D88A0(0x2a)
	virtual ~class_310a70();

	groups_310a70 active_groups;
	groups_310a70 inactive_groups;
	// operations waiting for the world to unlock
	pairs_310a70 pending;
	c_havok_body_group *fixed_group;
	char unknown30[8];
	hkEntity *fixed_body;
	pointers_310a70 array_3c;
	pointers_310a70 array_48;
	pointers_310a70 array_54;
	pointers_310a70 array_60;
	pointers_310a70 array_6c;
	pointers_310a70 array_78;
	pointers_310a70 array_84;
	pointers_310a70 array_90;
	pointers_310a70 array_9c;
	pointers_310a70 array_a8;
	pointers_310a70 array_b4;
	char unknownc0[4];
	referenced_2d88a0 *counted_c4;
	referenced_2d88a0 *counted_c8;
	settings_310a70 *settings;
	referenced_2d88a0 *counted_d0;
	char unknownd4[0xf0 - 0xd4];
	counted_310a70 waiting;
	pointers_310a70 array_fc;
	pointers_310a70 array_108;
	counted_310a70 released;
	pointers_310a70 volumes;
	bool locked;
	char unknown12d[0x138 - 0x12d];
	helper_3728b0 *helper;
	deletable_310a70 *owned_13c;
	char unknown140[0x250 - 0x140];
	referenced_2d88a0 *counted_250;
	deletable_310a70 *owned_254;
};

// @retail 0x310a70
class_310a70::~class_310a70()
{
	if (pending.size > 0)
	{
		((class_30f030 *)this)->function_30f6b0();
	}
	locked = false;
	while (volumes.size > 0)
	{
		((c_contact_query_world *)this)->remove(volumes.data[0]);
	}
	while (active_groups.size > 0)
	{
		((hkWorld *)this)->entity_world_remove(active_groups.data[0]->m_entities[0]);
	}
	while (inactive_groups.size > 0)
	{
		((hkWorld *)this)->entity_world_remove(inactive_groups.data[0]->m_entities[0]);
	}
	if (waiting.size != 0)
	{
		for (int i = 0; i < waiting.size; i++)
		{
			release_310a70(waiting.data[i]);
		}
		waiting.size = 0;
	}
	((hkWorld *)this)->entity_world_remove(fixed_body);
	release_310a70((referenced_2d88a0 *)fixed_body);
	fixed_body = 0;
	while (fixed_group->m_entity_count > 0)
	{
		((hkWorld *)this)->entity_world_remove(fixed_group->m_entities[0]);
	}
	if (fixed_group != 0)
	{
		((deletable_310a70 *)fixed_group)->destroy(1);
	}
	fixed_group = 0;
	for (int i = 0; i < released.size; i++)
	{
		((class_30d320 *)released.data[i])->function_30d400();
		release_310a70(released.data[i]);
	}
	released.size = 0;
	function_3184d0((listener_world_318320 *)this);
	release_310a70(counted_c4);
	counted_c4 = 0;
	release_310a70(counted_c8);
	counted_c8 = 0;
	release_310a70(counted_250);
	delete helper;
	if (owned_13c != 0)
	{
		owned_13c->destroy(1);
	}
	release_310a70(counted_d0);
	void *settings_block = settings->block;
	g_memory_2d88a0->release_block(settings_block, 0x20, 0x1a);
	settings_310a70 *settings_itself = settings;
	g_memory_2d88a0->release_block(settings_itself, 0x10, 0x1a);
	if (owned_254 != 0)
	{
		owned_254->destroy(1);
	}
}
