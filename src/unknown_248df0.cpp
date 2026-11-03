#include "unknown_11c920.h"
#include "frame_storage.h"
#include "effects.h"
#include "vector_products.h"

// @flags /O2 /arch:SSE /Gr

struct s_248df0
{
	byte field_00[8];
	real field_08;
	real field_0c;
	real field_10;
	word field_14[4];
	point3f field_1c;
	byte field_28[0x38 - 0x28];
	real field_38;
};


inline vector3f *function_4efb0(vector3f *arg_0);

PRIVATE __forceinline real function_248eff(real arg_0)
{
	real local_0 = arg_0 - (real)real_truncate(arg_0);
	if (local_0 < 0.f)
		local_0 += 1.f;
	return local_0;
}

PRIVATE __forceinline real function_248ef0(real const *arg_0)
{
	long local_0 = real_truncate(*arg_0);
	real local_1 = *arg_0 - (real)local_0;
	if (local_1 < 0.f)
		local_1 += 1.f;
	return local_1;
}

PRIVATE __forceinline real function_248df1(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	real local_0 = 0.f;
	s_248df0 const *local_1 = (s_248df0 const *)arg_3;
	s_particle_system_datum *local_2 = (s_particle_system_datum *)arg_1;
	switch (arg_0)
	{
	case 0:
		if (local_1) local_0 = local_1->field_08;
		break;
	case 1:
		if (local_1) local_0 = local_1->field_10;
		break;
	case 2:
	case 3:
		if (local_1) local_0 = local_1->field_14[arg_0 - 2] * (1.f / 65535.f);
		break;
	case 14:
	case 15:
		if (local_1) local_0 = local_1->field_14[arg_0 - 12] * (1.f / 65535.f);
		break;
	case 4:
		if (local_2) local_0 = local_2->unknown04;
		break;
	case 5:
	case 6:
		if (local_2) local_0 = ((real *)local_2)[arg_0 + 5];
		break;
	case 7:
		if (arg_2) local_0 = *(real *)((byte *)arg_2 + 0x1c);
		break;
	case 8:
		local_0 = (real)g_510c54->game_time * g_510c54->rate;
		break;
	case 9:
		if (local_2->effect_index != NONE)
			local_0 = ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_a;
		else
			return 0.f;
		break;
	case 10:
		if (local_2->effect_index != NONE)
			local_0 = ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_b;
		else
			return 0.f;
		break;
	case 11:
		if (local_1) local_0 = function_248ef0(&local_1->field_38);
		break;
	case 12:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			vector3d_from_points3d(&g_485618.position, &local_1->field_1c, &local_3);
			function_4efb0(&local_3);
			local_0 = function_248eff(dot3f(&local_3, &g_485618.forward) * local_1->field_38);
		}
		break;
	case 13:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			vector3f local_4;
			local_3.i = local_1->field_1c.x - g_485618.position.x;
			local_3.j = local_1->field_1c.y - g_485618.position.y;
			local_3.k = local_1->field_1c.z - g_485618.position.z;
			cross3f(&g_485618.up, &g_485618.forward, &local_4);
			function_4efb0(&local_3);
			local_0 = function_248eff((local_4.k * local_3.k + local_4.j * local_3.j + local_4.i * local_3.i) * local_1->field_38);
		}
		break;
	case 16:
		if (arg_2) local_0 = *((byte *)arg_2 + 3) * (1.f / 255.f);
		break;
	}
	return local_0;
}

