// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17D9A0.CPP: decal placement and rendering (0x17d9a0 onwards) */

#include "unknown_11c920.h"
#include "vector_products.h"
#include "vector2f_products.h"
#include "segment_sphere.h"
#include "unknown_0259d0.h"
#include <float.h>
#include "globals.h"
#include "unknown_03bcb0.h"
#include "unknown_1428b0.h"
#include <string.h>
#include <math.h>

#include "cluster_marks_storage.h"
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, short maximum_count, short *clusters);

struct s_decal_cluster_17e490
{
	byte unknown00[0x98];
	long count;
	short *indices;
	byte unknowna0[0x10];
};

struct s_decal_bound_17e490
{
	byte unknown00[0x3c];
	point3f center;
	real radius;
	byte unknown4c[0xc];
};

struct s_decal_bsp_17e490
{
	byte unknown00[0xa0];
	s_decal_cluster_17e490 *clusters;
	byte unknowna4[0xa0];
	s_decal_bound_17e490 *bounds;
};

// @retail 0x17e490
long function_17e490(long excluded, short cluster_index, point3f const *point, real radius, long maximum_count, s_decal_bound_17e490 **output)
{
	short clusters[512];
	short count = 0;
	if (cluster_index != NONE)
	{
		if (radius > 0.0f)
		{
			g_4e7414++;
			g_4e7411 = true;
			count = function_14a5b0(cluster_index, point, radius, 512, clusters);
			g_4e7411 = false;
			if (count > 512)
				count = 512;
		}
		else
		{
			clusters[0] = cluster_index;
			count = 1;
		}
	}
	dword visited[32];
	memset(visited, 0, sizeof(visited));
	long result = 0;
	if (excluded != NONE)
		visited[excluded >> 5] |= 1 << (excluded & 31);
	for (long i = 0; i < count; ++i)
	{
		s_decal_cluster_17e490 *cluster = &((s_decal_bsp_17e490 *)g_4e0348)->clusters[clusters[i]];
		for (long j = 0; j < cluster->count; ++j)
		{
			long index = cluster->indices[j];
			dword bit = 1 << (index & 31);
			if (!(visited[index >> 5] & bit) && result < maximum_count)
			{
				s_decal_bound_17e490 *bound = &((s_decal_bsp_17e490 *)g_4e0348)->bounds[index];
				real x = point->x - bound->center.x;
				real z = point->z - bound->center.z;
				real y = point->y - bound->center.y;
				real distance = x * x + z * z + y * y;
				real total_radius = bound->radius + radius;
				if (total_radius * total_radius >= distance)
				{
					output[result++] = bound;
					visited[index >> 5] |= bit;
				}
			}
		}
	}
	return result;
}

struct s_decal_mesh_face
{
	short plane_index;
	word first_edge;
	byte unknown04[4];
};

struct s_decal_quad_17d9a0
{
	transform4x3f matrix;
	box2f bounds;
	plane3f plane;
	short axis;
	bool positive;
	byte unknown57;
	point2f corners[4];
	vector2f edges[2];
	real inverse_determinant;
};

short function_120850(vector3f const *v);

inline void point2f_set(point2f *point, real x, real y);

/* the quad corner at (bounds[x], bounds[y]) on the matrix's forward/left plane */
PRIVATE inline real *decal_corner_point_17d9a0(transform4x3f const *matrix, real const *bounds, long x, long y, real *point)
{
	point[0] = matrix->forward.i * bounds[x] + matrix->left.i * bounds[y] + matrix->position.x;
	point[1] = matrix->left.j * bounds[y] + matrix->forward.j * bounds[x] + matrix->position.y;
	point[2] = matrix->forward.k * bounds[x] + matrix->left.k * bounds[y] + matrix->position.z;
	return point;
}

/* drops the axis the plane faces: the two remaining coordinates, in the table's order */
PRIVATE inline void decal_project_point_17d9a0(real const *point, short axis, bool side, point2f *out)
{
	point2f_set(out, point[g_440b94[axis * 2 + side][0]], point[g_440b94[axis * 2 + side][1]]);
}

/* the plane through a point with the given normal */
PRIVATE inline void decal_plane_17d9a0(plane3f *plane, vector3f const *normal, point3f const *point)
{
	plane->n = *normal;
	plane->d = dot3f(&plane->n, (vector3f const *)point);
}

// @retail 0x17d9a0
void function_17d9a0(transform4x3f const *matrix, real const *bounds, s_decal_quad_17d9a0 *quad)
{
	real point[3];
	quad->matrix = *matrix;
	quad->bounds = *(box2f const *)bounds;
	decal_plane_17d9a0(&quad->plane, &matrix->up, &matrix->position);
	quad->axis = function_120850(&quad->plane.n);
	quad->positive = ((real const *)&quad->plane.n)[quad->axis] > 0.0f;
	decal_project_point_17d9a0(decal_corner_point_17d9a0(matrix, bounds, 0, 2, point), quad->axis, quad->positive, &quad->corners[0]);
	decal_project_point_17d9a0(decal_corner_point_17d9a0(matrix, bounds, 1, 2, point), quad->axis, quad->positive, &quad->corners[1]);
	decal_project_point_17d9a0(decal_corner_point_17d9a0(matrix, bounds, 1, 3, point), quad->axis, quad->positive, &quad->corners[2]);
	decal_project_point_17d9a0(decal_corner_point_17d9a0(matrix, bounds, 0, 3, point), quad->axis, quad->positive, &quad->corners[3]);
	vector2d_from_points2d(&quad->corners[0], &quad->corners[1], (point2f *)&quad->edges[0]);
	vector2d_from_points2d(&quad->corners[0], &quad->corners[3], (point2f *)&quad->edges[1]);
	if (quad->edges[0].i * quad->edges[0].i + quad->edges[0].j * quad->edges[0].j > 0.0001f &&
		quad->edges[1].i * quad->edges[1].i + quad->edges[1].j * quad->edges[1].j > 0.0001f)
		quad->inverse_determinant = 1.0f / cross_product2d(&quad->edges[0], &quad->edges[1]);
	else
		quad->inverse_determinant = 0.0f;
}

struct s_decal_mesh_edge
{
	word vertices[2];
	word next_edges[2];
	short faces[2];
};

struct s_decal_mesh_vertex
{
	point3f position;
	byte unknown0c[4];
};

struct s_decal_mesh_view
{
	byte unknown00[0xc];
	plane3f const *planes;
	byte unknown10[0x18];
	long face_count;
	s_decal_mesh_face const *faces;
	long edge_count;
	s_decal_mesh_edge const *edges;
	long vertex_count;
	s_decal_mesh_vertex const *vertices;
};

// @retail 0x17d900
real function_17d900(s_decal_mesh_view const *mesh, long face_index, plane3f const *plane)
{
	s_decal_mesh_face const *face = &mesh->faces[face_index];
	real minimum = FLT_MAX;
	long edge_index = face->first_edge;
	s_decal_mesh_face const *first_face = face;

	do
	{
		s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
		bool reverse = edge->faces[1] == face_index;
		real distance = (real)fabs(plane_distance_to_point(plane, &mesh->vertices[edge->vertices[!reverse]].position));

		if (!(distance > minimum))
			minimum = distance;
		edge_index = edge->next_edges[reverse];
	} while (edge_index != first_face->first_edge);
	return minimum;
}

struct s_decal_vertex_17dd80
{
	point3f position;
	point2f texture;
};

struct s_decal_output_17dd80
{
	s_decal_vertex_17dd80 vertices[1024];
	short vertex_count;
	short polygon_sizes[1024];
	short polygon_count;
};

struct s_decal_limits_17dd80
{
	real maximum_angle;
	real adjacent_angle;
	real radius_scale;
	long flags;
};

PRIVATE s_decal_limits_17dd80 const g_444a38[4] =
{
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 40.0f, 110.0f, 1.5f, 1 },
	{ 10.0f, 10.0f, 1.5f, 0 }
};
point2f g_55ec10[2][12];

struct s_bsp3d;
struct plane2f { vector2f n; real d; };
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, long plane_index, plane3f *plane);
real function_11cf50(vector3f const *a, vector3f const *b);
real *function_120790(real *out, real const *plane, real const *point, short axis, byte side);
short function_23a6f0(point2f *output, short count, point2f const *points,
	plane2f const *plane, short capacity, dword *point_mask, bool *clipped, real epsilon);

PRIVATE __forceinline void decal_add_adjacent_17dd80(s_decal_mesh_view const *mesh, s_decal_mesh_edge const *edge,
	bool reverse, s_decal_quad_17d9a0 const *quad, real radius, short mode, long *adjacent, short *count)
{
	if (*count < 1024)
	{
		point3f const *start = &mesh->vertices[edge->vertices[!reverse]].position;
		point3f const *end = &mesh->vertices[edge->vertices[reverse]].position;
		vector3f direction;
		direction.i = end->x - start->x;
		direction.j = end->y - start->y;
		direction.k = end->z - start->z;
		if (function_11e5e0(start, &direction, &quad->matrix.position, g_444a38[mode].radius_scale * radius))
		{
			long face = edge->faces[!reverse];
			short i = 0;
			while (face != NONE && i < *count)
			{
				if (adjacent[i] == face)
					face = NONE;
				++i;
			}
			if (face != NONE)
				adjacent[(*count)++] = face;
		}
	}
}

