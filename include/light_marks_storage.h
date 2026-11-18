#ifndef LIGHT_MARKS_STORAGE_H
#define LIGHT_MARKS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* the light marks of the visibility pass: a flag set while the light walk
   (0x132220) runs and the current stamp it gives each light it reaches.
   Retail keeps them in one block, so LTCG sees the flag's stores as reaching
   the walk's stamp reads */
struct s_light_marks_storage
{
	bool field_0;
	byte unknown_1[0x3];
	long field_4;
};

extern s_light_marks_storage g_light_marks_storage;
#define g_4e0304 (g_light_marks_storage.field_0)
#define g_4e0308 (g_light_marks_storage.field_4)
typedef char assert_light_marks_offset_0[(offsetof(s_light_marks_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_light_marks_offset_4[(offsetof(s_light_marks_storage, field_4) == 0x4) ? 1 : -1];

typedef char assert_light_marks_size[(sizeof(s_light_marks_storage) == 0x8) ? 1 : -1];

#endif
