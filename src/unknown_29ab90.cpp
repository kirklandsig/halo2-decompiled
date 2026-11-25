// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include "normalize3d.h"
#include <math.h>

struct s_29ab90
{
	byte field_0[0x1c];
	real field_1c;
	byte field_20[0x2c];
	real field_4c;
	real field_50;
	byte field_54[0x10];
	real field_64;
	real field_68;
	real field_6c;
};
long function_1f5f30(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2,
	vector2f const *arg_3, vector2f const *arg_4, vector2f const *arg_5,
	vector2f const *arg_6, real arg_7, bool arg_8);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
real function_30bf0(vector3f *arg_0);
bool __stdcall function_2922b0(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, real *arg_4, real *arg_5, bool arg_6);
void function_29b2e0(long arg_0, long arg_1, vector3f const *arg_2,
	vector3f const *arg_3, real arg_4, vector3f *arg_5);

PRIVATE __forceinline real function_29ab91(vector3f arg_0)
{
	real local_0 = (real)sqrt(arg_0.i * arg_0.i + arg_0.j * arg_0.j + arg_0.k * arg_0.k);
	if (fabs(local_0) < 0.0001f) local_0 = 0.0f;
	return local_0;
}
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))

// score 286 (was 286): r130, r275 and r164 unchanged. The early object/settings spills and subsequent magnitude, saved direction and planar homes still differ.
// M3 lane 13 r148 (plain checks; the fast screen builds 0x29b2e0 at 1309 bytes here): local_20 built from local_4 ahead of
// the 0x29b2e0 call with no local_27 copy 378 (1816 bytes); each local_5 component computed and clamped in one PIN 338;
// local_6 copied from local_5 (the 0x4687a4 vector read once) 286 (same code); the 0x30bf0 fallback copying +0x290 directly
// 286 (same code); *arg_5 written from the +0x66c copy 320. Frame reading (one-object LTCG kit with the callees' objects
// and the ballast reproduces this build's homes exactly): homes follow compiled reference counts, local_4 18 against
// local_5 15, and the shared local_12/local_27 slot 10 against local_6 7; retail has local_5 below local_4 and local_6
// below that shared slot, and keeps local_27.i in ESI across the 0x29b2e0 call where this build stores all three.
// MiniMax E (11-20): no new form, no lower score (286): the frame's homes are still permuted - this build keeps the
// 1e5450 result at [esp+0x14] and the magnitude at [esp+0x10] where retail has them the other way round, the object
// view at [esp+0x24] where retail has [esp+0x1c], and the two copied vectors the other way round too (ours copies the
// actor's direction into [esp+0x38] and the global vector into [esp+0x44], retail the reverse), so declaring the two
// vectors the other way round measures 290 (their copies then come out in the other order) and is reverted; site 1's
// planar normalize still keeps its own zero path where retail calls the shared 0x2b400 helper (BLOCKED-BY 0x2b400),
// so the body stands.
// MiniMax S (11-19): no new form, no lower score (286): site 1 planar normalize written out with its own zero path measures 309, because retail reloads the second component after storing the first while ours folds that multiply into the memory operand; the frame homes still take each other slots and the working-vector group is still a rotation off retail, so the body stands.
// MiniMax G (11-18): no new form, no lower score (286): the frame's homes are still permuted (ours [esp+0x24]
// where retail has [esp+0x1c] for the unit pointer and ours [esp+0x14] where retail has [esp+0x10] for the
// magnitude), the working-vector group still sits at [esp+0x3c]..[esp+0x64] where retail has
// [esp+0x48]..[esp+0x38], and site 1's planar normalize still keeps its own zero path where retail uses the
// shared helper in include/unknown_11cc90.h (BLOCKED-BY 0x2b400); the body stands.
// MiniMax B (11-18): no new form, no lower score (286): the magnitude and the settings pointer still take each other's frame homes (ours [esp+0x10] and [esp+0x14] against retail [esp+0x14] and [esp+0x10]), the unit pointer still sits at [esp+0x24] where retail has [esp+0x1c], the three working vectors are still a rotation off retail's, and site 1's planar normalize still keeps its own zero path where retail calls the shared 0x2b400 helper in include/unknown_11cc90.h: BLOCKED-BY 0x2b400.
// MiniMax G (11-19): no new form, no lower score (286): the frame's homes are still permuted (ours [esp+0x24] where
// retail has [esp+0x1c] for the unit pointer and ours [esp+0x14] where retail has [esp+0x10] for the magnitude), the
// working-vector group still sits at [esp+0x3c]..[esp+0x64] where retail has [esp+0x38]..[esp+0x48], and site 1's
// planar normalize still keeps its own zero path where retail uses the shared 0x2b400 helper in
// include/unknown_11cc90.h (BLOCKED-BY 0x2b400); the body stands.
// MiniMax I (11-17): no new form, no lower score (286): the same four bytes of frame separate ours from retail (ours
// [esp+0x24] where retail has [esp+0x1c] for the unit pointer and ours [esp+0x14] where retail has [esp+0x10] for the
// magnitude), the working-vector group is still a rotation off retail's, site 1's planar normalize still keeps its own
// zero path where retail uses the shared 0x2b400 helper in include/unknown_11cc90.h, a file this lane may not edit
// (BLOCKED-BY 0x2b400), and a permuter over 23 tries on this body found nothing below 286, so the body stands;
// MiniMax D (11-17): no new form, no lower score (286): the frame's homes are still permuted the way the notes
// below describe (ours [esp+0x24] against retail [esp+0x1c] for the unit pointer, ours [esp+0x14] against
// retail [esp+0x10] for the magnitude, and the working-vector group at [esp+0x3c]..[esp+0x64] where retail has
// [esp+0x38]..[esp+0x58]), and site 1's planar normalize still keeps its own zero path, which needs the shape of
// the shared 0x2b400 helper in include/unknown_11cc90.h, a file this lane may not edit. BLOCKED-BY 0x2b400.
// MiniMax E (11-15): no new form, no lower score (286): the magnitude and the settings pointer still take each
// other's frame homes (ours [esp+0x10] and [esp+0x14], retail [esp+0x14] and [esp+0x10]), the two copies of the
// global still take each other's homes (ours [esp+0x38]..[esp+0x40] for the position copy and [esp+0x44]..
// [esp+0x4c] for the global's, retail the other way round), and the planar operand order and the xmm numbers of
// the floors still differ; site 1's planar normalize still keeps the zero path as the fall-through with a result
// home of its own where retail stores the zero into the magnitude's own home, which needs the 0x2b400 shape of
// the shared helper (include/unknown_11cc90.h, another file): BLOCKED-BY 0x2b400. A 35-try permuter run found no
// better form (best 284 differing instructions = the baseline).
// MiniMax R (11-14): no new form, no lower score (286): the frame slots still differ (ours reads [esp+0x24] and
// [esp+0x14] where retail reads [esp+0x1c] and [esp+0x10]), the saved-direction homes differ and the planar
// operand order differs; the fast check prints caveats that the callees 0x26c180 and 0x1f9e70 build differently in
// the reduced image, so the full check decides the true score.
// MiniMax D (11-11): no new form, no lower score (286): the object pointer's spill slot still sits eight bytes above retail's and the 0x1e5450 result four bytes above it, and the call result and the magnitude still take each other's registers; declaring the direction local ahead of the position copy puts the two copies in retail's homes but emits them in the other order (290), and declaring them first and assigning them in retail's order keeps the copy order but not the homes (286).
// MiniMax D (11-10): no new form, no lower score (286): the call result and the magnitude still take
// each other's homes, the object view still sits eight bytes higher, and the position copy and the first
// direction copy are still exchanged.
// MiniMax I (11-11): no new form, no lower score (286); the two vectors copied from the same global still take
// each other's frame homes - ours keeps the published vector in the lower slot and the one that feeds the
// history term in the upper one, retail keeps them the other way round, and only a declaration order gives that.
// MiniMax B (11-12): no new form, no lower score (286): the saved direction's planar copy taken as one aggregate (310)
// and the two copies of the global with their declaration order exchanged (286, the same code) leave the score where it is.
// MiniMax A (11-13): no new form, no lower score (286): the saved-direction homes, the planar operand order and the
// xmm numbers of the floors differ; the callee that held this one back now matches, so what is left is the homes, the
// operand order and the frame.
// Earlier 286 (was 286; 320, 306 and 345 before): frame 0x64 and retail's entry copies agree (1865 bytes against 1863); saved-direction homes, planar operand order and xmm numbers of the floors remain.
// b3M walk (11-06), every block against retail 0x29ab90..0x29b2d4: entry copies/0x658-0x678 history shift,
// actor+0x26c test, 3-D magnitude (k,j,i sum, unrounded fabs test), 0x5d0/*arg_7 scale test, 1f9760 call, planar
// normalizations (j,i sum), 2922b0 call, 30bf0 == 0 fallback, 0x47c branch, floor/ceiling clamps, 29b2e0 call
// (+0x290 pointer in EBX), field_50/68/6c/64 arithmetic, -1..1 clamps, 0x656/0x66c/output stores all agree with
// retail. ONE behaviour difference, fixed: 1f9e70's facing/desired vectors were swapped. Retail pushes &local_15
// (the +0x290 planar direction, 0x29b0b6..0x29b0be lea [esp+0x28] at shift 0xc = esp+0x1c) first and &local_20 (the
// new heading, lea [esp+0x3c] = esp+0x2c) second, so arg_4 (facing) = local_20 and arg_5 (desired) = local_15; the
// callee (0x1f9e70) takes the velocity dot with arg_5. The score is unchanged (306): the compiled pushes now match.
// The written-out PIN clamp helper became the file-local PIN macro (as bipeds.cpp does) at 306, and the written-out
// 2-D normalizer became normalize2d_fast from unknown_11cc90.h (already included) at 320 / 1866 bytes (retail keeps
// the normalizing path as the fall-through, the header helper flips it); written-out form was 306.
// MiniMax I (11-06): the vector copy order swapped and the magnitude declared early were tried
// (310, 306); the frame homes of the call result and of the x87 temporary still differ from
// retail's; no lower score.
// Earlier transport comparison: retail takes the first argument in EAX, keeps a frame of 0x64 and returns with ret 0x20, ours uses the fastcall ECX/EDX pair, a frame of 0x5c and ret 0x1c; the declaration lives in another file and the signature may not change, so the prologue, the argument homes and the frame size cannot be reproduced.
// MiniMax D: the requested direction read through its own address (a stack argument, as
// retail's) 366, not kept (an address-taken read needs a byte match); the magnitude summed in
// i/j/k order alone 383.
// frame 0x5c/0x64, actor/requested-vector transport and x87 homes differ.
// Earlier: frame 0x5c versus 0x64, actor/input transport and x87 homes differ.
// Earlier 396 from 428: magnitude helpers now give retail's copies, memory-operand products and epsilon;
// magnitude helpers now give retail's copies, memory-operand products and epsilon;
// magnitude helpers now give retail's copies, memory-operand products and epsilon;
// frame 0x5c versus 0x64 and the convention remain: ours takes arg_0 in ECX and arg_2 in EAX (kept in
// EBX) and folds the +0x290 vector into [ebp+0x290]; retail keeps that pointer in EBX, loads arg_2 into
// ESI after the position call and takes only arg_0 in EAX. x87 homes also differ.
// Double input components 437; scalar component parameters 469; double product lifetimes 437; repeated field access 434; size optimization 802.
// Degenerate normal first 434; independent speed components 434; initial/requested/output memcpy 434/434/434;
// fallback direction at use 441; shared initial vector value 434; native sign word 440.
// Double magnitude/planar/both returns 434/434/434; double magnitude carrier 471;
// named full squared sums magnitude/planar/both 434/437/437; planar input/output 434.
// DL131: native actor view 434;
// shared history weight 428; named current weight 434; both weights 428; native actor with
// history/current/both 428/434/428; actor/weights/shared fallback 428. Keep the history coefficient
// once for all six original uses; original arithmetic grouping, source vectors, stores, helpers and
// call arguments remain unchanged.
// Two-use stored/minimum/acceleration scalars and combinations 428/429/429/429/429/430/430; native velocity value 454.
// Existing native object getter/record with native actor, read-only binding
// and early velocity field lifetimes: eight forms all 428. Opening body and
// private math helpers restored; frame/input and x87 homes remain.
// DL134: unrounded magnitude/planar/both root boundaries 428/433/433; real sign 432;
// named correction root/term 428/428; scale output return 428; ordered clamp returns 428.
// Native magnitude/planar/scale reference input contracts independently and
// combined, with native scale output reference: eight forms all 428.
// Native real component inputs: magnitude memory/math order 472/472; planar memory/math
// order 428/428; both memory/math order 472/472; crossed declaration orders 472/472.
// Native coordinate-array magnitude/normalizer/both/scale/all interfaces 428 each;
// array normalizer with planar transfers 447; array normalizer plus planar names 428;
// both arrays, native transfers and planar names 447.
// First/second/both query Boolean declarations at entry 428 each; native acceleration
// aggregate 428, with those three lifetime forms 428 each; complete graph with
// reversed named Boolean declarations 428. Original helpers and all calls restored.
// Round 237: Published query-output lexical groups (Boolean pair, speed/turn limits,
// query vector), independently and combined 428/428/428/428/428/428/428;
// reversed independent output declarations 428. Original output homes restored.
// Scalar outputs magnitude/planar/both 428/428/428; magnitude reference/with planar output 428/428;
// output contracts with magnitude/planar/both native values 477/528/574. All restored.
// Direct actor-direction reads and initial vector order 428/429/429/429/429;
// retained direction pointer with two vector orders 435/436; direct reads with
// shared initial vector 428. Original body restored.
// Round 158: Complete parallel current sums, current/history products and shared-current-factor combinations
// all 428; coherent entry-provider/target inline specifications and separate noinline mode-query declaration
// all 428. Scores [428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428, 428]. Original
// executable body restored.
// Round 353: native actor/object handle and word-mode parameter domains in
// all seven combinations 428 each; signed native handles with word mode 428.
// Original definition and sole disabled declaration restored.
// Round 146: by-value magnitude helper 477; non-const/early/const-actor +0x290 pointer 428/426/428;
// existing normalize2d_fast for the planar steps 428 (retail's products, wrong branch order);
// planar length through magnitude2d with inverse-first/last products 428; plus float magnitude
// products 463; plus by-value magnitude 396 (kept); magnitude sum written i,j,k 396; magnitude
// zero test as an if statement 453 (retail's branch and slot, one instruction off elsewhere).
// With the disabled caller enabled (diagnostic) the convention stays ECX/EAX.
// Round 231: direction/velocity/paired field references 396/396/396; history/direction-
// history/velocity-history/all three references 397/397/397/397; all plus settings reference 396.
// Original body restored.
// Round 148: the +0x290 pointer through the actor view's field 396; local_4 copied from that field
// directly 396; the magnitude through length_sq3f 396; local_15 from the field directly 396; the scale
// helper taking its output first 396 (all the same code). Diagnostic only: arg_2 read once through its own
// address gives retail's convention (arg_0 in EAX, arg_2 on the stack) and length, 373 (difflib-aligned
// 402 -> 277); left then: the +0x290 pointer in EBX (ours folds it), frame 0x58/0x64, the magnitude's x87
// order and its zero test, and the magnitude in a dead parameter home.
// File default convention and complete scalar selections 396; result-first reusable
// clamp 439, with scalar joins/default convention/both 439. Clamp result assigned on
// all paths 451, with default convention or scalar joins 451. Those ten trial forms restored;
// private helpers and sole disabled declaration unchanged. Frame/input and x87 homes remain.
// Named ratio numerator 395; both complete turn factors 389; both forms together 383 kept.
// All three computed parts retain the original floating grouping and operands; frame/input homes remain.
// Round 267: scale vector return 412; planar vector return with length output 568;
// both vector returns 589; entry working-vector declarations 383; those three return
// graphs with entry declarations 412/568/589; both plus scale value input 710.
// Original helper interfaces, body and declarations restored; frame/input homes remain.
// Round 366: returned position producer 408; shared scale 383; both 408; full vector setter 399; position/setter 426; scale/setter 399; all 426; all with earlier reused magnitude 413. All restored.
// Round 245: existing scale formal orders 021/102/120/210 and clamp orders
// 102/120/201/210 all 383. Original helper definitions and every call restored.
// Frame 0x5c/0x64, actor/requested-vector transport and x87 homes remain.
// Round 208: all three native square-product carriers in six declaration orders 383 each;
// successive original K/J/I sum with those carriers 383; first-declared epsilon result on the
// product graph 439. All helper/body text restored; frame 0x5c/0x64, actor/input transport and x87
// homes remain.
// Round 368: settings-only magnitude 390; in-place epsilon result 454; separate turn/speed 385;
// scope/result 388; scope/separate 384; result/separate 448; all and native length names 382/382 (one-point
// gain rejected).
// Round 370: complete/default sign results with homogeneous planar multiplier placement and
// widened reciprocal carrier 383/383/441/441/383/383/441/441; original body and helpers restored.
// Round 377: common publication exit/object-first receivers/stored-turn result 383/444/396;
// paired exit+receivers/exit+turn/receivers+turn/all 444/396/455/455;
// all with read-only object receiver 455. All restored; frame/input/magnitude homes remain.
// Round 211: Complete history value alone/with shared current add/history scale/both
// 401/402/411/411; those four with a shared current coefficient 401/401/411/406. All original
// helpers, fields and arithmetic restored; frame/input and magnitude homes remain.
// Round 281: existing scale publication ikj/jik/jki/kij/kji 383 each; planar ji 383;
// scale jki/kji with planar ji 383/383. All helper bodies and the original movement body
// restored; frame/input transport and magnitude homes remain.
// Round 379: actor at later formal positions 1..8, preserving every other input order,
// 383/381/381/381/381/381/381/381. Positions 2..8 recover actor EAX but leave vector ECX,
// frame 0x5c and ret 0x1c; two-point gains rejected. Definition and disabled caller restored.
// Round 381: marked-function requested/movement/acceleration vector references, all seven
// nonempty subsets, 383 each; all vectors plus native scalar-output references 383.
// Every original signature, operation and disabled declaration/call restored.
// Round 153 (fast): vector argument through its address (diagnostic) 368, with an if-statement magnitude test 449;
// vector locals renamed into retail's slot order 383; output declared first 383; position declared first 383.
// Round 222: Selective ordinary-inline magnitude/planar/clamp boundaries, all seven
// subsets 383 each; all four private math helpers ordinary inline 383. Original
// forced boundaries, helper bodies and every call restored; frame/input homes remain.
// Round 174: coherent existing work/position, planar and magnitude home identities,
// all seven nonempty family subsets 383 each; complete graph with alternate role
// identities 383. Original names/helpers/body restored; frame 0x5c/0x64, actor/input
// delivery and native magnitude homes remain.
// Round 233: Distinct squared-speed/half-limit, later floor/heading and query-output
// identities in seven subsets, then alternate complete roles: 383/383/383/383/383/383/383/383.
// Original names, scopes, helper bodies and complete executable body restored.
// Round 182: input-updated clamp result in all six formal orders 438 each;
// lower-failure nesting with forward/reverse orders 438/438 (1814 bytes).
// Original repeated helper, all four uses and movement body restored.
// Round 238: Shared add/scale/affine blend with actual sum/current-workspace homes 5/5, 22/22,
// 6/6, 5/22, 22/5, 6/5, 5/6 and 22/6: 383 each. Original helpers, vectors and body restored.
// Round 240: Plain native-component blend/clamp loops: fused long/short/byte
// 409/479/479; blend before original clamps 383/415/415; fused with shared current
// coefficient/complete expression parts 409/409. Frame/input homes remain; original body restored.
// Round 251: native value-input scale 476; all six complete scaled-product declaration orders 476 each; complete scale plus planar staged products 476. Original repeated helper contracts and body restored.
// DL190: Native real root/magnitude-absolute/planar-absolute API subsets 1-7 all 383; all with native correction root/absolute 383. Original arithmetic and helpers restored; frame, requested-vector stack ownership and x87 homes remain.
// Round 442: Ordinary-inline declarations at the three query boundaries in seven subsets, then
// all plus turning publication, 383 each in eight compiled forms. All original declarations,
// helpers and body restored.
// Round 459: Native double correction-expression stages
// absolute/root/both/scaled/absolute-scaled/root-scaled/all-three/all-four
// 383/383/383/383/383/383/383/383. All executable trial forms restored.
// Lane 18 round 15 (fast with the callees): retail also reuses the dead homes of arg_2 and arg_8 for the two
// Boolean query outputs (and arg_8's for the planar magnitude). Magnitude helper's epsilon test as an if
// statement 454 (1847 bytes); the query's Boolean and vector outputs declared at function scope 383; only the
// vector output there 383; the planar input copied field by field 383; the 12-byte locals renamed to sort in
// retail's home order 383 (identical code: names do not order these homes); declared in retail's home order
// (local_5, local_4, local_6, local_12, local_7) 383 (identical: declaration order does not either).
// Diagnostic: arg_2 read once through its address gives retail's convention, 368 here, with arg_2 then in EBX
// for the whole body where retail keeps the +0x290 pointer in EBX and loads arg_2 into ESI after 0xb9dd0.
// Round 469: Native coordinate aggregate construction of working/output/query vectors in
// seven nonempty subsets, then all with shared final vector setters: 390/453/463/414/376/449/460/487.
// Working/query aggregates recover actor EAX and ret 0x20 at 376 (gain seven, rejected).
// On that graph, magnitude-result update/query outputs at entry/native vector-position names
// in seven subsets, then all with position at entry: 446/376/446/376/446/376/446/446.
// Independent native-vector producer orders 465/546/564/645 on that graph: 435/376/381/439.
// Original copies, helper bodies and call roles restored; frame and input transport remain.
// Round 288: On native working/query aggregates, magnitude/planar result pairs
// 415/376/379/379/415/416/379/379; current coefficient/history value/both 376/354/354;
// early magnitude result/root-initialized magnitude/planar result/current plus planar 393/445/355/355.
// Keep the complete history vector with the native working/query aggregates: 354 fast/plain,
// 1863/1863 bytes; original private math helpers, arithmetic groups and game-call roles retained.
// Round 292: on the retained history graph, native working/query copies and coordinate history
// in seven subsets 413/372/401/365/399/371/396; shared constant-vector observation 410.
// Original aggregate/native producers and all helpers restored; frame and magnitude homes remain.
// Round 294: isolated planar scalar products, ij/ji producer and ij/ji publication
// schedules, 354 each; four complete compound-product boundaries 354 each.
// Original three-use normalizer and complete body restored; frame 0x58/0x64 remains.
// Round 297: Actual acceleration workspace at entry/settings entry/movement-query entry, scalar stores or existing vector setter, 354 each; original-scope coordinate aggregate/setter 354/354. Original storage, native producers, helper bodies and complete body restored; frame 0x58/0x64 and magnitude/query homes remain.
// Round 299: On retained native working/query/history graph, magnitude
// negative-success/root-updated/zero-default/complete-arm results 357/426/380/354; each with planar
// zero-default result 348/421/420/345. Keep complete magnitude arms plus planar zero-default result, plain
// 345 at 1866/1863 bytes; all 11610 matches and every other metric preserved. The six-point form 348 is
// ineligible. Frame 0x58/0x64, initial vector/magnitude and query homes remain.
// Round 301: Direct shared clamp expansion at turn, complete final vector and both 345/345/345; separate
// turn/speed values alone or with each clamp boundary 351/351/351/351; separate values and private clamp
// forwarding to the shared expression 351. Frame remains 0x58 versus 0x64; restored.
// Round 120 (fast with the callees; convention agrees, so the slot forms were re-measured): the frame
// difference is one slot: the query's output vector shares +0x2c with the magnitude's input copy, where
// retail gives it its own +0x5c. A named copy of *arg_2 for the magnitude 345 (same code, still shared);
// magnitude helper zero assigned in place 421 (1854 bytes); planar helper zero assigned in place, zero
// arm first 357 (1863 bytes); both 427; planar in place with the scale arm first 354 (1863 bytes,
// LCS-aligned 229 -> 201: retail's single planar homes and branch order; the lead); with the in-place
// magnitude too 426 (retail's magnitude branch and single home; definition/magnitude homes 0x10/0x14
// swapped and the sum i-first where retail is k-first). Restored.
// Round 304: function-scope planar storage subsets 1-7, then all three at settings scope in reverse
// declaration order: 345 each. Every initializer stayed at its original read; original body restored.
// DH round 279: Initial working/output/query vector producers after the matched position provider, seven
// subsets then native in-place planar result [344, 421, 423, 348, 345, 409, 411, 419]. Original executable
// body restored.
// DH281: Complete magnitude input-component declaration orders ijk/ikj/jik/jki/kij/kji, then K/J/I input
// storage with planar single-root and both single-root results [343, 343, 345, 345, 345, 345, 354, 426].
// Original executable body restored.
// P260: Native magnitude square-product declaration orders ijk/ikj/jik/jki/kij/kji
// on complete magnitude/zero-default planar joins: 343/343/345/345/345/345; K/J/I products
// with named sum/absolute length 345/345. Two-point gains rejected;
// original helpers/body restored, frame 0x58/0x64 and magnitude/query overlap remain.
// DH283: Joint magnitude-input/query-output/working-vector lexical priorities abc/acb/bac/bca/cab/cba 345 each;
// independent vector producer order with joint identities 343, with entry query-output declarations 343.
// Two-point gains rejected; original executable body and private helpers restored.
// P265: Entry-owned magnitude result/speed-limit/turn-floor scalar homes, seven nonempty
// subsets then joint semantic scalar names, 345 each. All initializers stayed at their original
// producers; original declarations and body restored. Frame/query-magnitude overlap remains.
// Round 287: Existing-helper native/coordinate input ownership: magnitude only, paired
// native/coordinate planar inputs, then immutable paired inputs, 345/456/492/566/345/456/492/456. Original helpers
// and body restored; frame 0x58/0x64 and magnitude/query overlap remain.
// M3 lane 4 r155 (fast; callee CAVEATs, base reproduced 345): magnitude helper by pointer on a named copy 343 (frame still 0x58,
// the query output still shares the copy's slot), that copy declared before the position call 402 (frame 0x5c), by pointer on
// the input directly 456 (1880 bytes), zeroed in place by pointer 421 (1854 bytes); the planar helper zeroed in place (scale arm
// first) with the pointer copy 352; the magnitude helper's by-value input `vector3f const` 345 (same code); the query outputs
// declared before the magnitude 345 (same code: homes follow lifetimes). Re-measured: the query output vector at function scope
// 345; the three working vectors as struct copies 442 (1887 bytes); the planar helper zeroed in place alone 354.
// DH297: Twice-used magnitude before independent position/query/initial-vector
// producers at four native boundaries, then each with entry-owned query outputs:
// 404/406/459/403/404/406/459/403. Original producer order, query homes and full body restored.
// AC491: Forward-correction scalar result before fields/aggregate/setter vector publication,
// default/complete arms; entry scalar with fields/aggregate: 352/354/352/354/352/354/352/352.
// Original complete body and producers restored; native frame and homes remain.
// M3 lane 4 r157 (fast; positional / LCS-aligned, base 345 / 230): LEAD: retail keeps a full copy of the working
// vector made before 0x29b2e0 (i in ESI, j/k at +0x60/+0x64, sharing the query output's +0x5c home) and builds the
// planar input from it after the call; `vector3f previous = local_4;` before the call with the planar input
// from `previous` gives retail's frame 0x64 (first difference +0x2 -> +0x42) 386 / 216 (1915 bytes), the 0x668 store
// after the planar copy 386; with the planar helper zeroed in place (scale arm first) 385 / 182 (1910 bytes); plus
// magnitude terms ijk/kji/ikj 383/385/383 (aligned 175/182/179); plus the magnitude helper zeroed in place
// 435 / 177 (1898 bytes; its block is retail's but for the 0x10/0x14 homes of the definition pointer and the
// magnitude); on that, renaming the magnitude, the definition pointer or the query vector 435 each (same code).
// The rest is the entry copies: retail struct-copies all three working vectors (dword moves, the +0x290 one through
// `mov edx, ebx`); as struct copies here: all 479 (frame 0x68: arg_0 moves to ECX and arg_2 to EAX, so the query
// bools lose arg_2's dead stack home), the +0x290 one only 484, the second constant one only 450; earlier forms
// struct copies 513, +0x290 copied from the field 450, non-const +0x290 pointer 438. Also on the 435 form: the
// history read through a pointer to its field (retail reads +0x678..0x680 in place) 413 / 175 (1878 bytes, 449
// instructions; alone on the original 361 / 230, 1845 bytes); the correction vector written into the output vector
// (retail's +0x38 home) 413 (same code); with the entry struct copies all/first/constants 430/460/423 (the struct copy of
// +0x290 always moves the pointer out of EBX into ebp+0x290 addressing and passes arg_2 in EAX, ret 0x1c), with the
// pointer declared at entry 442 or in the settings block 460, through a two-pointer copy helper 460. Restored.
// DH311: Existing pointer-input scale as plain coordinate-array loop, long/dword/short/byte
// with streaming stores 345/345/435/435; complete native output before publication 464/464/515/515.
// Original helpers/body restored; frame 0x58/0x64 and magnitude/query overlap remain.
// m3 lane 11 r62 (fast): the turn and squared-length floors as `x = x > c ? x : c` 345 (same code); the planar
// dot product written j-first with operands swapped 345 (same code); the magnitude declared bare at entry 345
// (same code); computed as the first statement inside `if (local_2)` 353; named requested_magnitude 345 (same
// code). Retail gives the planar length, speed limit and floor the dead homes of arg_8/arg_7/arg_2 and the
// magnitude a frame slot; ours gives the magnitude arg_8's home and shifts the rest by one.
// Round 122 (lane 7, fast; base 345 [aligned 276]; retail keeps the planar copy's pre-call values in ESI and
// the query slot, then stores them into the magnitude copy's slot after 0x29b2e0): the query outputs in their
// own block 345 (same code); the planar vector declared at the top of the settings block 345 (same code);
// planar components captured in named reals before 0x29b2e0 (diagnostic) 380 (1906 bytes, frame 0x5c);
// all three working vectors as whole-struct copies 442 [406] (1887 bytes, frame 0x60); the working vector
// alone as a whole-struct copy 449 [421] (1884 bytes, frame 0x60).
// Y556: Zero-initialized discarded query vector, aggregate/library-clear/shared setter/
// chained fields at query/entry scope: 345/345/345/345/345/345/345/345. NOT kept: added local initialization
// fails byte agreement; original uninitialized output and complete body restored.
// m3 lane 11 r62 (fast): the turn and squared-length floors as `x = x > c ? x : c` 345 (same code); the planar
// dot product written j-first with operands swapped 345 (same code); the magnitude declared bare at entry 345
// (same code); computed as the first statement inside `if (local_2)` 353; named requested_magnitude 345 (same
// code). Retail gives the planar length, speed limit and floor the dead homes of arg_8/arg_7/arg_2 and the
// magnitude a frame slot; ours gives the magnitude arg_8's home and shifts the rest by one.
// M3 lane 9 r83 (fast): the private v*s scale copy replaced by the existing vector3f_scale 345 (identical code, kept);
// the magnitude helper zeroing its own root in place 421 (1854 bytes); the planar steps through normalize2d_fast,
// re-measured, 369 (1868); the blend rows with the weight complement written first 345 (same code); the weight
// field read at each use (retail reads +0x64 twice) 345 (1865 bytes), plus the history read in place through a
// pointer 361 (1845). Retail's blend computes the k row first and reads the history fields in place.
// M3 lane 6 r142 (fast): local_5 by components and local_6 as the struct copy 493 (1913 bytes); local_6 copied from
// local_5 366; that with local_4 as a struct copy of the +0x290 vector 442 (1887 bytes); local_6 declared before
// local_5 412 (1864 bytes); the input magnitude taken inside `if (local_2)` 353. Retail copies local_4 whole (k
// included) into +0x5c..+0x64 before 0x29b2e0 and builds local_20 from that copy after the call. Original body restored.
// Lane 14 r60 (fast; "no-home" = aligned count ignoring stack offsets, base 174): retail holds &+0x290 in EBX and arg_2 in ESI and
// copies the three working vectors whole through integer registers. All three whole copies 442 (1887 bytes, frame 0x60, no-home
// 214: the pointer is copy-propagated into [ebp+0x290] uses, arg_2 moves to EBX and arg_0 to ECX), with a non-const pointer 442
// (same code); local_6 alone whole 366 (frame 0x58, no-home 231); local_4 alone whole 449 (frame 0x60, no-home 272), with the
// non-const pointer 449 (same code); the query output vector at function scope 345 (same code), with all three whole 442, with
// local_6 alone whole 366.
// P305: Native library working/output/secondary entry transfers, seven subsets
// 449/345/366/449/442/366/442; all with the later requested-vector transfer 442.
// Original whole body and helpers restored; magnitude/query overlap and native frame remain.
// M359: Query outcome after escaped output production, positive/complementary
// bool/byte/short/long: 345/345/345/345/345/345/345/345. NOT kept: one-use call
// result is unmatched; original query guard, output lifetimes and complete body restored.
// DH324: Full pre-update saved direction owns the later in-place planar workspace:
// native/aggregate/setter/library copies, original then single-root planar result
// 363/332/366/363/325/366/332/325. NOT kept: 325 at 1874/1863 bytes moves length away from the starting 1866;
// native XY transfer/homes remain. Original helpers and complete body restored.
// DH326: Consistent native-pointer/direct history reads 361/360; history pointer with
// full pre-update native/coordinate/setter/library direction and separate later XY 362/356/356/362;
// native/coordinate saves with shared planar normalization 385/380. Full saves recover frame 0x64
// and first +0x42, but grow to 1895/1897 versus 1863 bytes. Original full body/helpers restored.
// DH328: Existing by-value magnitude over its native component table in K/J/I order,
// long/short ordinals and direct/named squares 442/442/513/513; input-updated root
// 434/434/502/502. NOT kept: regressions and longer code; full helpers/body restored.
// DH333: Typed coordinate-range copies at working/constant/history producers, seven subsets
// 438/376/459/378/498/402/501; complete backward ranges 501. History-only recovers frame 0x64
// and first +0x42 but grows to 1909/1863 bytes. NOT kept: regressions; original full body restored.
// DH355: Plain indexed working/constant/history coordinate-copy subsets and all
// with full pre-update direction: 345/502/355/502/355/490/490/550. NOT kept: ties/regressions and
// nonnative extents; original vectors, observations, helpers and complete body restored.
// Round 52 (m3 lane 2, fast; no-stack = aligned count ignoring stack offsets; base 345, no-stack about 225): CALL FIX: retail's 0x29b2e0 reads
// its first vector from EBX and retail's EBX at the call is the +0x290 pointer, so retail passes (arg_0, arg_1, local_3, &local_4, speed,
// &local_4); ours passed &local_4 twice, which is why the pointer left EBX. Fixed call 351 (1852 bytes); with whole-struct entry copies (now
// retail's entry code) 399 (1841, no-stack 163); in-place zero results in both magnitude helpers 361 (1824, 148); magnitude sum written
// i, j, k 355 (146); default-first floors and in-place clamp 350 (131); the tail written from retail's listing (saved direction copy before the
// call, scale read before the planar step, output clamps in place) 280 (1873, frame 0x64); fresh speed local and separate correction vector
// 278 (142); current+correction sums and a named complement 267 (1877, 134), which reads the +0x50 scale into a one-use local before the planar step as
// retail does (match-only; the same body reading the field at its use 306, 1863 bytes, no-stack 145, kept: local_2 then loses its +0x10 slot). On the 267 form: the saved direction by fields 311, as two reals 335
// (frame 0x5c), through a pointer 266 (one-use pointer, not kept); ternary zero floor 307, ternary squared floor 267, ternary turn floor 267
// (no-stack 171); planar vectors declared in other orders 267 (same code); flagged settings bodies in a compiled-out loop 345 each on the old
// body; whole-struct copies alone (old call) 442. Diagnostic: identity pointer locals before the planar step k=1..7
// 267/267/266/266/267/267/267 (k=2-4 flip the dot's operand order to retail's). What is left: the saved direction's i stays in ESI across the
// call and the planar copy after it uses integer moves in retail (ours reads the saved copy directly with float moves, so local_2 takes ESI);
// planar #2 reads x first where #1/#3 read y first and #1/#3 multiply y load-first (operand-state flips with symbol numbering); xmm numbers of
// the floors (zero web stays separate from the floor value).
// Same round, on the kept 306 form (fast): the file's clamp helper at the four clamps or if/else clamps 306 (same code; helper kept); names for the
// vectors, definition and magnitude 306 (same code: names do not order these slots). Match-only leads: the planar copy after the call as a whole
// `vector2f` read of the saved direction (`*(vector2f const *)&saved`) gives retail's ESI for its i, 305 (no-stack 113); that with the one-use scale local
// 302 (1869 bytes, no-stack 103, retail's local_2 slot); also storing the correction in the output vector 303 (retail's shared +0x38 home for its i, but
// the vector homes stay swapped); saved direction as a planar vector copied before the call 351 (1841, frame 0x60); planar copy before the call with no
// saved vector 370 (1810, frame 0x58). Left on the 302 form: the working and output vector homes swapped (+0x38/+0x44), planar #2's x-first order and #1's load-first y,
// floor registers, the zero web apart from the turn floor, and row order of the blend.
// DH360: Explicit epsilon-test truth in existing magnitude/planar helpers, bool/byte/short/long,
// eight at 345. NOT kept: unmatched one-use predicates do not recover the 0x64 frame or separate
// magnitude/query homes; original helpers, all native copies and complete body restored.
// DH362: Magnitude output-only/numeric-plus-output, pointer/reference, common/complete-arm
// publication on the retained native-input/result-join graph: eight at 345. NOT kept: frame
// 0x58 and query/magnitude overlap remain; original helper contract and complete body restored.
// DH364: Native magnitude/planar/movement coordinate-union consumer subsets
// 1/2/4/3/5/6/7, then float root/absolute APIs on complete result joins: eight at 345.
// Original copies, helpers, expression trees and full body restored; frame/query
// overlap remains.
// DH367: Overlap-safe CRT working/secondary/history transfers, seven subsets,
// then paired final public transfers: 447/363/376/440/501/388/473/480. History-only and secondary/history
// recover frame 0x64 and first +0x42 but grow to 1907/1894 versus 1863. NOT kept: all regress;
// original copies, helpers and complete body restored; frame/query-magnitude homes remain.
// Round 59 (m3 lane 15, fast; the convention agrees: arg_0 EAX, ret 0x20; first difference +0x54 is the definition pointer at frame+4 and
// the magnitude at frame+0 where retail has them the other way round): the magnitude helper's local named `magnitude` 306 and the definition
// pointer named `settings` 306 (names do not order these slots); `local_15` as `*(vector2f const *)local_3` 306, both planar locals that way
// 306; the turn dot through the existing dot_product2d 306 (all the same code). Retail sums the first planar length j, i and the second i, j,
// and multiplies the turn dot local_13-first; ours puts local_15 first in both products.
// AC551: Immutable saved direction; XY memcpy/memmove/field producers; immutable saved direction
// with each library transfer; pre-call destination declaration; transfer after actor publication:
// 306/305/344/306/305/344/305/305. NOT kept: no qualifying supported improvement; complete
// executable source restored. Saved-direction and planar homes, operand ranks and floor registers
// remain. The 305 XY memcpy form gains only one; the memmove forms miss native frame/length.
// DL268: Saved-direction assignment/memcpy/memmove/existing setter on the native-frame graph
// 306/306/419/308; complete vector-transfer memcpy/memmove, complete input memcpy and complete
// setter publication 306/445/306/430. NOT kept: ties/regressions or nonnative extent/frame;
// original copies, saved vector, helpers and entire body restored. Settings/magnitude and saved-direction homes remain.
// DL270: Typed two-element range copies at saved/requested/facing planar producers,
// seven subsets then complete backward ranges: 346/327/366/325/371/326/420/420.
// NOT kept: all regress or move frame/length away; native copies and full body restored.
// M396: Immutable original magnitude/turn-factor/blend-factor producer subsets
// 1/2/4/3/5/6/7, then immutable settings receiver: eight at 306. NOT kept: no
// settings/magnitude or saved-direction/planar home repair; original body restored.
// M402: Ordinary private magnitude/planar/clamp declarations, seven subsets,
// then complete ordinary graph with planar inline metadata: eight at 306. NOT kept:
// no settings/magnitude or saved-direction/planar home repair; original helpers/body restored.
// Round 178 (m3 lane 4, fast then plain; aligned = difflib count with addresses masked, 191 at 320): the history
// weight read from the settings field at each of its three uses instead of a local 286 (kept, aligned 184; retail
// still loads it once into xmm6 and re-reads it for 1 - weight). Retail's first planar normalize keeps the scaling
// path as the fall-through and the zero path after it (the 0x2b400 shape, which is noinline here). Also tried: the
// 0.05 floor as a MAX ternary 309 (on 286: 275, but aligned 196: retail's code is the in-place if); the 0x47c
// override read into local_18 and tested `== 0` 318 (on 286: 286, aligned 203); the minimum-step test as
// `local_17 >= local_18` 271 at 1863 bytes (on 286: 298; retail's own compare is `comiss local_18, local_17; ja`,
// the existing form, so the gain is a shift); the speed clamp as a ternary 320 (same code); on 286 the history
// pointer read at each use 286 (aligned 183), the velocity pointer local dropped 286 (aligned 185).
// Round 180 (m3 lane 4, fast; aligned/masked = difflib counts with addresses / also stack offsets masked, 184/107
// at 286): field_50 written as the first factor of the 0x1f9e70 product 286 (same code; retail reads +0x50 before
// the call into a stack temp); local_20 as the planar copy `*(vector2f const *)&local_27` 310 at 1864 bytes, but
// aligned 170 and masked 90 (LEAD: the copy of local_27 and local_20's home are then retail's, local_27.i stays in
// ESI); local_13 / local_15 as planar copies 286/286 (same code); all three 310. Left: site 1's normalize keeps the
// zero path as the fall-through and a separate result home (retail stores 0 into the magnitude's own home after
// the scaling path), the 0x1c/0x20 entry homes and the xmm numbering.
// Round 183 (m3 lane 4, fast; aligned/register-masked = difflib counts with addresses / also registers masked,
// 184/131 at 286): site 1's planar normalize is retail's inlined 0x2b400 shape (scaling path falls through, 0 stored
// into the magnitude's own arg_8 home); 0x2b400 is noinline on main in an avoid-listed file and normalize2d_fast has
// the other branch order, so that block stays (BLOCKED-BY 0x2b400). Tried: the blend rows k, i, j (retail computes
// the k row first) 286 (aligned 188); the planar dot as the existing dot_product2d 286 (same code); re-measured the
// one-use +0x50 scale local before the 0x1f9e70 call 334 (1889 bytes, aligned 174, register-masked 122); that with
// local_20 as `*(vector2f const *)&local_27` 294 (1882 bytes, aligned 161, register-masked 113; match-only, the
// closest shape so far). No other untried natural form is left for the slot homes (names, declaration orders and
// whole copies are all listed above).
// MiniMax C (11-14): no new form, no lower score (286); the stack homes still sit 8 and 4 bytes above
// retail's (ours at +0x24 against retail's +0x1c, ours at +0x14 against retail's +0x10).
// MiniMax G (11-20): no new form, no lower score (286): the magnitude and the query output still take the frame's earlier slots where retail puts the magnitude at +0x10 and the query output at +0x1c, and the group of working vectors is still stored in a different order; the two scaling sites are 0x2b400's shape, which is not matched yet.
// MiniMax I (11-14): no new form, no lower score (286): the magnitude and the query output still take the frame's
// earlier slots where retail puts the magnitude at +0x10 and the query output at +0x1c, and the group of working
// vectors is still stored in a different order than retail's.
// m3 lane 6 r181 (fast with 0x29b2e0/0x2922b0/0x1f9760/0x1f9e70/0x30bf0/0x1e5450/0x26c180/0xb9dd0/0x1f5f30; control 286, aligned/
// stack-masked/register-masked 234/146/68): the shared normalize2d_fast (include/unknown_11cc90.h) written scaling-path first
// (`if (!(fabs(m) < eps)) { scale; return m; } return 0;`, the 0x2b400 shape) 286 (masked 134/54: site 1's branch order is then
// retail's, its zero result still in its own home instead of the magnitude's), but 0x29b2e0 176 -> 203; header restored. Site 1
// through the existing normalize2d 407 (1786 B: a call, its noinline declaration keeps 0x2c2a70's call). The history weight in a
// local for the three rows with the complement still read from the field (retail loads it once into xmm6 and re-reads it for
// 1 - weight) 321 (1867 B); the current sum through the existing vector3f_add 316 (1863 B, aligned 255).
// Branch-owned direction/query adjustment/weighted motion, seven nonempty subsets, then all with aggregate clamping: 388/519/411/466/401/503/566/566.
// NOT kept: every form regressed and moved the frame away from 0x64.
// Original executable body restored.
// Object record reference 286. Native tag result viewed as settings pointer or record reference
// in its branch 286/286. With the object reference 286/286. Combined forms with named object,
// settings and magnitude, then named working/output/adjustment roles 286/286/286.
// NOT kept: object and settings spills retain their original stack homes.
// Object/settings spills and the initial vector homes remain. Separate vector declarations in all six
// orders with original assignments, call-scoped query outputs, and scoped outputs with the
// published vector declared first: eight at 286.
// M3 lane 6 r191 (fast with 0x29b2e0/0x1f9760/0x1e5450/0x30bf0/0xb9dd0; control 286, of which 37 flagged lines differ only in a
// stack offset): what orders the 12-byte homes is the order of first assignment, not the declarations or the names. The
// output vector (the first copy of the global) initialised ahead of the working vector puts both in retail's homes (+0x38 and
// +0x44) but emits its copy first, 290; the second copy of the global initialised ahead of the first 286 (its home does not
// move); the output vector declared bare ahead of the working vector 286, and at the top of the function 286 (same code);
// the four vectors renamed in ascending and in descending alphabetical order of retail's homes 286/286 (same code).
// Diagnostics: two more whole reads of the output vector at the end 301, three more settings-field reads 349 (1898 bytes):
// neither moves a home, so reference counts do not order them either. Retail therefore refers to the output vector before
// the working vector's copy without storing to it there; no natural form found. Site 1's planar normalize is still the
// inlined 0x2b400 (`x = x * inv; y = inv * y`, the zero written into the magnitude's own home, scaling path first), and
// 0x2b400 is pinned out of line in machine 1's src/unknown_02b400.cpp: BLOCKED-BY 0x2b400.
// M3 lane 9 r138 (fast; base 286, aligned 234, register-masked 73): 1-7 dead initialised locals (diagnostic) 286
// each, masked 71/71/73/75/73/73/75 (no frame home moves); the forward correction written into the output vector
// instead of its own local 286 (aligned 235, homes unmoved), into the working vector 329 (1875 bytes), into the
// output vector with the sum formed there too 286, into the secondary vector 286. Slot map (pending pushes taken
// off): retail settings +0x10, magnitude +0x14, object +0x18, second planar vector +0x1c/+0x20, first +0x24/+0x28;
// ours magnitude +0x10, settings +0x14, a planar pair +0x18/+0x1c, object +0x20, a planar pair +0x24/+0x28: the three
// scalar homes and the two planar pairs trade places together, and retail's first planar vector is normalised in
// place in its home (the 0x2b400 shape). Still BLOCKED-BY 0x2b400.
// Round 145 (m3 lane 16, fast, control 286): site 1 is 0x2b400's shape in retail (normalizing path first, the magnitude
// kept in the dead arg_8 home [esp+0x94], the zero stored there out of line at 0x29ae10), which this build cannot inline
// (0x2b400 is declared noinline for 0x2c2a70's call): BLOCKED-BY 0x2b400. Homes: retail's 12-byte order from ESP is the
// first global copy, the +0x290 copy, the second global copy, local_12, with local_15/local_13 at 0x1c/0x24; ours puts the
// +0x290 copy first and local_12 below the second global copy. local_4/local_5/local_6/local_7 each renamed to sort first
// 286 each (same code); local_12 declared at function scope 286; local_7 declared first 286; local_15 initialised before
// local_13 289 (1869 bytes).
// M3 lane 6 r201 (fast with 0x29b2e0, 0x1f9760, 0x1e5450, 0x30bf0, 0xb9dd0; control 286; aligned-equal of retail's 454
// with addresses / also registers and stack offsets masked, base 270 / 413; all restored). New finding: retail does not
// fold the gain's constants. Its last product is `(1 - floor) * 0.5`, times the gain, times 4.0 (mulss by the 0.5
// register, by the gain, by the 4.0 literal at 0x45dc10); main's one expression `(1.0f - local_26) * 0.5f * local_20 *
// 4.0f` compiles to two multiplies with the 2.0 literal. Any split restores retail's three: a named half-complement
// local 326 (1869 B, 272 / 415); `local_18 = (1.0f - local_26) * 0.5f; local_18 = local_18 * local_20 * 4.0f;` 326
// (same code); `local_18 = (1.0f - local_26) * 0.5f * local_20; local_18 *= 4.0f;` 326 (259 / 415); the gain factor
// grouped as `(local_20 * 4.0f)` 326 (259 / 414). None is kept: the body is then six bytes longer than retail's and
// every later line shifts (the count rises by 40). Diagnostic (a file-local copy of 0x2b400's body for site 1, not
// keepable): 286 (270 / 420: site 1's branch order and in-place scaling are then retail's, the zero still has its own
// home), with the split product 326 (272 / 422). Still BLOCKED-BY 0x2b400 for site 1; the frame homes and the turn
// dot's operand order remain as the earlier notes describe.
// M3 lane 14 r122 (fast with 0x29b2e0, 0x1f9760, 0x1e5450, 0x30bf0, 0xb9dd0; control 286): the output vector declared
// bare, bound to a `vector3f &` and assigned through it after the working vector's copy 286; the output vector copied
// from the second global copy 286; both global copies by one chained assignment 286; the forward correction as an
// aggregate initializer 286; the second global copy initialised from the output vector 286 (same code each: no home moves).
// M3 lane 5 r209 (fast with 0x29b2e0 at 176, 0x1f9760 at 1, 0x1e5450/0x30bf0/0xb9dd0 MATCH; control 286, aligned 234,
// register-masked 191): the turn factor's half width named (`half_width`, squared into the numerator) instead of
// reusing local_20, with the gain as one expression 325 (1868 B), the gain then halved 326 (1871 B), with the named
// numerator kept 286 (aligned 233, masked 186: not kept, no gain); the blend rows with the history product written
// first 286 (same code); the complement computed ahead of the sums 316. Retail loads the history weight once into
// XMM6 ahead of the sums and re-reads the field for the complement; site 1 is still BLOCKED-BY 0x2b400.
// M3 lane 3 r275 (fast with 0x29b2e0/0x1f9760/0x1e5450/0x30bf0/0xb9dd0; control 286): the zero-length test after
// 0x2922b0 as `0.0f == function_30bf0(...)` 286 (same score); the override test as `0.0f != field` 288 (1869 B); site 1
// through the existing planar_normalize (include/vector2f_normalize.h) 286; the velocity dot through local_1->velocity
// at each use instead of its pointer local 286; the magnitude helper returning a ternary 393 (1877 B). Site 1 stays
// BLOCKED-BY 0x2b400 (its noinline declaration sits in another lane's file this round).
// M3 lane 3 r277 (fast with 0x29b2e0/0x1f9760/0x1e5450/0x30bf0/0xb9dd0; control 286): the 0x1f9760 out-parameters declared
// at function scope 286; local_6 copied from local_5 286; local_27, local_31 or local_22 declared at the top of the
// `if (local_2)` block 286/286/286 (same code each). Site 1 still needs 0x2b400's parked zero path inline (BLOCKED-BY 0x2b400).
// M3 lane 14 r128 (fast with 0x29b2e0, 0x1f9760, 0x1e5450, 0x30bf0, 0xb9dd0; control 286): the history weight named
// once (`real local_33 = local_2->field_64;`) for the three blend rows and the complement 320 (1866 B), the same
// weight read ahead of the sums 320; the sums written into the blend rows without local_31 288; the two planar
// copies as `*(vector2f const *)` reads 286 (same code), with the third planar copy too 310 (1864 B). Retail loads
// the weight once into XMM6 for the rows and re-reads it for the complement; ours folds it from memory in every
// row. Site 1 is still BLOCKED-BY 0x2b400.
// M3 lane 16 r157 (fast with 0x29b2e0, 0x1f9760 and 0x2922b0 unchanged; control 286 at 1865 B, aligned / register- and
// offset-masked 184 / 41; body unchanged). The history weight in a named local: declared ahead of the sums with the
// complement read from the field and the history product first 320 (1866 B, 191 / 43); the same with the complement taken
// from the local 320 (196 / 43); declared after the sums, the rows in the existing order 320 (184 / 43); ahead of the
// sums, the rows in the existing order 320 (191 / 43): retail's one load of the weight into XMM6 does not come from a
// named local. The 0x1f9e70 product's factor read into a named local ahead of the call (retail reads +0x50 into a stack
// temporary before the call and multiplies the result by it) 334 (1889 B, 174 / 41); declared ahead of the third planar
// normalize 339 (173 / 39); lane 4 r180's planar copy of the saved direction re-measured 310 (1864 B, 170 / 35); the
// two together 294 and 298 (1882 B, 161 / 35 and 161 / 31: between the two late calls only five masked lines then
// differ, the third normalize's in-place scaling among them). None kept: a one-use local ahead of a call, and the
// count rises. Sites 1 and 3 are both 0x2b400's shape in retail (scaling in place in the vector's own home), so the
// function stays BLOCKED-BY 0x2b400 (machine 1's file this round).
// M3 lane 15 r119 (H2M fast; control 286): the blend rows through the existing vector3f_scale and vector3f_add_scaled
// 325 (1870 B); local_19 as one ternary where its if stands 275 (1865 B), at its declaration 323 (1866 B). Not kept:
// retail loads the 0.05 into XMM5 ahead of the 0x47c test and tests `comiss xmm0, xmm5; movaps xmm5, xmm0`, which is
// main's declaration-plus-if form (ours colours local_19 XMM6 and local_24 XMM5, where retail gives local_24 XMM1), so
// the 275 only drops the early load and re-aligns later rows. The forward-gain clamp as `local_16 < local_30` 286; the
// sign test as `0.0f > local_25` 286; local_18 declared ahead of local_19 286.
// Typed actor reset and target, direction and request flag, and turn status, alone and combined, all
// at 286 each. Typed actor fields with scalar and vector views of the history buffer also scored 286.
// The object/settings spills still differ with the 0x64 frame in place.
// M3 lane 2 r127 (fast; control 286 at 1865 B, aligned / register-masked / offset-masked 234 / 191 / 73): LEAD for site 1:
// the matched 0xedff0's file-local planar normalize in src/unknown_0a76b0.cpp (one length local, scaling path falling
// through, `m = 0` in the else, scale-first products through a separate out pointer) compiles to retail's site-1 and
// site-3 code here (the zero stored into the length's own home, y reloaded after the x store). Screened through a copy of
// it at site 1 306 (1863 B, retail's length; first difference +0x42 -> +0x54; 185 / 134 / 62: the length shares its
// home, which frees one 4-byte slot, and the object pointer and both planar vectors take retail's homes); at sites 1
// and 3 306 (same code); plus lane 4 r180's planar copy of local_27 309 (1856 B, 155 / 102 / 41); plus local_2 named
// `settings` 309 (same code); plus the +0x50 scale read into a local ahead of the third planar step 302 (1868 B, first
// +0xbb, 138 / 89 / 36: the definition and magnitude homes are then retail's); plus the correction written into
// local_5 302 (141 / 89 / 36). Left on that form: local_4 and local_5 swap homes (retail's local_5 slot also holds the
// correction's i, from the x87 result), site 2's loads in another order, the floors' xmm numbers and the dot's operand
// order, the blend rows. Not kept: the count rises, and keeping the helper needs it moved from that file to a shared
// header (the existing 2-D helpers there are the zero-first shape). Patch in build/r127/lead-29ab90-302.patch.
// M3 lane 16 r164 (fast; control 286, aligned 184/107/41): ours gives the dead magnitude copy's slot (+0x2c) to the first
// planar vector (local_13), retail to the saved direction's planar copy (local_20), which moves the object pointer to +0x20
// and the two planar pairs one slot round. local_20 as the planar idiom `*(vector2f const *)&local_27` 310 (1864 B,
// aligned 170); the 0.05 floor declared at its first use 275 (aligned 196: it drops retail's early 0.05 load, a moved
// declaration); local_20, the 0x668 store and the 0x1f9e70 product in their own block 286 (same code); the forward
// correction stored in the output vector through a renaming reference 285 (1863 B, aligned 189), with the output vector
// initialised first 289; the correction written into local_5 directly 286, with the planar idiom 309. Site 1 is still
// BLOCKED-BY 0x2b400.
// M3 lane 2 r130 (plain checks: the reduced image builds 0x29b2e0 at 1309 B and this function at 343, so it misleads):
// local_2 and local_9 renamed into each other's names 286 and actor/object/properties/speed names 286 (same code
// each: these slots do not follow names); the sum through the existing vector3f_add 316 (retail's 1863 B); the forward
// correction as vector3f_scale of a `{ 1, 0, 0 }` constant 326 (1843 B); the saved direction's planar copy taken from
// local_4 before the 0x29b2e0 call with local_27 dropped 378 (1816 B). Site 1 still BLOCKED-BY 0x2b400 (machine 1's file).
// M3 lane 16 r170 (plain checks: h2m fast now builds this body at 343, 1871 B, with any callee set, 0x29b2e0 there at 1309 B
// against 1707 B in the full build; control 286): the third planar step through the existing planar_normalize 286; the
// first and third through it 286; local_1, local_2 and local_9 renamed object, settings, magnitude 286 (names do not move
// these homes); local_4 read straight from the actor field with local_3 declared after it 286; local_15 as the planar idiom
// `*(vector2f const *)local_3` 286 (all the same code). Still BLOCKED-BY 0x2b400.
// M3 lane 16 r173 (plain checks; control 286, aligned 234, register-masked 191): one redundant pair around the turn dot's
// first product, `(local_13.i * local_15.i) + ...`, 286 (both dot products' operand orders flip, and one history-blend
// product with them; aligned 238); `local_22.j = local_22.k = local_30 * 0.0f;` 308 (retail's 1863 B, aligned 240); one
// pair around the first blend row's first product 286 (aligned 233, masked 186); the speed floor as `local_18 =
// local_18 > local_17 ? local_18 : local_17;` 286 (same code); the dot and blend pairs together 286 (aligned 234, masked
// 188). Site 1 still BLOCKED-BY 0x2b400 (its noinline declaration sits in machine 1's file). Body unchanged.
// M3 lane 8 r232 (plain checks, control 286 at 1865 B, aligned 234, register/offset-masked 73; the reduced image still builds
// 0x29b2e0 at 1309 B and this function at 343, so frame layouts were read from a full link and scores from plain checks;
// body unchanged). Slot order, lowest first, with the reference counts that order them (4-byte cells first, then 8, then
// 12; most references lowest): ours has the magnitude (5), local_2 (4), local_17 with local_20 (2, then 8 bytes), local_1
// (2), local_15, local_13 with the by-value magnitude argument, local_4 (19), local_5 (15), local_27 with local_12 (9),
// local_6 (7), local_7 with local_22; retail has local_2, the magnitude, local_1, the two planar pairs, the argument copy,
// local_5, local_4, local_6, the local_12 cell, local_7. So retail counts local_2 at or above the magnitude, local_1 above
// local_17 (whose cell is not a 4-byte slot of its own there), local_5 at or above local_4 and local_6 above local_27.
// Forms: the magnitude helper returning `fabs(m) < 0.0001f ? 0.0f : m` puts local_2 lowest and the magnitude (4) above
// it, as retail, but the root then goes through arg_8's home and is reloaded, 393 (1877 B, masked 78); the helper with
// two returns 393 (the same code). Layout only (no count taken, the homes did not move): local_6 copied from local_5,
// `*arg_5` stored ahead of `*arg_3`, local_5 declared ahead of local_4. Site 1 still BLOCKED-BY 0x2b400.
// M3 lane 16 r185 (plain checks, about 100 s each; control 286 at 1865 B, aligned 234, register/offset-masked 73).
// Reading: retail holds the history weight (field_64) in a register for the three history products and reads it from
// memory again for `1.0f - weight`, with 1.0 in a register of its own; ours reads the field at each product. The head
// differs only in stack homes (the object and settings pointers and the three vector copies sit in other cells), and
// site 1's planar step has the zero path ahead of the scaling path where retail's falls into the scaling. Forms on the
// history blend: the weight in a named local read at all four uses 320 (1866 B, aligned 262, masked 75); the local
// read by the three products only, the complement reading the field (a mix, for the reading only) 321 (1867 B, aligned
// 236, masked 71); each product written weight first 286 (same code); each row's history product ahead of the sum
// product 286 (same code); the complement written out in each row with no local 286 (aligned 233, masked 71: same
// length, two rows move). Site 1 still BLOCKED-BY 0x2b400. Body unchanged.
// In round 641, the object/settings spills and working-vector order stayed at the opening 0x64 frame. Shared squared
// length in the magnitude 286; aggregate correction 286, aggregate direction sum 286, both 286; shared in-place sum
// in the correction 286; scale-first planar helper at the second site 286, second and third 286, first and second 286.
// The body was restored. Site 1 still has the other planar normalization path.
// M3 lane 2 r148 (screens with 0x29b2e0, 0x2922b0, 0x1f9760, 0x1f9e70, 0x1f5f30, 0x30bf0, 0x1e5450, 0x26c180, 0xb9dd0, 0x2b400,
// 0x11ce20, 0x1caa40, 0x26be90, 0x26bfa0, 0xb8d30, 0xb9ef0, 0x170d70 and 0x11cc90 in the image: with the last two listed the
// reduced image builds 0x29b2e0 at 1707 B and this body at 286, 1865 B, first difference +0x42, as the full build does;
// aligned 184, 41 with registers and offsets masked; body unchanged). Two readings of retail not in the notes above. (1) The
// correction vector is built in the output vector: its first component is stored to the output's own cell after the speed
// clamp, the x87 square-root term is added to that cell in place, and the two zero products stay in registers. (2) The speed
// scale field is read ahead of the third planar step and held across the 0x1f9e70 call. Retail's cells, counted by
// reference: settings 6, magnitude 4, object 2; the two planar vectors 13 each; then a 12-byte cell shared by the entry
// magnitude's copy and the third planar vector; output 19, working direction 18, constant copy 7, saved direction 4,
// position 3; the 0x2922b0 minimum sits in the second argument's home and its limit in the eighth's. Forms: the correction
// written into local_5's members with no local_22 286 (aligned 185: its first component is still read from local_30's
// home); with the difference taken from that member 286 (same code); the scale field written ahead of the call in the
// product 286 (same code as now); the first component as `local_30 * 1.0f` 286 (same code); the correction as
// vector3f_scale of a constant `{ 1, 0, 0 }` into local_22 326 (1843 B, re-measured), into local_5 with the sums and the
// x87 term on local_5 318 (1843 B, aligned 159 [45]: local_4 and local_5 take retail's cells and the store, `fadd` and
// `fstp` rows on the output's first cell are retail's), on that the sums in place 318 (same code), the sum's operands
// exchanged 313 (1845 B, aligned 155 [39]), the sum through vector3f_add 331, the weight named for the three history
// products 318 (1844 B). Diagnostic, not keepable as written (a same-file copy of the matched planar normalize of
// src/unknown_0a76b0.cpp, which keeps its length in one local): site 1 through it 306 (1863 B, first difference +0x54,
// aligned 156 [36], re-measured); with the third site through it, the third planar vector as the planar idiom on local_27
// and the scale field read into a local ahead of the third step 302 (1868 B, +0xbb, aligned 117 [21]); those with the
// constant-axis correction into local_5 308 (1846 B, +0x15b, aligned 90 [25]: every stack cell of the function is then
// retail's); with the sum's operands exchanged 307 (aligned 85 [20]); with the weight named after the complement 306
// (1849 B, aligned 85 [18]; LEAD, build/r148/lead-0x29ab90-306.patch). Without the early scale read the same forms give
// 326 (1834 B, +0x54) and without the constant-axis correction 309 (1856 B), so no subset of keepable parts lowers the
// count. On the lead: one, two or three redundant pairs in the turn dot 306 or 307 (aligned 84 or 85), the dot's products
// exchanged 306, the first two planar vectors as the planar idiom 306 (same code each), the history term through
// vector3f_scale 299 (1850 B, aligned 105). Left on the lead (14 bytes short): site 1's second scale row (retail loads the
// component and multiplies by the scale, as the lead's third site already does), the turn dot's operand order, the
// settings pointer reloaded after the 0x1f9e70 call (the lead keeps it in ESI), and the blend, where retail holds the
// weight in XMM6 for the three history products and reads the field again for the complement. Site 1 still BLOCKED-BY
// 0x2b400.
// M3 lane 5 r246 (H2M_FAST with lane 2 r148's 18-callee set: 286 at 1865 B as the full build). Frame packing (a diagnostic
// print of the back end's slot assignment: size ascending, then references, ties to the first to reach the count): our
// 4-byte cells are the magnitude helper's root (5 references) and the settings pointer (5, tied, the root first), then
// local_17 with local_20 in a new 8-byte cell and the object view; retail's order (settings 6, magnitude 4, object 2,
// local_17 in an argument home) needs one more settings reference and one fewer root reference. A forced-ranking search
// (14 single decisions) found none below 286. Site 1 is retail's inlined 0x2b400 body, but retail calls 0x2b400 out of
// line from 37 other functions (many matched), so its existing noinline declaration stays. Forms: site 1, sites 1-2 and
// all three planar steps through the existing planar_normalize (include/vector2f_normalize.h) 286 each (same code);
// local_17 declared ahead of local_16 286, at function scope 286. Body unchanged.
// @retail 0x29ab90
void __stdcall function_29ab90(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	s_29ab90 const *local_2 = (s_29ab90 const *)function_1e5450(arg_0, local_1->tag_index);
	*(bool *)(local_0 + 0x6d1) = false;
	if (!arg_8)
	{
		*(real *)(local_0 + 0x658) = *(real *)(local_0 + 0x65c);
		*(real *)(local_0 + 0x660) = *(real *)(local_0 + 0x664);
		*(vector3f *)(local_0 + 0x678) = *(vector3f *)(local_0 + 0x66c);
	}
	if (*(long *)(local_0 + 0x26c) == NONE)
		*(long *)(local_0 + 0x7fc) = function_1f5f30(arg_0);
	vector3f const *local_3 = (vector3f const *)(local_0 + 0x290);
	vector3f local_4 = *local_3;
	vector3f local_5 = *g_4687a4;
	vector3f local_6 = *g_4687a4;
	point3f local_7;
	function_b9dd0(arg_1, &local_7);
	real local_9 = function_29ab91(*arg_2);
	if (local_2)
	{
		if (!*(bool *)(local_0 + 0x5d0) || *arg_7)
			vector3f_scale(local_3, 2.0f, arg_2);
		bool local_10;
		bool local_11;
		vector3f local_12;
		if (function_1f9760(arg_0, arg_1, &local_7, local_9, &local_10, &local_11, &local_12))
		{
			*arg_7 = true;
			vector3f_scale(local_3, 2.0f, arg_2);
		}
		function_26c180(arg_0);
		vector2f local_13 = { arg_2->i, arg_2->j };
		real local_14 = normalize2d_fast(&local_13);
		vector2f local_15 = { local_3->i, local_3->j };
		normalize2d_fast(&local_15);
		local_4 = *arg_2;
		real local_16 = 1.0f;
		real local_17;
		function_2922b0(arg_0, arg_1, &local_4, &local_6, &local_16, &local_17, true);
		if (function_30bf0(&local_4) == 0.0f) local_4 = *local_3;
		real local_19 = 0.05f;
		real local_18;
		if (*(real *)(local_0 + 0x47c) != 0.0f)
			local_18 = *(real *)(local_0 + 0x47c);
		else
		{
			real local_24 = 0.001f;
			real local_25 = local_14 * local_14;
			if (local_25 > local_24) local_24 = local_25;
			real local_20 = local_2->field_1c * 0.5f;
			real numerator = local_20 * local_20;
			local_20 = numerator / local_24;
			real local_21 = local_13.i * local_15.i + local_13.j * local_15.j;
			local_20 = local_20 * 0.5f;
			real local_26 = 0.0f;
			if (local_21 > local_26) local_26 = local_21;
			local_18 = (1.0f - local_26) * 0.5f * local_20 * 4.0f;
		}
		if (local_2->field_4c > local_19) local_19 = local_2->field_4c;
		local_18 = PIN(local_18, local_19, 1.0f);
		if (!(local_18 > local_17)) local_18 = local_17;
		vector3f local_27 = local_4;
		function_29b2e0(arg_0, arg_1, local_3, &local_4, local_18, &local_4);
		vector2f local_20 = { local_27.i, local_27.j };
		*(real *)(local_0 + 0x668) = local_18;
		normalize2d_fast(&local_20);
		real local_30 = function_1f9e70(arg_0, arg_1, (point2f const *)&local_7,
			&local_13, &local_20, &local_15, NULL, local_9, false) * local_2->field_50;
		*arg_6 = false;
		if (local_30 > local_16) local_30 = local_16;
		vector3f local_22;
		local_22.i = local_30;
		local_22.j = local_30 * 0.0f;
		local_22.k = local_30 * 0.0f;
		if (local_2->field_68 > 0.0f)
		{
			vector3f const *local_23 = &local_1->velocity;
			real local_24 = local_23->k * local_3->k + local_23->j * local_3->j + local_23->i * local_3->i;
			real local_25 = local_30 - local_24 / local_2->field_68;
			long local_26 = local_25 < 0.0f ? -1 : 1;
			local_22.i = (real)(sqrt(fabs((double)local_25)) * local_2->field_6c * local_26 + local_22.i);
		}
		vector3f const *history = (vector3f const *)(local_0 + 0x678);
		vector3f local_31;
		local_31.i = local_6.i + local_22.i;
		local_31.j = local_6.j + local_22.j;
		local_31.k = local_6.k + local_22.k;
		real local_32 = 1.0f - local_2->field_64;
		local_5.i = local_31.i * local_32 + history->i * local_2->field_64;
		local_5.j = local_31.j * local_32 + history->j * local_2->field_64;
		local_5.k = local_31.k * local_32 + history->k * local_2->field_64;
		local_5.i = PIN(local_5.i, -1.0f, 1.0f);
		local_5.j = PIN(local_5.j, -1.0f, 1.0f);
		local_5.k = PIN(local_5.k, -1.0f, 1.0f);
		*(short *)(local_0 + 0x656) = NONE;
	}
	*(vector3f *)(local_0 + 0x66c) = local_5;
	*arg_4 = 0;
	*arg_3 = local_4;
	*arg_5 = local_5;
}
