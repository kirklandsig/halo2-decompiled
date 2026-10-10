// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_265CB0.CPP: the props an actor knows (0x265cb0..) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "props.h"
#include <math.h>
#include "unknown_26b230.h"
#include "object_markers.h"
#include "unknown_1fb7e0.h"
#include "object_queries.h"
#include "unknown_249e20.h"
#include "units.h"
#include "unknown_2729b0.h"
#include "data_array.h"
#include "unknown_25d020.h"
#include <string.h>


point3f *function_b9dd0(long object_index, point3f *result);
long function_1469f0(real seconds);
long function_1e4a90(long index);
long function_25d810(long object_index, long actor_index, bool create);
void function_118e10(long object_index, point3f *point);
real function_20e190(long object_index);


struct s_object_motion_view
{
	point3f point;
	point3f center;
	byte unknown18[0xc];
	long location[2];
	vector3f velocity;
};

// @retail 0x2640c0
void function_2640c0(long object_index, s_object_motion_view *result)
{
	long const *object_reference = &object_index;
	s_slot_object_view *object = object_get(object_index);
	long root_index = NONE;
	for (long index = object_index; index != NONE; index = object_get(index)->parent_index)
		root_index = index;
	s_slot_object_view *root = object_get(root_index);
	function_b9dd0(object_index, &result->center);
	result->velocity = root->velocity;
	result->location[0] = *(long *)((byte *)root + 0x28);
	result->location[1] = *(long *)((byte *)root + 0x2c);
	if ((1 << object->type) & 3)
	{
		s_object_marker marker;
		function_b8d30(*object_reference, 0x4000095, &marker, 1, false);
		result->point = *(point3f *)((byte *)&marker + 0x60);
	}
	else if (object->type == 12)
		function_118e10(*object_reference, &result->point);
	else
		result->point = result->center;
}

// @retail 0x266540
void function_266540(long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	long index = *(long *)((byte *)actor + 0x338);
	if (index != NONE)
	{
		long object_index = actor->unknown018;
		if (object_index != NONE && *(char *)((byte *)actor + 0x324) >= 2)
		{
			s_prop_node *node = (s_prop_node *)prop_ref_get(index);
			s_type_76cf92 *prop = prop_get(node->prop_index);
			if (!prop->unknown35 && prop->unknown23 && *(real *)((byte *)prop + 0x2c) >= 2.0f)
			{
				s_type_f95cd3 *view = function_25d740(node);
				if (view->unknown14 != NONE && *(short *)view < 9 &&
					(real)(g_510c54->game_time - view->unknown14) > g_510c54->rate * 8.0f &&
					function_20e190(object_index) > 0.0f)
				{
					function_1fb7e0(0xb7, *actor_reference, NULL, node->object_index, NONE);
					prop->unknown35 = 1;
				}
			}
		}
	}
}

bool function_25d9b0(long prop_index);
bool function_1e2030(long actor_index);
void *function_1e4f90(long actor_index);
void *function_1e5240(long actor_index);
long function_1b9190(long object_index, long *rider_index);

// @retail 0x265d30
real __stdcall function_265d30(long actor_index, long prop_index)
{
	long const *actor_reference = &actor_index;
	long const *prop_reference = &prop_index;
	s_prop_datum *node = prop_ref_get(prop_index);
	s_actor_view *actor = actor_get(actor_index);
	s_type_76cf92 *prop = prop_get(node->prop_index);
	s_type_5cfb45 *state = function_25d690(node);
	s_type_f95cd3 *view = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking) view = &tracking->view;
	}
	if (*(bool *)((byte *)prop + 0x26) || !prop->unknown23 || node->state <= 0 ||
		(state->unknown5e && *(short *)((byte *)state + 0x5c) * g_510c54->rate >= 5.0f) ||
		object_get(node->object_index)->type == 15 || (node->type != 1 && node->type != 6))
		return 0.0f;
	short state_score = 0;
	short range_score = 0;
	real current_bonus = 0.0f;
	short special_score = 0;
	real view_bonus = 0.0f;
	real direction_bonus = 0.0f;
	if (actor->unknown007)
		range_score = 0;
	else if (function_25d9b0(*prop_reference))
		range_score = 0;
	else if (*(bool *)((byte *)actor + 0x269))
		range_score = *(long *)((byte *)actor + 0x26c) == state->unknown3c ? 10 : 0;
	else if (!function_1e2030(*actor_reference))
	{
		real minimum = 0.0f;
		byte *settings = (byte *)function_1e4f90(actor_index);
		if (settings) minimum = *(real *)(settings + 4);
		if (node->unknown28 < 1.0f && !(node->state == 3 && view && view->unknown88))
			range_score = 10;
		else if (state->unknown3c != NONE)
			range_score = 0;
		else if (state->unknown69 != *(bool *)((byte *)actor + 0x265))
			range_score = 10;
		else if (minimum > node->unknown28)
			range_score = 10;
	}
	else
	{
		byte *settings = (byte *)function_1e5240(actor_index);
		if (node->unknown28 < 1.0f && !(node->state == 3 && view && view->unknown88))
			range_score = 7;
		else if (settings)
		{
			real minimum = *(real *)(settings + 0x18);
			s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
			if (variant)
			{
				switch (function_272ad0(variant))
				{
				case 1: minimum = *(real *)(settings + 0x28); break;
				case 2: minimum = *(real *)(settings + 0x30); break;
				}
			}
			if (minimum > node->unknown28) range_score = 5;
			else if (*(real *)(settings + 0xc) > node->unknown28) range_score = 3;
		}
	}
	if (state->unknown5e) state_score = 1;
	else if (view && !actor->unknown007 && view->unknown2a) state_score = 6;
	else if (node->state >= 1)
	{
		if (actor->unknown007) state_score = 2;
		else if (view && (view->unknown06 == 0 || view->unknown06 == 1))
			state_score = state->unknown63 && view->unknown39 <= 1 ? 5 : 4;
		else if (node->state >= 3)
			state_score = !view || view->unknown88 ? 1 : 2;
		else state_score = 3;
	}
	if (*prop_reference == *(long *)((byte *)actor + 0x338) && actor->unknown086 >= 5 && !state->unknown5e)
		current_bonus = *(bool *)((byte *)actor + 0x354) ? 0.2f : 2.0f;
	if (state->unknown67) special_score = 10;
	if (view)
	{
		view_bonus = (real)(view->unknown60 * 0.3);
		if (view_bonus < 0.0f) view_bonus = 0.0f;
		else if (view_bonus > 3.0f) view_bonus = 3.0f;
	}
	real distance_bonus = 5.0f / (node->unknown28 * 0.1f + 1.0f);
	if (*(long *)((byte *)actor + 0x26c) != NONE && actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1))
	{
		long rider_index;
		function_1b9190(*(long *)((byte *)actor + 0x26c), &rider_index);
		if (rider_index != NONE)
		{
			s_slot_object_view *rider = object_get(rider_index);
			vector3f delta;
			if (view) delta = *(vector3f *)((byte *)view + 0x2c);
			else
			{
				delta.i = state->position.x - *(real *)((byte *)actor + 0x238);
				delta.j = state->position.y - *(real *)((byte *)actor + 0x23c);
				delta.k = state->position.z - *(real *)((byte *)actor + 0x240);
			}
			vector3f const volatile *velocity = (vector3f const volatile *)((byte *)rider + 0x150);
			direction_bonus = 0.0f > (velocity->k * delta.k + velocity->j * delta.j + velocity->i * delta.i) * 6.0f ? 0.0f : (velocity->k * delta.k + velocity->j * delta.j + velocity->i * delta.i) * 6.0f;
		}
	}
	return (real)(range_score + state_score) + (real)special_score + direction_bonus + distance_bonus + view_bonus + current_bonus;
}

