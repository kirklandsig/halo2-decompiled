#ifndef KIND_OBJECT_3192B0_H
#define KIND_OBJECT_3192B0_H
// Havok (third-party physics library): the shared object of kind 1 (vtable
// 0x415100, its kind query in the game's 0x9c7c0) that an object set to that
// kind takes (0x311690): built with one reference when the program starts
// (0x3192c0), and destroyed at exit (0x3192b0).
#include "unknown_0a9f20.h"
#include "reference_counted_2d88a0.h"

// Shared teardown has no table transition before the reference-base call.
class __declspec(novtable) kind_reference_3192b0 : public c_reference_a9f20
{
public:
	virtual ~kind_reference_3192b0();
};

class kind_object_3192b0 : public kind_reference_3192b0
{
public:
	kind_object_3192b0();

	static void operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((c_sized_reference *)object)->flags, 0x26);
	}
};

extern kind_object_3192b0 g_480bb8;

#endif
