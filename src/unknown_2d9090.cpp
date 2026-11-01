// @flags /O2 /GL-
// Havok (third-party physics library): an object that holds a reference to
// another reference-counted object.
#include "stream_holder_2d8e70.h"

// @retail 0x2d9090
stream_holder_2d8e70::stream_holder_2d8e70(referenced_2d88a0 *object)
{
	stream = object;
	object->reference_count++;
}
