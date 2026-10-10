// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_25FC30.CPP: firing position evaluators (firing_position_group) */

#include "unknown_11c920.h"
#include "unknown_25fc30.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_0d0690.h"
#include "unknown_1e1f20.h"
#include "unknown_0259a0.h"

typedef bool (__stdcall *firing_position_evaluate_proc)(long, s_type_967e20 *, s_type_b36ac5 *);
typedef void (__stdcall *firing_position_pre_evaluate_proc)(long, s_type_967e20 *, short, s_type_b36ac5 *);

struct firing_position_pre_evaluator
{
	short flags;
	firing_position_pre_evaluate_proc proc;
};

struct firing_position_post_evaluator
{
	short flags;
	firing_position_evaluate_proc proc;
};

s_type_b36ac5 *g_51eca0;
extern firing_position_pre_evaluator g_44ad90[12];
extern firing_position_post_evaluator g_44adf0[12];

void __stdcall function_25dd50(long actor_index, s_type_967e20 *context, short count, s_type_b36ac5 *positions);
void __stdcall function_25eea0(long actor_index, s_type_967e20 *context, short count, s_type_b36ac5 *positions);
void __stdcall function_25f290(long actor_index, s_type_967e20 *context, short count, s_type_b36ac5 *positions);
void __stdcall function_25e430(long actor_index, s_type_967e20 *context, short count, s_type_b36ac5 *positions);
bool __stdcall function_25fb60(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);

/* the actor fields the firing position evaluators read (the actor of
   g_4f55f0) */
struct s_actor_firing_view
{
	byte unknown000[0x18];
	long unknown018;
	byte unknown01c[0x24 - 0x1c];
	short unknown024;
	byte unknown026[0x30 - 0x26];
	long unknown030;
	byte unknown034[0x54 - 0x34];
	long unknown054;
	byte unknown058[0x238 - 0x58];
	point3f position;
	byte unknown244[0x26c - 0x244];
	long unknown26c;
	byte unknown270[0x36c - 0x270];
	real unknown36c;
	point3f unknown370;
	byte unknown37c[0x388 - 0x37c];
	point3f unknown388;
	real unknown394;
	real unknown398;
	real unknown39c;
	point3f unknown3a0;
	byte unknown3ac[0x888 - 0x3ac];
};

bool function_1b6070(long index, short unknown0, short unknown2);
bool function_262890(long actor_index, s_reference reference);
void *function_272a00(long actor_index);
void *function_1e5240(long actor_index);
short function_267a80(real *distance, point3f const *point, vector3f const *direction, point3f const *position, long unknown);
void function_1b0710(long object_index, long actor_index, real *distance_squared, real *maximum_distance_squared);

/* the squads (g_51e9d8, 0x98 bytes): the zone they hold and their leader */
struct s_squad_firing_view
{
	byte unknown00[0x2a];
	short zone_index;
	byte unknown2c[0x80 - 0x2c];
	long leader_index;
	byte unknown84[0x98 - 0x84];
};

/* the scenario's zones (g_4e0350 +0x244, 0x7c bytes) */
struct s_firing_zone_view
{
	byte unknown00[0x24];
	dword flags;
	byte unknown28[0x4e - 0x28];
	short unknown4e;
	byte unknown50[0x7c - 0x50];
};

struct s_scenario_zone_view
{
	byte unknown000[0x244];
	s_firing_zone_view *zones;
};

/* the unit's team (+0x138) */
struct s_unit_team_view
{
	byte unknown000[0x138];
	short team;
};

/* a character tag's cover behaviour */
struct s_character_cover
{
	dword flags;
};

struct s_character_cover_view
{
	byte unknown00[8];
	long parent_tag_index;
	byte unknown0c[0x5c - 0xc];
	long cover_count;
	s_character_cover *cover;
};

extern s_record_pool *g_51e9d8;

/* the structure bsp's pathfinding data (g_4e0348 +0xc4) */
struct s_bsp_pathfinding_view
{
	byte unknown000[0xc4];
	long pathfinding_count;
	s_pathfinding_data *pathfinding;
};

/* the faces of the pathfinding data (+0xc, 16 bytes each) */
struct s_pathfinding_face_view
{
	byte unknown0[5];
	byte flags;
	byte unknown6[0x10 - 0x6];
};

struct s_pathfinding_faces_view
{
	byte unknown00[0xc];
	s_pathfinding_face_view *faces;
};

/* the covers a context holds (+0x64, 0x70 bytes each) */
struct s_firing_cover_view
{
	byte unknown00[0x54];
	vector3f normal;
	point3f point;
	byte unknown6c[0x70 - 0x6c];
};

bool function_29d7b0(s_pathfinding_data *pathfinding, s_type_d4fbfa *definition, vector3f const *direction,
	point3f *point, vector3f *normal, char *side);

/* the unit's actor (+0x12c) */
struct s_unit_actor_view
{
	byte unknown000[0x12c];
	long actor_index;
};

/* what function_272a00 returns: the firing style of the actor's squad */
struct s_firing_style_view
{
	byte unknown00[0x24];
	short unknown24;
	byte unknown26[0x31 - 0x26];
	char unknown31;
};
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);
real function_11e130(point3f const *a, vector3f const *da, point3f const *b, vector3f const *db);
point3f *function_b9dd0(long object_index, point3f *result);
bool function_29e050(byte *unknown, long target_index, s_type_d4fbfa *definition, s_reference reference, long *unknown6a0);

