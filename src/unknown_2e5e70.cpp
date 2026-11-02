// @flags /O2 /GL-
/* UNKNOWN_2E5E70.CPP: records that one shape type is handled as another
   (s_1c3a40::function_2e5e70). A dispatcher whose tables are built first
   rebuilds them from its registrations, each with its own flag; then any
   earlier record of the same pair goes, the alternate's handlers spread to
   the type (0x2e5ce0), and the pair is recorded. */
#include "agent_table_2e5d50.h"

/* the same object, as 0x2e5990 resets its tables and 0x2e5ce0 spreads them */
struct table_2e5990
{
	void reset();
};

struct table_2e5a20
{
	void function_2e5ce0(long target, long source, long depth);
};

// @retail 0x2e5e70
void s_1c3a40::function_2e5e70(long type, long alternate)
{
	if (field_4d8c)
	{
		((table_2e5990 *)this)->reset();
		unsigned char flag = field_4d8d;
		for (int i = 0; i < registrations.size; i++)
		{
			registration_2e5d50 *registration = &registrations.data[i];
			field_4d8d = registration->flag;
			function_2e5d50(&registration->handlers, registration->first_type, registration->second_type);
		}
		field_4d8d = flag;
	}
	for (int i = 0; i < alternate_types.size; i++)
	{
		if (alternate_types.data[i].type == type && alternate_types.data[i].alternate == alternate)
		{
			alternate_types.size--;
			for (int j = i; j < alternate_types.size; j++)
			{
				alternate_types.data[j] = alternate_types.data[j + 1];
			}
			i--;
		}
	}
	((table_2e5a20 *)this)->function_2e5ce0(type, alternate, 0);
	alternate_type_2e5d50 &record = alternate_types.next_slot();
	record.type = type;
	record.alternate = alternate;
}
