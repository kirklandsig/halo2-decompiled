// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "object_queries.h"
#include <math.h>

class c_entry_list;




struct s_133f70
{
	long field_0;
	long field_4[4];
	byte field_14[4];
	long field_18[4];
	byte field_28[4];
	byte field_2c[4];
	byte field_30[0x18];
	byte *field_48[4];
	byte field_58[4];
	long field_5c[4];
	byte field_6c[4];
	byte field_70[4];
	byte field_74;
	byte field_75[4][16];
	byte field_b5[0xc6 - 0xb5];
	word field_c6[4][16];
	byte field_146[0x166 - 0x146];
	short field_166;
	byte field_168[2];
	short field_16a;
	bool field_16c;
	byte field_16d[3];
	dword field_170;
	byte field_174[0x194 - 0x174];
	point3f field_194;
	real field_1a0;
	byte field_1a4[8];
	dword field_1ac;
	dword field_1b0;
	real field_1b4;
	byte field_1b8[4];
	byte field_1bc;
	byte field_1bd;
	short field_1be;
	long field_1c0;
	long field_1c4;
	byte field_1c8[4];
	bool field_1cc;
	byte field_1cd[3];
	long field_1d0;
	bool field_1d4;
	byte field_1d5[3];
	real field_1d8;
	real field_1dc;
	real field_1e0;
};

typedef char c_133f70[sizeof(s_133f70) == 0x1e4 ? 1 : -1];


struct s_133721
{
    long field_0;
    short field_4;
    byte field_6[2];
    short *field_8;
    long *field_c;
    long *field_10;
    short *field_14;
};

struct s_133791
{
    long field_0;
    byte field_4[8];
    long field_c;
    long field_10;
    byte field_14[0x30-0x14];
    point3f field_30;
    real field_3c;
    byte field_40[0xaa-0x40];
    byte field_aa;
};

struct s_133792
{
    byte field_0[8];
    s_133791 *field_8;
};

extern real g_4ba028;
void function_134240(s_bit_vector_pool *arg_1);
dword function_1332f0(s_bit_vector_pool const *arg_1, word arg_2);
void function_133f70(s_133f70 const *arg_1, bool arg_2);
void function_4baf0(long arg_1, real arg_2, byte *arg_3, byte *arg_4);
void function_4b5d0(bool, long, long, real, real, s_133f70 *, point3f const *, bool);
typedef void (*t_function_4b5d0_133790)(bool arg_1, long arg_2, long arg_3, real arg_4, real arg_5,
    s_133f70 *arg_6, point3f const *arg_7, bool arg_8);
bool function_133790(s_bit_vector_pool *arg_1, short arg_2, bool arg_3, long *arg_4, long arg_5, bool arg_6);

PRIVATE __forceinline real function_133791(point3f const *arg_1, point3f const *arg_2)
{
    double local_1 = (double)arg_1->x - arg_2->x;
    double local_2 = (double)arg_1->y - arg_2->y;
    double local_3 = (double)arg_1->z - arg_2->z;
    return (real)sqrt(local_3 * local_3 + local_2 * local_2 + local_1 * local_1);
}

#pragma inline_depth(0)
// @retail 0x133720
void function_133720(s_bit_vector_pool *arg_1, bool arg_2)
{
    function_134240(arg_1);
    long local_1 = 0;
    bool local_2 = false;
    long local_3 = 0;
    while (local_3 < ((s_133721 *)arg_1->lists[2])->field_4)
    {
        bool local_4 = function_133790(arg_1, (short)local_3, local_2, &local_1, NONE, arg_2);
        local_2 = false;
        if (local_4 && arg_1->mode == 1)
        {
            local_2 = true;
            local_3--;
        }
        local_3++;
    }
}
#pragma inline_depth(255)

