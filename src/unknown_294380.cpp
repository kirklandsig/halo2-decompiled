#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_default_placement.h"
#include "unknown_293070.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

struct s_scenario_flock;
struct s_294050;
struct s_294051;
struct s_295600_state
{
    short duration;
    short elapsed;
    vector3f current;
    vector3f previous;
};
struct s_orientation_request_118f90
{
    long name0;
    long name4;
    bool enabled;
    byte unknown09[2];
    bool force;
    byte unknown0c[0xc];
    vector3f forward;
    vector3f up;
};
void function_118f90(s_orientation_request_118f90 *arg_0);
void function_118fe0(long arg_0, bool arg_1);
void function_b9fc0(long arg_0, vector3f *arg_1, vector3f *arg_2);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
real function_30bf0(vector3f *arg_0);
vector3f *function_11d090(vector3f const *arg_0, vector3f *arg_1);
void function_293d30(s_scenario_flock const *arg_0, point3f *arg_1,
    real *arg_2, vector3f *arg_3, bool arg_4, bool *arg_5);
void function_294050(s_294050 const *arg_0, s_294051 const *arg_1,
    point3f const *arg_2, vector3f *arg_3, real *arg_4, real *arg_5);
void function_295600(s_295600_state *arg_0, bool arg_1, real arg_2,
    real arg_3, real arg_4, vector3f *arg_5);
extern s_record_pool *g_51ecb4;

struct s_294380
{
    short field_0;
    short field_2;
    long field_4;
    byte field_8[0x20];
};
struct s_294381
{
    point3f field_0;
    real field_c;
};
struct s_294382
{
    byte field_0[0x14];
    long field_14;
    s_294381 *field_18;
    byte field_1c[0x18];
    real field_34, field_38, field_3c, field_40, field_44, field_48;
    real field_4c, field_50, field_54, field_58, field_5c, field_60;
    real field_64, field_68, field_6c, field_70, field_74, field_78, field_7c;
    long field_80;
};
struct s_294383
{
    byte field_0[0x354];
    s_294382 *field_354;
};
struct s_294384
{
    long field_0;
    short field_4;
    byte field_6[2];
    long field_8;
    bool field_c;
    bool field_d;
    byte field_e[0xa];
    s_295600_state field_18;
};

PRIVATE __forceinline s_294384 *function_2943dd(s_slot_object_view *arg_0)
{
    return *(long *)((byte *)arg_0 + 0x134) == 1
        ? (s_294384 *)((byte *)arg_0 + *(short *)((byte *)arg_0 + 0x13a)) : NULL;
}
PRIVATE __forceinline void function_2945bd(point3f const *arg_0,
    point3f const *arg_1, vector3f *arg_2)
{
    arg_2->i = arg_1->x - arg_0->x;
    arg_2->j = arg_1->y - arg_0->y;
    arg_2->k = arg_1->z - arg_0->z;
}
PRIVATE __forceinline void function_294669(vector3f *arg_0, real arg_1,
    vector3f const *arg_2)
{
    arg_0->i += arg_1 * arg_2->i;
    arg_0->j += arg_2->j * arg_1;
    arg_0->k += arg_2->k * arg_1;
}
PRIVATE __forceinline real function_294988(vector3f const *arg_0,
    vector3f const *arg_1)
{
    return arg_0->k * arg_1->k + arg_0->j * arg_1->j + arg_0->i * arg_1->i;
}
PRIVATE __forceinline void function_2949aa(s_294384 *arg_0, vector3f const *arg_1)
{
    if (function_294988(&arg_0->field_18.current, arg_1) < 0.0f)
        arg_0->field_18.current = *(vector3f *)g_468788;
    if (function_294988(&arg_0->field_18.previous, arg_1) < 0.0f)
        arg_0->field_18.previous = *(vector3f *)g_468788;
}
PRIVATE __forceinline void function_294b56(vector3f *arg_0, vector3f *arg_1,
    real *arg_2, real *arg_3)
{
    if (*arg_2 + *arg_3 > 3.0f) *arg_3 = 3.0f - *arg_2;
    function_30bf0(arg_1);
    function_294669(arg_0, *arg_3, arg_1);
    *arg_2 += *arg_3;
}

PRIVATE __forceinline void function_2950bd(transform4x3f const *arg_0,
    vector3f const *arg_1, vector3f *arg_2)
{
    real local_0 = arg_1->i;
    real local_1 = arg_1->j;
    real local_2 = arg_1->k;
    arg_2->i = arg_0->up.i * local_2 + arg_0->left.i * local_1 + arg_0->forward.i * local_0;
    arg_2->j = arg_0->up.j * local_2 + arg_0->left.j * local_1 + arg_0->forward.j * local_0;
    arg_2->k = arg_0->up.k * local_2 + arg_0->left.k * local_1 + arg_0->forward.k * local_0;
}
PRIVATE __forceinline void function_295027(long arg_0, vector3f *arg_1)
{
    s_slot_object_view *local_0 = object_get(arg_0);
    if (local_0->parent_index == NONE)
        *arg_1 = local_0->forward;
    else
        function_2950bd(object_node_matrix(object_get(local_0->parent_index), local_0->parent_node),
            &local_0->forward, arg_1);
}

