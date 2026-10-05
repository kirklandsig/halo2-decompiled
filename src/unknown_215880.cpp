#include "unknown_11c920.h"
#include "loop_allocator.h"
#include <new>

// @flags /O2 /Gr /GL-

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
	s_location_storage_table locations;
	long generation;
	byte unknown4bf00[0x6bf00 - 0x4bf00];
};

extern void *g_51ea14;
long g_55c150;
long g_55c15c;

// @retail 0x215880
void function_215880(void *reference)
{
	void *buffer = ((c_memory_source *)reference)->allocate(sizeof(s_location_storage));
	s_location_storage *storage = buffer ? new (buffer) s_location_storage : NULL;
	storage->generation = g_55c15c;
	g_51ea14 = storage;
	g_55c15c = (g_55c15c + 1) % 512;
	g_55c150 = 3;
}
