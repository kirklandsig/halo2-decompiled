// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_245BD0.CPP: collision tests of points and rays against capsules, spheres
   and flat prisms, plus a few small object accessors */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "matrix_transform.h"
#include "data_array.h"
#include "vector_products.h"
#include "globals.h"
#include <float.h>
#include <math.h>

#define k_real_epsilon 0.0001f

real function_30bf0(vector3f *v);
void record_pool_release(s_record_pool *data, long datum_index);
matrix3x3 *function_142da0(real yaw, real pitch, real roll, matrix3x3 *out);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
vector3f *function_143070(matrix3x3 const *m, vector3f const *v, vector3f *out);

/* what a shape belongs to (copied into a test's result) */
struct s_shape_header
{
	long unknown00;
	long unknown04;
	long unknown08;
	byte unknown0c;
	byte unknown0d;
	short unknown0e;
};

/* a segment (origin and vector) with a radius */
struct s_capsule
{
	s_shape_header header;
	point3f origin;
	vector3f vector;
	real radius;
};

struct s_sphere
{
	s_shape_header header;
	point3f center;
	real radius;
};

/* a slab: a plane, a thickness, and a convex polygon on the plane projected
   onto two axes */
struct s_prism
{
	s_shape_header header;
	plane3f plane;
	real thickness;
	short axis;
	byte side;
	byte unknown27;
	long point_count;
	point2f points[8];
};

/* projects a point on a prism's plane onto the two axes the prism was flattened along */
PRIVATE void prism_project(real const *q, short axis, byte side, point2f *out)
{
	out->x = q[g_440b94[side + axis * 2][0]];
	out->y = q[g_440b94[side + axis * 2][1]];
}

// @retail 0x245bd0
bool function_245bd0(
	point3f const *point,
	s_capsule const *capsule,
	plane3f *plane,
	real *distance)
{
	vector3f d;
	vector3d_from_points3d(&capsule->origin, point, &d);
	real dot = dot3f(&capsule->vector, &d);
	if (dot >= g_45dbd8)
	{
		real length2 = length_sq3f(&capsule->vector);
		if (length2 >= dot)
		{
			real d2 = length_sq3f(&d);
			if (d2 * length2 - dot * dot < capsule->radius * capsule->radius * length2)
			{
				if (length2 > g_45dbd8)
				{
					real t = dot / length2;
					vector3f scaled;
					scaled.i = capsule->vector.i * t;
					scaled.j = capsule->vector.j * t;
					scaled.k = capsule->vector.k * t;
					plane->i = d.i - scaled.i;
					plane->j = d.j - scaled.j;
					plane->k = d.k - scaled.k;
				}
				else
				{
					plane->n = d;
				}
				real magnitude = function_30bf0(&plane->n);
				if (magnitude == 0.f)
				{
					plane->i = 0.f;
					plane->j = 0.f;
					plane->k = 1.f;
				}
				plane->d = dot3f((vector3f const *)&capsule->origin, &plane->n) + capsule->radius;
				*distance = capsule->radius - magnitude;
				return true;
			}
		}
	}
	return false;
}

PRIVATE inline void shape_difference2d(point2f const *a, point2f const *b, point2f *out)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
}

PRIVATE inline real shape_cross2d(point2f const *a, point2f const *b)
{
	return a->x * b->y - a->y * b->x;
}

// @retail 0x245d80
bool function_245d80(
	s_prism const *prism,
	point3f const *point,
	real *distance,
	plane3f *out)
{
	real height = plane_distance_to_point(&prism->plane, point);
	if (height >= 0.f && prism->thickness > height)
	{
		real offset = 0.f - height;
		real q[3];
		q[0] = offset * prism->plane.i + point->x;
		q[1] = prism->plane.j * offset + point->y;
		q[2] = prism->plane.k * offset + point->z;
		point2f p2;
		prism_project(q, prism->axis, prism->side, &p2);
		long count = prism->point_count;
		long i = 0;
		if (count > 0)
		{
			do
			{
				point2f const *a = &prism->points[i];
				point2f da;
				shape_difference2d(a, &p2, &da);
				long next = (i + 1 >= count) ? 0 : i + 1;
				point2f const *b = &prism->points[next];
				point2f db;
				shape_difference2d(b, &p2, &db);
				if (0.f > shape_cross2d(&da, &db))
				{
					return false;
				}
				i++;
			} while (i < count);
		}
		out->n = prism->plane.n;
		out->d = prism->plane.d + prism->thickness;
		*distance = prism->thickness - height;
		return true;
	}
	return false;
}

