// @flags /O2 /Gr
/* UNKNOWN_1E46C0.CPP: the actor iterator (lane I's outside function; 12
   callers, among them the props code's 0x25db60) */

#include "unknown_11c920.h"
#include "unknown_1e46c0.h"

/* the actor as the iterator reads it */
struct s_actor_iterator_datum
{
	byte unknown0[9];
	bool active;
};

/* the next datum of the iterator's data array, kept in the iterator */
static inline bool actor_data_iterator_next(s_actor_iterator *iterator)
{
	byte *datum = data_iterator_next_inlined(&iterator->iterator);

	iterator->actor = datum;
	return datum != 0;
}

/* the next actor (only active ones if the iterator asks for them) */
// @retail 0x1e46c0
void *function_1e46c0(s_actor_iterator *iterator)
{
	void *result = NULL;

	if (g_4f55d0->active)
	{
		while (actor_data_iterator_next(iterator))
		{
			s_actor_iterator_datum *actor = (s_actor_iterator_datum *)iterator->actor;

			if (!iterator->active_only || actor->active)
			{
				result = actor;
				break;
			}
		}
		iterator->actor_index = iterator->iterator.datum_index;
	}
	return result;
}