// @retail 0x25dd20
long __stdcall function_25dd20(long key)
{
	return (key & 0xff) << 2;
}

// @retail 0x25dd30
bool __stdcall function_25dd30(long a, long b)
{
	return a == b;
}

// @retail 0x25dd50
void __stdcall function_25dd50(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	s_actor_firing_view *actor = (s_actor_firing_view *)actor_get(actor_index);
	short i;

	for (i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (!position->unknown4c)
		{
			continue;
		}

		if (function_262890(actor_index, position->reference))
		{
			position->unknown4d = true;
			if (!context->unknown14)
			{
				position->unknown4c = false;
				continue;
			}
		}

		if (context->unknown51)
		{
			vector3f segment;
			vector3f offset;
			real reach = actor->unknown39c + 2.5f;

			vector3d_from_points3d(&actor->unknown370, &actor->unknown388, &segment);
			vector3d_from_points3d(&actor->unknown3a0, &position->position, &offset);
			if (reach * reach > length_sq3f(&offset))
			{
				real distance;
				real bonus = 0.f;

				if ((real)sqrt(length_sq3f(&segment)) > 0.f)
				{
					distance = function_11e000(&actor->unknown370, &position->position, &segment);
				}
				else
				{
					vector3f delta;

					vector3d_from_points3d(&position->position, &actor->unknown370, &delta);
					distance = (real)sqrt(length_sq3f(&delta));
				}

				if (actor->unknown36c * actor->unknown36c > distance)
				{
					position->unknown4d = true;
					if (!context->unknown14)
					{
						position->unknown4c = false;
						continue;
					}
				}
				else if ((actor->unknown36c + 2.5f) * (actor->unknown36c + 2.5f) > distance)
				{
					bonus = ((real)sqrt(distance) - actor->unknown36c) * 8.f;
				}
				else
				{
					bonus = 20.f;
				}
				position->score += bonus;
			}

			reach = actor->unknown39c + 3.f;
			if (reach * reach > actor->unknown398 && actor->unknown394 > actor->unknown36c)
			{
				real radius = actor->unknown36c;

				if (function_11e000(&actor->unknown370, &actor->position, &segment) > radius * radius)
				{
					vector3f movement;

					movement.i = position->unknown1c.i * 3.f;
					movement.j = position->unknown1c.j * 3.f;
					movement.k = position->unknown1c.k * 3.f;
					if (length_sq3f(&movement) > 0.0001f)
					{
						radius = actor->unknown36c;
						if (radius * radius > function_11e130(&actor->unknown370, &segment, &actor->position, &movement))
						{
							position->unknown4d = true;
							if (!context->unknown14)
							{
								position->unknown4c = false;
								continue;
							}
						}
					}
				}
			}
		}

		if (context->sphere_count > 0)
		{
			real nearest = 1.f;
			real bonus = 10.f;

			for (short j = 0; j < context->sphere_count; j++)
			{
				real ratio = 3.4028235e38f;
				vector3f delta;

				vector3d_from_points3d(&position->position, &context->spheres[j].center, &delta);
				if (context->spheres[j].radius > 0.0)
				{
					ratio = length_sq3f(&delta) / (context->spheres[j].radius * context->spheres[j].radius);
				}
				if (nearest > ratio)
				{
					nearest = ratio;
				}
			}

			if (1.f > nearest)
			{
				bonus = (real)sqrt(nearest) * 10.f;
			}
			position->score += bonus;
		}
	}

	for (i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (position->unknown4c)
		{
			if (context->unknown698 &&
				(context->unknown69a & position->definition->flags0e) == position->definition->flags0e)
			{
				position->score += 4.f;
			}
			if (position->unknown5a && actor->unknown26c == NONE)
			{
				position->score += 4.f;
			}
		}
	}

	if (context->unknown5b)
	{
		s_slot_object_view *object = object_get(actor->unknown26c != NONE ? actor->unknown26c : actor->unknown018);
		point3f origin;

		function_b9dd0(actor->unknown018, &origin);
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c)
			{
				vector3f delta;
				real distance_squared;

				vector3d_from_points3d(&origin, &position->position, &delta);
				distance_squared = length_sq3f(&delta);
				if (fabs(distance_squared) >= 0.0001f && 900.f > distance_squared)
				{
					real cosine = dot3f(&delta, &object->forward) / (real)sqrt(distance_squared);

					if ((context->unknown5c || length_sq3f(&object->velocity) > 6.25f) &&
						64.f > distance_squared && cosine > 0.70710677f)
					{
						position->unknown4d = true;
						if (!context->unknown14)
						{
							position->unknown4c = false;
							continue;
						}
					}

					real scale = context->unknown60 > 0.f ? context->unknown60 * 0.5f : 5.f;
					real bonus;

					if (0.f > cosine)
					{
						bonus = scale - scale * cosine * -1.f;
					}
					else if (cosine > 0.8660254f)
					{
						bonus = ((cosine - 0.8660254f) * 7.4641008f + 1.f) * scale;
					}
					else
					{
						bonus = scale;
					}
					position->score += bonus;
				}
			}
		}
	}

	if (context->unknown692)
	{
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (*(long *)&position->reference == context->unknown694 && position->unknown4c)
			{
				position->score += 10.f;
			}
		}
	}
}

