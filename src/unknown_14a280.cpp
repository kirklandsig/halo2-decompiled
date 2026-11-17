#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <math.h>

// @flags /O2 /arch:SSE /Gr

/* a bsp3d: nodes of 8 bytes (a plane index and two 24-bit child indices,
   whose bit 23 marks a leaf) and planes of 16 bytes */
struct s_bsp3d_node
{
	short plane;
	byte children[6];
};

struct s_bsp3d_plane
{
	vector3f normal;
	real distance;
};

struct s_bsp3d
{
	byte unknown00[4];
	s_bsp3d_node *nodes;
	byte unknown08[4];
	s_bsp3d_plane *planes;
};

// @retail 0x14a280
long function_14a280(s_bsp3d *bsp, long index, point3f *point)
{
	do
	{
		s_bsp3d_node *node = &bsp->nodes[index & 0x7fffff];
		s_bsp3d_plane *plane = &bsp->planes[node->plane];
		real distance = plane->normal.k * point->z + plane->normal.j * point->y + plane->normal.i * point->x - plane->distance;
		bool side = distance >= 0.0f;

		index = *(long *)&node->children[side * 3 - 1] >> 8;
	}
	while (!(index & 0x800000));

	if (index != NONE)
		return index & 0x7fffff;
	return NONE;
}

// @retail 0x14a300
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, long plane_index, plane3f *plane)
{
	s_bsp3d_plane *source = &bsp->planes[plane_index & 0x7fff];

	if (plane_index & 0x8000)
	{
		plane->i = 0.0f - source->normal.i;
		plane->j = 0.0f - source->normal.j;
		plane->k = 0.0f - source->normal.k;
		plane->d = 0.0f - source->distance;
	}
	else
	{
		*plane = *(plane3f *)source;
	}
	return plane;
}

/* a structure bsp's portals (0x24 bytes each) and clusters (0xb0 bytes
   each), as the cluster flood fill reads them */
struct s_structure_portal
{
	short front_cluster;
	short back_cluster;
	long plane_index;
	point3f center;
	real radius;
	byte unknown18[4];
	long vertex_count;
	point3f *vertices;
};

struct s_structure_cluster_portals
{
	byte unknown00[0x8c];
	long portal_count;
	short *portal_indices;
	byte unknown94[0xb0 - 0x94];
};

struct s_structure_bsp_portals_view
{
	byte unknown00[0x18];
	s_bsp3d *field_18;
	byte unknown1c[0x60 - 0x1c];
	s_structure_portal *portals;
	byte unknown64[0xa0 - 0x64];
	s_structure_cluster_portals *clusters;
};

/* the structure bsp's collision bsp (unknown_11bed0.cpp) */
extern s_bsp3d *g_4e033c;

/* the flood fill's pass, and the pass that last reached each cluster
   (unknown_180b60.cpp) */
#include "cluster_marks_storage.h"

s_cluster_marks_storage g_cluster_marks_storage;

short function_120850(vector3f const *v);
bool function_23a160(point2f const *point, real radius, short count, point2f const *points);

/* whether a sphere reaches through a portal */
// @retail 0x14a370
bool function_14a370(s_structure_bsp_portals_view const *bsp, point3f const *point, short portal_index, real radius)
{
	s_structure_portal const *portal = &bsp->portals[portal_index];
	real distance = plane_distance_to_point((plane3f const *)&bsp->field_18->planes[portal->plane_index], point);

	if (radius > (real)fabs(distance))
	{
		vector3f offset;
		real portal_radius;

		vector3d_from_points3d(point, &portal->center, &offset);
		portal_radius = portal->radius + radius;
		if (offset.j * offset.j + offset.i * offset.i + offset.k * offset.k < portal_radius * portal_radius)
		{
			plane3f const *structure_plane = (plane3f const *)&g_4e033c->planes[portal->plane_index];
			short axis = function_120850(&structure_plane->n);
			bool positive = structure_plane->n.n[axis] > 0.0f;
			point3f projected;
			point2f point2d;
			point2f vertices[0x80];
			short i;

			projected.x = structure_plane->i * (0.0f - distance) + point->x;
			projected.y = structure_plane->j * (0.0f - distance) + point->y;
			projected.z = structure_plane->k * (0.0f - distance) + point->z;
			point2d.x = projected.n[g_440b94[axis * 2 + positive][0]];
			point2d.y = projected.n[g_440b94[axis * 2 + positive][1]];
			for (i = 0; i < portal->vertex_count; i++)
			{
				point2f *vertex2d = &vertices[i];
				point3f const *vertex = &portal->vertices[i];

				vertex2d->x = vertex->n[g_440b94[axis * 2 + positive][0]];
				vertex2d->y = vertex->n[g_440b94[axis * 2 + positive][1]];
			}
			if (function_23a160(&point2d, (real)sqrt(radius * radius - distance * distance), (short)portal->vertex_count, vertices))
				return true;
		}
	}
	return false;
}

