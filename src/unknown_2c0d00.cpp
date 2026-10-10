/* UNKNOWN_2C0D00.CPP: a list of up to 64 circular obstacles on the ground
   plane that a path is steered around: adding them, finding the ones a point
   or a ray hits, and grouping the ones that overlap */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_2c0d00.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

#include "ai_actor.h"
#include "path.h"
#include "object_markers.h"
#include "props.h"
#include "object_queries.h"
#include "unknown_1428b0.h"

real function_30bf0(vector3f *vector);
point3f *function_b9dd0(long object_index, point3f *result);
bool object_or_parent_hidden(long object_index);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_11bed0(s_location *location, point3f const *point);
short __stdcall function_bb050(long mask, dword type_mask, void const *location,
    point3f const *position, real radius, long *objects, short maximum_count);
short function_26f930(long object_index, s_object_marker *markers, short capacity, long flags);
short function_26fbf0(long object_index, s_object_marker *markers, short capacity, long flags);
short function_26fe20(long object_index, s_object_marker *markers, short capacity, long flags);
bool function_26f8f0(s_object_marker const *marker);
long function_25d810(long object_index, long actor_index, bool create);

struct s_obstacle_object_flags_2c
{
    dword : 22;
    dword restricted_spheres : 1;
    dword no_obstacle : 1;
    dword : 8;
};

struct s_obstacle_unit_flags_2c
{
    byte : 2;
    byte no_obstacle : 1;
    byte : 5;
};

struct s_obstacle_definition_flags_2c
{
    byte : 3;
    byte no_obstacle : 1;
    byte : 4;
};

struct s_obstacle_motion_definition_2c
{
    word flags;
    byte field_2[0xc - 2];
    short priority;
};

struct s_obstacle_sphere_2c
{
    short node;
    word flags;
    point3f center;
    real radius;
};

struct s_obstacle_physics_definition_2c
{
    byte field_0[0x24];
    long count;
    s_obstacle_sphere_2c *spheres;
};


/* the nearest obstacle a ray hits */
struct s_obstacle_hit
{
	real distance;
	short index;
	short group;
};

static inline void vector2d_from_points2d(point2f const *p0, point2f const *p1, point2f *out)
{
	out->x = p1->x - p0->x;
	out->y = p1->y - p0->y;
}

static inline real dot_product2d(point2f const *a, point2f const *b)
{
	return a->x * b->x + a->y * b->y;
}

static inline real length_sq2f(point2f const *v)
{
	return v->x * v->x + v->y * v->y;
}

static inline bool bit_vector_test(dword const *bits, long index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

static inline void bit_vector_set(dword *bits, long index)
{
	bits[index >> 5] |= 1 << (index & 31);
}

/* the two directions, turned either way from the direction, that just touch
   a circle of the radius at the distance, and how far along them it is
   touched */
// @retail 0x2c0c40
void tangent_directions(point2f const *direction, point2f *left, point2f *right, real distance, real radius, real *tangent_length)
{
	real sine = 1.0f;
	real cosine;
	real negative_sine;

	if (distance > 0.0f)
	{
		sine = radius / distance;
		if (sine > 1.0f)
		{
			sine = 1.0f;
		}
	}
	cosine = (real)sqrt(1.0f - sine * sine);
	negative_sine = 0.0f - sine;
	left->y = direction->x * negative_sine + cosine * direction->y;
	left->x = direction->x * cosine - negative_sine * direction->y;
	right->y = direction->x * sine + cosine * direction->y;
	right->x = cosine * direction->x - sine * direction->y;
	*tangent_length = cosine * distance;
}

static inline real normalize_local2d_2c(point2f *v)
{
	real magnitude = (real)sqrt(length_sq2f(v));

	if (fabs(magnitude) < 0.0001f)
	{
		magnitude = 0.0f;
	}
	else
	{
		real inverse = 1.0f / magnitude;

		v->x *= inverse;
		v->y *= inverse;
	}
	return magnitude;
}

/* the directions from the point that pass the obstacle, grown by the radius,
   on either side */
// @retail 0x2c1d90
void obstacle_tangent_directions(point2f const *point, s_obstacle_list const *list, short index, real radius,
	point2f *left, point2f *right, real *tangent_length)
{
	s_obstacle const *obstacle = &list->obstacles[index];
	point2f direction;
	real distance;

	vector2d_from_points2d(point, &obstacle->center, &direction);
	distance = normalize_local2d_2c(&direction);
	if (distance > 0.0f)
	{
		tangent_directions(&direction, left, right, distance, obstacle->radius + radius + 0.00390625f, tangent_length);
	}
	else
	{
		left->x = 0.0f;
		left->y = -1.0f;
		right->x = 0.0f;
		right->y = 1.0f;
		*tangent_length = 0.0f;
	}
}

// @retail 0x2c0d00
bool obstacle_list_add(s_obstacle_list *list, word flags, point2f const *center, long object_index, real radius)
{
	real volatile const *radius_reference = &radius;
    s_obstacle *obstacle;

	if (list->count == 64)
	{
		return false;
	}
	obstacle = &list->obstacles[list->count++];
	if (flags & 1)
	{
		list->flag0_count++;
	}
	if (flags & 8)
	{
		list->flag3_count++;
	}
	obstacle->flags = flags;
	obstacle->group = NONE;
	obstacle->object_index = object_index;
		obstacle->center.y = *(real volatile const *)&center->y;
    *(long *)&obstacle->center.x = *(long const *)&center->x;
    obstacle->radius = *radius_reference;
	return true;
}

void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
    s_path_settings const *settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
    point3f const *position, real radius, vector3f const *direction, long unit_index, long ignore_index);

