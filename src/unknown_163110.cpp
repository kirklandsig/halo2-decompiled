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
		real offset = origin->y * plane->normal.j + origin->z * plane->normal.k;
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
			vector3f_add_scaled((vector3f const *)&source->points[4], &source->directions[i], distance, (vector3f *)&points[i + 4]);
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

/* clips line segments against the view's near and far planes, projects them and updates bounds */
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
			segment_culled[segment] |= (!outside[segment * 2] && !outside[segment * 2 + 1]);
			if (!segment_culled[segment] && outside[segment * 2] != outside[segment * 2 + 1])
			{
				point3f *p0 = &local_points[segment * 2];
				point3f *p1 = &local_points[segment * 2 + 1];
				vector3f delta;
				vector3d_from_points3d(p0, p1, &delta);
				real t = ray_plane_distance_1648d0(p0, &delta, (s_plane const *)plane);
				point3f intersection;
				vector3f_add_scaled((vector3f const *)p0, &delta, t, (vector3f *)&intersection);
				if (!outside[segment * 2])
					*p0 = intersection;
				else
					*p1 = intersection;
			}
			segment++;
		} while (segment < segment_count);
		}
	}

	long total = 0;
	long segment = 0;
	long segment_count = point_count / 2;
	if (segment_count > 0)
	{
	do
	{
		if (!segment_culled[segment])
		{
			culled = false;
			point3f *out = &projected_points[total];
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
					*out = s;
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
				out++;
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
