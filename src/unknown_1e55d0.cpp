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
static __forceinline void capsule_update_1e55d0(c_z_shape_view *shape, byte *component, byte const *input, byte *out)
{
	real margin = g_47f05c + 0.001f;
	real radius = *(real *)((byte *)shape + 0xc) - margin;
	if (g_47f05c > radius) radius = g_47f05c;
	c_z_capsule_temp capsule((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
	transform4x3f matrix;
	((s_havok_component *)component)->rigid_body_matrix_get(0, &matrix);
	matrix.position.z += margin * 0.5f;
	if (!function_1c5210(&matrix, &capsule, *(long *)(input + 0x18), *(long *)(input + 0x24))) *(dword *)out |= 2;
	else *(dword *)out &= ~2;
}
// score 319 (was 319): Aligned frame 0x94 agrees; move/input ownership, physics reloads and native velocity guard remain.

// score 319 (was 319): Aligned 0x94 frame agrees; move/input register ownership, physics reloads and velocity null test remain.
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
		byte *component = g_51e9b8->data + (*(long *)(input + 0x18) & 0xffff) * 0xa0;
		byte *body = *(byte **)(*(byte **)(component + 0x70) + 0x40);
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
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		bool active = (*(dword *)(object + 4) & 0x100) != 0;
		if (active) function_b87b0(object_index);
		if (*(long *)(object + 0x14) != NONE ||
			(position.x >= -32768.0f && 32768.0f >= position.x && position.y >= -32768.0f &&
			32768.0f >= position.y && position.z >= -32768.0f && 32768.0f >= position.z))
		{
			*(point3f *)(object + 0x64) = position;
			object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			if (*(long *)(object + 0xd4) != NONE) function_b58c0(*(long *)(object + 0xd4), 2);
		}
		function_1c4a80(object_index, false, false);
		if (active) function_b8600(object_index, 0);
		function_bba20(object_index);
	}
}
