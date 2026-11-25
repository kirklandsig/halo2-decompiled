#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_0259d0.h"
#include <new>
// @flags /O2 /arch:SSE /Gr

struct s_shape_state;
struct s_state_c570;
struct s_biped_physics_move;
struct s_biped_physics_output;
class c_z_shape_view
{
public:
 virtual void v0() = 0;
 virtual void v1() = 0;
 virtual void v2() = 0;
 virtual void v3() = 0;
 virtual void v4() = 0;
 virtual long v5() = 0;
};
#include "unknown_0a9f20.h"
struct s_shape_carrier_contact;
struct s_motion_adjustment_result;
struct s_motion_adjustment;
struct s_ease_input;
struct s_ease_output;
struct s_ease_state;
struct s_tracking_state;
struct s_tracking_output;
struct s_tracked_point;
struct s_contact_dispatch_output;
struct s_biped_physics_move;
struct s_havok_component;
extern real g_47f05c;
void __stdcall function_1f0450(s_shape_state *state, byte *out, s_shape_carrier_contact const *input);
void __stdcall function_1fa810(long *ticks, s_motion_adjustment_result *out, s_motion_adjustment const *input);
void function_1ec690(byte const *input, s_state_c570 *state, byte *output);
void function_1eb6f0(s_ease_input *input, s_ease_state *state, s_ease_output *output);
void function_1fc4b0(s_tracked_point *state, s_tracking_state const *input, s_tracking_output *output);
void function_1e5b50(byte const *input, s_contact_dispatch_output *output, byte const *state);
void function_b7740(long object_index, vector3f const *linear, vector3f const *angular, bool skip);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long mode);
void function_1c4a80(long object_index, bool first, bool second);
void function_b58c0(long object_index, dword mask);
bool function_1c5210(transform4x3f const *matrix, void *shape, long component, long filter);

static __forceinline long real_to_long(real value);

/* sets or clears the blocked flag by testing a capsule just inside the shape at the component's
   position */
