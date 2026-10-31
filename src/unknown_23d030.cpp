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

struct s_orbit_state_23d790
{
	real unknown00;
	real distance;
	real unknown08;
	real yaw;
	real pitch;
};

PRIVATE __forceinline void orbit_direction_23d790(s_orbit_state_23d790 const *state, vector3f *direction)
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
	s_orbit_state_23d790 *values = (s_orbit_state_23d790 *)state;
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

struct s_free_state_23d260
{
	point3f position;
	real yaw;
	real pitch;
	real roll;
};
// score 102 (was 102): this round's side-vector assignment orders (k/j/i, j/i/k, i/j/k, j/k/i) and a
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
// @retail 0x23d260
void __stdcall function_23d260(void *state, void *input, void *output)
{
	s_free_state_23d260 *camera = (s_free_state_23d260 *)state;
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
	real pitch_cosine = (real)cos(camera->pitch);
	command->forward.i = (real)cos(camera->yaw) * pitch_cosine;
	command->forward.j = (real)sin(camera->yaw) * pitch_cosine;
	command->forward.k = (real)sin(camera->pitch);
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
