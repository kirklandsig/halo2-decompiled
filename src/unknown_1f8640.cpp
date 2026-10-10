// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"
#include "unknown_2626b0.h"
#include <math.h>
#include <string.h>

#define OWNER_STATE(index) ((s_slot_owner_entry *)(g_4f55f0->data + ((index) & 0xffff) * sizeof(s_slot_owner_entry)))

// @retail 0x1f8640
byte function_1f8640(long index)
{
	return OWNER_STATE(index)->unknown50c;
}

// @retail 0x1f8660
bool function_1f8660(long index)
{
	bool result = false;
	s_slot_owner_entry *s = OWNER_STATE(index);
	byte flag = s->unknown50c;
	if (flag && s->unknown504 == 1)
		result = true;
	return result;
}

// @retail 0x1f86a0
void function_1f86a0(long index)
{
	s_slot_owner_entry *s = OWNER_STATE(index);
	s->unknown50c = 0;
	s->unknown5ac = NONE;
	s->unknown5b0 = NONE;
	s->unknown5b4 = 0;
	s->unknown5b6 = 0;
	s->unknown4ac = 0;
	s->unknown504 = 0;
}

// @retail 0x1f86f0
bool function_1f86f0(long index)
{
	return OWNER_STATE(index)->unknown504 == 2;
}

// @retail 0x1f8720
bool function_1f8720(long index)
{
	return OWNER_STATE(index)->unknown504 == 3;
}

// @retail 0x1f8750
void function_1f8750(long index)
{
	s_slot_owner_entry *s = OWNER_STATE(index);
	s->unknown508++;
	s->unknown50c = 0;
	s->unknown504 = 3;
}

/* a request the movement code passes to the actor's unit (function_e6900) */
struct s_moving_unit_request
{
	long type;
	union
	{
		struct
		{
			point3f point;
			vector3f facing;
		} face;
		struct
		{
			short unknown4;
			byte unknown6[2];
			point3f point;
			vector3f vector;
		} turn;
	};
};

bool function_262590(long actor_index, s_reference reference, bool unknown);
long function_1e4990(long index);

/* the actor has arrived */
// @retail 0x1f8780
void function_1f8780(long actor_index, bool unknown)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	long type = 0;

	actor->unknown50c = true;
	actor->unknown504 = 2;
	if (!actor->unknown227)
	{
		short goal = actor->unknown4ac;

		if ((goal == 4 || goal == 6 || goal == 5) &&
			function_262590(actor_index, actor->unknown4b8_reference, false))
		{
			real radius = function_1e3920(actor_index);

			s_type_c3b527 *local_1 = (s_type_c3b527 *)function_262b40(actor->unknown4b8_reference);
			if (function_210ac0(local_1, &actor->position) <= radius + 0.2)
				actor->unknown227 = true;
		}
		else if (actor->unknown4e8)
		{
			actor->unknown227 = true;
		}
	}

	if (unknown)
	{
		s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[actor->unit_index & 0xffff];

		if (header->type == 0)
			type = header->object[0x3dc];
		switch (type)
		{
		case 2:
		{
			s_moving_unit_request request;

			if (actor->unknown4ac == 6)
			{
				request.type = 0x2d;
				request.turn.unknown4 = 0;
				if (!function_262a90(actor->unknown4b8_reference, &request.turn.point, &request.turn.vector))
					goto local_0;
			}
			else if (actor->unknown4ac == 4)
			{
				long character = function_1e4990(actor->tag_index);

				if (!character || (*(byte *)character & 2) ||
					!(function_262b40(actor->unknown4b8_reference)->flags & 0x40))
				{
					break;
				}
				request.type = 0x2f;
				if (!function_262af0(actor->unknown4b8_reference, &request.face.point, &request.face.facing))
					goto local_0;
				request.face.facing = actor->unknown290;
			}
			else
			{
				break;
			}

			if (function_e6900(actor->unit_index, (s_unit_request *)&request))
			{
				actor->unknown227 = true;
				break;
			}
local_0:
			{
				actor->unknown50c = false;
				actor->unknown504 = 3;
			}
			break;
		}
		}
	}
}
/* the length of the rest of the actor's path */
// @retail 0x1f8940
real function_1f8940(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real length = 0.0f;

	if (actor->unknown50c && actor->unknown504 == 1)
	{
		point3f previous = actor->position;
		short i;

		for (i = actor->path_index; i < actor->path_count; i++)
		{
			s_actor_path_point *point = &actor->path[i];
			point3f position;

			if (point->node.output_index == NONE ||
				!function_2104b0(point->node.output_index, &point->node.point, &position))
			{
				position = point->node.point;
			}
			length += distance3d(&position, &previous);
			previous = position;
		}
	}

	return length;
}
/* the parts of the actor's character block and of its vehicle's entry that
   set the radius of the actor's path source */
