#include "unknown_11c920.h"
#include "frame_storage.h"
#include "effects.h"

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

PRIVATE __forceinline real function_248df1(long arg_0, void *arg_1, void const *const &arg_3, void *arg_2)
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
			return ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_a;
		return 0.f;
	case 10:
		if (local_2->effect_index != NONE)
			return ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_b;
		return 0.f;
	case 11:
		if (local_1) local_0 = function_248ef0(&local_1->field_38);
		break;
	case 12:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			local_3.i = local_1->field_1c.x - g_485618.position.x;
			local_3.j = local_1->field_1c.y - g_485618.position.y;
			local_3.k = local_1->field_1c.z - g_485618.position.z;
			function_4efb0(&local_3);
			local_0 = function_248eff((local_3.k * g_485618.forward.k + local_3.j * g_485618.forward.j + g_485618.forward.i * local_3.i) * local_1->field_38);
		}
		break;
	case 13:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			volatile vector3f local_4;
			local_3.i = local_1->field_1c.x - g_485618.position.x;
			local_3.j = local_1->field_1c.y - g_485618.position.y;
			local_3.k = local_1->field_1c.z - g_485618.position.z;
			local_4.i = g_485618.up.j * g_485618.forward.k - g_485618.up.k * g_485618.forward.j;
			local_4.j = g_485618.up.k * g_485618.forward.i - g_485618.up.i * g_485618.forward.k;
			local_4.k = g_485618.up.i * g_485618.forward.j - g_485618.up.j * g_485618.forward.i;
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

// score 30 (was 30): case 12's projection loads the global factor first in all three products
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
// the fraction as a ternary 210. Round 69: case 12 dot3f in either argument order 70/70;
// cross3f_staged plain 212, with the inherited qualified vector 31, plain with a named projection
// 212; ordinary inline on the private implementation 1074 (17 bytes); case 13 dot3f in either
// argument order 31/31.
// @retail 0x248df0
real function_248df0(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	return function_248df1(arg_0, arg_1, arg_3, arg_2);
}
