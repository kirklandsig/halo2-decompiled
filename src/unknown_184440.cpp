#include "unknown_11c920.h"
#include "globals.h"
#include "effects.h"
#include "sound_sources.h"
#include <float.h>
#include <math.h>
// @flags /O2 /arch:SSE /Gr

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
bool g_46dd50 = true;
struct s_184440
{
    short field_0;
    word field_2;
    byte field_4;
    byte field_5;
    short field_6;
};
struct s_184441
{
    word field_0[2];
    word field_4[2];
    short field_8[2];
};
struct s_184442
{
    byte field_0[0xc];
    plane3f *field_c;
    byte field_10[0x28 - 0x10];
    long field_28;
    s_184440 *field_2c;
    long field_30;
    s_184441 *field_34;
    long field_38;
    byte *field_3c;
};
struct s_184443
{
    transform4x3f field_0;
    short field_34;
    byte field_36[0x58 - 0x36];
};
struct s_184444
{
    byte field_0[0x70];
    s_184442 field_70;
    byte field_b0[0xc8 - 0xb0];
};
struct s_184445
{
    byte field_0[8];
    short field_8;
    byte field_a[0x14 - 0xa];
};
struct s_184446
{
    byte field_0[0x10];
    s_184445 *field_10;
    byte field_14[0x13c - 0x14];
    s_184444 *field_13c;
    long field_140;
    s_184443 *field_144;
};
struct s_184447
{
    byte field_0[0x34];
    long field_34;
    byte field_38[0xb4 - 0x38];
};
struct s_184448
{
    byte field_0[0x150];
    long field_150;
    s_184447 *field_154;
};
struct s_184449
{
    byte field_0[0x10];
    long field_10;
    long field_14;
    s_effect_particle_system_definition *field_18;
    real field_1c;
};
struct s_18444a
{
    long field_0;
    byte field_4[0x1c - 4];
    s_location field_1c;
    byte field_24[0x30 - 0x24];
    point3f field_30;
    vector3f field_3c;
};
struct s_18444b
{
    real field_0;
    real field_4;
    real field_8;
    real field_c;
    real field_10;
    real field_14;
};
long function_48e70(real arg_0);
long function_173fd0(s_effect_particle_system_definition *arg_0, long arg_1, long arg_2, short arg_3, long arg_4);
void __stdcall function_175430(s_particle_system_datum *arg_0, point3f const *arg_1, vector3f const *arg_2);
dword vector3d_compress(vector3f const *arg_0);
long function_1895f0(s_sound_position const *arg_0, real arg_1, long arg_2);
bool function_23a220(point2f const *arg_0, short volatile arg_1, point2f const *arg_2, real arg_3);