void *function_1e51a0(long actor_index);
bool function_11bf30(s_location const *location);

struct s_perception_origin_view
{
	point3f point;
	byte unknown0c[0xc];
	vector3f direction;
	s_location location;
};

// @retail 0x263ed0
short function_263ed0(long actor_index, s_perception_origin_view const *origin, point3f const *point,
	s_location const *location, short type, short mode)
{
	s_location const *const *location_reference = &location;
	short const *type_reference = &type;
	short const *mode_reference = &mode;
	s_actor_view *actor = actor_get(actor_index);
	byte *settings = (byte *)function_1e51a0(actor_index);
	short result = 0;
	if (settings && type && origin->location.cluster_index != NONE && location->cluster_index != NONE)
	{
		vector3f delta;
		vector3d_from_points3d(&origin->point, point, &delta);
		real range = *(real *)(settings + 0x18);
		real distance_squared = length_sq3f(&delta);
		if (dot3f(&origin->direction, &delta) < 0.0f)
			range *= 0.8f;
		if (actor->unknown084 == 3)
			range *= 0.7f;
		else if (actor->unknown084 == 1)
			range *= 0.4f;
		if (*type_reference == 4)
			range *= 0.2f;
		else if (*type_reference == 1)
			range *= 0.45f;
		else if (*type_reference == 3)
			range *= 0.7f;
		if (function_11bf30(&origin->location) || function_11bf30(*location_reference))
			range *= 0.25f;
		if (*mode_reference != 0 && *mode_reference != 1)
			range *= 0.7f;
		if (range * range > distance_squared)
		{
			long cluster_a = origin->location.cluster_index;
			long cluster_b = location->cluster_index;
			s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
			if (!function_249c20(bsp, cluster_b, cluster_a))
			{
				real distance = function_249d60(bsp, cluster_b, cluster_a) * 1.3333333730697632f;
				real direct_distance = (real)sqrt(distance_squared);
				distance = distance > direct_distance ? distance : direct_distance;
				if (range > distance)
					result = 2 + (*type_reference >= 3);
			}
		}
	}
	return result;
}

struct s_distance_rates_view
{
	byte unknown00[8];
	real near_distance;
	real far_distance;
	real maximum_distance;
	real far_rate;
};

// @retail 0x263d70
void function_263d70(long actor_index, real distance, real rate, real scale, real *secondary, real *primary)
{
	s_distance_rates_view *settings = (s_distance_rates_view *)function_1e51a0(actor_index);
	real *const *primary_reference = &primary;
	real *const *secondary_reference = &secondary;
	real a = 0.0f;
	real b = 0.0f;
	if (settings)
	{
		if (distance > settings->maximum_distance)
		{
			a = 0.0f;
			b = 0.0f;
		}
		else
		{
			real near_rate = rate * scale;
			real far_rate = settings->far_rate * scale;
			real near_secondary = near_rate * 0.7f;
			real far_secondary = far_rate * 0.7f;
			if (far_secondary > 3.5f)
				far_secondary = 3.5f;
			if (distance > settings->far_distance)
			{
				a = far_rate;
				b = far_secondary;
			}
			else
			{
				real near_distance = settings->near_distance;
				real secondary_distance = near_distance * 0.8f;
				if (distance < near_distance)
					a = near_rate;
				else
				{
					real range = settings->far_distance - near_distance;
					if (range <= 0.0f)
						a = far_rate;
					else
					{
						real fraction = (distance - near_distance) / range;
						a = (1.0f - fraction) * near_rate + fraction * far_rate;
					}
				}
				if (distance < secondary_distance)
					b = near_secondary;
				else
				{
					real range = settings->far_distance - secondary_distance;
					if (range <= 0.0f)
						b = far_secondary;
					else
					{
						real fraction = (distance - secondary_distance) / range;
						b = (1.0f - fraction) * near_secondary + fraction * far_secondary;
					}
				}
			}
		}
	}
	**primary_reference = a;
	**secondary_reference = b;
}

PRIVATE inline real basis_dot3f(vector3f const *a, vector3f const *b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k;
}

// @retail 0x263740
void function_263740(long actor_index, vector3f const *direction, vector3f const *axis_a,
	vector3f const *axis_b, vector3f const *axis_c, real rate, real scale, real *secondary, real *primary)
{
	vector3f components;
	components.i = basis_dot3f(axis_a, direction);
	components.j = basis_dot3f(axis_b, direction);
	components.k = basis_dot3f(axis_c, direction);
	real vertical = (real)atan2(components.k, sqrt(components.i * components.i + components.j * components.j));
	if (vertical > 0.5235987901687622f || vertical < -0.7853981852531433f)
	{
		*secondary = 0.0f;
		*primary = 0.0f;
	}
	else
		function_263d70(actor_index, (real)fabs(atan2(components.j, components.i)), rate, scale, secondary, primary);
}

short __stdcall function_c8b80(long object_index, s_object_seat *seats, short maximum_count);
bool function_26d370(point3f const *point, vector3f const *direction, plane3f const *plane, real *distance);
real function_30bf0(vector3f *vector);

