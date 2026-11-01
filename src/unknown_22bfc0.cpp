// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_22BFC0.CPP: the first or second entry of two definition blocks
   (0x22bfc0, 0x22bff0; the file continues past 0x22c000) */

#include "unknown_11c920.h"

struct s_22bfc0_entry
{
	long unknown00;
	long value;
};

struct s_22bfc0_block
{
	long count;
	s_22bfc0_entry *entries;
};

struct s_22bfc0_definition
{
	byte unknown000[0x1a0];
	s_22bfc0_block block;
};

struct s_22bff0_element
{
	byte unknown00[0x7c];
	s_22bfc0_block block;
	byte unknown84[0xb0 - 0x84];
};

struct s_22bff0_definition
{
	byte unknown000[0x1cc];
	s_22bff0_element *elements;
};

#define MIN(a, b) ((a) > (b) ? (b) : (a))

PRIVATE inline long block_entry_value_get(s_22bfc0_block const *block, char second)
{
	short index = second ? 1 : 0;

	index = (short)MIN(index, block->count - 1);
	if (index < 0)
	{
		return NONE;
	}
	return block->entries[index].value;
}

// Both take the definition first: their one caller (0x13992a) sets the
// definition up before the flag, as retail does.
// @retail 0x22bfc0
long function_22bfc0(
	s_22bfc0_definition const *definition,
	char second)
{
	return block_entry_value_get(&definition->block, second);
}

// @retail 0x22bff0
long function_22bff0(
	s_22bff0_definition const *definition,
	short element_index,
	char second)
{
	s_22bff0_element const *element = &definition->elements[element_index];
	short index = second ? 1 : 0;

	index = (short)MIN(index, element->block.count - 1);
	if (index < 0)
	{
		return NONE;
	}
	return element->block.entries[index].value;
}