__forceinline real function_184441(vector3f const *arg_0, point3f const *arg_1)
{
    vector3f const *local_0 = arg_0;
    point3f const *local_1 = arg_1;
    return local_0->k * local_1->z + local_0->j * local_1->y + local_0->i * local_1->x;
}
__forceinline real function_184442(vector3f *arg_0)
{
    real local_0 = (real)sqrt((double)arg_0->k * arg_0->k + (double)arg_0->j * arg_0->j + (double)arg_0->i * arg_0->i);
    if (fabs(local_0) < 0.0001f)
        return 0.0f;
    real local_1 = 1.0f / local_0;
    arg_0->i = local_1 * arg_0->i;
    arg_0->j = local_1 * arg_0->j;
    arg_0->k = local_1 * arg_0->k;
    return local_0;
}
__forceinline real function_184443(real arg_0, real arg_1, real arg_2)
{
    return arg_0 < arg_1 ? arg_1 : (arg_0 > arg_2 ? arg_2 : arg_0);
}
__forceinline long function_184444(real arg_0)
{
    __asm
    {
        movss xmm0, arg_0
        cvttss2si eax, xmm0
        cvtsi2ss xmm1, eax
        cmpneqss xmm1, xmm0
        cmpltss xmm0, g_45dbd8
        andps xmm0, xmm1
        movmskps ecx, xmm0
        sub eax, ecx
    }
}
__forceinline void function_184445(transform4x3f const *arg_0, point3f *arg_1)
{
    point3f local_0 = *arg_1;
    if (arg_0->scale != 1.0f)
    {
        local_0.x *= arg_0->scale;
        local_0.y *= arg_0->scale;
        local_0.z *= arg_0->scale;
    }
    arg_1->x = arg_0->up.i * local_0.z + arg_0->left.i * local_0.y + arg_0->forward.i * local_0.x + arg_0->position.x;
    arg_1->y = arg_0->up.j * local_0.z + arg_0->left.j * local_0.y + arg_0->forward.j * local_0.x + arg_0->position.y;
    arg_1->z = arg_0->up.k * local_0.z + arg_0->left.k * local_0.y + arg_0->forward.k * local_0.x + arg_0->position.z;
}
__forceinline void function_184446(transform4x3f const *arg_0, plane3f *arg_1)
{
    vector3f local_0 = arg_1->n;
    arg_1->i = arg_0->forward.i * local_0.i + arg_0->up.i * local_0.k + arg_0->left.i * local_0.j;
    arg_1->j = arg_0->forward.j * local_0.i + arg_0->up.j * local_0.k + arg_0->left.j * local_0.j;
    arg_1->k = arg_0->left.k * local_0.j + arg_0->forward.k * local_0.i + arg_0->up.k * local_0.k;
    arg_1->d = arg_0->position.z * arg_1->k + arg_0->position.y * arg_1->j + arg_0->scale * arg_1->d + arg_0->position.x * arg_1->i;
}
__forceinline real function_184447(void)
{
    return (real)random_next(&g_4e7408->seed) * 1.5259021893143654e-05f * 1.5f - 0.75f;
}

extern c_type_4e7709 g_479868;
extern c_type_4e7709 g_479874;

__forceinline bool function_184448(long arg_0)
{
    dword local_0 = *(dword *)((byte *)g_4e3b44 + (short)arg_0 * 0x10);
    if (local_0 != 0x5052544d && local_0 != 0x70727433)
        return false;
    c_type_4e7709 &local_1 = local_0 == 0x5052544d ? g_479874 : g_479868;
    local_1.initialize(arg_0);
    return true;
}

