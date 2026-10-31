// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17D9A0.CPP: decal placement and rendering (0x17d9a0 onwards) */

#include "unknown_11c920.h"
#include "vector_products.h"
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
	struct s_edge { real i, j; } edges[2];
	real inverse_determinant;
};

short function_120850(vector3f const *v);

PRIVATE inline void decal_project_corner_17d9a0(transform4x3f const *matrix, real const *x, real const *y, s_decal_quad_17d9a0 *quad, point2f *out)
{
	point3f point;
	short const *axes = g_440b94[quad->axis * 2 + quad->positive];
	point.x = matrix->forward.i * *x + matrix->left.i * *y + matrix->position.x;
	point.y = matrix->left.j * *y + matrix->forward.j * *x + matrix->position.y;
	point.z = matrix->forward.k * *x + matrix->left.k * *y + matrix->position.z;
	short horizontal_axis = axes[0];
	short vertical_axis = axes[1];
	out->x = ((real const *)&point)[horizontal_axis];
	out->y = ((real const *)&point)[vertical_axis];
}

// score 138 (was 138): frame 0xc and matrix/dot prefix agree; plane-store base, projection operands/publication and inverse schedule remain.
// At 158: plane-distance store base and projection/edge scheduling remain; both indices long 179f, horizontal long 164f, vertical long 179f, captured-index Y-first 164f, plane view after normal copy 181f.
// Read-only projection metadata 164; selector field inputs 164; output reference 164; shared
// point construction 162; vertical index first 164; table row before point 158 (kept); complete
// edge predicate 166; row helper input 160; inverse at entry 230; normal block copy 168; copy-
// return plane receiver 158 at 993 bytes; copy return with early row 171; early row with shared
// point construction 160.
// At 152: plane store base and projection/edge scheduling still differ.
// return native destination 158f; native component array 158f.
// transform workspace name 158f; native output first contract 168f.
// native matrix reference 158f; copied normal receiver 161f.
// nested edge validation 158f; conditional inverse publication 152p.
// At 138: plane-distance base, projection selectors/stores and inverse schedule remain.
// Shared projection operation 334; native plane result 237; row-selector local 152;
// bounds-first/vertical-first helper contracts 140/140; pointer-input point initializer 152;
// vertical result snapshot 170; first-edge squared length 138; corrected contract/length combinations 146/146.
// At 138: Plane-distance base, projection selectors/stores and inverse scheduling remain. external output record
// 146; external corner array 138; output and corner array 146; external normal receiver 166; external
// bounds record 146; matrix memcpy 138; bounds memcpy 152; normal memcpy current length 152.
// Plane-distance base, projection publication and inverse scheduling still differ.
// corner input references 144f; origin corner value 172f; first edge value 156f; second edge
// value 166f; both edge values 183f; first eligibility result 146f; indexed bounds contract 142f;
// indexed bounds with origin value 174f.
// plane-distance store base, selector publication and edge/zero schedule remain.
// Actual table-element bindings both/horizontal/vertical 188/138/188; both plus Y-first 191;
// distance-field binding 146; metadata object reference 138; native row reference 138;
// edge records bound after publication 142 at 992 bytes (fast scores).
// plane-store base, selector/output scheduling and inverse registers remain.
// Distance-only plane scope 143; second-edge squared result 146; both 147; external basis
// references 152; first/all bound-first products 138/138; basis plus lengths 154; completed
// Y/X projected values plus lengths 158.
// At 138: plane-distance store base, product operand order and the positive register differ.
// Plane pointer for the normal copy/dot/store 157 (store base as retail, dot operands flip); point-first
// dot through it 153; pointer for copy and store only 151; projected y stored first 138; the existing
// function_120810 for each corner projection 189 at 966 bytes, though its inlined lookup is retail's
// exact sequence (start from it: the products then reorder).
// Existing projection with all parallel products 191; point setter 189; bound-first products 189; matrix/bound/basis references 189/189/189; all parallel offsets 185; setter plus matrix reference 189. Restored.
// Overlap-safe matrix/bounds/both/normal/matrix-normal/bounds-normal/all transfers
// 253/237/273/210/262/234/282; all plus existing final projection 294. Original body and private helper
// restored.
// Round 288: complete shared corner-vector producers, forward/left affine and two-scale sums,
// each with private/shared final projection 265, 250, 265, 250, 265, 241, 265, 241. Original body and helper restored.
// Native side field byte/signed-char/char 142/142/142; with existing final projection 193/193/193;
// byte/signed-char side with a reused short axis result 164/164. Original Boolean field, body and helper restored.
// Lane 14 r11: the existing projection helper 0x120810 per corner 189 (966 bytes; its lookup is
// retail's `shl`/`movsx` sequence, the products reorder); with natural row orders 189. Edges typed
// point2f and formed by the existing vector2d_from_points2d 142 (992 bytes; aligned 152 -> 138:
// retail's four edge stores then the reloads); on that, the inverse as if/else 157, as one inline
// ternary 156, both lengths named 150, first length named with if/else 143 (986 bytes, aligned 131:
// zero path stores the shared zero as retail; the epsilon load is hoisted and the projection lookup
// and product orders remain). Next: the edge helper with if/else plus a form for the lookup.
// Round 107 (fast; aligned count = unmatched instructions after alignment, base 94/93): bounds by
// value to the corner helper 192; corner helper forceinline 138 (identical); edges through
// vector2d_from_points2d with an if/else inverse 143 (986 bytes, aligned 86/87: the tail is retail's
// but for the epsilon load and the determinant reusing e1.j); that plus each corner through the
// existing 0x120810 180 (975 bytes, aligned 50/54: lookup, stores and tail retail's; left are the
// products' operand order per corner, retail's `mov al, [positive]` before each corner's products
// with `movzx` at the index, the first corner's axis in ax/positive in cl, and the plane store
// through the normal's base); with real_point3d_set as the lookup's argument 180; with the side
// byte read before the point 180 (aligned 56/60).
// Lane 15 r17, on the existing 0x120810 per corner (189): plane pointer 198, with a point-first dot
// 196 (store base as retail; the first two dot products then load the point first); edges as a
// vector2f through vector2d_from_points2d with the inverse as if/else 182 (975 bytes; the tail is
// then retail's but for the first cross product, which loads e0.i where retail copies e1.j's
// register); natural cross order 182 and retail's corner-1 product operand orders 182 (both identical
// code: in this form every product loads the matrix field first whatever the source order).
// Round 129: in the corner helper, the table row as `positive + axis * 2` 138; a named long row
// index 140 (aligned 152 -> 121: later corners' product operand orders move toward retail's); with
// y stored before x 140 (aligned 119); row index named in axis-first order 140 (aligned 121); both
// coordinates read through the row index into named values 176 (970 B). Retail's lookup is
// `shl eax,1` + two movsx loads, y copied through xmm0 and stored first.
// Round 17 (m3 lane 2, fast): the corner helper's lookup through the existing function_120810 189
// (966 bytes); with the edges through vector2d_from_points2d and an if/else inverse 182 (975
// bytes); that with a plane pointer for the normal copy, dot and store 190; that with the axis in a
// short local used for the store and the normal index 185; the axis local on the current body 160.
// Retail's first corner keeps the axis in AX (re-extended at the lookup) and the side in CL; ours
// keeps one sign-extended axis in ECX and the side in AL, so 4 corners x 2 bytes are missing.
// Round 10 (lane 17, fast): the corner point through the existing matrix_transform_vector of {x, y, 0} plus the
// position 444 (1171 bytes: the up terms are not folded); the corner point as a `real point[3]` passed to the
// existing function_120810 185 (970 bytes); that with the edges through vector2d_from_points2d and an if/else
// inverse 180 (975 bytes); that with the point built inside the call's first argument 180 (same code: the axis
// and side loads do not move ahead of the products); the plane set through a small inline helper taking
// (plane, normal, point) 151: the normal copy, the d store through the normal's base `[eax+0xc]` and the call
// argument are then retail's, only the dot's i term loads the point first, but every corner product's operand
// order changes with it (the order follows something global, not the corner's own source).
// Round 104 (lane 8): a compile-only screen reproduces the corner product operand orders exactly; they
// shift with the function's symbol numbering (1-8 unused locals at the top or in the corner helper,
// diagnostic only, give 7-24 product mismatches against the current 10), so they follow how many
// locals and inline temporaries the source has. Real: bounds typed box2f with named corner fields 138
// (identical); the corner point as a three-component loop with the table row first 139 (985 bytes);
// corner helper taking axis/side by value through the existing 0x120810 191 (966 bytes); the
// component loop through 0x120810 191; corners through quad->corners without the pointer local 193
// (953 bytes). Screens: 384 helper shapes on 0x120810 19-24 mismatches (only row order counts).
// Best screen 7 with four extra function-scope locals and one in the helper: retail's body may have
// more named locals than ours (in combination, not singly).
// @retail 0x17d9a0
void function_17d9a0(transform4x3f const *matrix, real const *bounds, s_decal_quad_17d9a0 *quad)
{
	quad->matrix = *matrix;
	quad->bounds = *(box2f const *)bounds;
	quad->plane.n = matrix->up;
	quad->plane.d = dot3f(&quad->plane.n, (vector3f const *)&matrix->position);
	quad->axis = function_120850(&quad->plane.n);
	quad->positive = ((real const *)&quad->plane.n)[quad->axis] > 0.0f;
	point2f *corners = quad->corners;
	decal_project_corner_17d9a0(matrix, &bounds[0], &bounds[2], quad, &corners[0]);
	decal_project_corner_17d9a0(matrix, &bounds[1], &bounds[2], quad, &corners[1]);
	decal_project_corner_17d9a0(matrix, &bounds[1], &bounds[3], quad, &corners[2]);
	decal_project_corner_17d9a0(matrix, &bounds[0], &bounds[3], quad, &corners[3]);
	quad->edges[0].i = corners[1].x - corners[0].x;
	quad->edges[0].j = corners[1].y - corners[0].y;
	quad->edges[1].i = corners[3].x - corners[0].x;
	quad->edges[1].j = corners[3].y - corners[0].y;
	real first_edge_squared = quad->edges[0].i * quad->edges[0].i + quad->edges[0].j * quad->edges[0].j;
	quad->inverse_determinant =
		first_edge_squared > 0.0001f &&
		quad->edges[1].i * quad->edges[1].i + quad->edges[1].j * quad->edges[1].j > 0.0001f
		? 1.0f / (quad->edges[1].j * quad->edges[0].i - quad->edges[0].j * quad->edges[1].i) : 0.0f;
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
struct vector2f { real i, j; };
struct plane2f { vector2f n; real d; };
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
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

PRIVATE __forceinline real decal_normal_angle_17fd20(vector3f const *a, vector3f const *b)
{
	if (*(dword const *)&a->i == *(dword const *)&b->i &&
		*(dword const *)&a->j == *(dword const *)&b->j &&
		*(dword const *)&a->k == *(dword const *)&b->k)
		return 0.0f;
	real cosine = a->i * b->i + a->k * b->k + a->j * b->j;
	cosine = cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
	return (real)acos(cosine < -1.0f ? -1.0f : cosine > 1.0f ? 1.0f : cosine);
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

// score 564 (was 564): frame 0x21dc and 3647-byte length agree; first +0x41 is the definition-pointer spill.
// frame 0x21dc and retail length agree; definition spill +0xac versus +0x94, count/plane homes and rotation spills remain.
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
	for (short i = 0; i < face_count; ++i)
	{
		function_17dd80(mesh, &projection->quad, output, faces[i], true, projection->radius,
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
					axis.i *= inverse;
					axis.j *= inverse;
					axis.k *= inverse;
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
