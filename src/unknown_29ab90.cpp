// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
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
	real local_0 = (real)sqrt(arg_0.k * arg_0.k + arg_0.j * arg_0.j + arg_0.i * arg_0.i);
	return fabs(local_0) < 0.0001f ? 0.0f : local_0;
}
PRIVATE __forceinline real function_29ab92(vector2f *arg_0)
{
	real local_0 = magnitude2d(arg_0);
	if (!(fabs(local_0) < 0.0001f))
	{
		real local_1 = 1.0f / local_0;
		arg_0->i = local_1 * arg_0->i;
		arg_0->j = arg_0->j * local_1;
	}
	else
		local_0 = 0.0f;
	return local_0;
}
PRIVATE __forceinline real function_29ab93(real arg_0, real arg_1, real arg_2)
{
	return arg_1 > arg_0 ? arg_1 : arg_0 > arg_2 ? arg_2 : arg_0;
}
PRIVATE __forceinline void function_29ab94(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
	arg_2->i = arg_0->i * arg_1;
	arg_2->j = arg_0->j * arg_1;
	arg_2->k = arg_0->k * arg_1;
}

// score 383 (was 383): frame 0x5c versus 0x64, actor/input transport and x87 homes differ.
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
			function_29ab94(local_3, 2.0f, arg_2);
		bool local_10;
		bool local_11;
		vector3f local_12;
		if (function_1f9760(arg_0, arg_1, &local_7, local_9, &local_10, &local_11, &local_12))
		{
			*arg_7 = true;
			function_29ab94(local_3, 2.0f, arg_2);
		}
		function_26c180(arg_0);
		vector2f local_13 = { arg_2->i, arg_2->j };
		real local_14 = function_29ab92(&local_13);
		vector2f local_15 = { local_3->i, local_3->j };
		function_29ab92(&local_15);
		local_4 = *arg_2;
		real local_16 = 1.0f;
		real local_17;
		function_2922b0(arg_0, arg_1, &local_4, &local_6, &local_16, &local_17, true);
		if (function_30bf0(&local_4) == 0.0f) local_4 = *local_3;
		real local_18;
		if (*(real *)(local_0 + 0x47c) != 0.0f)
			local_18 = *(real *)(local_0 + 0x47c);
		else
		{
			real local_19 = local_14 * local_14;
			if (!(local_19 > 0.001f)) local_19 = 0.001f;
			real local_20 = local_2->field_1c * 0.5f;
			real numerator = local_20 * local_20;
			local_20 = numerator / local_19;
			real local_21 = local_13.i * local_15.i + local_13.j * local_15.j;
			if (!(local_21 > 0.0f)) local_21 = 0.0f;
			real turn_weight = (1.0f - local_21) * 0.5f;
			real scaled_ratio = local_20 * 0.5f;
			local_18 = turn_weight * scaled_ratio * 4.0f;
		}
		real local_19 = local_2->field_4c > 0.05f ? local_2->field_4c : 0.05f;
		local_18 = function_29ab93(local_18, local_19, 1.0f);
		if (!(local_18 > local_17)) local_18 = local_17;
		vector2f local_20 = { local_4.i, local_4.j };
		function_29b2e0(arg_0, arg_1, &local_4, &local_4, local_18, &local_4);
		*(real *)(local_0 + 0x668) = local_18;
		function_29ab92(&local_20);
		local_18 = function_1f9e70(arg_0, arg_1, (point2f const *)&local_7,
			&local_13, &local_15, &local_20, NULL, local_9, false) * local_2->field_50;
		*arg_6 = false;
		if (local_18 > local_16) local_18 = local_16;
		vector3f local_22;
		local_22.i = local_18;
		local_22.j = local_18 * 0.0f;
		local_22.k = local_18 * 0.0f;
		if (local_2->field_68 > 0.0f)
		{
			vector3f const *local_23 = &local_1->velocity;
			real local_24 = local_23->k * local_3->k + local_23->j * local_3->j + local_23->i * local_3->i;
			real local_25 = local_18 - local_24 / local_2->field_68;
			long local_26 = local_25 < 0.0f ? -1 : 1;
			local_22.i = (real)(sqrt(fabs((double)local_25)) * local_2->field_6c * local_26 + local_22.i);
		}
		real history_weight = local_2->field_64;
		local_5.i = (local_6.i + local_22.i) * (1.0f - history_weight) + *(real *)(local_0 + 0x678) * history_weight;
		local_5.j = (local_6.j + local_22.j) * (1.0f - history_weight) + *(real *)(local_0 + 0x67c) * history_weight;
		local_5.k = (local_6.k + local_22.k) * (1.0f - history_weight) + *(real *)(local_0 + 0x680) * history_weight;
		local_5.i = function_29ab93(local_5.i, -1.0f, 1.0f);
		local_5.j = function_29ab93(local_5.j, -1.0f, 1.0f);
		local_5.k = function_29ab93(local_5.k, -1.0f, 1.0f);
		*(short *)(local_0 + 0x656) = NONE;
	}
	*(vector3f *)(local_0 + 0x66c) = local_5;
	*arg_4 = 0;
	*arg_3 = local_4;
	*arg_5 = local_5;
}
