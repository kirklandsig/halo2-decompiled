/* UNKNOWN_2BA3E0.CPP: filling a placement (a position, a unit direction and
   a second vector) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_1eb550.h"

// @flags /O2 /arch:SSE /Gr

/* a placement (0x28 bytes) */
struct s_placement
{
	point3f position;
	vector3f direction;
	vector3f up;
	short value24;
};

/* the direction used when the given one cannot be normalized */
extern vector3f *g_4687bc;

real function_1201a0(vector3f *v, vector3f const *fallback);

// @retail 0x2ba3e0
void placement_set(s_placement *placement, vector3f const *direction, point3f const *position, vector3f const *up, short value)
{
	vector3f normal = *direction;

	function_1201a0(&normal, g_4687bc);
	placement->value24 = value;
	placement->position = *position;
	placement->direction = normal;
	placement->up = *up;
}

#include "data_array.h"
extern s_record_pool *g_51ec84;

struct s_particle_2b96
{
    long id;
    long next;
    byte unknown08[0x1c - 8];
    point3f position;
    vector3f velocity;
    byte unknown34[12];
};

PRIVATE __forceinline real inverse_sqrt_2b96(real squared)
{
    real inverse;
    __asm
    {
        rsqrtss xmm0, squared
        movss inverse, xmm0
    }
    return inverse;
}

// @retail 0x2b9670
s_particle_2b96 *function_2b9670(s_particle_2b96 *particle, long index)
{
    s_particle_2b96 *result = 0;
    vector3f velocity = particle->velocity;
    real squared = velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k;
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        velocity.i *= inverse;
        velocity.j *= inverse;
        velocity.k *= inverse;
    }
    real closest = 0.0f;
    while (index != NONE)
    {
        s_particle_2b96 *other = &((s_particle_2b96 *)g_51ec84->data)[index & 0xffff];
        index = other->next;
        if (other != particle)
        {
            vector3f delta;
            delta.i = particle->position.x - other->position.x;
            delta.j = particle->position.y - other->position.y;
            delta.k = particle->position.z - other->position.z;
            real distance = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
            if (distance < closest || !result)
            {
                result = other;
                closest = distance;
            }
        }
    }
    return result;
}

PRIVATE __forceinline real particle_vector_squared_2b(vector3f const *v)
{
    real squared = v->k * v->k;
    squared += v->i * v->i;
    squared += v->j * v->j;
    return squared;
}

PRIVATE __forceinline void scale_particle_vector_2b(real scale, vector3f *v)
{
    v->i = scale * v->i;
    v->j = v->j * scale;
    v->k = v->k * scale;
}

PRIVATE __forceinline void normalize_particle_vector_2b(vector3f *v)
{
    real squared = particle_vector_squared_2b(v);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, v);
    }
}

// @retail 0x2b98b0
void function_2b98b0(s_particle_2b96 const *particle, s_particle_2b96 const *other, real scale, vector3f *result)
{
    vector3f a = particle->velocity;
    vector3f b = other->velocity;
    normalize_particle_vector_2b(&a);
    normalize_particle_vector_2b(&b);
    result->i += (b.i - a.i) * scale;
    result->j += (b.j - a.j) * scale;
    result->k += (b.k - a.k) * scale;
}

PRIVATE __forceinline real particle_separation_squared_values(real z, real y, real x)
{
    real squared = z * z;
    squared += y * y;
    squared += x * x;
    return squared;
}

// @retail 0x2b9750
void function_2b9750(s_particle_2b96 const *other, s_particle_2b96 const *particle, real radius, real scale, vector3f *result)
{
    if (fabs(scale) < 0.0001f)
        return;
    vector3f delta;
    delta.k = other->position.z - particle->position.z;
    delta.i = other->position.x - particle->position.x;
    delta.j = other->position.y - particle->position.y;
    real initial_squared = particle_separation_squared_values(delta.k, delta.j, delta.i);
    real bounded_radius = radius > 0.1f ? radius : 0.1f;
    radius = initial_squared;
    *(long *)&radius = (*(long *)&radius >> 1) + 0x1fc00000;
    real ratio = radius / bounded_radius;
    delta.k = particle->position.z - other->position.z;
    delta.i = particle->position.x - other->position.x;
    delta.j = particle->position.y - other->position.y;
    real normalized_squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
    if (normalized_squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(normalized_squared);
        scale_particle_vector_2b(inverse, &delta);
    }
    if (ratio < 1.0f)
        scale = 0.0f - ratio * scale;
    delta.i *= scale;
    delta.j *= scale;
    delta.k *= scale;
    result->i += delta.i;
    result->j += delta.j;
    result->k += delta.k;
}