/* the shapes tested together: spheres, capsules and prisms */
struct s_shapes
{
	short counts[3];
	byte unknown06[2];
	s_sphere spheres[256];
	s_capsule capsules[256];
	s_prism prisms[64];
};

/* the deepest shape a point is in */
struct s_shape_result
{
	s_shape_header header;
	real depth;
	byte unknown14[0xc];
	plane3f plane;
};

PRIVATE inline void shape_scale3d(vector3f const *v, real scale, vector3f *out)
{
	out->i = v->i * scale;
	out->j = scale * v->j;
	out->k = v->k * scale;
}

/* The point test inlined into the shape collection query. */
PRIVATE inline bool shape_sphere_point(s_sphere const *sphere, point3f const *point, plane3f *plane, real *depth)
{
	vector3f d;
	vector3d_from_points3d(&sphere->center, point, &d);
	real distance_squared = d.j * d.j + d.k * d.k + d.i * d.i;
	if (sphere->radius * sphere->radius > distance_squared)
	{
		real distance = (real)sqrt(distance_squared);
		if (distance > g_45dbd8)
		{
			real scale = 1.f / distance;
			shape_scale3d(&d, scale, &plane->n);
		}
		else
		{
			plane->i = plane->j = 0.f;
			plane->k = 1.f;
		}
		plane->d = dot3f((vector3f const *)&sphere->center, &plane->n) + sphere->radius;
		*depth = sphere->radius - distance;
		return true;
	}
	return false;
}

PRIVATE inline void shape_copy_header(s_shape_header const *source, s_shape_header *out)
{
	out->unknown00 = source->unknown00;
	out->unknown04 = source->unknown04;
	out->unknown08 = source->unknown08;
	out->unknown0c = source->unknown0c;
	out->unknown0d = source->unknown0d;
	out->unknown0e = source->unknown0e;
}