static __forceinline void capsule_update_1e55d0(c_z_shape_view *shape, s_havok_component *component, byte const *input, byte *out)
{
	real margin = g_47f05c + 0.001f;
	real radius = *(real *)((byte *)shape + 0xc) - margin;
	if (g_47f05c > radius) radius = g_47f05c;
	c_z_capsule_temp capsule((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
	transform4x3f matrix;
	component->rigid_body_matrix_get(0, &matrix);
	matrix.position.z += margin * 0.5f;
	if (!function_1c5210(&matrix, &capsule, *(long *)(input + 0x18), *(long *)(input + 0x24))) *(dword *)out |= 2;
	else *(dword *)out &= ~2;
}
// MiniMax D (11-20, round 132): no new form, no lower score (319): the body is unchanged. The aligned 0x94 frame still
// agrees and the first argument still arrives in eax as retail has it, but the output and the input take the other two
// of the registers retail uses because 0x1ec690 is still a stub in the file this lane may not edit, and the arms and
// the rate load are unchanged.
// MiniMax G (11-20): no new form, no lower score (319): the aligned 0x94 frame still agrees and the first argument still arrives in eax as retail has it, but the output and the input take the other two of the registers retail uses because 0x1ec690 is still a stub; the rest of the body is unchanged.
// score 319 (was 319; r239 unchanged): r130 (below) reads the objects through object_get (same code). The aligned 0x94 frame agrees. Left: retail keeps the output in EBX and the input in EDI (0x1ec690,
// still a stub, takes its input in EDI in retail) and never gives the state a register; it lays the non-compound and
// capsule arms after the epilogue, calls 0x1c4a80 where ours expands it, loads the rate once and tests the 0x20 flag by
// shift. Round 191's line at the end of this note has the latest forms.
// MiniMax A (11-19): no new form, no lower score (319): 0x1ec690 is still a stub in src/stubs/bipeds.cpp, so the output and the input still go to the other two of the registers retail uses, and the body is unchanged. BLOCKED-BY 0x1ec690 and 0xb7430.
// M3 lane 13 r125 (fast with 0xb7740, 0x1c4b00, 0x1ed340, 0x2901e0, 0x1e5b50, 0x1fc4b0, 0x1eb6f0, all MATCH): still
// BLOCKED-BY 0x1ec690 (its stub's convention; the draft's file is another lane's this round). Case 6's countdown local
// initialised in its declaration 319, the countdown passed straight to the byte store 319, both case-6 locals dropped 319,
// the velocity reference renamed 319, the tick value renamed 319 (all the same code: the slot homes do not move).
// MiniMax G (11-18): no new form, no lower score (319): the aligned 0x94 frame still agrees and the body still
// runs two bytes longer than retail's 1164; retail keeps the move pointer in EBX and the input pointer in EDI
// where this build keeps them the other way round, and that trade is settled by the case-3 callee 0x1ec690's
// stub convention in src/stubs/bipeds.cpp: BLOCKED-BY 0x1ec690.
// MiniMax H (11-18): no new form, no lower score (319): retail keeps the EAX argument in EBX and the third argument
// in EDI (`mov ebx, eax` then `mov edi, dword ptr [ebp + 0xc]`), this build keeps the third in EBX and the EAX one
// in EDI, and that trade is settled by the case-3 callee 0x1ec690's stub convention in src/stubs/bipeds.cpp, which
// this lane may not edit (BLOCKED-BY 0x1ec690); the six alias orders, the aligned capsule local and the case orders
// were already measured by earlier rounds at 319 each, so the body stands.
// MiniMax I (11-17, run 103): no new form, no lower score (319): re-measured the baseline (1176 bytes against retail's
// 1164) and the frame still agrees at 0x94; the output pointer's and the input pointer's registers still trade with
// each other against retail, and that trade is settled by the 0x1ec690 stub's convention in src/stubs/bipeds.cpp:
// BLOCKED-BY 0x1ec690.
// MiniMax S (11-16): score 319 (was 319): no new form; the aligned 0x94 frame still agrees, but retail takes its
// first argument in EAX and keeps the move pointer in EBX and the input pointer in EDI, while this build takes all
// three arguments on the stack and keeps the move pointer in EDI and the input pointer in EBX; the swap is settled
// by the stub for the case-3 callee 0x1ec690, which lives in a file this lane may not edit.
// MiniMax I (11-17): no new form, no lower score (319): the aligned 0x94 frame still agrees, but retail still
// takes the output pointer in EAX and keeps it in EBX with the input in EDI, while this build keeps the output in
// EDI and the input in EBX; both take the second argument from the stack, and the trade is settled by the 0x1ec690
// stub's convention in another file (a 900-second permuter run found no variant below the baseline of 310
// differing instructions): BLOCKED-BY 0x1ec690 (src/stubs/bipeds.cpp).
// Earlier 319 (was 319): no new form and no lower score; the aligned 0x94 frame still agrees, but the move
// pointer is in EDI here and in EBX in retail while the physics pointer trades it with it, and that swap is
// settled by the stub for 0x1ec690 (src/stubs/bipeds.cpp).
// MiniMax R (11-14): no new form, no lower score (319): retail keeps the output pointer in EBX and the input
// pointer in EDI, ours keeps the input in EBX and the output in EDI, and that swap decides the register of
// every later address field; the state, input and output pointers each taken through their parameters, the two
// late state reads through the parameter and the input without its const qualifier all compile to the same
// bytes, and a 300-second permuter run (3 tries) found no better form.
// MiniMax D (11-14): no new form, no lower score (319): the rate as a named local (one load instead of three) reaches 320, and the move/input registers still trade EBX and EDI, which the case-3 callee 0x1ec690's stub convention settles (BLOCKED-BY 0x1ec690).
// MiniMax C (11-14): no new form, no lower score (319): retail's prologue takes the state pointer in EAX
// and puts it in EBX, zeroes [EBX], then takes [ebp+0xc] into EDI; ours takes [ebp+0xc] into EBX and the
// state pointer into EDI, so every later lea and copy follows the swap and the body runs two bytes longer;
// the case-3 callee at 0x1ec690 (src/stubs/bipeds.cpp) settles which pointer the callee expects in which
// register: BLOCKED-BY 0x1ec690.
// MiniMax A (11-13): no new form, no lower score (319): the output pointer declared before the state
// pointer and declared after the first store (that store written through the move pointer) both compile to
// the same bytes; retail still keeps the output pointer in EBX and the input pointer in EDI where ours
// keeps them the other way round, and that swap is settled by the case-3 callee's own convention:
// BLOCKED-BY 0x1ec690 (src/stubs/bipeds.cpp).
// MiniMax G (11-12): no new form, no lower score (319): the velocity pointer as a pointer local gives the same code (the null test still folds), and the move/input register swap stays BLOCKED-BY 0x1ec690.
// MiniMax H (11-10): no new form, no lower score (319): the move and input pointers still trade EBX and
// EDI, and the case-3 callee's stub convention settles that swap: BLOCKED-BY 0x1ec690
// (src/stubs/bipeds.cpp).
// Earlier 319 (was 319): Aligned 0x94 frame agrees; move/input register ownership, physics reloads and velocity null test remain.
// Earlier measurement: Aligned 0x94 frame agrees; move/input register ownership, physics reloads and velocity null test remain.
// Walk b3I (11-06): every statement and block checked against retail 0x1e55d0-0x1e5a58 (field offsets, switch table 1..6 with
// 4/5 shared, case-6 0.3f scale and fistp into the countdown byte, shape compare at 0x1e5740-0x1e5757/0x1e591a, capsule arm at
// 0x1e5940-0x1e59f9, position range test at 0x1e587f-0x1e58ae, constants 0.3/1e-4/0.001/0.5/+-32768 read from the XBE): no
// behaviour difference found. Retail inlines 0xb77d0 (0x1e5775-0x1e57e1; angular NULL) and the disabled 0xb7430 position
// move (0x1e5801-0x1e58ed); the draft writes both out. `function_b77d0(object_index, &velocity, NULL)` instead of the
// written-out velocity block stays a call: 374 (1076 bytes), and the full build moves its caller 0x118700 from 92 to 148, not kept. Retail's `test ebx, ebx` (0x1e5796) is that
// inlined parameter's null test and is always false for &velocity, so no behaviour changes. The fast build also
// inlines the capsule destructor where retail calls 0xdc370 (0x1e59e1/0x1e59f4); full build not affected by this note.
// MiniMax R (11-10): no new form, no lower score (319); the move and input pointers still trade EBX and EDI,
// which the case-3 callee's stub convention settles, and the countdown byte is still converted out of line.
// MiniMax R (11-10): no new form, no lower score.
// MiniMax I (11-05): no new form, no lower score. Retail takes its first argument in eax and keeps the
// output in ebx and the physics pointer in edi, ours takes all three on the stack; retail's case-3
// callee 0x1ec690 also takes the input in edi and the state in eax (src/stubs/bipeds.cpp, another lane).
// MiniMax B (11-04): no new form, no lower score.
// Earlier: Aligned frame 0x94 agrees; move/input ownership, physics reloads and native velocity guard remain.
// MiniMax lane C (run 36, 10-31): no new form, no lower score; the move and input pointers still
// trade EBX and EDI, and ours still keeps the state pointer in ESI where retail reads it from its
// stack slot; 1176 bytes against retail's 1164.
// Round 16: retail takes its first argument in EAX and copies it to EBX at once, and reads the
// third argument from [ebp+0xc]; ours takes the same three arguments in the same registers, but
// keeps the output pointer in EDI and the input pointer in EBX where retail keeps them the other
// way round.
// Earlier: Aligned 0x94 frame agrees; move/input register ownership, physics reloads and velocity null test remain.
// BLOCKED-BY 0x1ec690 (round 30): retail passes the case-3 callee's input in EDI with two stack arguments; our
// stub (src/stubs/bipeds.cpp) takes ECX/EDX and one stack argument, which is what moves input/output to
// EBX/EDI here.
// Earlier: retail's aligned frame (and esp, -16; 0x94) and prologue. Left: the move and
// input pointers trade EBX and EDI (retail move EBX, input EDI, physics read from its stack slot at
// each use; ours keeps physics in ESI), and the inlined 0xb77d0 lacks retail's velocity null test
// (1176 bytes against 1164).
// Round 74: the capsule test (an automatic 16-byte-aligned capsule, the flag update inside its scope)
// moved into a force-inline helper: the frame aligns as retail's while 0xb77d0, 0x1c4b00, 0x1ed340 and
// 0x2901e0 keep matching (the aligned local in this body itself moves 0x1c4b00's EBX argument to the
// stack) 353 -> 319; replaces the hand-aligned placement-new buffer and the explicit destructor call.
// At 319 or worse: move/state/input alias declaration orders 319 each; case 6 before 4/5 (retail's
// block order) 318; compound test as an if/else 388 (1212 bytes); 0xb77d0 called (stays out of
// line) 374; input flag as a shifted bit 325; both 366.
// Global optimization off 988, inline depth 2 348, no reference truth test 348, shape flag result 352, combined 352.
// Base-position/velocity/both input values 379f/382f/413f; move-stage handle 352f;
// minimum-radius value 352f; activity/compound byte 352f/352f; header offset 352f.
// Last five forms retain the unchanged reduced executable instructions (preview 352f).
// frame 0x8c versus aligned 0x94; global optimization off 988, inline depth 2 348, no reference truth test 348, shape flag result 352, combined 352.
// qword backing 357; entry backing lifetime 357.
// entry matrix lifetime 357; both entry workspaces 357.
// immutable margin 357; immutable shape 357.
// immutable component 357; native point initializer 357.
// Native stack allocation 362 (1148 bytes); shared component/body/both lookups 353/353/353;
// base-position reference 353; shared object lookups 353; named height offset 353;
// distinct current-shape lifetime 353. Seven tying forms share the preview instruction prefix.
// Lead (@coldspear, #801): with the capsule made an aligned local instead of the placement-new
// buffer, the frame becomes retail's aligned one (and esp, -16) and the count falls to 326.
// Frame 0x8c versus aligned 0x94 remains; complete dispatch 4/5,1,3,6,2 350;
// 4/5,3,2,6,1 350; 4/5,6,3,1,2 354; 6,3,1,4/5,2 353; 6,4/5,2,3,1 354.
// Position yzx/zxy/zyx 353/353/353 (plain); no qualifying executable gain.
// Native output velocity 369; orientation 353; velocity/orientation 369; control fields 353;
// velocity/control 369; orientation/control 353; all fields 369; all fields with one time-step observation 374.
// Round 223: Complete shape decision result bool/byte/short/long, declared at its producer
// or at entry, all 364 (1148 bytes); common flag publication restored.
// Round 148: shape reads at every use 374; body-shape reads at every use 353; handle reads at
// every use 392; shape/body-shape 374; shape/handle 413; body-shape/handle 392; all field
// observations 413; all fields/shared length helper 413. All executable forms restored.
// Round 254: cached fallback shape kind with manual/aligned/shared-update/aligned-shared
// 361/316/435/398; those four with one time step 362/314/431/394.
// Aligned forms lose matched callees; other forms regress. Original repeated queries,
// manual capsule lifetime, expanded update and complete executable body restored.
// Round 358: Existing duration conversion with raw/shared length and raw/shared point production
// 353/353/353/353; all four with one three-use step 356 each. Manual capsule lifetime and body
// restored; matched controls preserved.
// Round 23: library input-orientation/zero-velocity/support copies and all pairs/all
// 353 each; all with final point library publication 353. All eight keep 1164 bytes,
// the plain 0x8c frame and 340 executable instructions; original transfers restored.
// Round 203: native vector setter for orientation, velocity, both, support/reset, orientation/support,
// velocity/support and all transfers 377/353/380/371/398/374/395; all with final point setter 395.
// All keep plain frame 0x8c and grow the code; every transfer and original body restored.
// Round 227: Capsule/intermediate-base destruction, each with immutable binding and earlier matrix combinations 355/353/355/353/355/353/355/353.
// Original body and all helpers restored.
// Round 237: component/body at uses and one input handle 368/357/356; component-body/
// component-handle/body-handle/all 398/367/356/359; all with automatic capsule 344, losing seven
// checked matches (control 353). Original body restored; plain frame and callee propagation remain.
// Round 14: the placement-new buffer as a 16-byte aligned local reaches 305 with retail's exact
// aligned frame and length (1164 bytes), the automatic aligned local 307; both break the matched
// functions 0xb77d0, 0x1c4b00, 0x1ed340 and 0x2901e0 (29/49/133/45) that the plain form keeps
// matching. Original body restored; plain frame and callee propagation remain.
// Round 390: Separate target inline/noinline contracts with manual/aligned-byte/automatic
// capsule lifetimes and aligned-byte period observation: 353/353/305/305/307/307/306/306. Original body restored.
// Aligned forms recover frame 0x94 but lose four matched controls; declaration contracts do not fix it.
// Round 269: velocity pointer 319; direct state parameter 319; both 319; direct input parameter 319;
// velocity/input 319; state/input 319; all 319; all with shared squared length 319. Original body restored.
// Round 418: native capsule component/flag-word/both/read-only-shape/
// component-shape/flags-shape/all/output-first contracts 319/319/319/319/319/319/319/319.
// All original contracts restored; aligned frame retained, move/input/physics
// ownership and the velocity null test remain.
// Round 308: Tail-handle/entry-point-workspace/native-byte-countdown subsets 325/319/325/319/325/319/325;
// all with word countdown 325. Every form preserves four matched controls; original lifetimes and
// carrier restored. Aligned frame agrees; pointer ownership and native velocity guard remain.
// Round 223: existing complete velocity updater on the aligned-helper graph, with fallback-kind/step/both,
// 374/386/370/382; each with direct state/input parameters 374/386/370/382. Updater stays out of line;
// original aligned capsule helper, expanded motion update and complete body restored.
// Round 2 of lane 18: the shape decision as a force-inline bool helper (compound arm returning the
// two-way test, else the direct compare) with early returns/one result 388/388 (1212 bytes; gives
// retail's block layout, the non-compound and capsule arms after the return, but the input then reloads
// from its slot and the velocity pointer takes EBX); with case 6 before 4/5 388; with a velocity
// pointer 388; both 388. Original body restored.
// Round 30 (lane 11, plain): the disabled 0x1ec690 draft (src/unknown_1eb8a0.cpp) activated with the stub
// removed 349: LTCG then passes that input in EDI and moves the output pointer to EBX as retail, but the
// draft is far off (601) and matched 0x2901e0 and 0x1ed340 stop matching (29/45); reverted.
// Also (fast): the velocity reference as a pointer local with `velocity &&` kept 319 (same code; VC folds
// the null test, where retail tests the pointer reloaded from its slot: 0xb77d0's inlined body, which
// LTCG cannot inline from its /Ob1 file).
// Also (fast): the rate on the right of each position product 319; the activity bit in shift form 319;
// the object lookup and activity read before the position computation 324 (1180 bytes). All restored.
// Round 54 (lane 10, fast): case 6 rounding straight into the state byte through real_to_long 319 (same
// code); countdown tested `!= 0` with post-decrement 320; moving test through length_sq3f 319 (same code);
// same-shape arm last as an if/else swap 319 (same code); current shape read once for both tests 368;
// the capsule radius clamp as a conditional expression 319 (same code); the ease state as the switch's
// default instead of case 6 with __assume 336.
// Round 280: aligned-helper conditional radius selection, matrix-first declaration,
// result-first lower clamp and consistent minimum-radius snapshot combinations: eight
// forms 319 each. Original helper and complete body restored; pointer ownership remains.
// Round 332: Native state header with shared squared length, one time step and native point, subsets
// 0-7: 319/319/320/320/319/319/320/320.
// Original executable source restored; no eligible gain.
// Round 284: inactive-tail early return, activity bool/byte/short/long and separate
// refreshed-object lifetime: 319/319/318/319 twice. One-point word-storage gain rejected;
// original aligned helper and complete body restored. Entry pointer ownership remains.
// DL231: Existing aligned capsule helper complete bool/byte/short/long results, direct predicate then
// result-first: 330/330/330/330/330/330/339/334. Move/input/physics ownership, case-3 convention and
// native velocity guard remain; original executable source restored.
// Round 43 (m3 lane 2, fast): the three alias locals in all six declaration orders 319 each (local
// symbol order does not move the move/input registers); countdown as `if (state[1]) state[1]--;` 320;
// the 0x54 flag stored after the 0x58 copy 323; the velocity test without `&velocity &&` 319 (same
// code); case 6 as one expression 319 (same code). Still BLOCKED-BY 0x1ec690 (input in EDI).
// Round 58 (lane 14; aligned counts in brackets, this body [215]): the draft of 0x1ec690 activated, plain, 349 [235] (input still EBX->EAX at entry; the draft inlines
// 0x1ed340 where retail calls it; 0x2901e0 29, 0x1ed340 45, 0xb77d0 49, 0x1c4b00 133 stop matching, 0xfa100 holds); fast: state through a pointer to its parameter 319
// at each use and read once 319, state written as the parameter at each use 319 (all same code); a named base pointer for the three position reads 319; with a named
// component index too 319.
// Round 138 (m3 lane 8; plain unless marked): the 0x1ec690 draft activated with its stub removed 349 again (0x2901e0 29, 0xb77d0 49,
// 0x1c4b00 133, 0x1ed340 45 stop matching; restored), so the move/input roles (retail EBX/EDI, state kept on the stack) stay
// BLOCKED-BY 0x1ec690. Fast: the shape test as an if/else on the compound result, each arm re-reading the current shape, 388 (1212);
// `same = compound ? current == shape || inner == shape : current == shape` 335 (1204 bytes, masked 235 -> 110: retail's
// `cmp eax, 0x17; jne` with the non-compound path out of line); the same ternary in the `if` 335; the `||` written in the `if` 319
// (same code); the input flag as `(bool)((flags >> 5) & 1)` 325 (1180: retail's `shr ecx, 5; test cl, 1`), with the ternary 351.
// Round 398: Existing aligned capsule shape/component/flag-word object-reference contracts,
// seven subsets then ordinary inline: 319/319/319/319/319/319/319/530. NOT kept: forced forms tie;
// ordinary expansion loses the native frame and regresses. Original helper, caller and full body restored.
// F402: Four-byte movement/input/state pointer-home lexical identities, seven
// subsets then the complete graph with the velocity identity, 319 each. NOT kept:
// all canonicalize; original names, aligned helper and complete body restored.
// M400: Complete original matrix/position/conversion output defaults, seven subsets and CRT,
// 312/330/323/319/312/330/323/317. NOT kept: matrix default gives native move/input registers
// and first +0x8f at 312, but grows to 1200/1164 bytes and gains only seven; all original producers restored.
// Round 112 (m3 lane 9, fast): both builds end in `mov esp, ebp; pop ebp; ret 8`; retail puts the epilogue
// mid-body with the non-compound shape test and the capsule/flag arms after it. Ours inlines 0x1c4a80 where retail
// calls it (an inline budget or site-frequency effect, perhaps of retail inlining 0xb77d0 here). Tried: the position
// through the existing vector3f_add_scaled 320; case 6 as `0.3f * ticks` 319 (same code); the bounds as
// `x <= 32768.0f` 319 (same code); `*(long *)(state + 4)` at each use instead of object_index 339 at 1192; object_get()
// for the two tail lookups 319 (same code). The move/input registers stay BLOCKED-BY 0x1ec690 (stub in
// src/stubs/bipeds.cpp); M400's matrix default looks like an initialiser that rigid_body_matrix_get overwrites.
// Round 244 (m3 lane 3, fast): the 0x1ec690 stub (its only caller is this function) declared __cdecl, caller-pops as
// retail's callee is, 328 (1180 bytes; input stays EBX, output EDI: the stub's ECX argument is not what picks them);
// 0xb7430's matching body enabled with its stub removed (diagnostic, restored) 319: 0x1c4a80 stays inlined here, so its
// inlining is not from 0xb7430 being a stub. The velocity block on its own object-index read and a second
// `*(long *)(state + 4)` read for the position move (retail reads [state+4] once per inlined helper) 319: the tail then
// has retail's re-read and its lea/object/flags registers, but the count does not move; on it the input flag in shift
// form 330, the shape match as `compound ? a || b : a` 345 (1204 bytes), both 348. All restored.
// MiniMax C (11-17): no new form, no lower score (319): the arguments already sit in retail's homes (the move
// pointer in EAX, the state at +0x8 and the input at +0xc) and only the two pointer registers differ (retail's
// out in EBX and its input in EDI, ours the other way round), which turns every later instruction; the six
// orders of the three aliases do not move them.
// M3 lane 6 r191 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319): stubs are built without
// link-time conventions (docs/DECOMPILING.md), so no stub signature can take the input in EDI as retail's 0x1ec690 does
// (it reads [edi + 0x18] at entry, two stack arguments, callee pops): the EBX/EDI trade needs the real 0x1ec690, whose
// draft is disabled in src/unknown_1eb8a0.cpp. With that trade masked out 298 of 335 aligned instructions still differ,
// from the block layout, the expanded 0x1c4a80 and the hand-written 0xb77d0/0xb7430 bodies. Combinations of forms that
// need no match, all worse: the rate named for its three products 320; the 0x20 flag in shift form 325 (1180 bytes);
// both 330 (1184); both with the rate on the right of each product 330; both with the object index read again for the
// position move 331. BLOCKED-BY 0x1ec690.
// Round 620: the move and input exchange EBX and EDI, and the physics pointer stays in ESI.
// Live shape slot 328, saved shape identity with live virtual accesses 342, direct shape field 331,
// conditional child comparison on the saved identity form 353. Capsule floor first 319,
// single minimum-radius read 319, extracted activity byte 319, typed late parent access 319.
// M3 lane 9 r138 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, MATCH each; positional / bytes / aligned /
// register-masked, base 319 / 1176 / 320 / 235): the unparented position move as a file-local forced-inline function
// with 0xb7430's own parameter tests (`if (position)`, `if (physics)`, the keep-partition tests) called with
// constants 355 / 1104 / 299 / 207: 0x1c4a80 is then a call as in retail, whose call sits under 0xb7430's `if
// (physics)` (the test folds away after the expansion but keeps the nested call out of line). The shape flag assigned
// in the two arms of the compound test, non-compound arm first 411 / 1220 / 336 / 204, compound arm first 388 / 1212
// / 268 / 116 (retail's out-of-line arm with `sete al; test al, al`); the position function with the compound-first
// flag 358 / 1140 / 244 / 91; those with the 0x20 flag in shift form 358 / 1140 / 241 / 90. None is keepable without
// a match (the positional count rises). Left on the last: the output/input registers (BLOCKED-BY 0x1ec690), retail's
// null test and single rate load from the expanded 0xb77d0, and the shape re-read from the input at each use of the
// capsule chain. BLOCKED-BY 0xb7430 (disabled: it closes a call cycle) for the 0x1c4a80 call.
// Round 145 (m3 lane 16, fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319): the component
// and its first rigid body read through the existing havok_component_get / havok_component_rigid_body_get (the body had
// both written out), the capsule helper taking the component typed: 319, the same code (kept). Retail's position
// update loads the rate once and multiplies `rate * v.i`, `v.j * rate`, `v.k * rate` with the base added from memory:
// the existing vector3f_scale into a temporary then the base added 335 (1188 bytes); scaled and added through
// vector3f_add 335; scaled into the position then the base added in place 335 (the compiler loads the base and adds
// the product to it). Still BLOCKED-BY 0x1ec690 for the EBX/EDI trade (its draft sits in another lane's file).
// M3 lane 6 r201 (fast with 0xb77d0, 0x1c4b00, 0x1ed340, 0x2901e0, 0x1c4a80, all MATCH unless noted; control 319;
// positional / bytes / aligned-equal of retail's 335 / register-masked equal, base 319 / 1176 / 133 / 216; all restored).
// Diagnostic: with the disabled 0x1ec690 draft switched on in place of its stub, the call is retail's (two pushes, the
// input moved into EDI) but the output takes EBX, the state ESI and the input only EAX: 349, and in that image
// 0x2901e0, 0xb77d0, 0x1c4b00 and 0x1ed340 all lose, so the draft is no shortcut. The existing 0xb77d0 called in place
// of its written-out body 374 / 1076: the call is not expanded (its file is /Ob1) and this function's own return
// becomes `ret 4` (the input then arrives in a register; retail and main return with `ret 8`), so every form with that
// call is off retail's convention (with the forms below 292 / 1128, 312 / 1128, 307 / 1132, 338 / 1100). On main's
// calls: the shape match assigned in the two arms of the compound test, compound arm first (lane 9 r138's form,
// re-measured) 388 / 1212 / 156 / 283; that with the shape read from the input at each use after the match (retail
// re-reads [input + 0x14c] before each call of the chain) 345 / 1224 / 213 / 294; with no shape local at all (the
// field read at every use) 362 / 1224 / 201 / 290. The state read once through the parameter's own address 319 (same
// code as main), on the 345 form 346: the state keeps ESI either way. Retail never gives the state a saved register
// (it reloads [ebp + 8] before the switch, for 0x1e5b50 and for each object index read) and adds 0x10 to that copy
// in place in each case.
// Round 149 (m3 lane 16, fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319): the capsule
// helper with two exits (`|= 2; return;`) 350 (1200 bytes; the destructor stays expanded, now twice, where retail calls
// 0xdc370 in each arm as the matched 0xa9f70 does); the capsule in its own block with a hit result tested after it
// (0xa9f70's shape) 323; the position products as `rate * v.i`, `v.j * rate`, `v.k * rate` 319 (same code); the body's
// current-shape field read at each use with main's `||`/`&&` 319 (same code); the same as `compound ? (field == shape ||
// field->0x30 == shape) : field == shape` 388 (1212 bytes) but aligned exact 214 -> 206 and register-masked 131 -> 79:
// the compound test with its out-of-line non-compound arm, the velocity block and the position block become retail's,
// while the input loses its register (reloaded from [ebp+0xc]); with the shape also read from the input at each use of
// the type chain and for the capsule (retail re-reads [input+0x14c] before each virtual call) 346 (1224 bytes, aligned
// exact 136, register-masked 55). Left on that: the input register (BLOCKED-BY 0x1ec690), and 0x1c4a80 and the 0xdc370
// destructor expanded here where retail calls both (open: retail also expands 0xb77d0 here, which its /Ob1 file keeps
// a call in this build, or the link's inline threshold sits one level deeper than retail's, as at 0x25d740).
// M3 lane 4 r227 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319; a/s/r = LCS-aligned
// unmatched lines exact / stack-masked / register-masked, control 214/207/131): the active flag as
// `(bool)((flags >> 8) & 1)` 319 (same code: ours already emits retail's `shr ebx, 8; and bl, 1`); that with the 0x20
// input flag in shift form 319 (215/208/131); the countdown stored as `(byte)real_to_long(ticks)` with no `rounded`
// local 319 (same code); the velocity reference declared ahead of the first output store 331; the active flag read as
// the flags' second byte (`object[5] & 1`) 304 at retail's 1164 bytes but 220/212/133 (a byte read where retail
// shifts the dword, so a realignment, not kept). Still BLOCKED-BY 0x1ec690: its draft sits under `#if 0` in a file on
// this round's Avoid list, and no stub convention passes the input in EDI.
// M3 lane 8 r202 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319, aligned 324, register-masked 251):
// the 0x1ec690 stub and its declaration `__stdcall` 317 (aligned 317, masked 242; the EBX/EDI trade stays); on that the 0x20
// flag in shift form 318 (aligned 306), the rate read once 318 (aligned 312), the shape match as `compound ? a || b : a` 349
// (1204 B, aligned 223, masked 123), all four 346 (1208 B, aligned 202, masked 115: the non-compound arm, rate and flag test
// then as retail; the extra 44 bytes are 0x1c4a80 and the capsule destructor expanded where retail calls both).
// Shared case payload 317, with countdown snapshot, separate state scopes, or both 317 each.
// Mode read before decrement 341, separate state scopes with or without countdown snapshot
// 319 each, shared payload with scoped state and countdown declared in the test 318.
// NOT kept: the shared payload reaches 1164 bytes with at most a two-point gain and
// keeps the entry register exchange.
// M3 lane 7 r200 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH throughout; control 319): the
// countdown through a byte reference bound to the state's second byte 319 (only the position products' operand order
// moves); the switch byte read into a named long 319 (same code); the input bound by a const reference for the shape
// read 319 (same code); the three aliases declared input first with the state as a const reference to the parameter 319
// (same code); the velocity pointer held through a const reference and tested in place of `&velocity` 319 (the null
// test still folds, operand order only). Still BLOCKED-BY 0x1ec690 (the input in EDI; its draft sits in an Avoid-list
// file this round).
// M3 lane 2 r118 (fast with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319): the output flag word
// bound once by reference for the shape block's three updates 319; the position base through a const reference to the
// input's point 319; the velocity reference formed from the move parameter instead of the output alias 319 (all the
// same code); case 6 ahead of 4/5 (retail's block order) re-measured 318; the capsule helper plain `static inline` 530
// (968 B, it stays out of line); the position bounds as a negated `||` chain 319, the shape match folded into the `if`
// 319, the active flag through TEST_FIELD_BIT 319, `NONE !=` on the parent test 319 (all the same code). Also
// BLOCKED-BY 0xb77d0 this round: `inline` on its /Ob1 definition (so the link could expand it here as retail does, in
// place of the written-out velocity block) needs src/unknown_0b7740.cpp, another lane's file this round.
// Shared case payload 317, with countdown snapshot, separate state scopes, or both 317 each.
// Mode read before decrement 341, separate state scopes with or without countdown snapshot
// 319 each, shared payload with scoped state and countdown declared in the test 318.
// NOT kept: the shared payload reaches 1164 bytes with at most a two-point gain and
// keeps the entry register exchange.
// M3 lane 1 r141 (fast with 0xb77d0, 0x1c4b00, 0x1ed340, 0x2901e0 and 0x1c4a80, all MATCH each run; control 319 at 1176 B,
// aligned 321, register-masked 236). Still BLOCKED-BY 0x1ec690 (ours; its draft under `#if 0` in src/unknown_1eb8a0.cpp is
// far off and, switched on, loses 0x2901e0, 0xb77d0, 0x1c4b00 and 0x1ed340, rounds 30 / 58 / 138 / 201 above) and by the
// written-out 0xb77d0 / 0xb7430 bodies (0xb77d0's /Ob1 file is another lane's this round; 0xb7430 is still a stub). Forms:
// the velocity pointer as a local read once through its own address, the way the real 0xb77d0 keeps its parameter, 319
// (same code: the null test still folds); that with the shape match as `compound ? (a || b) : a` 335 (1204 B, aligned
// 229, masked 113: round 138's form re-measured, the pointer read changes nothing on it); the input's flag word pointer
// read into a local ahead of the shape block 323 (1180 B); the object index read ahead of the shape block 325 (1180 B);
// the mode byte read into a named long ahead of the countdown 321. Body unchanged.
// MiniMax R (11-20): re-checked, no new form and no lower score (319): retail keeps the result pointer in ebx and the input pointer in edi while this build keeps the input pointer in ebx and the result pointer in edi, so every instruction from the prologue on differs, and the spilled address of the velocity field lands in [esp + 0x20] where this build uses [esp + 0x1c]; replacing the reference bound local with a pointer and reordering the aliases change nothing.
// M3 lane 2 r127 (fast; control 319, aligned / register-masked 320 / 247): the component as
// `(s_havok_component *)g_51e9b8->data + (index & 0xffff)` 319 (same code; retail loads the pool before masking here);
// round 138's `compound ? (a || b) : a` re-measured 335 (1204 B, 228 / 129); the compound test written in that ternary's
// condition with the shape reloaded in its arm 388 (1212 B, 268 / 135: retail's shape block, lane 18's 388 layout); on
// the ternary the 0x20 flag as `(flags >> 5) & 1` 335 and the state read through the parameter at each use 335 (both the
// same code). On the ternary the masked rows left are mostly the expanded 0x1c4a80 (retail calls it from the inlined
// 0xb7430 position move, which is disabled: BLOCKED-BY 0xb7430), the output/input register trade (BLOCKED-BY 0x1ec690)
// and the state pointer held in ESI where retail re-reads it.
// M3 lane 5 r222 (H2M_FAST with 0xb77d0/0x1c4b00/0x1ed340/0x2901e0/0x1c4a80, all MATCH; control 319 at 1176 B, aligned 320,
// register-masked 247): the shape match as `same = current == shape;` then set under `if (!same && compound)` 331 (1184 B);
// the shape pointer not read again after the type call 368 (1180 B); the position rows with the base first and the rate
// on the right in the j and k rows 319; the active flag in shift form 319; `if (shape != NULL)` with the last arm on its
// own line 319 (the last three the same code). Still BLOCKED-BY 0x1ec690 for the output/input registers.
// M3 lane 2 r130 (h2m fast with 0xb77d0, 0x2056e0, 0x1c4b00, 0x1ed340, 0x2901e0 and 0x1c4a80, all MATCH; control 319;
// plain confirmed). Kept: the two object lookups through the existing object_get 319 (same code). Measured, not kept:
// 0xb77d0's definition marked `inline` in its own /Ob1 file and called here in place of its written-out body 319 (the
// link then expands it here as retail does, same code), but the full check loses matched 0x2056e0, which expands it
// too (1390 B against 1265); on that, the position as vector3f_add_scaled(base, &velocity, rate, &position) in place
// of the three written-out rows 315 (1172 B), which without the inline call is 320; the disabled 0x1ec690 draft
// switched on (stub removed) 349, with 0xb77d0 49, 0x1c4b00 133, 0x1ed340 45 and 0x2901e0 29 (round 201's losses
// again); the velocity reference renamed 319 (same code); on the 315 form: the 0x20 input flag in shift form 320, the
// shape match as one ternary on the compound test 383 (1208 B), the active flag through TEST_FIELD_BIT 315 (same
// code), the input's flag word in a pointer local 315 (same code), the current shape read from the body at its first
// use 320. Still BLOCKED-BY 0x1ec690 for the output/input registers.
// M3 lane 6 r225 (H2M_FAST with 0xb77d0, 0x1c4a80 and 0x1c4b00, all MATCH; control 319, aligned 320, register-masked
// 235; the masked rows are the out-of-line arms, the expanded 0x1c4a80 and the state held in ESI): diagnostic, 0xb7430's draft enabled with its stub removed and
// called here for the position move (`function_b7430(object_index, &position, NULL, NULL, NULL, false, true, false,
// false)`) 355 (1104 B; 0x1c4a80 is then a call as in retail, but 0xb7430 itself 109 and matched 0xb75a0 73 on the
// reduced image); the state read through the parameter's own address 319 (same code); a second named state pointer for
// the contact call and the object-index read 319 (same code); diagnostics on the 0x1ec690 stub (its only live caller is
// this function): parameters as (state, output, input) 318, that `__stdcall` 316 (the EBX/EDI trade stays). Still
// BLOCKED-BY 0x1ec690 and 0xb7430.
// M3 lane 16 r180 (H2M_FAST; control 319, aligned 320, register-masked 247; still BLOCKED-BY 0x1ec690): the 0x20-flag block
// behind an early `return` when the flag is clear 319 (same code); the countdown test as `if (state[1])` 320; the object
// index read ahead of the flag test 319 (masked 251); the capsule chain reading the shape from the input at each use 342
// (1200 B); the case-6 countdown declared ahead of the tick value 319 (same code). Re-measured: the compound test as an
// if/else setting the match flag, compound arm first, 388 (1212 B, aligned 268, masked 135). Body unchanged.
// M3 lane 5 r239 (H2M_FAST with 0xb77d0, 0x1c4b00, 0x1c4a80, 0x1ed340 and 0x2901e0; control 319, all MATCH). Reading:
// retail calls the capsule's base destructor 0xdc370 out of line in both capsule arms (`lea ecx, [esp + 0x30]; call`),
// as 0xa9f70 does; ours expands it in the forced-inline capsule helper. Diagnostic, restored at once: 0xdc370 (a
// contributor's) compiled without link-time code generation beside the capsule constructor 0x2de0a0 303 at retail's
// 1164 B (0xa9f70 172 -> 188, then calling 0xdc370 through ECX as retail does), so retail's 0xdc370 sits outside it (a
// placement and flags lead for its owner); with the 0x1ec690 stub `__stdcall` as well 301; with the 0x1ec690 draft
// enabled 334 (its usual losses). The capsule arm written at its only call site with the one-caller helper dropped 307
// at retail's 1164 B (each arm then calls the capsule destructor, retail's shape), but the plain check loses matched
// 0xb77d0 (49), 0x1c4b00 (133), 0x1ed340 (45) and 0x2901e0 (29): 0x1c4b00's linear argument leaves EBX because the
// flag block then gives the velocity ESI (the same four the 0x1ec690 draft loses). On that form: the flag set after a
// block holding the capsule 319 (1148 B); the object index read from the parameter 307 (same code); the arm reading
// the shape from the input 312 (1172 B), every shape test reading it 344 (1192 B); the capsule placement-constructed
// and never destroyed (diagnostic) 325 with the same four losses; with the 0xdc370 diagnostic 307 (losses unchanged).
// Body unchanged; BLOCKED-BY 0x1ec690 and 0xdc370's placement.
// M3 lane 8 r235 (H2M_FAST with 0xb77d0, 0x1c4b00, 0x1ed340, 0x2901e0 and 0x1c4a80, all MATCH; control 319). Why the
// 0x1ec690 draft loses four matches (diagnostics on the draft, restored): with it on, this body's tail holds the object
// index in EBX and the velocity address in ESI at the 0xb7740 / 0x1c4b00 calls (main and retail the other way), and
// 0x1c4b00's linear register argument follows that caller. The draft without its sphere-selection block keeps all four
// (this body 348: move pointer in retail's EBX, but the input in EAX and the state still in ESI); the block reduced to
// a loop computing the clamped radius from a hoisted margin still loses them, the loop with only the subtraction or only
// the clamp keeps them, an empty real body keeps them (305 at 1164 B). Forms: the component through the pool data bound
// to a const reference (data loaded before the mask, as retail) 318; on it the input's shape field bound to a
// `c_z_shape_view *const &` 326 (1188 B), the body's current shape so bound 319, both 331 (1192 B), the tail's object
// index read through the physics parameter 318, the countdown byte bound to a `byte &` 318.
// M3 lane 1 r167 (h2m fast with 0xb77d0, 0x1c4b00, 0x1ed340, 0x2901e0 and 0x1c4a80, all MATCH; control 319, aligned
// 214/202, registers masked 131/119). Retail's case 3 pushes only the output and the state and passes the input in
// EDI, so the input's EDI and the output's EBX follow that callee's register argument, which the stub cannot give
// (still BLOCKED-BY 0x1ec690). Forms: the three byte views declared `*const` 319 (same code); the mode switch's
// default as `break` 343 (1180 B, a range check returns); the tail's object index read at the top 369 (1180 B); the
// shape match as a bool set by a second test 331 (1184 B); the input's flags word through a named pointer 319 (same
// code). Body unchanged.
// M3 lane 11 r168 (fast screen with 0xb77d0, 0x1c4b00, 0x1ed340, 0x2901e0 and 0x1c4a80, all MATCH each run; control
// 319 at 1176 B, aligned 321; body unchanged). Reading: the allocator ranks the move pointer first (175 against the
// input's 93) and it takes EDI because EBX carries a large penalty over its range; retail's case-3 callee takes the
// input in EDI, which is what would turn that choice, so still BLOCKED-BY 0x1ec690. Diagnostic (a forced register
// choice, not source): the move pointer alone moved to EBX gives 296 with the first difference at +0x8f, so 296 of
// the count is layout, the expanded 0x1c4a80 and capsule destructor and the hand-written 0xb77d0 / 0xb7430 bodies.
// Forms: the velocity as a pointer local set at the top and used for the two calls, the test and the products 319
// (same code: this build already reloads that address from its slot into EBX for 0x1c4b00, as retail does); with a
// null pointer local for the angular argument (diagnostic) 319 (same code); the pointer local with the rate named
// once and the products as rate * i, j * rate, k * rate 320; the j and k components read into named locals ahead of
// the products 319 (same code); the pointer local, the named rate and the object index read again for the position
// move 325 (1180 B); the named rate and the second index read alone 325.
// @retail 0x1e55d0
void __stdcall function_1e55d0(s_biped_physics_move *move, void *physics, s_biped_physics_output *output)
{
	byte *out = (byte *)move;
	byte *state = (byte *)physics;
	byte const *input = (byte const *)output;
	*(long *)out = 0;
	*(vector3f *)(out + 0x24) = *(vector3f const *)(input + 0x100);
	*(vector3f *)(out + 0x18) = *(vector3f const *)(input + 0xf4);
	vector3f &velocity = *(vector3f *)(out + 0xc);
	velocity = *g_4687a4;
	*(long *)(out + 4) = 0;
	out[8] = false;
	out[0x54] = false;
	*(vector3f *)(out + 0x58) = *(vector3f *)g_468788;
	*(vector3f *)(out + 0x64) = *g_4687a4;
	if (state[1] > 0) --state[1];
	switch (*state)
	{
	case 1: function_1f0450((s_shape_state *)(state + 0x10), out, (s_shape_carrier_contact const *)input); break;
	case 2: function_1fa810((long *)(state + 0x10), (s_motion_adjustment_result *)out, (s_motion_adjustment const *)input); break;
	case 3: function_1ec690(input, (s_state_c570 *)(state + 0x10), out); break;
	case 4:
	case 5: function_1fc4b0((s_tracked_point *)(state + 0x10), (s_tracking_state const *)input, (s_tracking_output *)out); break;
	case 6:
	{
		real ticks = (real)g_510c54->field_2_3 * 0.3f;
		long rounded;
		rounded = real_to_long(ticks);
		state[1] = (byte)rounded;
		function_1eb6f0((s_ease_input *)input, (s_ease_state *)(state + 0x10), (s_ease_output *)out);
		break;
	}
	default: __assume(0);
	}
	function_1e5b50(input, (s_contact_dispatch_output *)out, state);
	c_z_shape_view *shape = *(c_z_shape_view **)(input + 0x14c);
	if (shape)
	{
		s_havok_component *component = havok_component_get(*(long *)(input + 0x18));
		byte *body = (byte *)havok_component_rigid_body_get(0, component);
		c_z_shape_view *current = *(c_z_shape_view **)(body + 0xc);
		bool compound = current->v5() == 0x17;
		current = *(c_z_shape_view **)(body + 0xc);
		bool same = current == shape || compound && *(c_z_shape_view **)((byte *)current + 0x30) == shape;
		if (same) *(dword *)out |= 2;
		else if (shape->v5() == 7)
			capsule_update_1e55d0(shape, component, input, out);
		else if (shape->v5() == 4 || (shape->v5() == 0x15 && (*(c_z_shape_view **)((byte *)shape + 0xc))->v5() == 4)) *(dword *)out |= 2;
		else *(dword *)out &= ~2;
	}
	if (**(dword const **)(input + 8) & 0x20)
	{
		long object_index = *(long *)(state + 4);
		function_b7740(object_index, &velocity, NULL, false);
		function_1c4b00(object_index, &velocity, NULL, 1);
		if (&velocity && velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
		{
			function_b9b90(object_index, false);
			function_b7360(object_index);
			function_bba20(object_index);
		}
		point3f position;
		position.x = g_510c54->rate * velocity.i + *(real *)(input + 0xe8);
		position.y = g_510c54->rate * velocity.j + *(real *)(input + 0xec);
		position.z = g_510c54->rate * velocity.k + *(real *)(input + 0xf0);
		byte *object = (byte *)object_get(object_index);
		bool active = (*(dword *)(object + 4) & 0x100) != 0;
		if (active) function_b87b0(object_index);
		if (*(long *)(object + 0x14) != NONE ||
			(position.x >= -32768.0f && 32768.0f >= position.x && position.y >= -32768.0f &&
			32768.0f >= position.y && position.z >= -32768.0f && 32768.0f >= position.z))
		{
			*(point3f *)(object + 0x64) = position;
			object = (byte *)object_get(object_index);
			if (*(long *)(object + 0xd4) != NONE) function_b58c0(*(long *)(object + 0xd4), 2);
		}
		function_1c4a80(object_index, false, false);
		if (active) function_b8600(object_index, 0);
		function_bba20(object_index);
	}
}
