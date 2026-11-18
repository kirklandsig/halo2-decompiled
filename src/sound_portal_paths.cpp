// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <math.h>
#include <string.h>

struct s_portal_path_portal
{
	short clusters[2];
	long plane_index;
	point3f center;
	real radius;
	dword flags;
	byte unknown1c[8];
};

struct s_portal_path_cluster
{
	byte unknown00[0x72];
	short sound_index;
	short environment_index;
	byte unknown76[2];
	short sound_path_index;
	short environment_path_index;
	byte unknown7c[0xb0 - 0x7c];
};

struct s_portal_path_list
{
	long unknown00;
	long count;
	short *portals;
	byte unknown0c[8];
};

struct s_portal_path_sound
{
	byte unknown00[0x44];
	real range;
	byte unknown48[0x64 - 0x48];
};

struct s_portal_path_environment
{
	byte unknown00[0x28];
	real range;
	byte unknown2c[0x48 - 0x2c];
};

struct s_portal_path_bsp
{
	byte unknown00[0x60];
	s_portal_path_portal *portals;
	byte unknown64[0xa0 - 0x64];
	s_portal_path_cluster *clusters;
	byte unknowna4[0xd4 - 0xa4];
	long sound_count;
	s_portal_path_sound *sounds;
	long environment_count;
	s_portal_path_environment *environments;
	byte unknowne4[0x14c - 0xe4];
	s_portal_path_list *sound_paths;
	long environment_path_count;
	s_portal_path_list *environment_paths;
};

struct s_structure_bsp_view;
struct s_14b240_owner;
struct s_bsp3d_disk;
real function_249d60(s_structure_bsp_view *bsp, long cluster_a, long cluster_b);
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point);
long function_2197f0(real gain);
long __stdcall function_18ae50(long a, long b, real const *values);
typedef bool (__stdcall *t_portal_path_compare)(long, long, void const *);
void sort_4byte(void *unused, long *elements, unsigned long count, t_portal_path_compare compare, void const *context);
extern real g_44a0b4;

static inline void portal_path_set_bit(dword *bits, long index)
{
	dword *word = &bits[index >> 5];
	*word |= 1 << (index & 31);
}

static inline real portal_path_pin(real value)
{
	return 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
}

__forceinline real portal_path_linear_gain(real decibels)
{
	long bits;
	if (decibels < g_44a0b4)
		bits = 0xc2800000;
	else if (decibels > 0.0f)
		bits = 0;
	else
		bits = *(long *)&decibels;
	real scaled = *(real *)&bits * 0.05f;
	return (real)exp(scaled * 2.3025851f);
}

__forceinline long portal_path_gain(real distance, real range)
{
	range = range > 0.001f ? range : 0.001f;
	real t;
	if (fabs(range) < 0.0001f)
		t = distance < range ? 1.0f : 0.0f;
	else
		t = portal_path_pin((distance - range) / (0.0f - range));
	t = portal_path_pin(t);
	real lower = portal_path_linear_gain(-64.0f);
	real upper = portal_path_linear_gain(0.0f);
	real fraction = t * t * 3.0f - t * t * t * 2.0f;
	fraction = portal_path_pin(fraction);
	return function_2197f0((upper - lower) * fraction + lower);
}

// score 311 (was 371): Frame 0x42c versus 0x428; the starting-cluster register and BSP table lifetimes differ.
// Tried: const portal 371; const views 371; endpoint mask before shift 371; const portal and mask 371; shared
// seed array index with separate bit read 373; validity memset 311 kept (plain); path ordinal in while 371;
// index test before closest distance 371.
// @retail 0x18d730
void __stdcall function_18d730(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
	real maximum_range, long *indices, long *gains, long *portals, long *clusters, long *count)
{
	s_portal_path_cluster *cluster = &bsp->clusters[cluster_index];
	dword valid[2];
	real distances[64];
	long nearest_portals[64];
	long order[64];
	long nearest_clusters[64];
	real nearest_distance = FLT_MAX;
	memset(valid, 0, sizeof(valid));
	if (cluster->sound_index != NONE)
	{
		nearest_clusters[cluster->sound_index] = cluster_index;
		distances[cluster->sound_index] = 0.0f;
		nearest_portals[cluster->sound_index] = NONE;
		portal_path_set_bit(valid, cluster->sound_index);
	}
	s_portal_path_list *paths = &bsp->sound_paths[cluster->sound_path_index];
	for (long i = 0; i < paths->count; i++)
	{
		short encoded = paths->portals[i];
		long portal_index = (short)(encoded & 0x7fff);
		s_portal_path_portal *portal = &bsp->portals[portal_index];
		long other_index = portal->clusters[(encoded >> 15) & 1];
		s_portal_path_cluster *other = &bsp->clusters[other_index];
		if (!(portal->flags & 0x28) && other->sound_index != cluster->sound_index)
		{
			real travel = function_249d60((s_structure_bsp_view *)bsp, cluster_index, other_index);
			real distance = function_14b240((s_14b240_owner *)bsp, (s_bsp3d_disk *)portal, point) + travel;
			if (nearest_distance >= distance && cluster->sound_index != NONE)
			{
				nearest_portals[cluster->sound_index] = portal_index;
				nearest_distance = distance;
			}
			if (other->sound_index != NONE)
			{
				s_portal_path_sound *sound = &bsp->sounds[other->sound_index];
				real limit = sound ? sound->range : maximum_range;
				if (limit >= travel)
				{
					if (valid[other->sound_index >> 5] & (1 << (other->sound_index & 31)))
						limit = distances[other->sound_index];
					if (limit >= distance)
					{
						distances[other->sound_index] = distance;
						nearest_clusters[other->sound_index] = other_index;
						nearest_portals[other->sound_index] = portal_index;
						portal_path_set_bit(valid, other->sound_index);
					}
				}
			}
		}
	}
	for (long i = 0; i < bsp->sound_count; i++)
	{
		order[i] = i;
		if (!(valid[i >> 5] & (1 << (i & 31))))
			distances[i] = FLT_MAX;
	}
	real scratch;
	sort_4byte(&scratch, order, bsp->sound_count, (t_portal_path_compare)function_18ae50, distances);
	if (count)
		*count = 0;
	long limit = bsp->sound_count < 64 ? bsp->sound_count : 64;
	for (long i = 0; i < limit; i++)
	{
		long index = order[i];
		if (valid[index >> 5] & (1 << (index & 31)))
		{
			indices[i] = index;
			if (gains)
				gains[i] = index != NONE ? portal_path_gain(distances[index], bsp->sounds[index].range) : 0xc2800000;
			if (portals)
				portals[i] = nearest_portals[index];
			if (clusters)
				clusters[i] = nearest_clusters[index];
			if (count)
				(*count)++;
		}
		else
		{
			if (count)
				break;
			indices[i] = NONE;
			if (gains)
				gains[i] = 0xc2800000;
			if (portals)
				portals[i] = NONE;
			if (clusters)
				clusters[i] = NONE;
		}
	}
}

