#include "unknown_4e8c38.h"
// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_23D030.CPP: the camera that flies freely, and the point it keeps
   relative to an object (0x23d030..0x23d970) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "normalize3d.h"
#include <math.h>

struct s_player_control_camera
{
	long unit_index;
	short seat_index;
	byte unknown06[2];
	void *camera;
	point3f position;
};



extern real g_54e854;
void player_control_get_camera(long player_index, s_player_control_camera *camera);
vector3f *function_11d090(vector3f const *v, vector3f *out);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
struct s_observer_command;
void function_172520(s_observer_command *command);

struct s_camera_input_23d790
{
	long player_index;
	byte active;
	byte unknown05[7];
	real yaw;
	real pitch;
	byte unknown14[0x24 - 0x14];
	real zoom;
};

struct s_camera_output_23d790
{
	dword flags;
	point3f position;
	vector3f offset;
	byte unknown1c[8];
	real distance;
	real field_of_view;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x88 - 0x50];
	real timer;
};

/* the state both cameras of this file keep: the free camera's position, or the orbit
   camera's distance, then the view angles the direction helper reads */
struct s_camera_state_23d030
{
	union
	{
		point3f position;
		struct
		{
			real unknown00;
			real distance;
			real unknown08;
		};
	};
	real yaw;
	real pitch;
	real roll;
};

PRIVATE __forceinline void orbit_direction_23d790(s_camera_state_23d030 const *state, vector3f *direction)
{
	real cosine = (real)cos(state->pitch);
	direction->i = (real)cos(state->yaw) * cosine;
	direction->j = (real)sin(state->yaw) * cosine;
	direction->k = (real)sin(state->pitch);
}

// @retail 0x23d790
void __stdcall function_23d790(void *state, void *input, void *output)
{
	s_player_control_camera camera;
	s_camera_input_23d790 *controls = (s_camera_input_23d790 *)input;
	s_camera_state_23d030 *values = (s_camera_state_23d030 *)state;
	s_camera_output_23d790 *command = (s_camera_output_23d790 *)output;
	player_control_get_camera(controls->player_index, &camera);
	command->position = camera.position;
	if (controls->active)
	{
		values->yaw += controls->yaw;
		real pitch = values->pitch + controls->pitch;
		values->pitch = pitch < -1.2566371f ? -1.2566371f : pitch > 1.2566371f ? 1.2566371f : pitch;
		((byte *)g_4e8c44)[controls->player_index * 0x140 + 0x56] = 1;
	}
	real distance = values->distance - controls->zoom * (1.f / 3.f);
	values->distance = distance > 0.6f ? distance : 0.6f;
	if (camera.unit_index != NONE)
	{
		orbit_direction_23d790(values, &command->forward);
		function_11d090(&command->forward, &command->up);
		function_ba1d0(camera.unit_index, &command->velocity, 0);
		command->flags = 1;
	}
	command->offset = *g_4687a4;
	command->distance = values->distance;
	command->field_of_view = g_54e854;
	command->timer = 0.5f;
	function_172520((s_observer_command *)command);
}

void function_16c6f0(long object_index, transform4x3f *matrix);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);

/* the point the camera keeps (NULL when none), the object it is relative to
   and the point in that object's space */
point3f *g_51ec28;
long g_470a28 = NONE;
point3f g_51ec30;

byte g_51ec12;
real g_470a24 = 1.f;
extern byte g_51ec11;