// @retail 0x263810
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range)
{
	long const *actor_reference = &actor_index;
	point3f const *const *endpoint_reference = &endpoint;
	char const *posture_reference = &posture;
	short const *mode_reference = &mode;
	bool const *facing_reference = &use_facing;
	bool *const *range_reference = &out_of_range;
	volatile short result = 0;
	if (mode == 0 || mode == 1)
	{
		s_actor_view *actor = actor_get(actor_index);
		byte *settings = (byte *)function_1e51a0(actor_index);
		if (settings)
		{
			real range = *(real *)(settings + 4);
			vector3f delta;
			delta.i = point->x - origin->x;
			delta.j = point->y - origin->y;
			delta.k = point->z - origin->z;
			if (*endpoint_reference && !actor->unknown007)
			{
				vector3f motion;
				motion.i = endpoint->x - point->x;
				motion.j = endpoint->y - point->y;
				motion.k = endpoint->z - point->z;
				if (motion.k * motion.k + motion.j * motion.j + motion.i * motion.i > 0.002500000176951289f)
				{
					plane3f plane;
					plane.n = *(vector3f *)((byte *)actor + 0x2c0);
					plane.d = origin->y * plane.n.j + origin->z * plane.n.k + origin->x * plane.n.i;
					real volatile amount;
					if (function_26d370(point, &motion, &plane, (real *)&amount) && amount >= 0.0f)
					{
						real clipped_amount = amount;
						if (clipped_amount > 1.0f) clipped_amount = 1.0f;
						if (clipped_amount > 0.0f)
						{
							delta.i = motion.i * clipped_amount + point->x - origin->x;
							delta.j = motion.j * clipped_amount + point->y - origin->y;
							delta.k = motion.k * clipped_amount + point->z - origin->z;
						}
					}
				}
			}
			real distance_squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
			if (!(range * range > distance_squared))
			{
				if (*range_reference) **range_reference = true;
			}
			else
			{
				real scale = 1.0f;
				if (!(*settings & 1))
				{
					switch (*posture_reference)
					{
					case 0: scale = 0.3f; break;
					case 1: scale = 0.7f; break;
					}
				}
				real scaled_range = scale * range;
				if (!(scaled_range * scaled_range > distance_squared))
				{
					if (*range_reference) **range_reference = true;
				}
				else
				{
					real secondary, primary;
					if (!actor->unknown007 && *facing_reference)
					{
						function_263740(*actor_reference, &delta, (vector3f *)((byte *)actor + 0x2a8),
							(vector3f *)((byte *)actor + 0x2b4), (vector3f *)((byte *)actor + 0x2c0),
							range, scale, &secondary, &primary);
						long object_index = *(long *)((byte *)actor + 0x26c);
						if (object_index != NONE)
						{
							s_object_seat seats[64];
							short count = function_c8b80(object_index, seats, 64);
							for (short i = 0; i < count; ++i)
							{
								long occupant = function_c8f60(seats[i].object_index, seats[i].seat_index);
								if (occupant != NONE)
								{
									s_slot_object_view *object = object_get(occupant);
									if (object->player_index != NONE)
									{
										vector3f const *forward = (vector3f *)((byte *)object + 0x18c);
										vector3f side, up;
										side.i = g_4687b0->j * forward->k - g_4687b0->k * forward->j;
										side.j = g_4687b0->k * forward->i - g_4687b0->i * forward->k;
										side.k = g_4687b0->i * forward->j - g_4687b0->j * forward->i;
										function_30bf0(&side);
										up.i = side.k * forward->j - side.j * forward->k;
										up.j = side.i * forward->k - side.k * forward->i;
										up.k = side.j * forward->i - side.i * forward->j;
										real a, b;
										function_263740(*actor_reference, &delta, forward, &side, &up, range, scale, &a, &b);
										if (a > secondary) secondary = a;
										if (b > primary) primary = b;
									}
								}
							}
						}
					}
					else
					{
						primary = scaled_range;
						secondary = scaled_range * 0.7f;
					}
					if (*mode_reference == 0 && secondary * secondary > distance_squared)
						result = distance_squared < 36.0f ? 3 : 2;
					else if (primary * primary > distance_squared)
						result = 1;
				}
			}
		}
	}
	return result;
}

short const g_44ae3c[13] = { 1, 1, 2, 3, 4, 6, 7, 8, 8, 9, 9, 9, 9 };
short g_470fa4[4] = { 0, 0, 2, 3 };
short g_470fe0[3] = { 1, 2, 3 };

void *function_272a00(long actor_index);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

// @retail 0x2662f0
void function_2662f0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short desired;
	long task_index = actor->unknown858;
	if (task_index != NONE)
	{
		byte *task = g_502408->data + (task_index & 0xffff) * 0xd4;
		short level = *(short *)(task + 0x84);
		if (level > 0)
		{
			desired = level - 1;
			goto reset_time;
		}
	}
	desired = g_44ae3c[actor->unknown328];
	{
		short clump_minimum = 1;
		short recent_minimum = 0;
		if (actor->unknown07c != NONE)
		{
			s_clump *group = (s_clump *)(g_502420->data + (actor->unknown07c & 0xffff) * 0x50);
			clump_minimum = g_470fa4[group->state];
		}
		if (actor->unknown26c == NONE && desired == 1 && actor->unknown086 == 0)
			desired = 0;
		if (actor->unknown358 > 0)
			recent_minimum = 2;
		short maximum = clump_minimum > recent_minimum ? clump_minimum : recent_minimum;
		desired = desired > maximum ? desired : (clump_minimum > recent_minimum ? clump_minimum : recent_minimum);
	}
	if (desired >= 3)
		goto reset_time;
	if (actor->unknown086 > 3)
	{
		actor->unknown086 = 3;
		*(long *)actor->unknown088 = 0;
	}
	{
		byte *definition = NULL;
		if (actor->unknown030 != NONE)
		{
			byte *squad = g_51e9d8->data + (actor->unknown030 & 0xffff) * 0x98;
			short definition_index = *(short *)(squad + 0x2a);
			if (definition_index != NONE)
				definition = *(byte **)((byte *)g_4e0350 + 0x244) + definition_index * 0x7c;
		}
		if (definition && desired < *(short *)(definition + 0x28) - 1)
			desired = *(short *)(definition + 0x28) - 1;
		else
		{
			byte *variant = (byte *)function_272a00(actor_index);
			if (variant)
			{
				short level = g_470fe0[*(short *)(variant + 0x20)];
				if (actor->unknown086 >= level && desired < level)
					desired = level;
			}
		}
	}
	if (desired >= actor->unknown086)
		goto reset_time;
	desired = actor->unknown086;
	switch (desired)
	{
	case 3:
		if (*(short *)actor->unknown088 * g_510c54->rate < 0.0f)
			break;
		desired = 2;
	case 2:
		if (*(short *)actor->unknown088 * g_510c54->rate >= 15.0f)
			desired = 1;
		break;
	}
	goto update;
reset_time:
	*(long *)actor->unknown088 = 0;
update:
	if (actor->unknown086 != desired)
	{
		if (desired >= 3 && actor->unknown086 < 2)
			function_1a8220(actor_index, 7, 2, 3, 1, 42, 3);
		*(long *)actor->unknown088 = 0;
		actor->unknown086 = desired;
	}
	else
		++*(long *)actor->unknown088;
	if (actor->unknown086 >= 7)
		*(bool *)((byte *)actor + 0x8c) = true;
}

// @retail 0x265cb0
void function_265cb0(long actor_index)
{
	long prop_index = actor_get(actor_index)->first_prop_index;

	for (;;)
	{
		s_prop_node_view *node;
		long current_index;
		s_prop_view_fields *view;

		if (prop_index == NONE)
		{
			break;
		}
		node = prop_node_get(prop_index);
		current_index = prop_index;
		prop_index = node->next_index;
		view = prop_node_view(node);
		if (view)
		{
			view->unknown3c = function_265d30(actor_index, current_index);
		}
	}
}

// @retail 0x263d30
long function_263d30(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result;
	if (actor->unknown086 >= 3)
		result = 2;
	else
		result = actor->unknown084 >= 4;
	return result;
}

PRIVATE __forceinline s_type_f95cd3 *actor_tracked_prop_view(long prop_index)
{
	s_prop_node_view *node = prop_node_get(prop_index);
	s_type_f95cd3 *result = NULL;
	if (node->view_index != NONE)
	{
		byte *base = g_502414->data + (node->view_index & 0xffff) * sizeof(s_type_e5ff81);
		if (base)
			result = (s_type_f95cd3 *)(base + 0x70);
	}
	return result;
}

// @retail 0x265c30
void function_265c30(long prop_index, long actor_index, bool active)
{
	long const volatile *actor_reference = &actor_index;
	bool const *active_reference = &active;
	s_type_f95cd3 *view = actor_tracked_prop_view(prop_index);
	if (view)
	{
		if (*active_reference)
		{
			view->unknown50 = g_510c54->game_time;
			*(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, prop_index);
		}
		else
		{
			view->unknown50 = NONE;
			*(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, prop_index);
		}
	}
}

