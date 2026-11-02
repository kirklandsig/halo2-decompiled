#include "unknown_11c920.h"
#include "ai_actor.h"
#include "props.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"

// @flags /O2 /arch:SSE /Gr

#include "unknown_1af160.h"

struct s_1f4a20_entry;
struct s_1f4a20_source;
void *function_1e4be0(long arg_0);
bool __stdcall function_1ac610(long arg_0, point3f const *arg_1, s_object_marker *arg_2, short *arg_3);
bool function_1f4a20(s_1f4a20_entry const *arg_2, long arg_0, s_reference arg_1, s_1f4a20_source const *arg_8,
    bool arg_3, s_type_c3b527 *arg_4, vector3f *arg_5, signed char *arg_6, bool *arg_7);
bool function_1af530(long arg_0, long *arg_1, long *arg_2);
void function_265c30(long arg_0, long arg_1, bool arg_2);

// score 9 (was 9): the three dword homes still rotate against retail's (ours: flag,
// temp, actor, result; retail: actor, temp, result, flag); this round's source
// permuter (19 tries, 670 s) found no lower form of that order.
// Earlier: the frame homes for the actor, the result and the completion flag
// still rotate against retail's (ours: flag, temp, actor, result; retail: actor,
// temp, result, flag) although each of the four locals is read the same number of
// times; this round's trials in this file and the source permuter found no lower
// form of that order.
// Earlier: actor/source/completion homes rotate +0x2c/+0x30/+0x24 versus
// +0x24/+0x2c/+0x30. The frame and native conversion agree.
// Earlier layout/conversion observations and trials: the locals sat eight bytes lower, so the
// retail rounds the delay with its own fld/fistp straight into the long that
// the locals sit at stack addresses eight bytes lower than retail's, so the
// values handed to the calls are taken from the wrong slots, and retail converts the scaled delay
// inline (fld, fistp into local_13's dead slot, mov dx) where ours calls real_to_long.
// `static __forceinline long real_to_long(real value);` declared here gives retail's code
// instruction for instruction (44, all frame offsets: ours pairs the fistp temp with the actor
// offset's slot, so local_16 needs its own and the lower slots move by 4; retail also has local_13
// below local_4 and local_2 below local_14). With it: local_16 before local_17 46; local_13
// declared at the top 44; the product passed straight to the conversion 44; names swapped 44
// (names do not order this frame); a separate local for 0x1af530's unused output 44 (67 without).
// the 0x163c frame agrees, but scalar stack homes and the conversion
// helper boundary differ from retail.
// Round 197 fast trials (unchanged reduced preview 207; full 36): Native reference member
// comparisons, short-circuit/bitwise tests x aggregate/member copy x original/forced-inline
// conversion 279, 275, 348, 350, 346, 353, 402, 405.
// Round 77 (plain checks): forceinline conversion declaration 44 (retail's inline fld/fistp; only
// frame slots then differ: retail pairs the offset temporary with local_16 and local_13 with the
// conversion result, and orders local_1, local_14, local_2 at the top); with 0x1af530's second
// output in its own block local 44; local_14 renamed 44 (names move nothing); with the +0x14
// result field read once 46; the separate output local alone 67.
// Round 16 (plain, all with the forceinline declaration): the frame is one dword larger than
// retail's: retail packs the fistp temporary, 0x261280's long output and 0x1af530's output into
// one slot and the 0x262b40 result into the actor offset's; ours packs the fistp temporary and a
// separate 0x1af530 output into the actor offset's slot and gives the 0x262b40 result its own.
// The conversion result in a named long 44; the separate output declared before the real 44;
// the bool for 0x1f4a20 declared at the top 44; the long and bool outputs of 0x261280 declared
// the other way round 44; 0x261280's outputs scoped to a block ending after 0x2626b0 (the empty
// result branch returning early) 206.
// retail rounds the delay with its own fld/fistp straight into the long that
// also takes 0x261280's count output and 0x1af530's second output (the matched 0x1bd230 writes the
// same random-delay rounding in its body); the real_to_long helper's separate result cannot share
// that home. Left: three dword homes rotate (the actor spill, ours 0x2c / retail 0x24; the
// 0x25d690 result spill, 0x30 / 0x2c; the 0x261280 bool output, 0x24 / 0x30): retail sorts the
// bool output last, ours first, probably because it reaches the stubbed 0x260670 (BLOCKED-BY
// 0x260670, whose real body is disabled in the held src/unknown_261280.cpp).
// Tried: `static __forceinline long real_to_long(real value);` 44 (retail's fld/fistp, but the
// helper's result shares the actor offset's home, so every small slot moves): with local_16
// before local_17 46; local_13 declared at the top 44; the product passed straight to the
// conversion 44; names swapped 44; a separate local for 0x1af530's unused output 44 (67 without);
// local_14 renamed 44; the +0x14 result field read once 46. Native reference member comparisons,
// short-circuit/bitwise tests x aggregate/member copy x original/forced-inline conversion 279,
// 275, 348, 350, 346, 353, 402, 405 (fast). `local_13 = real_to_long(...)` with the forceinline
// helper 148 (981 bytes), without it 133; the 0x261280 outputs in a block closing after the
// 0x2626b0 call (equal case returning early) 206. Round 41: the in-body fld/fistp into a
// block-scoped long 44 (as the forceinline helper); into local_13 9 (kept). On that form: the bool
// output declared first 9; it renamed 9 (names do not order these homes); the existing actor and
// prop-node getters 9 (identical code, kept); the 0x262b40 result declared before its flag 13;
// a separate long for 0x1af530's second output 45.
// Round 204 (all-image fast): completion initialized 205; chained false outputs 13;
// acceptance producer 174; const source 9; guarded two-use key 9; complete role names 9;
// role names/completion initialized 205; role names/guarded key 9. Original body restored.
// a separate long for 0x1af530's second output 45. Round 43: descriptive names for the actor,
// location and flag 9; the 0x2626b0 call's test nested or as `!(a || b)` 9/9; the flag declared
// after the actor 9; the outputs declared before the scratch buffer 9. A no-/GL /FAs build orders
// these homes by weighted use count (names and declaration places never move them). Diagnostic,
// reverted: 0x2605d0 not passing the flag pointer on to the stubbed 0x260670 gives 36 with the
// flag's home moved, so the homes follow what LTCG knows of that callee chain (BLOCKED-BY 0x260670).
// Round 86 (all-image fast): call-only and two-use Boolean output pointers 9/9; one-element
// Boolean output array 9; count/Boolean declarations swapped 9; derived record declared before
// the node 9; search result declared before the outputs 9; node and derived declarations before
// the actor 9; const actor pointer 9. Every form has identical differences; original restored.
// Round 114 (lane 6): the 0x260670 stub writing its flag output first, as the disabled body does,
// 9 (plain; its callers unchanged), so a written output alone does not move the homes; 0x1f4a20's
// bool output declared at function scope 209 (975 bytes); the actor local for the equal-arm
// unknown040 store 256 (955 bytes); a fresh actor lookup for the unknown4ae store 176 (985 bytes);
// the prop position pointer named once for 0x1ac610 and 0x210ac0 227. Original body restored.
// Round 54 (lane 10, plain; fast is unusable here, control 209): result declared first 187; both
// 0x1f4a20 flags declared before the 0x262b40 result 9 (same code); the five cleared flags as one
// chained assignment 13; the result declared right before the search-result test 58; the 0x1af530
// range output declared at the top of the found branch 9 (same code).
// @retail 0x1af160
bool function_1af160(long arg_0, s_1af160 *arg_1)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_prop_node_view *local_1 = prop_node_get(local_0->prop_index);
    s_type_5cfb45 *local_2 = function_25d690((s_prop_datum *)local_1);
    s_reference local_3 = local_0->unknown418;
    dword *local_4 = (dword *)function_1e4be0(arg_0);
    bool local_6 = false;
    arg_1->field_7 = local_6;
    arg_1->field_a = local_6;
    arg_1->field_b = local_6;
    arg_1->field_28 = local_6;
    arg_1->field_8 = local_6;
    bool local_5 = true;
    s_object_marker local_7[32];
    short local_8[32];
    if (!*(bool *)((byte *)local_0 + 0x3c) && local_0->unknown227)
        local_6 = function_1ac610(arg_0, &local_2->position, local_7, local_8);
    s_prop_search local_9;
    memset(&local_9, 0, sizeof(local_9));
    local_9.type = 0;
    *(short *)((byte *)&local_9 + 0x69a) = 2;
    *(bool *)((byte *)&local_9 + 0x698) = true;
    local_9.unknown59 = local_4 && (*local_4 & 1);
    s_object_marker *local_10 = local_6 ? local_7 : NULL;
    *(s_object_marker **)((byte *)&local_9 + 0x64) = local_10;
    byte *local_11 = ai_scratch_buffer_get();
    s_261d20_entry local_12;
    long local_13;
    bool local_14;
    s_reference local_15 = function_261280(&local_9, arg_0, &local_12, &local_13, local_11, &local_14);
    if (REFERENCE_EQUAL(local_15, g_470fa0))
    {
        if (!local_0->unknown227)
        {
            arg_1->field_0 = 0;
            arg_1->field_6 = true;
        }
        else
        {
            actor_get(arg_0)->unknown040 = false;
            local_5 = false;
        }
    }
    else
    {
        bool local_17 = false;
        s_262b40_result *local_16 = function_262b40(local_15);
        bool local_18 = false;
        if (*(short *)((byte *)&local_12 + 0x5c) != 0 &&
            *(long *)((byte *)local_16 + 0x14) != NONE && *(long *)((byte *)local_16 + 0x14) != 0xffff)
        {
            local_18 = function_1f4a20((s_1f4a20_entry const *)&local_12, arg_0, local_15, (s_1f4a20_source const *)local_10,
                true, &arg_1->field_c, &arg_1->field_1c, &arg_1->field_28, &local_17);
            if (local_18)
            {
                arg_1->field_a = true;
                arg_1->field_b = false;
            }
            else if (local_17)
                local_15 = g_470fa0;
        }
        if (!local_17 && !local_18)
            local_15 = function_2626b0(arg_0, local_15, local_13, local_11, local_14, true);
        if (!REFERENCE_EQUAL(local_15, local_3))
        {
            real local_20 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 5.0f, 5.0f) * g_510c54->field_2_3;

            __asm
            {
                fld local_20
                fistp local_13
            }
            arg_1->field_0 = (short)local_13;
        }
        if (!REFERENCE_EQUAL(local_15, g_470fa0))
        {
            arg_1->field_4 = 0;
            local_0->unknown4ae = true;
            real local_22;
            if (function_1af530(arg_0, (long *)&local_22, &local_13) &&
                local_22 > function_210ac0((s_type_c3b527 const *)local_16, &local_2->position))
                arg_1->field_8 = true;
        }
        long local_23 = function_1e1f20(arg_0);
        if (local_23 != NONE)
        {
            bool local_26 = true;
            byte *local_24 = (byte *)function_1e5280(arg_0, ai_object_get(local_23)->definition_index);
            if (local_24)
            {
                if (*(real *)(local_24 + 0xc) > local_1->unknown28)
                    local_26 = false;
                else if (!REFERENCE_EQUAL(local_15, g_470fa0))
                {
                    point3f *local_28 = &function_25d690((s_prop_datum *)local_1)->position;
                    vector3f local_29;
                    vector3d_from_points3d(&local_12.point, local_28, &local_29);
                    if (length_sq3f(&local_29) < *(real *)(local_24 + 0xc) * *(real *)(local_24 + 0xc))
                        local_26 = false;
                }
                function_265c30(local_0->prop_index, arg_0, local_26);
            }
        }
    }
    ai_scratch_buffer_release(local_11);
    return local_5;
}
