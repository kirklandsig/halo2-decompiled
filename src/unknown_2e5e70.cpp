// @flags /O2 /GL-
#include "dispatcher_views_2e5990.h"

// Rebuild registered handlers before changing the alternate-type graph,
// remove duplicate edges, propagate the mask, then append the new edge.
// @retail 0x2e5e70
void s_1c3a40::function_2e5e70(long type, long alternate)
{
	if (field_4d8c)
	{
		((table_2e5990 *)this)->reset();
		unsigned char saved_flag = field_4d8d;
		for (long i = 0; i < registrations.size; ++i)
		{
			registration_2e5d50 const &entry = registrations[i];
			field_4d8d = entry.flag;
			function_2e5d50(&entry.handlers, entry.first_type, entry.second_type);
		}
		field_4d8d = saved_flag;
	}
	for (long i = 0; i < alternate_types.size; ++i)
	{
		if (alternate_types[i].type == type && alternate_types[i].alternate == alternate)
		{
			--alternate_types.size;
			for (long j = i; j < alternate_types.size; ++j)
				alternate_types[j] = alternate_types[j + 1];
			--i;
		}
	}
	((table_2e5a20 *)this)->function_2e5ce0(type, alternate, 0);
	alternate_type_2e5d50 &entry = alternate_types.next_slot();
	entry.type = type;
	entry.alternate = alternate;
}