// @retail 0x267770
void function_267770(long prop_index, long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	long previous = *(long *)((byte *)actor + 0x338);
	*(long *)((byte *)actor + 0x338) = prop_index;
	if (previous != NONE)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(previous);
		if (view) *(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, previous);
	}
	if (prop_index != NONE)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(prop_index);
		if (view) *(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, *(long *)((byte *)actor + 0x338));
	}
}

// @retail 0x267180
short function_267180(s_prop_datum *node)
{
	s_type_f95cd3 *view = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	long result;
	short state = node->state;
	if (!state)
		result = 0;
	else if (node->type != 1 && node->type != 6)
		result = 0;
	else if (function_25d690(node)->unknown5e)
		result = 1;
	else if (view->unknown2a)
		result = 12;
	else if (state >= 1 && state <= 2)
	{
		result = 6;
		if (node->unknown27 >= 1)
		{
			if (node->unknown28 < 1.0f)
				result = 11;
			else
			{
				if (node->unknown28 < 12.0f)
					result = 7;
				char level = view->unknown39;
				if (level <= 2)
				{
					result = 8;
					if (function_25d690(node)->unknown63)
					{
						result = 9;
						if (level <= 1)
							result = 10;
					}
				}
			}
		}
	}
	else if (state >= 3)
	{
		if (view->unknown69)
			result = 2;
		else if (view->unknown88)
			result = 3;
		else if ((g_510c54->game_time - view->unknown10) * g_510c54->rate < 1.0f && *(short *)view >= 5)
			result = 5;
		else
			result = 4;
	}
	if (view)
		*(short *)view = (short)result;
	return (short)result;
}

struct s_actor_prop_iterator
{
	long index;
	long next;
};

PRIVATE __forceinline s_prop_node_view *actor_next_prop(s_actor_prop_iterator *iterator)
{
	s_prop_node_view *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = prop_node_get(index);
		iterator->index = index;
		iterator->next = node->next_index;
	}
	return node;
}

// @retail 0x265bb0
void function_265bb0(long actor_index)
{
	s_actor_prop_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = actor_next_prop(&iterator)) != NULL)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(iterator.index);
		if (view)
		{
			view->unknown2a = false;
			*(short *)((byte *)view + 0x28) = NONE;
		}
	}
}

struct s_prop_thresholds
{
	byte unknown000[0x158];
	real actor_threshold;
	real object_threshold;
};

struct s_prop_threshold_table
{
	byte unknown000[0xc8];
	long count;
	s_prop_thresholds *entries;
};

long function_1e4990(long index);

long function_1e1f20(long actor_index);
long function_cbd50(long unit_index, short weapon_index);

struct s_object_activity_flags
{
	byte unknown00[0x10a];
	byte flag0 : 1;
	byte flag1 : 1;
	byte blocked : 1;
	byte unused : 5;
};

// @retail 0x267370
real function_267370(long volatile object_index)
{
	long const volatile *object_reference = &object_index;
	real result = 0.0f;
	long initial_object_index = *object_reference;
	if (initial_object_index != NONE)
	{
		s_slot_object_view *object = object_get(initial_object_index);
		s_actor_view *actor = NULL;
		byte *settings = NULL;
		if (((s_prop_threshold_table *)g_4e034c)->count > 0)
			settings = (byte *)((s_prop_threshold_table *)g_4e034c)->entries;
		if (!((bool)((s_object_activity_flags *)object)->blocked) && ((1 << object->type) & 3))
		{
			long actor_index = object->actor_index;
			if (actor_index != NONE)
				actor = actor_get(actor_index);
			long first;
			if (actor)
			{
				result = *(real *)((byte *)function_1e4990(actor->unknown054) + 8);
				first = function_1e1f20(actor_index);
			}
			else
			{
				first = function_cbd50(*object_reference, *(char *)((byte *)object + 0x212));
				if (object->player_index != NONE && settings)
					result = *(real *)(settings + 0x160);
			}
			long second = function_cbd50(*object_reference, *(char *)((byte *)object + 0x213));
			if (object->parent_index != NONE)
			{
				s_slot_object_view *parent = object_get(object->parent_index);
				if (parent->type == 1 && object->unknown1fc != NONE)
				{
					byte *tag = g_4e3b44[parent->tag_index & 0xffff].bytes;
					byte *seats = *(byte **)(tag + 0x1cc);
					result = *(real *)(seats + object->unknown1fc * 0xb0 + 0x38) + result;
				}
			}
			if (first != NONE)
				result = *(real *)(g_4e3b44[object_get(first)->tag_index & 0xffff].bytes + 0x238) + result;
			if (second != NONE)
				result = *(real *)(g_4e3b44[object_get(second)->tag_index & 0xffff].bytes + 0x238) + result;
			if (actor && actor->unknown225 && settings)
				result = *(real *)(settings + 0x164) + result;
		}
	}
	return result;
}

// @retail 0x267700
void function_267700(long actor_index, s_prop_datum *node, s_type_76cf92 *prop, s_type_f95cd3 *view)
{
	s_prop_datum *const *node_reference = &node;
	s_actor_view *actor = actor_get(actor_index);
	real value;
	if (prop->actor_index != NONE)
		value = *(real *)((byte *)actor_get(prop->actor_index) + 0x2cc);
	else
		value = function_267370((*node_reference)->object_index);
	value -= *(real *)((byte *)actor + 0x2cc);
	if (value < 0.0f)
		view->unknown60 = 0.0f;
	else
		view->unknown60 = value;
}

// @retail 0x267a80
short function_267a80(real *distance, point3f const *point, vector3f const *direction, point3f const *position, long unknown)
{
	long result = 0;
	vector3f *offset = (vector3f *)unknown;
	if (distance)
		*distance = 0.0f;
	if (offset)
		*offset = *g_4687a4;
	point2f planar = *(point2f const *)direction;
	real magnitude = (real)sqrt(planar.x * planar.x + planar.y * planar.y);
	if (fabs(magnitude) < 0.0001f)
		magnitude = 0.0f;
	else
	{
		real scale = 1.0f / magnitude;
		planar.x *= scale;
		planar.y = ((point2f volatile *)&planar)->y * scale;
	}
	if (magnitude > 0.0f)
	{
		vector3f delta;
		delta.i = position->x - point->x;
		delta.j = position->y - point->y;
		delta.k = position->z - point->z;
		real length = (real)sqrt(delta.i * delta.i + delta.j * delta.j);
		if (distance)
			*distance = length;
		real projection = delta.i * planar.x + delta.j * planar.y;
		if (projection > length * 0.8660253882408142f)
		{
			real scale = 0.0f - projection;
			delta.i = direction->i * scale + delta.i;
			delta.j = direction->j * scale + delta.j;
			delta.k = direction->k * scale + delta.k;
			if (offset)
			{
				offset->i = 0.0f - delta.i;
				offset->j = 0.0f - delta.j;
				offset->k = 0.0f - delta.k;
			}
			if (delta.k > -0.5f && delta.k < 0.9f)
				result = 2;
			else if (delta.k > -0.8f && delta.k < 1.2f)
				result = 1;
			else
				goto done;
			real squared = delta.i * delta.i + delta.j * delta.j;
			if (squared < 0.36000001430511475f)
				goto done;
			if (squared < 1.2100000381469727f)
			{
				result = 1;
				goto done;
			}
		}
	}
	result = 0;
done:
	return (short)result;
}

