#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
#include "slot_handler.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_29de20 { short field_0; short field_2; short field_4; };
struct s_29de21 { s_29de20 field_0[30]; short field_b4; };
short function_29de20(short arg_1, s_29de21 *arg_0, point3f const *arg_2, long arg_3);

struct s_29e0f0 { long field_0; point3f *field_4; };
struct s_29e0f1 { byte field_0[0x40]; long field_40; s_29e0f0 *field_44; };
struct s_29e0f2 { byte field_0[0x6c]; long field_6c; s_29e0f1 *field_70; };
struct s_29e0f3 { byte field_0[0xc4]; long field_c4; s_29e0f2 *field_c8; };
struct s_29e0f4 { short field_0; short field_2; };
struct s_29e0f5 { byte field_0[0x80]; long field_80; s_29e0f4 *field_84; };
struct s_29e0f6
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0xa];
    long field_30;
    s_29e0f5 *field_34;
};
struct s_29e0f7 { byte field_0[0x168]; long field_168; s_29e0f6 *field_16c; };
struct s_29e0f8
{
    short field_0;
    short field_2;
    long field_4;
    short field_8;
    byte field_a[2];
    s_type_c3b527 field_c;
};
struct s_29e0f9
{
    bool field_0;
    byte field_1[3];
    s_type_c3b527 field_4;
    long field_14;
    real field_18;
    point3f field_1c;
    short field_28;
    byte field_2a[2];
    bool field_2c;
    char field_2d;
    bool field_2e;
    byte field_2f;
    s_29e0f8 field_30[5];
    byte field_bc[8];
};

PRIVATE __forceinline real function_29e2ea(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_1->x - arg_0->x;
    real local_1 = arg_1->y - arg_0->y;
    real local_2 = arg_1->z - arg_0->z;
    return local_1 * local_1 + (local_0 * local_0 + local_2 * local_2);
}
PRIVATE __forceinline void function_29e64d(s_29e0f8 *arg_0, point3f const *arg_1)
{
    arg_0->field_0 = NONE;
    arg_0->field_8 = NONE;
    arg_0->field_c.point = *arg_1;
    arg_0->field_c.output_index = NONE;
    arg_0->field_4 = NONE;
    arg_0->field_2 = 0;
}