// @retail 0x2c0d60
void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
    s_path_settings const *volatile settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
    point3f const *position, real radius, vector3f const *direction, volatile long unit_index, long ignore_index)
{
    short ignore_priority = 0;
    short block_priority = 0;
    short range_min = 0;
    short range_max = 0;
    byte *self_unit = NULL;
    s_actor_view *volatile actor = NULL;
    bool straight = true;
    bool aligned = true;
    bool local_4d3867 = false;
    if (actor_index != NONE)
    {
        actor = actor_get(actor_index);
        byte *unit = (byte *)ai_object_get(actor->unknown018);
        if (previous_direction)
        {
            real dot = direction->k * previous_direction->k;
            dot += direction->j * previous_direction->j;
            dot += direction->i * previous_direction->i;
            straight = dot > 0.95f;
            aligned = dot > 0.8660253882408142f;
        }
        local_4d3867 = actor->unknown26c != NONE;
        if (unit[0xaa] == 0)
            self_unit = unit;
        if (settings)
        {
            if (flags & 1) ignore_priority = settings->unknown0e;
            if (flags & 4) block_priority = settings->unknown10;
            if (flags & 2)
            {
                range_min = settings->unknown12;
                range_max = settings->unknown14;
            }
        }
    }
    struct { s_location location; point3f raised; } query;
    s_location &location = query.location;
    if (unit_index != NONE)
        location = *(s_location *)((byte *)ai_object_get(unit_index) + 0x28);
    else
    {
        point3f &raised = query.raised;
        raised.x = g_4687b0->i * 0.05f + position->x;
        raised.y = g_4687b0->j * 0.05f + position->y;
        raised.z = g_4687b0->k * 0.05f + position->z;
        function_11bed0(&location, &raised);
    }
    long object_indices[128];
    short object_count = function_bb050(1, 0x8c3, &location, position, radius, object_indices, 128);
    s_object_marker first_markers[32];
    s_object_marker second_markers[32];
    s_object_marker third_markers[32];
    for (short i = 0; i < object_count; ++i)
    {
        long object_index = object_indices[i];
        byte *object = (byte *)ai_object_get(object_index);
        real inflation = 0.0f;
        bool skip_classification = false;
        bool device_spheres = false;
        bool restricted_spheres = false;
        if (object_index == unit_index || object_index == ignore_index)
            continue;
        s_obstacle_object_flags_2c *object_flags = (s_obstacle_object_flags_2c *)(object + 4);
        if (TEST_FIELD_BIT(object_flags->restricted_spheres))
            restricted_spheres = true;
        else if (TEST_FIELD_BIT(object_flags->no_obstacle))
            continue;
        if (object_or_parent_hidden(object_index))
            continue;
        if (self_unit && self_unit[0x3dc] == 1 && object_index == *(long *)(self_unit + 0x448))
            continue;
        long type = (char)object[0xaa];
        if (type == 0)
        {
            if (TEST_FIELD_BIT(((s_obstacle_unit_flags_2c *)(object + 0x10a))->no_obstacle))
                continue;
            if (actor_index != NONE)
            {
                long reference_index = function_25d810(object_index, actor_index, false);
                if (reference_index != NONE && prop_ref_get(reference_index)->state < 1)
                    continue;
            }
            long other_actor_index = *(long *)(object + 0x12c);
            if (other_actor_index != NONE)
            {
                s_actor_view *other = actor_get(other_actor_index);
                bool enemy = actor && function_1df560(other->unknown024, actor->unknown024);
                if (*((byte *)other + 0x5d0) && (!actor || actor->unknown26c == NONE))
                {
                    point3f center;
                    function_b9dd0(object_index, &center);
                    vector3f offset;
                    offset.i = center.x - position->x;
                    offset.j = center.y - position->y;
                    offset.k = center.z - position->z;
                    if (function_30bf0(&offset) > 0.0f)
                    {
                        vector3f const *other_direction = (vector3f const *)((byte *)other + 0x5ec);
                        real dot = offset.k * other_direction->k + offset.j * other_direction->j + offset.i * other_direction->i;
                        if (dot > magnitude3d(other_direction) * 0.7071067690849304f)
                            continue;
                    }
                }
                if (actor)
                {
                    if (actor->unknown26c != NONE && !enemy)
                        inflation = 1.0f;
                    else if (enemy && actor->unknown223)
                        inflation = 0.5f;
                }
                skip_classification = !enemy;
            }
            else if (actor && *(long *)(object + 0x13c) != NONE)
            {
                bool enemy = team_is_enemy(actor->unknown024, 1);
                if (actor->unknown26c != NONE && !enemy)
                    inflation = 1.0f;
                else if (enemy && actor->unknown223)
                    inflation = 0.5f;
                skip_classification = !enemy;
            }
        }
        else if (type == 1)
        {
            if (actor && actor->unknown26c == NONE)
            {
                short team = *(short *)(object + 0x138);
                skip_classification = team == actor->unknown024 || !function_1df560(team, actor->unknown024);
            }
        }
        else if (type == 7)
        {
            byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            word device_flags = *(word *)(definition + 0x11e);
            if (!(device_flags & 1) || ((device_flags & 2) && (*(real *)(object + 0x140) == 1.0f || !(object[0x1cc] & 1))))
                device_spheres = true;
        }
        else if (type == 11)
        {
            long component_index = *(long *)(object + 0xb4);
            if (component_index != NONE && *(char *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0 + 0x1c) <= 1)
                continue;
        }
        vector3f offset;
        offset.i = *(real *)(object + 0x30) - position->x;
        offset.j = *(real *)(object + 0x34) - position->y;
        offset.k = *(real *)(object + 0x38) - position->z;
        real object_radius = *(real *)(object + 0x3c) + radius;
        real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
        if (!(object_radius * object_radius >= distance_squared))
            continue;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        if (TEST_FIELD_BIT(((s_obstacle_definition_flags_2c *)(definition + 2))->no_obstacle))
            continue;
        s_obstacle_motion_definition_2c *motion = NULL;
        bool blocking = false;
        dword obstacle_flags = 0;
        dword sphere_flags = 0;
        if (settings->unknown18 != object_index && !skip_classification)
        {
            short priority = 0;
            bool enabled = true;
            if (*(long *)(definition + 0x5c) > 0)
            {
                motion = *(s_obstacle_motion_definition_2c **)(definition + 0x60);
                if ((motion->flags & 2) && *(real *)(object + 0xec) <= 0.0f)
                    enabled = false;
                else
                    priority = motion->priority;
            }
            if (priority == 0)
            {
                long component_index = *(long *)(object + 0xb4);
                if (component_index != NONE)
                    priority = *(char *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0 + 0x1c);
                if (!enabled)
                    continue;
            }
            if (priority > 0)
            {
                if (ignore_priority >= priority)
                    continue;
                if (block_priority >= priority && aligned)
                {
                    obstacle_flags = 0x10;
                    blocking = true;
                }
                else if (range_min <= priority && range_max >= priority && type != 0)
                {
                    obstacle_flags = 0x20;
                    blocking = true;
                }
            }
            if ((flags & 8) && straight)
            {
                short count = function_26f930(object_index, first_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    s_object_marker const *marker = &first_markers[m];
                    if (function_26f8f0(marker))
                    {
                        real dot = marker->matrix.up.k * (position->z - marker->matrix.position.z);
                        dot += marker->matrix.up.j * (position->y - marker->matrix.position.y);
                        dot += (position->x - marker->matrix.position.x) * marker->matrix.up.i;
                        if (dot >= -0.2f && dot < marker->unknown6c + 0.2f &&
                            fabs(marker->matrix.left.k * direction->k + marker->matrix.left.j * direction->j + marker->matrix.left.i * direction->i) >= 0.9f)
                        {
                            obstacle_flags |= 0x40;
                            blocking = true;
                            break;
                        }
                    }
                }
            }
            if ((flags & 0x10) && aligned)
            {
                short count = function_26fbf0(object_index, second_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    s_object_marker const *marker = &second_markers[m];
                    if (function_26f8f0(marker))
                    {
                        real dot = marker->matrix.up.k * (position->z - marker->matrix.position.z);
                        dot += marker->matrix.up.j * (position->y - marker->matrix.position.y);
                        dot += marker->matrix.up.i * (position->x - marker->matrix.position.x);
                        if (dot >= 0.1f && dot < marker->unknown6c - 0.1f &&
                            fabs(marker->matrix.left.k * direction->k + marker->matrix.left.j * direction->j + direction->i * marker->matrix.left.i) >= 0.9f)
                        {
                            obstacle_flags |= 0x100;
                            blocking = true;
                            break;
                        }
                    }
                }
            }
            if ((flags & 0x20) && aligned)
            {
                short count = function_26fe20(object_index, third_markers, 32, (word)settings->unknown04);
                for (short m = 0; m < count; ++m)
                {
                    if (function_26f8f0(&third_markers[m]))
                    {
                        obstacle_flags |= 0x80;
                        blocking = true;
                        break;
                    }
                }
            }
            if (actor && actor->unknown26c != NONE && ((1 << type) & 3))
            {
                point3f center;
                function_b9dd0(object_index, &center);
                real dot = (center.z - position->z) * direction->k;
                dot += (center.x - position->x) * direction->i;
                dot += direction->j * (center.y - position->y);
                long vehicle_index = *(long *)(object + 0x248);
                if (dot > 0.0f && vehicle_index != NONE)
                {
                    short team = *(short *)(object + 0x138);
                    if (team == actor->unknown024 || !function_1df560(team, actor->unknown024))
                    {
                        byte *vehicle = (byte *)ai_object_get(vehicle_index);
                        long vehicle_actor_index = *(long *)(vehicle + 0x12c);
                        byte *vehicle_actor = NULL;
                        if (vehicle_actor_index != NONE)
                            vehicle_actor = (byte *)actor_get(vehicle_actor_index);
                        vector3f const *velocity = (vector3f const *)(object + 0x88);
                        if ((vehicle_actor_index != NONE && vehicle_actor[0x5d0] &&
                                *(real *)(vehicle_actor + 0x5f4) * direction->k +
                                *(real *)(vehicle_actor + 0x5f0) * direction->j +
                                direction->i * *(real *)(vehicle_actor + 0x5ec) > 0.0f) ||
                            velocity->k * direction->k + velocity->j * direction->j + direction->i * velocity->i > 0.0f)
                        {
                            obstacle_flags |= 0x200;
                            blocking = true;
                            inflation = 4.0f;
                        }
                    }
                }
            }
            if (motion && (flags & 0x40) && (motion->flags & 2))
                sphere_flags = 8;
        }
        long model_index = *(long *)(definition + 0x38);
        if (model_index == NONE)
            continue;
        long physics_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0xc);
        if (physics_index == NONE)
            continue;
        s_obstacle_physics_definition_2c *physics = (s_obstacle_physics_definition_2c *)g_4e3b44[physics_index & 0xffff].bytes;
        if (physics->count <= 0)
            continue;
        transform4x3f object_matrix;
        function_ba160(object_index, &object_matrix);
        for (short s = 0; s < physics->count; ++s)
        {
            s_obstacle_sphere_2c const *sphere = &physics->spheres[s];
            if (device_spheres && !(sphere->flags & 1))
                continue;
            if (restricted_spheres && !(sphere->flags & 4))
                continue;
            if (!local_4d3867 && (sphere->flags & 2))
                continue;
            point3f center;
            real local_a47c5e;
            if (sphere->node != NONE)
            {
                transform4x3f *matrix = function_b8bd0(object_index, sphere->node);
                transform4x3f_apply_point(matrix, &sphere->center, &center);
                local_a47c5e = sphere->radius * matrix->scale;
            }
            else
            {
                transform4x3f_apply_point(&object_matrix, &sphere->center, &center);
                local_a47c5e = sphere->radius * object_matrix.scale;
            }
            if (position->z > center.z + local_a47c5e + 1.0f && direction->k > -0.2f)
                continue;
            if (center.z - local_a47c5e - 1.0f > position->z && direction->k < 0.2f)
                continue;
            vector3f difference;
            difference.k = center.z - position->z;
            difference.j = center.y - position->y;
            difference.i = center.x - position->x;
            real squared = difference.k * difference.k * 4.0f;
            squared += difference.j * difference.j;
            squared += difference.i * difference.i;
            real combined_radius = local_a47c5e + radius;
            if (!(combined_radius * combined_radius >= squared))
                continue;
            long moving = 0;
            if (type == 0)
            {
                real dot = direction->j * difference.j + difference.k * direction->k + direction->i * difference.i;
                if (dot > 0.0f)
                {
                    vector3f const *velocity = (vector3f const *)(object + 0x88);
                    if (velocity->k * direction->k + velocity->j * direction->j + velocity->i * direction->i > 2.0f)
                        moving = 1;
                }
            }
            word combined_flags = (word)(moving | sphere_flags | obstacle_flags);
            if (blocking)
            {
                if (blocking_obstacles)
                    obstacle_list_add(blocking_obstacles, (word)obstacle_flags, (point2f const *)&center, object_index, local_a47c5e + inflation);
            }
            else
                obstacle_list_add(obstacles, combined_flags, (point2f const *)&center, object_index, local_a47c5e + inflation);
        }
    }
}

