// @flags /O2 /arch:SSE /Gr
// BLOCKED-BY the full check (#581, batch r294): enabling 0x1f38c0 and 0x1f6090 loses matched 0x1c83c0 and
// 0x1e2f50 under whole-program optimisation; neither byte-matches yet. fecheck: both behave like retail on 6/6
// in-game snapshots (0x1f38c0 takes the actor in eax; 0x1f6090 is __stdcall, 7 pushed arguments, ret 0x1c).
// The stub for 0x1f38c0 stays.
#if 0
/* UNKNOWN_1F6090.CPP: a biped actor moving toward its current path point:
   whether the point is reached, the requests the path entry needs, the
   facing and the throttle, the path entry's own movement, a target object,
   and a pending jump */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1e3920.h"
#include "slot_handler.h"
#include "unit_requests.h"
#include "unknown_1fa590.h"
#include "unknown_26c380.h"
#include "unknown_0259a0.h"
#include "object_markers.h"
#include "path.h"
#include <math.h>
#include <string.h>

struct s_effect_owner;
struct s_type_952051;
struct s_1fb7e0_data;

/* the movement block of the actor's character (function_1e4a50) */
struct s_actor_movement_block
{
	byte flags;
	byte unknown01[3];
	real unknown04;
	real arrival_radius;
	byte unknown0c[0x1c - 0xc];
	dword marker_flags;
	real maximum_scale;
};

/* an entry of the actor's path (s_actor_view's unknown53c) */
struct s_actor_path_entry
{
	short type;
	byte unknown02[2];
	long node_index;
	short surface_index;
	byte unknown0a[2];
	s_type_c3b527 point;
};

/* a surface of the pathfinding data (0x14 bytes) */
struct s_actor_path_surface
{
	short type;
	short next;
	word unknown04;
	word first_vertex;
	word second_vertex;
	word unknown0a;
	byte flags;
	signed char types[3];
	short source_output;
	short destination_output;
};

/* the arguments of the requests built here (s_unit_request after its type) */
struct s_request_move
{
	long type;
	point3f point;
	real direction_i;
	real direction_j;
	bool unknown18;
	byte unknown19[0x20 - 0x19];
};

struct s_request_vector
{
	long type;
	point3f point;
	vector3f vector;
	byte unknown1c[0x20 - 0x1c];
};

struct s_request_jump
{
	long type;
	bool unknown04;
	byte unknown05[3];
	point2f direction;
	byte unknown10[0x20 - 0x10];
};

long function_1e4a50(long character_index);
long function_1e4990(long character_index);
void *function_1e4ef0(long actor_index);
bool __stdcall function_110ab0(long unit_index);
short *function_e70b0(long unit_index, short *value);
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra);
point3f *function_b9dd0(long object_index, point3f *result);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
long function_1fa7f0(void);
long function_1f5f30(long actor_index);
bool function_10f630(long object_index, long *first, long *second);
byte function_10fcd0(long unit_index, long mode, long weapon_class, long weapon_type);
long function_cbd50(long unit_index, short weapon_index);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
bool function_1f57f0(long actor_index, long animation, long const *target);
bool function_2967b0(long actor_index, s_type_952051 *specification,
	bool aiming, vector3f *direction, bool *has_aim_position, point3f *arg_0e6828);
real function_30bf0(vector3f *v);
real normalize2d(point2f *v);
long function_1469f0(real seconds);
void function_26c180(long actor_index);
bool function_26bf10(long object_index);
bool function_210770(short output_index, vector3f const *vector, vector3f *out);
point3f *function_210850(s_type_c3b527 const *point, point3f *out);
void function_e64e0(vector3f const *reference, vector3f const *direction, vector3f const *fallback,
	bool three_dimensional, vector3f *result, short *axis_index);
void function_1f40b0(long actor_index, real *stopping_distance, real *round_trip_distance);
bool function_1f58c0(long actor_index, vector3f *facing, short *unknown);
bool function_1f9240(long actor_index, s_path_settings *settings);
bool function_26f2d0(point3f const *start, point3f const *end, s_path_settings const *settings);
bool function_1f8160(long actor_index, signed char const *types, point3f const *target, bool force);
bool function_2702a0(long actor_index, long mode, long set, long target_mode, long target_set,
	point3f const *position, vector3f const *forward, point3f *out_position,
	long *location, vector3f *out_forward, long *node_index);
