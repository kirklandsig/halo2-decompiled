// @flags /O2 /Gr /arch:SSE
// The speed math sits in the two inline helpers below, not in the body, because of the whole-program pass: a function
// that itself declares a 16-aligned local (`__m128`) is marked before inlining, and the functions it calls then stop
// taking arguments in ebx; the mark passes on through a callee built with /Ob1 to that callee's own callees. With the
// `__m128` locals written in this body, 0x1cfd10 (called here, an /Ob1 file) passed the mark on to 0x182b90, whose
// fourth argument moved from ebx to the stack, and matched 0x1cfd10 and 0x277ec0 were lost (the earlier BLOCKED-BY
// 0x182b90). A helper's locals join this frame only after inlining: the frame is aligned to 16 as in retail and
// 0x182b90 keeps its ebx argument. The helpers take the result's address because a returned value changes the body's
// stores.
//
// Three more things in retail's bytes decide the body:
// - The first parameter is not a register candidate. Retail loads it from [ebp+8] for each of its two calls, and every
//   register in the function is the one the allocator picks when that parameter has no say. A plain pointer prefers
//   esi and edi (where the two callees take it) and keeps the other values off esi: the same instructions, score 175.
//   A one-member record passed by value is such a parameter; the caller 0x2daae0 passes one and stays matched.
// - The moving entity's 0x2002 property is stored (+0x55, +0xd7) and never loaded: the cell is next written through
//   the length helper's pointer. The compiler drops the stores of a value nothing reads, so the source reads it, and
//   the read left no code. An argument for a parameter the callee does not read does that: the whole-program pass
//   drops the argument and the stores stay. It is 0x1cfd10's fourth argument here. As a second argument of 0x2792c0
//   it gives the same bytes, so this function does not say which callee takes it. With the property in a variable of
//   its own the absolute speed has three references and `speed` four, and the frame gets retail's order: `speed` at
//   [esp+0x14], the property and the absolute speed in one cell at [esp+0x18], the relative speed at [esp+0x1c]. (As
//   the absolute speed's initial value the property's stores stay as well, but that variable then has five references
//   and is placed below `speed`.)
// - The tail's block order is that of a flag (`projects`) set in the first test and tested once.
#include "unknown_1cec30.h"

struct s_2792c0_context;
struct s_component_contact_pair;
bool function_2792c0(s_2792c0_context *);
void function_1cfd10(s_component_contact_pair const *, s_havok_component *, real, long);

/* the length of a vector's first three components */
static __forceinline void havok_vector_length3(hkVector4 const &vector, real *length)
{
    __m128 squared = _mm_mul_ps(vector.m_quad, vector.m_quad);
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

/* the contact as the Havok callback 0x2daae0 hands it on: its pointer in a record, passed by value */
struct s_component_contact_reference
{
    s_component_contact_pair const *pair;
};

// @retail 0x279410
bool __cdecl function_279410(s_component_contact_reference contact, hkEntity *moving,
    hkEntity *opposing, real limit, real reduction, real approach)
{
    long slot = havok_entity_property_get(moving, HAVOK_PROPERTY_COMPONENT_INDEX);
    long property_2002 = havok_entity_property_get(moving, HAVOK_PROPERTY_2002); /* only handed on, see above */
    long opposite_slot = havok_entity_property_get(opposing, HAVOK_PROPERTY_COMPONENT_INDEX);
    s_havok_component *part = havok_component_get(slot);
    byte *entity = (byte *)havok_object_get(part->object_index);
    s_havok_component *opposite_part = NULL;
    if (opposite_slot != NONE)
        opposite_part = havok_component_get(slot); /* the moving entity's slot, as retail has it */
    byte *second_entity;
    if (opposite_part)
        second_entity = (byte *)havok_object_get(opposite_part->object_index);
    else
        second_entity = NULL;

    real absolute_speed;
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
    bool projects = false;
    if (entity[0xaa] == 0)
    {
        byte *current = (byte *)havok_object_get(part->object_index);
        if (current[0x3dd] && second_entity && second_entity[0xaa] == 0)
            return false;
        if (!current[0x34b])
            projects = true;
    }
    if (projects)
    {
        real projected = approach;
        if (projected > speed)
            projected = speed;
        if (projected > limit)
            result = true;
        else
        {
            result = false;
            if (speed > limit && !function_2792c0((s_2792c0_context *)contact.pair))
                result = true;
        }
    }
    else if (speed > limit)
        result = true;
    else
        return false;
    if (result)
        function_1cfd10(contact.pair, part, speed - reduction, property_2002);
    return result;
}

