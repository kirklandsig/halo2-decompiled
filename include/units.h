#ifndef __UNITS_H__
#define __UNITS_H__

/* the flags of a seat of a unit's tag */
struct s_seat_definition_flags
{
	dword unknown0 : 2;
	dword bit2 : 1;
	dword bit3 : 1;
	dword unknown4 : 7;
	dword bit11 : 1;
	dword unknown12 : 20;
};

/* a seat of a unit's tag (0xb0 bytes) */
struct s_unit_seat_definition
{
	s_seat_definition_flags flags;
	long label;
	byte unknown08[0xb0 - 0x8];
};

/* the seats function_c8a40 lists: an object, one of its seats and the
   seat's definition */
struct s_object_seat
{
	long object_index;
	short seat_index;
	byte unknown6[2];
	s_unit_seat_definition *definition;
};

/* lists the seats of an object and of the units riding it */
void __stdcall function_c8a40(long object_index, s_object_seat *seats, short *count, short maximum_count);
/* whether the unit may take the seat of the object */
bool function_c8200(long unit_index, long object_index, short seat_index);
long function_c8f60(long unit_index, short seat_index);

#endif