void function_1fa600(vector3f *vector, word flags);
long function_26d100(vector3f const *up, long *unknown, s_collision_result_1697c0 *collision, point3f const *point);
bool __stdcall function_1f7c70(long actor_index, short type, bool allow_jump, vector3f *direction_out,
	short *type_out, vector3f *movement_out);
bool function_26fc80(long actor_index, long object_index, real distance, void *path, point3f *point);
void function_1f9450(long actor_index, long value);
bool function_1f50e0(long actor_index);
bool function_1fb7e0(short type, long actor_index, s_1fb7e0_data const *data, long target_index, long unknown);
void function_1e4260(long actor_index);
extern point2f *g_468778;

/* turns a vector about an axis: v cos + a (a . v)(1 - cos) - (v x a) sin */
static inline void actor_path_rotate(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real parallel = (vector->i * axis->i + axis->k * vector->k + axis->j * vector->j) * (1.0f - cosine);
	vector3f cross;
	cross.i = axis->k * vector->j - axis->j * vector->k;
	cross.j = axis->i * vector->k - vector->i * axis->k;
	cross.k = vector->i * axis->j - axis->i * vector->j;
	vector3f result;
	result.i = parallel * axis->i + vector->i * cosine - cross.i * sine;
	result.j = cosine * vector->j + parallel * axis->j - cross.j * sine;
	result.k = parallel * axis->k + cosine * vector->k - cross.k * sine;
	*vector = result;
}

/* marks a jump pending along a direction */
static inline void actor_path_jump_pending(s_actor_view *actor, real i, real j, real distance)
{
	real *jump = (real *)&actor->unknown468;
	actor->unknown464 = true;
	actor->unknown466 = true;
	jump[0] = i;
	jump[1] = j;
	jump[2] = distance;
	jump[3] = distance;
}

