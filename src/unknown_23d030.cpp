// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_23D030.CPP: the camera that flies freely, and the point it keeps
   relative to an object (0x23d030..0x23d970) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <math.h>

struct s_player_control_camera
{
	long unit_index;
	short seat_index;
	byte unknown06[2];
	void *camera;
	point3f position;
};

struct s_entry_155760
{
	byte unknown00[4];
	real value;
	byte unknown08[4];
	void *proc;
	real value10;
	real value14;
	byte unknown18[0x58 - 0x18];
	word word58;
	byte unknown5a[0x108 - 0x5a];
	byte flag108;
	byte unknown109[3];
	real value10c;
	byte unknown110[0x140 - 0x110];
};

extern s_entry_155760 g_4e8c44[];
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
	side.i = command->forward.j;
	side.j = -command->forward.i;
	side.k = 0.f;
	real length = (real)sqrt(side.j * side.j + side.i * side.i);
	if (0.0001f > fabs(length))
		goto side_default;
	{
		real inverse = 1.f / length;
		if (length == 0.f)
			goto side_default;
		side.i *= inverse;
		side.j *= inverse;
		side.k *= inverse;
		goto side_ready;
	}
side_default:
	side.i = 1.f;
	side.j = side.k = 0.f;
side_ready: 
	command->up.k = command->forward.j * side.i - side.j * command->forward.i;
	command->up.j = side.k * command->forward.i - command->forward.k * side.i;
	command->up.i = command->forward.k * side.j - command->forward.j * side.k;
	real sine = (real)sin(camera->roll);
	real cosine = (real)cos(camera->roll);
	vector3f up;
	up.i = command->up.i;
	up.j = command->up.j;
	up.k = command->up.k;
	vector3f volatile forward;
	forward.i = command->forward.i;
	forward.j = command->forward.j;
	forward.k = command->forward.k;
	real projection = (forward.i * up.i + forward.j * up.j + forward.k * up.k) * (1.f - cosine);
	command->up.i = forward.i * projection + cosine * up.i - (forward.k * up.j - forward.j * up.k) * sine;
	command->up.j = forward.j * projection + cosine * up.j - (forward.i * up.k - forward.k * up.i) * sine;
	command->up.k = forward.k * projection + cosine * up.k - (forward.j * up.i - forward.i * up.j) * sine;
	double yaw_cosine = cos(camera->yaw);
	double yaw_sine = sin(camera->yaw);
	vector3f movement;
	movement.i = (real)(yaw_cosine * *(real *)((byte *)input + 0x18) - yaw_sine * *(real *)((byte *)input + 0x1c));
	movement.j = (real)(yaw_sine * *(real *)((byte *)input + 0x18) + yaw_cosine * *(real *)((byte *)input + 0x1c));
	movement.k = *(real *)((byte *)input + 0x20);
	point3f position;
	if (g_470a28 != NONE)
	{
		transform4x3f matrix;
		function_16c6f0(g_470a28, &matrix);
		if (matrix.scale != 1.f)
		{
			real inverse = 1.f / matrix.scale;
			movement.i *= inverse;
			movement.j *= inverse;
			movement.k *= inverse;
		}
		vector3f local;
		local.i = matrix.forward.k * movement.k + matrix.forward.j * movement.j + matrix.forward.i * movement.i;
		local.j = matrix.left.k * movement.k + matrix.left.j * movement.j + matrix.left.i * movement.i;
		local.k = matrix.up.k * movement.k + matrix.up.j * movement.j + matrix.up.i * movement.i;
		g_51ec30.x += g_470a24 * local.i;
		g_51ec30.y += g_470a24 * local.j;
		g_51ec30.z += g_470a24 * local.k;
		point3f scaled = g_51ec30;
		if (matrix.scale != 1.f)
		{
			scaled.x *= matrix.scale;
			scaled.y *= matrix.scale;
			scaled.z *= matrix.scale;
		}
		position.x = matrix.up.i * scaled.z + matrix.left.i * scaled.y + matrix.forward.i * scaled.x + matrix.position.x;
		position.y = matrix.up.j * scaled.z + matrix.left.j * scaled.y + matrix.forward.j * scaled.x + matrix.position.y;
		position.z = matrix.up.k * scaled.z + matrix.left.k * scaled.y + matrix.forward.k * scaled.x + matrix.position.z;
	}
	else
	{
		position.x = g_470a24 * movement.i + camera->position.x;
		position.y = g_470a24 * movement.j + camera->position.y;
		position.z = g_470a24 * movement.k + camera->position.z;
	}
	camera->position.x = position.x;
	camera->position.y = position.y;
	camera->position.z = position.z;
	command->position.x = position.x;
	command->position.y = position.y;
	command->position.z = position.z;
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
