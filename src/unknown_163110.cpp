// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
#include "globals.h"
#include "unknown_13fd90.h"
#include "unknown_19c1d0.h"
#include "matrix_transform.h"
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <math.h>

#include "unknown_163110.h"
#include "vector_products.h"

struct s_22bbca { short top, left, bottom, right; };
void function_22bbca(s_22bbca const *bounds, long tag_index, long first, long second,
	long text_index, real alpha);

// @retail 0x163400
void function_163400(s_text_widget_a const *widget)
{
	if (g_4e034c && g_4e034c->index != NONE && widget->valid)
	{
		s_22bbca bounds = *(s_22bbca const *)&widget->bounds;
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		byte *data = *(byte **)(definition + 0xc);
		bounds.left += g_4b9dd2;
		bounds.right += g_4b9dd2;
		bounds.top += g_4b9dd0;
		bounds.bottom += g_4b9dd0;
		if (widget->text[0] == 0)
			function_22bbca((s_22bbca const *)&bounds, *(long *)(data + 0x54c),
				*(long *)(data + 0x554), 0, *(long *)widget->unknown34, widget->color_b.alpha);
		else if (widget->text[0] == 1)
			function_22bbca((s_22bbca const *)&bounds, *(long *)(data + 0x55c),
				*(long *)(data + 0x564), 0, *(long *)widget->unknown34, widget->color_b.alpha);
	}
}

// @retail 0x163110
s_text_buffer *text_buffer_format(s_text_buffer *buffer, const word *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnwprintf((wchar_t *)buffer->text, 0x4f, (const wchar_t *)format, arguments);
	buffer->text[0x4f] = 0;
	return buffer;
}

// @retail 0x163140
void s_text_widget_a::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x1631f0
void s_text_widget_b::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x1632a0
void s_text_widget_c::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x163350
void s_text_widget_d::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

struct s_loading_map_view
{
	long state;
	byte unknown04[0x14 - 4];
	long position_a;
	long position_b;
	char name[256];
};

// @retail 0x163610
char *function_163610()
{
	char *result = 0;
	s_loading_map_view *map = (s_loading_map_view *)&g_4e6948->state;
	if (map && map->state == 1)
	{
		long key0 = map->position_a;
		long key1 = map->position_b;
		if (key0 == NONE)
		{
			s_entry_a *entry = function_19c320(map->name);
			key0 = NONE;
			if (entry)
				key0 = entry->key0;
			entry = function_19c320(map->name);
			key1 = NONE;
			if (entry)
				key1 = entry->key1;
		}
		if (key0 != NONE)
		{
			long next = function_19c440(key0, key1);
			if (next != NONE)
			{
				s_entry_a *entry = function_19c270(key0, next);
				if (entry)
					result = entry->name;
			}
		}
	}
	return result;
}

class c_entry_list
{
public:
	c_entry_list(long maximum_count);
	bool add(long a, short b, long c, short d);
	short find(long a);
	void swap(short index0, short index1);

private:
	long maximum_count;
	word count;
	short *shorts_b;
	long *longs_a;
	long *longs_c;
	short *shorts_d;
};

// @retail 0x163c40
c_entry_list::c_entry_list(long maximum_count)
{
	longs_a = new long[maximum_count];
	shorts_b = new short[maximum_count];
	this->maximum_count = maximum_count;
	longs_c = new long[maximum_count];
	shorts_d = new short[maximum_count];
}

// @retail 0x163c80
bool c_entry_list::add(long a, short b, long c, short d)
{
	bool result = false;

	if (count < maximum_count - 1)
	{
		longs_a[count] = a;
		shorts_b[count] = b;
		longs_c[count] = c;
		shorts_d[count] = d;
		count++;
		result = true;
	}
	return result;
}

// @retail 0x163d20
short c_entry_list::find(long a)
{
	for (long i = 0; i < count; i++)
	{
		if (longs_a[i] == a)
			return (short)i;
	}
	return NONE;
}

// @retail 0x163d50
void c_entry_list::swap(short index0, short index1)
{
	long a = longs_a[index0];
	longs_a[index0] = longs_a[index1];
	longs_a[index1] = a;
	short b = shorts_b[index0];
	shorts_b[index0] = shorts_b[index1];
	shorts_b[index1] = b;
	long c = longs_c[index0];
	longs_c[index0] = longs_c[index1];
	longs_c[index1] = c;
	short d = shorts_d[index0];
	shorts_d[index0] = shorts_d[index1];
	shorts_d[index1] = d;
}

struct s_view
{
	byte unknown00[4];
	transform4x3f matrix;
	byte unknown38[0x54 - 0x38];
	point3f point54;
	byte unknown60[0x84 - 0x60];
	byte flag84;
	byte flag85;
	byte unknown86[0xb0 - 0x86];
	real margin;
	vector3f vector_b4;
	byte unknownc0[0xe4 - 0xc0];
	vector3f vector_e4;
};

struct s_bounds3d
{
	real x0, x1, y0, y1, z0, z1;
};

s_bounds3d *g_4687e0;

static void transform_point(const transform4x3f *matrix, const point3f *point, point3f *out)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// @retail 0x1650a0
bool function_1650a0(const s_view *view, const point3f *point, s_bounds3d *bounds, real radius)
{
	bool result = true;
	point3f p;
	transform_point(&view->matrix, point, &p);
	if (p.z > radius)
		result = false;
	else
	{
	long i = 0;
	real z2 = p.z * p.z;
	s_bounds3d b;
	real *bn = &b.x0;
	for (; i < 2; i++)
	{
		real d = (real)sqrt(p.n[i] * p.n[i] + z2);
		if (radius >= d)
		{
			bn[i * 2] = -10000.0f;
			bn[i * 2 + 1] = 10000.0f;
		}
		else
		{
			real a = (real)asin(radius / d);
			real angle = (real)atan2(p.n[i], -p.z);
			real lo = angle - a;
			if (lo < -1.5707964f)
				bn[i * 2] = -10000.0f;
			else
				bn[i * 2] = (real)tan(lo);
			real hi = angle + a;
			if (hi > 1.5707964f)
				bn[i * 2 + 1] = 10000.0f;
			else
				bn[i * 2 + 1] = (real)tan(hi);
		}
	}
	real z_lo = p.z - radius;
	b.z0 = z_lo > -1.5258800e-5f ? -1.5258800e-5f : z_lo;
	real z_hi = p.z + radius;
	b.z1 = z_hi > -1.5258800e-5f ? -1.5258800e-5f : z_hi;
	*bounds = b;
	}
	return result;
}
struct s_plane
{
	vector3f normal;
	real d;
};

struct s_frustum_1648d0
{
	long identifier;
	byte unknown04[4];
	long plane_index;
	byte unknown0c[4];
	box2f rectangle;
	byte unknown20[4];
	point3f points[5];
	s_bounds3d bounds;
	s_plane planes[6];
	vector3f directions[4];
};

struct s_camera_163db0
{
	byte unknown00[0x38];
	transform4x3f matrix;
	bool has_plane;
	byte unknown6d[7];
	s_plane plane;
	byte unknown84[4];
	real depth;
};

struct box3f;
box3f *function_11f770(box3f *rectangle, long count, point3f const points[]);
int __stdcall function_1429d0(transform4x3f const *matrix, long count, point3f const *source, point3f *destination);
real function_30bf0(vector3f *vector);

bool g_47ff86 = true;

PRIVATE inline void transform_plane_163db0(transform4x3f const *matrix, s_plane const *source, s_plane *plane)
{
	matrix_transform_vector(matrix, &source->normal, &plane->normal);
	real scale = matrix->scale;
	real x = matrix->position.x;
	real y = matrix->position.y;
	real z = matrix->position.z;
	real i = plane->normal.i;
	real j = plane->normal.j;
	real k = plane->normal.k;
	real distance = y * j;
	distance += source->d * scale;
	distance += z * k;
	distance += x * i;
	plane->d = distance;
}

/* the same transform, reading the matrix position and the normal through pointers */
PRIVATE inline void transform_plane_through_pointers_163db0(transform4x3f const *matrix, s_plane const *source, s_plane *plane)
{
	matrix_transform_vector(matrix, &source->normal, &plane->normal);
	point3f const *position = &matrix->position;
	vector3f const *normal = &plane->normal;
	real scale = matrix->scale;
	real distance = position->y * normal->j;
	distance += source->d * scale;
	distance += position->z * normal->k;
	distance += position->x * normal->i;
	plane->d = distance;
}

/* the same again, for the top and bottom planes, summing the normal's k term first */
PRIVATE inline void transform_vertical_plane_163db0(transform4x3f const *matrix, s_plane const *source, s_plane *plane)
{
	matrix_transform_vector(matrix, &source->normal, &plane->normal);
	point3f const *position = &matrix->position;
	vector3f const *normal = &plane->normal;
	real scale = matrix->scale;
	real distance = position->z * normal->k;
	distance += position->y * normal->j;
	distance += source->d * scale;
	distance += position->x * normal->i;
	plane->d = distance;
}

PRIVATE inline real plane_error_163db0(s_plane const *plane, point3f const *point)
{
	return (real)fabs(plane->normal.k * point->z + plane->normal.j * point->y + plane->normal.i * point->x - plane->d);
}

// @retail 0x163db0
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result)
{
	bool valid = false;
	if (rectangle->x1 > rectangle->x0 && rectangle->y1 > rectangle->y0)
	{
		point3f *points = &result->points[0];
		box2f const *rect = &result->rectangle;
		result->rectangle = *rectangle;
		result->identifier = identifier;
		real depth = camera->depth;
		real x0 = rect->x0 * depth;
		real x1 = rect->x1 * depth;
		real y0 = rect->y0 * depth;
		real y1 = rect->y1 * depth;
		real_point3d_set(&result->points[1], x0, y0, 0.0f - camera->depth);
		real_point3d_set(&result->points[3], x1, y0, 0.0f - camera->depth);
		real_point3d_set(points, x0, y1, 0.0f - camera->depth);
		real_point3d_set(&result->points[2], x1, y1, 0.0f - camera->depth);
		function_1429d0(&camera->matrix, 4, points, points);
		result->points[4] = camera->matrix.position;
		result->bounds = *g_4687e0;
		function_11f770((box3f *)&result->bounds, 5, result->points);
		vector3f_assign(&result->planes[0].normal, -1.0f, 0.0f, 0.0f - result->rectangle.x0);
		function_30bf0(&result->planes[0].normal);
		result->planes[0].d = 0.0f;
		transform_plane_163db0(&camera->matrix, &result->planes[0], &result->planes[0]);
		vector3f_assign(&result->planes[1].normal, 1.0f, 0.0f, result->rectangle.x1);
		function_30bf0(&result->planes[1].normal);
		result->planes[1].d = 0.0f;
		transform_plane_through_pointers_163db0(&camera->matrix, &result->planes[1], &result->planes[1]);
		vector3f_assign(&result->planes[2].normal, 0.0f, -1.0f, 0.0f - result->rectangle.y0);
		function_30bf0(&result->planes[2].normal);
		result->planes[2].d = 0.0f;
		transform_vertical_plane_163db0(&camera->matrix, &result->planes[2], &result->planes[2]);
		vector3f_assign(&result->planes[3].normal, 0.0f, 1.0f, result->rectangle.y1);
		function_30bf0(&result->planes[3].normal);
		result->planes[3].d = 0.0f;
		transform_vertical_plane_163db0(&camera->matrix, &result->planes[3], &result->planes[3]);
		if (camera->has_plane)
		{
			result->planes[4].normal.i = 0.0f - camera->plane.normal.i;
			result->planes[4].normal.j = 0.0f - camera->plane.normal.j;
			result->planes[4].normal.k = 0.0f - camera->plane.normal.k;
			result->planes[4].d = 0.0f - camera->plane.d;
		}
		else
		{
			vector3f_assign(&result->planes[4].normal, 0.0f, 0.0f, 1.0f);
			result->planes[4].d = 0.0f;
			transform_plane_163db0(&camera->matrix, &result->planes[4], &result->planes[4]);
		}
		vector3f_assign(&result->planes[5].normal, 0.0f, 0.0f, -1.0f);
		result->planes[5].d = camera->depth;
		transform_plane_163db0(&camera->matrix, &result->planes[5], &result->planes[5]);
		vector3d_from_points3d(&result->points[4], &result->points[0], &result->directions[0]);
		vector3d_from_points3d(&result->points[4], &result->points[1], &result->directions[1]);
		vector3d_from_points3d(&result->points[4], &result->points[2], &result->directions[2]);
		vector3d_from_points3d(&result->points[4], &result->points[3], &result->directions[3]);
		char const *error = 0;
		if (!(plane_error_163db0(&result->planes[0], &result->points[1]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[1]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[1]) < 0.01f))
			error = "_pyramid_bottom_left world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[1], &result->points[3]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[3]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[3]) < 0.01f))
			error = "_pyramid_bottom_right world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[0], &result->points[0]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[0]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[0]) < 0.01f))
			error = "_pyramid_top_left world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[1], &result->points[2]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[2]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[2]) < 0.01f))
			error = "_pyramid_top_right world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[0], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[1], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[4]) < 0.01f))
			error = "_pyramid_tip world-vertex off planes";
		if (g_47ff86 && error)
			g_47ff86 = false;
		valid = true;
	}
	return valid;
}

struct s_clip_1648d0
{
	byte unknown00[0x10];
	real x0, x1, y0, y1;
	real margin;
};

struct s_projection_1648d0
{
	s_bounds3d bounds;
	s_bounds3d unknown18;
	point2f hull[12];
	short hull_count;
};

struct s_plane_block_1648d0
{
	byte unknown00[0xc];
	s_plane *planes;
};

struct s_bsp_1648d0
{
	byte unknown00[0x18];
	s_plane_block_1648d0 *block;
};

/* how far along the ray the plane lies, in units of the direction (0 when they are parallel) */
PRIVATE inline real ray_plane_distance_1648d0(point3f const *origin, vector3f const *direction,
	s_plane const *plane)
{
	real dot = direction->i * plane->normal.i + direction->j * plane->normal.j + direction->k * plane->normal.k;
	real distance;
	if (dot != 0.0f)
	{
		real offset = origin->y * plane->normal.j;
		offset += origin->z * plane->normal.k;
		offset += plane->normal.i * origin->x;
		distance = (offset - plane->d) * (-1.0f / dot);
	}
	else
		distance = 0.0f;
	return distance;
}