// Retail 0x1f6090, disabled with its blocker above.
void __stdcall function_1f6090(long actor_index, vector3f const *input, vector3f *facing_out,
	short *type_out, vector3f *movement_out, bool *turning, bool *reached)
{
	s_actor_view *actor = actor_get(actor_index);
	s_actor_movement_block const *block = (s_actor_movement_block const *)function_1e4a50(actor->unknown054);
	short axis = actor->unknown5d6;
	short override_axis = *(short *)actor->unknown454;
	vector3f facing = actor->unknown290;
	vector3f throttle = *g_4687a4;
	real scale = 1.0f;
	real threshold = 0.8660254f;
	bool last = false;
	s_actor_path_entry *entry = NULL;

	vector3f v = *input;
	v.k = 0.0f;
	real distance = normalize_inline(&v);
	if (distance == 0.0f)
		v = actor->unknown290;

	real radius;
	if (block)
	{
		radius = 0.05f;
		if (block->arrival_radius > radius)
			radius = block->arrival_radius;
	}
	else
	{
		radius = 0.2f;
	}

	if (actor->unknown50c && actor->unknown504 == 1)
	{
		entry = (s_actor_path_entry *)&actor->unknown53c[actor->unknown53a];
		last = actor->unknown53a == actor->unknown539 - 1;
		if (entry->type != 2)
			*reached = radius > distance;
	}
	else
	{
		*reached = false;
	}

	/* a request made once */
	if (actor->unknown225 && !actor->unknown226 && !function_110ab0(actor->unknown018) &&
		!actor->unknown264 && !actor->unknown229)
	{
		long unit_index = actor->unknown018;
		short action;
		function_e70b0(unit_index, &action);
		if (action == 0)
		{
			if (function_e68c0(0x33, unit_index))
				function_114b60(NONE, 0x10, actor->unknown018, 0xd, NULL);
			actor->unknown226 = true;
		}
	}

	/* the requests the current entry needs */
	if (!*reached && block && !function_110ab0(actor->unknown018) && function_1f8660(actor_index))
	{
		long unit_index = actor->unknown018;
		byte *unit = (byte *)object_get(unit_index);
		short action = *(short *)(unit + *(short *)(unit + 0x346) + 0x36);

		if (action == 6 || action == 7)
		{
			function_e68c0(0x27, unit_index);
		}
		else if ((actor->unknown4ac == 6 || actor->unknown4ac == 5 || actor->unknown4ac == 3) &&
			!actor->unknown229 && (block->flags & 0x20))
		{
			if (action == 5)
				function_e68c0(0x28, unit_index);
			else if (!actor->unknown264)
				function_e68c0(0x2c, unit_index);
			actor->unknown5d0 = false;
		}
		else if (entry->type == 2)
		{
			long surface_index = entry->surface_index;
			if (surface_index != NONE && !actor->unknown264)
			{
				if (action == 5)
				{
					point3f position;
					vector3f forward;
					function_b9dd0(unit_index, &position);
					function_b9fc0(unit_index, &forward, NULL);
					s_request_move request;
					request.type = 0x22;
					request.point = position;
					request.direction_i = forward.i;
					request.direction_j = forward.j;
					request.unknown18 = true;
					function_e6900(actor->unknown018, (s_unit_request *)&request);
				}
				else if (action == 0)
				{
					s_pathfinding_data *pathfinding = (s_pathfinding_data *)function_1fa7f0();
					s_actor_path_surface *surface = (s_actor_path_surface *)pathfinding->surfaces + surface_index;
					if (surface->unknown0a == 2)
					{
						*reached = true;
					}
					else
					{
						pathfinding = (s_pathfinding_data *)function_1fa7f0();
						surface = (s_actor_path_surface *)pathfinding->surfaces + surface_index;
						point3f const *start = &pathfinding->vertices[surface->first_vertex];
						point3f const *end = &pathfinding->vertices[surface->second_vertex];
						s_request_move request;
						request.type = 0x22;
						request.point = *start;
						request.direction_i = end->x;
						request.direction_j = end->y;
						request.unknown18 = false;
						function_e6900(unit_index, (s_unit_request *)&request);
					}
				}
				else
				{
					function_e68c0(0x27, unit_index);
				}
			}
			actor->unknown5d0 = false;
		}
		else if (entry->type == NONE && action == 5)
		{
			byte const *other = (byte const *)function_1e4990(actor->unknown054);
			if ((block->flags & 0x20) || (other && (*other & 2)))
				function_e68c0(0x28, unit_index);
			else
				function_e68c0(0x27, unit_index);
		}
	}

	/* the unit's animation mode, taking the character's weapon for 0x4000089 */
	long mode = function_1f5f30(actor_index);
	if (mode == 0x4000089)
	{
		if (actor->unknown26c == NONE)
		{
			long unit_index = actor->unknown018;
			long first;
			long second;
			if (function_10f630(unit_index, &first, &second) && first != mode &&
				!function_10fcd0(unit_index, mode, 0x7000101, 0x7000101))
			{
				bool armed = false;
				byte const *unit = (byte const *)object_get(actor->unknown018);
				long weapon_index = function_cbd50(actor->unknown018, *(signed char const *)(unit + 0x212));
				if (weapon_index != NONE)
				{
					byte const *element = (byte const *)function_1e4ef0(actor_index);
					if (element)
					{
						long tag_index = *(long const *)(element + 0x48);
						if (tag_index != NONE && object_get(weapon_index)->tag_index != tag_index)
						{
							long placement[0xc4 / 4];
							function_b7930(placement, tag_index, actor->unknown018, NULL);
							long object_index = function_b7b40(placement);
							if (object_index != NONE && function_cd0c0(actor->unknown018, object_index, 4))
								armed = true;
						}
					}
				}
				if (!armed)
					mode = 0x6000086;
			}
		}
	}
	if (mode != NONE)
		actor->unknown7fc = mode;

	/* whether the actor may move, and a pending animation */
	if (actor->unknown267)
	{
		actor->unknown5d0 = false;
		actor->unknown5d6 = 0;
		actor->unknown6d1 = actor->unknown004 == 0xf || actor->unknown274 != NONE;
	}
	else if (actor_get(actor_index)->unknown018 != NONE && function_110ab0(actor_get(actor_index)->unknown018))
	{
		actor->unknown5d0 = false;
		actor->unknown6d1 = false;
	}
	else if (actor->unknown6c8 != 0)
	{
		actor->unknown6d1 = true;
		actor->unknown5d0 = false;
		actor->unknown620 = (short)real_to_long(g_510c54->field_2_3 * 2.0f);

		bool play = false;
		bool wait = false;
		if (!actor->unknown6cc)
		{
			play = true;
		}
		else if (actor->unknown686 > 0 && actor->unknown684 >= 1)
		{
			vector3f direction;
			if (function_2967b0(actor_index, (s_type_952051 *)actor->unknown688, false, &direction, NULL, NULL))
			{
				direction.k = 0.0f;
				if (function_30bf0(&direction) > 0.0f)
				{
					if (actor->unknown290.j * direction.j + actor->unknown290.k * direction.k +
						direction.i * actor->unknown290.i > 0.98f)
						play = true;
					else
						wait = true;
				}
			}
		}
		if (wait)
		{
			actor->unknown44d = true;
		}
		else
		{
			if (play)
				function_1f57f0(actor_index, actor->unknown6c8, NULL);
			actor->unknown6c8 = 0;
			actor->unknown6cc = false;
		}
	}
	else
	{
		byte const *unit = (byte const *)object_get(actor->unknown018);
		if (*(short const *)(unit + *(short const *)(unit + 0x346) + 0x36) != 0 || actor->unknown7fc == 0x6000084)
		{
			actor->unknown5d0 = false;
			actor->unknown6d1 = false;
		}
		else if (actor->unknown264 && !actor->unknown229)
		{
			actor->unknown5d0 = false;
			actor->unknown6d1 = true;
		}
		else if (actor->unknown3e2 > 0)
		{
			actor->unknown5d0 = false;
		}
		else if (actor->unknown7fc != 0x6000086 || (block && (block->flags & 2)))
		{
			actor->unknown5d1 = false;
		}
	}

	if (actor->unknown3e0 > 0)
	{
		actor->unknown449 = true;
		actor->unknown44a = true;
	}
	if (actor->unknown44d)
		actor->unknown6d2 = true;

	if (actor->unknown5d0 && !actor->unknown5d2)
	{
		/* weaving from side to side */
		if (actor->unknown481 && distance > 0.8f)
		{
			byte const *element = (byte const *)function_1e4ef0(actor_index);
			if (element && *(real const *)(element + 0x3c) > 0.0f && *(real const *)(element + 0x38) > 0.0f &&
				(function_26c180(actor_index), actor->unknown27c.unknown10 != NONE))
			{
				vector3f turned = v;
				real angle = *(real const *)(element + 0x38);
				long ticks = function_1469f0(*(real const *)(element + 0x3c));
				if (1.6f > distance)
					angle = (distance - 0.8f) * angle * 1.25f;
				short half = (short)(actor->unknown6fa * 2 / (short)ticks);
				if (half == 1)
					angle = -angle;
				else if (half > 1)
					actor->unknown6fa = 0;
				actor_path_rotate(&turned, g_4687b0, (real)sin(angle), (real)cos(angle));

				vector3f direction;
				function_210770(actor->unknown27c.point.output_index, &turned, &direction);
				s_sector_trace_result trace;
				function_26c590((s_pathfinding_data const *)function_1fa7f0(), &actor->unknown27c.point.point,
					actor->unknown27c.unknown10, NONE, &direction, 0.5f, NULL, (s_path_trace_result *)&trace);
				if (trace.blocked)
				{
					actor->unknown6fa = 0;
				}
				else
				{
					v = turned;
					actor->unknown6fa++;
					threshold = -0.2f;
				}
			}
			else
			{
				actor->unknown6fa = 0;
			}
		}
		else
		{
			actor->unknown6fa = 0;
		}

		/* the facing */
		if (override_axis >= 0 && override_axis <= 3)
		{
			axis = override_axis;
			switch (axis)
			{
			case 0:
				facing.i = v.i;
				facing.j = v.j;
				break;
			case 1:
				facing.i = -v.i;
				facing.j = -v.j;
				break;
			case 2:
				facing.i = -v.j;
				facing.j = v.i;
				break;
			case 3:
				facing.i = v.j;
				facing.j = -v.i;
				break;
			}
			facing.k = v.k;
		}
		else
		{
			if (distance > 0.8f && threshold > 0.0f)
				threshold = 0.0f;
			if (actor->unknown5d1)
			{
				function_e64e0(input, &actor->unknown5f8, &actor->unknown290, false, &facing, &axis);
			}
			else
			{
				facing = v;
				axis = 0;
			}
		}

		/* turning in place or moving */
		real dot = actor->unknown290.k * facing.k + actor->unknown290.j * facing.j + facing.i * actor->unknown290.i;
		if (actor->unknown604 && !(threshold > 0.99f))
			threshold = 0.99f;
		bool moving;
		if (dot > threshold)
		{
			moving = true;
			if (function_26bf10(actor->unknown018))
			{
				function_26c180(actor_index);
				if (actor->unknown27c.unknown10 != NONE)
				{
					vector3f const *forward = &actor->unknown290;
					vector3f direction;
					switch (axis)
					{
					case 0:
						direction.i = forward->i;
						direction.j = forward->j;
						direction.k = forward->k;
						break;
					case 1:
						direction.i = -forward->i;
						direction.j = -forward->j;
						direction.k = forward->k;
						break;
					case 2:
						direction.i = forward->j;
						direction.j = -forward->i;
						direction.k = forward->k;
						break;
					case 3:
						direction.i = -forward->j;
						direction.j = forward->i;
						direction.k = forward->k;
						break;
					}
					if (normalize2d((point2f *)&direction) > 0.0f)
					{
						vector3f converted;
						function_210770(actor->unknown27c.point.output_index, &direction, &converted);
						s_sector_trace_result trace;
						if (function_26c590((s_pathfinding_data const *)function_1fa7f0(), &actor->unknown27c.point.point,
							actor->unknown27c.unknown10, NONE, &converted, 0.4f, NULL, (s_path_trace_result *)&trace))
						{
							if (!(threshold > 0.95f))
								threshold = 0.95f;
							moving = dot > threshold;
						}
						if (trace.unknown1c && block->unknown04 * 1.5f > trace.unknown20 && trace.unknown20 > 0.0f)
							actor_path_jump_pending(actor, direction.i, direction.j, 0.75f);
					}
				}
			}
		}
		else
		{
			moving = false;
		}

		/* the throttle */
		real stopping;
		real round_trip;
		function_1f40b0(actor_index, &stopping, &round_trip);
		if (!actor->unknown4ae && stopping > distance)
		{
			if (distance > round_trip + 0.05f && stopping > round_trip)
			{
				real ratio = (distance - round_trip) / (stopping - round_trip);
				if (1.0f > ratio)
					scale = ratio;
			}
			else
			{
				scale = 0.0f;
			}
		}
		if (moving)
		{
			switch (axis)
			{
			case 0:
				throttle.i = 1.0f;
				break;
			case 1:
				throttle.i = -1.0f;
				break;
			case 2:
				throttle.j = -1.0f;
				break;
			case 3:
				throttle.j = 1.0f;
				break;
			}
			if (actor->unknown483)
				scale = 0.5f;
			if (block && block->maximum_scale > 0.0f && scale > block->maximum_scale)
				scale = block->maximum_scale;
			throttle.i *= scale;
			throttle.j *= scale;
			throttle.k *= scale;
			actor->unknown604 = false;
			*turning = false;
		}
		else
		{
			actor->unknown604 = true;
			actor->unknown6d2 = true;
			*turning = true;
		}
	}
	else
	{
		actor->unknown604 = false;
	}

	*type_out = axis;
	*facing_out = facing;
	*movement_out = throttle;
	bool handled = function_1f58c0(actor_index, facing_out, type_out);

	/* the current entry's own movement */
	s_actor_view *current = actor_get(actor_index);
	if (current->unknown50c && current->unknown504 == 1 && !function_110ab0(actor->unknown018) &&
		!handled && entry->type != NONE)
	{
		s_pathfinding_data *pathfinding = (s_pathfinding_data *)function_1fa7f0();
		s_actor_path_surface *surface = NULL;
		handled = true;
		if (entry->surface_index != NONE && pathfinding)
			surface = (s_actor_path_surface *)pathfinding->surfaces + entry->surface_index;

		switch (entry->type)
		{
		case 1:
		case 6:
		{
			function_26c180(actor_index);
			*facing_out = v;
			*type_out = 0;
			*movement_out = *g_4687a4;
			actor->unknown6d2 = true;
			actor->unknown6d1 = false;
			if (actor->unknown27c.unknown10 == entry->node_index)
			{
				*reached = true;
				break;
			}

			bool lined_up;
			if (actor->unknown5d0)
			{
				lined_up = v.j * actor->unknown290.j + v.i * actor->unknown290.i > 0.95;
			}
			else
			{
				short action;
				function_e70b0(actor->unknown018, &action);
				lined_up = action == 5;
			}
			if (!lined_up || (surface && surface->type == 1 && (surface->flags & 1)))
			{
				actor->unknown5d0 = false;
				break;
			}

			point3f point;
			function_210850(&entry->point, &point);
			point3f target = point;
			bool crossing = surface && surface->source_output != surface->destination_output;
			if (entry->type == 6)
			{
				if (function_2702a0(actor_index, 0x11000555, 0x5000049, 0x6000086, 0x800001e,
					&point, g_4687a8, &target, NULL, NULL, NULL))
				{
					target.x = point.x;
					target.y = point.y;
				}
				else
				{
					target = point;
				}
			}
			bool moved = function_1f8160(actor_index, surface ? surface->types : NULL, &target, !crossing);
			if (moved)
			{
				*movement_out = *g_4687a8;
			}
			else if (crossing)
			{
				*movement_out = *g_4687a4;
				actor->unknown5d0 = false;
				break;
			}
			else
			{
				actor_path_jump_pending(actor, v.i, v.j, 1.5f);
			}

			/* where the actor will face after the transition */
			if (entry->type == 6 && !actor->unknown264)
			{
				s_pathfinding_node const *node = &pathfinding->nodes[entry->node_index];
				s_collision_result_1697c0 collision;
				point3f entry_point;
				collision.unknown24 = NONE;
				function_210850(&entry->point, &entry_point);
				function_1fa600((vector3f *)&point, node->flags);
				if (function_26d100((vector3f const *)&point, NULL, &collision, &entry_point) != NONE)
				{
					vector3f const *normal = (vector3f const *)((byte const *)&collision + 0x28);
					vector3f vector;
					if (function_210770(entry->point.output_index, normal, (vector3f *)&point))
						vector = *(vector3f *)&point;
					else
						vector = *normal;
					actor->unknown634 = g_510c54->game_time + g_510c54->field_2_3 * 4;
					actor->unknown638 = entry->point;
					actor->unknown648 = vector;
				}
			}
			break;
		}
		case 5:
			if (actor->unknown5d0)
			{
				function_26c180(actor_index);
				*facing_out = v;
				*type_out = 0;
				*movement_out = *g_4687a4;
				actor->unknown6d2 = true;
				actor->unknown6d1 = false;
				*reached = false;
				if (v.j * actor->unknown290.j + v.i * actor->unknown290.i > 0.95)
				{
					s_path_settings settings;
					function_1f9240(actor_index, &settings);
					point3f const *vertices = pathfinding->vertices;
					point3f const *end = &vertices[surface->second_vertex];
					if (function_26f2d0(&actor->position, end, &settings))
					{
						point3f const *away = &vertices[surface->unknown0a];
						s_request_vector request;
						request.type = 0x2b;
						request.point = *end;
						request.vector.i = away->x * -1.0f;
						request.vector.j = away->y * -1.0f;
						request.vector.k = away->z * -1.0f;
						if (function_e6900(actor->unknown018, (s_unit_request *)&request))
							*reached = true;
					}
					else if (function_1f8160(actor_index, NULL, end, true))
					{
						*reached = true;
					}
				}
				else
				{
					actor->unknown5d0 = false;
				}
			}
			break;
		}
	}

	/* a target object */
	if (block && actor->unknown5ac != NONE)
	{
		if ((block->marker_flags & 0x1803) && actor->unknown5ac != NONE &&
			!function_110ab0(actor->unknown018) && actor->unknown5d0)
		{
			if (actor->unknown5b0 == 3)
				handled = function_1f7c70(actor_index, 3, true, facing_out, type_out, movement_out);
			else if (actor->unknown5b0 == 5)
				handled = function_1f7c70(actor_index, 5, false, facing_out, type_out, movement_out);
		}
		if (!handled)
		{
			bool approached = false;
			if (actor->unknown5b0 == 1 && !function_110ab0(actor->unknown018) && actor->unknown5d0)
			{
				s_slot_object_view *object = object_get(actor->unknown5ac);
				vector3f offset;
				offset.i = object->unknown030.x - actor->position.x;
				offset.j = object->unknown030.y - actor->position.y;
				offset.k = object->unknown030.z - actor->position.z;
				real length = function_30bf0(&offset);
				if (length > 0.0f && block->unknown04 * 2.0f + object->unknown03c > length)
				{
					real along = (offset.i * v.i + v.j * offset.j + offset.k * v.k) * length;
					if (along > 0.0f)
					{
						approached = true;
						if (actor->unknown290.k * v.k + actor->unknown290.j * v.j + v.i * actor->unknown290.i > 0.99f)
						{
							byte const *definition = (byte const *)g_4e3b44[object->tag_index & 0xffff].bytes;
							short shift = 1;
							if (*(long const *)(definition + 0x5c) > 0)
							{
								short count = *(short const *)(*(byte const *const *)(definition + 0x60) + 0xe);
								if (count > 0)
								{
									short limited = (count > 6 ? 6 : count) - 1;
									if (limited > 1)
										shift = limited;
								}
							}
							signed char types = (signed char)(1 << shift);
							real reach = along * 2.0f;
							point3f target;
							target.x = reach * v.i + actor->position.x;
							target.y = reach * v.j + actor->position.y;
							target.z = reach * v.k + actor->position.z;
							if (function_1f8160(actor_index, &types, &target, true))
							{
								function_1f9450(actor_index, actor->unknown5ac);
								actor->unknown5ac = NONE;
								actor->unknown5b0 = NONE;
							}
						}
						else
						{
							*facing_out = v;
							*type_out = 0;
							*movement_out = *g_4687a4;
							actor->unknown6d2 = true;
							actor->unknown6d1 = false;
							actor->unknown5d0 = false;
						}
					}
				}
			}
			if (!approached && (block->marker_flags & 0xe0) && actor->unknown5b0 == 4 &&
				!function_110ab0(actor->unknown018) && actor->unknown5d0)
			{
				s_object_marker marker;
				point3f target;
				if (function_26fc80(actor_index, actor->unknown5ac, block->unknown04, &marker, &target))
				{
					vector3f offset;
					offset.i = target.x - actor->position.x;
					offset.j = target.y - actor->position.y;
					offset.k = target.z - actor->position.z;
					real length = magnitude3d(&offset);
					if (length > 0.0f &&
						block->unknown04 * 2.0f + object_get(actor->unknown5ac)->unknown03c > length &&
						function_1f8160(actor_index, NULL, &target, true))
					{
						function_1f9450(actor_index, actor->unknown5ac);
						actor->unknown5ac = NONE;
						actor->unknown5b0 = NONE;
					}
				}
			}
		}
	}

	/* a jump toward the last point now and then */
	if (actor->unknown5d0 && last && actor->unknown5ac == NONE && !actor->unknown464 &&
		block && (block->flags & 8) && actor->unknown086 >= 3 && distance > 2.0f &&
		g_510c54->game_time - actor->unknown7f8 > g_510c54->field_2_3 * 3)
	{
		point3f target;
		target.x = input->i + actor->position.x;
		target.y = actor->position.y + input->j;
		target.z = actor->position.z + input->k;
		function_1f8160(actor_index, NULL, &target, false);
		actor->unknown7f8 = g_510c54->game_time;
	}

	/* the pending jump */
	if (actor->unknown264 || actor->unknown26c != NONE)
	{
		actor->unknown622 = false;
		return;
	}
	if (actor->unknown464 && !function_1f50e0(actor_index))
	{
		bool jumped = false;
		if (actor->unknown465)
		{
			s_request_jump request;
			memset(&request, 0, sizeof(request));
			request.type = 0x30;
			request.unknown04 = true;
			if (actor->unknown466)
			{
				request.direction = *(point2f *)&actor->unknown468;
			}
			else
			{
				request.direction.x = actor->unknown290.i;
				request.direction.y = actor->unknown290.j;
				if (normalize2d(&request.direction) == 0.0f)
					request.direction = *g_468778;
			}
			if (function_e6900(actor->unknown018, (s_unit_request *)&request))
			{
				function_1fb7e0(0x5a, actor_index, NULL, NONE, -1);
				jumped = true;
			}
		}
		if (!jumped)
			function_1e4260(actor_index);
		if (actor->unknown466)
		{
			real const *jump = (real const *)&actor->unknown468;
			actor->unknown624 = jump[0];
			actor->unknown628 = jump[1];
			actor->unknown62c = jump[2];
			actor->unknown630 = jump[3];
			actor->unknown622 = true;
		}
	}
}
#endif
