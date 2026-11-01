#include "unknown_11c920.h"
#include "normalize3d.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_26c380.h"
#include "unknown_2c0d00.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

struct s_298c30
{
	byte field_0[0x20];
	real field_20;
	real field_24;
};

struct s_298c46
{
	byte field_0;
	bool field_1;
	byte field_2[2];
	real field_4;
	real field_8;
	point2f field_c;
	byte field_14[4];
	short field_18;
	byte field_1a[0xe];
};

// @retail 0x298c30
void function_298c30(long arg_0, s_298c30 const *arg_1, real arg_2,
	real arg_3, s_298c46 *arg_4, point2f const *arg_5)
{
	s_slot_object_view *local_0 = object_get(arg_0);
	bool local_1 = arg_4->field_4 < 0.0f;
	bool local_2 = arg_3 > 0.0;
	real local_3 = 10.0f;
	real local_4;
	if (local_0->type == 1)
	{
		local_4 = *(real *)(g_4e3b44[(*(long *)local_0) & 0xffff].bytes + 0x1f4);
		if (local_4 <= 0.0f)
			local_4 = 15.0f;
	}
	else
		local_4 = 15.0f;
	if ((local_1 || arg_4->field_1) && local_2)
		local_3 /= (arg_3 / local_4) * 0.5f + 1.0f;
	if (local_1 || arg_4->field_1)
		local_3 /= arg_1->field_20 * arg_2 * 0.03333333507180214f + 1.0f;
	if (arg_4->field_1)
		local_3 *= 0.8f;
	local_3 = (real)(local_3 / (fabs(arg_4->field_4) * 0.2f + 1.0f));
	if (arg_5)
		arg_4->field_8 = (arg_4->field_c.y * arg_5->y + arg_4->field_c.x * arg_5->x + 3.0f) * local_3 * 0.25f;
	else
		arg_4->field_8 = local_3;
}

s_298c46 g_5047f8[8];
short g_504938;

__forceinline real function_298df3(point2f *arg_0)
{
	real local_0 = (real)sqrt(arg_0->x * arg_0->x + arg_0->y * arg_0->y);
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->x *= local_1;
		arg_0->y *= local_1;
	}
	return local_0;
}

real function_50650(real cosine);

__forceinline void function_2992ba(short arg_0, bool arg_1, real arg_2, point2f const *arg_3,
	point2f const *arg_4, long arg_5, s_298c30 const *arg_6, real arg_7, real arg_8, point2f const *arg_9, point2f *arg_10)
{
	g_5047f8[arg_0].field_0 = arg_1;
	g_5047f8[arg_0].field_4 = arg_2;
	point2f &local_0 = *arg_10;
	if (arg_2 < 0.0f)
	{
		local_0.x = arg_3->x * -1.0f;
		local_0.y = arg_3->y * -1.0f;
	}
	else
		local_0 = *arg_3;
	g_5047f8[arg_0].field_1 = local_0.y * arg_4->y + local_0.x * arg_4->x < 0.0f;
	g_5047f8[arg_0].field_c = *arg_3;
	function_298c30(arg_5, arg_6, arg_7, arg_8, &g_5047f8[arg_0], arg_9);
	g_5047f8[arg_0].field_18 = arg_0;
}