// score 57 (was 57): the sphere test's normal: retail multiplies the x and z differences in
// place (normal x/y/z in xmm1/xmm3/xmm2, differences x/z/y in xmm1/xmm2/xmm4) and loads 1.0 in
// each arm; ours multiplies copies of the scale, keeps z/y in xmm4/xmm3 and hoists 1.0 into xmm0
// before the fsqrt. The products go in place only when the sum's first reads of x and z are
// pointer reads and y's is not: a pointer local to the difference read for x and z in a
// statement sum x, z, y, the dot's inputs swapped and the unit-z zeros stored separately matches,
// and so does a one-caller helper returning the x and z squares through its pointer plus
// `+= d.j * d.j` (fast MATCH; neither keepable).
// Tried: unit-z zeros chained y-first 57 (y or z first 57, x first 61), stored separately 61,
// through vector3f_assign 61; unit-z arm first 62; arms swapped 170; depth before plane->d 61-65,
// before the normalization branch 173; distance-squared orders i,k,j 61, k,i,j 59, k,j,i 57,
// regrouped 57-61, direct statement sums in each order 61, products by fields after an x,z,y
// statement sum 57; squared-length/self-dot helpers (length_sq3f, dot3f(&d, &d), the existing
// reverse dot on the difference) 152 (also with both fixes, with vector3f_scale or the reciprocal
// passed to shape_scale3d); every square through a pointer local plus the dot swap 148; the scale
// helper's statement orders 61-158, operand orders 57, the factor as its first parameter 57;
// existing vector3f_scale with the reciprocal in the call or a scale local 57 (plus dot(normal,
// center) 57, plus vector3f_assign unit z 61); scale declared first 61; reciprocal in the call
// 57; double reciprocal 397; the difference by fields 61, in the plane normal 177-181; in-place
// scale + copy 397; separate/default normal workspace 397; radius-square local 61; a radius local
// 176 (668 bytes); component-square lifetimes 61; immutable distance, sphere and point references
// 61; shared scaled difference 57; squared-distance accumulator and paired difference 61/61;
// normal memset and accumulator/reset 176/180; component scale loops 57/57, with a normal reset
// 175; outside-sphere exit 57; the comparison's operands swapped 57; root declared first 57;
// sqrtf 57; default output before normalization 174; center/sphere value copies 397/396; center
// plus squared helper 397; late radius after normal/before root/after root, shared double-factor
// scale, in-place input, late radius with scale, native factor, partial squared distance
// 62/65/65/397/397/397/397/61; containment-test return, plane/depth pointer returns, normal-first
// projection, projection value, existing reverse dot in either order, test/projection result 57;
// dot swap 57; squared-distance axial array loop (j term then axes 2 and 0, long/short ordinal,
// either projection input order, chained/separate zeros) 397 at 456 bytes (sphere helper
// outlined), forced inline 235-262; a nested unit-vector helper taking the distance (reciprocal
// inside it) with length_sq3f 129 (and a one-caller helper); a named hit flag with if/else-if
// arms 157 (669 bytes); depth and plane declared at function scope 57; the distance test against
// the literal 0.f (0x45dbd8 is the 0.0 constant) 57. Compile-only (no gain):
// the sphere helper's 24 parameter orders and __forceinline; statement differences x,z,y; scalar
// component locals; radius square first; a 72-form screen of the squared-distance, scale, unit-z
// and dot helpers. Round 13 (exact LTCG harness, ~250 forms): the squared distance in every flat,
// parenthesized and statement order 57-61; the squared-length/self-dot/reverse-dot helpers with
// either plane-dot order, the reciprocal in the call and separate unit-z zeros 150-152 (aligned
// lower, but the difference is then computed i, j, k); the pointer local (diagnostic) leaves only
// the plane dot's i product, as the dot swap fixes; a single-exit result 57; the test inverted 58;
// a center pointer used twice 170 (669 bytes); single exit with length_sq3f 150; x and z summed
// before y 59; the difference written out as plain assignments (diagnostic) 57-61 in every order.
// Round 3 (lane 18, the unit-z arm and the 1.0 retail loads in each arm): unit z stored before the
// zeros 57; the arm as a const local {0, 0, 1} copied to the normal 397 (456 bytes); distance declared
// ahead of the radius test 57; the arm through the normal member (`plane->n.i = plane->n.j = 0.f`) 57,
// with unit z first 57. 1.0 stays hoisted ahead of the fsqrt in every form.
// Round 28 (lane 10): ours also hoists the 1.0 into XMM0 above the fsqrt for both arms (retail loads
// it in each arm). The squared distance through the existing out-of-line 0x24550 (k, i, j sum) 303
// (stays a call, 772 bytes); the sphere helper __forceinline 57 (same code); j + (i + k) grouping (the
// inline normalize's) 59; the scale helper __forceinline 57 (same code); the point copied by value
// 397 (helper outlined, 456 bytes).
// Round 27 (lane 11, real builds): the sphere helper forced inline 57; its parameters (point, sphere,
// plane, depth) 57, (sphere, point, depth, plane) 57, (point, sphere, depth, plane) 57; the sphere
// passed as `shapes->spheres + index` 57.
// Round 22 (lane 13; aligned difflib count 37, all in the sphere test: difference registers z/y,
// copies of the scale for x and y, the hoisted 1.0 and the plane-dot registers): the scale helper
// taking the factor by const reference 57 (0x2461a0 4 -> 50); the radius added first in plane->d
// 57; the best-* locals declared in reverse 59; plane declared before depth in the loop 57;
// best_depth declared first 60.
// Round 97 (lane 8, fast with 0x2461a0/0x19cb90/0x245bd0/0x245d80): the normalization test written
// `g_45dbd8 < distance` 57 (same code); the difference scaled in place (unit z in the other arm) and
// copied to the plane once 397 (456 bytes, helper outlined); the unit-z arm storing x and copying it to
// y 61, storing y and copying it to x 57; that with the reciprocal in the call 57. Ours keeps n.k in the
// scale's register (the k product goes in place on the scale), so both arms start with 1.0 in XMM0 and
// it is hoisted above the fsqrt; retail keeps n.k in the z difference's register.
// Round 32 (lane 11, fast with 0x19cb90/0x2461a0): the matching form's two keepable halves together (the
// dot's inputs swapped and the unit-z zeros stored separately) 61; with the sum grouped j + (i + k) 61, flat
// i, k, j 61, as i, k, j statements 61; flat i, k, j with the existing vector3f_scale 61.
// Round 7 (lane 17, fast with 0x2461a0/0x245bd0/0x245d80; the sphere helper forced inline where noted):
// the difference scaled in place and copied to the plane after the arms (unit z into the difference) 205
// (720 bytes: the difference goes to the frame), copied inside the scale arm 212 (726); a separate normal
// built in both arms and copied 205 (720); a pointer to the plane's normal for both arms and the dot 57
// (same code); the squared distance declared before the difference 57; the helper without `inline` 57
// (still inlined, same code).
// Round 113 (lane 6, fast; all with the dot's inputs swapped and the unit-z zeros stored separately): the
// sum as statements x, z, y 61, as `j + (i + k)` 61, as dot3f(&d, &d) 152, as length_sq3f(&d) 152, as
// statements j, i, k 61. Diagnostics (not keepable): length_sq3f(&d) plus any otherwise unused direct read
// of d.j, before or after the sum, 4 (only the sum's x/z order differs: z first); the same with dot3f or the
// reverse dot 4; a direct read of d.i instead 21; direct reads of d.j and d.k 164 (670 bytes). So the y
// product is a copy of the scale exactly when d.j is read directly somewhere before the normalization while
// i and k are read only through the vector's address (helpers or a pointer), as the listed pointer form.
// Round 115 (lane 6, fast): length_sq3f(&d) with the test written `distance_squared < r * r` 152; dot3f(&d, &d)
// with that test 152; the difference made in the plane normal, length_sq3f(&plane->n) and the existing
// vector3f_scale in place 176 (696 bytes); the sum grouped `i + (k + j)` 61, `k + (j + i)` 57. Harness
// screens (exact here): the eight other flat/parenthesized groupings 57-61; vector3f_scale with the
// reciprocal in the call 57, with length_sq3f 152.
// Round 59 (lane 12, exact one-file LTCG harness, 143 forms): the squared distance as j,k,i / i,k,j /
// k,i,j / i,j,k flat, `j + (i + k)`, i,k,j statements, length_sq3f or dot3f(&d, &d), crossed with
// shape_scale3d with a scale local or the reciprocal in the call or vector3f_scale, the unit-z zeros
// chained either way or stored separately, and either dot order: 57 (12 forms: the j,k,i sum or
// i,k,j statements with any scale/dot form and the chained y-first zeros), 59 (12), 61 (84), 152 (30),
// 162 (6, 671 bytes); i,k,j statements with the reciprocal in the call, separate zeros and the
// swapped dot 61 (fast agrees).
// Round 117 (fast): the listed pointer form still matches; its pointer sum alone (original dot and
// chained zeros) 123, with i and j through the pointer and k direct 169. With the dot's inputs swapped
// and the unit-z zeros stored separately: length_sq3f(&d) with the normal written by fields 152; the
// difference written by fields in x, z, y order with an x, z, y statement sum 59; flat k, i, j 61;
// `j + (k + i)` 61; k, i, j statements 61.
// Round 42 (lane 11, fast): the squared distance from the points themselves (the difference vector kept
// for the normal) 307 (776 bytes), with the separate unit-z zeros and the swapped dot 307 (776); with the
// difference vector made only in the scale arm 222 (722); the difference held in a plane3f local's
// normal 57 (same code); the difference as a brace-initialised vector 61. Compile-only screen of the
// sphere block (GPRs masked, base 42, the listed pointer diagnostic 12): the sum direct/length_sq3f/
// dot3f/from the points x the scale helper/reciprocal in the call/fields x chained or separate zeros
// x both dot orders: 34 at best (helper-only reads give every product in place).
// Round 12 (lane 17, fast with 0x2461a0/0x245bd0/0x245d80): the squared distance through a two-point
// helper with its own difference vector (vector3d_from_points3d + length_sq3f, the form other files carry)
// beside the normal's difference 307 (776 bytes: the subtractions are not merged), with the normal's
// difference made inside the radius test 227 (722 bytes); length_sq3f(&d) with the scaled normal through
// vector3f_assign(&plane->n, d.i * scale, scale * d.j, d.k * scale) 148; the unit-z arm as a copy of a
// file-scope const {0, 0, 1} 189 (711 bytes, loaded from memory); the unit-z arm stored x, z, y separately 61.
// Round 28 (m3 lane 2, fast with 0x2461a0/0x245bd0/0x245d80): the sphere helper defined after this function
// (prototype ahead) 57 (same code: the operand order is not parse order); the scale helper taking the vector
// by value 397 (456 bytes, sphere helper outlined); `real scale;` declared first in the helper with the dot's
// inputs swapped and the unit-z zeros stored separately 61 (every product still multiplies a copy of the scale,
// so declaration order does not set it); length_sq3f(&d) with scale declared first 152; the difference copied
// to the plane normal, then the existing vector3f_scale on the normal in place 397 (helper outlined).
// The permuter cannot help here: it rewrites this function's body, and every difference is in the inlined helper.
// Round 65 (lane 12, fast with 0x2461a0): the scale helper taking the vector by value 397 (456 bytes,
// sphere helper outlined; 0x2461a0 84); the difference made in the plane's normal with the j,k,i sum
// read from it and the scale in place 181 (693 bytes); the difference copied to the normal before the
// radius test and scaled in place there 397 (456); the difference through the existing
// scaled_difference3f with scale one 57 (same code), with the flat i,k,j sum 61. No existing helper
// reads only the i and k squares through a pointer (the pointer form's shape).
// Round 58 (lane 16, fast): the best-* updates stored plane, depth, type, index 63; the root kept as a double
// with the reciprocal `(real)(1.0 / root)` 397 (456 bytes, sphere helper outlined); the normal copied from the
// difference and scaled in place on the plane 397 (456); the radius test as `!(distance_squared >= r * r)` 58;
// the squared distance as a double 397 (456). Repeats: unit-z default before the epsilon test 174 (689 bytes),
// a radius local after the normal 62 (672 bytes).
// Round 15 (lane 17, fast with 0x19cb90/0x245bd0/0x245d80/0x2461a0): the sphere passed by reference 57; flat
// i, k, j sum with the reciprocal in the call 61; a named i + k partial plus the j square 59; the
// unit-z arm through the normal member, unit z first then separate zeros 61; a scalar point-to-point
// squared-distance helper (no vector of its own) beside the difference 307 (776 bytes), with the
// difference made inside the radius test 227 (722): the subtractions are still not merged. Diagnostic:
// the unit-z arm's 1.0 read from a named global stops the 1.0 hoist (retail's fsqrt block) 153 (681 bytes).
// Round 17 (lane 17, fast): the difference from a by-value helper 397 (sphere helper outlined, 456 bytes);
// length_sq3f(&d) with the normal by fields 152 again (every product then in place and 1.0 loaded in each
// arm, as retail, but the difference and sum run k, j, i); the scale helper taking the vector by value
// 397 (outlined; 0x2461a0 84); flat i, k, j with the existing vector3f_scale 61; the normal through the
// existing scaled_difference3f from the center and point 222 (722 bytes: subtractions recomputed).
// Round 49 (lane 10, fast): the depth test folded into the shape tests as `(...) && best_depth < depth` 57 (same
// code); one difference component copied into a named scalar for the sum (0x2461a0's lever): j or k in the
// j,k,i sum 57, i 61, j or k in an i,j,k or i,k,j sum 59; the sphere test as `distance_squared < r * r` with
// length_sq3f, vector3f_scale(&d, 1.f / distance, &plane->n), vector3f_assign(&plane->n, 0.f, 0.f, 1.f) and
// `distance > 0.f` 152, with the j,k,i sum 61, with shape_scale3d 61. Diagnostic: one to three dead locals in
// the sphere helper 57 (no symbol-count rotation here).
// @retail 0x245ef0
bool function_245ef0(s_shapes const *shapes, point3f const *point, s_shape_result *result)
{
	short best_type = NONE;
	short best_index = NONE;
	real best_depth = -FLT_MAX;
	plane3f best_plane;
	for (short type = 0; type < 3; type++)
	{
		for (short index = 0; index < shapes->counts[type]; index++)
		{
			real depth;
			plane3f plane;
			if ((type == 0 && shape_sphere_point(&shapes->spheres[index], point, &plane, &depth)) ||
				(type == 1 && function_245bd0(point, &shapes->capsules[index], &plane, &depth)) ||
				(type == 2 && function_245d80(&shapes->prisms[index], point, &depth, &plane)))
			{
				if (depth > best_depth)
				{
					best_type = type;
					best_index = index;
					best_depth = depth;
					best_plane = plane;
				}
			}
		}
	}
	if (best_type != NONE)
	{
		result->depth = best_depth;
		result->plane = best_plane;
		switch (best_type)
		{
		case 0: shape_copy_header(&shapes->spheres[best_index].header, &result->header); break;
		case 1: shape_copy_header(&shapes->capsules[best_index].header, &result->header); break;
		case 2: shape_copy_header(&shapes->prisms[best_index].header, &result->header); break;
		}
		return true;
	}
	return false;
}

