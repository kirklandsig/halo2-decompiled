// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "object_markers.h"
#include "normalize3d.h"
#include "vector_products.h"

struct s_28e770
{
	byte field_0[0x1c];
	long field_1c;
	byte field_20[0x4a4 - 0x20];
	char field_4a4;
	char field_4a5;
	byte field_4a6[2];
	long field_4a8;
	byte field_4ac[0x888 - 0x4ac];
};

struct s_28e771
{
	byte field_0[8];
	point3f field_8;
	byte field_14[0x10];
	point3f field_24;
	bool field_30;
	byte field_31[3];
};

struct s_28e600
{
	word field_0;
	byte field_2[0x12];
	point3f field_14;
	long field_20;
	short field_24;
	short field_26;
	byte field_28[0x24];
	short field_4c;
	byte field_4e[2];
	long field_50;
	short field_54;
	bool field_56;
	bool field_57;
	point3f field_58;
	byte field_64[4];
	real field_68;
	byte field_6c[8];
	real field_74;
	byte field_78[8];
	vector3f field_80;
	vector3f field_8c;
	vector3f field_98;
};

struct s_28e772
{
	byte field_0[0x14];
	long field_14;
	byte field_18[0x58];
	vector3f field_70;
	vector3f field_7c;
	byte field_88[0xa4];
	word : 5;
	word field_12c : 1;
	word : 10;
	byte field_12e[0x4e];
	byte field_17c;
	byte field_17d[0x73];
	vector3f field_1f0;
};

struct s_28e773
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
	real field_10;
};

struct s_object_ai_data;
struct s_28f0e0;
void *function_1e4b50(long arg_0);
void swarm_creature_update_countdown(long arg_0, s_object_ai_data *arg_1);
bool swarm_target_get_location(long arg_0, short arg_1, long *arg_2, point3f *arg_3, vector3f *arg_4);
real function_30bf0(vector3f *arg_0);
bool function_2900b0(long arg_0, point3f const *arg_1, real arg_2, real arg_3, vector3f *arg_4);
long function_1469f0(real arg_0);
byte function_118f70(long arg_0);
void function_b9a90(long arg_0);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
bool function_28f0e0(long arg_2, s_28f0e0 const *arg_1, vector3f const *arg_0, long arg_3);
bool function_28f290(long arg_0, vector3f *arg_1, vector3f *arg_2);
void function_28e600(long arg_0, s_28e600 *arg_1);
vector3f *g_4687b4;

__forceinline void function_28e774(point3f const *arg_0, point3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_0->x - arg_1->x;
	arg_2->j = arg_0->y - arg_1->y;
	arg_2->k = arg_0->z - arg_1->z;
}