// @retail 0x17dd80
void __stdcall function_17dd80(s_decal_mesh_view const *mesh, s_decal_quad_17d9a0 const *quad,
	s_decal_output_17dd80 *output, long face_index, bool gather, real radius, short mode,
	long *adjacent, short *adjacent_count, long *secondary, short *secondary_count)
{
	if (face_index == NONE)
		return;
	s_decal_mesh_face const *face = &mesh->faces[face_index];
	short count, other_count;
	if (gather)
	{
		count = *adjacent_count;
		other_count = *secondary_count;
	}
	plane3f plane;
	bsp3d_get_plane((s_bsp3d const *)mesh, face->plane_index, &plane);
	real angle = function_11cf50(&quad->plane.n, &plane.n);
	if (g_444a38[mode].maximum_angle * 0.017453292f >= angle &&
		radius * 0.05f > function_17d900(mesh, face_index, &quad->plane))
	{
		long edge_index = face->first_edge;
		short iteration = 0;
		point2f const *points = quad->corners;
		short point_count = 4;
		dword point_mask = 0;
		point2f previous;
		point2f *polygon_points;
		do
		{
			s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
			bool reverse = edge->faces[1] == face_index;
			point3f const *vertex = &mesh->vertices[edge->vertices[!reverse]].position;
			polygon_points = g_55ec10[iteration & 1];
			short const *axes = g_440b94[quad->axis * 2 + quad->positive];
			if (iteration == 0)
			{
				point3f const *first = &mesh->vertices[edge->vertices[reverse]].position;
				previous.x = ((real const *)first)[axes[0]];
				previous.y = ((real const *)first)[axes[1]];
			}
			point2f current;
			current.x = ((real const *)vertex)[axes[0]];
			current.y = ((real const *)vertex)[axes[1]];
			plane2f clip;
			clip.n.i = previous.y - current.y;
			clip.n.j = current.x - previous.x;
			real length = (real)sqrt(clip.n.i * clip.n.i + clip.n.j * clip.n.j);
			if (!(fabs(length) < 0.0001f))
			{
				real inverse = 1.0f / length;
				clip.n.i *= inverse;
				clip.n.j *= inverse;
			}
			else
				length = 0.0f;
			if (length != 0.0f)
			{
				clip.d = clip.n.j * current.y + clip.n.i * current.x;
				bool clipped;
				point_count = function_23a6f0(polygon_points, point_count, points, &clip, 12, &point_mask, &clipped, 0.0f);
				if (gather && clipped)
					decal_add_adjacent_17dd80(mesh, edge, reverse, quad, radius, mode, adjacent, &count);
			}
			else
			{
				clip.d = 0.0f;
				point_count = 0;
			}
			++iteration;
			previous = current;
			edge_index = edge->next_edges[reverse];
			points = polygon_points;
		} while (edge_index != face->first_edge && point_count > 0);
		if (point_count >= 3 && point_count <= 1024 - output->vertex_count && !(face->unknown04[0] & 0x2b))
		{
			output->polygon_sizes[output->polygon_count++] = point_count;
			for (short i = 0; i < point_count; ++i)
			{
				real x = polygon_points[i].x - quad->corners[0].x;
				real y = polygon_points[i].y - quad->corners[0].y;
				real v = 0.0f - (quad->edges[0].j * x - quad->edges[0].i * y) * quad->inverse_determinant;
				real u = (x * quad->edges[1].j - quad->edges[1].i * y) * quad->inverse_determinant;
				output->vertices[output->vertex_count].texture.x = u;
				output->vertices[output->vertex_count].texture.y = v;
				function_120790((real *)&output->vertices[output->vertex_count].position,
					(real const *)&plane, (real const *)&polygon_points[i], quad->axis, quad->positive);
				if (!(point_mask & (1 << i)))
				{
					point3f *position = &output->vertices[output->vertex_count].position;
					position->x += plane.n.i * 0.00390625f;
					position->y += plane.n.j * 0.00390625f;
					position->z += plane.n.k * 0.00390625f;
				}
				++output->vertex_count;
			}
		}
	}
	else
	{
		if (!gather)
			return;
		long edge_index = face->first_edge;
		do
		{
			s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
			bool reverse = edge->faces[1] == face_index;
			decal_add_adjacent_17dd80(mesh, edge, reverse, quad, radius, mode, adjacent, &count);
			edge_index = edge->next_edges[reverse];
		} while (edge_index != face->first_edge);
		if (g_444a38[mode].adjacent_angle * 0.017453292f >= angle && other_count < 1024)
			secondary[other_count++] = face_index;
	}
	if (gather)
	{
		*adjacent_count = count;
		*secondary_count = other_count;
	}
}

/* the state a decal is placed with (0x5c bytes) */
struct s_decal_placement
{
	long unknown00;
	long unknown04;
	point3f position;
	long unknown14;
	long unknown18;
	long unknown1c;
	long unknown20;
	short unknown24;
	byte unknown26[2];
	plane3f plane;
	long unknown38;
	long unknown3c;
	long unknown40;
	short unknown44;
	short unknown46;
	long unknown48;
	long unknown4c;
	long unknown50;
	long unknown54;
	byte unknown58;
	byte unknown59;
	short unknown5a;
};

// @retail 0x17ed70
void decal_placement_copy(s_decal_placement const *in, s_decal_placement *out)
{
	out->unknown00 = in->unknown00;
	out->unknown04 = in->unknown04;
	out->position = in->position;
	out->unknown14 = in->unknown14;
	out->unknown18 = in->unknown18;
	out->unknown1c = in->unknown1c;
	out->unknown20 = in->unknown20;
	out->unknown24 = in->unknown24;
	out->plane = in->plane;
	out->unknown38 = in->unknown38;
	out->unknown3c = in->unknown3c;
	out->unknown40 = in->unknown40;
	out->unknown44 = in->unknown44;
	out->unknown46 = in->unknown46;
	out->unknown48 = in->unknown48;
	out->unknown4c = in->unknown4c;
	out->unknown50 = in->unknown50;
	out->unknown54 = in->unknown54;
	out->unknown58 = in->unknown58;
	out->unknown59 = in->unknown59;
	out->unknown5a = in->unknown5a;
}

struct s_decal_projection_17fd20
{
	s_decal_quad_17d9a0 quad;
	real orientation_bounds[6];
	real radius;
	transform4x3f matrix;
	box2f bounds;
};

PRIVATE __forceinline void decal_face_plane_17fd20(s_decal_mesh_view const *mesh, long face_index, plane3f *plane)
{
	long index = mesh->faces[face_index].plane_index;
	plane3f const *source = &mesh->planes[index & 0x7fff];
	if (index & 0x8000)
	{
		plane->n.i = 0.0f - source->n.i;
		plane->n.j = 0.0f - source->n.j;
		plane->n.k = 0.0f - source->n.k;
		plane->d = 0.0f - source->d;
	}
	else
		*plane = *source;
}

real function_50650(real cosine);

PRIVATE __forceinline real decal_normal_angle_17fd20(vector3f const *a, vector3f const *b)
{
	if (*(dword const *)&a->i == *(dword const *)&b->i &&
		*(dword const *)&a->j == *(dword const *)&b->j &&
		*(dword const *)&a->k == *(dword const *)&b->k)
		return 0.0f;
	real cosine = a->i * b->i + a->k * b->k + a->j * b->j;
	cosine = cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
	return function_50650(cosine);
}

PRIVATE __forceinline void decal_rotate_vector_17fd20(vector3f const *forward, vector3f const *left,
	vector3f const *up, vector3f const *in, vector3f *out)
{
	vector3f rotated;
	rotated.i = up->i * in->k + left->i * in->j + forward->i * in->i;
	rotated.j = up->j * in->k + left->j * in->j + forward->j * in->i;
	rotated.k = up->k * in->k + left->k * in->j + forward->k * in->i;
	*out = rotated;
}

