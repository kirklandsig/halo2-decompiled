#include "unknown_11c920.h"
#include "unknown_1946f0.h"

// @flags /O1 /Oi /Ob1 /arch:SSE /Gr

PRIVATE __forceinline void direction_face_point(s_direction_face const *face, vector3f const *point, vector3f *out)
{
	real local_0 = face->scale;
	real x = point->i;
	real y = point->j;
	real z;
	if (local_0 == 1.f)
	{
		z = point->k;
	}
	else
	{
		x *= local_0;
		y *= local_0;
		z = local_0 * point->k;
	}
	out->i = face->axes[2].i * z + face->axes[1].i * y + face->axes[0].i * x + face->origin.i;
	out->j = face->axes[2].j * z + face->axes[1].j * y + face->axes[0].j * x + face->origin.j;
	out->k = face->axes[2].k * z + face->axes[1].k * y + face->axes[0].k * x + face->origin.k;
}

/* Expands a face index and two six-bit coordinates to a unit direction. */
// score 9 (was 9): retail sets z ahead of the scale test (`movaps xmm2, xmm3` before `jnp`, no else jump) with the
// scale in xmm4; ours assigns z in the arm, with the scale in xmm2. Earlier state at 19: the components are stored in retail's order (i, j then
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
// Round 68: independent original/scaled z with long x and direct scale 37 (242 bytes), with short x
// and the scale local 19, with short x and direct scale 53; mixed long x/short y 57; staged outputs 53;
// existing in-place scale helper 87; scale as a helper parameter 27; staged outputs with long x and
// direct scale 50 (242 bytes). Original helper and coordinates restored.
// Round 152: on the long coordinate with the scale read at each use (37: only the zero, made after the
// products in xmm2, and the face add after the products differ): the grid stores through the existing
// vector3f_assign 68 (254 bytes), with the scale local 57 (239 bytes); real x/y locals 37; a shared
// step constant 68; the casts named before the products 68; the index used directly 68; a face pointer
// local first or after the coordinates 75 (256 bytes).
// Round 156 (fast): a zero local between or before named products 57/57 (239 bytes); the grid through a forceinline set
// helper with the zero as argument 57 (239 bytes); z assigned in both arms of the scale test, reading point->k in each:
// short x 60 (243 bytes), long x with the scale at each use 44, long x with the scale local 10, inverted test 9 (kept;
// the zero moves to the top, in xmm3, and every line before the scale load is retail's). On the long x: point by value
// 106 (273 bytes); k local copied to z 57 (239 bytes); ternary z 70 (257 bytes); z = scale first, with else 10, inverted
// 9; z product first, with else or inverted 56/56 (244 bytes); z through the products 57; z initialised with arm x, z, y
// or z, x, y 50/50 (242 bytes: retail's copy before `jnp`, but the zero is made after the products, in the step's freed
// xmm2, and z takes another register). On that form: z declared first 51; scale local last 66 (251 bytes); casts named 50;
// step local 50; products swapped 50; mirrored arms 56 (244 bytes); scale at each use 38 (scale in xmm4 as retail, z and
// zero swapped), arm z, x, y 39, x, z, y scale-left 38, z, y, x 38, y, z, x 39, x, y, z 37; z declared first 48; reads
// y, x, z 48; z read through the out pointer 37; an else-arm read as well 10 (241 bytes).
// Lead: the zero comes first only when no z is set in the entry block; the wanted form sets z there and still has it.
// Round 201 (fast): z read in the entry block with the arm re-reading the point, `z = scale * point->k`
// 57 (239 bytes; z and the zero share a register, the zero made after the products), `point->k * scale`
// 57, `z = scale * z` 57, an empty `== 1` arm with the else scaling 57; in the else-form, `z = scale;`
// then `z *= point->k` 9 and `z = point->k * scale` 9 (same code).
// Round 35 (m3 lane 17, fast): on the scale local and short x, z set from point->k ahead of a `!=` test with arm
// x, y, z: `z = scale * point->k` 57 (239 bytes), `z *= scale` 57; that form with the scale read at each use 37
// (242 bytes, zero after the products), with k stored first 30 (zero first but in xmm2, the step in xmm3);
// k stored first on the kept inverted-test body 12 (241 bytes).
// P294: complete branch inputs 9;
// nonunity-first inputs 10;
// branch inputs with direct scale 58;
// scale-default z 9;
// returned transform output 9;
// unused transform return 9;
// returned output with branch inputs 9;
// returned output with scale-default z 9.
// Original executable body restored;
// no exact match from these ownership forms.
// @retail 0x24f6b0
real __fastcall function_24f6b0(dword index, vector3f *direction)
{
	int value = (int)index;
	long x_coordinate = value & 0x3f;
	long y_coordinate = (value >> 6) & 0x3f;
	direction->i = (2.f / 63.f) * (real)x_coordinate;
	direction->j = (2.f / 63.f) * (real)y_coordinate;
	direction->k = 0.f;
	direction_face_point(&g_475480[value >> 12], direction, direction);
	return function_30bf0(direction);
}
