// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "unknown_20fe20.h"
#include "globals.h"
#include <math.h>
#include "vector_products.h"
#include "normalize3d.h"

struct s_28dbd3
{
	long field_0;
	s_type_c3b527 field_4;
	real field_14;
	real field_18;
	long field_1c;
	byte field_20[0x10];
	long field_30;
};

struct s_28dbd0
{
	byte field_0[0x1c];
	point3f field_1c;
	vector3f field_28;
	vector3f field_34;
	vector3f field_40;
	byte field_4c[0x68];
	word field_b4;
	word field_b6;
	byte field_b8[0xc4 - 0xb8];
};

struct s_effect_owner;
struct s_object_ai_data;
struct s_28dab2;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_b8540(long arg_0);
void function_141ce0(real arg_0, real arg_1, real arg_2, transform4x3f *arg_3);
long function_1e06b0(long arg_0);
void swarm_creature_initialize(s_object_ai_data *arg_0);
bool function_28e320(long arg_0, long arg_1);
void swarm_creature_reset_position(long arg_0, s_object_ai_data *arg_1);

__forceinline void function_28dbd2(real arg_0, vector3f *arg_1, vector3f *arg_2)
{
	real local_1 = (real)cos(arg_0);
	real local_2 = dot3f(arg_1, arg_2) * (1.0f - local_1);
	vector3f local_3;
	cross3f(arg_1, arg_2, &local_3);
	real local_0 = (real)sin(arg_0);
	vector3f_scale(arg_1, local_2, arg_1);
	vector3f_scale(&local_3, local_0, &local_3);
	vector3f_scale(arg_2, local_1, arg_2);
	arg_2->i = arg_2->i + arg_1->i + local_3.i;
	arg_2->j = arg_2->j + arg_1->j + local_3.j;
	arg_2->k = arg_2->k + arg_1->k + local_3.k;
}

__forceinline void function_28dbd4(s_type_c3b527 const *arg_0, point3f *arg_1)
{
	if (arg_0->output_index == NONE || !function_2104b0(arg_0->output_index, &arg_0->point, arg_1))
		*arg_1 = arg_0->point;
}

__forceinline s_object_ai_data *function_28dbd5(s_handler_object_view *arg_0)
{
	s_object_ai_data *local_1;
	if (!arg_0->flags134)
		local_1 = (s_object_ai_data *)((byte *)arg_0 + arg_0->ai_offset);
	else
		local_1 = NULL;
	return local_1;
}