// MiniMax A (11-14): no new form, no lower score (793 on the reduced image): the frame is 0x21d8 against retail's
// 0x21dc, and the entry register roles differ - retail takes the object pointer in edx and the table global in ecx,
// while this build reads the second argument into ecx and a global into edx - so every later home sits one slot off.
// score 535 (was 535): earlier 539; frame 0x21dc and 3647-byte length agree; first +0x41 spills the tag
// MiniMax E (11-11): no new form, no lower score (535 on the full build, 834 on the reduced image):
// the frame is still 0x21d8 against retail's 0x21dc and the locals still sit in other stack homes.
// w14H (11-06, fast, plain): statement-by-statement walk against retail found no logic difference (fields, offsets,
// condition directions, callee arguments, loop shapes, rotation coefficients and bounds all agree). The angle helper's second
// clamp is now the existing function_50650 (retail's inlined clamped arccosine) instead of its body written out: fast 795 -> 793,
// 3672 bytes both, callers/callees unchanged; the full build is unconfirmed. normalize3d for the axis 812 (its dot3f sums i, j, k;
// retail sums i, k, j) NOT kept. flags is read as a byte (retail 0x17fdd2, +0xc of the 16-byte entry) through the cast.
// MiniMax E (11-05): no new form, no lower score; the first +0x41 tag spill, the plane base, the first
// selectors and the product memory sides are as the earlier rounds left them.
// MiniMax A (11-13): no new form, no lower score (535 plain, 834 on the reduced image); the frame and the
// length still agree, the tag still spills at +0xb4 against retail's +0x94, and the count, plane and
// rotation homes still differ.
// Frame 0x21dc and length 3647 agree; tag/count/plane homes and rotation spills remain.
// Earlier: 552 (was 564): frame 0x21dc and 3647-byte length agree; first +0x41 spills the tag
// at +0xb4 versus retail +0x94. Count/plane homes, initial face-call scheduling and rotation spills remain.
// Earlier: frame 0x21dc and retail length agree; definition spill +0xac versus +0x94, count/plane homes and rotation spills remain.
// Plane/count homes, initial face-call scheduling and rotation spills remain.
// Earlier aligned frame 0x21e4 versus 0x21dc, count/plane/axis homes and rotation scheduling remain.
// The axis uses vector3d_from_points3d with length summed in retail's i, k, j order (written j, k, i), which stores all three components before the sum as retail
// does; that frame is 0x21e4 against retail's 0x21dc (8 bytes more below the plane), 3631 bytes against
// 3647. The definition/output registers agree with the pivot offset rotated in place, the mode read at
// each use and the plane flip tested as bit 15. Retail increments the first loop's index before its
// call; the count and plane spill slots sit in a different order; retail preloads the axis j/k
// components for the scale; the rotation schedule differs after that. Round 30 (fast, on the kept
// form): the scale through vector3f_scale 572; the offset rotated inside the matrix position through a
// pointer 619; axis declared with the selected points 572; the sign through dot3f 632; min/max stored
// before the end copy 571 (store order as retail, count unchanged). Before the keep: counts declared
// face count first 587 (store order as retail); the subtraction helper on the `faces[i++]` loop 566,
// with the j, k, i sum 560.
// Round 12 (lane 10): retail computes the pivot offset straight into the matrix position (and
// copies only the i/j components it still needs), rotates it there in place, writes the forward/left/up
// rotations directly into the matrix and adds the start point last. The offset in the matrix position
// with the rotate helper copying its input vector first, then the start added in place: 588 at 3647
// bytes and frame 0x21dc (plain; slot order and the inner zero register XMM1/XMM2 then differ; our
// aggregate copy goes through integer registers where retail forwards the float values). Direct
// writes with the offset kept separate 763 (3455 bytes, frame 0x21ac); the input copied by component
// 700, into scalar locals 776; min/max stored before the end copy 587 (same code). All restored.
// Round 10 (lane 10): rotated position kept in its own vector 650 (frame 0x21ec); rotate helper
// writing its output directly 894 (frame 0x21c4); offset in place alone 717, with the mode at each
// use 762 (frame 0x21dc), plus the bit-15 flip 593, plus the counts declared after faces[0] 587
// (kept; either order); the bit-15 flip alone 761; grouping plane in its own block 650 (same code);
// count locals renamed 587 (slots unchanged); first loop as `while` with `faces[i++]` 574 (not kept:
// a respelled loop); the axis through normalize3d 712, plus vector3d_from_points3d 667.
// Round 8 (lane 10): counts set after faces[0] 716; float component equality 762 (ucomiss, but the
// cosine products then follow retail's order); dword component equality without the copy 650 (kept);
// short plane index 698 (test cx); mode read from the definition at each use 705 (registers, loop
// entry jump and final call as retail, rest worse; retail does read it at each use); rotation in the
// sine-axis/t*j*i/t*k shape 731 (x87 product as retail), its square and sine-product statement orders
// 731-740; with the mode at each use 717; matrix local renamed a_/z_ 650/650 (slot unchanged).
// Earlier: Self-aligned frame 0x2204 versus 0x21dc, definition/output carriers and angle equality homes remain.
// Earlier: frame 0x2204 versus 0x21dc; definition/output carriers still differ.
// Group word 866; remaining long 871; first input only 825; second only 878; unsigned plane 891; first plus group 774; first plus remaining 843.
// From 774: group-first test 753; in-place offset 779; vector return 800; deferred angle copy 808; word edge indices 776.
// Long emission index 763; tag-pointer rename/lifetime 774/774; group-first plus offset 786; plus long emission 747 (alignment worse).
// frame 0x2204 versus 0x21dc; definition/output homes and angle equality differ.
// Native initial face cursor 752; group-search cursor 785; candidate cursor 786;
// native bounds-component access 753; angle-input memcpy 749; single cosine clamp 803.
// Alias-free plane memcpy 798; rotated-result memcpy 796; both transfers 798; second angle-input
// memcpy 872; all copy boundaries 872. Original body and all helpers restored; frame 0x2204 versus
// 0x21dc and definition/output/equality homes remain.
// Frame 0x2204/0x21dc, definition/output carriers and angle equality homes remain.
// Earlier 753 (was 890): frame 0x2204 versus 0x21dc; definition/output carriers still differ.
// Earlier 749 (was 753): frame 0x2204 versus 0x21dc; definition/output homes and angle equality differ.
// Round 151: Alias-free pivot helper 753; native axis/offset 796/796; complete rotation row parts/products 788/798; fallback quad memcpy 796; native basis values 796; pivot helper plus row parts 764.
// frame 0x2204 versus 0x21dc; definition/output homes and angle equality differ.
// Without parameter-address aliases: reversed equality buffers 796; native plane result 925;
// rotation pointer result 796; native rotation output reference 796; native angle input 796 (fast).
// Original complete body and all helpers restored.
// Alias-free projection-plane/source-matrix/both/orientation-bounds/plane-bounds/matrix-bounds/all/group-plane receivers 892/791/859/791/859/795/892/847 (fast).
// Original complete body and helpers restored; frame 0x2204/0x21dc, definition/output and angle equality homes remain.
// Alias-free single angle exit 796; word edge-side predicate 807; unsigned-word list ordinals 773;
// native mesh reference 796; native rotation-basis references 796 (fast). Original body/helpers restored.
// Alias-free angle input order 796; rotation output/input first 792/792; plane output first 796;
// ordinary angle/rotation/plane inline 1168/796/796; all inline 1168. Original body/helpers restored.
// Round 155: native element references (alias-free), then complete rotation-input value: decoded plane 796; mesh edge 796; decoded plane mesh edge 796; edge endpoints 796; decoded plane edge endpoints 796; mesh edge edge endpoints 796; decoded plane mesh edge edge endpoints 796; rotation input value 1034. All restored.
// angle-products 781; axis-products 748.
// sign-products 748; angle-axis-products 805.
// angle-sign-products 804; axis-sign-products 770.
// angle-axis-sign-products 795; diagonal-axis-squares 800.
// Round 180 alias-free scalar names: selected distances, candidate distances, endpoint distances, axis,
// rotation, angle cosine, all distances, all scalars; scores 796/796/796/796/796/796/796/796 (fast); all
// restored.
// Round 354, without inherited parameter-address aliases: promoted signed plane index 788;
// mode field at every use 796; both 806; shared pivot-offset difference 753; promoted index plus
// shared offset 716 (kept, plain); mode plus offset 783; all three 787; all with shared normalizer 775.
// Retain the signed index promotion and existing offset helper; frame 0x2204 and entry carriers remain.
// Earlier 572 from 587. Shared-axis graph: uninitialized group-scope first/candidate/selected plane declarations in
// all seven nonempty subsets, then complete plane-role names, 572 each. All writes stay in place.
// Round 222: complete rotation/sign/angle product operand exchanges in all seven
// nonempty subsets 572 each; all exchanges with factor-first axis normalization 572.
// All original products restored; frame remains 0x21e4 versus 0x21dc and rotation spills differ.
// Round 204: complete rotation input with direct/native result 611/831; direct input plus pivot/axis scale/both 607/607/608; native axis scale 613; native input/result plus pivot/plus scale 760/761. Original whole body and helpers restored.
// Round 234: complete basis squares/sine/both/cosine pairs/squares+pairs/sine+pairs/all
// 726/582/601/618/564/631/598; all with count order 597. Squares+pairs kept (plain 564);
// each repeated part has two uses, every row is uniform and all float trees are preserved.
// Round 241: Actual emitted-face total short/word/long/dword at secondary pass entry and before initial
// projection: 735/723/723/723/749/626/626/626. Original outstanding count, face indices and body
// restored.
// Round 243: Existing tag record with native signed mode, const/mutable 572/572; actual limit
// flags byte/word/bool 572 each; tag plus each native flag declaration 572 each. All restored.
// Round 172: shared grouping/selected plane with short/word/long/dword outstanding total,
// original first-use lifetime 843/843/840/840; group-entry lifetime 843/843/840/840.
// Original separate planes and total restored; definition/count/plane homes and rotation spills remain.
// Round 166: On the exact-frame square/cosine-pair graph, tag/array-count/plane-endpoint lexical
// roles in all seven subsets 564 each; the uninitialized tag declaration before both scratch
// arrays 564. Original names, declarations and whole body restored; first tag/count/plane homes
// still differ.
// Round 253: native long initial/admission/selection/initial-admission/initial-selection/
// admission-selection/all/all with emission induction 730/767/710/765/707/781/779/756.
// Original signed short induction web, addressed counts and complete body restored.
// Round 236: complete square lifetimes before angle/before sine/between sine-cosine, reversed
// cosine-pair/square orders, early-square pair combinations and six-part names
// 617/607/564/611/564/611/583/564; all restored.
// Round 387: Negative/positive decoded-plane native setters, uniform per-use tag lookup and combinations 581/798/814/785/741/851/820/820; complete squared-axis sum included last. All restored.
// Round 221: native basis/rotation/both/position/basis-position/rotation-position/all setters
// 726/564/726/560/717/560/733; all plus axis setter 592. Four-point position gains rejected;
// original basis, complete rotation copy, matrix position and body restored.
// Round 262: actual distance/construction inline/noinline call summaries, separately
// and all four pairs 758/564/564/564/758/758/564/564. Original declarations, private
// helpers and body restored; definition/count/plane homes and rotation spills remain.
// Round 178: complete-vector memcmp 771; with decoded-output-first copy 774;
// result-first angle 770; both 776; with the complete rotation-result setter
// 771/774/770/776. Original equality, plane and rotation producers restored.
// Round 273: independent first/candidate/selected plane declarations at function entry,
// seven subsets 564 each; all three plus the quad declaration at entry 564. Original
// declarations and complete body restored; definition/count/plane homes still differ.
// Round 38: Staged cross product, with cross lifetime local/group and sign expression/branches,
// 559/559/559/559 fast at 3631 bytes, frame 0x21e4; each with common angle result 618
// at 3632 bytes. Five-point best rejected; original cross call, angle helper and body restored.
// Round 420: present-first-face region 564; missing-candidate continue 564; present-first-face region +
// missing-candidate continue 564; ordered geometry rejection guards 564; present-first-face region +
// ordered geometry rejection guards 564; missing-candidate continue + ordered geometry rejection guards
// 564; present-first-face region + missing-candidate continue + ordered geometry rejection guards 564;
// present-first-face region + missing-candidate continue + ordered geometry rejection guards + single
// angle result 564. Original body restored.
// Round 429: exact-frame count order 563 (one-point gain rejected); edge fields at use 694;
// both 694; complete group-owned selection workspace 769; count/workspace 768; edge/workspace
// and all three 790/790; all with ring anchor at use 707. Original body restored.
// DL round 206: selected edge direction at acceptance 647; with early pivot offset 633;
// with native point setter 648; early offset and point setter 682; those four with
// the staged shared cross product 647/633/648/682. Frames shrink to 0x21d4/0x21cc;
// original endpoints, pivot timing, helper operations and complete body restored.
// Existing plane/angle/normalizer boundaries, seven subsets, then all with staged cross:
// 704/594/727/669/726/660/742/755. Original complete executable source restored.
// J218: Current exact-frame ordered-distance result/accepted endpoint setters/rotation-entry pivot, seven subsets, then native transform value: 639/596/675/576/671/688/756/724. Original complete body restored.
// Round 489: plain normal negation / axis scaling / rotation component loops, seven
// subsets 589/552/564/603/589/594/559; all with short component ordinals 1157.
// Keep only the axis scaling loop, confirmed by plain check at 552; other producers restored.
// On the retained axis loop: staged cross / face count first / both 552 each; all restored.
// Round 452: Selected endpoints retained as word/long vertex indices, point pointers or edge-and-side
// identity; native points after selection, then field reads at use: 718/698/675/742/639/665/759/692. Original
// endpoint copies, helper boundaries and complete body restored; tag/count/plane homes and rotation spills
// remain.
// Round 454: Private plane/angle/rotation expansion-local and parameter role names in subsets
// 1..7, then all with tag-role names, 552 each. Original helper names and complete executable
// source restored; tag spill +0xb4 versus +0x94, count/plane homes and rotation spills remain.
// Round 239: Both accepted endpoint copies before the disjoint selected-plane output, with endpoint
// order, distance-store priority and pre-emission total decrement combinations, 683/683/683/683/785/785/786/786.
// Original endpoint/plane/total order and whole body restored; tag/count/plane homes and rotation spills remain.
// M288: Long/short edge side on long axis induction 677/637; dword induction
// 677/637; short induction 664/668; byte induction 664/668. All on the retained
// exact-frame axis-loop graph; original predicate, induction and complete body restored.
// Round 513: Bounded first-face for-loop consumes its ordinal in the array argument;
// short/word/long/int with original count order 539/609/741/741, face-count first
// 539/608/741/741. Keep the signed-short original-order form, plain 539 at 3647 bytes;
// face load now precedes count-pointer setup. Tag/count/plane homes and rotation spills remain.
// Round 462: complete mode/radius/projection-plane input ownership, all seven subsets
// 742/821/797/872/875/898/846; all with chained output counters 846. Original observations,
// copies and body restored; exact frame, tag/count/plane homes and rotation spills remain.
// Round 309: Scalar bitwise angle equality ikj/jik/jki/kij/kji 556/587/589/558/556;
// operand-symmetric ijk/kji 555/557; mismatch-owned angle evaluation 552.
// Original private angle gate, complete body and alias-safe rotation helper restored; tag/count/plane homes remain.
// M293: Consumed grouping-plane, selection metadata, basis scalar phase scopes, seven subsets,
// then all with initial face-count phase closed:
// 552/552/552/552/552/552/552/552. Original complete executable source restored.
// AC477: Two native sorted endpoint-distance homes, producer direction, exchange-slot scope and
// coherent near/far names: eight forms 793 each (reduced control 795; full 539). Two-point leads
// rejected; original distance graph, body and helpers restored. Tag/count/plane homes remain.
// Round 326: Complete old-bound snapshots/result branches/upper-first publication,
// subsets 1-7, then reversed observation order: fast 778/808/790/779/778/809/790/790
// against unchanged fast 795. Bounds snapshots plain 574 at 3616/3647, versus
// opening plain 539/3647; rejected. Original body and every helper restored;
// tag/count/plane homes and rotation spills remain.
// DH round 290: unconditional decoded plane owned by native value or destination,
// scalar fields/plain vector loop, then each with common angle result:
// 896/897/753/753/864/898/737/739. All regress; original decoders, angle gate and body restored.
// Round 532: native axis/bound-pair/up-component cursors, seven subsets and all with short
// orientation ordinal: fast 779/771/802/793/779/771/809/873 versus unchanged fast 795.
// Minimal axis/bounds cursors plain 574/591 at 3631/3599 bytes versus full 539/3647; both rejected.
// Original indexed component loops, complete body and all helpers restored; tag/count/plane homes remain.
// Round 330: sequential outer/inner signed-short ordinal and candidate/selected-plane
// storage subsets 1-7, then all with bound-first search, fast 771/780/781/762/822/807/774/770. Original body restored;
// first grouping plane stays independent; tag/count/plane homes remain.
// Plain confirmation: shared candidate/selected plane 754 at 3609 bytes; shared inner
// ordinal 705 at 3599. Both regress against 539; original body and every helper restored.
// Round 372: first-present-face searches ending with break, short/word/int/long, then separate
// bounded scans: fast 779/803/776/776/810/792/780/780 versus control 795.
// NOT kept: retail prologue/frame lost; original body restored, tag/count/plane homes remain.
// P274: Complete vector-return rotation producer with native input/component loop/entry basis
// declarations, eight calibrated forms 575/842/571/842/575/842/571/842 (control 539).
// Original helper and body restored; tag/count/plane homes and rotation spills remain.
// AC486: Outstanding total decremented at outer-group completion, short/long/word/dword
// 771/793/771/804; total also owns phase admission 771/794/771/805 (fast).
// Original total, decrement, admission and body restored; tag/count/plane homes remain.
// Ordinary outer-group decrement 746 at 3647 bytes versus full 539: NOT kept; fast lead was not confirmed.
// Round 490: Native initial/group-candidate/selection-emission ordinal role names,
// seven subsets tie fast 795; complete names with entry-owned initial zero 787.
// Minimal initial-zero ownership also 787 fast but 544 plain versus full 539: NOT kept.
// Original body/helpers restored; tag/count/plane homes remain.
// Y539: Complete decoder output reference/pointer return/reference return/value return,
// forced then ordinary inline: fast 795/795/795/875/795/795/795/963 against control
// 795 (full 539). Every peer score agrees with its preview. Original decoder, plane
// homes and body restored; native value returns move extent away from retail.
// m3 lane 11 r60 (plain): tag pointer and face count renamed (zz_/a_ prefixes, diagnostic) 539, same slots: names
// do not order these homes; the decal type read once into a `short type` used at its five uses 741; the quad
// declared at the group's top 539; the first plane declared at the group's top 539; both counts declared above
// the face arrays 539 (all same code).
// AC495: Existing shared plane decoder at first/candidate/selected sites, masks 1/2/4/3/5/6/7,
// 539/723/539/723/539/723/723; all with existing axis normalizer 715. Complete provider
// preview agrees with full 539. Original decoders, helper and complete body restored; tag/count/plane homes remain.
// Lane 17 r39 (plain): the definition renamed 539; the first loop's face read into a one-use local with the
// index incremented before the call 528 (retail's `inc esi` ahead of the pushes; NOT kept: one-use local, no
// match); the two counts declared face first 539; the definition declared ahead of the arrays 539; the quad
// declared at the top of the group pass 539; the first loop as a do/while 562 (3631 bytes). Body restored.
// AC497: Complete decoded-plane/rotation-result/fallback-quad memmove boundaries,
// subsets 1/2/4/3/5/6/7, then plane+rotation memmove with fallback memcpy:
// 742/592/803/741/781/819/826/741. NOT kept: all regress or move frame/extent away
// from retail; original copies, complete input-before-output helper and body restored.
// Round 67 (m3 lane 10, plain; `--fast` builds this body differently, 795 on unchanged source): the outer do-while
// as `while (remaining > 0)` 545 (3631 bytes); the first plane decoded after the face is taken 758; the quad
// branch inverted (fallback copy first) 574; the first loop stepping `++i` with `faces[i]` 552; the angle test as
// `angle <= maximum` 539 (same code).
// AC520: Complete signed-distance boundaries, real/double, immediate/paired fabs and
// ordering-result forms: 539/533/539/533/831/843/831/843. NOT kept: six-point leads
// grow frame to 0x21e4; double forms lose the native prologue; original body/helpers restored.
// AC522: Complete selected-edge acceptance bool/byte/short/long, then
// complementary rejection 533/533/533/533/606/606/606/606. NOT kept: six-point lead below
// gate and frame 0x21e4 versus 0x21dc; original complete body restored.
// Y566: Float sine/cosine/inverse-cosine API families 1/2/4/3/5/6/7, then
// float sine/cosine with a double angle: 539/539/562/539/582/582/570/840.
// Original complete body and helpers restored; tag/count/plane homes and rotation
// spills remain. Float inverse cosine shortens below retail; wide angle loses the prologue.
// J250: Complete decoder/angle/alias-safe rotation formal-reference contracts,
// seven subsets then ordinary complete expansion, 539 each. NOT kept: tag/count/plane
// homes and rotation spills remain; original helpers and body restored. All rotation
// inputs still precede the first output store. Shared-angle sum changes were not applied.
// Y584: Axis normalizer numeric-length result, zero-default/branch-owned with
// bool/byte/short/long epsilon admission: 548 each. NOT kept: frame 0x21e4
// and extent 3631 lose retail 0x21dc/3647; original full body and helpers restored.
// AC524: Complete bitwise normal identity bool/byte/short/long, then complementary
// mismatch results, 535 each at 3647 bytes. NOT kept: four-point gain below eight;
// original helper and complete body restored; tag/count/plane homes and rotation spills remain.
// AC526: Complete normalized-axis admission bool/byte/short/long, then complementary
// rejection results, 533 each at 3647 bytes. NOT kept: six-point lead below eight and
// frame grows to 0x21e4; original body/helpers restored; tag/count/plane homes remain.
// AC529: Complete forward-edge role, bool/byte/word/long direct then false-default publication:
// 555/555/621/578/624/624/624/630. NOT kept: no equality or gain; native tag/count/plane/topology homes remain.
// AC535: Complete candidate-plane grouping acceptance, positive then complementary
// bool/byte/short/long: 529/529/529/529/531/531/531/531 at 3647 bytes. NOT kept:
// frame grows to 0x21e4 versus native 0x21dc; original body/helpers restored.
// F393: Existing decoder masked plane ordinal long/dword/int/word, then sign-bit decision bool/byte/short/long:
// 539/539/539/574/736/736/736/574. NOT kept: no byte match; original complete body/helpers restored;
// native tag/count/plane homes and rotation spills remain.
// DH372: Partial angular/first-distance grouping decisions, bool/byte/short/long,
// 529 each at 3647 bytes. NOT kept: one-use outcomes are unmatched and enlarge the
// native 0x21dc frame to 0x21e4; original body/helpers restored, tag/count/plane homes remain.
// Round 56 (m3 lane 15, plain; masked = aligned diff with stack offsets and registers masked, 360 here; --fast stays unfaithful, one-link harness 783, masked 378):
// retail's rotation is 0x171d90's and 0x1546f0's shape. Squares before the trig values, pairs k-j, k-i, j-i, rows in forward/left/up order 671 (3615 B, masked 243);
// `t * k` shared, rows in retail's visit order, position rotated in place 583 (3631 B, masked 254); pairs j-i, k-i, k-j, retail's visit order 679 (3632 B, masked 264);
// 0x1546f0's statements, squares j, k, i, trig after 607 (3663 B, masked 303), squares i, j, k 607 (3663 B, masked 294); `t * k` shared, rows forward, left j/i/k, up k/i/j
// 648 (3647 B, masked 270); matrix3x3 basis with accumulating rotate rows in place 646 (3647 B, masked 384). All but the first change the frame (+0x7), which moves every home; the first keeps it, 32 bytes short. Harness: accumulating rows
// (`real i = up.i*k; i += left.i*j; ...`) as retail's spill pattern suggests, with or without an in==out copy, m397-423; 241 + 151 + 482 + 304 random basis forms best masked 258.
// T57: Accepted endpoint coordinate-array transfers, long/short/dword/word ordinals,
// paired then separate loops
// 547/541/547/541/539/574/539/574. NOT kept: no exact or qualifying structural gain; original complete body restored.
// T59: Native matrix3x3 rotation basis, original/group lifetime, original/complete input
// capture and separate/matrix-position pivot: 536/661/585/600/536/661/585/600.
// NOT kept: three-point lead is below eight and matrix grouping is unmatched; other forms
// lose native frame/extent. Original basis vectors, alias-safe helper and whole body restored.
// P351: Angle first/final clamp bound-comparison truths in bool/byte/short/long:
// 566/566/566/566/611/611/611/611. NOT kept: no byte match; tag/count/plane homes and rotation spills remain.
// Original complete body and existing helpers restored.
// Y606: Outstanding total before signed phase admission, short/int/long/dword,
// flags rejection then nested flags admission: 538/763/763/763/538/763/763/763.
// NOT kept: one-point native gain; wider totals lose native frame/extent. Original body restored.
// F396: Complete normal-identity reduction, Boolean AND and full-word XOR/OR, long/int/short/word ordinals: 780/780/803/803/715/715/742/742.
// NOT kept: no byte equality; native short-circuit identity, tag/count/plane homes and rotation spills remain. Original complete body and helpers restored.
// DH379: Two-consumer distance limit after angle/after plane/before candidate/at group,
// mutable and immutable: 539/539/726/726/792/792/791/791. NOT kept: native production ties,
// earlier lifetimes regress; original observations, helpers and complete body restored.
// DH383: Complete parallel near/far conditional/default/branch producers retain four original distances:
// 614/614/646/645/540/614/634/634. NOT kept: no equality or gain; several lose native frame or extent.
// Original four-value sort, tag/count/plane homes, all helpers and entire body restored.
// DH385: Entry output-counter native memset/first-face memcpy producers, seven subsets,
// then first-face memmove: 562/562/539/566/562/562/566/789. NOT kept: no gain or tag-home
// recovery; original scalar counters, face publication, all helpers and complete body restored.
// Round 108 (m3 lane 9, plain; masked = aligned with stack offsets and registers masked, baseline 496): `matrix.scale = 1.0f;`
// before the position 535 (masked 469); `short remaining = secondary_count;` before the guard, tested there 539; the counts
// declared face count first 535 (re-measure); `sign * decal_normal_angle_17fd20(...)` 535 (same code); the edge's min/max as two
// ternaries 613 (re-measure of DH383's conditional producers, 614); the axis scaled `inverse * axis.i`, `axis.j * inverse`,
// `axis.k * inverse` for the component loop 559; the edge side as `long reverse` 646 (3631 B).
// Retail's offset rotation writes each partial sum to +0x10 and reloads it: a register-pressure spill, not a source form.
// M3 lane 3 r225 (plain; base 535, aligned [exact, stack-masked, register-masked] [676, 501, 366]; the first ~400 instructions agree
// once stack offsets and registers are masked, apart from the first loop's index increment and the secondary-count store): the axis
// scale through vector3f_scale with the named inverse 559 (3631 bytes) [679, 471, 348]; with 1.0f / length written in the call 568
// (3631) [674, 468, 341]; as three `*=` statements 559 (3631); the three diagonal terms as (1 - square) * cosine + square 535 (same
// code); `remaining` declared under the flags test and tested before the do-while 534 (retail's store ahead of the jle, one point,
// not kept). Lead: retail loads 1.0f into xmm0 before the reciprocal and reuses it for the sign, the clamp and every 1 - x of the
// rotation (ours reloads the constant at each use), and it spills the k * k, j * sine and i * sine products (i * sine through x87),
// so the rotation block's register pressure differs, not its statement order.
// m3 lane 12 r106 (plain; fast gives 834 on the unchanged body here): start/end and the selected plane at function
// scope 535 (same code); face_count declared before secondary_count 535 (retail's store order for the two counts,
// slots unchanged); the first plane at function scope 535; the selected plane alone 535; start/end alone 535 (all
// the same frame homes: scope and declaration order do not move a slot in this body).
// MiniMax H (11-11): no new form, no lower score (535 plain, 834 on the reduced image): the frame is 0x21d8 against
// retail's 0x21dc and the definition pointer, the count and the zeroed word still sit in other slots than retail's,
// so every row after the first loop stays shifted.
// MiniMax D (11-10): no new form, no lower score: the definition pointer, the count and the zeroed word
// still sit in other frame slots than retail's (0x94, 0x20 and 0x54 against our 0xb4, 0x18 and 0x4c).
// Round 187 (m3 lane 4, plain): the edge walk started from `first_edge` (edge_index copied from it) 535 (same code); the
// angle limit named once ahead of the candidate loop 751 (3679 bytes); the endpoint vertices named as vertex pointers 535
// (same code); the endpoint vertex indices named, the vertex array indexed at each use 691 (3615 bytes); the endpoint
// addresses written in the distance calls and the two copies 664 (3631 bytes). Lead from that last form: the i term of
// both inlined plane distances then loads the plane first (`fld [eax + 0x44]; fmul [ecx]`) as retail does; with the
// point pointers ours loads the point first there.
// M3 lane 10 r92 (plain, the file's other eight functions unchanged in each; fast 834 on unchanged source; base 535, aligned 672,
// masked 355): nothing kept. The rotation's sine products named sine_i/j/k (re-measure of round 234's form) 569 (3663 bytes, masked
// 384); `one_minus_cosine * axis.k` named once for both k pairs (retail spills that product at +0xec) 536 (3663 bytes, masked 328);
// both 589 (3663 bytes); the sign as `-1.0f` set to 1.0f in an if 536 (masked 349); the squares computed k, i, j (retail's order)
// 537; the rows in retail's visit order (forward i, j, left i, j, forward k, up k, i, left k, up j) 545.
// m3 lane 14 r89 (plain; fast gives 834 here): `selected_plane` renamed `best_plane` 535 (the 16-byte plane homes
// stay, ours +0x30/+0x60, retail +0x44/+0x34: names do not order this body's slots); the axis length and normalisation
// through the existing normalize_inline (its sum order i, k, j is retail's) 650 (first difference +0x7); the four
// rotations through the existing vector3f_rotate_into_axes with a matrix3x3 basis 573 (3615 B; its rows sum j, k, i
// and i, j, k where retail's sum k, j, i as the private helper does); quad and matrix declared at the group's scope
// 535 (same code); the bounds loop reading `matrix.up.n[k]` for the cast 535 (same code). Left: weight-ordered spill
// homes (names and scopes do not move them) and the rotation block's schedule; rounds 10-12, 56, 92, 108, 187, 225,
// T59 and DH383 above list its statement orders, named products, helpers and in-place forms, and no untried natural
// form was found beyond these.
// P469: Complete cosine-weight/sine-weight/squared-axis vector producers, seven
// family subsets then all by plain component loops: 556/676/699/909/583/642/944/567.
// NOT kept: vector residence/rounding misses native code or frame/extent; original
// scalar parts, signed loops and complete input-before-output rotation restored.
// P483: Accepted endpoint memcpy start/end/both, memmove start/end/both, then
// both endpoints plus fallback quad with either library: 535/535/535/748/749/736/535/810. NOT kept:
// tag/count/plane homes persist or frame/extent regresses; original whole body
// and every private helper restored.
// T104: Accepted-face selection/emission cursors, original-count then actual-end bounds,
// separate/both/mixed phases: 667/535/682/660/539/679/671/671. NOT kept: no native
// tag/count/plane-home recovery or equality; original indexed traversals and full body restored.
// @retail 0x17fd20
void function_17fd20(s_decal_mesh_view const *mesh, long tag_index,
	s_decal_output_17dd80 *output, s_decal_projection_17fd20 *projection, s_decal_placement const *placement)
{
	long faces[1024];
	long secondary[1024];
	byte const *definition = g_4e3b44[tag_index & 0xffff].bytes;
	output->polygon_count = 0;
	output->vertex_count = 0;
	faces[0] = placement->unknown50;
	short secondary_count = 0;
	short face_count = 1;
	for (short i = 0; i < face_count;)
	{
		function_17dd80(mesh, &projection->quad, output, faces[i++], true, projection->radius,
			*(short const *)(definition + 2), faces, &face_count, secondary, &secondary_count);
	}
	if ((byte)g_444a38[*(short const *)(definition + 2)].flags && secondary_count > 0)
	{
		short remaining = secondary_count;
		do
		{
			short group_count = 0;
			for (short i = 0; !group_count && i < secondary_count; ++i)
			{
				long first_face = secondary[i];
				if (first_face == NONE)
					continue;
				plane3f first_plane;
				decal_face_plane_17fd20(mesh, first_face, &first_plane);
				faces[group_count++] = first_face;
				secondary[i] = NONE;
				for (short j = i + 1; j < secondary_count; ++j)
				{
					long face = secondary[j];
					if (face != NONE)
					{
						plane3f plane;
						decal_face_plane_17fd20(mesh, face, &plane);
						if (g_444a38[*(short const *)(definition + 2)].maximum_angle * 0.017453292f >= decal_normal_angle_17fd20(&first_plane.n, &plane.n) &&
							projection->radius * 0.05f > function_17d900(mesh, face, &first_plane) &&
							projection->radius * 0.05f > function_17d900(mesh, first_face, &plane))
						{
							faces[group_count++] = face;
							secondary[j] = NONE;
						}
					}
				}
				long selected_face = NONE;
				real selected_minimum, selected_maximum;
				point3f start, end;
				plane3f selected_plane;
				for (short j = 0; j < group_count; ++j)
				{
					long face = faces[j];
					long edge_index = mesh->faces[face].first_edge;
					long first_edge = edge_index;
					do
					{
						s_decal_mesh_edge const *edge = &mesh->edges[edge_index];
						bool reverse = edge->faces[1] == face;
						point3f const *a = &mesh->vertices[edge->vertices[!reverse]].position;
						point3f const *b = &mesh->vertices[edge->vertices[reverse]].position;
						real da = (real)fabs(plane_distance_to_point(&projection->quad.plane, a));
						real db = (real)fabs(plane_distance_to_point(&projection->quad.plane, b));
						real minimum, maximum;
						if (da > db)
						{
							minimum = db;
							maximum = da;
						}
						else
						{
							minimum = da;
							maximum = db;
						}
						if (selected_face == NONE || (selected_minimum >= minimum && selected_maximum >= maximum))
						{
							decal_face_plane_17fd20(mesh, face, &selected_plane);
							start = *a;
							end = *b;
							selected_minimum = minimum;
							selected_maximum = maximum;
							selected_face = face;
						}
						edge_index = edge->next_edges[reverse];
					} while (edge_index != first_edge);
				}
				vector3f axis;
				vector3d_from_points3d(&start, &end, &axis);
				real length = (real)sqrt(axis.j * axis.j + axis.k * axis.k + axis.i * axis.i);
				if (!(fabs(length) < 0.0001f))
				{
					real inverse = 1.0f / length;
					for (long component = 0; component < 3; ++component)
						axis.n[component] *= inverse;
				}
				else
					length = 0.0f;
				s_decal_quad_17d9a0 quad;
				if (length > 0.0f)
				{
					vector3f cross;
					cross3f(&selected_plane.n, &projection->quad.plane.n, &cross);
					real sign = cross.i * axis.i + cross.k * axis.k + cross.j * axis.j < 0.0f ? 1.0f : -1.0f;
					real angle = decal_normal_angle_17fd20(&selected_plane.n, &projection->quad.plane.n) * sign;
					real sine = (real)sin(angle);
					real cosine = (real)cos(angle);
					real one_minus_cosine = 1.0f - cosine;
					real square_i = axis.i * axis.i;
					real square_j = axis.j * axis.j;
					real square_k = axis.k * axis.k;
					real cosine_j_i = one_minus_cosine * axis.j * axis.i;
					real cosine_k_i = one_minus_cosine * axis.k * axis.i;
					real cosine_k_j = one_minus_cosine * axis.k * axis.j;
					vector3f forward, left, up;
					forward.i = square_i + (1.0f - square_i) * cosine;
					forward.j = cosine_j_i + axis.k * sine;
					forward.k = cosine_k_i - axis.j * sine;
					left.i = cosine_j_i - axis.k * sine;
					left.j = square_j + (1.0f - square_j) * cosine;
					left.k = cosine_k_j + axis.i * sine;
					up.i = cosine_k_i + axis.j * sine;
					up.j = cosine_k_j - axis.i * sine;
					up.k = square_k + (1.0f - square_k) * cosine;
					transform4x3f matrix;
					vector3f offset;
					vector3d_from_points3d(&start, &projection->matrix.position, &offset);
					decal_rotate_vector_17fd20(&forward, &left, &up, &offset, &offset);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.forward, &matrix.forward);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.left, &matrix.left);
					decal_rotate_vector_17fd20(&forward, &left, &up, &projection->matrix.up, &matrix.up);
					matrix.position.x = offset.i + start.x;
					matrix.position.y = offset.j + start.y;
					matrix.position.z = offset.k + start.z;
					matrix.scale = 1.0f;
					function_17d9a0(&matrix, (real const *)&projection->bounds, &quad);
					for (long k = 0; k < 3; ++k)
					{
						real value = ((real const *)&matrix.up)[k];
						projection->orientation_bounds[k * 2] = value > projection->orientation_bounds[k * 2] ? projection->orientation_bounds[k * 2] : value;
						projection->orientation_bounds[k * 2 + 1] = value > projection->orientation_bounds[k * 2 + 1] ? value : projection->orientation_bounds[k * 2 + 1];
					}
				}
				else
					quad = projection->quad;
				for (short j = 0; j < group_count; ++j)
				{
					function_17dd80(mesh, &quad, output, faces[j], false, projection->radius,
						*(short const *)(definition + 2), 0, 0, 0, 0);
				}
				remaining -= group_count;
			}
		} while (remaining > 0);
	}
}

