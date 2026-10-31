#include "unknown_11c920.h"
#include "unknown_1946f0.h"

// @flags /O1 /Oi /Ob1 /arch:SSE /Gr

PRIVATE __forceinline void direction_face_point(s_direction_face const *face, vector3f const *point, vector3f *out)
{
	real local_0 = face->scale;
	real x = point->i;
	real y = point->j;
	real z = point->k;
	if (local_0 != 1.f)
	{
		x *= local_0;
		y *= local_0;
		z = local_0 * z;
	}
	out->i = face->axes[2].i * z + face->axes[1].i * y + face->axes[0].i * x + face->origin.i;
	out->j = face->axes[2].j * z + face->axes[1].j * y + face->axes[0].j * x + face->origin.j;
	out->k = face->axes[2].k * z + face->axes[1].k * y + face->axes[0].k * x + face->origin.k;
}

/* Expands a face index and two six-bit coordinates to a unit direction. */
// score 19 (was 50): the components are stored in retail's order (i, j then
// k), so our zero is materialised after the multiplies where retail keeps it
// in xmm3 from the top of the function; the low six bits also come from a
// byte move here instead of retail's full copy of the index. Tried: a long
// low coordinate (57, 3 bytes short: no copy of the zero for z); the helper
// testing face->scale directly as 0x194d30 does, with a long coordinate (37,
// same size and the zero copied to z as retail, but the zero still made after
// the multiplies, into xmm2); that form with k stored first (30, zero at the
// top as retail but in xmm2 and stored first); reading z first (47) or z
// multiplied first (39); x*= vs scale*x forms (19); a plain inline helper (not
// inlined, 217); a vector3f local copied to the direction (125); a set helper
// for the three stores (68); /O2 /Ob1 flags (60). Round 89: a long low coordinate with the scale read
// from the face at each use is retail's code with the zero and z in xmm2/xmm3 swapped (37); on it, z
// read first 47, z/y/x reads 43, `z *= scale` 37, z scaled first 39, `x * (2/63)` or `x * (1/63) * 2`
// products 37; scale local after the reads 64/65 or inside the test 63; the point read into a vector3f
// local 102 (copy) or 37 (member-wise); `x * (2/63)` products here 19.
// Round 60 (an LTCG link of this file reproduces the full listing): a long coordinate with the scale
// read at each use is retail except that the zero is made after the products, in the constant's freed
// xmm2 (retail makes it first, in xmm3, and z takes xmm2); k stored first, or between i and j, makes the
// zero first but in xmm2 with the constant in xmm3. A grid local copied to the direction 109; stores
// i, k, j 28 (with a long coordinate and the scale read at each use 29, with the scale local 49); the
// existing vector3f_assign for the grid stores 19 (same code). Link-harness counts (about 2 below the check):
// a point3f grid read back as in 0x194d30's decoder 61, with the scale read at each use 55; named i, j, k
// locals 55; a face pointer local first 60; the point copied member-wise into a local 37; dead earlier
// zero stores 37 (removed before allocation); k stored first 30.
// @retail 0x24f6b0
real __fastcall function_24f6b0(dword index, vector3f *direction)
{
	int value = (int)index;
	short x_coordinate = (short)(value & 0x3f);
	long y_coordinate = (value >> 6) & 0x3f;
	direction->i = (2.f / 63.f) * (real)x_coordinate;
	direction->j = (2.f / 63.f) * (real)y_coordinate;
	direction->k = 0.f;
	direction_face_point(&g_475480[value >> 12], direction, direction);
	return function_30bf0(direction);
}
