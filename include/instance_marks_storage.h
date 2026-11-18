#ifndef INSTANCE_MARKS_STORAGE_H
#define INSTANCE_MARKS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* the instance marks of the visibility pass: a flag set while the instance
   walk (0x134390) runs, the current stamp, and each instance's last stamp.
   Retail keeps them in one block, so LTCG sees the flag's stores as reaching
   the walk's reads */
struct s_instance_marks_storage
{
	bool field_0;
	byte unknown_1[0x3];
	long field_4;
	long field_8[0x400];
};

extern s_instance_marks_storage g_instance_marks_storage;
#define g_4e7c18 (g_instance_marks_storage.field_0)
#define g_4e7c1c (g_instance_marks_storage.field_4)
#define g_4e7c20 (g_instance_marks_storage.field_8)
typedef char assert_instance_marks_offset_0[(offsetof(s_instance_marks_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_instance_marks_offset_4[(offsetof(s_instance_marks_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_instance_marks_offset_8[(offsetof(s_instance_marks_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_instance_marks_size[(sizeof(s_instance_marks_storage) == 0x1008) ? 1 : -1];

#endif