struct s_decal_bitmap_group;
struct s_decal_sprite_sequence;
struct s_decal_sprite_frame;

/* Layout recovered from the preparation, projection and commit accesses. */
struct s_decal_preparation_17ef10
{
    s_decal_projection_17fd20 projection;
    box2f texture_bounds;
    short sequence_index;
    short frame_index;
    short bitmap_index;
    short unknown102;
    s_decal_bitmap_group *field_104_2;
    s_decal_sprite_sequence *sequence;
    s_decal_sprite_frame *frame;
};

struct s_decal_chain_state_17ee20
{
    bool reuse;
    byte unknown01[3];
    real radius_fraction;
    real color_fraction;
    real unknown0c;
    real unknown10;
    real angle;
    long sequence_choice;
};

struct s_180d84;
struct s_cluster_query;
struct s_180d81;
struct s_180d83;
long __stdcall function_180d80(s_180d84 const *preparation,
    transform4x3f const *transform, long tag_index,
    s_cluster_query const *placement, bool unknown0,
    s_180d81 const *output, s_180d83 *state);
bool function_17ef10(transform4x3f const *transform, long tag_index,
    s_decal_placement const *placement, vector3f const *direction, real radius,
    bool unknown0, long unknown1, long unknown2,
    s_decal_preparation_17ef10 *preparation, s_decal_chain_state_17ee20 *state);