#pragma inline_depth(1)
// score 965 (was 965): Frame 0x11d4 versus 0x11cc and scene/surface geometry homes remain.
// Round 419: scene field observations 983; effect record reference 977; scene field observations/effect
// record reference 983; surface record reference 977; scene field observations/surface record reference
// 983; effect record reference/surface record reference 977; scene field observations/effect record
// reference/surface record reference 983; scene field observations/effect record reference/surface record
// reference/material record reference 989. Frame 0x11d4 versus 0x11cc and scene/surface geometry homes
// remain. Original body restored.
// frame 0x11d4 versus 0x11cc; initial surface and
// geometry/transform homes differ. Complete scalar normalization capture
// scored 989; common length result 999 recovers 0x11cc. Native copies
// scored 1097/1070. Full-width low endpoints 976 (one-point gain rejected);
// high/both 1009; row combinations 1020/1004/1035; selectors 1038/1017.
// Scalar-capture/endpoint combinations scored 992/1005/979/1014.
// Existing in-place point/plane transforms with complete scalar input capture: point 1045,
// plane 1024, both 1093; point scalar capture/native publication 984; plane 979; both 989;
// point staged/plane direct 1027, point direct/plane staged 1028. All original helpers restored;
// frame excess and initial surface/geometry homes remain.
// Plane scalar capture alone recovers frame 0x11cc at 1024 (4538 bytes, first +0x1d);
// staged point publication with that capture 1027. Both remain rejected.
// Round 315: Existing projection-point setter/sample displacement additions/velocity additions in
// seven subsets 989/976/983/990/1001/985/1001; full graph with sound-origin setter 996. The one-count
// gain is rejected. Original helpers, snapshots, arithmetic trees and complete 4590-byte body
// restored; frame 0x11d4 versus 0x11cc remains.
// Round 447: Native short worklist count/cursor and existing shared group lookup, seven subsets; shared lookup with full-width edge pair
// 982/981/982/990/981/984/981/990. Frame 0x11d4 versus 0x11cc and initial surface/geometry homes remain.
// Original executable source and all trial helper/interface changes restored.
// Round 245: complete double product/squared-sum/square-root intermediate ownership,
// seven subsets 989/977/989/989/977/989/977; full graph with single result 981.
// Original two-use normalizer and complete body restored; frame excess remains.
// Round 464: projected bounds initialized after both projection origins 965 (kept, plain);
// per-face 3D/2D/both polygon arrays 977/977/977; each with late projected bounds 965;
// all with world bounds declared at initialization 965. Keep only the later projected
// bounds initialization. Frame 0x11d4 versus 0x11cc and initial geometry homes remain.
// Round 144 (lane 4, fast): retail keeps the transform pointer only in its stack home (+0x98) and the surface
// list in ECX, loads the block count into a register before the index compare (as the compile-only build
// does) and initializes the projected bounds at the top of each face iteration. Defaults then `if (arg_0 !=
// NONE)` override 978 (4573 bytes); the bounds lookup as one `&&` test 962 (three-point gain rejected); the
// non-NONE arm first 986; NULL stored before the default list 965; a short surface group 967 (4573 bytes).
// Round 13 (lane 17): the plane transform reading the three normal components into scalars (no vector copy) 978 at
// 4557 bytes (frame 0x11cc as retail; the transform's products and adds then follow retail's sequence, one spill
// slot apart); the per-face bounds initialized at the top of the face loop (retail stores them there) 977; both 1024
// (4538 bytes); the existing normalize3d for both normalizations 1002 (4542 bytes); the two vector copies before
// the plane-distance dots dropped (the dots on the originals) 1062 (4477 bytes). Original helpers and body restored.
// Round 484: native packed plane index/orientation and Boolean edge-side, subsets
// index/flag/side/index+flag/index+side/flag+side/all/signed flag
// 965/982/981/982/981/982/982/964. Last form 4602 bytes (retail 4590), one-point gain
// rejected. Eight-byte record asserted in each typed form;
// frame excess remains. Original surface declaration and complete body restored.
// @retail 0x184440
void __stdcall function_184440(long arg_0, long arg_1, s_18444a const *arg_2, long arg_3)
{
    long local_0[1024];
    point3f local_1[8];
    point2f local_2[8];
    real_bounds local_3[3];
    if (!g_46dd50)
        return;
    s_184446 *local_4 = (s_184446 *)g_4e0348;
    s_184443 const *local_5;
    s_184442 *local_6;
    if (arg_0 == NONE)
    {
        local_6 = (s_184442 *)g_4e0340;
        local_5 = NULL;
    }
    else
    {
        local_5 = &local_4->field_144[arg_0];
        local_6 = &local_4->field_13c[local_5->field_34].field_70;
    }
    long local_7 = local_6->field_2c[arg_3].field_6;
    short local_8 = local_4->field_10[local_7].field_8;
    s_184447 *local_9 = NULL;
    if (local_8 != NONE && local_8 >= 0)
    {
        s_184448 *local_10 = (s_184448 *)g_4e034c;
        if (local_8 < local_10->field_150)
            local_9 = &local_10->field_154[local_8];
    }
    if (local_9->field_34 == NONE)
        return;
    s_184449 *local_11 = (s_184449 *)g_4e3b44[local_9->field_34 & 0xffff].bytes;
    for (long local_12 = 0; local_12 < 3; local_12++)
    {
        local_3[local_12].lo = FLT_MAX;
        local_3[local_12].hi = -FLT_MAX;
    }
    long local_13 = 1;
    local_0[0] = arg_3;
    for (long local_14 = 0; (short)local_14 < (short)local_13; )
    {
        long local_15 = local_0[(short)local_14++];
        s_184440 *local_16 = &local_6->field_2c[local_15];
        plane3f const *local_54 = &local_6->field_c[local_16->field_0 & 0x7fff];
        plane3f local_17;
        if (local_16->field_0 & 0x8000)
        {
            local_17.i = 0.0f - local_54->i;
            local_17.j = 0.0f - local_54->j;
            local_17.k = 0.0f - local_54->k;
            local_17.d = 0.0f - local_54->d;
        }
        else
            local_17 = *local_54;
        if (local_5)
            function_184446(&local_5->field_0, &local_17);
        real local_18 = (real)fabs(local_17.i);
        real local_19 = (real)fabs(local_17.j);
        real local_20 = (real)fabs(local_17.k);
        short local_21 = local_20 >= local_19 && local_20 >= local_18 ? 2 : (local_19 >= local_18 ? 1 : 0);
        short local_22 = local_21 * 2 + (local_17.n.n[local_21] > 0.0f);
        short local_23 = g_440b94[local_22][0];
        short local_24 = g_440b94[local_22][1];
        word local_25 = local_16->field_2;
        word local_26 = local_25;
        short local_27 = 0;
        do
        {
            s_184441 const *local_28 = &local_6->field_34[local_26];
            long local_29 = local_28->field_8[1] == local_15;
            local_1[local_27] = *(point3f *)(local_6->field_3c + local_28->field_0[local_29] * 0x10);
            if (local_5)
                function_184445(&local_5->field_0, &local_1[local_27]);
            local_2[local_27].x = local_1[local_27].n[local_23];
            local_2[local_27].y = local_1[local_27].n[local_24];
            local_27++;
            long local_30 = local_28->field_8[!local_29];
            long local_31 = 0;
            while (local_31 < (short)local_13 && local_0[local_31] != local_30)
                local_31++;
            if (local_31 == (short)local_13 && local_30 != NONE &&
                local_6->field_2c[local_30].field_5 == arg_1 && local_6->field_2c[local_30].field_6 == local_7)
                local_0[(short)local_13++] = local_30;
            local_26 = local_28->field_4[local_29];
        } while (local_26 != local_25);
        point3f local_28;
        if (local_15 == arg_3)
        {
            real local_29 = 0.0f - (arg_2->field_30.z * local_17.k + arg_2->field_30.y * local_17.j + local_17.i * arg_2->field_30.x - local_17.d);
            local_28.x = local_29 * local_17.i + arg_2->field_30.x;
            local_28.y = local_29 * local_17.j + arg_2->field_30.y;
            local_28.z = local_29 * local_17.k + arg_2->field_30.z;
        }
        else
            local_28 = local_1[0];
        vector3f local_29;
        vector3d_from_points3d(&local_1[0], &local_1[1], &local_29);
        function_184442(&local_29);
        vector3f local_30;
        local_30.i = local_29.k * local_17.j - local_29.j * local_17.k;
        local_30.j = local_17.k * local_29.i - local_29.k * local_17.i;
        local_30.k = local_29.j * local_17.i - local_17.j * local_29.i;
        vector3f local_55 = local_29;
        real local_31 = function_184441(&local_55, &local_28);
        vector3f local_56 = local_30;
        real local_32 = function_184441(&local_56, &local_28);
        real_bounds local_33[2] = { { FLT_MAX, -FLT_MAX }, { FLT_MAX, -FLT_MAX } };
        for (short local_34 = 0; local_34 < local_27; local_34++)
        {
            real local_35 = (local_55.k * local_1[local_34].z + local_55.j * local_1[local_34].y + local_1[local_34].x * local_55.i) - local_31;
            real local_36 = (local_56.k * local_1[local_34].z + local_56.j * local_1[local_34].y + local_1[local_34].x * local_56.i) - local_32;
            if (local_33[0].lo > local_35) local_33[0].lo = local_35;
            if (local_35 > local_33[0].hi) local_33[0].hi = local_35;
            if (local_33[1].lo > local_36) local_33[1].lo = local_36;
            if (local_36 > local_33[1].hi) local_33[1].hi = local_36;
            if (local_3[0].lo > local_1[local_34].x) local_3[0].lo = local_1[local_34].x;
            if (local_1[local_34].x > local_3[0].hi) local_3[0].hi = local_1[local_34].x;
            if (local_3[1].lo > local_1[local_34].y) local_3[1].lo = local_1[local_34].y;
            if (local_1[local_34].y > local_3[1].hi) local_3[1].hi = local_1[local_34].y;
            if (local_3[2].lo > local_1[local_34].z) local_3[2].lo = local_1[local_34].z;
            if (local_1[local_34].z > local_3[2].hi) local_3[2].hi = local_1[local_34].z;
        }
        for (long local_34 = 0; local_34 < local_11->field_14; local_34++)
        {
            s_effect_particle_system_definition *local_35 = &local_11->field_18[local_34];
            if (local_35->tag_index == NONE || !function_184448(local_35->tag_index) || local_35->unknown30 <= 0)
                continue;
            long local_36 = function_173fd0(local_35, NONE, local_9->field_34, (short)local_34, NONE);
            if (local_36 == NONE)
                continue;
            s_particle_system_datum *local_37 = (s_particle_system_datum *)g_510c74->data + (local_36 & 0xffff);
            real local_38 = local_11->field_1c;
            if (fabs(local_38) < 0.0001f)
                local_38 = 0.25f;
            local_37->set_location(&arg_2->field_1c);
            short local_39, local_40, local_41, local_42;
            if (local_38 != 0.0f)
            {
                real local_43 = 1.0f / local_38;
                local_39 = (short)function_48e70(function_184443(local_43 * local_33[0].lo, -1000.0f, 1000.0f));
                local_40 = (short)function_48e70(function_184443(local_43 * local_33[1].lo, -1000.0f, 1000.0f));
                local_41 = (short)function_184444(function_184443(local_43 * local_33[0].hi, -1000.0f, 1000.0f));
                local_42 = (short)function_184444(function_184443(local_43 * local_33[1].hi, -1000.0f, 1000.0f));
            }
            else
            {
                if (local_15 != arg_3)
                    continue;
                local_39 = local_40 = local_41 = local_42 = 0;
            }
            for (short local_43 = local_40; local_43 <= local_42; local_43++)
            {
                for (short local_44 = local_39; local_44 <= local_41; local_44++)
                {
                    point3f local_45 = local_28;
                    real local_46 = ((real)local_44 + function_184447()) * local_38;
                    real local_47 = ((real)local_43 + function_184447()) * local_38;
                    local_45.x += local_29.i * local_46;
                    local_45.y += local_29.j * local_46;
                    local_45.z += local_29.k * local_46;
                    local_45.x += local_30.i * local_47;
                    local_45.y += local_30.j * local_47;
                    local_45.z += local_30.k * local_47;
                    point2f local_48 = { local_45.n[local_23], local_45.n[local_24] };
                    if (!function_23a220(&local_48, local_27, local_2, 0.0f))
                        continue;
                    vector3f local_49;
                    vector3d_from_points3d(&arg_2->field_30, &local_45, &local_49);
                    real local_50 = function_184442(&local_49);
                    vector3f local_51 = *g_4687a4;
                    s_18444b *local_52 = (s_18444b *)(g_4e3b44[arg_2->field_0 & 0xffff].bytes + 0xb0);
                    if (local_52->field_10 > 0.0f)
                    {
                        real local_53 = function_184443(1.0f - local_50 / local_52->field_10, 0.0f, 1.0f);
                        if (local_52->field_14 != 0.0f)
                            local_53 = (real)pow(local_53, local_52->field_14);
                        local_53 *= local_52->field_c;
                        local_51.i += local_53 * local_49.i;
                        local_51.j += local_53 * local_49.j;
                        local_51.k += local_53 * local_49.k;
                    }
                    if (local_52->field_4 > 0.0f)
                    {
                        real local_53 = function_184443(1.0f - local_50 / local_52->field_4, 0.0f, 1.0f);
                        if (local_52->field_8 != 0.0f)
                            local_53 = (real)pow(local_53, local_52->field_8);
                        local_53 *= local_52->field_0;
                        local_51.i += arg_2->field_3c.i * local_53;
                        local_51.j += arg_2->field_3c.j * local_53;
                        local_51.k += arg_2->field_3c.k * local_53;
                    }
                    function_175430(local_37, &local_45, &local_51);
                }
            }
        }
    }
    if (local_11->field_10 != NONE)
    {
        s_sound_position local_14;
        local_14.position.x = (local_3[0].hi + local_3[0].lo) * 0.5f;
        local_14.position.y = (local_3[1].hi + local_3[1].lo) * 0.5f;
        local_14.position.z = (local_3[2].hi + local_3[2].lo) * 0.5f;
        local_14.compressed_forward = vector3d_compress(g_4687a8);
        local_14.velocity = *g_4687a4;
        local_14.location = arg_2->field_1c;
        function_1895f0(&local_14, 1.0f, local_11->field_10);
    }
}
#pragma inline_depth(255)