/* the first obstacle other than the one ignored whose circle, grown by the
   radius, holds the point */
// @retail 0x2c1c00
short obstacle_list_find_containing(s_obstacle_list const *list, short ignore_index, point2f const *point, real radius)
{
	short count = list->count;

	for (short i = 0; i < count; i++)
	{
		if (i != ignore_index)
		{
			s_obstacle const *obstacle = &list->obstacles[i];
			point2f offset;
			real distance;

			real distance_squared;

			vector2d_from_points2d(point, &obstacle->center, &offset);
			distance_squared = length_sq2f(&offset);
			distance = obstacle->radius + radius;
			if (distance * distance >= distance_squared)
			{
				return i;
			}
		}
	}
	return NONE;
}

/* the nearest obstacle, grown by the radius, that the ray from the origin
   hits within the maximum distance */
// @retail 0x2c1c70
bool obstacle_list_cast_ray(s_obstacle_list const *list, short ignore_index, point2f const *origin, point2f const *direction,
	real radius, real maximum_distance, bool ignore_flagged, s_obstacle_hit *hit)
{
	hit->distance = maximum_distance;
	hit->index = NONE;
	hit->group = NONE;
	for (short i = 0; i < list->count; i++)
	{
		if (i != ignore_index)
		{
			s_obstacle const *obstacle = &list->obstacles[i];

			if (!ignore_flagged || !(obstacle->flags & 1))
			{
				point2f offset;
				real along;
				real distance;

				vector2d_from_points2d(origin, &obstacle->center, &offset);
				distance = obstacle->radius + radius;
				along = dot_product2d(direction, &offset);
				if (along > 0.0f)
				{
					real outside = length_sq2f(&offset) - distance * distance;
					real hit_distance;

					if (outside <= 0.0f)
					{
						hit_distance = 0.0f;
					}
					else
					{
						real discriminant = along * along - outside;

						if (discriminant >= 0.0f)
						{
							hit_distance = (real)(along - sqrt(discriminant));
						}
						else
						{
							continue;
						}
					}
					if (hit->distance > hit_distance)
					{
						hit->distance = hit_distance;
						hit->index = i;
						hit->group = obstacle->group;
					}
				}
			}
		}
	}
	return hit->index != NONE;
}

