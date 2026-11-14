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
	real dot = direction->j * plane->normal.j;
	dot += direction->k * plane->normal.k;
	dot += plane->normal.i * direction->i;
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
			vector3f const *direction = &source->directions[i];
			real distance = ray_plane_distance_1648d0(&source->points[4], direction, plane);
			points[i] = source->points[i];
			points[i + 4].x = direction->i * distance + source->points[4].x;
			points[i + 4].y = direction->j * distance + source->points[4].y;
			points[i + 4].z = direction->k * distance + source->points[4].z;
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

// score 115 (was 115): coefficient XMM roles, classification operands and projection staging still differ.
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
// @retail 0x1652e0
bool function_1652e0(s_view *view, real margin, const point3f *points, long point_count, point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1)
{
	point3f local_points[32];
	byte outside[32];
	byte segment_culled[16];
	s_plane planes[2];
	bool culled = true;

	use_plane1 &= (view->flag84 || view->flag85);
	*bounds = *g_4687e0;

	memset(segment_culled, 0, sizeof(segment_culled));

	real near_sum = view->margin + margin;
	real near_distance = 1.52588e-5f > near_sum ? 1.52588e-5f : near_sum;

	vector3f const *forward = (vector3f const *)&view->point54;
	planes[0].normal = *forward;
	planes[0].d = view->vector_e4.i * planes[0].normal.i + view->vector_e4.k * planes[0].normal.k + view->vector_e4.j * planes[0].normal.j;
	planes[0].normal.i = -planes[0].normal.i;
	planes[0].normal.j = -planes[0].normal.j;
	planes[0].normal.k = -planes[0].normal.k;
	planes[0].d = -planes[0].d;
	planes[0].d += near_distance;

	vector3f negated;
	negated.i = -forward->i;
	negated.j = -forward->j;
	negated.k = -forward->k;
	planes[1].normal = negated;
	planes[1].d = view->vector_b4.i * planes[1].normal.i + view->vector_b4.k * planes[1].normal.k + view->vector_b4.j * planes[1].normal.j;
	planes[1].normal.i = -planes[1].normal.i;
	planes[1].normal.j = -planes[1].normal.j;
	planes[1].normal.k = -planes[1].normal.k;
	planes[1].d = -planes[1].d;

	memcpy(local_points, points, point_count * sizeof(point3f));

	for (long pass = 0; pass < 2; pass++)
	{
		s_plane *plane;
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
			outside[i] = plane->normal.i * local_points[i].x + plane->normal.k * local_points[i].z + plane->normal.j * local_points[i].y - plane->d > -0.0001f ? 1 : 0;
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
				real denominator = delta.i * plane->normal.i;
				denominator += delta.k * plane->normal.k;
				denominator += plane->normal.j * delta.j;
				real t;
				if (denominator != 0.0f)
				{
					real distance = plane->normal.j * p0->y;
					distance += p0->z * plane->normal.k;
					distance += plane->normal.i * p0->x;
					t = (distance - plane->d) * (-1.0f / denominator);
				}
				else
					t = 0.0f;
				vector3f_scale(&delta, t, &delta);
				point3f intersection;
				intersection.x = delta.i + p0->x;
				intersection.y = delta.j + p0->y;
				intersection.z = delta.k + p0->z;
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
	point3f *cursor = projected_points;
	do
	{
		if (!segment_culled[segment])
		{
			culled = false;
			total += 2;
			point3f *output = cursor;
			cursor += 2;
			for (long k = 0; k < 2; k++)
			{
				point3f t;
				point3f *out = &output[k];
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