// score 408 (was 408): Frame 0x64 agrees; entry loads, normalized XY/magnitude homes and 0x18 versus 0x1c cleanup differ.
// Earlier: Frame 0x64 agrees; entry loads, normalization homes and 0x18 versus 0x1c cleanup differ.
// The pointer-to-parameter trick for arg_3 is gone and the score is unchanged; the parameter's own use
// still differs from retail's stack home.
// frame 0x64 agrees; entry loads, parameter cleanup and normalization/trig homes differ.
// Earlier frame 0x68 forms: shared normalizer 433, unsigned count 437, unsigned endpoint 433, both 437, angle end pointer 457.
// Frame 0x64 agrees; entry loads, normalization homes and 0x18 versus 0x1c cleanup differ.
// Earlier shared normalizer 433; unsigned count 437; unsigned endpoint 433; both 437; angle end pointer 457.
// frame now 0x64; entry load schedule, point/magnitude homes
// and candidate arithmetic differ. Byte candidate count 417; byte endpoint 427;
// later center sum 444; early facing result 441; short angle count 433; both byte counts 408.
// Separate trig pairs 408; byte angle countdown 408; retained candidate entry 441;
// angle result from branches 408; separate evaluation scratches 425; general arm first 441.
// Signed byte candidate/endpoint/both 408/408/408; quarter-turn table loop 481;
// short facing result 430. Entry loads and 0x18 versus 0x1c return cleanup remain.
// Helper-owned/returned evaluation 425/425; native endpoint result 443; endpoint value 455;
// ratio value result 408; consuming angle cursor 416; side before distance 408; angle pair initialization 414.
// Native real fixed cosine/sine/pairs 409/430/420; general cosine/sine/pair 408/408/409;
// all cosines 408; all sines 430; all real trig 430. Original double APIs restored.
// On recovered frame: unused/direction/both formal carrier removals 409/408/409; normalized-point
// name/all point homes 408/408; early evaluation/angle workspace 408/408; dead-carrier removal with
// point names 409.
// Existing endpoint helper at negative/positive/both outputs 408/409/409; rounded distance/projection/ratio
// in double 442/444/445; native velocity fields 408; indexed endpoint observations 458. Entry loads,
// XY/magnitude homes and 0x18 versus 0x1c cleanup remain.
// Normalizer input snapshots 448; native normalizer/distance/both roots 443/408/448; snapshots with
// native distance root 448; void normalizer alone/with snapshots/with native roots 408/448/448.
// Original helper and body restored; entry loads, XY/magnitude homes and return cleanup remain.
// Round 341: Persistent endpoint displacement with separate workspace, shared magnitude and word counters 447/465/447/462; existing direction workspace 474/483/474/483. Original body/helpers restored.
// Native normalization-input/evaluation/candidate-direction library copies, pair/all combinations,
// plus native endpoint-array initialization 408/408/408/408/408/408/408/408. Original body and helpers restored.
// Complementary facing bool/byte/short/long 408/408/430/420; with complementary first endpoint
// 442/442/413/411. All restored; incoming object and normalized homes remain.
// Round 138: existing inline vector normalizer at input/direction/both 408/408/408;
// shared endpoint magnitude alone/with input/with direction/with both 407 each
// (frame 0x6c); existing outlined normalizer at both 549. One-point forms rejected;
// original helpers, calls and includes restored. Entry/XY homes and cleanup remain.
// Sum/direction/evaluation point-name order, eight combinations 408.
// Round 240: Shared normalizer factor-first, early uninitialized factor and role names,
// all seven combinations 408; native epsilon-rejection return 408. Internal magnitude/
// reciprocal lifetime forms do not recover entry/XY homes or cleanup; helper restored.
// Round 153 (fast): nullable point through its address (diagnostic) 427; short counters 471, with the point
// through its address 471 (the facing argument moves to EBX); point and facing through their addresses 427
// (ret 0x1c), with short counters 433.
// Round 68: split center around the object lookup with the unused carrier removed 409;
// shared displacement helper 438; direction declared first 409; component direction copy 450;
// entry/both magnitude captures 409/409; object-index address at entry 409 and at all uses 405
// (three-point gain rejected); unused input moved behind the stack inputs at definition/caller 444.
// DH round 234: complete cosine products/all sine products, with reused/separate second
// trig producers 442/408/442/408; those graphs with native word ordinals 471/471/471/472.
// All restored; entry/normalized homes and cleanup differ on the retail-size frame.
// Round 237: arccosine inline declaration, ordinary inline normalizer and ordinary inline
// candidate emitter, all seven nonempty combinations 408 each; all with queued-function
// noinline declaration 408. Original helper bodies and declarations restored;
// matched scoring and the shared-normalizer/caller controls stay 0/749/445.
// Round 240: Direct two-angle array indexing with angle/candidate/endpoint
// byte-byte-byte, short-byte-byte, long-byte-byte, short-short-byte,
// short-byte-short, short-short-short, long-long-byte and long-long-long
// domains: 408/408/409/458/417/471/463/495. Original cursor, counts and body restored.
// Round 251: candidate point/scalar snapshots 401/488; early direction publication 407/459; candidate plus facing snapshots 487/450; early publication with both 487/453. Seven-point best rejected; original helper and body restored.
// The 401 point-snapshot form gives 0x1c cleanup with frame 0x74 at 1733/1732 bytes; restored.
// Round 424: candidate snapshot with helper-owned point/returned point/scalar evaluation
// 445/445/431; each with void normalizer 445/445/431; point/scalar with ordinary inline
// emitter 713/431. All restored; nullable-input residency, XY homes and cleanup remain.
// DL201: optional scoring-input point captured at entry/after projection, by aggregate or
// library copy, direct selection 474/455/474/455; meaningful shared input receiver
// 445/466/445/466. All actual inputs, candidate order, helpers, source fields and
// inherited formal storage restored; entry/XY homes and 0x18/0x1c cleanup remain.
// Round 31 (lane 11): retail keeps the nullable point on the stack because its 16-bit counters
// (loop in bx, index in bp) take EBX/EBP and ESI holds the endpoint pointer; our byte counters
// free ESI, so the point comes in EAX (ret 0x18). Retail also takes the loop's point distance on
// x87 from the centre point's stack home; ours keeps the centre in XMM3/XMM4 (SSE difference,
// two extra homes). Short counters 471 (point back on the stack, facing pointer moves to EBX,
// 1680 bytes, frame 0x68); per-file two-point distance helper with dx/dy locals 411; both 450;
// separate rotated point in the angle loop 430, with short counters 473; the unused first
// parameter's address read removed 409 (and the caller 0x29a190 445 -> 699: LTCG drops the argument).
// Round 246: complete candidate-direction snapshot before publication with
// candidate/endpoint short-byte, word-byte, long-byte, byte-short, byte-word,
// byte-long, short-short and word-word counts 443/443/442/409/409/415/438/438.
// Byte/short gives 1736 bytes and 0x1c cleanup but frame 0x78. Original helper and body restored.
// Round 260: Double endpoint differences with byte-byte/short-byte/word-byte/long-byte/byte-short/byte-
// word/short-short/word-word carriers 437/513/513/510/451/451/515/515. Original distance, counters,
// helpers and body restored.
// Round 479: complete normalizer-square/distance-square/dot product parts,
// masks 1/2/4/3/5/6/7: 407/408/407/407/406/407/406; all plus named
// squared sums 402. One/two/six-point reductions rejected; helpers and body restored.
// Candidate input retained on normalizer/dot, all-part and named-sum graphs
// 405/405/405; named-sum graph with signed-word counters 444. Original input
// observations, helpers, counters and complete body restored.
// @retail 0x298d90
void function_298d90(long arg_0, long arg_1, s_298c30 const *arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, real arg_6, point2f const *arg_7, point2f const *arg_8)
{
	long const *const local_28 = &arg_0;
	long local_29 = *local_28;
	(void)local_29;
	s_slot_object_view *local_0 = object_get(arg_1);
	point2f local_1 = { arg_7->x + arg_8->x, arg_7->y + arg_8->y };
	point2f local_2 = *arg_8;
	function_298df3(&local_2);
	point2f local_27;
	point2f local_3[2];
	real local_30 = -arg_6;
	local_3[0].x = arg_4->x * local_30 + arg_7->x;
	local_3[0].y = arg_4->y * local_30 + arg_7->y;
	local_3[1].x = arg_4->x * arg_6 + arg_7->x;
	local_3[1].y = arg_4->y * arg_6 + arg_7->y;
	bool local_4 = local_2.x * arg_3->x + local_2.y * arg_3->y >= 0.0;
	point2f const *local_31 = (point2f const *)((byte *)local_0 + 0x88);
	real local_5 = local_31->y * arg_3->y + arg_3->x * local_31->x;
	byte local_6 = 0;
	for (byte local_7 = 0; local_7 < 2; local_7++)
	{
		point2f *local_8 = &local_3[local_7];
		real local_9 = (real)sqrt((local_1.x - local_8->x) * (local_1.x - local_8->x) + (local_1.y - local_8->y) * (local_1.y - local_8->y));
		bool local_10 = local_7 == 1;
		if (local_9 < arg_2->field_24)
		{
			real local_11 = (real)cos(1.5707963705062866);
			real local_12 = (real)sin(local_7 == 0 ? -1.5707963705062866 : 1.5707963705062866);
			g_5047f8[local_6].field_4 = 1.5707963705062866f;
			g_5047f8[local_6].field_c.x = local_11 * arg_3->x - local_12 * arg_3->y;
			g_5047f8[local_6].field_c.y = arg_3->y * local_11 + arg_3->x * local_12;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
			local_11 = (real)cos(-1.5707963705062866);
			local_12 = (real)sin(local_7 == 0 ? 1.5707963705062866 : -1.5707963705062866);
			g_5047f8[local_6].field_4 = -1.5707963705062866f;
			g_5047f8[local_6].field_c.x = local_11 * arg_3->x - local_12 * arg_3->y;
			g_5047f8[local_6].field_c.y = arg_3->y * local_11 + arg_3->x * local_12;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
		}
		else
		{
			real local_13 = arg_6 / local_9;
			if (local_13 > 1.0)
				local_13 = 1.0f;
			real local_14 = function_50650(local_13);
			point2f local_15 = { local_1.x - local_8->x, local_1.y - local_8->y };
			function_298df3(&local_15);
			real local_16 = local_15.y * arg_4->y + local_15.x * arg_4->x;
			if (local_16 > 1.0)
				local_16 = 1.0f;
			else if (local_16 < -1.0)
				local_16 = -1.0f;
			real local_17 = function_50650(local_16);
			real local_18;
			if (local_7 != 0)
				local_18 = local_4 ? 3.1415927410125732f - local_17 : local_17 + 3.1415927410125732f;
			else
				local_18 = local_4 ? local_17 : 6.2831854820251465f - local_17;
			real local_19[2];
			local_19[0] = local_18 + local_14;
			if (local_19[0] > 6.2831854820251465f)
				local_19[0] -= 6.2831854820251465f;
			local_19[1] = local_18 - local_14;
			real const *angle_cursor = local_19;
			long angles_remaining = 2;
			do
			{
				real local_21 = *angle_cursor;
				real local_22 = local_21 < 0.0f ? local_21 + 6.2831854820251465f : local_21 - 6.2831854820251465f;
				real local_23 = (real)sin(local_7 == 0 ? -local_22 : local_22);
				real local_24 = (real)cos(local_22);
				local_15.x = local_24 * arg_3->x - local_23 * arg_3->y;
				local_15.y = local_24 * arg_3->y + local_23 * arg_3->x;
				function_2992ba(local_6, local_10, local_21, &local_15, &local_2, arg_1, arg_2, local_13, local_5, arg_5, &local_27);
				local_6++;
				function_2992ba(local_6, local_10, local_22, &local_15, &local_2, arg_1, arg_2, local_13, local_5, arg_5, &local_27);
				local_6++;
				angle_cursor++;
			} while (--angles_remaining);
		}
	}
	g_504938 = local_6;
}