bool function_1652e0(s_view *view, real margin, const point3f *points, long point_count, point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1);
void plane_negate_168b40(plane3f const *plane, plane3f *out); /* forced inline in src/unknown_1689b0.cpp */
void vector3f_negate(vector3f const *vector, vector3f *result); /* forced inline in src/unknown_0b7740.cpp */
long function_239cb0(long count, point3f const *points, point2f *hull);

// @retail 0x1648d0
bool function_1648d0(s_frustum_1648d0 const *source, s_view *view,
	s_clip_1648d0 const *clip, s_projection_1648d0 *result, bool make_hull)
{
	bool success = false;
	s_clip_1648d0 const * *clip_reference = &clip;
	s_view * *view_reference = &view;
	long pyramid_edges[8][2] = { {0, 1}, {1, 3}, {3, 2}, {2, 0}, {4, 0}, {4, 1}, {4, 2}, {4, 3} };
	long box_edges[12][2] = { {0, 1}, {1, 3}, {3, 2}, {2, 0}, {4, 5}, {5, 7}, {7, 6}, {6, 4}, {4, 0}, {5, 1}, {6, 2}, {7, 3} };
	point3f points[8];
	long (*edges)[2];
	long edge_count;
	if (source->plane_index != NONE)
	{
		s_plane const *plane = &((s_bsp_1648d0 *)g_4e0348)->block->planes[source->plane_index];
		for (long i = 0; i < 4; i++)
		{
			real distance = ray_plane_distance_1648d0(&source->points[4], &source->directions[i], plane);
			points[i] = source->points[i];
			points[i + 4].x = source->directions[i].i * distance + source->points[4].x;
			points[i + 4].y = source->directions[i].j * distance + source->points[4].y;
			points[i + 4].z = source->directions[i].k * distance + source->points[4].z;
		}
		edge_count = 12;
		edges = box_edges;
	}
	else
	{
		memcpy(points, source->points, 5 * sizeof(point3f));
		edge_count = 8;
		edges = pyramid_edges;
	}
	long point_count = 0;
	real margin = 1.52588e-5f > (*clip_reference)->margin ? 1.52588e-5f : (*clip_reference)->margin;
	s_bounds3d bounds = *g_4687e0;
	point3f segments[24];
	point3f projected[24];
	short projected_count;
	for (long i = 0; i < edge_count; ++i)
	{
		segments[point_count++] = points[edges[i][0]];
		segments[point_count++] = points[edges[i][1]];
	}
	function_1652e0((*view_reference), margin, segments, point_count, projected, &projected_count, &bounds, true, true);
	result->bounds = *g_4687e0;
	result->unknown18 = *g_4687e0;
	result->hull_count = 0;
	if (projected_count >= 4)
	{
		box2f clipped;
		clipped.x0 = bounds.x0 > (*clip_reference)->x0 ? bounds.x0 : (*clip_reference)->x0;
		clipped.x1 = bounds.x1 > (*clip_reference)->x1 ? (*clip_reference)->x1 : bounds.x1;
		clipped.y0 = bounds.y0 > (*clip_reference)->y0 ? bounds.y0 : (*clip_reference)->y0;
		clipped.y1 = bounds.y1 > (*clip_reference)->y1 ? (*clip_reference)->y1 : bounds.y1;
		if (!(clipped.x0 > clipped.x1 || clipped.y0 > clipped.y1))
		{
			*(box2f *)&result->bounds = clipped;
			if (make_hull)
				result->hull_count = (short)function_239cb0(projected_count, projected, result->hull);
			result->bounds.z0 = bounds.z0;
			result->bounds.z1 = bounds.z1;
			success = true;
		}
	}
	return success;
}

