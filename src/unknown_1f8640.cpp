// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"
#include "unknown_2626b0.h"
#include "unknown_0259a0.h"
#include "unknown_1f9240.h"
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

/* inline: link-time code generation expands it at 0x1bf0f0's state test and keeps the
   out-of-line copy for the other callers */
// @retail 0x1f86f0
inline bool function_1f86f0(long index)
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
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1f8940
real function_1f8940(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real length = 0.0f;

	if (actor->unknown50c && actor->unknown504 == 1)
	{
		real total = 0.0f;
		point3f previous = actor_moving_get(actor_index)->position;
		short i = actor->path_index;

		if (i < actor->path_count)
		{
			do
			{
				s_actor_path_point *point = &actor->path[i];
				point3f position;

				if (point->node.output_index == NONE ||
					!function_2104b0(point->node.output_index, &point->node.point, &position))
				{
					position = point->node.point;
				}
				vector3f offset;
				offset.i = previous.x - position.x;
				offset.j = previous.y - position.y;
				offset.k = previous.z - position.z;
				real squared = offset.k * offset.k + offset.i * offset.i;
				squared += offset.j * offset.j;
				total += (real)sqrt(squared);
				previous = position;
				i++;
			}
			while (i < actor->path_count);
			length = total;
		}
	}

	return length;
}
#pragma function(_ReadWriteBarrier)
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
void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);

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

		function_11d180(forward, g_4687b0, &normalized_forward, &left, &up);
		out->i = dot3f(forward, vector);
		out->j = dot3f(vector, &left);
		out->k = dot3f(vector, &up);
		function_30bf0(out);
	}
	else
	{
		vector3f left;

		left.i = 0.0f - forward->j;
		left.j = forward->i;
		out->i = vector->i * forward->i + forward->j * vector->j;
		out->j = vector->i * left.i + left.j * vector->j;
		out->k = 0.0f;
		function_30bf0(out);
	}
}

struct s_1f8a70_frame
{
	short mode;
	byte unknown02[2];
	real unknown04;
	real unknown08;
	union
	{
		s_type_c3b527 point;
		s_reference reference;
		long index;
	} target;
	long source_node;
	real unknown20;
	real radius;
	byte unknown28[0x38 - 0x28];
	long root_index;
	byte unknown3c[4];
	s_type_c3b527 destination;
	long destination_node;
	real destination_radius;
};

struct s_actor_move_request;
struct s_path_result_view;
struct s_29e730;
struct s_29de21;
struct s_29e0f9;
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);
long function_1e4a50(long character_index);
void __stdcall function_26c2d0(long prop_index);
bool function_1f9490(long actor_index, s_reference reference, s_actor_move_request *request);
bool function_1f9580(long actor_index, s_reference reference);
void function_1caa40(long object_index, point3f *position);
long function_baf80(long object_index);
bool __stdcall function_29e730(s_29e730 *request, long actor_index, point3f const *position,
    s_type_c3b527 const *target, s_reference reference, s_29de21 *context,
    long root_index, s_29e0f9 *result);
void function_2713c0(s_type_f17a25 *state, s_type_c3b527 const *point, long node_index, real radius);
bool function_270930(long actor_index, s_type_f17a25 *state, s_path_result_view *result);
void function_2705c0(s_path_source *source, point3f const *point, real radius,
    long object_index, real weight, bool unknown);

struct s_1f8a70_actor
{
	byte unknown00[0x4ac];
	s_1f8a70_frame frame;
};
/* sets the frame's destination from its target (modes 2 to 7): false when the target is gone or the mode is
   unknown; *direct is set when the mode asks for the direct route */