#pragma inline_depth(0)
// @retail 0x25e430
void __stdcall function_25e430(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	if (!context->unknown55)
	{
		return;
	}

	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];
		s_type_d4fbfa *definition = position->definition;
		point3f point;
		vector3f normal;
		char side;

		if (!position->unknown4c || !(definition->flags & 0x10))
		{
			continue;
		}

		if (!(position->reference.unknown2 & 0x8000))
		{
			s_bsp_pathfinding_view *bsp = (s_bsp_pathfinding_view *)g_4e0348;
			s_pathfinding_data *pathfinding;
			vector3f direction;
			s_path_trace_result trace;

			if (bsp->pathfinding_count <= 0 || (pathfinding = bsp->pathfinding) == NULL)
			{
				continue;
			}

			direction.i = position->unknown40.i * -1.f;
			direction.j = position->unknown40.j * -1.f;
			direction.k = position->unknown40.k * -1.f;
			if (!function_26c590(definition->unknown14, (point3f *)definition, &trace, pathfinding,
					(point3f *)definition, NONE, &direction, 0.35f, 0) ||
				!*(bool *)&trace.unknown00 || *(long *)&trace.unknown10[4] == 0xffff ||
				!(((s_pathfinding_faces_view *)pathfinding)->faces[*(long *)&trace.unknown10[4]].flags & 8) ||
				!function_29d7b0(pathfinding, definition, &direction, &point, &normal, &side) ||
				!side)
			{
				continue;
			}
		}
		else
		{
			s_firing_cover_view *covers = (s_firing_cover_view *)context->unknown64;

			if (!covers)
			{
				continue;
			}

			s_firing_cover_view *cover = &covers[position->reference.unknown0];

			normal = cover->normal;
			if (dot3f(&position->unknown40, &normal) > 0.70710677f)
			{
				if (!function_210690(definition->unknown0c, &cover->point, &point) ||
					!function_210770(definition->unknown0c, &normal, &normal))
				{
					continue;
				}
				side = (char)position->unknown5c;
				if (!side)
				{
					continue;
				}
			}
			else
			{
				position->unknown5c = 0;
				continue;
			}
		}

		position->unknown5c = side;
		position->unknown60 = point;
		position->unknown6c = normal;
		position->score += 4.f;
		if (!(position->reference.unknown2 & 0x8000) && context->unknown618 &&
			(*(short *)context == 0 || *(short *)context == 3) && (side == 1 || side == 2))
		{
			point3f target;

			target.x = point.x - normal.i * 0.15f;
			target.y = point.y - normal.j * 0.15f;
			target.z = point.z - normal.k * 0.15f;
			switch (side)
			{
			case 1:
				target.x -= (0.f - normal.j) * 0.25f;
				target.y -= normal.i * 0.25f;
				break;
			case 2:
				target.x += (0.f - normal.j) * 0.25f;
				target.y += normal.i * 0.25f;
				break;
			}
			function_2104b0(definition->unknown0c, &target, &position->position);
		}
	}
}
#pragma inline_depth(255)

// @retail 0x25e780
void __stdcall function_25e780(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (position->unknown4c &&
			!((real)context->unknown684 > position->unknown28) &&
			!((real)context->unknown684 > position->unknown2c))
		{
			real value = 0.f;
			real time = (real)context->unknown688;

			if (time <= 0.f || time > position->unknown28)
			{
				value = 20.f;
			}
			else
			{
				real difference = time + 20.f - position->unknown28;
				if (difference > 0.f)
				{
					value = difference;
				}
			}

			position->score += value;
		}
	}
}

// @retail 0x25e800
void __stdcall function_25e800(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	long prop_index = NONE;
	short cached_sector = NONE;
	short cached_area = NONE;
	bool cached_result = false;
	bool use_normal = false;
	bool use_facing = false;

	if (context->unknown668 && length_sq3f(&context->unknown66c) > 0.f)
	{
		use_normal = context->unknown55;
		use_facing = context->unknown54;
	}

	if (context->unknown04)
	{
		prop_index = context->unknown08;
	}

	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (prop_index != NONE)
		{
			short sector = position->reference.unknown2;
			bool blocked = true;

			if (!(sector & 0x8000))
			{
				short area = NONE;
				union
				{
					s_reference field_0;
					long field_1;
				} local_1;
				local_1.field_1 = *(long const volatile *)&position->reference;
				s_262b40_result *result = function_262b40(local_1.field_0);
				if (result)
				{
					area = result->unknown10;
				}

				if (sector != cached_sector || area != cached_area)
				{
					cached_result = function_1b6070(prop_node_get(prop_index)->unknown08, sector, area);
					cached_sector = sector;
					cached_area = area;
				}
				blocked = cached_result;
			}

			if (blocked)
			{
				position->unknown4d = true;
				if (!context->unknown14)
				{
					position->unknown4c = false;
					continue;
				}
			}
		}

		if (position->unknown4c)
		{
			real value = 5.f;
			if (context->unknown11)
			{
				value = 1.f;
			}

			if (!(context->unknown18 * 0.5f > position->unknown18))
			{
				if (context->unknown18 > position->unknown18)
				{
					value = (context->unknown18 - position->unknown18) * (1.f / (context->unknown18 * 0.5f)) * value;
				}
				else
				{
					value = 0.f;
				}
			}
			position->score += value;

			if (context->unknown618)
			{
				if (20.f > position->unknown28)
				{
					position->score = (20.f - position->unknown28) * 0.25f + position->score;
				}
				else if (!context->unknown11)
				{
					position->unknown4d = true;
					if (!context->unknown14)
					{
						position->unknown4c = false;
						continue;
					}
				}

				if (use_facing)
				{
					real dot = dot3f(&position->unknown34, &context->unknown66c);
					real facing = 0.f;

					if (0.f > dot)
					{
						position->unknown4d = true;
						if (!context->unknown14)
						{
							position->unknown4c = false;
							continue;
						}
					}
					else
					{
						dot *= 1.4142135f;
						if (0.f > dot)
						{
							dot = 0.f;
						}
						facing = dot * 15.f;
					}
					position->score += facing;
				}

				if (use_normal)
				{
					real dot = dot3f(&position->unknown40, &context->unknown66c);

					if (0.f > dot)
					{
						position->unknown4d = true;
						if (!context->unknown14)
						{
							position->unknown4c = false;
						}
					}
					else
					{
						dot *= 1.4142135f;
						if (0.f > dot)
						{
							dot = 0.f;
						}
						position->score += dot * 15.f;
					}
				}
			}
		}
	}
}

