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

// MiniMax E (11-20): no new form, no lower score (174): the mode bit still loads the flags into EDX where retail shifts
// them in EAX (`mov eax, [0x4c1a80]`, `shr eax, 0xa`, `not al`, `test al, 1`), and the size still stays in a register
// across the two rounding calls where retail reloads it from the lowest slot, and that reload is what sets the frame's
// slots: BLOCKED-BY 0x48e70 (its xmm clobber set is set in src/unknown_02b400.cpp, another lane's file, where it is
// held). Retail's own slots were read off its listing - the size in the lowest slot, the mode above it, the maximum and
// the extent above those and the aspect next - while ours keeps the mode lowest and the size in the extent's old place,
// and the declaration order does not move them. The body stands.
// MiniMax G (11-19): no new form, no lower score (174): the mode bit still loads the flags into EDX where retail uses
// EAX, and the homes are still permuted (ours keeps the mode in the lowest slot at 0x8 and the offset at 0x14 where
// retail keeps the mode at 0xc and the offset at 0x10). This round the declaration order was tried with the mode
// declared at the top (174), with the mode declared before the offset (174), with the extent before the offset (333)
// and with the whole mode block moved to the top (333): the homes do not follow the declaration order, and the size
// still stays in a register across the two rounding calls where retail reloads it, which is BLOCKED-BY 0x48e70's
// link-time register summary in another lane's file. Retail's own slots were read off its listing to aim the rest: it
// keeps the size in the lowest slot, the mode above it, the maximum and the extent above those and the aspect next,
// while ours keeps the mode lowest and the size in the extent's old place, so the size's declaration was hoisted to the
// top of the function (174), with the maximum hoisted with it (174) and with the extent hoisted too (174): the homes do
// not move at all, and the mode's test was also written without its casts (174), with the cast dropped from the test
// (174) and through a dword local (174). Body unchanged.
// MiniMax G (11-19): no new form, no lower score (174): the mode bit still loads the flags into EDX where retail uses
// EAX, the mode's home still sits one slot lower than retail's (retail keeps the clamped size in the lowest slot and
// the mode above it, ours keeps the mode in the lowest slot and reuses the distance's slot for the size), and
// retail's clamps write the clamped value back through the size's slot in both arms where ours assigns it in one
// arm; the nested-ternary clamp, the limit as a long and the mode's placements all score the same or worse.
// Body unchanged.
// MiniMax E (11-15): no new form, no lower score (174): the mode bit still loads the flags into EDX where retail
// keeps them in EAX, so that load stays a byte longer and every later address field drifts, and the size still
// stays in a register across the two rounding calls where retail reloads it from the lowest slot, which sets the
// frame's slots: BLOCKED-BY 0x48e70 (its xmm clobber set, in another lane's file). A 30-try permuter run found
// no better form (best 168 differing instructions = the baseline); the height limit read as a dword 250, the size
// and the aspect volatile 251 (a banned form, reverted) and the mode as a ternary chain 174 (the same code) all
// leave the flags in EDX.
// MiniMax R (11-14): no new form, no lower score (174): retail's mode test reads the flags with the five-byte
// `mov eax, [flags]` form and shifts them in EAX, where this build reads them into EDX and shifts them there, so
// every later offset drifts; the mode sits at [esp+0xc] and the extent at [esp+0x14] where ours keeps them at
// [esp+8] and [esp+0x10], retail's maximum height is a full dword where ours sign-extends a short, and the size
// still stays in a register across the two rounding calls where retail reloads it from the lowest slot, which
// needs the xmm clobber set of 0x48e70 in another lane's file: BLOCKED-BY 0x48e70.
// MiniMax I (11-11): no new form, no lower score (174); the mode flag still loads the flags into EDX where retail
// uses EAX, the size still stays in a register across the two rounding calls where retail reloads it from its
// slot, and the frame's slots still run from the mode up where retail's start with the size.
// MiniMax B (11-12): no lower score than 174: the mode flag in a byte local gives retail's exact length (1479 bytes)
// but scores 176, and it in an int local, in a bool local or in one ternary gives 174, 174 and 174.
// MiniMax A (11-13): no new form, no lower score (174): the mode bit loads the flags into EDX where retail uses EAX,
// the size stays in a register across the two rounding calls where retail reloads it from its slot, and the frame's
// slots still run from the mode up where retail's start with the size; the rounding callee now matches.
// MiniMax A (11-18): no new form, no lower score (174): the five source variants tried this round score 250, 280, 264, 258
// and 258; the frame's homes still run mode, maximum, extent, size, distance, aspect, border where retail's start with
// the size, and the mode bit still loads the flags into EDX where retail uses EAX.
// score 174 (was 174): Frame 0x28 agrees. The mode flags load into EDX instead of EAX. Mode and size homes and rounding reloads differ.
// r275 unchanged. The far plane's near distance goes into a named local so the three
// point components share one load; the mode bit still loads the flags into edx where retail
// uses eax, and the size stays in a register across the rounding calls where retail reloads it.
// The earlier score was 178.
// MiniMax I (11-09): no new form, no lower score (174); the permuter's best form (153) needs the height limit in a byte, which turns 128 into -128 and 512 into 0 and so changes what the limit means, and its other six changes (the aspect declared before the border, the height limit's type, a cast on the width, the second limit test as an equality, the dot helper's arguments swapped, the plane's scale written as a product and the plane's distance kept in a local) are neutral (174); a height limit read as a 32-bit value (250), a byte flag for the two limits (280), the size declared at the top of the function (174) or just before the mode (174) and the size in a local of its own (174) all score worse; the frame's slots, read off the listings, run from the lowest up as the size, the mode, the maximum, the extent, the distance, the aspect and the border in retail, and as the mode, the maximum, the extent, the size, the distance, the aspect and the border in ours, because retail keeps the aspect and the size in memory across the two rounding calls and ours keeps them in registers, which needs the xmm clobber set of 0x48e70, in another lane's file.
// MiniMax I (11-09): no new form, no lower score (174); the height limit read as a 32-bit value matches retail's dword read of that limit (250) but moves the size and the two limits to other slots, and with the far plane's point in three scalars it keeps retail's far plane and its transport (255); the border's condition as a plain test (175), the mode's value in a byte (176) or a short (223), the mode zeroed at its branch (180), the extent folded into the size's expression (389) or split into a square of its own (287), the mode's store moved after the branch (339), a local before the mode (360) and the mode's block moved before the first stores (326) all score worse; retail keeps the size in the lowest slot, then the mode, the maximum, the extent (reused by the width limit), the distance (reused by the height limit), the aspect and the border, while ours keeps the mode, the maximum, the extent, the size, the distance, the aspect and the border in that order, and that difference follows from retail's store-back of the clamped size and its reloads after the rounding calls, which need the xmm clobber set of 0x48e70, in another lane's file.
// MiniMax I (11-09): no new form, no lower score (174); the far plane still takes its dot product from the local normal while retail copies the direction into the plane and reads the plane's three components back, and every form that reads them back (the plane's members, a dot helper on the plane, the point as three scalars, the point's components in retail's order) moves the state out of ecx and the matrix onto the stack, 339, 343, 346, 347; the size still stays in a register across the two rounding calls where retail reloads it from its slot, the mode bit still loads the flags into edx where retail uses eax, the width limit, the height limit and the size sit in other slots than retail's, the height limit is read as a sign-extended word where retail reads a dword (a 32-bit limit moves the two limits and the size to other slots, 250), and the border's product still follows the divide where retail multiplies first (the border's product and divide in two statements, with and without a carrier of its own, score 180).
// MiniMax R (11-09): no new form, no lower score (174); the mode bit still loads the flags into edx where retail uses eax and the size stays in a register across the rounding calls where retail reloads it; retail passes the state in ecx and the matrix in ebx, the order src/unknown_0350e0.cpp already declares, so the definition's own parameter names stay as they are.
// MiniMax H (11-07): no new form, no lower score; the extent, the width limit and the size sit one slot lower than retail's, which reuses the distance's and the extent's slots for the two limits, and the mode flag still loads the flags into edx where retail uses eax; the mode block moved after the sizing block, the flags written without the bool cast, the flags compared with one, the mode as a ternary chain and the flags in a named local all leave the flags in edx, and the moved block changes the convention and scores 245.
// MiniMax I (11-06): the flags and mode placements were tried again and the original body restored;
// the permuter produced no variant and the mode flag still loads the flags into edx where retail
// uses eax; no lower score.
// MiniMax B (11-04): no lower score; the new forms tried and dropped are the flags and mode
// placements, the width and height limit types and the mode's own block, all worse or identical.
// Round 112: direct far-plane output 343; scaled-add far point 174, near point 169 (five-point
// form not kept), both 222; long height limit 250; sequential border multiply/divide 180 (the
// border order is fixed but the body regresses); early size declaration and dimension rename
// 174 each. Original executable body restored; mode/size homes and rounding reloads remain.
// Earlier 178 (was 197): the width is rounded before the height, the overflow fix-up compares
// the width against the height the other way round and the near-plane distance is read from
// the tag; the mode bit still loads the flags into edx where retail uses eax.
// Earlier 197 (was 197): the mode bit loads the flags into edx where retail uses eax, so
// Earlier form (178 from 197): the width is rounded before the height, the overflow fix-up compares
// Earlier form (197 from 197): the mode bit loads the flags into edx where retail uses eax, so
// MiniMax lane E (11-01): no new form, no lower score.
// Earlier: the mode bit loads the flags into edx where retail uses eax, so
// the mode bit loads the flags into edx where retail uses eax, so
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
// Round 491: complete mode producers zero-default/nested/exhaustive/two-default, short 232/229/226/280,
// long 203/200/197/286. Long exhaustive result retained at 197 plain, 1484/1479; every input, float tree
// and call tuple unchanged; mode/size homes and rounding reloads remain.
// Round 283: width-first rounding on the retained overflow graph, full-word mode/height
// and shared capacity subsets 0/1/2/3/4/6/7: 231/232/250/250/343/353/355; native short width 278.
// Original height-first producers, bounds, mode, capacity expressions and complete body restored.
// Round 279: Complete immediate scalar results for projection/near/far row families,
// seven nonempty subsets and all with both plane-distance results, 230 each. Original
// compound updates and entire body restored; mode/size homes and rounding reloads remain.
// Round 44 (lane 14): retail rounds the width (size / aspect) first into ESI and the height into EDI
// and passes them to 0x1ddd0 in ESI/EDI with the index in ECX; ours keeps 0x1ddd0's width on the stack
// (BLOCKED-BY 0x1ddd0). Width rounded first with the fix-up on the width 238; with long mode and height
// limit 250; with long mode only 239; border as 0.75 * width / maximum or width / maximum * 0.75 230
// each (every spelling compiles to the divide first; retail multiplies by 0.75 first). With 0x1ddd0 in
// retail's convention (not kept: 0x1ddd0 21 -> 24, its caller 0x2d140 637 -> 686): 226; width first
// 218; width first with long mode 189 (fast); with long mode and height limit 245; long height limit 240.
// Round 325: Entry declaration families for both dimension results, all integer/real limits and
// the actual point/normal workspaces, seven subsets and complete role identities, all eight
// 230 at 1487 bytes. Original declarations restored; flags/mode/sizing and rounding homes remain.
// Round 491: complete mode producers zero-default/nested/exhaustive/two-default, short 232/229/226/280,
// long 203/200/197/286. Long exhaustive result retained at 197 plain, 1484/1479; every input, float tree
// and call tuple unchanged; mode/size homes and rounding reloads remain.
// Round 283: width-first rounding on the retained overflow graph, full-word mode/height
// and shared capacity subsets 0/1/2/3/4/6/7: 231/232/250/250/343/353/355; native short width 278.
// Original height-first producers, bounds, mode, capacity expressions and complete body restored.
// Y499: retained long-mode graph, size/mode/both/extent/size-extent/all/bounds/aspect
// scalar role families 197 each; original names and body restored. Mode/size homes and protected rounding reloads remain.
// Round 516: Complete raw/bounded size producers on exhaustive long-mode graph;
// branch/conditional clamp, height/width-first rounding and native/full height-limit
// combinations. Mode/size homes and rounding reloads remain.
// Scores 197/197/196/196/245/245/245/245; original executable body restored.
// Round 72 (m3 lane 1; harness exact against the listing, masked aligned in brackets, base [58]): BLOCKED-BY 0x48e70: retail reloads size and aspect around the rounding calls because it does not see that callee's XMM use, which takes `inline` on
// its definition (machine 1's file, scratch copy only; the same reload pattern is in 0x184440). On that scratch copy [51]: long height limit, the border product in parentheses, `real raw = size;` with one ternary clamp [43]; plus the near plane's
// normal copied directly into the plane and the dot on the plane [11]; plus `__assume(near_distance >= 0.0f)` after the named near distance **[0], 9 differing instructions at retail's length**: distance and extent slots swapped, and the mode flags in EDX. Other near-point forms on it: read at
// each use [9], reference or tag struct [9], y, z, x statement order [6], origin first, swapped products, casts, const, earlier declaration [11], shared forced-inline point helper [36]. For the last 9: `__assume` on extent, distance, aspect, offset, inverse, the global, the half cone [0] (no change);
// six renamings of distance and extent [0] (no change); square helper, `extent = extent + ...`, `... + extent`, other product orders [0] (no change); named half cone or area, mode test without `~`, `== 1` or a switch for the tag test, swapped or moved declarations all [2]-[19], higher.
// Real `--fast` without the scratch `inline`, all 32 subsets of long height, border, raw clamp, direct normal, near at each use: base 174, raw clamp alone 174 (same code), near 178 (1489 bytes), border 180 (1491), long height 250, height with border 243, direct normal 255-337 (under 1465); none under 174, so nothing is kept. The known 0x1ddd0 match form leaves this at 174.
// Round 59 (m3 lane 15, fast, base 174; BLOCKED-BY 0x48e70 for the size/aspect reloads around the rounding calls, machine 1's file): sqrtf/tanf
// for the aspect, size root and tangent 180 (1491 B); `g_485a47 = (byte)mode` right after the mode is chosen 339 (1488 B); the three
// small-target tests as one inline predicate 380 (1514 B); `mode` declared first 174 (same code); the first plane's dot as dot3f(point,
// normal) 174. Retail's max_height is a full dword (`mov eax, [h]; imul eax, [w]`, `cvtsi2ss xmm0, [h]`), ours sign-extends a short.
// AC551: Complete immediate projection/near-point/far-point multiplication nodes in subsets
// 1/2/4/3/5/6/7, then the complete homogeneous projection on all three families:
// 174/180/174/180/174/180/180/180. NOT kept: no qualifying supported improvement; complete
// executable source restored. Mode-flag register and protected rounding-call reloads remain.
// DL268: Native sqrtf aspect/size, tanf cone, all root/tangent pairs and all three,
// then a double half-angle at its producer: 180/180/180/180/180/180/180/180. NOT kept:
// no supported improvement; original math and body restored, stacked score headings merged.
// Mode/size homes and the protected rounding-call reload boundary remain.
// DL270: Published far-normal dot control 343; native near/far scale-helper
// subsets, published dot pairs, complete graph, dead snapshot removal and complete
// projection/plane scale graph 174/343/174/343/174/343/343/343. NOT kept:
// ties or nonnative frame/short extent; original copies, normal consumers and body restored.
// M396: Actual mode/width-limit/height-limit enum domains, subsets 1/2/4/3/5/6/7,
// 174/174/250/174/250/250/250; complete immutable limit graph 250. NOT kept:
// ties/regressions; original body restored. Mode/size homes and rounding reloads remain.
// M402: Complete reciprocal/depth-scale/offset result arms, seven subsets, then
// complete fallback-first reciprocal: 174/174/323/174/323/323/323/323. NOT kept:
// offset default moves after its branch; original scalar results and whole body restored.
// Round 82 (m3 lane 10, fast, control 174, aligned 89): `size` renamed `dimension` or `buffer_size`, `mode`
// renamed `technique`, `extent` renamed `radius` 174 each (identical: retail keeps the clamped size in the
// lowest slot below the mode, ours the mode lowest, and names move no slot here); `maximum` written as
// `(real)max_width` at each use 179 (1490 bytes, aligned 86); border stores [13] before [12] 178; the first
// plane's `d` scaled before its normal 178; the four g_51f3c8 clears before the mode byte store 172 (two
// points, not kept), with the mode byte after function_1cf50 178, with the clears in reverse order 172. The
// 0x1ddd0 call now matches retail (width/height in ESI/EDI, 0x1ddd0 matched). Still BLOCKED-BY 0x48e70
// (machine 1's file): retail spills the clamped size across its calls, which sets the frame slots.
// Round 98 (m3 lane 16, fast, agrees with the plain 174; aligned count in brackets, control 174 [70]): the tag test as a plain
// `switch` with one case or as `== 1` 336/336 [73]/[77] (the state arrives in EAX and moves to EDI, the tag takes ECX); the
// flags through an inline accessor taking the group index, returning the flags or the group 174/174 (same code). With the
// switch, all 24 orders of border, aspect, distance and offset ahead of it 334 to 336; the offset and switch first 341;
// the selector in a short or long local 336/336; the offset as a ternary or an if/else 336/336; a switch with a default
// 336; a `(long)` selector with `== 1` 336; the direction reference (and the origin's) declared at the top 333. Only an
// order with one of the two state reads after the switch keeps the state in ECX: aspect after it 176 [63]; distance
// after it 171 [61], and then the mode flags load into EAX as retail's do (not kept: three points, by a declaration
// moved by score; the distance's load then sits after the branch where retail has it ahead). On that form: a long
// height limit 240 [59]; the near plane's normal assigned straight into the plane with the dot on the plane 338 [95];
// both 249 [30] (1463 bytes): the plane blocks, both limits and every register are then retail's, and the 30 left are
// the distance's load and the size, which retail keeps in the frame's lowest slot and reloads around the two rounding
// calls (BLOCKED-BY 0x48e70). The same two forms on this body 255 [32], on the plain switch 219 [41], with the aspect
// after the switch 254 [32]; the first plane's dot on the local normal 202 [117]. Nothing kept: every count but 171
// is higher. Next, once 0x48e70's definition is `inline`: start from the switch with the long limit and the direct normal.
// MiniMax E (11-10): the parameter layout is retail's, confirmed from the compiled prologue, which matches
// retail opcode for opcode: `fld dword ptr [ecx + 8]` for the aspect, `mov edi, [esp + 0x34]` for
// the tag and the matrix read through EBX. So ECX is the state, EBX the matrix and [E+4] the tag in both, and
// nothing here is blocked by the declaration. Most of the 174 is the reduced image's relocated globals (ours
// 0x4efb28 against retail 0x4c9798).
// MiniMax E (11-10): no lower score (174). A 20-minute permuter (107 tries) found one improving form, a
// `volatile short` store to g_51f3c8[1][2], which is a banned form and was reverted; the same run's dot3f
// argument swap is byte-neutral here (174 with and without) and was reverted too. What is left is the mode
// flag's register (retail keeps g_4c1a80 in EAX and shifts it there, ours in EDX) and the mode/extent homes
// (+0xc/+0x14 in retail, +8/+0x10 in ours); the EAX form needs the distance local declared after the switch,
// a declaration moved by score, which the round above records as not kept. Nothing kept.
// MiniMax R (11-10): no new form, no lower score (174): the mode flag stays in EDX in all 16 expression forms and 5 position variants tried, where retail keeps it in EAX; the reduced image also forces the global address fields, which are most of the 174.
// Round 109 (m3 lane 12, fast; 174): the flag test through a file-local forceinline predicate returning the bool 174
// (same code); the distance read after the tag test on this if form 333 (1488 B, the state leaves ECX); the far plane's
// normal copied straight into the plane with the dot on the direction 339 (1459 B); the direction and origin read through
// the matrix at each use 331 (1491 B); the border as `width / maximum * 0.75f` 174 (same code: both spellings divide
// first, retail multiplies first). Most of the 174 is one byte of drift: retail's `mov eax, [flags]` is the 5-byte form
// and ours `mov edx, [flags]` the 6-byte one, so every later address field sits one byte off and counts.
// MiniMax C (11-14): no new form, no lower score (174); the mode flag still loads into edx where retail
// keeps it in eax, and that one byte of drift offsets every later address field; a 300-second permuter
// run (3 tries) found no better form.
// MiniMax I (11-14): no new form, no lower score (174): the mode bit still loads the flags into edx where retail uses
// eax, so that load is a byte longer and every later address drifts, and the mode and the size still take the frame's
// slots in the other order.
// m3 lane 11 r102 (fast, 0x1ddd0 MATCH each time; still BLOCKED-BY 0x48e70, machine 1's file this round): one named bool
// for the small-target test at its three uses 333 (1489 B); the size clamp with the upper bound tested first 248 (1479 B,
// retail's length, but the clamp then stays in XMM where retail stores it to the frame and reloads it around the rounding
// calls); both limits from one if/else 346 (1458 B); the clamp as two independent ifs 258 (1484 B); the direction reference
// declared ahead of the first plane rows and used there too 326 (1483 B). Same code: `maximum` declared ahead of the limits,
// the two rounding results named before their 64-alignment.
// b7E (11-17) walk against retail, fast build on r829 (174, 1486 B vs 1479; no source change, so no other function
// moves): no logic, field, condition, loop or constant difference found; walked block by block. Fields and offsets:
// aspect state+8 (0x48ee3), distance state+0x64 (0x48eeb), offset state+0x28 under tag word +0x8e == 1
// (0x48ef6-0x48f13), extent state+4 and cone state+0xc (0x48ff4-0x4901c), tag+0x14 for mode 1 (0x48fdd), tag+0x10 for
// the size, tag+0x2c/+0x30 for the near plane, direction matrix+0x28 and origin matrix+0 (ebp = ebx+0x28), all as the
// draft. Constants read from retail and equal: 1.0, 0.5, -FLT_MAX (distance 0 fallback), 1/3, 0.125, 128, 16777215,
// 0.75, 0.01, 100, -100 and 1000. Order and rounding: aspect rounds to float at 0x48f0d, the extent stays on the x87
// stack unrounded to 0x4901c (ours compiles the same), the size rounds at 0x490a7 and the maximum at 0x49071, then
// width = size / aspect is rounded first and height = size * aspect second, the fix-up compares the width with the
// height (0x4912e) and divides by the other side, the border is width * 0.75 / maximum (0x49230-0x49238) and both
// planes' dot sums run k, then j, then i, all as ours. The callers pass the second argument in ECX (state) and the
// first in EBX (matrix) at 0x3129a and 0x32905, the tag on the stack, so the parameter order is retail's. Two things
// retail shows that the source keeps for a reason: the border's zero is stored to its frame slot at entry (0x48efe,
// read back at 0x49240) and the offset's zero is copied to xmm3 before the tag test (0x48f0a) and overwritten at
// 0x48f13, so `border = 0.0f` and `offset = 0.0f` ahead of their tests are retail's own shape, not a steering value; as
// ternaries with no earlier zero they score offset 323, border 320 and both 187 (1479 B, but a 0x24 frame against
// retail's 0x28), not kept. The one real type difference: retail's maximum height is a full dword (0x49116, 0x49194),
// ours a short; both hold only 128 or 512, so it changes no behaviour, and the long form scores 250, so it stays a
// short (a type change that only raises the score). No include, helper or double added; the parameter conventions of
// 0x48ee0 and its callees are unchanged.
// M405: Source flags/bit sampled before projection slots 0/1/2/4.
// Scores 174/174/174/174/174/174/174/174. NOT kept: ties or regressions; original body restored.
// MiniMax D (11-17): no new form, no lower score (174 in both images); retail's frame homes (size, mode, maximum and
// extent one slot further down than ours), the mode flag's register (EAX there, EDX here) and the near plane's dot
// product (retail reads the published normal back) still differ, and the permuter found nothing in 300 s.
// m3 lane 7 r178 (fast; control 174, of which 81 lines are real and the rest only a relocated address): nothing kept, body unchanged.
// HELD (needs machine 1's OK): `inline` on 0x48e70's definition in src/unknown_02b400.cpp, measured in the real linker for the first
// time (a diagnostic, restored after each trial; full diff with the body below in build/r178/held-48ee0.patch). With it 0x48e70 still
// matches, this body is 175 (1496 B) with retail's reloads around both rounding calls and the size in the frame's lowest slot, and
// the only other caller 0x184440 reloads its scale and both clamp constants after each call as retail does (0x184f0d, 0x184f68;
// positional 965 -> 1014, aligned with addresses masked 710 -> 699). On that diagnostic: long height limit, border product in
// parentheses, `raw` with one nested ternary clamp and the near normal straight into the plane with the dot on the plane 148 at
// retail's 1479 B; that with the near distance read at each use 130 (plain 130; aligned 16, the held form); with lane 1's
// `__assume` on the named near distance instead 102 = 9 real lines: the flags in EDX (4) and the distance/extent slots swapped (5).
// Tag test on the 102 form: a one-case switch, a switch with a default, a named long selector, `== 1`, `(long)x == 1`, an enum
// compare, an enum switch, `== (short)1`, `(word)x == 1` 327 each (the named long selector with `== 1` 314): every compare of the
// word with the literal 1 loads the flags into EAX as retail does but blocks ECX (`== 1` shows `mov ecx, 1; cmp [tag + 0x8e], cx`
// and stores the mode from ECX), so the state arrives in EAX and moves to EDI; the diagnostic `case 5` gives state ECX, tag EDI and
// flags EAX at once. Subtract spellings (`- 1 == 0`, `!(x - 1)`, a switch on `x - 1` with case 0, a ternary, an empty then-arm, a
// long accessor) 102 each: state ECX, flags EDX. `long type = x - 1; if (!type)` 321, `if (--type == 0)` 321, `x >= 1 && x <= 1`
// 327, `x > 0 && x < 2` 343, `(dword)(x - 1) < 1` 334, a short accessor with `- 1 == 0` 330.
// With the switch: depth-bound reads through a second pointer 327 (same code); far distance named 327; near distance named ahead
// of the scale 339, both named 339; the mode's middle value changed to 3 (diagnostic) 327; the three limit tests as mask tests
// (diagnostic) 307, with the mode flag test too 323, with the mode value too 323 (the state then arrives in EDX, ECX stays
// blocked); short mode 244; parameters declared (tag, matrix, state) or (matrix, tag, state) with both callers 327 each; the member
// form with the state as `this` 327 (with the subtract test 104).
// Slots on the 102 form (retail gives the upper of the two reused limit slots to the distance, ours to the extent): limits renamed
// 102, distance renamed 102, extent renamed 102, height limit declared ahead of the width limit 102, extent declared ahead of the
// distance 102, `x + extent` 102, distance ahead of the aspect 104, extent as one ternary sum 329, extent copied into a second
// variable ahead of the limits 102 (same code), the raw size through an inline helper taking the extent by value 102 (same code),
// limits, maximum and clamp through one inline helper with reference outputs 441 (1317 B). On the inline-only form: long height
// limit 255 (it alone sets the two limit slots), border parentheses 180, raw clamp 244, direct near normal 338, long limit with the
// raw clamp 157. Accessors between the tag test and the flags on the 102 form: forward vector accessor for the rows, group pointer
// accessor, group flags accessor, bool mode-off predicate, state offset accessor 102 each (same code); a negated mode-on predicate 290.
// Left under the held change: a source form for the named near distance (retail loads the three direction components and
// multiplies each by the distance in XMM0, as the first plane does with its constant), the flags' register and that slot order.
// M419: shared first sizing bit 180; sampled sizing word and first bit 180; separate size-limit branches 180;
// first bit with dword height limit 243; reused pixel cap before height alignment 323, with dword height 351;
// pixel cap between rounding calls 323; cap with separate width alignment 323.
// NOT kept: every form regressed. Mode flag register, mode and size homes, and rounding reloads still differ.
// M3 lane 4 r218 (fast with 0x48e70/0x1cf50, both matched; control 174, aligned 70, masked 36): the border expression is
// reassociated here (divide, then 0.75) where retail multiplies first: `0.75f * (real)width / maximum`, `0.75f / maximum *
// (real)width` and `(real)width / maximum * 0.75f` 174 each (same code); `(real)width * (0.75f / maximum)` 232 (1495 B);
// the product and the divide as two statements 180 (1491 B, retail's mul-then-div, aligned 63, masked 31), with the
// width converted first 180. On base / on that form, the second plane set from the direction with the dot through the
// plane, the direction, or the read-back normal: 343/339/343 and 337/329/337 (state leaves ECX). Body unchanged.
// m3 lane 9 r145 (fast with 0x48e70 and 0x1cf50, both matched; control 174, 105 rows with addresses masked): retail reads the size and
// the aspect back from their homes after each 0x48e70 call (our build keeps both in XMM registers across it: the link-time code
// generator knows 0x48e70 leaves them alone), and its clamp stores the size to its home in both arms. On this body: the height
// limit as a long (retail's dword load at the product) 250 (217 masked: the limit slots move); the clamp as one nested ternary 174
// (same code); both 250; the clamp through a reference-bound `real &` to the size 180 (1491 bytes); the mode as one ternary chain
// 174 (same code). BLOCKED-BY 0x48e70's link-time register summary (held `inline` on its definition, machine 1's file).
// M3 lane 1 r129 (fast with 0x48e70, 0x1cf50 and 0x1ddd0, all matched, and 0x184440 965 throughout; control 174, aligned
// 70/68 = unmatched instructions ours/retail, masked 36/34; body unchanged). Retail's second plane copies the direction
// straight into the plane, reads the three normal components back from it for the dot and keeps them in XMM3/XMM4/XMM6
// for the scale products, with the point's three direction components loaded into registers first (y, z, then x).
// Re-measured in this tree: the direction straight into the plane with the dot through the plane 343 (1458 B, frame
// and entry change at +0x2), the same keeping the normal local 343 (same code), the dot through the direction 339
// (1459 B); new: the first plane's dot through the local normal instead of the plane 213 (retail's 1479 B, aligned
// 123/122); the second point's rows in retail's y, z, x order 178 (1489 B, aligned 66/63); the normal copied first and
// the point's rows read from it 337 (1469 B, entry changes at +0x2); the rows written origin first 174 (same code).
// Still BLOCKED-BY 0x48e70's link-time register summary (machine 1's file): with our build keeping the size
// and the aspect in XMM registers across its two calls, the true second-plane form moves the parameters' registers.
// M3 lane 3 r275 (fast with 0x48e70/0x1ddd0/0x1cf50 MATCH; control 174): the border written `w / maximum * 0.75f` and
// `0.75f * w / maximum` 174/174 (same code: the compiler divides first either way); `(real)((real)w * 0.75f) / maximum`
// 180 (1491 B): retail's multiply-then-divide, and the second point's direction rows then load into xmm2/xmm3/xmm4
// as retail's do, but the second plane's tail grows; that with the second plane's dot through the plane 337 (1463 B,
// entry changes at +0x2); the near distance named ahead of the depth scale and used in it 189 (1484 B).
// M3 lane 1 r132 (fast with 0x48e70 MATCH; control 174): retail's clamp compares the upper bound on both paths (the
// lower arm jumps to the comiss) and stores the size back in both of its arms, so it is two separate bounds on a size
// held in memory; written as two ifs (no else) 258 (1484 B: the size still stays in XMM0, BLOCKED-BY 0x48e70); with the
// border product in parentheses as well 265; the border product in parentheses alone 180 (1491 B: the multiply then
// comes first, as retail); that with the height limit as a long (retail's dword) 243 (1481 B); the mode as one nested
// ternary 174 (same code).
// M3 lane 11 r133 (fast; control 174, aligned 89, register-masked 77; body unchanged). The fixed-size test named once as a
// bool for both limits and the later branch 333 (1489 B, the entry changes at +0x2); that with the height limit as a long
// 295 (1461 B); both limits set in one if/else 346 (1458 B); the two rounded sizes as a call statement and a rounding
// statement each 174, with both calls ahead of both roundings 174 (same code each); the border test as `mode != 0` 175.
// Frame reading (two pushes down): retail has the size at +8, the mode at +0xc, the converted width limit at +0x10 and the
// extent, later the width limit, at +0x14; ours has the mode, the converted limit and the extent one cell lower each and the
// size at +0x14, because retail's size lives in memory across the two 0x48e70 calls and ours in XMM0. Still BLOCKED-BY
// 0x48e70 (machine 1's file).
// M3 lane 14 r128 (fast with 0x48e70 and 0x1ddd0, both MATCH): the border as `0.75f * (real)width / maximum` 174,
// `(real)width / maximum * 0.75f` 174, `0.75f * width / maximum` 174 (all the same divide-then-multiply code); the border
// scaled in one statement and divided in the next 227 (1489 B); the mode defaulted to 0 at its declaration with the two
// set arms 180. Still BLOCKED-BY 0x48e70 (its file is machine 1's again this round).
// M3 lane 16 r157 (fast with 0x48e70, 0x1cf50 and 0x1ddd0, all MATCH; control 174, aligned 70, masked 33; body unchanged).
// Screened without touching 0x48e70's file: as a diagnostic the two rounding calls went to an undefined stand-in declared
// in this file (the reduced link leaves it unresolved, so this caller treats it as opaque; restored after every trial).
// Stand-in alone 181 (1501 B, aligned 50). With lane 7 r178's held forms (long height limit, the raw size and one nested
// ternary clamp, the border product in parentheses) and the near distance read at each use with the second plane's normal
// copied straight in and its dot taken on the plane 128 (1479 B, aligned 17, masked 7: that round's 130). With the near
// distance kept in its named local instead (those forms, the second plane straight in, the dot on the plane) 100 at
// 1481 B, aligned 9, masked 0: only the flags in EDX for retail's EAX (4 instructions) and the distance and extent homes
// exchanged (5), the state lane 7 reached with an `__assume`; with the real callee defined inline that round measured the
// same forms at 148, so the near products' operand order moves with the symbol count. On the 100 form: the products
// written near distance first 100 (same code); the inverse read from the state field instead of the distance local at
// both uses, at the quotient only, at the test only 335/328/335 (the entry reloads the field); 1/2/3 redundant paren
// pairs in the first block 100/146/100 (two pairs: 1479 B, but the later planes' registers move and the flags and homes
// stay). Slot reading on that form: retail shares the extent's home with the width limit and the distance's with the
// height limit, ours the other way round. Still BLOCKED-BY 0x48e70 (machine 1's file; lane 7 r178's held patch).
// Mode bit as unsigned or signed storage with Boolean, zero comparison, positive arm,
// or Boolean tag result 302/299/190/299 and 299/299/307/299. NOT kept: the EAX forms
// use an early high-byte test, and the remaining forms keep the EDX load.
// M3 lane 3 r283 (fast, control 174; body unchanged): the mode declared at the top with the border 174 (same code); the
// two limits' product named once and used three times 174 (same code); the depth and w constants stored once after the
// if/else instead of in both arms 254 (1443 B); near and far distances named ahead of the scale 189 (1484 B); the four
// texture-slot clears as one memset 173 (only the mode byte's store moves among the clears; one point, not kept).
// Still BLOCKED-BY 0x48e70 (machine 1's file).
// M3 lane 15 r119 (control 174 fast and plain; the reduced image builds the far-plane point block differently for this
// function, so a gain needs the plain check): the distance test as `0.0f != distance` 175; the border's product kept
// ahead of the divide by a cast on the product 180 (1491 B, plain: retail's multiply-then-divide, but the far-plane point
// loads then reorder); the half-angle test, the second mode test and the two clamp tests with the constant on the left
// 174 each.
// Mode-bit or full flag-word reads before projection stores 0/1/4/7: 174/174/174/174 in each form.
// NOT kept: these observations leave the score, length and EDX load unchanged.
// M3 lane 5 r224 (h2m fast with 0x48e70 and 0x1cf50, both matched; control 174; still BLOCKED-BY 0x48e70 for the homes and
// reloads): aimed at the flags register (EDX for retail's EAX): the mode chain ahead of the first plane's stores 326 (1479 B,
// entry changes at +0x2); the inverse by if/else 174 (same code); the mode test as `!((flags >> 10) & 1)` 299 (1485 B);
// `long mode = 2;` with the other two values from a ternary under the negated flag test 291 (1484 B); the offset block
// read ahead of the distance 333 (1488 B).
// M3 lane 13 r137 (h2m fast; control 174): the height limit as a long with the size clamp as two separate ifs 251
// (1484 B); the mode byte stored right after the mode chain 339 (1488 B); the float width limit declared ahead of the
// height limit 253 (1484 B); the border as a conditional at its assignment 320 (1479 B, entry changes at +0x2); the
// offset as a conditional initialiser 323 (1488 B). Still BLOCKED-BY 0x48e70 (machine 1's file): retail keeps the
// size and aspect in memory across its two calls.
// M3 lane 5 r229 (h2m fast with 0x48e70/0x1cf50/0x1ddd0, all MATCH; control 174; still BLOCKED-BY 0x48e70): the mode-1
// tag test as `!*(short const *)(tag + 0x14)` 174; the overflow product written `height * width` 174 (same code each);
// the overflow test as `max_width * max_height < width * height` 176; `real border = 0.0f;` declared just ahead of its
// `mode > 0` test 316 (1479 B: the entry's zero store moves); the width limit's test without the `(bool)` casts 278
// (1479 B: the state then arrives in EDX).
// M3 lane 16 r170 (h2m fast with 0x48e70, 0x1cf50, 0x1ddd0, all MATCH; control 174): the offset as one conditional
// expression at its declaration 323 (1488 B, first +0x2a); the mode defaulted to 0 at its declaration with the two set arms
// 180 (re-measured); the forward vector at matrix+0x28 named once for the first block's seven reads 321 (1479 B, the entry
// changes at +0x2); the render flags word read once into a `dword` for the two limits 180 (1491 B); the upper clamp as a
// conditional expression in the else arm 174 (same code). Still BLOCKED-BY 0x48e70.
// M3 lane 1 r159 (H2M_FAST with 0x48e70 and 0x1cf50, both MATCH; control 174): the mode test as a mask (`!(g_4c1a80 &
// 0x400)`) 299 (1485 B); the offset test as `== 1` 336 (1491 B); the mode's else arm as one boolean assignment 174 (same
// code); `maximum` converted ahead of the height limit 253 (1484 B); the size's first term named for the product 174 (same
// code). Still BLOCKED-BY 0x48e70 (its file is on this round's avoid list). Body unchanged.
// M3 lane 11 r163 (H2M_FAST with 0x48e70 and 0x1cf50, both MATCH; control 174; body unchanged; still BLOCKED-BY 0x48e70,
// whose file is on this round's avoid list). Lane 16 r157's diagnostic base rebuilt (the two rounding calls to an
// undefined stand-in, the long height limit, the raw size with one nested conditional clamp, the border product in
// parentheses, the second plane's normal copied straight in with its dot on the plane) 100 at 1481 B, aligned 9: the
// flags word in EDX for retail's EAX and the distance and extent homes exchanged. Reading of the flags register: the
// flags word and the tag word are scratch temporaries, handed out by a cursor that rotates EAX, ECX, EDX; the tag word
// takes EAX and leaves the cursor on ECX, which the state pointer holds, so the flags word gets EDX. Retail's EAX needs
// one more integer scratch temporary between the two (or the tag word coloured as a variable). On that base: the offset
// read through a pointer local 100 (same code); through a file-local inline accessor returning a reference 100 (same
// code); the inverse through a file-local inline function taking the distance by reference 100 (same code): pointer
// temporaries fold away before the scratch registers are handed out. The tag test as a one-case switch 315 (1479 B,
// first +0x3), as `== 1` 315 (the state then arrives in EAX, as listed above). The extent renamed `breadth` or
// `a_extent`, the distance renamed `range` or `z_distance` 100 each (same code: the two homes do not follow the names).
// M3 lane 12 r185 (h2m fast with 0x48e70, 0x1cf50 and 0x1ddd0, all MATCH; control 174; body unchanged; still BLOCKED-BY
// 0x48e70, whose file is on this round's avoid list). Aimed at the flags word's register (EDX for retail's EAX), each
// the same code, 174: the flags global read into a named dword ahead of the mode chain; the shifted flags named; the
// negated bit named as a bool ahead of the chain; the bit test written `!= 0` without the bool cast; the tag word read
// into a named long ahead of its test; the same as a short. Reading of the scratch registers (handed out in code order
// from a cursor that steps EAX, ECX, EDX and skips a busy one): the tag word takes EAX and leaves the cursor on ECX,
// the state pointer holds ECX, so the flags word gets EDX and the cursor returns to EAX for the size-limit bits, which
// already sit where retail has them. Retail has the tag word, the flags word and the first size-limit bit all in EAX,
// which needs the cursor back on EAX twice: one more integer temporary ahead of the flags word and one more ahead of
// the size-limit bits, or neither the tag word nor the flags word taken from the cursor. Named locals fold away before
// the scratch registers are handed out, so none of the six forms adds one.
// Stage both rounding operands before either call, width-first or height-first,
// with direct alignment, separate height alignment, delayed width alignment, or
// both delayed, 174 each. NOT kept: the EDX load and size/aspect homes agree with
// the opening body.
// Initial direction captured once 378 or per row 399; whole scaled/negated/both row values 185/183/190;
// scale/negate/both helper results before publication 323/321/323. The row values retain the 0x28 frame. The EDX
// flag load and held rounding reloads remain.
// M3 lane 12 r187 (H2M_FAST with 0x48e70, 0x1cf50 and 0x1ddd0, all MATCH; control 174, aligned unmatched rows [exact,
// stack masked, registers masked] 70/55/36; body unchanged; still BLOCKED-BY 0x48e70, whose file is on this round's
// avoid list). Same code, 174 each: the aspect's root, the cone's tangent, the size's root, all three, the border's
// width and the maximum written without their `(real)` casts; the extent's sum with the cone term first. Reading of the
// flags register: the tag word's temporary takes EAX from the scratch cycle and leaves its cursor on ECX, which the
// state holds, so the flags word gets EDX; retail's flags word in EAX means its tag word did not come from that cycle,
// as a switch selector does not. The tag test as a one-case switch (on the short, or on a `(long)` cast of it) 336
// (1491 B) [73/58/37]: the flags word is then in EAX as retail's, but the tag is coloured ECX and the state EDI (it
// arrives in EAX), where retail has the state in ECX and the tag in EDI from the entry. Next, once 0x48e70 is
// `inline`: the switch with a form that ranks the tag above the state for the saved register.
// M3 lane 16 r189 (H2M_FAST with 0x48e70, 0x1cf50 and 0x1ddd0, all MATCH; control 174; 0x48e70's file is open this round,
// but `inline` on its definition stays held). The allocator reading of the scratch cycle: the tag word's temporary takes
// EAX and leaves the cursor on ECX (the state), so the flags word takes EDX. Forms: one direction reference declared after
// the inverse for every direction read 326 (1483 B, first +0x2); the mode started at 0 with the else arm dropped 180; the
// mode test as `!(bool)((flags >> 10) & 1)` 302 (1489 B); the mode's tag test as `!*(short const *)(tag + 0x14)` 174
// (same code); the border as one conditional initialiser at its use 320 (1479 B, first +0x2). Still BLOCKED-BY 0x48e70.
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
    long mode;
    if ((bool)((~(g_4c1a80 >> 10)) & 1))
        mode = 2;
    else if (g_485a40 > 0.0f && *(short const *)(tag + 0x14) == 0)
        mode = 1;
    else
        mode = 0;
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
    long width = (function_48e70(size / aspect) + 63) & ~63;
    long height = (function_48e70(size * aspect) + 63) & ~63;
    if (width * height > max_width * max_height)
    {
        if (width > height) width = (max_width * max_height / height) & ~63;
        else height = (max_width * max_height / width) & ~63;
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
    real scale = *(real const *)(tag + 0x30) > *(real const *)(tag + 0x2c)
        ? 1.0f / (*(real const *)(tag + 0x30) - *(real const *)(tag + 0x2c))
        : 1000.0f;
    plane = (plane3f *)&g_4c9798[20];
    real near_distance = *(real const *)(tag + 0x2c);
    point.x = direction.i * near_distance + origin.x;
    point.y = direction.j * near_distance + origin.y;
    point.z = direction.k * near_distance + origin.z;
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
