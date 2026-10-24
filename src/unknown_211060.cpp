// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_211060.CPP: moves a point through a medium by dt, with up to three
   bounces (docs/unknown_211060.md); the flexible chains (0x1161f0) and the
   contrail points (0x17c540) call it */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "object_queries.h"
#include "unknown_1eb550.h"
#include "unknown_211060.h"

/* the point's physics definition, as read here */
struct s_point_mover_settings
{
	dword flags;
	real unit_mass;
	byte unknown08[0x20 - 0x8];
	real density;
	real drag_unknown8;
	real drag_unknown4;
	real tangential_loss;
	real restitution;
};

/* function_1697c0's result (0x5c bytes; src/unknown_1689b0.cpp) */
struct s_collision_location
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	s_collision_location start_location;
	s_collision_location end_location;
	short material_type;
	byte unknown26[2];
	plane3f plane;
	byte unknown38[0x68 - 0x38];
};

extern real g_4f93ac;
extern real g_4f93b0;

void function_211b70(short index, real const *position, byte flags, vector3f *out);
bool function_211b30(byte const *location, real const *position, byte flags, vector3f *out);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
void function_11bed0(s_location *location, point3f const *point);

// @retail 0x211060
long function_211060(vector3f *velocity, dword flags, void const *mover_settings, s_location *location,
	long cell_slot, point3f *position, vector3f const *force, vector3f *normal, short *material_type, real radius,
	real dt)
{
	long result;
	s_point_mover_settings const *physics = (s_point_mover_settings const *)mover_settings;

	if (dt != 0.0f)
	{
		long collision_flags = 0;
		real unit_mass = physics->unit_mass;
		real radius_squared = radius * radius;
		real radius_cubed = radius_squared * radius;
		long mode = (physics->flags >> 3) & 1;
		if (physics->flags & 0x10)
			mode |= 2;
		else
			mode &= ~2;

		vector3f cell;
		bool medium_unknown4;
		if (flags & 1)
		{
			medium_unknown4 = (flags >> 1) & 1;
			function_211b70((short)cell_slot, position->n, (byte)mode, &cell);
		}
		else
		{
			medium_unknown4 = function_211b30((byte const *)location, position->n, (byte)mode, &cell);
		}

		real total_unit_mass;
		real gravity_scale;
		real coefficient;
		if (medium_unknown4)
		{
			total_unit_mass = g_4f93ac + unit_mass;
			gravity_scale = 1.0f - physics->density * 2.0f / (g_51e9c4->unknown4 + physics->density);
			coefficient = physics->drag_unknown4 * radius_squared;
			result = 2;
		}
		else
		{
			total_unit_mass = g_4f93b0 + unit_mass;
			gravity_scale = 1.0f - physics->density * 2.0f / (g_51e9c4->unknown8 + physics->density);
			coefficient = physics->drag_unknown8 * radius_squared;
			result = 1;
		}
		if (physics->flags & 0x20)
			gravity_scale = 0.0f;

		real drag;
		real mass = total_unit_mass * radius_cubed;
		if (mass != 0.0)
		{
			real scale = dt / mass;
			drag = scale * coefficient;
			if (0.0f > drag)
				drag = 0.0f;
			else if (drag > 1.0f)
				drag = 1.0f;
			if (force)
			{
				velocity->i += force->i * scale;
				velocity->j += force->j * scale;
				velocity->k += force->k * scale;
			}
		}
		else
		{
			drag = coefficient != 0.0f ? 1.0f : 0.0f;
		}

		velocity->k += g_51e9c4->unknown0 * gravity_scale * dt;
		velocity->i += (cell.i - velocity->i) * drag;
		velocity->j += (cell.j - velocity->j) * drag;
		velocity->k += (cell.k - velocity->k) * drag;

		if (physics->flags & 2)
			collision_flags = 0x800005;
		if (physics->flags & 4)
			collision_flags |= 0x800002;
		if (collision_flags == 0 || (flags & 4))
		{
			s_location new_location;

			position->x += velocity->i * dt;
			position->y += velocity->j * dt;
			position->z += velocity->k * dt;
			function_11bed0(&new_location, position);
			if (new_location.leaf_index != NONE)
				*location = new_location;
			return result;
		}

		short bounce = 0;
		while (dt != 0.0f && bounce < 3)
		{
			vector3f displacement;
			s_collision_result_1697c0 collision;

			displacement.i = velocity->i * dt;
			displacement.j = velocity->j * dt;
			displacement.k = velocity->k * dt;
			collision.material_type = NONE;
			if (!function_1697c0(collision_flags, position, &displacement, NONE, NONE, &collision))
			{
				if (collision.end_location.leaf_index != NONE)
					*location = *(s_location *)&collision.end_location;
				*position = collision.point;
				return result;
			}

			real offset = radius > 0.005f ? 0.005f : radius;
			if (collision.type == 2)
				result |= 8;
			else if (collision.type == 1)
				result |= 4;
			if (normal)
				*normal = collision.plane.n;
			if (material_type)
				*material_type = collision.material_type;

			real d = velocity->k * collision.plane.k;
			d += velocity->j * collision.plane.j;
			d += velocity->i * collision.plane.i;
			vector3f along = { d * collision.plane.i, d * collision.plane.j, d * collision.plane.k };
			vector3f tangent = { velocity->i - along.i, velocity->j - along.j, velocity->k - along.k };
			velocity->i = (1.0f - physics->tangential_loss) * tangent.i - physics->restitution * along.i;
			velocity->j = (1.0f - physics->tangential_loss) * tangent.j - physics->restitution * along.j;
			velocity->k = (1.0f - physics->tangential_loss) * tangent.k - physics->restitution * along.k;
			if (collision.end_location.leaf_index != NONE)
				*location = *(s_location *)&collision.end_location;
			position->x = collision.plane.i * offset + collision.point.x;
			position->y = collision.plane.j * offset + collision.point.y;
			position->z = collision.plane.k * offset + collision.point.z;
			bounce++;
			dt -= collision.t * dt;
		}
		return result;
	}
	return 0;
}
