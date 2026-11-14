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

// MiniMax B (11-12): no new form, no lower score (173); retail's rotation takes the second cross with the
// rotated vector on the left (its .x is the up vector's .k times the sine) and subtracts that from the sum,
// so the helper's cross the other way round and subtracted is the same value: 187. Retail keeps one of the
// three cross products in a register and spills the other two as scalars (+0x20 and +0x28, the sine at
// +0x24) where the helper spills the whole vector (12 bytes at +0x24..+0x2f, four bytes of frame more
// than retail's 0x118); scaling the cross per component at the point of the sums gives 193 with the cosine
// first and 197 with the sine first. The sine before the cosine gives 182, and the cross and the dot before
// the two trigonometric calls gives 238. So the frame and the colouring of the cross terms stay as they are.
// MiniMax R (11-11): the rotation rebuilt inside the helper the way retail reads it (sine first; the cross of
// the heading and the up vector in scalar products; the cosine after the dot; the second cross subtracted in
// the rows; the result left in registers) compiles to the same instruction sequence as retail, but the
// register colouring and the stack layout differ: 205 (1212 bytes, frame 0x118, the first difference at the
// zero register and then at the heading's homes). NOT kept: the score rises from 173; the original helper
// and body restored.
// score 173 (was 173): Frame 0x11c versus retail 0x118 (ours spills a rotation cross term to +0x2c
// where retail uses +0x28); the rotation computes the cosine with fcos before fsin (retail sine
// first, stored after the dot) and colours the cross terms differently.
// MiniMax E (11-10): no new form, no lower score (173); both sides run the same operations, but the frame is 0x11c here and 0x118 there, and the products are held in different registers, so the same work is scheduled differently and lands at other frame offsets.
// MiniMax S (11-06): the permuter's 42 rewrites all score 173 or worse on the reduced image; no new form, no lower score.
// MiniMax D (11-09), not kept by the coordinator: the struct's trailing bytes cut to 0xc0 gives a 0x118 frame and 161, but local_0 is the 0xc4-byte creation data function_b7930 fills (s_damage_object_creation in damage.cpp ends with unknownc0), so a shorter local lets b7930 write past it.
// MiniMax S (11-06): the permuter's 42 rewrites all score 173 or worse on the reduced image; no new form, no lower score.
// MiniMax D (11-09), not kept by the coordinator: the struct's trailing bytes cut to 0xc0 gives a 0x118 frame and 161, but local_0 is the 0xc4-byte creation data function_b7930 fills (s_damage_object_creation in damage.cpp ends with unknownc0), so a shorter local lets b7930 write past it.
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
// Round 335: Complete accepted-result bool/byte/short/long 173/173/197/195;
// complementary rejected results 202/202/200/201. All are exact-only post-test
// flag forms; none matches. Original result flow, rotation helper, full request
// and complete body restored; frame 0x11c versus 0x118 remains.
// Round 237: staged cross consuming weighted-preserved axis, four sine lifetimes 242/243/243/217;
// consuming weighted-preserved direction 226/227/208/209
// Original executable body and helpers restored.
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
// Round 284: shared scaled-add accumulation deferring cross/projection/both/direction+cross,
// original/sine-first production: 233/230, 204/252, 254/253, 235/231. Original
// complete helper and body restored; frame 0x11c versus 0x118 remains.
// Round 237: staged cross consuming weighted-preserved axis, four sine lifetimes 242/243/243/217;
// consuming weighted-preserved direction 226/227/208/209
// Original executable body and helpers restored.
// Round 291: Heading/paired transform/fallback-point overlap-safe CRT copies, subsets 1/2/4/3/5/6/7,
// then all with shared velocity scale, 264/299/206/280/275/313/296/296. Original full file and helpers restored.
// Round 318: Complete cosine-plus-projection scalar subtotals on the added-cross/consumed-axis
// graph in six producer orders, const subtotals and completed scalar outputs, eight at 228.
// Original shared products, helper and full body restored; frame 0x11c versus 0x118 and
// cross/trig homes remain.
// Round 256: fused per-coordinate rotation weighting/publication, long/dword/short/byte
// counters 225/225/216/216; separate axis scale with fused cross/direction/output
// 229/229/257/257. Original separate shared scales, helper and complete body restored.
// DH282: Complete caller heading/axis and inlined cross lexical priorities abc/acb/bac/bca/cab/cba; first
// with native request/transform/result identities, last with those and all scalar role names [173, 173,
// 173, 173, 173, 173, 173, 173]. Original executable body restored.
// Round 73 (m3 lane 16, fast; 173 stands): the rotation with named scalar cross terms and scalar outputs, cross k/j, sine,
// cosine, projection, cross i 237 (1220 bytes); j/k, cosine, sine, projection, i with the vector term first 238; i/j/k, sine,
// cosine, projection 237; sine, cosine, projection, j/k/i 237, with a vector output 275 (1244); sine, cosine, j/k,
// projection, i 237; cosine, sine, projection, j/k/i 238. Compile-only grids of 1440 statement orders and 224 inline-row
// forms (rows as 0xd0080, itself unmatched, writes them: `axis * along + x * cosine - (cross term) * sine`) found none near
// retail, but that screen's measure is register-sensitive, so it does not rule the shapes out. Retail's rotation: heading
// zero in xmm3 replaced by 1.0 after the dot, memory 0.0 then `xorps` later.
// DH296: Velocity publication before moving/fixed position offsets, native/shared
// scaler and original/late sine: 227/227/227/227/191/191/191/191. Every frame
// remains 0x11c; original offsets, velocity, private rotation helper and full body restored.
// F341: Native projection/sine/cosine last-use scopes, seven subsets then all
// with complete rotation-role names: 173 each. Only uninitialized declarations
// move; cosine/dot/cross/sine and scale order stay fixed. All restored; frame
// 0x11c versus 0x118 and cross/trig homes remain. Three exact controls survive.
// DH317: Immutable native cosine/projection/sine values, seven subsets and all
// with immutable angle input, eight at 173. Original helper and complete body
// restored; frame 0x11c/0x118 and cross/trig homes remain.
// Shared angle-to-direction heading with staged outer/rotation crosses and late
// sine, eight at 277/273/277/273/277/273/277/273. NOT kept: extra zero-pitch
// trig/products and longer body; original heading, includes and helper restored.
// Round 64 (m3 lane 1, fast): the (vector, axis, sine, cosine) helper of matched 0x2551f0/0x1ecf50 (scalar cross
// i/j/k, rows `v*cos + projection*axis - cross*sin`, called with sin then cos of the angle) 236 (1190 bytes, frame
// 0x114: sine first as retail, but heading at +0x10 and cosine at +0x1c, retail cosine +0x10 and heading +0x14; only
// the row-i result spills where retail spills cross j and k); with dot3f 236; cross terms declared j, k, i 236 (same
// code); the cross through cross3f into a vector 226 (1209 bytes, frame 0x120), with dot3f 226.
// Round 80 (m3 lane 12, fast, 0x259d0 listed): retail's rotation multiplies the zero k by the 0.0 constant from
// memory (`mulss xmm3, [0.0]`) where ours keeps it in a register, and runs fsin, the dot, then fcos. In the
// rotation helper: sine then cosine first, the dot after them 182 (frame 0x120); sine, a named dot, then cosine
// 225 (frame 0x120); the cross scaled by the sine first 227; the final sums written cross + projection +
// vector 173 (same code). The direction's zero k stored before i and j 173 (same code).
// AC503: Shared moving/fixed offset and velocity producers on the added-cross graph, seven
// subsets then the complete native point receiver 216/180/173/180/173/180/180/173. Original full file/helper restored;
// frame 0x11c/0x118 and cross/trig homes remain. NOT kept: unmatched producer/field-pointer forms.
// Round 68 (m3 lane 10, fast; aligned difflib counts in brackets, base 173 [183]): the rotation helper in the
// matched 0x177d10's shape (sine, cosine, projection with the dot written out, vector x axis cross j, k, i, rows
// `v*cos + axis*projection - cross*sin`) 231 [176] (1192 bytes, frame 0x114: sine +0x24, cosine +0x10 and heading
// as retail, but one cross spill where retail spills X.j and X.k); rows axis term first 231; X.i written in row i
// 231; dot3f for the dot 228 (1194); the cross through cross3f(vector, axis) into a vector 213 [159] (1209 bytes,
// frame 0x11c), with axis-first rows or either dot3f order 213/213/213. Original helper restored.
// Round 206 (m3 lane 3, fast): retail's rotation is the one 0x117dd0 inlines (sine first, X = vector x axis,
// X.j/X.k spilled with the sine's home between them, X.i formed in row i). Sine, named dot, cosine, scalar X.j/X.k
// with X.i inline 237 (1221 bytes); sine, dot, cosine, cross3f(vector, axis) subtracted 226 (frame 0x120), with
// vector-first rows 226; that body as a (vector, axis, sine, cosine) helper 226; sine, cross3f, dot, cosine 233 (1196).
// Original helper restored.
// AC503: Shared moving/fixed offset and velocity producers on the added-cross graph, seven
// subsets then the complete native point receiver 216/180/173/180/173/180/180/173. Original full file/helper restored;
// frame 0x11c/0x118 and cross/trig homes remain. NOT kept: unmatched producer/field-pointer forms.
// DH339: Native accumulation through three advancing component cursors, long/dword/short/byte
// bounds 229/229/315/315; output-end for/while/do walks 316/316/314; split adds 316.
// NOT kept: frame/length move away from retail; original scales, helper and complete body restored.
// DH341: Immutable heading-angle/launch-speed/tilt-angle draw storage, seven subsets
// then all five draws, 173 each. Original draw storage and complete body restored;
// frame 0x11c/0x118 and cross/trig homes remain.
// DH344: Real trig APIs at heading cosine/sine and rotation cosine/sine,
// individually, paired, all, then all with sine after axis scale: 180/180/180/180/186/185/173/173.
// NOT kept: no native frame/cross/trig-home recovery; original complete helper/body restored.
// DH346: Immutable existing rotation-axis/output and position-input pointer bindings,
// seven subsets, then all with immutable position-output binding: 173/173/173/173/173/173/173/173.
// Original helpers and complete body restored; frame 0x11c/0x118 and cross/trig homes remain.
// DH352: Existing double-factor axis/cross/direction publishers, subsets 1/2/4/3/5/6/7,
// then all with the double-factor launch publisher: 233/217/236/278/238/230/298/300. NOT kept:
// no exact precision/API form; original helper, real products and full body restored.
// Round 52 (m3 lane 2, fast; aligned count ignoring stack offsets in brackets, base 173 [154]; lead, not kept, scores rise): the rotation written in the
// caller as matched 0x117dd0 writes it, not in a helper: sine and cosine, cross3f(vector, axis), projection, rows `axis * projection + vector * cosine
// - cross * sine` 228 (1196 bytes, frame 0x11c) [125]; sine, cosine, projection, cross 222 (1209 bytes, frame 0x120) [123], with a named dot or
// cross3f_staged same code; axis x vector added 222 (1209) [128]; a copy of the vector 210 (1220) [148]; scalar X.j/X.k with X.i in row i 231 (1209, frame
// 0x114) [139], X.k/X.j 231 (1210) [121], all three scalars 231, cross before projection 228 twice [141]. Each view of the rotation or the whole arm in a
// compiled-out `do { } while (false)` same code. Retail's frame 0x118 sits between the aggregate cross (0x11c, 0x120) and the scalar cross (0x114); retail
// drops the zero k's register after the dot, then uses memory 0.0 and an `xorps`, ours keeps one register for it.
// DH360: Existing native axis/direction/point-output coordinate-array contracts, seven subsets
// and complete interfaces with immutable angle: eight at 173. NOT kept: unmatched interface casts;
// original vector contracts, consumed-axis stages, ordinary pooled reads and full body restored.
// DH362: Existing coordinate-union access at heading/rotation/request boundaries,
// seven subsets 173 each; all with native fallback-point copy loop 209. NOT kept: no
// native frame/cross-home gain; original helpers, draws, ordinary pooled reads and body restored.
// DH364: Existing placement-source/output native references and point-pointer
// return, subsets 1/2/4/3/5/6/7, then point-reference return: eight at 173.
// Original query address, fallback copy, helper contracts and complete body restored;
// frame 0x11c versus 0x118 and rotation cross/trig homes remain.
// DH367: Standard coordinate transforms at axis/cross/direction scales, seven subsets,
// then native two-stage sum transforms: 407/395/395/427/459/417/442/464. NOT kept: no byte match;
// original scales, sums, draws and complete helper/body restored; frame/cross/trig homes remain.
// Round 151 (fast): the rotation through a same-file copy of the existing rotate_direction with direct `(real)sin(a), (real)cos(a)` arguments 236 (1190 bytes, frame 0x114),
// of rotate_vector_170d70 direct 236 (1190), axis term first with (cosine, sine) parameters 238 (1190), named sine and cosine with each 241/241/241 (1193). Original helper restored.
// Retail's rotation read from its listing: `v * cosine + axis * projection - cross(v, axis) * sine`, v.k a constant zero; sine named before the copy or the cross with cosine named before the rotation 241/241, both named before
// the copy or the cross 241/241, sine named early with cosine direct 236/236 (all frame 0x114). Frame slots: +0xc scale, +0x10 cosine, +0x14-0x1c v, +0x20 cross k, +0x24 sine, +0x28 cross j.
// T60: Existing tilt-angle pointer/reference crossed with axis/output references:
// eight at 173/1209. NOT kept: no native frame/cross/trig-home recovery.
// Original value-angle/vector-pointer producer, consumed-axis phases, ordinary
// pooled reads, draws, request and complete body restored.
// F403: Existing stored forward as read-only rotation/outer-cross input, original
// consumed-axis or distinct projected-axis output, eight protocols: 231/235/241/256/232/231/238/198.
// NOT kept: no exact ownership form; original helper, input/output contracts and full body
// restored. Frame 0x11c/0x118, retained heading zero and cross/trig homes remain.
// Round 66 (m3 lane 14, fast): the rotation helper in 0x146320's statement shape (sine, cosine, projection, scalar cross j/k,
// cross i inline in row i), dot k/j/i, i/j/k and j/k/i, 231/231/231 (1192 bytes; register-masked aligned 46 against the
// baseline's 63, but the colouring regresses); that body with the axis passed as three scalars and the sine/cosine from the
// caller, named or as arguments, 240/240 (1193). Retail's outer cross reads each up-vector field into a register per product
// (`movss xmm5, [u.k]; mulss xmm5, h.j`) where ours folds one operand from memory. The axis as up x heading with the helper's
// cross taken vector x axis (the same values) 190 (1209 bytes: ours then loads each up field per product too, but in another
// order), with the outer cross staged 233 (1199).
// AC553: Complete source-record initializer/readonly/assignment/memcpy before
// or after request initialization: 330/330/330/330/335/335/335/335. NOT kept: copied
// input grows the frame and loses caller 0x28dab0; complete original body restored.
// DL269: Native double C trig inputs at heading/tilt cosine/sine producers,
// singly, paired, all and mixed: 186/186/185/185/173/173/173/185 (control 173).
// NOT kept: no gain or equality; original real API inputs, frame and full body restored.
// DL271: Typed standard coordinate transfers at transform forward/up and both heading
// publications, subsets 1..7: 283/283/292/264/295/280/279; complete backward ranges 279.
// NOT kept: no equality or gain; original native copies, frame, helper and body restored.
// MiniMax R (11-05): no new form, no lower score; the result local declared last or without its initializer
// changes nothing, every slot sits at the same address as retail's and only the frame size differs (0x11c/0x118).
// M3 lane 15 r60 (fast with 0x28dab0): a one-file compile without LTCG (`/FAs`) also gives frame 0x11c and lists the helper's
// cross vector with its own 12-byte home, so the frame is fixed before link time. Projection as `(1.0f - local_1) * dot3f(...)`
// 173 (same code); rows as `arg_2->i += arg_1->i + local_3.i` 173; scales axis, vector, cross 233 (1222 bytes); rows axis term
// first 173 (as round 120); the forward copy after the outer cross 190; velocity products speed first 173 (same code); rows
// `arg_2->i + local_3.i + arg_1->i` 173.
// Round 84 (m3 lane 1, fast with 0x28dab0 and 0x259d0 listed, 173 as the full check; aligned and masked aligned counts in brackets, base [131, 72]).
// Reading of retail's rotation (r206 already lists X.j/X.k spilled and X.i formed in row i): that is the order in-place row writes give, because row
// i's store kills the vector's i, which only X.j and X.k read. Retail keeps the vector's i and j in registers throughout and gives the zero k a register
// only for the outer cross and the dot; ours gives the zero a register throughout and re-reads the vector's j from its home. Re-measured listed shapes:
// sine, named dot, cosine, scalar X.j/X.k, X.i in row i 237 (1221) (r206), sine, dot, cosine with `projection *= 1 - cosine` 237 (1220), sine, cosine,
// projection 237 (1220) (r73's 237s), in-place rows with dot3f 228 (1194, frame 0x114) [124, 63] (r68), three scalar cross terms with a named dot 236
// (1190) (lane 1 r64), cross3f(vector, axis) 213 (1209, frame 0x11c) [129, 91] (r68) and 226 (frame 0x120) (r206). New: the cosine called at each use
// 238 (1221) [144, 84]; in-place rows with scalar X.j/X.k and a named dot before the cosine 236 (1187, 0x114) [140, 82]; three scalar cross terms with
// sine, cosine, projection 232 (1189) [139, 78]; the vector's components read into three locals first, scalar X.j/X.k or all three, either trig order,
// 227 each (1211, frame 0x11c) [144, 87]; a local copy of the vector with scalar cross terms 251/250/251/250 (1224, frame 0x114) [155-159, 96], with
// cross3f 256 (1228, frame 0x11c) and 249 (1221, frame 0x11c). No form has retail's 0x118 frame: scalar cross terms keep X.j in a register (one spill,
// 0x114) and a cross vector takes a 12-byte home (0x11c or more). Helper restored.
// M3 lane 13 r72 (fast with 0x28dab0, matched throughout; brackets: aligned count with registers and stack offsets
// masked, base 173 [41]). Retail's jitter arm copies z onto itself (`movss xmm0, [z]; movss [z], xmm0`), adds with
// `fld [x]; fadd st(2)` and pops both draws afterwards. A zero-k offset vector added component by component gives the z
// copy only 226 [39] (through vector3f_add the same code); that vector added through the existing
// vector3f_add_scaled with scale 1.0 222 [38]; with the arm's two 0.1 position updates through vector3f_add_scaled as
// well 234 [35]: the jitter arm is then retail's instruction for instruction (lead; the score rises, not kept). The
// offset through vector3f_assign 241/245, draws named first 245/241, real_point3d_set on the position 173 (same code).
// Retail's rotation read again: result = axis * P + v * cosine - (v x axis) * sine with v.k zero, sine taken first, the
// cosine after the dot, X.j and X.k spilled and X.i formed in row i, results left in registers. On the 234 base, in
// place on v: sine, dot, cosine, scalar X.j/X.k, X.i inline 226 [27] (frame 0x114); sine, cosine, projection 235 [30];
// named dot 234 [31]; cosine, projection, sine 223 [25] (frame 0x118 as retail, first difference +0xa9, 1231 bytes);
// cosine, dot, crosses, projection, sine 196 [29] (frame 0x118, 1213 bytes); dot3f operands swapped 235; vector term
// first in the rows 235, with sine, dot, cosine 226 [27]; X.k before X.j 237/249; the crosses ahead of the cosine or
// the dot 243/228/242/243/228 (three scalars or two), sine, cosine, dot, crosses 247, sine, dot, cosine, crosses 247;
// cross3f(v, axis) as a vector 235 [33] (frame 0x11c), 243 [37]; rows into a separate result vector (five spellings)
// 230 [33] (1211 bytes, frame 0x114), with cross3f 247 (frame 0x120), result vector copied back 303, scalar results
// 261 (frame 0x120); velocity through vector3f_scale 239. Matched 0x177d10's statement shape (s, c, t, cross j/k/i,
// rows v*c + axis*t - cross*s) in the helper 235 [30] (frame 0x114), with dot3f 235, written in the caller in place
// 248 [27], into a result vector 230 [33], with dot3f 230; 0x117dd0's shape (sine and cosine, cross3f, projection,
// its row orders) in the helper 240 [41] (frame 0x11c), in the caller in place 175 [33] (1218 bytes, frame 0x11c),
// into a result vector 230 [30] (1212 bytes, frame 0x11c). On the body below alone: the in-place scalar helper 228
// [35], cosine at each use 236, result vector 275, cross3f vector 213, scalar results 237.
// NOT kept: nothing below 173; body below unchanged.
// Round 87 (m3 lane 1, fast with 0x28dab0, matched throughout; aligned/masked base 131/72): matched 0x23d260's in-place rotation shape
// in the helper (sine, cosine, axis k local, staged projection, cross i/j/k, staged rows stored as they finish) 236 (1190 bytes,
// aligned 136, masked 83); matched 0x1055b0's shape (cosine, sine, `(1 - cosine) * dot3f(direction, axis)`, scalar cross, rows)
// 231 (1192 bytes, the code of the listed 231 forms). Round 72's jitter-arm base rebuilt from its description (both 0.1 updates
// and a zero-k jitter offset through vector3f_add_scaled) gives 252 here (1243 bytes, masked 94), not 234; on it 0x23d260's shape
// 253 (1233), 0x2551f0's (vector, axis, sine, cosine) shape 251 (1232). This note already covers the rotation's trig order,
// dot and cross spellings, helper shapes of the matched rotating functions, the heading, jitter and velocity arms, scales and
// creation data size; no untried natural form is left. Helper restored.
// Round 164 (m3 lane 6, fast with 0x28dab0, 0x256bd0 and the callees, plus 0x1ecf50/0x170d70 for the helpers' files; masked =
// aligned count with registers and stack offsets masked, 66 for this body): our ninth scalar dword is function_28dbd2's cross
// vector (an address-taken 12-byte home, two components touched); retail keeps cross j and k in separate homes (+0x28, +0x20),
// forms cross i in row i and has every XMM register busy (it multiplies v.k by a memory 0.0). The real cross-file
// rotate_direction re-measured 236 (1190 bytes, frame 0x114), with named sine/cosine 241, the real rotate_vector_170d70 236 (the
// same code as the same-file copies listed). On rotate_direction: the jitter arm as a zero-k offset vector through vector3f_add
// 228 (1209 bytes, masked 63); plus both 0.1 position updates through vector3f_add_scaled 223 (1214, masked 43); the position
// updates alone 237 (1191); 223 plus the velocity through vector3f_scale 223 (same code); 223 with rotate_vector_170d70 223 (same
// code). Every scalar-cross form keeps frame 0x114. Body unchanged.
// Round 166 (m3 lane 6, fast with 0x28dab0/0x256bd0; masked = register-masked aligned count, base 66): our calls are
// retail's nine in order, and the output point's lookup is the same single-copy inline code with no call on both sides;
// the level heading through direction_from_angles with pitch 0
// 277 (1223 bytes); the owner as a ternary 175 (masked 71), as if/else with the positive test first 175; success returning
// at once instead of the goto 289 (1210 bytes); the handler view passed straight to the ai-data getter 173 (same code).
// Body unchanged.
// M3 lane 10 r92 (fast with the matched callers 0x256bd0 and 0x28dab0; base 173, aligned 183, masked 66): nothing kept. In
// function_28dbd2: sine first, then cosine, dot, cross 182; sine, cross, cosine, dot 238 (1198 bytes); the cross taken v x axis and
// subtracted in the rows, current order 187 (masked 60), with sine, cross, cosine, dot 238; dot3f arguments swapped 173 (same code).
// m3 lane 13 r79 (fast with 0x28dab0, matched throughout; brackets: difflib aligned / register-masked, base 173
// [91/38]): the forward copy moved after the outer cross 190 [100/50]; the rotation's cross vector left unscaled and
// multiplied by the sine inside the three row sums 193 (1225 bytes, frame 0x128) [82/38]; the projection factor
// written `(1.0f - cosine) * dot3f(...)` 173 (same code); the heading aggregate-initialised 173 (same code); the
// sine taken between the projection and the cross 183 (frame 0x120). The ninth scalar dword stays.
// m3 lane 7 r150 (fast with 0x28dab0 and 0x256bd0, both stay MATCH): the in-place rotation saving the old v.i and v.j in two locals
// (v.k read at each use), sine, dot3f, cosine, `projection *= 1 - cosine`, retail's scalar X.j/X.k (cross(v, axis)) with X.i
// written in row i, rows `axis * projection + v * cosine - X * sine` 227 (1210 bytes, frame 0x11c); all three old components in
// locals 227 (same frame); cosine first with the projection in one statement 230 (0x11c); the projection named after the cosine
// in one statement 227; an aggregate copy of the old vector with the same scalar cross terms 253 (1224 bytes, frame 0x114).
// (The all-three-locals form is close to round 84's 227/1211; its code here is 1210 bytes.) Also new: the two old components saved
// after the dot 227 (frame 0x11c, other code); X.j/X.k computed before the cosine 227 (other code). Five codes, all 227-253.
// A one-link LTCG harness does not reproduce this body (function_259d0 stays out of line there), so screens need fast.
// M3 lane 1 r99 (fast with 0x28dab0, 0x256bd0 and 0x259d0, all MATCH throughout; base 173): one to four unused long locals at the
// top of the function (a scan for the symbol-count effect on SSE operand order that matched 0x1e6360) 173 each, the same code, so
// that lever does not reach this body. The rotation written in the caller in retail's order (sine, cross3f(heading, up), dot3f,
// cosine, `projection *= 1 - cosine`, scalar cross j and k of heading x axis, rows `axis * projection + heading * cosine - cross *
// sine` with cross i inline) into a result vector 251 (1188 bytes, frame 0x114), written back into the heading 231 (1209 bytes,
// retail's length, frame 0x114: the x87 order and the phases are retail's, the XMM colouring and the heading/cosine homes are not);
// those with the heading's zero k written as the constant 0.0f in the rotation terms 251/231 (the same code: the zero is already
// known); the in-place form with one or three unused locals 231 (same code). Body unchanged at 173.
// T105: Existing scale-output receivers, axis/cross/direction subsets 1..7,
// then complete returned-buffer shared sums: 226/227/227/226/226/227/227/233.
// NOT kept: no native frame/cross-home equality; original helper/body restored.
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
