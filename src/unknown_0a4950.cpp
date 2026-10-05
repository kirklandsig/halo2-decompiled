// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"

class c_game_engine_entity_definition;

/* the base of the update sizes of the game engine globals entity definitions
   (game_engine_entity_definitions.cpp) */
// @retail 0xa4950
long function_a4950(c_game_engine_entity_definition const *definition, dword *flags_pointer)
{
	dword flags = *flags_pointer;
	long result = 0x7fffffff;

	if (flags & 0x1)
		result = 0x4a;
	if ((flags & 0x2) && result > 7)
		result = 7;
	if ((flags & 0x4) && result > 6)
		result = 6;
	if ((flags & 0x8) && result > 10)
		result = 10;
	if ((flags & 0x10) && result > 21)
		result = 21;
	if (result == 0x7fffffff)
		result = 0;
	return result;
}

// @retail 0xa6900
void s_flags_a6900::function_a6900(long *result_pointer, c_object_type_definition const *definition) const
{
	long result = 0x7fffffff;

	if (flags & 0x2)
		result = 0x3a;
	if ((flags & 0x4) && result > 0x1f)
		result = 0x1f;
	if ((flags & 0x8) && result > 0x11)
		result = 0x11;
	if ((flags & 0x30) && result > 0x21)
		result = 0x21;
	if ((flags & 0x40) && result > 0x13)
		result = 0x13;
	if ((flags & 0x80) && result > 0x13)
		result = 0x13;
	if ((flags & 0x1) && result > 0xb)
		result = 0xb;
	if ((flags & 0x100) && result > 0x3e)
		result = 0x3e;
	if ((flags & 0x200) && result > 0x2f)
		result = 0x2f;
	if (result == 0x7fffffff)
		result = 0;
	*result_pointer = result;
}

/* the object header (12 bytes) and the object and tag views of d5b60 */
struct s_d5b60_tag
{
	byte unknown00[0x60];
	long count;
	long first;
};

struct s_d5b60_tag_instance
{
	byte unknown00[8];
	byte *data;
	byte unknown0c[4];
};

struct s_d5b60_object
{
	long tag_index;
};

struct s_d5b60_object_header
{
	byte unknown00[8];
	s_d5b60_object *object;
};


// @retail 0xd5b60
long function_d5b60(long object_index)
{
	long result = 0;

	if (object_index != NONE)
	{
		s_d5b60_object *object = ((s_d5b60_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
		long tag_index = *(long *)(definition + 0x38);

		if (tag_index != NONE)
		{
			s_d5b60_tag *tag = (s_d5b60_tag *)g_4e3b44[tag_index & 0xffff].bytes;
			if (tag->count > 0)
				result = tag->first;
		}
	}
	return result;
}
