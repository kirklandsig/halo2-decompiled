// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying an object that tells its
// listeners it is going.
#include "listened_object_315950.h"

// @retail 0x315950
class_315950::~class_315950()
{
	// The count is read once into a named local; the index starts one below it.
	int const size = listeners.size;
	for (int i = size - 1; i >= 0; i--)
	{
		listener_315950 *listener = listeners.data[i];
		if (listener)
		{
			listener->object_destroyed(this);
		}
	}
}
