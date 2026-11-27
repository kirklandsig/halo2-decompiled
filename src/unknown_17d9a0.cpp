// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17D9A0.CPP: decal placement and rendering (0x17d9a0 onwards) */

#include "unknown_11c920.h"
#include "vector_products.h"
#include "vector2f_products.h"
#include "segment_sphere.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
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
		}
		else
		{
			count = 1;
			clusters[0] = cluster_index;
		}
	}
	if (count > 512)
		count = 512;
	long cluster_count = count;
	long result = 0;
	dword visited[32];
	memset(visited, 0, sizeof(visited));
	if (excluded != NONE)
		visited[excluded >> 5] |= 1 << (excluded & 31);
	for (long i = 0; i < cluster_count; ++i)
	{
		s_decal_cluster_17e490 *cluster = &((s_decal_bsp_17e490 *)g_4e0348)->clusters[clusters[i]];
		for (long j = 0; j < cluster->count; ++j)
		{
			long index = cluster->indices[j];
			dword bit = 1 << (index & 31);
			if (!(visited[index >> 5] & bit) && result < maximum_count)
			{
				s_decal_bound_17e490 *bound = &((s_decal_bsp_17e490 *)g_4e0348)->bounds[index];
				real total_radius = bound->radius + radius;
				real x = point->x - bound->center.x;
				real y = point->y - bound->center.y;
				real z = point->z - bound->center.z;
				real distance = x * x;
				distance += z * z;
				distance += y * y;
				if (distance <= total_radius * total_radius)
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
inline void decal_project_point_17d9a0(real const *point, short axis, bool side, point2f *out)
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
	bool reverse, point3f const *start, s_decal_quad_17d9a0 const *quad, real radius, short mode, long *adjacent, short *count)
{
	if (*count < 1024)
	{
		point3f const *end = &mesh->vertices[edge->vertices[reverse]].position;
		vector3f direction;
		vector3d_from_points3d(start, end, &direction);
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

// score 387 (was 400) in a full build with stand-in SDK headers: retail computes the projection table index (axis *
// 2 + positive) at each projection, with the side as a by-value bool (0x143-0x161 for the first vertex, 0x180-0x199
// for the current one); the projection is an inline helper called for both. Left: frame 0x58 against retail's 0x5c,
// face_index in edi (retail esi), mesh in esi (retail ebx).
// M3 lane 13 r163 (h2m fast with 0x17e670, 0x17fd20 and the callees; plain check 400): kept, 423 -> 400 and frame 0x5c as
// retail's: the file's existing decal_project_point_17d9a0 at both projections in place of a second copy of it, the
// first vertex passed without its one-use pointer, and 0x11cf50 given the face plane first (retail's call passes the
// quad normal in EAX and the plane in ECX; 0x11cf50 is symmetric); the edge direction through the existing
// vector3d_from_points3d (same code). Steps: the 0x11cf50 order alone 433 (1821 B), with the 0x17d900 call ahead of
// `radius * 0.05f` 433; the helper alone 435 (1805 B), with both 449; the helper and no `first` pointer 408. On the kept
// body: the call-first comparison 400 (same count; retail's order there); the vertex's one-use pointer dropped too 438
// (frame back to 0x58); the normal offset through vector3f_add_scaled 400 (same code); `face_index == edge->faces[1]` 400
// (same code). Left: the quad normal's address in EBX (retail EDI), face_index EDI and mesh ESI (retail ESI and EBX);
// in the loop the vertex pointer ranks 254 and takes ECX ahead of the first projection's temporaries, so `reverse` gets
// EBX where retail has vertex ESI, edge EDI, polygon_points EBX and `reverse` in DL with its home in the dead `mode`
// slot; the output loop keeps the plane normal and the 1/256 constant in registers across 0x120790 where retail reloads
// them after each call (a callee register-set question). A forced-ranking search found no single decision below 400
// (the adjacent block's start pointer in ESI 428, the vertex in ESI 405).
// M3 lane 7 r235 (plain checks; the reduced image builds this function at 403-409 and 1783 B, so it is not used):
// kept, 400 -> 387 at retail's 1799 B: the adjacent-face helper takes the edge's start vertex from its caller (the
// clipping loop passes its `vertex`, the gather loop the same element address): retail reads the start through the
// clipping loop's vertex register after the clip call, with no new vertex-table load, and the gather loop forms it
// ahead of the count test. That gives retail's homes (face_index ESI, mesh EBX, the quad normal's address EDI) and
// frame. Lead (an evaluation-order swap, kept only with a match): the 0x17d900 call first in the comparison
// (`function_17d900(...) < radius * 0.05f`, retail's order: the call, then the radius load) 381. Left: the loop's
// iteration count stays in EDX from the latch into the loop top (retail re-reads it from its slot, and its slot sorts
// third where ours sorts sixth), the head's edge/faces loads in another order, and the output loop keeping the plane
// normal and 1/256 in XMM3-6 across 0x120790 (retail reloads them after each call). Tried on the 381 lead: the
// increment last in the body 382; the previous-point copy ahead of the increment 381; the copy written per field 425
// (1778 B); the gather loop's start named like the clipping loop's `vertex` 381 (same code). Also kept: the clip
// edge's normalisation through the existing normalize2d_fast in place of its body written out (same code, 381 on the
// lead).
// M3 lane 11 r175 (the reduced image builds this function exactly, 387 at 1799 B, once 0x22bbca is listed with it: that
// file holds the point2f_set the projection inlines; without it the projection stays a call): retail lays the
// clipping loop's adjacent-face add (the store after the search loop) out after the output loop's preheader, where
// ours keeps it after the search loop (about 9 rows of positional shift); the gather loop's copy sits in place in both.
// Same code, layout unmoved: the search loop as a `for` 387; the helper with early returns for the count and the
// sphere test 387; the face started as NONE with the add after the sphere test 387. The projection ahead of the buffer
// selection 439 (1808 B), with the 0x17d900 call first 439. Re-measured: the call first in the comparison 381. Reading:
// a forced-ranking search that puts the iteration counter's latch piece first in an early pass gives 296 with retail's
// memory operands at the loop head (`cmp word ptr` on its slot); no single decision moves the add's layout. The XMM
// constants held across 0x120790 (retail reloads them after each call) look like a caller compiled without the
// callee's register summary.
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
	real angle = function_11cf50(&plane.n, &quad->plane.n);
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
			if (iteration == 0)
			{
				decal_project_point_17d9a0((real const *)&mesh->vertices[edge->vertices[reverse]].position,
					quad->axis, quad->positive, &previous);
			}
			point2f current;
			decal_project_point_17d9a0((real const *)vertex, quad->axis, quad->positive, &current);
			plane2f clip;
			clip.n.i = previous.y - current.y;
			clip.n.j = current.x - previous.x;
			real length = normalize2d_fast(&clip.n);
			if (length != 0.0f)
			{
				clip.d = clip.n.j * current.y + clip.n.i * current.x;
				bool clipped;
				point_count = function_23a6f0(polygon_points, point_count, points, &clip, 12, &point_mask, &clipped, 0.0f);
				if (gather && clipped)
					decal_add_adjacent_17dd80(mesh, edge, reverse, vertex, quad, radius, mode, adjacent, &count);
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
			decal_add_adjacent_17dd80(mesh, edge, reverse, &mesh->vertices[edge->vertices[!reverse]].position, quad, radius,
				mode, adjacent, &count);
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
	real cosine = dot3f(a, b);
	cosine = cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
	return function_50650(cosine);
}

PRIVATE __forceinline void decal_rotate_vector_17fd20(transform4x3f const *m,
	vector3f const *v, vector3f *out)
{
	real x = v->i, y = v->j, z = v->k;
	real i = m->up.i * z;
	i += m->left.i * y;
	i += m->forward.i * x;
	out->i = i;
	real j = m->up.j * z;
	j += m->left.j * y;
	j += m->forward.j * x;
	out->j = j;
	real k = m->up.k * z;
	k += m->left.k * y;
	k += m->forward.k * x;
	out->k = k;
}

PRIVATE __forceinline void decal_matrix3x3_rotation_17fd20(matrix3x3 *m, vector3f const *axis, real sine, real cosine)
{
	vector3f squares;
	squares.i = axis->i * axis->i;
	squares.j = axis->j * axis->j;
	squares.k = axis->k * axis->k;
	vector3f sine_axis;
	sine_axis.i = axis->i * sine;
	sine_axis.j = axis->j * sine;
	sine_axis.k = axis->k * sine;
	m->forward.i = (1.0f - squares.i) * cosine + squares.i;
	m->left.i = (1.0f - cosine) * axis->j * axis->i - sine_axis.k;
	m->forward.j = (1.0f - cosine) * axis->j * axis->i + sine_axis.k;
	m->left.j = (1.0f - squares.j) * cosine + squares.j;
	m->up.i = (1.0f - cosine) * axis->k * axis->i + sine_axis.j;
	m->forward.k = (1.0f - cosine) * axis->k * axis->i - sine_axis.j;
	m->up.k = (1.0f - squares.k) * cosine + squares.k;
	m->up.j = (1.0f - cosine) * axis->k * axis->j - sine_axis.i;
	m->left.k = (1.0f - cosine) * axis->k * axis->j + sine_axis.i;
}

PRIVATE __forceinline void decal_transform_rotation_17fd20(transform4x3f *matrix, vector3f const *axis, real sine, real cosine)
{
	matrix->scale = 1.0f;
	decal_matrix3x3_rotation_17fd20(&matrix->rotation, axis, sine, cosine);
	matrix->position.z = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.x = 0.0f;
}

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
	short face_count = 1;
	short secondary_count = 0;
	for (short i = 0; i < face_count;)
	{
		long face = faces[i++];
		function_17dd80(mesh, &projection->quad, output, face, true, projection->radius,
			*(short const *)(definition + 2), faces, &face_count, secondary, &secondary_count);
	}
	if ((byte)g_444a38[*(short const *)(definition + 2)].flags)
	{
		short remaining = secondary_count;
		while (remaining > 0)
		{
			short group_count = 0;
			for (short i = 0; !group_count && i < secondary_count; ++i)
			{
				long first_face = secondary[i];
				if (first_face == NONE)
					continue;
				short j;
				long face;
				plane3f first_plane;
				decal_face_plane_17fd20(mesh, first_face, &first_plane);
				faces[group_count++] = first_face;
				secondary[i] = NONE;
				for (j = i + 1; j < secondary_count; ++j)
				{
					face = secondary[j];
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
				for (j = 0; j < group_count; ++j)
				{
					face = faces[j];
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
							maximum = da;
							minimum = db;
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
							selected_minimum = minimum;
							selected_maximum = maximum;
							end = *b;
							selected_face = face;
						}
						edge_index = edge->next_edges[reverse];
					} while (edge_index != first_edge);
				}
				vector3f axis;
				vector3d_from_points3d(&start, &end, &axis);
				s_decal_quad_17d9a0 quad;
				if (normalize3d(&axis) > 0.0f)
				{
					vector3f cross;
					cross3f(&selected_plane.n, &projection->quad.plane.n, &cross);
					real sign = dot3f(&cross, &axis) < 0.0f ? 1.0f : -1.0f;
					real angle = decal_normal_angle_17fd20(&selected_plane.n, &projection->quad.plane.n) * sign;
					real sine = (real)sin(angle);
					real cosine = (real)cos(angle);
					transform4x3f rotation;
					decal_transform_rotation_17fd20(&rotation, &axis, sine, cosine);
					transform4x3f matrix;
					vector3d_from_points3d(&start, &projection->matrix.position, (vector3f *)&matrix.position);
					decal_rotate_vector_17fd20(&rotation, (vector3f *)&matrix.position, (vector3f *)&matrix.position);
					decal_rotate_vector_17fd20(&rotation, &projection->matrix.forward, &matrix.forward);
					decal_rotate_vector_17fd20(&rotation, &projection->matrix.left, &matrix.left);
					decal_rotate_vector_17fd20(&rotation, &projection->matrix.up, &matrix.up);
					matrix.position.x += start.x;
					matrix.position.y += start.y;
					matrix.position.z += start.z;
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
				for (j = 0; j < group_count; ++j)
				{
					function_17dd80(mesh, &quad, output, faces[j], false, projection->radius,
						*(short const *)(definition + 2), 0, 0, 0, 0);
				}
				remaining -= group_count;
			}
		}
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
    bool result = false;
    s_decal_chain_state_17ee20 state;
    memset(&state, 0, sizeof(state));
    // retail stores the flag byte again after the seven dword clears
    state.reuse = false;
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
    vector3f scaled;
    if (m->scale != 1.0f)
        v = vector3f_scale(v, m->scale, &scaled);
    decal_rotate_vector_17fd20(m, v, out);
}

// score 837 (was 859; earlier 966, 967, 988 at r914, 987): the random angle, sequence choice and radius fraction drawn
// through the header's existing function_259d0, random_index and function_x82e52f (the file-local copies of the seed
// step removed) 849, and the flag tested as a plain mask (`definition->flags & 8`) 837 (machine 3 review trials on GA
// r29's 13-function fast list; full check 0 lost). M3 lane GA r29 reached 770 (3571/3591 B, +0x8, retail's `sub esp,
// 0x60` frame in place of 0x90, full check 0 lost), not kept at review: it writes out by hand, as scalar locals, the
// scale and rotate in decal_vector_17ef10, the transform of the placement position and the basis scale (existing
// helpers' bodies written out are reverted), drops the multi-use `matrix` and `bitmap` locals for repeated
// `preparation->` reads, fills an empty slot inside the loop as a second copy of the fill after it, and changes operand
// and store orders (kept only with a byte match). With a named angle local as well 835 (a gain of 2, not kept). What
// still differs in the 770 form: prologue `xor edi, edi` / `&placement->position` slot `[esp+0x20]` vs `[esp+0x14]`,
// loop 1 `count` in `esi` (retail spills `count` to `[esp+0x8c]` and keeps `g_4ea94c` in `esi`),
// `tangent`/`perpendicular` slot swap (`[esp+0x18..0x20]`/`[esp+0x24..0x2c]` vs retail
// `[esp+0x24..0x2c]`/`[esp+0x14..0x1c]`), `inverse` spill in normalize, `unknown1` loaded 32-bit vs `mov cx, word ptr`,
// and `0x12ce00`'s `movaps xmm1, xmm4` vs `xorps xmm1, xmm1`.
// M3 lane GA r29 (56 fast trials across 6 batches on 13-VA exact fast list with 11/11 matched peers MATCH; plain confirmed
// 770): full bundle with `bool placed` + 3 inline helpers (`decal_apply_point`/`vector_inverse`/`recent_fill`) 667 (3565 B,
// +0x21; without tail fixes 808); flag-free without extra helpers: `chosen = -1; break` with `real scale` 770 (the round's kept form, not kept at review; with
// 2-loop `recent` pointer 756/820, with `j == 24` 885/887, without `real scale` 824 at exact 3591 B); `bool placed` without
// extra helpers 684 (3571 B, 865/865 insns; with `real scale` 759/823, with `placement_position` ptr 769/817/852/862 at
// +0x21, `short unknown1` 744/754/810 at exact 3591 B); single-fill loop 895; keeping `bitmap` local 879; keeping `matrix`
// local 864; keeping orig `decal_vector_17ef10` 996 (`sub esp, 0x8c`); `cross3f` 865; `normalize_inline` 862; renaming
// `tangent`/`perpendicular` to `axis_u`/`axis_v` 852/817 (same slots); `count` before `scale == 0` 876 (`sub esp, 0x64`);
// caching `recent` across all 3 sites 935 (`sub esp, 0x68`).
// Earlier (859, was 966; earlier 967, 988 at r914, 987): retail keeps a bool result set true at entry in a dead argument home
// (0x17ef59) and returns it at the success exit (0x17fcc6) while both failures return 0, so the texture failure clears
// `result` and the projection is built only when it holds. Earlier: a mask test read as a field bit (TEST_FIELD_BIT) gives
// retail's load-and-shift; the rest still differs. (966 was the full build with stand-in SDK headers, 967 the reduced
// image: the result is a bool set true at entry, retail storing 1 into a dead argument home at 0x17ef59 and reading it
// back at the success exit, the texture failure clears it, and the 0x17d9a0 call and the orientation stores run under
// if (result).)
// Round 160 (m3 lane 14, fast and plain): kept, the scale-and-rotate helper as the existing vector3f_scale (into a local
// only when the scale is not 1) followed by the file's existing rotate helper (0x17fd20's, same row order): 966 -> 899
// (3551 B; aligned with esp offsets masked 1075 -> 1058 unaligned, exact aligned 226 -> 248); then the recent-decal
// array read from the decal globals at each use (no cached `recent` pointer; retail re-reads the global for the chosen
// entry): 899 -> 888 (3562 B, masked unaligned 1013, exact aligned 268); then the direction's inverse scale in the
// basis block through the existing vector3f_scale: 888 -> 859 (3578 B). On that block: the copy member by member with
// `*=` 893, with `inverse *` 893, with vector3f_scale 893; the copy kept and `inverse *` 889; vector3f_scale of the
// direction into the local or the plain copy in an else arm 858 (fast, -2 against the kept 860). decal_time_17ef10 as
// if/return 889 (same code), with a result local 900. On 860 (fast): the tangent and perpendicular crosses through
// cross3f in either arm or both 866 (same code each); the facing test through dot3f 897; the eight-direction loop's
// normalization through normalize2d_fast (dot y-first or x-first) 869. Its transform block is not
// retail's shape (retail keeps the scaled x, y, z in XMM0-2, scale in XMM3, each row then + the position): the copy goes
// through memory here. Tried: the transform reading placement->position directly with the position added in place 963
// (closest transform block: exact aligned 257); that with the output into a local first 972; the helper's rows as staged
// sums 972 (with the direct read 969); a whole copy of the input scaled member by member then rotated 902 (with the
// direct read 929), copied member by member with the direct read 1051 (frame 0x84), scaled through vector3f_scale 926. Left: frame 0x90
// (retail 0x60: retail keeps `count` in unknown2's home and `result` in preparation's), the recent-decal loops (retail's
// pointer walks the entry at +0x24 and keeps the loop index in EDI, the squared gaps sum x, z, y with the radius squared
// last; the test written sum-first 899 before the global change, 889 fast after it), the oldest-slot loop (retail duplicates the entry fill
// for the break path on the loop's own pointer) and every later block.
// M3 lane 12 r195 (fast): retail inlines existing transform helpers here, which our link keeps as calls (their
// file is /Ob1): the transform block through transform4x3f_apply_point 1163 (3288 B), the basis block's vector
// transforms through 0x142640 1295, plus the direction through 0x1427f0 1433, the texture fetch through 0x12360 964.
// The file's local vector-transform helper scaling x, y, z in registers (0x142640's shape) in place of the scaled
// copy: 975 (3476 B) with the frame down from 0x90 to 0x68 (retail 0x60); with the position read from the placement
// directly 965. Lead: that helper shape takes 0x28 off the frame.
// @retail 0x17ef10
bool function_17ef10(transform4x3f const *transform, long tag_index,
    s_decal_placement const *placement, vector3f const *direction, real radius,
    bool unknown0, long unknown1, long unknown2,
    s_decal_preparation_17ef10 *preparation, s_decal_chain_state_17ee20 *state)
{
    s_decal_definition_17ef10 const *definition =
        (s_decal_definition_17ef10 const *)g_4e3b44[tag_index & 0xffff].bytes;
    bool result = true;
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
        long count = 0;
        for (long i = 0; i < 24; ++i)
        {
            s_decal_recent_17ef10 const *entry = &((s_decal_recent_17ef10 *)((byte *)g_4ea94c + 0x380c))[i];
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
            s_decal_recent_17ef10 const *entry = &((s_decal_recent_17ef10 *)((byte *)g_4ea94c + 0x380c))[j];
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
            s_decal_recent_17ef10 *entry = &((s_decal_recent_17ef10 *)((byte *)g_4ea94c + 0x380c))[chosen];
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
    if ((definition->flags & 8) &&
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
                vector3f_scale(&local, 1.0f / basis.scale, &local);
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
        if (!state->reuse)
            state->angle = function_259d0(&g_4e7408->seed, NULL, 0, 0.0f, 6.2831854820251465f);
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
                state->sequence_choice = random_index(&g_4e7408->seed, (short)((short)bitmap->sequence_count - 1));
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
        state->radius_fraction = function_x82e52f(&g_4e7408->seed, NULL, 0);
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
                result = false;
        }
    }
    if (result)
    {
        function_17d9a0(matrix, (real const *)&preparation->projection.bounds, &preparation->projection.quad);
        preparation->projection.orientation_bounds[0] = preparation->projection.orientation_bounds[1] = normal.i;
        preparation->projection.orientation_bounds[2] = preparation->projection.orientation_bounds[3] = normal.j;
        preparation->projection.orientation_bounds[4] = preparation->projection.orientation_bounds[5] = normal.k;
    }
    return result;
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

// score 363 (was 430): kept in M3 lane 13 r163 (h2m fast with both matched callers 0x17a8a0 and 0x23aad0, plain check
// 363, callers MATCH): the deterministic restore and the neighbour search as one exit (search under `!deterministic &&
// radius test && (world hit || excluded bound)`, then `if (deterministic)` restore), 430 -> 368 at retail's 1787 B;
// `cluster` as a `long` (retail stores it as a sign-extended dword) 363 (428 on the old body). Left: frame 0x1178
// against 0x1170; retail keeps the vector in EBP, the source in ESI, the point in EDI and the sixth argument in BL with
// `excluded` in memory, where this build keeps `excluded` in EDI, the point in ESI and the source in EBX; the search
// radius is x87 with a float 0.25 in retail (a double constant here; `0.25f` in the double expression folds to the same
// double constant, 363). Leads, not kept: the sixth parameter as a `bool` (retail tests it as BL, pushes the register
// whole to 0x17ee20's bool argument and re-reads the argument byte at the end; the callers' declarations follow, both
// callers still match) 429 on the old body, 421 with the long cluster, 385 on the kept body; the restore test after the
// radius test (retail's test order) 390; the collision query only for a missing or type-4 source and that test repeated
// to pick the collision record, as retail does with no found flag, 414; the old body's returns with the three
// constant-initialised locals declared above the seed block 443; `deterministic` declared first 417; the placement
// pointer declared first 387.
// M3 lane 7 r235 (plain checks: the reduced image builds this function at 415 even with the whole file listed or the
// full ballast link): kept, the search radius as plain float math (`radius * definition->maximum_radius * 0.25f`,
// retail's x87 sequence and float constant, the double casts gone) 363. A frame trace explains the 8 extra bytes: the
// `deterministic` local takes a 4-byte slot and the case-3 mesh pointer is spilled, where retail keeps the sixth
// argument (a bool) in EBX and later the mesh there. Tried: the axes loop bounded by NUMBEROF(axes) 363 (retail's
// unsigned `jb`); the neighbour placement renamed to sort after the collision record 363 (its slot does not move);
// the sixth parameter as bool on this body 385 (frame 0x1174; the argument is read from memory and `excluded` keeps
// EDI), with the constant-initialised locals above the seed block 412, with the float radius 385; the radius with a
// double `0.25` and no casts 398 (1773 B).
// Gemini lane GA r31 (not kept): 338 at 1787 B (frame 0x1170; retail's registers for the vector, the source, the
// point and the sixth argument up to line 61) by dropping the `deterministic` local for the parameter test, declaring
// world_hit, excluded and cluster above the seed block, the seed as bits[2] ^ bits[1] ^ bits[0], the mesh as `meshes +
// mesh_index * 0xc8` with + 0x70 at the two calls, the tail guard in retail's test order (alone 355), `bound` on the
// left of the neighbour test, an unsigned axes counter and the existing vector3d_from_points3d and vector3f_scale for
// the direction and the rays. Most of that gain is declaration, statement and operand order, kept only with a byte
// match, so it is held as a lead; the rest alone (the parameter test, the two helpers and the unsigned counter in this
// body's order) is 393 in the full check. vector3f_add_scaled was left out: it adds a helper instance in this file and
// moves the whole-image canaries 0x1c4590 and 0x24cf66. Also from that round: retail's sixth parameter is a bool (with
// the callers changed, retail's mov ebx); retail tests `!source || source->unknown00 == 4` again after 0x1697c0 (4
// instructions, 1787 B) and keeps `collision` and `axes` at the same stack offset with `adjacent` at 0xc8. Tried: the
// bool parameter with a found flag, a scoped collision record, an outer adjacent record and plain axes, about 390; the
// short-circuit `(source && source->unknown00 != 4) || function_1697c0(...)` and its spellings 349 (retail's size and
// re-test, but -1 and the radius take EBP, the vector moves to EBX and the sixth argument to CL).
// @retail 0x17e670
void function_17e670(point3f const *point, s_effect_source *source, long tag_index,
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
    long cluster = -1;
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
    if (!deterministic && definition->maximum_radius * 2.0f > 0.2f && (world_hit || excluded != -1))
    {
        real search_radius = radius * definition->maximum_radius * 0.25f;
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
    if (deterministic)
        g_4e7408->seed = saved_seed;
}