PRIVATE inline real shape_dot_reverse(vector3f const *a, vector3f const *b)
{
	return a->k * b->k + a->j * b->j + a->i * b->i;
}

/* the fraction of direction at which point, moving along it, first touches the sphere (0 when it
   starts inside), with the sphere's tangent plane there */
// @retail 0x2461a0
bool function_2461a0(
	s_sphere const *sphere,
	point3f const *point,
	plane3f *plane,
	vector3f const *direction,
	real *t)
{
	bool result = false;
	vector3f d;
	vector3d_from_points3d(point, &sphere->center, &d);
	real d_j = d.j;
	real radius_squared = sphere->radius * sphere->radius;
	real c = d.k * d.k;
	c += d.i * d.i;
	c += d_j * d_j;
	c -= radius_squared;
	if (0.f >= c)
	{
		*t = 0.f;
		result = true;
	}
	else
	{
		/* retail sums these in this order */
		real b = direction->i * d.i + d.k * direction->k + d_j * direction->j;
		if (b > 0.f)
		{
			real a = direction->j * direction->j + direction->k * direction->k + direction->i * direction->i;
			real discriminant = b * b - a * c;
			if (discriminant >= 0.f)
			{
				/* retail compares the unrounded root and divides the rounded one */
				real root = b - sqrt(discriminant);
				if (a >= root)
				{
					*t = root / a;
					result = true;
				}
			}
		}
	}
	if (result)
	{
		vector3f scaled;
		shape_scale3d(direction, *t, &scaled);
		plane->i = scaled.i - d.i;
		plane->j = scaled.j - d_j;
		plane->k = scaled.k - d.k;
		if (function_30bf0(&plane->n) == 0.f)
		{
			plane->i = 0.f;
			plane->j = 0.f;
			plane->k = 1.f;
		}
		plane->d = dot3f((vector3f const *)&sphere->center, &plane->n) + sphere->radius;
	}
	return result;
}

