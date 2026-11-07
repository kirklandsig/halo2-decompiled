// @flags /O2 /GL-
// Havok (third-party physics library): puts one object in a whole row and a
// whole column of an 8x8 table of reference-counted objects.
#include "dispatcher_views_2e5990.h"
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

// score 166 (was unwritten; 174 with the alternate tests nested under a joint test, 166 as if/else-if):
// frame 0x410 versus 0x424; the type, row and
// text-call values use other registers and stack slots. Buffer extent 174;
// ordered text locals 159; separate level 175; both bounds 171; first bound
// 156; second bound 169; row/column names 171; array indexing 171; direct
// alternate fields 171; early bounds 195; record stores reordered 171;
// separate maps 171; record pointer 171; direct record fields 175; direct
// arguments with both bounds 178, first bound 189, separate level 189;
// branch bounds 141, with direct arguments 189. Text locals were reverted.
// @retail 0x2e5a90
void s_1c3a40::function_2e5a90(handlers_2e5d50 const *handlers, long first_type, long second_type,
	long first_type_again, long second_type_again, long level)
{
	field_4d8c = 1;
	for (long index = 0; index < alternate_types.size; ++index)
	{
		alternate_type_2e5d50 const &alternate = alternate_types.data[index];
		if (alternate.alternate == first_type)
			function_2e5a90(handlers, alternate.type, second_type, first_type_again, second_type_again, level + 1);
		else if (alternate.alternate == second_type)
			function_2e5a90(handlers, first_type, alternate.type, first_type_again, second_type_again, level + 1);
	}
	long first_end = first_type + 1;
	long second_end = second_type + 1;
	if (first_type == -1)
	{
		first_type = 1;
		first_end = 32;
		++level;
	}
	if (second_type == -1)
	{
		second_type = 1;
		second_end = 32;
		++level;
	}
	for (long first = first_type; first < first_end; ++first)
	{
		for (long second = second_type; second < second_end; ++second)
		{
			for (long slot = 0; slot < 4; ++slot)
				handler_tables[slot][first][second] = handlers->handler[slot];
			pair_record_2e5a90 &record = pair_records[first][second];
			if (field_4d8d && level > record.level)
			{
				char buffer[1024];
				function_2d8ca0(buffer, 1000,
					"Agent handling types <%s-%s> would override more specialized agent <%s-%s>\n"
					"Maybe the order of registering your collision agent is wrong, make sure you register your alternate type agents first",
					function_2e6220(first_type_again), function_2e6220(second_type_again),
					function_2e6220(record.first_type), function_2e6220(record.second_type));
			}
			record.first_type = first_type_again;
			record.second_type = second_type_again;
			record.level = level;
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