/* marks the obstacles that overlap the first one, directly or through
   others, each grown by the radius */
// @retail 0x2c1e70
void obstacle_list_flood(short first_index, dword *bits, real radius, s_obstacle_list const *list)
{
	memset(bits, 0, ((list->count + 31) >> 5) * sizeof(dword));
	if (first_index != NONE)
	{
		short stack[64];
		short stack_count;

		bit_vector_set(bits, first_index);
		stack[0] = first_index;
		stack_count = 1;
		do
		{
			s_obstacle const *current = &list->obstacles[stack[--stack_count]];

			for (short i = 0; i < list->count; i++)
			{
				if (!bit_vector_test(bits, i))
				{
					s_obstacle const *other = &list->obstacles[i];
					point2f offset;
					real distance;

					distance = (other->radius + radius) + (current->radius + radius);
					vector2d_from_points2d(&current->center, &other->center, &offset);
					if (distance * distance >= length_sq2f(&offset))
					{
						bit_vector_set(bits, i);
						stack[stack_count++] = i;
					}
				}
			}
		}
		while (stack_count > 0);
	}
}

/* numbers the groups of overlapping obstacles */
// @retail 0x2c1fb0
void obstacle_list_group(s_obstacle_list *list, real radius)
{
	short i;

	list->group_count = 0;
	for (i = 0; i < list->count; i++)
	{
		list->obstacles[i].group = NONE;
	}
	for (i = 0; i < list->count; i++)
	{
		if (list->obstacles[i].group == NONE)
		{
			dword bits[2];
			short group = list->group_count++;

			obstacle_list_flood(i, bits, radius, list);
			for (short j = 0; j < list->count; j++)
			{
				if (bit_vector_test(bits, j))
				{
					list->obstacles[j].group = group;
				}
			}
		}
	}
}

