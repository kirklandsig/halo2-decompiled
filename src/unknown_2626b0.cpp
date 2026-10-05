// @flags /O2 /Ob1 /Gr
/* UNKNOWN_2626B0.CPP: the reference an actor follows (actor +0x418) and the
   history of the references it gave up (actor +0x3fe, +0x400) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"

/* the blocks of g_4e0350 (the scenario) at +0x16c (0x38 bytes each) that
   function_262b40 looks references up in */
struct s_reference_range_view
{
	byte unknown00[0x38];
	short first;
	short count;
	byte unknown3c[0x88 - 0x3c];
};

struct s_262b40_block
{
	byte unknown00[0x28];
	long count;
	s_262b40_result *entries;
	byte unknown30[4];
	s_reference_range_view *ranges;
};

struct s_262b40_scenario_view
{
	byte unknown000[0x16c];
	s_262b40_block *blocks;
};

s_262b40_result *__stdcall function_26e030(s_reference reference);
void __stdcall function_1f4280(long actor_index);
bool __stdcall function_1f46f0(long actor_index, short type, s_reference reference, byte *scratch, bool unknown2);
void __stdcall function_2628f0(long actor_index, s_reference reference);

// @retail 0x2626b0
s_reference function_2626b0(long actor_index, s_reference reference, long other_actor_index, byte *scratch, bool unknown2, bool unknown3)
{
	s_actor_view *actor = actor_get(actor_index);

	if (REFERENCE_EQUAL(reference, g_470fa0))
	{
		function_1f4280(actor_index);
		function_2628f0(actor_index, g_470fa0);
		return actor->unknown418;
	}

	if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) && !REFERENCE_EQUAL(actor->unknown418, reference))
		function_262800(actor_index, actor->unknown418, true);

	if (other_actor_index != NONE && record_pool_lookup(g_4f55f0, other_actor_index))
	{
		function_1f4280(other_actor_index);
		function_2628f0(other_actor_index, g_470fa0);
	}

	if (!REFERENCE_EQUAL(actor->unknown418, reference))
	{
		function_2628f0(actor_index, reference);
		actor->unknown3f0 = !unknown2;
		actor->unknown3f1 = false;
	}

	if (unknown3)
	{
		s_262b40_result *result = function_262b40(reference);
		bool started;

		if (result)
		{
			if (result->flags & 0x20)
				started = function_1f46f0(actor_index, 6, reference, unknown2 ? scratch : NULL, false);
			else if (!(result->flags & 0x40))
				started = function_1f46f0(actor_index, 5, reference, unknown2 ? scratch : NULL, true);
			else
				started = function_1f46f0(actor_index, 4, reference, unknown2 ? scratch : NULL, false);

			if (started)
				return actor->unknown418;
		}

		function_2628f0(actor_index, g_470fa0);
	}

	return actor->unknown418;
}

// @retail 0x262800
void function_262800(long actor_index, s_reference reference, bool unknown)
{
	if (!REFERENCE_EQUAL(reference, g_470fa0))
	{
		s_actor_view *actor = actor_get(actor_index);
		s_reference_entry *entries = actor->unknown400;
		bool not_found = true;
		long i = 0;

		do
		{
			if (REFERENCE_EQUAL(reference, entries[i].reference))
			{
				not_found = false;
				if (!unknown)
					entries[i].unknown0 = unknown;
			}
			i++;
		}
		while (i < 4);

		if (not_found)
		{
			actor->unknown3fe = (actor->unknown3fe + 1) % 4;
			entries[actor->unknown3fe].unknown0 = unknown;
			entries[actor->unknown3fe].reference = reference;
		}
	}
}

// @retail 0x262b40
s_262b40_result *function_262b40(s_reference reference)
{
	s_262b40_result *result = NULL;

	if (!REFERENCE_EQUAL(reference, g_470fa0))
	{
		word block_index = reference.unknown2;

		if (block_index & 0x8000)
		{
			result = function_26e030(reference);
		}
		else
		{
			long index = reference.unknown0;
			s_262b40_block *block = &((s_262b40_scenario_view *)g_4e0350)->blocks[block_index];
			if (index >= 0 && index < block->count)
				result = &block->entries[index];
		}
	}

	return result;
}


// @retail 0x262890
bool function_262890(long actor_index, s_reference reference)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (!REFERENCE_EQUAL(reference, g_470fa0))
	{
		for (short i = 0; i < 4; i++)
		{
			if (REFERENCE_EQUAL(reference, actor->unknown400[i].reference))
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x262640
bool function_262640(short block_index, short range_index, s_reference reference)
{
	bool result = false;
	if (!(reference.unknown2 & 0x8000) && block_index != NONE && range_index != NONE && block_index == reference.unknown2)
	{
		s_262b40_block *block = &((s_262b40_scenario_view *)g_4e0350)->blocks[(word)block_index];
		s_reference_range_view *range = &block->ranges[range_index];
		result = reference.unknown0 >= range->first && reference.unknown0 < range->first + range->count;
	}
	return result;
}