// score 2 (was 30): case 12's dot evaluates its terms k, i, j; retail k, j, i (operand sides follow the position).
// Earlier at 30: case 12's projection loads the global factor first in all three products
// (retail: the stack component first for k and j, the global for i); case 13's cross-product
// XMM roles and scheduling differ. The volatile cross vector keeps case 13 apart: a plain one
// (or any helper writing it) makes case 13 exact but moves the result to XMM1 and lets case 13
// cross-jump into an earlier tail (212). Tried: separate i/j/k product pairs 30/30/30; all
// product pairs 30; positive/negative products first 30/30; scoped negative products 30;
// cross-vector initializer 31; projection operands exchanged 30; cross factors exchanged 30;
// both operand exchanges 30; cross.i subtrahend factors swapped 30 (same code); forward
// snapshot 151; ordinary cross output 212; shared cross helper with axis snapshots 264; axis
// snapshots with existing stores 174; descriptive vector names 30; vectors before switch 30;
// shared point difference 184; named projections 83; complete displacement staging in 12/13/
// both 39/208/208; destructive projected components in 12/13/both 99/212/218; staged 13 with
// component products 212; staged 12 with both product sets 218; direct definition 119;
// cross-first lexical homes 119; direction-first lexical homes 119; cross declared first 119;
// pointer fractions with existing boundary 30; pointer fraction in first/second/both
// projections 119/119/119; partial projection sums 30/30/30, both sums 214; cross component
// orders 34/34/31/31/34; cross before displacement/after normalization 45/42; returned
// direction at three uses 77/74; plain cross vector 212, with cross3f 212, with scale-first
// products or the pointer fraction in case 12/13 212/212/212/212; typed source, system, both,
// readonly system/both and direct source/system/both inputs all 30; the particle file's
// (up, forward) cross helper for case 13 212, computed before the point difference 211, its
// vector declared first 212; vector3d_from_points3d for case 12's / both cases' difference
// 31/31. Earlier fraction, case-tail, helper-parameter and case-order forms reached 121
// before later fixes. Round 29 (lane 14): a compile-only screen reproduces case 12 exactly:
// the address-taken direction is always the memory operand in every written-dot form; only
// vector3d_from_points3d with dot3f loads it first (i, k, j order) 69; a plain cross vector
// moves the result to XMM1 (also in the compile-only screen): with case 13 returning its
// fraction directly 152, cases 12 and 13 both 162, the fraction's `+ 1` returned early 210,
// the fraction as a ternary 210.
// Round 57 (m3 lane 1): case 12's roles follow the count of initialized locals before its
// displacement stores (17-18 more with an i, j, k dot give retail's case 12, 26; nested
// helper locals and bare declarations don't count); case 13's cross registers follow the first
// mention of each camera field in the function (up.i named before case 12 gives retail's
// registers, 71 at 880 bytes); no written cross order can give those ranks. i, j, k dot 30;
// ticks-to-seconds helper for case 8 30; named effect index for cases 9/10 30; dot3f in
// case 12 70; the first three together 30.
// the fraction as a ternary 210. Round 69: case 12 dot3f in either argument order 70/70;
// cross3f_staged plain 212, with the inherited qualified vector 31, plain with a named projection
// 212; ordinary inline on the private implementation 1074 (17 bytes); case 13 dot3f in either
// argument order 31/31.
// Round 60: the volatile cross vector is gone; a plain vector with `cross3f`, case 12 through `vector3d_from_points3d`
// and `dot3f`, and cases 9/10 as `if (valid) result = scale; else return 0.f; break;` reach 2; on those, case 13's
// displacement and dot through helpers too 3 (typed wrapper parameters also 3); `dot3f` in case 13 either order 3/3;
// helper displacement with written dot in 13 66; case 12's displacement written 4, also with dot3f in 13 8; helper in 13 and
// written in 12 68, with dot3f in 13 8; `cross3f_staged` 2; dot3f arguments exchanged 2; written case 12 dot in
// orders k,j,i i,j,k j,k,i i,k,j k,i,j j,i,k 68/67/68/67/68/68; a named projection or scale 2; a forward pointer 10; a by-value vector copy 139/149; the normalize's
// returned pointer as the dot argument 54; a shared projection helper for both cases or case 12 alone 313/247, a shared
// direction helper (dot3f either order, written dot) 314/314/306, its body written out 314/306, in case 12 alone 247/247; a direct
// definition without the wrapper 93; vectors at function scope 3; vector names 3; case 13 before case 12 77; a ticks helper
// for case 8 2. Diagnostic, not keepable: a pointer or reference to the whole local at the displacement and dot gives
// k, j, i with the j term's sides still reversed, 2. Route to 2: named `forward_i`/`up_i` after cross.i on the plain
// vector made case 13 exact (4 with those cases 9/10), and the helpers then fixed case 12's sides. Before that: forward
// pointer in 12 70; forward and up pointers in 13 30; dying-operand product order 30; 18 named one-use locals 26.
// A 600 s permuter run on the final form, none below 2.
// N334: Complete nullable source/system result assignments and projection admission, subsets 1-7,
// 108/156/156/147/112/150/150; every result arm with an explicit default and no entry zero 207.
// Original switch, helpers and full body restored; frame 0x1c and case-12 i/j schedule remain.
// AC492: On the plain-vector/helper graph, case-9/case-10/both retained effect indices
// 2/2/2; first/second/both native word-channel inputs 2/2/2; case-11 value fraction
// 155; complete immutable remainder/result arms 164. Original whole body and helpers restored;
// case-12 i/j projection scheduling remains. One-use field carriers NOT kept without a match.
// Round 133: force-inline dot/displacement declarations 2/2; two-use time pointer 2; shared
// projection accumulator 2; effect-table pointer in case 9/10/both 2/2/2 (NOT kept: one-use
// locals without a match); explicit fraction truncation 2; result declared last/middle 2/2.
// Opening helper graph and complete body restored; the same two case-12 operands differ.
// @retail 0x248df0
real function_248df0(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	return function_248df1(arg_0, arg_1, arg_2, arg_3);
}
