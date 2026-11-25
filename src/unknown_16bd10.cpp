// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_16BD10.CPP: the scripted camera (0x16bd10..0x16c6ea): the
   velocity profile of a camera pan, cutscene camera points, animated
   cameras. The state lives at g_510c6c (unknown_1552e0.cpp); its mode
   picks which view of the bytes at +0x40 is current. unknown_16bc40.cpp
   holds three more functions of this file (0x16c2b0, 0x16c6f0, 0x16c740). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

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
void function_3f660(transform4x3f const *matrix);
void function_1554b0(real elapsed);
void __stdcall function_16f280(real dt);

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

static inline long camera_seconds_to_ticks_round(real seconds)
{
	return real_to_long((real)g_510c54->field_2_3 * seconds);
}

/* the cross product a x b, its components computed before the stores (the shared
   cross3f of vector_products.h stores each as it goes) */
static inline void camera_cross3f(vector3f const *a, vector3f const *b, vector3f *out)
{
	real k = a->i * b->j - a->j * b->i;
	real j = a->k * b->i - a->i * b->k;
	real i = b->k * a->j - b->j * a->k;

	out->i = i;
	out->j = j;
	out->k = k;
}

// @retail 0x16bd10
bool camera_velocity_profile_new(s_camera_velocity_profile *profile, real total_seconds, real acceleration_seconds,
	real deceleration_seconds, real start_rate, real end_rate)
{
	bool result = false;
	short total_ticks = (short)camera_seconds_to_ticks_round(total_seconds);
	short acceleration_ticks = (short)camera_seconds_to_ticks_round(acceleration_seconds);
	short deceleration_ticks = (short)camera_seconds_to_ticks_round(deceleration_seconds);

	profile->valid = false;
	if (total_ticks > 0)
	{
		real ramp_down_span = (real)deceleration_ticks;
		real ramp_up_span = (real)acceleration_ticks;

		end_rate -= 1.0f;
		real distance = (start_rate - 1.0f) * ramp_up_span + (real)(total_ticks * 2) + ramp_down_span * end_rate;

		if (distance > 0.001f)
		{
			real peak_rate = 2.0f / distance;

			profile->peak_rate = peak_rate;
			profile->start_rate = peak_rate * start_rate;
			if (acceleration_ticks > 0)
			{
				profile->acceleration = (peak_rate - profile->start_rate) / (ramp_up_span * 2.0f);
			}
			else
			{
				profile->acceleration = 0.0f;
			}
			if (deceleration_ticks > 0)
			{
				profile->deceleration = peak_rate * end_rate / (ramp_down_span * 2.0f);
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

/* the profile's value at a tick: per segment ticks * rate + ticks^2 * acceleration, summed, clamped to [0, 1] */
// @retail 0x16be50
real camera_velocity_profile_evaluate(s_camera_velocity_profile const *profile, long ticks)
{
	real result = 1.0f;

	if (profile->valid)
	{
		vector3f rates;
		vector3f accelerations;
		long acceleration_ticks;
		long hold_ticks;
		long deceleration_ticks;
		real a, b, c;

		rates.i = profile->start_rate;
		rates.j = profile->peak_rate;
		rates.k = profile->peak_rate;
		accelerations.i = profile->acceleration;
		accelerations.j = 0.0f;
		accelerations.k = profile->deceleration;

		acceleration_ticks = PIN(ticks, 0, profile->acceleration_ticks);
		a = (real)acceleration_ticks;
		hold_ticks = PIN(ticks - profile->acceleration_ticks, 0,
			profile->total_ticks - profile->deceleration_ticks - profile->acceleration_ticks);
		b = (real)hold_ticks;
		deceleration_ticks = PIN(ticks - (profile->total_ticks - profile->deceleration_ticks), 0,
			profile->deceleration_ticks);
		c = (real)deceleration_ticks;
		{
			vector3f times = { a, b, c };
			vector3f squares = { a * a, b * b, c * c };
			result = dot3f(&squares, &accelerations) + dot3f(&times, &rates);
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

		state.initialize(animation_graph_index, NONE, true);
		c_type_709360 id = graph_tag_get(state.graph_tag_index)->animation_by_name(animation_name);
		if (id.index != NONE)
		{
			s_animation *animation = function_1daea0(graph_tag_get(state.graph_tag_index), id);

			if (animation)
			{
				s_camera_scripting_state *camera = camera_scripting_state;

				camera->mode = _camera_scripting_mode_animation;
				camera->active = true;
				short frame_count = animation->frame_count;
				camera->animation.cutscene_flag_index = cutscene_flag_index;
				camera->animation.graph_tag_index = animation_graph_index;
				camera->animation.animation_name = animation_name;
				camera->animation.start_time = g_510c54->game_time;
				camera->ticks = camera_seconds_to_ticks_round(((real)frame_count - 1.0f) * (1.0f / 30.0f));
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
		long point_type = point->type;
		s_camera_point_state *state = &camera->point;
		camera->point.type = (short)point_type;
		state->point_index = camera_point_index;
		camera->position = point->position;
		function_11df60((vector3f const *)point->orientation, &camera->forward, &camera->up);
		camera->ticks = camera_seconds_to_ticks_round(seconds);
		if (object_index != NONE)
		{
			transform4x3f matrix;

			point_type = camera->point.type;
			switch (point_type)
			{
			case 0:
				break;
			case 1:
				function_16c6f0(object_index, &matrix);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				function_142640(&matrix, (vector3f *)&camera->position, (vector3f *)&camera->position);
				break;
			case 3:
				function_16c6f0(object_index, &matrix);
				transform4x3f_apply_point(&matrix, &camera->position, &camera->position);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				object_index = NONE;
				break;
			case 2:
				function_16c6f0(object_index, &matrix);
				function_142640(&matrix, &camera->forward, &camera->forward);
				function_142640(&matrix, &camera->up, &camera->up);
				function_142640(&matrix, (vector3f *)&camera->position, (vector3f *)&camera->position);
			{
				/* retail derives the left vector's address from the offset's
				   (left directly follows offset in s_camera_point_state) */
				vector3f *offset = &camera->point.offset;
				vector3f *left;

				*offset = *(vector3f *)&matrix.position;
				left = offset + 1;
				vector3f const *up = &camera->up;
				vector3f const *forward = &camera->forward;
				camera_cross3f(forward, up, left);
				function_30bf0(left);
			}
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

		state.initialize(animation_graph_index, NONE, true);
		c_type_709360 animation_id = graph_tag_get(state.graph_tag_index)->animation_by_name(animation_name);
		if (animation_id.index != NONE)
		{
			transform4x3f matrix;
			transform4x3f transform;

			/* retail keeps object_index on the stack: reading it through its
			   address keeps it there */
			long const *object_index_reference = &object_index;

			state.animation_matrix_get(animation_id, (real)frame * (1.0f / 30.0f), 0, &matrix);
			if (*object_index_reference != NONE)
			{
				s_camera_object *object = ((s_camera_object_header *)g_4e0300->data)[*object_index_reference & 0xffff].object;
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
				function_11d090(&forward, &up);
				function_1420f0(&transform, &flag->position, &forward, &up);
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

/* the seconds a unit is into its animation, 0 when it has none */
static inline real camera_unit_animation_seconds(s_camera_unit_animation const *unit_animation)
{
	real result = 0.0f;

	if (unit_animation->graph_tag_index != NONE && unit_animation->animation_index != NONE)
	{
		result = unit_animation->frame * (1.0f / 30.0f);
	}
	return result;
}

/* the animated camera's matrix and the seconds it is into its animation */
// @retail 0x16cfa0
bool camera_scripting_animation_matrix_get(transform4x3f *matrix, real *seconds_out)
{
	bool result = false;
	s_camera_animation_state *camera_animation = &camera_scripting_state->animation;
	s_animation_state state;

	state.initialize(camera_animation->graph_tag_index, NONE, true);
	c_type_709360 animation_id = graph_tag_get(state.graph_tag_index)->animation_by_name(camera_animation->animation_name);
	if (animation_id.index != NONE)
	{
		long unit_index;
		real seconds;
		transform4x3f animation_matrix;

		function_1daea0(graph_tag_get(state.graph_tag_index), animation_id);
		unit_index = camera_scripting_state->object_index;
		if (unit_index != NONE && function_badc0(unit_index, 3) && function_11b930(unit_index))
		{
			s_camera_unit *unit = ((s_camera_unit_header *)g_4e0300->data)[unit_index & 0xffff].unit;
			s_camera_unit_animation *unit_animation = (s_camera_unit_animation *)((byte *)unit + unit->animation_offset);

			seconds = camera_unit_animation_seconds(unit_animation);
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
			function_11d090(&forward, &up);
			function_1420f0(&flag_matrix, &flag->position, &forward, &up);
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

extern real g_54e854;

/* retail's call sets up ECX (up), EDX (forward) and then EAX (out): register arguments are set up last to first */
matrix3x3 *function_142d10(matrix3x3 *out, vector3f const *forward, vector3f const *up);
void function_11da10(quaternionf const *a, quaternionf const *b, quaternionf *out, real t);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
void function_23c0e0(long object_index, s_observer_command *command);
void function_172520(s_observer_command *command);
bool function_13cb40(void);

PRIVATE inline real game_ticks_to_seconds(long ticks)
{
	real seconds = (real)ticks * g_510c54->rate;

	return seconds;
}

/* the camera director of scripted cameras: fills the observer's command */
// @retail 0x16c840
void __stdcall function_16c840(long user_index, long unused, s_observer_command *command)
{
	real time_scale = g_510c54->scale;
	s_camera_scripting_state *camera;
	real field_of_view;

	command->flags = 8;
	if (g_510c54->active && g_510c54->unknown01)
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
						plane3f plane;
						real length_squared;
						real t;

						command->object_matrix.forward = *g_4687a8;
						command->object_matrix.up = *g_4687b0;
						command->object_matrix.left = *g_4687ac;
						plane.n = camera->point.left;
						plane.d = dot3f((vector3f *)&command->object_matrix.position, &plane.n);
						length_squared = dot3f(&plane.n, &camera->point.left);
						if (length_squared != 0.0f)
						{
							t = 0.0f - plane_distance_to_point(&plane, (point3f *)&camera->point.offset) / length_squared;
						}
						else
						{
							t = 0.0f;
						}
						vector3f_add_scaled(&camera->point.offset, &camera->point.left, t, (vector3f *)&command->object_matrix.position);
					}
					break;
				case 1:
					command->object_matrix.forward = *g_4687a8;
					command->object_matrix.up = *g_4687b0;
					command->object_matrix.left = *g_4687ac;
					break;
				}
				command->flags |= 0x40;
			}
			command->timer = time_scale != 0.0f ? game_ticks_to_seconds(camera->ticks) / time_scale : 0.0f;
			command->forward = camera->forward;
			command->up = camera->up;
			if (camera->object_index != NONE)
			{
				real yaw = (real)atan2(command->forward.j, command->forward.i);
				real distance = dot3f((vector3f *)&camera->position, &command->forward);
				real sine;
				real cosine;
				vector3f offset;

				if (distance > 0.0f)
				{
					distance = 0.0f;
				}
				command->position = *g_468788;
				command->distance = 0.0f - distance;
				/* retail forms cosine * offset.i ahead of sine * offset.j and keeps the cosine in
				   the dead argument home (+0x64), the sine at +0x10; the compiler orders the two
				   products by when their factors were first assigned, which comes out that way only
				   with the sine taken after the j row and the k product named ahead of the cosine */
				offset.i = 0.0f - command->forward.i * distance;
				offset.j = 0.0f - command->forward.j * distance;
				sine = (real)sin(yaw);
				real scaled_k = command->forward.k * distance;
				offset.k = 0.0f - scaled_k;
				cosine = (real)cos(yaw);
				command->offset.i = cosine * offset.i + sine * offset.j;
				command->offset.j = sine * offset.i - cosine * offset.j;
				command->offset.k = offset.k;
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

			command->timer = 0.0f;
			t = PIN(t, 0.0f, 1.0f);
			{
				matrix3x3 matrix;

				function_141f60(function_142d10(&matrix, &camera->forward, &camera->up), &rotation);
			}
			function_11da10(&rotation, &camera->pan.rotation, &rotation, t);
			quaternion_unit_or_identity(&rotation);
			transform4x3f transform;

			function_141e10(&transform.rotation, &rotation);
			command->forward = transform.forward;
			command->up = transform.up;
			real fraction = PIN(t, 0.0f, 1.0f);
			camera = camera_scripting_state;
			point3f *position = &command->position;
			position->x = camera->position.x * (1.0f - fraction) + camera->pan.position.x * fraction;
			position->y = camera->position.y * (1.0f - fraction) + camera->pan.position.y * fraction;
			position->z = camera->position.z * (1.0f - fraction) + camera->pan.position.z * fraction;
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
				command->distance = 0.0f;
				command->timer = 0.0f;
				command->flags |= 0x19;
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
	if (function_13cb40() && camera->field_of_view_target != 0.0f)
	{
		if (camera->field_of_view_time >= camera->field_of_view_duration)
		{
			field_of_view = camera->field_of_view_target;
		}
		else
		{
			real t = camera->field_of_view_time / camera->field_of_view_duration;

			t = PIN(t, 0.0f, 1.0f);
			real start = camera->field_of_view_start;

			field_of_view = (camera->field_of_view_target - start) * t + start;
		}
	}
	command->field_of_view = field_of_view;
	command->unknown90 = 3;
	command->unknowna4 = 0.0f;
	function_172520(command);
}