PRIVATE __forceinline double function_25eb45(vector3f const *arg_0, vector3f const *arg_1)
{
	double local_0 = (double)arg_1->k * arg_0->k;
	local_0 += (double)arg_0->j * arg_1->j;
	local_0 += (double)arg_1->i * arg_0->i;
	return local_0;
}

// @retail 0x25eb00
void __stdcall function_25eb00(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	if (context->unknown618 && context->unknown54 && context->unknown668)
	{
		for (short i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c)
			{
				position->score *= (real)fabs(function_25eb45(&context->unknown66c, &position->unknown34));
			}
		}
	}
}

// @retail 0x25eb70
void __stdcall function_25eb70(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (position->unknown4c)
		{
			real value;

			if (context->range08 > position->unknown18)
			{
				value = 0.f;
			}
			else if (context->range08 * 2.f > position->unknown18)
			{
				value = (position->unknown18 - context->range08) / context->range08 * 8.f;
			}
			else if (context->range18 > position->unknown18)
			{
				value = (context->range18 - position->unknown18) * 8.f / (context->range18 - context->range08 * 2.f);
			}
			else
			{
				value = 0.f;
			}
			position->score += value;

			if (context->unknown618)
			{
				real distance_value;

				if (context->range0c * context->range0c > position->unknown30)
				{
					distance_value = 0.f;
				}
				else if (context->range10 * context->range10 > position->unknown30)
				{
					distance_value = (real)((sqrt(position->unknown30) - context->range0c) * 10.f / (context->range10 - context->range0c));
				}
				else
				{
					distance_value = 10.f;
				}
				position->score += distance_value;
			}
		}
	}
}

// @retail 0x25ec90
void __stdcall function_25ec90(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	short i;

	if (context->unknown18 > 0.f)
	{
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c)
			{
				real value = (1.f - position->unknown18 / context->unknown18) * 8.f;
				if (0.f > value)
				{
					value = 0.f;
				}
				position->score += value;
			}
		}
	}

	if (context->unknown68c)
	{
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c)
			{
				short local_0 = ((s_type_b36ac5 volatile *)position)->reference.unknown2;
				if ((local_0 & 0x8000) ||
					local_0 != context->unknown68e ||
					position->definition->unknown10 != context->unknown690)
				{
					position->unknown4d = true;
					if (!context->unknown14)
					{
						position->unknown4c = false;
					}
				}
			}
		}
	}
}

// @retail 0x25ed60
void __stdcall function_25ed60(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (position->unknown4c)
		{
			real value = position->unknown28 * 0.05f;
			value = (1.f - value) * 8.f;
			if (0.f > value)
			{
				value = 0.f;
			}
			position->score = value + position->score;
		}
	}
}

// @retail 0x25edd0
void __stdcall function_25edd0(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	if (context->unknown618 && context->unknown61c > 0.f)
	{
		for (short i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c)
			{
				real value;

				if (position->unknown2c < 3.4028235e38f)
				{
					value = position->unknown2c / (context->unknown61c * 0.8f);
					if (0.5f > value)
					{
						position->unknown4d = true;
						if (!context->unknown14)
						{
							position->unknown4c = false;
							continue;
						}
					}

					if (0.f > value)
					{
						value = 0.f;
					}
					else if (value > 1.f)
					{
						value = 1.f;
					}
				}
				else
				{
					value = 1.f;
				}
				position->score += value * 8.f;
			}
		}
	}
}

