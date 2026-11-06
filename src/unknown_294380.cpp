#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_default_placement.h"
#include "unknown_293070.h"
#include "normalize3d.h"
#include "vector_products.h"
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

PRIVATE __forceinline void function_294669(vector3f *arg_0, real arg_1,
    vector3f const *arg_2)
{
    arg_0->i += arg_1 * arg_2->i;
    arg_0->j += arg_2->j * arg_1;
    arg_0->k += arg_2->k * arg_1;
}
PRIVATE __forceinline void function_2949aa(s_294384 *arg_0, vector3f const *arg_1)
{
    if (dot3f(&arg_0->field_18.current, arg_1) < 0.0f)
        arg_0->field_18.current = *(vector3f *)g_468788;
    if (dot3f(&arg_0->field_18.previous, arg_1) < 0.0f)
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

__forceinline void object_get_forward(long object_index, vector3f *forward);

// score 446 (was 523): entry and registers now retail's (first difference +0x72, masked aligned 25 lines; fast check 465 at 3712 bytes, full build 446). Left: retail's
// dead store of zero to the speed vector's i (0x294552, overwritten at 0x294feb) is missing, 9 bytes, so the walk head gets
// a pad and every later line is two off; the walk's position and offset vector take each other's homes (position at 0x94,
// retail shares 0x20 with the post-walk vector); the character and shortest-distance slots are swapped (0x6c/0x74);
// x-operand forms at five products and the scaled centre (retail loads y and z first). The dead store stays gone in every
// plain-field form tried (compile-only, stores counted; vector3f_assign in the real build too): chain 7 of 8, vector3f_assign 7, vector3f_scale by zero 7, pointer
// or reference at either store 7, array index 7, memcpy at init/final copy 7, dead reads 7, 256
// extra locals 7, the later store through a cast/index/assign/memcpy 7 or fewer; a struct with unused extra members 7. Round 64 forms, real build (fast
// control 465, plain 446): temp vector for the two-arm copy 713 (3716 bytes); the walk with `while (next != NONE)` and the
// step inside 768 (3717 bytes, worse); the maximum as a ternary or if/else 591 (3709 bytes); object_get_forward 922
// (stays out of line, 3517 bytes); vector3f_assign for the zeros 706 (plain, 3719 bytes); offset or position
// declared at the walk's block or at function scope and the post-walk vector declared early, same code in all
// combinations (scopes don't move homes under LTCG); component-wise closest-point copy 743; the sum through vector3f_add
// 728; vector3f_add_scaled for the walk's terms 720/720/720; for every add 714 (3716 bytes); the reciprocal passed
// straight to vector3f_scale 463 fast (3716 bytes); scaling into a separate point 465 fast; a private pointer helper with
// either x operand order, same code; named y and z before the block 664; the sum copied first 741; the centre by named
// components 657; a chain of k identity pointers at the centre 714/721/731/466/449/720/463/465 (period 8, not kept).
// Lead, not kept (a struct that groups locals counts only with a match): the speed vector, the walk's offset and the value passed to 0x294050 as one
// three-member local struct bring the dead store and the position's shared home back together: plain 274 at 3717 bytes, masked 29, frame 0xc0
// (a struct over 16 bytes joins the request record's slot; retail keeps 0xdc). The store survives only in an aggregate over 16 bytes: speed + offset (24
// bytes) 294 fast, frame 0xc4; speed + that value (16 bytes) 733, store still gone. Offset + that value (16 bytes) 725 fast at 3716, frame 0xdc and the
// position's home right; one-member wrappers 465 (no effect); the speed vector or offset as real[3] 720/463; array + pair 478. So both puzzles are aggregate
// membership and size, not scopes or names: find the real record over 16 bytes that holds the speed vector and still leaves frame 0xdc.
// Earlier first line: score 523 (was 523): Frame 0xdc agrees; early ESI ownership and neighbor/steering homes differ.
// MiniMax B (11-04): no new form, no lower score.
// Statement walk against retail (f1B, 2026-11-05; every block from 0x294380 to the ret at 0x295201): no behaviour difference
// found. Entry (pool +0x44 with 40-byte records, definition +0x84 via the record's short at +2, object table +0x44, flock
// member = +0x13a when the +0x134 state is 1, parent at +0x14 tested against -1 then the member), the walk (head-tested
// iterator, self skipped by datum pointer, cos(+0x50) test on the normalised offset dot forward, +0x34/dist/zero tests,
// +0x40/+0x38/+0x44 terms into the +0x54 and +0x60 vectors, position sum, nearest else-arm with the 3.4e38 start), the
// 0x295600/0x293d30/0x294050/0x0b75a0/0x0b9fc0 calls (argument order and register/stack split read from the push sequence),
// the five 3.0 clamps (0x294814 and the four 0x294b56 inlines), the +0x5c/+0x60/+0x64 centre block (its scaled centre is not stored back in retail, dead in ours too), the
// +0x58/+0x34*0.5 blend, the waypoint block (+0x14/+0x18, 16-byte records, radius defaulting to 1.0), the maximum with
// local_18, the 0.7 (0x45dd1c) hysteresis on +0xd, the inlined forward getter and the 0x30-byte request copy to +0x13c all
// agree with the source, and the constants (0.5, 1.0, 3.0, 0.7, 3.4e38) read from the XBE agree. A multiset comparison of
// instruction text (registers kept) leaves only: the walk's two vector homes (0x94/0x20 swapped), the 0x6c/0x74 pair, x-operand
// order at five products, the missing dead store, and ESI/ECX choices; nothing is a logic, offset or condition difference.
// Earlier first line, after MiniMax B: score 523 (was 523): Frame 0xdc agrees; early ESI ownership and neighbor/steering homes differ.
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
// Round 65 (m3 lane 14, fast, control 465): the private copy of the forward getter replaced by the existing object_get_forward with a
// `__forceinline` declaration here 465 (same code, kept; plain 446); the speed vector as a local union with a real[3] 465 (vector3f is
// already a union); the three-member struct re-measured 293 (3718, frame 0xc0, store back, masked 39); with the request declared first 293;
// with the value read after the request's init 374 (3734, frame still 0xc0: the struct overlaps the request even then); with the request's
// +0xc copy after the +0x24 call 293.
// @retail 0x294380
void function_294380(long arg_0, long arg_1)
{
    s_294380 *local_0 = &((s_294380 *)g_51ecb4->data)[arg_0 & 0xffff];
    s_294382 *local_1 = &((s_294383 *)g_4e0350)->field_354[local_0->field_2];
    s_slot_object_view *local_2 = object_get(arg_1);
    s_294384 *local_3 = (s_294384 *)flock_object_get_member((s_flock_object *)local_2);
    if (local_2->parent_index != NONE || !local_3) return;
    point3f local_4;
    real local_5 = 0.0f;
    /* arg_1 reread from its stack slot: keeps arg_0 arriving in eax, as retail */
    function_b9dd0(arg_1, &local_4);
    vector3f local_6 = *g_4687a4;
    bool local_7 = !TEST_FIELD_BIT((*(dword *)(g_4e3b44[local_2->tag_index & 0xffff].bytes + 0xd4) >> 4) & 1);
    real local_8 = local_1->field_34 * 0.5f;
    point3f local_9 = *g_468788;
    real local_10 = 3.402823466e38f;
    point3f local_11;
    real local_12 = local_1->field_54;
    short local_15 = 0;
    vector3f local_13 = *g_4687a4;
    vector3f local_14 = *g_4687a4;
    real local_16 = 0.0f;
    real local_17 = 0.0f;
    real local_18 = 0.0f;
    vector3f local_19;
    vector3f_scale(&local_2->forward, local_1->field_3c, &local_19);
    local_6.i = 0.0f;
    local_6.j = 0.0f;
    local_6.k = 0.0f;
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
            vector3d_from_points3d(&local_4, &local_24, &local_25);
            real local_26 = function_30bf0(&local_25);
            if (dot3f(&local_25, &local_2->forward) > (real)cos(local_1->field_50))
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
        vector3f_add(&local_30, &local_19, &local_19);
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
                    vector3f_scale((vector3f *)&local_9, local_34, (vector3f *)&local_9);
                    vector3d_from_points3d(&local_4, &local_9, &local_30);
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
        vector3d_from_points3d(&local_4, &local_11, &local_30);
        function_30bf0(&local_30);
        function_294669(&local_19, local_1->field_5c, &local_30);
        local_12 = local_1->field_58;
    }
    else local_12 = local_1->field_54;
    if (local_3->field_4 >= 0 && local_3->field_4 < local_1->field_14)
    {
        s_294381 *local_36 = &local_1->field_18[local_3->field_4];
        vector3d_from_points3d(&local_4, &local_36->field_0, &local_30);
        real local_37 = function_30bf0(&local_30);
        if (local_37 > 0.0f)
        {
            real local_38 = 1.0f;
            if (local_36->field_c > 0.0f) local_38 = local_36->field_c;
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
        object_get_forward(arg_1, &local_19);
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