// score 5 (was 14; earlier 52, 115): head, both planes, all four unrolled classification rows, remainder classification,
// clip denominator (i,k,j), clip distance i term, intersection rows, and the entire projection loop (including `total`
// in EAX and `add edi, 0x18` after the `total` store) match retail; left are only 5 instructions (1851/1851 B): the clip
// distance y*j operand order (+0x456, 2 insns) and the projection header store order `segment/segment_count/total` (3 insns).
// M3 lane 6 r223 (h2m fast; control 115): plane 0's normal negated k, j, i (d last) 141; the negated forward vector
// computed ahead of plane 0's copy 426 (first +0xab); plane 0's d through vector_products.h's basis_dot_xzy (i, k, j
// with a staged j product), camera vector first 284 (1867 B), with plane 1 too 284 (1867 B), normal first 265 (1867 B),
// with plane 1 too 152 (1851 B, first +0xaf). Re-measured: d as `near_distance + -d` 423.
// MiniMax G (11-19): no new form, no lower score (115), body unchanged. Naming the three edge coefficients in locals read
// before the normal's copy scores 276 (1867 B): the compiler keeps them in registers and the copy's stores move behind the
// products. Swapping the i term's operands (`planes[0].normal.i * view->vector_e4.i`) compiles to exactly the same code
// (115 at 1851 B), so the product's operand order is not what decides the register roles. The residual difference is
// unchanged: retail loads the i coefficient into xmm4 before the copy and reads the copied i back from the stack, so the
// negation's zero sits in xmm4, while we keep the copied i in xmm4 and read the coefficient into xmm3 late; every XMM role
// after that point is swapped with it.
// Earlier m3 lane 16 r130 at the end, m3 lane 6 r195 just above the marker; earlier at 115, was 115, and again 115, was 115.
// Earlier at 115 (was 115; merged from a second score line; m3 lane 16 r130 at the end, m3 lane 6 r195 just above the marker; earlier at 115, was 115, and again 115, was 115): coefficient XMM roles, classification operands and projection staging still differ.
// Earlier status line, merged in r127 (115, was 115; m3 lane 16 r130 at the end, m3 lane 6 r195 just above the marker; earlier at 115, was 115, and again 115, was 115): coefficient XMM roles, classification operands and projection staging still differ.
// MiniMax E (11-20): no new form, no lower score (115): retail copies t for the intersection's x and scales y and z in place, and that form (`t * delta.i + p0->x`, `delta.j * t + p0->y`, `delta.k * t + p0->z`) matches the intersection block exactly here but scores 119, because the first plane's dot then keeps the copied normal's i in xmm4 and reads view->vector_e4.i late where retail loads the coefficient into xmm4 before the copy; a permuter run (34 tries) found nothing below 115.
// MiniMax B (11-18): no new form, no lower score (115): the first plane's dot still keeps the copied normal's i in xmm4 and reads view->vector_e4.i late into xmm3, where retail loads the coefficient into xmm4 before the copy; the six term orders of each plane's d re-measured on this tree (plane 0: 115 with i, k, j and 133-149 otherwise, plane 1: 115 with i, k, j and 129-138 otherwise) all leave the coefficient load after the copy.
// Earlier: coefficient XMM roles, classification operands and projection staging still differ.
// Earlier at 115 (was 115): coefficient XMM roles, classification operands and projection staging still differ.
// Earlier again at 115 (was 115): coefficient XMM roles, classification operands and projection staging still differ.
// Earlier at 115 (was 115): coefficient XMM roles, classification operands and projection staging still differ.
// Earlier at 115 (was 115; a second stacked first line, merged in r108): Frame 0x1ec agrees. The near-plane x coefficient loads late and exchanges the coefficient and zero registers.
// MiniMax S (11-17): no new form, no lower score (115): the first plane's dot still loads view->vector_e4.i after the
// forward copy where retail loads it before; naming the three coefficients ahead of the copy gives 294 (1867 bytes),
// naming only the i term 141, the i term's two operands swapped 115 and the terms in j, k, i order 133 leave the
// coefficient load where it is, and retail's intersection (the x row copying t, the y and z rows scaled in place) is
// reached only by writing the scale helper's body out, which the rules forbid: the x row alone with t first and the
// scale helper kept gives 124 (the first difference moves to +0xb6) and the helper's own rows give 115.
// MiniMax C (11-16): no new form, no lower score (115): retail loads the view's vector_e4.i into xmm4 before the
// forward copy and every term order tried here still leaves that load after the copy, so the register colouring of
// the first plane's dot stays as it is.
// MiniMax S (11-16): no new form, no lower score (115): the first plane's dot still keeps the copied normal's i in xmm4 right after the copy and reads view->vector_e4.i late into xmm3, where retail loads the coefficient into xmm4 before the copy and reads the normal's i back from the stack; the i term's two operands swapped (115), the terms in j, k, i order (133) and in i, j, k order (146), a named distance (119), the normal's components first (115), the forward pointer's components (442) and the j term first (133) all leave the coefficient load after the copy, and a permuter run over the file (30 tries) found nothing below 115.
// MiniMax R (11-15): score 115 (was 115): the first plane's dot still keeps the copied normal's i in xmm4 right after the copy and reads view->vector_e4.i afterwards, where retail loads the coefficient into xmm4 before the copy and reads the normal's i back from the stack, so the zero constant of the negation sits in xmm3 instead of xmm4; the shared dot3f (278), all six term orders of each plane (129-149), the per-term operand swaps (115 each), the plane pointer (285), the normal pointer (150), the named components (307) and a 300 s permuter run (3 tries, best 115) all leave that coefficient load after the copy.
// MiniMax G (11-14): no new form, no lower score (115, was 115): retail loads view->vector_e4.i into xmm4 before the forward copy and keeps the copied normal's i in xmm3, while this build loads the copied normal's i into xmm4 and reads the coefficient late into xmm3, so the zero constant of the negation moves with them; naming the three coefficients before the copy (276), copying the normal component by component (460) and reordering the first plane's d terms (133) all leave the coefficient load after the copy, and the permuter found nothing below 115 in 20 tries.
// MiniMax G (11-12): no new form, no lower score (115): retail loads view->vector_e4.i into xmm4 before the forward copy, ours loads the copied normal's i into xmm4 and reads the coefficient late into xmm3, swapping the XMM3 and XMM4 roles; every term, grouping and operand order of the first plane's d still leaves the coefficient load after the copy.
// M3 lane 1 r96 (fast, the file's matched functions unchanged): the near clamp's comparison flipped (`near_sum < k ? k :
// near_sum`) 115; a `vector3f const *const` forward pointer 115; the plane pointer declared outside the pass loop 115; the
// far normal copied from `*forward` and negated in place 428 (1867 B); the near clamp on the sum written out at both uses
// (no near_sum local) 261 (1867 B). Re-measured: the bare-comparison classification 320 (1835 B).
// MiniMax S (11-09): the plane-0 product with the two operands of the i term written the other way
// round changes no code at all (115), so the coefficient's register is not chosen by the operand
// order of that product.
// MiniMax S (11-09): still differs at the first plane: retail reads all three of view->vector_e4's components
// before the forward vector's copy (e4.i into xmm4), ours reads e4.j and e4.k there and reads e4.i only at its
// own product, which exchanges the xmm3 and xmm4 roles for the rest of the function.
// MiniMax I (11-06): no new form, no lower score; the view vector copied into a local for the first plane's dot product gives 463 and 452 against 115.
// MiniMax A (11-05): no new form, no lower score; the plane-0 term still loads `vector_e4.i` after the
// forward copy and keeps the zero in xmm3 where retail has it in xmm4.
// Round 16: retail loads the three `view->vector_e4` coefficients into xmm0, xmm3 and xmm4
// before it copies the point and then reloads the point's components from that copy, ours reloads
// the copied normal's x into xmm4; retail keeps the near distance in xmm5 and the zero in xmm4,
// ours keeps the near distance in xmm6 and the zero in xmm1.
// Intersection/projected point setters 119/119; existing in-place vector scale 115; long/byte result 216/119.
// Forward value copy 474; direct projected element 246; scaled direct element 247; separate scaled vector 115.
// Second-plane-first 425; reuse first negated normal 463; indexed plane selection 375.
// Frame pointer 574; zero inline depth 609.
// First/second normal memcpy 115/115; negated aggregate 115;
// native endpoint classification 387; default-zero intersection ratio 252.
// Earlier: Coefficient/zero XMM roles, classification products and projection staging remain.
// Existing scale for first/second/source/all normal negations 425/410/433/446f; reused projected
// workspace 249f; per-use outside bytes 249f; completed projection total 214f; native short total 115f.
// Earlier: coefficient/zero XMM roles, classification operands and projection homes remain.
// Native camera vector member 115; camera/bounds references 251/133; clipped/camera/projected point names 115/115/115;
// paired projection names 115; native vector plus three point names 115.
// First/second input-vector value copies 463/474; external vector references 251/279;
// both copies 454; shared scaled difference 390; conditional plane enable 474;
// projection cursor before its guard 115. Arithmetic grouping and count guards retained.
// Earlier: Normal/zero XMM roles, classification and projection homes remain; offset
// accumulator 269; shared segment count 468; count result 459; projected/intersection values 402/323;
// bool/second endpoint values 259/362; completed cursor 196.
// The locals as plane3f 115; plane_distance_to_point for the classification 265, plus the
// intersection distance 269, plus dot3f for the denominator 216; the distance alone 117;
// dot3f for the first plane's d 278 (either argument order), for both planes' d 155.
// Round 199: matching named xz subtotal, y product, x/z products and all products in both plane
// rows 305/267/280/269; with existing component assignment for every normal negation
// 306/301/174/285. All restored; original grouping, scale helper and guards retained; coefficient
// preload and projection homes remain.
// Round 419: complete near plane 496; complete far plane 460; complete near plane/complete far plane 564;
// complete near plane/immutable completed planes 496; complete far plane/immutable completed planes 460;
// complete near plane/complete far plane/immutable completed planes 564; complete near plane/complete far
// plane/parallel full offset producers 567; complete near plane/complete far plane/immutable completed
// planes/parallel full offset producers 567. Coefficient/zero XMM roles, classification operands and
// projection staging remain. Original body restored.
// Round 184: logical second-plane enable 459; implied pass retest removed 370;
// classification/clipping-pair/projection-pair source cursors 327/255/117;
// classification with clipping/projection cursors 193/327; all cursors 195.
// Native byte/Boolean culling-array initialization 489/489 (frame grows sixteen bytes).
// All restored; coefficient preload, classification operands and projection staging remain.
// Round 80: forward declaration first 196; const view 115; first-plane receiver 285,
// with j-k-i offset order 285; inner do/while and condition increment 115/115;
// const x value/reference 141/119. Original body restored.
// Lane 15 r19: retail loads all three vector_e4 components before the forward copy (ours loads e4.i
// after it, which swaps XMM3/XMM4 from there on). Both planes through one forceinline plane helper
// 281 (1867 bytes); plane 0's d as one negated expression 143; near distance after the copy 141;
// forward pointer removed 264 (1867 bytes); clip denominator / distance / both as single
// expressions 247/251/117.
// Round 91: first plane's d in i, j, k order 146, in j, k, i order 133; classification in i, j, k
// order 115, point-first products 115; the intersection scaled through a value-first scale helper
// (i = s * i, j = j * s, k = k * s) 115. Retail's first-plane block loads camera.i early and the
// normal's i late (ours the reverse); its denominator copies delta for i and k and folds the normal.
// Round 243: Existing normal-copy setters and overlap-safe bounds transfer,
// subsets 1..7 460/502/526/495/508/565/569; all with native negated input 569.
// Original transfers and complete body restored; coefficient preload and projection homes remain.
// Round 100 (lane 8, fast): plane 0's d in every term order with camera-first or normal-first products: operand order
// never changes the code; i/j/k 146, i/k/j normal-first 115 (same code), j/i/k 149, j/k/i 133, k/i/j 144, k/j/i 133.
// Round 188 (fast, caller 0x1648d0 matched each time; a one-file LTCG harness gives the same 115):
// plane 0's negation through vector3f_assign 115 (same code); d negated before the normal 143; the
// normal copy through vector3f_assign 460 (1819 bytes); d as `near_distance - d` 423; the top
// locals declared in reverse 115. Harness: all 120 orders of the five top declarations 115 each;
// near_sum/near_distance/forward/negated declared at the top in all 15 subsets, before or after
// culled, 115 each; the camera vector copied to a local before the normal copy 463 (1883 bytes).
// Round 141 (lane 4, fast): the zero constant sits in XMM4 for the whole function in retail and in XMM3 here,
// so the near plane's late e4.i load accounts for most of the setup, clip and t = 0 colour differences.
// The intersection through the existing vector3f_add_scaled 119; normal-first/point-first operands in
// the classification's i, k, j terms (nnp, npn, pnn, ppn) and a delta-first denominator j term 115 each
// (the operand order is canonicalised); the output element stored directly 247 (1867 bytes); p1
// declared before p0 120; the near plane's d grouped as `i + (k + j)` 145; the point memcpy
// moved before the planes (after the memset) 193.
// Round 58 (lane 16, fast): plane 0's d grouped `i + (k + j)` 145, `(k + j) + i` 145 (plane 0's code unchanged,
// later blocks move); the segment flags cleared after the planes 178; plane 0's d as statements j, += k, += i
// 287 (1867 bytes), the same sum in a named local (diagnostic) 251 (1867); plane 0's d as one sum of
// `(real)(a * b)` products 271 (1867).
// Round 124 (one-file LTCG harness, exact at 115; five confirmed with fast): whatever the term order,
// the last term of plane 0's d loads its camera component after the copy (the copied normal's i
// takes xmm4), and the zero that inherits camera i's register then shifts the clip loop's XMM homes.
// Plane 0's d with explicit grouping, every term order and operand order: (a+b)+c 145-179, a+(b+c)
// 145-179 (`e.i*n.i + (e.k*n.k + e.j*n.j)` 145 fast); d accumulated with `+=` in any order 287 (1867
// bytes); a named d 119 (fast); d computed from the forward pointer before or after the copy 441/442;
// the three camera components in named reals, any order 276; the near distance computed after the
// copy 141 (fast), before the bounds copy 191, just before the d negation 179; d negated together
// with the near add 423; the normal negations in the other five orders 126-141 (j, i, k 126 fast);
// a by-name normal copy 460/464; a plane-negate or vector-negate inline helper for both planes 269/247;
// classification in 96 term/operand/grouping forms, none below 115; the intersection written
// directly as `delta.i * t + p0->x` 119 (fast), point-first 119, into a separate scaled vector 115.
// Diagnostic (not keepable): plane 0's j and k negations through a normal pointer 113 (classification
// operands fixed in the unrolled loop, flipped in the remainder loop).
// Y488: plain near/source/far normal-negation loops, seven subsets,
// 259/257/259/259/267/259/113; all with short component indices 488.
// Two-point gain rejected; original body restored. Coefficient/zero XMM roles remain.
// Round 55 (lane 10, fast): retail follows the source's operand order in the remainder classification
// loop's j term and in the clip denominator/distance products, ours swaps some of them. The plane pointer
// const 115 (same code); the classification index declared in the for 115 (same code); the two planes as
// separate locals 242 (1830 bytes); the negated forward vector computed before plane 0's copy 426, right
// after plane 0's copy 133.
// Round 441: indexed near-final/far-input/far-final normal negation loops, subsets 1..7
// 259/257/259/259/267/259/113; all plus two-component screen loop 232. Two-point gain
// rejected; original body restored. Early coefficient preload and projection homes remain.
// Round 37 (lane 15, fast): that pointer used for all three plane-0 negations 251 (1867 bytes); the j/k-only
// diagnostic re-measured 113 (the e4.i load stays after the copy: its gain is in the later classification
// operands); plane 0's d negated before its normal 143; plane 1's negated vector computed before plane 0 426;
// vector_e4 copied into a local vector3f before the normal copy and read from it in d 463 (1883 bytes, the copy
// goes through integer registers to the frame); plane 1's negated vector declared at function scope 115 (same code).
// Diagnostic (initialised dead long temps at the top, k = 1..7): 119 135 130 242 246 135 127, first difference
// always +0xbb (the e4.i load does not follow the temp count).
// Y488: plain near/source/far normal-negation loops, seven subsets,
// 259/257/259/259/267/259/113; all with short component indices 488.
// Two-point gain rejected; original body restored. Coefficient/zero XMM roles remain.
// Round 215: complete classification/denominator/both products 263/247/131;
// complete numerator products 113; classification/numerator 265; denominator/numerator 251;
// all three product boundaries 140; existing shared vector addition after scale 115.
// All restored; early coefficient preload and clipping/projection allocation remain.
// Round 37 (lane 15, fast): that pointer used for all three plane-0 negations 251 (1867 bytes); the j/k-only
// diagnostic re-measured 113 (the e4.i load stays after the copy: its gain is in the later classification
// operands); plane 0's d negated before its normal 143; plane 1's negated vector computed before plane 0 426;
// vector_e4 copied into a local vector3f before the normal copy and read from it in d 463 (1883 bytes, the copy
// goes through integer registers to the frame); plane 1's negated vector declared at function scope 115 (same code).
// Diagnostic (initialised dead long temps at the top, k = 1..7): 119 135 130 242 246 135 127, first difference
// always +0xbb (the e4.i load does not follow the temp count).
// Y488: plain near/source/far normal-negation loops, seven subsets,
// 259/257/259/259/267/259/113; all with short component indices 488.
// Two-point gain rejected; original body restored. Coefficient/zero XMM roles remain.
// Round 215: complete classification/denominator/both products 263/247/131;
// complete numerator products 113; classification/numerator 265; denominator/numerator 251;
// all three product boundaries 140; existing shared vector addition after scale 115.
// All restored; early coefficient preload and clipping/projection allocation remain.
// Round 447: double near-sum/near-distance/screen-factor storage, subsets 1..7: 444/434/443/439/444/429/435; single-use clamp at its offset addition 208. Original domains and body restored; coefficient preload and projection homes remain.
// Round 219: positive visibility on guarded loops, entry-result/projection-input-cursor/shared-count
// masks 0..7: 236/236/132/132/470/470/470/470. Original culling result, independent counts and
// projection indexing restored; coefficient preload, zero roles and projection staging remain.
// Round 341: Native two-coordinate screen value with conditional bounds, intersection
// receiver and logical cull publication, masks 0-7: 368/469/410/378/374/411/395/380.
// Frame shrinks to 0x1e8 versus 0x1ec; original complete body restored.
// Round 449: Conditional projection-extent publications, x/y/z subsets 1..7
// 173/168/191/162/190/185/290; all six selections named symmetrically before publication 287 (fast control 115, full 115).
// Original complete executable source restored; opening structural differences remain.
// Round 451: Complete classification-distance/clipped-distance/negative-reciprocal expression
// nodes, subsets 1..7 and complete nodes with reciprocal first: 249/115/249/115/249/115/249/249. Original expression boundaries
// and complete body restored; coefficient preload, XMM zero and projection homes remain.
// Round 456: Near-normal/far-normal/input-point memmove transfers, subsets 1..7
// 458/469/346/401/422/457/475; all plus complete intersection publication 470.
// Original transfers, arithmetic trees, helpers and full body restored; coefficient preload and projection homes remain.
// Round 72 (lane 16, fast; a compile-only listing reproduces the clip block). Aligned counts matter here: forms that
// raised the positional score moved closer. dot3f denominator on plane3f locals 224 (first plane block then retail's
// order, xmm4/xmm5 swapped); plus intersection `delta.i * t + p0->x` 226, p1 declared before p0 224, total declared
// first 224, out pointer with a named input point 222. Then, on that form: no delta vector, the clip written from
// `(p1->y - p0->y)` expressions with only delta_i named, denominator staged j, k, i: 21, with named scaled_x/y/z rows and
// `out++` at the loop tail 19 (al 22). Same with i, k, j as one expression 31, tail 29; delta_j named, staged i, k, j
// 36 (denominator shape retail's, intersection roles swapped); separate delta_i for the intersection only 53; no named
// part 241/242 (1835 bytes, near/zero swap back); all named 245; each subset of two named 39+. What the 19 form still
// differs in: classification first-row j term, denominator i and j roles, distance j term, total's register at the
// projection entry. Not kept: a part named in only some rows and a one-use input pointer need a byte match. Other
// forms on the dot3f base: dot3f swapped 224; scalars 245 (1830); pointer to the normal 226; near clamp forms 222-260;
// negation orders and pointers 230-254; classification operand orders 222 (same code); 60 random combinations of
// term, helper and negation forms 152-275. Permuter on the 19 form: 19 on the full image too, 73 tries, nothing better.
// Original body restored.
// Round 471: Selected normal/offset/origin observations, masks 1-7,
// 515/386/494/515/584/511/565; complete selected-plane value 490. All restored.
// Round 473: Native short/word/signed-byte/byte ordinals for the fixed two-plane
// traversal, then the fixed output pair: 363/363/241/241/129/129/129/129.
// Original ordinals restored; early coefficient/zero roles and projection staging remain.
// P261: symmetric near/far coefficient captures, all six native component declaration
// orders 269 each; joint row lifetime and immutable inputs 269/269 (1867 bytes).
// All exact-only snapshots restored; coefficient preload, zero roles and projection homes remain.
// P277: Actual margin/clamp/negated-vector/plane-array/denominator/distance/classification
// scratch identities individually and together: 115 each, 1851/1851 bytes. Original names
// and body restored; early coefficient preload, zero XMM and projection homes remain.
// Y547: Complete projected-point memcpy/memmove publication, with native/library
// intersection and initial bounds transfers: 115/419/115/363/115/419/115/475. All restored;
// coefficient preload, zero XMM roles and projection staging remain.
// Lane 17 r36 (fast): bounds copy after the planes 193; `use_plane1 &=` after the planes 209; memset after the
// near clamp 172; near clamp as `near_distance = near_sum; if (1.52588e-5f > near_sum) ...` 120; `use_plane1 &=`
// after the bounds copy 151. Original body restored; the vector_e4.i load still follows the normal copy (XMM3/XMM4 swap).
// Round 33 (m3 lane 18, fast; 0x1648d0 and 0x320b0 matched each time): the clip parameter through
// the file's existing ray_plane_distance_1648d0 232; no forward pointer, the view's vector read at
// each use 264 (1867 bytes); the default bounds copied just before the point memcpy 193 (+0x41);
// use_plane1 masked after the memcpy 209 (+0x6); the pass test as one condition with
// `&planes[pass]` 258. All restored; the early e4.i load and the zero's XMM role remain.
// Round 56 (m3 lane 13, fast): the negated forward vector as an aggregate initializer 115 (same code); plane 1's
// normal negated straight from the forward pointer, no negated vector 465 (1803 bytes); `margin + view->margin`
// 115 (same code); a plane-0 pointer for the whole plane-0 block 285 (1867 bytes); the near distance as an
// if-clamp on the sum 266 (1867 bytes); plane 0's d as `near_distance + -d` 423. All restored.
// AC500: Construction-only scalar scope, per-plane classification array and point-array declaration
// at input-copy boundary, masks 1-7, then complete projected aggregate initialization: 115 each.
// All original lifetimes and complete body restored; coefficient preload, zero XMM and projection homes remain.
// Round 208 (m3 lane 3, fast): the delta vector at function scope / before p0 and p1 115/115; the intersection at
// function scope 115; t at function scope 115; the denominator as dot3f(&plane->normal, &delta) 224 (aligned 58
// against 64); the intersection rows `delta.i * t + p0->x` without the in-place scale 119, with that dot3f 226
// (aligned 51). Lane 16 r72's 19 form re-measured 19 but writes out the existing point/scale helpers.
// Round 48 (m3 lane 2, fast with 0x1648d0/0x320b0): lane 16 r72's 19 form rebuilt 19 (first +0x28a); on it, the
// distance j product's operands swapped 19 (same code); the denominator staged i, k, j with delta_i 33; all three
// deltas named (i, k, j or i, j, k declarations, rows from them) 118/118 (the near/zero swap at +0xb6 returns);
// delta_j named with the denominator staged i, k, j 30 (retail's denominator roles; the deltas are computed z, x, y
// where retail computes x, z, y, and the intersection's copied-t row is y where retail's is x). Body restored.
// AC513: Native near/far coordinate-point field types singly/both, both with native
// source-normal vector, each with native source-normal vector, and both with shared plane
// storage without/with source-normal vector: eight at 115, 1851 bytes. NOT kept:
// no exact match; coefficient preload and zero/classification/projection homes remain.
// Original geometry types, reads, arithmetic and complete body restored.
// Round 147 (m3 lane 6; one-file harness exact at 115, low forms confirmed with fast). The head's first difference is only
// the last term's registers (camera i and normal i in xmm3/xmm4 swapped); the zero inherits the swap. Natural forms (fast):
// `total` declared first 115; that with plane3f locals 115; with a walking `out` and a `source` element pointer 114, on plane3f
// locals 114. Harness: a walking `out` alone 249 (1867 bytes), `for (...; k++, out++)` 249, `cursor` walked in the loop 289,
// `cursor += 2` after the loop 151 (1858). Match-only chain (helpers written out, one delta named; not kept): plane3f locals,
// only delta_i named, denominator staged j, k, i, named scaled rows 25; `total` declared first 22 (fast 22, first +0x28a), `stc`
// 24, `cts`/`cst` 33, `tcs` 153; `out = output` with `out++` 25, `k++, out++` 152, `out = output++` 171, `out = cursor++` 239;
// walking `out` plus a `source` pointer 20; a 1000-try climb over term, operand and projection statement orders 18 (fast 18,
// first +0x39a). Left at 18: the remainder loop's first-row j term, the numerator's k term, the denominator's x/y roles mirrored
// (retail's rows read as x named, its denominator as y named), and `total` in ECX for retail's EAX, which moves its store.
// Grids on it: dead pointer locals 1..8 before the clip 119/135/130/242/246/135/127/135, in the clip 115 up to three; deltas named
// with the denominator staged i, k, j: none 264, i 36, k 217, j 33, ik 205, ij 234, kj 199, ikj 117 (all eight row operand orders
// tie); delta_k named as well 208; delta_i with twelve one-expression groupings 28-38; the name assigned after the denominator 244
// (1835), before t 94; y named with x named late/early 244/36; a struct delta with direct fields 119/119, with the scale helper
// 113, an array 119; rows `delta.i * t + p0->x` on the helpers 119. Permuter skipped: the 19 form already ran (73 tries).
// AC528: Complete cumulative segment rejection/survival in bool/byte/short/long
// 298/384/369/371/404/404/406/405. NOT kept: no exact match or eligible gain; original complete body restored.
// Coefficient preload, zero SIMD roles and clipping/projection homes remain. The native memory-OR result was preserved logically; no retained candidate.
// M3 lane 4 r165 (one-file harness, exact at 18 for lane 6 r147's form, then 500 climbed forms; none below 18; none keepable below a match):
// const references bound to temporaries (the form that matched 0x45560) for the deltas: x 148 (1830 B), y 254 (1830), z 218 (1830), all three
// 118 (1851), x and named y 253 or the reverse 257 (1830); all three named 247 (1830); denominator staged i, k, j (retail's order and
// roles) with named x 34, with x referenced 164 (1830), with x and y named 253 (1830). Classification through a named value, before or after
// the plane's d, 146 (1830), through a const reference 148 (1830). Walking source pointer instead of `&source[k]` 160 (1835) at either place in
// the tail and in five store orders: `total` then takes retail's EAX with `mov ecx, ebx` ahead of its store, but the walker's base loses
// retail's +8 and the loop head its alignment. Dead locals at eight sites (diagnostic, 1 to 7): top and pass loop 150, 166, 45, 18, 22, 38, 30; all
// later sites 146, 146, then 18. Left at 18: remainder row's j operands, denominator x/y roles, numerator k operands, total's register.
// Round 211 (m3 lane 3, fast; a stdcall copy under the compile-only listing tracks the clip block; aligned with registers
// and stack/both masked, control 64/22). Dot helpers grid, 360 compile-only: classification or distance through dot3f or
// plane_distance_to_point only raise it; denominator through dot3f with direct rows is its best, real 226 (51/15). Clip
// grids, about 1600 compile-only: delta_i named, denominator staged j, k, i, `out++` tail 45 (20/15), with the named
// scaled rows 152 (1830 bytes); the same staged i, k, j 61 (32/17), as one expression 53 (25/13); delta_j named, staged
// i, k, j, `t * dx` 56 (28/12), staged k, i, j 57 (29/14), one expression 59 (31/17). So a named delta multiplies as
// load-then-scale and keeps its register, a repeated expression copies first and is overwritten last; retail's denominator
// has y named-like and its intersection has x named-like, which no single naming gives. First-row j term, all still
// `[eax] * [ecx + 4]`: a named point pointer 171 (52/31), point-first terms 171, staged through a sum 179 (50/28), staged on
// the array 334 (54/30), plane_distance_to_point 171, a named sum 58 (30/14). Keepable forms: no delta local 235/239 (54/19,
// 63/18), all three named 246/247 (78/27, 79/29), only the `out++` tail 249 (65/23). Original body restored.
// M3 lane 12 r91 (one-link harness exact at 115; masked aligned 33): retail's registers read as temps numbered e4.j, e4.k, e4.i, n.j,
// n.k, n.i, ours has n.i before n.j; the far plane already loads its first camera term early. e4.i named before the normal copy 141 (masked 46),
// as a const reference 119 (38); the i product named after the copy 159 (57); an e4 pointer 251 (35, 1867 bytes); a normal pointer for d 150 (41);
// `(real)(j + k) + i` 176 (54), `i + (real)(j + k)` 176 (54), `(real)(k + j) + i` 145 (51), all products cast 146 (36), a double subtotal 413 (158, 1835 bytes).
// Round 75 (m3 lane 10, fast with 0x320b0/0x1648d0; control 115, difflib-aligned 64): the head's only cause is the i term
// of plane 0's d: retail loads e4.i with the other two coefficients ahead of the normal copy and reads n.i at the
// product, ours reads n.i early and e4.i at the product; the zero then takes XMM3 for XMM4 (about 35 lines). Both
// planes' d written j, k, i 152 (emitted i, k, j: VC emits these sums last term first), normal-first products 115 (same
// code), `i + (k + j)` without casts 247 (1867 bytes); dot3f for both re-measured 155 either order. Classification
// with the j term point-first 115, all terms point-first 115 (same code: retail's first-row j term loads n.j first).
// M3 lane 4 r168 (one-file harness, exact at 115). New: all 120 declaration orders of the five top-level locals (points,
// outside flags, segment flags, planes, culled) 115 each; both planes' normals copied before plane 0's d 133; the near clamp
// as `near_sum < k ? k : near_sum` 115 (same code), as `near_sum > k ? near_sum : k` 117. Re-measured listed forms: plane
// 0's d as j, k, i 133, normal-first 115, `i + (k + j)` 145, through dot3f either order 278 (1867 bytes); the normal copied
// component by component 460 (1819 bytes, as through vector3f_assign).
// AC552: Existing shared point transform output return/explicit inline/once-observed scale,
// subsets 1/2/4/3/5/6/7, then forced expansion: 115/115/243/115/243/243/243/115.
// NOT kept: no gain; every scale observation also raises the other user 0 to 94. Original helper/body restored.
// AC556: Copied near/far normal components in native scalar consumers, mutable/const
// and confined producer scopes: 295/287/309/295/287/309/309/309. NOT kept:
// all grow sixteen bytes; original normal copies, reads, helpers and complete body restored.
// AC558: Complete native byte enable-input contracts (bool/byte/char pairs except
// original bool/bool): eight at 115, 1851 bytes. NOT kept: no exact match;
// original definition, every prototype, both caller tuples and entire body restored.
// M398: Original delta/transformed-point/intersection zero defaults, masks
// 1/2/3/4/5/6/7, then all at entry: 115/212/212/115/115/212/212/415.
// NOT kept: no coefficient/zero-home gain; original outputs, helpers and body restored.
// M3 lane 16 r97 (fast, 0x1648d0 and 0x320b0 matched each time; body restored): classification stored as the bare
// comparison with no `? 1 : 0` 320 (1835 bytes); the clip segment loop as a plain `for` 129; the projection segment
// loop as a plain `for` 129; plane 0's d read through `planes->normal` 115 (same code); the segment flags cleared
// first, ahead of the plane-1 mask and the bounds copy 158; the clip ratio's arms reversed (`denominator == 0.0f`
// first) 124; the w clamp as `t.z < k ? t.z : k` 118; plane 1's normal copied from plane 0's finished normal with no
// negated vector 463 (1819 bytes). Diagnostic (reverted): plane 0's near add dropped 476 (1819 bytes), so the near
// value's lifetime is not what delays the view vector's i load.
// M3 lane 11 r81 (fast): dead initialised long probes at the top k = 8..20 135 135 135 138 262 262 135 127 115 119 135 130 243 (period 16),
// after the normal copy k = 1..8 119 135 130 242 246 135 127 135; the first difference stays +0xbb (the e4.i load is not a numbering tie).
// Store through `*(outside0 ? p1 : p0)` 368 (1867 bytes); `p1 = p0 + 1` 256 (1867); both segment loops as plain for loops 129;
// intersection rows point-first 115 (same code); total counted per projected point 116.
// Lane 3 r230 (fast, 0x1648d0 matched each time): the clip denominator written normal-first in all three terms 115
// (same code); repeats of lane 16 r97's forms: the clip loop alone as a plain `for` 129, the projection loop alone 135
// (listed at 129), the classification stored as the bare comparison (no `? 1 : 0`) 320 (1835 bytes); the outside flags
// as `bool` with `? true : false` 320 (1835); in the projection tail `culled = false` after the cursor step 115,
// `total += 2` after the per-point loop 214, the per-point counter as `short` 129. The head is still the late camera-i
// load and the XMM3/XMM4 swap after it.
// Round 65 (m3 lane 2, fast; 0x1648d0 and 0x320b0 matched each time; difflib aligned/register-masked in brackets, control
// [73/33]): ours copies `output` (EAX) then `out` (EDX) from the cursor, one `mov` more than retail's `mov edx, edi`, so the
// inner loop's pad is 3 bytes where retail has `jmp` + pad. Re-measured lane 6 r147's natural forms: walking `out` alone 249
// (1867 bytes) [69/31]; with the `source` element pointer 114 [71/32]; walking out with `total` first 249 [67/29]; all three
// 114 [69/30] (the classification's first-row j terms then match; left: e4.i, the denominator and numerator rows, the
// intersection adds, the outer loop's `jmp` pad and `total` in ECX), on plane3f planes 114; the distance terms' operands
// swapped on it 114 (same code). New: both outside flags named (`outside1` for the cull and the crossing test) 362 (1867
// bytes, first +0x15); `if (denominator)` 115 (same code); a const plane pointer 115 (same code); near_sum/near_distance
// `real const` 115 (same code); the projection cursor declared before the segment guard 115 [75/35].
// m3 lane 14 r77: lane 6 r147's match-only recipe rebuilt (fast; not keepable below a byte match, it writes out the point and
// scale helpers and names one delta of three): plane3f locals, `real delta_i` only, the denominator staged j, k, i from
// `(p1->y - p0->y)` expressions, named scaled rows 25; `total` declared first in the projection block 22 (at the top of the
// function 468, 1835 B); walking `out` plus a `source` pointer 21 (first +0x28a; saved as build/r77/1652e0-r77-21.patch in
// worktree m3-lane14); the denominator staged i, k, j instead 37. A compile-only (no /GL) build reproduces that form's
// clip block exactly, so screened there (SSE stream against retail, 25 at the 21 form): every subset of named deltas in
// every staged order (48) and every named-before / expression / named-after-t choice per axis for the denominator and
// the rows in both staged orders (250): none below the 21 form; all 48 ungrouped classification spellings give identical
// code (grouped ones worse), so its first-row j operand order is not set by the source spelling; all 72 orders of the two
// planes' negations (and d first or last) none better. Left at 21: the classification first-row j term (both loops), the
// denominator's computation order (retail x, z, y with y loaded first in its product; a named delta loads first, temps
// copy first, and the same named x must copy t in the rows), the distance's k operands, and total's register. New fast
// checks on the 21 form (all screened first; 144 distance spellings, staged or single, none better either): the
// distance through plane_distance_to_point 153 (1830 B), through dot3f 153 (1830 B); the classification through
// plane_distance_to_point 39; the denominator in retail's x, z, y order with delta_j named and the rows' x named after
// t 32 (masked 11 against the 21 form's 13: retail's denominator shape, the deltas computed z, x, y into XMM2/3/5 for
// retail's x, z, y into XMM5/3/2, and the copied-t row is y for retail's x); the distance staged i, j, k 25.
// M3 lane 16 r102 (fast, the file's matched functions and 0x320b0 unchanged): the two planes as two named plane locals (either
// declaration order) 242 (1830 B); the far plane's `pass == 1 &&` test dropped 370; `segment_culled` zeroed by an `= {0}` initializer
// in place of the memset 489 (1867 B); the far-plane mask as `if (!view->flag84 && !view->flag85) use_plane1 = false;` 474 (1835 B).
// Body unchanged at 115.
// M3 lane 12 r111 (fast, the file's 16 matched functions and 0x320b0 unchanged): plane 0's three negations read from `forward`
// in place of the copied normal 452 (1819 bytes); plane 0's `d += near_distance` after plane 1's block 176; the 0x54 field typed
// `vector3f vector54`, no cast 115 (same code); the 0xb4/0xe4 fields typed point3f and read as .x/.y/.z 115 (same code); `forward` as
// a `vector3f const &` 115 (same code). Left as before: retail allocates camera i to XMM4 and hoists it above the normal copy, ours
// loads the copied normal's i right after its store and takes camera i late into XMM3. Body restored.
// M3 lane 8 r152 (fast; masked aligned 22): retail's `segment_culled` update is the `|=` (`or byte`) and its bound updates and
// projection arithmetic are ours. Compile-only, identical code: `for (long i ...)` scope, the clip test as nested ifs, `bool`
// outside and culled arrays. Real: `outside[segment * 2]` read at each use in place of `outside0` 249 (1867 B); the culled update
// as `|= !(a | b)` 383, as `x = x || (...)` 284; the clip test's `&&` operands in the other order 275; the `w` clamp as an
// if-statement on a copy of t.z 249 (all 1867 B). Body unchanged.
// Round 188 (m3 lane 4, fast with 0x1648d0/0x320b0; aligned/masked 64/23): the `w` clamp as `-1.52588e-5f < t.z ? ...` 115
// (same code); intersection rows `p0->x + delta.i` 115 (same code); the intersection distance as one expression 251 (1867 B),
// staged i, k, j 251; the classification through a three-use `point3f const *point` 265 (1867 B); the clip store's arms
// as `if (outside0) *p1 = ...; else *p0 = ...;` 115 (code differs in 7 places); the projected point as a copy of t scaled
// in place 402 (1875 B). Re-measured: plane 0's d as `near_distance - d` 423.
// m3 lane 3 r233 (fast, 0x1648d0 and 0x320b0 matched each time; control 115, aligned 64, register-masked 23). The head's
// e4.i load still follows the normal copy: ours loads the copy's i into XMM4 straight after its store and e4.i late into
// XMM3, retail the reverse, and the zero inherits the swap. Forms: plane 0's normal negated straight from `forward` (not its
// own copy) 452 (1819 B: the copy folds away); each plane's normal negated first and d taken as e4 (b4) times the negated
// normal with no separate d negation (the same values) 479 (1803 B); that for plane 0 only 276, for plane 1 only 258 (first
// difference +0x60 each); with d through dot3f 479 (1803 B). Body unchanged.
// M3 lane 5 r177 (fast, 0x1648d0 and 0x320b0 matched each time): the far-plane mask as `use_plane1 = use_plane1 &&
// (flag84 || flag85)` 392 (first +0x6); `culled = false` after the per-point loop 238; the clip denominator and distance
// through a named `vector3f const *normal` 247 (1867 B); the far plane's negated vector as an aggregate initializer 115
// (same code); plane 0's near distance subtracted before the d negation (`d -= near_distance`, then `d = -d`) 151 (first
// +0x60). Re-measured: `out = &cursor[k]` with the cursor stepped after the per-point loop 196.
// M3 lane 9 r118: no trial run. The first difference (+0xbb) is still plane 0's camera-i load after the normal copy
// (retail loads it with camera j and k ahead of the copy). Every lead in this note was checked: the i term's spellings
// (named, referenced, pointer, cast groupings, j/k/i order, dot3f either order, negation orders), lane 6 r147's
// match-only recipe (18 after a 1000-try climb, 21 on lane 14 r77's rebuild, 1600 compile-only clip forms by lane 3 r211
// and 250+48 by lane 14 r77) and the tail's walker/total forms. Lane 14 r77's 21 form (its saved patch) rebuilt 21 (first
// +0x28a; the head matches there): the classification j term in both loops, the clip deltas' XMM2/XMM5 roles and products,
// and `total` in ECX (retail EAX, its store late). On it: `culled = false` after the cursor and source steps 21 (only that
// store moves; total keeps ECX). No other code-changing form outside the lists above.
// M3 lane 15 r83 (fast, 0x1648d0 and 0x320b0 matched each time): `__assume(view != NULL)` at the top 145 (the e4.i load stays
// after the copy; the classification's k and j terms swap operands in every row); `__assume(forward != NULL)` 145;
// `__assume(plane != NULL)` in the pass loop 145; `__assume(point_count <= 32)` before the point memcpy 263 (1867 B);
// `__assume(total >= 0)` before the return 247 (1867 B).
// M3 lane 12 r123 (one-file LTCG harness exact at 21 on lane 14 r77's match-only 21 form, then --fast): on that form,
// one plane's j and k negations through a `vector3f *` to its normal (either plane) 19 (the unrolled classification loop
// becomes retail's; j alone 35, i 21, i+j or i+k 33, all three 23, both planes 35-45); on it, all 48 classification
// spellings 19 (same code), grouped 39-47; every named-delta subset with every denominator order 19 at best (the
// existing delta_i, j, k, i), 23-38 otherwise; `out = cursor` ahead of `culled = false` 18 (fast 18, first +0x384;
// build/r123/1652e0-r123-18.patch in worktree m3-lane12). Left at 18: the remainder classification loop's i/k order
// (retail i, k, j), the clip deltas' order (retail x, z, y), `total` in ECX (retail EAX). On this body: plane 1's
// j and k negations through a normal pointer 113 (first +0xbb).
// M3 lane 16 r119 (fast, 0x1648d0 and 0x320b0 matched each time; control 115): the clip ratio as `real t = 0.0f;` set
// only in the nonzero-denominator arm 252 (first +0xb6); plane 1's normal negated straight into its components from
// `forward` (no `negated` vector) 465 (1803 B); plane 0's three negations through a `vector3f *` to its normal 251 (1867
// B), its j and k only 113, both planes' j and k 245. Same code: the projection's `output` taken before `culled = false`,
// the classification as `-0.0001f < sum`, the cull update as `|= !(outside0 || outside1)`. Body unchanged.
// Round 247 (m3 lane 3, fast, 0x1648d0 and 0x320b0 matched each time): the intersection rows through the existing
// vector3f_add (delta, then the point as a vector) 115 (same code, not kept); `out = output + k` 115; `p0`/`p1` as
// `local_points + segment * 2` (+ 1) 115; the projection factor as `-(1.0f / w)` 258; the scale folded into the rows
// (`t * delta.i + p0->x`, no vector3f_scale) 119. Body unchanged.
// M3 lane 8 r172 (fast with the file's 15 matched functions, all kept): as a member of the view (`this` = view, both
// callers through the view pointer; 0x320b0 stays matched) 251 (1867 B); plane 0's coefficients through a named
// `vector3f const *` to vector_e4 251 (1867 B); the classification's j term spelled point-first 115 (same code:
// canonicalised); plane 0's normal copied with memcpy 115 (same code); the far plane's negated vector computed ahead of
// plane 0's copy 426. Body unchanged.
// b9D (11-17) walk against retail, fast build on r839: no code change, score 115 (fast, was 115); callers 0x1648d0
// and 0x320b0 stay matched. Walked statement by statement against h2.py disasm and align.py: the argument slots
// ([esp+0x200..0x220] after the four pushes), the 0x84/0x85 plane-1 mask, the 0xb0 margin, the near clamp (comiss,
// ja), both planes' sums and negations (plane 0 at 0x28 with d at 0x34, plane 1 at 0x38 with d at 0x44), the
// memcpy size, the pass loop, the classification (> -0.0001, comiss + jbe, NaN gives 0), the clip test (ucomiss !=
// 0, NaN computes t), t = (distance - d) * (-1.0 / denominator), the intersection stores, the |= on
// segment_culled, the transform's scale test against 1.0, the -1.52588e-5 w clamp, the six bounds tests (jbe, x0
// x1 y0 y1 z0 z1), the tail, and the constants at 0x45e068, 0x45ddfc, 0x45dbcc, 0x45dbc0, 0x45de30: no field,
// offset, branch-sense, evaluation-order or behaviour difference found. What is left is register/scheduling only:
// retail loads e4.i at 0x16539b before the copy at 0x1653a8; the first-row and remainder j terms load n.j first
// (0x165567, 0x165677); the denominator copies dx and dz (0x16570a, 0x165711) and uses dy in place; the
// intersection copies t for x and scales y, z in place (0x165773-0x16578b); set-up order at 0x165840-0x165854.
// m3 lane 16 r130 (fast with 0x1648d0 and 0x320b0, both MATCH; control 115): the classification loop under a redundant
// `if (0 < point_count)` 271 (1867 B); plane 0's normal bound as `vector3f &` and copied, dotted and negated through it
// 290 (1867 B); the view's near coefficients bound as `vector3f const &` for plane 0's d 251 (1867 B); plane 0's normal
// copied through a trivial inline setter 127 (head unchanged, the e4.i load still after the copy); the forward vector
// bound as `vector3f const &` in place of the pointer 115 (same code).
// M3 lane 4 r207 (fast with 0x1648d0 and 0x320b0, matched each time): plane 0's d through a `real &` bound to the
// plane's d (assigned, negated and offset through it) 270 (1867 B); plane 0's normal through a `vector3f &` for the dot and
// the negations 272 (1830 B); the whole plane through an `s_plane &` 285 (1867 B); the clip delta as the aggregate
// initializer `{ p1->x - p0->x, ... }` 115 (same code); that with the scale folded into the intersection rows (`delta.i * t
// + p0->x`, no vector3f_scale) 119.
// Round 562: shared xzy dot for near plane 284; shared xzy dot for far plane 262; shared xzy dot for both
// planes 284; shared xzy dot for denominator 224; shared xzy dot for near plane and denominator 221;
// shared xzy dot for far plane and denominator 246; shared xzy dot for both planes and denominator 263;
// both plane dots with normal as first input 152.
// NOT kept: The shared dot can advance the x load, but changes its register and the surrounding
// coefficient schedule. All eight forms raise the score.
// M3 lane 7 r177 (fast with 0x1648d0/0x320b0, both MATCH; control 115, aligned 73): the projection's output pointer
// advanced at the inner loop's tail 249 (1867 B); `total` declared ahead of segment/segment_count 115 (aligned 71). Lane
// 16 r72's 19 form rebuilt on current main 19 (aligned 22; match-only); on it the classification and distance j terms
// point-first 19 (same code), the denominator staged i, k, j 33, `total += 2` ahead of `culled = false` 19 (same code).
// Common plane loop for normals and offsets 389, normals only 397, offsets only 303;
// nested component loop for normals 455, pointer walk 472, vector setter 418, near
// adjustment before common normal loop 396, nested loops with offsets 436. The x
// coefficient/zero roles remain. NOT kept: common orientation disturbs the earlier
// bound and margin setup, even in the four variants at retail length.
// M3 lane 11 r117 (fast, 0x1652e0 alone; control 115, aligned 73, register-masked 68). Reading: in the first plane's d
// only the i term differs: retail gives the coefficient (view->vector_e4.i) the fresh register XMM4, so its load rises
// above the forward copy with the j and k coefficients, and gives the copied normal's i the k coefficient's freed XMM3,
// so that load waits for the k product; ours colours the two the other way round, and the zero of the negations then
// takes the i product's register in each build (XMM4 retail, XMM3 ours). The second plane already has retail's shape in
// both builds (its i coefficient early in XMM5). Single changes: the near clamp as an if-assignment 249 (1867 B); plane
// 0's negations d first 143 (a listed count); the negated forward vector's components in k, j, i order 121; plane 1's
// negations d first 159; the point memcpy between the two planes' blocks 182; the bounds copy ahead of the use_plane1
// update 151; plane 0's d as `near_distance - d` 423 (a listed count). Compile-only screen (it reproduces this
// function's XMM stream; scored on the first 60 XMM instructions against retail, this body 36): 160 random combinations
// of the three top statements' order, the clamp form, forward declared before or after the clamp, plane 0's four
// negation orders, the near add as a statement or in the negation, the negated vector's component order, plane 1's
// negation order and the memcpy's place: none loads the i coefficient before the copy and none is under 38. Diagnostic
// (reverted): the planes at elements 1 and 2 of a three-element array 402 (1830 B) with the first plane's i term
// unchanged, so the role swap does not come from the normal's i sitting at the start of its local.
// Shared x-z-y dot at both plane offsets or near only 284/284,
// denominator only 224, all three 263, copied normal first at both
// offsets 152. Offset before normal copy 444, with shared dot 446,
// separate classification stores 747. NOT kept: copied-normal-first
// retains the extent but exchanges the first coefficient registers.
// M3 lane 5 r203 (fast with 0x1648d0 and 0x320b0, matched throughout; control 115): lane 12 r123's match-only 18 form
// (build/r123/1652e0-r123-18.patch in worktree m3-lane12) rebuilt on current main 18. Diagnostic on it, 0-8 unused
// initialised locals after `culled`: 18/34/41/14/18/162/154/14/18, so the rest follows the symbol count; at 3 (or 7)
// only 14 are left: the clip deltas are computed y, z, x (retail x, z, y, with the same register for each component)
// and `total` takes ECX (retail EAX). On the 3-local form: the four projection-block statements (culled, out, total,
// cursor) in nine orders 14-15 (`cursor += 2` ahead of `total += 2` gives `total` EAX, the stores then out of order);
// the denominator in retail's x, z, y order with no, one, two or three named deltas over 0-7 locals: best 27 (y
// named), 28 (x named), none named 240-270 (first +0x60), two named 62-273, three named 239-261; each product's
// operands either way round (24 forms) no change; the denominator as one expression in all six orders 27 (one canonical
// order). Nothing kept (the 18 form is match-only and the 14 needs unused locals).
// Round 264 (m3 lane 3, fast with 0x1648d0 and 0x320b0, both matched each time; control 115, aligned 73, masked 30): plane
// 0's d accumulated in the plane (`d = j term; d += k term; d += i term`) 287 (1867 bytes), the same with each term
// normal-first 287, accumulated k, j, i 287; accumulated j, k, i in a local `distance` then stored 251 (1867 bytes, aligned
// 69, masked 29: the head unchanged, the tail's projection rows differ); the j and k terms named as `distance` and the i
// term added at the store 172 (first +0xaf). The camera i coefficient still loads after the normal copy. Body unchanged.
// M3 lane 6 r195 (fast with 0x1648d0 and 0x320b0, both MATCH; control 115; the second stacked score line above merged into
// this note): the projected point stored as `output[k] = s` with no `out` pointer 247 (1867 B); the cursor indexed in the
// per-point loop and stepped after it (no `output` local) 196 (first +0x2); the cull update as a plain `if` setting the
// flag 274 (1867 B); the intersection as an aggregate initializer 115 (same code); the second end pointer as `p0 + 1`
// 256 (1867 B).
// Shared staged dot at far 262; near and intersection 221; far and intersection 246; classification 143.
// Normal first at near/far/intersection 265/252/224; near and intersection 221; far and
// intersection 222; all three 286. Point first for classification 263. NOT kept: all regress.
// Near-plane x load and XMM coefficient/zero roles remain.
// M3 lane 2 r108 (fast with 0x1648d0/0x320b0, both MATCH; control 115, 73 aligned lines): the view's +0x54 field typed
// as a vector3f (no cast at the forward pointer) 115 (same code); the first plane's normal copied by memcpy 115 (same
// code); the camera vector through a const pointer for plane 0's d 251 (1867 B); plane 0's d grouped `i + (k + j)` 145,
// `i + (j + k)` 176 (first +0xaf): VC keeps source grouping, so only the flat sum gives retail's (j + k) + i adds.
// M3 lane 12 r147 (fast with 0x1648d0/0x320b0, both MATCH each time; control 115), existing exported helpers not tried
// before: the far plane's negated vector through vector3f_negate (src/unknown_0b7740.cpp) 257 (1830 B); both planes'
// normals negated in place through it 129; the intersection through point_along_vector_221e30 (src/unknown_221da0.cpp)
// with the delta left unscaled 119 (first +0xb6); that with plane3f planes and `total` declared first 121; both planes
// built through the plane-through-point helper of src/unknown_17d9a0.cpp 310 (and 0x17d9a0 itself stops matching in
// the fast image, 256). The point memcpy after the segment clear re-measured 193 (a listed form).
// M3 lane 3 r270 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115, exact-aligned 73; lane 12 r123's 18
// form rebuilt 18, exact-aligned 22). On this body: one to seven unused initialised locals after `culled` 119, 135, 130,
// 242 (1830 B), 246 (1830 B), 135, 127 (first difference +0xbb at every count: the head is not a numbering tie, as lane 11
// r81 found at two other sites); the intersection through the existing vector3f_add_scaled (the point as the base, the
// unscaled delta and the ratio) 119 (first +0xb6); `delta` declared at function scope 115, the intersection point and
// the ratio declared at function scope 115 (same code each); the bounds copy and the segment clear moved below the near
// clamp 191 (first +0x41); plane 0's near add moved below plane 1's set-up 176 (first +0x60). Reading of the head: both
// builds sum the first plane's d as j, k, then i; retail evaluates the i term coefficient first (a fresh XMM4, its load
// then hoisted above the copy) and the copied normal's i second (the k coefficient's freed XMM3), ours the copied
// normal's i first; the k term is evaluated normal first and the j term coefficient first in both builds. In the clip
// rows retail multiplies the y and z deltas by the ratio in place and copies the ratio only for x; ours copies it for
// x and y, the scale helper's operand order.
// m3 lane 8 r198 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115, difflib-aligned 73). Lane 12 r123's
// match-only 18 form rebuilt 18 (aligned 22) and, with three unused locals after `culled` (diagnostic), 14 (aligned 18),
// as lane 5 r203 found. Reading of the 14 that are left: retail computes the clip deltas x, z, y into XMM5, XMM3, XMM2,
// multiplies the x and z deltas as a copy of the delta times the normal in memory and the y delta as the normal loaded
// times the delta, and carries `total` through EAX with its store after the source pointer's move; the 14 form computes
// y, z, x into the same three registers, with the x product normal-first and the y product delta-first, and carries
// `total` through ECX. In this build a named delta always comes out normal-first, takes XMM5 and has its scaled row
// built on a copy of t; an unnamed delta comes out delta-first with its row scaled in place: retail's x has the
// register and row of a named delta and the product of an unnamed one, its y the reverse. New on the 14 form (three
// unused locals kept for the comparison): the denominator x, z, y with only the x delta named 28; with all three named
// in x, z, y order 253 (1830 B); with none named 240 (1835 B); with only the y delta named 27 (the product forms are
// then retail's but y takes XMM5 and x XMM3), also 27 with that delta declared at its term, and 29/45/39 with zero, one
// or two unused locals; the x delta named for its row with the denominator's x term written from the points 50 (54
// without the unused locals), the same with the y delta named for the denominator only 50, declared at its term 50; the
// x delta declared after the denominator 92. The named delta renamed (three names for x, three for y) 14/27, the same
// code each: local names do not move the order. Nothing kept; body unchanged.
// Separate source and oriented planes: near/far/both aggregate stores 491/361/508,
// orient in place then copy 530, near/far/both field stores 394/135/395,
// immutable source and oriented aggregates 571. NOT kept: the far field stores keep
// the late coefficient load, and the other forms move the frame away from retail.
// M3 lane 8 r201 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115, difflib-aligned 73). Lane 12 r123's
// match-only 18 form rebuilt 18 (aligned 25): three sites are left and each is the same shape with one choice swapped.
// The classification remainder loop sums (z*n.k + x*n.i) + n.j*y where retail has (x*n.i + z*n.k) + n.j*y. The clip
// deltas: both builds compute the first two denominator terms as a copy of the delta times the normal in memory and the
// third as the normal loaded times the delta, give the x delta XMM5 and build its row on a copy of the ratio, so the
// product form follows the term's position and not whether the delta is named; retail's terms run x, z, y, the 18 form's
// y, z, x. The projection set-up loads `total` into ECX where retail uses EAX, which moves its store ahead of the
// `culled` clear. On the 18 form: the denominator terms x, z, y (x still the named delta) 32 (the named delta's
// product then loads the normal first, and the z delta and the denominator swap XMM1/XMM3); the five set-up statements
// (culled, source pointer, out, total, cursor) in the orders c-s-o-t-u 18, s-c-o-t-u 18 (same code as the 18 form),
// c-s-t-o-u 19, s-o-c-u-t 18 (aligned 28), s-c-t-o-u 19: `total` stays in ECX in each. Nothing kept; body unchanged.
// Near camera component array 115, near normal component array 115, both near component arrays 115, both far
// component arrays 115, all near and far component arrays 115, indexed near dot 173, indexed far dot 171, both
// indexed dots 326. NOT kept: The five component-array forms tie. Indexed accumulation raises the score and moves
// the first difference earlier, and indexing both dots adds sixteen bytes.
// M3 lane 13 r127 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115). Lane 12 r123's 18 patch rebuilt 18,
// with three unused locals (diagnostic) 14. A fast grid over main's body, 96 forms: {clip block as main, as `delta.i * t +
// p0->x` rows, as the lead's delta_i/staged-j,k,i/named rows} x {main's projection tail, the lead's walking `out` with a
// `source` element pointer and `total` declared first} x {plane 1's j and k negations through a normal pointer or not} x
// 0-7 unused locals. Keepable corners: the walking `out` with the source pointer alone 113 (two lower, under the bar), with
// the far plane's j/k through the pointer 111 (a mixed pointer/array spelling: match-only); the scaled rows alone 119;
// with the lead's clip block 25, 20 (tail), 23 (pointer), 18 (both), 14 at three or seven unused locals. Unused locals on
// main's body 119/135/130/242/246/135/127 (no phase helps). On plane3f planes with the walking tail (113): plane d through
// dot3f 153, the classification through plane_distance_to_point 265, the denominator through dot3f 224, the numerator
// through plane_distance_to_point 115, and every combination of the four 155-309, with 0-15 unused locals none below 113.
// Compile-only screen of the clip block on the lead (XMM stream from the denominator on; the lead scores 16, 12 at three
// unused locals): 2048 forms (named deltas in all eight subsets x the eight t-first/delta-first row sides x named or
// unnamed scaled rows x 0-15 unused locals) best 16, none below the lead. The projection entry (retail carries `total` in
// EAX and stores it after `mov ecx, ebx`, ours ECX before it): all 60 orders of the five statements there (culled, out,
// total, cursor, source) and 396 spellings (three declaration orders, the cursor declared inside or outside the guard,
// `!flag` / `== 0` / named flag, `total = total + 2`) at 0 and 3 unused locals compile to one of four shapes, none
// retail's. Body unchanged.
// MiniMax B (11-20): no new form, no lower score (115): retail keeps the clip deltas in the order x, z, y and ours
// keeps them x, y, z, so the three product rows and the plane normal loads still differ; the form that reorders them
// needs the delta helper written out by hand, which is only allowed on an exact match.
// M3 lane 11 r136 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115; the two stacked score lines merged
// into the first line above): one `segment_count` for the clip and projection loops 468 (1855 B, first +0x6); `total` typed
// short 115 (same code); the far plane's normal set through vector3f_assign from the negated forward components (no
// `negated` local) 504 (1766 B); the `negated` vector in a block of its own 115 (same code); the outside flags typed char
// 115 (same code); re-measured: the clip store through `*(outside0 ? p1 : p0)` 368 (1867 B). Diagnostic: lane 12 r123's
// match-only 18 form rebuilt 18; one to seven unused uninitialised locals on it 18 each (only initialised ones shift it,
// as lane 5 r203 found). Body unchanged.
// M3 lane 2 r122 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115; body unchanged). Lane 12 r123's match-only 18
// form rebuilt 18. New on it: retail's `add eax, esi` in the projection set-up (the per-point counter's 2 added to the total) is
// the total counted once per projected point, so the tail needs no cursor, output or source pointer: `if (projected_points)
// projected_points[total] = s;` with `total++` at the end of the per-point loop and the source as `&local_points[segment * 2 +
// k]`. With 0-7 unused initialised locals after `culled` (diagnostic) that tail gives 142 (1830 B), 158 (1830 B), 37, 12, 16, 32,
// 22, 10: at 7 the whole projection tail is retail's (total in EAX, its store after the source pointer's move) and 10 lines are
// left, the remainder classification loop's j term (point first for retail's normal first) and the clip deltas' order (y, z, x
// for x, z, y). The same tail with the source pointer kept 142 (1830 B), 30, 37, 12, 16, 32, 24, 140 (1830 B); `total++` per point
// with the cursor and output pointers kept 19, 35, 42, 15, 19, 163, 155, 15. On main's body the index tail is 232 (first +0xbb),
// so it is match-only with the lead (build/r122/lead-0x1652e0-index-tail.patch in the m3-lane2 worktree). On the lead with the
// index tail, the clip block from the existing vector3d_from_points3d with the scale helper, with written rows, or with a
// one-expression denominator 103-275 at every count (first +0xbb or +0xb6: the head needs a named scalar delta). Compile-only
// screen of the clip block on that form (it reproduces the fast build from the classification on; the 10-line state reads 14
// there), every count 0-7: 192 forms (each subset of the three deltas named, every declaration order, the six denominator
// orders, named or written scaled rows) best 14 (the lead's own block), next 16; 468 more forms with each delta named for the
// denominator only, for its row only, or declared after the ratio (denominator x, z, y or y, z, x) best 16. With x named and the
// denominator x, z, y the x product loads the normal first and the z delta and the denominator swap XMM1/XMM3; with y named
// the products are retail's but y takes XMM5; with none named the zero constant takes XMM5 and the head is lost.
// M3 lane 2 r124 (fast with 0x1648d0/0x320b0, both matched each run; control 115): loop shapes on this body: the
// classification loop bottom-tested under a count test (`if (point_count > 0) do { ...; i++; } while (i < point_count);`)
// 662 (1563 B: no longer unrolled); the pass loop bottom-tested (`do { ... } while (++pass < 2);`) 133; the per-point
// loop bottom-tested 115 (same code); the clip loop alone as a plain for loop 129; the projection loop alone as a plain
// for loop 129.
// M3 lane 4 r239 (fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115): the camera point copied into a local
// vector for plane 0's d 463 (1883 B); plane 0's i negation read from the forward pointer 423 (first +0x60); plane 0's d in a
// local stored as `near_distance - d0` 423, as `-d0 + near_distance` 423; the forward vector copied into a local normal with
// the plane's normal written from its negation 263 (1830 B); the i product named ahead of the k and j terms and added last 251
// (1867 B); the k and j terms assigned and the i product added by `+=` 168 (first +0x60). Body unchanged.
// Direct clipped endpoint through point setter, component writes or vector sum 394/377/377.
// Endpoint chosen before fraction, with setter, component writes or sum 382/366/366.
// Branch-selected endpoint with point setter or vector sum 371/377.
// NOT kept: direct output advances the first difference to +0x15 and loses retail length.
// Carry completed near/far/both offsets through normal orientation 119/119/135.
// With both named offsets, sign near/far/both before orientation 163/183/191; read both x coefficients before the copies 235,
// or combine the early reads with both early signs 297. The coefficient preload and zero register still differ.
// M3 lane 14 r140 (h2m fast with 0x1648d0/0x320b0, both MATCH; control 115, difflib-aligned 73, register-masked 33):
// retail keeps the source operand order in the clip denominator (delta first for i and k, normal first for j) and in the
// distance (normal, point, normal first) where ours loads the other operand first for two terms of each. Forms: the
// classification terms written i, j, k 115 (same code); the classification through a `point3f const *` element pointer
// 265 (1867 B); the denominator as one expression 247 (1867 B); the distance as one expression 251 (1867 B); the plane
// pointer typed const 115 (same code). Body unchanged.
// M3 lane 8 r220 (h2m fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115): the far-plane mask as `if
// (!view->flag84 && !view->flag85) use_plane1 = false;` 474 (1835 B, first +0x26); the classification store without its
// `? 1 : 0` 320 (1835 B); the outside flags typed bool with the same store and a bool `outside0` 115 (same code); the
// remainder loop's j term written point first 115 (same code) and the clip distance's first two products written with
// their operands swapped 115 (same code): the emitted load order of a product does not follow its written operand order
// here (retail loads in written order, ours the other operand first for the j term and the first two distance terms).
// Body unchanged.
// M3 lane 12 r174 (h2m fast with 0x1648d0 and 0x320b0, both MATCH; control 115). At +0xbb retail loads e4.i ahead of
// the forward copy into plane 0's normal (e4.i is then the product's destination and the zero takes XMM4); ours loads the
// copied n.i first. Plane 0's d from `forward->` after the copy 442 (1835 B, first +0x60), from `forward` ahead of the copy
// 441; the products written normal first 115 (same code); through dot3f(&e4, &normal) 278 (1867 B); redundant parentheses
// around the i, k, j, i+k or all three products 163/176/145/281/271. Body unchanged.
// M3 lane 8 r221 (h2m fast with 0x1648d0 and 0x320b0, both MATCH each time; control 115): plane 0's d read through a
// `vector3f const &` bound to the view's vector 251 (1867 B; the reference is not folded); plane 0 written through an
// `s_plane &` bound to planes[0] 285 (1867 B, first +0xaf); a redundant `if (0 < point_count)` ahead of the
// classification loop 271 (1867 B); plane 1's negated vector through vector_products.h's vector3f_negate_ab 257 (1830 B);
// both planes' in-place normal negations through vector3f_negate_ab 129 (1851 B). Body unchanged.
// M3 lane 1 r154 (H2M_FAST with 0x1648d0 and 0x320b0, both MATCH each run; control 115): lane 12 r123's match-only 18
// form rebuilt 18 (first +0x384). On main: plane 1's d read from the `negated` local instead of the copied normal 440
// (1835 B); the forward vector as a local copy (`vector3f forward = *(vector3f const *)&view->point54;`) 367 (1862 B,
// first +0x2); the clip ratio as one conditional expression with the distance summed in it 253 (1867 B). On the 18
// form: the classification through a `point3f const *` element pointer 32 (first +0x28a); plane 1's d from `negated`
// 448 (1814 B). Body unchanged.
// M3 lane 15 r134 (h2m fast with 0x1648d0 and 0x320b0, both MATCH; control 115): redundant paren pairs (diagnostic): around
// the near sum 1/2/3 pairs 145/247 (1867 B)/271 (1867 B); around the far-plane flag test 115/115/115 (same code); one pair
// around plane 0's d i, k or j product 163/176/145. Body unchanged.
// M3 lane 13 r147 (h2m fast with 0x1648d0 and 0x320b0, both MATCH each run; control 115, difflib-aligned 31 groups): the
// clip loop's pair base named once (`long first = segment * 2;`, five uses) 249 (1867 B, aligned 29: the distance's z
// term then loads the point first as retail, the intersection adds move, and the projection loop gains retail's `jmp`
// over a pad, so the body runs 16 B long); the always-true `pass == 1 &&` dropped from the far-plane skip 370; the plane
// picked as `&planes[pass]` after `if (!(pass ? use_plane1 : use_plane0)) continue;` 376; the named base with each of
// those two 386 / 392 (1835 B). Body unchanged.
// M3 lane 16 r178 (H2M_FAST with 0x1648d0 and 0x320b0, both MATCH; control 115): the near sum and distance after plane 0's
// copy 141 (re-measured, first +0x5d); the segment flags cleared after the near distance 172 (first +0x4a); the near
// distance's compare as `near_sum < 1.52588e-5f` 115 (same code); the forward pointer declared with the locals at the top
// 196 (first +0x6); the planes array declared ahead of the point arrays 115 (same code). Body unchanged.
// M3 lane 2 r142 (H2M_FAST with 0x1648d0 and 0x320b0, both MATCH each run; control 115, difflib-aligned 64, register-masked
// 22). At +0xbb retail loads all three view coefficients (j, k, i) ahead of the forward copy and reads the copied normal
// back j, k, i, so the i coefficient is the product's destination in XMM4 and the copied i sits in XMM3; ours reads the
// copied i first (it is stored first) and the i coefficient last. Plane 0 only: the distance negated and offset in one
// statement (`near_distance - d`) 423 (first +0x60); the dot kept in a local and stored negated after the normal's
// negations 119; the normal negated from `forward` again (the copy read by the dot only) 452 (1819 B); the distance
// negated ahead of the normal's negations 143 (first +0xae); the dot's terms written j, k, i 133 (first +0xaf); the i
// product added to a parenthesised k + j 145. Body unchanged.
// M3 lane 8 r231 (h2m fast with 0x1648d0 and 0x320b0, both MATCH each run; control 115): the segment flags typed bool 115
// (same code); the planes typed plane3f and read through its scalar members (`planes[0].i`, `plane->j`) 115 (same code);
// the classification index typed short 648 (1579 B: the loop is no longer unrolled); the pass index typed short 363; the
// `negated` vector declared with the top locals 115 (same code). Body unchanged.
// Near normal memcpy 115; reflected normal memcpy 115; named near/far plane references 306;
// indexed plane-enable array 482; guarded output cursor 254; success from projected count 459;
// count each projected vertex 116; retained cull byte 384. NOT kept: none restores the near x preload
// or the subsequent XMM roles.
// M3 lane 11 r163 (H2M_FAST with 0x1648d0 and 0x320b0, both MATCH each run; control 115, difflib-aligned 73; body
// unchanged). Reading of the head: it is one colouring decision made in the clip block. The allocator ranks the zero of
// the negations above each of the three clip deltas (214 against 196), so the zero is coloured first and takes XMM3;
// retail colours the y and z deltas first (XMM2, XMM3), then the zero (XMM4), then the x delta (XMM5), and with the zero
// in XMM4 the copied normal's i falls to XMM3 and the view coefficient's load, now in a fresh register, is scheduled
// above the copy. The y and z deltas outrank the zero only when their rows are scaled in place, which needs the ratio
// ranked between the deltas as a product operand: the compiler orders a product's operands by the order in which their
// symbols are first referenced, and retail's rows need x delta, ratio, then the z and y deltas. Forms: the denominator
// accumulated in the ratio variable (`real t = delta.i * n.i; t += ...; if (t != 0.0f) ... / t`) 200 (first +0x60,
// aligned 65: all three rows in place, the coefficient preload as retail, zero and near distance swapped XMM4/XMM5);
// diagnostic, the first product named and the ratio accumulating the other two (`real denominator = delta.i * n.i; real
// t = denominator + delta.k * n.k; t += n.j * delta.j;`) 191 (first +0x22f, aligned 36: the whole head, the delta
// registers and the three rows are retail's; left are the plane-normal operand orders and the projection tail), with
// the walking `out`, a `source` element pointer and `total` declared first 193 (aligned 32;
// build/r163/lead-0x1652e0-ratio-partial-sum.patch), with three unused locals 88 (first +0x22f); the denominator through
// the existing dot3f, either argument order, 224 (first +0x60, aligned 68: the ratio ranks below the deltas). The
// plane-normal loads sort by a class, not by the source order: the normal's j and k are fixed by their offsets, its i
// follows the plane pointer's symbol number mod 8 (why unused locals moved earlier counts with period 8). The plane as
// `&planes[pass]` after two separate flag tests gives the classes retail's orders need but computes the pointer 378
// (1835 B); the two flag tests with a conditional plane 249 (1867 B); the conditional plane ahead of the two-arm tests
// 387 (1835 B); one combined flag test with the conditional plane 382 (1867 B).
// M3 lane 2 r146 (H2M_FAST with 0x1648d0 and 0x320b0, both MATCH each run; control 115, aligned 64, 22 with registers
// masked; body unchanged). vector_products.h's i, k, j dot helper (basis_dot_xzy), not tried here before but for the plane
// distances: the clip denominator through it, delta first or normal first, 224 each (first +0x60, aligned 58: the same code
// for both argument orders, all three rows scaled in place, zero and near distance exchanged XMM4/XMM5 as with dot3f); the
// clip distance through it, either argument order, 115 (aligned 64, masked 21); the classification through it, normal
// first 143, point first 263 (1867 B); the denominator and the distance through it 228, 225 with the normal first in the
// denominator; the helper denominator with the intersection through vector3f_add_scaled 226 (aligned 51, masked 15: the
// closest structure, but the same XMM4/XMM5 exchange from +0x60 on). On main: the scale helper writing a second vector
// 115 (same code); `real t` declared ahead of the delta 115 (same code); `real t = 0.0f` ahead of the delta with no else
// arm 239 (first +0x60). Reading of the operand ranks (the compiler sorts a product's operands by rank, descending,
// ties in source order, and forms the product in the first): a load through a pointer ranks by its offset (+4 below +8,
// offset 0 by the pointer's symbol number mod 8), a scalar local by its symbol number, and here every local ranks below
// every such load except an offset-0 load whose pointer's number is a multiple of 8. This body's three delta parts and
// the ratio are locals numbered i, k, j, ratio, so each normal load is formed first and the ratio is formed first in every
// row. Retail forms delta.i and delta.k first in the denominator, normal.j first in its third product, the ratio first
// in the x row only and the y and z deltas in place: delta.i at or below the ratio, delta.j above the ratio but not above
// a +4 load, delta.k not below a +8 load, and the plane pointer's number a multiple of 8. No form of the existing delta
// and scale helpers gives the y and z parts those two ranks (they are numbered together); that is what the written-out
// 18 and 10 forms reach by naming one part.
// Whole-vector coefficient forms: both captured before the copies 455; near/far/both shared dots with coefficient
// first 437/440/517; near/far/both with normal first 437/449/496; both vectors early with shared dots 517. Early
// x-coefficient and following zero register roles still differ.
// In-place factor-first x and delta-first y/z scaler 115; with separate scaled delta 467; with shared sum 115;
// with point setter 115; with shared denominator dot 224; with denominator in the fraction local 221;
// with shared near normal negation 115; with shared forward negation for the far plane 130.
// The early x coefficient preload and exchanged zero XMM home remain.
// Lane GA r3 (fast with 0x1648d0/0x320b0, both matched unless noted; 67 trials across 6 batches): major structural lead:
// calling ray_plane_distance (including unmodified ray_plane_distance_1648d0, 0x1648d0 stays MATCH) with vector_products.h's
// vector3f_add_scaled naturally produces rank(delta.i) < rank(t) < rank(delta.j), rank(delta.k) without variable reuse or
// written-out helpers, matching the entire head (+0x00..+0x22f, e4.i preload into XMM4 and xorps xmm4, xmm4) and all three
// intersection rows at retail's 1851 B (173/170/169 positional only because a 1-byte diff in the k denominator product —
// movss xmm0, [ecx+8]; mulss xmm0, xmm3 (9 B) vs retail movaps xmm0, xmm3; mulss xmm0, [ecx+8] (8 B) — drops one alignment
// nop before the projection loop and shifts the identical 115-line tail). Tried: t=0 after denom/delta 252/239; plane helpers +
// vector3f_negate_ab (+ indexed proj / vector3f_add / vector3f_add_scaled) 230-315; n[] loops 266; ternary bounds 290;
// a one-caller crossing-point helper 249; single-expr denom/ternary t/no p0-p1 pointers + vector3f_add_scaled 119/121/119;
// point3f & refs 115; unmodified ray_plane_distance_1648d0 + vector3f_add_scaled 173 (+ all-3 vector3f_negate_ab 196);
// i,k,j ray_plane_distance + vector3f_add_scaled + walkout/out[k] 170/169, + all-3 vector3f_negate_ab 178 (matches all 6 XMM
// regs of head & intersection), + basis_dot_xzy 174, + single-expr dot 190, + delta i,k,j 264, + walkout2 177, + real_point3d_set
// 121/170, + plane helpers 206-274; changing shared ray_plane_distance_1648d0 to i,k,j breaks 0x1648d0 (186/194); diagnostic
// partial-sum ratio + plane helpers + vector3f_scale 106 (reuses t for denominator, not kept) and phase sweep k=1..15 on
// rpd_ikj + walkout 156..208 (period 8, best 156 at k=4/12, ~18 aligned diffs). Body unchanged.
// M3 lane AO r1 (fast; control 115): the intersection as vector_products.h's vector3f_add_scaled(p0, &delta, t, ...) in
// place of the scale and the three adds 119 (1851 B: the x row is retail's `t * delta.i + p0.x`, but the y and z rows
// form t first where retail scales delta.j and delta.k in place, and the head's XMM roles rotate once more); the segment
// flag set by an `if` 274 (1867 B); `outside0` as bool 259; the second pass without its `pass == 1 &&` 370; the clip
// denominator and the near distance each as one expression 247/251 (1867 B); plane 0's d as `near_distance - d` 423;
// the pass's plane as `&planes[pass]` behind one conditional test 380 (1835 B); the projection output taken as
// `&projected_points[total]` with no cursor 249 (1867 B). Body unchanged.
// M3 lane 12 r189 (reduced-image screen with 0x1648d0 and 0x320b0, MATCH each time; plain confirmed; control 115). Kept, 115
// -> 52: the clip block through the file's ray_plane_distance_1648d0 and vector_products.h's vector3f_add_scaled (the
// hand-written denominator, distance and rows were those helpers' bodies; alone 176 at 1851 B, the head then retail's),
// the negated forward through 0xb7740's file's vector3f_negate, and both planes negated through 0x168b40's file's
// plane_negate_168b40 (both declared here and expanded at link time; 0x168b40 and 0x1697c0 still match), with the
// planes typed as the shared plane type. Whole-image caution: with the negated forward through vector_products.h's
// vector3f_negate_ab instead (same code here, 52) matched 0x185be0 loses a two-row tie (the reloads of zero and one
// after a call swap); that happens whenever vector3f_negate_ab and vector3f_add_scaled are both expanded in this file
// (either alone, or plane_negate_168b40 with one of them, leaves 0x185be0 matched), wherever the header is included
// and whether plane_negate_168b40 lives in a header or in its own file. Reading that led there: operands of a product or sum are laid out by a rank. A load through a pointer at
// offset 0 ranks by the pointer's symbol number mod 8, a load at an offset by the count of expression nodes evaluated
// ahead of its first reading (two levels a node, mod 8), a local by its symbol number. On the two-helper clip block the
// plane normal's i/j/k sit at levels 5/2/4 and retail needs 1/6/0: four more symbols ahead of `plane` and two more nodes
// ahead of the classification. Diagnostic grid on it (0-7 unused locals x 0-3 redundant float pairs on the near sum):
// 176/190/170/178, 198/204/190/192, 214/214/206/202, 189/197/50/54, 177/191/38/46, 180/188/174/174, 196/198/190/184,
// 188/196/182/184, so 38 at four locals and two pairs. Each setup spelling measured for its levels and count: the near
// distance as one expression (sum written twice) 4/2/4 190; vector3f_negate_ab for the negated forward 0/2/4 207 (1830 B); for
// plane 0's normal 5/2/4 176 (same code); for plane 1's normal 5/4/6 210; plane 0's or plane 1's d through basis_dot_xzy
// 205/202 (first +0xab/+0x12e) or dot3f 233/226; plane 0's d as `near_distance - d` or `-d + near_distance` 5/6/0 442
// (first +0x5d); a far plane pointer for the normal copy 6/2/4 198; the near distance by an `if` 4/2/4 195; `0.0f - x`
// for either plane's normal or d 177 (same code as `-x`); the far flag cleared by an `if` 477; the second pass without
// `pass == 1 &&` 351 (one node fewer, 1/0/2). Combinations on the two-helper block: negated helper + far plane pointer
// 1/2/4 177; + plane 1's normal through the helper 1/4/6 193; + plane 1 negated through a one-pointer plane helper 1/0/2
// 46; + plane 1 (or both planes) negated through the two-pointer helper 1/6/0 38; without the far plane pointer (the
// kept form) 0/6/0 52; with a far-point pointer or a near-point pointer read three times 1/6/0 40 each (one of two
// parallel sites named: not kept), with both 2/6/0 174, both and the one-expression near distance 1/6/0 56 (first +0xbb).
// Diagnostics on the 38 form (not kept): a private i, k, j copy of the distance helper 13, with dot3f for both sums 17;
// on the 13 form `total` declared first 10; a walking `out` 10-11 (first +0x397); `out`, `culled`, a `source` element
// pointer, `total`, `cursor` in that order with `&source[k]` 8, a walking `source` 148 (the entry block is then retail's
// but the element pointer sits at +0 where retail's sits at +8); all 30 orders of those statements 8-11. Left for a
// match: one more symbol ahead of `plane`, the shared distance helper's denominator summed by rank (0x1648d0 needs j,
// k, i from the same text), the candidate's x load one level lower in the clip distance, and `total` in EAX.
// M3 lane GA r9 (fast with 0x1648d0, 0x320b0, 0x168b40, 0xbc070 and 13 file peers, all MATCH; control 52; 70 trials):
// projection tail without `output` copy (`total = 0` before `segment`, `point3f *out = cursor;` with `out++` at k-loop
// bottom, `cursor += 2; culled = false; total += 2;` or `out; cul; tot; cur`): 45 (gain 7, under the 8-point bar for >30;
// restores `total` in EAX, retail's alignment jmp, `total/segment/segment_count` store order and `n.j * p0.y`; 0x1648d0
// MATCH; NOT kept); other statement orders 45-47, `out++` before bounds 67, indexed `projected_points[total]` 62;
// `total` first alone 49; uninitialized `i`/`segment` at top 52/50, initialized `total=0`/`cursor` before pass 343-475;
// `plane3f plane0, plane1` split (replacing `planes[2]`) 186-188 (first +0x22f: shifts plane rank past 1/6/0); local
// plane-init helper 233; changing shared `ray_plane_distance_1648d0`'s `dot` from 3 `+=` statements to `dot3f` (either
// order), `dot3f` for both sums, or any flat 3-term sum (`i+j+k`/`i+k+j`/`j+k+i`) drops 0x1652e0 to 31 (fixing all of
// delta, intersection and `n.j * p0.y` with `i,k,j` sum order up to the plane-rank swap) while keeping 0x1648d0's `j,k,i`
// sum order, but breaks matched 0x1648d0 (186 at 1855 B: removing the `+=` statements drops `[direction+4]`'s rank below
// `plane->normal.j` and `distance`); `dot` with `plane_distance_to_point` 35 (0x1648d0 186); right-grouped `j + (k + i)`
// recovers `[direction+4]` in 0x1648d0 (27, only sum order `(k+i)+j`) and scores 29 here; 2-stmt `i` then `j+k` 25
// (0x1648d0 200); `dot3f` for `offset` only 56 (0x1648d0 16); `return`/ternary/`distance=0` in rpd 230-251; rpd param
// reorders 31 (0x1648d0 186); 3 term locals in rpd 31 (0x1648d0 188). Body unchanged.
// Classification with named x/y, x/z or y/z subtotals 55/68/62; x/y or x/z subtotal with a separate
// accumulation 55/70; named component products 185; signed classification bytes 52; separate branch
// stores 755. The three unrolled copies still need retail's product schedule; the signed-byte form ties.
// M3 lane 15 r147 (fast screen with 0x1648d0, 0x320b0, 0x168b40 and 0xb7740, all MATCH each run; control 52; without
// the last two files the reduced image scores 555). Lead, match-only (a renaming reference): the classification reading
// the plane through `plane3f const *const &clip = plane;` (bound to a converted temporary) 30: the three unrolled copies
// become retail's j, i, k, but the first copy's and the remainder loop's j term now load the point first; the same 30
// with the reference ahead of `long i` or after it, passed to the clip helper too 30, and in all 48 term orders and
// operand sides of the classification 30 (the text order is ignored). On that lead with lane GA r9's walking-output tail
// (`total` first, `point3f *out = cursor;` and `out++` at the k-loop bottom) 25 (the projection entry is then retail's
// but for `cursor += 2` scheduled early; the statements in output/culled/total/cursor order 25). Not better: the
// reference bound to `plane` itself (`plane3f *const &`, ahead of the plane tests) 46; a plain `plane3f const *` copy
// 199; the pass's plane typed const 52 (same code); the classification index, `total`, or one segment/segment_count
// pair shared by both loops declared at the top 52/52/52 (same code); on the 25 form, the point read through a pointer
// or a reference 45/45 (the unrolled copies go back to the old order), through a const reference to its address 183,
// the clip helper's start point through a const reference 25 (same code), its direction 349, its plane 47, both 185;
// diagnostic, the helper's dot as one i, k, j expression on the 25 form 55 (0x1648d0 186). Body unchanged.
// GB r14 (fast with 0x1648d0, 0x320b0, 0x168b40, 0xbc070 all MATCH; control 52 at 1851 B, al 61, mk 37; 38 trials):
// dropping `forward` and using symmetric 3-use `near_origin = &view->vector_e4` and `far_origin = &view->vector_b4`
// (+1 symbol ahead of `plane`, 1/6/0) gives 38 (+0x410, al 41, mk 17), plus GA r9's walking projection tail (`total`
// first, `point3f *out = cursor;`, `out++` at k-loop bottom) 33 (+0x410, al 37, mk 16); changing shared
// `ray_plane_distance_1648d0`'s `dot` from three `+=` statements to one flat sum (`i*i + j*j + k*k`, avoiding the
// `dot3f` expansion that breaks matched 0x185be0 by 2) while dropping the redundant `direction` local in 0x1648d0
// keeps 0x1648d0 and 0x185be0 MATCH and drops 0x1652e0 to 10 (1851 B, 474/474 insns, al 10, mk 8; kept, verified in
// plain check with all 15 file peers + 0x185be0 MATCH). Review kept it without the two statement-order moves
// (`total` declared first, `out` declared ahead of `culled`), which are match-only: 14 (+0x28a, 1851 B; 0x1648d0 and
// 0x185be0 MATCH). Also tried (not kept): symmetric plane0/plane1
// pointers without `forward` 245/211; both origins + one-expr near_distance + walk tail 35; real_max/real_min helpers
// 185-190 (with near_sum 45); `dot3f` for `dot` 10 (same code here, 0x1648d0 MATCH, breaks 0x185be0 by 2); `dot3f` for
// both dot and offset 12 (+0x456, breaks 0x1648d0 169); plane_distance_to_point for offset 12 (0x1648d0 MATCH); `p0 + 1`
// 70; 2-stmt offset (`y*j + z*k; += i*x`, all
// 4 operand orders, or with `origin` local in 0x1648d0) 6 (+0x55d, al 6, mk 4: fixes both +0x28a and clip distance i
// term, leaving only `total` in ECX vs EAX, but breaks 0x1648d0 by 8 on a 2-insn `origin->z * normal.k` swap); `y*j; +=
// z*k + i*x` 10 (0x1648d0 8); projection entry statement orders (`out;cur;cul;tot` puts `total` in EAX, 10 al 13 / 6
// al 9 on 2-stmt offset; `out;tot;cul;cur` 10; `tot;out;cul;cur` 11; `source` ptr 10-11).
// GB r16 (fast with 0x1648d0, 0x320b0, 0x168b40, 0xbc070 all MATCH; control 14 at 1851 B; 91 trials): dropping the
// artificial `cursor`/`out` locals in the projection loop and indexing `projected_points[total] = s;` with `total++;`
// inside the k-loop reproduces retail's exact projection entry (`mov eax, [esp+0x20]; mov esi, 2; add eax, esi; mov
// edx, edi; mov byte [esp+0x13], 0; mov ecx, ebx; mov [esp+0x20], eax; add edi, 0x18`) AND fixes +0x28a and +0x46d,
// dropping 0x1652e0 from 14 to 5 (+0x456, 1851/1851 B, 0x1648d0 untouched MATCH; kept, plain verified). With `total`
// declared before `segment` (match-only order) it scores 2 (+0x456, only `y*j` left; without `outside0` 2 at +0x46d,
// only `i*x` left); `point3f *out = &projected_points[total];` + `total++;` scores 4 (7 without `total` first); combining
// `out = &projected_points[total]` + `total++;` + `total` first + no `outside0` + 2-stmt offset in shared
// `ray_plane_distance_1648d0` hits 0 (MATCH) on 0x1652e0 but breaks matched 0x1648d0 by 8 (`directions[i].i * distance`
// operand swap), so not kept. Also tried (not kept): `cursor++` in k-loop without `out` 320-359; `cursor[k]` 231/131;
// `total += 2`/`cursor += 2` after k-loop 130-138; `src` ptr 10/471; `index0` local 11/4/2; dropping `outside0` alone
// 12/9; dropping `t` 11/2; direct `vector3f_negate` into `planes[1].n` 467; `if` `near_distance` 73; `for` segment loops
// 145/1869; `plane_distance_to_point` in `outside[i]` 49 / in rpd 13/6; rpd early return 248 / `== 0` 15 / 3 named terms 8/6.
// M3 lane 15 r149 (h2m fast and a reduced image with 0x320b0, 0x1648d0, 0x168b40, 0xb7740; control 5). Reading the
// allocator's operand ranks on the `total`-first form (2, only the clip distance's y*j left): the y*j product's two
// loads tie (level 6 each; plane i/j/k at 1/6/0, start point x/y/z at 0/6/0) and the tie resolves start point first.
// Tried on that form: the helper's first term written `plane->normal.j * origin->y` 2 (5 on main; 0x1648d0 MATCH: ties
// ignore text order); 0-7 unused longs at the top x 0-3 paren pairs on the near sum, best 2 (others 49-232); 1-7 unused
// longs after the projection loop or in the clip block 2 or 130 (the tied pair keeps its levels); outside1 named 210;
// the culled test unparenthesised 2; no plane cast 2; p1 declared first 4; `real const t` 2; intersection declared
// first 2; one ternary store 351; `++segment` 2; the delta written out in all six orders 255-256 (first +0xb3); the
// clip call's start point as `&local_points[segment * 2]` 6, its plane as `&planes[pass]` 278, both 278.
// @retail 0x1652e0
bool function_1652e0(s_view *view, real margin, const point3f *points, long point_count, point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1)
{
	point3f local_points[32];
	byte outside[32];
	byte segment_culled[16];
	plane3f planes[2];
	bool culled = true;

	use_plane1 &= (view->flag84 || view->flag85);
	*bounds = *g_4687e0;

	memset(segment_culled, 0, sizeof(segment_culled));

	real near_sum = view->margin + margin;
	real near_distance = 1.52588e-5f > near_sum ? 1.52588e-5f : near_sum;

	vector3f const *near_origin = &view->vector_e4;
	planes[0].n = *(vector3f const *)&view->point54;
	planes[0].d = near_origin->i * planes[0].n.i + near_origin->k * planes[0].n.k + near_origin->j * planes[0].n.j;
	plane_negate_168b40(&planes[0], &planes[0]);
	planes[0].d += near_distance;

	vector3f negated;
	vector3f_negate((vector3f const *)&view->point54, &negated);
	vector3f const *far_origin = &view->vector_b4;
	planes[1].n = negated;
	planes[1].d = far_origin->i * planes[1].n.i + far_origin->k * planes[1].n.k + far_origin->j * planes[1].n.j;
	plane_negate_168b40(&planes[1], &planes[1]);

	memcpy(local_points, points, point_count * sizeof(point3f));

	for (long pass = 0; pass < 2; pass++)
	{
		plane3f *plane;
		if (pass == 0)
		{
			if (!use_plane0)
				continue;
			plane = &planes[0];
		}
		else
		{
			if (pass == 1 && !use_plane1)
				continue;
			plane = &planes[1];
		}

		long i;
		for (i = 0; i < point_count; i++)
		{
			outside[i] = plane->n.i * local_points[i].x + plane->n.k * local_points[i].z + plane->n.j * local_points[i].y - plane->d > -0.0001f ? 1 : 0;
		}

		long segment_count = point_count / 2;
		long segment = 0;
		if (segment_count > 0)
		{
		do
		{
			byte outside0 = outside[segment * 2];
			segment_culled[segment] |= (!outside0 && !outside[segment * 2 + 1]);
			if (!segment_culled[segment] && outside0 != outside[segment * 2 + 1])
			{
				point3f *p0 = &local_points[segment * 2];
				point3f *p1 = &local_points[segment * 2 + 1];
				vector3f delta;
				vector3d_from_points3d(p0, p1, &delta);
				real t = ray_plane_distance_1648d0(p0, &delta, (s_plane const *)plane);
				point3f intersection;
				vector3f_add_scaled((vector3f const *)p0, &delta, t, (vector3f *)&intersection);
				if (!outside0)
					*p0 = intersection;
				else
					*p1 = intersection;
			}
			segment++;
		} while (segment < segment_count);
		}
	}

	long segment = 0;
	long segment_count = point_count / 2;
	long total = 0;
	if (segment_count > 0)
	{
	do
	{
		if (!segment_culled[segment])
		{
			culled = false;
			for (long k = 0; k < 2; k++)
			{
				point3f t;
				transform_point(&view->matrix, &local_points[segment * 2 + k], &t);
				real w = t.z > -1.52588e-5f ? -1.52588e-5f : t.z;
				real factor = -1.0f / w;
				point3f s;
				s.x = t.x * factor;
				s.y = t.y * factor;
				s.z = t.z;
				if (projected_points)
					projected_points[total] = s;
				if (bounds->x0 > s.x)
					bounds->x0 = s.x;
				if (s.x > bounds->x1)
					bounds->x1 = s.x;
				if (bounds->y0 > s.y)
					bounds->y0 = s.y;
				if (s.y > bounds->y1)
					bounds->y1 = s.y;
				if (bounds->z0 > s.z)
					bounds->z0 = s.z;
				if (s.z > bounds->z1)
					bounds->z1 = s.z;
				total++;
			}
		}
		segment++;
	} while (segment < segment_count);
	}

	if (projected_count)
		*projected_count = (short)total;
	return !culled;
}