// @retail 0x17ee20
bool function_17ee20(s_decal_mesh_view const *mesh,
    transform4x3f const *transform, long tag_index,
    s_decal_placement const *placement, vector3f const *direction, real radius,
    bool unknown0, long unknown1, long unknown2)
{
    s_decal_chain_state_17ee20 state;
    memset(&state, 0, sizeof(state));
    bool result = false;
    while (tag_index != -1 && (short)placement->unknown20 != -1)
    {
        s_decal_preparation_17ef10 preparation;
        memset(&preparation, 0, sizeof(preparation));
        s_decal_output_17dd80 output;
        if (!function_17ef10(transform, tag_index, placement, direction, radius,
            unknown0, unknown1, unknown2, &preparation, &state))
            break;
        output.polygon_count = 0;
        function_17fd20(mesh, tag_index, &output, &preparation.projection, placement);
        tag_index = function_180d80((s_180d84 const *)&preparation,
            transform, tag_index, (s_cluster_query const *)placement, unknown0,
            (s_180d81 const *)&output, (s_180d83 *)&state);
        if (output.polygon_count > 0)
            result = true;
    }
    return result;
}

struct s_decal_recent_17ef10
{
    long tag_index;
    point3f position;
    real radius;
    point3f unknown14;
    long unknown20;
    real expires;
    dword frame;
};
struct s_decal_globals;
extern s_decal_globals *g_4ea94c;
extern __int64 g_485aa0;
long g_46dd4c = NONE;
point3f g_510c80;
short g_468cac[8] = {1, 1, -1, -1, 0, 1, 0, -1};
short g_468cc0[8] = {1, -1, -1, 1, 1, 0, -1, 0};

