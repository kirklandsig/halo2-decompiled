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

// score 141 (was 269): GA14's 140 form matches all 214 instructions through the hit material store and all 87 after the reflection (exact 1428/1428 B, 0x8c frame, spill slots [esp+0x10]/[esp+0x14]/[esp+0x18], mode movzx, single-exit epilogues, material_type store and dt/bounce order); review left out its two statement-order moves (match-only), 141; only the 47-instruction bounce reflection block differs (ours computes n*d in j,k,i using xmm4/5/6 for n.k/n.j/n.i and spills tangent.k to [esp+0x30]; retail computes d*n in k,i,j using xmm3/4/5, frees xmm6 for v.i and spills along.k to [esp+0x98], 3 B longer). GA14 (149 trials, 269->140 kept): single-exit result=0 with else/break 155 (1424 B); plus dword mode if/else 141 (1428 B); plus material_type before displacement and dt before bounce++ 140 (1428 B; match-only, not kept); reflection forms (all on 140 base unless noted): s_collision_result_1697c0 0x5c/0x60/0x64 166/156/156; single-expression dot 150-177 (1413 B); dot3f/dot_kji helpers 150-177 (1413 B); scalar along/tangent or no tangent 175 (1409 B); no along or tangent 193 (1392 B); tangent*keep-along*rest 140; scalar/two loops 157-204; by-value vector helpers 284 (1521 B); lincomb/scale-sub helpers 195-256; along=plane.n / tangent=*velocity copies 160-359; vector3d_from_points3d / vector3f_add_scaled 140; for loop 344.
// @retail 0x211060
long function_211060(vector3f *velocity, dword flags, void const *mover_settings, s_location *location,
	long cell_slot, point3f *position, vector3f const *force, vector3f *normal, short *material_type, real radius,
	real dt)
{
	long result = 0;
	s_point_mover_settings const *physics = (s_point_mover_settings const *)mover_settings;

	if (dt != 0.0f)
	{
		long collision_flags = 0;
		real unit_mass = physics->unit_mass;
		real radius_squared = radius * radius;
		real radius_cubed = radius_squared * radius;
		dword mode;
		if (physics->flags & 8)
			mode = 1;
		else
			mode = 0;
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
		}
		else
		{
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
					break;
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
		}
	}
	return result;
}
