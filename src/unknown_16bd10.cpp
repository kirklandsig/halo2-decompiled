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

matrix3x3 *function_142d10(vector3f const *up, vector3f const *forward, matrix3x3 *out);
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
// score 331 (was 331; 331 before): m3 lane 8 r164 and m3 lane 2 r79 notes at the end; round 95 (m3 lane 11) before it; round 112 (m3 lane 12) before that. Earlier at 331 (was 385; 385 was 236 before the timer fix): round 63 (end of note) re-applied round 57's lead with shared helpers.
// MiniMax H (11-11): no new form, no lower score (331); retail keeps the point case's normal in XMM3/4/5 with the zero in
// XMM0 and ends case 2 with a read-modify-write `or [ebp], 0x40`, while ours splits the flag updates and keeps the zero in XMM1.
// MiniMax C (11-11): no new form, no lower score (331): the length agrees (1696 bytes); case 2 still
// loads the camera normal's j for the squared length where ours picks the function-wide zero register,
// and the dot3f helper that computes it lives in include/unknown_0259d0.h, which this lane may not
// edit; the point case's normals also still take XMM5/3/4 where retail gives them XMM3/4/5.
// Earlier 331 (was 331; 385 earlier, 385 was 236 before the timer fix): round 63 (end of note) re-applied round 57's lead with shared helpers.
// MiniMax C (11-05): no new form, no lower score; the plane's three normal copies are reloaded in the same order in both (i, k, j), but retail gives them XMM3/4/5 for i/j/k where ours gives XMM5/3/4; the squared length in field order scores 389, in k,i,j order 243, with the first two addends swapped 238, written from the direction alone 465, and with the multiply operands swapped it is byte-identical at 236.
// b5K 11-06 statement walk against retail found no wrong field, offset, condition, callee or loop; the one
// behaviour fix is the point-mode timer, which retail computes as (ticks * rate) / scale (cvtsi2ss, mulss [ebx+4], divss at
// 0x16ca6e-0x16ca78) and the one-line ternary compiled as (rate / scale) * ticks, rounding differently; the `seconds` local makes
// ours retail's order. That costs 149 only because the whole tail then shifts one line against retail's split default arm
// (xorps/or/mov at 0x16ca54); the plain if/else form scores 381. Also seen, compiler-side: the pan block's quaternion length sums
// w,k,j,i in retail (0x16cc2b) and j,k,w,i in ours (the helper's text is w,k,j,i and 0xa9b40 matches with it; not source-fixable here).
// Before the timer change (236): frame 72 agrees; the first real difference is the plane block's direction preload
// (retail d.j, ours d.k) with the normal in XMM3/4/5 for i/j/k (ours XMM5/3/4), then the zero/length roles.
// Earlier: the first difference was put at the switch's dispatch —
// frame 72 agrees and the arms are in retail's order at 1696 bytes (aligned
// difference 123): ours puts the length in xmm0 and the zero in xmm1 where retail has xmm1/xmm0
// (retail preloads direction j as the length accumulator), the normal's i/j/k in xmm5/xmm3/xmm4
// against retail's xmm3/xmm4/xmm5, the case 2 and default 0x40 flag updates swapped between the
// memory `or` and load/or/store forms, and the timer reassociated to (rate / scale) * ticks.
// Earlier (236): the first difference at the switch's dispatch is the default jump target (the
// code after the switch), so the case bodies' sizes differ and the switch region shifts with it.
// Earlier (236): frame 72 agrees; the camera mode and the object index are read into
// this round found the first difference at the switch's dispatch —
// the default jump target (the code after the switch) is not where retail puts it, so the
// four case bodies are laid out differently and the whole switch region differs with it.
// Earlier: frame 72 agrees; the camera mode and the object index are read into
// Earlier 236 (was 236): frame 72 agrees; the camera mode and the object index are read into
// named locals before the switch and the test; direction preload, length/zero XMM roles and
// flag stores still differ.
// frame 0x48 agrees; direction preload, length/zero
// registers and memory flag updates remain.
// Earlier 241 (from 241): frame 72 agrees; direction preload, length/zero XMM roles and flag stores differ.
// Source-field reference 245; copied-normal squared length 258; adjacent trig factors 246;
// native pan point 290; separate final state pointer 245 (unchanged reduced preview 245).
// frame 0x48 agrees; direction preload and length/zero XMM roles, flag RMWs and interpolation differ.
// Entry flag result 459; depth one 245; native plane construction 248; matrix return 326; point-category conditionals 272.
// direction preload, length/zero registers and flag stores still differ.
// Earlier 244 (was 246): direction preload, length/zero registers and flag stores still differ.
// Earlier 244 (from 246): direction preload, length/zero registers and flag stores still differ.
// native point state 401p; native pan state 246p.
// final result local scope 246p; reuse clamped pan fraction 312p.
// single default flag store 425p; native time record 246p.
// point state default flags 425p; pan state clamped fraction 312p.
// direction preload, length/zero XMM roles, flag stores and timer differ.
// Forced dot/plane/tick expansion 247 each; each pair and all three 247;
// all three plus immutable command pointer and scale 253 (fast base 247).
// frame 0x48 agrees; projection preload, length/zero
// Earlier 245 (was 245): frame 0x48 agrees; projection preload, length/zero
// Earlier 245 (from 245): frame 0x48 agrees; projection preload, length/zero
// registers and flag stores remain. Intrinsic copies of the normal 245, basis 401,
// output axes 245, pan axes 245, position 245, animation 245; projection copies
// together 401, all record copies 402 (each fast and plain).
// frame 0x48 agrees; direction preload, XMM roles and flag updates remain.
// Existing line helpers 368p/371p; fraction-home and output-record combinations 368p/367p/367p.
// External command record 236p (one executable row); pan/view fraction homes 245/245.
// direction preload, length/zero registers and memory flag updates remain.
// Projected output orders yxz/zxy/yzx/zyx/xzy 245/245/388/388/245;
// default basis up-left-forward/left-forward-up/forward-left-up 248/248/247 (fast).
// Complete state observations, with cached/live time root 236/236 (one executable row);
// live scale, live time/scale 450/407; live target, time/target, scale/target, all 264/379/411/457.
// Earlier 245 (was 245): direction preload, length/zero registers and memory flag updates remain.
// All three reused direction scalar components, six observation orders 473 each
// (frame 0x4c, 1724 bytes); complete normal from those scalars, ijk/jik orders 487/487
// (1664 bytes). Original field observations, plane copy, helpers and float groups restored.
// Earlier 245 (from 245): direction preload, length/zero registers and memory flag updates remain.
// Native projection/position/animation flag bit views, alone and all
// combinations, 245 each (233 executable rows); full dword bitfield 401
// (382 executable rows). Original dword field, complete body and helpers restored.
// Round 172: Immutable direction 245; coherent projected-position receiver 371, with immutable
// direction 371; source-origin receiver 401, with immutable direction 401; both receivers 381, with
// immutable direction 381 and length-at-expression 381. Executable differences
// 233/354/354/383/383/357/357/357 versus baseline 233; all original receivers restored. Frame 0x48
// agrees; direction preload, normal/length/zero allocation, flag stores and timer scheduling remain.
// Round 206: All three length products named at their expression in orders kji/kij/jki/jik/ikj/ijk
// 405/405/268/268/405/268; complete immutable products in jki/ijk order 268/268.
// Original (k+j)+i grouping, operands, observations, helpers and calls restored.
// Round 250: Projection for/while/do loops 360/360/401; pan for/while/do loops 385/385/292; both
// for/while loops 380/380 (expanded fast control 245). Original parallel rows restored; direction
// preload, length/zero allocation and flag stores remain.
// Round 188: Projection-case basis forward-left-up / up-forward-left / up-left-forward /
// left-forward-up / left-up-forward, then normal before basis at forward-up-left /
// up-forward-left / left-forward-up:
// 247,247,248,248,247,279,279,279 fast; reduced control 245, full 245. Original executable source restored.
// Round 356: Native-plane length before offset 250; common inner flag store 400; both 405;
// length/complement 262; flag/complement 400; both/complement 405; both/quotient 405; all 405.
// Original projection, native switch, separate flag stores and expression parts restored.
// Round 190: separate blend output 337; native target quaternion mutable/const 367/367;
// split output plus target 402; native converter result 363, with split output 393;
// independent normalization value 367, with split output 375. All original quaternion boundaries restored.
// Round 253: Parallel projection-product parts, ordinary/immutable 251/251;
// pan first/both weighted parts 245/245; projection with either pan form 251/251;
// native shared origin alone/with all parts 501/501 (1736 bytes). All restored.
// Round 80: direction declared before the plane 245; length summed j-first, `(n.j * d.j + n.k * d.k) + n.i * d.i`,
// 247 (retail's d.j preload, but the normal's i/k registers swapped and the i term still copies the
// normal); both 247; j-first with direction-first operands in every product 247, with the swapped
// declarations 247. Original body restored.
// Round 82: length as dot3f(direction, &plane.n) 375 (retail's d.j preload, normal i/j/k in xmm3/4/5
// and offset products, but the sum stays i-first and the rest shifts); dot3f(&plane.n, direction) 375;
// t as a ternary 245; ternary with dot3f 375; dot3f with direction declared first 375. Body restored.
// Round 60: length_sq3f(direction) 464; length_sq3f(&plane.n) 265;
// dot length with prezero 463; copied-normal helper with reversed plane dot 265;
// reversed plane dot alone 245; direction-j pointer 400; early copied-normal helper 272;
// early dot length in either input order 256/256. Original body restored.
// Round 84 (lane 5): VC reassociates the timer to (rate / scale) * ticks where retail keeps
// (ticks * rate) / scale; a named seconds local in the tick helper restores retail's order 388,
// a named seconds local at the use 384 (lockstep shift); field-of-view fields read at each use 259,
// with a named start 264, target local with fields elsewhere 254; seconds local plus those fields
// 380 (retail's instruction count, 462, but zero/value XMM roles swap throughout). Restored.
// Round 399: Outer native case orders point-animation-pan-first_person / point-animation-first_person-pan
// / point-first_person-pan-animation / point-first_person-animation-pan /
// pan-point-first_person-animation / pan-animation-point-first_person / pan-first_person-point-animation
// / animation-point-first_person-pan: 330/340/356/341/460/464/459/459. Original restored.
// Round 39: shared native pan rotation matrix, either declaration/conversion order, 243;
// with fraction before conversion 358 at 1716 bytes. Reusing the existing full pan transform
// in both conversions, four late-fraction forms, 237. Original native objects and body restored.
// Round 425: Existing vector setter for the complete plane normal, alone and with
// projection/offset/pan setters in all combinations, 443/447/443/447/448/456/448/456 fast.
// Original normal copy, all parallel rows, float groups and complete body restored.
// Round 434: native pan quaternion/transform/animation output entry lifetimes,
// seven subsets 236/236/236/236/236/236/236; complete graph with role names 236.
// Original workspace scopes, projection, helpers and complete body restored.
// Round 51 (lane 11): a compile-only screen of the plane block (it reproduces this build's preload and
// XMM numbering exactly) over all six length term orders x product operand orders (48), and over
// plane.d argument order x length form (six term orders, dot3f(d,n)/(n,d)/(d,d)/(n,n)) x `plane.n =
// *direction` or `= camera->point.left` x plane.d before/after the length (80): best block 35 against
// 38; n.i always lands in XMM5. Real builds: length terms i,k,j / k,i,j / j,k,i 243/243/238; direction
// and length declared at their first assignment 236; the four block declarations reordered 236.
// Round 19 (lane 18, fast, positional/aligned against 236/123): listed forms re-measured: seconds
// local in the tick helper 385/128 (retail's ticks * rate / scale, one instruction shorter, the rest
// shifts), j-first length 238/122 (retail's direction j preload; the k product still the
// accumulator); new: both 387/127; j-first length with the plane dot's operands reversed 238/122;
// with the plane declared after the scalars 238/122; one 0x40 flag update after the point-type
// switch 399 (1704 B); the length zero test first in the if/else 236/127. All restored.
// Round 68 (lane 12): retail carries 0.0 in XMM0 into the timer test from every inner-switch path (the
// point-category arm's length test leaves it there), which follows from its d.j preload; the double
// `flags = 8` store is retail's. Timer as an if/else statement 244 (1700 bytes); length summed j-first
// 238; both 246; distance dot with position first 236 (same code), with the j-first sum 238. Unused-local
// probes +1/+2/+3/+4/+8/+12 236/245/369/236/236/385 (diagnostic). Original body kept.
// Round 25 (lane 18, fast): the field-of-view switch test is the existing 0x13cb40 test (LTCG inlines it, as
// retail does), 236 with identical code (kept in place of its body copied through a made-up struct); on it the
// field-of-view fields read at each use 258 (1704 bytes; retail reads time/duration at each use but keeps the
// target loaded for `fov = target`); the length as dot3f(direction, &plane.n) 251 (d.j preload, i-first sum);
// the plane distance as dot3f(&plane.n, offset) - d 236 (same code); the length before the plane offset 241.
// Retail's plane distance computes the i product first in n.i's register (n.i xmm3 dies first), ours last.
// Round N284: Projection zero-first, absent-object-first and unfinished-FOV-first branch
// subsets 1..7 score 236/260/260/236/236/260/260; all with nested FOV guards 260.
// Original native normal copy, branch graph, captures and complete body restored.
// Round 455: Overlap-safe native transfers for normal/frame axes/pan axes, seven
// subsets, then all with animation outputs: 459/470/477/301/493/452/445/488. Original aggregate copies,
// projection, shared helpers and complete body restored.
// Round 458: Actual point-mode view axes/origin reset/final camera position through
// overlap-safe whole-value transfers, seven subsets, then all with memcpy normal input:
// 455/402/490/374/482/483/400/400. Original pose observations, transfers, projection and body restored;
// direction preload, normal/length/zero allocation and flag scheduling remain.
// Round 60 (m3 lane 1, fast): length sum right-grouped `n.i*d.i + (n.j*d.j + n.k*d.k)` 238, direction-first 238;
// `(n.j*d.j + n.k*d.k) + n.i*d.i` 238 (d.j preload, normal registers still rotated); `n.i*d.i + (n.k*d.k + n.j*d.j)`
// 236; `(d.j*n.j + d.k*n.k) + d.i*n.i` 238. A one-link harness screen of all 24 grouping x term order x operand order
// forms and both dot3f orders loads n.i into XMM5 in every one, but the real build gave XMM3/4/5 for
// dot3f(direction, &plane.n) (round 82) and the harness uses XMM6 where the build uses XMM7: not a numbering screen here.
// Round 463: actual partial length accumulation, full successive/leading-pair,
// zero-first factor result and uniform product directions, 256/236/371/368/256/236/371/368;
// original (k+j)+i grouping and complete executable source restored.
// Round 57 (lane 11, fast; control 236, positional rises while the shape gets closer). LEAD, 337 at 1696 bytes, aligned
// 125 from 139: FOV tail read at each use with a `start` local; a `seconds` local in the tick helper; plane.n copied from
// camera->point.left with no direction local; length `dot3f(&plane.n, &camera->point.left)`; the position through a
// file-local forceinline `a + b * scale` helper writing through a result pointer. The plane block then equals retail
// (normal XMM3/4/5, the third dot's in-place i product, the load-first pos.i product) but for the length's j/k load
// order and the length/zero roles; what is left follows the zero constant colouring XMM1 for retail's XMM0
// (t, length, timer, distance, fov one register off). Stores through a pointer also fuse `or [ebp], 0x40` as retail:
// the flags load cannot be hoisted past them. The default arm's `or`/xorps stays fused where retail splits it (its xorps
// tail hosts the NONE path's block). Forms: position through a pointer local 367; FOV tail alone 264 (1708 bytes);
// tick seconds + FOV 395 (1704); + pointer 356; + dot3f length 343; third dot dot3f(offset, n) with dot3f length 344
// (aligned 127); + 23 declaration orders of the plane block 344 each; dot/pointer/position grid of 36 forms 337-390;
// six explicit length term orders on the lead, either operand side, 356-359; normal + distance locals with no plane
// struct 379 (distance forward-substituted); on the lead field_of_view declared at its assignment 337, active cleared
// first 337, distance/timer stores swapped or chained 339/339; direct position stores 349 (1700 bytes).
// Round 289: Standard native-array normal/basis/converted-animation transfers, subsets 1..7,
// then backward normal with all: 446/475/474/337/464/465/437/437. Coherent output-first
// matrix-builder formal/declaration/call order 236, matched callee preserved. All transfers,
// include, interfaces and complete executable source restored; direction and zero homes remain.
// DH308: Point-mode planar cosine/sine product pairs and distance-clamp subsets
// 1/2/3/4/5/6/7, then all with enclosing product declarations: 245/245/236/236/245/245/236/236.
// All retain frame 0x48 and 1696 bytes; executable rows tie or regress. Original body restored.
// Round 65 (m3 lane 1, fast, control 236): length `(d.j*n.j + d.k*n.k) + d.i*n.i` with the direction operands
// first 238; plane distance base as dot3f(&plane.n, position) 236 (same code); the direction pointer
// initialised at its declaration ahead of the axis copies 255; length as dot3f(&plane.n, direction) 251;
// the normal copied from camera->point.left directly 236 (same code). Original body restored.
// Round 59 (m3 lane 11, fast): the timer through a `seconds` local at the call site 385; `t` as one ternary 236 (same
// code); the direction pointer taken before the three axis stores 255; the axis stores of cases 1 and 2 through one
// forceinline reset helper 236 (same code); the 0x40 flag set once before the point-type switch 426. All restored.
// Round 78 (m3 lane 16, fast): length `dot3f(direction, &plane.n)` 251 (retail's d.j preload; the copied normal
// loads i/j/k into XMM5/3/4 for retail's XMM3/4/5 and the plane-offset product copies n.i), with `0.0f !=
// length_squared` 250, with plane.n copied from camera->point.left 251; `0.0f != length_squared` alone 235
// (one point, not kept); the length written i,j,k direction-first 253; plane.d as dot3f(&plane.n, position)
// 236 (same code), with the dot3f length either operand order 251/251.
// AC509: Signed-distance/fraction real-double domains (-/double, real/-, real/real,
// real/double, double/-, double/real, double/double) 407/256/245/413/410/413/413;
// complete double factor 427. Archived real quotient 236 excluded. Original projection,
// helpers and body restored; direction/normal/zero homes and flag scheduling remain.
// AC511: Immutable complete plane with original/default-zero/conditional/zero-first
// factors, original/producer direction declaration: 443/437/443/443/443/437/443/443.
// NOT kept: readonly plane producers shorten to 1660/1668 versus 1696 and regress;
// original plane, direction, factor, scopes and complete body restored.
// AC513: Existing shared add-scaled projection; copied-normal squared length,
// dot length in both input orders, per-use direction, source squared length, default-zero
// and zero-first factor: 367/370/371/371/367/461/449/367. NOT kept: no exact match;
// original header set, projection, factors, helper bodies and whole body restored.
// AC518: Complete nonzero-length admission bool/byte/short/long at producer
// or declared before plane workspace: 236/236/236/236/236/236/236/236 (control 236).
// NOT kept: one-use predicate unmatched; original projection and body restored.
// AC523: Remaining complete case orders, point-pan-first-animation 262;
// pan-animation-first-point 468; pan-first-animation-point 464;
// animation-pan-point-first 457; animation-pan-first-point 458;
// animation-first-point-pan 456; animation-first-pan-point 459;
// first-point-animation-pan 440. NOT kept: all regress; original whole cases,
// projection, helpers, observations and complete body restored.
// AC525: Complete FOV-target presence/absence results in bool/byte/short/long,
// eight at 236. NOT kept: predicate residence unmatched; original target gate,
// snapshots, projection and full body restored; direction/normal/zero homes remain.
// Lane 9 r87 (fast; aligned against 236/129): three extra pointer locals in the plane arm (identity chain, one local repeats every 8)
// give retail's normal registers (i/j/k in XMM3/4/5) and offset products, 369/116, the rest shifts; named products in the expression
// count the same. Chain 1/2/4 236/245/236; with the length j-first, chain 1/2/4-7, 238/247/238/238/238/247; j-first at chain 3 375/123;
// accumulated j, k, i at chain 3 374/127; dot3f at chain 3/5 250/374. Left on the chain form: which direction load the scheduler hoists
// above the `lea` (retail d.j, ours the first product's d.k), which sets the length/zero roles for everything after the block.
// AC539: Actual FOV transition-completion result, positive then complementary
// bool/byte/short/long: eight at 236. NOT kept: unmatched one-use decision storage;
// original duration/target observations, projection, helpers and full body restored.
// AC541: Actual timer nonzero-scale decision, bool/byte/short/long at its consumer
// 236/236/236/236; at the entry snapshot 365/365/439/323. NOT kept:
// no equality/gain; original snapshot, projection, float trees and whole body restored.
// AC543: Actual animation-matrix completion/pending call results at the original
// call/test boundary, bool/byte/short/long: eight at 236. NOT kept: one-use call
// outcomes unmatched; direction/normal/length/zero homes remain. Original projection,
// output validity, helper calls, case order and complete body restored.
// F383: Coherent existing basis-builder input order 234, but its matched callee becomes 2;
// NOT kept. Immutable projection denominator/captured scale/pan fraction, seven
// subsets, all 236. Original interfaces, copied plane and complete body restored.
// Round 64 (m3 lane 14, fast; masked aligned 59, 462 instructions on both sides, register-sensitive 123): retail's field-of-view blend reads the
// fields at each use: time and duration read directly 259 (1704, aligned 50), target too 258 (54), with a named start 264 (1708, aligned 48: that block
// then has retail's shape, but the bytes grow); that with the dot3f length 266 (59), plus 1-7 pointer locals 265/265/265/265/376/376/261 (aligned
// 52/54/52/52/51/51/58); the dot3f length with 1-7 dead locals in the plane block 250/250/250/250/374/374/254 (register-sensitive 126-127, 113 at 5-6, but
// then d.k preloads again); the distance dot's operands swapped 236 (aligned 57, an order picked by score, not kept), through a forward pointer 236 (61);
// the timer as if/else 244 (1700); the offset products before the negations 239 (62). Most of the count is the function-wide zero in XMM1
// against retail's XMM0, which follows the length sum's d.j preload. Body unchanged.
// Round 63 (m3 lane 2, fast; control 385): kept: the projected position through the shared vector3f_add_scaled
// (include/vector_products.h; the file's own cross product renamed camera_cross3f for the header's cross3f) 384;
// with the FOV fields read at each use and a named start, plane.n = camera->point.left with no direction pointer
// and the length as dot3f(&plane.n, &camera->point.left) 353; with the tick conversion named inside the tick
// helper (main's call-site seconds local goes) 337; `start` initialised at its declaration 331 (kept). Each part pays: hand-written position 349 (1700 B), FOV
// locals as before 356 -> 386, the old plane block 356. Not kept: the line parameter through a new
// (plane, point, direction) pointer helper 389 (1680 B); on 337, the timer as if/else 339 (1700 B), the length as
// dot3f(&camera->point.left, &plane.n) 352, the pan lerp through 0x1a4c00's vector3f_interpolate moved to
// vector_products.h 342 (the pan arm's flag update becomes retail's memory `or`, but the lerp products commute),
// length_sq3f(&plane.n) 370, the distance dot's operands swapped 337 (same code). Left: retail's normal XMM3/4/5
// (ours 5/3/4) and its shared zero in XMM0 (ours XMM1) from the plane arm through the timer and FOV tail.
// On 331: the offset through vector3f_scale then negated 331 (aligned worse); plane.d dotted with
// camera->point.left 356; the timer as `time_scale == 0.0f ? 0.0f : ...` 331; command->object_index from the
// camera_object_index local 342; start declared before t 331. Main's call-site seconds local in place of the
// helper's 373. The pan quaternion length still sums j, k, w, i where retail keeps the helper's w, k, j, i.
// M3 lane 12 r108 (fast; xmm and general registers masked, sequence aligned, 43 lines at 331): the pan position through
// real_point3d_set 338 (masked 55: retail's memory `or [ebp], 1` after the stores, but all three sums are computed before the
// stores); a pointer local for the pan position 391 (50); a named FOV `target` for the test, the reached arm and the blend 386
// (66; perhaps round 63's FOV locals form); case 1 with its own `flags |= 0x40; break;` 331 (same code); the length as dot3f(&camera->point.left, &camera->point.left) 369 (1700 B); the distance clamp as `dot > 0.0f ? 0.0f :
// dot` 373 (1700 B). Masked, the rest is: the length's hoisted load (retail left.j, ours left.k, which puts the length in XMM0
// and the shared zero in XMM1), the default arm's split `or` plus its own xorps (retail jumps over the NONE path's xorps), the
// distance dot's operand loads (retail forward for k and j, position for i), the pan quaternion sum order, the FOV target reused
// from its test register in the reached arm (ours reloads it). Body restored.
// Round 65 (m3 lane 2, fast, control 331): the plane arm now has retail's normal XMM3/5/4 and offset; the first real
// difference is the length's hoisted load (retail left.j, ours left.k), which puts retail's zero in XMM0 for the rest.
// Re-measured on this body: the length written out j,k,i 374 (1700 B: the left.j preload, but the normal rotates), left-first products 374, j,i,k 374,
// k,j,i 371; `0.0f != length_squared` 330 (only the ucomiss operands, one point, not kept); the length before plane.d 377
// (1700 B); the zero arm first 341; `t = 0.0f` then the guarded assignment 382 (1704 B); `t = -(...)` either grouping 331
// (same code; new from here); `command->distance = -distance` 331 (same code); one function-scope `t` for the plane, pan and FOV blends
// 371 (1700 B), for the pan and FOV blends only 331.
// Lane 5 r166 (fast alone, control 331 = plain baseline; CAVEAT on callee 0x16cfa0): `switch (camera->mode)` with no mode
// local 331; camera_object_index as long 331; case 1 with its own `flags |= 0x40; break;` instead of falling into default 331
// (same code); `t` preset to 0.0f with no else arm 454 (1704 bytes); the length as length_sq3f(&camera->point.left) 480 (1720).
// Body restored.
// m3 lane 14 r77 (fast; masked = difflib count with registers and offsets masked, control 25): on round 63's dot3f base
// (the reason to re-measure old length orders) the length written out, the other dot3f calls kept: (j + k) + i 374,
// (k + j) + i 371, (i + k) + j 371, (i + j) + k (dot3f's own body written out) 376, j + (k + i) 376, i + (j + k) 374,
// k + (i + j) 372, all 1700 bytes (masked 26-29). A compile-only build (no /GL) loads the plane's first length operand from the other
// base register (EDI for retail's ESI), so it is no screen for this block.
// Round 153 (m3 lane 8, fast; control 331; aligned 111): the inner switch's case 2 already matches; the plane length's hoisted load
// (retail left.j, ours left.k) still decides the XMM0/XMM1 zero for the rest. Tried: the pan position stored through a
// `point3f *` local to command->position 391; plane.n copied before the three matrix stores 377; plane.d as
// dot3f(&plane.n, position) 331 (same code); a `vector3f const *` direction for the copy and the length 361 (first difference
// +0x5c); the inner switch's default arm first, case 1 then ending in its own flag update and break 331 (same code).
// Round 87 (m3 lane 11, fast; aligned in brackets, control 331 [111]): the normal copied ahead of the three axis stores
// (round 188's normal-before-basis, re-measured) 377 [173]; the plane distance named in its expression before the
// division 384 [118] (1704 B). No plane-construction or line-projection helper exists to call here.
// M3 lane 12 r112 (fast with 0x16c6f0/0x13cb40, both matched; 331, aligned 94, masked 27): no trial run. Of the 27 masked
// lines, two blocks are read-modify-writes of command->flags: the default arm, where retail splits `flags |= 0x40` into
// load/or/store around its own `xorps xmm0, xmm0` and jumps over the NONE path's xorps (ours keeps the memory `or` and
// falls into that shared xorps: once the scheduler splits the store the two tails differ and are not cross-jumped), and the
// pan arm, where retail keeps the memory `or [ebp], 1` after the position stores and ours hoists the flags load among them.
// The forms that reach those (case 1's own `|= 0x40; break;`, the 0x40 set once before the switch, the pan position through
// real_point3d_set, a pointer or the moved lerp helper) are listed above; the rest is the length's hoisted left.j load that
// puts retail's zero in xmm0, the distance dot's operand loads and the shared quaternion helper's sum order.
// M3 lane 13 r83 (fast, control 331, fully masked aligned 27): the point-mode timer preset to 0.0f and set under
// `if (time_scale != 0.0f)` before one store 383 [33]; the point-type switch as `if (type == 2) {...} else { if (type == 1)
// {axes} flags |= 0x40; }` 401 [32]. No other untried natural form found: the length's hoisted load
// (left.j in retail, left.k here) still sets the function-wide zero register (retail XMM0, ours XMM1), and the term
// orders, dot operand orders, ternary/if forms for t and the timer, lerp loops and helpers that move it are listed above.
// M3 lane 10 r97 (fast with 0x16c6f0 and 0x13cb40 MATCH; base 331): nothing kept. In the field-of-view tail: the target in a
// named local for the test and the reached arm, the blend reading the field (a mix, diagnostic: retail keeps the test register
// for `fov = target`; a listed form, round 84's target local) 360; the time ratio pinned in its initializer 337; the blend written `start + (target - start) * t` 331 (same
// code); `t` declared ahead of the function_13cb40 test 331 (same code). In the plane block: the length as dot3f(&plane.n,
// &plane.n) 361; plane.n copied member by member 437 (1664 bytes).
// M3 lane 11 r95 (fast with the file's matches, 0x16c6f0, 0x13cb40; base 331, masked aligned 38): the FOV blend reading
// start from the field at both uses (no `start` local) 337 (masked 55); `if (function_13cb40() && target != 0.0f)` as one
// test 331 (same code); the yaw rotation's products written `offset * cosine` / `offset * sine` 331 (same code: VC picks
// the copied operand itself; retail copies the sine, ours the offset); `vector3f offset` declared before sine and cosine
// 331 (same code); `command->distance = 0.0f - distance;` ahead of the offset rows 331 (masked 32: retail computes that
// subtraction first and the three forward products before the other subtractions; a statement order, not kept). Body restored.
// T96: Actual direction/origin/observer-position input values, then direction-origin/
// direction-position/origin-position/all/all immutable: 501/505/435/544/552/538/590/590.
// NOT kept: input copies grow the frame/extent and do not recover native direction-j
// preload or zero/normal allocation. Original shared math, timer and full body restored.
// MiniMax B (11-13): no new form, no lower score (331): the plane length's hoisted load still takes the normal's other component first, so retail's zero sits in xmm0 and ours in xmm1; the permuter cannot search this function because the reduced image scores it 348 against 331.
// M3 lane 2 r79 (fast with 0x16c6f0/0x13cb40, both MATCH; base 331, register-masked aligned 43): the FOV blend's `start` declared
// and read after the PIN of t (retail loads start after the clamp) 337 (masked 45); the basis builder's returned matrix in its
// own statement before function_141f60 373 (1700 B); the pan lerp's `1.0f - fraction` named once for the three rows 373 (1700
// B); the point type read into a `short` local for the inner switch 373 (1700 B): any one added local gives this 373/1700 phase.
// Kept as a clean-up: the one-use `dword camera_object_index` copy dropped, the NONE test reading `camera->object_index`, 331
// (only the distance dot's i product changes: forward.i loaded first, where retail loads position.x first; masked 45). With
// the mode local dropped too (lane 5 r166 listed that alone at 331) 371 (1700 B). The point-type local is the same code as the
// basis statement. On the kept body: the animation case's flags update after its distance and timer stores 333 (masked 43).
// m3 lane 8 r164 (fast with 0x23d090, MATCH each time; base 331): the squared length as length_sq3f(&plane.n) 373 (round 60's
// 265 was before the timer fix) and as dot3f(&camera->point.left, &plane.n) with no direction local 372 (1700 B);
// length_sq3f(&camera->point.left) re-measured 480; the point case reading camera->object_index at each of its three uses
// (no object_index local) 372 (1700 B); the timer reading game_time->scale at both uses (no time_scale local) 410 (1700 B);
// the projected point offset along `&plane.n` (the copy of the same left vector) 449. Retail's default arm carries the
// 0.0f of case 2 into the timer test in XMM0 (its own xorps splits the `|= 0x40` into load/or/store); ours re-zeroes XMM1.
// P471: Complete case orders first/pan/point/animation, first/pan/animation/point,
// first/animation/point/pan; standard distance minimum, projection/timer/FOV nonzero
// and FOV-completion predicates: 456/461/456/404/518/483/428/323.
// NOT kept: 323 grows native 1696 bytes to 1708 and adds a call; all other forms
// regress. Original full body and includes restored.
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
	short mode_kind = camera->mode;
	switch (mode_kind)
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
			command->timer = time_scale != 0.0f ? game_ticks_to_seconds(camera->ticks) / time_scale : 0.0f;
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

			command->timer = 0.0f;
			t = PIN(t, 0.0f, 1.0f);
			{
				matrix3x3 matrix;

				function_141f60(function_142d10(&camera->up, &camera->forward, &matrix), &rotation);
			}
			function_11da10(&rotation, &camera->pan.rotation, &rotation, t);
			quaternion_unit_or_identity(&rotation);
			transform4x3f transform;

			function_141e10(&transform.rotation, &rotation);
			command->forward = transform.forward;
			command->up = transform.up;
			real fraction = PIN(t, 0.0f, 1.0f);
			camera = camera_scripting_state;
			command->position.x = camera->position.x * (1.0f - fraction) + camera->pan.position.x * fraction;
			command->position.y = camera->position.y * (1.0f - fraction) + camera->pan.position.y * fraction;
			command->position.z = camera->position.z * (1.0f - fraction) + camera->pan.position.z * fraction;
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
	if (function_13cb40())
	{
		if (camera->field_of_view_target != 0.0f)
		{
			if (camera->field_of_view_time >= camera->field_of_view_duration)
			{
				field_of_view = camera->field_of_view_target;
			}
			else
			{
				real t = camera->field_of_view_time / camera->field_of_view_duration;
				real start = camera->field_of_view_start;

				t = PIN(t, 0.0f, 1.0f);
				field_of_view = (camera->field_of_view_target - start) * t + start;
			}
		}
	}
	command->field_of_view = field_of_view;
	command->unknown90 = 3;
	command->unknowna4 = 0.0f;
	function_172520(command);
}
