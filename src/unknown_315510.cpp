// @flags /O2 /Ob1 /GL- /GX
#include "contact_constraint_315510.h"
#include <stddef.h>

typedef char contact_ids_offset[offsetof(contact_constraint_315510, ids) == 0x20 ? 1 : -1];
typedef char contact_points_offset[offsetof(contact_constraint_315510, points) == 0x34 ? 1 : -1];
typedef char contact_properties_offset[offsetof(contact_constraint_315510, properties) == 0x90 ? 1 : -1];

// Array members release their owned blocks in reverse declaration order.
// @retail 0x315510
contact_constraint_315510::~contact_constraint_315510() {}
// @retail 0x3155d0
contact_sizes_3155f0::contact_sizes_3155f0() {}
// @retail 0x315700
contact_sizes_315720::contact_sizes_315720() {}
// @retail 0x3156c0
contact_runtime_3156c0::~contact_runtime_3156c0() {}
// @retail 0x3156d0 deleting contact_sizes_3155f0
// @retail 0x315610 adjustor contact_sizes_3155f0 8