point2f const g_440b3c = { 0.0f, 1.0f };
point2f const *g_46877c = &g_440b3c;

PRIVATE __forceinline void scale_offset_direction_2c(real scale, point2f const *v, point2f *out)
{
    out->x = scale * v->x;
    out->y = v->y * scale;
}

PRIVATE __forceinline real normalize_offset_direction_2c(point2f *v)
{
    real magnitude = (real)sqrt(length_sq2f(v));
    if (fabs(magnitude) < 0.0001f)
        magnitude = 0.0f;
    else
    {
        real inverse = 1.0f / magnitude;
        scale_offset_direction_2c(inverse, v, v);
    }
    return magnitude;
}

// @retail 0x2c2910
void function_2c2910(point2f const *center, point2f const *points, point2f const *previous, real radius, point2f *result)
{
    point2f first, second;
    vector2d_from_points2d(center, &points[0], &first);
    vector2d_from_points2d(center, &points[1], &second);
    real cross = first.x * second.y - second.x * first.y;
    if (!(fabs(cross) < 0.0001f))
    {
        real factor = radius * radius / cross;
        real y = center->y + (first.x - second.x) * factor;
        real x = center->x - (first.y - second.y) * factor;
        result->x = x;
        result->y = y;
        point2f delta;
        vector2d_from_points2d(center, result, &delta);
        if (!(length_sq2f(&delta) > radius * radius * 4.0f))
            return;
    }
    point2f direction;
    vector2d_from_points2d(previous, points, &direction);
    if (normalize_offset_direction_2c(&direction) == 0.0f)
        direction = *g_46877c;
    result->x = points[0].x + direction.x * radius;
    result->y = points[0].y + direction.y * radius;
}

