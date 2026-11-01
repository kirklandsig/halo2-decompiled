// @flags /O2 /Gr /arch:SSE
/* the render state constants of a projected source: built without /Op, which
   keeps sqrt and tan inline (src/unknown_0494b0.cpp is /Op) */

#include "unknown_11c920.h"
#include "globals.h"
#include "render_source_storage.h"
#include "texture_stage_storage.h"
#include "unknown_0494b0.h"
#include <xtl.h>
#include <math.h>

void function_1cf50(void);
void function_01ddd0(long index, dword width, dword height);
long function_48e70(real value);
extern real g_485a34, g_485a40;
extern byte g_485a47;
extern real g_485a0c[8];
extern real g_4c9798[36];

// score 230 (was 230): the mode bit loads the flags into edx where retail uses eax, so
// every later offset drifts; the mode takes the frame's lowest slot where retail keeps
// the size, and the size and the aspect stay in registers across the rounding calls.
// Earlier frame: 0x24 versus 0x28; state in EAX/EDI (retail ECX), the size home across the rounding calls and the far-plane copies differ.
// Earlier form: frame 0x20 versus 0x28; input registers and rounding-call scalar reloads differ.
// Branch-defined extent 321; two-vector copy loop 311; frame pointer 372; size policy 497; inline depth one 311.
// Global optimization off 1009; expansion depth zero 349; late mode 350;
// near normal first 309; height clamp first 311; sample y/z first 313, plus normal first 311.
// Earlier form: Frame 0x20 versus 0x28 and input/rounding homes remain. Near normal input 313; far normal copy 295 kept (plain, retail length); integer maximum reuse 311; far point before scale 313; height rounding first 311 (fast).
// Earlier form: Frame 0x20 versus 0x28 and input/rounding homes remain.
// Frame 0x20 versus 0x28; state/matrix and protected rounding/dimension carriers remain.
// frame 0x20 versus 0x28; input registers and rounding-call scalar reloads remain.
// frame 0x20/0x28 and state/matrix/rounding homes remain.
// Radius/scaled-radius/padded-radius parts 295/295/295; size accumulation 295;
// width/height/both producers 295/295/295; consecutive clamp bounds 301.
// The one-use radius call result is restored; matched storage-provider root resolves all trials.
// frame 0x20 versus 0x28; input registers and rounding-call scalar reloads differ.
// Frame 0x20 versus 0x28 and input/rounding homes remain. Near normal input 313; far normal copy 295 kept (plain, retail length); integer maximum reuse 311; far point before scale 313; height rounding first 311 (fast).
// Frame 0x20 versus 0x28 and input/rounding homes remain.
// Distinct normals 295; scoped near normal 295; local near-normal dot 301; point setters 305;
// late pixel area 295; origin pointer 295; clamp result 295; fallback-first far scale 296 (plain).
// Frame 0x20 versus 0x28, input registers and rounding reloads remain. Tried texture row
// memset 300; whole table memcpy 361; two vector memcpy calls 295; row clear and whole copy 366; row clear and
// vector copies 300; near normal memcpy 295; far normal memcpy 295; both normal memcpy calls 295.
// Distance before aspect 296; aspect after offset 296; reciprocal before aspect 300;
// conditional/branch offset 309 each; branch reciprocal 295; complete size result 295; combined entry/results 313.
// Five consistent formal permutations 295 each; native byte mode 332; unsigned mode 295;
// signed-byte mode 331.
// Direction/origin/both value snapshots 376/329/411; far output dot 311;
// point component loops 303; normal loop 295; plane scaling loops 295; combined loops 300.
// Native width/height/both result arms 295/295/295; shared sizing predicate 331;
// height limit first 303; complete mode result 295; initial projection/opposite loops 295/295.
// Shared near/far/both/projection/all scaling 295/295/295/295/295; near/far/both point accumulation 295/295/295.
// Round 188 complete sizing scope, unsigned-word mode, depth-bound scope, their
// pairs/all, and signed-word mode with sizing scope: 300/291/295/297/300/291/297/297.
// All original scalar lifetimes, native mode, geometry and protected interfaces retained.
// Round 167: Individual or mixed bounded dimension carriers:
// width short 296; width word 296; height short 324; height word 324;
// short/word 325; word/short 325; short/int 296; int/word 324.
// Original native limits, far-normal copy and all protected call interfaces restored.
// Round 213: direct lower/upper/both depth fields 294/295/294; each with distinct far normal
// 294/295/294; both with distinct far point 302, with distinct far point and normal 294.
// One-point gains rejected; original field snapshots and captured far normal restored.
// Round 194: Native projection/dimension/border-identity row combinations 297/295/297/295/297/295/297; all
// rows with projection scope 298. Original body restored.
// Round 375: Complete near/far/shared-pair/distinct-pair plane outputs on the
// retained far-normal graph 322/335/361/364; CRT copies 322/335/361/364.
// Frame 0x30 versus 0x28; all exact-only output-record forms restored.
// Round 139: Repeated ratio/angle inputs captured once separately/together 316/305/316; each with
// real shared pixel area 321/326/321; joint input home names without/with area 316/321. Frame
// 0x20/0x28, state/matrix and protected rounding homes remain. Original executable source, every
// float tree and accepted far-normal copy restored.
// Round 96: retail takes the camera-like record (read at +8/+0x64/+0x28) in ECX and keeps the source
// record (origin +0, direction +0x28) in EBX across the calls; ours takes the first in EAX and leaves the
// second on the stack. The two pointer parameters were named crossed against both callers (kept: now
// swapped, data flow as retail, 295). Direction/origin fields read at each use 328; tag test as a
// one-case switch 295; origin/direction references at entry 300.
// Round 60: maximum width/height written through pointers by an inline helper 295; flag test as
// a shifted bool, its negated-bool and its compare-with-zero spellings 311/337/295; offset as a
// ternary 309.
// Lane 10 r13: kept 252: the first block reads the direction through the record and the direction
// reference is declared with the origin for the planes (292: the record now arrives in EBX and
// stays there, with retail's lea/mixed [ebx+0x2c]/[ebp] direction reads); the mode bit tested as
// (bool)((~(flags >> 10)) & 1) (252: retail's shr/not al/test al,1). Also tried: size clamp as one
// ternary 295 (identical); !(bool) bit 299; (~flags >> 10) & 1 295; ~(flags >> 10) & 1 253; an
// inline declaration of 0x48e70 252 (identical); height declared first 252; far plane copied
// straight into the output 344; depth fields read at each use 253; 0.75 first in the border 252.
// Retail reloads the size after each 0x48e70 call (ours keeps it in xmm0: LTCG knows the callee
// leaves xmm alone; BLOCKED-BY 0x48e70, in a file this lane does not edit).
// ternary 309. Round 103: retail stores 0.0 to a frame slot at entry and loads it as the border's
// else value: `real border = 0.0f;` at the top (or after the distance/offset) with `if (mode > 0)
// border = ...` reproduces those entry stores and holds matrix in one register as retail does, but
// 318 (1489; aligned 223 -> 214), with the origin reference at entry 318, declared after the mode
// 314. Ours keeps size and aspect in xmm registers across the 0x48e70
// calls where retail reloads them; the tag test's constant 1 (ecx, reused for mode = 1) puts the state
// record in esi where retail tests with movsx/dec and passes it in ecx.
// Round 245: short/word mode with short-width/long-height limits 269/269;
// with long-width/short-height limits 239/239; both short limits 267/267;
// both word limits 267/267. Original long mode with short/word height 243/243.
// Short mode and short height limit kept: 239 plain, 1483/1479 bytes; all call values unchanged.
// Frame 0x24/0x28, state transport and protected rounding reloads remain.
// Round 208 on the late-direction/complemented-mode graph: early zero border 322 (frame 0x28);
// complete bounded-size result 252; reused native pixel area 342; zero+size 322; zero+area 354;
// size+area 342; all three 354; all with zero declared after mode 353. All restored; state
// transport, protected rounding reloads and far-plane stores remain.
// Round 219: short mode/height/both/all limit role names 239 each; entry halfword declarations 239;
// readonly mode/height/both 239 each. Original restored.
// Round 166: Existing native state fields 239; shared width-first/height-first bounded dimensions
// 298/306; with native fields 298/306; native fields with reused signed pixel capacity 333; shared
// width/capacity without/with native fields 322/322. Original short mode/height, scalar
// observations and far-normal copy restored.
// Round 279: both category-admission predicates kept for publication and border, each
// with multiple consumers: bool [363,348]; byte [363,348]; short [383,393];
// long [373,388] (each pair late/entry zero border). Original body restored; state transport,
// rounding reloads and the missing frame word remain.
// Round 249: Existing native tag/direction/both field views 239 each;
// early border zero alone/with tag/direction/both 342 each, zero after mode
// with both views 297. Border forms recover frame 0x28 but regress; original
// short mode/height, observations, copies and protected call inputs restored.
// Round 109: the tag test as a one-case switch gives retail's `movsx eax; dec eax` and frees ECX
// but LTCG still passes the state in EAX, 246; mode as a bool-valued arm 239; mode through
// if/else 239; mode computed first 328; the inverse before the offset 333.
// Round 222: Existing native projection/opposite/near-normal vector setters, all seven
// subsets 252/255/264/239/252/255/264; all with both point setters 264. Frame
// 0x24/0x28 and state/rounding homes remain; complete original body restored.
// Round 174: entry tag category captured as short/long 239/239; complete raw sizing flags
// 237; short/long tag with raw flags 237/237; tag short/long with entry border zero
// 342/342; short tag, raw flags and zero 335. Two-count gains and exact-only tag copies
// restored; frame 0x24/0x28, state transport and protected rounding reloads remain.
// Round 72 (lane 8): the cause of the state transport is the tag test: ours compares the word
// against a constant 1 it shares with `mode = 1` (ECX), so state arrives in EAX and moves to EDI;
// retail loads the word with movsx and tests it with `dec eax`, which leaves ECX for the state
// record (the entry then matches retail but for the frame). Only the steering spelling
// `... - 1 == 0` gives that here (319, 1479 B; with an entry border zero 240/238), not kept.
// Also tried: the tag word in a named local 239; long mode 243; offset as a ternary 335; distance
// read before the aspect 240. Retail also passes width/height to 0x1ddd0 in ESI/EDI (ours: EAX
// and the stack; 0x1ddd0's other callers are in a contributor file).
// Round 191: distance field at every use with saved/lower-live/upper-live/both-live
// depth observations 330/323/330/324; with entry-owned border zero 267/268/267/268.
// All regress; original distance/depth snapshots and border restored.
// Round 280 (lane E, run 14): the overflow fix-up compares the height against the width
// and the height is rounded first, so the fix-up block matches retail instruction for
// instruction (cmp esi, edi; cdq; jle; idiv; the & ~63 masks). 230, kept.
// Round 289: On the retained height-first overflow graph, long-mode/long-height/short-width carrier subsets 1 through 7; positive word counterpart:
// [231, 250, 250, 274, 280, 268, 265, 274]. All executable source and temporary types restored.
// Round 299: pixel-size declaration at entry/before mode/before extent 230 each;
// each with coordinated pixel-size/mode names 230; coordinated names alone and
// diameter lexical rank 230. Original declarations, names and body restored.
// @retail 0x48ee0
void function_48ee0(byte const *matrix, byte const *state, byte const *tag)
{
    real border = 0.0f;
    real aspect = (real)sqrt(*(real const *)(state + 8));
    real distance = *(real const *)(state + 0x64);
    real offset = 0.0f;
    if (*(short const *)(tag + 0x8e) - 1 == 0)
        offset = *(real const *)(state + 0x28);
    real inverse = distance != 0.0f ? 1.0f / distance : -3.4028234663852886e38f;
    g_4c9798[0] = ((vector3f const *)(matrix + 0x28))->i * inverse;
    g_4c9798[1] = ((vector3f const *)(matrix + 0x28))->j * inverse;
    g_4c9798[2] = ((vector3f const *)(matrix + 0x28))->k * inverse;
    g_4c9798[3] = 0.0f - inverse * offset;
    g_4c9798[4] = 0.0f - ((vector3f const *)(matrix + 0x28))->i;
    g_4c9798[5] = 0.0f - ((vector3f const *)(matrix + 0x28))->j;
    g_4c9798[6] = 0.0f - ((vector3f const *)(matrix + 0x28))->k;
    g_4c9798[7] = 0.0f;
    short mode = (bool)((~(g_4c1a80 >> 10)) & 1) ? 2 : g_485a40 > 0.0f && *(short const *)(tag + 0x14) == 0 ? 1 : 0;
    real extent = *(real const *)(state + 4);
    if (*(real const *)(state + 0xc) > 0.0f)
        extent += (real)tan(*(real const *)(state + 0xc) * 0.5f) * distance * 2.0f;
    long max_width = (bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 128 : 512;
    short max_height = (bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 128 : 512;
    real maximum = (real)max_width;
    real size = ((real)sqrt(extent * extent * *(real const *)(state + 8)) * 0.33333334f + *(real const *)(tag + 0x10) * 0.125f) *
        (maximum - 128.0f) * g_485a34 + 128.0f;
    if (size < 128.0f) size = 128.0f;
    else if (size > maximum) size = maximum;
    long height = (function_48e70(size * aspect) + 63) & ~63;
    long width = (function_48e70(size / aspect) + 63) & ~63;
    if (width * height > max_width * max_height)
    {
        if (height > width) height = (max_width * max_height / width) & ~63;
        else width = (max_width * max_height / height) & ~63;
    }
    g_485a47 = (byte)mode;
    g_51f3c8[1][0] = 0;
    g_51f3c8[1][1] = 0;
    g_51f3c8[1][2] = 0;
    g_51f3c8[1][3] = 0;
    function_1cf50();
    if ((bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1))
    {
        g_4c9798[8] = maximum;
        g_4c9798[9] = (real)max_height;
        g_4c9798[10] = 16777215.0f;
        g_4c9798[11] = 1.0f;
    }
    else
    {
        function_01ddd0(9, width, height);
        g_4c9798[8] = (real)width;
        g_4c9798[9] = (real)height;
        g_4c9798[10] = 16777215.0f;
        g_4c9798[11] = 1.0f;
    }
    g_4c9798[32] = 1.0f;
    g_4c9798[33] = 1.0f;
    g_4c9798[34] = 0.0f;
    g_4c9798[35] = 0.0f;
    if (mode > 0)
        border = (real)width * 0.75f / maximum;
    g_4c9798[12] = border + 0.5f;
    g_4c9798[13] = 0.5f - border;
    g_4c9798[15] = 0.0f;
    vector3f const &direction = *(vector3f const *)(matrix + 0x28);
    point3f const &origin = *(point3f const *)matrix;
    plane3f *plane = (plane3f *)&g_4c9798[16];
    vector3f normal;
    point3f point;
    point.x = direction.i * 0.01f + origin.x;
    point.y = direction.j * 0.01f + origin.y;
    point.z = direction.k * 0.01f + origin.z;
    normal.i = 0.0f - direction.i;
    normal.j = 0.0f - direction.j;
    normal.k = 0.0f - direction.k;
    plane->n = normal;
    plane->d = dot3f(&plane->n, (vector3f const *)&point);
    plane->i *= 100.0f;
    plane->j *= 100.0f;
    plane->k *= 100.0f;
    plane->d *= -100.0f;
    real lower = *(real const *)(tag + 0x2c);
    real upper = *(real const *)(tag + 0x30);
    real scale = upper > lower ? 1.0f / (upper - lower) : 1000.0f;
    plane = (plane3f *)&g_4c9798[20];
    point.x = direction.i * lower + origin.x;
    point.y = direction.j * lower + origin.y;
    point.z = direction.k * lower + origin.z;
    normal = direction;
    plane->n = normal;
    plane->d = dot3f(&normal, (vector3f const *)&point);
    plane->i *= scale;
    plane->j *= scale;
    plane->k *= scale;
    plane->d = 0.0f - plane->d * scale;
    ((vector4f *)&g_4c9798[24])[0] = ((vector4f const *)g_485a0c)[0];
    ((vector4f *)&g_4c9798[24])[1] = ((vector4f const *)g_485a0c)[1];
    D3DDevice_SetVertexShaderConstantFast(-56, g_4c9798, 9);
}
