#include "unknown_246cd0.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "effects.h"
#include "normalize3d.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_particle_spawn_definition
{
	byte unknown00[0x18];
	s_particle_property lifetime;
	s_particle_property speed;
	s_particle_property rotation;
	byte unknown48[0x78 - 0x48];
	long shape;
	s_particle_property radius;
	s_particle_property angle;
};

struct s_particle_spawn_state
{
	byte unknown00[0xc];
	real inverse_lifetime;
	byte unknown10[0xc];
	point3f position;
	vector3f velocity;
	real rotation;
};

PRIVATE __forceinline real particle_inverse_sqrt(real squared)
{
	real inverse;
	__asm
	{
		rsqrtss xmm0, squared
		movss inverse, xmm0
	}
	return inverse;
}

PRIVATE inline void particle_scale_vector(real scale, vector3f *vector)
{
	vector->i = scale * vector->i;
	vector->j = vector->j * scale;
	vector->k = vector->k * scale;
}

PRIVATE inline void particle_scale_position(real scale, vector3f const *direction, point3f *point)
{
	point->x = direction->i * scale;
	point->y = direction->j * scale;
	point->z = direction->k * scale;
}

vector3f *random_unit_vector(vector3f *result, dword *seed);
point2f const g_440af4 = { 0.f, 0.f };
point2f const *g_468760 = &g_440af4;

PRIVATE __forceinline void particle_random_fraction(dword *seed, real &result)
{
	result = function_x82e52f(seed, NULL, 0);
}

PRIVATE __forceinline point2f const *particle_spawn_extents(s_particle_system_datum const *system)
{
	if (system->effect_index != NONE)
		return (point2f const *)&((s_effect_datum *)g_4ea93c->data)[system->effect_index & 0xffff].unknown74;
	return g_468760;
}

