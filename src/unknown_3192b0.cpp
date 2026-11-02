// @flags /O2 /GL- /Ob1
// Havok (third-party physics library): the shared object of kind 1, built
// with one reference and destroyed as its base is.
// The global instance and allocated instances use the same lifetime interface.
#include "kind_object_3192b0.h"
#include <new>

kind_object_3192b0 g_480bb8;

// @retail 0x3192b0
kind_reference_3192b0::~kind_reference_3192b0()
{
}

// @retail 0x3192c0
__forceinline kind_object_3192b0::kind_object_3192b0()
{
}

// The deleting wrapper calls the shared teardown before optionally releasing storage.
// @retail 0x3192d0 deleting kind_object_3192b0

// @retail 0x31c4f0
kind_object_3192b0 *__cdecl function_31c4f0()
{
	kind_object_3192b0 *object = (kind_object_3192b0 *)g_memory_2d88a0->allocate_block(
		sizeof(kind_object_3192b0), 0x26);
	object->flags = sizeof(kind_object_3192b0);
	return ::new (object) kind_object_3192b0;
}