// score 89 (was 89): frame 0x4c agrees; zero XMM3 (retail XMM1), normalize_inline's i-first squared length and
// the side/up/roll SSE colouring differ. Kept: both cameras share one state type and the forward vector comes from
// this file's direction helper, which puts the side.i copy and the movement x87 block where retail has them.
// Earlier side-vector assignment orders (k/j/i, j/i/k, i/j/k, j/k/i) and a
// local-carrier form all compile to the same 102/103, and a named clamp or a named zero carrier give
// 103, so the zero constant still lands in xmm3 against retail's xmm1.
// the side vector's i and j roles were swapped and the shared `normalize3d`
// (dot3f) was tried for the length; both give 102 and 291. The shared `normalize_inline` squares
// side.i before side.j while retail squares side.j first, and its scale order is side.i, side.j,
// side.k, so the two orders cannot both come from that helper.
// this round re-read the side-vector block: retail keeps the zero constant in
// xmm1 and the pitch clamps in xmm2 (ours xmm3 and xmm1) and squares side.j before side.i, so the
// normalizer's xmm ranking and the up/roll carriers still differ.
// Zero/side XMM roles and speculative normalization remain; frame 0x4c agrees.
// Earlier complete axial/cosine-normal/affine vector contributions, masks 1/2/3/4/5/6/7, scored
// 281/281/335/313/314/329/332; existing add-scaled affine assembly 313. Original normalizer,
// projection, roll rows and movement restored.
// Earlier: the rest is register allocation in the side vector
// block: retail keeps zero in xmm1 (ours xmm3) and the side vector in
// xmm2/xmm3/xmm4 (ours xmm1/xmm2/xmm0, retail moves side.k out of the inverse's
// register), divides before the lahf, and stores up as k, j, i (ours k, i, j);
// the registers after it follow. Locals `real yaw_cosine = (real)cos(yaw)` and
// `yaw_sine` for the movement give retail's x87 sequence exactly (fmul [0x18];
// fxch; fmul [0x1c]) but score 274 here, since the side block is then one
// instruction short and shifts the rest: apply them once that block matches.
// Tried: the hand-written normalize with gotos (278), normalize_inline (110 but
// 1330 bytes, squares i first), early-return or reordered-scale normalize helpers
// (118 / 288), cross3f(&side, &forward, &up) (277), up written i, j, k (253),
// k, i, j (276) or j, k, i / i, k, j / j, i, k (106-111 but not 1326 bytes: they
// spill forward.k to a parameter slot), the transforms as __forceinline helpers (322), fresh names for the
// scaled point (373), movement operands swapped (282), the roll if/else inverted
// (279), a cross vector in the turn about forward (277), declaration order (same)
// Earlier: zero XMM3 versus XMM1 and normalization/side-vector allocation remain.
// Roll-input snapshot 324; live yaw/pitch/roll fields 275/121/121; live forward/up/both fields 295/154/157; normalization result 319.
// Earlier: the existing transform4x3f_apply_point/function_1427f0 called for the two
// matrix blocks 603 (992 bytes, kept out of line); if/else pitch clamp 282; roll ternary 146; side j
// and k assigned apart 118; cosine before sine 119; side.k first 118; up written j, i, k / j, k, i /
// i, k, j 254/251/249 (1330-1333 bytes).
// Round 71: shared normalize_inline with side.k first kept at 110 (1330 bytes, plain);
// planar initialization reversed 110; up written j, k, i / j, i, k / i, k, j on that graph
// 293/112/287; shared yaw sine/cosine 110; roll update before the pitch clamp 132;
// a one-use normalization input pointer 107 (1329 bytes, exact-match-only). No pointer kept.
// Round 409: Existing staged up cross alone/fallback setter/initial setter/both setters 244/243/244/238; all recover zero XMM1 and first +0xa6, last reaches 1326 bytes, but side/up/roll allocation worsens. Initial/fallback/both whole-vector setters 110; uniformly const trig values 110. Original body and normalization retained; no helper or include kept.
// Round 419: projection/cross/output-row accumulation masks 1/2/3/4/5/6/7
// 288/251/257/105/102/286/257; all three plus normalization truth test 290.
// Keep projection and every output row accumulated at 102, 1326 bytes; row-only 105
// fails the eight-point gate. Zero/side/normalization and up/roll carriers still differ.
// Round 194: existing shared rotation/staged up cross/direct trig/both 285/221/224/220;
// those four with alternate normalization 289/307/293/299. Staged forms recover zero
// and first +0xa6 but regress the complete body. Original rotation, normalizer and all includes restored.
// Round 426: one reusable rolled-row accumulator 102; plain projection traversal, alone/with
// accumulator reuse 441/441; native cross and row traversal, without/with projection traversal
// 446/447; direct traversed row 445; shared cross with accumulated/direct traversed rows 446/445.
// All executable forms and the temporary include restored; zero and side normalization remain.
// Round 429: One pitch/roll cosine workspace, with entry/producer cosine, sine and side
// declarations: all eight 117 at 1326 bytes. Original body restored.
// Round 42 (lane 16): the existing normalize3d for the side vector gives retail's x87 squared length
// exactly (side.j kept on the stack and squared before side.i) but 291 at 1309 bytes; with the yaw
// cosine/sine locals as well the movement block matches too, 276 at 1326 bytes (the side/cross block is
// then two instructions short: retail moves side.k out of the inverse's register and loads forward.k into
// xmm6, and stores up k, j, i where ours stores k, i, j). On that form: side.k assigned last 276; fallback
// j and k assigned apart 276; normalize3d result in a named local 310.
// Round 440: Native forward producer scalar/setter, published/local consumers 355/343/339/370; entry declarations repeat
// 355/343/339/370. All native direction producers and consumer changes restored; zero/side normalization remains.
// Round 321: Complete native up-vector ownership with staged shared cross/forward snapshot/entry declaration subsets: 359/338/380/373/359/338/380/373.
// Round 321: All lengthen the body; original publication, normalizer, projection, row accumulation and include set restored. Stacked notes merged.
// Round 8 (lane 18, fast, aligned counts in brackets, base 102 [59]): fallback as `side.i = 1.f; side.k = 0.f;
// side.j = 0.f;` 102 [59]; as `side.k = side.j = 0.f; side.i = 1.f;` 118 [52] (1328 bytes); the normalized
// length in a named local 103 [59]; side declared at the top 102 [59]; `if (!normalize_inline(&side))` 121
// [59] (1328 bytes); normalize3d with the yaw cosine/sine locals re-measured 276 [66] (x87 length exact,
// the rotation's products then change operand order). The zero carrier stays XMM3 in every form.
// Round 214: inverse movement setter 256; transformed position setter 102; world position setter
// 102; inverse/transformed setters 277; inverse/world setters 277; both position setters 258; all
// three setters 277; native world position update 195.
// Round 21 (m3 lane 2, fast): side as an aggregate initializer 102 (same code); a pitch sine local computed
// with the pitch cosine 106 (1328 bytes, first difference +0x94: zero XMM1, clamp and side i/j registers as
// retail, but the x87 order changes and side.k stays in XMM0); that local after forward.j 103 (zero XMM3 again);
// the local also for both forward.k reads of the up cross product 260; yaw cosine/sine locals at the forward
// vector 324 (1351 bytes), before the movement 291.
// Round 58 (lane 12, fast = plain here; none kept): side, movement, position and pitch-cosine declared
// at the function's top, each and all together, 102 (same code); the normalize test as `!` 121 (1328
// bytes) and as `0.f ==` 102 (same code); the roll update with the zero arm first 107. The zero stays
// in XMM3 and the clamp constants in XMM1 in every form.
// Round 23 (m3 lane 2, fast): on lane 16's normalize3d + yaw cosine/sine form (276 here too: retail's x87 length,
// but the side.i copy is scheduled right after the forward.j store, before forward.k, which shifts everything after
// by one instruction; zero still XMM3 and the divide after the lahf) the up stores written k, i, j 274; j, k, i 257;
// i, k, j 286; j, i, k 283; i, j, k 285; side.j assigned before side.i 257 (the copy stays early and side.i is then
// squared first), with side.k last as well 257; side.j before side.i on the original body 102 (same code).
// Round 120 (m3 lane 5): `side` declared with `side.k = 0.f` at the function's top 102; the movement's
// yaw cosine/sine as named locals on this graph 291 (movement x87 then retail's apart from the k load,
// but the side/up block recolours and ours runs four instructions short ahead of it); the roll
// rotation's cosine products as `v->x * cosine` 102 and cross_i's second product as `axis->j * v->k`
// 102 (same code both: VC picks these operand orders itself); side filled i, j, then k last 102.
// Original body restored.
// Round 125 (m3 lane 5, fast): screen of normalize_inline/normalize3d x hand-written up/cross3f(&side,
// &forward, &up)/cross3f_staged x the six side assignment orders: normalize_inline with any order 102,
// with cross3f 257, with cross3f_staged 256; normalize3d with the hand-written up 291 (1309 bytes for
// k,i,j/i,j,k/i,k,j, 1326 for the others); normalize3d with cross3f 279 (side.i before side.j: x87
// length exact, but the side.i copy is hoisted above the forward.k store where retail keeps it after
// the side.j load) or 143 (side.j before side.i: the length squares side.i first again); normalize3d
// with cross3f_staged 289/290. With the yaw cosine/sine as named locals for the movement: normalize3d
// and the hand-written up 276 (k,i,j) / 257 (j,i,k and k,j,i); normalize3d and cross3f 279/283/283;
// normalize_inline 291.
// Round 25 (m3 lane 2, fast): a 605 s permuter run, 24 tries, none below 102 (its reduced image scores 320, so it
// cannot steer this function); the side vector through the existing vector3f_assign with normalize3d 291 (1309
// bytes), with the yaw cosine/sine locals 276 (same as plain assignments); the forward vector through the existing
// direction_from_angles (by-value angles) 282. The forward vector written through a pointer helper (this file's
// orbit_direction_23d790, reading the angles through its state pointer) moves the side.i copy after `fld
// forward.i` as in retail: called with the state cast to the orbit state 89 (1327 bytes; not keepable, a cast
// between unrelated structs), with normalize3d 261, with normalize3d and the yaw locals 279 (1312 bytes); written
// properly (a yaw/pitch pair type in both states, the helper taking it, 0x23d790 still matching) 279 (1326 bytes:
// the same x87 fix, but the side/up SSE colouring follows the type path and is worse). Zero stays in XMM3 in all.
// Round 34 (m3 lane 14, fast; aligned counts in brackets, base 102 [89]): normalize3d with yaw cosine/sine
// locals re-measured 276 [88]; on it the up components written k,i,j / j,k,i / j,i,k / i,k,j / i,j,k 274/257
// [86]/283/286/285, up as cross3f 279 [86], as cross3f_staged 289; the forward vector built in a local and
// copied out, the side read from it, 367 (1367 bytes); forward.k computed after the side vector 288 [109].
// Round 60 (lane 16, fast; aligned counts in brackets, base 102 [89]): the existing cross3f_staged for up
// 256 [48] (first difference +0xa6: zero XMM1, clamp and side registers as retail); with a file-local
// normalize (normalize_inline's shape, length as `i*i + (j*j + k*k)`) as well 236 [48]: the x87 squared
// length, zero XMM1 and the side scale are then retail's exactly, and only the up cross block (retail
// stores k, j, i as it goes, reloads forward.k/j) and the roll rotation's operand orders remain. Every
// other cross form loses the zero register again: the cross written out k, j, i 269 [89], cross3f 273 [82], staged locals stored k, j, i 268 [84]; normalize3d
// or a dot-ordered helper with the staged cross 289 [92]. On that lead: side j, i, k 256 [45]; yaw
// cosine/sine locals 237 [51]; rotated_k computed before the cross terms 230 [75] (1327 bytes); pitch
// sum operands swapped 236. On the round-42 form (276 [88]): up written i,j,k / j,k,i / j,i,k / i,k,j /
// k,i,j 285/257/283/286/274; cross3f 279; cross3f_staged 289. A 900 s permuter run on the lead reached
// 128 only through a `volatile` axis pointer (not usable). No helper or include kept.
// Round 53 (lane 10, fast): retail's side/up block is the matched 0x11d090's code (the same x87 length
// with side.j squared first, the same scale), but with a constant (1, 0, 0) fallback and the up stores k, j, i.
// The side normalized in 0x11d090's own form (length `j*j + i*i + k*k` in a local, `else length = 0`, then
// `if (length == 0.f)` fallback) 119 (retail's x87 length exactly, but the side.i copy is scheduled before
// the forward.k sine and zero stays XMM3); with `-forward.i` 119; with the epsilon test as `fabs < eps` 119;
// with side.k assigned first 119; with cross3f_staged for up 289.
// Round 40 (lane 14, fast): movement.j with the cosine product first 121 (1328 bytes); the rotation's
// projection and rows each as one expression (0x11d3b0's style) 110 (1330 bytes); the rotation's
// forward/up pointers declared before the side vector and used for the side and up cross too 289
// (1311 bytes); the roll sine/cosine inside the rotation block after its pointers 141; the two
// pointers at function scope after the sine/cosine 102; the roll sine/cosine before the side vector 118.
// Round 29 (m3 lane 2, fast; none kept): retail's side block is 0x11d090's code (`w.i = v->j; w.j = 0.f - v->i;
// w.k = 0.f`, length `sqrt(j*j + i*i + k*k)`, `!(0.0001f > fabs(length))` scale, `else length = 0.f`, then `if
// (length == 0.f)`), with a constant fallback. That side form 119 (x87 squares j first as retail, but the side.i
// copy is hoisted to the forward.j store; zero XMM3); with cross3f_staged 289; with the existing
// direction_from_angles for forward 157 (copy placed as retail, angles kept on the x87 stack). `s_view_angles`
// (globals.h) in both camera states, the file's direction helper taking it (0x23d790 still matching) + that side
// form 270: the whole forward/side x87 block is then retail's, only the SSE colouring differs (zero XMM3, length
// XMM4); on it side.k first 259, normalize3d 267, fallback as three stores 259, fallback without side.k 129
// (1328 B). On the s_view_angles form with normalize_inline and cross3f_staged 264 (1330 B, first diff +0xbd):
// the SSE side block is then exactly retail's (zero XMM1, length XMM6, inverse XMM0, side XMM2/3/4, divss
// before lahf, `movaps xmm4, xmm0`) but normalize_inline squares side.i first. Every j-first sum with
// cross3f_staged swaps zero and the entry temps (zero XMM0): normalize3d 282, the 0x11d090 form 281, local
// two-return dot-order normalizers 282/282/282/281 (k as `v->k * inv` or `inv * v->k`, early zero return,
// named length). `real length = normalize3d(&side); if (length == 0.f)` 303 (1342 B): zero XMM1 and every
// side register as retail, but the short-length path is no longer threaded to the fallback. So the zero's
// register follows the float temp count (the extra x87 reload in normalize_inline's order gives XMM1).
// Lead: s_view_angles + j-first normalize + staged cross, plus whatever adds the one temp retail has.
// Round 128 (m3 lane 5; aligned counts in brackets, base 102 [89]): the forward vector written through
// `vector3f *forward = &command->forward` (reads left on `command->forward`) 94 [78] (1328 bytes: the side.i
// copy then follows the forward.k store as in retail; zero still XMM3, side.i squared first), not kept: a pointer
// that only renames the field, with mixed access. Also: movement.j terms swapped 121 (1328 bytes, the
// product order does not move); the roll rows as `v->i = v->i * cosine + projection * axis->i - cross_i * sine`
// 288; on the pointer form: normalize3d 259 [77] (x87 length and 1326 bytes as retail, side.k then stays in the
// inverse's XMM0); that with yaw cosine/sine locals 274 [82]; cross3f_staged(&side, forward, &command->up) 282
// [51] (1335 bytes; first difference +0xae: zero XMM1 as retail, forward.k kept in a stack copy); that with
// normalize3d 267 [57]; up written k, i, j 272 [71]; cross3f 273; the up reads through `forward` 275, the side
// reads 254, both 284; yaw cosine/sine locals alone 100; side assigned k, j, i or i, j, k 94 (same code).
// Round 40 (lane 14, fast): movement.j with the cosine product first 121 (1328 bytes); the rotation's
// projection and rows each as one expression (0x11d3b0's style) 110 (1330 bytes); the rotation's
// forward/up pointers declared before the side vector and used for the side and up cross too 289
// (1311 bytes); the roll sine/cosine inside the rotation block after its pointers 141; the two
// pointers at function scope after the sine/cosine 102; the roll sine/cosine before the side vector 118.
// Round 55 (lane 10, fast): the side normalized through two-return j-first normalizers with the hand-written
// up (k, j, i): sum `k*k + j*j + i*i` with every scale `inverse * x` 275; `i*i + (j*j + k*k)` with `k * inv`
// 269; `j*j + k*k + i*i` with `(1 / m) * i` first 268; the `i*i + (j*j + k*k)` normalizer with cross3f_staged
// and the forward vector written and read through one `forward` pointer 273 (1330 bytes, first difference +0xa6),
// with the pointer for the writes only 281.
// Round 441: existing complete side/up producer with native forward/up ownership and
// two-use double movement trig values, eight subsets 512/484/474/444/503/477/465/439;
// separate force-inline declaration 512 (producer remains out of line). Original body and
// declarations restored; zero/side normalization and up/roll allocation remain.
// Round 286: paired shared horizontal input observations in real/real, double/real,
// real/double and double/double domains 293/264/267/153; reverse read order repeats
// 293/264/267/153. Zero/side ownership remains; original input reads and body restored.
// Round 219: native roll/movement input with alternate normalization, staged up cross and repeated
// double movement trig results, masks 0..7: 257/274/256/289/264/274/258/157. Original input layout,
// normalizer and complete body restored; zero/side and up/roll allocation remain.
// Round 334: Frame 0x4c agrees; first +9 is zero XMM3 versus XMM1, then side
// normalization and up/roll allocation. Uniform named product parts at the actual
// up-cross/roll-cross/rolled-row expressions, subsets 1/2/3/4/5/6/7,
// 315/312/129/312/114/257/291; all parts const 291. Every parallel row uses
// the same form; original arithmetic trees and store phases preserved. Original body restored.
// Round 337: immediate up-component accumulations, separate/shared scratch, producer/entry
// declarations, then each with complete roll-cross accumulation: 256/254/256/254/288/289/288/289.
// Original body restored; zero/side normalization and up/roll allocation remain.
// Round 38 (m3 lane 2, fast with 0x23d790/0x11d090, both matched throughout; masked-aligned counts in
// brackets, base 102 [85]): retail's side block is matched 0x11d090's own code, with 0x11d090's fallback
// vector as constants. Calling 0x11d090 for the side and up 512 (1130 bytes: it stays a call), with a
// `__forceinline` redeclaration 512 (still a call); the forward vector through this file's direction helper
// taking the yaw/pitch pair by a pointer to the yaw (both callers) 279 [69] (the forward/side x87 then
// retail's except normalize_inline's i-first length; zero XMM3); that with 0x11d090's j-first side form
// 270 [80], with normalize3d 267 [82] (as round 29's s_view_angles forms).
// Round 340: coordinated roll-cross/row/trig workspace names on the retained
// accumulated-row graph, seven subsets and all six cross/row declarations at entry, 102 each.
// Original body restored; zero XMM3/XMM1 and side/up/roll allocation remain.
// Round 351: Complete camera-angle observation ownership on the accumulated projection/row graph;
// 324/135/365/266/361/252/362/362. Pitch-only recovers zero XMM1, first +0x89,
// but regresses to 135. Original executable source restored.
// Round 467: Existing projection dot on accumulated rows, reused pitch/complement/shared-cosine
// subsets: 289/284/97/136/290/284/95/134; complement-first compound projection 289/290.
// Best 95/1329 is seven below 102 and fails the eight-point gate; original body restored.
// Round 353: Final camera-position, command-position and offset copy setters, seven
// subsets then all with earlier bare position storage, 316/293/335/152/306/320/338/338.
// Original output publications and complete body restored; zero/side and up/roll allocation remain.
// Round 355: shared complete roll cross, direct/staged, on accumulated projection/rows,
// with pitch observation and named projection complement subsets: 282/282/298/298/191/191/347/347.
// Original body restored; zero/side normalization and up/roll allocation remain.
// Round 360: complete inverse/transformed/world position accumulations, subsets 1/2/4/3/5/6/7,
// 288/121/121/288/121/102/121; all with reused pitch observation 276. Original matrix rows,
// publications and complete body restored; zero/side normalization and up/roll allocation remain.
// P261: complete native accumulated roll rows before shared-setter publication,
// all six producer orders 291 each; direct complete publications 291/292.
// Original interleaved row stores and full body restored; zero/side/up allocation remains.
// Round 137 (m3 lane 6, fast, plain where marked; 0x23d790/0x23d030 stay matched): the two matrix blocks as
// file-local force-inline copies of 0x142570/0x1427f0 277 (1292 bytes, diagnostic: retail's matrix code already
// agrees); `side.j = 0.f - forward.i` 102 (same code); the forward vector through the direction helper with the
// state cast to the orbit state 245; with one shared state type instead 89 plain (1327 bytes, kept). On 89:
// normalize3d 261 (x87 length exact, divss after lahf, zero XMM3); cross3f_staged(&side, forward, &up) 283, with
// normalize3d 266; cross3f(&side, ...) with normalize3d 263; diagnostic: the sign-flipped
// cross3f_staged(forward, &side, &up) gives zero XMM1 and the whole side block exactly (144; with normalize3d 48,
// first difference +0x11d), so the up cross's operand shape sets the zero register; up rows k,i,j / j,k,i / j,i,k /
// i,j,k / i,k,j 273/90/272/275/275, with normalize3d 266/270/262/260/261; side filled i,j,k / j,i,k / k,j,i /
// i,k,j / j,k,i 89 each, with normalize3d 271/281/271/271/281. The correct cross3f_staged(&side, ...) gives retail's
// side block shape (divss before lahf, side.k moved to XMM4) with every register renamed, but its products put side
// first where retail's (and this body's) rows load forward first. The up rows computed into up_k/up_j/up_i before
// the stores, stored k,j,i / i,j,k: 94 (1328 bytes) / 274, with normalize3d 259/258.
// F332: Consumed roll cosine/sine coefficient reused for inverse scale, phase/entry
// storage and consistent matrix-scale observation, 319/322/319/322 in both cases.
// Original scalar lifetimes and full body restored; zero/side/up allocation remains.
// N320: Shared direction-helper pitch/yaw observations and scalar/native vector publication regress
// the target and the matched second user. Original helper restored; zero, normalization and up/roll
// allocation remain. Scores 291/128/280/304/101/307/295/345.
// Original executable source restored.
// DH302: Roll cosine at the unscaled projection / projection-and-cross boundary, then both
// coefficients at those boundaries: 92/102/89/151; bare entry declarations repeat 92/102/89/151.
// The 92/102 forms reach 1326 bytes; all tie or regress. Original coefficient phases and body restored.
// AC483: Existing planar side normalizer with planar/spatial fallback and original/setter/staged/direct
// up publication: 268/285/282/282/229/261/127/259. The planar length tree is exact-match-only;
// no match, so original side/normalization/rotation and complete body restored. Zero and up/roll allocation remain.
// Actual side/roll-coefficient/movement phase endings and their pairs/all 89/89/89/89/89/89/89; all
// with transformed-displacement phase ending 89. Zero/normalization/up homes remain; original body
// restored.
// M326: Native normalization fallback bool/byte/short/long results, then success polarity,
// 234/234/234/234/234/234/234/234. All lengthen to 1328/1326; one-use admission storage
// does not match. Original complete body restored; zero/normalization/up homes remain.
// N327: Complete control-produced roll retained through both trig uses,
// branch/conditional/default/additive updates with producer/entry result declarations:
// 322/322/321/319/322/322/321/319. All lengthen the body; original control result, state observations and
// complete body restored. Zero/side/up allocation remains.
// Round 380: Final timer-policy truth, bool/byte/short/long suppressed then enabled
// complement: 232/232/232/232/232/232/232/232. Zero/side/up allocation does not recover; original final
// guard, publications, decrement and complete body restored.
// Round 382: Complete entry control admission, bool/byte/short/long active then inactive
// complement: 236/236/236/236/236/236/236/236. All lengthen and regress; original entry byte test and
// complete body restored. Zero/side normalization and up/roll allocation remain.
// DH309: Existing shared normalizer magnitude/zero/separate/complete result ownership,
// failure/success polarity and failure early return: 91/276/89/89/335/274/274/89.
// Original shared body restored; zero/side normalization and up/roll allocation remain.
// Round 77 (m3 lane 9, fast with 0x23d790/0x23d030, both stay matched): the zero fallback as two stores
// `side.j = 0.f; side.k = 0.f;` 276 (1326 bytes; the up rows then store k, j, i as retail, but the x87 length still
// squares i before j and every register is renamed), with normalize3d 261 (1326), with side filled i, j, k 276; the
// fallback chained `side.k = side.j = 0.f` 274 (1326); `side` declared at the function's top 89 (same code); a
// forward pointer local for the up cross 236 (1330). Retail's length keeps side.j on the x87 stack (fst) and
// squares it first; zero XMM1 and the up/roll allocation remain.
// Round 199 (fast, with 0x23d790): side assignment orders k/j/i, j/i/k, j/k/i, i/j/k on the current body 89 each;
// diagnostic, a scratch normalizer with the length as `i*i + (j*j + k*k)` or `(j*j + k*k) + i*i` gives retail's
// j-first squares and 1326 bytes but 266 (aligned 52, unchanged), `k*k + j*j + i*i` / `j*j + i*i + k*k` 271
// (aligned 59): the square order is not what moves the zero/side registers. Fallback stores: separate i/j/k
// 276 (1326 bytes, aligned 55), k/j/i 276, `side.j = side.k = 0.f; side.i = 1.f;` 86 (aligned 47, a gain of 3:
// not kept), `side.k = side.j = 0.f; side.i = 1.f;` 274 (aligned 46), j/k/i separate 276 (aligned 55); that
// 86 form with the scratch normalizer 264 (aligned 47).
// Lane 17 r39 (fast, with 0x23d790): retail loads the other operand first in two cross products (`v->k * axis->j`,
// `axis->i * v->k`); both written the other way round 89, each alone 89/89 (same code); fallback
// `side.i = 1.f; side.k = side.j = 0.f;` 274, `side.i = 1.f; side.k = 0.f; side.j = 0.f;` 276 (both 1326
// bytes). Original body restored.
// AC501: Native input roll/horizontal/vertical field domains on the shared-state graph,
// masks 1/2/4/3/5/6/7, then complete read-only controls: 274/89/89/118/274/89/118/118.
// Original input layout, observations and full body restored; zero/side/up allocation remains.
// Round 158 (m3 lane 4, fast): normalize3d with a forward pointer and the up rows staged, declared and stored i,j,k 52 (aligned 28); that with the else arm's own speed local 45 (aligned 23, 1330; plain
// confirmed). NOT kept (review): the gain needs the staged up_i/up_j/up_k, one-use locals (match only); there the zero
// and side block agree, and the up rows copy forward.i where retail copies side.j/side.k. Body restored at 89.
// On the way: helper taking pointers, rows i,j,k stored k,j,i 68, rows k,i,j stored j,k,i 56; in the body without the pointer 259-269; with the pointer rows k,i,j stored j,k,i 63, stored i,j,k 53; shared with the roll axis
// 232 (1326), 62, 95; forward.i through the command 227 (1326); speed at function scope 194; plain rows through the pointer 263-265; rows through a side pointer 136; forward pointer before the side fill 190, at the function top 44 (aligned 27: frame homes shift, not taken);
// side filled j,k,i 281; product text flips in the rows and roll 53 (same code); cross rows k,j,i 79, j,i,k 52; one-expression rotated rows 52; cosine first 59; trig inside the roll block 47; v after axis 232; `cosine * v + rotated` 45.
// M3 lane 9 r83 (fast, 0x23d790/0x23d030 stay matched): movement.k assigned before i/j 237 (1329 bytes); the free
// position rows as `camera + speed * movement` 89; the command position stored before the camera state's 125 (1330);
// the pitch clamp as an if/else chain into the local 274 (1326 bytes); the zero distance stored before the offset
// copy 101. Zero XMM3/XMM1 and the side/up/roll allocation remain.
// Round 66 (m3 lane 10, fast): side as an aggregate initializer `{ forward.j, -forward.i, 0.f }` 89 (same code); the up
// rows' products commuted, j row / i row / k row each, 89/89/89 (same code); the projection scale as
// `projection = (1.f - cosine) * projection` 118 (1326 bytes); the pitch sum written controls first 89 (same code).
// Zero XMM3 and the side/up homes remain.
// F363: Shared direction producer owned by camera state, instance/static,
// pointer/reference output and forced/ordinary inline: eight forms 89 each.
// Original state type, helper, both calls and body restored; zero/side/up homes remain.
// @retail 0x23d260
void __stdcall function_23d260(void *state, void *input, void *output)
{
	s_camera_state_23d030 *camera = (s_camera_state_23d030 *)state;
	s_camera_input_23d790 *controls = (s_camera_input_23d790 *)input;
	s_camera_output_23d790 *command = (s_camera_output_23d790 *)output;
	if (controls->active)
	{
		camera->yaw += controls->yaw;
		real pitch = camera->pitch + controls->pitch;
		camera->pitch = pitch < -1.56765485f ? -1.56765485f : pitch > 1.56765485f ? 1.56765485f : pitch;
		if (g_51ec12)
			camera->roll += *(real *)((byte *)input + 0x14);
		else
			camera->roll = 0.f;
	}
	command->timer = 0.3f;
	orbit_direction_23d790(camera, &command->forward);
	vector3f side;
	side.k = 0.f;
	side.i = command->forward.j;
	side.j = -command->forward.i;
	if (normalize_inline(&side) == 0.f)
	{
		side.i = 1.f;
		side.j = side.k = 0.f;
	}
	command->up.k = command->forward.j * side.i - side.j * command->forward.i;
	command->up.j = side.k * command->forward.i - command->forward.k * side.i;
	command->up.i = command->forward.k * side.j - command->forward.j * side.k;
	real sine = (real)sin(camera->roll);
	real cosine = (real)cos(camera->roll);
	{
		vector3f *v = &command->up;
		vector3f const *axis = &command->forward;
		real projection = axis->i * v->i;
		projection += axis->j * v->j;
		projection += axis->k * v->k;
		projection *= 1.f - cosine;
		real cross_i = axis->k * v->j - v->k * axis->j;
		real cross_j = axis->i * v->k - axis->k * v->i;
		real cross_k = axis->j * v->i - axis->i * v->j;
		real rotated_i = axis->i * projection;
		rotated_i += cosine * v->i;
		rotated_i -= cross_i * sine;
		v->i = rotated_i;
		real rotated_j = axis->j * projection;
		rotated_j += cosine * v->j;
		rotated_j -= cross_j * sine;
		v->j = rotated_j;
		real rotated_k = axis->k * projection;
		rotated_k += cosine * v->k;
		rotated_k -= cross_k * sine;
		v->k = rotated_k;
	}
	vector3f movement;
	movement.i = (real)(cos(camera->yaw) * *(real *)((byte *)input + 0x18) - sin(camera->yaw) * *(real *)((byte *)input + 0x1c));
	movement.j = (real)(sin(camera->yaw) * *(real *)((byte *)input + 0x18) + cos(camera->yaw) * *(real *)((byte *)input + 0x1c));
	movement.k = *(real *)((byte *)input + 0x20);
	point3f position;
	if (g_470a28 != NONE)
	{
		transform4x3f matrix;
		function_16c6f0(g_470a28, &matrix);
		real x = movement.i;
		real y = movement.j;
		real z = movement.k;
		if (matrix.scale != 1.f)
		{
			real inverse = 1.f / matrix.scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		vector3f local;
		local.i = matrix.forward.k * z + matrix.forward.j * y + matrix.forward.i * x;
		local.j = matrix.left.k * z + matrix.left.j * y + matrix.left.i * x;
		local.k = matrix.up.k * z + matrix.up.j * y + matrix.up.i * x;
		real speed = g_470a24;
		g_51ec30.x += speed * local.i;
		g_51ec30.y += speed * local.j;
		g_51ec30.z += speed * local.k;
		x = g_51ec30.x;
		y = g_51ec30.y;
		z = g_51ec30.z;
		if (matrix.scale != 1.f)
		{
			x = matrix.scale * g_51ec30.x;
			y = matrix.scale * g_51ec30.y;
			z = matrix.scale * g_51ec30.z;
		}
		position.x = matrix.up.i * z + matrix.left.i * y + matrix.forward.i * x + matrix.position.x;
		position.y = matrix.up.j * z + matrix.left.j * y + matrix.forward.j * x + matrix.position.y;
		position.z = matrix.up.k * z + matrix.left.k * y + matrix.forward.k * x + matrix.position.z;
	}
	else
	{
		position.x = g_470a24 * movement.i + camera->position.x;
		position.y = g_470a24 * movement.j + camera->position.y;
		position.z = g_470a24 * movement.k + camera->position.z;
	}
	camera->position = position;
	command->position = position;
	command->offset = *g_4687a4;
	command->distance = 0.f;
	command->field_of_view = g_54e854;
	command->flags = 1;
	if (g_51ec11 > 0)
	{
		command->timer = 0.f;
		command->flags = 9;
		g_51ec11--;
	}
	function_172520((s_observer_command *)command);
}

/* makes the kept point relative to the object (to the world for none) */
// @retail 0x23d030
void function_23d030(long object_index)
{
	point3f *point = g_51ec28;

	g_470a28 = object_index;
	if (point)
	{
		transform4x3f matrix;

		function_16c6f0(object_index, &matrix);
		if (object_index != NONE)
		{
			function_142700(&matrix, point, &g_51ec30);
		}
		else
		{
			g_51ec30 = *g_468788;
		}
	}
}
