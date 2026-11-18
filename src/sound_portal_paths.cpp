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

// score 296 (was 311): The 0x428 frame agrees. The starting-cluster register, table loads, validity words
// and sort scratch offset differ.
// Before the sort scope, the frame was 0x42c.
// Tried: const portal 371; const views 371; endpoint mask before shift 371; const portal and mask 371; shared
// seed array index with separate bit read 373; validity memset 311 kept (plain); path ordinal in while 371;
// index test before closest distance 371.
// Round 605. The sort scratch scoped to its call scores 296 and restores the four 64-entry array offsets (plain
// confirmed). Bitmap clear before cluster 312. Closest distance after seed 308. Search and sort scopes 296.
// Early scratch 311. Named ordinals 311. Bounded output count 311. Shared output element 311. Explicit gain
// arms 302 (NOT kept: frame 0x434). Search scope 311. Sort scope with gain arms 302 (NOT kept: frame 0x430).
// Round 628: signed portal 363, both ordinals signed short 363, const path-list reference 296,
// integer sort scratch 296, aggregate validity clear 368, shared destination sound index 355,
// shared seed sound index 367, long packed link 317, sort validity before ordinal 299.
// The entry still uses EBX for the starting cluster and spills both table bases.
// The starting cluster still occupies EBX, and both table bases are kept across the portal loop.
// Shared bitmap setter/range test/order-output tests/all operations 296/317/296/317.
// Seed-and-walk cluster scope 298; early closest distance 296; signed starting sound at the
// closest test 296; signed destination through range/bitmap 317, with shared test 317.
// Closest distance, valid bitmap and starting distance names 296/296/296.
// Round 422: Explicit portal table / cluster table / both / pointer addition 296/296/296/296.
// Sort-preparation count snapshot 296, capacity-first limit 296, selected comparison distance 296,
// default range with an override 351. The table spills, starting-cluster and validity homes remain.
// Portal/destination record references, separately and together, 296/296/296. Workspace before starting-
// cluster lookup 296, early path-list lookup 358, packed-entry reference 341, nested flag and sound
// gates 296, destination lookup inside the flag gate 328. The early list and late destination forms
// recover EBP at entry, with diffs at +0x28 and higher scores.
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
	{
		real scratch;
		sort_4byte(&scratch, order, bsp->sound_count, (t_portal_path_compare)function_18ae50, distances);
	}
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

// score 354 (was 354): Frame 0x430 versus 0x428. Retail keeps the starting cluster in EBP and reloads
// both BSP table bases inside the portal loop. Tried: const portal 354; const views 354; endpoint mask
// before shift 354; const portal and mask 354; shared seed array index with separate bit read 373; validity memset 364; short endpoint with early
// path increment 371; index test before closest distance 354.
// F417: Invalid output first 353; count-stop return 354; explicit gain arms 365;
// sort cursor with ordinal bound 376; default two-entry limit 354; shared valid row 374;
// sort ordinal after distance write 354; separate output bounds 362; index with sort cursor 360;
// bounded end cursor 332. NOT kept: the end cursor recovers the 0x428 frame but shrinks to
// 1346/1363 bytes and keeps the table bases live. Original indexed output and body restored.
// AC571: Combined rejecting guard / combined accepting guard / same-environment guard within the open-portal
// block / eligible path inside the flag guard 368/368/368/354. Signed endpoint test 373, with a short cluster
// index 354 at retail length. One destination index through the cap with fresh publication 333, and a separate
// read at the cap 354. NOT kept: 333 shortens to 1347 and retains frame 0x430; the mixed reads do not match.
// The short cluster form reaches frame 0x42c. Both BSP table reloads and the 0x428 frame still differ.
// Path ordinal, destination ordinal, path list and minimum named separately and together 354 each.
// Uninitialized arrays before the valid bits, minimum between the bits and arrays, or bit clear
// before the initial cluster lookup 354 each. The 0x430 frame and cached table bases remain.
// Round 623: narrow destination before flags 374, in nested gates 374; unsigned packed link and
// cluster before flags 373; one destination environment after distance calls 363; rejecting range gates
// 354, selected distance 354, comparison selected by the validity bit 356; one starting environment 326.
// NOT kept: retail rereads the starting environment, and the 326 form retains the 0x430 frame.
// Round 625: one path count 323, one entry array 356, both 323, packed-entry range 356;
// portal distance before addition 354, total in the travel result carrier 352 at 1363 bytes,
// nearest distance before portal 354, late order array and integer sort scratch 354.
// NOT kept: both 323 forms keep the count snapshot where retail reloads the count and retain the 0x430 frame.
// The 352 form gains two. The path table spills and starting-cluster register still differ.
// Round 628: scoped sort scratch 354, scoped integer scratch 354, scoped scratch with validity
// memset 365, and the paired clear/sort/positive guard 364 with the 0x428 frame.
// Separate traversal scope 354, chained validity words 354, long packed link 351,
// late distance initialization 358, scoped scratch and explicit output cap 363.
// NOT kept: the 351 form gains three, with the 0x430 frame still present.
// Advance the gain, portal or cluster output pointer at each row store 368/355/357.
// Cursor pairs 350/350/365, all optional cursors 359, every output cursor 359.
// NOT kept: the two 350 forms recover the 0x428 frame at 1368 bytes and gain four.
// Both BSP table bases still spill and the starting cluster remains in EDI.
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