// score 104 (was 106): every sum now associates as in retail (the compiler reorders unparenthesized
// sums of three products); what still differs is scheduling of the first block: retail multiplies
// v.k by u.k from memory before summing a, and keeps a in XMM3 and uv in XMM0 (ours XMM4, XMM1), which
// swaps registers to the end. Tried: any subset of the four parenthesized sums (151-214), w computed
// k, j, i by hand (211), a or uu parenthesized (180, 206), uv unparenthesized in any order (214),
// uv as += statements (182), a/uv/uu statement orders (104-217), den = a * uu (104).
// @retail 0x246360
bool function_246360(
	vector3f const *direction,
	s_capsule const *capsule,
	point3f const *start,
	real *t,
	plane3f *plane)
{
	vector3f const *u = direction;
	vector3f const *v = &capsule->vector;
	real a = length_sq3f(v);
	real uv = (v->k * u->k + v->j * u->j) + v->i * u->i;
	real uu = length_sq3f(u);
	real den = uu * a - uv * uv;
	if (den == g_45dbd8)
	{
		return false;
	}
	vector3f w;
	vector3d_from_points3d(&capsule->origin, start, &w);
	real uw = (u->j * w.j + u->i * w.i) + u->k * w.k;
	real vw = (v->k * w.k + v->j * w.j) + v->i * w.i;
	real b = vw * uv - uw * a;
	real c = ((w.k * w.k + w.j * w.j) + w.i * w.i - capsule->radius * capsule->radius) * a - vw * vw;
	real disc = b * b - c * den;
	if (0.f > disc)
	{
		return false;
	}
	real root = (real)sqrt(disc);
	real inv = 1.f / den;
	real t_enter = (b - root) * inv;
	real t_exit = (root + b) * inv;
	if (t_enter > 1.f || 0.f > t_exit)
	{
		return false;
	}
	if (0.f > t_enter)
	{
		t_enter = 0.f;
	}
	if (t_exit > 1.f)
	{
		t_exit = 1.f;
	}
	if (uv != 0.f)
	{
		real inv_uv = 1.f / uv;
		real s0 = 0.f - inv_uv * vw;
		real s1 = (a - vw) * inv_uv;
		if (uv > 0.f)
		{
			if (s0 > t_enter)
			{
				t_enter = s0;
			}
			if (t_exit > s1)
			{
				t_exit = s1;
			}
		}
		else
		{
			if (s1 > t_enter)
			{
				t_enter = s1;
			}
			if (t_exit > s0)
			{
				t_exit = s0;
			}
		}
		if (t_enter > t_exit)
		{
			return false;
		}
	}
	else
	{
		if (0.f > vw || vw > a)
		{
			return false;
		}
	}
	*t = t_enter;
	vector3f q;
	q.i = u->i * t_enter + w.i;
	q.j = u->j * t_enter + w.j;
	q.k = u->k * t_enter + w.k;
	real along = ((v->k * q.k + v->j * q.j) + q.i * v->i) / a;
	real back = 0.f - along;
	plane->i = v->i * back + q.i;
	plane->j = v->j * back + q.j;
	plane->k = v->k * back + q.k;
	if (function_30bf0(&plane->n) == 0.f)
	{
		plane->i = 1.f;
		plane->j = 0.f;
		plane->k = 0.f;
	}
	plane->d = capsule->origin.z * plane->k + capsule->origin.y * plane->j + capsule->origin.x * plane->i + capsule->radius;
	return true;
}