// score 4 (was 4): 2416 bytes as retail; left are the alignment dot's i product (retail
// local_14->i * field_80.i) and the case-four y sum (retail placed_point.y first); until round 15 also
// case five's facing dot (retail sums j, i, k; ours j, k, i once case six adds through vector3f_add). At 68 case six's i
// add also differed (retail loads field_80.i and stores it at once; ours folded `addss xmm1, [edi]`
// and stored late, which shifted the rest of case six): retail reads and stores the vector through
// two pointers the compiler cannot prove equal, so no store sinks.
// Lane 15 r19: case five with early breaks so both clears reach one NONE store 110 -> 80 (kept);
// case zero's own block-local vector plus a facing pointer used for the dot and the k test 80 -> 68
// (kept; the facing dot and case six's preloads become retail's); the else branch's negation through
// the existing vector3f_scale 68 (identical code, kept). On 80: local_29 declared before local_14 82
// (the compile-only harness shows retail's alignment dot, the real build flips all three products);
// local_14 removed 82; facing pointer alone 76, as a reference 76; case zero's vector alone 74; case
// five's own vector 80; case six's own vector 124; case-five point at the case top 80; contribution at
// the case top 80; all three per-case vectors 74. On 68: contribution through vector3f_add (either
// order) 70/70, through vector3f_add_scaled 105 (2400 bytes); contribution at function scope in four
// places 68/68/68/68; case four's or case one/two's own offset vector 68/68/68; contribution adds or
// the alignment dot through local_3->field_80 (diagnostic) 75/72; direction scaled in place 78, with
// the negation helper 78; function-scope declaration orders 68/68/68/68; local_29, local_12/local_14
// or both declared ahead of the perception lookup 472/465/513/521.
// At 110: the case-five clear join (retail reloads ESI ahead of one shared NONE store, ours keeps
// two), the last alignment product's operands, the case-four y sum and facing dot orders, and the
// case-six contribution (retail preloads direction.j) remained.
// Lane 15 r17: the case-six scale factor written at its one use (no local) 118 -> 110 (kept; the
// length becomes retail's); the alignment test through the existing dot3f 110 (kept: it replaces the
// hand-written dot, same count; either argument order the same code); a goto from the facing
// rejection into the else branch's clear (diagnostic, not kept) 76 before that change: what is left
// then is the four operand/sum orders and the contribution block; case-four y sum with its operands
// swapped: identical code (VC orders them itself); contribution through vector3f_add_scaled 141,
// direction scaled in place 142, contribution declared first 110 (same code).
// At 118: staged alignment 136 plain, direct motion 235f, direct forward 253f, double-factor scaling 190f, signed-byte mode 504f.
// Round 21: the case-five clear join is the 4-byte length difference (retail reloads esi ahead of one
// shared NONE store; ours keeps two stores with different joins). Case-six forms leave the case 1/2,
// 4 and 5 sum orders as they are (one product fewer 145f, two more 168f): no function-wide rotation.
// Negating the first -1 product 105f (2416 bytes, but retail multiplies there, not kept); `2 - x * (2/3)`
// 145f; `/ 3.0f` 118f; contribution through the existing vector3f_add 141f, through vector3f_add_scaled
// 139f; case-five dot3f arguments swapped 118f.
// Tried (MiniMax B run 19): the case-four NONE store reached by a goto into the else branch's
// clear 76; not kept (a goto is kept only with a byte match).
// At 138: alignment/clear-join/contribution schedules remain; fast preview 144.
// Read-only forward 144f; alignment snapshot 414f; named alignment 144f;
// marker failure break 185f; in-place contribution 204f; scale result 149f;
// aggregate sum 210f; shared retreat scale 144f.
// At 118: alignment product, facing dot, clear join and contribution schedules remain;
// named target distance 119 plain / 122 fast; motion memcpy 142f;
// point-getter result in case four/five/both 281/195/287f; failure-first arm 179f;
// output reuse 192f; scale force-inline 142f; distance names / placement capture 122/122f.
// At 121: alignment/facing products, marker-clear join and contribution schedule remain.
// Fast: motion reference 125; forward reference 125; both vector references 125;
// perception reference 125; limits reference 157; perception/motion references 125;
// limits/motion references 157; object reference 125.
// alignment operands, marker clear join and contribution schedule remain;
// long marker count 206f; case-assigned short/long count 129/206f;
// prior motion across scale 203f; elapsed fraction 129f; clamped elapsed 141f;
// mode/all inputs assigned by arms 536/552f; unchanged preview 129f.
// At 118: alignment operands, marker clear join and contribution schedule remain.
// In-place direction scale 124f; contribution-first adds 124f; both 124f;
// separation/combined length results 124/124f; both 124f; drift distance 124f;
// normalization results with in-place scale 124f; preview 124f, all 103 code differences.
// At 118: alignment/facing products, clear join and contribution schedule remain.
// Genuine alignment i/j/k products 125/129/125f; ij/ik/jk products 129/125/125f;
// all products and products with complete result 125/125f; preview 125f.
// alignment operands, marker-clear join and contribution schedule remain;
// byte busy result / elapsed consumer / factor accumulator / contribution-first sums / combined 124/124/124/124/124f;
// marker comparison result 124f; terminal marker case / comparison 206/206f.
// At 118: at aligned 21 the 4-byte length comes from case five's two field_50 = NONE
// stores (retail reloads esi ahead of one shared store) and case six's first contribution term.
// dot3f for the alignment test (either order) 118; case-four sum order 118; failure-first
// case-five arm 198; `x = x + c` contribution adds 118; contribution scaled into direction 118;
// the dot test as two NONE-storing ifs 234.
// Round 242: shared/native displacement with repeated observations 129/118;
// with normalization reuse 186/183; fused contribution plus these four forms 150/139/214/211.
// All native displacement forms restored; alignment, facing sum, clear join and contribution remain.
// Round 244: shared add after scale 141, with native point publication 141;
// direct scale-return/add pipeline 141, with point publication 141; reverse add inputs
// 141/141; native point publication alone 118; planar vector/point constructors with
// pipeline 143. All restored; marker-clear receiver join and contribution schedule remain.
// Round 247: complete published-motion observation, with facing/forward/both 410/448/443/476; complete candidate-motion observation, with facing/forward/both 417/450/464/481. Each worsens the matching prefix; marker-clear join and contribution schedule remain.
// Round 250: complete case orders 12034567/12304567/12340567/12345067
// 277/329/398/454; 12345607/12345670/40123567/41230567
// 475/479/424/420. All complete cases and original order restored; idle relocation breaks the prefix.
// Round 253: Existing native object getter with independent idle/marker/separation direction
// workspaces, all eight subsets 118/118/118/118/157/118/118/118. Seven forms retain 103 code
// differences; separation-only gives 142. Both callers survive, all tables unchanged. Original
// getter, common workspace and complete cases restored.
// Round 255: marker-facing rejection direct orders 113/121/113; immediate bool results in original/OR/AND/both orders 118/113/121/113; long result 118. The 113 forms reach 2416 bytes but gain only five and worsen the code window 103 -> 110. All restored.
// Round 191: actual source-direction alignment 362; complete scalar contribution sums 118;
// combined 362; source/facing ownership 435, with sums 435; source/direct admission 362,
// with sums 362, all forms 435. All source/member substitutions were exact-match-only;
// all trial bodies restored. Original vectors, cases, call tuples and expression trees retained.
// Round 93 (lane 8): the field written directly everywhere (no local_29) with case six's add as
// vector3f_add(&field_80, &contribution, &field_80) 68 -> 8 (kept: case six becomes retail's, the facing
// dot's sum order flips); with contribution at the switch scope or declared before the direction test
// 8/8; the helper's first two arguments swapped 8; case five's facing pointer removed 10. On 68: case
// six reading through a second pointer to the field and storing through local_29 4 (all of case six
// retail's; not kept: a second pointer that renames local_29); reads through the field and stores
// through local_29 (scalar) 32 (j, k read as [ebp + 0x84]); stores through the field, reads through
// local_29 64; vector3f_add(local_29, &contribution, local_29) 70; vector3f_add(local_29, &contribution,
// &field_80) 70; vector3f_add(&field_80 or &contribution first, ..., local_29) 6/6 (facing dot flips);
// case four's offset through function_28e774 then k = 0 70 (the x sum flips too); case four's sum through
// vector3f_add 72. On the 4 form: case four's offset through vector2d_from_points2d 6 (x flips); its j
// statement first 10; its k store first 12, between i and j 9; its sum's y statement first 8; all sums
// written placed-first or offset-first 4/4; the alignment dot's arguments swapped 4; the dot through
// &local_12->field_70 6 (j and k flip, i becomes retail's); the dot through &field_80 4; through
// &local_13 394; case one/two's copy as a direct field store 8 (the facing dot flips).
// Round 15 (m3 lane 2): case four's sum through the existing vector3f_add(placed point, offset) 8 -> 4
// (kept; the facing dot becomes retail's). On 4: offset through vector2d_from_points2d 4 and through
// function_28e774 with k = 0 4 (y becomes retail's, x flips); vector3f_add arguments swapped 4; the
// alignment dot through &local_12->field_70 8; a named alignment local 6. Compile-only screens (exact
// for these three spots): the spots follow the function's live-symbol count (extra used locals flip
// them), not declaration order (24 orders), dot argument order, const or the top copy's spelling; no
// offset/sum helper combination gives retail's x (offset first) with retail's y (placed first).
// Round 131 (lane 4) on 8: alignment dot3f(local_14, &field_80) 8 (same code); dot3f(&field_80,
// &local_12->field_70) 74; dot3f(&local_12->field_70, &field_80) 68; facing dot3f(&direction, facing)
// 8 (same code: VC orders the facing sum itself); one placed point declared ahead of local_13 at the
// switch scope for cases four and five 43; permuter (4 tries) best 6 only by a volatile bool for
// the location result (a steering volatile, not applied).
// Also on 8: case four's placed point declared before its function_b9a90 call 8 (same code); local_14
// as `vector3f const *` 8 (same code).
// Round 17 (m3 lane 2) on 4: the alignment dot's i order follows how many symbols are created
// before local_14 (code-neutral screens: one fewer there, none fewer after, gives retail's); the
// case-four sums follow field creation order (a statement creates its right side's fields first,
// inline helpers their left side first; the newer operand loads first), so retail's x and y need
// placed_point.x read before the offset's i exists and local_13.j created before placed_point.y.
// local_14 taken from local_1 before local_12 2 (only the y sum left); one typed pointer for
// local_1/local_12 66; that plus a case-seven velocity pointer 2; local_14 first with the offset
// through function_28e774 then k = 0 2 (y right, x flips); that with the sum written out (x
// placed-first, y offset-first) 4 (the facing dot flips); local_12 dropped with casts at its uses
// 66, with that offset 6.
// @retail 0x28e770
void __stdcall function_28e770(long arg_0, long arg_1)
{
	s_28e770 *local_0 = &((s_28e770 *)g_4f55f0->data)[arg_0 & 0xffff];
	s_handler_object_view *local_1 = handler_object_get(arg_1);
	s_28e773 *local_2 = (s_28e773 *)function_1e4b50(arg_0);
	if (!local_1->flags134)
	{
		s_28e600 *local_3 = (s_28e600 *)((byte *)local_1 + local_1->ai_offset);
		if (local_3 && local_2)
		{
			s_28e771 *local_4 = (s_28e771 *)perception_get(local_0->field_1c);
			swarm_creature_update_countdown(arg_1, (s_object_ai_data *)local_3);
			short local_5 = local_0->field_4a4;
			short local_6 = local_0->field_4a5;
			long local_7 = local_0->field_4a8;
			if (local_3->field_0 & 1)
			{
				local_5 = 4;
				local_6 = 1;
				local_7 = ((s_28e772 *)local_1)->field_14;
			}
			else if (local_3->field_50 != NONE)
			{
				local_5 = 5;
				local_6 = 2;
				local_7 = local_3->field_50;
			}
			point3f local_8;
			vector3f direction;
			long local_10;
			vector3f target_velocity;
			bool local_11 = swarm_target_get_location(local_7, local_6, &local_10, &local_8, &target_velocity);
			s_28e772 *local_12 = (s_28e772 *)local_1;
			vector3f *local_14 = &local_12->field_70;
			local_3->field_80 = *local_14;
			local_3->field_8c = local_12->field_7c;
			local_3->field_98 = *g_4687a4;
			if (local_5 != local_3->field_24)
				local_3->field_26 = 0;
			else
				local_3->field_26++;
			vector3f local_13;
			s_object_marker local_15;
			switch (local_5)
			{
			case 0:
			{
				if (!local_4->field_30)
				{
					local_4->field_24 = local_4->field_8;
					local_4->field_30 = true;
				}
				real local_16 = local_4->field_24.x - local_3->field_14.x;
				real local_17 = local_4->field_24.y - local_3->field_14.y;
				real local_18 = local_4->field_24.z - local_3->field_14.z;
				real local_19 = local_18 * local_18 + local_17 * local_17 + local_16 * local_16;
				if (local_3->field_56 || local_19 > 9.0f)
				{
					if (local_19 < 1.0f)
						local_3->field_56 = false;
					else
					{
						vector3f goal_direction;
						function_28e774(&local_4->field_24, &local_3->field_14, &goal_direction);
						if (function_30bf0(&goal_direction) > 0.0f)
						{
							local_3->field_80 = goal_direction;
							local_3->field_98 = *g_4687a8;
						}
						local_3->field_56 = true;
					}
				}
				break;
			}
			case 1:
			case 2:
				if (local_11)
				{
					function_28e774(&local_8, &local_3->field_14, &local_13);
					real target_distance = function_30bf0(&local_13);
					if (target_distance > 0.0f && !local_3->field_57 && !(bool)local_12->field_12c)
					{
						if (local_3->field_4c > 0)
							goto local_21;
						local_3->field_80 = local_13;
						local_3->field_98 = *g_4687a8;
						if (dot3f(&local_3->field_80, local_14) > 0.95f)
						{
							if (function_2900b0(arg_1, (point3f const *)&target_velocity, 0.9f, 2.7f, (vector3f *)&local_3->field_58))
							{
								real local_20 = function_259d0((dword *)g_4e7408, NULL, 0, 1.0f, 3.0f);
								local_3->field_57 = true;
								local_3->field_4c = (short)function_1469f0(local_20);
							}
						}
					}
				}
				break;
			case 3:
			local_21:
				if (local_11)
				{
					function_28e774(&local_8, &local_3->field_14, &local_13);
					real local_22 = function_30bf0(&local_13);
					if (local_22 > 0.0f)
					{
						local_3->field_80 = local_13;
						if (local_22 > local_2->field_10)
						{
							local_3->field_98 = *g_4687a8;
							if (local_3->field_68 < 0.0f) local_3->field_68 = 0.0f;
							if (local_3->field_74 < 0.0f) local_3->field_74 = 0.0f;
						}
						else if (local_22 < local_2->field_c)
						{
							local_3->field_98 = *g_4687b4;
							if (local_3->field_68 > 0.0f) local_3->field_68 = 0.0f;
							if (local_3->field_74 > 0.0f) local_3->field_74 = 0.0f;
						}
					}
				}
				break;
			case 4:
				if (!function_118f70(arg_1) && (real)local_3->field_26 * g_510c54->rate > 1.0f)
				{
					function_b9a90(arg_1);
					if (local_11)
					{
						point3f placed_point;
						function_b9dd0(arg_1, &placed_point);
						local_13.i = placed_point.x - local_8.x;
						local_13.j = placed_point.y - local_8.y;
						local_13.k = 0.0f;
						if (function_30bf0(&local_13) > 0.0f)
						{
							vector3f_add((vector3f const *)&placed_point, &local_13, (vector3f *)&local_8);
							if (function_2900b0(arg_1, &local_8, 0.9f, 2.7f, (vector3f *)&local_3->field_58))
								local_3->field_57 = true;
						}
					}
				}
				break;
			case 5:
				if (local_11)
				{
					short local_23 = 0;
					switch (local_3->field_54)
					{
					case 0: local_23 = function_b8d30(local_10, 0xf0005b4, &local_15, 1, false); break;
					case 1: local_23 = function_b8d30(local_10, 0xe0005b5, &local_15, 1, false); break;
					}
					if (local_23 > 0)
					{
						point3f placed_point;
						function_b9dd0(arg_1, &placed_point);
						function_28e774(&local_15.matrix.position, &placed_point, &direction);
						real local_24 = function_30bf0(&direction);
						if (!(local_24 > 0.0f))
							break;
						local_3->field_80 = direction;
						local_3->field_98 = *g_4687a8;
						if (local_24 < 0.2f)
						{
							function_28f0e0(arg_1, (s_28f0e0 const *)&local_15, &direction, local_10);
							break;
						}
						if (local_12->field_17c != 1 || (bool)local_12->field_12c)
							break;
						vector3f const *facing = &local_12->field_1f0;
						real local_25 = dot3f(facing, &direction);
						if (!(local_25 < -0.99f || (facing->k > 0.70710677f && local_25 > 0.99f)))
							break;
					}
					local_3->field_50 = NONE;
				}
				break;
			case 6:
				if (local_11)
				{
					function_28e774(&local_3->field_14, &local_8, &local_13);
					real local_26 = function_30bf0(&local_13);
					if (local_26 > 0.0f)
					{
						if (local_26 < local_2->field_4)
						{
							local_3->field_80 = local_13;
							local_3->field_98 = *g_4687a8;
							function_28e774(&local_3->field_14, &local_4->field_8, &direction);
							if (function_30bf0(&direction) > 0.0f)
							{
								real local_27 = (real)local_3->field_26 * g_510c54->rate;
								local_27 = local_27 < 0.0f ? 0.0f : local_27 > 5.0f ? 5.0f : local_27;
								vector3f contribution;
								vector3f_scale(&direction, (1.0f - local_27 * (1.0f / 3.0f)) * 2.0f, &contribution);
								vector3f_add(&local_3->field_80, &contribution, &local_3->field_80);
								if (function_30bf0(&local_3->field_80) == 0.0f)
									local_3->field_80 = local_13;
							}
						}
						else
						{
							vector3f_scale(&local_13, -1.0f, &local_3->field_80);
							local_3->field_98 = *g_4687a4;
						}
					}
				}
				break;
			case 7:
				function_28f290(arg_1, &local_3->field_80, &local_3->field_98);
				break;
			}
			local_3->field_24 = local_5;
			if (local_5 != 0) local_4->field_30 = false;
			function_28e600(arg_1, local_3);
		}
	}
}
