// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0CAFC0.CPP: the point a unit sees from (its eyes, its seat's
   camera marker, or its head marker) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_0259a0.h"

/* the unit as this file reads it */
struct s_unit_eye_object
{
	long definition_index;
	byte unknown04[0x14 - 0x4];
	long parent_index;
	byte unknown18[0xa0 - 0x18];
	real scale;
	byte unknowna4[0xaa - 0xa4];
	byte type;
	byte unknownab[0x10a - 0xab];
	byte flags10a_0 : 1;
	byte flags10a_1 : 1;
	byte flags10a_2 : 1;
	byte : 5;
	byte unknown10b[0x168 - 0x10b];
	vector3f forward;
	byte unknown174[0x1fc - 0x174];
	short seat_index;
	byte unknown1fe[0x24c - 0x1fe];
	long unknown24c;
	byte unknown250[0x2c4 - 0x250];
	real crouch;
	byte unknown2c8[0x348 - 0x2c8];
	word : 13;
	word flags348_13 : 1;
	word : 2;
	byte unknown34a[0x3dc - 0x34a];
	byte unknown3dc;
	byte unknown3dd[0x3f8 - 0x3dd];
	bool unknown3f8;
};

struct s_unit_eye_object_header
{
	byte unknown00[8];
	s_unit_eye_object *object;
};

/* a seat of a unit's tag (0xb0 bytes) */
struct s_unit_eye_seat
{
	byte unknown00[8];
	long marker_name;
	byte unknown0c[0x60 - 0xc];
	long camera_marker_name;
	byte unknown64[0xb0 - 0x64];
};

/* a unit's tag */
struct s_unit_eye_definition
{
	byte unknown00[0x1c8];
	long seat_count;
	s_unit_eye_seat *seats;
	byte unknown1d0[0x218 - 0x1d0];
	real standing_height;
	real crouching_height;
};

/* an entry of g_51e9b8 (0xa0 bytes) */
struct s_unit_eye_attachment
{
	byte unknown00[0x18];
	char index;
	byte unknown19[0x70 - 0x19];
	struct s_unit_eye_attachment_element
	{
		byte unknown00[0x40];
		struct
		{
			byte unknown00[0x3c];
			struct
			{
				byte unknown00[0x70];
				point3f position;
			} *data;
		} *definition;
		byte unknown44[0x60 - 0x44];
	} *elements;
	long element_count;
	byte unknown78[0xa0 - 0x78];
};

#define EYE_OBJECT(index) (((s_unit_eye_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define EYE_DEFINITION(index) ((s_unit_eye_definition *)g_4e3b44[(index) & 0xffff].bytes)

point3f *function_b9dd0(long object_index, point3f *result);
vector3f *function_11d090(vector3f const *v, vector3f *out);
real function_30bf0(vector3f *v);
void function_df380(long unit_index, point3f *origin, vector3f *forward, vector3f *up);

// @retail 0xe0dc0
bool function_e0dc0(long unit_index)
{
	s_unit_eye_object *unit = EYE_OBJECT(unit_index);
	bool result = false;

	if (unit->unknown3dc == 6 && unit->unknown3f8 && TEST_FIELD_BIT(unit->flags348_13))
	{
		result = true;
	}
	return result;
}

// @retail 0xb9ef0
point3f *function_b9ef0(long object_index, point3f *result)
{
	long root_index = NONE;
	long index = object_index;
	long attachment_index;

	while (index != NONE)
	{
		root_index = index;
		index = EYE_OBJECT(index)->parent_index;
	}
	attachment_index = *(long *)((byte *)EYE_OBJECT(root_index) + 0xb4);
	if (attachment_index != NONE)
	{
		s_unit_eye_attachment *attachment = &((s_unit_eye_attachment *)g_51e9b8->data)[attachment_index & 0xffff];
		short element_index = (attachment->index >= 0 && attachment->index < attachment->element_count) ? attachment->index : NONE;

		if (element_index != NONE)
		{
			*result = attachment->elements[element_index].definition->data->position;
			return result;
		}
	}
	function_b9dd0(object_index, result);
	return result;
}

// @retail 0xcafc0
void function_cafc0(long unit_index, point3f *position)
{
	s_unit_eye_object *unit = EYE_OBJECT(unit_index);
	s_unit_eye_definition *definition = EYE_DEFINITION(unit->definition_index);

	if (unit->parent_index == NONE && !TEST_FIELD_BIT(unit->flags10a_2) && !unit->type)
	{
		s_unit_eye_definition *local_98b918 = EYE_DEFINITION(unit->definition_index);
		real crouch;
		point3f origin;
		vector3f forward;
		vector3f up;
		vector3f offset;
		point3f point;
		s_collision_result_1697c0 collision;

		function_b9dd0(unit_index, position);
		crouch = function_e0dc0(unit_index) ? 0.0f : unit->crouch;
		position->z += ((1.0f - crouch) * local_98b918->standing_height + local_98b918->crouching_height * crouch) * unit->scale;
		origin = *position;
		forward = unit->forward;
		collision.unknown24 = NONE;
		function_11d090(&forward, &up);
		function_df380(unit_index, &origin, &forward, &up);
		offset.i = origin.x - position->x;
		offset.j = origin.y - position->y;
		offset.k = origin.z - position->z;
		if (function_30bf0(&offset) > g_45dbd8)
		{
			forward.i = offset.i * -0.25f;
			forward.j = offset.j * -0.25f;
			forward.k = offset.k * -0.25f;
			if (function_1697c0(0x4808c2d, position, &forward, NONE, NONE, &collision))
			{
				real t = *(real *)&collision.unknown00[4] * 0.9f;

				point.x = forward.i * t + position->x;
				point.y = forward.j * t + position->y;
				point.z = forward.k * t + position->z;
			}
			else
			{
				point = collision.point;
			}
			if (function_1697c0(0x4808c2d, &point, &offset, NONE, NONE, &collision))
			{
				vector3f to_origin;
				vector3f to_collision;

				vector3d_from_points3d(&point, &origin, &to_origin);
				vector3d_from_points3d(&point, &collision.point, &to_collision);
				if (dot3f(&to_origin, &offset) + 0.05f > dot3f(&to_collision, &offset))
				{
					position->x = collision.point.x - offset.i * 0.05f;
					position->y = collision.point.y - offset.j * 0.05f;
					position->z = collision.point.z - offset.k * 0.05f;
					return;
				}
			}
			*position = origin;
		}
	}
	else if (unit->parent_index == NONE)
	{
		long marker_name = 0x4000095;
		s_object_marker marker;

		if (unit->unknown24c != NONE)
		{
			s_unit_eye_object *object = EYE_OBJECT(unit->unknown24c);

			if (object->seat_index != NONE)
			{
				marker_name = definition->seats[object->seat_index].marker_name;
			}
		}
		function_b8d30(unit_index, marker_name, &marker, 1, false);
		*position = marker.matrix.position;
	}
	else
	{
		function_b9ef0(unit->parent_index, position);
		if (unit->seat_index != NONE)
		{
			long parent_index = unit->parent_index;
			s_unit_eye_object *parent = EYE_OBJECT(parent_index);
			s_unit_eye_seat *seat = &EYE_DEFINITION(parent->definition_index)->seats[unit->seat_index];

			if (parent->type != 1 || seat->camera_marker_name)
			{
				s_object_marker marker;

				function_b8d30(parent_index, seat->camera_marker_name, &marker, 1, false);
				*position = marker.matrix.position;
			}
		}
	}
}