s_ai_player *ai_player_get(long player_index);
void function_cb7e0(long unit_index, vector3f *vector);
bool function_1e1e50(long actor_index, vector3f *direction);
void function_26be00(long actor_index, s_iterator *iterator);
real function_30bf0(vector3f *vector);

// @retail 0x267840
bool function_267840(long actor_index, long prop_index, vector3f *direction)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_datum *node = prop_ref_get(prop_index);
	s_type_76cf92 *prop = prop_get(node->prop_index);
	s_type_5cfb45 *state = function_25d690(node);
	volatile bool result = false;
	if (!prop->unknown22)
	{
		if (prop->unknown25)
		{
			long player_index = object_get(*(long *)((byte *)prop + 8))->player_index;
			s_ai_player *player;
			if (player_index != NONE && (player = ai_player_get(player_index)) != NULL)
			{
				if (state->unknown63)
					result = true;
				else
				{
					real ticks = (real)g_510c54->field_2_3 * 2.5f;
					long rounded;
					__asm { fld ticks }
					__asm { fistp rounded }
					result = g_510c54->game_time - player->unknown0c < rounded;
				}
				function_cb7e0(node->object_index, direction);
				if (!result && *(char *)((byte *)actor + 0x324) > 0)
				{
					s_iterator iterator;
					function_26be00(actor_index, &iterator);
					long index = iterator.next;
					while (index != NONE)
					{
						s_prop_datum *other = prop_ref_get(index);
						index = other->next_index;
						if (other->state >= 1 && prop_get(other->prop_index)->unknown23)
						{
							s_type_5cfb45 *other_state = function_25d690(other);
							vector3f delta;
							delta.i = other_state->position.x - state->position.x;
							delta.j = other_state->position.y - state->position.y;
							delta.k = other_state->position.z - state->position.z;
							if (function_30bf0(&delta) > 0.0f &&
								direction->i * delta.i + direction->j * delta.j + direction->k * delta.k > 0.5f)
							{
								result = true;
								break;
							}
						}
					}
				}
			}
		}
		else if (prop->actor_index != NONE)
			result = function_1e1e50(prop->actor_index, direction);
	}
	return result;
}

// @retail 0x267550
bool function_267550(long actor_index)
{
	byte *definition = (byte *)function_1e4990(actor_get(actor_index)->unknown054);
	bool result = false;
	if (definition && ((s_prop_threshold_table *)g_4e034c)->count > 0)
		result = *(real *)(definition + 8) >= ((s_prop_threshold_table *)g_4e034c)->entries->actor_threshold;
	return result;
}

struct s_equipped_object_view
{
	long tag_index;
	byte unknown004[0x212 - 4];
	char selected;
	byte unknown213[5];
	long objects[4];
};

/* The held-object lookup rereads the pool's data field. */

// @retail 0x2675f0
bool function_2675f0(long object_index)
{
	s_equipped_object_view *object = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[object_index & 0xffff].object;
	long held_index = NONE;
	short selected = object->selected;
	if (selected != NONE)
		held_index = object->objects[selected];
	bool result = false;
	if (held_index != NONE && ((s_prop_threshold_table *)g_4e034c)->count > 0)
	{
		s_prop_thresholds *thresholds = ((s_prop_threshold_table *)g_4e034c)->entries;
		s_equipped_object_view *held = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[held_index & 0xffff].object;
		byte *definition = g_4e3b44[held->tag_index & 0xffff].bytes;
		result = thresholds->object_threshold <= *(real *)(definition + 0x238);
	}
	return result;
}

// @retail 0x2684f0
short function_2684f0(s_actor_view *actor)
{
	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_view_fields_get(actor->prop_index);
		if (view)
			return view->unknown00;
	}
	return 0;
}

long function_1e4a10(long index);
short function_1a6fe0(long owner_index, short type);
struct s_node_view;
s_node_view *function_26be30(s_iterator *iterator);
void function_1f86a0(long actor_index);
void function_262800(long actor_index, s_reference reference, bool unknown);

PRIVATE __forceinline long round_tick_count(real ticks)
{
	long result;
	__asm { fld ticks }
	__asm { fistp result }
	return result;
}

