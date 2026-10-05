// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"

// 0x14f2e0 passes a 0x34-byte record; only this prefix is read here.
struct s_marker_position_angle
{
	point3f position;
	real angle;
};

// @retail 0x11c7d0
void marker_record_get_position_and_angle(s_marker_position_angle const *record,
	point3f *position, real *angle)
{
	if (position)
		*position = record->position;
	if (angle)
		*angle = record->angle;
}

// @retail 0x11c800
void marker_get_position_and_angle(long index, point3f *position, real *angle)
{
	s_marker_entry const *entry = &g_4e0350->marker_entries[index];
	if (position)
		*position = entry->position;
	if (angle)
		*angle = *(real const *)entry->unknown0c;
}