// @retail 0x14a550
short structure_clusters_from_bit_vector(dword const *bits, short *count, short maximum_count, short *clusters)
{
	s_match_globals *bsp = g_4e0348;
	short cluster_count = 0;
	long i;

	*count = cluster_count;
	for (i = 0; i < bsp->list_count; i++)
	{
		if (bits[i >> 5] & (1 << (i & 0x1f)))
		{
			if (cluster_count < maximum_count)
			{
				clusters[cluster_count] = (short)i;
				cluster_count++;
			}
			(*count)++;
		}
	}
	return cluster_count;
}

/* the clusters a sphere reaches from one, through their portals */
// @retail 0x14a5b0
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, short maximum_count, short *clusters)
{
	s_structure_bsp_portals_view *bsp = (s_structure_bsp_portals_view *)g_4e0348;
	s_structure_cluster_portals *cluster = &bsp->clusters[cluster_index];
	short count;
	short i;

	if (maximum_count-- > 0)
	{
		*clusters++ = cluster_index;
	}
	if (g_4e7418[cluster_index] != g_4e7414)
	{
		g_4e7418[cluster_index] = g_4e7414;
	}
	count = 1;
	for (i = 0; i < cluster->portal_count; i++)
	{
		short portal_index = cluster->portal_indices[i];
		s_structure_portal *portal = &bsp->portals[portal_index];
		short other_cluster = portal->front_cluster == cluster_index ? portal->back_cluster : portal->front_cluster;

		if (g_4e7418[other_cluster] != g_4e7414 && function_14a370(bsp, point, portal_index, radius))
		{
			short added = function_14a5b0(other_cluster, point, radius, maximum_count, clusters);

			clusters += added;
			count += added;
			maximum_count -= added;
		}
	}
	return count;
}


/* the clusters a cone reaches from one, through their portals: a portal
   counts when its sphere is within the distance along the direction and
   inside the cone (whose cosine is given) */
// @retail 0x14a6d0
short function_14a6d0(short cluster_index, real cosine, point3f const *point, vector3f const *direction,
	real maximum_distance, real scale, short maximum_count, short *clusters, short *count)
{
	short stack[0x200];
	short stack_count = 0;
	short cluster_count = 0;

	g_4e7414++;
	g_4e7411 = true;
	if (g_4e7418[cluster_index] != g_4e7414)
	{
		g_4e7418[cluster_index] = g_4e7414;
	}
	stack[stack_count++] = cluster_index;
	do
	{
		s_structure_bsp_portals_view *bsp = (s_structure_bsp_portals_view *)g_4e0348;
		short current = stack[--stack_count];
		s_structure_cluster_portals *cluster = &bsp->clusters[current];

		if (cluster_count < maximum_count)
		{
			clusters[cluster_count] = current;
		}
		cluster_count++;
		for (short i = 0; i < cluster->portal_count; i++)
		{
			short *portal_indices = cluster->portal_indices;
			s_structure_portal *portals = bsp->portals;
			s_structure_portal *portal = &portals[portal_indices[i]];
			short other_cluster = portal->front_cluster;

			if (other_cluster == current)
			{
				other_cluster = portal->back_cluster;
			}
			if (g_4e7418[other_cluster] != g_4e7414)
			{
				vector3f offset;
				real distance;
				real radius = portal->radius;

				vector3d_from_points3d(point, &portal->center, &offset);
				distance = direction->k * offset.k + direction->j * offset.j + direction->i * offset.i;
				if (distance >= 0.0f - radius && radius + maximum_distance >= distance &&
					(radius * scale * 2.0f + distance) * distance + radius * radius >= length_sq3f(&offset) * cosine * cosine)
				{
					g_4e7418[other_cluster] = g_4e7414;
					stack[stack_count++] = other_cluster;
				}
			}
		}
	}
	while (stack_count > 0);
	g_4e7411 = false;
	if (count)
	{
		*count = cluster_count;
	}
	if (cluster_count > maximum_count)
	{
		cluster_count = maximum_count;
	}
	return cluster_count;
}

