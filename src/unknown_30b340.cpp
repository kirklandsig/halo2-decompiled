// @flags /O2 /GL-
// Havok (third-party physics library): a reference-counted object's
// constructor: it takes a reference to the object it is given, and points
// its list's owner at itself.
#include "world_object_30b340.h"

// @retail 0x30b340
class_30b340::class_30b340(referenced_2d88a0 *object_, long value)
	: field_8(0), body(object_), handle(value)
{
	list.owner = this;
	if (object_)
	{
		object_->reference_count++;
	}
}