// score 2 (was 2): what is left is the case 1 fsin/xorps order; the first normalisation product
// pair (retail folds i and loads j; ours loaded i and folded j) matches since round 91.
// Round 42: one function-scope seed
// pointer assigned in cases 5, 6, 7 and 9 (it then shares one home, the dead system argument's, as
// in retail) 57 -> 6; on it function-scope sine/azimuth/radius/polar/radial/all six 314/36/32/313/
// 395/422, extents 23, direction 356; tail without the volatile casts (plain, scale helper, `*=`)
// 47 each; case 1 in retail store order/zero pair first/position.x first/sine products 6/6/8/6.
// Before it: renamed case locals 57 (identical); case 7 seed before extents 128; case 1 velocity.j
// before velocity.i 57; squared speed through length_sq3f or dot3f 57 (identical); permuter 77
// tries, none under 6. Round 40: the position.z clear written in cases 0 and 9 (no labels) lets
// case 0 jump to case 9's store as in retail, 65 -> 58; the side test with the converted value on
// the left 58 -> 57. Then: normalisation operands swapped 57 (same code);
// volatile-free normalisation and speed scale 98, through the existing scale helper 98/98; case 0
// with the angle as the first operand, polar declared first or angle declared at the top 57 each;
// case 1 with a chained zero, no sine local (309) or the zero store first 57.
// Round 129 (82): side test with the converted value on the left 81 (fixes the ucomiss only);
// case conversion/angle/seed/extent homes, trigonometric order, side test and normalization loads differ.
// Earlier at 65: what is left is per-case stack homes (conversion temporaries, angles, seed
// and extents in cases 0, 1, 5, 7 and 9), the case 1/9 fsin/xorps order, the side test's ucomiss
// operands and the first normalisation product (scale first in retail, one byte shorter).
// extents) differ in cases 0, 1, 5, 7 and 9; also the case 1/9 fsin/xorps order, the side
// test's ucomiss operands and the first normalisation product (scale first in retail, one
// byte shorter, which also moves the jump table by four bytes).
// Round 129: side test with the converted value on the left 81 (fixes the ucomiss only);
// random draws as `real x = function_x82e52f(seed, NULL, 0); x *= k;` 82 (same code; as one
// product 554 in screen terms); named inverse local 82; the speed scale through the existing
// scale helper 91 (2348 bytes, but its loads merge); swap plus direct draws 81. Screened:
// local renames in case 0 (no slot change), `> 0.f`/reversed zero tests 109/114, an
// in/out-pointer scale helper in every operand order 100.
// Before: whole input vector 167, staged output 152, component loop 546, compound loop 546,
// input snapshot/reverse stores 167, scaled components 106, aggregate output 152, snapshot
// loop 586; matching 0x1c frame; earlier per-case expression forms excluded; with unqualified
// scale inputs: ordinary inputs 106; existing scale helper 106; shared squared length 106;
// shared azimuth 99; shared polar 329; shared shape scalars 416; scalar name order 106; case
// declarations first 106; lifetime clamp 106; property inline/noinline 106/106; random-vector
// inline/noinline 106/106; ordinary inverse inline 136; native side test 359; shared side
// sign 345; ring/sphere/both shared position producers 94/84/96; cone/disk point setters
// 84/95; shared/named direction workspace 380/82; all position helpers 111;
// direction/seed/reverse case orders 319/575/518.
// Ordinary pooled scaling with fraction result reference-first/pointer 106/106; returned real 82;
// ordinary-inline reference-first/pointer/return 819/819/976; returned real plus ordinary extent query
// 443; ordinary-inline original reference 819.
// Round 421: cone velocity setter 114; ring velocity setter 548; cone/ring setters 552; fixed/direction setter 99;
// cone/fixed setters 107; ring/fixed setters 558; all velocity setters 562; all velocity setters/final scale
// helper 562.
// Round 431: complete velocity/position/both field receivers 647/534/647; reversed receiver declarations 647;
// with shared tail scales 647/534/647; both receivers with local tail scales 647. All trial pooled reads ordinary.
// Original six differences remain: ring trig/zero order and the first normalization product pair.
// On the original case-home graph, pointer-returning shared normalization/speed/both scales 47/47/47;
// paired ring sine/cosine values alone or with each scale combination 497 each (exact-only, restored).
// Round 91: both scales through the existing vector3f_scale on &particle->velocity, without the
// volatile casts, 6 -> 2 (kept); the same on the velocity pointer local 47; the speed in a named local
// first 24; the volatile casts dropped from the plain products 47. Case 1: zero stores after both trig
// stores 2; no sine local 286; sine and cosine locals with their products 504; zero pair first 2;
// position.y from the sine local 2; zero stores last 17; position.x copied from velocity.i 2;
// velocity.i zeroed before the sine 2; radius-first products 2; permuter 68 tries, none under 2.
// Round 207: float trig family 29; native reciprocal-root intrinsic 688 (16-byte aligned frame,
// nine match losses, restored); native component array with inherited/ordinary reads 6/47;
// all three tail declarations at entry 6; tail local names 6; reciprocal/squared-speed declaration
// at entry separately 6/6. Original frame, helper, field accesses and qualified reads restored.
// Round 16: normalisation operands swapped 6 (same code); without the casts, compound `*=` 47, tail
// stores through particle->velocity 105, tail reads through particle->velocity 88 (that one keeps
// retail's load-multiply-store order: the loads and stores sit on two unrelated bases); case 1 with
// the zero stores last 21, a chained zero 6, a cosine local 504.
// Round 207: float trig family 29; native reciprocal-root intrinsic 688 (16-byte aligned frame,
// nine match losses, restored); native component array with inherited/ordinary reads 6/47;
// all three tail declarations at entry 6; tail local names 6; reciprocal/squared-speed declaration
// at entry separately 6/6. Original frame, helper, field accesses and qualified reads restored.
// Round 12 (m3 lane 2; one-link harness with 0x247950's, 0x246cd0's, 0x259d0's and 0x1462e0's files
// reproduces case 1 exactly): case 1 sinf 6, cosf 4; `position.x = velocity.i = 0.f` 2; velocity through
// vector3f_assign with sine/cosine arguments 549 (2300 bytes: case 1's tail cross-jumps into case 2's);
// a double sine 507. Harness only: the two zero stores in all 60 placements among the four trig stores
// (both statement orders) keep xorps (or the velocity lea) between fld and fsin; the velocity through
// vector3f_assign and the position through real_point3d_set 493; the position setter alone 305 (positional).
// Round 29 (lane 11): case 1 with sinf for the sine 6, cosf for the cosine 4, both 8; the cosine store
// before the sine store 11 (the one-use sine is evaluated at its use, after the fcos); the sine declared
// before the radius call 2 (same code: it is carried past the call). Our scheduler fills the fld latency
// before case 1's fsin with the xorps (as retail does in case 6) where retail's case 1 and case 4 do not.
// Round 96 (one-link harness with 0x247950's, 0x246cd0's, 0x259d0's and 0x1462e0's files; case 1 swap
// reproduced): the two zero stores placed before/after the sine definition in all four combinations keep the
// swap; either one before the radius call 549-563 (stores move ahead of the call); a cosine local after
// velocity.j / after position.x with sine/cosine products 321 (swap kept); `real const sine` 2 (same code);
// position.y before velocity.k / velocity.k before position.y 2 (same code); sine declared ahead of the
// radius call and assigned after it 2 (same code).
// Round 211: shared seed cursor extended to cases 1/0/4/8/0+1/1+4/1+8/0+1+4+8
// 2/16/2/16/9/16/9/24.
// returned random-vector outputs in cases 2/3/5/2+3/2+5/3+5/all; all with sphere position helper
// 78/460/75/477/527/473/532/535.
// All source forms restored; case 1 fsin/xorps scheduling remains.
// Round 58: reverse zero-field dependency 4; velocity/position/both zero components through the unions 2/2/2;
// all velocity/all output components 2/2; entry-wide tail velocity pointer 512; const case radius 2. Body restored.
// @retail 0x249170
void function_249170(real const *values, s_particle_spawn_state *particle, s_particle_spawn_definition const *definition, s_particle_system_datum const *system)
{
	dword *seed;
	real lifetime = function_246cd0(&definition->lifetime, values);
	particle->inverse_lifetime = 1.f / (lifetime > 0.01f ? lifetime : 0.01f);
	particle->rotation = function_246cd0(&definition->rotation, values) * (1.f / 360.f);
	switch (definition->shape)
	{
	case 0:
	{
		real azimuth;
		particle_random_fraction(&g_4e7408->seed, azimuth);
		azimuth *= 6.2831855f;
		real angle = function_246cd0(&definition->angle, values) * 0.017453292f;
		real polar;
		particle_random_fraction(&g_4e7408->seed, polar);
		polar *= angle;
		real radial = (real)sin(polar);
		particle->position.x = 0.f;
		particle->position.y = 0.f;
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		particle->position.z = 0.f;
		break;
	}
	case 1:
	{
		real azimuth;
		particle_random_fraction(&g_4e7408->seed, azimuth);
		azimuth *= 6.2831855f;
		real radius = function_246cd0(&definition->radius, values);
		real sine = (real)sin(azimuth);
		particle->velocity.i = 0.f;
		particle->velocity.j = sine;
		particle->velocity.k = (real)cos(azimuth);
		particle->position.x = 0.f;
		particle->position.y = particle->velocity.j * radius;
		particle->position.z = particle->velocity.k * radius;
		break;
	}
	case 2:
	{
		real radius = function_246cd0(&definition->radius, values);
		random_unit_vector(&particle->velocity, &g_4e7408->seed);
		particle->position.x = particle->velocity.i * radius;
		particle->position.y = particle->velocity.j * radius;
		particle->position.z = particle->velocity.k * radius;
		break;
	}
	case 3:
	{
		vector3f direction;
		random_unit_vector(&direction, &g_4e7408->seed);
		real radius = function_246cd0(&definition->radius, values);
		particle_scale_position(radius, &direction, &particle->position);
		particle->velocity.i = -direction.i;
		particle->velocity.j = -direction.j;
		particle->velocity.k = -direction.k;
		break;
	}
	case 4:
	{
		real azimuth;
		particle_random_fraction(&g_4e7408->seed, azimuth);
		azimuth *= 6.2831855f;
		real polar = function_246cd0(&definition->angle, values) * 0.017453292f;
		real radius = function_246cd0(&definition->radius, values);
		real sine = (real)sin(azimuth);
		real cosine = (real)cos(azimuth);
		particle->position.x = 0.f;
		particle->position.y = sine * radius;
		particle->position.z = cosine * radius;
		real radial = (real)sin(polar);
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = sine * radial;
		particle->velocity.k = cosine * radial;
		break;
	}
	case 5:
	{
		seed = &g_4e7408->seed;
		real azimuth;
		particle_random_fraction(seed, azimuth);
		azimuth *= 6.2831855f;
		real angle = function_246cd0(&definition->angle, values) * 0.017453292f;
		real polar;
		particle_random_fraction(seed, polar);
		polar *= angle;
		real radial = (real)sin(polar);
		real radius = function_246cd0(&definition->radius, values);
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		vector3f direction;
		random_unit_vector(&direction, seed);
		particle_scale_position(radius, &direction, &particle->position);
		break;
	}
	case 6:
	{
		point2f const *extents = particle_spawn_extents(system);
		real polar = function_246cd0(&definition->angle, values) * 0.017453292f;
		seed = &g_4e7408->seed;
		bool side = (real)(long)(random_next(seed) & 1) == 0.f;
		particle->position.x = 0.f;
		particle->velocity.i = (real)cos(polar);
		random_next(seed);
		dword local_sign = *seed;
		if (side)
		{
			real sign = ((byte)(local_sign >> 16) & 1) ? -1.f : 1.f;
			particle->position.y = extents->x * sign;
			particle->position.z = function_259d0(seed, NULL, 0, -extents->y, extents->y);
			particle->velocity.j = (real)sin(polar) * sign;
			particle->velocity.k = 0.f;
		}
		else
		{
			real sign = ((byte)(local_sign >> 16) & 1) ? -1.f : 1.f;
			particle->position.y = function_259d0(seed, NULL, 0, -extents->x, extents->x);
			particle->position.z = extents->y * sign;
			particle->velocity.j = 0.f;
			particle->velocity.k = (real)sin(polar) * sign;
		}
		break;
	}
	case 7:
	{
		point2f const *extents = particle_spawn_extents(system);
		seed = &g_4e7408->seed;
		real azimuth;
		particle_random_fraction(seed, azimuth);
		azimuth *= 6.2831855f;
		real polar = function_246cd0(&definition->angle, values) * 0.017453292f;
		real radial = (real)sin(polar);
		particle->position.x = 0.f;
		particle->position.y = function_259d0(seed, NULL, 0, -extents->x, extents->x);
		particle->position.z = function_259d0(seed, NULL, 0, -extents->y, extents->y);
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		break;
	}
	case 8:
	{
		real azimuth;
		particle_random_fraction(&g_4e7408->seed, azimuth);
		azimuth *= 6.2831855f;
		real radius = function_246cd0(&definition->radius, values);
		real distance;
		particle_random_fraction(&g_4e7408->seed, distance);
		distance *= radius;
		particle->position.x = 0.f;
		particle->position.y = (real)sin(azimuth) * distance;
		particle->position.z = (real)cos(azimuth) * distance;
		particle->velocity.i = 0.f;
		particle->velocity.j = 0.f;
		particle->velocity.k = 1.f;
		break;
	}
	case 9:
	{
		seed = &g_4e7408->seed;
		real azimuth;
		particle_random_fraction(seed, azimuth);
		azimuth *= 6.2831855f;
		real angle = function_246cd0(&definition->angle, values) * 0.017453292f;
		real polar;
		particle_random_fraction(seed, polar);
		polar *= angle;
		real radial = (real)sin(polar);
		real radius = function_246cd0(&definition->radius, values);
		particle->position.x = 0.f;
		particle->position.y = function_259d0(seed, NULL, 0, -radius, radius);
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		particle->position.z = 0.f;
		break;
	}
	default: __assume(0);
	}
	vector3f *velocity = &particle->velocity;
	real speed_squared = velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k;
	if (speed_squared != 0.f)
	{
		real inverse_speed = particle_inverse_sqrt(speed_squared);
		vector3f_scale(&particle->velocity, inverse_speed, &particle->velocity);
	}
	vector3f_scale(&particle->velocity, function_246cd0(&definition->speed, values), &particle->velocity);
}

// @retail 0x249aa0
void function_249aa0(s_particle_spawn_definition const *definition, s_particle_spawn_state *particle, point3f const *position, real const *values, vector3f const *velocity)
{
	real lifetime = function_246cd0(&definition->lifetime, values);
	real speed_squared = velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k;
	particle->inverse_lifetime = 1.f / (lifetime > 0.01f ? lifetime : 0.01f);
	particle->rotation = function_246cd0(&definition->rotation, values) * (1.f / 360.f);
	particle->position = *position;
	vector3f *out_velocity = &particle->velocity;
	*out_velocity = *velocity;
	if (speed_squared > 0.0001f * 0.0001f)
	{
		real inverse_speed = particle_inverse_sqrt(speed_squared);
		real speed = function_246cd0(&definition->speed, values) * inverse_speed;
		/* Keep the component accesses in the order used by the particle update. */
		volatile vector3f *target = out_velocity;
		target->i = speed * out_velocity->i;
		target->j *= speed;
		target->k *= speed;
	}
}