struct s_299460
{
	long field_0;
	long field_4;
	point2f field_8;
	real field_10;
};

struct s_29946c
{
	short field_0;
	short field_2;
	byte field_4[8];
	s_299460 field_c[1];
};

short function_2108a0(long arg_0);

__forceinline s_pathfinding_data *function_29962f()
{
	if (*(long *)((byte *)g_4e0348 + 0xc4) > 0)
		return *(s_pathfinding_data **)((byte *)g_4e0348 + 0xc8);
	return NULL;
}

__forceinline real function_299a36(point3f const *arg_0, point3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_1->x - arg_0->x;
	arg_2->j = arg_1->y - arg_0->y;
	arg_2->k = arg_1->z - arg_0->z;
	real local_0 = (real)sqrt(arg_2->j * arg_2->j + (arg_2->i * arg_2->i + arg_2->k * arg_2->k));
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		vector3f_scale(arg_2, local_1, arg_2);
	}
	else
		local_0 = 0.0f;
	return local_0;
}

__forceinline real function_299ae9(point3f const *arg_0, point3f const *arg_1)
{
	real local_0 = arg_0->x - arg_1->x;
	real local_1 = arg_0->y - arg_1->y;
	real local_2 = arg_0->z - arg_1->z;
	return (real)sqrt(local_0 * local_0 + local_1 * local_1 + local_2 * local_2);
}

__forceinline void function_29991b(point2f const *arg_0, real arg_1, real arg_2,
	point3f const *arg_3, real arg_4, point3f *arg_5)
{
	arg_5->x = arg_0->x * arg_2 - arg_0->y * arg_1 + arg_3->x;
	arg_5->y = arg_0->y * arg_2 + arg_0->x * arg_1 + arg_3->y;
	arg_5->z = arg_4;
}

__forceinline void function_2994a1(point2f const *arg_0, real arg_1,
	point2f const *arg_2, point2f *arg_3)
{
	arg_3->x = arg_1 * arg_2->x + arg_0->x;
	arg_3->y = arg_1 * arg_2->y + arg_0->y;
}