struct s_decal_definition_17ef10
{
    word flags;
    byte unknown02[4];
    word nearby_limit;
    byte unknown08[8];
    real minimum_radius;
    real maximum_radius;
    real nearby_scale;
    byte unknown1c[0x38 - 0x1c];
    real lifetime;
    byte unknown3c[0x8c - 0x3c];
    long bitmap_index;
};
struct s_decal_bitmap_view_17ef10
{
    short type;
    byte unknown02[0x3c - 2];
    long sequence_count;
    byte *sequences;
    long bitmap_count;
    byte *bitmaps;
};
struct s_decal_sprite_definition;
bool function_17cb00(s_decal_sprite_definition const *definition,
    short sequence_index, short frame_index, real scale,
    box2f *texture_bounds, box2f *position_bounds);
void function_142390(plane3f const *plane, transform4x3f *out);

PRIVATE __forceinline real decal_time_17ef10()
{
    return g_510c54 && g_510c54->active
        ? (real)g_510c54->game_time * g_510c54->rate : 0.0f;
}
PRIVATE __forceinline dword decal_random_17ef10()
{
    g_4e7408->seed = g_4e7408->seed * 0x19660d + 0x3c6ef35f;
    return g_4e7408->seed >> 16;
}
PRIVATE __forceinline real decal_fraction_17ef10()
{
    return (real)decal_random_17ef10() * 1.5259021893143654e-05f;
}
PRIVATE __forceinline void decal_normalize_17ef10(vector3f *v)
{
    real length = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        v->i = inverse * v->i;
        v->j = inverse * v->j;
        v->k = inverse * v->k;
    }
}
PRIVATE __forceinline void decal_vector_17ef10(transform4x3f const *m,
    vector3f const *v, vector3f *out)
{
    real x = v->i, y = v->j, z = v->k;
    if (m->scale != 1.0f)
    {
        x *= m->scale;
        y *= m->scale;
        z *= m->scale;
    }
    out->i = m->up.i * z + m->left.i * y + m->forward.i * x;
    out->j = m->up.j * z + m->left.j * y + m->forward.j * x;
    out->k = m->up.k * z + m->left.k * y + m->forward.k * x;
}