/* where along start + t * direction (t in [0, 1]) the segment first enters a prism, and the
   prism's top plane */
// @retail 0x2466b0
bool function_2466b0(
	point3f const *start,
	vector3f const *direction,
	s_prism const *prism,
	real *t,
	plane3f *out)
{
	real b = plane_distance_to_point(&prism->plane, start);
	real t0 = 0.f;
	real t1 = 1.f;
	real a = shape_dot_reverse(&prism->plane.n, direction);
	if (!(fabs(a) < k_real_epsilon))
	{
		real inv = 1.f / a;
		real s0 = 0.f - inv * b;
		real s1 = 0.f - (b - prism->thickness) * inv;
		if (a > 0.f)
		{
			if (s0 > 0.f)
			{
				t0 = s0;
			}
			if (1.f > s1)
			{
				t1 = s1;
			}
		}
		else
		{
			if (s1 > 0.f)
			{
				t0 = s1;
			}
			if (1.f > s0)
			{
				t1 = s0;
			}
		}
		if (t0 > t1)
		{
			return false;
		}
	}
	else if (0.f > b || b >= prism->thickness)
	{
		return false;
	}
	vector3f q0;
	vector3f_add_scaled((vector3f const *)start, &prism->plane.n, -b, &q0);
	real na = 0.f - a;
	real q1[3];
	q1[0] = na * prism->plane.i + direction->i;
	q1[1] = prism->plane.j * na + direction->j;
	q1[2] = prism->plane.k * na + direction->k;
	point2f s2;
	prism_project(&q0.i, prism->axis, prism->side, &s2);
	point2f d2;
	prism_project(q1, prism->axis, prism->side, &d2);
	long count = prism->point_count;
	long i = 0;
	if (count > 0)
	{
		do
		{
			point2f const *vertex = &prism->points[i];
			long next = (i + 1 >= count) ? 0 : i + 1;
			point2f const *next_vertex = &prism->points[next];
			point2f edge;
			vector2d_from_points2d(vertex, next_vertex, &edge);
			point2f offset;
			vector2d_from_points2d(vertex, &s2, &offset);
			real denom = shape_cross2d(&d2, &edge);
			real num = shape_cross2d(&edge, &offset);
			if (!(fabs(denom) < k_real_epsilon))
			{
				real r = num / denom;
				if (denom < 0.f)
				{
					if (r > t0)
					{
						t0 = r;
					}
				}
				else
				{
					if (t1 > r)
					{
						t1 = r;
					}
				}
				if (t0 > t1)
				{
					return false;
				}
			}
			else if (0.f > num)
			{
				return false;
			}
			i++;
		} while (i < count);
	}
	*t = t0;
	out->n = prism->plane.n;
	out->d = prism->plane.d + prism->thickness;
	return true;
}

