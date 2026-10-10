// @flags /O1 /Gr
/* UNKNOWN_2369B3.CPP: searches of the user interface globals tag's blocks by
   identifier */

#include "unknown_11c920.h"
#include "globals.h"

struct s_tag_reference_entry
{
	long group;
	long index;
};

struct s_reference_list
{
	long id;
	byte unknown04[4];
	long count;
	s_tag_reference_entry *references;
	byte unknown10[8];
};

struct s_interface_tag
{
	byte unknown00[0x18];
	long count;
	s_reference_list *lists;
};

struct s_interface_globals
{
	byte unknown00[0x14];
	long tag_index;
};

struct s_indexed_entry
{
	byte unknown00[4];
	long id;
	byte unknown08[4];
};

struct s_indexed_block
{
	byte unknown00[0x1c];
	long count;
	s_indexed_entry *entries;
};

void *function_1482e8(void);

// @retail 0x236a7f
s_interface_tag *function_236a7f(void)
{
	s_interface_tag *result = 0;
	s_interface_globals *globals = (s_interface_globals *)function_1482e8();

	if (globals && globals->tag_index != NONE)
		result = (s_interface_tag *)g_4e3b44[globals->tag_index & 0xffff].bytes;
	return result;
}

// @retail 0x2369b3
long *function_2369b3(long id)
{
	long *result = 0;
	s_interface_tag *tag = function_236a7f();

	if (tag)
	{
		for (long i = 0; i < tag->count && !result; i++)
		{
			s_reference_list *list = &tag->lists[i];
			for (long j = 0; j < list->count && !result; j++)
			{
				s_tag_reference_entry *reference = &list->references[j];

				if (reference->index != NONE)
				{
					long *referenced_tag_bytes = (long *)g_4e3b44[reference->index & 0xffff].bytes;
					if (*referenced_tag_bytes == id)
						result = referenced_tag_bytes;
				}
			}
		}
	}
	return result;
}

// @retail 0x236a3a
s_reference_list *function_236a3a(long id)
{
	s_reference_list *result = 0;
	s_interface_tag *tag = function_236a7f();

	if (tag)
	{
		for (long i = 0; i < tag->count && !result; i++)
		{
			s_reference_list *list = &tag->lists[i];

			if (list->id == id)
				result = list;
		}
	}
	return result;
}

// @retail 0x236aa9
s_indexed_entry *function_236aa9(s_indexed_block *block, long id)
{
	s_indexed_entry *result = 0;

	for (long i = 0; i < block->count && !result; i++)
	{
		s_indexed_entry *entry = &block->entries[i];

		if (entry->id == id)
			result = entry;
	}
	return result;
}

// @retail 0x2365e0
long function_2365e0(long type)
{
	long result = NONE;
	switch (type)
	{
	case 0:
		result = 0x70002ff;
		break;
	case 1:
		result = 0x8000300;
		break;
	}
	return result;
}

bool function_13cb40(void);
bool function_138800();

// @retail 0x2365f7
bool function_2365f7(void)
{
	bool result = true;

	if (function_13cb40())
	{
		result = false;
	}
	if (function_138800() && g_4e6948->state == 3)
	{
		result = true;
	}
	return result;
}