// score 13 (was 108): the radius load and point/index store schedule still differs.
// Earlier projection operand/sum order 95; radius local 30; multiply from the
// object 32; radius copy first 19; depth as one expression 19-30;
// record fields read directly 28; position pointer local 172.
// Native point value 168; point/radius values 173; existing point setter 172;
// setter with shared radius 173; radius before setter 173; contiguous copy 107;
// point value shared with distance 177; native vector copy 219.
// early radius/point acquisition and index/radius store scheduling differ.
// Earlier comparison: the float multiply reads its operands from the other two globals and the sum is added in the other order
// Earlier forms: retail loads the radius into xmm2 before the point copy (and the copy's last
// load before its middle store); ours loads it after the copy. Radius local 30; multiply from the
// object 32; radius first in the record 19; depth as one expression 19-30; record fields read
// directly 28; position pointer local 172.
// Scalar point inputs shared by setter/projection: y/z/x 178/178/94; yz/xy/xz 101/173/173;
// xyz/xyz with shared radius 178/178. All scalar-input forms restored.
// Existing acquisition helper ordinary 206; inline with standalone noinline 206; inline without it 106;
// early radius with delayed stores 30; delayed direct stores 19; early direct with delayed index 19;
// radius pointer and two direct field uses 32 each.
// Record stores point/index/radius, radius/point/index, radius/index/point, index/point/radius,
// index/radius/point 19/19/20/13/21; radius local before the copy with the index stored first 30
// (radius stored first 31, local after the record 30, index first 34); point read directly 24.
// Round 32: BLOCKED-BY 0xbd610. The record's address reaches the 0xbd610 stub (built without /GL)
// through 0x4b5d0 -> 0x137800's node-section argument, so LTCG treats the record as escaped and
// will not move the object's radius/point loads above the record stores. One-file LTCG harness
// diagnostic: with only that argument nulled at 0x137800's call, this body is 2 (only the
// radius/index stores swapped) and index/point/radius or point/index/radius record order matches
// byte for byte. Normal build: index/point/radius 13, radius local first 30, radius local stored
// before the index 31, record declared first 13, source point through a pointer local 175.
// @retail 0x133790
bool function_133790(s_bit_vector_pool *arg_1, short arg_2, bool arg_3, long *arg_4, long arg_5, bool arg_6)
{
    bool local_1 = arg_1->mode == 2 || arg_3;
    s_133721 *local_2 = (s_133721 *)arg_1->lists[2];
    long local_11 = arg_2;
    long local_3 = local_2->field_c[local_11];
    s_133791 const *local_4 = ((s_133792 *)g_4e0300->data)[local_3 & 0xffff].field_8;
    s_133f70 local_5;
    local_5.field_194 = local_4->field_30;
    local_5.field_1a0 = local_4->field_3c;
    local_5.field_0 = local_3;
    // retail does these in SSE single precision: every product and sum is rounded to float
    point3f const *local_16 = &local_5.field_194;
    real local_13 = local_16->y * g_4b9e2c;
    real local_6 = local_16->z * g_4b9e38;
    local_6 += local_13;
    real local_14 = local_16->x * g_4b9e20;
    local_6 += local_14;
    local_6 += g_4b9e44;
    if (!(local_6 >= 0.0f)) local_6 = 0.0f - local_6;
    if (!(local_6 > 0.1f)) local_6 = 0.1f;
    real local_7 = g_4b9ed0 / local_6;
    real local_8 = local_7 * local_5.field_1a0;
    local_5.field_1b4 = local_8 * 2.0f * g_4ba028;
    *(real *)local_5.field_1b8 = function_133791(&local_5.field_194, &g_4b9da0) * g_4b9dcc;
    byte local_15;
    function_4baf0(local_3, *(real *)local_5.field_1b8, &local_5.field_1bc, &local_15);
    local_2 = (s_133721 *)arg_1->lists[2];
    long local_12 = local_2->field_8[local_11];
    local_5.field_1ac = function_1332f0(arg_1, local_12);
    *(bool *)(local_5.field_1a4 + 4) = (local_12 & 0x200) != 0;
    local_5.field_1e0 = 0.0f;
    local_5.field_1cc = false;
    *(long **)local_5.field_1c8 = arg_4;
    local_5.field_1c0 = arg_5;
    local_5.field_1d0 = NONE;
    local_5.field_1d4 = false;
    local_5.field_1d8 = 0.0f;
    local_5.field_1dc = 0.0f;
    *(long *)local_5.field_1a4 = 0;
    local_5.field_1be = NONE;
    short marker = local_12 & 0x800 ? NONE : local_2->field_14[local_11];
    if (marker != NONE)
    {
        local_5.field_1b0 = 0x40;
        local_5.field_1be = marker;
    }
    else
        local_5.field_1b0 = 1;
    ((t_function_4b5d0_133790)function_4b5d0)(local_1, arg_1->mode, local_5.field_0, *(real *)local_5.field_1b8,
        local_5.field_1b4, (s_133f70 *)((byte *)&local_5 + 4), &local_5.field_194, *(bool *)(local_5.field_1a4 + 4));
    local_5.field_1b0 |= 0x208;
    if (arg_3)
    {
        local_5.field_16c = false;
        local_5.field_1ac = (local_5.field_1ac & ~2) | 4;
    }
    else if (local_5.field_16c && arg_1->mode == 1)
        local_5.field_1ac &= ~4;
    bool local_18 = local_5.field_16c;
    if (local_5.field_1bc > 0)
    {
        if (!(local_5.field_170 & 2) || arg_1->mode == 0)
        {
            arg_1->function_133ab0(&local_5, arg_6, false, local_1);
            local_18 = local_5.field_16c;
        }
    }
    else ((s_133721 *)arg_1->lists[2])->field_8[local_11] |= 1;
    return local_18;
}