/* the colors of the connection bars: the frame, then good, fair and poor */
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;
color4f *g_4686d8;
color4f *g_4686dc;
color4f *g_4686e8;

void function_36880(s_short_rectangle const *rectangle, color4f const *color);

/* draws a connection quality widget: a frame and one bar per quality step */
// @retail 0x1634d0
void connection_widget_draw(s_text_widget_c const *widget)
{
	if (widget->valid)
	{
		long bar_count;
		color4f color;
		s_short_rectangle bounds;
		color4f const *bar_color;
		long i;

		if (widget->text[0] < 1)
		{
			bar_count = 1;
		}
		else if (widget->text[0] > 6)
		{
			bar_count = 6;
		}
		else
		{
			bar_count = widget->text[0];
		}
		color = *(color4f *)g_4686d4;
		bounds.left = widget->bounds.left + 1;
		bounds.right = widget->bounds.right - 1;
		color.alpha = widget->color_b.alpha;
		bounds.top = widget->bounds.bottom - 0x12;
		bounds.bottom = widget->bounds.bottom - 1;
		function_36880(&bounds, &color);

		bounds.left = widget->bounds.left + 1;
		bounds.right = widget->bounds.right - 1;
		bounds.top = widget->bounds.bottom - 3;
		bounds.bottom = widget->bounds.bottom - 1;
		if (bar_count >= 4)
		{
			color = *g_4686dc;
		}
		else if (bar_count >= 2)
		{
			color = *g_4686e8;
		}
		else
		{
			color = *g_4686d8;
		}
		color.alpha = widget->color_b.alpha;
		for (i = bar_count; i > 0; i--)
		{
			function_36880(&bounds, &color);
			bounds.top -= 3;
			bounds.bottom -= 3;
		}
	}
}
