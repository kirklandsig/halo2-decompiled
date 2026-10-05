#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

struct s_output_entry;
extern s_output_entry *g_4f93a0;

struct s_output_object_view
{
	byte unknown00[0xaa];
	byte type;
};

struct s_output_object_header
{
	byte unknown00[8];
	s_output_object_view *object;
};

struct s_output_clear_view
{
	long object_index;
	byte unknown04[0xc];
};

// @retail 0x2103d0
void function_2103d0(long object_index)
{
	s_output_object_view *object = ((s_output_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	if (object->type == 7)
	{
		for (long i = 0; i < 100; i++)
		{
			if (((s_output_clear_view *)g_4f93a0)[i].object_index == object_index)
				((s_output_clear_view *)g_4f93a0)[i].object_index = NONE;
		}
	}
}