// @retail 0x2c2740
void function_2c2740(point2f const *point, point2f const *center, real radius, bool side, point2f *result)
{
    point2f direction;
    vector2d_from_points2d(center, point, &direction);
    real distance_squared = direction.y * direction.y + direction.x * direction.x;
    real factor = distance_squared > 0.0f ? radius / distance_squared : 0.0f;
    point2f candidates[2];
    real tangent_squared = distance_squared - radius * radius;
    if (tangent_squared > 0.0f)
    {
        real tangent = (real)sqrt(tangent_squared);
        candidates[0].x = center->x + (direction.x * radius + direction.y * tangent) * factor;
        candidates[0].y = center->y + (direction.y * radius - direction.x * tangent) * factor;
        candidates[1].x = center->x + (direction.x * radius - direction.y * tangent) * factor;
        candidates[1].y = center->y + (direction.y * radius + direction.x * tangent) * factor;
        point2f first, second;
        vector2d_from_points2d(point, &candidates[0], &first);
        vector2d_from_points2d(point, &candidates[1], &second);
        long orientation = first.x * second.y - first.y * second.x > 0.0f ? 1 : 0;
        *result = candidates[orientation != side];
    }
    else
    {
        candidates[0] = direction;
        if (normalize_offset_direction_2c(&candidates[0]) == 0.0f)
            candidates[0] = *g_46877c;
        result->x = center->x + candidates[0].x * radius;
        result->y = center->y + candidates[0].y * radius;
    }
}

#include "unknown_1fa590.h"

PRIVATE __forceinline word edge_node_2c(s_pathfinding_edge const *edge, long side)
{
    return side ? edge->surface : edge->unknown0c;
}

PRIVATE __forceinline bool path_node_open_2c(s_pathfinding_data const *data, long index)
{
    if (index == NONE || index == 0xffff)
        return false;
    s_pathfinding_node const *node = &data->nodes[index];
    return (node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(node, data));
}

// @retail 0x2c3030
bool function_2c3030(s_pathfinding_data const *data, point2f const *origin, real radius,
    long edge_index, bool side, point2f *result)
{
    volatile long previous_vertex = NONE;
    volatile long first_vertex = NONE;
    for (;;)
    {
        s_pathfinding_edge const *volatile edge = &data->edges[edge_index];
        bool first_open = path_node_open_2c(data, edge_node_2c(edge, 0));
        long node_index = edge_node_2c(edge, !first_open);
        if (!path_node_open_2c(data, node_index))
            return false;
        s_pathfinding_node const *node = &data->nodes[node_index];
        short surface_index = *(short const *)(edge->unknown04 + 2);
        if (previous_vertex != NONE && surface_index != NONE && data->surface_count > 0)
        {
            s_pathfinding_edge const *portal = &data->edges[data->surfaces[surface_index].index];
            long opposite = edge_node_2c(portal, portal->surface != node_index);
            if (path_node_open_2c(data, opposite))
            {
                *result = *(point2f const *)&data->vertices[previous_vertex];
                return true;
            }
        }
        long vertex0 = edge->vertices[0];
        long vertex1 = edge->vertices[1];
        point2f const *start = (point2f const *)&data->vertices[first_open ? vertex1 : vertex0];
        point2f const *end = (point2f const *)&data->vertices[first_open ? vertex0 : vertex1];
        point2f direction;
        vector2d_from_points2d(start, end, &direction);
        point2f normal = { direction.y, 0.0f - direction.x };
        normalize_offset_direction_2c(&normal);
        point2f positive = { origin->x + normal.x * radius, origin->y + normal.y * radius };
        point2f negative = { origin->x + normal.x * (0.0f - radius), origin->y + normal.y * (0.0f - radius) };
        point2f first, second;
        vector2d_from_points2d(&positive, start, &first);
        vector2d_from_points2d(&negative, start, &second);
        bool crossing = false;
        if ((first.y * direction.y + first.x * direction.x < 0.0f ? 1 : 0) == side &&
            first.x * direction.y - first.y * direction.x < 0.0f)
            crossing = true;
        if (second.x * direction.y - second.y * direction.x < 0.0f)
            crossing = true;
        if (first_vertex == NONE)
            crossing = true;
        long vertex = ((crossing != first_open) == side) ? vertex1 : vertex0;
        if (vertex == previous_vertex)
        {
            *result = *(point2f const *)&data->vertices[vertex];
            return true;
        }
        if (vertex == first_vertex)
            return false;
        if (first_vertex == NONE)
        {
            if (edge->unknown04[0] & 0x40)
            {
                *result = *(point2f const *)&data->vertices[vertex];
                return true;
            }
            first_vertex = vertex;
        }
        if (!(node->flags & 8))
        {
            for (long surface = node->first_surface; surface != NONE && data->surface_count > 0;
                surface = data->surfaces[surface].next)
            {
                s_pathfinding_surface const *portal = &data->surfaces[surface];
                if (portal->type == 0 && ((long const *)portal->unknown0c)[!side] == edge_index)
                {
                    s_pathfinding_edge const *target = &data->edges[portal->object_index];
                    if (path_node_open_2c(data, edge_node_2c(target, 0)) ||
                        path_node_open_2c(data, edge_node_2c(target, 1)))
                    {
                        s_pathfinding_edge const *source = &data->edges[portal->index];
                        edge_index = portal->object_index;
                        previous_vertex = source->vertices[side ? 0 : 1];
                        first_vertex = NONE;
                        goto next_edge;
                    }
                }
            }
        }
        {
            long first_edge = edge_index;
            short count = 0;
            for (;;)
            {
                long direction_index = vertex != edge->vertices[1];
                if (path_node_open_2c(data, edge_node_2c(edge, direction_index)) == side)
                    break;
                edge_index = edge->next_edges[direction_index];
                if (edge_index == 0xffff)
                    return false;
                edge = &data->edges[edge_index];
                if (edge_index == first_edge || ++count > 50)
                    return false;
            }
        }
        previous_vertex = vertex;
next_edge:;
    }
}