// score 719 (was 719): frame 0x124 versus retail 0x114; radius and trace/pivot homes differ.
// Frame 0x11c versus 0x114, early radius retention and trace/pivot/result homes remain.
// Earlier: frame 0x11c versus 0x114; radius caching and trace/point homes differ.
// Shared endpoint setters 767; center CRT copy 767; step CRT copies 767;
// obstacle-radius observation 813; requested-angle observation 796; all restored.
// Trace-field forward sector 772, reverse 776, both 774; settings radius rereads 812; short-index obstacle cursor 814.
// Frame 0x11c versus 0x114 and radius/trace homes remain. Early settings radius 800; const center 767; early selector 847; final-angle selection first 798; reverse component assignments 767 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain.
// native endpoint return 947; native rotated-point return 961; endpoint input order 767;
// two-coordinate pivot 767; side transform first 822 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. One sector sample 809; cosine first 767; signed step branches 848; normalization result 839; maximum rejection guard 767 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. Absent-obstacle exit 768; positive center clearance 820; direct edge distances 916; early annular bounds 767; reverse-vector component loop 839 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. Selected obstacle pointer 802;
// forward state on hit 771; reverse state on hit 771; both states on hit 784; active edge updates 857;
// in-place initial endpoint 850 (fast).
// All parallel sweep scale terms 749; both endpoint distance carriers 791; scales plus distances 755;
// obstacle owner field rereads 767; separate annular failure guards 767; shared coordinate join 875
// (fast). Frame 0x11c versus 0x114 and radius/trace/pivot homes remain.
// Rotation output reference 767; endpoint output reference 767; endpoint input references 767;
// rotation height-source input 767; endpoint pointer result 767; native settings member 767 (fast).
// Original helper contracts and settings storage restored.
// Native rotation/endpoint/direction results 928/910/805; rotation+endpoint 1050;
// rotation+direction 978; endpoint+direction 966; all 1101; result memcpy 1050. All restored.
// Round 155: actual state-field references, then obstacle-element reference: side 749; angle 840; side angle 840; clearance 793; side clearance 793; angle clearance 845; side angle clearance 845; obstacle element reference 749. All restored.
// rotation-products 749; normalization-products 749.
// distance-products 749; rotation-normalization-products 749.
// rotation-distance-products 749; normalization-distance-products 749.
// rotation-normalization-distance-products 749; obstacle-rotation-products 749.
// Round 141: Native point input, vector input and both 749/749/749; direct actor center 815;
// point/vector/both native inputs with direct center 815 each; readonly actor with native inputs/direct
// center 815. Frame 0x11c versus 0x114 and eager radius remain.
// Round 141: Shared transform/index/both/trace/transform-trace/index-trace/all inline and transform
// noinline 749 each (3383 with transform or index; otherwise 3372). The inline forms recover
// branch-time radius loads but frame 0x11c remains. In that context: ordinary/after-actor/after-sector
// result 778/778/814; state names 749; ordinary state names 778; positive sector/common exit/common
// exit with names 762 each. All headers/providers/body restored.
// Round 180 trace-clear states reverse/forward/center/both edges/reverse-center/forward-center/all/byte
// center 802/802/778/754/771/764/782/778 (fast); all restored. Frame 0x11c/0x114, eager radius and
// trace/pivot homes remain.
// Round 185 obstacle-facing result bool/byte/short/long 749/749/775/777; complementary result
// 749/749/775/777 (fast). All restored; frame 0x11c/0x114 and radius/trace/pivot homes remain.
// Rotation publication xzy/yxz/yzx/zxy/zyx 749/749/748/749/748;
// endpoint yx alone/with yxz rotation 811/811; normalized kij publication 749.
// One-point forms rejected; original helper stores and body restored.
// Round 212: Split transformed output 749; planar pivot 755 (frame 0x114, first +0x14);
// producer scopes 749/755; reverse transforms with full/planar pivot 819/829, scoped 819/829.
// On the planar frame: initial/pivot/both radius joins 870/856/905; ordinary result 801;
// later ordinary result 822; ordinary common exit 753; late radius 826; endpoint inline 755.
// Round 174: shared three-use height 779; forward/reverse/both endpoint calculations at consumption
// 803/798/858; center-first/reverse-first initial points 836/802; reverse-first with late forward 792;
// center-first with late reverse 797. Original geometry production restored; frame 0x11c/0x114,
// eager radius and trace/pivot/point homes remain.
// Split-output planar frame with native trace/sample-point/state role-name groups,
// their pairs/all and reversed trace declarations: 755 each (fast control 749). All restored.
// Separate transform/planar pivot plus all sweep-rotation products, with complete
// normalizer-square/distance-square/obstacle-rotation product phases: all eight 755
// (3351 bytes, frame 0x114, first +0x14). Eager radius remains; all restored.
// Unsigned-word edge pair/center/both/facing/edge-facing/center-facing/all results
// 776/779/776/775/775/790/756; all four native-dword results 752 (fast control 749).
// Frame/radius and trace/pivot homes remain; original result widths and body restored.
// Round 382: maximum-selection presence/absence states as bool/byte/short/long
// 783/783/783/783/778/753/778/770; no exact match. Original ordinal and full body restored.
// Round 175: native state/settings/source-point reference formals, all seven subsets,
// 749 each; all five nonnull object/vector input formals 749. Original pointer
// definition, external declaration and both real calls restored.
// Round 393: Native planar pivot with eight coherent existing rotation-producer role orders:
// 755/755/755/755/755/755/755/755. Original pivot, helper contracts and both transform outputs restored.
// Round 396: Native real root APIs in direction/distance/planar normalizers, seven
// subsets and all with both obstacle roots: eight forms 749 each. All original APIs,
// helper bodies and precision restored; frame, eager radius and trace/pivot homes remain.
// Round 240: Split-output planar frame with shared difference/normalization, settings-radius
// rereads and result-first getter, seven subsets 776/841/788/797/784/835/812;
// full graph plus shared planar difference 808. Original helpers/body restored;
// frame/radius and trace/pivot/result homes remain.
// Round 249: Consistent side/whole-angle/both/bool-side+angle observations 781/767/755/813;
// on the split-output planar frame 756/786/805/806. Original reads, frame and helpers restored.
// Round 217: existing in-place normalized vector scale alone/with shared difference 749/749;
// plus reverse scale 742/742 (3374 bytes, seven-point gain rejected); planar pivot with those
// four forms 755/755/769/769. Frame and eager radius remain; all helpers and body restored.
// Shared sine/cosine/both across sweep and obstacle phases at sweep scope 743/743/748;
// at entry 743/743/748; consistent rotation-pair names at both scopes 748/748.
// Six-point isolated gain rejected. Shared sine/cosine plus existing in-place normalization
// and reverse-vector scale 734/733 fast; cosine form kept 733 plain at 3383/3372.
// Original magnitude tree, real trig rounding, trace calls, values and short loops retained.
// Round 313: Separate transform output, original/ordinary result 733/730; with shared difference
// 733/730. Planar pivot 762/761, with shared difference 762/761. Three-count ordinary-result gain
// rejected; frames 0x124/0x118 still differ from 0x114. Original helpers and body restored.
// Round 239: one actual reused trace context, crossed with independent transform
// output, ordinary result and shared sine: 854/854/826/826/850/850/814/814. Frames
// 0x120/0x124/0x128 versus 0x114; original context reads and complete body restored.
// Round 31 (lane 11): the three extra frame dwords are the sweep sine and the shared cosine (retail
// packs them with the trace distances at +0x28/+0x38) and the obstacle offset difference (retail
// writes it into the direction vector's dead home at +0x18/+0x1c). Sine/cosine in a block with
// a separate obstacle cosine 742 (frame 0x11c); obstacle difference written into local_6 763
// (3372 bytes, frame 0x118); both 772, separate cosines without the block plus that 772 (3351
// bytes, frame 0x114, homes then ordered by reference counts); blocked flags declared before
// the result 733; trace-origin names swapped 733 (no home moves); getter as one ternary 734;
// result-first getter 773 (retail's getter code at all five traces); frame-0x114 form plus the
// result-first getter 799. Entry still hoists the step into XMM5 and the NONE exit is inline.
// Round 452: sweep planar vectors 17/18/19 reused for obstacle direction 742 each;
// also for fallback rotation 740 each; fallback rotation alone through 17/18 738 each.
// All remain above 733; original native workspaces, helpers and complete body restored.
// Round 461: complete conditional admission bool/byte/short/long and exhaustive
// branch results 726 each (seven-point nonmatch gains restored). Conditional
// admission plus ordinary function result 719; direct test plus ordinary result
// 719 (plain kept), direct test with original result 726. Retail length 3372
// recovered; frame grows 0x120 to 0x124, radius and trace/pivot homes remain.
// Round 255: independent normalized sweep-direction output at sweep/iteration scope,
// crossed with separate transformed-forward output and separate phase cosines,
// 763/763/763/763/769/769/769/769. Original output ownership and helpers restored;
// frame 0x124/0x114, eager radius and trace/pivot homes remain.
// Round 476: Native indexed endpoints, paired long/short/word/unsigned-long coordinate loops 782/749/749/782; separate endpoint loops 738/819/819/738.
// All frames remain 0x124 versus 0x114; lengths 3356/3372/3373/3403 do not yield a gain. Original sample producers, complete body and helper contracts restored.
// Round 144 (lane 4, fast; counts are positional, with the mnemonic-aligned count in brackets, base 719 [244]):
// retail computes the reverse endpoint with one `0 - distance` value used for both products (ours folds each
// `i * -d + x` into `x - i * d`), and rotates the obstacle direction in x87 from an unrounded angle (`fld; fchs`
// or `fadd dword [pi]`, then `fld st0; fsin; fxch; fcos`, products consuming cosine and sine in place, each
// component stored as a float and reread by SSE). A named negated reverse distance 739 [244] (3387 bytes);
// a double angle with double sine/cosine for the obstacle rotation 850 [232] (3281 bytes, frame 0x118: the
// offset math then stays in x87), with the sweep cosine moved into its loop 850 [232], with the rotation
// stored field by field 850 [232], written into local_18 844 [232]; plus the existing 2-D point helper for
// the circle point and a difference vector 826 [218] (3313 bytes, frame 0x118, offset math in SSE as retail;
// sine/cosine still popped at the end instead of consumed), the difference subtracted in place 826 [218],
// rotation operands swapped 826 [218] (same code). Next: retail shares slots between sweep and obstacle
// locals (+0x14 angle/best index, +0x28 sine/best ratio, +0x44 reverse offset/rotation, +0x18 direction/
// obstacle difference), so its sweep locals end before the obstacle block.
// @retail 0x299460
bool function_299460(long arg_0, s_298c46 *arg_1, s_298c30 const *arg_2,
	point3f const *arg_3, vector3f const *arg_4, vector3f const *arg_5,
	real arg_6, s_29946c const *arg_7)
{
	bool local_0 = true;
	s_actor_view *local_1 = actor_get(arg_0);
	point2f local_2;
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)arg_3, arg_6, (point2f const *)arg_5, &local_2);
	}
	else
	{
		function_2994a1((point2f const *)arg_3, -arg_6, (point2f const *)arg_5, &local_2);
	}
	real local_3 = *(real *)((byte const *)arg_2 + 0x14);
	short local_4 = function_2108a0(*(long *)((byte *)local_1 + 0x28c));
	point3f *local_5 = (point3f *)((byte *)local_1 + 0x27c);
	vector3f local_6;
	point3f local_7;
	function_210770(local_4, arg_4, (vector3f *)&local_7);
	function_210770(local_4, arg_5, &local_6);
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)local_5, arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	else
	{
		function_2994a1((point2f const *)local_5, -arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	vector3f local_8;
	vector3f_scale(&local_6, -1.0f, &local_8);
	bool local_9 = false;
	bool local_10 = false;
	long local_11 = *(long *)((byte *)local_1 + 0x28c);
	if (local_11 == NONE)
		return true;
	s_path_trace_result local_12, local_13, local_14;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_6, local_3, NULL, &local_12);
	long local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_8, local_3, NULL, &local_13);
	long local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
	point2f local_17, local_18, local_19;
	if (arg_1->field_0)
	{
		real forward_scale = -(arg_6 - local_12.distance);
		real reverse_scale = -(local_13.distance + arg_6);
		real center_scale = -arg_6;
		local_17.x = local_6.i * forward_scale;
		local_17.y = local_6.j * forward_scale;
		local_18.x = local_6.i * reverse_scale;
		local_18.y = local_6.j * reverse_scale;
		local_19.x = local_6.i * center_scale;
		local_19.y = local_6.j * center_scale;
	}
	else
	{
		real forward_scale = (local_12.distance + arg_6);
		real reverse_scale = (arg_6 - local_13.distance);
		real center_scale = arg_6;
		local_17.x = local_6.i * forward_scale;
		local_17.y = local_6.j * forward_scale;
		local_18.x = local_6.i * reverse_scale;
		local_18.y = local_6.j * reverse_scale;
		local_19.x = local_6.i * center_scale;
		local_19.y = local_6.j * center_scale;
	}
	point3f local_20 = *local_5;
	point3f local_21 = {local_6.i * local_12.distance + local_5->x, local_6.j * local_12.distance + local_5->y, local_5->z};
	point3f local_22 = {local_6.i * -local_13.distance + local_5->x, local_6.j * -local_13.distance + local_5->y, local_5->z};
	short local_23 = (short)ceil(fabs(arg_1->field_4) * 1.2732394933700562f);
	real angle_cosine;
	point3f local_24, local_25, local_26;
	for (short local_27 = 1; local_27 <= local_23; local_27++)
	{
		real local_28 = (arg_1->field_4 < 0.0f ? -local_27 : local_27) * 0.7853981852531433f;
		if (local_27 == local_23)
			local_28 = arg_1->field_4;
		if (!arg_1->field_0)
			local_28 *= -1.0f;
		real local_29 = (real)sin(local_28);
		angle_cosine = (real)cos(local_28);
		if (!local_10)
			function_29991b(&local_17, local_29, angle_cosine, &local_7, arg_3->z, &local_24);
		function_29991b(&local_19, local_29, angle_cosine, &local_7, arg_3->z, &local_25);
		if (!local_9)
			function_29991b(&local_18, local_29, angle_cosine, &local_7, arg_3->z, &local_26);
		real local_31 = function_299a36(&local_20, &local_25, &local_6);
		if (!local_10)
		{
			real local_32 = function_299ae9(&local_21, &local_24);
			local_10 = function_26c590(function_29962f(), &local_21, local_15, NONE, &local_6, local_32, NULL, &local_12);
			if (!local_10)
				local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		bool local_33 = function_26c590(function_29962f(), &local_20, local_11, NONE, &local_6, local_31, NULL, &local_14);
		if (!local_33)
			local_11 = ((s_sector_trace_result *)&local_14)->sector_index;
		if (!local_9)
		{
			real local_34 = function_299ae9(&local_22, &local_26);
			local_9 = function_26c590(function_29962f(), &local_22, local_16, NONE, &local_6, local_34, NULL, &local_13);
			if (!local_9)
				local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		if (local_33 || (local_10 && local_9))
		{
			arg_1->field_4 = arg_1->field_0 ? local_28 : -local_28;
			arg_1->field_8 *= (real)(local_27 - 1) / local_23;
			local_0 = false;
			return local_0;
		}
		local_20 = local_25;
		local_21 = local_24;
		local_22 = local_26;
	}
	if (arg_7)
	{
		short local_35 = NONE;
		real local_36 = 0.0f;
		for (short local_37 = 0; local_37 < arg_7->field_2; local_37++)
		{
			s_299460 const *local_38 = &arg_7->field_c[local_37];
			s_slot_object_view *local_39 = object_get(local_38->field_4);
			if (local_39->type == 0)
			{
				long local_40 = *(long *)((byte *)local_39 + 0x12c);
				if (local_40 != NONE && function_1df560(local_1->unknown024, actor_get(local_40)->unknown024))
					continue;
			}
			real local_41 = local_2.x - local_38->field_8.x;
			real local_42 = local_2.y - local_38->field_8.y;
			real local_43 = (real)sqrt(local_41 * local_41 + local_42 * local_42);
			real local_44 = local_38->field_10 + local_3 + arg_6;
			real local_45 = arg_6 - local_3 - local_38->field_10;
			if (!(local_44 > local_43 && local_43 > local_45))
				continue;
			point2f local_46 = { local_38->field_8.x - local_2.x, local_38->field_8.y - local_2.y };
			function_298df3(&local_46);
			bool local_47 = arg_4->j * local_46.y + arg_4->i * local_46.x >= 0.0f;
			real local_48 = local_46.x * arg_5->i + local_46.y * arg_5->j;
			if (local_48 > 1.0)
				local_48 = 1.0f;
			else if (local_48 < -1.0)
				local_48 = -1.0f;
			real local_49 = function_50650(local_48);
			real local_50;
			if (arg_1->field_0)
				local_50 = local_47 ? 3.1415927410125732f - local_49 : local_49 + 3.1415927410125732f;
			else
				local_50 = local_47 ? local_49 : 6.2831854820251465f - local_49;
			real local_51;
			if (arg_1->field_4 > 0.0 ? arg_1->field_4 > local_50 :
				local_50 > arg_1->field_4 + 6.2831854820251465f)
				local_51 = arg_6 > local_43 ? local_43 - local_45 : local_44 - local_43;
			else
			{
				real local_52 = arg_1->field_0 ? arg_1->field_4 + 3.1415927410125732f : -arg_1->field_4;
				real local_53 = (real)sin(local_52);
				angle_cosine = (real)cos(local_52);
				point2f local_55 = { angle_cosine * arg_5->i - local_53 * arg_5->j, angle_cosine * arg_5->j + local_53 * arg_5->i };
				real local_56 = local_55.x * arg_6 + local_2.x - local_38->field_8.x;
				real local_57 = local_55.y * arg_6 + local_2.y - local_38->field_8.y;
				local_51 = local_3 + local_38->field_10 - (real)sqrt(local_57 * local_57 + local_56 * local_56);
				if (!(local_51 > 0.0f))
					continue;
			}
			local_51 /= local_3;
			if (local_35 == NONE || local_51 > local_36)
			{
				local_35 = local_37;
				local_36 = local_51;
			}
		}
		if (local_35 != NONE)
		{
			local_0 = false;
			if (local_36 > 1.0)
				arg_1->field_8 = 0.0f;
			else
				arg_1->field_8 *= 1.0f - local_36;
		}
	}
	return local_0;
}

union vector2f;
real function_11cc90(vector2f const *a, vector2f const *b);
extern point2f *g_468778;
real function_30bf0(vector3f *arg_0);
real normalize2d(point2f *arg_0);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_26c180(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
bool function_1f9240(long arg_0, s_path_settings *arg_1);
void obstacle_list_group(s_obstacle_list *arg_0, real arg_1);
void __stdcall function_2c0d60(vector3f const *arg_0, long arg_1, byte arg_2, s_path_settings const *arg_3,
	s_obstacle_list *arg_4, s_obstacle_list *arg_5, point3f const *arg_6, real arg_7, vector3f const *arg_8,
	long arg_9, long arg_10);
real function_1f99d0(long arg_0, long arg_1, short arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, s_obstacle_list const *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2, vector2f const *arg_3,
	vector2f const *arg_4, vector2f const *arg_5, vector2f const *arg_6, real arg_7, bool arg_8);

__forceinline void function_29a20e(vector3f *arg_0)
{
	real local_0 = (real)sqrt(arg_0->i * arg_0->i + arg_0->j * arg_0->j);
	arg_0->k = 0.0f;
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->i *= local_1;
		arg_0->j *= local_1;
		arg_0->k *= local_1;
	}
}

// score 445 (was 445): Frame 0x580 agrees; scalar/vector homes, planar root scheduling and tail placement differ.
// Earlier score 445 (was 493): the callee's pointer-to-parameter trick is gone, so the call site and the entry
// the callee's pointer-to-parameter trick is gone, so the call site and the entry
// schedule agree with retail; the frame homes still differ (our s_298c30 pointer at esp+0x14 and our
// float at esp+0x10 against retail's esp+0x10 and esp+0x18), and retail keeps the flattened forward
// vector's length on the x87 stack with fst/fabs/fcompi where ours stores and reloads it.
// frame 0x580 agrees; retail duplicates the epilogue into both arms of the final
// clamp and places the failure exit last (ours 82 bytes shorter), keeps the flattened forward vector's
// length on the x87 stack and loads the reference vector's fields before multiplying. Tried: the
// flattened length summed j first 551; the flag cleared before the forward copy 550; the flatten helper
// returning the length 551; the existing normalize3d after clearing k 676 (retail's fst/fabs, but the
// angle dot is one instruction short and every later line shifts); angle dot products local-first 676;
// the failure test after the loop 550; the final clamp as if/else 547; one exit for the final heading
// store 550; the velocity dot with the k term first 550. Round 67: also retail lays the 12-byte
// locals out in declaration order (position 0x3c, the 0x1f9760 output 0x48, the object pointer 0x54,
// the perpendicular 0x58; ours puts the perpendicular first at 0x3c), and keeps each angle-update arm's
// store and the clamp's reloads (ours hoists the angle load above the branch and sinks both stores into
// the clamp). Descriptive names for the four vectors 550 (no slot moved); the perpendicular assigned
// field by field 550; the angle arms as explicit `a = a - d` / `a = d + a` 550; the dead success test
// as break-first 550; the failure statements written at each of the three gotos 583 (2622 bytes, not
// merged).
// Round 448: Planar normalizer complete XY capture / early epsilon rejection / scalar home roles,
// all seven nonempty combinations, then native whole-vector capture 449/445/449/445/449/445/449/670.
// Original body and helpers restored; Frame 0x580 agrees; scalar/vector homes and planar root scheduling remain.
// Round 31 (lane 11): the baseline's two early errors cancel (the fstp/fld normalizer is one
// instruction long, the inlined 0x11cc90 dot one short), so fixing one shifts every later line.
// Flatten then the existing normalize3d, as sibling 0x29cbb0 does, 577 (normalizer exact, dot
// still commuted and folded); the inline normalize copy instead 615; plus a pointer local to the
// global axis 575 (first dot product takes retail's order, the second still folds); that pointer
// declared first 617; settings lookup after the angle 640 (diagnostic); settings/angle names
// swapped 445; angle declared first 445 or beside the flags 445; the two flags declared the other
// way 446; final clamp as if/else 504 (no tail duplication, 2467 bytes); the flatten through a
// two-statement inline helper 577; the axis global declared as the 2-D vector it is defined as 577.
// @retail 0x29a190
void function_29a190(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, real *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	s_298c30 const *local_2 = (s_298c30 const *)function_1e5450(arg_0, *(long *)local_1);
	bool local_3 = false;
	bool local_4 = false;
	if (arg_8 == 0)
		*(real *)(local_0 + 0x674) = *(real *)(local_0 + 0x670);
	vector3f local_5 = *(vector3f *)(local_0 + 0x290);
	*(bool *)(local_0 + 0x6d1) = false;
	function_29a20e(&local_5);
	real local_6 = function_11cc90((vector2f const *)g_468778, (vector2f const *)&local_5);
	if (!*(bool *)(local_0 + 0x5d0) || *(bool *)(local_0 + 0x5d2) || !local_2)
	{
		*(real *)(local_0 + 0x670) = 0.0f;
		*(bool *)(local_0 + 0x678) = false;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
	point3f local_7;
	function_b9dd0(arg_1, &local_7);
	real local_8 = local_2->field_20;
	real local_9 = (real)sqrt(arg_2->i * arg_2->i + arg_2->j * arg_2->j + arg_2->k * arg_2->k);
	function_26c180(arg_0);
	vector3f local_10;
	if (function_1f9760(arg_0, arg_1, &local_7, local_9, &local_4, &local_3, &local_10))
	{
		*arg_7 = true;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
	if (local_3)
	{
		local_10.k = 0.0f;
		if (function_30bf0(&local_10) == 0.0f)
			local_3 = false;
	}
	vector3f local_11 = {-local_5.j, local_5.i, 0.0f};
	s_obstacle_list local_12;
	s_obstacle_list *local_13 = &local_12;
	s_path_settings local_14;
	if (*(bool *)(local_0 + 0x478))
		local_13 = NULL;
	else
	{
		function_1f9240(arg_0, &local_14);
		local_12.group_count = 0;
		local_12.count = 0;
		local_12.flag0_count = 0;
		local_12.flag3_count = 0;
		*(short *)local_12.unknown08 = 0;
		function_2c0d60(0, arg_0, true, &local_14, &local_12, 0, &local_7, local_8 * 3.0f, arg_2, arg_1, NONE);
		obstacle_list_group(&local_12, *(real *)((byte const *)local_2 + 0x14));
	}
	s_298c46 *local_15 = (s_298c46 *)(local_0 + 0x654);
	bool local_16 = true;
	if (!*(bool *)(local_0 + 0x40) && *(bool *)(local_0 + 0x678))
	{
		real local_17 = local_15->field_c.y * local_5.j + local_15->field_c.x * local_5.i;
		if (!(local_17 > 0.0f))
			local_16 = false;
		else if (!(local_17 > 0.93f))
		{
			bool local_18 = local_15->field_c.y * local_11.j + local_15->field_c.x * local_11.i > 0.0f;
			if (local_15->field_0)
				local_16 = (local_15->field_4 > 0.0f && !local_18) || (local_15->field_4 < 0.0f && local_18);
			else
				local_16 = (local_15->field_4 > 0.0f && local_18) || (local_15->field_4 < 0.0f && !local_18);
		}
	}
	if (local_16)
	{
		function_298d90(arg_0, arg_1, local_2, (point2f *)&local_5, (point2f *)&local_11,
			local_3 ? (point2f *)&local_10 : NULL, local_8, (point2f *)&local_7, (point2f const *)arg_2);
		short local_19 = *(short *)(local_0 + 0x66c);
		if (local_19 >= 0 && local_19 < g_504938)
			g_5047f8[local_19].field_8 *= 1.25f;
		short local_20 = 0;
		bool local_21 = false;
		for (;;)
		{
			short local_22 = NONE;
			real local_23 = 0.0f;
			for (short local_24 = 0; local_24 < g_504938; local_24++)
			{
				if (g_5047f8[local_24].field_8 > local_23)
				{
					local_23 = g_5047f8[local_24].field_8;
					local_22 = local_24;
				}
			}
			if (local_22 == NONE)
				goto local_25;
			if (local_20 & (1 << local_22))
			{
				if (!(g_5047f8[local_22].field_8 > 0.0f))
					goto local_25;
				local_21 = true;
			}
			if (function_299460(arg_0, &g_5047f8[local_22], local_2, &local_7,
				&local_5, &local_11, local_8, (s_29946c const *)local_13))
				local_21 = true;
			else
			{
				local_20 |= 1 << local_22;
				if (!local_21)
					continue;
			}
			local_15->field_0 = g_5047f8[local_22].field_0;
			local_15->field_1 = g_5047f8[local_22].field_1;
			local_15->field_4 = g_5047f8[local_22].field_4;
			local_15->field_8 = g_5047f8[local_22].field_8;
			local_15->field_c = g_5047f8[local_22].field_c;
			local_15->field_18 = g_5047f8[local_22].field_18;
			*(bool *)(local_0 + 0x678) = true;
			if (!local_21)
				goto local_25;
			break;
		}
	}
	else
	{
		real local_26 = local_6 - *(real *)(local_0 + 0x668);
		if (local_15->field_0)
			local_15->field_4 -= local_26;
		else
			local_15->field_4 += local_26;
		local_15->field_4 = -3.1415927410125732f > local_15->field_4 ? -3.1415927410125732f :
			(local_15->field_4 > 3.1415927410125732f ? 3.1415927410125732f : local_15->field_4);
	}
	{
		real local_27 = 0.0f;
		if (local_3 && local_15->field_4 > 0.0f && !local_15->field_1)
			local_27 = function_1f99d0(arg_0, arg_1, (short)!local_4,
				(point2f const *)(local_0 + 0x548 + *(char *)(local_0 + 0x53a) * 0x1c),
				(point2f const *)&local_7, (point2f const *)&local_10, local_13);
		real local_28 = *(real *)((byte const *)local_2 + 0x30) * 0.01745329238474369f;
		real local_29 = *(real *)((byte const *)local_2 + 0x38);
		real local_30;
		if (local_15->field_0)
		{
			local_30 = (local_27 + local_15->field_4) * local_29;
			if (local_15->field_4 < 0.0f && local_15->field_1)
				local_30 = -local_28;
		}
		else
		{
			local_30 = (local_27 - local_15->field_4) * local_29;
			if (local_15->field_4 < 0.0f && local_15->field_1)
				local_30 = local_28;
		}
		if (-local_28 > local_30)
			local_30 = -local_28;
		else if (local_30 > local_28)
			local_30 = local_28;
		real local_31 = (real)sin(local_30);
		real local_32 = (real)cos(local_30);
		arg_3->k = 0.0f;
		arg_3->i = local_5.i * local_32 - local_31 * local_5.j;
		arg_3->j = local_31 * local_5.i + local_32 * local_5.j;
		if (function_30bf0(arg_3) == 0.0f)
			*arg_3 = *(vector3f *)(local_0 + 0x290);
		real local_33;
		if (local_15->field_4 >= 0.0f)
		{
			point2f local_34 = {arg_2->i, arg_2->j};
			normalize2d(&local_34);
			local_33 = function_1f9e70(arg_0, arg_1, (point2f *)&local_7, (vector2f *)&local_34,
				(vector2f *)arg_3, (vector2f *)&local_5, local_3 ? (vector2f *)&local_10 : NULL, local_9, false);
		}
		else
			local_33 = -1.0f;
		real local_35 = *(real *)((byte *)local_1 + 0x8c) * local_5.j + *(real *)((byte *)local_1 + 0x90) * local_5.k + *(real *)((byte *)local_1 + 0x88) * local_5.i;
		real local_36 = *(real *)((byte const *)local_2 + 0x68);
		real local_37;
		if (local_36 > 0.0f)
			local_37 = *(real *)((byte const *)local_2 + 0x6c);
		else
		{
			local_37 = 4.019999980926514f;
			local_36 = 6.0f;
		}
		real local_38 = (local_33 - local_35 / local_36) * local_37 + local_33;
		real local_39 = *(real *)((byte const *)local_2 + 0x60);
		if (local_38 > 0.0f && local_39 > 0.0f)
		{
			real local_40 = local_38 - *(real *)(local_0 + 0x674);
			local_38 = *(real *)(local_0 + 0x674) + (local_40 > local_39 ? local_39 : local_40);
		}
		*(real *)(local_0 + 0x670) = 0.0f > local_38 ? 0.0f : local_38;
		*arg_5 = local_38;
		*arg_5 = *arg_5 > *(real *)((byte const *)local_2 + 0x50) ?
			*(real *)((byte const *)local_2 + 0x50) :
			(-*(real *)((byte const *)local_2 + 0x50) > *arg_5 ?
				-*(real *)((byte const *)local_2 + 0x50) : *arg_5);
		if (*arg_5 > 1.0f)
		{
			*arg_5 = 1.0f;
			*arg_6 = false;
			*arg_4 = 0;
			*(real *)(local_0 + 0x668) = local_6;
			return;
		}
		if (-1.0f > *arg_5)
		{
			*arg_5 = -1.0f;
			*arg_6 = false;
			*arg_4 = 0;
			*(real *)(local_0 + 0x668) = local_6;
			return;
		}
		*arg_6 = false;
		*arg_4 = 0;
		*(real *)(local_0 + 0x668) = local_6;
		return;
	}
local_25:
	*(bool *)(local_0 + 0x5d0) = false;
	*(short *)(local_0 + 0x5d6) = 0;
	*(bool *)(local_0 + 0x6d1) = false;
	*arg_6 = true;
	*arg_4 = 0;
	*(real *)(local_0 + 0x668) = local_6;
}