struct s_ray_shape_result
{
	s_shape_header header;
	real time;
	point3f position;
	plane3f plane;
};

// @retail 0x2469b0
bool function_2469b0(s_shapes const *shapes, point3f const *start,
	vector3f const *direction, s_ray_shape_result *result)
{
	short best_type = NONE;
	short best_index = NONE;
	real best_time = FLT_MAX;
	plane3f best_plane;
	for (short type = 0; type < 3; type++)
	{
		for (short index = 0; index < shapes->counts[type]; index++)
		{
			real time;
			plane3f plane;
			if (((type == 0 && function_2461a0(&shapes->spheres[index], start, &plane, direction, &time)) ||
				(type == 1 && function_246360(direction, &shapes->capsules[index], start, &time, &plane)) ||
				(type == 2 && function_2466b0(start, direction, &shapes->prisms[index], &time, &plane))) &&
				best_time > time && dot3f(direction, &plane.n) < -0.0001f)
			{
				best_time = time;
				best_type = type;
				best_index = index;
				best_plane = plane;
			}
		}
	}
	if (best_type != NONE)
	{
		result->time = best_time;
		point3f *position = &result->position;
		position->x = direction->i * best_time + start->x;
		position->y = direction->j * best_time + start->y;
		position->z = direction->k * best_time + start->z;
		result->plane = best_plane;
		switch (best_type)
		{
		case 0: shape_copy_header(&shapes->spheres[best_index].header, &result->header); break;
		case 1: shape_copy_header(&shapes->capsules[best_index].header, &result->header); break;
		case 2: shape_copy_header(&shapes->prisms[best_index].header, &result->header); break;
		}
		return true;
	}
	result->time = 1.f;
	result->position.x = start->x + direction->i;
	result->position.y = start->y + direction->j;
	result->position.z = start->z + direction->k;
	return false;
}