// score 173 (was 173): Frame 0x11c versus retail 0x118 (ours spills a rotation cross term to +0x2c
// where retail uses +0x28); the rotation computes the cosine with fcos before fsin (retail sine
// first, stored after the dot) and colours the cross terms differently.
// Earlier 185: Frame 0x11c versus retail 0x118; the request is four bytes
// late, and cross-term/sine/cosine homes differ. Earlier delayed-sine forms 180/209
// recover the frame/frame plus request homes but do not meet the keep gate.
// Round 43: mixed two-scalar cross pairs k/j/i-first 241/232/232; request-forward launch copy 246;
// distinct forward input/local output 235; velocity output with shared/scalar speed scaling 209/209;
// mixed cross with velocity output 240.
// cross before trig 234; alignment before trig 234; shared velocity scale 213; plain rotation component loop
// 234; global optimization off 549; named request word 213; complete creation-stage scope 213; random arm first
// 284; shared rotation output setter 226; size optimization 563; rotation axis input first 213; rotation output
// first 213; native rotation input copy 250; cosine complement lifetime 226; separate creation result 319;
// creation value initialization 353; frame pointer 304; float consistency 512; shared-cross scalar captures 226;
// axis value 293; post-trig alignment 234; launch-speed name 213; tilt name 213; jitter base value 231; two
// offset component loops 479; transform memcpy 213; in-place input scale 260; in-place cross scale 229; both
// scales 232; ordinary rotation inline 213; forced shared cross 213; output-pointer result 213; result plus
// forced cross 213; result plus both scales 232; completed scalar outputs 226; output orders jik/kij/jki/kji/ikj
// 223/195/222/237/216; double angle 216; output-angle-axis helper 213; sine-weighted cross products 226;
// cosine-plus-projection sums 226; axis projection products 226; cosine products 226; rotation-helper names
// 213; cross/projection lexical homes 213; sine/cosine products 227; launch-offset products 213; consumed-axis
// projection 222, with cross scale 222, with direction scale 200; all three shared scales 186 (kept);
// corresponding native loops 230/229/231/235; direct named pooled flag field 186; native vector result
// pointer/pointer, value/pointer, pointer/value, value/value, reference/pointer, pointer/reference,
// reference/reference 296/372/325/409/296/296/296, distinct input/output 296 (frames 0x12c/0x134); distinct
// shared-scale cosine/projection/cross outputs 241/224/233, paired 246/252/225, all three 246, with a native
// component output loop 246; complete axis+direction inputs 324, cosine/projection/turn terms 305/319/324,
// cosine+projection 338, cosine+turn 303, projection+turn 312, all weighted terms 342; paired double trig 244;
// double projection 244; both 244; double launch speed 244; launch/trig 321; launch/projection 316; all three
// 322; existing double velocity scaler 245; consumed-axis shared-scale orders projection/direction/cross 190,
// cross/projection/direction 227, cross/direction/projection 236, direction/projection/cross 248,
// direction/cross/projection 252; early cross 234; early alignment 235; cosine first 227. Frame
// 0x11c versus 0x118 and rotation cross homes remain; original helper and all calls restored.
// Round 175: Consumed-axis reference axis/direction/both 186/186/186;
// axis value with pointer/reference direction 308/308; double trig input with
// pointer/axis-reference/both-reference inputs 188/188/188. Original helper restored.
// Round 145: Cross into consumed axis with preserved native input and six shared-scale
// orders 321/318/300/302/305/305; cross into direction with preserved input 287/282.
// Alias-safe distinct cross inputs retained in every trial; original helper restored.
// Round 177: Consumed-axis complete output by assignment/aggregate 287/287; native returned output with
// pointer/reference axis 297/297; complete scalar/cross role names 186/186; early cross storage
// with original/named scalars 186/186. Frame 0x11c versus 0x118 persists; all original code restored.
// Round 243: Delayed-sine schedules 187/187/180/212/209/212/209; output loop 230; scalar
// trig contracts 227/187/224/191. The 180 form fixes frame only, gains six and is
// rejected; 209 fixes frame and request homes. Original helper and body restored.
// direction/cross/projection 252; early cross 234; early alignment 235; cosine first 227; consumed-axis
// reference axis/direction/both 186/186/186; axis value with pointer/reference direction 308/308; double trig
// input with pointer/axis-reference/both-reference inputs 188/188/188.
// Round 57: retail's rotation (read from its listing) is projection first, then two spilled cross terms and
// outputs `axis * projection + vector * cosine - cross * sine`. Scalar helper forms all give frame 0x114
// (four bytes short): the 0x194a10 form 234, the 0x2551f0 form 237, cross orders jki 234 (both output orders),
// ijk 237, kji 234; a cross3f struct with scalar outputs keeps 0x11c, 213 (both output orders).
// Round 59: ours keeps the vector's zero k in a register through the cross terms (retail reloads it as
// a constant after the dot, so it spills one value fewer); helper locals named by role 186; the
// existing 0x210850 called for the point output 352 (not inlined); zero component stored first 186;
// forward copy before the two draws 256; axis term first in the output sums 186.
// Round 192: reused orientation flags/source index/both 257/186/257; existing pooled-data
// result initialized before its test 186, with flags/source/both 257/186/257; combined metadata
// and conditional getter result 268. Original body/helpers restored; frame 0x11c versus 0x118 remains.
// DL round 156: per-component add/sub 233; two vector passes 225; native sums with
// loop/aggregate input and loop/scalar output 225/229/229/225; parallel scalar sums
// mutable/const 227/227. All increase the frame to 0x120; original helper restored.
// Round 194: aggregate/zero-initialized/CRT-zeroed direction inputs 186/245/263;
// each with delayed sine after direction scale 180/239/258; aggregate/CRT-zeroed
// inputs with cross before projection 209/246. Six-point 180 form restored.
// Round 210: Native heading aggregate/zero-first with forward publication after draws,
// before draws, between draws and after axis: 186/256/251/196 each. Original restored.
// Lane 4 r106 (fast): retail's arms read like pointer helpers (the random arm adds an {r1, r2, 0} offset:
// z is stored back unchanged), but launch through vector3f_scale 186, both offsets through a point
// add-scaled helper 192, both 192, the random offset as a vector added to the point 236 (1220 bytes),
// all three 253, the rotation sum through vector3f_add then the cross subtraction 224.
// Round 241: complete heading through the existing vector setter 228; with delayed sine
// 220, early cross 236, both offset helpers 237, delayed sine/offsets 225, velocity scale
// 228, delayed sine/velocity 220, and all three 219. Original body/helpers restored.
// Round 63: delayed first cross scale 228; scalar crosses on delayed-sine schedules 180/209;
// typed request field 186; own-address zero read 247; direct sums on delayed-sine schedules 216/224;
// inline library forward copy 186. Original helper and body restored.
// Round 262: complete fixed-axis/constructed-point/both values 234/324/352;
// each with two-use source index 234/324/352; readonly axis/point-setter
// publication on the full graph 352/368. Original values, helpers and body restored.
// Round 238: Nested forced dot/scale/dot+scale/dot+cross/scale+cross/all expansion 185 each; all
// with inline/automatic rotation 185/185. Original helper and declarations restored; frame
// 0x11c/0x118 remains.
// Round 250: existing staged outer/rotation/both crosses 226/185/226;
// with sine after axis scale 222/187/222; both staged crosses before projection 233,
// with delayed sine 205 (frame 0x118, first +0xa9). Original helper and body restored; frame 0x11c/0x118 remains.
// Round 424: fused moving/fixed offsets, moving-offset/velocity, fixed-offset/velocity,
// all three component loops 188/188/193/193; with delayed sine 182/182/184/184.
// Three-point best rejected: frame 0x118, request twelve bytes early; original body/helper restored.
// Round 29 (fast): the first difference past the slots is the heading's zero k (ours xmm5 for both cross
// products; retail xmm3 for the first and a memory 0.0 in the rotation). Rotation-helper locals named
// sine/cosine and with projection/cross too, the request declared first, the heading declared before
// its angle draw, the axis declared before the field copy, and the zero k stored again before the
// rotation: identical code (185); the zero k stored in each branch instead of once 241.
// Round 457: initial heading draw in double storage, scalar/aggregate heading,
// original/delayed sine and scalar/shared velocity scale: 185/185/187/187/185/185/187/187.
// Frame remains 0x11c; original heading, helper, shared scales and complete body restored.
// Round 120 (lane 6, fast = plain): the rotation helper's cross dot product reversed 185 (same code);
// the cross taken as axis x vector and added (`cross3f(arg_1, arg_2, ...)`, `+ local_3`) 182; that
// with the axis term first in the sums 182; with the sine computed after the cross 173 (kept: cosine,
// projection, cross, sine); on it the dot reversed 173, the axis term first 173 (same code), the sine
// before the cross 183, the three scales vector first 245 (1231 bytes), the cross first 210 (frame
// 0x118 and the head as retail, 1198 bytes, aligned 162 against 183); the orders sine/cross/cosine
// 238, cross/sine/cosine 192, sine/cosine/cross 238, cross/cosine/sine 239, cosine/cross/sine 239;
// the up-first outer cross negated 260.
// Round 123 (lane 6, fast): every order of the helper's sine S, cosine C, projection P (after C) and cross X,
// with the cross as axis x vector added (a) / vector x axis subtracted (s): aSCPX 182, aSCXP 238, aSXCP 238,
// aCSPX 226, aCSXP 195, aCPSX 183, aCXSP 239, aCXPS 210, aXSCP 192 (cross first with sine first, 1215 bytes),
// aXCSP 239, aXCPS 210; sSCPX 185, sSCXP 238, sSXCP 238, sCSPX 227, sCSXP 234, sCPSX 187, sCPXS 187, sCXSP 239,
// sCXPS 210, sXSCP 233, sXCSP 239, sXCPS 210. Only the CXPS/XCPS orders give frame 0x118 (1198 bytes).
// Round 261: Axis-first added-cross graph: sine after axis/axis+direction scales 173/233; sine-first
// 182; shared final sums with original/late/later sine 233/233/229; complete output with late/later
// sine 288/288. Frame 0x11c versus 0x118 remains; all original code restored.
// Round 114 (m3 lane 7, fast): helper order sine, named dot, cosine, projection, `cross3f(v, axis)` subtracted 226
// (retail's fsin placement, frame 0x120); that with `cross3f(axis, v)` added 225, with cross3f_staged 226, with
// direct per-component sums 226; the kept order with only the dot named before the cosine 226. Retail keeps the
// heading's zero k in a register only through the dot (then a memory 0.0 and an `xorps`); ours keeps it throughout.
// Round 45 (lane 13, fast): sine/cosine passed to a (vector, axis, sine, cosine) helper with scalar
// vector-x-axis cross terms subtracted 229 (frame 0x114, no cross spills); cross order j/k/i with
// the axis term first 229 (same code); cross through cross3f into a local 226 (frame 0x120); separate
// output vector 251; sine/cosine named in the caller 240. Original helper restored.
// Round 40 (m3 lane 15, fast = plain): the 0x11c/0x118 frame is the helper's cross vector home (local_3 at
// +0x24..+0x2f, its .j slot never used), where retail spills scalar cross terms (+0x20, +0x28). Cross scaled by the sine
// in the sums 193 (1225 bytes); cross3f_staged 173 (same code); scaled cross into a second vector 226; vector x axis
// subtracted 187; the 0x194a10 scalar form (sine, cosine, P, X.j, X.k, X.i inline) 231 (1192), with dot3f 228 (1194):
// sine/cosine slots retail's but frame 0x114, heading.k's zero kept in a register and heading.j reloaded from its home.
// Round 23 (lane 18, fast): the matched 0x170d70 rotation form (cross i, projection, cross j, k; rows
// v*cos + projection*axis - cross*sin) with sine then cosine computed by the caller 241 (frame 0x114,
// 1193 bytes: sine first and its +0x24 home as retail, but heading at +0x10, cosine +0x1c, one cross spill);
// that with the cosine declared before the heading or renamed 241 (same code: neither moves its home);
// sine/cosine before the heading copy and outer cross 241; projection, cross j, k, i with every row
// projection*axis + v*cos - cross*sin 241 (same code); the 173 form with three scalar cross terms 246
// (frame 0x124). Diagnostic: the random arm's two draws as an offset vector with zero k (z stored back,
// the -0.5 applied per draw as retail; ours still consumes the draws in the adds) 226 (1220 bytes).
// @retail 0x28dbd0
long function_28dbd0(long arg_0, long arg_1, long arg_2, s_28dab2 const *arg_3, bool arg_4)
{
	long local_1 = NONE;
	s_28dbd3 const *local_4 = (s_28dbd3 const *)arg_3;
	s_28dbd0 local_0;
	function_b7930(&local_0, arg_2, NONE, NULL);
	function_28dbd4(&local_4->field_4, &local_0.field_1c);
	if (local_4->field_1c & 4)
	{
		transform4x3f local_1;
		function_141ce0(local_4->field_14, local_4->field_18, 0.0f, &local_1);
		local_0.field_28 = local_1.forward;
		local_0.field_34 = local_1.up;
	}
	else
	{
		real local_1 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -3.1415927f, 3.1415927f);
		vector3f local_2;
		local_2.i = (real)cos(local_1);
		local_2.j = (real)sin(local_1);
		local_2.k = 0.0f;
		if (local_4->field_1c & 2)
		{
			real local_3 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 2.0f, 3.0f);
			real local_4 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.2617994f, 1.5707964f);
			local_0.field_28 = local_2;
			vector3f local_5;
			cross3f(&local_2, g_4687b0, &local_5);
			function_28dbd2(local_4, &local_5, &local_2);
			local_0.field_1c.x += local_2.i * 0.1f;
			local_0.field_1c.y += local_2.j * 0.1f;
			local_0.field_1c.z += local_2.k * 0.1f;
			local_0.field_1c.x += g_4687b0->i * 0.1f;
			local_0.field_1c.y += g_4687b0->j * 0.1f;
			local_0.field_1c.z += g_4687b0->k * 0.1f;
			local_0.field_40.i = local_2.i * local_3;
			local_0.field_40.j = local_2.j * local_3;
			local_0.field_40.k = local_2.k * local_3;
		}
		else
		{
			if (!arg_4)
			{
				real local_3 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -0.5f, 0.5f);
				real local_4 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -0.5f, 0.5f);
				local_0.field_1c.x += local_3;
				local_0.field_1c.y += local_4;
			}
			local_0.field_28 = local_2;
		}
	}
	if (!local_4->field_30)
		*(long *)((byte *)&local_0 + 0xc) = function_1e06b0(arg_1);
	else
		*(long *)((byte *)&local_0 + 0xc) = local_4->field_30;
	local_0.field_b4 = 0;
	local_0.field_b6 = 0xb0;
	local_1 = function_b7b40(&local_0);
	if (local_1 != NONE)
	{
		s_handler_object_view *local_2 = handler_object_get(local_1);
		s_object_ai_data *local_3 = function_28dbd5(local_2);
		if (local_3)
		{
			swarm_creature_initialize(local_3);
			if (function_28e320(arg_0, local_1))
			{
				swarm_creature_reset_position(local_1, local_3);
				goto local_6;
			}
		}
		function_b8540(local_1);
		local_1 = NONE;
	}
local_6:
	return local_1;
}
