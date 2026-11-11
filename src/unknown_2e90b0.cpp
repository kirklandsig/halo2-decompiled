// @flags /O2 /GL-
// Havok: release the kept child agents before deleting their parent.
#include "bounded_2e9530.h"
#include "child_refresh_2e9210.h"

inline element_2e9530 *end_of(array_2e9530 &array) { return array.data + array.size; }

// @retail 0x2e90b0
void bounded_2e9530::function_2e90b0()
{
	element_2e9530 *entry = elements.data;
	element_2e9530 *end = end_of(elements);
	while (entry != end)
	{
		if (entry->value_8)
		{
			((kept_child_2e9210 *)entry->value_8)->release();
		}
		++entry;
	}

	delete this;
}
