// @flags /O2 /GL-
// Havok (third-party physics library): puts one object in a whole row and a
// whole column of an 8x8 table of reference-counted objects.
#include "reference_counted_2d88a0.h"
#include "agent_table_2e5d50.h"

static inline void assign_2e5a20(referenced_2d88a0 *&slot, referenced_2d88a0 *object)
{
	object->reference_count++;
	referenced_2d88a0 *old = slot;
	if (--old->reference_count == 0)
	{
		delete old;
	}
	slot = object;
}

// The same object as s_1c3a40 (include/agent_table_2e5d50.h), seen from these two functions.
class table_2e5a20
{
public:
	char unknown0[0xc];
	referenced_2d88a0 *entry[8][8];
	unsigned long masks[32];
	char unknown18c[0x4d90 - 0x18c];
	alternate_types_2e5d50 alternate_types;

	void set_all(referenced_2d88a0 *object, long index);
	void function_2e5ce0(long target, long source, long depth);
};

// @retail 0x2e5a20
void table_2e5a20::set_all(referenced_2d88a0 *object, long index)
{
	for (int i = 0; i < 8; i++)
	{
		assign_2e5a20(entry[i][index], object);
		assign_2e5a20(entry[index][i], object);
	}
}

#include "agent_table_2e5d50.h"
#include "string_helpers_2d8c90.h"

char const *__cdecl function_2e6220(long type);

// Expands alternate shape types before replacing the selected handler rows.
// Diagnostics describe this recursive pair; the record keeps the original
// registration pair. Resolve the four names in retail call order.
// @retail 0x2e5a90
void s_1c3a40::function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type,
	long first_type_again, long second_type_again, long level)
{
	field_4d8c = 1;
	for (long index = 0; index < alternate_types.size; ++index)
	{
		alternate_type_2e5d50 const &alternate = alternate_types.data[index];
		if (alternate.alternate == first_type || alternate.alternate == second_type)
		{
			if (alternate.alternate == first_type)
				function_2e5a90(handlers, alternate.type, second_type, first_type_again, second_type_again, level + 1);
			else
				function_2e5a90(handlers, first_type, alternate.type, first_type_again, second_type_again, level + 1);
		}
	}
	long first_begin = first_type;
	long second_begin = second_type;
	long first_end = first_type + 1;
	long second_end = second_type + 1;
	long registration_level = level;
	if (first_type == -1)
	{
		first_begin = 1;
		first_end = 32;
		++registration_level;
	}
	if (second_type == -1)
	{
		second_begin = 1;
		second_end = 32;
		++registration_level;
	}
	for (long first = first_begin; first < first_end; ++first)
	{
		for (long second = second_begin; second < second_end; ++second)
		{
			for (long slot = 0; slot < 4; ++slot)
				handler_tables[slot][first][second] = handlers->handler[slot];
			pair_record_2e5a90 &record = pair_records[first][second];
			if (field_4d8d && registration_level > record.level)
			{
				char buffer[1024];
				char const *old_first = function_2e6220(record.first_type);
				char const *old_second = function_2e6220(record.second_type);
				char const *new_first = function_2e6220(first_type);
				char const *new_second = function_2e6220(second_type);
				function_2d8ca0(buffer, 1000,
					"Agent handling types <%s-%s> would override more specialized agent <%s-%s>\n"
					"Maybe the order of registering your collision agent is wrong, make sure you register your alternate type agents first",
					new_first, new_second, old_first, old_second);
			}
			record.level = registration_level;
			record.first_type = first_type_again;
			record.second_type = second_type_again;
		}
	}
}

// @retail 0x2e5ce0
void table_2e5a20::function_2e5ce0(long target, long source, long depth)
{
	masks[target] |= masks[source];
	for (long i = 0; i < alternate_types.size; i++)
	{
		alternate_type_2e5d50 const &edge = alternate_types[i];
		if (edge.alternate == target)
		{
			function_2e5ce0(edge.type, edge.alternate, depth + 1);
		}
	}
}
