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

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *arg_0, unsigned arg_1, int arg_2, constructor_proc arg_3);

PRIVATE void *__fastcall function_215881(void *arg_0)
{
	return new (arg_0) c_location_record;
}

struct s_215880
{
	long field_0;
	byte field_4[0xbef8 - 4];
	long field_bef8;
	byte field_befc[0x40000];
	long field_4befc;
	byte field_4bf00[0x20000];
	__forceinline s_215880()
	{
		field_0 = 0;
		field_bef8 = 0;
		vector_constructor_iterator(field_befc, sizeof(c_location_record), 0x1000, function_215881);
	}
};

// @retail 0x215880
void function_215880(void *reference)
{
	void *buffer = ((c_memory_source *)reference)->allocate(sizeof(s_215880));
	s_215880 *storage = buffer ? new (buffer) s_215880 : NULL;
	storage->field_4befc = g_55c15c;
	g_51ea14 = storage;
	g_55c15c = (g_55c15c + 1) % 512;
	g_55c150 = 3;
}
