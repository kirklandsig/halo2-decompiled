#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
// @flags /O2 /arch:SSE /Gr

struct s_1badc0
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0x5c - 0x26];
};

real normalize2d(point2f *arg_0);

// score 211 (was 211): frame 0x90 agrees; output EBX/EBP, point/vector homes and SSE scheduling remain.
// Earlier: Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain. Retail
// copies the input point a second time (at 0x28, through integer moves) for the two vector
// differences and keeps the center at 0x34; ours has the center at 0x28 and the to-center vector at 0x34.
// Round 90: radius read before the center copy 215 and the copied point as the actor-to-point
// input 211 (both kept); the copy as the point-to-center input 212, as both inputs 264; a second
// point copy for both 236 (1029 bytes); center/to-center names in four pairs 211; an early false
// return for the lookup result 213; the two differences in the other order 211; squared and
// projection terms in the other order 211/211.
// Delayed vertical zero 274; with center difference 271; native 2D perpendicular 286/285;
// complete aggregate 277/274; delayed zero and early three-use clearance 220; with center 259.
// Delayed zero and early clearance kept; other executable forms restored.
// Shared perpendicular 265; split squared/root distances 220; both 265; clean output with shared/split/both
// 282/279/282; shared perpendicular with coherent point source 225; with split distances 225.
// Staged second/first/both direction outputs 231/227/262; center-staged second alone/with first
// 240/273; copied-point staged second/both/center pair 231/262/276. Opening directions restored.
// Expanded radius and existing point-setter first/second/both combinations all 220;
// native squared-vector helper 273. Original point construction restored.
// Round 204: shared far-output store 252; native collision-test result 282; both 284;
// later adjusted-point copy 267; with shared far store/collision result/both 272/283/285;
// all with clean output access 301. Original output, spill and point lifetime restored.
// Output EBX/EBP, point/vector homes and SSE scheduling remain.
// No output self-address access 295; copied input/native object/both 281/295/281;
// shared collision storage alone/with input/object/both 295/282/295/282;
// copied input with entry/after-actor common result 287/287.
// frame 0x90 agrees; output EBX/EBP, vector homes and SSE scheduling differ.
// Vector/center/point/direction names 277 each; named clearance 277; shared actor-to-object difference
// 274; center memcpy 277; names/clearance/difference 274. Three-point gains rejected.
// Round 147: Planar scalar differences 255; complete projection products 220; both side-product
// sets 220; all squared terms 220; projection/side 220; planar/projection/side 255; all products
// 220; all products with planar differences 255.
// Round 150: Coherent first/second/both point inputs 215/217/264; second producer first with
// original/first/second/both 220/216/217/270; early uninitialized directions with both inputs 264.
// Inline collision provider on original/first/second/both inputs 220/215/217/264, identical with an
// explicit out-of-line declaration. Best five-point gain rejected; original body restored.
// Partial original-point input forms are exact-match-only; all trial bodies are restored.
// Round 174: center/radius/both per-use sources 291/242/296; complete actor-position snapshot
// alone/with center/radius/both 267/283/262/246; complete sources with clean output access 296.
// Original observations, early clearance, delayed zero, output access and scalar spill restored.
// Direction declaration/call ordering and collision-storage lifetime combinations 220 in seven forms;
// those together with radius before center copy 214 (six-point gain restored).
// Same-size point/vector name swaps, separately and in combinations, 220 in eight forms.
// Far planar separation through the existing 2D/3D helpers 228/228; complete
// native 2D/3D initializers 220/220; each with entry collision storage 228/220/228/220.
// All restored; original scalar geometry and collision-storage lifetime remain.
// Round 191: Direct-output mode first with original returns, Boolean/long/byte
// common completion results 261/261/236/242; all four with radius before center
// 261/261/236/242. Original mode/result graph and executable body restored.
// Round 345: Native original/center/both CRT copies 220/220/220; entry point
// storage alone/with original/center/both copies 218/218/218/218; both copies
// and entry storage with radius before center 213 (seven-point gain below gate). All restored.
// Round 194: original-mode common bool/byte/long completion 248/274/262;
// native query-test completion 274; Boolean/query-test forms with radius first
// 243/274; common Boolean with entry points 248; query-test completion
// with entry points and radius first 274. Original result graph restored.
// Round 254: entry collision-sentinel initialization with original/first-point/center/both
// entry storage 269/269/269/269; with radius-before-center 269/269/269/269.
// Original late collision initialization, point storage and executable body restored.
// Round 195: Normalizer inline/noinline/combined/forceinline declarations 220 each;
// with radius before center 215 each. Five-point best restored; original output access,
// scalar spill, radius lifetime and body retained, output/vector homes unresolved.
// Round 23: direct const/nonconst parameter reads 279 each; separate output pointer 279;
// nonconst output reference and actor-first order 220 each; far/near/both output point
// setters 226/262/262; actor-first with near setter 262. Original body restored.
// Round 259: Collision-branch-only completion results false-first bool/byte/short/long
// 269/247/275/276; true-first 262/258/258/260. Direct-output exits unchanged.
// Original result graph and entire body restored; output EBP and vector homes remain.
// Round 240: Complete final offset through the existing shared scaler 258; with
// radius first/copied point inputs/both 258/259/259; entry point storage alone/
// with radius/with copied inputs/all 256/256/259/259. Original body restored.
// Round 249: Native object receiver retaining output access with original/early radius
// and aggregate/CRT center copies 220/215/220/215; all four with read-only actor
// 220/215/220/215. Five-point radius lead below gate; original body restored.
// Round 251: Multi-use actor-position receiver with original/entry point storage and
// radius-order combinations 299/299/299/299; consistent current actor-position lookups
// with those combinations 307/307/306/306. Original receiver and full body restored.
// Round 253: successive squared sums/projection/both side sums/both squared-projection/
// squared-side/projection-side/all/all plus radius first 264/220/220/264/264/220/264/264.
// Original complete expressions and executable body restored.
// Round 256: shared final scaled add/far sum/planar center-then-displace subsets
// 220 each; full producers with radius first 215 (five-point form restored).
// Output EBX/EBP and point/vector homes remain; frame 0x90 agrees.
// Round 258: Native planar initial first/second/both directions 220/225/225; with radius first
// 215/220/220; both with entry point storage 223, with radius 218. Five-point best restored;
// output EBP and original 0x90 frame/direction homes remain.
// Round 260: distinct first/second/both perpendicular storage declared at entry or geometry
// scope 220 each; both at entry with radius first 215; all direction/perpendicular storage
// at entry 220. Five-point radius gain below gate; original declarations and body restored.
// Round 263: existing state carrier conditional/branch-owned initialization, crossed with
// entry point storage and early radius, 266/266/264/264/260/260/258/258. Original stores, homes and body restored.
// Round 265: clean output with bool/long common completion, entry-point storage and
// radius-order combinations 279/279/279/279/285/285/285/285. Original complete body restored.
// Round 268: plain component loops far/adjusted/both with long ordinals 220/220/220;
// short ordinals 246/286/284; both byte/dword 284/220. Original scalar stores restored;
// output EBX/EBP, vector homes and return tails remain.
// Round 271: full collision output with the shared native head as member/base, late/entry
// storage, 220 each; all four with radius before center 215 each (five-point gain restored).
// Original opaque full output, output access, scalar spill and complete body restored.
// Round 273: uninitialized squared-separation/projection/adjusted-distance declarations
// at entry, seven subsets 220 each; all with the clearance declaration at entry 220.
// Original scalar declarations restored; output EBX/EBP and vector homes remain.
// Round 269: displacement workspace 260; separation workspace 211; both 260; direction-scale workspace 251;
// displacement/scale 207; separation/scale 251; all 207; all with entry point storage 207.
// Four-point best below the retention margin; original executable body restored.
// Round 430: Native parameter-role names, all seven index/point-output/optional-result groups, then
// all with collision storage/result names: eight forms 211 at 1004/1025. Original identifiers and
// entire executable body restored.
// J198: existing complete point/word getter, crossed with entry point/collision storage
// and coherent input, 296/296/296/296/300/300/300/300. Scalar output cast exact-only; all nonmatching forms restored.
// Output EBX/EBP, vector homes and geometric schedules remain.
// Round 302: Clearance after successful normalization, with shared expanded radius,
// distinct squared/root length and existing scaled-add subsets: all eight 270.
// Original clearance lifetime, output access and body restored; output/vector homes remain.
// Round 29 (fast): a second copy of the input point, after the center, with the to-center vector
// computed in place over it (as retail's stores show) 235, as local_6 only 229, taken first 239; with
// an actor-to-center vector computed before the first test and used in both later tests 226; on that,
// the squared length written i-then-j 222, the 3D squared length i, j, k 219, both 215 (retail's 0x28
// copy / 0x34 center layout and vector sources; output still EBX, 1032 bytes); product operand and
// sum orders in the two dot tests 215-226 (canonicalised); the arg_5 path as an else with one final
// false return 215 (same code); without the output self-address read 238.
// Round 46: retail's 0x28 slot is one variable: the point copy is stored there and the
// point-to-center vector is written over it component by component (copy y stored after the
// vector's i). A to-center vector copied from the point and differenced in place puts the
// center at retail's 0x34 and aligns the middle block, but scores 235; with a named
// actor-to-center vector after/first/between the point vectors 232/226/226 (sum and product
// operand orders then mirror retail's); separate origin copy for both vectors 229. Restored.
// Round 4 (lane 18, fast): the center local renamed to sort after the to-center vector 211 (slots
// unchanged: center stays at 0x28); both vectors declared ahead of the point copy 211; the arg_5 path
// as an else with one final false return 211; the point copy renamed to sort last 211; the point copied
// after the center and radius 224 (1006 bytes). Restored.
// Round 37 (lane 1): the dead pathfinding-data store, the record with its NONE word and the direct
// 0x26d100 call are 0x26d0e0 inlined with 0x1fa7f0's result as its third argument (the form matched
// 0x1b13b0 and 0x25b6b0 call out of line); written as that call 298 plain (975 bytes: our LTCG inlines
// 0x26d0e0 but drops the third argument's [esp+0xc] store, and this function's convention changes to
// arg_0 on the stack and ret 0x10, so matched callers 0x1ba5c0/0x1bdad0/0x1bff80 stop matching). Fast:
// a result local set true before the out-pointer test 278 (the same convention change); the center and
// radius through s_slot_object_view's unknown030/unknown03c 211 (same code); `2.0f - distance` inside
// the normalize-success block, where retail computes it, 270.
// Round 247: complete input snapshot across both real modes, with native/library copy
// and phase/entry collision storage, 255/255/255/255; the same four without output
// self-address observation 250/250/250/250. All disturb two matched callers in the
// reduced image. Original input observations, output access, scalar spill and body restored.
// @retail 0x1badc0
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6)
{
    s_type_c3b527 *const *output_reference = &arg_3;
    arg_3 = *output_reference;
    s_actor_view *local_0 = actor_get(arg_0);
    if (arg_6)
        *arg_6 = false;
    if (!arg_5)
    {
        point3f local_1 = *arg_2;
        byte *local_2 = (byte *)object_get(arg_1);
        real local_4 = *(real *)(local_2 + 0x3c);
        point3f local_3 = *(point3f *)(local_2 + 0x30);
        vector3f local_5;
        vector3d_from_points3d(&local_0->position, &local_1, &local_5);
        vector3f local_6;
        vector3d_from_points3d(arg_2, &local_3, &local_6);
        real local_7 = local_5.j * local_5.j + local_5.i * local_5.i;
        if (local_7 > 0.0f)
        {
            real local_8 = ((local_3.y - local_0->position.y) * local_5.j +
                (local_3.x - local_0->position.x) * local_5.i) / local_7;
            if (local_8 > 0.0f && local_8 < 1.2f)
            {
                point2f local_9;
                local_9.x = 0.0f - local_5.j;
                local_9.y = local_5.i;
                if ((local_3.y - local_0->position.y) * local_5.i +
                    (local_3.x - local_0->position.x) * local_9.x > 0.0f)
                {
                    local_9.x = 0.0f - local_9.x;
                    local_9.y = 0.0f - local_9.y;
                }
                if (normalize2d(&local_9) > 0.0f)
                {
                    local_1.x = local_9.x * (local_4 * 1.1f) + local_3.x;
                    local_1.y = local_9.y * (local_4 * 1.1f) + local_3.y;
                    vector3f local_10;
                    vector3d_from_points3d(&local_0->position, &local_1, &local_10);
                    real local_11 = local_10.k * local_10.k + local_10.j * local_10.j + local_10.i * local_10.i;
                    if (local_11 > 0.0001f && local_11 < 4.0f)
                    {
                        local_11 = (real)sqrt(local_11);
                        vector3f local_12;
                        local_12.i = 0.0f - local_6.j;
                        local_12.j = local_6.i;
                        if (0.0f > local_10.j * local_6.i + local_12.i * local_10.i)
                        {
                            local_12.i = 0.0f - local_12.i;
                            local_12.j = 0.0f - local_12.j;
                        }
                        local_12.k = 0.0f;
                        real local_13 = 2.0f - local_11;
                        if (normalize2d((point2f *)&local_12) > 0.0f)
                        {
                            local_1.x += local_12.i * local_13;
                            local_1.y += local_12.j * local_13;
                            local_1.z += local_12.k * local_13;
                        }
                    }
                    if (arg_6)
                        *arg_6 = true;
                }
            }
        }
        byte *local_14 = (byte *)g_4e0348;
        volatile long local_15 = 0;
        if (*(long *)(local_14 + 0xc4) > 0)
            local_15 = *(long *)(local_14 + 0xc8);
        s_1badc0 local_16;
        local_16.field_24 = NONE;
        long local_17 = function_26d100(g_4687b0, (long *)arg_3, (s_collision_result_1697c0 *)&local_16, &local_1);
        if (local_17 != NONE)
        {
            if (arg_4)
                *arg_4 = local_17;
            return true;
        }
        return false;
    }
    real local_18 = local_0->position.x - arg_2->x;
    real local_19 = local_0->position.y - arg_2->y;
    real local_20 = (real)sqrt(local_19 * local_19 + local_18 * local_18);
    if (local_20 > 2.0f)
    {
        arg_3->point.x = arg_2->x + g_4687b0->i;
        arg_3->point.y = arg_2->y + g_4687b0->j;
        arg_3->point.z = arg_2->z + g_4687b0->k;
        arg_3->output_index = NONE;
        if (arg_6)
            *arg_6 = true;
    }
    else
    {
        arg_3->point = *arg_2;
        arg_3->output_index = NONE;
    }
    return true;
}