// score 987 (was 1005): a mask test read as a field bit (TEST_FIELD_BIT) gives retail's load-and-shift; the rest still differs.
// @retail 0x17ef10
bool function_17ef10(transform4x3f const *transform, long tag_index,
    s_decal_placement const *placement, vector3f const *direction, real radius,
    bool unknown0, long unknown1, long unknown2,
    s_decal_preparation_17ef10 *preparation, s_decal_chain_state_17ee20 *state)
{
    s_decal_definition_17ef10 const *definition =
        (s_decal_definition_17ef10 const *)g_4e3b44[tag_index & 0xffff].bytes;
    point3f position = placement->position;
    if (transform)
    {
        vector3f input = {position.x, position.y, position.z};
        vector3f output;
        decal_vector_17ef10(transform, &input, &output);
        position.x = output.i + transform->position.x;
        position.y = output.j + transform->position.y;
        position.z = output.k + transform->position.z;
    }
    if (definition->nearby_limit > 0 && !unknown2)
    {
        real scale = definition->nearby_scale;
        real now = decal_time_17ef10();
        if (scale == 0.0f)
            scale = 1.0f;
        s_decal_recent_17ef10 *recent =
            (s_decal_recent_17ef10 *)((byte *)g_4ea94c + 0x380c);
        long count = 0;
        for (long i = 0; i < 24; ++i)
        {
            s_decal_recent_17ef10 const *entry = &recent[i];
            if (entry->tag_index && (entry->expires == 0.0f || entry->expires > now)
                && entry->tag_index == tag_index && entry->radius > 0.0f)
            {
                real distance = definition->maximum_radius * scale + entry->radius;
                real dx = entry->position.x - position.x;
                real dz = entry->position.z - position.z;
                real dy = entry->position.y - position.y;
                if (distance * distance >= dx * dx + dz * dz + dy * dy)
                {
                    if (++count >= definition->nearby_limit)
                        return false;
                }
            }
        }
        long chosen = -1;
        dword oldest = 0;
        for (long j = 0; j < 24; ++j)
        {
            s_decal_recent_17ef10 const *entry = &recent[j];
            if (!entry->tag_index || (entry->expires > 0.0f && now > entry->expires))
            {
                chosen = j;
                break;
            }
            if (entry->frame <= oldest || !oldest)
            {
                chosen = j;
                oldest = entry->frame;
            }
        }
        if (chosen != -1)
        {
            s_decal_recent_17ef10 *entry = &recent[chosen];
            entry->tag_index = tag_index;
            entry->position = position;
            entry->radius = definition->maximum_radius * scale;
            entry->unknown14 = g_510c80;
            entry->unknown20 = g_46dd4c;
            entry->expires = 0.0f;
            entry->frame = (dword)g_485aa0;
            if (definition->lifetime != 0.0f)
                entry->expires = definition->lifetime + decal_time_17ef10();
        }
    }

    vector3f normal = placement->plane.n;
    vector3f tangent, perpendicular;
    real cosine, sine;
    if (TEST_FIELD_BIT((definition->flags >> 3) & 1) &&
        -0.0001f > normal.j * direction->j + normal.k * direction->k + normal.i * direction->i)
    {
        cosine = -1.0f;
        sine = 0.0f;
        if (definition->flags & 0x20)
        {
            transform4x3f basis;
            function_142390(&placement->plane, &basis);
            vector3f local = *direction;
            if (basis.scale != 1.0f)
            {
                real inverse = 1.0f / basis.scale;
                local.i *= inverse;
                local.j *= inverse;
                local.k *= inverse;
            }
            real x = basis.forward.k * local.k + basis.forward.j * local.j + basis.forward.i * local.i;
            real y = basis.left.k * local.k + basis.left.j * local.j + basis.left.i * local.i;
            long selected = 4;
            real minimum = FLT_MAX;
            for (long k = 0; k < 8; ++k)
            {
                real a = (real)g_468cac[k];
                real b = (real)g_468cc0[k];
                real length = (real)sqrt(b * b + a * a);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    a *= inverse;
                    b *= inverse;
                }
                real dot = y * b + x * a;
                if (minimum > dot)
                {
                    minimum = dot;
                    selected = k;
                }
            }
            if (selected != 8)
            {
                vector3f a = {(real)g_468cac[selected], (real)g_468cc0[selected], 0.0f};
                vector3f b = {(real)g_468cc0[selected], (real)-g_468cac[selected], 0.0f};
                decal_vector_17ef10(&basis, &a, &tangent);
                decal_vector_17ef10(&basis, &b, &perpendicular);
            }
            else
            {
                tangent = basis.left;
                perpendicular = basis.up;
            }
        }
        else
        {
            tangent.i = normal.j * direction->k - normal.k * direction->j;
            tangent.j = direction->i * normal.k - normal.i * direction->k;
            tangent.k = normal.i * direction->j - direction->i * normal.j;
            perpendicular.i = normal.j * tangent.k - normal.k * tangent.j;
            perpendicular.j = normal.k * tangent.i - normal.i * tangent.k;
            perpendicular.k = normal.i * tangent.j - normal.j * tangent.i;
        }
    }
    else
    {
        /* Retail keeps both multiplications in x87 precision before storing
           the angle. Rounding the random fraction first changes the basis. */
        if (!state->reuse)
            state->angle = (real)((double)decal_random_17ef10()
                * (double)1.5259021893143654e-05f * (double)6.2831854820251465f);
        cosine = (real)cos(state->angle);
        sine = (real)sin(state->angle);
        real x = (real)fabs(normal.i), y = (real)fabs(normal.j), z = (real)fabs(normal.k);
        if (y >= x && z >= x)
        {
            tangent.i = 0.0f;
            tangent.j = normal.k;
            tangent.k = 0.0f - normal.j;
        }
        else if (z >= y)
        {
            tangent.i = 0.0f - normal.k;
            tangent.j = 0.0f;
            tangent.k = normal.i;
        }
        else
        {
            tangent.i = normal.j;
            tangent.j = 0.0f - normal.i;
            tangent.k = 0.0f;
        }
        perpendicular.i = normal.j * tangent.k - normal.k * tangent.j;
        perpendicular.j = normal.k * tangent.i - normal.i * tangent.k;
        perpendicular.k = normal.i * tangent.j - normal.j * tangent.i;
    }
    decal_normalize_17ef10(&tangent);
    decal_normalize_17ef10(&perpendicular);
    transform4x3f *matrix = &preparation->projection.matrix;
    matrix->position = placement->position;
    matrix->forward.i = perpendicular.i * cosine - tangent.i * sine;
    matrix->forward.j = perpendicular.j * cosine - tangent.j * sine;
    matrix->forward.k = perpendicular.k * cosine - tangent.k * sine;
    matrix->left.i = tangent.i * cosine + perpendicular.i * sine;
    matrix->left.j = tangent.j * cosine + perpendicular.j * sine;
    matrix->left.k = tangent.k * cosine + perpendicular.k * sine;
    matrix->up = normal;

    s_decal_bitmap_view_17ef10 *bitmap =
        (s_decal_bitmap_view_17ef10 *)g_4e3b44[definition->bitmap_index & 0xffff].bytes;
    preparation->field_104_2 = (s_decal_bitmap_group *)bitmap;
    if (bitmap->sequence_count > 0)
    {
        if ((short)unknown1 != -1)
            preparation->sequence_index = (short)unknown1;
        else
        {
            if (!state->reuse)
            {
                long maximum = (short)((short)bitmap->sequence_count - 1);
                state->sequence_choice = (short)((decal_random_17ef10() * maximum) >> 16);
            }
            long selected = state->sequence_choice;
            if (selected < 0) selected = 0;
            else if (selected > bitmap->sequence_count - 1) selected = bitmap->sequence_count - 1;
            preparation->sequence_index = (short)selected;
        }
    }
    else
        preparation->sequence_index = 0;
    preparation->frame_index = 0;
    if (radius == 0.0f)
        radius = 1.0f;
    if (!state->reuse)
        state->radius_fraction = decal_fraction_17ef10();
    preparation->projection.radius = ((definition->maximum_radius - definition->minimum_radius)
        * state->radius_fraction + definition->minimum_radius) * radius;
    if (bitmap->type == 3 && function_17cb00((s_decal_sprite_definition const *)definition,
        preparation->sequence_index, 0, preparation->projection.radius,
        &preparation->texture_bounds, &preparation->projection.bounds))
    {
        byte *sequence = bitmap->sequences + preparation->sequence_index * 0x3c;
        byte *frame = *(byte **)(sequence + 0x38) + preparation->frame_index * 0x20;
        preparation->sequence = (s_decal_sprite_sequence *)sequence;
        preparation->frame = (s_decal_sprite_frame *)frame;
        preparation->bitmap_index = *(short *)frame;
    }
    else
    {
        real aspect = 1.0f;
        if (definition->flags & 0x100)
        {
            byte *data = bitmap->bitmaps + preparation->bitmap_index * 0x74;
            aspect = (real)*(short *)(data + 6) / (real)*(short *)(data + 4);
        }
        preparation->projection.bounds.x0 = 0.0f - preparation->projection.radius;
        preparation->projection.bounds.x1 = preparation->projection.radius;
        preparation->projection.bounds.y0 = 0.0f - preparation->projection.radius * aspect;
        preparation->projection.bounds.y1 = preparation->projection.radius * aspect;
        preparation->texture_bounds.x0 = preparation->texture_bounds.y0 = 0.0f;
        preparation->texture_bounds.x1 = preparation->texture_bounds.y1 = 1.0f;
    }
    if (!unknown0)
    {
        s_bitmap_predict_view *data = (s_bitmap_predict_view *)(bitmap->bitmaps + preparation->bitmap_index * 0x74);
        if (!(data->last_frame > g_4e6488 && data->texture))
        {
            _mm_prefetch((char const *)&data->flags, _MM_HINT_T0);
            _mm_prefetch((char const *)&data->data_offset, _MM_HINT_T0);
            _mm_prefetch((char const *)&data->data_offset1, _MM_HINT_T0);
            _mm_prefetch((char const *)&data->data_offset2, _MM_HINT_T0);
            _mm_prefetch((char const *)data->texture, _MM_HINT_T0);
            if (!texture_cache_bitmap_get_texture((s_bitmap_data *)data, 2, 0.0f)
                && !function_12ce00((s_bitmap_data *)data, 2, 0.0f))
                return false;
        }
    }
    function_17d9a0(matrix, (real const *)&preparation->projection.bounds, &preparation->projection.quad);
    preparation->projection.orientation_bounds[0] = preparation->projection.orientation_bounds[1] = normal.i;
    preparation->projection.orientation_bounds[2] = preparation->projection.orientation_bounds[3] = normal.j;
    preparation->projection.orientation_bounds[4] = preparation->projection.orientation_bounds[5] = normal.k;
    return true;
}