// score 354 (was 354): Frame 0x430 versus 0x428; the starting-cluster register and portal and cluster table
// lifetimes differ. Tried: const portal 354; const views 354; endpoint mask before shift 354; const portal and
// mask 354; shared seed array index with separate bit read 373; validity memset 364; short endpoint with early
// path increment 371; index test before closest distance 354.
// F417: Invalid output first 353; count-stop return 354; explicit gain arms 365;
// sort cursor with ordinal bound 376; default two-entry limit 354; shared valid row 374;
// sort ordinal after distance write 354; separate output bounds 362; index with sort cursor 360;
// bounded end cursor 332. NOT kept: the end cursor recovers the 0x428 frame but shrinks to
// 1346/1363 bytes and keeps the table bases live. Original indexed output and body restored.
// @retail 0x18dc90
void __stdcall function_18dc90(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
	real maximum_range, long *indices, long *gains, long *portals, long *clusters, long *count)
{
	s_portal_path_cluster *cluster = &bsp->clusters[cluster_index];
	dword valid[2] = { 0, 0 };
	real distances[64];
	long nearest_portals[64];
	long order[64];
	long nearest_clusters[64];
	real nearest_distance = FLT_MAX;
	if (cluster->environment_index != NONE)
	{
		nearest_clusters[cluster->environment_index] = cluster_index;
		distances[cluster->environment_index] = 0.0f;
		nearest_portals[cluster->environment_index] = NONE;
		portal_path_set_bit(valid, cluster->environment_index);
	}
	s_portal_path_list *paths = &bsp->environment_paths[cluster->environment_path_index];
	for (long i = 0; i < paths->count; i++)
	{
		short encoded = paths->portals[i];
		short portal_index = (short)(encoded & 0x7fff);
		s_portal_path_portal *portal = &bsp->portals[portal_index];
		long other_index = portal->clusters[(encoded >> 15) & 1];
		if (portal->flags & 0x28) continue;
		s_portal_path_cluster *other = &bsp->clusters[other_index];
		if (other->environment_index == cluster->environment_index) continue;
		real travel = function_249d60((s_structure_bsp_view *)bsp, cluster_index, other_index);
		real distance = function_14b240((s_14b240_owner *)bsp, (s_bsp3d_disk *)portal, point) + travel;
		if (nearest_distance >= distance && cluster->environment_index != NONE)
		{
			nearest_portals[cluster->environment_index] = portal_index;
			nearest_distance = distance;
		}
		if (other->environment_index != NONE)
		{
			s_portal_path_environment *environment = &bsp->environments[other->environment_index];
			real limit = environment ? environment->range : maximum_range;
			if (limit >= travel)
			{
				if (valid[other->environment_index >> 5] & (1 << (other->environment_index & 31)))
					limit = distances[other->environment_index];
				if (limit >= distance)
				{
					distances[other->environment_index] = distance;
					nearest_clusters[other->environment_index] = other_index;
					nearest_portals[other->environment_index] = portal_index;
					portal_path_set_bit(valid, other->environment_index);
				}
			}
		}
	}
	for (long i = 0; i < bsp->environment_count; i++)
	{
		order[i] = i;
		if (!(valid[i >> 5] & (1 << (i & 31))))
			distances[i] = FLT_MAX;
	}
	real scratch;
	sort_4byte(&scratch, order, bsp->environment_count, (t_portal_path_compare)function_18ae50, distances);
	if (count)
		*count = 0;
	long limit = bsp->environment_count < 2 ? bsp->environment_count : 2;
	for (long i = 0; i < limit; i++)
	{
		long index = order[i];
		if (valid[index >> 5] & (1 << (index & 31)))
		{
			indices[i] = index;
			if (gains)
				gains[i] = index != NONE ? portal_path_gain(distances[index], bsp->environments[index].range) : 0xc2800000;
			if (portals)
				portals[i] = nearest_portals[index];
			if (clusters)
				clusters[i] = nearest_clusters[index];
			if (count)
				(*count)++;
		}
		else
		{
			if (count)
				break;
			indices[i] = NONE;
			if (gains)
				gains[i] = 0xc2800000;
			if (portals)
				portals[i] = NONE;
			if (clusters)
				clusters[i] = NONE;
		}
	}
}
