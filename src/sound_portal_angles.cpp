// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

struct s_sound_portal
{
	short clusters[2];
	long plane_index;
	point3f center;
	real radius;
	byte unknown18[0xc];
};

struct s_sound_portal_bsp
{
	byte unknown00[0x60];
	s_sound_portal *portals;
};

struct s_sound_listener_rotation
{
	byte unknown00[0xc];
	matrix3x3 rotation;
};

struct s_sound_portal_angles
{
	real lower;
	real upper;
	real horizontal_scale;
	real vertical_scale;
};

struct s_14b240_owner;
struct s_bsp3d_disk;
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point);
real function_50650(real cosine);

// score 20 (was 20): the whole block is one consistent XMM4/XMM5 swap: ours loads the
// offset's k into xmm5 and its i into xmm4, retail loads them the other way round. Ours gives
// XMM4 to whichever of i/k is computed first before the call; retail computes i first (with k
// last, through the portal pointer) and still gives k XMM4. Round 23: offset statements i,j,k
// 38; k,j,i 43; i,k,j 38; j,k,i 21 (the reloads become retail's, the pre-call order breaks);
// k,i,j 38; j,k,i without the point address read 22; the radius clamp as a ternary 20.
// Round 29 (lane 14): the radius and the else arm's zero only follow the offset colouring (the
// radius takes the register the last product frees); j,k,i gives retail's reloads but hoists
// `left.k * k` and the radius load. `distance < radius` 20 (same code); `radius <= 0.001f` 21;
// the negation arm first 30; the portal pointer before the NONE test 199; j,k,i with the call
// reading `point` directly 21.
// Round 57 (m3 lane 1): the first-assigned component stays in XMM7, the second gets XMM4 and the
// third XMM5 at the reload; declaration order doesn't move it. vector3d_from_points3d for the
// offset (with *point_reference / point) 38/38 (i stays in XMM7); scalar offsets declared j,k,i /
// k,j,i / i,j,k and assigned j,i,k 24/24/24.
// @retail 0x18ab70
void function_18ab70(s_sound_portal_bsp const *bsp, long portal_index, s_sound_listener_rotation const *listener,
	bool reverse, point3f const *point, real radius, s_sound_portal_angles *angles)
{
	point3f const *const *point_reference = &point;
	if (portal_index != NONE)
	{
		s_sound_portal const *portal = &bsp->portals[portal_index];
		vector3f offset;
		offset.j = (*point_reference)->y - portal->center.y;
		offset.i = (*point_reference)->x - portal->center.x;
		offset.k = (*point_reference)->z - portal->center.z;
		real distance = function_14b240((s_14b240_owner const *)bsp, (s_bsp3d_disk const *)portal, *point_reference);
		vector3f direction;
		direction.i = listener->rotation.forward.k * offset.k;
		direction.i += listener->rotation.forward.j * offset.j;
		direction.i += listener->rotation.forward.i * offset.i;
		direction.j = listener->rotation.left.i * offset.i;
		direction.j += listener->rotation.left.k * offset.k;
		direction.j += listener->rotation.left.j * offset.j;
		direction.k = listener->rotation.up.j * offset.j;
		direction.k += listener->rotation.up.i * offset.i;
		direction.k += listener->rotation.up.k * offset.k;
		if (radius > distance)
		{
			if (reverse)
				distance = -distance;
			else
			{
				direction.i = -direction.i;
				direction.j = -direction.j;
				direction.k = -direction.k;
			}
			if (!(radius > 0.001f))
				radius = 0.001f;
			real ratio = distance / radius;
			ratio = ratio < -1.0f ? -1.0f : (ratio > 1.0f ? 1.0f : ratio);
			real sphere_angle = function_50650(ratio);
			real disk_angle = (real)atan2(portal->radius, distance);
			real azimuth = (real)atan2(direction.j, direction.i);
			real angle;
			if (reverse)
				angle = sphere_angle;
			else
			{
				real fade = (real)(1.0f - fabs(ratio));
				fade = fade < 0.0f ? 0.0f : (fade > 1.0f ? 1.0f : fade);
				angle = (disk_angle > sphere_angle ? disk_angle : sphere_angle) * fade;
			}
			angles->lower = azimuth - angle;
			angles->upper = azimuth + angle;
			if (angles->lower < -6.2831855f)
			{
				angles->lower += 6.2831855f;
				angles->upper += 6.2831855f;
			}
			else if (angles->upper > 6.2831855f)
			{
				angles->lower -= 6.2831855f;
				angles->upper -= 6.2831855f;
			}
			if (reverse)
				angles->vertical_scale = (real)((1.0f + fabs(ratio)) * 0.5f);
			else
				angles->vertical_scale = (real)((1.0f - fabs(ratio)) * 0.5f);
			angles->vertical_scale = (real)(fabs(direction.k) * angles->vertical_scale);
			angles->horizontal_scale = (real)(1.0f - fabs(direction.k));
			return;
		}
	}
	angles->lower = 0.0f;
	angles->upper = 6.2831855f;
	angles->horizontal_scale = reverse ? 1.0f : 0.0f;
	angles->vertical_scale = 0.0f;
}
