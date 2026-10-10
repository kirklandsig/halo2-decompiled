// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_16BD10.CPP: the scripted camera (0x16bd10..0x16c6ea): the
   velocity profile of a camera pan, cutscene camera points, animated
   cameras. The state lives at g_510c6c (unknown_1552e0.cpp); its mode
   picks which view of the bytes at +0x40 is current. unknown_16bc40.cpp
   holds three more functions of this file (0x16c2b0, 0x16c6f0, 0x16c740). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"

/* a camera pan's velocity profile: it accelerates from the start rate to
   the peak, holds it, then moves toward the end rate (0x18 bytes) */
struct s_camera_velocity_profile
{
	real start_rate;
	real peak_rate;
	real acceleration;
	real deceleration;
	short acceleration_ticks;
	short deceleration_ticks;
	short total_ticks;
	bool valid;
};

enum
{
	_camera_scripting_mode_none = 0,
	_camera_scripting_mode_point,
	_camera_scripting_mode_pan,
	_camera_scripting_mode_animation,
	_camera_scripting_mode_first_person
};

/* the state of each mode, at +0x40 of the scripted camera */
struct s_camera_point_state
{
	short point_index;
	short type;
	vector3f offset;
	vector3f left;
};

struct s_camera_pan_state
{
	point3f position;
	quaternionf rotation;
	long start_time;
	s_camera_velocity_profile profile;
};

struct s_camera_animation_state
{
	long start_time;
	long graph_tag_index;
	long animation_name;
	short cutscene_flag_index;
};

/* the scripted camera (g_510c6c, 0x78 bytes) */
struct s_camera_scripting_state
{
	byte unknown00;
	bool active;
	short mode;
	real field_of_view_time;
	real field_of_view_duration;
	real field_of_view_start;
	real field_of_view_target;
	long ticks;
	point3f position;
	vector3f forward;
	vector3f up;
	long object_index;
	union
	{
		s_camera_point_state point;
		s_camera_pan_state pan;
		s_camera_animation_state animation;
	};
};

struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
#define camera_scripting_state ((s_camera_scripting_state *)g_510c6c)

/* the scenario's cutscene camera points (0x40 bytes) and flags (0x38 bytes) */
struct s_cutscene_camera_point
{
	short flags;
	short type;
	byte unknown04[0x28 - 0x4];
	point3f position;
	real orientation[3];
};

struct s_cutscene_flag
{
	byte unknown00[0x24];
	point3f position;
	real facing[2];
};

struct s_camera_scenario_view
{
	byte unknown000[0x1e0];
	long cutscene_flag_count;
	s_cutscene_flag *cutscene_flags;
	long camera_point_count;
	s_cutscene_camera_point *camera_points;
};

#define camera_scenario ((s_camera_scenario_view *)g_4e0350)

void function_141ce0(real a, real b, real c, transform4x3f *out);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);
vector3f *function_142640(transform4x3f const *matrix, vector3f const *vector, vector3f *out);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward,
	vector3f const *up);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
vector3f *function_11d090(vector3f const *v, vector3f *out);
void function_11df60(vector3f const *rotation, vector3f *forward, vector3f *up);
real function_30bf0(vector3f *v);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_11b930(long unit_index);
void function_16c6f0(long object_index, transform4x3f *matrix);
long function_16c2b0(void);
s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id);
extern point3f *g_468788;
c_type_709360 function_1dd0b0(s_graph_tag *graph, long name);
void function_3f660(transform4x3f const *matrix);
void function_1554b0(real elapsed);
void __stdcall function_16f280(real dt);