// @retail 0x267c50
void function_267c50(long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	byte *data = (byte *)actor;
	*(real *)(data + 0x3d4) = 0.0f;
	short level = *(short *)(data + 0x328);
	s_prop_threshold_table *table = (s_prop_threshold_table *)g_4e034c;
	if (table && table->count > 0)
	{
		byte *values = (byte *)table->entries;
		if (level >= 12)
		{
			real threshold_a = 0.25f, threshold_b = 0.25f;
			byte *settings = (byte *)function_1e4a10(actor->unknown054);
			if (settings)
			{
				if (*(real *)(settings + 0x60) > 0.0f) threshold_a = *(real *)(settings + 0x60);
				if (*(real *)(settings + 0x5c) > 0.0f) threshold_b = *(real *)(settings + 0x5c);
			}
			if (*(real *)(data + 0x2d8) > threshold_a) *(real *)(data + 0x3d4) = *(real *)(values + 0x2c);
			else if (*(real *)(data + 0x2d8) > 0.0f) *(real *)(data + 0x3d4) = *(real *)(values + 0x28);
			else if (*(real *)(data + 0x2dc) > threshold_b) *(real *)(data + 0x3d4) = *(real *)(values + 0x24);
			else *(real *)(data + 0x3d4) = *(real *)(values + 0x20);
		}
		else if (level >= 11) *(real *)(data + 0x3d4) = *(real *)(values + 0x18);
		else if (level >= 10) *(real *)(data + 0x3d4) = *(real *)(values + 0x10);
		else if (level >= 9) *(real *)(data + 0x3d4) = *(real *)(values + 8);
		else if (level >= 8) *(real *)(data + 0x3d4) = *(real *)values;
		else *(real *)(data + 0x3d4) = 0.0f;
	}
	long ticks;
	if (*(volatile real *)(data + 0x3d4) > *(volatile real *)(data + 0x3d8))
		ticks = round_tick_count((real)*(volatile short *)&g_510c54->field_2_3 * 0.25f);
	else
		ticks = round_tick_count((real)*(volatile short *)&g_510c54->field_2_3);
	real difference = *(volatile real *)(data + 0x3d4) - *(volatile real *)(data + 0x3d8);
	volatile real decay = expf(-0.6931471824645996f / ticks);
	real blend = 1.0f - decay;
	*(real *)(data + 0x3d8) += difference * blend;
	if (actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1) &&
		*(long *)(data + 0x26c) == NONE && !*(bool *)(data + 0x225) && actor->unknown858 == NONE &&
		function_1a6fe0(actor_index, 0x38) == NONE && function_1a6fe0(actor_index, 0x10) == NONE &&
		function_1a6fe0(actor_index, 0x11) == NONE)
	{
		bool moving_toward = false, nearby_player = false;
		bool check_target = function_2684f0(actor) > 6;
		vector3f target_direction;
		if (check_target)
		{
			s_type_f95cd3 *view = function_25d700(*(long *)(data + 0x338));
			if (view) target_direction = *(vector3f *)((byte *)view + 0x2c);
		}
		if (actor->unknown086 > 5)
		{
			s_iterator iterator;
			function_26be00(*actor_reference, &iterator);
			s_prop_datum *node;
			while ((node = (s_prop_datum *)function_26be30(&iterator)) != NULL)
			{
				s_type_5cfb45 *state = function_25d690(node);
				s_type_76cf92 *prop = prop_get(node->prop_index);
				if (!prop->unknown23 && !function_25d690(node)->unknown5e && !prop->unknown22 &&
					(prop->unknown25 || state->unknown3c == NONE))
				{
					vector3f direction;
					if (function_267840(*actor_reference, iterator.index, &direction))
					{
						bool active = false;
						vector3f offset = {0.0f, 0.0f, 0.0f};
						short proximity = function_267a80(NULL, &state->position, &direction,
							(point3f *)(data + 0x238), (long)&offset);
						if (proximity >= 1)
						{
							*(bool *)(data + 0x3de) = true;
							if (prop_get(node->prop_index)->unknown25)
							{
								*(bool *)(data + 0x3dc) = true;
								nearby_player = true;
							}
						}
						if (state->unknown63) active = true;
						else
						{
							long player_index = object_get(node->object_index)->player_index;
							s_ai_player *player;
							if (player_index != NONE && (player = ai_player_get(player_index)) != NULL)
								active = g_510c54->game_time - player->unknown0c < round_tick_count((real)g_510c54->field_2_3 * 2.5f);
						}
						if (prop_get(node->prop_index)->unknown25 && active)
						{
							real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
							if (distance_squared < 1.0f && *(bool *)(data + 0x5d0))
							{
								vector3f movement = *(vector3f *)(data + 0x5ec);
								if (function_30bf0(&movement) > 0.0f)
								{
									point3f ahead;
									ahead.x = movement.i * 0.4f + *(real *)(data + 0x238);
									ahead.y = movement.j * 0.4f + *(real *)(data + 0x23c);
									ahead.z = movement.k * 0.4f + *(real *)(data + 0x240);
									short projected = function_267a80(NULL, &state->position, &direction, &ahead, 0);
									if ((projected > proximity ? projected : proximity) >= 1)
									{
										real dot = movement.k * offset.k + movement.j * offset.j + movement.i * offset.i;
										if (dot > (distance_squared < 0.25f ? 0.0f : 0.8660253882408142f))
										{
											*(bool *)(data + 0x3dd) = true;
											moving_toward = true;
										}
									}
								}
							}
						}
					}
					if (check_target && function_267a80(NULL, (point3f *)(data + 0x238), &target_direction, &state->position, 0) >= 2)
						*(bool *)(data + 0x3df) = true;
				}
			}
		}
		if (moving_toward)
		{
			function_262800(*actor_reference, *(s_reference *)(data + 0x418), true);
			function_1f86a0(actor_index);
			*(short *)(data + 0x3e2) = (short)round_tick_count((real)g_510c54->field_2_3);
		}
		else if (*(short *)(data + 0x3e2) > 0)
			--*(short *)(data + 0x3e2);
		else
			*(bool *)(data + 0x3dd) = false;
		if (nearby_player || moving_toward)
			*(short *)(data + 0x3e0) = (short)round_tick_count((real)g_510c54->field_2_3 * 1.5f);
		else if (*(short *)(data + 0x3e0) > 0)
			--*(short *)(data + 0x3e0);
		else
		{
			*(bool *)(data + 0x3dc) = false;
			*(bool *)(data + 0x3dd) = false;
			*(bool *)(data + 0x3de) = false;
			*(bool *)(data + 0x3df) = false;
		}
	}
	else
	{
		*(bool *)(data + 0x3de) = false;
		*(bool *)(data + 0x3dc) = false;
		*(bool *)(data + 0x3df) = false;
		*(bool *)(data + 0x3dd) = false;
		*(short *)(data + 0x3e2) = 0;
		*(short *)(data + 0x3e0) = 0;
	}
	if (actor->unknown086 >= 2) actor->unknown084 = 4;
	else actor->unknown084 = 1 + 2 * (actor->unknown086 >= 1);
}

// @retail 0x2675b0
bool function_2675b0(long node_index)
{
	s_prop_node_view *node = prop_node_get(node_index);
	s_type_76cf92 *prop = prop_get(node->unknown08);
	bool result = false;
	if (prop->actor_index != NONE)
		result = function_267550(prop->actor_index);
	return result;
}

/* These wrappers read the handle after resolving the complete record address. */
// @retail 0x267680
bool function_267680(long node_index)
{
	s_prop_node_view volatile *node = prop_node_get(node_index);
	long object_index = node->object_index;
	long type = ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].type;
	bool result = false;
	if ((1 << type) & 3)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2676d0
bool function_2676d0(long actor_index)
{
	s_actor_view volatile *actor = actor_get(actor_index);
	long object_index = actor->unknown018;
	bool result = false;
	if (object_index != NONE)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2672e0
void function_2672e0(s_type_f95cd3 *view, s_prop_node_view const *node)
{
	s_prop_node_view const *const *node_reference = &node;
	if (view->unknown54 > 1.0f)
		view->unknown54 = 1.0f;
	real value = 1.0f;
	if (!view->unknown2a)
	{
		if ((*node_reference)->unknown27 < 2 || view->unknown39 == 4)
			value = 0.0f;
		else if (view->unknown39 == 3)
			value = 0.3f;
		else if (view->unknown39 == 2)
			value = 0.6f;
		else
			value = 0.8f;
	}
	if (!(value >= view->unknown54))
		value = value * (1.0f - 0.995f) + view->unknown54 * 0.995f;
	view->unknown54 = value;
}


PRIVATE inline s_type_f95cd3 *countdown_prop_view(s_prop_datum *node)
{
	s_type_f95cd3 *result = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking)
			result = &tracking->view;
	}
	return result;
}

// @retail 0x263670
void function_263670(s_prop_datum *node)
{
	short *countdown = (short *)node->unknown1e;
	if (*countdown > 0)
	{
		--*countdown;
		if (*countdown == 0)
			node->unknown1c = NONE;
	}
	s_type_5cfb45 *state = function_25d690(node);
	s_type_f95cd3 *view = countdown_prop_view(node);
	if (state->unknown5e)
		++*(short *)((byte *)state + 0x5c);
	else
		*(short *)((byte *)state + 0x5c) = 0;
	if (view)
	{
		short *age = (short *)((byte *)view + 0x28);
		if (*age != NONE)
		{
			++*age;
			if ((real)*age * g_510c54->rate >= 1.5f)
			{
				view->unknown2a = false;
				*age = NONE;
			}
		}
		short *remaining = (short *)view->unknowna0;
		if (*remaining > 0)
			--*remaining;
		short *duration = (short *)view->unknown08;
		if (node->unknown27 >= 1)
		{
			if (*duration < 0x7fff)
				++*duration;
		}
		else
			*duration = 0;
	}
}


struct s_prop_object_links_view
{
	byte unknown00[0x10a];
	word unknown10a_0 : 2;
	word has_links : 1;
	word unknown10a_3 : 13;
	byte unknown10c[0x120 - 0x10c];
	short size;
	short offset;
};