struct s_path_radius_block
{
	byte unknown00[4];
	real radius;
};

struct s_path_radius_element
{
	byte unknown00[0x14];
	real radius;
};

long function_1e4a50(long index);

static inline void path_source_initialize(s_path_source *source, real radius, byte unknown04, long object_index, real unknown4c)
{
	memset(source, 0, sizeof(*source));
	source->radius = radius;
	source->unknown04 = unknown04;
	source->object_index = object_index;
	source->unknown0c = NONE;
	source->unknown4c = unknown4c;
}

static inline void path_source_set_point(s_path_source *source, s_path_point const *point, long unknown24)
{
	source->has_point = true;
	source->point = *point;
	source->unknown24 = unknown24;
}

/* where the actor's paths start: its position, or its vehicle's */
// @retail 0x1f90f0
void function_1f90f0(long actor_index, s_path_source *source)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real radius = ((s_path_radius_block *)function_1e4a50(actor->tag_index))->radius;
	long object_index = actor->unit_index;
	real unknown4c;

	if (actor->unknown266)
	{
		s_path_radius_element *element;

		element = (s_path_radius_element *)function_1e5450(actor_index, moving_object_get(actor->unknown26c)->tag_index);
		object_index = actor->unknown26c;
		if (element && element->radius > 0.0f)
			radius = element->radius;
	}

	if (actor->unknown086 >= 3 && actor->unknown328 >= 3)
		unknown4c = 1.0f;
	else
		unknown4c = 5.0f;

	path_source_initialize(source, radius, actor->unknown3e4, object_index, unknown4c);
	if (actor->unknown229)
	{
		s_path_point point;

		point.point = actor->position;
		point.cluster_index = NONE;
		path_source_set_point(source, &point, NONE);
	}
	else
	{
		function_26c180(actor_index);
		path_source_set_point(source, &actor->location, actor->unknown28c);
	}
}

real function_30bf0(vector3f *v);
void function_11d180(vector3f *left, vector3f const *in, vector3f *out, vector3f const *up, vector3f *forward);

/* the vector in the frame of the forward direction (forward, left, up), as
   a unit vector; flat frames ignore the vertical */
// @retail 0x1f8510
void __stdcall function_1f8510(bool full_frame, vector3f const *vector, vector3f const *forward, vector3f *out)
{
	if (full_frame)
	{
		vector3f left;
		vector3f up;
		vector3f normalized_forward;

		function_11d180(&left, forward, &up, g_4687b0, &normalized_forward);
		out->i = dot3f(forward, vector);
		out->j = dot3f(vector, &left);
		out->k = dot3f(vector, &up);
		function_30bf0(out);
	}
	else
	{
		real local_0 = forward->i;
		real local_1 = forward->j;
		real local_2 = 0.0f;
		real local_5 = vector->i * local_0;
		real local_6 = vector->j * local_1;
		real local_4 = local_2 - forward->j;
		out->i = local_5 + local_6;
		out->j = vector->i * local_4 + vector->j * local_0;
		out->k = 0.0f;
		function_30bf0(out);
	}
}