/* the object at 0x246eb0: clamp(|vector| - 0.5, 0, 1) of the vector at +0x28 */
struct s_object_2470e0
{
	byte unknown00[0x9c];
	vector3f vector9c;
	real unknowna8;
	real unknownac;
};

// @retail 0x2470e0
inline vector3f *function_2470e0(s_object_2470e0 *object)
{
	return &object->vector9c;
}

// @retail 0x2470f0
real *function_2470f0(s_object_2470e0 *object)
{
	return &object->unknowna8;
}

struct s_object_247120
{
	byte unknown00[0xc];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
	byte unknown0e[8];
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

// @retail 0x247120
long function_247120(s_object_247120 const *object)
{
	return object->bit7;
}

// @retail 0x247130
long function_247130(s_object_247120 const *object)
{
	return TEST_FIELD_BIT(object->flag9);
}

struct s_frame_2477b0
{
	byte unknown00[0x10];
	matrix3x3 rotation;
	point3f position;
};

// @retail 0x2477b0
void function_2477b0(
	s_object_2470e0 *object,
	s_frame_2477b0 *frame,
	transform4x3f const *source)
{
	if (source)
	{
		real const *angles = function_2470f0(object);
		frame->rotation.forward = source->forward;
		frame->rotation.left = source->left;
		frame->rotation.up = source->up;
		frame->position = source->position;
		vector3f offset;
		function_2477b1(&frame->rotation, function_2470e0(object), &offset);
		if (!(fabs(angles[1]) < k_real_epsilon) || !(fabs(angles[0]) < k_real_epsilon))
		{
			matrix3x3 rotation;
			function_142da0(angles[0], angles[1], 0.f, &rotation);
			function_142eb0(&frame->rotation, &rotation, &frame->rotation);
		}
		frame->position.x += offset.i;
		frame->position.y += offset.j;
		frame->position.z += offset.k;
	}
}
