// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object's overlaps brought up to
// date with its new bounds: the broad phase finds the pairs that begin and
// end, which are put in order and passed on, and the object's array of
// overlapping objects loses the entries that were cleared.
#include "placed_shape_312800.h"

// drops the null entries from an array of pointers, the others kept in order
// @retail 0x3158d0
static void function_3158d0(pointers_312800 *array)
{
	for (int k = array->size - 1; k >= 0; k--)
	{
		if (array->data[k] == 0)
		{
			array->size--;
			for (int j = k; j < array->size; j++)
			{
				array->data[j] = array->data[j + 1];
			}
		}
	}
}

// passes on the pairs that begin and end
inline void __cdecl dispatch_315a10(dispatcher_3728c0 *dispatcher, pairs_30d870 const &added, pairs_30d870 const &removed, pair_listener_315a10 *listener)
{
	dispatcher->function_3728c0(added.data, added.size, removed.data, removed.size, listener);
}

// @retail 0x315a10
void overlapped_312800::function_315a10(bounds_312800 const *bounds)
{
	if (field_8)
	{
		pairs_30d870 added(((world_312590 *)field_8)->pair_capacity);
		pairs_30d870 removed(((world_312590 *)field_8)->pair_capacity);
		handle_315a10 *handle = (collidable_315a10 *)&body.shape;
		((world_312590 *)field_8)->pair_finder->update_bounds((void **)&handle, bounds, 1, &added, &removed);
		if (added.size != 0 || removed.size != 0)
		{
			function_372b50(added.data, added.size);
			function_372b50(removed.data, removed.size);
			world_312590 *world = (world_312590 *)field_8;
			dispatch_315a10(world->dispatcher, added, removed, world->pair_listener);
			function_3158d0(&field_3c);
		}
	}
}
