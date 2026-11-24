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
// score 126 (was 126): Frame 0x48 and extent agree. The projection preloads k in this build and j in retail (round 196's forms at the end of this note).
// The switch span, direction-k preload and exchanged length and zero XMM homes remain.
// 1696 bytes and frame 0x48 agree; first +0x49 is the switch span, then direction-k versus j preload and
// length/zero homes differ.
// Frame 0x48 and extent agree. The projection preloads k in this build and j in retail.
// Earlier (126, was 126): 1696 bytes and frame 0x48 agree; first +0x49 is the switch span, then direction-k versus j preload and length/zero homes differ.
// MiniMax E (11-20): score 126 (was 126): no new form, no lower score. The case-2 squared length still hoists the
// camera point's left.k into XMM1 where retail hoists left.j, so the sum's accumulator stays in XMM0 against retail's
// XMM1 and every later float block runs with the registers shifted by one; the distance dot itself is instruction for
// instruction the same in both. Measured this round and dropped: the squared length with the camera point's left on
// both sides 392, the length written out as left.j, left.k and left.i squares 487, the squared length's dot arguments
// swapped 394, the distance dot's arguments swapped 171 and the two dot statements in the other order 399. The hoisted
// component is not reachable from the dot's spelling, so the body stands.
// MiniMax E (11-19): score 126 (was 126): no new form, no lower score. Read from the listing again: the case-2 plane
// length still preloads the camera point's left.k into XMM1 where retail preloads left.j, so the length's accumulator
// stays in XMM0 against retail's XMM1 and the zero that follows stays in XMM1 against retail's XMM0; the plane distance
// dot itself matches instruction for instruction, so the difference is in the hoisted load alone. The length dot written
// out with the six term orders (394-397), the swapped dot arguments (394) and the swapped distance dot's arguments (126,
// identical code) were measured and dropped. This round measured five more spellings of the same dot: the two dots in
// the other order 399, the squared length of the camera point 478, the squared length of the plane 399, the swapped dot
// arguments 394 and the dot against itself 177, all worse than 126, so the hoisted component is not reachable from the
// dot's spelling. The distance dot's products still load the position's component first
// where retail loads the forward's, the yaw block still keeps its zero in XMM1 and the pan arm's two lea still come
// out the other way round.
// M3 lane 15 r104 (fast with 0x142d10, which stays MATCH; control 126, all restored): 0x142d10's parameters in the order
// retail's call setup implies (ECX up, EDX forward, EAX out set up in that order: `out, forward, up`; definition,
// declaration and the one call) 124 (fixes the pan arm's two `lea`s; a gain under 4, so not kept: the lead for a
// round that finds more); on it: case 2's squared length before the plane distance 400 (1700 B); the point case's
// distance dot with its arguments swapped 124 (only register names move: retail loads forward first in the k and j
// products and the position first in the i product); the animation case's flags update after the distance and timer
// stores (retail's store order) 125; case 2's locals declared t, squared length, plane 124 (same code); the timer's
// ternary as an if/else 150 (1700 B).
// MiniMax C (11-17): score 126 (was 126): no new form, no lower score. The case-2 plane length still preloads the
// left vector's k component into XMM1 where retail preloads its j, and the length's accumulator stays in XMM0 where
// retail keeps it in XMM1, so the zero that follows stays in XMM1 against retail's XMM0; the swapped dot arguments
// (394), the swapped dots (399) and the moved distance store (127-133) were measured and dropped.
// MiniMax I (11-16): score 126 (was 126): no new form, no lower score; the length still preloads the
// camera point's left.k into xmm1 where retail preloads left.j, and the zero register stays xmm1 for
// the timer, the animation case and the field-of-view tail; `t = 0.0f;` before the test (375), the
// swapped dot3f operands (394), length_sq3f (399) and a `direction` pointer local (417) are worse.
// Earlier header detail: earlier 149, and 149 was 331; 1696 bytes as retail since r202; earlier 1700 bytes for retail's 1696. Every block's zero is XMM1 where retail uses XMM0 (the plane arm,
// timer, animation case and field-of-view tail), following the plane length's hoisted left.k load (retail left.j); that
// costs the animation case a zero copy and the field-of-view tail its reuse of the target test's register (the 4 extra
// bytes). Also left: the distance dot's operand loads, the yaw rotation's product/negation order, and the pan arm's
// basis-builder argument setup and the shared quaternion helper's sum order.
// Earlier at 331 (was 331): every block's zero is XMM1 where retail uses XMM0 (the plane arm, timer, animation case and
// field-of-view tail), following the plane length's hoisted left.k load (retail left.j); the pan arm's basis-builder
// argument setup and the shared quaternion helper's sum order remain.
// MiniMax R (11-14): no lower score (331): in the case-2 length the dot hoists the left vector's k component before the copy and
// sums in xmm0, where retail hoists j and sums in xmm1; the term order of the shared dot3f does not move it (all six orders
// Earlier 331, and that header is not this lane's to edit) and the self-dot spelling gives the same squared length at 329.
// Earlier at 331 (was 331; 331 before): m3 lane 8 r164 and m3 lane 2 r79 notes at the end; round 95 (m3 lane 11) before it; round 112 (m3 lane 12) before that. Earlier at 331 (was 385; 385 was 236 before the timer fix): round 63 (end of note) re-applied round 57's lead with shared helpers.
// MiniMax H (11-11): no new form, no lower score (331); retail keeps the point case's normal in XMM3/4/5 with the zero in
// XMM0 and ends case 2 with a read-modify-write `or [ebp], 0x40`, while ours splits the flag updates and keeps the zero in XMM1.
// MiniMax C (11-11): no new form, no lower score (331): the length agrees (1696 bytes); case 2 still
// loads the camera normal's j for the squared length where ours picks the function-wide zero register,
// and the dot3f helper that computes it lives in include/unknown_0259d0.h, which this lane may not
// edit; the point case's normals also still take XMM5/3/4 where retail gives them XMM3/4/5.
// Earlier 331 (was 331; 385 earlier, 385 was 236 before the timer fix): round 63 (end of note) re-applied round 57's lead with shared helpers.
// MiniMax C (11-05): no new form, no lower score; the plane's three normal copies are reloaded in the same order in both (i, k, j), but retail gives them XMM3/4/5 for i/j/k where ours gives XMM5/3/4; the squared length in field order scores 389, in k,i,j order 243, with the first two addends swapped 238, written from the direction alone 465, and with the multiply operands swapped it is byte-identical at 236.
// b5K 11-06 statement walk against retail found no wrong field, offset, condition, callee or loop; the one
// change is retail's evaluation order for the point-mode timer, which retail computes as (ticks * rate) / scale (cvtsi2ss, mulss [ebx+4], divss at
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
// M3 lane 16 r117 (fast with 0x16c6f0/0x13cb40/0x23d090): the plane length as `dot3f(&camera->point.left, &plane.n)`
// 372 (1700 B; it still preloads left.k where retail preloads left.j); plane.d as `dot3f(&plane.n, position)` 331 (same
// code), both together 372; `real length_squared` declared at its assignment 331 (same code); a `vector3f const *direction`
// for the normal copy and the length 336.
// T108: Ordinary inline metadata at existing object-matrix/basis/matrix-quaternion/quaternion-matrix providers, four isolated, three pairs and all:
// 438/331/331/331/438/331/331/438. NOT kept: no qualifying native-shape gain or equality; original complete source
// and every provider declaration restored. Native frame and first storage/scheduling gap remain.
// M3 lane 5 r183 (fast with 0x23d090, MATCH in each): dead-local phase scan n = 1..7 331/373/361/372/372/371/371 (n = 3
// moves the normal registers and the first difference to +0x5c, left.k still preloads); the field-of-view blend's t
// renamed a_t or z_t 331 each (diagnostic: the tail's XMM colours, retail zero/fov/t 0/1/2 against ours 1/2/0, do not
// follow names); the animation case's `command->distance = command->timer = 0.0f;` 333, `command->timer =
// command->distance = 0.0f;` 331; the plane arm's t as one ternary 331 (same code); the cosine computed right after the
// sine 331; the animation case's `flags |= 0x19` after its three stores 332.
// M3 lane 13 r94 (fast with 0x16c6f0/0x13cb40/0x23d090, all MATCH; plain where kept; masked = registers masked, aligned):
// kept, 331 -> 187 (1704 B, masked 45 -> 40): the point-type switch's `flags |= 0x40` written once after the switch
// (case 2 and the old default arm without theirs, case 1 ending in its own break). VC tail-duplicates the update into
// both arms, and the old default arm then takes retail's split load/xorps/or/store with its own jump; the base's
// fused default `or` had hidden 8 extra bytes in the pan and animation arms. Kept, 187 -> 149 (1700 B, masked 35): the
// pan rows written through `point3f *position = &command->position`; the stores through the pointer keep the flags
// load below them, so the pan arm ends in retail's memory `or [ebp], 1` (on older bases this pointer scored 367/391).
// On 331: the case-2 point fields through an `s_camera_point_state *` local 331 (masked 43, the distance dot's i
// product loads flip to retail's order); the projection scale through a new (plane, point, direction) inline helper
// 371 (1700 B); with the point pointer, the self-dot length 369 (1700 B) and `dot3f(&left, &plane.n)` 372 (1700 B);
// re-measured, the self-dot length 329 (masked 41: retail's left.j preload, but the length/zero colours stay swapped);
// `plane` at the point-case scope, `if (length_squared)` and `time_scale ?` 331 each (same code). Diagnostic: a zero
// store at the top still colours XMM1 (456), so the zero's register does not follow which 0.0f comes first.
// On 187: t initialised to 0.0f with the guarded assignment 404; the pan position through real_point3d_set 167 (1700 B,
// masked 47); the case-2 zero arm first 197; the self-dot length 185 (masked 36). On 149: the distance dot's operands
// swapped 149 (masked 33: its k and j products take retail's load order, the i product does not); with the case-2
// point pointer 400 (1704 B), the point pointer alone 400; the field-of-view target named for its test, reached arm and
// blend 393 (1696 B); the length as `dot3f(&left, &plane.n)` 399 (1704 B), as the self-dot 397 (1704 B), as
// `dot3f(&plane.n, &plane.n)` 185 (1696 B).
// P517: Completed object-validation available/missing result domains bool/byte/short/long at the original
// early call/test; 400/400/400/400/400/400/400/400.
// NOT kept: no exact or qualifying native-shape gain; original executable body restored.
// P520: Standard plane-length/pan-range/position-fraction/FOV-range/origin-dot/signed-distance/distance-dot/complete-plane reduction providers:
// 475/425/426/459/474/495/428/473. NOT kept: no exact native storage/provider recovery; original complete body and helpers restored.
// P522: Real angle/sine/cosine API domains, seven subsets then immutable complete yaw graph:
// 149/149/149/149/149/149/149/149. NOT kept: no exact native preload/zero recovery; original complete body and helpers restored.
// M3 lane 4 r202 (fast with 0x16c6f0/0x13cb40/0x23d090, all MATCH; plain for the kept form): kept, 149 -> 126 at
// retail's 1696 bytes: the field-of-view test written `0.0f != camera->field_of_view_target` (the tail's registers become
// retail's). Also: the plane-length test as `0.0f != length_squared` 148 (on 126: 125), the timer as `0.0f != time_scale
// ?` 126, both 125; the plane length before plane.d 404 (1704 B); the normal copied component by component 430 (1672 B);
// the t test as `== 0.0f` with the arms swapped 159; on 126: the distance clamp as `0.0f < distance` 126, the distance
// dot's arguments swapped 126. Left: the plane length's left.k preload (retail left.j) and the zero in XMM1 in the plane
// arm, the timer and the distance block.
// P524: Actual camera point-type/tick/object/pan-time storage, masks 1/2/4/8/3/12/15, unsigned point type:
// 149/149/149/149/149/149/149/150. NOT kept: no exact native storage recovery; original record declarations, helpers and full body restored.
// P526: Actual observer flags unsigned-int/long, object index int/full-width-long, output mode byte/char/signed-char fields, coherent complete output metadata:
// 149/149/149/149/149/149/149/149. NOT kept: no exact native preload/zero recovery.
// Original complete executable source restored.
// M3 lane 7 r167 (fast with 0x16c6f0/0x13cb40/0x23d090, all MATCH): the field-of-view test as `duration <= time` 126;
// `0.0f != length_squared` 125 (re-measure); the length as dot3f(&plane.n, &plane.n) 177, with the compare flipped 176;
// as length_sq3f(&plane.n) with it 398; as dot3f(&left, &left) 392 (1700 B), with the compare flipped 391; as
// dot3f(&left, &plane.n) 394 (1700 B).
// P541: Existing profile/animation-matrix automatic, ordinary and forced expansion declarations:
// 126/684/126/779/126/684/779/1272. NOT kept: ties or forced expansion grows frame/extent; direction-j preload and length/zero homes remain.
// Original complete executable source and existing helper contracts restored.
// P561: Complete entry time-state active/inactive outcome in bool/byte/short/long:
// eight 395 at 1700/1696 bytes. NOT kept: all regress and grow beyond native extent;
// original command stores, time observations, projection and complete body restored.
// P564: Zero-seeded complete mixed-input length reduction, forward/backward, real/double accumulation,
// default/explicit operations: 251/473/481/509/527/544/566/578. NOT kept: native direction-j preload and
// persistent length/zero homes remain; all regress and grow extent. Original shared dot and full body restored.
// P566: Complete mixed-input product vector, forward/backward long and short,
// forward dword/word/int/unsigned traversal: 400/399/450/450/400/450/400/400.
// NOT kept: staged vector storage regresses and grows beyond native extent;
// direction-j preload and persistent length/zero homes remain. Original full body restored.
// P569: Existing shared dot: product parts 396; accumulator 399; pair sum 399;
// immutable parts/pair 396/399; double accumulator 490; pair/final product 400; return local 394.
// NOT kept: all regress and grow past 1696 bytes. Original helper/body restored;
// direction-j preload and length/zero homes remain.
// P571: Final policy enabled/disabled result at its original producer, bool/byte/short/long:
// 395/395/395/395/395/395/395/395. NOT kept: all regress and grow to 1700/1696 bytes; direction-j preload
// and length/zero homes remain. Original policy query, projection and complete body restored.
// M3 lane 16 r126 (fast): the yaw offset through the existing vector3f_scale (forward times distance) and then each
// component taken from zero 129 (products first, but the zero still XMM1); the distance's negation stored ahead of the
// three offset rows 127; the offset's k row chained into the command's k store 126 (same code); the squared length as
// dot3f(left, plane normal) 394 (1700 B); the plane offset as dot3f(normal, position) 126 (same code).
// P574: Original-site point-distance cap admission, positive/complementary
// bool/byte/short/long: 395/395/395/395/395/395/395/395. NOT kept: no native direction-j
// preload or length/zero-home recovery; original comparison and full body restored.
// P577: Scalar memcpy for captured scale/mode/point handle/FOV start 471/126/126/405;
// paired FOV time-duration 408; admitted timer ticks 399; complete target 389;
// animation zero-float memset pair 155. NOT kept: no equality; original frame,
// direction observations, persistent zero, helpers and entire body restored.
// P580: Promoted yaw/sine/cosine receiver storage, seven subsets then immutable reused yaw:
// 126/342/341/343/336/341/348/348. NOT kept: no exact direction-j/length-zero recovery; original scalar domains,
// real API-result conversions, helpers and complete body restored.
// M3 lane 12 r133 (fast with 0x16c6f0 and 0x13cb40, both MATCH; control 126): diagnostic sweep of the plane length
// written out (the dot3f body by hand; reverted): left-first k, j, i 394, j, k, i 397, normal-first k, j, i 394,
// (k + j) + i 394, i + (k + j) 394, all 1700 bytes. Only j, k, i hoists retail's left.j load (into XMM1), but every
// written-out form also reorders the plane-distance dot ahead of it (position z and y loads swap, the k operand is read
// from the field instead of through the copied pointer), so the length/zero registers still do not follow. Body unchanged.
// m3 lane 16 r130 (fast with 0x16c6f0 and 0x13cb40, both MATCH; control 126): the camera point's direction through a
// trivial inline accessor for the plane copy and the length 126; the plane normal bound as `vector3f &` and filled through
// it 126; the field-of-view target through a trivial inline accessor at its test and reached arm 126 (all same code); the
// timer through an inline helper returning a literal zero on the zero-scale path 126 (same code); the plane scale `t`
// through such a helper 394 (1700 B). Re-measured: `real length_squared` declared at its assignment 126 (same code).
// M3 lane 9 r131 (fast; base 126): the point-case left vector bound to a `vector3f const &` for the normal copy, the
// length and the scaled add 416 (1700 B); the command forward bound to a `vector3f const &` for the yaw, the distance dot
// and the offset rows 232; the distance dot ahead of the yaw 126 (same code); the sine and cosine declared at their
// assignments 126 (same code); the length test as `0.0f != length_squared` 125 (only the ucomiss operands swap, the
// zero still XMM1: a positional coincidence, not kept). The compile-only screen does not reproduce this function (263).
// m3 lane 5 r195 (fast with 0x16c6f0, = full 126; aligned / register-masked aligned counts in brackets, base [103, 35]):
// both dots sum (j product + k product) + i product and add into the j product's register; retail's second dot loads the
// camera vector's j ahead of the normal's copy and its k at the product, ours the other way round, so the length ends in
// XMM0 (retail XMM1) and every later zero takes the other register. The second dot as dot3f(camera vector, normal) 394
// (1700 bytes) [119, 40]; as the camera vector's self-dot 392 (1700) [115, 37]; the length taken ahead of the plane
// distance 399 (1700) [131, 44]; the distance as dot3f(normal, position) 126 (same code); the distance against the
// camera vector instead of the copied normal 172 [120, 52]; t as one conditional expression 126 (same code). The three
// 39x forms are worse aligned too, not only shifted. Body unchanged.
// Unnegated quotient through the join, negated at use or before it 164/164;
// squared length as the zero arm or factor default 378/378; conditional
// factor at the scaled add 126; distance before the test or length 411/411;
// complete plane aggregate 440. Direction-j preload and zero home remain.
// DH401: Existing staged dot for length, plane offset, point distance,
// offset/length, length/distance, offset/distance and all three, then
// direction first for length: 151/400/126/399/151/400/399/405.
// NOT kept: the revised accumulation order requires a match.
// Round 624: transform reference/pointer through both point subcases 461/461; projection-case
// reference/pointer 473/473; rotation subobject reference 438; transform and point-state references 461;
// transform pointer with staged scale 461; staged signed distance, quotient and factor 126. The direction-j
// preload and zero register remain.
// Matrix receiver by pointer or reference 461/461; direct rotation member 126;
// shared rotation pointer 439; shared point or vector position 395/395; named signed
// plane distance 411; with a shared direction 411. The direction preload and zero
// home remain. Restored the original executable body.
// M3 lane 3 r259 (fast with 0x16c6f0/0x13cb40/0x23d090, all MATCH; control 126): the timer as an if/else statement 152
// (1700 B); the plane length as `length_sq3f(&camera->point.left)` 478 (1720 B); the three world-basis stores of cases 1
// and 2 through one shared inline helper 582 (1576 B); the plane length through the existing `basis_dot_xzy` (i, k, j)
// 151 at 1696 B (it hoists left.i into XMM1 and sums in XMM0); diagnostic, both plane dots summed j, k, i through a
// local copy of path.cpp's `dot3f_jki` 394 (1700 B; left.k still hoisted). Left as before: the plane length's k preload
// and its XMM0 sum, so every later zero takes XMM1 (retail XMM0).
// The direction-j preload and resulting length/zero allocation remain.
// normal by component assignment 435; basis dot for plane constant 400; basis dot for length 151;
// basis dots for constant and length 399; basis dot for signed distance 189;
// basis dots for all three projection products 412; component assignment and basis dot for length 439;
// basis dots for constant and signed distance 412.
// NOT kept: the basis length changes the preload to i and retains the length and zero registers. The other component and basis forms regress.
// Direction j seeds the length before the basis, before its left store, before
// the normal copy, or before the plane constant, with successive or paired
// sums 431/431, 415/415, 392/392, 392/392. NOT kept: the early j loads
// lengthen the body, and the later forms still preload k. XMM1 remains zero.
// Direction k still loads before the normal copy where retail loads j. Plane after basis stores 126, shared typed
// offset 395, accumulated zero-first scale 454, plane scoped to the factor 126, plane and const length after basis
// stores 126, complete translated-vector local 427, in-place offset copy 470, separate scale and add 409.
// NOT kept: the scope forms tie and the translated vectors enlarge the body.
// Copied normal for final scaled point 427. Conditional plane constant 406, with copied normal 427.
// Named distance and quotient 394. Signed numerator before division 395.
// Timer published after both axes with conditional expression or branch result 378/378,
// or elapsed seconds named on the nonzero path 377. The j preload and zero register still differ.
// Direction j still needs the initial load and XMM1 still holds zero in the point path.
// Tried both point cases share the basis reset before projecting 489, recognized point types share the
// basis setup 489, contiguous point types share the basis setup 487, recognized types reset the basis
// after any position projection 494, project the displacement from the matrix position onto the plane
// normal 454, project the displacement toward the matrix position with the corresponding sign 466,
// explicit origin and offset projections supply the signed factor 412, implicit plane offset gives way to
// a native normal and independent projections 413.
// NOT kept: The shared basis reset removes retail copy instructions. Point-difference forms alter
// arithmetic grouping. The independent projections keep separate dots but still miss the preload and grow
// the body.
// M3 lane 1 r122 (fast with 0x16c6f0 and 0x13cb40, both MATCH; control 126, aligned 92; diagnostic, body unchanged):
// a phase scan of 1 to 33 chained dead locals at function scope, and 1 to 8 in the plane block, has period 8 and
// only two outcomes: 126 (aligned 92, the control's code) at 7, 15, 16, 23, 24, 31, 32 (plane block: 7) and
// 394-395 (aligned 98-101) everywhere else; no phase preloads left.j, and the 394 forms also rotate the normal to
// XMM5/4/3. So the plane length's first-loaded operand does not follow the symbol count on this body. Read again in
// the listing: both plane dots add the k product into the j product; retail loads the length's j operand ahead of
// the normal's copy (off the camera pointer) and its k operand at the product (off the copied address), ours the
// reverse, and that alone mirrors XMM0/XMM1 for the length, t, the timer and every later zero; retail's distance
// dot adds j into k and loads forward for k and j but position for i, where ours loads position for all three.
// Forms on the body (fast, control 126): the inner switch with case 1 written before case 2 126 (same code); the plane
// distance taken against the camera vector before the normal is copied 172 [aligned 96]; the point state through a
// `s_camera_point_state const &` for the case-2 block 395 (1700 bytes); the field-of-view's `active = false` before
// the default field-of-view read 126 (same code); the matrix position through a `vector3f *` local for plane.d and
// the scaled add 395 [106]. Body unchanged.
// M3 lane 7 r187 (fast with 0x142d10, 0x16c2b0, 0x16c6f0, 0x16be50, 0x23d090, all MATCH; plain control 126 = fast). Tried:
// 0x142d10's parameters as (out, forward, up) at its definition, declaration and call (retail sets up ECX up, EDX forward,
// EAX out in that order) 124 (re-measured, the pan arm's two `lea`s); that with `0.0f != length_squared` 123, and with the
// timer as `0.0f != time_scale ?` too 123 (gains under 4, not kept); a pointer local to camera->point.left used for the
// plane copy, the length dot and the add-scaled call 393 (1700 B: the length then preloads left.j into XMM1 as retail,
// but the first dot's products and the copy's registers turn round); the pointer at the length dot and add-scaled only 393,
// at the copy and add-scaled only 395, at the length dot only 394. Diagnostic (not kept): one more `unknowna4 = 0.0f`
// store at entry 459 (the zero is then held in XMM1 through the function).
// M3 lane 10 r126 (fast with every callee, which reproduces the full image's 126; the reduced image without them scores 275,
// so the permuter declines this body): the pan quaternion declared before `t` 126 (same code); the pan rotation into a
// matrix3x3 local in place of the transform 394 (1700 B); the basis matrix at case scope without its block 219 (1708 B);
// the pan fraction after the camera reload 126 (same code); `command->distance` stored before offset.i 127; offset.k
// computed and stored before offset.j 125 (one point, not kept).
// Direct squared-length condition, field-of-view fallback arm, mutable target through blend,
// ordered fraction clamp, staged angle accumulation, named angular delta, branch-local command
// field stores, and a role-named projection scale: 126/141/390/126/126/395/154/126.
// NOT kept: The direction-k preload and length/zero register split remain.
// Normal fields in ikj/jik/jki/kij/kji order 435/435/436/435/436, direct indexed
// coordinates 435, ascending/descending copies 441/441. NOT kept: the field forms
// remove the native dword normal copy, and all shorten to 1668 bytes. The length
// and persistent zero registers remain reversed.
// M3 lane 12 r152 (fast with 0x16c6f0/0x13cb40/0x142d10, all MATCH; control 126): redundant parentheses (no grouping
// change) around the field-of-view lerp product 1-3 pairs 126/126/126, around the case-2 division 126/126/147, around the
// timer division 126/126/147; the plane length written out j, k, i without dot3f 397 (1700 B: left.j hoisted as retail, but
// the normal's i/k registers swap and left.k is read off the camera pointer), with 1-3 pairs around its first product
// 398/398/392 or its leading pair 397/397/391; written out i, j, k 397, with the same pairs 398/398/392 and 397/397/393.
// direct vertical output 394, planar point inputs 394, planar scalar inputs 394, products after distance 392,
// products around distance 392, products before distance 392, sine terms before cosine 395, planar point result
// 394.
// NOT kept: All forms add four bytes. The direction preload and length and zero registers still differ.
// M3 lane 15 r113 (fast with all eleven callees and 0x23d090; control 126): the yaw distance dot with its operands
// swapped 126 (every product then loads the forward first; retail loads the position first for i only); the animation
// arm's two zero stores chained 126; the field-of-view test as `target != 0.0f` 149 (1700 B: the target is loaded for
// the test as retail, but the reached arm reloads it where retail copies the register). Leads, not kept (gains under 8):
// the inlined quaternion normalise sums w, k, j, i in the shared helper's source order in retail, where this build sums
// j, k, w, i; the helper's terms in natural i, j, k, w order (or j, k, w, i) give retail's order 124, and the shared
// header in natural order keeps 0xa9b40 matched (fast); 0x142d10's parameters as (out, forward, up), its only caller,
// the order retail's ECX up / EDX forward / EAX out setup implies, 124 (0x142d10 still matched), both together 122.
// The field-of-view tail takes retail's exact shape (target loaded once for the test and copied in the reached arm, the
// interpolation re-reading the field after the clamp, the start read after the clamp) only with a target local at the
// test and reached arm and the field read in the interpolation (a mix) 395 (1700 B), with the local at all three uses
// 396 (no re-read); with the two leads 396, and every xmm0/xmm1 role in the function then swapped against retail
// (zero XMM1 for retail XMM0). Diagnostic phase scan of 1 to 8 dead locals on that combination: 396/395/395/400/400/145/
// 145/396 (145 at 6 and 7: aligned 94 against the control's 103, the zero still XMM1 everywhere).
// Plane aggregate from the vector, components or complete offset 126/435/172.
// Plane distance from zero-offset vector aggregate, component aggregate or separate
// stores 126/435/126. Vector aggregate with scoped length or initial factor 126/126.
// The direction preload and the squared-length and zero registers still differ.
// M3 lane 7 r196 (fast with all eleven callees, 0x13cb40 and 0x23d090; control 126, aligned 88, register-masked 23; body
// unchanged). Read from retail: its yaw block computes `0 - distance` into XMM2 and loads forward i/j/k into XMM3/4/5
// together, multiplies all three by the distance and only then negates them (ours interleaves rows and keeps the
// distance in XMM2); its animation arm stores the shared zero directly where ours copies it to XMM0 first. Tried: one
// variable for the plane length and the scale, with the zero else arm 126 (same registers), without it 378; the
// yaw offset through the existing vector3f_scale and three negations, distance store after or before it 129/129 (the
// helper reads forward through its pointer, [eax+4]/[eax+8], where retail reads the fields directly); three scale rows,
// the distance store, three negations 129 (forward in XMM2/3/4, the distance sum now in XMM0 with temporaries in
// XMM2: retail's yaw block with XMM0/XMM1 exchanged); the distance store first, then the rows and negations 126
// (aligned 86, masked 18; a statement order, same score, not kept); the distance store first with the three direct rows
// 127; a named negated distance ahead of the rows 395 (1700 B, forwarded to its store). Re-measured: a direction
// pointer for the copy, the length dot and the scaled add 393 (1700 B: left.j preloads, but the length then sums
// (i + j) + k). Diagnostics (removed at once): the pan arm's three zeros replaced by another constant 159 (the point
// arm's zero stays XMM1, so the pan arm's XMM0 return value does not pin it); the animation arm's zero stores dropped
// 163, the timer's zero arm dropped 345, the last zero store changed 146, the field-of-view block dropped 296 (zero
// XMM1 in the plane block each time); only the yaw block without its zeros moves it (469: the plane block's zero
// becomes a local XMM6 and the timer's XMM0), so the zero is one register from the plane arm through the yaw block.
// Left-only or up-and-left basis stores after the normal copy 171/203, after the plane constant 177/208,
// after the squared length 177/208, or just before the projected position 189/220. NOT kept: All eight
// raise the score at the retail extent. Direction preload and length/zero register allocation remain.
// Source-direction displacement with direct/in-place/separate/origin-copy addition 409/443/433/474.
// Same four forms using the copied plane normal 461/492/493/539.
// NOT kept: The staged displacement grows the body, and reading the copied normal for the final projection moves the first difference into the prologue.
// Round 611. early direction self-dot 164, early squared magnitude 417, early dot before plane initialization 411,
// plane guarded by self-dot 185, plane guarded by squared magnitude 421, early dot with zero-offset plane 417,
// scoped length and plane constant 406, zero-first offset 407. NOT kept: the early dot loads j into XMM1 but
// computes the length in XMM0 and reorders the normal copy. The guard forms also regress.
// M3 lane 1 r137 (H2M_FAST with 0x142d10 and 0xa9b40, both MATCH; control 126): re-measured 0x142d10 as (out, forward,
// up) 124 and that with the shared quaternion normalise summing i, j, k, w 122 (round 113's leads, a gain of 4, under the
// bar). On that 122: case 2's test as `0.0f != length_squared` 121 (only the ucomiss operand text lines up; the registers
// stay swapped), the timer as `0.0f != time_scale` 122, both 121, the yaw clamp as `0.0f < distance` 122, the animation
// arm's two zero stores chained 122.
// M3 lane 6 r211 (fast with 0x16c6f0/0x13cb40/0x142d10, all MATCH; control 126; a compile-only screen of the plane block
// reproduces its left.k preload and XMM0 length, 22 differing xmm lines): the plane length through a local dot written in
// all six term orders, either grouping (12 forms) 22 each (the sum is canonicalised: same code); the same with the two
// arguments the other way round (12 forms) 33-37, (j, k, i) left-first confirmed 405 (1700 B); the distance numerator as
// `dot3f(&plane.n, offset) - plane.d` 126 (same code), as `dot3f(offset, &plane.n) - plane.d` 394 (1700 B), against the
// direction instead of the normal 37/37 (screen); the plane constant as `dot3f(&plane.n, position)` 22 (same code), against
// the direction either way round 36/36 (screen). No spelling preloads left.j.
// M3 lane 4 r237 (fast with 0x16c6f0, 0x13cb40 and 0x142d10, all MATCH; control 126): the yaw distance dot with its arguments
// the other way round 126; the distance dot ahead of the atan2 126; the field-of-view tail as `start + (target - start) * t`
// 126 (same code each); the tail's clamp into a named fraction local 395 (1700 B); the tail's range in a named local 396.
// M3 lane 8 r208 (h2m fast with 0x16c6f0, 0x13cb40, 0x142d10 and 0xa9b40, all MATCH; control 126): case 2's squared length
// as the copied normal's own dot (`dot3f(&plane.n, &plane.n)`, no left loads) 177; `t` started at zero at its declaration
// with no else arm 454; the length test as `== 0.0f` with the zero arm first 136; t as one ternary 126 (same code); the
// squared length between the normal copy and the plane constant 399 (1700 B).
// MiniMax R (11-20): score 126 (was 126): no new form; the plane block still preloads the normal's z into xmm1 where
// retail preloads its y, so the squared length lands in xmm0 where retail keeps it in xmm1 and the divide and the zero
// copy that follow stay exchanged with it; the shared dot helper's term order is fixed in a header this lane may not
// touch, so no spelling reaches retail's preload.
// m3 lane 6 r221 (H2M fast with 0x16c6f0/0x13cb40/0x142d10, all MATCH; control 126): case 2's squared length accumulated
// through a reference in retail's (j + k) + i order 401 (1700 B); a redundant `t = 0.0f;` ahead of the length test 126;
// `t` initialised at its declaration with the else arm kept 126; the field-of-view target bound by `real const &` 395
// (1700 B); diagnostic: the yaw distance written with position first in i and forward first in j and k 126 (same code:
// the compiler orders each product's operands itself).
// M3 lane 5 r222 (H2M_FAST with 0x142d10 and 0x16c6f0, both MATCH; control 126, aligned 103): the camera point's left vector
// through a `vector3f const &left` reference at the copy, the length dot and the add-scaled call 393 (1700 B, aligned 120):
// the first form here to hoist retail's left.j (`movss xmm1, [esi + 0x54]`), but the normal then takes XMM5/3/4 for i/j/k
// and the length sums j, i, k; the reference at the dot only 394, the copy only 395, copy and dot 394, dot and add-scaled
// 393, copy and add-scaled 395, add-scaled only 395 (all 1700 B, the k preload back in the last three); the left copy moved
// ahead of the three matrix copies 186; `t` as one conditional expression 126 (same code).
// Plane-to-origin numerator direct, named, accumulated or joined 389/382/389/389.
// Negated residual before division direct, named, compound or joined 135/395/135/135.
// NOT kept: the numerator alternatives change the rounding boundary, raise the score
// and leave the preload and persistent zero unresolved.
// MiniMax C (11-20): no new form, no lower score (126): the length agrees (1696 bytes), but the plane length's dot
// accumulates in the other register order - this build keeps the third component's product in xmm1 and the sum in xmm0,
// retail keeps the product in xmm0 and the sum in xmm1 - while the offset's own terms agree with retail's
// (sine*offset.i - cosine*offset.j); the term order inside the dot is canonicalised by the compiler, so no spelling
// of it moves the block.
// M3 lane 1 r147 (fast with 0x16c6f0, 0x13cb40, 0x142d10 and 0xa9b40, all MATCH; control 126): the yaw and the distance dot
// reading the camera's own forward (the copy's source) 332 (1700 B); only the distance dot reading it 332 (1700 B); that dot
// as (position, camera forward) 332 (1700 B); only the yaw reading it 331 (1700 B); the origin store moved to the top of the
// yaw block 136.
// M3 lane 14 r138 (H2M_FAST with 0x142d10, 0x16c6f0 and 0x13cb40, all MATCH; control 126). Reading: in the plane block the
// two sums are the same in both builds ((j + k) + i, the j term the accumulator); only the length load hoisted into the
// plane-constant dot differs (retail left.j into XMM1, ours left.k), which sets the accumulator and so the zero register.
// Tried: the squared length declared at its assignment 126; the plane declared after the length and t 126; the length
// test as `if (length_squared)` 126 (each the same code); the normal copied ahead of the three basis copies re-measured
// 186. Diagnostics (names only, reverted): the length renamed twice and the plane renamed 126 each (no tie on names).
// Round 627. Shared yaw negation after product assignment, aggregate, or
// component stores 397/396/396; in-place assignment or stores 141/131;
// scoped aggregate 396; distance between products and negation 397, or
// before in-place assignment 139. NOT kept: the projection preload and
// length/zero homes still differ, and all forms raise the score.
// M3 lane 2 r131 (H2M_FAST with 0x16c6f0/0x13cb40/0x142d10/0x23d090/0xa9b40, all MATCH; control 126): three listed leads
// together (0x142d10 as (out, forward, up) at its definition, declaration and call; the shared quaternion helper's length
// summed i, j, k, w; case 2's test as `0.0f != length_squared`) 121 (aligned 98, masked 33; under the bar). On 121: 1-7 dead
// locals (diagnostic) 395/394/394/394/394/394/121; the yaw distance dot's arguments swapped 121; the distance store ahead
// of the first offset row 122; the timer as `0.0f != time_scale` 121; the field-of-view test as `target != 0.0f` 144 (1700
// B). Reading on 121: the animation arm's zero copy (`movaps xmm0, xmm1`, 3 bytes) moves the field-of-view tail, so 14 jumps
// to it differ only in their target; the copy follows from the zero's register (XMM1 here, XMM0 in retail), which the
// plane length sets. Body unchanged.
// Copied-normal address after or before the copy 148/148, write through the normal address 126.
// Shared live direction on those forms 393/394/378. Dot-local normal address with original or shared
// live direction 148/393. NOT kept: The forms with a shared live direction can recover the j preload,
// but keep zero in XMM1 and change the copied normal's registers and product schedule.
// The 0x48 frame agrees. The projection still preloads direction k instead of j and swaps the length and zero XMM homes.
// Copied normal through displacement 427; component setter 485, aggregate normal 485, representation copy 427.
// Snapshot squared length 435; j capture and setter with that length 485; indexed setter 485; snapshot self-dot 435.
// NOT kept: extending the copied normal changes the entry and displaces the original direction load. The component and aggregate forms move the body away from retail length.
// M3 lane 1 r155 (H2M_FAST with 0x16c6f0, 0x13cb40, 0x142d10 and 0x16c740, all MATCH; control 126). Lead: the field-of-view
// tail is the computation of matched 0x16c740's file-local field_of_view_current (target tested first, start read after
// the clamp); written that way (`if (function_13cb40() && target != 0.0f)`, start after the PIN) the tail is retail's
// instruction for instruction but for the zero register, 149 (1700 B: the animation arm's zero copy remains); through that
// helper itself (declared here, LTCG inlines it) 399 (1704 B; case 2's normal registers move). On the 149 form: the
// animation arm's zero stores chained 149, the timer test as `0.0f != time_scale` 149 (same code each). The squared length
// at each use 441 (1728 B). Diagnostics (reverted): the end stores moved to the top 496 (one zero register for the whole
// body, still XMM1); every pan-arm zero removed 321 and case 2's zero tests removed 437 (the other blocks keep the value in
// XMM0, the zero in XMM1 or XMM2): each block gives XMM0 to the value where retail gives it to the zero.
// M3 lane 16 r175 (H2M_FAST with 0x142d10; control 126): retail sets up 0x142d10's arguments ECX (forward), EDX (up), EAX
// (out), ours EDX first. 0x142d10 as (out, forward, up) at its definition, declaration and call 124 (0x142d10 MATCH; the
// callee matches in either order, so not kept); (forward, up, out) and (forward, out, up) 124 with 0x142d10 at 2; (out, up,
// forward) and (up, out, forward) 126 (same code); `switch (camera->mode)` with no mode local 126 (the yaw dot's i product
// swaps operands), with (out, forward, up) 124; `field_of_view` declared at its assignment 126 (same code).
// M3 lane 11 r156 (H2M fast with 0x142d10/0x16c6f0/0x13cb40, all MATCH; control 126, aligned 103, masked 37): case 2
// through one point-state pointer for all its reads 395 (1700 B); the denominator as `dot3f(&left, &plane.n)` (0x26d370's
// ray style) 394 (1700 B); `t = -(distance / length_squared)` 126 (same code); the plane constant dotted with the left
// vector itself 172; the length as `dot3f(&left, &left)` 392 (1700 B); the tail as `if (function_13cb40() && target !=
// 0.0f)` with start read at each use 134 (aligned 102), with the lerp arm first 134; the tail's test as `target != 0.0f`
// alone 149 (1700 B). Reading: the 14 jumps into the tail sit 3 bytes late because of the animation arm's `movaps xmm0,
// xmm1`; retail's success path stores the zero from XMM0 and re-enters the tail at its `xorps xmm0, xmm0`. Body unchanged.
// M3 lane 12 r177 (H2M_FAST with 0x142d10/0x16c6f0/0x13cb40, all MATCH; control 126): the point arm's object_index local
// at its later uses too 328; the pan arm's game time through the game_time local 126 and its transform declared at the
// arm's top 126 (both the same code); the pan arm's rotation matrix without its own scope 219 (1708 B, the frame moves);
// the position blend from the once-pinned t (no second PIN) 239 (1672 B). Body unchanged.
// M3 lane 6 r235 (H2M_FAST with 0x16c6f0/0x13cb40/0x142d10/0x16c740, all MATCH; control 126): the yaw block's rotated
// offset through the shared 2-D dot and cross helpers on a {cosine, sine} pair, offset first 368 (frame move, first +0x2),
// the pair first with the cross negated 390 (1704 B); the yaw block's offset as three scalar locals instead of the
// vector3f 394 (1700 B: the scalars are not the same code as the vector's members); the distance dot ahead of the yaw
// 126 and the origin store ahead of the sine 126 (same code each). Reading: the tail keeps the target in a register for
// both the zero test and `current = target` in retail (0x16c740's helper shape), and every block's zero is XMM0 there.
// Body unchanged.
// M3 lane 3 r303 (H2M_FAST with 0x16c6f0/0x13cb40/0x142d10/0x16c740, all MATCH; control 126, aligned 103). Diagnostic
// dead-local scan at the function top, K = 0..40: an 8-period, 126 at K = 0 and 7 (mod 8) and 394 or 395 at every other
// count (the same at an anchor inside the yaw block), so the kept body sits in the good window and a form that adds or
// removes a counted symbol reads 39x until the count is put back. Forms re-measured with the count compensated (best of
// K = 0..7, the K in brackets): the squared length as length_sq3f(&plane.n) 177 [2, 3]; as
// length_sq3f(&camera->point.left) 478 [0, 7]; the squared-length dot's arguments swapped 126 [3] (the kept code: the
// swap only moves the window); the yaw distance dot's arguments swapped 126 [0, 7] (same code); the camera point's left
// through a `vector3f const &` at the copy, the dot and the add-scaled call, or at the dot and the add-scaled call only,
// 126 [6] each (the kept code again: the listed 393 with retail's left.j preload is the out-of-window state, whose plane
// block is otherwise further off); the yaw block's offset through the existing vector3f_scale and vector3f_negate_ab
// with the distance store between them or ahead of them 141 [0, 7] (aligned 124: the forward is then read through one
// pointer where retail reads i through the copy's pointer and j, k from the command), with the store after them 140
// (aligned 130). Reading of retail's yaw block: the forward's three components are loaded first, all three are scaled
// by the distance (the negated distance stored after the first product), then the three are subtracted from zero in
// i, j, k order; every block takes its zero in XMM0 before the block's own value. Body unchanged.
// M3 lane 16 r184 (H2M_FAST with 0x16c6f0, 0x13cb40, 0x142d10, 0x16c740 and, for the helper change, 0xa9b40, all MATCH
// each run; control 126, aligned 107, masked 39). LEAD at 8 (aligned 15, every difference in one sum), not kept: its main
// steps are statement moves, which stay only with a byte match; the whole diff is build/r184/lead-16c840.patch.
// Reading: the zero constant is one register candidate for the whole body. The allocator ranks it 104, second to the
// yaw block's offset.i (112), which is inflated because this body multiplies offset.i by the sine in place; so offset.i
// takes XMM0 and the zero XMM1 in every block. Retail multiplies in the sine's register. Operands of a float product
// are ordered by when each was last assigned, the later one first, so the sine has to be assigned after the offset rows.
// The lead, step by step (each on top of the ones before): the sine assigned just ahead of the cosine 85 (the zero now
// ranks first and takes XMM0 everywhere); the tail as `if (function_13cb40() && target != 0.0f)` with `start` read after
// the PIN, the shape of 0x16c740's helper, 38; the yaw distance dot's arguments exchanged 36; the animation arm's flags
// store after its two zero stores 33; `switch (camera->mode)` with no mode local 31; `command->offset.k = offset.k`
// after the two rotation rows 24; `command->distance = 0.0f - distance` ahead of the offset rows 12 (the three forward
// loads and products then come first as in retail); 0x142d10 as (out, forward, up) at its definition, declaration and
// its only call 10 (retail sets up ECX up, EDX forward, EAX out; 0x142d10 matches either way); the shared
// quaternion_unit_or_identity summing i, j, k, w 8 (0xa9b40, its other user, still matches).
// Left at 8: `cosine * offset.i + sine * offset.j` is evaluated sine product first here, cosine product first in
// retail, which also exchanges the sine's and cosine's stack cells and XMM6/XMM7. Measured rule: the product whose
// offset was assigned later goes first (rows in j, i, k order give retail's sum, 10, but exchange the i and j
// registers; `cosine * offset.k + sine * offset.j` and `cosine * offset.i + sine * offset.k` follow the same rule).
// On the 12 form (before the last two steps), none lower: the sum's terms or factors in any written order 12 (same
// code); the cosine declared ahead of the sine, the offset declared first, sine and cosine declared at their
// assignments 12 (same code); the sine assigned after the i row 11 (retail's sum order and cells, but `offset.j * sine`
// copies the offset), after the j row or with the k row between sine and cosine 12, ahead of all rows 151, cosine
// assigned first 14; the rotation rows exchanged 18; the origin store after the rows 259, after the rotation 267,
// between the distance store and the rows 33; the object index store ahead of the rotation 22; the offset.k store
// between the rotation rows 12, ahead of them 32; the rows as `distance * forward` 12; the yaw as a double 13; the
// offset as a point3f 12; the rows through vector3f_scale and vector3f_negate_ab on the offset 168, through a second
// vector 169, as product rows with vector3f_negate_ab in place 39, as product rows with the negation written inside
// the rotation rows 160 to 164 (276 with unary minus), through vector3f_assign 152 and 167 (in each of these the
// offsets outrank the zero again). The symbol count has a period of 8 with a window two wide: with the mode local
// the yaw dot's i product has its factors exchanged, any other count costs the plane block (38x). Body unchanged.
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
		if (0.0f != camera->field_of_view_target)
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
