// @flags /O2 /Gr /arch:SSE
// The speed math sits in the two inline helpers below, not in the body, because of the whole-program pass: a function
// that itself declares a 16-aligned local (`__m128`) is marked before inlining, and the functions it calls then stop
// taking arguments in ebx; the mark passes on through a callee built with /Ob1 to that callee's own callees. With the
// `__m128` locals written in this body, 0x1cfd10 (called here, an /Ob1 file) passed the mark on to 0x182b90, whose
// fourth argument moved from ebx to the stack, and matched 0x1cfd10 and 0x277ec0 were lost (the earlier BLOCKED-BY
// 0x182b90). A helper's locals join this frame only after inlining: the body's code is byte for byte what it was (frame
// aligned to 16 as in retail, 600 of 600 bytes, score 149 in the full check) and 0x182b90 keeps its ebx argument. The
// helpers take the result's address because a returned value changes the body's stores (193).
#include "unknown_1cec30.h"

struct s_2792c0_context;
struct s_component_contact_pair;
bool function_2792c0(s_2792c0_context *);
void function_1cfd10(s_component_contact_pair const *, s_havok_component *, real);

/* an entity's property value read as a real (0 when it has no such property) */
static inline real havok_entity_property_real_get(hkEntity const *entity, dword key)
{
    for (long i = 0; i < entity->m_property_count; i++)
    {
        if (entity->m_properties[i].m_key == key)
            return *(real const *)&entity->m_properties[i].m_value.m_data;
    }
    return 0.0f;
}

/* the length of a vector's first three components */
static __forceinline void havok_vector_length3(hkVector4 const &vector, real *length)
{
    __m128 squared = vector.m_quad;
    squared = _mm_mul_ps(squared, squared);
    __m128 accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0x55), squared);
    accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0xaa), accumulated);
    accumulated = _mm_sqrt_ss(accumulated);
    _mm_store_ss(length, accumulated);
}

/* the length of the difference of two vectors (first three components) */
static __forceinline void havok_vector_distance3(hkVector4 const &a, hkVector4 const &b, real *length)
{
    __m128 difference = _mm_sub_ps(a.m_quad, b.m_quad);
    difference = _mm_mul_ps(difference, difference);
    __m128 sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0x55), difference);
    sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0xaa), sum);
    sum = _mm_sqrt_ss(sum);
    _mm_store_ss(length, sum);
}

// @retail 0x279410
bool __cdecl function_279410(s_component_contact_pair const *pair, hkEntity *moving,
    hkEntity *opposing, real limit, real reduction, real approach)
{
    long slot = havok_entity_property_get(moving, 0x2001);
    real absolute_speed = havok_entity_property_real_get(moving, 0x2002);
    long opposite_slot = havok_entity_property_get(opposing, 0x2001);
    s_havok_component *part = havok_component_get(slot);
    byte *entity = (byte *)havok_object_get(part->object_index);
    byte *second_entity = NULL;
    if (opposite_slot != NONE && part)
        second_entity = (byte *)havok_object_get(part->object_index);

    havok_vector_length3(((hkRigidBody *)moving)->m_motion->m_linear_velocity, &absolute_speed);
    real speed = absolute_speed;
    if (((hkRigidBody *)opposing)->m_motion->kind_code() != 7)
    {
        real relative_speed;
        havok_vector_distance3(((hkRigidBody *)moving)->m_motion->m_linear_velocity,
            ((hkRigidBody *)opposing)->m_motion->m_linear_velocity, &relative_speed);
        if (absolute_speed > relative_speed)
            speed = relative_speed;
    }
    bool result;
    if (entity[0xaa] == 0)
    {
        byte *current = (byte *)havok_object_get(part->object_index);
        if (current[0x3dd] && second_entity && second_entity[0xaa] == 0)
            return false;
        if (!current[0x34b])
        {
            real projected = approach;
            if (projected > speed)
                projected = speed;
            result = projected > limit || speed > limit && !function_2792c0((s_2792c0_context *)pair);
        }
        else
            result = speed > limit;
    }
    else
        result = speed > limit;
    if (result)
        function_1cfd10(pair, part, speed - reduction);
    return result;
}