#include "globals.h"
#include "unknown_246cd0.h"
void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values);
void function_211b70(short index, real const *position, byte flags, vector3f *out);
real function_17c900(short function_type, real input);

struct s_particle_property_entry_2ba
{
    long unused;
    s_particle_property property;
};
struct s_particle_properties_2ba
{
    byte unknown00[8];
    s_particle_property_entry_2ba *properties;
    dword constant_mask;
    dword input_mask;
};

struct s_particle_cache_2ba
{
    real values[17];
    dword valid;
    void *current_system;
    void *current_emitter;
    s_particle_2b96 *current_particle;
};

struct s_particle_entry_group_2ba
{
    byte unknown00[8];
    long mode;
    long count;
    s_particle_properties_2ba *entries;
};

void function_2b8da0(s_particle_properties_2ba const *definition, void *system,
    long first, void const *origin, real scale, long mode);

// @retail 0x2ba3a0
void function_2ba3a0(s_particle_entry_group_2ba const *group, long first,
    void *system, void const *origin, real scale)
{
    for (long i = 0; i < group->count; ++i)
    {
        function_2b8da0(&group->entries[i], system, first, origin, scale, group->mode);
    }
}

// @retail 0x2ba100
void function_2ba100(s_particle_properties_2ba const *definition, void *system, long first, real scale)
{
    dword remaining = definition->constant_mask;
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    dword valid = 0;
    void *current_system = 0;
    void *current_emitter = 0;
    s_particle_2b96 *current_particle = 0;
    real properties_values[3] = { 0.0f, 0.0f, 0.0f };
    real values[17];
    if (system)
    {
        current_system = system;
        valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, current_system, current_emitter, current_particle, values);
    valid |= requested;
    s_particle_property_entry_2ba *first_properties = properties;
    for (dword i = 0; i < 3 && remaining; ++i, ++properties)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            properties_values[i] = function_246cd0(&properties->property, values);
            remaining &= ~bit;
        }
    }
    properties = first_properties;
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        if (!(*((byte *)particle + 2) & 9) && *(real *)((byte *)particle + 8) <= 1.0f)
        {
            if (particle != current_particle)
            {
                current_particle = particle;
                valid &= 0xffff07f0;
            }
            requested = definition->input_mask & 0xf80f;
            function_173ba0(requested & ~valid, current_system, current_emitter, current_particle, values);
            valid |= requested;
            for (dword j = 0; j < 3; ++j)
            {
                if (!(definition->constant_mask & (1 << j)))
                    properties_values[j] = function_246cd0(&properties[j].property, values);
            }
            short cluster = *(short *)((byte *)system + 0x20);
            short wind_index = NONE;
            if (cluster != NONE)
            {
                byte *clusters = *(byte **)((byte *)g_4e0348 + 0xa0);
                wind_index = *(short *)(clusters + cluster * 0xb0 + 0x76);
            }
            vector3f wind;
            function_211b70(wind_index, particle->position.n, 8, &wind);
            real time = g_510c54->game_time * g_510c54->rate;
            real phase = function_17c900(10, time + properties_values[1] * scale);
            real amount = (phase - 0.5f) * scale;
            *(real *)((byte *)particle + 0x34) += amount * properties_values[2];
            amount *= properties_values[0];
            vector3f delta;
            delta.i = amount * wind.i;
            delta.j = wind.j * amount;
            delta.k = wind.k * amount;
            particle->velocity.i += delta.i;
            particle->velocity.j += delta.j;
            particle->velocity.k += delta.k;
        }
    }
}

#include "unknown_0259a0.h"

// @retail 0x2b9a00
void function_2b9a00(s_particle_2b96 const *particle, real distance, vector3f *result)
{
    vector3f direction = particle->velocity;
    union
    {
        s_collision_result_1697c0 value;
        byte storage[0x5c];
    } collision;
    real squared = direction.i * direction.i + direction.j * direction.j + direction.k * direction.k;
    collision.value.unknown24 = NONE;
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, &direction);
    }
    direction.i *= distance;
    direction.j *= distance;
    direction.k *= distance;
    if (function_1697c0(0x800005, &particle->position, &direction, NONE, NONE, &collision.value))
    {
        vector3f const *normal = (vector3f const *)(collision.storage + 0x28);
        result->i += normal->i;
        result->j += normal->j;
        result->k += normal->k;
    }
}