// @retail 0x25eea0
void __stdcall function_25eea0(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	s_character_weapon *weapon = NULL;
	long weapon_index = function_1e1f20(actor_index);

	if (weapon_index != NONE)
	{
		weapon = (s_character_weapon *)function_1e5280(actor_index, unit_weapon_view_get(weapon_index)->tag_index);
	}

	if (!weapon)
	{
		s_actor_firing_view *actor = (s_actor_firing_view *)actor_get(actor_index);
		s_object_child_iterator iterator;

		if (actor->unknown26c == NONE)
		{
			return;
		}

		function_d0620(actor->unknown26c, &iterator);
		while (function_d0690(&iterator))
		{
			long unit_index = iterator.child_index;
			s_unit_weapon_view *unit = unit_weapon_view_get(unit_index);

			if (unit->type == 0)
			{
				long unit_weapon_index = function_e5280(unit_index);

				if (unit_weapon_index != NONE)
				{
					long owner_index = ((s_unit_actor_view *)unit)->actor_index;
					s_character_weapon *candidate = (s_character_weapon *)function_1e5280(owner_index != NONE ? owner_index : actor_index,
						unit_weapon_view_get(unit_weapon_index)->tag_index);

					if (candidate && (!weapon || weapon->unknown0c > candidate->unknown0c))
					{
						weapon = candidate;
					}
				}
			}
		}

		if (!weapon)
		{
			return;
		}
	}

	s_firing_style_view *style = (s_firing_style_view *)function_272a00(actor_index);
	real maximum_range = weapon->unknown18;
	real minimum_range = weapon->unknown14;

	if (style)
	{
		short mode = style->unknown31 > 0 ? style->unknown31 - 1 : style->unknown24;

		switch (mode)
		{
		case 1:
			maximum_range = weapon->unknown28;
			minimum_range = weapon->unknown24;
			break;
		case 2:
			maximum_range = weapon->unknown30;
			minimum_range = weapon->unknown2c;
			break;
		}
	}

	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];

		if (!position->unknown4c)
		{
			continue;
		}

		if (context->unknown618)
		{
			real distance = (real)sqrt(position->unknown30);

			if (weapon->unknown0c > 0.f)
			{
				real range = weapon->unknown0c * 0.8f;

				position->score += range > distance ? 10.f : range / distance * 10.f;
			}

			if (maximum_range > 0.f && maximum_range > distance)
			{
				real margin;
				real bonus = 0.f;

				if (!(minimum_range > context->unknown678))
				{
					minimum_range = context->unknown678;
				}
				margin = maximum_range - distance;
				if (minimum_range > 0.f && margin > distance - minimum_range)
				{
					margin = distance - minimum_range;
				}
				if (margin > 2.f)
				{
					bonus = 10.f;
				}
				else if (margin > 0.f)
				{
					bonus = margin * 0.5f * 10.f;
				}
				position->score += bonus;
			}

			if (position->definition->unknown0c == context->unknown65c)
			{
				position->score += 20.f;
			}
		}

		if (context->unknown278 > 0)
		{
			real nearest = 3.4028235e38f;
			real bonus = 6.f;

			for (short j = 0; j < context->line_count; j++)
			{
				if (context->lines[j].type == 2)
				{
					vector3f offset;
					real t;

					vector3d_from_points3d(&context->lines[j].point, &position->position, &offset);
					t = context->lines[j].direction.i * offset.i + context->lines[j].direction.k * offset.k + offset.j * context->lines[j].direction.j;
					if (t > 0.f)
					{
						vector3f closest;
						real nt = 0.f - t;

						closest.i = context->lines[j].direction.i * nt + offset.i;
						closest.j = context->lines[j].direction.j * nt + offset.j;
						closest.k = nt * context->lines[j].direction.k + offset.k;
						if (nearest > length_sq3f(&closest))
						{
							nearest = length_sq3f(&closest);
						}
					}
				}
			}

			if (12.25f > nearest)
			{
				bonus = (real)sqrt(nearest) * 0.25f;
			}
			position->score += bonus;
		}
	}
}