struct s_effect_source;
struct s_collision_result_1697c0;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
bool g_46dd48 = true;
real function_30bf0(vector3f *vector);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
plane3f *function_1428b0(transform4x3f const *matrix, plane3f const *plane, plane3f *out);
int __stdcall function_1429d0(transform4x3f const *matrix, long count,
    point3f const *source, point3f *destination);

// @retail 0x17e670
void function_17e670(s_effect_source *source, point3f const *point, long tag_index,
    vector3f const *vector, real radius, long unknown0, long unknown1, long unknown2)
{
    if (!g_46dd48)
        return;
    s_decal_definition_17ef10 const *definition =
        (s_decal_definition_17ef10 const *)g_4e3b44[tag_index & 0xffff].bytes;
    dword saved_seed = g_4e7408->seed;
    bool deterministic = (byte)unknown0 != 0;
    if (deterministic)
    {
        dword const *bits = (dword const *)point;
        g_4e7408->seed = bits[0] ^ bits[1] ^ bits[2] ^ 0xdeadc0de;
    }
    long excluded = -1;
    short cluster = -1;
    bool world_hit = false;
    point3f center;
    vector3f direction = *vector;
    function_30bf0(&direction);
    point3f start;
    start.x = point->x - direction.i * 0.01f;
    start.y = point->y - direction.j * 0.01f;
    start.z = point->z - direction.k * 0.01f;
    s_decal_placement collision;
    collision.unknown24 = -1;
    s_decal_placement *placement = (s_decal_placement *)source;
    bool found = true;
    if (!placement || placement->unknown00 == 4)
    {
        found = function_1697c0(0x20800007, &start, vector, -1, -1,
            (s_collision_result_1697c0 *)&collision);
        placement = &collision;
    }
    if (found)
    {
        cluster = (short)placement->unknown20;
        if (placement->unknown00 == 1)
        {
            world_hit = true;
            center = placement->position;
            g_510c80 = placement->position;
            g_46dd4c = 0;
            function_17ee20((s_decal_mesh_view const *)g_4e0340, 0, tag_index,
                placement, vector, radius, deterministic, unknown1, unknown2);
        }
        else if (placement->unknown00 == 3)
        {
            s_decal_bound_17e490 *bound = &((s_decal_bsp_17e490 *)g_4e0348)->bounds[placement->unknown3c];
            short mesh_index = *(short *)((byte *)bound + 0x34);
            byte *meshes = *(byte **)((byte *)g_4e0348 + 0x13c);
            s_decal_mesh_view const *mesh = (s_decal_mesh_view const *)(meshes + mesh_index * 0xc8 + 0x70);
            transform4x3f const *matrix = (transform4x3f const *)bound;
            if (placement != &collision)
                decal_placement_copy(placement, &collision);
            excluded = placement->unknown3c;
            center = placement->position;
            function_142700(matrix, &collision.position, &collision.position);
            function_1428b0(matrix, &collision.plane, &collision.plane);
            vector3f local_direction;
            function_1427f0(matrix, vector, &local_direction);
            g_510c80 = center;
            g_46dd4c = 1;
            if (function_17ee20(mesh, matrix, tag_index, &collision, &local_direction,
                radius, deterministic, unknown1, unknown2) && unknown2)
                function_1429d0(matrix, 4, (point3f const *)unknown2, (point3f *)unknown2);
        }
    }
    if (deterministic)
    {
        g_4e7408->seed = saved_seed;
        return;
    }
    if (!(definition->maximum_radius * 2.0f > 0.2f) || (!world_hit && excluded == -1))
        return;
    real search_radius = (real)((double)radius * (double)definition->maximum_radius * 0.25);
    s_decal_bound_17e490 *nearby[1024];
    long count = function_17e490(excluded, cluster, &center, search_radius, 1024, nearby);
    for (long i = 0; i < count; ++i)
    {
        s_decal_bound_17e490 *bound = nearby[i];
        short mesh_index = *(short *)((byte *)bound + 0x34);
        byte *meshes = *(byte **)((byte *)g_4e0348 + 0x13c);
        s_decal_mesh_view const *mesh = (s_decal_mesh_view const *)(meshes + mesh_index * 0xc8 + 0x70);
        s_decal_placement adjacent;
        adjacent.unknown24 = -1;
        direction.i = bound->center.x - center.x;
        direction.j = bound->center.y - center.y;
        direction.k = bound->center.z - center.z;
        if (function_30bf0(&direction) >= 0.0001f)
        {
            point3f origin;
            origin.x = direction.i * 0.01f + center.x;
            origin.y = direction.j * 0.01f + center.y;
            origin.z = direction.k * 0.01f + center.z;
            real length = search_radius * 2.0f;
            direction.i *= length;
            direction.j *= length;
            direction.k *= length;
            if (function_1697c0(0x800005, &origin, &direction, -1, -1,
                (s_collision_result_1697c0 *)&adjacent) && adjacent.unknown00 == 3
                && &((s_decal_bsp_17e490 *)g_4e0348)->bounds[adjacent.unknown3c] == bound)
            {
                transform4x3f const *matrix = (transform4x3f const *)bound;
                function_142700(matrix, &adjacent.position, &adjacent.position);
                function_1428b0(matrix, &adjacent.plane, &adjacent.plane);
                function_1427f0(matrix, &direction, &direction);
                g_46dd4c = 2;
                function_17ee20(mesh, matrix, tag_index, &adjacent, &direction,
                    radius, false, unknown1, 0);
            }
        }
    }
    if (!world_hit)
    {
        vector3f axes[6] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
        real length = search_radius * 2.0f;
        for (long j = 0; j < 6; ++j)
        {
            point3f origin;
            origin.x = axes[j].i * 0.01f + center.x;
            origin.y = axes[j].j * 0.01f + center.y;
            origin.z = axes[j].k * 0.01f + center.z;
            vector3f ray;
            ray.i = axes[j].i * length;
            ray.j = axes[j].j * length;
            ray.k = axes[j].k * length;
            s_decal_placement world;
            world.unknown24 = -1;
            if (function_1697c0(0x20800007, &origin, &ray, -1, -1,
                (s_collision_result_1697c0 *)&world) && world.unknown00 == 1)
            {
                g_46dd4c = 3;
                if (function_17ee20((s_decal_mesh_view const *)g_4e0340, 0, tag_index,
                    &world, &ray, radius, false, unknown1, 0))
                    break;
            }
        }
    }
}