// @retail 0x133ab0
void s_bit_vector_pool::function_133ab0(s_133f70 *arg_2, bool arg_3, bool arg_4, bool arg_5)
{
    long local_7;
    if (arg_2->field_166 > 0)
    {
        bool local_1 = false;
        if (!arg_4)
        {
            if (arg_2->field_170 & 0x80)
            {
                vector3f delta;
                vector3d_from_points3d(&this->center, &arg_2->field_194, &delta);
                arg_2->field_1e0 = dot3f(&delta, &g_4b9dac);
                local_1 = false;
                if (!arg_2->field_1cc)
                {
                    long *counter = *(long **)arg_2->field_1c8;
                    arg_2->field_1cc = true;
                    arg_2->field_1c0 = (*counter)++;
                    arg_2->field_1c4 = 3;
                }
                s_133f70 local_2 = *arg_2;
                local_2.field_1ac = local_2.field_1b0;
                local_2.field_1b0 = 0;
                local_2.field_1d0 = *(long *)((byte *)&local_2 + 0x180);
                local_2.field_1dc = *(real *)((byte *)&local_2 + 0x190);
                function_133ab0(&local_2, true, true, arg_5);
            }
            if (arg_2->field_170 & 2)
            {
                vector3f delta;
                vector3d_from_points3d(&this->center, &arg_2->field_194, &delta);
                real local_3 = dot3f(&delta, &g_4b9dac);
                local_1 = true;
                if (!arg_2->field_1cc)
                {
                    long *counter = *(long **)arg_2->field_1c8;
                    arg_2->field_1cc = true;
                    arg_2->field_1c0 = (*counter)++;
                    arg_2->field_1c4 = 1;
                }
                s_133f70 local_4 = *arg_2;
                local_4.field_1ac = arg_2->field_1b0;
                local_4.field_1b0 = 0;
                local_4.field_1d0 = *(long *)((byte *)&local_4 + 0x178);
                local_4.field_1d4 = true;
                local_4.field_1d8 = *(real *)((byte *)&local_4 + 0x188);
                local_4.field_1e0 = local_3;
                function_133ab0(&local_4, true, true, arg_5);
            }
            dword entry_flags = arg_2->field_170;
            if (entry_flags & 1)
            {
                entry_flags &= ~1;
                s_133f70 local_5 = *arg_2;
                local_5.field_1ac |= 0x8000;
                local_5.field_1d0 = *(long *)((byte *)&local_5 + 0x174);
                local_5.field_1dc = *(real *)((byte *)&local_5 + 0x184);
                arg_2->field_170 = entry_flags;
                local_1 = (bool)((entry_flags >> 1) & 1);
                function_133ab0(&local_5, false, true, arg_5);
            }
            dword entry_flags_2 = arg_2->field_170;
            if (entry_flags_2 & 0x20)
            {
                entry_flags_2 &= ~0x20;
                s_133f70 local_6 = *arg_2;
                local_6.field_1ac |= 0x8000;
                local_6.field_1d0 = *(long *)((byte *)&local_6 + 0x17c);
                local_6.field_1dc = *(real *)((byte *)&local_6 + 0x18c);
                arg_2->field_170 = entry_flags_2;
                local_1 = (bool)((entry_flags_2 >> 1) & 1);
                function_133ab0(&local_6, false, true, arg_5);
            }
        }
        if (arg_4 || !local_1)
            function_133f70(arg_2, !arg_3);
    }
    if (arg_3 && !arg_2->field_16c)
    {
        local_7 = ((s_133792 *)g_4e0300->data)[arg_2->field_0 & 0xffff].field_8->field_10;
        while (local_7 != NONE)
        {
            s_133f70 local_8 = *arg_2;
            local_8.field_0 = local_7;
            if (local_8.field_1c4 == 1)
            {
                s_133791 const *local_9 = ((s_133792 *)g_4e0300->data)[local_7 & 0xffff].field_8;
                if (local_9->field_aa == 2 ? (bool)*(short *)(g_4e3b44[local_9->field_0 & 0xffff].bytes + 0x290) : false)
                {
                    local_8.field_1cc = false;
                    local_8.field_1c0 = 0x1f;
                    local_8.field_1c4 = NONE;
                    local_8.field_1d0 = NONE;
                    local_8.field_1d4 = false;
                    local_8.field_1d8 = 0.0f;
                }
            }
            local_8.field_1b0 = (local_8.field_1b0 & ~0x40) | 1;
            local_8.field_1be = NONE;
            ((t_function_4b5d0_133790)function_4b5d0)(arg_5, this->mode, local_7, *(real *)local_8.field_1b8,
                local_8.field_1b4, (s_133f70 *)((byte *)&local_8 + 4), &local_8.field_194, *(bool *)(local_8.field_1a4 + 4));
            function_133ab0(&local_8, true, arg_4, arg_5);
            local_7 = ((s_133792 *)g_4e0300->data)[local_7 & 0xffff].field_8->field_c;
        }
    }
}
