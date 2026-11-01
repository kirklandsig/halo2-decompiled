// @flags /O2 /GL- /GX
#include "growable_array_2d91d0.h"
#include "sized_reference_a9ef0.h"
bool __cdecl function_279740(long value);
#include "physics_allocator.h"

// Pointer array used by the library singleton registration passes.
struct lane_a_registration
{
	c_sized_reference_a9ef0 *(__cdecl *create)();
	lane_a_registration *next;
	c_sized_reference_a9ef0 **storage;
};
struct lane_a_registration_array
{
	lane_a_registration **data;
	long size;
	long capacity_and_flags;
	lane_a_registration_array() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~lane_a_registration_array();
	void append(lane_a_registration *entry)
	{
		if (size == (capacity_and_flags & 0x7fffffff))
			function_2d91d0(this, sizeof(*data));
		data[size++] = entry;
	}
};

// @retail 0x2d7540
lane_a_registration_array::~lane_a_registration_array()
{
	if ((capacity_and_flags & 0x80000000) == 0)
	{
		havok_allocator()->release(data,
			(capacity_and_flags & 0x7fffffff) * 4, 0x12);
	}
}

lane_a_registration *g_480174;

// Create services whose dependencies are ready, postponing failed factories
// until a later pass. Successful retries rejoin the registration list.
// @retail 0x2d7ca0
void __cdecl initialize_registered_services_2d7ca0()
{
	lane_a_registration **link = &g_480174;
	lane_a_registration *entry = *link;
	lane_a_registration_array pending;
	while (entry)
	{
		if (!*entry->storage)
		{
			c_sized_reference_a9ef0 *object = entry->create();
			if (object)
			{
				*entry->storage = object;
			}
			else
			{
				pending.append(entry);
				lane_a_registration *next = entry->next;
				(*link)->next = 0;
				*link = next;
				entry = next;
				continue;
			}
		}
		link = &entry->next;
		entry = *link;
	}
	while (pending.size)
	{
		for (long i = pending.size - 1; i >= 0; --i)
		{
			entry = pending.data[i];
			c_sized_reference_a9ef0 *object = entry->create();
			if (object)
			{
				*entry->storage = object;
				*link = entry;
				link = &entry->next;
				--pending.size;
				pending.data[i] = pending.data[pending.size];
			}
		}
	}
}

// @retail 0x2d7dd0
void __cdecl function_2d7dd0()
{
	lane_a_registration_array entries;
	function_2d9160(&entries, 16, sizeof(*entries.data));
	for (lane_a_registration *entry = g_480174; entry; entry = entry->next)
		entries.append(entry);
	for (long i = entries.size - 1; i >= 0; --i)
	{
		c_sized_reference_a9ef0 *object = *entries.data[i]->storage;
		object->reference_count--;
		if (!object->reference_count)
		{
			if (function_279740((long)object))
				object->reference_count = 1;
			else
				delete object;
		}
		*entries.data[i]->storage = 0;
	}
}