struct s_prop_object_link
{
	byte unknown00[4];
	word packed_index;
	byte unknown06[2];
};

/* The optional output pointer stays on the stack until the scan is complete. */
// @retail 0x2651e0
bool function_2651e0(long object_index, short *volatile output_index)
{
	s_prop_object_links_view *object = (s_prop_object_links_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	bool result = false;
	short index = NONE;
	if (TEST_FIELD_BIT(object->has_links))
	{
		long count = (dword)(long)object->size >> 3;
		s_prop_object_link *links = (s_prop_object_link *)((byte *)object + object->offset);
		index = 0x7fff;
		for (short i = 0; i < count; i++)
		{
			dword packed = links[i].packed_index;
			if ((packed & 0xfff8) > 0)
			{
				word candidate = links[i].packed_index;
				if ((word)(candidate >> 3) < (word)index)
				{
					index = candidate >> 3;
					result = true;
				}
				break;
			}
		}
	}
	short *output = output_index;
	if (output)
	{
		if (result)
			*output = index;
		else
			*output = NONE;
	}
	return result;
}


// @retail 0x264260
long function_264260(vector3f const *direction, vector3f const *facing, real distance)
{
	real cosine = 0.0f - (direction->k * facing->k + direction->j * facing->j + direction->i * facing->i);
	real lateral;
	if (cosine <= 0.0f)
		lateral = 3.402823466e+38F;
	else if (cosine >= 1.0f)
		lateral = 0.0f;
	else
		lateral = (real)(sqrt(1.0f - cosine * cosine) * distance);

	if (cosine > 0.9925f || lateral < 0.5f)
		return 0;
	if (cosine > 0.9063f || lateral < 1.5f)
		return 1;
	if (cosine > 0.5f)
		return 2;
	if (cosine > 0.0f)
		return 3;
	return 4;
}


void *function_1e51a0(long actor_index);

short const g_44ae58[4][4] =
{
	{0, 0, 1, 3},
	{0, 1, 2, 3},
	{0, 2, 3, 4},
	{0, 3, 4, 4}
};

struct s_prop_rate_definition
{
	byte unknown00[0x24];
	real durations[3];
};

// @retail 0x2683f0
real function_2683f0(long actor_index, long node_index, short type)
{
	real result = 0.0f;
	s_prop_rate_definition *definition = (s_prop_rate_definition *)function_1e51a0(actor_index);
	if (definition)
	{
		s_prop_datum *node = prop_ref_get(node_index);
		switch (node->type)
		{
		case 2:
			result = 1.0f;
			break;
		case 3:
			result = 1.0f;
			break;
		case 4:
			result = 1.0f;
			break;
		case 8:
			result = 1.0f;
			break;
		case 1:
			if (function_25d690(node)->unknown3c != NONE)
			{
				result = 1.0f;
				break;
			}
		case 5:
		case 6:
		case 7:
			switch (g_44ae58[(short)function_263d30(actor_index)][type])
			{
			case 0:
				break;
			case 1:
				result = definition->durations[0] > 0.0f ? g_510c54->rate / definition->durations[0] : 1.0f;
				break;
			case 2:
				result = definition->durations[1] > 0.0f ? g_510c54->rate / definition->durations[1] : 1.0f;
				break;
			case 3:
				result = definition->durations[2] > 0.0f ? g_510c54->rate / definition->durations[2] : 1.0f;
				break;
			case 4:
				result = 1.0f;
				break;
			default:
				__assume(0);
			}
			break;
		}
	}
	return result;
}

// @retail 0x266640
void function_266640(long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	byte *data = (byte *)actor;
	long best_node = NONE, strongest_node = NONE;
	short best_level = 0;
	real strongest = 0.0f;
	*(long *)(data + 0x324) = 0;
	*(long *)(data + 0x328) = 0;
	*(long *)(data + 0x32c) = 0;
	*(long *)(data + 0x330) = 0;
	long next = actor_get(actor_index)->first_prop_index;
	while (next != NONE)
	{
		long index = next;
		s_prop_datum *node = prop_ref_get(index);
		next = *(long *)((byte *)node + 0x2c);
		s_type_f95cd3 *view = NULL;
		if (node->tracking_index != NONE)
		{
			s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
			if (tracking) view = &tracking->view;
		}
		s_type_5cfb45 *state = function_25d690(node);
		s_type_76cf92 *prop = prop_get(node->prop_index);
		if (node->state >= 1)
		{
			if (!view) continue;
			if (!function_25d690(node)->unknown5e)
			{
				short level = function_267180(node);
				if (level > best_level)
				{
					best_level = level;
					best_node = index;
				}
				if (prop_get(node->prop_index)->unknown23)
				{
					function_2672e0(view, (s_prop_node_view *)node);
					function_267700(*actor_reference, node, prop, view);
					if (node->unknown27 >= 1) ++*(char *)(data + 0x325);
					++*(char *)(data + 0x324);
				}
				else if (node->type == 2)
				{
					long other_index = object_get(node->object_index)->actor_index;
					s_actor_view *other = other_index == NONE ? NULL : actor_get(other_index);
					bool close = node->unknown28 < 8.0f;
					if (!close && *(bool *)((byte *)state + 0x62) && other &&
						*(long *)(data + 0x338) != NONE && *(long *)((byte *)other + 0x338) != NONE)
						close = prop_ref_get(*(long *)(data + 0x338))->object_index == prop_ref_get(*(long *)((byte *)other + 0x338))->object_index;
					bool very_close = (view->unknown06 == 0 || view->unknown06 == 1) && node->unknown28 < 3.0f;
					if (close) ++*(char *)(data + 0x326);
					if (very_close) ++*(char *)(data + 0x327);
				}
			}
		}
		if (view && *(real *)((byte *)view + 0x3c) > strongest)
		{
			strongest = *(real *)((byte *)view + 0x3c);
			strongest_node = index;
		}
	}
	*(short *)(data + 0x328) = best_level;
	*(long *)(data + 0x32c) = best_node;
	if (strongest_node != *(long *)(data + 0x338)) function_267770(strongest_node, *actor_reference);
	function_266540(*actor_reference);
	*(bool *)(data + 0x340) = false;
	function_2662f0(*actor_reference);
	if (*(bool *)(data + 0x30c) && *(short *)(data + 0x310) >= 0 &&
		*(short *)(data + 0x310) < 2 && actor->unknown086 < 8)
	{
		s_ai_player *player = &g_4f55cc[*(short *)(data + 0x310)];
		if (player->unknown0c <= 0 || g_510c54->game_time - player->unknown0c > g_510c54->field_2_3 * 3)
		{
			short first = g_510c54->field_2_3 * 3;
			short second = g_510c54->field_2_3 * 15;
			real *settings = (real *)function_1e4a90(*(long *)(data + 0x54));
			if (settings)
			{
				first = (short)function_1469f0(settings[0]);
				second = (short)function_1469f0(settings[1]);
			}
			short elapsed = ++*(short *)(data + 0x30e);
			if (elapsed == first)
			{
				if (player->player_index != NONE)
					function_1fb7e0(0xa8, *actor_reference, NULL, *(long *)(g_4e8c24->data + (g_4f55cc[*(short *)(data + 0x310)].player_index & 0xffff) * 0x21c + 0x2c), NONE);
			}
			else if (elapsed == second && player->player_index != NONE)
				function_1fb7e0(0xa9, *actor_reference, NULL, *(long *)(g_4e8c24->data + (g_4f55cc[*(short *)(data + 0x310)].player_index & 0xffff) * 0x21c + 0x2c), NONE);
		}
		else *(short *)(data + 0x30e) = 0;
	}
	else *(short *)(data + 0x30e) = 0;
	if (actor->unknown26c == NONE || actor->unknown018 == NONE ||
		(actor->unknown024 != 1 && team_is_enemy(actor->unknown024, 1)))
	{
		actor->unknown31e = 0;
		*(short *)(data + 0x320) = 0;
		actor->unknown31c = NONE;
		return;
	}
	s_slot_object_view *vehicle = object_get(actor->unknown26c);
	short vehicle_type = *(short *)(g_4e3b44[vehicle->tag_index & 0xffff].bytes + 0x1f0);
	if (vehicle_type != 0 && vehicle_type != 1 && vehicle_type != 4)
	{
		actor->unknown31e = 0;
		*(short *)(data + 0x320) = 0;
		actor->unknown31c = NONE;
		return;
	}
	s_data_datum_iterator players;
	players.data = g_4e8c24;
	players.index = NONE;
	players.datum_index = NONE;
	while (data_datum_iterator_next(&players))
	{
		s_ai_player *player = NULL;
		for (long p = 0; p < 2; p++)
			if (g_4f55cc[p].player_index == players.datum_index) { player = &g_4f55cc[p]; break; }
		short player_slot = NONE;
		for (long i = 0; i < 2; i++)
			if (g_4f55cc[i].player_index == players.datum_index) { player_slot = (short)i; break; }
		long threshold = round_tick_count((real)g_510c54->field_2_3 * 2.0f);
		if (player && *(long *)(players.datum + 0x2c) != NONE)
		{
			long unit_index = *(long *)(players.datum + 0x2c);
			if (actor->unknown31c != NONE && actor->unknown31c != player_slot) continue;
			s_slot_object_view *unit = object_get(unit_index);
			if (*(short *)((byte *)unit + 0x1fc) == NONE)
			{
				vector3f forward = *(vector3f *)((byte *)unit + 0x70);
				if (unit->parent_index != NONE)
				{
					byte *parent = (byte *)object_get(unit->parent_index);
					real *matrix = (real *)(parent + *(short *)(parent + 0x116) + *(char *)((byte *)unit + 0x18) * 0x34);
					vector3f transformed;
					transformed.i = matrix[7] * forward.k + matrix[4] * forward.j + matrix[1] * forward.i;
					transformed.j = matrix[8] * forward.k + matrix[5] * forward.j + matrix[2] * forward.i;
					transformed.k = matrix[9] * forward.k + matrix[6] * forward.j + matrix[3] * forward.i;
					forward = transformed;
				}
				point3f point;
				function_b9dd0(unit_index, &point);
				vector3f direction;
				direction.i = actor->position.x - point.x;
				direction.j = actor->position.y - point.y;
				direction.k = actor->position.z - point.z;
				real distance = function_30bf0(&direction);
				bool established = actor->unknown31c == player_slot && actor->unknown31e > (short)threshold;
				if (!(distance > 0.0f)) continue;
				long reference = function_25d810(unit_index, *actor_reference, false);
				vector3f velocity = unit->velocity;
				vector3f relative;
				relative.i = velocity.i - vehicle->velocity.i;
				relative.j = velocity.j - vehicle->velocity.j;
				relative.k = velocity.k - vehicle->velocity.k;
				real approach = 0.0f;
				if (function_30bf0(&velocity) > 0.0f)
					approach = velocity.k * direction.k + velocity.j * direction.j + velocity.i * direction.i;
				if (function_30bf0(&relative) > 0.0f)
				{
					real value = relative.k * direction.k + relative.j * direction.j + relative.i * direction.i;
					if (value > approach) approach = value;
				}
				bool visible = reference == NONE || prop_ref_get(reference)->unknown27 >= 1;
				real facing = 0.0f, score = 0.0f;
				bool positive = false;
				if (distance < 15.0f)
				{
					if (visible)
					{
						facing = direction.k * forward.k + direction.j * forward.j + direction.i * forward.i;
						if (facing > 0.8660253882408142f) score = (facing - 0.8660253882408142f) * 7.4641008377075195f;
						else if (distance < 1.0f) score = 1.0f;
					}
					if (established)
					{
						if (distance < 2.5f) score += 1.0f;
						else if (distance <= 3.5f)
						{
							real decrease = distance - 2.5f;
							if (decrease < 0.0f) decrease = 0.0f;
							score += 1.0f - decrease;
						}
						if (approach > 0.5f) score += 1.0f;
						positive = score > 0.0f;
					}
					else if (visible && !(distance > 3.5f && approach < 0.5f)) positive = score > 0.0f;
				}
				bool retain = true;
				if (positive)
				{
					if (actor->unknown31c != NONE)
					{
						if (actor->unknown31c != player_slot) continue;
						if (*(short *)(data + 0x320) > 0) *(short *)(data + 0x320) -= 2;
					}
					else { actor->unknown31e = 0; *(short *)(data + 0x320) = 0; }
					actor->unknown31c = player_slot;
				}
				else
				{
					if (actor->unknown31c != player_slot) continue;
					++*(short *)(data + 0x320);
					if (approach < 0.0f) ++*(short *)(data + 0x320);
					if (*(short *)(data + 0x320) > round_tick_count((real)g_510c54->field_2_3 * 4.0f) || actor->unknown31e < (short)threshold) retain = false;
				}
				if (actor->unknown31c == player_slot)
				{
					++actor->unknown31e;
					if (facing > 0.8660253882408142f && distance < 2.5f) ++actor->unknown31e;
				}
				if (retain) continue;
			}
		}
		if (actor->unknown31c == player_slot)
		{
			actor->unknown31c = NONE;
			actor->unknown31e = 0;
			*(short *)(data + 0x320) = 0;
		}
	}
}

struct s_actor_object_sample;
bool function_28fa60(long perception_index, point3f const *point, s_actor_object_sample *sample);

__declspec(noinline) bool __stdcall function_2641c0(long actor_index, s_2641c0 *sample, point3f const *point);

// @retail 0x2641c0
bool __stdcall function_2641c0(long actor_index, s_2641c0 *sample, point3f const *point)
{
    point3f const *const *point_reference = &point;
    s_actor_view *actor = actor_get(actor_index);
    if (*((byte *)actor + 7) && *(long *)((byte *)actor + 0x1c) != NONE)
        return function_28fa60(*(long *)((byte *)actor + 0x1c), *point_reference, (s_actor_object_sample *)sample);
    memcpy(sample, (byte *)actor + 0x22c, sizeof(*sample));
    return true;
}

// @retail 0x264210
void function_264210(long actor_index, long prop_ref_index)
{
    s_prop_datum *reference = (s_prop_datum *)(g_502418->data + (prop_ref_index & 0xffff) * 0x3c);
    s_2641c0 context;
    if (function_2641c0(actor_index, &context, &function_25d690(reference)->position))
        function_264330(actor_index, prop_ref_index, &context, 0, true);
}