static inline long camera_seconds_to_ticks_round(real seconds)
{
	real ticks_real = (real)g_510c54->field_2_3 * seconds;
	long ticks;
	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

static inline void cross3f(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = a->j * b->k - a->k * b->j;
	out->j = a->k * b->i - a->i * b->k;
	out->k = a->i * b->j - a->j * b->i;
}

// @retail 0x16bd10
bool camera_velocity_profile_new(s_camera_velocity_profile *profile, real total_seconds, real acceleration_seconds,
	real deceleration_seconds, real start_rate, real end_rate)
{
	short total_ticks = (short)camera_seconds_to_ticks_round(total_seconds);
	short acceleration_ticks = (short)camera_seconds_to_ticks_round(acceleration_seconds);
	short deceleration_ticks = (short)camera_seconds_to_ticks_round(deceleration_seconds);
	bool result = false;

	profile->valid = false;
	if (total_ticks > 0)
	{
		real distance = (start_rate - 1.0f) * (real)acceleration_ticks + (real)(total_ticks * 2) +
			(real)deceleration_ticks * (end_rate - 1.0f);

		if (distance > 0.001f)
		{
			real peak_rate = 2.0f / distance;

			profile->peak_rate = peak_rate;
			profile->start_rate = peak_rate * start_rate;
			if (acceleration_ticks > 0)
			{
				profile->acceleration = (peak_rate - profile->start_rate) / ((real)acceleration_ticks * 2.0f);
			}
			else
			{
				profile->acceleration = 0.0f;
			}
			if (deceleration_ticks > 0)
			{
				profile->deceleration = peak_rate * (end_rate - 1.0f) / ((real)deceleration_ticks * 2.0f);
			}
			else
			{
				profile->deceleration = 0.0f;
			}
			result = true;
			profile->acceleration_ticks = acceleration_ticks;
			profile->deceleration_ticks = deceleration_ticks;
			profile->total_ticks = total_ticks;
			profile->valid = result;
		}
	}

	return result;
}

// @retail 0x16be50
real camera_velocity_profile_evaluate(s_camera_velocity_profile const *profile, long ticks)
{
	real result = 1.0f;

	if (profile->valid)
	{
		real rates[3];
		real accelerations[3];
		long acceleration_ticks;
		long hold_ticks;
		long deceleration_ticks;

		rates[0] = profile->start_rate;
		rates[1] = profile->peak_rate;
		rates[2] = profile->peak_rate;
		accelerations[0] = profile->acceleration;
		accelerations[1] = 0.0f;
		accelerations[2] = profile->deceleration;

		if (ticks < 0)
		{
			acceleration_ticks = 0;
		}
		else if (ticks > profile->acceleration_ticks)
		{
			acceleration_ticks = profile->acceleration_ticks;
		}
		else
		{
			acceleration_ticks = ticks;
		}

		if (ticks - profile->acceleration_ticks < 0)
		{
			hold_ticks = 0;
		}
		else if (ticks - profile->acceleration_ticks >
			profile->total_ticks - profile->deceleration_ticks - profile->acceleration_ticks)
		{
			hold_ticks = profile->total_ticks - profile->deceleration_ticks - profile->acceleration_ticks;
		}
		else
		{
			hold_ticks = ticks - profile->acceleration_ticks;
		}

		if (ticks - (profile->total_ticks - profile->deceleration_ticks) < 0)
		{
			deceleration_ticks = 0;
		}
		else if (ticks - (profile->total_ticks - profile->deceleration_ticks) > profile->deceleration_ticks)
		{
			deceleration_ticks = profile->deceleration_ticks;
		}
		else
		{
			deceleration_ticks = ticks - (profile->total_ticks - profile->deceleration_ticks);
		}

		{
			real a = (real)acceleration_ticks;
			real b = (real)hold_ticks;
			real c = (real)deceleration_ticks;

			result = c * c * accelerations[2] + a * a * accelerations[0] + c * rates[2] + b * rates[1] + a * rates[0] +
				b * b * accelerations[1];
		}
		if (0.0f > result)
		{
			result = 0.0f;
		}
		else if (result > 1.0f)
		{
			result = 1.0f;
		}
	}

	return result;
}

// @retail 0x16bf70
void function_16bf70(long animation_graph_index, long animation_name, long unit_index, short cutscene_flag_index)
{
	if (animation_graph_index != NONE)
	{
		s_animation_state state;
		c_type_709360 animation_id;

		state.initialize(animation_graph_index, NONE, true);
		animation_id = function_1dd0b0(graph_tag_get(state.graph_tag_index), animation_name);
		if (animation_id.index != NONE)
		{
			s_animation *animation = function_1daea0(graph_tag_get(state.graph_tag_index), animation_id);

			if (animation)
			{
				s_camera_scripting_state *camera = camera_scripting_state;

				camera->mode = _camera_scripting_mode_animation;
				camera->active = true;
				camera->animation.cutscene_flag_index = cutscene_flag_index;
				camera->animation.graph_tag_index = animation_graph_index;
				camera->animation.animation_name = animation_name;
				camera->animation.start_time = g_510c54->game_time;
				camera->ticks = camera_seconds_to_ticks_round(((real)animation->frame_count - 1.0f) * (1.0f / 30.0f));
				if (unit_index != NONE && function_badc0(unit_index, 3) && function_11b930(unit_index))
				{
					camera->object_index = unit_index;
				}
				else
				{
					camera->object_index = NONE;
				}
			}
		}
		state.channels_clear_partial();
	}
}

// @retail 0x16c0b0
void function_16c0b0(short camera_point_index)
{
	s_camera_scenario_view *scenario = camera_scenario;

	if (camera_point_index >= 0 && camera_point_index < scenario->camera_point_count)
	{
		s_cutscene_camera_point *point = &scenario->camera_points[camera_point_index];
		transform4x3f matrix;

		function_141ce0(point->orientation[0], point->orientation[1], point->orientation[2], &matrix);
		matrix.position = point->position;
		function_3f660(&matrix);
	}
}

// @retail 0x16c2f0
void function_16c2f0(short camera_point_index, short ticks, long object_index)
{
	s_camera_scenario_view *scenario = camera_scenario;

	if (camera_point_index >= 0 && camera_point_index < scenario->camera_point_count)
	{
		s_cutscene_camera_point *point = &scenario->camera_points[camera_point_index];
		s_camera_scripting_state *camera = camera_scripting_state;
		real seconds;

		camera->mode = _camera_scripting_mode_point;
		camera->active = true;
		seconds = (real)ticks * (1.0f / 30.0f);
		camera->point.type = point->type;
		camera->point.point_index = camera_point_index;
		camera->position = point->position;
		function_11df60((vector3f const *)point->orientation, &camera->forward, &camera->up);
		camera->ticks = camera_seconds_to_ticks_round(seconds);
		if (object_index != NONE)
		{
			transform4x3f matrix;

			switch (camera->point.type)
			{
			case 0:
				break;
			case 1:
				function_16c6f0(object_index, &matrix);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				function_142640(&matrix, (vector3f *)&camera->position, (vector3f *)&camera->position);
				break;
			case 2:
				function_16c6f0(object_index, &matrix);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				function_142640(&matrix, (vector3f *)&camera->position, (vector3f *)&camera->position);
				camera->point.offset = *(vector3f *)&matrix.position;
				cross3f(&camera->forward, &camera->up, &camera->point.left);
				function_30bf0(&camera->point.left);
				break;
			case 3:
				function_16c6f0(object_index, &matrix);
				transform4x3f_apply_point(&matrix, &camera->position, &camera->position);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				object_index = NONE;
				break;
			default:
				__assume(0);
			}
		}
		camera->object_index = object_index;
		function_1554b0(0);
		function_16f280(0.0001f);
	}
}

// @retail 0x16c4f0
void function_16c4f0(short camera_point_index, short ticks)
{
	if (ticks > 0)
	{
		s_cutscene_camera_point *point = &camera_scenario->camera_points[camera_point_index];
		s_camera_scripting_state *camera = camera_scripting_state;
		s_camera_pan_state *pan;
		transform4x3f matrix;
		matrix3x3 rotation;

		camera->mode = _camera_scripting_mode_pan;
		camera->active = true;
		pan = &camera->pan;
		pan->position = point->position;
		function_141ce0(point->orientation[0], point->orientation[1], point->orientation[2], &matrix);
		rotation.forward = matrix.forward;
		rotation.up = matrix.up;
		rotation.left = matrix.left;
		function_141f60(&rotation, &pan->rotation);
		pan->start_time = g_510c54->game_time;
		camera_velocity_profile_new(&pan->profile, (real)ticks * (1.0f / 30.0f), 0.0f, 0.0f, 1.0f, 1.0f);
	}
	else
	{
		function_16c2f0(camera_point_index, ticks, NONE);
	}
}

// @retail 0x16c5f0
void function_16c5f0(short camera_point_index, short target_point_index, short ticks, short acceleration_ticks,
	real start_rate, short deceleration_ticks, real end_rate)
{
	s_camera_scripting_state *camera;

	function_16c2f0(camera_point_index, 0, NONE);
	camera = camera_scripting_state;
	*(volatile short *)&camera->mode = _camera_scripting_mode_pan;
	*(volatile bool *)&camera->active = true;
	if (camera_velocity_profile_new(&camera->pan.profile, (real)ticks * (1.0f / 30.0f),
		(real)acceleration_ticks * (1.0f / 30.0f), (real)deceleration_ticks * (1.0f / 30.0f), start_rate, end_rate))
	{
		s_cutscene_camera_point *point = &camera_scenario->camera_points[target_point_index];
		transform4x3f matrix;

		camera->mode = _camera_scripting_mode_pan;
		camera->active = true;
		s_camera_pan_state *pan = &camera->pan;

		pan->position = point->position;
		function_141ce0(point->orientation[0], point->orientation[1], point->orientation[2], &matrix);
		function_141f60(&matrix.rotation, &pan->rotation);
		pan->start_time = g_510c54->game_time;
	}
	else
	{
		function_16c4f0(target_point_index, ticks);
	}
}

/* the objects' header data and the tags, as 0x16c110 reads them */
struct s_camera_object
{
	long definition_index;
	byte unknown004[0x116 - 0x4];
	short node_matrices_offset;
};

struct s_camera_object_header
{
	byte unknown00[8];
	s_camera_object *object;
};

static inline void *camera_tag_get(long tag_index)
{
	return g_4e3b44[tag_index & 0xffff].bytes;
}

// @retail 0x16c110
void function_16c110(long animation_graph_index, long animation_name, long object_index, short cutscene_flag_index,
	long frame)
{
	if (animation_graph_index != NONE)
	{
		s_animation_state state;
		c_type_709360 animation_id;

		state.initialize(animation_graph_index, NONE, true);
		animation_id = function_1dd0b0(graph_tag_get(state.graph_tag_index), animation_name);
		if (animation_id.index != NONE)
		{
			transform4x3f matrix;
			transform4x3f transform;

			state.animation_matrix_get(animation_id, (real)frame * (1.0f / 30.0f), 0, &matrix);
			if (object_index != NONE)
			{
				s_camera_object *object = ((s_camera_object_header *)g_4e0300->data)[object_index & 0xffff].object;
				byte *object_definition = (byte *)camera_tag_get(object->definition_index);
				byte *model = (byte *)camera_tag_get(*(long *)(object_definition + 0x38));
				byte *render_model = (byte *)camera_tag_get(*(long *)(model + 0x4));
				byte *nodes = *(byte **)(render_model + 0x4c);

				function_142a60((transform4x3f *)((byte *)object + object->node_matrices_offset),
					(transform4x3f *)(nodes + 0x28), &transform);
				function_142a60(&transform, &matrix, &matrix);
			}
			if (cutscene_flag_index != NONE)
			{
				s_cutscene_flag *flag = &camera_scenario->cutscene_flags[cutscene_flag_index];
				vector3f forward;
				vector3f up;

				forward.i = (real)cos(flag->facing[0]) * (real)cos(flag->facing[1]);
				forward.j = (real)sin(flag->facing[0]) * (real)cos(flag->facing[1]);
				forward.k = (real)sin(flag->facing[1]);
				function_1420f0(&transform, &flag->position, &forward, function_11d090(&forward, &up));
				function_142a60(&transform, &matrix, &matrix);
			}
			function_3f660(&matrix);
		}
		state.channels_clear_partial();
	}
}

/* a unit's animation, as 0x16cfa0 reads it (at the offset in +0x12a) */
struct s_camera_unit_animation
{
	long graph_tag_index;
	short unknown04;
	short animation_index;
	byte unknown08[0x1c - 0x8];
	real frame;
};

struct s_camera_unit
{
	byte unknown000[0x12a];
	short animation_offset;
};

struct s_camera_unit_header
{
	byte unknown00[8];
	s_camera_unit *unit;
};

bool function_172350(point3f const *point);

/* the animated camera's matrix and the seconds it is into its animation */
// @retail 0x16cfa0
bool camera_scripting_animation_matrix_get(transform4x3f *matrix, real *seconds_out)
{
	bool result = false;
	s_camera_animation_state *camera_animation = &camera_scripting_state->animation;
	s_animation_state state;

	state.initialize(camera_animation->graph_tag_index, NONE, true);
	c_type_709360 animation_id = function_1dd0b0(graph_tag_get(state.graph_tag_index), camera_animation->animation_name);
	if (animation_id.index != NONE)
	{
		long unit_index;
		real seconds = 0.0f;
		transform4x3f animation_matrix;

		function_1daea0(graph_tag_get(state.graph_tag_index), animation_id);
		unit_index = camera_scripting_state->object_index;
		if (unit_index != NONE && function_badc0(unit_index, 3) && function_11b930(unit_index))
		{
			s_camera_unit *unit = ((s_camera_unit_header *)g_4e0300->data)[unit_index & 0xffff].unit;
			s_camera_unit_animation *unit_animation = (s_camera_unit_animation *)((byte *)unit + unit->animation_offset);

			if (unit_animation->graph_tag_index != NONE && unit_animation->animation_index != NONE)
			{
				seconds = unit_animation->frame * (1.0f / 30.0f);
			}
		}
		else
		{
			seconds = (real)(g_510c54->game_time - camera_animation->start_time) * g_510c54->rate;
		}
		*seconds_out = seconds;
		state.animation_matrix_get(animation_id, seconds, 0, &animation_matrix);
		if (!function_172350(&animation_matrix.position))
		{
			animation_matrix.position = *g_468788;
		}
		if (camera_animation->cutscene_flag_index != NONE)
		{
			s_cutscene_flag *flag = &camera_scenario->cutscene_flags[camera_animation->cutscene_flag_index];
			vector3f forward;
			vector3f up;
			transform4x3f flag_matrix;

			forward.i = (real)cos(flag->facing[0]) * (real)cos(flag->facing[1]);
			forward.j = (real)sin(flag->facing[0]) * (real)cos(flag->facing[1]);
			forward.k = (real)sin(flag->facing[1]);
			function_1420f0(&flag_matrix, &flag->position, &forward, function_11d090(&forward, &up));
			function_142a60(&flag_matrix, &animation_matrix, &animation_matrix);
		}
		*matrix = animation_matrix;
		result = true;
	}
	state.channels_clear_partial();

	return result;
}

struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
long structure_leaf_cluster_get(long leaf_index);
extern s_bsp3d *g_4e033c;
void function_23bc90(long object_index, point3f *position, vector3f *forward);

struct s_camera_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_camera_leaves_view
{
	byte unknown00[0x30];
	s_camera_leaf *leaves;
};

static __forceinline long camera_cluster_from_point(point3f *point)
{
	long leaf_index = function_14a280(g_4e033c, 0, point);
	long result;

	if (leaf_index != NONE)
	{
		result = ((s_camera_leaves_view *)g_4e0348)->leaves[leaf_index].cluster_index;
	}
	else
	{
		result = NONE;
	}
	return result;
}

/* the cluster of the structure bsp the scripted camera is in */
// @retail 0x16cee0
long camera_scripting_cluster_get(void)
{
	long mode = camera_scripting_state->mode;
	s_camera_scripting_state *camera = camera_scripting_state;
	long result = NONE;

	switch (mode)
	{
	case _camera_scripting_mode_point:
		result = camera_cluster_from_point(&camera->position);
		break;
	case _camera_scripting_mode_pan:
		result = camera_cluster_from_point(&camera->pan.position);
		break;
	case _camera_scripting_mode_animation:
		{
			transform4x3f matrix;
			real seconds;

			if (camera_scripting_animation_matrix_get(&matrix, &seconds))
			{
				result = structure_leaf_cluster_get(function_14a280(g_4e033c, 0, &matrix.position));
			}
		}
		break;
	case _camera_scripting_mode_first_person:
		{
			long object_index = function_16c2b0();

			if (object_index != NONE)
			{
				point3f position;
				vector3f forward;

				function_23bc90(object_index, &position, &forward);
				result = structure_leaf_cluster_get(function_14a280(g_4e033c, 0, &position));
			}
		}
		break;
	}
	return result;
}

/* the observer's command a camera director fills in (lane R's observer.cpp),
   as written here */
struct s_observer_command
{
	dword flags;
	point3f position;
	vector3f offset;
	byte unknown1c[0x24 - 0x1c];
	real distance;
	real field_of_view;
	vector3f forward;
	vector3f up;
	byte unknown44[0x50 - 0x44];
	transform4x3f object_matrix;
	long object_index;
	real timer;
	byte unknown8c[4];
	byte unknown90;
	byte unknown91[0xa4 - 0x91];
	real unknowna4;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
extern real g_54e854;

/* the field of view interpolation's switch (g_510c50) */
struct s_camera_field_of_view_flags
{
	byte unknown0[5];
	bool active;
};

matrix3x3 *function_142d10(vector3f const *up, vector3f const *forward, matrix3x3 *out);
void function_11da10(quaternionf const *a, quaternionf const *b, quaternionf *out, real t);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
void function_23c0e0(long object_index, s_observer_command *command);
void function_172520(s_observer_command *command);

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the camera director of scripted cameras: fills the observer's command */
// @retail 0x16c840
void __stdcall function_16c840(long user_index, long unused, s_observer_command *command)
{
	s_game_time_globals *game_time = g_510c54;
	real time_scale = game_time->scale;
	s_camera_scripting_state *camera;
	real field_of_view;

	command->flags = 8;
	if (game_time->active && game_time->unknown01)
	{
		command->flags = 0x28;
	}
	else
	{
		command->flags = 8;
	}

	camera = camera_scripting_state;
	switch (camera->mode)
	{
	case _camera_scripting_mode_point:
		{
			long object_index = camera->object_index;

			if (object_index != NONE)
			{
				if (!function_badc0(object_index, NONE))
				{
					break;
				}
				function_16c6f0(object_index, &command->object_matrix);
				switch (camera->point.type)
				{
				case 2:
					{
						vector3f left;
						real position_distance;
						real length_squared;
						real t;

						command->object_matrix.forward = *g_4687a8;
						command->object_matrix.up = *g_4687b0;
						command->object_matrix.left = *g_4687ac;
						left = camera->point.left;
						position_distance = dot3f((vector3f *)&command->object_matrix.position, &left);
						length_squared = dot3f(&left, &left);
						if (!(length_squared != 0.0f))
						{
							t = 0.0f;
						}
						else
						{
							t = 0.0f - (dot3f(&camera->point.offset, &left) - position_distance) / length_squared;
						}
						command->object_matrix.position.x = camera->point.left.i * t + camera->point.offset.i;
						command->object_matrix.position.y = camera->point.left.j * t + camera->point.offset.j;
						command->object_matrix.position.z = camera->point.left.k * t + camera->point.offset.k;
						command->flags |= 0x40;
					}
					break;
				case 1:
					command->object_matrix.forward = *g_4687a8;
					command->object_matrix.up = *g_4687b0;
					command->object_matrix.left = *g_4687ac;
				default:
					command->flags |= 0x40;
					break;
				}
			}
			if (time_scale != 0.0f)
			{
				command->timer = (real)camera->ticks * game_time->rate / time_scale;
			}
			else
			{
				command->timer = 0.0f;
			}
			command->forward = camera->forward;
			command->up = camera->up;
			if (camera->object_index != NONE)
			{
				real yaw = (real)atan2(command->forward.j, command->forward.i);
				real distance = dot3f(&command->forward, (vector3f *)&camera->position);
				real sine;
				real cosine;
				vector3f offset;

				if (distance > 0.0f)
				{
					distance = 0.0f;
				}
				sine = (real)sin(yaw);
				command->position = *g_468788;
				offset.i = 0.0f - command->forward.i * distance;
				command->distance = 0.0f - distance;
				offset.j = 0.0f - command->forward.j * distance;
				offset.k = 0.0f - command->forward.k * distance;
				command->offset.k = offset.k;
				cosine = (real)cos(yaw);
				command->offset.i = cosine * offset.i + sine * offset.j;
				command->offset.j = sine * offset.i - cosine * offset.j;
				command->object_index = camera->object_index;
			}
			else
			{
				command->object_index = NONE;
			}
			command->position = camera->position;
			command->flags |= 1;
		}
		break;
	case _camera_scripting_mode_pan:
		{
			real t = camera_velocity_profile_evaluate(&camera->pan.profile, g_510c54->game_time - camera->pan.start_time);
			quaternionf rotation;
			matrix3x3 matrix;
			real length_squared;

			command->timer = 0.0f;
			t = PIN(t, 0.0f, 1.0f);
			function_141f60(function_142d10(&camera->up, &camera->forward, &matrix), &rotation);
			function_11da10(&rotation, &camera->pan.rotation, &rotation, t);
			length_squared = rotation.i * rotation.i + rotation.j * rotation.j + rotation.k * rotation.k + rotation.w * rotation.w;
			if (length_squared > 0.0f)
			{
				real inverse = 1.0f / (real)sqrt(length_squared);

				rotation.i *= inverse;
				rotation.j *= inverse;
				rotation.k *= inverse;
				rotation.w *= inverse;
			}
			else
			{
				rotation.i = 0.0f;
				rotation.j = 0.0f;
				rotation.k = 0.0f;
				rotation.w = 1.0f;
			}
			function_141e10(&matrix, &rotation);
			command->forward = matrix.forward;
			command->up = matrix.up;
			t = PIN(t, 0.0f, 1.0f);
			camera = camera_scripting_state;
			command->position.x = camera->position.x * (1.0f - t) + camera->pan.position.x * t;
			command->position.y = camera->position.y * (1.0f - t) + camera->pan.position.y * t;
			command->position.z = camera->position.z * (1.0f - t) + camera->pan.position.z * t;
			command->flags |= 1;
		}
		break;
	case _camera_scripting_mode_animation:
		{
			transform4x3f matrix;
			real seconds;

			if (camera_scripting_animation_matrix_get(&matrix, &seconds))
			{
				command->forward = matrix.forward;
				command->up = matrix.up;
				command->position = matrix.position;
				command->flags |= 0x19;
				command->distance = 0.0f;
				command->timer = 0.0f;
				command->object_index = NONE;
			}
		}
		break;
	case _camera_scripting_mode_first_person:
		{
			long object_index = function_16c2b0();

			if (object_index != NONE)
			{
				function_23c0e0(object_index, command);
			}
		}
		break;
	}

	camera = camera_scripting_state;
	field_of_view = g_54e854;
	camera->active = false;
	if (g_510c50 && ((s_camera_field_of_view_flags *)g_510c50)->active && camera->field_of_view_target != 0.0f)
	{
		if (camera->field_of_view_time >= camera->field_of_view_duration)
		{
			field_of_view = camera->field_of_view_target;
		}
		else
		{
			real t = camera->field_of_view_time / camera->field_of_view_duration;

			t = PIN(t, 0.0f, 1.0f);
			field_of_view = (camera->field_of_view_target - camera->field_of_view_start) * t + camera->field_of_view_start;
		}
	}
	command->field_of_view = field_of_view;
	command->unknown90 = 3;
	command->unknowna4 = 0.0f;
	function_172520(command);
}