PRIVATE __forceinline real particle_distance_estimate_2b(real squared)
{
    union { real value; long bits; } estimate;
    estimate.value = squared;
    estimate.bits = (estimate.bits >> 1) + 0x1fc00000;
    return estimate.value;
}

PRIVATE __forceinline void normalize_particle_velocity_2b(vector3f *v)
{
    real squared = v->i * v->i + v->j * v->j + v->k * v->k;
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        v->i *= inverse;
        v->j *= inverse;
        v->k *= inverse;
    }
}

// @retail 0x2b9b10
void function_2b9b10(s_particle_properties_2ba const *definition, void const *origin, long first, real scale, void *system)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        if (particle != cache.current_particle)
        {
            cache.current_particle = particle;
            cache.valid &= 0xffff07f0;
        }
        requested = definition->input_mask & 0xf80f;
        function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
        cache.valid |= requested;
        next = particle->next;
        s_particle_2b96 *nearest = function_2b9670(particle, first);
        struct { real minimum_speed, maximum_speed, maximum_change; vector3f change, delta, previous; } steering;
        real &minimum_speed = steering.minimum_speed;
        minimum_speed = function_246cd0(&properties[6].property, cache.values);
        real &maximum_speed = steering.maximum_speed;
        maximum_speed = function_246cd0(&properties[7].property, cache.values);
        real &maximum_change = steering.maximum_change;
        maximum_change = function_246cd0(&properties[8].property, cache.values);
        vector3f &change = steering.change;
        change.i = change.j = change.k = 0.0f;
        if (nearest)
        {
            function_2b9750(nearest, particle,
                function_246cd0(&properties[0].property, cache.values),
                function_246cd0(&properties[1].property, cache.values), &change);
            function_2b98b0(particle, nearest,
                function_246cd0(&properties[4].property, cache.values), &change);
        }
        real attraction_scale = function_246cd0(&properties[3].property, cache.values);
        real attraction_radius = function_246cd0(&properties[2].property, cache.values);
        if (attraction_radius > 0.0f)
        {
            point3f const *center = (point3f const *)((byte const *)origin + 0x10);
            vector3f &delta = steering.delta;
            delta.i = center->x - particle->position.x;
            delta.j = center->y - particle->position.y;
            delta.k = center->z - particle->position.z;
            real squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
            real amount = particle_distance_estimate_2b(squared) / attraction_radius * attraction_scale;
            if (squared != 0.0f)
            {
                real inverse = inverse_sqrt_2b96(squared);
                scale_particle_vector_2b(inverse, &delta);
            }
            delta.i *= amount;
            delta.j *= amount;
            delta.k *= amount;
            change.i += delta.i;
            change.j += delta.j;
            change.k += delta.k;
        }
        function_2b9a00(particle, function_246cd0(&properties[5].property, cache.values), &change);
        change.i *= scale;
        change.j *= scale;
        change.k *= scale;
        real squared = change.k * change.k + change.j * change.j + change.i * change.i;
        if (particle_distance_estimate_2b(squared) > maximum_change)
        {
            if (squared != 0.0f)
            {
                real inverse = inverse_sqrt_2b96(squared);
                scale_particle_vector_2b(inverse, &change);
            }
            change.i *= maximum_change;
            change.j *= maximum_change;
            change.k *= maximum_change;
        }
        vector3f &local_c01284 = steering.previous;
        local_c01284 = particle->velocity;
        scale_particle_vector_2b(0.75f, &local_c01284);
        particle->velocity.i += change.i;
        particle->velocity.j += change.j;
        particle->velocity.k += change.k;
        particle->velocity.i *= 0.25f;
        particle->velocity.j *= 0.25f;
        particle->velocity.k *= 0.25f;
        particle->velocity.i += local_c01284.i;
        particle->velocity.j += local_c01284.j;
        particle->velocity.k += local_c01284.k;
        squared = particle->velocity.k * particle->velocity.k + particle->velocity.j * particle->velocity.j + particle->velocity.i * particle->velocity.i;
        if (squared > maximum_speed * maximum_speed)
        {
            normalize_particle_velocity_2b(&particle->velocity);
            particle->velocity.i *= maximum_speed;
            particle->velocity.j *= maximum_speed;
            particle->velocity.k *= maximum_speed;
        }
        else if (minimum_speed * minimum_speed > squared)
        {
            normalize_particle_velocity_2b(&particle->velocity);
            particle->velocity.i *= minimum_speed;
            particle->velocity.j *= minimum_speed;
            particle->velocity.k *= minimum_speed;
        }
    }
}