PRIVATE __forceinline real function_29e3b1(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_0->x - arg_1->x;
    real local_1 = arg_0->y - arg_1->y;
    real local_2 = arg_0->z - arg_1->z;
    return (real)sqrt(local_2 * local_2 + local_1 * local_1 + local_0 * local_0);
}
// score 390 (was 390): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Round 27: the instruction order follows retail closely, but the locals sit in different frame
// slots, the group pointer is in EBP where retail keeps EBX and retail parks the group index in the
// incoming s_reference slot at 0x80; no form found this round.
// MiniMax S (10-31): no new form, no lower score. The frame agrees (0x64), but retail parks its loop
// index and point index in memory (frame 0x2c and 0x80) where ours keeps them in registers, its three
// NONE defaults sit at frame 0x44/0x28/0x24 with the best distance at 0x38, against our 0x3c/0x40/0x1c
// and 0x34, and it holds the group table in ebx and the point table in ebp where we swap them.
// Earlier 390 (was 398): frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// frame 0x64 now agrees; selected defaults, point bitmap, endpoint homes, root-table roles and x87 scheduling differ.
// Round 132 (m3 lane 5, fast): root-table test nested under the count test with the pointer declared there
// (no NULL default) 398; overall result declared first 399; best distance before the selected defaults 398;
// group bitmap declared before the record/group pointers 412; record pointer after the marker call 420.
// Earlier: frame 0x68/0x64, induction/endpoint homes and x87 scheduling differ.
// Earlier: table-pointer roles now agree; frame 0x68/0x64, outer-loop reload placement, inner-loop registers and x87 square order remain.
// Complete initial guard exits 458; CRT output point copy 416; distance second point by value 569;
// squared-distance second point by value 430; conditional table pointer initialization 416.
// Group-table field at each use 457; native long overall result 445; bounded output for 420; same-group pretested while 416; positive nearest-point scope 416.
// Entry secondary root 417; direct initial root fields 416; native output-point reference 416;
// native distance references 416; native first squared-distance point 433; unsigned-word fixed pass 416.
// Round 121: Success set before output fill 440; native first point of length helper 579; signed-byte direction step 416; metadata before point copy 439; signed-byte per-pass result 416. Original helper contracts and complete body restored.
// Earlier: entry bitmap storage 416; output-helper formal order 454; failed-selection common exit 416; ordinary distance inline 416; byte overall result 445.
// Round 341: One short direction counter, point payload and long record-index graphs 425/454/431/459; long direction counter 479/453/477/453. Original loops and values restored.
// Round 350: real outer/inner/both link-element receivers 416/416/416; with read-only
// link/group/both/point/all views 416 each. Frame 0x6c/0x64, table roles and bound spill remain.
// Round 146: short group index/pass counter/best indices (kept) 406; the three alone 409/416/512;
// distance index at top of the inner while 416; long search result 415, on the kept form 428;
// long point bounds 456; long last point 424; long nearest point 396 (word compares in retail, not kept).
// Round 86 (fast; aligned counts in brackets, 378 at the start): long group index 513 [313, retail's first-loop
// registers]; separate output cursor (retail compares the end index with the original start) 416 [357]; both 490
// [278]; bit_vector_test/bit_vector_set instead of the open-coded bits, identical; else-false result 459 [238];
// record index set before the pass loop and after each step 446 [189, frame 0x64]; memset group bitmap 437 [185];
// bit test on the field read ahead of the group-index local 197 [65], with the (point, record) output-helper order
// 199 [57], x*x + (y*y + z*z) squared sum 190 [48] and the found flag declared first 188 [46] (not kept: a field read
// beside a local copy needs a match; the same body without it 437). Left on 188: x87 square orders (retail rotates
// them per call; a vector-local distance with j, k, i terms gives d1/d2 175), arg_0 in EDX with arg_3 kept in ESI
// at the 0x29de20 call, AL reused for the found flag, the pass latch's lea, the output loop's entry jump.
// Round 148: the visited bits through the existing bit_vector_test/bit_vector_set (include/bit_vector.h)
// 416 at 1577 bytes, but difflib-aligned 378 -> 338: retail's separate test and set (word address and
// mask recomputed, load/or/store) and its unpadded outer loop; the same on the group bits only 416; with a
// long group index 513 (the set CSEs again); the length helper's sum written y, x, z 416 (406 without the
// bit helpers; same code).
// Round 267: complete parallel square terms in squared-distance/length/both helpers
// 406/406/406; selected-index role names 406; those names with squared-distance/length/
// both terms 406/406/406; complete graph with helper-axis names 406. Frame/index and
// square-input homes remain; original helpers, names, loops and body restored.
// Round 366: overwritten endpoint initializer removed 417; early group receiver 432; both 440; complete two-leg distance sum 406; with endpoint/receiver/both 417/432/440; all with scoped best-distance default 474. All restored.
// Round 232: direct-step pretested inner for, alone and with all seven complete
// link/directional/selected short-workspace identity subsets, 406 each. All restored.
// Round 234: complete-search link count/base/both 459/438/478; with promoted group
// domain 452/494/465; count/base with shared bits 462, plus promoted group 465. All restored.
// Round 368: test-produced final result 440; complementary pass admission 406; search-scoped best distance
// 457; result/admission 440; result/distance 478; admission/distance 457; all and named result 478/478.
// Original executable body restored.
// Round 370: shared point-array base with long group index, captured point count and shared
// bitmap operations 429/428/410/432/515/515/464/464; body, helpers and original includes restored.
// Round 214: Function-owned short outer/grouped-link/pass induction declarations, all seven
// nonempty subsets, 406 each; all three plus the record-search induction declaration 406.
// Every original initialization/pretest/increment and executable body restored; frame/index spill remains.
// Round 163: selected-index chain/per-group index chain/explicit native bitmap
// defaults, all seven subsets and complete graph with separate bitmap stores:
// 406 each at 1596 bytes. Frame 0x68/0x64 and bound/endpoint homes remain;
// original default initializers and every helper restored.
// Round 380: Native point bitmap unsigned/signed 456/456; unsigned group bitmap with array/unsigned/signed point bitmap 426/434/434; signed group bitmap 426/434/434. Original arrays restored.
// Round 239: coherent validator/reference-provider ordinary, inline and noinline
// declaration pairs (eight non-original combinations) 406 each. All restored.
// Earlier: the selected point index is a byte, as retail stores it. The frame 0x68 versus
// 0x64, the group and point bitmap homes and the remaining register rotations remain.
// Round 412: byte terminal/pass/both/output/terminal-output/pass-output/all/all plus accepted byte 446/436/439/398/447/436/439/439; native signed group on the current byte producer 496. All original types, loops and helper contracts restored.
// Round 215: consistent group/lower-endpoint/both field observations on the byte-last-point graph
// 462/452/473; with shared bitmap operations 488/429/476; real link receiver for group/all 429/475.
// Original snapshots, bitmap operations and body restored; frame 0x64 agrees, selected defaults, root
// roles and endpoint homes remain.
// Round 434: scan-scoped selected group/first/both/terminal/group-terminal/first-terminal/all
// constant defaults 438/438/440/438/440/440/444; all plus scan-scoped best distance 468. Frame
// stays 0x64; the last grows to 1609 bytes. Late group default recovers root registers but loses an entry store; restored.
// Round 205: Consistent link-array base with outer/inner/both actual cursors 464/433/447; record cursor 433;
// outer-record/inner-record/all cursors 500/455/436; all with shared bitmap operations 458.
// All restored; selected-default, endpoint and bitmap homes and root-table roles remain.
// Round 4 (lane 18, fast): the selected defaults, best distance or first default renamed to sort
// first/last 398 each (slots don't move: retail 0x44/0x28/0x24/0x38, ours 0x3c/0x40/0x1c/0x34); the
// group table read before the link table 414; the group bitmap cleared before the marker call 413;
// the three defaults as one chained assignment 398; the group index declared with its read 398; the
// marker position declared at function scope 398. Restored.
// Round 205: Consistent link-array base with outer/inner/both actual cursors 464/433/447; record cursor 433;
// outer-record/inner-record/all cursors 500/455/436; all with shared bitmap operations 458.
// All restored; selected-default, endpoint and bitmap homes and root-table roles remain.
// Round 245: complete squared/length metric deltas via the shared point difference or
// native whole-vector initializer, eight non-original pairs 402/398/418/398/418/398/418/398.
// Original scalar producers and entire body restored on the byte-endpoint/frame-0x64 graph.
// Round 464: late per-group record default/endpoints/both/nearest-distance default
// 403/448/430/398; record-distance/endpoints-distance/all 403/448/430; all with
// complete matching-record search 423. Original per-group lifetimes and search restored.
// Round 146 (lane 4, fast with 0x29e730): the group read through a multi-use `link` pointer at every use (no
// local_15) 429 (1601 bytes); round 86's bit test on the field read ahead of local_15 459 (aligned 209 masked,
// against 213); `long local_15` 496 (1559 bytes); the group bits through the existing bit_vector_test/
// bit_vector_set 424 (1561 bytes, masked aligned 195: retail's separate test and set, but the pass-scan compare
// stays 16-bit, `cmp word [..], dx`, where retail compares the movsx'd field with a 32-bit group in EBX); the
// point bits through them too 424 (same code); `long local_15` with the helpers 496 (the set shares the test's mask again). Retail spills the group (EBX) to the arg_2 home and later reuses
// that home for the pass's found flag (`mov byte ptr [esp + 0x80], 0`).
// Round 474: Native early reference/provider/both ordinal snapshots 398 each;
// complete link payload alone/with reference/provider/both 475 each; complete
// graph with promoted provider ordinal 513. Original body and helpers restored.
// Round 112 (lane 8, fast): group-table pointer declared first and assigned at its test 398; scenario
// pointer declared first 398; no structure-BSP local (global read at both uses) 398; the three defaults
// declared in reverse order 398; the group-table pointer as one ternary initialiser 398 (all same code).
// Round 494: complete point-input copies inside squared/length metrics, first/second/both
// in each alone, then both squared with first/both length: 439/437/473/558/555/787/677/892.
// Original input reads, metric helpers and entire body restored; selected homes and table roles remain.
// Round 221: native selected group/terminal/both 466/394/459; with native group input 513/470/471
// (native input control 496); complete graph with short byte-narrowed terminal producer 433. Native
// selected terminal plus actual output-record cursor and consistent point-table fields 390, kept:
// best-distance home agrees at frame 0x64. Separate short/long output-point induction 415/412; with one
// shared computed step 418/427. Original signed-short input scans, byte narrowing, metric helpers and
// validator tuples remain; selected-default, bitmap/endpoint, root-table and x87 homes still differ.
// Round 51 (lane 11, fast): round 86's t19 form rebuilt on this body 437 with the byte selected point,
// 188 (aligned 46) with it short again; on that graph the existing distance3d (0x3ea30, inlined) for the
// three point distances with b - a operands 185 (aligned 57), with a - b 216; the distance helper through
// a vector local with j,k,i / i,j,k / k,j,i sums 188 each (same code). Left on 188: per-call x87 square
// orders, arg_3 not kept in ESI across the distances to the 0x29de20 call, the found flag stored from AL,
// the record-index add, the output loop's entry jump. distance3d on the original 418/418.
// Round 494: complete point-input copies inside squared/length metrics, first/second/both
// in each alone, then both squared with first/both length: 439/437/473/558/555/787/677/892.
// Original input reads, metric helpers and entire body restored; selected homes and table roles remain.
// DH round 264: Actual destination-record cursor with short/word/byte/long output ordinals 402/402/402/404;
// with consistent point-table fields 391/391/391/392. Indexed destination with those fields 412;
// point-first/terminal-first eligibility on the 391 graph 391/391. Seven-point lead below gate;
// original body restored; selected-default/root roles and metric/bitmap homes remain.
// DL224: Group range-first/negative-first/split-range/split-bitmap eligibility 473/440/473/390;
// the four with shared group bits 434/434/434/424. Original group observation, domains, helpers
// and body restored; default/bitmap/endpoint homes, root roles and x87 scheduling remain.
// @retail 0x29e0f0
bool __stdcall function_29e0f0(point3f const *arg_0, long arg_1, s_reference arg_2,
    s_29de21 *arg_3, s_29e0f9 *arg_4)
{
    short local_0 = NONE;
    short local_1 = NONE;
    long local_2 = NONE;
    real best_distance = 3.402823466e38f;
    bool local_4 = false;
    s_29e0f3 *local_5 = (s_29e0f3 *)g_4e0348;
    s_29e0f2 *local_6 = NULL;
    if (local_5->field_c4 > 0) local_6 = local_5->field_c8;
    if (local_6 && local_6->field_6c > 0 && !(arg_2.unknown2 & 0x8000) && arg_2.unknown2 >= 0)
    {
        s_29e0f7 *local_7 = (s_29e0f7 *)g_4e0350;
        if (arg_2.unknown2 < local_7->field_168)
        {
            s_262b40_result *local_8 = function_262b40(arg_2);
            if (local_8)
            {
                s_29e0f6 *local_9 = &local_7->field_16c[(word)arg_2.unknown2];
                if (local_8->unknown10 >= 0 && local_8->unknown10 < local_9->field_30 && local_9->field_24 == g_4686c4)
                {
                    s_29e0f5 *local_10 = &local_9->field_34[local_8->unknown10];
                    s_29e0f1 *local_11 = local_6->field_70;
                    point3f marker_position;
                    function_210850((s_type_c3b527 const *)local_8, &marker_position);
                    dword visited_groups[2] = {0, 0};
                    for (short local_14 = 0; local_14 < local_10->field_80; ++local_14)
                    {
                        short local_15;
                        local_15 = local_10->field_84[local_14].field_0;
                        if ((visited_groups[local_15 >> 5] & (1 << (local_15 & 31))) || local_15 < 0 || local_15 >= local_11->field_40) continue;
                        visited_groups[local_15 >> 5] |= 1 << (local_15 & 31);
                        dword visited_points[1] = {0};
                        short local_17 = local_10->field_84[local_14].field_2;
                        short local_18 = local_17;
                        short local_19 = NONE;
                        short local_20 = NONE;
                        real local_21 = 3.402823466e38f;
                        s_29e0f0 *local_22 = &local_11->field_44[local_15];
                        for (short local_23 = local_14; local_23 < local_10->field_80 && local_10->field_84[local_23].field_0 == local_15; ++local_23)
                        {
                            local_18 = local_10->field_84[local_23].field_2;
                            visited_points[local_18 >> 5] |= 1 << (local_18 & 31);
                            if (local_18 >= 0 && local_18 < local_22->field_0)
                            {
                                real local_24 = function_29e2ea(&marker_position, &local_22->field_4[local_18]);
                                if (local_21 > local_24) { local_21 = local_24; local_19 = local_18; }
                            }
                        }
                        if (local_19 == NONE) continue;
                        for (short local_25 = 0; local_25 < arg_3->field_b4; ++local_25)
                            if (arg_3->field_0[local_25].field_0 == local_15) { local_20 = local_25; break; }
                        short local_26 = 1;
                        short local_28 = local_19;
                        for (short local_27 = 0; local_27 < 2; ++local_27)
                        {
                            char local_29 = local_19;
                            point3f *local_30 = &local_22->field_4[local_19];
                            real local_31 = function_29e3b1(&marker_position, local_30);
                            bool local_32 = false;
                            while (local_28 >= 0 && local_28 < local_22->field_0)
                            {
                                if (local_32 && ((local_27 == 0 && local_28 > local_18) || (local_27 == 1 && local_28 < local_17))) break;
                                if (visited_points[local_28 >> 5] & (1 << (local_28 & 31)))
                                {
                                    local_30 = &local_22->field_4[local_28];
                                    local_29 = local_28;
                                    local_31 = function_29e3b1(&marker_position, local_30);
                                }
                                short local_33 = local_20 + local_28;
                                if (local_33 >= 0 && local_33 < arg_3->field_b4 && arg_3->field_0[local_33].field_4 != 2)
                                {
                                    point3f *local_34 = &local_22->field_4[local_28];
                                    real local_35 = function_29e3b1(local_34, arg_0) + function_29e3b1(local_30, local_34) + local_31;
                                    if (best_distance > local_35 && function_29de20(local_33, arg_3, arg_0, arg_1) == 1)
                                    {
                                        best_distance = local_35;
                                        local_0 = local_15;
                                        local_1 = local_28;
                                        local_2 = local_29;
                                        local_32 = true;
                                    }
                                }
                                local_28 += local_26;
                            }
                            local_26 = -local_26;
                            local_28 = local_19 + local_26;
                        }
                    }
                    if (local_0 != NONE && local_1 != NONE && local_2 != NONE)
                    {
                        s_29e0f8 *output = &arg_4->field_30[0];
                        short local_37 = 0;
                        do
                        {
                            function_29e64d(output, &local_11->field_44[local_0].field_4[local_1]);
                            ++output;
                            ++local_37;
                            if (local_1 == local_2) break;
                            local_1 += local_2 > local_1 ? 1 : -1;
                        } while (local_37 < 4);
                        if (local_37 < 4)
                        {
                            function_29e64d(output, &marker_position);
                            ++local_37;
                            arg_4->field_2c = true;
                        }
                        else arg_4->field_2c = false;
                        arg_4->field_2d = (byte)local_37;
                        local_4 = true;
                    }
                }
            }
        }
    }
    return local_4;
}

