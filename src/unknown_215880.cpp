#include "unknown_11c920.h"
#include "loop_allocator.h"
#include <new>

// @flags /O2 /Gr

class c_location_record
{
public:
	c_location_record();
	char path[0x14];
	word name[0x16];
};

struct s_location_storage_table
{
	__forceinline s_location_storage_table() : count(0) {}
	long count;
	c_location_record entries[0x1000];
};

struct s_location_storage
{
	__forceinline s_location_storage() : count(0) {}
	long count;
	byte unknown004[0xbef8 - 4];
	/* one table per storage device (the users index it by memory unit); this build has one */
	s_location_storage_table locations[1];
	long generation;
	byte unknown4bf00[0x6bf00 - 0x4bf00];
};

extern void *g_51ea14;
long g_55c150;
long g_55c15c;

// Matches as a link-time object. The tables are an array of one: the compiler constructs it with its vector
// constructor iterator, expands that call here, and then cannot expand the same iterator again for the table's
// records, so retail's call to 0x20670 with the record constructor stays (a lone table member has the record loop
// expanded instead). Retail reads g_55c15c again after the store through the new cache: the global lies in the block
// 0x215690 clears by address (0x55c030..0x55c284), so a store through a pointer may change it. Here it is a
// separate global, and binding it to a reference is what takes its address.
// @retail 0x215880
void function_215880(void *reference)
{
	void *buffer = ((c_memory_source *)reference)->allocate(sizeof(s_location_storage));
	s_location_storage *storage = buffer ? new (buffer) s_location_storage : NULL;
	long &next_generation = g_55c15c;
	storage->generation = next_generation;
	g_51ea14 = storage;
	next_generation = (next_generation + 1) % 512;
	g_55c150 = 3;
}