PRIVATE __declspec(noinline) void particle_damping_update_2b(s_particle_properties_2ba const *definition,
    void *system, long first, real scale)
{
    dword remaining = definition->constant_mask;
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    dword valid = 0;
    void *current_system = 0;
    void *current_emitter = 0;
    s_particle_2b96 *current_particle = 0;
    real properties_values[3] = { 0.0f, 0.0f, 0.0f };
    real values[17];
    if (system)
    {
        current_system = system;
        valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, current_system, current_emitter, current_particle, values);
    valid |= requested;
    for (dword i = 0; i < 3 && remaining; ++i)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            properties_values[i] = function_246cd0(&properties[i].property, values);
            remaining &= ~bit;
        }
    }
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        if (!(*((byte *)particle + 2) & 9) && *(real *)((byte *)particle + 8) <= 1.0f)
        {
            if (particle != current_particle)
            {
                current_particle = particle;
                valid &= 0xffff07f0;
            }
            requested = definition->input_mask & 0xf80f;
            function_173ba0(requested & ~valid, current_system, current_emitter, current_particle, values);
            valid |= requested;
            for (dword j = 0; j < 3; ++j)
            {
                if (!(definition->constant_mask & (1 << j)))
                    properties_values[j] = function_246cd0(&properties[j].property, values);
            }
            real linear = properties_values[1] * scale;
            real angular = properties_values[2] * scale;
            linear = linear < 0.0f ? 0.0f : (linear > 1.0f ? 1.0f : linear);
            angular = angular < 0.0f ? 0.0f : (angular > 1.0f ? 1.0f : angular);
            particle->velocity.k -= g_51e9c4->unknown0 * properties_values[0] * scale;
            real linear_remaining = 1.0f - linear;
            particle->velocity.i *= linear_remaining;
            particle->velocity.j *= linear_remaining;
            particle->velocity.k *= linear_remaining;
            *(real *)((byte *)particle + 0x34) *= 1.0f - angular;
        }
    }
}

void __stdcall function_2b9060(s_particle_properties_2ba const *definition, void *system,
    long first, real scale, long mode);

#include "effects.h"

struct s_object_246eb0;
struct s_particle_impact
{
    point3f position;
    vector3f direction;
    vector3f normal;
    short index;
};

class c_particle_collision_interface_2b
{
public:
    virtual dword group() = 0;
    virtual long tag_index() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void v13() = 0;
    virtual bool stop_on_surface() = 0;
    virtual bool stop_on_object() = 0;
};

void function_246fa0(s_object_246eb0 const *particle, s_particle_impact const *impact, long tag_index);
void function_247000(s_object_246eb0 const *particle, volatile long tag_index);
void function_247070(s_object_246eb0 const *particle, s_particle_impact const *impact, long definition_index, long tag_index);

real g_55e618;