/* the fields of a structure bsp's clusters (0xb0 bytes each) read here */
struct s_cluster_view
{
	byte unknown00[0x6e];
	char sky_index;
	byte unknown6f[0xb0 - 0x6f];
};

struct s_structure_bsp_clusters_view
{
	byte unknown00[0xa0];
	s_cluster_view *clusters;
};

struct s_scenario_sky_reference
{
	byte unknown00[4];
	long sky_index;
};

struct s_scenario_skies_view
{
	byte unknown00[8];
	long sky_count;
	s_scenario_sky_reference *skies;
	byte unknown10[2];
	byte flags;
};

struct s_sky_view
{
	byte unknown00[4];
	long unknown04;
	byte unknown08[8];
	byte flags;
	byte unknown11[0xa0 - 0x11];
	vector3f vector;
};

/* a disk around a point, in the plane of one of the bsp's planes */
struct s_bsp3d_disk
{
	byte unknown00[4];
	long plane_index;
	point3f center;
	real radius;
};

struct s_14b240_owner
{
	byte unknown00[0x18];
	s_bsp3d *bsp;
};

/* the distance from a point to a disk */
// @retail 0x14b240
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point)
{
	s_bsp3d_plane const *plane = &owner->bsp->planes[disk->plane_index];
	vector3f offset;
	vector3f projection;
	real distance;
	real height;
	real result;

	vector3d_from_points3d(&disk->center, point, &offset);
	distance = dot3f(&plane->normal, &offset);
	projection.i = plane->normal.i * distance;
	projection.j = plane->normal.j * distance;
	projection.k = plane->normal.k * distance;
	offset.i -= projection.i;
	offset.j -= projection.j;
	offset.k -= projection.k;
	height = projection.j * plane->normal.j;
	height += projection.k * plane->normal.k;
	height += projection.i * plane->normal.i;
	distance = length_sq3f(&offset);
	if (disk->radius * disk->radius >= distance)
	{
		result = (real)fabs(height);
	}
	else
	{
		result = (real)sqrt((sqrt(distance) - disk->radius) * (sqrt(distance) - disk->radius) + height * height);
	}
	return result;
}

// score 44 (was 44): retail loads the sky count into ebp for the range test
// (cmp edx, ebp) where ours compares with [esi + 8]; retail's ebp then stays
// saved up to the tag-index test, so its NONE preset (or ecx, -1) and that
// test are not jump-threaded as ours are. Tried: a ternary 44, the count in
// a local 44, count > index 44, an inline helper returning NONE early 44,
// (long)index 44, the index lookup moved out of the cluster test 49, a copy
// of the scenario view 101, (short)*sky_index at each use 44, tag_index
// declared first 44, a long copy of the index for the compare 44.
// @retail 0x14b360
bool cluster_get_sky(long cluster_index, long *sky_index, vector3f *vector, bool *found)
{
	bool result = false;
	s_scenario_skies_view *scenario = (s_scenario_skies_view *)g_4e0350;

	*found = result;
	*sky_index = NONE;

	if (cluster_index != NONE)
	{
		*sky_index = ((s_structure_bsp_clusters_view *)g_4e0348)->clusters[cluster_index].sky_index;
		if (*sky_index == NONE && (scenario->flags & 2) && scenario->sky_count > 0)
		{
			*sky_index = 0;
		}

		short index = (short)*sky_index;
		long tag_index = NONE;

		if (index >= 0 && index < scenario->sky_count)
		{
			tag_index = scenario->skies[index].sky_index;
		}

		if (tag_index != NONE)
		{
			s_sky_view *sky = (s_sky_view *)g_4e3b44[tag_index & 0xffff].bytes;

			if (sky && sky->unknown04 != NONE)
			{
				result = true;
				if (sky->flags & 0x20)
				{
					*vector = sky->vector;
					*found = result;
				}
			}
		}
	}
	return result;
}
