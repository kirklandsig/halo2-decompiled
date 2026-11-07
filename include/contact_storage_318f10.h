#ifndef CONTACT_STORAGE_318F10_H
#define CONTACT_STORAGE_318F10_H
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
#include "lane_a_contact_manager.h"

// Accessed storage prefix only; this does not reconstruct the virtual interface.
struct s_contact_point_318f10 { __m128 position, normal; };
struct s_contact_property_318f30 { unsigned char bytes[0x1c]; };
struct s_contact_ids_318f50
{
	unsigned char *data;
	long size, capacity_and_flags;
	void collect(array_2d91d0 *out) const;
};
struct s_contact_storage_318f10
{
	unsigned char unknown0[8];
	listener_world_318320 *world;
	long remaining;
	unsigned char unknown10[0x20];
	s_contact_ids_318f50 ids;
	unsigned long unknown3c[2];
	s_contact_point_318f10 *points;
	unsigned char unknown48[0x58];
	s_contact_property_318f30 *properties;
	s_contact_point_318f10 *point(unsigned short id);
	s_contact_property_318f30 *property(unsigned short id);
	void collect(array_2d91d0 *out) const;
};
#endif
