// @flags /O2 /Ob1 /Gr
/* UNKNOWN_0B9D70.CPP */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0bbf40.h"

/* the objects (a local view) */
struct s_object_0b9d70
{
	long definition_index;
	dword flags;
};

struct s_object_header_0b9d70
{
	byte unknown00[8];
	s_object_0b9d70 *object;
};

struct s_object_definition_0b9d70
{
	byte unknown00[2];
	byte flags;
};

/* sets flag 16 of an object, which stays set while its definition has flag 0 */
// @retail 0xb9d70
void function_b9d70(long object_index, bool flag)
{
	s_object_0b9d70 *object = ((s_object_header_0b9d70 *)g_4e0300->data)[object_index & 0xffff].object;
	if (flag)
		object->flags = object->flags | 0x10000;
	else if ((((s_object_definition_0b9d70 *)g_4e3b44[object->definition_index & 0xffff].bytes)->flags & 1))
		object->flags = object->flags | 0x10000;
	else
		object->flags = object->flags & ~0x10000;
}