#pragma inline_depth(0)
// @retail 0x25f290
void __stdcall function_25f290(
	long actor_index,
	s_type_967e20 *context,
	short count,
	s_type_b36ac5 *positions)
{
	s_actor_firing_view *actor = (s_actor_firing_view *)((s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view)));
	bool scored = false;
	short i;

	if (context->unknown5b)
	{
		return;
	}

	real weight = 12.f;

	if (context->unknown618)
	{
		s_character_weapon *weapon = (s_character_weapon *)function_1e5240(actor_index);

		if (weapon)
		{
			if (context->unknown61c > weapon->unknown0c * 1.5f)
			{
				weight = 4.f;
			}
			else if (context->unknown61c > weapon->unknown0c)
			{
				weight = (1.f - (context->unknown61c - weapon->unknown0c) * weapon->unknown0c * 2.f) * weight;
				if (!(weight > 4.f))
				{
					weight = 4.f;
				}
			}
		}
	}

	if (weight > 0.f)
	{
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];

			if (position->unknown4c && context->unknown18 > position->unknown18)
			{
				real factor = 1.f - position->unknown18 / context->unknown18;

				if (0.f > factor)
				{
					factor = 0.f;
				}
				position->score += factor * weight;
			}
		}
	}

	if (actor->unknown030 != NONE)
	{
		s_squad_firing_view *squad = (s_squad_firing_view *)(g_51e9d8->data + (actor->unknown030 & 0xffff) * sizeof(s_squad_firing_view));

		if (squad->zone_index != NONE)
		{
			s_firing_zone_view *zone = &((s_scenario_zone_view *)g_4e0350)->zones[squad->zone_index];

			if (zone && ((zone->flags & 0x10) || ((zone->flags & 0x20) && zone->unknown4e != NONE)) &&
				squad->leader_index != NONE &&
				!function_1df560(actor->unknown024, ((s_unit_team_view *)((s_unit_weapon_view *)((s_object_header_weapon_view *)g_4e0300->data)[squad->leader_index & 0xffff].object))->team))
			{
				long leader_index = squad->leader_index;
				real inner;
				real outer;
				point3f origin;

				function_1b0710(leader_index, actor_index, &inner, &outer);
				inner = (real)sqrt(inner);
				outer = (real)sqrt(outer);
				function_b9dd0(leader_index, &origin);
				for (i = 0; i < count; i++)
				{
					s_type_b36ac5 *position = &positions[i];

					if (position->unknown4c)
					{
						vector3f delta;
						real distance;
						real bonus = 12.f;

						delta.i = position->position.x - origin.x;
						delta.j = position->position.y - origin.y;
						delta.k = position->position.z - origin.z;
						distance = (real)sqrt((delta.i * delta.i + delta.j * delta.j + delta.k * delta.k));
						if (!(inner > distance) && outer > distance)
						{
							bonus = (1.f - (distance - inner) / (outer - inner)) * 12.f;
						}
						position->score += bonus;
					}
				}
				scored = true;
			}
		}
	}

	if (context->unknown276 > 0)
	{
		for (i = 0; i < count; i++)
		{
			s_type_b36ac5 *position = &positions[i];
			short behind_cover = 0;
			short exposed = 0;
			short moving = 0;

			if (!position->unknown4c)
			{
				continue;
			}

			real nearest = 3.4028235e38f;
			real bonus = 10.f;

			if (context->line_count > 0)
			{
				for (short j = 0; j < context->line_count; j++)
				{
					short type = context->lines[j].type;
					real distance = 0.f;

					if (type == 0 || type == 1)
					{
						short result = function_267a80(&distance, &context->lines[j].point, &context->lines[j].direction, &position->position, 0);

						if (nearest > distance)
						{
							nearest = distance;
						}
						if (context->lines[j].type == 0)
						{
							if (exposed <= result)
							{
								exposed = result;
							}
						}
						else if (context->lines[j].type == 1)
						{
							if (behind_cover <= result)
							{
								behind_cover = result;
							}
						}

						if (result == 0 && context->unknown55)
						{
							result = function_267a80(NULL, &context->lines[j].point, &position->unknown40, &position->position, 0);
							if (moving <= result)
							{
								moving = result;
							}
						}
					}
				}

				if (behind_cover >= 2)
				{
					bonus = 0.f;
				}
				else if (behind_cover >= 1)
				{
					bonus = 1.5f;
				}
				else if (exposed >= 2)
				{
					bonus = 4.f;
				}
				else if (exposed >= 1)
				{
					bonus = 6.5f;
				}
			}
			position->score += bonus;
			position->score += moving >= 2 ? 0.f : moving >= 1 ? 3.f : 6.f;

			if (!scored)
			{
				long tag_index = actor->unknown054;

				while (tag_index != NONE)
				{
					s_character_cover_view *character = (s_character_cover_view *)g_4e3b44[tag_index & 0xffff].bytes;

					if (character->cover_count > 0)
					{
						if (character->cover && (character->cover->flags & 4) && 15.f > nearest)
						{
							real cover_bonus = 10.f;

							if (nearest > 3.f)
							{
								cover_bonus = (1.f - (nearest - 3.f) * 0.083333336f) * 10.f;
							}
							position->score += cover_bonus;
						}
						break;
					}
					tag_index = character->parent_tag_index;
				}
			}
		}
	}
}
#pragma inline_depth(255)