__declspec(noinline) real normalize2d(point2f *v);

PRIVATE __forceinline real clamped_acos_2c(real value)
{
    real clamped = value < -1.0f ? -1.0f : value > 1.0f ? 1.0f : value;
    clamped = clamped < -1.0f ? -1.0f : clamped > 1.0f ? 1.0f : clamped;
    return (real)acos(clamped);
}

PRIVATE __forceinline real signed_angle_2c(point2f const *a, point2f const *b, bool reverse)
{
    real angle = clamped_acos_2c(a->y * b->y + a->x * b->x);
    real cross = reverse ? b->x * a->y - b->y * a->x : a->x * b->y - a->y * b->x;
    if (cross < 0.0f)
        angle = 0.0f - angle;
    return angle;
}

// @retail 0x2c2a70
bool function_2c2a70(point2f const *origin, point2f const *endpoint,
    point2f const *first, point2f const *second, point2f const *target)
{
    point2f direction;
    vector2d_from_points2d(origin, endpoint, &direction);
    if (!(normalize_offset_direction_2c(&direction) > 0.0f))
        return true;
    point2f normal = { 0.0f - direction.y, direction.x };
    point2f first_direction, second_direction;
    vector2d_from_points2d(first, origin, &first_direction);
    normalize_offset_direction_2c(&first_direction);
    vector2d_from_points2d(second, origin, &second_direction);
    normalize_offset_direction_2c(&second_direction);
    real first_side = first_direction.y * normal.y + first_direction.x * normal.x;
    real second_side = second_direction.y * normal.y + second_direction.x * normal.x;
    second_side = fabs(second_side) < 0.001 ? 0.0f : second_side;
    first_side = fabs(first_side) < 0.001 ? 0.0f : first_side;
    if (!(first_side * second_side < 0.0f))
    {
        point2f target_first, target_second, endpoint_first, endpoint_second;
        vector2d_from_points2d(first, target, &target_first);
        normalize2d(&target_first);
        vector2d_from_points2d(second, target, &target_second);
        normalize2d(&target_second);
        vector2d_from_points2d(first, endpoint, &endpoint_first);
        normalize2d(&endpoint_first);
        vector2d_from_points2d(second, endpoint, &endpoint_second);
        normalize2d(&endpoint_second);
        real first_target_angle = signed_angle_2c(&target_first, &first_direction, false);
        real first_endpoint_angle = signed_angle_2c(&endpoint_first, &first_direction, true);
        real second_target_angle = signed_angle_2c(&target_second, &second_direction, false);
        real second_endpoint_angle = signed_angle_2c(&endpoint_second, &second_direction, true);
        if (second_endpoint_angle > 0.0f)
            second_endpoint_angle -= 6.2831854820251465f;
        if (first_endpoint_angle < 0.0f)
            first_endpoint_angle += 6.2831854820251465f;
        return 0.0f - (second_target_angle + second_endpoint_angle) > first_target_angle + first_endpoint_angle;
    }
    point2f target_first, endpoint_first, target_second, endpoint_second;
    vector2d_from_points2d(target, first, &target_first);
    vector2d_from_points2d(endpoint, first, &endpoint_first);
    vector2d_from_points2d(target, second, &target_second);
    vector2d_from_points2d(endpoint, second, &endpoint_second);
    real first_distance = (real)sqrt(length_sq2f(&endpoint_first)) + (real)sqrt(length_sq2f(&target_first));
    real second_distance = (real)sqrt(length_sq2f(&endpoint_second)) + (real)sqrt(length_sq2f(&target_second));
    return second_distance > first_distance;
}

#include "globals.h"
#include "path.h"
#include "unknown_20fe20.h"
#include "unknown_26c380.h"

struct s_path_step_view
{
    short type;
    short unknown02;
    long node_index;
    short link_index;
    short unknown0a;
    s_type_c3b527 point;
};
struct s_avoidance_trace
{
    real distance;
    long sector;
    long edge;
    short obstacle;
    short group;
};
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
    short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
    real radius, real distance, bool first, bool stop_at_goal, bool ignore_flagged,
    s_path_location const *location, s_avoidance_trace *trace);
bool function_26c4e0(s_type_c3b527 const *start, s_type_c3b527 const *end,
    s_path_trace_result *trace, s_pathfinding_data *pathfinding,
    long sector_index, long target_sector_index, long location);

