// @flags /O2 /Gr
/* UNITS.CPP: the seats of units */

#include "unknown_11c920.h"
#include "globals.h"
#include "units.h"

/* where a block of an object's data lives, relative to the object */
struct s_object_header_block_reference
{
	short size;
	short offset;
};

struct s_unit_object
{
	long definition_index;
	dword unknown04 : 26;
	dword flag26 : 1;
	dword unknown04b : 5;
	byte unknown08[4];
	long next_sibling;
	long first_child;
	long parent_index;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x10a - 0xab];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word unknown10a_3 : 13;
	byte unknown10c[0x128 - 0x10c];
	s_object_header_block_reference field_x69a903;
	long unknown12c;
	long unknown130;
	dword flags;
	byte unknown138[4];
	long unknown13c;
	byte unknown140[0x1fc - 0x140];
	short parent_seat_index;
	byte unknown1fe[0x218 - 0x1fe];
	long weapon_object_indices[4];
};

#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

struct s_unit_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_unit_object *object;
};

struct s_unit_definition
{
	byte unknown000[0x1c8];
	long seat_count;
	s_unit_seat_definition *seats;
};

#define UNIT_OBJECT_HEADER(index) (&((s_unit_object_header *)g_4e0300->data)[(index) & 0xffff])
#define UNIT_OBJECT(index) (UNIT_OBJECT_HEADER(index)->object)
#define UNIT_DEFINITION(object) ((s_unit_definition *)g_4e3b44[(object)->definition_index & 0xffff].bytes)

bool function_1cb920(void *data, long label);

// @retail 0xc8200
bool function_c8200(long unit_index, long object_index, short seat_index)
{
	s_unit_definition *definition = UNIT_DEFINITION(UNIT_OBJECT(object_index));
	bool result = false;

	if (seat_index >= 0 && seat_index < definition->seat_count)
	{
		s_unit_object *unit = UNIT_OBJECT(unit_index);

		if (unit->type == 1)
		{
			result = true;
		}
		else
		{
			long label = definition->seats[seat_index].label;
			byte *data = (byte *)unit + unit->field_x69a903.offset;

			if (function_1cb920(data, label))
				result = true;
		}
	}

	return result;
}

// @retail 0xc8a40
void __stdcall function_c8a40(long object_index, s_object_seat *seats, short *count, short maximum_count)
{
	s_unit_object_header *header = UNIT_OBJECT_HEADER(object_index);
	s_unit_object *object = header->object;

	if (*count < maximum_count)
	{
		if (((1 << header->type) & 3) && !TEST_FIELD_BIT(object->flag10a_2))
		{
			s_unit_definition *definition = UNIT_DEFINITION(object);
			long seat_index;

			for (seat_index = 0; seat_index < definition->seat_count; seat_index++)
			{
				if (*count >= maximum_count)
					return;

				seats[*count].seat_index = (short)seat_index;
				seats[*count].object_index = object_index;
				seats[*count].definition = &definition->seats[seat_index];
				(*count)++;
			}
		}

		if (*count < maximum_count)
		{
			long child_index;

			for (child_index = object->first_child; child_index != NONE; )
			{
				s_unit_object *child = UNIT_OBJECT(child_index);

				if (((1 << child->type) & 3) && TEST_FIELD_BIT(child->flag26))
					function_c8a40(child_index, seats, count, maximum_count);

				child_index = child->next_sibling;
			}
		}
	}
}

// @retail 0xc8f60
long function_c8f60(long unit_index, short seat_index)
{
	long child_index = UNIT_OBJECT(unit_index)->first_child;

	while (child_index != NONE)
	{
		s_unit_object *child = UNIT_OBJECT(child_index);

		if (((1 << child->type) & 3) && child->parent_seat_index == seat_index)
			break;

		child_index = child->next_sibling;
	}

	return child_index;
}

void function_10ccc0(long item_index);
void __stdcall function_cc810(long vehicle_index);

/* whether the unit's object flag 0 is set */
// @retail 0xcbf40
bool function_cbf40(long unit_index)
{
	return (UNIT_OBJECT(unit_index)->flags & FLAG(0)) ? true : false;
}

inline void unit_flags_set(dword *flags, long bit, bool value)
{
	if (value)
		*flags |= FLAG(bit);
	else
		*flags &= ~FLAG(bit);
}

/* the standard marker (docs/DECOMPILING.md): (1) with it this body matches
   byte for byte; without it LTCG passes the unit in ebx and the bool in a
   register. (2) retail holds no reference to 0xcbf60's address, and all its
   callers (0xa7b30, 0xa7bc0, 0xa9500, 0x14cad0 twice, 0x1e0d50, 0x1e1250,
   0x1e1a00, 0x1e31b0, 0x1e4390, 0x1fb360, 0x1fb510) are LTCG code that
   pushes both arguments. (3) tried: modifying the parameter, the SET_FLAG
   macro, a conditional-assignment helper and a returning helper; all keep
   the register convention. */
/* sets the unit's object flags 0 and 1: always while anything at 0x12c,
   0x130 or 0x13c is set, never while flag10a_2 is set; then updates the
   unit's weapons and the unit (0xcc810) */
// @retail 0xcbf60 standard
void __stdcall function_cbf60(long unit_index, bool active)
{
	s_unit_object *unit = UNIT_OBJECT(unit_index);
	long weapon_index;

	if (unit->unknown12c != NONE || unit->unknown130 != NONE || unit->unknown13c != NONE)
		active = true;
	if (TEST_FIELD_BIT(unit->flag10a_2))
		active = false;

	unit_flags_set(&unit->flags, 1, active);
	unit_flags_set(&unit->flags, 0, active);

	for (weapon_index = 0; weapon_index < 4; weapon_index++)
	{
		if (unit->weapon_object_indices[weapon_index] != NONE)
			function_10ccc0(unit->weapon_object_indices[weapon_index]);
	}

	function_cc810(unit_index);
}