// @retail 0x2b9060
void __stdcall function_2b9060(s_particle_properties_2ba const *definition, void *system,
    long first, real scale, long mode)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    c_particle_collision_interface_2b *material = (c_particle_collision_interface_2b *)function_137bd0(
        ((s_particle_system_datum *)system)->function_1751d0()->tag_index);
    dword remaining = definition->constant_mask;
    long next;
    real coefficients[2];
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    coefficients[0] = 0.0f;
    coefficients[1] = 0.0f;
    next = first;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    for (dword i = 0; i < 2 && remaining; ++i)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            coefficients[i] = function_246cd0(&properties[i].property, cache.values);
            remaining &= ~bit;
        }
    }
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        word *flags = (word *)((byte *)particle + 2);
        if ((*flags & 9) || !(*(real *)((byte *)particle + 8) <= 1.0f))
            continue;
        word countdown = *flags >> 13;
        if (countdown)
        {
            *flags = (*flags & 0x1fff) | ((countdown - 1) << 13);
            continue;
        }
        if (cache.current_particle != particle)
        {
            cache.current_particle = particle;
            cache.valid &= 0xffff07f0;
        }
        requested = definition->input_mask & 0xf80f;
        function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
        cache.valid |= requested;
        dword collision_flags = 0x800000 + ((mode & 2) != 0);
        if (mode & 2) collision_flags |= 4;
        else collision_flags &= ~4;
        if (mode & 4) collision_flags |= 2;
        else collision_flags &= ~2;
        if (mode & 0x10) collision_flags |= 0x20;
        else collision_flags &= ~0x20;
        if (mode & 8) collision_flags |= 0x400;
        else collision_flags &= ~0x400;
        if (mode & 0x20) collision_flags |= 0x10;
        else collision_flags &= ~0x10;
        if (collision_flags & 0x430) collision_flags |= 8;
        if (!(mode & 1) && !(mode & 2) && !(mode & 4) && !(mode & 8))
        {
            *flags &= ~2;
            continue;
        }
        vector3f displacement;
        displacement.i = particle->velocity.i * scale;
        point3f start = particle->position;
        displacement.j = particle->velocity.j * scale;
        displacement.k = particle->velocity.k * scale;
        vector3f sweep;
        sweep.i = displacement.i * 12.0f;
        sweep.j = displacement.j * 12.0f;
        sweep.k = displacement.k * 12.0f;
        start.x -= displacement.i;
        start.y -= displacement.j;
        start.z -= displacement.k;
        union
        {
            s_collision_result_1697c0 value;
            byte storage[0x5c];
        } collision;
        collision.value.unknown24 = NONE;
        if (function_1697c0(collision_flags, &start, &sweep, NONE, NONE, &collision.value))
        {
            if (g_55e618 > *(real *)(collision.storage + 4))
            {
                for (dword j = 0; j < 2 && remaining; ++j)
                {
                    dword bit = 1 << j;
                    if (remaining & bit)
                    {
                        coefficients[j] = function_246cd0(&properties[j].property, cache.values);
                        remaining &= ~bit;
                    }
                }
                vector3f const *normal = (vector3f const *)(collision.storage + 0x28);
                real keep = 1.0f - coefficients[0];
                vector3f velocity;
                velocity.i = particle->velocity.i;
                velocity.j = particle->velocity.j;
                velocity.k = particle->velocity.k;
                real dot = velocity.i * normal->i + normal->k * velocity.k + normal->j * velocity.j;
                vector3f perpendicular;
                perpendicular.i = dot * normal->i;
                perpendicular.j = dot * normal->j;
                perpendicular.k = dot * normal->k;
                vector3f tangent;
                tangent.i = velocity.i - perpendicular.i;
                tangent.j = velocity.j - perpendicular.j;
                tangent.k = velocity.k - perpendicular.k;
                particle->position.x = normal->i * 0.005f + collision.value.point.x;
                particle->position.y = normal->j * 0.005f + collision.value.point.y;
                particle->position.z = normal->k * 0.005f + collision.value.point.z;
                s_particle_impact impact;
                impact.index = NONE;
                placement_set((s_placement *)&impact, &particle->velocity, &particle->position, normal, collision.value.unknown24);
                particle->velocity.i = tangent.i * keep - perpendicular.i * coefficients[1];
                particle->velocity.j = tangent.j * keep - perpendicular.j * coefficients[1];
                particle->velocity.k = tangent.k * keep - perpendicular.k * coefficients[1];
                *(real *)((byte *)particle + 0x34) *= keep;
                if (!(*flags & 2))
                {
                    if (((s_particle_system_datum *)system)->function_1751d0()->location_mode != 1 && material->tag_index() != NONE)
                    {
                        long definition_index = ((s_particle_system_datum *)system)->function_1751d0()->tag_index;
                        long tag_index = material->tag_index();
                        dword group = material->group();
                        if (tag_index != NONE)
                        {
                            switch (group)
                            {
                            case 'snd!': function_247000((s_object_246eb0 *)particle, tag_index); break;
                            case 'foot': function_247070((s_object_246eb0 *)particle, &impact, definition_index, tag_index); break;
                            case 'effe': function_246fa0((s_object_246eb0 *)particle, &impact, tag_index); break;
                            }
                        }
                    }
                    *flags |= 2;
                }
                if (normal->k > 0.8f) *flags |= 0x10;
                else *flags &= ~0x10;
                if ((*(long *)collision.storage == 1 && material->stop_on_surface()) ||
                    (*(long *)collision.storage == 3 && material->stop_on_surface()) ||
                    (*(long *)collision.storage == 2 && material->stop_on_object()))
                    *flags |= 1;
            }
            else
                *flags &= 0x1fff;
        }
        else
            *flags = (*flags & 0x1ffd) | 0xa000;
    }
}

// @retail 0x2b8da0
void function_2b8da0(s_particle_properties_2ba const *definition, void *system,
    long first, void const *origin, real scale, long mode)
{
    switch (*(word const *)definition)
    {
    case 0:
        particle_damping_update_2b(definition, system, first, scale);
        break;
    case 1:
        function_2b9060(definition, system, first, scale, mode);
        break;
    case 2:
        function_2b9b10(definition, origin, first, scale, system);
        break;
    case 3:
        function_2ba100(definition, system, first, scale);
        break;
    }
}