// @retail 0x2c2060
void __stdcall function_2c2060(s_type_f17a25 *query, short count, s_path_step_view const *points,
    short *result_count, s_path_step_view *result, bool *complete)
{
    volatile real source_radius = query->source.radius;
    real radius = (real)(source_radius + 0.05);
    s_pathfinding_data *data = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ?
        *(s_pathfinding_data **)((byte *)g_4e0348 + 0xc8) : 0;
    if (!data)
    {
        *result_count = 0;
        *complete = false;
        return;
    }
    if (count <= 1)
    {
        *result_count = 1;
        result[0] = points[0];
        return;
    }
    point2f origin = *(point2f *)&query->source.point.point;
    long sector = points[0].node_index;
    short used = 0;
    bool copied_last = false;
    for (short index = 0; index < count; )
    {
        s_path_step_view const *entry = &points[index];
        short next = NONE;
        long edge = 0xffff;
        bool found = false;
        volatile short output_index;
        if (entry->type != NONE)
            next = index;
        else
        {
            short end = index;
            while (end < count && points[end].type == NONE)
                ++end;
            if (end < count)
            {
                next = end;
                found = true;
            }
            for (short candidate = end - 1; candidate >= index; --candidate)
            {
                point2f direction;
                vector2d_from_points2d(&origin, (point2f *)&points[candidate].point.point, &direction);
                real distance = normalize_offset_direction_2c(&direction);
                if (!(distance > 0.0f))
                    break;
                s_avoidance_trace trace;
                if (!function_26ccb0(data, 0, NONE, &origin, sector, points[candidate].node_index,
                    &direction, radius, distance, false, candidate == count - 1, false, &query->location, &trace))
                    break;
                edge = trace.edge;
                next = candidate;
                found = true;
            }
            if (!found)
            {
                result[used++] = points[count - 1];
                copied_last = true;
                break;
            }
            if (edge != 0xffff && points[next].type == NONE)
            {
                point2f corners[2];
                bool left = function_2c3030((s_pathfinding_data *)query->pathfinding,
                    &origin, radius, edge, true, &corners[0]);
                bool right = function_2c3030((s_pathfinding_data *)query->pathfinding,
                    &origin, radius, edge, false, &corners[1]);
                if (right && left)
                {
                    point2f const *previous = next > 0 ? (point2f *)&points[next - 1].point.point :
                        (point2f *)&query->source.point.point;
                    point2f const *endpoint = (point2f *)&points[next].point.point;
                    bool side = function_2c2a70(previous, endpoint, &corners[0], &corners[1], &origin);
                    point3f start = { origin.x, origin.y, 0.0f };
                    for (short attempt = 0; attempt < 2; ++attempt)
                    {
                        point2f center = corners[side ? 0 : 1];
                        point2f tangents[2], candidate;
                        function_2c2740(&origin, &center, radius, side, &tangents[0]);
                        side = !side;
                        function_2c2740(endpoint, &center, radius, side, &tangents[1]);
                        function_2c2910(&center, tangents, &origin, radius, &candidate);
                        point3f target = { candidate.x, candidate.y, 0.0f };
                        s_path_trace_result first, second;
                        function_26c4e0((s_type_c3b527 *)&start, (s_type_c3b527 *)&target,
                            &first, data, sector, NONE, (long)&query->location);
                        if (!((s_sector_trace_result *)&first)->blocked &&
                            !function_26c4e0((s_type_c3b527 *)&target, &points[next].point,
                                &second, data, ((s_sector_trace_result *)&first)->sector_index,
                                points[next].node_index, (long)&query->location))
                        {
                            sector = ((s_sector_trace_result *)&first)->sector_index;
                            origin = candidate;
                            output_index = points[next].point.output_index;
                            goto emit;
                        }
                    }
                }
            }
        }
        if (next != 0 && next != index)
        {
            origin = *(point2f *)&points[next - 1].point.point;
            sector = points[next - 1].node_index;
            output_index = points[next - 1].point.output_index;
        }
        else
        {
            origin = *(point2f *)&points[next].point.point;
            sector = points[next].node_index;
            output_index = points[next].point.output_index;
            next = index + 1;
        }
    emit:
        s_path_step_view *out = &result[used];
        out->node_index = sector;
        out->point.point.x = origin.x;
        out->point.point.y = origin.y;
        out->point.point.z = entry->point.point.z;
        out->point.output_index = output_index;
        out->unknown02 = entry->unknown02;
        ++used;
        if (entry->type != NONE)
        {
            out->type = entry->type;
            out->link_index = entry->link_index;
        }
        else
        {
            out->type = NONE;
            out->link_index = NONE;
        }
        if (used >= 4)
            break;
        point3f delta;
        s_type_c3b527 const *source = (s_type_c3b527 *)&query->source.point;
        if (source->output_index == out->point.output_index)
        {
            delta.x = out->point.point.x - source->point.x;
            delta.y = out->point.point.y - source->point.y;
            delta.z = out->point.point.z - source->point.z;
        }
        else
        {
            point3f start, end;
            if (source->output_index == NONE || !function_2104b0(source->output_index, &source->point, &start))
                start = source->point;
            if (out->point.output_index == NONE || !function_2104b0(out->point.output_index, &out->point.point, &end))
                end = out->point.point;
            delta.x = end.x - start.x;
            delta.y = end.y - start.y;
            delta.z = end.z - start.z;
        }
        if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x > 400.0f)
            break;
        index = next;
    }
    *result_count = used;
    if (!copied_last)
        *complete = false;
}