// score 523 (was 523): Frame 0xdc agrees; early ESI ownership and neighbor/steering homes differ.
// Earlier score 523 (was 581): frame 0xdc agrees; since round 67 the neighbour walk is the shared inline member
// step (retail's head-tested loop) and 0x118fe0 stays out of line as in retail. Left: retail keeps the
// walk's position at 0x20 sharing its slot with the post-walk vector (ours shares the offset vector,
// position at 0x94), and pushes ESI only after the early return (ours uses ESI for the definition-table
// temporary). Round 67: the walk through the existing out-of-line step 837 (call kept, frame 0xe4);
// 0x118fe0 noinline alone 792 (3718 bytes); the inline step alone 794; both 510 with a private step copy,
// 523 with the shared one (kept); on that: the second index read plainly 676 (entry, ESI push and frame
// exactly as retail, but registers/slots after it move); the scaled forward through vector3f_scale 728;
// the forward scale as a named local 642; the walk offset declared before the position 523; the offset
// declared at the walk's enclosing block 523.
// Tried: the flock-state getter as if/else 731 (fast); without the stack read of the second index 679
// (fast; the first index then arrives in ECX); 0x118fe0 noinline with the arms swapped 792 (3718 bytes);
// the scaled forward through vector3f_scale 742 (fast), with noinline 726; the walk as a for loop 1022;
// names for the walk's two vectors 734 (fast, no change); the drop term or the dot helper's i term
// with its operands swapped 581 each.
// Round 448: Complete point/vector home roles and early uninitialized steering workspace,
// all seven nonempty combinations, then alternate complete home ranking 521/521/521/521/521/521/521/521.
// Reduced preview is 521/3707 versus full 523/3706; every trial ties that preview.
// Original body and helpers restored; Frame 0xdc agrees; early ESI ownership and neighbor/steering homes remain.
// @retail 0x294380
void function_294380(long arg_0, long arg_1)
{
    s_294380 *local_0 = &((s_294380 *)g_51ecb4->data)[arg_0 & 0xffff];
    s_294382 *local_1 = &((s_294383 *)g_4e0350)->field_354[local_0->field_2];
    s_slot_object_view *local_2 = object_get(arg_1);
    s_294384 *local_3 = function_2943dd(local_2);
    if (local_2->parent_index != NONE || !local_3) return;
    point3f local_4;
    real local_5 = 0.0f;
    /* arg_1 reread from its stack slot: keeps arg_0 arriving in eax, as retail */
    function_b9dd0(*(long const volatile *)&arg_1, &local_4);
    vector3f local_6 = *g_4687a4;
    bool local_7 = !TEST_FIELD_BIT((*(dword *)(g_4e3b44[local_2->tag_index & 0xffff].bytes + 0xd4) >> 4) & 1);
    real local_8 = local_1->field_34 * 0.5f;
    point3f local_9 = *g_468788;
    real local_10 = 3.402823466e38f;
    point3f local_11;
    real local_12 = local_1->field_54;
    vector3f local_13 = *g_4687a4;
    vector3f local_14 = *g_4687a4;
    short local_15 = 0;
    real local_16 = 0.0f;
    real local_17 = 0.0f;
    real local_18 = 0.0f;
    vector3f local_19;
    local_19.i = local_2->forward.i * local_1->field_3c;
    local_19.j = local_2->forward.j * local_1->field_3c;
    local_19.k = local_2->forward.k * local_1->field_3c;
    local_6.i = local_6.j = local_6.k = 0.0f;
    if (local_1->field_34 > 0.0f)
    {
        s_flock_member_iterator local_20;
        local_20.next_object_index = local_0->field_4;
        local_20.object_index = NONE;
        s_flock_object *local_21;
        while ((local_21 = function_2937a1(&local_20)) != NULL)
        {
            s_slot_object_view *local_22 = (s_slot_object_view *)local_21;
            if (local_22 == local_2) continue;
            point3f local_24;
            function_b9dd0(local_20.object_index, &local_24);
            vector3f local_25;
            function_2945bd(&local_4, &local_24, &local_25);
            real local_26 = function_30bf0(&local_25);
            if (function_294988(&local_25, &local_2->forward) > (real)cos(local_1->field_50))
            {
                if (local_1->field_34 > local_26 && local_26 > 0.0f)
                {
                    real local_27 = 1.0f / (local_26 * local_26);
                    real local_28 = local_1->field_40 * local_27;
                    function_294669(&local_13, local_28, &local_22->forward);
                    local_16 += local_28;
                    if (local_1->field_38 * local_1->field_38 > local_26 * local_26)
                    {
                        real local_29 = (local_27 - 1.0f / (local_1->field_38 * local_1->field_38)) * local_1->field_44;
                        function_294669(&local_14, -local_29, &local_25);
                        local_17 += local_29;
                    }
                    local_9.x += local_24.x;
                    local_9.y += local_24.y;
                    local_9.z += local_24.z;
                    local_15++;
                }
                else if (local_10 > local_26)
                {
                    local_10 = local_26;
                    local_11 = local_24;
                }
            }
        }
    }
    vector3f local_30;
    real local_31;
    function_295600(&local_3->field_18, local_7, local_1->field_74,
        local_1->field_78, local_1->field_7c, &local_30);
    if (local_7)
    {
        local_31 = function_30bf0(&local_30);
        if (local_31 > 0.0f)
        {
            if (local_31 > 3.0f) local_31 = 3.0f;
            function_30bf0(&local_30);
            function_294669(&local_19, local_31, &local_30);
            local_5 = local_31;
        }
    }
    else
    {
        local_19.i += local_30.i;
        local_19.j += local_30.j;
        local_19.k += local_30.k;
    }
    bool local_32 = false;
    function_293d30((s_scenario_flock *)local_1, &local_4, &local_31, &local_30, local_7, &local_32);
    function_294669(&local_19, local_31, &local_30);
    local_5 += local_31;
    if (local_32) function_b75a0(arg_1, &local_4, NULL, NULL, NULL, false);
    if (local_31 > 0.0f) function_2949aa(local_3, &local_30);
    else if (!local_7) local_19.k -= local_2->forward.k * local_1->field_48;
    local_31 = 0.0f;
    function_294050((s_294050 *)local_0, (s_294051 *)local_1,
        &local_4, &local_30, &local_31, &local_18);
    if (local_31 > 0.0f)
    {
        function_294669(&local_19, local_31, &local_30);
        local_5 += local_31;
        function_2949aa(local_3, &local_30);
    }
    if (local_15 > 0)
    {
        if (local_5 < 3.0f)
        {
            function_294b56(&local_19, &local_14, &local_5, &local_17);
            if (local_5 < 3.0f)
            {
                function_294b56(&local_19, &local_13, &local_5, &local_16);
                if (local_5 < 3.0f)
                {
                    real local_33 = 3.0f - local_5;
                    local_16 = local_1->field_5c * local_33;
                    real local_34 = 1.0f / local_15;
                    local_30.i = local_9.x * local_34 - local_4.x;
                    local_30.j = local_9.y * local_34 - local_4.y;
                    local_30.k = local_9.z * local_34 - local_4.z;
                    real local_35 = function_30bf0(&local_30);
                    if (local_35 > local_1->field_60)
                    {
                        if (local_1->field_64 > local_35)
                            local_16 *= (local_35 - local_1->field_60) / (local_1->field_64 - local_1->field_60);
                        function_294b56(&local_19, &local_30, &local_5, &local_16);
                    }
                    if (local_35 > local_1->field_34) local_12 = local_1->field_58;
                    else if (local_35 > local_8)
                        local_12 += (local_1->field_58 - local_12) * (local_35 - local_8) / (local_1->field_34 - local_8);
                }
            }
        }
    }
    else if (3.402823466e38f > local_10)
    {
        function_2945bd(&local_4, &local_11, &local_30);
        function_30bf0(&local_30);
        function_294669(&local_19, local_1->field_5c, &local_30);
        local_12 = local_1->field_58;
    }
    else local_12 = local_1->field_54;
    if (local_3->field_4 >= 0 && local_3->field_4 < local_1->field_14)
    {
        s_294381 *local_36 = &local_1->field_18[local_3->field_4];
        function_2945bd(&local_4, &local_36->field_0, &local_30);
        real local_37 = function_30bf0(&local_30);
        if (local_37 > 0.0f)
        {
            real local_38 = local_36->field_c > 0.0f ? local_36->field_c : 1.0f;
            if (local_38 > local_37) local_3->field_c = true;
            else
            {
                local_31 = local_1->field_4c;
                if (local_5 < 3.0f) function_294b56(&local_19, &local_30, &local_5, &local_31);
            }
        }
    }
    if (!(local_12 > local_18)) local_12 = local_18;
    local_6.i = local_12;
    if (local_7) { local_6.k = 0.0f; local_19.k = 0.0f; }
    if (local_3->field_d ? local_5 >= local_1->field_68 * 0.7f : local_5 >= local_1->field_68)
    {
        if (function_30bf0(&local_19) == 0.0f) function_b9fc0(arg_1, &local_19, NULL);
        local_3->field_d = true;
    }
    else
    {
        local_6 = *g_4687a4;
        function_295027(arg_1, &local_19);
        local_3->field_d = false;
    }
    s_orientation_request_118f90 local_39;
    function_118f90(&local_39);
    local_39.forward = local_19;
    *(vector3f *)local_39.unknown0c = local_6;
    function_11d090(&local_19, &local_39.up);
    function_118fe0(arg_1, true);
    *(s_orientation_request_118f90 *)((byte *)object_get(arg_1) + 0x13c) = local_39;
}