PRIVATE __forceinline s_actor_view *function_25fb61(long arg_0)
{
	return (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
}

// @retail 0x25fb60
bool __stdcall function_25fb60(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position)
{
	s_actor_view *actor = function_25fb61(actor_index);

	if (context->unknown56 ||
		(context->unknown5a && (!position || position->unknown58)) ||
		(context->unknown58 && (!position || position->unknown59)))
	{
		if (!position)
		{
			context->unknown680 += 15.f;
		}
		else if (position->unknown4c)
		{
			if (function_29e050(context->unknown60c, actor->unknown26c, position->definition, position->reference, &context->unknown6a0))
			{
				position->score += 15.f;
			}
			else
			{
				position->unknown4d = true;
				if (!context->unknown14)
				{
					position->unknown4c = false;
				}
			}
		}
	}
	bool local_0 = true;
	if (position)
	{
		local_0 = position->unknown4c;
	}
	return local_0;
}

// @retail 0x25fc30
bool __stdcall function_25fc30(
	long unused,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	bool local_0;
	if (position)
	{
		if (!context->unknown11)
		{
			short x = 0;
			long start = NONE;
			s_game_time_globals *globals = g_510c54;
			long time = globals->game_time;
			bool flag = true;

			if (position->type == 0 && position->unknown18 < 6.f)
			{
				start = time;
				x = 7;
				flag = false;
			}

			if (context->unknown10)
			{
				if (flag)
				{
					position->score += 15.f;
				}
			}
			else if (!flag)
			{
				position->unknown4d = true;
				if (!context->unknown14)
				{
					position->unknown4c = false;
				}
			}

			if (position->unknown4c)
			{
				real a = 0.f;
				real local_1 = 10.f;
				real scaled = globals->field_2_3 * local_1;

				if (start != NONE)
				{
					long rounded;

					__asm
					{
						fld scaled
						fistp rounded
					}

					if (start + rounded >= time)
					{
						if (start < time)
						{
							a = (time - start) * globals->rate;
						}
					}
					else
					{
						a = local_1;
					}
				}
				else
				{
					a = local_1;
				}
				position->score += a;

				real b = 0.f;
				if (x < 4)
				{
					b = (4 - x) * 5.f;
				}
				position->score += b;
			}
		}

		local_0 = position->unknown4c;
	}
	else
	{
		local_0 = false;
	}

	return local_0;
}

// @retail 0x25fd50
bool __stdcall function_25fd50(
	long unused,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	if (context->unknown618)
	{
		if (!position)
		{
			context->unknown680 += 15.f;
		}
		else
		{
			real value = 0.f;

			if (position->unknown58)
			{
				switch (position->type)
				{
				case 2:
					value = 15.f;
					break;
				case 3:
					value = 8.f;
					break;
				case 0:
				case 1:
					value = 6.f;
					break;
				case 4:
					value = 12.f;
					break;
				}
			}
			else
			{
				switch (position->type)
				{
				case 2:
					value = 12.f;
					break;
				case 4:
					value = 10.f;
					break;
				case 3:
					value = 4.f;
					break;
				case 1:
					if (position->definition->flags & 0x10)
					{
						break;
					}
				case 0:
					position->unknown4d = true;
					if (!context->unknown14)
					{
						position->unknown4c = false;
					}
					break;
				}
			}

			position->score += value;
		}
	}

	if (!position)
	{
		return true;
	}

	return position->unknown4c;
}

// @retail 0x25fe50
bool __stdcall function_25fe50(
	long unused,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	if (context->unknown618)
	{
		if (!position)
		{
			context->unknown680 += 10.f;
		}
		else
		{
			real value = 0.f;

			switch (position->type)
			{
			case 1:
			case 4:
				value = 2.f;
				break;
			case 0:
				value = 10.f;
				break;
			case 2:
			case 3:
				value = 0.f;
				break;
			}

			position->score += value;
		}
	}

	if (!position)
	{
		return true;
	}

	return position->unknown4c;
}

// @retail 0x25fee0
bool __stdcall function_25fee0(
	long unused,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	if (context->unknown618)
	{
		if (!position)
		{
			context->unknown680 += 20.f;
		}
		else
		{
			real value = 0.f;

			switch (position->type)
			{
			case 0:
				value = 20.f;
				break;
			case 1:
				value = 10.f;
				break;
			default:
				{
					real d = context->unknown61c - 7.5f;
					if (d < 0.f || position->unknown30 > d * d)
					{
						position->unknown4d = true;
						if (!context->unknown14)
						{
							position->unknown4c = false;
						}
					}
				}
				break;
			}

			position->score += value;
		}
	}

	if (!position)
	{
		return true;
	}

	return position->unknown4c;
}

// @retail 0x25ff90
bool __stdcall function_25ff90(
	long unused,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	if (context->unknown618)
	{
		if (!position)
		{
			context->unknown680 += context->unknown644 ? 6.f : 15.f;
		}
		else
		{
			real value = 0.f;

			switch (position->type)
			{
			default:
				if (!context->unknown644)
				{
					position->unknown4d = true;
					if (!context->unknown14)
					{
						position->unknown4c = false;
					}
				}
				break;
			case 1:
				value = context->unknown644 ? 2.5f : 5.f;
				break;
			case 0:
				value = context->unknown644 ? 6.f : 15.f;
				break;
			}

			position->score += value;
		}
	}

	if (!position)
	{
		return true;
	}

	return position->unknown4c;
}
// @retail 0x260060
void function_260060(
	long a,
	long b,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	for (firing_position_pre_evaluator *e = g_44ad90; e->proc; e++)
	{
		if ((1 << context->type) & e->flags)
		{
			e->proc(a, context, b, position);
		}
	}
}

// @retail 0x2600a0
bool function_2600a0(
	long a,
	s_type_967e20 *context,
	s_type_b36ac5 *position)
{
	bool result = true;

	for (firing_position_post_evaluator *e = g_44adf0; result && e->proc; e++)
	{
		if ((1 << context->type) & e->flags)
		{
			result = e->proc(a, context, position);
		}
	}

	return result;
}

// @retail 0x2600e0
bool __stdcall function_2600e0(
	long a,
	long b,
	void *unused)
{
	s_type_b36ac5 *pa = &g_51eca0[a];
	s_type_b36ac5 *pb = &g_51eca0[b];

	if (pa->unknown4c != pb->unknown4c)
	{
		return (pa->unknown4c ? -1 : 1) > 0;
	}
	if (pa->unknown4d != pb->unknown4d)
	{
		return (pa->unknown4d ? 1 : -1) > 0;
	}
	if (pa->score > pb->score)
	{
		return false;
	}
	if (pb->score > pa->score)
	{
		return true;
	}
	return false;
}
firing_position_pre_evaluator g_44ad90[12] =
{
	{-1, function_25dd50},
	{0x1, function_25eea0},
	{0x4d, function_25f290},
	{0x1, function_25eb00},
	{0x10, function_25ec90},
	{0x100, function_25ed60},
	{0x82, function_25eb70},
	{0x20, function_25e800},
	{0x8, function_25e780},
	{0x6, function_25edd0},
	{0xd, function_25e430},
};

firing_position_post_evaluator g_44adf0[12] =
{
	{0x41, function_25ff90},
	{0x8, function_25fee0},
	{0x6, function_25fd50},
	{0x20, function_25fc30},
	{0x80, function_25fe50},
	{-1, function_25fb60},
};

bool (__stdcall *g_comparator)(long, long, void *) = function_2600e0;

void function_1e3b00(long object_index, long mode, point3f const *reference,
	void const *unknown0, void const *unknown1, point3f *position);
long function_11c010(short row, short column);
short __stdcall function_272af0(s_match_globals *arg_0, point3f const *arg_1);
short __stdcall function_1c8df0(long arg_0, point3f const *arg_4, short arg_3, short arg_2,
	void const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9);
real normalize2d(point2f *v);

#pragma optimize("s", on)
// @retail 0x25f7b0
void __stdcall function_25f7b0(long arg_0, s_type_967e20 *arg_1, s_type_b36ac5 *arg_2)
{
	s_actor_view *local_0 = actor_get(arg_0);
	short local_1 = *(short *)arg_1;
	if (local_1 == 5 && !arg_1->unknown11)
	{
		if (arg_2->unknown18 < 6.f)
		{
			point3f local_2;
			function_1e3b00(local_0->unknown018, 1, &arg_2->position, NULL, NULL, &local_2);
			short local_3;
			if (arg_2->definition->flags & 8)
				local_3 = function_272af0(g_4e0348, &arg_2->position);
			else
				local_3 = arg_2->definition->unknown12;
			bool local_4 = local_0->unknown26c != NONE;
			short local_5 = *(short *)((byte *)local_0 + 0x254);
			point3f const *local_6 = (point3f *)((byte *)local_0 + 0x22c);
			if (local_5 == NONE || local_3 == NONE || function_11c010(local_3, local_5))
			{
				s_collision_result_1697c0 local_7;
				local_7.unknown24 = NONE;
				long local_8 = local_4 ? 0x15808c0f : 0x15808c2f;
				vector3f local_9;
				vector3d_from_points3d(local_6, &local_2, &local_9);
				if (!function_1697c0(local_8, local_6, &local_9, NONE, NONE, &local_7))
				{
					arg_2->type = 0;
					return;
				}
				double local_10 = (double)local_2.x - local_6->x;
				double local_11 = (double)local_2.y - local_6->y;
				double local_12 = (double)local_2.z - local_6->z;
				real local_13 = (real)sqrt(local_12 * local_12 + local_11 * local_11 + local_10 * local_10);
				if (local_13 >= 1.f && local_13 * *(real *)((byte *)&local_7 + 4) < 1.f)
				{
					arg_2->type = 2;
					return;
				}
			}
		}
		arg_2->type = 4;
		return;
	}
	long local_14 = 0;
	void const *local_15 = NULL;
	vector3f const *local_16;
	vector3f local_17;
	short local_18;
	if (local_1 == 1 || local_1 == 2)
	{
		local_18 = 2;
		local_16 = NULL;
	}
	else if (*((byte *)arg_1 + 0x5fc))
	{
		vector3d_from_points3d(&arg_2->position, (point3f *)((byte *)arg_1 + 0x62c), &local_17);
		local_18 = 3;
		local_15 = (byte *)arg_1 + 0x600;
		if (normalize2d((point2f *)&local_17) > 0.f)
		{
			local_17.k = 0.f;
			local_16 = &local_17;
		}
		else
			local_16 = &local_0->unknown290;
	}
	else
	{
		local_18 = 1;
		local_16 = NULL;
	}
	long local_19 = function_1e1f20(arg_0);
	if (local_19 != NONE)
	{
		byte *local_20 = (byte *)function_1e5280(arg_0, object_get(local_19)->tag_index);
		if (local_20 && (*local_20 & 2))
			local_18 = 2;
	}
	point3f local_21;
	function_1e3b00(local_0->unknown018, local_18, &arg_2->position, local_16, local_15, &local_21);
	if (local_18 == 2 && arg_2->unknown5c == 3)
		local_21.z -= 0.1f;
	switch (*(short *)arg_1)
	{
	case 1:
	case 3:
		local_14 = 1;
		break;
	case 2:
		local_14 = !(arg_2->definition->flags & 0x10);
		break;
	}
	bool local_22 = *(short *)arg_1 != 1 && *(short *)arg_1 != 2;
	short local_23;
	if (arg_2->definition->flags & 8)
		local_23 = function_272af0(g_4e0348, &local_21);
	else
		local_23 = arg_2->definition->unknown12;
	arg_2->type = function_1c8df0(*(long *)((byte *)arg_1 + 0x648), (point3f const *)((byte *)arg_1 + 0x638),
		*(short *)((byte *)arg_1 + 0x660), local_23, &local_21, local_14, true,
		local_0->unknown26c != NONE, local_22, NULL);
}
#pragma optimize("", on)