long function_baf80(long arg_0);
long function_1e4990(long arg_0);
bool function_2702a0(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4,
    point3f const *arg_5, vector3f const *arg_6, point3f *arg_7,
    long *arg_8, vector3f *arg_9, long *arg_10);
struct s_29db90;
void function_29db90(s_29db90 *list, short block_index, short range_index);
bool function_29dd50(point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3);

struct s_29e730
{
    short field_0;
    byte field_2[0xa];
    s_reference field_c;
    bool field_10;
    byte field_11[0x18];
    bool field_29;
    byte field_2a[2];
    vector3f field_2c;
};
struct s_29e731 { byte field_0[0x18]; long field_18; byte field_1c[0x38]; long field_54; };

// @retail 0x29e730
bool __stdcall function_29e730(s_29e730 *arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 const *arg_3, s_reference arg_4, s_29de21 *arg_5,
    long arg_6, s_29e0f9 *arg_7)
{
    bool local_0 = false;
    s_29e731 *local_1 = (s_29e731 *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888);
    long local_2;
    if (local_1->field_18 != NONE)
        local_2 = function_baf80(local_1->field_18);
    else
        local_2 = NONE;
    memset(arg_7, 0, sizeof(*arg_7));
    s_type_c3b527 local_3;
    s_type_c3b527 local_6;
    if (arg_0->field_0 == 6)
    {
        vector3f local_4;
        vector3f local_8;
        point3f local_9;
        {
            point3f local_5;
            if (!function_262a90(arg_0->field_c, &local_5, &local_4)) goto local_20;
            if (!function_2702a0(arg_1, 0xf000545, 0x5000049, 0x6000542, 0x400000c,
                &local_5, &local_4, &local_9, NULL, &local_8, NULL)) goto local_20;
        }
        s_262b40_result *local_10 = function_262b40(arg_0->field_c);
        vector3f local_11;
        if (!function_210770(((s_type_c3b527 *)local_10)->output_index, &local_8, &local_11)) goto local_20;
        if (!function_210690(((s_type_c3b527 *)local_10)->output_index, &local_9, &local_3.point)) goto local_20;
        local_3.output_index = ((s_type_c3b527 *)local_10)->output_index;
        point3f local_24;
        local_24.x = local_9.x + local_4.i * 1.5f;
        local_24.y = local_9.y + local_4.j * 1.5f;
        local_24.z = local_9.z + local_4.k * 1.5f;
        if (arg_0->field_10)
        {
            arg_0->field_29 = true;
            arg_0->field_2c = local_8;
        }
        else if (function_210690(((s_type_c3b527 *)local_10)->output_index, &local_24, &local_6.point))
        {
            local_6.output_index = local_3.output_index;
            local_0 = true;
        }
        goto local_21;
    }

    if (arg_0->field_0 == 4)
    {
        long local_12 = function_1e4990(local_1->field_54);
        if (!local_12 || (*(byte *)local_12 & 2)) goto local_20;
        s_262b40_result *local_10 = function_262b40(arg_0->field_c);
        if (!local_10 || !((byte)local_10->flags & 0x40)) goto local_20;
        vector3f local_4;
        point3f local_5;
        vector3f local_8;
        point3f local_9;
        if (!function_262af0(arg_0->field_c, &local_5, &local_8)) goto local_20;
        if (!function_2702a0(arg_1, 0x6000542, 0x400069d, 0x6000542, 0x400000c,
            &local_5, &local_8, &local_9, NULL, &local_4, NULL)) goto local_20;
        if (!function_210690(((s_type_c3b527 *)local_10)->output_index, &local_9, &local_3.point)) goto local_20;
        local_3.output_index = ((s_type_c3b527 *)local_10)->output_index;
        goto local_21;
    }
local_20:
    local_3 = *arg_3;
    local_0 = false;
local_21:
    point3f local_7;
    if (local_3.output_index != NONE)
    {
        if (!function_2104b0(local_3.output_index, &local_3.point, &local_7))
            local_7 = local_3.point;
    }
    else
        local_7 = local_3.point;
    arg_7->field_1c = *arg_2;
    arg_7->field_4 = local_3;
    arg_7->field_28 = NONE;
    arg_7->field_14 = NONE;
    arg_7->field_2e = false;
    arg_7->field_18 = 0.0f;
    bool local_13;
    if (function_29dd50(arg_2, &local_7, local_2, arg_6))
    {
        arg_7->field_30[0].field_0 = NONE;
        arg_7->field_30[0].field_c = local_3;
        arg_7->field_30[0].field_4 = NONE;
        arg_7->field_30[0].field_8 = NONE;
        arg_7->field_2d = 1;
        arg_7->field_2c = true;
        local_13 = true;
    }
    else
    {
        if (REFERENCE_EQUAL(arg_4, g_470fa0) || (arg_4.unknown2 & 0x8000))
        {
            local_13 = false;
        }
        else
        {
            s_29de21 local_15;
            if (!arg_5)
            {
                s_262b40_result *range_data = function_262b40(arg_4);
                arg_5 = &local_15;
                local_15.field_b4 = 0;
                function_29db90((s_29db90 *)&local_15, arg_4.unknown2, range_data->unknown10);
            }
            local_13 = function_29e0f0(arg_2, local_2, arg_4, arg_5, arg_7);
            if (local_13 && arg_7->field_2c && arg_7->field_2d > 0)
            {
                arg_7->field_30[arg_7->field_2d - 1].field_0 = NONE;
                arg_7->field_30[arg_7->field_2d - 1].field_c = local_3;
                arg_7->field_30[arg_7->field_2d - 1].field_4 = NONE;
                arg_7->field_30[arg_7->field_2d - 1].field_8 = NONE;
            }
        }
    }
    if (local_13 && arg_7->field_2c && local_0)
    {
        short local_18 = arg_7->field_2d - 1;
        if (arg_7->field_2d < 4)
        {
            arg_7->field_30[arg_7->field_2d] = arg_7->field_30[arg_7->field_2d - 1];
            ++arg_7->field_2d;
        }
        else arg_7->field_2c = false;
        s_29e0f8 *local_19 = &arg_7->field_30[local_18];
        local_19->field_0 = NONE;
        local_19->field_8 = NONE;
        local_19->field_c = local_6;
        local_19->field_4 = NONE;
        local_19->field_2 = 0;
    }
    arg_7->field_0 = local_13;
    if (!local_13)
    {
        short *local_23 = (short *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888 + 0x5b4);
        if (++*local_23 > 32) *local_23 = 0;
    }
    return arg_7->field_0;
}
