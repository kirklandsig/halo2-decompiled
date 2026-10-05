#include "unknown_246cd0.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "effects.h"

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

PRIVATE inline point2f const *particle_spawn_extents(s_particle_system_datum const *system)
{
	if (system->effect_index != NONE)
		return (point2f const *)&((s_effect_datum *)g_4ea93c->data)[system->effect_index & 0xffff].unknown74;
	return g_468760;
}

// @retail 0x249170
void function_249170(real const *values, s_particle_spawn_state *particle, s_particle_spawn_definition const *definition, s_particle_system_datum const *system)
{
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
		particle->position.z = 0.f;
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		break;
	}
	case 1:
	{
		real azimuth;
		particle_random_fraction(&g_4e7408->seed, azimuth);
		azimuth *= 6.2831855f;
		real radius = function_246cd0(&definition->radius, values);
		particle->velocity.i = 0.f;
		particle->velocity.j = (real)sin(azimuth);
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
		particle_scale_position(radius, &particle->velocity, &particle->position);
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
		dword *seed = &g_4e7408->seed;
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
		dword *seed = &g_4e7408->seed;
		bool side = (real)(random_next(seed) & 1) == 0.f;
		particle->position.x = 0.f;
		particle->velocity.i = (real)cos(polar);
		real sign = (random_next(seed) & 1) ? -1.f : 1.f;
		if (side)
		{
			particle->position.y = extents->x * sign;
			particle->position.z = function_259d0(seed, NULL, 0, -extents->y, extents->y);
			particle->velocity.j = (real)sin(polar) * sign;
			particle->velocity.k = 0.f;
		}
		else
		{
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
		dword *seed = &g_4e7408->seed;
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
		dword *seed = &g_4e7408->seed;
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
		particle->position.z = 0.f;
		particle->velocity.i = (real)cos(polar);
		particle->velocity.j = (real)sin(azimuth) * radial;
		particle->velocity.k = (real)cos(azimuth) * radial;
		break;
	}
	default: __assume(0);
	}
	vector3f *velocity = &particle->velocity;
	real speed_squared = velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k;
	if (speed_squared != 0.f)
		particle_scale_vector(particle_inverse_sqrt(speed_squared), velocity);
	particle_scale_vector(function_246cd0(&definition->speed, values), velocity);
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