static __forceinline bool goal_from_target_kind(long actor_index, s_actor_view *actor, short *route_mode, bool *direct)
{
	switch (((s_1f8a70_actor *)actor)->frame.mode)
	{
	case 2:
		((s_1f8a70_actor *)actor)->frame.destination = ((s_1f8a70_actor *)actor)->frame.target.point;
		((s_1f8a70_actor *)actor)->frame.destination_node = ((s_1f8a70_actor *)actor)->frame.source_node;
		((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
		break;
	case 3:
		((s_1f8a70_actor *)actor)->frame.destination = ((s_1f8a70_actor *)actor)->frame.target.point;
		((s_1f8a70_actor *)actor)->frame.destination_node = NONE;
		((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
		*direct = true;
		break;
	case 4:
	{
		s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
		if (!target)
			return false;
		((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
		((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)target + 0x14);
		((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
		break;
	}
	case 5:
	{
		s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
		if (!target)
			return false;
		((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
		((s_1f8a70_actor *)actor)->frame.destination_node = NONE;
		((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
		*direct = true;
		break;
	}
	case 6:
	{
		s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
		if (!target)
			return false;
		byte *movement = (byte *)function_1e4a50(actor->unknown054);
		if (!movement)
			return false;
		if (*movement & 0x10)
		{
			((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
			((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)target + 0x14);
			((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
			if (*movement & 0x20)
				*direct = true;
			else if (*route_mode == 0 && function_1f9580(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference))
				*route_mode = 2;
		}
		break;
	}
	case 7:
	{
		long prop_index = ((s_1f8a70_actor *)actor)->frame.target.index;
		s_prop_node_view *node = prop_node_get(prop_index);
		function_26c2d0(prop_index);
		s_prop_state_view *state = prop_node_state(node);
		if (*route_mode == 1)
		{
			((s_1f8a70_actor *)actor)->frame.destination.point = *(point3f *)((byte *)state + 0x10);
			((s_1f8a70_actor *)actor)->frame.destination.output_index = NONE;
		}
		else
			((s_1f8a70_actor *)actor)->frame.destination = state->unknown48;
		((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)state + 0x44);
		((s_1f8a70_actor *)actor)->frame.destination_radius = ((s_1f8a70_actor *)actor)->frame.radius;
		break;
	}
	default:
		return false;
	}
	return true;
}

/* plans an actor's movement: stops an actor that may not move, leaves a path it is already on, and otherwise
   sets the frame's destination, finds a route to it in the chosen mode and reports whether one was found */
// @retail 0x1f8a70
bool __stdcall function_1f8a70(long actor_index, long unknown)
{
	bool result;
	if (actor_get(actor_index)->unknown267)
	{
		result = false;
		function_1f86a0(actor_index);
	}
	else
	{
		s_actor_view *actor = actor_get(actor_index);
		if (*((byte *)actor + 0x480))
			result = true;
		else if (actor->unknown504 == 2 && function_1f3230(actor_index, function_1e3920(actor_index) * 3.0f))
		{
			function_1f8780(actor_index, false);
			result = true;
		}
		else if (actor->unknown4ac == 0 || actor->unknown4ac == 1)
		{
			function_1f86a0(actor_index);
			result = true;
		}
		else
		{
			bool route_found;
			short route_mode = actor->unknown229 != 0;
			{
				if (function_e70b0(actor->unknown018).value == 5)
				{
					byte *properties = (byte *)function_1e4990(actor->unknown054);
					byte *movement = (byte *)function_1e4a50(actor->unknown054);
					if (properties && !(*properties & 2) && movement && (*movement & 0x20))
						route_mode = 1;
				}
			}
			{
				s_actor_view *reset = actor_get(actor_index);
				reset->unknown50c = false;
				reset->unknown5ac = NONE;
				reset->unknown5b0 = NONE;
			}
			actor->unknown504 = 0;
			*((bool *)actor + 0x5d2) = false;
			s_type_f17a25 *path = (s_type_f17a25 *)unknown;
			bool direct = false;
			if (!goal_from_target_kind(actor_index, actor, &route_mode, &direct))
			{
				route_found = false;
			}
			else
			{
				if (direct)
					route_mode = 1;
				long route = route_mode;
				bool valid;
				if (route != 0)
					valid = route > 0 && route <= 2;
				else if (((s_1f8a70_actor *)actor)->frame.destination_radius != 0.0f)
					valid = true;
				else
					valid = ((s_1f8a70_actor *)actor)->frame.destination_node != NONE;
				if (!valid)
				{
					route_found = false;
				}
				else
				{
					switch (route)
					{
					case 2:
						route_found = function_1f9490(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, (s_actor_move_request *)((byte *)actor + 0x50c));
						break;
					case 1:
					{
						s_reference reference = g_470fa0;
						long root_index = NONE;
						if (((s_1f8a70_actor *)actor)->frame.mode == 4 || ((s_1f8a70_actor *)actor)->frame.mode == 5 || ((s_1f8a70_actor *)actor)->frame.mode == 6)
							reference = ((s_1f8a70_actor *)actor)->frame.target.reference;
						else if (((s_1f8a70_actor *)actor)->frame.mode == 7)
							root_index = function_baf80(prop_node_get(((s_1f8a70_actor *)actor)->frame.target.index)->object_index);
						if (((s_1f8a70_actor *)actor)->frame.root_index != NONE)
							root_index = ((s_1f8a70_actor *)actor)->frame.root_index;
						point3f position;
						if (actor->unknown26c != NONE)
						function_1caa40(actor->unknown26c, &position);
					else
						function_1caa40(actor->unknown018, &position);
						s_29de21 *context = path ? (s_29de21 *)((byte *)path + 0x140d4) : NULL;
						route_found = function_29e730((s_29e730 *)&((s_1f8a70_actor *)actor)->frame, actor_index, &position, &((s_1f8a70_actor *)actor)->frame.destination,
							reference, context, root_index, (s_29e0f9 *)((byte *)actor + 0x50c));
						break;
					}
					case 0:
						if (path)
						{
							function_2713c0(path, &((s_1f8a70_actor *)actor)->frame.destination, ((s_1f8a70_actor *)actor)->frame.destination_node, ((s_1f8a70_actor *)actor)->frame.destination_radius);
							route_found = function_270930(actor_index, path, (s_path_result_view *)((byte *)actor + 0x50c));
						}
						else
						{
							s_path_source source;
							s_path_settings settings;
							route_found = false;
							function_1f90f0(actor_index, &source);
							if (((s_1f8a70_actor *)actor)->frame.root_index != NONE)
								source.unknown0c = ((s_1f8a70_actor *)actor)->frame.root_index;
							if (actor->unknown358 > 0 && !actor->unknown35e)
								function_2705c0(&source, &actor->unknown370, actor->unknown36c, actor->unknown360, 10.0f, true);
							function_1f9240(actor_index, &settings);
							byte *buffer = ai_scratch_buffer_get();
							s_type_f17a25 *state = (s_type_f17a25 *)buffer;
							function_271300(state, (s_path_location *)((byte *)actor + 0x5b4), &settings, &source, 0);
							function_2713c0(state, &((s_1f8a70_actor *)actor)->frame.destination, ((s_1f8a70_actor *)actor)->frame.destination_node, ((s_1f8a70_actor *)actor)->frame.destination_radius);
							if (function_2715a0(buffer))
							{
								route_found = function_270930(actor_index, state, (s_path_result_view *)((byte *)actor + 0x50c));
								if (!route_found && actor->unknown5b2[0] && ((s_1f8a70_actor *)actor)->frame.mode == 4)
									function_262800(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, false);
							}
							else
								*(s_path_location *)((byte *)actor + 0x5b4) = *(s_path_location *)(buffer + 0x70);
							ai_scratch_buffer_release(buffer);
						}
						break;
					default:
						route_found = false;
						break;
					}
					actor->unknown506 = true;
				}
			}
			if (route_found)
			{
				actor->unknown504 = 1;
				actor->unknown5e8 = 0;
			}
			else
				function_1f8750(actor_index);
			result = route_found;
		}
	}
	return result;
}

