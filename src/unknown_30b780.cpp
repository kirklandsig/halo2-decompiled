// @flags /O2 /GL-
// Havok (third-party physics library): an object placed by a transform given
// another shape: it takes a reference to the new one and drops the old one,
// and in a world its bounds are brought up to date.
#include "placed_shape_312800.h"

bool __cdecl function_279740(long value);

class deletable_30b780
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// @retail 0x30b780
void placed_shape_312800::function_30b780(shape_312800 *shape)
{
	shape->reference_count++;
	field_54 = shape->slot_5();
	referenced_2d88a0 *old = body.shape;
	if (--old->reference_count == 0)
	{
		if (function_279740((long)old))
		{
			old->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_30b780 *)old)->destroy(1);
		}
	}
	body.shape = shape;
	if (field_8)
	{
		bounds_312800 bounds;
		shape->get_bounds(transform, ((world_312590 *)field_8)->tolerance->value, &bounds);
		function_315a10(&bounds);
	}
}
