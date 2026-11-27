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

// score 965 (was 965): m3 lane 11 r158 unchanged (its line just above the marker); the frame is 0x11d4 against retail's 0x11cc, so every later slot sits at another offset;
// the head's registers differ (retail: base in EDX, local_6 in ECX, arg_3 in EDI; ours: base in ECX, local_6 in
// EBX, arg_3 in ESI) and the record and surface pointers' stack homes are exchanged.
// MiniMax D (11-20): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc, so every later slot sits
// at another offset, and the head's registers differ (retail: base in EDX, local_6 in ECX, arg_3 in EDI); the 0x48e70 call sets
// the rest and its definition is in another machine's file: BLOCKED-BY 0x48e70.
// M3 lane 15 r124 (H2M_FAST with the callers 0x9cca0/0x184060, both MATCH each run; control 965): the start face
// parameter read through its address for the group read 965 (same code); the surface list declared ahead of the record
// pointer 965 (same code); the record pointer read through a const reference at its two tests and calls 984 (4573 B);
// the cached axis locals typed `long` 982 (4605 B); the surface list read through a const reference at every use 965
// (4573 B, other code, the head unchanged). Body unchanged.
// MiniMax A (11-20): no new form, no lower score (965); the frame is still 0x11d4 against retail's 0x11cc, so the worklist array and every later slot sit at another offset, and retail carries the start surface in edi where this build carries it in esi; the 0x48e70 call sets the rest (BLOCKED-BY, its definition is in another machine's file).
// Earlier score line: 965 (was 965): the frame is 0x11d4 against retail's 0x11cc (the two cached axis shorts), the head's registers and the
// record and surface pointers' homes differ (retail keeps both in memory); earlier rounds below, this round's at the end.
// MiniMax H (11-18): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc, and the
// record and surface records still sit in each other's stack homes where retail keeps the record pointer at +0x98 and
// the surface list at +0xe0/+0xe4 from the start; dropping the two cached axis indices gives retail's frame and
// retail's length but scores 976, because the first difference moves later and the homes stay different; the 0x48e70
// call, whose body is in another machine's file, still decides the register rotation.
// MiniMax H (11-18): no new form, no lower score (965): the first difference is still the frame (ours 0x11d4,
// retail 0x11cc) and the record pointer's home (ours at +0x64, retail at +0x98 with the slot zeroed at the start),
// with the surface record in edi where retail keeps it in esi; the two cached axis shorts are what the eight bytes
// are made of, and reading them from g_440b94 at their use gives retail's frame and length but scores 976, so the
// body stands: BLOCKED-BY 0x48e70, whose body is in another file.
// MiniMax E (11-11): no new form, no lower score (965); the frame is 0x11d4 against retail's 0x11cc and the two
// cached short axis locals are what costs the eight bytes - the form that drops them matches retail's frame and
// length but scores 976 because the first difference moves earlier, so the 965 body stays.
// MiniMax I (11-10): no new form, no lower score (965); the first difference is still the frame size (ours
// 0x11d4 against retail's 0x11cc) together with the scene and surface records sitting in each other's stack
// homes, and that rotation depends on 0x48e70 in another lane's file.
// MiniMax G (11-11): no new form, no lower score (965): the first difference is at +0x7 - ours loads 0x11d4 where retail loads 0x11cc and keeps the argument in esi where retail moves it into edi; the body length agrees (4590 bytes both).
// MiniMax A (11-13): no new form, no lower score (965); the frame is still 0x11d4 against retail's 0x11cc —
// the two cached short axis locals cost those eight bytes — and the definition and surface records still sit
// in each other's stack homes.
// MiniMax B (11-14): no new form, no lower score (965); the gap opens in the first block, where retail loads the base pointer into
// EDI and keeps the computed offset in EAX while ours uses ESI and EBX, and retail clears the frame dword with a single store
// where ours zeroes a register first; the extent is retail's 4590 bytes, so only the block layout differs.
// MiniMax S (11-14): no new form, no lower score (965): the two type changes that give retail's frame and
// length were measured last round at 976, above the score here, so they were not taken again; the body was
// re-measured after the comment edits and did not move.
// MiniMax I (11-17): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc, and the
// two type changes retail's bytes show (the axis values reloaded from g_440b94 and the full-width vertex counters)
// were measured at 976 last round, above the score here, so they were not taken again; the length is retail's 4590
// bytes, so only the block layout and the stack homes differ.
// Earlier 965 (was 977): the frame is 0x11d4 versus retail 0x11cc, and the scene and surface homes differ.
// A statement walk found no behaviour difference. Every condition, field,
// callee and argument order agrees. Not kept, since together they score 976: two type changes retail's bytes show (1) no cached axis locals - retail
// reloads g_440b94[idx][0/1] in the cell loop (0x185171/0x185178) and keeps the pre-scaled offsets only as loop-invariants in
// the vertex loop (0x1847e2); (2) full-width vertex counters (retail's count at [esp+0xb8] is a dword, pushed whole at
// 0x185241, with pointer-stepped vertex stores at 0x18497a). Together they give frame 0x11cc and 4590 bytes as retail (the
// 0x11d4 excess of the older rounds below came from the cached short axes), but score 976 because the first
// difference moves from +0x7 to +0x10 and every later stack home still differs. The code here keeps the short axes. Still open: retail keeps the record/surface
// pointers at [esp+0x98]/[esp+0xe0]/[esp+0xe4] from the start; retail tests the plane flag as movsx + `test dh,dh` (ours
// zero-extends); retail's point/plane transforms read scalars (no vector copy); retail stores the four cell bounds as words
// at +0x3c..+0x42. On the 976 form (opcode-normalized similarity vs retail, base 0.724 -> 0.741): bool local_29 (retail's
// sete/test al at 0x184834) 1001; face bounds at the top of the iteration 997; both 993; plus plane transform on scalars
// 967 (4567 bytes, frame 0x11c4); plus point scalars too 1007 (frame 0x11bc); point scalars alone 1028; test as `< 0` 1003;
// long loop counters for the cells 993; cell bounds as a short rectangle 984/994; surface lookup as one && test 976;
// record/list declarations swapped 976.
// MiniMax S (11-10): no new form, no lower score (965); the frame is 0x11d4 against retail's 0x11cc, the definition and listener records sit in different stack slots and the falloff and bounds values are kept in separate homes.
// MiniMax B (11-04): no new form, no lower score.
// Earlier (965, was 977): the same frame and homes.
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
// Round 50 (lane 1, fast): arg_0 test as `!= NONE` with the record arm first 986; NULL record set
// before the surface pointer 965; worklist entry stored before its count 965; both 965; the record
// and worklist-count locals renamed to sort first/last 965 each with every stack home unchanged.
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
// Round 49 (lane 10, fast): the surface lookup as one `&&` test with the table pointer declared before it 962
// (three-point gain, not kept); on it: the per-face bounds initialized at the top of the face loop 974; the plane
// transform reading the normal into scalars (i, j, k) 975 (4557 bytes, frame 0x11cc as retail), captured k, j, i
// 996 (4541), j, k, i 975 (4557); scalars plus face-top bounds 1021 (4538). Body restored.
// Round 484: native packed plane index/orientation and Boolean edge-side, subsets
// index/flag/side/index+flag/index+side/flag+side/all/signed flag
// 965/982/981/982/981/982/982/964. Last form 4602 bytes (retail 4590), one-point gain
// rejected. Eight-byte record asserted in each typed form;
// frame excess remains. Original surface declaration and complete body restored.
// Round 487: actual matrix-pointer lifetime 974; full-width bounded polygon count 1021; both 1035;
// native projected box2f 965; box2f/matrix pointer 974; box2f/count 1021; all three 1035; all three with
// native world box3f initialized at its first store 1053. All eight forms restored; frame 0x11d4 versus
// 0x11cc and initial surface/transform homes remain.
// Round 49 (lane 10, fast): the surface lookup as one `&&` test with the table pointer declared before it 962
// (three-point gain, not kept); on it: the per-face bounds initialized at the top of the face loop 974; the plane
// transform reading the normal into scalars (i, j, k) 975 (4557 bytes, frame 0x11cc as retail), captured k, j, i
// 996 (4541), j, k, i 975 (4557); scalars plus face-top bounds 1021 (4538). Body restored.
// Round 245: repeated request position/direction/location snapshots, seven subsets:
// 1025/1005/1100/992/1076/1056/1124; all three acquired after admission 1112.
// Location captures raise caller 0x9cca0 from 9 to 12; all eight regress.
// workspaces, transforms, random sequence and complete executable source; frame/homes remain.
// DH round 268: complete dot-input vector/point/both captures 1018/1035/1081;
// paired polygon projections through the same dot producer, original/vector/point/both inputs
// 977/1020/1133/1236; complete product parts 1236. Original dot helper and body restored.
// Round 455: Existing point/plane input capture through native field initialization,
// point/plane/both, memcpy point/plane/both, memmove point/plane:
// 1021/1007/1022/965/965/965/1109/1039. Both field inputs give frame 0x11cc at 4506/4590 bytes; regresses.
// Original native copies, transform helpers and complete body restored.
// Round 458: Actual external face-plane/polygon-vertex transfers, native/memcpy/memmove
// pairs excluding native/native: 965/1021/965/965/1021/989/989/1027. Original source transfers and
// existing in-place transforms restored; frame 0x11d4 versus 0x11cc remains.
// Round 463: complete normalizer output publication, scalar/native early result, direct/scalar/native
// common result, then early scalar/common direct/common native with inverse declared first:
// 965/1087/988/988/1062/965/988/1062. Original helper restored; frame excess remains.
// Round 249: Matrix input snapshots in existing point/plane transforms: point matrix 1110, plane
// matrix 1152, both 1248; reused point scale 992, with plane matrix 1129; complete point
// basis/translation 1199, plane 1181, both 1398. Original helpers restored; frame excess remains.
// Round 318: Existing point/plane transforms return completed native values: point 1024, plane
// 1016, both in ijk/ikj/jik/jki/kij/kji producer orders 1086 each.
// Original native input copies, in-place helpers and full body restored; frame 0x11d4 versus
// 0x11cc remains.
// Round 465: complete plane input before orientation, native/memcpy/normal-distance/
// normal setter-distance, at face/entry scope: 987/987/966/973 twice.
// Original split plane producer and body restored; frame and geometry homes remain.
// Round 523: Scalar plane-transform input with complete scalar/native normalizer capture,
// early/common result and original/scalar point input, 999/1080/1011/1052/1060/982/1033/976.
// First form recovers frame 0x11cc and first +0x1d but regresses; original in-place inputs,
// helpers, projection lifetimes and complete body restored.
// P263: existing dominant-axis provider 1057; six persistent instance/geometry/material
// pointer priorities 965/965/965/965/965/965; first/last with late worklist 965/965.
// Original body and declaration restored; excess frame and geometry homes remain.
// DH285: Joint projection-origin/perpendicular-origin/grid-spacing scalar lexical
// priorities, six, then two with uninitialized face-owned spacing: 965/965/965/965/965/965/965/965.
// Original complete body restored; eight-byte frame excess and initial geometry homes remain.
// M3 lane 4 r155 (fast; also LCS-aligned counts, base 710): retail stores the four projected cell bounds as one packed
// short rectangle (word stores at +0x3e/+0x3c/+0x42/+0x40 for left/top/right/bottom, zeroed bottom/right/top/left). The bounds
// as the existing short_rectangle2d (fields left, top, right, bottom assigned, loops over top..bottom / left..right) 1007
// (4630 bytes, aligned 709; ours then packs it at +0x2c, frame still 0x11d4), with the zeroing in retail's order 1007 (chained
// or as four statements), declared at function scope 1007 (aligned 709); the per-face projected bounds initialized by four
// statements 969 (4595 bytes, aligned 718). Re-measured: record/surface declarations before the instance pointer 965, surface
// before record 965, rectangle plus that order 1007.
// Retail keeps the record pointer (+0x98) and surface pointer (+0xe4) in memory from the start; ours holds them in EDI/EBX.
// AC485: complete plane-transform input including distance, normal-plus-distance/native/scalar/memcpy,
// direct/native output: 965/1011/999/1011/1046/1041/981/1041; original in-place helper restored.
// Frame and geometry homes remain; no other helper user exists.
// AC489: Plain component traversals for normalization output, point input scale and world
// bounds expansion, subsets 1/2/4/3/5/6/7: 984/986/969/987/969/991/971; complete graph
// with short counters 1093. Original helpers and body restored; frame and geometry homes remain.
// MiniMax G (11-14): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc and the
// initial surface pointer is still held in ESI where retail holds it in EDI; the remaining difference is set by the
// 0x48e70 call, whose definition is in another machine's file.
// m3 lane 11 r60 (fast 965): record/list pointer declarations swapped 965; both chosen by ternaries 998
// (4608 bytes); NULL/list defaults then `if (arg_0 != NONE)` 978; 0x23a220 with its two point parameters
// swapped (retail's three callers set the single point up in EBX before the polygon in EDX; definition and every
// call) 965, 0x23a220 still matched, 0x173910 69 -> 70, so not kept; point/plane arrays declared in the other
// order 965; the per-face bounds pair set field by field 969 (4595 bytes).
// AC495: Existing shared plane decoder with native polygon-count/edge-pair/selector carrier
// masks 0/1/2/4/3/5/6/7: 969/1021/985/988/1044/1022/981/1031. Original decoder,
// declarations, carriers and complete body restored; excess frame and initial geometry homes remain.
// m3 lane 11 r63 (fast): the two-bounds aggregate initializer as four stores 969 (4595 bytes); the vertex and
// planar arrays declared inside the loop 965; short queue count/cursor 969 (4595 bytes); the world bounds
// declared after the definition lookup 965; the four arrays declared in reverse 965. Frame stays 0x11d4 in all;
// original declarations restored.
// Round 127 (lane 8, fast): record/surface pointers as two ternary initializers 998 (4608 B); axis selection as
// if/else-if 965; vector copies, dots and bounds loop in an inner block 965; selector incremented when positive 976;
// one falloff scalar shared by both force arms 994 (4564 B, frame 0x11cc as retail, first difference +0x1d).
// AC497: Eligible effect, created resource and accepted sample regions, subsets
// 1/2/4/3/5/6/7, then origin-owned zero-spacing fallback: 965/965/965/991/965/965/991/991.
// Original admission, copies, random order and body restored; frame and geometry homes remain.
// Round 130 (m3 lane 8, fast): the 2D sample point assigned by fields 965 (same code); both direction copies
// before the two plane-distance calls 965 (same code); the force record pointer before the velocity copy 969
// (4595 B); the four arrays declared in reverse order 965 (same code); the sample point copied after its two
// random offsets 966. Ours still uses every slot to +0x1e0 where retail ends at +0x1d8 (frame 0x11d4/0x11cc).
// Round 144 (m3 lane 6, fast; [LCS-aligned] this body [938]): the worklist seeded before its count 965, the world
// bounds initialized after the seed 965, the neighbour test with the group compare first 965 (same code each); the
// per-face projected bounds initialized by a two-pass loop 980 [1026], that loop at the top of each face iteration
// 997 [975]. BLOCKED-BY 0x48e70: in the cell-bounds block retail spills the inverse spacing (+0xd8) and reloads it
// after every 0x48e70 call, the pattern of a caller that does not see the callee's register usage (the 0x22750
// effect on 0x22cf0, reproduced there by `inline` on that definition); ours keeps it in XMM2. 0x48e70's definition
// is in machine 1's src/unknown_02b400.cpp.
// Round 109 (m3 lane 12, fast; 965): the plane flip test as `field_0 < 0` 984 (4573 B, `test cx, cx; jge`); the flag
// word read once into a long local for the index and the flip test 974 (gives retail's `movsx` load, registers move);
// the recorded axes-at-each-use and long counters form re-measured 976 (frame 0x11cc), with that long flag word 992
// (4593 B); the first record as an element pointer before its field read 965 (same code); the bounds compare written
// count first (retail loads the count into EBX before the compare) 966. The file-scope
// inline_depth pragma pair around this function removed: identical code (kept as a cleanup).
// MiniMax D (11-14): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc; dropping
// the 2D sample array to seven entries matches the frame size but reaches only 964 and needs a declaration change,
// and the remaining difference is set by the 0x48e70 call, whose definition is in another machine's file.
// MiniMax D (11-11): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc and the
// initial surface pointer is still held in ESI where retail holds it in EDI; the remaining difference is set by the
// 0x48e70 call, whose definition is in another machine's file.
// M3 lane 10 r106 (fast; masked aligned count in brackets, base 454): retail's normalizes square each component with
// memory operands (`fld [k]; fmul [k]`, sum k, j, i) and keep the root on the x87 stack for fabs; the double-product
// helper here loads each component once and pops three leftovers. The zero-velocity copy and the force-record lookup
// ahead of the sample normalize (retail schedules both inside its x87 block) 981 [430]; both normalizes through the
// existing normalize_inline 982 [414] (4563 B; its squares come k, i, j); the helper as `(real)sqrt((double)(k*k + j*j +
// i*i))` with the scale arm first 990 [415] (4563 B; the compile-only build gives retail's x87 block exactly); reorder plus
// normalize_inline 958 [407] (4590 B, 994 instructions as retail; seven points, not kept); that plus the axis table read
// at each use 967 [374] (frame 0x11cc, first difference +0x1c); plus a long vertex counter 1006 [396] (4622 B); plus a
// long bounds index 991 [383]; reorder plus the float-sum helper 963 [393] (4596 B); reorder, normalize_inline and the
// per-face bounds at the face-loop top 969 [412], the same on the axis form 969 [365]; reorder, normalize_inline and a
// bool edge side 963 [397]. The positional count stays near 965 because the first block's registers shift every line.
// MiniMax R (11-15): no new form, no lower score (965): the frame is still 0x11d4 against retail's 0x11cc, so the
// worklist array sits at +0x1e0 where retail's sits at +0x1d8 and the interior slot layout differs throughout; the
// initial surface pointer is still carried in ESI where retail carries it in EDI, and the surface list pointer in EBX
// where retail carries it in ECX; the negated plane is still computed in the registers where retail stores it and reads
// it back. New forms tried and dropped: the search condition with the element test first (971), the permuter's four
// forms (969 with the cached worklist entry dropped and the sample store moved), the negation in the other order (964,
// one point), and the prefix increment and the zero comparison (no change).
// MiniMax S (11-15): no new form, no lower score (965): retail's stack reserve is 0x11cc against our 0x11d4 and the scene and
// geometry homes are rotated, so every later load sits at a different offset; the endpoint, counter and width forms of the
// earlier rounds all score 965 or worse.
// MiniMax I (11-16): no new form, no lower score (965): retail's stack reserve is 0x11cc against our 0x11d4, so the
// worklist array and every later slot sit at a different offset and the scene and geometry homes are rotated; the
// earlier rounds' form that does reach retail's frame (the axes read at each use with full-width vertex counters)
// scores 976, and the endpoint, counter and width forms of those rounds score 965 or worse.
// MiniMax I (11-17): no new form, no lower score (965): the first difference is still the stack reserve (retail
// 0x11cc against our 0x11d4), so the worklist array and the scene and geometry homes sit at other offsets and retail
// carries the start surface in edi where this build carries it in esi; the earlier rounds' form that does reach
// retail's frame (the axes read at each use with full-width vertex counters) scores 976, and the endpoint, counter
// and width forms of those rounds score 965 or worse.
// M3 lane 15 r97 (fast with the callers 0x9cca0/0x184250 and the callees, all matched throughout; LCS-aligned count in
// brackets, base 965 [549]): retail never gives the record or the surface a callee-saved register (the record is read from
// +0x98 at both transforms, the surface is reloaded into ECX at the top of each face and its edge and vertex arrays are
// fetched just before the vertex loop), where ours hoists both loads above the face loop into EDI/EBX. The selection through
// an inline helper with two output pointers 965 [552] (4573 B; the indirection is removed and both stay in registers); the
// face array in a per-face local used for the face and the neighbour test 965 [551] (same head); the edge and vertex arrays
// in locals before the vertex loop 984 [551] (4573 B), with the face-array local 978 [545]; the worklist cursor declared
// before the world-bounds loop with the loop as a while 958 [548] (retail's `xor eax, eax` before the six bounds stores and
// its entry-then-count stores; a moved declaration, so match-only), the same with the count assigned before the entry 958
// (same code); that with the lookup as one `&&` test and the table pointer declared first 955 [543] (the table load then
// sits above the tests where retail loads it after them), with the table global written at both uses 971 [542].
// MiniMax G (11-20): no new form, no lower score (965): retail's stack reserve is 0x11cc against our 0x11d4, so the worklist array and every later home sit at a different offset, and the scene and geometry homes are still rotated from retail's (retail carries the surface pointer in edi and the surface list in ecx, where this build carries them in esi and ebx).
// MiniMax C (11-17): no new form, no lower score (965): retail's stack reserve is 0x11cc against our 0x11d4, so the worklist array and every later slot sit at a different offset, and the scene and geometry homes are rotated (retail carries the surface pointer in edi and the surface list in ecx, where this build carries them in esi and ebx); the earlier rounds' form that does reach retail's frame (the axes read at each use with full-width vertex counters) scores 976, and the endpoint, counter and width forms of those rounds score 965 or worse.
// 463: shared two-axis projection at vertices/samples/both 1034/1056/1033, or with long
// vertex traversal 1037/1083/1037. Both with Boolean orientation 1037, direct table
// reads through the coordinate setter 1007, advancing 3D vertex pointer with direct
// reads/shared projection 992/1033. NOT kept: the 992 form recovers the frame but
// shortens the body and loses a caller match in the reduced image. All scores regress.
// M3 lane 1 r122 (fast with the callers 0x9cca0/0x184250 and the callees, all MATCH throughout; LCS-aligned 711 on
// this body): the worklist cursor declared before the world-bounds loop with the loop as a while re-measured 958
// (aligned 710; seven points and a moved declaration, not kept). On that 958: the record defaulted to NULL at its
// declaration with only the list chosen in both arms 959; the group count read into a local for the range test 958;
// the first face through an element pointer for its group read 958; the seed stored before the count 958 (same code
// each); the record through a block local with the list computed from it and the record assigned last 967 (4573
// bytes). Read in the entry block: retail takes the start face (arg_3) in EDI, clears the record's home with an
// immediate store and never holds the record or the list in a callee-saved register, where ours takes the face in
// ESI and keeps the record in EDI and the list in EBX; none of the five forms moves that.
// M3 lane 6 r203 (fast with the callers 0x9cca0/0x184060, both MATCH throughout; control 965, aligned 711/705): dead-local
// scan after the early return, K = 1, 2, 3, 5, 7 ... 31 (diagnostic, removed): the frame stays 0x11d4 (first difference +0x7)
// at every K, so the 8 extra bytes are not a symbol-count state; lengths 4557-4595 B, scores 964 (K = 7, 29) to 991. Tried:
// the two plane offsets through the existing dot3f in place of the private dot helper 984 (4573 B); the six clamps through
// the existing PIN_126660 965 (same code); the start face through a pointer local for local_7 (retail forms that address
// with its own lea) 965 (same code: folded); the record and list chosen by two conditional expressions 998 (4608 B).
// M3 lane 13 r117 (fast, control 965): everything after the definition lookup as a new forceinline inner function
// taking the looked-up pointers 965 (same code); the same split right after the record/list choice 976 (4575 bytes);
// the entry surface through an element pointer (retail forms its address with a lea and reads +6 through it) 965;
// the block count named before its compare (retail loads it into EBX first) 965; both 965; the group entry through
// its own element address 965 (the four are the same code: the pointers are folded away). Body unchanged.
// M3 lane 1 r131 (fast with 0x9cca0/0x184250; register-masked LCS-aligned counts ours/retail in brackets, base
// [278/272]): a dead-local scan (K = 1-8, 10, 12, 16 initialised longs at the top, diagnostic) keeps frame 0x11d4 at
// every K (983/991/980/1011/976/976/964/965/990/980/965), so the frame is not a symbol-count state. Forms: the two
// projection axes read from g_440b94 at each use with no axis locals (short counters kept) 974 at 4595 B [251/244]:
// frame 0x11cc as retail, and the NONE record is then an immediate store to its home as retail's; on it the start
// face through an element pointer 974 (same code); on it the per-face projected bounds declared at the face-loop
// top 987 [249/241] (their four stores land at +0xa0..+0xac before the flip test, as retail's); on that the face's
// flag word read once into a long for the plane index and the flip test 986 at retail's 4590 B [249/234] (retail's
// `movsx` + `test dh, dh`; the loop top is then retail's instruction for instruction but for registers: the list
// in EDI for retail's ECX, the plane array in ESI for EDI, the face index in EDX for ESI, and the homes); on that a
// long vertex counter 1020 (4625 B, and 0x9cca0 stops matching in the fast image). Left on the 986 form: the entry
// takes the start face in EBX (retail EDI) and the list in EDI (retail ECX, homed at +0xe4). Body unchanged.
// m3 lane 4 r224 (one-object LTCG screen linking the reduced build's objects and the ballast: 0 rows from the full
// build, 709 aligned from retail, masked 277): the record and surface pointers held in one local aggregate 712 (masked
// 274; each alone the same code: they are still enregistered); the axes read from g_440b94 at each use (frame 0x11cc as
// retail) 974 at 4595 bytes (aligned 682, masked 251); the flag word read once into a long for the index and the flip
// test 974 (4590 B, aligned 711); with the axes at each use 970 (aligned 688); the axes plus the per-face projected
// bounds initialised at the top of the face loop 987 (4595 B, aligned 675); all three 986 (4590 B, aligned 681; 670 in
// the screen); full-width vertex counters alone 721 aligned in the screen, with the axes 684, with the axes and bounds
// 683, all four 681. Every evidence form lowers the aligned count but raises the positional one (the head's registers
// stay: arg_3 in ebx/esi where retail takes edi, g_4e0348 in ecx where retail uses edx, the record and surface
// pointers kept in edi/ebx where retail keeps them in their stack homes). Body unchanged.
// M3 lane 11 r131 (fast with the callers 0x9cca0/0x184060, both MATCH unless said; aligned / register-and-slot-masked
// counts in brackets, this body [710/278]). The forms retail's bytes show were combined instead of measured one by one.
// Alone on this body: axis table at each use with a bool sign 988 [688/254]; long vertex count and bounds counter 1008
// (the callers lose their match: arg_0 moves to a register); bool edge side 981; per-face bounds at the top of the face
// loop 977; plane transform on three scalars 978; plane word in a long 974; cell bounds as short_rectangle2d 1007.
// Together: the first and the plane scalars 1015; all but the count and rectangle forms 946 (4575 B); all seven 1002
// [686/225] (frame 0x11cc, head agrees through instruction 14, callers MATCH); plus long edge indices 992, plus an element
// pointer for the current vertex 984, plus both 957 [653/212]: there the record and surface pointers leave their
// callee-saved registers for their stack homes (retail's `mov dword ptr [home], 0`) and the vertex loop agrees with
// retail but for registers. Plus the else arm assigning the surface list before the record 952 [649/211], the lowest
// count (not kept: 980 without the moved bounds declaration, and the forms that need no match reach 985, 961 with the
// float normalizer, 1046 with point scalars, 1012 without the rectangle). On 952: seed before count 952, cursor declared
// before the bounds loop 952, start-face element pointer 952, lookup spellings (global at each use, named count, one &&
// test, both) 954/952/952/954; leave-one-out in the order above, then edge indices, vertex pointer: 960/966/970/980/
// 1022/976/993/979/987. On 957, further toward retail's structure but off its frame (0x11c4): point transform on
// scalars 1033 [-/206]; neighbour index read before the vertex copy 1012 (callers differ); a found flag for the worklist
// scan (retail has no index == count test) 960; those three 961 [609/180]; with the normalizer squaring in float 981
// [612/156] (962 with the else-arm order); with a point2f element pointer for the projection 975 [607/153]. The float
// normalizer on 957: 962/963 (early return / scale arm first), with the else-arm order 958. Open on 952: arg_3 in EBX
// where retail takes EDI (then the group table in ESI and its count in EBX; ours EDI/ESI). The 8 bytes the structural
// form lacks are retail's homes for the inverse spacing and the clamp constants it reloads after each 0x48e70 call:
// BLOCKED-BY 0x48e70.
// M3 lane 7 r200 (fast with 0x9cca0/0x184250/0x184060, all MATCH throughout; control 965): the worklist scan as a for
// loop with a break on a hit 965 (same code); the start face bound by a const reference for the entry lookup and the
// seed 965 (other code); the face and plane arrays read into per-face locals at the face-loop top (retail loads both
// there into EBX/EDI) with the face array also in the neighbour test 965 (other code); the axis row through one
// `short const *` pointer at all four uses 999 (4572 B, frame 0x11cc as retail, first difference +0x1c); that with the
// sign named as a bool 1001 (4598 B). BLOCKED-BY 0x48e70 as above.
// M3 lane 15 r116 (fast with the callers 0x9cca0/0x184060, both MATCH; control 965): the flipped plane negated with unary
// minus 965 (same code); the plane copied first and negated in place under the flip test 987 (4602 B), with unary minus 987;
// the else arm's copy written field by field 1017 (4544 B); the three magnitudes through fabsf 965 (same code), computed
// k, j, i 965 (same code).
// M3 lane 1 r141 (fast with the callers 0x9cca0 / 0x184060 and 0x184250, all MATCH unless said; control 965, aligned 938,
// register-masked 459, frame 0x11d4). Lane 11 r131's 952 lead re-applied and re-measured 952 (frame 0x11cc, aligned 890,
// masked 371; still a moved-declaration form, not kept). Reading of the head on that lead: retail loads the start face
// into EDI for the head only (later uses read its parameter home, `cmp eax, [ebp + 0x14]`), never gives the record
// pointer a register (an immediate zero or EAX stored to its home in each arm, reloaded at each test) and takes the group
// count into EBX for `cmp eax, ebx`; the lead takes the start face in EBX and carries the record pointer in EDI to one
// store at the join. On this body: the record pointer cleared at its declaration and set only in the instance arm 966;
// the instance arm written first with the world arm in the else 986 (both callers lose their match, 15 each: arg_0
// moves); the group count read into a named long for the bounds test 965 (same code); the start face through an element
// pointer for its group read 965 (same code). On the 952 lead: the named count and the element pointer together 954
// (aligned 886, masked 358); the record pointer cleared ahead of the test 970 (4600 B). Still BLOCKED-BY 0x48e70 (its
// file is another machine's this round) for the reloads after each of its calls. Body unchanged.
// M3 lane 11 r145 (H2M fast with the callers 0x9cca0/0x184060, both MATCH each run; control 965): the four clamped
// cell bounds named first and then floored/ceiled 991 (4562 B); the zero-spacing arm first (`== 0.0f`) 976 (4595 B);
// the axes read from g_440b94 at each use with a long table index 985 (4595 B, frame 0x11cc as retail, first
// difference +0x1c), with the dominant axis long as well 985 (same code); the particle system through the existing
// DATUM macro 965 (same code). Still BLOCKED-BY 0x48e70 for the reloads after its calls. Body unchanged.
// M3 lane 12 r169 (h2m fast with 0x9cca0, 0x184060 and 0x184250, all MATCH; control 965): the worklist scan index as a
// short 1013 (4621 B); the projected cell point stored member by member 965 (same code); the neighbour tests through the
// element's address 965 (same code); the random cell offsets ahead of the point copy 966; the bounds products bound-first
// 965 (same code). Still BLOCKED-BY 0x48e70: retail reloads XMM values after its calls here and in 0x48ee0 alike.
// M3 lane 6 r224 (H2M_FAST with the callers 0x9cca0/0x184060, MATCH unless said; control 965): the loop's two start-face
// compares against the worklist's seed slot 974 (4593 B; the callers 18 / 11); the start face copied into a local for its
// four reads 1004 (4541 B); the table global cast at each use with no local for it 978; the bounds-initialisation counter
// typed short 1004 (4570 B); the record tests written on the instance argument (`arg_0 != NONE`) 989 (4599 B). Body
// unchanged; still BLOCKED-BY 0x48e70.
// M3 lane 12 r169 (h2m fast with 0x9cca0, 0x184060 and 0x184250, all MATCH; control 965): the worklist scan index as a
// short 1013 (4621 B); the projected cell point stored member by member 965 (same code); the neighbour tests through the
// element's address 965 (same code); the random cell offsets ahead of the point copy 966; the bounds products bound-first
// 965 (same code). Still BLOCKED-BY 0x48e70: retail reloads XMM values after its calls here and in 0x48ee0 alike.
// M3 lane 16 r176 (H2M_FAST with 0x9cca0, 0x184060 and 0x184250, all MATCH each run; control 965): the start face read
// through the face-pointer local (declared at the top, reassigned per face) 965; the side bit as `> 0.0f ? 1 : 0` 965;
// the dominant axis as an if/else-if chain 965; the definition lookup after the world-bounds loop 965; the worklist scan
// as a for loop with a break 965 (all the same code). Diagnostic, not kept: the two 0x48e70 calls redirected to a
// same-file copy of 0x48e70 declared `__declspec(noinline) inline` 1014 (4623 B): the inverse spacing and both clamp
// constants are then reloaded after each call exactly as retail does (the head is unchanged); the same copy without
// `inline` 965 (no reloads). So the reloads follow 0x48e70 being an inline-declared definition kept out of line, as
// 0x22750 is for 0x22cf0; the lead is `inline` on 0x48e70's own definition (machine 1's file), not measured here.
// M3 lane 16 r178 (coordinator diagnostic, one full check, restored byte for byte): `inline` on 0x48e70's own definition
// (0x22750's spelling) 965 -> 1014 (4590 -> 4623 B, first +0x7 unchanged); 0x48ee0 174 -> 175 (1486 -> 1496 B, first
// +0x5c -> +0x29); 0x48e70 itself still matched (kept out of line); no other count moved, no match lost or gained.
// M3 lane 11 r158 (h2m fast with the callers 0x9cca0/0x184060, both MATCH each run; control 965): the surface-list
// pointer declared const 965 (same code); the per-face bounds initialised by a two-pass loop at the same place 980; the
// material lookup through the existing global_material_get in place of its body written out 978 (head identical, later
// homes move), with the one-use index folded into the call 978; the edge side as a bool re-measured 981; the two early
// returns as nested blocks 965 (same code). Body unchanged.
// M3 lane 7 r224 (fast screen with the callers 0x9cca0/0x184060/0x184250, all MATCH each run; control 965, aligned 938,
// register-and-slot-masked 454, frame 0x11d4). Lane 11 r131's two saved leads rebuilt: the 952 lead 952 (frame 0x11cc,
// aligned 888, masked 363) and the structural lead 975 (4573 B, frame 0x11c4, aligned 803, masked 252). Lane 16 r176's
// diagnostic (the two 0x48e70 calls sent to a same-file copy declared `__declspec(noinline) inline`, which gives
// retail's reloads after each call) had only been measured on this body (1014, re-measured, aligned 929); on the
// leads: 952 lead 1033 (4651 B, frame back to 0x11d4, aligned 803, masked 346), structural lead 977 (4602 B, frame
// 0x11c4, aligned 756, masked 247). New forms read from retail's sample loop: (A) the two random cell offsets named
// ahead of the two scaled sums (retail stores each to a float home, 0x120/0x124, and adds it with addss to a
// cvtsi2ss of the column and to the row's hoisted float; ours folds the sum into the x87 chain with fiadd): on this
// body 1012 (4559 B, frame 0x11cc as retail, aligned 973, masked 432); (C) the zero-velocity copy and the force
// record lookup ahead of the sample normalize (lane 10 r106's order) re-measured 981 (aligned 992, masked 430); both
// 1014 (4556 B, frame 0x11cc, masked 415). On the structural lead with the diagnostic: A 840 (4572 B, aligned 666,
// masked 244), C 969 (4603 B, aligned 756, masked 220), A and C 822 at retail's 4590 B and retail's 994 instructions
// (aligned 648, masked 221; frame 0x11c4, so two dword homes are still missing: retail's slot run ends at +0x12c with
// +0xf4 and +0x104 never referenced); the structural lead with A and C but without the diagnostic 1004 (4516 B, frame
// 0x11bc). None kept: every one is a match-only form (one-use locals, moved declarations) and the 822 needs `inline`
// on 0x48e70's definition, which is another machine's file and a held form. Left on the 822 form: the start face's
// group read (retail forms the element address with a lea and reads +6 through it), the worklist cursor zeroed ahead
// of the six bounds stores, the per-face bounds constants loaded at the face-loop top, and the product order in the
// plane transform. Body unchanged; still BLOCKED-BY 0x48e70.
// M3 lane 5 r244 (fast screen with the callers 0x9cca0/0x184060/0x184250, all MATCH each run; control 965). On the form
// with retail's frame (the axes read from g_440b94 at each use, re-measured 974 at 4595 B, first difference +0x1c; with
// the flag word in a long for the index and the flip test, re-measured 970 at retail's 4590 B): the face array in a
// per-face local indexed by the face for both field reads and for the neighbour tests, with no element pointer
// (retail's `[ebx + esi*8]` and its +0x10 home) 970 (same code: the array was already one value); that with the plane
// array in a per-face local as well 975 (4573 B); that with the edge and vertex arrays in locals ahead of the vertex
// loop 968 (4590 B); that with the per-face bounds initialised at the face-loop top 979 (4590 B); the worklist scan
// with a found flag (retail has no index == count test) on this body 970 (4573 B), on the 968 form 987 (4595 B).
// Reading of the allocator's order on the 970 and 968 forms: the head's three register differences (the start face in
// EBX for retail's EDI, the scenario base in ECX for EDX, the surface list in EDI for ECX) are one decision. The list's
// range from the head to the vertex loop ranks 124 and takes EDI, the lowest-scored free register; retail gives it ECX,
// which needs the face array (EBX; ranked 56 here), the plane array (EDI) and the face index (ESI; ranked 152 here and
// given EDX) coloured ahead of it, and the flag word (ranked 96 here, given ECX) in EDX. Forcing any one of those
// decisions (a diagnostic, not source) moves the count by 13 at most (957), so no single count gives retail's head.
// Body unchanged; still BLOCKED-BY 0x48e70 for the reloads after its calls.
// Per-face surface-array capture 965, neighbor record 984, both 984; signed plane word alone
// 984, with the array 983, with the neighbor 983, with both 978, and current-face reference 978.
// The 0x11d4 frame and the initial scene and surface homes remain.
// M3 lane 2 r148 (screens with the callers 0x9cca0/0x184060/0x184250 and 0x48e70 in the image, all MATCH each run; control
// 965 at 4590 B, aligned 711, 279 with registers and offsets masked; body unchanged): the worklist's first entry stored ahead
// of its count (retail's store order) 965 (same count); the material lookup's inner test as a conditional expression 968; the
// surface list computed ahead of the record pointer in the else arm 963; the material table pointer declared ahead of the
// lookup 962 (three points, a declaration move); the plane word read once into a long for the index and the flip test on
// this body 974; the lookup's sign test joined to the count test 965 (same count); the edge's far face read ahead of the
// vertex copy 986 (4602 B); the first and third together 963. Reading of retail's head, for the next round: the record
// pointer is never in a register there (its NULL is one immediate store to +0x98 and every later test reloads it), the
// material count is loaded into EBX ahead of its compare, and the vertex loop keeps the two axis offsets pre-scaled
// (`axis * 4 - 8` from a pointer stepped through the vertex array) with the table row offset saved for the sample loop,
// which is the axes-read-at-each-use form already listed at 974/970. Still BLOCKED-BY 0x48e70 for the reloads after its calls.
// M3 lane 5 r251 (fast screen with the callers 0x9cca0/0x184060/0x184250, all MATCH each run; control 965). Reading of
// the head: retail keeps the record pointer (local_5) in memory from the start (one immediate NULL store, +0x98) and the
// material record (local_9) in memory as well (+0xe0, NULL stored from a zeroed EDX). Forms: local_5 started NULL at its
// declaration with the NONE arm setting local_6 only 966; the material count named ahead of its compare (retail loads
// it into EBX first) 965 (same code); the start face as a named element pointer read at +6 965 (same code); the three
// together 966; local_5 and local_6 as two conditional initialisers 965 (same code). 0x48e70 is machine 3's now, but its
// lever (its register use hidden from link-time code generation: an `inline` definition or a separate non-LTCG build)
// is a held or flag-only form, so this stays BLOCKED-BY 0x48e70.
// M3 lane 12 r196 (fast screen with the callers 0x9cca0/0x184060/0x184250 and 0x48e70, all MATCH each run; control 965).
// The start face's plane distance through the existing plane_distance_to_point in place of its body written out 965 (the
// same count; that block's three products then run z, y, x as retail's do), but the full check then lost the matched
// 0x1c4590 and 0x24cf66 (whole-image flips), so it was reverted. Forms: the projected point through the
// existing vector3f_add_scaled 1009 (4553 B), with the distance helper as well 1009; the two sample offsets through it
// 964 (4591 B; one lower at another length, not kept); the two force sums through it 986 (4563 B). Still BLOCKED-BY
// 0x48e70 for the reloads after its calls.
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
                    if (!function_23a220(local_2, local_27, &local_48, 0.0f))
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
